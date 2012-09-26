// $Header: /CAMCAD/5.0/read_wrt/OGP_out.cpp 33    6/17/07 8:59p Kurt Van Ness $
                      
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.
*/           

/* TODO
  file rotation
  defaults
  edge & tol
  new code and h files
  arcs, polys, circles
*/                                        

#include "stdafx.h"
#include "ccdoc.h"
#include "ogp\include\db_const.h"
#include "ogp\include\db_decl.h"
#include "ogp\include\db_proto.h"
#include "attrib.h"
#include "ogp.h"
#include "polylib.h"
#include <math.h>
#include <float.h>
#include "apertur2.h"
#include "extents.h"
#include "RwUiLib.h"

const int Color = 14;

BOOL FindPointOnLine(double p1x, double p1y, double p2x, double p2y, double distance, double *nx, double *ny);

static double inchUnitFactor, nativeUnitFactor;
static double FOV;

static CCEtoODBDoc *doc;

static void FillFeature(DataStruct *data, FileStruct *file, Feature *feat);
static void PutLine(DataStruct *data, Feature *feat, double x1, double y1, double x2, double y2);
static void PutCircle(DataStruct *data, Feature *feat, double cx, double cy, double radius, BOOL PositiveDelta);
static void PutArc(DataStruct *data, Feature *feat, double cx, double cy, double radius, double sa, double da);
static void PutLineFeature(DataStruct *data, Feature *feat, double x1, double y1, double x2, double y2);
static void PutCircleFeature(DataStruct *data, Feature *feat, double cx, double cy, double radius, BOOL PositiveDelta);
static void PutArcFeature(DataStruct *data, Feature *feat, double cx, double cy, double radius, double sa, double da);
static void PutPointFeature(DataStruct *data, Feature *feat, double x, double y);
static void PutCentroidFeature(DataStruct *data, Feature *feat);
static void PutCommon(DataStruct *data, Feature *feat, BOOL IsPoint, BOOL IsCentroid);

/*****************************************************************************
* OGP_WriteFiles
*
*     Main Routine
*/
void OGP_WriteFiles(const char *filename, CCEtoODBDoc *Doc)
{
   doc = Doc;

   inchUnitFactor = Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES);

   if (doc->getSettings().getPageUnits() == UNIT_MM)
      nativeUnitFactor = 1.0;
   else
      nativeUnitFactor = Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES);

   DeleteFile(GetLogfilePath("OGP.log"));

   // init database
   if (dbInitDataBase() != NO_ERROR)
   {
      ErrorMessage("Can't Initialize Database");
      return;
   }

   // allocate feature
   Feature *feat;

   if ((feat = dbNewFeature()) == NULL)
   {
      ErrorMessage("Can't get new feature");
      return;
   }

   // loop files
   BOOL WarnInserts = FALSE;
   BOOL WarnNoFOV = FALSE;
   
   for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      // loop datas (no hierarchy)      
      for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)   
         {
            BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (!(subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE || subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL))
            {
               if (!WarnInserts)
               {
                  WarnInserts = TRUE;

                  if (ErrorMessage("Do you want to continue?", "There are Inserts", MB_YESNO) == IDNO)
                  {
                     dbDisposeFeature(feat);
                     return;
                  }
               }

               continue;
            }
         }

         // write this entity
         Attrib* attrib;
         int keyword;
         keyword = doc->IsKeyWord(ATT_FOV_Inches, FALSE);

         if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
         {
            if (doc->getSettings().getPageUnits() == UNIT_MM)
               FOV = attrib->getDoubleValue() * Units_Factor(UNIT_INCHES, UNIT_MM); 
            else
               FOV = attrib->getDoubleValue(); 
         }
         else
         {
            if (!WarnNoFOV)
            {
               WarnNoFOV = TRUE;

               if (ErrorMessage("Do you want to continue?", "Skipping Entities that don't have FOV parameters set.", MB_YESNO) == IDNO)
               {
                  dbDisposeFeature(feat);
                  return;
               }
            }

            continue;
         }

         FillFeature(data, file, feat);
      }
   }

   // free feature mem
   dbDisposeFeature(feat);

   // save to file
   int i;
   char *fname = STRDUP(filename);

   if (i = dbSaveDataBase(fname))
   {
      char buf[20];
      ErrorMessage("Error in save", _itoa(i, buf, 10));
   }

   free(fname);

   // ErrFile
   FILE *temp = fopen(GetLogfilePath("OGP.log"), "r");

   if (temp)
   {
      fclose(temp);
      Logreader(GetLogfilePath("OGP.log"));
   }
}

void DoPoly(DataStruct *data, Feature *feat, CPolyList *polylist, double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m);
/*****************************************************************************
* FillFeature
*
*     Fill feature for this data entity
*/
void FillFeature(DataStruct *data, FileStruct *file, Feature *feat)
{
   Mat2x2 m;
   RotMat2(&m, file->getRotation());

   switch (data->getDataType())
   {
   case T_POINT:
      PutPointFeature(data, feat, data->getPoint()->x, data->getPoint()->y);
      break;

   case T_POLY:
      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_Centroid_IsCentroid, 0), attrib))
         {
            if (attrib->getIntValue())
            {
               PutCentroidFeature(data, feat);
               break;
            }            
         }
      }

      DoPoly(data, feat, data->getPolyList(), file->getScale(), file->isMirrored(), file->getInsertX(), file->getInsertY(), &m);
      break;

   case T_INSERT:
      {
         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
         {
            Point2 point2;      
            point2.x = data->getInsert()->getOriginX() * file->getScale();
            point2.y = data->getInsert()->getOriginY() * file->getScale();

            if (file->isMirrored()) point2.x = -point2.x;

            TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

            CPolyList *polylist = ApertureToPoly(block, data->getInsert(), file->isMirrored());

            if (polylist)
            {
               DoPoly(data, feat, polylist, file->getScale(), file->isMirrored(), file->getInsertX(), file->getInsertY(), &m);
               FreePolyList(polylist);
            }
         }
      }
   }
}

