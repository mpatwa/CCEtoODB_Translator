// $Header: /CAMCAD/4.6/ApDlg.cpp 11    5/24/07 5:22p Lynn Phung $

#include "StdAfx.h"
#include "General.h"
#include "CCEtoODB.h"
#include "apdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *shapes[];

/////////////////////////////////////////////////////////////////////////////
// SmartReader dialog
SmartReader::SmartReader(CWnd* pParent /*=NULL*/)
   : CDialog(SmartReader::IDD, pParent)
{
   //{{AFX_DATA_INIT(SmartReader)
   m_ApPrefix = _T("");
   m_Logfile = _T("");
   m_MacFile = _T("");
   //}}AFX_DATA_INIT
}


void SmartReader::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SmartReader)
   DDX_Text(pDX, IDC_AP_PREFIX, m_ApPrefix);
   DDX_Text(pDX, IDC_LOGFILE, m_Logfile);
   DDX_Text(pDX, IDC_MAC_FILE, m_MacFile);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SmartReader, CDialog)
   //{{AFX_MSG_MAP(SmartReader)
   ON_BN_CLICKED(IDC_CHANGE_MAC_FILE, OnChangeMacFile)
   ON_BN_CLICKED(IDHELP, OnHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SmartReader message handlers

void SmartReader::OnChangeMacFile() 
{
   UpdateData();

   CFileDialog FileDialog(TRUE, "MAC", m_MacFile,
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "Macro File (*.MAC)|*.MAC||", this);
   if (FileDialog.DoModal() != IDOK) return;

   m_MacFile = FileDialog.GetPathName();

   UpdateData(FALSE);
}

void SmartReader::OnHelp() 
{ 
   //getApp().OnHelpIndex();
}

/////////////////////////////////////////////////////////////////////////////
// ApImportList dialog
ApImportList::ApImportList(AperStruct **Head, char *Prefix, char *Logfile, 
                           int apUnits, char *macName, CWnd* pParent /*=NULL*/)
   : CDialog(ApImportList::IDD, pParent)
{
   head = Head;
   prefix = Prefix;
   logfile = Logfile;
   //{{AFX_DATA_INIT(ApImportList)
   m_units = _T("MILS");
   m_macName = _T("");
   //}}AFX_DATA_INIT
   m_units = GetUnitName(apUnits);
   decimals = GetDecimals(apUnits);
   m_macName = macName;
}

ApImportList::~ApImportList()
{
   if (m_imageList)
      delete m_imageList;
}

void ApImportList::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ApImportList)
   DDX_Control(pDX, IDC_LISTCTRL, m_listCtrl);
   DDX_Text(pDX, IDC_UNITS, m_units);
   DDX_Text(pDX, IDC_MAC_NAME, m_macName);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ApImportList, CDialog)
   //{{AFX_MSG_MAP(ApImportList)
   ON_BN_CLICKED(IDC_LOGFILE, OnLogfile)
   ON_BN_CLICKED(IDHELP, OnHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ApImportList message handlers

void ApImportList::OnLogfile() 
{
   CString notepad;

/* if (access(file, 0)) 
   {
      ErrorMessage("Can't find What's New file!", file);
      return; // whatsnew.txt file is not in camcad directory
   }*/

   Notepad(logfile);
}

BOOL ApImportList::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 11, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_UNDEFINED));
   m_imageList->Add(app->LoadIcon(IDI_ROUND));
   m_imageList->Add(app->LoadIcon(IDI_SQUARE));
   m_imageList->Add(app->LoadIcon(IDI_RECTANGLE));
   m_imageList->Add(app->LoadIcon(IDI_TARGET));
   m_imageList->Add(app->LoadIcon(IDI_THERMAL));
   m_imageList->Add(app->LoadIcon(IDI_COMPLEX));
   m_imageList->Add(app->LoadIcon(IDI_DONUT));
   m_imageList->Add(app->LoadIcon(IDI_OCTAGON));
   m_imageList->Add(app->LoadIcon(IDI_OBLONG));
   m_imageList->Add(app->LoadIcon(IDI_BLANK));

   m_listCtrl.SetImageList(m_imageList, LVSIL_SMALL);

   // columns
   LV_COLUMN column;
   CRect rect;
   m_listCtrl.GetWindowRect(&rect);
   int width = rect.Width() / 4;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = width;

   column.pszText = "Aperture Name";
   column.iSubItem = 0;
   m_listCtrl.InsertColumn(0, &column);

   column.pszText = "Size A";
   column.iSubItem = 1;
   m_listCtrl.InsertColumn(1, &column);

   column.pszText = "Size B";
   column.iSubItem = 2;
   m_listCtrl.InsertColumn(2, &column);

   column.pszText = "Rotation";
   column.iSubItem = 3;
   m_listCtrl.InsertColumn(3, &column);

   AperStruct *p;
   CString buf;
   LV_ITEM item;
   int actualItem;
   int j = 0;

   // items
   p = *head;
   while (p != NULL)
   {
      buf = prefix;
      buf += p->Name; // " prefix " + " D_Code"

      item.mask = LVIF_TEXT | LVIF_IMAGE;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = (char*)(LPCTSTR)buf;
      item.iImage = p->Type;
      actualItem = m_listCtrl.InsertItem(&item);
//    if (actualItem == -1) ErrorMessage("Insert Failed");
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_listCtrl.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_listCtrl.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_listCtrl.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_listCtrl.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_listCtrl.InsertItem(&item);
               }
            }
         }
      }

      item.mask = LVIF_TEXT;
      item.iItem = actualItem;
      item.iSubItem = 1;
      buf.Format("%.*lf", decimals, p->Size_a);
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);

      item.iSubItem = 2;
      switch (p->Type)
      {
         case T_RECTANGLE:
         case T_DONUT:
         case T_THERMAL:
         case T_OBLONG:
            buf.Format("%.*lf", decimals, p->Size_b);
         break;
         default:
            buf = "";
         break;
      }
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);

      item.iSubItem = 3;
      if (p->Rotation)
         buf.Format("%d", p->Rotation);
      else 
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);

      p = p->next;
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ApImportList::OnHelp() 
{ 
	//getApp().OnHelpIndex(); 
}

