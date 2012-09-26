// $Header: /CAMCAD/5.0/AccessAnalysis.h 6     3/09/07 9:43p Kurt Van Ness $

#if !defined(_AccessAnalysis_h__)
#define _AccessAnalysis_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccdoc.h"
#include "Point2d.h"
#include "find.h"
#include "Region.h"
#include "xmldomwrapper.h"
#include "DFTShared.h"
#include "Sm_Anal.h"

class CTestPlan;

enum NetAccessTypeTag
{
   netAccessAll,
   netAccessNonConnectedOnly,
	netAccessExcludeNonConnected,
};

enum ExposedMetalTypeTag
{
   exposedMetalOriginal,
   exposedMetalByBoardOutline,
   exposedMetalByComponentOutline,
   exposedMetalByBoardComponentOutline,
   exposedMetalBySolder,
   exposedMetalBySolderBoardOutline,
   exposedMetalBySolderComponentOutline,
   exposedMetalBySolderBoardComponentOutline,
};

enum MinProbeSizeTag
{
   MinProbeSizeFeature,
   MinProbeSizeBeadProbe,
   MinProbeSizeLast
};

//-----------------------------------------------------------------------------
// CHeightRange
//-----------------------------------------------------------------------------
class CHeightRange
{
public:
   CHeightRange(CString name, double outlineDistance, double minHeight, double maxHeight)
   {
      m_sName = name;
      m_dOutlineDistance = outlineDistance;
      m_dMinHeight = minHeight;
      m_dMaxHeight = maxHeight;
   }
   CHeightRange(const CHeightRange& other)
   {
      *this = other;
   }
   ~CHeightRange(){}

   CHeightRange& operator=(const CHeightRange &other)
   {
      if (&other != this)
      {
         m_sName = other.m_sName;
         m_dOutlineDistance = other.m_dOutlineDistance;
         m_dMinHeight = other.m_dMinHeight;
         m_dMaxHeight = other.m_dMaxHeight;
      }

      return *this;
   }

private:
   CString m_sName;
   double m_dOutlineDistance;
   double m_dMinHeight;
   double m_dMaxHeight;    // Max height is set to -1 or less than zero to indicate infinity
 
public:
   CString GetName()           const { return m_sName;             }
   double GetOutlineDistance() const { return m_dOutlineDistance;  }
   double GetMinHeight()       const { return m_dMinHeight;        }
   double GetMaxHeight()       const { return m_dMaxHeight;        }

   void SetName(const CString name)                { m_sName = name;                }
   void SetOutlineDistance(const double distance)  { m_dOutlineDistance = distance; }
   void SetMinHeight(const double height)          { m_dMinHeight = height;         }
   void SetMaxHeight(const double height)          { m_dMaxHeight = height;         }
};

//-----------------------------------------------------------------------------
// CHeightAnalysis
//-----------------------------------------------------------------------------
class CHeightAnalysis : private CTypedMapStringToPtrContainer<CHeightRange*>
{
public:
   CHeightAnalysis(PageUnitsTag pageUnitsTag);
   ~CHeightAnalysis();

   CHeightAnalysis& operator=(const CHeightAnalysis& other);

private:
   bool m_bModified;
   bool m_bUse;
   PageUnitsTag m_pageUnitsTag;

public:
   void SetDefault(PageUnitsTag pageUnits);
   void Scale(double factor);

   bool IsModified()                      const { return m_bModified;      }
   void ResetModified()                         { m_bModified = false;     }
   void SetModified(const bool modified)        { m_bModified = modified;  }

   bool IsUse()                           const { return m_bUse;           }
   void SetUse(const bool isUse);

   void AddHeightRange(CHeightRange* heightRange);
   CHeightRange* AddHeightRange(CString name, double outlineDistance, double minHeight, double maxHeight);
   CHeightRange* AddHeightRange(CString name, CString outlineDistance, CString minHeight, CString maxHeight);
   CHeightRange* FindHeightRange(CString name);
   void DeleteHeightRange(CString name);

   CString GetNewHeightName();
   POSITION GetStartPosition() const;
   CHeightRange* GetNext(POSITION& pos) const;
   int GetCount() const;
   void RemoveAllRange();

   double GetPackageOutlineDistance(const double compHeight);
   
   void DumpToFile(CFormatStdioFile &file, int indent);
};

//-----------------------------------------------------------------------------
// CPhysicalConst
//-----------------------------------------------------------------------------
class CPhysicalConst
{
private:
   bool m_bModified;
   bool m_bEnableBoardOutine;
   bool m_bEnableCompOutline;
   bool m_bEnableMinFeatureSize;
   bool m_bEnableBeadProbeSize;
   double m_dValueBoardOutline;
   double m_dValueCompOutline;
   double m_dValueMinFeatureSize;
   double m_dValueBeadProbeSize;

public:
   CPhysicalConst(int pageUnit);

   void DumpToFile(CFormatStdioFile &file, int indent);

   bool IsModified()const              {return m_bModified;}
   bool GetEnableBoardOutline()  const {return m_bEnableBoardOutine;}
   bool GetEnableCompOutline()   const {return m_bEnableCompOutline;}
   bool GetEnableMinFeatureSize()   const {return m_bEnableMinFeatureSize;}
   bool GetEnableBeadProbeSize()   const {return m_bEnableBeadProbeSize;}
   double GetValueBoardOutline() const {return m_dValueBoardOutline;}
   double GetValueCompOutline()  const {return m_dValueCompOutline;}
   double GetValueMinFeatureSize()  const {return m_dValueMinFeatureSize;}
   double GetValueBeadProbeSize()  const {return m_dValueBeadProbeSize;}

   double GetMinValueFeatureSize(int pageUnit) const;
   double GetMinValueBeadProbeSize(int pageUnit) const;

