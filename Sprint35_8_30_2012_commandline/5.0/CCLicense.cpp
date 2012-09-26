/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// CAMCADLICENSE.CPP

#include "stdafx.h"
#include "CAMCAD.h"
#include "splash.h"
#include "RwUiLib.h"
#include "Api.h"
#include "mgls_client.h"
#include "camcadlicense.h"
#include "MainFrm.h"
#include "Gauge.h"
#include "ProfileLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern unsigned long apiSerialNum; // API.CPP

static CCamcadTimer LicenseTimer;
CCamcadTimer &GetLicenseTimer()  { return LicenseTimer; }

#define NO_CPRINT_MSG   "Other CAMCAD modules will not be checked out without this primary license."

#define LICENSE_CHECKOUT_LOGFILENAME  "CamcadLicenseCheckOut.log"
#define LICENSE_CHECKIN_LOGFILENAME   "CamcadLicenseCheckIn.log"



CString getCamcadProductDisplayString(const CamcadProductIdTag camcadProductId)
{
   return "test";/*CString displayName;

   if (Product & PRODUCT_PROFESSIONAL)
      displayName = camcadProfessionalProductIdToDisplayString(camcadProductId);
   else if (Product & PRODUCT_PCB_TRANSLATOR)
      displayName = camcadPcbTranslatorProductIdToDisplayString(camcadProductId);
   else if (Product & PRODUCT_VISION) 
      displayName = camcadVisionProductIdToDisplayString(camcadProductId);
   else if (Product & PRODUCT_GRAPHIC)
      displayName = camcadGrapicProductIdToDisplayString(camcadProductId);

   return displayName;*/
}


CString getMentorProductDisplayString(const MentorProductIdTag mentorProductId)
{
   return "test";/*CString displayName;

   if (Product & PRODUCT_PROFESSIONAL)
      displayName = mentorProfessionalProductIdToDisplayString(mentorProductId);
   else if (Product & PRODUCT_PCB_TRANSLATOR)
      displayName = mentorPcbTranslatorProductIdToDisplayString(mentorProductId);
   else if (Product & PRODUCT_VISION) 
      displayName = mentorVisionProductIdToDisplayString(mentorProductId);
   else if (Product & PRODUCT_GRAPHIC)
      displayName = mentorGrapicProductIdToDisplayString(mentorProductId);

   return displayName;*/
}


//-----------------------------------------------------------------------------
// MentorProductLicense
//-----------------------------------------------------------------------------
MentorProductLicense::MentorProductLicense(const MentorProductIdTag mentorProductId)
{
   m_mentorProductId = mentorProductId;
   m_transactionId = 0;
   m_isLicenseGranted = false;
   m_isQueued = false;
}

MentorProductLicense::~MentorProductLicense()
{
}

CString MentorProductLicense::getMentorProductName()const
{
   return getMentorProductDisplayString(m_mentorProductId);
}

//-----------------------------------------------------------------------------
// CamcadProduct
//-----------------------------------------------------------------------------
CamcadProduct::CamcadProduct(const CamcadProductIdTag camcadProductId, const MentorProductIdTag mentorProductId, const FileTypeTag camcadFileType, const long accessTypeFlag, const long productTypeFlag)
{
   m_camcadProductId = camcadProductId;
   m_mentorProductId = mentorProductId;
   m_camcadFileType = camcadFileType;
   m_accessTypeFlag = accessTypeFlag;
   m_productTypeFlag = productTypeFlag;
   m_accessCode = 0;
   m_isAPI = false;

   m_originalAccessCode = 0;
   m_licenseName.Empty();
   m_allowed = false;
   m_isQueued = false;
}
//Leave For Now
CamcadProduct::CamcadProduct(const CamcadProductIdTag camcadProductId, const CString licenseName, const FileTypeTag camcadFileType, const long accessTypeFlag, const long productTypeFlag)
{
   m_camcadProductId = camcadProductId;
   m_mentorProductId = mentorProductCpUnknown;
   m_camcadFileType = camcadFileType;
   m_accessTypeFlag = accessTypeFlag;
   m_productTypeFlag = productTypeFlag;
   m_accessCode = 0;
   m_isAPI = false;

   m_licenseName = licenseName;
   m_originalAccessCode = 0;
   m_allowed = false;
}
//Leave For Now
CamcadProduct::~CamcadProduct()
{
}

//Leave For Now
CString CamcadProduct::getCamcadDisplayName()const
{
   return getCamcadProductDisplayString(m_camcadProductId);
}

//-----------------------------------------------------------------------------
// CamcadLicense
//-----------------------------------------------------------------------------
//Leave For Now
CamcadLicense::CamcadLicense()
   : m_mentorProductLicenseMap(nextPrime2n(20), true)
{
   // This date is use by the Mentor License to do Exact Access licensing.
   // Change to this date will affect already licensed users; therefore, this change
   // must be authorized from Development Engineering Manager (Moustafa).
   // The format is always "Mmm dd yyyy".
   m_camcadLicenseExactAccessDate = "Jun 01 2012";
   m_hasBaseLicense = false;
}
//Leave For Now
CamcadLicense::~CamcadLicense()
{
   checkInAllLicenses();
   m_camcadProductMap.empty();
   m_availableProductMap.RemoveAll();
}

