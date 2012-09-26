// $Header: /CAMCAD/4.6/read_wrt/Xxxxx.h 2     11/30/06 9:26p Kurt Van Ness $

#if !defined(__UltimateGrid_h__)
#define __UltimateGrid_h__

#pragma once

#include "RwLib.h"
#include "DataDoctor.h"

// Sometimes the current build state had this defined but our Ultimate Grid is not built with OLE enabled
// so having this symbol defined before this include causes three undefined references at link time.
// It is not previous includes right here necessarily, it can be previous includes where this .h gets included.
#undef __AFXOLE_H__

#include "UGCtrl.h"

//=====================================================================================
// These symbols are described in UG Manual, but do not exist in UGCtrl.h.
// The function exists, but not the symbols. Rather than use integer constants in
// our code, we provide the #defines here. We only define two symbols, bitwise-OR them
// to get the "both" setting.
//
// For use with:   int CUGCtrl::SetCurrentCellMode(int mode) 
// 
// Parameter 
// mode – Specifies the highlight/focus mode. The following values are legal:
#define UG_CELLMODE_FOCUS     1  // (UG_CELLMODE_FOCUS) - current cell has a focus RECT.
#define UG_CELLMODE_HIGHLIGHT 2  // (UG_CELLMODE_HIGHLIGHT) - current cell is highlighted.
                                 //  3 (UG_CELLMODE_FOCUS | UG_CELLMODE_HIGHLIGHT) - both focus RECT and highlighted. 



//=====================================================================================
// Kurt probably had plans for this, but apparantly they were never fulfilled.
// Or maybe he just wanted to put "include CUltimateGrid.h" instead of CUGCtrl.h.
// But as it stands, this is a frivolous useless indirection.

class CUltimateGrid : public CUGCtrl
{
public:
   CUltimateGrid();
   virtual ~CUltimateGrid();
};

//=====================================================================================
//=====================================================================================

class CDDBaseGrid;

//=====================================================================================
// For help managing user defined column additions to grid

class CDDSpecialColumn : public CObject
{
private:
   CString m_name;
   int m_columnIndx;

public:
   CDDSpecialColumn(CString name) { m_name = name; m_columnIndx = -1; }

   int& GetColumnIndexRef()      { return m_columnIndx; }
   CString GetName()             { return m_name; }

};

class CDDSpecialColumnMap : public CMapSortedStringToOb<CDDSpecialColumn>
{
public:
   CDDSpecialColumn* Add(CString columnName);
   CDDSpecialColumn* GetSpecialColumn(int colIndx);
};

//=====================================================================================

// For mapping column name to column index

class CColumnDef : public CObject
{
private:
   CString m_title;
   int *m_colIndxPtr;

public:
   CColumnDef(CString title, int *colIndxPtr)   { m_title = title; m_colIndxPtr = colIndxPtr; }
   CString GetTitle()      { return m_title; }
   int *GetColIndexPtr()   { return m_colIndxPtr; }
   void SetColIndexPtr(int *ptr) { m_colIndxPtr = ptr; }
};


class CColumnDefMap : public CTypedObArrayWithMapContainer<CColumnDef>
{
private:
   CString m_name;

public:
   CColumnDefMap(CString name)   { m_name = name; }
   void AddColDef(CString title, int *colIndxPtr) { *colIndxPtr = this->getSize(); add( title, new CColumnDef(title, colIndxPtr) ); }
   void AddCustomAttributes(CDDSpecialColumnMap &customColumnMapToConfig, CDataDoctorLibraryAttribMap &customAttMapFromUser);
   void Update(CDDBaseGrid *grid);
   CString GetName() { return m_name; }
   void SaveSettings();
   void LoadSettings();
};

//=====================================================================================

// Originally the DataDoctor's "standard base grid", is now being promoted to the
// standard base grid for almost all CAMCAD grids. Many do not use this, it is a
// work in progress.

#define myUG_FIT_ALL_ROWS 0
#define myUG_NOT_A_COLUMN -99

class CDDBaseGrid : public CUGCtrl
{
public:
   int hocus;

private:
   //CMapStringToPtr m_dataMap; // used for support of flexGrid style "setRowData" feature
   
   // These are to support the effect of toggling sort direction for a column on
   // successive clicks.
   bool m_sortEnabled;
   CString m_lastSortCol;
   bool m_sortAscending;
   int m_THButtonDownCol;

   bool m_isSetUp; // is ready, is initialized ..., set this in the OnSetup() func

