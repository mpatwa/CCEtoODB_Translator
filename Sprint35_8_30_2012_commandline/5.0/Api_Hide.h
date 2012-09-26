// $Header: /CAMCAD/4.3/Api_Hide.h 8     8/12/03 9:05p Kurt Van Ness $

#if !defined(AFX_API_HIDE_H__F03360ED_531F_4C3D_BB43_534F00260D34__INCLUDED_)
#define AFX_API_HIDE_H__F03360ED_531F_4C3D_BB43_534F00260D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// HideCAMCADDlg dialog
class HideCAMCADDlg : public CDialog
{
// Construction
public:
   HideCAMCADDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(HideCAMCADDlg)
   enum { IDD = IDD_HIDE_CAMCAD };
   CProgressCtrl  m_progress;
   CString  m_message;
   CString  m_company;
   CString  m_serialNum;
   CString  m_user;
   CString  m_version;
   //}}AFX_DATA
   UINT timerID;

   void UpdateLicInfo();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HideCAMCADDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
   void OnCancel();
   void OnOK();

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(HideCAMCADDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnTimer(UINT nIDEvent);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_API_HIDE_H__F03360ED_531F_4C3D_BB43_534F00260D34__INCLUDED_)
