// $Header: /CAMCAD/5.0/EditPoly.cpp 29    6/21/07 8:26p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2004. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "ccview.h"
#include "geomlib.h"
#include "mainfrm.h"
#include "license.h"
#include "crypt.h"
#include "graph.h"
#include <math.h>
#include "polylib.h"
#include "bb_lib.h"
#include "EntityNumber.h"
#include "CCEtoODB.h"

extern CView   *activeView;

static CPnt *cur, *prev, *next;  // vertices to remember
static CPoly *poly;              // poly to work with
static BOOL FirstLine, LastLine; // whether should not draw some lines because first or last line of poly
static int ax, ay, bx, by;       // connection points
static int dx, dy;               // delta values for moving a segment
DataStruct *CurrentPolyData;
static CPoly *CurrentPoly;
static int CurrentPolyFile;

extern CStatusBar *StatusBar; // from MAINFRM.CPP

void ResetButtons();
void InsertModifyUndo(CCEtoODBDoc *doc, BOOL Head);
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);
void FillWidthCB(CComboBox *m_widthCB, short penWidthIndex, CCEtoODBDoc *doc);
void GetWidthIndex(CComboBox *m_widthCB, short *dataWidthIndex, CCEtoODBDoc *doc);

/* FoundAPoly ********************************************************************/
bool FoundAPoly(CCEtoODBDoc *doc, SelectStruct *s, CPnt *Pnt, CPoly *&Poly, BOOL Before)
{
   Mat2x2 m;
   RotMat2(&m, s->rotation);
   cur = Pnt;
   poly = Poly;

	if (poly == NULL)
		return false;

   switch (((CCEtoODBView*)activeView)->cursorMode)
   {
   case EditPoly_EditBulge:
      {
         POSITION pos = poly->getPntList().GetHeadPosition();
         poly->getPntList().GetNext(pos);
         FirstLine = cur == poly->getPntList().GetAt(pos);
         LastLine = cur == poly->getPntList().GetTail();

         pos = poly->getPntList().Find(cur);
         poly->getPntList().GetPrev(pos);
         prev = poly->getPntList().GetAt(pos);

         // clear a bulge
         if (fabs(prev->bulge) > SMALLNUMBER)
         {
            InsertModifyUndo(doc, TRUE);
            //((CCEtoODBView*)activeView)->HideSearchCursor();
            doc->DrawEntity(s, -1, FALSE);
            prev->bulge = (DbUnit)0.0;
            doc->DrawEntity(s, 1, FALSE);
            //((CCEtoODBView*)activeView)->ShowSearchCursor();
         }
         else
         {
            double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
            Point2 p;

            // calculate (ax, ay) for prev pnt
            p.x = prev->x * s->scale; 
            p.y = prev->y * s->scale;
            if (s->mirror) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);
            
            ax = round(p.x / factor);
            ay = round(p.y / factor);

            // calculate (bx, by) for this pnt
            p.x = cur->x * s->scale; 
            p.y = cur->y * s->scale;
            if (s->mirror) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            bx = round(p.x / factor);
            by = round(p.y / factor);

            //((CCEtoODBView*)activeView)->HideSearchCursor();
            doc->FoundPolyEntity = TRUE;
            //((CCEtoODBView*)activeView)->ShowSearchCursor();
         }
      }
      break;

   case EditPoly_MoveVertex:
      {
         double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
         Point2 p;

         // cur should be pnt to move
         POSITION pos = poly->getPntList().Find(cur);
         if (Before)
            poly->getPntList().GetPrev(pos);
         cur = poly->getPntList().GetAt(pos);

         // figure FirstLine & LastLine
         FirstLine = cur == poly->getPntList().GetHead();
         LastLine = cur == poly->getPntList().GetTail();

         // calc (ax, ay) for prev pnt
         if (!FirstLine || poly->isClosed())
         {
            POSITION prevPos;
            if (poly->isClosed() && FirstLine)
               prevPos = poly->getPntList().GetTailPosition();
            else
               prevPos = pos;
            poly->getPntList().GetPrev(prevPos);
            prev = poly->getPntList().GetAt(prevPos);

            p.x = prev->x * s->scale; 
            p.y = prev->y * s->scale;
            if (s->mirror) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            ax = (int)(p.x / factor);
            ay = (int)(p.y / factor);
         }

         // calc (bx, by) for next pnt
         if (!LastLine || poly->isClosed())
         {
            if (poly->isClosed() && LastLine)
               pos = poly->getPntList().GetHeadPosition();
            poly->getPntList().GetNext(pos);
            next = poly->getPntList().GetAt(pos);

            p.x = next->x * s->scale; 
            p.y = next->y * s->scale;
            if (s->mirror) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);
            
            bx =(int)(p.x / factor);
            by =(int)(p.y / factor);
         }

         //((CCEtoODBView*)activeView)->HideSearchCursor();
         doc->FoundPolyEntity = TRUE;
         //((CCEtoODBView*)activeView)->ShowSearchCursor();
      }
      break;

   case EditPoly_AddVertex:
      {
         double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
         Point2 p;

         POSITION pos = poly->getPntList().Find(cur);
         poly->getPntList().GetPrev(pos);
         prev = poly->getPntList().GetAt(pos);

         p.x = prev->x * s->scale; 
         p.y = prev->y * s->scale;
         if (s->mirror) p.x = -p.x;
         TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

         ax = (int)(p.x / factor);
         ay = (int)(p.y / factor);

         p.x = cur->x * s->scale; 
         p.y = cur->y * s->scale;
         if (s->mirror) p.x = -p.x;
         TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

         bx = (int)(p.x / factor);
         by = (int)(p.y / factor);

         //((CCEtoODBView*)activeView)->HideSearchCursor();
         doc->FoundPolyEntity = TRUE;
         //((CCEtoODBView*)activeView)->ShowSearchCursor();
      }
      break;

   case EditPoly_DeleteVertex:
      {
         InsertModifyUndo(doc, TRUE);
         //((CCEtoODBView*)activeView)->HideSearchCursor();
         doc->DrawEntity(s, -1, FALSE);

         POSITION pos = poly->getPntList().Find(cur);
         if (Before)
         {
            poly->getPntList().GetPrev(pos);
            cur = poly->getPntList().GetAt(pos);
         }

         if (poly->isClosed() && cur == poly->getPntList().GetHead())
         {
            CPnt *tail, *newHead;
            POSITION tempPos = poly->getPntList().GetHeadPosition();
            poly->getPntList().GetNext(tempPos);
            newHead = poly->getPntList().GetAt(tempPos);
            tail = poly->getPntList().GetTail();
            tail->x = newHead->x;
            tail->y = newHead->y;
         }

         if (poly->isClosed() && cur == poly->getPntList().GetTail())
         {
            CPnt *head, *newTail;
            POSITION tempPos = poly->getPntList().GetTailPosition();
            poly->getPntList().GetPrev(tempPos);
            newTail = poly->getPntList().GetAt(tempPos);
            head = poly->getPntList().GetHead();
            head->x = newTail->x;
            head->y = newTail->y;
         }

         poly->getPntList().RemoveAt(pos);
         delete cur;

         doc->DrawEntity(s, 1, FALSE);
         //((CCEtoODBView*)activeView)->ShowSearchCursor();
         break;
      }

   case EditPoly_DeleteSegment:
      {
         InsertModifyUndo(doc, TRUE);
         //((CCEtoODBView*)activeView)->HideSearchCursor();
         doc->DrawEntity(s, -1, FALSE);

         if (cur == poly->getPntList().GetHead())
         {
            delete poly->getPntList().GetHead();
            poly->getPntList().RemoveHead();
         }
         else if (cur == poly->getPntList().GetTail())
         {
            delete poly->getPntList().GetTail();
            poly->getPntList().RemoveTail();
         }
         else
         {
            if (poly->isClosed()) // reorganize points from 1 continuous poly
            {
               // remove head point because it was a duplicate of tail for Closed poly
               delete poly->getPntList().RemoveHead(); 

               CPnt *v = poly->getPntList().GetHead();
               while (v != cur)
               {
                  poly->getPntList().AddTail(v);
                  poly->getPntList().RemoveHead();
                  v = poly->getPntList().GetHead();
               }
            }
            else // break poly into 2 polys
            {
               CPoly *newPoly = new CPoly;
               memcpy(newPoly, poly, sizeof(CPoly)-sizeof(CPntList));
               s->getData()->getPolyList()->AddTail(newPoly);

               CPnt *v = poly->getPntList().GetTail();
               newPoly->getPntList().AddHead(v);
               poly->getPntList().RemoveTail();
               while (v != cur)
               {
                  v = poly->getPntList().GetTail();
                  newPoly->getPntList().AddHead(v);
                  poly->getPntList().RemoveTail();
               }
            }
         }

         poly->setClosed(false);
         poly->setFilled(false);

         // if last segment in this poly, remove poly from polystruct
         if (poly->getPntList().GetCount() < 2)
         {
            delete poly->getPntList().GetHead();
            poly->getPntList().RemoveHead();
            s->getData()->getPolyList()->deleteAt(s->getData()->getPolyList()->Find(poly));
				poly = NULL;
				Poly = NULL;
         }

         // if no polys in polystruct, remove data from list
         if (!s->getData()->getPolyList()->GetCount())
         {
            RemoveOneEntityFromDataList(doc, s->getParentDataList(), s->getData());
            return true;
         }
         else
         {
            doc->DrawEntity(s, 1, FALSE);
            //((CCEtoODBView*)activeView)->ShowSearchCursor();
         }
         break;
      }

   case EditPoly_MoveSegment:
      {
         Point2 p;
         CPnt *v;
         double prev_x, prev_y, cur_x, cur_y;
         double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);

         // figure FirstLine & LastLine
         POSITION pos = poly->getPntList().Find(cur);
         poly->getPntList().GetPrev(pos);
         prev = poly->getPntList().GetAt(pos);
         FirstLine = prev == poly->getPntList().GetHead();
         LastLine = cur == poly->getPntList().GetTail();

         // calculate dx & dy for segment
         p.x = prev->x * s->scale; 
         p.y = prev->y * s->scale;
         if (s->mirror) p.x = -p.x;
         TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

         prev_x = p.x / factor;
         prev_y = p.y / factor;
         
         p.x = cur->x * s->scale; 
         p.y = cur->y * s->scale;
         if (s->mirror) p.x = -p.x;
         TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

         cur_x = p.x / factor;
         cur_y = p.y / factor;

         dx = (int)(cur_x - prev_x);
         dy = (int)(cur_y - prev_y);

         // calc (ax, ay) for prev pnt
         if (!FirstLine || poly->isClosed())
         {
            if (FirstLine && poly->isClosed())
               pos = poly->getPntList().GetTailPosition();
            else
               pos = poly->getPntList().Find(prev);
            poly->getPntList().GetPrev(pos);
            v = poly->getPntList().GetAt(pos);
            
            p.x = v->x * s->scale; 
            p.y = v->y * s->scale;
            if (s->mirror) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            ax = (int)(p.x / factor);
            ay = (int)(p.y / factor);
         }

         // calc (bx, by) for cur pnt
         if (!LastLine || poly->isClosed())
         {
            if (LastLine && poly->isClosed())
               pos = poly->getPntList().GetHeadPosition();
            else
               pos = poly->getPntList().Find(cur);
            poly->getPntList().GetNext(pos);
            v = poly->getPntList().GetAt(pos);
            
            p.x = v->x * s->scale; 
            p.y = v->y * s->scale;
            if (s->mirror) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            bx = (int)(p.x / factor);
            by = (int)(p.y / factor);
         }
         // if LastLine, but Closed

         //((CCEtoODBView*)activeView)->HideSearchCursor();
         doc->FoundPolyEntity = TRUE;
         //((CCEtoODBView*)activeView)->ShowSearchCursor();
         break;
      }
   };
   return false;
}

