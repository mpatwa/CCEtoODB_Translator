// $Header: /CAMCAD/4.5/FlexGridLib.cpp 11    1/31/06 8:50p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// FLEXGRIDLIB.CPP

#include "StdAfx.h"
#include "FlexGridLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL GetLicenseString(CLSID clsid, BSTR &licStr);


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CFlexGridStatic
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CExtendedFlexGrid::CExtendedFlexGrid()
{
}

BOOL CExtendedFlexGrid::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
         UINT nID, CFile* pPersist, BOOL bStorage,
         BSTR bstrLicKey)
{ 
   if (bstrLicKey == NULL)
   {
      if (!GetLicenseString(GetClsid(), bstrLicKey))
      {
         bstrLicKey = NULL;
      }
   }

   return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
               pPersist, bStorage, bstrLicKey); 
}

CString CExtendedFlexGrid::getCellText(int row,int col)
{
   COleVariant oleRow((GRID_CNT_TYPE)row);
   COleVariant oleCol((GRID_CNT_TYPE)col);

   COleVariant oleData = get_Cell(0,oleRow,oleCol,oleRow,oleCol);
   CString retval = oleData;

   return retval;
}

void CExtendedFlexGrid::setRadioCheck(int row,int col,int startCol,int endCol)
{
   if (col >= startCol && col <= endCol)
   {
      if (getCellChecked(row,col))
      {
         for (int colInd = startCol;colInd <= endCol;colInd++)
         {
            if (colInd != col)
            {
               if (getCellChecked(row,colInd))
               {
                  setCellChecked(row,colInd,false);
               }
            }
         }
      }
   }
}

void CExtendedFlexGrid::setCurrentCell(int row,int col)
{
   put_Row(row);
   put_RowSel(row);
   put_Col(col);
   put_ColSel(col);
}

void CExtendedFlexGrid::setCellComboList(int row,int col,const CString& comboList)
{
   setCurrentCell(row,col);
   put_ComboList(comboList);
}

void CExtendedFlexGrid::setColumnComboList(int col,const CString& comboList)
{
   put_ColComboList(col,comboList);
}

void CExtendedFlexGrid::setCellPicture(int row,int col,int iconId)
{ 
   CPictureHolder pic;
   pic.CreateFromIcon(iconId);
   LPDISPATCH pPic = pic.GetPictureDispatch();

   setCurrentCell(row,col);
   put_CellPicture(pPic);

   pPic->Release();
}

void CExtendedFlexGrid::setCellPicture(int row,int col,CPictureHolder& pic)
{ 
   LPDISPATCH pPic = pic.GetPictureDispatch();

   setCurrentCell(row,col);
   put_CellPicture(pPic);

   pPic->Release();
}

void CExtendedFlexGrid::setRowData(int row,void* value)
{ 
   COleVariant oleData;
   oleData.pintVal = (int*)value;

   put_RowData(row,oleData);
}

void* CExtendedFlexGrid::getRowData(int row)
{ 
   VARIANT data = get_RowData(row);

   return (void*)(data.pintVal);
}


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CFlexGridStatic
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
WNDCLASS CFlexGridStatic::m_wndClass;
WNDPROC CFlexGridStatic::m_oldWndProc = NULL;
CString CFlexGridStatic::m_wndClassName("FLEXGRIDSTATIC");

CFlexGridStatic::CFlexGridStatic()
{
   m_grid          = NULL;
   m_allocatedGrid = NULL;
   m_windowPlacement.showCmd = SW_HIDE;
   m_mouseUpCount  = 0;

   initSuperclass();
}

CFlexGridStatic::~CFlexGridStatic()
{
   delete m_allocatedGrid;
}

void CFlexGridStatic::setGrid(CExtendedFlexGrid* grid)
{
   m_grid = grid;
}

BOOL CFlexGridStatic::Create(LPCTSTR lpszText,DWORD dwStyle,const RECT& rect,
                             CWnd* pParentWnd,UINT nID)
{
   BOOL retval = CStatic::Create(lpszText,dwStyle,rect,pParentWnd,nID);

   return retval;
}

BOOL CFlexGridStatic::Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle,const RECT& rect,
   CWnd* pParentWnd,UINT nID,CCreateContext* pContext )
{
   BOOL retval = CWnd::Create(lpszClassName,lpszWindowName,dwStyle,rect,
                     pParentWnd,nID,pContext);

   return retval;
}

