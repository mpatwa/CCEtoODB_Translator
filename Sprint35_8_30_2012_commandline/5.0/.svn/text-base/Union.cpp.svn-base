// $Header: /CAMCAD/5.0/Union.cpp 20    6/21/07 8:28p Kurt Van Ness $
                                                                              
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "union.h"
#include "polylib.h"
#include "outline.h"
#include "graph.h"
#include "xform.h"
#include "measure.h"
#include "rgn.h"
#include "bb_lib.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include "c2cdefs.h"
#include "t2bdefs.h"
#include "t2cdefs.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef DEBUG_UNION

static CPolyList *GetThinLineParts(CCEtoODBDoc *doc, CPolyList *partList);
static void VectorizeParts(CPolyList *thinLinePartList,PageUnitsTag pageUnits);
static CPolyList *ConstuctUnion(CPolyList *thinLinePartList);
static int UnionPolys(CPolyList *results, CPoly *unionPoly, CPoly *polyToAdd);
static void GraphSegments(CCEtoODBDoc *doc, CPolyList *polyList, CString layerName);

static int thinWidthIndex;

/******************************************************************************
* TestUnion
*/
void TestUnion(CCEtoODBDoc *doc, BlockStruct *block)
{
   CPolyList regions;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_POLY)
         continue;

      POSITION polyPos = data->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = data->getPolyList()->GetNext(polyPos);

         CPoly *newPoly = new CPoly;
         newPoly->setClosed(poly->isClosed());
         newPoly->setFilled(poly->isFilled());
         newPoly->setWidthIndex(poly->getWidthIndex());
         POSITION pntPos = poly->getPntList().GetHeadPosition();
         while (pntPos)
         {
            CPnt *pnt = poly->getPntList().GetNext(pntPos);  
            CPnt *newPnt = new CPnt(*pnt);
            newPoly->getPntList().AddTail(newPnt);
         }

         regions.AddTail(newPoly);
      }
   }

   CPolyList *results = Union(doc, &regions);

   FreePolyList(results);
}

/******************************************************************************
* Union
*/
CPolyList *Union(CCEtoODBDoc *doc, CPolyList *partList)
{
   CWaitCursor wait;

   thinWidthIndex = Graph_Aperture("", T_ROUND, 0, 0, 0, 0, 0, 0, 0, 1, NULL);

#ifdef DEBUG_UNION
   GraphSegments(doc, partList, "Regions");
#endif

   // construct thin line closed polys from Regions
   CPolyList *thinLinePartList = GetThinLineParts(doc, partList);
#ifdef DEBUG_UNION
   GraphSegments(doc, thinLinePartList, "ThinLines");
#endif

   // vectorize arcs
   VectorizeParts(thinLinePartList,doc->getPageUnits());
#ifdef DEBUG_UNION
   GraphSegments(doc, thinLinePartList, "Vectorized");
#endif

   // construct union regions from vectorized thin-lined parts
   CPolyList *results = ConstuctUnion(thinLinePartList);

#ifdef DEBUG_UNION
   GraphSegments(doc, results, "Results");
#endif

   return results;
}

/******************************************************************************
* GetThinLineParts
*/
CPolyList *GetThinLineParts(CCEtoODBDoc *doc, CPolyList *partList)
{
   CPolyList *thinLinePartList = new CPolyList;

   while (partList->GetCount())
   {
      CPoly *poly = partList->removeHead();

      double widthRadius = 0;

      if (poly->getWidthIndex() >= 0)
         widthRadius = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() / 2;

      double cx, cy, radius;

      if (PolyIsCircle(poly, &cx, &cy, &radius)) // put simple circle only
      {
         CPoly *newPoly = new CPoly;
         newPoly->setClosed(true);
         newPoly->setFilled(false);
         newPoly->setWidthIndex(thinWidthIndex);
         CPnt *newPnt = new CPnt;
         newPnt->x = (DbUnit)(cx - radius - widthRadius);
         newPnt->y = (DbUnit)cy;
         newPnt->bulge = 1;
         newPoly->getPntList().AddTail(newPnt);
         newPnt = new CPnt;
         newPnt->x = (DbUnit)(cx + radius + widthRadius);
         newPnt->y = (DbUnit)cy;
         newPnt->bulge = 1;
         newPoly->getPntList().AddTail(newPnt);
         newPnt = new CPnt;
         newPnt->x = (DbUnit)(cx - radius - widthRadius);
         newPnt->y = (DbUnit)cy;
         newPnt->bulge = 0;
         newPoly->getPntList().AddTail(newPnt);
         thinLinePartList->AddTail(newPoly);
      }
      else
      {
         if (poly->isFilled()) // need to include fill
         {
            CPoly *newPoly = new CPoly;
            newPoly->setClosed(true);
            newPoly->setFilled(false);
            newPoly->setWidthIndex(thinWidthIndex);
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);
               CPnt *newPnt = new CPnt(*pnt);
               newPoly->getPntList().AddTail(newPnt);
            }

            thinLinePartList->AddTail(newPoly);
         }

         if (widthRadius > 0.001)
         {
            CPnt *pnt;
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            if (pntPos)
               pnt = poly->getPntList().GetNext(pntPos);
            while (pntPos)
            {
               CPnt *lastPnt = pnt;
               pnt = poly->getPntList().GetNext(pntPos);

               thinLinePartList->AddTail(GetSegmentOutline(lastPnt->x, lastPnt->y, pnt->x, pnt->y, lastPnt->bulge, widthRadius, thinWidthIndex));
            }
         }
         else if (poly->isClosed())
         {
            CPoly *newPoly = new CPoly;
            newPoly->setClosed(true);
            newPoly->setFilled(false);
            newPoly->setWidthIndex(thinWidthIndex);
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);
               CPnt *newPnt = new CPnt(*pnt);
               newPoly->getPntList().AddTail(newPnt);
            }

            thinLinePartList->AddTail(newPoly);
         }
   //    else // non-closed thin lines
      }

      delete poly;
   }

   return thinLinePartList;
}

