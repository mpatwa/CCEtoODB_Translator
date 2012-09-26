
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           
 
#include "stdafx.h"             
#include "add.h"
#include "geomlib.h"
#include "graph.h"
#include "license.h"                      
#include "mainfrm.h"
#include "math.h"
#include "crypt.h"
#include "attrib.h"
#include "polylib.h"
#include "pcbutil.h"
#include "drc.h"
#include "find.h"
#include "response.h"
#include "centroid.h"
#include "Fiducial.h"
#include "CCEtoODB.h"
#include "AddOutline.h"
extern DataStruct *CurrentPolyData; // from EDITPOLY.CPP
extern CDialogBar *CurrentSettings; // from MAINFRM.CPP
extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern CPoint rightClickPnt; // from CCVIEW.CPP

static BlockStruct *lastBlockInserted;
static FileStruct *lastFileUsed;
static int lastLayerUsed;

//void UpdateLayers(CCEtoODBDoc *doc);
void GetWidthIndex(CComboBox *m_widthCB, short *dataWidthIndex, CCEtoODBDoc *doc);
void ResetButtons();
extern bool IsOutlinePresent( CCEtoODBDoc *doc, CDataList *DataList,BlockTypeTag BlockType);
static BOOL Good_File_Layer()
{
   if ( (((CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB))->GetCurSel() == CB_ERR) ||
      (((CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB))->GetCurSel() == CB_ERR))
   {
      if (StatusBar)
         StatusBar->SetWindowText("Current File and Current Layer must be legal");

      ErrorMessage("A File and a Layer must be selected in the Current Settings Dialog before Adding Entities.  Use Add|File and/or Add|Layer.", 
            "See Help File for more info", MB_OK | MB_ICONINFORMATION);
            
      return 0;
   }

   return 1;
}

/******************************************************************************
* Untransform
*/
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file)
{
   // insertion point
   pnt->x -= file->insert_x;
   pnt->y -= file->insert_y;

   // mirror
   double file_rot = file->rotation;
   if (file->mirror) 
   {
      pnt->x = -pnt->x;
      file_rot = -file_rot;
   }

   // rotate
   Mat2x2 m;
   RotMat2(&m, -file_rot);
   TransPoint2(pnt, 1, &m, 0.0, 0.0);

   // scale
   pnt->x /= file->scale;
   pnt->y /= file->scale;
}

/* ResetButtons *****************************************************************/
void ResetButtons()
{
   CurrentPolyData = NULL;

   CToolBarCtrl *c = &((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl();

   c->PressButton(ID_ADD_LINE_SEGMENT, FALSE);
   c->PressButton(ID_ADD_LINE_POLYLINE, FALSE);
   c->PressButton(ID_ADD_CIRCLE, FALSE);
   c->PressButton(ID_ADD_ARC_CENTERRADIUSSTARTDELTA, FALSE);
   c->PressButton(ID_ADD_ARC_COUNTER_CENTERRADIUSSTARTDELTA, FALSE);
   c->PressButton(ID_ADD_ARC_STARTENDCENTER, FALSE);
   c->PressButton(ID_ADD_POINT, FALSE);
   c->PressButton(ID_ADD_TEXT, FALSE);
   c->PressButton(ID_ADD_GEOMETRY, FALSE);
   c->PressButton(ID_ADD_DRCMARKER, FALSE);

   c = &((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl();
   c->PressButton(ID_EDIT_BULGE, FALSE);
   c->PressButton(ID_MOVE_VERTEX, FALSE);
   c->PressButton(ID_ADD_VERTEX, FALSE);
   c->PressButton(ID_DELETE_VERTEX, FALSE);
   c->PressButton(ID_MOVE_SEGMENT, FALSE);
   c->PressButton(ID_DELETE_SEGMENT, FALSE);
   c->PressButton(ID_MERGE_POLY, FALSE);
   c->PressButton(ID_LINK_POLYS, FALSE);
   c->PressButton(ID_UNLINK_POLY, FALSE);

   c = &((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl();
   c->PressButton(ID_RL_LEADER, FALSE);
   c->PressButton(ID_RL_BOX, FALSE);
   c->PressButton(ID_RL_CLOUD, FALSE);
   c->PressButton(ID_RL_CROSS_OUT, FALSE);
   c->PressButton(ID_RL_SKETCH, FALSE);
   c->PressButton(ID_RL_NOTE, FALSE);

   c = &((CMainFrame*)AfxGetMainWnd())->m_JoinToolBar.GetToolBarCtrl();
   c->PressButton(ID_JOIN_SNAP, FALSE);
   c->PressButton(ID_JOIN_EXTEND, FALSE);
   c->PressButton(ID_JOIN_FILLET, FALSE);
   c->PressButton(ID_JOIN_CHAMFER, FALSE);

   if (getActiveView()->getMeasure() != NULL)
   {
      getActiveView()->getMeasure()->setMeasureOn(false);
   }
}

/* OnAddLineSegment *****************************************************************/
void CCEtoODBView::OnAddLineSegment() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddLineSegment)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   //HideSearchCursor();
   cursorMode = AddLineSegment;
   //ShowSearchCursor();
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_LINE_SEGMENT);
#endif
}

void CCEtoODBView::OnMouseMove_AddLineSegment(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
}

void CCEtoODBView::OnLButtonDown_AddLineSegment(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;

   if (!cursorAnchored)
   {
      cursorAnchored = TRUE;
      anchor_x = x;
      anchor_y = y;
      cursorAnchor.x = round(x / factor);
      cursorAnchor.y = round(y / factor);
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;

   if (!Good_File_Layer())
   {
      ResetButtons();
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
   short width;
   GetWidthIndex(CurrentWidth, &width, doc);
   FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel()));
   doc->PrepareAddEntity(file);

   s.setParentDataList(&(file->getBlock()->getDataList()));
   s.scale = file->getScale();
   s.insert_x = file->getInsertX();
   s.insert_y = file->getInsertY();
   s.rotation = file->getRotation();
   s.mirror = file->getResultantMirror(doc->getBottomView());
   Point2 p1, p2;
   p1.x = anchor_x;
   p1.y = anchor_y;
   p2.x = x;
   p2.y = y;
   Untransform(doc, &p1, &s);
   Untransform(doc, &p2, &s);
   s.setData(Graph_Line(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(),
                         p1.x, p1.y, p2.x, p2.y, 0, width, FALSE));

   doc->DrawEntity(&s, 0, FALSE);

   InsertAddUndo(&s, doc);
}

/* OnAddLinePolyline *****************************************************************/
void CCEtoODBView::OnAddLinePolyline() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddPolyline)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   //HideSearchCursor();
   cursorMode = AddPolyline;
   //ShowSearchCursor();

   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_LINE_POLYLINE);
#endif
}

