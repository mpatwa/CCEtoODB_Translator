// $Header: /CAMCAD/4.5/TreeListFrame.cpp 8     2/24/05 6:37p Kurt Van Ness $

#include "StdAfx.h"
#include "TreeListFrame.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListFrame

CTreeListFrame::CTreeListFrame()
{
	m_RTL = FALSE;
   m_sortingEnabled = false;
   m_tree = NULL;
}

CTreeListFrame::~CTreeListFrame()
{
   delete m_tree;
}

CTreeListCtrl& CTreeListFrame::getTree()
{
   if (m_tree == NULL)
   {
      m_tree = new CTreeListCtrl();
   }

   return *m_tree;
}


BEGIN_MESSAGE_MAP(CTreeListFrame, CWnd)
	//{{AFX_MSG_MAP(CTreeListFrame)
	ON_WM_HSCROLL()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTreeListFrame message handlers

LONG FAR PASCAL CTreeListFrame::DummyWndProc(HWND h, UINT u, WPARAM w, LPARAM l)
{
	return ::DefWindowProc(h, u, w, l);
}

void CTreeListFrame::RegisterClass()
{
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	
	wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW | CS_GLOBALCLASS;
	wc.lpfnWndProc = DummyWndProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hCursor = 0;
	wc.lpszClassName = "LANTIVTREELISTCTRL";
	wc.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
	
	if (!::RegisterClass(&wc))
	{
		ASSERT(FALSE);
	}
}

BOOL CTreeListFrame::SubclassDlgItem(UINT nID, CWnd* parent)
{
	if (!CWnd::SubclassDlgItem(nID, parent)) return FALSE;
	Initialize();
	return TRUE;
}

void CTreeListFrame::Initialize()
{
	// creates all the objects in frame -
	// header, tree, horizontal scroll bar

   int scrollBarWidth  = GetSystemMetrics(SM_CXHSCROLL);
   int scrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);

	CRect m_wndRect;
	GetWindowRect(&m_wndRect);
	CRect m_headerRect;

	// create the header
	{
		//m_headerRect.left = m_headerRect.top = -1;
		m_headerRect.left = m_headerRect.top = 0;
		m_headerRect.right = m_wndRect.Width();

		getTree().m_wndHeader.Create(WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ, m_headerRect, this, ID_TREE_LIST_HEADER);
	}

	CSize textSize;
	// set header's pos, dimensions and image list
	{
		LOGFONT logfont;

		CFont *pFont = GetParent()->GetFont();
		pFont->GetLogFont( &logfont );

		getTree().m_headerFont.CreateFontIndirect( &logfont );
		getTree().m_wndHeader.SetFont(&getTree().m_headerFont);

		CDC *pDC = getTree().m_wndHeader.GetDC();
		pDC->SelectObject(&getTree().m_headerFont);
		textSize = pDC->GetTextExtent("A");

		//getTree().m_wndHeader.SetWindowPos(&wndTop, -1, -1, m_headerRect.Width(), textSize.cy+4, SWP_SHOWWINDOW);
		getTree().m_wndHeader.SetWindowPos(&wndTop, 0, 0, m_headerRect.Width(), textSize.cy+4, SWP_SHOWWINDOW);

		getTree().m_cImageList.Create(IDB_HEADERSORTARROWS, 16, 10, RGB(192,192,192));
		getTree().m_wndHeader.SetImageList(&getTree().m_cImageList);
	}

	CRect m_treeRect;

	// create the tree itself
	{
		GetWindowRect(&m_wndRect);

		m_treeRect.left   = 0;
		m_treeRect.top    = textSize.cy + 4;
		m_treeRect.right  = m_headerRect.Width() - 5;
		//m_treeRect.bottom = m_wndRect.Height() - GetSystemMetrics(SM_CYHSCROLL) - 4;
		m_treeRect.bottom = m_wndRect.Height() - GetSystemMetrics(SM_CYHSCROLL);

		getTree().Create(WS_BORDER | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, m_treeRect, this, ID_TREE_LIST_CTRL);
	}

	// finally, create the horizontal scroll bar
	{
		CRect m_scrollRect;
		m_scrollRect.left   = 0;
		m_scrollRect.top    = m_treeRect.bottom;
		int scrollRectTop   = m_wndRect.bottom   - GetSystemMetrics(SM_CYHSCROLL);
		m_scrollRect.right  = m_treeRect.Width() - GetSystemMetrics(SM_CXVSCROLL);
		m_scrollRect.bottom = m_wndRect.bottom;

		m_horScrollBar.Create(WS_CHILD | WS_VISIBLE | WS_DISABLED | SBS_HORZ | SBS_TOPALIGN, m_scrollRect, this, ID_TREE_LIST_SCROLLBAR);
	}

	SortTree(0, TRUE, getTree().GetRootItem());
}

