// $Header: /CAMCAD/4.3/GpcbDlgs.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_GERBIN_H__2CB0DD02_32C4_11D2_9B3C_00400538DF91__INCLUDED_)
#define AFX_GERBIN_H__2CB0DD02_32C4_11D2_9B3C_00400538DF91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CreatePadstacks dialog
class CreatePadstacks : public CDialog
{
// Construction
public:
   CreatePadstacks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CreatePadstacks)
   enum { IDD = IDD_CREATE_PADSTACKS };
   CComboBox   m_layerCB;
   CString  m_tolerance;
   int      m_apertures;
   int      m_drill;
   int      m_layer;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CreatePadstacks)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CreatePadstacks)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_GERBIN_H__2CB0DD02_32C4_11D2_9B3C_00400538DF91__INCLUDED_)