   // Multi-select support
   int m_selStartCol;
   long m_selStartRow;
   int m_selEndCol;
   long m_selEndRow;

public:
   // Column Definition Map
   CColumnDefMap m_columnDefMap;

   // Useful colors
   COLORREF m_redBackgroundColor;
   COLORREF m_greenBackgroundColor;
   COLORREF m_lightGrayBackgroundColor;
   COLORREF m_whiteBackgroundColor;

   //*rcf Found this elsewhere in CAMCAD code. I'm noting it here as a comment/reminder
   // for it I ever get around to more thoroughly standardizing he color handling for
   // basegrid based grids. Worth looking into what colors can be gotten this way, may
   // perhaps lead to nicer match-up of colors in grids to colors in other part of the GUI.
   // m_lightGrayBackgroundColor = GetSysColor(COLOR_BTNFACE);
   

 public:
   const static long DD_CELLTYPE_REFNAME = 101;
   const static long DD_CELLTYPE_OTHER   = 102;

   CDDBaseGrid();
   CDDBaseGrid(CString gridNameForStickySettings);
   //~CDDBaseGrid() { }

   CColumnDefMap &GetColumnDefMap()  { return m_columnDefMap; }

   virtual void OnCharDown(UINT* vcKey, BOOL processed);
   virtual void OnKeyDown(UINT *vcKey, BOOL processed);
   virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);
   virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);

	virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);

   virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);

	virtual BOOL OnCanColSwap(int fromCol,int toCol);
	virtual void OnColSwapped(int fromCol,int toCol);

   //virtual void OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed=0);

	virtual int OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags);

   CUGEdit m_myCUGEdit;


   //
   // Extensions
   //

   bool IsSetUp()             { return m_isSetUp; }
   void SetSetUp(bool flag)   { m_isSetUp = flag; }

   void SetMultiColHeading(int col1, int row1, int col2, int row2, CString headingStr);
   void SetCellRangeBackColor(int col1, int row1, int col2, int row2, COLORREF backColor);

   void SetRowBackColor(int rowIndx, COLORREF color);
   void QuickSetParam(int col, long row, long paramval);
   long QuickGetParam(int col, long row);

   int QuickSetBool(int col, long row, bool val);

   int QuickSetNumber(int col, long row, int number);
   int QuickSetNumber(int col, long row, double number, int decimals);

   int QuickSetTextAndColor(int col, long row, LPCTSTR string, COLORREF color);

   int QuickAddColumn(LPCTSTR columnTitle);

   bool GetSortEnabled()               { return m_sortEnabled; } // Turn on/off column heading click sort
   void SetSortEnabled(bool b)         { m_sortEnabled = b; }
   int SortBy(int colindx);            // perform what is likely a new sort
   int SortBy(CString colname);        // perform what is likely a new sort
   int ReSort(CString defaultcolname); // re-perform the previous sort, if there wasn't one then sort by defaultcol

   void ResizeAll()  { this->BestFit(0, this->GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS); }

   void RemoveAllRows()  { this->SetPaintMode(FALSE); while (this->GetNumberRows() > 0) this->DeleteRow(0); this->SetPaintMode(TRUE);}   

   // Use "param" value on "row heading" cell
   void SetRowData(int rowIndx, void *dataPtr)  { this->QuickSetParam(-1, rowIndx, (long)dataPtr); }
   void *GetRowData(int rowIndx)                { return (void*)(this->QuickGetParam(-1, rowIndx)); }

   int GetColFromName(LPCTSTR name, int *col);

   void SelectRow(long row);
   void CaptureSelection();
   void RefreshSelection();
   //void ClearSelectionVisibly(); // the UG ClearSelection() doesn't really, leaves remnants visible on next selection
   bool GetMultiSelectStart(int *col, long *row)   { *col = m_selStartCol; *row = m_selStartRow; return (m_selStartRow != m_selEndRow); }
   bool GetMultiSelectEnd(int *col, long *row)     { *col = m_selEndCol; *row = m_selEndRow; return (m_selStartRow != m_selEndRow); }

   int OnEditVerifyPercentCell(int col, long row,CWnd *edit,UINT *vcKey);
   int OnEditVerifyFloatCell(int col, long row,CWnd *edit,UINT *vcKey);
   //int OnEditVerifyIntegerCell(int col, long row,CWnd *edit,UINT *vcKey);  not implemented due to lack of need, but thought about it
   int OnEditVerifyUnsignedIntegerCell(int col, long row,CWnd *edit,UINT *vcKey);

   void FlushEvents();


};



#endif
