// $Header: /CAMCAD/5.0/Pan.cpp 40    6/17/07 8:53p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCview.h"
#include "pan.h"
#include "resource.h"
#include <math.h>
#include "attrib.h"
#include "crypt.h"
#include "wrldview.h"
#include "xform.h"
#include "CCEtoODB.h"
#include "ViewSynchronizer.h"
#include "DcaEnumIterator.h"

extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern CView *activeView; // from CCVIEW.CPP

static long zoomBoxX, zoomBoxY;
static HCURSOR hCursor;
static double dragLeft, dragRight, dragTop, dragBottom;
static BOOL CaseSensitive = FALSE;
static BOOL SubString = FALSE;
static BOOL FindTexts = TRUE;
static BOOL FindAttribs = TRUE;
static CString searchText;
static int minScrollPos, maxScrollPos;

void ResetButtons();

void CCEtoODBView::OnAddToSynchronizedViews()
{
   CViewSynchronizer& viewSynchronizer = getApp().getViewSynchronizer();

   viewSynchronizer.registerView(this);

   if (viewSynchronizer.getViewList().getCount() > 1)
   {
      viewSynchronizer.setEnabled(true);
   }

   viewSynchronizer.synchronize();
}

void CCEtoODBView::OnDisableViewSynchronization()
{
   CViewSynchronizer& viewSynchronizer = getApp().getViewSynchronizer();

   viewSynchronizer.setEnabled(false);
   viewSynchronizer.empty();
}

/******************************************************************************
* OnSize
*/
void CCEtoODBView::OnSize(UINT nType, int cx, int cy) 
{
   CView::OnSize(nType, cx, cy);
   
   SetScroll();

   //getApp().getViewSynchronizer().setSize(this,cx,cy);
   //getApp().getViewSynchronizer().setZoom(this,GetXPos(),GetYPos(),ScaleNum,ScaleDenom);
}

/******************************************************************************
* UpdateScale
*/
void CCEtoODBView::UpdateScale()
{
   CCEtoODBDoc *doc = GetDocument();
   int maxZoom = doc->getSettings().getMaxZoom();

   if (ScaleDenom > maxZoom)
   {
      ScaleDenom = maxZoom;

      if (StatusBar)
      {
         CString message;
         message.Format("Max Zoom is %d:1",maxZoom);

         StatusBar->SetWindowText(message);
      }
   }

   if (ScaleNum > 2)
   {
      ScaleNum = 2;

      if (StatusBar)
         StatusBar->SetWindowText("Min Zoom is 0.5:1");
   }

   SetScroll();

   if (StatusBar)
   {
      char buf[20];
      if (ScaleDenom < 20)
         sprintf(buf, "Scale %.2lf:1", ScaleDenom / ScaleNum);
      else
         sprintf(buf, "Scale %.0lf:1", ScaleDenom / ScaleNum);
      StatusBar->SetPaneText(StatusBar->CommandToIndex(IDS_SCALE), buf);
   }
}

/******************************************************************************
* SetScroll
*/
void CCEtoODBView::SetScroll()
{
   CCEtoODBDoc *doc = GetDocument();

   CRect clientRect;
   GetClientRect(&clientRect);

   long unitsX, unitsY;
   Window_Shape(clientRect.Width(), clientRect.Height(), &unitsX, &unitsY, doc->maxXCoord - doc->minXCoord, doc->maxYCoord - doc->minYCoord);

   SCROLLINFO horzScrollInfo;
   horzScrollInfo.fMask = SIF_PAGE | SIF_RANGE;
   horzScrollInfo.nPage = (UINT)ceil(unitsX / ScaleDenom) + 1;
   horzScrollInfo.nMin = doc->minXCoord;
   horzScrollInfo.nMax = doc->maxXCoord;
   SetScrollInfo(SB_HORZ, &horzScrollInfo);

   SCROLLINFO vertScrollInfo;
   vertScrollInfo.fMask = SIF_PAGE | SIF_RANGE;
   vertScrollInfo.nPage = (UINT)ceil(unitsY / ScaleDenom) + 1;
   vertScrollInfo.nMin = doc->minYCoord;
   vertScrollInfo.nMax = doc->maxYCoord;
   SetScrollInfo(SB_VERT, &vertScrollInfo);
}

/******************************************************************************
* OnHScroll
*/
void CCEtoODBView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   SCROLLINFO scrollInfo;
   GetScrollInfo(SB_HORZ, &scrollInfo);

   int scrollPos;
   CCEtoODBDoc *doc = NULL;

   switch (nSBCode)
   {
   case SB_LEFT:
      scrollPos = GetDocument()->minXCoord;
      break;

   case SB_RIGHT:
      scrollPos = GetDocument()->maxXCoord;
      break;

   case SB_PAGELEFT:
      scrollPos = scrollInfo.nPos - (int)scrollInfo.nPage;
      break;

   case SB_PAGERIGHT:
      scrollPos = scrollInfo.nPos + (int)scrollInfo.nPage;
      break;

   case SB_LINELEFT:
      scrollPos = scrollInfo.nPos - (int)scrollInfo.nPage / 10;
      break;

   case SB_LINERIGHT:
      scrollPos = scrollInfo.nPos + (int)scrollInfo.nPage / 10;
      break;

   case SB_THUMBPOSITION:
      scrollPos = scrollInfo.nTrackPos;

      doc = GetDocument();
      if (doc->getSettings().HideDetail)
      {
         ThumbTracking = FALSE;
   
         SetXPos(scrollInfo.nTrackPos);

         Invalidate();
         return;

/*       CRect clientRect;
         GetClientRect(&clientRect);

         long unitsX, unitsY;
         Window_Shape(clientRect.Width(), clientRect.Height(), &unitsX, &unitsY, doc->maxXCoord - doc->minXCoord, doc->maxYCoord - doc->minYCoord);

         if (maxScrollPos - minScrollPos > unitsX / ScaleDenom)
         {
            Invalidate();
            return;
         }

         if (maxScrollPos > scrollPos) // redraw left side
         {
            CRect invalidRect = clientRect;

            invalidRect.right = (long)ceil(1.03 * invalidRect.right * (maxScrollPos - scrollPos) / unitsX * ScaleDenom);

            InvalidateRect(invalidRect, FALSE);
         }

         if (minScrollPos < scrollPos) // redraw right side
         {
            CRect invalidRect = clientRect;

            invalidRect.left = invalidRect.right - (long)ceil(1.03 * invalidRect.right * (scrollPos - minScrollPos) / unitsX * ScaleDenom);

            InvalidateRect(invalidRect, FALSE);
         }

         return;*/
      }
      break;

   case SB_THUMBTRACK:
      scrollPos = scrollInfo.nTrackPos;

      if (!ThumbTracking && (CCEtoODBDoc*)GetDocument()->getSettings().HideDetail)
      {
         ThumbTracking = TRUE;

         minScrollPos = maxScrollPos = scrollPos;
      }

      if (scrollPos < minScrollPos)
         minScrollPos = scrollPos;
      
      if (scrollPos > maxScrollPos)
         maxScrollPos = scrollPos;

      break;

   case SB_ENDSCROLL:
      return;

   default:
      scrollPos = scrollInfo.nPos;
      break;
   }

   SetXPos(scrollPos);

   getApp().getViewSynchronizer().setZoom(this,scrollPos,GetYPos(),ScaleNum,ScaleDenom);
}

