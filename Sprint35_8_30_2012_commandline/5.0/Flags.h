// $Header: /CAMCAD/4.3/Flags.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#if !defined(AFX_FLAGS_H__6C296A31_91E5_4DC8_9D18_FAE2018B0772__INCLUDED_)
#define AFX_FLAGS_H__6C296A31_91E5_4DC8_9D18_FAE2018B0772__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Flags.h : header file
//

enum FlagGroup
{
   Data,
   Block,
};

/////////////////////////////////////////////////////////////////////////////
// FlagsDlg dialog
class FlagsDlg : public CDialog
{
// Construction
public:
   FlagsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FlagsDlg)
   enum { IDD = IDD_FLAGS };
   CListBox m_list;
   //}}AFX_DATA
   FlagGroup group;
   void *item;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FlagsDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillList();
protected:

   // Generated message map functions
   //{{AFX_MSG(FlagsDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeList1();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLAGS_H__6C296A31_91E5_4DC8_9D18_FAE2018B0772__INCLUDED_)
