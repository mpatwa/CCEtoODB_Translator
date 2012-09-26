// $Header: /CAMCAD/5.0/StandardApertureDialog.cpp 40    6/04/07 5:21p Lynn Phung $

#include "StdAfx.h"
#include "Resource.h"
#include "StandardApertureDialog.h"
#include "Draw.h"
#include ".\standardaperturedialog.h"

BOOL AngleArc_Lines95(CDC *dc, int X, int Y, double radius, double sa, double da);

//_____________________________________________________________________________
CFramelessCamCadDoc::CFramelessCamCadDoc()
{
}

CFramelessCamCadDoc::~CFramelessCamCadDoc()
{
}

//_____________________________________________________________________________
CFramelessCamCadView::CFramelessCamCadView()
{
}

CFramelessCamCadView::~CFramelessCamCadView()
{
}

//_____________________________________________________________________________
CFramelessCamCadFrame::CFramelessCamCadFrame()
{
   m_camCadDoc      = new CFramelessCamCadDoc();
   CCamCadDocGraph camCadDocGraph(m_camCadDoc);

   //m_camCadView     = new CFramelessCamCadView();
   m_camCadView     = NULL;
   m_camCadDatabase = new CCamCadDatabase(*m_camCadDoc);

   //m_camCadDoc->AddView(m_camCadView);
   m_camCadDoc->setShowFills(true);

   m_pcbFile = m_camCadDoc->Add_File("StandardApertureShapeView",Type_Unknown);
   m_pcbFile->setBlockType(blockTypePcb);
   m_pcbFile->setShow(true);
   m_pcbFile->setHidden(false);
   m_pcbFile->setNotPlacedYet(false);
   m_pcbBlock = m_pcbFile->getBlock();

   m_displayedLayerIndices.SetSize(0,20);

   defineLayer(QAxes                 ,colorDkGray);

   defineLayer(QOppositeSilkScreen   ,colorHtmlDarkTurquoise);
   defineLayer(QSilkScreen           ,colorHtmlMediumPurple);
   defineLayer(QDimOppositeCopperPad ,colorDkBlue);
   defineLayer(QDimOppositeStencilPad,colorDkOrange);

   defineLayer(QDimCopperPad         ,colorDkGreen);
   defineLayer(QBrightCopperPad      ,colorGreen);
   defineLayer(QDimToolHole          ,colorDkYellow);
   defineLayer(QBrightToolHole       ,colorYellow);

   defineLayer(QDimStencilPad        ,colorDkRed);
   defineLayer(QBrightStencilPad     ,colorRed);
   defineLayer(QTextAnnotation       ,colorGreen);
   defineLayer(QDimensionAnnotation  ,colorYellow);
   defineLayer(QPointAnnotation      ,colorLtGray);
}

CFramelessCamCadFrame::~CFramelessCamCadFrame()
{
   //if (m_camCadDoc != NULL)
   //{
   //   m_camCadDoc->RemoveView(m_camCadView);
   //}

   delete m_camCadDatabase;
   delete m_camCadView;
   delete m_camCadDoc;
}

void CFramelessCamCadFrame::defineLayer(const CString layerName,COLORREF layerDisplayColor)
{
   LayerStruct* layerStruct = m_camCadDatabase->getDefinedLayer(layerName);
   layerStruct->setColor(layerDisplayColor);

   m_displayedLayerIndices.Add(layerStruct->getLayerIndex());
}

//_____________________________________________________________________________
CCamCadDocGraph::CCamCadDocGraph(CCEtoODBDoc* camCadDoc)
{
   if (camCadDoc != NULL)
   {
      m_oldCamCadDoc = camCadDoc->StoreDocForImporting();
   }
   else
   {
      m_oldCamCadDoc = NULL;
   }
}

CCamCadDocGraph::~CCamCadDocGraph()
{
   if (m_oldCamCadDoc != NULL)
   {
      m_oldCamCadDoc->StoreDocForImporting();
   }
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CStandardApertureViewStatic, CStatic)
int CStandardApertureViewStatic::m_nextId = 0;

//CStandardApertureViewStatic::CStandardApertureViewStatic(CCamCadDatabase& camCadDatabase)
//{
//   m_camCadFrame    = NULL;
//   m_camCadDatabase = &camCadDatabase;
//}

CStandardApertureViewStatic::CStandardApertureViewStatic()
{
   m_id = m_nextId++;

   m_camCadFrame    = new CFramelessCamCadFrame();

   m_camCadDatabase = &(m_camCadFrame->getCamCadDatabase());
   m_standardApertureShape  = NULL;

   empty();
}

CStandardApertureViewStatic::~CStandardApertureViewStatic()
{
   delete m_camCadFrame;
   delete m_standardApertureShape;
}

void CStandardApertureViewStatic::empty()
{
   CStandardApertureNull nullApertureShape;

   setStandardApertureShape(nullApertureShape);
}

CExtent CStandardApertureViewStatic::getExtent()
{
   CExtent extent;
   
   if (m_standardApertureShape != NULL)
   {
      extent = m_standardApertureShape->getExtent();
   }

   return extent;
}

