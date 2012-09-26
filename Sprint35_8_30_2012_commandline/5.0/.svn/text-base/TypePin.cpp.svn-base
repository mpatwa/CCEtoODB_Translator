// $Header: /CAMCAD/4.5/TypePin.cpp 12    4/28/06 11:37a Kurt Van Ness $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

#include "stdafx.h"
#include "typepin.h"

void CCEtoODBDoc::OnTypePinEditor() 
{
   ListTypePin dlg;
   dlg.doc = this;
   dlg.DoModal(); 
}

/////////////////////////////////////////////////////////////////////////////
// ListTypePin dialog
ListTypePin::ListTypePin(CWnd* pParent /*=NULL*/)
   : CDialog(ListTypePin::IDD, pParent)
{
   //{{AFX_DATA_INIT(ListTypePin)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void ListTypePin::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ListTypePin)
   DDX_Control(pDX, IDC_TREE, m_tree);
   DDX_Control(pDX, IDC_FILE, m_fileCB);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ListTypePin, CDialog)
   //{{AFX_MSG_MAP(ListTypePin)
   ON_CBN_SELCHANGE(IDC_FILE, OnSelchangeFile)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define iType        0
#define iTypePin     1
/////////////////////////////////////////////////////////////////////////////
// ListTypePin message handlers
BOOL ListTypePin::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 3, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_TYPE));
   m_imageList->Add(app->LoadIcon(IDI_TYPEPIN));
   m_imageList->Add(app->LoadIcon(IDI_TYPEPINA));
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   // Files
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->getTypeList().IsEmpty())
         m_fileCB.SetItemDataPtr(m_fileCB.AddString(file->getName()), file);
   }
   m_fileCB.SetCurSel(0); // select first file with nets
   FillTree();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ListTypePin::OnSelchangeFile() 
{
   FillTree();
}

void ListTypePin::FillTree() 
{
   CWaitCursor wait;

   m_tree.DeleteAllItems();

   int sel = m_fileCB.GetCurSel();
   if (sel == CB_ERR) return;
   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(sel);

   POSITION typePos, typePinPos;
   TypeStruct *type;
   TypePinStruct *typePin;
   HTREEITEM typeRoot, item = NULL;

   // TypeList
   if (!file->getTypeList().IsEmpty())
   {
      typePos = file->getTypeList().GetHeadPosition();
      while (typePos != NULL)
      {
         type = file->getTypeList().GetNext(typePos);

         typeRoot = m_tree.InsertItem(type->getName(), iType, iType, TVI_ROOT, TVI_SORT);
         m_tree.SetItemData(typeRoot, NULL);

         // typePinList
         typePinPos = type->getTypePinList().GetHeadPosition();
         while (typePinPos != NULL)
         {
            typePin = type->getTypePinList().GetNext(typePinPos);

            int icon = iTypePin;
            item = m_tree.InsertItem(typePin->getPhysicalPinName(), icon, icon, typeRoot, TVI_SORT);
            m_tree.SetItemData(item, (DWORD)typePin);
         }
      }
   }
}

void ListTypePin::OnEdit() 
{
   HTREEITEM item;
   TypePinStruct *tp;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   tp = (TypePinStruct*)m_tree.GetItemData(item); 

   if (tp == NULL) return; // non-data item (folder, etc.)

   EditTypePin dlg;
   dlg.m_physpin = tp->getPhysicalPinName();
   dlg.m_logipin = tp->getLogicalPinName();
   dlg.m_gatenr = tp->getGateNumber();
   dlg.m_swap = tp->getSwap();
   dlg.m_pintype = tp->getPinType();
   if (dlg.DoModal() == IDOK)
   {
      tp->setPhysicalPinName( dlg.m_physpin);
      tp->setLogicalPinName( dlg.m_logipin);
      tp->setGateNumber( dlg.m_gatenr);
      tp->setSwap( dlg.m_swap);
      tp->setPinType( dlg.m_pintype);
   }
}

void ListTypePin::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   *pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
// EditTypePin dialog


EditTypePin::EditTypePin(CWnd* pParent /*=NULL*/)
   : CDialog(EditTypePin::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditTypePin)
   m_physpin = _T("");
   m_logipin = _T("");
   m_gatenr = 0;
   m_swap = 0;
   m_pintype = 0;
   //}}AFX_DATA_INIT
}


void EditTypePin::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditTypePin)
   DDX_Text(pDX, IDC_PHYSPIN, m_physpin);
   DDX_Text(pDX, IDC_LOGIPIN, m_logipin);
   DDX_Text(pDX, IDC_GATENR, m_gatenr);
   DDX_Text(pDX, IDC_SWAP, m_swap);
   DDX_Text(pDX, IDC_PINTYPE, m_pintype);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditTypePin, CDialog)
   //{{AFX_MSG_MAP(EditTypePin)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditTypePin message handlers