CExtendedFlexGrid* CFlexGridStatic::createGrid(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
   UINT nID, CFile* pPersist, BOOL bStorage,BSTR bstrLicKey)
{
   delete m_allocatedGrid;

   m_allocatedGrid = new CExtendedFlexGrid();

   if (bstrLicKey == NULL)
   {
      if (!GetLicenseString(m_allocatedGrid->GetClsid(), bstrLicKey))
      {
         bstrLicKey = NULL;
      }
   }
   
   m_allocatedGrid->Create(lpszWindowName,dwStyle,rect,pParentWnd,nID,
                      pPersist,bStorage,bstrLicKey);

   m_grid = m_allocatedGrid;

   return m_grid;
}

CExtendedFlexGrid* CFlexGridStatic::replaceDlgItem(UINT dlgItemId,LPCTSTR lpszWindowName,CWnd& parentWnd)
{
   CRect dlgItemRect;
   CWnd* dlgItemWnd = parentWnd.GetDlgItem(dlgItemId);
   dlgItemWnd->GetWindowRect(dlgItemRect);
   parentWnd.ScreenToClient(&dlgItemRect);
   BOOL destroyResult = dlgItemWnd->DestroyWindow();

   CExtendedFlexGrid* extendedFlexGrid = createGrid(lpszWindowName,WS_VISIBLE,dlgItemRect,&parentWnd,dlgItemId);

   return extendedFlexGrid;
}

CExtendedFlexGrid* CFlexGridStatic::substituteForDlgItem(UINT gridControlId,UINT dlgItemId,LPCTSTR lpszWindowName,CWnd& parentWnd)
{
   CRect dlgItemRect;
   CWnd* dlgItemWnd = parentWnd.GetDlgItem(dlgItemId);
   dlgItemWnd->GetWindowRect(dlgItemRect);
   parentWnd.ScreenToClient(&dlgItemRect);
   dlgItemWnd->ShowWindow(SW_HIDE);

   CExtendedFlexGrid* extendedFlexGrid = createGrid(lpszWindowName,WS_VISIBLE,dlgItemRect,&parentWnd,gridControlId);

   CWnd* gridWnd = parentWnd.GetDlgItem(gridControlId);

   return extendedFlexGrid;
}

void CFlexGridStatic::restoreWindowPlacement()
{
   if (m_windowPlacement.showCmd == SW_SHOW)
   {
      m_windowPlacement.showCmd = SW_SHOW;
      m_grid->SetWindowPlacement(&m_windowPlacement);
   }
}
   
bool CFlexGridStatic::onMouseUp()
{
   bool retval = false;

   m_mouseUpCount++;

   if (m_mouseUpCount < 2)
   {
      long row1,col1,row2,col2;
      m_grid->GetSelection(&row1,&col1,&row2,&col2);

      //m_grid->ShowCell(1,1);
      //m_grid->clearSelection();
      restoreWindowPlacement();

      m_grid->select(row2,col2,row2,col2);
      m_grid->GetSelection(&row1,&col1,&row2,&col2);

      retval = true;
   }

   return retval;
}

void CFlexGridStatic::onSize()
{
   m_mouseUpCount = 0;
}

void CFlexGridStatic::initSuperclass()
{
   if (m_oldWndProc == NULL)
   {
      ::GetClassInfo(AfxGetInstanceHandle(),"STATIC",&m_wndClass);

      m_oldWndProc             = m_wndClass.lpfnWndProc;
      m_wndClass.hInstance     = AfxGetInstanceHandle();
      m_wndClass.lpfnWndProc   = wndProc;
      m_wndClass.lpszClassName = m_wndClassName;

      ::RegisterClass(&m_wndClass);
   }
}

LRESULT CALLBACK CFlexGridStatic::wndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT retval;

   switch (msg)
   {
   case WM_CREATE:
      //break;
   default:
      retval = (m_oldWndProc)(hWnd,msg,wParam,lParam);
   }

   return retval;
}

BEGIN_MESSAGE_MAP(CFlexGridStatic, CStatic)
   ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

void CFlexGridStatic::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
   CStatic::OnWindowPosChanged(lpwndpos);

   if (m_grid != NULL)
   {
      GetWindowPlacement(&m_windowPlacement);
      m_windowPlacement.showCmd = SW_SHOW;

      m_grid->SetWindowPlacement(&m_windowPlacement);
   }
}
