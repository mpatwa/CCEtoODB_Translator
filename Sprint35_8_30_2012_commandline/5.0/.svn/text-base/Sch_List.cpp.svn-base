// $Header: /CAMCAD/5.0/Sch_List.cpp 114   6/17/07 8:53p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Sch_List.h"
#include "ccdoc.h"
#include "Sch_Lib.h"
#include "net_util.h"
#include "RwLib.h"

/* Function Prototypes *********************************************************/
void EditGeometryAndShowDlg(CCEtoODBDoc *doc, BlockStruct *block);
void HighlightPins(CDC *pDC, CCEtoODBView *view);
int PanReference(CCEtoODBView *view, const char *ref);
BOOL IsItemAFile(void* selSheetItem, CCEtoODBDoc *doc1);
BOOL IsCompPinOnThisSheet(CCEtoODBDoc *doc, WORD designKeywordIndex, CompPinStruct *compPin, BlockStruct *block);
BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, double *xmin, double *xmax, double *ymin, double *ymax, int SELECT_COMP);

/******************************************************************************
* IMPLEMENT_DYNAMIC
*/
IMPLEMENT_DYNAMIC(CSchematicList, CDialog)
CSchematicList::CSchematicList(CWnd* pParent /*=NULL*/)
   : CResizingDialog(CSchematicList::IDD, pParent)
   , m_cpInst(TRUE)
   , m_cpSymbols(TRUE)
   , m_onOffSheets(TRUE)
   , m_onOffInstances(TRUE)
   , m_onOffNets(TRUE)
   , m_onOffSymbols(TRUE)
{
   view = NULL;

   m_sheetsCheckBoxField = &(addFieldControl(IDC_SCH_SHEETS_CHCK,anchorLeft,growStatic));

   m_instancesCheckBoxField = &(addFieldControl(IDC_SCH_INST_CHCK,anchorProportionalVertical,growStatic));
   m_instancesCurrentSheetOnlyCheckBoxField = &(addFieldControl(IDC_CP_INST,anchorProportionalVertical));

   m_netsCheckBoxField = &(addFieldControl(IDC_SCH_NETS_CHCK,anchorProportionalVertical,growStatic));

   m_symbolsCheckBoxField = &(addFieldControl(IDC_SCH_SYM_CHCK ,anchorProportionalVertical,growStatic));
   m_symbolsCurrentSheetOnlyCheckBoxField = &(addFieldControl(IDC_CP_SYMBOLS   ,anchorProportionalVertical));

   m_sheetsField = &(addFieldControl(IDC_SHEET_TREE  ,anchorLeft,growHorizontal));
   m_sheetsField->getOperations().addOperation(glueTopEdge   ,toBottomEdge,m_sheetsCheckBoxField);
   m_sheetsField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,m_instancesCheckBoxField);

   m_instancesField = &(addFieldControl(IDC_INST_TREE   ,anchorLeft,growHorizontal));
   m_instancesField->getOperations().addOperation(glueTopEdge   ,toBottomEdge,m_instancesCheckBoxField);
   m_instancesField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,m_netsCheckBoxField);

   m_netsField = &(addFieldControl(IDC_NET_TREE    ,anchorLeft,growHorizontal));
   m_netsField->getOperations().addOperation(glueTopEdge   ,toBottomEdge,m_netsCheckBoxField);
   m_netsField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,m_symbolsCheckBoxField);

   m_symbolsField = &(addFieldControl(IDC_SYMBOL_TREE ,anchorBottom,growHorizontal));
   m_symbolsField->getOperations().addOperation(glueTopEdge   ,toBottomEdge,m_symbolsCheckBoxField);
}

/******************************************************************************
* SchematicList::~SchematicList
*/
CSchematicList::~CSchematicList()
{
}

/******************************************************************************
* CSchematicList::DoDataExchange
*/
void CSchematicList::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SHEET_TREE, m_sheetsTree);
   DDX_Check(pDX, IDC_CP_INST, m_cpInst);
   DDX_Control(pDX, IDC_INST_TREE, m_instancesTree);
   DDX_Check(pDX, IDC_CP_SYMBOLS, m_cpSymbols);
   DDX_Control(pDX, IDC_NET_TREE, m_netsTree);
   DDX_Control(pDX, IDC_SYMBOL_TREE, m_symbolsTree);
   DDX_Check(pDX, IDC_SCH_SHEETS_CHCK, m_onOffSheets);
   DDX_Check(pDX, IDC_SCH_INST_CHCK, m_onOffInstances);
   DDX_Check(pDX, IDC_SCH_NETS_CHCK, m_onOffNets);
   DDX_Check(pDX, IDC_SCH_SYM_CHCK, m_onOffSymbols);
}

/******************************************************************************
* BEGIN_MESSAGE_MAP
*/
BEGIN_MESSAGE_MAP(CSchematicList, CResizingDialog)
   ON_BN_CLICKED(IDC_CP_INST, OnBnClickedCpInst)
   ON_BN_CLICKED(IDC_CP_NETS, OnBnClickedCpNets)
   ON_BN_CLICKED(IDC_CP_SYMBOLS, OnBnClickedCpSymbols)
   ON_NOTIFY(TVN_SELCHANGED, IDC_SHEET_TREE, OnTvnSelchangedSheetTree)
   ON_NOTIFY(TVN_SELCHANGED, IDC_INST_TREE, OnTvnSelchangedInstTree)
   ON_NOTIFY(TVN_SELCHANGED, IDC_NET_TREE, OnTvnSelchangedNetTree)
   ON_NOTIFY(NM_DBLCLK, IDC_SYMBOL_TREE, OnNMDblclkSymbolTree)
   ON_BN_CLICKED(IDC_SCH_SHEETS_CHCK, OnBnClickedSchSheetsChck)
   ON_BN_CLICKED(IDC_SCH_INST_CHCK, OnBnClickedSchInstChck)
   ON_BN_CLICKED(IDC_SCH_NETS_CHCK, OnBnClickedSchNetsChck)
   ON_BN_CLICKED(IDC_SCH_SYM_CHCK, OnBnClickedSchSymChck)
END_MESSAGE_MAP()

/******************************************************************************
* CSchematicList::OnCancel
*/
void CSchematicList::OnCancel() 
{  
   UpdateData(true);

   // Save dialog settings to registry
   CAppRegistrySetting(GetDialogProfileEntry(), "Show Current Sheet Instances").Set(m_cpInst==TRUE?true:false);
   CAppRegistrySetting(GetDialogProfileEntry(), "Show Current Sheet Symbols").Set(m_cpSymbols==TRUE?true:false);
   CAppRegistrySetting(GetDialogProfileEntry(), "Show Sheets").Set(m_onOffSheets==TRUE?true:false);
   CAppRegistrySetting(GetDialogProfileEntry(), "Show Instances").Set(m_onOffInstances==TRUE?true:false);
   CAppRegistrySetting(GetDialogProfileEntry(), "Show Nets").Set(m_onOffNets==TRUE?true:false);
   CAppRegistrySetting(GetDialogProfileEntry(), "Show Symbols").Set(m_onOffSymbols==TRUE?true:false);

   UnHighLightNets();
   DestroyWindow();  
}

/******************************************************************************
* CSchematicList::PostNcDestroy
*/
void CSchematicList::PostNcDestroy() 
{
   if (view)
        ((CCEtoODBView*)view)->schListDlg = NULL;
   delete m_imageList;
   delete this;
   CResizingDialog::PostNcDestroy();
}

/******************************************************************************
* CSchematicList::OnInitDialog
*/
BOOL CSchematicList::OnInitDialog()
{
   // Get dialog settings from registry
   bool value = false;
   if (CAppRegistrySetting(GetDialogProfileEntry(), "Show Current Sheet Instances").Get(value))
      m_cpInst = value;
   if (CAppRegistrySetting(GetDialogProfileEntry(), "Show Current Sheet Symbols").Get(value))
      m_cpSymbols = value;
   if (CAppRegistrySetting(GetDialogProfileEntry(), "Show Sheets").Get(value))
      m_onOffSheets = value;
   if (CAppRegistrySetting(GetDialogProfileEntry(), "Show Instances").Get(value))
      m_onOffInstances = value;
   if (CAppRegistrySetting(GetDialogProfileEntry(), "Show Nets").Get(value))
      m_onOffNets = value;
   if (CAppRegistrySetting(GetDialogProfileEntry(), "Show Symbols").Get(value))
      m_onOffSymbols = value;

//   UpdateData(false);

   CResizingDialog::OnInitDialog();

   //m_cpInst = CAppRegistrySetting(GetDialogProfileEntry, "Show Current Sheet Instances").Get();
   //m_cpSymbols = CAppRegistrySetting(GetDialogProfileEntry, "Show Current Sheet Symbols").Get();
   //m_onOffSheets = CAppRegistrySetting(GetDialogProfileEntry, "Show Sheets").Get();
   //m_onOffInstances = CAppRegistrySetting(GetDialogProfileEntry, "Show Instances").Get();
   //m_onOffNets = CAppRegistrySetting(GetDialogProfileEntry, "Show Nets").Get();
   //m_onOffSymbols = CAppRegistrySetting(GetDialogProfileEntry, "Show Symbols").Get();

    // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 32, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_SCH));
   m_imageList->Add(app->LoadIcon(IDI_GATE));
   m_imageList->Add(app->LoadIcon(IDI_NET));
   m_imageList->Add(app->LoadIcon(IDI_CP));
   m_imageList->Add(app->LoadIcon(IDI_HEIRARCHY));
   m_imageList->Add(app->LoadIcon(IDI_BUS));
   m_imageList->Add(app->LoadIcon(IDI_BUSSES));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_BUSSES));
   m_imageList->Add(app->LoadIcon(IDI_NETS));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_NETS));
	m_imageList->SetOverlayImage(SCH_ICON_OPEN_NETS, SCH_ICON_NETS);

   m_sheetsTree.SetImageList(m_imageList, TVSIL_NORMAL);
   m_instancesTree.SetImageList(m_imageList, TVSIL_NORMAL);
   m_netsTree.SetImageList(m_imageList, TVSIL_NORMAL);
   m_symbolsTree.SetImageList(m_imageList, TVSIL_NORMAL);

   GetWindowRect(initRect);

   currentViewFile = NULL;

   librarykeywordIndex = doc->IsKeyWord(SCH_ATT_LIBRARYNAME, 0);
   cellkeywordIndex = doc->IsKeyWord(SCH_ATT_CELLNAME, 0);
   clusterkeywordIndex = doc->IsKeyWord(SCH_ATT_CLUSTERNAME, 0);
   desigkeywordIndex = doc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);

   // Change cursor to hourglass and create progress dialog
   CWaitCursor wait;
   COperationProgress progress;

   FillSheetTree(&progress);
   
   FileStruct *file;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      file = doc->getFileList().GetNext(filePos);
      if (file && file->isShown())
      {
         Attrib *attrib = NULL;
         WORD key = (WORD)doc->IsKeyWord(SCH_ATT_REFDES_MAP, 0);
         if (file->getBlock()->getAttributesRef() && file->getBlock()->getAttributesRef()->Lookup(key, attrib))
         {
            if (attrib && attrib->getValueType() == VT_STRING)
            {
               CString refdesAttrib = attrib->getStringValue();
               desigkeywordIndex = doc->IsKeyWord(refdesAttrib, 0);
            }
         }

         SelectSheet(file->getBlock());
         currentViewFile = file;
         break;
      }
   }

   // File the net tree
   FillBussesAndNetsTree(&progress);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
