// $Header: /CAMCAD/5.0/Dca/DcaDft.h 5     3/19/07 4:33a Kurt Van Ness $

#if !defined(__DcaDft_h__)
#define __DcaDft_h__

#pragma once

#include "DcaEntity.h"
#include "DcaPoint2d.h"
#include "DcaSortedMap.h"
#include "DcaContainer.h"

class CDcaTestPlan;

enum ETestSurface;
enum ETestResourceType;
enum PageUnitsTag;

//_____________________________________________________________________________
class CDcaTestPlan
{
private:

public:
   CDcaTestPlan();
   ~CDcaTestPlan();
};

//_____________________________________________________________________________
class CDcaAAAccessLocation : public CObject
{
public:
   CDcaAAAccessLocation();
   ~CDcaAAAccessLocation();

private:
   ETestSurface m_eMountSurface;       // surface of the feature
   ETestSurface m_eAccessSurface;      // surface of the access point, can be diff erence than surface of the feature
   CEntity m_featureEntity;
   double m_dExposedMetalDiameter;
   bool m_bAccessable;
   CString m_sName;
   CPoint2d m_ptAccessLocation;
};

//_____________________________________________________________________________
class CDcaAAAccessLocationList : public CTypedMapSortStringToObContainer<CDcaAAAccessLocation>
{
};

//_____________________________________________________________________________
class CDcaAANetAccess : public CObject
{
public:
   CDcaAANetAccess();
   ~CDcaAANetAccess();

private:
   ETestSurface m_eSurface;
   CDcaAAAccessLocationList m_accessibleLocationList;
};

//_____________________________________________________________________________
class CDcaAANetAccessMap : public CMapSortedStringToOb<CDcaAANetAccess>
{
};

//_____________________________________________________________________________
class CDcaAccessAnalysisSolution
{
private:
   CDcaAANetAccessMap m_netAccessMap;

public:
   CDcaAccessAnalysisSolution();
   ~CDcaAccessAnalysisSolution();
};

//_____________________________________________________________________________
class CDcaPPProbeResult
{
public:
   CDcaPPProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, long probeNumber);
   ~CDcaPPProbeResult();

private:
   ETestResourceType m_eResourceType;
   CString m_sNetName;
   CString m_sAccessName;
   CString m_sProbeTemplateName;
   long m_lProbeNumber;

public:
   ETestResourceType GetResourceType()         const { return m_eResourceType;      }
   CString GetNetName()                        const { return m_sNetName;           }
   CString GetAccessName()                     const { return m_sAccessName;        }
   CString GetProbeTemplateName()              const { return m_sProbeTemplateName; }
   long GetProbeNumber()                       const { return m_lProbeNumber;       }

   void SetAccessName(CString const accessName)      { m_sAccessName = accessName;  }
};

//_____________________________________________________________________________
class CDcaPPProbeResultList: public CTypedPtrListContainer<CDcaPPProbeResult*>
{
};

//_____________________________________________________________________________
class CDcaPPNetResult
{
public:
   CDcaPPNetResult();
   ~CDcaPPNetResult();

private:
   CString m_sNetName;
   bool m_bNoProbe;
   CDcaPPProbeResultList m_probeResultList;

public:
   CString GetNetName()                  const { return m_sNetName;    }
   bool IsNoProbe()                      const { return m_bNoProbe;    }
   void SetNoProbe(bool const noProbe)         { m_bNoProbe = noProbe; }
   
   //POSITION GetProbeHeadPosition()             { return m_probeResultList.GetHeadPosition(); }
   //POSITION GetProbeTailPosition()             { return m_probeResultList.GetTailPosition(); }
   //CPPProbeResult* GetProbeNext(POSITION &pos) { return m_probeResultList.GetNext(pos);      }
   //CPPProbeResult* GetProbePrev(POSITION &pos) { return m_probeResultList.GetPrev(pos);      }
   //CPPProbeResult* GetProbeHead()              { return m_probeResultList.GetHead();         }
   //CPPProbeResult* GetProbeTail()              { return m_probeResultList.GetTail();         } 

   //CPPProbeResult* AddProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, long probeNumber);
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   //int LoadXML(CXMLNode *node);
};

//_____________________________________________________________________________
class CDcaPPNetResultList: public CTypedPtrListContainer<CDcaPPNetResult*>
{
};

//_____________________________________________________________________________
class CDcaProbePlacementSolution
{
private:
   CDcaPPNetResultList m_netResultList;

public:
   CDcaProbePlacementSolution();
   ~CDcaProbePlacementSolution();
};

//_____________________________________________________________________________
class CDcaDftSolution
{
private:
   CString m_name;
   CDcaTestPlan m_testPlan;
   CDcaAccessAnalysisSolution *m_accessAnalysisSolution;
   CDcaProbePlacementSolution *m_probePlacementSolution;

   //void dumpAAToFile(CFormatStdioFile &file, int indent);
   //void dumpPPToFile(CFormatStdioFile &file, int indent);

public:
   CDcaDftSolution(CString name,PageUnitsTag pageUnits);
   ~CDcaDftSolution();

   //void DestroySolutions();
   //void CopyDFTSolution(const CDFTSolution& copyFromSolution);

   //CDFTSolution& operator=(const CDFTSolution &dftSolution);
   //bool DumpToFile(CString Filename);
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   //int LoadXML(CXMLNode *node, CCEtoODBDoc *doc, FileStruct *file);
   //void Scale(double factor);

   CString GetName()                      const { return m_name;           }
   CDcaTestPlan *GetTestPlan()                  { return &m_testPlan;      }

   const CDcaTestPlan& getTestPlan() const { return m_testPlan; }

   void SetTestPlan(const CDcaTestPlan& testPlan)  { m_testPlan = testPlan;   }

   CDcaAccessAnalysisSolution* getAccessAnalysisSolution() const { return m_accessAnalysisSolution; }
   CDcaProbePlacementSolution* getProbePlacementSolution() const { return m_probePlacementSolution; }

   //CAccessAnalysisSolution* GetAccessAnalysisSolution();
   //CAccessAnalysisSolution* CreateAccessAnalysisSolution(CCEtoODBDoc *doc, FileStruct *PCBFile);
   //CProbePlacementSolution* GetProbePlacementSolution();
   //CProbePlacementSolution* CreateProbePlacementSolution(CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CDcaDftSolutionList : public CTypedPtrListContainer<CDcaDftSolution*>
{
};

#endif
