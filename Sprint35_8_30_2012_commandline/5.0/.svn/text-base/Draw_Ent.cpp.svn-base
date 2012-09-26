// $Header: /CAMCAD/5.0/Draw_Ent.cpp 48    5/21/07 3:47p Kurt Van Ness $

/*****************************************************************************/
/*           
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/

#include "stdafx.h"
#include "draw.h"
#include "polylib.h"
#include "Font.h"

#define ARC_TO_LINE_THRESHOLD    100   // number of logical units (pixels) between endpoints of bulge to draw line instead of arc

BOOL AngleArc_Lines95(CDC *dc, int X, int Y, double radius, double sa, double da);
void DrawPolys(CDC *dc, CCEtoODBView *view, CPolyList *polylist, double scale, int mirror, 
      double insert_x, double insert_y, Mat2x2 *m, int layer, BOOL showFills, COLORREF voidColor);
void DrawPoly(CCEtoODBView *view, CDC *pDC, CPoly *poly, int PenWidth, int PenShape, 
      double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m, int layer, BOOL showFills);
void DrawLineWithRectangleAperture(CCEtoODBView *view, CDC *pDC, Point2 *start, Point2 *end, 
      double scale, BlockStruct *polyWidth);
void PrepareMemDC(CCEtoODBView *view, CDC *pDC, int background);
void FreeMemDC(CCEtoODBView *view, CDC *pDC);
BOOL DrawTransparent(HDC hdc, HBITMAP hBitmap, COLORREF crColor, int x, int y);
      
extern COLORREF PenColor; // from DRAW.CPP
extern BOOL Print_Reverse; // from PRINT.CPP
extern double userDefinedWidth; // from DRAW.CPP

static HPEN pen, origPen;
static HBRUSH brush, origBrush;
static COLORREF CurrPenColor, CurrBrushColor;
static int CurrPenWidth;
static CDC *memDC;
static CBitmap *memBitmap;

/****************************************************************************
* InitPens
*/
void InitPens(CDC *pDC, int fillStyle)
{
   CurrPenColor = CurrBrushColor = RGB(255, 255, 255);
   CurrPenWidth = 0;

   pen = CreatePen(PS_SOLID, CurrPenWidth, CurrPenColor);

   if (fillStyle == HS_SOLID)
      brush = CreateSolidBrush(CurrBrushColor);
   else
      brush = CreateHatchBrush(fillStyle, CurrBrushColor);

   origPen = (HPEN)SelectObject(pDC->m_hDC, pen);
   origBrush = (HBRUSH)SelectObject(pDC->m_hDC, brush);
}

/****************************************************************************
* FreePens
*/
void FreePens(CDC *pDC)
{
   SelectObject(pDC->m_hDC, origPen);
   SelectObject(pDC->m_hDC, origBrush);

   DeleteObject(pen);
   DeleteObject(brush);
}

/****************************************************************************
* SetupPens
*/
void SetupPens(CCEtoODBDoc *doc, CDC *dc, BOOL VoidPoly, int PenWidth, double scale, BOOL showFills, COLORREF voidColor)
{
   COLORREF color = PenColor;

   if (VoidPoly)
      color = voidColor;

   // Brush
   if (color != CurrBrushColor)
   {
      if (doc->getSettings().FillStyle == HS_SOLID)// || (VoidPoly && !doc->NegativeView))
         brush = CreateSolidBrush(color);
      else
         brush = CreateHatchBrush(doc->getSettings().FillStyle, color);
      DeleteObject(SelectObject(dc->m_hDC, brush));
      CurrBrushColor = color;
   }

   if ( (color != CurrPenColor) || (PenWidth != CurrPenWidth) )
   {
      pen = CreatePen(PS_SOLID, PenWidth, color);
      DeleteObject(SelectObject(dc->m_hDC, pen));
      CurrPenColor = color;
      CurrPenWidth = PenWidth;
   }
}

/*******************************************************************************
* DrawPolyStruct
*
*  RETURN: returns TRUE if drew to MemDC
*/
BOOL CCEtoODBView::DrawPolyStruct(CDC *dc, CPolyList *polylist, double scale, int mirror, 
      double insert_x, double insert_y, Mat2x2 *m, int layer, BOOL showFills, BOOL MemDCVoids)
{
   CCEtoODBDoc *doc = GetDocument();

   BOOL returnValue = FALSE;

   if (doc->showDirection)
      DrawDirectionArrow(doc, polylist->GetHead(), TRUE, insert_x, insert_y, scale, m, mirror);

   // check for voids
   BOOL Voids = FALSE;
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = polylist->GetNext(polyPos);
      if (poly->isVoid())
      {
         Voids = TRUE;
         break;
      }
   }

   if (dc->IsPrinting())
   {
      DrawPolys(dc, this, polylist, scale, mirror, insert_x, insert_y, m, layer, showFills, (!doc->NegativeView?RGB(255,255,255):doc->get_layer_color(layer, mirror)));
   }
   else if (!showFills || !MemDCVoids || !Voids || doc->getSettings().FillStyle != HS_SOLID)
   {
      DrawPolys(dc, this, polylist, scale, mirror, insert_x, insert_y, m, layer, showFills, (!doc->NegativeView?doc->getSettings().Foregrnd:doc->get_layer_color(layer, mirror)));
   }
   else
   {
      returnValue = TRUE;

      int left, bottom, width, height;
      left = doc->minXCoord;
      bottom = doc->minYCoord;
      width = doc->maxXCoord - doc->minXCoord;
      height = doc->maxYCoord - doc->minYCoord;

//    if (!dc->IsPrinting())
      {
         PrepareMemDC(this, dc, BLACKNESS);

         DrawPolys(memDC, this, polylist, scale, mirror, insert_x, insert_y, m, layer, showFills, (!doc->NegativeView?doc->getSettings().Foregrnd:doc->get_layer_color(layer, mirror)));

         BitBlt(dc->m_hDC, left, bottom, width, height, memDC->m_hDC, left, bottom, SRCPAINT);

         FreeMemDC(this, dc);


         PrepareMemDC(this, dc, WHITENESS);

         DrawPolys(memDC, this, polylist, scale, mirror, insert_x, insert_y, m, layer, showFills, (!doc->NegativeView?RGB(255, 255, 255):doc->get_layer_color(layer, mirror)));

         BitBlt(dc->m_hDC, left, bottom, width, height, memDC->m_hDC, left, bottom, SRCAND);

         FreeMemDC(this, dc);
      }
/* TODO : file with voids, zoom in, print to extents
      else
      {
         PrepareMemDC(this, dc, WHITENESS);

         DrawPolys(memDC, this, polylist, scale, mirror, insert_x, insert_y, m, layer, showFills, (!doc->NegativeView?RGB(255, 255, 255):doc->get_layer_color(layer, mirror)));

         BitBlt(dc->m_hDC, left, bottom, width, height, memDC->m_hDC, left, bottom, SRCAND);

         FreeMemDC(this, dc);


         PrepareMemDC(this, dc, BLACKNESS);

         DrawPolys(memDC, this, polylist, scale, mirror, insert_x, insert_y, m, layer, showFills, (!doc->NegativeView?RGB(0, 0, 0):doc->get_layer_color(layer, mirror)));

         BitBlt(dc->m_hDC, left, bottom, width, height, memDC->m_hDC, left, bottom, SRCPAINT);

         FreeMemDC(this, dc);
      }   */
   }

   return returnValue;
}                                                                              

