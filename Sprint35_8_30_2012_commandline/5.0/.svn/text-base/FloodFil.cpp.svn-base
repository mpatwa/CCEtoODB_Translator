// $Header: /CAMCAD/5.0/FloodFil.cpp 30    6/17/07 8:52p Kurt Van Ness $
  
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           
 
#include "stdafx.h"
#include "ccdoc.h"
#include "floodfil.h"
#include "apertur2.h"
#include "polylib.h"
#include "attrib.h"
#include "net_util.h"
#include "sm_anal.h"
#include "find.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView *activeView; // from CCVIEW.CPP
extern BOOL bmpBW; // from NET_DRV.CPP
extern BOOL algRecursive; // from NET_DRV.CPP
extern BOOL bmpStart; // from NET_DRV.CPP
extern BOOL bmpFills; // from NET_DRV.CPP

static CCEtoODBDoc *doc;
static double scaleFactor;
static CDC *memDC, *voidDC;
static CBitmap *memBitmap, *origMemBitmap, *voidBitmap, *origVoidBitmap;
static WORD oldNetnameKW;
static WORD layerNetnameKW;
static CStringList *layerNetnamesList;
static FileStruct *file;
static COLORREF backColor, drawColor, fillColor, borderColor;
static double resolution;
static BOOL OneToOne;
static BOOL Animate;
static UINT milliSecs;

BOOL AngleArc_Lines95(CDC *dc, int X, int Y, double radius, double sa, double da);
static void CreateMyDC(CDC **dc, CBitmap **newBitmap, CBitmap **origBitmap, int bitCount);
static void CreateMyBitmap(CDC *dc, CBitmap **newBitmap, int bitCount);

static void FF_DrawPolys(CDC *dc, CPolyList *polylist, 
      Mat2x2 *m, double insert_x, double insert_y, double scale, int mirror, 
      BOOL negative, COLORREF background);
static void FF_DrawPoly(CDC *dc, CPoly *poly, COLORREF color, int PenWidth, BOOL Filled,
      Mat2x2 *m, double insert_x, double insert_y, double scale, int mirror);

/******************************************************************************
* FF_Start
*/
int FF_Start(CCEtoODBDoc *Doc, double Resolution)
{
   doc = Doc;
   resolution = Resolution;
   Animate = FALSE;
   milliSecs = 1000;

   oldNetnameKW = doc->RegisterKeyWord("OldNetname", 1, VT_STRING);
   layerNetnameKW = doc->RegisterKeyWord(ATT_NETNAME, 1, VT_STRING);


   // scaleFactor & resolution
   scaleFactor = round(1.0 / resolution);
      

   // memDC
   CreateMyDC(&memDC, &memBitmap, &origMemBitmap, 16);

   if (!memBitmap)
   {
      ErrorMessage("Bitmap too large.  Try making the Page Size smaller or, for Netlist derive, decreasing the resolution.\n\nExample:\n   Old Value = 0.0001\n   New Value = 0.001", "Failure");
      return FALSE;
   }

   if (!memBitmap->m_hObject)
   {
      ErrorMessage("FF Problem #1");
      delete memDC;
      return FALSE;
   }

   // set colors
   backColor = RGB(0, 0, 0); // BLACK
   drawColor = RGB(255, 255, 255); // WHITE
   fillColor = RGB(0, 255, 0); // GREEN
   borderColor = RGB(255, 0, 0); // RED

   // draw page
   HBRUSH hBrush = CreateSolidBrush(backColor);
   HBRUSH oldBrush = (HBRUSH) SelectObject(memDC->m_hDC, hBrush);
   memDC->Rectangle(round(doc->getSettings().getXmin() * scaleFactor), round(doc->getSettings().getYmin() * scaleFactor), 
         round(doc->getSettings().getXmax() * scaleFactor), round(doc->getSettings().getYmax() * scaleFactor));
   DeleteObject(SelectObject(memDC->m_hDC, oldBrush)); 

   return TRUE;
}

