// $Header: /CAMCAD/4.4/Pcb_Grid.h 13    3/08/04 1:54a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "resource.h"

class CCEtoODBDoc;
class FileStruct;

/////////////////////////////////////////////////////////////////////////////
// PCB_Grid dialog
class PCB_Grid : public CDialog
{
// Construction
public:
   PCB_Grid(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PCB_Grid)
   enum { IDD = IDD_PCB_GRID };
   CString  m_board;
   CString  m_size;
   CString  m_hSizeString;
   int      m_horz;
   CString  m_vSizeString;
   int      m_vert;
   int      m_hSteps;
   int      m_vSteps;
   CString  m_gridThicknessString;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PCB_Grid)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

public:
   void LoadGridSettings(CString fileName, BOOL warnFileOpen);
   void rectifyValues();
   double getHSizePageUnits();
   double getVSizePageUnits();
   void setHSizePageUnits(double pageUnits);
   void setVSizePageUnits(double pageUnits);
   CString getHSizePageUnitsString();
   CString getVSizePageUnitsString();

// Implementation
public:
   CCEtoODBDoc *doc;
   FileStruct *pcbFile;
   double left;
   double right;
   double bottom;
   double top;
   double sizeX;
   double sizeY;

private:
   int m_pageUnits;
   double m_hSizePageUnits;
   double m_vSizePageUnits;
   double m_gridThickness;

protected:

   // Generated message map functions
   //{{AFX_MSG(PCB_Grid)
   virtual void OnOK();
   afx_msg void OnSave();
   afx_msg void OnLoad();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHStepsButton();
   afx_msg void OnBnClickedVStepsButton();
   afx_msg void OnBnClickedHSizeButton();
   afx_msg void OnBnClickedVSizeButton();
   afx_msg void OnEnKillfocusHSteps();
   afx_msg void OnEnKillfocusVSteps();
   afx_msg void OnEnKillfocusHSize();
   afx_msg void OnEnKillfocusVSize();
};