void CStandardApertureViewStatic::calcScaling(double scale)
{
   if (m_camCadFrame != NULL && m_standardApertureShape != NULL)
   {
      CCEtoODBDoc* doc = getCamCadDoc();

      CExtent extent = getExtent();

      if (extent.isValid())
      {
         // Case dts0100393020
         // Extent can be valid but still have zero size.
         // This results in scaling such that aperture is invisible.
         // Also can get us a divide by zero.
         // So override the extent to a non-zero size.

         if (extent.getYsize() == 0.0)
         {
            extent.setYmin( -.100 );
            extent.setYmax( +.100 );
         }
         if (extent.getXsize() == 0.0)
         {
            extent.setXmin( -.100 );
            extent.setXmax( +.100 );
         }
         
         extent.scale(scale);

         CRect clientRect;
         GetClientRect(&clientRect);

         double extentAspectRatio  = extent.getYsize() != 0.0 ? extent.getXsize() / extent.getYsize() : 1.0;
         double clientAspectRatio  = (double)clientRect.Width() / clientRect.Height();

         double newXsize,newYsize;

         if (extentAspectRatio > clientAspectRatio)
         {
            newXsize = extent.getXsize();
            newYsize = extent.getXsize() / clientAspectRatio; 
         }
         else
         {
            newXsize = extent.getYsize() * clientAspectRatio;
            newYsize = extent.getYsize(); 
         }

         extent.expand(newXsize - extent.getXsize(),newYsize - extent.getYsize());

         doc->getSettings().setXmin(extent.getXmin());
         doc->getSettings().setYmin(extent.getYmin());
         doc->getSettings().setXmax(extent.getXmax());
         doc->getSettings().setYmax(extent.getYmax());

         double ratio;

         if (doc->getSettings().getXmax() - doc->getSettings().getXmin() > doc->getSettings().getYmax() - doc->getSettings().getYmin())
         {
            ratio = 1.0 * MAX_COORD / (doc->getSettings().getXmax() - doc->getSettings().getXmin());
         }
         else
         {
            ratio = 1.0 * MAX_COORD / (doc->getSettings().getYmax() - doc->getSettings().getYmin());
         }

         doc->minXCoord = round(ratio * doc->getSettings().getXmin());
         doc->maxXCoord = round(ratio * doc->getSettings().getXmax());
         doc->minYCoord = round(ratio * doc->getSettings().getYmin());
         doc->maxYCoord = round(ratio * doc->getSettings().getYmax());

         Invalidate();
      }
   }
}

void CStandardApertureViewStatic::setStandardApertureShape(CStandardApertureShape& standardApertureShape)
{
   //m_standardApertureShape = &standardApertureShape;
   delete m_standardApertureShape;
   m_standardApertureShape = CStandardApertureShape::create(standardApertureShape);

   if (m_camCadFrame != NULL)
   {
      CCEtoODBDoc* doc = getCamCadDoc();
      CCamCadDocGraph camCadDocGraph(doc);

      CCamCadLayerMask layerMask(*m_camCadDatabase);
      layerMask.addAll();
      m_camCadDatabase->deleteEntitiesOnLayers(layerMask);

      LayerStruct* layer = m_camCadDatabase->getDefinedLayer(QBrightStencilPad);
      int layerIndex = layer->getLayerIndex();

      DataStruct* polyStruct = m_camCadDatabase->newPolyStruct(layerIndex,0,false,graphicClassNormal);
      m_standardApertureShape->makeValid();
      m_standardApertureShape->getAperturePoly(m_camCadDatabase->getCamCadData(),m_camCadFrame->getPcbBlock().getDataList(),polyStruct);

      m_camCadFrame->getPcbBlock().getDataList().AddTail(polyStruct);

      calcScaling();
   }
}

void CStandardApertureViewStatic::PrepareDC(CDC *dc) 
{
   if (m_camCadFrame != NULL)
   {
      CCEtoODBDoc* doc = getCamCadDoc();
      CCamCadDocGraph camCadDocGraph(doc);

      CRect clientRect;
      GetClientRect(&clientRect);
      CRect drawRect = clientRect;
      //drawRect.DeflateRect(DeflateBy, DeflateBy);

      m_scaleFactor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
      //m_camCadFrame->getCamCadView().scaleFactor = m_scaleFactor;

      dc->SetMapMode(MM_ISOTROPIC);

      int sizeX = doc->maxXCoord - doc->minXCoord;
      int sizeY = doc->minYCoord - doc->maxYCoord;

      dc->SetWindowOrg(0, 0);
      dc->SetWindowExt(sizeX, sizeY); // (x, -y)
      //dc->BeginPath();
      //dc->MoveTo(0,0);
      //dc->LineTo(0,sizeY);
      //dc->LineTo(sizeX,sizeY);
      //dc->CloseFigure();
      //dc->SelectClipPath(RGN_COPY);

      double ratioX = 1.0 * drawRect.Width()  / (doc->maxXCoord - doc->minXCoord);
      double ratioY = 1.0 * drawRect.Height() / (doc->maxYCoord - doc->minYCoord);
      double ratio = min(ratioX, ratioY);

   // dc->SetViewportOrg(round(ratio * -doc->minXCoord) + DeflateBy, round(ratio * doc->maxYCoord) + DeflateBy);
      int originX = round((clientRect.Width()  - ratio * (doc->maxXCoord - doc->minXCoord)) / 2 - ratio * doc->minXCoord);
      int originY = round((clientRect.Height() - ratio * (doc->maxYCoord - doc->minYCoord)) / 2 + ratio * doc->maxYCoord);
      sizeX = drawRect.Width();
      sizeY = drawRect.Height();

      dc->SetViewportOrg(originX,originY);             
      dc->SetViewportExt(sizeX,sizeY);

      //dc->BeginPath();
      //dc->MoveTo(originX        ,originY);
      //dc->LineTo(originX        ,originY + sizeY);
      //dc->LineTo(originX + sizeX,originY + sizeY);
      //dc->CloseFigure();
      //dc->SelectClipPath(RGN_COPY);

      dc->SetBkMode(TRANSPARENT);
   }
}

void CStandardApertureViewStatic::OnPaint()
{
   CPaintDC dc(this); // device context for painting
   // TODO: Add your message handler code here
   // Do not call CStatic::OnPaint() for painting messages

   //CRect clientRect;
   //GetClientRect(&clientRect);
   //dc.Ellipse(&clientRect);

   CCEtoODBDoc* doc = getCamCadDoc();
   CCamCadDocGraph camCadDocGraph(doc);

   if (m_camCadFrame != NULL)
   {
      PrepareDC(&dc);

      HBRUSH hBrush = CreateSolidBrush(doc->getSettings().Foregrnd);
      HBRUSH oldBrush = (HBRUSH) SelectObject(dc.m_hDC, hBrush);

      CRect clientRect;
      GetClientRect(&clientRect);
      dc.DPtoLP(&clientRect);
      dc.Rectangle(&clientRect);

      //dc.Rectangle(doc->minXCoord, doc->minYCoord, doc->maxXCoord, doc->maxYCoord);
      DeleteObject(SelectObject(dc.m_hDC, oldBrush));

      Draw(&dc);
   }
}

