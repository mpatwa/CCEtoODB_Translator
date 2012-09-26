// $Header: /CAMCAD/4.3/Coords.h 7     8/12/03 9:05p Kurt Van Ness $

#if !defined(AFX_COORDS_H__32A9DD06_D422_4932_B310_34576B854690__INCLUDED_)
#define AFX_COORDS_H__32A9DD06_D422_4932_B310_34576B854690__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccview.h"

/////////////////////////////////////////////////////////////////////////////
// CoordsDlg dialog
class CoordsDlg : public CDialog
{
// Construction
public:
   CoordsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CoordsDlg)
   enum { IDD = IDD_COORDS };
   CString  m_x;
   CString  m_y;
   int      m_absRel;
   //}}AFX_DATA
   CCEtoODBView *view;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CoordsDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CoordsDlg)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COORDS_H__32A9DD06_D422_4932_B310_34576B854690__INCLUDED_)
