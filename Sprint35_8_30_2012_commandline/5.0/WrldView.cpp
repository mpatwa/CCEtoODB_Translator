// $Header: /CAMCAD/4.6/WrldView.cpp 23    12/07/06 12:12p Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include <math.h>
#include "CCEtoODB.h"
#include "WrldView.h"
#include "ccview.h"
#include "draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView *activeView; // from CCVIEW.CPP
extern CString worldViewFile; // from MAINFRM.CPP
extern BOOL HideDrawings; // from API.CPP

BOOL WorldViewOn;
WorldView *worldView = NULL;

BOOL AngleArc_Lines95(CDC *dc, int X, int Y, double radius, double sa, double da);

void CCEtoODBDoc::OnWorldView() 
{
   WorldViewOn = !WorldViewOn;
   worldView->ShowWindow(WorldViewOn ? SW_SHOW : SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
// WorldView dialog
WorldView::WorldView(CWnd* pParent /*=NULL*/)
   : CDialog(WorldView::IDD, pParent)
{
   //{{AFX_DATA_INIT(WorldView)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   Anchored = FALSE;
}

void WorldView::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(WorldView)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(WorldView, CDialog)
   //{{AFX_MSG_MAP(WorldView)
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
   ON_WM_SIZE()
   ON_WM_ERASEBKGND()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WorldView message handlers
void WorldView::PostNcDestroy() 
{
   delete this;
   worldView = NULL; 
   CDialog::PostNcDestroy();
}

void WorldView::OnCancel() 
{
   WorldViewOn = FALSE;
   worldView->ShowWindow(SW_HIDE);
   CMenu *menu = AfxGetMainWnd()->GetMenu();
   if (menu)
      menu->CheckMenuItem(ID_WORLDVIEW, MF_UNCHECKED);
}

const int DeflateBy = 7;
void WorldView::PrepareDC(CDC *dc, CCEtoODBDoc *doc) 
{
   CRect clientRect;
   GetClientRect(&clientRect);
   CRect drawRect = clientRect;
   drawRect.DeflateRect(DeflateBy, DeflateBy);

   scaleFactor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);

   dc->SetMapMode(MM_ISOTROPIC);

   dc->SetWindowOrg(0, 0);
   dc->SetWindowExt(doc->maxXCoord - doc->minXCoord, doc->minYCoord - doc->maxYCoord); // (x, -y)

   double ratioX = 1.0 * drawRect.Width() / (doc->maxXCoord - doc->minXCoord);
   double ratioY = 1.0 * drawRect.Height() / (doc->maxYCoord - doc->minYCoord);
   double ratio = min(ratioX, ratioY);

// dc->SetViewportOrg(round(ratio * -doc->minXCoord) + DeflateBy, round(ratio * doc->maxYCoord) + DeflateBy);
   dc->SetViewportOrg( round((clientRect.Width() - ratio * (doc->maxXCoord - doc->minXCoord)) / 2 - ratio * doc->minXCoord), 
         round((clientRect.Height() - ratio * (doc->maxYCoord - doc->minYCoord)) / 2 + ratio * doc->maxYCoord));
   dc->SetViewportExt(drawRect.Width(), drawRect.Height());
}

void WorldView::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   if (!activeView)
      return;

   doc = (CCEtoODBDoc*)activeView->GetDocument();

   PrepareDC(&dc, doc);

   HBRUSH hBrush = CreateSolidBrush(doc->getSettings().Foregrnd);
   HBRUSH oldBrush = (HBRUSH) SelectObject(dc.m_hDC, hBrush);
   dc.Rectangle(doc->minXCoord, doc->minYCoord, doc->maxXCoord, doc->maxYCoord);
   DeleteObject(SelectObject(dc.m_hDC, oldBrush));

   if (!HideDrawings)
      Draw(&dc);

   if (doc->getSettings().Crosshairs)
   {
      // draw crosshairs
      HPEN hPen = CreatePen(PS_SOLID, 0, doc->getSettings().CrossColor);
      HPEN oldPen = (HPEN) SelectObject(dc.m_hDC, hPen);
      dc.MoveTo(doc->minXCoord, 0);
      dc.LineTo(doc->maxXCoord, 0);
      dc.MoveTo(0, doc->minYCoord);
      dc.LineTo(0, doc->maxYCoord);
      DeleteObject(SelectObject(dc.m_hDC, oldPen));
   }

   // draw zoom box
   if (((CCEtoODBView*)activeView)->ScaleDenom > 1.1)
   {
      dc.SetROP2(R2_MERGEPEN);
      dc.SelectStockObject(WHITE_PEN);
      dc.SelectStockObject(GRAY_BRUSH);
      dc.Rectangle(((CCEtoODBView*)activeView)->visibleRect);
   }
}

