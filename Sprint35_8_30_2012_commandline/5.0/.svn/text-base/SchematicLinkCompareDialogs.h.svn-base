// $Header: /CAMCAD/4.6/SchematicLinkCompareDialogs.h 6     10/16/06 3:28a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// SchematicLinkCompareDialogs.h

#if !defined(__SchematicLinkCompareDialogs__h__)
#define __SchematicLinkCompareDialogs__h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "afxwin.h"
#include "UGCtrl.h"

enum GridSortOrderTag
{
   sortByAscending = UG_SORT_ASCENDING,
   sortByDescending = UG_SORT_DESCENDING,
};

enum NetCompareByType
{
	netCompareByName,
	netCompareByContent,
	netCompareByPcbAsMaster,
};

enum ClearMatchType
{
	clearMatchAll,
	clearMatchByCompare,
	clearMatchByCrossReference,
};

enum ShowMatchType
{
	showAllMatch,
	showOnlyMatch,
	showNonMatch,
};

//---------------------------------------------------------------------------------------
// CSchematicGrid
//---------------------------------------------------------------------------------------
class CSchematicGrid : public CUGCtrl
{
public:
   CSchematicGrid();
   ~CSchematicGrid();

private:
   const static int m_colSchematic = 0;
   const static int m_colPcb = 1;
   GridSortOrderTag m_colSchematicSortOrder;
   GridSortOrderTag m_colPcbSortOrder;
   bool m_initiated;
   bool m_editMode;

public:
   void initGrid();
   void addSchematicName(const CString schematicName);
   void addNames(const CString schematicName, const CString pcbName);
   void sort(const int col, const GridSortOrderTag sortOrder);
   void setEditMode(const bool editMode) { m_editMode = editMode; }

   int getColPcb() { return m_colPcb; }
   int getColSchematic() { return m_colSchematic; }

	virtual void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
	virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
};

//---------------------------------------------------------------------------------------
// CSchematicLinkCompareRefdesDlg
//---------------------------------------------------------------------------------------
class CSchematicLinkCompareRefdesDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CSchematicLinkCompareRefdesDlg)

public:
	CSchematicLinkCompareRefdesDlg(SchematicLink& schematicLink);
	virtual ~CSchematicLinkCompareRefdesDlg();
	enum { IDD = IDD_SchematicLinkCompareRefdes };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
   SchematicLink& m_schematicLink;
   CMapStringToString m_schematicNameMap;
   CMapStringToString m_pcbNameMap;

	BOOL m_caseSensitive;
	BOOL m_killTrailing;
	BOOL m_clearMatchType;
	BOOL m_showResultType;
	BOOL m_generateReport;
	CString m_killTrialingSuffix;

   CButton m_editMatch;
   CListBox m_schematicListbox;
   CListBox m_pcbListbox;
   CSchematicGrid m_schematicGrid;

   bool m_editMode;

   void initialDataLoad();
   void updateMatchCount();

public:
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedKillTrailing();
   afx_msg void OnBnClickedLoadCrossReference();
   afx_msg void OnBnClickedSaveCrossReference();
   afx_msg void OnBnClickedClearMatch();
   afx_msg void OnBnClickedEditMatchResult();
   afx_msg void OnBnClickedCompareItem();
   virtual BOOL OnSetActive();
   virtual LRESULT OnWizardNext();
};

//---------------------------------------------------------------------------------------
// CSchematicLinkCompareNetDlg
//---------------------------------------------------------------------------------------
class CSchematicLinkCompareNetDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CSchematicLinkCompareNetDlg)

public:
	CSchematicLinkCompareNetDlg(SchematicLink& schematicLink);
	virtual ~CSchematicLinkCompareNetDlg();
	enum { IDD = IDD_SchematicLinkCompareNet };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
   SchematicLink& m_schematicLink;
   CMapStringToString m_schematicNameMap;
   CMapStringToString m_pcbNameMap;
	
	BOOL m_caseSensitive;
	BOOL m_compareByType;
	BOOL m_clearMatchType;
	BOOL m_showResultType;
	BOOL m_generateReport;
   CString m_matchPercentage;

	CButton m_followByName;
	CButton m_editMatch;
   CListBox m_schematicListbox;
   CListBox m_pcbListbox;
   CSchematicGrid m_schematicGrid;

   void initialDataLoad();
   void updateMatchCount();

public:
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickCompareBy();
   afx_msg void OnBnClickedLoadCrossReference();
   afx_msg void OnBnClickedSaveCrossReference();
   afx_msg void OnBnClickedClearMatch();
   afx_msg void OnBnClickedEditMatchResult();
   afx_msg void OnBnClickedCompareItem();
   virtual BOOL OnSetActive();
   virtual BOOL OnWizardFinish();
};

//---------------------------------------------------------------------------------------
// CSelectMatchedDlg
//---------------------------------------------------------------------------------------
class CSelectMatchedDlg : public CResizingDialog
{
public:
	CSelectMatchedDlg(SchematicLink& schematicLink, const bool isComponent, const bool isSchematic);   // standard constructor
	virtual ~CSelectMatchedDlg();
	enum { IDD = IDD_SchematicLinkSelectMatch };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
   SchematicLink& m_schematicLink;
   CTypedPtrListContainer<ItemData*> m_matchedItemDataList;
   bool m_isComponent;
   bool m_isSchematic;
   ItemData* m_lastSelectedItemData;
   NetItem* m_netItem;
   CListBox m_matchedListbox;

   void fillListBox();

public:
   void addMatchItemData(ItemData* itemData);
   void setNetItem(NetItem* netItem);

   afx_msg void OnLbnSelChangeMatchList();
   virtual BOOL OnInitDialog();
};

#endif // __SchematicLinkCompareDialogs__h__