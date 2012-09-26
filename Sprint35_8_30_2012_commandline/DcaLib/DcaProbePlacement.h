// $Header: /CAMCAD/DcaLib/DcaProbePlacement.h 4     6/29/07 12:22p Lynn Phung $

#if !defined(__DcaProbePlacement_h__)
#define __DcaProbePlacement_h__

#pragma once

#include "DcaEntityWithAttributes.h"

class CPPProbe;
class CCamCadData;
class DataStruct;
class CPoint2d;
class CFormatStdioFile;

enum ETestSurface;

//_____________________________________________________________________________
class CPPAccessibleLocation
{
public:
   CPPAccessibleLocation(CCamCadData& camCadData, DataStruct *testAccessData = NULL);
   CPPAccessibleLocation::CPPAccessibleLocation(CCamCadData& camCadData,const CEntity& featureEntity, CPoint2d location, CString targetType, CString netName, ETestSurface surface);
   //CPPAccessibleLocation(CCamCadData& camCadData, EEntityType featureType, void *feature, CPoint2d location, CString targetType, CString netName, ETestSurface surface);
   CPPAccessibleLocation(CPPAccessibleLocation &accessibleLocation);
   ~CPPAccessibleLocation();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);

private:
   CCamCadData& m_camCadData;
 //EEntityType m_eFeatureType;
 //void *m_pFeature;
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

//_____________________________________________________________________________
class CPPAccessibleLocationList : public CTypedPtrList<CPtrList, CPPAccessibleLocation*>
{
};

//_____________________________________________________________________________
class CPPProbe
{
public:
   CPPProbe(CCamCadData& camCadData, ETestResourceType resType, CString netName);
   CPPProbe(CPPProbe &probe);
   ~CPPProbe();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);

protected:
   CCamCadData& m_camCadData;
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

//_____________________________________________________________________________
class CPPProbeList: public CTypedPtrList<CPtrList, CPPProbe*>
{
public:
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
class CPPNet : public CObject
{
public:
   CPPNet(CCamCadData& camCadData, CString netName = "");
   CPPNet(CPPNet &ppNet);
   ~CPPNet();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);

protected:
   CString m_sNetName;
   eProbeableNetType m_eNetType;
   bool m_bNoProbe;
   int m_iTestResourcesNeeded;
   int m_iPwrInjResourcesNeeded;

   CCamCadData& m_camCadData;
   NetStruct *m_pNet;
   CPPProbeList m_probes;                                // list of all probes for memory management
   CPPAccessibleLocationList m_accessibleLocations;      // list of all accessible locations for memory management

public:
   CPPNet& operator=(const CPPNet &ppNet);

   NetStruct *GetNet()                                                     { return m_pNet;                             }
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

//_____________________________________________________________________________
class CPPNetMap : public CMapSortedStringToOb<CPPNet>
{
public:
   static int AscendingNetNameSortFunc(const void *a, const void *b);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
class CQfeAccessibleLocation : public CQfe
{
private:
   CPPAccessibleLocation *m_pAccLocation;

public:
   CQfeAccessibleLocation();
   CQfeAccessibleLocation(CPPAccessibleLocation *accessiblelocation);
   ~CQfeAccessibleLocation();

public:
   virtual CPoint2d getOrigin() const;
   virtual CExtent getExtent() const;
   virtual CString getInfoString() const;

   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType();

   CPPAccessibleLocation *GetAccessibleLocation();
};

//_____________________________________________________________________________
class CAccessibleLocationList : public CTypedPtrList<CQfeList,CQfeAccessibleLocation*>
{
public:
   CAccessibleLocationList(int nBlockSize=200);
};

//_____________________________________________________________________________
class CAccessibleLocationTree : public CQfeExtentLimitedContainer
{
public:
   CAccessibleLocationTree();
   ~CAccessibleLocationTree();

public:
   virtual int search(const CExtent& extent,CAccessibleLocationList& foundList);
   virtual void setAt(CQfeAccessibleLocation* accessiblelocation);
};

//_____________________________________________________________________________
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

//_____________________________________________________________________________
class CProbeCountArray : public CArray<CProbeCount>
{
};

//_____________________________________________________________________________
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
   bool IsPointInside(const CPoint2d location)         const { return m_pPolyList.isPointInside(location);         }
   void Transform(const CTMatrix& transformationMatrix)      { return m_pPolyList.transform(transformationMatrix); }
};

//_____________________________________________________________________________
class CPolyListItemList : public CList<CPolyListItem>
{
};

//_____________________________________________________________________________
class CPPProbeResult
{
public:
   CPPProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, CString probeRefname);
   //CPPProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, long probeNumber);
   CPPProbeResult(const CPPProbeResult& other);
   ~CPPProbeResult();

private:
   ETestResourceType m_eResourceType;
   CString m_sNetName;
   CString m_sAccessName;
   CString m_sProbeTemplateName;
   CString m_sProbeRefname;

public:
   ETestResourceType GetResourceType()         const { return m_eResourceType;      }
   CString GetNetName()                        const { return m_sNetName;           }
   CString GetAccessName()                     const { return m_sAccessName;        }
   CString GetProbeTemplateName()              const { return m_sProbeTemplateName; }
   CString GetProbeRefname()                   const { return m_sProbeRefname;      }

   void SetAccessName(CString const accessName)      { m_sAccessName = accessName;  }
};

//_____________________________________________________________________________
class CPPProbeResultList: public CTypedPtrListContainer<CPPProbeResult*>
{
public:
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
class CPPNetResult
{
public:
   CPPNetResult(CString netName);
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

   CPPProbeResult* AddProbeResult(ETestResourceType resType, CString netName, CString accessName, CString templateName, CString probeRefname);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);
};

//_____________________________________________________________________________
class CPPNetResultList: public CTypedPtrListContainer<CPPNetResult*>
{
};

//_____________________________________________________________________________
class CProbePlacementSolution
{
public:
   CProbePlacementSolution(CCamCadData& camCadData, CTestPlan *testPlan, FileStruct *file=NULL);
   CProbePlacementSolution(CProbePlacementSolution &ppSolution, CTestPlan *testPlan);
   ~CProbePlacementSolution();

   virtual void DumpToFile(CFormatStdioFile &file, int indent);
   virtual void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);

protected:
   CCamCadData& m_camCadData;
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
   //CDFTProbeTemplate *getOptimalProbe(CPPAccessibleLocation *pAccLoc, int &totalCost);

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
   long PlaceProbesResultOnBoard(CMapStringToPtr& accessPointMap);
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

#endif
