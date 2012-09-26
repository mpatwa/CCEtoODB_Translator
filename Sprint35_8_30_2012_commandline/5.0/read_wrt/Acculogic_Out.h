// $Header: /CAMCAD/5.0/read_wrt/Acculogic_Out.h 1     5/22/07 2:47a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2007. All Rights Reserved.
*/

#pragma once

#ifndef __ACCULOGIC_OUT_H_INCLUDED__
#define __ACCULOGIC_OUT_H_INCLUDED__

#include "DeviceType.h"
#include "DcaEntity.h"

//-----------------------------------------------------------

class CAcculogicComponent
{
private:
   CCEtoODBDoc *m_doc;
   CString m_sortableRefname;
   DataStruct *m_compData;
   DeviceTypeTag m_deviceType;
   bool m_supportedDevType;
   int m_partID; 

public:
   CAcculogicComponent(CCEtoODBDoc *doc, DataStruct *data);

   CString GetSortKey()    { return m_sortableRefname; }
   DataStruct *GetCompData()  { return m_compData; }
   CString GetDeviceType()    { return deviceTypeTagToValueString(m_deviceType); }
   DeviceTypeTag GetDeviceTypeTag() { return m_deviceType; }

   bool IsSupported();

   CString GetSubclassAttrib();
   CString GetValueAttrib();
   CString GetPartnumberAttrib();
   CString GetGridLocationAttrib();
   CString GetMountTechAttrib();
   CString GetCapacitiveOpensAttrib();
   CString GetDiodeOpensAttrib();
   double  GetCompHeightAttrib();

   CString GetCompValue();
   CString GetCompValueUnits();
   CString GetPlusTolerance();
   CString GetMinusTolerance();

   int GetPartID()         { return m_partID; }
   void SetPartID(int id)  { m_partID = id;   }

   BlockStruct *GetInsertedCompGeometryBlock();


   CString GetAcculogicDevType();

};

//-----------------------------------------------------------

class CAcculogicPin
{
private:
   CCEtoODBDoc *m_doc;
   CString m_netName;
   CString m_pinName;
   double m_locX;
   double m_locY;
   CompPinStruct *m_cp;
   CAcculogicComponent *m_parentComp;

public:
   CAcculogicPin(CCEtoODBDoc *doc, CAcculogicComponent *parentcomp, CString pinname, double x, double y, CString netname, CompPinStruct *cp) { m_doc = doc; m_parentComp = parentcomp; m_pinName = pinname; m_locX = x; m_locY = y; m_netName = netname; m_cp = cp;}

   CompPinStruct *GetCompPin()            { return m_cp; }
   CAcculogicComponent *GetParentComp()   { return m_parentComp; }
   CString GetPinName()                   { return m_pinName; }
   CString GetNetName()                   { return m_netName; }
   double GetX()                          { return m_locX; }
   double GetY()                          { return m_locY; }
   CString GetAcculogicPinName();


};

//-----------------------------------------------------------

class CAcculogicProbe
{
private:
   DataStruct *m_data;
   CString m_refname;
   bool m_IsProbedDiePin;
   CEntity m_accessEntity;
   CEntity m_probeEntity;

public:
   CAcculogicProbe(CCEtoODBDoc *doc, DataStruct *probedata);
   DataStruct *GetProbeData()    { return m_data; }
   CString GetRefname()          { return m_refname; }
   void SetRefname(int probenum) { m_refname.Format("%d", probenum); }
   bool IsProbedDiePin() const   {  return m_IsProbedDiePin;}

   void SetProbedDiePin(CCamCadData &cancadData, DataStruct* data);
};

//-----------------------------------------------------------

class CAcculogicBoard
{
private:
   CCEtoODBDoc &m_doc;
   BlockStruct *m_pcbBlock;

public:
   CAcculogicBoard(CCEtoODBDoc &doc, BlockStruct *pcbblock);
   DataStruct *GetBoardOutline();
   CExtent getBoardOutlineExtents();
};

//-----------------------------------------------------------

class CAcculogicICTWriter : public CObject
{
private:
	CCEtoODBDoc &m_doc;
	FileStruct &m_file;
	FILE *m_logFile;
   CString m_localLogFilename;
   CTypedPtrArrayContainer<CAcculogicComponent*> m_componentList;
   CTypedPtrArrayContainer<CAcculogicPin*> m_pinList;
   CTypedPtrArrayContainer<CAcculogicProbe*> m_probeList;
   CMapStringToInt m_netIdMap;
   //CMapStringToString m_probeMap;
   CTime m_time;
   int m_maxPartID;

   //void FillProbeMap();

   // Prep
   void GatherComponents();
   void GatherPins();
   void SortComponents();
   void GatherFiducials(bool topFlag, DataStruct **fid1, DataStruct **fid2, DataStruct **fid3, DataStruct **fid4);
   void GatherProbes();
   void SortProbes();
   void ValidateProbes();

   // Output
   bool WriteHeader(CStdioFileWriteFormat &outFile);
   bool WriteComponents(CStdioFileWriteFormat &outFile, DeviceTypeTag targetDeviceType);
   bool WriteComponent(CStdioFileWriteFormat &outFile, CAcculogicComponent *kcomp);
   bool WriteComponentPins(CStdioFileWriteFormat &outFile);
   bool WriteProbes(CStdioFileWriteFormat &outFile);
   CString GetFidX(DataStruct *fid);
   CString GetFidY(DataStruct *fid);
   bool WriteFiducials(CStdioFileWriteFormat &outFile);
   //void WriteComponent(CStdioFileWriteFormat &outFile)
   bool WriteFrameScanTests(CStdioFileWriteFormat &outFile);
   bool WriteDeltaScanTests(CStdioFileWriteFormat &outFile);

   double AccUnits(double camcadValue);


public:
	CAcculogicICTWriter(CCEtoODBDoc &doc, FileStruct &file, CString outputFilename);
   ~CAcculogicICTWriter();

   bool WriteFile(CString filename);
   void WriteUnsupportedToLog();
   

};

#endif
