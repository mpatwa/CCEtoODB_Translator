// $Header: /CAMCAD/4.5/ToolbarM.h 19    12/22/05 7:50p Kurt Van Ness $

#if !defined(__ToolBarM_h__)
#define __ToolBarM_h__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar window
class CMyToolBar : public CToolBar
{
private:
   CFrameWnd& m_parentFrame;
   CArray<int,int> m_commandIds;
   bool m_showState;
   DWORD m_barStyle;

// Construction
public:
   CMyToolBar(CFrameWnd& parentFrame);
   virtual ~CMyToolBar();

// Attributes
public:

// Operations
public:
   afx_msg void OnDropDown(NMHDR *pNotifyStruct, LRESULT *result);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMyToolBar)
   //}}AFX_VIRTUAL

private:
   void saveCommandIds();
   void restoreCommandIds();

// Implementation
public:
   virtual void enable(bool enableFlag=true);
   virtual void disable();
   virtual void hide();
   virtual void unhide();
   virtual void hideButton(int id);

   // Generated message map functions
protected:
   //{{AFX_MSG(CMyToolBar)
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// ArrangeToolbars dialog

class ArrangeToolbars : public CDialog
{
// Construction
public:
   ArrangeToolbars(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ArrangeToolbars)
   enum { IDD = IDD_TOOLBARS };
   BOOL  m_add;
   BOOL  m_currentSettings;
   BOOL  m_navigator;
   BOOL  m_custom;
   BOOL  m_custom2;
   BOOL  m_dft;
   BOOL  m_edit;
   BOOL  m_filter;
   BOOL  m_gerberEducator;
   BOOL  m_geometry;
   BOOL  m_gc;
   BOOL  m_it;
   BOOL  m_join;
   BOOL  m_main;
   BOOL  m_ogp;
   BOOL  m_pcb;
   BOOL  m_poly;
   BOOL  m_query;
   BOOL  m_schematic;
   BOOL  m_sequence;
   BOOL  m_snap;
   BOOL  m_view;
   BOOL  m_worldView;
   BOOL  m_redline;
   BOOL  m_tools;
   
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ArrangeToolbars)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ArrangeToolbars)
   afx_msg void OnApply();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__ToolBarM_h__)