void CamcadLicense::initializeLicenses()
{
   LicenseTimer.AddMessage("Enter initializeLicenses()");

   if (!m_initializedLicenses)
   {
      // Set license to not display error message automatically
      // We will trap and display the error message where desired
      LicenseTimer.AddMessage("core_Mheap_set MGLS_ATTR_APP_TYPE");
      if (core_Mheap_set(MGLS_ATTR_APP_TYPE, MGC_LIC_APP_NO_ERR_MSG) == MGLS_OK)
      {
         // Set attribute to queue requested license that is no available asynchronously
         LicenseTimer.AddMessage("core_Mheap_set MGLS_ATTR_MULTI_QUEUE");
         if (core_Mheap_set(MGLS_ATTR_MULTI_QUEUE, 1) == MGLS_OK)
         {
            LicenseTimer.AddMessage("core_Mheap_set MGLS_ATTR_QUEUE, MGLS_QUEUE_DEFAULT");
            if (core_Mheap_set(MGLS_ATTR_QUEUE, MGLS_QUEUE_DEFAULT) == MGLS_OK)
            {
               // Might want to set this attribute so we can check for license that are in use by calling core_Mheap_valid()
               LicenseTimer.AddMessage("core_Mheap_set MGLS_ATTR_LICENSE_INUSE");
               core_Mheap_set(MGLS_ATTR_LICENSE_INUSE, 1);
               m_initializedLicenses = true;
            }
         }
      }
      LicenseTimer.AddMessage("4 call core_Mheap_set sequence complete");
   }
   LicenseTimer.AddMessage("Exit initializeLIcenses()");
}


//Remove
void CamcadLicense::addCamcadProduct(const CamcadProductIdTag camcadProductId, const MentorProductIdTag mentorProductId, const FileTypeTag camcadFileType, const long accessTypeFlag, const long productTypeFlag)
{
   /*CamcadProduct* camcadProduct = findCamcadProduct(camcadProductId);
   if (camcadProduct == NULL)
   {
      MentorProductLicense* mentorProductLicense = findMentorProductLicense(mentorProductId);
      if (mentorProductLicense == NULL)
      {
         mentorProductLicense = new MentorProductLicense(mentorProductId);
         m_mentorProductLicenseMap.SetAt((int)mentorProductId, mentorProductLicense);
      }

      camcadProduct = new CamcadProduct(camcadProductId, mentorProductId, camcadFileType, accessTypeFlag, productTypeFlag);
      if (accessTypeFlag & ACCESS_DEFAULT && productTypeFlag & Product)
         camcadProduct->setAllowed(true);

      m_camcadProductMap.SetAt((int)camcadProductId, camcadProduct);
   }*/
}

//remove
MentorProductLicense* CamcadLicense::findMentorProductLicense(const MentorProductIdTag mentorProductId)
{
   /*MentorProductLicense* mentorProductLicense = NULL;
   m_mentorProductLicenseMap.Lookup(mentorProductId, mentorProductLicense);
   return mentorProductLicense;*/
   return NULL;
}

void CamcadLicense::loadAvailableProductMap()
{
}

void CamcadLicense::saveAvailableProductMap()
{
}

bool CamcadLicense::checkOutLicenses()
{
   if (m_checkOutLicenses)
      return true;


#ifdef LICENSE_SERVER
   // Check Out Licenses from License Server

   // if licenses checked out successfully, return
   if (FALSE) 
   {
      setSpecialLicenses();
      return;
   }
#endif

#ifndef SHAREWARE // do not open or read or process license file

   m_checkOutLicenses = checkOutLicenses(false);
   return m_checkOutLicenses;

#endif // SHAREWARE 
}

bool CamcadLicense::checkAvialable()
{
   // This function will first check to see if base license is still licensed.
   // If it is then return true, else ask if users want to rescan to acquire license.

   bool retval = false;

#ifndef LicenseBypass

   MentorProductLicense* mentorProductLicense = findMentorProductLicense(m_baseLicenseProductId);
   if (mentorProductLicense != NULL && (mentorProductLicense->getLicenseGranted() || mentorProductLicense->isQueued()))
   {
      long status = core_Mheap_valid(mentorProductLicense->getMentorProductId());
      while (status == MGLS_ERR)
      {
         checkInAllLicenses();
         char* err_info = (char*)malloc(SIZE_OF_MGLS_ERROR_STRING);
         core_Mheap_errs(&err_info);

         CString errMessage;
         errMessage.Format("License Error:\n\n");
         errMessage.AppendFormat("   No license is available.  Would you like to rescan for license?\n");
         errMessage.AppendFormat("   YES - Will attempt to acquire license\n");
         errMessage.AppendFormat("   No  - Will exit Camcad");

         int response = formatMessageBoxApp(MB_ICONERROR | MB_YESNO, errMessage);
         if (response == IDYES)
         {
            // Recan for license
            CProgressDlg* progress = new CProgressDlg("Camcad Licensing", false);
            progress->Create(AfxGetMainWnd());
            progress->SetStatus("Rescaning for license.  Please wait....");
            progress->ShowWindow(SW_SHOW);

            retval = checkOutLicenses(true);
            if (retval)
               status = MGLS_OK;

            progress->ShowWindow(SW_HIDE);
            if (progress->DestroyWindow())
            {
               delete progress;
               progress = NULL;
            }
         }
         else
         {
            // Close Camcad
            //getMainFrame()->enableExitWithoutSave(true);
            //getApp().OnAppExit();

            // Set Camcad to Unlicensed Mode
            getApp().SetCAMCADTitle();
            break;
         }
      }
   
      if (status == MGLS_LICENSE_AVAILABLE)
      {
         // License is available
         retval = true;
      }
      else
      {
         retval = false;
      }
   }
   
#else

   retval = true;

#endif

   return retval;
}