void CCEtoODBView::OnMouseMove_AddPolyline(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
}

void CCEtoODBView::OnLButtonDown_AddPolyline(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;

   CPoint temp;
   temp.x = round(x / factor);
   temp.y = round(y / factor);

   OnMouseMove_AddPolyline(dc, &temp);

   static UndoItemStruct *i;

   if (!cursorAnchored)
   {
      CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
      CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
      CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
      CCEtoODBDoc *doc = GetDocument();
      short width;
      GetWidthIndex(CurrentWidth, &width, doc);
      FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel()));
      doc->PrepareAddEntity(file);
      s.setParentDataList(&(file->getBlock()->getDataList()));
      s.scale = file->getScale();
      s.insert_x = file->getInsertX();
      s.insert_y = file->getInsertY();
      s.rotation = file->getRotation();
      s.mirror = file->getResultantMirror(doc->getBottomView());
      s.setData(Graph_PolyStruct(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(),0L, 0));
      Graph_Poly(NULL, width, 0, 0, 0);

      i = InsertAddUndo(&s, doc);
   }

   cursorAnchor.x = round(x / factor);
   cursorAnchor.y = round(y / factor);

   Point2 p;
   p.x = x;
   p.y = y;
   Untransform(doc, &p, &s);

   Graph_Vertex(p.x, p.y, 0.0);

   // update Add Undo
   FreePolyList(i->data->getPolyList());
   CopyPoly(i->data, s.getData());

   cursorFirstDrag = TRUE;
   cursorAnchored = TRUE;

   //HideSearchCursor();
   doc->DrawEntity(&s, 0, FALSE);
   //ShowSearchCursor();
}

void CCEtoODBView::OnPolylineCloseAndFill() 
{
   if (cursorMode != AddPolyline)
   {
      ErrorMessage("You are not in Add Polyline Mode\nUse Add | Polyline in menu or on Add Toolbar", 
         "Error", MB_OK | MB_ICONSTOP);
      return;
   }

   s.getData()->getPolyList()->GetHead()->setFilled(true);
   OnPolylineClose();
   //HideSearchCursor();
   GetDocument()->DrawEntity(&s, 0, FALSE);
   //ShowSearchCursor();
}

void CCEtoODBView::OnPolylineClose() 
{
   if (cursorMode != AddPolyline)
   {
      ErrorMessage("You are not in Add Polyline Mode\nUse Add | Polyline in menu or on Add Toolbar", 
         "Error", MB_OK | MB_ICONSTOP);
      return;
   }

   if (!cursorAnchored)
      return;

   if (!cursorFirstDrag)
   {
      CClientDC dc(this);
      //OnPrepareDC(&dc);
      dc.SetROP2(R2_NOT);
      dc.MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc.LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   }

   CPoly *poly = s.getData()->getPolyList()->GetHead();
   CPnt *p1 = poly->getPntList().GetHead();
   Graph_Vertex(p1->x, p1->y, 0.0);

   poly->setClosed(true);

   ResetButtons();
   //HideSearchCursor();
   GetDocument()->DrawEntity(&s, 0, FALSE);
   cursorMode = Search;
   //ShowSearchCursor();
}

void CCEtoODBView::OnPolylineEnd() 
{
   if (cursorMode != AddPolyline)
   {
      ErrorMessage("You are not in Add Polyline Mode\nUse Add | Polyline in menu or on Add Toolbar", 
         "Error", MB_OK | MB_ICONSTOP);
      return;
   }

   if (!cursorAnchored) return;

   ResetButtons();
   //HideSearchCursor();
   cursorMode = Search;
   //ShowSearchCursor();
}

/* OnAddCircle *****************************************************************/
void CCEtoODBView::OnAddCircle() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddCircle)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   radius = 0.0;
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_CIRCLE);
   //HideSearchCursor();
   cursorMode = AddCircle;
   //ShowSearchCursor();
#endif
}

void CCEtoODBView::OnMouseMove_AddCircle(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   CCEtoODBDoc *doc = GetDocument();

   dc->SetROP2(R2_NOT);

   HBRUSH orgBrush = (HBRUSH)dc->SelectObject(GetStockObject(HOLLOW_BRUSH));

   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
            round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;
   radius = sqrt(pow(1.0f * cursorAnchor.x - cursorPrevPnt.x, 2) + pow(1.0f * cursorAnchor.y - cursorPrevPnt.y, 2));
   
   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
         round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));

   dc->SelectObject(orgBrush);
}

void CCEtoODBView::OnLButtonDown_AddCircle(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   if (!cursorAnchored)
   {
      cursorAnchored = TRUE;
      anchor_x = x;
      anchor_y = y;
      cursorAnchor.x = round(x / factor);
      cursorAnchor.y = round(y / factor);
      return;
   }
   
   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(round(x / factor), round(y / factor));

   if (!Good_File_Layer())
   {
      ResetButtons();
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
   short width;
   GetWidthIndex(CurrentWidth, &width, doc);
   FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel()));
   doc->PrepareAddEntity(file);

   s.setParentDataList(&(file->getBlock()->getDataList()));
   s.scale = file->getScale();
   s.insert_x = file->getInsertX();
   s.insert_y = file->getInsertY();
   s.rotation = file->getRotation();
   s.mirror = file->getResultantMirror(doc->getBottomView());
   Point2 c;
   c.x = anchor_x;
   c.y = anchor_y;
   Untransform(doc, &c, &s);
   radius = sqrt(pow(anchor_x - x, 2) + pow(anchor_y - y, 2));
   s.setData(Graph_Circle(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(),
                              c.x, c.y, radius / file->getScale(), 0, width, FALSE, FALSE));

   doc->DrawEntity(&s, 0, FALSE);

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   radius = 0.0;

   InsertAddUndo(&s, doc);
}

/* OnAddArc_CenterStartEnd *****************************************************************/
void CCEtoODBView::OnAddArcCRSE() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddArcCRSE)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = 0;
   radius = 0.0;
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_ARC_CENTERRADIUSSTARTDELTA);
   //HideSearchCursor();
   cursorMode = AddArcCRSE;
   //ShowSearchCursor();
   CounterClockwise = FALSE;
#endif
}

