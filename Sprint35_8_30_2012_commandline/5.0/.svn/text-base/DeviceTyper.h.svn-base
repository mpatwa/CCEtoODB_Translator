// $Header: /CAMCAD/4.5/DeviceTyper.h 46    5/17/06 4:43p Rick Faltersack $

#if !defined(__DeviceTyper_h__)
#define __DeviceTyper_h__

#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "FlexGrid.h"

#define ICON_FOLDER          0
#define ICON_OPENFOLDER      1
#define ICON_FILE            2
#define ICON_RED_DOT         3
#define ICON_GREEN_DOT       4
#define ICON_YELLOW_DOT      5
#define ICON_HOLLOW_DOT      6

// Files used by Device Typer
#define QDEV_TYPER_FILENAME_DEVICES_STP       "devices.stp"
#define QDEV_TYPER_FILENAME_REFDESPREFIX_STP  "RefDesPrefix.stp"
#define QDEV_TYPER_FILENAME_ATTRIB_STP        "attrib.stp"
#define QDEV_TYPER_FILENAME_REPORT_STP        "report.stp"
#define QDEV_TYPER_FILENAME_GEOMPINMAP_STP    "geomPinMap.stp"
#define QDEV_TYPER_FILENAME_PNPINMAP_STP      "PNPinMap.stp"
#define QDEV_TYPER_FILENAME_CONFIG_STP        "config.stp"

typedef long GRID_CNT_TYPE;

enum PinmapViolationTag
{
	pinmapDeviceUnassigned, // aka hollow dot
	pinmapNoViolation, // aka green
	pinmapYellowViolation,
	pinmapRedViolation,
};


//_____________________________________________________________________________
//struct DeviceTypeStruct
class DeviceTypeStruct
{
public:
	CString deviceTypeName;

	int index;
	short minPinCount;
	short maxPinCount;
	CString expPinNames;
	CStringArray refNames;
	int refCount;
};

typedef CTypedPtrArray<CPtrArray, DeviceTypeStruct*> CDeviceType_Array_;

class CDeviceTypeArray : public CDeviceType_Array_
{
private:
	CCEtoODBDoc *m_doc;
	WORD m_deviceTypeKeyword;
	WORD m_tempDeviceTypeKeyword;
	WORD m_deviceToPackagePinMap;

public:
	void Init(CCEtoODBDoc *doc);
	void DestroyAll();
	DeviceTypeStruct *FindDeviceTypeByName(CString deviceName);
	DeviceTypeStruct *GetDevice(DataStruct *data);
	bool IsPinTyped(CompPinStruct *compPin,  CString &pinType);
	PinmapViolationTag GetPinMapViolation(DataStruct *data);
	int GetPinMapStatusIcon(DataStruct *selectedData);
	int GetRawPinCount(DataStruct *data);
	bool HasPinCountAnomaly(DataStruct *data);
};


typedef CTypedPtrArray<CPtrArray, DataStruct*> CDeviceTypeCompArray;

CDeviceTypeCompArray compArray;
int compArrayCount;

//_____________________________________________________________________________

class CDevtyperTreeCtrl : public CTreeCtrl
{
public:
	void DeleteItem(HTREEITEM node) { CTreeCtrl::DeleteItem(node); }
	void DeleteItem(CString itemName);
	void DeleteItem(HTREEITEM root, CString itemName);

	HTREEITEM FindItem(CString itemName);
	HTREEITEM FindItem(HTREEITEM root, CString itemName);
};

//_____________________________________________________________________________
class CDeviceTyperCompPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CDeviceTyperCompPropertyPage)

protected:
	CCEtoODBDoc *doc;
	int m_activeFileNumber;
	WORD deviceTypeKeyword;
	WORD tempDeviceTypeKeyword;
	WORD deviceToPackagePinMap;
	CStringArray prefixGeomArray;
	int prefixGeomArrayCount;

	CStringArray keyValArray;
	int keyValArrayCount;
	int m_compsCount;
	int m_unassignedCount;
	int m_assignedCount;
	int m_voilationCount;
	BOOL m_onInit;
	BOOL IsDataPinTyped(DataStruct* data, DataStruct* pin);
	BOOL IsDataPinTyped(DataStruct* data, DataStruct* pin, CString &pinType);
	
