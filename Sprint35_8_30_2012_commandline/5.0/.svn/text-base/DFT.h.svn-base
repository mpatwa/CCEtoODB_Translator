// $Header: /CAMCAD/5.0/DFT.h 154   6/21/07 8:25p Kurt Van Ness $

#if !defined(__Dft_h__)
#define __Dft_h__

/////#if _MSC_VER > 1000
#pragma once
//////#endif // _MSC_VER > 1000

#include "find.h"
#include "qfelib.h"
#include "region.h"
#include "xmldomwrapper.h"
#include "DFTShared.h"
#include "AccessAnalysis.h"
#include "DcaVia.h"

//#define DebugAccessAnalysisOffset

#define DFT_SOLUTION_XML_FILESTRUCT_NAME   "ImportedDftSln"

/* Fucntion Prototype Section *********************************************************/

void RemoveTestAccessAttributes(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top = true, bool bottom = true);
void DeleteTestAccessPoints(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top = true, bool bottom = true);
void DeleteTestProbes(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top = true, bool bottom = true);

void DeleteAllTestAccessPoints(CCEtoODBDoc *doc, FileStruct *pcbFile);
void DeleteAllProbes(CCEtoODBDoc *doc, FileStruct *pcbFile);

DataStruct *PlaceTestAccessPoint(CCEtoODBDoc *doc, BlockStruct *pcbFileBlock, CAAAccessLocation *accLoc, 
                                 int dataLink, int pageUnits);

DataStruct *PlaceTestAccessPoint(CCEtoODBDoc *doc, BlockStruct *pcbFileBlock, CString refName, double x, double y, 
                                 ETestSurface surface, CString netName, CString targetType, int dataLink,
                                 double exposeMetalDiameter, int pageUnits,int accessPointId=0,int exposedDataId=0);

DataStruct *PlaceTestProbe(CCEtoODBDoc *doc,  BlockStruct *pcbFileBlock, 
                           CString refName, double x, double y, double rot, ETestSurface surface,  ETestResourceType testResourceType,
									CString netName, DataStruct *accessData,
                           CString probeBaseName, double probediamInch, double drillsizeInch);

DataStruct *PlaceTestProbe(int probeNameNominalSizeMils, CCEtoODBDoc *doc,  BlockStruct *pcbFileBlock, 
                           CString refName, double x, double y, double rot, ETestSurface surface, ETestResourceType testResourceType, 
									CString netName, DataStruct *accessData);

int PlaceAccessAndProbe(int probeNameNominalSizeMils, CCEtoODBDoc *doc,  FileStruct *cur_file, double x, double y, double rot, 
								long pinEntityNum, CString refName, CString netName, ETestSurface side);

BlockStruct* CreateTestProbeGeometry(CCEtoODBDoc *doc, const char *name, double probediam, const char *tool, double drillsize, EProbeShape shape = probeShapeDefaultTarget, COLORREF color = -1);
void         CreateTestProbeRefnameAttr(CCEtoODBDoc *doc, DataStruct *probe, CString refname, double probesize, double textsize, bool never_mirror = false); // preferred
void         CreateTestProbeRefnameAttr(CCEtoODBDoc *doc, DataStruct *probe, CString refname, double probesize, bool never_mirror = false); // backward compatibility, where text height is not available to to be defaulted to earlier behavior

CString SwapSpecialCharacters(CString string);     // used for writing xml files

//_____________________________________________________________________________
class DFT_probes
{
public:
   CString  probename;
   unsigned long  entity;
   double   x,y;              // DataStruct positions
   double   xnorm, ynorm;     // zero normalized. this has all stribes etc... normalized.
   int      xstep, ystep;     // stripe number
   int      bottom;
   int      sequencenr;       // updated in sort
   int      used;             // needed in sort. The qsort must sort used from lowest to highest
   BOOL      placed;

   // accessors
   long getEntityNumber()            const { return entity; }
   void setEntityNumber(long entityNumber) { entity = entityNumber; }
};

//_____________________________________________________________________________
class CProbeArray : public CTypedPtrArray<CPtrArray, DFT_probes*>
{
};

//_____________________________________________________________________________
class DFT_ProbeGraphic
{
public:
   double   scale;
   CString  PROBEGRIDLAYER;
   CString  PROBEDRILLTOP;
   CString  PROBEDRILLBOTTOM;
   CString  PROBETOP;
   CString  PROBEBOTTOM;
   CString  PROBENAMETOP;
   CString  PROBENAMEBOTTOM;
   CString  PROBEPATHTOP;
   CString  PROBEPATHBOTTOM;
   double   framewidth;
};

int doKelvinAnalysis(CCEtoODBDoc *doc, bool useResistors, double resistorValue, bool useCapacitors, double capacitorValue, bool useInductor);

/////////////////////////////////////////////////////////////////////////////
// CDFTTarget
/////////////////////////////////////////////////////////////////////////////
class CDFTTarget
{
public:
   CDFTTarget() { m_bModified = true; }
   CDFTTarget(const CDFTTarget &Target);
   CDFTTarget(bool Enable, CString Name);
   ~CDFTTarget() {}

   void DumpToFile(CFormatStdioFile &file, int indent);

private:
   bool m_bModified;

   bool m_bEnable;
   CString m_sName;

public:
   CDFTTarget& operator=(const CDFTTarget &Target);
   bool IsModified()             const { return m_bModified;                  }

   bool GetEnabledFlag()         const { return m_bEnable;                    }
   CString GetName()             const { return m_sName;                      }

   void ResetModified()                { m_bModified = FALSE;                 }
   void SetEnabledFlag(bool Enable);
   void SetEnabledFlag(BOOL Enable)    { SetEnabledFlag(Enable?true:false);   }
   void SetName(CString Name);
};
typedef CTypedPtrArray<CPtrArray, CDFTTarget*> CDFTTargetArray;

/////////////////////////////////////////////////////////////////////////////
// CDFTTargetPriority
/////////////////////////////////////////////////////////////////////////////
class CDFTTargetPriority
{
public:
   CDFTTargetPriority() {}
   ~CDFTTargetPriority();

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteXML(CWriteFormat& writeFormat/*, CCamCadFileWriteProgress& progress*/);
   int LoadXML(CXMLNode *node);

private:
   bool m_bModified;

   CDFTTargetArray m_targetsTop;
   CDFTTargetArray m_targetsBot;

public:
   CDFTTargetPriority& operator=(const CDFTTargetPriority &TargetPriority);
   bool IsModified() const;

   int GetCount_TopTargets()                             const { return m_targetsTop.GetCount();         }
   CDFTTarget *GetAt_TopTargets(int index)               const { return m_targetsTop.GetAt(index);       }
   CDFTTarget *Find_TopTargets(CString Name, int &index) const;

   int GetCount_BotTargets()                             const { return m_targetsBot.GetCount();         }
   CDFTTarget *GetAt_BotTargets(int index)               const { return m_targetsBot.GetAt(index);       }
   CDFTTarget *Find_BotTargets(CString Name, int &index) const;

   void ResetModified();

   int Add_TopTargets(bool Enable, CString Name);
   CDFTTarget *InsertAt_TopTargets(int index, bool Enable, CString Name);
   void Swap_TopTargets(int fromIndex, int toIndex);
   void RemoveAt_TopTargets(int index);
   void RemoveAll_TopTargets();

   int Add_BotTargets(bool Enable, CString Name);
   CDFTTarget *InsertAt_BotTargets(int index, bool Enable, CString Name);
   void Swap_BotTargets(int fromIndex, int toIndex);
   void RemoveAt_BotTargets(int index);
   void RemoveAll_BotTargets();
};

/////////////////////////////////////////////////////////////////////////////
// CDFTProbeTemplate
/////////////////////////////////////////////////////////////////////////////
class CDFTProbeTemplate
{
public:
   CDFTProbeTemplate();
   CDFTProbeTemplate(const CDFTProbeTemplate &Probe);
   CDFTProbeTemplate(bool Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, bool UseComponentOutline, double ComponentOutlineDistance);
   ~CDFTProbeTemplate() {}

   void DumpToFile(CFormatStdioFile &file, int indent);
   int LoadXML(CXMLNode *node);

private:
   bool m_bModified;

   bool m_bUse;
   CString m_sName;
   double m_dDiameter;
   double m_dDrillSize;
   double m_dTextSize;
   int m_iCost;
   bool m_bUseComponentOutline;
   double m_dComponentOutlineDistance;

public:
   CDFTProbeTemplate& operator=(const CDFTProbeTemplate &Probe);
   bool IsModified()                    const { return m_bModified;                                          }

   bool GetUseFlag()                    const { return m_bUse;                                               }
   CString GetName()                    const { return m_sName;                                              }
   double GetDiameter()                 const { return m_dDiameter; }
   double GetDrillSize()                const { return m_dDrillSize; }
   double GetTextSize()                 const { return m_dTextSize > 0.0 ? m_dTextSize : m_dDrillSize * 0.70;}
   int GetCost()                        const { return m_iCost;                                              }
   bool GetUseComponentOutline()        const { return m_bUseComponentOutline;                               }
   double GetComponentOutlineDistance() const { return m_dComponentOutlineDistance;                          }

   void ResetModified()                                  { m_bModified = FALSE;                                         }
   void SetUseFlag(bool Use);
   void SetUseFlag(BOOL Use)                             { SetUseFlag(Use?true:false);                                  }
   void SetName(CString Name);
   void SetDiameter(double Diameter);
   void SetDrillSize(double DrillSize);
   void SetTextSize(double TextSize);
   void SetCost(int Cost);
   void SetUseComponentOutline(bool UseComponentOutline);
   void SetUseComponentOutline(BOOL UseComponentOutline) { SetUseComponentOutline(UseComponentOutline?true:false);      }
   void SetComponentOutlineDistance(double ComponentOutlineDistance);
};
typedef CTypedPtrList<CPtrList, CDFTProbeTemplate*> CDFTProbeTemplateList;

