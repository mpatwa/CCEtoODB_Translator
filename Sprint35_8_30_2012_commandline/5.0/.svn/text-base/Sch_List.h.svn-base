// $Header: /CAMCAD/4.4/Sch_List.h 47    10/11/04 2:13p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ResizingDialog.h"
#include "TypedContainer.h"

/* Define Section *********************************************************/
#define SCH_NUM_PATCH_LENGTH     8  // The length of number to patch up to

// Use to itemType to indicate what type of item
#define SCH_ITEM_UNKNOWN         0  
#define SCH_ITEM_SHEET           1  // schematic sheet
#define SCH_ITEM_LOGICSYMBOL     2  // schematic logical symbol insert
#define SCH_ITEM_HIERARCHSYMBOL  3  // schematic hierarchy symbol insert
#define SCH_ITEM_NET             4  // net
#define SCH_ITEM_COMPPIN         5  // comppin
#define SCH_ITEM_SYMBOL          6  // geometry(symbol) of the logical symbol insert
#define SCH_ITEM_BUS					7
#define SCH_ITEM_BUSSES				8
#define SCH_ITEM_NETS				9
#define SCH_ITEM_SHEETINSTANCE	10
#define SCH_ITEM_SHEETINSTANCES	11

// Use to identify the index of the image used in the treeview in the Schematic Navigator
#define SCH_ICON_FOLDER          0
#define SCH_ICON_OPENFOLDER      1
#define SCH_ICON_FILE            2
#define SCH_ICON_SHEET           3
#define SCH_ICON_GATE            4
#define SCH_ICON_NET             5
#define SCH_ICON_CP              6
#define SCH_ICON_HIERARCHY       7
#define SCH_ICON_BUS					8
#define SCH_ICON_BUSSES				9
#define SCH_ICON_OPEN_BUSSES		10
#define SCH_ICON_NETS				11
#define SCH_ICON_OPEN_NETS			12


/* Structures Section *********************************************************/
typedef struct
{
   CString sortKey;
   void *voidPtr;
   int itemType;  // indicate the type of item, one of the 6 above defines
}TreeItemData;
typedef CTypedPtrListContainer<TreeItemData*> CTreeItemDataList;

// SchematicList dialog
class CCCAMCADDoc;


////////////////////////////////////////////////////////////////////////////////////////
class CSchematicList : public CResizingDialog
{
   DECLARE_DYNAMIC(CSchematicList)

public:
   CSchematicList(CWnd* pParent = NULL);   // standard constructor
   virtual ~CSchematicList();

// Dialog Data
   enum { IDD = IDD_LIST_SCHEMATIC };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   void FillSheetTree(COperationProgress *progress);
   void FillHierarchySheet(BlockStruct *block, HTREEITEM sheetItem, CMapStringToString *usedSheetsMap);
   void FillInstTree();
   void FillInstTreeFromBlock(BlockStruct *block);
   void FillBussesAndNetsTree(COperationProgress *progress);
	void FillTreeWithBusses(COperationProgress *progress);
	void FillTreeWithSubBussesAndNets(CBusStruct *bus, HTREEITEM busItem);
	void FillTreeWithNets(COperationProgress *progress);
	void FillTreeWithNet(NetStruct *net, HTREEITEM netItem);
   void FillSymbolsTree();
   void SelectSheet(BlockStruct *sheetBlock);
   BOOL SelectHierarchySheet(BlockStruct *selectedSheetBlock, HTREEITEM sheetItem);
   void SelectHierarchySymbol(DataStruct *hierarchyData);
   void SelectInstance(DataStruct *instData);
   void SelectNet(NetStruct *net);
	void SelectBus(CBusStruct *bus);
	void HightLightParentBus(CString busName);
   void UnHighLightNets();
   void Reset();
   void Resize(int n);
   void PanTo(CompPinStruct *compPin);
   void SetTreeItemData(CTreeCtrl *treeview, HTREEITEM treeItem, CString itemText, int itemType, void *voidPtr);
   virtual BOOL OnInitDialog();
   virtual void PostNcDestroy();   

public:
   virtual void OnCancel();

   virtual CString GetDialogProfileEntry() { return CString("SchematicListDialog"); }

   DECLARE_MESSAGE_MAP()

private:
   CResizingDialogField* m_sheetsField;
   CResizingDialogField* m_instancesField;
   CResizingDialogField* m_netsField;
   CResizingDialogField* m_symbolsField;
   CResizingDialogField* m_sheetsCheckBoxField;
   CResizingDialogField* m_instancesCheckBoxField;
   CResizingDialogField* m_netsCheckBoxField;
   CResizingDialogField* m_symbolsCheckBoxField;
   CResizingDialogField* m_instancesCurrentSheetOnlyCheckBoxField;
   CResizingDialogField* m_symbolsCurrentSheetOnlyCheckBoxField;

   HDWP m_hDwp;

public:
   CCEtoODBDoc *doc;
   FileStruct *currentViewFile;
   CView *view;
   CTreeItemDataList itemDataList;
   CImageList *m_imageList;
   CTreeCtrl m_sheetsTree;
   CTreeCtrl m_instancesTree;
   CTreeCtrl m_netsTree;
   CTreeCtrl m_symbolsTree;
	CMapStringToString m_netAndBusToParentMap;

   BOOL m_cpInst;
   BOOL m_cpSymbols;
   BOOL m_onOffSheets;
   BOOL m_onOffInstances;
   BOOL m_onOffNets;
   BOOL m_onOffSymbols;
   CRect initRect;

   afx_msg void OnBnClickedCpInst();
   afx_msg void OnBnClickedCpNets();
   afx_msg void OnBnClickedCpSymbols();
   afx_msg void OnTvnSelchangedSheetTree(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnTvnSelchangedInstTree(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnTvnSelchangedNetTree(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnDbClickSymbolTree(NMHDR *pNMHDR, LRESULT *pResult);   
   afx_msg void OnNMDblclkSymbolTree(NMHDR *pNMHDR, LRESULT *pResult);

   //Functions for change od selection in the camcad document.
   void InstSelectionChanged(DataStruct *instData);
   void NetSelectionChanged(NetStruct *net);
	void BusSelectionChanged(CBusStruct *bus);
   void SheetChanged(BlockStruct *sheetBlock);

private:
   WORD librarykeywordIndex;
   WORD cellkeywordIndex;
   WORD clusterkeywordIndex;
   WORD desigkeywordIndex;

public:
   afx_msg void OnBnClickedSchSheetsChck();
   afx_msg void OnBnClickedSchInstChck();
   afx_msg void OnBnClickedSchNetsChck();
   afx_msg void OnBnClickedSchSymChck();
   virtual void OnSize(UINT nType, int cx, int cy);

private:
   void deferWindowPos(CResizingDialogField* field,int dYpos,int newHeight,int dx);
};
