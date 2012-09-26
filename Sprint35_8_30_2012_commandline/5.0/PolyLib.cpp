// $Header: /CAMCAD/5.0/PolyLib.cpp 41    6/21/07 8:26p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "polylib.h"
#include "bb_lib.h"
#include "graph.h"
#include "xform.h"
#include <float.h>

extern "C"
{
#include <bbsdefs.h>
#include <c2vdefs.h>
#include <c2ddefs.h>
#include <c2cdefs.h>
#include <vpidefs.h>
#include <vp2defs.h>
#include <grrdefs.h>
#include <v2ddefs.h>
#include <dmldefs.h>
#include <dmdefs.h>
#include "c2ddefs.h"
#include "c2cdefs.h"
#include "t2ddefs.h"
#include "t2bdefs.h"
#include "t2cdefs.h"
#include "t2idefs.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWriteFormat& getDebugWriteFormat();

/******************************************************************************
* FreePolylist()
*/
void FreePolyList(CPolyList*& polylist) 
{  
   //if (!polylist)
   // return;

   //while (polylist->GetCount())
   // FreePoly(polylist->RemoveHead());

   delete polylist;
   polylist = NULL;
}

/******************************************************************************
* FreePoly()
*/
//void FreePoly(CPoly *poly) 
//{  
//   //if (!poly)
//   // return;
//
//   //while (poly->getPntList().GetCount())
//   // delete poly->getPntList().RemoveHead();
//
//   delete poly;
//}

/******************************************************************************
* FreePntList()
*/
void FreePntList(CPntList *pntList)
{
   //if (!pntList)
   // return;

   //while (pntList->GetCount())
   // delete pntList->RemoveHead();

   delete pntList;
}

/******************************************************************************
* CopyPoly*/
void CopyPoly(DataStruct *dest, DataStruct *src)
{
   POSITION polyPos, pntPos;
   CPoly *poly, *newPoly;
   CPnt *pnt, *newPnt;

   dest->getPolyList() = new CPolyList;

   polyPos = src->getPolyList()->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = src->getPolyList()->GetNext(polyPos);
      newPoly = new CPoly;
      newPoly->setWidthIndex(poly->getWidthIndex());
      newPoly->setFilled(poly->isFilled());
      newPoly->setClosed(poly->isClosed());
      newPoly->setVoid(poly->isVoid());
      newPoly->setThermalLine(poly->isThermalLine());
      newPoly->setFloodBoundary(poly->isThermalLine());
      newPoly->setHidden(poly->isHidden());
      newPoly->setHatchLine(poly->isHatchLine());

      dest->getPolyList()->AddTail(newPoly);

      pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         pnt = poly->getPntList().GetNext(pntPos);
         newPnt = new CPnt(*pnt);
         newPoly->getPntList().AddTail(newPnt);
      }
   }
}

/******************************************************************************
* TransformPolyList
*/
void TransformPolyList(CPolyList *polylist, double insert_x, double insert_y, double rotation, int mirror, double scale)
{
	for (POSITION polyPos = polylist->GetHeadPosition(); polyPos != NULL; )
	{
		CPoly *poly = polylist->GetNext(polyPos);
		TransformPoly(poly, insert_x, insert_y, rotation, mirror, scale);
	}
}

