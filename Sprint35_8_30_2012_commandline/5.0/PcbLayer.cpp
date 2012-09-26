// $Header: /CAMCAD/4.5/PcbLayer.cpp 11    4/28/06 2:15p Kurt Van Ness $

#include "stdafx.h"
#include "ccdoc.h"               

#include "CCEtoODB.h"
#include "pcblayer.h"
#include "pcbutil.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern char          *protel_layer[];  // in pfw_out.cpp
extern CString       pads_layer[];     // in pads_out.cpp

#define NOPHYSICAL   "No Physical"
#define TOP          "TOP"
#define BOTTOM       "BOTTOM"
#define INNER        "INNER"
#define ALL          "ALL"
#define OUTER        "OUTER"
#define PLANE        "PLANE"

/////////////////////////////////////////////////////////////////////////////
// MentorLayerDlg dialog
MentorLayerDlg::MentorLayerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(MentorLayerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(MentorLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

MentorLayerDlg::~MentorLayerDlg()
{
   _chdir(cwd);
}

void MentorLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MentorLayerDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MentorLayerDlg, CDialog)
   //{{AFX_MSG_MAP(MentorLayerDlg)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MentorLayerDlg message handlers
BOOL MentorLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;
   sortBy = 0;
   reverse = FALSE;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "Artwork";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "XRF";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.pszText = "Ty";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);

   column.cx = width * 2;
   column.pszText = "Mentor Name";
   column.iSubItem = 5;
   m_list.InsertColumn(5, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK MentorCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   MentorLayerDlg *dlg = (MentorLayerDlg*)lParamSort; 

   int swap = (dlg->reverse?-1:1);

   switch (dlg->sortBy)
   {
   case 0: // Artwork
   default:
      return swap * (((MentorLayerStruct*)lParam1)->stackNum - ((MentorLayerStruct*)lParam2)->stackNum);
   case 1: // XRF
      return swap * (((MentorLayerStruct*)lParam1)->xrf - ((MentorLayerStruct*)lParam2)->xrf);
   case 2: // On
      return swap * (((MentorLayerStruct*)lParam1)->on - ((MentorLayerStruct*)lParam2)->on);
   case 3: // Ty
      return swap * (((MentorLayerStruct*)lParam1)->type - ((MentorLayerStruct*)lParam2)->type);
   case 4: // CAMCAD Name
      return swap * ((MentorLayerStruct*)lParam1)->oldName.CompareNoCase(((MentorLayerStruct*)lParam2)->oldName);
   case 5: // Mentor Name
      return swap * ((MentorLayerStruct*)lParam1)->newName.CompareNoCase(((MentorLayerStruct*)lParam2)->newName);
   }
}

void MentorLayerDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   if (sortBy == pNMListView->iSubItem)
      reverse = !reverse;
   else
   {
      sortBy = pNMListView->iSubItem;
      reverse = FALSE;
   }

   m_list.SortItems(MentorCompareFunc, (LPARAM)this);

   *pResult = 0;
}

void MentorLayerDlg::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      MentorLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);
      item.iItem = actualItem;

      item.iSubItem = 1;
      buf.Format("%d", ml->xrf);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 2;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 3;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 5;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(MentorCompareFunc, (LPARAM)this);
}

void MentorLayerDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   MentorLayerStruct *ml = (MentorLayerStruct*)m_list.GetItemData(selItem);

   MentorLayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.m_new = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_xrf = ml->xrf;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.m_new;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
      ml->xrf = dlg.m_xrf;
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   buf.Format("%d", ml->xrf);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 2;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 3;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 5;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void MentorLayerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   //MentorLayerStruct *ml = arr->GetAt(selItem);
   MentorLayerStruct *ml = (MentorLayerStruct*)m_list.GetItemData(selItem);

   ml->on = !ml->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 2;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void MentorLayerDlg::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         MentorLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".MENTOR"))                continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         // layername can be either a word or "a b c"
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();

         int  number_of_same_camcad_names = 0;
         for (i=0;i<count;i++)
         {
            MentorLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
               number_of_same_camcad_names++;
         }

         for (i=0;i<count;i++)
         {
            MentorLayerStruct *ml = arr->GetAt(i);

            // mentor has a signal and area layer for the same name !!!
            // so it is not enough to only compare the name !
            if (number_of_same_camcad_names > 1)
            {
               if (ml->type != mn.type)   continue;
            }

            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }
 
   FillListCtrl();
}

void MentorLayerDlg::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .MENTOR Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         MentorLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".MENTOR %3d   %3d  %c  \"%s\"  \"%s\"\n", 
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// MentorLayerEdit dialog
MentorLayerEdit::MentorLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(MentorLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(MentorLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_type = _T("");
   m_stacknum = _T("");
   m_xrf = 0;
   //}}AFX_DATA_INIT
}

void MentorLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MentorLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_Text(pDX, IDC_TYPE, m_type);
   DDV_MaxChars(pDX, m_type, 1);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_XRF, m_xrf);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MentorLayerEdit, CDialog)
   //{{AFX_MSG_MAP(MentorLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MentorLayerEdit message handlers
BOOL MentorLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// AllegroLayer dialog
AllegroLayer::AllegroLayer(CWnd* pParent /*=NULL*/)
   : CDialog(AllegroLayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(AllegroLayer)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

AllegroLayer::~AllegroLayer()
{
   _chdir(cwd);
}

void AllegroLayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AllegroLayer)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AllegroLayer, CDialog)
   //{{AFX_MSG_MAP(AllegroLayer)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave1)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AllegroLayer message handlers
