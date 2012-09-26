// $Header: /CAMCAD/4.6/Draw_Msc.cpp 34    4/18/07 6:40p Kurt Van Ness $
                       
/*****************************************************************************/
/*           
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "Draw.h"
#include "PinLabelSettings.h"
#include "TMState.h"

extern double clipLeft, clipRight, clipBottom, clipTop, viewWidth, viewHeight; // from DRAW.CPP
extern HPEN hideDetailPen; // from DRAW.CPP
extern HBRUSH hideDetailBrush; // from DRAW.CPP

static const double Shade_Margin = 0.005;
static const double Shade_MinSide = 0.05;

/*********************************************************************************
* DrawPinNumsNets
*/
int DrawPinNumsNets(CDC *pDC, CCEtoODBView *view)
{
   CCEtoODBDoc *doc = view->GetDocument();

   BOOL Regen = FALSE; // FALSE until we hit a pin with no pinkoos

   int pageUnits = doc->getSettings().getPageUnits();
   double pageInchesPerPageUnits   = Units_Factor(pageUnits,UNIT_INCHES);
   double logicalUnitsPerPageUnits = view->getScaleFactor();

   CSize windowExtentsLogicalUnits = pDC->GetWindowExt();
   CSize viewportExtentsPixels     = pDC->GetViewportExt();
   double xLogicalUnitsPerPixel    = ((double)windowExtentsLogicalUnits.cx)/viewportExtentsPixels.cx;
   double yLogicalUnitsPerPixel    = ((double)windowExtentsLogicalUnits.cy)/viewportExtentsPixels.cy;

   double heightPageUnits    = doc->getSettings().getPinLabelSettings().getTextHeightInches() / pageInchesPerPageUnits;
   double heightLogicalUnits = heightPageUnits * logicalUnitsPerPageUnits;
   int heightPixels          = round(heightLogicalUnits / xLogicalUnitsPerPixel);

   bool tooSmallFlag = (heightPixels <  doc->getSettings().getPinLabelSettings().getMinTextHeightPixels());
   bool tooBigFlag   = (heightPixels >  doc->getSettings().getPinLabelSettings().getMaxTextHeightPixels());
   bool textBoxFlag  = (heightPixels <= doc->getSettings().getPinLabelSettings().getTextBoxPixels());

   if      (textBoxFlag ) heightPixels = doc->getSettings().getPinLabelSettings().getTextBoxPixels();
   else if (tooSmallFlag) heightPixels = doc->getSettings().getPinLabelSettings().getMinTextHeightPixels();
   else if (tooBigFlag  ) heightPixels = doc->getSettings().getPinLabelSettings().getMaxTextHeightPixels();

   CString faceName("Courier");

   LOGFONT newLogFont;
   doc->getSettings().getPinLabelSettings().getLogFont(newLogFont);
   newLogFont.lfHeight         = (int)(heightPixels * xLogicalUnitsPerPixel);

   double textRotation = doc->getSettings().getPinLabelSettings().getTextDegrees();
   CTMatrix matrix;
   matrix.rotateDegrees(textRotation);

   CFont newFont;
   newFont.CreateFontIndirect(&newLogFont);

   CFont* oldFont = pDC->SelectObject(&newFont);

   // - - - -

   for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet())
      {
         continue;
      }

      if (file->getBlock() == NULL)
      {
         continue;
      }

      double fileX = file->getInsertX();
      double fileRot = file->getRotation();
      int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

      if (doc->getBottomView())
      {
         fileX = -fileX;
         fileRot = -fileRot;
      }

      Mat2x2 m;
      RotMat2(&m, fileRot);

      pDC->SetBkMode(OPAQUE);

      for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (net->getFlags() & NETFLAG_UNUSEDNET && (doc->showPinnetsTop || doc->showPinnetsBottom))
         {
            continue;
         }

         for (POSITION compPinPos = net->getHeadCompPinPosition();compPinPos != NULL;)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            if (!compPin->getPinCoordinatesComplete())
            {
               Regen = TRUE;
               continue;
            }

            if ( ((doc->showPinnrsTop    || doc->showPinnetsTop   ) && (compPin->isVisibleTop()   )) ||
                 ((doc->showPinnrsBottom || doc->showPinnetsBottom) && (compPin->isVisibleBottom()))    )
            {
               Point2 point2;
               point2.x = compPin->getOriginX() * file->getScale();
               point2.y = compPin->getOriginY() * file->getScale(); 

               // add center extent of the padstack as offset
               BlockStruct *padstackBlock = doc->getBlockAt(compPin->getPadstackBlockNumber());              
               if(padstackBlock)
               {
                  CPoint2d centerPoint = padstackBlock->getExtent().getCenter();
                  point2.x += centerPoint.x;
                  point2.y += centerPoint.y;
               }

               if (fileMirror & MIRROR_FLIP)
               {
                  point2.x = -point2.x;
               }

               TransPoint2(&point2, 1, &m, fileX, file->getInsertY());

               CString label;

               if (textBoxFlag)
               {
                  label = " ";
               }
               else if (doc->showPinnrsTop || doc->showPinnrsBottom)
               {
                  label = compPin->getPinName();
               }
               else
               {
                  label = net->getNetName();
               }

               CSize textExtents = pDC->GetOutputTextExtent(label);

               CPoint2d offset(0.,textExtents.cy/2.);
               matrix.transform(offset);

               pDC->TextOut(round((point2.x * view->getScaleFactor()) + offset.x),round((point2.y * view->getScaleFactor()) + offset.y) ,label);
            }
         }
      }

      pDC->SetBkMode(TRANSPARENT);
   }

   pDC->SelectObject(oldFont);

   return Regen;
}

