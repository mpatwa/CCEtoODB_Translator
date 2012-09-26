// $Header: /CAMCAD/4.6/CompPin.cpp 35    3/27/07 3:41p Lynn Phung $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "CompPin.h"
#include "ccview.h"
#include "atrblist.h"
#include ".\comppin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SelectNets *selectNetsDlg; // from NETS.CPP

ListCompPin *CPdlg = NULL;

/******************************************************************************
* OnCompPinAttribs
*/
void CCEtoODBDoc::OnCompPinAttribs() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Query or Edit!");
      return;
   }*/

   if (nothingIsSelected())
   {
      ErrorMessage("Select a pin and try again", "No pin selected");
      return;
   }

   DataStruct *compInsert, *pinInsert;
   compInsert = getSelectStack().getAt(0)->getData();
   pinInsert  = getSelectStack().getAt(1)->getData();

   bool IsPin = (compInsert->isInsertType(insertTypePcbComponent) && pinInsert->isInsertType(insertTypePin));
   bool IsDiePin = (compInsert->isInsertType(insertTypeDie) && pinInsert->isInsertType(insertTypeDiePin));
   if ( !IsPin && !IsDiePin )
   {
      ErrorMessage("Select a pin/Die pin and try again", "No pin selected");
      return;
   }

   FileStruct *file = Find_File(getSelectStack().getAt(0)->filenum);

   BOOL Found = FALSE;
	NetStruct *net = NULL;
   CompPinStruct *cp = NULL;
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL && !Found)
   {
      net = file->getNetList().GetNext(netPos);
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL && !Found)
      {
         cp = net->getNextCompPin(compPinPos);

         if (!cp->getRefDes().CompareNoCase(compInsert->getInsert()->getRefname()) && !cp->getPinName().CompareNoCase(pinInsert->getInsert()->getRefname()))
            Found = TRUE;
      }
   }

   if (!Found)
   {
      ErrorMessage("May need to generate NetList.", "Comp/Pin not found in NetList");
      return;
   }

   EditAttribs dlg;
   dlg.doc = this;
	dlg.m_item.Format("%s %s    NET: %s", cp->getRefDes(), cp->getPinName(), net->getNetName());
   dlg.setMap(cp->getAttributesRef());
   BOOL Empty = (cp->getAttributesRef() == NULL);
   dlg.DoModal();
}

/******************************************************************************
* OnCompPinEditor
*/
void CCEtoODBDoc::OnCompPinEditor() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Query or Edit!");
      return;
   }*/

   if (CPdlg)
      return;

   CPdlg = new ListCompPin;
   CPdlg->doc = this;
   CPdlg->Create(IDD_LIST_COMP_PIN);
}


/////////////////////////////////////////////////////////////////////////////
// ListCompPin dialog
ListCompPin::ListCompPin(CWnd* pParent /*=NULL*/)
   : CResizingDialog(ListCompPin::IDD, pParent)
{
   //{{AFX_DATA_INIT(ListCompPin)
   m_comp = _T("");
   m_pin = _T("");
   //}}AFX_DATA_INIT

   FillingTree = TRUE;
   lastNet = NULL;

   addFieldControl(IDC_FILE         ,anchorLeft,growHorizontal);
   addFieldControl(IDC_TREE         ,anchorLeft,growBoth);
   addFieldControl(IDC_NET          ,anchorBottom);
   addFieldControl(IDC_STATIC1      ,anchorBottom);
   addFieldControl(IDC_STATIC2      ,anchorBottom);
   addFieldControl(IDC_COMP         ,anchorBottom);
   addFieldControl(IDC_PIN          ,anchorBottom);
   addFieldControl(IDC_FIND_COMP_PIN,anchorBottom);
   addFieldControl(IDC_STATIC3      ,anchorBottom);
   addFieldControl(IDC_EDIT         ,anchorBottom);
   addFieldControl(IDC_PAN          ,anchorBottom);
   addFieldControl(IDC_ATTRIBS      ,anchorBottom);
   addFieldControl(IDC_CLEAR        ,anchorBottom);

}

ListCompPin::~ListCompPin()
{
   if (m_imageList)
      delete m_imageList;
}

