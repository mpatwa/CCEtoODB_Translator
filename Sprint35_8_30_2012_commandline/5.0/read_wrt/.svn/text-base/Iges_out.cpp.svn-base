// $Header: /CAMCAD/4.5/read_wrt/Iges_out.cpp 19    4/28/06 6:52p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1998. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "igeslib.h"
#include "lyr_lyr.h"
#include "apertur2.h"
#include "polylib.h"
#include "graph.h"

#define  WR_ROOT_ONLY      20
#define  WR_ALL_ENTITYS    30

static CCEtoODBDoc *doc;

static void IGES_WriteData(int explodeStyle, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int insertLayer);

//--------------------------------------------------------------
void IGES_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int explodeStyle)
{
   FILE *wfp;
   POSITION pos;
   FileStruct *file;

   doc = Doc;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }

	wiges_Graph_Init(format->PortFileUnits);

   // here we need to do decimal points, realwidth
   // from what width on lines and arcs and circles
   // are drawn with a stroke or width
   wiges_Graph_File_Open(wfp, NULL);
   wiges_Graph_Line_End(format->AddLineEnds);


   // no explode is not implemented.
   // if NOEXPLODE,
   //   write entire list in order,
   //   then write file inserts
/* if (flag & NO_EXPLODE)
   {

      MessageBox(NULL, "1", "IGES", MB_OK | MB_ICONHAND);

      for (filePtr; filePtr->next != NULL; filePtr=filePtr->next)
      {
         wiges_Graph_Block_Reference( filePtr->name,
                                     filePtr->insert_x * exportScale,
                                     filePtr->insert_y * exportScale,
                                     0.0,
                                     filePtr->rotation, filePtr->mirror,
                                     filePtr->scale * exportScale);
      }
   }
*/

   // only explode root is implemented.

   if (explodeStyle == EXPLODE_ALL)
   {
      // if EXPLODEALL,
      //   for each file:
      //     write entities of block (including any embedded inserts)
      pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);
         if (!file->isShown())  continue;

			double toIgesUnitsFactor = getUnitsFactor(doc->getPageUnits(), (PageUnitsTag)format->PortFileUnits);
			double conversionFactor = toIgesUnitsFactor * format->Scale;

         IGES_WriteData(explodeStyle, &(file->getBlock()->getDataList()), 
               file->getInsertX() * conversionFactor, file->getInsertY() * conversionFactor,
               file->getRotation(), file->isMirrored(), 
               file->getScale() * conversionFactor, -1);
      }
   }
   else
   if (explodeStyle == EXPLODE_ROOT)
   {
#ifdef NOT_IMPLEMENTED
      // if EXPLODEROOT,
      //   for each file:
      //     write data inside block (including any embedded inserts)
      for (int i=0; i<doc->maxBlockArray; i++)
      {
         BlockStruct *block = doc->BlockArray[i];
         if (block == NULL)   continue;

         if (block->getFlags() & BL_WIDTH)
            continue;

         // create necessary aperture blocks
         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            if (block->getShape() == T_COMPLEX)
            {
               int dcode = block->getDcode();
               block = doc->Find_Block_by_Num((int)(block->getSizeA()));

               if (!block) continue;
               if (block->getDataList().IsEmpty()) continue;
/*
                  sprintf(prosa,"%%AM%s*\n",block->name);
                  wgerb_write2file(prosa);
                  Gerb_WriteAppData(&(block->getDataList()),
                     0.0,
                     0.0,
                     0.0,
                     0,
                     format->Scale, block->name);
                  wgerb_write2file("\n%%\n");
*/
            }
            else
            {
               wiges_Graph_Block_On(block->name);
               wiges_Graph_Level("0");
               wiges_Graph_Aperture(block->getShape(), 0.0, 0.0, block->getSizeA(), block->getSizeB());
               wiges_Graph_Block_Off();
            }
         }
         // create all other blocks that are not file blocks
         else if (!(block->getFlags() & BL_FILE))
         {
            wiges_Graph_Block_On(block->name);
            IGES_WriteData(explodeStyle, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, -1);
            wiges_Graph_Block_Off();
         }
      }

      // write file blocks as exploded
      pos = doc->FileList.GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->FileList.GetNext(pos);
         if (!file->isShown())  continue;

         IGES_WriteData(explodeStyle, &(file->getBlock()->getDataList()), file->getInsertX() * format->Scale,
               file->getInsertY() * format->Scale,
               file->getRotation(), 
               file->isMirrored(), 
               file->getScale() * format->Scale, -1);
      }
#endif  
   }
   wiges_Graph_File_Close();

   // close write file
   fclose(wfp);
   return ;
}

