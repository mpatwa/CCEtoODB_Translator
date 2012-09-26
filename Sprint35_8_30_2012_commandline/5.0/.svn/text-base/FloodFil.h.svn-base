// $Header: /CAMCAD/4.3/FloodFil.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

int FF_Start(CCEtoODBDoc *Doc, double resolution);
void FF_End();
void FF_Draw(FileStruct *file, int layer, CStringList *layerNetnamesList);
void FF_DrawBlock(BlockStruct *block, double insert_x, double insert_y, double rotation, double scale, int mirror, 
      BOOL negative, CString insertNetName, CString compName, int insertLayer, int drawLayer, BOOL IgnoreLayerNetnames);
void FF_DrawSelected(CCEtoODBDoc *doc);
void FF_ShowBitmap(CString text = "", CDC *dc = NULL);
int FF_Fill(double x, double y, BOOL NegativeLayer);
BOOL FF_Query(double x, double y);

/////////////////////////////////////////////////////////////////////////////
// DisplayDlg dialog
class DisplayDlg : public CDialog
{
// Construction
public:
   DisplayDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(DisplayDlg)
   enum { IDD = IDD_DISPLAY };
   BOOL  m_fills;
   BOOL  m_starts;
   BOOL  m_oneToOne;
   BOOL  m_animate;
   CString  m_text;
   UINT  m_milliSecs;
   //}}AFX_DATA
   int pageWidth, pageHeight, lineWidth, lineHeight;
   UINT timerID;
   CDC *dc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DisplayDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(DisplayDlg)
   afx_msg void OnPaint();
   virtual BOOL OnInitDialog();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnToggle();
   afx_msg void OnCancel();
   afx_msg void OnAnimate();
   afx_msg void OnTimer(UINT nIDEvent);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