void CStandardApertureViewStatic::drawText(CDC* dc,TextStruct* textStruct)
{
   double logicalUnitsPerPageUnits = 1./m_scaleFactor;

   CSize windowExtentsLogicalUnits = dc->GetWindowExt();
   CSize viewportExtentsPixels     = dc->GetViewportExt();
   double xLogicalUnitsPerPixel    = ((double)windowExtentsLogicalUnits.cx)/viewportExtentsPixels.cx;
   double yLogicalUnitsPerPixel    = ((double)windowExtentsLogicalUnits.cy)/viewportExtentsPixels.cy;

   double heightPageUnits    = textStruct->getHeight();
   double heightLogicalUnits = heightPageUnits * logicalUnitsPerPageUnits;

   LOGFONT logFont;
   logFont.lfHeight         = 0;
   logFont.lfWidth          = 0;
   logFont.lfEscapement     = 0;
   logFont.lfOrientation    = 0;
   logFont.lfWeight         = FW_NORMAL;
   logFont.lfItalic         = 0;
   logFont.lfUnderline      = 0;
   logFont.lfStrikeOut      = 0;
   logFont.lfCharSet        = ANSI_CHARSET;
   logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
   logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
   logFont.lfQuality        = ANTIALIASED_QUALITY;
   logFont.lfPitchAndFamily = DEFAULT_PITCH || FF_SWISS;
   strncpy(logFont.lfFaceName,"Microsoft Sans Serif",LF_FACESIZE);
   logFont.lfHeight         = (int)(heightLogicalUnits);

   CFont font;
   font.CreateFontIndirect(&logFont);
   CFont* oldFont = dc->SelectObject(&font);

   CString text(textStruct->getText());
   CPoint2d textOriginPageUnits(textStruct->getOrigin());
   CPoint2d textOffsetLogicalUnits;

   CSize textExtentsLogicalUnits = dc->GetOutputTextExtent(text);
   CSize2d textExtentsPageUnits;
   textExtentsPageUnits.cx = textExtentsLogicalUnits.cx / logicalUnitsPerPageUnits;
   textExtentsPageUnits.cy = textExtentsLogicalUnits.cy / logicalUnitsPerPageUnits;

   // offset of 0.,0. corresponds to upper left
   switch (textStruct->getVerticalPosition())
   {
   case verticalPositionTop:       textOffsetLogicalUnits.y = 0.;                              break;
   case verticalPositionCenter:    textOffsetLogicalUnits.y =  textExtentsLogicalUnits.cy/2.;  break;
   case verticalPositionBottom:
   case verticalPositionBaseline:  textOffsetLogicalUnits.y =  textExtentsLogicalUnits.cy;     break;
   }

   switch (textStruct->getHorizontalPosition())
   {
   case horizontalPositionLeft:    textOffsetLogicalUnits.x = 0.;                              break;
   case horizontalPositionCenter:  textOffsetLogicalUnits.x = -textExtentsLogicalUnits.cy/2.;  break;
   case horizontalPositionRight:   textOffsetLogicalUnits.x = -textExtentsLogicalUnits.cy;     break;
   }

   CPoint2d textOriginLogicalUnits;
   textOriginLogicalUnits.x = (textOriginPageUnits.x * logicalUnitsPerPageUnits) + textOffsetLogicalUnits.x;
   textOriginLogicalUnits.y = (textOriginPageUnits.y * logicalUnitsPerPageUnits) + textOffsetLogicalUnits.y;

   dc->TextOut(round(textOriginLogicalUnits.x),round(textOriginLogicalUnits.y),text);

   dc->SelectObject(oldFont);
}

void CStandardApertureViewStatic::OnMove(int x,int y)
{
   CWnd::OnMove(x,y);
   calcScaling();
}

void CStandardApertureViewStatic::OnSize(UINT nType,int cx,int cy)
{
   CWnd::OnSize(nType,cx,cy);
   calcScaling();
}

