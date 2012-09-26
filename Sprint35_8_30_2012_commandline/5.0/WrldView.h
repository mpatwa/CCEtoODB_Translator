// $Header: /CAMCAD/4.3/WrldView.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_WRLDVIEW_H__2A7239A4_7D6A_11D2_9866_004005408E44__INCLUDED_)
#define AFX_WRLDVIEW_H__2A7239A4_7D6A_11D2_9866_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccdoc.h"

void LoadWorldViewState();
void SaveWorldViewState();

/////////////////////////////////////////////////////////////////////////////
// WorldView dialog
class WorldView : public CDialog
{
// Construction
public:
   WorldView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(WorldView)
   enum { IDD = IDD_WORLD_VIEW };
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA
   double scaleFactor;
   CCEtoODBDoc *doc;
   CPoint anchor, previous;
   BOOL Anchored;
   HCURSOR hCursor;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(WorldView)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
   void Draw(CDC *dc);

protected:
   void OnCancel();
   void PrepareDC(CDC *dc, CCEtoODBDoc *doc);

   // Generated message map functions
   //{{AFX_MSG(WorldView)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRLDVIEW_H__2A7239A4_7D6A_11D2_9866_004005408E44__INCLUDED_)
