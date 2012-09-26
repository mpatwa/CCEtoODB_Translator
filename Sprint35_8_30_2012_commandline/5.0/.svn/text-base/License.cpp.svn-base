// $Header: /CAMCAD/4.6/License.cpp 66    5/24/07 5:23p Lynn Phung $
                     
#include "stdafx.h"
#include "CCEtoODB.h"
#include "License.h"
#include "mainfrm.h"
#include "crypt.h"
#include "splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
   File format for Wincad LIC File
      40 char + 1 for Name
      40 char + 1 for Comp
      1000 X unligned long for license pointers

   License should work the following
      No license file generates a 0 serial number
      0 serial number is used for demo software

   To obtain a license the following scenario
      There is a LICGEN WIndows program at Mentor Graphics
      A customer gets a generic floppy send and a fax paper
      which has 
      NAME:
      COMP:
      ACCESS CODES ...

      When opening screen CAMCAD shows, the software tries to find 
      the license file and reads NAME and COMP and serial number and 
      all access allowed.

      The customer than runs CAMCAD, and goes into ACCESSCODE
      and type his NAME and COMP and ACCESS code. While entering
      the access code, the software accepts or rejects an access code.

      When Exiting ACCESSCODE, the software writes a new license file.

      // We need to resolve NETWORK installation -> where to find the file.

*/

#define NO_CPRINT_MSG   "Other CAMCAD modules will not be checked out without this primary license."

extern BOOL LockLicenses;
extern License *licenses;

//void CheckOutLicenses(const char *company);

//unsigned long tempSerialNum;
//int           month, day, year;
//long          duration;
//int APILicenseLevel = 0;
char *LICENSE_FAULT = "License Fault";
//CString user, company, serialNum;
BOOL SecurityKeyUser = FALSE;

static BOOL NetworkUser;
static BOOL Admin;
//static BOOL HasExpiration; // from CAMCAD.CPP

#ifndef SHAREWARE // do not open or read or process license file
char *NO_C_EDIT = "You do not have a\n C_EDIT License!\n Refer to Help|About to find out how to obtain licenses.";
char *NO_C_PRINT = "You do not have a\n C_PRINT License!\n Refer to Help|About to find out how to obtain licenses.";
char *NO_FILE_FORMAT = "You do not have a License for this File Format!\n Refer to Help|About to find out how to obtain licenses.";
#else
char *NO_C_EDIT = "CAMCAD Shareware does not allow EDIT!\nTo obtain this permission, must must purchase this option.";
char *NO_C_PRINT = "CAMCAD Shareware does not allow PRINT!\nTo obtain this permission, must must purchase this option.";
char *NO_FILE_FORMAT = "CAMCAD Shareware does not allow this File Format!\nTo obtain this permission, must must purchase this option.";
#endif

/******************************************************************************
* WarnExpiration
*/
//void WarnExpiration()
//{
//   if (!HasExpiration)
//      return;
//
//   if (!month)
//      return;
//   CTime expiration(year+2000, month, day, 23, 59, 59);  
//
//   CTime currentTime = CTime::GetCurrentTime();
//   if (currentTime > expiration)
//      return;
//
//   CTimeSpan span = expiration - currentTime;
//   if (span.GetDays() < 30)
//   {
//      CString buf;
//      buf.Format("Temporary License expires in %d days.", span.GetDays());
//      ErrorMessage(buf, "Warning");
//   }
//}
            