/******************************************************************************
* CreateMyDC
*/
void CreateMyDC(CDC **dc, CBitmap **newBitmap, CBitmap **origBitmap, int bitCount)
{
   CPaintDC paintDC(activeView);

   // creat dc
   *dc = new CDC;
   (*dc)->CreateCompatibleDC(&paintDC);

   // Prepare DC
   (*dc)->SetMapMode(MM_ISOTROPIC);

   (*dc)->SetWindowOrg(0, 0);
   (*dc)->SetWindowExt(1, -1);

   (*dc)->SetViewportOrg(round(-doc->getSettings().getXmin() * scaleFactor), round(doc->getSettings().getYmax() * scaleFactor));
   (*dc)->SetViewportExt(1, 1);


   // create bitmap
   CreateMyBitmap(*dc, newBitmap, bitCount);

   // select bitmap into DC
   *origBitmap = (*dc)->SelectObject(*newBitmap);
}

/******************************************************************************
* CreateMyBitmap
*/
void CreateMyBitmap(CDC *dc, CBitmap **newBitmap, int bitCount)
{
   BITMAPINFO bitmapInfo;
   ZeroMemory(&bitmapInfo, sizeof( BITMAPINFO ) );
   BITMAPINFOHEADER& bitmapInfoHeader = bitmapInfo.bmiHeader;
   ZeroMemory(&bitmapInfoHeader, sizeof( BITMAPINFOHEADER ) );
   bitmapInfoHeader.biSize = sizeof( BITMAPINFOHEADER );
   bitmapInfoHeader.biWidth = round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor);
   bitmapInfoHeader.biHeight = round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor);                                        
   bitmapInfoHeader.biCompression = BI_RGB;
   bitmapInfoHeader.biPlanes = 1;
   bitmapInfoHeader.biBitCount = bitCount; //16;//8;

   LPBYTE pBits = NULL;
   HBITMAP hbmp = CreateDIBSection(dc->m_hDC, &bitmapInfo, DIB_PAL_COLORS, (void**)(&pBits), NULL, 0);
   
   *newBitmap = CBitmap::FromHandle(hbmp);
}

/******************************************************************************
* FF_End
*/
void FF_End()
{
   memDC->SelectObject(origMemBitmap);
   DeleteObject(memBitmap->m_hObject);
   delete memDC;
}

void Fill(int x, int y)
{
   COLORREF colorToFill = memDC->GetPixel(x, y);
   if (colorToFill == fillColor)
      return;

   memDC->SetPixel(x, y, fillColor);

   // fill this y left and right
   int left = x, right = x;
   while (memDC->GetPixel(right+1, y) == colorToFill)
   {
      memDC->SetPixel(right+1, y, fillColor);
      right++;
   }
   while (memDC->GetPixel(left-1, y) == colorToFill)
   {
      memDC->SetPixel(left-1, y, fillColor);
      left--;
   }

   // recurse above and below each pixel on this line
	int i = 0;
   for (i=left; i<=right; i++)
   {
      if (memDC->GetPixel(i, y+1) == colorToFill)
         Fill(i, y+1);
   }
   for (i=left; i<=right; i++)
   {
      if (memDC->GetPixel(i, y-1) == colorToFill)
         Fill(i, y-1);
   }

/*
   if (memDC->GetPixel(x, y) == drawColor)
   {
      memDC->SetPixel(x, y, fillColor);
      Fill(x+1, y);
      Fill(x-1, y);
      Fill(x, y+1);
      Fill(x, y-1);
   }*/
}

/******************************************************************************
* FF_Fill
*/
int FF_Fill(double x, double y, BOOL NegativeLayer)
{
   COLORREF origColor = memDC->GetPixel(round(x * scaleFactor), round(y * scaleFactor));
   if (origColor == -1)
      return 0;

   if (NegativeLayer)
   {
      if (origColor != backColor)
         return -1;
   }

   if (algRecursive)
   {
      Fill(round(x*scaleFactor), round(y*scaleFactor));
      return 1;
   }
   else
   {
      CBrush brush(fillColor);
      CBrush *origBrush = memDC->SelectObject(&brush);

      int res = memDC->ExtFloodFill(round(x * scaleFactor), round(y *scaleFactor), origColor, FLOODFILLSURFACE);

      memDC->SelectObject(origBrush);

      return res;
   }
}

