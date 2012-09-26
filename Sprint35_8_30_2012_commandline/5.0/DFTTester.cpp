// $Header: /CAMCAD/4.3/DFTTester.cpp 6     8/16/03 1:34a Kurt Van Ness $

#include "stdafx.h"
#include "DFTTester.h"

// CDFTTester dialog

#if !defined(DisableDftModule)

IMPLEMENT_DYNAMIC(CDFTTester, CDialog)

CDFTTester::CDFTTester(CDFTFacade* pParent )
   : CDialog(CDFTTester::IDD, pParent)
{
   pFacade = pParent;
}

CDFTTester::~CDFTTester()
{
}

void CDFTTester::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SHOW_OUTPUT, Output);
}

BEGIN_MESSAGE_MAP(CDFTTester, CDialog)
   ON_BN_CLICKED(IDC_VALIDATE_PCB, OnBnClickedValidatePcb)
   ON_BN_CLICKED(IDC_ASSIGN_TP, OnBnClickedAssignTp)
   ON_BN_CLICKED(IDC_TEST_PLAN, OnBnClickedTestPlan)
   ON_BN_CLICKED(IDC_EDIT_ATTRIBUTES, OnBnClickedEditAttributes)
   ON_LBN_SELCHANGE(IDC_SHOW_OUTPUT, OnLbnSelchangeShowOutput)
END_MESSAGE_MAP()

// CDFTTester message handlers

void CDFTTester::OnBnClickedValidatePcb()
{
   pFacade->OnDFTValidatePcb();
}


void CDFTTester::OnBnClickedAssignTp()
{
   pFacade->OnDFTOptions();
}

void CDFTTester::OnBnClickedTestPlan()
{
   pFacade->OnDFTTestPlan();
}

void CDFTTester::OnBnClickedEditAttributes()
{
   pFacade->OnEditAttributes("");
}

void CDFTTester::OnLbnSelchangeShowOutput()
{
}

#endif
