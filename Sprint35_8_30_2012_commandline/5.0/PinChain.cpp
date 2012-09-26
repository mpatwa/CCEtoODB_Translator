// $Header: /CAMCAD/5.0/PinChain.cpp 7     3/12/07 12:43p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "PinChain.h"
#include "DcaData.h"
#include "DcaInsert.h"

//_____________________________________________________________________________
CPinData::CPinData(DataStruct* topPad,DataStruct* bottomPad)
: m_topPad(topPad)
, m_bottomPad(bottomPad)
{
   m_parentChain = NULL;
}

CPoint2d CPinData::getOrigin() const
{
   CPoint2d origin;

   if (m_topPad != NULL)
   {
      origin = m_topPad->getInsert()->getOrigin2d();
   }
   else if (m_bottomPad != NULL)
   {
      origin = m_bottomPad->getInsert()->getOrigin2d();
   }

   return origin;
}

void CPinData::swapSurfaces()
{
   DataStruct* temp = m_topPad;
   m_topPad    = m_bottomPad;
   m_bottomPad = temp;
}

//_____________________________________________________________________________
CPinDataList::CPinDataList(bool isContainer)
: CTypedPtrListContainer<CPinData*>(isContainer)
{
}

//_____________________________________________________________________________
CPinDataTreeList::CPinDataTreeList(double searchTolerance)
: m_searchTolerance(searchTolerance)
, m_pinList(false)
{
}

void CPinDataTreeList::empty()
{
   m_pinList.empty();
   m_pinTree.deleteAll();
}

POSITION CPinDataTreeList::GetHeadPosition() const
{
   return m_pinList.GetHeadPosition();
}

CPinData* CPinDataTreeList::GetNext(POSITION& pos) const
{
   CPinData* pinData = m_pinList.GetNext(pos);

   return pinData;
}

CPinData* CPinDataTreeList::GetHead() const
{
   CPinData* pinData = m_pinList.GetHead();

   return pinData;
}

int CPinDataTreeList::GetCount() const
{
   return m_pinList.GetCount();
}

bool CPinDataTreeList::IsEmpty() const
{
   return (m_pinList.IsEmpty() != 0);
}

CPinData* CPinDataTreeList::add(DataStruct* pin,bool topFlag,bool addOnlyToExistingPinFlag)
{
   CPinData* retval = NULL;

   if (pin != NULL)
   {
      //const double searchTolerance = .001; /*CGerberEducator::getSearchTolerance();*/

      CPoint2d origin = pin->getInsert()->getOrigin2d();
      CPinDataQfeList foundList;
      CExtent searchExtent(origin,m_searchTolerance);

      m_pinTree.search(searchExtent,foundList);
      int existingPinCount = foundList.GetCount();

      if (existingPinCount == 0)
      {
         if (!addOnlyToExistingPinFlag)
         {
            CPinData* pinData = new CPinData((topFlag ? pin : NULL),(!topFlag ? pin : NULL));
            CTypedQfe<CPinData>* qfe = new CTypedQfe<CPinData>(origin,pinData);
            m_pinTree.setAt(qfe);
            m_pinList.AddTail(pinData);

            retval = pinData;
         }
      }
      else if (existingPinCount == 1)
      {
         CPinData* pinData = foundList.GetHead()->getObject();

         if (pinData->getPad(topFlag) == NULL)
         {
            pinData->setPad(topFlag,pin);
            retval = pinData;
         }
      }
   }

   return retval;
}

CPinData* CPinDataTreeList::addToExistingPin(DataStruct* pin,bool topFlag)
{
   return add(pin,topFlag,true);
}

CPoint2d CPinDataTreeList::getCentroid() const
{
   CPoint2d centroid(0.,0.);
   int pinCount = m_pinList.GetCount();

   if (pinCount > 0)
   {
      for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
      {
         CPinData* pinData = m_pinList.GetNext(pos);
         centroid = centroid + pinData->getOrigin();
      }

      centroid.x /= pinCount;
      centroid.y /= pinCount;
   }

   return centroid;
}

