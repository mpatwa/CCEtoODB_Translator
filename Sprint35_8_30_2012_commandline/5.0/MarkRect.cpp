// $Header: /CAMCAD/5.0/MarkRect.cpp 32    3/12/07 12:42p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/
 
#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "ccdoc.h"
#include "extents.h"
#include "draw.h"
#include "bb_lib.h"
#include "GerberEducatorUi.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include "dmldefs.h"
#include "c2cdefs.h"
#include "c2ddefs.h"
}

static BOOL PolyBox(CCEtoODBDoc *doc, double xmin, double xmax, double ymin, double ymax, 
      CPolyList *polylist, double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m);

/*******************************************************************************
* MarkRect
*/
void CCEtoODBDoc::MarkRect(double xmin, double xmax, double ymin, double ymax, BOOL CompletelyEnclosedIn)
{
   CWaitCursor wait;

   BlockStruct *subblock;
   ExtentRect extents;

   int selectListModificationCount = SelectList.getModificationCount();
   double spaceRatio = getSettings().getTextSpaceRatio();

   for (POSITION filePos = getFileList().GetHeadPosition(); filePos != NULL;)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      double fileX   = file->getInsertX();
      double fileRot = file->getRotation();

      if (getBottomView())
      {
         fileX = -fileX;
         fileRot = -fileRot;
      }

		Mat2x2 m;
      RotMat2(&m, fileRot);

      CTMatrix matrix;
      matrix.scale(file->getResultantMirror(getBottomView()) ? -file->getScale() : file->getScale(), file->getScale());
      matrix.rotateRadians(fileRot);
      matrix.translate(fileX, file->getInsertY());

      for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition(); dataPos != NULL;)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         int layerIndex = data->getLayerIndex();

#ifdef UseIsEntityVisible
         if (!IsEntityVisible(data, &subblock, this, (file->getResultantMirror(Bottom) ? MIRROR_FLIP : 0), &layerIndex, FALSE))
            continue;
#else
         if (!data->isSelectable(getCamCadData() /*,NULL,file->getResultantMirror(Bottom)*/))
            continue;

         if (data->getDataType() == dataTypeInsert)
         {
            subblock = getBlockAt(data->getInsert()->getBlockNumber());
         }