//--------------------------------------------------------------
void IGES_WriteData(int explodeStyle, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int insertLayer)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   char        tmp[255];
   POSITION    pos;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         // insert has not always a layer definition.
         if (np->getLayerIndex() < 0 || doc->IsFloatingLayer(np->getLayerIndex()))
            wiges_Graph_Level("0");
         else
         {
            if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
               continue;
            const char *layname = Get_Layer_Layername(doc->get_layer_mirror(np->getLayerIndex(), mirror));
            wiges_Graph_Level(layname);
         }
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

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               double w = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale;

               wiges_Graph_Line_Width(w);

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
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
                  if (mirror) p2.bulge = -p2.bulge;

                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  points[cnt] = p2;
                  cnt++;
               }
               wiges_Graph_Polyline(points,cnt,0.0, polyFilled);
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

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
/*
            Mat2x2   m1;
            RotMat2(&m1, text_rot);
*/
            int text_mirror = np->getText()->getResultantMirror(mirror);
            wiges_Graph_Text( np->getText()->getText(), point2.x, point2.y, 0.0,
                             np->getText()->getHeight()*scale, np->getText()->getWidth()*scale,   // char width
                             text_rot, text_mirror );
         }
         break;
   
         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ (np->getInsert()->getMirrorFlags() & MIRROR_FLIP);
            double block_rot;

            if (mirror & MIRROR_FLIP)
               block_rot = rotation - np->getInsert()->getAngle();
            else
               block_rot = rotation + np->getInsert()->getAngle();

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror & MIRROR_FLIP) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if ( (explodeStyle == NO_EXPLODE) || (explodeStyle == EXPLODE_ROOT) ) // write insert entity
               {
                  strcpy(tmp, block->getName());

                  const char *layname = Get_Layer_Layername(doc->get_layer_mirror(layer, mirror));
                  wiges_Graph_Block_Reference_Layer(layname, block->getName(),
                                        point2.x, point2.y, block_rot, block_mirror,
                                        np->getInsert()->getScale());
               }

               if (explodeStyle == EXPLODE_ALL) // then call IGES_WriteData() recursively to write embedded entities
               {
                  if (!doc->get_layer_visible(layer, mirror))
                     continue;

                  const char *layname = Get_Layer_Layername(doc->get_layer_mirror(layer, mirror));
                  wiges_Graph_Level(layname);

                  if (block->getShape() == T_COMPLEX)
                  {
                     BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                     IGES_WriteData(explodeStyle, &(subblock->getDataList()),
                           point2.x, point2.y,
                           block_rot, block_mirror, scale * np->getInsert()->getScale(),
                           np->getLayerIndex());
                     break;
                  }

                  CPolyList *poly_list;

                  if ((poly_list = ApertureToPoly_Base(block, 0.0, 0.0, block_rot, 0)) == NULL)
                     break;

                  //draw polylist---------------------------------
                  Point2   *points;
                  int      cnt = 0;
                  CPoly    *poly;
                  CPnt     *pnt;
                  POSITION polyPos, pntPos;

                  // count thru polys
                  polyPos = poly_list->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     poly = poly_list->GetNext(polyPos);
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        cnt++;
                     }
                  }  // while
   
                  Mat2x2   mm;
                  RotMat2(&mm, 0.0);   // do not double rotate. Once in block_rot when aperture to poly and than again.
            
                  points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                  // loop thru polys
                  polyPos = poly_list->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     poly = poly_list->GetNext(polyPos);

                     BOOL polyFilled = poly->isFilled();
                     BOOL closed     = poly->isClosed();
         
                     double   cx, cy, radius;
                     if (PolyIsCircle(poly, &cx, &cy, &radius))
                     {
                        Point2 p2;
                        p2.x = cx * scale;
                        p2.y = cy * scale;
                        radius *= scale;
                        TransPoint2(&p2, 1, &mm, insert_x, insert_y);
                        wiges_Graph_Circle(p2.x, p2.y, 0.0,radius);
                     }
                     else
                     {
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
                           if (mirror) p2.bulge = -p2.bulge;
                           TransPoint2(&p2, 1, &mm, insert_x, insert_y);
                           // here deal with bulge
                           points[cnt] = p2;
                           cnt++;
                        }
                        wiges_Graph_Polyline(points,cnt,0.0, polyFilled);
                     }
                  }  // while
                  free(points);
                  // free_poly_list
                  FreePolyList(poly_list);
               }
            }
            else // not an aperture
            {
               if ( (explodeStyle == NO_EXPLODE) || (explodeStyle == EXPLODE_ROOT) ) // write insert entity
               {
                  wiges_Graph_Block_Reference(doc->Find_Block_by_Num(np->getInsert()->getBlockNumber())->getName(),
                                        point2.x, point2.y,
                                        0.0,
                                        block_rot, block_mirror,
                                        np->getInsert()->getScale() * scale);
               }

               if (explodeStyle == EXPLODE_ALL) // then call IGES_WriteData() recursively to write embedded entities
               {
                  BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
                  IGES_WriteData(explodeStyle, &(block->getDataList()),
                        point2.x, point2.y,
                        block_rot, block_mirror, scale * np->getInsert()->getScale(),
                        np->getLayerIndex());
               } // end if EXPLODE_ALL
            } // end else not aperture
         } // end case INSERT:
         break;
      } // end switch
   } // end for
   return;
} /* end IGES_WriteData */

// end IGES_OUT.CPP
