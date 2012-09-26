// $Header: /CAMCAD/4.5/PinChain.h 5     8/04/05 8:18p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__PinChain_h__)
#define __PinChain_h__

#pragma once

#include "TypedContainer.h"
#include "Qfe.h"
#include "Units.h"
#include "Line2d.h"

class DataStruct;
class CPinChain;
class CPoint2d;


//_____________________________________________________________________________
class CPinData
{
private:
   DataStruct* m_topPad;
   DataStruct* m_bottomPad;
   CPinChain* m_parentChain;

public:
   CPinData(DataStruct* topPad=NULL,DataStruct* bottomPad=NULL);

   DataStruct* getTopPad() const { return m_topPad; }
   DataStruct* getBottomPad() const { return m_bottomPad; }
   DataStruct* getPad(bool topFlag) const { return (topFlag ? m_topPad : m_bottomPad); }
   DataStruct* getPad() const { return ((m_topPad != NULL) ? m_topPad : m_bottomPad); }
   void setPad(bool topFlag,DataStruct* pad) { DataStruct*& surfacePad = (topFlag ? m_topPad : m_bottomPad);  surfacePad = pad; }
   CPoint2d getOrigin() const;

   CPinChain* getParentChain() { return m_parentChain; }
   void setParentChain(CPinChain* parentChain) { m_parentChain = parentChain; }

   bool isSmd()   const { return ((m_topPad == NULL) != (m_bottomPad == NULL)); }
   bool isTh()    const { return ((m_topPad != NULL) && (m_bottomPad != NULL)); }
   bool isEmpty() const { return ((m_topPad == NULL) && (m_bottomPad == NULL)); }
   void swapSurfaces();
};

//_____________________________________________________________________________
class CPinDataList : public CTypedPtrListContainer<CPinData*>
{
public:
   CPinDataList(bool isContainer=true);
};

//_____________________________________________________________________________
class CPinDataTree : public CTypedQfeTree<CPinData>
{
};

//_____________________________________________________________________________
class CPinDataQfeList : public CTypedQfeList<CPinData>
{
};

//_____________________________________________________________________________
class CPinDataTreeList
{
private:
   double m_searchTolerance;
   CPinDataList m_pinList;
   CPinDataTree m_pinTree;  // the tree is the CPinData container

public:
   CPinDataTreeList(double searchTolerance);
   void empty();

   POSITION GetHeadPosition() const;
   CPinData* GetNext(POSITION& pos) const;
   CPinData* GetHead() const;
   int GetCount() const;
   bool IsEmpty() const;
   void getPadCounts(int& topPadCount,int& bottomPadCount);
   void getPadCounts(int& topPadCount,int& bottomPadCount,int& bothPadCount,int& neitherPadCount);
   CPinDataTree& getPinDataTree() { return m_pinTree; }
   double getSearchTolerance() const { return m_searchTolerance; }

   CPinData* add(DataStruct* pinData,bool topFlag = true,bool addOnlyToExistingPinFlag=false);
   CPinData* addToExistingPin(DataStruct* pinData,bool topFlag = true);
   CPoint2d getCentroid() const;
   CPoint2d getPinOriginExtentsCenter() const;
   void swapSurfaces();
   void removeParents();
   bool removeSurface(bool topFlag);
};

//_____________________________________________________________________________
class CPinChain
{
private:
   CPinDataList m_pinDataList;
   double m_tolerance;
   double m_pinPitch;
   double m_dx;
   double m_dy;

public:
   CPinChain(double tolerance);

   CPinDataList& getPinDataList() { return m_pinDataList; }

   double getPinPitch();
   double getPinPitchSqr();
   double getTolerance();
   double getToleranceSqr();
   int getPinCount();
   CPoint2d getHeadPoint();
   CPoint2d getTailPoint();
   CPoint2d getNextHeadPoint();
   CPoint2d getNextTailPoint();
   CPinData* getHead();
   CPinData* getTail();

   bool canAddPin(CPinData* pinData);
   bool addPin(CPinData* pinData);

private:
   void update();
};

//_____________________________________________________________________________
class CPinChainList
{
private:
   CTypedPtrListContainer<CPinChain*> m_pinChainList;

public:
   CPinChainList(bool isContainer=true);

   void add(CPinChain* pinChain);
   POSITION getHeadPosition() const;
   CPinChain* getNext(POSITION& pos) const;
};

//_____________________________________________________________________________
class CPinChainGroup 
{
private:
   CPinDataTreeList& m_pinDataTreeList;
   CPinChainList m_pinChainList;
   CUnits m_units;
   double m_tolerance;

public:
   CPinChainGroup(CPinDataTreeList& pinDataTreeList,PageUnitsTag pageUnits,double tolerance);

   CString getDescriptor();
   CPinChainList& getPinChainList() { return m_pinChainList; }

private:
   void addPins();
   CPinChain* propagateChain(CPinData* pinData0,CPinData* pinData1);
   CPinData* findClosestOrphan(const CPinData& pinData,double searchRadius);
   CPinData* findPin(const CPoint2d& origin);
   bool canChainWith(CPinData* freePin,CPinData* queryPin);
   //void dissolveChain(CPinChain* pinChain);
};

#endif
