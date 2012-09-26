// $Header: /CAMCAD/4.5/read_wrt/Asymtek_Out.h 5     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"

enum EOutputFormatType
{
	formatPaste = 0,
	formatGlue = 1,
};

////////////////////////////////////////////////////////////////////////////////
// CDialogAsymtek dialog
////////////////////////////////////////////////////////////////////////////////
class CDialogAsymtek : public CDialog
{
	DECLARE_DYNAMIC(CDialogAsymtek)

public:
	CDialogAsymtek(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogAsymtek();
	enum { IDD = IDD_FORMAT_ASYMTEK_OUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CString m_txtHeaderDirectory;
	CString m_txtOutputFileName;
	BOOL m_optOutputFormat;

	void WriteDirectoryToRegistry(CString directoryPath);
	CString GetDirectoryFromRegistry();

public:
	CString GetHeaderDirectory()				{ return m_txtHeaderDirectory;									};
	CString GetOutputFileName()				{ return m_txtOutputFileName;										};	
	EOutputFormatType GetOutputFormat()		{ return (m_optOutputFormat==0)?formatPaste:formatGlue;	};
	
	afx_msg void OnBnClickedButtonBrowse();
	virtual BOOL OnInitDialog();
};


/******************************************************************************
* AsymtekWrite
*/
class AsymtekWrite
{
public:
	AsymtekWrite(CCEtoODBDoc &doc);
	~AsymtekWrite();

private:
	CCEtoODBDoc &m_pDoc;
	CFormatStdioFile m_logFile;
	int m_displayErr;
	double m_unitFactor;

	bool AllCompHasCentriod(BlockStruct *block);
	int readAndCopyHeaderFile(CString headerFileName, CFormatStdioFile &topFile, CFormatStdioFile &botFile, CString &asymtekUnit);
	bool copyAVWFile(CString avwFileName, CString outAvwFileName);
	void setUnitFactor(FormatStruct *format, int pageUnit, CString asymtekUnit);
	void writePanelDesign(CFormatStdioFile &topFile, CFormatStdioFile &botFile, EOutputFormatType outputFormat, FileStruct *panel);
	void writeBoardDesign(CFormatStdioFile &topFile, CFormatStdioFile &botFile, EOutputFormatType outputFormat,	BlockStruct *block,
				double x, double y, double scale, double rotation, int mirror);
	void writeSMDdata(CFormatStdioFile &topFile, CFormatStdioFile &botFile, DataStruct *data, CString partNumber,
				CString packageName, CString refdes, Point2 point, bool mirror);

public:
	int WriteFile(CString pathName, CString logFileName, FormatStruct *format, int pageUnit);
};

