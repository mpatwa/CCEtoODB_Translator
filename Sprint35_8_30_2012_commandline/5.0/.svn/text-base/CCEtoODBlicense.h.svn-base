
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// CCETOODBLICENSE.H

#if ! defined (__CamcadLicense_h__)
#define __CamcadLicense_h__

#pragma once

#include "DbUtil.h"
#include "TypedContainer.h"
#include "Lic.h"
#include "License.h"

// Are these license no longer use?

// Kill List for 4.6
// =================
// LIC_BNRIN_S                 16  "Unicad/BNR Read"
// LIC_CCTIN_S                 22  "CCT Design File Read"
// LIC_HP3070IN_S              41  "Agilent 3070 Read"
// LIC_PADSLIBIN_S             115 "Pads Library (.d) Read"
// LIC_PDIFSCHIN_S             51  "PCAD PDIF Schematic Read"
// LIC_PFWSCHIN_S              52  "Protel PFW Schematic Read"
// LIC_ULTIBOARDIN_S           55  "ULTIBOARD (.edf) Layout Read"
// LIC_VIEWDRAWIN_S            58  "VIEWDRAW Read"
// LIC_CADIFOUT_S              66  "Redac CADIF (.paf) Layout Write"
// LIC_FATFOUT_S               77  "FABMASTER FATF Write"
// LIC_MENTNEUTOUT_S           90  "Mentor Neutral File Write"
// LIC_ODBPPOUT_S              92  "ODB++ Write"
// LIC_RDCOUT_S                103 "RDC-Dbframe Write"
// LIC_TAKAYA8OUT_S            106 "TAKAYA .CA8 Write"
// LIC_THEDAOUT_S              110 "THEDA (.tl) Layout, Panel Write"


enum MentorProductIdTag
{
   mentorProductCpUnknown                             = -1,

   //
   // These are tags for Camcad Professional
   //
   mentorProductCpCamcadProfessional                  = 3604,      // Older base license key, supported for backward compatibility
   mentorProductCpCamcadCPPROBASE                     = 100274,    // As of 4.7 this is the new standard base license key
   //mentorProductCpCamcadCmdLineImportExportCPCMD      = 100594,    // Command line import CAD, export CCZ
   mentorProductCpCamcadCmdLineImportExportCPCMD      = 102219,
   
};


//-----------------------------------------------------------------------------
// MentorProductLicense
//-----------------------------------------------------------------------------
class MentorProductLicense
{
public:
   MentorProductLicense(const MentorProductIdTag mentorProductId);
   ~MentorProductLicense();

private:
   MentorProductIdTag m_mentorProductId;
   long m_transactionId;
   bool m_isLicenseGranted;
   bool m_isQueued;

public:
   MentorProductIdTag getMentorProductId()const { return m_mentorProductId; }

   void setTranactionId(const long transactionId) { m_transactionId = transactionId; }
   long getTranactionId()const { return m_transactionId; }

   void setLicenseGranted(const bool isGranted) { m_isLicenseGranted = isGranted; }
   bool getLicenseGranted()const { return m_isLicenseGranted; }

   void setQueued(const bool isQueued) { m_isQueued = isQueued; }
   bool isQueued()const { return m_isQueued; }

   //CString getMentorProductName()const;
};

struct DateStruct
{  //                          legal range  true range
   unsigned int   hour:5;     // [0..23]     [0..31]
   unsigned int   minute:6;   // [0..59]     [0..63]
   unsigned int   month:4;    // [1..12]     [0..15]
   unsigned int   day:5;      // [1..31]     [0..31]
   unsigned int   year:12;    // [1997..)    [0..4095]
};

//-----------------------------------------------------------------------------
// CamcadLicenseBase
//-----------------------------------------------------------------------------
class CamcadLicenseBase
{
public:
   CamcadLicenseBase();
   ~CamcadLicenseBase();

protected:
   CString m_camcadLicenseKey;
   int m_licensedApiLevel;
   bool m_initializedLicenses;
   bool m_checkOutLicenses;
   bool m_hasBaseLicense;

   DateStruct m_expirationDate;
   bool m_useDongle;
   bool m_hasExpiration;
   CString m_serialNumber;
   CString m_company;
   CString m_user;
   CString m_hostId;

public:
   bool hasBaseLicense()const { return m_hasBaseLicense; }

   CString getCamcadLicenseKey() const { return m_camcadLicenseKey; }
   int getLicensedApiLevel() const { return m_licensedApiLevel; }
   DateStruct getExpirationDate() const { return m_expirationDate; }
   CString getCompany() const { return m_company; }
   CString getUser() const { return m_user; }
};

//-----------------------------------------------------------------------------
// CamcadLicense
//-----------------------------------------------------------------------------
class CamcadLicense : public CamcadLicenseBase
{
public:
   CamcadLicense();
   ~CamcadLicense();

private:
   CTypedMapIntToPtrContainer<MentorProductLicense*> m_mentorProductLicenseMap;
   CMapStringToString m_availableProductMap;
   CString m_camcadLicenseExactAccessDate;
   CString m_availableLicenseFileName;
   MentorProductIdTag m_baseLicenseProductId;

   long checkOutLicense(MentorProductLicense& mentorProductLicense, CString& errorMessage, const bool rescan);

public:
   MentorProductIdTag getBaseMentorProductId() const { return m_baseLicenseProductId; }

   void initializeLicenses();
   bool checkOutLicenses();
   bool checkOutLicenses(const bool rescan);
};



#endif