/*********************************************************************************
* TransformInsertExtents
*/
void TransformInsertExtents(double *xmin, double *xmax, double *ymin, double *ymax,
      BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror)
{
   if (!block->extentIsValid())
      return;

   CExtent extent;

   *xmin = *ymin =  FLT_MAX;
   *xmax = *ymax = -FLT_MAX;

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

   if (extent.isValid())
   {
      *xmin = extent.getXmin();
      *xmax = extent.getXmax();
      *ymin = extent.getYmin();
      *ymax = extent.getYmax();
   }
}

/*********************************************************************************
* NeedToDrawBlock
*/
BOOL NeedToDrawBlock(SettingsStruct *Settings, BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror, 
      CDC *pDC, double scaleFactor)
{
   if (pDC->IsPrinting())
      return TRUE;

   if (!Settings->HideDetail)
      return TRUE;

   if (!block->extentIsValid())
      return TRUE;

   double xmin, xmax, ymin, ymax;
   TransformInsertExtents(&xmin, &xmax, &ymin, &ymax, block, insert_x, insert_y, scale, rotation, mirror);

   if (xmin > clipRight)
      return FALSE;

   if (xmax < clipLeft)
      return FALSE;

   if (ymin > clipTop)
      return FALSE;

   if (ymax < clipBottom)
      return FALSE;

   double width, height;
   width = xmax - xmin;
   height = ymax - ymin;

   if (width > Settings->HideDetailRatio * viewWidth)
      return TRUE;
   
   if (height > Settings->HideDetailRatio * viewHeight)
      return TRUE;

   // draw box
   HPEN orgPen;
   HBRUSH orgBrush;
   orgPen = (HPEN) SelectObject(pDC->m_hDC, hideDetailPen);
   orgBrush = (HBRUSH) SelectObject(pDC->m_hDC, hideDetailBrush);
   pDC->Rectangle(round(xmin * scaleFactor), round(ymax * scaleFactor), round(xmax * scaleFactor), round(ymin * scaleFactor));
   SelectObject(pDC->m_hDC, orgPen);
   SelectObject(pDC->m_hDC, orgBrush);

   return FALSE;
}

/****************************************************************************
* DrawShadeRect
*/
void DrawShadeRect(CDC *pDC,const ColorStruct *color, BlockStruct *block, double scaleFactor,
                   double insert_x, double insert_y, double scale, double rotation, int mirror)
{
   double xmin, xmax, ymin, ymax;

   TransformInsertExtents(&xmin, &xmax, &ymin, &ymax, block, insert_x, insert_y, scale, rotation, mirror);

   int orgMixMode = pDC->GetROP2();
   if (color->Transparent)
      pDC->SetROP2(R2_MERGEPEN);
   
   HPEN orgPen, hPen;
   HBRUSH orgBrush, hBrush;
   COLORREF c = RGB(color->red, color->green, color->blue);
   hPen = CreatePen(PS_SOLID, 0, c);
   orgPen = (HPEN) SelectObject(pDC->m_hDC, hPen);

   if (color->Hatched)
      hBrush = CreateHatchBrush(HS_DIAGCROSS, c);
   else
      hBrush = CreateSolidBrush(c);
   orgBrush = (HBRUSH) SelectObject(pDC->m_hDC, hBrush);

   xmin -= Shade_Margin;
   xmax += Shade_Margin;
   ymin -= Shade_Margin;
   ymax += Shade_Margin;
   if (xmax - xmin < Shade_MinSide)
   {
      double temp = (xmax + xmin) / 2;
      xmax = temp + Shade_MinSide / 2;
      xmin = temp - Shade_MinSide / 2;
   }
   if (ymax - ymin < Shade_MinSide)
   {
      double temp = (ymax + ymin) / 2;
      ymax = temp + Shade_MinSide / 2;
      ymin = temp - Shade_MinSide / 2;
   }

   pDC->Rectangle(round(xmin * scaleFactor), round(ymax * scaleFactor), round(xmax * scaleFactor), round(ymin * scaleFactor));

   DeleteObject(SelectObject(pDC->m_hDC, orgPen));
   DeleteObject(SelectObject(pDC->m_hDC, orgBrush));
   pDC->SetROP2(orgMixMode);
}