#endif

         switch (data->getDataType())
         {
         case dataTypePoint:
            {
               Point2 p;
               p.x = data->getPoint()->x * file->getScale();

               if (file->getResultantMirror(getBottomView())) p.x = -p.x;

               p.y = data->getPoint()->y * file->getScale();
               TransPoint2(&p, 1, &m, file->getInsertX(), file->getInsertY());

               extents.left = extents.right = p.x;
               extents.bottom = extents.top = p.y;
            }
            break;

         case dataTypePoly:
            PolyExtents(this, data->getPolyList(), &extents, file->getScale(), (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0), 
						fileX, file->getInsertY(), &m, TRUE);
            break;

         case dataTypeText:
            {
               CExtent textExtent = data->getText()->getExtent(spaceRatio,matrix);
               extents.bottom = textExtent.getYmin();
               extents.top    = textExtent.getYmax();
               extents.left   = textExtent.getXmin();
               extents.right  = textExtent.getXmax();

               //TextExtents(this, np, &extents, file->getScale(), (file->isMirrored() ^ Bottom ? MIRROR_FLIP : 0), 
               //   file->getInsertX(), file->getInsertY(), file_rot);
            }
            break;  

         case dataTypeInsert:
            {
               if (subblock->getName().CompareNoCase("GE_Component-0") == 0)
               {
                  int iii = 3;
               }

               // calculate point
               Point2 point2;
               point2.x = data->getInsert()->getOriginX() * file->getScale();
               point2.y = data->getInsert()->getOriginY() * file->getScale();

               if (file->getResultantMirror(getBottomView())) point2.x = -point2.x;

               TransPoint2(&point2, 1, &m, fileX, file->getInsertY());

               bool DrawAp       = false;
               bool DrawSubblock = false;
               bool IsTool       = (subblock->getFlags() & BL_TOOL    ) || (subblock->getFlags() & BL_BLOCK_TOOL    );
               bool IsAp         = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
           
               double _x      = point2.x;
               double _y      = point2.y;
               double _rot    = file->getRotation();
               double _scale  = file->getScale();
               int    _mirror = (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0);
               int    _layer  = layerIndex;

               // TOOL
               if (IsTool)
               {
                  if (subblock->getToolDisplay()) // use another block's graphics to display this tool
                  {
                     subblock = this->Find_Block_by_Num(subblock->getToolBlockNumber());

                     if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                        DrawAp = true;
                     else
                        DrawSubblock = true;
                  }
                  else // draw generic tool
                  {
                     extents.left = extents.right = _x;
                     extents.bottom = extents.top = _y;
                  }
               } // end TOOL

               // APERTURE
               if (IsAp || DrawAp)
               {
                  // COMPLEX APERTURE
                  if (subblock->getShape() == T_COMPLEX)
                  {
                     // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)
                     //_x = ;
                     //_y = ;
                     _rot += subblock->getRotation();
      
                     // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                     subblock = Find_Block_by_Num((int)(subblock->getSizeA()));
                     DrawSubblock = TRUE;
                  }
      
                  // NORMAL APERTURE
                  else
                  {
                     double sizeA = subblock->getSizeA();
                     double sizeB = subblock->getSizeB();

                     switch (subblock->getShape())
                     {
                     case T_RECTANGLE:
                     case T_OBLONG:
                        extents.left   = _x - sizeA/2;
                        extents.right  = _x + sizeA/2;
                        extents.bottom = _y - sizeB/2;
                        extents.top    = _y + sizeB/2;
                        break;
                     case T_SQUARE:
                     case T_ROUND:
                     case T_DONUT:
                     case T_TARGET:
                     case T_THERMAL:
                     case T_OCTAGON:
                        extents.left   = _x - sizeA/2;
                        extents.right  = _x + sizeA/2;
                        extents.bottom = _y - sizeA/2;
                        extents.top    = _y + sizeA/2;
                        break;
                     case T_COMPLEX:
                     case T_UNDEFINED:
                     default:
                        extents.left   = _x;
                        extents.right  = _x;
                        extents.bottom = _y;
                        extents.top    = _y;

                        break;
                     }
                  } // end NORMAL APERTURE
               } // end APERTURE
      
               // GRAPHIC BLOCK
               if (!IsAp && !IsTool) 
               {
                  DrawSubblock = TRUE;
   
                  if (file->getResultantMirror(getBottomView()))
                     _rot = file->getRotation() - data->getInsert()->getAngle();
                  else
                     _rot = file->getRotation() + data->getInsert()->getAngle();

                  _mirror = (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0) ^ data->getInsert()->getMirrorFlags();
                  _scale = file->getScale() * data->getInsert()->getScale();
               }   

               // Draw Block
               if (DrawSubblock)
               {
                  if (subblock->getDataList().IsEmpty())
                  {     
                     extents.left = extents.right = _x;
                     extents.top = extents.bottom = _y;
                  }
                  else
                  {
                     double _xmin, _xmax, _ymin, _ymax;

                     _xmin = _ymin =  FLT_MAX;
                     _xmax = _ymax = -FLT_MAX;

                     // use extents if at a 90 degree angle
                     if (abs(round(RadToDeg(_rot)) % 90) < 2)
                     {
                        validateBlockExtents(subblock);
      
                        if (subblock->extentIsValid())
                           UseInsertExtents(&_xmin, &_xmax, &_ymin, &_ymax, subblock, _x, _y, _rot, _mirror, _scale);
                     }
                     else
                     {
                        block_extents(this, &_xmin, &_xmax, &_ymin, &_ymax, &(subblock->getDataList()), _x, _y, _rot, _mirror, _scale, layerIndex, TRUE);
                     }

                     if (_xmin > _xmax)
                     {
                        continue;
                     }

                     extents.left   = _xmin;
                     extents.right  = _xmax;
                     extents.bottom = _ymin;
                     extents.top    = _ymax;
                  }
               } // end Draw Block
            }

            break;
         } // end switch type

         BOOL Mark = FALSE;

         // Completely Enclosed In
         if (extents.left >= xmin && extents.right <= xmax && extents.bottom >= ymin && extents.top <= ymax)
         {
            Mark = TRUE;
         }
         else if (!CompletelyEnclosedIn)
         {
            if (data->getDataType() != dataTypePoly)
            {
               if (extents.right >= xmin && extents.left <= xmax && extents.top >= ymin && extents.bottom <= ymax)
                  Mark = TRUE;
            }
            else // POLY
            {
               Mark = PolyBox(this, xmin, xmax, ymin, ymax, data->getPolyList(), file->getScale(), file->getResultantMirror(getBottomView()), file->getInsertX(), file->getInsertY(), &m);
            }
         }

         if (Mark)
         {
            data->setMarked(true);
            SelectStruct *s = InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
                  file->getResultantMirror(getBottomView()), &(file->getBlock()->getDataList()), TRUE, FALSE);
            DrawEntity(s, 2, FALSE);
         }
      } // end loop through datas
   } // end loop thorough files

   updateGerberEducator(selectListModificationCount);

   if (SelectList.getModificationCount() != selectListModificationCount)
   {
      CGerberEducatorUi::getGerberEducatorUi(*this).updateSelectedEntity();
   }
}