void ListCompPin::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ListCompPin)
   DDX_Control(pDX, IDC_NET, m_netButton);
   DDX_Control(pDX, IDC_FILE, m_fileCB);
   DDX_Control(pDX, IDC_TREE, m_tree);
   DDX_Text(pDX, IDC_COMP, m_comp);
   DDX_Text(pDX, IDC_PIN, m_pin);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ListCompPin, CResizingDialog)
   //{{AFX_MSG_MAP(ListCompPin)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_CBN_SELCHANGE(IDC_FILE, OnSelchangeFile)
   ON_BN_CLICKED(IDC_FIND_COMP_PIN, OnFindCompPin)
   ON_BN_CLICKED(IDC_PAN, OnPan)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_NET, OnNet)
   //}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
END_MESSAGE_MAP()

void ListCompPin::OnCancel() 
{
   OnBnClickedClear();
	FreeItems();
   DestroyWindow();  
}

void ListCompPin::PostNcDestroy() 
{
   delete this;
   CPdlg = NULL;
   CResizingDialog::PostNcDestroy();
}

void ListCompPin::FreeItems() 
{
   TempStruct *memStruct;

   HTREEITEM root = m_tree.GetRootItem();
   while (root != NULL)
   {
      memStruct = (TempStruct*)m_tree.GetItemData(root);
      
      if (memStruct)
         delete memStruct;

      HTREEITEM item = m_tree.GetChildItem(root);
      while (item != NULL)
      {
         memStruct = (TempStruct*)m_tree.GetItemData(item);
      
         if (memStruct)
            delete memStruct;
      
         item = m_tree.GetNextItem(item, TVGN_NEXT);
      } 
      root = m_tree.GetNextItem(root, TVGN_NEXT);
   }
}

#define iNet         0
#define iCP          2
BOOL ListCompPin::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 4, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_NET));
   m_imageList->Add(app->LoadIcon(IDI_NETA));
   m_imageList->Add(app->LoadIcon(IDI_CP));
   m_imageList->Add(app->LoadIcon(IDI_CPA));
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   StartedSearch = FALSE;
   Generated = FALSE;
   item = NULL;   // just in case there is no netlist.   

   // Files
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())  continue;

      if (!file->getNetList().IsEmpty())
      {
         m_fileCB.SetItemDataPtr(m_fileCB.AddString(file->getName()), file);
      }
   }
   m_fileCB.SetCurSel(0); // select first file with nets
   FillTree();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ListCompPin::FillTree() 
{
   CWaitCursor wait;

   FreeItems();
   m_tree.DeleteAllItems();

   int sel = m_fileCB.GetCurSel();
   if (sel == CB_ERR) return;
   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(sel);

   POSITION netPos, compPinPos;
   NetStruct *net;
   CompPinStruct *compPin;
   HTREEITEM netRoot, item = NULL;
   TempStruct *memStruct;
   CString buf;

   // NetList
   if (!file->getNetList().IsEmpty())
   {
      netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         net = file->getNetList().GetNext(netPos);

         int icon = iNet;
         if (net->getAttributesRef() && !net->getAttributesRef()->IsEmpty()) icon++;
         netRoot = m_tree.InsertItem(net->getNetName(), icon, icon, TVI_ROOT, TVI_SORT);
         memStruct = new TempStruct;
         memStruct->cp = FALSE;
         memStruct->voidPtr = net;
         m_tree.SetItemData(netRoot, (DWORD)memStruct);

         // CompPinList
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);

            buf.Format("%s %s", compPin->getRefDes(), compPin->getPinName());
            int icon = iCP;
            if (compPin->getAttributesRef() && !compPin->getAttributesRef()->IsEmpty()) icon++;
            item = m_tree.InsertItem(buf, icon, icon, netRoot, TVI_SORT);
            memStruct = new TempStruct;
            memStruct->cp = TRUE;
            memStruct->voidPtr = compPin;
            m_tree.SetItemData(item, (DWORD)memStruct);
         }
      }
   }

   FillingTree = FALSE;
}