/******************************************************************************************************
* IsEntityVisible
*/
bool IsEntityVisible(DataStruct *data, BlockStruct **subblock, CCEtoODBDoc *doc, int mirror, int *layer, BOOL IgnoreClass)
{
   bool retval = false;

   while (true)
   {
      if (!IgnoreClass)
      {
         if (data->getDataType() == dataTypeInsert)
         {
            if (!doc->getInsertTypeFilter().contains(data->getInsert()->getInsertType()))
            {
               break;
            }
         }
         else
         {  
            if (!doc->getGraphicsClassFilter().contains((data->getGraphicClass()>0?data->getGraphicClass():0)))
            {
               break;
            }
         }
      }

      if (data->getDataType() == dataTypeInsert)
      {
         *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (*subblock == NULL)
         {
            break;  // a block was deleted ???
         }
      }

      bool insertFlag          = (data->getDataType() == dataTypeInsert);
      bool toolFlag            = insertFlag && (((*subblock)->getFlags() & (BL_TOOL | BL_BLOCK_TOOL)) != 0);
      bool simpleApertureFlag  = insertFlag && (*subblock)->isSimpleAperture();
      bool complexApertureFlag = insertFlag && (*subblock)->isComplexAperture();

      // Check Layer Visible
      if (data->getDataType() != dataTypeInsert)
      {
         // only check layer for non insert entities
         if(data->HasLayerSpan())
         {
            int spanlayer = doc->getLayerArray().FindFirstVisLayerIndxInStackupRange(data->GetPhysicalStartLayer(), data->GetPhysicalEndLayer(), ((mirror & MIRROR_LAYERS) != 0));
            if(spanlayer != -1)
            {
               retval = true;
               *layer = spanlayer;
            }
            else
               retval = false;
         }
         else
            retval = doc->get_layer_visible(*layer, mirror);
      }
      else if (toolFlag)
      {
         retval = doc->get_layer_visible(*layer, mirror);
      }
      else if (simpleApertureFlag)
      {
         bool mirrorFlag = ((mirror & MIRROR_LAYERS) != 0) != data->getInsert()->getLayerMirrored();

         retval = doc->isLayerVisible(*layer, mirrorFlag);
      }
      else if (complexApertureFlag)
      {
         bool mirrorFlag = ((mirror & MIRROR_LAYERS) != 0) != data->getInsert()->getLayerMirrored();

			if (*layer == -1)
         {
            BlockStruct* apertureBlock = doc->getBlockAt((*subblock)->getComplexApertureSubBlockNumber());

            if (!apertureBlock->getDataList().IsEmpty())
            {
               *layer = apertureBlock->getDataList().GetHead()->getLayerIndex();
            }
			}
	       
         retval = doc->isLayerVisible(*layer, mirrorFlag);
      }
      else // other insert
      {
         retval = true;

         //if (*layer == -1 && 
         //      (((*subblock)->getFlags() & BL_APERTURE) || ((*subblock)->getFlags() & BL_BLOCK_APERTURE) ||
         //       ((*subblock)->getFlags() & BL_TOOL    ) || ((*subblock)->getFlags() & BL_BLOCK_TOOL    )    ) )
         //{
         //   if (!(*subblock)->getDataList().IsEmpty() )
         //   {
         //      *layer = (*subblock)->getDataList().GetHead()->getLayerIndex();

         //      if (!doc->get_layer_visible(*layer, mirror))
         //         return FALSE;
         //   }
         //}
      }

      break;
   }

   return retval;
}