BOOL AllegroLayer::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "XRF";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ty";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "Allegro Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK AllegroCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((AllegroLayerStruct *)lParam1)->stackNum - ((AllegroLayerStruct *)lParam2)->stackNum);
}

void AllegroLayer::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      AllegroLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(AllegroCompareFunc, 0);
}

void AllegroLayer::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   AllegroLayerStruct *ml = (AllegroLayerStruct*)m_list.GetItemData(selItem);

   AllegroLayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.m_new = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.m_new;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void AllegroLayer::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   AllegroLayerStruct *ml = (AllegroLayerStruct*)m_list.GetItemData(selItem);

   ml->on = !ml->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void AllegroLayer::OnSave1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .ALLEGRO Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         AllegroLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".ALLEGRO %3d   %3d  %c  \"%s\"  \"%s\"\n",   
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void AllegroLayer::OnLoad1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         AllegroLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".ALLEGRO"))                  continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            AllegroLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// AllegroLayerEdit dialog
AllegroLayerEdit::AllegroLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(AllegroLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(AllegroLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void AllegroLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AllegroLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AllegroLayerEdit, CDialog)
   //{{AFX_MSG_MAP(AllegroLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AllegroLayerEdit message handlers
BOOL AllegroLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// GenCADLayerDlg dialog
GenCADLayerDlg::GenCADLayerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(GenCADLayerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(GenCADLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

GenCADLayerDlg::~GenCADLayerDlg()
{
   _chdir(cwd);
}

void GenCADLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GenCADLayerDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GenCADLayerDlg, CDialog)
   //{{AFX_MSG_MAP(GenCADLayerDlg)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GenCADLayerDlg message handlers
BOOL GenCADLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "XRF";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ty";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "GenCAD Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
      return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK GenCADCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((GenCADLayerStruct *)lParam1)->stackNum - ((GenCADLayerStruct *)lParam2)->stackNum);
}

void GenCADLayerDlg::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      GenCADLayerStruct *gl = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (gl->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", gl->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)gl);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (gl->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", gl->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = gl->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = gl->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(GenCADCompareFunc, 0);
}

void GenCADLayerDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   GenCADLayerStruct *gl = (GenCADLayerStruct*)m_list.GetItemData(selItem);

   GenCADLayerEdit dlg;
   dlg.stacknum = gl->stackNum;
   dlg.m_old = gl->oldName;
   dlg.m_new = gl->newName;
   dlg.m_check = gl->on;
   dlg.m_type.Format("%c", gl->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         gl->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         gl->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         gl->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         gl->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         gl->stackNum = -4;
      else if (!dlg.m_stacknum.Compare(OUTER))
         gl->stackNum = -5;
      else
         gl->stackNum = atoi(dlg.m_stacknum);

      gl->newName = dlg.m_new;
      gl->on = dlg.m_check;
      gl->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (gl->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      case -5:
         buf = OUTER;
      break;
      default:
         buf.Format("%d", gl->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (gl->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format( "%c", gl->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = gl->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = gl->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void GenCADLayerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   GenCADLayerStruct *gl = (GenCADLayerStruct*)m_list.GetItemData(selItem);

   gl->on = !gl->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (gl->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void GenCADLayerDlg::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         GenCADLayerStruct g;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".GENCAD"))                continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            GenCADLayerStruct *gl = arr->GetAt(i);
            if (gl->oldName.Compare(g.oldName) == 0)
            {
               gl->stackNum = g.stackNum;
               gl->on = g.on;
               gl->type = g.type;
               gl->newName = g.newName;
               arr->SetAt(i, gl);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }
 
   FillListCtrl();
}

void GenCADLayerDlg::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .GENCAD Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,"; Stacknumber -5 = OUTER (electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         GenCADLayerStruct *gl = arr->GetAt(i);
         fprintf(fp,".GENCAD %3d   %3d  %c  \"%s\"  \"%s\"\n", 
            gl->stackNum, gl->on, gl->type, gl->oldName, gl->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// GenCADLayerEdit dialog
GenCADLayerEdit::GenCADLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(GenCADLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(GenCADLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void GenCADLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GenCADLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GenCADLayerEdit, CDialog)
   //{{AFX_MSG_MAP(GenCADLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GenCADLayerEdit message handlers
BOOL GenCADLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// GenCAMLayerDlg dialog
GenCAMLayerDlg::GenCAMLayerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(GenCAMLayerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(GenCAMLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

GenCAMLayerDlg::~GenCAMLayerDlg()
{
   _chdir(cwd);
}

void GenCAMLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GenCAMLayerDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GenCAMLayerDlg, CDialog)
   //{{AFX_MSG_MAP(GenCAMLayerDlg)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GenCAMLayerDlg message handlers
BOOL GenCAMLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "XRF";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ty";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "GenCAM Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
      return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK GenCAMCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((GenCAMLayerStruct *)lParam1)->stackNum - ((GenCADLayerStruct *)lParam2)->stackNum);
}

void GenCAMLayerDlg::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      GenCAMLayerStruct *gl = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (gl->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", gl->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)gl);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (gl->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", gl->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = gl->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = gl->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(GenCAMCompareFunc, 0);
}

void GenCAMLayerDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   GenCAMLayerStruct *gl = (GenCAMLayerStruct*)m_list.GetItemData(selItem);

   GenCAMLayerEdit dlg;
   dlg.stacknum = gl->stackNum;
   dlg.m_old = gl->oldName;
   dlg.m_new = gl->newName;
   dlg.m_check = gl->on;
   dlg.m_type.Format("%c", gl->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         gl->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         gl->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         gl->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         gl->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         gl->stackNum = -4;
      else if (!dlg.m_stacknum.Compare(OUTER))
         gl->stackNum = -5;
      else
         gl->stackNum = atoi(dlg.m_stacknum);

      gl->newName = dlg.m_new;
      gl->on = dlg.m_check;
      gl->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (gl->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      case -5:
         buf = OUTER;
      break;
      default:
         buf.Format("%d", gl->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (gl->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format( "%c", gl->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = gl->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = gl->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void GenCAMLayerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   GenCAMLayerStruct *gl = (GenCAMLayerStruct*)m_list.GetItemData(selItem);

   gl->on = !gl->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (gl->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void GenCAMLayerDlg::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         GenCAMLayerStruct g;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".GENCAM"))                continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            GenCAMLayerStruct *gl = arr->GetAt(i);
            if (gl->oldName.Compare(g.oldName) == 0)
            {
               gl->stackNum = g.stackNum;
               gl->on = g.on;
               gl->type = g.type;
               gl->newName = g.newName;
               arr->SetAt(i, gl);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }
 
   FillListCtrl();
}

void GenCAMLayerDlg::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .GENCAM Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,"; Stacknumber -5 = OUTER (electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         GenCAMLayerStruct *gl = arr->GetAt(i);
         fprintf(fp,".GENCAM %3d   %3d  %c  \"%s\"  \"%s\"\n", 
            gl->stackNum, gl->on, gl->type, gl->oldName, gl->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// GenCADLayerEdit dialog
GenCAMLayerEdit::GenCAMLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(GenCAMLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(GenCAMLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void GenCAMLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GenCAMLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GenCAMLayerEdit, CDialog)
   //{{AFX_MSG_MAP(GenCAMLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GenCAMLayerEdit message handlers
BOOL GenCAMLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// FabMasterLayerDlg dialog
FabMasterLayerDlg::FabMasterLayerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(FabMasterLayerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(FabMasterLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

FabMasterLayerDlg::~FabMasterLayerDlg()
{
   _chdir(cwd);
}

void FabMasterLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FabMasterLayerDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FabMasterLayerDlg, CDialog)
   //{{AFX_MSG_MAP(FabMasterLayerDlg)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FabMasterLayerDlg message handlers
BOOL FabMasterLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "XRF";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ty";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "FabMaster Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
      return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK FabMasterCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((FabMasterLayerStruct *)lParam1)->stackNum - ((FabMasterLayerStruct *)lParam2)->stackNum);
}

void FabMasterLayerDlg::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      FabMasterLayerStruct *gl = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (gl->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", gl->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)gl);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (gl->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", gl->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = gl->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = gl->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(FabMasterCompareFunc, 0);
}

void FabMasterLayerDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   FabMasterLayerStruct *gl = (FabMasterLayerStruct*)m_list.GetItemData(selItem);

   FabMasterLayerEdit dlg;
   dlg.stacknum = gl->stackNum;
   dlg.m_old = gl->oldName;
   dlg.m_new = gl->newName;
   dlg.m_check = gl->on;
   dlg.m_type.Format("%c", gl->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         gl->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         gl->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         gl->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         gl->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         gl->stackNum = -4;
      else if (!dlg.m_stacknum.Compare(OUTER))
         gl->stackNum = -5;
      else
         gl->stackNum = atoi(dlg.m_stacknum);

      gl->newName = dlg.m_new;
      gl->on = dlg.m_check;
      gl->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (gl->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      case -5:
         buf = OUTER;
      break;
      default:
         buf.Format("%d", gl->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (gl->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format( "%c", gl->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = gl->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = gl->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void FabMasterLayerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   FabMasterLayerStruct *gl = (FabMasterLayerStruct*)m_list.GetItemData(selItem);

   gl->on = !gl->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (gl->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void FabMasterLayerDlg::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         FabMasterLayerStruct g;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".FabMaster"))                continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         g.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            FabMasterLayerStruct *gl = arr->GetAt(i);
            if (gl->oldName.Compare(g.oldName) == 0)
            {
               gl->stackNum = g.stackNum;
               gl->on = g.on;
               gl->type = g.type;
               gl->newName = g.newName;
               arr->SetAt(i, gl);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }
 
   FillListCtrl();
}

void FabMasterLayerDlg::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .FabMaster Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,"; Stacknumber -5 = OUTER (electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         FabMasterLayerStruct *gl = arr->GetAt(i);
         fprintf(fp,".FabMaster %3d   %3d  %c  \"%s\"  \"%s\"\n", 
            gl->stackNum, gl->on, gl->type, gl->oldName, gl->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// FabMasterLayerEdit dialog
FabMasterLayerEdit::FabMasterLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(FabMasterLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(FabMasterLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void FabMasterLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FabMasterLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FabMasterLayerEdit, CDialog)
   //{{AFX_MSG_MAP(FabMasterLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FabMasterLayerEdit message handlers
BOOL FabMasterLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// VB99Layer dialog
VB99Layer::VB99Layer(CWnd* pParent /*=NULL*/)
   : CDialog(VB99Layer::IDD, pParent)
{
   //{{AFX_DATA_INIT(VB99Layer)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

VB99Layer::~VB99Layer()
{
   _chdir(cwd);
}

void VB99Layer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VB99Layer)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(VB99Layer, CDialog)
   //{{AFX_MSG_MAP(VB99Layer)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VB99Layer message handlers
BOOL VB99Layer::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "XRF";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ty";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "Expedition Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK VB99CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((VB99LayerStruct *)lParam1)->stackNum - ((VB99LayerStruct *)lParam2)->stackNum);
}

void VB99Layer::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      VB99LayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(VB99CompareFunc, 0);
}

void VB99Layer::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   VB99LayerStruct *ml = (VB99LayerStruct*)m_list.GetItemData(selItem);

   VB99LayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.m_new = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.m_new;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void VB99Layer::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   VB99LayerStruct *ml = (VB99LayerStruct*)m_list.GetItemData(selItem);

   ml->on = !ml->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void VB99Layer::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .VB99 Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         VB99LayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".VB99 %3d   %3d  %c  \"%s\"  \"%s\"\n",   
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void VB99Layer::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         VB99LayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".VB99"))                  continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            VB99LayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// VB99LayerEdit dialog
VB99LayerEdit::VB99LayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(VB99LayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(VB99LayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void VB99LayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(VB99LayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(VB99LayerEdit, CDialog)
   //{{AFX_MSG_MAP(VB99LayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VB99LayerEdit message handlers
BOOL VB99LayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// ACCELLayer dialog
ACCELLayer::ACCELLayer(CWnd* pParent /*=NULL*/)
   : CDialog(ACCELLayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(ACCELLayer)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

ACCELLayer::~ACCELLayer()
{
   _chdir(cwd);
}

void ACCELLayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ACCELLayer)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ACCELLayer, CDialog)
   //{{AFX_MSG_MAP(ACCELLayer)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ACCELLayer message handlers
BOOL ACCELLayer::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 6;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "Layer Number";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width/2;
   column.pszText = "Type";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.pszText = "Accel Layer Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK ACCELCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((ACCELLayerStruct *)lParam1)->stackNum - ((ACCELLayerStruct *)lParam2)->stackNum);
}

void ACCELLayer::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      ACCELLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         case -5:
            buf = PLANE;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(ACCELCompareFunc, 0);
}

void ACCELLayer::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   ACCELLayerStruct *ml = (ACCELLayerStruct*)m_list.GetItemData(selItem);

   ACCELLayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.m_new = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else if (!dlg.m_stacknum.Compare(PLANE))
         ml->stackNum = -5;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.m_new;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      case -5:
         buf = PLANE;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void ACCELLayer::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   ACCELLayerStruct *p = (ACCELLayerStruct*)m_list.GetItemData(selItem);

   p->on = !p->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (p->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void ACCELLayer::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .ACCEL Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,"; Stacknumber -5 = PLANE (Plane pad)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         ACCELLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".ACCEL %3d   %3d  %c  \"%s\"  \"%s\"\n",  
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void ACCELLayer::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         ACCELLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".ACCEL"))                 continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            ACCELLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// ACCELLayerEdit dialog
ACCELLayerEdit::ACCELLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(ACCELLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(ACCELLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void ACCELLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ACCELLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ACCELLayerEdit, CDialog)
   //{{AFX_MSG_MAP(ACCELLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ACCELLayerEdit message handlers
BOOL ACCELLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// ORCADLayer dialog
ORCADLayer::ORCADLayer(CWnd* pParent /*=NULL*/)
   : CDialog(ORCADLayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(ORCADLayer)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

ORCADLayer::~ORCADLayer()
{
   _chdir(cwd);
}

void ORCADLayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ORCADLayer)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ORCADLayer, CDialog)
   //{{AFX_MSG_MAP(ORCADLayer)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ORCADLayer message handlers
BOOL ORCADLayer::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 6;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width/2;
   column.pszText = "Layer Number";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width/2;
   column.pszText = "Type";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = round(width*1.5);
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width;
   column.pszText = "Orcad Layer";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);

   column.pszText = "Nick Name";
   column.iSubItem = 5;
   m_list.InsertColumn(5, &column);

   column.pszText = "Lib Name";
   column.iSubItem = 6;
   m_list.InsertColumn(6, &column);

   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK ORCADCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((ORCADLayerStruct *)lParam1)->stackNum - ((ORCADLayerStruct *)lParam2)->stackNum);
}

void ORCADLayer::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      ORCADLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 5;
      item.pszText = ml->nickName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 6;
      item.pszText = ml->libName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(ORCADCompareFunc, 0);
}

void ORCADLayer::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   ORCADLayerStruct *ml = (ORCADLayerStruct*)m_list.GetItemData(selItem);

   ORCADLayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.m_new = ml->newName;
   dlg.m_nickname = ml->nickName;
   dlg.m_libname = ml->libName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.m_new;
      ml->nickName = dlg.m_nickname;
      ml->libName = dlg.m_libname;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 5;
   item.pszText = ml->nickName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 6;
   item.pszText = ml->libName.GetBuffer(0);
   m_list.SetItem(&item);
}

void ORCADLayer::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   ORCADLayerStruct *p = (ORCADLayerStruct*)m_list.GetItemData(selItem);

   p->on = !p->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (p->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void ORCADLayer::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .ORCAD Stacknumber   On/Off   Type   OldName   NewName NickName LibName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         ORCADLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".ORCAD %3d   %3d  %c  \"%s\"  \"%s\" \"%s\" \"%s\"\n",   
            ml->stackNum, ml->on, ml->type, ml->oldName, 
            ml->newName, ml->nickName, ml->libName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void ORCADLayer::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName NickName LibName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         ORCADLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;
         
         // check for a correct line.
         if (STRCMPI(lp,".ORCAD"))                 continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.nickName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.libName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            ORCADLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               ml->nickName = mn.nickName;
               ml->libName = mn.libName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// ORCADLayerEdit dialog
ORCADLayerEdit::ORCADLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(ORCADLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(ORCADLayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   m_nickname = _T("");
   m_libname = _T("");
   //}}AFX_DATA_INIT
}

void ORCADLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ORCADLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   DDX_Text(pDX, IDC_NICKNAME, m_nickname);
   DDX_Text(pDX, IDC_LIBNAME, m_libname);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ORCADLayerEdit, CDialog)
   //{{AFX_MSG_MAP(ORCADLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ORCADLayerEdit message handlers
BOOL ORCADLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CADIFLayer dialog
CADIFLayer::CADIFLayer(CWnd* pParent /*=NULL*/)
   : CDialog(CADIFLayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(CADIFLayer)
   m_technologyName = _T("");
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

CADIFLayer::~CADIFLayer()
{
   _chdir(cwd);
}

void CADIFLayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CADIFLayer)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Text(pDX, IDC_TECHNOLOGY_NAME, m_technologyName);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CADIFLayer, CDialog)
   //{{AFX_MSG_MAP(CADIFLayer)
   ON_BN_CLICKED(IDC_LOAD_TECHNOLOGY, OnLoadTechnology)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CADIFLayer message handlers
BOOL CADIFLayer::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = round(width*1.5);
   column.pszText = "XRF";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ty";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 2;
   column.pszText = "CADIF Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK CADIFCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((CADIFLayerStruct *)lParam1)->stackNum - ((CADIFLayerStruct *)lParam2)->stackNum);
}

void CADIFLayer::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      CADIFLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(CADIFCompareFunc, 0);
}