/******************************************************************************
* VectorizeParts
*/
void VectorizeParts(CPolyList *thinLinePartList,PageUnitsTag pageUnits)
{
   POSITION polyPos = thinLinePartList->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = thinLinePartList->GetNext(polyPos);

      VectorizePoly(poly,pageUnits);
   }
}

/******************************************************************************
* ConstuctUnion
*
CPolyList *ConstuctUnion(CPolyList *thinLinePartList)
{
   DTransform xform;
   Point2 p1, p2;

   CPolyList *results = new CPolyList;

   while (thinLinePartList->GetCount())
   {
      CPoly *newPoly = thinLinePartList->RemoveHead();

      CPolyList polysToUnion;

      POSITION polyPos = results->GetHeadPosition();
      while (polyPos)
      {
         POSITION tempPos = polyPos;
         CPoly *resultPoly = results->GetNext(polyPos);

         // if thinLinePoly touches a resultPoly
         if (fabs(MeasurePolyToPoly(NULL, newPoly, &xform, TRUE, resultPoly, &xform, TRUE, &p1, &p2)) < SMALLNUMBER)
         {
            results->RemoveAt(tempPos);
            polysToUnion.AddTail(resultPoly);
         }
      }

      if (!polysToUnion.GetCount())
      {
         results->AddTail(newPoly);
         continue;
      }

      // union touching polys
      CPoly *unionPoly = newPoly;
      while (polysToUnion.GetCount())
      {
         CPoly *polyToAdd = polysToUnion.RemoveHead();
         
         UnionPolys(results, unionPoly, polyToAdd);
         if (polysToUnion.GetCount())
            unionPoly = results->RemoveTail();
      }
   }

   delete thinLinePartList;
   
   return results;
}

/******************************************************************************
* UnionPolys
*
BOOL UnionPolys(CPolyList *results, CPoly *unionPoly, CPoly *polyToAdd)
{
   T2_REGION region1 = BB_CreateRegion(&unionPoly->list);
   T2_REGION region2 = BB_CreateRegion(&polyToAdd->list);
   
   DML_LIST unionList = t2b_union_rr(region1, region2, NULL);

   BB_free_region(region1);
   BB_free_region(region2);

   if (!dml_length(unionList))
   {
      results->AddTail(polyToAdd);
      results->AddTail(unionPoly);
      
      // free unionList
      dml_destroy_list(unionList, BB_free_region);

      return FALSE;
   }
   
   FreePoly(unionPoly);
   FreePoly(polyToAdd);

   DML_ITEM rgnItem;
   DML_WALK_LIST(unionList, rgnItem)
   {
      T2_REGION region = (T2_REGION) dml_record(rgnItem);

      DML_ITEM loopItem;
      DML_WALK_LIST(T2_REGION_LOOP_LIST(region), loopItem)
      {
         T2_LOOP loop = (T2_LOOP)DML_RECORD(loopItem);

         CPoly *poly = new CPoly;
         results->AddTail(poly);
         poly->setClosed(true);
         poly->isFilled() = FALSE;
         poly->getWidthIndex() = thinWidthIndex;

         DML_ITEM edgeItem;
         DML_WALK_LIST(T2_LOOP_EDGE_LIST(loop), edgeItem)
         {
            T2_EDGE edge = (T2_EDGE)DML_RECORD(edgeItem);

            PT2 pt;
            t2c_ept0(edge, pt);

            CPnt *pnt = new CPnt;
            poly->getPntList().AddTail(pnt);
            pnt->x = (DbUnit)PT2_X(pt);
            pnt->y = (DbUnit)PT2_Y(pt);
            pnt->bulge = 0;

            CPnt *prevPnt = pnt;
            DML_WALK_LIST(T2_LOOP_EDGE_LIST(loop), edgeItem)
            {
               edge = (T2_EDGE)DML_RECORD(edgeItem);

               t2c_ept1(edge, pt);

               CPnt *pnt = new CPnt;
               poly->getPntList().AddTail(pnt);
               pnt->x = (DbUnit)PT2_X(pt);
               pnt->y = (DbUnit)PT2_Y(pt);
               pnt->bulge = 0;

               C2_CURVE curve = T2_EDGE_CURVE(edge);
               if (C2_CURVE_IS_ARC(curve))
               {
                  REAL sweep;
                  c2c_get_arc_sweep(curve, &sweep);
                  int dir;
                  c2c_get_arc_direction(curve, &dir);
                  prevPnt->bulge = (DbUnit)(tan(sweep/4) * dir);
                  if (T2_EDGE_REVERSED(edge))
                     prevPnt->bulge *= -1;
               }
               
               prevPnt = pnt;
            }
         }
      }
   }

   // free unionList
   dml_destroy_list(unionList, BB_free_region);

   return TRUE;
}

/******************************************************************************
* GraphSegments
*/
void GraphSegments(CCEtoODBDoc *doc, CPolyList *polyList, CString layerName)
{
   doc->PrepareAddEntity(doc->getFileList().GetHead());

   int layer = Graph_Level(layerName, "DEBUG_", FALSE);

   POSITION polyPos = polyList->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = polyList->GetNext(polyPos);

      DataStruct *data = Graph_PolyStruct(layer, 0, 0);
      CPoly *newPoly = Graph_Poly(data, (poly->getWidthIndex()>=0 ? poly->getWidthIndex() : 0), poly->isFilled(), 0, poly->isClosed());

      POSITION pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos)
      {
         CPnt *pnt = poly->getPntList().GetNext(pntPos);
         CPnt *newPnt = Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
      }
   }
}

