// $Header: /CAMCAD/5.0/Extents.cpp 60    6/17/07 8:52p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "polylib.h"
#include "extents.h"
#include "apertur2.h"
#include "attrib.h"
#include "draw.h"
#include "pcbutil.h"
#include "RwLib.h"
#include <math.h>
#include <float.h>

static BlockStruct **WidthTable;

/*********************************************************************
* OnRegen
*/ //Keep
void CCEtoODBDoc::OnRegen() 
{
   // clear all validExtents
	int i=0;
   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (block == NULL)   continue;

      block->resetExtent();
   }

   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (block == NULL)   continue;

      if (block->extentIsValid()) // may have already been recalculated in recursion
         continue;

      CalcBlockExtents(block);
   }

#ifdef _DEBUG
   data_mem_calc(this);
#endif

}

/*********************************************************************************
* UseInsertExtents
*/
#if !defined(EnableDcaCamCadDocLegacyCode)
bool UseInsertExtents(double* lxmin, double* lxmax, double* lymin, double* lymax,BlockStruct* block, 
      double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   CExtent extent = UseInsertExtents(block, insert_x, insert_y, rotation, mirror, scale);

   if (extent.isValid())
   {
      *lxmin = extent.getXmin();
      *lymin = extent.getYmin();
      *lxmax = extent.getXmax();
      *lymax = extent.getYmax();
   }

   return extent.isValid();
}

CExtent UseInsertExtents(BlockStruct* block, double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   CExtent extent;

   if (block->getExtent().isSensible(1e10))
   {
      Mat2x2 m;
      RotMat2(&m, rotation);
      Point2 p;

      { // (xmin, ymin)
         p.x = block->getXmin() * scale;
         p.y = block->getYmin() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }

      { // (xmin, ymax)
         p.x = block->getXmin() * scale;
         p.y = block->getYmax() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }

      { // (xmax, ymax)
         p.x = block->getXmax() * scale;
         p.y = block->getYmax() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }

      { // (xmax, ymin)
         p.x = block->getXmax() * scale;
         p.y = block->getYmin() * scale;

         if (mirror & MIRROR_FLIP)  p.x = -p.x;

         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
      }
   }

   return extent;
}
#endif

/*********************************************************************
* func: get_extents
* gets the extents of everything visible in the current draw window
*/
void CCEtoODBDoc::get_extents(double *lxmin, double *lxmax, double *lymin, double *lymax)
{
   CExtent extent = getExtent();

   *lxmin = extent.getXmin();
   *lymin = extent.getYmin();
   *lxmax = extent.getXmax();
   *lymax = extent.getYmax();
}

CExtent CCEtoODBDoc::getExtent()
{
   CWaitCursor wait;
   int extentCount = 0;

   CExtent extent;
   
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      if (file->getBlock() == NULL)
         continue;

      double x = file->getInsertX();
      double fileRot = file->getRotation();
      int fileMirror = (getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

      if (getBottomView())
      {
         x = -x;
         fileRot = -fileRot;
      }

      // use extents if at a 90 degree angle
      if (abs(round(RadToDeg(file->getRotation())) % 90) < 2)
      {
         validateBlockExtents(file->getBlock());

         if (file->getBlock()->extentIsValid())
         {
				CExtent fileExtent = UseInsertExtents(file->getBlock(), x, file->getInsertY(), fileRot, fileMirror, file->getScale());
            extent.update(fileExtent);

            if (extent.isValid())
            {
               //if (!extent.isFloat())
               //{
               //   int iii = 3;
               //}

               extentCount++;
            }
         }
      }
      else
      {
         double xmin, xmax, ymin, ymax;

         block_extents(this, &xmin, &xmax, &ymin, &ymax, &file->getBlock()->getDataList(),
               x, file->getInsertY(), fileRot, fileMirror, file->getScale(), -1, TRUE);

         extent.update(xmin,ymin);
         extent.update(xmax,ymax);

         //if (!extent.isFloat())
         //{
         //   int iii = 3;
         //}

         extentCount++;
      }
   }

   if (extent.isValid())
   {
      double marginFactor = (0.01 * getSettings().ExtentMargin)*2.;

      extent.expand(marginFactor*extent.getXsize(), marginFactor*extent.getYsize());
   }

   return extent;
}

/******************************************************************************
* CalcBlockExtents
*/
void CCEtoODBDoc::CalcBlockExtents(BlockStruct* block)
{
   if (block != NULL)
   {
      block->calculateVisibleBlockExtents(getCamCadData());
   }
}