BOOL CTreeListFrame::VerticalScrollVisible()
{
	int sMin, sMax;
	getTree().GetScrollRange(SB_VERT, &sMin, &sMax);
	return sMax!=0;
}

BOOL CTreeListFrame::HorizontalScrollVisible()
{
	int sMin, sMax;
	m_horScrollBar.GetScrollRange(&sMin, &sMax);
	return sMax!=0;
}

int CTreeListFrame::StretchWidth(int m_nWidth, int m_nMeasure)
{
	return ((m_nWidth/m_nMeasure)+1)*m_nMeasure;
}


void CTreeListFrame::ResetScrollBar()
{
	// resetting the horizontal scroll bar

	int m_nTotalWidth=0, m_nPageWidth;

	CRect m_treeRect;
	getTree().GetClientRect(&m_treeRect);

	CRect m_wndRect;
	GetClientRect(&m_wndRect);

	CRect m_headerRect;
	getTree().m_wndHeader.GetClientRect(&m_headerRect);

	CRect m_barRect;
	m_horScrollBar.GetClientRect(m_barRect);

	m_nPageWidth = m_treeRect.Width();

	m_nTotalWidth = getTree().GetColumnsWidth();

	if(m_nTotalWidth > m_nPageWidth)
	{
		// show the scroll bar and adjust it's size
		{
			m_horScrollBar.EnableWindow(TRUE);

			m_horScrollBar.ShowWindow(SW_SHOW);

			// the tree becomes smaller
			getTree().SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

			if(!VerticalScrollVisible())
				// i.e. vertical scroll bar isn't visible
			{
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_barRect.Height(), SWP_NOMOVE);
			}
			else
			{
				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
			}
		}

		m_horScrollBar.SetScrollRange(0, m_nTotalWidth-m_nPageWidth);

		// recalculate the offset
		{
			CRect m_wndHeaderRect;
			getTree().m_wndHeader.GetWindowRect(&m_wndHeaderRect);
			ScreenToClient(&m_wndHeaderRect);

			getTree().m_nOffset = m_wndHeaderRect.left;
			m_horScrollBar.SetScrollPos(-getTree().m_nOffset);
		}
	}
	else
	{
		m_horScrollBar.EnableWindow(FALSE);

		// we no longer need it, so hide it!
		{
			m_horScrollBar.ShowWindow(SW_HIDE);

			getTree().SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_headerRect.Height(), SWP_NOMOVE);
			// the tree takes scroll's place
		}

		m_horScrollBar.SetScrollRange(0, 0);

		// set scroll offset to zero
		{
			getTree().m_nOffset = 0;
			getTree().Invalidate();
			CRect m_headerRect;
			getTree().m_wndHeader.GetWindowRect(&m_headerRect);
			CRect m_wndRect;
			GetClientRect(&m_wndRect);
			getTree().m_wndHeader.SetWindowPos(&wndTop, getTree().m_nOffset, 0, max(StretchWidth(getTree().GetColumnsWidth(),m_wndRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
		}
	}
}

void CTreeListFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect m_treeRect;
	getTree().GetClientRect(&m_treeRect);

	if(pScrollBar == &m_horScrollBar)
	{
		int m_nCurPos = m_horScrollBar.GetScrollPos();
		int m_nPrevPos = m_nCurPos;
		switch(nSBCode)
		{
			case SB_LEFT:			m_nCurPos = 0;
									break;
			case SB_RIGHT:			m_nCurPos = m_horScrollBar.GetScrollLimit()-1;
									break;
			case SB_LINELEFT:		m_nCurPos = max(m_nCurPos-6, 0);
									break;
			case SB_LINERIGHT:		m_nCurPos = min(m_nCurPos+6, m_horScrollBar.GetScrollLimit()-1);
									break;
			case SB_PAGELEFT:		m_nCurPos = max(m_nCurPos-m_treeRect.Width(), 0);
									break;
			case SB_PAGERIGHT:		m_nCurPos = min(m_nCurPos+m_treeRect.Width(), m_horScrollBar.GetScrollLimit()-1);
									break;
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:  if(nPos==0)
									    m_nCurPos = 0;
								    else
									    m_nCurPos = min(StretchWidth(nPos, 6), m_horScrollBar.GetScrollLimit()-1);
								    break;
		}
		// 6 is Microsoft's step in a CListCtrl for example

		m_horScrollBar.SetScrollPos(m_nCurPos);
		getTree().m_nOffset = -m_nCurPos;

		// smoothly scroll the tree control
		{
			CRect m_scrollRect;
			getTree().GetClientRect(&m_scrollRect);
			getTree().ScrollWindow(m_nPrevPos - m_nCurPos, 0, &m_scrollRect, &m_scrollRect);
		}

		CRect m_headerRect;
		getTree().m_wndHeader.GetWindowRect(&m_headerRect);
		CRect m_wndRect;
		GetClientRect(&m_wndRect);

		getTree().m_wndHeader.SetWindowPos(&wndTop, getTree().m_nOffset, 0, max(StretchWidth(getTree().GetColumnsWidth(),m_treeRect.Width()),m_wndRect.Width()), m_headerRect.Height(), SWP_SHOWWINDOW);
	}
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTreeListFrame::SortTree(int nCol, BOOL bAscending, HTREEITEM hParent)
{
   if (m_sortingEnabled)
   {
	   HTREEITEM hChild;
	   HTREEITEM hPa = hParent;

	   while (hPa != NULL)
	   {
		   if(getTree().ItemHasChildren(hPa))
		   {
			   hChild = getTree().GetChildItem(hPa);
			   getTree().SortItems(nCol, bAscending, hPa);
			   SortTree(nCol, bAscending, hChild);
		   }

		   hPa = getTree().GetNextSiblingItem(hPa);
	   }
   }
}

BOOL CTreeListFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;

	if((wParam == ID_TREE_LIST_HEADER) && (pHDN->hdr.code == HDN_ITEMCLICK))
	{
      if (m_sortingEnabled)
      {
		   int nCol = pHDN->iItem;

		   BOOL bAscending = FALSE;

		   if(getTree().m_wndHeader.GetItemImage(nCol)==-1)
			   bAscending = TRUE;
		   else
		   if(getTree().m_wndHeader.GetItemImage(nCol)==1)
			   bAscending = TRUE;

		   for(int i=0;i<getTree().GetColumnsNum();i++)
		   {
			   getTree().m_wndHeader.SetItemImage(i, -1);
		   }

		   if(bAscending)
			   getTree().m_wndHeader.SetItemImage(nCol, 0);
		   else
			   getTree().m_wndHeader.SetItemImage(nCol, 1);

		   getTree().SortItems(nCol, bAscending, NULL);
		   SortTree(nCol, bAscending, getTree().GetRootItem());

		   getTree().UpdateWindow();
      }
	}
	else if((wParam == ID_TREE_LIST_HEADER) && (pHDN->hdr.code == HDN_ITEMCHANGED))
	{
		int m_nPrevColumnsWidth = getTree().GetColumnsWidth();
		getTree().RecalcColumnsWidth();
		ResetScrollBar();

		// in case we were at the scroll bar's end,
		// and some column's width was reduced,
		// update header's position (move to the right).
		CRect m_treeRect;
		getTree().GetClientRect(&m_treeRect);

		CRect m_headerRect;
		getTree().m_wndHeader.GetClientRect(&m_headerRect);

		if((m_nPrevColumnsWidth > getTree().GetColumnsWidth()) &&
		   (m_horScrollBar.GetScrollPos() == m_horScrollBar.GetScrollLimit()-1) &&
		   (m_treeRect.Width() < getTree().GetColumnsWidth()))
		{
			getTree().m_nOffset = -getTree().GetColumnsWidth()+m_treeRect.Width();
			getTree().m_wndHeader.SetWindowPos(&wndTop, getTree().m_nOffset, 0, 0, 0, SWP_NOSIZE);
		}

		getTree().Invalidate();
		GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
	}
	else
   {
		GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
   }

	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CTreeListFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd, 0);
}

