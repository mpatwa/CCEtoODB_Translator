// $Header: /CAMCAD/4.6/Splash.cpp 20    4/19/07 4:07p Rick Faltersack $

// CG: This file was added by the Splash Screen component.
//

#include "stdafx.h"  // e. g. stdafx.h
#include "resource.h"  // e.g. resource.h
#include "Splash.h"  // e.g. splash.h
#include "lic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CSplashWnd::CSplashWnd()
{
   canClose = FALSE;
}

CSplashWnd::~CSplashWnd()
{
   // Clear the static window pointer.
   ASSERT(c_pSplashWnd == this);
   c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
   //{{AFX_MSG_MAP(CSplashWnd)
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
   c_bShowSplashWnd = bEnable;
}                  

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/,CRect* monitorRect /*= NULL*/)
{
   if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
      return;

#ifdef _DEBUG
   return;
#endif

   // Allocate a new splash screen, and create the window.
   c_pSplashWnd = new CSplashWnd;
   if (!c_pSplashWnd->Create(pParentWnd,monitorRect))
      delete c_pSplashWnd;
   else
      c_pSplashWnd->UpdateWindow();

   c_pSplashWnd->canClose = FALSE;
}

void CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
   if (c_pSplashWnd == NULL)
      return;

/* // If we get a keyboard or mouse message, hide the splash screen.
   if (pMsg->message == WM_KEYDOWN ||
       pMsg->message == WM_SYSKEYDOWN ||
       pMsg->message == WM_LBUTTONDOWN ||
       pMsg->message == WM_RBUTTONDOWN ||
       pMsg->message == WM_MBUTTONDOWN ||
       pMsg->message == WM_NCLBUTTONDOWN ||
       pMsg->message == WM_NCRBUTTONDOWN ||
       pMsg->message == WM_NCMBUTTONDOWN)
   {
      c_pSplashWnd->HideSplashScreen();
   } */
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/,CRect* monitorRect /*= NULL*/)
{
   switch (Product)
   {

   case PRODUCT_PROFESSIONAL:
      if (!m_bitmap.LoadBitmap(IDB_MENTOR_SPLASH_PRO))
         return FALSE;
      break;

   case PRODUCT_PCB_TRANSLATOR:
      if (!m_bitmap.LoadBitmap(IDB_MENTOR_SPLASH_PCB))
         return FALSE;
      break;

   case PRODUCT_GRAPHIC:
	   if (!m_bitmap.LoadBitmap(IDB_MENTOR_SPLASH_GRA))
         return FALSE;
      break;
   case PRODUCT_VISION:
	   if (!m_bitmap.LoadBitmap(IDB_MENTOR_SPLASH_VSN))
         return FALSE;
      break;
   default:
     // if (!m_bitmap.LoadBitmap(IDB_MENTOR_SPLASH_SPLASH))
         return true;
   }

   BITMAP bm;
   m_bitmap.GetBitmap(&bm);

   BOOL res = CreateEx(0,
      AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
      NULL, WS_POPUP | WS_VISIBLE/* | WS_EX_TOPMOST*/, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);

   if (monitorRect == NULL)
   {
      SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
   }
   else
   {
      int x = ((monitorRect->left + monitorRect->right )/2) - bm.bmWidth/2;
      int y = ((monitorRect->top  + monitorRect->bottom)/2) - bm.bmHeight/2;

      SetWindowPos(&wndTopMost, x, y, 0, 0, SWP_NOSIZE);
   }

   return res;
}

void CSplashWnd::HideSplashScreen()
{
   // Destroy the window, and update the mainframe.
   DestroyWindow();
   AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
   // Free the C++ class.
   delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   // Center the window.
   CenterWindow();

   // Set a timer (in milliseconds) to destroy the splash screen.
   SetTimer(1, 2500, NULL);

   return 0;
}

void CSplashWnd::OnPaint()
{
   CPaintDC dc(this);

   CDC dcImage;
   if (!dcImage.CreateCompatibleDC(&dc))
      return;

   BITMAP bm;
   m_bitmap.GetBitmap(&bm);

   // Paint the image.
   CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
   dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
   dcImage.SelectObject(pOldBitmap);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
   // If we can close, close it
   if (canClose)
   {
      // Make sure we don't close it again
      canClose = FALSE;
      HideSplashScreen();
   }
   // Or else set that we can close it
   else
      canClose = TRUE;
}

void CSplashWnd::SetClosable()
{
   if (!c_pSplashWnd) return;

   // If we can close, close it
   if (c_pSplashWnd->canClose)
   {
      // Make sure we don't close it again
      c_pSplashWnd->canClose = FALSE;
      c_pSplashWnd->HideSplashScreen();
   }
   // Or else set that we can close it
   else
      c_pSplashWnd->canClose = TRUE;
}
