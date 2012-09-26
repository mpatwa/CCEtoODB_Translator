// $Header: /CAMCAD/5.0/Region.cpp 46    6/21/07 8:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "region.h"
#include "ccdoc.h"
#include "rgn.h"
#include "Debug.h"
#include <float.h>

CWriteFormat& getDebugWriteFormat();

//_____________________________________________________________________________
Rect::Rect(long Left,long Right,long Top,long Bottom) :
   left(Left),right(Right),top(Top),bottom(Bottom)
{
}

Rect::Rect(const Rect& other) :
   left(other.left),right(other.right),top(other.top),bottom(other.bottom)
{
}

//_____________________________________________________________________________
Region::Region(bool filledFlag) : 
   m_filledFlag(filledFlag)
{
}

Region::~Region()
{
}

/******************************************************************************
* ClearRegion
*/
void Region::ClearRegion()
{
   m_rects.empty();
   //POSITION rectPos = list.GetHeadPosition();
   //while (rectPos)
   //{
   //   Rect *rect = (Rect*)list.GetNext(rectPos);
   //   delete rect;
   //}
   //list.RemoveAll();
}

Region *Region::Clone()
{
	Region *newRegion = new Region();

   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
      Rect* rect = getNext(pos);
      newRegion->addTail(new Rect(*rect));
   }
   
  // POSITION rectPos = list.GetHeadPosition();
  // while (rectPos)
  // {
  //    Rect *rect = (Rect*)list.GetNext(rectPos);

		//newRegion->list.AddTail((CObject*)rect->Clone());
  // }

	return newRegion;
}


/******************************************************************************
* RegionFromPolylist
*/
Region *RegionFromPolylist(CCEtoODBDoc *doc, CPolyList *polyList, float scaleFactor)
{
   CRgn *rgn = CreateRgn(doc, polyList, scaleFactor);
   Region *region = RegionFromRgn(rgn);
   delete rgn;
   return region;
}

/******************************************************************************
* RegionFromPoly
*/
Region *RegionFromPoly(CPoly *poly, float scaleFactor,PageUnitsTag pageUnits)
{
   CRgn *rgn = GetPolyRgn(poly, scaleFactor,pageUnits);
   Region *region = RegionFromRgn(rgn);
   delete rgn;
   return region;
}

/******************************************************************************
* RegionFromRgn
*/
Region *RegionFromRgn(CRgn *rgn)
{
   Region *region = new Region();

   int size = rgn->GetRegionData(NULL, 0);
   RGNDATA *rd = (RGNDATA*)calloc(size, 1);
   rgn->GetRegionData(rd, size);

   RECT *rects = (RECT*)rd->Buffer;
   int rectCount = (int)rd->rdh.nCount;

   for (int i=rectCount - 1; i>=0; i--)
   {
      RECT *rect = &rects[i];

      Rect *newRect = new Rect(rect->left, rect->right, rect->bottom, rect->top); // flip because Windows increases going down
      //region->InsertRectInRegion(newRect);
      region->addTail(newRect);
   }

   free(rd);

   return region;
}

/******************************************************************************
* IntersectRegions
*/
Region *IntersectRegions(Region *region1, Region *region2)
{
   Region *resultRegion = NULL;

   for (POSITION pos1 = region1->getHeadPosition();pos1 != NULL;)
   {
      Rect* rect1 = region1->getNext(pos1);

      for (POSITION pos2 = region2->getHeadPosition();pos2 != NULL;)
      {
         Rect* rect2 = region2->getNext(pos2);

         Rect* resultRect = IntersectRects(rect1,rect2);

         if (resultRect != NULL)
         {
            if (resultRegion == NULL)
            {
               resultRegion = new Region();
            }

            resultRegion->InsertRectInRegion(resultRect);
         }
      }
   }

   //POSITION rectPos1 = region1->list.GetHeadPosition();
   //while (rectPos1)
   //{
   //   Rect *rect1 = (Rect*)region1->list.GetNext(rectPos1);

   //   POSITION rectPos2 = region2->list.GetHeadPosition();
   //   while (rectPos2)
   //   {
   //      Rect *rect2 = (Rect*)region2->list.GetNext(rectPos2);

   //      Rect *resultRect = IntersectRects(rect1, rect2);

   //      if (resultRect)
   //      {
   //         if (!resultRegion)
   //            resultRegion = new Region();

   //         resultRegion->InsertRectInRegion(resultRect);
   //      }
   //   }
   //}

   if (resultRegion != NULL)
   {
      resultRegion->Optimize();
   }

   return resultRegion;
}

long Region::getArea() const
{
   long area = 0;

   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
      Rect* rect = getNext(pos);

      long rectArea = abs(rect->top - rect->bottom) * abs(rect->right - rect->left);

      area += rectArea;
   }

   return area;
}

/******************************************************************************
* Region::InsertRectInRegion
*/
void Region::InsertRectInRegion(Rect *rect)
{
   //POSITION rectPos = list.GetTailPosition();
   for (POSITION rectPos = getTailPosition();rectPos != NULL;)
   {
      POSITION tempPos = rectPos;
      Rect* currentRect = getPrev(rectPos);

      if (currentRect->Contains(rect))
      {
         delete rect;
         return;
      }

      if (rect->Contains(currentRect))
      {
         removeAt(tempPos);
         //list.RemoveAt(tempPos);
         delete currentRect;
         continue;
      }

      if (rect->top == currentRect->top)
      {
         if (rect->bottom == currentRect->bottom)
         {
            if (rect->left > currentRect->left)
            {
               insertAfter(tempPos,rect);
               //list.InsertAfter(tempPos, (CObject*)rect);
               return;
            }
         }
         else // need to split rects
         {
            if (rect->bottom < currentRect->bottom) // split rect
            {
               Rect *newRect = new Rect(rect->left, rect->right, currentRect->bottom, rect->bottom);
               rect->bottom = currentRect->bottom;
               InsertRectInRegion(rect);
               InsertRectInRegion(newRect);
               return;
            }
            else // split currentRect
            {
               Rect *newRect = new Rect(currentRect->left, currentRect->right, rect->bottom, currentRect->bottom);
               currentRect->bottom = rect->bottom;
               InsertRectInRegion(newRect);
               InsertRectInRegion(rect);
               return;
            }
         }
      }

      if (rect->top < currentRect->top)
      {
         insertAfter(tempPos,rect);
         //list.InsertAfter(tempPos, (CObject*)rect);
         return;
      }
   }

   addHead(rect);
   //list.AddHead((CObject*)rect); // must be first rect
}

/******************************************************************************
* UnionRegions
*/
Region *UnionRegions(Region *region1, Region *region2)
{
   Region *resultRegion = new Region;

   for (POSITION rectPos = region1->getHeadPosition();rectPos != NULL;)
   {
      Rect* rect = region1->getNext(rectPos);

      resultRegion->InsertRectInRegion(new Rect(*rect));
   }

   for (POSITION rectPos = region2->getHeadPosition();rectPos != NULL;)
   {
      Rect* rect = region2->getNext(rectPos);

      resultRegion->InsertRectInRegion(new Rect(*rect));
   }

   //POSITION rectPos = region1->list.GetHeadPosition();
   //while (rectPos)
   //{
   //   Rect *rect = (Rect*)region1->list.GetNext(rectPos);

   //   //resultRegion->InsertRectInRegion(new Rect(rect->left, rect->right, rect->top, rect->bottom));
   //   resultRegion->list.AddTail((CObject*)new Rect(rect->left, rect->right, rect->top, rect->bottom));
   //}

   //rectPos = region2->list.GetHeadPosition();
   //while (rectPos)
   //{
   //   Rect *rect = (Rect*)region2->list.GetNext(rectPos);

   //   resultRegion->InsertRectInRegion(new Rect(rect->left, rect->right, rect->top, rect->bottom));
   //}

   resultRegion->Optimize();

   return resultRegion;
}

/******************************************************************************
* CombineWith
*/
void Region::CombineWith(Region *region)
{
   for (POSITION rectPos = region->getHeadPosition();rectPos != NULL;)
   {
      Rect* rect = region->getNext(rectPos);

      InsertRectInRegion(new Rect(*rect));
   }

   //POSITION rectPos = region->list.GetHeadPosition();
   //while (rectPos)
   //{
   //   Rect *rect = (Rect*)region->list.GetNext(rectPos);

   //   InsertRectInRegion(new Rect(rect->left, rect->right, rect->top, rect->bottom));
   //}

   Optimize();
}

/******************************************************************************
* DiffRegions
*/
Region *DiffRegions(Region *parentRegion, Region *voidRegion)
{
   Region *resultRegion = parentRegion->Clone();
	CMapStringToString yCoordinateMap;	// Must use a string map to prevent lost during convertion
	
	// collect y's in a map
	CString yTop, yBot, tmp;
   for (POSITION voidPos = voidRegion->getHeadPosition();voidPos != NULL;)
   {
      Rect* voidRect = voidRegion->getNext(voidPos);

		yTop.Format("%d", voidRect->top);
		yBot.Format("%d", voidRect->bottom);

		if (!yCoordinateMap.Lookup(yTop, tmp))
			yCoordinateMap.SetAt(yTop, yTop);

		if (!yCoordinateMap.Lookup(yBot, tmp))
			yCoordinateMap.SetAt(yBot, yBot);
   }


	// split parent region widths by y's	
	for (POSITION mapPos = yCoordinateMap.GetStartPosition();mapPos != NULL;)
	{
		CString key;
		CString yStr;
		yCoordinateMap.GetNextAssoc(mapPos, key, yStr);

		long y = atol(yStr);
		
		for (POSITION resultPos = resultRegion->getHeadPosition();resultPos != NULL;)
		{
			Rect* resultRect = resultRegion->getAt(resultPos);

			if (resultRect->top > y && resultRect->bottom < y)
			{
				Rect *newRect = new Rect(resultRect->left, resultRect->right, y, resultRect->bottom);
				resultRect->bottom = y;

				resultRegion->InsertRectInRegion(newRect);
			}

			resultRegion->getNext(resultPos);
		}
   }


	// remove each voidRegion rectangle from parentRectangle   
   for (POSITION pos = voidRegion->getHeadPosition();pos != NULL;)
   {
	   Rect* rect = voidRegion->getNext(pos);
		resultRegion->RemoveRect(rect);
   }

	resultRegion->Optimize();

   return resultRegion;
}