private:
	CImageList *m_imageList;
	//CTreeCtrl m_CompsTree;
	CDevtyperTreeCtrl m_CompsTree;
	int m_radioView;
	int m_radioGrouping;
	CListBox m_ListDevices;
	CComboBox m_cboPrefixGeom;
	CEdit m_compToFind;
	CComboBox m_cboKwValue;
	CStatic m_lblTotal;
	CStatic m_lblAssigned;
	CStatic m_lbl_NotAssigned;
	CStatic m_lblPercentage;
	CString lblTotal;
	CString lblAssigned;
	CString lblNotAssigned;
	CString lblPercentage;
	CButton m_addToStpFile;

   void UpdateCountsDisplay();
   void ApplySelectedTypeToSelectedTreeComps();
   void RemoveFromUnassignedView(HTREEITEM selItem);

// Dialog Data
	enum { IDD = IDD_DEVICETYPER_COMP };

public:
	CDeviceTyperCompPropertyPage();
	CDeviceTyperCompPropertyPage(CCEtoODBDoc& document);
	virtual ~CDeviceTyperCompPropertyPage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnSetActive();
	void GetGeomPinMapping();
	void GetPartNumPinMapping();
	void UpdateFromFile();
	void GetPrefixGeomArray();
	void SetPrefixGeomFile();
	void AddPrefixGeomItem(CString prefix, CString geom, CString type);
	void AssignPinMappingBasedOnPinName();

	void GetKeyValArray();
	void SetKeyValFile();
	void AddKeyValItem(CString key, CString val, CString type);

public:
	afx_msg BOOL ReadDeviceTypeMasterList();
	afx_msg void FillKeywordValueComboList();
	afx_msg void FillPrefixGeomComboList();
	afx_msg void AssignTypeByPrefix(CString pre, CString geom, CString type);
	afx_msg void AssignTypeByKeywordValue(CString keyword, CString value, CString type);
	afx_msg BlockStruct* ItemIsGeom(HTREEITEM item);
	afx_msg DataStruct* ItemIsComp(HTREEITEM item);
	afx_msg void OnBnClickedBtnAutoAssign();
	afx_msg void OnBnClickedRadioViewall();
	afx_msg void OnBnClickedRadioViewunassigned();
	afx_msg void OnBnClickedRadioviewviolations();
	afx_msg void OnBnClickedRadioFlat();
	afx_msg void OnBnClickedRadioKwvalue();
	afx_msg void OnBnClickedRadioPrefixgeom();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnTvnSelchangedCompsTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboPrefixgeom();
	afx_msg void OnBnClickedButtonManual();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonZoom1to1();
	afx_msg void OnBnClickedButtonClrkw();
	afx_msg void OnBnClickedButtonClrpre();
	afx_msg void OnCbnSelchangeComboKwvalue();
	afx_msg void OnBnClickedButtonReport();
	afx_msg void OnOK();
	//afx_msg BOOL OnApply();
	afx_msg void OnCancel();
	afx_msg void OnBnClickedButtonClearall();
	afx_msg void OnBnClickedButton1();
	afx_msg CString GetDeviceTypeByPrefix(CString prefix);
};

//_____________________________________________________________________________
class CDeviceTyperPinPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CDeviceTyperPinPropertyPage)

private:
	CFlexGrid m_flexGrid;

protected:
	CCEtoODBDoc *doc;
	int m_activeFileNumber;
	HTREEITEM prevSelected;
	BOOL messageSent;
	BOOL Updated;
	WORD deviceTypeKeyword;
	WORD tempDeviceTypeKeyword;
	WORD deviceToPackagePinMap;
	CStringArray prefixArray;
	int prefixArrayCount;
	int  initFlag;

public:
	CImageList *m_imageList;
	CDevtyperTreeCtrl m_pinsTree;
	int m_radioGeom;
	CButton m_addToStpFile;
	
// Dialog Data
	enum { IDD = IDD_DEVICETYPER_PIN };

