// $Header: /CAMCAD/4.3/Redline.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_REDLINE_H__B98A36A1_B291_11D1_BA40_444553540000__INCLUDED_)
#define AFX_REDLINE_H__B98A36A1_B291_11D1_BA40_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// redline.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LeaderNote dialog

class LeaderNote : public CDialog
{
// Construction
public:
   LeaderNote(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LeaderNote)
   enum { IDD = IDD_ADD_LEADER_NOTE };
   CString  m_note;
   CString  m_height;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LeaderNote)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LeaderNote)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REDLINE_H__B98A36A1_B291_11D1_BA40_444553540000__INCLUDED_)