/*******************************************************************************
* DrawPolys
*/
void DrawPolys(CDC *dc, CCEtoODBView *view, CPolyList *polylist, double scale, int mirror, 
      double insert_x, double insert_y, Mat2x2 *m, int layer, BOOL showFills, COLORREF voidColor)
{
   CCEtoODBDoc *doc = view->GetDocument();

   // draw each poly to the selected DC
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = polylist->GetNext(polyPos);
      if (!poly->getPntList().GetCount()) continue;
      if (poly->isHidden()) continue;

      int PenWidth, PenShape;
      if (poly->getWidthIndex() >= 0)
      {
         BlockStruct *polyWidth = doc->getWidthTable()[poly->getWidthIndex()];
         if (polyWidth)
         {
                PenWidth = round(polyWidth->getSizeA() * scale * view->getScaleFactor());
            PenShape = polyWidth->getShape();
         }
         else
         {
				// use this user defined width that was set previously in the draw routine
            PenWidth = round(userDefinedWidth * scale * view->getScaleFactor());
            PenShape = T_ROUND;
         }
      }
      else
      {
			// use this user defined width that was set previously in the draw routine
         PenWidth = round(userDefinedWidth * scale * view->getScaleFactor());
         PenShape = T_ROUND;
      }

      if (poly->isVoid() && showFills) // draw thin pen filled void before drawing positive void outline
      {
         SetupPens(doc, dc, TRUE, 0, scale * view->getScaleFactor(), TRUE, voidColor);
         DrawPoly(view, dc, poly, 0, T_ROUND, scale, mirror, insert_x, insert_y, m, layer, TRUE);
      }

      SetupPens(doc, dc, FALSE, PenWidth, scale * view->getScaleFactor(), showFills, voidColor);
      DrawPoly(view, dc, poly, PenWidth, PenShape, scale, mirror, insert_x, insert_y, m, layer, (showFills & !poly->isVoid()));
   }
}

