// $Header: /CAMCAD/4.5/ProbeStyles.h 12    1/09/07 12:12p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

// PROBESTYLES.H

#pragma once

#ifndef __PROBESTYLES__
#define __PROBESTYLES__

#include "dft.h"
#include "DeviceType.h"
#include "UltimateGrid.h"

/******************************************************************************
* EProbeStyleColumnTag
*/
enum EProbeStyleColumnTag
{
	probeStyleColumnUse = 0,
	probeStyleColumnName = 1, // style name
	probeStyleColumnProbeName = 2,
	probeStyleColumnShapeColor = 3,
	probeStyleColumnTechnology = 4,
	probeStyleColumnSide = 5,
	probeStyleColumnDrillMin = 6,
	probeStyleColumnDrillMax = 7,
	probeStyleColumnExposedMetalMin = 8,
	probeStyleColumnExposedMetalMax = 9,
	probeStyleColumnTargetType = 10,
	probeStyleColumnLastCol = 11,
	probeStyleColumnMaxCols = 11,
};

//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based LayerTypes Grid
//

// For the Probe Style Shape and Color Column
class CProbeStyleShapeCellType : public CUGCellType
{

public:
	CProbeStyleShapeCellType();
	~CProbeStyleShapeCellType();

	virtual LPCTSTR GetName();
	virtual LPCUGID GetUGID();

	virtual BOOL OnLClicked(int col,long row,int updn,RECT *rect,POINT *point);
	virtual BOOL OnDClicked(int col,long row,RECT *rect,POINT *point);
	virtual void OnDraw(CDC *dc,RECT *rect,int col,long row,CUGCell *cell,int selected,int current);

   static void EncodeVal(EProbeShape shapeTag, COLORREF foreColor, CString &encodedStr);
   static void DecodeVal(CString &encodedStr, EProbeShape &shapeTag, COLORREF &foreColor);

protected:
   void DrawProbeStyleShape(CDC &dc, EProbeShape shape, COLORREF color, COLORREF bkColor, CRect rect);
};

//---------------------------------------------------------------------------------------------

class CProbeStylesGrid : public CDDBaseGrid
{
private:
   CProbeStyleShapeCellType m_probeStyleShapeCellType;

public:
    CProbeStylesGrid();
    ~CProbeStylesGrid() { }

    int GetSelectedRow();  // returns row index of selected row or -1 if none selected
    int GetSelectedCol();  // returns col index of selected col or -1 if none selected

    void SizeToFit(CRect &rect);

    void OnDClicked(int col,long row, RECT *rect,POINT *point,BOOL processed);

    void GetRowValues(int row, bool &use, CString &styleName, CString &probeName, CString &technology, CString &surface, EProbeShape &shape, COLORREF &color, CString &drillMin, CString &drillMax, CString &exposedMin, CString &exposedMax, CString &targetType);

    virtual void OnSetup();
    virtual int OnEditStart(int col, long row,CWnd **edit);
    virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
};


//////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
* CRange 
*/
template <typename TYPE>
class CRange : public CObject
{
public:
	CRange() : CObject() {};
	CRange(TYPE initValue, TYPE comparisonTolerance) : CObject(),
		m_initialValue(initValue), m_min(initValue), m_max(initValue), m_comparisonTolerance(comparisonTolerance) {};
	CRange(const CRange& range)
	{
		if (this != &range)
		{
			m_initialValue = range.m_initialValue;
			m_min = range.m_min;
			m_max = range.m_max;
			m_comparisonTolerance = range.m_comparisonTolerance;
		}
	};
	~CRange() {};

private:
	TYPE m_initialValue;
	TYPE m_min;
	TYPE m_max;
	TYPE m_comparisonTolerance;

public:
	void operator=(const CRange& range)
	{
		if (this != &range)
		{
			m_initialValue = range.m_initialValue;
			m_min = range.m_min;
			m_max = range.m_max;
			m_comparisonTolerance = range.m_comparisonTolerance;
		}
	}

