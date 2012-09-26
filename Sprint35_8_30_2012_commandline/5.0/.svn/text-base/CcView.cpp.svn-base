// $Header: /CAMCAD/4.6/CcView.cpp 76    4/18/07 7:18p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-8. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCview.h"
#include "resource.h"
#include "cedit.h"
#include <math.h>
#include "mainfrm.h"
#include "add.h"
#include "measure.h"
#include <float.h>
#include "edit.h"
#include "wrldview.h" 
#include "coords.h"
#include "response.h"
#include "comppin.h"
#include "Sch_List.h"
#include "GerberEducatorToolbar.h"
#include "CCEtoODB.h"
#include "Draw.h"
#include ".\ccview.h"
#include "SelectStackDialog.h"
#include "Status.h"
#include "ViewSynchronizer.h"
//#include "Debug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern WorldView *worldView;
extern CCEtoODBView *mouseView; // from CAMCAD.CPP
extern CCEtoODBView *apiView; // from API.CPP

bool MarkLast = false;
CPoint rightClickPnt;
CView *activeView = NULL;

CCEtoODBView* getActiveView()
{
   return (CCEtoODBView*)activeView;
}

static int childCount = 0;


/////////////////////////////////////////////////////////////////////////////
// CCEtoODBView
IMPLEMENT_DYNCREATE(CCEtoODBView, CView)

BEGIN_MESSAGE_MAP(CCEtoODBView, CView)
END_MESSAGE_MAP()

/******************************************************************************
* CCEtoODBView
*/
CCEtoODBView::CCEtoODBView()
: m_rotationAngleEditBar(NULL)
{
   ScaleNum = 1;
   ScaleDenom = 1.0;
   HideCursor = FALSE;
   zoomMode = NoZoom;
   cursorMode = Search;
   lastCommand = LC_Nothing;
   textFilePos = NULL;
   FilterFirst = TRUE;
   FirstDrag = TRUE;
   Drawing = FALSE;
   ThumbTracking = FALSE;
   MovingDatas = FALSE;
   DrawVariablesInitialized = FALSE;
   schListDlg = NULL;
   showSchNav = FALSE;
   lastScaleNum = -1;
	m_beginUpdateRequests = 0;
   m_contextMenuEnabled = true;
   m_measure = NULL;
	lastRightClickX = 0;
	lastRightClickY = 0;
}

/* Destructor *****************************************************************/
CCEtoODBView::~CCEtoODBView()
{
   if (mouseView == this)
      mouseView = NULL;
   getApp().getViewSynchronizer().unregisterView(this);
}

/* Create *********************************************************************/
BOOL CCEtoODBView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
   dwStyle |= WS_HSCROLL | WS_VSCROLL;
   return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

#define BREAK_GRID_RATE    100

void CCEtoODBView::OnDraw(CDC* pDC)
{
}

CCEtoODBDoc* CCEtoODBView::GetDocument()
{
	return (CCEtoODBDoc*)m_pDocument;
}


void CCEtoODBView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
   CView *lastActiveView = activeView;

   CCEtoODBDoc *doc = GetDocument();

   if (bActivate)
   {
      activeView = pActivateView;
      apiView = (CCEtoODBView*)activeView;
	  CView::OnActivateView(bActivate, pActivateView, pDeactiveView);


      /*if (worldView)
         worldView->Invalidate();*/

      /*CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
      frame->getNavigator().setDoc(((CCEtoODBView*)pActivateView)->GetDocument());*/
   }

   /*if (bActivate && pActivateView == this && lastActiveView != this) // if activating this view and it was not already active
   {
      WINDOWPLACEMENT wndPlacement;

      /*if (schListDlg && schListDlg->m_hWnd)
         schListDlg->GetWindowPlacement(&wndPlacement);*/

      /*if (lastActiveView)
      {
         CCEtoODBView *ccView = (CCEtoODBView*)lastActiveView;
         CCEtoODBDoc *ccDoc = (CCEtoODBDoc*)ccView->GetDocument();

         //ccDoc->OnDoneEditing();

         /*if (ccView->schListDlg && ccView->schListDlg->m_hWnd)
         {
            ccView->schListDlg->ShowWindow(SW_HIDE);
            ccView->schListDlg->GetWindowPlacement(&wndPlacement);
         }
      }*/

      /*if (showSchNav && schListDlg && schListDlg->m_hWnd)
      {
         schListDlg->ShowWindow(SW_SHOW);
         schListDlg->SetWindowPlacement(&wndPlacement);
      }*/

      /*FillCurrentSettings(doc); // fill with this file list and other doc info
      // pick first file if there is a file
      SetCurrentFile(-1);       //       
      SetCurrentWidth(doc->getWidthTable()[0]);*/

		/*CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
		frame->getNavigator().setDoc(((CCEtoODBView*)pActivateView)->GetDocument());

      cursorMode = Search;
      zoomMode = NoZoom;
      ResetButtons();
      CloseDRCList();*/

      //CloseSchematicList();
      /*if (CPdlg)
         CPdlg->OnCancel();
      if (selectNetsDlg)
         selectNetsDlg->OnCancel();

      /*POSITION pos = doc->GetDocumentPosition();
      CString buf;
      buf.Format("%lu", pos);
      SendResponse("Document Active", buf);
   }*/
}

