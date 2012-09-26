// $Header: /CAMCAD/5.0/Rgn.cpp 32    6/21/07 8:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "data.h"
#include "rgn.h"
#include "ccdoc.h"
#include "polylib.h"
#include "outline.h"
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static BOOL CanWrapRgn(RECT *rects, int rectCount);
static CPolyList *WrapRects(RECT *rects, int rectCount, float scaleFactor);
static CPolyList *BuildRectPolys(RECT *rects, int rectCount, float scaleFactor);

/******************************************************************************
* CreateRgn
*/
CRgn* CreateRgn(CCEtoODBDoc *doc, CPolyList *polyList, float scaleFactor)
{
   CRgn* rgn = NULL;

   if (doc != NULL && polyList != NULL)
   {
      rgn = CreateRgn(*doc,*polyList,(double)scaleFactor);
   }

   return rgn;
}

CRgn* CreateRgn(CCEtoODBDoc& doc,CPolyList& polyList,double scaleFactor)
{
   CRgn* rgn = new CRgn;
   rgn->CreateRectRgn(0, 0, 0, 0);
   
   for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
   {
      CPoly* poly = polyList.GetNext(polyPos);

      if (poly->isClosed())
      {
         CRgn* polyRgn = GetPolyRgn(*poly,scaleFactor,doc.getPageUnits());

         if (polyRgn == NULL)
         {
            continue;
         }

         if (poly->isVoid())
         {
            rgn->CombineRgn(rgn, polyRgn, RGN_DIFF);
         }
         else
         {
            rgn->CombineRgn(rgn, polyRgn, RGN_OR);
         }

         delete polyRgn;
      }
      else
      {
         if (poly->getPntList().GetCount() < 2)
         {
            continue;
         }

         double widthRadius = 0;

         if (poly->getWidthIndex() >= 0)
         {
            widthRadius = doc.getWidthTable()[poly->getWidthIndex()]->getSizeA() / 2;
         }

         POSITION pntPos = poly->getPntList().GetHeadPosition();
         CPnt *pnt = poly->getPntList().GetNext(pntPos);

         while (pntPos)
         {
            CPnt* prevPnt = pnt;
            pnt = poly->getPntList().GetNext(pntPos);

            CPoly* segmentPoly = GetSegmentOutline(prevPnt->x, prevPnt->y, pnt->x, pnt->y, prevPnt->bulge, widthRadius, 0);

            CRgn* segmentPolyRgn = GetPolyRgn(*segmentPoly, scaleFactor,doc.getPageUnits());
            rgn->CombineRgn(rgn, segmentPolyRgn, RGN_OR);

            delete segmentPolyRgn;
            delete segmentPoly;
         }
      }
   }

   return rgn;
}

/******************************************************************************
* GetPolyRgn
*/
CRgn* GetPolyRgn(CPoly *poly, float scaleFactor,PageUnitsTag pageUnits)
{
   CRgn* rgn = NULL;

   if (poly != NULL)
   {
      rgn = GetPolyRgn(*poly,(double)scaleFactor,pageUnits);
   }

   return rgn;
}

CRgn* GetPolyRgn(CPoly& poly,double scaleFactor,PageUnitsTag pageUnits)
{
   bool HasBulge = false;

   // check for bulges   
   for (POSITION pntPos = poly.getPntList().GetHeadPosition();pntPos != NULL;)
   {
      if (fabs(poly.getPntList().GetNext(pntPos)->bulge) >= BULGE_THRESHOLD)
      {
         HasBulge = TRUE;
         break;
      }
   }

   CPoly* polyToConvert = &poly;

	if (poly.hasBulge())
	{
		polyToConvert = new CPoly(poly);
		polyToConvert->vectorize(pageUnits);
	}

   int count = polyToConvert->getPntList().GetCount();
   POINT* pts = (POINT*)calloc(count, sizeof(POINT));
   int i=0;
   
   for (POSITION pntPos = polyToConvert->getPntList().GetHeadPosition();pntPos != NULL;)
   {
      CPnt* pnt = polyToConvert->getPntList().GetNext(pntPos);

      pts[i].x   = round(pnt->x * scaleFactor);
      pts[i++].y = round(pnt->y * scaleFactor);
   }
   
   CRgn* polyRgn = new CRgn();
   polyRgn->CreatePolygonRgn(pts, count, ALTERNATE);

   free(pts);

   if (polyToConvert != &poly)
   {
      delete polyToConvert;
      polyToConvert = NULL;
   }

   return polyRgn;
}