/*******************************************************************************
* DrawPoly
*/
void DrawPoly(CCEtoODBView *view, CDC *dc, CPoly *poly, int PenWidth, int PenShape, double scale, int mirror, 
      double insert_x, double insert_y, Mat2x2 *m, int layer, BOOL showFills)
{
   CCEtoODBDoc *doc = view->GetDocument();
   double scaleFactor = view->getScaleFactor();
   
   double cx, cy, rad;
   Point2 vertex;
   POSITION pntPos;
   CPnt *pnt;
   BOOL FirstPoint;
   double bulge, lastx, lasty;

   // Single Point Poly
   if (poly->getPntList().GetCount() == 1)
   {
      CPnt *pnt = poly->getPntList().GetHead();

      Point2 p;
      p.x = pnt->x * scale;
      p.y = pnt->y * scale;
      if (mirror & MIRROR_FLIP) p.x = -p.x;
      TransPoint2(&p, 1, m, insert_x, insert_y);

      dc->SetPixelV(round(p.x * scaleFactor), round(p.y * scaleFactor), PenColor);

      return;
   }

   // CIRCLE
   if (PolyIsCircle(poly, &cx, &cy, &rad))
   {
      Point2 c;
      c.x = cx * scale;
      c.y = cy * scale;
      rad = rad * scale;
      if (mirror & MIRROR_FLIP) c.x = -c.x;
      TransPoint2(&c, 1, m, insert_x, insert_y);

      HBRUSH lastBrush;
      if (!poly->isFilled() || !showFills)
         lastBrush = (HBRUSH) SelectObject(dc->m_hDC, GetStockObject(HOLLOW_BRUSH));

      if (!dc->IsPrinting() && rad * 2000 < view->pageRect.right - view->pageRect.left)
         dc->SetPixelV(round(c.x * scaleFactor), round(c.y * scaleFactor), PenColor);
      else
         dc->Ellipse(lround((c.x - rad) * scaleFactor),  // left
               lround((c.y + rad) * scaleFactor),  // top
               lround((c.x + rad) * scaleFactor),  // right
               lround((c.y - rad) * scaleFactor)); // bottom

      if (!poly->isFilled() || !showFills)
         SelectObject(dc->m_hDC, lastBrush);

      return;
   }

   // Rectangle Aperture, Not Filled, No Bulges
   BOOL HasBulge = FALSE;
   if (PenShape == T_RECTANGLE && !(poly->isFilled() && showFills))
   {
      // check for bulges
      pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         if (fabs(poly->getPntList().GetNext(pntPos)->bulge) >= BULGE_THRESHOLD)
         {
            HasBulge = TRUE;
            break;
         }
      }

      if (!HasBulge)
      {
         BlockStruct *polyWidth = doc->getWidthTable()[poly->getWidthIndex()];

         HPEN thinPen = CreatePen(PS_SOLID, 0, PenColor);
         HPEN lastPen = (HPEN)SelectObject(dc->m_hDC, thinPen);

         // draw lines
         FirstPoint = TRUE;
         Point2 start, end;
         pntPos = poly->getPntList().GetHeadPosition();
         if (pntPos != NULL)
         {
            pnt = poly->getPntList().GetNext(pntPos);

            end.x = pnt->x * scale;
            if (mirror & MIRROR_FLIP) end.x = -end.x;
            end.y = pnt->y * scale;
            TransPoint2(&end, 1, m, insert_x, insert_y);
         }
         while (pntPos != NULL)
         {
            pnt = poly->getPntList().GetNext(pntPos);

            start.x = end.x;
            start.y = end.y;

            end.x = pnt->x * scale;
            if (mirror & MIRROR_FLIP) end.x = -end.x;
            end.y = pnt->y * scale;
            TransPoint2(&end, 1, m, insert_x, insert_y);

            DrawLineWithRectangleAperture(view, dc, &start, &end, scale, polyWidth); 
         }

         DeleteObject(SelectObject(dc->m_hDC, lastPen));

         return;
      }
   }

   // if sqare end line segment
   if (PenShape == T_SQUARE &&                        // square ends
         poly->getPntList().GetCount() == 2 &&                      // 2 endpoints (line)
         fabs(poly->getPntList().GetHead()->bulge) < BULGE_THRESHOLD)  // not an arc
   {
      HPEN thinPen = CreatePen(PS_SOLID, 0, PenColor);
      HPEN lastPen = (HPEN)SelectObject(dc->m_hDC, thinPen);

      Point2 head, tail;

      pnt = poly->getPntList().GetHead();
      head.x = pnt->x * scale;
      head.y = pnt->y * scale;
      if (mirror & MIRROR_FLIP) head.x = -head.x;
      TransPoint2(&head, 1, m, insert_x, insert_y);

      pnt = poly->getPntList().GetTail();
      tail.x = pnt->x * scale;
      tail.y = pnt->y * scale;
      if (mirror & MIRROR_FLIP) tail.x = -tail.x;
      TransPoint2(&tail, 1, m, insert_x, insert_y);

      POINT points[4];

      double size = (0.5 * doc->getWidthTable()[poly->getWidthIndex()]->getSizeA());
      if (fabs(head.x - tail.x) < SMALLNUMBER) // undefined (vertical)
      {
         double maxy = max(head.y, tail.y);
         double miny = min(head.y, tail.y);

         points[0].y = points[1].y = lround((maxy + size) * scaleFactor);
         points[0].x = lround((head.x - size) * scaleFactor);
         points[1].x = lround((head.x + size) * scaleFactor);

         points[2].y = points[3].y = lround((miny - size) * scaleFactor);
         points[2].x = lround((head.x + size) * scaleFactor);
         points[3].x = lround((head.x - size) * scaleFactor);
      }
      else if (fabs(head.y - tail.y) < SMALLNUMBER) // 0 slope (horizontal)
      {
         double maxx = max(head.x, tail.x);
         double minx = min(head.x, tail.x);

         points[0].x = points[1].x = lround((maxx + size) * scaleFactor);
         points[0].y = lround((head.y - size) * scaleFactor);
         points[1].y = lround((head.y + size) * scaleFactor);

         points[2].x = points[3].x = lround((minx - size) * scaleFactor);
         points[2].y = lround((head.y + size) * scaleFactor);
         points[3].y = lround((head.y - size) * scaleFactor);
      }
      else 
      {
         double angle = atan2(head.y - tail.y, head.x - tail.x);
         double cosine = size * cos(angle);
         double sine = size * sin(angle);

         points[0].x = lround((tail.x - cosine - sine) * scaleFactor);
         points[0].y = lround((tail.y - sine + cosine) * scaleFactor);
      
         points[1].x = lround((tail.x - cosine + sine) * scaleFactor);
         points[1].y = lround((tail.y - sine - cosine) * scaleFactor);
               
         points[2].x = lround((head.x + cosine + sine) * scaleFactor);
         points[2].y = lround((head.y + sine - cosine) * scaleFactor);
            
         points[3].x = lround((head.x + cosine - sine) * scaleFactor);
         points[3].y = lround((head.y + sine + cosine) * scaleFactor);
      }
      
      dc->Polygon(points, 4);

      DeleteObject(SelectObject(dc->m_hDC, lastPen));
      return;
   }

   // else normal polyline
   // draw lines with round ends
   FirstPoint = TRUE;

   if (poly->isFilled() && showFills)
      dc->BeginPath(); 

   Point2 head, tail;
   double headAngle, tailAngle;

   pntPos = poly->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      pnt = poly->getPntList().GetNext(pntPos);

      vertex.x = pnt->x * scale;
      vertex.y = pnt->y * scale;
      if (mirror & MIRROR_FLIP) vertex.x = -vertex.x;
      TransPoint2(&vertex, 1, m, insert_x, insert_y);

      // if first point
      if (pnt == poly->getPntList().GetHead() && poly->getPntList().GetCount() > 1) 
      {
         // if not round ends, remember where to flash ends
         if (PenShape == T_SQUARE || PenShape == T_RECTANGLE)
         {
            CPnt *pnt2 = poly->getPntList().GetAt(pntPos);

            Point2 next;
            next.x = pnt2->x * scale;
            next.y = pnt2->y * scale;
            if (mirror & MIRROR_FLIP) next.x = -next.x;
            TransPoint2(&next, 1, m, insert_x, insert_y);

            headAngle = ArcTan2(next.y - vertex.y, next.x - vertex.x);

            head.x = vertex.x;
            head.y = vertex.y;
         }
      }
      // last point
      else if (pnt == poly->getPntList().GetTail() && poly->getPntList().GetCount() > 1)
      {
         // if not round ends, remember where to flash ends
         if (PenShape == T_SQUARE || PenShape == T_RECTANGLE)
         {
            POSITION tempPos = poly->getPntList().GetTailPosition();
            poly->getPntList().GetPrev(tempPos);

            CPnt *pnt2 = poly->getPntList().GetAt(tempPos);

            Point2 prev;

            prev.x = pnt2->x * scale;
            prev.y = pnt2->y * scale;
            if (mirror & MIRROR_FLIP) prev.x = -prev.x;
            TransPoint2(&prev, 1, m, insert_x, insert_y);

            tailAngle = ArcTan2(prev.y - vertex.y, prev.x - vertex.x);

            tail.x = vertex.x;
            tail.y = vertex.y;
         }
      }

      if (FirstPoint)
      {
         dc->MoveTo(lround(vertex.x * scaleFactor), lround(vertex.y * scaleFactor));
         FirstPoint = FALSE;
      }
      else
      {
         // if (fabs(bulge) < BULGE_THRESHOLD make a draw error on smaller bulges.
         // if a bulge is smaller than 0.01, the resulting angle is 2 degrees.
         if (fabs(bulge) < BULGE_THRESHOLD || 
               (!dc->IsPrinting() && (fabs(vertex.x - lastx) * scaleFactor * view->ScaleDenom < ARC_TO_LINE_THRESHOLD && // draw line for very small arcs
               fabs(vertex.y - lasty) * scaleFactor * view->ScaleDenom < ARC_TO_LINE_THRESHOLD)))
         {
            dc->LineTo(lround(vertex.x * scaleFactor), lround(vertex.y * scaleFactor));
         }
         else
         {
            double da, sa, r, cx, cy;
            da = atan(bulge) * 4;

            if (mirror & MIRROR_FLIP) da = -da;

            ArcPoint2Angle(lastx, lasty, vertex.x, vertex.y, da, &cx, &cy, &r, &sa);

            if (Platform == WINNT) // Windows NT
            {
               dc->SetArcDirection(da<0.0 ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE);
               dc->ArcTo(lround((cx - r) * scaleFactor), lround((cy + r) * scaleFactor), 
                     lround((cx + r) * scaleFactor), lround((cy - r) * scaleFactor),
                     lround(lastx * scaleFactor), lround(lasty * scaleFactor), 
                     lround(vertex.x * scaleFactor), lround(vertex.y * scaleFactor));  

               dc->SetArcDirection(AD_COUNTERCLOCKWISE);

               dc->LineTo(lround(vertex.x * scaleFactor), lround(vertex.y * scaleFactor));
            }
            else // Windows 95
            {
               if (cx * scaleFactor > doc->maxXCoord || cx * scaleFactor < doc->minXCoord || 
                     cy * scaleFactor > doc->maxYCoord || cy * scaleFactor < doc->minYCoord)
                  dc->LineTo((int)(vertex.x * scaleFactor),(int)(vertex.y * scaleFactor));
               else //if (poly->isFilled() && showFills)
                  AngleArc_Lines95(dc, (int)(cx * scaleFactor), (int)(cy * scaleFactor), r * scaleFactor, sa, da);
/*             else // just draw segments instead of arcs  on 9x - Pads sample zoomed in - arc disappears
               {
                  if (da < 0.0) // clockwise
                  {     
                     dc->Arc((int)((cx - r) * scaleFactor), (int)((cy + r) * scaleFactor), 
                           (int)((cx + r) * scaleFactor), (int)((cy - r) * scaleFactor),
                           (int)(vertex.x * scaleFactor), (int)(vertex.y * scaleFactor),
                           (int)(lastx * scaleFactor), (int)(lasty * scaleFactor));
                  }
                  else // counter-clockwise
                  {
                     dc->Arc((int)((cx - r) * scaleFactor), (int)((cy + r) * scaleFactor), 
                           (int)((cx + r) * scaleFactor), (int)((cy - r) * scaleFactor),
                           (int)(lastx * scaleFactor), (int)(lasty * scaleFactor), 
                           (int)(vertex.x * scaleFactor), 
                           (int)(vertex.y * scaleFactor));  
                  }
                  dc->MoveTo((int)(vertex.x * scaleFactor),(int)(vertex.y * scaleFactor));
               }*/
            }
         }
      }
      bulge = pnt->bulge;
      lastx = vertex.x;
      lasty = vertex.y;
   }

	if (poly->isFilled() && showFills) 
   {
      dc->EndPath();

		if (poly->getPntList().GetCount() > 10000) // Case 1643 : Complexity of 27,000 vertex poly when zoomed in with translucency on severly crashes display device drivers which chrashes entire system
		{
			int mixmode = dc->GetROP2();
			dc->SetROP2(R2_COPYPEN);
			dc->StrokeAndFillPath();
			dc->SetROP2(mixmode);
		}
		else
			dc->StrokeAndFillPath();
   }

   // flash squares on line ends
   if ((PenShape == T_SQUARE || PenShape == T_RECTANGLE) && 
      poly->getPntList().GetCount() > 1 && !poly->isClosed())
   {
      HPEN thinPen = CreatePen(PS_SOLID, 0, PenColor);
      HPEN lastPen = (HPEN)SelectObject(dc->m_hDC, thinPen);

      POINT points[4];
      double size = (0.5 * (double)PenWidth * sqrt(2.));
      // only flash on head if head is not an arc
      if (fabs(poly->getPntList().GetHead()->bulge) < SMALLNUMBER)
      {
         double sine = (sin(45*PI/180 + headAngle)*size);
         double cosine = (cos(45*PI/180 + headAngle)*size);
         double lx = (head.x * scaleFactor);
         double ly = (head.y * scaleFactor);

         // 3 > 0
         // ^   v                
         // 2 < 1

         points[0].x = lround(lx + cosine);
         points[0].y = lround(ly + sine);
            
         points[1].x = lround(lx + sine);
         points[1].y = lround(ly - cosine);
            
         points[2].x = lround(lx - cosine);
         points[2].y = lround(ly - sine);
            
         points[3].x = lround(lx - sine);
         points[3].y = lround(ly + cosine);

         dc->Polygon(points, 4);
      }

      // only flash on tail if tail is not an arc
      POSITION tempPos = poly->getPntList().GetTailPosition();
      poly->getPntList().GetPrev(tempPos); // get bulge from second to last Pnt
      if (fabs(poly->getPntList().GetAt(tempPos)->bulge) < SMALLNUMBER)
      {
         double sine = (sin(45*PI/180 + tailAngle)*size);
         double cosine = (cos(45*PI/180 + tailAngle)*size);
         double tx = (tail.x * scaleFactor);
         double ty = (tail.y * scaleFactor);

         points[0].x = lround(tx + cosine);
         points[0].y = lround(ty + sine);
            
         points[1].x = lround(tx + sine);
         points[1].y = lround(ty - cosine);
            
         points[2].x = lround(tx - cosine);
         points[2].y = lround(ty - sine);
            
         points[3].x = lround(tx - sine);
         points[3].y = lround(ty + cosine);

         dc->Polygon(points, 4);
      }

      DeleteObject(SelectObject(dc->m_hDC, lastPen));
   }
}