/* OnEditBulge ********************************************************************/
void CCEtoODBView::OnEditBulge() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == EditPoly_EditBulge)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = EditPoly_EditBulge;
   //ShowSearchCursor();
   cursorFirstDrag = TRUE;
   GetDocument()->FoundPolyEntity = FALSE;
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_EDIT_BULGE);
#endif
}

void CCEtoODBView::OnMouseMove_EditBulge(CDC *dc, CPoint *point)
{
   if (!GetDocument()->FoundPolyEntity) return;

   dc->SetROP2(R2_NOT);

   double cx, cy, r, sa, da;
   if (!cursorFirstDrag)
   {
      ArcPoint3(ax, ay, cursorPrevPnt.x, cursorPrevPnt.y, bx, by, &cx, &cy, &r, &sa, &da);
      if (da > 0.0)
         dc->Arc(round(cx-r), round(cy+r), round(cx+r), round(cy-r), 
            round(cx+r*cos(sa)), round(cy+r*sin(sa)), round(cx+r*cos(sa+da)), round(cy+r*sin(sa+da)));
      else
         dc->Arc(round(cx-r), round(cy+r), round(cx+r), round(cy-r), 
            round(cx+r*cos(sa+da)), round(cy+r*sin(sa+da)), round(cx+r*cos(sa)), round(cy+r*sin(sa)));
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = point->x;
   cursorPrevPnt.y = point->y;

   ArcPoint3(ax, ay, cursorPrevPnt.x, cursorPrevPnt.y, bx, by, &cx, &cy, &r, &sa, &da);
   if (da > 0.0)
      dc->Arc(round(cx-r), round(cy+r), round(cx+r), round(cy-r), 
              round(cx+r*cos(sa)), round(cy+r*sin(sa)), round(cx+r*cos(sa+da)), round(cy+r*sin(sa+da)));
   else
      dc->Arc(round(cx-r), round(cy+r), round(cx+r), round(cy-r), 
               round(cx+r*cos(sa+da)), round(cy+r*sin(sa+da)), round(cx+r*cos(sa)), round(cy+r*sin(sa)));
}

void CCEtoODBView::OnLButtonDown_EditBulge(CDC *dc, double x, double y)
{
   //HideSearchCursor();

   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->SelectList.GetHead(); // poly is selected item
   doc->DrawEntity(s, -1, FALSE); // erase old poly

   InsertModifyUndo(doc, TRUE);

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
   Point2 p, a, b;
   p.x = x; p.y = y;
   Untransform(doc, &p, s);
   a.x = ax*factor; a.y = ay*factor;
   Untransform(doc, &a, s);
   b.x = bx*factor; b.y = by*factor;
   Untransform(doc, &b, s);

   double cx, cy, r, sa, da;
   ArcPoint3(a.x, a.y, p.x, p.y, b.x, b.y, &cx, &cy, &r, &sa, &da);
   prev->bulge = (DbUnit)tan(da/4);

   doc->DrawEntity(s, 1, FALSE); // draw new poly

   cursorFirstDrag = TRUE;
   doc->FoundPolyEntity = FALSE;
   //ShowSearchCursor();
}

/* OnDeleteSegment ****************************************************************/
void CCEtoODBView::OnDeleteSegment() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == EditPoly_DeleteSegment)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = EditPoly_DeleteSegment;
   //ShowSearchCursor();
   GetDocument()->FoundPolyEntity = FALSE;
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_DELETE_SEGMENT);
#endif
}

