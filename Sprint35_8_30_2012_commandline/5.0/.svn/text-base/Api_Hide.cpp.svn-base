// $Header: /CAMCAD/4.6/Api_Hide.cpp 13    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "StdAfx.h"
#include "General.h"
#include "api_hide.h"
#include "api.h"
#include "lic.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CString user, company; // from LICENSE.CPP

HideCAMCADDlg *hideCAMCADDlg = NULL;

/******************************************************************************
* HideCAMCAD
*/
short API::HideCAMCAD(BOOL Hide) 
{
   if (Hide)
   {
      if (hideCAMCADDlg)
         return RC_SUCCESS;

      hideCAMCADDlg = new HideCAMCADDlg;

      AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);

      hideCAMCADDlg->Create(IDD_HIDE_CAMCAD);
      hideCAMCADDlg->ShowWindow(SW_SHOW);
      hideCAMCADDlg->SetWindowPos(&hideCAMCADDlg->wndTopMost, 0, 0, 1, 1, SWP_NOMOVE | SWP_NOSIZE);

      FlushQueue();
   }
   else
   {
      if (!hideCAMCADDlg)
         return RC_SUCCESS;

      AfxGetMainWnd()->ShowWindow(SW_RESTORE);

      hideCAMCADDlg->DestroyWindow();
      delete hideCAMCADDlg;
      hideCAMCADDlg = NULL;
   }

   return RC_SUCCESS;
}

/******************************************************************************
* SplashMessage
*/
short API::SplashMessage(LPCTSTR message) 
{
   if (!hideCAMCADDlg)
      return RC_GENERAL_ERROR;

   hideCAMCADDlg->m_message = message;

   hideCAMCADDlg->UpdateData(FALSE);

   return RC_SUCCESS;
}

/******************************************************************************
* SplashProgressRange
*/
short API::SplashProgressRange(short range) 
{
   if (!hideCAMCADDlg)
      return RC_GENERAL_ERROR;

   hideCAMCADDlg->m_progress.SetRange(0, range);

   return RC_SUCCESS;
}

/******************************************************************************
* SplashProgressPosition
*/
short API::SplashProgressPosition(short position) 
{
   if (!hideCAMCADDlg)
      return RC_GENERAL_ERROR;

   hideCAMCADDlg->m_progress.SetPos(position);

   return RC_SUCCESS;
}

/******************************************************************************
* SplashMin
*/
short API::SplashMin() 
{
   if (!hideCAMCADDlg)
      return RC_GENERAL_ERROR;

   WINDOWPLACEMENT wp;

   hideCAMCADDlg->GetWindowPlacement(&wp);

   wp.showCmd = SW_SHOWNORMAL;

   int height = wp.rcNormalPosition.top - wp.rcNormalPosition.bottom;

   wp.rcNormalPosition.bottom += round(0.72 * height);

   hideCAMCADDlg->SetWindowPlacement(&wp);   

   return RC_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// HideCAMCADDlg dialog
HideCAMCADDlg::HideCAMCADDlg(CWnd* pParent /*=NULL*/)
   : CDialog(HideCAMCADDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(HideCAMCADDlg)
   m_message = _T("");
   m_company = _T("");
   m_serialNum = _T("");
   m_user = _T("");
   m_version = _T("");
   //}}AFX_DATA_INIT
}

void HideCAMCADDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(HideCAMCADDlg)
   DDX_Control(pDX, IDC_PROGRESS, m_progress);
   DDX_Text(pDX, IDC_MESSAGE, m_message);
   DDX_Text(pDX, IDC_COMPANY, m_company);
   DDX_Text(pDX, IDC_SERIALNUM, m_serialNum);
   DDX_Text(pDX, IDC_USER, m_user);
   DDX_Text(pDX, IDC_VERSION, m_version);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(HideCAMCADDlg, CDialog)
   //{{AFX_MSG_MAP(HideCAMCADDlg)
   ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HideCAMCADDlg message handlers
BOOL HideCAMCADDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   UpdateLicInfo();

   timerID = SetTimer(2, 2000, 0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void HideCAMCADDlg::OnCancel()
{
   return;
}

void HideCAMCADDlg::OnOK()
{
   return;
}

void HideCAMCADDlg::OnTimer(UINT nIDEvent) 
{
   SetWindowPos(&wndTopMost, 0, 0, 1, 1, SWP_NOMOVE | SWP_NOSIZE);
   
   CDialog::OnTimer(nIDEvent);
}

void HideCAMCADDlg::UpdateLicInfo()
{
   m_version = getApp().getCamCadSubtitle();
   m_user = getApp().getCamcadLicense().getUser();
   m_company = getApp().getCamcadLicense().getCompany();

   CString names = getApp().getCamcadLicense().getUser();
   names += getApp().getCamcadLicense().getCompany();
   if (names.IsEmpty())
      return;

   unsigned long serialNum = hash(names);
   m_serialNum.Format("%ld", serialNum);

   UpdateData(FALSE);
}