/******************************************************************************
* DoPoly
*/
void DoPoly(DataStruct *data, Feature *feat, CPolyList *polylist, double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m)
{   
   for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
   {
      CPoly *poly = polylist->GetNext(polyPos);

      // circle
      double cx, cy, radius;

      if (PolyIsCircle(poly, &cx, &cy, &radius))
      {
         PutCircle(data, feat, cx, cy, radius, poly->getPntList().GetHead()->bulge > 0);
         continue;
      }
   
      CPnt *p1, *p2;
      POSITION pntPos = poly->getPntList().GetHeadPosition();
      p2 = poly->getPntList().GetNext(pntPos);

      while (pntPos != NULL)
      {
         p1 = p2;
         p2 = poly->getPntList().GetNext(pntPos);

         if (fabs(p1->bulge) > SMALLNUMBER)
         {
            double sa, da, cx, cy, r;
            da = atan(p1->bulge) * 4;
            ArcPoint2Angle(p1->x, p1->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);
            PutArc(data, feat, cx, cy, r, sa, da);
         }
         else
         {
            PutLine(data, feat, p1->x, p1->y, p2->x, p2->y);
         }
      }
   }
}

/*****************************************************************************
* PutLine
*
*     Export a LINE entity
*/
void PutLine(DataStruct *data, Feature *feat, double x1, double y1, double x2, double y2)
{
   double percentFeature = 1.0; 
   int percentCoverage = 0;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;

      keyword = doc->IsKeyWord(ATT_Edge_PercentFeature, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         percentFeature = 0.01 * attrib->getIntValue(); 
      }

      keyword = doc->IsKeyWord(ATT_Edge_PercentCoverage, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         percentCoverage = attrib->getIntValue(); 
      }
   }

   if (percentFeature < SMALLNUMBER)
      percentFeature = 1.0;

   double distance =  sqrt(pow(x2-x1,2) + pow(y2-y1,2));
   double adjustedDistance = distance * percentFeature;
   double ax, ay, bx, by;

   // shorten to Percent of Full Feature
   if (percentFeature >= 1)
   {
      ax = x1;  ay = y1;
      bx = x2;  by = y2;
   }
   else
   {
      // (1 - percentFeature) / 2 
      // example: 90% -> 5% off each end
      FindPointOnLine(x1, y1, x2, y2, (1 - percentFeature) * distance / 2, &ax, &ay);
      FindPointOnLine(x2, y2, x1, y1, (1 - percentFeature) * distance / 2, &bx, &by);
   }

   // how much of LINE fits in FOV
   double ratio = FOV / adjustedDistance; 

   if (ratio >= 0.99) // 1 piece
   {
      PutLineFeature(data, feat, ax, ay, bx, by);
   }
   else if (ratio >= 0.5) // break line in half
   {
      double cx, cy;

      FindPointOnLine(ax, ay, bx, by, adjustedDistance / 2, &cx, &cy); 
      PutLineFeature(data, feat, ax, ay, cx, cy);
      PutLineFeature(data, feat, cx, cy, bx, by);
   }
   else if (ratio >= 0.33) // break line into 3 
   {
      double cx, cy, dx, dy;

      FindPointOnLine(ax, ay, bx, by, adjustedDistance / 3, &cx, &cy); 
      FindPointOnLine(ax, ay, bx, by, adjustedDistance / 3 * 2, &dx, &dy); 
      PutLineFeature(data, feat, ax, ay, cx, cy);
      PutLineFeature(data, feat, cx, cy, dx, dy);
      PutLineFeature(data, feat, dx, dy, bx, by);
   }
   else // break based on % coverage 
   {
      if (percentCoverage > 99) // break into equal pieces for complete coverage
      {
         int pieces = (int)ceil(adjustedDistance / FOV);
         double size = adjustedDistance / pieces;

         double cx, cy, dx, dy;
         cx = ax;   cy = ay;

         for (int i=1; i<=pieces; i++)
         {
            FindPointOnLine(cx, cy, bx, by, size, &dx, &dy); 
            PutLineFeature(data, feat, cx, cy, dx, dy);
            cx = dx;
            cy = dy;
         }
      }
      else // break (incomplete coverage)
      {
         double cx, cy, dx, dy;
         double drawDistance = adjustedDistance * 0.01 * percentCoverage;
         int pieces = (int)ceil(drawDistance / FOV);

         if (pieces < 4) // minimum 3 pieces (end, center, other end)
         {
            FindPointOnLine(ax, ay, bx, by, FOV, &cx, &cy); 
            PutLineFeature(data, feat, ax, ay, cx, cy);
            FindPointOnLine(ax, ay, bx, by, (adjustedDistance - FOV) /2, &cx, &cy); 
            FindPointOnLine(cx, cy, bx, by, FOV, &dx, &dy); 
            PutLineFeature(data, feat, cx, cy, dx, dy);
            FindPointOnLine(bx, by, ax, ay, FOV, &cx, &cy); 
            PutLineFeature(data, feat, cx, cy, bx, by);
         }
         else
         {
            double size = drawDistance / pieces;
            double space = (adjustedDistance - drawDistance) / (pieces - 1);

            cx = ax;   cy = ay;

            for (int i=1; i<=pieces; i++)
            {
               FindPointOnLine(cx, cy, bx, by, size, &dx, &dy); 
               PutLineFeature(data, feat, cx, cy, dx, dy);
               FindPointOnLine(dx, dy, bx, by, space, &cx, &cy); 
            }
         }
      }
   }
}