	TYPE GetMin() const		{ return m_min; };
	TYPE GetMax() const		{ return m_max; };
	bool IsInRange(TYPE value) const
	{
		if (m_min > m_max)
			return false;

		return (value - m_min >= m_comparisonTolerance && value - m_max <= m_comparisonTolerance);
	}

	void Reset()				{ m_min = m_max = m_initialValue; };
	void SetMin(TYPE value)	{ m_min = value; };
	void SetMax(TYPE value)	{ m_max = value; };
};

/******************************************************************************
* CProbeStyle 
*/
class CProbeStyle : public CObject
{
public:
	CProbeStyle();
	CProbeStyle(const CProbeStyle& other);
	~CProbeStyle() {};

private:
	bool m_bUse;
	CString m_sName;
	CString m_sTechnology;
	ETestSurface m_eSurface;
	CRange<double> m_rDrillDiameterRange;				// always stored as inches
	CString m_sProbeName;
	CRange<double> m_rExposedMetalDiameterRange;		// always stored as inches
	CString m_sTargetType;
	EProbeShape m_eShape;
	COLORREF m_color;

public:
	bool IsUsed() const								{ return m_bUse; };
	CString GetName() const							{ return m_sName; };
	CString GetTechnology() const					{ return m_sTechnology; };
	ETestSurface GetSurface() const				{ return m_eSurface; };
	CString GetSurfaceString() const;
	CRange<double> GetDrillRange() const		{ return m_rDrillDiameterRange; };
	CString GetProbeName() const					{ return m_sProbeName; };
	CRange<double> GetExposedRange() const		{ return m_rExposedMetalDiameterRange; };
	CString GetTargetType() const					{ return m_sTargetType; };
	EProbeShape GetShape() const					{ return m_eShape; };
	COLORREF GetColor() const						{ return m_color; };

	void SetUse(bool value)							{ m_bUse = value; };
	void SetName(CString value)					{ m_sName = value; };
	void SetTechnology(CString value)			{ m_sTechnology = value; };
	void SetSurface(ETestSurface value)			{ m_eSurface = value; };
	void SetSurfaceString(CString surface);
	void SetDrillMinRange(double value)			{ m_rDrillDiameterRange.SetMin(value); };
	void SetDrillMinRange(CString value)		{ m_rDrillDiameterRange.SetMin(value.IsEmpty()?-DBL_MAX:atof(value)); };
	void SetDrillMaxRange(double value)			{ m_rDrillDiameterRange.SetMax(value); };
	void SetDrillMaxRange(CString value)		{ m_rDrillDiameterRange.SetMax(value.IsEmpty()?DBL_MAX:atof(value)); };
	void SetProbeName(CString value)				{ m_sProbeName = value; };
	void SetExposedMinRange(double value)		{ m_rExposedMetalDiameterRange.SetMin(value); };
	void SetExposedMinRange(CString value)		{ m_rExposedMetalDiameterRange.SetMin(value.IsEmpty()?-DBL_MAX:atof(value)); };
	void SetExposedMaxRange(double value)		{ m_rExposedMetalDiameterRange.SetMax(value); };
	void SetExposedMaxRange(CString value)		{ m_rExposedMetalDiameterRange.SetMax(value.IsEmpty()?DBL_MAX:atof(value)); };
	void SetTargetType(CString value)			{ m_sTargetType = value; };
	void SetShape(EProbeShape shape);
	void SetColor(COLORREF color)					{ m_color = color; };

	bool IsMatchingProfile(CString technology = "", ETestSurface surface = testSurfaceUnset, double drillDiameter = 0., CString probeName = "", double exposedMetalDiameter = 0., CString targetType = "", InsertTypeTag insertType= (InsertTypeTag)-1, DeviceTypeTag deviceType = deviceTypeUnknown);
	void Dump(CStreamFileWriteFormat &file);
};

