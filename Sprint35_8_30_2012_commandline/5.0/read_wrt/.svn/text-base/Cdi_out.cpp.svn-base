// $Header: /CAMCAD/4.5/read_wrt/Cdi_out.cpp 19    4/30/06 12:22p Kurt Van Ness $

/******************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1998. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include <math.h>
#include "format_s.h"
#include "string.h"
#include "graph.h"
#include "cdilib.h"
#include "lyr_lyr.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define MAX_TEXTHEIGHT    100

typedef struct
{
   double h;
   double w;      // not used yet
   int    code;
}Textheight;

static   CCEtoODBDoc        *doc;
static   FormatStruct      *format;

static   int               display_error;
static   FILE              *flog;

static Textheight          *textheight;
static int                 textheightcnt;

static void    CDI_gettext(CDataList *DataList, double scale, BOOL mirror);
static int     store_textheight(double h);
static int     optimize_textheight(double step);
static void    sort_textheight();

static void    CDI_WriteData(CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel, int insertlayer);

//--------------------------------------------------------------
void CDI_WriteFiles(const char *Filename, CCEtoODBDoc *Doc, FormatStruct *Format,
                    double xmin, double xmax, double ymin, double ymax)
{
   FILE     *wfp;
   char     *lp;
   int      i;
   char     filename[_MAX_PATH];

   strcpy(filename, Filename); 
   FileStruct *file;
   POSITION pos;

   doc = Doc;
   format = Format;

   CString logFile = GetLogfilePath("cdi.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   display_error = 0;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }

   wCDI_Graph_Init();
   lp = strtok(filename,".");
   wCDI_Graph_File_Open(wfp, lp);

   // here write all width
   fprintf(wfp,".ASS\n");

   for (i=0;i<doc->getNextWidthIndex();i++)
   {
      fprintf(wfp,"TRA %d - %02.3lf\n",format->WidthCodes[i],format->Scale * doc->getWidthTable()[i]->getSizeA());
   }
   for (i=0;i<doc->getNextWidthIndex();i++)
   {
      fprintf(wfp,"NKW %d - %02.3lf\n",format->WidthCodes[i],format->Scale * doc->getWidthTable()[i]->getSizeA());
   }

   for (i=0;i<doc->getNextWidthIndex();i++)
   {
      fprintf(wfp,"OUT %d %02.3lf\n",format->WidthCodes[i],format->Scale * doc->getWidthTable()[i]->getSizeA());
   }
   for (i=0;i<doc->getNextWidthIndex();i++)
   {
      fprintf(wfp,"FIG %d %02.3lf\n",format->WidthCodes[i],format->Scale * doc->getWidthTable()[i]->getSizeA());
   }

   if ((textheight = (Textheight *)calloc(MAX_TEXTHEIGHT,sizeof(Textheight))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   else
   {
      // here i need to get all text sizes used.
      pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);
         CDI_gettext(&(file->getBlock()->getDataList()), file->getScale() * format->Scale, file->isMirrored());
      }

      sort_textheight();
      if (textheightcnt > 8)
      {
         double step = 0.002; // in mils
         // do it until TRUE, this means 8 or less found.
         while (!optimize_textheight(step))
            step += 0.002;
      }
      else
      {
         int   i;
         for (i=0;i<textheightcnt;i++)
            textheight[i].code = i;
      }

      // it must be written COD 0 to 7
      int p, done;

      for (p=0;p<8;p++)
      {
         done = FALSE;
         for (i=0;i<textheightcnt && !done;i++)
         {
            if (textheight[i].code == p)
            {
               fprintf(wfp,"TEX %d %02.3lf %02.3lf\n",textheight[i].code,textheight[i].h,0.01);
               done = TRUE;
            }
         }
         if (!done)
            fprintf(wfp,"TEX %d %02.3lf %02.3lf\n",p,0.05,0.01);
      }
   } // else textheight mem

   // end of assign
   fprintf(wfp,"\n");

   // EXPLODEALL,
   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);

      CDI_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * format->Scale,
               file->getInsertY() * format->Scale,
               file->getRotation(), 
               file->isMirrored(), 
               file->getScale() * format->Scale, 0,-1);
   }
   wCDI_Graph_File_Close(xmin,ymin,xmax,ymax);

   if (xmin < 0 || ymin < 0)
   {
      fprintf(flog, "CDI Minimum Coor [%2.3lf, %2.3lf] are not allowed to be below 0,0\n",
            xmin, ymin);
      display_error++;
   }


   free(textheight);
   textheightcnt = 0;
   // close write file
   fclose(wfp);
   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

//--------------------------------------------------------------
void CDI_WriteData(CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   const char *lname;
   DataStruct *np;
   POSITION pos;
   int  layer;
   
   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
//      Get_Layer_Layername
      if (np->getDataType() != T_INSERT)
      {
         if (np->getLayerIndex() == -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((lname = Get_Layer_Layername(doc->get_layer_mirror(np->getLayerIndex(), mirror))) == NULL)
                  continue;
        wCDI_Graph_Level(lname);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            Point2 *points;
            int    cnt = 0;

            int linecode;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               linecode = format->WidthCodes[poly->getWidthIndex()];
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); 
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               
               Point2 p2;
               cnt = 0;
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
                  points[cnt] = p2;
                  cnt++;
               }
               wCDI_Graph_Polyline(points,cnt,polyFilled, linecode);
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            if (mirror) point2.x = -point2.x;

            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot;
            if (mirror)
               text_rot = rotation - np->getText()->getRotation();
            else
               text_rot = rotation + np->getText()->getRotation();
            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            int textptr = store_textheight(np->getText()->getHeight()*scale);

            // in CADSTAR mirror text has a shift in insert point.

            wCDI_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              textheight[textptr].code,
                              np->getText()->getHeight()*scale,
                              text_rot,text_mirror);
         }
         break;

         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  wCDI_Graph_Level(PenNum);
                  penWidth = HP[PenNum].width * unitsFactor;
                  wCDI_Graph_Aperture(block->ApShape,
                                      point2.x,
                                      point2.y,
                                      block->getSizeA() * unitsFactor,
                                      block->getSizeB() * unitsFactor, penWidth);
*/
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               point2.x = np->getInsert()->getOriginX() * scale;
               if (mirror) point2.x = -point2.x;
               point2.y = np->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x,insert_y);
               CDI_WriteData(&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1,block_layer);
            } // end else not aperture
         }
         break;
      } // end switch
   } // end for
} /* end CDI_WriteData */