   void ResetModified();
   void SetDefault(int pageUnit);
   void SetEnableBoardOutline(bool enable);  
   void SetEnableCompOutline(bool enable);      
   void SetEnableMinFeatureSize(bool enable);
   void SetEnableBeadProbeSize(bool enable);
   void SetValueBoardOutline(double value);  
   void SetValueCompOutline(double value);      
   void SetValueMinFeatureSize(double value);
   void SetValueBeadProbeSize(double value);

   void SetEnableBoardOutline(BOOL enable)      { SetEnableBoardOutline(enable?true:false);     }
   void SetEnableCompOutline(BOOL enable)       { SetEnableCompOutline(enable?true:false);      }
   void SetEnableMinFeatureSize(BOOL enable)    { SetEnableMinFeatureSize(enable?true:false);   }
   void SetEnableBeadProbeSize(BOOL enable)     { SetEnableBeadProbeSize(enable?true:false);    }
};

//-----------------------------------------------------------------------------
// CTargetType
//-----------------------------------------------------------------------------
class CTargetType
{
private:
   bool m_bModified;
   bool m_bEnableTestAttrib;
   bool m_bEnableVia;
   bool m_bEnableConnector;
   bool m_bEnableSMD;
   bool m_bEnableSinglePinSMD;
   bool m_bEnableTHUR;
   bool m_bEnableCADPadstack;
   CList<int, int> m_padstackList;  // list of geometry number that are used for CAD Padstack   

public:
   CTargetType();
   CTargetType(CTargetType &targetTypes);
   CTargetType& operator=(const CTargetType &targetTypes);

   void DumpToFile(CFormatStdioFile &file, int indent);

   bool IsModified()                         const { return m_bModified;                        }
   bool GetEnableTestAttrib()                const { return m_bEnableTestAttrib;                }
   bool GetEnableVia()                       const { return m_bEnableVia;                       }
   bool GetEnableConnector()                 const { return m_bEnableConnector;                 }
   bool GetEnableSMD()                       const { return m_bEnableSMD;                       }
   bool GetEnableSinglePinSMD()              const { return m_bEnableSinglePinSMD;              }
   bool GetEnableTHUR()                      const { return m_bEnableTHUR;                      }
   bool GetEnableCADPadstack()               const { return m_bEnableCADPadstack;               }
   POSITION GetPadstackHeadPosition()        const { return m_padstackList.GetHeadPosition();   }
   POSITION GetPadstackTailPosition()        const { return m_padstackList.GetTailPosition();   }
   int GetPadstackHead()                     const { return m_padstackList.GetHead();           }
   int GetPadstackTail()                     const { return m_padstackList.GetTail();           }
   int GetPadstackNext(POSITION &pos)        const { return m_padstackList.GetNext(pos);        } 
   int GetPadstackPrev(POSITION &pos)        const { return m_padstackList.GetPrev(pos);        }
   bool HasPadstackInList(int srchNum)       const { POSITION pos = GetPadstackHeadPosition(); 
                                                     while (pos != NULL) 
                                                     { 
                                                        int curNum = GetPadstackNext(pos);
                                                        if (curNum == srchNum)
                                                           return true;
                                                     }
                                                     return false;
                                                    }

   void ResetModified();
   void SetDefault();
   void SetEnableTestAttrib(bool enable);
   void SetEnableTestAttrib(BOOL enable)           { SetEnableTestAttrib(enable?true:false);    }
   void SetEnableVia(bool enable);
   void SetEnableVia(BOOL enable)                  { SetEnableVia(enable?true:false);           }
   void SetEnableConnector(bool enable);
   void SetEnableConnector(BOOL enable)            { SetEnableConnector(enable?true:false);     }
   void SetEnableSMD(bool enable);
   void SetEnableSMD(BOOL enable)                  { SetEnableSMD(enable?true:false);           }
   void SetEnableSinglePinSMD(bool enable);
   void SetEnableSinglePinSMD(BOOL enable)         { SetEnableSinglePinSMD(enable?true:false);  }
   void SetEnableTHUR(bool enable);
   void SetEnableTHUR(BOOL enable)                 { SetEnableTHUR(enable?true:false);          }
   void SetEnableCADPadstack(bool enable);
   void SetEnableCADPadstack(BOOL enable)          { SetEnableCADPadstack(enable?true:false);   }
   void AddPadstackHead(int geomNum);
   void AddPadstackTail(int geomNum);
   void SetPadstackAt(POSITION pos, int geomNum);
   void RemovePadstackAt(POSITION pos);
   void RemovePadstackAll();
};

//-----------------------------------------------------------------------------
// CPrioritizedAccessOffsetDirectionsList
//-----------------------------------------------------------------------------
class CPrioritizedAccessOffsetDirectionsList
{
private:
   CList<AccessOffsetDirectionTag,AccessOffsetDirectionTag> m_list;

public:
   CPrioritizedAccessOffsetDirectionsList();
   CPrioritizedAccessOffsetDirectionsList(const CPrioritizedAccessOffsetDirectionsList& other);

   CPrioritizedAccessOffsetDirectionsList& operator=(const CPrioritizedAccessOffsetDirectionsList& other);
   bool operator==(const CPrioritizedAccessOffsetDirectionsList& other) const;
   bool operator!=(const CPrioritizedAccessOffsetDirectionsList& other) const;

   void empty();
   void addAll();
   void add(AccessOffsetDirectionTag accessOffsetDirection);
   void add(const CPrioritizedAccessOffsetDirectionsList& other);
   void remove(AccessOffsetDirectionTag accessOffsetDirection);
   void remove(const CPrioritizedAccessOffsetDirectionsList& other);
   void setToComplement(const CPrioritizedAccessOffsetDirectionsList& other);
   bool moveTowardsHead(int index);
   bool moveTowardsTail(int index);
   CString getEncodedString() const;
   void setFromEncodedString(const CString& encodedString);
   bool contains(AccessOffsetDirectionTag accessOffsetDirection) const;

   POSITION GetHeadPosition() const;
   AccessOffsetDirectionTag GetNext(POSITION& pos) const;
   int GetCount() const;
};