/*****************************************************************************
* PutCircle
*
*     Export a CIRCLE entity
*/
void PutCircle(DataStruct *data, Feature *feat, double cx, double cy, double radius, BOOL PositiveDelta)
{
   // if whole CIRCLE fits in FOV
   if (radius * 2 < FOV)
   {
      PutCircleFeature(data, feat, cx, cy, radius, PositiveDelta);
   }
   else
   {
      int percentCoverage = 0;

      if (data->getAttributesRef())
      {
         Attrib* attrib;
         int keyword;

         keyword = doc->IsKeyWord(ATT_Edge_PercentCoverage, FALSE);

         if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
         {
            percentCoverage = attrib->getIntValue(); 
         }
      }

      if (1.414213562373 * radius < FOV) // length of 90° chord = sqrt( r^2 + r^2) = sqrt(2) * r
      {
         double da = PI / 2;  

         if (!PositiveDelta)
            da = -da;

         PutArcFeature(data, feat, cx, cy, radius, 0, da);              // 0°
         PutArcFeature(data, feat, cx, cy, radius, PI / 2, da);         // 90°
         PutArcFeature(data, feat, cx, cy, radius, PI, da);             // 180°
         PutArcFeature(data, feat, cx, cy, radius, 3.0 / 2 * PI, da);   // 270°
      }
      else // smaller than 90° arcs needed
      {
         double delta = 2.0 * asin(FOV / 2 / radius); // calculate size of delta that will fit in FOV

         if (percentCoverage > 99) // break into equal pieces for complete coverage
         {
            int pieces = (int)ceil(2.0 * PI / delta);
            double eqaulDeltas = 2.0 * PI / pieces;
            double tempSA = 0.0;

            if (!PositiveDelta)
               eqaulDeltas = -eqaulDeltas;

            for (int i=1; i<=pieces; i++)
            {
               PutArcFeature(data, feat, cx, cy, radius, tempSA, eqaulDeltas);
               tempSA += eqaulDeltas;
            }
         }
         else // break (incomplete coverage)
         {
            double drawDelta = 2.0 * PI * 0.01 * percentCoverage; // circumfrence * percentCoverage
            int pieces = (int)ceil(fabs(drawDelta) / delta);

            if (pieces < 4) // minimum 3 pieces 
            {
               if (PositiveDelta)
               {
                  PutArcFeature(data, feat, cx, cy, radius, 0.0, delta);
                  PutArcFeature(data, feat, cx, cy, radius, PI * 2 / 3, delta);
                  PutArcFeature(data, feat, cx, cy, radius, PI * 4 / 3, delta);
               }
               else
               {
                  PutArcFeature(data, feat, cx, cy, radius, PI * 4 / 3, -delta);
                  PutArcFeature(data, feat, cx, cy, radius, PI * 2 / 3, -delta);
                  PutArcFeature(data, feat, cx, cy, radius, 0.0, -delta);
               }
            }
            else
            {
               double spaceDelta = (2.0 * PI - delta * pieces) / pieces;
               double tempSA = 0.0;

               if (!PositiveDelta)
               {
                  delta = -delta;
                  spaceDelta = -spaceDelta;
               }

               for (int i=1; i<=pieces; i++)
               {
                  PutArcFeature(data, feat, cx, cy, radius, tempSA, delta);
                  tempSA += delta + spaceDelta;
               }
            }
         }
      }
   }
}

/*****************************************************************************
* PutArc
*
*     Export a ARC entity
*/
void PutArc(DataStruct *data, Feature *feat, double cx, double cy, double radius, double sa, double da)
{
   double percentFeature = 1.0;
   int percentCoverage = 0;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;
      keyword = doc->IsKeyWord(ATT_Edge_PercentFeature, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         percentFeature = 0.01 * attrib->getIntValue(); 
      }

      keyword = doc->IsKeyWord(ATT_Edge_PercentCoverage, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         percentCoverage = attrib->getIntValue(); 
      }
   }

   if (percentFeature < SMALLNUMBER)
      percentFeature = 1.0;

   double new_sa, new_da;

   if (percentFeature >= 1)
   {
      new_sa = sa;
      new_da = da;
   }
   else
   {
      // offset startAngle
      // (1 - percentFeature) / 2 
      // example: 90% -> 5% off each end
      new_sa = sa + (1 - percentFeature) / 2 * da;
      new_da = da * percentFeature;
   }

   // if circle fits in FOV, then arc fits
   if (radius * 2 < FOV)
   {
      PutArcFeature(data, feat, cx, cy, radius, new_sa, new_da);
   }
   else 
   {
      double delta = 2.0 * asin(FOV / 2 / radius); // calculate size of delta that will fit in FOV (delta is positive)

      if (delta > 90) delta = 90;

      // if new_da < the max delta for this radius and FOV, then fits without breaking
      if (fabs(new_da) < delta)
      {
         PutArcFeature(data, feat, cx, cy, radius, new_sa, new_da);
      }
      else // break into smaller arcs
      {
         // how much of ARC fits in FOV
         double ratio = delta / fabs(new_da); // (ratio is positive)

         if (ratio >= 0.99) // 1 piece
         {
            PutArcFeature(data, feat, cx, cy, radius, new_sa, new_da);
         }
         else if (ratio >= 0.5) // break arc in 2 pieces
         {
            PutArcFeature(data, feat, cx, cy, radius, new_sa, new_da/2);
            PutArcFeature(data, feat, cx, cy, radius, new_sa + new_da/2, new_da/2);
         }
         else if (ratio >= 0.33) // break line into 3 
         {
            PutArcFeature(data, feat, cx, cy, radius, new_sa, new_da/3);
            PutArcFeature(data, feat, cx, cy, radius, new_sa + new_da/3, new_da/3);
            PutArcFeature(data, feat, cx, cy, radius, new_sa + new_da/3*2, new_da/3);
         }
         else // break based on % coverage 
         {
            if (percentCoverage > 99) // break into equal pieces for complete coverage
            {
               int pieces = (int)ceil(fabs(new_da) / delta);
               double eqaulDeltas = new_da / pieces;
               double tempSA = new_sa;

               for (int i=1; i<=pieces; i++)
               {
                  PutArcFeature(data, feat, cx, cy, radius, tempSA, eqaulDeltas);
                  tempSA += eqaulDeltas;
               }
            }
            else // break (incomplete coverage)
            {
               double drawDA = new_da * 0.01 * percentCoverage;
               int pieces = (int)ceil(fabs(drawDA) / delta);

               if (new_da < 0)
                  delta = -delta;

               if (pieces < 4) // minimum 3 pieces (end, center, other end)
               {
                  PutArcFeature(data, feat, cx, cy, radius, new_sa, delta);
                  PutArcFeature(data, feat, cx, cy, radius, new_sa + new_da /2 - delta /2, delta);
                  PutArcFeature(data, feat, cx, cy, radius, new_sa + new_da - delta, delta);
               }
               else
               {
                  double spaceDelta = (new_da - delta * pieces) / (pieces -1);
                  double tempSA = new_sa;

                  for (int i=1; i<=pieces; i++)
                  {
                     PutArcFeature(data, feat, cx, cy, radius, tempSA, delta);
                     tempSA += delta + spaceDelta;
                  }
               }
            }
         }
      }
   }
}