void WorldView::Draw(CDC *dc)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet())
         continue;

      if (file->getBlock() == NULL)
         continue;

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

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         switch (data->getDataType())
         {
         case T_TEXT:
            break;

         case T_INSERT:
            {
               switch (data->getInsert()->getInsertType())
               {
               case INSERTTYPE_UNKNOWN:
               case INSERTTYPE_PCBCOMPONENT:  
               case INSERTTYPE_PCB:
               case INSERTTYPE_TOOLING:
               case INSERTTYPE_SYMBOL:
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
                  dc->SetPixelV(round(point2.x / scaleFactor), round(point2.y / scaleFactor), doc->get_layer_color(data->getLayerIndex(), fileMirror));
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
                  dc->Rectangle(round(xmin / scaleFactor), round(ymax / scaleFactor), round(xmax / scaleFactor), round(ymin / scaleFactor));
                  DeleteObject(SelectObject(dc->m_hDC, orgPen));
                  SelectObject(dc->m_hDC, orgBrush);
               }
            }
            break;

         case T_POINT:
            {
               if (!doc->FindLayer(data->getLayerIndex())->isWorldView())
                  continue;

               Point2 point2;
               point2.x = data->getPoint()->x * file->getScale();
               if (fileMirror)
                  point2.x = -point2.x;
               point2.y = data->getPoint()->y * file->getScale();
               TransPoint2(&point2, 1, &m, fileX, file->getInsertY());

               dc->SetPixelV(round(point2.x / scaleFactor), round(point2.y / scaleFactor), doc->get_layer_color(data->getLayerIndex(), fileMirror));
            }
            break;

         case T_POLY:
            {
               switch (data->getGraphicClass())
               {
               case GR_CLASS_BOARDOUTLINE: 
               case GR_CLASS_PANELOUTLINE: 
               case graphicClassNormal:
                  break;

               default:
                  continue;
               }

               LayerStruct *layer = doc->FindLayer(data->getLayerIndex());
               if (!layer->isWorldView())
                  continue;

               switch (layer->getLayerType())
               {
               case LAYTYPE_SIGNAL_TOP:
               case LAYTYPE_SIGNAL_BOT:
               case LAYTYPE_SIGNAL_INNER:
               case LAYTYPE_SIGNAL_ALL:
               case LAYTYPE_SIGNAL_OUTER:
               case LAYTYPE_PAD_TOP:
               case LAYTYPE_PAD_BOTTOM:
               case LAYTYPE_PAD_INNER:
               case LAYTYPE_PAD_ALL:
               case LAYTYPE_PAD_THERMAL:
                  continue;
               default:
                  break;
               }

               HPEN orgPen, hPen;
               hPen = CreatePen(PS_SOLID, 0, doc->get_layer_color(data->getLayerIndex(), fileMirror));
               orgPen = (HPEN) SelectObject(dc->m_hDC, hPen);

               POSITION polyPos = data->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  CPoly *poly = data->getPolyList()->GetNext(polyPos);

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
                        dc->MoveTo(round(point2.x / scaleFactor), round(point2.y / scaleFactor));
                     else
                     {
                        if (fabs(bulge) < BULGE_THRESHOLD)
                           dc->LineTo(round(point2.x / scaleFactor), round(point2.y / scaleFactor));
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
                              dc->ArcTo(lround((cx - r) / scaleFactor), lround((cy + r) / scaleFactor), 
                                    lround((cx + r) / scaleFactor), lround((cy - r) / scaleFactor),
                                    lround(lastx / scaleFactor), lround(lasty / scaleFactor), 
                                    lround(point2.x / scaleFactor), lround(point2.y / scaleFactor));  
                              dc->SetArcDirection(AD_COUNTERCLOCKWISE);
                           }
                           else // Windows 95
                           {
                              if (cx / scaleFactor > doc->maxXCoord || cx / scaleFactor < doc->minXCoord || 
                                    cy / scaleFactor > doc->maxYCoord || cy / scaleFactor < doc->minYCoord)
                                 dc->LineTo((int)(point2.x / scaleFactor),(int)(point2.y / scaleFactor));
                              else if (poly->isFilled())// && showFills)
                                 AngleArc_Lines95(dc, (int)(cx / scaleFactor), (int)(cy / scaleFactor), 
                                       r / scaleFactor, sa, da);
                              else
                              {
                                 if (da < 0.0) // clockwise
                                 {     
                                    dc->Arc((int)((cx - r) / scaleFactor), (int)((cy + r) / scaleFactor), 
                                          (int)((cx + r) / scaleFactor), (int)((cy - r) / scaleFactor),
                                          (int)(point2.x / scaleFactor), (int)(point2.y / scaleFactor),
                                          (int)(lastx / scaleFactor), (int)(lasty / scaleFactor));
                                 }
                                 else // counter-clockwise
                                 {
                                    dc->Arc((int)((cx - r) / scaleFactor), (int)((cy + r) / scaleFactor), 
                                          (int)((cx + r) / scaleFactor), (int)((cy - r) / scaleFactor),
                                          (int)(lastx / scaleFactor), (int)(lasty / scaleFactor), 
                                          (int)(point2.x / scaleFactor), (int)(point2.y / scaleFactor));  
                                 }
                                 dc->MoveTo((int)(point2.x / scaleFactor),(int)(point2.y / scaleFactor));
                              }
                           }

                        }
                     }
                     First = FALSE;

                     bulge = pnt->bulge;
                     lastx = point2.x;
                     lasty = point2.y;
                  }
               }
               DeleteObject(SelectObject(dc->m_hDC, orgPen));
            }
            break;
         }
      }
   }
}