/******************************************************************************
* Region::RemoveRect
*/
void Region::RemoveRect(Rect *rectToRemove)
{
   
   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
		POSITION tempPos = pos;
		Rect* rect = getNext(pos);

		if (DoRectsIntersect(rect, rectToRemove))
		{
			if (rectToRemove->Contains(rect))
			{
				removeAt(tempPos);
				delete rect;
				continue;
			}

			if (rectToRemove->top >= rect->top && rectToRemove->bottom <= rect->bottom)
			{
				if (rectToRemove->left < rect->left)
				{
					if (rectToRemove->right < rect->right)
               {
						rect->left = rectToRemove->right;
               }
					else
					{
						removeAt(tempPos);
						delete rect;
					}
				}
				else if (rectToRemove->right > rect->right)
				{
					if (rectToRemove->left > rect->left)
               {
						rect->right = rectToRemove->left;
               }
					else
					{
						removeAt(tempPos);
						delete rect;
					}
				}
				else
				{
					Rect* newRect  = rect->Clone();
					newRect->right = rectToRemove->left;
					rect->left     = rectToRemove->right;
               insertBefore(tempPos,newRect);					
				}
			}
		}
	}
}


/******************************************************************************
* Region::GetExtents
*/
void Region::GetExtents(ExtentRect *extents, float scaleFactor)
{
   float reciprocal = 1.0f / scaleFactor;

   extents->right = extents->top    = -FLT_MAX;
   extents->left  = extents->bottom =  FLT_MAX;
   
   for (POSITION rectPos = getHeadPosition();rectPos != NULL;)
   {
      Rect* rect = getNext(rectPos);

      if (reciprocal * rect->left < extents->left)
         extents->left = reciprocal * rect->left;

      if (reciprocal * rect->right > extents->right)
         extents->right = reciprocal * rect->right;

      if (reciprocal * rect->top > extents->top)
         extents->top = reciprocal * rect->top;

      if (reciprocal * rect->bottom < extents->bottom)
         extents->bottom = reciprocal * rect->bottom;
   }
}

/******************************************************************************
* Region::GetPolys
*/
CPolyList *Region::GetPolys(float scaleFactor)
{
   if (IsEmpty())
      return NULL;

   Optimize();

   CPolyList *polyList = NULL;

   if (CanWrapRegion())
      return WrapRects(scaleFactor);
   else
      return BuildRectPolys(scaleFactor);
}

CPolyList *Region::GetSmoothedPolys(float scaleFactor)
{
   if (IsEmpty())
      return NULL;

   Optimize();

   CPolyList *polyList = NULL;

   if (CanWrapRegion())
   {
      polyList = SmoothlyWrapRects(scaleFactor);
   }
   else
   {
      polyList = BuildRectPolys(scaleFactor);
   }

   return polyList;
}

/******************************************************************************
* CanWrapRegion
*/
bool Region::CanWrapRegion()
{
   int rectCount = getCount();

   if (rectCount < 2)
      return FALSE;

   POSITION rectPos = getHeadPosition();
   Rect* rect = getNext(rectPos);

   while (rectPos != NULL)
   {
      Rect* prevRect = rect;
      rect = getNext(rectPos);

      if (rect->top != prevRect->bottom)
      {
         return false;
      }
   }

   return true;
}

void Region::fillVerticalConcavities()
{
   if (getCount() > 1)
   {
      POSITION pos = getHeadPosition();
      Rect* prevRect = getNext(pos);
      POSITION oldPos;

      while (pos != NULL)
      {
         oldPos = pos;
         Rect* rect = getNext(pos);

         if (prevRect->top == rect->top)
         {
            prevRect->left  = min(prevRect->left ,rect->left );
            prevRect->right = max(prevRect->right,rect->right);

            removeAt(oldPos);
            delete rect;
         }
         else
         {
            prevRect = rect;
         }
      }
   }
}

