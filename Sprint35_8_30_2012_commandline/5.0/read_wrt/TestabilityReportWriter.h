// $Header: /CAMCAD/5.0/read_wrt/TestabilityReportWritert.h 1     5/22/07 2:47a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2007. All Rights Reserved.
*/

#pragma once

#ifndef __TESTABILITY_REPORT_WRITER_H_INCLUDED__
#define __TESTABILITY_REPORT_WRITER_H_INCLUDED__

#include "ccdoc.h"
#include "DeviceType.h"

#define MAGIC_NUMBER_E 2.71828182845904523536

class CTRNetMap;


enum ProbeStatusTag
{
   ProbeStatusUnknown,
   ProbeStatusFullyProbed,
   ProbeStatusPartiallyProbed,
   ProbeStatusNotProbed
};

enum NetTypeTag
{
   NetTypeGround,
   NetTypePower,
   NetTypeSignal
};

enum MergedStatusTag
{
   MergedStatusUnknown,
   MergedStatusPrimary,
   MergedStatusSolitary,
   MergedStatusIgnored
};

//---------------------------------------------------------------------

class CTRProbeStatus
{
private:
   ProbeStatusTag m_tag;

public:
   CTRProbeStatus();
   CTRProbeStatus(ProbeStatusTag tag)  { m_tag = tag; }

   ProbeStatusTag Get() { return m_tag; }
   void Set(ProbeStatusTag tag)  { m_tag = tag; }

   CString GetStr();
};

//---------------------------------------------------------------------

class CTRMergedStatus
{
private:
   MergedStatusTag m_tag;

public:
   CTRMergedStatus();
   CTRMergedStatus(MergedStatusTag tag)  { m_tag = tag; }

   MergedStatusTag Get() { return m_tag; }

   void Set(MergedStatusTag tag)  { m_tag = tag; }
   void Set(CString mstat);

   CString GetStr();
};

//---------------------------------------------------------------------

class CTRSmartRefdes : public CObject
{
private:
   CString m_prefix;
   int m_number;

public:
	CTRSmartRefdes(const CString& wholeRefdes);

	CString GetPrefix()  { return m_prefix; }
	int GetNumber()      { return m_number; }

	int CompareNoCase(CTRSmartRefdes &otherrefdes);
   int CompareNoCase(CTRSmartRefdes *otherrefdes) { return otherrefdes != NULL ? CompareNoCase(*otherrefdes) : -1; }
};

class CTRSmartRefdesList : public CMapSortedStringToOb<CTRSmartRefdes>
{
private:
public:
   void AddRefname(CString refname);
   CString GetCompressedRefdes();  // e.g. given C1, C2, C3, C10, C20, C21, C22, R1, R2, R3 return C1-C3, C10, C20-C22, R1-R3
   static int AscendingRefnameSortFunc(const void *a, const void *b);
};

//---------------------------------------------------------------------

class CTRSection
{
private:
   CString m_name;      // internal document link
   CString m_title;     // what the user sees
   bool m_visible;      // write section if visible, don't if not

public:
   CTRSection(CString name, CString title, bool visible)      { m_name = name; m_title = title; m_visible = visible; }
   CString GetName()    { return m_name;  }
   CString GetTitle()   { return m_title; }
   bool IsVisible()     { return m_visible; }
};


class CTRSectionArray : public CTypedArrayContainer<CPtrArray, CTRSection*>
{
public:
   void AddSection(CString name, CString title, bool visible) { this->Add( new CTRSection(name, title, visible) ); }
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

// Attribure used for save/load of testability report settings in ccz file
#define ATT_TESTABILITY_REPORT_SETTINGS  "TESTABILITY_REPORT_SETTINGS"


class CTRSettings
{
private:
   int m_dpmo;
   double m_maxHeightTop;
   double m_maxHeightBot;   
   PageUnitsTag m_topHeightUnits;
   PageUnitsTag m_botHeightUnits;

   int m_maxProbesPerSquare;
   double m_areaSideLength;
   PageUnitsTag m_sideLengthUnits;

   bool m_writeBoardSection;
   bool m_writeTopIssuesSection;
   bool m_writeTestabilityDetailSection;
   bool m_writeNetDetailSection;
   bool m_writeProbeDetailSection;
   bool m_writeHeightDetailSection;
   bool m_writeKelvinSection;
   bool m_writePowerInjectionSection;
   bool m_writeParallelAnalysisSection;
   bool m_writePartsSection;

public:
   CTRSettings();