/******************************************************************************
* OnVScroll
*/
void CCEtoODBView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   SCROLLINFO scrollInfo;
   GetScrollInfo(SB_VERT, &scrollInfo);

   int scrollPos = scrollInfo.nPos; 

   switch (nSBCode)
   {
   case SB_TOP:
      scrollPos = GetDocument()->minYCoord;
      break;

   case SB_BOTTOM:
      scrollPos = GetDocument()->maxYCoord;
      break;

   case SB_PAGEDOWN:
      scrollPos = scrollInfo.nPos + (int)scrollInfo.nPage;
      break;

   case SB_PAGEUP:
      scrollPos = scrollInfo.nPos - (int)scrollInfo.nPage;
      break;

   case SB_LINEDOWN:
      scrollPos = scrollInfo.nPos + (int)scrollInfo.nPage / 10;
      break;

   case SB_LINEUP:
      scrollPos = scrollInfo.nPos - (int)scrollInfo.nPage / 10;
      break;

   case SB_THUMBPOSITION:
      {
         scrollPos = scrollInfo.nTrackPos;
         int actualScrollPos = -scrollPos + scrollInfo.nMax + scrollInfo.nMin;

         CCEtoODBDoc *doc = GetDocument();
         if (doc->getSettings().HideDetail)
         {
            ThumbTracking = FALSE;
      
            SetYPos(actualScrollPos);

            Invalidate();
            return;
 /*
            CRect clientRect;
            GetClientRect(&clientRect);

            long unitsX, unitsY;
            Window_Shape(clientRect.Width(), clientRect.Height(), &unitsX, &unitsY, doc->maxXCoord - doc->minXCoord, doc->maxYCoord - doc->minYCoord);

            if (maxScrollPos - minScrollPos > unitsY / ScaleDenom)
            {
               Invalidate();
               return;
            }

            if (maxScrollPos > actualScrollPos) // redraw bottom
            {
               CRect invalidRect = clientRect;

               invalidRect.top = invalidRect.bottom - (long)ceil(1.03 * invalidRect.bottom * (maxScrollPos - actualScrollPos) / unitsY * ScaleDenom);

               InvalidateRect(invalidRect, FALSE);
            }

            if (minScrollPos < actualScrollPos) // redraw top
            {
               CRect invalidRect = clientRect;

               invalidRect.bottom =(long)ceil(1.03 * invalidRect.bottom * (actualScrollPos - minScrollPos) / unitsY * ScaleDenom);

               InvalidateRect(invalidRect, FALSE);
            }

            return;*/
         }
      }
      break;

   case SB_THUMBTRACK:
      {
         scrollPos = scrollInfo.nTrackPos;
         int actualScrollPos = -scrollPos + scrollInfo.nMax + scrollInfo.nMin;

         if (!ThumbTracking && (CCEtoODBDoc*)GetDocument()->getSettings().HideDetail)
         {
            ThumbTracking = TRUE;

            minScrollPos = maxScrollPos = actualScrollPos;
         }

         if (actualScrollPos < minScrollPos)
            minScrollPos = actualScrollPos;
         
         if (actualScrollPos > maxScrollPos)
            maxScrollPos = actualScrollPos;
      }
      break;

   case SB_ENDSCROLL:
      return;

   default:
      scrollPos = scrollInfo.nPos;
      break;
   }

   scrollPos = -scrollPos + scrollInfo.nMax + scrollInfo.nMin;

   SetYPos(scrollPos);

   getApp().getViewSynchronizer().setZoom(this,GetXPos(),scrollPos,ScaleNum,ScaleDenom);
}


/******************************************************************************
* GetXPos
*/
int CCEtoODBView::GetXPos()
{ 
   SCROLLINFO scrollInfo;
   GetScrollInfo(SB_HORZ, &scrollInfo);

   return scrollInfo.nPos;
}

/******************************************************************************
* GetYPos
*/
int CCEtoODBView::GetYPos()
{ 
   SCROLLINFO scrollInfo;
   GetScrollInfo(SB_VERT, &scrollInfo);

   return -scrollInfo.nPos + scrollInfo.nMax + scrollInfo.nMin;
}

/******************************************************************************
* SetXPos
*/
void CCEtoODBView::SetXPos(int scrollPos)
{  
   CCEtoODBDoc *doc = GetDocument();

   SCROLLINFO scrollInfo;
   GetScrollInfo(SB_HORZ, &scrollInfo);

   int lastScrollPos = scrollInfo.nPos;

   if (scrollPos > scrollInfo.nMax - (int)scrollInfo.nPage + 1)
      scrollPos = scrollInfo.nMax - (int)scrollInfo.nPage + 1;
   if (scrollPos < scrollInfo.nMin)
      scrollPos = scrollInfo.nMin;

   if (scrollPos == lastScrollPos)
      return;

   scrollInfo.fMask = SIF_POS;
   scrollInfo.nPos = scrollPos;
   SetScrollInfo(SB_HORZ, &scrollInfo);

   CClientDC dc(this);
   //OnPrepareDC(&dc);

   CSize size;                    
   size.cx = lastScrollPos - scrollPos;
   size.cy = 0;
   dc.LPtoDP(&size);
   ScrollWindow(size.cx, 0);
}