/******************************************************************************
* ValidateExpiration
*/
//BOOL ValidateExpiration()
//{
//   HasExpiration = FALSE;
//
//   if (!month)
//      return FALSE;
//   CTime expiration(year+2000, month, day, 23, 59, 59);  
//   CTime begining(year+2000, month, day, 1, 0, 0);  
//   if (expiration < 0 || begining < 0)
//   {
//      ErrorMessage("Invalid Date", "Invalid Date", MB_OK | MB_ICONEXCLAMATION);
//      month = day = year = duration = 0;
//      return FALSE;
//   }
//
//   begining -= CTimeSpan(duration, 0, 0, 0);
//   CTime currentTime = CTime::GetCurrentTime();
//   BOOL valid = currentTime > begining;
//   BOOL expired = currentTime > expiration;
//   if (!valid)
//   {
//      ErrorMessage("Invalid Date", "Invalid Date", MB_OK | MB_ICONEXCLAMATION);
//      month = day = year = duration = 0;
//      return FALSE;
//   }
//   if (expired)
//   {
//      ErrorMessage("Temporary Access Codes Expired", "Expired");
//      return FALSE;
//   }
//
//   HasExpiration = TRUE;
//
//   return TRUE;
//}
//
static bool CheckLastUse()
{
	DateStruct date;
   unsigned int datenum = getApp().GetProfileInt("Settings", "LU", 0);
	if (datenum == 0)
		return true;

   memcpy(&date, &datenum, 4);
	CTime luTime(date.year, date.month, date.day, date.hour, date.minute, 0);
   CTime today = CTime::GetCurrentTime();

	bool retval = (today.GetTime() >= luTime.GetTime());
	return retval;
}

static void UpdateLastUse()
{
   unsigned int datenum;
   DateStruct date;
   CTime currentTime = CTime::GetCurrentTime();
   date.year = currentTime.GetYear();
   date.month = currentTime.GetMonth();
   date.day = currentTime.GetDay();
   date.hour = currentTime.GetHour();
   date.minute = currentTime.GetMinute();
   memcpy(&datenum, &date, 4);
   AfxGetApp()->WriteProfileInt("Settings", "LU", datenum);
}

int GetMonth(CString sMonth)
{
   if (sMonth == "jan")
      return 1;
   else
   if (sMonth == "feb")
      return 2;
   else
   if (sMonth == "mar")
      return 3;
   else
   if (sMonth == "apr")
      return 4;
   else
   if (sMonth == "may")
      return 5;
   else
   if (sMonth == "jun")
      return 6;
   else
   if (sMonth == "jul")
      return 7;
   else
   if (sMonth == "aug")
      return 8;
   else
   if (sMonth == "sep")
      return 9;
   else
   if (sMonth == "oct")
      return 10;
   else
   if (sMonth == "nov")
      return 11;
   else
   if (sMonth == "dec")
      return 12;
   else
      return 0;
}

CString getLicensePrefix()
{
   CString retStr;

   if (Product & PRODUCT_GRAPHIC)
      retStr = "CC_Gra_";
   else
   if (Product & PRODUCT_PROFESSIONAL)
      retStr = "CC_Pro_";
   else
   if (Product & PRODUCT_PCB_TRANSLATOR)
      retStr = "CC_PCB_";
   else
   if (Product & PRODUCT_VISION)
      retStr = "CC_Vis_";

	return retStr;
}