void CCEtoODBView::OnAddArcCRSECounter() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if ( (((CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB))->GetCurSel() == CB_ERR) ||
      (((CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB))->GetCurSel() == CB_ERR))
   {
      if (StatusBar)
         StatusBar->SetWindowText("Current File and Current Layer must be legal");
      return;
   }

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddArcCounterCRSE)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = 0;
   radius = 0.0;
   //HideSearchCursor();
   cursorMode = AddArcCounterCRSE;
   //ShowSearchCursor();
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_ARC_COUNTER_CENTERRADIUSSTARTDELTA);
   CounterClockwise = TRUE;
#endif
}

void CCEtoODBView::OnMouseMove_AddArcCRSE(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();

   dc->SetROP2(R2_NOT);

   switch (cursorAnchored) {
   case 0: return;

   case 1: // center set, draw circle
   {
      HBRUSH orgBrush = (HBRUSH)dc->SelectObject(GetStockObject(HOLLOW_BRUSH));

      if (!cursorFirstDrag)
      {     
         dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
         dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
         dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
               round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));
      }
      cursorFirstDrag = FALSE;

      cursorPrevPnt.x = pnt->x;
      cursorPrevPnt.y = pnt->y;
      radius = sqrt(pow(1.0f * cursorAnchor.x - cursorPrevPnt.x, 2) + pow(1.0f * cursorAnchor.y - cursorPrevPnt.y, 2));
   
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
            round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));

      dc->SelectObject(orgBrush);
      break;
   }

   case 2: // radius set, draw start angle
   {
      int dx, dy;

      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + _x, cursorAnchor.y + _y);

      dx = pnt->x - cursorAnchor.x;
      dy = pnt->y - cursorAnchor.y;

      if (dx) _x = (int)(1.0 * radius / sqrt(1 + pow(1.0*dy/dx, 2))); else _x = 0;
      if (dx < 0) _x *= -1;

      if (dy) _y = (int)(1.0 * radius / sqrt(1 + pow(1.0*dx/dy, 2))); else _y = 0;
      if (dy < 0) _y *= -1;

      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + _x, cursorAnchor.y + _y);
      break;
   }

   case 3: // start set, draw end
   {
      int dx, dy;

      if (!cursorFirstDrag)
      {     
         dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
         dc->LineTo(cursorAnchor.x + _x, cursorAnchor.y + _y);
         if (CounterClockwise)
            dc->Arc(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
                  round(cursorAnchor.x + radius), round(cursorAnchor.y - radius),
                  cursorAnchor2.x, cursorAnchor2.y, cursorPrevPnt.x, cursorPrevPnt.y);
         else
            dc->Arc(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
                  round(cursorAnchor.x + radius), round(cursorAnchor.y - radius),
                  cursorPrevPnt.x, cursorPrevPnt.y, cursorAnchor2.x, cursorAnchor2.y);
      }
      cursorFirstDrag = FALSE;

      cursorPrevPnt.x = pnt->x;
      cursorPrevPnt.y = pnt->y;

      dx = pnt->x - cursorAnchor.x;
      dy = pnt->y - cursorAnchor.y;

      if (dx) _x = (int)(1.0 * radius / sqrt(1 + pow(1.0*dy/dx, 2))); else _x = 0;
      if (dx < 0) _x *= -1;

      if (dy) _y = (int)(1.0 * radius / sqrt(1 + pow(1.0*dx/dy, 2))); else _y = 0;
      if (dy < 0) _y *= -1;

      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + _x, cursorAnchor.y + _y);
      if (CounterClockwise)
         dc->Arc(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
               round(cursorAnchor.x + radius), round(cursorAnchor.y - radius),
               cursorAnchor2.x, cursorAnchor2.y, cursorPrevPnt.x, cursorPrevPnt.y);
      else
         dc->Arc(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
               round(cursorAnchor.x + radius), round(cursorAnchor.y - radius),
               cursorPrevPnt.x, cursorPrevPnt.y, cursorAnchor2.x, cursorAnchor2.y);
      break;
   }
   }
}

void CCEtoODBView::OnLButtonDown_AddArcCRSE(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   switch (cursorAnchored)
   {
   case 0:
      anchor_x = x;
      anchor_y = y;
      cursorAnchor.x = round(x / factor);
      cursorAnchor.y = round(y / factor);
      cursorAnchored++;
      break;
   case 1:
   {
      // set up x and y for start angle to begin where radius is
      int dx, dy;
      dx = round(x / factor) - cursorAnchor.x;
      dy = round(y / factor) - cursorAnchor.y;

      if (dx) _x = (int)(1.0 * radius / sqrt(1 + pow(1.0*dy/dx, 2))); else _x = 0;
      if (dx < 0) _x *= -1;

      if (dy) _y = (int)(1.0 * radius / sqrt(1 + pow(1.0*dx/dy, 2))); else _y = 0;
      if (dy < 0) _y *= -1;
      cursorAnchored++;
      break;
   }
   case 2:
   {
      anchor2_x = x;
      anchor2_y = y;
      cursorAnchor2.x = round(x / factor);
      cursorAnchor2.y = round(y / factor);
      cursorFirstDrag = TRUE;
      HBRUSH orgBrush = (HBRUSH)dc->SelectObject(GetStockObject(HOLLOW_BRUSH));
      dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
               round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));
      dc->SelectObject(orgBrush);
      cursorAnchored++;
      break;
   }
   case 3:
   {
      // erase end angle radius
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + _x, cursorAnchor.y + _y);

      // erase start angle radius
      int dx, dy;
      dx = cursorAnchor2.x - cursorAnchor.x;
      dy = cursorAnchor2.y - cursorAnchor.y;

      if (dx) _x = (int)(1.0 * radius / sqrt(1 + pow(1.0*dy/dx, 2))); else _x = 0;
      if (dx < 0) _x *= -1;

      if (dy) _y = (int)(1.0 * radius / sqrt(1 + pow(1.0*dx/dy, 2))); else _y = 0;
      if (dy < 0) _y *= -1;
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + _x, cursorAnchor.y + _y);
   
      if (!Good_File_Layer())
      {
         ResetButtons();
         //HideSearchCursor();
         cursorMode = Search;
         //ShowSearchCursor();
         return;
      }

      CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
      CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
      CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
      short width;
      GetWidthIndex(CurrentWidth, &width, doc);
      FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel()));
      doc->PrepareAddEntity(file);

      s.setParentDataList(&(file->getBlock()->getDataList()));
      s.scale = file->getScale();
      s.insert_x = file->getInsertX();
      s.insert_y = file->getInsertY();
      s.rotation = file->getRotation();
      s.mirror = file->getResultantMirror(doc->getBottomView());
      Point2 a, b, c;
      a.x = anchor2_x;
      a.y = anchor2_y;
      b.x = x;
      b.y = y;
      c.x = anchor_x;
      c.y = anchor_y;
      Untransform(doc, &a, &s);
      Untransform(doc, &b, &s);
      Untransform(doc, &c, &s);
      double startAngle, endAngle, deltaAngle;
      startAngle = ArcTan2(a.y - c.y, a.x - c.x);
      endAngle = ArcTan2(b.y - c.y, b.x - c.x);
      if (startAngle > endAngle) endAngle += 2.0 * PI;
      deltaAngle = endAngle - startAngle;
      if (!CounterClockwise ^ file->isMirrored() ^ doc->getBottomView()) deltaAngle -= 2.0 * PI;
      s.setData(Graph_Arc(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(),
                           c.x, c.y, factor / file->getScale() * radius, 
                           startAngle, deltaAngle, 0, width, FALSE));
   
      doc->DrawEntity(&s, 0, FALSE);

      cursorFirstDrag = TRUE;
      cursorAnchored = 0;
      radius = 0.0;

      InsertAddUndo(&s, GetDocument());
      break;
   }
   }
}