void WorldView::OnSize(UINT nType, int cx, int cy) 
{
   CDialog::OnSize(nType, cx, cy);
   
   Invalidate();
}

BOOL WorldView::OnEraseBkgnd(CDC* pDC) 
{
   CBrush brush;
   COLORREF color = RGB(160, 160, 160);
   if (activeView)
      color = ((CCEtoODBDoc*)activeView->GetDocument())->getSettings().Bckgrnd;
   brush.CreateSolidBrush(color);
   CBrush *oldBrush = pDC->SelectObject(&brush);
   CRect rect;
   pDC->GetClipBox(&rect);
   pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
   pDC->SelectObject(oldBrush);
   
   return TRUE; 
}

void WorldView::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if (activeView)
   {
      CClientDC dc(this); // device context for painting

      doc = (CCEtoODBDoc*)activeView->GetDocument();
   
      PrepareDC(&dc, doc);
   
      dc.DPtoLP(&point);
      anchor = previous = point;

      Anchored = TRUE;

      SetCursor(hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMWINDOW));
   }
   
   CDialog::OnLButtonDown(nFlags, point);
}

void WorldView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if (!Anchored)
      return;

   if (activeView)
   {
      {
         CClientDC dc(this); // device context for painting

         doc = (CCEtoODBDoc*)activeView->GetDocument();
   
         PrepareDC(&dc, doc);
   
         dc.DPtoLP(&point);

         const int ZOOM_THRESHOLD = 100;
         if (abs(anchor.x - point.x) < ZOOM_THRESHOLD || abs(anchor.y - point.y) < ZOOM_THRESHOLD) // pan
         {
            if (((CCEtoODBView*)activeView)->ScaleDenom > 1.1)
               ((CCEtoODBView*)activeView)->PanCoordinate(scaleFactor * anchor.x, scaleFactor * anchor.y, FALSE);
         }
         else // zoom window
            ((CCEtoODBView*)activeView)->ZoomBox(scaleFactor * min(anchor.x, point.x), scaleFactor * max(anchor.x, point.x),
                  scaleFactor * min(anchor.y, point.y), scaleFactor * max(anchor.y, point.y));

         Anchored = FALSE;
      }
   }

   
   CDialog::OnLButtonUp(nFlags, point);
}

