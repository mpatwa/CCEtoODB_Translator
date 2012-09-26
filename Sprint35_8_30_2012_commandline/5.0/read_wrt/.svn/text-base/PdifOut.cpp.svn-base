// $Header: /CAMCAD/4.5/read_wrt/PdifOut.cpp 21    4/28/06 6:53p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD        
  Router Solutions Inc.            
  Copyright © 1994, 2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "pdiflib.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "graph.h"

#define  WR_ROOT_ONLY      20
#define  WR_ALL_ENTITYS    30

#define  MAX_APPNUM        1000

static CCEtoODBDoc *doc;
static FormatStruct *format;
static double unitsFactor;

static int    *appnum;
static int    appnumcnt = 0;

static int  write_app(double scale);

static void PDIF_WriteFiles();

void PDIF_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror, double scale, int embeddedLevel, int insertlayer);

//--------------------------------------------------------------
static int get_appnum(int dcode)
{
   int   i;

   for (i=0;i<appnumcnt;i++)
   {
      if (appnum[i] == dcode)
         return i+1;
   }

   return 1;
}

//--------------------------------------------------------------
void PDIF_Layout_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, 
                     double UnitsFactor)
{

// format->PDIF_True_TextRot

   FILE *wfp;
   LayerStruct  *np;

   format = Format;
   doc = Doc;
   unitsFactor = UnitsFactor;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      return;
   }

   // malloc applist
   if ((appnum = (int *)calloc(MAX_APPNUM,sizeof(int))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   appnumcnt = 0;

   wpdif_Graph_Init();

   // file open does COMPONENT
   char  f[_MAX_FNAME];   // filename
   _splitpath(filename,NULL,NULL,f,NULL);
   wpdif_Graph_File_Open_Layout(wfp, f);

   wpdif_Write_Line("{ENVIRONMENT");
   wpdif_plusident();

   wpdif_Write_Line("{PDIFvrev 7.00}");
   wpdif_Write_Line("{Program \"PDIF-OUT Version 7.0\"}");
   wpdif_Write_Line("{DBtype \"PC-Board\"}");
   wpdif_Write_Line("{DBvrev 2.08}");
   wpdif_Write_Line("{DBunit \"CENTIMIL\"}");
   wpdif_Write_Line("{DBgrid 1}");

   wpdif_plusident();
   wpdif_Write_Line("{Lyrstr");

   int lcnt = 0;

   for (int j=0; j < doc->getMaxLayerIndex(); j++)
   {
      np = doc->getLayerArray()[j];
      if (np == NULL)   continue; // could have been deleted.

      if (np->isVisible())
      {
         const char *lname;
         char tmp[80];
         if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
         {
            sprintf(tmp,"\"%s\" %d",lname,(lcnt++ % 16) + 1);
            wpdif_Write_Line(tmp);
         }
      }
   }

   wpdif_minusident();  // end LYRSTR
   wpdif_Write_Line("}");

   wpdif_minusident();  // end ENVIRONEMT
   wpdif_Write_Line("}");

 // APERTURE_TABLE
   wpdif_Write_Line("{APERTURE_TABLE");
   wpdif_plusident();
   write_app(UnitsFactor);                      
   wpdif_minusident();  // end Apertur
   wpdif_Write_Line("}");

 // SYMBOL

 // DETAIL
   wpdif_Write_Line("{DETAIL");
   wpdif_plusident();

   // ANNOTATE
   wpdif_Write_Line("{ANNOTATE");
   wpdif_plusident();

   PDIF_WriteFiles();
   // end of annotatote
   wpdif_minusident();
   wpdif_Write_Line("}");

   wpdif_Write_Line("%% netdef %%");
   wpdif_Write_Line("%% padstack %%");

   // do SUBCOMP
   wpdif_Write_OddAngleText();
   // end of SUBCOMP

   // end detail
   wpdif_minusident();
   wpdif_Write_Line("}");

   // Does end of COMPONENT
   wpdif_Graph_File_Close();

   // close write file
   fclose(wfp);

   free(appnum);

   return;
}

void PDIF_Schematic_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, 
                     double UnitsFactor)
{
   FILE *wfp;
   LayerStruct  *np;

   format = Format;
   doc = Doc;
   unitsFactor = UnitsFactor;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      return;
   }

   wpdif_Graph_Init();

   // file open does COMPONENT
   char  f[_MAX_FNAME];   // filename
   _splitpath(filename,NULL,NULL,f,NULL);
   wpdif_Graph_File_Open_Schematic(wfp, f);

   wpdif_Write_Line("{ENVIRONMENT");
   wpdif_plusident();

   wpdif_Write_Line("{PDIFvrev 7.00}");
   wpdif_Write_Line("{Program \"PDIF-OUT Version 7.0\"}");
   wpdif_Write_Line("{DBtype \"Schematic\"}");
   wpdif_Write_Line("{DBvrev 2.08}");
   wpdif_Write_Line("{DBunit \"CENTIMIL\"}");
   wpdif_Write_Line("{DBgrid 10}");

   format->Scale = format->Scale / 10;  // this is how schematic is scaled.

   wpdif_plusident();
   wpdif_Write_Line("{Lyrstr");

   int lcnt = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      np = doc->getLayerArray()[j];
      if (np == NULL)   continue; // could have been deleted.

      if (np->isVisible())
      {
         const char *lname;
         char tmp[80];
         if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
         {
            sprintf(tmp,"\"%s\" %d",lname,(lcnt++ % 16) + 1);
            wpdif_Write_Line(tmp);
         }
      }
   }

   wpdif_minusident();  // end LYRSTR
   wpdif_Write_Line("}");

   wpdif_minusident();  // end ENVIRONEMT
   wpdif_Write_Line("}");

 // DETAIL
   wpdif_Write_Line("{DETAIL");
   wpdif_plusident();

   // ANNOTATE
   wpdif_Write_Line("{ANNOTATE");
   wpdif_plusident();

   PDIF_WriteFiles();

   // end of annotate
   wpdif_minusident();
   wpdif_Write_Line("}");

   wpdif_Write_Line("// netdef");
   wpdif_Write_Line("// padstack");
   wpdif_Write_Line("// subcomp");

   // end detail
   wpdif_minusident();
   wpdif_Write_Line("}");

   // Does end of COMPONENT
   wpdif_Graph_File_Close();

   // close write file
   fclose(wfp);

   return;
}

