// $Header: /CAMCAD/4.5/read_wrt/Agilent_i1000_Writer.h    4/03/06 12:47p Rick Faltersack $

/*****************************************************************************/

#if !defined(__Agilent_i1000_Writer_h__)
#define __Agilent_i1000_Writer_h__

#pragma once

#include "DeviceType.h"

class CAgI1000Probe;
class CAgI1000ProbeMap;

////////////////////////////////////////////////////////////////////////////////////////////////

class CAgI1000CompDataTest : public CObject
{
private:
   DeviceTypeTag m_devtype;

   CString m_probeflag;
   CString m_refname;
   CString m_gridlocation;
   CString m_value;
   CString m_plustol;
   CString m_minustol;
   CString m_testmode;
   CString m_pin1probe;
   CString m_pin2probe;

public:
   CAgI1000CompDataTest(DeviceTypeTag devType, CString probeFlag, CString refName, CString gridLoc, 
      CString value, CString plusTol, CString minusTol, 
      CString testMode, CString pin1probe, CString pin2probe);

   CAgI1000CompDataTest(DeviceTypeTag devType, CString probeFlag, CString refName, CString gridLoc, 
      CString value, CString plusTol, CString minusTol, 
      CString testMode, int pin1probe, int pin2probe);

   DeviceTypeTag GetDeviceType() { return m_devtype;}

   CString GetProbeFlag()        { return m_probeflag.IsEmpty()    ? "@" : m_probeflag;    }
   CString GetRefName()          { return m_refname.IsEmpty()      ? "@" : m_refname;      }
   CString GetGridLocation()     { return m_gridlocation.IsEmpty() ? "@" : m_gridlocation; }
   CString GetValue()            { return m_value.IsEmpty()        ? "@" : m_value;        }
   CString GetPlusTolerance()    { return m_plustol.IsEmpty()      ? "@" : m_plustol;      }
   CString GetMinusTolerance()   { return m_minustol.IsEmpty()     ? "@" : m_minustol;     }
   CString GetTestMode()         { return m_testmode.IsEmpty()     ? "@" : m_testmode;     }
   CString GetPin1Probe()        { return m_pin1probe.IsEmpty()    ? "@" : m_pin1probe;    }
   CString GetPin2Probe()        { return m_pin2probe.IsEmpty()    ? "@" : m_pin2probe;    }

};


class CAgI1000CompDataTestList : public CTypedPtrListContainer<CAgI1000CompDataTest*>
{
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CAgI1000Via : public CObject
{
private:
   DataStruct *m_camcadViaData;
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbFile;

   CAgI1000Probe *m_probe;

public:
   CAgI1000Via(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadViaData, CAgI1000Probe *probe) { m_doc = doc; m_pcbFile = pcbFile; m_camcadViaData = camcadViaData; m_probe = probe; }

   DataStruct *GetCamcadViaData()      { return m_camcadViaData; }
   CAgI1000Probe *GetProbe()           { return m_probe; }

   bool IsSMD();
};

class CAgI1000ViaMap : public CMapSortedStringToOb<CAgI1000Via>
{
private:
   static int AscendingRefnameSortFunc(const void *a, const void *b);
   
public:
   void CollectVias(CCEtoODBDoc *doc, FileStruct *pcbFile, CAgI1000ProbeMap &probeMap);
};


////////////////////////////////////////////////////////////////////////////////////////////////

class CAgI1000Pin : public CObject
{
private:
   DataStruct *m_camcadPartData;
   DataStruct *m_camcadPinData;
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbFile;

   CompPinStruct *m_comppin;
   NetStruct *m_net;

public:
   CAgI1000Pin(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData, DataStruct *camcadPinData);