/////////////////////////////////////////////////////////////////////////////
// CDFTProbeTemplates
/////////////////////////////////////////////////////////////////////////////
class CDFTProbeTemplates
{
public:
   CDFTProbeTemplates() 
   {
      m_probeTemplatesTop.RemoveAll();
      m_probeTemplatesBot.RemoveAll();
   }
   ~CDFTProbeTemplates();

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteXML(CWriteFormat& writeFormat/*, CCamCadFileWriteProgress& progress*/);
   int LoadXML(CXMLNode *node);

private:
   bool m_bModified;

   CDFTProbeTemplateList m_probeTemplatesTop;
   CDFTProbeTemplateList m_probeTemplatesBot;

public:
   CDFTProbeTemplates& operator=(const CDFTProbeTemplates &Probes);
   bool IsModified() const;

   POSITION GetHeadPosition_TopProbes()                     const { return m_probeTemplatesTop.GetHeadPosition();    }
   POSITION GetTailPosition_TopProbes()                     const { return m_probeTemplatesTop.GetTailPosition();    }
   CDFTProbeTemplate *GetHead_TopProbes()                   const { return m_probeTemplatesTop.GetHead();            }
   CDFTProbeTemplate *GetAt_TopProbes(POSITION pos)         const { return m_probeTemplatesTop.GetAt(pos);           }
   CDFTProbeTemplate *GetTail_TopProbes()                   const { return m_probeTemplatesTop.GetTail();            }
   CDFTProbeTemplate *GetNext_TopProbes(POSITION &pos)      const { return m_probeTemplatesTop.GetNext(pos);         } 
   CDFTProbeTemplate *GetPrev_TopProbes(POSITION &pos)      const { return m_probeTemplatesTop.GetPrev(pos);         }
   CDFTProbeTemplate *Find_TopProbes(CString Name, POSITION &pos) const;

   CDFTProbeTemplate *GetLargest_UsedTopProbes(POSITION &pos) const;
   CDFTProbeTemplate *GetSmallest_UsedTopProbes(POSITION &pos) const;
   POSITION GetTailPosition_UsedTopProbes() const;
   CDFTProbeTemplate *GetPrev_UsedTopProbes(POSITION &pos) const;

   POSITION GetHeadPosition_BotProbes()                     const { return m_probeTemplatesBot.GetHeadPosition();    }
   POSITION GetTailPosition_BotProbes()                     const { return m_probeTemplatesBot.GetTailPosition();    }
   CDFTProbeTemplate *GetHead_BotProbes()                   const { return m_probeTemplatesBot.GetHead();            }
   CDFTProbeTemplate *GetAt_BotProbes(POSITION pos)         const { return m_probeTemplatesBot.GetAt(pos);           }
   CDFTProbeTemplate *GetTail_BotProbes()                   const { return m_probeTemplatesBot.GetTail();            }
   CDFTProbeTemplate *GetNext_BotProbes(POSITION &pos)      const { return m_probeTemplatesBot.GetNext(pos);         }
   CDFTProbeTemplate *GetPrev_BotProbes(POSITION &pos)      const { return m_probeTemplatesBot.GetPrev(pos);         }
   CDFTProbeTemplate *Find_BotProbes(CString Name, POSITION &pos) const;

   CDFTProbeTemplate *Find_Probe(CString Name, POSITION &pos, ETestSurface surface) const;
   CDFTProbeTemplate *GetLargest_UsedBotProbes(POSITION &pos) const;
   CDFTProbeTemplate *GetSmallest_UsedBotProbes(POSITION &pos) const;
   POSITION GetTailPosition_UsedBotProbes() const;
   CDFTProbeTemplate *GetPrev_UsedBotProbes(POSITION &pos) const;

   void ResetModified();

   void Add_TopProbes(bool Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, bool UseComponentOutline, double ComponentOutlineDistance);
   void Add_TopProbes(BOOL Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, BOOL UseComponentOutline, double ComponentOutlineDistance)
      { Add_TopProbes(Use?true:false, Name, Diameter, DrillSize, TextSize, Cost, UseComponentOutline?true:false, ComponentOutlineDistance); }
   void RemoveAt_TopProbes(POSITION pos);
   void RemoveAll_TopProbes();

   void Add_BotProbes(bool Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, bool UseComponentOutline, double ComponentOutlineDistance);
   void Add_BotProbes(BOOL Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, BOOL UseComponentOutline, double ComponentOutlineDistance)
      { Add_BotProbes(Use?true:false, Name, Diameter, DrillSize, TextSize, Cost, UseComponentOutline?true:false, ComponentOutlineDistance); }
   void RemoveAt_BotProbes(POSITION pos);
   void RemoveAll_BotProbes();
};

/////////////////////////////////////////////////////////////////////////////
// CDFTFeature
/////////////////////////////////////////////////////////////////////////////
class CDFTFeature
{
public:
   CDFTFeature();
   CDFTFeature(const CDFTFeature &Feature);
   CDFTFeature(CString Component, CString Pin, CString Device);
   ~CDFTFeature() {}

   void DumpToFile(CFormatStdioFile &file, int indent);

private:
   bool m_bModified;

   CString m_sComponent;
   CString m_sPin;
   CString m_sDevice;

public:
   CDFTFeature& operator=(const CDFTFeature &Feature);
   bool IsModified()                const { return m_bModified;   }

   CString GetComponent()           const { return m_sComponent;  }
   CString GetPin()                 const { return m_sPin;        }
   CString GetDevice()              const { return m_sDevice;     }

   void ResetModified()                   { m_bModified = FALSE;  }
   void SetComponent(CString Component);
   void SetPin(CString Pin);
   void SetDevice(CString Device);
};
typedef CTypedPtrList<CPtrList, CDFTFeature*> CDFTFeatureList;

/////////////////////////////////////////////////////////////////////////////
// CDFTPreconditions
/////////////////////////////////////////////////////////////////////////////
class CDFTPreconditions
{
public:
   CDFTPreconditions() {}
   ~CDFTPreconditions();

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);

private:
   bool m_bModified;

   CDFTFeatureList m_forceProbedFeatures;
   CDFTFeatureList m_noProbedFeatures;

public:
   CDFTPreconditions& operator=(const CDFTPreconditions &Preconditions);
   bool IsModified() const;

   POSITION GetHeadPosition_ForcedFeatures()             const { return m_forceProbedFeatures.GetHeadPosition();  }
   POSITION GetTailPosition_ForcedFeatures()             const { return m_forceProbedFeatures.GetTailPosition();  }
   CDFTFeature *GetHead_ForcedFeatures()                 const { return m_forceProbedFeatures.GetHead();          }
   CDFTFeature *GetAt_ForcedFeatures(POSITION pos)       const { return m_forceProbedFeatures.GetAt(pos);         }
   CDFTFeature *GetTail_ForcedFeatures()                 const { return m_forceProbedFeatures.GetTail();          }
   CDFTFeature *GetNext_ForcedFeatures(POSITION &pos)    const { return m_forceProbedFeatures.GetNext(pos);       } 
   CDFTFeature *GetPrev_ForcedFeatures(POSITION &pos)    const { return m_forceProbedFeatures.GetPrev(pos);       }
   CDFTFeature *Find_ForcedFeatures(CString Component, CString Pin, POSITION &pos);

   POSITION GetHeadPosition_NoProbedFeatures()           const { return m_noProbedFeatures.GetHeadPosition();     }
   POSITION GetTailPosition_NoProbedFeatures()           const { return m_noProbedFeatures.GetTailPosition();     }
   CDFTFeature *GetHead_NoProbedFeatures()               const { return m_noProbedFeatures.GetHead();             }
   CDFTFeature *GetAt_NoProbedFeatures(POSITION pos)     const { return m_noProbedFeatures.GetAt(pos);            }
   CDFTFeature *GetTail_NoProbedFeatures()               const { return m_noProbedFeatures.GetTail();             }
   CDFTFeature *GetNext_NoProbedFeatures(POSITION &pos)  const { return m_noProbedFeatures.GetNext(pos);          }
   CDFTFeature *GetPrev_NoProbedFeatures(POSITION &pos)  const { return m_noProbedFeatures.GetPrev(pos);          }
   CDFTFeature *Find_NoProbedFeatures(CString Component, CString Pin, POSITION &pos);

   void ResetModified();

   void AddHead_ForcedFeatures(CString Component, CString Pin, CString Device);
   void AddTail_ForcedFeatures(CString Component, CString Pin, CString Device);
   void RemoveAt_ForcedFeatures(POSITION pos);
   void RemoveAll_ForcedFeatures();

   void AddHead_NoProbedFeatures(CString Component, CString Pin, CString Device);
   void AddTail_NoProbedFeatures(CString Component, CString Pin, CString Device);
   void RemoveAt_NoProbedFeatures(POSITION pos);
   void RemoveAll_NoProbedFeatures();
};   
   
/////////////////////////////////////////////////////////////////////////////
// CDFTProbeableNet
/////////////////////////////////////////////////////////////////////////////
class CDFTProbeableNet
{
public:
   CDFTProbeableNet();
   CDFTProbeableNet(const CDFTProbeableNet &ProbeableNet);
   CDFTProbeableNet(CString Name, int PinCount, eProbeableNetType NetType, bool NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet);
   ~CDFTProbeableNet() {}

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteCSV(CFormatStdioFile &file);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);

private:
   bool m_bModified;

   CString m_sName;
   int m_iPinCount;
   eProbeableNetType m_eNetType;          // 0 - Power, 1 - Ground, 2 - Signal
   bool m_bNoProbe;
   double m_dVoltage;
   double m_dCurrent;
   int m_iTestResourcesPerNet;
   int m_iPowerInjectionResourcesPerNet;