void PDIF_WriteFiles()
{
   // EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   FileStruct *filePtr;
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      filePtr = doc->getFileList().GetNext(pos);

      PDIF_WriteData(&(filePtr->getBlock()->getDataList()), filePtr->getInsertX() * format->Scale * unitsFactor,
         filePtr->getInsertY() * format->Scale * unitsFactor,
         filePtr->getRotation(), filePtr->isMirrored(), filePtr->getScale() * format->Scale * unitsFactor, 0, -1);
   }
   return;
}

//--------------------------------------------------------------
void PDIF_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   const char *lname;
   int   layer;

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
         wpdif_Graph_Level(Get_Layer_Layername(doc->get_layer_mirror(layer, mirror)));
      }

      switch(np->getDataType())
      {
         case T_POLY:
            if (doc->get_layer_visible(np->getLayerIndex(), mirror))
            {
               CPoly *poly;
               CPnt  *pnt;
               POSITION polyPos, pntPos;

               // loop thru polys
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);
                  BOOL polyFilled = poly->isFilled();
                  BOOL closed     = poly->isClosed();

                  Point2   p;
                  
                  int vertexcnt = 0;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     vertexcnt++;
                  }        
                  int tcnt = vertexcnt;
                  Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

                  if( points != NULL )
                  {
                     vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        wpdif_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
                        p.x = pnt->x * scale;
                        if (mirror) p.x = -p.x;
                        p.y = pnt->y * scale;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);
                        // need to check for bulge
                        points[vertexcnt].x = p.x;
                        points[vertexcnt].y = p.y;
                        points[vertexcnt].bulge = p.bulge;
                        vertexcnt++;
                     }
                     wpdif_Graph_Polyline(points,vertexcnt,polyFilled);
                     free(points);
                  }
               }
            } // if color
         break;

         case T_TEXT:
         {
            double text_rot;
   
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            if (mirror)
            {
               text_rot = rotation - np->getText()->getRotation();
               point2.x = -point2.x;
            }
            else
               text_rot = rotation + np->getText()->getRotation();

            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            wpdif_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            Point2 point2;

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
                  if ((lname = Get_Layer_Layername(doc->get_layer_mirror(layer, mirror))) != NULL)
                  {
                    wpdif_Graph_Level(lname);

                    wpdif_Graph_Aperture(point2.x,point2.y,get_appnum(block->getBlockNumber()));
                  }
               } // end if layer visible
            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               PDIF_WriteData(&(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1,-1);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for
   return;
} /* end PDIF_WriteData */

