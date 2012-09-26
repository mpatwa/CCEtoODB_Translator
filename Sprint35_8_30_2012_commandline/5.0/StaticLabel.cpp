// $Header: /CAMCAD/4.5/StaticLabel.cpp 5     12/19/05 3:04a Kurt Van Ness $

#include "StdAfx.h"
#include "Resource.h"
#include "StaticLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
CStaticLabel::CStaticLabel()
: m_bkColor(0)
{
	m_crText = GetSysColor(COLOR_WINDOWTEXT);
	m_hBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(m_lf),&m_lf);

	m_font.CreateFontIndirect(&m_lf);
	m_bTimer = FALSE;
	m_bState = FALSE;
	m_bLink = TRUE;
	m_hCursor = NULL;
	m_Type = None;

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
}


CStaticLabel::~CStaticLabel()
{
	m_font.DeleteObject();
	::DeleteObject(m_hBrush);
}

CStaticLabel& CStaticLabel::SetText(const CString& strText)
{
	SetWindowText(strText);
	return *this;
}

CStaticLabel& CStaticLabel::SetTextColor(COLORREF crText)
{
	m_crText = crText;
	RedrawWindow();
	return *this;
}

CStaticLabel& CStaticLabel::SetFontBold(BOOL bBold)
{	
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	RedrawWindow();
	return *this;
}

CStaticLabel& CStaticLabel::SetFontUnderline(BOOL bSet)
{	
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	RedrawWindow();
	return *this;
}

CStaticLabel& CStaticLabel::SetFontItalic(BOOL bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
	RedrawWindow();
	return *this;	
}

CStaticLabel& CStaticLabel::SetSunken(BOOL bSet)
{
	if (!bSet)
		ModifyStyleEx(WS_EX_STATICEDGE,0,SWP_DRAWFRAME);
	else
		ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_DRAWFRAME);
		
	return *this;	
}

CStaticLabel& CStaticLabel::SetBorder(BOOL bSet)
{
	if (!bSet)
		ModifyStyle(WS_BORDER,0,SWP_DRAWFRAME);
	else
		ModifyStyle(0,WS_BORDER,SWP_DRAWFRAME);
		
	return *this;	
}

CStaticLabel& CStaticLabel::SetFontSize(int nSize)
{
	nSize*=-1;
	m_lf.lfHeight = nSize;
	ReconstructFont();
	RedrawWindow();
	return *this;
}


CStaticLabel& CStaticLabel::SetBkColor(COLORREF crBkgnd)
{
   m_bkColor = crBkgnd;

	if (m_hBrush)
		::DeleteObject(m_hBrush);
	
	m_hBrush = ::CreateSolidBrush(crBkgnd);
	return *this;
}

CStaticLabel& CStaticLabel::SetFontName(const CString& strFont)
{	
	strcpy(m_lf.lfFaceName,strFont);
	ReconstructFont();
	RedrawWindow();
	return *this;
}


BEGIN_MESSAGE_MAP(CStaticLabel, CStatic)
	//{{AFX_MSG_MAP(CStaticLabel)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticLabel message handlers

HBRUSH CStaticLabel::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SelectObject(&m_font);
		pDC->SetTextColor(m_crText);
		pDC->SetBkMode(TRANSPARENT);
	}


	if (m_Type == Background)
	{
		if (!m_bState)
			return m_hwndBrush;
	}

	return m_hBrush;
}

void CStaticLabel::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);

	ASSERT(bCreated);
}


CStaticLabel& CStaticLabel::FlashText(BOOL bActivate)
{
	if (m_bTimer)
	{
		SetWindowText(m_strText);
		KillTimer(1);
	}

	if (bActivate)
	{
		GetWindowText(m_strText);
		m_bState = FALSE;
		
		m_bTimer = TRUE;
		SetTimer(1,500,NULL);
		m_Type = Text;
	}

	return *this;
}

CStaticLabel& CStaticLabel::FlashBackground(BOOL bActivate)
{

	if (m_bTimer)
		KillTimer(1);

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;
		SetTimer(1,500,NULL);

		m_Type = Background;
	}

	return *this;
}


void CStaticLabel::OnTimer(UINT nIDEvent) 
{
	m_bState = !m_bState;

	switch (m_Type)
	{
		case Text:
			if (m_bState)
				SetWindowText("");
			else
				SetWindowText(m_strText);
		break;

		case Background:
			InvalidateRect(NULL,FALSE);
			UpdateWindow();
		break;
	}
	
	CStatic::OnTimer(nIDEvent);
}

CStaticLabel& CStaticLabel::SetLink(BOOL bLink)
{
	m_bLink = bLink;

	if (bLink)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);

	return *this;
}

void CStaticLabel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CString strLink;

	GetWindowText(strLink);
	ShellExecute(NULL,"open",strLink,NULL,NULL,SW_SHOWNORMAL);
		
	CStatic::OnLButtonDown(nFlags, point);
}

BOOL CStaticLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

CStaticLabel& CStaticLabel::SetLinkCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
	return *this;
}

//_____________________________________________________________________________
CStaticCounter::CStaticCounter()
: m_count(0)
{
}


CStaticCounter::~CStaticCounter()
{
}

int CStaticCounter::getCount() const
{
   return m_count;
}

void CStaticCounter::setCount(int count)
{
   m_count = count;

   CString text;
   text.Format("%d",m_count);
   SetText(text);
}

int CStaticCounter::incrementCount(int delta)
{
   setCount(getCount() + delta);

   return getCount();
}

int CStaticCounter::decrementCount(int delta)
{
   setCount(getCount() - delta);

   return getCount();
}


