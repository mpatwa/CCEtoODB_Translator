// $Header: /CAMCAD/4.5/TreeListCtrl.cpp 19    8/11/06 7:50p Kurt Van Ness $

#include "StdAfx.h"
#include "TreeListCtrl.h"
#include "TreeListFrame.h"
#include ".\treelistctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________

// Implementation of CTreeListComboBox class
BEGIN_MESSAGE_MAP(CTreeListComboBox, CComboBox)
   //{{AFX_MSG_MAP(CTreeComboBox)
   ON_CONTROL_REFLECT(CBN_SELENDCANCEL, CTreeListComboBox::OnSelCancel)
   ON_CONTROL_REFLECT(CBN_CLOSEUP, CTreeListComboBox::OnCloseUp)
   ON_CONTROL_REFLECT(CBN_KILLFOCUS, CTreeListComboBox::OnKillFocus)
   ON_CONTROL_REFLECT(CBN_EDITUPDATE, CTreeListComboBox::OnEditUpdate)
   //}}AFX_MSG_MAP
   ON_CONTROL_REFLECT(CBN_SETFOCUS, OnCbnSetfocus)
END_MESSAGE_MAP()

CTreeListComboBox::CTreeListComboBox(int column) :
   m_column(column)
{
   m_enableEditing = false;
   m_editFlag      = false;
}

CTreeListComboBox::~CTreeListComboBox()
{
}

void CTreeListComboBox::OnSelCancel() 
{
   CString strText;
   CTreeListCtrl* pParent = (CTreeListCtrl*)GetParent();
   ASSERT( pParent );

   //int iCurSel = GetCurSel();
   //if ( iCurSel == CB_ERR )
   // strText = strCurrText;
   //else
   // GetLBText( iCurSel, strText );

   //if ( !strText.IsEmpty() )
   // pParent->SetItemText( pParent->GetSelectedItem(), strText );
   
   pParent->SetFocus();
}

void CTreeListComboBox::OnCloseUp() 
{
   CString strText;
   CTreeListCtrl* pParent = (CTreeListCtrl*)GetParent();
   ASSERT( pParent );

   //int iCurSel = GetCurSel();
   //if ( iCurSel == CB_ERR )
   // strText = strCurrText;
   //else
   // GetLBText( iCurSel, strText );

   //if ( !strText.IsEmpty() )
   // pParent->SetItemText( pParent->GetSelectedItem(), strText );
   
   pParent->SetFocus();
}

void CTreeListComboBox::OnKillFocus() 
{
   ShowWindow(SW_HIDE);
}

void CTreeListComboBox::OnEditUpdate() 
{
   // We need to hold last entered text to set it as an item description, 'cause
   // combo box sets matched text into it's edit part
   GetWindowText(m_currentEditBoxText);
   m_editFlag = true;
}

