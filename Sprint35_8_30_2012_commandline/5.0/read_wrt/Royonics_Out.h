// $Header: /CAMCAD/4.5/read_wrt/Royonics_Out.h 7     5/22/06 2:20p Moustafa Noureddine $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"
#include "General.h"
#define DEFAULT_ORBOTECH_UNITS	unit_o_Mils;

enum ERoyonicsUnits
{
	unitUnknown = -1,
	unit_o_Mils = 0,		// thousandths of an inch
	unit_o_MM = 1,	// millemeter
	unit_o_Inch = 2,
	unit_o_TInch = 3,
	unit_o_TMM = 4,
};

class RoyonicsWrite
{
public:

	RoyonicsWrite(CCEtoODBDoc *document, double l_Scale);
	RoyonicsWrite();
	void GetRoyonicsHeader(CString hdr);
	BOOL Write(CString RoyonicsFile);
	int LoadSettings(CString settingsFileName);
	CString m_PackageName;
	int intFidID;
	CString RoyonicsHDR;
	CStringArray m_stringArray;
	CString m_OrderAtt;
	CString RoyonicsBoardName;
	CString m_SymbAtt;
	CString m_DescAtt;
	CString m_DescCompAtt;
	CString m_TechnologyAtt;
	CString m_TechnologyValue;
private:
	ERoyonicsUnits o_Units;
	CFormatStdioFile royonicsFile;
	
	CCEtoODBDoc *m_pDoc;
	double m_dUnitFactor;
	double p_Scale;
};

#pragma once
// RoyonicsDLG dialog

class RoyonicsDLG : public CDialog
{
	DECLARE_DYNAMIC(RoyonicsDLG)

public:
	RoyonicsDLG(CWnd* pParent = NULL);   // standard constructor
	virtual ~RoyonicsDLG();

// Dialog Data
	enum { IDD = IDD_DIALOG9 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_BoardName;
	CString m_RoyBoard;
};