   int GetDPMO()                       { return m_dpmo; }

   double GetMaxHeightTop()            { return m_maxHeightTop; }
   double GetMaxHeightTop(PageUnitsTag scaleToUnits) { return m_maxHeightTop * Units_Factor(m_topHeightUnits, scaleToUnits); }
   double GetMaxHeightBot()            { return m_maxHeightBot; }
   double GetMaxHeightBot(PageUnitsTag scaleToUnits) { return m_maxHeightBot * Units_Factor(m_botHeightUnits, scaleToUnits); }

   int GetMaxProbesPerSqr()            { return m_maxProbesPerSquare; }
   double GetAreaSideLength()          { return m_areaSideLength; }
   double GetAreaSideLength(PageUnitsTag scaleToUnits) { return m_areaSideLength * Units_Factor(m_sideLengthUnits, scaleToUnits); }

   bool GetWriteBoard()                { return m_writeBoardSection; }
   bool GetWriteTopIssues()            { return m_writeTopIssuesSection; }
   bool GetWriteTestabilityDetail()    { return m_writeTestabilityDetailSection; }
   bool GetWriteNetDetail()            { return m_writeNetDetailSection; }
   bool GetWriteProbeDetail()          { return m_writeProbeDetailSection; }
   bool GetWriteHeightDetail()         { return m_writeHeightDetailSection; }
   bool GetWriteKelvin()               { return m_writeKelvinSection; }
   bool GetWritePowerInjection()       { return m_writePowerInjectionSection; }
   bool GetWriteParallelAnalysis()     { return m_writeParallelAnalysisSection; }
   bool GetWriteParts()                { return m_writePartsSection; }


   void SetDPMO(int dpmo)                       { m_dpmo = dpmo; }
   
   void SetMaxHeightTop(double value, PageUnitsTag unitsTag)   { m_maxHeightTop = value; m_topHeightUnits = unitsTag; }
   void SetMaxHeightBot(double value, PageUnitsTag unitsTag)   { m_maxHeightBot = value; m_botHeightUnits = unitsTag; }

   void SetMaxProbesPerSqr(int maxprobes)       { m_maxProbesPerSquare = maxprobes; }
   void SetSideLength(double value, PageUnitsTag unitsTag)   { m_areaSideLength = value; m_sideLengthUnits = unitsTag; }

   void SetWriteBoard(bool flag)                { m_writeBoardSection = flag; }
   void SetWriteTopIssues(bool flag)            { m_writeTopIssuesSection = flag; }
   void SetWriteTestabilityDetail(bool flag)    { m_writeTestabilityDetailSection = flag; }
   void SetWriteNetDetail(bool flag)            { m_writeNetDetailSection = flag; }
   void SetWriteProbeDetail(bool flag)          { m_writeProbeDetailSection = flag; }
   void SetWriteHeightDetail(bool flag)         { m_writeHeightDetailSection = flag; }
   void SetWriteKelvin(bool flag)               { m_writeKelvinSection = flag; }
   void SetWritePowerInjection(bool flag)       { m_writePowerInjectionSection = flag; }
   void SetWriteParallelAnalysis(bool flag)     { m_writeParallelAnalysisSection = flag; }
   void SetWriteParts(bool flag)                { m_writePartsSection = flag; }


   void LoadStandardSettingsFile();
   void LoadFile(const CString& filePath);
   void ApplyAttributes(CCEtoODBDoc *doc);
   void SaveAttributes(CCEtoODBDoc *doc);

};

//---------------------------------------------------------------------

class CTRSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CTRSettingsDlg)

private:
   CTRSettings &m_settings;

public:
	CTRSettingsDlg(CTRSettings &settings, PageUnitsTag currentPageUnits, CWnd* pParent = NULL);   // standard constructor
	virtual ~CTRSettingsDlg();