/* OnAddArc_StartEndCenter *****************************************************************/
void CCEtoODBView::OnAddArcSEC() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddArcSEC)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = 0;
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_ARC_STARTENDCENTER);
   //HideSearchCursor();
   cursorMode = AddArcSEC;
   //ShowSearchCursor();
#endif
}

void CCEtoODBView::OnMouseMove_AddArcSEC(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();

   dc->SetROP2(R2_NOT);

   static double c_x, c_y;

   switch (cursorAnchored)
   {
   case 0:
      return;

   case 1: // 1st point, draw chord
      if (!cursorFirstDrag)
      {
         dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
         dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      }
      cursorFirstDrag = FALSE;

      c_x = cursorPrevPnt.x = pnt->x;
      c_y = cursorPrevPnt.y = pnt->y;
   
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      break;

   case 2: // 2nd point, draw arc
   {
      if (!cursorFirstDrag)
      {
         if (CounterClockwise)
            dc->Arc((int)(c_x - radius), (int)(c_y + radius), (int)(c_x + radius), (int)(c_y - radius), 
                  cursorAnchor2.x, cursorAnchor2.y, cursorAnchor.x, cursorAnchor.y);
         else
            dc->Arc((int)(c_x - radius), (int)(c_y + radius), (int)(c_x + radius), (int)(c_y - radius), 
                  cursorAnchor.x, cursorAnchor.y, cursorAnchor2.x, cursorAnchor2.y);
      }
      cursorFirstDrag = FALSE;

      cursorPrevPnt.x = pnt->x;
      cursorPrevPnt.y = pnt->y;
      CirclePoint3(cursorAnchor.x, cursorAnchor.y, cursorAnchor2.x, cursorAnchor2.y, pnt->x, pnt->y, &c_x, &c_y, &radius);
   
      CounterClockwise = (cross_product(cursorAnchor.x, cursorAnchor.y, cursorAnchor2.x, cursorAnchor2.y)
            + cross_product(cursorAnchor2.x, cursorAnchor2.y, pnt->x, pnt->y)
            + cross_product(pnt->x, pnt->y, cursorAnchor.x, cursorAnchor.y) > 0.0);
      if (CounterClockwise)
         dc->Arc((int)(c_x - radius), (int)(c_y + radius), (int)(c_x + radius), (int)(c_y - radius), 
               cursorAnchor2.x, cursorAnchor2.y, cursorAnchor.x, cursorAnchor.y);
      else
         dc->Arc((int)(c_x - radius), (int)(c_y + radius), (int)(c_x + radius), (int)(c_y - radius), 
               cursorAnchor.x, cursorAnchor.y, cursorAnchor2.x, cursorAnchor2.y);
      break;
   }
   }
}

void CCEtoODBView::OnLButtonDown_AddArcSEC(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   switch (cursorAnchored) 
   {
   case 0:
      anchor_x = x;
      anchor_y = y;
      cursorAnchor.x = round(x / factor);
      cursorAnchor.y = round(y / factor);
      cursorAnchored++;
      return;
   case 1:
      anchor2_x = x;
      anchor2_y = y;
      cursorAnchor2.x = round(x / factor);
      cursorAnchor2.y = round(y / factor);
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y); // erase chord
      dc->LineTo(cursorAnchor2.x, cursorAnchor2.y);
      cursorFirstDrag = TRUE;
      cursorAnchored++;
      return;
   case 2: 
   {
      if (!Good_File_Layer())
      {
         ResetButtons();
         //HideSearchCursor();
         cursorMode = Search;
         //ShowSearchCursor();
         return;
      }

      CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
      CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
      CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
      short width;
      GetWidthIndex(CurrentWidth, &width, doc);
      FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel()));
      doc->PrepareAddEntity(file);

      s.setParentDataList(&(file->getBlock()->getDataList()));
      s.scale = file->getScale();
      s.insert_x = file->getInsertX();
      s.insert_y = file->getInsertY();
      s.rotation = file->getRotation();
      s.mirror = file->getResultantMirror(doc->getBottomView());
      Point2 a, b, c;
      a.x = anchor_x;
      a.y = anchor_y;
      b.x = x;
      b.y = y;
      c.x = anchor2_x;
      c.y = anchor2_y;
      Untransform(doc, &a, &s);
      Untransform(doc, &b, &s);
      Untransform(doc, &c, &s);
      double startAngle, deltaAngle, center_x, center_y, radius;
      ArcPoint3(a.x, a.y, b.x, b.y, c.x, c.y, 
          &center_x, &center_y, &radius, &startAngle, &deltaAngle);
      s.setData(Graph_Arc(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(),
                           center_x, center_y, radius, 
                           startAngle, deltaAngle, 0, width, FALSE));
   
      doc->DrawEntity(&s, 0, FALSE);
   
      cursorAnchored = 0;
      cursorFirstDrag = TRUE;

      InsertAddUndo(&s, GetDocument());
   }
   }
}

/* OnAddPoint *****************************************************************/
void CCEtoODBView::OnAddPoint() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddPoint)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   radius = 0.0;
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_POINT);
   //HideSearchCursor();
   cursorMode = AddPoint;
   ///ShowSearchCursor();
#endif
}