/*****************************************************************************
* PutLineFeature
*
*     Add one LINE to database
*/
void PutLineFeature(DataStruct *data, Feature *feat, double x1, double y1, double x2, double y2)
{
   double z = 0.0;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;
      keyword = doc->IsKeyWord(ATT_Tol_zLoc, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         z = attrib->getDoubleValue(); 
      }
   }

   // clear feature
   dbInitFeature(feat);
   PutCommon(data, feat, FALSE, FALSE);

   dbPutFeatureType(feat, LINE); // set feature type to line
   dbPutSmearType(feat, 2);  // set the weak edge type to line (2 for line, 3 for arc, 4 for full circle)

   int pointnum;
   // tell the db to add a point to the feature's point list
   pointnum = dbIncNumberPoints(feat); 
   dbPutPointTargetColor(feat, pointnum, Color);
   
   // store the first point's nominal location
   dbPutPointX(feat, pointnum, x1 * inchUnitFactor);
   dbPutPointY(feat, pointnum, y1 * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);

   // store the first point's actual location
   dbPutActPointX(feat, pointnum, x1 * inchUnitFactor);
   dbPutActPointY(feat, pointnum, y1 * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);

   // store the weak edge start point location
   dbPutSmearStartPtX(feat, x1 * inchUnitFactor);
   dbPutSmearStartPtY(feat, y1 * inchUnitFactor);

   // add another point to the point list (now we have 2)
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);
   dbPutPointX(feat, pointnum, x2 * inchUnitFactor);
   dbPutPointY(feat, pointnum, y2 * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   dbPutActPointX(feat, pointnum, x2 * inchUnitFactor);
   dbPutActPointY(feat, pointnum, y2 * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);
   dbPutSmearEndPtX(feat, x2 * inchUnitFactor);
   dbPutSmearEndPtY(feat, y2 * inchUnitFactor);

   // midpoint of line    
   double midpt_x = (x1 + x2) / 2.0;
   double midpt_y = (y1 + y2) / 2.0;

   // store the midpoint of the line as its actual location
   dbPutActualLocX(feat, midpt_x * inchUnitFactor);
   dbPutActualLocY(feat, midpt_y * inchUnitFactor);
   dbPutActualLocZ(feat, z * inchUnitFactor);

   // and as its nominal location
   dbPutNominalLocX(feat, midpt_x * nativeUnitFactor);
   dbPutNominalLocY(feat, midpt_y * nativeUnitFactor);
   dbPutNominalLocZ(feat, z * nativeUnitFactor);

   // center of FOV (use midpoint)
   // set the weak edge center point to the midpoint so the line is centered in the center of the FOV
   dbPutSmearCenterX(feat, midpt_x * inchUnitFactor);
   dbPutSmearCenterY(feat, midpt_y * inchUnitFactor);
   dbPutSmearCenterZ(feat, z * inchUnitFactor);

   double angle = ArcTan2(y2-y1, x2-x1) * 180 / PI;  // calculate the angle of the line from the start to the end point
   // store it as both the nominal and actual
   dbPutNominalSize(feat, angle); 
   dbPutActualSize(feat, angle); 

   // and now for the direction of the weak edge
   dbPutSmearDirection(feat, 0); 
   
   int err;

   if ((err = dbPutFeatureInDB(feat)) != NO_ERROR)
   {
      ErrorMessage("Can't put feature in database");
   }
}

/*****************************************************************************
* PutCircleFeature
*
*     Add one CIRCLE to database
*/
void PutCircleFeature(DataStruct *data, Feature *feat, double cx, double cy, double radius, BOOL PositiveDelta)
{
   double z = 0.0;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;
      keyword = doc->IsKeyWord(ATT_Tol_zLoc, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         z = attrib->getDoubleValue(); 
      }
   }

   // clear feature
   dbInitFeature(feat);
   PutCommon(data, feat, FALSE, FALSE);

   dbPutFeatureType(feat, CIRCLE); // set feature type to circle
   dbPutSmearType(feat, 4); // set the weak edge type to circle (2 for line, 3 for arc, 4 for full circle)
   dbPutLastDisplay(feat, DIAMETER);

   int pointnum;
   // tell the db to add a point to the feature's point list
   pointnum = dbIncNumberPoints(feat); 
   dbPutPointTargetColor(feat, pointnum, Color);
   
   // store the first point's nominal location
   dbPutPointX(feat, pointnum, (cx + radius) * inchUnitFactor);
   dbPutPointY(feat, pointnum, cy * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   // store the first point's actual location
   dbPutActPointX(feat, pointnum, (cx + radius) * inchUnitFactor);
   dbPutActPointY(feat, pointnum, cy * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);
   // store the weak edge first point location
   dbPutSmearStartPtX(feat, (cx + radius) * inchUnitFactor);
   dbPutSmearStartPtY(feat, cy * inchUnitFactor);

   // add another point to the point list (now we have 2)
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);
   // store the second point's nominal location
   dbPutPointX(feat, pointnum, cx * inchUnitFactor);
   dbPutPointY(feat, pointnum, (PositiveDelta ? cy + radius : cy - radius) * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   // store the second point's actual location
   dbPutActPointX(feat, pointnum, cx * inchUnitFactor);
   dbPutActPointY(feat, pointnum, (PositiveDelta ? cy + radius : cy - radius) * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);

   // add another point to the point list (now we have 3)
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);
   // store the third point's nominal location
   dbPutPointX(feat, pointnum, (cx - radius) * inchUnitFactor);
   dbPutPointY(feat, pointnum, cy * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   // store the third point's actual location
   dbPutActPointX(feat, pointnum, (cx - radius) * inchUnitFactor);
   dbPutActPointY(feat, pointnum, cy * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);
   // store the weak edge third point location
   dbPutSmearEndPtX(feat, (cx - radius) * inchUnitFactor);
   dbPutSmearEndPtY(feat, cy * inchUnitFactor);

   // store the midpoint of the line as its actual location
   dbPutActualLocX(feat, cx * inchUnitFactor);
   dbPutActualLocY(feat, cy * inchUnitFactor);
   dbPutActualLocZ(feat, z * inchUnitFactor);

   // and as its nominal location
   dbPutNominalLocX(feat, cx * nativeUnitFactor);
   dbPutNominalLocY(feat, cy * nativeUnitFactor);
   dbPutNominalLocZ(feat, z * nativeUnitFactor);

   // center of FOV (use midpoint)
   // set the weak edge center point to the midpoint so the line is centered in the center of the FOV
   dbPutSmearCenterX(feat, cx * inchUnitFactor);
   dbPutSmearCenterY(feat, cy * inchUnitFactor);
   dbPutSmearCenterZ(feat, z * inchUnitFactor);

   // for arcs and circles we need to store the midpoint of the circle which is the center of the center of the circle.
   dbPutSmearMidPtX(feat, cx * inchUnitFactor);
   dbPutSmearMidPtY(feat, cy * inchUnitFactor);

   dbPutNominalSize(feat, radius * 2.0 * nativeUnitFactor); // diameter
   dbPutActualSize(feat, radius * inchUnitFactor); // radius

   // and now for the direction of the weak edge
   dbPutSmearDirection(feat, (PositiveDelta?-1:1));
   
   int err;

   if ((err = dbPutFeatureInDB(feat)) != NO_ERROR)
   {
      ErrorMessage("Can't put feature in database");
   }
}

