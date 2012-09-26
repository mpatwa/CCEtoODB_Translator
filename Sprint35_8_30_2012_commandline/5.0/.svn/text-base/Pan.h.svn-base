// $Header: /CAMCAD/4.3/Pan.h 8     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "ResizingDialog.h"

class CCEtoODBDoc;

////////////////////////////////////////////////////////////////////////////
// PanToTextDialog
class PanToTextDialog : public CDialog
{
// Construction
public:
   PanToTextDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PanToText)
   enum { IDD = IDD_PAN_TO_TEXT };
   BOOL  m_case;
   CString  m_text;
   BOOL  m_substring;
   BOOL  m_attribs;
   BOOL  m_text_ent;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PanToText)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PanToText)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// PanToCoordDialog
class PanToCoordDialog : public CDialog
{
// Construction
public:
   PanToCoordDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PanToCoord)
   enum { IDD = IDD_PAN_COORD };
   double   m_x;
   double   m_y;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PanToCoord)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PanToCoord)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// PanToInsertDialog
class PanToInsertDialog : public CResizingDialog
{
// Construction
public:
   PanToInsertDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PanToInsert)
   enum { IDD = IDD_PAN_TO_INSERT };
   CComboBox   m_fileCB;
   CListBox m_inserttypeLB;
   CListBox m_insertLB;
   CString  m_count;
   //}}AFX_DATA
   CCEtoODBDoc *doc;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PanToInsert)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("PanToInsertDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PanToInsert)
   afx_msg void OnPanTo();
   afx_msg void OnSelChange();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