/* MoveVertex ****************************************************************/
void CCEtoODBView::OnMoveVertex() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == EditPoly_MoveVertex)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = EditPoly_MoveVertex;
   //ShowSearchCursor();
   cursorFirstDrag = TRUE;
   GetDocument()->FoundPolyEntity = FALSE;
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_MOVE_VERTEX);
#endif
}

void CCEtoODBView::OnMouseMove_MoveVertex(CDC *dc, CPoint *pnt)
{
   if (!GetDocument()->FoundPolyEntity)
		return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      if (!FirstLine || (poly != NULL && poly->isClosed()))
      {
         dc->MoveTo(ax, ay);
         dc->LineTo(cursorPrevPnt);
      }
      else
         dc->MoveTo(cursorPrevPnt);

      if (!LastLine || (poly != NULL && poly->isClosed()))
         dc->LineTo(bx, by);
   }

   cursorFirstDrag = FALSE;
   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   if (!FirstLine || (poly != NULL && poly->isClosed()))
   {
      dc->MoveTo(ax, ay);
      dc->LineTo(cursorPrevPnt);
   }
   else
      dc->MoveTo(cursorPrevPnt);

   if (!LastLine || (poly != NULL && poly->isClosed()))
      dc->LineTo(bx, by);
}

void CCEtoODBView::OnLButtonDown_MoveVertex(CDC *dc, double x, double y)
{
   //HideSearchCursor();

   // erase ghosts
   if (!cursorFirstDrag)
   {
      if (!FirstLine || (poly != NULL && poly->isClosed()))
      {
         dc->MoveTo(ax, ay);
         dc->LineTo(cursorPrevPnt);
      }
      else
         dc->MoveTo(cursorPrevPnt);

      if (!LastLine || (poly != NULL && poly->isClosed()))
         dc->LineTo(bx, by);
   }

   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->SelectList.GetHead(); // poly is selected item
   doc->DrawEntity(s, -1, FALSE); // erase old poly

   InsertModifyUndo(doc, TRUE);

   Point2 p;
   p.x = x; p.y = y;
   Untransform(doc, &p, s);

   cur->x = (DbUnit)p.x;
   cur->y = (DbUnit)p.y;

   if (FirstLine && poly != NULL && poly->isClosed()) // need to modify tail
   {
      CPnt *tail = poly->getPntList().GetTail();
      tail->x = (DbUnit)p.x;
      tail->y = (DbUnit)p.y;
   }

   if (LastLine && poly != NULL && poly->isClosed()) // need to modify head
   {
      CPnt *head = poly->getPntList().GetHead();
      head->x = (DbUnit)p.x;
      head->y = (DbUnit)p.y;
   }

   doc->DrawEntity(s, 1, FALSE); // draw new poly

   cursorFirstDrag = TRUE;
   doc->FoundPolyEntity = FALSE;
   //ShowSearchCursor();
}