#ifdef DEADCODE
// This stuff was used prior to 4.6, not in use today in 4.10
bool haveMaintenance(CString &errMsg, CString &num_days)
{
#if defined (LicenseBypass)
   return true;
#endif
	
	LONGLONG iDaysLeft = 0;
	CCEtoODBApp &app = getApp();
	errMsg.Empty();

   // check for the maintainance date
   CString line;
   int maintenanceDay = 0, maintenanceMonth = 0, maintenanceYear = 0;
   char sTemp[32], sMonth[3], *curP;
   bool hasExpiration = false;

   CString moduleName = getLicensePrefix() + "Maintenance";
   if (app.flexlm.GetUserData(moduleName, FEATURE_EXPIRATIONDATE, line) != RET_NO_ERROR)
	{
		CString daysLeft;
		CTime today = CTime::GetCurrentTime();

		if (!CheckLastUse())
		{
			errMsg = "System time has been tampered with!";
			ErrorMessage(errMsg);
			return false;
		}

		unsigned int SSA1 = app.GetProfileInt("Settings", "SSA1", 1234567);
		if (SSA1 == 1234567)
		{
			// SSA1 didn't exist before
			unsigned int datenum;
			DateStruct date;
			date.year = today.GetYear();
			date.month = today.GetMonth();
			date.day = today.GetDay();
			date.hour = today.GetHour();
			date.minute = today.GetMinute();
			memcpy(&datenum, &date, 4);
			app.WriteProfileInt("Settings", "SSA1", datenum);
			daysLeft = "10";
			iDaysLeft = 10;
		}
		else
		{
			DateStruct date;
			memcpy(&date, &SSA1, 4);
			CTime SSATime(date.year, date.month, date.day, date.hour, date.minute, 0);
			CTimeSpan span = today - SSATime;
			daysLeft.Format("%d", 10 - span.GetDays());
			iDaysLeft = 10 - (long)span.GetDays();
		}
		
		CString companyName, serialNumber, hostID, hasDongle;

		moduleName = getLicensePrefix() + LIC_PRINT_S;
		app.flexlm.GetUserData(LIC_MAIN_DATA, FEATURE_COMPANY, companyName);
		app.flexlm.GetUserData(moduleName, FEATURE_SERIALNUMBER, serialNumber);
		app.flexlm.GetUserData(moduleName, FEATURE_HOSTID, hostID);
		app.flexlm.GetUserData(moduleName, FEATURE_USINGDONGLE, hasDongle);

		errMsg = "WARNING!!\n";
		errMsg += "The current license file is missing critical information required by CAMCAD.  Your\n";
		errMsg += "Software Support Agreement (SSA) expiration date cannot be verified.  This application\n";
		errMsg += "will be allowed to run for 10 days.  You must contact Mentor Graphics\n";
		errMsg += "immediately to update your SSA or revert to a previous version of CAMCAD\n";
		errMsg += "\n";
		errMsg += "When contacting Mentor Graphics, please provide the following information:\n";
		errMsg += (CString)"CAMCAD Version : " + getApp().getVersionString() + "\n";
		errMsg += (CString)"Company Name : " + companyName + "\n";
		errMsg += (CString)"Serial Number : " + serialNumber + "\n";
		
		if (hasDongle == "FALSE")
				errMsg += (CString)"Host ID : " + hostID + "\n";
			else
				errMsg += (CString)"Dongle ID : " + hostID + "\n";

		errMsg += (CString)"SSA Expiration : Not Available\n";
		errMsg += "\n";
		errMsg += "Web Site : http://www.mentor.com/supportnet\n";
		errMsg += "\n";
		
		if (iDaysLeft < 0)
		{
			num_days = "Your 10 days have elapsed.  You are not authorized to run this version of CAMCAD.";
		}
		else
		{
			num_days = (CString)"This version of CAMCAD will run for " + daysLeft + " more days.";
			
		}

		//num_days = (CString)"This version of CAMCAD will run for " + daysLeft + " more days.";
	
		UpdateLastUse();

		if (iDaysLeft < 0)
		{
			ErrorMessage(num_days);
			return false;
		}
		else
			return true;
	}

   strcpy(sTemp, line);
   if (curP = strtok(sTemp, "-"))
   {
      maintenanceDay = atoi(curP);
      curP = strtok(NULL, "-");
      strcpy(sMonth, curP);
      curP = strtok(NULL, "-");
      maintenanceYear = atoi(curP) - 2000;

      maintenanceMonth = GetMonth(sMonth);
		hasExpiration = (maintenanceYear > 0)?true:false;
   }

   // make sure there is an expiration date, if not, then they have maintenance
   if (!hasExpiration)
	{
		app.WriteProfileInt("Settings", "SSA1", 1234567);
		UpdateLastUse();
		return true;
	}

   CTime expiration(maintenanceYear+2000, maintenanceMonth, maintenanceDay, 23, 59, 59);  
   CTime begining(maintenanceYear+2000, maintenanceMonth, maintenanceDay, 1, 0, 0);  
   if (expiration < 0 || begining < 0)
   {
      //ErrorMessage("Invalid Date", "Invalid Date", MB_OK | MB_ICONEXCLAMATION);
      //month = day = year = duration = 0;
		errMsg = "You do not have maintenance to run this version of CAMCAD.\n\nPlease contact Mentor Graphics to continue working with this version.";
		ErrorMessage(errMsg);
		UpdateLastUse();
		return false;
   }

	// get the build date from the version string
	CString version = getApp().getVersionString();
	int datePos = version.ReverseFind('.');
	CString buildDate = version.Mid(datePos+1);
	if (buildDate.GetLength() == 6)
	{
		CString mYear = buildDate.Mid(0, 2), mMonth = buildDate.Mid(2, 2), mDay = buildDate.Mid(4, 2);
		CTime buildTime(atoi(buildDate.Mid(0, 2))+2000, atoi(buildDate.Mid(2, 2)), atoi(buildDate.Mid(4, 2)), 23, 59, 59);
		CTimeSpan tenDaySpan(30, 0, 0, 0);
		
		if (buildTime > expiration)
		{
			CString daysLeft;
			CTime today = CTime::GetCurrentTime();

			if (!CheckLastUse())
			{
				errMsg = "System time has been tampered with!";
				ErrorMessage(errMsg);
				return false;
			}

			unsigned int SSA1 = app.GetProfileInt("Settings", "SSA1", 1234567);
			if (SSA1 == 1234567)
			{
				// SSA1 didn't exist before
				unsigned int datenum;
				DateStruct date;
				date.year = today.GetYear();
				date.month = today.GetMonth();
				date.day = today.GetDay();
				date.hour = today.GetHour();
				date.minute = today.GetMinute();
				memcpy(&datenum, &date, 4);
				app.WriteProfileInt("Settings", "SSA1", datenum);
				daysLeft = "10";
				iDaysLeft = 10;
			}
			else
			{
				DateStruct date;
				memcpy(&date, &SSA1, 4);
				CTime SSATime(date.year, date.month, date.day, date.hour, date.minute, 0);
				CTimeSpan span = today - SSATime;
				daysLeft.Format("%d", 10 - span.GetDays());
				iDaysLeft = 10 - (long)span.GetDays();				
			}

			CString companyName, serialNumber, hostID, hasDongle;

		   moduleName = getLicensePrefix() + LIC_PRINT_S;
			app.flexlm.GetUserData(LIC_MAIN_DATA, FEATURE_COMPANY, companyName);
			app.flexlm.GetUserData(moduleName, FEATURE_SERIALNUMBER, serialNumber);
			app.flexlm.GetUserData(moduleName, FEATURE_HOSTID, hostID);
			app.flexlm.GetUserData(moduleName, FEATURE_USINGDONGLE, hasDongle);

			errMsg = "WARNING!!\n";
			errMsg += "The currently installed version of CAMCAD is newer than your Software Support\n";
			errMsg += "Agreement (SSA) expiration date.  This application will be allowed to run for 10 days.\n";
			errMsg += "You must contact Mentor Graphics immediately to renew your SSA or\n";
			errMsg += "revert to a previous version of CAMCAD\n";
			errMsg += "\n";
			errMsg += "When contacting Mentor Graphics, please provide the following information:\n";
			errMsg += (CString)"CAMCAD Version : " + getApp().getVersionString() + "\n";
			errMsg += (CString)"Company Name : " + companyName + "\n";
			errMsg += (CString)"Serial Number : " + serialNumber + "\n";
			
			if (hasDongle == "FALSE")
				errMsg += (CString)"Host ID : " + hostID + "\n";
			else
				errMsg += (CString)"Dongle ID : " + hostID + "\n";

			errMsg += (CString)"SSA Expiration : " + expiration.FormatGmt("%m/%d/%Y") + "\n";
			errMsg += "\n";
		   errMsg += "Web Site : http://www.mentor.com/supportnet\n";
			errMsg += "\n";
			if (iDaysLeft < 0)
			{
					errMsg = "You are not authorized to run this version of CAMCAD.  SSA Agreement has expired.";
					
			}
			else
				errMsg += (CString)"This version of CAMCAD will run for " + daysLeft + " more days.";

			UpdateLastUse();

			if (iDaysLeft < 0)
			{
				return false;
				ErrorMessage(errMsg);
			}
			else
				return true;
		}
		else if (expiration - CTime::GetCurrentTime() <= tenDaySpan)
		{
			CString daysLeft;
			CTimeSpan remainingTime(expiration - CTime::GetCurrentTime());
			daysLeft.Format("%d", remainingTime.GetDays());

			CString companyName, serialNumber, hostID, hasDongle;

		   moduleName = getLicensePrefix() + LIC_PRINT_S;
			app.flexlm.GetUserData(LIC_MAIN_DATA, FEATURE_COMPANY, companyName);
			app.flexlm.GetUserData(moduleName, FEATURE_SERIALNUMBER, serialNumber);
			app.flexlm.GetUserData(moduleName, FEATURE_HOSTID, hostID);
			app.flexlm.GetUserData(moduleName, FEATURE_USINGDONGLE, hasDongle);

			errMsg = "WARNING!!\n";
			errMsg += (CString)"Your Software Support Agreement will expire in " + daysLeft + " days.\n";
			errMsg += "\n";
			errMsg += "To ensure you are entitled to receive uninterrupted product updates and technical\n";
			errMsg += "support, please contact Mentor Graphics immediately to renew your.\n";
			errMsg += "Software Support Agreement (SSA).  Any version of CAMCAD which is newer than\n";
			errMsg += "your SSA expiration date will not operate on this computer.\n";
			errMsg += "\n";
			errMsg += "When contacting Mentor Graphics, please provide the following information:\n";
			errMsg += (CString)"CAMCAD Version : " + getApp().getVersionString() + "\n";
			errMsg += (CString)"Company Name : " + companyName + "\n";
			errMsg += (CString)"Serial Number : " + serialNumber + "\n";

			if (hasDongle == "FALSE")
				errMsg += (CString)"Host ID : " + hostID + "\n";
			else
				errMsg += (CString)"Dongle ID : " + hostID + "\n";


			errMsg += (CString)"SSA Expiration : " + expiration.FormatGmt("%m/%d/%Y") + "\n";
			errMsg += "\n";
		   errMsg += "Web Site : http://www.mentor.com/supportnet\n";
		}
	}
	else
		// no build date to check on, so just assume we can work
		return true;

	app.WriteProfileInt("Settings", "SSA1", 1234567);
	UpdateLastUse();

	return true;
}
#endif

