// $Header: /CAMCAD/4.5/ResizingPropertySheet.h 7     2/22/07 2:51p Rick Faltersack $

#if !defined(__ResizingPropertySheet_H__)
#define __ResizingPropertySheet_H__

#pragma once

#include "ResizingDialog.h"

CPoint calcRectCenter(CRect rect);

//_____________________________________________________________________________
class CResizingPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CResizingPropertySheet)

protected:
   bool   m_bNeedInit;
   CRect  m_rCrt;
   int    m_minWidth;
   int    m_minHeight;
   CString m_caption;
   bool m_allowResize;
   bool m_rememberState;
   bool m_drawGripper;
   CDialogGripper m_gripper;
   CSize m_previousClientSize;

public:
   static int CALLBACK XmnPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam);

public:
	CResizingPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CResizingPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CResizingPropertySheet();

   bool getAllowResize() const { return m_allowResize; }
   void setAllowResize(bool allowResize);

   INT_PTR DoModal(void);
   virtual void saveWindowState();
   virtual void restoreWindowState();
   virtual CString GetDialogProfileEntry();

protected:
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMove(int x,int y);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
   afx_msg void OnDestroy();
};

#if _MSC_VER >= 1400
typedef LRESULT NCHITTEST_UNIT; 
#else
typedef UINT NCHITTEST_UNIT;
#endif

//_____________________________________________________________________________
class CResizingPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CResizingPropertyPage)

protected:
   CResizingDialogFields m_fields;
   CSize m_originalWindowSize;
   bool m_initializedWindowDataFlag;
   bool m_stateChanged;
   CSize m_originalClientSize;
   CSize m_previousClientSize;

// Construction
public:
   CResizingPropertyPage(UINT nIDTemplate);

   CResizingDialogField& addFieldControl(int controlId,
      DialogFieldOperationTag anchorValue=anchorLeft,DialogFieldOperationTag growValue=growStatic);
   CResizingDialogField& addFieldControl(int controlId,
      DialogFieldOperationTag anchorValue,CResizingDialogField* anchorRelatedField,
      DialogFieldOperationTag growValue=growStatic);

   bool GetRememberState() { return  m_rememberState;}
   bool GetInitializedWindowDataFlag() { return  m_initializedWindowDataFlag;}
   void SetRememberState(bool rememberSize) { m_rememberState = rememberSize;}

   CSize getOriginalWindowSize() { return m_originalWindowSize; }
   void setOriginalWindowSize(const CSize& size) { m_originalWindowSize = size; }

   CSize getOriginalClientSize() { return m_originalClientSize; }
   void setOriginalClientSize(const CSize& size) { m_originalClientSize = size; }

   CSize getPreviousClientSize() { return m_previousClientSize; }
   void setPreviousClientSize(const CSize& size) { m_previousClientSize = size; }

   void initDialog();
   CSize getMinMaxSize();
   void setMinMaxSize(CSize& size);
   void adjustSizeForToolBar(int toolbarHeight);
   virtual CString GetDialogProfileEntry();

   virtual BOOL OnSetActive();

// Dialog Data
protected:
   // Set rememberState to FALSE if you don't want the dialog box 
   // to remember its size between sessions.
   bool m_allowResize;
   bool m_rememberState;
   // Set m_bDrawGipper if you don't want a Resizing Gripper

private:
   int m_minWidth;
   int m_minHeight;
   UINT m_nIDTemplate;

protected:
   // Generated message map functions
   //{{AFX_MSG(CResizingPropertyPage)
   virtual BOOL OnInitDialog();
   virtual afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMove(int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
   afx_msg void OnDestroy();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg NCHITTEST_UNIT OnNcHitTest(CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif

