// $Header: /CAMCAD/4.5/GerberEducatorReviewInserts.h 29    1/22/07 11:46a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__GerberEducatorReviewInserts_h__)
#define __GerberEducatorReviewInserts_h__

#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "FlexGridLib.h"
#include "GerberEducatorToolbar.h"
#include "GerberEducatorReviewGeometries.h"
#include "GerberEducatorUi.h"
#include "DialogToolbar.h"
#include "GerberEducator.h"

#define CONST_REVIEW					"Reviewed"
#define CONST_REFDES					"Refdes"
#define CONST_TYPE					"Type"
#define CONST_ROTATION				"  Rotation  "
#define CONST_SURFACE				"  Surface  "
#define CONST_GEOMETRYNAME			"Geometry Name"
#define CONST_VIEWTOP				"Top"
#define CONST_VIEWBOTTOM			"Bottom"
#define CONST_VIEWBOTH				"Both"

//-----------------------------------------------------------------------------
// CGEReviewInsertsDlg dialog
//-----------------------------------------------------------------------------
class CGEReviewInsertsDlg : public CResizingDialog
{
	//DECLARE_DYNAMIC(CGEReviewInsertsDlg)

private:
	const static int m_colDegrees				=  0;
	const static int m_colSortableRefDes	=  1;
	const static int m_colReview				=  2;
	const static int m_colRefdes				=  3;
	const static int m_colType					=  4;
	const static int m_colRotation			=  5;
	const static int m_colSurface				=  6;
	const static int m_colGeometryName		=  7;
   const static int m_colCount				=	8;

	const static int m_viewTop					=  0;
	const static int m_viewBottom				=  1;
	const static int m_viewBoth				=  2;

	CGerberEducatorUi& m_gerberEducatorUi;
	CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;
	FileStruct& m_fileStruct;
   DataStruct* m_selectedDataBeforeSort;
	CGerberEducatorComponents m_gerberEducatorComponents;
   CGerberEducatorColorScheme m_layerColor;

	CFlexGridStatic m_insertGrid;
	CSliderCtrl m_zoomSlider;
   CResizingDialogToolBar m_toolBar;

	// Summary table
	CStaticLabel m_geomTypeLabel;
	CStaticLabel m_pcbCompLabel;
	CStaticLabel m_mechCompLabel;
	CStaticLabel m_toolingLabel;
	CStaticLabel m_fiducialLabel;
	CStaticLabel m_reviewQtyLabel;
	CStaticLabel m_topReviewQtyLabel;
	CStaticLabel m_bottomReviewQtyLabel;

	CReviewQtySummary m_topReviewQuantitySummary;
	CReviewQtySummary m_bottomReviewQuantitySummary;

	int m_curZoomMargin;
   int m_maxZoomMargin;
	BOOL m_viewOption;
	BOOL m_turnOnPinLabel;
	bool m_initialized;
	bool m_pinLabelTopWasOn;
	bool m_pinLabelBottomWasOn;
	CellEditErrorTag m_cellEditError;

public:
	CGEReviewInsertsDlg(CGerberEducatorUi& gerberEducatorUi);
	virtual ~CGEReviewInsertsDlg();
	enum { IDD = IDD_GE_ReviewInserts };

   virtual CString GetDialogProfileEntry() { return "GEReviewInsertsDlg"; }

private:
	virtual BOOL OnInitDialog();
	
	void initGrid();
	void loadGrid();
	void setView();

	bool isReviewed(DataStruct* data);
   CString getAllowDegrees(DataStruct* data);
	bool checkDuplicateRefdes(int editRow, CString newRefdes);

	DataStruct* getSelectedData();
	DataStruct* getDataByRow(long row);
	DataStruct* getSelectedDataFromCAMCAD();
   CExtendedFlexGrid& getFlexGrid()				{ return *(m_insertGrid.getGrid());	}
	void setComponentSelectionInGrid(DataStruct* componentData, long col);

	void updateComppinRefdes(CString oldRefdes, CString newRefdes);
	void updateComppinLocation(DataStruct* data);
	void addComppin(DataStruct* data);
	void removeCompPin(DataStruct* data);

	void updateRotationOfComppin(DataStruct* data, bool updateMirrored = false);
	void updateReviewStatusToInsertData();
	void setReviewedDataColor(DataStruct* data, long row, bool reviewed);
	void drawData(DataStruct* data, int state);

	void loadDialogSettings();
	void saveDialogSettings();
	void turnOnPinLabel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

public:
	void updateSelectedEntity();
	bool isInitialized()								{ return m_initialized;					}

	DECLARE_EVENTSINK_MAP()
	void OnClickInsertGrid();
	void KeyDownEditInsertGrid(long Row, long Col, short * KeyCode, short Shift);
	void AfterRowColChangeOnInsertGrid(long OldRow, long OldCol, long NewRow, long NewCol);
	void AfterEditInsertGrid(long Row, long Col);
	void EnterCellInsertGrid();
	void BeforeEditInsertGrid(long Row, long Col, VARIANT_BOOL* Cancel);
	void ChangeEditInsertGrid();
	void ValidateEditInsertGrid(long Row, long Col, VARIANT_BOOL* Cancel);
	void BeforeMouseDownInsertGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel);
	void BeforeSortInsertGrid(long col, short* order);
   void AfterSortInsertGrid(long col, short* order);

   afx_msg void OnBnClickedZoomIn();
   afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomOut();
   afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomWindow();
   afx_msg void OnUpdateZoomWindow(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomExtents();
   afx_msg void OnUpdateZoomExtents(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomFull();
   afx_msg void OnUpdateZoomFull(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedPanCenter();
   afx_msg void OnUpdatePanCenter(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedQueryItem();
   afx_msg void OnUpdateQueryItem(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedRepaint();
   afx_msg void OnUpdateRepaint(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedViewOpton();
	afx_msg void OnBnClickedClose();
	afx_msg void OnClose();
	afx_msg void OnNMReleasedcaptureZoomslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedReviewcurrentgeometry();
	afx_msg void OnBnClickedTurnonpinlabel();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

#endif
