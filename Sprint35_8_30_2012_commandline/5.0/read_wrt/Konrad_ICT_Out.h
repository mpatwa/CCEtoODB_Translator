
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2004. All Rights Reserved.
*/

#pragma once

#ifndef __KONRAD_ICT_O__
#define __KONRAD_ICT_O__

#include "DeviceType.h"

class CKonradComponent;

//-----------------------------------------------------------

class CKonradSettings
{
private:
   bool m_reportFileNotFoundError; // meta - about the settings reader, not a setting itself

	bool m_ExportUnloadedParts;
   bool m_WriteInaccessible;
	bool Load_Konradsettings(const CString fname);

public:
	CKonradSettings(bool reportFileNotFound = true);
	~CKonradSettings();

    bool getExportUnloadedParts() { return m_ExportUnloadedParts; }
    bool getWriteInaccessible()   { return m_WriteInaccessible;   }
};

//-----------------------------------------------------------
class CKonradTestElement
{
private:
   CSubclassElement *m_scElement;

   CKonradComponent *m_kcomp;

   CString m_konradDevtype;
   DeviceTypeTag m_devtype;

   CString m_refname;
   CString m_infovalue;
   CString m_testvalue;
   CString m_partnumber;
   CString m_ptol;
   CString m_ntol;
   CString m_mergedvalue;
   CString m_mergedStatus;
   CString m_loadedValue;

   CompPinStruct *m_pin1cp;
   CompPinStruct *m_pin2cp;
   NetStruct *m_pin1net;
   NetStruct *m_pin2net;

public:
   CKonradTestElement(CKonradComponent &kcomp, CSubclassElement *el = NULL);

   DeviceTypeTag GetDeviceTypeTag()    { return m_devtype; }

   bool IsGeneratedElement()                 { return m_scElement != NULL; }
   CSubclassElement *GetSubclassElement()    { return m_scElement; }
   CKonradComponent *GetKComp()              { return m_kcomp; }

   bool Write(CStdioFileWriteFormat &outFile, CMapStringToString &probeMap, FILE* logFile);
};

//-----------------------------------------------------------

class CKonradTestElementList : public CTypedPtrArrayContainer<CKonradTestElement*>
{
};

//-----------------------------------------------------------

class CKonradComponent
{
private:
   CCEtoODBDoc *m_doc;
   CString m_sortableRefname;
   DataStruct *m_compData;
   DeviceTypeTag m_deviceType;
   bool m_supportedDevType;
   CKonradSettings m_krSettings;
   CKonradTestElementList m_testElementList;

public:
   CKonradComponent(CCEtoODBDoc *doc, DataStruct *data);

   CString GetSortKey()    { return m_sortableRefname; }
   DataStruct *GetCompData()  { return m_compData; }
   CString GetDeviceType()    { return deviceTypeTagToValueString(m_deviceType); }
   DeviceTypeTag GetDeviceTypeTag() { return m_deviceType; }

   bool IsSupported()            { return m_supportedDevType; }
   void SetSupported(bool flg)   { m_supportedDevType = flg; }

   CString GetRefname()          { return this->GetCompData()->getInsert()->getRefname(); }

   CKonradTestElementList &GetTestElementList(CSubclassList &subclassList);

   CString GetValue();
   CString GetPlusTolerance();
   CString GetMinusTolerance();
   CString GetNormalizedValue();
   CString GetPartNumber();
   CString GetMergedStatus();
   CString GetMergedValue();
   CString GetLoaded();
   CString GetSubclass();
   CString GetAttribValue ( Attrib *attrib );
   
   bool GetCompPin(CString pinRefname, CompPinStruct **retCompPin, NetStruct **retNet);
   bool GetConnectedPin(int pinnum, CompPinStruct **compPin, NetStruct **net);
   CString GetKonradDevType(DeviceTypeTag devtypetag = deviceTypeUndefined);

   CKonradSettings *getSettings()	{return &m_krSettings; }

};

//-----------------------------------------------------------

class CKonradICTWriter : public CObject
{
private:
   CCEtoODBDoc &m_doc;
   FileStruct &m_file;
   FILE* m_logFile;
   CString m_localLogFilename;
   CTypedPtrArrayContainer<CKonradComponent*> m_componentList;
   CSubclassList m_subclassList;

   CMapStringToString m_probeMap;

   CKonradSettings m_krSettings;

   void FillProbeMap();

   bool WriteComponents(CStdioFileWriteFormat &outFile, DeviceTypeTag targetDeviceType, CStringArray &skippedCompMsgs);
   //void WriteComponent(CStdioFileWriteFormat &outFile)

public:
	CKonradICTWriter(CCEtoODBDoc &doc, FileStruct &file, CString outputFilename);
   ~CKonradICTWriter();

   void GatherComponents();
   void SortComponents();

   bool WriteFile(CString filename);
   void WriteUnsupportedToLog();
   
   CKonradSettings *getSettings()	{return &m_krSettings; }
   
};



#endif // __KONRAD_ICT_O__