/*******************************************************************************
* AngleArc_Lines95
*/
BOOL AngleArc_Lines95(CDC *dc, int X, int Y, double radius, double sa, double da)
{
   double currentAngle;       // Current angle in radians
   double stepAngle = 0.03;   // The sweep increment value in radians
   double ea = sa + da;       // End angle in radians
   int ix, iy;                // Current point on arc
 
   /* Draw a line to the starting point */
   ix = X + (int)(radius * cos(sa));
   iy = Y + (int)(radius * sin(sa));
   dc->LineTo(ix, iy);

   /* Calculate and draw the sweep */
   if (da > 0.0)
      for (currentAngle = sa; currentAngle <= ea; currentAngle += stepAngle) 
      {
         ix = X + (int)(radius * cos(currentAngle));
         iy = Y + (int)(radius * sin(currentAngle));
         dc->LineTo(ix, iy);
      }
   else
      for (currentAngle = sa; currentAngle >= ea; currentAngle -= stepAngle) 
      {
         ix = X + (int)(radius * cos(currentAngle));
         iy = Y + (int)(radius * sin(currentAngle));
         dc->LineTo(ix, iy);
      }
 
   /* Draw a line to the ending point */
   ix = X + (int)(radius * cos(ea));
   iy = Y + (int)(radius * sin(ea));
   dc->LineTo(ix, iy);

   return TRUE;
}
 