/******************************************************************************
* FF_Query
*/
BOOL FF_Query(double x, double y)
{
   COLORREF pixelColor = memDC->GetPixel(round(x * scaleFactor), round(y * scaleFactor));
   return (fillColor == pixelColor);
}

/******************************************************************************
* FF_ShowBitmap
*/
void FF_ShowBitmap(CString text, CDC *dc)
{
   DisplayDlg dlg;
   dlg.m_text = text;
   if (dc)
      dlg.dc = dc;
   else
      dlg.dc = memDC;
   dlg.DoModal();
}


/******************************************************************************
* FF_Draw
*/
void FF_Draw(FileStruct *File, int drawLayer, CStringList *LayerNetnamesList)
{
   layerNetnamesList = LayerNetnamesList;

   file = File;

   FF_DrawBlock(file->getBlock(), file->getInsertX(), file->getInsertY(), file->getRotation(), file->getScale(), file->isMirrored(), FALSE, 
         "", "", -1, drawLayer, FALSE);
}

/******************************************************************************
* FF_DrawSelected
*/
void FF_DrawSelected(CCEtoODBDoc *doc)
{
   Mat2x2 m;

   POSITION selPos = doc->SelectList.GetHeadPosition();
   while (selPos)
   {
      SelectStruct *s = doc->SelectList.GetNext(selPos);

      RotMat2(&m, s->rotation);

      switch (s->getData()->getDataType())
      {
         case T_POINT:
         {
            Point2 p;
            p.x = s->getData()->getPoint()->x * s->scale;
            if (s->mirror) p.x = -p.x;
            p.y = s->getData()->getPoint()->y * s->scale;
            TransPoint2(&p, 1, &m, s->insert_x, s->insert_y);

            memDC->SetPixelV(round(p.x * scaleFactor), round(p.y * scaleFactor), drawColor);
         }
         break;

         case T_POLY:
            FF_DrawPolys(memDC, s->getData()->getPolyList(), &m, s->insert_x, s->insert_y, s->scale, s->mirror, s->getData()->isNegative(), 0);
         break;
      }
   }
}