void CTreeListComboBox::OnCbnSetfocus()
{
   GetWindowText(m_currentEditBoxText);
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListItem

CTreeListItem::CTreeListItem()
{
   m_cEnding = '¶';
   m_itemString = "";
   m_Bold = FALSE;
   m_Color = ::GetSysColor(COLOR_WINDOWTEXT);
}

CTreeListItem::CTreeListItem(const CTreeListItem& copyItem)
{
   m_cEnding = copyItem.m_cEnding;
   m_itemString = copyItem.GetItemString();
   m_Bold = copyItem.m_Bold;
   m_Color = copyItem.m_Color;
   itemData = copyItem.itemData;
}


CString CTreeListItem::GetSubstring(CTreeListCtrl& treeListCtrl,HTREEITEM item,int nSub) const
{
   return GetSubstring(nSub);
}

CString CTreeListItem::GetSubstring(int m_nSub) const
{
   CString m_tmpStr("");
   int i=0, nHits=0;
   int length = m_itemString.GetLength();

   while((i<length) && (nHits<=m_nSub))
   {
      if (m_itemString[i]==m_cEnding)
      {
         nHits++;
      }
      else
      if (nHits==m_nSub)
         m_tmpStr+=m_itemString[i];

      i++;
   }

   if ((i>=length) && (nHits<m_nSub))
      return "";
   else
      return m_tmpStr;
}

void CTreeListItem::SetSubstring(int m_nSub, CString m_sText)
{
   CString m_tmpStr("");
   int i=0, nHits=0, first=0;
   int length = m_itemString.GetLength();

   while((i<length) && (nHits<=m_nSub))
   {
      if (m_itemString[i]==m_cEnding)
      {
         if (nHits!=m_nSub)
            first = i;
         nHits++;
      }

      i++;
   }

   CString m_newStr("");
   if ((nHits>m_nSub) || ((nHits==m_nSub) && (i>=length)))
   {
      // insert in the middle
      if (first!=0)
      {
         m_newStr = m_itemString.Left(first);
         m_newStr += m_cEnding; 
      }
      m_newStr += m_sText;
      if (i<length)
      {
         m_newStr += m_cEnding;
         m_newStr += m_itemString.Right(m_itemString.GetLength()-i);
      }

      m_itemString=m_newStr;
   }
   else
   {
      // insert at the end
      for(i=nHits;i<m_nSub;i++)
         m_itemString+=m_cEnding;
      m_itemString+=m_sText;
   }
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl

CTreeListCtrl::CTreeListCtrl()
{
   m_nColumns = m_nColumnsWidth = 0;
   m_nOffset = 0;
   m_treeListComboBox = NULL;
   m_selectedItem     = NULL;
}

CTreeListCtrl::~CTreeListCtrl()
{
   delete m_treeListComboBox;
}

BEGIN_MESSAGE_MAP(CTreeListCtrl, CTreeCtrl)
   //{{AFX_MSG_MAP(CTreeListCtrl)
   ON_WM_PAINT()
   ON_WM_CREATE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_KEYDOWN()
   ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl message handlers

int CTreeListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
      return -1;


   return 0;
}

HTREEITEM CTreeListCtrl::GetTreeItem(int nItem)
{
   HTREEITEM m_ParentItem = GetRootItem();
   int m_nCount = 0;

   while((m_ParentItem!=NULL) && (m_nCount<nItem))
   {
      m_nCount ++ ;
      GetNextSiblingItem(m_ParentItem);
   }

   return m_ParentItem;
}

int CTreeListCtrl::GetListItem(HTREEITEM hItem)
{
   HTREEITEM m_ParentItem = GetRootItem();
   int m_nCount = 0;

   while((m_ParentItem!=NULL) && (m_ParentItem!=hItem))
   {
      m_nCount ++ ;
      GetNextSiblingItem(m_ParentItem);
   }

   return m_nCount;
}

int CTreeListCtrl::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
   HD_ITEM hdi;
   hdi.mask = HDI_TEXT | HDI_FORMAT;
   if (nWidth!=-1)
   {
      hdi.mask |= HDI_WIDTH;
      hdi.cxy = nWidth;
   }
   
   hdi.pszText = (LPTSTR)lpszColumnHeading;
   hdi.fmt = HDF_OWNERDRAW;

   if (nFormat == LVCFMT_RIGHT)
      hdi.fmt |= HDF_RIGHT;
   else
   if (nFormat == LVCFMT_CENTER)
      hdi.fmt |= HDF_CENTER;
   else
      hdi.fmt |= HDF_LEFT;

   m_nColumns ++ ;

   int m_nReturn = m_wndHeader.InsertItem(nCol, &hdi);

   if (m_nColumns==1)
   {
      m_wndHeader.SetItemImage(m_nReturn, 0);
   }

   RecalcColumnsWidth();

   return m_nReturn;
}

int CTreeListCtrl::GetColumnWidth(int nCol)
{
   HD_ITEM hItem;
   hItem.mask = HDI_WIDTH;
   if (!m_wndHeader.GetItem(nCol, &hItem))
      return 0;

   return hItem.cxy;
}

void CTreeListCtrl::SetColumnWidth(int nCol,int width)
{
   HD_ITEM hItem;
   hItem.mask = HDI_WIDTH;

   if (m_wndHeader.GetItem(nCol, &hItem))
   {
      hItem.cxy = width;
      m_wndHeader.SetItem(nCol, &hItem);
      RecalcColumnsWidth();
   }
}

int CTreeListCtrl::GetColumnAlign(int nCol)
{
   HD_ITEM hItem;
   hItem.mask = HDI_FORMAT;
   if (!m_wndHeader.GetItem(nCol, &hItem))
      return LVCFMT_LEFT;

   if (hItem.fmt & HDF_RIGHT)
      return LVCFMT_RIGHT;
   else
   if (hItem.fmt & HDF_CENTER)
      return LVCFMT_CENTER;
   else
      return LVCFMT_LEFT;
}

void CTreeListCtrl::RecalcColumnsWidth()
{
   m_nColumnsWidth = 0;

   for(int i=0;i<m_nColumns;i++)
   {
      m_nColumnsWidth += GetColumnWidth(i);
   }
}