void CStandardApertureViewStatic::Draw(CDC* dc)
{
   CCEtoODBDoc* doc = getCamCadDoc();
   CCamCadDocGraph camCadDocGraph(doc);

   static bool writeCcFileForDebugFlag = false;

   if (writeCcFileForDebugFlag)
   {
      //doc->SaveDatafile("C:\\CStandardApertureViewStatic-Debug.cc");
   }

   if (m_camCadFrame == NULL)
   {
      return;
   }

   LOGFONT logFont;
   logFont.lfHeight         = 0;
   logFont.lfWidth          = 0;
   logFont.lfEscapement     = 0;
   logFont.lfOrientation    = 0;
   logFont.lfWeight         = FW_NORMAL;
   logFont.lfItalic         = 0;
   logFont.lfUnderline      = 0;
   logFont.lfStrikeOut      = 0;
   logFont.lfCharSet        = ANSI_CHARSET;
   logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
   logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
   logFont.lfQuality        = ANTIALIASED_QUALITY;
   logFont.lfPitchAndFamily = DEFAULT_PITCH || FF_SWISS;
   strncpy(logFont.lfFaceName,"Microsoft Sans Serif",LF_FACESIZE);

   CSize windowExtents = dc->GetWindowExt();
   CPoint windowOrigin = dc->GetWindowOrg();

   for (int displayLayerIndex = 0;displayLayerIndex < m_camCadFrame->getDisplayedLayerCount();displayLayerIndex++)
   {
      int layerIndex = m_camCadFrame->getDisplayedLayerIndex(displayLayerIndex);
      LayerStruct* layer = doc->getLayerAt(layerIndex);

      bool stencilLayerFlag = ((layer->getName().CompareNoCase(QDimStencilPad)    == 0) ||
                               (layer->getName().CompareNoCase(QBrightStencilPad) == 0)    );

      for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (!file->isShown() || file->isHidden() || file->notPlacedYet())
            continue;

         if (file->getBlock() == NULL)
            continue;

         CTMatrix matrix;
         matrix.translateCtm(file->getInsertX(),file->getInsertY());
         matrix.rotateRadiansCtm(file->getRotation());
         matrix.scale(file->getScale(),file->getScale());

         double fileX = file->getInsertX();
         double fileRot = file->getRotation();
         int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);
         if (doc->getBottomView())
         {
            fileX = -fileX;
            fileRot = -fileRot;
            matrix.scale(-1.,1.);
         }

         matrix.scale(1./m_scaleFactor,1./m_scaleFactor);
         matrix.translate(-windowExtents.cx * m_scaleFactor/2.,-windowExtents.cy * m_scaleFactor/2.);

         Mat2x2 m;
         RotMat2(&m, fileRot);
         
         for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();dataPos != NULL;)
         {
            DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);
            LayerStruct* layerStruct = doc->getLayerAt(data->getLayerIndex());

            if (layerStruct->getLayerIndex() != layerIndex)
            {
               continue;
            }

            switch (data->getDataType())
            {
            case dataTypeText:
               {
                  TextStruct* textStruct = data->getText();
                  dc->SetTextColor(layerStruct->getColor());
                  drawText(dc,textStruct);
               }

               break;

            case dataTypeInsert:
               {
                  switch (data->getInsert()->getInsertType())
                  {
                  case insertTypeUnknown:
                  case insertTypePcbComponent:  
                  case insertTypePcb:
                  case insertTypeDrillTool:
                  case insertTypeSymbol:
                  case insertTypeStencilHole:
                     break;
                     
                  default:
                     continue;
                  }

                  if (data->getInsert()->getPlacedBottom() ^ (doc->getBottomView() != 0))
                     continue;

                  BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

                  Point2 point2;
                  point2.x = data->getInsert()->getOriginX() * file->getScale();
                  if (fileMirror & MIRROR_FLIP)
                     point2.x = -point2.x;
                  point2.y = data->getInsert()->getOriginY() * file->getScale();
                  TransPoint2(&point2, 1, &m, fileX, file->getInsertY());

                  if ( (subblock->getFlags() & BL_TOOL) || (subblock->getFlags() & BL_BLOCK_TOOL) ||
                        (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     dc->SetPixelV(round(point2.x / m_scaleFactor), round(point2.y / m_scaleFactor), doc->get_layer_color(data->getLayerIndex(), fileMirror));
                  }
                  else if (subblock->extentIsValid())
                  {
                     double _rot;
                     if (fileMirror & MIRROR_FLIP)
                        _rot = fileRot - data->getInsert()->getAngle();
                     else
                        _rot = fileRot + data->getInsert()->getAngle();
                     int _mirror = fileMirror ^ data->getInsert()->getMirrorFlags();
                     double _scale = file->getScale() * data->getInsert()->getScale();


                     double xmin, xmax, ymin, ymax;
                     TransformInsertExtents(&xmin, &xmax, &ymin, &ymax, subblock, point2.x, point2.y, _scale, _rot, _mirror);
      
                     HPEN orgPen, hPen;
                     HBRUSH orgBrush;
                     hPen = CreatePen(PS_SOLID, 0, doc->getSettings().HideDetailColor);
                     orgPen = (HPEN) SelectObject(dc->m_hDC, hPen);
                     orgBrush = (HBRUSH) dc->SelectStockObject(HOLLOW_BRUSH);
                     dc->Rectangle(round(xmin / m_scaleFactor), round(ymax / m_scaleFactor), round(xmax / m_scaleFactor), round(ymin / m_scaleFactor));
                     DeleteObject(SelectObject(dc->m_hDC, orgPen));
                     SelectObject(dc->m_hDC, orgBrush);
                  }
               }
               break;

            case dataTypePoint:
               {
                  if (!doc->FindLayer(data->getLayerIndex())->isWorldView())
                     continue;

                  Point2 point2;
                  point2.x = data->getPoint()->x * file->getScale();
                  if (fileMirror)
                     point2.x = -point2.x;
                  point2.y = data->getPoint()->y * file->getScale();
                  TransPoint2(&point2, 1, &m, fileX, file->getInsertY());

                  dc->SetPixelV(round(point2.x / m_scaleFactor), round(point2.y / m_scaleFactor), doc->get_layer_color(data->getLayerIndex(), fileMirror));
               }
               break;

            case dataTypePoly:
               {
                  COLORREF layerColor = doc->get_layer_color(data->getLayerIndex(), fileMirror);
                  int widthIndex = -2;

                  HPEN orgPen, hPen;
                  hPen = CreatePen(PS_SOLID, 0, layerColor);
                  orgPen = (HPEN) SelectObject(dc->m_hDC, hPen);

                  HBRUSH originalBrush,brush;

                  if (stencilLayerFlag)
                  {
                     brush = CreateHatchBrush(HS_DIAGCROSS,layerColor);
                  }
                  else
                  {
                     brush = CreateSolidBrush(layerColor);
                  }

                  originalBrush = (HBRUSH)SelectObject(dc->m_hDC,brush);

                  POSITION polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     CPoly *poly = data->getPolyList()->GetNext(polyPos);

                     if (poly->getWidthIndex() != widthIndex)
                     {
                        widthIndex = poly->getWidthIndex();
                        double width = doc->getWidth(widthIndex);
                        int pixels = round(width / m_scaleFactor);

                        HPEN hPen = CreatePen(PS_SOLID,pixels, layerColor);
                        DeleteObject(SelectObject(dc->m_hDC,hPen));
                     }

                     if (poly->isFilled())
                     {
                        dc->BeginPath();
                     }

                     double bulge, lastx, lasty;

                     BOOL First = TRUE;
                     POSITION pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        CPnt *pnt = poly->getPntList().GetNext(pntPos);

                        Point2 point2;
                        point2.x = pnt->x * file->getScale();
                        if (fileMirror & MIRROR_FLIP)
                           point2.x = -point2.x;
                        point2.y = pnt->y * file->getScale();
                        TransPoint2(&point2, 1, &m, fileX, file->getInsertY());

                        if (First)
                           dc->MoveTo(round(point2.x / m_scaleFactor), round(point2.y / m_scaleFactor));
                        else
                        {
                           if (fabs(bulge) < BULGE_THRESHOLD)
                              dc->LineTo(round(point2.x / m_scaleFactor), round(point2.y / m_scaleFactor));
                           else
                           {
                              double da, sa, r, cx, cy;
                              da = atan(bulge) * 4;
                              if (fileMirror & MIRROR_FLIP)
                                 da = -da;
                              ArcPoint2Angle(lastx, lasty, point2.x, point2.y, da, &cx, &cy, &r, &sa);

                              if (Platform == WINNT) // Windows NT
                              {
                                 dc->SetArcDirection(da<0.0 ? AD_CLOCKWISE : AD_COUNTERCLOCKWISE);
                                 dc->ArcTo(lround((cx - r) / m_scaleFactor), lround((cy + r) / m_scaleFactor), 
                                       lround((cx + r) / m_scaleFactor), lround((cy - r) / m_scaleFactor),
                                       lround(lastx / m_scaleFactor), lround(lasty / m_scaleFactor), 
                                       lround(point2.x / m_scaleFactor), lround(point2.y / m_scaleFactor));  
                                 dc->SetArcDirection(AD_COUNTERCLOCKWISE);

                                 dc->LineTo(lround(point2.x / m_scaleFactor),lround(point2.y / m_scaleFactor));
                              }
                              else // Windows 95
                              {
                                 if (cx / m_scaleFactor > doc->maxXCoord || cx / m_scaleFactor < doc->minXCoord || 
                                       cy / m_scaleFactor > doc->maxYCoord || cy / m_scaleFactor < doc->minYCoord)
                                    dc->LineTo((int)(point2.x / m_scaleFactor),(int)(point2.y / m_scaleFactor));
                                 else if (poly->isFilled())// && showFills)
                                    AngleArc_Lines95(dc, (int)(cx / m_scaleFactor), (int)(cy / m_scaleFactor), 
                                          r / m_scaleFactor, sa, da);
                                 else
                                 {
                                    if (da < 0.0) // clockwise
                                    {     
                                       dc->Arc((int)((cx - r) / m_scaleFactor), (int)((cy + r) / m_scaleFactor), 
                                             (int)((cx + r) / m_scaleFactor), (int)((cy - r) / m_scaleFactor),
                                             (int)(point2.x / m_scaleFactor), (int)(point2.y / m_scaleFactor),
                                             (int)(lastx / m_scaleFactor), (int)(lasty / m_scaleFactor));
                                    }
                                    else // counter-clockwise
                                    {
                                       dc->Arc((int)((cx - r) / m_scaleFactor), (int)((cy + r) / m_scaleFactor), 
                                             (int)((cx + r) / m_scaleFactor), (int)((cy - r) / m_scaleFactor),
                                             (int)(lastx / m_scaleFactor), (int)(lasty / m_scaleFactor), 
                                             (int)(point2.x / m_scaleFactor), (int)(point2.y / m_scaleFactor));  
                                    }
                                    dc->MoveTo((int)(point2.x / m_scaleFactor),(int)(point2.y / m_scaleFactor));
                                 }
                              }

                           }
                        }
                        First = FALSE;

                        bulge = pnt->bulge;
                        lastx = point2.x;
                        lasty = point2.y;
                     }

                     if (poly->isFilled()) 
                     {
                        dc->EndPath();
                        dc->StrokeAndFillPath();
                     }
                  }

                  DeleteObject(SelectObject(dc->m_hDC, orgPen));
                  DeleteObject(SelectObject(dc->m_hDC, originalBrush));
               }
               break;
            }
         }
      }
   }
}