* TreeSortCompareFunc
   return negative if lParam1 comes before lParam2
*/
static int CALLBACK TreeSortCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   TreeItemData *itemData1 = (TreeItemData*)lParam1;
   TreeItemData *itemData2 = (TreeItemData*)lParam2;

   if (itemData1->sortKey.MakeUpper() < itemData2->sortKey.MakeUpper())
      return -1;
   else if (itemData1->sortKey.MakeUpper() == itemData2->sortKey.MakeUpper())
      return 0;
   else
      return 1;
}

/******************************************************************************
* CSchematicList::SetTreeDataItem
*/
void CSchematicList::SetTreeItemData(CTreeCtrl *treeview, HTREEITEM treeItem, CString itemText, int itemType, void *voidPtr)
{
   CString sortKey = "";
   CString numberString = "";
   int numberCount = 0;

   // (1) Creat the sort key
   for (int i=0; i<itemText.GetLength(); i++)
   {
      if (int(itemText[i]) >= 48 && int(itemText[i]) <= 57)
      {
         // The character is a number
         numberCount++;
         numberString.AppendChar(itemText[i]);
      }
      else 
      {
         // The character is NOT a number

         if (numberCount > 0)
         {
            // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
            while (numberCount < SCH_NUM_PATCH_LENGTH)
            {
               numberString.Insert(0, "0");
               numberCount++;
            }

            // Append the number to the sortKey
            sortKey.Append(numberString);
         }

         // Append the character to the sortKey
         sortKey.AppendChar(itemText[i]);
         numberString = "";
         numberCount = 0;
      }
   }

   // Need to check of the end of the string is number
   if (numberCount > 0)
   {
      // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
      while (numberCount < SCH_NUM_PATCH_LENGTH)
      {
         numberString.Insert(0, "0");
         numberCount++;
      }

      // Append the number to the sortKey
      sortKey.Append(numberString);
   }


   // (2) Creat a new item data
   TreeItemData *itemData = new TreeItemData;
   itemData->itemType = itemType;
   itemData->voidPtr = voidPtr;
   itemData->sortKey = sortKey;

   // (3) Add item data to list
   this->itemDataList.AddTail(itemData);

   // (4) Add item data to the tree
   treeview->SetItemData(treeItem, (DWORD)itemData);
}

/******************************************************************************
* CSchematicList::FillSheetTree
*/
void CSchematicList::FillSheetTree(COperationProgress *progress) 
{
   BOOL first = TRUE;
	CMapStringToString usedSheetsMap;
   int hierarchyIndex = doc->IsKeyWord("HierarchyGeom", 0);
   int primaryNameIndex = doc->IsKeyWord("PRIMARYNAME", 0);

	progress->updateStatus("Populating Schematic Navigator: Filling Sheet Tree...", doc->getMaxBlockIndex());
   for (int i = 0; i < doc->getMaxBlockIndex(); i++)
   {
		progress->incrementProgress();
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL || block->getBlockType() != blockTypeSheet)
         continue;

      FileStruct *file = doc->Find_File(block->getFileNumber());
      if(!file)
         continue;
      if (block->getFileNumber() != file->getFileNumber())
			continue;

      // If there is "HierarchyGeom" attribute, then it is not a top level sheet.  Skip it.
      Attrib *attrib = NULL;
      if (block->getAttributesRef() && block->getAttributesRef()->Lookup(hierarchyIndex, attrib))
         continue;


      CString blockName = block->getName();
      if (block->getAttributesRef() && block->getAttributesRef()->Lookup(primaryNameIndex, attrib))
      {
         if (attrib)
            blockName = attrib->getStringValue();
      }
      else
      {
         int index = blockName.ReverseFind(SCH_DELIMITER);
         blockName = blockName.Right(blockName.GetLength() - index - 1);
      }

      HTREEITEM sheetItem = m_sheetsTree.InsertItem(blockName, SCH_ICON_SHEET, SCH_ICON_SHEET, TVI_ROOT, TVI_SORT);
      SetTreeItemData(&m_sheetsTree, sheetItem, blockName, SCH_ITEM_SHEET, block);
		usedSheetsMap.SetAt(block->getName(), block->getName());

      FillHierarchySheet(block, sheetItem, &usedSheetsMap);
   }


   //Set the sort of the sheets
   TVSORTCB pSort;
   pSort.hParent = TVI_ROOT;
   pSort.lParam = 0;
   pSort.lpfnCompare = TreeSortCompareFunc;
   m_sheetsTree.SortChildrenCB(&pSort);


	// Add any reminding sheets that are not top level and are not linked by hierarchy into the "Unlink Sheets" folder
	HTREEITEM unlinkSheetItem = m_sheetsTree.InsertItem("Unlink Sheets", SCH_ICON_FOLDER, SCH_ICON_FOLDER, TVI_ROOT, TVI_SORT);
   SetTreeItemData(&m_sheetsTree, unlinkSheetItem, "Unlink Sheets", SCH_ITEM_UNKNOWN, NULL);
	
	bool hasUnlinkSheet = false;
   for (int i = 0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);      
      if (block == NULL || block->getBlockType() != blockTypeSheet)
         continue;

      FileStruct *file = doc->Find_File(block->getFileNumber());    
      if(!file)
         continue;
      if (block->getFileNumber() != file->getFileNumber())
			continue;


		// Schematic sheet is not use so add it to the "Unlink Sheets" folder
		CString blockName = "";
		if (!usedSheetsMap.Lookup(block->getName(), blockName))
		{
         Attrib *attrib = NULL;
         if (block->getAttributesRef() && block->getAttributesRef()->Lookup(primaryNameIndex, attrib))
         {
            if (attrib)
               blockName = attrib->getStringValue();
         }
         else
         {
				blockName = block->getName();
            int index = blockName.ReverseFind(SCH_DELIMITER);
            blockName = blockName.Right(blockName.GetLength() - index - 1);
         }

         HTREEITEM sheetItem = m_sheetsTree.InsertItem(blockName, SCH_ICON_SHEET, SCH_ICON_SHEET, unlinkSheetItem, TVI_SORT);
         SetTreeItemData(&m_sheetsTree, sheetItem, blockName, SCH_ITEM_SHEET, block);
			usedSheetsMap.SetAt(block->getName(), block->getName());

         FillHierarchySheet(block, sheetItem, &usedSheetsMap);
			hasUnlinkSheet = true;
		}
	}

	if (!hasUnlinkSheet)
		m_sheetsTree.DeleteItem(unlinkSheetItem);
}

/******************************************************************************
* CSchematicList::FileHierarchySheet
*/
void CSchematicList::FillHierarchySheet(BlockStruct *block, HTREEITEM sheetItem, CMapStringToString *usedSheetsMap)
{
   WORD geomNumKeywordIndex = doc->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);
   WORD desigKeywordIndex   = doc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
   WORD cellKeywordIndex    = doc->IsKeyWord(SCH_ATT_CELLNAME, 0);
   WORD primaryNameIndex    = doc->IsKeyWord("PRIMARYNAME", 0);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
		if (!data || !data->getInsert())
			continue;
      if (data->getInsert()->getInsertType() != insertTypeHierarchicalSymbol &&
			 data->getInsert()->getInsertType() != insertTypeSymbol)
         continue;

      Attrib *geomNumAttrib = NULL;
      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(geomNumKeywordIndex, geomNumAttrib))
      {
         if (!geomNumAttrib)
            continue;

         // Get the designator of the insert
			Attrib *attrib = NULL;
         CString designatorAttribValue = "";
         if (data->getAttributesRef()->Lookup(desigKeywordIndex, attrib))
         {
            if (attrib)
               designatorAttribValue = attrib->getStringValue();
         }

         // Get the cell name of the insert
         CString cellName = "";
         if (data->getAttributesRef()->Lookup(cellKeywordIndex, attrib))
         {
            if (attrib)
               cellName = attrib->getStringValue();
         }

         CString hierarchyInsertName;
         hierarchyInsertName.Format("%s [%s]", designatorAttribValue, cellName);

         // Insert the hierarchy symbol into the Sheet tree
         HTREEITEM hierarchyInsertItem = m_sheetsTree.InsertItem(hierarchyInsertName, SCH_ICON_HIERARCHY, SCH_ICON_HIERARCHY, sheetItem, TVI_SORT);
         SetTreeItemData(&m_sheetsTree, hierarchyInsertItem, hierarchyInsertName, SCH_ITEM_HIERARCHSYMBOL, data);

         // Look for the hierarchy sheets of this hierarchy symbol
         CString hierarchyGeomNum = geomNumAttrib->getStringValue();
         while (hierarchyGeomNum != "")
         {
            int start = 0;
            CString tmp = hierarchyGeomNum.Tokenize(" \t\n", start);

            int geomNum = atoi(tmp);
            hierarchyGeomNum.Delete(0, strlen(tmp));
            hierarchyGeomNum.Trim();

            BlockStruct *block = doc->getBlockAt(geomNum);
				if (block == NULL)
					continue;

            CString blockName = block->getName();
            if (block->getAttributesRef() && block->getAttributesRef()->Lookup(primaryNameIndex, attrib))
            {
               if (attrib)
                  blockName = attrib->getStringValue();
            }
            else
            {
               int index = blockName.ReverseFind(SCH_DELIMITER);
               blockName = blockName.Right(blockName.GetLength() - index - 1);
            }

            // Insert the hierarchy sheet under the hierarchy symbol
            HTREEITEM subSheetItem = m_sheetsTree.InsertItem(blockName, SCH_ICON_SHEET, SCH_ICON_SHEET, hierarchyInsertItem, TVI_SORT);
            SetTreeItemData(&m_sheetsTree, subSheetItem, blockName, SCH_ITEM_SHEET, block);
				usedSheetsMap->SetAt(block->getName(), block->getName());

            // Fill in nested hierarchy
            FillHierarchySheet(block, subSheetItem, usedSheetsMap);
         }

         //Set the sort of the sheet under each hierarchy symbol
         TVSORTCB pSort;
         pSort.hParent = hierarchyInsertItem;
         pSort.lParam = 0;
         pSort.lpfnCompare = TreeSortCompareFunc;
         m_sheetsTree.SortChildrenCB(&pSort);
      }
   }

   //Set the sort of the hierarchy symbol under the sheet
   TVSORTCB pSort;
   pSort.hParent = sheetItem;
   pSort.lParam = 0;
   pSort.lpfnCompare = TreeSortCompareFunc;
   m_sheetsTree.SortChildrenCB(&pSort);
}