bool CamcadLicense::checkOutLicenses(const bool rescan)
{
     CString errorMessage;
	long licenseRetval = MGLS_ERR;
	MentorProductLicense* mentorBaseProductLicense = new MentorProductLicense(mentorProductCpCamcadCmdLineImportExportCPCMD);//findMentorProductLicense(mentorProductCpCamcadCmdLineImportExportCPCMD);//mentorProductCCEtoODBexporter);

	if(mentorBaseProductLicense == NULL)
		return false;

	licenseRetval = checkOutLicense(*mentorBaseProductLicense, errorMessage, rescan);
	

	if (mentorBaseProductLicense != NULL)
	{
		if (licenseRetval == MGLS_OK)
		{
			 // We have command line feature license, enable CC Pro to in turn enable Importer access
             // Lie, say CC Pro license was okay
             licenseRetval = MGLS_OK; 
             // Enable CC Pro in order to get at Importer licenses it owns
             mentorBaseProductLicense->setLicenseGranted(true); 
             // Shift base license product ID over to CPCMD
			 m_hasBaseLicense = true;
			 //setCamcadProductAllowed();
			 return true;
		}
	}
	return false;
}

//Keep
long CamcadLicense::checkOutLicense(MentorProductLicense& mentorProductLicense, CString& errorMessage, const bool rescan)
{
   LicenseTimer.AddMessage("Enter checkOutLicenses[2]");

   long status = MGLS_OK;
   char* err_info = (char*)malloc(SIZE_OF_MGLS_ERROR_STRING);
   errorMessage.Empty();

   int lickey = (int)mentorProductLicense.getMentorProductId();

   if (!mentorProductLicense.isQueued() && !mentorProductLicense.getLicenseGranted())
   {  
      LicenseTimer.AddMessage("CALL core_Mheap_allocv", mentorProductLicense.getMentorProductName());
      // "status" returned here is transaction ID if > 0, Is -1 aka MGLS_ERR if failed
      status = core_Mheap_allocv((int)mentorProductLicense.getMentorProductId(), m_camcadLicenseExactAccessDate.GetBuffer(0));
      LicenseTimer.AddMessage("RETURNED core_Mheap_allocv", mentorProductLicense.getMentorProductName());
      if (status == MGLS_ERR)
      {
         core_Mheap_errs(&err_info);
         errorMessage = err_info;
      }
      else
      {
         mentorProductLicense.setTranactionId(status);
      }
   }

   if (status != MGLS_ERR)
   {
      LicenseTimer.AddMessage("CALL core_Mheap_stat");
      status = core_Mheap_stat(mentorProductLicense.getTranactionId());
      LicenseTimer.AddMessage("RETURNED core_Mheap_stat");
      if (status == MGLS_ERR)
      {
         core_Mheap_errs(&err_info);
         errorMessage = err_info;
      }
      else if (status == MGLS_OK)
      {
         mentorProductLicense.setLicenseGranted(true);
         mentorProductLicense.setQueued(false);
      }
      else if (status == MGLS_REQ_IN_QUEUE)
      {
         mentorProductLicense.setQueued(true);
      }
   }

   errorMessage.Replace("null", getMentorProductDisplayString(mentorProductLicense.getMentorProductId()));

   LicenseTimer.AddMessage("Exit checkOutLicenses[2]");

   return status;
}

//Remove
void CamcadLicense::setCamcadProductAllowed()
{
   for (POSITION pos=getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = getNextProduct(pos);
      /*if (camcadProduct != NULL)
      {

#ifndef LicenseBypass

         MentorProductLicense* mentorLicense = findMentorProductLicense(camcadProduct->getMentorProductProId());
         if (mentorLicense != NULL)
         {
            if (mentorLicense->getLicenseGranted())
            {
               camcadProduct->setAllowed(true);
               camcadProduct->setQueued(false);
            }
            else if (mentorLicense->isQueued())
            {
               camcadProduct->setQueued(true);
            }
            else
            {
               camcadProduct->setAllowed(true);
               camcadProduct->setQueued(false);
            }
         }

#else*/
        camcadProduct->setAllowed(true);
      
//#endif
      //}
   }
}