BEGIN_MESSAGE_MAP(CStandardApertureViewStatic, CStatic)
   ON_WM_DRAWITEM_REFLECT()
   ON_WM_PAINT()
   ON_WM_MOVE()
   ON_WM_SIZE()
END_MESSAGE_MAP()

// CStandardApertureViewStatic message handlers

void CStandardApertureViewStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

   // TODO:  Add your code to draw the specified item
   //CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);
   //dc->Ellipse(&(lpDrawItemStruct->rcItem));
}

//_____________________________________________________________________________
//IMPLEMENT_DYNAMIC(CStandardApertureDialog, CResizingDialog)

CStandardApertureDialog::CStandardApertureDialog(PageUnitsTag pageUnits)
	: CResizingDialog(CStandardApertureDialog::IDD, NULL)
   , m_pageUnits(pageUnits)
     
{
   m_standardApertureShape        = NULL;
   m_genericStandardApertureShape = NULL;
   m_initialApertureType          = standardApertureCircle;
   //m_camCadDatabase        = NULL;

   m_parameters.addParameter(m_parameterCaption0,m_valueEditBox0,m_parameterDescriptionStatic0,m_genericParameterCaption0);
   m_parameters.addParameter(m_parameterCaption1,m_valueEditBox1,m_parameterDescriptionStatic1,m_genericParameterCaption1);
   m_parameters.addParameter(m_parameterCaption2,m_valueEditBox2,m_parameterDescriptionStatic2,m_genericParameterCaption2);
   m_parameters.addParameter(m_parameterCaption3,m_valueEditBox3,m_parameterDescriptionStatic3,m_genericParameterCaption3);
   m_parameters.addParameter(m_parameterCaption4,m_valueEditBox4,m_parameterDescriptionStatic4,m_genericParameterCaption4);
   m_parameters.addParameter(m_parameterCaption5,m_valueEditBox5,m_parameterDescriptionStatic5,m_genericParameterCaption5);
   m_parameters.addCenters(m_originGroupBox,m_originOptionsComboBox,m_displayCenterAnnotationsCheckBox);

   // Resizing
   CResizingDialogField& apertureTypeField = addFieldControl(IDC_ApertureType         ,anchorRight);

   CResizingDialogField& descriptorField = addFieldControl(IDC_Descriptor,anchorProportionalVertical);
   descriptorField.getOperations().addOperation(glueRightEdge,toLeftEdge ,&apertureTypeField);

   CResizingDialogField& s0 = addFieldControl(IDC_GenericApertureDiagram  ,anchorLeft);
   s0.getOperations().addOperation(glueBottomEdge,toTopEdge ,&descriptorField);
   s0.getOperations().addOperation(glueRightEdge,toRightEdge,&descriptorField);
   s0.getOperations().addOperation(glueLeftEdge ,toLeftEdge ,&descriptorField);

   CResizingDialogField& s1 = addFieldControl(IDC_ApertureDiagram         ,anchorBottomLeft);
   s1.getOperations().addOperation(glueTopEdge,toBottomEdge ,&descriptorField);
   s1.getOperations().addOperation(glueRightEdge,toRightEdge,&descriptorField);
   s1.getOperations().addOperation(glueLeftEdge ,toLeftEdge ,&descriptorField);

   addFieldControl(IDC_GenericParameterCaption0,anchorRight);
   addFieldControl(IDC_GenericParameterCaption1,anchorRight);
   addFieldControl(IDC_GenericParameterCaption2,anchorRight);
   addFieldControl(IDC_GenericParameterCaption3,anchorRight);
   addFieldControl(IDC_GenericParameterCaption4,anchorRight);
   addFieldControl(IDC_GenericParameterCaption5,anchorRight);

   addFieldControl(IDC_GenericParameterDescription0,anchorRight);
   addFieldControl(IDC_GenericParameterDescription1,anchorRight);
   addFieldControl(IDC_GenericParameterDescription2,anchorRight);
   addFieldControl(IDC_GenericParameterDescription3,anchorRight);
   addFieldControl(IDC_GenericParameterDescription4,anchorRight);
   addFieldControl(IDC_GenericParameterDescription5,anchorRight);

   CResizingDialogField& c0 = addFieldControl(IDC_ParameterCaption0,anchorRight);
   c0.getOperations().addOperation(glueTopEdge   ,toTopEdge,&descriptorField);
   c0.getOperations().addOperation(glueBottomEdge,toTopEdge,&descriptorField);
   addFieldControl(IDC_ParameterCaption1,anchorRelativeToField,&c0);
   addFieldControl(IDC_ParameterCaption2,anchorRelativeToField,&c0);
   addFieldControl(IDC_ParameterCaption3,anchorRelativeToField,&c0);
   addFieldControl(IDC_ParameterCaption4,anchorRelativeToField,&c0);
   addFieldControl(IDC_ParameterCaption5,anchorRelativeToField,&c0);

   CResizingDialogField& pv0 = addFieldControl(IDC_ParameterValue0,anchorRight);
   pv0.getOperations().addOperation(glueTopEdge   ,toTopEdge,&descriptorField);
   pv0.getOperations().addOperation(glueBottomEdge,toTopEdge,&descriptorField);
   addFieldControl(IDC_ParameterValue1,anchorRelativeToField,&pv0);
   addFieldControl(IDC_ParameterValue2,anchorRelativeToField,&pv0);
   addFieldControl(IDC_ParameterValue3,anchorRelativeToField,&pv0);
   addFieldControl(IDC_ParameterValue4,anchorRelativeToField,&pv0);
   addFieldControl(IDC_ParameterValue5,anchorRelativeToField,&pv0);

   CResizingDialogField& g0 = addFieldControl(IDC_ApertureUnitsGroup ,anchorRight);
   g0.getOperations().addOperation(glueTopEdge   ,toTopEdge,&descriptorField);
   g0.getOperations().addOperation(glueBottomEdge,toTopEdge,&descriptorField);
   addFieldControl(IDC_ApertureUnitsStatic,anchorRelativeToField,&g0);

   CResizingDialogField& g1 = addFieldControl(IDC_OriginGroup ,anchorRight);
   g1.getOperations().addOperation(glueTopEdge   ,toTopEdge,&descriptorField);
   g1.getOperations().addOperation(glueBottomEdge,toTopEdge,&descriptorField);
   addFieldControl(IDC_OriginOptions,anchorRelativeToField,&g1);
   addFieldControl(IDC_DisplayCenterAnnotations,anchorRelativeToField,&g1);

   addFieldControl(IDC_GenericDescriptor,anchorLeft,growHorizontal);

   addFieldControl(IDOK     ,anchorBottom);
   addFieldControl(IDCANCEL ,anchorBottom);
}