/******************************************************************************
* CSchematicList::SelectSheet
*/
void CSchematicList::SelectSheet(BlockStruct *sheetBlock) 
{
   // loop through tree items in m_sheetsTree looking for sheetBlock (GetItemData) and select it
   HTREEITEM root = m_sheetsTree.GetRootItem();
   while (root)
   {
      TreeItemData *itemData = (TreeItemData*)m_sheetsTree.GetItemData(root);
      if (itemData != NULL && sheetBlock == (BlockStruct*)(itemData->voidPtr))
      {
         m_sheetsTree.SelectItem(root);
         m_sheetsTree.Select(root, TVGN_DROPHILITE);
         break;
      }

      // Look for the selected sheetBlock in the hierarchy sheets
      if (SelectHierarchySheet(sheetBlock, root))
         return;

      root = m_sheetsTree.GetNextSiblingItem(root);
   }
}

/******************************************************************************
* CSchematicList::SelectHierarchySheet
*/
BOOL CSchematicList::SelectHierarchySheet(BlockStruct *selectedSheetBlock, HTREEITEM sheetItem)
{
   // Look for the selected sheetBlock in the hierarchy sheets
   HTREEITEM hierarchyItem = m_sheetsTree.GetChildItem(sheetItem);
   while (hierarchyItem)
   {
      HTREEITEM sheetItem = m_sheetsTree.GetChildItem(hierarchyItem);
      while (sheetItem)
      {
         TreeItemData *itemData = (TreeItemData*)m_sheetsTree.GetItemData(sheetItem);
         if (selectedSheetBlock == (BlockStruct*)itemData->voidPtr)
         {
            m_sheetsTree.SelectItem(sheetItem);
            m_sheetsTree.Select(sheetItem, TVGN_DROPHILITE);
            return TRUE;
         }

         // Recursively look in the hierarchy for the selected sheetBlock
         if (SelectHierarchySheet(selectedSheetBlock, sheetItem))
            return TRUE;

         sheetItem = m_sheetsTree.GetNextSiblingItem(sheetItem);
      }

      hierarchyItem = m_sheetsTree.GetNextSiblingItem(hierarchyItem);
   }
   return FALSE;
}

/******************************************************************************
* CSchematicList::SelectHierarchySymbol
*/
void CSchematicList::SelectHierarchySymbol(DataStruct *hierarchyData)
{
   HTREEITEM item = m_sheetsTree.GetSelectedItem();
   if (item == NULL)
      return;

   int iconImage;
   m_sheetsTree.GetItemImage(item, iconImage, iconImage);

   // If the selected item in Sheet tree is an hierarchy item then get the parent sheet
   // else it is it already the parent sheet
   if (iconImage == SCH_ICON_HIERARCHY)
      item = m_sheetsTree.GetParentItem(item);
   
   HTREEITEM hierarchyItem = m_sheetsTree.GetChildItem(item);
   while (hierarchyItem)
   {
      TreeItemData *itemData = (TreeItemData*)m_sheetsTree.GetItemData(hierarchyItem);
      if (hierarchyData == (DataStruct*)itemData->voidPtr)
      {
         m_sheetsTree.Select(hierarchyItem, TVGN_DROPHILITE);
			m_sheetsTree.SelectSetFirstVisible(hierarchyItem);
         return;
      }
      hierarchyItem = m_sheetsTree.GetNextSiblingItem(hierarchyItem);
   }
}

/******************************************************************************
* CSchematicList::SelectInstance
*/
void CSchematicList::SelectInstance(DataStruct *instData) 
{
   // loop through tree items in m_instancesTree looking for instData (GetItemData) and select it
   HTREEITEM root = m_instancesTree.GetRootItem();
   while (root)
   {
      TreeItemData *itemData = (TreeItemData*)m_instancesTree.GetItemData(root);
      if (instData == (DataStruct*)itemData->voidPtr)
      {
         m_instancesTree.Select(root, TVGN_DROPHILITE);
			m_instancesTree.SelectSetFirstVisible(root);

         return;
      }
   
      root = m_instancesTree.GetNextSiblingItem(root);
   }

   // Instance is not in the instance tree so clear previous selected item
   m_instancesTree.SelectItem(NULL);
   m_instancesTree.SelectDropTarget(NULL);
}  

/******************************************************************************
* CSchematicList::SelectNet
*/
void CSchematicList::SelectNet(NetStruct *net) 
{
   // loop through tree items in m_netsTree looking for net and select it


   HTREEITEM root = m_netsTree.GetRootItem();
   while (root)
   {
      TreeItemData *itemData = (TreeItemData*)m_netsTree.GetItemData(root);
		if (itemData->itemType == SCH_ITEM_NETS)
		{
			HTREEITEM netItem = m_netsTree.GetChildItem(root);
			while (netItem)
			{
				itemData = (TreeItemData*)m_netsTree.GetItemData(netItem);
				if (net == (NetStruct*)itemData->voidPtr)
				{
					m_netsTree.SelectItem(netItem);
					m_netsTree.Select(netItem, TVGN_DROPHILITE);
					HightLightParentBus(net->getNetName());
					return;
				}
				
				netItem = m_netsTree.GetNextSiblingItem(netItem);
			}
		}
	   
      root = m_netsTree.GetNextSiblingItem(root);
   }

   // Net is not in the net tree so clear previous selected item
   m_instancesTree.SelectItem(NULL);
   m_instancesTree.SelectDropTarget(NULL);

   // Unhightlight
   int value = doc->RegisterValue(net->getNetName());
   doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);
}

/******************************************************************************
* CSchematicList::SelectBus
*/
void CSchematicList::SelectBus(CBusStruct *bus) 
{
   // loop through tree items in m_netsTree looking for bus and select it


   HTREEITEM root = m_netsTree.GetRootItem();
   while (root)
   {
      TreeItemData *itemData = (TreeItemData*)m_netsTree.GetItemData(root);
		if (itemData->itemType == SCH_ITEM_BUSSES)
		{
			HTREEITEM busItem = m_netsTree.GetChildItem(root);
			while (busItem)
			{
				itemData = (TreeItemData*)m_netsTree.GetItemData(busItem);
				if (bus == (CBusStruct*)itemData->voidPtr)
				{
					m_netsTree.SelectItem(busItem);
					m_netsTree.Select(busItem, TVGN_DROPHILITE);
					HightLightParentBus(bus->GetName());
					return;
				}
				
				busItem = m_netsTree.GetNextSiblingItem(busItem);
			}
		}
	   
      root = m_netsTree.GetNextSiblingItem(root);
   }

   // Net is not in the net tree so clear previous selected item
   m_instancesTree.SelectItem(NULL);
   m_instancesTree.SelectDropTarget(NULL);

   // Unhightlight
   int value = doc->RegisterValue(bus->GetName());
   doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);
}

/******************************************************************************
* CSchematicList::HightLightParentBus
*/
void CSchematicList::HightLightParentBus(CString netOrBusName)
{
	while (!netOrBusName.IsEmpty())
	{
		CString parentName;
		if (m_netAndBusToParentMap.Lookup(netOrBusName, parentName))
		{
			CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
			int value = doc->RegisterValue(parentName);
			doc->HighlightedNetsMap.SetAt(value, pApp->schSettings.SchBusHightlightColor);
			doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);

			netOrBusName = parentName;
		}
		else
		{
			netOrBusName.Empty();
		}
	}
}

/******************************************************************************
* CSchematicList::FillInstTree
*/
void CSchematicList::FillInstTree() 
{
   UpdateData();

   m_instancesTree.ShowWindow(SW_HIDE);
   m_instancesTree.SelectItem(NULL);
   m_instancesTree.DeleteAllItems();
   
   HTREEITEM item = m_sheetsTree.GetSelectedItem();
         
   if (!item)
      return;

   TreeItemData *itemData = (TreeItemData*)m_sheetsTree.GetItemData(item);
   if (m_cpInst && itemData->voidPtr && !IsItemAFile(itemData->voidPtr, doc)) // if current page checked
   {
      BlockStruct *block = (BlockStruct*)itemData->voidPtr;  // then block is the selected item
      FillInstTreeFromBlock(block);
   }
   else
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (!block)
            continue;

         if (block->getBlockType() != BLOCKTYPE_SHEET)
            continue;

         if (block->getFileNumber() != currentViewFile->getFileNumber())
            continue;

         FillInstTreeFromBlock(block);
      }
   }
   m_instancesTree.ShowWindow(SW_SHOW);

   m_instancesTree.SelectItem(NULL);
}

/******************************************************************************
* SchematicList::FillInstTreeFromBlock()
*/
void CSchematicList::FillInstTreeFromBlock(BlockStruct *block)
{
   WORD geomNumKeywordIndex = doc->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      if (!data || data->getDataType() != dataTypeInsert || !data->getAttributesRef())
			continue;
                                                            
      InsertStruct *insert = data->getInsert();
      if (!insert->getRefname() || !strlen(insert->getRefname()) ||
          (insert->getInsertType() != insertTypeSymbol && insert->getInsertType() != insertTypeHierarchicalSymbol)) 
         continue;

      Attrib *attrib = NULL;

      if (!data->getAttributesRef()->Lookup(librarykeywordIndex, attrib))
         continue;

      CString designatorAttribValue = "";
      if (data->getAttributesRef()->Lookup(desigkeywordIndex, attrib))
      {
         if (attrib)
            designatorAttribValue = attrib->getStringValue();
      }

      CString cellName = "";
      if (data->getAttributesRef()->Lookup(cellkeywordIndex, attrib))
      {
         if (attrib)
            cellName = attrib->getStringValue();
      }

      FileStruct *file = doc->Find_File(block->getFileNumber());

      CString buf;
      if (m_cpInst)
      {
         buf.Format("%s [%s]", designatorAttribValue, cellName);
      }
      else
      {
         int index = block->getName().ReverseFind(SCH_DELIMITER);
         CString blockName = block->getName().Right(block->getName().GetLength() - index - 1);

         buf.Format("%s [%s] (%s)", designatorAttribValue, cellName, blockName);
      }

		if (insert->getInsertType() == insertTypeSymbol)
		{
			// Regular logic symbol
			HTREEITEM dataItem = m_instancesTree.InsertItem(buf, SCH_ICON_GATE, SCH_ICON_GATE, TVI_ROOT, TVI_SORT);
			SetTreeItemData(&m_instancesTree, dataItem, buf, SCH_ITEM_LOGICSYMBOL, data);
		}
		else
		{
			// Hierarchy symbol
			HTREEITEM dataItem = m_instancesTree.InsertItem(buf, SCH_ICON_HIERARCHY, SCH_ICON_HIERARCHY, TVI_ROOT, TVI_SORT);
			SetTreeItemData(&m_instancesTree, dataItem, buf, SCH_ITEM_HIERARCHSYMBOL, data);
		}
   }

   //Set the sort of nets
   TVSORTCB pSort;
   pSort.hParent = TVI_ROOT;
   pSort.lParam = 0;
   pSort.lpfnCompare = TreeSortCompareFunc;
   m_instancesTree.SortChildrenCB(&pSort);
}