//Debug to see if we get into here
void CamcadLicense::checkInAllLicenses()
{
   m_hasBaseLicense = false;

   if (!m_initializedLicenses)
      return;

#ifndef LicenseBypass

   CStdioFileWriteFormat logFile;
   bool hasError = false;
   // License log is using TEMP dir only, not standard logfilepath stuff.
   CString logFilePath( GetTempFilePath(LICENSE_CHECKIN_LOGFILENAME) );
   bool hasLogFile = logFilePath.IsEmpty() ? false : logFile.open(logFilePath);

   if (hasLogFile)
   {
      logFile.writef("Checking in licenses for Camcad Product...");
   }

   for (POSITION pos=m_mentorProductLicenseMap.GetStartPosition(); pos != NULL;)
   {
      int mentorProductId;
      MentorProductLicense* mentorProductLicense = NULL;
      m_mentorProductLicenseMap.GetNextAssoc(pos, mentorProductId, mentorProductLicense);
      if (mentorProductLicense != NULL && mentorProductLicense->getTranactionId() > 0)
      {
         CString errorMessage;
         long retval = checkInLicense(mentorProductLicense->getTranactionId(), errorMessage);
         if (retval != MGLS_OK && hasLogFile)
         {
            if (!hasError)
            {
               // Then this is the first error, print the error message header line.
               logFile.writef("\n\nThe following feature(s) failed to check-in:\n");
            }

            logFile.writef("Error# %d: %s\n", retval, errorMessage);
            hasError = true;
         }

         mentorProductLicense->setTranactionId(0);
         mentorProductLicense->setLicenseGranted(false);
         mentorProductLicense->setQueued(false);
      }
   }

   char* err_info = (char*)malloc(SIZE_OF_MGLS_ERROR_STRING);
   long status = core_Mheap_clean();
   if (status == MGLS_ERR)
   {
      core_Mheap_errs(&err_info);
      if (hasLogFile)
      {
         logFile.writef("Error# %d: %s\n", status, err_info);
         hasError = true;
      }
   }

   logFile.write("License check-in done.\n");

   logFile.close();
   setCamcadProductAllowed();
   setSpecialLicenses();
   saveAvailableProductMap();

   if (hasError  && hasLogFile)
   {
      Logreader(logFilePath);
   }

#endif
}

long CamcadLicense::checkInLicense(const long transactionId, CString &errorMessage)
{
   errorMessage.Empty();
   long status = MGLS_OK;
   char* err_info = (char*)malloc(SIZE_OF_MGLS_ERROR_STRING);

   status = core_Mheap_free(transactionId);
   if (status == MGLS_ERR)
   {
      core_Mheap_errs(&err_info);
      errorMessage = err_info;

      int applicationId = 0;
      status = core_Mheap_which(transactionId, &applicationId);
      if (status != MGLS_ERR)
      {
         errorMessage.Replace("null", getMentorProductDisplayString((MentorProductIdTag)applicationId));
      }
   }

   return status;
}

bool CamcadLicense::isLicensed(CamcadProductIdTag camcadProductId)
{
   bool retval = false;
   CamcadProduct* camcadProduct = findCamcadProduct(camcadProductId);
   if (camcadProduct != NULL)
   {

#if defined ( LicenseBypass )
      retval = true;

#elif defined ( SHAREWARE ) 
      retval = (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT);

#else
      if (camcadProduct->isAPI() && camcadProduct->getAllowed())
      {
         if (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT || 
               (camcadProduct->getAccessCode() > 0 && camcadProduct->getAccessCode() == (0xffffffff ^ camcadProduct->getOriginalAccessCode()) &&
                camcadProduct->getAccessCode() == crypt(apiSerialNum, (int)camcadProductId)
               )
            )
         {
            retval = true;
         }
      }
      else if (camcadProduct->getAllowed() && checkAvialable())
      {
         retval = true;
      }
#endif

   }
   
   return retval;
}

CamcadProduct *CamcadLicenseBase::getImporterProduct(FileTypeTag filetype)
{
   for(POSITION pos=getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         if ( (camcadProduct->getAccessTypeFlag() & ACCESS_INPUT) &&
             (camcadProduct->getCamcadFileType() == filetype)   && 
             (camcadProduct->getProductTypeFlag() & Product) )
         {
            return camcadProduct;
         }
      }
   }

   return NULL;
}

CamcadProduct *CamcadLicenseBase::getExporterProduct(FileTypeTag filetype)
{
   for(POSITION pos=getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         if ( (camcadProduct->getAccessTypeFlag() & ACCESS_OUTPUT) &&
             (camcadProduct->getCamcadFileType() == filetype)   && 
             (camcadProduct->getProductTypeFlag() & Product) )
         {
            return camcadProduct;
         }
      }
   }

   return NULL;
}


int CamcadLicense::isFormatLicensed(const int fileFormat, const bool import)
{
   int retval = RC_ITEM_NOT_FOUND;

   for(POSITION pos=getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         if (((import && camcadProduct->getAccessTypeFlag() & ACCESS_INPUT) || (!import && camcadProduct->getAccessTypeFlag() & ACCESS_OUTPUT)) &&
             (int)camcadProduct->getCamcadFileType() == fileFormat && camcadProduct->getProductTypeFlag() & Product)
         {
            if (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT)
            {
               retval = RC_SUCCESS;
            }
            else if (isLicensed(camcadProduct->getCamcadProductId()))
            {
               retval = RC_SUCCESS;
            }
            else
            {
               retval = RC_NO_LICENSE;
            }
            break;
         }
      }
   }

   return retval;
}

//-----------------------------------------------------------------------------
// CamcadLicenseBase
//-----------------------------------------------------------------------------
CamcadLicenseBase::CamcadLicenseBase()
   : m_camcadProductMap(nextPrime2n(20), true)
{
   m_camcadLicenseKey =    "utreweowityayp";   // must be all lower char VERSION 3.x.x
   m_licensedApiLevel = 0;
   m_initializedLicenses = false;
   m_checkOutLicenses = false;
   m_hasBaseLicense = false;

   m_expirationDate.minute = 0;
   m_expirationDate.hour = 0;
   m_expirationDate.day = 0;
   m_expirationDate.month = 0;
   m_expirationDate.year = 0;

   m_useDongle = false;
   m_hasExpiration = false;
}

CamcadLicenseBase::~CamcadLicenseBase()
{ 
   m_camcadProductMap.empty();
}

