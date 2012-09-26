// $Header: /CAMCAD/4.5/GeneralDialog.cpp 5     5/02/06 3:14p Kurt Van Ness $

#include "StdAfx.h"
#include "GeneralDialog.h"
#include "mainfrm.h"
#include "PersistantColorDialog.h"

//_____________________________________________________________________________
void ErrorAccess(const char *caption)
{
   AccessCodeError dlg;
   dlg.caption = caption;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// InputDlg dialog
InputDlg::InputDlg(CWnd* pParent /*=NULL*/)
   : CDialog(InputDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(InputDlg)
   m_input = _T("");
   m_prompt = _T("");
   //}}AFX_DATA_INIT
}

void InputDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(InputDlg)
   DDX_Text(pDX, IDC_EDIT, m_input);
   DDX_Text(pDX, IDC_PROMPT, m_prompt);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(InputDlg, CDialog)
   //{{AFX_MSG_MAP(InputDlg)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog
CSelectDlg::CSelectDlg(CString prompt, bool allowMultipleSelect, bool sortList, CWnd* pParent /*=NULL*/)
   : CDialog(CSelectDlg::IDD, pParent)
{
	m_listBox = NULL;
   m_sPrompt = prompt;
	m_bAllowMultipleSelect = allowMultipleSelect;
	m_bSortList = sortList;
	m_allItemList.empty();
	m_selItemList.empty();
}

CSelectDlg::~CSelectDlg()
{
	delete m_listBox;
	m_listBox = NULL;
	m_allItemList.empty();
	m_selItemList.empty();
}

void CSelectDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_PROMPT, m_sPrompt);
}

BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
   ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSelectDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

	CRect rect;
	GetDlgItem( IDC_STATIC_LISTBOX )->GetWindowRect( &rect );
	ScreenToClient( &rect );

	m_listBox = new CListBox;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|LBS_STANDARD|WS_HSCROLL;
	if (!m_bSortList)
		dwStyle &= ~LBS_SORT;
		
	m_listBox->Create(dwStyle, rect, this, IDC_LIST1);

   POSITION pos = m_allItemList.GetHeadPosition();
   while (pos != NULL)
	{
		CSelItem *item = m_allItemList.GetNext(pos);
		if (item == NULL)
			continue;

      int index = m_listBox->AddString(item->GetName());
		m_listBox->SetItemData(index, (DWORD)item);
	}

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectDlg::AddItem(CSelItem *item)
{
	m_allItemList.AddTail(item);
}

void CSelectDlg::OnDblclkList() 
{
   OnBnClickedOk();  
}

void CSelectDlg::OnBnClickedOk()
{
	int nCount = m_listBox->GetSelCount();
	int selIndex = m_listBox->GetCurSel();
	if (nCount < 1 && selIndex < 0)
		return;

	if (nCount > 0)
	{
		CArray<int,int> aryListBoxSel;
		aryListBoxSel.SetSize(nCount, nCount);
		m_listBox->GetSelItems(nCount, aryListBoxSel.GetData());

		for (int i=0; i<nCount; i++)
		{
			CSelItem *item = (CSelItem *)m_listBox->GetItemData(aryListBoxSel.GetAt(i));
			if (item == NULL)
				continue;

			item->SetSelected(TRUE);
			m_allItemList.RemoveAt(m_allItemList.Find(item));
			m_selItemList.AddTail(item);
		}
	}
	else
	{
		CSelItem *item = (CSelItem *)m_listBox->GetItemData(selIndex);
		if (item != NULL)
		{
			item->SetSelected(TRUE);
			m_allItemList.RemoveAt(m_allItemList.Find(item));
			m_selItemList.AddTail(item);
		}
	}

	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// AccessCodeError dialog
AccessCodeError::AccessCodeError(CWnd* pParent /*=NULL*/)
   : CDialog(AccessCodeError::IDD, pParent)
{
   //{{AFX_DATA_INIT(AccessCodeError)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void AccessCodeError::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AccessCodeError)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AccessCodeError, CDialog)
   //{{AFX_MSG_MAP(AccessCodeError)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccessCodeError message handlers
BOOL AccessCodeError::OnInitDialog() 
{
   CDialog::OnInitDialog();

   SetWindowText(caption); 
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}



////////////////////////////////////////////////////////////////////////////////
// CSelItem Class
////////////////////////////////////////////////////////////////////////////////
CSelItem::CSelItem(CString name, BOOL selected)
{
	m_name = name; 
	m_selected = selected;
}

CString CSelItem::GetName()const
{
	return m_name;
}

BOOL CSelItem::GetSelect()const
{
	return m_selected;
}

void CSelItem::SetSelected(BOOL selected)
{
	m_selected = selected;
}



////////////////////////////////////////////////////////////////////////////////
// CMultiSelectDlg dialog
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMultiSelectDlg, CDialog)
CMultiSelectDlg::CMultiSelectDlg(CString caption, CWnd* pParent /*=NULL*/)
	: CDialog(CMultiSelectDlg::IDD, pParent)
{
	m_allItemList.empty();
	m_resultItemList.empty();
}

CMultiSelectDlg::~CMultiSelectDlg()
{
	m_allItemList.empty();
	m_resultItemList.empty();
}

void CMultiSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AVAILABLE, m_lstAvailable);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_lstSelected);
}