void CCEtoODBView::OnMouseMove_AddPoint(CDC *dc, CPoint *pnt)
{
/* if (!cursorAnchored) return;

   CCEtoODBDoc *doc = GetDocument();

   dc->SetROP2(R2_NOT);

   HBRUSH orgBrush = (HBRUSH)dc->SelectObject(GetStockObject(HOLLOW_BRUSH));

   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
            round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;
   radius = sqrt(pow(cursorAnchor.x - cursorPrevPnt.x, 2) + pow(cursorAnchor.y - cursorPrevPnt.y, 2));
   
   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   dc->Ellipse(round(cursorAnchor.x - radius), round(cursorAnchor.y + radius), 
         round(cursorAnchor.x + radius), round(cursorAnchor.y - radius));

   dc->SelectObject(orgBrush);*/
}

void CCEtoODBView::OnLButtonDown_AddPoint(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   if (!Good_File_Layer())
   {
      ResetButtons();
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
   short width;
   GetWidthIndex(CurrentWidth, &width, doc);
   FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel()));
   doc->PrepareAddEntity(file);

   s.setParentDataList(&(file->getBlock()->getDataList()));
   s.scale = file->getScale();
   s.insert_x = file->getInsertX();
   s.insert_y = file->getInsertY();
   s.rotation = file->getRotation();
   s.mirror = file->getResultantMirror(doc->getBottomView());
   Point2 p;
   p.x = x;
   p.y = y;
   Untransform(doc, &p, &s);
   s.setData(Graph_Point(p.x, p.y, ((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(), 0L, FALSE));

   //HideSearchCursor();
   doc->DrawEntity(&s, 0, FALSE);
   //ShowSearchCursor();

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   radius = 0.0;

   InsertAddUndo(&s, doc);
}

/* OnAddText *****************************************************************/
AddTextDlg *addTextDlg;

void CCEtoODBView::OnAddText() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   if (!CFontList::fontIsLoaded())
   {
      ErrorMessage("No font file loaded");
      return;
   }

   CString buf;
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT))->GetWindowText(buf);
   if (atof(buf) < SMALLNUMBER)
   {
      ErrorMessage("The Text Size in Current Settings is not legal!", "Current Settings Error");
      return;
   }

   ResetButtons();
   if (cursorMode == AddText)
   {
      //HideSearchCursor();
      cursorMode = Search;
      ///ShowSearchCursor();
      delete addTextDlg;
      CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
      FileStruct *file = (FileStruct *)CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel());
      file->getBlock()->getDataList().RemoveAt(file->getBlock()->getDataList().Find(s.getData()));
      delete s.getData();
      s.setData(NULL);
      return;
   }

   addTextDlg = new AddTextDlg();
   addTextDlg->view = this;
   if (!addTextDlg->Create(IDD_ADD_TEXT, NULL))
   {
      delete addTextDlg;
      return;
   }

   CCEtoODBDoc *doc = GetDocument();
   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   //HideSearchCursor();
   cursorMode = AddText;
   ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_TEXT);

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   FileStruct *file = (FileStruct *)CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel());
   s.setParentDataList(&(file->getBlock()->getDataList()));
   s.scale = file->getScale();
   s.insert_x = file->getInsertX();
   s.insert_y = file->getInsertY();
   s.rotation = file->getRotation();
   s.mirror = file->getResultantMirror(doc->getBottomView());
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   GetDocument()->PrepareAddEntity(file);
   s.setData(Graph_Text(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex(),
                           "a", 0.0, 0.0, 0.0, 0.0, 0.0, 0, 1, 0, 0, 0, 0, 0));
#endif
}

void CCEtoODBView::OnMouseMove_AddText(CDC *dc, CPoint *pnt)
{
   if (cursorAnchored) return;

   dc->SetROP2(R2_NOT);

   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   if (!cursorFirstDrag)
      doc->DrawEntity(&s, -2, FALSE);
   cursorFirstDrag = FALSE;

   CString buf;
/*
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT))->GetWindowText(buf);
   if (atof(buf) < SMALLNUMBER)
   {
      ErrorMessage("The Text Size in Current Settings is not legal!", "Current Settings Error");
      return;
   }
*/

   Point2 p;
   p.x = factor * pnt->x;
   p.y = factor * pnt->y;
   Untransform(doc, &p, &s);
   s.getData()->getText()->setPnt((DbUnit)p.x,(DbUnit)p.y);
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_ROTATION))->GetWindowText(buf);
   s.getData()->getText()->setRotation(DegToRad(atof(buf)));
   s.getData()->getText()->setMirrored(((CButton*)CurrentSettings->GetDlgItem(IDC_MIRROR))->GetCheck() ^ (int)doc->getBottomView());

   ((CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT))->GetWindowText(buf);
   s.getData()->getText()->setHeight(atof(buf));
   s.getData()->getText()->setWidth(s.getData()->getText()->getHeight() / 2);

   addTextDlg->UpdateData();

   CString oldLF, newLF;
   newLF.Format("%c", '\n');
   oldLF.Format("%c%c", 13, 10);
   addTextDlg->m_text.Replace(oldLF, newLF);
   s.getData()->getText()->setText(STRDUP(addTextDlg->m_text));

   doc->DrawEntity(&s, -2, FALSE);
}

void CCEtoODBView::OnLButtonDown_AddText(CDC *dc, double x, double y)
{
   addTextDlg->UpdateData();

   cursorFirstDrag = TRUE;
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   if (!Good_File_Layer())
   {
      ResetButtons();
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      delete addTextDlg;
      return;
   }

   CString buf;
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT))->GetWindowText(buf);
   if (atof(buf) < SMALLNUMBER)
   {
      ErrorMessage("The Text Size in Current Settings is not legal!", "Current Settings Error");
      return;
   }

   Point2 p;
   p.x = x;
   p.y = y;
   Untransform(doc, &p, &s);
   s.getData()->getText()->setPnt((DbUnit)p.x,(DbUnit)p.y);
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_ROTATION))->GetWindowText(buf);
   s.getData()->getText()->setRotation(DegToRad(atof(buf)));
   s.getData()->getText()->setMirrored(((CButton*)CurrentSettings->GetDlgItem(IDC_MIRROR))->GetCheck() ^ (int)doc->getBottomView());
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT))->GetWindowText(buf);

   s.getData()->getText()->setHeight(atof(buf));
   s.getData()->getText()->setWidth(s.getData()->getText()->getHeight() / 2);
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   s.getData()->setLayerIndex(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex());

   addTextDlg->UpdateData();

   CString oldLF, newLF;
   newLF.Format("%c", '\n');
   oldLF.Format("%c%c", 13, 10);
   addTextDlg->m_text.Replace(oldLF, newLF);
   s.getData()->getText()->setText(STRDUP(addTextDlg->m_text));

   doc->DrawEntity(&s, 0, FALSE);

   ResetButtons();
   //HideSearchCursor();
   cursorMode = Search;
   //ShowSearchCursor();
   delete addTextDlg;

   InsertAddUndo(&s, doc);
}