void WorldView::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (activeView)
   {
      if (Anchored)
      {
         SetCursor(hCursor);

         CClientDC dc(this); // device context for painting

         doc = (CCEtoODBDoc*)activeView->GetDocument();
   
         PrepareDC(&dc, doc);
   
         dc.DPtoLP(&point);
         
         dc.SetROP2(R2_NOT);
         dc.SelectStockObject(HOLLOW_BRUSH);

         dc.Rectangle(anchor.x, anchor.y, previous.x, previous.y);
         
         previous = point;

         dc.Rectangle(anchor.x, anchor.y, previous.x, previous.y);
      }
   }
   
   CDialog::OnMouseMove(nFlags, point);
}

void LoadWorldViewState()
{
   WINDOWPLACEMENT placement;
   placement.length = sizeof(placement);
   placement.flags = 0L;

   CWaitCursor();
   
   if (!strlen(worldViewFile))   return;

   FILE *stream;
   if ((stream = fopen(worldViewFile, "rt")) == NULL)
      return;

   char  line[500], *tok;

   if (fgets(line,500,stream) == NULL)
      return;

   if (strcmp(line, "! CAMCAD World View\n"))
   {
      ErrorMessage("This is not a CAMCAD World View File");
      return;
   }

   while (fgets(line,500,stream))
   {  
      if (line[0] == '!' || line[0] == '\n') continue; // remark

      if ((tok = strtok(line, " \t\n(),")) == NULL) continue;
      placement.rcNormalPosition.top = atol(tok);

      if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
      placement.rcNormalPosition.left = atol(tok);

      if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
      placement.rcNormalPosition.right = atol(tok);

      if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
      placement.rcNormalPosition.bottom = atol(tok);

      if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
      WorldViewOn = atoi(tok);
      placement.showCmd = WorldViewOn ? SW_SHOW : SW_HIDE;

      worldView->SetWindowPlacement(&placement);
      
      CMenu *menu = AfxGetMainWnd()->GetMenu();
      if (menu)
         menu->CheckMenuItem(ID_WORLDVIEW, (WorldViewOn?MF_CHECKED:MF_UNCHECKED));

      break;
   }
   fclose(stream);
}

void SaveWorldViewState()
{
   WINDOWPLACEMENT placement;
   placement.length = sizeof(placement);
   placement.flags = 0L;

   worldView->GetWindowPlacement(&placement);

   CWaitCursor();

   FILE *stream;
   if ((stream = fopen(worldViewFile, "wt")) == NULL)
      return;

   fputs("! CAMCAD World View\n\n", stream);
   fprintf(stream, "%ld, %ld, %ld, %ld, %d", placement.rcNormalPosition.top, placement.rcNormalPosition.left, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom, WorldViewOn);
   fclose(stream);
}

// end WRLDVIEW.CPP

