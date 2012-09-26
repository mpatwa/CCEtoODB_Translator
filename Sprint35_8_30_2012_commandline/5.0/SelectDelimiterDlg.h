// $Header:

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// SelectDelimiterDlg.h

#if ! defined (__SelectDelimiterDlg_h__)
#define __SelectDelimiterDlg_h__

#pragma once

#include "resource.h"
#include "afxwin.h"


/////////////////////////////////////////////////////////////////////////////
// CSelectDelimiterDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CSelectDelimiterDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectDelimiterDlg)

public:
	CSelectDelimiterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectDelimiterDlg();
	enum { IDD = IDD_SELECT_DELIMITER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_optDelimiter;
	
	CString m_IgnoreLines;
	CString m_otherDelimiter;
	CString m_HeaderStartsLine;

public:
	char GetDelimiter();
	
	int GetHeaderStartsLine();
	int IgnoreLines();
	afx_msg void OnBnClickedOk();
private:
public:
	afx_msg void OnBnClickedRadioComma();
	
	afx_msg void OnBnClickedCheck1();
	CEdit edit_IgnoreLines;
	afx_msg void OnEnChangeEditDelimiter();
};


#endif