void CTreeListFrame::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// resize all the controls
	{
		CRect m_wndRect;
		GetClientRect(&m_wndRect);

		CRect m_headerRect;
		getTree().m_wndHeader.GetClientRect(&m_headerRect);
		getTree().m_wndHeader.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_headerRect.Height(), SWP_NOMOVE);

		CRect m_scrollRect;
		m_horScrollBar.GetClientRect(&m_scrollRect);

		getTree().SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_scrollRect.Height(), SWP_NOMOVE);

		CRect m_treeRect;
		getTree().GetClientRect(&m_treeRect);
		m_horScrollBar.MoveWindow(0, m_treeRect.bottom, m_wndRect.Width(), m_scrollRect.Height());

		if(getTree().GetColumnsWidth() > m_treeRect.Width())
		{
			// show the horz scroll bar
			{
				CRect m_barRect;
				m_horScrollBar.GetClientRect(&m_barRect);

				m_horScrollBar.EnableWindow(TRUE);

				m_horScrollBar.ShowWindow(SW_SHOW);

				// the tree becomes smaller
				getTree().SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_barRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

				m_horScrollBar.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width() - GetSystemMetrics(SM_CXVSCROLL), m_barRect.Height(), SWP_NOMOVE);
			}

			m_horScrollBar.SetScrollRange(0, getTree().GetColumnsWidth()-m_treeRect.Width());

		}
		else
		{
			// hide the scroll bar
			{
				m_horScrollBar.EnableWindow(FALSE);

				m_horScrollBar.ShowWindow(SW_HIDE);

				// the tree becomes larger
				getTree().SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height()-m_headerRect.Height(), SWP_NOMOVE);

			}

			m_horScrollBar.SetScrollRange(0, 0);
		}

		getTree().ResetVertScrollBar();
	}
}

void CTreeListFrame::OnMove(int x, int y) 
{
	CWnd::OnMove(x, y);
	
	// resize all the controls
	{
		CRect m_wndRect;
		GetClientRect(&m_wndRect);

		CRect m_headerRect;
		getTree().m_wndHeader.GetClientRect(&m_headerRect);
		getTree().m_wndHeader.SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_headerRect.Height(), SWP_NOMOVE);

		CRect m_scrollRect;
		m_horScrollBar.GetClientRect(&m_scrollRect);

		getTree().SetWindowPos(&wndTop, 0, 0, m_wndRect.Width(), m_wndRect.Height() - m_scrollRect.Height(), SWP_NOMOVE);

		CRect m_treeRect;
		getTree().GetClientRect(&m_treeRect);
		m_horScrollBar.SetWindowPos(&wndTop, 0, m_treeRect.bottom, m_wndRect.Width(), m_scrollRect.Height(), SWP_SHOWWINDOW);

		getTree().ResetVertScrollBar();
	}
}

BOOL CTreeListFrame::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult )
{

	return CWnd::OnChildNotify( message, wParam, lParam, pLResult );
}