//-----------------------------------------------------------------------------
// CAccessOffsetItem
//-----------------------------------------------------------------------------
class CAccessOffsetItem
{
private:
   AccessItemTypeTag m_itemType;
   bool m_enableSmdOffsetFlag;
   bool m_enableThOffsetFlag;
   CPrioritizedAccessOffsetDirectionsList m_directionsList;
   AccessOffsetPlacementTag m_smdOffsetPlacement;

   static const CString m_trueString;
   static const CString m_falseString;

public:
   CAccessOffsetItem(AccessItemTypeTag itemType);
   CAccessOffsetItem(const CAccessOffsetItem& other);
   CAccessOffsetItem& operator=(const CAccessOffsetItem& other);
   void initialize();

   bool operator==(const CAccessOffsetItem& other) const;
   bool operator!=(const CAccessOffsetItem& other) const;

   AccessItemTypeTag getItemType() const { return m_itemType; }

   bool getEnableSmdOffsetFlag() const { return m_enableSmdOffsetFlag; }
   void setEnableSmdOffsetFlag(bool flag) { m_enableSmdOffsetFlag = flag; }
   CString getEnableSmdOffsetFlagString() const { return m_enableSmdOffsetFlag ? m_trueString : m_falseString; }
   CString getEnableSmdOffsetString() const { return m_enableSmdOffsetFlag ? m_trueString + "(" +  getSmdOffsetPlacementString() + ")" : m_falseString; }

   bool getEnableThOffsetFlag() const { return m_enableThOffsetFlag; }
   void setEnableThOffsetFlag(bool flag) { m_enableThOffsetFlag = flag; }
   CString getEnableThOffsetFlagString() const { return m_enableThOffsetFlag ? m_trueString : m_falseString; }
   CString getEnableThOffsetString() const { return m_enableThOffsetFlag ? m_trueString + "(" +  getThOffsetDirectionMaskString() + ")" : m_falseString; }

   CPrioritizedAccessOffsetDirectionsList& getDirectionList() { return m_directionsList; }
   const CPrioritizedAccessOffsetDirectionsList& getDirectionList() const { return m_directionsList; }

   CString getThOffsetDirectionMaskString() const;
   void setThOffsetDirectionMaskString(const CString& maskString);

   AccessOffsetPlacementTag getSmdOffsetPlacement() const;
   CString getSmdOffsetPlacementString() const;
   void setSmdOffsetPlacement(const CString& tagValue);
   void setSmdOffsetPlacement(AccessOffsetPlacementTag tagValue);
};

//-----------------------------------------------------------------------------
// CAccessOffsetOptions
//-----------------------------------------------------------------------------
class CAccessOffsetOptions
{
private:
   bool m_enableOffsetFlag;
   CAccessOffsetItem m_pinOffsetItem;
   CAccessOffsetItem m_viaOffsetItem;
   CAccessOffsetItem m_tpOffsetItem;

public:
   CAccessOffsetOptions();
   CAccessOffsetOptions(const CAccessOffsetOptions& other);
   CAccessOffsetOptions& operator=(const CAccessOffsetOptions& other);
   void initialize();

   bool operator==(const CAccessOffsetOptions& other) const;
   bool operator!=(const CAccessOffsetOptions& other) const;

   bool getEnableOffsetFlag() const { return m_enableOffsetFlag; }
   void setEnableOffsetFlag(bool flag) { m_enableOffsetFlag = flag; }

   CAccessOffsetItem& getAccessOffsetItem(AccessItemTypeTag accessOffsetItem);
   const CAccessOffsetItem& getAccessOffsetItem(AccessItemTypeTag accessOffsetItem) const;

   bool isOffsetAccessAllowed(AccessItemTypeTag itemType,int targetType) const;
   AccessItemTypeTag getAccessOffsetItemTypeFromTargetType(int targetType) const;

};

//-----------------------------------------------------------------------------
// CComponentMap
//-----------------------------------------------------------------------------
class CComponentMap : public CTypedMapStringToPtrContainer<DataStruct*>
{
public:
   CComponentMap(bool isContainer=true) : CTypedMapStringToPtrContainer<DataStruct*>(nextPrime2n(500),isContainer)
   {
   }
};

//-----------------------------------------------------------------------------
// CExposedData
//-----------------------------------------------------------------------------
class CExposedData
{
private:
   static int m_nextId;

   int m_id;
   CCEtoODBDoc& m_camCadDoc;
   AccessItemTypeTag m_itemType;

   // can remove
   DataStruct* m_data;              // via - via insert
                                    // pin - pcb component insert
                                    // tp  - tp component insert
   CPoint2d m_ptCompCentroid;       // centroid of the component for comp pin, same as dataCentroid for via
   CPoint2d m_ptDataCentroid;       // centroid of via or comp pin
   ApertureShapeTag m_eDataShape;   // pad shape of via or comp pin
   double m_dMetalDiameter;         // the largest exposed metal diameter

   // might be able to remove
   BlockStruct* m_padStackGeometry;
   int m_iTargetType;               // indicate if the data is Via, SMD, THRU HOLE
   int m_iDataEntityNum;            // entity num of via or comp pin
   bool m_bSinglePin;               // indicate if data is single pin component
   bool m_bUnloaded;                // indicate if data is unloaded