/*******************************************************************************
* DrawLineWithRectangleAperture
*/
void DrawLineWithRectangleAperture(CCEtoODBView *view, CDC *pDC, Point2 *start, Point2 *end, double scale, BlockStruct *polyWidth)
{
   CCEtoODBDoc *doc = view->GetDocument();
   double w = 0.5 * polyWidth->getSizeA() * scale * view->getScaleFactor();
   if (min(start->x, end->x) - w > view->pageRect.right ||
         max(start->x, end->x) + w < view->pageRect.left ||
         min(start->y, end->y) - w > view->pageRect.top ||
         max(start->y, end->y) + w < view->pageRect.bottom)
      return;

   POINT p[6];
   double angle = ArcTan2(end->y - start->y, end->x - start->x);
   double width, height;

   if (polyWidth)
   {
      width = 0.5 * polyWidth->getSizeA() * scale * view->getScaleFactor();
      if (polyWidth->getShape() == T_SQUARE)
         height = width;
      else
         height = 0.5 * polyWidth->getSizeB() * scale * view->getScaleFactor();
   }

   if (angle >= PI/2) // if in quadrant II
   {
      p[0].x = lround(start->x * view->getScaleFactor() + width);
      p[0].y = lround(start->y * view->getScaleFactor() + height);
      p[1].x = lround(start->x * view->getScaleFactor() + width);
      p[1].y = lround(start->y * view->getScaleFactor() - height);
      p[2].x = lround(start->x * view->getScaleFactor() - width);
      p[2].y = lround(start->y * view->getScaleFactor() - height);
      p[3].x = lround(end->x * view->getScaleFactor() - width);
      p[3].y = lround(end->y * view->getScaleFactor() - height);
      p[4].x = lround(end->x * view->getScaleFactor() - width);
      p[4].y = lround(end->y * view->getScaleFactor() + height);
      p[5].x = lround(end->x * view->getScaleFactor() + width);
      p[5].y = lround(end->y * view->getScaleFactor() + height);
   }
   else if (angle >= 0) // if in quadrant I
   {
      p[0].x = lround(start->x * view->getScaleFactor() + width);
      p[0].y = lround(start->y * view->getScaleFactor() - height);
      p[1].x = lround(start->x * view->getScaleFactor() - width);
      p[1].y = lround(start->y * view->getScaleFactor() - height);
      p[2].x = lround(start->x * view->getScaleFactor() - width);
      p[2].y = lround(start->y * view->getScaleFactor() + height);
      p[3].x = lround(end->x * view->getScaleFactor() - width);
      p[3].y = lround(end->y * view->getScaleFactor() + height);
      p[4].x = lround(end->x * view->getScaleFactor() + width);
      p[4].y = lround(end->y * view->getScaleFactor() + height);
      p[5].x = lround(end->x * view->getScaleFactor() + width);
      p[5].y = lround(end->y * view->getScaleFactor() - height);
   }
   else if (angle >= -PI/2) // if in quadrant IV
   {
      p[0].x = lround(start->x * view->getScaleFactor() - width);
      p[0].y = lround(start->y * view->getScaleFactor() - height);
      p[1].x = lround(start->x * view->getScaleFactor() - width);
      p[1].y = lround(start->y * view->getScaleFactor() + height);
      p[2].x = lround(start->x * view->getScaleFactor() + width);
      p[2].y = lround(start->y * view->getScaleFactor() + height);
      p[3].x = lround(end->x * view->getScaleFactor() + width);
      p[3].y = lround(end->y * view->getScaleFactor() + height);
      p[4].x = lround(end->x * view->getScaleFactor() + width);
      p[4].y = lround(end->y * view->getScaleFactor() - height);
      p[5].x = lround(end->x * view->getScaleFactor() - width);
      p[5].y = lround(end->y * view->getScaleFactor() - height);
   }
   else // if in quadrant III
   {
      p[0].x = lround(start->x * view->getScaleFactor() - width);
      p[0].y = lround(start->y * view->getScaleFactor() + height);
      p[1].x = lround(start->x * view->getScaleFactor() + width);
      p[1].y = lround(start->y * view->getScaleFactor() + height);
      p[2].x = lround(start->x * view->getScaleFactor() + width);
      p[2].y = lround(start->y * view->getScaleFactor() - height);
      p[3].x = lround(end->x * view->getScaleFactor() + width);
      p[3].y = lround(end->y * view->getScaleFactor() - height);
      p[4].x = lround(end->x * view->getScaleFactor() - width);
      p[4].y = lround(end->y * view->getScaleFactor() - height);
      p[5].x = lround(end->x * view->getScaleFactor() - width);
      p[5].y = lround(end->y * view->getScaleFactor() + height);
   }

   Polygon(pDC->m_hDC, p, 6);
} // end DrawLineWithRectangleAperture