void CamcadLicenseBase::resetAllLicense()
{
   for (POSITION pos=m_camcadProductMap.GetStartPosition(); pos != NULL;)
   {
      int camcadProductId;
      CamcadProduct* camcadProduct = NULL;
      m_camcadProductMap.GetNextAssoc(pos, camcadProductId, camcadProduct);

      if (camcadProduct != NULL)
      {
         if (camcadProduct->getProductTypeFlag() & ACCESS_DEFAULT && camcadProduct->getProductTypeFlag() & Product)
         {
            camcadProduct->setAllowed(true);
         }
         else
         {
            camcadProduct->setAccessCode(0);
            camcadProduct->setAllowed(false);
         }
      }
   }
}

void CamcadLicenseBase::initializeLicenses()
{
}


void CamcadLicenseBase::addCamcadProduct(const CamcadProductIdTag camcadProductId, const FileTypeTag camcadFileType, const char accessTypeFlag, const long productTypeFlag)
{
   CamcadProduct* camcadProduct = findCamcadProduct(camcadProductId);
   if (camcadProduct == NULL)
   {
      CString licenseName = makeFlexLmLicenseName(camcadProductId);
      camcadProduct = new CamcadProduct(camcadProductId, licenseName, camcadFileType, accessTypeFlag, productTypeFlag);
      if (accessTypeFlag & ACCESS_DEFAULT && productTypeFlag & Product)
         camcadProduct->setAllowed(true);

      m_camcadProductMap.SetAt((int)camcadProductId, camcadProduct);
   }
}

CamcadProduct* CamcadLicenseBase::findCamcadProduct(const CamcadProductIdTag camcadProductId)
{
   CamcadProduct* camcadProduct = NULL;
   m_camcadProductMap.Lookup(camcadProductId, camcadProduct);
   return camcadProduct;
}

bool CamcadLicenseBase::checkOutLicenses()
{
   if (m_checkOutLicenses)
      return true;

#ifdef LICENSE_SERVER
   // Check Out Licenses from License Server

   // if licenses checked out successfully, return
   if (FALSE) 
   {
      setSpecialLicenses();
      return;
   }
#endif

#ifndef SHAREWARE // do not open or read or process license file

   m_checkOutLicenses = checkOutLicenses(false);
   return m_checkOutLicenses;

#endif // SHAREWARE 
}

bool CamcadLicenseBase::checkOutLicenses(const bool rescan)
{
#ifdef OLD_RSI_FLEXLM

   BOOL keepTrying = TRUE;
   CString availableLicFilename;

   if (!rescan)
   {
      if (Product == PRODUCT_PROFESSIONAL)
         availableLicFilename = "proLicData.dat";
      else
      if (Product == PRODUCT_GRAPHIC)
         availableLicFilename = "graLicData.dat";
      else
      if (Product == PRODUCT_PCB_TRANSLATOR)
         availableLicFilename = "pcbLicData.dat";
      else
      if (Product == PRODUCT_VISION)
         availableLicFilename = "visLicData.dat";
   }

   //if (getApp().flexlm.InitFLEXlm(getApp().getCamcadExeFolderPath(), availableLicFilename, OnExitCall) != RET_NO_ERROR)
   {
      if (!rescan) CSplashWnd::SetClosable();

      AfxMessageBox(getApp().flexlm.GetErrorString());
      return false;
   }

   if (!rescan)
   {
      getApp().flexlm.SetMaintenanceModuleName(getLicensePrefix() + "Maintenance");
   }
   else
   { 
      getApp().flexlm.ResetLicenseList();
   }
   getApp().flexlm.LogBegin(GetLogfilePath("lic.log"));

   for (POSITION pos=m_camcadProductMap.GetStartPosition(); pos != NULL;)
   {
      int camcadProductId;
      CamcadProduct* camcadProduct = NULL;
      m_camcadProductMap.GetNextAssoc(pos, camcadProductId, camcadProduct);

      if (camcadProduct != NULL)
      {
         // make sure this license can be licensed for the current product
         if (camcadProduct->getProductTypeFlag() & Product)
         {
            int retVal = getApp().flexlm.CheckOut(camcadProduct->getCamcadLicenseName(), LIC_VERSION);
            switch (retVal)
            {
            case 0:  // checked out
               {
                  camcadProduct->setAllowed(true);

                  // get the current license's user information
                  CString dongle;
                  getApp().flexlm.GetUserData(camcadProduct->getCamcadLicenseName(), FEATURE_SERIALNUMBER, m_serialNumber);

                  // are we using a dongle [NOT WORKING CORRECTLY]
                  getApp().flexlm.GetUserData(camcadProduct->getCamcadLicenseName(), FEATURE_USINGDONGLE, dongle);
                  m_useDongle = (dongle == "TRUE");               
               
                  getApp().flexlm.GetUserData(camcadProduct->getCamcadLicenseName(), FEATURE_HOSTID, m_hostId);
               }
               break;
            case LM_BADCODE:              // invalid signature
            case LM_MAXLIMIT:             // too many users from options file
            case LM_MAXUSERS:             // too many users from license
            case LM_NOFEATURE:            // no such feature
            case LM_NOSERVSUPP:
            case LM_CANTCONNECT:          // can't connect to server
               {
                  if ((CamcadProductIdTag)camcadProductId == camcadProductPrint)
                  {
                     if (!rescan) CSplashWnd::SetClosable();

                     CString msg = getApp().flexlm.GetErrorString() + "\n\n" + NO_CPRINT_MSG;
                     AfxMessageBox(msg);
                     keepTrying = FALSE;
                  }
               }
               break;
            case LM_NO_SERVER_IN_FILE:    // can't find server
            case LM_BADSYSDATE:           // machine's clock was set back to prolong license use
            case LM_BADHOST:              // Conflicting hosts
               {
                  AfxMessageBox(getApp().flexlm.GetErrorString());
                  keepTrying = FALSE;
               }
               break;
            default:
               {
                  if ((CamcadProductIdTag)camcadProductId == camcadProductPrint)
                  {
                     CString msg = getApp().flexlm.GetErrorString() + "\n\n" + NO_CPRINT_MSG;

                     if (!rescan)
                     {
                        CSplashWnd::SetClosable();
                        #ifndef LicenseBypass
                           AfxMessageBox(msg);
                        #endif
                     }
                     else
                     {
                        AfxMessageBox(msg);
                     }

                     keepTrying = FALSE;
                  }
               }
            }
         }
      }

      if (!keepTrying)
         break;
   }
   getApp().flexlm.LogEnd();

   // check for the expiration date
   CString line;
   char sTemp[32], sMonth[3], *curP;
   m_hasExpiration = false;
   CString moduleName = makeFlexLmLicenseName(camcadProductPrint);
   getApp().flexlm.GetUserData(moduleName, FEATURE_EXPIRATIONDATE, line);
   strcpy(sTemp, line);
   if (curP = strtok(sTemp, "-"))
   {
      m_expirationDate.day = atoi(curP);
      if (m_expirationDate.day < 0 || m_expirationDate.day > 31)
         m_expirationDate.day = 0;

      curP = strtok(NULL, "-");
      strcpy(sMonth, curP);
      m_expirationDate.month = GetMonth(sMonth);
      if (m_expirationDate.month < 0 || m_expirationDate.month > 12)
         m_expirationDate.month = 0;

      curP = strtok(NULL, "-");
      m_expirationDate.year = atoi(curP);
      if (m_expirationDate.year < 0)
         m_expirationDate.year = 0;

      m_hasExpiration = (m_expirationDate.year > 0);
   }

   //make sure we set the application to not have temporary licenses
   if (!m_hasExpiration)
      m_expirationDate.month = 0;

   //After all licenses have been checked out, if any, get the current user and company name
   getApp().flexlm.GetUserData(LIC_MAIN_DATA, FEATURE_COMPANY, m_company);
   getApp().flexlm.GetUserData(LIC_MAIN_DATA, FEATURE_USERNAME, m_user);

   setSpecialLicenses();

#endif

   return true;   
}

