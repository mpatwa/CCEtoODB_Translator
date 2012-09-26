// $Header: /CAMCAD/4.6/Sch_Link.h 49    10/16/06 3:24a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

//#define ERR_SETOFF            1
//#define ERR_NOERROR           0
//#define ERR_GENERAL           -1
//#define ERR_BADTYPE           -2
//#define ERR_NOTFOUND          -3
//#define ERR_CANTCOMPARE       -4
//#define ERR_NOTANET           -5
//#define ERR_NODATA            -6

#include "GeneralDialog.h"
#include "data.h"
#include "file.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ResizingDialog.h"
#include "FlexGridLib.h"  // Use the CFlexGridStatic class from it

class CCEtoODBDoc;
class SchLink;

////////////////////////////////////////////////////////////////////////////////////////////
// CFilter data and class
////////////////////////////////////////////////////////////////////////////////////////////
struct sCmd_Struct
{
   char cmd;
   char fileType;
   char dataType;
   CString findArg;
   CString replaceArg;
};
typedef CTypedPtrArray<CPtrArray, sCmd_Struct*> CsCmdArray;

#define STP_SUB_FULL    's'
#define STP_SUB_PARTIAL 'x'

#define STP_NET         'n'
#define STP_COMP        'c'

class CFilter
{
public:
   CFilter();
//   CFilter(CString logPath);
   ~CFilter();

private:
   CsCmdArray sCmd_Lst;

   int sCmd_Add(CString strCmd);

   CStdioFile logFile;
   BOOL logOpen;
   void writeLog(LPCSTR format, ...);

public:
   BOOL FileReloaded;
   BOOL KillTrailingAlphas;
   BOOL killTrailingSuffix;

   int GetCmdCount();
   void ClearAllData();
   int ReadFile(CString filePath);
   CString CheckName(CString strSrc, BOOL Component, BOOL Schematic);
};

////////////////////////////////////////////////////////////////////////////////////////////
// SchCompData class
////////////////////////////////////////////////////////////////////////////////////////////
class SchCompData
{
public:
   SchCompData()
   {
      OrigRefName = CompareName = "";
      Sheet = -1;
      data = NULL;
      indexMatch = -1;
      surface = 0;
      xrfMatch = FALSE;
   };
   ~SchCompData() {};

   CString OrigRefName;
   CString CompareName;
   CString SortKey;
   int Sheet;                 // The sheet the component is on (-1 if not on a sheet)
   DataStruct *data;          // pointer to DataStruct
   long indexMatch;           // -1 means we haven't been matched
   BOOL xrfMatch;             // indicates if the match is due to the xrf file
   short surface;             // Surface the component is on
};
typedef CTypedPtrArray<CPtrArray, SchCompData*> CSchCompArray;

////////////////////////////////////////////////////////////////////////////////////////////
// SchCompData class
////////////////////////////////////////////////////////////////////////////////////////////
class SchCompPinData
{
public:
   SchCompPinData()
   {
      CompName.Empty();
      PinName.Empty();
      m_matchedFound = false;
   };
   ~SchCompPinData() {};

   CString CompName;
   CString PinName;

   CString matchedCompName;

private:
   bool m_matchedFound;

public:
   int Compare(SchCompPinData *cpData);
   int Compare(CString compName, CString pinName);
   int CompareNoCase(SchCompPinData *cpData);
   int CompareNoCase(CString compName, CString pinName);

   bool isMatched() { return m_matchedFound; }
   void setMatched() { m_matchedFound = true; }
};
typedef CTypedPtrArray<CPtrArray, SchCompPinData*> CSchCompPinArray;

////////////////////////////////////////////////////////////////////////////////////////////
// SchNetData class
////////////////////////////////////////////////////////////////////////////////////////////
class SchNetData
{
public:
   SchNetData();
   ~SchNetData();

	void ClearCompPins();
private:
	int m_iPossibleMatches;
   long m_matchedCount;
	//CString m_sCPCompareString;

	CString getCPCompName(CompPinStruct *cpData, BOOL pcb, CCEtoODBDoc *doc);
   CString getCPPinName(CompPinStruct *cpData, BOOL pcb, CCEtoODBDoc *doc);

   int addCompPin(CString compName, CString pinName, CString matchedName = "");
public:
   CString OrigNetName;
   CString CompareName;
	CString SortKey;
   DataStruct *data;					// pointer to DataStruct
   NetStruct *net;					// pointer to NetStruct
   int Sheet;							// The sheet the component is on (-1 if not on a sheet)
   long indexMatch;					// -1 means we haven't been matched
   BOOL xrfMatch;						// indicates if the match is due to the xrf file
   CSchCompPinArray cpArray;		// comp/pin array that are in the netlist