CPoint2d CPinDataTreeList::getPinOriginExtentsCenter() const
{
   CExtent pinOriginExtent;
   CPoint2d pinOriginExtentCenter(0.,0.);
   int pinCount = m_pinList.GetCount();

   if (pinCount > 0)
   {
      for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
      {
         CPinData* pinData = m_pinList.GetNext(pos);
         pinOriginExtent.update(pinData->getOrigin());
      }

      pinOriginExtentCenter = pinOriginExtent.getCenter();
   }

   return pinOriginExtentCenter;
}

void CPinDataTreeList::swapSurfaces()
{
   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
   {
      CPinData* pinData = m_pinList.GetNext(pos);
      pinData->swapSurfaces();
   }
}

void CPinDataTreeList::removeParents()
{
   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
   {
      CPinData* pinData = m_pinList.GetNext(pos);
      pinData->setParentChain(NULL);
   }
}

// returns true if all pins end up having one non-null pad
bool CPinDataTreeList::removeSurface(bool topFlag)
{
   bool retval = true;

   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
   {
      CPinData* pinData = m_pinList.GetNext(pos);
      pinData->setPad(topFlag,NULL);

      if (pinData->getPad(!topFlag) == NULL)
      {
         retval = false;
      }
   }

   return false;
}

void CPinDataTreeList::getPadCounts(int& topPadCount,int& bottomPadCount)
{
   int bothPadCount;
   int neitherPadCount;

   getPadCounts(topPadCount,bottomPadCount,bothPadCount,neitherPadCount);
}

void CPinDataTreeList::getPadCounts(int& topPadCount,int& bottomPadCount,int& bothPadCount,int& neitherPadCount)
{
   topPadCount = bottomPadCount = bothPadCount = neitherPadCount = 0;

   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
   {
      CPinData* pinData = m_pinList.GetNext(pos);

      int topIncrement    = ((pinData->getTopPad()    != NULL) ? 1 : 0);
      int bottomIncrement = ((pinData->getBottomPad() != NULL) ? 1 : 0);

      if (pinData->getTopPad() != NULL)
      {
         topPadCount++;

         if (pinData->getBottomPad() != NULL)
         {
            bottomPadCount++;
            bothPadCount++;
         }
      }
      else if (pinData->getBottomPad() != NULL)
      {
         bottomPadCount++;
      }
      else
      {
         neitherPadCount++;
      }
   }
}

//_____________________________________________________________________________
CPinChain::CPinChain(double tolerance)
: m_tolerance(tolerance)
, m_pinDataList(false)
{
   update();
}

void CPinChain::update()
{
   if (getPinCount() > 1)
   {
      CLine2d line(getHeadPoint(),getTailPoint());

      int pinIntervalCount = getPinCount() - 1;

      m_pinPitch = line.getLength() / pinIntervalCount;      
      m_dx = (line.get1().x - line.get0().x) / pinIntervalCount;
      m_dy = (line.get1().y - line.get0().y) / pinIntervalCount;
   }
   else
   {
      m_pinPitch = 0.;
      m_dx       = 0.;
      m_dy       = 0.;
   }
}

double CPinChain::getPinPitch()
{
   return m_pinPitch;
}

double CPinChain::getPinPitchSqr()
{
   return m_pinPitch * m_pinPitch;
}

double CPinChain::getTolerance()
{
   return m_tolerance;
}

double CPinChain::getToleranceSqr()
{
   return m_tolerance * m_tolerance;
}

int CPinChain::getPinCount()
{
   return m_pinDataList.GetCount();
}

CPinData* CPinChain::getHead()
{
   CPinData* pinData = NULL;

   if (m_pinDataList.GetCount() > 0)
   {
      pinData = m_pinDataList.GetHead();
   }

   return pinData;
}

CPinData* CPinChain::getTail()
{
   CPinData* pinData = NULL;

   if (m_pinDataList.GetCount() > 0)
   {
      pinData = m_pinDataList.GetTail();
   }

   return pinData;
}

CPoint2d CPinChain::getHeadPoint()
{
   CPoint2d headPoint;

   if (getPinCount() > 1)
   {
      headPoint = m_pinDataList.GetHead()->getOrigin();
   }

   return headPoint;
}

CPoint2d CPinChain::getTailPoint()
{
   CPoint2d tailPoint;

   if (getPinCount() > 1)
   {
      tailPoint = m_pinDataList.GetTail()->getOrigin();
   }

   return tailPoint;
}