public:
   CDFTProbeableNet& operator=(const CDFTProbeableNet &ProbeableNet);
   bool IsModified()                      const { return m_bModified;                        }

   CString GetName()                      const { return m_sName;                            }
   int GetPinCount()                      const { return m_iPinCount;                        }
   eProbeableNetType GetNetType()         const { return m_eNetType;                         }
   CString GetNetTypeName() const;
   bool GetNoProbe()                      const { return m_bNoProbe;                         }
   double GetVoltage()                    const { return m_dVoltage;                         }
   double GetCurrent()                    const { return m_dCurrent;                         }
   int GetTestResourcesPerNet()           const { return m_iTestResourcesPerNet;             }
   int GetPowerInjectionResourcesPerNet() const { return m_iPowerInjectionResourcesPerNet;   }
   CString GetFirstPinRef();

   void ResetModified()                         { m_bModified = FALSE;                       }
   void SetName(CString Name);
   void SetPinCount(int PinCount);
   void SetNetType(eProbeableNetType NetType);
   void SetNoProbe(bool NoProbe);
   void SetNoProbe(BOOL NoProbe)                { SetNoProbe(NoProbe?true:false);            }
   void SetVoltage(double Voltage);
   void SetCurrent(double Current);
   void SetTestResourcesPerNet(int TestResourcesPerNet);
   void SetPowerInjectionResourcesPerNet(int PowerInjectionResourcesPerNet);
};

typedef CTypedPtrList<CPtrList, CDFTProbeableNet*> CDFTProbableNetList;

/////////////////////////////////////////////////////////////////////////////
// CDFTNetConditions
/////////////////////////////////////////////////////////////////////////////
class CDFTNetConditions
{
public:
   CDFTNetConditions() {}
   ~CDFTNetConditions();

   void DumpToFile(CFormatStdioFile &file, int indent);
   bool WriteCSV(CString folder);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node, FileStruct *file);

private:
   bool m_bModified;

   CDFTProbableNetList m_probeableNets;

public:
   CDFTNetConditions& operator=(const CDFTNetConditions &NetConditions);
   bool IsModified() const;

   POSITION GetHeadPosition_ProbeableNets()                 const { return m_probeableNets.GetHeadPosition();  }
   POSITION GetTailPosition_ProbeableNets()                 const { return m_probeableNets.GetTailPosition();  }
   CDFTProbeableNet *GetHead_ProbeableNets()                const { return m_probeableNets.GetHead();          }
   CDFTProbeableNet *GetAt_ProbeableNets(POSITION pos)      const { return m_probeableNets.GetAt(pos);         }
   CDFTProbeableNet *GetTail_ProbeableNets()                const { return m_probeableNets.GetTail();          }
   CDFTProbeableNet *GetNext_ProbeableNets(POSITION &pos)   const { return m_probeableNets.GetNext(pos);       } 
   CDFTProbeableNet *GetPrev_ProbeableNets(POSITION &pos)   const { return m_probeableNets.GetPrev(pos);       }
   int GetCount_ProbeableNets()                             const { return m_probeableNets.GetCount();         }
   CDFTProbeableNet *Find_ProbeableNet(CString Name, POSITION &pos, bool caseSensitive = true) const;

   void ResetModified();
   void Initialize(CCEtoODBDoc *doc, FileStruct *pcbfile, CDFTSolution *dftSolution);

   void AddHead_ProbeableNets(CString Name, int PinCount, eProbeableNetType NetType, bool NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet);
   void AddHead_ProbeableNets(CString Name, int PinCount, eProbeableNetType NetType, BOOL NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet)
      { AddHead_ProbeableNets(Name, PinCount, NetType, NoProbe?true:false, Voltage, Current, TestResourcesPerNet, PowerInjectionResourcesPerNet); }
   void AddTail_ProbeableNets(CString Name, int PinCount, eProbeableNetType NetType, bool NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet);
   void AddTail_ProbeableNets(CString Name, int PinCount, eProbeableNetType NetType, BOOL NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet)
      { AddTail_ProbeableNets(Name, PinCount, NetType, NoProbe?true:false, Voltage, Current, TestResourcesPerNet, PowerInjectionResourcesPerNet); }
   void RemoveAt_ProbeableNets(POSITION pos);
   void RemoveAll_ProbeableNets();

   void UpdateNetsInFile(FileStruct *file, CCEtoODBDoc *doc);
};


/////////////////////////////////////////////////////////////////////////////
// CTestPlan
/////////////////////////////////////////////////////////////////////////////
class CTestPlan
{
private:
   static int m_nextId;
   int m_id;
   PageUnitsTag m_pageUnits;
   bool m_bModified;

public:
   CTestPlan(PageUnitsTag pageUnit = DFT_DEFAULT_UNIT);
   CTestPlan(const CTestPlan &testPlan);
   CTestPlan(CString fileName, PageUnitsTag pageUnit = DFT_DEFAULT_UNIT);
   ~CTestPlan() {}

   CTestPlan& operator=(const CTestPlan &testPlan);

private:
   int loadXML_PPOnly(CXMLNode *node, FileStruct *file);

public:
   int getId() const;
   bool IsModified();
   void ResetModified();
   void SetDefaults(int pageUnit = DFT_DEFAULT_UNIT);
   void SetPPDefaults();

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteAAReport(CFormatStdioFile &file, int indent);
   void WriteReport(CFormatStdioFile &file, int indent);

   int LoadFile(CString fileName, int pageUnit);
   int LoadFile_PPOnly(CString fileName);
   int SaveToFile(CString fileName, int pageUnit);

   int LoadXML(CXMLNode *node, FileStruct *file, int pageUnit);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   void Scale(double factor);

//////////////////////////////////////////////////////////////////////////
// Access Analysis private members
private:
   ////////////////////////////////////////
   // Package Outline
   bool m_bPackageOutlineModified;
   bool m_bEnableRectangularOutline;
   bool m_bEnableOverwriteOutline;
   bool m_bIncludeSinglePinComp;

   ////////////////////////////////////////
   // Height Analysis
   CHeightAnalysis m_heightAnalysisTop;
   CHeightAnalysis m_heightAnalysisBot;

   ////////////////////////////////////////
   // Physical Constraints
   bool m_bEnableSolermaskAnalysis; 
   int m_iOutlineToUse;          // -1-None, 0-Real Part outline, 1-DFT outline, 2-Both
   int m_iOutlinePriority;       // 0-Real Part outline, 1-DFT outline
   bool m_bIgnore1PinCompOutline;
   bool m_bIgnoreUnloadedCompOutline;
   CPhysicalConst m_constraintsTop;          
   CPhysicalConst m_constraintsBot;
   bool m_beadProbeUpdateNeeded;

   ////////////////////////////////////////
   // Target Type
   CTargetType m_targetTypesTop;
   CTargetType m_targetTypesBot;

   ////////////////////////////////////////
   // Options
   int m_iSurface;               // 0-Top, 1-Bottom, 2-Both
   int m_iTesterType;            // 0-Fixture, 1-Fixtureless
   bool m_bIncludeMulPinNet;
   bool m_bIncludeSngPinNet;
   bool m_bIncludeNCNet;
   bool m_bCreateNCNet;
   bool m_bRetainedExistingProbe;
   bool m_bMultipleReasonCodes;

   //bool m_bEnableAccessOffset;
   //bool m_bEnableViaOffset;
   //bool m_bEnableSmdOffset;
   //bool m_bEnableThruOffset;
   CAccessOffsetOptions m_accessOffsetOptions;

   bool m_bAAWriteIgnoredSurface;

   int loadAccessAnalysis(CXMLNode *node, double unitFactor, int pageUnit);
   int loadHeightAnalysis(CXMLNode *node, double unitFactor, int pageUnit);
   int loadPhysicalConstrants(CXMLNode *node, double unitFactor);
   int loadTargetTypes(CXMLNode *node);

public:
   bool GetPackageOutlineModified()        const { return m_bPackageOutlineModified;                   }
   bool GetEnableRectangularOutline()      const { return m_bEnableRectangularOutline;                 }
   bool GetEnableOverwriteOutline()        const { return m_bEnableOverwriteOutline;                   }
   bool GetIncludeSinglePinComp()          const { return m_bIncludeSinglePinComp;                     }
   bool GetEnableSoldermaskAnalysis()      const { return m_bEnableSolermaskAnalysis;                  }
   int GetOutlineToUse()                   const { return m_iOutlineToUse;                             }
   int GetOutlinePriority()                const { return m_iOutlinePriority;                          }
   bool GetIgnore1PinCompOutline()         const { return m_bIgnore1PinCompOutline;                    }
   bool GetIgnoreUnloadedCompOutline()     const { return m_bIgnoreUnloadedCompOutline;                }
   CHeightAnalysis* GetTopHeightAnalysis()       { return &m_heightAnalysisTop;                        }
   CHeightAnalysis* GetBotHeightAnalysis()       { return &m_heightAnalysisBot;                        }
   CPhysicalConst* GetTopConstraints()           { return &m_constraintsTop;                           }
   CPhysicalConst* GetBotConstraints()           { return &m_constraintsBot;                           }
   CTargetType* GetTopTargetTypes()              { return &m_targetTypesTop;                           }
   CTargetType* GetBotTargetTypes()              { return &m_targetTypesBot;                           }
   int GetSurface()                        const { return m_iSurface;                                  }
   int GetTesterType()                     const { return m_iTesterType;                               }
   bool GetIncludeMulPinNet()              const { return m_bIncludeMulPinNet;                         }
   bool GetIncludeSngPinNet()              const { return m_bIncludeSngPinNet;                         }
   bool GetIncludeNCNet()                  const { return m_bIncludeNCNet;                             }
   bool GetCreateNCNet()                   const { return m_bCreateNCNet;                              }
   bool GetRetainedExistingProbe()         const { return m_bRetainedExistingProbe;                    }
   bool GetMultipleReasonCodes()           const { return m_bMultipleReasonCodes;                      }
   bool GetAAWriteIgnoreSurface()          const { return m_bAAWriteIgnoredSurface;                    }
   bool IsBeadProbeUpdateNeeded()          const { return m_beadProbeUpdateNeeded;                     }
   void SetBeadProbeUpdateNeeded(bool updateNeeded)    { m_beadProbeUpdateNeeded = updateNeeded;       }
   void ValidateSettings(PageUnitsTag pageUnits);
   const CAccessOffsetOptions& getAccessOffsetOptions() const { return m_accessOffsetOptions;                       }
   void setAccessOffsetOptions(const CAccessOffsetOptions& accessOffsetOptions) { m_accessOffsetOptions = accessOffsetOptions; }

