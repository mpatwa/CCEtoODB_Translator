// $Header: /CAMCAD/4.5/Feature.cpp 11    12/06/04 6:46p Alvin $

#include "StdAfx.h"
#include "feature.h"

#ifdef OLD_RSI_FLEXLM

featureData::featureData(void)
{
   this->feature = this->expDate = this->user = this->company = "";
   this->sign = this->vendorStr = this->serial = "";
   this->useDongle = FALSE;
}

featureData::~featureData(void)
{
   this->feature = this->expDate = this->user = this->company = "";
   this->sign = this->vendorStr = this->serial = "";
   this->useDongle = FALSE;
}

featureData::featureData(LM_HANDLE *job, CONFIG *conf)
{
   if (!conf)
   {
      this->feature = this->expDate = this->user = this->company = "";
      this->sign = this->vendorStr = this->serial = "";
      return;
   }

   this->feature = conf->feature;
   this->expDate = conf->date;

   //set the vendor string
   setVendorString(conf->lc_vendor_def);

   //get SIGN
   this->sign = conf->lc_sign;

   //get SN
	// Case 1229 : check if this feature has a parent package
	//   If so, then then serial number is found on the package structure
	if (conf->parent_feat)
		this->serial = conf->parent_feat->lc_serial;
	else
		this->serial = conf->lc_serial;

   //check hostID for dongle usage
   checkForDongle(job);
}

featureData::featureData(CString feat, CString date, CString userName, CString companyName,
                         CString signature, CString vendorString, CString serialNumber)
{
   this->feature = feat;
   this->expDate = date;
   this->user = userName;
   this->company = companyName;
   this->sign = signature;
   this->vendorStr = vendorString;
   this->serial = serialNumber;
}

void featureData::setVendorString(CString vStr)
{
   this->vendorStr = vStr;

   //parse VENDOR_STRING
   CString tmp;
   tmp = vStr.Mid(vStr.Find("-")+1);


   this->user = tmp.Mid(0, tmp.Find(VSTR_DELIM));
   this->company = tmp.Mid(tmp.Find(VSTR_DELIM)+1);
}

void featureData::checkForDongle(LM_HANDLE *job)
{
   // TODO: ?? not working correctly...need to research better.
   char buf[MAX_CONFIG_LINE];
   int retVal = lc_hostid(job, HOSTID_DEFAULT, buf);

   hostID = buf;
   useDongle = (hostID.Find("FLEXid=") >= 0);
}

#endif