   //*rcf CString m_sErrorMessage;         // error message
   CAccessFailureReason m_failureReason;
   DataStruct *m_pExposedData;      // exposed metal data
   //POSITION m_exposedDataPos;       // the POSITION of exposed metal data in the dataList

public:
   CExposedData(CCEtoODBDoc& camCadDoc,AccessItemTypeTag itemType,
      DataStruct* data,BlockStruct* padStackGeometry,
      CPoint2d compCentriod, CPoint2d dataCentriod,
      int entityNum, int targetType, bool singlePin, bool unloaded);
   ~CExposedData();

public:
   CCEtoODBDoc& getCamCadDoc()      const { return m_camCadDoc;              }
   //CPoint2d GetCompCentroid()      const { return m_ptCompCentroid;         }
   //CPoint2d GetDataCentroid()      const { return m_ptDataCentroid;         }
   //int GetDataEntityNum()          const { return m_iDataEntityNum;         }
   //ApertureShapeTag GetDataShape() const { return m_eDataShape;             }
   int GetTargetType()             const { return m_iTargetType;            }
   //double GetMetalDiameter()       const { return m_dMetalDiameter;         }
   bool IsSinglePin()              const { return m_bSinglePin;             }
   bool IsUnloaded()               const { return m_bUnloaded;              }
   DataStruct *GetExposedData()          { return m_pExposedData;           }
   AccessItemTypeTag getItemType() const { return m_itemType;               }

   void SetCompCentroid(CPoint2d centroid)      { m_ptCompCentroid = centroid;     }
   void SetDataCentroid(CPoint2d centroid)      { m_ptDataCentroid = centroid;     }
   //void SetDataEntityNum(int entityNum)       { m_iDataEntityNum = entityNum;    }
   void SetDataShape(ApertureShapeTag shape)    { m_eDataShape     = shape;        }
   void SetTargetType(int targetType);
   void SetMetalDiameter(double diameter)       { m_dMetalDiameter = diameter;     }
   void SetSinglePin(bool singlePin)            { m_bSinglePin     = singlePin;    }
   void SetUnloaded(bool unloaded)              { m_bUnloaded      = unloaded;     }

   void SetFailureRC(EAAReasonCode rc, double optionalP1 = -1.)   { this->m_failureReason.SetReasonCode(rc, optionalP1); }
   void SetExposedData(DataStruct *exposedData)                   { m_pExposedData   = exposedData;  }

   bool GetAccessLocation(CCEtoODBDoc *doc, FileStruct *pPCBFile,const CAccessOffsetOptions& accessOffsetOptions,
            double featureSize, double &x, double &y, double &metalDiameter, double tolerance);

   int  getId() const { return m_id; }
   bool isSmd() const;
   bool isThruHole() const;

private:
//   bool calculateLocationAndMaxSize(double featureSize, double &x, double &y, double &metalDiameter, double tolerance);
//   bool calculateOffsetLocation(CCEtoODBDoc *doc, FileStruct *pPCBFile,const CAccessOffsetOptions& accessOffsetOptions,
//      double featureSize, double &x, double &y, double &metalDiameter,double tolerance);
//   bool findFarthestOffsetInCircle(double radius, double featureSize, double &x, double &y,const CAccessOffsetItem& accessOffsetItem);
   bool findFarthestOffset(CCEtoODBDoc *doc, FileStruct *pPCBFile, double featureSize, double &x, double &y);
   bool findOffsetInPolyList(CCEtoODBDoc *doc, FileStruct *pPCBFile, bool useLeftSide, double featureSize, double &x, double &y);
   bool isFeatureInPoly(CCEtoODBDoc *doc, FileStruct *pPCBFile, Region *exposeRegion, double radius, double x, double y);
//   double getDrillSize() const;
// double findMetalDiameterByDataCentriod(CCEtoODBDoc *doc);

   //CPoint2d getTargetPoint(const CAccessOffsetItem* accessOffsetItem) const;
   //CPoint2d getGridOffsetPoint(const CPoint2d& probePoint,int index,const CSize2d& searchGrid) const;
   //bool getCentralPoint(CPolyList& polyList,CPoint2d& inCircleCenter,double& inCircleRadius,
   //   double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem=NULL) const;
   //bool getCentralPointWithHoleAvoidance(CPolyList& polyList,CPoint2d& inCircleCenter,double& inCircleRadius,
   //   CPoint2d holeCenter,double holeRadius,double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem=NULL) const;
   //bool getCentralPoint(CPolygon& polygon,CPoint2d& inCircleCenter,double& circleRadius,double minFeatureSize,double tolerance,
   //   const CAccessOffsetItem* accessOffsetItem=NULL) const;
   //bool getCentralPointWithHoleAvoidance(CPolygon& polygon,CPoint2d& inCircleCenter,double& circleRadius,double minFeatureSize,double tolerance,
   //   CPoint2d holeCenter,double holeRadius,const CAccessOffsetItem* accessOffsetItem=NULL) const;

   //bool depthSearchNearestCentralPoint(CPolygon& polygon,
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double minFeatureSize,double tolerance,
   //   const CAccessOffsetItem* accessOffsetItem=NULL) const;

   //bool depthSearchNearestCentralPointWithHoleAvoidance(CPolygon& polygon,
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   CPoint2d holeCenter,double holeRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double minFeatureSize,double tolerance,
   //   const CAccessOffsetItem* accessOffsetItem=NULL) const;

   //bool breadthSearchNearestCentralPoint(CPolygon& polygon,
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double minFeatureSize,double tolerance,
   //   const CAccessOffsetItem* accessOffsetItem=NULL) const;

   //bool breadthSearchNearestCentralPointWithHoleAvoidance(CPolygon& polygon,
   //   CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
   //   CPoint2d holeCenter,double holeRadius,
   //   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double minFeatureSize,double tolerance,
   //   const CAccessOffsetItem* accessOffsetItem=NULL) const;

//   CPoly* getExposedPoly();
};

class CExposedDataMap : public CTypedPtrMap<CMapStringToPtr, CString, CExposedData*>
{
public:
   CExposedDataMap(CCEtoODBDoc& camCadDoc);
   ~CExposedDataMap();
   
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   CCEtoODBDoc&  getCamCadDoc()  { return m_camCadDoc; }
   CCamCadData& getCamCadData() { return m_camCadDoc.getCamCadData(); }

   void AddExposedViaData(int targetType, ETestSurface accessSurface, DataStruct *via);
   void AddExposedPinData(int targetType,AccessItemTypeTag accessItemType, ETestSurface accessSurface, CompPinStruct *compPin, DataStruct *compData);
   void ResetErrorMessageAndExposedData(); //FileStruct &pcbFile);
   void RemoveAll();
};

//-----------------------------------------------------------------------------
// CAAAccessLocation
//-----------------------------------------------------------------------------
class CAAAccessLocation : public CObject
{
public:
   CAAAccessLocation(const CEntity& featureEntity, CString refDes, int targetType, AccessItemTypeTag itemType, 
                     ETestSurface mountSurface, ETestSurface accessSurface, CString netName, CPoint2d location,
                     PageUnitsTag pageUnits, Bool3Tag singlePin = boolUnknown, Bool3Tag unloaded = boolUnknown);
   CAAAccessLocation(const CAAAccessLocation& other);
   CAAAccessLocation(ETestSurface accessSurface, CString accessName, CPoint2d location, CString netName);
   ~CAAAccessLocation();

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   void WriteXML(CWriteFormat& writeFormat, CAccessFailureReasonList &ClnList);

   static CAAAccessLocation *LoadXML(CXMLNode *node);

private:
   // these members are only set by constructor
   CEntity m_featureEntity;
   bool m_bAccessable;
   Bool3Tag m_singlePin;               // indicate if data is single pin component
   Bool3Tag m_unloaded;                // indicate if data is unloaded
   AccessItemTypeTag m_itemType;
   ETestSurface m_eMountSurface;       // surface of the feature
   ETestSurface m_eAccessSurface;      // surface of the access point, can be diff erence than surface of the feature
   CPoint2d m_ptFeatureLocation;
   CPoint2d m_ptAccessLocation;
   CString m_sRefDes;
   CString m_surfaceSpecificRefdes;
   CString m_sSortKey;
   CString m_sNetName;
   CString m_sName;
   double m_dExposedMetalDiameter;
   int m_eTargetType;
   int m_id;
   int m_exposedDataId;
   int m_exposedMetalId;
   PageUnitsTag m_pageUnits;

   CAccessFailureReasonList m_reasonCodes;

   static int m_nextId;

public:
   void ResetAccessLocation();

   const CEntity& getFeatureEntity() const { return m_featureEntity;         }
   bool GetAccessible()              const { return m_bAccessable;           }
   bool IsSinglePin()                const { return m_singlePin==boolTrue?true:false; }
   bool IsUnloaded()                 const { return m_unloaded==boolTrue?true:false; }
   AccessItemTypeTag getItemType()   const { return m_itemType;              }
   ETestSurface GetMountSurface()    const { return m_eMountSurface;         }
   ETestSurface GetAccessSurface()   const { return m_eAccessSurface;        }
   CPoint2d GetAccessLocation()      const { return m_ptAccessLocation;      }
   CPoint2d GetFeatureLocation()     const { return m_ptFeatureLocation;     }
   CString GetTargetTypeToString()   const;
   CString GetRefDes()               const { return m_sRefDes;               } 
   CString GetSurfaceSpecificRefDes() const { return m_surfaceSpecificRefdes; } 
   CString GetSortKey()              const { return m_sSortKey;              }
   CString GetNetName()              const { return m_sNetName;              }
   void SetNetName(CString nn)       { m_sNetName = nn; }
   CString GetName()                 const { return m_sName;                 }
   double GetExposedMetalDiameter()  const { return m_dExposedMetalDiameter; }
   int GetTargetType()               const { return m_eTargetType;           }
   int getId()                       const { return m_id;                    }
   int getExposedDataId()            const { return m_exposedDataId; }
   int getExposedMetalId()           const { return m_exposedMetalId; } 

   void ResetTargetType(int targetType)       { m_eTargetType = targetType;                            }
   void AddTargetType(int targetType)         { m_eTargetType |= targetType;                           }
   void SetAccessible(bool accessible)        { m_bAccessable = accessible;                            }
   void SetAccessLocation(CPoint2d location)  { m_ptAccessLocation = location; }
   void SetAccessLocation(double x, double y) { m_ptAccessLocation.x = x; m_ptAccessLocation.y = y;    }
   void SetName(CString const name)           { m_sName = name;                                        }
   void SetAccessSurface(ETestSurface surface) { m_eAccessSurface = surface;    }

   void SetFeatureEntity(CompPinStruct *cp)   { m_featureEntity.setCompPin(cp); }
   void SetFeatureEntity(DataStruct *data)    { m_featureEntity.setData(data);  }

   CString GetErrorMessage(const char* separator = ", ");
   CAccessFailureReasonList& GetRCList()  { return m_reasonCodes; }

   void SetExposedMetalDiameter(double diameter)      { m_dExposedMetalDiameter = diameter; }
   void setExposedDataId(const int id) { m_exposedDataId = id; }
   void setExposedMetaId(const int id) { m_exposedMetalId = id; }
   //void getAccessLocation(CMetalAnalysis& metalAnalysis);

private:
   CString generateSortKey(CString name) const; 
};

class CAAAccessLocationList : public CTypedMapSortStringToObContainer<CAAAccessLocation>  // CTypedPtrList<CPtrList, CAAAccessLocation*>
{
public:
   CAAAccessLocationList();
   ~CAAAccessLocationList();

   void CopyAcccesLocationList(const CAAAccessLocationList &fromList);
   void RemoveAccessLocations();

   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

//-----------------------------------------------------------------------------
// CExposedMetal
//-----------------------------------------------------------------------------
class CMetalAnalysis;

class CExposedMetal
{
public:
   CExposedMetal(CCEtoODBDoc& doc, BlockStruct& padstackBlock, const ETestSurface surface, AccessItemTypeTag itemType, CPoint2d compCentriod, CPoint2d dataCentriod, int targetType);
   ~CExposedMetal();

private:
   static int m_nextId;