/////////////////////////////////////////////////////////////////////////////
// AddTextDlg dialog
AddTextDlg::AddTextDlg(CWnd* pParent /*=NULL*/)
   : CResizingDialog(AddTextDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(AddTextDlg)
   m_text = _T("");
   //}}AFX_DATA_INIT
}

AddTextDlg::~AddTextDlg()
{
   if (::IsWindow(this->m_hWnd))
   {
      addTextDlg->DestroyWindow();
   }
}

void AddTextDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AddTextDlg)
   DDX_Text(pDX, IDC_TEXT, m_text);
   //}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(AddTextDlg, CResizingDialog)
   //{{AFX_MSG_MAP(AddTextDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void AddTextDlg::OnOK()
{
   return;
}

void AddTextDlg::OnCancel() 
{
   //view->HideSearchCursor();
   view->cursorMode = Search;
   if (!view->cursorFirstDrag)
      view->GetDocument()->DrawEntity(&(view->s), -2, FALSE);
   //view->ShowSearchCursor();
   ResetButtons();
   CResizingDialog::OnCancel();
   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   FileStruct *file = (FileStruct *)CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel());
   file->getBlock()->getDataList().RemoveAt(file->getBlock()->getDataList().Find(view->s.getData()));

   delete view->s.getData();
   view->s.setData(NULL);

   delete addTextDlg;
}

/* OnAddDRCMarker *****************************************************************/
void CCEtoODBView::OnAddDRCMarker() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddDRCMarker)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   //HideSearchCursor();
   cursorMode = AddDRCMarker;
   //ShowSearchCursor();
#endif
}

void CCEtoODBView::OnLButtonDown_AddDRCMarker(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();

   if (!Good_File_Layer())
   {
      ResetButtons();
      ////HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   lastCommand = LC_AddDRCMarker;

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   FileStruct *file = (FileStruct *)CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel());

   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   LayerStruct *layer = (LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel());

   s.setParentDataList(&(file->getBlock()->getDataList()));
   s.scale = file->getScale();
   s.insert_x = file->getInsertX();
   s.insert_y = file->getInsertY();
   s.rotation = file->getRotation();
   s.mirror = file->getResultantMirror(doc->getBottomView());

   Point2 p;
   p.x = x;
   p.y = y;
   Untransform(doc, &p, &s);

   CString buf;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   buf.Format("DRC Marker at (%+.*lf,%+.*lf)", decimals, p.x, decimals, p.y);
   DataStruct *data;
   int algIndex = GetAlgorithmNameIndex(doc, "User DRCs");
   DRCStruct *drc = AddDRCAndMarker(doc, file, p.x, p.y, buf, DRC_CLASS_SIMPLE, 0, algIndex, DRC_ALG_GENERIC, layer->getName(), &data);

   CDataList *datalist;
   s.setData(FindDataEntity(doc, drc->getInsertEntityNumber(), &datalist, NULL));

   buf.Format("%lu", drc->getEntityNumber());
   SendResponse("Add DRC Marker", buf);

   //HideSearchCursor();
   cursorMode = Search;
   doc->DrawEntity(&s, 0, FALSE);
   //ShowSearchCursor();
}


/* OnAddInsert *****************************************************************/
void CCEtoODBView::OnAddInsert() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   if (!Good_File_Layer())
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == AddInsert)
   {
      ////HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   FileStruct *file = (FileStruct *)CurrentFile->GetItemDataPtr(CurrentFile->GetCurSel());
   AddInsertDlg dlg;
   dlg.doc = GetDocument();
   dlg.filenum = file->getFileNumber();
   if (dlg.DoModal() != IDOK) return;

   lastCommand = LC_AddInsert;
   lastBlockInserted = dlg.block;
   lastFileUsed = file;

   CCEtoODBDoc *doc = GetDocument();

   cursorFirstDrag = TRUE;

   //HideSearchCursor();
   cursorMode = AddInsert;
   //ShowSearchCursor();

   s.setParentDataList(&(file->getBlock()->getDataList()));
   s.scale = file->getScale();
   s.insert_x = file->getInsertX();
   s.insert_y = file->getInsertY();
   s.rotation = file->getRotation();
   s.mirror = file->getResultantMirror(doc->getBottomView());
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   int layer = -1, sel;
   if ((sel = CurrentLayer->GetCurSel()) != CB_ERR)
      layer = sel;
   GetDocument()->PrepareAddEntity(file);

   lastLayerUsed = layer;

   s.setData(Graph_Block_Reference(dlg.block->getName(), NULL, dlg.block->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layer, 0));
   s.getData()->getInsert()->setInsertType(default_inserttype(dlg.block->getBlockType()));
#endif
}

void CCEtoODBView::AddAnotherInsert() 
{
   if (!Good_File_Layer())
      return;

   ResetButtons();

   cursorFirstDrag = TRUE;
   //HideSearchCursor();
   cursorMode = AddInsert;
   zoomMode = NoZoom;
   //ShowSearchCursor();
// ((CMainFrame*)AfxGetMainWnd())->m_AddToolBar.GetToolBarCtrl().PressButton(ID_ADD_INSERT);

   CCEtoODBDoc *doc = GetDocument();
   s.setParentDataList(&(lastFileUsed->getBlock()->getDataList()));
   s.scale = lastFileUsed->getScale();
   s.insert_x = lastFileUsed->getInsertX();
   s.insert_y = lastFileUsed->getInsertY();
   s.rotation = lastFileUsed->getRotation();
   s.mirror = lastFileUsed->getResultantMirror(doc->getBottomView());
   GetDocument()->PrepareAddEntity(lastFileUsed);

   s.setData(Graph_Block_Reference(lastBlockInserted->getName(), NULL, lastBlockInserted->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, lastLayerUsed, 0));
   s.getData()->getInsert()->setInsertType(default_inserttype(lastBlockInserted->getBlockType()));
}

/* OnAddFiducial *****************************************************************/
void CCEtoODBView::OnAddFiducial()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

