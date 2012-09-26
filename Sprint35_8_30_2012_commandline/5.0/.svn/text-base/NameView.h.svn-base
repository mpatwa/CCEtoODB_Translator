// $Header: /CAMCAD/5.0/NameView.h 19    12/10/06 3:56p Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#if !defined(__NameView_h__)
#define __NameView_h__

#pragma once

#include <afxtempl.h>
#include "ccview.h"
#include "resource.h"
#include "layer.h"
#include "ResizingDialog.h"
#include "TypedContainer.h"
#include "DcaNamedView.h"

//#if defined(__CcDoc_h__)
//#error CcDoc.h included in __FILE__
//#endif

class CCEtoODBDoc; 

//_____________________________________________________________________________
/////////////////////////////////////////////////////////////////////////////
// NamedViewDlg dialog
class NamedViewDlg : public CResizingDialog
{
// Construction
public:
   NamedViewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(NamedViewDlg)
   enum { IDD = IDD_LIST_NAMED_VIEWS };
   CListBox m_listLB;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   BOOL Expanded;
   int width;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(NamedViewDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("NamedViewDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(NamedViewDlg)
   afx_msg void OnRecall();
   afx_msg void OnSave();
   virtual BOOL OnInitDialog();
   afx_msg void OnExpand();
   afx_msg void OnDelete();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// end NAMEVIEW.H
#endif