/******************************************************************************
* FF_DrawBlock
*/
void FF_DrawBlock(BlockStruct *block, double insert_x, double insert_y, double rotation, double scale, int mirror, BOOL negative, 
      CString insertNetName, CString compName, int insertLayer, int drawLayer, BOOL IgnoreLayerNetnames)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      
      if (!data)
         continue;
      // figure LAYER
      int layer;
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layer = insertLayer;
      else
         layer = data->getLayerIndex();

      if ((mirror & MIRROR_LAYERS) && (data->getLayerIndex() != -1))
         layer = doc->getLayerArray()[data->getLayerIndex()]->getMirroredLayerIndex();


      switch (data->getDataType())
      {
         case T_TEXT:
         break;

         case T_INSERT:
         {
            // pass original insert's netname through recursion
            CString subInsertNetName = insertNetName;
            Attrib* attrib;

            if (data->getAttributesRef() && data->getAttributesRef()->Lookup(oldNetnameKW, attrib))
            {
               subInsertNetName = attrib->getStringValue();
            }

            BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (!subblock)
               continue;

            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            point2.y = data->getInsert()->getOriginY() * scale;

            if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BOOL 
               DrawAp = FALSE, 
               DrawSubblock = FALSE,
               IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
               IsAp = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
            double 
               _x = point2.x,
               _y = point2.y,
               _rot = rotation,
               _scale = scale;
            int 
               _mirror = mirror;

            // TOOL
            if (IsTool)
            {
               if (subblock->getToolDisplay()) // use another block's graphics to display this tool
               {
                  subblock = doc->Find_Block_by_Num(subblock->getToolBlockNumber());

                  if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                     DrawAp = TRUE;
                  else
                     DrawSubblock = TRUE;
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
                  subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));
                  DrawSubblock = TRUE;
               }

               // NORMAL APERTURE
               else
               {
                  if (subblock->getShape() == T_UNDEFINED)
                  {
                     continue;
                  }
                  else
                  {
                     if (layer != drawLayer)
                        continue;

                     // skip appropriate vias for power planes
                     if (!IgnoreLayerNetnames && layerNetnamesList->GetCount() && layerNetnamesList->Find(subInsertNetName))
                        continue;
                     
                     CUseVoidsInDonuts::setUseVoidsInDonuts(false);
                     CPolyList *polylist = ApertureToPoly(subblock, data->getInsert(), mirror);
                     CUseVoidsInDonuts::setUseVoidsInDonuts(true);
                     if (polylist)
                     {
                        FF_DrawPolys(memDC, polylist, &m, insert_x, insert_y, scale, mirror, data->isNegative() || negative, 0);
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
               _scale = scale * data->getInsert()->getScale();
            }


            // Draw Block
            if (DrawSubblock)
            {
               // data->getInsert()->refname can be NULL or emptry
               if (!IgnoreLayerNetnames && layerNetnamesList->GetCount() && !compName.IsEmpty() && data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
               {
                  NetStruct *net;
                  if (FindCompPin(file, compName, data->getInsert()->getRefname(), &net))
                  {
                     if (layerNetnamesList->Find(net->getNetName()))
                        continue;
                  }
               }
               CString insertRefName = compName;
               if (data->getInsert()->getRefname())
                  insertRefName = data->getInsert()->getRefname();
               FF_DrawBlock(subblock, _x, _y, _rot, _scale, _mirror, data->isNegative() || negative,
                     subInsertNetName, insertRefName, layer, drawLayer, IgnoreLayerNetnames);
            }
         }
         break;

         case T_POINT:
         {
            if (layer != drawLayer)
               continue;

            Point2 p;
            p.x = data->getPoint()->x * scale;
            if (mirror) p.x = -p.x;
            p.y = data->getPoint()->y * scale;
            TransPoint2(&p, 1, &m, insert_x, insert_y);

            memDC->SetPixelV(round(p.x * scaleFactor), round(p.y * scaleFactor), drawColor);
         }
         break;

         case T_POLY:
         {
            switch (data->getGraphicClass())  
            {
               case GR_CLASS_BOARDOUTLINE:
                  FF_DrawPolys(memDC, data->getPolyList(), &m, insert_x, insert_y, scale, mirror, data->isNegative(), 0);
               break;

               case graphicClassNormal:
               case GR_CLASS_TRACEFANOUT:
               case GR_CLASS_ETCH:
                  {
                     if (layer != drawLayer)
                        continue;

                     // if has voids
                     BOOL Voids = FALSE;
                     POSITION polyPos = data->getPolyList()->GetHeadPosition();
                     while (polyPos)
                     {
                        CPoly *poly = data->getPolyList()->GetNext(polyPos);
                        if (poly->isVoid())
                        {
                           Voids = TRUE;
                           break;
                        }
                     }

                     if (!Voids)
                        FF_DrawPolys(memDC, data->getPolyList(), &m, insert_x, insert_y, scale, mirror, data->isNegative(), 0);
                     else
                     {
//FF_ShowBitmap("Before Void Draw");

                        int left = round(doc->getSettings().getXmin() * scaleFactor);
                        int top = round(doc->getSettings().getYmax() * scaleFactor);
                        int width = round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor);
                        int height = round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor);

                        CreateMyDC(&voidDC, &voidBitmap, &origVoidBitmap, 16);

                        voidDC->SelectStockObject(BLACK_BRUSH);
                        voidDC->Rectangle(round(doc->getSettings().getXmin() * scaleFactor), round(doc->getSettings().getYmin() * scaleFactor), 
                              round(doc->getSettings().getXmax() * scaleFactor), round(doc->getSettings().getYmax() * scaleFactor));

                        FF_DrawPolys(voidDC, data->getPolyList(), &m, insert_x, insert_y, scale, mirror, data->isNegative(), RGB(0, 0, 0));
//FF_ShowBitmap("Void DC", voidDC);

                        BitBlt(memDC->m_hDC, left, top, width, -height, 
                              voidDC->m_hDC, left, top, SRCPAINT);
//FF_ShowBitmap("After BitBlt");

                        voidDC->SelectObject(origVoidBitmap);
                        DeleteObject(voidBitmap->m_hObject);
                        delete voidDC;
                     }
                  }
               break;

               default:
                  // do not evaluate graphic, such as ROUTE KEEPOUT, KEEPIN etc...
               break;
            }
         }
         break;
      }
   }
}