/******************************************************************************
* validateBlockExtents
*/
bool CCEtoODBDoc::validateBlockExtents(BlockStruct* block,bool checkOnlyVisibleEntitiesFlag)
{
   bool calculatedFlag = (block != NULL && ! block->extentIsValid());

   if (calculatedFlag)
   {
      block->calculateBlockExtents(getCamCadData(),checkOnlyVisibleEntitiesFlag);
   }

   return calculatedFlag;
}
   
/************************************************************************
* blockExtents
*/
CExtent CCEtoODBDoc::blockExtents(CDataList* DataList,     
      double insert_x,double insert_y,double rotation,int mirror,double scale,int insertLayer,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter)
{
	bool debugFlag = false;
   static int callLevel = 0;  // for debugging

   callLevel++;

   CExtent extent;
   BlockStruct *subblock;
   ExtentRect extents;
   int layer;
   int extentCount = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   CTMatrix matrix;

   matrix.scale(((mirror & MIRROR_FLIP) != 0) ? -scale : scale,scale);
   matrix.rotateRadians(rotation);
   matrix.translate(insert_x,insert_y);

   double spaceRatio = getSettings().getTextSpaceRatio();

   POSITION dataPos = DataList->GetHeadPosition();

   for (int index=0;dataPos != NULL;index++)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->isHidden())
         continue;

      if (data->getDataType() == dataTypeBlob)
      {
         Point2 p;
         p.x = data->getBlob()->pnt.x * scale;

         if (mirror & MIRROR_FLIP)
            p.x = -p.x;

         p.y = data->getBlob()->pnt.y * scale;
         TransPoint2(&p, 1, &m, insert_x, insert_y);

         extent.update(p.x,p.y);
         extent.update(p.x + data->getBlob()->width * scale,p.y + data->getBlob()->height * scale);
         //extent.assertFloat();

         extentCount++;
      }
      else
      {
         if (data->getDataType() == dataTypeInsert)
         {
            if (checkOnlyVisibleEntitiesFlag && !getInsertTypeFilter().contains(data->getInsert()->getInsertType()))
               continue;
         }
         else 
         {  
            if (checkOnlyVisibleEntitiesFlag && !getGraphicsClassFilter().contains((data->getGraphicClass()>0?data->getGraphicClass():0)))
               continue;
         }

         // figure LAYER
         if (IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // figure TYPE
         if (data->getDataType() == dataTypeInsert)
         {
            subblock = Find_Block_by_Num(data->getInsert()->getBlockNumber());
            // this only can happen if a insert refers to a non existing geometry
            if (subblock == NULL)
               continue;
         }

         // Check Layer Visible
         if (data->getDataType() != dataTypeInsert || 
               ((subblock->getFlags() & BL_TOOL     || subblock->getFlags() & BL_BLOCK_TOOL    ) && !subblock->getToolDisplay()) || 
               ((subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) &&  subblock->getShape() != T_COMPLEX))
         {
            // only check layer for non insert entities
            if (checkOnlyVisibleEntitiesFlag && !IsFloatingLayer(layer) && !get_layer_visible_extents(layer)) 
               continue;
         }
         else // block
         { 
            // adjust for LAYER = -1 
            // only for Block Apertures and Block Tools when insertLayer = -1
            if (layer == -1 &&
               ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE) ||
                (subblock->getFlags() & BL_TOOL    ) || (subblock->getFlags() & BL_BLOCK_TOOL    )) )
            {
               if (!subblock->getDataList().IsEmpty() )
               {
                  layer = subblock->getDataList().GetHead()->getLayerIndex();

                  if (checkOnlyVisibleEntitiesFlag && !get_layer_visible_extents(layer))
                     continue;
               }
            }
         }

         switch (data->getDataType())
         {
         case dataTypePoint:
            {
               Point2 p;
               p.x = data->getPoint()->x * scale;

               if (mirror & MIRROR_FLIP)
                  p.x = -p.x;

               p.y = data->getPoint()->y * scale;
               TransPoint2(&p, 1, &m, insert_x, insert_y);

               extent.update(p.x,p.y);
               //extent.assertFloat();

               extentCount++;
            }

            break;

         case dataTypePoly:
            if (data->getPolyList()->hasPoints())
            {
               CExtent polyExtent = PolyExtents(this,data->getPolyList(),scale,mirror & MIRROR_FLIP,insert_x,insert_y,&m,TRUE);

               if (polyExtent.isValid())
               {
                  extent.update(polyExtent);
                  //extent.assertFloat();

                  extentCount++;
               }
            }

            break;

         case dataTypeText:
            {
					double penWidth = getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA();
               CExtent textExtent = data->getText()->getExtent(spaceRatio,matrix,penWidth);
               extent.update(textExtent);
               extentCount++;
            }

            //if (TextExtents(this, data, &extents, scale, mirror & MIRROR_FLIP, insert_x, insert_y, rotation))
            //{
            //   extent.update(extents.left, extents.bottom);
            //   extent.update(extents.right, extents.top);
            //   //extent.assertFloat();

            //   extentCount++;
            //}

            break;  

         case dataTypeInsert:
            {
               // calculate point

					// Removed as per Case #845...  Centroids should be included in extents calculations.
               //if (data->getInsert()->getInsertType() == insertTypeCentroid)
					//	continue;

               Point2 point2;
               point2.x = data->getInsert()->getOriginX() * scale;

               if (mirror & MIRROR_FLIP)
                  point2.x = -point2.x;

               point2.y = data->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               BOOL 
                  DrawAp = FALSE, 
                  DrawSubblock = FALSE,
                  IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
                  IsAp   = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
               double 
                  _x     = point2.x,
                  _y     = point2.y,
                  _rot   = rotation,
                  _scale = scale;
               int 
                  _mirror = mirror;
               int 
                  _layer = layer;

               // TOOL
               if (IsTool)
               {
                  if (subblock->getToolDisplay()) // use another block's graphics to display this tool
                  {
                     subblock = Find_Block_by_Num(subblock->getToolBlockNumber());

                     if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                        DrawAp = TRUE;
                     else
                        DrawSubblock = TRUE;
                  }
                  else // draw generic tool
                  {
                     extent.update(_x - subblock->getToolSize() / 2,_y - subblock->getToolSize() / 2);
                     extent.update(_x + subblock->getToolSize() / 2,_y + subblock->getToolSize() / 2);
                     //extent.assertFloat();

                     extentCount++;
                  }
               } // end TOOL

               // APERTURE
               if (IsAp || DrawAp)
               {
                  // COMPLEX APERTURE
                  if (subblock->getShape() == T_COMPLEX)
                  {
                     // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)

                     if (mirror & MIRROR_FLIP)
                        _rot = rotation - subblock->getRotation() - data->getInsert()->getAngle();
                     else
                        _rot = rotation + subblock->getRotation() + data->getInsert()->getAngle();
      
                     // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                     subblock = Find_Block_by_Num((int)(subblock->getSizeA()));
                     DrawSubblock = TRUE;
                  }     
                  else // NORMAL APERTURE
                  {
                     if (subblock->getShape() == T_UNDEFINED)
                     {
                        extent.update(data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
                        //extent.assertFloat();

                        extentCount++;
                     }
                     else
                     {
                        CPolyList *polylist = ApertureToPoly(subblock, data->getInsert(), mirror);

                        if (polylist != NULL && polylist->hasPoints())
                        {
                           extents.left  = extents.bottom =  FLT_MAX;
                           extents.right = extents.top    = -FLT_MAX;

                           if (PolyExtents(this, polylist, &extents, scale, mirror & MIRROR_FLIP, insert_x, insert_y, &m, TRUE))
                           {
                              extent.update(extents.left ,extents.bottom);
                              extent.update(extents.right,extents.top   );
                              //extent.assertFloat();

                              extentCount++;
                           }

                           FreePolyList(polylist);
                        }
                     }
                  } // end NORMAL APERTURE
               } // end APERTURE
      
               // GRAPHIC BLOCK
               if (!IsAp && !IsTool) 
               {
                  DrawSubblock = TRUE;

                  if (mirror & MIRROR_FLIP)
                     _rot = rotation - data->getInsert()->getAngle();
                  else
                     _rot = rotation + data->getInsert()->getAngle();

                  _mirror = mirror ^ data->getInsert()->getMirrorFlags();
                  _scale  =  scale * data->getInsert()->getScale();
               }

               // Draw Block
               if (DrawSubblock)
               {
                  if (subblock->getDataList().IsEmpty())
                  {     
                     extent.update(_x,_y);
                     //extent.assertFloat();

                     extentCount++;
                  }
                  else
                  {
                     double xmin, xmax, ymin, ymax;
                     xmin = ymin =  FLT_MAX;
                     xmax = ymax = -FLT_MAX;

                     //if (extentCount < 1)
                     //{
                     //   int iii = 3;
                     //}

                     // use extents if at a 90 degree angle
                     if (abs(round(RadToDeg(_rot)) % 90) < 2)
                     {
                        validateBlockExtents(subblock,checkOnlyVisibleEntitiesFlag);
      
                        if (subblock->extentIsValid())
                        {
                           if (UseInsertExtents(&xmin, &xmax, &ymin, &ymax, subblock, _x, _y, _rot, _mirror, _scale))
                           {
                              extent.update(xmin,ymin);
                              extent.update(xmax,ymax);
                              //extent.assertFloat();

                              extentCount++;
                           }
                        }
                     }
                     else
                     {
                        if (block_extents(this, &xmin, &xmax, &ymin, &ymax, &(subblock->getDataList()), _x, _y, _rot, _mirror, _scale, layer,checkOnlyVisibleEntitiesFlag))
                        {
                           extent.update(xmin,ymin);
                           extent.update(xmax,ymax);
                           //extent.assertFloat();

                           extentCount++;
                        }
                     }
                  }
               } // end Draw Block

               // check visible attribs
               if (getShowAttribs() && CFontList::fontIsLoaded() && data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
               {
                  CExtent attribExtents = VisibleAttribsExtents(this,data->getAttributesRef(),&point2,_scale,_rot,_mirror);

                  if (attribExtents.isValid())
                  {
                     extent.update(attribExtents);
                     //extent.assertFloat();

                     extentCount++;
                  }
               }
            }

            break;
         }
      }

		if (debugFlag && messageFilter != NULL)
		{
			messageFilter->formatMessage("EntityNumber = %d\txMin = %0.3f\txMax = %0.3f\tyMin = %0.3f\tyMax = %0.3f\n",
					data->getEntityNumber(), extent.getXmin(), extent.getXmax(), extent.getYmin(), extent.getYmax());
		}
   }

   callLevel--;

   //if (extentCount > 0)
   //{
   //   if (! extent.isFloat())
   //   {
   //      int iii = 3;
   //   }
   //}

   return extent;
}
   
