
#if !defined(__PanelizerDialog_h__)
#define __PanelizerDialog_h__

#pragma once

#include "ResizingDialog.h"
#include "Panelizer.h"
#include "Resource.h"
#include "afxwin.h"
#include "UltimateGrid.h"
#include "DcaCollections.h"

class CPanelEntityGrid;

//_____________________________________________________________________________
template < typename EnumType >
class CEnumLabelString
{
private:
   int m_min;
   int m_max;
   int m_index;
   CString (*m_toStringFunction)(EnumType);

public:
   CEnumLabelString(EnumType min,EnumType max,CString (*toStringFunction)(EnumType));

   //bool hasNext();

   //EnumType getNext();

   CString getLabelString();
};

//_____________________________________________________________________________
template <typename EnumType>
CEnumLabelString<EnumType>::CEnumLabelString(EnumType min,EnumType max,CString (*toStringFunction)(EnumType))
: m_min(min)
, m_max(max)
, m_toStringFunction(toStringFunction)
, m_index(min)
{
}

//template <typename EnumType>
//bool CEnumLabelString<EnumType>::hasNext()
//{
//   return (m_index <= m_max);
//}
//
//template <typename EnumType>
//EnumType CEnumLabelString<EnumType>::getNext()
//{
//   return (EnumType)(m_index++);
//}

template <typename EnumType>
CString CEnumLabelString<EnumType>::getLabelString()
{
   CString labelString;

   for (int index = m_min;index <= m_max;index++)
   {
      labelString += (*m_toStringFunction)((EnumType)index) + "\n";
   }

   return labelString;
}

//_____________________________________________________________________________
#define EnumLabelString(tagType,variable) CEnumLabelString<tagType> variable(tagType##Min,tagType##Max,tagType##ToString)

//enum PanelEntitySheetCellTypeTag
//{
//   panelEntitySheetCellTypeNumber,
//   panelEntitySheetCellTypeString,
//   panelEntitySheetCellTypeList,
//};

//_________________________________________________________________________________________________
class CPanelEntityGridMultiSelect : public CUGMultiSelect
{
public:
   void filterInColFromSelection(int col);
};

//_________________________________________________________________________________________________
enum PanelEntitySelectionStateTag
{
   panelEntitySelectionStateEmpty,
   panelEntitySelectionStateRows,
   panelEntitySelectionStateColumn,
};

//_________________________________________________________________________________________________
class CPanelEntityGridSelectionController
{
private:
   CPanelEntityGrid& m_panelEntityGrid;
   PanelEntitySelectionStateTag m_state;
   bool m_currentlyProcessingFlag;
   bool m_ctrlKeyDown;
   bool m_shiftKeyDown;
   int m_lastClickedRow;
   int m_lastFocusedCol;
   int m_highlightedRow;

public:
   CPanelEntityGridSelectionController(CPanelEntityGrid& panelEntityGrid);
   void reset();

   PanelEntitySelectionStateTag getSelectionState() const;
   bool isRowSelected(int row) const;
   int getSelectedRowCount() const;
   int getHighlightedRow() const;

   void OnSH_LClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed);
   void OnSH_RClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed);
	void OnSelectionChanged(int startCol,long startRow,int endCol,long endRow,int blockNum);
   void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);
   void OnLClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,int processed);

private:
   void selectRow(long row);
   void selectRows(long rowStart,long rowEnd);
   void clearSelections();
   //void selectCell(int col,long row);
   void clearRowHighlights();
   void highlightRow(long row);
   void drawRowHighlight(long row);
   void undrawRowHighlight(long row);

};

//_________________________________________________________________________________________________
class CPanelEntityClipBoard
{
private:
   CPanelTemplate m_panelTemplate; // clip board buffer

public:
   CPanelEntityClipBoard(CPanelizer& panelizer);

   CPanelTemplate& getPanelTemplate();

   int getCount(PanelEntityTypeTag entityType);
   void empty();
   void addClipItem(const CPanelTemplateEntity& templateEntity);
   //void addClipItem(const CPanelTemplateEntityOutline&     templateEntityOutline);
   //void addClipItem(const CPanelTemplateEntityPcbInstance& templateEntityPcbInstance);
   //void addClipItem(const CPanelTemplateEntityPcbArray&    templateEntityPcbArray);
   //void addClipItem(const CPanelTemplateEntityFiducial&    templateEntityFiducial);
   //void addClipItem(const CPanelTemplateEntityToolingHole& templateEntityToolingHole);
};

//_________________________________________________________________________________________________
class CPanelEntityGridDropListType : public CUGDropListType
{
private:
   COLORREF m_buttonColor;

public:
   CPanelEntityGridDropListType(COLORREF buttonColor);

