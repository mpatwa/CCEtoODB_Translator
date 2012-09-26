// $Header: /CAMCAD/5.0/Draw.cpp 84    6/17/07 8:52p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "draw.h"
#include "attrib.h"
#include "apertur2.h"
#include "polylib.h"
#include "xform.h"
#include "dfm.h"

//#define DebuggingCase1953

extern CRgn *resultRegion;

#define BREAK_DRAW_RATE          40

void HighlightPins(CDC *pDC, CCEtoODBView *view);
//BOOL CancelDraw();
void SetupPens(CCEtoODBDoc *doc, CDC *dc, BOOL VoidPoly, int PenWidth, double scale, BOOL showFills, COLORREF voidColor);
void DrawEntityInView(CDC *dc, CCEtoODBView *view, SelectStruct *s, int state, BOOL Override);

COLORREF PenColor;
double clipLeft, clipRight, clipBottom, clipTop, viewWidth, viewHeight;
HPEN hideDetailPen;
HBRUSH hideDetailBrush;
double userDefinedWidth;
int globalShowFills; // -1=normal, 0=off, 1=on

extern BOOL Print_Black; // from PRINT.CPP
extern BOOL Print_Reverse; // from PRINT.CPP
extern BOOL Fit_to_Page; // from PRINT.CPP
extern double PrintFactor; // from PRINT.CPP

static int entityCount;
static double seq_x, seq_y;
static CSelectList DrawOnTop;
extern void BringLayerToFront(CCEtoODBDoc *doc, int layerNum);
/****************************************************************************
* DrawFiles
*/
void CCEtoODBView::DrawFiles(CDC *pDC, double ViewWidth, double ViewHeight)
{
   if (getActiveView() == NULL)
      return;

   CCEtoODBDoc *doc = GetDocument();

   // Case 2166
   // Reset draw-on-top list. Uses of DrawBlock that do not originate through
   // here may have left leftover entries in the list. If those leftover entries
   // refer to blocks in a doc that was just closed, camcad will crash.
   // As of this writing, DrawFiles() is the only "consumer" of DrawOnTop list entries.
   while (!DrawOnTop.IsEmpty())
   {
      SelectStruct *s = DrawOnTop.RemoveHead();
      delete s;
   }

   InitPens(pDC, doc->getSettings().FillStyle);

   viewWidth = ViewWidth;
   viewHeight = ViewHeight;

   RECT rect;
   pDC->GetClipBox(&rect);

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
   clipLeft   = rect.left   * factor;
   clipRight  = rect.right  * factor;
   clipTop    = rect.top    * factor;
   clipBottom = rect.bottom * factor;

   pDC->SetTextAlign(TA_CENTER);
   pDC->SetBkColor(RGB(255, 255, 255));
   pDC->SetBkMode(TRANSPARENT);

   hideDetailPen = CreatePen(PS_SOLID, 0, doc->getSettings().HideDetailColor);
   hideDetailBrush = CreateHatchBrush(HS_DIAGCROSS, doc->getSettings().HideDetailColor);

   entityCount = 0;
   globalShowFills = -1;

   //Stencil Top/Bottom data should be drawn last.
   if(doc->m_firstTimeDraw)
   {
      for (int i=0; i< doc->getMaxLayerIndex(); i++)
      {  
         LayerStruct *layer = doc->getLayer(i);
         if(NULL != layer && doc->isLayerVisible(layer->getLayerIndex(),false))
         {
            if((LAYTYPE_STENCIL_BOTTOM == layer->getLayerType())
               ||(LAYTYPE_STENCIL_TOP == layer->getLayerType()))
            {
               BringLayerToFront(doc,layer->getLayerIndex());
            }
         }
      }
      doc->m_firstTimeDraw = false;
   }
   

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet())
         continue;

      if (file->getBlock() == NULL)
         continue;

#if defined (EnableGeometryTreeImplementation)
      if (doc->m_qfeFilesArray != NULL)
      {
         CGeometryTree* geometryTree = doc->m_qfeFilesArray->GetAt(file->getFileNumber());
         CExtent viewExtent(clipLeft,clipTop,clipRight,clipBottom);
         geometryTree->draw(*doc,*pDC,viewExtent);

         continue;
      }
#endif

      double fileX = file->getInsertX();
      double fileRot = file->getRotation();
      int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);
      if (doc->getBottomView())
      {
         fileX = -fileX;
         fileRot = -fileRot;
      }

      if (!NeedToDrawBlock(&doc->getSettings(), file->getBlock(), fileX, file->getInsertY(), file->getScale(), fileRot, fileMirror,
            pDC, getScaleFactor()))
         continue;

      
      
      // Direction of Travel
      int direction = GetDirectionOfTravel(doc, file->getBlock());
      if (direction)
      {
         doc->validateBlockExtents(file->getBlock());

         double xmin, xmax, ymin, ymax;
         TransformInsertExtents(&xmin, &xmax, &ymin, &ymax, file->getBlock(), fileX, file->getInsertY(), file->getScale(), fileRot, fileMirror);

         double margin;
         double offsetDivisor = -20;
         double x1, y1, x2, y2;

         switch (direction)
         {
         case 3:
            margin = (ymax - ymin) / 4;
            x1 = x2 = margin / offsetDivisor;
            y1 = ymin + margin;
            y2 = ymax - margin;
            break;
         case 4:
            margin = (ymax - ymin) / 4;
            x1 = x2 = margin / offsetDivisor;
            y1 = ymax - margin;
            y2 = ymin + margin;
            break;
         case 2:
            margin = (xmax - xmin) / 4;
            y1 = y2 = margin / offsetDivisor;
            x1 = xmax - margin;
            x2 = xmin + margin;
            break;
         case 1:
         default:
            margin = (xmax - xmin) / 4;
            y1 = y2 = margin / offsetDivisor;
            x1 = xmin + margin;
            x2 = xmax - margin;
            break;
         }

         double arrowSize = margin / 3;
         DrawArrow(pDC, x1, y1, x2, y2, getScaleFactor(), arrowSize, RGB(255, 255, 0));
      }

      if (DrawBlock(pDC, file->getBlock(), fileX, file->getInsertY(), fileRot, fileMirror, file->getScale(), -1, 0, 0, 0, 0L, FALSE)
            == DRAW_BREAK)
         break;
   }

   while (!DrawOnTop.IsEmpty())
   {
      SelectStruct *s = DrawOnTop.RemoveHead();
      DrawEntityInView(pDC, this, s, 0, TRUE);
      delete s;
   }

   DeleteObject(hideDetailPen);
   DeleteObject(hideDetailBrush);

   HighlightPins(pDC, this);

   FreePens(pDC);

   doc->DrawSubSelected();

   // if show pin numbers or netnames
   if (doc->showPinnrsTop || doc->showPinnrsBottom || doc->showPinnetsTop || doc->showPinnetsBottom)
   {
      int Regen = DrawPinNumsNets(pDC, this);
      // if pin(s) missing pinkoos, ask to regenerate (unless switched DontAskRegenPinLocs on)
      if (Regen && !doc->DontAskRegenPinLocs)
      {
         if (ErrorMessage("Would you like to regenerate NC Net / Pic Locs?", "Pins found without Pin Locs", MB_YESNO) == IDYES)
         {
            doc->OnGeneratePinloc();
            DrawPinNumsNets(pDC, this);
         }
         doc->DontAskRegenPinLocs = TRUE;
      }
   }
   cursorFirstDrag = TRUE;
}

