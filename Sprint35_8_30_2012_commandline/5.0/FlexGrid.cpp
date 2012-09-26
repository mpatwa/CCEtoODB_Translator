// $Header: /CAMCAD/4.4/FlexGrid.cpp 6     10/28/04 9:40a Kurt Van Ness $

#include "StdAfx.h"
#include "FlexGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CFlexGrid

IMPLEMENT_DYNCREATE(CFlexGrid, CWnd)

CFlexGrid::CFlexGrid()
{
   int iii = 3;
}

BOOL CFlexGrid::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
               const RECT& rect, CWnd* pParentWnd, UINT nID, 
               CCreateContext* pContext)
{ 
   return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
}

BOOL CFlexGrid::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
         UINT nID, CFile* pPersist, BOOL bStorage,
         BSTR bstrLicKey)
{ 
   return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
   pPersist, bStorage, bstrLicKey); 
}

// CFlexGrid properties

// CFlexGrid operations
