
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include "stdafx.h"
#include "atrblist.h"
#include "attrib.h"
#include "ccview.h"
#include "drc.h"
#include "CCEtoODB.h"

extern CView *activeView; // from CCVIEW.CPP
extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern char *attribute_group[];

void FillWidthCB(CComboBox *widthCB, short dataWidthIndex, CCEtoODBDoc *doc);
void GetWidthIndex(CComboBox *m_widthCB, short *dataWidthIndex, CCEtoODBDoc *doc);
bool getShowHiddenAttributes();

int insertItem(CListCtrl& listControl,LVITEM* item)
{
   int actualItem = listControl.InsertItem(item);

   // ugly, dirty way to try to work around microsoft bug
   if (actualItem == -1)
   {
      actualItem = listControl.InsertItem(item);

      if (actualItem == -1)
      {
         actualItem = listControl.InsertItem(item);

         if (actualItem == -1)
         {
            actualItem = listControl.InsertItem(item);

            if (actualItem == -1)
            {
               actualItem = listControl.InsertItem(item);

               if (actualItem == -1)
               {
                  actualItem = listControl.InsertItem(item);
               }
            }
         }
      }
   }

   return actualItem;
}

/* OnAssignAttrib()
*/
void CCEtoODBDoc::OnAssignAttrib() 
{         
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   ((CCEtoODBView*)activeView)->lastCommand = LC_AssignAttrib;

   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   static EditAttrib dlg;
   dlg.doc = this;
   if (dlg.DoModal() != IDOK)
      return;

   WORD keyword = IsKeyWord(dlg.keywordName, 0);

   int tempInt;
   double tempDouble;
   void *voidPtr;
   int valueType = getKeyWordArray()[keyword]->getValueType();
   switch (valueType)
   {
   case VT_NONE:
   default:
      voidPtr = NULL;
      break;
   case VT_STRING:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      voidPtr = dlg.m_value.GetBuffer(0);
      break;
   case VT_INTEGER:
      tempInt = atoi(dlg.m_value);
      voidPtr = &tempInt;
      break;
   case VT_DOUBLE:
   case VT_UNIT_DOUBLE:
      tempDouble = atof(dlg.m_value);
      voidPtr = &tempDouble;
      break;
   }

   BOOL applyAll = FALSE;
   int curMethod = SA_RETURN;
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = SelectList.GetNext(pos)->getData();

      if (SetVisAttrib(&data->getAttributesRef(), keyword, valueType, voidPtr, 
            atof(dlg.m_x), atof(dlg.m_y), DegToRad(atof(dlg.m_angle)), atof(dlg.m_height), atof(dlg.m_width), 
            dlg.m_proportional, 0, dlg.m_visible, SA_RETURN, 0L, dlg.layer, dlg.m_neverMirror, 0, 0, dlg.m_mirrorFlip))
      {
         if (valueType != VT_NONE)
         {
            const KeyWordStruct *kw = getKeyWordArray()[keyword];
            Attrib* temp;

            data->getAttributesRef()->Lookup(keyword, temp);

            if (!applyAll)
            {
               OverwriteAttrib overwriteDlg;
               overwriteDlg.doc = this;
               overwriteDlg.attrib = temp;
               overwriteDlg.valueType = kw->getValueType();
               overwriteDlg.m_keyword.Format("Keyword : %s", kw->getCCKeyword());
               overwriteDlg.DoModal();
               
               applyAll = overwriteDlg.All;
               curMethod = overwriteDlg.method;
            }

            if (curMethod != SA_RETURN)
            {
               SetVisAttrib(&data->getAttributesRef(), keyword, valueType, voidPtr, 
                     atof(dlg.m_x), atof(dlg.m_y), DegToRad(atof(dlg.m_angle)), atof(dlg.m_height), atof(dlg.m_width), 
                     dlg.m_proportional, 0, dlg.m_visible, curMethod, 0L, dlg.layer, dlg.m_neverMirror,
                     (temp?((Attrib*)temp)->getHorizontalPosition():0), (temp?((Attrib*)temp)->getVerticalPosition():0),
                     dlg.m_mirrorFlip);
            }
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// ListAttribs dialog
ListAttribs::ListAttribs(CWnd* pParent /*=NULL*/)
   : CResizingDialog(ListAttribs::IDD, pParent)
{
   //{{AFX_DATA_INIT(ListAttribs)
   m_showApertures = FALSE;
   m_showFreepads = FALSE;
   m_showTools = FALSE;
   m_showVias = FALSE;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_SHOW_VIAS     ,anchorRight);
   addFieldControl(IDC_SHOW_FREEPADS ,anchorRight);
   addFieldControl(IDC_SHOW_APERTURES,anchorRight);
   addFieldControl(IDC_SHOW_TOOLS    ,anchorRight);
   addFieldControl(IDOK              ,anchorRight);
   addFieldControl(IDC_EDIT          ,anchorRight);
   addFieldControl(IDC_TREE          ,anchorLeft ,growBoth);
}

ListAttribs::~ListAttribs()
{
   if (m_imageList)
      delete m_imageList;
}

void ListAttribs::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ListAttribs)
   DDX_Control(pDX, IDC_TREE, m_tree);
   DDX_Check(pDX, IDC_SHOW_APERTURES, m_showApertures);
   DDX_Check(pDX, IDC_SHOW_FREEPADS, m_showFreepads);
   DDX_Check(pDX, IDC_SHOW_TOOLS, m_showTools);
   DDX_Check(pDX, IDC_SHOW_VIAS, m_showVias);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ListAttribs, CResizingDialog)
   //{{AFX_MSG_MAP(ListAttribs)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
   ON_BN_CLICKED(IDC_SHOW_APERTURES, OnShowApertures)
   ON_BN_CLICKED(IDC_SHOW_FREEPADS, OnShowFreepads)
   ON_BN_CLICKED(IDC_SHOW_TOOLS, OnShowTools)
   ON_BN_CLICKED(IDC_SHOW_VIAS, OnShowVias)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define iFolder      0