/******************************************************************************
* CSchematicList::FillBussesAndNetsTree
*/
void CSchematicList::FillBussesAndNetsTree(COperationProgress *progress) 
{
   UpdateData();

   m_netsTree.ShowWindow(SW_HIDE);
   m_netsTree.SelectItem(NULL);
   m_netsTree.DeleteAllItems();
   
	FillTreeWithBusses(progress);
	FillTreeWithNets(progress);

   m_netsTree.ShowWindow(SW_SHOW);  
}

/******************************************************************************
* CSchematicList::FillTreeWithBusses
*/
void CSchematicList::FillTreeWithBusses(COperationProgress *progress) 
{
   if (!currentViewFile)
      return;


	// Fill the tree with NETS
   HTREEITEM bussesItem = m_netsTree.InsertItem("BUSSES", SCH_ICON_BUSSES, SCH_ICON_BUSSES, TVI_ROOT, TVI_SORT);
   SetTreeItemData(&m_netsTree, bussesItem, "BUSSES", SCH_ITEM_BUSSES, NULL);

	progress->updateStatus("Populating Schematic Navigator: Filling Bus Tree...", currentViewFile->getBusList().GetCount());
	POSITION busPos = currentViewFile->getBusList().GetHeadPosition();
	while (busPos)
	{
		progress->incrementProgress();
		CBusStruct *bus = currentViewFile->getBusList().GetNext(busPos);
		if (bus == NULL)
			continue;

		// Insert the bus name into the tree
		HTREEITEM busItem = m_netsTree.InsertItem(bus->GetName(), SCH_ICON_BUS, SCH_ICON_BUS, bussesItem, TVI_SORT);
		SetTreeItemData(&m_netsTree, busItem, bus->GetName(), SCH_ITEM_BUS, bus);

		FillTreeWithSubBussesAndNets(bus, busItem);
	}

   //Set the sort of busses
   TVSORTCB pSort;
   pSort.hParent = bussesItem;
   pSort.lParam = 0;
   pSort.lpfnCompare = TreeSortCompareFunc;
   m_netsTree.SortChildrenCB(&pSort);
}

void CSchematicList::FillTreeWithSubBussesAndNets(CBusStruct *bus, HTREEITEM busItem)
{
	if (bus == NULL || busItem == NULL)
		return;

   WORD netnameKeywordIndex = doc->IsKeyWord(SCH_ATT_NETNAME, 0);


	// Create a sheet Instances Foler to hold all the sheet instances that the bus is on
	HTREEITEM sheetInstancesItem = m_netsTree.InsertItem("Sheets", SCH_ICON_SHEET, SCH_ICON_SHEET, busItem, TVI_LAST);
   SetTreeItemData(&m_netsTree, sheetInstancesItem, "Sheets", SCH_ITEM_SHEET, NULL);

	// Get the sheets that the bus is on
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      // Make sure that the block is a sheet
      if (!block || block->getFileNumber() != currentViewFile->getFileNumber() || block->getBlockType() != BLOCKTYPE_SHEET)
         continue;

      // Loop and get the sheet name that the net is on and insert it under the net name in the tree
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         // Make sure the data is an etch
         if (data->getDataType() != T_POLY || data->getGraphicClass() != graphicClassBus)
            continue;

         if (!data->getAttributesRef())
            continue;

         // Make sure the etch has a netname
			Attrib *attrib = NULL;
         if (!data->getAttributesRef()->Lookup(netnameKeywordIndex, attrib))
            continue;

         // Make sure the netname is the same
         if (!attrib || bus->GetName() != attrib->getStringValue())
            continue;

         // Get the name of the sheet
         int index = block->getName().ReverseFind(SCH_DELIMITER);
         CString blockName = block->getName().Right(block->getName().GetLength() - index - 1);

         // Insert the sheet name into the tree and exit loop
         HTREEITEM sheetWithSelBus = m_netsTree.InsertItem(blockName, SCH_ICON_SHEET, SCH_ICON_SHEET, sheetInstancesItem, TVI_SORT);
         SetTreeItemData(&m_netsTree, sheetWithSelBus, blockName, SCH_ITEM_SHEET, block);
         break;
      }
	}

	// Set the sort of Sheet Instance folder
	TVSORTCB pSort;
	pSort.hParent = sheetInstancesItem;
	pSort.lParam = 0;
	pSort.lpfnCompare = TreeSortCompareFunc;
	m_netsTree.SortChildrenCB(&pSort);


	// Get all the sub busses under this bus
	POSITION busNamePos = bus->GetHeadPositionBusName();
	if (busNamePos != NULL)
	{
		// Create a Busses Foler to hold all the sub busses
		HTREEITEM subBussesItem = m_netsTree.InsertItem("SubBusses", SCH_ICON_BUSSES, SCH_ICON_BUSSES, busItem, TVI_LAST);
		SetTreeItemData(&m_netsTree, subBussesItem, "SubBusses", SCH_ITEM_BUSSES, NULL);
		while (busNamePos)
		{
			CString subBusName = bus->GetNextBusName(busNamePos);
			if (subBusName.IsEmpty())
				continue;

			CBusStruct *subBus = currentViewFile->getBusList().Find(subBusName);
			if (subBus == NULL)
				continue;

			HTREEITEM subBusItem = m_netsTree.InsertItem(subBusName, SCH_ICON_BUS, SCH_ICON_BUS, subBussesItem, TVI_SORT);
			SetTreeItemData(&m_netsTree, subBusItem, subBusName, SCH_ITEM_BUS, subBus);

			FillTreeWithSubBussesAndNets(subBus, subBusItem);
			m_netAndBusToParentMap.SetAt(subBusName, bus->GetName());
		}	

		// Set the sort of Sub Busses folder
		pSort.hParent = subBussesItem;
		pSort.lParam = 0;
		pSort.lpfnCompare = TreeSortCompareFunc;
		m_netsTree.SortChildrenCB(&pSort);
	}


	// Get all the nets under this bus
	POSITION netNamePos = bus->GetHeadPositionNetName();
	if (netNamePos !=  NULL)
	{
		// Create a Nets Foler to hold all the sub busses
		HTREEITEM netsItem = m_netsTree.InsertItem("Nets", SCH_ICON_NETS, SCH_ICON_NETS, busItem, TVI_LAST);
		SetTreeItemData(&m_netsTree, netsItem, "Nets", SCH_ITEM_NETS, NULL);

		while (netNamePos)
		{
			CString netName = bus->GetNextNetName(netNamePos);
			if (netName.IsEmpty())
				continue;

			NetStruct *net = FindNet(currentViewFile, netName);
			if (net == NULL)
				continue;

			HTREEITEM netItem = m_netsTree.InsertItem(netName, SCH_ICON_NET, SCH_ICON_NET, netsItem, TVI_SORT);
			SetTreeItemData(&m_netsTree, netItem, netName, SCH_ITEM_NET, net);

			FillTreeWithNet(net, netItem);
			m_netAndBusToParentMap.SetAt(netName, bus->GetName());
		}		

		// Set the sort of Nets folder
		pSort.hParent = netsItem;
		pSort.lParam = 0;
		pSort.lpfnCompare = TreeSortCompareFunc;
		m_netsTree.SortChildrenCB(&pSort);
	}
}

/******************************************************************************
* CSchematicList::FillTreeWithNets
*/
void CSchematicList::FillTreeWithNets(COperationProgress *progress) 
{
   if (!currentViewFile)
      return;


	// Fill the tree with NETS
   HTREEITEM netsItem = m_netsTree.InsertItem("NETS", SCH_ICON_NETS, SCH_ICON_NETS, TVI_ROOT, TVI_SORT);
   SetTreeItemData(&m_netsTree, netsItem, "NETS", SCH_ITEM_NETS, NULL);

	progress->updateStatus("Populating Schematic Navigator: Filling Net Tree...", currentViewFile->getNetList().GetCount());
   POSITION netPos = currentViewFile->getNetList().GetHeadPosition();
   while (netPos)
   {
		progress->incrementProgress();
      NetStruct *net = currentViewFile->getNetList().GetNext(netPos);
      if (net == NULL)
         continue;

      // Inser the net name into the tree
      HTREEITEM netItem = m_netsTree.InsertItem(net->getNetName(), SCH_ICON_NET, SCH_ICON_NET, netsItem, TVI_SORT);
      SetTreeItemData(&m_netsTree, netItem, net->getNetName(), SCH_ITEM_NET, net);

		FillTreeWithNet(net, netItem);
   }

   //Set the sort of nets
   TVSORTCB pSort;
   pSort.hParent = netsItem;
   pSort.lParam = 0;
   pSort.lpfnCompare = TreeSortCompareFunc;
   m_netsTree.SortChildrenCB(&pSort);
}

