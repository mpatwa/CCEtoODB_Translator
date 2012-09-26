// $Header: /CAMCAD/4.3/Export.cpp 8     8/12/03 3:53p Kurt Van Ness $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "export.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int SortBy;
static BOOL Reverse;

/////////////////////////////////////////////////////////////////////////////
// TakayaOutput dialog
TakayaOutput::TakayaOutput(CWnd* pParent /*=NULL*/)
   : CDialog(TakayaOutput::IDD, pParent)
{
   //{{AFX_DATA_INIT(TakayaOutput)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void TakayaOutput::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TakayaOutput)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TakayaOutput, CDialog)
   //{{AFX_MSG_MAP(TakayaOutput)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TakayaOutput message handlers
BOOL TakayaOutput::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   SortBy = -1;
   Reverse = FALSE;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 20;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "File#";
   column.iSubItem = 0;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Surface";
   column.iSubItem = 2;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Refname";
   column.iSubItem = 3;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "TRefname";
   column.iSubItem = 4;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Val1";
   column.iSubItem = 5;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Val2";
   column.iSubItem = 6;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "*";
   column.iSubItem = 7;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "RD";
   column.iSubItem = 8;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 3;
   column.pszText = "XY";
   column.iSubItem = 9;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 3;
   column.pszText = "XY";
   column.iSubItem = 10;
   m_list.InsertColumn(column.iSubItem, &column);

   LV_ITEM item;

   // items
   int j = 0;
   int actualItem;
   for (int i=0; i<count; i++)
   {  
      TKoutput *tk = array->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = "";
      m_list.SetItem(&item);

      actualItem = m_list.InsertItem(&item);
//    if (actualItem == -1) ErrorMessage("Insert Failed");
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

      m_list.SetItemData(actualItem, (LPARAM)tk);

      FillItem(actualItem, tk);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void TakayaOutput::FillItem(int actualItem, TKoutput *tk)
{
   LV_ITEM item;
   CString buf;

   item.mask = LVIF_TEXT;
   item.iItem = actualItem;

   item.iSubItem = 0;
   buf.Format("%d", tk->filenumber);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (tk->onoff)
      item.pszText = "ON";
   else
      item.pszText = "off";
   m_list.SetItem(&item);

   item.iSubItem = 2;
   if (!tk->topbottom)
      item.pszText = "TOP";
   else
      item.pszText = "BOT";
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = tk->refname.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = tk->tkrefname.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 5;
   item.pszText = tk->value.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 6;
   item.pszText = tk->comment.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 7;
   item.pszText = tk->location.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 8;
   item.pszText = tk->element.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 9;
   buf.Format("(%ld,%ld)", tk->x1, tk->y1);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 10;
   buf.Format("(%ld,%ld)", tk->x2, tk->y2);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);
}

void TakayaOutput::OnEdit() 
{
   // get selected geometry
   int count = m_list.GetItemCount();

   if (!count)
      return;

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

   TKoutput *tk = (TKoutput*)m_list.GetItemData(selItem);

   TAKAYAEdit dlg;
   dlg.m_onoff = tk->onoff;
   dlg.m_topbottom = tk->topbottom;
   dlg.m_refname = tk->refname;
   dlg.m_trefname = tk->tkrefname;
   dlg.m_val1 = tk->value;
   dlg.m_val2 = tk->comment;
   dlg.m_star = tk->location;
   dlg.m_rd = tk->element;
   dlg.m_x1.Format("%ld", tk->x1);
   dlg.m_y1.Format("%ld", tk->y1);
   dlg.m_x2.Format("%ld", tk->x2);
   dlg.m_y2.Format("%ld", tk->y2);
   if (dlg.DoModal() == IDOK)
   {
      tk->onoff = dlg.m_onoff;
      tk->topbottom = dlg.m_topbottom;
      tk->refname = dlg.m_refname;
      tk->tkrefname = dlg.m_trefname;
      tk->value = dlg.m_val1;
      tk->comment = dlg.m_val2;
      tk->location = dlg.m_star;
      tk->element = dlg.m_rd;
      tk->x1 = atol(dlg.m_x1);
      tk->y1 = atol(dlg.m_y1);
      tk->x2 = atol(dlg.m_x2);
      tk->y2 = atol(dlg.m_y2);

      FillItem(selItem, tk);
   }
}

void TakayaOutput::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();   
   *pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// TAKAYAEdit dialog
TAKAYAEdit::TAKAYAEdit(CWnd* pParent /*=NULL*/)
   : CDialog(TAKAYAEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(TAKAYAEdit)
   m_onoff = FALSE;
   m_rd = _T("");
   m_refname = _T("");
   m_star = _T("");
   m_topbottom = -1;
   m_trefname = _T("");
   m_val1 = _T("");
   m_val2 = _T("");
   m_x1 = _T("");
   m_x2 = _T("");
   m_y1 = _T("");
   m_y2 = _T("");
   //}}AFX_DATA_INIT
}

void TAKAYAEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TAKAYAEdit)
   DDX_Check(pDX, IDC_ONOFF, m_onoff);
   DDX_Text(pDX, IDC_RD, m_rd);
   DDX_Text(pDX, IDC_REFNAME, m_refname);
   DDX_Text(pDX, IDC_STAR, m_star);
   DDX_Radio(pDX, IDC_TOPBOTTOM, m_topbottom);
   DDX_Text(pDX, IDC_TREFNAME, m_trefname);
   DDX_Text(pDX, IDC_VAL1, m_val1);
   DDX_Text(pDX, IDC_VAL2, m_val2);
   DDX_Text(pDX, IDC_X1, m_x1);
   DDX_Text(pDX, IDC_X2, m_x2);
   DDX_Text(pDX, IDC_Y1, m_y1);
   DDX_Text(pDX, IDC_Y2, m_y2);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TAKAYAEdit, CDialog)
   //{{AFX_MSG_MAP(TAKAYAEdit)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// return negative if 1 comes before 2
