// $Header: /CAMCAD/4.3/PcbCheck.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_PCBCHECK_H__6393F253_8BD5_46E3_AF58_0FB3C66D23B7__INCLUDED_)
#define AFX_PCBCHECK_H__6393F253_8BD5_46E3_AF58_0FB3C66D23B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// ValidatePCBdlg dialog

class ValidatePCBdlg : public CDialog
{
// Construction
public:
   ValidatePCBdlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ValidatePCBdlg)
   enum { IDD = IDD_VALIDATE_PCB_DATA };
   CListBox m_list;
   int      m_action;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ValidatePCBdlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ValidatePCBdlg)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnSelect();
   afx_msg void OnUnselect();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCBCHECK_H__6393F253_8BD5_46E3_AF58_0FB3C66D23B7__INCLUDED_)