#define iOpenFolder  1
#define iRound       2
#define iRef         4
#define iBlock       6
#define iLayer       8
#define iNet         10
#define iType        12
#define iTypePin     14
#define iCP          16
#define iFile        18 
#define iCompBlk     20
#define iPadStk      22
#define iPad         24
#define iCompLocal   26
#define iTool        28
#define iGate        30
#define iDRC         32
/////////////////////////////////////////////////////////////////////////////
// ListAttribs message handlers
BOOL ListAttribs::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 32, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_ROUND));
   m_imageList->Add(app->LoadIcon(IDI_ROUNDA));
   m_imageList->Add(app->LoadIcon(IDI_REF));
   m_imageList->Add(app->LoadIcon(IDI_REFA));
   m_imageList->Add(app->LoadIcon(IDI_BLOCK));
   m_imageList->Add(app->LoadIcon(IDI_BLOCKA));
   m_imageList->Add(app->LoadIcon(IDI_LAYER));
   m_imageList->Add(app->LoadIcon(IDI_LAYERA));
   m_imageList->Add(app->LoadIcon(IDI_NET));
   m_imageList->Add(app->LoadIcon(IDI_NETA));
   m_imageList->Add(app->LoadIcon(IDI_TYPE));
   m_imageList->Add(app->LoadIcon(IDI_TYPEA));
   m_imageList->Add(app->LoadIcon(IDI_TYPEPIN));
   m_imageList->Add(app->LoadIcon(IDI_TYPEPINA));
   m_imageList->Add(app->LoadIcon(IDI_CP));
   m_imageList->Add(app->LoadIcon(IDI_CPA));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_FILEA));
   m_imageList->Add(app->LoadIcon(IDI_COMPBLK));
   m_imageList->Add(app->LoadIcon(IDI_COMPBLKA));
   m_imageList->Add(app->LoadIcon(IDI_PADSTK));
   m_imageList->Add(app->LoadIcon(IDI_PADSTKA));
   m_imageList->Add(app->LoadIcon(IDI_PAD));
   m_imageList->Add(app->LoadIcon(IDI_PADA));
   m_imageList->Add(app->LoadIcon(IDI_COMPLOCAL));
   m_imageList->Add(app->LoadIcon(IDI_COMPLOCALA));
   m_imageList->Add(app->LoadIcon(IDI_DRILL));
   m_imageList->Add(app->LoadIcon(IDI_DRILLA));
   m_imageList->Add(app->LoadIcon(IDI_GATE));
   m_imageList->Add(app->LoadIcon(IDI_GATEA));
   m_imageList->Add(app->LoadIcon(IDI_DRC));
   m_imageList->Add(app->LoadIcon(IDI_DRCA));
   
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   FillTree();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ListAttribs::FillTree() 
{
   UpdateData();

   CWaitCursor wait;

   m_tree.DeleteAllItems();

   POSITION pos, netPos, compPinPos, typePos, typePinPos, drcPos;
   HTREEITEM root, fileRoot, netListRoot, netRoot, typeListRoot, typeRoot, drcRoot, item;
   CString buf;
   int icon;

   // Files
   root = m_tree.InsertItem("Files", iFolder, iOpenFolder);
   m_tree.SetItemData(root, NULL);
   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);

      fileRoot = m_tree.InsertItem(file->getName(), iFolder, iOpenFolder, root, TVI_SORT);
      m_tree.SetItemData(fileRoot, NULL);

      // File Block
      InsertBlock(file->getBlock(), fileRoot, TRUE);

      // NetList
      if (!file->getNetList().IsEmpty())
      {
         netListRoot = m_tree.InsertItem("Net List", iFolder, iOpenFolder, fileRoot);
         m_tree.SetItemData(netListRoot, NULL);
         netPos = file->getNetList().GetHeadPosition();
         while (netPos != NULL)
         {
            NetStruct *net = file->getNetList().GetNext(netPos);
            icon = iNet;

            // if attribmap and not empty
            if (net->getAttributesRef() && !net->getAttributesRef()->IsEmpty()) icon++;
            netRoot = m_tree.InsertItem(net->getNetName(), icon, icon, netListRoot, TVI_SORT);
            m_tree.SetItemData(netRoot, (DWORD)&(net->getAttributesRef()));

            // CompPinList
            compPinPos = net->getHeadCompPinPosition();
            while (compPinPos != NULL)
            {
               CompPinStruct *compPin = net->getNextCompPin(compPinPos);
               icon = iCP;

               // if attribmap and not empty
               if (compPin->getAttributesRef() && !compPin->getAttributesRef()->IsEmpty()) icon++;
               buf.Format("%s %s", compPin->getRefDes(), compPin->getPinName());
               item = m_tree.InsertItem(buf, icon, icon, netRoot, TVI_SORT);
               m_tree.SetItemData(item, (DWORD)&(compPin->getAttributesRef()));
            }
         }
      }

      // TypeList
      if (!file->getTypeList().IsEmpty())
      {
         typeListRoot = m_tree.InsertItem("Device List", iFolder, iOpenFolder, fileRoot);
         m_tree.SetItemData(typeListRoot, NULL);
         typePos = file->getTypeList().GetHeadPosition();
         while (typePos != NULL)
         {
            TypeStruct *type = file->getTypeList().GetNext(typePos);
            icon = iType;

            // if attribmap and not empty
            if (type->getAttributesRef() && !type->getAttributesRef()->IsEmpty()) icon++;
            
            // if blocknum -1 then no geometry was assigned
            CString  bname = "";

            // this block not be there - deleted by someone, but the typelist is not updated.
            if (type->getBlockNumber() > -1)
            {
					BlockStruct *block = doc->getBlockAt(type->getBlockNumber());
					if (block == NULL || block->getName().IsEmpty())
                  bname = "No Geometry";
					else
						bname = block->getName();
            }
            buf.Format("%s (%s)", type->getName(),(type->getBlockNumber() < 0)?"No Geometry":bname);

            typeRoot = m_tree.InsertItem(buf, icon, icon, typeListRoot, TVI_SORT);
            m_tree.SetItemData(typeRoot, (DWORD)&(type->getAttributesRef()));

            // TypePinList
            typePinPos = type->getTypePinList().GetHeadPosition();
            while (typePinPos != NULL)
            {
               TypePinStruct *typePin = type->getTypePinList().GetNext(typePinPos);
               icon = iTypePin;

               // if attribmap and not empty
               if (typePin->getAttributesRef() && !typePin->getAttributesRef()->IsEmpty()) icon++;

               item = m_tree.InsertItem(typePin->getPhysicalPinName(), icon, icon, typeRoot, TVI_SORT);
               m_tree.SetItemData(item, (DWORD)&(typePin->getAttributesRef()));
            }
         }
      }

      // DRCList
      if (!file->getDRCList().IsEmpty())
      {
         drcRoot = m_tree.InsertItem("DRC", iFolder, iOpenFolder, fileRoot);
         m_tree.SetItemData(drcRoot, NULL);
         drcPos = file->getDRCList().GetHeadPosition();
         while (drcPos != NULL)
         {
            DRCStruct *drc = file->getDRCList().GetNext(drcPos);
            icon = iDRC;

            // if attribmap and not empty
            if (drc->getAttributesRef() && !drc->getAttributesRef()->IsEmpty()) icon++;
            
            item = m_tree.InsertItem(drc->getString(), icon, icon, drcRoot, TVI_SORT);
            m_tree.SetItemData(item, (DWORD)&(drc->getAttributesRef()));
         }
      }
   }

   // Geometries
   root = m_tree.InsertItem("Geometries", iFolder, iOpenFolder);
   m_tree.SetItemData(root, NULL);
	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getName().IsEmpty()) continue;

      if (block->getFlags() & BL_FILE)
         continue;

      InsertBlock(block, root, FALSE);
   }

   // Layers
   root = m_tree.InsertItem("Layers", iFolder, iOpenFolder);
   m_tree.SetItemData(root, NULL);

   for (i=0; i< doc->getMaxLayerIndex(); i++)
   {
      CString  t;
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      icon = iLayer;

      // if layer attribute and not empty
      if (layer->getAttributesRef() && !layer->getAttributesRef()->IsEmpty()) icon++;
      
      if (layer->getComment().GetLength())
         t.Format("%s (%s)",layer->getName(), layer->getComment());
      else
         t.Format("%s",layer->getName());

      item = m_tree.InsertItem(t, icon, icon, root, TVI_SORT);
      m_tree.SetItemData(item, (DWORD)&(layer->getAttributesRef()));
   }
}