/* MoveSegment ***************************************************************/
void CCEtoODBView::OnMoveSegment() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == EditPoly_MoveSegment)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorMode = EditPoly_MoveSegment;
   cursorFirstDrag = TRUE;
   GetDocument()->FoundPolyEntity = FALSE;
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_MOVE_SEGMENT);
#endif
}

void CCEtoODBView::OnMouseMove_MoveSegment(CDC *dc, CPoint *pnt)
{
   if (!GetDocument()->FoundPolyEntity) return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      if (!FirstLine || (poly != NULL && poly->isClosed()))
      {
         dc->MoveTo(ax, ay);
         dc->LineTo(cursorPrevPnt);
      }
      else
         dc->MoveTo(cursorPrevPnt);

      dc->LineTo(cursorPrevPnt.x + dx, cursorPrevPnt.y + dy);
      if (!LastLine || (poly != NULL && poly->isClosed()))
         dc->LineTo(bx, by);
   }

   cursorFirstDrag = FALSE;
   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   if (!FirstLine || (poly != NULL && poly->isClosed()))
   {
      dc->MoveTo(ax, ay);
      dc->LineTo(cursorPrevPnt);
   }
   else
      dc->MoveTo(cursorPrevPnt);

   dc->LineTo(cursorPrevPnt.x + dx, cursorPrevPnt.y + dy);
   if (!LastLine || (poly != NULL && poly->isClosed()))
      dc->LineTo(bx, by);
}