/******************************************************************************
* SetYPos
*/
void CCEtoODBView::SetYPos(int scrollPos)
{ 
   CCEtoODBDoc *doc = GetDocument();

   SCROLLINFO scrollInfo;
   GetScrollInfo(SB_VERT, &scrollInfo);

   scrollPos = -scrollPos + scrollInfo.nMax + scrollInfo.nMin;

   int lastScrollPos = scrollInfo.nPos;

   if (scrollPos > scrollInfo.nMax - (int)scrollInfo.nPage + 1)
      scrollPos = scrollInfo.nMax - (int)scrollInfo.nPage + 1;
   if (scrollPos < scrollInfo.nMin)
      scrollPos = scrollInfo.nMin;

   if (scrollPos == lastScrollPos)
      return;

   scrollInfo.fMask = SIF_POS;
   scrollInfo.nPos = scrollPos;
   SetScrollInfo(SB_VERT, &scrollInfo);

   CClientDC dc(this);
   //OnPrepareDC(&dc);

   CSize size;                    
   size.cx = 0;
   size.cy = lastScrollPos - scrollPos;
   dc.LPtoDP(&size);
   ScrollWindow(0, size.cy);
}

/******************************************************************************
* OnZoom1To1
*/
void CCEtoODBView::OnZoom1To1() 
{
   ScaleNum = 1;
   ScaleDenom = 1.0;
   UpdateScale();

   int xpos = GetDocument()->minXCoord;
   int ypos = GetDocument()->minYCoord;

   SetXPos(xpos);
   SetYPos(ypos);
   
   Invalidate();
   
   zoomFirstDrag = TRUE;
   cursorFirstDrag = TRUE;

   getApp().getViewSynchronizer().setZoom(this,xpos,ypos,ScaleNum,ScaleDenom);
}

/******************************************************************************
* OnZoomIn
*/
void CCEtoODBView::OnZoomIn() 
{
   if (zoomMode == ZoomIn) // turn off zoom
   {
      if (!zoomFirstDrag)
      {
         CClientDC dc(this);
         //OnPrepareDC(&dc);
         dc.SetROP2(R2_NOT);
         dc.SelectObject(GetStockObject(HOLLOW_BRUSH));
         dc.Rectangle(zoomPrevPnt.x - zoomBoxX, zoomPrevPnt.y + zoomBoxY, 
               zoomPrevPnt.x + zoomBoxX, zoomPrevPnt.y - zoomBoxY);
      }

      zoomMode = NoZoom;
      SetCursor(hCursor = AfxGetApp()->LoadCursor(IDC_CROSSHAIR));
      zoomFirstDrag = TRUE;

      return;
   }

   if (ScaleNum > 1)
   {
      ScaleNum = (int)ceil(1.0 * ScaleNum / 2);

      UpdateScale();

      Invalidate();

      getApp().getViewSynchronizer().setZoom(this,GetXPos(),GetYPos(),ScaleNum,ScaleDenom);

      return;
   }

   // find size of client window for zoom box
   zoomBoxX = zoomBoxY = 0;
   RECT clientRect;
   GetClientRect(&clientRect);
   Window_Shape(clientRect.right, clientRect.bottom, &zoomBoxX, &zoomBoxY, 
         GetDocument()->maxXCoord - GetDocument()->minXCoord, GetDocument()->maxYCoord - GetDocument()->minYCoord);
   zoomBoxX = (long)(1.0 * zoomBoxX / 4 / ScaleDenom);
   zoomBoxY = (long)(1.0 * zoomBoxY / 4 / ScaleDenom);

   if (zoomBoxX > (GetDocument()->maxXCoord-GetDocument()->minXCoord)) zoomBoxX = GetDocument()->maxXCoord-GetDocument()->minXCoord;
   if (zoomBoxY > (GetDocument()->maxYCoord-GetDocument()->minYCoord)) zoomBoxY = GetDocument()->maxYCoord-GetDocument()->minYCoord;

   zoomFirstDrag = TRUE;

   zoomMode = ZoomIn;
   SetCursor(hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMIN));
}

void CCEtoODBView::OnMouseMove_ZoomIn(CDC *dc, CPoint *pnt)
{
   SetCursor(hCursor);
   dc->SetROP2(R2_NOT);
   dc->SelectObject(GetStockObject(HOLLOW_BRUSH));
   
   if (!zoomFirstDrag)
      dc->Rectangle(zoomPrevPnt.x - zoomBoxX, zoomPrevPnt.y + zoomBoxY, 
            zoomPrevPnt.x + zoomBoxX, zoomPrevPnt.y - zoomBoxY);

   zoomFirstDrag = FALSE;
   zoomPrevPnt.x = pnt->x;
   zoomPrevPnt.y = pnt->y;

   dc->Rectangle(pnt->x - zoomBoxX, pnt->y + zoomBoxY, 
         pnt->x + zoomBoxX, pnt->y - zoomBoxY);
}

void CCEtoODBView::OnLButtonDown_ZoomIn(CDC *dc, CPoint *pnt)
{
   long x, y;
   x = pnt->x - zoomBoxX;
   if (x + zoomBoxX > GetDocument()->maxXCoord)
      x = GetDocument()->maxXCoord - zoomBoxX*2;
   if (x < GetDocument()->minXCoord)
      x = GetDocument()->minXCoord;

   y = pnt->y + zoomBoxY;
   if (y - zoomBoxY < GetDocument()->minYCoord)
      y = GetDocument()->minYCoord + zoomBoxY*2;
   if (y > GetDocument()->maxYCoord)
      y = GetDocument()->maxYCoord;
   
   ScaleDenom *= 2.0;
   UpdateScale();

   SetXPos((int)x);
   SetYPos((int)y);

   Invalidate();

   zoomMode = NoZoom;
   cursorFirstDrag = TRUE;
}

void CCEtoODBView::OnZoominHotkey() 
{
   zoomMode = NoZoom;
   cursorFirstDrag = TRUE;

   if (ScaleNum > 1)
   {
      ScaleNum = (int)ceil(1.0 * ScaleNum / 2);
   }
   else
   {
      RECT clientRect;
      long xUnits, yUnits;
      GetClientRect(&clientRect);
      Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
            GetDocument()->maxXCoord - GetDocument()->minXCoord, GetDocument()->maxYCoord - GetDocument()->minYCoord);

      int xPos = (GetXPos() + ((int)((1.0 * xUnits / ScaleDenom) / 2)) / 2);
      int yPos = (GetYPos() - ((int)((1.0 * yUnits / ScaleDenom) / 2)) / 2);

      ScaleDenom *= 2.0;
      SetScroll();
      SetXPos(xPos);
      SetYPos(yPos);

      UpdateScale();
   }

   UpdateScale();

   Invalidate();
}