#ifndef SHAREWARE

   CCEtoODBDoc* doc = GetDocument();
   if (doc->getFileList().GetCount() == 0)
   {
      ErrorMessage("A File and a Layer must be selected in the Current Settings Dialog before Adding Entities.  Use Add|File and/or Add|Layer.", 
            "See Help File for more info", MB_OK | MB_ICONINFORMATION);
      return;
   }

   FileStruct* file = doc->getFileList().GetOnlyShown(blockTypeUndefined);
   if (file ==  NULL)
   {
      CMessageFilter message(messageFilterTypeMessage);
      message.formatMessageBoxApp("Multiple visible files detected.  Only single visible file is allowed for Add Fiducial.");
   }
   else
   {
      CAddFiducialDlg dlg(*doc, *file, 0.0, 0.0);
      dlg.DoModal();
   }

#endif
}

void CCEtoODBView::OnPopupAddFiducial()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

#ifndef SHAREWARE

   CCEtoODBDoc* doc = GetDocument();
   if (doc->getFileList().GetCount() == 0)
   {
      ErrorMessage("A File and a Layer must be selected in the Current Settings Dialog before Adding Entities.  Use Add|File and/or Add|Layer.", 
            "See Help File for more info", MB_OK | MB_ICONINFORMATION);
      return;
   }

   FileStruct* file = doc->getFileList().GetOnlyShown(blockTypeUndefined);
   if (file ==  NULL)
   {
      CMessageFilter message(messageFilterTypeMessage);
      message.formatMessageBoxApp("Multiple visible files detected.  Only single visible file is allowed for Add Fiducial.");
   }
   else
   {
      CAddFiducialDlg dlg(*doc, *file, lastRightClickX, lastRightClickY);
      dlg.DoModal();
   }

#endif
}

/* OnAddOutline *****************************************************************/
void CCEtoODBView::OnAddOutline()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

#ifndef SHAREWARE

   CCEtoODBDoc* doc = GetDocument();
   if (doc->getFileList().GetCount() == 0)
   {
      ErrorMessage("A File must be selected in the Current Settings Dialog before Adding Entities.  Use Add|File .", 
            "See Help File for more info", MB_OK | MB_ICONINFORMATION);
      return;
   }

   FileStruct* file = doc->getFileList().GetOnlyShown(blockTypeUndefined);
   if (file ==  NULL)
   {
      CMessageFilter message(messageFilterTypeMessage);
      message.formatMessageBoxApp("Multiple visible files detected.  Only single visible file is allowed for Adding Outline.");
   }
   else
   {
      bool res = IsOutlinePresent(doc, &(file->getBlock()->getDataList()),file->getBlockType());
      if(res)
      {
         if(file->getBlockType() == blockTypePanel)
            AfxMessageBox("Panel outline graphic already defined.\n");
         if(file->getBlockType() == blockTypePcb)
            AfxMessageBox("Board outline graphic already defined.\n");
      }
      else
      {
         CAddOutlineDlg dlg(*doc, *file, 0.0, 0.0);
         dlg.DoModal();
      }
   }

#endif
}

/******************************************************************************
* CCEtoODBView::OnMouseMove_AddInsert
*/
void CCEtoODBView::OnMouseMove_AddInsert(CDC *dc, CPoint *pnt)
{
	if (cursorFirstDrag)
	{
		cursorFirstDrag = FALSE;
		return;
	}

   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   Point2 p;
   p.x = factor * pnt->x;
   p.y = factor * pnt->y;

	bool samePoint = (fabs(s.getData()->getInsert()->getOriginX() - p.x) <= SMALLNUMBER && (fabs(s.getData()->getInsert()->getOriginY() - p.y) <= SMALLNUMBER))?true:false;

	if ((doc->getSettings().FilterOn && !samePoint) || (!doc->getSettings().FilterOn))
		doc->DrawEntity(&s, -2, FALSE);
	else
		return;

   CString buf;
   Untransform(doc, &p, &s);
   s.getData()->getInsert()->setOriginX(p.x);
   s.getData()->getInsert()->setOriginY(p.y);
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_SCALE))->GetWindowText(buf);
   s.getData()->getInsert()->setScale(atof(buf));
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_ROTATION))->GetWindowText(buf);
   s.getData()->getInsert()->setAngle(DegToRad(atof(buf)));
   s.getData()->getInsert()->setMirrorFlags(((CButton*)CurrentSettings->GetDlgItem(IDC_MIRROR))->GetCheck() ^ (int)doc->getBottomView());

   doc->DrawEntity(&s, -2, FALSE);
   cursorFirstDrag = FALSE;
}

void CCEtoODBView::OnLButtonDown_AddInsert(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();

   CString buf;
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   if (!Good_File_Layer())
   {
      ResetButtons();
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   Point2 p;
   p.x = x;
   p.y = y;
   Untransform(doc, &p, &s);
   s.getData()->getInsert()->setOriginX(p.x);
   s.getData()->getInsert()->setOriginY(p.y);

   ((CEdit*)CurrentSettings->GetDlgItem(IDC_SCALE))->GetWindowText(buf);
   s.getData()->getInsert()->setScale(atof(buf));
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_ROTATION))->GetWindowText(buf);
   s.getData()->getInsert()->setAngle(DegToRad(atof(buf)));
   s.getData()->getInsert()->setMirrorFlags(((CButton*)CurrentSettings->GetDlgItem(IDC_MIRROR))->GetCheck() ^ (int)doc->getBottomView());

   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   s.getData()->setLayerIndex(((LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel()))->getLayerIndex());

   //HideSearchCursor();
   cursorMode = Search;
   doc->DrawEntity(&s, 0, FALSE);
   //ShowSearchCursor();

   InsertAddUndo(&s, doc);
}

// AddInsertDlg dialog
AddInsertDlg::AddInsertDlg(CWnd* pParent /*=NULL*/)
   : CResizingDialog(AddInsertDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(AddInsertDlg)
   //}}AFX_DATA_INIT
}

AddInsertDlg::~AddInsertDlg()
{
   if (m_imageList)
      delete m_imageList;
}

void AddInsertDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AddInsertDlg)
   DDX_Control(pDX, IDC_FILECB, m_fileCB);
   DDX_Control(pDX, IDC_TREE, m_tree);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AddInsertDlg, CResizingDialog)
   //{{AFX_MSG_MAP(AddInsertDlg)
   ON_CBN_SELCHANGE(IDC_FILECB, OnSelchangeFilecb)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define iFolder      0
#define iOpenFolder  1
#define iRound       2
#define iBlock       3
#define iFile        4  
#define iCompBlk     5
#define iPadStk      6
#define iPad         7
#define iCompLocal   8
#define iTool        9