/*****************************************************************************
* PutArcFeature
*
*     Add one ARC to database
*/
void PutArcFeature(DataStruct *data, Feature *feat, double cx, double cy, double radius, double sa, double da)
{
   double z = 0.0;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;
      keyword = doc->IsKeyWord(ATT_Tol_zLoc, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         z = attrib->getDoubleValue(); 
      }
   }

   // clear feature
   dbInitFeature(feat);
   PutCommon(data, feat, FALSE, FALSE);

   dbPutFeatureType(feat, CIRCLE); // set feature type to circle
   dbPutSmearType(feat, 3); // set the weak edge type to line (2 for line, 3 for arc, 4 for full circle)
   dbPutLastDisplay(feat, RADIUS);

   int pointnum;
   // tell the db to add a point to the feature's point list
   pointnum = dbIncNumberPoints(feat); 
   dbPutPointTargetColor(feat, pointnum, Color);
   
   double cosine, sine;

   cosine = cos(sa) * radius;
   sine = sin(sa) * radius;
   // store the first point's nominal location
   dbPutPointX(feat, pointnum, (cx + cosine) * inchUnitFactor);
   dbPutPointY(feat, pointnum, (cy + sine) * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   // store the first point's actual location
   dbPutActPointX(feat, pointnum, (cx + cosine) * inchUnitFactor);
   dbPutActPointY(feat, pointnum, (cy + sine) * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);
   // store the weak edge first point location
   dbPutSmearStartPtX(feat, (cx + cosine) * inchUnitFactor);
   dbPutSmearStartPtY(feat, (cy + sine) * inchUnitFactor);

   cosine = cos(sa + da/2) * radius;
   sine = sin(sa + da/2) * radius;
   // add another point to the point list (now we have 2)
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);
   // store the second point's nominal location
   dbPutPointX(feat, pointnum, (cx + cosine) * inchUnitFactor);
   dbPutPointY(feat, pointnum, (cy + sine) * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   // store the second point's actual location
   dbPutActPointX(feat, pointnum, (cx + cosine) * inchUnitFactor);
   dbPutActPointY(feat, pointnum, (cy + sine) * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);

   cosine = cos(sa + da) * radius;
   sine = sin(sa + da) * radius;
   // add another point to the point list (now we have 3)
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);
   // store the third point's nominal location
   dbPutPointX(feat, pointnum, (cx + cosine) * inchUnitFactor);
   dbPutPointY(feat, pointnum, (cy + sine) * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);
   // store the third point's actual location
   dbPutActPointX(feat, pointnum, (cx + cosine) * inchUnitFactor);
   dbPutActPointY(feat, pointnum, (cy + sine) * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);
   // store the weak edge third point location
   dbPutSmearEndPtX(feat, (cx + cosine) * inchUnitFactor);
   dbPutSmearEndPtY(feat, (cy + sine) * inchUnitFactor);

   // store the midpoint of the line as its actual location
   dbPutActualLocX(feat, cx * inchUnitFactor);
   dbPutActualLocY(feat, cy * inchUnitFactor);
   dbPutActualLocZ(feat, z * inchUnitFactor);

   // and as its nominal location
   dbPutNominalLocX(feat, cx * nativeUnitFactor);
   dbPutNominalLocY(feat, cy * nativeUnitFactor);
   dbPutNominalLocZ(feat, z * nativeUnitFactor);

   // center of FOV (use midpoint)
   // set the weak edge center point to the midpoint so the line is centered in the center of the FOV
   if (fabs(da) < PI + SMALLNUMBER) // use middle of chord for FOV center
   {
      double ax, ay, bx, by;
      ax = cx + cos(sa) * radius;
      ay = cy + sin(sa) * radius;
      bx = cx + cos(sa + da) * radius;
      by = cy + sin(sa + da) * radius;
      dbPutSmearCenterX(feat, (ax + bx) / 2.0 * inchUnitFactor);
      dbPutSmearCenterY(feat, (ay + by) / 2.0 * inchUnitFactor);
   }
   else // if more than half a circle, use circle's center as FOV center
   {
      dbPutSmearCenterX(feat, cx * inchUnitFactor);
      dbPutSmearCenterY(feat, cy * inchUnitFactor);
   }

   dbPutSmearCenterZ(feat, z * inchUnitFactor);

   // for arcs and circles we need to store the midpoint of the circle which is the center of the circle.
   dbPutSmearMidPtX(feat, cx * inchUnitFactor);
   dbPutSmearMidPtY(feat, cy * inchUnitFactor);

   dbPutNominalSize(feat, radius * nativeUnitFactor); 
   dbPutActualSize(feat, radius * inchUnitFactor); 

   // and now for the direction of the weak edge
   dbPutSmearDirection(feat, (da>0?-1:1)); 
   
   int err;

   if ((err = dbPutFeatureInDB(feat)) != NO_ERROR)
   {
      ErrorMessage("Can't put feature in database");
   }
}