// here all geometries are listed in Navigator
void ListAttribs::InsertBlock(BlockStruct *block, HTREEITEM root, BOOL IsFile) 
{
   DataStruct *data;
   BlockStruct *subblock;
   POSITION dataPos;
   HTREEITEM item, blockRoot;

   int icon = iBlock;
   if (block->getFlags() & BL_TOOL 
      || block->getFlags() & BL_BLOCK_TOOL
      || block->getFlags() & BL_COMPLEX_TOOL)
      icon = iTool;
   else if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      icon = iRound;
   if (block->getFlags() & BL_FILE)
      icon = iFile;

   CString text = block->getName();
   if (block->getFileNumber() != -1 && !(block->getFlags() & BL_GLOBAL))
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);
         if (file->getFileNumber() == block->getFileNumber())
         {
            text += " [";
            text += file->getName();
            text += "]";
            break;
         }
      }
   }

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
         // here do layer
      break;
      case BLOCKTYPE_LOCALPCBCOMP:
         icon = iCompLocal;
      break;
      case BLOCKTYPE_TOOLING:
         icon = iTool;
      break;
      case BLOCKTYPE_SYMBOL:
         icon = iGate;
      break;
   }
   int orig_icon = icon;   // remember 

   // here increment to icon - attribute bitmap, which is +1 of normal
   if (block->getAttributesRef() &&!block->getAttributesRef()->IsEmpty()) 
         icon++;
   
   blockRoot = m_tree.InsertItem(text, icon, icon, root, TVI_SORT);

   m_tree.SetItemData(blockRoot, (DWORD)&(block->getAttributesRef()));
   dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      data = block->getDataList().GetNext(dataPos);
      if (data->getDataType() == T_INSERT)
      {
         if ((!m_showVias && data->getInsert()->getInsertType() == INSERTTYPE_VIA) || 
               (!m_showFreepads && data->getInsert()->getInsertType() == INSERTTYPE_FREEPAD))
            continue;

         subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         if (subblock == NULL)   continue;

         if (!m_showApertures && ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE)))
            continue;

         if (!m_showTools && ((subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL)))
            continue;

         CString text;

         // here make the layername for padstacks.
         if (orig_icon == iPadStk)
         {
            CString layer;
            if (data->getLayerIndex() < 0)
               layer = "";
            else
               layer = doc->getLayerArray()[data->getLayerIndex()]->getName();

            text.Format("%s [%s]", subblock->getName(), layer);
         }
         else
            text.Format("%s \"%s\"", subblock->getName(), 
               (data->getInsert()->getRefname() ? data->getInsert()->getRefname() : ""));

         icon = iRef;

         // if inserttype
         switch (data->getInsert()->getInsertType())
         {
            case INSERTTYPE_PCBCOMPONENT:
               icon = iCompBlk;
            break;
         }

         // if inserts a tool
         if (subblock->getFlags() & BL_TOOL 
            || subblock->getFlags() & BL_BLOCK_TOOL
            || subblock->getFlags() & BL_COMPLEX_TOOL)
            icon = iTool;
         // if inserts an aperture
         else if (subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE)
            icon = iRound;

         // here must be attribute and not empty
         if (data->getAttributesRef() && !data->getAttributesRef()->IsEmpty()) icon++;
         item = m_tree.InsertItem(text, icon, icon, blockRoot, TVI_SORT);
         m_tree.SetItemData(item, (DWORD)&(data->getAttributesRef()));
      }
   }
}

void ListAttribs::OnShowApertures() 
{
   FillTree(); 
}

void ListAttribs::OnShowFreepads() 
{
   FillTree(); 
}

void ListAttribs::OnShowTools() 
{
   FillTree(); 
}

void ListAttribs::OnShowVias() 
{
   FillTree(); 
}

void ListAttribs::OnEdit() 
{
   HTREEITEM item;
   CAttributes** map;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   map = (CAttributes**)m_tree.GetItemData(item); 

   if (map == NULL) return; // non-data item (folder, etc.)

   EditAttribs dlg;
   dlg.doc = doc;
   dlg.m_item = m_tree.GetItemText(item);
   dlg.setMap(*map);
   BOOL Empty = ((*map) == NULL);

   if (dlg.DoModal() == IDOK)
   {
      if (Empty && ((*map) != NULL)) // if map was empty and user added attributes, then update icon
      {
         int image, selImage;
         m_tree.GetItemImage(item, image, selImage);
         m_tree.SetItemImage(item, image+1, image+1); 
      }
   }
}

void ListAttribs::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();   
   *pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// EditAttribs dialog
EditAttribs::EditAttribs(CWnd* pParent /*=NULL*/)
   : CResizingDialog(EditAttribs::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditAttribs)
   m_item = _T("");
   //}}AFX_DATA_INIT

   m_imageList = NULL;
   m_map       = NULL;

   addFieldControl(IDC_LIST,anchorLeft,growBoth);
}

EditAttribs::~EditAttribs()
{
   delete m_imageList;
   delete m_map;
}

void EditAttribs::setMap(CAttributes*& map)
{
   delete m_map;

   m_map = new CAttributeMapWrap(map);
}

void EditAttribs::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditAttribs)
   DDX_Control(pDX, IDC_LIST, m_list);
   DDX_Text(pDX, IDC_ITEM, m_item);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditAttribs, CResizingDialog)
   //{{AFX_MSG_MAP(EditAttribs)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_NEW, OnNew)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
   ON_WM_DRAWITEM()
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_DUPLICATE, OnBnClickedDuplicate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditAttribs message handlers
BOOL EditAttribs::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 4, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_OFF));
   m_imageList->Add(app->LoadIcon(IDI_ON));
   m_imageList->Add(app->LoadIcon(IDI_OFF_INT));
   m_imageList->Add(app->LoadIcon(IDI_ON_INT));
   
   m_list.SetImageList(m_imageList, LVSIL_SMALL);

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 11;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width * 2;
   column.pszText = "KeyWord";
   column.iSubItem = m_keywordColumnIndex;
   m_list.InsertColumn(m_keywordColumnIndex, &column);

   column.pszText = "Value";
   column.iSubItem = m_valueColumnIndex;
   m_list.InsertColumn(m_valueColumnIndex, &column);

   column.cx = width/3;
   column.pszText = "#";
   column.iSubItem = m_instanceColumnIndex;
   m_list.InsertColumn(m_instanceColumnIndex, &column);

   column.cx = width;
   column.pszText = "X Offset";
   column.iSubItem = m_xOffsetColumnIndex;
   m_list.InsertColumn(m_xOffsetColumnIndex, &column);

   column.pszText = "Y Offset";
   column.iSubItem = m_yOffsetColumnIndex;
   m_list.InsertColumn(m_yOffsetColumnIndex, &column);

   column.pszText = "Angle";
   column.iSubItem = m_angleColumnIndex;
   m_list.InsertColumn(m_angleColumnIndex, &column);

   column.pszText = "Height";
   column.iSubItem = m_heightColumnIndex;
   m_list.InsertColumn(m_heightColumnIndex, &column);

   column.cx = width * 2;
   column.pszText = "Layer";
   column.iSubItem = m_layerColumnIndex;
   m_list.InsertColumn(m_layerColumnIndex, &column);

   column.cx = width;
   column.pszText = "Never Mirror";
   column.iSubItem = m_neverMirrorColumnIndex;
   m_list.InsertColumn(m_neverMirrorColumnIndex, &column);

   column.cx = width;
   column.pszText = "Mirror Flip";
   column.iSubItem = m_mirrorFlipColumnIndex;
   m_list.InsertColumn(m_mirrorFlipColumnIndex, &column);

   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