/******************************************************************************
* Region::WrapRects
*/
CPolyList *Region::WrapRects(float scaleFactor)
{
   float reciprocal = 1.0f / scaleFactor;

   CPolyList *polyList = new CPolyList;

   CPoly *poly = new CPoly;
   polyList->AddTail(poly);
   poly->setWidthIndex(-1);
   poly->setClosed(true);
   poly->setHatchLine(false);
   poly->setFilled(m_filledFlag);
   poly->setVoid(false);
   poly->setHidden(false);
   poly->setFloodBoundary(false);
   poly->setBreakOut(false);
   poly->setThermalLine(false);
   
   for (POSITION rectPos = getHeadPosition();rectPos != NULL;)
   {
      Rect* rect = getNext(rectPos);

      CPnt *pnt = new CPnt;
      pnt->x = reciprocal * rect->left;
      pnt->y = reciprocal * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = reciprocal * rect->left;
      pnt->y = reciprocal * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   for (POSITION rectPos = getTailPosition();rectPos != NULL;)
   {
      Rect* rect = getPrev(rectPos);

      CPnt *pnt = new CPnt;
      pnt->x = reciprocal * rect->right;
      pnt->y = reciprocal * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = reciprocal * rect->right;
      pnt->y = reciprocal * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   Rect *rect = getHead();
   CPnt *pnt = new CPnt;
   pnt->x = reciprocal * rect->left;
   pnt->y = reciprocal * rect->top;
   pnt->bulge = 0;
   poly->getPntList().AddTail(pnt);

   return polyList;
}

void Region::addPntToPoly(CPoly* poly,POINT point,int prevPrevDx,double scale)
{
   double y = point.y;

   if (prevPrevDx < 0)
   {
      y -= .5;
   }
   else if (prevPrevDx > 0)
   {
      y += .5;
   }

   CPnt* pnt = new CPnt((DbUnit)(scale * point.x),(DbUnit)(scale * y));
   poly->getPntList().AddTail(pnt);
}

CPolyList* Region::SmoothlyWrapRects(float scaleFactor)
{
   float reciprocal = 1.0f / scaleFactor;

   CPolyList *polyList = new CPolyList;

   CPoly *poly = new CPoly;
   polyList->AddTail(poly);

   poly->setWidthIndex(-1);
   poly->setClosed(true);
   poly->setHatchLine(false);
   poly->setFilled(m_filledFlag);
   poly->setVoid(false);
   poly->setHidden(false);
   poly->setFloodBoundary(false);
   poly->setBreakOut(false);
   poly->setThermalLine(false);

   long dx,dy,prevDx,prevDy,prevPrevDx;
   int  stepCount = 0;
   int  loopCount = 0;
   POINT point,prevPoint;
   bool stepFlag;

   for (POSITION rectPos = getHeadPosition();rectPos != NULL;loopCount++)
   {
      Rect* rect = getNext(rectPos);

      if (loopCount == 0)
      {
         prevPoint.x = rect->right;
         prevPoint.y = rect->top;
         prevDx = rect->left - rect->right;
         prevDy = 1;
         prevPrevDx = prevDx;
      }

      point.x = rect->left;
      point.y = ((prevDx < 0 || (prevDx == 0 && prevPrevDx < 0)) ? rect->top : rect->bottom);

      if (point.x == prevPoint.x && point.y == prevPoint.y)
      {
         continue;
      }

      dx = point.x - prevPoint.x;
      dy = point.y - prevPoint.y;

      stepFlag = (dy * prevDx == prevDy * dx);

      if (!stepFlag)
      {
         addPntToPoly(poly,prevPoint,prevPrevDx,reciprocal);
      }

      if (prevDx != 0)
      {
         prevPrevDx = prevDx;
      }

      prevDx = dx;
      prevDy = dy;
      prevPoint.x = point.x;
      prevPoint.y = point.y;
   }

   loopCount = 0;

   for (POSITION rectPos = getTailPosition();rectPos != NULL;loopCount++)
   {
      Rect* rect = getPrev(rectPos);

      if (loopCount == 0)
      {
         prevPoint.x = rect->left;
         prevPoint.y = rect->bottom;
         prevDx = rect->right - rect->left;
         prevDy = -1;
         prevPrevDx = prevDx;
      }

      point.x = rect->right;
      //point.y = ((prevDx >= 0) ? rect->bottom : rect->top);
      point.y = ((prevDx > 0 || (prevDx == 0 && prevPrevDx > 0)) ? rect->bottom : rect->top);

      if (point.x == prevPoint.x && point.y == prevPoint.y)
      {
         continue;
      }

      dx = point.x - prevPoint.x;
      dy = point.y - prevPoint.y;

      stepFlag = (dx == prevDx && dy == prevDy);

      if (!stepFlag)
      {
         addPntToPoly(poly,prevPoint,prevPrevDx,reciprocal);
      }

      if (prevDx != 0)
      {
         prevPrevDx = prevDx;
      }

      prevDx = dx;
      prevDy = dy;
      prevPoint.x = point.x;
      prevPoint.y = point.y;
   }

   poly->close();

   return polyList;
}

/******************************************************************************
* Region::BuildRectPolys
*/
CPolyList *Region::BuildRectPolys(float scaleFactor)
{
   float reciprocal = 1.0f / scaleFactor;

   CPolyList *polyList = new CPolyList;
   
   for (POSITION rectPos = getHeadPosition();rectPos != NULL;)
   {
      Rect* rect = getNext(rectPos);

      CPoly *poly = new CPoly;
      polyList->AddTail(poly);
      poly->setWidthIndex(-1);
      poly->setClosed(true);
      poly->setFilled(m_filledFlag);
      poly->setHatchLine(false);
      poly->setVoid(false);
      poly->setHidden(false);
      poly->setFloodBoundary(false);
      poly->setBreakOut(false);
      poly->setThermalLine(false);

      CPnt *pnt = new CPnt;
      pnt->x = reciprocal * rect->left;
      pnt->y = reciprocal * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = reciprocal * rect->right;
      pnt->y = reciprocal * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = reciprocal * rect->right;
      pnt->y = reciprocal * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = reciprocal * rect->left;
      pnt->y = reciprocal * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = reciprocal * rect->left;
      pnt->y = reciprocal * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   return polyList;
}

/******************************************************************************
* AreRegionsEqual
*/
bool AreRegionsEqual(Region *region1, Region *region2)
{
   region1->Optimize();
   region2->Optimize();

	if (region1->getCount() != region2->getCount())
   {
		return false;
   }

   POSITION rectPos1 = region1->getHeadPosition();
   POSITION rectPos2 = region2->getHeadPosition();

   while (rectPos1 != NULL && rectPos2 != NULL)
   {
      Rect *rect1 = region1->getNext(rectPos1);
      Rect *rect2 = region2->getNext(rectPos2);

      if (!AreRectsEqual(rect1, rect2))
      {
         return false;
      }
   }

   return true;
}

/******************************************************************************
* AreRectsEqual
*/
bool AreRectsEqual(Rect *rect1, Rect *rect2)
{
   return (rect1->left == rect2->left && rect1->right == rect2->right && rect1->top == rect2->top && rect1->bottom == rect2->bottom);
}

/******************************************************************************
* DoRegionsIntersect
*/
bool DoRegionsIntersect(Region *region1, Region *region2)
{   
   for (POSITION rectPos1 = region1->getHeadPosition();rectPos1 != NULL;)
   {
      Rect *rect1 = region1->getNext(rectPos1);
      
      for (POSITION rectPos2 = region2->getHeadPosition();rectPos2 != NULL;)
      {
         Rect *rect2 = region2->getNext(rectPos2);

         if (DoRectsIntersect(rect1, rect2))
         {
            return true;
         }
      }
   }

   return false;
}

/******************************************************************************
* DoRectsIntersect
*/
bool DoRectsIntersect(Rect *rect1, Rect *rect2)
{
   if (rect1->bottom >= rect2->top)
      return false;

   if (rect2->bottom >= rect1->top)
      return false;

   if (rect1->left >= rect2->right)
      return false;

   if (rect2->left >= rect1->right)
      return false;

   return true;
}

/******************************************************************************
* DoRegionsTouch
*/
bool DoRegionsTouch(Region *region1, Region *region2)
{
   for (POSITION rectPos1 = region1->getHeadPosition();rectPos1 != NULL;)
   {
      Rect *rect1 = region1->getNext(rectPos1);
      
      for (POSITION rectPos2 = region2->getHeadPosition();rectPos2 != NULL;)
      {
         Rect *rect2 = region2->getNext(rectPos2);

         if (DoRectsTouch(rect1, rect2))
         {
            return true;
         }
      }
   }

   return false;
}

/******************************************************************************
* DoRectsTouch
*/
bool DoRectsTouch(Rect *rect1, Rect *rect2)
{
   if (rect1->bottom > rect2->top)
      return false;

   if (rect2->bottom > rect1->top)
      return false;

   if (rect1->left > rect2->right)
      return false;

   if (rect2->left > rect1->right)
      return false;

   return true;
}

/******************************************************************************
* IntersectRects
*/
Rect *IntersectRects(Rect *rect1, Rect *rect2)
{
   if (!DoRectsIntersect(rect1, rect2))
   {
      return NULL;
   }

   return new Rect(max(rect1->left, rect2->left), min(rect1->right, rect2->right), min(rect1->top, rect2->top), max(rect1->bottom, rect2->bottom));
}

/******************************************************************************
* Rect::Contains
*/
bool Rect::Contains(Rect *rect)
{
   if (rect->top >= top)
      return false;

   if (rect->bottom <= bottom)
      return false;

   if (rect->left <= left)
      return false;

   if (rect->right >= right)
      return false;

   return true;
}

/******************************************************************************
* Region::Optimize
*/
void Region::Optimize()
{
   if (getCount() < 2)
   {
      return;
   }

   POSITION nextPos = getHeadPosition();
   Rect *curRect = getNext(nextPos);

   while (nextPos)
   {
      bool removeCurRect = false;
      Rect *prevRect = curRect;
      POSITION curPos = nextPos;
      curRect = getNext(nextPos);

      // These section are new code added for case #1969 to merge rectangle that are
      // intersected in a column or in a row or are within each other
      if (curRect->left == prevRect->left && curRect->right == prevRect->right)
      {
         if (curRect->top <= prevRect->top && curRect->bottom >= prevRect->bottom)
         {
            removeCurRect = true;
         }
         else
         {
            if (curRect->top > prevRect->top && curRect->bottom <= prevRect->top)
            {
               prevRect->top = curRect->top;
               removeCurRect = true;
            }
            
            if (curRect->bottom < prevRect->bottom && curRect->top >= prevRect->bottom)
            {
               prevRect->bottom = curRect->bottom;
               removeCurRect = true;
            }
         }
      }
      else if (curRect->top == prevRect->top && curRect->bottom == prevRect->bottom)
      {
         if (curRect->left >= prevRect->left && curRect->right <= prevRect->right)
         {
            removeCurRect = true;
         }
         else 
         {
            if (curRect->left < prevRect->left && curRect->right >= prevRect->left)
            {
               prevRect->left = curRect->left;
               removeCurRect = true;
            }
            
            if (curRect->right > prevRect->right && curRect->left <= prevRect->right)
            {
               prevRect->right = curRect->right;
               removeCurRect = true;
            }
         }
      }
      else if (curRect->top <= prevRect->top && curRect->bottom >= prevRect->bottom &&
               curRect->left >= prevRect->left && curRect->right <= prevRect->right)
      {
         removeCurRect = true;
      }
      else if (curRect->top >= prevRect->top && curRect->bottom <= prevRect->bottom &&
               curRect->left <= prevRect->left && curRect->right >= prevRect->right)
      {
         prevRect->top = curRect->top;
         prevRect->bottom = curRect->bottom;
         prevRect->left = curRect->left;
         prevRect->right = curRect->right;
         removeCurRect = true;
      }

      if (removeCurRect)
      {
         removeAt(curPos);
         delete curRect;

         // Start checking from the beginning again
         nextPos = getHeadPosition();
         curRect = getNext(nextPos);
      }

      //if (curRect->top == prevRect->bottom)
      //{
      //   if (prevRect->left == curRect->left && prevRect->right == curRect->right)
      //   {
      //      prevRect->bottom = curRect->bottom;
      //      removeAt(curPos);
      //      delete curRect;
      //      curRect = prevRect;
      //   }
      //}
      //else if (curRect->top == prevRect->top && curRect->bottom == prevRect->bottom && curRect->left <= prevRect->right && curRect->right >= prevRect->left)
      //{
      //   prevRect->right = curRect->right;
      //   removeAt(curPos);
      //   delete curRect;
      //   curRect = prevRect;
      //}
   }
}

void Region::Dump(CString filename)
{
   FILE *dump = fopen(filename, "w");

   if (!dump)
   {
      return;
   }
   
   for (POSITION rectPos = getHeadPosition();rectPos != NULL;)
   {
      Rect *rect = getNext(rectPos);
      fprintf(dump, "l=%d, r=%d, t=%d, b=%d\n", 
            rect->left, rect->right, rect->top, rect->bottom);
   }

   fclose(dump);
}

//_____________________________________________________________________________
CExtendedRgn::CExtendedRgn()
{
   CreateRectRgn(0,0,0,0);
   m_regionData = NULL;
}

CExtendedRgn::CExtendedRgn(const CRgn* rgn)
{
   int size = rgn->GetRegionData(NULL, 0);
   RGNDATA* rgnData = (RGNDATA*)calloc(size, 1);
   rgn->GetRegionData(rgnData, size);

   BOOL success = CreateFromData(NULL,size,rgnData);

   if (success == 0)
   {
      int iii = 3;
   }

   free(rgnData);

   //const RGNDATA* rgnData = rgn
   //CreateRectRgn(0,0,0,0);
   //CRgn* pRgn = (CRgn*)(&rgn);
   //CopyRgn(pRgn);

   m_regionData = NULL;
}

CExtendedRgn::~CExtendedRgn()
{
   releaseRegionData();
}

RGNDATA* CExtendedRgn::getRegionData()
{
   if (m_regionData == NULL)
   {
      int size = GetRegionData(NULL, 0);
      m_regionData = (RGNDATA*)calloc(size, 1);
      GetRegionData(m_regionData, size);
   }

   return m_regionData;
}

const RGNDATA* CExtendedRgn::getRegionData() const
{
   if (m_regionData == NULL)
   {
      int size = GetRegionData(NULL, 0);
      m_regionData = (RGNDATA*)calloc(size, 1);
      GetRegionData(m_regionData, size);
   }

   return m_regionData;
}

int CExtendedRgn::getRegionDataRectCount() const
{
   int rectCount = getRegionData()->rdh.nCount;

   return rectCount;
}

void CExtendedRgn::setRegionDataRectCount(int newCount)
{
   if (newCount >= 0 && newCount < getRegionDataRectCount())
   {  
      getRegionData()->rdh.nCount = newCount;
   }
}

RECT* CExtendedRgn::getRegionDataRect(int index)
{
   RECT* rect = NULL;

   if (index >= 0 && index < getRegionDataRectCount())
   {
      rect = (RECT*)(getRegionData()->Buffer + (index * sizeof(RECT)));
   }

   return rect;
}

const RECT* CExtendedRgn::getRegionDataRect(int index) const
{
   const RECT* rect = NULL;

   if (index >= 0 && index < getRegionDataRectCount())
   {
      rect = (RECT*)(getRegionData()->Buffer + (index * sizeof(RECT)));
   }

   return rect;
}

void CExtendedRgn::releaseRegionData()
{
   if (m_regionData != NULL)
   {
      free(m_regionData);
      m_regionData = NULL;
   }
}

bool CExtendedRgn::isValid() const
{
   bool retval = true;

   RECT* rects = (RECT*)getRegionData()->Buffer;
   int rectCount = (int)getRegionData()->rdh.nCount;

   const RECT* prevRect = NULL;

   for (int index = 0;index < rectCount && retval;index++)
   {
      const RECT* rect = &(rects[index]);

      if (rect->left >= rect->right)
      {
         retval = false;
      }
      else if (rect->top >= rect->bottom)
      {
         retval = false;
      }

      if (prevRect != NULL && retval)
      {
         if (rect->top == prevRect->top)
         {
            if (rect->bottom != prevRect->bottom)
            {
               retval = false;
            }
            else if (rect->left <= prevRect->right)
            {
               retval = false;
            }
         }
         else if (rect->top > prevRect->bottom)
         {
            retval = false;
         }
      }

      prevRect = rect;
   }

   return retval;
}

void CExtendedRgn::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;
   writeFormat.pushHeader("CRgn: ");

   const RGNDATAHEADER& header = getRegionData()->rdh;
   writeFormat.writef("hdr.dwSize   = %u\n",(unsigned int)header.dwSize);
   writeFormat.writef("hdr.iType    = %u\n",(unsigned int)header.iType);
   writeFormat.writef("hdr.nCount   = %u\n",(unsigned int)header.nCount);
   writeFormat.writef("hdr.nRgnSize = %u\n",(unsigned int)header.nRgnSize);
   writeFormat.writef("hdr.rcBound (L,R,T,B) = (%d, %d, %d, %d)\n",
      header.rcBound.left,header.rcBound.right,header.rcBound.top,header.rcBound.bottom);

   RECT* rects = (RECT*)getRegionData()->Buffer;
   int rectCount = (int)getRegionData()->rdh.nCount;

   if (depth != 0)
   {
      for (int index = 0;index < rectCount;index++)
      {
         RECT* rect = &(rects[index]);

         writeFormat.writef("rect[%3d] (L,R,T,B) = (%4d, %4d, %4d, %4d)\n",
            index,rect->left,rect->right,rect->top,rect->bottom);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
CRgnRectList::CRgnRectList()
{
   m_rgn = NULL;
}

CRgnRectList::~CRgnRectList()
{
   delete m_rgn;
}

void CRgnRectList::detachRgn()
{
   delete m_rgn;
   m_rgn = NULL;
}

void CRgnRectList::addRectCopy(RECT& rect)
{
   m_rectList.AddTail(new CRect(rect));

   detachRgn();
}

CExtendedRgn* CRgnRectList::getRgn()
{
   if (m_rgn == NULL)
   {
      CIntExtent extent;

      int size = sizeof(RGNDATAHEADER) + sizeof(RECT)*m_rectList.GetCount();
      RGNDATA* regionData = (RGNDATA*)calloc(size, 1);

      regionData->rdh.dwSize   = sizeof(RGNDATAHEADER);
      regionData->rdh.iType    = RDH_RECTANGLES;
      regionData->rdh.nCount   = m_rectList.GetCount();
      regionData->rdh.nRgnSize = 0;

      int rectIndex = 0;

      for (POSITION pos = m_rectList.GetHeadPosition();pos != NULL;)
      {
         CRect* rect = m_rectList.GetNext(pos);

         RECT* rectBuf = (RECT*)(regionData->Buffer + (rectIndex++ * sizeof(RECT)));
         rectBuf->left   = rect->left;
         rectBuf->right  = rect->right;
         rectBuf->top    = rect->top;
         rectBuf->bottom = rect->bottom;

         extent.update(rect->left ,rect->top);
         extent.update(rect->right,rect->bottom);
      }

      regionData->rdh.rcBound.left   = extent.getXmin();
      regionData->rdh.rcBound.right  = extent.getXmax();
      regionData->rdh.rcBound.top    = extent.getYmin();
      regionData->rdh.rcBound.bottom = extent.getYmax();

      m_rgn = new CExtendedRgn();
      m_rgn->CreateFromData(NULL,size,regionData);

      free(regionData);
   }

   return m_rgn;
}

//_____________________________________________________________________________
CRegionRect::CRegionRect(const Rect& rect,int index) :
   m_index(index)
{
   initRelatedRects();

   if (rect.left <= rect.right)
   {
      m_minX = rect.left;
      m_maxX = rect.right;
   }
   else
   {
      m_minX = rect.right;
      m_maxX = rect.left;
   }

   if (rect.bottom <= rect.top)
   {
      m_minY = rect.bottom;
      m_maxY = rect.top;
   }
   else
   {
      m_minY = rect.top;
      m_maxY = rect.bottom;
   }
}

CRegionRect::CRegionRect(const RECT& rect,int index) :
   m_index(index)
{
   initRelatedRects();

   if (rect.left <= rect.right)
   {
      m_minX = rect.left;
      m_maxX = rect.right;
   }
   else
   {
      m_minX = rect.right;
      m_maxX = rect.left;
   }

   if (rect.bottom <= rect.top)
   {
      m_minY = rect.bottom;
      m_maxY = rect.top;
   }
   else
   {
      m_minY = rect.top;
      m_maxY = rect.bottom;
   }
}

CRegionRect::CRegionRect(const CRegionRect& rect,int index) :
   m_index(index)
{
   initRelatedRects();

   m_minX = rect.m_minX;
   m_minY = rect.m_minY;
   m_maxX = rect.m_maxX;
   m_maxY = rect.m_maxY;
}

void CRegionRect::setMinY(int minY)
{
   m_minY = minY;

   if (m_minY > m_maxY)
   {
      m_maxY = m_minY;
   }
}

void CRegionRect::setMaxY(int maxY)
{
   m_maxY = maxY;

   if (m_maxY < m_minY)
   {
      m_minY = m_maxY;
   }
}

void CRegionRect::initRelatedRects()
{
   for (int index = regionRectRelationTagFirst;index <= regionRectRelationTagLast;index++)
   {
      m_relatedRects[index] = NULL;
   }

   m_perimeterIndex = -1;
}

void CRegionRect::setRelatedRect(RegionRectRelationTag relation,CRegionRect* relatedRect)
{
   m_relatedRects[relation] = relatedRect;
}

CRegionRect* CRegionRect::getRelatedRect(RegionRectRelationTag relation)
{
   CRegionRect* rect = m_relatedRects[relation];

   return rect;
}

CIntExtent CRegionRect::getExtent() const
{
   CIntExtent extent(m_minX,m_minY,m_maxX,m_maxY);

   return extent;
}

int CRegionRect::getArea() const
{
   int area = getWidth() * getHeight();

   return area;
}

void CRegionRect::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CRegionRect\n"
"{\n"
"   index = %d\n"
"   ( (m_minX=%d,m_minY=%d), (m_maxX=%d,m_maxY=%d) )\n"
"   rectLeftOf           = %p [%3d]\n"
"   rectLeftOfGoingDown  = %p [%3d]\n"
"   rectLeftOfGoingUp    = %p [%3d]\n"
"   rectRightOf          = %p [%3d]\n"
"   rectRightOfGoingDown = %p [%3d]\n"
"   rectRightOfGoingUp   = %p [%3d]\n",
m_index,
m_minX,m_minY,m_maxX,m_maxY,
m_relatedRects[rectLeftOf          ],((m_relatedRects[rectLeftOf          ] == NULL) ? -1 : m_relatedRects[rectLeftOf          ]->getIndex()),
m_relatedRects[rectLeftOfGoingDown ],((m_relatedRects[rectLeftOfGoingDown ] == NULL) ? -1 : m_relatedRects[rectLeftOfGoingDown ]->getIndex()),
m_relatedRects[rectLeftOfGoingUp   ],((m_relatedRects[rectLeftOfGoingUp   ] == NULL) ? -1 : m_relatedRects[rectLeftOfGoingUp   ]->getIndex()),
m_relatedRects[rectRightOf         ],((m_relatedRects[rectRightOf         ] == NULL) ? -1 : m_relatedRects[rectRightOf         ]->getIndex()),
m_relatedRects[rectRightOfGoingDown],((m_relatedRects[rectRightOfGoingDown] == NULL) ? -1 : m_relatedRects[rectRightOfGoingDown]->getIndex()),
m_relatedRects[rectRightOfGoingUp  ],((m_relatedRects[rectRightOfGoingUp  ] == NULL) ? -1 : m_relatedRects[rectRightOfGoingUp  ]->getIndex())
);

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CRegionRectList::CRegionRectList(bool isContainer) :
   CTypedPtrListContainer<CRegionRect*>(isContainer)
{
}

CRegionRectList::CRegionRectList(const CRegionRectList& other,bool isContainer) :
   CTypedPtrListContainer<CRegionRect*>(isContainer)
{
   for (POSITION pos = other.GetHeadPosition();pos != NULL;)
   {
      CRegionRect* regionRect = other.GetNext(pos);

      if (isContainer)
      {
         regionRect = new CRegionRect(*regionRect);
      }

      AddTail(regionRect);
   }
}

CRegionRectList::CRegionRectList(CRgn& rgn,bool isContainer)
: CTypedPtrListContainer<CRegionRect*>(isContainer)
{
   set(rgn);
}

CRegionRectList::CRegionRectList(const Region& region,bool isContainer)
: CTypedPtrListContainer<CRegionRect*>(isContainer)
{
   set(region);
}

CRegionRect* CRegionRectList::addRect(Rect& rect)
{
   CRegionRect* regionRect = new CRegionRect(rect,GetCount());
   AddTail(regionRect);

   return regionRect;
}

CRegionRect* CRegionRectList::addRect(CRegionRect& rect)
{
   CRegionRect* regionRect = new CRegionRect(rect,GetCount());
   AddTail(regionRect);

   return regionRect;
}

void CRegionRectList::set(CRgn& rgn)
{
   int size = rgn.GetRegionData(NULL, 0);
   RGNDATA* rd = (RGNDATA*)calloc(size, 1);
   rgn.GetRegionData(rd, size);

   RECT* rects = (RECT*)rd->Buffer;
   int rectCount = (int)rd->rdh.nCount;
   RECT* lastRect = NULL;

   // Windows orders the RECTS from left to right and then from top to bottom.
   // Unfortunately top to bottom in windows corresponds to a decreasing y coordinate.
   // Therefore, the row ordering must be reversed so that the CRegionRects are ordered
   // from left to right and then top to bottom with increasing y coordinates between rows.
   // knv - 20050119.1855

   POSITION rowPos;

   for (int index = 0;index < rectCount;index++)
   {
      RECT* rect = &(rects[index]);
      CRegionRect* regionRect = new CRegionRect(*rect);

      if (lastRect != NULL)
      {
         bool sameRow = (rect->top == lastRect->top);

         if (sameRow)
         {
            rowPos = InsertAfter(rowPos,regionRect);
         }
         else
         {
            rowPos = AddHead(regionRect);
         }
      }
      else
      {
         rowPos = AddHead(regionRect);
      }

      lastRect = rect;
   }

   int newIndex = 0;

   for (POSITION pos = GetHeadPosition();pos != NULL;newIndex++)
   {
      CRegionRect* rect = GetNext(pos);
      rect->setIndex(newIndex);
   }

   free(rd);
}

void CRegionRectList::set(const Region& region)
{
   int newIndex = 0;

   for (POSITION pos = region.getHeadPosition();pos != NULL;newIndex++)
   {
      Rect* rect = region.getNext(pos);
      CRegionRect* regionRect = new CRegionRect(*rect);
      regionRect->setIndex(newIndex);
      AddTail(regionRect);
   }
}

CExtendedRgn* CRegionRectList::getRgn() const
{
   CIntExtent extent(getExtent());

   int size = sizeof(RGNDATAHEADER) + sizeof(RECT)*GetCount();
   RGNDATA* regionData = (RGNDATA*)calloc(size, 1);

   regionData->rdh.dwSize   = sizeof(RGNDATAHEADER);
   regionData->rdh.iType    = RDH_RECTANGLES;
   regionData->rdh.nCount   = GetCount();
   regionData->rdh.nRgnSize = 0;
   regionData->rdh.rcBound.left   = extent.getXmin();
   regionData->rdh.rcBound.right  = extent.getXmax();
   regionData->rdh.rcBound.top    = extent.getYmin();
   regionData->rdh.rcBound.bottom = extent.getYmax();

   // Windows orders the RECTS from left to right and then from top to bottom.
   // Unfortunately top to bottom in windows corresponds to a decreasing y coordinate.
   // Therefore, the row ordering must be reversed.
   // knv - 20050522.1749

   CRegionRect* lastRegionRect = NULL;
   CRegionRectList currentRow(false);
   int rectIndex = 0;

   for (POSITION pos = GetTailPosition();;)
   {
      CRegionRect* regionRect = ((pos != NULL) ? GetPrev(pos) : NULL);

      bool sameRow  = ((lastRegionRect != NULL) && (regionRect != NULL) && (regionRect->getMaxY() == lastRegionRect->getMaxY()));
      bool endOfRow = (pos == NULL || !sameRow);

      if (sameRow)
      {
         currentRow.AddHead(regionRect);
      }

      if (endOfRow)
      {
         if (currentRow.GetCount() > 0)
         {
            for (POSITION bufPos = currentRow.GetHeadPosition();bufPos != NULL;)
            {
               CRegionRect* bufRegionRect = GetNext(bufPos);

               RECT* rect = (RECT*)(regionData->Buffer + (rectIndex++ * sizeof(RECT)));
               rect->left   = bufRegionRect->getMinX();
               rect->right  = bufRegionRect->getMaxX();
               rect->top    = bufRegionRect->getMinY();
               rect->bottom = bufRegionRect->getMaxY();
            }

            currentRow.empty();
         }

         if (regionRect == NULL)
         {
            break;
         }
      }

      if (!sameRow && (regionRect != NULL))
      {
         currentRow.AddTail(regionRect);
      }

      lastRegionRect = regionRect;
   }

   CExtendedRgn* rgn = new CExtendedRgn();
   rgn->CreateFromData(NULL,size,regionData);

   return rgn;
}

CIntExtent CRegionRectList::getExtent() const
{
   CIntExtent extent;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CRegionRect* rect = GetNext(pos);

      if (rect != NULL)
      {
         extent.update(rect->getExtent());
      }
   }

   return extent;
}

int CRegionRectList::getArea() const
{
   int area = 0;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CRegionRect* rect = GetNext(pos);

      if (rect != NULL)
      {
         area += rect->getArea();
      }
   }

   return area;
}

void CRegionRectList::initPerimeterIndices()
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CRegionRect* rect = GetNext(pos);

      if (rect != NULL)
      {
         rect->setPerimeterIndex(-1);
      }
   }
}