   virtual COLORREF GetButtonColor();
};

//_________________________________________________________________________________________________
class CPanelEntitySheet
{
private:
   CPanelEntityGrid& m_panelEntityGrid;

   int m_index;
   CString m_name;
   COLORREF m_cellBackgroundColor;
   CStringArray m_columnNames;
   CPanelEntityGridMultiSelect* m_multiSelect;
   CPanelEntityGridDropListType* m_dropListType;
   int m_dropListTypeId;
   bool m_validFlag;

public:
   CPanelEntitySheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor);
   ~CPanelEntitySheet();
   void init();

   CPanelEntityGrid& getPanelEntityGrid();
   CPanelizer& getPanelizer();
   CPanelEntityGridSelectionController& getSelectionController();
   CPanelEntityClipBoard& getClipBoard();
   CPanelEntityGridMultiSelect* getMultiSelect();
   int getDropListTypeId() const;

   virtual PanelEntityTypeTag getSheetType() const = 0;
   void deleteAllRows();

   virtual void fillGrid();
   virtual void fillInvalidGrid();

   int getIndex() const;

   CString getName() const;

   COLORREF getCellBackgroundColor()    const;
   COLORREF getTabBackgroundColor()     const;
   COLORREF getDefaultBackgroundColor() const;
   COLORREF getHeadingBackgroundColor() const;
   COLORREF getHighlightColor()         const;

   void setColumnTypeNumeric(int columnIndex);
   void setColumnTypeText(int columnIndex);
   void setColumnTypeList(int columnIndex,const CString& valueList);

   virtual void invalidate();
   virtual void invalidateBoardMapping();
   virtual int getBoardLogicalNameColumn() const;
   virtual int getBoardPhysicalNameColumn() const;
   virtual CString booleanLabelString() const;

   virtual CPanelTemplateEntityArray& getEntityArray() = 0;
   virtual int onMenuStart(int row,long col,int section);
   virtual int startMenu(const CString& entityDescription);
   virtual void onMenuCommand(int row,long col,int section,int item);
   virtual int addMenuItem(int itemId,const char* format,...);
   virtual void emptyClipboard();
   virtual bool canAddRow();

   // operations
   virtual int addColumn(const CString& columnName);
   //virtual int addNumericColumn(const CString& columnName);
   //virtual int addTextColumn(const CString& columnName);
   //virtual int addListColumn(const CString& columnName,const CString& valueList);
   virtual bool onEditVerify(int col,long row,CWnd* edit,UINT* vcKey);
   virtual bool onEditFinish(int col,long row,CWnd* edit,CString& string,BOOL cancelFlag);
   virtual bool onEditStart(int col,long row,CWnd** edit);
   virtual void updateRow(long row);

   virtual void clearEntityHighlights();
   virtual void highlightEntityFromRow(int row);
   virtual void highlightEntitiesFromRows(int rowStart,int rowEnd);
};

//_________________________________________________________________________________________________
class CPanelEntityOutlineSheet : public CPanelEntitySheet
{
private:
   int m_xOriginColumnIndex;
   int m_yOriginColumnIndex;
   int m_xSizeColumnIndex;
   int m_ySizeColumnIndex;

public:
   CPanelEntityOutlineSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor);

   virtual PanelEntityTypeTag getSheetType() const;
   virtual CPanelTemplateEntityArray& getEntityArray();
   virtual int onMenuStart(int row,long col,int section);
   //virtual void onMenuCommand(int row,long col,int section,int item);
   virtual bool canAddRow();

   // operations
   //virtual void updateRow(long row);
};

//_________________________________________________________________________________________________
class CPanelEntityPcbInstanceSheet : public CPanelEntitySheet
{
private:
   int m_nameColumnIndex;
   int m_xOriginColumnIndex;   
   int m_yOriginColumnIndex;   
   int m_angleColumnIndex;   
   int m_mirroredColumnIndex;
   int m_originTypeColumnIndex;
   int m_physicalNameColumnIndex;

public:
   CPanelEntityPcbInstanceSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor);

   virtual void invalidateBoardMapping();
   virtual int getBoardLogicalNameColumn() const;
   virtual int getBoardPhysicalNameColumn() const;

   virtual PanelEntityTypeTag getSheetType() const;
   virtual CPanelTemplateEntityArray& getEntityArray();
   virtual int onMenuStart(int row,long col,int section);
   //virtual void onMenuCommand(int row,long col,int section,int item);

   // operations
   //virtual void updateRow(long row);
};