void ListCompPin::OnSelchangeFile() 
{
   FillingTree = TRUE;
   StartedSearch = FALSE;
   FillTree();
}

void ListCompPin::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   *pResult = 0;

   m_netButton.SetWindowText("");
                    
   if (FillingTree)
      return;

   HTREEITEM item = pNMTreeView->itemNew.hItem;
   if (!item)
      return;

   TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item); 
   if (!memStruct)
      return;

   if (memStruct->cp)
   {
      item = m_tree.GetParentItem(item);
      if (!item)
         return;

      memStruct = (TempStruct*)m_tree.GetItemData(item); 
   }

   if (memStruct)
   {
      NetStruct *net = (NetStruct*)memStruct->voidPtr;
		m_netButton.EnableWindow(true);
      m_netButton.SetWindowText(net->getNetName());
   }
}

void ListCompPin::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();   
   *pResult = 0;
}

void ListCompPin::OnEdit() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   HTREEITEM item;
   CompPinStruct *cp;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item); 
   if (!memStruct || !memStruct->cp) return; // non-data item (folder, etc.)

   cp = (CompPinStruct*)memStruct->voidPtr; 

   EditCompPin dlg;
   dlg.cp = cp;
   dlg.decimals = GetDecimals(doc->getSettings().getPageUnits());
   dlg.doc = doc;
   dlg.DoModal();
}

void ListCompPin::OnPan() 
{
   HTREEITEM item;
   CompPinStruct *cp;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item); 
   if (!memStruct || !memStruct->cp) return; // non-data item (folder, etc.)

   cp = (CompPinStruct*)memStruct->voidPtr; 

   PanTo(cp);
}

void ListCompPin::OnAttribs() 
{
   HTREEITEM item;
   CAttributes** map;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item); 
   if (!memStruct) return; // non-data item (folder, etc.)
   if (memStruct->cp)
   {
      CompPinStruct *cp = (CompPinStruct*)memStruct->voidPtr;
      map = &cp->getAttributesRef();
   }
   else
   {
      NetStruct *net = (NetStruct*)memStruct->voidPtr;
      map = &net->getAttributesRef();
   }

   EditAttribs dlg;
   dlg.doc = doc;
   dlg.m_item = m_tree.GetItemText(item);
   dlg.setMap(*map);
   BOOL Empty = (*map == NULL);

   if (dlg.DoModal() == IDOK)
   {
      if (Empty && (*map != NULL)) // if map was empty and user added attributes, then update icon
      {
         int image, selImage;
         m_tree.GetItemImage(item, image, selImage);
         m_tree.SetItemImage(item, image+1, image+1); 
      }

      if (!Empty && (*map == NULL)) // if map was not empty and user removed all attributes, then update icon
      {
         int image, selImage;
         m_tree.GetItemImage(item, image, selImage);
         m_tree.SetItemImage(item, image-1, image-1); 
      }

   }
}

void ListCompPin::OnFindCompPin() 
{
   UpdateData();

   if (StartedSearch && !compName.CompareNoCase(m_comp) && !pinName.CompareNoCase(m_pin))
      FindNextCompPin();
   else
   {
      StartedSearch = TRUE;
      compName = m_comp;
      pinName = m_pin;

      FindFirstCompPin();
   }
}

void ListCompPin::FindFirstCompPin()
{
   if (m_comp.IsEmpty())
   {
      ErrorMessage("No Comp Name to find");
      return;
   }

   root = m_tree.GetRootItem();
   while (root != NULL)
   {
      item = m_tree.GetChildItem(root);
      while (item != NULL)
      {
         TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item);
         if (!memStruct || !memStruct->cp) continue;
         CompPinStruct *compPin = (CompPinStruct*)memStruct->voidPtr;

      
         if (!compPin->getRefDes().CompareNoCase(compName) && 
               (pinName.IsEmpty() || !compPin->getPinName().CompareNoCase(pinName)) )
         {
            m_tree.Select(item, TVGN_CARET);
            return;
         }        
      
         item = m_tree.GetNextItem(item, TVGN_NEXT);
      } 
      root = m_tree.GetNextItem(root, TVGN_NEXT);
   }

   // comp / pin not found

   // check if valid comp name
   int sel = m_fileCB.GetCurSel();
   if (sel == CB_ERR) return;
   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(sel);

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT && 
            data->getInsert()->getRefname() && !compName.CompareNoCase(data->getInsert()->getRefname()))
      {
         // comp name is OK
         if (Generated || doc->DontAskRegenPinLocs)
            ErrorMessage("Comp/Pin Not Found");
         else 
         {
            if (ErrorMessage("Would you like to regenerate NC Net / Pic Locs?", "Comp/Pin Not Found", MB_YESNO) == IDYES)
            {
               Generated = TRUE;
               doc->OnGeneratePinloc();
               FillTree();
               FindFirstCompPin();
            }
         }
         return;
      }
   }

   // bad comp name
   ErrorMessage(compName, "Component Name does not exist in this file");
}

