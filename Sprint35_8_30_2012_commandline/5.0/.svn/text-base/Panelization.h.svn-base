// $Header: /CAMCAD/4.5/Panelization.h 17    10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include "point2d.h"
#include "ccdoc.h"
#include "file.h"
#include "data.h"
#include "afxwin.h"
#include "afxcmn.h"

#define SEPTYPE_SPACING		0
#define SEPTYPE_OFFSET		1

#define LOCATION_BY_ORIGIN		0
#define LOCATION_BY_CENTER		1

//___________________________________________________________
class CBoardInstance
{
public:
	CBoardInstance(CCEtoODBDoc *doc, DataStruct *board);
	~CBoardInstance();

	void DeleteFromCamcad(CDataList *dataList);
	void Draw(double x, double y, double rotation, int mirror, double scale, COLORREF color, CDataList *dataList);
	void ClearColor();

private:
	CCEtoODBDoc *m_pDoc;
	BlockStruct *m_pBoard;
	DataStruct *m_pBoardData;

	CPoint2d m_boardCenter;
	CExtent m_boardExtents;
	short m_lastLocationSetBy;

	void recalcuateLocationFromCenter(double rotation, int mirror, double &x, double &y);
	
	DataStruct *getBoardOutline();
	CExtent getBoardOutlineExtents();

public:
	void SetBoard(BlockStruct *block);

	CString GetBoardInstName();
	void SetBoardInstName(CString instName);
	CString GetBoardName();
   BlockStruct *GetBoardBlock(){return m_pBoard;}

	double GetXSize();
	double GetYSize();

	void GetCenterPoint(double &x, double &y);
	short GetLastSetLocationBy();

	// instance location, calculates the actual location
	bool GetLocation(CPoint2d &location);
	bool GetLocation(double &x, double &y);
	void SetLocation(double x, double y, double rotation, bool mirror, bool aboutCenter);
	void TranslateLocation(double x, double y);		// based on current location
	//void GetBoardOffset(double &xOffset, double &yOffset);

	// calculates the actual location
	double GetRotation();
	bool GetMirror();

	bool IsInExtents(CExtent extent);
};
typedef CTypedPtrList<CPtrList, CBoardInstance*> CBoardInstanceList;


//___________________________________________________________
class CPanelInstance
{
public:
	CPanelInstance(CCEtoODBDoc *doc, FileStruct *panel = NULL);
	~CPanelInstance();

	void ClearAll(bool removeInCAMCAD = FALSE);
	void RemoveBoard(CBoardInstance *boardInst, bool removeInCAMCAD = FALSE);

	bool IsEqual(FileStruct *file);
	void DeleteFromCamcad(CFileList *fileList);
	void DrawBoard(CBoardInstance *boardInst, COLORREF color = NULL);

	bool Validate();
	void ClearValidate();
	bool isValid();

private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pPanelFile;
	CExtent m_panelExtents;

	CBoardInstanceList boards;
	bool validPanel;

	DataStruct *getPanelOutline();
	int getPanelOutlineLayer();
	CExtent getPanelOutlineExtents();

public:
	CString GetPanelName();
	void SetPanelName(CString panelName);

	CBoardInstance *GetBoard(CString boardName);
	CBoardInstance *GetFirstBoard();
	CBoardInstance *GetNextBoard(CBoardInstance *&board);

	void Show(bool show = TRUE);

	double GetXSize();
	double GetYSize();
	void SetSize(double xSize, double ySize, double xOrigin, double yOrigin, int widthIndex = -1);
	bool SetOrigin(double xOrigin, double yOrigin);

	// manual placement of boards
	CBoardInstance *CreateNewBoard(CString boardName, double x, double y, double rotation, bool mirror, bool aboutCenter, BlockStruct *boardToAdd);

	// board array placement
	bool AddBoardArray(CString baseBoardName, int xCount, int yCount, double rotation, bool mirror, bool aboutCenter, double xOffset, double yOffset, double xValue, double yValue, int spacingType, BlockStruct *boardToAdd);