/******************************************************************************
* PolyBox
*/
BOOL PolyBox(CCEtoODBDoc *doc, double xmin, double xmax, double ymin, double ymax, 
      CPolyList *polylist, double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m)
{
   // create curve for bounding box
   C2_CURVE top, bottom, left, right;
   top = BB_CreateCurve(xmin, ymax, xmax, ymax, 0);
   bottom = BB_CreateCurve(xmin, ymin, xmax, ymin, 0);
   left = BB_CreateCurve(xmin, ymin, xmin, ymax, 0);
   right = BB_CreateCurve(xmax, ymin, xmax, ymax, 0);

   BOOL Intersects = FALSE;

   // loop polys
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos && !Intersects)
   {
      CPoly *poly = polylist->GetNext(polyPos);

      CPnt *lastPnt;
      POSITION pntPos = poly->getPntList().GetHeadPosition();
      if (pntPos != NULL)
         lastPnt = poly->getPntList().GetNext(pntPos);
      while (pntPos && !Intersects)
      {                    
         CPnt *pnt = poly->getPntList().GetNext(pntPos);

         Point2 point, lastPoint;
         point.x = pnt->x * scale;
         if (mirror & MIRROR_FLIP) point.x = -point.x;
         point.y = pnt->y * scale;
         TransPoint2(&point, 1, m, insert_x, insert_y);

         lastPoint.x = lastPnt->x * scale;
         if (mirror & MIRROR_FLIP) lastPoint.x = -lastPoint.x;
         lastPoint.y = lastPnt->y * scale;
         TransPoint2(&lastPoint, 1, m, insert_x, insert_y);

         double bulge = lastPnt->bulge;
         if (mirror & MIRROR_FLIP)
            bulge = -bulge;

         C2_CURVE curve = BB_CreateCurve(lastPoint.x, lastPoint.y, point.x, point.y, bulge);

         INT inter_type[2];
         REAL param1[2], param2[2];
         PT2 int_pt[2];
         BOOLEAN near_tan[2];
         if (c2c_intersect_line_or_arc(top, curve, param1, param2, int_pt, inter_type, near_tan) || 
               c2c_intersect_line_or_arc(bottom, curve, param1, param2, int_pt, inter_type, near_tan) || 
               c2c_intersect_line_or_arc(left, curve, param1, param2, int_pt, inter_type, near_tan) || 
               c2c_intersect_line_or_arc(right, curve, param1, param2, int_pt, inter_type, near_tan) )
            Intersects = TRUE;  

         // free curve
         c2d_free_curve(curve);    

         lastPnt = pnt;
      }
   }

   // free box                                                                       
   c2d_free_curve(top);
   c2d_free_curve(bottom);
   c2d_free_curve(left);
   c2d_free_curve(right);

   return Intersects;
}