int CALLBACK EditAttribs::sortByNameAndInstanceNumber(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
   // lParamSort contains a pointer to the list view control.
   CListCtrl* listControl = (CListCtrl*)lParamSort;
   CNamedAttributeInstance* namedAttribute1 = (CNamedAttributeInstance*)lParam1;
   CNamedAttributeInstance* namedAttribute2 = (CNamedAttributeInstance*)lParam2;

   int retval = namedAttribute1->getName().CompareNoCase(namedAttribute2->getName());

   if (retval == 0)
   {
      retval = namedAttribute1->getInstanceNumber() - namedAttribute2->getInstanceNumber();
   }

   return retval;
}

void EditAttribs::FillListCtrl() 
{
   m_list.DeleteAllItems();
   m_namedAttributeInstanceList.empty();

   if (m_map == NULL)
   {
      return;
   }

   Attrib* parentAttrib;
   CString buf;
   WORD word;
   CString keyword;
   LV_ITEM item;
   int actualItem;
   LayerStruct *layer;

   int decimals = GetDecimals(doc->getSettings().getPageUnits());  
   int j = 0;

   for (POSITION pos = m_map->GetStartPosition();pos != NULL;)
   {
      m_map->GetNextAssoc(pos, word, parentAttrib);
      bool multipleInstanceFlag = (parentAttrib->getCount() > 1);
      int instanceNumber = 0;

      AttribIterator attribIterator(parentAttrib);
      Attrib attrib(parentAttrib->getCamCadData());

      while (attribIterator.getNext(attrib))
      {
         const KeyWordStruct* keywordStruct = doc->getKeyWordArray()[word];

         if (keywordStruct->getHidden() && !getShowHiddenAttributes())
         {
            continue;
         }

         item.mask = LVIF_TEXT | LVIF_IMAGE;
         item.iItem = j++;
         item.iSubItem = 0;
         keyword = keywordStruct->getCCKeyword();
         item.pszText = keyword.GetBuffer(0);
         item.iImage = attrib.isVisible();

         instanceNumber++;
         CNamedAttributeInstance* namedAttribute = new CNamedAttributeInstance(word,keyword,attrib,instanceNumber);
         m_namedAttributeInstanceList.AddTail(namedAttribute);

         if (keywordStruct->getGroup())
         {
            item.iImage += 2;
         }

         actualItem = insertItem(m_list,&item);

         m_list.SetItemData(actualItem, (LPARAM)namedAttribute);

         switch(attrib.getValueType())
         {
         case VT_NONE:
            buf = "";
            break;
         case VT_INTEGER:
            buf.Format("%d", attrib.getIntValue());
            break;
         case VT_DOUBLE:
         case VT_UNIT_DOUBLE:
            buf.Format("%.*lf", decimals, attrib.getDoubleValue());
            break;
         case VT_STRING:
            {
               if (attrib.getStringValueIndex() == -1)
               {
                  buf = "";
               }
               else
               {
                  char *tok, *temp = STRDUP(attrib.getStringValue());

                  if ((tok = strtok(temp, "\n")) == NULL)
                  {
                     buf = "\"\"";
                  }
                  else
                  {
                     buf.Format("\"%s\"", tok);
                     tok = strtok(NULL, "\n");

                     while (tok != NULL)
                     {
                        buf += ",\"";
                        buf += tok;
                        buf += "\"";
                        tok = strtok(NULL, "\n");
                     }
                  }

                  free(temp);
               }
            }
            break;
         case VT_EMAIL_ADDRESS:
            buf = "";

            if (attrib.getStringValue().Left(7) != "mailto:")
               buf = "mailto:";
            if (attrib.getStringValueIndex() != -1)
               buf += attrib.getStringValue();
            break;
         case VT_WEB_ADDRESS:
            buf = "";

            if (attrib.getStringValue().Left(7) != "http://")
               buf = "http://";
            if (attrib.getStringValueIndex() != -1)
               buf += attrib.getStringValue();
            break;
         }

         item.mask = LVIF_TEXT;
         item.iItem = actualItem;
         item.iSubItem = m_valueColumnIndex;
         item.pszText = buf.GetBuffer(0);
         m_list.SetItem(&item);

         CString instanceString(" ");

         if (multipleInstanceFlag)
         {
            instanceString.Format("%d",instanceNumber);
         }

         item.iSubItem = m_instanceColumnIndex;
         item.pszText = instanceString.GetBuffer();
         m_list.SetItem(&item);

         if (attrib.isVisible())
         {
            item.iSubItem = m_xOffsetColumnIndex;
            buf.Format("%.*lf", decimals, attrib.getX());
            item.pszText = buf.GetBuffer(0);
            m_list.SetItem(&item);

            item.iSubItem = m_yOffsetColumnIndex;
            buf.Format("%.*lf", decimals, attrib.getY());
            item.pszText = buf.GetBuffer(0);
            m_list.SetItem(&item);

            item.iSubItem = m_angleColumnIndex;
            buf.Format("%.*lf", decimals, normalizeDegrees(attrib.getRotationDegrees()));
            item.pszText = buf.GetBuffer(0);
            m_list.SetItem(&item);

            item.iSubItem = m_heightColumnIndex;
            buf.Format("%.*lf", decimals, attrib.getHeight());
            item.pszText = buf.GetBuffer(0);
            m_list.SetItem(&item);

            item.iSubItem = m_layerColumnIndex;
            layer = doc->FindLayer(attrib.getLayerIndex());
            CString layerName((layer != NULL) ? layer->getName() : "");
            item.pszText = layerName.GetBuffer(0);
            m_list.SetItem(&item);

            item.iSubItem = m_neverMirrorColumnIndex;
            item.pszText = (attrib.getMirrorDisabled() ? "X" : "");
            m_list.SetItem(&item);

            item.iSubItem = m_mirrorFlipColumnIndex;
            item.pszText = (attrib.getMirrorFlip() ? "X" : "");
            m_list.SetItem(&item);
         }
         else // do not show these fields for non-visible attributes
         {
            char* blank = "";

            for (int x = m_instanceColumnIndex + 1; x <= m_maxColumnIndex; x++)
            {
               item.iSubItem = x;
               item.pszText = blank;
               m_list.SetItem(&item);
            }
         }
      }
   }

   m_list.SortItems(sortByNameAndInstanceNumber,(LPARAM)(&m_list));
}