void CTreeListCtrl::DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat)
{
    //
    // Make sure the text will fit in the prescribed rectangle, and truncate
    // it if it won't.
    //
    BOOL bNeedDots = FALSE;
    int nMaxWidth = nWidth - 4;

    while ((text.GetLength()>0) && (pDC->GetTextExtent((LPCTSTR) text).cx > (nMaxWidth - 4))) {
        text = text.Left (text.GetLength () - 1);
        bNeedDots = TRUE;
    }

    if (bNeedDots) {
        if (text.GetLength () >= 1)
            text = text.Left (text.GetLength () - 1);
        text += "...";
    }

    //
    // Draw the text into the rectangle using MFC's handy CDC::DrawText
    // function.
    //
    rect.right = rect.left + nMaxWidth;

    UINT nStyle = DT_VCENTER | DT_SINGLELINE;
    if (nFormat == LVCFMT_LEFT)
        nStyle |= DT_LEFT;
    else if (nFormat == LVCFMT_CENTER)
        nStyle |= DT_CENTER;
    else // nFormat == LVCFMT_RIGHT
        nStyle |= DT_RIGHT;

   if ((text.GetLength()>0) && (rect.right>rect.left))
      pDC->DrawText (text, rect, nStyle);
}


void CTreeListCtrl::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   CRect rcClip, rcClient;
   dc.GetClipBox( &rcClip );
   GetClientRect(&rcClient);

   // Set clip region to be same as that in paint DC
   CRgn rgn;
   rgn.CreateRectRgnIndirect( &rcClip );
   dc.SelectClipRgn(&rgn);
   rgn.DeleteObject();
   
   COLORREF m_wndColor = GetSysColor( COLOR_WINDOW );

   dc.SetViewportOrg(m_nOffset, 0);

   dc.SetTextColor(m_wndColor);

    
   // First let the control do its default drawing.
   CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );

   HTREEITEM hItem = GetFirstVisibleItem();

   int n = GetVisibleCount(), m_nWidth;

   dc.FillSolidRect(GetColumnWidth(0),1,rcClient.Width(),rcClient.Height(),m_wndColor);

   CTreeListItem *pItem;

   // the most urgent thing is to erase the labels that were drawn by the tree
   while(hItem!=NULL && n>=0)
   {
      CRect rect;

      UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
   
      CRect m_labelRect;
      GetItemRect( hItem, &m_labelRect, TRUE );
      GetItemRect( hItem, &rect, FALSE );
      if (GetColumnsNum()>1)
         rect.left = min(m_labelRect.left, GetColumnWidth(0));
      else
         rect.left = m_labelRect.left;
      rect.right = m_nColumnsWidth;
      dc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height(),m_wndColor);

      hItem = GetNextVisibleItem( hItem );
      n--;
   }

   // create the font
   CFont *pFontDC;
   CFont fontDC, boldFontDC;
   LOGFONT logfont;

   CFont *pFont = GetFont();
   pFont->GetLogFont( &logfont );

   fontDC.CreateFontIndirect( &logfont );
   pFontDC = dc.SelectObject( &fontDC );

   logfont.lfWeight = 700;
   boldFontDC.CreateFontIndirect( &logfont );

   // and now let's get to the painting itself

   hItem = GetFirstVisibleItem();
   n = GetVisibleCount();

   while (hItem != NULL && n >= 0)
   {
      CRect rect;

      UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
   
      if ( !(GetItemState( hItem, selflag ) & selflag ))
      {
         dc.SetBkMode(TRANSPARENT);

         pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);
         CString sItem = ((pItem != NULL) ? pItem->GetItemText() : "");

         CRect m_labelRect;
         GetItemRect( hItem, &m_labelRect, TRUE );
         GetItemRect( hItem, &rect, FALSE );
         if (GetColumnsNum()>1)
            rect.left = min(m_labelRect.left, GetColumnWidth(0));
         else
            rect.left = m_labelRect.left;
         rect.right = m_nColumnsWidth;

         dc.SetBkColor( m_wndColor );

         if (pItem != NULL)
         {
            dc.SetTextColor( pItem->m_Color );

            if (pItem->m_Bold)
            {
               dc.SelectObject( &boldFontDC );
            }
         }

         int imageIndex;
         int selectedImageIndex;

         GetItemImage(hItem,imageIndex,selectedImageIndex);

         int imageOffset = 0;

         if (imageIndex >= 0)
         {
            m_imageList->Draw(&dc,imageIndex,rect.TopLeft(),ILD_TRANSPARENT);
            imageOffset = 16;
         }

         DrawItemText(&dc, sItem, CRect(rect.left+2+imageOffset, rect.top, GetColumnWidth(0)-imageOffset, rect.bottom),
            GetColumnWidth(0)-rect.left-2-imageOffset, GetColumnAlign(0));

         m_nWidth = 0;

         for (int i = 1;i < m_nColumns;i++)
         {
            CString subString = ((pItem != NULL) ? pItem->GetSubstring(*this,hItem,i) : "");

            m_nWidth += GetColumnWidth(i-1);
            DrawItemText(&dc,subString,CRect(m_nWidth, rect.top, m_nWidth+GetColumnWidth(i), rect.bottom), GetColumnWidth(i), GetColumnAlign(i));
         }
         
         dc.SetTextColor(::GetSysColor (COLOR_WINDOWTEXT ));

         if (pItem != NULL && pItem->m_Bold)
         {
            dc.SelectObject( &fontDC );
         }
      }
      else
      {
         CRect m_labelRect;
         GetItemRect( hItem, &m_labelRect, TRUE );
         GetItemRect( hItem, &rect, FALSE );

         if (GetColumnsNum() > 1)
            rect.left = min(m_labelRect.left, GetColumnWidth(0));
         else
            rect.left = m_labelRect.left;

         rect.right = m_nColumnsWidth;

         int imageIndex;
         int selectedImageIndex;

         GetItemImage(hItem,imageIndex,selectedImageIndex);

         int imageOffset = 0;

         if (selectedImageIndex >= 0)
         {
            m_imageList->Draw(&dc,selectedImageIndex,rect.TopLeft(),ILD_TRANSPARENT);
            imageOffset = 16;
         }

         // If the item is selected, paint the rectangle with the system color
         // COLOR_HIGHLIGHT

         COLORREF m_highlightColor = ::GetSysColor (COLOR_HIGHLIGHT);

         CBrush brush(m_highlightColor);
         CRect fillRect(rect);
         fillRect.left += imageOffset;
         dc.FillRect (fillRect, &brush);

         // draw a dotted focus rectangle

         dc.DrawFocusRect (rect);
         
         pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);
         CString sItem = ((pItem != NULL) ? pItem->GetItemText() : "");

         dc.SetBkColor(m_highlightColor);

         dc.SetTextColor(::GetSysColor (COLOR_HIGHLIGHTTEXT));

         if (pItem != NULL && pItem->m_Bold)
         {
            dc.SelectObject( &boldFontDC );
         }

         //DrawItemText(&dc, sItem, CRect(rect.left+2, rect.top, GetColumnWidth(0), rect.bottom), GetColumnWidth(0)-rect.left-2, GetColumnAlign(0));
         DrawItemText(&dc, sItem, CRect(rect.left+2+imageOffset, rect.top, GetColumnWidth(0)-imageOffset, rect.bottom),
            GetColumnWidth(0)-rect.left-2-imageOffset, GetColumnAlign(0));

         m_nWidth = 0;

         for (int i = 1;i < m_nColumns;i++)
         {
            CString subString = ((pItem != NULL) ? pItem->GetSubstring(*this,hItem,i) : "");

            m_nWidth += GetColumnWidth(i-1);
            DrawItemText(&dc,subString,CRect(m_nWidth, rect.top, m_nWidth+GetColumnWidth(i), rect.bottom), GetColumnWidth(i), GetColumnAlign(i));
         }

         if (pItem != NULL && pItem->m_Bold)
         {
            dc.SelectObject( &fontDC );
         }
      }

      hItem = GetNextVisibleItem( hItem );
      n--;
   }

   dc.SelectObject( pFontDC );
}