/******************************************************************************
* FF_DrawPolys
*/
void FF_DrawPolys(CDC *dc, CPolyList *polylist, 
      Mat2x2 *m, double insert_x, double insert_y, double scale, int mirror, 
      BOOL negative, COLORREF background)
{
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = polylist->GetNext(polyPos);

      COLORREF color;
      if (poly->isFloodBoundary())
         color = borderColor;
      else if (negative)
         color = backColor;
      else
         color = drawColor;

      int PenWidth = 0;
      if (poly->getWidthIndex() >= 0)
         PenWidth = round(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale * scaleFactor);

      if (poly->isVoid()) // first draw zero width cutout before drawing border normal
         FF_DrawPoly(dc, poly, background, 0, poly->isFilled(), m, insert_x, insert_y, scale, mirror);

      FF_DrawPoly(dc, poly, color, PenWidth, poly->isFilled() && !poly->isVoid(), m, insert_x, insert_y, scale, mirror);
   }
}

/******************************************************************************
* FF_DrawPoly
*/
void FF_DrawPoly(CDC *dc, CPoly *poly, COLORREF color, int PenWidth, BOOL Filled,
      Mat2x2 *m, double insert_x, double insert_y, double scale, int mirror)
{
   if (Filled)
      dc->BeginPath(); 

   HPEN orgPen, hPen;
   hPen = CreatePen(PS_SOLID, PenWidth, color);
   orgPen = (HPEN) SelectObject(dc->m_hDC, hPen);

   HBRUSH hBrush, orgBrush;
   hBrush = CreateSolidBrush(color);
   orgBrush = (HBRUSH) SelectObject(dc->m_hDC, hBrush);

   double bulge, lastx, lasty;

   POSITION pntPos = poly->getPntList().GetHeadPosition();
   BOOL First = TRUE;
   while (pntPos != NULL)
   {
      CPnt *pnt = poly->getPntList().GetNext(pntPos);
      Point2 p;
      p.x = pnt->x * scale;
      if (mirror) p.x = -p.x;
      p.y = pnt->y * scale;
      TransPoint2(&p, 1, m, insert_x, insert_y);
      if (First)
         dc->MoveTo(round(p.x * scaleFactor), round(p.y * scaleFactor));
      else
      {
         double length = sqrt((lastx-p.x)*(lastx-p.x) + (lasty-p.y)*(lasty-p.y));

         if (fabs(bulge) < BULGE_THRESHOLD || length < 2*resolution)
            dc->LineTo(round(p.x * scaleFactor), round(p.y * scaleFactor));
         else
         {
            double da, sa, r, cx, cy;
            da = atan(bulge) * 4;
            if (mirror & MIRROR_FLIP) da = -da;
            ArcPoint2Angle(lastx, lasty, p.x, p.y, da, &cx, &cy, &r, &sa);

            if (Platform == WINNT) // Windows NT
            {
               dc->SetArcDirection(da<0.0 ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE);
               dc->ArcTo(lround((cx - r) * scaleFactor), lround((cy + r) * scaleFactor), 
                     lround((cx + r) * scaleFactor), lround((cy - r) * scaleFactor),
                     lround(lastx * scaleFactor), lround(lasty * scaleFactor), 
                     lround(p.x * scaleFactor), lround(p.y * scaleFactor));  
               dc->SetArcDirection(AD_COUNTERCLOCKWISE);
            }
            else // Windows 95
            {
               if (cx * scaleFactor > doc->maxXCoord || cx * scaleFactor < doc->minXCoord || 
                     cy * scaleFactor > doc->maxYCoord || cy * scaleFactor < doc->minYCoord)
                  dc->LineTo((int)(p.x * scaleFactor),(int)(p.y * scaleFactor));
               else if (Filled)
                  AngleArc_Lines95(dc, (int)(cx * scaleFactor), (int)(cy * scaleFactor), 
                        r * scaleFactor, sa, da);
               else
               {
                  if (da < 0.0) // clockwise
                  {     
                     dc->Arc((int)((cx - r) * scaleFactor), (int)((cy + r) * scaleFactor), 
                           (int)((cx + r) * scaleFactor), (int)((cy - r) * scaleFactor),
                           (int)(p.x * scaleFactor), (int)(p.y * scaleFactor),
                           (int)(lastx * scaleFactor), (int)(lasty * scaleFactor));
                  }
                  else // counter-clockwise
                  {
                     dc->Arc((int)((cx - r) * scaleFactor), (int)((cy + r) * scaleFactor), 
                           (int)((cx + r) * scaleFactor), (int)((cy - r) * scaleFactor),
                           (int)(lastx * scaleFactor), (int)(lasty * scaleFactor), 
                           (int)(p.x * scaleFactor), 
                           (int)(p.y * scaleFactor));  
                  }
                  dc->MoveTo((int)(p.x * scaleFactor),(int)(p.y * scaleFactor));
               }
            }

         }
      }
      First = FALSE;

      bulge = pnt->bulge;
      lastx = p.x;
      lasty = p.y;
   }

   if (Filled)
   {
      dc->EndPath();
      dc->StrokeAndFillPath();
   }

   DeleteObject(SelectObject(dc->m_hDC, orgPen));
   DeleteObject(SelectObject(dc->m_hDC, orgBrush));
}

