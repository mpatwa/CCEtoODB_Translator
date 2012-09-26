// $Header: /CAMCAD/4.5/TestPadDensityDfm.cpp 25    8/04/06 8:55p Kurt Van Ness $

#include "StdAfx.h"
#include "CamCadDatabase.h"
#include "ManufacturingGrid.h"
#include "PcbUtil.h"
#include "WriteFormat.h"

//_____________________________________________________________________________
class CQfeTestPoint : public CQfe
{
private:
   CPoint2d m_origin;
   int m_density;

public:
   CQfeTestPoint(const CPoint2d& origin);
   ~CQfeTestPoint();

public:
   CPoint2d getOrigin() const { return m_origin; }
   CExtent getExtent() const;

   int incrementDensity() { return ++m_density; }
   int getDensity() { return m_density; }
   virtual CAttributes* getAttributeMap() = 0;

   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const;
   virtual CString getInfoString() const;

public:
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CQfeTestPoint::CQfeTestPoint(const CPoint2d& origin)   
{
   m_origin  = origin;
   m_density = 0;
}

CQfeTestPoint::~CQfeTestPoint()
{
}

int CQfeTestPoint::getObjectType() const
{
   return 0;
}

CExtent CQfeTestPoint::getExtent() const
{
   return CExtent(getOrigin(),getOrigin());
}

bool CQfeTestPoint::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double edgeDistance = _hypot(getOrigin().x - point.x,
                                getOrigin().y - point.y);

   bool retval = (edgeDistance < distance);

   return retval;
}

bool CQfeTestPoint::isExtentIntersecting(const CExtent& extent)
{
   bool retval = extent.distanceTo(getOrigin()) == 0.;

   return retval;
}

bool CQfeTestPoint::isInViolation(CObject2d& otherObject)
{
   ASSERT(0);
   return false;
}

CString CQfeTestPoint::getInfoString() const
{
   CString retval;

   retval.Format("CQfeTestPoint: density=%d, origin=(%s,%s), extent=(%s,%s),(%s,%s)",
      m_density,
      fpfmt(getOrigin().x),
      fpfmt(getOrigin().y),
      fpfmt(getExtent().getXmin()),
      fpfmt(getExtent().getYmin()),
      fpfmt(getExtent().getXmax()),
      fpfmt(getExtent().getYmax())   );

   return retval;
}

//_____________________________________________________________________________
class CQfeTestPointPin : public CQfeTestPoint
{
private:
   CompPinStruct* m_pin;

public:
   CQfeTestPointPin(CompPinStruct* pin);
   //~CQfeTestPointPin();

public:
   virtual CAttributes* getAttributeMap();
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CQfeTestPointPin::CQfeTestPointPin(CompPinStruct* pin) :
   CQfeTestPoint(pin->getOrigin())
{
   m_pin = pin;
}

CAttributes* CQfeTestPointPin::getAttributeMap()
{
   if (m_pin->getAttributesRef() == NULL)
   {
      m_pin->getAttributesRef() = new CAttributes();
   }

   return m_pin->getAttributesRef();
}

//_____________________________________________________________________________
class CQfeTestPointVia : public CQfeTestPoint
{
private:
   DataStruct* m_via;

public:
   CQfeTestPointVia(DataStruct* via);
   //~CQfeTestPointVia();

public:
   virtual CAttributes* getAttributeMap();
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CQfeTestPointVia::CQfeTestPointVia(DataStruct* via) :
   CQfeTestPoint(CPoint2d(via->getPoint()->x,via->getPoint()->y))
{
   m_via = via;
}

CAttributes* CQfeTestPointVia::getAttributeMap()
{
   if (m_via->getAttributesRef() == NULL)
   {
      m_via->getAttributesRef() = new CAttributes();
   }

   return m_via->getAttributesRef();
}

//_____________________________________________________________________________
class CQfeTestPointList : public CTypedPtrList<CQfeList,CQfeTestPoint*>
{
public:
   CQfeTestPointList(int nBlockSize=200);
   //~CQfeTestPointList(){}
   void empty() { RemoveAll(); }
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CQfeTestPointList::CQfeTestPointList(int nBlockSize) : 
   CTypedPtrList<CQfeList,CQfeTestPoint*>(nBlockSize)
{
}

//_____________________________________________________________________________
class CQfeTestPointTree : public CQfeExtentLimitedContainer
{
private:

public:
   CQfeTestPointTree();
   //~CQfeTestPointTree();

public:
   virtual int search(const CExtent& extent,CQfeTestPointList& foundList)
      { return CQfeExtentLimitedContainer::search(extent,foundList); }

   virtual CQfeTestPoint* findFirst(const CExtent& extent)
      { return (CQfeTestPoint*)CQfeExtentLimitedContainer::findFirst(extent); }