/******************************************************************************
* GetRgnExtents
*/
void GetRgnExtents(CRgn *rgn, ExtentRect *extents, float scaleFactor)
{
   extents->left = extents->bottom = FLT_MAX;
   extents->right = extents->top = -FLT_MAX;

   int size = rgn->GetRegionData(NULL, 0);
   RGNDATA *rd = (RGNDATA*)calloc(size, 1);
   rgn->GetRegionData(rd, size);

   if ((int)rd->rdh.nCount < 1)
   {
      free(rd);
      extents->left = extents->bottom = extents->right = extents->top = 0;
      return;
   }

   RECT *rects = (RECT*)rd->Buffer;
   int rectCount = (int)rd->rdh.nCount;

   for (int i=0; i<rectCount; i++)
   {
      RECT *rect = &rects[i];

      if (rect->left < extents->left)
         extents->left = rect->left;
      if (rect->right > extents->right)
         extents->right = rect->right;
      if (rect->bottom < extents->bottom)
         extents->bottom = rect->bottom;
      if (rect->top > extents->top)
         extents->top = rect->top;
   }

   extents->left /= scaleFactor;
   extents->bottom /= scaleFactor;
   extents->right /= scaleFactor;
   extents->top /= scaleFactor;

   free(rd);
}

/******************************************************************************
* GetPolyFromRgn
*/
CPolyList *GetPolyFromRgn(CRgn *rgn, float scaleFactor)
{
   scaleFactor = 1.0f / scaleFactor;

   int size = rgn->GetRegionData(NULL, 0);
   RGNDATA *rd = (RGNDATA*)calloc(size, 1);
   rgn->GetRegionData(rd, size);

   if ((int)rd->rdh.nCount < 1)
   {
      free(rd);
      return NULL;
   }

   RECT *rects = (RECT*)rd->Buffer;
   int rectCount = (int)rd->rdh.nCount;

   CPolyList *polyList = NULL;

   if (CanWrapRgn(rects, rectCount))
      polyList = WrapRects(rects, rectCount, scaleFactor);
   else
      polyList = BuildRectPolys(rects, rectCount, scaleFactor);

   free(rd);

   return polyList;
}

/******************************************************************************
* CanWrapRgn
*/
static BOOL CanWrapRgn(RECT *rects, int rectCount)
{
   for (int i=0; i<rectCount-1; i++)
   {
      RECT *rect = &rects[i];
      RECT *nextRect = &rects[i+1];

      if (rect->bottom != nextRect->top) // windows regions are upside-down of cartesian coordinates
         return FALSE;
   }

   return TRUE;
}

/******************************************************************************
* WrapRects
*/
static CPolyList *WrapRects(RECT *rects, int rectCount, float scaleFactor)
{
   CPolyList *polyList = new CPolyList;

   CPoly *poly = new CPoly;
   polyList->AddTail(poly);
   poly->setWidthIndex(-1);
   poly->setClosed(true);
   poly->setHatchLine(false);
   poly->setFilled(false);
   poly->setVoid(false);
   poly->setHidden(false);
   poly->setFloodBoundary(false);
   poly->setBreakOut(false);
   poly->setThermalLine(false);

	int i=0;
   for (i=0; i<rectCount; i++)
   {
      RECT *rect = &rects[i];

      CPnt *pnt = new CPnt;
      pnt->x = scaleFactor * rect->left;
      pnt->y = scaleFactor * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = scaleFactor * rect->left;
      pnt->y = scaleFactor * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   for (i=rectCount-1; i>=0; i--)
   {
      RECT *rect = &rects[i];

      CPnt *pnt = new CPnt;
      pnt->x = scaleFactor * rect->right;
      pnt->y = scaleFactor * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = scaleFactor * rect->right;
      pnt->y = scaleFactor * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   RECT *rect = &rects[0];

   CPnt *pnt = new CPnt;
   pnt->x = scaleFactor * rect->left;
   pnt->y = scaleFactor * rect->top;
   pnt->bulge = 0;
   poly->getPntList().AddTail(pnt);

   return polyList;
}

/******************************************************************************
* BuildRectPolys
*/
static CPolyList *BuildRectPolys(RECT *rects, int rectCount, float scaleFactor)
{
   CPolyList *polyList = new CPolyList;

   for (int i=0; i<rectCount; i++)
   {
      RECT *rect = &rects[i];

      CPoly *poly = new CPoly;
      polyList->AddTail(poly);
      poly->setWidthIndex(-1);
      poly->setClosed(true);
      poly->setHatchLine(false);
      poly->setFilled(false);
      poly->setVoid(false);
      poly->setHidden(false);
      poly->setFloodBoundary(false);
      poly->setBreakOut(false);
      poly->setThermalLine(false);

      CPnt *pnt = new CPnt;
      pnt->x = scaleFactor * rect->left;
      pnt->y = scaleFactor * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = scaleFactor * rect->right;
      pnt->y = scaleFactor * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = scaleFactor * rect->right;
      pnt->y = scaleFactor * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = scaleFactor * rect->left;
      pnt->y = scaleFactor * rect->bottom;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = scaleFactor * rect->left;
      pnt->y = scaleFactor * rect->top;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   return polyList;
}