   void SetPackageOutlineModified();
   void SetEnableRectangularOutline(bool enable)         { m_bEnableRectangularOutline = enable;               }
   void SetEnableOverwriteOutline(bool enable)           { m_bEnableOverwriteOutline = enable;                 }
   void SetIncludeSinglePinComp(bool enable)             { m_bIncludeSinglePinComp = enable;                   }
   void SetEnableSoldermaskAnalysis(bool enable);
   void SetEnableSoldermaskAnalysis(BOOL enable)         { SetEnableSoldermaskAnalysis(enable?true:false);     }
   void SetOutlineToUse(int value);
   void SetOutlinePriority(int value);
   void SetIgnore1PinCompOutline(bool enable);
   void SetIgnore1PinCompOutline(BOOL enable)            { SetIgnore1PinCompOutline(enable?true:false);        }
   void SetIgnoreUnloadedCompOutline(bool enable);
   void SetIgnoreUnloadedCompOutline(BOOL enable)        { SetIgnoreUnloadedCompOutline(enable?true:false);    }
   void SetTopHeightAnalysis(CHeightAnalysis heightAnalysis);
   void SetBotHeightAnalysis(CHeightAnalysis heightAnalysis);
   void SetTopConstraints(CPhysicalConst constraints);
   void SetBotConstraints(CPhysicalConst constraints);
   void SetTopTargetTypes(CTargetType targetTypes);
   void SetBotTargetTypes(CTargetType targetTypes);
   void SetSurface(int value);
   void SetTesterType(int value);
   void SetIncludeMulPinNet(bool enable);
   void SetIncludeMulPinNet(BOOL enable)                 { SetIncludeMulPinNet(enable?true:false);             }
   void SetIncludeSngPinNet(bool enable);
   void SetIncludeSngPinNet(BOOL enable)                 { SetIncludeSngPinNet(enable?true:false);             }
   void SetIncludeNCNet(bool enable);
   void SetIncludeNCNet(BOOL enable)                     { SetIncludeNCNet(enable?true:false);                 }
   void SetCreateNCNet(bool enable);
   void SetCreateNCNet(BOOL enable)                      { SetCreateNCNet(enable?true:false);                  }
   void SetRetainedExistingProbe(bool enable);
   void SetMultipleReasonCodes(bool enable);
   void SetMultipleReasonCodes(BOOL enable)              { SetMultipleReasonCodes(enable?true:false);          }
   void SetAAWriteIgnoreSurface(bool enable);                           
   void SetAAWriteIgnoreSurface(BOOL enable)             { SetAAWriteIgnoreSurface(enable?true:false);            }

//////////////////////////////////////////////////////////////////////////
// Probe Placement private members
private:
   bool m_bPPWriteIgnoredSurface;

   ////////////////////////////////////////
   // Target Priority
   CDFTTargetPriority m_targetPriority;

   ////////////////////////////////////////
   // Probes
   CDFTProbeTemplates m_probeTemplates;
   bool m_bNeverMirrorRefname;

   ////////////////////////////////////////
   // Kelvin & Power Injection 
   //    Kelvin Analysis
   bool m_bUseResistor;
   double m_dResistorValue;
   bool m_bUseCapacitor;
   double m_dCapacitorValue;
   bool m_bUseInductor;
   int m_iPowerInjectionUsage;   // 0 - No power injection
                                 // 1 - Probes per power rail
                                 // 2 - Probes per number of connection per power rail
                                 // 3 - Probes per number of nets per power rail
   int m_iPowerInjectionValue;   // Meaning depends on m_powerInjectionUsage

   ////////////////////////////////////////
   // Preconditions
   CDFTPreconditions m_preconditions;

   ////////////////////////////////////////
   // Net Conditions
   CDFTNetConditions m_netConditions;

   ////////////////////////////////////////
   // Options
   int m_iProbeStartNumber;
   bool m_bAllowDoubleWiring;
   ETestSurface m_eProbeSide;    // 0-Top, 1-Bottom, 2-Both
   ETestSurface m_iProbeSidePreference;   // 0-Top, 1-Bottom, 2-NotValid
   bool m_bUseProbeOffset;
   bool m_bPlaceOnAllAccessible;
   bool m_bOffsetSMDTop;
   bool m_bOffsetPTHTop;
   bool m_bOffsetVIATPTop;
   bool m_bOffsetSMDBot;
   bool m_bOffsetPTHBot;
   bool m_bOffsetVIATPBot;
   bool m_bPlaceOnAllAccess;
   bool m_bUseExistingProbes;
   EProbeUsage m_eProbeUse;
   bool m_bRetainProbeNames;
   bool m_bRetainProbePositions;
   bool m_bRetainProbeSizes;
   bool m_bNetConditionCaseSensitive;

   int loadProbePlacement(CXMLNode *node, FileStruct *file);

public:
   bool GetPPWriteIgnoreSurface()                  const { return m_bPPWriteIgnoredSurface;                                                  }
   void SetPPWriteIgnoreSurface(bool enable);

   ////////////////////////////////////////
   // Target Priority
   CDFTTargetPriority &GetTargetPriority()               { return m_targetPriority;                                                          }

   ////////////////////////////////////////
   // Probes
   CDFTProbeTemplates &GetProbes()                       { return m_probeTemplates;                                                          }
   bool GetNeverMirrorRefname()                          { return m_bNeverMirrorRefname;   }
   void SetNeverMirrorRefname(bool nm)                   { m_bNeverMirrorRefname = nm;     }

   ////////////////////////////////////////
   // Kelvin & Power Injection 
   //    Kelvin Analysis
   bool GetUseResistor()        const { return m_bUseResistor;         }
   double GetResistorValue()    const { return m_dResistorValue;       }
   bool GetUseCapacitor()       const { return m_bUseCapacitor;        }
   double GetCapacitorValue()   const { return m_dCapacitorValue;      }
   bool GetUseInductor()        const { return m_bUseInductor;         }
   int GetPowerInjectionUsage() const { return m_iPowerInjectionUsage; }
   int GetPowerInjectionValue() const { return m_iPowerInjectionValue; }

   void SetUseResistor(bool UseResistor);
   void SetUseResistor(BOOL UseResistor)                 { SetUseResistor(UseResistor?true:false);                                           }
   void SetResistorValue(double ResistorValue);
   void SetUseCapacitor(bool UseCapacitor);
   void SetUseCapacitor(BOOL UseCapacitor)               { SetUseCapacitor(UseCapacitor?true:false);                                         }
   void SetCapacitorValue(double CapacitorValue);
   void SetUseInductor(bool UseInductor);
   void SetUseInductor(BOOL UseInductor)                 { SetUseInductor(UseInductor?true:false);                                           }
   void SetPowerInjectionUsage(int PowerInjectionUsage);
   void SetPowerInjectionValue(int PowerInjectionValue);

   ////////////////////////////////////////
   // Preconditions
   CDFTPreconditions &GetPreconditions()                 { return m_preconditions;                                                           }

   ////////////////////////////////////////
   // Net Conditions
   CDFTNetConditions &GetNetConditions()                 { return m_netConditions;                                                           }

   ////////////////////////////////////////
   // Options
   int GetProbeStartNumber()             const { return m_iProbeStartNumber;          }
   bool GetAllowDoubleWiring()           const { return m_bAllowDoubleWiring;         }
   ETestSurface GetProbeSide()           const { return m_eProbeSide;                 }
   bool CanProbeTopSide()                const { return (m_eProbeSide == testSurfaceTop || m_eProbeSide == testSurfaceBoth);       }
   bool CanProbeBotSide()                const { return (m_eProbeSide == testSurfaceBottom || m_eProbeSide == testSurfaceBoth);    }
   ETestSurface GetProbeSidePreference() const { return m_iProbeSidePreference;       }
   bool GetUseProbeOffset()              const { return m_bUseProbeOffset;            }
   bool GetPlaceOnAllAccessible()        const { return m_bPlaceOnAllAccessible;      }
   bool GetOffsetSMDTop()                const { return m_bOffsetSMDTop;              }
   bool GetOffsetPTHTop()                const { return m_bOffsetPTHTop;              }
   bool GetOffsetVIATPTop()              const { return m_bOffsetVIATPTop;            }
   bool GetOffsetSMDBot()                const { return m_bOffsetSMDBot;              }
   bool GetOffsetPTHBot()                const { return m_bOffsetPTHBot;              }
   bool GetOffsetVIATPBot()              const { return m_bOffsetVIATPBot;            }
   bool GetPlaceOnAllAccess()            const { return m_bPlaceOnAllAccess;          }
   bool GetUseExistingProbes()           const { return m_bUseExistingProbes;         }
   EProbeUsage GetProbeUse()             const { return m_eProbeUse;                  }
   bool RetainProbeNames()               const { return m_bRetainProbeNames;          }
   bool RetainProbePositions()           const { return m_bRetainProbePositions;      }
   bool RetainProbeSizes()               const { return m_bRetainProbeSizes;          }
   bool GetNetConditionCaseSensitive()   const { return m_bNetConditionCaseSensitive; }

