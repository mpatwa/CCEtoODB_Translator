// $Header: /CAMCAD/4.3/ChildFrm.cpp 3     8/12/03 3:29p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "childfrm.h"

// CChildFrame

extern BOOL LockDocuments; // from API.CPP

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
   ON_WM_SIZE()
   ON_WM_MOVE()
END_MESSAGE_MAP()

// CChildFrame message handlers

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
   CMDIChildWnd::OnSize(nType, cx, cy);

   if (!LockDocuments)
      GetWindowPlacement(&lastWindowPlacement);
   else
      SetWindowPlacement(&lastWindowPlacement);
}

void CChildFrame::OnMove(int x, int y)
{
   CMDIChildWnd::OnMove(x, y);

   WINDOWPLACEMENT wndPlacement;
   GetWindowPlacement(&wndPlacement);

   int curWidth = wndPlacement.rcNormalPosition.right - wndPlacement.rcNormalPosition.left;
   int curHeight = wndPlacement.rcNormalPosition.bottom - wndPlacement.rcNormalPosition.top;
   int prevWidth = lastWindowPlacement.rcNormalPosition.right - lastWindowPlacement.rcNormalPosition.left;
   int prevHeight = lastWindowPlacement.rcNormalPosition.bottom - lastWindowPlacement.rcNormalPosition.top;

   if (LockDocuments && curWidth == prevWidth && curHeight == prevHeight)
      GetWindowPlacement(&lastWindowPlacement);
   else if (!LockDocuments)
      GetWindowPlacement(&lastWindowPlacement);
}