void CamcadLicenseBase::activateCamcadProduct(const CamcadProductIdTag ccProductId)
{
   CamcadProduct* camcadProduct = findCamcadProduct(ccProductId);
   if (camcadProduct != NULL)
   {
      camcadProduct->setAllowed(true);
   }
}

void CamcadLicenseBase::activateBaseProductLicenses()
{
   if (isLicensed(camcadProductCpCamcadCPPROBASE) ||
      isLicensed(camcadProductPrint) || // Older license base product
      isLicensed(camcadProductPEdit))   // Another older base product license
   {
      // CAMCAD "Products" that have no individual Mentor key/part# association

      activateCamcadProduct(camcadProductPrint);
      activateCamcadProduct(camcadProductApiLevel1);
      activateCamcadProduct(camcadProductApiLevel2);
      activateCamcadProduct(camcadProductApiLevel3);
      activateCamcadProduct(camcadProductApiLevel4);     
      activateCamcadProduct(camcadProductRedline);
      activateCamcadProduct(camcadProductStencilStepper);

      activateCamcadProduct(camcadProductCommandLineCPCMD);

      activateCamcadProduct(camcadProductApertureRead);
      activateCamcadProduct(camcadProductAttributeBomDelimitedRead);
      activateCamcadProduct(camcadProductAttributeBomRead);
      activateCamcadProduct(camcadProductAutocadAdiRead);
      activateCamcadProduct(camcadProductBarcoDpfRead);
      activateCamcadProduct(camcadProductCadenceAllegroIpfRead);
      activateCamcadProduct(camcadProductCasePlotRead);
      activateCamcadProduct(camcadProductDxfRead);
      activateCamcadProduct(camcadProductExcellonDrillRouteRead);
      activateCamcadProduct(camcadProductFabmasterDeviceRead);
      activateCamcadProduct(camcadProductGerberRead);
      activateCamcadProduct(camcadProductHpEgsRead);
      activateCamcadProduct(camcadProductHpglRead);
      activateCamcadProduct(camcadProductIgesRead);
      activateCamcadProduct(camcadProductSchematicNetlistRead);

      activateCamcadProduct(camcadProductAttributeBomWrite);


      // CAMCAD Products that also have individual Mentor key/part# associations

      activateCamcadProduct(camcadProductRealPartPackage);      //cprealpartpkg, dts0100558670 added this to base product

      activateCamcadProduct(camcadProductBoardStationRead);     // cprdmenbst
      activateCamcadProduct(camcadProductMentorNeutralRead);    // cprdmenneu
      activateCamcadProduct(camcadProductPadsRead);             // cprdmenpad
      activateCamcadProduct(camcadProductVeribestEifRead);      // cprdmeneif
      activateCamcadProduct(camcadProductSupermaxDdeRead);      // cprdmendde
      activateCamcadProduct(camcadProductExpeditionAsciiRead);  // cprdmenexp
      activateCamcadProduct(camcadProductGenericCentroidRead);  // cprdgencen
      activateCamcadProduct(camcadProductOdbPlusPlusRead);      // cprdvalodb

      activateCamcadProduct(camcadProductApertureWrite);        // cpwrgenapr
      activateCamcadProduct(camcadProductDxfWrite);             // cpwrgendxf
      activateCamcadProduct(camcadProductExcellonDrillWrite);   // cpwrexcdrl, dts0100574881 added this to base product
      activateCamcadProduct(camcadProductGerberWrite);          // cpwrgengbr
      activateCamcadProduct(camcadProductHpglWrite);            // cpwrgenhgl
      activateCamcadProduct(camcadProductBarcoPdfWrite);        // cpwrbardpf   (note it is actually DPF not PDF)
      activateCamcadProduct(camcadProductIgesWrite);            // cpwriges
      activateCamcadProduct(camcadProductOdbPlusPlusWrite);     // cpwrvalodb
   }
}

