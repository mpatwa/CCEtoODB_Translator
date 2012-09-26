
/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccview.h"
#include "mainfrm.h"
#include "graph.h"
#include "polylib.h"
#include "CCEtoODB.h"

static int joinMode;
static DataStruct *joinPolyData;
static CPoly *joinPoly;
static int joinPolyFile;
static double headX, headY, tailX, tailY;
static BOOL AutoJoinRun = FALSE;
static UndoItemStruct undoModified;
static int FirstAuto;
static UndoItemList *undoItemList = NULL;

void ResetButtons();
static int Find_Merge_Pnts(CCEtoODBDoc *doc, CPnt *p1, CPnt *p2, CPnt*p3, CPnt *p4, CPnt *merge_p1, CPnt *merge_p2);
void ArrangePolysForMerge(CPoly *poly1, CPoly *poly2);

/******************************************************************************
* JoinMode
*/
void JoinMode(CCEtoODBView *view, CursorMode newMode) 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   if (view->zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (view->cursorMode == newMode)
   {
      //view->HideSearchCursor();
      view->cursorMode = Search;
      //view->ShowSearchCursor();
      return;
   }

   joinMode = newMode;

   //view->HideSearchCursor();
   view->cursorMode = newMode;
   //view->ShowSearchCursor();

   joinPolyData = NULL;
}

void CCEtoODBView::OnJoinSnap() 
{  JoinMode(this, Join_Snap); }

void CCEtoODBView::OnJoinExtend() 
{  JoinMode(this, Join_Extend); }

void CCEtoODBView::OnJoinChamfer() 
{  JoinMode(this, Join_Chamfer); }

void CCEtoODBView::OnJoinFillet() 
{  JoinMode(this, Join_Fillet); }