//--------------------------------------------------------------
static void CDI_gettext(CDataList *DataList, double scale, BOOL mirror)
{
   DataStruct *np;
   BlockStruct *block;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_TEXT:
         {
            if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
               continue;
            store_textheight(np->getText()->getHeight()*scale);
         }
         break;
         case T_INSERT:
            block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            CDI_gettext(&(block->getDataList()), scale * np->getInsert()->getScale(), (np->getInsert()->getMirrorFlags() ^ mirror));
         break;
         } // end switch
      } // end for

} /* end CDI_gettext */

static int store_textheight(double h)
{
   int   i;

   for (i=0;i<textheightcnt;i++)
   {
      if (fabs(textheight[i].h - h) < 0.001) // 1 mil is CADSTAR smallest unit
         return i;
   }

   if (textheightcnt < MAX_TEXTHEIGHT)
   {
      textheight[textheightcnt].h = h;
      textheightcnt++;
   }
   else
   {
      // too many text height
   }

   return textheightcnt-1;
}

static int optimize_textheight(double step)
{
   int i, tcnt = textheightcnt;
   int code = 0;

   for (i=1;i<textheightcnt;i++)
   {
      if ((textheight[i-1].h - textheight[i].h) < step)
      {
         textheight[i-1].code = code;
         textheight[i].code = code;
         code++;
         tcnt--;
      }
   }

   if (tcnt < 8)
      return TRUE;

   return FALSE;
}

static void sort_textheight()
{
   int   done = FALSE;
   int   i;
   double h;

   while (!done)
   {
      done = TRUE;
      for (i=1;i<textheightcnt;i++)
      {
         if (textheight[i-1].h > textheight[i].h)
         {
            // swap
            h = textheight[i-1].h;
            textheight[i-1].h = textheight[i].h;
            textheight[i].h = h;
            done = FALSE;
         }
      }
   }
   return;
}

// end CDI_OUT.CPP
