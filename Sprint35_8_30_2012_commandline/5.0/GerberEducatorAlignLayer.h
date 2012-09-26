// $Header: /CAMCAD/4.5/GerberEducatorAlignLayer.h 9     2/06/06 5:34p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GerberEducatorAlignLayer.h

#if ! defined (__GerberEducatorAlignLayer_h__)
#define __GerberEducatorAlignLayer_h__

#pragma once


#include "afxcmn.h"
#include "afxwin.h"
#include "FlexGridLib.h"
#include "CamCadDatabase.h"
#include "GerberEducatorUi.h"
#include "GerberEducator.h"
#include "DialogToolbar.h"
#include "MeasureDlg.h"

#define CONST_COLOR					"Color"
#define CONST_SELECT             "Sel"
#define CONST_LAYERNAME				"Layer Name"
#define CONST_DATASOURCE			"Data Source"
#define CONST_DATATYPE				"Data Type"

//-----------------------------------------------------------------------------
// CGEAlignLayerDlg dialog
//-----------------------------------------------------------------------------
class CGEAlignLayerDlg : public CMeasureBaseDialog  // CDialog // CResizingDialog
{
private:
	const static int m_colLayerIndex			= 0;
	const static int m_colColor				= 1;
   const static int m_colSelect           = 2;
	const static int m_colLayerName			= 3;
	const static int m_colDataSource			= 4;
	const static int m_colDataType			= 5;
   const static int m_colCount            = 6;

//	CGerberEducatorUi& m_gerberEducatorUi;
//	CCamCadDatabase m_camCadDatabase;
//	FileStruct& m_fileStruct;
   CGEStateLayerColor m_layerColor;

   CGerberEducatorUi* m_gerberEducatorUi;
   FileStruct* m_fileStruct;
   LayerStruct* m_temporaryCentroidLayer;

   CFlexGridStatic m_layerGrid;
   CResizingDialogToolBar m_toolBar;
   //CPoint2d* m_offsetFromPoint;
   //CPoint2d* m_offsetToPoint;
   //bool m_definingOffset;
	bool m_initialized;

   BOOL m_snapToInsertOrigin;
   BOOL m_snapToLineEnd;
   CButton m_rotateLayerButton;
   CButton m_mirrorLayerButton;
   CButton m_defineOffsetButton;
   CButton m_applyShiftButton;
   CButton m_createTemporaryCentroidButton;
   CButton m_noneRadio;
   CButton m_insertOriginRadio;
   CButton m_lineEndRadio;

public:
   CGEAlignLayerDlg(CMeasure& measure);
//   CGEAlignLayerDlg(CGerberEducatorUi& gerberEducatorUi);
   virtual ~CGEAlignLayerDlg();
   enum { IDD = IDD_GE_AlignLayer };

   CGerberEducatorUi& getGerberEducatorUi();
   void setGerberEducatorUi(CGerberEducatorUi* gerberEducatorUi);

private:
   virtual BOOL OnInitDialog();
   void initGrid();
   void addLayerToGrid();

   CExtendedFlexGrid& getFlexGrid()				{ return *(m_layerGrid.getGrid());	}
   FileStruct& getFileStruct();
   LayerStruct* getDefinedTemporaryCentroidLayer();
   void deleteTemporaryCentroidLayer();

   void drawOffsetArrow();
   void transformLayer(CTMatrix matrix);
   void enterDefineOffsetMode();
   void existDefineOffsetMode();

protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP() 

public:
   bool isInitialized() const { return m_initialized; }
   void updateMeasure();
   void updateSelectedEntity();
   BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_EVENTSINK_MAP()
   void AfterEditLayerGrid(long Row, long Col);
	void BeforeMouseDownLayerGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel);

   afx_msg void OnBnClickedMarkByWindow();
   afx_msg void OnUpdateMarkByWindow(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedMarkByWindowCross();
   afx_msg void OnUpdateMarkByWindowCross(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedClearMarked();
   afx_msg void OnUpdateClearMarked(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedRepaint();
   afx_msg void OnUpdateRepaint(CCmdUI* pCmdUI);

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
   afx_msg void OnBnClickedQuery();
   afx_msg void OnUpdateQuery(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedRotateLayer();
	afx_msg void OnBnClickedMirrorLayer();
	afx_msg void OnBnClickedDefineOffset();
   afx_msg void OnBnClickedNoneRadio();
   afx_msg void OnBnClickedInsertOriginRadio();
   afx_msg void OnBnClickedLineEndRadio();
	afx_msg void OnBnClickedApplyShift();
	afx_msg void OnBnClickedCreateTemporaryCentroid();
   afx_msg void OnBnClickedCancel();
   afx_msg void OnClose();
};

#endif