   virtual CQfeTestPoint* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (CQfeTestPoint*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual CQfeTestPoint* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (CQfeTestPoint*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual CQfeTestPoint* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (CQfeTestPoint*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CQfeTestPoint* qfe)
      { CQfeExtentLimitedContainer::setAt(qfe); }
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CQfeTestPointTree::CQfeTestPointTree() : CQfeExtentLimitedContainer(CSize2d(0.,0.),false)
{
}

//_____________________________________________________________________________
class CTestPointGridSpace : public CObject
{
private:
   CString m_gridCoordinate;
   int m_criticalViolationCount;
   int m_marginalViolationCount;

public:
   CTestPointGridSpace(const CString& gridCoordinate);

   int incCriticalViolationCount() { return ++m_criticalViolationCount; }
   int incMarginalViolationCount() { return ++m_marginalViolationCount; }
   int getCriticalViolationCount() { return   m_criticalViolationCount; }
   int getMarginalViolationCount() { return   m_marginalViolationCount; }
};

CTestPointGridSpace::CTestPointGridSpace(const CString& gridCoordinate)
{
   m_gridCoordinate = gridCoordinate;
   m_criticalViolationCount = 0;
   m_marginalViolationCount = 0;
}

//_____________________________________________________________________________
class CTestPointGrid : public CMapSortedStringToOb<CTestPointGridSpace>
{
public:
   CTestPointGridSpace& getAt(const CString& gridCoordinate);
};

CTestPointGridSpace& CTestPointGrid::getAt(const CString& gridCoordinate)
{
   CTestPointGridSpace* gridSpace = NULL;

   Lookup(gridCoordinate,gridSpace);

   if (gridSpace == NULL)
   {
      gridSpace = new CTestPointGridSpace(gridCoordinate);
      SetAt(gridCoordinate,gridSpace);
   }

   return *gridSpace;
}

//_____________________________________________________________________________
bool DFM_TestPadDensityCheck(CCEtoODBDoc& camCadDoc,BlockStruct& pcbBlock,double gridSize,
   int criticalDensity,int marginalDensity,bool addTestPointDensityAttributeFlag) 
{
   bool retval = false;
   addTestPointDensityAttributeFlag = true;

   while (true)
   {
      if (pcbBlock.getBlockType() != BLOCKTYPE_PCB)
      {
         break;
      }

      CNullWriteFormat errorLog;
      CCamCadDatabase camCadDatabase(camCadDoc);

      CQfeTestPointList qfeTestPointList;
      CQfeTestPointTree qfeTestPointTree;
      CTestPointGrid testPointGrid;
      Attrib* value;

      FileStruct* pcbFile = camCadDoc.Find_File(pcbBlock.getFileNumber());
      CExtent extent = camCadDoc.calcFileOutlineExtents(pcbFile);

      if (gridSize <= 0) gridSize = 1.;

      int xSteps = (int)(extent.getXsize()/gridSize) + 1;
      int ySteps = (int)(extent.getYsize()/gridSize) + 1;
      CManufacturingGrid manufacturingGrid(xSteps,gridSize,ySteps,gridSize);
      manufacturingGrid.regenerateGrid(camCadDoc,*pcbFile,extent,.005);

      //int manufacturingGridDefinitionKeywordIndex = camCadDatabase.getKeywordIndex(manufacturingGrid.getAttributeName());

      //if (pcbBlock.getAttributesRef() != NULL)
      //{
      //   if (pcbBlock.getAttributesRef()->Lookup(manufacturingGridDefinitionKeywordIndex,value))
      //   {
      //      Attrib* attribute = (Attrib*)value;
      //      CString manufacturingGridDefinition = camCadDatabase.getAttributeStringValue(attribute);
      //      manufacturingGrid.set(manufacturingGridDefinition);
      //   }
      //}

      int testKeywordIndex = camCadDoc.IsKeyWord(ATT_TEST,0);
      CString valueString;
      CTypedPtrMap<CMapStringToPtr,CString,DataStruct*> testComponents;
      testComponents.InitHashTable(nextPrime2n(512));
      CDataList& pcbDataList = pcbBlock.getDataList();

      // scan for components and vias with TEST attributes
      for (POSITION pcbDataPos = pcbDataList.GetHeadPosition();pcbDataPos != NULL;)
      {
         DataStruct* pcbDataStruct = pcbDataList.GetNext(pcbDataPos);

         if (pcbDataStruct != NULL && pcbDataStruct->getDataType() == T_INSERT)
         {
            InsertStruct* pcbInsert = pcbDataStruct->getInsert();

            if (pcbInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               if (pcbDataStruct->getAttributesRef() && pcbDataStruct->getAttributesRef()->Lookup(testKeywordIndex,value))
               {
                  CString refDes      = pcbInsert->getRefname();
                  testComponents.SetAt(refDes,pcbDataStruct);
               }
            }
            else if (pcbInsert->getInsertType() == INSERTTYPE_VIA)
            {
               if (pcbDataStruct->getAttributesRef() && pcbDataStruct->getAttributesRef()->Lookup(testKeywordIndex,value))
               {
                  CQfeTestPointVia* testVia = new CQfeTestPointVia(pcbDataStruct);
                  qfeTestPointList.AddTail(testVia);
                  qfeTestPointTree.setAt(testVia);
               }
            }
         }
      }

      // gather component pins with TEST attribute
      DataStruct* componentDataStruct;

      generate_PINLOC(&camCadDoc,pcbFile,true);

      for (POSITION netPos = pcbFile->getNetList().GetHeadPosition();netPos != NULL;)
      {
         NetStruct* net = pcbFile->getNetList().GetNext(netPos);

         for (POSITION pinPos = net->getHeadCompPinPosition();pinPos != NULL;)
         {
            CompPinStruct* pin = net->getNextCompPin(pinPos);

            if (pin->getAttributesRef() && ( pin->getAttributesRef()->Lookup(testKeywordIndex,value) ||
                testComponents.Lookup(pin->getRefDes(),componentDataStruct) )    )
            {
               CQfeTestPoint* testPoint = new CQfeTestPointPin(pin);
               qfeTestPointList.AddTail(testPoint);
               qfeTestPointTree.setAt(testPoint);
            }
         }
      }

      int algorithmIndex = GetAlgorithmNameIndex(&camCadDoc,"Test Point Density Check");
      double searchRadius = .5;
      double searchRadiusSquared = searchRadius*searchRadius;
      CExtent searchExtent;
      CQfeTestPointList foundList;

      for (POSITION testPointPos = qfeTestPointList.GetHeadPosition();testPointPos != NULL;)
      {
         CQfeTestPoint* testPoint = qfeTestPointList.GetNext(testPointPos);
         foundList.empty();
         CPoint2d testPointOrigin = testPoint->getOrigin();

         searchExtent.set(testPointOrigin.x - searchRadius,
                          testPointOrigin.y - searchRadius,
                          testPointOrigin.x + searchRadius,
                          testPointOrigin.y + searchRadius );

         qfeTestPointTree.search(searchExtent,foundList);

         for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
         {
            CQfeTestPoint* foundTestPoint = foundList.GetNext(foundPos);

            double dx = foundTestPoint->getOrigin().x - testPointOrigin.x;
            double dy = foundTestPoint->getOrigin().y - testPointOrigin.y;
            double distanceSquared = dx*dx + dy*dy;

            if (distanceSquared <= searchRadiusSquared)
            {
               testPoint->incrementDensity();
            }
         }

         int density = testPoint->getDensity();
         DrcFailureRangeTag failureRange = drcUndefinedFailure;

         if (density >= criticalDensity)
         {
            failureRange = drcCriticalFailure;
         }
         else if (density >= marginalDensity)
         {
            failureRange = drcMarginalFailure;
         }

         if (failureRange != drcUndefinedFailure)
         {
            CString description;
            description.Format("Test Point Density Violation; TestPointDensity=%d",density);

            camCadDatabase.addDrc(*pcbFile,testPointOrigin.x,testPointOrigin.y,description,
               drcSimple,failureRange,algorithmIndex,drcGeneric);

            CString gridCoordinate = manufacturingGrid.getGridCoordinate(testPoint->getOrigin());
            CTestPointGridSpace& testPointGridSpace = testPointGrid.getAt(gridCoordinate);

            if (failureRange == drcCriticalFailure)
            {
               testPointGridSpace.incCriticalViolationCount();
            }
            else
            {
               testPointGridSpace.incMarginalViolationCount();
            }
         }

         if (addTestPointDensityAttributeFlag)
         {
            CAttributes* attributeMap = testPoint->getAttributeMap();
            valueString.Format("%d",testPoint->getDensity());
            camCadDatabase.addAttribute(&attributeMap,"TestPointDensity",valueTypeInteger,valueString,errorLog);
         }
      }

      //CManufacturingGrid manufacturingGrid(hSteps,hSize,vSteps,vSize);
      //manufacturingGrid.regenerateGrid(*doc,*pcbFile,extent,atof(m_gridThickness));

      CTestPointGridSpace* testPointGridSpace;
      CString gridCoordinate;
      int index;
      algorithmIndex = GetAlgorithmNameIndex(&camCadDoc,"Test Point Density Check (by Grid Location)");

      for (testPointGrid.rewind(index);testPointGrid.next(testPointGridSpace,gridCoordinate,index);)
      {
         CString description;
         description.Format("GridLocation=%s, criticalCount=%d, marginalCount=%d",
            (const char*)gridCoordinate,
            testPointGridSpace->getCriticalViolationCount(),
            testPointGridSpace->getMarginalViolationCount());

         CPoint2d origin = manufacturingGrid.getGridSpaceCenter(gridCoordinate);

         if (testPointGridSpace->getCriticalViolationCount() > 0)
         {
            camCadDatabase.addDrc(*pcbFile,origin.x,origin.y,description,
               drcSimple,drcCriticalFailure,algorithmIndex,drcGeneric);
         }

         if (testPointGridSpace->getMarginalViolationCount() > 0)
         {
            camCadDatabase.addDrc(*pcbFile,origin.x,origin.y,description,
               drcSimple,drcMarginalFailure,algorithmIndex,drcGeneric);
         }
      }

      retval = true;

      break;
   }

   return retval;
}