   void SetNet(NetStruct *netData, BOOL pcb, CCEtoODBDoc *doc);
	void ApplyCompMatches(BOOL pcb, CCEtoODBDoc *doc, SchLink *schLink = NULL);
	int GetPossibleMatches();

   void ResetMatchedCount() { m_matchedCount = 0; }
   void AddMatchedCount(int matchedCount);
   long GetMatchedCount();
   long GetUnmatchedCount();
};
typedef CTypedPtrArray<CPtrArray, SchNetData*> CSchNetArray;

////////////////////////////////////////////////////////////////////////////////////////////
// SchLink class
////////////////////////////////////////////////////////////////////////////////////////////
class SchLink
{
public:
   SchLink();
   ~SchLink();

private:
   CString errMsg;
   BOOL caseSensitive;
   BOOL killTrailingAlphas;

   BOOL killTrailingSuffix;
   CString trailingSuffix;

   BOOL keepActiveDoc;
   CString schRefDesAttrib;
   CString pcbRefDesAttrib;
   CString schNetNameAttrib;
   CString pcbNetNameAttrib;

   int netCompType;
   int defaultFirstNetCompare;
   int minPercentMatch;

   BOOL completelyDestroy;

   CList<int, int&> sheetList;

   // Net arrays are sorted by compare names
   //  Sorting is achieved through an insertion sort algoritm in addCompToArray
   CSchCompArray pcbCompArray;
   CSchCompArray schCompArray;
	CMapStringToWord m_schCompMapCompareName;
	CMapStringToWord m_schCompMapRefName;

   // Net arrays are sorted by filtered (filterSettings) compare names
   //  Sorting is achieved through an insertion sort algoritm in addNetToArray
   CSchNetArray pcbNetArray;
   CSchNetArray schNetArray;

   //CFilter filterSettings;

   void clearAllSheetData();
   void clearAllSchCompData();
   void clearAllSchNetData();
   void clearAllPcbCompData();
   void clearAllPcbNetData();

   void clearSchXrefData();
   void clearPcbXrefData();

   int fillArrays();
   int fillSchArrays();
   int fillPcbArrays();
   int fillCompArrays();
   int fillNetArrays();
   int fillSheetArray();
   int fillSchCompArray();
   int fillSchNetArray();
   int fillPcbCompArray();
   int fillPcbNetArray();

   int addCompToArray(CSchCompArray *compArray, DataStruct *data, int sheetGeomNum);
   int addNetToArray(CSchNetArray *netArray, DataStruct *data, int sheetGeomNum);
   int addNetToArray(CSchNetArray *netArray, NetStruct *net, int sheetGeomNum);
   CString createSortKey(CString name);

   void applyCompMatches();
   long applyCompXrefNameMatches();
   long applyNetXrefNameMatches();

   int getPossibleMatches(CSchCompPinArray* list);
   BOOL compareNet(SchNetData *schData, SchNetData *pcbData);
   int compareNetsByName();
   int compareNetsByNameWithFilters();
   int compareNetsByContent();
   int compareNetsByContentUsingPcbAsMaster();
   BOOL compareComp(SchCompData *schData, SchCompData *pcbData);
public:
   CCEtoODBDoc *pcbDocPtr;
   FileStruct *pcbFilePtr;
   CCEtoODBDoc *schDocPtr;
   FileStruct *schFilePtr;

   BOOL netLinkState;
   BOOL compLinkState;

	CStringArray CompXRefArray[2];
   CStringArray NetXRefArray[2];

   BOOL SetCaseSensitive(BOOL CaseSensitive = TRUE);
   BOOL SetKillTrailingAlphas(BOOL KillTrailingAlphas = TRUE);
   BOOL SetKillTrailingSuffix(CString TrailingSuffix, BOOL KillTrailingSuffix = TRUE);
   BOOL SetToKeepActiveDoc(BOOL KeepActiveDoc = TRUE);
   int SetNetCompType(int NetCompType);