/*****************************************************************************
* PutPointFeature
*
*     Add one POINT to database
*/
void PutPointFeature(DataStruct *data, Feature *feat, double x, double y)
{
   double z = 0.0;
   int bounds = 32, extents = 32;
   int type = 9; // 9 = strong edge, 11 = focus
   int edgeSeek = SEEK_EDGE_MAX_CONTRAST;
   int backLight = 255;
   int ringLight = 255;
   int auxLight = 255;
   int zoomCounts = 1000;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;

      keyword = doc->IsKeyWord(ATT_Tol_zLoc, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         z = attrib->getDoubleValue();
      }

      keyword = doc->IsKeyWord(ATT_PointType, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         CString value = attrib->getStringValue();

         if (value == PT_STRONG_EDGE)
         {
            type = 9;
            edgeSeek = SEEK_EDGE_MAX_CONTRAST;
         }
         else if (value == PT_STRONG_EDGE_RIGHT)
         {
            type = 9;
            edgeSeek = SEEK_EDGE_RIGHT;
         }
         else if (value == PT_STRONG_EDGE_LEFT)
         {
            type = 9;
            edgeSeek = SEEK_EDGE_LEFT;
         }
         else if (value == PT_STRONG_EDGE_UP)
         {
            type = 9;
            edgeSeek = SEEK_EDGE_UP;
         }
         else if (value == PT_STRONG_EDGE_DOWN)
         {
            type = 9;
            edgeSeek = SEEK_EDGE_DOWN;
         }
         else if (value == PT_STRONG_EDGE_LOCATE)
         {
            type = 9;
            edgeSeek = SEEK_LOCATE;
         }
         else if (value == PT_FOCUS_TOP_DOWN)
         {
            type = 11;
            edgeSeek = TOP_DOWN_FOCUS;
         }
         else if (value == PT_FOCUS_BOTTOM_UP)
         {
            type = 11;
            edgeSeek = BOTTOM_UP_FOCUS;
         }
         else if (value == PT_FOCUS_M_C_FOCUS)
         {
            type = 11;
            edgeSeek = MAX_CONTRAST_FOCUS;
         }
         else if (value == PT_FOCUS_M_C_GRID)
         {
            type = 11;
            edgeSeek =MAX_CONTRAST_GRID;
         }
      }

      keyword = doc->IsKeyWord(ATT_Edge_Bounds, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         bounds = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Edge_Extents, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         extents = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Light_Profile, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         backLight = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Light_Surface, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         auxLight = attrib->getIntValue();
      }
      
      keyword = doc->IsKeyWord(ATT_Light_Ring, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         ringLight = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_FOV_ZoomCounts, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         zoomCounts =  attrib->getIntValue();
      }
   }

   // clear feature
   dbInitFeature(feat);
   PutCommon(data, feat, TRUE, FALSE);

   dbPutFeatureType(feat, POINT); // set feature type to point

   int pointnum;
   // tell the db to add a point to the feature's point list
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);

   // x and y pixel size of focus box or search box
   dbPutPointTargetSpacing(feat, pointnum, bounds, extents);   

   // point type
   dbPutPointTargetType(feat, pointnum, type);
   dbPutPointEdgeSeek(feat, pointnum, edgeSeek);

   // store the first point's nominal location
   dbPutPointX(feat, pointnum, x * inchUnitFactor);
   dbPutPointY(feat, pointnum, y * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);

   // store the first point's actual location
   dbPutActPointX(feat, pointnum, x * inchUnitFactor);
   dbPutActPointY(feat, pointnum, y * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);

   // store the midpoint of the edge as its actual location
   dbPutActualLocX(feat, x * inchUnitFactor);
   dbPutActualLocY(feat, y * inchUnitFactor);
   dbPutActualLocZ(feat, z * inchUnitFactor);

   // and as its nominal location
   dbPutNominalLocX(feat, x * nativeUnitFactor);
   dbPutNominalLocY(feat, y * nativeUnitFactor);
   dbPutNominalLocZ(feat, z * nativeUnitFactor);

   // zoom
   dbPutPointZoom(feat, pointnum, zoomCounts);

   // lights
   dbPutPointBackLight(feat, pointnum, backLight);
   dbPutPointRingLight(feat, pointnum, ringLight);
   dbPutPointAuxLight(feat, pointnum, auxLight);

/* case PT_WEAK:
      dbPutSmearType(feat, 1);  // set the weak edge type to point (1 for average point, 5 for maximum point, 6 for minimum point)

      // store the weak edge start point location
      dbPutSmearStartPtX(feat, x * inchUnitFactor);
      dbPutSmearStartPtY(feat, y * inchUnitFactor);

      // add another point to the point list (now we have 2)
      dbPutSmearEndPtX(feat, x * inchUnitFactor);
      dbPutSmearEndPtY(feat, y * inchUnitFactor);

      // center of FOV (use midpoint)
      // set the weak edge center point to the midpoint so the edge is
      // centered in the center of the FOV
      dbPutSmearCenterX(feat, x * inchUnitFactor);
      dbPutSmearCenterY(feat, y * inchUnitFactor);
      dbPutSmearCenterZ(feat, z * inchUnitFactor);

      // and now for the direction of the weak edge
      dbPutSmearDirection(feat, 0);
*/

   int err;

   if ((err = dbPutFeatureInDB(feat)) != NO_ERROR)
   {
      ErrorMessage("Can't put feature in database");
   }
}

