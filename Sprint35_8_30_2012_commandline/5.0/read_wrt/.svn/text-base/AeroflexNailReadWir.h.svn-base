// $Header: /CAMCAD/4.5/read_wrt/AeroflexNailWir.h 2     9/29/06 4:18p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/

#if !defined(__Aeroflex_Nail_Read_Wir_h__)
#define __Aeroflex_Nail_Read_Wir_h__

#pragma once

#include "CcDoc.h"


// Sample fixture file had these four dashes as placeholders in fields with no data
#define AERO_FIELD_SKIPPED "----"

//----------------------------------------------------------------------------
// CAeroflexTestProbe
//
class CAeroflexTestProbe
{
private:
   DataStruct* m_testprobe;
   CString m_pinRefname;
   CString m_targetRefname;
   bool  m_assignedFlag;
public:
   CAeroflexTestProbe(DataStruct *data, CString refname);
   
   DataStruct* getTestProbe(){return m_testprobe;}
   CString &getPinRefname(){return m_pinRefname;}
   CString &getTargetRefname(){return m_targetRefname;}
   
   void setPinRefname(CString val){m_pinRefname = val;}
   bool isAssigned(){return m_assignedFlag;}
   void setAssignedFlag(bool val){m_assignedFlag = val;}
};

//----------------------------------------------------------------------------
// CAeroflexNailRecord
//
enum WirDataSection
{
   WDS_UNKNOWN = -1,
   WDS_TESTPROBE = 0,
};

class CAeroflexNailWirRecord : public CObject
{
private:
   WirDataSection m_dataSection;
   // Fields extracted as-is from data
   CString m_wireIndex;
   CString m_testerInterfaceName; // No idea what this is about
   CString m_pinRefname; // Pin Number in Aeroflex terms
   CString m_targetRefname; // TestProbe Refname should be placed
   CString m_netname;

   CAeroflexTestProbe *m_afxTestProbe;
public:
   CAeroflexNailWirRecord(CString wholeEnchalada)     { this->Set(wholeEnchalada); }

   CString GetWireIndex()              { return m_wireIndex; }
   CString GetTesterInterfaceName()    { return m_testerInterfaceName; }
   CString GetPinRefname()                { return m_pinRefname; }
   CString GetTargetRefname()          { return m_targetRefname; }
   CString GetNetName()                { return m_netname; }
    
   void SetAfxTestprobe(CAeroflexTestProbe *val) {m_afxTestProbe = val;}
   CAeroflexTestProbe *GetAfxTestprobe(){return m_afxTestProbe;}


   void Reset(); // clear all values
   void Set(CString recordStr); // set fields from file record string
};

//----------------------------------------------------------------------------
// CAeroflexNailWirRecordList
//
class CAeroflexNailWirRecordList : public CTypedObArrayContainer<CAeroflexNailWirRecord*>
{
public:
   ~CAeroflexNailWirRecordList();
};

//----------------------------------------------------------------------------
// CAeroflexNailWirRecordMap
//
class CAeroflexNailWirRecordMap : public CTypedPtrMap<CMapStringToPtr,CString,CAeroflexNailWirRecordList*>
{
private:
   int m_maxWireIndex;
public:
   CAeroflexNailWirRecordMap();
   ~CAeroflexNailWirRecordMap();

   int getMaxWireIndex(){return m_maxWireIndex;}
   void setMaxWireIndex(int val){m_maxWireIndex = val;}
};


//----------------------------------------------------------------------------
// CAeroflexTestProbeList
//
class CAeroflexTestProbeList  : public CTypedPtrList<CPtrList, CAeroflexTestProbe*>
{
public:
   ~CAeroflexTestProbeList();
};

//----------------------------------------------------------------------------
// CCBNetNameList
//
class CCBNetList : public CTypedPtrList<CPtrList, NetStruct*>
{
private:
   CAeroflexTestProbeList m_TestProbeList;

public:
   CAeroflexTestProbeList& getTestProbeList(){return m_TestProbeList;}
};
//----------------------------------------------------------------------------
// CCBNetNameListMap
//
class CCBNetListMap : public CTypedPtrMap<CMapStringToPtr,CString,CCBNetList*>
{
public:
   ~CCBNetListMap();
};

//----------------------------------------------------------------------------
// CAeroflexNailWirReader
//
class CAeroflexNailWirReader
{
private:
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbfile;
   CString m_inputFilename;
   CFormatStdioFile m_infile;
   bool m_infileIsOpen;
   CStdioFileWriteFormat m_logfile;
   bool m_logIsOpen;
   
 
   CAeroflexNailWirRecordMap m_placedProbeMap;
   CCBNetListMap m_cbNetlistMap;

   bool Parse();
   void RegisterAeroflexKeyWords();
   void SortplacedProbeMap(CAeroflexNailWirRecordList &recordlist);

   bool BuildCBNeListMap();
   bool LinkNetToCBNetListMap();
   bool LinkTestProbeToCBNetListMap();

   CCBNetList *getCBNetListByNet(NetStruct *net);
   CAeroflexTestProbe* FindAeroflexTestProbe(CAeroflexNailWirRecord *nail,CCBNetList *cbnetlist);
   CAeroflexTestProbe* FindUncertainAeroflexTestProbe(CAeroflexNailWirRecord *nail,CCBNetList *cbnetlist);

   void UpdateTestProbesByAeroflexNailWir();
   bool UpdateTestProbesAttributes(CAeroflexNailWirRecord *nail, CAeroflexTestProbe *testprobe);

   void WriteUpdatedProbes();
   void WriteNonUpdatedProbes(int placed);
   void WriteFinalReport();
public:
   CAeroflexNailWirReader(CString infilename, CString logfilename, CCEtoODBDoc *doc, FileStruct *pcbfile);
   ~CAeroflexNailWirReader();

   bool Process();

};



///
/// EXAMPLE FILE
///
//(Analogue_Pins
//
//(ICR192 board 1 Pin allocation 
//
//Wire  ATE-Connector  Biscuit   Dev.Pin        ICR.Pin Pre-alloc? Signal
//-------------------------------------------------------------------------------
//
//1    (                                                                        )
//2    (CON CN3_C2           DEV  CL118.1        PIN  1.2      yes C            )
//3    (CON CN3_C3           DEV  C2365.2        PIN  1.3      no  NET0012496   )
//4    (CON CN3_C4           DEV  CL277.1        PIN  1.4      no  NET0013113   )
//5    (CON CN3_C5           DEV  CL304.1        PIN  1.5      no  NET0013126   )
//6    (CON CN3_C6           DEV  DS12.34        PIN  1.6      no  AL_VD_2      )
//7    (CON CN3_C7           DEV  CL107.1        PIN  1.7      no  NET0013412   )
//8    (CON CN3_C8           DEV  CL243.1        PIN  1.8      no  NET0013050   )
//)

#endif /*__Aeroflex_Nail_Read_Wir_h__*/

