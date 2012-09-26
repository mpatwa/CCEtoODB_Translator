// $Header: /CAMCAD/4.5/read_wrt/DmisOut.cpp 17    4/28/06 11:40a Kurt Van Ness $
 
/*****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1999. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_pen.h"
#include "ogp.h"
#include "attrib.h"
#include "graph.h"
#include "apertur2.h"
#include "polylib.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  WR_ROOT_ONLY   20
#define  WR_ALL_ENTITYS 30

#define  RoundEnd       TRUE
#define  SquareEnd      FALSE

static   CCEtoODBDoc     *doc;
static   double         unitsFactor;
static   int            display_error;
static   FILE           *flog;
static   long           featurecnt;
static   int            tolcnt;

static void DMIS_WriteData(FILE *wfp, CDataList *DataList, 
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale, int insertLayer);

//--------------------------------------------------------------
void DMIS_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int page_units)
{
   FILE     *wfp;

   // Set Static variables so don't have to pass them every time
   doc = Doc;
   featurecnt = 0;
   tolcnt = 0;
   display_error = FALSE;
    
   // open file for writing
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      ErrorMessage("Error open file", filename);
      return;
   }

   CString logFile = GetLogfilePath("DMIS.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fprintf(wfp,"DMISMN / 'DMIS_EDIP file'\n");

   switch (page_units)
   {
      case UNIT_INCHES:
         fprintf(wfp,"UNITS / INCH, ANGDEC\n");
         unitsFactor = 1;
      break;
      case UNIT_MM:
         fprintf(wfp,"UNITS / MM, ANGDEC\n");
         unitsFactor = 1;
      break;
      default:
         fprintf(wfp,"UNITS / INCH, ANGDEC\n");
         unitsFactor = Units_Factor(page_units, UNIT_INCHES);
      break;
   }
   fprintf(wfp,"\n");

/* get it from Devin.
$$ Camera definitions
$$
   S(zoom1) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 0.8, 22
   S(zoom2) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 1, 22
   S(zoom3) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 1.2, 22
   S(zoom4) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 1.4, 22
   S(zoom5) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 1.6, 22
   S(zoom6) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 1.8, 22
   S(zoom7) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 1.9, 22
   S(zoom8) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 2.2, 22
   S(zoom9) = SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 4.4,22
   S(zoom10)= SNSDEF/VIDEO,FIXED,VEC, 0, 0, -1, 0, 0, -1, 50, 5.2,22
$$
*/

   fprintf(wfp,"$$ default light definitions.\n");
   fprintf(wfp,"VL(profile) = LITDEF / BACK, 0, 0, 1\n"); 
   fprintf(wfp,"VL(surface) = LITDEF / BACK, 0, 0, -1\n"); 
   fprintf(wfp,"VL(ring) = LITDEF / BACK, 0, 0, -0.707\n"); 
   fprintf(wfp,"\n");

   // EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   FileStruct *file;
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);

      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file
      DMIS_WriteData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * unitsFactor,
          file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(),
         file->getScale() * unitsFactor, 0);
   }

   fprintf(wfp,"$$\n$$\nENDFIL\n");

   // close write file
   fclose(wfp);
   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

//--------------------------------------------------------------
static int get_tol(CCEtoODBDoc *doc, CAttributes* map, const char *att1, const char *att2, 
                  double *tollower, double *tolupper)
{
   int   tol = 0;
   Attrib   *a;

   if (a =  is_attvalue(doc, map, att2,   2))
   {
      CString val; 
      val = get_attvalue_string(doc, a);
      *tolupper = atof(val);
      tol |= 1;
   }
   if (a =  is_attvalue(doc, map, att1,   2))
   {
      CString val; 
      val = get_attvalue_string(doc, a);
      *tollower = atof(val);
      tol |= 2;
   }

   return tol;
}