/*****************************************************************************
* PutCentroidFeature
*
*     Add one CENTROID to database
*/
void PutCentroidFeature(DataStruct *data, Feature *feat)
{
   double z = 0.0;
   int backLight = 255;
   int ringLight = 255;
   int auxLight = 255;
   int threshold = 128;
   int filter = 0;
   int multiple = 0;
   int illumination = -1;
   unsigned int flags = 0;
   int zoomCounts = 1000;
   double fovInches = 0;

   if (data->getAttributesRef())
   {
      Attrib* attrib;
      int keyword;

      keyword = doc->IsKeyWord(ATT_Tol_zLoc, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         z = attrib->getDoubleValue();
      }

      keyword = doc->IsKeyWord(ATT_Light_Profile, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         backLight = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Light_Surface, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         auxLight = attrib->getIntValue();
      }
      
      keyword = doc->IsKeyWord(ATT_Light_Ring, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         ringLight = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_FOV_ZoomCounts, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         zoomCounts = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_FOV_Inches, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         fovInches = attrib->getDoubleValue();
      }

      keyword = doc->IsKeyWord(ATT_Centroid_Threshold, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         threshold = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Centroid_Filter, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         filter = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Centroid_MultipleValue, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         multiple = attrib->getIntValue();
      }

      keyword = doc->IsKeyWord(ATT_Centroid_MultipleOn, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getIntValue())
            flags |= 1<<0;
         else
            multiple = 0;
      }

      keyword = doc->IsKeyWord(ATT_Centroid_Fill, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getIntValue())
            flags |= 1<<2;
      }

      keyword = doc->IsKeyWord(ATT_Centroid_TouchBoundary, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getIntValue())
            flags |= 1<<3;
      }

      keyword = doc->IsKeyWord(ATT_Centroid_Illumination, FALSE);

      if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         illumination = attrib->getIntValue();
      }
   }

   ExtentRect extentRect;
   extentRect.left  = extentRect.bottom =  FLT_MAX;
   extentRect.right = extentRect.top    = -FLT_MAX;

   Mat2x2 m;
   RotMat2(&m, 0);
   
   for (POSITION polyPos = data->getPolyList()->GetHeadPosition();polyPos != NULL;)
   {
      CPoly *poly = data->getPolyList()->GetNext(polyPos);

      ExtentRect polyExtents;
      polyExtents.left = polyExtents.bottom = FLT_MAX;
      polyExtents.right = polyExtents.top = -FLT_MAX;
      PolyPntExtents(&poly->getPntList(), &polyExtents, 1.0, 0, 0, 0, &m, 0/*poly->getWidthIndex()...*/);

      extentRect.left = min(extentRect.left, polyExtents.left);
      extentRect.right = max(extentRect.right, polyExtents.right);
      extentRect.bottom = min(extentRect.bottom, polyExtents.bottom);
      extentRect.top = max(extentRect.top, polyExtents.top);
   }

   double x = (extentRect.right + extentRect.left) / 2;
   double y = (extentRect.top + extentRect.bottom) / 2;

   double width = extentRect.right - extentRect.left;
   double height = extentRect.top - extentRect.bottom;

   if (width * inchUnitFactor > fovInches || height * inchUnitFactor > fovInches)
   {
      ErrorMessage("Centroid Feature does not fit in FOV");

      return;
   }

   // clear feature
   dbInitFeature(feat);

   // FOV Zoom Counts
   dbPutSmearZoom(feat, fovInches);

   PutCommon(data, feat, FALSE, TRUE);

   dbPutFeatureType(feat, CENTROID); // set feature type to CENTROID

   int pointnum;
   // tell the db to add a point to the feature's point list
   pointnum = dbIncNumberPoints(feat);
   dbPutPointTargetColor(feat, pointnum, Color);

   // store the first point's nominal location
   dbPutPointX(feat, pointnum, x * inchUnitFactor);
   dbPutPointY(feat, pointnum, y * inchUnitFactor);
   dbPutPointZ(feat, pointnum, z * inchUnitFactor);

   // store the first point's actual location
   dbPutActPointX(feat, pointnum, x * inchUnitFactor);
   dbPutActPointY(feat, pointnum, y * inchUnitFactor);
   dbPutActPointZ(feat, pointnum, z * inchUnitFactor);

   // store the midpoint of the edge as its actual location
   dbPutActualLocX(feat, x * inchUnitFactor);
   dbPutActualLocY(feat, y * inchUnitFactor);
   dbPutActualLocZ(feat, z * inchUnitFactor);

   // and as its nominal location
   dbPutNominalLocX(feat, x * nativeUnitFactor);
   dbPutNominalLocY(feat, y * nativeUnitFactor);
   dbPutNominalLocZ(feat, z * nativeUnitFactor);

   // zoom
   dbPutPointZoom(feat, pointnum, zoomCounts);

   // lights
   dbPutPointBackLight(feat, pointnum, backLight);
   dbPutPointRingLight(feat, pointnum, ringLight);
   dbPutPointAuxLight(feat, pointnum, auxLight);

   dbPutSmearType(feat, 8);  // set the weak edge type to point (8 for blob)

   // store the weak edge start point location
   dbPutSmearStartPtX(feat, x * inchUnitFactor);
   dbPutSmearStartPtY(feat, y * inchUnitFactor);
   dbPutSmearStartPtZ(feat, z * inchUnitFactor);

   // add another point to the point list (now we have 2)
   dbPutSmearEndPtX(feat, x * inchUnitFactor);
   dbPutSmearEndPtY(feat, y * inchUnitFactor);
   dbPutSmearEndPtZ(feat, z * inchUnitFactor);

   dbPutSmearMidPtX(feat, x * inchUnitFactor);
   dbPutSmearMidPtY(feat, y * inchUnitFactor);
   dbPutSmearMidPtZ(feat, z * inchUnitFactor);

   // center of FOV (use midpoint)
   // set the weak edge center point to the midpoint so the edge is centered in the center of the FOV
   dbPutSmearCenterX(feat, x * inchUnitFactor);
   dbPutSmearCenterY(feat, y * inchUnitFactor);
   dbPutSmearCenterZ(feat, z * inchUnitFactor);

   // and now for the direction of the weak edge
   dbPutSmearDirection(feat, 3);

   int widthPixels  = (int)ceil(width  / 2 * inchUnitFactor / fovInches * 480);
   int heightPixels = (int)ceil(height / 2 * inchUnitFactor / fovInches * 480);

   dbPutSmearMinQuality(feat, threshold);
   dbPutSmearExtent(feat, widthPixels);
   dbPutSmearBounds(feat, heightPixels);
   dbPutPointTargetSpacing(feat, pointnum, (int)(1.2 * widthPixels), (int)(1.2 * heightPixels));
   dbPutSmearContrast(feat, illumination); // -1=auto, 0=dark, +1=light
   dbPutSmearWgtFirst(feat, 0); // reserved
   
   dbPutSmearWgtSecond(feat, flags); // bit flags, bit 0=multiple on/off, bit 1=reserved, bit 2=fill on/off, bit 3=touch boundary on/off
   dbPutSmearWgtNominal(feat, multiple);
   dbPutSmearStdDev(feat, filter);

   int err;

   if ((err = dbPutFeatureInDB(feat)) != NO_ERROR)
   {
      ErrorMessage("Can't put feature in database");
   }
}