/******************************************************************************
* FF_SM_DrawBlock
*/
void FF_SM_DrawBlock(SMApList *smApList, BlockStruct *block, double insert_x, double insert_y, double rotation, double scale, int mirror, 
      int insertLayer, int drawLayer, BOOL FoundFlash, CString compName, CString pinName, DataStruct *padstack)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // figure LAYER
      int layer;
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layer = insertLayer;
      else
         layer = data->getLayerIndex();

      if ((mirror & MIRROR_LAYERS) && (data->getLayerIndex() != -1))
         layer = doc->getLayerArray()[data->getLayerIndex()]->getMirroredLayerIndex();


      switch (data->getDataType())
      {
         case T_TEXT:
         break;

         case T_INSERT:
         {
            BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
            if (!subblock)
               continue;

            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            point2.y = data->getInsert()->getOriginY() * scale;
            if (mirror & MIRROR_FLIP)  point2.x = -point2.x;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BOOL 
               DrawAp = FALSE, 
               DrawSubblock = FALSE,
               IsTool = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL),
               IsAp = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
            double 
               _x = point2.x,
               _y = point2.y,
               _rot = rotation,
               _scale = scale;
            int 
               _mirror = mirror;
            int 
               _layer = layer;

            BOOL Soldermask = FALSE;

            // TOOL
            if (IsTool)
            {
               if (subblock->getToolDisplay()) // use another block's graphics to display this tool
               {
                  subblock = doc->Find_Block_by_Num(subblock->getToolBlockNumber());

                  if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                     DrawAp = TRUE;
                  else
                     DrawSubblock = TRUE;
               }

               Soldermask = TRUE;
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
                  subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));
                  DrawSubblock = TRUE;
               }

               // NORMAL APERTURE
               else
               {
                  if (subblock->getShape() == T_UNDEFINED)
                  {
                     continue;
                  }
                  else
                  {
                     if (layer != drawLayer)
                        continue;

                     CUseVoidsInDonuts::setUseVoidsInDonuts(false);
                     CPolyList *polylist = ApertureToPoly(subblock, data->getInsert(), mirror);
                     CUseVoidsInDonuts::setUseVoidsInDonuts(true);
                     if (polylist)
                     {
                        FF_DrawPolys(memDC, polylist, &m, insert_x, insert_y, scale, mirror, 0, 0);
                        FreePolyList(polylist);
                     }
                  }
               } // end NORMAL APERTURE

               Soldermask = TRUE;

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
               _scale = scale * data->getInsert()->getScale();
            }


            // Draw Block
            if (DrawSubblock)
            {
               CString newCompName = compName;
               CString newPinName = pinName;
               DataStruct *newPadstack = padstack;

               switch (data->getInsert()->getInsertType())
               {
               case INSERTTYPE_PCBCOMPONENT:
                  if (data->getInsert()->getRefname()) 
                     newCompName = data->getInsert()->getRefname();
                  break;
               case INSERTTYPE_PIN:
                  if (data->getInsert()->getRefname()) 
                     newPinName = data->getInsert()->getRefname();
                  break;
               case INSERTTYPE_VIA:
                  newPadstack = data;
                  break;
               }

               FF_SM_DrawBlock(smApList, subblock, _x, _y, _rot, _scale, _mirror, layer, drawLayer, IsAp || FoundFlash,
                     newCompName, newPinName, newPadstack);
            }        

            if (Soldermask)
            {
               SMApStruct *smAp = new SMApStruct;
               smAp->x = _x;
               smAp->y = _y;
               
               smAp->map = &data->getAttributesRef();
               if (padstack)
                  smAp->map = &padstack->getAttributesRef();
               if (!compName.IsEmpty() && !pinName.IsEmpty())
               {
                  CompPinStruct *cp = FindCompPin_ByName(doc, compName, pinName, NULL, NULL);
                  if (cp)
                     smAp->map = &cp->getAttributesRef();
               }

               smApList->AddTail(smAp);
            }
         }
         break;

         case T_POINT:
         {
            if (layer != drawLayer)
               continue;

            Point2 p;
            p.x = data->getPoint()->x * scale;
            if (mirror) p.x = -p.x;
            p.y = data->getPoint()->y * scale;
            TransPoint2(&p, 1, &m, insert_x, insert_y);

            memDC->SetPixelV(round(p.x * scaleFactor), round(p.y * scaleFactor), drawColor);
         }
         break;

         case T_POLY:
            if (FoundFlash && layer == drawLayer)
               FF_DrawPolys(memDC, data->getPolyList(), &m, insert_x, insert_y, scale, mirror, data->isNegative(), 0);
            break;
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// DisplayDlg dialog
DisplayDlg::DisplayDlg(CWnd* pParent /*=NULL*/)
   : CDialog(DisplayDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(DisplayDlg)
   m_fills = bmpFills;
   m_starts = bmpStart;
   m_oneToOne = OneToOne;
   m_animate = Animate;
   m_text = _T("");
   m_milliSecs = milliSecs;
   //}}AFX_DATA_INIT
}

void DisplayDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DisplayDlg)
   DDX_Check(pDX, IDC_FILLS, m_fills);
   DDX_Check(pDX, IDC_STARTS, m_starts);
   DDX_Check(pDX, IDC_TOGGLE, m_oneToOne);
   DDX_Check(pDX, IDC_ANIMATE, m_animate);
   DDX_Text(pDX, IDC_TEXT, m_text);
   DDX_Text(pDX, IDC_MILLISECS, m_milliSecs);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DisplayDlg, CDialog)
   //{{AFX_MSG_MAP(DisplayDlg)
   ON_WM_PAINT()
   ON_WM_HSCROLL()
   ON_WM_VSCROLL()
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_TOGGLE, OnToggle)
   ON_BN_CLICKED(IDC_ANIMATE, OnAnimate)
   ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DisplayDlg message handlers
BOOL DisplayDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   CRect clientRect;
   GetClientRect(&clientRect);

   pageWidth = clientRect.right;
   lineWidth = pageWidth / 10;

   pageHeight = clientRect.bottom;
   lineHeight = pageHeight / 10;

   SetScrollRange(SB_HORZ, 0, round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor) - pageWidth);
   SetScrollRange(SB_VERT, 0, round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor) - pageHeight);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void DisplayDlg::OnSize(UINT nType, int cx, int cy) 
{
   CDialog::OnSize(nType, cx, cy);
   
   CRect clientRect;
   GetClientRect(&clientRect);

   pageWidth = clientRect.right;
   lineWidth = pageWidth / 10;

   pageHeight = clientRect.bottom;
   lineHeight = pageHeight / 10;

   SetScrollRange(SB_HORZ, 0, round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor) - pageWidth);
   SetScrollRange(SB_VERT, 0, round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor) - pageHeight);

   Invalidate();
}