void CADIFLayer::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   CADIFLayerStruct *ml = (CADIFLayerStruct*)m_list.GetItemData(selItem);

   CADIFLayerEditDlg dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.m_new = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.m_new;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
   
}

void CADIFLayer::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}

void CADIFLayer::OnLoad1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         CADIFLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".CADIF"))                 continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            CADIFLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

void CADIFLayer::OnSave1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .CADIF Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         CADIFLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".CADIF %3d   %3d  %c  \"%s\"  \"%s\"\n",  
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void CADIFLayer::OnLoadTechnology() 
{
   CString  TecName;

   // TODO: Add your control notification handler code here
   CFileDialog FileDialog(TRUE, "TECH", "*.tech",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Technology File (*.TECH)|*.TECH|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   TecName =  FileDialog.GetPathName();

   //CADIF_LoadTechnology(TecName);   
   m_technologyName = TecName;
   UpdateData(FALSE);

}


/////////////////////////////////////////////////////////////////////////////
// CADIFLayerEditDlg dialog
CADIFLayerEditDlg::CADIFLayerEditDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CADIFLayerEditDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CADIFLayerEditDlg)
   m_xrf = 0;
   m_type = _T("");
   m_old = _T("");
   m_new = _T("");
   m_check = FALSE;
   m_stacknum = _T("");
   //}}AFX_DATA_INIT
}

void CADIFLayerEditDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CADIFLayerEditDlg)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Text(pDX, IDC_TYPE, m_type);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CADIFLayerEditDlg, CDialog)
   //{{AFX_MSG_MAP(CADIFLayerEditDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CADIFLayerEditDlg message handlers
BOOL CADIFLayerEditDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// ProtelLayerDlg dialog
ProtelLayerDlg::ProtelLayerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(ProtelLayerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(ProtelLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

ProtelLayerDlg::~ProtelLayerDlg()
{
   _chdir(cwd);
}

void ProtelLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ProtelLayerDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ProtelLayerDlg, CDialog)
   //{{AFX_MSG_MAP(ProtelLayerDlg)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProtelLayerDlg message handlers
BOOL ProtelLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "Layer Number";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width/2;
   column.pszText = "Type";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.pszText = "Protel Layer Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK ProtelCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((ProtelLayerStruct *)lParam1)->stackNum - ((ProtelLayerStruct *)lParam2)->stackNum);
}

void ProtelLayerDlg::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      ProtelLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(ProtelCompareFunc, 0);
}

void ProtelLayerDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   ProtelLayerStruct *ml = (ProtelLayerStruct*)m_list.GetItemData(selItem);

   ProtelLayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.newname = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.newname;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void ProtelLayerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   ProtelLayerStruct *ml = (ProtelLayerStruct*)m_list.GetItemData(selItem);

   ml->on = !ml->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void ProtelLayerDlg::OnSave1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .Protel Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         ProtelLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".Protel %3d   %3d  %c  \"%s\"  \"%s\"\n", 
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void ProtelLayerDlg::OnLoad1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         ProtelLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".Protel"))                continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            ProtelLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// ProtelLayerEdit dialog
ProtelLayerEdit::ProtelLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(ProtelLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(ProtelLayerEdit)
   m_check = FALSE;
   m_old = _T("");
   m_type = _T("");
   m_stacknum = _T("");
   //}}AFX_DATA_INIT
}

void ProtelLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ProtelLayerEdit)
   DDX_Control(pDX, IDC_NEW_CB, m_newCB);
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_Text(pDX, IDC_TYPE, m_type);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ProtelLayerEdit, CDialog)
   //{{AFX_MSG_MAP(ProtelLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProtelLayerEdit message handlers