//_________________________________________________________________________________________________
class CPanelEntityPcbArraySheet : public CPanelEntitySheet
{
private:
   int m_nameColumnIndex;
   int m_xOriginColumnIndex;
   int m_yOriginColumnIndex;
   int m_angleColumnIndex;
   int m_mirroredColumnIndex;
   int m_originTypeColumnIndex;
   int m_xCountColumnIndex;
   int m_yCountColumnIndex;
   int m_xSizeColumnIndex;
   int m_ySizeColumnIndex;
   int  m_physicalNameColumnIndex;

public:
   CPanelEntityPcbArraySheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor);

   virtual void invalidateBoardMapping();
   virtual int getBoardLogicalNameColumn() const;
   virtual int getBoardPhysicalNameColumn() const;

   virtual PanelEntityTypeTag getSheetType() const;
   virtual CPanelTemplateEntityArray& getEntityArray();
   virtual int onMenuStart(int row,long col,int section);
   //virtual void onMenuCommand(int row,long col,int section,int item);

   // operations
   //virtual void updateRow(long row);
};

//_________________________________________________________________________________________________
class CPanelEntityFiducialSheet : public CPanelEntitySheet
{
private:
   int m_typeColumnIndex;
   int m_sizeColumnIndex;
   int m_xOriginColumnIndex;
   int m_yOriginColumnIndex;
   int m_angleColumnIndex;
   int m_mirroredColumnIndex;

public:
   CPanelEntityFiducialSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor);

   virtual PanelEntityTypeTag getSheetType() const;
   virtual CPanelTemplateEntityArray& getEntityArray();
   virtual int onMenuStart(int row,long col,int section);
   //virtual void onMenuCommand(int row,long col,int section,int item);

   // operations
   //virtual void updateRow(long row);
};

//_________________________________________________________________________________________________
class CPanelEntityToolingHoleSheet : public CPanelEntitySheet
{
private:
   int m_sizeColumnIndex;
   int m_xOriginColumnIndex;
   int m_yOriginColumnIndex;

public:
   CPanelEntityToolingHoleSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor);

   virtual PanelEntityTypeTag getSheetType() const;
   virtual CPanelTemplateEntityArray& getEntityArray();
   virtual int onMenuStart(int row,long col,int section);
   //virtual void onMenuCommand(int row,long col,int section,int item);

   // operations
   //virtual void updateRow(long row);
};

//_________________________________________________________________________________________________
class CPanelEntitySheets
{
private:
   CPanelEntityGrid& m_panelEntityGrid;
   CTypedPtrArrayContainer<CPanelEntitySheet*> m_panelEntitySheets;

public:
   CPanelEntitySheets(CPanelEntityGrid& panelEntityGrid);

   int getCount() const;
   CPanelEntitySheet* getAt(int index);

   CPanelEntitySheet* addSheet(PanelEntityTypeTag sheetType,const CString& name,COLORREF cellBackgroundColor);

};

////_________________________________________________________________________________________________
//class CPanelEntityGridEdit : public CUGEdit
//{
//private:
//
//public:
//   virtual afx_msg void OnSetFocus(CWnd* pOldWnd); 
//};

//_________________________________________________________________________________________________
class CPanelEntityGrid : public CUltimateGrid
{
private:
   CPanelizer& m_panelizer;
   CPanelEntitySheets m_panelEntitySheets;
   CPanelEntityGridSelectionController m_selectionController;
   CPanelEntityClipBoard m_clipBoard;
   CUGEdit* m_editControl;
   int m_dropListTypeId;
   CFont* m_defaultFont;       // fonts are initialized in CUGCtrl::AttachGrid()
   CFont* m_defaultHeadingFont;

   const static int m_extentTabId           = 0;
   const static int m_pcbInstanceTabId      = 1;
   const static int m_pcbInstanceArrayTabId = 2;
   const static int m_fiducialTabId         = 3;
   const static int m_toolingHoleTabId      = 4;
   const static int m_maxTabId              = 4;

public:
   CPanelEntityGrid(CPanelizer& panelizer);
   ~CPanelEntityGrid();

   CPanelizer& getPanelizer();
   CPanelEntityGridSelectionController& getSelectionController();
   CPanelEntityClipBoard& getClipBoard();
   CFont* getDefaultFont() const;
   CFont* getDefaultHeadingFont() const;

   CUGEdit* getEditControl();
   CPanelEntitySheet* getCurrentSheet();
   CPanelEntitySheet* getSheet(int sheetIndex);

   void invalidateBoardMapping();
   void fillGrid();
   void clearEntityHighlights();
   void highlightEntityFromRow(int row);
   void highlightEntitiesFromRows(int rowStart,int rowEnd);