   DataStruct *GetCamcadPinData()      { return m_camcadPinData; }
   NetStruct *GetNet()                 { return m_net; }

};

class CAgI1000PinMap : public CMapSortedStringToOb<CAgI1000Pin>
{
private:
   static int AscendingRefnameSortFunc(const void *a, const void *b);

public:
   void CollectPins(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CAgI1000Part : public CObject
{
private:
   DataStruct *m_camcadPartData;
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbFile;
   DeviceTypeTag m_deviceType;

   CAgI1000PinMap m_pinMap;

   CompPinStruct *m_anodePin;     // Or pin 1 if pins not typed
   CompPinStruct *m_cathodePin;   // Or pin 2 if pins not typed
   CompPinStruct *m_basePin;
   CompPinStruct *m_collectorPin;
   CompPinStruct *m_emitterPin;

   NetStruct *m_anodeNet;
   NetStruct *m_cathodeNet;
   NetStruct *m_baseNet;
   NetStruct *m_collectorNet;
   NetStruct *m_emitterNet;

   CompPinStruct *FindCompPinAndNet(PinTypeTag desiredPinType, NetStruct *&net);

   CString GetValueAttrib();
   CString GetPlusToleranceAttrib();
   CString GetMinusToleranceAttrib();
   CString GetCapacitiveOpensAttrib();

public:
   CAgI1000Part(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadPartData);

   DataStruct *GetCamcadPartData()  { return m_camcadPartData; }
   DeviceTypeTag GetDeviceType()    { return m_deviceType; }

   CompPinStruct *GetCompPin(PinTypeTag pinType);
   NetStruct *GetPinNet(PinTypeTag pinType);

   CAgI1000PinMap &GetPinMap()   { return m_pinMap; }

   CString GetGridLocation();
   CString GetValue();
   CString GetPlusTolerance();
   CString GetMinusTolerance();

   bool TestCapacitiveOpens();
   bool IsSMD();

   CString GetDescription(CString usersDescAttribName);
};

class CAgI1000PartMap : public CMapSortedStringToOb<CAgI1000Part>
{
private:
   static int AscendingRefnameSortFunc(const void *a, const void *b);

public:
   void CollectParts(CCEtoODBDoc *doc, FileStruct *pcbFile);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CAgI1000Probe : public CObject
{
private:
   DataStruct *m_camcadProbeData;
   DataStruct *m_accessMarkerData;
   CEntity m_probedEntity;
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbFile;
   int m_probeNumber;
   bool m_placed;

   CString m_probedNetName;
   CString m_probedNetType;

   CString GetNetNameAttrib(DataStruct *data);
   CString GetNetNameAttrib(CompPinStruct *compPin);

public:
   CAgI1000Probe(CCEtoODBDoc *doc, FileStruct *pcbFile, DataStruct *camcadProbeData);

   DataStruct *GetCamcadProbeData()  { return m_camcadProbeData; }
   DataStruct *GetAccessMarkerData() { return m_accessMarkerData; }

   bool IsPlaced()      { return m_placed; }
   bool IsProbedDiePin();

   CString GetProbedNetName();
   CString GetProbedNetType(); // "POWER", "GROUND", or "SIGNAL"
   int GetProbeNumber()          { return m_probeNumber; }
   void SetProbeNumber(int num);

   CString GetGridLocation();

   EEntityType GetProbedItemEntityType() { return m_probedEntity.getEntityType(); };
   InsertTypeTag GetProbedItemInsertType();
   DataStruct *GetProbedItemData()     { return ((m_probedEntity.getEntityType() == entityTypeData) ? m_probedEntity.getData() : NULL); }
   CString GetProbedItemRefName();
   CString GetProbedItemPinName();

};

class CAgI1000ProbeMap : public CMapSortedStringToOb<CAgI1000Probe>
{
private:
   FILE *m_logFp;

   static int AscendingRefnameSortFunc(const void *a, const void *b);

   CString EncodeNetname(CString netname, int probeNumber);

   void ValidateProbeNumbers();
   int AssignProbeNumbers(int startingNumber);

public:
   CAgI1000ProbeMap(FILE *logFp)  {m_logFp = logFp;}
   int CollectProbes(CCEtoODBDoc *doc, FileStruct *pcbFile, bool collectPlaced, int startingProbeNumber);

   CAgI1000Probe *LookupNthProbe(CString netname, int n);
   CAgI1000Probe *LookupFirstProbe(CString netname)       { return LookupNthProbe(netname, 1); }

   CString GetProbeListForNet(CString netname, CString separator);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CAgilentI1000WriterSettings
{
private:
   bool m_exportTestJet;
   CString m_descriptionAttribName;
   double m_pinSpacingThreshold;
   PageUnitsTag   m_pinSpacingThresholdUnits;
   PageUnitsTag   m_outputUnits;
   int m_outputDecimalPlaces;
   CMapWordToPtr m_deviceTypeMap;

   void AddPinShortTestType(CString devTypeStr);

public:
   CAgilentI1000WriterSettings();
   void Read();

   bool GetExportTestJetFlag()                     { return m_exportTestJet; }

   CString GetDescriptionAttribName()              { return m_descriptionAttribName; }

   double GetPinSpacingThreshold(PageUnitsTag scaleToUnits) { return m_pinSpacingThreshold * Units_Factor(m_pinSpacingThresholdUnits, scaleToUnits); }
    
   double GetOutputValue(double camcadValue, PageUnitsTag camcadUnits) { return camcadValue * Units_Factor(camcadUnits, m_outputUnits); }
   int    GetOutputDecimalPlaces()  { return m_outputDecimalPlaces; }
  
   bool IsPinShortTestType(DeviceTypeTag devType)  { void *ptr; return (m_deviceTypeMap.Lookup((WORD)devType, ptr)) ? true : false; }
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CAgilentI1000Writer
{
private:
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbFile;

   CAgI1000PartMap m_partMap;
   CAgI1000ProbeMap m_placedProbeMap;
   CAgI1000ProbeMap m_unplacedProbeMap;
   CAgI1000ViaMap m_viaMap;
   CAgI1000CompDataTestList m_testList;

   CAgilentI1000WriterSettings m_settings;
   FILE *m_logFp;
   CString m_localLogFilename;

   void InitData();
   bool GetLogFile(CString outputfolderpath, CString logname);

   CString GetComponentTypeSectionName(DeviceTypeTag devType);
   CString GetComponentTestMode(DeviceTypeTag devType);

   void GenerateATDFileCompDataTest(CAgI1000Part *part, PinTypeTag pin1Type, PinTypeTag pin2Type, bool putPinsInRefname = false);
   void CollectATDFileCompDataTests(DeviceTypeTag devTypeToCollect);

   void GenerateATDFileCompDataPinShortTest(CAgI1000Part *part, InsertStruct *pinAInsert, InsertStruct *pinBInsert);
   void CollectATDFileCompDataPinShortTests();

   void WriteATDFile(CStdioFileWriteFormat& atdFile);

   void WriteATDFileComponentDataTests(CStdioFileWriteFormat& atdFile);
   void WriteATDFileCompDataTests(CStdioFileWriteFormat& atdFile, DeviceTypeTag devTypeToWrite);
   void WriteATDFileCompDataTest(CStdioFileWriteFormat& atdFile, int recId, CAgI1000CompDataTest *test);

   void WriteATDFileCapOpensTests(CStdioFileWriteFormat& atdFile);
   void WriteATDFileCapOpensTest(CStdioFileWriteFormat& atdFile, CAgI1000Part *part);

   void WriteNailsFile(CStdioFileWriteFormat& nailsFile);

   void WritePinsFile(CStdioFileWriteFormat& pinsFile);
   void WritePinsFileHeader(CStdioFileWriteFormat& pinsFile);
   void WritePinsFileComponents(CStdioFileWriteFormat& pinsFile);
   void WritePinsFileVias(CStdioFileWriteFormat& pinsFile);


public:
   CAgilentI1000Writer(CCEtoODBDoc *doc, FileStruct *pcbFile);

   void WriteFiles(CString outputfolderpath);
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