// Dialog Data
	enum { IDD = IDD_TESTABILITY_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

private:
   PageUnitsTag m_cczPageUnits;

   BOOL m_writeBoardStats;
   BOOL m_writeTopIssues;
   BOOL m_writeTestability;
   BOOL m_writeNetDetails;
   BOOL m_writeProbeDetails;
   BOOL m_writeHeightDetails;
   BOOL m_writeKelvinDetails;
   BOOL m_writePowerDetails;
   BOOL m_writeParallelAnalysis;
   BOOL m_writeParts;

   CString m_baseDPMOStr;
   CString m_topHeightStr;
   CString m_botHeightStr;
   CString m_maxProbesStr;
   CString m_sideLengthStr;

   
public:
   afx_msg void OnBnClickedOk();
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CTRCompPin : public CObject
{
private:
   CString m_refname;
   CompPinStruct *m_comppin;
   NetStruct *m_net;
   bool m_noconnect; // from pin mapping

public:
   CTRCompPin(CCamCadData &ccdata, CString refname, CompPinStruct *comppin, NetStruct *net);

   CString GetRefname()          { return m_refname; }
   CompPinStruct *GetCompPin()   { return m_comppin; }
   NetStruct *GetNet()           { return m_net; }
   bool IsNoConnect()            { return m_noconnect; }

   int GetPlacedTestProbeCount(CTRNetMap &trnetMap);

   CString GetNetName()          { return m_net != NULL ? m_net->getNetName() : "Not Connected"; }
};

//---------------------------------------------------------------------

class CTRCompPinMap : public CMapSortedStringToOb<CTRCompPin>
{
public:
   CTRCompPin *GetPin(int n);
   CString GetPinPairCombinedNetName(int n1, int n2);

   bool HasUnprobedPin(CTRNetMap &trnetMap);
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CTRComponent : public CObject
{
private:
   DataStruct *m_component;
   CString m_value;
   double m_plustolerance;
   double m_minustolerance;
   bool m_kelvin;
   bool m_loaded;
   double m_height;
   int m_dpmoBase;      // base DPMO aka per-unit DPMO, like per-pin, from component attribute
   int m_dpmoProcess;   // for entire component, indpendent of testing
   int m_dpmoFinal;     // for component after testing is accounted for
   CString m_partnumber;
   CString m_subclass;
   CString m_description;
   DeviceTypeTag m_devicetype;
   CTRProbeStatus m_kelvinstatus;
   CString m_mergedValue;
   CTRMergedStatus m_mergedStatus;

   CTRCompPinMap m_comppinmap;

public:
   CTRComponent(CCamCadData &ccdata, DataStruct *compdata, CTRSettings &settings);

   DataStruct *GetComponentData()      { return m_component; }
   CString GetValue()                  { return m_value; }
   double GetValueDouble()             { return ComponentValues(m_value).getScientificValue(); }

   double GetPlusTolerance()           { return m_plustolerance; }
   double GetMinusTolerance()          { return m_minustolerance; }
   CString GetPlusToleranceStr()       { ComponentValues cv(m_plustolerance,  valueUnitPercent); return m_plustolerance  > 0. ? cv.GetPrintableString() : "-"; }
   CString GetMinusToleranceStr()      { ComponentValues cv(m_minustolerance, valueUnitPercent); return m_minustolerance > 0. ? cv.GetPrintableString() : "-"; }

   MergedStatusTag GetMergedStatus()   { return m_mergedStatus.Get(); }
   CString GetMergedStatusStr()        { return m_mergedStatus.GetStr(); }
   CString GetMergedValue()            { return m_mergedValue; }

   int GetBaseDPMO()                   { return m_dpmoBase; }
   int GetProcessDPMO()                { return m_dpmoProcess; }
   int GetFinalDPMO()                  { return m_dpmoFinal; }
   int GetTheoreticalBestDPMO();

   void SetProcessDPMO(int dpmo)       { m_dpmoProcess = dpmo; }
   void SetFinalDPMO(int dpmo)         { m_dpmoFinal = dpmo; }

   bool GetKelvin()                    { return m_kelvin; }
   void SetKelvin(bool flag)           { m_kelvin = flag; }

   bool GetLoaded()                    { return m_loaded; }

   double GetHeight()                  { return m_height; }

   CString GetPartNumber()             { return m_partnumber; }

   CString GetSubclass()               { return m_subclass; }

   CString GetDescription()            { return m_description; }

   DeviceTypeTag GetDeviceType()       { return m_devicetype; }

   CTRCompPinMap& GetCompPinMap()      { return m_comppinmap; }

   int GetPinCount();   // Does not include no-connect pins

   CString GetTestabilityReason(CTRNetMap &trnetMap);
   bool HasUnprobedPin(CTRNetMap &trnetMap)  { return m_comppinmap.HasUnprobedPin(trnetMap); }

   ProbeStatusTag GetProbeStatus()           { return m_kelvinstatus.Get(); }  //*rcf Originally just used for Kelvin, now used for all
   CString GetProbeStatusStr()               { return m_kelvinstatus.GetStr(); }  //*rcf get around to adjusting kelvin out of name some time
   void SetProbeStatus(ProbeStatusTag tag)   { m_kelvinstatus.Set(tag); }

   bool IsDie()                              {return (m_component)?m_component->isInsertType(insertTypeDie):false;}

};

//---------------------------------------------------------------------

class CTRComponentMap : public CMapSortedStringToOb<CTRComponent>
{
private:
   int m_CompCount;
   int m_DieCount;

public:
   void CollectComponents(CCamCadData &ccdata, FileStruct *pcbFile, CTRSettings &settings);

   int GetTotalProcessDPMO();
   int GetTotalFinalDPMO();
   int GetTotalTheoreticalBestDPMO();
   int GetDieCount() const          {return m_DieCount;}
   int GetComponentCount() const    {return m_CompCount;}

   static int AscendingValueSortFunc(const void *a, const void *b);
   static int AscendingHeightSortFunc(const void *a, const void *b);
   static int DescendingFinalDPMOSortFunc(const void *a, const void *b);
   static int DescendingHeightSortFunc(const void *a, const void *b);
   static int KelvinSectionSortFunc(const void *a, const void *b);
   static int MergedStatusSortFunc(const void *a, const void *b);
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CTRPart : public CObject
{
private:
   CString m_partNumber;

   CTypedPtrArray<CObArray, CTRComponent*> m_componentAry;

public:
   CTRPart(CString partnumber);

   CString GetPartNumber()          { return m_partNumber; }

   void AddComponent(CTRComponent *trcomp);

   int GetCompCount()               { return m_componentAry.GetCount(); }
   CTRComponent *GetCompAt(int i)   { if (i >= 0 && i < m_componentAry.GetCount()) return m_componentAry.GetAt(i); return NULL; }
};

//---------------------------------------------------------------------

class CTRPartMap : public CMapSortedStringToOb<CTRPart>
{
private:
   CTRPart *AddPart(CString partnumber);
   CTRPart *AddPart(CTRComponent *trcomp);

public:
   void CollectParts(CCamCadData &ccdata, CTRComponentMap &compMap);

   static int AscendingPartNumberSortFunc(const void *a, const void *b);

};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CTRNet : public CObject
{
private:
   NetStruct *m_ccnet;
   NetTypeTag m_netType;
   CAANetAccess *m_netaccess;
   int m_testResourcesRequired;
   int m_powerResourcesRequired;
   bool m_kelvinPlayer;
   int m_placedTestProbeCount;
   int m_placedPowerProbeCount;
   int m_unplacedProbeCount;  // test and power combined
   int m_potentialDPMO; // calculated and cached here

public:
   CTRNet(CCamCadData &ccdata, NetStruct *net, CAccessAnalysisSolution *aasol);

   NetStruct *GetCamCadNet()        { return m_ccnet; }

   CAANetAccess *GetNetAccess()     { return m_netaccess; }

   NetTypeTag GetNetType()          { return m_netType; }
   CString GetNetTypeStr();
   //int GetTestResourcesRequired()   { return m_testResourcesRequired; }
   int GetTestResourcesRequired()   { return GetKelvin() ? max(2, m_testResourcesRequired) : m_testResourcesRequired; }
   int GetPowerResourcesRequired()  { return m_powerResourcesRequired; }
   void SetPowerResourcesReq(int p) { m_powerResourcesRequired = p; } //*rcf Look at moving the setting of this into a trnet func

   bool GetKelvin()                 { return m_kelvinPlayer; }
   void SetKelvin(bool flag)        { m_kelvinPlayer = flag; }

   int GetPlacedTestProbeCount()          { return m_placedTestProbeCount; }
   int GetPlacedPowerProbeCount()         { return m_placedPowerProbeCount; }
   void IncrementPlacedTestProbeCount()   { m_placedTestProbeCount++; }
   void IncrementPlacedPowerProbeCount()  { m_placedPowerProbeCount++; }

   int GetUnplacedProbeCount()        { return m_unplacedProbeCount; }
   void IncrementUnplacedProbeCount() { m_unplacedProbeCount++; }

   ProbeStatusTag GetTestProbeStatus();
   CString GetTestProbeStatusStr();

   ProbeStatusTag GetPowerProbeStatus();
   CString GetPowerProbeStatusStr();

   ProbeStatusTag GetCombinedProbeStatus();  // Both of the above considered together
   CString GetCombinedProbeStatusStr();

   int CalculatePotentialDPMO(CTRComponentMap &trCompMap);
   int GetPotentialDPMO()     { return m_potentialDPMO; }

};

//---------------------------------------------------------------------

class CTRNetMap : public CMapSortedStringToOb<CTRNet>
{
private:
   

public:
   void CollectNets(CCamCadData &ccdata, FileStruct *pcbFile, CDFTSolution *dftSolution);

   void AnalyzePotentialDPMO(CTRComponentMap &trCompMap);

   static int AscendingNetNameSortFunc(const void *a, const void *b);
   static int DescendingDPMOSortFunc(const void *a, const void *b);
   static int NetDetailsSortFunc(const void *a, const void *b);
   static int TestProbeStatusSortFunc(const void *a, const void *b);
   static int PowerProbeStatusSortFunc(const void *a, const void *b);
   static int CombinedProbeStatusSortFunc(const void *a, const void *b);
   static int ProbeStatusSortFuncHelper(const void *a, const void *b, int probetype);
};

//---------------------------------------------------------------------
/*
int CTRComponentMap::AscendingValueSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   double aval = itemA->GetValueDouble();
   double bval = itemB->GetValueDouble();

   return (aval < bval);
}
*/

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CTRProbe : public CObject
{
private:
   DataStruct *m_probe;
   bool m_active;          // This is used during processing, it changes along the way
   bool m_densitycenter;   // This too, true = this probe is center of density violation
   bool m_placed;
   int m_probesize;
   int m_targetentity;     // Entity number of target (not access marker) probed if this probe is placed
   CString m_netname;
   CTypedPtrArray<CObArray, CTRProbe*> m_probesInProximity;

public:
   CTRProbe(CCamCadData &ccdata, DataStruct *probe);

   DataStruct *GetCCProbeData()  { return m_probe; }

   CString GetNetName()          { return m_netname; }

   int GetProbeSize()            { return m_probesize; }
   CString GetProbeSizeStr()     { CString buf; if (m_probesize > 0) buf.Format("%d", m_probesize); return buf; } // leave empty if zero or less

   int GetAllNeighborCount()           { return m_probesInProximity.GetCount(); }
   int GetActiveNeighborCount();

   void AddNeighbor(CTRProbe *p)       { m_probesInProximity.Add(p); }
   CTRProbe *GetNeightborAt(int i)     { return m_probesInProximity.GetAt(i); }

   bool IsPlaced()                     { return m_placed; }

   int  GetProbedTargetEntityNum()     { return m_targetentity; }

   bool IsActive()                     { return m_active; }
   void SetActive(bool flag)           { m_active = flag; }

   bool IsDensityCenter()              { return m_densitycenter; }
   void SetDensityCenter(bool flag)    { m_densitycenter = flag; }

   void DeactivateNeighborhood();


};

//---------------------------------------------------------------------

class CTRProbeMap  : public CMapSortedStringToOb<CTRProbe>
{
private:
public:
   void CollectProbes(CCamCadData &ccdata, FileStruct *pcbFile, bool topSide);
   int GetPlacedProbeCount();
   int GetUnplacedProbeCount();

   CTRProbe *GetProbeForTargetEntity(int targetEntityNum);

   static int DescendingDensitySortFunc(const void *a, const void *b);
   static int DescendingProbeSizeSortFunc(const void *a, const void *b);

   // Requries use from within Probe Density Tool, must determine neighborhoods first
   CTRProbe *GetOverCrowdedProbe(int maxNeighbors);
};

//---------------------------------------------------------------------

class CTRProbeDensityTool
{
private:
   CTRSettings &m_settings;
   PageUnitsTag m_currentPageUnits; // page units active ccz

   void CollectNeighbors(CTRProbeMap &probeMap);

public:
   CTRProbeDensityTool(CTRSettings &settings, PageUnitsTag currentPageUnits);

   // Basic work horse, uses probe map in parameter list
   int  DetermineViolations(CTRProbeMap &probeMap);

   // These are self contained, build own probe maps
   //*rcf Was first shot, but needed result in map retained, probably should delete once all done if they don't get used
   int GetViolationsTop   (CCamCadData &ccdata, FileStruct *pcbFile);
   int GetViolationsBottom(CCamCadData &ccdata, FileStruct *pcbFile);
};

//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CTestabilityReportWriter
{
public:
	CTestabilityReportWriter(CCEtoODBDoc& doc, CString outfilename);
	~CTestabilityReportWriter();

   CFormatStdioFile m_file;

   CTRSettings m_settings;

   int m_decimals;
   PageUnitsTag m_pageUnits;

   CCEtoODBDoc &m_doc;
   FileStruct *m_visibleFile;
   CTestPlan *m_testPlan;

   CTRComponentMap m_componentMap;
   CTRNetMap m_netMap;
   CTRPartMap m_partMap;
   CTRProbeMap m_topProbeMap;
   CTRProbeMap m_botProbeMap;

   bool m_boardOutlineFound;
   double m_boardSizeX;
   double m_boardSizeY;
   int m_singlePinNetCount;
   int m_multiPinNetCount;
   int m_noPinNetCount;
   int m_fullyProbedNetCount;
   int m_partiallyProbedNetCount;
   int m_notProbedNetCount;
   int m_fittedCompCount;
   int m_notFittedCompCount;
   int m_fullyProbedCompCount;
   int m_partiallyProbedCompCount;
   int m_notProbedCompCount;
   int m_electLayerCount;
   int m_topHeightViolationCount;
   int m_botHeightViolationCount;
   int m_topDensityViolationCount;
   int m_botDensityViolationCount;

   CTRSectionArray m_sectionAry;

   //CString NonEmptyStr(CString str) { return str.IsEmpty() ? "-" : str; }
   CString NonEmptyStr(CString str) { return str.IsEmpty() ? "&nbsp" : str; }
   CString NonEmptyStr()            { return NonEmptyStr(""); }
   CString PercentStr(int numerator, int denominator);

   CString AccessSurfaceStr(ETestSurface surf)  { switch (surf) { case testSurfaceTop: return "Top"; case testSurfaceBottom: return "Bottom"; case testSurfaceBoth: return "Both"; } return "Unknown"; }

   // Unit DPMO, e.g. per pin
   int GetDPMO(CTRComponent *trcomp)      { return trcomp->GetBaseDPMO() > 0 ? trcomp->GetBaseDPMO() : m_settings.GetDPMO(); }
   //CString GetDPMOStr(CTRComponent *trcomp)   { CString buf; int dpmo = GetDPMO(trcomp); if (dpmo > 0) buf.Format("%d", dpmo); return buf; }

   // Collective DPMO, e.g. for entire component
   int GetDPMOTotal(CTRComponent *trcomp) { return GetDPMO(trcomp) * (1 + trcomp->GetCompPinMap().GetCount()); }
   //CString GetDPMOTotalStr(CTRComponent *trcomp)   { CString buf; int dpmo = GetDPMOTotal(trcomp); if (dpmo > 0) buf.Format("%d", dpmo); return buf; }

   int GetProbedPinCount(CTRComponent *trcomp);

   //    ( "e" to-the-power (minus)(DPMO / one million) ) x 100 to get percent
   double GetYield(int dpmo) {   return  ( pow ( MAGIC_NUMBER_E ,  -(dpmo / 1.0E6) ) * 100.0 );  }

   void WriteTableOfContents();
   void WriteSections();

   void WriteBoardSection(CString name, CString title);
   void WriteKelvinSection(CString name, CString title);
   void WriteNetSection(CString name, CString title);
   void WriteProbeSection(CString name, CString title);
   void WriteHeightSection(CString name, CString title);
   void WritePowerSection(CString name, CString title);
   void WriteParallelAnalysisSection(CString name, CString title);
   void WritePartSection(CString name, CString title);
   void WriteIssuesSection(CString name, CString title);
   void WriteTestabilitySection(CString name, CString title);

   void WriteProbeDensityViolations();
   void WritePlacedProbes();

   void WriteBogusSection(CString name, CString title); // Placeholder

   CTRSettings& GetSettings() { return m_settings; }

   void Analyze();
   void CalculatePowerInjectionRequirements();

public:
	bool WriteFiles(CString filepath);
};


#endif


