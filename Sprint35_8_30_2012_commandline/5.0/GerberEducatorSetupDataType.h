// $Header: /CAMCAD/4.5/GerberEducatorSetupDataType.h 16    1/31/06 7:04p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GerberEducatorSetupDataType.h

#if ! defined (__GerberEducatorSetupDataType_h__)
#define __GerberEducatorSetupDataType_h__

#pragma once


#include "afxcmn.h"
#include "afxwin.h"
#include "FlexGridLib.h"
#include "CamCadDatabase.h"
#include "GerberEducatorUi.h"
#include "GerberEducator.h"
#include "DialogToolbar.h"

#define CONST_COLOR					"Color"
#define CONST_LAYERNAME				"Layer Name"
#define CONST_DATASOURCE			"Data Source"
#define CONST_DATATYPE				"Data Type"
#define CONST_DRAWS					"Draws"
#define CONST_FLASHES				"Flashes"
#define CONST_ACTIONS            "Actions"

#define CONST_ACTION_DRAWTOFLASH "Draw to Flash"
#define CONST_ACTION_ALIGNLAYER  "Align Layer"

//-----------------------------------------------------------------------------
// CGESetupDataType dialog
//-----------------------------------------------------------------------------
class CGESetupDataType : public CResizingDialog
{
private:
	const static int m_colLayerIndex			= 0;
	const static int m_colColor				= 1;
	const static int m_colLayerName			= 2;
	const static int m_colDataSource			= 3;
	const static int m_colDataType			= 4;
	const static int m_colDraws				= 5;
	const static int m_colFlashes				= 6;
	const static int m_colCount				= 7;

	CGerberEducatorUi& m_gerberEducatorUi;
	CCamCadDatabase m_camCadDatabase;
   CGerberEducatorColorScheme m_layerColor;
	CFlexGridStatic m_layerGrid;
   CResizingDialogToolBar m_toolBar;
	bool m_initialized;

public:
	CGESetupDataType(CGerberEducatorUi& gerberEducatorUi);   // standard constructor
	virtual ~CGESetupDataType();
	enum { IDD = IDD_GE_SetupDataType };

private:
	virtual BOOL OnInitDialog();
	void initGrid();
	void loadGrid();
	void setVisibleLayer();
   void setActionButtons();
	void moveLayersToGerberPCB();
   GerberEducatorDataSourceTypeTag autoAssignDataType(LayerStruct* layer, GerberEducatorDataSourceTag dataSourceTag);
   void updateLayerTypeAndColor(long row);
	void updateDrawFlashLayerAndCount();
   long isLayerInGrid(int layerIndex);

   CExtendedFlexGrid& getFlexGrid()				{ return *(m_layerGrid.getGrid());	}
	LayerStruct* getLayerByRow(long row);
	FileStruct* getGerberPCBFile();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	bool isInitialized()								{ return m_initialized;					}

	DECLARE_EVENTSINK_MAP()
   void BeforeRowColChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol, VARIANT_BOOL* Cancel);
   void BeforeSelChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol, VARIANT_BOOL* Cancel);
   void AfterRowColChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol);
   void AfterSelChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol);
	void BeforeEditLayerGrid(long Row, long Col, VARIANT_BOOL* Cancel);
	void ChangeEditLayerGrid();
	void BeforeMouseDownLayerGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel);
	void EnterLayerGrid();
   void OnDblClickLayerGrid();

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

	afx_msg void OnClose();
	afx_msg void OnBnClickedClose();
   afx_msg void OnBnClickedAlignLayers();
   afx_msg void OnBnClickedDrawToFlash();
};

#endif