CNamedAttributeInstance& EditAttribs::getListControlAttributeInstance(int index)
{
   CNamedAttributeInstance* namedAttributeInstance = (CNamedAttributeInstance*)m_list.GetItemData(index);

   return *namedAttributeInstance;
}

void EditAttribs::OnEdit() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Edit!");
      return;
   }*/

   if (m_map == NULL || !m_list.GetSelectedCount())
   {
      return;
   }

   int count = m_list.GetItemCount();

	int i=0;
   for (i=0; i < count; i++)
   {
      if (m_list.GetItemState(i, LVIS_SELECTED))
         break;
   }

   CString keyword = m_list.GetItemText(i, 0);
   CNamedAttributeInstance& namedAttributeInstance = getListControlAttributeInstance(i);;
   const Attrib& attrib = namedAttributeInstance.getAttribute();
   DbFlag   flg;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   EditAttrib dlg;
   dlg.doc = doc;
   dlg.keywordName = keyword;
   flg = attrib.getFlags();
   dlg.m_valuetype = "Value Type : ";

   switch (attrib.getValueType())
   {
   case VT_NONE:
      dlg.m_valuetype += "None";
      dlg.m_value = "";
      break;
   case VT_INTEGER:
      dlg.m_valuetype += "Integer";
      dlg.m_value.Format("%d", attrib.getIntValue());
      break;
   case VT_DOUBLE:
      dlg.m_valuetype += "Floating Point";
      dlg.m_value.Format("%.*lf", decimals, attrib.getDoubleValue());
      break;
   case VT_UNIT_DOUBLE:
      dlg.m_valuetype += "Unit-Dependent Floating Point";
      dlg.m_value.Format("%.*lf", decimals, attrib.getDoubleValue());
      break;
   case VT_STRING:
      dlg.m_valuetype += "String";
      dlg.m_value = attrib.getStringValue();
      break;
   case VT_EMAIL_ADDRESS:
      dlg.m_valuetype += "Email Address";
      dlg.m_value = attrib.getStringValue();
      break;
   case VT_WEB_ADDRESS:
      dlg.m_valuetype += "Web Address";
      dlg.m_value = attrib.getStringValue();
      break;
   }

   dlg.m_x.Format("%.*lf", decimals, attrib.getX());
   dlg.m_y.Format("%.*lf", decimals, attrib.getY());
	dlg.m_angle.Format("%.2lf", normalizeDegrees(attrib.getRotationDegrees()));
	dlg.m_height.Format("%.*lf", decimals, attrib.getHeight());
   dlg.m_width.Format("%.*lf", decimals, attrib.getWidth());
   dlg.m_proportional = attrib.isProportionallySpaced();
   dlg.m_visible      = attrib.isVisible();
   dlg.m_neverMirror  = attrib.getMirrorDisabled();
   dlg.m_mirrorFlip   = attrib.getMirrorFlip();
   dlg.layer          = attrib.getLayerIndex();
   dlg.penWidthIndex  = attrib.getPenWidthIndex();
   dlg.setHorizontalPosition(attrib.getHorizontalPosition());
   dlg.setVerticalPosition(attrib.getVerticalPosition());

   if (dlg.DoModal() != IDOK)
      return;

   WORD kw = doc->IsKeyWord(dlg.keywordName, 0);

   int tempInt;
   double tempDouble;
   void *voidPtr;
   int valueType = doc->getKeyWordArray()[kw]->getValueType();

   switch (valueType)
   {
   case VT_NONE:
   default:
      voidPtr = NULL;
      break;
   case VT_STRING:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      voidPtr = dlg.m_value.GetBuffer(0);
      break;
   case VT_INTEGER:
      tempInt = atoi(dlg.m_value);
      voidPtr = &tempInt;
      break;
   case VT_DOUBLE:
   case VT_UNIT_DOUBLE:
      tempDouble = atof(dlg.m_value);
      voidPtr = &tempDouble;
      break;
   }

   bool changedKeywordFlag = (dlg.keywordName.CompareNoCase(keyword) != 0);

   if (changedKeywordFlag)
   {
      m_map->deleteInstanceLike(namedAttributeInstance);

      doc->SetVisAttrib(&(m_map->getMapPtr()), kw, valueType, voidPtr, 
            atof(dlg.m_x), atof(dlg.m_y), DegToRad(atof(dlg.m_angle)), atof(dlg.m_height), atof(dlg.m_width), 
            dlg.m_proportional, dlg.penWidthIndex, dlg.m_visible, SA_APPEND, 0L, 
            dlg.layer, dlg.m_neverMirror, dlg.getHorizontalPosition(), dlg.getVerticalPosition(), dlg.m_mirrorFlip);
   }
   else
   {
      Attrib modifiedAttrib(attrib);

      modifiedAttrib.setProperties(atof(dlg.m_x),atof(dlg.m_y),DegToRad(atof(dlg.m_angle)),atof(dlg.m_height),
         atof(dlg.m_width),dlg.penWidthIndex,dlg.m_proportional,
         dlg.m_visible,dlg.m_neverMirror,dlg.m_mirrorFlip,0,dlg.getHorizontalPosition(), dlg.getVerticalPosition(),dlg.layer,0);
      modifiedAttrib.setValueFromString(doc->getCamCadData(), kw,dlg.m_value);

      m_map->updateInstanceLike(namedAttributeInstance,modifiedAttrib);
   }

   FillListCtrl();
}

void EditAttribs::OnDelete() 
{
   if (m_map == NULL || m_list.GetSelectedCount() == 0)
   {
      return;
   }

   int count = m_list.GetItemCount();

	int i=0;
   for (i=0; i < count; i++)
   {
      if (m_list.GetItemState(i, LVIS_SELECTED))
         break;
   }

   CString keyword = m_list.GetItemText(i, 0);
   //int keywordIndex = doc->IsKeyWord(keyword, FALSE);
   CNamedAttributeInstance& namedAttributeInstance = getListControlAttributeInstance(i);;

   m_map->deleteInstanceLike(namedAttributeInstance);

   FillListCtrl();
}

void EditAttribs::OnBnClickedDuplicate()
{
   if (m_map == NULL || m_list.GetSelectedCount() == 0)
   {
      return;
   }

   int count = m_list.GetItemCount();

	int i=0; 
   for (i=0; i < count; i++)
   {
      if (m_list.GetItemState(i, LVIS_SELECTED))
         break;
   }

   CString keyword = m_list.GetItemText(i, 0);
   int keywordIndex = doc->IsKeyWord(keyword, FALSE);
   CNamedAttributeInstance& namedAttributeInstance = getListControlAttributeInstance(i);

   m_map->duplicateInstanceLike(namedAttributeInstance);

   FillListCtrl();
}

void EditAttribs::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();   
   *pResult = 0;
}