LicenseDialog::LicenseDialog(CWnd* pParent /*=NULL*/)
   : CDialog(LicenseDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(LicenseDialog)
   m_Company = "";
   m_User = "";
   m_product = _T("");
   m_duration = 0;
   m_dongle = FALSE;
   m_temporary = FALSE;
   m_day = -1;
   m_month = -1;
   m_year = -1;
   //}}AFX_DATA_INIT
}

void LicenseDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LicenseDialog)
   DDX_Control(pDX, IDC_LICENCES, m_LicenseLB);
   DDX_Text(pDX, IDC_COMPANY, m_Company);
   DDX_Text(pDX, IDC_USER, m_User);
   DDX_Text(pDX, IDC_PRODUCT, m_product);
   DDX_Text(pDX, IDC_DURATION, m_duration);
   DDX_Check(pDX, IDC_DONGLE, m_dongle);
   DDX_Check(pDX, IDC_TEMPORARY, m_temporary);
   DDX_CBIndex(pDX, IDC_DAY_CB, m_day);
   DDX_CBIndex(pDX, IDC_MONTH_CB, m_month);
   DDX_CBIndex(pDX, IDC_YEAR_CB, m_year);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LicenseDialog, CDialog)
   //{{AFX_MSG_MAP(LicenseDialog)
   //ON_EN_CHANGE(IDC_COMPANY, OnChangeUser)
   ON_BN_CLICKED(IDHELP, OnHelp)
   //ON_LBN_DBLCLK(IDC_LICENCES, OnDblclkLicences)
   //ON_EN_CHANGE(IDC_DURATION, OnChangeDate)
   //ON_BN_CLICKED(IDC_TEMPORARY, OnTemporary)
   //ON_EN_CHANGE(IDC_USER, OnChangeUser)
   //ON_BN_CLICKED(IDC_DONGLE, OnChangeUser)
   //ON_CBN_SELCHANGE(IDC_MONTH_CB, OnChangeDate)
   //ON_CBN_SELCHANGE(IDC_DAY_CB, OnChangeDate)
   //ON_CBN_SELCHANGE(IDC_YEAR_CB, OnChangeDate)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(ID_RESCAN, OnBnClickedRescan)
