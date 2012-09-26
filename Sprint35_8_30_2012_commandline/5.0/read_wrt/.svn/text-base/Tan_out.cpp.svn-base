// $Header: /CAMCAD/4.5/read_wrt/Tan_out.cpp 17    4/28/06 6:53p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1997. All Rights Reserved.

  Arc read errors may happen because TANGO need the center of the arc inside 
  32", but it may be a bulge, which is ok.
*/
                              
#include "stdafx.h"
#include "ccdoc.h"
#include "tanlib.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "graph.h"
#include <math.h>

static CCEtoODBDoc *doc;
typedef CArray<Point2, Point2&> CPolyArray;

static double unitsFactor;
static   CPolyArray  polyarray;
static   int         polycnt;

static void Tango_WriteData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

//--------------------------------------------------------------
void TANGO_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format,
                     double UnitsFactor)
{
   FILE *wfp;
   FileStruct *file;
   doc = Doc;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      return;
   }

   polyarray.SetSize(100,100);
   polycnt = 0;

   wtan_Graph_Init_Layout();

   wtan_Graph_File_Open_Layout(wfp);

   // Set Static variables so don't have to pass them every time
   unitsFactor = UnitsFactor;

   // EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file

      Tango_WriteData(&(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
   }
   // Does end of COMPONENT
   wtan_Graph_File_Close_Layout();

   polyarray.RemoveAll();

   // close write file
   fclose(wfp);

   return;
}


//--------------------------------------------------------------
void Tango_WriteData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2, p2;
   DataStruct *np;
   int layer;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         wtan_Graph_Level(Get_Layer_Layername(doc->get_layer_mirror(layer, mirror)));
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale;
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               int first = TRUE;

               if (!polyFilled)
               {
                  // just normal lines and arcs
                  Point2   p;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     if (fabs(p.bulge) > BULGE_THRESHOLD)
                     {
                        double cx,cy,r,sa;
                        Point2 p2;
                        double da = atan(p.bulge) * 4;
                        CPnt *pnt1;                                                 
      
                        // GetNext advance to next, GetAt does not.
                        if (pntPos == NULL)
                           ErrorMessage("Poly Bulge data error !");
                        else
                           pnt1 = poly->getPntList().GetAt(pntPos);
                     
                        p2.x = pnt1->x * scale;
                        if (mirror) p2.x = -p2.x;
                        p2.y = pnt1->y * scale;
                        p2.bulge = pnt1->bulge;
                        TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  
                        ArcPoint2Angle(p.x,p.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                        // make positive start angle.
                        if (sa < 0) sa += PI2;
                        wtan_Graph_Arc(cx, cy,r, sa, da, lineWidth);
                     }
                     else
                     {
                        if (!first)
                           wtan_Graph_Line( p.x, p.y, p2.x, p2.y,lineWidth );
                        p2 = p;
                     }
                     first = FALSE;
                  }
               }
               else
               {
                  Point2   p;
                  polyarray.RemoveAll();
                  polycnt = 0;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     if (fabs(p.bulge) > BULGE_THRESHOLD)
                     {
                        int     i, ppolycnt;
                        Point2  ppoly[255];
                        double cx,cy,r,sa;
                        Point2 p2;
                        double da = atan(p.bulge) * 4;
                        CPnt *pnt1;                                                 
      
                        // GetNext advance to next, GetAt does not.
                        if (pntPos == NULL)
                           ErrorMessage("Poly Bulge data error !");
                        else
                           pnt1 = poly->getPntList().GetAt(pntPos);
                     
                        p2.x = pnt1->x * scale;
                        if (mirror) p2.x = -p2.x;
                        p2.y = pnt1->y * scale;
                        p2.bulge = pnt1->bulge;
                        TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  
                        ArcPoint2Angle(p.x,p.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                        // make positive start angle.
                        if (sa < 0) sa += PI2;
      
                        ppolycnt = 255;
                        // start center
                        ArcPoly2(p.x,p.y,cx,cy,da,
                           1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree
                        for (i=0;i<ppolycnt;i++)
                        {
                           polyarray.SetAtGrow(polycnt,ppoly[i]);
                           polycnt++;
                        }
                     }
                     else
                     {
                        polyarray.SetAtGrow(polycnt,p);
                        polycnt++;
                     }
                  }

                  // here test for small number
                  int done = FALSE,i;
                  int pcnt = polycnt;
                  while (!done)
                  {
                     done = TRUE;
                     for (i=1;i<polycnt;i++)
                     {
                        Point2 p1 = polyarray.ElementAt(i-1);
                        Point2 p2 = polyarray.ElementAt(i);

                        // 1 mil is the smallest unit in tango
                        if (fabs(p1.x - p2.x) < 1 &&  
                            fabs(p1.y - p2.y) < 1)
                        {
                           polyarray.RemoveAt(i);
                           done = FALSE;
                           polycnt--;
                           break;
                        }
                     }
                  }
               
                  Point2 *points;
                  if ((points = (Point2 *)calloc(polycnt, sizeof(Point2))) == NULL) 
                     MemErrorMessage(__FILE__, __LINE__);
            
                  for (i=0;i<polycnt;i++)
                  {
                     Point2 p2 = polyarray.ElementAt(i);
                     points[i].x = p2.x;
                     points[i].y = p2.y;
                     points[i].bulge = 0;
                  }

                  if (polycnt > 1)
                  {
                     if (polycnt <= 2)
                     {
                        Point2 p1,p2;
                        p1.x = points[0].x;
                        p1.y = points[0].y;
                        p2.x = points[1].x;
                        p2.y = points[1].y;
                        wtan_Graph_Line( p1.x, p1.y, p2.x, p2.y,lineWidth );
                     }
                     else
                        wtan_Graph_Polyline(points, polycnt,polyFilled);
                  }
                  free(points);
               } // if color
            } // if filled
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            if (!doc->get_layer_visible(np->getLayerIndex(), mirror)) continue;

            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wtan_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror) ) // only write data item if it goes in this file
               {
                     wtan_Graph_Level(Get_Layer_Layername(doc->get_layer_mirror(layer, mirror)));
                     wtan_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * unitsFactor,
                        block->getSizeB() * unitsFactor,
                        block->getShape(), np->getInsert()->getAngle());
               }
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               Tango_WriteData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for
   return;
 } /* end Tango_WriteData */

/****************************************************************************/
/*
   end TAN_OUT.CPP
*/
/****************************************************************************/

