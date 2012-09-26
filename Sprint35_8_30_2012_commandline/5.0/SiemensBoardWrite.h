// $Header: /CAMCAD/4.6/SiemensBoardWrite.h 3     5/10/07 10:29a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"
#include "General.h"

class SiemensBoardWrite
{
public:
	
	SiemensBoardWrite(CCEtoODBDoc *document, double l_Scale);
	SiemensBoardWrite();
	BOOL Write(CString BoardFile);
	int LoadSettings(CString settingsFile);
	int ProcessPanel();
	CString m_PackageName;
	
private:
	
	CFormatStdioFile BoardFile;
	CCEtoODBDoc *m_pDoc;
	double pinPitch;
};

class SiemensBoardDlg : public CDialog
{
	DECLARE_DYNAMIC(SiemensBoardDlg)

private:
	
public:
	SiemensBoardDlg(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, CWnd* pParent = NULL);   // standard constructor
	virtual ~SiemensBoardDlg();
	
// Dialog Data
	enum { IDD = IDD_DIALOG8 };

private:
   FileStruct& m_fileStruct;
   CCEtoODBDoc& m_camcadDoc;

   void LoadSettingFromAttribute();
   void SaveSettingToAttribute();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	CString m_ProgNr;
	CString m_Product;
	CString m_Version;
};
