// $Header: /CAMCAD/4.3/Test_Acc.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "block.h"
#include "ccdoc.h"
                                          
/////////////////////////////////////////////////////////////////////////////
// TestAccessPlacementDefaults dialog
class TestAccessPlacementDefaults : public CDialog
{
// Construction
public:
   TestAccessPlacementDefaults(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TestAccessPlacementDefaults)
   enum { IDD = IDD_TESTACCESS_PLACEMENT_DEFAULTS };
   CListBox m_geometryLB;
   UINT  m_testPreference;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   BlockStruct *testBlock;
   

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TestAccessPlacementDefaults)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillListBox();

   // Generated message map functions
   //{{AFX_MSG(TestAccessPlacementDefaults)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnGenerate();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// TestAccess_Offsets dialog
class TestAccess_Offsets : public CDialog
{
// Construction
public:
   TestAccess_Offsets(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TestAccess_Offsets)
   enum { IDD = IDD_TESTACCESS_OFFSETS };
   CString  m_botX;
   CString  m_botY;
   BOOL  m_getBot;
   BOOL  m_getTop;
   CString  m_topX;
   CString  m_topY;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TestAccess_Offsets)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL
   virtual void OnCancel();

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(TestAccess_Offsets)
   afx_msg void OnApply();
   afx_msg void OnGet();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
