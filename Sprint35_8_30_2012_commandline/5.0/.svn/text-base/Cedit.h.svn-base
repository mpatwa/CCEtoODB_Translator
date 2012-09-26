// $Header: /CAMCAD/4.5/Cedit.h 10    11/18/05 6:06p Kurt Van Ness $

/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/           

#pragma once

#include "ccdoc.h"
#include "afxwin.h"

//void FillCurrentSettings(CCEtoODBDoc *doc);
//void SetCurrentFile(int num);
//void SetCurrentLayer(int num);
//void SetCurrentWidth(BlockStruct *b);
//void SetCurrentScale(double s);
//void SetCurrentRotation(double r);
//void SetCurrentTextHeight(double h);
//void SetCurrentMirror(BOOL m);

/////////////////////////////////////////////////////////////////////////////
// CSDialogBar dialog
class CSDialogBar : public CDialogBar
{
   DECLARE_DYNAMIC(CSDialogBar)
public:
   CSDialogBar();

   CString hint;

protected:
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// SelectLayerDlg dialog
class SelectLayerDlg : public CDialog
{
// Construction
public:
   SelectLayerDlg(UINT nIDTemplate = IDD_SELECT_LAYER, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(SelectLayerDlg)
   enum { IDD = IDD_SELECT_LAYER };
   CComboBox   m_layerCB;
   BOOL m_copy;
   //}}AFX_DATA

   CCEtoODBDoc *doc;
   int layer;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectLayerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

public:
   bool getCopy() const { return m_copy != 0; }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SelectLayerDlg)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
};