   CString SetSchRefDesAttrib(CString RefDesAttrib);
   CString SetPcbRefDesAttrib(CString RefDesAttrib);
   CString SetSchNetNameAttrib(CString NetNameAttrib);
   CString SetPcbNetNameAttrib(CString NetNameAttrib);
   CString GetSchRefDesAttrib() {return schRefDesAttrib;};
   CString GetPcbRefDesAttrib() {return pcbRefDesAttrib;};
   CString GetSchNetNameAttrib() {return schNetNameAttrib;};
   CString GetPcbNetNameAttrib() {return pcbNetNameAttrib;};

   SchCompData *findSchComp(CString findString, CString option);
   SchCompData *findPcbComp(CString findString, CString option);

   int SetDocuments(CCEtoODBDoc *pcbDoc, CCEtoODBDoc *schDoc);
	bool ApplyXrefNameMatches();
   
   int processNetXRef();
   int processCompXRef();
   int CompareNets(BOOL firstTime = FALSE);
   int CompareComps(BOOL firstTime = FALSE);
   void ResetComps();
   void ResetNets();

   void Reset();
   void ClearAllData();
   CString ApplyFilters(CString src, BOOL Component, BOOL Schematic);

   void removeHighlightedNets();
   void removeOverriddenComps();

   int LoadXRefFile(CString xRefPath, BOOL component, CStringArray *schArray, CStringArray *pcbArray);
   int LoadFilterFile(CString filterPath);
   void ClearFilterData();
   CString GetSheetName(int index);

   void ClearCrossProbedColors();

   int GetMatchForSch(CString schItemName, CString &matchedName, BOOL component, BOOL zoom = TRUE);
   int GetMatchForSch(DataStruct *schItem, CString& matchedName, BOOL zoom = TRUE);
   int GetMatchForPcb(CString pcbItemName, CString &matchedName, BOOL component, BOOL zoom = TRUE);
   int GetMatchForPcb(DataStruct *pcbItem, CString& matchedName, BOOL zoom = TRUE);
   void ZoomToComp(CCEtoODBDoc *doc, SchCompData *compData);
   void ZoomToNet(CCEtoODBDoc *doc, SchNetData *netData);

   CString GetLastError();
	void AnnotateNetnameUsingSchematicResult();
};



////////////////////////////////////////////////////////////////////////////////////////////
// CCompareCompPP dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CCompareCompPP : public CPropertyPage
{
   DECLARE_DYNAMIC(CCompareCompPP)

public:
   CCompareCompPP();
   virtual ~CCompareCompPP();

// Dialog Data
   enum { IDD = IDD_SCHLINK_REFDESCOMPARE };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();

   DECLARE_MESSAGE_MAP()
private:
   CListBox m_SchListBox;
   CListBox m_PcbListBox;
   CListBox m_UnmatchedSchListBox;
   CListBox m_UnmatchedPcbListBox;
   CString m_schRefDesAttrib;
   CString m_pcbRefDesAttrib;
   BOOL m_killAlphas;
   BOOL m_caseSensitive;
   BOOL m_killSuffix;
   CString m_suffix;

   BOOL compareItems(BOOL firstTime = FALSE);
   void fillListBox();
   void updateXrefButtons();
   void loadXrfFile(CString xrfPath, CStringArray *schArray, CStringArray *pcbArray);
   int editXrfFile(BOOL fromLoad, CStringArray *schArray, CStringArray *pcbArray);
public:
   afx_msg void OnBnClickedRecompare();
   afx_msg void OnBnClickedLoadxrf();
   afx_msg void OnBnClickedEditxrf();
   afx_msg void OnBnClickedClearxrf();
   afx_msg void OnEnChangeXrfpath();
   afx_msg void OnBnClickedNewrefdesSch();
   afx_msg void OnBnClickedNewrefdesPcb();
   afx_msg void OnBnClickedKilltrailingsuffix();
   virtual LRESULT OnWizardNext();
};

