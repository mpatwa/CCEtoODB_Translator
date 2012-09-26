// $Header: /CAMCAD/4.5/HP5DX.cpp 15    1/27/05 7:43p Kurt Van Ness $

#include "StdAfx.h"
#include "General.h"
#include "CCEtoODB.h"
#include "HP5DX.h"
#include "DirDlg.h"
#include "dbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HP_Export dialog
HP_Export::HP_Export(CWnd* pParent /*=NULL*/)
   : CDialog(HP_Export::IDD, pParent)
{
   //{{AFX_DATA_INIT(HP_Export)
   m_dir = _T("");
   m_name = _T("");
   //}}AFX_DATA_INIT
}

void HP_Export::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HP_Export)
   DDX_Text(pDX, IDC_DIR, m_dir);
   DDX_Text(pDX, IDC_NAME, m_name);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(HP_Export, CDialog)
   //{{AFX_MSG_MAP(HP_Export)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void HP_Export::OnBrowse() 
{
   UpdateData();

   CBrowse dlg;
   dlg.m_strSelDir = m_dir;
   dlg.m_strTitle = "Select path for exported HP 5DX files";
   if (dlg.DoBrowse())
   {
      m_dir = dlg.m_strPath;
      UpdateData(FALSE);
   }
}

void HP_Export::OnOK() 
{
   UpdateData();

   if (m_name.GetLength() > 26)
   {
      ErrorMessage("26 characters max", "Name Length Violation", MB_ICONSTOP | MB_OK);
      return;
   }

   int i;
   if ((i = m_name.FindOneOf(NotAllowed)) != -1)
   {
      if (m_name[i] == ' ')
         ErrorMessage("[SPACE]", "Illegal Character", MB_ICONSTOP | MB_OK);
      else
      {
         char buf[2];
         buf[0] = m_name[i];
         buf[1] = 0;
         ErrorMessage(buf, "Illegal Character", MB_ICONSTOP | MB_OK);
      }
      return;
   }

   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// HP_DeafultSettingsDlg dialog
HP_DeafultSettingsDlg::HP_DeafultSettingsDlg(CString defaultStageSpeed, CWnd* pParent /*=NULL*/)
   : CDialog(HP_DeafultSettingsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(HP_DeafultSettingsDlg)
   m_boardThickness = _T("");
   m_rotation = _T("");
   m_stageSpeed = _T("");
   m_defaultStageSpeed = defaultStageSpeed;
   m_units = -1;
   m_nonorthogonal = TRUE;
	m_enableAlphaNumericePnNumbers = FALSE;
   m_material = -1;
   m_othermaterial = _T("");
   //}}AFX_DATA_INIT
}

void HP_DeafultSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HP_DeafultSettingsDlg)
   DDX_Control(pDX, IDC_MATERIAL_OTHER, m_ctl_othermaterial);
   DDX_Text(pDX, IDC_BOARD_THICKNESS, m_boardThickness);
   DDX_CBString(pDX, IDC_ROTATION_CB, m_rotation);
   DDX_CBString(pDX, IDC_STAGE_SPEED_CB, m_stageSpeed);
   DDX_CBIndex(pDX, IDC_UNITS_CB, m_units);
   DDX_Check(pDX, IDC_NONORTHOGONAL, m_nonorthogonal);
   DDX_Radio(pDX, IDC_MATERIAL, m_material);
   DDX_Text(pDX, IDC_MATERIAL_OTHER, m_othermaterial);
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_EnableAlphaNumericPinNumber, m_enableAlphaNumericePnNumbers);
}

BEGIN_MESSAGE_MAP(HP_DeafultSettingsDlg, CDialog)
   //{{AFX_MSG_MAP(HP_DeafultSettingsDlg)
   ON_CBN_SELCHANGE(IDC_ROTATION_CB, OnSelchangeRotationCb)
   ON_CBN_SELCHANGE(IDC_UNITS_CB, OnSelchangeUnitsCb)
   ON_BN_CLICKED(IDC_RESET, OnReset)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL HP_DeafultSettingsDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   fromUnits = m_units;
   m_material = 0;

   for (int i=0;i<materialcnt;i++)
   {
      char *lp;
      lp = materialarray->GetAt(i)->name.GetBuffer(0);
      m_ctl_othermaterial.AddString(lp);
   }
   OnReset();


	// Read the setting for m_enableAlphaNumericePnNumbers from the registry and overwrite the default setting
	CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
	LPCTSTR profileName = pApp->m_pszProfileName;
   pApp->m_pszProfileName = _tcsdup("Imports");
	m_enableAlphaNumericePnNumbers = pApp->GetProfileInt("HP5DX", "EnableAlphanumericPins", 0)==1?TRUE:FALSE;
   free((void*)pApp->m_pszProfileName);
   pApp->m_pszProfileName = profileName;  

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void HP_DeafultSettingsDlg::OnReset() 
{
   m_rotation = "0";
   m_stageSpeed = validatedStageSpeed(m_defaultStageSpeed);
   fromUnits = m_units = page_units; // 0=Inches, 1=Mils, 2=mm
   m_nonorthogonal = TRUE;
	m_enableAlphaNumericePnNumbers = FALSE;

   // default value
   m_boardThickness.Format("%1.3lf", thickness);

   UpdateData(FALSE);
}