/******************************************************************************
* HighlightPolyItem()
*/
void CCEtoODBDoc::HighlightPolyItem(SelectStruct *s, CPoly *poly, CPnt *pnt, BOOL Erase)
{
   int x1, y1, x2, y2, size;

   HPEN orgPen, hPen;

   Mat2x2 m;
   RotMat2(&m, s->rotation);

   CCEtoODBView *view;
   POSITION viewPos = GetFirstViewPosition();
   while (viewPos != NULL)
   {
      view = (CCEtoODBView *)GetNextView(viewPos);

      CClientDC dc(view);
      //view->OnPrepareDC(&dc);

      if (Erase)
         hPen = CreatePen(PS_SOLID, 0, getSettings().Foregrnd);
      else
         hPen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
      orgPen = (HPEN) SelectObject(dc.m_hDC, hPen);

      if (pnt)
      {
         Point2 p;
         p.x = pnt->x * s->scale;
         p.y = pnt->y * s->scale;
         if (s->mirror & MIRROR_FLIP) p.x = -p.x;
         TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

         x1 = (int)(p.x * view->getScaleFactor());
         y1 = (int)(p.y * view->getScaleFactor());

         size = 0; //view->GetCursorSize() * 2;

         dc.MoveTo(x1, y1 - size);
         dc.LineTo(x1, y1 + size);
         dc.MoveTo(x1 - size, y1);
         dc.LineTo(x1 + size, y1);

         // draw bulge direction
         if (pnt != poly->getPntList().GetTail())
         {
            POSITION pntPos = poly->getPntList().Find(pnt);
            poly->getPntList().GetNext(pntPos);
            CPnt *next = poly->getPntList().GetAt(pntPos);

            p.x = next->x * s->scale;
            p.y = next->y * s->scale;
            if (s->mirror & MIRROR_FLIP) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            x2 = (int)(p.x * view->getScaleFactor());
            y2 = (int)(p.y * view->getScaleFactor());

            dc.MoveTo(x1, y1);
            dc.LineTo(x2, y2);
         }
      }
      else // poly
      {
         POSITION pntPos = poly->getPntList().GetHeadPosition();
         if (pntPos)
         {
            CPnt *pt = poly->getPntList().GetNext(pntPos);

            Point2 p;
            p.x = pt->x * s->scale;
            p.y = pt->y * s->scale;
            if (s->mirror & MIRROR_FLIP) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            x1 = (int)(p.x * view->getScaleFactor());
            y1 = (int)(p.y * view->getScaleFactor());

            dc.MoveTo(x1, y1);
         }
         while (pntPos)
         {
            CPnt *next = poly->getPntList().GetNext(pntPos);

            Point2 p;
            p.x = next->x * s->scale;
            p.y = next->y * s->scale;
            if (s->mirror & MIRROR_FLIP) p.x = -p.x;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            x2 = (int)(p.x * view->getScaleFactor());
            y2 = (int)(p.y * view->getScaleFactor());

            dc.LineTo(x2, y2);
         }
      }

      DeleteObject(SelectObject(dc.m_hDC, orgPen));
   }
}

/********************************************************************
* DrawArrow
*/
void DrawArrow(CDC *pDC, double x1, double y1, double x2, double y2, double scaleFactor, double arrowsize, COLORREF color)
{
   HPEN orgPen = (HPEN) SelectObject(pDC->m_hDC, CreatePen(PS_SOLID, 0, color));
   HBRUSH orgBrush = (HBRUSH) SelectObject(pDC->m_hDC, CreateSolidBrush(color));

   double angle = ArcTan2(y1 - y2, x1 - x2);
   const double da = DegToRad(20);

   // arrow shaft
   pDC->MoveTo(round(x1 * scaleFactor), round(y1 * scaleFactor));
   pDC->LineTo(round(x2 * scaleFactor), round(y2 * scaleFactor));

   // arrow head
   double dx, dy;
   pDC->BeginPath();

   pDC->MoveTo(round(x2 * scaleFactor), round(y2 * scaleFactor));

   dx = cos(angle - da) * arrowsize;
   dy = sin(angle - da) * arrowsize;
   pDC->LineTo(round((x2 + dx) * scaleFactor), 
         round((y2 + dy) * scaleFactor));

   dx = cos(angle + da) * arrowsize;
   dy = sin(angle + da) * arrowsize;
   pDC->LineTo(round((x2 + dx) * scaleFactor), 
         round((y2 + dy) * scaleFactor));

   pDC->LineTo(round(x2 * scaleFactor), round(y2 * scaleFactor));

   pDC->EndPath();
   pDC->StrokeAndFillPath();

   DeleteObject(SelectObject(pDC->m_hDC, orgPen)); 
   DeleteObject(SelectObject(pDC->m_hDC, orgBrush));
}