/******************************************************************************
* CProbeStyleList 
*/
class CProbeStyleList : public CTypedPtrListContainer<CProbeStyle*>
{
public:
	CProbeStyleList() : CTypedPtrListContainer<CProbeStyle*>() , m_sStyleListName("Default1") {};
	CProbeStyleList(CString name) : CTypedPtrListContainer<CProbeStyle*>() { m_sStyleListName = name; };

private:
	CString m_sStyleListName;

public:
	CString GetName() const				{ return m_sStyleListName; };

	void SetName(CString name)			{ m_sStyleListName = name; };

	CProbeStyle *FindMatchingProfile(CString technology = "", ETestSurface surface = testSurfaceUnset, double drillDiameter = 0., CString probeName = "", double exposedMetalDiameter = 0., CString targetTypeCSV = "", InsertTypeTag insertType = (InsertTypeTag)-1, DeviceTypeTag devType = deviceTypeUnknown);

	void Dump(const CString &filename);

	void Save(const CString &filename);
	void Save(CStreamFileWriteFormat &writeFormat);
	void Load(const CString &filename);
};


/******************************************************************************
* CProbeShapeColorDialog 
*/
class CProbeShapeColorDialog : public CDialog
{
	DECLARE_DYNAMIC(CProbeShapeColorDialog)

public:
	CProbeShapeColorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProbeShapeColorDialog();

// Dialog Data
	enum { IDD = IDD_PP_PROBESTYLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_cmbShape;
   CColorButton m_probeColorBtn;
	BOOL m_centerStyle;

	EProbeShape m_eShape;
	COLORREF m_color;

public:
	virtual BOOL OnInitDialog();

	EProbeShape GetProbeShape()						{ return m_eShape; };
	COLORREF GetProbeColor()							{ return m_color; };

	void SetProbeShape(EProbeShape shape)			{ m_eShape = shape; };
	void SetProbeColor(COLORREF color)				{ m_color = color; };

protected:
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeShapeCombo();
};


/******************************************************************************
* CProbeStylesDlg 
*/
class CProbeStylesDlg : public CResizingDialog
{
	//DECLARE_DYNAMIC(CProbeStylesDlg)

private:
   CProbeStylesGrid m_probeStylesGrid;
	CProbeStyleList m_lProbeStyles;
	CUnits m_curPageUnits;

	void UpdateGridColumnSizes();
	void updateGridWithProbeStyles();
	void drawShape(CDC &dc, EProbeShape shape, COLORREF color, COLORREF bkColor, CRect rect);
	void addStyleToGrid(bool use, CString name, CString technology, CString surface, double drillMin, double drillMax, CString probeName, EProbeShape shape, COLORREF color, double exposedMin, double exposedMax, CString targetType, long atRow=-1);
	void addStyleToGrid(bool use, CString name, CString technology, CString surface, CString drillMin, CString drillMax, CString probeName, EProbeShape shape, COLORREF color, CString exposedMin, CString exposedMax, CString targetType, long atRow=-1);
	void addStyleToGrid(CProbeStyle *probeStyle, long atRow=-1);

   void MoveRow(int directionFlag);  // Use UG_LINEUP and UG_LINEDOWN for direction flags

public:
	CProbeStylesDlg(PageUnitsTag curPageUnits, CWnd* pParent = NULL);   // standard constructor
	virtual ~CProbeStylesDlg();

// Dialog Data
	enum { IDD = IDD_PROBE_STYLES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual CString GetDialogProfileEntry() { return CString("ProbeStylesDialog"); }
	DECLARE_EVENTSINK_MAP()
	void DrawCellVsflex(long hDC, long Row, long Col, long Left, long Top, long Right, long Bottom, BOOL* Done);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedMoveUp();
	afx_msg void OnBnClickedMoveDown();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	void GetProbeStyles(CProbeStyleList &probeStyles);
};

#endif // __PROBESTYLES__
