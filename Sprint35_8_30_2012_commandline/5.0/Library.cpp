
#include "stdafx.h"
#include <math.h>
#include "CCEtoODB.h"
#include "library.h"
#include "graph.h"
#include "cedit.h"
#include "attrib.h"
#include "EntityNumber.h"
#include "DcaEnumIterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMultiDocTemplate* pDocTemplate; // from CAMCAD.CPP
//extern LayerTypeInfoStruct layertypes[]; // from DBUTIL.CPP

static int ImportGeometry(CCEtoODBDoc *doc, CCEtoODBDoc *libDoc, FileStruct *file, BlockStruct *geom);
static void LibCopyAttribs(CAttributes* origMap, CCEtoODBDoc *libDoc, CAttributes** newMap, CCEtoODBDoc *doc);
static void FillKeywordMap(CCEtoODBDoc *libDoc, CAttributes* attribMap);
static void FillLayerMap(CCEtoODBDoc *libDoc, CCEtoODBDoc *doc, int layer);

static CMapStringToString *keywordMap;
static CMapPtrToPtr *layerMap;

/******************************************************************************
* OnImportGeometries
*/
void CCEtoODBDoc::OnImportGeometries() 
{
   // Select Library Document
   SelectDocument sdDlg;
   sdDlg.doc = this;
   if (sdDlg.DoModal() != IDOK)
      return;
   

   // Select Geometries
   SelectGeometries sgDlg;
   sgDlg.doc = sdDlg.libDoc;
   if (sgDlg.DoModal() != IDOK)
      return;

   // Add Dependent Blocks
   POSITION geomPos = sgDlg.geomsList.GetHeadPosition();
   while (geomPos != NULL)
   {
      BlockStruct *geom = (BlockStruct*)sgDlg.geomsList.GetNext(geomPos);

      POSITION dataPos = geom->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = geom->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)
         {
            BlockStruct *subblock = sdDlg.libDoc->getBlockAt(data->getInsert()->getBlockNumber());
            if (!(subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE || subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL))
            {
               if (!sgDlg.geomsList.Find(subblock))
               {
                  sgDlg.geomsList.AddTail(subblock);
               }
            }
         }
      }
   }

   // Set Up Keyword Map
   CMapStringToString localKeywordMap;
   keywordMap = &localKeywordMap;
   geomPos = sgDlg.geomsList.GetHeadPosition();
   while (geomPos != NULL)
   {
      BlockStruct *geom = (BlockStruct*)sgDlg.geomsList.GetNext(geomPos);
      FillKeywordMap(sdDlg.libDoc, geom->getAttributesRef());

      POSITION dataPos = geom->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = geom->getDataList().GetNext(dataPos);
         FillKeywordMap(sdDlg.libDoc, data->getAttributesRef());
      }
   }

   // Map Keywords
   MapKeywords keywordDlg;
   if (keywordDlg.DoModal() != IDOK)
      return;

   // Set Up Layer Map
   CMapPtrToPtr localLayerMap;
   layerMap = &localLayerMap;
   geomPos = sgDlg.geomsList.GetHeadPosition();
   while (geomPos != NULL)
   {
      BlockStruct *geom = (BlockStruct*)sgDlg.geomsList.GetNext(geomPos);

      POSITION dataPos = geom->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = geom->getDataList().GetNext(dataPos);
         FillLayerMap(sdDlg.libDoc, sdDlg.doc, data->getLayerIndex());
         if (data->getLayerIndex() >= 0)
         {
            LayerStruct *layer = sdDlg.libDoc->getLayerArray()[data->getLayerIndex()];
            if (layer->getMirroredLayerIndex())
               FillLayerMap(sdDlg.libDoc, sdDlg.doc, layer->getMirroredLayerIndex());
         }
      }
   }

   // Map Layers
   MapLayersDlg layerDlg;
   layerDlg.doc = this;
   layerDlg.libDoc = sdDlg.libDoc;
   if (layerDlg.DoModal() != IDOK)
      return;

   // fix mirror layers
   void *origVoidPtr, *newVoidPtr;

   POSITION pos = layerMap->GetStartPosition();
   while (pos != NULL)
   {
      layerMap->GetNextAssoc(pos, origVoidPtr, newVoidPtr);

      LayerStruct *origLayer = (LayerStruct*)origVoidPtr, *newLayer = (LayerStruct*)newVoidPtr;

      if (newLayer == NULL)  continue;  // was crashing because it wasn't checking newLayer == NULL - TSR 3705
      if (origLayer->getMirroredLayerIndex() != origLayer->getLayerIndex() && newLayer->getMirroredLayerIndex() == newLayer->getLayerIndex())
      {
         LayerStruct *origMirrorLayer = sdDlg.libDoc->getLayerArray()[origLayer->getMirroredLayerIndex()];
         layerMap->Lookup(origMirrorLayer, newVoidPtr);
         LayerStruct *newMirrorLayer = (LayerStruct*)newVoidPtr;
         newLayer->setMirroredLayerIndex(newMirrorLayer->getLayerIndex());
         newMirrorLayer->setMirroredLayerIndex(newLayer->getLayerIndex());
      }
   }

   // Import Geometries
   FileStruct *file;
   BOOL Found = FALSE;
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      file = getFileList().GetNext(filePos);
      if (file->getBlockType() == blockTypeGeomLibrary)
      {
         Found = TRUE;
         PrepareAddEntity(file);
         break;
      }
   }

   if (!Found)
   {
      StoreDocForImporting();
      file = Graph_File_Start("Local Library", Type_Unknown);
      file->setNotPlacedYet(false);
      file->setBlockType(blockTypeGeomLibrary);
   }

   geomPos = sgDlg.geomsList.GetHeadPosition();
   while (geomPos != NULL)
   {
      BlockStruct *geom = (BlockStruct*)sgDlg.geomsList.GetNext(geomPos);

      ImportGeometry(this, sdDlg.libDoc, file, geom);
   }

   //FillCurrentSettings(this);

   OnReplaceGeometry();
}

