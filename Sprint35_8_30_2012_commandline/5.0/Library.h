// $Header: /CAMCAD/4.3/Library.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_LIBRARY_H__29B456D0_2F12_11D3_8649_004005408E44__INCLUDED_)
#define AFX_LIBRARY_H__29B456D0_2F12_11D3_8649_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// SelectDocument dialog
class SelectDocument : public CDialog
{
// Construction
public:
   SelectDocument(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(SelectDocument)
   enum { IDD = IDD_SELECT_DOCUMENT };
   CListBox m_list;
   //}}AFX_DATA  
   CCEtoODBDoc *doc, *libDoc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectDocument)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SelectDocument)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// SelectGeometries dialog
class SelectGeometries : public CDialog
{
// Construction
public:
   SelectGeometries(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(SelectGeometries)
   enum { IDD = IDD_SELECT_GEOMETRIES };
   CListBox m_typesLB;
   CListBox m_list;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CPtrList geomsList;
   int count;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectGeometries)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SelectGeometries)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSelectAll();
   afx_msg void OnUnselectAll();
   afx_msg void OnSelectByType();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// ReplaceGeometry dialog
class ReplaceGeometry : public CDialog
{
// Construction
public:
   ReplaceGeometry(CWnd* pParent = NULL);   // standard constructor
   ~ReplaceGeometry();

// Dialog Data
   //{{AFX_DATA(ReplaceGeometry)
   enum { IDD = IDD_REPLACE_GEOMETRY };
   CTreeCtrl   m_with;
   CTreeCtrl   m_replace;
   BOOL  m_allGeometries;
   int      m_attribs;
   int      m_collisions;
   CString  m_x;
   CString  m_y;
   CString  m_angle;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   BlockStruct *replace, *with;
   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ReplaceGeometry)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillTree();
   void ReplaceGeom();

   // Generated message map functions
   //{{AFX_MSG(ReplaceGeometry)
   virtual BOOL OnInitDialog();
   afx_msg void OnReplaceGeom();
   afx_msg void OnAllGeometries();
   afx_msg void OnAutoReplace();
   //}}AFX_MSG
   void InsertBlock(BlockStruct *block, int filenum, HTREEITEM replaceRoot, HTREEITEM withRoot);
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// MapKeywords dialog
class MapKeywords : public CDialog
{
// Construction
public:
   MapKeywords(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MapKeywords)
   enum { IDD = IDD_MAP_KEYWORDS };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MapKeywords)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MapKeywords)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// MapKeywordEdit dialog
class MapKeywordEdit : public CDialog
{
// Construction
public:
   MapKeywordEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MapKeywordEdit)
   enum { IDD = IDD_MAP_KEYWORD_EDIT };
   CString  m_orig;
   CString  m_new;
   //}}AFX_DATA
   CString newKW;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MapKeywordEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MapKeywordEdit)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// MapLayersDlg dialog
class MapLayersDlg : public CDialog
{
// Construction
public:
   MapLayersDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MapLayersDlg)
   enum { IDD = IDD_LIBRARY_LAYERMAP };
   CListCtrl   m_list;
   //}}AFX_DATA
   CCEtoODBDoc *libDoc, *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MapLayersDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillList();

   // Generated message map functions
   //{{AFX_MSG(MapLayersDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnOk();
   afx_msg void OnDuplicateAll();
   afx_msg void OnDuplicateUnmapped();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// MapLayerEdit dialog
class MapLayerEdit : public CDialog
{
// Construction
public:
   MapLayerEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MapLayerEdit)
   enum { IDD = IDD_LIBRARY_LAYERMAP_EDIT };
   CListBox m_sameLB;
   CListBox m_differentLB;
   int      m_choice;
   CString  m_newLayer;
   CString  m_origLayer;
   CString  m_origType;
   //}}AFX_DATA
   LayerStruct *origLayer, *newLayer;
   CCEtoODBDoc *doc;
   int layertype;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MapLayerEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MapLayerEdit)
   virtual BOOL OnInitDialog();
   afx_msg void OnDifferent();
   afx_msg void OnNew();
   afx_msg void OnSame();
   virtual void OnOK();
   afx_msg void OnDontMap();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIBRARY_H__29B456D0_2F12_11D3_8649_004005408E44__INCLUDED_)