/******************************************************************************
* OnZoomOut
*/
void CCEtoODBView::OnZoomOut() 
{
   zoomMode = NoZoom;
   cursorFirstDrag = TRUE;

   if (ScaleDenom > 2.0)
   {
      RECT clientRect;
      long xUnits, yUnits;
      GetClientRect(&clientRect);
      Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
            GetDocument()->maxXCoord - GetDocument()->minXCoord, GetDocument()->maxYCoord - GetDocument()->minYCoord);
      SetXPos(GetXPos() - ((int)(1.0 * xUnits / ScaleDenom / 2)));
      SetYPos(GetYPos() + ((int)(1.0 * yUnits / ScaleDenom / 2)));

      ScaleDenom /= 2.0;
   }
   else if (ScaleDenom > 1.0)
   {
      ScaleDenom = 1.0;
      SetScrollPos(SB_HORZ, GetDocument()->minXCoord);
      SetScrollPos(SB_VERT, 0);
   }
   else
   {
      ScaleNum *= 2;
   }
      
   UpdateScale();

   Invalidate();
   zoomFirstDrag = TRUE;

   getApp().getViewSynchronizer().setZoom(this,GetXPos(),GetYPos(),ScaleNum,ScaleDenom);
}

/******************************************************************************
* ZoomBox
*/
void CCEtoODBView::ZoomBox(double xmin, double xmax, double ymin, double ymax) 
{
   RECT clientRect;
   long xUnits, yUnits;
   GetClientRect(&clientRect);
   Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
         GetDocument()->maxXCoord - GetDocument()->minXCoord, GetDocument()->maxYCoord - GetDocument()->minYCoord);

   double factor = (GetDocument()->maxXCoord-GetDocument()->minXCoord) / (GetDocument()->getSettings().getXmax()-GetDocument()->getSettings().getXmin());
   double scaleX = 1.0 * xUnits / ((xmax - xmin) * factor);
   double scaleY = 1.0 * yUnits / ((ymax - ymin) * factor);

   ScaleDenom = min(scaleX, scaleY);
   ScaleNum = 1;

   // zoom 1:1 if page size is extents of drawing
   if (fabs(ScaleDenom - 1) < 0.01)
   {
      ScaleNum = 1; 
      ScaleDenom = 1.0;
      ShowScrollBar(SB_BOTH, FALSE);
      SetScrollPos(SB_HORZ, GetDocument()->minXCoord);
      SetScrollPos(SB_VERT, 0);
   }

   // zoomed out from 1:1
   else if (ScaleDenom < 1.0) 
   {
      ScaleNum = (int)ceil(1.0 / ScaleDenom); 
      ScaleDenom = 1.0;
      ShowScrollBar(SB_BOTH, TRUE);
      SetScrollPos(SB_HORZ, GetDocument()->minXCoord);
      SetScrollPos(SB_VERT, 0);
   }

   UpdateScale();

   Invalidate();

   if (ScaleDenom > 1.0001)
   {
      ShowScrollBar(SB_BOTH, TRUE);
      PanCoordinate((xmax+xmin)/2, (ymax+ymin)/2, FALSE);
   }

   getApp().getViewSynchronizer().setZoom(this,GetXPos(),GetYPos(),ScaleNum,ScaleDenom);
}

void CCEtoODBView::setZoom(int xPos,int yPos,int scaleNumerator,double scaleDenominator)
{
   ScaleNum   = scaleNumerator;
   ScaleDenom = scaleDenominator;
   UpdateScale();

   SetXPos(xPos);
   SetYPos(yPos);

   Invalidate();
}

/******************************************************************************
* ZoomExtents
*/
void CCEtoODBView::OnZoomExtents() 
{
   double xmin, xmax, ymin, ymax;

   static bool testFlag = true;

   if (testFlag)
   {
      GetDocument()->resetBlockExtents();
   }

   GetDocument()->get_extents(&xmin, &xmax, &ymin, &ymax);

   if (xmax <= xmin)
   {
      OnZoom1To1();
      return;
   }

   ZoomBox(xmin, xmax, ymin, ymax);

   zoomMode = NoZoom;
   zoomFirstDrag = TRUE;
   cursorFirstDrag = TRUE;

   getApp().getViewSynchronizer().setZoom(this,GetXPos(),GetYPos(),ScaleNum,ScaleDenom);
}

/******************************************************************************
* ZoomWindow
*/
void CCEtoODBView::OnZoomWindow() 
{
   if (zoomMode == ZoomWindow) // turn off zoom
   {
      if (zoomAnchored)
      {
         CClientDC dc(this);
         //OnPrepareDC(&dc);
         dc.SetROP2(R2_NOT);
         dc.SelectObject(GetStockObject(HOLLOW_BRUSH));
         int top, left, bottom, right;
         top = max(zoomPrevPnt.x, zoomAnchorPnt.x);
         bottom = min(zoomPrevPnt.x, zoomAnchorPnt.x);
         right = max(zoomPrevPnt.y, zoomAnchorPnt.y);
         left = min(zoomPrevPnt.y, zoomAnchorPnt.y);
         dc.Rectangle(top, left, bottom, right);
      }
      zoomMode = NoZoom;
      return;
   }

   zoomMode = ZoomWindow;
   zoomAnchored = FALSE;
   SetCursor(hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMWINDOW));
}

void CCEtoODBView::OnMouseMove_ZoomWindow(CDC *dc, CPoint *pnt)
{
   int top, left, bottom, right;

   SetCursor(hCursor);

   if (!zoomAnchored) return;

   dc->SetROP2(R2_NOT);
   dc->SelectObject(GetStockObject(HOLLOW_BRUSH));

   right = max(zoomPrevPnt.x, zoomAnchorPnt.x);
   left = min(zoomPrevPnt.x, zoomAnchorPnt.x);
   top = max(zoomPrevPnt.y, zoomAnchorPnt.y);
   bottom = min(zoomPrevPnt.y, zoomAnchorPnt.y);
   dc->Rectangle(left, top, right, bottom);

   zoomPrevPnt.x = pnt->x;
   zoomPrevPnt.y = pnt->y;

   right = max(zoomPrevPnt.x, zoomAnchorPnt.x);
   left = min(zoomPrevPnt.x, zoomAnchorPnt.x);
   top = max(zoomPrevPnt.y, zoomAnchorPnt.y);
   bottom = min(zoomPrevPnt.y, zoomAnchorPnt.y);
   dc->Rectangle(left, top, right, bottom);
}

void CCEtoODBView::OnLButtonDown_ZoomWindow(CDC *dc, CPoint *pnt)
{
   if (!zoomAnchored)
   {
      zoomAnchorPnt.x = zoomPrevPnt.x = pnt->x;
      zoomAnchorPnt.y = zoomPrevPnt.y = pnt->y;
      zoomAnchored = TRUE;
   }
   else
      OnLButtonUp_ZoomWindow(dc, pnt);
}