/******************************************************************************
* ImportGeometry
*/
int ImportGeometry(CCEtoODBDoc *doc, CCEtoODBDoc *libDoc, FileStruct *file, BlockStruct *geom)
{
   CCamCadData& camCadData = doc->getCamCadData();

   BlockStruct *newGeom;

   newGeom = Graph_Block_On(GBO_OVERWRITE, geom->getName(), file->getFileNumber(), 0);
   newGeom->setBlockType(geom->getBlockType());
   
   POSITION dataPos = geom->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = geom->getDataList().GetNext(dataPos);
      DataStruct *newData;

      void *voidPtr;
      if (data->getLayerIndex() >= 0)
      {
         if (!layerMap->Lookup(libDoc->getLayerArray()[data->getLayerIndex()], voidPtr))
         {
            LayerStruct *layer = libDoc->getLayerArray()[data->getLayerIndex()];
            int i=0;
            continue;
         }
         if (!voidPtr)
            continue;
      }

      if (data->getDataType() != T_INSERT)
      {
         newData = camCadData.getNewDataStruct(*data);
         newGeom->getDataList().AddTail(newData);


         // fix widthIndex
         if (data->getDataType() == T_POLY)
         {
            POSITION polyPos = newData->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = newData->getPolyList()->GetNext(polyPos);

               BlockStruct *width = libDoc->getWidthTable()[poly->getWidthIndex()];

               poly->setWidthIndex(Graph_Aperture(width->getName(), width->getShape(), width->getSizeA(), width->getSizeB(), 
                     width->getXoffset(), width->getYoffset(), width->getRotation(), width->getDcode(), width->getFlags(), TRUE, NULL)); 
            }
         }

         if (data->getDataType() == T_TEXT)
         {
            BlockStruct *width = libDoc->getWidthTable()[newData->getText()->getPenWidthIndex()];

            newData->getText()->setPenWidthIndex(Graph_Aperture(width->getName(), width->getShape(), width->getSizeA(), width->getSizeB(), 
                  width->getXoffset(), width->getYoffset(), width->getRotation(), width->getDcode(), width->getFlags(), TRUE, NULL)); 
         }
      }
      else // T_INSERT
      {
         BlockStruct *subblock = libDoc->getBlockAt(data->getInsert()->getBlockNumber());
         int blockNum;

         if (subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE)
         {
            if (subblock->getShape() == T_COMPLEX)
            {
               blockNum = ImportGeometry(doc, libDoc, file, libDoc->getBlockAt((int)(subblock->getSizeA())));
            }
            else
            {
               int result = TRUE;
               CString name = subblock->getName();
               int widthIndex;
               while (result)
               {
                  widthIndex = Graph_Aperture(name, subblock->getShape(), subblock->getSizeA(), subblock->getSizeB(),
                     subblock->getXoffset(), subblock->getYoffset(), subblock->getRotation(), subblock->getDcode(), subblock->getFlags(), 
                     FALSE, &result);
                  name += "*";
               }

               blockNum = doc->getWidthTable()[widthIndex]->getBlockNumber();
            }
         }
         else if (subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL)
         {
            blockNum = Graph_Tool(subblock->getName(), subblock->getTcode(), subblock->getToolSize(), subblock->getToolType(), subblock->getToolBlockNumber(), 
                     subblock->getToolDisplay(), subblock->getFlags());
         }
         else
         {
            blockNum = ImportGeometry(doc, libDoc, file, libDoc->getBlockAt(data->getInsert()->getBlockNumber()));
         }

         newData = doc->getCamCadData().getNewDataStruct(*data,false);
         newData->getInsert()->setBlockNumber(blockNum);
         newGeom->getDataList().AddTail(newData);
      }

      if (data->getLayerIndex() >= 0)
         newData->setLayerIndex(((LayerStruct*)voidPtr)->getLayerIndex());

      LibCopyAttribs(data->getAttributesRef(), libDoc, &newData->getAttributesRef(), doc);
   }

   Graph_Block_Off();

   LibCopyAttribs(geom->getAttributesRef(), libDoc, &newGeom->getAttributesRef(), doc);

   return newGeom->getBlockNumber();
}