CPoint2d CPinChain::getNextHeadPoint()
{
   CPoint2d nextHeadPoint;

   if (getPinCount() > 1)
   {
      nextHeadPoint = getHeadPoint() + CPoint2d(-m_dx,-m_dy);
   }

   return nextHeadPoint;
}

CPoint2d CPinChain::getNextTailPoint()
{
   CPoint2d nextTailPoint;

   if (getPinCount() > 1)
   {
      nextTailPoint = getTailPoint() + CPoint2d(m_dx,m_dy);
   }

   return nextTailPoint;
}

bool CPinChain::canAddPin(CPinData* pinData)
{
   bool retval = true;

   if (getPinCount() > 1)
   {
      CPoint2d origin = pinData->getOrigin();
      double distanceSqr = origin.distanceSqr(getNextHeadPoint());

      if (distanceSqr < getToleranceSqr())
      {
         retval = true;
      }
      else
      {
         distanceSqr = origin.distanceSqr(getNextTailPoint());

         if (distanceSqr < getToleranceSqr())
         {
            retval = true;
         }
      }
   }

   return retval;
}

bool CPinChain::addPin(CPinData* pinData)
{
   bool retval = true;

   CPoint2d origin = pinData->getOrigin();

   if (getPinCount() > 1)
   {
      double distanceSqr = origin.distanceSqr(getNextHeadPoint());

      if (distanceSqr < getToleranceSqr())
      {
         m_pinDataList.AddHead(pinData);
      }
      else
      {
         distanceSqr = origin.distanceSqr(getNextTailPoint());

         if (distanceSqr < getToleranceSqr())
         {
            m_pinDataList.AddTail(pinData);
         }
         else
         {
            retval = false;
         }
      }
   }
   else if (getPinCount() == 1)
   {
      CPinData* firstPin = m_pinDataList.GetHead();
      CPoint2d firstPinOrigin = firstPin->getOrigin();

      if (origin.x >= firstPinOrigin.x)
      {
         if (origin.x == firstPinOrigin.x && origin.y < firstPinOrigin.y)
         {
            m_pinDataList.AddHead(pinData);
         }
         else
         {
            m_pinDataList.AddTail(pinData);
         }
      }
      else
      {
         m_pinDataList.AddHead(pinData);
      }
   }
   else
   {
      m_pinDataList.AddTail(pinData);
   }

   if (retval)
   {
      update();
      pinData->setParentChain(this);
   }

   return retval;
}

//_____________________________________________________________________________
CPinChainList::CPinChainList(bool isContainer)
: m_pinChainList(isContainer)
{
}

void CPinChainList::add(CPinChain* pinChain)
{
   for (POSITION pos = m_pinChainList.GetHeadPosition();;)
   {
      if (pos == NULL)
      {
         m_pinChainList.AddTail(pinChain);
         break;
      }

      POSITION oldPos = pos;
      CPinChain* currentPinChain = m_pinChainList.GetNext(pos);

      if (pinChain->getPinCount() >= currentPinChain->getPinCount())
      {
         m_pinChainList.InsertBefore(pos,pinChain);
         break;
      }
   }
}

POSITION CPinChainList::getHeadPosition() const
{
   return m_pinChainList.GetHeadPosition();
}

CPinChain* CPinChainList::getNext(POSITION& pos) const
{
   return m_pinChainList.GetNext(pos);
}

//_____________________________________________________________________________
CPinChainGroup::CPinChainGroup(CPinDataTreeList& pinDataTreeList,PageUnitsTag pageUnits,double tolerance)
: m_pinDataTreeList(pinDataTreeList)
, m_units(pageUnits)
, m_tolerance(tolerance)
{
   addPins();
}

//void CPinChainGroup::dissolveChain(CPinChain* pinChain)
//{
//}