static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   int swap = (Reverse?-1:1);

   switch (SortBy)
   {
   case 0: // On/Off
      return swap * (((TKoutput *)lParam1)->onoff - ((TKoutput *)lParam2)->onoff);

   case 1: // Surface
      return swap * (((TKoutput *)lParam1)->topbottom - ((TKoutput *)lParam2)->topbottom);

   case 2: // refname
      return swap * STRICMP(((TKoutput *)lParam1)->refname, ((TKoutput *)lParam2)->refname);

   case 3: // trefname
      return swap * STRICMP(((TKoutput *)lParam1)->tkrefname, ((TKoutput *)lParam2)->tkrefname);

   case 4: // val1
      return swap * STRICMP(((TKoutput *)lParam1)->value, ((TKoutput *)lParam2)->value);

   case 5: // val2
      return swap * STRICMP(((TKoutput *)lParam1)->comment, ((TKoutput *)lParam2)->comment);

   case 6: // *
      return swap * STRICMP(((TKoutput *)lParam1)->location, ((TKoutput *)lParam2)->location);

   case 7: // rd
      return swap * STRICMP(((TKoutput *)lParam1)->element, ((TKoutput *)lParam2)->element);

   case 8: // xy
      return swap * (((TKoutput *)lParam1)->x1 - ((TKoutput *)lParam2)->x1);

   case 9: // xy
      return swap * (((TKoutput *)lParam1)->x2 - ((TKoutput *)lParam2)->x2);
   }

   return 0;
}

void TakayaOutput::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   if (SortBy == pNMListView->iSubItem)
      Reverse = !Reverse;
   else 
      Reverse = FALSE;
   SortBy = pNMListView->iSubItem;

   m_list.SortItems(CompareFunc, 0);
   
   *pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// TRIOutput dialog
TRIOutput::TRIOutput(CWnd* pParent /*=NULL*/)
   : CDialog(TRIOutput::IDD, pParent)
{
   //{{AFX_DATA_INIT(TRIOutput)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void TRIOutput::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TRIOutput)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TRIOutput, CDialog)
   //{{AFX_MSG_MAP(TRIOutput)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TRIOutput message handlers
BOOL TRIOutput::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   SortBy = -1;
   Reverse = FALSE;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 20;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "On";
   column.iSubItem = 0;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Refname";
   column.iSubItem = 1;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Type";
   column.iSubItem = 2;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "File";
   column.iSubItem = 3;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Hi-P#";
   column.iSubItem = 4;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Lo-P#";
   column.iSubItem = 5;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "GP1";
   column.iSubItem = 6;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "GP2";
   column.iSubItem = 7;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "PTOL";
   column.iSubItem = 8;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "NTOL";
   column.iSubItem = 9;
   m_list.InsertColumn(column.iSubItem, &column);

   LV_ITEM item;

   // items
   int j = 0;
   int actualItem;
   for (int i=0; i<count; i++)
   {  
      TRIoutput *tri = array->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = "";
      m_list.SetItem(&item);

      actualItem = m_list.InsertItem(&item);
//    if (actualItem == -1) ErrorMessage("Insert Failed");
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

      m_list.SetItemData(actualItem, (LPARAM)tri);

      FillItem(actualItem, tri);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void TRIOutput::FillItem(int actualItem, TRIoutput *tri)
{
   LV_ITEM item;
   CString buf;

   item.mask = LVIF_TEXT;
   item.iItem = actualItem;

   item.iSubItem = 0;
   if (tri->onoff)
      item.pszText = "ON";
   else
      item.pszText = "off";
   m_list.SetItem(&item);

   item.iSubItem = 1;
   item.pszText = tri->refname.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 2;
   item.pszText = tri->tri_type.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = tri->file.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;   // hi
   buf.Format("%d", tri->testpin[0]);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 5;   // lo
   buf.Format("%d", tri->testpin[1]);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 6;   // gp1
   buf.Format("%d", tri->testpin[2]);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 7;   // gp2
   buf.Format("%d", tri->testpin[1]);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 8;   // ptol
   if (fabs(tri->ptol) > 0)
      buf.Format("%1.1lf", tri->ptol);
   else
      buf = "";

   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 9;   // ntol
   if (fabs(tri->ntol) > 0)
      buf.Format("%1.1lf", tri->ntol);
   else
      buf = "";

   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

}

// end export.cpp