/******************************************************************************
* CSchematicList::FillTreeWithNet
*/
void CSchematicList::FillTreeWithNet(NetStruct *net, HTREEITEM netItem)
{
	if (net == NULL || netItem == NULL)
		return;

   WORD netnameKeywordIndex = doc->IsKeyWord(SCH_ATT_NETNAME, 0);
   WORD compKeywordIndex = doc->IsKeyWord(SCH_ATT_COMPDESIGNATOR, 0);
   WORD pinKeywordIndex = doc->IsKeyWord(SCH_ATT_PINDESIGNATOR, 0);


   // Get the sheets and the comppin on those sheets
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      // Make sure that the block is a sheet
      if (!block || block->getFileNumber() != currentViewFile->getFileNumber() || block->getBlockType() != BLOCKTYPE_SHEET)
         continue;

      HTREEITEM sheetWithSelNet = NULL;

      // Loop and get the sheet name that the net is on and insert it under the net name in the tree
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         // Make sure the data is an etch
         if (data->getDataType() != T_POLY || data->getGraphicClass() != graphicClassSignal)
            continue;

         if (!data->getAttributesRef())
            continue;

         // Make sure the etch has a netname
			Attrib *attrib = NULL;
         if (!data->getAttributesRef()->Lookup(netnameKeywordIndex, attrib))
            continue;

         // Make sure the netname is the same
         if (!attrib || net->getNetName() != attrib->getStringValue())
            continue;

         // Get the name of the sheet
         int index = block->getName().ReverseFind(SCH_DELIMITER);
         CString blockName = block->getName().Right(block->getName().GetLength() - index - 1);

         // Insert the sheet name into the tree and exit loop
         sheetWithSelNet = m_netsTree.InsertItem(blockName, SCH_ICON_SHEET, SCH_ICON_SHEET, netItem, TVI_SORT);
         SetTreeItemData(&m_netsTree, sheetWithSelNet, blockName, SCH_ITEM_SHEET, block);
         break;
      }

      if (!sheetWithSelNet)
         continue;

      // Loop and get all the comppin on the sheet and insert it under the sheet name in the tree
      HTREEITEM compPinItem;
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         // Make sure that is a comppin and it is on this sheet
         if (!compPin->getRefDes() || !strlen(compPin->getRefDes()) || !IsCompPinOnThisSheet(doc, this->desigkeywordIndex, compPin, block))
            continue;
         
         // Get the comp refdes
			Attrib *attrib = NULL;
         CString compRefDes = "";
         if (compPin->getAttributesRef() && compPin->getAttributesRef()->Lookup(compKeywordIndex, attrib))
         {
            if (attrib)
               compRefDes = attrib->getStringValue();
         }
         
         // Get the pin refdes
         CString pinRefDes = "";
         if (compPin->getAttributesRef() && compPin->getAttributesRef()->Lookup(pinKeywordIndex, attrib))
         {
            if (attrib)
               pinRefDes = attrib->getStringValue();
         }

         // Inser the comppin refdes into the tree under the sheet name
         CString compPinRefDes = "";
         compPinRefDes.Format("%s(%s)", compRefDes, pinRefDes);
         compPinItem = m_netsTree.InsertItem(compPinRefDes, SCH_ICON_CP, SCH_ICON_CP, sheetWithSelNet, TVI_SORT);
         SetTreeItemData(&m_netsTree, compPinItem, compPinRefDes, SCH_ITEM_COMPPIN, compPin);
      }

      if (sheetWithSelNet)
      {
         if (!m_netsTree.ItemHasChildren(sheetWithSelNet))
         {
            m_netsTree.DeleteItem(sheetWithSelNet);            
         }
         else
         {
            //Set the sort of comppin under the sheet
            TVSORTCB pSort;
            pSort.hParent = sheetWithSelNet;
            pSort.lParam = 0;
            pSort.lpfnCompare = TreeSortCompareFunc;
            m_netsTree.SortChildrenCB(&pSort);
         }
      }
   }

   /*if (!m_netsTree.ItemHasChildren(netItem))
   {
      m_netsTree.DeleteItem(netItem);
   }
   else
   {*/
      //Set the sort of sheets under the net
      TVSORTCB pSort;
      pSort.hParent = netItem;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_netsTree.SortChildrenCB(&pSort);
   //}
}

/******************************************************************************
* CSchematicList::FillSymbolsTree
*/
void CSchematicList::FillSymbolsTree()
{
   UpdateData();
   m_symbolsTree.ShowWindow(SW_HIDE);
   m_symbolsTree.SelectItem(NULL);
   m_symbolsTree.DeleteAllItems();

   if (!currentViewFile)
      return;
   
   void *voidPtr;
   CMapStringToPtr libNameMap;
   CString libName, cellName, clusterName;
   
   WORD geomNumKeywordIndex = doc->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);

   if (m_cpSymbols)
   {
      POSITION dataPos = currentViewFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = currentViewFile->getBlock()->getDataList().GetNext(dataPos);

         if (!data || data->getDataType() != dataTypeInsert)
            continue;

         if (data->getInsert()->getInsertType() != insertTypeSymbol)
            continue;

         Attrib* geomNumAttrib;

         // If it is an hierarchy symbol, then skip it
         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(geomNumKeywordIndex, geomNumAttrib))
            continue;

         BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());   

         if (block && block->getAttributesRef())
         {
            Attrib *attrib = NULL;

            //lib name
            if (block->getAttributesRef()->Lookup(librarykeywordIndex, attrib))
            {
               if (attrib)
                  libName = attrib->getStringValue();
            }
            else
               continue;

            //cell name
            if (block->getAttributesRef()->Lookup(cellkeywordIndex, attrib))
            {
               if (attrib)
                  cellName = attrib->getStringValue();
            }
            else
               continue;

            //cluster name
            if (block->getAttributesRef()->Lookup(clusterkeywordIndex, attrib))
            {
               if (attrib)
                  clusterName = attrib->getStringValue();
            }
            else
               continue;
         }
         else
            continue;
         
         CMapStringToPtr *cellMap; 
         if (libNameMap.Lookup(libName, voidPtr))
            cellMap = (CMapStringToPtr*)voidPtr;
         else
         {
            cellMap = new CMapStringToPtr();
            libNameMap.SetAt(libName, cellMap);
         }

         CMapStringToPtr *clusterMap;
         if (cellMap->Lookup(cellName, voidPtr))   
            clusterMap = (CMapStringToPtr*)voidPtr;
         else
         {
            clusterMap = new CMapStringToPtr();
            cellMap->SetAt(cellName, clusterMap);
         }

         CMapStringToPtr *symbolName;
         if (clusterMap->Lookup(clusterName, voidPtr))
            symbolName = (CMapStringToPtr*)voidPtr;
         else
         {
            symbolName = new CMapStringToPtr();
            clusterMap->SetAt(clusterName, symbolName);
         }
         symbolName->SetAt(block->getName(), block);
      }

      POSITION libMapPos = libNameMap.GetStartPosition();
      while (libMapPos)
      {
         libNameMap.GetNextAssoc(libMapPos, libName, voidPtr);
    
         HTREEITEM libItem = m_symbolsTree.InsertItem(libName, SCH_ICON_FOLDER, SCH_ICON_FOLDER, TVI_ROOT, TVI_SORT);
         SetTreeItemData(&m_symbolsTree, libItem, libName, SCH_ITEM_UNKNOWN, NULL);
         
         CMapStringToPtr *cellMap = (CMapStringToPtr*)voidPtr;
         POSITION cellMapPos = cellMap->GetStartPosition();
         while (cellMapPos)
         {
            cellMap->GetNextAssoc(cellMapPos, cellName, voidPtr);
            CMapStringToPtr *clusterMap = (CMapStringToPtr*)voidPtr;

            HTREEITEM cellItem = m_symbolsTree.InsertItem(cellName, SCH_ICON_FOLDER, SCH_ICON_FOLDER, libItem, TVI_SORT);
            SetTreeItemData(&m_symbolsTree, cellItem, cellName, SCH_ITEM_UNKNOWN, NULL);

            POSITION clusterMapPos = clusterMap->GetStartPosition();
            while (clusterMapPos)
            {
               clusterMap->GetNextAssoc(clusterMapPos, clusterName, voidPtr);
               CMapStringToPtr *symbolNameMap = (CMapStringToPtr*)voidPtr;

               HTREEITEM clusterItem = m_symbolsTree.InsertItem(clusterName, SCH_ICON_FOLDER, SCH_ICON_FOLDER, cellItem, TVI_SORT);
               SetTreeItemData(&m_symbolsTree, clusterItem, clusterName, SCH_ITEM_UNKNOWN, NULL);
               
               POSITION symbolNamePos = symbolNameMap->GetStartPosition();
               while (symbolNamePos)
               {
                  CString blockName;
                  symbolNameMap->GetNextAssoc(symbolNamePos, blockName, voidPtr);
                  BlockStruct *block = (BlockStruct*)voidPtr;

                  if (!block)
                     continue;

                  int index = block->getName().ReverseFind(SCH_DELIMITER);
                  blockName = blockName.Right(block->getName().GetLength() - index - 1);

                  HTREEITEM symbolItem = m_symbolsTree.InsertItem(blockName, SCH_ICON_GATE, SCH_ICON_GATE, clusterItem, TVI_SORT);
                  SetTreeItemData(&m_symbolsTree, symbolItem, blockName, SCH_ITEM_SYMBOL, block);

                  symbolNameMap->RemoveKey(blockName);
               }

               symbolNameMap->RemoveAll();
               delete symbolNameMap;
               clusterMap->RemoveKey(clusterName);

               //Set the sort of the symbol under the cluster
               TVSORTCB pSort;
               pSort.hParent = cellItem;
               pSort.lParam = 0;
               pSort.lpfnCompare = TreeSortCompareFunc;
               m_symbolsTree.SortChildrenCB(&pSort);
            }
            clusterMap->RemoveAll();
            delete clusterMap;
            cellMap->RemoveKey(cellName);

            //Set the sort of the clusters under the cell
            TVSORTCB pSort;
            pSort.hParent = cellItem;
            pSort.lParam = 0;
            pSort.lpfnCompare = TreeSortCompareFunc;
            m_symbolsTree.SortChildrenCB(&pSort);
         }
         cellMap->RemoveAll();
         delete cellMap;
         libNameMap.RemoveKey(libName);

         //Set the sort of the cells under the librarry
         TVSORTCB pSort;
         pSort.hParent = libItem;
         pSort.lParam = 0;
         pSort.lpfnCompare = TreeSortCompareFunc;
         m_symbolsTree.SortChildrenCB(&pSort);
      }
      libNameMap.RemoveAll();

      //Set the sort of the library
      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_symbolsTree.SortChildrenCB(&pSort);
   }
   else
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);
         if (!block || block->getBlockType() != BLOCKTYPE_SYMBOL)
            continue;

         if (block->getAttributesRef())
         {
            Attrib *attrib = NULL;

            //lib name
            if (block->getAttributesRef()->Lookup(librarykeywordIndex, attrib))
            {
               if (attrib)
                  libName = attrib->getStringValue();
            }
            else
               continue;

            //cell name
            if (block->getAttributesRef()->Lookup(cellkeywordIndex, attrib))
            {
               if (attrib)
                  cellName = attrib->getStringValue();
            }
            else
               continue;

            //cluster name
            if (block->getAttributesRef()->Lookup(clusterkeywordIndex, attrib))
            {
               if (attrib)
                  clusterName = attrib->getStringValue();
            }
            else
               continue;
         }
         else
            continue;
         
         CMapStringToPtr *cellMap; 
         if (libNameMap.Lookup(libName, voidPtr))
            cellMap = (CMapStringToPtr*)voidPtr;
         else
         {
            cellMap = new CMapStringToPtr();
            libNameMap.SetAt(libName, cellMap);
         }

         CMapStringToPtr *clusterMap;
         if (cellMap->Lookup(cellName, voidPtr))   
            clusterMap = (CMapStringToPtr*)voidPtr;
         else
         {
            clusterMap = new CMapStringToPtr();
            cellMap->SetAt(cellName, clusterMap);
         }

         CMapStringToPtr *symbolName;
         if (clusterMap->Lookup(clusterName, voidPtr))
            symbolName = (CMapStringToPtr*)voidPtr;
         else
         {
            symbolName = new CMapStringToPtr();
            clusterMap->SetAt(clusterName, symbolName);
         }
         symbolName->SetAt(block->getName(), block);
      }

      POSITION libMapPos = libNameMap.GetStartPosition();
      while (libMapPos)
      {
         libNameMap.GetNextAssoc(libMapPos, libName, voidPtr);
    
         HTREEITEM libItem = m_symbolsTree.InsertItem(libName, SCH_ICON_FOLDER, SCH_ICON_FOLDER, TVI_ROOT, TVI_SORT);
         SetTreeItemData(&m_symbolsTree, libItem, libName, SCH_ITEM_UNKNOWN, NULL);
         
         CMapStringToPtr *cellMap = (CMapStringToPtr*)voidPtr;
         POSITION cellMapPos = cellMap->GetStartPosition();
         while (cellMapPos)
         {
            cellMap->GetNextAssoc(cellMapPos, cellName, voidPtr);
            CMapStringToPtr *clusterMap = (CMapStringToPtr*)voidPtr;

            HTREEITEM cellItem = m_symbolsTree.InsertItem(cellName, SCH_ICON_FOLDER, SCH_ICON_FOLDER, libItem, TVI_SORT);
            SetTreeItemData(&m_symbolsTree, cellItem, cellName, SCH_ITEM_UNKNOWN, NULL);

            POSITION clusterMapPos = clusterMap->GetStartPosition();
            while (clusterMapPos)
            {
               clusterMap->GetNextAssoc(clusterMapPos, clusterName, voidPtr);
               CMapStringToPtr *symbolNameMap = (CMapStringToPtr*)voidPtr;

               HTREEITEM clusterItem = m_symbolsTree.InsertItem(clusterName, SCH_ICON_FOLDER, SCH_ICON_FOLDER, cellItem, TVI_SORT);
               SetTreeItemData(&m_symbolsTree, clusterItem, clusterName, SCH_ITEM_UNKNOWN, NULL);
               
               POSITION symbolNamePos = symbolNameMap->GetStartPosition();
               while (symbolNamePos)
               {
                  CString blockName;
                  symbolNameMap->GetNextAssoc(symbolNamePos, blockName, voidPtr);
                  BlockStruct *block = (BlockStruct*)voidPtr;

                  if (!block)
                     continue;

                  int index = block->getName().ReverseFind(SCH_DELIMITER);
                  blockName = blockName.Right(block->getName().GetLength() - index - 1);

                  HTREEITEM symbolItem = m_symbolsTree.InsertItem(blockName, SCH_ICON_GATE, SCH_ICON_GATE, clusterItem, TVI_SORT);
                  SetTreeItemData(&m_symbolsTree, symbolItem, blockName, SCH_ITEM_SYMBOL, block);

                  symbolNameMap->RemoveKey(blockName);
               }

               symbolNameMap->RemoveAll();
               delete symbolNameMap;
               clusterMap->RemoveKey(clusterName);

               //Set the sort of the symbol under the cluster
               TVSORTCB pSort;
               pSort.hParent = cellItem;
               pSort.lParam = 0;
               pSort.lpfnCompare = TreeSortCompareFunc;
               m_symbolsTree.SortChildrenCB(&pSort);
            }
            clusterMap->RemoveAll();
            delete clusterMap;
            cellMap->RemoveKey(cellName);

            //Set the sort of the clusters under the cell
            TVSORTCB pSort;
            pSort.hParent = cellItem;
            pSort.lParam = 0;
            pSort.lpfnCompare = TreeSortCompareFunc;
            m_symbolsTree.SortChildrenCB(&pSort);
         }
         cellMap->RemoveAll();
         delete cellMap;
         libNameMap.RemoveKey(libName);

         //Set the sort of the cells under the librarry
         TVSORTCB pSort;
         pSort.hParent = libItem;
         pSort.lParam = 0;
         pSort.lpfnCompare = TreeSortCompareFunc;
         m_symbolsTree.SortChildrenCB(&pSort);
      }
      libNameMap.RemoveAll();

      //Set the sort of the library
      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_symbolsTree.SortChildrenCB(&pSort);
   } 
   m_symbolsTree.ShowWindow(SW_SHOW);
} 

