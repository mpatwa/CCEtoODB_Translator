// $Header: /CAMCAD/4.5/read_wrt/FabMasterDeviceIn.h 8     9/29/06 8:42p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#if ! defined (__FabMasterIn_h__)
#define __FabMasterIn_h__

#pragma once

#include "TypedContainer.h"
#include "CamCadDatabase.h"

enum ECorrelationFlag
{
	correlateAuto,      // First try lookup by refdes, if fail then lookup by partnumber
	correlateRefDes,    // Lookup by refdes only
	correlatePartNumber // Lookup by part number only
};


//_____________________________________________________________________________
class CFabMasterDeviceFileSettings
{
private:
   ECorrelationFlag m_correlation;
   bool m_useFilPartnumber;

public:
   CFabMasterDeviceFileSettings();

   // accessors
   ECorrelationFlag getCorrelation() { return m_correlation; }
   bool getUseFilPartnumber()          { return m_useFilPartnumber; }
   void setUseFilPartnumber(bool flg)  { m_useFilPartnumber = flg; }
   
   // operations
   bool read(CWriteFormat* errorWriteFormat=NULL);
};

//_____________________________________________________________________________
class CFabMasterPart
{
private:
   CString m_partNumber;
   CMapStringToString m_namedValueMap;

   static int m_deviceTypeKeywordIndex;
   static int m_partnumKeywordIndex;
   static int m_valueKeywordIndex;
   static int m_pToleranceKeywordIndex;
   static int m_nToleranceKeywordIndex;
   static int m_subclassKeywordIndex;
   static int m_componentHeightKeywordIndex;
   static int m_capOpensKeywordIndex;
   static int m_diodeOpensKeywordIndex;


public:
   CFabMasterPart(const CString& partNumber);

   static int getDeviceTypeKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getPartnumKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getValueKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getPToleranceKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getNToleranceKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getSubclassKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getComponentHeightKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getCapOpensKeywordIndex(CCamCadDatabase& camCadDatabase);
   static int getDiodeOpensKeywordIndex(CCamCadDatabase& camCadDatabase);
   static void resetKeywordIndices();

   void addNamedValue(const CString& name,const CString& value);

   // operations
   void updateComponentAttributes(CCamCadDatabase& camCadDatabase,DataStruct& pcbComponent, CFabMasterDeviceFileSettings& settings);
};

//_____________________________________________________________________________
class CFabMasterParts
{
private:
	CTypedMapStringToPtrContainer<CFabMasterPart*> m_parts;
	CTypedMapStringToPtrContainer<CFabMasterPart*> m_refs;
	CTypedMapStringToPtrContainer<CString*> m_refmap;
	CFabMasterDeviceFileSettings m_settings;

public:
   CFabMasterParts();

   CFabMasterPart* getPart(const CString& partName);
   CFabMasterPart& getDefinedPart(const CString& partName);

	CFabMasterPart* getRef(const CString& partName);
   CFabMasterPart& getDefinedRef(const CString& partName);

   CFabMasterDeviceFileSettings &getSettings()  { return m_settings; }

   // operations
   void updateComponentAttributes(CCamCadDatabase& camCadDatabase);
	void makeRefDesMap();
};

//_____________________________________________________________________________
class CFabMasterDeviceFile
{
private:
   CString m_filePath;
   CFabMasterParts m_partlist;

public:
   CFabMasterDeviceFile(const CString& filePath);

   // accessors
   CFabMasterParts& getParts() { return m_partlist; }
   
   // operations
   bool read(CWriteFormat* errorWriteFormat=NULL);
};

//_____________________________________________________________________________
class CSmartRefdes
{
private:
   CString m_prefix;
   int m_number;

public:
	CSmartRefdes(const CString& wholeRefdes);
	CString getPrefix() { return m_prefix; }
	int getNumber() { return m_number; }
};


#endif