   void SetProbeStartNumber(int ProbeStartNumber);
   void SetAllowDoubleWiring(bool AllowDoubleWiring);
   void SetAllowDoubleWiring(BOOL AllowDoubleWiring)        { SetAllowDoubleWiring(AllowDoubleWiring?true:false);       }
   void SetProbeSide(ETestSurface ProbeSide);
   void SetProbeSidePreference(ETestSurface ProbeSidePreference);
   void SetUseProbeOffset(bool UseProbeOffset);
   void SetUseProbeOffset(BOOL UseProbeOffset)              { SetUseProbeOffset(UseProbeOffset?true:false);             }
   void SetPlaceOnAllAccessible(bool PlaceOnAllAccessible);
   void SetPlaceOnAllAccessible(BOOL PlaceOnAllAccessible)  { SetPlaceOnAllAccessible(PlaceOnAllAccessible?true:false); }
   void SetOffsetSMDTop(bool OffsetSMDTop);
   void SetOffsetSMDTop(BOOL OffsetSMDTop)                  { SetOffsetSMDTop(OffsetSMDTop?true:false);                 }
   void SetOffsetPTHTop(bool OffsetPTHTop);
   void SetOffsetPTHTop(BOOL OffsetPTHTop)                  { SetOffsetPTHTop(OffsetPTHTop?true:false);                 }
   void SetOffsetVIATPTop(bool OffsetVIATPTop);
   void SetOffsetVIATPTop(BOOL OffsetVIATPTop)              { SetOffsetVIATPTop(OffsetVIATPTop?true:false);             }
   void SetOffsetSMDBot(bool OffsetSMDBot);
   void SetOffsetSMDBot(BOOL OffsetSMDBot)                  { SetOffsetSMDBot(OffsetSMDBot?true:false);                 }
   void SetOffsetPTHBot(bool OffsetPTHBot);
   void SetOffsetPTHBot(BOOL OffsetPTHBot)                  { SetOffsetPTHBot(OffsetPTHBot?true:false);                 }
   void SetOffsetVIATPBot(bool OffsetVIATPBot);
   void SetOffsetVIATPBot(BOOL OffsetVIATPBot)              { SetOffsetVIATPBot(OffsetVIATPBot?true:false);             }
   void SetPlaceOnAllAccess(bool PlaceOnAllAccess);
   void SetPlaceOnAllAccess(BOOL PlaceOnAllAccess)          { SetPlaceOnAllAccess(PlaceOnAllAccess?true:false);         }
   void SetUseExistingProbes(bool UseExistingProbes);
   void SetUseExistingProbes(BOOL UseExistingProbes)        { SetUseExistingProbes(UseExistingProbes?true:false);       }
   void SetProbeUse(EProbeUsage ProbeUse);
   void SetRetainProbeNames(bool RetainProbeNames);
   void SetRetainProbeNames(BOOL RetainProbeNames)          { SetRetainProbeNames(RetainProbeNames?true:false);         }
   void SetRetainProbePositions(bool RetainProbePositions);
   void SetRetainProbePositions(BOOL RetainProbePositions)  { SetRetainProbePositions(RetainProbePositions?true:false); }
   void SetRetainProbeSizes(bool RetainProbeSizes);
   void SetRetainProbeSizes(BOOL RetainProbeSizes)          { SetRetainProbeSizes(RetainProbeSizes?true:false);         }
   void SetNetConditionCaseSensitive(bool caseSensitive);

   bool ReadTestWayFile(CString fileName, bool caseSensitive);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Probe Placement Solution
////////////////////////////////////////////////////////////////////////////////
class CPPProbe;

/******************************************************************************
* CPPAccessibleLocation
*/
class CPPAccessibleLocation
{
public:
   CPPAccessibleLocation(CCEtoODBDoc *doc, DataStruct *testAccessData = NULL);
   CPPAccessibleLocation::CPPAccessibleLocation(CCEtoODBDoc& camCadDoc,const CEntity& featureEntity, CPoint2d location, CString targetType, CString netName, ETestSurface surface);
   //CPPAccessibleLocation(CCEtoODBDoc *doc, EEntityType featureType, void *feature, CPoint2d location, CString targetType, CString netName, ETestSurface surface);
   CPPAccessibleLocation(CPPAccessibleLocation &accessibleLocation);
   ~CPPAccessibleLocation();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);

private:
   CCEtoODBDoc *m_pDoc;
   CEntity m_featureEntity;
   DataStruct *m_pTA;
   CPoint2d m_ptLocation;
   CString m_sTargetType;
   CString m_sNetName;
   ETestSurface m_eSurface;
   double m_distToClosestPart; // used during probe placement/optimization

   EFeatureCondition m_eFeatureCondition;
   CPPProbe *m_pProbe;
   CPPProbe *m_pDoubleWiredProbe;

public:
   CPPAccessibleLocation& operator=(const CPPAccessibleLocation &accessibleLocation);

 //EEntityType GetFeatureType()            const { return m_eFeatureType;                }
 //void *GetFeature()                      const { return m_pFeature;                    }
   const CEntity& getFeatureEntity()       const { return m_featureEntity;               }
   DataStruct *GetTAInsert()               const { return m_pTA;                         }
   CString GetTargetType()                 const { return m_sTargetType;                 }
   CString GetNetName()                    const { return m_sNetName;                    }
   ETestSurface GetSurface()               const { return m_eSurface;                    }
   CPoint2d GetLocation()                  const { return m_ptLocation;                  }
   EFeatureCondition GetFeatureCondition() const { return m_eFeatureCondition;           }
   CPPProbe *GetPlaceableProbe()           const { return m_pProbe;                      }
   CPPProbe *GetDoubleWiredProbe()         const { return m_pDoubleWiredProbe;           }
   bool IsProbed()                         const { return (m_pProbe != NULL);            }
   bool IsDoubleWired()                    const { return (m_pDoubleWiredProbe != NULL); }

   void SetClosestDistance(double dist) { m_distToClosestPart = dist; }
   double GetClosestDistance()          { return m_distToClosestPart; }

   void SetFeatureCondition(EFeatureCondition condition);

   void PlaceProbe(CPPProbe *probe);
   CPPProbe *RemoveProbe();
   void PlaceDoubleWiredProbe(CPPProbe *probe);
   CPPProbe *RemoveDoubleWiredProbe();
   void SetTAInsert(DataStruct *taInsert) { m_pTA = taInsert; }
};
typedef CTypedPtrList<CPtrList, CPPAccessibleLocation*> CPPAccessibleLocationList;

/******************************************************************************
* CPPProbe
*/
class CPPProbe
{
public:
   CPPProbe(CCEtoODBDoc *doc, ETestResourceType resType, CString netName);
   CPPProbe(CPPProbe &probe);
   ~CPPProbe();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);

protected:
   CCEtoODBDoc *m_pDoc;
   ETestResourceType m_eResourceType;
   CString m_sNetName;
   CPPAccessibleLocation *m_pAccessibleLocation;
   CString m_sProbeRefname;
   bool m_bPlaced;
   CString m_sProbeTemplateName;
   //CDFTProbeTemplate *m_pProbe;
   bool m_bExisting;

public:
   CPPProbe& operator=(const CPPProbe &probe);

   ETestResourceType GetResourceType()            const { return m_eResourceType;                 }
   CString GetNetName()                           const { return m_sNetName;                      }
   CPPAccessibleLocation *GetAccessibleLocation() const { return m_pAccessibleLocation;           }
   CString GetProbeRefname()                      const { return m_sProbeRefname;                 }
   bool IsPlaced()                                const { return m_bPlaced;                       }
   bool HasProbeTemplate()                        const { return !m_sProbeTemplateName.IsEmpty(); }
   CString GetProbeTemplateName()                 const { return m_sProbeTemplateName;            }
   bool DidExist()                                const { return m_bExisting;                     }

   void SetProbeRefname(CString refname)                { m_sProbeRefname = refname;              }
   void SetAsExisting(bool existed)                     { m_bExisting = existed;                  }

   void SetAccessibleLocationPtr(CPPAccessibleLocation *aLocation);
   CPPAccessibleLocation *RemoveAccessibleLocationPtr();
   void SetProbeTemplate(CDFTProbeTemplate *probeTemplate);

   DataStruct *AddProbeUnplacedToBoard(FileStruct *file, CTestPlan &testPlan);
};

class CPPProbeList: public CTypedPtrList<CPtrList, CPPProbe*>
{
public:
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

/******************************************************************************
* CPPNet
*/
class CPPNet : public CObject
{
public:
   CPPNet(CCEtoODBDoc *doc, CString netName = "");
   CPPNet(CPPNet &ppNet);
   ~CPPNet();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);

private:
   CString m_sNetName;
   NetStruct *m_pNet;

protected:
   eProbeableNetType m_eNetType;
   bool m_bNoProbe;
   int m_iTestResourcesNeeded;
   int m_iPwrInjResourcesNeeded;

   CCEtoODBDoc *m_pDoc;  
   CPPProbeList m_probes;                                // list of all probes for memory management
   CPPAccessibleLocationList m_accessibleLocations;      // list of all accessible locations for memory management

public:
   CPPNet& operator=(const CPPNet &ppNet);

   NetStruct *GetNet()                                                     { return m_pNet;                             }
   CString GetNetName()                                                    { return m_sNetName;                         }
   eProbeableNetType GetNetType()                                    const { return m_eNetType;                         }
   bool GetNoProbeFlag()                                             const { return m_bNoProbe;                         }
   int GetTestResourcesNeeded()                                      const { return m_iTestResourcesNeeded;             }
   int GetPowerInjectionResourcesNeeded()                            const { return m_iPwrInjResourcesNeeded;           }

   POSITION GetHeadPosition_Probes()                                 const { return m_probes.GetHeadPosition();         }
   POSITION GetTailPosition_Probes()                                 const { return m_probes.GetTailPosition();         }
   CPPProbe *GetHead_Probes()                                        const { return m_probes.GetHead();                 }
   CPPProbe *GetTail_Probes()                                        const { return m_probes.GetTail();                 }
   CPPProbe *GetNext_Probes(POSITION &pos)                           const { return m_probes.GetNext(pos);              } 
   CPPProbe *GetPrev_Probes(POSITION &pos)                           const { return m_probes.GetPrev(pos);              }
   CPPProbe *GetAt_Probes(POSITION &pos)                             const { return m_probes.GetAt(pos);                } 
   int GetCount_Probes()                                             const { return m_probes.GetCount();                } 
   int GetCount_TestProbes() const;
   int GetCount_PowerInjectionProbes() const;
   bool HasUnplacedProbe();

