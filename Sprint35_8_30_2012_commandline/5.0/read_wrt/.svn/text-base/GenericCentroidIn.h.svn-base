// $Header: /CAMCAD/4.5/read_wrt/GenericCentroidIn.h 5     12/05/05 11:53p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GenericCentroidIn.h

#if ! defined (__GenericCentroidIn_h__)
#define __GenericCentroidIn_h__

#pragma once

#include "afxwin.h"
#include "FlexGridLib.h"
#include "CamCadDatabase.h"
#include "ResizingDialog.h"

enum ESurface
{
	surfaceTop,
	surfaceBottom,
	surfaceBoth,
};

enum ECreateGeomType
{
   eCentroidsOnly = 0,
   ePcbComponents = 1,
};

//-----------------------------------------------------------------------------
// CGenericCentroidImportDlg dialog
//-----------------------------------------------------------------------------
class CGenericCentroidImportDlg : public CResizingDialog
{
private:
	CCamCadDatabase m_camCadDatabase;
	CFlexGridStatic m_centroidGrid;
	CMapStringToInt m_columnNameToIndexMap;
	CString m_fileName;
	char m_delimiter;
	int m_headerLineNumber;
	int m_dataStartLineNumber;
	bool m_initialized;
	bool m_rejected;

	CComboBox m_refdesCombo;
	CComboBox m_partnumberCombo;
	CComboBox m_xCoordinateCombo;
	CComboBox m_yCoordinateCombo;
	CComboBox m_rotationCombo;
	CComboBox m_surfaceCombo;
	CComboBox m_topCombo;
	CComboBox m_bottomCombo;
	CComboBox m_unitsCombo;

	CEdit m_topEdit;
	CEdit m_bottomEdit;

	BOOL m_surfaceOption;
   int m_createGeomTypeOption;

public:
	CGenericCentroidImportDlg(CCEtoODBDoc& camcadDoc, const CString fileName , const char delimiter, int headerLine, int dataStartLine);
	virtual ~CGenericCentroidImportDlg();

	enum { IDD = IDD_FormatCentroidIn };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void initGrid();
	void loadGrid();
	void loadFieldNameToCombo();

   CExtendedFlexGrid& getFlexGrid()				{ return *(m_centroidGrid.getGrid());	}
	FileStruct* getCentroidFile();
	bool areAllFieldsAssigned();

   BlockStruct *GetDefinedPcbComponent(CString geometryName);

public:
	virtual BOOL OnInitDialog();

	DECLARE_EVENTSINK_MAP()
	void AfterEditGrid(long Row, long Col);
	void BeforeEditGrid(long Row, long Col, VARIANT_BOOL* Cancel);
	void BeforeMouseDownGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel);
	void BeforeSortGrid(long col, short* order);
	void DoubleClickGrid();
	void EnterCellGrid();
	void ValidateEditGrid(long Row, long Col, VARIANT_BOOL* Cancel);

	afx_msg void OnBnClickedSurfaceOption();
	afx_msg void OnCbnSelchangeSurfaceCombo();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedReject();
   afx_msg void OnBnClickedCreateGeomTypeOption();

public:
	bool isImportRejected() const { return m_rejected; }
};


#endif