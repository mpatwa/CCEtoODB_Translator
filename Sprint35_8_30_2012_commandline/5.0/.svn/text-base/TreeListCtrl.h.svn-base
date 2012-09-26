// $Header: /CAMCAD/4.5/TreeListCtrl.h 14    8/11/06 7:50p Kurt Van Ness $

#if ! defined (__TreeListCtrl_h__)
#define __TreeListCtrl_h__

#pragma once

#include "TreeListHeaderCtrl.h"
#include "TypedContainer.h"

class CTreeListCtrl;

//_____________________________________________________________________________
class CTreeListComboBox : public CComboBox
{
protected:
   int m_column;
   CString m_currentEditBoxText;
   bool m_enableEditing;
   bool m_editFlag;

public:
   CTreeListComboBox(int column);
   virtual ~CTreeListComboBox();

   virtual CTreeListComboBox* create(CRect rect,CWnd* parentWnd) = 0;
   virtual void setSelectedText(const CString& text) = 0;
   virtual bool getEditFlag() { return m_editFlag; }

   //{{AFX_MSG(CTreeComboBox)
   virtual afx_msg void   OnCloseUp();
   virtual afx_msg void   OnKillFocus();
   virtual afx_msg void   OnEditUpdate();
   virtual afx_msg void   OnSelCancel();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   afx_msg void OnCbnSetfocus();
};

//_____________________________________________________________________________
struct SSortType
{
   int nCol;
   BOOL bAscending;
};

class CTreeListItem
{
//private:
public:
   CString m_itemString;

public:
   CTreeListItem();
   CTreeListItem(const CTreeListItem& copyItem);
   DWORD itemData;

   char m_cEnding;

   // visual attributes
   BOOL m_Bold;
   COLORREF m_Color;

   // m_nSub is zero-based
   CString GetItemString() const { return m_itemString; };
   CString GetSubstring(int m_nSub) const;
   virtual CString GetSubstring(CTreeListCtrl& treeListCtrl,HTREEITEM item,int m_nSub) const;
   CString GetItemText() const { return GetSubstring(0); };
   void SetSubstring(int m_nSub, CString m_sText);
   void InsertItem(CString m_sText) { SetSubstring(0, m_sText); };
};

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl window

class CTreeListCtrl : public CTreeCtrl
{
// Construction
public:
   CTreeListCtrl();

// Attributes
protected:
   int m_nColumns;
   int m_nColumnsWidth;
   int m_nItems;
   CDWordArray m_dwaItems;
   CTreeListComboBox* m_treeListComboBox;
   CTypedObArrayContainer<CTreeListComboBox*> m_comboBoxArray;
   HTREEITEM m_selectedItem;
   CImageList* m_imageList;

public:
   CTreeListHeaderCtrl m_wndHeader;
   CFont m_headerFont;
   CImageList m_cImageList;
   int m_nOffset;

// Operations
public:
   int GetColumnsNum() { return m_nColumns; };
   int GetColumnsWidth() { return m_nColumnsWidth; };
   int GetItemCount() { return m_nItems; };
   void RecalcColumnsWidth();

   void ResetVertScrollBar();

   HTREEITEM GetTreeItem(int nItem);
   int GetListItem(HTREEITEM hItem);

   int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
   int GetColumnWidth(int nCol);
   void SetColumnWidth(int nCol,int width);
   int GetColumnAlign(int nCol);

   BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
   DWORD GetItemData(HTREEITEM hItem) const;
   DWORD GetTreeItemData(HTREEITEM hItem) const;

   CString GetItemText(HTREEITEM hItem, int nSubItem = 0 );
   CString GetItemText(int nItem, int nSubItem );

   HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
   HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST );
   HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter );

   HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
   HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);

   BOOL DeleteItem( HTREEITEM hItem );
   BOOL DeleteItem( int nItem );

   void MemDeleteAllItems(HTREEITEM hParent);
   BOOL DeleteAllItems();

   BOOL SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem );

   BOOL SetItemColor( HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate = TRUE);
   BOOL SetItemBold( HTREEITEM hItem, BOOL m_Bold = TRUE, BOOL m_bInvalidate = TRUE );

   static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
   BOOL SortItems( int nCol, BOOL bAscending, HTREEITEM low);

   void DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat);
   virtual void OnItemDblClk(int selectedColumn,CPoint point,int left,int right);
   //void updateColumn(int column,const CString& text);

   // added by knv
   int getColumnItemClicked(CRect& itemRect,CPoint point);
   void showComboBox(HTREEITEM clickedItem,int selectedColumn,CRect itemRect);
   void setComboBoxAt(int column,CTreeListComboBox* comboBox);
   CTreeListComboBox* getComboBoxAt(int column);
   int getLevel(HTREEITEM item) const;
   HTREEITEM getAncestorAtLevel(HTREEITEM item,int level)const;
   virtual CTreeListItem* newTreeListItem();
   virtual CTreeListItem* newTreeListItem(const CTreeListItem& other);
   virtual void invalidateItem(HTREEITEM item);

   // 

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTreeListCtrl)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CTreeListCtrl();

   // Generated message map functions
protected:
   //{{AFX_MSG(CTreeListCtrl)
   afx_msg void OnPaint();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   //afx_msg void OnDestroy();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_)
