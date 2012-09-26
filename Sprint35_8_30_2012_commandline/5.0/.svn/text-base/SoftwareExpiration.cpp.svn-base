// $Header: /CAMCAD/4.5/SoftwareExpiration.cpp 5     7/21/06 3:28p Moustafa Noureddine $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "SoftwareExpiration.h"
#include ".\softwareexpiration.h"

// SoftwareExpiraion dialog

IMPLEMENT_DYNAMIC(SoftwareExpiration, CDialog)
SoftwareExpiration::SoftwareExpiration(CWnd* pParent /*=NULL*/)
	: CDialog(SoftwareExpiration::IDD, pParent)
{
}

SoftwareExpiration::SoftwareExpiration(CString &msg, CString &num_days, CWnd* pParent /*=NULL*/)
	: CDialog(SoftwareExpiration::IDD, pParent)
{
	msgStr  = msg;
	daysStr = num_days;
}

SoftwareExpiration::~SoftwareExpiration()
{
}

void SoftwareExpiration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MSG, m_msg);
	DDX_Control(pDX, IDC_STATIC_DAYS, m_days);
	DDX_Control(pDX, IDC_CHECK1, m_DoNotShow);
}


BEGIN_MESSAGE_MAP(SoftwareExpiration, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_STN_CLICKED(IDC_STATIC_DAYS, OnStnClickedStaticDays)
	ON_STN_CLICKED(IDC_STATIC_MSG, OnStnClickedStaticMsg)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()


// SoftwareExpiraion message handlers

BOOL SoftwareExpiration::OnInitDialog()
{
	CDialog::OnInitDialog();

	setupLabels();

	return TRUE;
}

void SoftwareExpiration::setupLabels()
{

   //CFont *pFontDC;
   CFont boldFontDC;
   LOGFONT logfont;

   m_msg.SetWindowText(msgStr);

   logfont.lfWeight = 1200;

   boldFontDC.CreateFontIndirect( &logfont );

   m_days.SetWindowText(daysStr);

   m_days.SetFont(&boldFontDC);

   UpdateData(FALSE);	



}

void SoftwareExpiration::OnBnClickedOk()
{
	OnOK();
}


void SoftwareExpiration::OnStnClickedStaticDays()
{
	// TODO: Add your control notification handler code here
}

void SoftwareExpiration::OnStnClickedStaticMsg()
{
	// TODO: Add your control notification handler code here
}

void SoftwareExpiration::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	int m_autoOpenFlag = (m_DoNotShow.GetCheck() != 0);
	CCEtoODBApp &app = getApp();
	app.WriteProfileInt("Settings", "MaintenanceDialog", m_autoOpenFlag);
		
}
