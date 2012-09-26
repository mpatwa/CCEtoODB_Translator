// $Header: /CAMCAD/4.3/Feature.h 8     8/12/03 9:06p Kurt Van Ness $

#pragma once

#ifdef OLD_RSI_FLEXLM

#include "flexlm.h"

#define VSTR_DELIM "|"

class featureData
{
private:
   // member functions
   void setVendorString(CString vStr);
   void checkForDongle(LM_HANDLE *job);

public:
   // member variables
   CString feature;
   CString expDate;
   CString user;
   CString company;
   CString sign;
   CString serial;
   CString vendorStr;
   CString hostID;
   BOOL useDongle;
   
   ///////////////////////////////////////////////////////////
   // Default Constructor
   featureData(void);
   featureData(LM_HANDLE *job, CONFIG *conf);
   featureData(CString feat, CString date = "", CString userName = "", CString companyName = "",
         CString signature = "", CString vendorString = "", CString serialNumber = "");

   // Destructor
   ~featureData(void);
   //
   ///////////////////////////////////////////////////////////
};

#endif