/******************************************************************************
* LibCopyAttribs
*/
void LibCopyAttribs(CAttributes* origMap, CCEtoODBDoc *libDoc, CAttributes** newMap, CCEtoODBDoc *doc)
{
   if (!origMap)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = origMap->GetStartPosition();pos != NULL;)
   {
      origMap->GetNextAssoc(pos, keyword, attrib);

      CString buf;

      switch (attrib->getValueType())
      {
      case VT_NONE:
         buf = "";
         break;
      case VT_STRING:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         buf = attrib->getStringValue();
         break;
      case VT_INTEGER:
         buf.Format("%d", attrib->getIntValue());
         break;
      case VT_DOUBLE:
      case VT_UNIT_DOUBLE:
         buf.Format("%lf", attrib->getDoubleValue());
         break;
      }

      CString newKW;
      keywordMap->Lookup(libDoc->getKeyWordArray()[keyword]->cc, newKW);

      doc->SetUnknownVisAttrib(newMap, newKW, 
            buf, attrib->getX(), attrib->getY(), attrib->getRotationRadians(), attrib->getHeight(), 
            attrib->getWidth(), attrib->isProportionallySpaced(), attrib->getPenWidthIndex(), 
            attrib->isVisible(), SA_OVERWRITE, attrib->getFlags(), attrib->getLayerIndex(), attrib->getMirrorDisabled(),
            attrib->getHorizontalPosition(), attrib->getVerticalPosition());
   }
}

/******************************************************************************
* FillKeywordMap
*/
void FillKeywordMap(CCEtoODBDoc *libDoc, CAttributes* attribMap)
{
   if (!attribMap)
      return;

   for (POSITION pos = attribMap->GetStartPosition();pos != NULL;)
   {
      WORD keyword;
      Attrib* attrib;

      attribMap->GetNextAssoc(pos, keyword, attrib);

      CString kw, buf;
      kw = libDoc->getKeyWordArray()[keyword]->cc;

      if (!keywordMap->Lookup(kw, buf))
         keywordMap->SetAt(kw, kw);
   }
}

/******************************************************************************
* FillLayerMap
*/
void FillLayerMap(CCEtoODBDoc *libDoc, CCEtoODBDoc *doc, int layer)
{
   if (layer < 0)
      return;

   LayerStruct *origLayer = libDoc->getLayerArray()[layer];

   void *voidPtr;
   if (!layerMap->Lookup(origLayer, voidPtr))
   {
      // look for same name and type
		int i=0;
      for (i=0; i<doc->getMaxLayerIndex(); i++)
      {
         LayerStruct *newLayer = doc->getLayerArray()[i];
         if (newLayer == NULL)   continue;
         if (!newLayer->getName().Compare(origLayer->getName()) && newLayer->getLayerType() == origLayer->getLayerType())
         {
            layerMap->SetAt(origLayer, newLayer);
            return;
         }
      }

      // look for same type
      for (i=0; i<doc->getMaxLayerIndex(); i++)
      {
         LayerStruct *newLayer = doc->getLayerArray()[i];
         if (newLayer == NULL)   continue;

         if (newLayer->getLayerType() == origLayer->getLayerType())
         {
            layerMap->SetAt(origLayer, newLayer);
            return;
         }
      }

      layerMap->SetAt(origLayer, NULL);
   }
}

/******************************************************************************
* OnReplaceGeometry
*/
void CCEtoODBDoc::OnReplaceGeometry() 
{
   ReplaceGeometry dlg;
   dlg.doc = this;
   if (dlg.DoModal() != IDOK)
      return;

}

