// $Header: /CAMCAD/4.6/Redline.cpp 23    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "ccview.h"
#include "cedit.h"
#include "graph.h"
#include "mainfrm.h"
#include "redline.h"
#include "crypt.h"
#include "CCEtoODB.h"

static COLORREF PenColor;
static int PenWidth;

extern CDialogBar *CurrentSettings; // from MAINFRM.CPP

void ResetButtons();
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);

enum Modes{
   Leader,
   Box,
   Cloud,
   Crossout,   
   Sketch,
   Note,
};

static Modes mode;
static FileStruct *file;
static LayerStruct *layer;

/* PenModes *
*/
enum PenModes{
   Small,
   Medium,
   Large,
};
static PenModes penMode;

// Small Pen Button
void CCEtoODBView::OnRlSmall() 
{
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_SMALL);
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_MEDIUM, FALSE);
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_LARGE, FALSE);

   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_REDLINE_SMALL, MF_CHECKED);
      menu->CheckMenuItem(ID_REDLINE_MEDIUM, MF_UNCHECKED);
      menu->CheckMenuItem(ID_REDLINE_LARGE, MF_UNCHECKED);
   }

   penMode = Small;
}

// Medium Pen Button
void CCEtoODBView::OnRlMedium() 
{
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_SMALL, FALSE);
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_MEDIUM);
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_LARGE, FALSE);

   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_REDLINE_SMALL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_REDLINE_MEDIUM, MF_CHECKED);
      menu->CheckMenuItem(ID_REDLINE_LARGE, MF_UNCHECKED);
   }

   penMode = Medium;
}

// Large Pen Button
void CCEtoODBView::OnRlLarge() 
{
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_SMALL, FALSE);
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_MEDIUM, FALSE);
   frame->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_REDLINE_LARGE);

   CMenu *menu = frame->GetMenu();
   if (menu)
   {
      menu->CheckMenuItem(ID_REDLINE_SMALL, MF_UNCHECKED);
      menu->CheckMenuItem(ID_REDLINE_MEDIUM, MF_UNCHECKED);
      menu->CheckMenuItem(ID_REDLINE_LARGE, MF_CHECKED);
   }

   penMode = Large;
}

/* GetRedlinePenIndex
*/
static int GetRedlinePenIndex(CCEtoODBDoc *doc)
{
   switch (penMode)
   { 
   case Medium:
      return Graph_Aperture("Redline Medium", T_ROUND, doc->getSettings().RL_WM, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);
   case Large:
      return Graph_Aperture("Redline Large", T_ROUND, doc->getSettings().RL_WL, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);
   case Small:
   default:
      return Graph_Aperture("Redline Small", T_ROUND, doc->getSettings().RL_WS, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);
   }
}

/* RedliningSetup
*/
BOOL RedliningSetup(CCEtoODBView *view)
{
   CCEtoODBDoc *doc = view->GetDocument();
   doc->StoreDocForImporting();

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);

   file = NULL;
   layer = NULL;
   int sel = CurrentFile->GetCurSel();
   if (sel != CB_ERR)
      file = (FileStruct*)(CurrentFile->GetItemDataPtr(sel));
   sel = CurrentLayer->GetCurSel();
   if (sel != CB_ERR)
      layer = (LayerStruct*)(CurrentLayer->GetItemDataPtr(sel));

   if (!file)
   {
      ErrorMessage("A File must be selected in the Current Settings Dialog before Adding Entities.  Use Add|File.", 
            "See Help File for more info", MB_OK | MB_ICONINFORMATION);
      return FALSE;
   }

   doc->PrepareAddEntity(file);

   if (!layer || layer->getLayerType() != LAYTYPE_REDLINE)
   {
      BOOL NeedNewLayer = TRUE;

      // find first redline layer
      for (int i=0; i<doc->getMaxLayerIndex(); i++)
      {
         if (doc->getLayerArray()[i] == NULL)  continue;

         layer = doc->getLayerArray()[i];
         if (layer->getLayerType() == LAYTYPE_REDLINE)
         {
            //SetCurrentLayer(layer->getLayerIndex());
            NeedNewLayer = FALSE;
            break;
         }
      }

      if (NeedNewLayer)
      {
         layer = doc->Add_Layer("Redline");
         layer->setColor( RGB(255, 0, 0));
         layer->setVisible(true);
         layer->setEditable(true);
         layer->setOriginalColor( RGB(255, 0, 0));
         layer->setOriginalVisible(true);
         layer->setOriginalEditable(true);
         layer->setLayerType(LAYTYPE_REDLINE);
         //FillCurrentSettings(doc);
      }
   }

   view->s.setParentDataList(&(file->getBlock()->getDataList()));
   view->s.scale = file->getScale();
   view->s.insert_x = file->getInsertX();
   view->s.insert_y = file->getInsertY();
   view->s.rotation = file->getRotation();
   view->s.mirror = file->getResultantMirror(doc->getBottomView());

   return TRUE;
}