void CRegionRectList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CRegionRectList\n"
"{\n"
"   Count = %d\n",
GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CRegionRect* rect = GetNext(pos);

         if (rect != NULL)
         {
            rect->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CPointList::CPointList()
{
}

CPointList::~CPointList()
{
}

void CPointList::empty()
{
   m_points.empty();
}

void CPointList::takeData(CPointList& other)
{
   for (POSITION pos = other.m_points.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CPoint* point = other.m_points.GetNext(pos);

      if (point != NULL)
      {
         m_points.AddTail(point);
      }

      other.m_points.RemoveAt(oldPos);
   }
}

void CPointList::moveHeadToTail()
{
   m_points.AddTail(m_points.RemoveHead());
}

CPoint CPointList::getHead()
{
   CPoint* point = m_points.GetHead();

   return *point;
}

CPoint* CPointList::addPoint(int x,int y)
{
   CPoint* newPoint = new CPoint(x,y);
   m_points.AddTail(newPoint);

   return newPoint;
}

CPoint* CPointList::addPoint(const CPoint& point)
{
   CPoint* lastPoint = NULL;
   CPoint* newPoint;

   if (m_points.GetCount() > 0)
   {
      lastPoint = m_points.GetTail();
   }

   if (lastPoint == NULL || !(lastPoint->x == point.x && lastPoint->y == point.y))
   {
      newPoint = new CPoint(point);
      m_points.AddTail(newPoint);
   }
   else
   {
      newPoint = lastPoint;
   }

   return newPoint;
}

void CPointList::removePoint(POSITION pos)
{
   CPoint* point = m_points.GetAt(pos);
   m_points.RemoveAt(pos);
   delete point;
}

void CPointList::removeCollinearPoints()
{
   int minX = INT_MAX;

   if (m_points.GetCount() > 2)
   {
      CPoint* prevPoint = m_points.GetTail();
      POSITION pos      = m_points.GetHeadPosition();
      POSITION prevPos;
      POSITION pointPos = pos;
      CPoint* point     = m_points.GetNext(pos);

      __int64 prevDx    = point->x - prevPoint->x;
      __int64 prevDy    = point->y - prevPoint->y;
      __int64 dx,dy;

      for (int loopIndex=0;pos != NULL;loopIndex++)
      {
         prevPoint = point;
         prevPos   = pointPos;
         pointPos  = pos;
         point     = m_points.GetNext(pos);

         if (point->x < minX)
         {
            minX = point->x;
         }

         dx = point->x - prevPoint->x;
         dy = point->y - prevPoint->y;

         if ((dx * prevDy) == (dy * prevDx))
         {
            removePoint(prevPos);
         }

         prevDx = dx;
         prevDy = dy;
      }
   }
}

CPolyList* CPointList::getPolyList(double pixelsPerPageUnit) const
{
   CPolyList* polyList = NULL;

   if (m_points.GetCount() > 0)
   {
      polyList = new CPolyList();
      CPoly* poly = new CPoly;
      polyList->AddTail(poly);

      poly->setFilled(true);

      for (POSITION pos = m_points.GetHeadPosition();pos != NULL;)
      {
         CPoint* point = m_points.GetNext(pos);

         CPoint2d point2d(point->x / pixelsPerPageUnit,point->y  / pixelsPerPageUnit);
         poly->addVertex(point2d);
      }

      poly->close();
   }

   return polyList;
}

//_____________________________________________________________________________
RegionRectRelationTag getOtherSideRectRelation(RegionRectRelationTag relation)
{
   RegionRectRelationTag otherSideRelation;

   switch (relation)
   {
   case rectLeftOf:            otherSideRelation = rectRightOf;           break;
   case rectLeftOfGoingDown:   otherSideRelation = rectRightOfGoingDown;  break;
   case rectLeftOfGoingUp:     otherSideRelation = rectRightOfGoingUp;    break;
   case rectRightOf:           otherSideRelation = rectLeftOf;            break;
   case rectRightOfGoingDown:  otherSideRelation = rectLeftOfGoingDown;   break;
   case rectRightOfGoingUp:    otherSideRelation = rectLeftOfGoingUp;     break;
   }

   return otherSideRelation;
}

RegionRectRelationTag getOppositeRectRelation(RegionRectRelationTag relation)
{
   RegionRectRelationTag oppositeRelation;

   switch (relation)
   {
   case rectLeftOf:            oppositeRelation = rectRightOf;           break;
   case rectLeftOfGoingDown:   oppositeRelation = rectRightOfGoingUp;    break;
   case rectLeftOfGoingUp:     oppositeRelation = rectRightOfGoingDown;  break;
   case rectRightOf:           oppositeRelation = rectLeftOf;            break;
   case rectRightOfGoingDown:  oppositeRelation = rectLeftOfGoingUp;     break;
   case rectRightOfGoingUp:    oppositeRelation = rectLeftOfGoingDown;   break;
   }

   return oppositeRelation;
}

//_____________________________________________________________________________
CRegionPolygon::CRegionPolygon(CCEtoODBDoc& camCadDoc,CPolyList& polyList,double pixelsPerPageUnit)
: m_camCadDoc(camCadDoc)
, m_pixelsPerPageUnit(pixelsPerPageUnit)
{
   CRgn* rgn = CreateRgn(camCadDoc,polyList,pixelsPerPageUnit);
   m_rects.set(*rgn);
   calcRectRelationships();

   //Region *region = RegionFromRgn(rgn);
   delete rgn;
   //return region;
}

CRegionPolygon::CRegionPolygon(CCEtoODBDoc& camCadDoc,const CRegionRectList& rects,double pixelsPerPageUnit)
: m_camCadDoc(camCadDoc)
, m_rects(rects,true)
, m_pixelsPerPageUnit(pixelsPerPageUnit)
{
   calcRectRelationships();
}

CRegionPolygon::CRegionPolygon(CCEtoODBDoc& camCadDoc,const Region& region,double pixelsPerPageUnit)
: m_camCadDoc(camCadDoc)
, m_rects(region,true)
, m_pixelsPerPageUnit(pixelsPerPageUnit)
{
   calcRectRelationships();
}

void CRegionPolygon::calcRectRelationships()
{
   CRegionRect* prevRegionRect = NULL;
   CRegionRectList row0(false);
   CRegionRectList row1(false);
   CRegionRectList* currentRow  = &row0;
   CRegionRectList* previousRow = &row1;
   CRegionRectList* tempRow;
   int completedRowCount = 0;

   for (POSITION pos = m_rects.GetHeadPosition();pos != NULL;)
   {
      CRegionRect* regionRect = m_rects.GetNext(pos);
      bool lastElementFlag = (pos == NULL);

      if (prevRegionRect != NULL)
      {
         bool sameRowFlag = false;

         if (prevRegionRect->getMinY() == regionRect->getMinY())
         {
            if (prevRegionRect->getMaxX() < regionRect->getMinX())
            {
               sameRowFlag = true;
            }
            else
            {
               logicError(1);
            }
         }

         if (sameRowFlag)
         {
            regionRect->setRelatedRect(rectLeftOf,prevRegionRect);
            prevRegionRect->setRelatedRect(rectRightOf,regionRect);
         }
         else
         {
            completedRowCount++;

            tempRow     = previousRow;
            previousRow = currentRow;
            currentRow  = tempRow;

            currentRow->empty();
         }

         if (completedRowCount > 0)
         {
            for (POSITION previousRowPos = previousRow->GetHeadPosition();previousRowPos != NULL;)
            {
               CRegionRect* previousRowRect = previousRow->GetNext(previousRowPos);

               bool touchingFlag = !((previousRowRect->getMinX() > regionRect->getMaxX()) ||
                                     (previousRowRect->getMaxX() < regionRect->getMinX())    );

               if (touchingFlag)
               {
                  if (regionRect->getRelatedRect(rectLeftOfGoingUp) == NULL)
                  {
                     // only the first touching rect in the previous row should be related by rectLeftOfGoingUp
                     regionRect->setRelatedRect(rectLeftOfGoingUp,previousRowRect);
                  }

                  // the last touching rect in the previous row should be related by rectRightOfGoingUp
                  regionRect->setRelatedRect(rectRightOfGoingUp,previousRowRect);

                  if (previousRowRect->getRelatedRect(rectLeftOfGoingDown) == NULL)
                  {
                     // only the first touching rect in the current row should be related by rectLeftOfGoingDown
                     previousRowRect->setRelatedRect(rectLeftOfGoingDown,regionRect);
                  }

                  // the last touching rect in the current row should be related by rectRightOfGoingDown
                  previousRowRect->setRelatedRect(rectRightOfGoingDown,regionRect);
               }
            }
         }
      }

      currentRow->AddTail(regionRect);

      prevRegionRect = regionRect;
   }

   //m_rects.dump(CDebugWriteFormat::getWriteFormat(),-1);
}

void CRegionPolygon::updateRelations()
{
   if (m_leftSideFlag)
   {
      m_siblingRectRelation = rectLeftOf;
      m_nextRectRelation    = (m_goingDownFlag ? rectLeftOfGoingDown : rectLeftOfGoingUp);
   }
   else
   {
      m_siblingRectRelation = rectRightOf;
      m_nextRectRelation    = (m_goingDownFlag ? rectRightOfGoingDown : rectRightOfGoingUp);
   }
}

void CRegionPolygon::setGoingDownFlag(bool flag)
{
   m_goingDownFlag = flag;
   updateRelations();
}

void CRegionPolygon::setLeftSideFlag(bool flag)
{
   m_leftSideFlag = flag;
   updateRelations();
}

void CRegionPolygon::toggleGoingDownFlag()
{
   m_goingDownFlag = !m_goingDownFlag;
   updateRelations();
}

void CRegionPolygon::toggleLeftSideFlag()
{
   m_leftSideFlag = !m_leftSideFlag;
   updateRelations();
}

CPoint CRegionPolygon::getPoint(CRegionRect& rect,int pointOrdinate) const
{
   //    1----0
   //    |    |
   //    2----3

   CPoint point;

   bool ccwFlag = (getGoingDownFlag() == getLeftSideFlag());
   int initialOrdinate;

   if (getGoingDownFlag())
   {
      initialOrdinate = (getLeftSideFlag() ? 1 : 0);
   }
   else
   {
      initialOrdinate = (getLeftSideFlag() ? 2 : 3);
   }

   int ordinate = ((initialOrdinate + (ccwFlag ? pointOrdinate : -pointOrdinate)) + 4) % 4;

   switch (ordinate)
   {
   case 0:  point.x = rect.getMaxX();  point.y = rect.getMaxY();  break;
   case 1:  point.x = rect.getMinX();  point.y = rect.getMaxY();  break;
   case 2:  point.x = rect.getMinX();  point.y = rect.getMinY();  break;
   case 3:  point.x = rect.getMaxX();  point.y = rect.getMinY();  break;
   }

   return point;
}

// returns point list with the first point the upper right coordinate of rect[0] and
// the last point the lower right coordinate of rect[0]
bool CRegionPolygon::getPerimeter(CPointList& perimeterPointList)
{
   bool retval = false;
   int perimeterIndex = 0;
   int maxArea = 0;
   int rectCount = m_rects.GetCount();
   m_rects.initPerimeterIndices();

   for (POSITION pos = m_rects.GetHeadPosition();pos != NULL;perimeterIndex++)
   {
      POSITION startPos = pos;
      CRegionRect* rootRect = m_rects.GetNext(pos);

      if (rootRect->getPerimeterIndex() < 0)
      {
         CPointList subPerimeterPointList;
         int area = 0;

         if (getPerimeter(startPos,perimeterIndex,subPerimeterPointList,area,rectCount))
         {
            if (area > maxArea)
            {
               maxArea = area;
               perimeterPointList.empty();
               perimeterPointList.takeData(subPerimeterPointList);

               retval = true;
            }
         }
      }
   }

   return retval;
}

bool CRegionPolygon::getPerimeter(POSITION startPos,int perimeterIndex,CPointList& perimeterPointList,int& area,int& rectCount)
{
   bool retval = false;

   perimeterPointList.empty();
   area = 0;
   CWriteFormat* writeFormat = NULL;
   //CWriteFormat* writeFormat = &(CDebugWriteFormat::getWriteFormat());

   setGoingDownFlag(true);
   setLeftSideFlag(true);
   int maxLoopCount = m_rects.GetCount() * 4;

   if (rectCount > 0)
   {
      CRegionRect* firstRect       = m_rects.GetAt(startPos);
      CRegionRect* rect            = firstRect;

      perimeterPointList.addPoint(getPoint(*rect,2));
      perimeterPointList.addPoint(getPoint(*rect,3));

      if (rectCount > 1)
      {
         for (int loopIndex = 0;;loopIndex++)
         {
            if (rect->getPerimeterIndex() < 0)
            {
               rect->setPerimeterIndex(perimeterIndex);
               rectCount--;
               area += rect->getArea();
            }

            if (loopIndex > maxLoopCount)
            {
               logicError(4);
               break;
            }

            if (writeFormat != NULL)
            {
writeFormat->writef("loop = %3d, rectIndex = %3d, %s-%s  -  ",loopIndex,rect->getIndex(),
(getLeftSideFlag() ? "left " : "right"),(getGoingDownFlag() ? "down" : "up  "));
            }

            CRegionRect* siblingRect = rect->getRelatedRect(getSiblingRectRelation());
            CRegionRect* nextRect    = rect->getRelatedRect(getNextRectRelation());

            if (nextRect != NULL && 
                siblingRect != NULL && siblingRect->getRelatedRect(getOtherSideRectRelation(getNextRectRelation())) == nextRect)
            {  
               // columns joining at nextRect (interior traversal)
               perimeterPointList.addPoint(getPoint(*rect,0));
               perimeterPointList.addPoint(getPoint(*rect,1));

               toggleGoingDownFlag();
               toggleLeftSideFlag();

               nextRect = siblingRect;

               if (writeFormat != NULL)
               {
writeFormat->writef("columns joined, nextRect = %d\n",nextRect->getIndex());
               }
            }
            else if (nextRect == NULL)
            {
               // end of a column, traverse back the other way
               perimeterPointList.addPoint(getPoint(*rect,0));
               perimeterPointList.addPoint(getPoint(*rect,1));
               perimeterPointList.addPoint(getPoint(*rect,2));
               perimeterPointList.addPoint(getPoint(*rect,3));

               toggleGoingDownFlag();
               toggleLeftSideFlag();

               nextRect    = rect->getRelatedRect(getNextRectRelation());

               if (nextRect == NULL)
               {
                  logicError(2);
                  break;
               }

               siblingRect = rect->getRelatedRect(getSiblingRectRelation());

               if (siblingRect != NULL && siblingRect->getRelatedRect(getOtherSideRectRelation(getNextRectRelation())) == nextRect)
               {  
                  // columns joining at nextRect (interior traversal)

                  toggleGoingDownFlag();
                  toggleLeftSideFlag();

                  nextRect = siblingRect;

                  if (writeFormat != NULL)
                  {
writeFormat->writef("end of column, other side of column joined, nextRect = %d\n",nextRect->getIndex());
                  }
               }
               else
               {
                  if (writeFormat != NULL)
                  {
writeFormat->writef("end of column, nextRect = %d\n",nextRect->getIndex());
                  }
               }
            }
            else
            {
               // continuation of column
               perimeterPointList.addPoint(getPoint(*rect,0));
               perimeterPointList.addPoint(getPoint(*rect,1));

               if (writeFormat != NULL)
               {
writeFormat->writef("continuation of column, nextRect = %d\n",nextRect->getIndex());
               }
            }

            rect            = nextRect;

            if (rect == firstRect)
            {
               retval = true;
               break;
            }
         }
      }
      else
      {
         perimeterPointList.addPoint(getPoint(*rect,0));
         perimeterPointList.addPoint(getPoint(*rect,1));

         if (rect->getPerimeterIndex() < 0)
         {
            rect->setPerimeterIndex(perimeterIndex);
            rectCount--;
            area += rect->getArea();
         }

         retval = true;
      }
   }

   if (perimeterPointList.GetCount() > 0)
   {
      perimeterPointList.moveHeadToTail();
      perimeterPointList.removeCollinearPoints();
   }

   return retval;
}

CPolyList* CRegionPolygon::getPolys()
{
   CPolyList* polyList = NULL;
   CPointList perimeterPointList;

   if (getPerimeter(perimeterPointList))
   {
      polyList = perimeterPointList.getPolyList(m_pixelsPerPageUnit);
      polyList->setWidthIndex(m_camCadDoc.getZeroWidthIndex());
   }

   return polyList;
}

CPolyList* CRegionPolygon::getRectangles()
{
   CPolyList *polyList = new CPolyList;
   
   for (POSITION rectPos = m_rects.GetHeadPosition();rectPos != NULL;)
   {
      CRegionRect* rect = m_rects.GetNext(rectPos);

      CPoly* poly = new CPoly();
      poly->setWidthIndex(m_camCadDoc.getZeroWidthIndex());
      poly->setClosed(true);
      poly->setFilled(false);

      poly->addVertex(rect->getMinX()/m_pixelsPerPageUnit,rect->getMinY()/m_pixelsPerPageUnit);
      poly->addVertex(rect->getMaxX()/m_pixelsPerPageUnit,rect->getMinY()/m_pixelsPerPageUnit);
      poly->addVertex(rect->getMaxX()/m_pixelsPerPageUnit,rect->getMaxY()/m_pixelsPerPageUnit);
      poly->addVertex(rect->getMinX()/m_pixelsPerPageUnit,rect->getMaxY()/m_pixelsPerPageUnit);
      poly->addVertex(rect->getMinX()/m_pixelsPerPageUnit,rect->getMinY()/m_pixelsPerPageUnit);

      polyList->AddTail(poly);
   }

   return polyList;
}

CPolyList* CRegionPolygon::getSmoothedPolys()
{
   CPolyList* polyList = NULL;
   CPointList perimeterPointList,smoothedPerimeterList;

   if (getPerimeter(perimeterPointList))
   {
      perimeterPointList.removeCollinearPoints();

      if (perimeterPointList.GetCount() > 3)
      {
         perimeterPointList.addPoint(perimeterPointList.getHead());

         POSITION pos = perimeterPointList.GetHeadPosition();

         CPoint* prevPoint = perimeterPointList.GetNext(pos);
         CPoint* point     = perimeterPointList.GetNext(pos);
         CPoint* nextPoint;

         int dx,dy;
         bool removePointFlag;

         for (int loopIndex=0;;loopIndex++)
         {
            smoothedPerimeterList.addPoint(*prevPoint);

            if (pos == NULL)
            {
               //smoothedPerimeterList.addPoint(*point);
               break;
            }

            nextPoint    = perimeterPointList.GetNext(pos);

            CPoint2d prevPoint2d(prevPoint->x / m_pixelsPerPageUnit,prevPoint->y / m_pixelsPerPageUnit);
            CPoint2d point2d(        point->x / m_pixelsPerPageUnit,    point->y / m_pixelsPerPageUnit);
            CPoint2d nextPoint2d(nextPoint->x / m_pixelsPerPageUnit,nextPoint->y / m_pixelsPerPageUnit);

            dx = nextPoint->x - prevPoint->x;
            dy = nextPoint->y - prevPoint->y;

            // dx and dy can't be 0
            if (dx == 0 || dy == 0)
            {
               logicError(5);
            }

            removePointFlag = false;

            if (dx == 1 || dx == -1 || dy == 1 || dy == -1)
            {
               if ((((dx < 0) == (dy < 0)) && (point->y == nextPoint->y)) ||
                   (((dx < 0) != (dy < 0)) && (point->y == prevPoint->y))    )
               {
                  removePointFlag = true;
               }
            }

            if (removePointFlag)
            {
               prevPoint = nextPoint;

               if (pos == NULL) continue;

               point    = perimeterPointList.GetNext(pos);
            }
            else
            {
               prevPoint = point;
               point     = nextPoint;
            }
         }
      }

      smoothedPerimeterList.removeCollinearPoints();
      polyList = smoothedPerimeterList.getPolyList(m_pixelsPerPageUnit);
   }

   return polyList;
}

void debugSawRgn(int id,const CExtendedRgn& rgn,const CExtendedRgn& sawRgn)
{
   CRgn* pSawRgn = (CRgn*)(&sawRgn);

   for (int reducedSize = rgn.getRegionDataRectCount();reducedSize > 0;reducedSize--)
   {
      CExtendedRgn reducedRgn(&rgn);
      reducedRgn.setRegionDataRectCount(reducedSize);

      CExtendedRgn resultRgn;
      int regionType = resultRgn.CombineRgn(&reducedRgn,pSawRgn,RGN_DIFF);

      if (regionType != ERROR)
      {
         CDebug::addDebugRegion(*(getActiveDocument()),reducedRgn,"db reducedRgn %d%s",id,reducedRgn.isValid() ? "" : " invalid");
         CDebug::addDebugRegion(*(getActiveDocument()),sawRgn    ,"db sawRgn %d%s"    ,id,    sawRgn.isValid() ? "" : " invalid");

         break;
      }
   }
}

bool CRegionPolygon::splitRegion(CSplitRegionPolygons& regionPolygons,
   int lineWidthPixels, int maxFeatureSizePixels,int webSizePixels,bool stripeFlag)
{
   bool retval = true;

   CExtendedRgn* rgn = m_rects.getRgn();
   CIntExtent extent(m_rects.getExtent());

   // Case dts0100472908
   // Actual rect we get here has been reduced by line width (line width is based on corner radius).
   // For split, put back half a line width for each boundary line, same as adding one line width back into extent.

   int yNumStrips = round(ceil(((double)(extent.getYsize() + lineWidthPixels + webSizePixels)) / (maxFeatureSizePixels + webSizePixels)));
   int xNumStrips = (stripeFlag ? 1 : round(ceil(((double)(extent.getXsize() + lineWidthPixels + webSizePixels)) / (maxFeatureSizePixels + webSizePixels))));

   regionPolygons.setDimensions(xNumStrips,yNumStrips);

   CArray<int,int> tops,bottoms,lefts,rights;
   tops.SetSize(0,yNumStrips);
   bottoms.SetSize(0,yNumStrips);
   lefts.SetSize(0,xNumStrips);
   rights.SetSize(0,xNumStrips);

   double pixelsPerCell = (extent.getYsize() - (yNumStrips - 1) * webSizePixels) / ((double)yNumStrips);
   double sawLine0 = extent.getYmin();
   double sawSpacing = pixelsPerCell + webSizePixels;

   tops.Add(extent.getYmin());

   for (int sawLineIndex = 1;;sawLineIndex++)
   {
      int nextCellTop    = round(sawLine0 + sawLineIndex*sawSpacing);  // top of next cell
      int thisCellBottom = nextCellTop - webSizePixels;                // bottom of this cell

      bottoms.Add(thisCellBottom);

      if (nextCellTop >= extent.getYmax())
      {
         break;
      }

      tops.Add(nextCellTop);

      // normalized rects - left must be less than right, top is less than bottom

      CRect rect(extent.getXmin() - 1,thisCellBottom,extent.getXmax() + 1,nextCellTop);
      rect.NormalizeRect();

      CExtendedRgn sawRgn;
      BOOL success = sawRgn.CreateRectRgnIndirect(&rect);

      CExtendedRgn resultRgn;
      int regionType = resultRgn.CombineRgn(rgn,&sawRgn,RGN_DIFF);

      if (regionType == ERROR)
      {
         retval = false;

         int id = CDebug::getNextDebugRegionId();

         getDebugWriteFormat().writef("rgn %d\n",id);
         rgn->dump(getDebugWriteFormat());

         getDebugWriteFormat().writef("sawRgn %d\n",id);
         sawRgn.dump(getDebugWriteFormat());

         if (id < 20)
         {
            CDebug::addDebugRegion(*(getActiveDocument()),*rgn  ,"rgn %d%s"   ,id,  rgn->isValid() ? "" : " invalid");
            CDebug::addDebugRegion(*(getActiveDocument()),sawRgn,"sawRgn %d%s",id,sawRgn.isValid() ? "" : " invalid");

            debugSawRgn(id,*rgn,sawRgn);
         }

         int iii = 3;
      }
      else
      {
         rgn->CopyRgn(&resultRgn);
      }
   }

   pixelsPerCell = (extent.getXsize() - (xNumStrips - 1) * webSizePixels) / ((double)xNumStrips);
   sawLine0 = extent.getXmin();
   sawSpacing = pixelsPerCell + webSizePixels;

   lefts.Add(extent.getXmin());

   for (int sawLineIndex = 1;;sawLineIndex++)
   {
      int nextCellLeft  = round(sawLine0 + sawLineIndex*sawSpacing);  // left of next cell
      int thisCellRight = nextCellLeft - webSizePixels;               // right of this cell

      rights.Add(thisCellRight);

      if (nextCellLeft >= extent.getXmax())
      {
         break;
      }

      lefts.Add(nextCellLeft);

      CRect rect(thisCellRight,extent.getYmin() - 1,nextCellLeft,extent.getYmax() + 1);
      rect.NormalizeRect();

      CExtendedRgn sawRgn;
      BOOL success = sawRgn.CreateRectRgnIndirect(&rect);

      CExtendedRgn resultRgn;
      int regionType = resultRgn.CombineRgn(rgn,&sawRgn,RGN_DIFF);

      if (regionType == ERROR)
      {
         retval = false;

         int id = CDebug::getNextDebugRegionId();

         getDebugWriteFormat().writef("rgn %d\n",id);
         rgn->dump(getDebugWriteFormat());

         getDebugWriteFormat().writef("sawRgn %d\n",id);
         sawRgn.dump(getDebugWriteFormat());

         if (id < 20)
         {
            CDebug::addDebugRegion(*(getActiveDocument()),*rgn  ,"rgn %d%s"   ,id,  rgn->isValid() ? "" : " invalid");
            CDebug::addDebugRegion(*(getActiveDocument()),sawRgn,"sawRgn %d%s",id,sawRgn.isValid() ? "" : " invalid");

            debugSawRgn(id,*rgn,sawRgn);
         }

         int iii = 3;
      }
      else
      {
         rgn->CopyRgn(&resultRgn);
      }
   }

   CTypedPtrArrayContainer<CRgnRectList*> regionLists;
   int rectCount = rgn->getRegionDataRectCount();
   int xIndex,yIndex;

   for (int index = 0;index < rectCount;index++)
   {
      RECT* rect = rgn->getRegionDataRect(index);

      for (xIndex = 0;xIndex < lefts.GetSize();xIndex++)
      {
         if (rect->left  >=  lefts.GetAt(xIndex) &&
             rect->right <= rights.GetAt(xIndex)     )
         {
            break;
         }
      }

      for (yIndex = 0;yIndex < tops.GetSize();yIndex++)
      {
         if (rect->top    >=    tops.GetAt(yIndex) &&
             rect->bottom <= bottoms.GetAt(yIndex)     )
         {
            break;
         }
      }

      if (xIndex < lefts.GetSize() && yIndex < tops.GetSize())
      {
         int index = xIndex * yNumStrips + yIndex;
         CRgnRectList* regionList = NULL;

         if (index < regionLists.GetSize())
         {
            regionList = regionLists.GetAt(index);
         }

         if (regionList == NULL)
         {
            regionList = new CRgnRectList();
            regionLists.SetAtGrow(index,regionList);
         }

         regionList->addRectCopy(*rect);
      }
      else
      {
         logicError(8);
      }
   }

   for (int index = 0;index < regionLists.GetSize();index++)
   {
      CRgnRectList* rectList = regionLists.GetAt(index);

      if (rectList != NULL)
      {
         CRegionRectList regionRectList(*(rectList->getRgn()));
         CRegionPolygon* cellRegionPolygon = new CRegionPolygon(m_camCadDoc,regionRectList,m_pixelsPerPageUnit);

         int yIndex = index % yNumStrips;
         int xIndex = index / yNumStrips;

         regionPolygons.setAt(xIndex,yIndex,cellRegionPolygon);
      }
   }

   delete rgn;

   return retval;
}

//void CRegionPolygon::splitRegion(CSplitRegionPolygons& regionPolygons,int maxFeatureSizePixels,int webSizePixels)
//{
//   CIntExtent extent = m_rects.getExtent();
//
//   CSplitRegionPolygons horizontalStripes;
//   horizontallySplitRegion(horizontalStripes,*this,maxFeatureSizePixels,webSizePixels,extent);
//
//   for (POSITION pos = horizontalStripes.GetHeadPosition();pos != NULL;)
//   {
//      CRegionPolygon* stripePolygon = horizontalStripes.GetNext(pos);
//
//      CSplitRegionPolygons cells;
//      verticallySplitRegion(cells,*stripePolygon,maxFeatureSizePixels,webSizePixels,extent);
//
//      for (POSITION pos = cells.GetHeadPosition();pos != NULL;)
//      {
//         CRegionPolygon* cell = cells.GetNext(pos);
//
//         regionPolygons.AddTail(&cells);
//      }
//   }
//}

void CRegionPolygon::verticallySplitRegion(CSplitRegionPolygons& regionPolygons,const CRegionPolygon& polygon,
   int maxFeatureSizePixels,int webSizePixels,const CIntExtent& extent)
{
   int xSplits = 1 + ((extent.getXsize() + webSizePixels - 1) / (maxFeatureSizePixels + webSizePixels));
}

bool CRegionPolygon::horizontallyBisect(int bisectorLine)
{
   bool retval = false;
   CSplitRegionPolygons regionPolygons;

   CRegionRectList lowerRects;
   int splitCount = 0;
   int rectIndex = 0;

   for (POSITION pos = m_rects.GetHeadPosition();pos != NULL;rectIndex++)
   {
      POSITION prevPos = pos;
      CRegionRect* regionRect = m_rects.GetNext(pos);

      if (regionRect->getMaxY() > bisectorLine && regionRect->getMinY() < bisectorLine)
      {
         CRegionRect* bottomRect = new CRegionRect(*regionRect,splitCount++);
         regionRect->setMinY(bisectorLine);
         bottomRect->setMaxY(bisectorLine);

         lowerRects.AddTail(bottomRect);
      } 
      else if (regionRect->getMaxY() < bisectorLine)
      {
         if (rectIndex == 0)
         {  // if all rects lie below the bisector line, then don't bisect the region polygon.
            break;
         }

         m_rects.RemoveAt(prevPos);
         lowerRects.AddTail(regionRect);
      }
   }

   if (lowerRects.GetSize() > 0)
   {
      retval = true;


   }

   return retval;
}

void CRegionPolygon::horizontallySplitRegion(CSplitRegionPolygons& regionPolygons,const CRegionPolygon& polygon,
   int maxFeatureSizePixels,int webSizePixels,const CIntExtent& extent)
{
   // n * cellWidth + (n - 1) * webWidth >= extent
   // n >= (extent + webWidth) / (cellWidth + webWidth)

   int numStripes = round(ceil(((double)(extent.getYsize() + webSizePixels)) / (maxFeatureSizePixels + webSizePixels)));
   double pixelsPerCell = (extent.getYsize() - (numStripes - 1) * webSizePixels) / ((double)numStripes);
   double sawLine0 = extent.getYsize();
   double sawSpacing = pixelsPerCell + webSizePixels;

   for (int sawLineIndex = 1;;sawLineIndex++)
   {
      int bisectorLine = round(sawLine0 - sawLineIndex*sawSpacing);

      if (bisectorLine <= extent.getYmin())
      {
         break;
      }

      horizontallyBisect(bisectorLine);
      horizontallyBisect(bisectorLine + webSizePixels);
   }
}

bool CRegionPolygon::logicError(int errorNumber)
{
   static unsigned int status = 0;

   status |= (1 << errorNumber);

   return (status != 0);
}

//_____________________________________________________________________________
CSplitRegionPolygons::CSplitRegionPolygons()
{
   m_xSize = 0;
   m_ySize = 0;
}

void CSplitRegionPolygons::setDimensions(int xSize,int ySize)
{
   m_polygons.empty();

   m_xSize = xSize;
   m_ySize = ySize;

   m_polygons.SetSize(0,m_xSize * m_ySize);
}

CRegionPolygon* CSplitRegionPolygons::getAt(int x,int y)
{
   CRegionPolygon* polygon = NULL;

   if (x >= 0 && x < m_xSize && y >= 0 && y < m_ySize)
   {
      int index = getIndex(x,y);

      polygon = m_polygons.GetAt(index);
   }
   else
   {
   }

   return polygon;
}

void CSplitRegionPolygons::setAt(int x,int y,CRegionPolygon* polygon)
{
   if (x >= 0 && x < m_xSize && y >= 0 && y < m_ySize)
   {
      int index = getIndex(x,y);

      m_polygons.SetAtGrow(index,polygon);
   }
   else
   {
   }
}

int CSplitRegionPolygons::getIndex(int x, int y)
{
   int index = x * m_ySize + y;

   return index;
}