CStandardApertureDialog::~CStandardApertureDialog()
{
   delete m_standardApertureShape;
   delete m_genericStandardApertureShape;
}

void CStandardApertureDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_GenericParameterCaption0, m_genericParameterCaption0);
   DDX_Control(pDX, IDC_GenericParameterCaption1, m_genericParameterCaption1);
   DDX_Control(pDX, IDC_GenericParameterCaption2, m_genericParameterCaption2);
   DDX_Control(pDX, IDC_GenericParameterCaption3, m_genericParameterCaption3);
   DDX_Control(pDX, IDC_GenericParameterCaption4, m_genericParameterCaption4);
   DDX_Control(pDX, IDC_GenericParameterCaption5, m_genericParameterCaption5);
   DDX_Control(pDX, IDC_ParameterCaption0, m_parameterCaption0);
   DDX_Control(pDX, IDC_ParameterCaption1, m_parameterCaption1);
   DDX_Control(pDX, IDC_ParameterCaption2, m_parameterCaption2);
   DDX_Control(pDX, IDC_ParameterCaption3, m_parameterCaption3);
   DDX_Control(pDX, IDC_ParameterCaption4, m_parameterCaption4);
   DDX_Control(pDX, IDC_ParameterCaption5, m_parameterCaption5);
   DDX_Control(pDX, IDC_ParameterValue0, m_valueEditBox0);
   DDX_Control(pDX, IDC_ParameterValue1, m_valueEditBox1);
   DDX_Control(pDX, IDC_ParameterValue2, m_valueEditBox2);
   DDX_Control(pDX, IDC_ParameterValue3, m_valueEditBox3);
   DDX_Control(pDX, IDC_ParameterValue4, m_valueEditBox4);
   DDX_Control(pDX, IDC_ParameterValue5, m_valueEditBox5);
   DDX_Control(pDX, IDC_Descriptor, m_descriptorEditBox);
   DDX_Control(pDX, IDC_GenericDescriptor, m_genericDescriptorStatic);
   DDX_Control(pDX, IDC_ApertureType, m_apertureTypeComboBox);
   DDX_Control(pDX, IDC_GenericParameterDescription0, m_parameterDescriptionStatic0);
   DDX_Control(pDX, IDC_GenericParameterDescription1, m_parameterDescriptionStatic1);
   DDX_Control(pDX, IDC_GenericParameterDescription2, m_parameterDescriptionStatic2);
   DDX_Control(pDX, IDC_GenericParameterDescription3, m_parameterDescriptionStatic3);
   DDX_Control(pDX, IDC_GenericParameterDescription4, m_parameterDescriptionStatic4);
   DDX_Control(pDX, IDC_GenericParameterDescription5, m_parameterDescriptionStatic5);
   DDX_Control(pDX, IDC_GenericApertureDiagram, m_genericApertureViewStatic);
   DDX_Control(pDX, IDC_ApertureDiagram, m_apertureViewStatic);
   DDX_Control(pDX, IDC_ApertureUnitsStatic, m_apertureUnitsStatic);
   DDX_Control(pDX, IDC_OriginOptions, m_originOptionsComboBox);
   DDX_Control(pDX, IDC_DisplayCenterAnnotations, m_displayCenterAnnotationsCheckBox);
   DDX_Control(pDX, IDC_OriginGroup, m_originGroupBox);
}