/* SetupRedlining
*/
int SetupRedlining(CCEtoODBView *view, int redlinemode)
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRedline)) 
   {
      ErrorAccess("You do not have a License to Redline!");
      return 0;
   }*/

   if (view->zoomMode != NoZoom) 
      return 0;

   if (view->cursorMode == Redlining && mode == redlinemode)
   {
      ResetButtons();
      //view->HideSearchCursor();
      view->cursorMode = Search;
      //view->ShowSearchCursor();
      return 0;
   }

   ResetButtons();
   //view->HideSearchCursor();
   view->cursorMode = Redlining;
   //view->ShowSearchCursor();
   view->cursorAnchored = FALSE;
   view->cursorFirstDrag = TRUE;

   RedliningSetup(view);

   return 1;
}

/* OnRlLeader *****************************************************************/
void CCEtoODBView::OnRlLeader() 
{
   if (!SetupRedlining(this, Leader))
      return;

   ((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_RL_LEADER);
   mode = Leader;
}
   
void CCEtoODBView::OnRlBox() 
{
   if (!SetupRedlining(this, Box))
      return;

   ((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_RL_BOX);
   mode = Box;
}

void CCEtoODBView::OnRlCloud() 
{
   if (!SetupRedlining(this, Cloud))
      return;

   ((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_RL_CLOUD);
   mode = Cloud;
}

void CCEtoODBView::OnRlCrossOut() 
{
   if (!SetupRedlining(this, Crossout))
      return;

   ((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_RL_CROSS_OUT);
   mode = Crossout;
}

void CCEtoODBView::OnRlSketch() 
{
   if (!SetupRedlining(this, Sketch))
      return;

   ((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_RL_SKETCH);
   mode = Sketch;
}

void CCEtoODBView::OnRlNote() 
{
   if (!SetupRedlining(this, Note))
      return;

   ((CMainFrame*)AfxGetMainWnd())->m_RedlineToolBar.GetToolBarCtrl().PressButton(ID_RL_NOTE);
   mode = Note;
}

void CCEtoODBView::OnMouseMove_Redlining(CDC *dc, CPoint *pnt, double x, double y)
{
   if (!cursorAnchored) return;

   CCEtoODBDoc *doc = GetDocument();

   if (mode == Sketch)
   {
      Point2 pCur;
      pCur.x = x;
      pCur.y = y;
      Untransform(doc, &pCur, &s);

      Graph_Vertex(pCur.x, pCur.y, 0.0);
      
      if (!cursorFirstDrag)
      {
         //HideSearchCursor();
         HPEN oldPen = (HPEN)SelectObject(dc->m_hDC, CreatePen(PS_SOLID, PenWidth, PenColor));
         dc->MoveTo(cursorPrevPnt.x, cursorPrevPnt.y);
         dc->LineTo(pnt->x, pnt->y);
         DeleteObject(SelectObject(dc->m_hDC, oldPen));
         //ShowSearchCursor();
      }
      cursorFirstDrag = FALSE;
      
      cursorPrevPnt.x = pnt->x;
      cursorPrevPnt.y = pnt->y;
      
      return;
   }

   dc->SetROP2(R2_NOT);
   dc->SelectStockObject(HOLLOW_BRUSH);


   if (!cursorFirstDrag)
   {
      switch (mode)
      {
      case Leader:
         dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
         dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
         break;
      case Box:
         dc->Rectangle(cursorAnchor.x, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);
         break;
      case Cloud:
         dc->Ellipse(cursorAnchor.x, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);
         break;
      case Crossout:
         {
            dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
            dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
            dc->MoveTo(cursorAnchor.x, cursorPrevPnt.y);
            dc->LineTo(cursorPrevPnt.x, cursorAnchor.y);
         }
         break;
      }
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   switch (mode)
   {
   case Leader:
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      break;
   case Box:
      dc->Rectangle(cursorAnchor.x, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);
      break;
   case Cloud:
      dc->Ellipse(cursorAnchor.x, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);
      break;
   case Crossout:
      {
         dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
         dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
         dc->MoveTo(cursorAnchor.x, cursorPrevPnt.y);
         dc->LineTo(cursorPrevPnt.x, cursorAnchor.y);
      }
      break;
   }
}

void CCEtoODBView::OnLButtonDown_Redlining(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   CCamCadData& camCadData = doc->getCamCadData();

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) 
      * ScaleNum;

   if (!RedliningSetup(this))
      return;

   Point2 pCur;
   pCur.x = x;
   pCur.y = y;
   Untransform(doc, &pCur, &s);

   Point2 pAnchor;
   pAnchor.x = anchor_x;
   pAnchor.y = anchor_y;
   Untransform(doc, &pAnchor, &s);

   int widthIndex = GetRedlinePenIndex(doc);

   if (mode == Sketch)
   {
      if (!cursorAnchored)
      {
         cursorAnchored = TRUE;
         cursorFirstDrag = TRUE;
         s.setData(Graph_PolyStruct(layer->getLayerIndex(), 0L, 0));
         Graph_Poly(NULL, widthIndex, 0, 0, 0);
         Graph_Vertex(pCur.x, pCur.y, 0.0);
         InsertAddUndo(&s, doc);
         PenColor = doc->get_layer_color(layer->getLayerIndex(), 0);
         PenWidth = round(doc->getWidthTable()[widthIndex]->getSizeA() / factor);
      }
      else
         cursorAnchored = FALSE;

      return;
   }

   if (!cursorAnchored && mode != Note)
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

   switch (mode)
   {
      case Leader:
      {
         int undolevel = doc->undo_level;

         LeaderNote dlg;
         static double height = doc->getSettings().RL_textHeight;
         dlg.m_height.Format("%lg", height);
         dlg.DoModal();
         height = atof(dlg.m_height);
         if (height < SMALLNUMBER)
            height = doc->getSettings().RL_textHeight;

         dlg.m_note.TrimRight(); // get rid of an carridge return only.
         dlg.m_note.Remove(13);

         if (dlg.m_note.IsEmpty() || height < SMALLNUMBER)
            return;

         BOOL OnRight = x > anchor_x;

         s.setData(Graph_PolyStruct(layer->getLayerIndex(), 0L, 0));
         // leader line
         Graph_Poly(NULL, widthIndex, 0, 0, 0);
         Graph_Vertex(pAnchor.x, pAnchor.y, 0.0);
         Graph_Vertex(pCur.x, pCur.y, 0.0);
         if (OnRight)
            Graph_Vertex(pCur.x + height, pCur.y, 0.0);
         else
            Graph_Vertex(pCur.x - height, pCur.y, 0.0);

         // arrowhead
         double angle = ArcTan2(pCur.y-pAnchor.y, pCur.x-pAnchor.x);
         const double da = DegToRad(20);
         Graph_Poly(NULL, widthIndex, 1, 0, 0);
         Graph_Vertex(pAnchor.x + cos(angle + da) * doc->getSettings().RL_arrowSize, pAnchor.y + sin(angle + da) * doc->getSettings().RL_arrowSize, 0.0);
         Graph_Vertex(pAnchor.x, pAnchor.y, 0.0);
         Graph_Vertex(pAnchor.x + cos(angle - da) * doc->getSettings().RL_arrowSize, pAnchor.y + sin(angle - da) * doc->getSettings().RL_arrowSize, 0.0);
         Graph_Vertex(pAnchor.x + cos(angle + da) * doc->getSettings().RL_arrowSize, pAnchor.y + sin(angle + da) * doc->getSettings().RL_arrowSize, 0.0);
         InsertAddUndo(&s, doc);

         //HideSearchCursor();
         doc->DrawEntity(&s, 0, FALSE);
         //ShowSearchCursor();
         dlg.m_note.TrimLeft();
         dlg.m_note.TrimRight();
      
         double textX;
         if (OnRight)
            textX = pCur.x + height * 1.5;
         else
         {
            int chars = strlen(dlg.m_note);
            int n = dlg.m_note.Find('\n');
            if (n != -1)
               chars = n;

            textX = pCur.x - height * (2 + chars);
         }
         s.setData(Graph_Text(layer->getLayerIndex(), dlg.m_note, textX, pCur.y - height/2, height, height, 0.0, 0L, 1, 0, 0, 0, widthIndex, 0));

         // put text entity in same undolevel as leader arrow
         UndoItemStruct *i = new UndoItemStruct;
         i->DataList = s.getParentDataList();
         i->num = s.getData()->getEntityNumber();

         DataStruct *newData = camCadData.getNewDataStruct(*(s.getData()));
         //memcpy(newData, s.p, sizeof(DataStruct));
         //newData->getAttributesRef() = NULL;
         //doc->CopyAttribs(&newData->getAttributesRef(), s.p->getAttributesRef());
         //newData->getText() = new TextStruct;
         //memcpy(newData->getText(), s.p->getText(), sizeof(TextStruct));
         //newData->getText()->setText(strdup(s.p->getText()->getText()));

         i->data = newData;
         doc->undo[undolevel]->AddTail(i);
      }
      break;
      case Box:
         {
            Point2 p1, p2, p3, p4;
            p1.x = anchor_x;
            p1.y = anchor_y;
            Untransform(doc, &p1, &s);
            p2.x = anchor_x;
            p2.y = y;
            Untransform(doc, &p2, &s);
            p3.x = x;
            p3.y = y;
            Untransform(doc, &p3, &s);
            p4.x = x;
            p4.y = anchor_y;
            Untransform(doc, &p4, &s);

            s.setData(Graph_PolyStruct(layer->getLayerIndex(), 0L, 0));
            Graph_Poly(NULL, widthIndex, 0, 0, 0);
            Graph_Vertex(p1.x, p1.y, 0.0);
            Graph_Vertex(p2.x, p2.y, 0.0);
            Graph_Vertex(p3.x, p3.y, 0.0);
            Graph_Vertex(p4.x, p4.y, 0.0);
            Graph_Vertex(p1.x, p1.y, 0.0);

            InsertAddUndo(&s, doc);
         }
      break;
      case Cloud:
      {
         // erase rubber-banding
         dc->SetROP2(R2_NOT);
         dc->SelectStockObject(HOLLOW_BRUSH);
         dc->Ellipse(cursorAnchor.x, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);

         double x1, x2, x3, x4, y1, y2, y3, y4;
         x1 = min(anchor_x, x);
         x4 = max(anchor_x, x);
         x2 = (x1 + x4) / 2 - (x4 - x1) / 4.83;
         x3 = (x1 + x4) / 2 + (x4 - x1) / 4.83;
         y1 = min(anchor_y, y);
         y4 = max(anchor_y, y);
         y2 = (y1 + y4) / 2 - (y4 - y1) / 4.83;
         y3 = (y1 + y4) / 2 + (y4 - y1) / 4.83;

         s.setData(Graph_PolyStruct(layer->getLayerIndex(), 0L, 0));
         Graph_Poly(NULL, widthIndex, 0, 0, 0);

         double bulge = s.mirror ? -1 : 1;

         Point2 p;
         p.x = x1;
         p.y = y2;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x2;
         p.y = y1;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x3;
         p.y = y1;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x4;
         p.y = y2;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x4;
         p.y = y3;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x3;
         p.y = y4;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x2;
         p.y = y4;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x1;
         p.y = y3;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         p.x = x1;
         p.y = y2;
         Untransform(doc, &p, &s);
         Graph_Vertex(p.x, p.y, bulge);

         InsertAddUndo(&s, doc);
      }
      break;
      case Crossout:
      {
         Point2 p1, p2, p3, p4;
         p1.x = anchor_x;
         p1.y = anchor_y;
         Untransform(doc, &p1, &s);
         p2.x = anchor_x;
         p2.y = y;
         Untransform(doc, &p2, &s);
         p3.x = x;
         p3.y = y;
         Untransform(doc, &p3, &s);
         p4.x = x;
         p4.y = anchor_y;
         Untransform(doc, &p4, &s);

         s.setData(Graph_PolyStruct(layer->getLayerIndex(), 0L, 0));
         Graph_Poly(NULL, widthIndex, 0, 0, 0);
         Graph_Vertex(p1.x, p1.y, 0.0);
         Graph_Vertex(p3.x, p3.y, 0.0);
         Graph_Poly(NULL, widthIndex, 0, 0, 0);
         Graph_Vertex(p2.x, p2.y, 0.0);
         Graph_Vertex(p4.x, p4.y, 0.0);

         InsertAddUndo(&s, doc);
      }
      break;
      case Note:
      {
         LeaderNote dlg;
         dlg.m_height.Format("%lg", doc->getSettings().RL_textHeight);
         dlg.DoModal();
         double height = atof(dlg.m_height);
         if (dlg.m_note.IsEmpty() || height < SMALLNUMBER)
            return;

         dlg.m_note.TrimLeft();
         dlg.m_note.TrimRight();
         s.setData(Graph_Text(layer->getLayerIndex(), dlg.m_note, pCur.x, pCur.y, height, height, 0.0, 0L, 1, 0, 0, 0, widthIndex, 0));

         InsertAddUndo(&s, doc);
      }
      break;

      default:
      return;
   };

   //HideSearchCursor();
   doc->DrawEntity(&s, 0, FALSE);
   //ShowSearchCursor();
}

/////////////////////////////////////////////////////////////////////////////
// LeaderNote dialog
LeaderNote::LeaderNote(CWnd* pParent /*=NULL*/)
   : CDialog(LeaderNote::IDD, pParent)
{
   //{{AFX_DATA_INIT(LeaderNote)
   m_note = _T("");
   m_height = _T("");
   //}}AFX_DATA_INIT
}

void LeaderNote::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LeaderNote)
   DDX_Text(pDX, IDC_EDIT1, m_note);
   DDX_Text(pDX, IDC_EDIT2, m_height);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LeaderNote, CDialog)
   //{{AFX_MSG_MAP(LeaderNote)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// end REDLINE.CPP
