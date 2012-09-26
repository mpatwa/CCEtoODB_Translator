// $Header: /CAMCAD/5.0/GerberEducatorReviewGeometries.h 55    3/12/07 12:41p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GerberEducatorReviewGeometries.h

#if ! defined (__GerberEducatorReviewGeometries_h__)
#define __GerberEducatorReviewGeometries_h__

#pragma once


#include "afxcmn.h"
#include "afxwin.h"
#include "FlexGridLib.h"
#include "CamCadDatabase.h"
#include "PcbComponentPinAnalyzer.h"
#include "StaticLabel.h"
#include "GerberEducatorUi.h"
#include "DialogToolbar.h"

#define CONST_REVIEW					"Reviewed"
#define CONST_GEOMETRYNAME			"Geometry Name"
#define CONST_GEOMETRYTYPE			"Geometry Type"
#define CONST_TYPE					"Type"
#define CONST_PINNAME				"Pin"
#define CONST_PINSIZE				"Size"
#define CONST_PINDRILL				"Drill"

CString ipcStatusTagToString(IPCStatusTag ipcStatusTag);
IPCStatusTag stringToIPCStatusTag(CString ipcStatusString);

enum OriginMethodTag
{
	originMethodUndefined = -1,
	originMethodPinCenter = 0,
	originMethodPinExtent = 1,
	originMethodSnapToPad = 2,
	originMethodOffsetFromZero = 3,
};

CString originMethodTagToString(OriginMethodTag originMethodTag);
OriginMethodTag stringToOriginMethodTag(CString originMethodString);
OriginMethodTag intToOriginMethodTag(int tagValue);

//_____________________________________________________________________________
class CGeometryReviewStatusAttribute
{
private:
   CCamCadDatabase&		m_camCadDatabase;

   BookReadingOrderTag	m_bookReadingOrder;
   PinLabelingMethodTag m_pinLabelingMethod;
   PinOrderingMethodTag m_pinOrderingMethod;
   CString              m_alphaSkipList;
	IPCStatusTag			m_ipcStandard;
	OriginMethodTag		m_originMethod;
	double					m_xOffset;
	double					m_yOffset;
	Bool3Tag					m_reviewed;
   Bool3Tag             m_includeMechPin;
   Bool3Tag             m_includeFidPin;
   InsertTypeTag        m_blockInsertType;

public:
   CGeometryReviewStatusAttribute(CCamCadDatabase& camCadDatabase);
	void resetToDefault();

   BookReadingOrderTag getBookReadingOrder() const { return m_bookReadingOrder; }
   void setBookReadingOrder(BookReadingOrderTag bookReadingOrder) { m_bookReadingOrder = bookReadingOrder; }

   PinLabelingMethodTag getPinLabelingMethod() const { return m_pinLabelingMethod; }
   void setPinLabelingMethod(PinLabelingMethodTag pinNumberingMethod) { m_pinLabelingMethod = pinNumberingMethod; }

   PinOrderingMethodTag getPinOrderingMethod() const { return m_pinOrderingMethod; }
   void setPinOrderingMethod(PinOrderingMethodTag pinOrderingMethod) { m_pinOrderingMethod = pinOrderingMethod; }

   CString getAlphaSkipList() const { return m_alphaSkipList; }
   void setAlphaSkipList(const CString& alphaSkipList) { m_alphaSkipList = alphaSkipList; }

	IPCStatusTag getIPCStandard() const { return m_ipcStandard; }
	void setIPCStandard(const IPCStatusTag ipdStandard) { m_ipcStandard = ipdStandard; }

	OriginMethodTag getOriginMethod() const { return m_originMethod; }
	void setOriginMethod(const OriginMethodTag originMethod) { m_originMethod = originMethod; }

	double getXOffset() const { return m_xOffset; }
	void setXOffset(const double xOffset) { m_xOffset = xOffset; }

	double getYOffset() const { return m_yOffset; }
	void setYOffset(const double yOffset) { m_yOffset = yOffset; }

	Bool3Tag getReviewed() const { return m_reviewed; }
	void setReviewed(const bool reviewed) { m_reviewed = reviewed?boolTrue:boolFalse; }

   Bool3Tag getIncludeMechPin() const { return m_includeMechPin; }
   void setIncludeMechPin(const bool included) { m_includeMechPin = included?boolTrue:boolFalse; }

   Bool3Tag getIncludeFinPin() const { return m_includeFidPin; }
   void setIncludeFidPin(const bool included) { m_includeFidPin = included?boolTrue:boolFalse; }

   InsertTypeTag getBlockInsertType() const { return m_blockInsertType; }
   void setBlockInsertType(const InsertTypeTag blockInsertType) { m_blockInsertType = blockInsertType; }