/******************************************************************************
* JoinPolyline
*
*  DESCRIPTION :
*     - called when a polyline is selcted and in Join Poly mode
*/
void JoinPolyline(SelectStruct *s, CPoly *selPoly, CCEtoODBDoc *doc)
{
   if (!selPoly) // in case have no selPoly
      return;

   if (joinPolyData == NULL) // if first entity selected
   {
      joinPolyData = s->getData();
      joinPoly = selPoly;
      joinPolyFile = s->filenum;

      undoModified.DataList = s->getParentDataList();
      undoModified.num = s->getData()->getEntityNumber();
      undoModified.type = Edit_Modify;
      return; 
   } 
   
   else // adding to joinPolyData 
   {
      if (joinPolyData->getPolyList()->GetCount() > 1)
      {
         ErrorMessage("Illegal Selection.  PolyStruct has multiple polys", "Ignoring Selection");
         return;
      }

      if (joinPolyFile != s->filenum)
      {
         ErrorMessage("Illegal Selection.  Polylines in different files.", "Ignoring Selection");
         return;
      }

      if (joinPolyData->getLayerIndex() != s->getData()->getLayerIndex())
      {
         ErrorMessage("Illegal Selection.  Polylines on differnt layers.", "Ignoring Selection");
         return;
      }

      // illegal if same poly
      if (joinPoly == selPoly)
      {
         ErrorMessage("Can not join a poly to itself", "Ignoring Selection");
         return;
      }


      POSITION pos;
      CPnt *joinPoly_p1, *joinPoly_p2, *selPoly_p1, *selPoly_p2, merge_p1, merge_p2;

      // make polys so that add Head of selPoly to Tail of joinPoly
      ArrangePolysForMerge(joinPoly, selPoly);

      // get last segment from joinPoly (joinPoly_p1 --> joinPoly_p2)      
      pos = joinPoly->getPntList().GetTailPosition();
      joinPoly_p2 = joinPoly->getPntList().GetPrev(pos);
      joinPoly_p1 = joinPoly->getPntList().GetAt(pos);
      
      // get first segment from selPoly (selPoly_p1 --> selPoly_p2)     
      pos = selPoly->getPntList().GetHeadPosition();
      selPoly_p1 = selPoly->getPntList().GetNext(pos);
      selPoly_p2 = selPoly->getPntList().GetAt(pos);

      // find the extra pnts need to add for the merge
      int method = Find_Merge_Pnts(doc, joinPoly_p1, joinPoly_p2, selPoly_p1, selPoly_p2, &merge_p1, &merge_p2);

      // if can't do anything
      if (method == -1)
         return;

      // undo
      doc->PrepareThisUndoLevel();
      UndoItemList *undoItemList = doc->undo[doc->undo_level] = new UndoItemList;
      doc->undo_level = doc->redo_max = (doc->undo_level + 1) % MAX_UNDO_LEVEL;
      doc->ClearNextUndoLevel();

      joinPolyData->setSelected(false);
      joinPolyData->setMarked(false);
      undoModified.data = doc->getCamCadData().getNewDataStruct(*joinPolyData,true,joinPolyData->getEntityNumber());
      //undoModified.data->setEntityNumber(joinPolyData->getEntityNumber());
      UndoItemStruct *undoModified2 = new UndoItemStruct;
      memcpy(undoModified2, &undoModified, sizeof(UndoItemStruct));
      undoItemList->AddTail(undoModified2);


      UndoItemStruct *undoDeleted = new UndoItemStruct;
      undoItemList->AddTail(undoDeleted);
      undoDeleted->data = s->getData();
      undoDeleted->type = Edit_Delete;
      undoDeleted->DataList = s->getParentDataList();
      POSITION dataPos = s->getParentDataList()->Find(s->getData());
      POSITION tempPos = dataPos;
      s->getParentDataList()->GetPrev(tempPos);
      if (tempPos)
         undoDeleted->num = s->getParentDataList()->GetAt(tempPos)->getEntityNumber();
      else
         undoDeleted->num = 0;
      s->getParentDataList()->RemoveAt(dataPos);
      s->getData()->setSelected(false);
      s->getData()->setMarked(false);


      switch (method)
      {
         case 0: // snap
         {
            CPnt *newPnt = new CPnt(*(selPoly_p1));
            joinPoly->getPntList().AddTail(newPnt);
         }
         break;

         case 1: // extend
         {
            joinPoly_p2->x = merge_p1.x;
            joinPoly_p2->y = merge_p1.y;
            joinPoly_p2->bulge = merge_p1.bulge;
         }
         break;

         case 2: // chamfer or fillet
         {
            joinPoly_p2->x = merge_p1.x;
            joinPoly_p2->y = merge_p1.y;
            joinPoly_p2->bulge = merge_p1.bulge;
            
            CPnt *p = new CPnt;
            p->x = merge_p2.x;
            p->y = merge_p2.y;
            p->bulge = merge_p2.bulge;
            joinPoly->getPntList().AddTail(p);
         }
         break;
      }

      // add the rest of the points from selPoly to Tail of joinPoly
      pos = selPoly->getPntList().GetHeadPosition();
      if (pos) selPoly->getPntList().GetNext(pos); // skip first point
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(selPoly->getPntList().GetNext(pos)));
         joinPoly->getPntList().AddTail(newPnt);
      }
   }

   
   (doc->SelectList.GetHead())->setData(joinPolyData);
   s->setData(joinPolyData);
   s->getData()->setSelected(true);
   doc->DrawEntity(s, 1, TRUE);
}

/******************************************************************************
* ArrangePolysForMerge
*
*  DESCRIPTION:
*     - Reveses order of points in poly if necessary to guarantee that 
*        connecting poly2's head to poly1's tail is correct
*/
void ArrangePolysForMerge(CPoly *poly1, CPoly *poly2)
{
   CPnt *head_1, *tail_1, *head_2, *tail_2; // head_1 is poly1's head
   double t1_h2, h1_h2, t1_t2, h1_t2; // t1_h2 is the distance between the Tail of poly1 and the Head of poly2

   head_1 = poly1->getPntList().GetHead();
   tail_1 = poly1->getPntList().GetTail();
   head_2 = poly2->getPntList().GetHead();
   tail_2 = poly2->getPntList().GetTail();
   t1_h2 = sqrt(pow(tail_1->x - head_2->x, 2) + pow(tail_1->y - head_2->y, 2));
   h1_h2 = sqrt(pow(head_1->x - head_2->x, 2) + pow(head_1->y - head_2->y, 2));
   t1_t2 = sqrt(pow(tail_1->x - tail_2->x, 2) + pow(tail_1->y - tail_2->y, 2));
   h1_t2 = sqrt(pow(tail_2->x - head_1->x, 2) + pow(tail_2->y - head_1->y, 2));

   if (min(h1_h2, h1_t2) >= min(t1_h2, t1_t2)) // poly1 is ok
   {
      if (t1_h2 > t1_t2)
         ReversePoly(&poly2->getPntList());
   }
   else // reverse poly1
   {
      ReversePoly(&poly1->getPntList());

      if (h1_h2 > h1_t2) 
         ReversePoly(&poly2->getPntList());
   }
}