/******************************************************************************
* CSchematicList::OnTvnSelchangedSheetTree
*/
void CSchematicList::OnTvnSelchangedSheetTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

   CWaitCursor wait;

   Reset();
   m_netsTree.SelectItem(NULL);
   m_netsTree.SelectDropTarget(NULL);
   m_instancesTree.SelectItem(NULL);
   m_instancesTree.SelectDropTarget(NULL);

   HTREEITEM item = m_sheetsTree.GetSelectedItem();
   
   if (!item) // nothing selected
      return; 

   int iconImage;
   m_sheetsTree.GetItemImage(item, iconImage, iconImage);
   TreeItemData *itemData = (TreeItemData*)m_sheetsTree.GetItemData(item);

   if (iconImage == SCH_ICON_HIERARCHY)
   {
      DataStruct *hierarchyData = (DataStruct*)itemData->voidPtr;
      if (!hierarchyData)
         return;

      HTREEITEM sheetItem = m_sheetsTree.GetParentItem(item);
      TreeItemData *sheetItemData = (TreeItemData*)m_sheetsTree.GetItemData(sheetItem);
      BlockStruct *block = (BlockStruct*)sheetItemData->voidPtr;

      // if not current page, selected hierarchy symbol is on different page
      if (currentViewFile->getBlock() !=  block) 
      {
         m_sheetsTree.SelectItem(sheetItem);
         m_sheetsTree.Select(sheetItem, TVGN_DROPHILITE);
      }

      // scrolls into view, or redraws the item
      m_sheetsTree.Select(item, TVGN_DROPHILITE);

      //zoom to selected instance and select it!
      if (!view)
         return;

      UpdateData();

      double xmin, xmax, ymin, ymax;
      CString m_ref = hierarchyData->getInsert()->getRefname();
      if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
      {
         PanReference(((CCEtoODBView*)view), m_ref);
         ErrorMessage("Component has no extents", "Panning");
         return;
      }
      
      double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
      double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;

      double marginSize = max(marginSizeX, marginSizeY);

      xmin -= marginSize;
      xmax += marginSize;
      
      ymin -= marginSize;
      ymax += marginSize;     

      ((CCEtoODBView*)view)->ZoomBox(xmin, xmax, ymin, ymax);
   }
   else if (iconImage == SCH_ICON_SHEET)
   {
      BlockStruct *block = (BlockStruct*)itemData->voidPtr;

      if (!block || block->getBlockType() != BLOCKTYPE_SHEET)
         return;

      FileStruct *viewFile = doc->Find_File(block->getFileNumber());
      if (viewFile)
            viewFile->setBlock(block);

      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (!file)
            continue;

         file->setShow(file->getFileNumber() == block->getFileNumber());
      }
      
      doc->OnFitPageToImage();

      m_sheetsTree.Select(item, TVGN_DROPHILITE);

      BOOL selBlockIsOnDifferentFile = FALSE;
      if (currentViewFile != viewFile)
      {
         currentViewFile = viewFile;
         FillInstTree();
         FillSymbolsTree();
         selBlockIsOnDifferentFile = TRUE;
      }
      
      if (m_cpSymbols && !selBlockIsOnDifferentFile)
         FillSymbolsTree();
      if (m_cpInst && !selBlockIsOnDifferentFile)
         FillInstTree();
   }
	else if (iconImage == SCH_ICON_FOLDER)
	{
      // Expaned the selected net
      m_sheetsTree.Expand(item, TVE_EXPAND);
	}
   else if (IsItemAFile(itemData->voidPtr, doc))
   {
      FileStruct *viewFile = (FileStruct*)itemData->voidPtr;
      if (currentViewFile != viewFile)
      {
         currentViewFile = viewFile;

         POSITION filePos = doc->getFileList().GetHeadPosition();
         while (filePos)
         {
            FileStruct *file = doc->getFileList().GetNext(filePos);

            if (!file)
               continue;

            file->setShow(file == currentViewFile);
         }

         doc->OnFitPageToImage();
         m_sheetsTree.Select(item, TVGN_DROPHILITE);
         
         m_instancesTree.SelectItem(NULL);
         m_instancesTree.SelectDropTarget(NULL);
         m_instancesTree.ShowWindow(SW_HIDE);
         m_instancesTree.DeleteAllItems();
         m_instancesTree.ShowWindow(SW_SHOW);
         m_symbolsTree.SelectItem(NULL);
         m_symbolsTree.SelectDropTarget(NULL);
         m_symbolsTree.ShowWindow(SW_HIDE);
         m_symbolsTree.DeleteAllItems();
         m_symbolsTree.ShowWindow(SW_SHOW);
      }
      else
      {
         m_sheetsTree.Select(item, TVGN_DROPHILITE);

         m_instancesTree.SelectItem(NULL);
         m_instancesTree.SelectDropTarget(NULL);
         m_instancesTree.ShowWindow(SW_HIDE);
         m_instancesTree.DeleteAllItems();
         m_instancesTree.ShowWindow(SW_SHOW);

         m_symbolsTree.SelectItem(NULL);
         m_symbolsTree.SelectDropTarget(NULL);
         m_symbolsTree.ShowWindow(SW_HIDE);
         m_symbolsTree.DeleteAllItems();
         m_symbolsTree.ShowWindow(SW_SHOW);
      }
   }

   *pResult = 0;
}

/******************************************************************************
* IsItemAFile
This function checks if the item selected from the sheet tree is a file(view) or a block(sheet).
*/
BOOL IsItemAFile(void *selSheetItem, CCEtoODBDoc *document)
{
   POSITION filePos = document->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = document->getFileList().GetNext(filePos);

      if (file && selSheetItem == (void*)file)
         return TRUE;
   }
   return FALSE;

}

/******************************************************************************
* CSchematicList::OnBnClickedCpInst
*/
void CSchematicList::OnBnClickedCpInst()
{
   CWaitCursor wait;

   FillInstTree();
}