/************************************************************************
* block_extents
*/
bool block_extents(CCEtoODBDoc *doc, double *lxmin, double *lxmax, double *lymin, double *lymax, CDataList *DataList,     
      double insert_x, double insert_y, double rotation, int mirror, double scale, int insertLayer, BOOL onlyVisible)
{
   CExtent extent = doc->blockExtents(DataList,insert_x,insert_y,rotation,mirror,scale,insertLayer,onlyVisible != 0);

   bool retval = extent.isValid();

   if (retval)
   {
      *lxmin = extent.getXmin();
      *lxmax = extent.getXmax();
      *lymin = extent.getYmin();
      *lymax = extent.getYmax();
   }

   return retval;
}

/*****************************************************************************
* VisibleAttribsExtents
*/
CExtent VisibleAttribsExtents(CCEtoODBDoc *doc,CAttributes* map,Point2* pnt, 
   double block_scale, double block_rot, int block_mir)
{
   CExtent extent;
   
   for (POSITION attribPos = map->GetStartPosition();attribPos != NULL;)
   {
      Attrib* attrib;
      WORD keyword;

      map->GetNextAssoc(attribPos, keyword, attrib);

      if (!attrib->isVisible() || attrib->getValueType() != VT_STRING || !doc->get_layer_visible(attrib->getLayerIndex(), block_mir))
         continue;

      CExtent attribExtent = 
         VisibleAttribExtents(doc,attrib,pnt,block_scale,block_rot,block_mir);

      if (attribExtent.isValid())
      {
         extent.update(attribExtent);
      }
   }

   return extent;
}

/*****************************************************************************
* VisibleAttribExtents
*/
CExtent VisibleAttribExtents(CCEtoODBDoc *doc,Attrib* attrib,Point2* pnt, 
   double block_scale, double block_rot, int block_mir)
{
   CExtent extent;

   bool validCoordsFlag = (_finite(attrib->getX()) && _finite(attrib->getY()));

   if (validCoordsFlag)
   {
      Mat2x2 m;
      RotMat2(&m, block_rot);
      Point2 p;
      p.x = attrib->getX() * block_scale;

      if (block_mir & MIRROR_FLIP)
         p.x = -p.x;

      p.y = attrib->getY() * block_scale;
      TransPoint2(&p, 1, &m, pnt->x, pnt->y);

      double text_rot;

      if (block_mir & MIRROR_FLIP)
         text_rot = block_rot - attrib->getRotationRadians();
      else
         text_rot = block_rot + attrib->getRotationRadians();

      extent = TextBox(doc,attrib->getStringValue(), 
            &p, attrib->getWidth() * block_scale, attrib->getHeight() * block_scale, text_rot, block_mir, FALSE);
   }

   return extent;
}

/*******************************************************************************
* PolyExtents 
*/
bool PolyExtents(CCEtoODBDoc *doc, CPolyList *polylist, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, int use_width)
{
   bool retval = false;

   CPoly *poly;
   double width;

   extents->left  = extents->bottom =  FLT_MAX;
   extents->right = extents->top    = -FLT_MAX;
   
   for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
   {
      poly = polylist->GetNext(polyPos);

      if (use_width && poly->getWidthIndex() >= 0 && doc->getWidthTable()[poly->getWidthIndex()])
      {
         width = (doc->getWidthTable()[poly->getWidthIndex()])->getSizeA() / 2 * scale;
      }
      else
      {
         width = 0;
      }

      if (PolyPntExtents(&poly->getPntList(), extents, scale, mirror, insert_x, insert_y, m, width))
      {
         retval = true;
      }
   }

   extents->top    = dbunitround(extents->top   );
   extents->bottom = dbunitround(extents->bottom);
   extents->left   = dbunitround(extents->left  );
   extents->right  = dbunitround(extents->right );

   return retval;
}  // END PolyExtents

/******************************************************************************
* PolyExtents
*/
CExtent PolyExtents(CCEtoODBDoc *doc, CPolyList *polylist, double scale, int mirror,
						  double insert_x, double insert_y, Mat2x2 *m, int use_width)
{
   bool retval = false;
   CPoly *poly;
   double width;
   CExtent extent;

	if (polylist == NULL)
		return extent;

   for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
   {
      poly = polylist->GetNext(polyPos);

      if (use_width && poly->getWidthIndex() >= 0 && doc->getWidthTable()[poly->getWidthIndex()])
      {
         width = (doc->getWidthTable()[poly->getWidthIndex()])->getSizeA() / 2 * scale;
      }
      else
      {
         width = 0;
      }

      if (PolyPntExtents(&poly->getPntList(), extent, scale, mirror, insert_x, insert_y, m, width))
      {
         retval = true;
      }
   }

   return extent;
}  // END PolyExtents

#if !defined(EnableDcaCamCadDocLegacyCode)

/*******************************************************************************
* PolyPntExtents 
*/
bool PolyPntExtents(CPntList *pntList, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, double width)
{
	if (pntList == NULL)
		return false;

   CPnt *pnt;
   Point2 vertex, last;
   double bulge;

   POSITION pntPos = pntList->GetHeadPosition();
   bool retval = (pntPos != NULL);

   if (retval)
   {
      pnt = pntList->GetNext(pntPos);
      vertex.x = pnt->x * scale;  vertex.y = pnt->y * scale;

      if (mirror & MIRROR_FLIP) vertex.x = -vertex.x;

      TransPoint2(&vertex, 1, m, insert_x, insert_y);

      bulge = pnt->bulge;

      extents->left   = ( (vertex.x-width <= extents->left  ) ? vertex.x-width : extents->left  );
      extents->right  = ( (vertex.x+width >= extents->right ) ? vertex.x+width : extents->right );
      extents->top    = ( (vertex.y+width >= extents->top   ) ? vertex.y+width : extents->top   );
      extents->bottom = ( (vertex.y-width <= extents->bottom) ? vertex.y-width : extents->bottom);

      while (pntPos != NULL)
      {
         pnt = pntList->GetNext(pntPos);

         last.x = vertex.x;  last.y = vertex.y;

         vertex.x = pnt->x * scale;  vertex.y = pnt->y * scale;

         if (mirror & MIRROR_FLIP) vertex.x = -vertex.x;

         TransPoint2(&vertex, 1, m, insert_x, insert_y);

         extents->left   = ( (vertex.x-width <= extents->left  ) ? vertex.x - width : extents->left  );
         extents->right  = ( (vertex.x+width >= extents->right ) ? vertex.x + width : extents->right );
         extents->top    = ( (vertex.y+width >= extents->top   ) ? vertex.y + width : extents->top   );
         extents->bottom = ( (vertex.y-width <= extents->bottom) ? vertex.y - width : extents->bottom);

         if (fabs(bulge) > SMALLNUMBER) // bulge
         {
            double da, sa, r, cx, cy;
            da = atan(bulge) * 4;

            if (mirror & MIRROR_FLIP) da = -da;

            ArcPoint2Angle(last.x, last.y, vertex.x, vertex.y, da, &cx, &cy, &r, &sa);
            // if an extent point of circle is on correct side of line between last & vertex,
            // then it should be considered in the arc extents
            BOOL Undefined = fabs(last.x - vertex.x) < SMALLNUMBER;

            if (Undefined)
            {
               if ((last.y > vertex.y) ^ (da < 0)) // if Left of line between last & vertex 
               {
                  // left side of circle
                  extents->left = min(extents->left,cx - r - width);
                  // top and bottom of circle
                  if (cx < vertex.x)
                  {
                     extents->top    = max(extents->top   ,cy + r + width);
                     extents->bottom = min(extents->bottom,cy - r - width);
                  }
               }
               else // if Right of line between last & vertex 
               {
                  // right side of circle
                  extents->right = max(extents->right,cx + r + width);
                  // top and bottom of circle
                  if (cx > vertex.x)
                  {
                     extents->top    = max(extents->top   ,cy + r + width);
                     extents->bottom = min(extents->bottom,cy - r - width);
                  }
               }
            }
            else // defined slope of line between last & vertex
            {
               BOOL Below; // consider points Above or Below line between last & vertex 
               double m, b;
               m = (last.y - vertex.y) / (last.x - vertex.x); // m = dy/dx
               b = vertex.y - m * vertex.x; // y - mx = b

               Below = (last.x < vertex.x) ^ (da < 0);

               // left side of circle
               if ((cy > m * (cx-r) + b) ^ Below)
                  extents->left = min(extents->left, cx-r-width);

               // right side of circle
               if ((cy > m * (cx+r) + b) ^ Below)
                  extents->right = max(extents->right, cx+r+width);

               // top of circle
               if ((cy+r > m * cx + b) ^ Below)
                  extents->top = max(extents->top, cy+r+width);

               // bottom of circle
               if ((cy-r > m * cx + b) ^ Below)
                  extents->bottom = min(extents->bottom, cy-r-width);
            }
         }

         bulge = pnt->bulge;
      }
   }

   extents->top    = dbunitround(extents->top   );
   extents->bottom = dbunitround(extents->bottom);
   extents->left   = dbunitround(extents->left  );
   extents->right  = dbunitround(extents->right );

   return retval;
}  // END PolyPntExtents

bool PolyPntExtents(CPntList *pntList,CExtent& extent, double scale, int mirror,
      double insert_x, double insert_y, Mat2x2 *m, double width)
{
   CPnt *pnt;
   Point2 lastVertex, vertex;
   double bulge;
   int pointCount = 0;
   double xScale = ((mirror & MIRROR_FLIP) ? -scale : scale);

   for (POSITION pntPos = pntList->GetHeadPosition();pntPos != NULL;pointCount++)
   {
      pnt = pntList->GetNext(pntPos);
      vertex.x = pnt->x * xScale;  
      vertex.y = pnt->y * scale;

      TransPoint2(&vertex, 1, m, insert_x, insert_y);

      extent.update(vertex.x - width,vertex.y - width);
      extent.update(vertex.x + width,vertex.y + width);

      if (pointCount > 0 && fabs(bulge) > SMALLNUMBER) // bulge
      {
         double da, sa, r, cx, cy;
         da = atan(bulge) * 4;

         if (mirror & MIRROR_FLIP) da = -da;

         ArcPoint2Angle(lastVertex.x, lastVertex.y, vertex.x, vertex.y, da, &cx, &cy, &r, &sa);

         double startAngleDegrees,endAngleDegrees;

         if (da >= 0.)
         {
            startAngleDegrees = normalizeDegrees(radiansToDegrees(sa));
            endAngleDegrees   = startAngleDegrees + normalizeDegrees(radiansToDegrees(da));
         }
         else
         {
            startAngleDegrees = normalizeDegrees(radiansToDegrees(sa + da));
            endAngleDegrees   = startAngleDegrees - normalizeDegrees(radiansToDegrees(da));
         }

         double rCos = r + width;
         double rSin = 0.;

         for (int angleDegrees = 0;angleDegrees <= 720;angleDegrees += 90)
         {
            if (angleDegrees >= startAngleDegrees && angleDegrees <= endAngleDegrees)
            {
               extent.update(cx + rCos,cy + rSin);
            }

            double temp = rCos;
            rCos = -rSin;
            rSin = temp;
         }
      }

      lastVertex.x = vertex.x;
      lastVertex.y = vertex.y;
      bulge        = pnt->bulge;
   }

   return (pointCount > 0);
}  // END PolyPntExtents
#endif  //#if !defined(EnableDcaCamCadDocLegacyCode)

#ifdef defineOldTextExtents
/*******************************************************************************
* TextExtents 
*/
bool TextExtents(CCEtoODBDoc *doc, DataStruct *np, ExtentRect *extents, double scale, int mirror,
      double insert_x, double insert_y, double rotation)
{
   CTMatrix matrix;
   CExtent extent;
   TextStruct* textStruct = np->getText();

   matrix.scale(((mirror & MIRROR_FLIP) != 0) ? -scale : scale,scale);
   matrix.rotateRadians(rotation + textStruct->angle);
   matrix.translate(insert_x,insert_y);

   Point2 point, offset;
   double height = textStruct->height * scale;
   double width  = textStruct->width  * scale;
   double spaceratio = 0.01 * doc->Settings.TextSpaceRatio;

   Mat2x2 n;

   // Find insertion point of text string
   point.x = textStruct->pnt.x * scale;

   if (mirror & MIRROR_FLIP) point.x = -point.x;

   point.y = textStruct->pnt.y * scale;
   TransPoint2(&point, 1, m, insert_x, insert_y);

   extent.update(point.x,point.y);

   double text_rot;

   if (mirror & MIRROR_FLIP)
      text_rot = rotation - textStruct->angle;
   else
      text_rot = rotation + textStruct->angle;

   RotMat2(&n, text_rot);

   // point plus height
   offset.x = 0.0;
   offset.y = height;
   TransPoint2(&offset, 1, &n, point.x, point.y);

   extent.update(offset.x,offset.y);

   if (CFontList::fontIsLoaded())
   {
      FontStruct *fs;
      
      for (POSITION pos = FontList->GetTailPosition();pos != NULL;)
      {
         fs = FontList->GetPrev(pos);

         if (fs->fontNum == textStruct->fontnum)
         {
            break;
         }
      }

      int text_mirror;
      // text is mirrored if mirror is set or text.mirror but not if none or both
      text_mirror = textStruct->mirror ^ mirror;

      double x_offset = 0.0, y_offset = 0.0, max_x_offset = 0.0;
      unsigned char c;

      for (unsigned int i = 0; i < strlen(textStruct->getText()); i++)
      {
         c = textStruct->getText()[i]; 

         if (c == '\n')
         {
            y_offset -= height * (1.0 + spaceratio);

            if (x_offset > max_x_offset)
            {
               max_x_offset = x_offset - width * spaceratio; // remove space after last char;
            }

            x_offset = 0;

            continue;
         }

         if (c == '%')
         {
            ++i;
            c = textStruct->getText()[i];

            if (c == 'O' || c == 'U')
            {
               continue;
            }
         }

         if (c == ' ')
         {
            if (textStruct->proportional)
            {
               x_offset += 2.0 * spaceratio * width; 
            }
            else
            {
               x_offset += width * (1.0 + spaceratio);
            }

            continue;
         }

         if (!isgraph(c)) // if space or other non-printable char
            continue;

         // Find offset from insertion point for this character
         if (textStruct->proportional)
         {
            x_offset += (fs->width[c] * 2.0 + spaceratio) * width; // in .fnt file, chars are 4x8, so full width is 0.5
         }
         else // not proprtional
         {
            x_offset += width * (1.0 + spaceratio);
         }
      } // end for loop

      x_offset -= width * spaceratio; // remove space after last char

      if (x_offset > max_x_offset)
         max_x_offset = x_offset;

      offset.x = max_x_offset;

      if (text_mirror & MIRROR_FLIP) offset.x = -offset.x;
      offset.y = y_offset;

      CPoint2d textBoxLL(0.,offset.y);
      CPoint2d textBoxUR(max_x_offset,height);
      double textBoxWidth  = textBoxUR.x - textBoxLL.x;
      double textBoxHeight = textBoxUR.y - textBoxLL.y;

      switch (textStruct->getHorizontalPosition())
      {
      case horizontalPositionLeft:
         break;
      case horizontalPositionCenter:
         textBoxLL.x -= max_x_offset/2.;
         textBoxUR.x -= max_x_offset/2.;
         break;
      case horizontalPositionRight:
         textBoxLL.x -= max_x_offset;
         textBoxUR.x -= max_x_offset;
         break;
      }

      switch (textStruct->getVerticalPosition())
      {
      case verticalPositionBottom:
         break;
      case verticalPositionCenter:
         textBoxLL.y += (textBoxHeight/2.) - height;
         textBoxUR.y += (textBoxHeight/2.) - height;
         break;
      case verticalPositionTop:
         textBoxLL.y += textBoxHeight - height;
         textBoxUR.y += textBoxHeight - height;
         break;
      }

      TransPoint2(&offset, 1, &n, point.x, point.y);

      extent.update(offset.x,offset.y);

      // offset + height
      offset.x = 0.0;

      if (text_mirror & MIRROR_FLIP) offset.x = -offset.x;

      offset.y = height;
      TransPoint2(&offset, 1, &n, point.x, point.y);

      extent.update(offset.x,offset.y);
   } // end if FontLoaded

   bool retval = extent.isValid();

   if (retval)
   {
      extent.expand(.1 * height,.1 * height);

      *extents = extent;
   }

   return retval;
} // END TextExtents                                         
#endif