//--------------------------------------------------------------
static int DMIS_write_poly(FILE *wfp,CPolyList *polylist, CAttributes* map, long *f,
                        double scale, int mirror, double rotation, double insert_x, double insert_y)
{
   double      tollower, tolupper;
   Mat2x2      m;
   CString     output_string;

   RotMat2(&m, rotation);

   CPoly *poly;
   CPnt  *pnt;
   POSITION polyPos, pntPos;
   long  featurecnt = *f;

   //if no poly lines exist, return
   if(!polylist || !polylist->GetCount())
      return 0;

   // need to fix this here correctly
   poly = polylist->GetHead();

   // loop thru polys without width
   polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = polylist->GetNext(polyPos);
      BOOL polyFilled = poly->isFilled();

      int first = TRUE;
      Point2   firstp;
      Point2   p1,p2;

      // here check for circle
      double cx, cy, radius;
      if (PolyIsCircle(poly, &cx, &cy, &radius))
      {
         p2.x = cx * scale;
         if (mirror) p2.x = -p2.x;
         p2.y = cy * scale;
         TransPoint2(&p2, 1, &m, insert_x, insert_y);

         fprintf(wfp,"$$ EDIP CIRCLE\n");
         fprintf(wfp,"F(CI%ld) = FEAT / CIRCLE, INNER, CART, %lg, %lg, 0.0, 0.0, 0.0, 1.0, %lg\n",
                     ++featurecnt, cx, cy, radius * 2 * scale);

         output_string.Format("OUTPUT / F(CI%ld)", featurecnt);

         if (get_tol(doc, map, ATT_Tol_fLower, ATT_Tol_fUpper, &tollower, &tolupper) == 3)   // both tolerances done
         {
            CString  tmp;
            fprintf(wfp,"T(%d) = TOL / DIAM, %lg, %lg\n", ++tolcnt, tollower, tolupper);
            tmp.Format(", T(%d)",tolcnt);
            output_string += tmp;
         }
         fprintf(wfp,"MEAS / CIRCLE, F(CI%ld), 3\n", featurecnt);
         fprintf(wfp,"ENDMES\n");

         if (get_tol(doc, map, ATT_Tol_xLower, ATT_Tol_xUpper, &tollower, &tolupper) == 3)   // both tolerances done
         {
            CString  tmp;
            fprintf(wfp,"T(%d) = TOL / CORTOL, XAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
            tmp.Format(", T(%d)",tolcnt);
            output_string += tmp;
         }

         if (get_tol(doc, map, ATT_Tol_yLower, ATT_Tol_yUpper, &tollower, &tolupper) == 3)   // both tolerances done
         {
            CString  tmp;
            fprintf(wfp,"T(%d) = TOL / CORTOL, YAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
            tmp.Format(", T(%d)",tolcnt);
            output_string += tmp;
         }

         if (get_tol(doc, map, ATT_Tol_zLower, ATT_Tol_zUpper, &tollower, &tolupper) == 3)   // both tolerances done
         {
            CString  tmp;
            fprintf(wfp,"T(%d) = TOL / CORTOL, ZAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
            tmp.Format(", T(%d)",tolcnt);
            output_string += tmp;
         }
         fprintf(wfp,"%s\n\n", output_string);
      }
      else
      {
         pntPos = poly->getPntList().GetHeadPosition();
         while (pntPos != NULL)
         {
            pnt = poly->getPntList().GetNext(pntPos);
            p2.x = pnt->x * scale;
            if (mirror) p2.x = -p2.x;
            p2.y = pnt->y * scale;
            p2.bulge = pnt->bulge;
            TransPoint2(&p2, 1, &m, insert_x, insert_y);
            // here deal with bulge
            if (first)
            {
               firstp = p2;
               p1 = p2;
               first = FALSE;
            }
            else
            {
               double da = atan(p1.bulge) * 4;
               if (fabs(da) > SMALLANGLE)
               {
                  double cx,cy,r,sa;
                  ArcPoint2Angle(p1.x,p1.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                  // make positive start angle.
                  if (sa < 0) sa += PI2;
                  fprintf(wfp,"$$ EDIP ARC\n");
                  fprintf(wfp,"F(AR%ld) = FEAT / ARC, INNER, CART, %lg, %lg, 0.0, 0.0, 0.0, 1.0, %lg, %lg, %lg\n",
                     ++featurecnt, cx, cy, r, RadToDeg(sa), RadToDeg(da));
                  output_string.Format("OUTPUT / F(AR%ld)", featurecnt);

                  if (get_tol(doc, map, ATT_Tol_fLower, ATT_Tol_fUpper, &tollower, &tolupper) == 3)   // both tolerances done
                  {
                     CString  tmp;
                     fprintf(wfp,"T(%d) = TOL / DIAM, %lg, %lg\n", ++tolcnt, tollower, tolupper);
                     tmp.Format(", T(%d)",tolcnt);
                     output_string += tmp;
                  }
                  // here meas MEAS / ARC, F(AR203), 6 (6 = number of measurement taken)
                  fprintf(wfp,"MEAS / ARC, F(AR%ld), 6\n", featurecnt);
                  fprintf(wfp,"ENDMES\n");
               }
               else
               {
                  if (p1.x == p2.x && p1.y == p2.y)   continue;   // eliminate  segments which are Zero;
                  fprintf(wfp,"$$ EDIP LINE\n");
                  fprintf(wfp,"F(LN%ld) = FEAT / LINE, BND, CART, %lg, %lg, 0.0, %lg, %lg, 0.0, 0.0, 0.0, 1.0\n",
                     ++featurecnt, p1.x, p1.y, p2.x, p2.y);
                  output_string.Format("OUTPUT / F(LN%ld)", featurecnt);

                  if (get_tol(doc, map, ATT_Tol_fLower, ATT_Tol_fUpper, &tollower, &tolupper) == 3)   // both tolerances done
                  {
                     CString  tmp;
                     fprintf(wfp,"T(%d) = TOL / ANG, %lg, %lg\n", ++tolcnt, tollower, tolupper);
                     tmp.Format(", T(%d)",tolcnt);
                     output_string += tmp;
                  }
                  fprintf(wfp,"MEAS / LINE, F(LN%ld), 2\n", featurecnt);
                  fprintf(wfp,"ENDMES\n");
               }
   /* just in case Devin is wrong.
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_Edge_Bounds,   2))
               {
                  fprintf(wfp,"BOUND / F(%ld)\n", featurecnt);
               }
*/
               if (get_tol(doc, map, ATT_Tol_xLower, ATT_Tol_xUpper, &tollower, &tolupper) == 3)   // both tolerances done
               {
                  CString  tmp;
                  fprintf(wfp,"T(%d) = TOL / CORTOL, XAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
                  tmp.Format(", T(%d)",tolcnt);
                  output_string += tmp;
               }

               if (get_tol(doc, map, ATT_Tol_yLower, ATT_Tol_yUpper, &tollower, &tolupper) == 3)   // both tolerances done
               {
                  CString  tmp;
                  fprintf(wfp,"T(%d) = TOL / CORTOL, YAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
                  tmp.Format(", T(%d)",tolcnt);
                  output_string += tmp;
               }

               if (get_tol(doc, map, ATT_Tol_zLower, ATT_Tol_zUpper, &tollower, &tolupper) == 3)   // both tolerances done
               {
                  CString  tmp;
                  fprintf(wfp,"T(%d) = TOL / CORTOL, ZAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
                  tmp.Format(", T(%d)",tolcnt);
                  output_string += tmp;
               }
               fprintf(wfp,"%s\n\n", output_string);
               p1 = p2;
            }
         }
      } // if circle
   }

   *f = featurecnt;
   return 1;
}

//--------------------------------------------------------------
static int DMIS_write_point(FILE *wfp,PointStruct *point, CAttributes* map, long *f,
                        double scale, int mirror, double rotation, double insert_x, double insert_y)
{
   double      tollower, tolupper;
   Mat2x2      m;

   RotMat2(&m, rotation);

   long  featurecnt = *f;

   Point2   p2;
   CString  output_string;

   p2.x = point->x * scale;
   if (mirror) p2.x = -p2.x;
   p2.y = point->y * scale;
   TransPoint2(&p2, 1, &m, insert_x, insert_y);

   fprintf(wfp,"$$ EDIP POINT\n");
   fprintf(wfp,"F(PN%ld) = FEAT / POINT, BND, CART, %lg, %lg, 0.0, 0.0, 0.0, 1.0\n",
         ++featurecnt, p2.x, p2.y);
   output_string.Format("OUTPUT / F(AR%ld)", featurecnt);

   if (get_tol(doc, map, ATT_Tol_fLower, ATT_Tol_fUpper, &tollower, &tolupper) == 3)   // both tolerances done
   {
      CString  tmp;
      fprintf(wfp,"T(%d) = TOL / ANG, %lg, %lg\n", ++tolcnt, tollower, tolupper);
      tmp.Format(", T(%d)",tolcnt);
      output_string += tmp;
   }
   fprintf(wfp,"MEAS / POINT, F(PN%ld), 2\n", featurecnt);
   fprintf(wfp,"ENDMES\n");

   if (get_tol(doc, map, ATT_Tol_xLower, ATT_Tol_xUpper, &tollower, &tolupper) == 3)   // both tolerances done
   {
      CString  tmp;
      fprintf(wfp,"T(%d) = TOL / CORTOL, XAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
      tmp.Format(", T(%d)",tolcnt);
      output_string += tmp;
   }

   if (get_tol(doc, map, ATT_Tol_yLower, ATT_Tol_yUpper, &tollower, &tolupper) == 3)   // both tolerances done
   {
      CString  tmp;
      fprintf(wfp,"T(%d) = TOL / CORTOL, YAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
      tmp.Format(", T(%d)",tolcnt);
      output_string += tmp;
   }

   if (get_tol(doc, map, ATT_Tol_zLower, ATT_Tol_zUpper, &tollower, &tolupper) == 3)   // both tolerances done
   {
      CString  tmp;
      fprintf(wfp,"T(%d) = TOL / CORTOL, ZAXIS, %lg, %lg\n", ++tolcnt, tollower, tolupper);
      tmp.Format(", T(%d)",tolcnt);
      output_string += tmp;
   }
   fprintf(wfp,"%s\n\n", output_string);

   *f = featurecnt;
   return 1;
}

//--------------------------------------------------------------
void DMIS_WriteData(FILE *wfp, CDataList *DataList, 
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int insertLayer)
{
   Mat2x2      m;
   Point2      p1;
   DataStruct  *np;
   Attrib      *a;
   int         tol = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
            continue;

         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_Tol_ReportText,   2))
         {
            //CString val; 
            //val = get_attvalue_string(doc, a);
            //fprintf(fp, "%sPART: \"%s\";\n", ident, val);
            fprintf(wfp,"DISPLY / PRINT\n");
         }

         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_Tol_FeatureText,  2))
         {
            //CString val; 
            //val = get_attvalue_string(doc, a);
            //fprintf(fp, "%sPART: \"%s\";\n", ident, val);
            fprintf(wfp,"DISPLY / TERM\n");
         }

         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_Light_Profile, 2))
         {
            CString val; 
            val = get_attvalue_string(doc, a);
            double l = atof(val) / 255;
            fprintf(wfp,"SNSET / VL(profile), %lg\n", l);
         }
         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_Light_Surface, 2))
         {
            CString val; 
            val = get_attvalue_string(doc, a);
            double l = atof(val) / 255;
            fprintf(wfp,"SNSET / VL(surface), %lg\n", l);

         }
         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_Light_Ring, 2))
         {
            CString val; 
            val = get_attvalue_string(doc, a);
            double l = atof(val) / 255;
            fprintf(wfp,"SNSET / VL(ring), %lg\n", l);
         }
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            // polylist and attribute
            DMIS_write_poly(wfp,np->getPolyList(),np->getAttributesRef(), &featurecnt, scale, mirror, rotation, insert_x, insert_y);
         }
         break;  // POLYSTRUCT

         case T_POINT:
         {
            DMIS_write_point(wfp,np->getPoint(),np->getAttributesRef(), &featurecnt, scale, mirror, rotation, insert_x, insert_y);
         }
         break;  // POINT
  
         case T_TEXT:
         {
         }
         break;

         case T_INSERT:
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            p1.x = np->getInsert()->getOriginX() * scale;
            if (mirror) p1.x = -p1.x;
            p1.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&p1, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot;

            if (mirror)
               block_rot = rotation - np->getInsert()->getAngle();
            else
               block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if ( doc->get_layer_visible(layer, mirror) )
               {
                  //fprintf(wfp,"$$ Aperture [%d] found -> ignored\n", block->getShape());
                  
                  if (block->getShape() == T_COMPLEX)
                  {
                     BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                     block = subblock;
                  }

                  CPolyList *p = ApertureToPoly_Base(block, 0.0, 0.0, 0.0, 0);   // uses block->rot

                  DMIS_write_poly(wfp,p,np->getAttributesRef(), &featurecnt, scale, block_mirror, block_rot, p1.x, p1.y);
                  FreePolyList(p);//this function in Polylib.cpp
               }
            }
            else
            {
               DMIS_WriteData(wfp, &(block->getDataList()),
                            p1.x, p1.y,
                            block_rot, 
                            block_mirror,
                            scale * np->getInsert()->getScale(),
                            np->getLayerIndex());
            } // end else not aperture
         break;
      } // end switch
   } // end for
   return;
} /* end DMIS_WriteData */

// end DMISOUT.CPP