CString HP_DeafultSettingsDlg::validatedStageSpeed(CString stageSpeedStr)
{
   // Range is 1..20. Default to 2 if out of range, including blank.
   // Reset the string value based on int value, rather  than just return
   // original string if found to be in range. This removes extra fluff that
   // might be in the string, which possibly came from .out file, and could
   // be anything. E.g. "20 hi mom" will come back as int 20, and be acceptable.
   // But we don't want to drag the "hi mom" along.

   int validStageSpeed = 2; // Overall default

   if (!stageSpeedStr.IsEmpty())
   {
      int stageSpeed = atoi(stageSpeedStr);
      if (stageSpeed >= 1 && stageSpeed <= 20) // Valid range 1..20
      {
         validStageSpeed = stageSpeed;
      }
      // else validStageSpeed is already defaulted to 2
   }

   CString validatedStageSpeedStr;
   validatedStageSpeedStr.Format("%d", validStageSpeed);

   return validatedStageSpeedStr;
}


void HP_DeafultSettingsDlg::OnSelchangeRotationCb() 
{
   if (ErrorMessage("Are you sure you want to rotate the panel on output?", "Warning", MB_YESNO) == IDNO)
   {
      m_rotation = "0";
      UpdateData(FALSE);
   }  
}

void HP_DeafultSettingsDlg::OnSelchangeUnitsCb() 
{
   UpdateData();
   
   if (m_units == -1)
      return;

   int decimals;

   switch (m_units)
   {
   case 0: // inches
      decimals = GetDecimals(UNIT_INCHES);
      switch (fromUnits)
      {
      case 1: // mils
         m_boardThickness.Format("%.*lf", decimals, atof(m_boardThickness) * Units_Factor(UNIT_MILS, UNIT_INCHES));
         break;
      case 2: // mm
         m_boardThickness.Format("%.*lf", decimals, atof(m_boardThickness) * Units_Factor(UNIT_MM, UNIT_INCHES));
         break;
      }
      break;
   case 1: // mils
      decimals = GetDecimals(UNIT_MILS);
      switch (fromUnits)
      {
      case 0: // inches
         m_boardThickness.Format("%.*lf", decimals, atof(m_boardThickness) * Units_Factor(UNIT_INCHES, UNIT_MILS));
         break;
      case 2: // mm
         m_boardThickness.Format("%.*lf", decimals, atof(m_boardThickness) * Units_Factor(UNIT_MM, UNIT_MILS));
         break;
      }
      break;
   case 2: // mm
      decimals = GetDecimals(UNIT_MM);
      switch (fromUnits)
      {
      case 0: // inches
         m_boardThickness.Format("%.*lf", decimals, atof(m_boardThickness) * Units_Factor(UNIT_INCHES, UNIT_MM));
         break;
      case 1: // mils
         m_boardThickness.Format("%.*lf", decimals, atof(m_boardThickness) * Units_Factor(UNIT_MILS, UNIT_MM));
         break;
      }
      break;
   }

   fromUnits = m_units;
   
   UpdateData(FALSE);
}