#ifdef CPinChainGroup_addPins2
void CPinChainGroup::addPins2()
{
   m_pinDataTreeList.removeParents();

   double initialSearchRadius = m_units.convertFrom(pageUnitsMils,25.);
   int orphanCount = 0;
   int remainingOrphanCount = 0;

   // construct chains
   for (double searchRadius = initialSearchRadius;;searchRadius *= 2.)
   {
      orphanCount = 0;

      for (POSITION pinDataPos = m_pinDataTreeList.GetHeadPosition();pinDataPos != NULL;)
      {
         CPinData* orphanPinData = m_pinDataTreeList.GetNext(pinDataPos);

         if (orphanPinData->getParentChain() != NULL)
         {
            continue;
         }

         // found an orphan
         orphanCount++;

         CExtent searchExtent(orphanPinData->getOrigin(),searchRadius);
         CPinDataQfeList foundList;

         m_pinDataTreeList.getPinDataTree().search(searchExtent,foundList);
         CPinData* closestFoundPinData = NULL;
         double closestDistanceSqr = 1e20;
         CPinChainList solubleChains(false);

         // find the closest pin to the orphan
         for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
         {
            CPinData* foundPinData = foundList.GetNext(foundPos)->getObject();

            if (foundPinData != orphanPinData)
            {
               if (closestFoundPinData != NULL)
               {
                  double distanceSqr = orphanPinData->getOrigin().distanceSqr(foundPinData->getOrigin());

                  if (distanceSqr < closestDistanceSqr)
                  {
                     CPinChain* foundChain = foundPinData->getParentChain();

                     if (foundChain != NULL)
                     {
                        if (!foundChain->canAddPin(orphanPinData))
                        {
                           // dissolve the closest pin's parent chain if the 
                           // parent chain's pin pitch is greater than the orphan's pitch
                           if (foundChain->getPinPitchSqr() > distanceSqr)
                           {
                              solubleChains.add(foundChain);
                           }
                           else
                           {
                              // this closest neighbor is part of a tighter pin chain,
                              // continue looking for the next closest neighbor
                              continue;
                           }
                        }
                     }

                     closestDistanceSqr  = distanceSqr;
                     closestFoundPinData = foundPinData;
                  }
               }
               else
               {
                  closestFoundPinData = foundPinData;
               }
            }
         }

         // found the closest available pin to the orphan
         if (closestFoundPinData != NULL)
         {
            CPinChain* foundChain = closestFoundPinData->getParentChain();

            // if the closest pin has a parent chain, try to add the orphan to this chain
            // if the orphan is successfully added, it's parent chain will be set to foundChain.
            if (foundChain != NULL)
            {
               if (!foundChain->addPin(orphanPinData))
               {
                  // The orphan was not added.
                  // The found chain must already have been added to the soluble chain list above.
               }
            }

            // dissolve all the chains in the soluble chain list
            for (POSITION solubleChainPos = solubleChains.GetHeadPosition();solubleChainPos != NULL;)
            {
               CPinChain* solubleChain = solubleChains.GetNext(solubleChainPos);

               if (solubleChain == foundChain)
               {
                  int iii = 3;  // logic error
               }

               dissolveChain(solubleChain);
            }

            if (orphanPinData->getParentChain() == NULL)
            {
               // The orphan was not added to the closest pin's parent chain.
               // Create a new two pin chain containing the orphan and the closest pin
               CPinChain* newChain = new CPinChain(m_tolerance);

               if (!newChain->addPin(orphanPinData))
               {
                  int iii = 3;
               }

               if (!newChain->addPin(closestFoundPinData))
               {
                  int iii = 3;
               }

               m_pinChainList.AddTail(newChain);
            }
            else
            {
               // the orphan was successfully added to the closest pin's parent chain
            }
         }
         else
         {
            // No available pin within the search radius was found.
            // Another attempt with this orphan will be made when the searchRadius is increased.
            remainingOrphanCount++;
         }
      }

      if (remainingOrphanCount < 2)
      {
         // At least two remaining orphans must be available to pair with each other,
         // othewise the last orphan must remain an orphan.
         break;
      }
   }

   // merge the chains


}
#endif