END_MESSAGE_MAP()

BOOL LicenseDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Set Font to fixed so spaces are same width as chars (for columns)
   SendDlgItemMessage(IDC_LICENCES, WM_SETFONT, (WPARAM) GetStockObject(SYSTEM_FIXED_FONT), 0L);

   Admin = FALSE;
   //if (NetworkUser)
   // m_admin = 0;
   //else
   //{
   // Admin = TRUE;
   // m_admin = 2;
   //}
   
#ifdef SHAREWARE // Shareware or Professional in Frame Title
   m_product = "Shareware";
#else
   switch (Product)
   {
      case PRODUCT_GRAPHIC:
         m_product = "CAMCAD Graphic";
      break;
      case PRODUCT_PROFESSIONAL:
         m_product = "CAMCAD Professional";
      break;
      case PRODUCT_PCB_TRANSLATOR:
         m_product = "CAMCAD PCB Translator";
      break;
      case PRODUCT_VISION:
         m_product = "CAMCAD Vision";
      break;
   }
#endif

   DateStruct expirationDate = getApp().getCamcadLicense().getExpirationDate();
   if (expirationDate.month > 0)
   {
      m_duration = 0;
      m_month = expirationDate.month-1;
      m_day = expirationDate.day-1;
      m_year = expirationDate.year;
      m_temporary = TRUE;
   }
   else
   {
      m_duration = 0;
      m_month = m_day = m_year = -1;
      m_temporary = FALSE;
   }

   UpdateData(FALSE);

   FillListBox();
   Modified = FALSE;

   if (GetDlgItem(IDC_DURATION))
      GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_MONTH_CB))
      GetDlgItem(IDC_MONTH_CB)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_DAY_CB))
      GetDlgItem(IDC_DAY_CB)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_YEAR_CB))
      GetDlgItem(IDC_YEAR_CB)->EnableWindow(FALSE);
   //}
   
   if (GetDlgItem(IDC_COMPANY))
      GetDlgItem(IDC_COMPANY)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_USER))  
      GetDlgItem(IDC_USER)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_TEMPORARY))
      GetDlgItem(IDC_TEMPORARY)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_DONGLE))
      GetDlgItem(IDC_DONGLE)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_DURATION))
      GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_MONTH_CB))
      GetDlgItem(IDC_MONTH_CB)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_DAY_CB))
      GetDlgItem(IDC_DAY_CB)->EnableWindow(FALSE);

   if (GetDlgItem(IDC_YEAR_CB))
      GetDlgItem(IDC_YEAR_CB)->EnableWindow(FALSE);

   if (GetDlgItem(ID_RESCAN))
      GetDlgItem(ID_RESCAN)->EnableWindow(!LockLicenses);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