   int m_id;
   ETestSurface m_surface;
   CCEtoODBDoc& m_doc;
   BlockStruct& m_padStackGeometry;
   DataStruct* m_pExposedData;   // only use a place holder to hold the current metal used
   CPoint2d m_ptCompCentroid;       // centroid of the component for comp pin, same as dataCentroid for via
   CPoint2d m_ptDataCentroid;       // centroid of via or comp pin
   int m_iTargetType;               // indicate if the data is Via, SMD, THRU HOLE
   AccessItemTypeTag m_itemType;

   DataStruct* m_metalOriginal;
   DataStruct* m_metalByBoardOutline;
   DataStruct* m_metalByComponentOutline;
   DataStruct* m_metalByBoardComponentOutline;
   DataStruct* m_metalBySolder;
   DataStruct* m_metalBySolderBoardOutline;
   DataStruct* m_metalBySolderComponentOutline;
   DataStruct* m_metalBySolderBoardComponentOutline;

   CAccessFailureReasonList m_metalOriginalError;
   CAccessFailureReasonList m_metalByBoardOutlineError;
   CAccessFailureReasonList m_metalByComponentOutlineError;
   CAccessFailureReasonList m_metalByBoardComponentOutineError;
   CAccessFailureReasonList m_metalBySolderError;
   CAccessFailureReasonList m_metalBySolderBoardOutlineError;
   CAccessFailureReasonList m_metalBySolderComponentOutlineError;
   CAccessFailureReasonList m_metalBySolderBoardComputerOutlineError;
   
public:
   void empty();
   int getId() const { return m_id; }
   int GetTargetType() const { return m_iTargetType; }
   ETestSurface getSurface() const { return m_surface; }
   CCEtoODBDoc& getCamCadDoc() const { return m_doc; }
   CCamCadData& getCamCadData() { return m_doc.getCamCadData(); }
   AccessItemTypeTag getItemType() const { return m_itemType; }
   DataStruct* getExposedData() { return m_pExposedData; }

   void resetExposedMetal(ExposedMetalTypeTag exposedMetalType);
   bool isAccessible(CMetalAnalysis& metalAnalysis, CAAAccessLocation& accessLocation, CTestPlan& testPlan, const CAccessOffsetOptions& accessOffsetOptions,
         FileStruct& pcbFile, const double tolerance);

private:
   void setMembersToNull();
   ExposedMetalTypeTag getMetalTypeTage(CTestPlan& testPlan);
   void verifyExposedMetalWithTestplan(CTestPlan& testPlan);

   // Call this one:
   DataStruct* getExposedMetalData(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag,  const ExposedMetalTypeTag exposedMetalType, CAccessFailureReasonList& errorMessages, bool collectAllRCs);
   // Which in turn will call these as appropriate:
   DataStruct* getMetalOriginal(const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalByBoardOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalByComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalByBoardComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalSolder(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalBySolderBoardOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalBySolderComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);
   DataStruct* getMetalBySolderBoardComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs);

   bool calculateLocationAndMaxSize(double featureSize, double &x, double &y, double &metalDiameter, double tolerance);
   bool calculateOffsetLocation(CCEtoODBDoc *doc, FileStruct *pPCBFile,const CAccessOffsetOptions& accessOffsetOptions,
      double featureSize, double &x, double &y, double &metalDiameter,double tolerance);
   bool findFarthestOffsetInCircle(double radius, double featureSize, double &x, double &y,const CAccessOffsetItem& accessOffsetItem);

   CPoint2d getTargetPoint(const CAccessOffsetItem* accessOffsetItem) const;
   CPoint2d getGridOffsetPoint(const CPoint2d& probePoint,int index,const CSize2d& searchGrid) const;
   bool getCentralPoint(CPolyList& polyList,CPoint2d& inCircleCenter,double& inCircleRadius,
      double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem=NULL) const;
   bool getCentralPointWithHoleAvoidance(CPolyList& polyList,CPoint2d& inCircleCenter,double& inCircleRadius,
      CPoint2d holeCenter,double holeRadius,double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem=NULL) const;
   bool getCentralPoint(CPolygon& polygon,CPoint2d& inCircleCenter,double& circleRadius,double minFeatureSize,double tolerance,
      const CAccessOffsetItem* accessOffsetItem=NULL) const;
   bool getCentralPointWithHoleAvoidance(CPolygon& polygon,CPoint2d& inCircleCenter,double& circleRadius,double minFeatureSize,double tolerance,
      CPoint2d holeCenter,double holeRadius,const CAccessOffsetItem* accessOffsetItem=NULL) const;

   bool depthSearchNearestCentralPoint(CPolygon& polygon,
      CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
      int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double minFeatureSize,double tolerance,
      const CAccessOffsetItem* accessOffsetItem=NULL) const;

   bool depthSearchNearestCentralPointWithHoleAvoidance(CPolygon& polygon,
      CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
      CPoint2d holeCenter,double holeRadius,
      int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double minFeatureSize,double tolerance,
      const CAccessOffsetItem* accessOffsetItem=NULL) const;

   bool breadthSearchNearestCentralPoint(CPolygon& polygon,
      CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
      int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double minFeatureSize,double tolerance,
      const CAccessOffsetItem* accessOffsetItem=NULL) const;

   bool breadthSearchNearestCentralPointWithHoleAvoidance(CPolygon& polygon,
      CPoint2d& pointNearestInCircleCenter,double& inCircleRadius,
      CPoint2d holeCenter,double holeRadius,
      int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double minFeatureSize,double tolerance,
      const CAccessOffsetItem* accessOffsetItem=NULL) const;

   double getDrillSize() const;
   CPoly* getExposedPoly();
};

//-----------------------------------------------------------------------------
// CMetalAnalysis
//-----------------------------------------------------------------------------
class CMetalAnalysis
{
public:
   CMetalAnalysis(CCEtoODBDoc& doc, FileStruct& pcbFile, CTestPlan& testPlan);
   ~CMetalAnalysis();

private:
   CCEtoODBDoc& m_doc;
   FileStruct& m_pcbFile;
   CTestPlan& m_testPlan;
   CTypedMapIntToPtrContainer<CExposedMetal*> m_exposedMetalMap;