	// translate all panel data into the document
	void ApplyToDocPanel();

};
typedef CTypedPtrList<CPtrList, CPanelInstance*> CPanelInstanceList;


//_____________________________________________________________________
// CAddPanelDlg dialog
class CAddPanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddPanelDlg)

public:
	CAddPanelDlg(CCEtoODBDoc *doc, bool singleBoardAddition = false, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddPanelDlg();

// Dialog Data
	enum { IDD = IDD_PANELIZATION_SINGLEBOARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_placementOption;

	CCEtoODBDoc *m_pDoc;
	bool m_bAddSingleBoard;
	CString m_panelName;
   CString m_xSizeStr; // X and Y sizes are handled as strings in order to control
   CString m_ySizeStr; // the number of decimal places displayed.
	int m_iPlacementOption;

	CComboBox m_boardsCB;
	BlockStruct *m_pBlockRet;

	DataStruct *getBoardOutline(BlockStruct *block);

protected:
	afx_msg void OnCbnSelchangeBoard();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

public:
	CString GetPanelName() const			{ return m_panelName; };
	double GetPanelXSize() const			{ return atof(m_xSizeStr);	};
	double GetPanelYSize() const			{ return atof(m_ySizeStr);	};
	BlockStruct *GetSelBlock() const		{ return m_pBlockRet; };
	int GetPlacementOption() const		{ return m_iPlacementOption;	};
};

//___________________________________________________________
// CPanelization
class CPanelization
{
public:
	CPanelization(CCEtoODBDoc *doc);
	~CPanelization();

	void ClearAll(bool removeInCAMCAD = FALSE);
	void RemovePanel(CPanelInstance *panelInst, bool removeInCAMCAD = FALSE);
	bool ValidateCurPanel();
	void ClearCurPanelValidation();

private:
	CCEtoODBDoc *m_pDoc;
	CPanelInstance *pCurPanelInst;
	
	CPanelInstanceList panels;

	CPanelInstance *pLastVisiblePanel;
	FileStruct *pLastVisibleFile;

	void getPanelsFromCAMCAD();
public:
	CPanelInstance *CreateNewPanel(CString panelName, double width, double height, double xOrigin = 0.0, double yOrigin = 0.0);

	CPanelInstance *GetCurPanel();
	void SetCurPanel(CPanelInstance *panelInst);
	CPanelInstance *GetPanel(CString panelName);
	CPanelInstance *GetFirstPanel();
	CPanelInstance *GetNextPanel(CPanelInstance *&panel);
};
	
//_____________________________________________________________________
// CManualBoardPlacementPropPage dialog
class CManualBoardPlacementPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CManualBoardPlacementPropPage)

public:
	CManualBoardPlacementPropPage(CPanelization *panelInfo, CCEtoODBDoc *doc);
	virtual ~CManualBoardPlacementPropPage();

// Dialog Data
	enum { IDD = IDD_PANELIZATION_MANUAL };

private:
	CCEtoODBDoc *m_pDoc;
	CPanelization *m_pPanelInfo;
	CBoardInstance *m_lastDrawnBoard;

	CComboBox m_panelCB;
	CString m_xSizeStr;
	CString m_ySizeStr;
	CListCtrl m_boardLC;

	int addToBoardList(CBoardInstance *boardInst, int itemIndex = -1);
	void fillBoardList(CPanelInstance *panelInst);
	void clearBoardColors(bool redraw = true);
   int FindUniqueBoardNameOnPanel(CString &selectItem, BlockTypeTag blcoktype);
   int FindAllBoardNamesOnFile(BlockTypeTag blcoktype);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangePanelCb();
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedCreatePanel();
	afx_msg void OnBnClickedRenamePanel();
	afx_msg void OnBnClickedRemovePanel();
	afx_msg void OnBnClickedAddBoard();
	afx_msg void OnBnClickedEditBoard();
	afx_msg void OnBnClickedRemoveBoard();
   afx_msg void OnBnClickedUpdateBoardName();
	afx_msg void OnNMDblclkBoardsLc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedApplyProperties();
	afx_msg void OnLvnColumnclickBoardsLc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedBoardsLc(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClickBoardsLc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelendokPanelCb();
	afx_msg void OnBnClickedSingleBoard();

	double GetPanelXSize() const			{ return atof(m_xSizeStr);	};
	double GetPanelYSize() const			{ return atof(m_ySizeStr);	};
};


