// $Header: /CAMCAD/4.5/Net_Drv.h 10    4/04/05 6:24p Moustafa Noureddine $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

/////////////////////////////////////////////////////////////////////////////
// DeriveNetlistOptions dialog

class DeriveNetlistOptions : public CDialog
{
// Construction
public:
   DeriveNetlistOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(DeriveNetlistOptions)
   enum { IDD = IDD_DERIVE_NETLIST };
   CString  m_resolution;
   BOOL  m_padstack;
   BOOL  m_singlePin;
   BOOL  m_bmpFills;
   BOOL  m_bmpStart;
   BOOL  m_bmpBW;
   BOOL  m_recursive;
   BOOL  m_renameUnconnected;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DeriveNetlistOptions)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(DeriveNetlistOptions)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPin();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

