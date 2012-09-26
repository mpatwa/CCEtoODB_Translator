// $Header: /CAMCAD/4.5/FLEXlm.h 15    2/18/05 4:49p Alvin $

#pragma once

#include "lmclient.h" 
#include "lm_attr.h"

//error constants
#define RET_NO_ERROR                 0
#define RET_INVALID_PARAM           -1
#define RET_NO_LIC                  -2
#define RET_NO_LIC_EXIT_CALL        -3
#define RET_NO_LIC_NO_DONGLE        -4
#define RET_NO_USERDATA_PROPERTY    -5
#define RET_NOT_INITIALIZED         -6

#define LIC_MAIN_DATA            "MAIN_DATA"

#define FEATURE_USERNAME         "FEATURE_USERNAME"
#define FEATURE_COMPANY          "FEATURE_COMPANY"
#define FEATURE_EXPIRATIONDATE   "FEATURE_EXPIRATIONDATE"
#define FEATURE_SIGNATURE        "FEATURE_SIGNATURE"
#define FEATURE_SERIALNUMBER     "FEATURE_SERIALNUMBER"
#define FEATURE_HOSTID				"FEATURE_HOSTID"
#define FEATURE_USINGDONGLE      "FEATURE_USINGDONGLE"

#define MAX_FEATURE_LENGTH 30
#define MAX_VERSION_LENGTH 10

#ifdef DEADCODE
// Obsolete, support for original RSI FLEXlm, we now use MGLS.

class FLEXlm
{
public:
   FLEXlm(void);
   ~FLEXlm(void);

private:
   CString m_sCwd;

   BOOL m_bIsInitialized;
   BOOL m_bLicsAreGood;
   LM_HANDLE *m_lmJob;
   VENDORCODE m_code;

   CMapStringToPtr m_jobMap;
   CString m_sErrMsg;
   CString m_sUser;
   CString m_sCompany;

	CString m_sMaintenanceName;
	CString m_sMaintenanceDate;

   CStdioFile fp;
   BOOL m_bUseLog;

   CString m_sAvailLicFName;
   CStringArray m_saLicsToCheck;
   BOOL m_bCreateLicList;

   void logMsg(CString msg);

   void readLicAvailablity();
   void addToCheckList(CString feature);
   BOOL shouldCheckout(CString feature);
   BOOL isInList(CString feature);

public:

   SHORT InitFLEXlm(CString exePath, CString availableLicsFileName, VOID *funcPtr);
	void SetMaintenanceModuleName(CString moduleName)												{ m_sMaintenanceName = moduleName; };
   SHORT CheckOut(CString moduleName, CString curVersion);
   SHORT CheckIn(CString moduleName);
   CString GetErrorString();
   SHORT GetUserData(CString moduleName, CString userProperty, CString &userData);

   SHORT LogBegin(CString logPath);
   SHORT LogEnd();

   SHORT ResetLicenseList();
   void OnNoLicense(CString moduleName, int reason);
};

#endif