/////////////////////////////////////////////////////////////////////////////
// SelectDocument dialog
SelectDocument::SelectDocument(CWnd* pParent /*=NULL*/)
   : CDialog(SelectDocument::IDD, pParent)
{
   //{{AFX_DATA_INIT(SelectDocument)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void SelectDocument::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectDocument)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectDocument, CDialog)
   //{{AFX_MSG_MAP(SelectDocument)
   ON_LBN_DBLCLK(IDC_LIST1, OnOK)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SelectDocument::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      CDocument *document = pDocTemplate->GetNextDoc(pos);
      if (document != doc)
         m_list.SetItemDataPtr(m_list.AddString(document->GetTitle()), document);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectDocument::OnOK() 
{
   int index = m_list.GetCurSel();
   if (index < 0)
      return;

   libDoc = (CCEtoODBDoc*)m_list.GetItemDataPtr(index);
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// SelectGeometries dialog
SelectGeometries::SelectGeometries(CWnd* pParent /*=NULL*/)
   : CDialog(SelectGeometries::IDD, pParent)
{
   //{{AFX_DATA_INIT(SelectGeometries)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void SelectGeometries::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectGeometries)
   DDX_Control(pDX, IDC_TYPE_LB, m_typesLB);
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectGeometries, CDialog)
   //{{AFX_MSG_MAP(SelectGeometries)
   ON_BN_CLICKED(IDC_ALL, OnSelectAll)
   ON_BN_CLICKED(IDC_NONE, OnUnselectAll)
   ON_BN_CLICKED(IDC_SELECT_BY_TYPE, OnSelectByType)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SelectGeometries::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CWaitCursor wait;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getFlags() & BL_FILE || block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE || 
            block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)
         continue;

      m_list.SetItemDataPtr(m_list.AddString(block->getName()), block);
   }

   for (EnumIterator(BlockTypeTag,blockTypeTagIterator);blockTypeTagIterator.hasNext();)
   {
      BlockTypeTag blockType = blockTypeTagIterator.getNext();

      m_typesLB.SetItemData(m_typesLB.AddString(blockTypeToDisplayString(blockType)),blockType);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectGeometries::OnSelectAll() 
{
   int count = m_list.GetCount();
   for (int i=0; i<count; i++)
      m_list.SetSel(i); 
}

void SelectGeometries::OnSelectByType() 
{
   int sel = m_typesLB.GetCurSel();
   if (sel == LB_ERR)
   {
      ErrorMessage("You must select a type.", "Error");
      return;
   }

   int type = m_typesLB.GetItemData(sel);

   int count = m_list.GetCount();
   for (int i=0; i<count; i++)
   {
      BlockStruct *block = (BlockStruct*)m_list.GetItemDataPtr(i);
      m_list.SetSel(i, block->getBlockType() == type);  
   }
}

void SelectGeometries::OnUnselectAll() 
{
   int count = m_list.GetCount();
   for (int i=0; i<count; i++)
      m_list.SetSel(i, FALSE);   
}

void SelectGeometries::OnOK() 
{
   count = m_list.GetSelCount();
   if (!count)
      return;

   int *array = (int*)calloc(count, sizeof(int));
   m_list.GetSelItems(count, array);

   for (int i=0; i<count; i++)
   {
      geomsList.AddTail(m_list.GetItemDataPtr(array[i]));
   }

   free(array);   

   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// ReplaceGeometry dialog
ReplaceGeometry::ReplaceGeometry(CWnd* pParent /*=NULL*/)
   : CDialog(ReplaceGeometry::IDD, pParent)
{
   //{{AFX_DATA_INIT(ReplaceGeometry)
   m_allGeometries = FALSE;
   m_attribs = 2;
   m_collisions = 0;
   m_x = _T("0");
   m_y = _T("0");
   m_angle = _T("0");
   //}}AFX_DATA_INIT
}

void ReplaceGeometry::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ReplaceGeometry)
   DDX_Control(pDX, IDC_WITH, m_with);
   DDX_Control(pDX, IDC_REPLACE, m_replace);
   DDX_Check(pDX, IDC_ALL_GEOMETRIES, m_allGeometries);
   DDX_Radio(pDX, IDC_ATTRIBS, m_attribs);
   DDX_Radio(pDX, IDC_COLLISIONS, m_collisions);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   //}}AFX_DATA_MAP
}

ReplaceGeometry::~ReplaceGeometry()
{
   delete m_imageList;  
}

BEGIN_MESSAGE_MAP(ReplaceGeometry, CDialog)
   //{{AFX_MSG_MAP(ReplaceGeometry)
   ON_BN_CLICKED(IDC_REPLACE_GEOM, OnReplaceGeom)
   ON_BN_CLICKED(IDC_ALL_GEOMETRIES, OnAllGeometries)
   ON_BN_CLICKED(IDC_AUTO, OnAutoReplace)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define iFolder      0
#define iBlock       1
#define iFile        2  
#define iCompBlk     3
#define iPadStk      4
#define iPad         5
#define iCompLocal   6
BOOL ReplaceGeometry::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 7, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_BLOCK));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_COMPBLK));
   m_imageList->Add(app->LoadIcon(IDI_PADSTK));
   m_imageList->Add(app->LoadIcon(IDI_PAD));
   m_imageList->Add(app->LoadIcon(IDI_COMPLOCAL));
   m_replace.SetImageList(m_imageList, TVSIL_NORMAL);
   m_with.SetImageList(m_imageList, TVSIL_NORMAL);

   FillTree();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ReplaceGeometry::OnAllGeometries() 
{
   UpdateData();
   FillTree(); 
}

void ReplaceGeometry::FillTree()
{
   m_replace.DeleteAllItems();
   m_with.DeleteAllItems();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      BOOL LibFile = file->getBlockType() == blockTypeGeomLibrary;

      HTREEITEM replaceRoot = NULL, withRoot = NULL;
      int icon;

      if (LibFile)
         icon = iFolder;
      else
         icon = iFile;

      if (!LibFile || m_allGeometries)
      {
         replaceRoot = m_replace.InsertItem(file->getName(), icon, icon);
         m_replace.SetItemData(replaceRoot, NULL);
      }

      if (LibFile || m_allGeometries)
      {
         withRoot = m_with.InsertItem(file->getName(), icon, icon);
         m_with.SetItemData(withRoot, NULL);
      }

      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);
         if (block == NULL)      continue;
         InsertBlock(block, file->getFileNumber(), replaceRoot, withRoot);
      }

      if (LibFile && !m_allGeometries)
         m_with.Expand(withRoot, TVE_EXPAND);
   }

   HTREEITEM globalReplaceRoot, globalWithRoot;
   globalReplaceRoot = m_replace.InsertItem("Global", iFolder, iFolder);
   m_replace.SetItemData(globalReplaceRoot, NULL);

   if (m_allGeometries)
   {
      globalWithRoot = m_with.InsertItem("Global", iFolder, iFolder);
      m_with.SetItemData(globalWithRoot, NULL);
   }
   else
      globalWithRoot = NULL;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      InsertBlock(block, -1, globalReplaceRoot, globalWithRoot);
   }
}

