// $Header: /CAMCAD/4.5/read_wrt/AeroflexNailIn.h 2     9/29/06 4:18p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/

#if !defined(__Aeroflex_Nail_Read_h__)
#define __Aeroflex_Nail_Read_h__

#pragma once

#include "CcDoc.h"

// Sample fixture file had these four dashes as placeholders in fields with no data
#define AERO_FIELD_SKIPPED "----"

class CAeroflexNailRecord : public CObject
{
private:
   // Fields extracted as-is from data
   CString m_xLoc;
   CString m_yLoc;
   CString m_refname; // Pin Number in Aeroflex terms
   CString m_pcbSide;
   CString m_probeType; // Embodies probe size
   CString m_testerInterfaceName; // No idea what this is about
   CString m_wireSize;
   CString m_wireColor;
   CString m_targetRefname; // Where access marker should be
   CString m_netname;

   // Fields from analysis and processing
   CString m_targetCompRefname;  // These two only valid if m_pinRef turns out to be a CompPin
   CString m_targetCompPinname;  //   "

public:
   CAeroflexNailRecord(CString wholeEnchalada)     { this->Set(wholeEnchalada); }

   CString GetXLocStr()                { return m_xLoc; }
   CString GetYLocStr()                { return m_yLoc; }
   CString GetRefname()                { return m_refname; }
   CString GetPcbSide()                { return m_pcbSide; }
   CString GetProbeType()              { return m_probeType; }
   CString GetTesterInterfaceName()    { return m_testerInterfaceName; }
   CString GetWireSize()               { return m_wireSize; }
   CString GetWireColor()              { return m_wireColor; }
   CString GetTargetRefname()          { return m_targetRefname; }
   CString GetNetName()                { return m_netname; }

   CString GetTargetCompRefname()      { return m_targetCompRefname; }
   CString GetTargetCompPinname()      { return m_targetCompPinname; }

   double GetXLoc()              { return atof(m_xLoc); }
   double GetYLoc()              { return atof(m_yLoc); }

   ETestSurface GetSurface()     { return m_pcbSide.Find("BOT") > -1 ? testSurfaceBottom : testSurfaceTop; }

   // This applies an interpretation, look for 100 or 75, default is 50.
   int GetProbeSizeMils()        { if (m_probeType.Find("100") > -1) return 100; else if (m_probeType.Find("75") > -1) return 75; return 50; }

   void Reset(); // clear all values
   void Set(CString recordStr); // set fields from file record string
};

//----------------------------------------------------------------------------

class CAeroflexNailRecordList : public CTypedObArrayContainer<CAeroflexNailRecord*>
{
public:
   ////CSubclassElementList& operator=(const CSubclassElementList &other);
};

//----------------------------------------------------------------------------

class CAeroflexAccessMarker : public CObject
{
private:
   DataStruct *m_accessMarker;
   DataStruct *m_targetData;   // Set if access target is data
   CompPinStruct *m_compPin;   // Set if access target is comppin

   CString m_netName;

   CStringArray m_oldProbeRefnames;  // Probes that were on this access marker before import

   CArray<DataStruct*> m_newProbes; // Set during processing, note that more than one probe may be on same target

public:
   CAeroflexAccessMarker(DataStruct *accessMark, DataStruct *targetData)
   { m_accessMarker = accessMark; m_targetData = targetData; m_compPin = NULL; }

   CAeroflexAccessMarker(DataStruct *accessMark, CompPinStruct *targetCompPin)
   { m_accessMarker = accessMark; m_targetData = NULL; m_compPin = targetCompPin; }

   ETestSurface GetAccessMarkSurface() 
   { return ( (m_accessMarker != NULL && m_accessMarker->getInsert()->getPlacedBottom()) ? testSurfaceBottom : testSurfaceTop ); }

   DataStruct *GetCamcadAccessMarker()    { return m_accessMarker; }
   CompPinStruct *GetCompPin()            { return m_compPin;      }
   DataStruct *GetTargetData()            { return m_targetData; }

   CStringArray &GetOldProbeNames()       { return m_oldProbeRefnames; }

   DataStruct *GetNewProbeData(int indx)      { if (indx >= 0 && indx < m_newProbes.GetCount()) return m_newProbes.GetAt(indx); return NULL;  }
   void AddNewProbeData(DataStruct *probe)    { m_newProbes.Add(probe); }
   int GetNewProbeCount()                     { return m_newProbes.GetCount(); }

   CString GetNetName(CCEtoODBDoc *doc = NULL);

};

//----------------------------------------------------------------------------

class CAccessMarkerMap : public CMapSortedStringToOb<CAeroflexAccessMarker>
{
private:
   void CollectProbes(CCEtoODBDoc *doc, FileStruct *pcbFile);

public:
   void CollectAccessMarkers(CCEtoODBDoc *doc, FileStruct *pcbFile);

   static int AscendingNetNameSortFunc(const void *a, const void *b);

   CAeroflexAccessMarker *FindAccessDataAtLocation(double pageunitsX, double pageunitsY, double tolerance);
};