void CamcadLicenseBase::setSpecialLicenses()
{
   m_licensedApiLevel = 0;
   if (isLicensed(camcadProductApiLevel1))
      m_licensedApiLevel = 1;
   if (isLicensed(camcadProductApiLevel2))
      m_licensedApiLevel = 2;
   if (isLicensed(camcadProductApiLevel3))
      m_licensedApiLevel = 3;
   if (isLicensed(camcadProductApiLevel4))
      m_licensedApiLevel = 4;

   activateBaseProductLicenses();  // Must be first, before looking at rest of "special licenses"

	if (isLicensed(camcadProductDft))
   {
      activateCamcadProduct(camcadProductFixtureWrite);
   }

	if (isLicensed(camcadProductTeradyne228xCktWrite))
   {
      activateCamcadProduct(camcadProductNailAssignmentRead);
      activateCamcadProduct(camcadProductTeradyne228xNavWrite);
   }

	if (isLicensed(camcadProductAgilentAoiWrite))
   {
      activateCamcadProduct(camcadProductAgilentSjPlxRead);
   }

	if (isLicensed(camcadProductPrint))
   {
      CamcadProductIdTag camcadCPrintProductTag[] =
      {
         camcadProductAttributeBomDelimitedRead,
         camcadProductApertureRead,
         camcadProductDxfRead,
         camcadProductGerberRead,
         camcadProductHpglRead
      };

      // This may look redundant, but it isn't.
      // When licensing is done using flex lm licensing, the "add product" calls are associating these
      // with base license. But that does not work for "OEM License" scheme as used by API.
      // See setOEMLicenseByApi().
      // So these are here to support OEM License by api, to associate these with CPRINT license.
      int sizeofCprintTag = sizeof(camcadCPrintProductTag)/sizeof(CamcadProductIdTag);
      for(int idx = 0; idx < sizeofCprintTag; idx++)
      {
         activateCamcadProduct(camcadCPrintProductTag[idx]);
         addCamcadProductAccessFlag(camcadCPrintProductTag[idx], ACCESS_DEFAULT);
      }
   }

   if (isLicensed(camcadProductRealPartPackage))
   {
      activateCamcadProduct(camcadProductRealPartDevice);
   }
}

//Need to check
CString CamcadLicenseBase::makeFlexLmLicenseName(const CamcadProductIdTag camcadProductId)
{
   CString licenseName = "Test";//camcadProductIdToDisplayString(camcadProductId);

   if (camcadProductId == camcadProductEdifRead)
   {
      // Because the display name of Edif is "EDIF V 200...300 Read" after we removed the Edif 400 Read.
      // But licensing used to use the string "EDIF V 200...400 Read", so when checking license we have to 
      // use this string
      licenseName = "EDIF V 200...400 Read";
   }

   CString buf = getLicensePrefix() + removePuncutation(licenseName);

	// make sure we are within the 30 character limit
   return buf.Left(30);
}

CString CamcadLicenseBase::removePuncutation(const CString sourceString)
{
   CString retStr = sourceString;

   // remove all invalid punctuation for FLEXlm
   retStr.Replace("(", "_");
   retStr.Replace(")", "_");

   retStr.Replace(",", "_");
   retStr.Replace(".", "_");
   retStr.Replace("&", "_");

   retStr.Replace(" ", "_");

   retStr.Replace("/", "_");
   retStr.Replace("+", "_");
   retStr.Replace("-", "_");
   retStr.Replace("*", "_");

   // remove all double underscores
   while (retStr.Find("__") >= 0)
      retStr.Replace("__", "_");

   return retStr;
}
bool CamcadLicenseBase::isLicensed(CamcadProductIdTag camcadProductId)
{
   bool retval = false;
   CamcadProduct* camcadProduct = findCamcadProduct(camcadProductId);
   if (camcadProduct != NULL)
   {

#if defined ( LicenseBypass )
      retval = true;

#elif defined ( SHAREWARE ) 
      retval = (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT);

#else
      if (camcadProduct->isAPI() && camcadProduct->getAllowed())
      {
         if (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT || 
               (camcadProduct->getAccessCode() > 0 && camcadProduct->getAccessCode() == (0xffffffff ^ camcadProduct->getOriginalAccessCode()) &&
                camcadProduct->getAccessCode() == crypt(apiSerialNum, (int)camcadProductId)
               )
            )
         {
            retval = true;
         }
      }
      else if (camcadProduct->getAllowed())
      {
         retval = true;
      }

#endif

   }
   
   return retval;
}