BEGIN_MESSAGE_MAP(CMultiSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CMultiSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lstAvailable.ResetContent();
	m_lstSelected.ResetContent();

	POSITION pos = m_allItemList.GetHeadPosition();
	while (pos)
	{
		CSelItem *item = m_allItemList.GetNext(pos);
		if (item->GetSelect())
		{
			// Add to Selected listbox
			int index = m_lstSelected.AddString(item->GetName());
			m_lstSelected.SetItemData(index, (DWORD)item);
		}
		else
		{
			// Add to Available listbox
			CString name = item->GetName();
			int index = m_lstAvailable.AddString(name);
			m_lstAvailable.SetItemData(index, (DWORD)item);
		}
	}

	SetWindowText(m_caption);

	return TRUE; 
}

void CMultiSelectDlg::AddItem(CSelItem *item)
{
	m_allItemList.AddTail(item);
}

void CMultiSelectDlg::OnBnClickedButtonAdd()
{
	int nCount = m_lstAvailable.GetSelCount();
	if (nCount < 1)
		return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount, nCount);
	m_lstAvailable.GetSelItems(nCount, aryListBoxSel.GetData());

	// Removed the item from Available listbox and add it into Selected listbox
	for (int i=nCount-1; i>-1; i--)
	{
		CSelItem *item = (CSelItem *)m_lstAvailable.GetItemData(aryListBoxSel.GetAt(i));
		item->SetSelected(TRUE);

		int index = m_lstSelected.AddString(item->GetName());
		m_lstSelected.SetItemData(index, (DWORD)item);

		m_lstAvailable.DeleteString(aryListBoxSel.GetAt(i));
	}
}

void CMultiSelectDlg::OnBnClickedButtonRemove()
{
	int nCount = m_lstSelected.GetSelCount();
	if (nCount < 1)
		return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount, nCount);
	m_lstSelected.GetSelItems(nCount, aryListBoxSel.GetData());

	// Removed the item from Selected listbox and put it back into Available listbox
	for (int i=nCount-1; i>-1; i--)
	{
		CSelItem *item = (CSelItem *)m_lstSelected.GetItemData(aryListBoxSel.GetAt(i));
		item->SetSelected(FALSE);
		
		int index = m_lstAvailable.AddString(item->GetName());
		m_lstAvailable.SetItemData(index, (DWORD)item);

		m_lstSelected.DeleteString(aryListBoxSel.GetAt(i));
	}
}

void CMultiSelectDlg::OnBnClickedOk()
{
	OnOK();

	// Add the item to the result list
	for (int i=0; i<m_lstSelected.GetCount() ; i++)
	{
		CSelItem *item = (CSelItem *)m_lstSelected.GetItemData(i);

		POSITION pos = m_allItemList.Find(item);
		m_allItemList.RemoveAt(pos);

		m_resultItemList.AddTail(item);	
	}

	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
	frame->getNavigator().setDoc(NULL);	
}

void CMultiSelectDlg::OnBnClickedCancel()
{
	OnCancel();
}



/////////////////////////////////////////////////////////////////////////////
// CColorButton
/////////////////////////////////////////////////////////////////////////////
CColorButton::CColorButton()
{
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
   //{{AFX_MSG_MAP(CColorButton)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers
void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   HBRUSH hBrush, orgBrush;
   unsigned long color;

   if (lpDrawItemStruct->itemAction != ODA_DRAWENTIRE && lpDrawItemStruct->itemAction != ODA_SELECT)
      return;

   color = tempColor;

   if (lpDrawItemStruct->itemAction == ODA_DRAWENTIRE) // draw button color
   {
		if (IsWindowEnabled())
		{
			hBrush = CreateSolidBrush(color);
			orgBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
			Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, 
					lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, 
					lpDrawItemStruct->rcItem.bottom);
			DeleteObject(SelectObject(lpDrawItemStruct->hDC, orgBrush));
		}
		else
		{	
			hBrush = CreateSolidBrush(RGB(100, 100, 100));
			orgBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
			Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, 
					lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, 
					lpDrawItemStruct->rcItem.bottom);
			DeleteObject(SelectObject(lpDrawItemStruct->hDC, orgBrush));

			hBrush = CreateHatchBrush(HS_BDIAGONAL, color);
			orgBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
			Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, 
					lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, 
					lpDrawItemStruct->rcItem.bottom);
			DeleteObject(SelectObject(lpDrawItemStruct->hDC, orgBrush));
		}
	}
   else if (lpDrawItemStruct->itemAction == ODA_SELECT) // button clicked
   {
      if (lpDrawItemStruct->itemState & ODS_SELECTED) // button state changes twice(down then up) - only answer once
      {
         CPersistantColorDialog dialog(color);
         dialog.DoModal();
         color = dialog.GetColor();
         Invalidate();
         tempColor = color;
      }
   }
}

