
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2004. All Rights Reserved.
*/

#pragma once

#ifndef __Testronics_ICT_O__
#define __Testronics_ICT_O__

#include "DeviceType.h"

//-----------------------------------------------------------

class CTestronicsSettings
{
private:
	bool m_ExportUnloadedParts;
   bool m_WriteInaccessible;
	bool Load_TestronicsSettings(const CString fname, bool silenceErrMsgs);

public:
	CTestronicsSettings(bool silenceErrMsg);
	~CTestronicsSettings();

    bool getExportUnloadedParts() { return m_ExportUnloadedParts; }
    bool getWriteInaccessible()   { return m_WriteInaccessible;   }
};

//-----------------------------------------------------------

class CTestronicsComponent : public CObject
{
private:
   CCEtoODBDoc *m_doc;
   CString m_sortableRefname;
   DataStruct *m_compData;
   DeviceTypeTag m_deviceType;
   bool m_supportedDevType;
   CTestronicsSettings m_krSettings;

public:
   CTestronicsComponent(CCEtoODBDoc *doc, DataStruct *data);

   CString GetSortKey()    { return m_sortableRefname; }
   DataStruct *GetCompData()  { return m_compData; }
   CString GetDeviceType()    { return deviceTypeTagToValueString(m_deviceType); }
   DeviceTypeTag GetDeviceTypeTag() { return m_deviceType; }

   CString GetSortableRefname()  { return m_sortableRefname; }

   bool IsSupported()            { return m_supportedDevType; }
   void SetSupported(bool flg)   { m_supportedDevType = flg; }


   CString GetValue();
   CString GetPlusTolerance();
   CString GetMinusTolerance();
   CString GetNormalizedValue();
   CString GetPartNumber();
   CString GetSubclass();
   CString GetMergedStatus();
   CString GetMergedValue();
   CString GetLoaded();
   CString GetAttribValue ( Attrib *attrib );
   
   bool GetConnectedPin(int pinnum, CompPinStruct **compPin, NetStruct **net);
   CString GetTestronicsDevType();

   bool Write(CStdioFileWriteFormat &outFile, CMapStringToString &probeMap, FILE *logFile);

   CTestronicsSettings *getSettings()	{return &m_krSettings; }

};

//-----------------------------------------------------------

class CTestronicsICTWriter
{
private:
   CCEtoODBDoc &m_doc;
   FileStruct &m_file;
   FILE* m_logFile;
   CString m_localLogFilename;
   //CTypedPtrArrayContainer<CTestronicsComponent*> m_componentList;
   CMapSortedStringToOb<CTestronicsComponent> m_componentList;
   CMapStringToString m_probeMap;

   CTestronicsSettings m_krSettings;

   void FillProbeMap();

   bool WriteComponents(CStdioFileWriteFormat &outFile, DeviceTypeTag targetDeviceType);
   //void WriteComponent(CStdioFileWriteFormat &outFile)
   bool WritePins(CStdioFileWriteFormat &outFile);
   void WriteHeader(CStdioFileWriteFormat &outFile);

public:
	CTestronicsICTWriter(CCEtoODBDoc &doc, FileStruct &file, CString outputFilename);
   ~CTestronicsICTWriter();

   void GatherComponents();
   void SortComponents();

   bool WriteFile(CString filename);
   void WriteUnsupportedToLog();
   
   CTestronicsSettings *getSettings()	{return &m_krSettings; }
   
};



#endif // __Testronics_ICT_O__