void EditAttribs::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NMITEMACTIVATE *nm = (NMITEMACTIVATE*)pNMHDR;

   *pResult = 0;

   int item, subitem;

   if ((subitem = nm->iSubItem) != 1)
      return;

   LVHITTESTINFO hitTestInfo;
   hitTestInfo.pt.x = 5;
   hitTestInfo.pt.y = nm->ptAction.y;
   m_list.HitTest(&hitTestInfo);
   
   if ((item = hitTestInfo.iItem) == -1)
      return;

   CNamedAttributeInstance& namedAttributeInstance = getListControlAttributeInstance(item);;
   const Attrib& attrib = namedAttributeInstance.getAttribute();
   CString buf;

   switch (attrib.getValueType())
   {
   case VT_EMAIL_ADDRESS:
      buf = "mailto:";
      buf += attrib.getStringValue();
      break;
   case VT_WEB_ADDRESS:
      buf = "http://";
      buf += attrib.getStringValue();
      break;
   default:
      return;
   }

   SHELLEXECUTEINFO shellExecInfo;
   shellExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
   shellExecInfo.fMask = 0;
   shellExecInfo.hwnd = AfxGetMainWnd()->m_hWnd;
   shellExecInfo.lpVerb = "open";
   shellExecInfo.lpFile = buf;
   shellExecInfo.lpParameters = NULL;
   shellExecInfo.lpDirectory = NULL;
   shellExecInfo.nShow = SW_SHOW;
   ShellExecuteEx(&shellExecInfo);
}

void EditAttribs::OnNew() 
{
   DbFlag flg = 0;
   EditAttrib dlg;
   dlg.doc = doc;
   dlg.keywordName = "";
   dlg.m_neverMirror = 0;
   dlg.m_mirrorFlip = 0;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   dlg.m_visible = 0;
   dlg.layer = 0;
   dlg.penWidthIndex = 0;
   if (dlg.DoModal() != IDOK) return;

   WORD keyword = doc->IsKeyWord(dlg.keywordName, 0);

   int tempInt;
   double tempDouble;
   void *voidPtr;
   ValueTypeTag valueType = doc->getKeyWordArray()[keyword]->getValueType();

   switch (valueType)
   {
   case VT_NONE:
   default:
      voidPtr = NULL;
      break;
   case VT_STRING:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      voidPtr = dlg.m_value.GetBuffer(0);
      break;
   case VT_INTEGER:
      tempInt = atoi(dlg.m_value);
      voidPtr = &tempInt;
      break;
   case VT_DOUBLE:
   case VT_UNIT_DOUBLE:
      tempDouble = atof(dlg.m_value);
      voidPtr = &tempDouble;
      break;
   }

   m_map->addInstance(doc->getCamCadData(), keyword, valueType, voidPtr,
         atof(dlg.m_x),atof(dlg.m_y),DegToRad(atof(dlg.m_angle)),atof(dlg.m_height),atof(dlg.m_width),
         dlg.m_proportional != 0,0,dlg.m_visible != 0,0L,dlg.layer,dlg.m_neverMirror != 0, dlg.m_mirrorFlip != 0,
         horizontalPositionLeft, verticalPositionBaseline);

   FillListCtrl();
}

LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
   static const _TCHAR szThreeDots[] = _T("...");

   int nStringLen = lstrlen(lpszLong);

   if(nStringLen == 0 ||
      (pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
   {
      return(lpszLong);
   }

   static _TCHAR szShort[MAX_PATH];

   lstrcpy(szShort,lpszLong);
   int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

   for(int i = nStringLen-1; i > 0; i--)
   {
      szShort[i] = 0;
      if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen)
         <= nColumnLen)
      {
         break;
      }
   }

   lstrcat(szShort, szThreeDots);
   return(szShort);
}

#define OFFSET_FIRST    2
#define OFFSET_OTHER    6