void CCEtoODBView::OnLButtonUp_ZoomWindow(CDC *dc, CPoint *pnt)
{
   double scaleX, scaleY;
   RECT clientRect;
   long xUnits, yUnits;

   if (zoomAnchorPnt.x == pnt->x || zoomAnchorPnt.y == pnt->y) return;

   GetClientRect(&clientRect);
   Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
         GetDocument()->maxXCoord - GetDocument()->minXCoord, GetDocument()->maxYCoord - GetDocument()->minYCoord);

   scaleX = 1.0 * xUnits / (1.0*abs(pnt->x - zoomAnchorPnt.x)) / ScaleNum;
   scaleY = 1.0 * yUnits / (1.0*abs(pnt->y - zoomAnchorPnt.y)) / ScaleNum;

   ScaleNum = 1;
   ScaleDenom = min(scaleX, scaleY);
   UpdateScale();

   int xpos = (int)((pnt->x + zoomAnchorPnt.x) / 2 - ((int)(1.0 * xUnits / ScaleDenom / 2)));
   int ypos = (int)((pnt->y + zoomAnchorPnt.y) / 2 + ((int)(1.0 * yUnits / ScaleDenom / 2)));

   SetXPos(xpos);
   SetYPos(ypos);

   Invalidate();

   zoomMode = NoZoom;
   zoomFirstDrag = TRUE;
   cursorFirstDrag = TRUE;

   getApp().getViewSynchronizer().setZoom(this,xpos,ypos,ScaleNum,ScaleDenom);
}

/******************************************************************************
* OnPan
*/
void CCEtoODBView::OnPan() 
{
   if (ScaleDenom == 1.0)
      return;

   //HideSearchCursor();

   if (zoomMode == Pan)
      zoomMode = NoZoom;
   else
      zoomMode = Pan;

   //ShowSearchCursor();
}

void CCEtoODBView::OnLButtonDown_Pan(CDC *dc, CPoint *pnt)
{
   RECT clientRect;
   long xUnits, yUnits;
   GetClientRect(&clientRect);
   Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
         GetDocument()->maxXCoord - GetDocument()->minXCoord, GetDocument()->maxYCoord - GetDocument()->minYCoord);

   int xpos = pnt->x - ((int)(1.0 * xUnits / ScaleDenom / 2));
   int ypos = pnt->y + ((int)(1.0 * yUnits / ScaleDenom / 2));

   SetXPos(xpos);
   SetYPos(ypos);

   Invalidate();
   zoomMode = NoZoom;
   cursorFirstDrag = TRUE;

   getApp().getViewSynchronizer().setZoom(this,xpos,ypos,ScaleNum,ScaleDenom);
}

void CCEtoODBView::showCross(double x,double y)
{
	CCEtoODBDoc *doc = GetDocument();
	RECT clientRect;
	long xUnits, yUnits;
	GetClientRect(&clientRect);
	Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
			doc->maxXCoord - doc->minXCoord, doc->maxYCoord - doc->minYCoord);

   FlushQueue();

   int size = round(1.0 * xUnits / ScaleDenom / 20);
   CClientDC dc(this);
   //OnPrepareDC(&dc);
   dc.SelectStockObject(WHITE_PEN);
   dc.MoveTo(round(x * getScaleFactor() + size), round(y * getScaleFactor()));
   dc.LineTo(round(x * getScaleFactor() - size), round(y * getScaleFactor()));
   dc.MoveTo(round(x * getScaleFactor()), round(y * getScaleFactor() + size));
   dc.LineTo(round(x * getScaleFactor()), round(y * getScaleFactor() - size));

   // show a rotated box around it.
   dc.MoveTo(round(x * getScaleFactor() + size/2), round(y * getScaleFactor()));
   dc.LineTo(round(x * getScaleFactor()), round(y * getScaleFactor() + size/2));
   dc.LineTo(round(x * getScaleFactor() - size/2), round(y * getScaleFactor()));
   dc.LineTo(round(x * getScaleFactor()), round(y * getScaleFactor() - size/2));
   dc.LineTo(round(x * getScaleFactor() + size/2), round(y * getScaleFactor()));

   // show a rotated box around it.
   dc.MoveTo(round(x * getScaleFactor() + size/1.2), round(y * getScaleFactor()));
   dc.LineTo(round(x * getScaleFactor()), round(y * getScaleFactor() + size/1.2));
   dc.LineTo(round(x * getScaleFactor() - size/1.2), round(y * getScaleFactor()));
   dc.LineTo(round(x * getScaleFactor()), round(y * getScaleFactor() - size/1.2));
   dc.LineTo(round(x * getScaleFactor() + size/1.2), round(y * getScaleFactor()));
}

void CCEtoODBView::PanCoordinate(double x, double y, BOOL ShowCross)
{
	CCEtoODBDoc *doc = GetDocument();
	double factor = (doc->maxXCoord - doc->minXCoord) / (doc->getSettings().getXmax() - doc->getSettings().getXmin());
	RECT clientRect;
	long xUnits, yUnits;
	GetClientRect(&clientRect);
	Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
			doc->maxXCoord - doc->minXCoord, doc->maxYCoord - doc->minYCoord);

   int xpos = (int)(x * factor - ((int)(1.0 * xUnits / ScaleDenom / 2)));
   int ypos = (int)(y * factor + ((int)(1.0 * yUnits / ScaleDenom / 2)));

	SetXPos(xpos);
	SetYPos(ypos);

	doc->UpdateAllViews(NULL);

   getApp().getViewSynchronizer().setZoom(this,xpos,ypos,ScaleNum,ScaleDenom);

   if (ShowCross)
   {
      showCross(x,y);
      //FlushQueue();

      //int size = round(1.0 * xUnits / ScaleDenom / 20);
      //CClientDC dc(this);
      //OnPrepareDC(&dc);
      //dc.SelectStockObject(WHITE_PEN);
      //dc.MoveTo(round(x * scaleFactor + size), round(y * scaleFactor));
      //dc.LineTo(round(x * scaleFactor - size), round(y * scaleFactor));
      //dc.MoveTo(round(x * scaleFactor), round(y * scaleFactor + size));
      //dc.LineTo(round(x * scaleFactor), round(y * scaleFactor - size));

      //// show a rotated box around it.
      //dc.MoveTo(round(x * scaleFactor + size/2), round(y * scaleFactor));
      //dc.LineTo(round(x * scaleFactor), round(y * scaleFactor + size/2));
      //dc.LineTo(round(x * scaleFactor - size/2), round(y * scaleFactor));
      //dc.LineTo(round(x * scaleFactor), round(y * scaleFactor - size/2));
      //dc.LineTo(round(x * scaleFactor + size/2), round(y * scaleFactor));

      //// show a rotated box around it.
      //dc.MoveTo(round(x * scaleFactor + size/1.2), round(y * scaleFactor));
      //dc.LineTo(round(x * scaleFactor), round(y * scaleFactor + size/1.2));
      //dc.LineTo(round(x * scaleFactor - size/1.2), round(y * scaleFactor));
      //dc.LineTo(round(x * scaleFactor), round(y * scaleFactor - size/1.2));
      //dc.LineTo(round(x * scaleFactor + size/1.2), round(y * scaleFactor));
   }
}

void CCEtoODBView::OnPanCoordinate() 
{
   PanToCoordDialog dlg;
   if (dlg.DoModal() == IDOK)
      PanCoordinate(dlg.m_x, dlg.m_y, TRUE);
}

/******************************************************************************
* OnPanToText
*/
void CCEtoODBView::OnPanToText() 
{
   PanToTextDialog dlg;
   dlg.m_case = CaseSensitive;
   dlg.m_substring = SubString;
   dlg.m_text_ent = FindTexts;
   dlg.m_attribs = FindAttribs;
   if (dlg.DoModal() != IDOK) return;
   CaseSensitive = dlg.m_case;
   SubString = dlg.m_substring;
   FindTexts = dlg.m_text_ent;
   FindAttribs = dlg.m_attribs;
   searchText = dlg.m_text;

   PanToText();
}

/******************************************************************************
* PanToText
*/
void CCEtoODBView::PanToText()
{
   CCEtoODBDoc *doc = GetDocument();
   FileStruct *file;
   DataStruct *data;

   textFilePos = doc->getFileList().GetHeadPosition();
   while (textFilePos != NULL)
   {
      file = doc->getFileList().GetAt(textFilePos);
      if (file->isShown())
      {
         textDataPos = file->getBlock()->getDataList().GetHeadPosition();
         while (textDataPos != NULL)
         {
            data = file->getBlock()->getDataList().GetNext(textDataPos);

            if (FindTexts && data->getDataType() == T_TEXT)
            {
               if (DoTextPan(data->getText()->getText(), data, file, FALSE))
                  return;
            }

            // fix ws - must have attribute != NULL 
            if (FindAttribs && data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
            {
               WORD keyword;
               Attrib* attrib;
               
               for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
               {
                  data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

                  if (attrib->getValueType() != VT_STRING) continue;

                  if (!attrib->isVisible()) continue;

                  if (DoTextPan(attrib->getStringValue(), data, file, TRUE))
                     return;
               }
            }
         }
      }

      doc->getFileList().GetNext(textFilePos);
   }
}

/******************************************************************************
* DoTextPan
*/
int CCEtoODBView::DoTextPan(const char *text, DataStruct *data, FileStruct *file, BOOL Attrib) 
{
   CCEtoODBDoc *doc = GetDocument();

   if (SubString)
   {
      if (!CaseSensitive)
      {
         CString t1 = text;
         CString t2 = searchText;
         t1.MakeUpper();
         t2.MakeUpper();
         if (!strstr(t1,t2))
            return 0;
      }
      else
      {
         if (!strstr(text, searchText))
            return 0;
      }
   }
   else if (CaseSensitive)
   {
      if (strcmp(text, searchText))
         return 0;
   }
   else 
   {
      if (STRCMPI(text, searchText))
         return 0;
   }
   
   doc->InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), 
         file->getScale(), file->getRotation(), file->isMirrored(), &(file->getBlock()->getDataList()), FALSE, TRUE);

   double factor = (doc->maxXCoord - doc->minXCoord) /
         (doc->getSettings().getXmax() - doc->getSettings().getXmin());
   RECT clientRect;
   long xUnits, yUnits;
   GetClientRect(&clientRect);
   Window_Shape(clientRect.right, clientRect.bottom, &xUnits, &yUnits, 
         doc->maxXCoord - doc->minXCoord, doc->maxYCoord - doc->minYCoord);
   Point2 pnt;
   if (!Attrib)
   {
      pnt.x = data->getText()->getPnt().x;
      pnt.y = data->getText()->getPnt().y;
   }
   else
   {
      switch (data->getDataType())
      {
      case T_INSERT:
         pnt.x = data->getInsert()->getOriginX();
         pnt.y = data->getInsert()->getOriginY();
      }
   }

   Mat2x2 m;
   RotMat2(&m, file->getRotation());
   pnt.x *= file->getScale();
   if (file->isMirrored()) pnt.x = -pnt.x;
   pnt.y *= file->getScale();
   TransPoint2(&pnt, 1, &m, file->getInsertX(), file->getInsertY());

   int xpos = (int)(pnt.x * factor - ((int)(1.0 * xUnits / ScaleDenom / 2)));
   int ypos = (int)(pnt.y * factor + ((int)(1.0 * yUnits / ScaleDenom / 2)));

   SetXPos(xpos);
   SetYPos(ypos);

   Invalidate();

   getApp().getViewSynchronizer().setZoom(this,xpos,ypos,ScaleNum,ScaleDenom);

   return 1;
}

void CCEtoODBView::OnNextText() 
{
   CCEtoODBDoc *doc = GetDocument();
   FileStruct *file;
   DataStruct *data;

   // continue with last file
   while (textFilePos != NULL)
   {
      file = doc->getFileList().GetAt(textFilePos);

      if (file->isShown())
      {
         while (textDataPos != NULL)
         {
            data = file->getBlock()->getDataList().GetNext(textDataPos);
   
            if (FindTexts && data->getDataType() == T_TEXT)
            {
               if (DoTextPan(data->getText()->getText(), data, file, FALSE))
                  return;
            }
            // same fix here
            if (FindAttribs && data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
            {
               WORD keyword;
               Attrib* attrib;
               
               for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
               {
                  data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

                  if (attrib->getValueType() != VT_STRING) continue;

                  if (!attrib->isVisible()) continue;

                  if (DoTextPan(attrib->getStringValue(), data, file, TRUE))
                     return;
               }
            }
         }
      }

      doc->getFileList().GetNext(textFilePos);
   }

   // if not found, restart at begining of FileList
   textFilePos = doc->getFileList().GetHeadPosition();
   while (textFilePos != NULL)
   {
      file = doc->getFileList().GetAt(textFilePos);

      if (file->isShown())
      {
         textDataPos = file->getBlock()->getDataList().GetHeadPosition();
         while (textDataPos != NULL)
         {
            data = file->getBlock()->getDataList().GetNext(textDataPos);

            if (FindTexts && data->getDataType() == T_TEXT)
            {
               if (DoTextPan(data->getText()->getText(), data, file, FALSE))
                  return;
            }

            // same fix here
            if (FindAttribs && data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
            {
               WORD keyword;
               Attrib* attrib;
               
               for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
               {
                  data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

                  if (attrib->getValueType() != VT_STRING) continue;

                  if (!attrib->isVisible()) continue;

                  if (DoTextPan(attrib->getStringValue(), data, file, TRUE))
                     return;
               }
            }
         }
      }

      doc->getFileList().GetNext(textFilePos);
   }
}

/******************************************************************************
* OnMoveOrigin
*/
void CCEtoODBView::OnMoveOrigin() 
{
   if (zoomMode != NoZoom) 
      return;

   if (cursorMode == NewOrg)
   {
      cursorMode = Search;
      return;
   }

   cursorFirstDrag = TRUE;
   cursorMode = NewOrg;
}

void CCEtoODBView::OnMouseMove_MoveOrigin(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();
   SetCursor(NULL);

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      dc->MoveTo(doc->minXCoord, cursorPrevPnt.y);
      dc->LineTo(doc->maxXCoord, cursorPrevPnt.y);
      dc->MoveTo(cursorPrevPnt.x, doc->minYCoord);
      dc->LineTo(cursorPrevPnt.x, doc->maxYCoord);
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(doc->minXCoord, cursorPrevPnt.y);
   dc->LineTo(doc->maxXCoord, cursorPrevPnt.y);
   dc->MoveTo(cursorPrevPnt.x, doc->minYCoord);
   dc->LineTo(cursorPrevPnt.x, doc->maxYCoord);
}

void CCEtoODBView::OnLButtonDown_MoveOrigin(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;

   if (pnt->x < doc->minXCoord || pnt->x > doc->maxXCoord || 
         pnt->y < doc->minYCoord || pnt->y > doc->maxYCoord)
   {
      cursorMode = Search;
      return;
   }

   double x = pnt->x * factor;
   if (doc->getBottomView())
      x = -x;
   doc->MoveOrigin(x, pnt->y * factor);

   cursorMode = Search;
}

void CCEtoODBDoc::MoveOrigin(double x, double y)
{
   double _x = x;
   if (getBottomView())
      _x = -_x;

   UnselectAll(FALSE);

   double factor = (getSettings().getXmax() - getSettings().getXmin()) / (maxXCoord - minXCoord);

   minXCoord -= round(_x / factor);
   maxXCoord -= round(_x / factor);
   minYCoord -= round(y / factor);
   maxYCoord -= round(y / factor);
                                                        
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *fp = getFileList().GetNext(filePos);
      if (!fp->isShown() || fp->isHidden()) continue;
      fp->setInsertX(fp->getInsertX() - (DbUnit)x);
      fp->setInsertY(fp->getInsertY() - (DbUnit)y);
   }
   
   getSettings().setXmin(getSettings().getXmin() - _x);
   getSettings().setXmax(getSettings().getXmax() - _x);
   getSettings().setYmin(getSettings().getYmin() -  y);
   getSettings().setYmax(getSettings().getYmax() -  y);

   POSITION viewPos = GetFirstViewPosition();
   while (viewPos)
   {
      CCEtoODBView *view = (CCEtoODBView *)GetNextView(viewPos);

      view->ScaleNum = 1;
      view->ScaleDenom = 1.0;
      view->UpdateScale();

      view->FilterFirst = TRUE;
   }

   UpdateAllViews(NULL);
}

/******************************************************************************
* OnMarkRect
*/
void CCEtoODBView::OnMarkRect() 
{
   ResetButtons();
   if (cursorMode == MarkRect)
   {
      cursorMode = Search;
      return;
   }

   //HideSearchCursor();
   cursorMode = MarkRect;
   //ShowSearchCursor();
   cursorAnchored = FALSE;
}

/******************************************************************************
* OnMarkRect_CrossCheck
*/
void CCEtoODBView::OnMarkRect_CrossCheck() 
{
   ResetButtons();
   if (cursorMode == MarkRect_CrossCheck)
   {
      cursorMode = Search;
      return;
   }

   //HideSearchCursor();
   cursorMode = MarkRect_CrossCheck;
   //ShowSearchCursor();
   cursorAnchored = FALSE;
}

void CCEtoODBView::OnMouseMove_MarkRect(CDC *dc, CPoint *pnt)
{
   int top, left, bottom, right;

   if (!cursorAnchored) return;

   dc->SetROP2(R2_NOT);
   dc->SelectObject(GetStockObject(HOLLOW_BRUSH));

   right = max(cursorPrevPnt.x, cursorAnchor.x);
   left = min(cursorPrevPnt.x, cursorAnchor.x);
   top = max(cursorPrevPnt.y, cursorAnchor.y);
   bottom = min(cursorPrevPnt.y, cursorAnchor.y);
   dc->Rectangle(left, top, right, bottom);

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   right = max(cursorPrevPnt.x, cursorAnchor.x);
   left = min(cursorPrevPnt.x, cursorAnchor.x);
   top = max(cursorPrevPnt.y, cursorAnchor.y);
   bottom = min(cursorPrevPnt.y, cursorAnchor.y);
   dc->Rectangle(left, top, right, bottom);
}

void CCEtoODBView::OnLButtonDown_MarkRect(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored)
   {
      cursorAnchor.x = cursorPrevPnt.x = pnt->x;
      cursorAnchor.y = cursorPrevPnt.y = pnt->y;
      cursorAnchored = TRUE;
   }
   else
      OnLButtonUp_MarkRect(dc, pnt);
}

void CCEtoODBView::OnLButtonUp_MarkRect(CDC *dc, CPoint *pnt)
{
   if (cursorAnchor.x == pnt->x || cursorAnchor.y == pnt->y) return;

   int left, right, bottom, top;
   right = (int)max(cursorPrevPnt.x, cursorAnchor.x);
   left = (int)min(cursorPrevPnt.x, cursorAnchor.x);
   top = (int)max(cursorPrevPnt.y, cursorAnchor.y);
   bottom = (int)min(cursorPrevPnt.y, cursorAnchor.y);
   dc->SetROP2(R2_NOT);
   dc->SelectObject(GetStockObject(HOLLOW_BRUSH));
   dc->Rectangle(left, top, right, bottom);

   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
   doc->MarkRect(factor * min(pnt->x, cursorAnchor.x), factor * max(pnt->x, cursorAnchor.x), 
         factor * min(pnt->y, cursorAnchor.y), factor * max(pnt->y, cursorAnchor.y), cursorMode == MarkRect);

   //HideSearchCursor();
   cursorMode = Search;
   //ShowSearchCursor();
}

/////////////////////////////////////////////////////////////////////////////
// PanToTextDialog
PanToTextDialog::PanToTextDialog(CWnd* pParent /*=NULL*/) : CDialog(PanToTextDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(PanToTextDialog)
   m_case = FALSE;
   m_text = _T("");
   m_substring = FALSE;
   m_attribs = FALSE;
   m_text_ent = FALSE;
   //}}AFX_DATA_INIT
}

void PanToTextDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PanToTextDialog)
   DDX_Check(pDX, IDC_CASE_SENSITIVE, m_case);
   DDX_Text(pDX, IDC_TEXT, m_text);
   DDX_Check(pDX, IDC_SUBSTRING, m_substring);
   DDX_Check(pDX, IDC_ATTRIBS, m_attribs);
   DDX_Check(pDX, IDC_TEXT_ENT, m_text_ent);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PanToTextDialog, CDialog)
   //{{AFX_MSG_MAP(PanToTextDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PanToTextDialog message handlers
void PanToTextDialog::OnOK() 
{
   UpdateData();
   if (!m_attribs && !m_text_ent)
   {
      ErrorMessage("You must choose at least one: Text Entities and/or Attributes");
      return;
   }
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// PanToCoordDialog
PanToCoordDialog::PanToCoordDialog(CWnd* pParent /*=NULL*/) : CDialog(PanToCoordDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(PanToCoordDialog)
   m_x = 0.0;
   m_y = 0.0;
   //}}AFX_DATA_INIT
}

void PanToCoordDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PanToCoordDialog)
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PanToCoordDialog, CDialog)
   //{{AFX_MSG_MAP(PanToCoordDialog)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/******************************************************************************
* OnPanInsert
*/
void CCEtoODBDoc::OnPanInsert() 
{
   PanToInsertDialog dlg;
   dlg.doc = this;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// PanToInsertDialog
PanToInsertDialog::PanToInsertDialog(CWnd* pParent /*=NULL*/) : 
   CResizingDialog(PanToInsertDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(PanToInsertDialog)
   m_count = _T("");
   //}}AFX_DATA_INIT

   CResizingDialogField& fileField = addFieldControl(IDC_FILE_CB,anchorProportionalHorizontal);
   addFieldControl(IDC_FileStatic    ,anchorRelativeToField,&fileField);

   addFieldControl(IDC_PAN_TO        ,anchorRight);
   addFieldControl(IDOK              ,anchorRight);
   addFieldControl(IDC_COUNT         ,anchorRight);
   addFieldControl(IDC_STATIC1       ,anchorLeft);
   addFieldControl(IDC_STATIC2       ,anchorRelativeToField,&fileField);

   CResizingDialogField& typeField = addFieldControl(IDC_INSERTTYPES_LB,anchorLeft,growVertical);
   typeField.getOperations().addOperation(glueRightEdge,toLeftEdge,&fileField);

   CResizingDialogField& nameField = addFieldControl(IDC_INSERTS_LB,anchorRight,growVertical);
   nameField.getOperations().addOperation(glueLeftEdge,toLeftEdge,&fileField);
}

void PanToInsertDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PanToInsertDialog)
   DDX_Control(pDX, IDC_FILE_CB, m_fileCB);
   DDX_Control(pDX, IDC_INSERTTYPES_LB, m_inserttypeLB);
   DDX_Control(pDX, IDC_INSERTS_LB, m_insertLB);
   DDX_Text(pDX, IDC_COUNT, m_count);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PanToInsertDialog, CResizingDialog)
   //{{AFX_MSG_MAP(PanToInsertDialog)
   ON_BN_CLICKED(IDC_PAN_TO, OnPanTo)
   ON_LBN_SELCHANGE(IDC_INSERTTYPES_LB, OnSelChange)
   ON_CBN_SELCHANGE(IDC_FILE_CB, OnSelChange)
   ON_LBN_DBLCLK(IDC_INSERTS_LB, OnPanTo)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PanToInsertDialog message handlers
BOOL PanToInsertDialog::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      m_inserttypeLB.SetItemData(m_inserttypeLB.AddString(insertTypeToDisplayString(insertType)),insertType);
   }
   
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      if (!file->isShown() || file->isHidden())
         continue;

      m_fileCB.SetItemDataPtr(m_fileCB.AddString(file->getName()), file);
   }

   m_fileCB.SetCurSel(0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void PanToInsertDialog::OnSelChange() 
{
   m_insertLB.ResetContent();

   int inserttype = m_inserttypeLB.GetItemData(m_inserttypeLB.GetCurSel());
   int count = 0;

   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(m_fileCB.GetCurSel());
   if(!file || m_fileCB.GetCount() == 0)
      return;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == inserttype)
      {
         CString buf;
         buf.Format("%s [%s] (%ld)", doc->getBlockAt(data->getInsert()->getBlockNumber())->getName(), data->getInsert()->getRefname(), data->getEntityNumber());
         m_insertLB.SetItemDataPtr(m_insertLB.AddString(buf), data);
         count++;
      }
   }  

   m_count.Format("Count:%d", count);

   UpdateData(FALSE);
}

void PanToInsertDialog::OnPanTo() 
{
   int sel = m_insertLB.GetCurSel();
   if (sel == LB_ERR)
      return;

   DataStruct *data = (DataStruct*)m_insertLB.GetItemDataPtr(sel);
   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(m_fileCB.GetCurSel());

   if (data && file)
   {
      doc->InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), 
         file->getScale(), file->getRotation(), file->isMirrored(), &(file->getBlock()->getDataList()), FALSE, TRUE);
   }

   Point2 point;
   point.x = data->getInsert()->getOriginX();
   point.y = data->getInsert()->getOriginY();
   point.bulge = 0;

   DTransform xform;
   xform.x = file->getInsertX();
   if (doc->getBottomView())
      xform.x = -xform.x;
   xform.y = file->getInsertY();
   xform.scale = file->getScale();
   xform.mirror = (file->getResultantMirror(doc->getBottomView()) ? MIRROR_FLIP : 0);
   xform.SetRotation(file->getRotation());
   xform.TransformPoint(&point);

   ((CCEtoODBView*)activeView)->PanCoordinate(point.x, point.y, TRUE);
}

