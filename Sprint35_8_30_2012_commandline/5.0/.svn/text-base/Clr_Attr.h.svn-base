// $Header: /CAMCAD/4.3/Clr_Attr.h 9     8/27/03 5:40p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

class CCEtoODBDoc;

/////////////////////////////////////////////////////////////////////////////
// ColorByAttrib dialog
class ColorByAttrib : public CDialog
{
// Construction
public:
   ColorByAttrib(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ColorByAttrib)
   enum { IDD = IDD_COLOR_BY_ATTRIB };
   CListBox m_valueLB;
   CListBox m_keywordLB;
   //}}AFX_DATA

   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ColorByAttrib)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillAttribKeywords(CAttributes* map, BOOL IsData, void *ptr);
   void FillGeometryNames(DataStruct *data);

   // Generated message map functions
   //{{AFX_MSG(ColorByAttrib)
   virtual BOOL OnInitDialog();
   afx_msg void OnColor();
   afx_msg void OnUncolor();
   afx_msg void OnClear();
   afx_msg void OnSelchangeKeywordLb();
   virtual void OnCancel();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