void CTreeListCtrl::ResetVertScrollBar()
{
   CTreeListFrame *pFrame = (CTreeListFrame*)GetParent();

   CRect m_treeRect;
   GetClientRect(&m_treeRect);

   CRect m_wndRect;
   pFrame->GetClientRect(&m_wndRect);

   CRect m_headerRect;
   m_wndHeader.GetClientRect(&m_headerRect);

   CRect m_barRect;
   pFrame->m_horScrollBar.GetClientRect(&m_barRect);

   if (!pFrame->HorizontalScrollVisible())
      SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);
   else
      SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

   if (pFrame->HorizontalScrollVisible())
   {
      if (!pFrame->VerticalScrollVisible())
      {
         pFrame->m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);

         int nMin, nMax;
         pFrame->m_horScrollBar.GetScrollRange(&nMin, &nMax);
         if ((nMax-nMin) == (GetColumnsWidth()-m_treeRect.Width()+GetSystemMetrics(SM_CXVSCROLL)))
            // i.e. it disappeared because of calling
            // SetWindowPos
         {
            if (nMax - GetSystemMetrics(SM_CXVSCROLL) > 0)
               pFrame->m_horScrollBar.SetScrollRange(nMin, nMax - GetSystemMetrics(SM_CXVSCROLL));
            else
               // hide the horz scroll bar and update the tree
            {
               pFrame->m_horScrollBar.EnableWindow(FALSE);

               // we no longer need it, so hide it!
               {
                  pFrame->m_horScrollBar.ShowWindow(SW_HIDE);

                  SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
                  // the tree takes scroll's place
               }

               pFrame->m_horScrollBar.SetScrollRange(0, 0);

               // set scroll offset to zero
               {
                  m_nOffset = 0;
                  Invalidate();
                  m_wndHeader.GetWindowRect(&m_headerRect);
                  m_wndHeader.SetWindowPos(&wndTop, m_nOffset, 0, max(pFrame->StretchWidth(GetColumnsWidth(),m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
               }
            }
         }
      }
      else
      {
         pFrame->m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);

         int nMin, nMax;
         pFrame->m_horScrollBar.GetScrollRange(&nMin, &nMax);
         if ((nMax-nMin) == (GetColumnsWidth()-m_treeRect.Width()-GetSystemMetrics(SM_CXVSCROLL)))
            // i.e. it appeared because of calling
            // SetWindowPos
         {
            pFrame->m_horScrollBar.SetScrollRange(nMin, nMax + GetSystemMetrics(SM_CXVSCROLL));
         }
      }
   }
   else
   if (pFrame->VerticalScrollVisible())
   {
      if (GetColumnsWidth()>m_treeRect.Width())
         // the vertical scroll bar takes some place
         // and the columns are a bit bigger than the client
         // area but smaller than (client area + vertical scroll width)
      {
         // show the horz scroll bar
         {
            pFrame->m_horScrollBar.EnableWindow(TRUE);

            pFrame->m_horScrollBar.ShowWindow(SW_SHOW);

            // the tree becomes smaller
            SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

            pFrame->m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
         }

         pFrame->m_horScrollBar.SetScrollRange(0, GetColumnsWidth()-m_treeRect.Width());

      }
   }
}