/******************************************************************************
* CSchematicList::OnTvnSelchangedInstTree
*/
void CSchematicList::OnTvnSelchangedInstTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   *pResult = 0;
   CWaitCursor wait;

   HTREEITEM item = m_instancesTree.GetSelectedItem();   
   if (!item) // nothing selected
      return; 

   Reset();
   m_netsTree.SelectItem(NULL);
   m_netsTree.SelectDropTarget(NULL);

   TreeItemData *itemData = (TreeItemData*)m_instancesTree.GetItemData(item);
   DataStruct *instData = (DataStruct*)itemData->voidPtr;

   if (!instData || instData->getDataType() != dataTypeInsert)
      return;

   FileStruct *currentFile = currentViewFile;
   if (!m_cpInst) // if not current page only, selected instance might be in a different file
   {
      BOOL dataFound = FALSE;
      for (int i=0; i<doc->getMaxBlockIndex() && !dataFound; i++)
      {
         BlockStruct *block = doc->getBlockAt(i);
         if (!block || block->getBlockType() != blockTypeSheet)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos && !dataFound)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);            
            if (data && data == instData)
            {
               dataFound = TRUE;
               FileStruct *file = doc->Find_File(block->getFileNumber());
               SelectSheet(block);
               currentFile = file;
               break;
            }
         }

         if (dataFound)
            break;
      }
   }
   else
   {
      // Since the same sheet, then just check to see if the sheet is being selected
      // or is its child (hierarchy symbol) is being selected
      HTREEITEM item = m_sheetsTree.GetSelectedItem();

      int iconImage;
      m_sheetsTree.GetItemImage(item, iconImage, iconImage);

      // If the selected item in Sheet tree is an hierarchy item then get the parent sheet
      // else it is it already the parent sheet
      if (iconImage == SCH_ICON_HIERARCHY)
      {
         item = m_sheetsTree.GetParentItem(item);
         m_sheetsTree.SelectItem(item);
         m_sheetsTree.Select(item, TVGN_DROPHILITE);
      }

      currentFile = currentViewFile;
   }
   
   m_instancesTree.SelectItem(item);
   m_instancesTree.Select(item, TVGN_DROPHILITE);
   
   
	//zoom to selected instance and select it!   
   double xmin, xmax, ymin, ymax;

	InsertStruct *insert = instData->getInsert();
	if (insert == NULL)
		return;

   if (insert->getInsertType() == insertTypeSymbol || insert->getInsertType() == insertTypeHierarchicalSymbol)
   {
      if (!view)
         return;

      UpdateData();

      CString m_ref = instData->getInsert()->getRefname();
      if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
      {
         PanReference(((CCEtoODBView*)view), m_ref);
         ErrorMessage("Component has no extents", "Panning");
         return;
      }
      
      double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
      double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;

      double marginSize = max(marginSizeX, marginSizeY);

      xmin -= marginSize;
      xmax += marginSize;
      
      ymin -= marginSize;
      ymax += marginSize;     


      ((CCEtoODBView*)view)->ZoomBox(xmin, xmax, ymin, ymax);
   }
   else if (insert->getInsertType() == insertTypePortInstance)
   {
      Mat2x2 m;
      RotMat2(&m, currentFile->getRotation());
      Point2 point2;
      point2.x = instData->getInsert()->getOriginX() * currentFile->getScale();
      point2.y = instData->getInsert()->getOriginY() * currentFile->getScale();
      ((CCEtoODBView*)view)->PanCoordinate(point2.x, point2.y, TRUE);
   }

   m_netsTree.SelectItem(NULL);
   m_netsTree.SelectDropTarget(NULL);
}

/******************************************************************************
* CSchematicList::OnBnClickedCpNets
*/
void CSchematicList::OnBnClickedCpNets()
{
}

/******************************************************************************
* CSchematicList::OnTvnSelchangedNetsTree
*/
void CSchematicList::OnTvnSelchangedNetTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   CWaitCursor wait;

   HTREEITEM item = m_netsTree.GetSelectedItem();
   if (!item) // nothing selected
      return; 

   m_instancesTree.SelectItem(NULL);
   m_instancesTree.SelectDropTarget(NULL);
   
   TreeItemData *itemData = (TreeItemData*)m_netsTree.GetItemData(item);
	if (itemData == NULL)
		return;
		
	if (itemData->itemType == SCH_ITEM_NETS || itemData->itemType == SCH_ITEM_BUSSES)
	{
		// NETS folder
      Reset();

		// Expaned the the folder
		m_netsTree.Expand(item, TVE_EXPAND);

		// Select the net sheet in under the NETS folder
		m_netsTree.SelectItem(item);
		m_netsTree.Select(item, TVGN_DROPHILITE);
	}
	else if (itemData->itemType == SCH_ITEM_NET)
	{
		// Individual Net item under the NETS folder
      Reset();

		NetStruct *net = (NetStruct*)itemData->voidPtr;
		if (net)
		{
			CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
			int value = doc->RegisterValue(net->getNetName());
			doc->HighlightedNetsMap.SetAt(value, pApp->schSettings.SchHightlightColor);
			doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);
			HightLightParentBus(net->getNetName());
		}
	   
		// Expand each node(sheet) under the selected net
		HTREEITEM firstSheetItem = m_netsTree.GetChildItem(item);
		HTREEITEM sheetItem = firstSheetItem;
		while (sheetItem)
		{
			m_netsTree.Expand(sheetItem, TVE_EXPAND);
			sheetItem = m_netsTree.GetNextSiblingItem(sheetItem);
		}

		// Expaned the selected net
		m_netsTree.Expand(item, TVE_EXPAND);

		// Select the first sheet in under the net
		m_netsTree.SelectItem(item);
		m_netsTree.Select(item, TVGN_DROPHILITE);
	}
	else if (itemData->itemType == SCH_ITEM_BUS)
	{
		// Individual Bus item under the BUSSES folder
      Reset();

		CBusStruct *bus = (CBusStruct*)itemData->voidPtr;
		if (bus != NULL)
		{
			CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
			int value = doc->RegisterValue(bus->GetName());
			doc->HighlightedNetsMap.SetAt(value, pApp->schSettings.SchBusHightlightColor);
			doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);
			HightLightParentBus(bus->GetName());
		}

		// Expaned the selected net
		m_netsTree.Expand(item, TVE_EXPAND);

		// Select the first sheet in under the net
		m_netsTree.SelectItem(item);
		m_netsTree.Select(item, TVGN_DROPHILITE);
	}
	else if (itemData->itemType == SCH_ITEM_SHEET)
	{
		// Sheet item under the Net item
      BlockStruct *block = (BlockStruct*)itemData->voidPtr;
		if (block != NULL)
			SelectSheet(block);

		// Expaned the selected item
		m_netsTree.Expand(item, TVE_EXPAND);

      m_netsTree.SelectItem(item);
      m_netsTree.Select(item, TVGN_DROPHILITE);
      HTREEITEM parentItem = m_netsTree.GetParentItem(item);
      TreeItemData *parentItemData = (TreeItemData*)m_netsTree.GetItemData(parentItem);

		if (parentItemData->itemType == SCH_ITEM_SHEET)
		{
			Reset();
			
			// Get the Bus Item
			parentItem = m_netsTree.GetParentItem(parentItem);
		   parentItemData = (TreeItemData*)m_netsTree.GetItemData(parentItem);
			if (parentItemData->itemType == SCH_ITEM_BUS)
			{

				// Show the selected bus on this sheet
				CBusStruct *bus = (CBusStruct*)parentItemData->voidPtr;
				if (bus != NULL)
				{
					double left, right, top, bottom;
					if (!NetExtents(doc, bus->GetName(), &left, &right, &bottom, &top))
					{
						ErrorMessage("Bus has no extents", "Panning to Bus");
						return;
					}

					CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
					int value = doc->RegisterValue(bus->GetName());
					doc->HighlightedNetsMap.SetAt(value, pApp->schSettings.SchBusHightlightColor);
					doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);

					double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
					double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
					double marginSize = max(marginSizeX, marginSizeY);

					left -= marginSize;
					right += marginSize;
					bottom -= marginSize;
					top += marginSize;

					((CCEtoODBView*)view)->ZoomBox(left, right, bottom, top);
					HightLightParentBus(bus->GetName());
				}
			}
		}
		else if (parentItemData->itemType == SCH_ITEM_NET)
		{
			Reset();

			// Show the selected net on this sheet
			NetStruct *net = (NetStruct*)parentItemData->voidPtr;
			if (net != NULL)
			{
				// Make sure there is an extent before zooming
				double left, right, top, bottom;
				if (!NetExtents(doc, net->getNetName(), &left, &right, &bottom, &top))
				{
					ErrorMessage("Net has no extents", "Panning to Net");
					return;
				}

				CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
				int value = doc->RegisterValue(net->getNetName());
				doc->HighlightedNetsMap.SetAt(value, pApp->schSettings.SchHightlightColor);
				doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);

				double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
				double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
				double marginSize = max(marginSizeX, marginSizeY);

				left -= marginSize;
				right += marginSize;
				bottom -= marginSize;
				top += marginSize;

				((CCEtoODBView*)view)->ZoomBox(left, right, bottom, top);
				HightLightParentBus(net->getNetName());
			}
		}
	}
	else if (itemData->itemType == SCH_ITEM_COMPPIN)
	{
		// Comppin item under the Sheet item
      doc->UnselectAll(FALSE);
      doc->OnDoneEditing();
      double xmin, xmax, ymin, ymax;

      TreeItemData *itemData = (TreeItemData*)m_netsTree.GetItemData(item);
      CompPinStruct * compPin = (CompPinStruct*)itemData->voidPtr;
      
      if (!view)
         return;

      UpdateData();

      if (!IsCompPinOnThisSheet(doc, this->desigkeywordIndex, compPin, currentViewFile->getBlock()))
      {
         TreeItemData *itemData = (TreeItemData*)m_netsTree.GetItemData(m_netsTree.GetParentItem(item));
         SelectSheet((BlockStruct*)itemData->voidPtr);
      }
      else
      {
         HTREEITEM item = m_sheetsTree.GetSelectedItem();

         int iconImage;
         m_sheetsTree.GetItemImage(item, iconImage, iconImage);

         // If the selected item in Sheet tree is an hierarchy item then get the parent sheet
         // else it is it already the parent sheet
         if (iconImage == SCH_ICON_HIERARCHY)
         {
            item = m_sheetsTree.GetParentItem(item);
            m_sheetsTree.SelectItem(item);
            m_sheetsTree.Select(item, TVGN_DROPHILITE);
         }
      }

      m_netsTree.SelectItem(item);
      m_netsTree.Select(item, TVGN_DROPHILITE);

      TreeItemData *netItemData = (TreeItemData*)m_netsTree.GetItemData(m_netsTree.GetParentItem(item));
      NetStruct *net = (NetStruct*)netItemData->voidPtr;

      CString m_ref = compPin->getRefDes();
      if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
      {
         PanReference(((CCEtoODBView*)view), m_ref);
         ErrorMessage("Component has no extents", "Panning to Component");
         return;
      }
      double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;
      double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.SchMargin / 100;

      double marginSize = max(marginSizeX, marginSizeY);

      xmin -= marginSize;
      xmax += marginSize;
      
      ymin -= marginSize;
      ymax += marginSize;


      ((CCEtoODBView*)view)->ZoomBox(xmin, xmax, ymin, ymax);
	}

   *pResult = 0;
}


/******************************************************************************
* CSchematicList::OnBnClickedCpSymbols
*/
void CSchematicList::OnBnClickedCpSymbols()
{
   FillSymbolsTree();
}

/******************************************************************************
* CSchematicList::OnNMDblclkSymbolTree
*/
void CSchematicList::OnNMDblclkSymbolTree(NMHDR *pNMHDR, LRESULT *pResult)
{

   HTREEITEM item = m_symbolsTree.GetSelectedItem();
   
   if (!item) // nothing selected
      return; 

   if (m_symbolsTree.GetChildItem(item))
      return;

   m_instancesTree.SelectItem(NULL);

   UnHighLightNets();

   TreeItemData *itemData = (TreeItemData*)m_symbolsTree.GetItemData(item);
   BlockStruct *symbolBlock = (BlockStruct*)itemData->voidPtr;

   if (symbolBlock)
   {
      EditGeometryAndShowDlg(doc, symbolBlock);
      doc->OnRegen();
   }

   m_netsTree.SelectItem(NULL);

   *pResult = 0;
}