/*******************************************************************************
* DrawBlob
*/
void CCEtoODBView::DrawBlob(CDC *dc, DataStruct *data, double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m)
{
   Point2 point2;
   point2.x = data->getBlob()->pnt.x * scale;
   point2.y = data->getBlob()->pnt.y * scale;
   if (mirror & MIRROR_FLIP)  point2.x = -point2.x;
   TransPoint2(&point2, 1, m, insert_x, insert_y);

/*       if (mirror & MIRROR_FLIP)
      _rot = rotation - data->getInsert()->getAngle();
   else
      _rot = rotation + data->getInsert()->getAngle();*/

   if (data->getBlob()->getBitmap() == NULL)  // no bitmap attached
   {
      CPen *orgPen = (CPen*)dc->SelectStockObject(WHITE_PEN);

      int left = round(point2.x * getScaleFactor());
      int right = round((point2.x + data->getBlob()->width * scale) * getScaleFactor());
      int bottom = round(point2.y * getScaleFactor());
      int top = round((point2.y + data->getBlob()->height * scale) * getScaleFactor());

      // X
      dc->MoveTo(left, top);
      dc->LineTo(right, bottom);
      dc->MoveTo(left, bottom);
      dc->LineTo(right, top);

      // Box
      dc->MoveTo(left, top);
      dc->LineTo(right, top);
      dc->LineTo(right, bottom);
      dc->LineTo(left, bottom);
      dc->LineTo(left, top);

      dc->SetBkMode(OPAQUE);
      dc->SetTextAlign(TA_CENTER);
      dc->TextOut(round((point2.x + data->getBlob()->width * scale / 2) * getScaleFactor()), 
            round((point2.y + data->getBlob()->height * scale / 2) * getScaleFactor()), data->getBlob()->filename);
      dc->SetBkMode(TRANSPARENT);

      dc->SelectObject(orgPen);
   }
   else
   {
      CDC memDC;
      memDC.CreateCompatibleDC(dc);

      memDC.SelectObject(data->getBlob()->getBitmap());

      BITMAP bmpInfo;
      data->getBlob()->getBitmap()->GetBitmap(&bmpInfo);

      dc->StretchBlt(round(point2.x * getScaleFactor()), round((point2.y + data->getBlob()->height * scale) * getScaleFactor()), 
         round(data->getBlob()->width * scale * getScaleFactor()), round(-data->getBlob()->height * scale * getScaleFactor()), 
         &memDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
   }
}


/*******************************************************************************
* 
*  DrawTextEntity
      special char :
      %O = Overline a char
      %U = Underline a char
      %% = percent char
*/
void CCEtoODBView::DrawTextEntity(CDC *pDC, DataStruct *np, double scale, int mirror, double insert_x, double insert_y,
      Mat2x2 *m, double rotation)
{
   if (!CFontList::fontIsLoaded())
   {
      return;
   }

   FontStruct* fs = CFontList::getFontList().getFont(np->getText()->getFontNumber());

   Point2 point;
   int oblique;

   oblique = np->getText()->getOblique();
   if (oblique > 80)
      oblique = 80;
   if (oblique < -80)
      oblique = -80;

   int PenWidth = round(GetDocument()->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA() * scale * getScaleFactor()); 
   if ( (PenColor != CurrPenColor) || (PenWidth != CurrPenWidth) )
   {
      pen = CreatePen(PS_SOLID, PenWidth, PenColor);
      DeleteObject(SelectObject(pDC->m_hDC, pen));
      CurrPenColor = PenColor;
      CurrPenWidth = PenWidth;
   }
   // Find insertion point of text string
   point.x = np->getText()->getPnt().x * scale;
   point.y = np->getText()->getPnt().y * scale;
   if (mirror & MIRROR_FLIP) point.x = -point.x;
   TransPoint2(&point, 1, m, insert_x, insert_y);

   double text_rot;
   if (mirror & MIRROR_FLIP)
      text_rot = rotation - np->getText()->getRotation();
   else
      text_rot = rotation + np->getText()->getRotation();

   int text_mirror;
   // text is mirrored if mirror is set or text.mirror but not if none or both
   text_mirror = np->getText()->getResultantMirror(mirror);

   DrawText(pDC, np->getText()->getText(), fs, &point, scale, text_rot, 
         np->getText()->getHeight() * scale, np->getText()->getWidth() * scale,
         text_mirror, oblique, np->getText()->isProportionallySpaced(), np->getText()->getMirrorDisabled(), np->getLayerIndex(),
         np->getText()->getHorizontalPosition(), np->getText()->getVerticalPosition());
} // DrawTextEntity

/*******************************************************************************
*
*   DrawText
      special char :
      %O = Overline a char
      %U = Underline a char
      %% = percent char

*/

/******************************************************************************
* CCEtoODBView::DrawText
*/
void CCEtoODBView::DrawText(CDC *pDC, const char *text, FontStruct *fs, Point2 *point, 
      double scale, double text_rot, double height, double width, BOOL text_mirror, 
      int oblique, BOOL proportional, BOOL nevermirror, int layer,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition)
{
   Mat2x2 m1;
   RotMat2(&m1, text_rot);

   const double spaceratio = GetDocument()->getSettings().getTextSpaceRatio(); // turn percent into a decimal

   CTMatrix matrix;
   matrix.translateCtm(point->x,point->y);
   matrix.scaleCtm(scale,scale);

   TextStruct textStruct(text,proportional != 0);
	textStruct.setWidth(width);

   // Calculate the width of the string taking into account the proportional flag
   double stringWidth = textStruct.getMaxLineLength(spaceratio, CurrPenWidth/getScaleFactor()/scale);
   CPoint2d initialCharacterPosition;

   switch (horizontalPosition)
   {
   case horizontalPositionLeft: // left align
   default:
      initialCharacterPosition.x = 0;
      break;
   case horizontalPositionCenter: // center align
      initialCharacterPosition.x = -stringWidth / 2;
      break;
   case horizontalPositionRight: // right align
      initialCharacterPosition.x = -stringWidth;
      break;
   }

   if (text_mirror)
   {
      initialCharacterPosition.x *= -1.;
      matrix.scaleCtm(-1.,1.);
   }

   int lineCount = textStruct.getNumLines();

   switch (verticalPosition)
   {
   case verticalPositionBottom: // bottom align
      initialCharacterPosition.y = height * (1.0 + spaceratio) * (lineCount - 1);
      break;
   case verticalPositionCenter: // center align
      initialCharacterPosition.y = -(height / 2) + (height * (1.0 + spaceratio) * (lineCount - 1) / 2);
      break;
   case verticalPositionTop: // top align
      initialCharacterPosition.y = -height;
      break;
   case verticalPositionBaseline: // baseline align
   default:
      initialCharacterPosition.y = 0.;
      break;
   }

   if (text_mirror && nevermirror)  // if never mirror, text start offset must be moved in -x
   {
      initialCharacterPosition.x -= stringWidth;
      text_mirror = 0;
      horizontalPosition = oppositeHorizontalPosition(horizontalPosition);
      matrix.scaleCtm(-1.,1.);
      //matrix.translateCtm(-stringWidth,0.);
   }

   matrix.rotateRadiansCtm(text_rot);

   bool drawBoundaryFlag = false;

   if (drawBoundaryFlag)
   {
      textStruct.setHorizontalPosition(horizontalPosition);
      textStruct.setVerticalPosition(verticalPosition);
      textStruct.setHeight(height);
      textStruct.setWidth(width);
      CPoly textBoundary;
      textStruct.getBoundaryPoly(textBoundary,spaceratio);
      textBoundary.transform(matrix);

      bool drawFlag = false;

      for (POSITION pos = textBoundary.getPntList().GetHeadPosition();pos != NULL;)
      {
         CPnt* pnt = textBoundary.getPntList().GetNext(pos);

         if (drawFlag)
         {
            pDC->LineTo(DcaRound(pnt->x * getScaleFactor()),DcaRound(pnt->y * getScaleFactor()));
         }
         else
         {
            pDC->MoveTo(DcaRound(pnt->x * getScaleFactor()),DcaRound(pnt->y * getScaleFactor()));
            drawFlag = true;
         }
      }
   }

	// DO NOT REMOVE:  This is used for debugging purposes
	//{
	//	Point2 offset;
	//	offset.x = x_offset;
	//	offset.y = y_offset;

	//	if (text_mirror)
	//		offset.x = -offset.x;

	//	TransPoint2(&offset, 1, &m1, 0, 0);
	//	pDC->MoveTo((point->x + offset.x - .001) * scaleFactor, (point->y + offset.y) * scaleFactor);
	//	pDC->LineTo((point->x + offset.x + .001) * scaleFactor, (point->y + offset.y) * scaleFactor);
	//	pDC->MoveTo((point->x + offset.x) * scaleFactor, (point->y + offset.y - .001) * scaleFactor);
	//	pDC->LineTo((point->x + offset.x) * scaleFactor, (point->y + offset.y + .001) * scaleFactor);

	//	pDC->MoveTo((point->x + offset.x) * scaleFactor, (point->y + offset.y) * scaleFactor);
	//	pDC->LineTo((point->x + offset.x + stringWidth) * scaleFactor, (point->y + offset.y) * scaleFactor);
	//	pDC->LineTo((point->x + offset.x + stringWidth) * scaleFactor, (point->y + offset.y + height) * scaleFactor);
	//	pDC->LineTo((point->x + offset.x) * scaleFactor, (point->y + offset.y + height) * scaleFactor);
	//	pDC->LineTo((point->x + offset.x) * scaleFactor, (point->y + offset.y) * scaleFactor);
	//}

   CPoint2d characterPosition(initialCharacterPosition);

   for (unsigned int i = 0; i < strlen(text); i++)
   {
      unsigned char c = text[i];

      if (c == '\n')
      {
         characterPosition.y -= height * (1.0 + spaceratio);

         //char *newLine = strchr(text + i + 1, '\n');

         //if (newLine != NULL)
         //{
         //   charCount = newLine - (text + i) - 1;
         //}
         //else 
         //{
         //   charCount = strlen(text) - i - 1;
         //}

         //double currentStringWidth = (1.0 + spaceratio) * charCount * width;

         characterPosition.x = initialCharacterPosition.x;

         continue;
      }

      if (c == '\r')
      {
         continue;
      }

      int textMode = 0;

      if (c == '%')
      {
         ++i;
         c = text[i];

         if (c == 'O')
         {
            textMode |= TEXT_OVERLINE; // next char
            continue;   // fetch next char
         }
         else if (c == 'U')
         {
            textMode |= TEXT_UNDERLINE;   // next char
            continue;   // fetch next char
         }
         else
         {
            // this is nothing
         }        
      }

      if (c == ' ')
      {
         if (proportional)
         {
            characterPosition.x += 2.0 * spaceratio * width; 
         }
         else
         {
            characterPosition.x += width * (1.0 + spaceratio); 
         }

         textMode = 0;  

         continue;
      }

      // calc bar if needed
      double barLength;

      if (proportional)
      {
         barLength = (fs->getCharacterWidth(c) * 2.0 + spaceratio) * width;
      }
      else // not proportional
      {
         barLength = width * (1.0 + spaceratio);
      }

      Point2 offset;
      offset.x = characterPosition.x;
      offset.y = characterPosition.y;

      if (text_mirror)
		{
         switch (horizontalPosition)
         {
         case horizontalPositionCenter: // center align
            offset.x = stringWidth - offset.x;
            break;
         case horizontalPositionRight: // right align
            offset.x = (2. * stringWidth) - offset.x;
            break;
         case horizontalPositionLeft: // left align
         default:
	         offset.x = -offset.x;
            break;
         }
		}

      TransPoint2(&offset, 1, &m1, 0, 0);

      CPolyList* polyList = fs->getCharacterData(c);

      if (polyList == NULL)
      {
         // unprintable char - draw a ? char
         polyList = fs->getCharacterData('?');
      }

      if (polyList != NULL)
      {
// DO NOT REMOVE:  This is used for debugging purposes
//{
//   pDC->MoveTo((point->x + offset.x) * scaleFactor, (point->y + offset.y) * scaleFactor);
//   pDC->LineTo(((point->x + offset.x) + width) * scaleFactor, (point->y + offset.y) * scaleFactor);
//   pDC->LineTo(((point->x + offset.x) + width) * scaleFactor, ((point->y + offset.y) + height) * scaleFactor);
//   pDC->LineTo((point->x + offset.x) * scaleFactor, ((point->y + offset.y) + height) * scaleFactor);
//   pDC->LineTo((point->x + offset.x) * scaleFactor, (point->y + offset.y) * scaleFactor);
//}
         DrawChar(pDC, polyList,
               point->x + offset.x, point->y + offset.y,
               text_rot, height, width,
               text_mirror, oblique, textMode, barLength, layer);
      }

      // Find offset from insertion point for this character
      double charWidth;

      if (proportional)
      {
         charWidth = (fs->getCharacterWidth(c) * 2.0 + spaceratio) * width;
      }
      else // not proportional
      {
         charWidth = (1.0 + spaceratio) * width;
      }

      characterPosition.x += charWidth + (CurrPenWidth/getScaleFactor()/scale); // in .fnt file, chars are 4x8, so full width is 0.5

      textMode = 0;  // must be on the end.
   }
}

/**********************************************************************************
* DrawUndefined           
*     PARAMETERS:
*        - x, y, radius : in windows drawing units
*        - int code : number to put in circle
*     NOTES:
*        - PenColor should already be set
*/
void CCEtoODBView::DrawUndefined(CDC *pDC, double x, double y, double radius, int code)
{
   int PenWidth = 0;
   if ( (PenColor != CurrPenColor) || (PenWidth != CurrPenWidth) )
   {
      pen = CreatePen(PS_SOLID, PenWidth, PenColor);
      DeleteObject(SelectObject(pDC->m_hDC, pen));
      CurrPenColor = PenColor;
      CurrPenWidth = PenWidth;
   }
   HBRUSH lastBrush = (HBRUSH) SelectObject(pDC->m_hDC, GetStockObject(HOLLOW_BRUSH));

   double height;
   pDC->Ellipse(round(x - radius),  // left
            round(y + radius),  // top
            round(x + radius),  // right
            round(y - radius)); // bottom

   SelectObject(pDC->m_hDC, lastBrush);

   if (!GetDocument()->getSettings().ShowUndefinedNums || !code || !CFontList::fontIsLoaded()) // do not draw numbers (.. || if ApNum == 0)
      return;

   // scale factor is factored in inside DrawChar. Everything passed in should be calculated w/o scaleFactor.
   x /= getScaleFactor();
   y /= getScaleFactor();

   FontStruct* fs = CFontList::getFontList().getFirstFont();

   if (code / 10000) // more than 4 digits
   {
   }
   else if (code / 1000) // if a 4 digit aperture number
   {
      height = 0.6 * radius / getScaleFactor();
            
      // draw thousands digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + code/1000),
            x - 1.375*height,
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);   

      // draw hundreds digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + (code/100)%10),
            x - 0.625*height,
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);   

      // draw tens digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + (code/10)%10),
            x + 0.125*height, 
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);

      // draw ones digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + code%10),
            x + 0.875*height,
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);
   }
   else if (code / 100) // if a 3 digit aperture number
   {
      height = 0.8 * radius / getScaleFactor();
            
      // draw hundreds digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + code/100),
            x - height,
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);   

      // draw tens digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + (code/10)%10),
            x - height/4, // offset by half of the width to center digit
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);

      // draw ones digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + code%10),
            x + height/2,
            y - height/2,
            0, height, height/2, 0, 0, 0, 0);
   }
   else // 2 digit Aperture number
   {
      height = 1.0 * radius / getScaleFactor();
      // draw tens digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + code/10),
            x - 0.7*height, y - height/2,
            0, height, height/2, 0, 0, 0, 0);

      // draw ones digit
      DrawChar(pDC, fs->getCharacterData(ASCII_ZERO + code%10),
            x, y - height/2,
            0, height, height/2, 0, 0, 0, 0);
   }
}

/*******************************************************************************
   DrawChar
   flag mirror
   int oblique; // angle to slant - deg - 0 < oblique < 90]
   scale is the actual height of the char.

*/
void CCEtoODBView::DrawChar(CDC *pDC, CPolyList *polylist, double insert_x, double insert_y,
      double rotation, double height, double width, char mirror, 
      int oblique, int textmode, double barlength, int layer)
{
   CCEtoODBDoc *doc = (CCEtoODBDoc*)GetDocument();
	CPolyList modPolylist;

   Mat2x2 m;
   RotMat2(&m, rotation);
   double obliqueRatio = tan( DegToRad((double)oblique) );

   BOOL Voids = FALSE;
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = polylist->GetNext(polyPos);
      CPoly *newPoly = new CPoly(*poly);

	   // check for voids
      if (newPoly->isVoid())
         Voids = TRUE;

      POSITION pntPos = newPoly->getPntList().GetHeadPosition();
      while (pntPos)
      {
         CPnt *pnt = newPoly->getPntList().GetNext(pntPos);
         Point2 point2;

         // oblique text lines
         point2.x = pnt->x + (obliqueRatio * pnt->y);
         point2.y = pnt->y;

         point2.x *= 2.0 * width; 
         point2.y *= height;

			pnt->x = (DbUnit)point2.x;
			pnt->y = (DbUnit)point2.y;
      }

		newPoly->setWidthIndex(-1);
		modPolylist.AddTail(newPoly);
	}

	if (!Voids)
	{
		double prevWidth = userDefinedWidth;
		userDefinedWidth = CurrPenWidth / getScaleFactor();
	   DrawPolys(pDC, this, &modPolylist, 1, mirror, insert_x, insert_y, &m, layer, TRUE, doc->getSettings().Foregrnd);
		userDefinedWidth = prevWidth;
	}
	else
	{
      int left, bottom, width, height;
      left = doc->minXCoord;
      bottom = doc->minYCoord;
      width = doc->maxXCoord - doc->minXCoord;
      height = doc->maxYCoord - doc->minYCoord;

		PrepareMemDC(this, pDC, BLACKNESS);
		DrawPolys(memDC, this, &modPolylist, 1, mirror, insert_x, insert_y, &m, layer, TRUE, doc->getSettings().Foregrnd);
		BitBlt(pDC->m_hDC, left, bottom, width, height, memDC->m_hDC, left, bottom, SRCPAINT);
		FreeMemDC(this, pDC);

		PrepareMemDC(this, pDC, WHITENESS);
		DrawPolys(memDC, this, &modPolylist, 1, mirror, insert_x, insert_y, &m, layer, TRUE, RGB(255, 255, 255));
		BitBlt(pDC->m_hDC, left, bottom, width, height, memDC->m_hDC, left, bottom, SRCAND);
		FreeMemDC(this, pDC);
	}

	modPolylist.empty();

	Point2 p1, p2;
   if (textmode & TEXT_OVERLINE)
   {
      // text char is from 0..1
      p1.x = 0 + obliqueRatio * height;
      p2.x = barlength + obliqueRatio * height;
      p1.y = p2.y = 1.1 * height;

      if (mirror)
      {
         p1.x = -p1.x;
         p2.x = -p2.x;
      }

      TransPoint2(&p1, 1, &m, insert_x, insert_y);
      TransPoint2(&p2, 1, &m, insert_x, insert_y);

      pDC->MoveTo(lround(p1.x * getScaleFactor()), round(p1.y * getScaleFactor()));
      pDC->LineTo(lround(p2.x * getScaleFactor()), round(p2.y * getScaleFactor()));
   }
   
   if (textmode & TEXT_UNDERLINE)
   {
      // text char is from 0..1
      p1.x = 0;
      p2.x = barlength;
      p1.y = p2.y = -0.1 * height;

      if (mirror)
         p2.x = -p2.x;

      TransPoint2(&p1, 1, &m, insert_x, insert_y);
      TransPoint2(&p2, 1, &m, insert_x, insert_y);

      pDC->MoveTo(lround(p1.x * getScaleFactor()), round(p1.y * getScaleFactor()));
      pDC->LineTo(lround(p2.x * getScaleFactor()), round(p2.y * getScaleFactor()));
   }
} // end DrawChar