	void set(CPcbComponentPinAnalyzer& pinAnalyzer);
	void loadFromAttribute(CAttributes& attributes);
   void storeToAttribute(CAttributes& attributes);
};

class CReviewQtyStaticLabel : public CStaticLabel
{
public:
	CReviewQtyStaticLabel()		{ m_quantity = m_review = 0; };
	~CReviewQtyStaticLabel()	{};

private:
	int m_quantity;
	int m_review;

public:
	void quantityPlus()						{ m_quantity++;			}
	void quantityMinus()						{ m_quantity--;			}
	void reviewPlus()							{ m_review++;				}
	void reviewMinus()						{ m_review--;				}

	void quantityPlus(const int count)	{ m_quantity += count;	}
	void quantityMinus(const int count)	{ m_quantity -= count;	}
	void reviewPlus(const int count)		{ m_review += count;		}
	void reviewMinus(const int count)	{ m_review -= count;		}

	void reset()								{ m_quantity = m_review = 0;}
	void updateLabel()						
	{
		CString label;
		label.Format("%d/%d", m_review, m_quantity);
		this->SetWindowText(label);
	}
};


class CReviewQtySummary
{
public:
	CReviewQtySummary();
	~CReviewQtySummary();

private:
	CReviewQtyStaticLabel m_pcbCompReviewQtyLabel;
	CReviewQtyStaticLabel m_mechCompReviewQtyLabel;
	CReviewQtyStaticLabel m_toolingReviewQtyLabel;
	CReviewQtyStaticLabel m_fiducialReviewQtyLabel;
   CReviewQtyStaticLabel m_viaReviewQtyLabel;

	BlockTypeTag getBlockType(InsertTypeTag insertType);

public:
	CReviewQtyStaticLabel& getPcbCompReviewQtyLabel()		{ return m_pcbCompReviewQtyLabel; }
	CReviewQtyStaticLabel& getMechCompReviewQtyLabel()		{ return m_mechCompReviewQtyLabel; }
	CReviewQtyStaticLabel& getToolingReviewQtyLabel()		{ return m_toolingReviewQtyLabel; }
	CReviewQtyStaticLabel& getFiducialReviewQtyLabel()		{ return m_fiducialReviewQtyLabel; }	
	CReviewQtyStaticLabel& getViaReviewQtyLabel()		   { return m_viaReviewQtyLabel; }	   

	void removeReviewQuantityFrom(BlockTypeTag blockType, int review, int quantity);
	void addReviewQuantityTo(BlockTypeTag blockType, int review, int quantity);

	void removeReviewQuantityFrom(InsertTypeTag insertType, int review, int quantity);
	void addReviewQuantityTo(InsertTypeTag insertType, int review, int quantity);

	void clearSummary();
	void setBackGroundColor(COLORREF backGroundColor);
};


//-----------------------------------------------------------------------------
// CGEReviewGeometriesDlg dialog
//-----------------------------------------------------------------------------
class CGEReviewGeometriesDlg : public CResizingDialog
{
	//DECLARE_DYNAMIC(CGEReviewGeometriesDlg)

private:
	// const for geometries grid
	const static int m_colPinNumOption			=  0;
	const static int m_colBookReadingOrder		=  1;
	const static int m_colPinNumberingOrder	=  2;
	const static int m_colSkipAlphas				=  3;
   const static int m_colIPCStatus				=	4;
	const static int m_colOriginOption			=  5;
   const static int m_colXoffset					=  6;
	const static int m_colYoffset					=  7;
   const static int m_colIncludeMechPin      =  8;
   const static int m_colIncludeFidPin       =  9;
	const static int m_colReview					=  10;
   const static int m_colGeometryName			=  11;
	const static int m_colGeometryType			=  12;
   const static int m_colCount					=	13;

	// const for pins grid
	const static int m_colSortablePinRefDes	=  0;
	const static int m_colPinName					=  1;
	const static int m_colPinType					=  2;
	const static int m_colPinDrill				=	3;
	const static int m_colPinSize					=	4;
   const static int m_colCountPinGrid			=	5;

	const static int m_buttonStatePush			= 0x0004;

	CGerberEducatorUi& m_gerberEducatorUi;
	CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;
	FileStruct& m_fileStruct;
	CMapStringToString m_pinNameMap;
	CFlexGridStatic m_geometriesGrid;
	CFlexGridStatic m_pinsGrid;
   CResizingDialogToolBar m_toolBar;
   CGerberEducatorColorScheme m_layerColor;

	CComboBox m_readingOrderCombo;
	CComboBox m_pinNumberingCombo;