void EditAttribs::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   COLORREF m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
   COLORREF m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
   COLORREF m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);
   COLORREF clrImage = m_clrBkgnd;
   COLORREF origTextColor, origBkColor;

   CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
   CRect rcItem(lpDrawItemStruct->rcItem);
   int nItem = lpDrawItemStruct->itemID;
   static _TCHAR szBuff[MAX_PATH];
   LPCTSTR pszText;

   // get item data
   LV_ITEM lvi;
   lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
   lvi.iItem = nItem;
   lvi.iSubItem = 0;
   lvi.pszText = szBuff;
   lvi.cchTextMax = sizeof(szBuff);
   lvi.stateMask = 0xFFFF;     // get all state flags
   m_list.GetItem(&lvi);

   BOOL bSelected = lvi.state & LVIS_SELECTED;

   // set colors if item is selected
   CRect rcLabel;
   m_list.GetItemRect(nItem, rcLabel, LVIR_LABEL);

   if (bSelected)
   {
      origTextColor = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      origBkColor = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

      pDC->FillRect(rcLabel, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
   }
   else
      pDC->FillRect(rcLabel, &CBrush(m_clrTextBk));

   // set color and mask for the icon
   UINT uiFlags = ILD_TRANSPARENT;
   if (lvi.state & LVIS_CUT)
   {
      clrImage = m_clrBkgnd;
      uiFlags |= ILD_BLEND50;
   }
   else if (bSelected)
   {
      clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
      uiFlags |= ILD_BLEND50;
   }

   // draw state icon
   CImageList* pImageList;
   UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
   if (nStateImageMask)
   {
      int nImage = (nStateImageMask>>12) - 1;
      pImageList = m_list.GetImageList(LVSIL_STATE);
      if (pImageList)
      {
         pImageList->Draw(pDC, nImage,
            CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
      }
   }

   // draw normal and overlay icon
   CRect rcIcon;
   m_list.GetItemRect(nItem, rcIcon, LVIR_ICON);

   pImageList = m_list.GetImageList(LVSIL_SMALL);
   if (pImageList)
   {
      UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
      if (rcItem.left<rcItem.right-1)
      {
         ImageList_DrawEx(pImageList->m_hImageList, lvi.iImage,
               pDC->m_hDC,rcIcon.left,rcIcon.top, 16, 16,
               m_clrBkgnd, clrImage, uiFlags | nOvlImageMask);
      }
   }

   // draw item label
   m_list.GetItemRect(nItem, rcItem, LVIR_LABEL);

   pszText = MakeShortString(pDC, szBuff,
            rcItem.right-rcItem.left, 2*OFFSET_FIRST);

   rcLabel = rcItem;
   rcLabel.left += OFFSET_FIRST;
   rcLabel.right -= OFFSET_FIRST;

   pDC->DrawText(pszText, -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

   // draw labels for extra columns
   LV_COLUMN lvc;
   lvc.mask = LVCF_FMT | LVCF_WIDTH;

   // set original colors if item was selected
   if (bSelected)
   {
      pDC->SetTextColor(origTextColor);
      pDC->SetBkColor(origBkColor);
   }

   Attrib *attrib = (Attrib*)lpDrawItemStruct->itemData;
   CFont *font, newFont;

   for(int nColumn = 1; m_list.GetColumn(nColumn, &lvc); nColumn++)
   {
      BOOL HotLink = (nColumn == 1 && (attrib->getValueType() == VT_EMAIL_ADDRESS || attrib->getValueType() == VT_WEB_ADDRESS));

      if (HotLink)
      {
         origTextColor = pDC->SetTextColor(RGB(0, 0, 255));
         font = GetFont();
         LOGFONT lf;
         font->GetLogFont(&lf);
         lf.lfUnderline = TRUE;
         newFont.CreateFontIndirect(&lf);
         pDC->SelectObject(&newFont);
      }

      rcItem.left = rcItem.right;
      rcItem.right += lvc.cx;

      int nRetLen = m_list.GetItemText(nItem, nColumn,
                  szBuff, sizeof(szBuff));
      if (nRetLen == 0)
         continue;

      pszText = MakeShortString(pDC, szBuff,
         rcItem.right - rcItem.left, 2*OFFSET_OTHER);

      UINT nJustify = DT_LEFT;

      if(pszText == szBuff)
      {
         switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
         {
         case LVCFMT_RIGHT:
            nJustify = DT_RIGHT;
            break;
         case LVCFMT_CENTER:
            nJustify = DT_CENTER;
            break;
         default:
            break;
         }
      }

      rcLabel = rcItem;
      rcLabel.left += OFFSET_OTHER;
      rcLabel.right -= OFFSET_OTHER;

      pDC->FillRect(rcLabel, &CBrush(m_clrTextBk));
      pDC->DrawText(pszText, -1, rcLabel,
         nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

      if (HotLink)
      {
         pDC->SetTextColor(origTextColor);
         pDC->SelectObject(font);
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// EditAttrib dialog
EditAttrib::EditAttrib(CWnd* pParent /*=NULL*/)
   : CDialog(EditAttrib::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditAttrib)
   m_value = _T("");
   m_keyword = _T("");
   m_angle = _T("0.00");
   m_height = _T("1.00");
   m_visible = FALSE;
   m_x = _T("0.00");
   m_y = _T("0.00");
   m_neverMirror = FALSE;
   m_mirrorFlip = FALSE;
   m_proportional = TRUE;
   m_width = _T("1.00");
   m_valuetype = _T("");
   //}}AFX_DATA_INIT

   m_horizontalPosition = horizontalPositionLeft;
   m_verticalPosition   = verticalPositionBaseline;
}

void EditAttrib::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditAttrib)
   DDX_Control(pDX, IDC_GROUP_LB, m_groupsLB);
   DDX_Control(pDX, IDC_PEN, m_penCB);
   DDX_Control(pDX, IDC_LAYERCB, m_layerCB);
   DDX_Control(pDX, IDC_Keyword, m_keywordCB);
   DDX_Text(pDX, IDC_VALUE, m_value);
   DDX_CBString(pDX, IDC_Keyword, m_keyword);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   DDX_Text(pDX, IDC_HEIGHT, m_height);
   DDX_Check(pDX, IDC_VISIBLE, m_visible);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Check(pDX, IDC_NEVER_MIRROR, m_neverMirror);
   DDX_Check(pDX, IDC_MIRROR_FLIP, m_mirrorFlip);
   DDX_Check(pDX, IDC_PROPORTIONAL, m_proportional);
   DDX_Text(pDX, IDC_WIDTH, m_width);
   DDX_Text(pDX, IDC_VALUETYPE, m_valuetype);
   DDX_Control(pDX, IDC_HorizontalAnchorPosition, m_horizontalAnchorPosition);
   DDX_Control(pDX, IDC_VerticalAnchorPosition, m_verticalAnchorPosition);
   //}}AFX_DATA_MAP
}

HorizontalPositionTag EditAttrib::getHorizontalPosition()
{
   return m_horizontalPosition;
}

void EditAttrib::setHorizontalPosition(HorizontalPositionTag horizontalPosition)
{
   m_horizontalPosition = horizontalPosition;
}

VerticalPositionTag EditAttrib::getVerticalPosition()
{
   return m_verticalPosition;
}

void EditAttrib::setVerticalPosition(VerticalPositionTag verticalPosition)
{
   m_verticalPosition = verticalPosition;
}

BEGIN_MESSAGE_MAP(EditAttrib, CDialog)
   //{{AFX_MSG_MAP(EditAttrib)
   ON_CBN_SELCHANGE(IDC_Keyword, OnSelchangeKeyword)
   ON_LBN_SELCHANGE(IDC_GROUP_LB, OnSelchangeGroupLb)
   //}}AFX_MSG_MAP
   ON_CBN_SELCHANGE(IDC_HorizontalAnchorPosition, OnCbnSelChangeHorizontalAnchorPosition)
   ON_CBN_SELCHANGE(IDC_VerticalAnchorPosition, OnCbnSelChangeVerticalAnchorPosition)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditAttrib message handlers
BOOL EditAttrib::OnInitDialog() 
{
   CDialog::OnInitDialog();

	int i=0;
   for (i=0; i<=AttribGroupTagMax; i++)
   {
      m_groupsLB.SetItemData(m_groupsLB.AddString(attribute_group[i]), i);
      m_groupsLB.SetSel(i);
      groups[i] = TRUE;
   }

   FillKeywords();

   for (i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *l = doc->getLayerArray()[i];
      if (!l) continue;
      int index = m_layerCB.AddString(l->getName());
      m_layerCB.SetItemData(index, i);
      if (i == layer)
         m_layerCB.SetCurSel(index);
   }

   FillWidthCB(&m_penCB, penWidthIndex, doc);

   m_horizontalAnchorPosition.setValue(m_horizontalPosition);
   m_verticalAnchorPosition.setValue(m_verticalPosition);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditAttrib::FillKeywords() 
{
   m_keywordCB.ResetContent();

   for (int i=0; i<doc->getKeyWordArray().GetCount(); i++)
   {
		const KeyWordStruct *kw = doc->getKeyWordArray()[i];
      if (groups[kw->getGroup()])
      {
         CString buf;
         buf.Format("%s (%s)", kw->cc, attribute_group[kw->group]);
         m_keywordCB.SetItemData(m_keywordCB.AddString(buf), i);
      }
   }

   m_keywordCB.SelectString(-1, keywordName);
}

void EditAttrib::OnSelchangeGroupLb() 
{
   int count = m_groupsLB.GetSelCount();
   int *array = (int*)calloc(count, sizeof(int));
   m_groupsLB.GetSelItems(count, array);

	int i=0; 
   for (i=0; i<=AttribGroupTagMax; i++)
      groups[i] = FALSE;

   for (i=0; i<count; i++)
      groups[array[i]] = TRUE;

   FillKeywords();   
}

void EditAttrib::OnSelchangeKeyword() 
{
   UpdateData();

   int sel = m_keywordCB.GetCurSel();
   if (sel == CB_ERR) return;
   const KeyWordStruct *kw = doc->getKeyWordArray()[m_keywordCB.GetItemData(sel)];
   m_valuetype = "Value Type : ";
   switch (kw->getValueType())
   {
   case VT_NONE:
      m_valuetype += "None";
      break;
   case VT_STRING:
      m_valuetype += "String";
      break;
   case VT_EMAIL_ADDRESS:
      m_valuetype += "Email Address";
      break;
   case VT_WEB_ADDRESS:
      m_valuetype += "Web Address";
      break;
   case VT_INTEGER:
      m_valuetype += "Integer";
      break;
   case VT_DOUBLE:
      m_valuetype += "Floating Point";
      break;
   case VT_UNIT_DOUBLE:
      m_valuetype += "Unit-Dependent Floating Point";
      break;
   }

   UpdateData(FALSE);
}

void EditAttrib::OnOK() 
{
   UpdateData();

   if (m_keyword.IsEmpty())
   {
      ErrorMessage("There must be a Keyword", "Error");
      return;
   }

   int i = m_keywordCB.GetCurSel();
   if (i == CB_ERR)
      keywordName = m_keyword;
   else
      keywordName = doc->getKeyWordArray()[m_keywordCB.GetItemData(i)]->getCCKeyword();

   int keywordIndex = doc->IsKeyWord(keywordName, 0);

   if (doc->getKeyWordArray()[keywordIndex]->getValueType() == VT_NONE && !m_value.IsEmpty())
   {
      ErrorMessage("Value Type is NONE, but you entered a value");
      return;
   }

   layer = m_layerCB.GetItemData(m_layerCB.GetCurSel());

   GetWidthIndex(&m_penCB, &penWidthIndex, doc);

   CDialog::OnOK();
}

void EditAttrib::OnCbnSelChangeHorizontalAnchorPosition()
{
   m_horizontalPosition = m_horizontalAnchorPosition.getValue();
}

void EditAttrib::OnCbnSelChangeVerticalAnchorPosition()
{
   m_verticalPosition   = m_verticalAnchorPosition.getValue();
}


/******************************************************************************
* CCEtoODBDoc::OnAttribVisibility
*/
void CCEtoODBDoc::OnAttribVisibility()
{
   AttribVisDlg dlg;
   dlg.doc = this;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// AttribVisDlg dialog
IMPLEMENT_DYNAMIC(AttribVisDlg, CDialog)
AttribVisDlg::AttribVisDlg(CWnd* pParent /*=NULL*/)
   : CDialog(AttribVisDlg::IDD, pParent)
   , m_visible(TRUE)
   , m_neverMirror(FALSE)
   , m_mirrorFlip(FALSE)
   , m_proportional(FALSE)
   , m_x(_T(""))
   , m_y(_T(""))
   , m_height(_T(""))
   , m_width(_T(""))
   , m_angle(_T("0"))
{
}

AttribVisDlg::~AttribVisDlg()
{
}

void AttribVisDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_Keyword, m_keywordCB);
   DDX_Control(pDX, IDC_GROUP_LB, m_groupsLB);
   DDX_Check(pDX, IDC_VISIBLE, m_visible);
   DDX_Check(pDX, IDC_NEVER_MIRROR, m_neverMirror);
   DDX_Check(pDX, IDC_MIRROR_FLIP, m_mirrorFlip);
   DDX_Check(pDX, IDC_PROPORTIONAL, m_proportional);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_HEIGHT, m_height);
   DDX_Text(pDX, IDC_WIDTH, m_width);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   DDX_Control(pDX, IDC_PEN, m_penCB);
   DDX_Control(pDX, IDC_LAYERCB, m_layerCB);
}


BEGIN_MESSAGE_MAP(AttribVisDlg, CDialog)
   ON_LBN_SELCHANGE(IDC_GROUP_LB, OnSelchangeGroupLb)
   ON_BN_CLICKED(IDOK, OnOK)
   ON_BN_CLICKED(IDC_HIDE, OnHideAll)
END_MESSAGE_MAP()


// AttribVisDlg message handlers
BOOL AttribVisDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   double size = 0.1 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   m_height.Format("%.*lf", decimals, size);
   m_width.Format("%.*lf", decimals, size/2);
   m_x.Format("%.*lf", decimals, 0);
   m_y.Format("%.*lf", decimals, 0);
   UpdateData(FALSE);

	int i=0;
   for (i=0; i<=AttribGroupTagMax; i++)
   {
      m_groupsLB.SetItemData(m_groupsLB.AddString(attribute_group[i]), i);
      m_groupsLB.SetSel(i);
      groups[i] = TRUE;
   }

   FillKeywords();

   for (i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (!layer) continue;
      int index = m_layerCB.AddString(layer->getName());
      m_layerCB.SetItemData(index, i);
   }

   FillWidthCB(&m_penCB, 0, doc);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void AttribVisDlg::FillKeywords() 
{
   m_keywordCB.ResetContent();

   for (int i=0; i<doc->getKeyWordArray().GetCount(); i++)
   {
		const KeyWordStruct *kw = doc->getKeyWordArray()[i];
      if (groups[kw->getGroup()])
      {
         CString buf;
         buf.Format("%s (%s)", kw->cc, attribute_group[kw->group]);
         m_keywordCB.SetItemData(m_keywordCB.AddString(buf), i);
      }
   }
}

void AttribVisDlg::OnSelchangeGroupLb()
{
   int count = m_groupsLB.GetSelCount();
   int *array = (int*)calloc(count, sizeof(int));
   m_groupsLB.GetSelItems(count, array);

	int i=0; 
   for (i=0; i<=AttribGroupTagMax; i++)
      groups[i] = FALSE;

   for (i=0; i<count; i++)
      groups[array[i]] = TRUE;

   FillKeywords();   
}

void AttribVisDlg::OnOK()
{
   UpdateData();
   
   int i = m_keywordCB.GetCurSel();

   if (i == CB_ERR)
   {
      ErrorMessage("Select a Keyword", "Error");
      return;
   }

   CString keywordString = doc->getKeyWordArray()[m_keywordCB.GetItemData(i)]->getCCKeyword();
   WORD keyword = doc->IsKeyWord(keywordString, 0);

   i = m_layerCB.GetCurSel();

   if (i == CB_ERR)
   {
      ErrorMessage("You must select a Layer", "Error");
      return;
   }

   int layer = m_layerCB.GetItemData(i);

   short penWidthIndex;
   GetWidthIndex(&m_penCB, &penWidthIndex, doc);

   Attrib* attrib;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getAttributesRef() == NULL)
            continue;

         if (data->getAttributesRef()->Lookup(keyword, attrib))
         {
            if (!m_visible)
            {
               attrib->setVisibleAllInstances(false);
            }
            else
            {
               attrib->setProperties(atof(m_x),atof(m_y),DegToRad(atof(m_angle)),atof(m_height),
                  atof(m_width),penWidthIndex,m_proportional,
                  TRUE,m_neverMirror,m_mirrorFlip,0,horizontalPositionLeft,verticalPositionBaseline,layer,0);
            }
         }         
      }
   }

   doc->UpdateAllViews(NULL);
}

void AttribVisDlg::OnHideAll()
{
   WORD keyword;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;
      
      for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getAttributesRef() == NULL)
            continue;
         
         for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
         {
            Attrib* attrib;
            data->getAttributesRef()->GetNextAssoc(attribPos, keyword, attrib);

            attrib->setVisibleAllInstances(false);
         }
      }
   }

   doc->UpdateAllViews(NULL);
}