public:
	CDeviceTyperPinPropertyPage();
	CDeviceTyperPinPropertyPage(CCEtoODBDoc& document);
	virtual ~CDeviceTyperPinPropertyPage();

	DECLARE_EVENTSINK_MAP()
	void AfterEditPinsGrid(long Row, long Col);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void GridSetup();
	void setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue);
	void fillPartNumberTree();
	bool PartNumberAttributePresent();
	void GetPrefix();
	void GetGeomPinMapping();
	void GetPartNumPinMapping();
	void UpdateFromFile();
	BOOL ItemIsInArray(CString item, int pnOrGeom, int arrayCount);
	bool IsPinTyped(CompPinStruct *compPin,  CString &pinType);
	bool IsPinTyped(DataStruct* data, DataStruct* pin, CString &pinType);

	void UpdateItemsInTree(HTREEITEM root, HTREEITEM selectedItem);
	void UpdateNodeStatus(HTREEITEM root);

	CString GetPinTypeOptionMenuString(CString deviceType);
	bool SetPinTypeOptionMenu(DataStruct *data);
	void UpdatePinsGrid(DataStruct *componentData);
	void UpdateNodeStatusIndicators(HTREEITEM selectedItem);

	void UpdateTreePeculiarFashion(HTREEITEM selectedItem, DataStruct *data);

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnSetActive();
	void MouseUpVsflex(short Button, short Shift, float X, float Y);

public:
	afx_msg void OnBnClickedRadioGeom();
	afx_msg void OnBnClickedRadioPartnum();
	afx_msg void OnBnClickedRadioRef();
	afx_msg void OnTvnSelchangedTreeDevicetypePin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnOK();
};

//_____________________________________________________________________________
class CDeviceTyperStylePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CDeviceTyperStylePropertyPage)

protected:
	CCEtoODBDoc *doc;
	int m_activeFileNumber;
	CImageList *m_imageList;
	BOOL    sendMessageOnInit;

public:
	CTreeCtrl m_PNTree;
	CEdit m_style;
	int m_showStyle;
	WORD styleKeyword;

// Dialog Data
	enum { IDD = IDD_DEVICETYPER_STYLE };

public:
	CDeviceTyperStylePropertyPage();
	CDeviceTyperStylePropertyPage(CCEtoODBDoc& document);
	virtual ~CDeviceTyperStylePropertyPage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	BOOL ReadDeviceTypeMasterList();
	void GetEnabledDevices(CStringArray *enabledDevices);
	BOOL DeviceEnabled(CString deviceName);
	void GetCompsWithEnabledDevices(CDeviceTypeCompArray *compArray);

	DECLARE_MESSAGE_MAP()

public:
	void FillTree();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedRadioShowstyle();
	afx_msg void OnTvnSelchangedTreePns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnTvnItemexpandedTreePns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpandingTreePns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnClickTreePns(NMHDR *pNMHDR, LRESULT *pResult);
};

//_____________________________________________________________________________
class CDeviceTyperStyleConfigPropertyPage : public CDialog
{
	DECLARE_DYNAMIC(CDeviceTyperStyleConfigPropertyPage)

protected:
	CListBox m_disabledList;
	CListBox m_enabledList;

public:
// Dialog Data
	enum { IDD = IDD_DEVICETYPER_STYLE_CONFIG };

public:
	CDeviceTyperStyleConfigPropertyPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDeviceTyperStyleConfigPropertyPage();

public:
	void FillDisabledTypesList();
	void FillEnabledTypesList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	BOOL DeviceEnabled(CString deviceName);
	void GetEnabledDevices(CStringArray *enabledDevices);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonMoveright();
	afx_msg void OnBnClickedButtonmoveleft();
	afx_msg void OnBnClickedOk();
};

//_____________________________________________________________________________
class CDeviceTyperPropertySheet : public CPropertySheet
{
private:
   CDeviceTyperCompPropertyPage  m_compPage;
   CDeviceTyperPinPropertyPage   m_pinPage;
   CDeviceTyperStylePropertyPage m_stylePage;

public:
   CDeviceTyperPropertySheet(CCEtoODBDoc& camCadDoc);

};

#endif
