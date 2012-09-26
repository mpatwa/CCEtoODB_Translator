// $Header: /CAMCAD/4.5/FlexGridLib.h 14    1/31/06 8:50p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if !defined(__FlexGridLib_h__)
#define __FlexGridLib_h__

#pragma once

#include "FlexGrid.h"
#include <afxctl.h>


enum CellEditErrorTag
{
	cellEditErrorNone			= 0,
	cellEditErrorEmpty		= 1,
	cellEditErrorDuplicate	= 2,
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CExtendedFlexGrid
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CExtendedFlexGrid : public CFlexGrid
{
public:
   CExtendedFlexGrid();

public:
   virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
            UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
            BSTR bstrLicKey = NULL);

   CString getCellText(int row,int col);

   COleVariant getCell(long setting,int row,int col)
   { 
      COleVariant oleRow((GRID_CNT_TYPE)row);
      COleVariant oleCol((GRID_CNT_TYPE)col);

      COleVariant retval(get_Cell(setting,oleRow,oleCol,oleRow,oleCol));

      return retval;
   }

   void setCell(long setting,int row,int col,COleVariant& value)
   { 
      COleVariant oleRow((GRID_CNT_TYPE)row);
      COleVariant oleCol((GRID_CNT_TYPE)col);

      put_Cell(setting,oleRow,oleCol,oleRow,oleCol,value);
   }

   void setCells(long setting,int row0,int col0,int row1,int col1,COleVariant& value)
   { 
      COleVariant oleRow0((GRID_CNT_TYPE)row0);
      COleVariant oleCol0((GRID_CNT_TYPE)col0);
      COleVariant oleRow1((GRID_CNT_TYPE)row1);
      COleVariant oleCol1((GRID_CNT_TYPE)col1);

      put_Cell(setting,oleRow0,oleCol0,oleRow1,oleCol1,value);
   }

   void setCell(int row,int col,const CString& string)
   { 
      setCell(CFlexGrid::flexcpText,row,col,COleVariant(string));
   }

   void setCellStyleBold(int row,int col,bool flag=true)
   { 
      setCell(CFlexGrid::flexcpFontBold,row,col,COleVariant((GRID_CNT_TYPE)flag));
   }

   void setCellBackroundColor(int row,int col,COLORREF backgroundColor)
   { 
      setCell(CFlexGrid::flexcpBackColor,row,col,COleVariant((GRID_CNT_TYPE)backgroundColor));
   }

   void setCellForegroundColor(int row,int col,COLORREF foregroundColor)
   { 
      setCell(CFlexGrid::flexcpForeColor,row,col,COleVariant((GRID_CNT_TYPE)foregroundColor));
   }

   void setRowBackgroundColor(int row,COLORREF color)
   { 
      int columnCount = get_Cols();
      setCells(CFlexGrid::flexcpBackColor,row,0,row,columnCount - 1,COleVariant((GRID_CNT_TYPE)color));
   }

   void select(int row,int col,int rowSel = 0,int colSel = 0)
   {
      COleVariant oleRowSel((GRID_CNT_TYPE)rowSel);
      COleVariant oleColSel((GRID_CNT_TYPE)colSel);

		Select(row,col,oleRowSel,oleColSel);
   }

   void clearSelection()
   {
      put_Row(-1);
      put_Col(-1);
      put_RowSel(0);
      put_ColSel(0);
   }

   void addItem(LPCTSTR Item, int Row = -1)
   {
		if (Row == -1)
			AddItem(Item, COleVariant((GRID_CNT_TYPE)get_Rows()));
		else
			AddItem(Item, COleVariant((GRID_CNT_TYPE)Row));
   }

	void removeItem(int Row = -1)
   {
		RemoveItem(COleVariant((GRID_CNT_TYPE)Row));
   }

	void setCellPicture(int row,int col,int iconId);
   void setCellPicture(int row,int col,CPictureHolder& pic);
   void setCellComboList(int row,int col,const CString& comboList);
   void setColumnComboList(int col,const CString& comboList);
   void setCurrentCell(int row,int col);

   bool getCellChecked(int row,int col)
   {
      COleVariant cellValue(getCell(CFlexGrid::flexcpChecked,row,col));
      bool retval = (cellValue.iVal == flexChecked);

      return retval;
   }

   void setCellChecked(int row,int col,bool flag=true)
   { 
      setCell(CFlexGrid::flexcpChecked,row,col,COleVariant((GRID_CNT_TYPE)(flag ? flexChecked : flexUnchecked)));
   }

   void autoSizeColumns()
   { 
      AutoSize(0,COleVariant((GRID_CNT_TYPE)(get_Cols() - 1)),COleVariant((GRID_CNT_TYPE)0),COleVariant((GRID_CNT_TYPE)0));
   }

   void clear()
   { 
      Clear(COleVariant((GRID_CNT_TYPE)flexClearEverything),COleVariant((GRID_CNT_TYPE)flexClearEverywhere));
   }

   void setRadioCheck(int row,int col,int startCol,int endCol);

   void setRowData(int row,void* value);
   void* getRowData(int row);

   //void setCellEditable(int row,int col,bool flag=true)
   //{ 
   //   setCell(CFlexGrid::flexEDKbdMouse,row,col,COleVariant((GRID_CNT_TYPE)flag));
   //}
};



/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CFlexGridStatic
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CFlexGridStatic : public CStatic
{
private:
   static WNDCLASS m_wndClass;
   static WNDPROC m_oldWndProc;
   static CString m_wndClassName;

   CExtendedFlexGrid* m_grid;
   CExtendedFlexGrid* m_allocatedGrid;
   WINDOWPLACEMENT m_windowPlacement;
   int m_mouseUpCount;

public:
   CFlexGridStatic();
   ~CFlexGridStatic();

   void setGrid(CExtendedFlexGrid* grid);
   CExtendedFlexGrid* getGrid() const { return m_grid; }
   CExtendedFlexGrid* createGrid(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
            UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE, BSTR bstrLicKey = NULL);
   CExtendedFlexGrid* replaceDlgItem(UINT dlgItemId,LPCTSTR lpszWindowName,CWnd& parentWnd);
   CExtendedFlexGrid* substituteForDlgItem(UINT gridControlId,UINT dlgItemId,LPCTSTR lpszWindowName,CWnd& parentWnd);
   void restoreWindowPlacement();

   static void initSuperclass();
   static LRESULT CALLBACK wndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

   virtual BOOL Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle,const RECT& rect,
      CWnd* pParentWnd,UINT nID,CCreateContext* pContext = NULL);
   virtual BOOL Create(LPCTSTR lpszText,DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID = 0xffff);
   
   bool onMouseUp();
   void onSize();

   DECLARE_MESSAGE_MAP()
   afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};

#endif
