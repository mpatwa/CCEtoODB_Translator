// $Header: /CAMCAD/4.6/TestAttr2DFT.h 1     1/04/06 6:50p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__TestAttr2DFT_H__)
#define __TestAttr2DFT_H__

#pragma once

#include "Resource.h"
#include "ResizingDialog.h"
#include "horz_lb.h"
#include "afxwin.h"


class CProbeNumberer;




// TestAttr2DftDlg dialog

class TestAttr2DftDlg : public CResizingDialog
{
	DECLARE_DYNAMIC(TestAttr2DftDlg)

public:
	//TestAttr2DftDlg(CWnd* pParent = NULL);   // standard constructor
	TestAttr2DftDlg(CCEtoODBDoc *doc, FileStruct *file, CWnd* pParent = NULL);   // standard constructor
	virtual ~TestAttr2DftDlg();

private:
	CCEtoODBDoc *m_doc;
	FileStruct *m_file;

// Dialog Data
	enum { IDD = IDD_TEST_ATTR_2_DFT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreateDftSoution();

	int m_radioProbeSide;
	CString m_strPageUnits;
	CString m_strProbeSize;
};


class TestAttr2Dft
{
public:
	TestAttr2Dft(CCEtoODBDoc *doc, FileStruct *file, int probeSide, double probeSize);
	~TestAttr2Dft();
	void ProcessInserts();
	void ProcessCompPins();

private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pcbFile;
	FILE *m_logfp;
	ETestSurface m_testSurface;
	double m_probeSize;
	CProbeNumberer *m_probeNamer;

	// PlacePair means place Test Access Point and Test Probe pair

	void PlacePair(CompPinStruct *compPin, CString netName, DataStruct *componentData = NULL);

	void PlacePair(int probeNumber, CPoint2d location, ETestSurface surface, 
		CString netName, CString targetType, int dataLink, double exposedMetalDiameter,
		CString debugStr);

	void PlacePairOnEachPin(DataStruct *componentData); // place on compPins of component

	void PlacePair(DataStruct *componentData); // place directly on component


	CString GetTechnology(CompPinStruct *compPin, DataStruct *componentdata);
	ETestSurface GetSurface(bool bothAllowed, CString testAttrVal, CString technologyAttrVal, bool placedTop);
	DataStruct *FindComponentData(CString refdes);

};


class CProbeNumberer
{

private:
	CCEtoODBDoc *m_doc;
	FileStruct *m_file;
	int m_nextProbeNumber;
	int m_specialNextProbeNumber;
	CMapPtrToPtr m_reservedProbeNumbers;

public:
	CProbeNumberer(CCEtoODBDoc *doc, FileStruct *file);

	void SetProbeNumber(int num)		{ m_specialNextProbeNumber = num; }
	int GetProbeNumber(bool clearSpecial = true);
	bool NumberIsReservedForEntity(int probeNumber, int entityNumber);
	void Report(FILE *fp);

};

#endif