/******************************************************************************
* Find_Merge_Pnts
*  
*  DESCRIPTION:
*     - Find out how many points need to be added
*     - Figure out the points
*     - Test Method and Circumstances of Join
*        (if last segment of first poly or first segment of second poly has bulge, do the Snap)
*        (if can't do Extend, Chamfer, Fillet, do the Snap)
*
*  RETURNS:
*     - -1 : Do Nothing
*     -  0 : Don't need to add any points (snap)
*     -  1 : Add 1 point (extend)
*     -  2 : Add 2 points (chamfer or fillet)
*/
int Find_Merge_Pnts(CCEtoODBDoc *doc, CPnt *poly1_p1, CPnt *poly1_p2, CPnt* poly2_p1, CPnt *poly2_p2, CPnt *merge_p1, CPnt *merge_p2)
{  
   // if joinPoly's last segment has bulge or selPoly's first segment has bulge, only do snap
   if (joinMode != Join_Snap && (fabs(poly1_p1->bulge) > SMALLNUMBER || fabs(poly2_p1->bulge) > SMALLNUMBER) )
   {
      if (ErrorMessage("There is a bulge.  Snap is the only allowed joining method.\nWould you like to just snap instead?", 
            "Warning", MB_YESNO) == IDYES)
         return 0;
      return -1;
   }

   switch (joinMode)
   {
   case Join_Extend:
   {
      Point2 p_1,p_2,p_3,p_4,inter;//two lines end pnts and intersection
      p_1.x = poly1_p1->x; p_1.y = poly1_p1->y; p_1.bulge = poly1_p1->bulge;
      p_2.x = poly1_p2->x; p_2.y = poly1_p2->y; p_2.bulge = poly1_p2->bulge;
      p_3.x = poly2_p1->x; p_3.y = poly2_p1->y; p_3.bulge = poly2_p1->bulge;
      p_4.x = poly2_p2->x; p_4.y = poly2_p2->y; p_4.bulge = poly2_p2->bulge;
      if (!IntersectInfiniteLine2(&p_1, &p_2, &p_3, &p_4,&inter)) // p1p2 - p3p4
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }

      if (!Point2InSeg(&p_1, &p_2, &inter) && 
         !Point2InSeg(&p_3, &p_4, &inter) &&
         Point2InSeg(&p_1, &inter, &p_2) &&
         Point2InSeg(&p_4, &inter, &p_3))
      {
         merge_p1->x = (DbUnit)inter.x; merge_p1->y = (DbUnit)inter.y; 
         merge_p1->bulge = (DbUnit)0.0;
         return 1;
      }
      else if (Point2InSeg(&p_1, &p_2, &inter) &&
             Point2InSeg(&p_3, &p_4, &inter))
      {
         merge_p1->x = (DbUnit)inter.x; merge_p1->y = (DbUnit)inter.y; 
         merge_p1->bulge = (DbUnit)0.0;
         return 1;
      }
      else if (!Point2InSeg(&p_1, &p_2, &inter) &&
             Point2InSeg(&p_3, &p_4, &inter) &&
             Point2InSeg(&p_1, &inter, &p_2))
      {
         merge_p1->x = (DbUnit)inter.x; merge_p1->y = (DbUnit)inter.y; 
         merge_p1->bulge = (DbUnit)0.0;
         return 1;
      }
      else if (Point2InSeg(&p_1, &p_2, &inter) &&
             !Point2InSeg(&p_3, &p_4, &inter) &&
             Point2InSeg(&p_4, &inter, &p_3))
      {
         merge_p1->x = (DbUnit)inter.x; merge_p1->y = (DbUnit)inter.y; 
         merge_p1->bulge = (DbUnit)0.0;
         return 1;
      }
      else
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
   }
   break;

   case Join_Chamfer:
   {
      Point2 p_1,p_2,p_3,p_4,inter,inter1,inter2;
      double angle1,angle2,da,dis_inter,dis1,dis2,dis3,dis4;
      p_1.x = poly1_p1->x; p_1.y = poly1_p1->y; p_1.bulge = poly1_p1->bulge;
      p_2.x = poly1_p2->x; p_2.y = poly1_p2->y; p_2.bulge = poly1_p2->bulge;
      p_3.x = poly2_p1->x; p_3.y = poly2_p1->y; p_3.bulge = poly2_p1->bulge;
      p_4.x = poly2_p2->x; p_4.y = poly2_p2->y; p_4.bulge = poly2_p2->bulge;
      if (!IntersectInfiniteLine2(&p_1, &p_2, &p_3, &p_4,&inter)) // p1p2 - p3p4
      {
         if (ErrorMessage("Can't do it. How about snap?", "Look out", MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }

      dis1 = sqrt(pow(p_1.x - inter.x,2) + pow(p_1.y - inter.y,2));
      dis2 = sqrt(pow(p_2.x - inter.x,2) + pow(p_2.y - inter.y,2)); 
      dis3 = sqrt(pow(p_3.x - inter.x,2) + pow(p_3.y - inter.y,2));
      dis4 = sqrt(pow(p_4.x - inter.x,2) + pow(p_4.y - inter.y,2));
      if ((dis2 > dis1 || dis3 > dis4) &&
         (!Point2InSeg(&p_1, &p_2, &inter) && 
         !Point2InSeg(&p_3, &p_4, &inter)))
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
      else if ((dis2 > dis1 && !Point2InSeg(&p_1, &p_2, &inter)) ||
            (dis3 > dis4 && !Point2InSeg(&p_3, &p_4, &inter)))
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
      angle2 = ArcTan2(p_4.y - inter.y,p_4.x - inter.x);
      angle1 = ArcTan2(p_1.y - inter.y,p_1.x - inter.x);
      da = angle2 - angle1;
      if (da >= 2*PI)
         da -= 2*PI;
      if (da < 0.0)
         da += 2*PI;
      if (da > PI)
         da = 2*PI - da;
      if (fabs(da - 0) < SMALLNUMBER)
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
      dis_inter = (doc->getSettings().JOIN_chamfer/2) / sin(da/2);
      inter1.x = inter.x + dis_inter * (p_1.x - inter.x) / dis1;
      inter1.y = inter.y + dis_inter * (p_1.y - inter.y) / dis1;
      inter2.x = inter.x + dis_inter * (p_4.x - inter.x) / dis4;
      inter2.y = inter.y + dis_inter * (p_4.y - inter.y) / dis4;
      
      merge_p1->x = (DbUnit)inter1.x; merge_p1->y = (DbUnit)inter1.y; 
      merge_p1->bulge = (DbUnit)0.0;
      merge_p2->x = (DbUnit)inter2.x; merge_p2->y = (DbUnit)inter2.y; 
      merge_p2->bulge = (DbUnit)0.0;
      return 2;
   }
   break;
   
   case Join_Fillet:
   {
      Point2 p_1,p_2,p_3,p_4,inter,inter1,inter2,center;
      double angle1,angle2,da,sa,dis_inter,dis1,dis2,dis3,dis4,bulge, radius;
      p_1.x = poly1_p1->x; p_1.y = poly1_p1->y; p_1.bulge = poly1_p1->bulge;
      p_2.x = poly1_p2->x; p_2.y = poly1_p2->y; p_2.bulge = poly1_p2->bulge;
      p_3.x = poly2_p1->x; p_3.y = poly2_p1->y; p_3.bulge = poly2_p1->bulge;
      p_4.x = poly2_p2->x; p_4.y = poly2_p2->y; p_4.bulge = poly2_p2->bulge;
      if (!IntersectInfiniteLine2(&p_1, &p_2, &p_3, &p_4,&inter)) // p1p2 - p3p4
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }

      dis1 = sqrt(pow(p_1.x - inter.x,2) + pow(p_1.y - inter.y,2));
      dis2 = sqrt(pow(p_2.x - inter.x,2) + pow(p_2.y - inter.y,2)); 
      dis3 = sqrt(pow(p_3.x - inter.x,2) + pow(p_3.y - inter.y,2));
      dis4 = sqrt(pow(p_4.x - inter.x,2) + pow(p_4.y - inter.y,2));
      if ((dis2 > dis1 || dis3 > dis4) &&
         (!Point2InSeg(&p_1, &p_2, &inter) || 
         !Point2InSeg(&p_3, &p_4, &inter)))
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
      else if ((dis2 > dis1 && !Point2InSeg(&p_1, &p_2, &inter)) ||
            (dis3 > dis4 && !Point2InSeg(&p_3, &p_4, &inter)))
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
      angle2 = ArcTan2(p_4.y - inter.y,p_4.x - inter.x);
      angle1 = ArcTan2(p_1.y - inter.y,p_1.x - inter.x);
      da = angle2 - angle1;
      if (da >= 2*PI)
         da -= 2*PI;
      if (da < 0.0)
         da += 2*PI;
      if (da > PI)
         da = 2*PI - da;
      if (fabs(da - PI) < SMALLNUMBER)
      {
         if (ErrorMessage("Can't do it. How about snap?",
                   "Look out",MB_YESNO) == IDYES)
            return 0; 
         return -1;
      }
      dis_inter = doc->getSettings().JOIN_radius / tan(da/2);
      inter1.x = inter.x + dis_inter * (p_1.x - inter.x) / dis1;
      inter1.y = inter.y + dis_inter * (p_1.y - inter.y) / dis1;
      inter2.x = inter.x + dis_inter * (p_4.x - inter.x) / dis4;
      inter2.y = inter.y + dis_inter * (p_4.y - inter.y) / dis4;
      merge_p1->x = (DbUnit)inter1.x; merge_p1->y = (DbUnit)inter1.y; 
      merge_p1->bulge = (DbUnit)0.0;
      merge_p2->x = (DbUnit)inter2.x; merge_p2->y = (DbUnit)inter2.y; 
      merge_p2->bulge = (DbUnit)0.0;
      

      da = da - PI;//da is inter1--center--inter2
      bulge = tan(da/4);
      ArcPoint2Angle(inter1.x, inter1.y, inter2.x, inter2.y,
                  da, &center.x, &center.y, &radius, &sa);

      double mx, my;
      GetBulgeMidPoint(merge_p1->x, merge_p1->y, merge_p2->x, merge_p2->y, bulge, &mx, &my); 

      dis1 = sqrt(pow(mx - inter.x,2) + 
               pow(my - inter.y,2));
      dis2 = sqrt(pow(mx - inter.x,2) + 
               pow(my - inter.y,2));
      if (dis2 < dis1)
         bulge = -bulge;
      merge_p1->bulge = (DbUnit)bulge;
      return 2;
   }
   break;
   
   case Join_Snap:
      {
         Point2 p_1,p_2,p_3,p_4,inter;
         p_1.x = poly1_p1->x; p_1.y = poly1_p1->y; p_1.bulge = poly1_p1->bulge;
         p_2.x = poly1_p2->x; p_2.y = poly1_p2->y; p_2.bulge = poly1_p2->bulge;
         p_3.x = poly2_p1->x; p_3.y = poly2_p1->y; p_3.bulge = poly2_p1->bulge;
         p_4.x = poly2_p2->x; p_4.y = poly2_p2->y; p_4.bulge = poly2_p2->bulge;
         if (!IntersectInfiniteLine2(&p_1, &p_2, &p_3, &p_4,&inter)) // p1p2 - p3p4
            return 0;
         else if (Point2InSeg(&p_1, &p_2, &inter) &&
                Point2InSeg(&p_3, &p_4, &inter))
         {
            merge_p1->x = (DbUnit)inter.x; merge_p1->y = (DbUnit)inter.y; 
            merge_p1->bulge = poly2_p1->bulge;
            return 1;
         }
         return 0;
      }
   break;
   default:
      return -1;
   } // end switch mode

   return 0;
}