void CCEtoODBView::OnLButtonDown_MoveSegment(CDC *dc, double x, double y)
{
   //HideSearchCursor();

   if (!cursorFirstDrag)
   {
      if (!FirstLine || (poly != NULL && poly->isClosed()))
      {
         dc->MoveTo(ax, ay);
         dc->LineTo(cursorPrevPnt);
      }
      else
         dc->MoveTo(cursorPrevPnt);

      dc->LineTo(cursorPrevPnt.x + dx, cursorPrevPnt.y + dy);
      if (!LastLine || (poly != NULL && poly->isClosed()))
         dc->LineTo(bx, by);
   }

   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->SelectList.GetHead(); // poly is selected item

   doc->DrawEntity(s, -1, FALSE); // erase old poly
   
   InsertModifyUndo(doc, TRUE);

   Point2 p, delta;
   p.x = x; p.y = y;
   Untransform(doc, &p, s);

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
   delta.x = factor * dx + s->insert_x; delta.y = factor * dy + s->insert_y;
   Untransform(doc, &delta, s);

   prev->x = (DbUnit)p.x;
   prev->y = (DbUnit)p.y;
   cur->x = (DbUnit)(p.x + delta.x);
   cur->y = (DbUnit)(p.y + delta.y);

   if (FirstLine && poly != NULL && poly->isClosed()) // need to modify tail
   {
      CPnt *tail = poly->getPntList().GetTail();
      tail->x = prev->x;
      tail->y = prev->y;
   }

   if (LastLine && poly != NULL && poly->isClosed()) // need to modify head
   {
      CPnt *head = poly->getPntList().GetHead();
      head->x = cur->x;
      head->y = cur->y;
   }

   doc->DrawEntity(s, 1, FALSE); // draw new poly

   cursorFirstDrag = TRUE;
   doc->FoundPolyEntity = FALSE;
   //ShowSearchCursor();
}