int CTreeListCtrl::getColumnItemClicked(CRect& itemRect,CPoint point)
{
   int selectedColumn = -1;

   UINT flags;
   HTREEITEM clickedItem = HitTest(point,&flags);

   if (clickedItem != NULL)
   {
      GetItemRect(clickedItem,&itemRect,TRUE);

      int xLeft = 0;
      int columnWidth = 0;

      for (int column = 0;column < GetColumnsNum();column++)
      {
         columnWidth = GetColumnWidth(column);

         if (point.x >= xLeft && point.x < xLeft + columnWidth)
         {
            selectedColumn = column;
            break;
         }

         xLeft += columnWidth;
      }

      itemRect.left  = xLeft;
      itemRect.right = xLeft + columnWidth;
   }

   return selectedColumn;
}

void CTreeListCtrl::invalidateItem(HTREEITEM item)
{
   CRect rect;

   GetItemRect(item,&rect,false);
   InvalidateRect(rect);
}

void CTreeListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
   UINT flags;
   m_selectedItem = HitTest(point, &flags);

   if ((flags & TVHT_ONITEMRIGHT) || (flags & TVHT_ONITEMINDENT) ||
      (flags & TVHT_ONITEM))
   {
      SelectItem(m_selectedItem);
   }

   if ((GetColumnsNum() == 0) || (point.x < GetColumnWidth(0)))
   {
      point.x -= m_nOffset;
      m_selectedItem = HitTest(point, &flags);

      if (flags & TVHT_ONITEMBUTTON)
      {
         Expand(m_selectedItem, TVE_TOGGLE);
      }

      SetFocus();

      ResetVertScrollBar();
   }
   else if (m_selectedItem != NULL)
   {
      if (GetItemData(m_selectedItem) != NULL)
      {
         CRect itemRect;
         int columnClicked = getColumnItemClicked(itemRect,point);

         if (columnClicked > 0)
         {
            showComboBox(m_selectedItem,columnClicked,itemRect);
         }
      }
   }

// CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTreeListCtrl::showComboBox(HTREEITEM clickedItem,int selectedColumn,CRect itemRect) 
{
   if (clickedItem != NULL)
   {
      delete m_treeListComboBox;
      m_treeListComboBox = NULL;

      CTreeListComboBox* comboBoxFactory = getComboBoxAt(selectedColumn);

      if (comboBoxFactory != NULL)
      {
         m_treeListComboBox = comboBoxFactory->create(itemRect,this);

         //if (m_treeListComboBox->Create(dwComboBoxStyle,itemRect,this,0))
         //{
            //pCB->SetWindowPos( NULL, iComboLeft, itemRect.top, iComboWidth, m_iComboDroppedHeight, SWP_NOZORDER );

            // Set the same font as CTreeCtrl has.
            m_treeListComboBox->SetFont(GetFont(),FALSE);

            // Select current item in combo box if it's found.
            CString text = GetItemText(clickedItem,selectedColumn);
            m_treeListComboBox->setSelectedText(text);

               //CString strItemText = GetItemText(hti);
               //int iFIndex = pCB->FindString( -1, strItemText );
               //if ( iFIndex != CB_ERR )
               //pCB->SetCurSel( iFIndex );
               //else
               //pCB->SetWindowText( strItemText );

            m_treeListComboBox->ShowWindow(SW_SHOWNORMAL);
            m_treeListComboBox->ShowDropDown(TRUE);
            m_treeListComboBox->SetFocus();

            // Add TreeCtrl items text into ComboBox to be sure that item in TreeCtrl is from its combo list.
            //pCB->InsertString( 0, lpszItem );
            //m_mapExControls[ hti ] = pCB;
         //} // if ( pCB->Create( WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST, CRect(), this, 0 ) )'s end
      }
   }


      //(flags & TVHT_ONITEMLABEL) &&
      //ShowComboBox(GetSelectedItem())
    // )
      //return;
}

void CTreeListCtrl::OnItemDblClk(int selectedColumn,CPoint point,int left,int right) 
{
   //UINT flags;
   //HTREEITEM clickedItem = HitTest(point,&flags);

   //if (clickedItem != NULL)
   //{
   //   delete m_treeListComboBox;

   //   CRect itemRect;
   //   GetItemRect(clickedItem,&itemRect,TRUE);
   //   itemRect.left  = left;
   //   itemRect.right = right;

   //   m_treeListComboBox = new CTreeListComboBox(selectedColumn);
   //   DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL | CBS_DROPDOWN;

   //   if (m_treeListComboBox->Create(dwComboBoxStyle,itemRect,this,0))
   //   {
   //      //pCB->SetWindowPos( NULL, iComboLeft, itemRect.top, iComboWidth, m_iComboDroppedHeight, SWP_NOZORDER );

   //      // Set the same font as CTreeCtrl has.
   //      m_treeListComboBox->SetFont(GetFont(),FALSE);

   //      // Select current item in combo box if it's found.
   //      //CString strItemText = GetItemText(hti);
   //      //int iFIndex = pCB->FindString( -1, strItemText );
   //      //if ( iFIndex != CB_ERR )
   //        // pCB->SetCurSel( iFIndex );
   //      //else
   //        // pCB->SetWindowText( strItemText );

   //      m_treeListComboBox->ShowWindow(SW_SHOWNORMAL);
   //      m_treeListComboBox->ShowDropDown(TRUE);
   //      m_treeListComboBox->SetFocus();

   //      // Add TreeCtrl items text into ComboBox to be sure that item in TreeCtrl is from its combo list.
   //      //pCB->InsertString( 0, lpszItem );
   //      //m_mapExControls[ hti ] = pCB;
   //   } // if ( pCB->Create( WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST, CRect(), this, 0 ) )'s end
   //}


      //(flags & TVHT_ONITEMLABEL) &&
      //ShowComboBox(GetSelectedItem())
    // )
      //return;
}

void CTreeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if ((GetColumnsNum() == 0) || (point.x < GetColumnWidth(0)))
   {
      //CTreeCtrl::OnLButtonDblClk(nFlags, point);
      ResetVertScrollBar();

      SetFocus();
   }
   else
   {
      int xLeft = 0;
      int columnWidth = 0;
      int selectedColumn = -1;

      for (int column = 0;column < GetColumnsNum();column++)
      {
         columnWidth = GetColumnWidth(column);

         if (point.x >= xLeft && point.x < xLeft + columnWidth)
         {
            selectedColumn = column;
            break;
         }

         xLeft += columnWidth;
      }

      if (selectedColumn >= 0)
      {
         OnItemDblClk(selectedColumn,point,xLeft,xLeft + columnWidth);
      }

      //CTreeCtrl::OnLButtonDblClk(nFlags, point);
   }
}

void CTreeListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
   ResetVertScrollBar();
}

BOOL CTreeListCtrl::SetItemData(HTREEITEM hItem, DWORD dwData)
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return FALSE;

   pItem->itemData = dwData;

   return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

DWORD CTreeListCtrl::GetItemData(HTREEITEM hItem) const
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return NULL;

   return pItem->itemData;
}

DWORD CTreeListCtrl::GetTreeItemData(HTREEITEM hItem) const
{
   DWORD itemData = CTreeCtrl::GetItemData(hItem);

   return itemData;
}