#define SPACING 34
void LicenseDialog::FillListBox()
{
   int topIndex = m_LicenseLB.GetTopIndex();
   m_LicenseLB.ResetContent();

   CamcadLicense& camcadLicense = getApp().getCamcadLicense();
   /*for (POSITION pos = camcadLicense.getProductStartPosition(); pos != NULL;)
	{
      CamcadProduct* camcadProduct = camcadLicense.getNextProduct(pos);
      if (camcadProduct != NULL)
      {

// Used to filter out licenses from visibility in unspecified builds.
//		Currently, Release w/ Debug Info, Release Test and Debug are not filtering licenses.
//		All other builds filter licenses specified.
#	if !defined _RDEBUG && !defined _RELTEST && !defined _DEBUG
		   //// licenses to omit when in testing phase for any builds not mentioned
		   if (camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST)
			   continue;
#	endif

		   if (Product & PRODUCT_VISION && (camcadProduct->getCamcadProductId() == camcadProductHpglWrite || camcadProduct->getCamcadProductId() == camcadProductGerberWrite))
			   continue;

         if (!(camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT) && camcadProduct->getProductTypeFlag() & Product)
		   {
            CString licenseName = camcadProduct->getCamcadDisplayName();

            if (camcadProduct->getMentorProductProId() == getApp().getCamcadLicense().getBaseMentorProductId() && camcadProduct->getCamcadProductId() != camcadProductPrint)
            {
               // Don't list anything that is linked to the base license except the Print because that will list the base product name
               continue;
            }

            int len = licenseName.GetLength();
            for (int s=0; s<(SPACING-len); s++)
               licenseName += ".";

            if (camcadProduct->getAllowed())
            {
               if (camcadProduct->isAPI())
                  licenseName += "API ENABLED";
               else
                  licenseName +=  "FULL ACCESS";
            }      
            else if (camcadProduct->isQueued())
            {
               licenseName += "QUEUED";
            }
            else
            {
               licenseName += "UNAVAILABLE";
            }

            m_LicenseLB.SetItemData(m_LicenseLB.AddString(licenseName), (int)camcadProduct->getCamcadProductId());

            //CString prefix = licenseName.Left(3);
            //prefix.MakeUpper();

            //if (prefix == "PRO" || prefix == "PCB" || prefix == "GPH" || prefix == "VSN" || licenseName.CompareNoCase("Unknown"))
            //{
            //   // Only list those has a prefix for the product or if it is unknown
            //   m_LicenseLB.SetItemData(m_LicenseLB.AddString(licenseName), (int)camcadProduct->getCamcadProductId());
            //}
         }
      }
	}*/

   m_LicenseLB.SetTopIndex(topIndex);
}

