// $Header: /CAMCAD/4.5/read_wrt/HpglOut.cpp 27    8/05/06 4:55p Kurt Van Ness $

/*****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "hpgllib.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_pen.h"
#include <math.h>
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
static   HPstruct       HP;
static   double         unitsFactor, mmFactor;
static   int            hpgl2 = TRUE;     // should output be written in HPGL2 format
static   double         realwidth = 0;    // lines smaller realwidth should be drawn with only one stroke
static   int            display_error;
static   FILE           *flog;

static void HPGL_WriteData(CDataList *DataList, 
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,double mmscale,
                    int insertLayer);

//--------------------------------------------------------------
void HPGL_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, 
                double UnitsFactor, // in HP units
                double  MMFactor   // in MM for text and pen width 
               )
{
   FILE     *wfp;

   // Set Static variables so don't have to pass them every time
   doc = Doc;
   unitsFactor = UnitsFactor;
   mmFactor = MMFactor;
   display_error = FALSE;

   HP = format->HP;

   hpgl2 = HP.HPGL2;
   realwidth = format->RealWidth * unitsFactor;
       
   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      ErrorMessage("Error open file", filename);
      return;
   }

   CString logFile = GetLogfilePath("HPGL.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   whpgl_Graph_Init(hpgl2);
   whpgl_Graph_File_Open(wfp);

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
      HPGL_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * unitsFactor,
          file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(),
         file->getScale() * unitsFactor, 
         file->getScale() * mmFactor, -1);
   }

   whpgl_Graph_File_Close();

   // close write file
   fclose(wfp);
   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

//--------------------------------------------------------------
static void write_attributes(CAttributes* map, double ix, double iy, double rotation, int mirror, double scale, double mmscale)
{
   if (map == NULL)
      return;

   CString  hp_line;
   WORD     keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      //if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      //if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      int      flg = 0;
      double   height, width, angle;

      if (!attrib->isVisible())
      {
         flg = 1;
         continue;      // do not show an invisble attribute.
      }

      height = attrib->getHeight() * scale;
      width  = attrib->getWidth() * scale;

      if (mirror)
         angle = rotation - attrib->getRotationRadians();
      else
         angle = rotation + attrib->getRotationRadians();

      // ix, iy
      Point2   point2;
      Mat2x2   m;

      RotMat2(&m, rotation);

      point2.x = attrib->getX()*scale;

      if (mirror)
      {
         point2.x = -attrib->getX()*scale;
      }

      point2.y = attrib->getY()*scale;

      TransPoint2(&point2, 1, &m, ix,iy);
      
      //if (!doc->get_layer_visible(np->getLayerIndex(), mirror, np->getDataType()))
      //    continue;
      //wdxf_Graph_Level(Get_Layer_Layername(doc->get_layer_mirror(attrib->layer, mirror)));
      if (!doc->get_layer_visible(attrib->getLayerIndex(), mirror))
         continue;

      int PenNum = Get_Layer_PenNum(attrib->getLayerIndex());

      whpgl_Graph_Level(PenNum);

      // if an attribute is set to be never mirrored.
      if (attrib->getMirrorDisabled()) mirror = 0;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         hp_line.Format("%d", attrib->getIntValue());

         // here do text
         whpgl_Graph_Text( hp_line, point2.x, point2.y, height, width,  angle, 0, mirror);

         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         hp_line.Format("%lg", attrib->getDoubleValue());
         // here do text
         // here do text
         whpgl_Graph_Text( hp_line, point2.x, point2.y, height, width,  angle, 0, mirror);
         break;
      case VT_STRING:
         {
            char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));
            
            tok = strtok(temp,"\n");

            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
                  hp_line.Format("%s", tok); // multiple values are delimited by \n
               else
               {                 
                  continue;
               }

               // here do text
               whpgl_Graph_Text( hp_line, point2.x, point2.y, height, width,  angle, 0, mirror);

               tok = strtok(NULL,"\n");
            }

            free(temp);
         }
         break;
      }
   }

   return;
}

//--------------------------------------------------------------
// special character defintions
// underbar is %U
// overbar  is %O
// percent  is %%
static const char *clean_text(const char *t)
{
   static   CString  tmp;
   int      i;

   tmp = "";
   for (i=0;i<(int)strlen(t); i++)
   {
      if (t[i] == '%')
      {
         if ((i+1) < (int)strlen(t))
         {
            if (t[i+1] == '%')
               i++;
         }
      }
      tmp += t[i];
   }

   return tmp.GetBuffer(0);
}

//--------------------------------------------------------------

void HPGL_WriteData(CDataList *DataList, 
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale, double mmscale,
                    int insertLayer)
{
   Mat2x2      m;
   Point2      p1;
   DataStruct  *np;
   int         PenNum = 0;
   double      penWidth = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      int dataLayerIndex = np->getLayerIndex();

      if (np->isHidden())  continue;

      if (np->getDataType() != T_INSERT)
      {
         // insert has not always a layer definition.
         if (dataLayerIndex < 0)
         {
            whpgl_Graph_Level(1);
            PenNum = 1;
         }
         else
         {
            LayerStruct* dataLayer = doc->getLayerAt(dataLayerIndex);
            int layerIndex;

            if (dataLayer != NULL && dataLayer->isFloating())
            {
               layerIndex = insertLayer;
            }
            else
            {
               layerIndex = dataLayerIndex;
            }

            if (!doc->get_layer_visible(layerIndex, mirror))
               continue;

            PenNum = Get_Layer_PenNum(layerIndex);
            whpgl_Graph_Level(PenNum);
         }
         if (PenNum > 0)
            penWidth = HP.HpPens[PenNum-1].width * scale;
         else
            penWidth = 0;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // need to fix this here correctly
            poly = np->getPolyList()->GetHead();
            if (poly->isHidden()) continue;

            double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
            double w = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
         
            whpgl_Graph_Line_Width(lineWidth);
            // do border
            if (lineWidth > realwidth && penWidth < lineWidth) // need to make multiple passes with pen to draw line of this width
            {
               // make a border around to get the real size.
               if (hpgl2)
               {
                  // PU must be in MM 
                  whpgl2_PenWidth(w*mmscale);
               }
               else
               {
                  // loop thru polys
                  polyPos = np->getPolyList()->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     poly = np->getPolyList()->GetNext(polyPos);
                     if (poly->isHidden()) continue;

                     BOOL polyFilled = poly->isFilled();

                     // this is drawn with the original dcode of the unfilled poly.
                     int first = TRUE;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        Point2   p1,p2;
                        pnt = poly->getPntList().GetNext(pntPos);
                        p2.x = pnt->x * scale;
                        if (mirror & MIRROR_FLIP) p2.x = -p2.x;
                        p2.y = pnt->y * scale;
                        p2.bulge = pnt->bulge;
                        if (mirror & MIRROR_FLIP) p2.bulge = -p2.bulge;

                        TransPoint2(&p2, 1, &m, insert_x, insert_y);
                        // here deal with bulge
                        if (first)
                        {
                           first = FALSE;
                        }
                        else
                        {
                           whpgl_Graph_Line_with_Width( p1.x, p1.y, p1.bulge, 
                                    p2.x, p2.y, p2.bulge,
                                    lineWidth, penWidth, RoundEnd );
                        }
                        p1 = p2;
                     }
                  }
               }
            }
            else
               whpgl2_PenWidth(-1); // reset previous width.

            // loop thru polys without width
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();

               // this is drawn with the original dcode of the unfilled poly.
               int first = TRUE;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  Point2   p;
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x * scale;
                  if (mirror & MIRROR_FLIP) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  if (mirror & MIRROR_FLIP) p.bulge = -p.bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  if (first)
                  {
                     whpgl_Graph_Polyline(p,polyFilled);
                     first = FALSE;
                  }
                  else
                     whpgl_Graph_Vertex(p);
               }
               whpgl_Graph_PolyClose(polyFilled);
            }
         }
         break;  // POLYSTRUCT
  
         case T_TEXT:
         {
            double text_rot;

            p1.x = (np->getText()->getPnt().x)*scale;
            p1.y = (np->getText()->getPnt().y)*scale;

            if (mirror)
            {
               text_rot = rotation - np->getText()->getRotation();
               p1.x = -p1.x;
            }
            else
               text_rot = rotation + np->getText()->getRotation();

            TransPoint2(&p1, 1, &m,insert_x,insert_y);

            int text_mirror = np->getText()->getResultantMirror(mirror);
            if (np->getText()->getMirrorDisabled())   text_mirror = 0;
      
            if (!strlen(np->getText()->getText()))  break;
            if (strlen(np->getText()->getText()) > 255) // hp limit
            {
               fprintf(flog,"HP Label too long [%d] maximum [255] characters allowed.\n", 
                  strlen(np->getText()->getText()));
               display_error++;
            }
            else
            {
               whpgl_Graph_Text( clean_text(np->getText()->getText()), p1.x, p1.y,
                       np->getText()->getHeight()*scale, np->getText()->getWidth()*scale,
                       text_rot, np->getText()->getOblique(), text_mirror);
            }
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
               block_rot = block_rot + block->getRotation();  // aperture rotation offset

               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if ( doc->get_layer_visible(layer, mirror) )
               {
                  PenNum = Get_Layer_PenNum(np->getLayerIndex());
                  whpgl_Graph_Level(PenNum);
                  //penWidth  = 0;

                  CPolyList *poly_list;
   
                  if (block->getShape() == T_COMPLEX)
                  {
                     BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                     HPGL_WriteData(&(subblock->getDataList()),
                               p1.x, p1.y,
                               block_rot, 
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               mmscale * np->getInsert()->getScale(),
                               np->getLayerIndex());
                     break;
                  }

                  if (hpgl2)
                  {
                     if ((poly_list = ApertureToPoly_Base(block, 0.0, 0.0, block_rot, 0)) == NULL)
                        break;

                     int      polyFilled = TRUE;
                     int      first = TRUE;
                     CPoly    *poly;
                     CPnt     *pnt;
                     POSITION polyPos, pntPos;
                     Point2   p2;
   
                     whpgl2_PenWidth(0);  // reset previous width.

                     polyPos = poly_list->GetHeadPosition();
                     while (polyPos != NULL)
                     {
                        first = TRUE;
                        poly = poly_list->GetNext(polyPos);
                        pntPos = poly->getPntList().GetHeadPosition();
                        while (pntPos != NULL)
                        {
                           pnt = poly->getPntList().GetNext(pntPos);
                           p2.x = pnt->x * scale;
                           if (mirror) p2.x = -p2.x;
                           p2.y = pnt->y * scale;
                           p2.bulge = pnt->bulge;
                           p2.x += p1.x;
                           p2.y += p1.y;
                           if (first)
                           {
                              whpgl_Graph_Polyline(p2,polyFilled);
                              first = FALSE;
                           }
                           else
                              whpgl_Graph_Vertex(p2);
                        }
                        if (!first) 
                           whpgl_Graph_PolyClose(polyFilled);
                     }
                     FreePolyList(poly_list);
                  } // if hpgl2
                  else
                  {
                     whpgl_Graph_Aperture(block->getShape(),
                                      p1.x, p1.y,
                                      block->getSizeA() * unitsFactor,
                                      block->getSizeB() * unitsFactor, 
                                      block_rot, 
                                      block->getXoffset() * unitsFactor,
                                      block->getYoffset() * unitsFactor, 
                                      penWidth);
                  }
               } // layer mirrored
            }
            else
            {
               HPGL_WriteData(&(block->getDataList()),
                            p1.x, p1.y,
                            block_rot, 
                            block_mirror,
                            scale * np->getInsert()->getScale(),
                            mmscale * np->getInsert()->getScale(),
                            np->getLayerIndex());
               // here write out attribute
               write_attributes(np->getAttributesRef(), p1.x, p1.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(), mmscale * np->getInsert()->getScale());

            } // end else not aperture
         break;
      } // end switch
   } // end for
   return;
} /* end HPGL_WriteData */

// end HPGLOUT.CPP

