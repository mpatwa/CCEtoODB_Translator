// $Header: /CAMCAD/4.6/FLEXlm.cpp 23    4/03/07 5:46p Lynn Phung $

#include "stdafx.h"
#include "flexlm.h"
#include "feature.h"

#define NO_LICMGR_S     (CString)"License management software has not been installed.  You must install \n" + \
                        "the license manager software prior to running any licensed Mentor Graphics application."
#define NOT_CONFIG_S    (CString)"License management software has not been configured correctly.  You must run\n" + \
                        "the license manager software to specify the license file used for Mentor Graphics products."
#define NOT_INIT_S      (CString)"License functionality has not been initialized."

typedef int FUNCPTR(const char*);

#ifdef OLD_RSI_FLEXLM

static FUNCPTR* gExitFuncPtr;
static FLEXlm *flexObj;

int OnFLEXlmExitCall(char *feature)
{
   flexObj->OnNoLicense(feature, RET_NO_LIC_EXIT_CALL);

   return RET_NO_ERROR;
}

int OnCheckOutFilter(CONFIG *conf)
{
   //parse VENDOR_STRING
   CString vStr = conf->lc_vendor_def, tmp, userName, companyName;
   tmp = vStr.Mid(vStr.Find("-")+1);


   userName = tmp.Mid(0, tmp.Find(VSTR_DELIM));
   companyName = tmp.Mid(tmp.Find(VSTR_DELIM)+1);
   
   //if this license is from the network, then ignore the company and username
   if (userName.Find("Network") < 0)
   {
      CString company, user;

      // get the current overall company name [feature = LIC_MAIN_DATA]
      flexObj->GetUserData(LIC_MAIN_DATA, FEATURE_COMPANY, company);
      flexObj->GetUserData(LIC_MAIN_DATA, FEATURE_USERNAME, user);

      // make sure the company and username match up
      if (!company.IsEmpty() && company != companyName) return -123;
      if (!user.IsEmpty() && user != userName) return -123;
   }

   return RET_NO_ERROR;
}



FLEXlm::FLEXlm(void)
{
   SHORT result = 0;

   m_bLicsAreGood = TRUE;
   m_lmJob = NULL;
   m_bIsInitialized = FALSE;

   //Start a new job used to check out the module
   result = lc_new_job(0, NULL, &m_code, &m_lmJob);
   if (result)
   {
      m_bLicsAreGood = FALSE;
      m_sErrMsg = lc_errstring(m_lmJob);
   }
}