   CPPAccessibleLocation *GetHead_AccessibleLocations()              const { return m_accessibleLocations.GetHead();    }
   CPPAccessibleLocation *GetTail_AccessibleLocations()              const { return m_accessibleLocations.GetTail();    }
   CPPAccessibleLocation *GetPrev_AccessibleLocations(POSITION &pos) const { return m_accessibleLocations.GetPrev(pos); }
   CPPAccessibleLocation *GetAt_AccessibleLocations(POSITION &pos)   const { return m_accessibleLocations.GetAt(pos);   }
   int GetCount_AccessibleLocations()                                const { return m_accessibleLocations.GetCount();   }

   POSITION GetHeadPosition_AccessibleLocations(EEntityType featureType = (EEntityType)-1, CString targetType = "", CString netName = "", ETestSurface surface = testSurfaceUnset) const;
   POSITION GetTailPosition_AccessibleLocations(EEntityType featureType = (EEntityType)-1, CString targetType = "", CString netName = "", ETestSurface surface = testSurfaceUnset) const;
   CPPAccessibleLocation *GetNext_AccessibleLocations(POSITION &pos, EEntityType featureType = (EEntityType)-1, CString targetType = "", CString netName = "", ETestSurface surface = testSurfaceUnset) const;

   void SetNet(NetStruct *net)                          { m_pNet = net;                         }
   void SetNetType(eProbeableNetType netType)           { m_eNetType = netType;                 }
   void SetNoProbeFlag(bool noProbe)                    { m_bNoProbe = noProbe;                 }
   void SetTestResourcesNeeded(int resNeeded)           { m_iTestResourcesNeeded = resNeeded;   }
   void SetPowerInjectionResourcesNeeded(int resNeeded) { m_iPwrInjResourcesNeeded = resNeeded; }

   bool IsPowerNet()    { return m_eNetType == probeableNetTypePower || (m_iPwrInjResourcesNeeded > 0 && m_eNetType != probeableNetTypeGround); }
   bool IsGroundNet()   { return m_eNetType == probeableNetTypeGround; }

   virtual CPPProbe *AddHead_Probes(ETestResourceType resType, CString netName, CString probeRefname);
   virtual CPPProbe *AddTail_Probes(ETestResourceType resType, CString netName, CString probeRefname);
   void RemoveAt_Probes(POSITION pos);
   void RemoveAll_Probes();

   virtual CPPAccessibleLocation *AddHead_AccessibleLocations(DataStruct *testAccessData);
   virtual CPPAccessibleLocation *AddTail_AccessibleLocations(DataStruct *testAccessData);
   virtual CPPAccessibleLocation *AddTail_AccessibleLocations(const CEntity& entity, CPoint2d location, CString targetType, CString netName, ETestSurface surface);
   void RemoveAt_AccessibleLocations(POSITION pos);
   void RemoveAll_AccessibleLocations();
};


class CPPNetMap : public CMapSortedStringToOb<CPPNet>
{
public:
   static int AscendingNetNameSortFunc(const void *a, const void *b);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

/******************************************************************************
* CQfeAccessibleLocation
*/
class CQfeAccessibleLocation : public CQfe
{
private:
   CPPAccessibleLocation *m_pAccLocation;

public:
   CQfeAccessibleLocation() {m_pAccLocation = NULL;}
   CQfeAccessibleLocation(CPPAccessibleLocation *accessiblelocation) { m_pAccLocation = accessiblelocation;                   }
   ~CQfeAccessibleLocation() {m_pAccLocation = NULL;}

public:
   virtual CPoint2d getOrigin()                                const { return m_pAccLocation->GetLocation();                  }
   virtual CExtent getExtent()                                 const { return CExtent(0, 0, 0, 0);                            }
   virtual CString getInfoString()                             const { return "";                                             }

   virtual bool isExtentIntersecting(const CExtent& extent)          { return (psInside == extent.pointState(m_pAccLocation->GetLocation()));      }
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance)
                                                                     { return (m_pAccLocation->GetLocation().distance(point) <= distance);         }
   virtual bool isInViolation(CObject2d& otherObject)                { return FALSE;                                          }
   virtual int getObjectType()                                 const { return 0;                                              }


   CPPAccessibleLocation *GetAccessibleLocation()                    { return m_pAccLocation;                                 }
};

/******************************************************************************
* CAccessibleLocationList
*/
class CAccessibleLocationList : public CTypedPtrList<CQfeList,CQfeAccessibleLocation*>
{
public:
   CAccessibleLocationList(int nBlockSize=200) : CTypedPtrList<CQfeList,CQfeAccessibleLocation*>(nBlockSize) { }
};

/******************************************************************************
* CAccessibleLocationTree
*/
class CAccessibleLocationTree : public CQfeExtentLimitedContainer
{
public:
   CAccessibleLocationTree() : CQfeExtentLimitedContainer(CSize2d(0., 0.), false, true) {}
   ~CAccessibleLocationTree() {}

public:
   virtual int search(const CExtent& extent,CAccessibleLocationList& foundList)     { return CQfeExtentLimitedContainer::search(extent,foundList); }
   virtual void setAt(CQfeAccessibleLocation* accessiblelocation)                   { CQfeExtentLimitedContainer::setAt(accessiblelocation);       }
};

/******************************************************************************
* CProbeCount
*/
class CProbeCount
{
public:
   CProbeCount();
   CProbeCount(CString probeName, ETestSurface surface, bool used);
   CProbeCount(CProbeCount &probeCount);
   ~CProbeCount() {}

private:
   CString m_sName;
   ETestSurface m_eSurface;
   bool m_bUsedTop;
   bool m_bUsedBot;
   int m_iTestResourceCountTop;
   int m_iPwrInjResourceCountTop;
   int m_iTestResourceCountBot;
   int m_iPwrInjResourceCountBot;

public:
   bool GetTopUsedFlag()                      const { return m_bUsedTop;                                          }
   bool GetBottomUsedFlag()                   const { return m_bUsedBot;                                          }
   CString GetProbeName()                     const { return m_sName;                                             }
   ETestSurface GetProbeSurface()             const { return m_eSurface;                                          }
   int GetTopTestResourceCount()              const { return m_iTestResourceCountTop;                             }
   int GetTopPowerInjectionResourceCount()    const { return m_iPwrInjResourceCountTop;                           }
   int GetBottomTestResourceCount()           const { return m_iTestResourceCountBot;                             }
   int GetBottomPowerInjectionResourceCount() const { return m_iPwrInjResourceCountBot;                           }
   int GetTotalTestResources()                const { return m_iTestResourceCountTop+m_iTestResourceCountBot;     }
   int GetTotalPowerInjectionResources()      const { return m_iPwrInjResourceCountTop+m_iPwrInjResourceCountBot; }

   void SetTopUsedFlag(bool used)                { m_bUsedTop = used;           }
   void SetBottomUsedFlag(bool used)             { m_bUsedBot = used;           }
   void SetProbeSurface(ETestSurface surface)    { m_eSurface = surface;        }

   // If we are incrementing the usage count, then the item is used, be sure to update that flag too.
   // See case dts0100380281, re Forced Probes on inactive PCB Side.
   void IncrementTopTestResources()              { m_iTestResourceCountTop++;   m_bUsedTop = true; }
   void IncrememtTopPowerInjectionResources()    { m_iPwrInjResourceCountTop++; m_bUsedTop = true; }
   void IncrementBottomTestResources()           { m_iTestResourceCountBot++;   m_bUsedBot = true; }
   void IncrememtBottomPowerInjectionResources() { m_iPwrInjResourceCountBot++; m_bUsedBot = true; }
};
typedef CArray<CProbeCount> CProbeCountArray;

/******************************************************************************
* CPolyListItem
*/
class CPolyListItem
{
public:
   CPolyListItem() {}
   CPolyListItem(CPolyListItem &listItem);
   CPolyListItem(CPolyList *polyList, ETestSurface surface);
   ~CPolyListItem();

private:
   ETestSurface m_eSurface;
   CPolyList m_pPolyList;

public:
   ETestSurface GetSurface()                           const { return m_eSurface;                                  }
   CPolyList &GetPolyList()                                  { return m_pPolyList;                                 }
   double DistanceTo(const CPoint2d location)          const { return m_pPolyList.distanceTo(location);            }
   bool IsPointInside(const CPoint2d location,PageUnitsTag pageUnits) const { return m_pPolyList.isPointInside(location,pageUnits);         }
   void Transform(const CTMatrix& transformationMatrix)      { return m_pPolyList.transform(transformationMatrix); }
};

class CPolyListItemList : public CList<CPolyListItem>
{
};

/******************************************************************************
* CPPProbeResult
*/
class CPPProbeResult
{
public:
   CPPProbeResult(ETestResourceType resType, CString netName, CString accessName, int accessEntityNum, CString templateName, CString probeRefname, double locX = DBL_MIN, double locY = DBL_MIN);
   CPPProbeResult(const CPPProbeResult& other);
   ~CPPProbeResult();

private:
   ETestResourceType m_eResourceType;
   CString m_sNetName;
   CString m_sAccessName;
   CString m_sProbeTemplateName;
   CString m_sProbeRefname;
   int m_nAccessEntityNum;

   double m_locX;
   double m_locY;

public:
   ETestResourceType GetResourceType()         const { return m_eResourceType;      }
   CString GetNetName()                        const { return m_sNetName;           }
   void SetNetName(CString nn)                  { m_sNetName = nn; }
   CString GetAccessName()                     const { return m_sAccessName;        }
   CString GetProbeTemplateName()              const { return m_sProbeTemplateName; }
   CString GetProbeRefname()                   const { return m_sProbeRefname;      }
   int GetAccessEntityNum()                    const { return m_nAccessEntityNum;   }   // Not saved, used during probe placement processing

