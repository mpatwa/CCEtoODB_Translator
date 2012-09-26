// $Header: /CAMCAD/4.4/Gauge.cpp 11    10/11/04 6:05p Tom Shelley $

// CG: This file was added by the Progress Dialog component

#include "stdafx.h"
#include "resource.h"
#include "gauge.h"
#include "CCEtoODB.h"  // for getApp()

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

CProgressDlg::CProgressDlg(CString Caption, BOOL showGauge)
{
   ShowGauge = showGauge;  // Show scroll slider and percentage, or not (not about overall window)
   caption = Caption;
   m_nLower=0;
   m_nUpper=100;
   m_nStep=10;
    //{{AFX_DATA_INIT(CProgressDlg)
   m_percent = _T("");
   m_status = _T("");
   //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
}

CProgressDlg::~CProgressDlg()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CProgressDlg::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CProgressDlg::OnCancel()
{
}

BOOL CProgressDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    this->ShowWindow( getApp().SilentRunning ? SW_HIDE : SW_SHOW );

    // Center it
    if (this->GetParent() != NULL)
    {
       CRect progressDlgRect;
       this->GetWindowRect(progressDlgRect);
       CRect parentRect;
       this->GetParent()->GetWindowRect(parentRect);
       int xloc = parentRect.CenterPoint().x - (progressDlgRect.Size().cx / 2);
       int yloc = parentRect.CenterPoint().y - (progressDlgRect.Size().cy / 2);
       this->SetWindowPos(NULL, xloc, yloc, progressDlgRect.Size().cx, progressDlgRect.Size().cy, 0 /*flags*/);
    }

    m_Progress.SetRange((short)m_nLower,(short)m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);

    SetWindowText(caption);

    // ShowGauge is about the scroller and numeric items, not
    // about the window overall. Might want to not ShowGauge
    // but still want window to show (e.g. to show status message)

    if (ShowGauge)
    {
       GetDlgItem(CG_IDC_PROGDLG_PROGRESS)->ShowWindow(SW_SHOW);
       GetDlgItem(CG_IDC_PROGDLG_PERCENT)->ShowWindow(SW_SHOW);
    }
    else
    {
       GetDlgItem(CG_IDC_PROGDLG_PROGRESS)->ShowWindow(SW_HIDE);
       GetDlgItem(CG_IDC_PROGDLG_PERCENT)->ShowWindow(SW_HIDE);
    }

    return TRUE;  
}

void CProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CProgressDlg::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressDlg)
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
   DDX_Text(pDX, CG_IDC_PROGDLG_PERCENT, m_percent);
   DDX_Text(pDX, CG_IDC_PROGDLG_STATUS, m_status);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CProgressDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProgressDlg::SetStatus(LPCTSTR lpszMessage)
{
    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(CG_IDC_PROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);
    pWndStatus->SetWindowText(lpszMessage);
}

void CProgressDlg::SetRange(int nLower,int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange(nLower,nUpper);
}
  
void CProgressDlg::SetRange32(long nLower,long nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange32(nLower,nUpper);
}
int CProgressDlg::SetPos(long nPos)
{
   if (nPos == m_Progress.GetPos())
      return nPos;

   PumpMessages();
   int iResult = m_Progress.SetPos(nPos);
   UpdatePercent(nPos);
   return iResult;
}

int CProgressDlg::SetStep(long nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_Progress.SetStep(nStep);
}

int CProgressDlg::OffsetPos(long nPos)
{
   if (!nPos)
      return 0;

   PumpMessages();
   int iResult = m_Progress.OffsetPos(nPos);
   UpdatePercent(iResult+nPos);
   return iResult;
}

int CProgressDlg::StepIt()
{
   PumpMessages();
   int iResult = m_Progress.StepIt();
   UpdatePercent(iResult+m_nStep);
   return iResult;
}

void CProgressDlg::PumpMessages()
{
   // Must call Create() before using the dialog
   ASSERT(m_hWnd!=NULL);

   MSG msg;
   // Handle dialog messages
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      if(!IsDialogMessage(&msg))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);  
      }
   }
}

void CProgressDlg::UpdatePercent(int nNewPos)
{
   CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);
   double nPercent;
    
   double nDivisor = m_nUpper - m_nLower;
   ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

   double nDividend = (nNewPos - m_nLower);
   ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

	/* CASE 1070:  Old Code
   nPercent = nDividend * 100 / nDivisor;

	Since Dividend can be a large number, we end up with an overflow on the int32 value
	when multiplying by 100.  So, we convert the equation to do only division and make all
	variables doubles to allow for the potential of large number values.
	*/
	nPercent = (double)nDividend / ((double)nDivisor / 100.0);

   // Since the Progress Control wraps, we will wrap the percentage
   // along with it. However, don't reset 100% back to 0%
   while(nPercent > 100.0)
      nPercent /= 100;
	if (nPercent < 0.0)
		nPercent = 0.0;

   // Display the percentage
   CString strBuf;
   strBuf.Format(_T("%3.0f%c"),nPercent,_T('%'));

   CString strCur; // get current percentage
   pWndPercent->GetWindowText(strCur);

   if (strCur != strBuf)
      pWndPercent->SetWindowText(strBuf);
}
    