void LicenseDialog::OnBnClickedRescan()
{
   CWaitCursor wait;
   getApp().getCamcadLicense().checkOutLicenses(true);
   FillListBox();
   getApp().SetCAMCADTitle();
}

void LicenseDialog::OnHelp() 
{ 
	//getApp().OnHelpIndex(); 
}



/////////////////////////////////////////////////////////////////////////////
// LicenseNumber dialog
LicenseNumber::LicenseNumber(char *code, CWnd* pParent /*=NULL*/)
   : CDialog(LicenseNumber::IDD, pParent)
{
   m_code = code;
   //{{AFX_DATA_INIT(LicenseNumber)
   m_Text = _T("Enter Access Code");
   m_AccessCode = code;
   //}}AFX_DATA_INIT
}

void LicenseNumber::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LicenseNumber)
   DDX_Text(pDX, IDC_TEXT, m_Text);
   DDX_Text(pDX, IDC_LICENSE_NUM, m_AccessCode);
   DDV_MaxChars(pDX, m_AccessCode, 9);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LicenseNumber, CDialog)
   //{{AFX_MSG_MAP(LicenseNumber)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LicenseNumber message handlers
void LicenseNumber::OnOK() 
{
   CDialog::OnOK();

   CpyStr(m_code, m_AccessCode, 10);
}

/////////////////////////////////////////////////////////////////////////////
// PasswordDlg dialog
PasswordDlg::PasswordDlg(CWnd* pParent /*=NULL*/)
   : CDialog(PasswordDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(PasswordDlg)
   m_password = _T("");
   //}}AFX_DATA_INIT
}

void PasswordDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PasswordDlg)
   DDX_Text(pDX, IDC_PASSWORD, m_password);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PasswordDlg, CDialog)
   //{{AFX_MSG_MAP(PasswordDlg)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// PasswordDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// ChangePassword dialog
ChangePassword::ChangePassword(CWnd* pParent /*=NULL*/)
   : CDialog(ChangePassword::IDD, pParent)
{
   //{{AFX_DATA_INIT(ChangePassword)
   m_new1 = _T("");
   m_new2 = _T("");
   m_old = _T("");
   //}}AFX_DATA_INIT
}

void ChangePassword::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ChangePassword)
   DDX_Text(pDX, IDC_NEW1, m_new1);
   DDX_Text(pDX, IDC_NEW2, m_new2);
   DDX_Text(pDX, IDC_OLD, m_old);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ChangePassword, CDialog)
   //{{AFX_MSG_MAP(ChangePassword)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChangePassword message handlers
void ChangePassword::OnOK() 
{
   UpdateData();

   if (!password.IsEmpty())   // password not set
   {
      if (password.CompareNoCase(m_old))
      {
         ErrorMessage("Incorrect old password");
         m_old.Empty();
         UpdateData(FALSE);
         return;
      }
   }

   if (m_new1.CompareNoCase(m_new2))
   {
      ErrorMessage("Mistyped new password");
      m_new1.Empty();
      m_new2.Empty();
      UpdateData(FALSE);
      return;
   }

   AfxGetApp()->WriteProfileString("Network", "Admin", m_new1);
   
   CDialog::OnOK();
}

BOOL ChangePassword::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   password = AfxGetApp()->GetProfileString("Network", "Admin");
   if (password.IsEmpty())
      SendDlgItemMessage(IDC_OLD, WM_ENABLE, 0);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}