void ReplaceGeometry::InsertBlock(BlockStruct *block, int filenum, HTREEITEM replaceRoot, HTREEITEM withRoot) 
{
   if (block->getFlags() & BL_FILE || block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE || 
         block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL) 
      return;

   int icon; 
   HTREEITEM item;

   if (block->getFileNumber() == filenum)
   {
      switch (block->getBlockType())
      {
      case BLOCKTYPE_PCBCOMPONENT:
         icon = iCompBlk;
         break;
      case BLOCKTYPE_PADSTACK:
         icon = iPadStk;
         break;
      case BLOCKTYPE_PADSHAPE:
         icon = iPad;
         break;
      case BLOCKTYPE_LOCALPCBCOMP:
         icon = iCompLocal;
         break;
      default:
         icon = iBlock;
         break;
      };

      if (replaceRoot)
      {
         item = m_replace.InsertItem(block->getName(), icon, icon, replaceRoot, TVI_SORT);
         m_replace.SetItemData(item, (DWORD)block);
      }

      if (withRoot)
      {
         item = m_with.InsertItem(block->getName(), icon, icon, withRoot, TVI_SORT);
         m_with.SetItemData(item, (DWORD)block);
      }
   }
}

void ReplaceGeometry::OnReplaceGeom() 
{
   UpdateData();

   HTREEITEM item;
   
   if ((item = m_replace.GetSelectedItem()) == NULL) return; // nothing selected
   replace = (BlockStruct*)m_replace.GetItemData(item); 
   if (replace == NULL) return; 
   
   if ((item = m_with.GetSelectedItem()) == NULL) return; // nothing selected
   with = (BlockStruct*)m_with.GetItemData(item); 
   if (with == NULL) return; 

   ReplaceGeom();

   doc->UpdateAllViews(NULL);
}
   
void ReplaceGeometry::ReplaceGeom() 
{
   CCamCadData& camCadData = doc->getCamCadData();

   // remove old datas
   POSITION pos = replace->getDataList().GetHeadPosition();      
   while (pos != NULL)
   {
      DataStruct *data = replace->getDataList().GetNext(pos);
      RemoveOneEntityFromDataList(doc, &replace->getDataList(), data);
   }
   replace->getDataList().RemoveAll();


   // add new datas
   pos = with->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = with->getDataList().GetNext(pos);
      DataStruct *newData = camCadData.getNewDataStruct(*data);
      replace->getDataList().AddTail(newData);
   }  


   // geometry attributes
   int method;
   switch (m_collisions)
   {
   case 0:
      method = SA_OVERWRITE;
      break;
   case 1:
      method = SA_APPEND;
      break;
   case 2:
      method = SA_RETURN;
      break;
   };

   switch (m_attribs)
   {
   case 0: // keep new attribs only
      replace->getAttributesRef()->empty();
      doc->CopyAttribs(&replace->getAttributesRef(), with->getAttributesRef());
      break;

   case 1: // keep old attribs only
      break;

   case 2: // merge
      {
         if (!with->getAttributesRef())
            break;

         WORD keyword;
         Attrib* attrib;

         for (POSITION pos = with->getAttributesRef()->GetStartPosition();pos != NULL;)
         {
            with->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

            CString buf;

            switch (attrib->getValueType())
            {
            case VT_NONE:
               buf = "";
               break;
            case VT_STRING:
            case VT_EMAIL_ADDRESS:
            case VT_WEB_ADDRESS:
               buf = attrib->getStringValue();
               break;
            case VT_INTEGER:
               buf.Format("%d", attrib->getIntValue());
               break;
            case VT_DOUBLE:
            case VT_UNIT_DOUBLE:
               buf.Format("%lf", attrib->getDoubleValue());
               break;
            }

            doc->SetUnknownVisAttrib(&replace->getAttributesRef(), doc->getKeyWordArray()[keyword]->cc, 
                  buf, attrib->getX(), attrib->getY(), attrib->getRotationRadians(), attrib->getHeight(), 
                  attrib->getWidth(), attrib->isProportionallySpaced(), attrib->getPenWidthIndex(), 
                  attrib->isVisible(), method, attrib->getFlags(), attrib->getLayerIndex(), attrib->getMirrorDisabled(), 
                  attrib->getHorizontalPosition(), attrib->getVerticalPosition());

            break;
         }

         break;
      }
   }

   // set "Derived From" attribute
   doc->SetUnknownAttrib(&replace->getAttributesRef(), ATT_DERIVED, with->getName(), SA_OVERWRITE, NULL);

   // fix inserts
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)   continue;

      if (block->getBlockType() != BLOCKTYPE_PCB)
         continue;

      for (POSITION pos = block->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(pos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (data->getInsert()->getBlockNumber() == replace->getBlockNumber())
         {
            data->getInsert()->incOriginX(atof(m_x));
            data->getInsert()->incOriginY(atof(m_y));
            data->getInsert()->incAngle(DegToRad(atof(m_angle)));
         }
      }
   }
}

