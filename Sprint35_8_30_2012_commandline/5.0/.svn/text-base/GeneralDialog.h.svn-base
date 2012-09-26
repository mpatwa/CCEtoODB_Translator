// $Header: /CAMCAD/5.0/GeneralDialog.h 3     3/09/07 2:35p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#if ! defined (__GeneralDialog_h__)
#define __GeneralDialog_h__

#pragma once

#include "Resource.h"
#include "TypedContainer.h"
#include "DcaFormatStdioFile.h"

/////////////////////////////////////////////////////////////////////////////
// InputDlg dialog
class InputDlg : public CDialog
{
// Construction
public:
   InputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(InputDlg)
   enum { IDD = IDD_INPUT };
   CString  m_input;
   CString  m_prompt;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(InputDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(InputDlg)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// AccessCodeError dialog
class AccessCodeError : public CDialog
{
// Construction
public:
   AccessCodeError(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(AccessCodeError)
   enum { IDD = IDD_ACCESS_CODE_ERROR };
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA
   CString caption;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AccessCodeError)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AccessCodeError)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};



////////////////////////////////////////////////////////////////////////////////
// CSelItem Class
////////////////////////////////////////////////////////////////////////////////
class CSelItem
{
protected:
	CString m_name;
	BOOL m_selected;

public:
	CSelItem(CString name, BOOL selected = FALSE);
	~CSelItem(){};

	CString GetName()const;				
	BOOL GetSelect()const;				
	void SetSelected(BOOL selected);	
};
typedef CTypedPtrListContainer<CSelItem*> CSelItemList;


////////////////////////////////////////////////////////////////////////////////
// CSelItem Class
////////////////////////////////////////////////////////////////////////////////
class CVoidPtrSelItem : public CSelItem
{
public:
	CVoidPtrSelItem(CString name = "", void* voidPtr = NULL, BOOL selected = FALSE):CSelItem(name, selected)
	{
		m_pVoidPtr = voidPtr;
	}
	~CVoidPtrSelItem()
	{
		m_pVoidPtr = NULL;
	}

	void* GetVoidPtrItem() { return m_pVoidPtr; }

private:
	void* m_pVoidPtr;
};
typedef CTypedPtrListContainer<CVoidPtrSelItem*> CVoidPtrSelItemList;


/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CSelectDlg : public CDialog
{
// Construction
public:
   CSelectDlg(CString prompt, bool allowMultipleSelect, bool sortList, CWnd* pParent = NULL);  
	virtual ~CSelectDlg();
   enum { IDD = IDD_SELECT };

private:
	CSelItemList m_allItemList;
	CSelItemList m_selItemList;
	CListBox* m_listBox;
   CString m_sPrompt;
	bool m_bAllowMultipleSelect;
	bool m_bSortList;

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()


public:
	void SetPromp(CString prompt)								{ m_sPrompt = prompt;								};

	// Function to access the selected list
	void AddItem(CSelItem *item);
	POSITION GetSelItemHeadPosition() const				{ return m_selItemList.GetHeadPosition();		};
	POSITION GetSelItemTailPosition() const				{ return m_selItemList.GetTailPosition();		};
	CSelItem *GetSelItemHead() const					{ return m_selItemList.GetHead();				};
	CSelItem *GetSelItemTail() const					{ return m_selItemList.GetTail();				};
	CSelItem *GetSelItemNext(POSITION &pos) const	{ return m_selItemList.GetNext(pos);			};	
	CSelItem *GetSelItemPrev(POSITION &pos) const	{ return m_selItemList.GetPrev(pos);			};
	CSelItem *GetSelItemAt(POSITION pos) const		{ return m_selItemList.GetAt(pos);				};
	int GetSelItemCount()const									{ return m_selItemList.GetCount();				};

protected:
   virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
public:
	afx_msg void OnBnClickedOk();
};



////////////////////////////////////////////////////////////////////////////////
// CMultiSelectDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CMultiSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CMultiSelectDlg)

public:
	CMultiSelectDlg(CString caption = "", CWnd* pParent = NULL);   // standard constructor
	virtual ~CMultiSelectDlg();
	enum { IDD = IDD_SELECT_MULTIPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CSelItemList m_allItemList;
	CSelItemList m_resultItemList;
	CListBox m_lstAvailable;
	CListBox m_lstSelected;
	CString m_caption;

public:
	virtual BOOL OnInitDialog();
	void SetDialogCaption(CString caption)				{m_caption = caption;};

	void AddItem(CSelItem *item);
	POSITION GetItemHeadPosition() const				{return m_resultItemList.GetHeadPosition();};
	POSITION GetItemTailPosition() const				{return m_resultItemList.GetTailPosition();};
	CSelItem *GetItemHead() const					{return m_resultItemList.GetHead();};
	CSelItem *GetItemTail() const					{return m_resultItemList.GetTail();};
	CSelItem *GetItemNext(POSITION &pos) const	{return m_resultItemList.GetNext(pos);};	
	CSelItem *GetItemPrev(POSITION &pos) const	{return m_resultItemList.GetPrev(pos);};
	CSelItem *GetItemAt(POSITION pos) const		{return m_resultItemList.GetAt(pos);};
	int GetItemCount()const									{return m_resultItemList.GetCount();};

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};


/////////////////////////////////////////////////////////////////////////////
// CColorButton window
////////////////////////////////////////////////////////////////////////////////////////////
class CColorButton : public CButton
{
// Construction
public:
   CColorButton();

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CLayerColorButton)
   public:
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
   //}}AFX_VIRTUAL
   COLORREF tempColor;

// Implementation
public:
   virtual ~CColorButton();

   // Generated message map functions
protected:
   //{{AFX_MSG(CLayerColorButton)
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif

