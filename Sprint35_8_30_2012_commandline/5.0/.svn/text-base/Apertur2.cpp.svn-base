// $Header: /CAMCAD/5.0/Apertur2.cpp 25    12/10/06 3:52p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/         
// APERTUR2.CPP

#include "stdafx.h"
#include "apertur2.h"
#include "CcDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* ApertureToPolyTranspose
*
*  DESCRIPTION:
*     Produces a CPolyList that represents the aperture with its offset and rotation, 
*     AND Transposed by the hierarchical scale, rotation, mirror, and x,y pos
*
*  PARAMETERS:
*     block : aperture block
*     insert : data->getInsert()
*
*     insert_x, insert_y, angle, scale, mirror : hierachical transpose
*
*  NOTES:
*     Caller is responsible for freeing memory.  Call FreePoly(CPolyList *polylist);
*/
CPolyList *ApertureToPolyTranspose(BlockStruct *block, InsertStruct *insert, 
      double insert_x, double insert_y, double angle, double scale, int mirror)
{
   return ApertureToPolyTranspose_Base(block, insert->getOriginX(), insert->getOriginY(), insert->getAngle(), 
         insert_x, insert_y, angle, scale, mirror);
}

/******************************************************************************
* ApertureToPolyTranspose_Base
*
*  DESCRIPTION:
*     Produces a CPolyList that represents the aperture with its offset and rotation, 
*     AND Transposed by the hierarchical scale, rotation, mirror, and x,y pos
*
*  PARAMETERS:
*     block : aperture block
*     x, y, rot : insert position and rotation (from data->getInsert())
*
*     insert_x, insert_y, angle, scale, mirror : hierachical transpose
*
*  NOTES:
*     Caller is responsible for freeing memory.  Call FreePoly(CPolyList *polylist);
*/
CPolyList *ApertureToPolyTranspose_Base(BlockStruct *block, double x, double y, double rot, 
      double insert_x, double insert_y, double angle, double scale, int mirror)
{
   POSITION polyPos, pntPos;
   CPolyList *polylist;
   CPoly *poly;
   CPnt *pnt;
   Point2 p;

   polylist = ApertureToPoly_Base(block, x, y, rot, mirror);

   if (!polylist)
      return NULL;

   Mat2x2 m;
   RotMat2(&m, angle);

   polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = polylist->GetNext(polyPos);
    
      pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         pnt = poly->getPntList().GetNext(pntPos);

         p.x = pnt->x * scale;
         if (mirror & MIRROR_FLIP) 
         {
            p.x = -p.x;
            pnt->bulge = -pnt->bulge;
         }
         p.y = pnt->y * scale;
         TransPoint2(&p, 1, &m, insert_x, insert_y);

         pnt->x = (DbUnit)p.x;
         pnt->y = (DbUnit)p.y;
      }
   }

   return polylist;
}

/******************************************************************************
* ApertureToPoly
*
*  DESCRIPTION:
*     Produces a CPolyList that represents the aperture with its offset and rotation.
*
*  PARAMETERS:
*     block : aperture block
*     insert : data->getInsert()
*     mirror : hiearchical mirroring
*
*  NOTES:
*     Caller is responsible for freeing memory.  Call FreePoly(CPolyList *polylist);
*/
CPolyList* ApertureToPoly(BlockStruct *block, InsertStruct *insert, int mirror)
{
   return ApertureToPoly(block, insert->getOriginX(), insert->getOriginY(), insert->getAngle(), mirror);
}

/******************************************************************************
* ApertureToPoly_Base
*
*  DESCRIPTION:
*     Produces a CPolyList that represents the aperture with its offset and rotation.
*
*  PARAMETERS:
*     block : aperture block
*     x, y, rot : insert position and rotation (from data->getInsert())
*     mirror : hiearchical mirroring
*
*  NOTES:
*     Caller is responsible for freeing memory.  Call FreePoly(CPolyList *polylist);
*/
CPolyList* ApertureToPoly_Base(CCEtoODBDoc* camCadDoc,const BlockStruct* block, double x, double y, double rot, int mirror)
{
   return ApertureToPoly(camCadDoc,block,x,y,rot,mirror);
}

CPolyList* ApertureToPoly_Base(const BlockStruct* block, double x, double y, double rot, int mirror)
{
   return ApertureToPoly(block,x,y,rot,mirror);
}

CPolyList* ApertureToPoly(const BlockStruct* block, double x, double y, double rot, int mirror)
{
   return ApertureToPoly(NULL,block,x,y,rot,mirror);
}

