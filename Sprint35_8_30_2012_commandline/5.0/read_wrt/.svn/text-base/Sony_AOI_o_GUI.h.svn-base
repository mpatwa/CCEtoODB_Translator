// $Header: /CAMCAD/4.5/read_wrt/Sony_AOI_o_GUI.h 9     3/24/06 8:31p Rick Faltersack $

#if ! defined (__Sony_AOI_o_GUI_h__)
#define __Sony_AOI_o_GUI_h__

#define USE_SONY_AOI_GUI
#ifdef USE_SONY_AOI_GUI

#pragma once


#include "Sony_AOI_o.h"
#include "GerberThermalDfm.h"
#include "ResizingPropertySheet.h"
#include <afxctl.h>
#include "RegularExpression.h"
#include "afxwin.h"
#include "UltimateGrid.h"


#define QStatus            "Status"
#define QPartNumber        "Part Number"
#define QGeom					"Geom"
#define QBitmap				"Bitmap"
#define QCategory				"Category"
#define QModel					"Model"
#define QSave					"Save"
#define QExplode				"Explode"
#define QRotAdj				"Rot. Adj."

#define QNotAvailable      "N/A"


// For saving Fid Page settings to attribute
#define QBoardName			"BoardName"
#define QTopFid1				"TopFid1"
#define QTopFid2				"TopFid2"
#define QBotFid1				"BotFid1"
#define QBotFid2				"BotFid2"

class CSonyAOIPropertySheet;

enum FilterSchemeTag
{
   filterSchemeFiducial,
   filterSchemeTooling,
   filterSchemeFiducialDefault,
   filterSchemeToolingDefault,
   filterSchemeUndefined
};

//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based SonyAOI Grid
//

class CSonyAoiPnXrefGrid : public CDDBaseGrid
{
private:
   bool m_saveOptionEnabled;

public:
    CSonyAoiPnXrefGrid();
    ~CSonyAoiPnXrefGrid() { }

    void SizeToFit(CRect &rect, bool redraw = true);

    virtual void OnSetup();
    virtual int  OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
    virtual int OnCellTypeNotify(long ID,int col,long row,long msg,LONG_PTR param);
    virtual int OnEditStart(int col, long row,CWnd **edit);

    virtual int  OnCanSizeCol(int col) { return false; }
    virtual int  OnCanSizeTopHdg()     { return false; }

    void FillCategoryDroplistOptions(CSonyDatabase *masterDB);
    void FillModelDroplistOptions(CSonyDatabase *masterDB, int rowIndx);

    bool GetSaveOptionEnabled()        { return m_saveOptionEnabled; }
    void SetSaveOptionEnabled(bool b)  { m_saveOptionEnabled = b;    }

	 // Column index vars
    int m_colStatus;
    int m_colPartNumber;
    int m_colGeom;
    int m_colBitmap;
    int m_colCategory;
    int m_colModel;
    int m_colSave;
    int m_colExplode;
	 int m_colRotAdj;
    int m_colCount;
};

//////////////////////////////////////////////////////////////////////////////////


//_____________________________________________________________________________
class CSonyAoiXrefPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CSonyAoiXrefPage)

private:
   BOOL m_overwriteValues;
	BOOL m_storeOption;

   CSonyAOIPropertySheet& m_parent;
   CString m_deviceTypeComboList;
   COLORREF m_redBackgroundColor;
   COLORREF m_greenBackgroundColor;
	COLORREF m_yellowBackgroundColor;
   COLORREF m_lightGrayBackgroundColor;

   CSonyAoiPnXrefGrid m_pnXrefGrid;


public:
	CSonyAoiXrefPage(CSonyAOIPropertySheet& parent);
	virtual ~CSonyAoiXrefPage();

   CSonyAOIPropertySheet& getParent() { return m_parent; }

   void fillGrid();
	void fillGridRow(int rowIndex, CSonyPart *part);
   void setRowStatus(int rowIndex,CSonyPart& part);
   void updatePartInGrid(long row,long col);
   void update();
	void updateRow(int rowIndex);

   void SetSaveOptionEnabled(bool flag)   { m_pnXrefGrid.SetSaveOptionEnabled(flag); }

   void UpdateGridColumnSizes(bool redraw = true);

// Dialog Data
	enum { IDD = IDD_SONY_AOI_XREF_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
   virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();

   DECLARE_EVENTSINK_MAP()
   LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   
   DECLARE_MESSAGE_MAP()
   afx_msg void OnSize(UINT nType, int cx, int cy);
};

//_____________________________________________________________________________

// CSonyAoiBoardFidPage dialog

class CSonyAoiBoardFidPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CSonyAoiBoardFidPage)

private:
	   CSonyAOIPropertySheet& m_parent;

public:
	CSonyAoiBoardFidPage(CSonyAOIPropertySheet& parent);
	virtual ~CSonyAoiBoardFidPage();

	CSonyAOIPropertySheet& getParent() { return m_parent; }

// Dialog Data
	enum { IDD = IDD_SONY_AOI_BOARDFID_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeBoardName();
	afx_msg void OnCbnSelchangeTopFid1();
	afx_msg void OnCbnSelchangeTopFid2();
	afx_msg void OnCbnSelchangeBotFid1();
	afx_msg void OnCbnSelchangeBotFid2();

	DataStruct *commitTopFid1();
	DataStruct *commitTopFid2();
	DataStruct *commitBotFid1();
	DataStruct *commitBotFid2();

	void saveBoardNameSetting(); // put board name setting into active CSonyBoard
	CString getSettingsString(); // for saving settings as attribute

private:
	CEdit m_txtBoardName;
	CComboBox m_cboTopFid1;
	CComboBox m_cboTopFid2;
	CComboBox m_cboBotFid1;
	CComboBox m_cboBotFid2;

	CStatic m_topFidFrame;
	CStatic m_botFidFrame;
	CStatic m_fidEnableExportMsg;

	virtual BOOL OnInitDialog();
	void fillFiducialCombos();
	void enableFiducialCombos();
	void loadSettingsFromAttribute();

};

//_____________________________________________________________________________

class CSonyAOIPropertySheet : public CResizingPropertySheet
{
	DECLARE_DYNAMIC(CSonyAOIPropertySheet)

private:
   CSonyBoard& m_sonyBoard;
	CSonyAoiBoardFidPage m_boardFidPage;
   CSonyAoiXrefPage m_xrefPage;

   void init();

public:
	CSonyAOIPropertySheet(CSonyBoard& sonyBoard);
	//virtual ~CSonyAOIPropertySheet();

   CSonyBoard& getSonyBoard() { return m_sonyBoard; }

   void updatePropertyPages(CPropertyPage* sendingPage);

   virtual CString GetDialogProfileEntry() { return CString("SonyAOIPropertySheet"); };

	void saveSettingsToAttribute();  // puts page settings in ccz as attribute for later restoration

   void SetSaveOptionEnabled(bool flag)   { m_xrefPage.SetSaveOptionEnabled(flag); }

protected:
	DECLARE_MESSAGE_MAP()

public:
	void UpdateExportButton();  // enable/disable based on export settings status
   virtual BOOL OnInitDialog();
   afx_msg void OnClose();
	afx_msg void OnBnClickedExport();
};




#endif

#endif