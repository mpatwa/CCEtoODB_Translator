// $Header: /CAMCAD/4.6/FileType.cpp 32    5/24/07 5:23p Lynn Phung $
 
#include "StdAfx.h"
#include "General.h"
#include "DBUTIL.H"
#include "CCEtoODB.h"
#include "filetype.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int lastImport = -1;
int lastExport = -1;

/////////////////////////////////////////////////////////////////////////////
// CFileTypeDialog dialog
CFileTypeDialog::CFileTypeDialog(FileTypeTag *FileType, CWnd* pParent /*=NULL*/)
   : CResizingDialog(CFileTypeDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFileTypeDialog)
   m_sel = -1;
   //}}AFX_DATA_INIT

   m_fileType = FileType;

   CResizingDialogField& f1 = addFieldControl(IDC_LICENSED_MODULES  ,anchorLeft ,growProportionalHorizontal);
   f1.getOperations().addOperation(growVertical);

   CResizingDialogField& f2 = addFieldControl(IDC_UNLICENSED_MODULES,anchorRight,growVertical);
   f2.getOperations().addOperation(glueLeftEdge,toRightEdge,&f1);

   addFieldControl(IDOK                  ,anchorBottom);
   addFieldControl(IDCANCEL              ,anchorBottom);
   addFieldControl(IDC_HELP2             ,anchorBottom);
   addFieldControl(IDC_STATIC1           ,anchorLeft  );

   CResizingDialogField& f3 = addFieldControl(IDC_STATIC2           ,anchorRight );
   f3.getOperations().addOperation(glueLeftEdge,toRightEdge,&f1);
}

void CFileTypeDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFileTypeDialog)
   DDX_Control(pDX, IDC_UNLICENSED_MODULES, m_UnlicensedLB);
   DDX_Control(pDX, IDC_LICENSED_MODULES, m_LicensedLB);
   DDX_LBIndex(pDX, IDC_LICENSED_MODULES, m_sel);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileTypeDialog, CResizingDialog)
   //{{AFX_MSG_MAP(CFileTypeDialog)
   ON_LBN_DBLCLK(IDC_LICENSED_MODULES, OnOK)
   ON_BN_CLICKED(IDHELP, OnHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTypeDialog message handlers
BOOL CFileTypeDialog::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   CamcadLicense& camcadLicense = getApp().getCamcadLicense();
   /*for (POSITION pos=camcadLicense.getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = camcadLicense.getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         CString displayLicenseName = camcadProduct->getCamcadDisplayName();

   #ifdef SHAREWARE // put default readers in Licensed and all else in Unlicensed
         if (camcadProduct->getAccessTypeFlag() & ACCESS_INPUT)
         {
            if (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT)
               m_LicensedLB.SetItemData(m_LicensedLB.AddString(displayLicenseName), (int)camcadProduct->getCamcadProductId());
            else
               m_UnlicensedLB.AddString(discamcadProduct->getAccessTypeFlag()playLicenseName); // no access code
         }
   #else

   // Used to filter out reader licenses from visibility in unspecified builds.
   //		Currently, Release w/ Debug Info, Release Test and Debug are not filtering licenses.
   //		All other builds filter licenses specified.
   #	if !defined _RDEBUG && !defined _RELTEST && !defined _DEBUG
   //#error expressionIsTrue
		   // licenses to omit when in testing phase for any builds not mentioned
		   if (camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST)
			   continue;
   #	endif

         if (camcadProduct->getAccessTypeFlag() & ACCESS_INPUT && camcadProduct->getProductTypeFlag() & Product)
         {

			   if (camcadProduct->getAllowed() || camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST)
               m_LicensedLB.SetItemData(m_LicensedLB.AddString(displayLicenseName), (int)camcadProduct->getCamcadProductId());
            else
               m_UnlicensedLB.AddString(displayLicenseName); // no access code
         }
   #endif
      }
   }*/

   m_LicensedLB.SetCurSel(lastImport);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileTypeDialog::OnOK() 
{
   UpdateData();

   int sel = m_LicensedLB.GetItemData(m_sel);
   if (sel == LB_ERR)
   {
      ErrorMessage("You must select a format from the list of Licensed Modules.", "Select a format");
      return;
   }
   else
   {
      /*CamcadProduct* camcadProduct = getApp().getCamcadLicense().findCamcadProduct((CamcadProductIdTag)sel);
      if (camcadProduct != NULL)
      {
         *m_fileType = camcadProduct->getCamcadFileType();

         if (*m_fileType < 0)
         {
            ErrorMessage("This Reader Not Yet Supported.");
            return;
         }

         lastImport = m_sel;
      }*/
   }

   CResizingDialog::OnOK();
}

void CFileTypeDialog::WinHelp(DWORD dwData, UINT nCmd) 
{
   // TODO: Add your specialized code here and/or call the base class
   
   CResizingDialog::WinHelp(dwData, nCmd);
}

/////////////////////////////////////////////////////////////////////////////
// FileExportType dialog
FileExportType::FileExportType(int *FileType, CWnd* pParent /*=NULL*/)
   : CResizingDialog(FileExportType::IDD, pParent)
{
   //{{AFX_DATA_INIT(FileExportType)
   m_sel = -1;
   //}}AFX_DATA_INIT
   fileType = FileType;

   CResizingDialogField& f1 = addFieldControl(IDC_LICENSED_MODULES  ,anchorLeft ,growProportionalHorizontal);
   f1.getOperations().addOperation(growVertical);

   CResizingDialogField& f2 = addFieldControl(IDC_UNLICENSED_MODULES,anchorRight,growVertical);
   f2.getOperations().addOperation(glueLeftEdge,toRightEdge,&f1);

   addFieldControl(IDOK                  ,anchorBottom);
   addFieldControl(IDCANCEL              ,anchorBottom);

   CResizingDialogField& f3 = addFieldControl(IDC_STATIC2           ,anchorRight );
   f3.getOperations().addOperation(glueLeftEdge,toRightEdge,&f1);
}

void FileExportType::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FileExportType)
   DDX_Control(pDX, IDC_UNLICENSED_MODULES, m_UnlicensedLB);
   DDX_Control(pDX, IDC_LICENSED_MODULES, m_LicensedLB);
   DDX_LBIndex(pDX, IDC_LICENSED_MODULES, m_sel);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FileExportType, CResizingDialog)
   //{{AFX_MSG_MAP(FileExportType)
   ON_LBN_DBLCLK(IDC_LICENSED_MODULES, OnOK)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FileExportType message handlers