   CStaticLabel m_ipcStatusStatic;

	// Summary table
	CStaticLabel m_geomTypeLabel;
	CStaticLabel m_pcbCompLabel;
	CStaticLabel m_mechCompLabel;
	CStaticLabel m_toolingLabel;
	CStaticLabel m_fiducialLabel;
   CStaticLabel m_viaLabel;
	CStaticLabel m_reviewQtyLabel;

	CReviewQtySummary m_reviewQuantitySummary;

   // Variables that hold the value of controls
	int m_pinNumberingOption;     // pin number radio buttons
	int m_originOption;           // origin radio buttons
	BOOL m_rotationOption;        // rotation standard
   BOOL m_includeMechPin;        // include Mechanical Pin checkbox
   BOOL m_includeFidPin;         // include Fiducail Pin checkbox

	int m_geomReviewStatusKeyword;
	bool m_initialized;
	bool m_pinLabelTopWasOn;
	bool m_pinLabelBottomWasOn;
	CString m_prevXOffset;
	CString m_prevYOffset;
	CellEditErrorTag m_cellEditError;

	DataStruct* m_pinOneData;
	double m_tolerance;

	// Pin direction bitmap
	CBitmapButton m_leftUpArrow;
	CBitmapButton m_leftDownArrow;
	CBitmapButton m_rightUpArrow;
	CBitmapButton m_rightDownArrow;
	CBitmapButton m_topLeftArrow;
	CBitmapButton m_topRightArrow;
	CBitmapButton m_bottomLeftArrow;
	CBitmapButton m_bottomRightArrow;

	// Starting Pin bitmap
	CBitmapButton m_topLeftPin;
	CBitmapButton m_topRightPin;
	CBitmapButton m_bottomLeftPin;
	CBitmapButton m_bottomRightPin;

	// Orientation Marker bitmap;
	CBitmapButton m_orientationTopLeft;
	CBitmapButton m_orientationTop;
	CBitmapButton m_orientationTopRight;
	CBitmapButton m_orientationRight;
	CBitmapButton m_orientationBottomRight;
	CBitmapButton m_orientationBottom;
	CBitmapButton m_orientationBottomLeft;
	CBitmapButton m_orientationLeft;

public:
	CGEReviewGeometriesDlg(CGerberEducatorUi& gerberEducatorUi);
	virtual ~CGEReviewGeometriesDlg();
	enum { IDD = IDD_GE_ReviewGeometries };

   virtual CString GetDialogProfileEntry() { return "GEReviewGeometriesDlg"; }

public:
   CCamCadData& getCamCadData() { return m_camCadDatabase.getCamCadData(); }

private:
	virtual BOOL OnInitDialog();

	void setReviewGeometriesView();
	void initGrids();
	void loadGeomtriesGrid();
	void loadPinsGrid();
	void loadGridSelectionImage();
	void loadOrientationImage();
	void sortPinsGridByRefdes(bool ascending);

	void enableControlsByReviewedStatus();
	void enablePinNameEdit(bool enable);
	void enableReadingOrder(bool enable);
	void enableOrientationMarker(bool enable);
	void enableIPCStatusGroup(bool enable);
	void enableOriginGroup(bool enable);

	void setGridSelectionButtons(BookReadingOrderTag bookReadingOrder, PinLabelingMethodTag pinNumbering);
	void resetGridSelectionButtons();
	void setPinAndDirectionButtons(BookReadingOrderTag bookReadingOrder, PinLabelingMethodTag pinNumbering);
	void setVisibleGridSelectionButtons();
	BookReadingOrderTag getBookReadingOrderFromButtons();
	PinLabelingMethodTag getPinNumberingFromComboBox();

	void setOrientationMarkerButtons(OutlineIndicatorOrientationTag orientationTag);
	void resetOrientationMarkerButtons();
	OutlineIndicatorOrientationTag getOrientationFromButtons();
	OutlineIndicatorOrientationTag getOrientationFromGeometry();

	bool checkDuplicateGeometryName(int editRow, CString newGeometryName);
	void undoStandardIPCStatus();
	void setIpcStatus(IPCStatusTag ipcStatus);

	BlockStruct* getSelectedGeometry();
	void setGeometrySelctionInGrid(BlockStruct* block);

	DataStruct* getSelectedPinData();
	DataStruct* getPinDataByRow(long row);
	DataStruct* getFirstSortedPinInGrid(InsertTypeTag insertType);
	InsertStruct* getSelectedPin();
	InsertStruct* getSelectedPinFromCAMCAD();
   int getPinCountByType(InsertTypeTag insertType);
   CString getUnusedMechanicalPinName();
   CString getUnusedFiducialName();