/******************************************************************************
* TransformPoly
*/
void TransformPoly(CPoly *poly, double insert_x, double insert_y, double rotation, int mirror, double scale)
{
	Mat2x2 m;
	RotMat2(&m, rotation);

	for (POSITION pntPos = poly->getPntList().GetHeadPosition(); pntPos != NULL; )
	{	      
		CPnt *pnt = poly->getPntList().GetNext(pntPos);

		Point2 p;
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

/******************************************************************************
* PolyIsRectangle
* The same functionality could have been added to
* PolyIsRectangle(CPoly *poly, double *llx, double *lly, double *uux, double *uuy)
* by just adding a width arg with default 0.0 to end of arg list.
* But it is nice (I think) to have the input args first, in a group, and output args
* last, in a group. Also, by doing it this way (as separate wrapper function)
* this extension that is targeted for 4.6 does not alter the original function
* as used in 4.5.
* Some time we'll probably want to refactor this to put the two together
* and just go around and change all those refernces.
*/
bool PolyIsRectangle(CPoly *poly, double lineWidth, double *llx, double *lly, double *uux, double *uuy)
{
   bool retval = PolyIsRectangle(poly, llx, lly, uux, uuy);

   if (retval && lineWidth > 0.0)
   {
      double halfWidth = lineWidth / 2.0;
      (*llx) -= halfWidth;
      (*lly) -= halfWidth;
      (*uux) += halfWidth;
      (*uuy) += halfWidth;
   }

   return retval;
}

/******************************************************************************
* PolyIsRectangle
*/
bool PolyIsRectangle(CPoly *poly, double *llx, double *lly, double *uux, double *uuy)
{
   if (poly->getPntList().GetCount() != 5)  
      return false;

   CPnt *pnt;
   Point2 points[5];
   POSITION pos = poly->getPntList().GetHeadPosition();

   pnt = poly->getPntList().GetNext(pos);
   points[0].x = pnt->x;
   points[0].y = pnt->y;
   points[0].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[1].x = pnt->x;
   points[1].y = pnt->y;
   points[1].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[2].x = pnt->x;
   points[2].y = pnt->y;
   points[2].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[3].x = pnt->x;
   points[3].y = pnt->y;
   points[3].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[4].x = pnt->x;
   points[4].y = pnt->y;
   points[4].bulge = pnt->bulge;

   if (!poly->isClosed()) // maybe close flag is not set !
   {
      if (fabs(points[0].x - points[4].x) > SMALLNUMBER)
         return false;

      if (fabs(points[0].y - points[4].y) > SMALLNUMBER)
         return false;
   }

   if (fabs(points[0].bulge) > SMALLNUMBER)  
      return false;

   if (fabs(points[1].bulge) > SMALLNUMBER)
      return false;

   if (fabs(points[2].bulge) > SMALLNUMBER)
      return false;

   if (fabs(points[3].bulge) > SMALLNUMBER)
      return false;

   // make a bounding box
   CPnt ll, tr;

   tr.x = ll.x = (DbUnit)points[0].x;
   tr.y = ll.y = (DbUnit)points[0].y;
   tr.bulge = ll.bulge = 0;

	int i=0;
   for (i=0;i<4;i++)
   {
      if (ll.x > points[i].x)
         ll.x = (DbUnit)points[i].x;

      if (ll.y > points[i].y)
         ll.y = (DbUnit)points[i].y;

      if (tr.x < points[i].x)
         tr.x = (DbUnit)points[i].x;

      if (tr.y < points[i].y)
         tr.y = (DbUnit)points[i].y;
   }

   // check that every point hits a corner of the bounding box
   int corner[4];
   corner[0] = corner[1] = corner[2] = corner[3] = 0;

   for (i=0;i<4;i++)
   {
      if (fabs(ll.x - points[i].x) < SMALLNUMBER && fabs(ll.y - points[i].y) < SMALLNUMBER)
         corner[i] = true;

      if (fabs(tr.x - points[i].x) < SMALLNUMBER && fabs(ll.y - points[i].y) < SMALLNUMBER)
         corner[i] = true;

      if (fabs(tr.x - points[i].x) < SMALLNUMBER && fabs(tr.y - points[i].y) < SMALLNUMBER)
         corner[i] = true;

      if (fabs(ll.x - points[i].x) < SMALLNUMBER && fabs(tr.y - points[i].y) < SMALLNUMBER)
         corner[i] = true;
   }

   // if all 4 corners are hit, thats a rectangle
   if (corner[0] && corner[1] && corner[2] && corner[3])
   {
      *llx = ll.x;
      *lly = ll.y;
      *uux = tr.x;
      *uuy = tr.y;

      return true;
   }

   return false;
}

/******************************************************************************
* PolyIsRectangleAngle
*/
BOOL PolyIsRectangleAngle(CPoly *poly, double linewidth, double accuracy, 
                          double *left, double *bottom, double *right, double *top, double *rotation)
{
   if (poly->getPntList().GetCount() != 5)  
      return FALSE;

   CPnt *pnt;
   Point2 points[5];
   POSITION pos = poly->getPntList().GetHeadPosition();

   pnt = poly->getPntList().GetNext(pos);
   points[0].x = pnt->x;
   points[0].y = pnt->y;
   points[0].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[1].x = pnt->x;
   points[1].y = pnt->y;
   points[1].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[2].x = pnt->x;
   points[2].y = pnt->y;
   points[2].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[3].x = pnt->x;
   points[3].y = pnt->y;
   points[3].bulge = pnt->bulge;

   pnt = poly->getPntList().GetNext(pos);
   points[4].x = pnt->x;
   points[4].y = pnt->y;
   points[4].bulge = pnt->bulge;

   if (!poly->isClosed()) // maybe close flag is not set !
   {
      if (fabs(points[0].x - points[4].x) > SMALLNUMBER)
         return FALSE;
      if (fabs(points[0].y - points[4].y) > SMALLNUMBER)
         return FALSE;
   }

   if (fabs(points[0].bulge) > SMALLNUMBER)
      return FALSE;
   if (fabs(points[1].bulge) > SMALLNUMBER)
      return FALSE;
   if (fabs(points[2].bulge) > SMALLNUMBER)
      return FALSE;
   if (fabs(points[3].bulge) > SMALLNUMBER)
      return FALSE;

   // make a bounding box
   CPnt ll, tr;

   tr.x = ll.x = (DbUnit)points[0].x;
   tr.y = ll.y = (DbUnit)points[0].y;
   tr.bulge = ll.bulge = 0;

	int i=0;
   for (i=0; i<4; i++)
   {
      if (ll.x > points[i].x)
         ll.x = (DbUnit)points[i].x;
      if (ll.y > points[i].y)
         ll.y = (DbUnit)points[i].y;
      if (tr.x < points[i].x)
         tr.x = (DbUnit)points[i].x;
      if (tr.y < points[i].y)
         tr.y = (DbUnit)points[i].y;
   }

   // here now update an angle
   double angle1 = atan2((points[1].y - points[0].y), (points[1].x - points[0].x));
   double angle2 = atan2((points[0].y - points[1].y), (points[0].x - points[1].x));
   *rotation = min(angle1, angle2);

   for (i=0;i<4;i++)
   {
      double xrot, yrot;
      Rotate(points[i].x, points[i].y, -RadToDeg(*rotation), &xrot, &yrot);   // need to unrotate
      points[i].x = xrot;
      points[i].y = yrot;
   }

   tr.x = ll.x = (DbUnit)points[0].x;
   tr.y = ll.y = (DbUnit)points[0].y;
   tr.bulge = ll.bulge = 0;

   // now do the rest
   for (i=0; i<4; i++)
   {
      if (ll.x > points[i].x) 
         ll.x = (DbUnit)points[i].x;
      if (ll.y > points[i].y) 
         ll.y = (DbUnit)points[i].y;
      if (tr.x < points[i].x)
         tr.x = (DbUnit)points[i].x;
      if (tr.y < points[i].y)
         tr.y = (DbUnit)points[i].y;
   }

   // check that every point hits a corner of the bounding box
   int corner[4];
   corner[0] = corner[1] = corner[2] = corner[3] = 0;
   for (i=0;i<4;i++)
   {
      // can not use SMALLNUMBER, the arc rotate makes it not accurate
      if (fabs(ll.x - points[i].x) < accuracy && fabs(ll.y - points[i].y) < accuracy)  
         corner[i] = TRUE;
      if (fabs(tr.x - points[i].x) < accuracy && fabs(ll.y - points[i].y) < accuracy)
         corner[i] = TRUE;
      if (fabs(tr.x - points[i].x) < accuracy && fabs(tr.y - points[i].y) < accuracy)
         corner[i] = TRUE;
      if (fabs(ll.x - points[i].x) < accuracy && fabs(tr.y - points[i].y) < accuracy)
         corner[i] = TRUE;
   }

   // if all 4 corners are hit, thats a rectangle
   if (corner[0] && corner[1] && corner[2] && corner[3])
   {
      *left = ll.x - linewidth/2;
      *bottom = ll.y - linewidth/2;
      *right = tr.x + linewidth/2;
      *top = tr.y + linewidth/2;
      return TRUE;
   }

   return FALSE;
}

/***************************************************
* PolyIsCircle
*/
bool PolyIsCircle(const CPoly *poly, double *cx, double *cy, double *radius)
{
	return PntListIsCircle(&poly->getPntList(), cx, cy, radius);
}

bool PntListIsCircle(const CPntList* pntList,double* pxCenter,double* pyCenter,double* pRadius)
{
   double xCenterLocal,yCenterLocal,radiusLocal;

   double& xCenter = ((pxCenter == NULL) ? xCenterLocal : *pxCenter);
   double& yCenter = ((pyCenter == NULL) ? yCenterLocal : *pyCenter);
   double& radius  = ((pRadius  == NULL) ? radiusLocal  : *pRadius );

   return pntList->isSimpleCircle(xCenter,yCenter,radius);
}

/***************************************************
* PolyIsOblong
*/
BOOL PolyIsOblong(CPoly *poly, double *sizeA, double *sizeB, double *cx, double *cy)
{
   if (poly->getPntList().GetCount() != 5)
      return FALSE;

   CPnt *a, *b, *c, *d, *e;
   POSITION pos = poly->getPntList().GetHeadPosition();
   a = poly->getPntList().GetNext(pos);
   b = poly->getPntList().GetNext(pos);
   c = poly->getPntList().GetNext(pos);
   d = poly->getPntList().GetNext(pos);
   e = poly->getPntList().GetNext(pos);
   
   // closed
   if (fabs(a->x - e->x) > SMALLNUMBER)
      return FALSE;
   if (fabs(a->y - e->y) > SMALLNUMBER)
      return FALSE;

   CPnt *b1, *l1, *b2, *l2;

   // first point has bulge
   if (fabs(a->bulge) > BULGE_THRESHOLD)
   {
      b1 = a;
      l1 = b;
      b2 = c;
      l2 = d;
   }
   else // first point has no bulge
   {
      b1 = b;
      l1 = c;
      b2 = d;
      l2 = e;
   }

   // must have 2 bulges
   if (fabs(b1->bulge) < BULGE_THRESHOLD)
      return FALSE;

   if (fabs(b2->bulge) < BULGE_THRESHOLD)
      return FALSE;

   // 180
   if (fabs(b1->bulge) - 1 > BULGE_THRESHOLD)
      return FALSE;

   // 0 
   if (fabs(l1->bulge) > BULGE_THRESHOLD)
      return FALSE;

   // 180
   if (fabs(b2->bulge) - 1 > BULGE_THRESHOLD)
      return FALSE;

   // 0
   if (fabs(l2->bulge) > BULGE_THRESHOLD)
      return FALSE;

   // both +180 or both -180
   if (fabs(b1->bulge - b2->bulge) > BULGE_THRESHOLD)
      return FALSE;


   BOOL Vertical = fabs(l1->x - b2->x) < SMALLNUMBER;

   if (Vertical)
   {
      if (fabs(b2->y - l2->y) > SMALLNUMBER)
         return FALSE;

      if (fabs(l2->x - b1->x) > SMALLNUMBER)
         return FALSE;

      if (fabs(b1->y - l1->y) > SMALLNUMBER)
         return FALSE;
   }
   else
   {
      if (fabs(l1->x - b1->x) > SMALLNUMBER)
         return FALSE;

      if (fabs(l1->y - b2->y) > SMALLNUMBER)
         return FALSE;

      if (fabs(b2->x - l2->x) > SMALLNUMBER)
         return FALSE;

      if (fabs(l2->y - b1->y) > SMALLNUMBER)
         return FALSE;
   }


   // clockwise and bulges
   if (IsClockwise(&poly->getPntList()))
   {
      if (b1->bulge > 0)
         return FALSE;

      if (b2->bulge > 0)
         return FALSE;
   }
   else
   {
      if (b1->bulge < 0)
         return FALSE;

      if (b2->bulge < 0)
         return FALSE;
   }


   if (sizeA && sizeB)
   {
      *sizeA = fabs(b1->x - b2->x);
      *sizeB = fabs(b1->y - b2->y);
      *cx = (b1->x + b2->x) / 2;
      *cy = (b1->y + b2->y) / 2;

      if (Vertical)
      {
         *sizeB += *sizeA;
      }
      else
      {
         *sizeA += *sizeB;
      }
   }

   return TRUE;
}

/******************************************************************************
* ClosePoly()
*/
void ClosePoly(CPoly *poly)
{
   CPnt *head,*tail, *p;
   head = poly->getPntList().GetHead();
   tail = poly->getPntList().GetTail();
   if (fabs(head->x - tail->x) > SMALLNUMBER 
      || fabs(head->y - tail->y) > SMALLNUMBER)
   {
      tail->bulge = (DbUnit)0.0;
      p = new CPnt;
      p->x = head->x; p->y = head->y; p->bulge = (DbUnit)0.0;
      poly->getPntList().AddTail(p);
   }
   poly->setClosed(true);
}

/******************************************************************************
* Is_Pnt_Inside_Poly()
*/
int Is_Pnt_Inside_Poly(CPoly *Poly, CPnt *p,PageUnitsTag pageUnits,double tolerance)                          
{
   return Is_Pnt_Inside_PntList(&Poly->getPntList(), p,pageUnits,tolerance);
}

/*******************************************************
* int Is_Pnt_Inside_PntList(CPntList *list, CPnt *p)
*
* Description:
*              --- checks if point on poly, returns -1
*              --- sums angles of triangles to determine if inside poly  
*              --- assumes all polylines do not have bulges, i.e. considers
*                  all polylines with bulges to be straight line segments.
*
* Parameters:
*              --- poly -> one poly
*              --- p ->  pnt which need
*                           to check inside or outside or on poly
* Notes:
*              --- point p with every two points (each line) become
*                  to a triangle and the number of triangles is equar to
*                  the number of lines.
*              --- Angle is the angle of 
*                  (last_point)-->(p)-->(current_point)
*              --- Sum of all Angles is always equar to 0
*                  when the point p is outside 
*
* Return:      --- 1
*                        if point p is inside 
*
*              --- 0 
*                        if point p is outside 
*
*              --- -1 
*                        if point p on poly
*
*********************************************************/     
int Is_Pnt_Inside_PntList(const CPntList *list, CPnt *p,PageUnitsTag pageUnits, double tolerance)                          
{
   return list->isPointInside(p->x,p->y,pageUnits,tolerance);
}

int Is_Pnt_Inside_PntList(const CPntList* list, double xPoint, double yPoint,PageUnitsTag pageUnits, double tolerance)                            
{
   return list->isPointInside(xPoint,yPoint,pageUnits,tolerance);
}

/******************************************************************************
*  CleanPoly()
   dist is the smallest Delta allowed
*/
void CleanPoly(CPoly *poly, double dist)
{
   CleanPntList(&poly->getPntList(), dist);
}

/******************************************************************************
* ReversePoly
*  change the poly to backword 
*                 before change poly is from head to tail
*                 after change the new poly is from tail to head
*                 the bulge change to match the old poly
*/
void ReversePoly(CPntList *pntList)
{
   double bulge = 0.0;
   
   POSITION pntPos = pntList->GetHeadPosition();
   while (pntPos)
   {
      POSITION tempPos = pntPos;
      CPnt *pnt = pntList->GetNext(pntPos);
      double temp_bulge = pnt->bulge;
      pnt->bulge = (DbUnit)(-bulge);
      bulge = temp_bulge;
      pntList->AddHead(pnt);
      pntList->RemoveAt(tempPos);
   }
}


/******************************************************************************
* PolySelfIntersects
*/
BOOL PolySelfIntersects(CPntList *pntList)
{
/* C2_CURVE curve = CreatePCurve(pntList);

   DML_LIST intersectionList = dml_create_list();

   int result = c2c_self_intersect(curve, intersectionList);

   c2d_free_curve(curve);
   dml_destroy_list(intersectionList, dmm_free);//(PF_ACTION)t2i_free_int_rec);

   return result;
*/

   T2_REGION rgn = BB_CreateRegion(pntList);

   DML_LIST intersectionlist = dml_create_list();
   T2_LOOP loop = (T2_LOOP)DML_RECORD(DML_FIRST(T2_REGION_LOOP_LIST(rgn)));
   int result = t2i_intersect_loop_self(loop, intersectionlist);

/*if (result) DEBUG only !
{
   C2_INT_REC intersect = (C2_INT_REC)DML_RECORD(DML_FIRST(intersectionlist));
   double x = PT2_X(C2_INT_REC_PT(intersect));
   double y = PT2_Y(C2_INT_REC_PT(intersect));
   int i=0;
}*/


/* if (result)
   {
      CPnt *head = pntList->GetHead();

      DML_ITEM item;
      DML_WALK_LIST(intersectionlist, item)
      {
         C2_INT_REC intersect = (C2_INT_REC)DML_RECORD(item);

         double x, y;
         x = PT2_X(C2_INT_REC_PT(intersect));
         y = PT2_Y(C2_INT_REC_PT(intersect));

         if (fabs(head->x - x) < SMALLNUMBER && fabs(head->y - y) < SMALLNUMBER)
         {
            result--;   
            break;
         }
      }
   }
*/
   dml_destroy_list(intersectionlist, (PF_ACTION)t2i_free_int_rec);

   t2d_free_region(rgn);

   return result;
}

/******************************************************************************
* FixFreeVoids
*/
int FixFreeVoids(CCEtoODBDoc *doc, CDataList *datalist)
{
   int failedCount = 0;

   POSITION voidPos = datalist->GetHeadPosition();
   while (voidPos)
   {
      DataStruct *voidData = datalist->GetNext(voidPos);

      if (voidData->getDataType() != dataTypePoly)
         continue;

      CPoly *voidPoly = voidData->getPolyList()->GetHead();
      if (!voidPoly->isVoid())
         continue;

      // voiddata netname

      T2_REGION voidRegion = BB_CreateRegion(&voidPoly->getPntList());

      // find a parent
      DataStruct *goodParent = NULL;
      POSITION parentPos = datalist->GetHeadPosition();
      while (parentPos)
      {
         DataStruct *parentData = datalist->GetNext(parentPos);

         if (parentData == voidData)
            continue;

         if (parentData->getDataType() != dataTypePoly)
            continue;

         if (parentData->getLayerIndex() != voidData->getLayerIndex())
            continue;

         // compare netnames

         CPoly *parentPoly = parentData->getPolyList()->GetHead();
         
         T2_REGION parentRegion = BB_CreateRegion(&parentPoly->getPntList());

         DML_LIST results = t2b_common_rr(voidRegion, parentRegion, NULL);

         if (dml_length(results))
         {
            CString buf;
            if (goodParent)
            {
               buf.Format("free void #%ld", voidData->getEntityNumber());
               ErrorMessage(buf, "Found multiple parents");
               goodParent = NULL;
               break;
            }

            buf.Format("Connecting #%ld to #%ld", voidData->getEntityNumber(), parentData->getEntityNumber());
            ErrorMessage(buf, "Found a parent");
            goodParent = parentData;
         }

         dml_destroy_list(results, BB_free_region);

         t2d_free_region(parentRegion);
      }

      t2d_free_region(voidRegion);

      if (goodParent)
      {
         LinkPolyDatas(doc, datalist, goodParent, voidData);
      }
      else
      {
         failedCount++;
      }
   }     

   return failedCount;
}

/******************************************************************************
* LinkPolyDatas
*/
void LinkPolyDatas(CCEtoODBDoc *doc, CDataList *datalist, DataStruct *parentData, DataStruct *childData)
{
   // move child polys over to parent polystruct
   parentData->getPolyList()->takeData(*(childData->getPolyList()));

   //while (childData->getPolyList()->GetCount())
   //{
   //   parentData->getPolyList()->AddTail(childData->getPolyList()->GetHead());
   //   childData->getPolyList()->RemoveHead();
   //}

   // attributes

   // clean memory
   RemoveOneEntityFromDataList(doc, datalist, childData);
}

/******************************************************************************
* GetTransform
   return TRUE for transform validated on 3 points
   return FALSE for validated, but found wrong due to bad input values
*/
int CalcTransform(CPnt *referencePnt, 
      CPnt *origA, CPnt *origB, CPnt *origC,
      CPnt *newA, CPnt *newB, CPnt *newC,
      double *offsetX, double *offsetY, double *scale, double *rotation, BOOL *mirror)
{

	// MIRROR

	// Since there are always three points in origList and newList use the function ArcPoint3()
	// to find the delta angle and then use that to find out if clockwise or counter clockwise 
	// is betten than the function IsClockwise().  Becasue the function IsClockwise() will find
	// xmin, xmax, ymin, ymax points in the list. If those points happens to be only two set of 
	// X&Y coordinate, then it will ended up with a straight line and can't tell if the list is 
	// clockwise or counter clockwise
   double startAngle;
   double deltaAngle;
   double centerX;
   double centerY;
   double radius;

   ArcPoint3(origA->x, origA->y, origB->x, origB->y, origC->x, origC->y, &centerX, &centerY, &radius, &startAngle, &deltaAngle);
	bool origClockwise = (deltaAngle>0)?true:false;

   ArcPoint3(newA->x, newA->y, newB->x, newB->y, newC->x, newC->y, &centerX, &centerY, &radius, &startAngle, &deltaAngle);
	bool newClockwise = (deltaAngle>0)?true:false;

	*mirror = (origClockwise != newClockwise)?TRUE:FALSE;


   // SCALE
   double origDist, newDist;
   origDist = sqrt( (origB->x - origA->x) * (origB->x - origA->x) + 
         (origB->y - origA->y) * (origB->y - origA->y) );
   newDist = sqrt( (newB->x - newA->x) * (newB->x - newA->x) + 
         (newB->y - newA->y) * (newB->y - newA->y) );
   *scale = newDist / origDist;


   // ROTATION
   double origAngle, newAngle;
   if (*mirror)
      origAngle = atan2(origB->y - origA->y, origA->x - origB->x);
   else
      origAngle = atan2(origB->y - origA->y, origB->x - origA->x);
   newAngle = atan2(newB->y - newA->y, newB->x - newA->x);
   *rotation = newAngle - origAngle;


   // OFFSET
   DTransform xform (referencePnt->x, referencePnt->y, *scale, *rotation, *mirror);
   Point2 point;
   point.x = origA->x;
   point.y = origA->y;
   xform.TransformPoint(&point);
   *offsetX = newA->x - point.x;
   *offsetY = newA->y - point.y;

   return 1;
}

/******************************************************************************
* VectorizePoly
*/
void VectorizePoly(CPoly *poly,PageUnitsTag pageUnits)
{
	VectorizePntList(&poly->getPntList(),pageUnits);
}

/******************************************************************************
* VectorizePntList
*/
void VectorizePntList(CPntList* pntList,PageUnitsTag pageUnits)
{
   pntList->vectorize(pageUnits);
}