   // overrides of CUGCtrl
	virtual void OnSetup();
   virtual void OnTabSelected(int tabId);
   virtual int  OnMenuStart(int row,long col,int section);
   virtual void OnMenuCommand(int row,long col,int section,int item);
   int OnEditVerify(int col,long row,CWnd* edit,UINT* vcKey);
   int OnEditFinish(int col,long row,CWnd* edit,CString& string,BOOL cancelFlag);
   void OnCharDown(UINT* vcKey,BOOL processed);
   int OnEditStart(int col,long row,CWnd** edit);
   int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

   virtual void OnSH_LClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed);
   virtual void OnSH_RClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed);
	virtual void OnSelectionChanged(int startCol,long startRow,int endCol,long endRow,int blockNum);
   virtual void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);
   virtual void OnLClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,int processed);

	virtual COLORREF OnGetDefBackColor(int section);
};

//_________________________________________________________________________________________________
class CPanelizerDialog : public CResizingDialog
{
private:
   CPanelizer& m_panelizer;
   CPanelEntityGrid m_panelEntityGrid;

   CStatic m_currentTemplateGroup;
   CStatic m_currentTemplateDescriptorStatic;
   CButton m_loadButton;
   CButton m_saveButton;
   CButton m_saveAsButton;
   CButton m_instantiateButton;
   CButton m_cancelButton;

   FileStruct *m_mostRecentlyCreatedPanelFile;

public:
	CPanelizerDialog(CPanelizer& panelizer);   // standard constructor
	virtual ~CPanelizerDialog();

   FileStruct *GetMostRecentPanelFile()      { return m_mostRecentlyCreatedPanelFile; }

// Dialog Data
	enum { IDD = IDD_PanelizerDialog };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedSaveAs();
   afx_msg void OnBnClickedApply();
   afx_msg void OnBnClickedCancel();
public:
   virtual BOOL OnInitDialog();
};


/////////////////////////////////////////////////////////////////////////////
// Add Panel dialog
class CreatePanelDlg : public CDialog
{
private:
   CString  m_panelName;
   BOOL     m_replaceExisting;

   CMapStringToInt m_nameMap;

   void EnableOkButton();

public:
   CreatePanelDlg(CCEtoODBDoc &doc, CWnd* pParent = NULL);   // standard constructor
   ~CreatePanelDlg();

// Dialog Data
   //{{AFX_DATA(AddLayer)
   enum { IDD = IDD_PanelizerCreatePanel };
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return "CreatePanelDlg"; }

   CString GetPanelName()     { return m_panelName; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddLayer)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   afx_msg void OnEnChangePanelName();
   afx_msg void OnBnClickedReplaceExisting();
};



/////////////////////////////////////////////////////////////////////////////
// SaveAs Chooser dialog
class SaveAsChooserDlg : public CDialog
{
private:
   CString  m_templateName;
   BOOL     m_replaceExisting;

   CListBox m_lstExistingNames;

   CMapStringToInt m_nameMap;

   void EnableOkButton();

public:
   SaveAsChooserDlg(CStringArray &existingNames, CWnd* pParent = NULL);   // standard constructor
   ~SaveAsChooserDlg();

// Dialog Data
   //{{AFX_DATA(AddLayer)
   enum { IDD = IDD_PanelizerSaveAsChooser };
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return "PanelTemplateSaveAsChooserDlg"; }

   CString GetTemplateName()     { return m_templateName; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddLayer)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   afx_msg void OnEnChangePanelName();
   afx_msg void OnBnClickedReplaceExisting();
   afx_msg void OnLbnSelchangeExistingNamesList();
};

/////////////////////////////////////////////////////////////////////////////
// Load Chooser dialog
class LoadChooserDlg : public CDialog
{
private:
   CString  m_templateName;

   CListBox m_lstExistingNames;

   CMapStringToInt m_nameMap;

   void EnableOkButton();

public:
   LoadChooserDlg(CStringArray &existingNames, CWnd* pParent = NULL);   // standard constructor
   ~LoadChooserDlg();

// Dialog Data
   //{{AFX_DATA(AddLayer)
   enum { IDD = IDD_PanelizerLoadChooser };
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return "PanelTemplateSaveAsChooserDlg"; }

   CString GetTemplateName()     { return m_templateName; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddLayer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddLayer)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   //afx_msg void OnEnChangePanelName();
   //afx_msg void OnBnClickedReplaceExisting();
   afx_msg void OnLbnSelchangeExistingNamesList();
   //afx_msg void OnLButtonDblClk();
};

#endif