BOOL FileExportType::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   CamcadLicense& camcadLicense = getApp().getCamcadLicense();
   /*for (POSITION pos=camcadLicense.getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = camcadLicense.getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         CString displayLicenseName = camcadProduct->getCamcadDisplayName();

   #ifdef SHAREWARE // put all writers in Unlicensed
         if (lcamcadProduct->getAccessTypeFlag() & ACCESS_OUTPUT)
            m_UnlicensedLB.AddString(displayLicenseName); // no access code
   #else

   // Used to filter out writer licenses from visibility in unspecified builds.
   //		Currently, Release w/ Debug Info, Release Test and Debug are not filtering licenses.
   //		All other builds filter licenses specified.
   #	if !defined _RDEBUG && !defined _RELTEST && !defined _DEBUG
		   //// licenses to omit when in testing phase for any builds not mentioned
		   if (camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST)
			   continue;
   #	endif

		   if (camcadProduct->getAccessTypeFlag() & ACCESS_OUTPUT && camcadProduct->getProductTypeFlag() & Product)
		   {
			   if (camcadProduct->getAllowed() || camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST)
				   m_LicensedLB.SetItemData(m_LicensedLB.AddString(displayLicenseName), (int)camcadProduct->getCamcadProductId());
			   else
			   {
				   if ((!(Product & PRODUCT_VISION) || (camcadProduct->getCamcadProductId() != camcadProductGerberWrite && 
                     camcadProduct->getCamcadProductId() != camcadProductHpglWrite)) &&
                   (camcadProduct->getCamcadProductId() != camcadProductUnicamWrite))
					   m_UnlicensedLB.AddString(displayLicenseName); // no access code
			   }
		   }
   #endif
      }
   }*/

   m_LicensedLB.SetCurSel(lastExport);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FileExportType::OnOK() 
{
   UpdateData();

   int sel = m_LicensedLB.GetItemData(m_sel);
   if (sel == LB_ERR)
   {
      ErrorMessage("You must select a format from the list of Licensed Modules.", "Select a format");
      return;
   }
   else
   {
      /*CamcadProduct* camcadProduct = getApp().getCamcadLicense().findCamcadProduct((CamcadProductIdTag)sel);
      if (camcadProduct != NULL)
      {
         *fileType = (int)camcadProduct->getCamcadFileType();

         if (*fileType < 0)
         {
            ErrorMessage("This Writer Not Yet Supported.");
            return;
         }

         lastExport = m_sel;
      }*/
   }

   CResizingDialog::OnOK();
}

void CFileTypeDialog::OnHelp() 
{ 
	//getApp().OnHelpIndex(); 
}

/////////////////////////////////////////////////////////////////////////////
// MentorFileType dialog
MentorFileType::MentorFileType(CWnd* pParent /*=NULL*/)
   : CResizingDialog(MentorFileType::IDD, pParent)
{
   //{{AFX_DATA_INIT(MentorFileType)
   m_format = -1;
   //}}AFX_DATA_INIT
}

void MentorFileType::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MentorFileType)
   DDX_Radio(pDX, IDC_RADIO1, m_format);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MentorFileType, CResizingDialog)
   //{{AFX_MSG_MAP(MentorFileType)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MentorFileType message handlers
void MentorFileType::OnOK() 
{
   UpdateData();

   if (m_format < 0 || m_format > 6)
      return;
   
   CResizingDialog::OnOK();
}

// end FILETYPE.CPP