HTREEITEM CTreeListCtrl::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
   CTreeListItem *pItem = newTreeListItem();
   pItem->InsertItem(lpszItem);
   m_nItems++;
   ((CTreeListFrame*)GetParent())->ResetScrollBar();

   return CTreeCtrl::InsertItem(TVIF_PARAM|TVIF_TEXT, "", 0, 0, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM CTreeListCtrl::InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
   CTreeListItem *pItem = newTreeListItem();
   pItem->InsertItem(lpszItem);
   m_nItems++;
   ((CTreeListFrame*)GetParent())->ResetScrollBar();
   
   return CTreeCtrl::InsertItem(TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE, "", nImage, nSelectedImage, 0, 0, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM CTreeListCtrl::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter )
{
   CTreeListItem *pItem = newTreeListItem();
   pItem->InsertItem(lpszItem);
   pItem->itemData = lParam;
   m_nItems++;
   ((CTreeListFrame*)GetParent())->ResetScrollBar();
   
   return CTreeCtrl::InsertItem(nMask, "", nImage, nSelectedImage, nState, nStateMask, (LPARAM)pItem, hParent, hInsertAfter);
}

HTREEITEM CTreeListCtrl::CopyItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
   if (ItemHasChildren(hItem))
      return NULL;

   TV_ITEM item;
   item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
   item.hItem = hItem;
   GetItem(&item);
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);
   CTreeListItem *pNewItem = ((pItem != NULL) ? newTreeListItem(*pItem) : NULL);

   item.lParam = (LPARAM)pNewItem;

   TV_INSERTSTRUCT insStruct;
   insStruct.item = item;
   insStruct.hParent = hParent;
   insStruct.hInsertAfter = hInsertAfter;

   return CTreeCtrl::InsertItem(&insStruct);
}


HTREEITEM CTreeListCtrl::MoveItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
   if (ItemHasChildren(hItem))
      return NULL;

   TV_ITEM item;
   item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
   item.hItem = hItem;
   GetItem(&item);
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);
   CTreeListItem *pNewItem = ((pItem != NULL) ? newTreeListItem(*pItem) : NULL);
   DeleteItem(hItem);

   item.lParam = (LPARAM)pNewItem;

   TV_INSERTSTRUCT insStruct;
   insStruct.item = item;
   insStruct.hParent = hParent;
   insStruct.hInsertAfter = hInsertAfter;

   return CTreeCtrl::InsertItem(&insStruct);
}

BOOL CTreeListCtrl::SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem )
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return FALSE;

   pItem->SetSubstring(nCol, lpszItem);

   return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

BOOL CTreeListCtrl::SetItemColor( HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate )
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return FALSE;

   pItem->m_Color = m_newColor;

   if (!CTreeCtrl::SetItemData(hItem, (LPARAM)pItem))
      return FALSE;

   if (m_bInvalidate)
      Invalidate();

   return TRUE;
}

BOOL CTreeListCtrl::SetItemBold( HTREEITEM hItem, BOOL m_Bold, BOOL m_bInvalidate )
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return FALSE;

   pItem->m_Bold = m_Bold;

   if (!CTreeCtrl::SetItemData(hItem, (LPARAM)pItem))
      return FALSE;

   if (m_bInvalidate)
      Invalidate();

   return TRUE;
}

CString CTreeListCtrl::GetItemText( HTREEITEM hItem, int nSubItem )
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return _T("");

   return pItem->GetSubstring(*this,hItem,nSubItem);
}

CString CTreeListCtrl::GetItemText( int nItem, int nSubItem )
{
   return GetItemText(GetTreeItem(nItem), nSubItem);
}

BOOL CTreeListCtrl::DeleteItem( HTREEITEM hItem )
{
   CTreeListItem *pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

   if (!pItem)
      return FALSE;

   m_nItems -- ;

   return CTreeCtrl::DeleteItem(hItem);
}

BOOL CTreeListCtrl::DeleteItem( int nItem )
{
   return DeleteItem(GetTreeItem(nItem));
}