BOOL ProtelLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   //for (int i=0; i<35; i++)
      //m_newCB.AddString(protel_layer[i]);

   m_newCB.SelectString(0, newname);

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ProtelLayerEdit::OnOK() 
{
   UpdateData();

   m_newCB.GetLBText(m_newCB.GetCurSel(), newname);
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// PadsLayerDlg dialog
PadsLayerDlg::PadsLayerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(PadsLayerDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(PadsLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

PadsLayerDlg::~PadsLayerDlg()
{
   _chdir(cwd);
}

void PadsLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PadsLayerDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PadsLayerDlg, CDialog)
   //{{AFX_MSG_MAP(PadsLayerDlg)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad)
   ON_BN_CLICKED(IDC_SAVE1, OnSave)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PadsLayerDlg message handlers
BOOL PadsLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "Stack";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width/2;
   column.pszText = "Type";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width;
   column.pszText = "Pads Lev.";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK PADSCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((PADSLayerStruct *)lParam1)->stackNum - ((PADSLayerStruct *)lParam2)->stackNum);
}

void PadsLayerDlg::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      PADSLayerStruct *ml = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (ml->stackNum)
      {
         case 0:
            buf = NOPHYSICAL;
         break;
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         case -5:
            buf = "UNKNOWN";
         break;
         default:
            buf.Format("%d", ml->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)ml);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (ml->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", ml->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = ml->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = ml->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(PADSCompareFunc, 0);
}

void PadsLayerDlg::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   PADSLayerStruct *ml = (PADSLayerStruct*)m_list.GetItemData(selItem);

   PADSLayerEdit dlg;
   dlg.stacknum = ml->stackNum;
   dlg.m_old = ml->oldName;
   dlg.newname = ml->newName;
   dlg.m_check = ml->on;
   dlg.m_type.Format("%c", ml->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(NOPHYSICAL))
         ml->stackNum = 0;
      else if (!dlg.m_stacknum.Compare(TOP))
         ml->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         ml->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         ml->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         ml->stackNum = -4;
      else
         ml->stackNum = atoi(dlg.m_stacknum);

      ml->newName = dlg.newname;
      ml->on = dlg.m_check;
      ml->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (ml->stackNum)
   {
      case 0:
         buf = NOPHYSICAL;
      break;
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", ml->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", ml->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = ml->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = ml->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void PadsLayerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   PADSLayerStruct *ml = (PADSLayerStruct*)m_list.GetItemData(selItem);

   ml->on = !ml->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (ml->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void PadsLayerDlg::OnSave() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .PADS Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber -1 = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber -2 = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber -3 = INNER (all electrical Inner)\n");
      fprintf(fp,"; Stacknumber -4 = ALL (all electrical top-bottom)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         PADSLayerStruct *ml = arr->GetAt(i);
         fprintf(fp,".Pads %3d   %3d  %c  \"%s\"  \"%s\"\n",   
            ml->stackNum, ml->on, ml->type, ml->oldName, ml->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

void PadsLayerDlg::OnLoad() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         PADSLayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".Pads"))                  continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            PADSLayerStruct *ml = arr->GetAt(i);
            if (ml->oldName.Compare(mn.oldName) == 0)
            {
               ml->stackNum = mn.stackNum;
               ml->on = mn.on;
               ml->type = mn.type;
               ml->newName = mn.newName;
               arr->SetAt(i,ml);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// PADSLayerEdit dialog
PADSLayerEdit::PADSLayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(PADSLayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(PADSLayerEdit)
   m_check = FALSE;
   m_old = _T("");
   m_stacknum = _T("");
   m_type = _T("");
   //}}AFX_DATA_INIT
}

void PADSLayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PADSLayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Control(pDX, IDC_NEW_CB, m_newCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   DDX_Text(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PADSLayerEdit, CDialog)
   //{{AFX_MSG_MAP(PADSLayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PADSLayerEdit message handlers
BOOL PADSLayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case 0:
      m_stacknum = NOPHYSICAL;
      break;
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   // pads_layer has the true pads names
   for (int i=0; i<30; i++)   // 1..30
   {
      CString l;
      l.Format("%d", i+1);
      m_newCB.AddString(l);
   }

   int sel = m_newCB.FindStringExact(0, newname);
   m_newCB.SetCurSel(sel);

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void PADSLayerEdit::OnOK() 
{
   int sel = m_newCB.GetCurSel();
   if (sel != CB_ERR)
      m_newCB.GetLBText(sel, newname);
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// DDELayer dialog
DDELayer::DDELayer(CWnd* pParent /*=NULL*/)
   : CDialog(DDELayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(DDELayer)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   _getcwd(cwd, _MAX_PATH);
}

DDELayer::~DDELayer()
{
   _chdir(cwd);
}

void DDELayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DDELayer)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DDELayer, CDialog)
   //{{AFX_MSG_MAP(DDELayer)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD1, OnLoad1)
   ON_BN_CLICKED(IDC_SAVE1, OnSave1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DDELayer message handlers
BOOL DDELayer::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   selItem = 0;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;
   column.pszText = "Layer Number";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width/2;
   column.pszText = "On";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width/2;
   column.pszText = "Type";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width * 2;
   column.pszText = "CAMCAD Name";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.pszText = "DDE Layer Name";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);
   FillListCtrl();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// return negative if 1 comes before 2
static int CALLBACK DDECompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   return (((DDELayerStruct *)lParam1)->stackNum - ((DDELayerStruct *)lParam2)->stackNum);
}

void DDELayer::FillListCtrl() 
{
   LV_ITEM  item;
   int      actualItem;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   m_list.DeleteAllItems();

   for (int i=0; i<maxArr; i++)
   {
      DDELayerStruct *p = arr->GetAt(i);

      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      switch (p->stackNum)
      {
         case -1:
            buf = TOP;
         break;
         case -2:
            buf = BOTTOM;
         break;
         case -3:
            buf = INNER;
         break;
         case -4:
            buf = ALL;
         break;
         default:
            buf.Format("%d", p->stackNum);
         break;
      }
      item.pszText = buf.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)p);

      item.iItem = actualItem;
      item.iSubItem = 1;
      if (p->on)
         item.pszText = cTrue;
      else
         item.pszText = cFalse;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%c", p->type);
      item.pszText = buf.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = p->oldName.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = p->newName.GetBuffer(0);
      m_list.SetItem(&item);
   }

   m_list.SortItems(DDECompareFunc, 0);
}

