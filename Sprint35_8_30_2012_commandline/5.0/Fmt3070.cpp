// $Header: /CAMCAD/4.4/Fmt3070.cpp 9     3/15/04 7:10a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Fmt3070.h"
#include "attrib.h"
#include "ck.h"
#include "ccdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// PartNumbersDlg dialog
IMPLEMENT_DYNAMIC(PartNumbersDlg, CDialog)
PartNumbersDlg::PartNumbersDlg(CWnd* pParent /*=NULL*/)
   : CDialog(PartNumbersDlg::IDD, pParent)
{
}

PartNumbersDlg::~PartNumbersDlg()
{
}

void PartNumbersDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(PartNumbersDlg, CDialog)
   ON_BN_CLICKED(IDC_RENAME, OnRename)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
END_MESSAGE_MAP()

// PartNumbersDlg message handlers
BOOL PartNumbersDlg::OnInitDialog( )
{
   CDialog::OnInitDialog();
   
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 2;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.pszText = "Invalid Partnumber";
   column.iSubItem = 0;
   column.cx = width;
   m_list.InsertColumn(0, &column);

   column.pszText = "New Partnumber";
   column.iSubItem = 1;
   column.cx = width;
   m_list.InsertColumn(1, &column);

   int i=0;

   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_PARTNUMBER, 2);
      CString origPartnumber = get_attvalue_string(doc, attrib);
      CString newPartnumber = check_name('b', origPartnumber);

      CString temp;
      if (!partnumberMap.Lookup(origPartnumber, temp))
         partnumberMap.SetAt(origPartnumber, newPartnumber);
   }

   POSITION mapPos = partnumberMap.GetStartPosition();
   while (mapPos)
   {
      CString origPartnumber, newPartnumber;
      POSITION tempPos = mapPos;
      partnumberMap.GetNextAssoc(mapPos, origPartnumber, newPartnumber);

      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = i++;
      item.iSubItem = 0;
      item.pszText = origPartnumber.GetBuffer(0);
      int actualItem = m_list.InsertItem(&item);
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

      m_list.SetItemData(actualItem, (LPARAM)tempPos);

      item.iItem = actualItem;
      item.iSubItem = 1;
      item.pszText = newPartnumber.GetBuffer(0);
      m_list.SetItem(&item);
   }

   return TRUE;   // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void PartNumbersDlg::OnRename()
{
   // get selected geometry
   int count = m_list.GetItemCount();
   BOOL Selected = FALSE;
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
      {
         Selected = TRUE;
         break;
      }
   }
   if (!Selected)
      return;


   POSITION mapPos = (POSITION)m_list.GetItemData(selItem);

   CString origPartnumber, newPartnumber;

   partnumberMap.GetNextAssoc(mapPos, origPartnumber, newPartnumber);


   InputDlg dlg;
   dlg.m_prompt = origPartnumber;
   dlg.m_input = newPartnumber;
   if (dlg.DoModal() != IDOK)
      return;
   
   LV_ITEM item;
   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 1;
   item.pszText = dlg.m_input.GetBuffer(0);
   m_list.SetItem(&item);

   WORD keyword = doc->IsKeyWord(ATT_PARTNUMBER, 1);

   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_PARTNUMBER, 2);
      CString partnumber = get_attvalue_string(doc, attrib);
      if (!partnumber.Compare(origPartnumber))
         doc->SetAttrib(&data->getAttributesRef(), keyword, VT_STRING, dlg.m_input.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

void PartNumbersDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
   OnRename();

   *pResult = 0;
}
