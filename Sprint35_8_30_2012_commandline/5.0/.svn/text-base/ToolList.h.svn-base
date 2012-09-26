// $Header: /CAMCAD/4.6/ToolList.h 12    5/25/07 4:36p Rick Faltersack $

#include "ccdoc.h"
#include "afxwin.h"
#include "ResizingDialog.h"

/////////////////////////////////////////////////////////////////////////////
// ListTools dialog

class ListTools : public CResizingDialog
{
// Construction
public:
   ListTools(CWnd* pParent = NULL);   // standard constructor
   ~ListTools();

// Dialog Data
   //{{AFX_DATA(ListTools)
   enum { IDD = IDD_LIST_TOOL };
   CListCtrl   m_list;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   int m_decimals;

   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ListTools)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("ListToolsDialog"); }

private:
   void LoadToolIconImages();
   int GetToolIconImageIndex(BlockStruct *toolBlk);

protected:
   void FillListCtrl();
   void PrintHeader(CDC *dc, int hRes, int vRes, int space, int page);

   // Generated message map functions
   //{{AFX_MSG(ListTools)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnAdd();
   afx_msg void OnSettcodes();
   afx_msg void OnPrint();
   afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditTool dialog
class EditTool : public CDialog
{
// Construction
public:
   EditTool(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditTool)
   enum { IDD = IDD_EDIT_TOOL };
   CComboBox   m_blockCB;
   BOOL  m_display;
   CString  m_name;
   CString  m_size;
   UINT  m_tcode;
   //}}AFX_DATA
   BOOL m_plated;

   CCEtoODBDoc *doc;
   int displayBlockNum;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditTool)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditTool)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