//----------------------------------------------------------------------------

class CAeroflexNailReader
{
private:
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbfile;
   CString m_inputFilename;
   CFormatStdioFile m_infile;
   bool m_infileIsOpen;
   WORD m_preassigned_chan_kw;
   CStdioFileWriteFormat m_logfile;
   bool m_logIsOpen;

   int m_decimals; // For resolution in writing locations to log file

   PageUnitsTag m_fixtureFileUnits;
   double m_scaleFactor; // From fixture file units to page units
   double m_offsetX; // In fixture file units
   double m_offsetY; // In fixture file units

   CAeroflexNailRecordList m_placedProbeList;
   CAeroflexNailRecordList m_pilotHoleList;

   CAccessMarkerMap m_accessMarkerMap;
   
   double ConvertX(double x)     { return ((x - m_offsetX) * m_scaleFactor); }
   double ConvertY(double y)     { return ((y - m_offsetY) * m_scaleFactor); }

   bool Parse();

   void WriteFinalReport();

   bool ValidateTargetName(CAeroflexAccessMarker *aeroAccessMark, CAeroflexNailRecord *nail);
   bool ValidateNetName(CAeroflexAccessMarker *aeroAccessMark, CAeroflexNailRecord *nail);
   bool ValidateSurface(CAeroflexAccessMarker *aeroAccessMark, CAeroflexNailRecord *nail);

   void DeleteTestProbes();

public:
   CAeroflexNailReader(CString infilename, CString logfilename, CCEtoODBDoc *doc, FileStruct *pcbfile);
   ~CAeroflexNailReader();

   bool Process();

};

///
/// EXAMPLE FILE
///
/*
/ CVT_VERSION 2.0
/ Filename.............: 44830220.cvt
/ Date Created.........: Apr 28 2008 (15:08)
/ Created By...........: CAPG - mkfix - V1.3
/ Units................: METRIC mm
/ Fixture Type.........: Single UUT
/ Safety Cover.........: Required
/
/ Note: QCn = FA15 card - Connector PLC
/       CCn = FD05 card - Connector PLA1
/
/ ---- UUT Extents and locations ----
/
190.02  302.02  T8     / Min Extent
506.98  426.98  T8     / Max Extent
/
/ ---- i/f to i/f wiring ----
/
/ WIRE Ab-Aa21    -> Ab-Aa22    26AWG  GREEN 
/ WIRE Ab-Aa29    -> uSW-NO     26AWG  GREEN 
/ WIRE Ab-Aa30    -> uSW-COM    26AWG  GREEN 
/ WIRE Ab-Ac29    -> uSW-NC     26AWG  GREEN 
/ WIRE l-Ab1      -> m-Ab1      26AWG  GREEN 
/ WIRE l-Ab1      -> Ac-Aa12    26AWG  GREEN 

...
/
/
/ ---- Test probes and power pins ----
/
469.72  351.42  N0     BOT  100THOU N-Ac32     30AWG  ----   TEST.PAD   "A11"
395.05  325.59  N1     BOT  100THOU N-Ac31     30AWG  ----   PLB1.9     "A1"
437.24  416.60  N2     BOT  100THOU N-Ac30     30AWG  ----   PLC1.86    "SCL"
198.91  412.20  N3     BOT  100THOU N-Ac29     30AWG  ----   PLL1.2     "RXDATA2"
263.60  325.59  N4     BOT  100THOU N-Ac28     30AWG  ----   TP16.1     "RW_L_"
196.37  368.31  N5     BOT  100THOU N-Ac27     30AWG  ----   PLK1.9     "RI1_L_"
196.37  413.57  N6     BOT  100THOU N-Ac26     30AWG  ----   PLL1.6     "DSR2_L_"
198.91  414.97  N7     BOT  100THOU N-Ac25     30AWG  ----   PLL1.1     "DCD2_L_"
225.25  337.83  N8     BOT  100THOU N-Ac16     30AWG  ----   TEST.PAD   "XSIG010394"
274.40  374.48  N9     BOT  100THOU N-Ac15     30AWG  ----   TEST.PAD   "XSIG010528"

...

/
/ ---- Pilot holes ----
/
361.50  221.49  T11    BOT  100THOU NO_WIRE    ----   ----   ALIGN1.1   "NC__352"
150.33  238.64  T11    BOT   75THOU NO_WIRE    ----   ----   AT420.1    "XSIG020362"
177.00  249.23  T11    BOT   75THOU NO_WIRE    ----   ----   AT421.1    "XSIG020363"
149.06  223.40  T11    BOT   50THOU NO_WIRE    ----   ----   AT422.1    "XSIG020366"
180.17  250.07  T11    BOT  100THOU NO_WIRE    ----   ----   AT424.1    "XSIG020453"
182.71  250.07  T11    BOT  100THOU NO_WIRE    ----   ----   AT425.1    "XSIG020454"
185.25  250.07  T11    BOT  100THOU NO_WIRE    ----   ----   AT426.1    "XSIG020455"


*/

#endif