/* DeleteVertex *****************************************************************/
void CCEtoODBView::OnDeleteVertex() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == EditPoly_DeleteVertex)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = EditPoly_DeleteVertex;
   //ShowSearchCursor();
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_DELETE_VERTEX);
#endif
}

/* AddVertex *****************************************************************/
void CCEtoODBView::OnAddVertex() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == EditPoly_AddVertex)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = EditPoly_AddVertex;
   //ShowSearchCursor();
   cursorFirstDrag = TRUE;
   GetDocument()->FoundPolyEntity = FALSE;
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_ADD_VERTEX);
#endif
}

void CCEtoODBView::OnMouseMove_AddVertex(CDC *dc, CPoint *pnt)
{
   if (!GetDocument()->FoundPolyEntity) return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      dc->MoveTo(ax, ay);
      dc->LineTo(cursorPrevPnt);
      dc->LineTo(bx, by);
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(ax, ay);
   dc->LineTo(cursorPrevPnt);
   dc->LineTo(bx, by);
}

void CCEtoODBView::OnLButtonDown_AddVertex(CDC *dc, double x, double y)
{
   //HideSearchCursor();

   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->SelectList.GetHead(); // poly is selected item

   // erase ghosts
   if (!cursorFirstDrag)
   {
      dc->MoveTo(ax, ay);
      dc->LineTo(cursorPrevPnt);
      dc->LineTo(bx, by);
   }

   doc->DrawEntity(s, -1, FALSE); // erase old poly

   InsertModifyUndo(doc, TRUE);

   Point2 p;
   p.x = x; p.y = y;
   Untransform(doc, &p, s);

   CPnt *newPnt = new CPnt;
   newPnt->x = (DbUnit)p.x;
   newPnt->y = (DbUnit)p.y;
   newPnt->bulge = (DbUnit)0.0;

	if (poly != NULL)
		poly->getPntList().InsertBefore(poly->getPntList().Find(cur), newPnt);

   doc->DrawEntity(s, 1, FALSE); // draw new poly

   cursorFirstDrag = TRUE;
   doc->FoundPolyEntity = FALSE;
   //ShowSearchCursor();
}

/* OnLinkPolys *****************************************************************/
void CCEtoODBView::OnLinkPolys() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == LinkPolys)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = LinkPolys;
   //ShowSearchCursor();
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_LINK_POLYS);

   CurrentPolyData = NULL;

   if (StatusBar)
      StatusBar->SetWindowText("Select Parent Poly");
}

/* LinkPolylines *****************************************************************/
void LinkPolylines(SelectStruct *s, CCEtoODBDoc *doc)
{
   // first entity selected
   if (CurrentPolyData == NULL) 
   {
      CurrentPolyData = s->getData();
      CurrentPolyFile = s->filenum;
      if (StatusBar)
         StatusBar->SetWindowText("Select Child Poly");
      return; 
   }

   if (CurrentPolyData == s->getData())
   {
      ErrorMessage("Same Entity Selected!", "Ignoring Selection");
      return;
   }

   if (CurrentPolyData->getLayerIndex() != s->getData()->getLayerIndex() || CurrentPolyFile != s->filenum)
   {
      ErrorMessage("Illegal Selection.  May be on a different layer or file.", "Ignoring Selection");
      return;
   }

   LinkPolyDatas(doc, s->getParentDataList(), CurrentPolyData, s->getData());

   SelectStruct *sel = doc->MakeThisEntitySelected(CurrentPolyData, s->filenum, s->insert_x, s->insert_y, s->scale, s->rotation, s->mirror, s->layer, s->getParentDataList());

   ((CCEtoODBView*)activeView)->cursorMode = Search;
   ResetButtons();

   doc->DrawEntity(sel, 3, TRUE);
}

/* OnUnlinkPoly *****************************************************************/
void CCEtoODBView::OnUnlinkPoly() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == UnlinkPoly)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = UnlinkPoly;
   //ShowSearchCursor();
   ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_UNLINK_POLY);

   if (StatusBar)
      StatusBar->SetWindowText("Select Poly");
}

void UnlinkPolyline(SelectStruct *s, CPoly *poly, CCEtoODBDoc *doc)
{
   CCamCadData& camCadData = doc->getCamCadData();
   FileStruct *file = NULL;

   if (s == NULL || 
      poly == NULL || 
      s->getData()->getPolyList()->GetCount() < 2 ||
      (file = doc->Find_File(s->filenum)) == NULL)
   {
      ((CCEtoODBView*)activeView)->cursorMode = Search;
      ResetButtons();
      return;
   }

   // add polylines to file polyline is in
   doc->PrepareAddEntity(file);

   // new poly entity with same characteristics
   DataStruct *newData = camCadData.getNewDataStruct(*(s->getData()));

   s->getData()->setSelected(false);
   s->getParentDataList()->InsertAfter(s->getParentDataList()->Find(s->getData()), newData);
      
   // move poly over (no need to free memory)
   newData->getPolyList() = new CPolyList;
   newData->getPolyList()->AddTail(poly);

   // remove polys from polylist (no need to free memory)
   s->getData()->getPolyList()->deleteAt(s->getData()->getPolyList()->Find(poly));
   poly = NULL;

   ((CCEtoODBView*)activeView)->cursorMode = Search;
   ResetButtons();

   doc->DrawEntity(s, 0, TRUE);
   s->setData(newData);
   doc->DrawEntity(s, 1, TRUE);
}


static void GraphResults(CCEtoODBDoc *doc, CDataList *dataList, CPntList *pntList)
{
   doc->PrepareAddEntity(doc->getFileList().GetHead());
   SetCurrentDataList(dataList);

   DataStruct *data = Graph_PolyStruct(Graph_Level("shrink", NULL, 0), 0, 0);

   int err;

   int width = Graph_Aperture("", T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0L, 0, &err);
   CPoly *poly = Graph_Poly(NULL, width, 0, 0, 1); 

   POSITION pos = pntList->GetHeadPosition();
   while (pos != NULL)
   {
      CPnt *pnt = pntList->GetNext(pos);
      Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
   }

   doc->UpdateAllViews(NULL);
}

/******************************************************************************
* OnPolyShrink()
*/
void CCEtoODBDoc::OnPolyShrink() 
{
   if (!SelectList.GetCount())
      return;

   SelectStruct *s = SelectList.GetHead();

   if (s->getData()->getDataType() != T_POLY)
      return;

   CPoly *poly = s->getData()->getPolyList()->GetHead();

   if (!poly->isClosed())
   {
      ErrorMessage("Close selected poly and retry.", "Selected Poly is not closed!");
      return;
   }

   InputDlg dlg;
   dlg.m_prompt = "Enter size of offset:\n(Positive=>Shrink, Negative=>Expand)";
   if (dlg.DoModal() != IDOK)
      return;
   double size = atof(dlg.m_input);
   if (fabs(size) < SMALLNUMBER)
      return;

   CPntList *newPntList = ShrinkPoly(&poly->getPntList(),getPageUnits(), size);
   
   if (newPntList)
   {
      GraphResults(this, s->getParentDataList(), newPntList);
      FreePntList(newPntList);
   }
}

/******************************************************************************
* OnPolyRemoveHatch()
*/
void CCEtoODBDoc::OnPolyRemoveHatch() 
{
   if (!SelectList.GetCount())
   {
      ErrorMessage("SelectList is Empty");
      return;
   }

   POSITION selectPos = SelectList.GetHeadPosition();
   while (selectPos)
   {
      SelectStruct *s = SelectList.GetNext(selectPos);

      if (s->getData()->getDataType() != T_POLY)
         continue;

      POSITION polyPos = s->getData()->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         POSITION tempPos = polyPos;
         CPoly *poly = s->getData()->getPolyList()->GetNext(polyPos);

         if (poly->isHatchLine())
         {
            //FreePoly(poly);
            s->getData()->getPolyList()->deleteAt(tempPos);
         }
      }
   }

   UpdateAllViews(NULL);
}


// end EDITPOLY.CPP
