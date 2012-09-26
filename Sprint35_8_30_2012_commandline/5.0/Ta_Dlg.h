// $Header: /CAMCAD/4.5/Ta_Dlg.h 11    1/27/05 7:43p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "GeneralDialog.h"
#include <afxtempl.h>

// this is for vias and other padstacks from the datastruct
typedef struct // use in short analysis
{
   CString        netname;
   long           entitynumber;
   double         x,y,rot;
   int            mirror;                 // is entity mirrored
   int            aperturepadstackindex;  // index into the aperture padstack
   int            layer;                  // 1 top , 2 bottom, 3 all
} TA_PadstackKoo;
typedef CTypedPtrArray<CPtrArray, TA_PadstackKoo*> PadstackKooArray;

class CCEtoODBDoc;
class FileStruct;

/////////////////////////////////////////////////////////////////////////////
// TestabilityAnalyzer dialog
class TestabilityAnalyzer : public CDialog
{
// Construction
public:
   TestabilityAnalyzer(CWnd* pParent = NULL);   // standard constructor
   ~TestabilityAnalyzer();

// Dialog Data
   //{{AFX_DATA(TestabilityAnalyzer)
   enum { IDD = IDD_TESTABILITY_ANALYZER };
   //}}AFX_DATA

   CCEtoODBDoc *doc;
   FileStruct *pcbFile;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TestabilityAnalyzer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   int InsertItem(LV_ITEM *item);

   // Generated message map functions
   //{{AFX_MSG(TestabilityAnalyzer)
   afx_msg void OnClearAccess();
   afx_msg void OnClearProbe();
   afx_msg void OnClearShort();
   afx_msg void OnSave();
   afx_msg void OnLoad();
   afx_msg void OnRunAccess();
   afx_msg void OnRunShort();
   afx_msg void OnAttribInstance();
   afx_msg void OnMoveAccess();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