/******************************************************************************
* OnJoinAutojoin
*/
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance);
static int TryToConnect(BOOL Head, SelectStruct *s, double variance, CCEtoODBDoc *doc);
static int CheckForBadVariance(CPoly *poly1, CPoly*poly2, double variance);



/******************************************************************************
*/
void CCEtoODBView::OnAutojoinPanHead() 
{
   if (AutoJoinRun)
      PanCoordinate(headX, headY, TRUE);  
}

/******************************************************************************
*/
void CCEtoODBView::OnAutojoinPanTail() 
{
   if (AutoJoinRun)
      PanCoordinate(tailX, tailY, TRUE);  
}

/******************************************************************************
*/
void CCEtoODBDoc::OnJoinAutojoin() 
{
   AutoJoinRun = FALSE;
   FirstAuto = TRUE;

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   if (nothingIsSelected())
   {
      ErrorMessage("Nothing Selected");
      return;
   }

   SelectStruct *s = getSelectStack().getAt(0);

   if (s->getData()->getDataType() != T_POLY)
   {
      ErrorMessage("Selected Entity is not a Polyline");
      return;
   }

   if (s->getData()->getPolyList()->GetHead()->isClosed())
   {
      ErrorMessage("Can not Join a Closed Polyline");
      return;
   }

   ClearUndo();

   double variance = getSettings().JOIN_autoSnapSize;
   if (variance < SMALLNUMBER)
   {
      int decimals = GetDecimals(getSettings().getPageUnits());
      variance = 1;
      for (int i=0; i<decimals; i++)
         variance *= 0.1;
   }

   BOOL Head = TRUE;
   while (TRUE)
   {
      int result = TryToConnect(Head, s, variance, this);

      if (result == 2) // closed
         return;

      if (!result) // couldn't connect anymore to this end
      {
         if (Head)
            Head = FALSE;
         else
            break;
      }
   }

   Mat2x2 m;
   RotMat2(&m, s->rotation);
   Point2 point;

   CPnt *pnt = s->getData()->getPolyList()->GetHead()->getPntList().GetHead();
   point.x = pnt->x * s->scale;
   if (s->mirror & MIRROR_FLIP) point.x = -point.x;
   point.y = pnt->y * s->scale;
   TransPoint2(&point, 1, &m, s->insert_x, s->insert_y);
   headX = point.x;
   headY = point.y;

   pnt = s->getData()->getPolyList()->GetHead()->getPntList().GetTail();
   point.x = pnt->x * s->scale;
   if (s->mirror & MIRROR_FLIP) point.x = -point.x;
   point.y = pnt->y * s->scale;
   TransPoint2(&point, 1, &m, s->insert_x, s->insert_y);
   tailX = point.x;
   tailY = point.y;

   AutoJoinRun = TRUE;
}