void CPinChainGroup::addPins()
{
   m_pinDataTreeList.removeParents();

   double initialSearchRadius = m_units.convertFrom(pageUnitsMils,25.);

   // construct chains
   for (double searchRadius = initialSearchRadius;;searchRadius *= 2.)
   {
      int orphanCount = 0;
      int remainingOrphanCount = 0;
      CPinData* lastOrphanPinData = NULL;

      for (POSITION pinDataPos = m_pinDataTreeList.GetHeadPosition();pinDataPos != NULL;)
      {
         CPinData* orphanPinData = m_pinDataTreeList.GetNext(pinDataPos);

         if (orphanPinData->getParentChain() != NULL)
         {
            continue;
         }

         // found an orphan
         orphanCount++;

         CPinData* closestFoundPinData = findClosestOrphan(*orphanPinData,searchRadius);

         if (closestFoundPinData != NULL)
         {
            CPinChain* pinChain = propagateChain(orphanPinData,closestFoundPinData);

            if (pinChain != NULL)
            {
               m_pinChainList.add(pinChain);
            }
            else
            {
               remainingOrphanCount++;
               lastOrphanPinData = orphanPinData;
            }
         }
         else
         {
            // No available pin within the search radius was found.
            // Another attempt with this orphan will be made when the searchRadius is increased.
            remainingOrphanCount++;
            lastOrphanPinData = orphanPinData;
         }
      }

      if (remainingOrphanCount < 2)
      {
         // At least two remaining orphans must be available to pair with each other
         // otherwise the last orphan must be placed in a one pin chain.

         if (remainingOrphanCount == 1 && lastOrphanPinData != NULL)
         {
            // put the last orphan in its own pinChain
            CPinChain* pinChain = new CPinChain(m_tolerance);
            pinChain->addPin(lastOrphanPinData);
            m_pinChainList.add(pinChain);
         }

         break;
      }
   }
}

CPinData* CPinChainGroup::findClosestOrphan(const CPinData& pinData,double searchRadius)
{
   CPoint2d origin = pinData.getOrigin();

   CExtent searchExtent(origin,searchRadius);
   CPinDataQfeList foundList;

   m_pinDataTreeList.getPinDataTree().search(searchExtent,foundList);
   CPinData* closestFoundPinData = NULL;
   double closestDistanceSqr = 1e20;

   // find the closest available pin to the orphan
   for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
   {
      CPinData* foundPinData = foundList.GetNext(foundPos)->getObject();

      if (foundPinData->getParentChain() == NULL && foundPinData != &pinData)
      {
         if (closestFoundPinData != NULL)
         {
            double distanceSqr = origin.distanceSqr(foundPinData->getOrigin());

            if (distanceSqr < closestDistanceSqr)
            {
               closestDistanceSqr  = distanceSqr;
               closestFoundPinData = foundPinData;
            }
         }
         else
         {
            closestFoundPinData = foundPinData;
         }
      }
   }

   return closestFoundPinData;
}

CPinData* CPinChainGroup::findPin(const CPoint2d& origin)
{
   CPinData* pinData = NULL;

   CExtent searchExtent(origin,m_tolerance);
   CPinDataQfeList foundList;

   m_pinDataTreeList.getPinDataTree().search(searchExtent,foundList);

   if (foundList.GetCount() > 0)
   {
      pinData = foundList.GetHead()->getObject();
   }

   return pinData;
}

bool CPinChainGroup::canChainWith(CPinData* freePin,CPinData* queryPin)
{
   bool retval = true;

   double pinPitch = freePin->getOrigin().distance(queryPin->getOrigin());
   CPinData* closestPinToQueryPin = findClosestOrphan(*queryPin,pinPitch);

   if (closestPinToQueryPin != freePin && closestPinToQueryPin != NULL)
   {
      double otherPinPitch = closestPinToQueryPin->getOrigin().distance(queryPin->getOrigin());

      retval = fpnear(pinPitch,otherPinPitch,m_tolerance);
   }

   return retval;
}

CPinChain* CPinChainGroup::propagateChain(CPinData* pinData0,CPinData* pinData1)
{
   CPinChain* pinChain = NULL;

   if (canChainWith(pinData0,pinData1))
   {
      pinChain = new CPinChain(m_tolerance);

      pinChain->addPin(pinData0);
      pinChain->addPin(pinData1);

      for (int pass = 0;pass < 2;pass++)
      {
         for (int loopCount=0;;loopCount++)
         {
            CPinData* endPin = ((pass == 0) ? pinChain->getTail() : pinChain->getHead());
            CPoint2d nextEndPoint = ((pass == 0) ? pinChain->getNextTailPoint() : pinChain->getNextHeadPoint());
            CPinData* nextPin = findPin(nextEndPoint);

            if (nextPin == NULL)
            {
               break;
            }

            if (!canChainWith(endPin,nextPin))
            {
               break;
            }

            if (! pinChain->addPin(nextPin))
            {
               break;
            }
         }
      }
   }

   return pinChain;
}