void DDELayer::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   DDELayerStruct *p = (DDELayerStruct*)m_list.GetItemData(selItem);

   p->on = !p->on;

   LV_ITEM item;
   char *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;

   item.iSubItem = 1;
   if (p->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   *pResult = 0;
}

void DDELayer::OnEdit() 
{
   if (!m_list.GetSelectedCount())
      return;

   int count = m_list.GetItemCount();
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   DDELayerStruct *p = (DDELayerStruct*)m_list.GetItemData(selItem);

   DDELayerEdit dlg;
   dlg.stacknum = p->stackNum;
   dlg.m_old = p->oldName;
   dlg.m_new = p->newName;
   dlg.m_check = p->on;
   dlg.m_type.Format("%c", p->type);
   if (dlg.DoModal() == IDOK)
   {
      if (!dlg.m_stacknum.Compare(TOP))
         p->stackNum = -1;
      else if (!dlg.m_stacknum.Compare(BOTTOM))
         p->stackNum = -2;
      else if (!dlg.m_stacknum.Compare(INNER))
         p->stackNum = -3;
      else if (!dlg.m_stacknum.Compare(ALL))
         p->stackNum = -4;
      else
         p->stackNum = atoi(dlg.m_stacknum);

      p->newName = dlg.m_new;
      p->on = dlg.m_check;
      p->type = dlg.m_type[0];
   }

   LV_ITEM  item;
   CString  buf;
   char     *cTrue = "X", *cFalse = "";

   item.mask = LVIF_TEXT;
   item.iItem = selItem;
   item.iSubItem = 0;
   switch (p->stackNum)
   {
      case -1:
         buf = TOP;
      break;
      case -2:
         buf = BOTTOM;
      break;
      case -3:
         buf = INNER;
      break;
      case -4:
         buf = ALL;
      break;
      default:
         buf.Format("%d", p->stackNum);
      break;
   }
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 1;
   if (p->on)
      item.pszText = cTrue;
   else
      item.pszText = cFalse;
   m_list.SetItem(&item);

   item.iSubItem = 2;
   buf.Format("%c", p->type);
   item.pszText = buf.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 3;
   item.pszText = p->oldName.GetBuffer(0);
   m_list.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = p->newName.GetBuffer(0);
   m_list.SetItem(&item);
}

void DDELayer::OnLoad1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.lyr",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // this loads a layer file, but only entries where the
   // Oldname matches
   FILE  *fp;

   if ((fp = fopen(FileName,"rt")) != NULL)
   {
      //fprintf(fp,"; start : %s\n",FileName);
      //fprintf(fp,"; Stacknumber   On/Off   Type   OldName   NewName\n");
      //fprintf(fp,";\n");
      char  iline[255+1];
      while (fgets(iline, 255, fp))
      {
         char  *lp;
         DDELayerStruct mn;

         // load data
         if ((lp = get_string(iline," \t\n")) == NULL)   continue;

         // check for a correct line.
         if (STRCMPI(lp,".DDE"))                continue;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.stackNum = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.on = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.type = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.oldName = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         mn.newName = lp;

         if (mn.stackNum < 0)                         continue;
         if (mn.stackNum > 255)                       continue;

         // find entry and update
         int i, count = m_list.GetItemCount();
         for (i=0;i<count;i++)
         {
            DDELayerStruct *p = arr->GetAt(i);
            if (p->oldName.Compare(mn.oldName) == 0)
            {
               p->stackNum = mn.stackNum;
               p->on = mn.on;
               p->type = mn.type;
               p->newName = mn.newName;
               arr->SetAt(i, p);
            }
         }
      }
      fclose(fp);
   }
   else
   {
      // file open error
   }

   FillListCtrl();
}

