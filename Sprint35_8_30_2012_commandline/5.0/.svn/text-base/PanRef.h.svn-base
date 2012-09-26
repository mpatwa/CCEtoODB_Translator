// $Header: /CAMCAD/4.4/PanRef.h 11    1/28/04 6:09p Alvin $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/

#include "ccview.h"
#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// PanToReference dialog
class PanToReference : public CDialog
{
// Construction
public:
   PanToReference(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PanToReference)
   enum { IDD = IDD_PAN_REFERENCE };
   CComboBox   m_refCB;
   CString  m_ref;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PanToReference)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL
   int DoesStringExist(const CString& refName);
// Implementation
   void OnCancel();
protected:

   // Generated message map functions
   //{{AFX_MSG(PanToReference)
   afx_msg void OnFill();
   afx_msg void OnZoomTo();
   afx_msg void OnPanTo();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, CDblRect *rect, int SELECT_COMP);
BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, double *xmin, double *xmax, double *ymin, double *ymax, int SELECT_COMP);
