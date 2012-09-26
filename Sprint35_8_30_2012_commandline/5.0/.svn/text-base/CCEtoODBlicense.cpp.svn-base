/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// CCETOODBLICENSE.CPP

#include "stdafx.h"
#include "CCEtoODB.h"
#include "splash.h"
#include "RwUiLib.h"
#include "Api.h"
#include "mgls_client.h"
#include "CCEtoODBlicense.h"
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



/*CString getCamcadProductDisplayString(const CamcadProductIdTag camcadProductId)
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

   return displayName;
}*/

/*CString getMentorProductDisplayString(const MentorProductIdTag mentorProductId)
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

   return displayName;
}*/


//-----------------------------------------------------------------------------
// MentorProductLicense
//-----------------------------------------------------------------------------

//Keep
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
   //checkInAllLicenses();
   //m_camcadProductMap.empty();
   m_availableProductMap.RemoveAll();
}
//Keep
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





//Keep
bool CamcadLicense::checkOutLicenses()
{
   if (m_checkOutLicenses)
      return true;


#ifdef LICENSE_SERVER
   // Check Out Licenses from License Server

   // if licenses checked out successfully, return
   if (FALSE) 
   {
      //setSpecialLicenses();
      return;
   }
#endif

#ifndef SHAREWARE // do not open or read or process license file

   m_checkOutLicenses = checkOutLicenses(false);
   return m_checkOutLicenses;

#endif // SHAREWARE 
}


//Keep
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
      //LicenseTimer.AddMessage("CALL core_Mheap_allocv", mentorProductLicense.getMentorProductName());
      // "status" returned here is transaction ID if > 0, Is -1 aka MGLS_ERR if failed
      status = core_Mheap_allocv((int)mentorProductLicense.getMentorProductId(), m_camcadLicenseExactAccessDate.GetBuffer(0));
      //LicenseTimer.AddMessage("RETURNED core_Mheap_allocv", mentorProductLicense.getMentorProductName());
      //status = 0; // Miten -- Hardcoded this value....Need to check back and see what went wrong here
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
	  //status = 0; // Miten -- Hardcoded this value....Need to check back and see what went wrong here
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

   //errorMessage.Replace("null", getMentorProductDisplayString(mentorProductLicense.getMentorProductId()));

   LicenseTimer.AddMessage("Exit checkOutLicenses[2]");

   return status;
}

CamcadLicenseBase::CamcadLicenseBase()
   
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
}