   CSMList m_soldermaskTopList;
   CSMList m_soldermaskBottomList;
   CSMList m_boardOutlineTopList;
   CSMList m_boardOutlineBottomList;
   CSMList m_componentOutlineTopList;
   CSMList m_componentOutlineBottomList;

   bool m_soldermaskTopListDone;
   bool m_soldermaskBottomListDone;
   bool m_boardOutlineTopListDone;
   bool m_boardOutlineBottomListDone;
   bool m_componentOutlineTopListDone;
   bool m_componentOutlineBottomListDone;

   double m_boardOutlineTopValue;
   double m_boardOutlineBottomValue;
   double m_componentOutlineTopValue;
   double m_componentOutlineBottomValue;

   void emptyList(CSMList& smList, const bool removePolyList = true);
   CExposedMetal* getExposedMetal(const int id);

   bool isBoardOutlineModified(const ETestSurface surface);
   bool isComponentOutlineModified(const ETestSurface surface);

   void graphicList(CSMList& list, const int layerIndex);
   void graphicExposedMetal(const int topLayerIndex, const int bottomLayerIndex);

public:
   void empty();
   void prepareForAnalysis();
   bool analysisSoldermask(DataStruct& metal, ETestSurface analysisSurface);
   bool analysisBoardOutline(DataStruct& metal, ETestSurface analysisSurface, CAccessFailureReasonList& errorMessages);
   bool analysisComponentOutline(DataStruct& metal, ETestSurface analysisSurface, CAccessFailureReasonList& errorMessages);

   int addExposedMetal(BlockStruct& padstack, const ETestSurface surface, AccessItemTypeTag itemType, CPoint2d compCentroid, CPoint2d dataCentroid, int targetType);
   bool getAccessibleMetal(CAAAccessLocation& accessLocation, CTestPlan& testPlan, const CAccessOffsetOptions& accessOffsetOptions,
         CCEtoODBDoc& pDoc, FileStruct& pFile, const double tolerance);

   void graphicData();
};

//-----------------------------------------------------------------------------
// CAAFeatureLost
//-----------------------------------------------------------------------------
class CAAFeatureLost
{
public:
   int m_iTestAttrib;
   int m_iVia;
   int m_iConnector;
   int m_iMultiPinSMD;
   int m_iSinglePinSMD;
   int m_iTHRU;
   int m_iCADPadstack;
   int m_iBoardOutline;
   int m_iCompOutline;
   int m_iFeatureSize;
   int m_iBeadProbeSize;

   CAAFeatureLost();
   CAAFeatureLost(CAAFeatureLost &featureLost);
   ~CAAFeatureLost();

   CAAFeatureLost& operator=(const CAAFeatureLost &featureLost);
   CAAFeatureLost operator+(const CAAFeatureLost &featureLost);
   void Reset();
};

//_____________________________________________________________________________
class CRetainedProbeMap
{
public:
   CRetainedProbeMap();
   ~CRetainedProbeMap();

private:
   CTypedMapStringToPtrContainer<DataStruct*> m_retainedProbeMap;

   CString getKey(const long entityNumber, const bool isMirrored);

public:
   void collectProbes(CCEtoODBDoc& camCadDoc, FileStruct& pcbFile);
   DataStruct* findProbe(const long entityNumber, const bool isMirrored);

};

//-----------------------------------------------------------------------------
// CAAErrorMessage
//-----------------------------------------------------------------------------
class CAAErrorMessage : public CObject
{
public:
   int LoadXML(CAAAccessLocation *ascLoc,CXMLNode *node);
};

//-----------------------------------------------------------------------------
// CAANetAccess
//-----------------------------------------------------------------------------
class CAANetAccess : public CObject
{
public:
   CAANetAccess(CCEtoODBDoc *doc, FileStruct *PCBFile, CTestPlan *testPlan, CString netName);
   CAANetAccess(const CAANetAccess& other, CTestPlan *testPlan);
   ~CAANetAccess();

   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);

private:
   CCEtoODBDoc *m_pDoc;
   FileStruct *m_pPCBFile;
   CTestPlan *m_pTestPlan;
   CString m_sNetName;
   CString m_sSortKey;
   EIncludedNetType m_eIncludedNetType;
   int m_iTopAccessibleCount;
   int m_iBotAccessibleCount;
   int m_iPinCount;
   ETestSurface m_eSurface;
   CAAAccessLocationList m_accessibleLocationList;
   CAAFeatureLost m_topFeatureLost;
   CAAFeatureLost m_botFeatureLost;

public:
   void AddAccessLocation(CAAAccessLocation *accLoc);
   int AddViaAccessLocation(CMetalAnalysis& metalAnalysis, DataStruct *via, CExposedDataMap *exposedDataMap);
   int AddNetListAccessLocation(CMetalAnalysis& metalAnalysis, NetStruct *net,CComponentMap& componentMap, CExposedDataMap *exposedDataMap, bool breakOnError);
   void AnalizeNetAccess(CMetalAnalysis& metalAnalysis, CExposedDataMap *exposedDataMap,const CAccessOffsetOptions& accessOffsetOptions, double tolerance);
   void PlaceNetAccess(int &accessCount, BlockStruct *testAccessBlock, CMapWordToPtr* accessPointerMap, CRetainedProbeMap& retainedProbeMap);
   void PlaceNetAccess(int &accessCount, BlockStruct *testAccessBlock, CMapWordToPtr* accessPointerMap);
   void WriteNetAccess(CFormatStdioFile &file);
   void AddMaverick(DataStruct *accessPointData);

   CCamCadData& getCamCadData() { return m_pDoc->getCamCadData(); }

   CAAAccessLocation *FindNearestAccess(CPoint2d location, double &distance);
   void ApplyToNet(NetStruct *net, FileStruct *pcbFile);