   CPoint2d GetLocation()                      const { return CPoint2d(m_locX, m_locY); }

   static CPPProbeResult * LoadXML(CXMLNode *node);
};

class CPPProbeResultList: public CTypedPtrListContainer<CPPProbeResult*>
{
public:
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

/******************************************************************************
* CPPNetResult
*/
class CPPNetResult
{
public:
   CPPNetResult(CString netName, bool noProbe = true);  // True makes this backward compatible, not clear if anything depends on this default.
   CPPNetResult(const CPPNetResult& other);
   ~CPPNetResult();

private:
   CString m_sNetName;
   bool m_bNoProbe;
   CPPProbeResultList m_probeResultList;

public:
   CString GetNetName()                  const { return m_sNetName;    }
   bool IsNoProbe()                      const { return m_bNoProbe;    }
   void SetNoProbe(bool const noProbe)         { m_bNoProbe = noProbe; }
   
   POSITION GetProbeHeadPosition()             { return m_probeResultList.GetHeadPosition(); }
   POSITION GetProbeTailPosition()             { return m_probeResultList.GetTailPosition(); }
   CPPProbeResult* GetProbeNext(POSITION &pos) { return m_probeResultList.GetNext(pos);      }
   CPPProbeResult* GetProbePrev(POSITION &pos) { return m_probeResultList.GetPrev(pos);      }
   CPPProbeResult* GetProbeHead()              { return m_probeResultList.GetHead();         }
   CPPProbeResult* GetProbeTail()              { return m_probeResultList.GetTail();         } 

   CPPProbeResult* AddProbeResult(ETestResourceType resType, CString netName, CString accessName, int accessEntityNum, CString templateName, CString probeRefname);
   CPPProbeResult* AddProbeResult(CPPProbeResult *ppRes);

   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);
};

class CPPNetResultList: public CTypedPtrListContainer<CPPNetResult*>
{
public:
   CPPNetResult *GetResultForNet(CString netName);
};


//******************************************************************************

// For alloating probe names aka probe numbers where Power nets get P1, P2, P3, ...,
// Ground nets get G1, G2, G3, ..., and regular signal nets get 1, 2, 3, ...

class CProbeNamer
{
private:
   int m_normalProbeNum;
   int m_powerProbeNum;
   int m_groundProbeNum;
   CStringArray m_offlimits;

public:
   CProbeNamer()  { m_normalProbeNum = 1; m_powerProbeNum = 1; m_groundProbeNum = 1; }
   CProbeNamer& operator=(const CProbeNamer &otherNamer);

   void Reset(int initialNormalProbeNum = 1, int initialPowerProbeNum = 1, int initialGroundProbeNum = 1)
   { m_normalProbeNum = initialNormalProbeNum; m_powerProbeNum = initialPowerProbeNum; 
   m_groundProbeNum = initialGroundProbeNum; m_offlimits.RemoveAll(); }

   void Strike(CString name)  { if (!name.IsEmpty()) m_offlimits.Add(name); }
   bool IsStricken(CString name);

   CString GetNextProbeRefname(CPPNet *net, ETestResourceType resourceType);
};

/******************************************************************************
* CProbePlacementSolution
*/
class CProbePlacementSolution
{
public:
   CProbePlacementSolution(CCEtoODBDoc *doc, CTestPlan *testPlan, FileStruct *file=NULL);
   CProbePlacementSolution(CProbePlacementSolution &ppSolution, CTestPlan *testPlan);
   ~CProbePlacementSolution();

   void ApplyToPcb(FileStruct *pcbFile);

   virtual void DumpToFile(CFormatStdioFile &file, int indent);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);

protected:
   CCEtoODBDoc *m_pDoc;
   FileStruct *m_pFile;
   CTestPlan *m_pTestPlan;
   CPPNetMap m_ppNets;
   CPolyListItemList m_polyList;
   CPPNetResultList m_netResultList;

   CProbeNamer m_probeNamer;

   bool haveTopProbes;
   bool haveBotProbes;
   CProgressDlg *m_pProgressDlg;

   CString m_sErrMsg;
   CAccessibleLocationTree m_LocationTreeTop;
   CAccessibleLocationTree m_LocationTreeBot;

   void applyNoProbeFeatures(CDFTPreconditions &Preconditions);
   void createForcedAccessLocations(CDFTPreconditions &Preconditions);
   void applyForcedFeatures(CDFTPreconditions &Preconditions);
   void applyPreconditions(CDFTPreconditions &Preconditions);

   void calculateNeededProbes();

   void gatherPackageOutlines();
   virtual void addAllNets();
   void gatherAccesibleLocations();
   void createProbes();
   void createAdditionalProbes();

   void gatherAndMatchProbes();
   void gatherPlacedProbes();

   CPPAccessibleLocation* getAccessLocationByTargetPriority(CPPNet *ppNet, ETestSurface favored);
   CPPAccessibleLocation* getAvailableAccessLocationOnSurface(CPPNet *ppNet, ETestSurface favored);
   bool anyProbeUsesComponentOutline();
   void placeProbes();
   void removeExcessUnplacedProbes();
   void placeProbesOnAccessPoints();
   
   // getProbeDensity() assumes homogeneous probe sizes
   int getProbeDensity(CPoint2d point, double diameter, ETestSurface surface, CAccessibleLocationList &foundList);
   // getActualProbeDensity() considers sizes of probes being examined
   int getActualProbeDensity(CPoint2d point, double diameter, ETestSurface surface, CAccessibleLocationList &foundList);
   
   int getTargetTypePriority(CPPAccessibleLocation *accLoc);
   bool isValidTargetType(CPPAccessibleLocation *accLoc);
   bool isTooCloseToComponentOutline(CPPAccessibleLocation *accLoc, double minDistToOutline);
   double findDistanceToClosestComponentOutline(CPPAccessibleLocation *accLoc);
   void resetAccessPointDistances();
   int  optimizeProbeLocations(double diameter, double minDistToOutline, bool removeProbeIfNeeded);
   void optimizeProbeLocationsOnSide(ETestSurface surface);
   CDFTProbeTemplate *getOptimalProbeSize(CPPAccessibleLocation *accLoc);
   void getOptimalProbeCombination(CPPAccessibleLocation *accLoc1, CPPAccessibleLocation *accLoc2, CDFTProbeTemplate *&template1, CDFTProbeTemplate *&template2);
   int maximizeProbeSizes();
   int resurrectNonViolatingUnplacedProbes();
   int lastChanceMaximizeProbeSize(ETestSurface surface);
   int fixViolatingProbeTemplates();
   int removeTemplatesFromProbes();
   int removeProbesWithNoTemplate();
   void setSmallestProbeSizes();
   void doubleWireRemainingProbes();

   void destroyProgress();

   POSITION GetHeadPosition_Probes(CString netName) const;
   POSITION GetTailPosition_Probes(CString netName) const;
   CPPProbe *GetHead_Probes(CString netName) const;
   CPPProbe *GetTail_Probes(CString netName) const;
   CPPProbe *GetNext_Probes(CString netName, POSITION &pos) const;   
   CPPProbe *GetPrev_Probes(CString netName, POSITION &pos) const;

   POSITION GetHeadPosition_AccessibleLocations(CString netName) const;
   POSITION GetTailPosition_AccessibleLocations(CString netName) const;
   CPPAccessibleLocation *GetHead_AccessibleLocations(CString netName) const;
   CPPAccessibleLocation *GetTail_AccessibleLocations(CString netName) const;
   CPPAccessibleLocation *GetPrev_AccessibleLocations(CString netName, POSITION &pos) const;

   CPPAccessibleLocation *GetNext_AccessibleLocations(CString netName, POSITION &pos, EEntityType featureType = (EEntityType)-1, CString targetType = "", ETestSurface surface = testSurfaceUnset) const;

   CPPProbe *AddHead_Probes(CString netName, ETestResourceType resType, CString probeRefname = "");
   CPPProbe *AddTail_Probes(CString netName, ETestResourceType resType, CString probeRefname = "");
   void RemoveAll_Probes(CString netName = "");

   CPPAccessibleLocation *AddHead_AccessibleLocations(DataStruct *testAccessData);
   CPPAccessibleLocation *AddTail_AccessibleLocations(DataStruct *testAccessData);
   CPPAccessibleLocation *AddTail_AccessibleLocations(CString netName,const CEntity& featureEntity, CPoint2d location, CString targetType, ETestSurface surface);
   void RemoveAt_AccessibleLocations(CString netName);
   void RemoveAll_AccessibleLocations(CString netName = "");

   void RemoveAll_Nets();

public:
   CProbePlacementSolution& operator=(const CProbePlacementSolution &ppSolution);

   void SetCurrentTestplan(CTestPlan *testPlan)       { m_pTestPlan = testPlan;     }

   CString GetErrorMessage()                          { return m_sErrMsg;           }
   void ShowProgress();
   void HideProgress();

   int RunProbePlacement();

   void RenumberProbes();
   void ReservePreexistingProbeRefnames();

   long PlaceProbesOnBoard();
   long PlaceProbesResultOnBoard(CMapWordToPtr& accessPointMap);
   int ValidateAndPlaceProbes();
   void GetProbeCounts(int &topPlaced, int &botPlaced, int &totUnplaced, int &forcedInactiveSide, CProbeCountArray &countArray,
      int &netsInvolved, int &netsFullyProbed, int &netsPartiallyProbed, int &netsNotProbed);
   bool CreateReport(CString Filename, CAccessAnalysisSolution *aaSolution);
   void WriteUnplacedProbePlacement(CFormatStdioFile &file, CAccessAnalysisSolution *aaSolution, EIncludedNetType netType);
   void WritePlacedProbePlacement(CFormatStdioFile &file, CAccessAnalysisSolution *aaSolution, EIncludedNetType netType);
   void WriteNetAccessInfo(CFormatStdioFile &file, CAANetAccess *netAcc);
   void WriteNetAccessInfo(CFormatStdioFile &file, CPPNet *ppNet);

   // debugging function
   void DrawOutlines();
};

