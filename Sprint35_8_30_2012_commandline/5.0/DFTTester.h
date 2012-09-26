// $Header: /CAMCAD/4.3/DFTTester.h 4     8/16/03 1:34a Kurt Van Ness $

#pragma once

#include "resource.h"
#include "DFTFacade.h"
#include "afxwin.h"

#if !defined(DisableDftModule)

class CDFTFacade;

// CDFTTester dialog

class CDFTTester : public CDialog
{
   DECLARE_DYNAMIC(CDFTTester)

   CDFTFacade *pFacade;

public:
   CDFTTester( CDFTFacade *pParent = NULL);   // standard constructor
   virtual ~CDFTTester();

// Dialog Data
   enum { IDD = IDD_DFTTESTER };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedValidatePcb();
   afx_msg void OnBnClickedButton2();
   afx_msg void OnBnClickedAssignTp();
   afx_msg void OnBnClickedTestPlan();
   afx_msg void OnBnClickedEditAttributes();
   afx_msg void OnLbnSelchangeShowOutput();
   CListBox Output;
};

#endif