/*****************************************************************************/
/*
*/
int write_app(double unitsFactor)
{
   char ApString[120];

   wpdif_Write_Line("{Apfile }");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if ( (block->getFlags() & BL_BLOCK_APERTURE) || (block->getFlags() & BL_APERTURE) )
      {
         if (appnumcnt < MAX_APPNUM)
         {
            appnum[appnumcnt] = block->getBlockNumber();
            appnumcnt++;
         }


         sprintf(ApString,"{Aprnum %d",appnumcnt);
         wpdif_Write_Line(ApString);
         wpdif_plusident();

         // PCAD can not deal with aperture of size 0
         if ((block->getSizeA() * unitsFactor) < 0.01) 
         {
            wpdif_Write_Line("%% Zero size Aperture");
            wpdif_Write_Line("{Shp ROUND}");
            sprintf(ApString,"{DCODE %d}",block->getDcode());
            wpdif_Write_Line(ApString);
            sprintf(ApString,"{Odia 1.00}");
            wpdif_Write_Line(ApString);
         }
         else  
         {
         switch (block->getShape())
         {
            case T_ROUND:
               wpdif_Write_Line("{Shp ROUND}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Odia %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_SQUARE:
               wpdif_Write_Line("{Shp RECTANGLE}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Owd %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Oht %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_TARGET:
               wpdif_Write_Line("{Shp TARGET}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Odia %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_OCTAGON:
               wpdif_Write_Line("{Shp OCTAGON}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Owd %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Oht %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_RECTANGLE:
               wpdif_Write_Line("{Shp RECTANGLE}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Owd %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Oht %1.2lf}",block->getSizeB() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_DONUT:
               wpdif_Write_Line("{Shp DONUT}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Odia %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_THERMAL:
               wpdif_Write_Line("{Shp THERMAL}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Odia %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
               // pcad does not allows the inner equal the outer
               if (block->getSizeA() > block->getSizeB())
                  sprintf(ApString,"{Idia %1.2lf}",block->getSizeB() * unitsFactor);
               else
                  sprintf(ApString,"{Idia %1.2lf}",(block->getSizeB() * unitsFactor) - 1);

               wpdif_Write_Line(ApString);
               wpdif_Write_Line("{Ti 2}"); 
               wpdif_Write_Line("{Twd 6.0}"); 
               wpdif_Write_Line("{Ta 0}"); 
               wpdif_Write_Line("{Aptype FLASH}"); 
            break;
            case T_OBLONG:
               wpdif_Write_Line("{Shp OBLONG}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Owd %1.2lf}",block->getSizeA() * unitsFactor);
               wpdif_Write_Line(ApString);
               sprintf(ApString,"{Oht %1.2lf}",block->getSizeB() * unitsFactor);
               wpdif_Write_Line(ApString);
            break;
            case T_COMPLEX:
               wpdif_Write_Line("{Shp CUSTOM}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
            break;
            case T_UNDEFINED:
            default:
               wpdif_Write_Line("{Shp UNDEF}");
               sprintf(ApString,"{DCODE %d}",block->getDcode());
               wpdif_Write_Line(ApString);
            break;
         } // end switch
         }  // not zero
         wpdif_minusident();
         wpdif_Write_Line("}");
      } // end if aperture
   } // end for loop through blocklist
   return TRUE;
}

// end PDIFOUT.CPP