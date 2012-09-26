// $Header: /CAMCAD/4.3/Status.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

// STATUS.H

#ifndef _INCLUDE_PROGRESSBAR_H_
#define _INCLUDE_PROGRESSBAR_H_

/////////////////////////////////////////////////////////////////////////////
// CProgressBar -  status bar progress control
//
class CProgressBar: public CProgressCtrl
// Creates a ProgressBar in the status bar
{
public:
    CProgressBar();
    CProgressBar(LPCTSTR strMessage, int nSize=100, int MaxValue=100, BOOL bSmooth=FALSE);
    ~CProgressBar();
    BOOL Create(LPCTSTR strMessage, int nSize=100, int MaxValue=100, BOOL bSmooth=FALSE);

    DECLARE_DYNCREATE(CProgressBar)

// operations
public:
    BOOL SetRange(int nLower, int nUpper, int nStep = 1);
    BOOL SetText(LPCTSTR strMessage);
    BOOL SetSize(int nSize);
    COLORREF SetBarColour(COLORREF clrBar);
    COLORREF SetBkColour(COLORREF clrBk);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  SetStep(int nStep);
    int  StepIt();
    void Clear();

// Overrides
    //{{AFX_VIRTUAL(CProgressBar)
    //}}AFX_VIRTUAL

// implementation
protected:
    int       m_nSize;        // Percentage size of control
    CString   m_strMessage;   // Message to display to left of control
    CRect     m_Rect;         // Dimensions of the whole thing

    CStatusBar *GetStatusBar();
    BOOL Resize();

// Generated message map functions
protected:
    //{{AFX_MSG(CProgressBar)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEditBar -  status bar edit control
//
class CEditBar: public CEdit
// Creates an Edit Ctrl in the status bar
{
public:
    CEditBar();
    CEditBar(LPCTSTR strMessage, int nSize=100);
    ~CEditBar();
    BOOL Create(LPCTSTR strMessage, int nSize=100);

    DECLARE_DYNCREATE(CEditBar)

// operations
public:
    BOOL SetText(LPCTSTR strMessage);
    BOOL SetSize(int nSize);
    void Clear();

// Overrides
    //{{AFX_VIRTUAL(CEditBar)
    //}}AFX_VIRTUAL

// implementation
protected:
    int       m_nSize;        // Percentage size of control
    CString   m_strMessage;   // Message to display to left of control
    CRect     m_Rect;         // Dimensions of the whole thing

    CStatusBar *GetStatusBar();
    BOOL Resize();

// Generated message map functions
protected:
    //{{AFX_MSG(CEditBar)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif

// end STATUS.H