int CALLBACK CTreeListCtrl::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CTreeListItem *pItem1 = (CTreeListItem *)lParam1;
   CTreeListItem *pItem2 = (CTreeListItem *)lParam2;

   SSortType *pSortType = (SSortType *)lParamSort;

   CString str1 = pItem1->GetSubstring(pSortType->nCol);
   CString str2 = pItem2->GetSubstring(pSortType->nCol);

   int m_nComp;
   // compare the two strings, but
   // notice:
   // in this case, "xxxx10" comes after "xxxx2"
   {
      CString tmpStr1, tmpStr2;
      int index = str1.FindOneOf("0123456789");
      if (index!=-1)
         tmpStr1 = str1.Right(str1.GetLength()-index);
      index = str2.FindOneOf("0123456789");
      if (index!=-1)
         tmpStr2 = str2.Right(str2.GetLength()-index);

      tmpStr1 = tmpStr1.SpanIncluding("0123456789");
      tmpStr2 = tmpStr2.SpanIncluding("0123456789");

      if ((tmpStr1=="") && (tmpStr2==""))
         m_nComp = str1.CompareNoCase(str2);
      else
      {
         int num1 = atoi(tmpStr1);
         int num2 = atoi(tmpStr2);

         tmpStr1 = str1.SpanExcluding("0123456789");
         tmpStr2 = str2.SpanExcluding("0123456789");

         if (tmpStr1 == tmpStr2)
         {
            if (num1 > num2)
               m_nComp = 1;
            else
            if (num1 < num2)
               m_nComp = -1;
            else
               m_nComp = str1.CompareNoCase(str2);
         }
         else
            m_nComp = str1.CompareNoCase(str2);       
      }
   }

   if (!pSortType->bAscending)
   {
      if (m_nComp == 1)
         m_nComp = -1;
      else
      if (m_nComp == -1)
         m_nComp = 1;
   }

   return m_nComp;
}

BOOL CTreeListCtrl::SortItems( int nCol, BOOL bAscending, HTREEITEM low)
{
   TV_SORTCB tSort;

   tSort.hParent = low;
   tSort.lpfnCompare = CompareFunc;

   SSortType *pSortType = new SSortType;
   pSortType->nCol = nCol;
   pSortType->bAscending = bAscending;
   tSort.lParam = (LPARAM)pSortType;

   BOOL m_bReturn = SortChildrenCB(&tSort);

   delete pSortType;

   return m_bReturn;
}

void CTreeListCtrl::MemDeleteAllItems(HTREEITEM hParent)
{
   HTREEITEM hItem = hParent;
   CTreeListItem *pItem;

   while (hItem != NULL)
   {
      pItem = (CTreeListItem *)CTreeCtrl::GetItemData(hItem);

      delete pItem;

      CTreeCtrl::SetItemData(hItem,NULL);

      if (ItemHasChildren(hItem))
      {
         MemDeleteAllItems(GetChildItem(hItem));
      }

      hItem = GetNextSiblingItem(hItem);
   }
}

BOOL CTreeListCtrl::DeleteAllItems()
{
   LockWindowUpdate();
   BeginWaitCursor();

   MemDeleteAllItems(GetRootItem());
   BOOL m_bReturn = CTreeCtrl::DeleteAllItems();

   EndWaitCursor();
   UnlockWindowUpdate();
   return m_bReturn;
}

/*void CTreeListCtrl::OnDestroy()
{
   MemDeleteAllItems(GetRootItem());

   CTreeCtrl::OnDestroy();
}*/

void CTreeListCtrl::setComboBoxAt(int column,CTreeListComboBox* comboBox)
{
   m_comboBoxArray.SetAtGrow(column,comboBox);
}

CTreeListComboBox* CTreeListCtrl::getComboBoxAt(int column)
{
   CTreeListComboBox* comboBox = NULL;

   if (column >= 0 && column < m_comboBoxArray.GetSize())
   {
      comboBox = m_comboBoxArray.GetAt(column);
   }

   return comboBox;
}

int CTreeListCtrl::getLevel(HTREEITEM item) const
{
   int level = 0;

   while (item != NULL)
   {
      item = GetParentItem(item);

      if (item != NULL)
      {
         level++;
      }
   }

   return level;
}

HTREEITEM CTreeListCtrl::getAncestorAtLevel(HTREEITEM item,int level) const
{
   HTREEITEM ancestor = NULL;
   CTypedPtrArray<CPtrArray,HTREEITEM> levels;

   while (item != NULL)
   {
      item = GetParentItem(item);

      if (item != NULL)
      {
         levels.Add(item);
      }
   }

   if (levels.GetCount() > level)
   {
      ancestor = levels.GetAt(levels.GetCount() - 1 - level);
   }

   return ancestor;
}

CTreeListItem* CTreeListCtrl::newTreeListItem()
{
   return new CTreeListItem();
}

CTreeListItem* CTreeListCtrl::newTreeListItem(const CTreeListItem& other)
{
   return new CTreeListItem(other);
}

//void CTreeListCtrl::updateColumn(int column,const CString& text)
//{
//   DWORD itemData = GetItemData(m_selectedItem);
//
//   if (itemData != NULL)
//   {
//      CStencilPin* stencilHole = (CStencilPin*)itemData;
//   }
//}




