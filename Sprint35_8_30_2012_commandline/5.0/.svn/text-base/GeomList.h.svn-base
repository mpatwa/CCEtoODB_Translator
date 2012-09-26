// $Header: /CAMCAD/4.3/GeomList.h 8     8/12/03 9:06p Kurt Van Ness $

#include "ccdoc.h"
#include "ResizingDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CGeometryListDialog dialog
class CGeometryListDialog : public CResizingDialog
{
// Construction
public:
   CGeometryListDialog(CCEtoODBDoc *Doc, CWnd* pParent=NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CGeometryListDialog)
   enum { IDD = IDD_LIST_GEOMETRY };
   CComboBox   m_typeCB;
   CButton  m_IsTool;
   CListCtrl   m_geomLC;
   CButton  m_IsAperture;
   CListBox m_FilesLB;
   BOOL  m_ShowApertures;
   BOOL  m_ShowBlocks;
   BOOL  m_ShowFiles;
   BOOL  m_ShowWidths;
   BOOL  m_ShowTools;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   int itemnum;
   int decimals;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGeometryListDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("GeometryListDialog"); }

// Implementation
protected:
   void AddGeometry(BlockStruct *ptr,const char* type,const char* apSize,const char* toolSize);

   // Generated message map functions
   //{{AFX_MSG(CGeometryListDialog)
   afx_msg void OnIsAperture();
   afx_msg void OnIsDrill();
   afx_msg void FillGeometryLC();
   virtual BOOL OnInitDialog();
   afx_msg void OnItemchangedGeometries(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDblclkGeometries(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnIsTool();
   afx_msg void OnCopy();
   afx_msg void OnSelchangeTypeCb();
   afx_msg void OnLoadApertureAssign();
   afx_msg void OnSaveApertureAssign();
   afx_msg void OnLoadToolAssign();
   afx_msg void OnSaveToolAssign();
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// ChangeInsertTypes dialog
class ChangeInsertTypes : public CDialog
{
// Construction
public:
   ChangeInsertTypes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ChangeInsertTypes)
   enum { IDD = IDD_CHANGE_INSERTTYPE };
   CListBox m_list;
   CString  m_blockName;
   //}}AFX_DATA
   int inserttype;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ChangeInsertTypes)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ChangeInsertTypes)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
