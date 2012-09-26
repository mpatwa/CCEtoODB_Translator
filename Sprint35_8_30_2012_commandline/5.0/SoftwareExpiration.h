// $Header: /CAMCAD/4.5/SoftwareExpiration.h 4     7/21/06 3:28p Moustafa Noureddine $

#pragma once

#include "afxwin.h"

// SoftwareExpiration dialog

class SoftwareExpiration : public CDialog
{
	DECLARE_DYNAMIC(SoftwareExpiration)

public:
	SoftwareExpiration(CWnd* pParent = NULL);   // standard constructor

	SoftwareExpiration(CString &msg, CString &num_days, CWnd* pParent = NULL); //overloaded constructor
	virtual ~SoftwareExpiration();

// Dialog Data
	enum { IDD = IDD_DIALOG_LICENSE_DISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
   
   void setupLabels();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_msg;
	CStatic m_days;
	CString msgStr, daysStr;
	CButton m_checkBox;
	CButton m_DoNotShow;
	afx_msg void OnBnClickedOk();
	afx_msg void OnStnClickedStaticDays();
	afx_msg void OnStnClickedStaticMsg();
	afx_msg void OnBnClickedCheck1();
	//CButton m_DoNotShow;
};
