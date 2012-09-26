// $Header: /CAMCAD/4.5/PcbUtil2.h 13    2/14/07 8:19p Rick Faltersack $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright ?1994-97. All Rights Reserved.
*/           

#if !defined(AFX_PCBUTIL2_H__585A1601_D79A_11D2_9866_004005408E44__INCLUDED_)
#define AFX_PCBUTIL2_H__585A1601_D79A_11D2_9866_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// pcbutil2.h : header file
//

#include "RegularExpression.h"

/////////////////////////////////////////////////////////////////////////////
// ManufacturingRefDes dialog
class ManufacturingRefDes : public CDialog
{
// Construction
public:
   ManufacturingRefDes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ManufacturingRefDes)
   enum { IDD = IDD_MANUFACTURING_REF_DES };
   CString  m_max;
   CString  m_min;
   CString  m_bottom;
   CString  m_top;
   BOOL     m_neverMirrorComps;
   BOOL     m_neverMirrorProbes;
   BOOL     m_applyToComponents;
   BOOL     m_applyToProbes;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ManufacturingRefDes)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ManufacturingRefDes)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   virtual BOOL OnInitDialog();

public:
   afx_msg void OnBnClickedChkComponents();
   afx_msg void OnBnClickedChkProbes();
};

/////////////////////////////////////////////////////////////////////////////
// AttribRefresh dialog

class AttribRefresh : public CDialog
{
// Construction
public:
   AttribRefresh(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(AttribRefresh)
   enum { IDD = IDD_ATTRIB_REFRESH };
   BOOL  m_typetocomp;
   int      m_method;
   BOOL  m_geomtocomp;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AttribRefresh)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AttribRefresh)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CTypeChanger
{
   // Class handles the .MAKE_INTO_xxxx commands for .in files.
   // Currently supporting .MAKE_INTO_TESTPOINT and .MAKE_INTO_FIDUCIAL.

private:
   CRegularExpressionList m_testpoints;
   CRegularExpressionList m_fiducials;
   CRegularExpressionList m_tooling;

   void CleanUp() { m_testpoints.empty(); m_fiducials.empty(); m_tooling.empty();}
   void Add(const CString& regularExpressionString, CRegularExpressionList &list);
   bool Apply(CCEtoODBDoc *doc, BlockStruct *block);
   void Apply(CCEtoODBDoc *doc, DataStruct *data, BlockStruct *insertedblock, CRegularExpressionList &expressionlist, InsertTypeTag insertType);

   bool HasExpressionsToApply();

public:
   CTypeChanger()  { CleanUp(); }
   ~CTypeChanger() { CleanUp(); }

   void Reset()   { CleanUp(); }

   bool ParseCommand(CString commandLine);         // Returns true if recognized command, otherwise false.
   bool Apply(CCEtoODBDoc *doc, FileStruct *file);  // Apply mapping to inserts in file.
   bool Apply(CCEtoODBDoc *doc);                    // Apply mapping to inserts in all files in doc.
};

////////////////////////////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCBUTIL2_H__585A1601_D79A_11D2_9866_004005408E44__INCLUDED_)