void DDELayer::OnSave1() 
{
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.lyr",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layermap File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());
   
   // save complete layer file  
   FILE  *fp;

   if ((fp = fopen(FileName,"wt")) != NULL)
   {
      fprintf(fp,"; start : %s\n",FileName);
      fprintf(fp,"; .DDE Stacknumber   On/Off   Type   OldName   NewName\n");

      fprintf(fp,"; Stacknumber 0   = TOP (Component Side)\n");
      fprintf(fp,"; Stacknumber 1   = BOTTOM (Solder Side)\n");
      fprintf(fp,"; Stacknumber 255 = ALL (All Electrical)\n");
      fprintf(fp,";\n");

      int i, count = m_list.GetItemCount();
      for (i=0;i<count;i++)
      {
         DDELayerStruct *p = arr->GetAt(i);
         fprintf(fp,".DDE %3d   %3d  %c  \"%s\"  \"%s\"\n", 
            p->stackNum, p->on, p->type, p->oldName, p->newName);
      }

      fprintf(fp,";\n");
      fprintf(fp,"; end\n");
      fclose(fp);
   }
   else
   {
      // file open error
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// DDELayerEdit dialog
DDELayerEdit::DDELayerEdit(CWnd* pParent /*=NULL*/)
   : CDialog(DDELayerEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(DDELayerEdit)
   m_check = FALSE;
   m_new = _T("");
   m_old = _T("");
   m_type = _T("");
   m_stacknum = _T("");
   //}}AFX_DATA_INIT
}

void DDELayerEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DDELayerEdit)
   DDX_Control(pDX, IDC_STACKNUM, m_stacknumCB);
   DDX_Check(pDX, IDC_CHECK, m_check);
   DDX_Text(pDX, IDC_NEW, m_new);
   DDX_Text(pDX, IDC_OLD, m_old);
   DDX_Text(pDX, IDC_TYPE, m_type);
   DDX_CBString(pDX, IDC_STACKNUM, m_stacknum);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DDELayerEdit, CDialog)
   //{{AFX_MSG_MAP(DDELayerEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DDELayerEdit message handlers
BOOL DDELayerEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_stacknumCB.AddString(NOPHYSICAL);
   m_stacknumCB.AddString(TOP);
   m_stacknumCB.AddString(BOTTOM);
   m_stacknumCB.AddString(INNER);
   m_stacknumCB.AddString(ALL);

   switch (stacknum)
   {
   case -1:
      m_stacknum = TOP;
      break;
   case -2:
      m_stacknum = BOTTOM;
      break;
   case -3:
      m_stacknum = INNER;
      break;
   case -4:
      m_stacknum = ALL;
      break;
   default:
      m_stacknum.Format("%d", stacknum);
      break;
   }

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

//--------------------------------------------------------------------------------

// This is a utility for loading .LAYERATTR and .MIRRORLAYER settings from
// any settings file and applying them. The settings file name is passed in to
// the constructor. The app using this then calls the Apply() method.
// E.g. from GenCamIn.cpp
//   assign_layer(); // something local in reader
//   CLayerSettings layerSettings(s_genCamReader->getSettingsFilename());
//   layerSettings.Apply(doc);
//   doc->RemoveUnusedLayers(); // something else local in the reader

CLayerSettings::CLayerSettings(CString settingsFileName)
: m_settingsFileName(settingsFileName)
{
	LoadSettings();
}

void CLayerSettings::LoadSettings()
{
	if (!m_settingsFileName.IsEmpty())
	{
		char line[255];

		FILE *fp = fopen(m_settingsFileName, "rt");
		if (fp == NULL)
		{
			// We don't want messages from this, settings file presence and
			// any settings at all are entirely optional.
			// But if we change our mind... here is the message code.
			///CString tmp;
			///tmp.Format("File [%s] not found", m_settingsFileName);
			///MessageBox(NULL, tmp, "Layer Settings", MB_OK | MB_ICONHAND);
			return;
		}

		while (fgets(line, 255, fp))
		{
			CString w = line;
			char *lp = get_string(line, " \t\n");
			if (lp == NULL)
				continue;

			if (lp[0] == '.')
			{
				if (!STRICMP(lp, ".LAYERATTR"))
				{
					if ((lp = get_string(NULL, " \t\n")) == NULL)
						continue; 

					CString layerName(lp);

					if ((lp = get_string(NULL, " \t\n")) == NULL)
						continue; 

					CString camcadLayerTypeString(lp);

					layerName.MakeUpper();
					layerName.Trim();
					camcadLayerTypeString.Trim();

					m_layerTypeMap.SetAt(layerName, camcadLayerTypeString);
				}
				else if (!STRICMP(lp, ".MIRRORLAYER"))
				{
					if ((lp = get_string(NULL, " ;\t\n")) == NULL)
						continue; 

					CString layerName1(lp);

					if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 

					CString layerName2(lp);

					layerName1.MakeUpper();
					layerName2.MakeUpper();
					m_mirrorMap.SetAt(layerName1, layerName2);
				}
			}
		}
		fclose(fp);
	}
}

void CLayerSettings::Apply(CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
		for (int i = 0; i < doc->getMaxLayerIndex(); i++)
		{
			LayerStruct *layer = doc->getLayerArray()[i];

			if (layer != NULL)
			{
				CString layerName = layer->getName();
				layerName.MakeUpper();

				// Set layer type
				CString layerType;
				if (m_layerTypeMap.Lookup(layerName, layerType))
				{
					if (!layerType.IsEmpty())
					{
						layer->setLayerType(stringToLayerTypeTag(layerType));
					}
				}

				// Set mirror layers
				CString mirrorLayer;
				if (m_mirrorMap.Lookup(layerName, mirrorLayer))
				{
					LayerStruct *mirLayer = doc->getLayer(mirrorLayer);
					if (mirLayer != NULL)
					{
						int mirLayerIndx = mirLayer->getLayerIndex();
						layer->setMirroredLayerIndex(mirLayerIndx);
						mirLayer->setMirroredLayerIndex(layer->getLayerIndex());
					}
				}

			}
		}
	}

}