static int TryToConnect(BOOL Head, SelectStruct *s, double variance, CCEtoODBDoc *doc)
{
   SelectStruct temp(*s);
   //memcpy(&temp, s, sizeof(SelectStruct));

   CPoly *poly = s->getData()->getPolyList()->GetHead();
   CPnt *pnt;

   if (Head)
      pnt = poly->getPntList().GetHead();
   else 
      pnt = poly->getPntList().GetTail();

   DataStruct *connectData;
   CPoly *connectPoly = NULL;
   BOOL connectHead;

   POSITION dataPos = s->getParentDataList()->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = s->getParentDataList()->GetNext(dataPos);

      if (data->getDataType() != T_POLY || data->getLayerIndex() != s->getData()->getLayerIndex())
         continue;

      if (data == s->getData())
         continue;

      CPoly *testPoly = data->getPolyList()->GetHead();

      if (poly->getWidthIndex() != testPoly->getWidthIndex())
         continue;

      CPnt *testPnt = testPoly->getPntList().GetHead();
      if (PntsOverlap(pnt, testPnt, variance))
      {
         if (connectPoly)
         {
            int decimals = GetDecimals(doc->getSettings().getPageUnits());
            if (CheckForBadVariance(connectPoly, testPoly, variance))
            {
               CString  tmp;
               tmp.Format("It appears that the Auto Join Snap Size %1.*lf is too large.\nGo to System Settings to change",
                  decimals, variance);
               ErrorMessage(tmp, "Warning");
            }

            CString buf;
            buf.Format("Reached a fork at (%+.*lf, %+.*lf).\nUse manual join functions to continue.\n(If there doesn't seem to be a fork, there may be overlapping lines.)", 
                  decimals, pnt->x, decimals, pnt->y);
            ErrorMessage(buf, "Reached a Decision");
            return 0; // decision
         }
         else
         {
            temp.setData(data);
            connectPoly = testPoly;
            connectHead = TRUE;
            connectData = data;
         }
      }
      else
      {
         testPnt = testPoly->getPntList().GetTail();
         if (PntsOverlap(pnt, testPnt, variance))
         {
            if (connectPoly)
            {
               int decimals = GetDecimals(doc->getSettings().getPageUnits());
               if (CheckForBadVariance(connectPoly, testPoly, variance))
               {
                  CString  tmp;
                  tmp.Format("It appears that the Auto Join Snap Size %1.*lf is too large.\nGo to System Settings to change",
                     decimals, variance);
                  ErrorMessage(tmp, "Warning");
               }
               CString buf;
               buf.Format("Reached a fork at (%+.*lf, %+.*lf).\nUse manual join functions to continue.\n(If there doesn't seem to be a fork, there may be overlapping lines.)", 
                     decimals, pnt->x, decimals, pnt->y);
               ErrorMessage(buf, "Reached a Decision");
               return 0; // decision
            }
            else
            {
               temp.setData(data);
               connectPoly = testPoly;
               connectHead = FALSE;
               connectData = data;
            }
         }
      }
   }

   if (!connectPoly)
      return 0;

   // connect polys
   if (FirstAuto)
   {
      FirstAuto = FALSE;
      doc->PrepareThisUndoLevel();
      undoItemList = doc->undo[doc->undo_level] = new UndoItemList;
      doc->undo_level = doc->redo_max = (doc->undo_level + 1) % MAX_UNDO_LEVEL;
      doc->ClearNextUndoLevel();

      s->getData()->setSelected(false);
      s->getData()->setMarked(false);

      UndoItemStruct *item = new UndoItemStruct;
      item->data = doc->getCamCadData().getNewDataStruct(*(s->getData()),true,s->getData()->getEntityNumber());
      //item->data->setEntityNumber(s->getData()->getEntityNumber());
      item->DataList = s->getParentDataList();
      item->num = s->getData()->getEntityNumber();
      item->type = Edit_Modify;
      undoItemList->AddTail(item);
   }

   UndoItemStruct *undoDeleted = new UndoItemStruct;
   undoItemList->AddTail(undoDeleted);
   undoDeleted->data = connectData;
   undoDeleted->type = Edit_Delete;
   undoDeleted->DataList = s->getParentDataList();
   dataPos = s->getParentDataList()->Find(connectData);
   POSITION tempPos = dataPos;
   s->getParentDataList()->GetPrev(tempPos);
   if (tempPos)
      undoDeleted->num = s->getParentDataList()->GetAt(tempPos)->getEntityNumber();
   else
      undoDeleted->num = 0;
   s->getParentDataList()->RemoveAt(dataPos);
   connectData->setSelected(false);
   connectData->setMarked(false);

   if (Head)
   {
      if (connectHead)
         ReversePoly(&connectPoly->getPntList());

      POSITION pos = connectPoly->getPntList().GetTailPosition();
      if (pos)
         connectPoly->getPntList().GetPrev(pos);
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(connectPoly->getPntList().GetPrev(pos)));
         poly->getPntList().AddHead(newPnt);
      }
   }
   else
   {
      if (!connectHead)
         ReversePoly(&connectPoly->getPntList());

      poly->getPntList().GetTail()->bulge = connectPoly->getPntList().GetHead()->bulge;

      POSITION pos = connectPoly->getPntList().GetHeadPosition();
      if (pos)
         connectPoly->getPntList().GetNext(pos);
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(connectPoly->getPntList().GetNext(pos)));
         poly->getPntList().AddTail(newPnt);
      }
   }

   // close poly?
   CPnt *headPnt = poly->getPntList().GetHead(), *tailPnt = poly->getPntList().GetTail();
   if (PntsOverlap(headPnt, tailPnt, variance))
   {
      tailPnt->x = headPnt->x;
      tailPnt->y = headPnt->y;
      poly->setClosed(true);
      doc->DrawEntity(s, 1, TRUE);
      return 2;
   }

   doc->DrawEntity(s, 1, TRUE);

   // set up globals for join by hand
   joinPolyData = s->getData();
   joinPoly = poly;
   joinPolyFile = s->filenum;

   return 1;
}

/******************************************************************************
* PntsOverlap
*/
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance)
{ return ((fabs(p1->x - p2->x) < variance) && (fabs(p1->y - p2->y) < variance)); }

/******************************************************************************
* CheckForBadVariance
*/
static int CheckForBadVariance(CPoly *poly1, CPoly* poly2, double variance)
{
   CPnt *p1a, *p1b, *p2a, *p2b;
   p1a = poly1->getPntList().GetHead();
   p1b = poly1->getPntList().GetTail();
   p2a = poly2->getPntList().GetHead();
   p2b = poly2->getPntList().GetTail();
   if ((sqrt(pow((p1a->x - p1b->x), 2) + pow((p1a->y - p1b->y), 2)) < variance) ||
         (sqrt(pow((p2a->x - p2b->x), 2) + pow((p2a->y - p2b->y), 2)) < variance))
   {
      return 1;
   }
   return 0;
}

/******************************************************************************
*/


// end JOIN.CPP