BOOL AddInsertDlg::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 10, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_ROUND));
   m_imageList->Add(app->LoadIcon(IDI_BLOCK));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_COMPBLK));
   m_imageList->Add(app->LoadIcon(IDI_PADSTK));
   m_imageList->Add(app->LoadIcon(IDI_PAD));
   m_imageList->Add(app->LoadIcon(IDI_COMPLOCAL));
   m_imageList->Add(app->LoadIcon(IDI_DRILL));
   
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   // fill FileCB
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      int i = m_fileCB.AddString(file->getName());
      m_fileCB.SetItemDataPtr(i, file);
      if (file->getFileNumber() == filenum)
         m_fileCB.SetCurSel(i);
   }

   FillTree();
      
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void AddInsertDlg::FillTree()
{
   m_tree.DeleteAllItems();

   int currentFile = ((FileStruct*)m_fileCB.GetItemDataPtr(m_fileCB.GetCurSel()))->getFileNumber();

   HTREEITEM blockRoot, apRoot, toolRoot, specialRoot;
   CString buf;

   // Blocks
   blockRoot = m_tree.InsertItem("Geometries", iFolder, iOpenFolder);
   m_tree.SetItemData(blockRoot, 0L);
                                  
   // Special Geometries
   specialRoot = m_tree.InsertItem("Special Geometries", iFolder, iOpenFolder);
   m_tree.SetItemData(specialRoot, 0L);

   // Apertures
   apRoot = m_tree.InsertItem("Apertures", iFolder, iOpenFolder);
   m_tree.SetItemData(apRoot, 0L);

   // Tools
   toolRoot = m_tree.InsertItem("Tools", iFolder, iOpenFolder);
   m_tree.SetItemData(toolRoot, 0L);

   for (int i=0;i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getFlags() & BL_FILE)
      {
         if (block->getFileNumber() != currentFile)
            m_tree.SetItemData(m_tree.InsertItem(block->getName(), iFile ,iFile, blockRoot, TVI_SORT), 
                  (DWORD)block);
         continue;
      }

      if (block->getFileNumber() == currentFile || block->getFileNumber() == -1 || block->getFlags() & BL_GLOBAL)
      {
/*       if (block->getFlags() & BL_WIDTH)
            continue;*/

         if (block->getFlags() & BL_SPECIAL)
            m_tree.SetItemData(m_tree.InsertItem(block->getName(), iBlock ,iBlock, specialRoot, TVI_SORT), (DWORD)block);
         else if ((block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL))
            m_tree.SetItemData(m_tree.InsertItem(block->getName(), iTool ,iTool, toolRoot, TVI_SORT), 
                  (DWORD)block);
         else if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE))
            m_tree.SetItemData(m_tree.InsertItem(block->getName(), iRound ,iRound, apRoot, TVI_SORT), 
                  (DWORD)block);
         else
         {
            int icon = iBlock;

            switch (block->getBlockType())
            {
            case BLOCKTYPE_PCBCOMPONENT:
               icon = iCompBlk;
               break;
            case BLOCKTYPE_PADSTACK:
               icon = iPadStk;
               break;
            case BLOCKTYPE_PADSHAPE:
               icon = iPad;
               break;
            case BLOCKTYPE_LOCALPCBCOMP:
               icon = iCompLocal;
               break;
            case BLOCKTYPE_TOOLING:
               icon = iTool;
               break;
            }

            m_tree.SetItemData(m_tree.InsertItem(block->getName(), icon ,icon, blockRoot, TVI_SORT), 
                  (DWORD)block);
         }
      }
   }
}

void AddInsertDlg::OnOK() 
{
   HTREEITEM item;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected
                           
   DWORD data = m_tree.GetItemData(item);
   if (!data) return; // folder selected

   switch (data)
   {
      case 1:
         block = doc->GetCentroidGeometry();
      break;

      case 2:

      break;

      default:
         block = (BlockStruct *)data;
      break;
   }
   
   CResizingDialog::OnOK();
}

void CCEtoODBDoc::OnAddFile() 
{
   StoreDocForImporting();
   static int file = 1;
   FileStruct *fp;
   AddFile dlg;
   dlg.m_file.Format("NewFile_%d", file++);
   if (dlg.DoModal() != IDOK)
      return;
   fp = Graph_File_Start(dlg.m_file, Type_Unknown);
   fp->setNotPlacedYet(false);

   //FillCurrentSettings(this);

   // select this file in Current Settings
   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   int count = CurrentFile->GetCount();
   for (int i=0; i<count; i++)
   {
      if (CurrentFile->GetItemDataPtr(i) == fp)
      {
         CurrentFile->SetCurSel(i);
         break;
      }
   }
}

void CCEtoODBDoc::OnAddLayer() 
{
   AddLayer dlg;
   if (dlg.DoModal() == IDOK)
   {
      StoreDocForImporting();
      int index = Graph_Level(dlg.m_layer, NULL, 0);
      LayerStruct *layer = getLayerArray()[index];
                       
      //UpdateLayers(this);

      // select this layer in Current Settings
      CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
      int count = CurrentLayer->GetCount();
      for (int i=0; i<count; i++)
      {
         if (CurrentLayer->GetItemDataPtr(i) == layer)
         {
            CurrentLayer->SetCurSel(i);
            break;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// AddLayer dialog
AddLayer::AddLayer(CWnd* pParent /*=NULL*/)
   : CResizingDialog(AddLayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(AddLayer)
   m_layer = _T("");
   //}}AFX_DATA_INIT
}

void AddLayer::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AddLayer)
   DDX_Text(pDX, IDC_LAYER, m_layer);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AddLayer, CResizingDialog)
   //{{AFX_MSG_MAP(AddLayer)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL AddLayer::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   static int layernum = 1;
   m_layer.Format("NewLayer_%d", layernum++);
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// end ADD.CPP

/////////////////////////////////////////////////////////////////////////////
// AddFile dialog


AddFile::AddFile(CWnd* pParent /*=NULL*/)
   : CResizingDialog(AddFile::IDD, pParent)
{
   //{{AFX_DATA_INIT(AddFile)
   m_file = _T("");
   //}}AFX_DATA_INIT
}


void AddFile::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AddFile)
   DDX_Text(pDX, IDC_FILE, m_file);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddFile, CResizingDialog)
   //{{AFX_MSG_MAP(AddFile)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddFile message handlers

void AddInsertDlg::OnSelchangeFilecb() 
{
   FillTree();
}

