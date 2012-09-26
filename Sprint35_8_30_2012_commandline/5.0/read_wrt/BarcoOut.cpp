// $Header: /CAMCAD/4.6/read_wrt/BarcoOut.cpp 22    12/07/06 12:13p Kurt Van Ness $

/****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "Barcolib.h"
#include "font.h"
#include "graph.h"
#include <math.h>
#include "lyr_file.h"
#include "gauge.h"
#include "polylib.h"
#include "CCEtoODB.h"

#define  WR_ROOT_ONLY      20
#define  WR_ALL_ENTITYS    30

extern CProgressDlg *progress;
extern CFileLayerList *FileLayerList;
extern FontStruct *FontStructure;

static CCEtoODBDoc *doc;
static FileLayerStruct *FileLayerPtr;

static void Barco_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                         int mirror,
                    double scale,
                    int embeddedLevel, int insertlayer, double unitScale, int apdef);

static void Barco_WriteAppData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                         int mirror,
                    double scale);

//--------------------------------------------------------------
void Barco_WriteFiles(CCEtoODBDoc *Doc, FormatStruct *format)
{
   char  prosa[255];
   int res; // result
   POSITION pos;
   FileStruct *file;
   double   scale = format->Scale;

   doc = Doc;
   //loop FileList
   POSITION flpos = FileLayerList->GetHeadPosition();
   while (flpos != NULL)
   {      
      FileLayerPtr = FileLayerList->GetNext(flpos);

      progress->SetStatus(FileLayerPtr->file);

      if ((res = wbarco_Graph_File_Open(FileLayerPtr->file)) != 1)
      {
         char  tmp[80+_MAX_PATH];
         // error occured
         // -1 = file can not open
         // 0  = memory failed
         // 1 = all ok
         if(res == -1)
         {
            // file error
            sprintf(tmp,"Can not open file [%s]", FileLayerPtr->file);
            ErrorMessage(tmp);
            return;
         }
         if (res == 0)
            MemErrorMessage(__FILE__, __LINE__);
      }

      sprintf(prosa,"; Disk Plot File definition format\n");
      wbarco_write2file(prosa);

      // here date and time
      char *months[] = { "January", "February", "March", "April", "May", "June", "July", 
         "August", "September", "October", "November", "December" };
      char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

      //CString camcad;
      //camcad.LoadString(IDR_MAINFRAME);
      CTime time = CTime::GetCurrentTime();
      int hour = time.GetHour();

      sprintf(prosa,"; Created by %s on %s, %s %d, %d at %d:%02d%s\n", getApp().getCamCadTitle(), 
         days[time.GetDayOfWeek()-1], months[time.GetMonth()-1], 
         time.GetDay(), time.GetYear(), (hour>12 ? hour-12 : hour), time.GetMinute(),
         (hour>11 ? "pm" : "am"));
      wbarco_write2file(prosa);
      wbarco_write2file(";units\n");
      if (format->GR.units == 0)
         sprintf(prosa,"U=INCH\n");
      else
         sprintf(prosa,"U=MM\n");
      wbarco_write2file(prosa);

      // here i need the page extents
      // doc->Settings.Xmin, Ymin, Xmax Ymax
      wbarco_write2file(";job minima and maxima\n");
      sprintf(prosa,"X%1.4lf,%1.4lf Y%1.4lf,%1.4lf\n",
         doc->getSettings().getXmin()*scale, doc->getSettings().getXmax()*scale,
         doc->getSettings().getYmin()*scale, doc->getSettings().getYmax()*scale);
      wbarco_write2file(prosa);

      wbarco_write2file("; Aperture Definition\n");

      // first do AM - this are all non round or square app
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);
         if (block == NULL)   continue;

         double sizeA = block->getSizeA()*scale;
         double sizeB = block->getSizeB()*scale;
         // create necessary aperture blocks
         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) ||
              (block->getFlags()& BL_WIDTH))
         {
            switch (block->getShape())
            {
               case T_COMPLEX:
               {
                  int   bnum = block->getBlockNumber();
                  sprintf(prosa,"; Complex Aperture Definition %s\n",block->getName());
                  wbarco_write2file(prosa);
                  block = doc->Find_Block_by_Num((int)(block->getSizeA())); // here is get the subblock
                  if (!block) continue;
                  if (block->getDataList().IsEmpty()) continue;
                  sprintf(prosa,"A%d=BLOCK,(\n",bnum);
                  wbarco_write2file(prosa);
                  Barco_WriteData(&(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, scale, TRUE);
                  sprintf(prosa,")\n",bnum);
                  wbarco_write2file(prosa);
               }
               break;
               case  T_RECTANGLE:
                  sprintf(prosa,"A%d=RECTANGLE,%1.4lf,%1.4lf\n",
                     block->getBlockNumber(),sizeA,sizeB);
                  wbarco_write2file(prosa);
               break;               
               case  T_SQUARE:
                  sprintf(prosa,"A%d=SQUARE,%1.4lf\n",
                     block->getBlockNumber(),sizeA);
                  wbarco_write2file(prosa);
               break;
               case T_TARGET:
                  sprintf(prosa,"A%d=CIRCLE,%1.4lf\n",block->getBlockNumber(),sizeA);
                  //sprintf(prosa,"A%d=TARGET ???\n", block->num);
                  wbarco_write2file(prosa);
               break;
               case T_THERMAL:
               {
                  double   gapsize = sizeA - sizeB;
                  double   angle = 45;
                  int      num_spokes = 4;

                  if (block->getSizeC()*scale > 0)
                     gapsize = block->getSizeC()*scale;
                  if (block->getSizeD() > 0)
                     angle = RadToDeg(block->getSizeD());
                  if (block->getSpokeCount() > 0)
                     num_spokes = block->getSpokeCount();

                  sprintf(prosa,"A%d=THERMAL,%1.4lf,%1.4lf,%1.4lf,%d,%1.1lf,RR\n",block->getBlockNumber(),
                     sizeA, sizeB, gapsize, num_spokes, angle);
                  wbarco_write2file(prosa);
               }
               break;
               case T_DONUT:
                  sprintf(prosa,"A%d=DONUT,%1.4lf,%1.4lf,RR\n",block->getBlockNumber(),sizeA,sizeB);
                  wbarco_write2file(prosa);
               break;
               case T_OCTAGON:
                  sprintf(prosa,"A%d=OCTAGON,%1.4lf\n",
                     block->getBlockNumber(),sizeA);
                  wbarco_write2file(prosa);
               break;
               case T_OBLONG:
               {
                  double r;
                  if (sizeA < sizeB)
                     r = sizeA/2;
                  else
                     r = sizeB/2;
                  sprintf(prosa,"A%d=BOX,%1.4lf,%1.4lf,R=%1.4lf\n", block->getBlockNumber(), sizeA, sizeB, r);
                  wbarco_write2file(prosa);
               }
               break;
               case T_ROUND:
                  sprintf(prosa,"A%d=CIRCLE,%1.4lf\n",block->getBlockNumber(),sizeA);
                  wbarco_write2file(prosa);
               break;                 
               default:
                  prosa[0] = '\0';
               break;
            }
         }
      }

      pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);

         if (!file->isShown())  continue;
         if (file->getBlock() == NULL)  continue;   // empty file
         Barco_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * format->Scale,
               file->getInsertY() * format->Scale,
               file->getRotation(), file->isMirrored(), file->getScale() * format->Scale, 0, -1, format->Scale, FALSE);
      }
      wbarco_Graph_File_Close();
   }
   return;
}

//--------------------------------------------------------------
void Barco_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, 
                    double unitScale, int apdef) // unitScale only used for apertures because they are not scaled
{
   Mat2x2   m;
   Point2   point2;
   int layer;
   long  entcnt = 0;
   POSITION pos;
   DataStruct *np;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      entcnt++;
      // only draw visible layer (Inserts do not have layers)
      if (np->getDataType() != T_INSERT)
      {
         if (np->getLayerIndex() == -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!apdef) // in complex aperture definition, do not check for layers
         {
            if (!doc->get_layer_visible(layer, mirror))
               continue;
            layer = doc->get_layer_mirror(layer, mirror);
            if (!LayerInFile(FileLayerPtr, layer)) // only write data item if it goes in this file
               continue;
         }
      }

      // inserts have no initial layers
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
               if (poly->isHidden()) continue;

               int dcode = doc->getWidthTable()[poly->getWidthIndex()]->getBlockNumber();
               int apshape = doc->getWidthTable()[poly->getWidthIndex()]->getShape();

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2    firstp;
               Point2   p;

               // make sure it has more than 1 vertex, because otherwise Autocad chockes.
               int cnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
               if (cnt < 2)   break;

               double cx, cy, radius;
               if (polyFilled && PolyIsCircle(poly, &cx, &cy, &radius))
               {
                  double w = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
                  radius += w/2; // scale is multiplied
                  p.x = cx * scale;
                  if (mirror) p.x = -p.x;
                  p.y = cy * scale;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  CString  prosa;
                  // a contour can not be reversed
                  if (np->isNegative())
                     prosa.Format("A9999=C,%1.4lf,REVERSE F%1.4lf,%1.4lf\n", radius*2*scale, p.x, p.y);
                  else
                     prosa.Format("A9999=C,%1.4lf F%1.4lf,%1.4lf\n", radius*2*scale, p.x, p.y);
                  wbarco_write2file(prosa);
               }
               else
               {
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (first)
                     {
                        wbarco_Graph_Polyline(p, closed, polyFilled, dcode, apshape);
                        first = FALSE;
                        firstp = p;
                     }
                     else
                        wbarco_Graph_Vertex(p);
                  }
                  // force close
                  if (polyFilled && (firstp.x != p.x || firstp.y != p.y))
                        wbarco_Graph_Vertex(firstp);
                  // a contour can not be reversed
                  //if (np->isNegative())
                  // wbarco_write2file(",REVERSE\n");
               }
            }  // while
         }
         break;

         case T_TEXT:
         {
            Point2 point;
            double height = np->getText()->getHeight() * scale;

            point.x = np->getText()->getPnt().x * scale;
            if (mirror) point.x = -point.x;
            point.y = np->getText()->getPnt().y * scale;
            TransPoint2(&point, 1, &m, insert_x, insert_y);

            double text_rot = rotation + np->getText()->getRotation();
            // text is mirrored if mirror is set or text.mirror but not if none or both
            int text_mirror = np->getText()->getResultantMirror(mirror);

            // here write text
            wbarco_Graph_Text( np->getText()->getText(), point.x, point.y,
                              np->getText()->getHeight(), np->getText()->getWidth(), text_rot, 
                              text_mirror, 0);

         }
         break;
         
         case T_INSERT:
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

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

               int newlayer = doc->get_layer_mirror(layer, mirror);

               if ( doc->get_layer_visible(layer, mirror) &&
                  LayerInFile(FileLayerPtr, newlayer) ) // only write data item if it goes in this file
               wbarco_Flash_App(point2, block->getBlockNumber());
            }
            else
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

               Barco_WriteData(&(block->getDataList()),
                     point2.x,
                     point2.y,
                     block_rot,
                     block_mirror,
                     scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, unitScale, apdef);
            }
         break;
      } // end switch
   } // end for
   return;
} // end Barco_WriteData

// end BARCOOUT.CPP