CPolyList* ApertureToPoly(CCEtoODBDoc* camCadDoc,const BlockStruct* block, double x, double y, double rot, int mirror)
{
   double block_rot;
   if (mirror & MIRROR_FLIP)
      block_rot = block->getRotation() - rot;
   else
      block_rot = block->getRotation() + rot;

   Mat2x2 m;
   RotMat2(&m, block_rot);

   if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
   {
      switch (block->getShape())
      {
      case T_UNDEFINED:
      case T_COMPLEX:
      case T_BLANK:
      default:
         {
            /*CPolyList *polylist = new CPolyList;
            CPoly *poly = new CPoly;
            polylist->AddTail(poly);
            CPnt *pnt = new CPnt;
            pnt->bulge = 0.0;
            Point2 p;
            p.x = block->getXoffset();
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
            return polylist;*/
            return NULL;
         }

      case T_SQUARE:
      case T_RECTANGLE:
         {
            double sizeA, sizeB;
            sizeA = sizeB = block->getSizeA() / 2;
            if (block->getShape() == T_RECTANGLE)
               sizeB = block->getSizeB() / 2;

            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + sizeA;
            p.y = block->getYoffset() + sizeB;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + sizeA;
            p.y = block->getYoffset() - sizeB;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - sizeA;
            p.y = block->getYoffset() - sizeB;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - sizeA;
            p.y = block->getYoffset() + sizeB;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + sizeA;
            p.y = block->getYoffset() + sizeB;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            return polylist;
         } // end T_SQUARE & T_RECTANGLE

      case T_ROUND:
         {
            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() + block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() - block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            return polylist;
         } // end T_ROUND

      case T_DONUT:
         if (CUseVoidsInDonuts::getUseVoidsInDonuts())
         {
            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() + block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() - block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            // Donut Hole
            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setVoid(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() + block->getSizeB()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() - block->getSizeB()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + block->getSizeB()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            return polylist;
         }
         else // thick circle
         {
            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            double radius  = (block->getSizeA() + block->getSizeB()) / 4; // ( A/2 + B/2 ) /2
            int widthIndex = -1;

            if (camCadDoc != NULL)
            {
               double width = (block->getSizeA() - block->getSizeB())/2.;
               widthIndex = camCadDoc->getDefinedWidthIndex(width);
            }

            poly = new CPoly;
            poly->setWidthIndex(widthIndex);
            poly->setClosed(true);
            poly->setFilled(false);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() + radius;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() - radius;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + radius;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            return polylist;
         } // end T_DONUT

      case T_OCTAGON:
         {
            double radius = block->getSizeA()/2;
            double halfSide = block->getSizeA()/4.83; // size of half of one side of octagon

            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - radius;
            p.y = block->getYoffset() + halfSide;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - halfSide;
            p.y = block->getYoffset() + radius;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + halfSide;
            p.y = block->getYoffset() + radius;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + radius;
            p.y = block->getYoffset() + halfSide;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + radius;
            p.y = block->getYoffset() - halfSide;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + halfSide;
            p.y = block->getYoffset() - radius;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - halfSide;
            p.y = block->getYoffset() - radius;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - radius;
            p.y = block->getYoffset() - halfSide;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - radius;
            p.y = block->getYoffset() + halfSide;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
         
            return polylist;
         } // end T_OCTAGON

      case T_TARGET:
         {
            double radius = block->getSizeA()/2;

            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            // circle
            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(false);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() + block->getSizeA()/4;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            p.x = block->getXoffset() - block->getSizeA()/4;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + block->getSizeA()/4;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
         
            // horizontal line
            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(false);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() - block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
          
            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset() + block->getSizeA()/2;
            p.y = block->getYoffset();
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
          
            // vertical line
            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(false);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset();
            p.y = block->getYoffset() + block->getSizeA()/2;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
          
            pnt = new CPnt;
            pnt->bulge = 0.0;
            p.x = block->getXoffset();
            p.y = block->getYoffset() - block->getSizeA()/2;
            TransPoint2(&p, 1, &m, x, y);
            pnt->x = (DbUnit)p.x;
            pnt->y = (DbUnit)p.y;
            poly->getPntList().AddTail(pnt);
          
            return polylist;
         } // end T_TARGET

         case T_OBLONG: // make sure it is counter clock wise.
         {
            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);
            polylist->AddTail(poly);

            if (block->getSizeA() >= block->getSizeB()) // horizontal oblong
            {
               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - (block->getSizeA()/2 - block->getSizeB()/2);
               p.y = block->getYoffset() - block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 1.0;
               p.x = block->getXoffset() + (block->getSizeA()/2 - block->getSizeB()/2);
               p.y = block->getYoffset() - block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + (block->getSizeA()/2 - block->getSizeB()/2);
               p.y = block->getYoffset() + block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 1.0;
               p.x = block->getXoffset() - (block->getSizeA()/2 - block->getSizeB()/2);
               p.y = block->getYoffset() + block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);


               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - (block->getSizeA()/2 - block->getSizeB()/2);
               p.y = block->getYoffset() - block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }
          
            else // vertical oblong
            {
               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + block->getSizeA()/2;
               p.y = block->getYoffset() - (block->getSizeB()/2 - block->getSizeA()/2);
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 1.0;
               p.x = block->getXoffset() + block->getSizeA()/2;
               p.y = block->getYoffset() + (block->getSizeB()/2 - block->getSizeA()/2);
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - block->getSizeA()/2;
               p.y = block->getYoffset() + (block->getSizeB()/2 - block->getSizeA()/2);
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 1.0;
               p.x = block->getXoffset() - block->getSizeA()/2;
               p.y = block->getYoffset() - (block->getSizeB()/2 - block->getSizeA()/2);
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);


               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + block->getSizeA()/2;
               p.y = block->getYoffset() - (block->getSizeB()/2 - block->getSizeA()/2);
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }

            return polylist;
         } // end T_OBLONG

      case T_THERMAL:
         {
            // 60° arcs, therfore 30° between them
            const DbUnit bulge = (DbUnit)0.268; // bulge = tan(da/4) ; da = 60°
            const double sin30 = 0.5; 
            const double cos30 = 0.866;

            CPolyList *polylist = new CPolyList;
            CPoly *poly;
            CPnt *pnt;
            Point2 p;

            // top arc
            if (block->getSizeB() >= block->getSizeA())
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(false);
               poly->setClosed(false);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = bulge;
               p.x = block->getXoffset() + sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }
            else
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(true);
               poly->setClosed(true);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + sin30 * block->getSizeB()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = bulge;
               p.x = block->getXoffset() + sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = -bulge;
               p.x = block->getXoffset() - sin30 * block->getSizeB()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + sin30 * block->getSizeB()/2;
               p.y = block->getYoffset() + cos30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }

            // bottom arc
            if (block->getSizeB() >= block->getSizeA())
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(false);
               poly->setClosed(false);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = -bulge;
               p.x = block->getXoffset() + sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }
            else
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(true);
               poly->setClosed(true);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + sin30 * block->getSizeB()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = -bulge;
               p.x = block->getXoffset() + sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - sin30 * block->getSizeA()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = bulge;
               p.x = block->getXoffset() - sin30 * block->getSizeB()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + sin30 * block->getSizeB()/2;
               p.y = block->getYoffset() - cos30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }

            // right arc
            if (block->getSizeB() >= block->getSizeA())
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(false);
               poly->setClosed(false);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = -bulge;
               p.x = block->getXoffset() + cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() - sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

            }
            else
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(true);
               poly->setClosed(true);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + cos30 * block->getSizeB()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = -bulge;
               p.x = block->getXoffset() + cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() - sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = bulge;
               p.x = block->getXoffset() + cos30 * block->getSizeB()/2;
               p.y = block->getYoffset() - sin30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() + cos30 * block->getSizeB()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }

            // left arc
            if (block->getSizeB() >= block->getSizeA())
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(false);
               poly->setClosed(false);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               poly->setVoid(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = bulge;
               p.x = block->getXoffset() - cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() - sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

            }
            else
            {
               poly = new CPoly;
               poly->setWidthIndex(-1);
               poly->setFilled(true);
               poly->setClosed(true);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);
               polylist->AddTail(poly);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - cos30 * block->getSizeB()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = bulge;
               p.x = block->getXoffset() - cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - cos30 * block->getSizeA()/2;
               p.y = block->getYoffset() - sin30 * block->getSizeA()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = -bulge;
               p.x = block->getXoffset() - cos30 * block->getSizeB()/2;
               p.y = block->getYoffset() - sin30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);

               pnt = new CPnt;
               pnt->bulge = 0.0;
               p.x = block->getXoffset() - cos30 * block->getSizeB()/2;
               p.y = block->getYoffset() + sin30 * block->getSizeB()/2;
               TransPoint2(&p, 1, &m, x, y);
               pnt->x = (DbUnit)p.x;
               pnt->y = (DbUnit)p.y;
               poly->getPntList().AddTail(pnt);
            }
            return polylist;
         } // end T_THERMAL
      }
   }

   return NULL;
}
