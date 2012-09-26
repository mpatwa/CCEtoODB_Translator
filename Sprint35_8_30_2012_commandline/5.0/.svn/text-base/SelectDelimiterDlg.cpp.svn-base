// $Header:

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// SelectDelimiterDlg.cpp


#include "stdafx.h"
#include "SelectDelimiterDlg.h"
#include ".\selectdelimiterdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CSelectDelimiterDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSelectDelimiterDlg, CDialog)
CSelectDelimiterDlg::CSelectDelimiterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDelimiterDlg::IDD, pParent)
	, m_optDelimiter(FALSE)
	, m_otherDelimiter(_T(""))
	, m_IgnoreLines(_T("2"))
	, m_HeaderStartsLine(_T("1"))
{
}

CSelectDelimiterDlg::~CSelectDelimiterDlg()
{
}

void CSelectDelimiterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_COMMA, m_optDelimiter);
	DDX_Text(pDX, IDC_EDIT_DELIMITER, m_otherDelimiter);
	
	DDX_Text(pDX, IDC_EDIT_DELIMITER2, m_IgnoreLines);
	DDX_Text(pDX, IDC_HEADERSTARTS, m_HeaderStartsLine);
}

BEGIN_MESSAGE_MAP(CSelectDelimiterDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_COMMA, OnBnClickedRadioComma)
	ON_BN_CLICKED(IDC_RADIO_PIPE, OnBnClickedRadioComma)
	ON_BN_CLICKED(IDC_RADIO_SPACE, OnBnClickedRadioComma)
	ON_BN_CLICKED(IDC_RADIO_SEMICOLON, OnBnClickedRadioComma)
	ON_BN_CLICKED(IDC_RADIO_TAB, OnBnClickedRadioComma)
	ON_BN_CLICKED(IDC_RADIO_OTHERS, OnBnClickedRadioComma)

	//ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT_DELIMITER, OnEnChangeEditDelimiter)
END_MESSAGE_MAP()

char CSelectDelimiterDlg::GetDelimiter()
{
	switch (m_optDelimiter)
	{
	case 0:			return ',';
	case 1:			return '|';
	case 2:			return ' ';
	case 3:			return ';';
	case 4:			return '\t';
	case 5:			return m_otherDelimiter.GetAt(0);
	default:			return '\0';
	}
}
//BOOL CSelectDelimiterDlg::IgnoreHeader ()
//{
//	if (m_IgnoreHeader == 1)
//		return true;
//
//	return false;
//}
int CSelectDelimiterDlg::IgnoreLines()
{
	CString txtVal = m_IgnoreLines.GetString();
	int intVal = atoi((LPCTSTR)txtVal); 
	return intVal;  //m_IgnoreLines.GetAt(0);
}
int CSelectDelimiterDlg::GetHeaderStartsLine()
{
	CString txtVal = m_HeaderStartsLine.GetString();
	int intVal = atoi((LPCTSTR)txtVal); 
	return intVal; 
}
void CSelectDelimiterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_otherDelimiter.Trim();
	m_IgnoreLines.Trim();

	OnOK();
}

void CSelectDelimiterDlg::OnBnClickedRadioComma()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_optDelimiter == 5)	// other delimiter
		GetDlgItem(IDC_EDIT_DELIMITER)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_DELIMITER)->EnableWindow(FALSE);
}

//void CSelectDelimiterDlg::OnBnClickedCheck1()
//{
//	m_IgnoreHeader = chk_IgnoreHeader.GetCheck();
//	// TODO: Add your control notification handler code here
//}

void CSelectDelimiterDlg::OnEnChangeEditDelimiter()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