   CExtendedFlexGrid& getFlexGrid()				{ return *(m_geometriesGrid.getGrid());	}
   CExtendedFlexGrid& getPinsFlexGrid()		{ return *(m_pinsGrid.getGrid());			}
	void setPinSelectionInGrid(InsertStruct* pinInsert);

	void updateOptionSettingsToGrid();
	void updateOptionSettingsFromGrid();
	void updateReviewedStatuToGeometries();
	void updateComponentRotationAndLocation(double rotationOffset, double xOffset, double yOffset);
	void updateComponentRotationAndLocation(const CTMatrix& transformationMatrix);
	void updateDrill(DataStruct* pinData, double drillSize);
	
   void updateComppins(BlockStruct* componentBlock, CString oldPinName, CString newPinName);
	void updateNetlist();
   void removeComppins(BlockStruct* componentBlock, CString pinName);
   void addComppin(BlockStruct* componentBlock, CString pinName);

	void doPinNumbering();
	void doGeometryOrigin(CString xOffset = "0.0", CString yOffset = "0.0");
	void doTransform(const CTMatrix& transformationMatrix);
	void doOrientation(OutlineIndicatorOrientationTag orientationTag);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	void updateSelectedEntity();
	bool isInitialized()								{ return m_initialized;							}

	DECLARE_EVENTSINK_MAP()
	void OnClickGeometryGrid();
	void KeyDownEditGeometryGrid(long Row, long Col, short * KeyCode, short Shift);
	void AfterEditGeometryGrid(long Row, long Col);
	void AfterRowColChangeOnGeometryGrid(long OldRow, long OldCol, long NewRow, long NewCol);
	void EnterCellGeometryGrid();
	void ChangeEditGeometryGrid();
	void ValidateEditGeometryGrid(long Row, long Col, VARIANT_BOOL* Cancel);
   void BeforeEditGeometryGrid(long Row, long Col, VARIANT_BOOL* Cancel);
	void BeforeMouseDownGeometryGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel);
	void BeforeSortGeometryGrid(long col, short* order);

	void OnClickPinsGrid();
	void KeyDownEditPinsGrid(long Row, long Col, short * KeyCode, short Shift);
	void AfterEditPinsGrid(long Row, long Col);
	void AfterRowColChangeOnPinsGrid(long OldRow, long OldCol, long NewRow, long NewCol);
	void EnterCellPinsGrid();
	void ChangeEditPinsGrid();
	void ValidateEditPinsGrid(long Row, long Col, VARIANT_BOOL* Cancel);
	void BeforeMouseDownPinsGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel);
	void BeforeSortPinGrid(long col, short* order);

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

	afx_msg void OnPinNumberingClick();
	afx_msg void OnCbnSelchangeReadingOrderCombo();
	afx_msg void OnCbnSelchangePinNumberingCombo();
	afx_msg void OnKillFocusSkipAlphas();
	afx_msg void OnBnClickSetPinOne();
	afx_msg void OnBnClickedRotate90();
	afx_msg void OnOriginOptionClick();
	afx_msg void OnBnClickedSnapToSelectedPad();
	afx_msg void OnKillFocusXOffset();
	afx_msg void OnKillFocusYOffset();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedClose();
	afx_msg void OnClose();
	afx_msg void OnPaint();

	afx_msg void OnBnClickedLeftUpArrow();
	afx_msg void OnBnClickedLeftDownArrow();
	afx_msg void OnBnClickedRightDownArrow();
	afx_msg void OnBnClickedRightUpArrow();
	afx_msg void OnBnClickedTopLeftArrow();
	afx_msg void OnBnClickedTopRightArrow();
	afx_msg void OnBnClickedBottomLeftArrow();
	afx_msg void OnBnClickedBottomRightArrow();

	afx_msg void OnBnClickedTopLeftPin();
	afx_msg void OnBnClickedTopRightPin();
	afx_msg void OnBnClickedBottomLeftPin();
	afx_msg void OnBnClickedBottomRightPin();

	afx_msg void OnBnClickedOrientationTopLeft();
	afx_msg void OnBnClickedOrientationTop();
	afx_msg void OnBnClickedOrientationTopRight();
	afx_msg void OnBnClickedOrientationRight();
	afx_msg void OnBnClickedOrientationBottomRight();
	afx_msg void OnBnClickedOrientationBottom();
	afx_msg void OnBnClickedOrientationBottomLeft();
	afx_msg void OnBnClickedOrientationLeft();
	afx_msg void OnBnClickedClearOrienationMarker();
	afx_msg void OnBnClickedSetIpcStandard();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

#endif