void CStandardApertureDialog::setStandardApertureShape(CStandardApertureShape* apertureShape)
{
   delete m_standardApertureShape;

   m_standardApertureShape = apertureShape;
   m_standardApertureShape->makeValid();
   m_standardApertureShape->storeToApertureParameters(m_parameters);

   m_apertureViewStatic.setStandardApertureShape(*m_standardApertureShape);
   m_standardApertureShape->addPointAnnotation(m_apertureViewStatic.getCamCadDatabase()->getCamCadData(),0.,0.);

   if (getDisplayCenterAnnotations())
   {
      m_standardApertureShape->addCentroidAnnotations(m_apertureViewStatic.getCamCadDatabase()->getCamCadData());
   }

   m_descriptorEditBox.SetWindowText(m_standardApertureShape->getDescriptor());
   m_apertureUnitsStatic.SetWindowText(m_standardApertureShape->getDescriptorDimensionUnitsString());

   CString descriptorPrefix = standardApertureTypeTagToApertureNameDescriptor(m_standardApertureShape->getType());
   m_apertureTypeComboBox.SelectString(0,descriptorPrefix);
}

void CStandardApertureDialog::setGenericStandardApertureShape(CStandardApertureShape* apertureShape)
{
   delete m_genericStandardApertureShape;

   m_genericStandardApertureShape = apertureShape;

   m_genericApertureViewStatic.setStandardApertureShape(*m_genericStandardApertureShape);
   m_genericStandardApertureShape->addDiagramAnnotations(m_genericApertureViewStatic.getCamCadDatabase()->getCamCadData());

   if (getDisplayCenterAnnotations())
   {
      m_genericStandardApertureShape->addCentroidAnnotations(m_genericApertureViewStatic.getCamCadDatabase()->getCamCadData());
   }

   m_genericDescriptorStatic.SetWindowText(m_parameters.getDescriptorTemplate());
}

void CStandardApertureDialog::setApertureType(StandardApertureTypeTag apertureType)
{
   m_parameters.setDescriptorPrefix(standardApertureTypeTagToDescriptorPrefix(apertureType));

   PageUnitsTag pageUnits = getPageUnits();
   int exponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(pageUnits);

   setStandardApertureShape(CStandardApertureShape::create(apertureType,pageUnits,exponent));
   setGenericStandardApertureShape(CStandardApertureShape::create(apertureType,pageUnits,exponent));
}

bool CStandardApertureDialog::setApertureFromDescriptor(const CString& descriptor)
{
   CStandardApertureShape* standardApertureShape = 
      CStandardApertureShape::create(descriptor,CStandardApertureShape::getDefaultPageUnits());

   standardApertureShape->makeValid();

   setApertureType(standardApertureShape->getType());
   setStandardApertureShape(standardApertureShape);

   return standardApertureShape->isValid();
}

const CStandardApertureShape& CStandardApertureDialog::getStandardApertureShape()
{
   if (m_standardApertureShape == NULL)
   {
      setApertureType(standardApertureCircle);
   }

   return *m_standardApertureShape;
}

CString CStandardApertureDialog::getApertureDescriptor() const
{
   CString descriptor;

   if (m_standardApertureShape != NULL && m_standardApertureShape->isValid())
   {
      descriptor = m_standardApertureShape->getDescriptor();
   }

   return descriptor;
}

void CStandardApertureDialog::drawApertureView()
{
   //if (m_camCadDatabase != NULL)
   //{
   //   //m_camCadDatabase->getCamCadDoc().dra
   //}
}