void ListCompPin::FindNextCompPin() 
{
   CompPinStruct *compPin;
   TempStruct *memStruct;

   if (item != NULL) // if no netlist is there
      item = m_tree.GetNextItem(item, TVGN_NEXT);

   while (root != NULL)
   {
      while (item != NULL)
      {
         memStruct = (TempStruct*)m_tree.GetItemData(item); 
         if (!memStruct || !memStruct->cp) continue; // non-data item (folder, etc.)
         compPin = (CompPinStruct*)memStruct->voidPtr; 
      
         if (!compPin->getRefDes().CompareNoCase(compName) && 
            (pinName.IsEmpty() || !compPin->getPinName().CompareNoCase(pinName)) )
         {
            m_tree.Select(item, TVGN_CARET);
            return;
         }        
   
         item = m_tree.GetNextItem(item, TVGN_NEXT);
      } 
      root = m_tree.GetNextItem(root, TVGN_NEXT);
      if (root != NULL)
         item = m_tree.GetChildItem(root);
   } 
   
   FindFirstCompPin();
}

void ListCompPin::PanTo(CompPinStruct *cp) 
{
   if (!cp->getPinCoordinatesComplete())
   {
      if (Generated || doc->DontAskRegenPinLocs)
      {
         ErrorMessage("No coordinate location for this Comp/Pin.");
         return;
      }
      else if (ErrorMessage("No coordinate location for this Comp/Pin.", "Would you like to generate Pin Locations?", MB_YESNO) == IDYES)
      {
         Generated = TRUE;
         doc->OnGeneratePinloc();
      }
      else
         return;
   }

   int sel = m_fileCB.GetCurSel();
   if (sel == CB_ERR) return;
   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(sel);

   Mat2x2 m;
   RotMat2(&m, file->getRotation());
   Point2 point2;
   point2.x = cp->getOriginX() * file->getScale();
   point2.y = cp->getOriginY() * file->getScale();
   int mirror = file->getResultantMirror(doc->getBottomView());
   if (mirror & MIRROR_FLIP)  point2.x = -point2.x;
   TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

   POSITION pos = doc->GetFirstViewPosition();
   CCEtoODBView *view;
   while (pos != NULL)
   {
      view = (CCEtoODBView *)doc->GetNextView(pos);
      view->PanCoordinate(point2.x, point2.y, TRUE);
   }
}

void ListCompPin::OnNet() 
{
   HTREEITEM item = m_tree.GetSelectedItem();
   if (!item)
      return;

   TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item); 
   if (!memStruct)
      return;

   if (memStruct->cp)
   {
      item = m_tree.GetParentItem(item);
      if (!item)
         return;

      memStruct = (TempStruct*)m_tree.GetItemData(item); 
   }

   if (memStruct)
   {
      if (lastNet)
      {
         lastNet->setHighlighted(true);
         int value = doc->RegisterValue(lastNet->getNetName());
         doc->UnhighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, value);
      }

      NetStruct *net = (NetStruct*)memStruct->voidPtr;

      net->setHighlighted(true);
      int value = doc->RegisterValue(net->getNetName());
      doc->HighlightedNetsMap.SetAt(value, RGB(255, 255, 255));

      doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, value);
      //doc->ColorNet(net, RGB(255, 255, 255)); 
      
      if (selectNetsDlg)
         selectNetsDlg->Refresh();

      lastNet = net;
   }
}