/****************************************************************************
* DrawBlock
*/
int CCEtoODBView::DrawBlock(CDC *pDC, BlockStruct *block,
      double insert_x, double insert_y,
      double rotation, int mirror, double scale,
      int insertLayer, int highlight, int recurseLevel, BOOL ColorOverride, COLORREF overrideColor, BOOL negative)
{
   if (block == NULL)
      return DRAW_BREAK;

   CCEtoODBDoc *doc = GetDocument();
   BlockStruct *subblock = NULL;
   Mat2x2 m;
   int layer;

   int seqnum = 1;

   RotMat2(&m, rotation);

   int NetNameKW = doc->IsKeyWord(ATT_NETNAME, 0);

   POSITION dataPos;
   dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getEntityNumber() == 10)
      {
         int iii = 3;
      }

      if (entityCount++ > BREAK_DRAW_RATE && !pDC->IsPrinting()) // How often to flush message queue to check for ESCAPE
      {
         entityCount = 0;

         /*if (CancelDraw())
         {
            FreePens(pDC);
            return DRAW_BREAK;
         }*/
      }

      if (data->isHidden())
         continue;

      if (data->getDataType() == T_BLOB)
      {
         DrawBlob(pDC, data, scale, mirror, insert_x, insert_y, &m);
         continue;
      }

      BOOL DrawEntOnTop = FALSE;
      COLORREF *undefColor = &PenColor;
      COLORREF *toolsColor = &PenColor;
      COLORREF *undefToolsColor = &PenColor;

      // figure LAYER
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layer = insertLayer;
      else
         layer = data->getLayerIndex();

      // Highlighted Nets
      BOOL IgnoreClass = FALSE;
      BOOL HighlightedTrace = FALSE;
      BOOL HighlightedVia = FALSE;
      COLORREF highlightColor;

      if (!doc->HighlightedNetsMap.IsEmpty())
      {
         Attrib* attrib;

         if (data->getDataType() == T_POLY && (data->getGraphicClass() == graphicClassEtch || data->getGraphicClass() == graphicClassBus || data->getGraphicClass() == graphicClassSignal)
             && data->getAttributesRef() && data->getAttributesRef()->Lookup(NetNameKW, attrib))
         {
            COLORREF color;

            if (doc->HighlightedNetsMap.Lookup(attrib->getStringValueIndex(), color))
            {
               IgnoreClass = TRUE;

               if (color != -1)
               {
                  HighlightedTrace = TRUE;
                  highlightColor = color;
               }
            }
         }

         if (data->getDataType() == T_INSERT && data->getAttributesRef() && data->getAttributesRef()->Lookup(NetNameKW, attrib))
         {
            COLORREF color;

            if (doc->HighlightedNetsMap.Lookup(attrib->getStringValueIndex(), color))
            {
               IgnoreClass = TRUE;

               if (color != -1)
               {
                  HighlightedVia = TRUE;
                  highlightColor = color;
               }
            }
         }
      }

      // If the entity is not visible, then skip it, unless CAMCAD is set to always
      // show entities with an override color (this check done later)
      if (!data->hasHighlightColor() && ColorOverride != 2)
      {
         if (!IsEntityVisible(data, &subblock, doc, ((doc->GeometryEditing && recurseLevel == 0) ? -1 : mirror), &layer, IgnoreClass) &&
             !doc->getSettings().AlwaysShowOverrides)
         {
            continue;
         }
      }

      bool layerMirroredFlag = ((mirror & MIRROR_LAYERS) != 0);
      bool mirroredSimpleApertureFlag = (subblock != NULL && subblock->isSimpleAperture() && 
                                           (data->getInsert()->getLayerMirrored() != layerMirroredFlag)) ;
      // assign span layer
      int spanlayer = -1;
      if(data->HasLayerSpan())
      {
          spanlayer = doc->getLayerArray().FindFirstVisLayerIndxInStackupRange(data->GetPhysicalStartLayer(), data->GetPhysicalEndLayer(), layerMirroredFlag);
          if(spanlayer != -1) layer = spanlayer;
      }

      // PENCOLOR
      if (pDC->IsPrinting() && Print_Reverse)
      {
         // if printing reverse, negative->black, others->white
         if ((negative || data->isNegative()) && !doc->NegativeView)
            PenColor = RGB(0,0,0);
         else
            PenColor = RGB(255, 255, 255);
      }
      else if (pDC->IsPrinting() && Print_Black)
      {
         // if printing black&white, negative->white, others->black
         if ((negative || data->isNegative()) && !doc->NegativeView)
            PenColor = RGB(255, 255, 255);
         else
            PenColor = RGB(0,0,0);
      }
      else // not printing black&white
      {
         switch (highlight)
         {
         case 0: // this block is drawn normal
            if (data->isSelected())
               PenColor = doc->getSettings().SelectColor;
            else if (data->isMarked())
               PenColor = doc->getSettings().MarkedColor;
            else if (doc->NegativeView && !(negative || data->isNegative()))
               PenColor = doc->getSettings().Bckgrnd;
            else if (!doc->NegativeView && doc->getShowFills() && (negative || data->isNegative()))
            {
               if (pDC->IsPrinting())
                  PenColor = RGB(255, 255, 255);
               else
                  PenColor = doc->getSettings().Foregrnd;
            }
            else
            {
               if (data->hasHighlightColor())
                  PenColor = data->getHighlightColor();
               else if (data->getColorOverride())
                  PenColor = data->getOverrideColor();
               else if (ColorOverride)
                  PenColor = overrideColor;
               else
               {
                  if (mirroredSimpleApertureFlag)
                  {
                     PenColor = doc->get_layer_color(layer, MIRROR_LAYERS);
                  }
                  else
                  {
                     PenColor = doc->get_layer_color(layer, mirror);
                  }

                  if (HighlightedTrace)
                  {
                     PenColor = highlightColor;
                     DrawEntOnTop = TRUE;
                  }
               }
               undefColor = &doc->getSettings().UndefinedColor;
               toolsColor = &doc->getSettings().ToolColor;
               undefToolsColor = &doc->getSettings().UndefToolColor;
            }
            break;

         case 1: // selected
            PenColor = doc->getSettings().SelectColor;
            break;
         case 2: // marked
            PenColor = doc->getSettings().MarkedColor;
            break;
         case 3: // highlight
            PenColor = doc->getSettings().HighlightColor;
            break;
         case -1: // erase
            PenColor = doc->getSettings().Foregrnd;
            break;
         }

         // if negative inside a highlighted block,
         if ((negative || data->isNegative()) && (highlight == 1 || highlight == 2) )
         {
            if (doc->NegativeView)
               PenColor = doc->get_layer_color(layer, mirror);
            else if (pDC->IsPrinting())
               PenColor = RGB(255, 255, 255);
            else
               PenColor = doc->getSettings().Foregrnd;
         }

         // if white pen on white paper and not negative
         if (pDC->IsPrinting() && PenColor == RGB(255,255,255) && !(negative || data->isNegative()))
            PenColor = RGB(0,0,0);
      }

      BOOL _ColorOverride = ColorOverride;
      COLORREF _overrideColor = overrideColor;

      if (HighlightedVia)
      {
         _ColorOverride = TRUE;
         _overrideColor = highlightColor;
         DrawEntOnTop = TRUE;
      }

      if (!data->isMarked() && (DrawEntOnTop || data->getColorOverride() || data->hasHighlightColor()))
      {
         SelectStruct *s = new SelectStruct;
         s->setData(data);
         s->setParentDataList(&block->getDataList());
         s->filenum  = block->getFileNumber();
         s->insert_x = (DbUnit)insert_x;
         s->insert_y = (DbUnit)insert_y;
         s->rotation = (DbUnit)rotation;
         s->mirror   = mirror;
         s->scale    = (DbUnit)scale;
         s->layer    = layer;

         DrawOnTop.AddTail(s);
         continue;
      }

      // check if the entity doesn't have an override color and it's not visible, then we can skip it
      if (!ColorOverride && !IsEntityVisible(data, &subblock, doc, (doc->GeometryEditing && !recurseLevel ? -1 : mirror), &layer, IgnoreClass))
         continue;

      switch (data->getDataType())
      {
      case T_POINT:
         {
            if (!doc->getSettings().ShowPoints)
               continue;

            if (doc->showSeqNums || doc->showSeqArrows)
            {
               Point2 point2;
               point2.x = data->getPoint()->x * scale;
               point2.y = data->getPoint()->y * scale;

               if (mirror & MIRROR_FLIP)
                  point2.x = -point2.x;

               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               if (doc->showSeqNums)
               {
                  char buf[10];
                  sprintf(buf, "%d", seqnum++);
                  pDC->SetBkMode(OPAQUE);
                  pDC->TextOut(round(point2.x * getScaleFactor()), round(point2.y * getScaleFactor()), buf);
                  pDC->SetBkMode(TRANSPARENT);
               }

               if (doc->showSeqArrows)
               {
                  if (data->getFlags() & DATA_SEQ_HEAD)
                  {
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
                  else if (data->getFlags() & DATA_SEQ_CONT)
                  {
                     DrawArrow(pDC, seq_x, seq_y, point2.x, point2.y, getScaleFactor(), doc->getSettings().RL_arrowSize, RGB(255, 0, 0));
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
               }
            }

            Point2 p;
            p.x = data->getPoint()->x * scale;

            if (mirror) p.x = -p.x;

            p.y = data->getPoint()->y * scale;
            TransPoint2(&p, 1, &m, insert_x, insert_y);

            int x, y, size;
            size = 0; //round(GetCursorSize());
            x = round(p.x * getScaleFactor());
            y = round(p.y * getScaleFactor());

            SetupPens(doc, pDC, FALSE, 0, 1.0, FALSE, 0);

            pDC->MoveTo(x + size, y);
            pDC->LineTo(x - size, y);
            pDC->MoveTo(x, y + size);
            pDC->LineTo(x, y - size);
         }
         break;

      case T_POLY:
         {
            if (ThumbTracking && (data->getGraphicClass() == graphicClassEtch || data->getGraphicClass() == graphicClassSignal))
               continue;

            if (doc->showSeqNums || doc->showSeqArrows)
            {
               Point2 point2;
               CPoly *poly = data->getPolyList()->GetHead();

               if (poly->isHidden()) break;

               CPnt *p1, *p2;
               POSITION pos = poly->getPntList().GetHeadPosition();
               p1 = poly->getPntList().GetNext(pos);
               p2 = poly->getPntList().GetNext(pos);
               double cx, cy, r;

               if (PolyIsCircle(poly, &cx, &cy, &r)) // concentric circles would have same exact point
               {
                  point2.x = p1->x * scale;
                  point2.y = p1->y * scale;
               }
               else
               {
                  point2.x = (p1->x + p2->x) / 2 * scale;
                  point2.y = (p1->y + p2->y) / 2 * scale;
               }

               if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               if (doc->showSeqNums)
               {
                  char buf[10];
                  sprintf(buf, "%d", seqnum++);
                  pDC->SetBkMode(OPAQUE);
                  pDC->TextOut(round(point2.x * getScaleFactor()), round(point2.y * getScaleFactor()), buf);
                  pDC->SetBkMode(TRANSPARENT);
               }

               if (doc->showSeqArrows)
               {
                  if (data->getFlags() & DATA_SEQ_HEAD)
                  {
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
                  else if (data->getFlags() & DATA_SEQ_CONT)
                  {
                     DrawArrow(pDC, seq_x, seq_y, point2.x, point2.y, getScaleFactor(), doc->getSettings().RL_arrowSize, RGB(255, 0, 0));
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
               }
            }

            double lastUserDefinedWidth = userDefinedWidth;
            userDefinedWidth = 0;

            BOOL ShowFills = doc->getShowFills();

            if (globalShowFills >=0)
               ShowFills = globalShowFills;

            if (DrawPolyStruct(pDC, data->getPolyList(), scale, mirror & MIRROR_FLIP, insert_x, insert_y, &m, layer, ShowFills, TRUE))
               entityCount = BREAK_DRAW_RATE + 1; // check for ESCAPE after memDC polys because they take so long

            userDefinedWidth = lastUserDefinedWidth;
         }
         break;

      case T_TEXT:
         if (CFontList::fontIsLoaded() && !ThumbTracking)
         {
            if (doc->showSeqNums || doc->showSeqArrows)
            {
               pDC->SetBkMode(OPAQUE);
               Point2 point2;
               point2.x = data->getText()->getPnt().x * scale;
               point2.y = data->getText()->getPnt().y * scale;

               if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               if (doc->showSeqNums)
               {
                  char buf[10];
                  sprintf(buf, "%d", seqnum++);
                  pDC->TextOut(round(point2.x * getScaleFactor()), round(point2.x * getScaleFactor()), buf);
                  pDC->SetBkMode(TRANSPARENT);
               }

               if (doc->showSeqArrows)
               {
                  if (data->getFlags() & DATA_SEQ_HEAD)
                  {
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
                  else if (data->getFlags() & DATA_SEQ_CONT)
                  {
                     DrawArrow(pDC, seq_x, seq_y, point2.x, point2.y, getScaleFactor(), doc->getSettings().RL_arrowSize, RGB(255, 0, 0));
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
               }
            }

            DrawTextEntity(pDC, data, scale, mirror & MIRROR_FLIP, insert_x, insert_y, &m, rotation);
         }
         break;

      case T_INSERT:
         {
            int prevGlobalShowFills = globalShowFills;

            subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (subblock == NULL)
               continue;  // a block was deleted ???

            // calculate point
            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            point2.y = data->getInsert()->getOriginY() * scale;

            if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            if (doc->showSeqNums || doc->showSeqArrows)
            {
               if (doc->showSeqNums)
               {
                  pDC->SetBkMode(OPAQUE);
                  char buf[10];
                  sprintf(buf, "%d", seqnum++);
                  pDC->TextOut(round(point2.x * getScaleFactor()), round(point2.y * getScaleFactor()), buf);
                  pDC->SetBkMode(TRANSPARENT);
               }

               if (doc->showSeqArrows)
               {
                  if (data->getFlags() & DATA_SEQ_HEAD)
                  {
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
                  else if (data->getFlags() & DATA_SEQ_CONT)
                  {
                     DrawArrow(pDC, seq_x, seq_y, point2.x, point2.y, getScaleFactor(), doc->getSettings().RL_arrowSize, RGB(255, 0, 0));
                     seq_x = point2.x;
                     seq_y = point2.y;
                  }
               }
            }

            BOOL DrawAp       = FALSE;
            BOOL DrawSubblock = FALSE;
            BOOL IsTool       = (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL) || (subblock->getFlags() & BL_COMPLEX_TOOL);
            BOOL IsAp         = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);
            double _x     = point2.x;
            double _y     = point2.y;
            double _rot   = rotation;
            double _scale = scale;
            int _mirror   = mirror;
            int _layer    = layer;

            // TOOL
            if (IsTool)
            {
               if (subblock->getToolDisplay()) // use another block's graphics to display this tool
               {
                  subblock = doc->Find_Block_by_Num(subblock->getToolBlockNumber());

                  if (subblock == NULL)
                     continue;

                  if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                     DrawAp = TRUE;
                  else
                     DrawSubblock = TRUE;
               }
               else // draw generic tool
               {
                  if(!(subblock->getFlags() & BL_COMPLEX_TOOL))
                  {
                     double toolradius;

                     if (subblock->getToolSize() > SMALLNUMBER)
                     {
                        toolradius = subblock->getToolSize() / 2 * getScaleFactor();
                        PenColor = *toolsColor;
                     }
                     else
                     {
                        if (pDC->IsPrinting() && !Fit_to_Page) // print to scale
                        {
                           toolradius = PrintFactor * 0.25 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES); // quarter inch radius
                        }
                        else
                        {
                           int UApSize = GetDocument()->getSettings().UndefinedApSize;

                           if (Platform == WINNT)
                              UApSize = round(1.0 * UApSize * MAX_32BIT / MAX_16BIT);

                           toolradius = 1.0 * UApSize / 2 / ScaleDenom;
                        }

                        PenColor = *undefToolsColor;
                     }

                     DrawUndefined(pDC, point2.x * getScaleFactor(), point2.y * getScaleFactor(), toolradius, subblock->getTcode());
                  }
                  else
                  {
                     IsTool = false;
                  }
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

                  if (mirror & MIRROR_FLIP)
                     _rot = rotation - subblock->getRotation() - data->getInsert()->getAngle();
                  else
                     _rot = rotation + subblock->getRotation() + data->getInsert()->getAngle();

                  _mirror = mirror ^ data->getInsert()->getMirrorFlags();

                  // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
                  subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));

                  if (subblock == NULL)
                     continue;

                  DrawSubblock = TRUE;
                  globalShowFills = doc->getShowApFills();
               }               
               else
               {
                  // NORMAL APERTURE
                  if (subblock->getShape() == T_UNDEFINED)
                  {
                     if (doc->getSettings().ShowUndefined)
                     {
                        PenColor = *undefColor;

                        double radius;

                        if (pDC->IsPrinting() && !Fit_to_Page) // print to scale
                        {
                           radius = PrintFactor * 0.25 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES); // quarter inch radius
                        }
                        else
                        {
                           int UApSize = doc->getSettings().UndefinedApSize;

                           if (Platform == WINNT)
                              UApSize = round(1.0 * UApSize * MAX_32BIT / MAX_16BIT);

                           radius = 1.0 * UApSize / 2 / ScaleDenom;
                        }

                        DrawUndefined(pDC, point2.x * getScaleFactor(), point2.y * getScaleFactor(), radius,
                              (doc->getSettings().ShowUndefined ? subblock->getDcode() : 0));
                     }
                  }
                  else
                  {
                     // Simple defined aperture
                     CUseVoidsInDonuts::setUseVoidsInDonuts(false);

                     //_mirror = mirror ^ data->getInsert()->getMirrorFlags();
                     //CPolyList *polylist = ApertureToPoly(subblock, data->getInsert(),_mirror);
                     CTMatrix matrix;
                     matrix.mirrorAboutYAxis((mirror & MIRROR_FLIP) != 0);
                     CTMatrix insertMatrix = data->getInsert()->getTMatrix() * matrix;
                     CPolyList* polylist = subblock->convertApertureToPoly();

                     CUseVoidsInDonuts::setUseVoidsInDonuts(true);

                     if (polylist)
                     {
                        polylist->transform(insertMatrix);

                        if (subblock->getShape() == T_DONUT)
                           userDefinedWidth = (subblock->getSizeA() - subblock->getSizeB()) / 2; // A/2 - B/2
                        else
                           userDefinedWidth = 0;

                        if (_ColorOverride && !highlight)
                           PenColor = _overrideColor;

                        bool apertureMirrorFlag = ((mirror & MIRROR_LAYERS) != 0) != data->getInsert()->getLayerMirrored();
                        int apertureLayerIndex = doc->getMirroredLayerIndex(layer,apertureMirrorFlag);

                        DrawPolyStruct(pDC, polylist, scale, 0, insert_x, insert_y, &m, apertureLayerIndex, doc->getShowApFills(), TRUE);
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
               if (data->hasHighlightColor())
               {
                  _ColorOverride = 2;
                  _overrideColor = data->getHighlightColor();
               }
               else if (data->getColorOverride())
               {
                  _ColorOverride = TRUE;
                  _overrideColor = data->getOverrideColor();
               }

               if (NeedToDrawBlock(&doc->getSettings(), subblock, _x, _y, _scale, _rot, _mirror, pDC, getScaleFactor()))
               {
                  if (data->getInsert()->getShading().On)
                     doc->CalcBlockExtents(block);

                  if (data->getInsert()->getShading().On && data->getInsert()->getShading().Before)
                     DrawShadeRect(pDC, &data->getInsert()->getShading(), subblock, getScaleFactor(), _x, _y, _scale, _rot, _mirror);

                  if (DrawBlock(pDC, subblock,
                           _x, _y, _rot, _mirror, _scale, layer,
                           (highlight ? highlight : (data->isSelected() ? 1 : (data->isMarked() ? 2 : 0) ) ),
                           recurseLevel+1, _ColorOverride, _overrideColor, (negative || data->isNegative())) == DRAW_BREAK)
                  {
                     return DRAW_BREAK;
                  }

                  if (data->getInsert()->getShading().On && !data->getInsert()->getShading().Before)
                     DrawShadeRect(pDC, &data->getInsert()->getShading(), subblock, getScaleFactor(), _x, _y, _scale, _rot, _mirror);

                  // Pins with RefNames
                  if (doc->GeometryEditing && (doc->showPinnrsTop || doc->showPinnrsBottom) &&
                        data->getInsert()->getInsertType() == INSERTTYPE_PIN && data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
                  {
                     int pageUnits = doc->getSettings().getPageUnits();
                     double pageInchesPerPageUnits   = Units_Factor(pageUnits,UNIT_INCHES);
                     double logicalUnitsPerPageUnits = this->getScaleFactor();

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

                     LOGFONT newLogFont;
                     doc->getSettings().getPinLabelSettings().getLogFont(newLogFont);
                     newLogFont.lfHeight         = (int)(heightPixels * xLogicalUnitsPerPixel);

                     CFont newFont;
                     newFont.CreateFontIndirect(&newLogFont);

                     CFont* oldFont = pDC->SelectObject(&newFont);

                     double textRotation = doc->getSettings().getPinLabelSettings().getTextDegrees();
                     CTMatrix matrix;
                     matrix.rotateDegrees(textRotation);

                     CSize textExtents = pDC->GetOutputTextExtent(data->getInsert()->getRefname());
                     CPoint2d offset(0.,textExtents.cy/2.);
                     matrix.transform(offset);

                     // add center extent of the padstack as offset
                     BlockStruct *padstackBlock = doc->getBlockAt(data->getInsert()->getBlockNumber()); 
                     CPoint2d centerTextPt(_x,_y);
                     if(padstackBlock)
                        centerTextPt = centerTextPt + padstackBlock->getExtent().getCenter();

                     pDC->SetBkMode(OPAQUE);
                     pDC->TextOut(round(centerTextPt.x * getScaleFactor() + offset.x), round(centerTextPt.y * getScaleFactor() + offset.y), data->getInsert()->getRefname());
                     pDC->SetBkMode(TRANSPARENT);

                     pDC->SelectObject(oldFont);
                  }
               }
            } // end Draw Block

            if (doc->showTAOffsetsTop || doc->showTAOffsetsBottom)
               DrawTestAccess(pDC, data, subblock, _x, _y, _scale, _rot, _mirror);

            globalShowFills = prevGlobalShowFills;
         } // end INSERT
         break; // INSERT

      } // end switch

      if (doc->getShowAttribs() && CFontList::fontIsLoaded() && data->getAttributesRef())
      {
         if (!data->getAttributesRef()->IsEmpty())
            DrawAttribs(pDC, data, scale, mirror, insert_x, insert_y, rotation, highlight);
      }

   } // end while

   return DRAW_OK;
}


/*******************************************************************************
* DrawTestAccess
*/
void CCEtoODBView::DrawTestAccess(CDC *pDC, DataStruct *data, BlockStruct *subblock,
      double insert_x, double insert_y, double scale, double angle, int mirror)
{
   if (subblock == NULL)
      return;

   switch (data->getInsert()->getInsertType())
   {
   case INSERTTYPE_VIA:
   case INSERTTYPE_PIN:
   case INSERTTYPE_FREEPAD:
   case INSERTTYPE_TESTPAD:
      {
         CCEtoODBDoc *doc = GetDocument();
         int size = doc->getSettings().TestAccessSize;

         if (Platform == WINNT)
            size = round(1.0 * size * MAX_32BIT / MAX_16BIT);

         size = round(1.0 * size / ScaleDenom / 2);

         Attrib* attrib;
         WORD keyword;
         HPEN origPen = (HPEN)SelectObject(pDC->m_hDC, CreatePen(PS_SOLID, 0, doc->getSettings().TestAccessColor));
         DTransform xform(insert_x, insert_y, scale, angle, mirror);
         Point2 p;

         BOOL AccessTop = subblock->getFlags() & BL_ACCESS_TOP || subblock->getFlags() & BL_ACCESS_TOP_ONLY;
         BOOL AccessBottom = subblock->getFlags() & BL_ACCESS_BOTTOM || subblock->getFlags() & BL_ACCESS_BOTTOM_ONLY;

         BOOL Mirrored = (mirror & MIRROR_FLIP) ^ doc->getBottomView();

         if (doc->showTAOffsetsTop)
         {
            if ( (AccessTop ^ Mirrored) || (AccessBottom ^ !Mirrored) )
            {
               p.x = p.y = p.bulge = 0.0;
               keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
               {
                  p.x = attrib->getDoubleValue();
               }

               keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
               {
                  p.y = attrib->getDoubleValue();
               }

               xform.TransformPoint(&p);

               pDC->MoveTo(round(p.x * getScaleFactor()) - size, round(p.y * getScaleFactor()));
               pDC->LineTo(round(p.x * getScaleFactor()) + size, round(p.y * getScaleFactor()));
               pDC->MoveTo(round(p.x * getScaleFactor()), round(p.y * getScaleFactor()) - size);
               pDC->LineTo(round(p.x * getScaleFactor()), round(p.y * getScaleFactor()) + size);
            }
         }

         if (doc->showTAOffsetsBottom)
         {
            if (doc->GeometryEditing || (AccessBottom ^ Mirrored) || (AccessTop ^ !Mirrored) )
            {
               p.x = p.y = p.bulge = 0.0;
               keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
               {
                  p.x = attrib->getDoubleValue();
               }

               keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
               {
                  p.y = attrib->getDoubleValue();
               }

               xform.TransformPoint(&p);

               pDC->MoveTo(round(p.x * getScaleFactor()) - size, round(p.y * getScaleFactor()) - size);
               pDC->LineTo(round(p.x * getScaleFactor()) + size, round(p.y * getScaleFactor()) + size);
               pDC->MoveTo(round(p.x * getScaleFactor()) + size, round(p.y * getScaleFactor()) - size);
               pDC->LineTo(round(p.x * getScaleFactor()) - size, round(p.y * getScaleFactor()) + size);
            }
         }

         DeleteObject(SelectObject(pDC->m_hDC, origPen));
      }
      break;
   }
}

/*******************************************************************************
* DrawAttribs
*/
void CCEtoODBView::DrawAttribs(CDC *pDC, DataStruct *data, double scale, int parent_mirror,
                          double insert_x, double insert_y, double angle, int highlight)
{
   if (data->getDataType() != T_INSERT) // only for insert, what center would you use for line ???
      return;

   int block_mir = parent_mirror ^ data->getInsert()->getMirrorFlags();

   CCEtoODBDoc *doc = GetDocument();

   for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
   {
      Attrib* pAttrib;
      WORD keyword;
      data->getAttributesRef()->GetNextAssoc(pos,keyword,pAttrib);

      AttribIterator attribIterator(pAttrib);
      Attrib attrib(pAttrib->getCamCadData());

      while (attribIterator.getNext(attrib))
      {
         if (!attrib.isVisible() || attrib.getValueType() != VT_STRING || !doc->get_layer_visible(attrib.getLayerIndex(), block_mir))
            continue;

         int PenWidth = round(doc->getWidthTable()[attrib.getPenWidthIndex()]->getSizeA() * scale * getScaleFactor());

         if (pDC->IsPrinting() && Print_Reverse)
         {
            PenColor = RGB(255, 255, 255);
         }
         else if (pDC->IsPrinting() && Print_Black)
         {
            PenColor = RGB(0,0,0);
         }
         else // not printing black&white
         {
            switch (highlight)
            {
            case 0: // this block is drawn normal
               if (data->isSelected())
                  PenColor = doc->getSettings().SelectColor;
               else if (data->isMarked())
                  PenColor = doc->getSettings().MarkedColor;
               else if (doc->NegativeView)
                  PenColor = doc->getSettings().Bckgrnd;
               else
                  PenColor = doc->get_layer_color(attrib.getLayerIndex(), block_mir);
               break;
            case 1: // selected
               PenColor = doc->getSettings().SelectColor;
               break;
            case 2: // marked
               PenColor = doc->getSettings().MarkedColor;
               break;
            case 3: // highlight
               PenColor = doc->getSettings().HighlightColor;
               break;
            case -1: // erase
               PenColor = doc->getSettings().Foregrnd;
               break;
            }
         }

         SetupPens(doc, pDC, FALSE, PenWidth, scale, FALSE, RGB(0, 0, 0));

         Mat2x2 m;
         RotMat2(&m, angle);

         Point2 p1, p2;
         p1.x = data->getInsert()->getOriginX() * scale;

         if (parent_mirror & MIRROR_FLIP)
         {
            p1.x = -p1.x;
         }

         p1.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&p1, 1, &m, insert_x, insert_y);

         double block_scale = scale * data->getInsert()->getScale();
         double block_rot;

         if (parent_mirror & MIRROR_FLIP)
         {
            block_rot = angle - data->getInsert()->getAngle();
         }
         else
         {
            block_rot = angle + data->getInsert()->getAngle();
         }

         RotMat2(&m, block_rot);
         p2.x = attrib.getX() * block_scale;

         if (block_mir & MIRROR_FLIP)
         {
            p2.x = -p2.x;
         }

         p2.y = attrib.getY() * block_scale;
         TransPoint2(&p2, 1, &m, p1.x, p1.y);

         double text_rot;

         if (block_mir & MIRROR_FLIP)
         {
            text_rot = block_rot - attrib.getRotationRadians();
         }
         else
         {
            text_rot = block_rot + attrib.getRotationRadians();
         }

         BOOL resolved_insert_mirror = (block_mir & MIRROR_FLIP);
         BOOL attrib_mirror = attrib.getMirrorFlip();
         BOOL text_mirror = resolved_insert_mirror ^ attrib_mirror;
         FontStruct* fs = CFontList::getFontList().getFirstFont();

         DrawText(pDC, attrib.getStringValue(), fs,
               &p2, block_scale, text_rot,
               attrib.getHeight() * block_scale, attrib.getWidth() * block_scale,
               text_mirror, 0, attrib.isProportionallySpaced(), attrib.getMirrorDisabled(), attrib.getLayerIndex(),
               attrib.getHorizontalPosition(), attrib.getVerticalPosition());
      }
   }
}

/*********************************************************************
* DrawEntity
*
*  Draws 1 entity (could be an insert of a block)
*
*  SelectStruct* s   - pointer to entity with scale and rotation
*  int state         - draw, erase, highlight, etc.
*      0 -> normal
*      1 -> selected
*      2 -> marked
*      3 -> highlighted
*     -1 -> erase
*     -2 -> R2_NOT
*     -3 -> according to data State
*
*  BOOL Override     - Override showClasses[].  TRUE ignores a class being off
*/
void CCEtoODBDoc::DrawEntity(SelectStruct& selectStruct,DrawStateTag drawState)
{
   DrawEntity(&selectStruct,drawState,false);
}

void CCEtoODBDoc::DrawEntityIgnoreClassFilter(SelectStruct& selectStruct,DrawStateTag drawState)
{
   DrawEntity(&selectStruct,drawState,true);
}

void CCEtoODBDoc::DrawEntity(SelectStruct *s, int state, BOOL Override)
{
   hideDetailPen = CreatePen(PS_SOLID, 0, getSettings().HideDetailColor);
   hideDetailBrush = CreateHatchBrush(HS_DIAGCROSS, getSettings().HideDetailColor);

   CCEtoODBView *view;
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      view = (CCEtoODBView *)GetNextView(pos);

      CClientDC dc(view);
      //view->OnPrepareDC(&dc);

      dc.SetTextAlign(TA_CENTER);
      dc.SetBkColor(RGB(255, 255, 255));
      dc.SetBkMode(TRANSPARENT);

      InitPens(&dc, getSettings().FillStyle);

      RECT rect;
      dc.GetClipBox(&rect);
      double factor = (getSettings().getXmax() - getSettings().getXmin()) / (maxXCoord - minXCoord);
      clipLeft = rect.left * factor;
      clipRight = rect.right * factor;
      clipTop = rect.top * factor;
      clipBottom = rect.bottom * factor;

      DrawEntityInView(&dc, view, s, state, Override);

      FreePens(&dc);
   }

   DeleteObject(hideDetailPen);
   DeleteObject(hideDetailBrush);
}

void DrawEntityInView(CDC *dc, CCEtoODBView *view, SelectStruct *selectStruct, int state, BOOL Override)
{
   CCEtoODBDoc *doc = (CCEtoODBDoc*)view->GetDocument();

#ifdef DebuggingCase1953
   // debug
   DataStruct* selectedDataStruct = selectStruct->getData();

   if (!AfxIsValidAddress(selectedDataStruct,sizeof(DataStruct),true) ||
       !selectedDataStruct->isValid())
   {
      int iii = 3;
   }
#endif

   if (state == -3) // according to selected
   {
      if (selectStruct->getData()->isSelected())
         state = 3;
      else if (selectStruct->getData()->isMarked())
         state = 2;
      else
         state = 0;
   }

   BlockStruct *subblock;
   Mat2x2 m;
   RotMat2(&m, selectStruct->rotation);

   COLORREF *undefColor = &PenColor;
   COLORREF *toolsColor = &PenColor;
   COLORREF *undefToolsColor = &PenColor;

   BOOL _ColorOverride = selectStruct->getData()->getColorOverride();
   COLORREF _overrideColor = selectStruct->getData()->getOverrideColor();

   if (selectStruct->getData()->hasHighlightColor())
   {
      _ColorOverride = 2;
      _overrideColor = selectStruct->getData()->getHighlightColor();
   }

   int layerIndex = selectStruct->getData()->getLayerIndex();
   if(selectStruct->getData()->HasLayerSpan())
   {
      int spanlayer = doc->getLayerArray().FindFirstVisLayerIndxInStackupRange(selectStruct->getData()->GetPhysicalStartLayer(), 
         selectStruct->getData()->GetPhysicalEndLayer(), ((selectStruct->mirror & MIRROR_LAYERS) != 0));
      if(spanlayer > -1)
         layerIndex = spanlayer;
   }
   if ((selectStruct->getData()->getDataType() != T_INSERT && !doc->get_layer_visible(layerIndex, selectStruct->mirror)) ||
         (!Override &&
            (selectStruct->getData()->getDataType() != T_INSERT && !doc->getGraphicsClassFilter().contains((selectStruct->getData()->getGraphicClass()>0?selectStruct->getData()->getGraphicClass():0))) ||
            (selectStruct->getData()->getDataType() == T_INSERT && !doc->getInsertTypeFilter().contains(selectStruct->getData()->getInsert()->getInsertType())) ) )
   {
      PenColor = doc->getSettings().Foregrnd;
   }
   else
   {
      switch (state)
      {
      case 1: // select
         PenColor = doc->getSettings().SelectColor;
         break;
      case 2: // marked
         PenColor = doc->getSettings().MarkedColor;
         break;
      case 3: // highlight
         PenColor = doc->getSettings().HighlightColor;
         break;
      case 0: // normal
         if (doc->NegativeView && !selectStruct->getData()->isNegative())
            PenColor = doc->getSettings().Bckgrnd;
         else if (!doc->NegativeView && doc->getShowFills() && selectStruct->getData()->isNegative() && !state)
            PenColor = doc->getSettings().Foregrnd;
         else
         {
            PenColor = doc->get_layer_color(layerIndex, selectStruct->mirror);

            Attrib* attrib;

            if (!doc->HighlightedNetsMap.IsEmpty() && /*selectStruct->p->getGraphicClass() == graphicClassEtch && */
                  selectStruct->getData()->getAttributesRef() && selectStruct->getData()->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_NETNAME, 0), attrib))
            {
               COLORREF color;

               if (doc->HighlightedNetsMap.Lookup(attrib->getStringValueIndex(), color) && color != -1)
                  PenColor = color;
            }

            if (selectStruct->getData()->getColorOverride())
               PenColor = selectStruct->getData()->getOverrideColor();

            undefColor = &doc->getSettings().UndefinedColor;
            toolsColor = &doc->getSettings().ToolColor;
            undefToolsColor = &doc->getSettings().UndefToolColor;
         }
         break;
      case -1: // erase
         PenColor = doc->getSettings().Foregrnd;
         break;
      }
   }

   if (state == -2) dc->SetROP2(R2_NOT);

   Attrib* attrib;

   if (state == 0 && !doc->HighlightedNetsMap.IsEmpty() && selectStruct->getData()->getDataType() == T_INSERT &&
         selectStruct->getData()->getAttributesRef() && selectStruct->getData()->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_NETNAME, 0), attrib))
   {
      COLORREF color;

      if (doc->HighlightedNetsMap.Lookup(attrib->getStringValueIndex(), color) && color != -1)
      {
         PenColor = color;
         _ColorOverride = TRUE;
         _overrideColor = color;
      }
   }

   switch (selectStruct->getData()->getDataType())
   {
   case T_BLOB:
      {
         switch (state)
         {
         case 1: // select
         case 2: // marked
         case 3: // highlight
            dc->SelectStockObject(WHITE_PEN);
            break;

         case 0: // normal
         case -1: // erase
            dc->SelectStockObject(BLACK_PEN);
            break;
         }

         Point2 point2;
         point2.x = selectStruct->getData()->getBlob()->pnt.x * selectStruct->scale;
         point2.y = selectStruct->getData()->getBlob()->pnt.y * selectStruct->scale;

         if (selectStruct->mirror & MIRROR_FLIP)  point2.x = -point2.x;

         TransPoint2(&point2, 1, &m, selectStruct->insert_x, selectStruct->insert_y);

         dc->SelectStockObject(BLACK_BRUSH);

         double margin = selectStruct->getData()->getBlob()->width * view->getScaleFactor() * 0.02;

         dc->Rectangle(round(point2.x * view->getScaleFactor() - margin),
               round(point2.y * view->getScaleFactor() - margin),
               round((point2.x + selectStruct->getData()->getBlob()->width  * selectStruct->scale) * view->getScaleFactor() + margin),
               round((point2.y + selectStruct->getData()->getBlob()->height * selectStruct->scale) * view->getScaleFactor() + margin));

         if (state >= 0 && !view->MovingDatas)
         {
            view->DrawBlob(dc, selectStruct->getData(), selectStruct->scale, selectStruct->mirror, selectStruct->insert_x, selectStruct->insert_y, &m);
         }

         break;
      }

   case T_POINT:
      {
         Point2 p;
         p.x = selectStruct->getData()->getPoint()->x * selectStruct->scale;

         if (selectStruct->mirror & MIRROR_FLIP) p.x = -p.x;

         p.y = selectStruct->getData()->getPoint()->y * selectStruct->scale;
         TransPoint2(&p, 1, &m, selectStruct->insert_x, selectStruct->insert_y);

         int x, y, size;
         size = 0; //round(view->GetCursorSize());
         x = round(p.x * view->getScaleFactor());
         y = round(p.y * view->getScaleFactor());

         SetupPens(view->GetDocument(), dc, FALSE, 0, 1.0, FALSE, 0);

         dc->MoveTo(x + size, y);
         dc->LineTo(x - size, y);
         dc->MoveTo(x, y + size);
         dc->LineTo(x, y - size);
      }
      break;

   case T_POLY:
      {
         int ShowFills = doc->getShowFills();

         if (globalShowFills >=0)
            ShowFills = globalShowFills;

         int layerIndex = selectStruct->getData()->getLayerIndex();
         if(selectStruct->getData()->HasLayerSpan())
         {
            int spanlayer = doc->getLayerArray().FindFirstVisLayerIndxInStackupRange(selectStruct->getData()->GetPhysicalStartLayer(), 
                              selectStruct->getData()->GetPhysicalEndLayer(),  ((selectStruct->mirror & MIRROR_LAYERS) != 0));
            if(spanlayer > -1)
               layerIndex = spanlayer;
         }

         view->DrawPolyStruct(dc, selectStruct->getData()->getPolyList(), selectStruct->scale, selectStruct->mirror, selectStruct->insert_x, selectStruct->insert_y, &m,
                              layerIndex, (state==-2?FALSE:ShowFills), (state==-2?FALSE:TRUE));
      }
      break;

   case T_TEXT:
      view->DrawTextEntity(dc, selectStruct->getData(), selectStruct->scale, selectStruct->mirror, selectStruct->insert_x, selectStruct->insert_y, &m, selectStruct->rotation);
      break;

   case T_INSERT:
      {
         Point2 point2;
         point2.x = selectStruct->getData()->getInsert()->getOriginX() * selectStruct->scale;
         point2.y = selectStruct->getData()->getInsert()->getOriginY() * selectStruct->scale;

         if (selectStruct->mirror & MIRROR_FLIP)  point2.x = -point2.x;

         TransPoint2(&point2, 1, &m, selectStruct->insert_x, selectStruct->insert_y);

         subblock = doc->Find_Block_by_Num(selectStruct->getData()->getInsert()->getBlockNumber());

         if (subblock == NULL)
            break;

         double block_rot = subblock->getRotation();

         // TOOL
         // Process regular old fashioned round tools here, aways. 
         // Process complex tools ONLY IF it will use the drill symbol block for display, otherwise the
         // Complex tool has a data list of graphics like regular blocks, so let it fall to where that is already done.
         if ((subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL) || 
            ((subblock->getFlags() & BL_COMPLEX_TOOL) && subblock->getToolDisplay()))
         {
            if (subblock->getToolDisplay())
            {
               BlockStruct *tmpBlock = doc->Find_Block_by_Num(subblock->getToolBlockNumber());

               if (tmpBlock == NULL)
                  break;

               view->DrawBlock(dc, tmpBlock,
                     point2.x, point2.y,
                     selectStruct->rotation, selectStruct->mirror, selectStruct->scale,
                     selectStruct->getData()->getLayerIndex(), state, 1, _ColorOverride, _overrideColor, selectStruct->getData()->isNegative());
            }
            else // draw generic tool
            {
               double toolradius;

               if (subblock->getToolSize() > SMALLNUMBER)
               {
                  toolradius = subblock->getToolSize() / 2 * view->getScaleFactor();
                  PenColor = *toolsColor;
               }
               else
               {
                  int UApSize = doc->getSettings().UndefinedApSize;

                  if (Platform == WINNT)
                     UApSize = round(1.0 * UApSize * MAX_32BIT / MAX_16BIT);

                  toolradius = 1.0 * UApSize / 2 / view->ScaleDenom;
                  PenColor = *undefToolsColor;
               }

               view->DrawUndefined(dc, point2.x * view->getScaleFactor(), point2.y * view->getScaleFactor(),
                     toolradius, subblock->getTcode());
            }
         }         
         else if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE) )
         {
            // APERTURE

            if (subblock->getShape() == T_COMPLEX)
            {
               // subblock is complex aperture insert (only looking at block's values ex. sizeA & rotation)
               //_x = ;
               //_y = ;

               if (selectStruct->mirror & MIRROR_FLIP)
                  block_rot = selectStruct->rotation - subblock->getRotation() - selectStruct->getData()->getInsert()->getAngle();
               else
                  block_rot = selectStruct->rotation + subblock->getRotation() + selectStruct->getData()->getInsert()->getAngle();

               // now change subblock to aperture definition block (looking at block definition ex. polys & circles)
               subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));

               if (subblock == NULL || subblock->getDataList().IsEmpty()) break;

               int prevGlobalShowFills = globalShowFills;
               globalShowFills = doc->getShowApFills();
               view->DrawBlock(dc, subblock, point2.x, point2.y, block_rot, selectStruct->mirror ^ selectStruct->getData()->getInsert()->getMirrorFlags(), 1,
                     selectStruct->getData()->getLayerIndex(), state, 1, _ColorOverride, _overrideColor, selectStruct->getData()->isNegative());
               globalShowFills = prevGlobalShowFills;
            }
            else
            {
               if (subblock->getShape() == T_UNDEFINED)
               {
                  if (doc->getSettings().ShowUndefined)
                  {
                     PenColor = *undefColor;

                     int UApSize = doc->getSettings().UndefinedApSize;

                     if (Platform == WINNT)
                        UApSize = round(1.0 * UApSize * MAX_32BIT / MAX_16BIT);

                     double radius = 1.0 * UApSize / 2 / view->ScaleDenom;

                     view->DrawUndefined(dc, point2.x * view->getScaleFactor(), point2.y * view->getScaleFactor(), radius,
                           (doc->getSettings().ShowUndefined ? subblock->getDcode() : 0));
                  }
               }
               else
               {
                  int iii;

                  switch (state)
                  {
                  case drawStateNormal:                iii = 3;
                     break;
                  case drawStateSelected:              iii = 3;
                     break;
                  case drawStateMarked:                iii = 3;
                     break;
                  case drawStateHighlighted:           iii = 3;
                     break;
                  case drawStateErase:                 iii = 3;
                     break;
                  case drawStateR2Not:                 iii = 3;
                     break;
                  case drawStateAccordingToDataState:  iii = 3;
                     break;
                  }

                  CUseVoidsInDonuts::setUseVoidsInDonuts(false);

                  //CPolyList *polylist = ApertureToPoly(subblock, selectStruct->p->getInsert(), selectStruct->mirror);
                  CTMatrix insertMatrix = selectStruct->getData()->getInsert()->getTMatrix();
                  CPolyList* polylist = subblock->convertApertureToPoly();

                  CUseVoidsInDonuts::setUseVoidsInDonuts(true);

                  if (polylist)
                  {
                     polylist->transform(insertMatrix);

                     if (subblock->getShape() == T_DONUT)
                        userDefinedWidth = (subblock->getSizeA() - subblock->getSizeB()) / 2; // A/2 - B/2
                     else
                        userDefinedWidth = 0;

                     bool apertureMirrorFlag = ((selectStruct->mirror & MIRROR_LAYERS) != 0) != selectStruct->getData()->getInsert()->getLayerMirrored();
                     int apertureLayerIndex = doc->getMirroredLayerIndex(selectStruct->getData()->getLayerIndex(),apertureMirrorFlag);

                     view->DrawPolyStruct(dc, polylist, selectStruct->scale, selectStruct->mirror, selectStruct->insert_x, selectStruct->insert_y, &m, apertureLayerIndex, doc->getShowApFills(), TRUE);
                     FreePolyList(polylist);
                  }
               }
            }
         }         
         else
         {
            // GRAPHIC BLOCK

            if (selectStruct->mirror & MIRROR_FLIP)
               block_rot = selectStruct->rotation - selectStruct->getData()->getInsert()->getAngle();
            else
               block_rot = selectStruct->rotation + selectStruct->getData()->getInsert()->getAngle();

            if (view->MovingDatas)
            {
               doc->validateBlockExtents(subblock);

               double xmin, xmax, ymin, ymax;
               TransformInsertExtents(&xmin, &xmax, &ymin, &ymax, subblock, selectStruct->insert_x, selectStruct->insert_y, selectStruct->scale,
                     selectStruct->rotation + selectStruct->getData()->getInsert()->getAngle(), (selectStruct->mirror & MIRROR_FLIP) ^ (selectStruct->getData()->getInsert()->getMirrorFlags() & MIRROR_FLIP));

               dc->SelectStockObject(HOLLOW_BRUSH);

               if (doc->getBottomView())
               {
                  dc->Rectangle(round((-selectStruct->getData()->getInsert()->getOriginX() + xmin) * view->getScaleFactor()),
                        round((selectStruct->getData()->getInsert()->getOriginY() + ymin) * view->getScaleFactor()),
                        round((-selectStruct->getData()->getInsert()->getOriginX() + xmax) * view->getScaleFactor()),
                        round((selectStruct->getData()->getInsert()->getOriginY() + ymax) * view->getScaleFactor()));
               }
               else
               {
                  dc->Rectangle(round((selectStruct->getData()->getInsert()->getOriginX() + xmin) * view->getScaleFactor()),
                        round((selectStruct->getData()->getInsert()->getOriginY() + ymin) * view->getScaleFactor()),
                        round((selectStruct->getData()->getInsert()->getOriginX() + xmax) * view->getScaleFactor()),
                        round((selectStruct->getData()->getInsert()->getOriginY() + ymax) * view->getScaleFactor()));
               }
            }
            else
            {
               view->DrawBlock(dc, subblock, point2.x, point2.y, block_rot, selectStruct->mirror ^ selectStruct->getData()->getInsert()->getMirrorFlags(),
                     selectStruct->scale * selectStruct->getData()->getInsert()->getScale(), selectStruct->getData()->getLayerIndex(), state, 1, _ColorOverride, _overrideColor, selectStruct->getData()->isNegative());
            }
         }

         if (doc->showTAOffsetsTop || doc->showTAOffsetsBottom)
            view->DrawTestAccess(dc, selectStruct->getData(), subblock, point2.x, point2.y, selectStruct->scale * selectStruct->getData()->getInsert()->getScale(), block_rot, selectStruct->mirror ^ selectStruct->getData()->getInsert()->getMirrorFlags());

         // Pins with RefNames
         if ((doc->showPinnrsTop || doc->showPinnrsBottom) && selectStruct->getData()->getInsert()->getInsertType() == INSERTTYPE_PIN &&
             selectStruct->getData()->getInsert()->getRefname() && strlen(selectStruct->getData()->getInsert()->getRefname()))
         {
            int pageUnits = doc->getSettings().getPageUnits();
            double pageInchesPerPageUnits   = Units_Factor(pageUnits,UNIT_INCHES);
            double logicalUnitsPerPageUnits = view->getScaleFactor();

            CSize windowExtentsLogicalUnits = dc->GetWindowExt();
            CSize viewportExtentsPixels     = dc->GetViewportExt();
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

            LOGFONT newLogFont;
            doc->getSettings().getPinLabelSettings().getLogFont(newLogFont);
            newLogFont.lfHeight         = (int)(heightPixels * xLogicalUnitsPerPixel);

            CFont newFont;
            newFont.CreateFontIndirect(&newLogFont);

            CFont* oldFont = dc->SelectObject(&newFont);

            double textRotation = doc->getSettings().getPinLabelSettings().getTextDegrees();
            CTMatrix matrix;
            matrix.rotateDegrees(textRotation);

            CSize textExtents = dc->GetOutputTextExtent(selectStruct->getData()->getInsert()->getRefname());
            CPoint2d offset(0.,textExtents.cy/2.);
            matrix.transform(offset);

            // add center extent of the padstack as offset
            BlockStruct *padstackBlock = doc->getBlockAt(selectStruct->getData()->getInsert()->getBlockNumber()); 
            CPoint2d centerTextPt(point2.x,point2.y);
            if(padstackBlock)
               centerTextPt = centerTextPt + padstackBlock->getExtent().getCenter();

            // commented out for case # 1401.  No need to redraw pin labels
            dc->SetBkMode(OPAQUE);
            dc->TextOut(round(centerTextPt.x * view->getScaleFactor() + offset.x), round(centerTextPt.y * view->getScaleFactor() + offset.y), selectStruct->getData()->getInsert()->getRefname());
            dc->SetBkMode(TRANSPARENT);

            dc->SelectObject(oldFont);
         }
      }
   }