   CString GetNetName()                 const { return m_sNetName;            }
   CString GetSortKey()                 const { return m_sSortKey;            }
   EIncludedNetType GetIncludeNetType() const { return m_eIncludedNetType;    }
   int GetTopAccessibleCount()          const { return m_iTopAccessibleCount; }
   int GetBotAccessiblecount()          const { return m_iBotAccessibleCount; }
   int GetSurface()                     const { return m_eSurface;            }
   CAAFeatureLost GetTopFeatureLost()         { return m_topFeatureLost;      }
   CAAFeatureLost GetBotFeatureLost()         { return m_botFeatureLost;      }
   bool IsNetAnalyzed() const;
   
   POSITION GetHeadPosition_AccessibleLocations()                const { return m_accessibleLocationList.GetStartPosition(); }
   CAAAccessLocation *GetNext_AccessibleLocations(POSITION &pos) const 
   { 
      CString key;
      CAAAccessLocation* accessLocation = NULL;
      if (pos != NULL)
      {
         m_accessibleLocationList.GetNextAssoc(pos, key, accessLocation);
      }

      return accessLocation;      
   }

private:
   bool addAccessLocation(CMetalAnalysis& metalAnalysis, const CEntity& featureEntity, const CString featureRefDes,
         const CPoint2d featureOrigin, const AccessItemTypeTag accessItemType, const int targetType, const bool isMirrored, 
         const bool singlePin, const bool unloaded, BlockStruct& padstackBlock, AccessItemTypeTag itemType, CPoint2d compCentriod, CPoint2d dataCentriod);
   CAAAccessLocation* InsertToList(const CEntity& featureEntity, const CString featureRefdes, const CPoint2d featureOrigin,
        const AccessItemTypeTag accessItemType, const int targetType, const ETestSurface mountSurface,
        const ETestSurface accessSurface, const CString netName, const bool singlePin, const bool unloaded);

   void WriteAccessLocation(CFormatStdioFile &file, bool accessible, EEntityType entityTypeData);

   int GetTargetType(DataStruct *data, CompPinStruct *compPin = NULL);
   int CheckAttribForTargetType(CAttributes *attribMap);
   void UpdateCADPadstackTargetType();
   void ResetNetAccess();
   bool isNetAccessible();
   bool isSurfaceAccessible(CAAAccessLocation& accLoc);
   bool isTargetTypeAccessible(CAAAccessLocation& accLoc);
   CString generateSortKey(CString name) const; 
};

//-----------------------------------------------------------------------------
// CAANetResultCount
//-----------------------------------------------------------------------------
class CAANetResultCount
{
public:
   int m_iMulPinNetTot;
   int m_iMulPinNetAcc;
   int m_iSngPinNetTot;
   int m_iSngPinNetAcc;
   int m_iNCPinNetTot;
   int m_iNCPinNetAcc;
   int m_iAllNetTot;
   int m_iAllNetAcc;

   CAANetResultCount();
   CAANetResultCount(CAANetResultCount &resultCount);
   ~CAANetResultCount();

   CAANetResultCount& operator=(const CAANetResultCount &resultCount);
   CAANetResultCount operator+(const CAANetResultCount &resultCount);
   void Reset();
};

//-----------------------------------------------------------------------------
// CAccessAnalysisSolution
//-----------------------------------------------------------------------------
class CAccessAnalysisSolution
{
public:
   CAccessAnalysisSolution(CCEtoODBDoc *doc, FileStruct *PCBFile, CTestPlan *testPlan);
   CAccessAnalysisSolution(const CAccessAnalysisSolution& other, CTestPlan *testPlan);
   ~CAccessAnalysisSolution();

private:
   CCEtoODBDoc *m_pDoc;
   FileStruct *m_pPCBFile;
   CTestPlan *m_pTestPlan;
   CMapSortStringToOb m_netAccessMap;

   CAANetResultCount m_topNetResultCount;
   CAANetResultCount m_botNetResultCount;
   CAANetResultCount m_bothNetResultCount;
   CAANetResultCount m_totalNetResultCount;
   CAAFeatureLost m_topFeatureLost;
   CAAFeatureLost m_botFeatureLost;

public:
   void DumpToFile(CFormatStdioFile &file, int indent);
   void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
   int LoadXML(CXMLNode *node);

   void ApplyToPcb(FileStruct *pcbFile);

   void RemoveNetAccess(CExposedDataMap *exposedDataMap);
   int CreateNetAccess(CMetalAnalysis& metalAnalysis, CExposedDataMap *exposedDataMap, bool breakOnError, NetAccessTypeTag netAccessType, COperationProgress* progress = NULL);
   CAANetAccess* GetNetAccess(CString netName);
   CMapSortStringToOb& GetNetAccessMap(){return m_netAccessMap;}
   void ProcessAccessAnalysis(CMetalAnalysis& metalAnalysis, CExposedDataMap *exposedDataMap,const CAccessOffsetOptions& accessOffsetOptions, double tolerance);
   void PlaceAccessSolution(CMapWordToPtr* accessPointerMap);
   bool CreateAccessAnalysisReport(CString Filename);

   CAANetResultCount GetTopNetResultCount()   { return m_topNetResultCount;   }       
   CAANetResultCount GetBotNetResultCount()   { return m_botNetResultCount;   }
   CAANetResultCount GetBothNetResultCount()  { return m_bothNetResultCount;  }
   CAANetResultCount GetTotalNetResultCount() { return m_totalNetResultCount; }
   CAAFeatureLost GetTopFeatureLost()         { return m_topFeatureLost;      }
   CAAFeatureLost GetBotFeatureLost()         { return m_botFeatureLost;      }

private:
   static int NetSortFunction(const void* elem1,const void* elem2);
   void WriteNetSummary(CFormatStdioFile &file, EIncludedNetType netType, int &totalWithAccCnt, int &zeroAccCnt, int &oneAccCnt, int &twoAccCnt, int &moreAccCnt);
   void WriteNetTestAcess(CFormatStdioFile &file, EIncludedNetType netType, bool inaccessibleNet);
};


#endif // !defined(_AccessAnalysis_h__)