void ReplaceGeometry::OnAutoReplace() 
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      with = doc->getBlockAt(i);

      if (!with) continue;

      if (with->getFlags() & BL_FILE)
         continue;

      if (with->getFileNumber() == -1)
         continue;

      if (doc->Find_File(with->getFileNumber())->getBlockType() != blockTypeGeomLibrary)
         continue;

      for (int j=0; j<doc->getMaxBlockIndex(); j++)
      {
         replace = doc->getBlockAt(j);

         if (!replace) continue;

         if (replace->getFlags() & BL_FILE)
            continue;

         if (replace->getFileNumber() != -1)
            if (doc->Find_File(replace->getFileNumber())->getBlockType() == blockTypeGeomLibrary)
               continue;

         if (!replace->getName().Compare(with->getName()))
            ReplaceGeom();
      }
   }

   doc->UpdateAllViews(NULL);
   OnCancel();
}


/////////////////////////////////////////////////////////////////////////////
// MapKeywords dialog
MapKeywords::MapKeywords(CWnd* pParent /*=NULL*/)
   : CDialog(MapKeywords::IDD, pParent)
{
   //{{AFX_DATA_INIT(MapKeywords)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void MapKeywords::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MapKeywords)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MapKeywords, CDialog)
   //{{AFX_MSG_MAP(MapKeywords)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL MapKeywords::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 2;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = width;

   column.pszText = "Original";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.pszText = "New";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   // fill list
   LV_ITEM item;
   int actualItem;
   int j = 0;
   POSITION pos = keywordMap->GetStartPosition();
   while (pos != NULL)
   {
      CString oldKW, newKW;

      keywordMap->GetNextAssoc(pos, oldKW, newKW);

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = oldKW.GetBuffer(0);
      actualItem = m_list.InsertItem(&item);
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_list.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_list.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_list.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_list.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_list.InsertItem(&item);
               }
            }
         }
      }

      item.iItem = actualItem;
      item.iSubItem = 1;
      item.pszText = newKW.GetBuffer(0);
      m_list.SetItem(&item);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void MapKeywords::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
	int i=0;
   for (i=0; i < count; i++)
   {
      if (m_list.GetItemState(i, LVIS_SELECTED))
         break;
   }

   MapKeywordEdit dlg;
   dlg.m_orig = m_list.GetItemText(i, 0);
   keywordMap->Lookup(dlg.m_orig, dlg.m_new);
   if (dlg.DoModal() == IDOK)
   {
      keywordMap->SetAt(dlg.m_orig, dlg.m_new);

      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 1;
      item.pszText = dlg.m_new.GetBuffer(0);
      m_list.SetItem(&item);
   }  
}

void MapKeywords::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// MapKeywordEdit dialog
MapKeywordEdit::MapKeywordEdit(CWnd* pParent /*=NULL*/)
   : CDialog(MapKeywordEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(MapKeywordEdit)
   m_orig = _T("");
   m_new = _T("");
   //}}AFX_DATA_INIT
}

void MapKeywordEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MapKeywordEdit)
   DDX_Text(pDX, IDC_ORIG, m_orig);
   DDX_Text(pDX, IDC_NEW, m_new);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MapKeywordEdit, CDialog)
   //{{AFX_MSG_MAP(MapKeywordEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MapLayersDlg dialog
MapLayersDlg::MapLayersDlg(CWnd* pParent /*=NULL*/)
   : CDialog(MapLayersDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(MapLayersDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void MapLayersDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MapLayersDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MapLayersDlg, CDialog)
   //{{AFX_MSG_MAP(MapLayersDlg)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDOK, OnOk)
   ON_BN_CLICKED(IDC_DUPLICATE_ALL, OnDuplicateAll)
   ON_BN_CLICKED(IDC_DUPLICATE_UNMAPPED, OnDuplicateUnmapped)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL MapLayersDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 3;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "Library Layer";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.pszText = "Layer Type";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);
   
   column.pszText = "New Layer";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);
   
   // fill list
   FillList();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void MapLayersDlg::FillList()
{
   m_list.DeleteAllItems();

   LV_ITEM item;
   int actualItem;
   int j = 0;
   void *voidPtr1, *voidPtr2;
   LayerStruct *origLayer, *newLayer;
   
   for (POSITION pos = layerMap->GetStartPosition();pos != NULL;)
   {
      layerMap->GetNextAssoc(pos, voidPtr1, voidPtr2);
      origLayer = (LayerStruct*)voidPtr1;
      newLayer = (LayerStruct*)voidPtr2;

      CString originalLayerName(origLayer->getName());

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = originalLayerName.GetBuffer(0);
      actualItem = m_list.InsertItem(&item);

      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_list.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_list.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_list.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_list.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_list.InsertItem(&item);
               }
            }
         }
      }

      m_list.SetItemData(actualItem, (DWORD)origLayer);

      item.iItem = actualItem;
      item.iSubItem = 1;
      item.pszText = (char*)(LPCTSTR)layerTypeToString(origLayer->getLayerType());
      m_list.SetItem(&item);

      item.iItem = actualItem;
      item.iSubItem = 2;

      if (newLayer)
      {
         // THIS IS REALLY BAD CODE
         // It is saving a ptr to a buffer on a CString and using it after the CString is deleted.
         // It fails obviously in Debug mode, but curiously it seems to work okay in Release mode.
         // I suspect that is luck of the draw, and won't always be so accomodating.
         // This source file is filled with many instances of this same error.
         CString layerName(newLayer->getName());
         item.pszText = layerName.GetBuffer(0);
      }
      else
         item.pszText = "";

      m_list.SetItem(&item);
   }
}

void MapLayersDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();

	int i=0;
   for (i=0; i < count; i++)
   {
      if (m_list.GetItemState(i, LVIS_SELECTED))
         break;
   }

   LayerStruct *origLayer = (LayerStruct*)m_list.GetItemData(i);

   MapLayerEdit dlg;
   dlg.origLayer = origLayer;
   dlg.m_origLayer = origLayer->getName();
   dlg.m_origType = layerTypeToString(origLayer->getLayerType());
   dlg.layertype = origLayer->getLayerType();
   dlg.doc = doc;

   if (dlg.DoModal() == IDOK)
   {
      layerMap->SetAt(origLayer, dlg.newLayer);

      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 2;

      CString layerName(dlg.newLayer->getName());

      if (dlg.newLayer)
         item.pszText = layerName.GetBuffer(0);
      else
         item.pszText = "";

      m_list.SetItem(&item);
   }
}

void MapLayersDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}

void MapLayersDlg::OnDuplicateAll() 
{
   void *origVoidPtr, *newVoidPtr;

   POSITION pos = layerMap->GetStartPosition();
   while (pos != NULL)
   {
      layerMap->GetNextAssoc(pos, origVoidPtr, newVoidPtr);

      LayerStruct *origLayer = (LayerStruct*)origVoidPtr, *newLayer = (LayerStruct*)newVoidPtr;

      if (!newLayer || origLayer->getName().Compare(newLayer->getName()))
      {
         newLayer = doc->Add_Layer(origLayer->getName());
         newLayer->setLayerType(origLayer->getLayerType());
         layerMap->SetAt(origLayer, newLayer);
      }
   }

   FillList();
}

void MapLayersDlg::OnDuplicateUnmapped() 
{
   void *origVoidPtr, *newVoidPtr;

   POSITION pos = layerMap->GetStartPosition();
   while (pos != NULL)
   {
      layerMap->GetNextAssoc(pos, origVoidPtr, newVoidPtr);

      LayerStruct *origLayer = (LayerStruct*)origVoidPtr, *newLayer = (LayerStruct*)newVoidPtr;

      if (!newLayer)
      {
         newLayer = doc->Add_Layer(origLayer->getName());
         newLayer->setLayerType(origLayer->getLayerType());
         layerMap->SetAt(origLayer, newLayer);
      }
   }

   FillList();
}

void MapLayersDlg::OnOk() 
{
   BOOL FoundAMappedLayer = FALSE, FoundAnUnmappedLayer = FALSE;

   void *layer, *voidPtr;

   POSITION pos = layerMap->GetStartPosition();
   while (pos != NULL)
   {
      layerMap->GetNextAssoc(pos, layer, voidPtr);
      if (voidPtr)
         FoundAMappedLayer = TRUE;
      else
         FoundAnUnmappedLayer = TRUE;
   }

   if (!FoundAMappedLayer)
   {
      ErrorMessage("You must map at least one layer before proceeding.", "No Layers Mapped");
      return;
   }

   if (FoundAnUnmappedLayer)
   {
      if (IDNO == ErrorMessage("Do you want to proceed?", "There are unmapped layers.", MB_YESNO))
         return;
   }

   OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// MapLayerEdit dialog
MapLayerEdit::MapLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(MapLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(MapLayerEdit)
   m_choice = 1;
   m_newLayer = _T("");
   m_origLayer = _T("");
   m_origType = _T("");
   //}}AFX_DATA_INIT
}


void MapLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MapLayerEdit)
   DDX_Control(pDX, IDC_SAME_LB, m_sameLB);
   DDX_Control(pDX, IDC_DIFFERENT_LB, m_differentLB);
   DDX_Radio(pDX, IDC_CHOICE, m_choice);
   DDX_Text(pDX, IDC_NEW_LAYER, m_newLayer);
   DDX_Text(pDX, IDC_ORIG_LAYER, m_origLayer);
   DDX_Text(pDX, IDC_ORIG_TYPE, m_origType);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MapLayerEdit, CDialog)
   //{{AFX_MSG_MAP(MapLayerEdit)
   ON_BN_CLICKED(IDC_DIFFERENT, OnDifferent)
   ON_BN_CLICKED(IDC_CHOICE, OnNew)
   ON_BN_CLICKED(IDC_SAME, OnSame)
   ON_BN_CLICKED(IDC_DONT_MAP, OnDontMap)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL MapLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      if (layer->getLayerType() == layertype)
         m_sameLB.SetItemDataPtr(m_sameLB.AddString(layer->getName()), layer);
      else
      {
         CString buf;
         buf.Format("%s [%s]", layer->getName(), layerTypeToString(layer->getLayerType()));
         m_differentLB.SetItemDataPtr(m_differentLB.AddString(buf), layer);
      }
   }  

   OnSame();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void MapLayerEdit::OnNew() 
{
   GetDlgItem(IDC_NEW_LAYER)->EnableWindow(TRUE);
   GetDlgItem(IDC_SAME_LB)->EnableWindow(FALSE);
   GetDlgItem(IDC_DIFFERENT_LB)->EnableWindow(FALSE);
}

void MapLayerEdit::OnSame() 
{
   GetDlgItem(IDC_NEW_LAYER)->EnableWindow(FALSE);
   GetDlgItem(IDC_SAME_LB)->EnableWindow(TRUE);
   GetDlgItem(IDC_DIFFERENT_LB)->EnableWindow(FALSE);
}