/////////////////////////////////////////////////////////////////////////////
// EditCompPin dialog
EditCompPin::EditCompPin(CWnd* pParent /*=NULL*/)
   : CDialog(EditCompPin::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditCompPin)
   m_x = _T("");
   m_y = _T("");
   m_angle = _T("");
   m_visibleBottom = FALSE;
   m_visibleTop = FALSE;
   m_mirrorGraphics = FALSE;
   m_mirrorLayers = FALSE;
   //}}AFX_DATA_INIT
}

void EditCompPin::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditCompPin)
   DDX_Control(pDX, IDC_BLOCK, m_blockCB);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   DDX_Check(pDX, IDC_VISIBLE_BOTTOM, m_visibleBottom);
   DDX_Check(pDX, IDC_VISIBLE_TOP, m_visibleTop);
   DDX_Check(pDX, IDC_MIRROR_GRAPHICS, m_mirrorGraphics);
   DDX_Check(pDX, IDC_MIRROR_LAYERS, m_mirrorLayers);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditCompPin, CDialog)
   //{{AFX_MSG_MAP(EditCompPin)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditCompPin message handlers
BOOL EditCompPin::OnInitDialog() 
{
   CDialog::OnInitDialog();

   int index = m_blockCB.AddString("* NONE *");
   m_blockCB.SetItemData(index, -1);
   m_blockCB.SetCurSel(index);

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         index = m_blockCB.AddString(block->getName());

         m_blockCB.SetItemData(index, block->getBlockNumber());

         if (cp->getPadstackBlockNumber() == block->getBlockNumber())
            m_blockCB.SetCurSel(index);
      }
   }

   if (cp->getPinCoordinatesComplete())
   {
      m_x.Format("%.*lf", decimals, cp->getOriginX());
      m_y.Format("%.*lf", decimals, cp->getOriginY());
		m_angle.Format("%.2lf", normalizeDegrees(cp->getRotationDegrees()));
		m_mirrorGraphics = cp->getMirror() & MIRROR_FLIP;
      m_mirrorLayers = cp->getMirror() & MIRROR_LAYERS;
      m_visibleTop    = cp->isVisibleTop();
      m_visibleBottom = cp->isVisibleBottom();
      UpdateData(FALSE);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditCompPin::OnOK() 
{
   UpdateData();

   if (!m_x.GetLength() && !m_y.GetLength())
   {
      cp->setOrigin(0.,0.);
      cp->setRotationRadians(0.0);
      cp->setMirror(0);
      cp->setVisible(0);
      cp->setPinCoordinatesComplete(false);
   }
   else
   {
      cp->setOrigin(atof(m_x),atof(m_y));
      cp->setRotationDegrees( atof(m_angle));
      cp->setMirror( (m_mirrorGraphics?MIRROR_FLIP:0) | (m_mirrorLayers?MIRROR_LAYERS:0));
      cp->setVisible(0);
      if (m_visibleTop)
         cp->setVisibleBits(VISIBLE_TOP);
      if (m_visibleBottom)
         cp->setVisibleBits(VISIBLE_BOTTOM);
      cp->setPinCoordinatesComplete(true);
   }

   cp->setPadstackBlockNumber(m_blockCB.GetItemData(m_blockCB.GetCurSel()));

   CDialog::OnOK();
}



void ListCompPin::OnBnClickedClear()
{
	HTREEITEM item = m_tree.GetSelectedItem();
   if (!item)
      return;

   TempStruct *memStruct = (TempStruct*)m_tree.GetItemData(item); 
   if (!memStruct)
      return;

   if (memStruct->cp)
   {
      item = m_tree.GetParentItem(item);
      if (!item)
         return;

      memStruct = (TempStruct*)m_tree.GetItemData(item); 
   }

   if (memStruct)
   {
      if (lastNet)
      {
         lastNet->setHighlighted(false);
         int value = doc->RegisterValue(lastNet->getNetName());
         doc->UnhighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, value);
      }

     
   }
}