/******************************************************************************
* CSchematicList::UnHighLightNets
*/
void CSchematicList::UnHighLightNets()
{
   doc->HighlightedNetsMap.RemoveAll();
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file)
         continue;

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         file->getNetList().GetNext(netPos)->setHighlighted(false);
      }
   }

   doc->UpdateAllViews(NULL);
}

/******************************************************************************
* CSchematicList::Reset
*/
void CSchematicList::Reset()
{
   UnHighLightNets();
   doc->UnselectAll(FALSE);
   doc->OnDoneEditing();
}

/******************************************************************************
* IsCompPinOnThisSheet
*/
BOOL IsCompPinOnThisSheet(CCEtoODBDoc *doc, WORD designKeywordIndex, CompPinStruct *compPin, BlockStruct *block)
{
   WORD compKeywordIndex = doc->IsKeyWord(SCH_ATT_COMPDESIGNATOR, 0);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      if (!data || !data->getInsert() || data->getInsert()->getInsertType() != insertTypeSymbol)
         continue;
      if (data->getInsert()->getRefname() != compPin->getRefDes())
         continue;

      Attrib *attrib = NULL;
      CString designator = "";
      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(designKeywordIndex, attrib))
      {
         if (attrib)
            designator = attrib->getStringValue();
      }

      CString compRefDes = "";
      if (compPin->getAttributesRef() && compPin->getAttributesRef()->Lookup(compKeywordIndex, attrib))
      {
         if (attrib)
            compRefDes = attrib->getStringValue();
      }

      if (compRefDes.IsEmpty() || !compRefDes.CompareNoCase(designator))
         return TRUE;
   }
   return FALSE;
}

/******************************************************************************
* CSchematicList::InstSelectionChanged
*
* This function is called when a gate (component) gets selected from the view.
*/
void CSchematicList::InstSelectionChanged(DataStruct *instData)
{
	if (!instData && instData->getDataType() != dataTypeInsert)
		return;

   if (instData->getInsert()->getInsertType() == insertTypeHierarchicalSymbol ||
		 instData->getInsert()->getInsertType() == insertTypeSymbol)
   {
      Attrib* attrib;
      WORD geomNumKeywordIndex = doc->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);

      if (instData->getAttributesRef() && instData->getAttributesRef()->Lookup(geomNumKeywordIndex, attrib))
		{
			// !!!MUST!!! First select hierarchy symbol in the instance tree
         SelectInstance(instData);

			// Then select hierarchy symbol in the sheet tree
         SelectHierarchySymbol(instData);
		}
      else
		{
         SelectInstance(instData);
		}
   }
}  

/******************************************************************************
* CSchematicList::NetSelectionChanged
*
*  This function is called when a net gets selected from the view.
*/
void CSchematicList::NetSelectionChanged(NetStruct *net)
{
   if (net)
      SelectNet(net);
}

/******************************************************************************
* CSchematicList::BusSelectionChanged
*/
void CSchematicList::BusSelectionChanged(CBusStruct *bus)
{
   if (bus)
      SelectBus(bus);
}

/******************************************************************************
* CSchematicList::SheetChanged
*
* This function is called when sheets changes from outside the schematic navigator.
*/
void CSchematicList::SheetChanged(BlockStruct *sheetBlock)
{
   if (sheetBlock)
      SelectSheet(sheetBlock);
}

/******************************************************************************
* CSchematicList::OnBnClickedSchSheetsChck
*
* This function calls the OnSize() routine to resize the Schematic Navigator Dialog.
* It gets called when the Sheets check box is clicked.
*/
void CSchematicList::OnBnClickedSchSheetsChck()
{
   UpdateData(true);

   CRect clientRect;
   GetClientRect(&clientRect);
   OnSize(SIZE_RESTORED,clientRect.Width(),clientRect.Height());
}

/******************************************************************************
* CSchematicList::OnBnClickedSchInstChck
*
* This function calls the OnSize() routine to resize the Schematic Navigator Dialog.
* It gets called when the Instances check box is clicked.
*/
void CSchematicList::OnBnClickedSchInstChck()
{
   UpdateData();

   CRect clientRect;
   GetClientRect(&clientRect);
   OnSize(SIZE_RESTORED,clientRect.Width(),clientRect.Height());
}

/******************************************************************************
* CSchematicList::OnBnClickedSchNetsChck
*
* This function calls the OnSize() routine to resize the Schematic Navigator Dialog.
* It gets called when the Nets check box is clicked.
*/
void CSchematicList::OnBnClickedSchNetsChck()
{
   UpdateData();

   CRect clientRect;
   GetClientRect(&clientRect);
   OnSize(SIZE_RESTORED,clientRect.Width(),clientRect.Height());
}

/******************************************************************************
* CSchematicList::OnBnClickedSchSymChck
*
* This function calls the OnSize() routine to resize the Schematic Navigator Dialog.
* It gets called when the Symbols check box is clicked.
*/
void CSchematicList::OnBnClickedSchSymChck()
{
   UpdateData();

   CRect clientRect;
   GetClientRect(&clientRect);
   OnSize(SIZE_RESTORED,clientRect.Width(),clientRect.Height());
}

void CSchematicList::deferWindowPos(CResizingDialogField* field,int dYpos,int newHeight,int dx) 
{
   CRect fieldRect = field->getOriginalRect();

   if (newHeight < 0)
   {
      newHeight = fieldRect.bottom - fieldRect.top;
   }

   fieldRect = field->getOriginalRect();
   fieldRect.top   += dYpos;
   fieldRect.bottom = fieldRect.top + newHeight;
   fieldRect.right += dx;

   CWnd* wnd = GetDlgItem(field->getControlId());

   field->setRect(fieldRect);

   m_hDwp = ::DeferWindowPos(m_hDwp,*wnd,0,fieldRect.left,fieldRect.top,
      fieldRect.Width(),fieldRect.Height(),SWP_NOZORDER);
}

/******************************************************************************
* CSchematicList::OnSize
	Set the dialog controls new position and size
*/
void CSchematicList::OnSize(UINT nType, int cx, int cy) 
{
   if (m_originalClientSize.cx == 0 && m_originalClientSize.cy == 0)
   {
      m_originalClientSize = CSize(cx,cy);
   }

   CRect currentWindowRect;
   CSize currentWindowSize;

   if (::IsWindow(*this))
   {
      GetWindowRect(&currentWindowRect);
      currentWindowSize = currentWindowRect.Size();
   }

   CDialog::OnSize(nType, cx, cy);
   
   if (nType == SIZE_MINIMIZED)
   {
      return;
   }

   if (m_initializedWindowDataFlag)
   {
      int dx  = cx - m_originalClientSize.cx;
      int dy  = cy - m_originalClientSize.cy;
      int dx2 = cx - m_previousClientSize.cx;
      int dy2 = cy - m_previousClientSize.cy;
      double fx = (double)(currentWindowSize.cx) / m_originalWindowSize.cx;
      double fy = (double)(currentWindowSize.cy) / m_originalWindowSize.cy;

      //TRACE("dx = %d, dy = %d, currentWindowSize(%d,%d)\n",
      //   dx,dy,currentWindowSize.cx,currentWindowSize.cy);

      bool drawGripper = (m_drawGripper && ::IsWindow(m_gripper));

      m_hDwp = ::BeginDeferWindowPos(m_fields.GetCount() + (drawGripper ? 1 : 0));

      // Move and Size the controls using the information
      // we got in SetControlInfo()
      //
      m_stateChanged = true;
      CRect WndRect;

      if (drawGripper)
      {
         m_gripper.GetWindowRect(&WndRect);  
         ScreenToClient(&WndRect);
         
         WndRect.OffsetRect(dx2,dy2);

         m_hDwp = ::DeferWindowPos(m_hDwp,m_gripper,0,WndRect.left,WndRect.top,WndRect.Width(),WndRect.Height(),SWP_NOZORDER);
      }

      int originalSheetsHeight    = m_sheetsField->getOriginalRect().Height();
      int originalInstancesHeight = m_instancesField->getOriginalRect().Height();
      int originalNetsHeight      = m_netsField->getOriginalRect().Height();
      int originalSymbolsHeight   = m_symbolsField->getOriginalRect().Height();

      int totalOriginalTreeControlsHeight = originalSheetsHeight + originalInstancesHeight +
                                            originalNetsHeight   + originalSymbolsHeight     ;

      int totalOriginalEnabledTreeControlHeight =
         (m_onOffSheets    ? originalSheetsHeight    : 0) + 
         (m_onOffInstances ? originalInstancesHeight : 0) + 
         (m_onOffNets      ? originalNetsHeight      : 0) + 
         (m_onOffSymbols   ? originalSymbolsHeight   : 0);

      int totalOriginalNonTreeControlHeight = m_originalClientSize.cy - totalOriginalTreeControlsHeight;
      int totalNewTreeControlHeight = cy - totalOriginalNonTreeControlHeight;
      double factor = ((double)totalNewTreeControlHeight) / totalOriginalEnabledTreeControlHeight;

      int newSheetsHeight    = (int)((m_onOffSheets    ? originalSheetsHeight    : 0) * factor + .5);
      int newInstancesHeight = (int)((m_onOffInstances ? originalInstancesHeight : 0) * factor + .5);
      int newNetsHeight      = (int)((m_onOffNets      ? originalNetsHeight      : 0) * factor + .5);
      int newSymbolsHeight   = (int)((m_onOffSymbols   ? originalSymbolsHeight   : 0) * factor + .5);

      int originalYpos = m_sheetsField->getOriginalRect().top;
      int newYpos      = originalYpos;
      CRect fieldRect;

      // section 1
      deferWindowPos(m_sheetsField,newYpos - originalYpos,newSheetsHeight,dx);

      originalYpos += originalSheetsHeight;
      newYpos      += newSheetsHeight;

      // section 2
      deferWindowPos(m_instancesCheckBoxField,newYpos - originalYpos,-1,0);
      deferWindowPos(m_instancesCurrentSheetOnlyCheckBoxField,newYpos - originalYpos,-1,0);
      deferWindowPos(m_instancesField,newYpos - originalYpos,newInstancesHeight,dx);

      originalYpos += originalInstancesHeight;
      newYpos      += newInstancesHeight;

      // section 3
      deferWindowPos(m_netsCheckBoxField,newYpos - originalYpos,-1,0);
      deferWindowPos(m_netsField,newYpos - originalYpos,newNetsHeight,dx);

      originalYpos += originalNetsHeight;
      newYpos      += newNetsHeight;

      // section 4
      deferWindowPos(m_symbolsCheckBoxField,newYpos - originalYpos,-1,0);
      deferWindowPos(m_symbolsCurrentSheetOnlyCheckBoxField,newYpos - originalYpos,-1,0);
      deferWindowPos(m_symbolsField,newYpos - originalYpos,newSymbolsHeight,dx);

      ::EndDeferWindowPos(m_hDwp);
   }

   m_previousClientSize = CSize(cx,cy);
}