/*****************************************************************************
* PutCommon
*
*     Fill in enitity info common to all data types
*/
void PutCommon(DataStruct *data, Feature *feat, BOOL IsPoint, BOOL IsCentroid)
{
   // feature created and step number set
   dbNextFeatureNumber(feat);
   dbPutFeatureMode(feat, MEASURE);
   dbPutFeaturePlane(feat, XY_PLANE);

   if (doc->getSettings().getPageUnits() == UNIT_MM)
      dbPutInMmFlag(feat, MM); 
   else
      dbPutInMmFlag(feat, INCH); 

   dbPutCartPolarFlag(feat, CART);

   if (!data->getAttributesRef())
      return;

   Attrib* attrib;
   int keyword;

   /***** FOV *****/
   // FOV Zoom Counts
   keyword = doc->IsKeyWord(ATT_FOV_ZoomCounts, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearZoom(feat, attrib->getIntValue());
   }

   /***** TEXT *****/
   // save the report text / comment
   keyword = doc->IsKeyWord(ATT_Tol_ReportText, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getValueType() == VT_STRING)
      {
         CString temp= attrib->getStringValue();
         dbPutFeatureComment(feat, temp.GetBuffer(0)); 
      }
   }

   // save the user prompt
   keyword = doc->IsKeyWord(ATT_Tol_FeatureText, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getValueType() == VT_STRING)
      {
         CString temp= attrib->getStringValue();
         dbPutFeaturePrompt(feat, temp.GetBuffer(0)); 
      }
   }

   /***** TOLERANCES *****/
   // we also need to store the upper and lower tolerances for the line's nominal location x and y
   keyword = doc->IsKeyWord(ATT_Tol_fUpper, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutUpperSizeTol(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_fLower, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutLowerSizeTol(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_xUpper, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutUpperLocTolX(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_xLower, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutLowerLocTolX(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_yUpper, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutUpperLocTolY(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_yLower, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutLowerLocTolY(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_zUpper, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutUpperLocTolZ(feat, attrib->getDoubleValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Tol_zLower, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutLowerLocTolZ(feat, attrib->getDoubleValue()); 
   }

   int flag;
   /***** PRINT *****/
   flag = 0;
   keyword = doc->IsKeyWord(ATT_Tol_fPrint, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_FEAT;
   }

   keyword = doc->IsKeyWord(ATT_Tol_xPrint, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_X;
   }

   keyword = doc->IsKeyWord(ATT_Tol_yPrint, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_Y;
   }

   keyword = doc->IsKeyWord(ATT_Tol_zPrint, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_Z;
   }

   dbPutPrintFlags(feat, flag);

   /***** STATS *****/
   flag = 0;
   keyword = doc->IsKeyWord(ATT_Tol_fStats, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_FEAT;
   }

   keyword = doc->IsKeyWord(ATT_Tol_xStats, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_X;
   }

   keyword = doc->IsKeyWord(ATT_Tol_yStats, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_Y;
   }

   keyword = doc->IsKeyWord(ATT_Tol_zStats, FALSE);

   if (keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      if (attrib->getIntValue())
         flag |= FLAG_Z;
   }

   dbPutStatisticsFlags(feat, flag);    

   // store the light settings for the weak edge
   keyword = doc->IsKeyWord(ATT_Light_Profile, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearBackLight(feat, attrib->getIntValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Light_Surface, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearAuxLight(feat, attrib->getIntValue()); 
   }

   keyword = doc->IsKeyWord(ATT_Light_Ring, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearRingLight(feat, attrib->getIntValue()); 
   }

   /***** EDGES *****/
   // set the weak edge bounds from the slider
   keyword = doc->IsKeyWord(ATT_Edge_Bounds, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearBounds(feat, attrib->getIntValue()); 
   }

   // set the weak edge extent (how wide the patch is)
   keyword = doc->IsKeyWord(ATT_Edge_Extents, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearExtent(feat, attrib->getIntValue()); 
   }

   // set minimum quality == rough/smooth slider
   keyword = doc->IsKeyWord(ATT_Edge_WeakStrong, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearMinQuality(feat, attrib->getIntValue()); 
   }

   // how much to weight the nominal values
   keyword = doc->IsKeyWord(ATT_Edge_NearestNominal, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearWgtNominal(feat, attrib->getIntValue()); 
   }

   // " "   contrast
   keyword = doc->IsKeyWord(ATT_Edge_Contrast, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearWgtContrast(feat, attrib->getIntValue()); 
   }

   // " "   first edge found
   keyword = doc->IsKeyWord(ATT_Edge_First, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearWgtFirst(feat, attrib->getIntValue()); 
   }

   // " "   last edge found
   keyword = doc->IsKeyWord(ATT_Edge_Last, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearWgtLast(feat, attrib->getIntValue()); 
   }

   // " "   first of 2 close points found
   keyword = doc->IsKeyWord(ATT_Edge_FirstOf2, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearWgtSecond(feat, attrib->getIntValue()); 
   }

   // " "   second of 2 close points found
   keyword = doc->IsKeyWord(ATT_Edge_LastOf2, FALSE);

   if (!IsPoint && !IsCentroid && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearWgt2Last(feat, attrib->getIntValue()); 
   }

   // set standard deviation ==
   keyword = doc->IsKeyWord(ATT_Edge_RoughSmooth, FALSE);

   if (!IsPoint && keyword != -1 && data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
   {
      dbPutSmearStdDev(feat, attrib->getIntValue()); 
   }

   dbPutNominalsEntered(feat, 1); // tell MeasureMind we have put nominals in the feature
}

// end OGP.CPP