/////////////////////////////////////////////////////////////////////////////
// CDFTSolution
/////////////////////////////////////////////////////////////////////////////
class CDFTSolution
{
private:
   CString m_name;
   bool m_flipped;
   CTestPlan m_TestPlan;
   CAccessAnalysisSolution *m_aaSolution;
   CProbePlacementSolution *m_ppSolution;

   void dumpAAToFile(CFormatStdioFile &file, int indent);
   void dumpPPToFile(CFormatStdioFile &file, int indent);

public:
   CDFTSolution(CString name, bool flipped, PageUnitsTag pageUnits);
   ~CDFTSolution();

   void DestroySolutions();
   void CopyDFTSolution(const CDFTSolution& copyFromSolution);

   //CDFTSolution& operator=(const CDFTSolution &dftSolution);
   bool DumpToFile(CString Filename);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node, CCEtoODBDoc *doc, FileStruct *file);
   void Scale(double factor);

   CString GetName()                      const { return m_name;           }
   bool IsFlipped()                       const { return m_flipped;        }
   CTestPlan *GetTestPlan()                     { return &m_TestPlan;      }
   void SetTestPlan(const CTestPlan& testPlan)  { m_TestPlan = testPlan;   }

   CAccessAnalysisSolution *GetAccessAnalysisSolution();
   CAccessAnalysisSolution *CreateAccessAnalysisSolution(CCEtoODBDoc *doc, FileStruct *PCBFile);
   CProbePlacementSolution *GetProbePlacementSolution();
   CProbePlacementSolution *CreateProbePlacementSolution(CCEtoODBDoc *doc);
};

class CDFTSolutionList : public CTypedPtrListContainer<CDFTSolution*>
{
private:
   int m_probeNumberOffset; // Offset to use for probes in panel DFT solution.

   int LoadXML_OneDftSolution(CXMLNode *node, CCEtoODBDoc *doc, FileStruct *file);

public:
   CDFTSolutionList()   { m_probeNumberOffset = -1; } // -1 means not set.

   CDFTSolution* AddNewDFTSolution(CString name, bool isFlipped, PageUnitsTag pageUnits);
   CDFTSolution* FindDFTSolution(CString name, bool isFlipped);

   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXMLFile(CString xmlFileName, CCEtoODBDoc *doc, FileStruct *file);
   int LoadXML(CString xmlString, CCEtoODBDoc *doc, FileStruct *file);
   
   void Scale(double factor);

   int GetProbeNumberOffset()             { return m_probeNumberOffset; }
   void SetProbeNumberOffset(int pno)     { m_probeNumberOffset = pno;  }
};

/////////////////////////////////////////////////////////////////////////////
// CTestProbeData
/////////////////////////////////////////////////////////////////////////////
enum TestProbePinType
{
   TestProbePinTypeComppin,
   TestProbePinTypeVia
};

typedef struct
{
   double			x;
   double			y;
	TestProbePinType	pintype;
   ETestSurface   pcbside;
   CString			netname;
   CString			refname;
} TTestPoints;

class CTestProbeData
{
private:
   TestProbePinType	m_pinType;  // ProbeTestPinTypeComppin or ProbeTestPinTypeVia
   CString			m_refname;
   CString			m_netname;
   ETestSurface	m_pcbside;

   // Pin name used for comppins only
   CString			m_compname;
   CString			m_pinname;

   // Location used for vias only
   double		m_x; 
   double		m_y;

public:
   CTestProbeData(CString refname, CString netname, ETestSurface   pcbside, double x, double y); //via 
   CTestProbeData(CString refname, CString netname, ETestSurface   pcbside, CString compname, CString piname, 
		double x, double y); //comppin

   inline TestProbePinType GetTestPinType()     { return m_pinType; }
   inline CString GetRefname()                  { return m_refname; }
   inline CString GetNetname()                  { return m_netname; }
   inline CString GetCompname()                 { return m_compname; }
   inline CString GetPinname()                  { return m_pinname; }
   inline ETestSurface GetPcbside()             { return m_pcbside; }
   inline double GetX()                         { return m_x; }
   inline double GetY()                         { return m_y; }
};

class CTestProbeDataList : public CTypedPtrListContainer<CTestProbeData*>
{
private:
   CViaListMap m_ViaListMap;
   double   m_threadshold;

public:
   CTestProbeDataList();
   ~CTestProbeDataList();
   void ClearAllData();
   void RemoveTestData();
   void RemoveViaList();

private:
   DataStruct *findViaAt(FileStruct *pcbfile, CPoint2d location);
   DataStruct *findViaAt(CCEtoODBDoc *doc, FileStruct *pcbfile, CTestProbeData *testprobedata);
   DataStruct *findViabyMapAt(FileStruct *pcbfile, CTestProbeData *testprobedata);
   CompPinStruct *findCompPinAt(FileStruct *pcbfile, CTestProbeData *testprobedata);
   
public:
   void Add(CString refname, CString netname, ETestSurface   pcbside, double x, double y); //via
   void Add(CString refname, CString netname, ETestSurface   pcbside, CString compname, CString piname,
		double x, double y);
   void Add(TTestPoints &testpoint);
   void MarkProbeTestPointsInCamcad(CCEtoODBDoc *doc, FileStruct *pcbFile, CStringArray *logMessages);
   void MarkViaAndComPinProbeTest(CCEtoODBDoc *doc, FileStruct *pcbFile, CStringArray *logMessages);

   CViaListMap& getViaListMap(){return m_ViaListMap;}
   void BuildViaListMap(CCEtoODBDoc *doc, FileStruct *pcbfile);
};

/////////////////////////////////////////////////////////////////////////////
// CProbeTypeConversion
/////////////////////////////////////////////////////////////////////////////
enum ProbeTypeTag
{
   ProbeTypeTag_UNKNOWN = -1,
   ProbeTypeTag_50 = 0,
   ProbeTypeTag_75,
   ProbeTypeTag_100,
   ProbeTypeTag_Max
};

class CProbeTypeConversion
{
private:
   CString m_probeName;   
   ProbeTypeTag m_ProbeSize;
   ETestSurface m_PCBSide;
   double m_unitFactor;

public:
   CProbeTypeConversion(CString probeName,CCEtoODBDoc *doc);
   CProbeTypeConversion(CCEtoODBDoc *doc);

   CString &getProbeName(){ return m_probeName;}
   ProbeTypeTag getProbeType(){return m_ProbeSize;}
   ETestSurface getPCBSide(){ return m_PCBSide;}

   double getProbeDrillSize(ProbeTypeTag probeType);
   double getDiamSize(ProbeTypeTag probeType);
   ProbeTypeTag getProbeType(CString probeName);
   ETestSurface getPCBSide(CString probeName);
};

//----------------------------------------------------------------------------------

class CEntityNumberMap : public CMap<int, int, int, int>
{
};

//------------------

typedef CTypedPtrArray<CPtrArray, DataStruct*> PcbSequenceArray;

//------------------

class CSuperBoard
{
private:
   CCEtoODBDoc &m_doc;
   CCamCadData &m_camcadData;

   FileStruct *m_panelFile;  // Original incoming panel that superboard is based on.
   FileStruct *m_superBoard; // The super board we build.
   CDFTSolution *m_superBoardDftSln; // The dft solution for superboard.

   bool m_probeOffsetTooSmall;        // Per Mark - Probe offset should be greater than or equal to highest probe number in base PCA solutions.
   int m_addedProbeCount;             // Keeps track of number of probes added to superboard.
   int m_activeProbeNumberOffset;     // Offset to apply during processing of a PCB.
   int m_userProbeNumberOffset;       // The user's offset setting.
   int m_highestOriginalProbeNumber;  // Highest probe number considering all PCAs in Panel.
   int m_highestProbePlacedSoFar;     // Highest probe plac... well, what the name says.

   int CalcProbeNumberOffset(int pcaSeqId, BlockStruct *pcbBlock);

   CString m_pcbInstancePrefix;
   CString GetPcbInstancePrefix(int pcbNumber);
   CString AdjustRefname(int pcbNum, CString originalRefname);
   CString AdjustRefname(int pcbNum, DataStruct *insertData);
   CString AdjustAttrib(int pcbNum, CAttributes *attributes, StandardAttributeTag attribTag);
   void FlipMirrorLayers(int pcbNum, DataStruct *anyData);

   void    AdjustDatalink(DataStruct *anyData, CEntityNumberMap &entityNumberMap);

   int CSuperBoard::GetLowestProbeNumber(BlockStruct *pcbBlock);
   int CSuperBoard::GetHighestProbeNumber(BlockStruct *pcbBlock);

   void FillPcbSequenceArray(BlockStruct *panelBlock, PcbSequenceArray &pcbArray);
   void DetermineHighestOriginalProbeNumber();
   void CopyNonPcbInsertData();
   void FlattenAllPcbs();
   void AdjustOutlines(); // Upper level
   void AdjustOutlines(BlockStruct *block, GraphicClassTag oldGc, GraphicClassTag newGc);  // Low level worker function.
   void FlattenOnePcb(InsertStruct *pcbInsert, int &pcbCount);
   void CopyData(int pcbNum, InsertStruct *pcbInsert, CEntityNumberMap &entityNumberMap);
   void CopyNetlist(int pcbNum, BlockStruct *pcbBlock, CEntityNumberMap &entityNumberMap);
   void CopyAccessAndProbes(int pcbNum, InsertStruct *pcbInsert, CEntityNumberMap &entityNumberMap);

public:
   CSuperBoard(CCEtoODBDoc &doc, FileStruct *panelFile);

   FileStruct *GetSuperBoard() { return m_superBoard; }

   bool HasProbeOffsetError()          { return m_probeOffsetTooSmall; }
   int GetHighestOriginalProbeNumber() { return m_highestOriginalProbeNumber; } 
   int GetUserProbeNumberOffset()      { return m_userProbeNumberOffset; }

};

#endif // !defined(__Dft_h__)