void CamcadLicenseBase::warnExpiration()
{
   if (!m_hasExpiration)
      return;

   if ( m_expirationDate.year <= 0 || m_expirationDate.month <= 0 || m_expirationDate.day <= 0)
      return;

   CTime expiration(m_expirationDate.year, m_expirationDate.month, m_expirationDate.day, 23, 59, 59);  

   CTime currentTime = CTime::GetCurrentTime();
   if (currentTime > expiration)
      return;

   CTimeSpan span = expiration - currentTime;
   if (span.GetDays() < 30)
   {
      CString buf;
      buf.Format("Temporary License expires in %d days.", span.GetDays());
      ErrorMessage(buf, "Warning");
   }
}

int CamcadLicenseBase::isFormatLicensed(const int fileFormat, const bool import)
{
   int retval = RC_ITEM_NOT_FOUND;
   for(POSITION pos=getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         if (((import && camcadProduct->getAccessTypeFlag() & ACCESS_INPUT) || (!import && camcadProduct->getAccessTypeFlag() & ACCESS_OUTPUT)) &&
             (int)camcadProduct->getCamcadFileType() == fileFormat && camcadProduct->getProductTypeFlag() & Product)
         {
            if (camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT || isLicensed(camcadProduct->getCamcadProductId()))
            {
               retval = RC_SUCCESS;
            }
            else
            {
               retval = RC_NO_LICENSE;
            }
            break;
         }
      }
   }

   return retval;
}

short CamcadLicenseBase::setLicenseByApi(const CamcadProductIdTag camcadProductId, const long accessCode)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   short retval = RC_SUCCESS;
   CCAMCADApp& camcadApp = getApp();

   for (POSITION pos = getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = getNextProduct(pos);
      if (camcadProduct != NULL && !camcadProduct->isAPI())
      {
         camcadProduct->setAllowed(false);
         camcadProduct->setAccessCode(0);
#ifdef OLD_RSI_FLEXLM
         camcadApp.flexlm.CheckIn(camcadProduct->getCamcadLicenseName());
#endif
      }
   }

   m_user = "API User";
   m_company.Empty();

   CString names = "API Internal 40";
   names += "Router Solutions";
   apiSerialNum = hash(names);

   CamcadProduct* camcadProduct = findCamcadProduct(camcadProductId);
   if (camcadProduct != NULL)
   {
      camcadProduct->setOriginalAccessCode(0xffffffff ^ accessCode);
      if (accessCode == crypt(apiSerialNum, (int)camcadProductId))
      {
         camcadProduct->setAPI(true);
         camcadProduct->setAccessCode(accessCode);
         if (camcadProduct->getProductTypeFlag() & Product)
         {
            camcadProduct->setAllowed(true);
         }
      }
      else
      {
         camcadProduct->setAllowed(false);
         camcadProduct->setAccessCode(0);
         retval = RC_GENERAL_ERROR;
      }
   }

   return retval;
}

short CamcadLicenseBase::setOEMLicenseByApi(const CString OEMName, const CamcadProductIdTag camcadProductId, const long accessCode, bool& lockLicense)
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif


   short retval = RC_SUCCESS;
   CCAMCADApp& camcadApp = getApp();

   if (!lockLicense)
   {
      for (POSITION pos = getProductStartPosition(); pos != NULL;)
      {
         CamcadProduct* camcadProduct = getNextProduct(pos);
         if (camcadProduct != NULL)
         {
            if (!camcadProduct->isAPI() && camcadProduct->getAccessTypeFlag() & ACCESS_DEFAULT && camcadProduct->getProductTypeFlag() & Product)
            {
               camcadProduct->setAllowed(true);
            }
            else
            {
               camcadProduct->setAllowed(false);
               camcadProduct->setAccessCode(0);
            }
         }
      }
   }

   lockLicense = true;

   m_user = "API User";
   m_company = OEMName;

   CString names = "API Internal 40";
   names += OEMName;
   apiSerialNum = hash(names);

   CamcadProduct* camcadProduct = findCamcadProduct(camcadProductId);
   if (camcadProduct != NULL)
   {
      camcadProduct->setOriginalAccessCode(0xffffffff ^ accessCode);
      if (accessCode == crypt(apiSerialNum, (int)camcadProductId))
      {
         camcadProduct->setAPI(true);
         camcadProduct->setAccessCode(accessCode);
         if (camcadProduct->getProductTypeFlag() & Product)
         {
            camcadProduct->setAllowed(true);
         }
      }
      else
      {
         camcadProduct->setAllowed(false);
         camcadProduct->setAccessCode(0);
         retval = RC_GENERAL_ERROR;
      }
   }

   // This handles turning on multiple additional products potentially associated with
   // license curerntly being activated.
   setSpecialLicenses();

   return retval;
}

CString CamcadLicenseBase::getExpirationDateString() const
{
   CString expirationDate;
   expirationDate.Format("%02d/%02d/%4d", m_expirationDate.month, m_expirationDate.day, m_expirationDate.year);

   return expirationDate;
}

void CamcadLicenseBase::addCamcadProductAccessFlag(const CamcadProductIdTag ccProductId, int accessFlag)
{
   CamcadProduct* camcadProduct = findCamcadProduct(ccProductId);
   if (camcadProduct != NULL)
   {
      camcadProduct->setAccessTypeFlag(camcadProduct->getAccessTypeFlag() | accessFlag);
   }
}