#ifdef DebuggingCase1953
   // debug
   DataStruct* selectedDataStruct2 = selectStruct->getData();

   if (!AfxIsValidAddress(selectedDataStruct2,sizeof(DataStruct),true) ||
       selectedDataStruct2 != selectedDataStruct ||
       !selectedDataStruct->isValid())
   {
      int iii = 3;
   }
#endif

   if (doc->getShowAttribs() && CFontList::fontIsLoaded() && selectStruct->getData()->getAttributesRef() && !selectStruct->getData()->getAttributesRef()->IsEmpty())
      view->DrawAttribs(dc, selectStruct->getData(), selectStruct->scale, selectStruct->mirror/* & MIRROR_FLIP*/, selectStruct->insert_x, selectStruct->insert_y,
            selectStruct->rotation, state);
}

/*********************************************************************
* DrawSubSelected
*
*  - highlights selected subentity of selected entity
*/
void CCEtoODBDoc::DrawSubSelected()
{
   if (nothingIsSelected())
      return;

   // draw direction arrow
   SelectStruct *s = getSelectStack().getAt(0);

   if (showDirection && s->getData()->getDataType() == T_POLY)
   {
      Mat2x2 m;
      RotMat2(&m, s->rotation);
      DrawDirectionArrow(this, s->getData()->getPolyList()->GetHead(), TRUE, s->insert_x, s->insert_y, s->scale, &m, s->mirror);
   }

   double x = 0.0, y = 0.0, rot = 0.0, scale = 1.0;
   int mirror = 0;

   //for (int i=0; i < getSelectStackLevelIndex(); i++)
   //{
   //   s = getSelectionAt(i);
   for (int levelIndex=0; levelIndex <= getSelectStack().getLevelIndex(); levelIndex++)
   {
      SelectStruct* selectStruct = getSelectStack().getAt(levelIndex);

      DrawEntity(selectStruct, 1, FALSE);

      Mat2x2   m;
      RotMat2(&m, rot);

      Point2 point2;
      point2.x = x * scale;
      point2.y = y * scale;

      if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

      TransPoint2(&point2, 1, &m, x, y);

      if (mirror & MIRROR_FLIP)
         rot = rot - s->rotation;
      else
         rot = rot + s->rotation;

      x = point2.x;
      y = point2.y;
      mirror = mirror ^ s->mirror;
      scale = scale * s->scale;
   }

   s = getSelectStack().getAtLevel();
   DrawEntity(s, 3, TRUE);
}


// end DRAW.CPP