//_____________________________________________________________________
// CBoardArrayPlacementPropPage dialog
class CBoardArrayPlacementPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBoardArrayPlacementPropPage)

public:
	CBoardArrayPlacementPropPage(CPanelization *panelInfo, CCEtoODBDoc *doc);
	virtual ~CBoardArrayPlacementPropPage();

// Dialog Data
	enum { IDD = IDD_PANELIZATION_ARRAY };

private:
	CCEtoODBDoc *m_pDoc;
	CPanelization *m_pPanelInfo;
	CBoardInstance *m_pPreviewBoardInst;

	void drawSpacingBMP(short spacingType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CComboBox m_boardCB;
	CComboBox m_rotationCB;
	BOOL m_separationType;
	double m_xOffset;
	double m_yOffset;
	BOOL m_flipped;
	short m_rows;
	short m_cols;
	double m_xValue;
	double m_yValue;
	BOOL m_locationBy;

public:
	afx_msg void OnBnClickedBoardseparation();
	afx_msg void OnBnClickedBoardseparation2();
	afx_msg void OnPaint();
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	afx_msg void OnBnClickedPreviewBoard();
	afx_msg void OnCbnSelchangeBoardname();
	afx_msg void OnBnClickedApplyBoardArray();
	afx_msg void OnBnClickedClearboards();
	virtual BOOL OnKillActive();
};

//___________________________________________________________
class CPanelizationPropertySheet : public CPropertySheet
{
private:
	CManualBoardPlacementPropPage m_manualPage;
	CBoardArrayPlacementPropPage m_arrayPage;

public:
   CPanelizationPropertySheet(CPanelization& panelInfo,CCEtoODBDoc& camCadDoc);

	virtual BOOL OnInitDialog();
};

/////////////////////////////////////////////////////////////////////////////
// PanelBoardDlg dialog
class PanelBoardDlg : public CDialog
{
// Construction
public:
   PanelBoardDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PanelBoardDlg)
   enum { IDD = IDD_PANEL_BOARD };
   CComboBox   m_boardsCB;
   BOOL  m_mirror;
   CString  m_refName;
   CString  m_rotation;
   CString  m_x;
   CString  m_y;
   CString  m_units;
   //}}AFX_DATA
   int decimals;
   CString blockname;
   
	BlockStruct *block;
   CCEtoODBDoc *doc;
	CBoardInstance *pBoardInst;

	double centerX, centerY;
	double originX, originY;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PanelBoardDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PanelBoardDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
	BOOL m_locationBy;
	afx_msg void OnBnClickedByorigin();
	afx_msg void OnBnClickedBycenter();
	afx_msg void OnBnClickedApply();
	afx_msg void OnCbnSelchangeBoardsCb();
	afx_msg void OnEnChangeRefName();
	afx_msg void OnEnChangeX();
	afx_msg void OnEnChangeY();
	afx_msg void OnCbnSelchangeRotation();
	afx_msg void OnBnClickedMirror();
};

/////////////////////////////////////////////////////////////////////////////
// UpdateBoardNameDlg dialog
class UpdateBoardNameDlg : public CDialog
{
// Construction
public:
   UpdateBoardNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(UpdateBoardNameDlg)
   enum { IDD = IDD_BOARD_UPDATENAME };
   CComboBox   m_boardsCB; 
   //}}AFX_DATA
   CCEtoODBDoc *doc; 
	CBoardInstance *m_pBoardInst;
   
   CString m_existBlockName;
   BlockStruct *m_selectBlock;
   BlockTypeTag m_assignBoardType;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(UpdateBoardNameDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(UpdateBoardNameDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
	
};