FLEXlm::~FLEXlm(void)
{
   VOID *voidPtr = NULL;
   CString key;

   //check in any checked out licenses
   POSITION pos = m_jobMap.GetStartPosition();
   while (pos)
   {
      m_jobMap.GetNextAssoc(pos, key, voidPtr);
      CheckIn(key);
   }

   m_sErrMsg.Empty();

   // see if we need to save the license data
   if (!m_saLicsToCheck.IsEmpty() && m_bCreateLicList)
   {
      CString path = m_sCwd + m_sAvailLicFName;
      CStdioFile licFile;

      if (licFile.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
      {
         for (int i=0; i<m_saLicsToCheck.GetCount(); i++)
            licFile.WriteString(m_saLicsToCheck[i] + "\n");
      }
      licFile.Close();
   }

   //Free the memory associated with this job
   lc_free_job(m_lmJob);
}

void FLEXlm::readLicAvailablity()
{
   if (m_sAvailLicFName.IsEmpty()) return;

   CString path = m_sCwd + m_sAvailLicFName, line;
   CStdioFile licFile;

   if (!licFile.Open(path, CFile::modeRead))
   {
      ResetLicenseList();
      return;
   }

   //read the file containing information about what to attempt to check out
   while (licFile.ReadString(line))
      m_saLicsToCheck.Add(line);

   licFile.Close();
}

BOOL FLEXlm::isInList(CString feature)
{
   if (m_sAvailLicFName.IsEmpty()) return FALSE;

   for (int i=0; i<m_saLicsToCheck.GetCount(); i++)
      if (m_saLicsToCheck[i] == feature) return TRUE;

   return FALSE;
}

BOOL FLEXlm::shouldCheckout(CString feature)
{
   if (m_sAvailLicFName.IsEmpty()) return TRUE;

   if (m_bCreateLicList) return TRUE;

   return isInList(feature);
}

void FLEXlm::addToCheckList(CString feature)
{
   if (m_sAvailLicFName.IsEmpty()) return;

   if (!isInList(feature)) m_saLicsToCheck.Add(feature);
}

SHORT FLEXlm::InitFLEXlm(CString exePath, CString availableLicsFileName, VOID *funcPtr)
{
   if (m_bIsInitialized) return RET_NO_ERROR;

   {
      //Make sure the registry entry for the license file exists (Did the user configure the license file?)
      HKEY flexKey = NULL;
      LONG result = 0;
      result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\FLEXlm License Manager", 0, KEY_QUERY_VALUE, &flexKey);
      if (result != ERROR_SUCCESS)
      {
         LPVOID lpMsgBuf;
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

         m_sErrMsg = NO_LICMGR_S + "\n\n" + (LPCTSTR)lpMsgBuf;

         // Free the buffer.
         LocalFree( lpMsgBuf );
         return RET_INVALID_PARAM;
      }

      const int licPathSize = 64;  // This allows a successful RegQueryValueEx() when executing a Release Build with debug info and optimization. (knv)
      char licPath[licPathSize];
      DWORD bufLen = licPathSize;
      result = RegQueryValueEx(flexKey, "RSI_LM_LICENSE_FILE", NULL, NULL, (LPBYTE)licPath, &bufLen);
      if ((result == ERROR_SUCCESS && strlen(licPath) <= 0) || (result != ERROR_SUCCESS && result != ERROR_MORE_DATA))
      {
         LPVOID lpMsgBuf;
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

         m_sErrMsg = NOT_CONFIG_S + "\n\n" + (LPCTSTR)lpMsgBuf;

         // Free the buffer.
         LocalFree( lpMsgBuf );
         return RET_INVALID_PARAM;
      }

      RegCloseKey(flexKey);
   }

   if (!m_bLicsAreGood) return RET_NO_LIC;

   SHORT result = 0;

   m_sErrMsg.Empty();
   flexObj = this;
   m_bUseLog = m_bCreateLicList = FALSE;
   m_saLicsToCheck.RemoveAll();

   m_sCwd = exePath;
   m_sAvailLicFName = availableLicsFileName;

   VOID *voidPtr = NULL;
   CString key;

   //check in any checked out licenses
   POSITION pos = m_jobMap.GetStartPosition();
   while (pos)
   {
      m_jobMap.GetNextAssoc(pos, key, voidPtr);
      CheckIn(key);
   }

   readLicAvailablity();

   gExitFuncPtr = (FUNCPTR*)funcPtr;
   result = lc_set_attr(m_lmJob, LM_A_USER_EXITCALL, (LM_A_VAL_TYPE)OnFLEXlmExitCall);
   result = lc_set_attr(m_lmJob, LM_A_CHECKOUTFILTER, (LM_A_VAL_TYPE)OnCheckOutFilter);
   result = lc_set_attr(m_lmJob, LM_A_CHECK_INTERVAL, (LM_A_VAL_TYPE)30);
   result = lc_set_attr(m_lmJob, LM_A_RETRY_INTERVAL, (LM_A_VAL_TYPE)30);
   result = lc_set_attr(m_lmJob, LM_A_PROMPT_FOR_FILE, (LM_A_VAL_TYPE)FALSE);

   m_bIsInitialized = TRUE;
   return RET_NO_ERROR;
}

SHORT FLEXlm::CheckOut(CString moduleName, CString curVersion)
{
   if (!m_bIsInitialized) 
   {
      logMsg((CString)"Check out: " + moduleName + " -- " + NOT_INIT_S);
      m_sErrMsg = NOT_INIT_S;
      return RET_NOT_INITIALIZED;
   }

   if (!moduleName)
   {
      logMsg((CString)"Check out: " + moduleName + " -- Invalid parameter.");
      m_sErrMsg = "Invalid parameter";
      return RET_INVALID_PARAM;
   }

   if (!strlen(moduleName))
   {
      logMsg((CString)"Check out: " + moduleName + " -- Invalid parameter.");
      m_sErrMsg = "Invalid parameter";
      return RET_INVALID_PARAM;
   }

   //See if we should be checking out this module
   if (!shouldCheckout(moduleName))
   {
      logMsg((CString)"Check out: " + moduleName + " -- Set to not check out.");
      m_sErrMsg = "Set to not check out";
      return RET_NO_LIC;
   }

   VOID *voidPtr = NULL;

   SHORT result = 0;
   char feature[MAX_FEATURE_LENGTH+1];
   char version[MAX_VERSION_LENGTH+1];

   //See if the module is already checked out
   if (m_jobMap.Lookup(moduleName, voidPtr))
   {
      logMsg((CString)"Check Out: " + moduleName + " already checked out.");
      return RET_NO_ERROR;
   }

   //Check out the module
   strncpy(feature, moduleName, MAX_FEATURE_LENGTH);
   feature[MAX_FEATURE_LENGTH] = '\0';
   strncpy(version, curVersion, MAX_VERSION_LENGTH);
   version[MAX_VERSION_LENGTH] = '\0';

   result = lc_checkout(m_lmJob, feature, version, 1, LM_CO_NOWAIT, &m_code, LM_DUP_NONE);
   if (result)
   {
      //Check if check out failed due to lack of licenses
      // This means that licenses exist and are accesible, but not at this current time
      // due to having too many users logged in concurrently
      if (result == LM_USERSQUEUED || result == LM_MAXUSERS || result == LM_MAXLIMIT)
      {
         if (m_bCreateLicList)
            addToCheckList(moduleName);
      }

      m_sErrMsg = lc_errstring(m_lmJob);
      logMsg((CString)"Check Out: " + m_sErrMsg);
      return result;
   }

   //Get all feature data and set the information
   featureData *fData = NULL;
   CONFIG *conf = NULL;

   conf = lc_auth_data(m_lmJob, feature);
   if (!conf)
   {
      fData = new featureData(feature);
      m_sErrMsg = (CString)"No feature data was found for " + feature + "\n\n" + lc_errstring(m_lmJob);
      logMsg((CString)"Check Out: " + m_sErrMsg);
   }
   else
      fData = new featureData(m_lmJob, conf);

   //if this license is from the network, then ignore the company and username
   if (fData->user.Find("Network") < 0)
   {
      // set the current company and user name if they have not been already set for future verification
      if (m_sCompany.IsEmpty()) m_sCompany = fData->company;
      if (m_sUser.IsEmpty()) m_sUser = fData->user;
   }

   //Add the job to the map for checking the license back in
   m_jobMap.SetAt(moduleName, (VOID*)fData);

   //Add to the list of licenses to check out for future reference if set to
   if (m_bCreateLicList)
      addToCheckList(moduleName);

   CString msg;
   msg.Format("%s was checked out.\n  User: %s\n  Company: %s\n  job: %x", moduleName, fData->user, fData->company, m_lmJob);
   logMsg(msg);

   return RET_NO_ERROR;
}

SHORT FLEXlm::CheckIn(CString moduleName)
{
   if (!m_bIsInitialized) 
   {
      logMsg((CString)"Check in: " + moduleName + " -- " + NOT_INIT_S);
      m_sErrMsg = NOT_INIT_S;
      return RET_NOT_INITIALIZED;
   }

   if (moduleName.IsEmpty())
   {
      logMsg((CString)"Check in: " + moduleName + " -- Invalid parameter.");
      m_sErrMsg = "Invalid parameter";
      return RET_INVALID_PARAM;
   }

   VOID *voidPtr = NULL;
   featureData *fData = NULL;
   char feature[MAX_FEATURE_LENGTH];
   SHORT result = 0;

   //Make sure the module is already checked out
   if (!m_jobMap.Lookup(moduleName, voidPtr))
   {
      logMsg((CString)"Check in: " + moduleName + " not checked out.");
      return NO_ERROR;
   }

   fData = (featureData*)voidPtr;

   //Check in the module
   strncpy(feature, moduleName, MAX_FEATURE_LENGTH);
   lc_checkin(m_lmJob, feature, 0);

   CString msg;
   msg.Format("%s was checked in.\n  User: %s\n  Company: %s\n  job: %x", moduleName, fData->user, fData->company, m_lmJob);
   logMsg(msg);

   //Remove the job from the map for checking the license back in
   m_jobMap.RemoveKey(moduleName);

   //Free the memory associated with this feature
   delete fData;

   return RET_NO_ERROR;
}

SHORT FLEXlm::GetUserData(CString moduleName, CString userProperty, CString &userData)
{
   if (!m_bIsInitialized) 
   {
      m_sErrMsg = NOT_INIT_S;
      return RET_NOT_INITIALIZED;
   }

   if (moduleName.IsEmpty())
      return RET_INVALID_PARAM;

   if (userProperty.IsEmpty())
      return RET_INVALID_PARAM;

   if (moduleName == LIC_MAIN_DATA)
   {
      //We only want to get the main data, only company and user names are valid
      if (userProperty == FEATURE_USERNAME)
      {
         //Make sure there is a user name
         if (!m_sUser.IsEmpty())
            userData = m_sUser;
         else
         {
            //Since there is no user name, then that means all the licenses have been checked out
            // from the server...so get the name off the first checked out feature.
            POSITION pos = m_jobMap.GetStartPosition();
            featureData *fData;
            VOID *voidPtr;
            CString key;

            //Check if there was something checked out
            if (!pos) return RET_NO_USERDATA_PROPERTY;

            m_jobMap.GetNextAssoc(pos, key, voidPtr);
            if (voidPtr)
            {
               fData = (featureData*)voidPtr;
               userData = fData->user;
            }
         }
      }
      else
      if (userProperty == FEATURE_COMPANY)
      {
         if (!m_sCompany.IsEmpty())
            userData = m_sCompany;
         else
         {
            //Since there is no user name, then that means all the licenses have been checked out
            // from the server...so get the name off the first checked out feature.
            POSITION pos = m_jobMap.GetStartPosition();
            featureData *fData;
            VOID *voidPtr;
            CString key;

            //Check if there was something checked out
            if (!pos) return RET_NO_USERDATA_PROPERTY;

            m_jobMap.GetNextAssoc(pos, key, voidPtr);
            if (voidPtr)
            {
               fData = (featureData*)voidPtr;
               userData = fData->company;
            }
         }
      }
      else
         return RET_NO_USERDATA_PROPERTY;
   }
	else
   if (moduleName == m_sMaintenanceName)
	{
		if (userProperty == FEATURE_EXPIRATIONDATE)
		{
			if (m_sMaintenanceDate.IsEmpty())
			{
				char feature[MAX_FEATURE_LENGTH+1];
				strncpy(feature, moduleName, MAX_FEATURE_LENGTH);
				feature[MAX_FEATURE_LENGTH] = '\0';
				CONFIG *conf = lc_get_config(m_lmJob, feature);
				if (conf)
					m_sMaintenanceDate = conf->date;
				else
					return RET_NO_LIC;
			}

			userData = m_sMaintenanceDate;
			return RET_NO_ERROR;
		}
	}


   featureData *fData = NULL;

   //Make sure the module is already checked out
   if (!m_jobMap.Lookup(moduleName, (void*&)fData))
      return RET_NO_LIC;

   if (userProperty == FEATURE_USERNAME)
   {
      userData = fData->user;
   }
   else
   if (userProperty == FEATURE_COMPANY)
   {
      userData = fData->company;
   }
   else
   if (userProperty == FEATURE_EXPIRATIONDATE)
   {
      userData = fData->expDate;
   }
   else
   if (userProperty == FEATURE_SIGNATURE)
   {
      userData = fData->sign;
   }
   else
   if (userProperty == FEATURE_SERIALNUMBER)
   {
      userData = fData->serial;
   }
   else
   if (userProperty == FEATURE_USINGDONGLE)
   {
      userData = (fData->useDongle?"TRUE":"FALSE");
   }
   else
   if (userProperty == FEATURE_HOSTID)
   {
      userData = fData->hostID;
   }
   else
      return RET_NO_USERDATA_PROPERTY;

   return RET_NO_ERROR;
}

CString FLEXlm::GetErrorString()
{
   return m_sErrMsg;
}

void FLEXlm::logMsg(CString msg)
{
   if (m_bUseLog)
   {
      fp.WriteString("--------------------------\n");
      fp.WriteString(msg + "\n");
   }
}

SHORT FLEXlm::LogBegin(CString logPath)
{
   if (!m_bIsInitialized) 
   {
      m_sErrMsg = NOT_INIT_S;
      return RET_NOT_INITIALIZED;
   }

   if (m_bUseLog) return m_bUseLog;

   m_bUseLog = fp.Open(logPath, CFile::modeCreate|CFile::modeWrite|CFile::typeText);

   return m_bUseLog;
}

SHORT FLEXlm::LogEnd()
{
   if (!m_bIsInitialized) 
   {
      m_sErrMsg = NOT_INIT_S;
      return RET_NOT_INITIALIZED;
   }

   if (!m_bUseLog) return RET_NO_ERROR;

   fp.Close();

   m_bUseLog = FALSE;

   return RET_NO_ERROR;
}

SHORT FLEXlm::ResetLicenseList()
{
   VOID *voidPtr = NULL;
   CString key;

   m_bCreateLicList = TRUE;

   //Make sure only licenses currently checked out are included in the list
   m_saLicsToCheck.RemoveAll();
   POSITION pos = m_jobMap.GetStartPosition();
   while (pos)
   {
      m_jobMap.GetNextAssoc(pos, key, voidPtr);
      addToCheckList(key);
   }

   return RET_NO_ERROR;
}

void FLEXlm::OnNoLicense(CString moduleName, int reason)
{
   VOID *voidPtr = NULL;
   BSTR feature = moduleName.AllocSysString();

   switch (reason)
   {
   case RET_NO_LIC_EXIT_CALL:
      m_sErrMsg = (CString)moduleName + " is no longer avaliable.\n\n" +
         "The server could not be found or is no longer\n" +
         "avaliable. Contact your system administrator for\n" +
         "more information.";
         break;
   }

   (gExitFuncPtr)(moduleName);
}

#endif