void CStandardApertureDialog::updateParameters()
{
   UpdateData(true);

   m_standardApertureShape->loadFromApertureParameters(m_parameters);
   m_standardApertureShape->storeToApertureParameters(m_parameters);

   m_descriptorEditBox.SetWindowText(m_standardApertureShape->getDescriptor());
   m_apertureViewStatic.setStandardApertureShape(*m_standardApertureShape);
   m_standardApertureShape->addPointAnnotation(m_apertureViewStatic.getCamCadDatabase()->getCamCadData(),0.,0.);

   if (getDisplayCenterAnnotations())
   {
      m_standardApertureShape->addCentroidAnnotations(m_apertureViewStatic.getCamCadDatabase()->getCamCadData());
   }

   m_apertureViewStatic.Invalidate();

   UpdateData(false);
}

BEGIN_MESSAGE_MAP(CStandardApertureDialog, CResizingDialog)
   ON_CBN_EDITCHANGE(IDC_ApertureType, OnEditChangeApertureTypeComboBox)
   ON_CBN_SELCHANGE(IDC_ApertureType, OnSelChangeApertureTypeComboBox)
   ON_EN_KILLFOCUS(IDC_ParameterValue0, OnEnKillFocusParameterValue0)
   ON_EN_KILLFOCUS(IDC_ParameterValue1, OnEnKillFocusParameterValue1)
   ON_EN_KILLFOCUS(IDC_ParameterValue2, OnEnKillFocusParameterValue2)
   ON_EN_KILLFOCUS(IDC_ParameterValue3, OnEnKillFocusParameterValue3)
   ON_EN_KILLFOCUS(IDC_ParameterValue4, OnEnKillFocusParameterValue4)
   ON_EN_KILLFOCUS(IDC_ParameterValue5, OnEnKillFocusParameterValue5)
   ON_EN_KILLFOCUS(IDC_Descriptor, OnEnKillFocusDescriptor)
   ON_BN_CLICKED(IDC_DisplayCenterAnnotations, OnBnClickedDisplayCenterAnnotations)
   ON_CBN_SELCHANGE(IDC_OriginOptions, OnCbnSelchangeOriginOptions)
END_MESSAGE_MAP()


// CStandardApertureDialog message handlers

BOOL CStandardApertureDialog::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   CString descriptorPrefix;
   int selectedIndex = 0;

   for (int index = standardApertureFirstValid;index < standardApertureInvalid;index++)
   {
      StandardApertureTypeTag apertureType = intToStandardApertureTypeTag(index);
      descriptorPrefix = standardApertureTypeTagToApertureNameDescriptor(apertureType);

      bool includeFlag = true;

      switch (apertureType)
      {
      case standardApertureHole:
      case standardApertureMoire:
      case standardApertureNull:
      case standardApertureRectangularThermal:
      case standardApertureRectangularThermalOpenCorners:
      case standardApertureRoundThermalRounded:
      case standardApertureRoundThermalSquare:
      case standardApertureSquareRoundThermal:
      case standardApertureSquareThermal:
      case standardApertureSquareThermalOpenCorners:
         includeFlag = false;
         break;
      }

      if (includeFlag)
      {
         int itemIndex = m_apertureTypeComboBox.AddString(descriptorPrefix);
         m_apertureTypeComboBox.SetItemData(itemIndex,index);

         if (apertureType == m_initialApertureType)
         {
            selectedIndex = index;
         }
      }
   }

   ::SetWindowLong(m_genericApertureViewStatic.m_hWnd,GWL_STYLE,
      m_genericApertureViewStatic.GetStyle() | SS_OWNERDRAW);

   ::SetWindowLong(m_apertureViewStatic.m_hWnd,GWL_STYLE,
      m_apertureViewStatic.GetStyle() | SS_OWNERDRAW);

   if (!setApertureFromDescriptor(m_initialDescriptor))
   {
      m_apertureTypeComboBox.SetCurSel(selectedIndex);
      setApertureType(m_initialApertureType);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CStandardApertureDialog::OnEditChangeApertureTypeComboBox()
{
   // TODO: Add your control notification handler code here
}

void CStandardApertureDialog::OnSelChangeApertureTypeComboBox()
{
   int selectedItem = m_apertureTypeComboBox.GetCurSel();

   if (selectedItem != CB_ERR)
   {
      DWORD_PTR itemData = m_apertureTypeComboBox.GetItemData(selectedItem);
      setApertureType(intToStandardApertureTypeTag(itemData));

      drawApertureView();
   }
}

void CStandardApertureDialog::OnEnKillFocusParameterValue0()
{
   updateParameters();
}

void CStandardApertureDialog::OnEnKillFocusParameterValue1()
{
   updateParameters();
}

void CStandardApertureDialog::OnEnKillFocusParameterValue2()
{
   updateParameters();
}

void CStandardApertureDialog::OnEnKillFocusParameterValue3()
{
   updateParameters();
}

void CStandardApertureDialog::OnEnKillFocusParameterValue4()
{
   updateParameters();
}

void CStandardApertureDialog::OnEnKillFocusParameterValue5()
{
   updateParameters();
}

void CStandardApertureDialog::OnEnKillFocusDescriptor()
{
   CString descriptor;
   m_descriptorEditBox.GetWindowText(descriptor);

   setApertureFromDescriptor(descriptor);
}

void CStandardApertureDialog::OnBnClickedDisplayCenterAnnotations()
{
   CStandardApertureShape* genericStandardApertureShape = m_genericStandardApertureShape;
   m_genericStandardApertureShape = NULL;
   setGenericStandardApertureShape(genericStandardApertureShape);

   CStandardApertureShape* standardApertureShape = m_standardApertureShape;
   m_standardApertureShape = NULL;
   setStandardApertureShape(standardApertureShape);
}

void CStandardApertureDialog::OnCbnSelchangeOriginOptions()
{
   updateParameters();
}