static HBITMAP origBitmap;
static HPEN origMemPen;
static HBRUSH origMemBrush;
static HBITMAP origMemBitmap;
static COLORREF lastPenColor, lastBrushColor;
static int lastPenWidth;
static HPEN lastPen;
static HBRUSH lastBrush;
/*********************************************************************
* PrepareMemDC
*/
void PrepareMemDC(CCEtoODBView *view, CDC *pDC, int background)
{
   CCEtoODBDoc *doc = view->GetDocument();

   CRect rect;
   view->GetClientRect(&rect);

   memDC = new CDC;
   memDC->CreateCompatibleDC(pDC);                     
   memBitmap = new CBitmap;
   memBitmap->CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
   origMemBitmap = (HBITMAP)SelectObject(memDC->m_hDC, memBitmap->m_hObject);

   BitBlt(memDC->m_hDC, rect.left, rect.top, abs(rect.Width()), abs(rect.Height()), NULL, 0, 0, background);

   { // prepareDC
      SetMapMode(memDC->m_hDC, MM_ISOTROPIC);

      SetWindowOrgEx(memDC->m_hDC, view->GetXPos(), view->GetYPos(), NULL); // scroller pos
      SetWindowExtEx(memDC->m_hDC, (int)(1.0*(doc->maxXCoord - doc->minXCoord) / view->ScaleDenom), 
            (int)(-1.0*(doc->maxYCoord - doc->minYCoord) / view->ScaleDenom), NULL);

      SetViewportOrgEx(memDC->m_hDC, 0, 0, NULL);
      SetViewportExtEx(memDC->m_hDC, rect.right, rect.bottom, NULL); // set size of what it is to be shown within
   }

   lastPenColor = CurrPenColor;
   lastPenWidth = CurrPenWidth;
   lastBrushColor = CurrBrushColor;
   lastPen = pen;
   lastBrush = brush;


   HPEN hPen = CreatePen(PS_SOLID, CurrPenWidth, CurrPenColor);
   origMemPen = (HPEN) SelectObject(memDC->m_hDC, hPen);

   HBRUSH hBrush;
   if (doc->getSettings().FillStyle == HS_SOLID)
      hBrush = CreateSolidBrush(CurrBrushColor);
   else
      hBrush = CreateHatchBrush(doc->getSettings().FillStyle, CurrBrushColor);
   origMemBrush = (HBRUSH) SelectObject(memDC->m_hDC, hBrush); 
}

/*********************************************************************
* FreeMemDC      
*/
void FreeMemDC(CCEtoODBView *view, CDC *pDC)
{
   /*DeleteObject(*/SelectObject(memDC->m_hDC, origMemBitmap);
   DeleteObject(SelectObject(memDC->m_hDC, origMemPen));
   DeleteObject(SelectObject(memDC->m_hDC, origMemBrush));

   CurrPenColor = lastPenColor;
   CurrPenWidth = lastPenWidth;
   CurrBrushColor = lastBrushColor;
   pen = lastPen;
   brush = lastBrush;

   delete memBitmap;
   delete memDC;

   if (!pDC->IsPrinting())
   {
      CRect rect;
      view->GetClientRect(&rect);

      CCEtoODBDoc *doc = view->GetDocument();

      SetMapMode(pDC->m_hDC, MM_ISOTROPIC);

      SetWindowOrgEx(pDC->m_hDC, view->GetXPos(), view->GetYPos(), NULL); // scroller pos
      SetWindowExtEx(pDC->m_hDC, (int)(1.0*(doc->maxXCoord - doc->minXCoord) / view->ScaleDenom), 
            (int)(-1.0*(doc->maxYCoord - doc->minYCoord) / view->ScaleDenom), NULL);

      SetViewportOrgEx(pDC->m_hDC, 0, 0, NULL);
      SetViewportExtEx(pDC->m_hDC, rect.right, rect.bottom, NULL); // set size of what it is to be shown within
   }
}

/*********************************************************************
* DrawTransparent      
*/
BOOL DrawTransparent (HDC hdc, HBITMAP hBitmap, COLORREF crColor, int x, int y)
{
   BITMAP bm;
   HIMAGELIST hImageList;                                    
   BOOL bResult;
   bResult = FALSE;
   if (GetObject(hBitmap, sizeof (bm), &bm)) 
   {
      hImageList = ImageList_Create(bm.bmWidth, bm.bmHeight, ILC_MASK,  1, 1);
      if (hImageList != NULL) 
      {
         if (ImageList_AddMasked(hImageList, hBitmap, crColor) != -1)
            bResult = ImageList_Draw(hImageList, 0, hdc, x, y, ILD_TRANSPARENT);
      }
      DeleteObject(hImageList);
   }
   return bResult;
}

 