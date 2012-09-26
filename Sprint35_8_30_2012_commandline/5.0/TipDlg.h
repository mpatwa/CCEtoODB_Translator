// $Header: /CAMCAD/4.5/TipDlg.h 10    11/29/04 9:39p Kurt Van Ness $

// CG: This file added by 'Tip of the Day' component.

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

#if !defined(__TipDlg_H__)
#define __TipDlg_H__

#pragma once

class CTipDlg : public CDialog
{
// Construction
public:
   CTipDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
   //{{AFX_DATA(CTipDlg)
   // enum { IDD = IDD_TIP };
   BOOL  m_bStartup;
   CString  m_strTip;
   //}}AFX_DATA

   FILE* m_pStream;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTipDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CTipDlg();

protected:
   // Generated message map functions
   //{{AFX_MSG(CTipDlg)
   afx_msg void OnNextTip();
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   virtual void OnOK();
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void GetNextTipString(CString& strNext);
};

#endif