////////////////////////////////////////////////////////////////////////////////////////////
// CCompareNetPP dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CCompareNetPP : public CPropertyPage
{
   DECLARE_DYNAMIC(CCompareNetPP)

public:
   CCompareNetPP();
   virtual ~CCompareNetPP();

// Dialog Data
   enum { IDD = IDD_SCHLINK_NETCOMPARE };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();

   DECLARE_MESSAGE_MAP()
private:
   CListBox m_SchListBox;
   CListBox m_PcbListBox;
   CListBox m_UnmatchedSchListBox;
   CListBox m_UnmatchedPcbListBox;
   BOOL m_caseSensitive;
   BOOL m_CompareBy;

   void fillListBox();
   void updateXrefButtons();
   void loadXrfFile(CString xrfPath, CStringArray *schArray, CStringArray *pcbArray);
   int editXrfFile(BOOL fromLoad, CStringArray *schArray, CStringArray *pcbArray);
   void loadFilterFile(CString filterPath);
public:
   BOOL compareItems(BOOL firstTime = FALSE);
   afx_msg void OnBnClickedRecompare();
   afx_msg void OnBnClickedLoadxrf();
   afx_msg void OnBnClickedEditxrf();
   afx_msg void OnBnClickedClearxrf();
   afx_msg void OnEnChangeXrfpath();
   afx_msg void OnBnClickedCasesensitive();
   afx_msg void OnBnClickedLoadfilter();
   afx_msg void OnBnClickedEditfilter();
   afx_msg void OnBnClickedClearfilter();
   virtual BOOL OnWizardFinish();
};

////////////////////////////////////////////////////////////////////////////////////////////
// CXrefEditorDlg dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CXrefEditorDlg : public CDialog
{
   DECLARE_DYNAMIC(CXrefEditorDlg)

public:
   CXrefEditorDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~CXrefEditorDlg();

// Dialog Data
   enum { IDD = IDD_SCHLINK_XRFEDITOR };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()
private:
   CFlexGrid m_flexGrid;
   BOOL m_DropDownOption;

   CString comboBoxString;
   BOOL lastDropDownOption;

   CStringArray xRefArray[2];
   CStringArray pcbMatchArray[2];

   CFilter filterSettings;

   void setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue);
   void fillGrid();
   CString getComboBoxString();
public:
   BOOL forCompData;
   CString SavedName;

   void SetData(CStringArray *schArray, CStringArray *pcbArray);
   BOOL GetXRefData(long index, CString &schName, CString &pcbName);

   afx_msg void OnBnClickedShowmatches();
	afx_msg void OnBnClickedSetunmatchedByname();
   afx_msg void OnBnClickedClearmatches();
   afx_msg void OnBnClickedClearall();
   afx_msg void OnBnClickedSavexrf();
   DECLARE_EVENTSINK_MAP()
   afx_msg void OnBnClickedOk();
   void KeyPressVsflex(short* KeyAscii);
   void KeyUpVsflex(short* KeyCode, short Shift);
   void MouseUpVsflex(short Button, short Shift, float X, float Y);
   void ChangeEditVsflex();
   afx_msg void OnBnClickedCancel();
};

////////////////////////////////////////////////////////////////////////////////////////////
// CSelectItem dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CSelectItem : public CResizingDialog
{
public:
   CSelectItem(CWnd* pParent = NULL);   // standard constructor
   virtual ~CSelectItem();
   enum { IDD = IDD_SCHLINK_SELMULTI };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   DECLARE_MESSAGE_MAP()

private:
	CFlexGridStatic m_ocxStaGrid;
   SchCompData *lastSelCompData;
   
   CSchCompArray compArray;
   CSchNetArray netArray;
   void fillGrid();

public:
   BOOL ForComps;
   SchLink *SchLinkPtr;
   CCEtoODBDoc *DocPtr;
   FileStruct *FilePtr;
   CString NetAttrib;

   int AddItem(VOID *itemPtr);
   DECLARE_EVENTSINK_MAP()
   void SelChangeVsflex();
	void AfterRowColChangeVsflex(long oldRow, long oldCol, long newRow, long newCol);
	void MouseDownBrdDataGrid();
};

////////////////////////////////////////////////////////////////////////////////////////////
// CSchematicSettings dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CSchematicSettings
{
public:
   CSchematicSettings();
   ~CSchematicSettings();

   int PcbMargin;
   int SchMargin;

   COLORREF SchHightlightColor;
   COLORREF SchBusHightlightColor;
   COLORREF PcbHightlightColor;

   void LoadSchSettings();
   void SaveSchSettings();
};

////////////////////////////////////////////////////////////////////////////////////////////
// CSchLinkSettings dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CSchLinkSettings : public CDialog
{
   DECLARE_DYNAMIC(CSchLinkSettings)

public:
   CSchLinkSettings(CWnd* pParent = NULL);   // standard constructor
   virtual ~CSchLinkSettings();

// Dialog Data
   enum { IDD = IDD_SCHLINK_SETTINGS };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()
public:
   CString m_schMargin;
   CString m_pcbMargin;
   CColorButton m_schHighlightColorBtn;
   CColorButton m_pcbHighlightColorBtn;
	CColorButton m_schBusHighlightColorBtn;
};