void HP_DeafultSettingsDlg::OnOK() 
{
   UpdateData();

   switch (m_material)
   {
   case 0:
      material = "6337_cu";
      break;
   case 1:
      material = "6337";
      break;
   case 2:
      if (m_othermaterial.IsEmpty())
      {
         ErrorMessage("You must enter in a material type.", "", MB_ICONEXCLAMATION);
         return;
      }
      material = m_othermaterial;
   }
   

	// Save the setting for m_enableAlphaNumericePnNumbers to the registry
	CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
	LPCTSTR profileName = pApp->m_pszProfileName;
   pApp->m_pszProfileName = _tcsdup("Imports");	
	pApp->WriteProfileInt("HP5DX", "EnableAlphanumericPins", m_enableAlphaNumericePnNumbers==TRUE?1:0);
   free((void*)pApp->m_pszProfileName);
   pApp->m_pszProfileName = profileName;  


   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// HP_BoardNames dialog
HP_BoardNames::HP_BoardNames(CWnd* pParent /*=NULL*/)
   : CDialog(HP_BoardNames::IDD, pParent)
{
   //{{AFX_DATA_INIT(HP_BoardNames)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void HP_BoardNames::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HP_BoardNames)
   DDX_Control(pDX, IDC_LIST, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(HP_BoardNames, CDialog)
   //{{AFX_MSG_MAP(HP_BoardNames)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL HP_BoardNames::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 3;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = width;

   column.pszText = "CAMCAD Board Name";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.pszText = "5DX Primary Board Side";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "5DX Secondary Board Side";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   // fill list
   for (int i=0; i<maxArray; i++)
   {
      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      item.pszText = array[i].geomname.GetBuffer(0);
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
      m_list.SetItemData(actualItem, i);

      item.iItem = actualItem;

      item.iSubItem = 1;
      item.pszText = array[i].topname.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 2;
      item.pszText = array[i].bottomname.GetBuffer(0);
      m_list.SetItem(&item);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void HP_BoardNames::OnEdit() 
{
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

   int index = m_list.GetItemData(selItem);

   HP_BoardNamesEdit dlg;
   dlg.m_camcad = array[index].geomname;
   dlg.m_top = array[index].topname;
   dlg.m_bottom = array[index].bottomname;
   if (dlg.DoModal() == IDOK)
   {
      array[index].topname = dlg.m_top;
      array[index].bottomname = dlg.m_bottom;
      
      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = selItem;

      item.iSubItem = 1;
      item.pszText = array[index].topname.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 2;
      item.pszText = array[index].bottomname.GetBuffer(0);
      m_list.SetItem(&item);
   }
}

void HP_BoardNames::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// HP_BoardNamesEdit dialog
HP_BoardNamesEdit::HP_BoardNamesEdit(CWnd* pParent /*=NULL*/)
   : CDialog(HP_BoardNamesEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(HP_BoardNamesEdit)
   m_bottom = _T("");
   m_camcad = _T("");
   m_top = _T("");
   //}}AFX_DATA_INIT
}

void HP_BoardNamesEdit::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HP_BoardNamesEdit)
   DDX_Text(pDX, IDC_BOTTOM, m_bottom);
   DDX_Text(pDX, IDC_CAMCAD, m_camcad);
   DDX_Text(pDX, IDC_TOP, m_top);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(HP_BoardNamesEdit, CDialog)
   //{{AFX_MSG_MAP(HP_BoardNamesEdit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void HP_BoardNamesEdit::OnOK() 
{
   UpdateData();

   // top
   if (m_top.GetLength() > 40)
   {
      ErrorMessage("40 characters max", "Name Length Violation", MB_ICONSTOP | MB_OK);
      return;
   }

   int i;
   if ((i = m_top.FindOneOf(NotAllowed)) != -1)
   {
      if (m_top[i] == ' ')
         ErrorMessage("[SPACE]", "Illegal Character", MB_ICONSTOP | MB_OK);
      else
      {
         char buf[2];
         buf[0] = m_top[i];
         buf[1] = 0;
         ErrorMessage(buf, "Illegal Character", MB_ICONSTOP | MB_OK);
      }
      return;
   }

   // bottom
   if (m_bottom.GetLength() > 40)
   {
      ErrorMessage("40 characters max", "Name Length Violation", MB_ICONSTOP | MB_OK);
      return;
   }

   if ((i = m_bottom.FindOneOf(NotAllowed)) != -1)
   {
      if (m_bottom[i] == ' ')
         ErrorMessage("[SPACE]", "Illegal Character", MB_ICONSTOP | MB_OK);
      else
      {
         char buf[2];
         buf[0] = m_bottom[i];
         buf[1] = 0;
         ErrorMessage(buf, "Illegal Character", MB_ICONSTOP | MB_OK);
      }
      return;
   }

   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// HP_DirTree dialog
HP_DirTree::HP_DirTree(CWnd* pParent /*=NULL*/)
   : CDialog(HP_DirTree::IDD, pParent)
{
   //{{AFX_DATA_INIT(HP_DirTree)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

HP_DirTree::~HP_DirTree()
{
   if (m_imageList)
      delete m_imageList;
}

void HP_DirTree::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HP_DirTree)
   DDX_Control(pDX, IDC_TREE1, m_tree);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(HP_DirTree, CDialog)
   //{{AFX_MSG_MAP(HP_DirTree)
   ON_BN_CLICKED(IDC_PRINT, OnPrint)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define iFolder      0
#define iOpenFolder  1
BOOL HP_DirTree::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 2, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   HTREEITEM root;
   CString buf;
   root = m_tree.InsertItem(panelHashName, iFolder, iOpenFolder);
   for (int i=0; i<maxArray; i++)
   {
      if (!array[i].tophashname.IsEmpty())
      {
         buf.Format("%s (%s)", array[i].tophashname, array[i].topname); 
         m_tree.InsertItem(buf, iFolder, iOpenFolder, root);
      }
      if (!array[i].bottomhashname.IsEmpty())
      {
         buf.Format("%s (%s)", array[i].bottomhashname, array[i].bottomname); 
         m_tree.InsertItem(buf, iFolder, iOpenFolder, root);
      }
   }

   m_tree.Expand(root, TVE_EXPAND);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void HP_DirTree::OnPrint() 
{ 
   CDC dc;
   CPrintDialog dlg(FALSE); // FALSE - not print setup dialog
   
   if (dlg.DoModal() != IDOK) return;

   dc.Attach(dlg.GetPrinterDC());

   if (!dc.m_hDC) return;

   DOCINFO di;
   ::ZeroMemory(&di, sizeof(DOCINFO));
   di.cbSize = sizeof(DOCINFO);
   di.lpszDocName = "5DX Directory Tree"; // title of print document

   dc.StartDoc(&di);
   dc.StartPage();

   CString buf;
   int y, 
         hRes = dc.GetDeviceCaps(HORZRES), // pixels wide
         vRes = dc.GetDeviceCaps(VERTRES); // pixels high

   TEXTMETRIC met;
   dc.GetTextMetrics(&met);
   int space = met.tmHeight;

   // Header & Footer
   {
      // header 
      dc.MoveTo(0, space); // header line
      dc.LineTo(hRes, space); 
      dc.TextOut(0, 0, "5DX Directory Tree");
   
      // footer
      dc.MoveTo(0, vRes - space*2 - 20); // footer line
      dc.LineTo(hRes, vRes - space*2 - 20);  
   
      CTime t = t.GetCurrentTime(); // footer text
      buf.Format("Created by CCE to ODB++ on %s", t.Format("%A, %B %d, %Y at %I:%M:%S %p"));
      dc.TextOut(0, vRes - space*2, buf);
      buf = getApp().getCompanyNameString();
      buf += " ";
      buf += getApp().getCopyrightString();
      buf += " ";
      buf += getApp().getAllRightsReservedString();
      dc.TextOut(0, vRes - space, buf);
   }


   y = space * 2;
   dc.TextOut(0, y, panelHashName);
   for (int i=0; i<maxArray; i++)
   {
      if (!array[i].tophashname.IsEmpty())
      {
         y += space;
         buf.Format("%s (%s)", array[i].tophashname, array[i].topname); 
         dc.TextOut(space, y, buf);
         dc.MoveTo(space/2, y+space/2);
         dc.LineTo(space, y+space/2);
      }
      if (!array[i].bottomhashname.IsEmpty())
      {
         y += space;
         buf.Format("%s (%s)", array[i].bottomhashname, array[i].bottomname); 
         dc.TextOut(space, y, buf);
         dc.MoveTo(space/2, y+space/2);
         dc.LineTo(space, y+space/2);
      }
   }

   // tree lines
   dc.MoveTo(space/2, space*3);
   dc.LineTo(space/2, y+space/2);

   dc.EndPage();
   dc.EndDoc();
}


/////////////////////////////////////////////////////////////////////////////
// HP_PackageMap dialog
HP_PackageMap::HP_PackageMap(CWnd* pParent /*=NULL*/)
   : CDialog(HP_PackageMap::IDD, pParent)
{
   //{{AFX_DATA_INIT(HP_PackageMap)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void HP_PackageMap::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HP_PackageMap)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(HP_PackageMap, CDialog)
   //{{AFX_MSG_MAP(HP_PackageMap)
   ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, OnEndlabeleditList1)
   ON_BN_CLICKED(IDC_AUTO_GEN, OnAutoGen)
   ON_BN_CLICKED(IDC_PRINT, OnPrint)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL HP_PackageMap::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.pszText = "Package ID";
   column.iSubItem = 0;
   column.cx = width * 3;
   m_list.InsertColumn(0, &column);

   column.pszText = "Land Pattern";
   column.iSubItem = 1;
   column.cx = width * 3;
   m_list.InsertColumn(1, &column);

   column.pszText = "Ref Prefix";
   column.iSubItem = 2;
   column.cx = width * 2;
   m_list.InsertColumn(2, &column);

   FillList();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void HP_PackageMap::FillList()
{
   m_list.DeleteAllItems();

   // fill list
   for (int i=0; i<maxArray; i++)
   {
      if (!array->GetAt(i)->used)
         continue;

      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = i;
      item.iSubItem = 0;
      item.pszText = array->GetAt(i)->package_id.GetBuffer(0);
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
      m_list.SetItemData(actualItem, i);

      item.iItem = actualItem;

      item.iSubItem = 1;
      item.pszText = array->GetAt(i)->geomname.GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 2;
      item.pszText = array->GetAt(i)->prefix.GetBuffer(0);
      m_list.SetItem(&item);
   }
}

void HP_PackageMap::OnEndlabeleditList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

   *pResult = 0;

   char *text = pDispInfo->item.pszText;

   if (!text)
      return;

   int count = m_list.GetItemCount();
   if (!count) return;

	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   int index = m_list.GetItemData(selItem);

   array->GetAt(index)->package_id = text;
   array->GetAt(index)->changed = TRUE;
   *pResult = 1;
}

void HP_PackageMap::OnAutoGen() 
{
   for (int i=0; i<maxArray; i++)
   {
      HP5DXF_PackageMap *pm = array->GetAt(i);
      if (pm->package_id.IsEmpty())
      {
         pm->package_id.Format("%s_%s", pm->geomname, pm->prefix);
         pm->changed = TRUE;
      }
   }  
   FillList();
}

#define col1 (0)
#define col2 (hRes/8*3)
#define col3 (hRes/8*6)
void HP_PackageMap::OnPrint() 
{
   CDC dc;
   CPrintDialog dlg(FALSE); // FALSE - not print setup dialog
   
   if (dlg.DoModal() != IDOK) return;

   dc.Attach(dlg.GetPrinterDC());

   if (!dc.m_hDC) return;

   DOCINFO di;
   ::ZeroMemory(&di, sizeof(DOCINFO));
   di.cbSize = sizeof(DOCINFO);
   di.lpszDocName = "5DX Package Map"; // title of print document

   dc.StartDoc(&di);
   dc.StartPage();

   CString buf;
   int y, 
         hRes = dc.GetDeviceCaps(HORZRES), // pixels wide
         vRes = dc.GetDeviceCaps(VERTRES); // pixels high

   TEXTMETRIC met;
   dc.GetTextMetrics(&met);
   int space = met.tmHeight;

   // Header & Footer
   {
      // header 
      dc.MoveTo(0, space); // header line
      dc.LineTo(hRes, space); 
      dc.TextOut(0, 0, "5DX Package Map");
   
      // footer
      dc.MoveTo(0, vRes - space*2 - 20); // footer line
      dc.LineTo(hRes, vRes - space*2 - 20);  
   
      CTime t = t.GetCurrentTime(); // footer text
      buf.Format("Created by CCE to ODB++ on %s", t.Format("%A, %B %d, %Y at %I:%M:%S %p"));
      dc.TextOut(0, vRes - space*2, buf);
      buf = getApp().getCompanyNameString();
      buf += " ";
      buf += getApp().getCopyrightString();
      buf += " ";
      buf += getApp().getAllRightsReservedString();
      dc.TextOut(0, vRes - space, buf);
   }

   y = space * 2;
   dc.TextOut(col1, y, "Package ID");
   dc.TextOut(col2, y, "Land Pattern");
   dc.TextOut(col3, y, "Reference Prefix");

   y += space;
   dc.MoveTo(0, y); 
   dc.LineTo(hRes, y);  

   for (int i=0; i<maxArray; i++)
   {
      HP5DXF_PackageMap *pm = array->GetAt(i);

      if (!pm->used)
         continue;

      y += space;
      
      dc.TextOut(col1, y, pm->package_id);
      dc.TextOut(col2, y, pm->geomname);
      dc.TextOut(col3, y, pm->prefix);
   }

   dc.EndPage();
   dc.EndDoc();
}