void MapLayerEdit::OnDifferent() 
{
   GetDlgItem(IDC_NEW_LAYER)->EnableWindow(FALSE);
   GetDlgItem(IDC_SAME_LB)->EnableWindow(FALSE);
   GetDlgItem(IDC_DIFFERENT_LB)->EnableWindow(TRUE);
}

void MapLayerEdit::OnDontMap() 
{
   GetDlgItem(IDC_NEW_LAYER)->EnableWindow(FALSE);
   GetDlgItem(IDC_SAME_LB)->EnableWindow(FALSE);
   GetDlgItem(IDC_DIFFERENT_LB)->EnableWindow(FALSE);
}

void MapLayerEdit::OnOK() 
{
   UpdateData();

   switch (m_choice)
   {
   case 0: // new layer
      {
         if (m_newLayer.IsEmpty())
            return;
         newLayer = doc->Add_Layer(m_newLayer);
         newLayer->setLayerType(layertype);
      }
      break;

   case 1: // same layertype
      {
         int sel = m_sameLB.GetCurSel();
         if (sel == LB_ERR)
            return;
         newLayer = (LayerStruct*)m_sameLB.GetItemDataPtr(sel);
      }
      break;

   case 2: // different layertype
      {
         int sel = m_differentLB.GetCurSel();
         if (sel == LB_ERR)
            return;
         newLayer = (LayerStruct*)m_differentLB.GetItemDataPtr(sel);
      }
      break;

   case 3: // don't map
      newLayer = NULL;
      break;
   }
   
   CDialog::OnOK();
}


/******************************************************************************
* OnLocalLibrary
*/
void CCEtoODBDoc::OnLocalLibrary() 
{
   OnDoneEditing();

   UnselectAll(FALSE);

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      getFileList().GetNext(filePos)->setShow(false);
   }

   FileStruct *file;
   BOOL Found = FALSE;
   filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      file = getFileList().GetNext(filePos);
      if (file->getBlockType() == blockTypeGeomLibrary)
      {
         Found = TRUE;
         PrepareAddEntity(file);
         break;
      }
   }

   if (!Found)
   {
      StoreDocForImporting();
      file = Graph_File_Start("Local Library", Type_Unknown);
      file->setNotPlacedYet(false);
      file->setBlockType(blockTypeGeomLibrary);
   }

   file->setShow(true);

   // remove old inserts
   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();    
   while (pos != NULL)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);
      RemoveOneEntityFromDataList(this, &file->getBlock()->getDataList(), data);
   }
   file->getBlock()->getDataList().RemoveAll();

   double maxWidth = 0, maxHeight = 0;
   int count = 0;
	int i=0; 
   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);
      
      if (!block)    continue;

      if (block->getFlags() & BL_FILE || block->getFlags() & BL_APERTURE || block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_APERTURE || block->getFlags() & BL_BLOCK_TOOL)
         continue;

      validateBlockExtents(block);

      if (block->extentIsValid() && block->getXmax() - block->getXmin() > maxWidth)
         maxWidth = block->getXmax() - block->getXmin();

      if (block->extentIsValid() && block->getYmax() - block->getYmin() > maxHeight)
         maxHeight = block->getYmax() - block->getYmin();

      count++;
   }

   int rows = (int)floor(sqrt((double)count));
   double x = 0, y = 0;
   int j = 0;
   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (!block)    continue;

      if (block->getFlags() & BL_FILE || block->getFlags() & BL_APERTURE || block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_APERTURE || block->getFlags() & BL_BLOCK_TOOL)
         continue;

      validateBlockExtents(block);

      x = floor((double)(j % rows)) * 1.2 * maxWidth  - (block->extentIsValid() ? block->getXmin() : 0);
      y = floor((double)(j / rows)) * 1.2 * maxHeight - (block->extentIsValid() ? block->getYmin() : 0);

      DataStruct *data = getCamCadData().getNewDataStruct(dataTypeInsert);
      file->getBlock()->getDataList().AddTail(data);
      //data->getAttributesRef() = NULL;
      //data->setEntityNumber(CEntityNumber::allocate());
      //data->setDataType(dataTypeInsert);
      data->setLayerIndex(-1);
      //data->setSelected(false);
      //data->setMarked(false);
      //data->setNegative(false);
      //data->setGraphicClass(graphicClassNormal);
      //data->setFlags(0);
      //data->setColorOverride(false);
      //data->setOverrideColor(0);
      //data->setHidden(false);

      //data->getInsert() = new InsertStruct;
      data->getInsert()->getShadingRef().On = FALSE;
      data->getInsert()->setOriginX(x);
      data->getInsert()->setOriginY(y);
      data->getInsert()->setAngle(0.0);
      data->getInsert()->setMirrorFlags(0);
      data->getInsert()->setScale(1.0);
      data->getInsert()->setBlockNumber(i); 
      data->getInsert()->setInsertType(insertTypeUnknown);
      data->getInsert()->setPlacedBottom(false);

      data->getInsert()->setRefname(NULL);

      j++;
   }

   file->setNotPlacedYet(false);

   //FillCurrentSettings(this);

   OnFitPageToImage();
}