void DisplayDlg::OnPaint() 
{
   CPaintDC dlgDC(this); // device context for painting

   dlgDC.SetMapMode(MM_ISOTROPIC);

   dlgDC.SetWindowOrg(0, 0);
   dlgDC.SetViewportOrg(0, 0);
   if (m_oneToOne)
   {
      dlgDC.SetWindowExt(1, -1);
      dlgDC.SetViewportExt(1, 1);
   }
   else
   {
      CRect clientRect;
      GetClientRect(&clientRect);
      dlgDC.SetWindowExt(round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor), round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor));
      dlgDC.SetViewportExt(clientRect.right, -clientRect.bottom);
   }
   
   dlgDC.BitBlt(-GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT), round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor), 
         -round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor), 
         dc, 
         round(doc->getSettings().getXmin() * scaleFactor), round(doc->getSettings().getYmax() * scaleFactor),
         SRCCOPY);

   if (m_animate)
      OnAnimate();
}

void DisplayDlg::OnToggle() 
{
   UpdateData();
   if (!m_oneToOne)
   {
      SetScrollPos(SB_HORZ, 0);     
      SetScrollPos(SB_VERT, 0);     
   }
   Invalidate();  
}

void DisplayDlg::OnCancel() 
{
   UpdateData();
   bmpStart = m_starts;
   bmpFills = m_fills;
   OneToOne = m_oneToOne;
   Animate = m_animate;
   milliSecs = m_milliSecs;
   CDialog::OnCancel();
}

void DisplayDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   switch (nSBCode)
   {
   case SB_LEFT:
      SetScrollPos(SB_HORZ, 0);     
      break;
   case SB_RIGHT:
      SetScrollPos(SB_HORZ, round((doc->getSettings().getXmax() - doc->getSettings().getXmin()) * scaleFactor) - pageWidth);    
      break;

   case SB_PAGELEFT:
      SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - pageWidth);
      break;
   case SB_PAGERIGHT:
      SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + pageWidth);
      break;

   case SB_LINELEFT:
      SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - lineWidth);
      break;
   case SB_LINERIGHT:
      SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + lineWidth);
      break;

   case SB_THUMBPOSITION:
      SetScrollPos(SB_HORZ, nPos);
      break;

   case SB_THUMBTRACK:
      SetScrollPos(SB_HORZ, nPos);
      break;

   case SB_ENDSCROLL:
      break;
   }
   
   Invalidate();

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void DisplayDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   switch (nSBCode)
   {
   case SB_TOP:
      SetScrollPos(SB_VERT, 0);     
      break;
   case SB_BOTTOM:
      SetScrollPos(SB_VERT, round((doc->getSettings().getYmax() - doc->getSettings().getYmin()) * scaleFactor) - pageHeight);      
      break;

   case SB_PAGEUP:
      SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - pageHeight);
      break;
   case SB_PAGEDOWN:
      SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + pageHeight);
      break;

   case SB_LINEUP:
      SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - lineHeight);
      break;
   case SB_LINEDOWN:
      SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + lineHeight);
      break;

   case SB_THUMBPOSITION:
      SetScrollPos(SB_VERT, nPos);
      break;

   case SB_THUMBTRACK:
      SetScrollPos(SB_VERT, nPos);
      break;

   case SB_ENDSCROLL:
      break;
   }
   
   Invalidate();

   CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void DisplayDlg::OnAnimate() 
{
   UpdateData();
   if (m_animate)
   {
      Animate = TRUE;
      m_fills = TRUE;
      m_starts = TRUE;
      UpdateData(FALSE);
      timerID = SetTimer(4, m_milliSecs, NULL);
   }
   else
      KillTimer(timerID);
}

void DisplayDlg::OnTimer(UINT nIDEvent) 
{
   KillTimer(timerID);
   OnCancel();
}