#define  SMALL_DEVIATION    0.0001
/******************************************************************************
* ConstuctUnion
*/
CPolyList *ConstuctUnion(CPolyList *thinLinePartList)
{
   // build regions
   DML_LIST rgnList = dml_create_list();

   POSITION pos = thinLinePartList->GetHeadPosition();
   while (pos)
   {
      CPoly *poly = thinLinePartList->GetNext(pos);

      T2_REGION rgn = BB_CreateRegion(&(poly->getPntList()));

      dml_append_data(rgnList, (ANY) rgn);

      //delete poly;
   }

   // find union
   DML_LIST unionRgnList = t2b_union_l(rgnList, NULL);

   dml_destroy_list(rgnList, BB_free_region);
   
   CPolyList *resultList = new CPolyList;

   DML_ITEM rgnItem;
   DML_WALK_LIST(unionRgnList, rgnItem)
   {
      T2_REGION unionRgn = (T2_REGION) dml_record(rgnItem);

      DML_ITEM loopItem;
      DML_WALK_LIST(T2_REGION_LOOP_LIST(unionRgn), loopItem)
      {
         T2_LOOP loop = (T2_LOOP)DML_RECORD(loopItem);

         CPoly *poly = new CPoly;
         resultList->AddTail(poly);
         poly->setClosed(true);
         poly->setFilled(false);
         poly->setWidthIndex(thinWidthIndex);

         DML_ITEM edgeItem;
         DML_WALK_LIST(T2_LOOP_EDGE_LIST(loop), edgeItem)
         {
            T2_EDGE edge = (T2_EDGE)DML_RECORD(edgeItem);

            PT2 pt;
            t2c_ept0(edge, pt);

            CPnt *pnt = new CPnt;
            poly->getPntList().AddTail(pnt);
            pnt->x = (DbUnit)PT2_X(pt);
            pnt->y = (DbUnit)PT2_Y(pt);
            pnt->bulge = 0;

            CPnt *prevPnt = pnt;
            DML_WALK_LIST(T2_LOOP_EDGE_LIST(loop), edgeItem)
            {
               edge = (T2_EDGE)DML_RECORD(edgeItem);

               t2c_ept1(edge, pt);

               if (fabs(prevPnt->x - PT2_X(pt)) < SMALL_DEVIATION && fabs(prevPnt->y - PT2_Y(pt)) < SMALL_DEVIATION)
                  continue;

               CPnt *pnt = new CPnt;
               poly->getPntList().AddTail(pnt);
               pnt->x = (DbUnit)PT2_X(pt);
               pnt->y = (DbUnit)PT2_Y(pt);
               pnt->bulge = 0;

               C2_CURVE curve = T2_EDGE_CURVE(edge);
               if (C2_CURVE_IS_ARC(curve))
               {
                  REAL sweep;
                  c2c_get_arc_sweep(curve, &sweep);
                  int dir;
                  c2c_get_arc_direction(curve, &dir);
                  prevPnt->bulge = (DbUnit)(tan(sweep/4) * dir);
                  if (T2_EDGE_REVERSED(edge))
                     prevPnt->bulge *= -1;
               }
               
               prevPnt = pnt;
            }
         }
      }
   }

   // free unionRgnList
   dml_destroy_list(unionRgnList, BB_free_region);

   delete thinLinePartList;

   return resultList;
}
