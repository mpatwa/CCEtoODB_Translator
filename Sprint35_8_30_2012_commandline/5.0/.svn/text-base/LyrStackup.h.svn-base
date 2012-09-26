// $Header: /CAMCAD/4.3/LyrStackup.h 8     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "ResizingDialog.h"

class CCEtoODBDoc;

/////////////////////////////////////////////////////////////////////////////
// LayerStackup dialog
class LayerStackup : public CResizingDialog
{
// Construction
public:
   LayerStackup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LayerStackup)
   enum { IDD = IDD_LAYER_STACKUP };
   CListBox m_list;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   int maxStackup;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerStackup)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("LayerStackupDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LayerStackup)
   virtual BOOL OnInitDialog();
   afx_msg void OnApply();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