/******************************************************************************
* TextBox
*
*  - finds box around text
*/
CExtent TextBox(CCEtoODBDoc* doc,const char* text, 
   Point2* point, double width, double height, double rot, int mirror, BOOL proportional)
{
   TextStruct textStruct;

   textStruct.setText(text);
   textStruct.setOrigin(point->x,point->y);
   textStruct.setWidth(width);
   textStruct.setHeight(height);
   textStruct.setRotation(rot);
   textStruct.setMirrored(mirror != 0);
   textStruct.setProportionalSpacing(proportional != 0);

   CExtent extent = textStruct.getExtent(doc->getSettings().getTextSpaceRatio());

   return extent;

#ifdef defineOldTextExtents
   double spaceratio = 0.01 * doc->Settings.TextSpaceRatio;
   Mat2x2 n;
   CExtent extent;

   // point
   extent.update(point->x,point->y);

   RotMat2(&n, rot);

   // point plus height
   Point2 offset;
   offset.x = 0.0;
   offset.y = height;
   TransPoint2(&offset, 1, &n, point->x, point->y);

   extent.update(offset.x,offset.y);

   if (CFontList::fontIsLoaded())
   {
      FontStruct *fs = FontList->GetHead();

      double x_offset = 0.0, y_offset = 0.0, max_x_offset = 0.0;
      unsigned char c;

      for (unsigned int i = 0; i < strlen(text); i++)
      {
         c = text[i]; 

         if (c == '\n')
         {
            y_offset -= height * (1.0 + spaceratio);

            if (x_offset > max_x_offset)
            {
               max_x_offset = x_offset - width * spaceratio; // remove space after last char;
            }

            x_offset = 0;
            continue;
         }

         if (c == '%')
         {
            ++i;
            c = text[i];

            if (c == 'O' || c == 'U')
               continue;
         }

         if (c == ' ')
         {
            if (proportional)
               x_offset += 2.0 * spaceratio * width; 
            else
               x_offset += width * (1.0 + spaceratio);

            continue;
         }

         if (!isgraph(c)) // if space or other non-printable char
            continue;

         // Find offset from insertion point for this character
         if (proportional)
            x_offset += (fs->width[c] * 2.0 + spaceratio) * width; // in .fnt file, chars are 4x8, so full width is 0.5
         else // not proprtional
            x_offset += width * (1.0 + spaceratio);
      } // end for loop
      
      x_offset -= width * spaceratio; // remove space after last char

      if (x_offset > max_x_offset)
         max_x_offset = x_offset;

      offset.x = max_x_offset;

      if (mirror & MIRROR_FLIP) offset.x = -offset.x;

      offset.y = y_offset;
      TransPoint2(&offset, 1, &n, point->x, point->y);

      extent.update(offset.x,offset.y);

      // offset + height
      offset.x = 0.0;

      if (mirror & MIRROR_FLIP) offset.x = -offset.x;

      offset.y = height;
      TransPoint2(&offset, 1, &n, point->x, point->y);

      extent.update(offset.x,offset.y);
   } // end if FontLoaded
#endif

   return extent;
}

// END EXTENTS.CPP 