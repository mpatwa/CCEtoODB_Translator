// $Header: /CAMCAD/4.5/AtrbList.h 21    4/28/06 4:56p Lynn Phung $

#if ! defined (__AtrbList_h__)
#define __AtrbList_h__

#pragma once

#include "ccdoc.h"
#include "attrib.h"
#include "afxwin.h"
#include "Attribute.h"
#include "ResizingDialog.h"
#include "Edit.h"

/////////////////////////////////////////////////////////////////////////////
// ListAttribs dialog
class ListAttribs : public CResizingDialog
{
// Construction
public:
   ListAttribs(CWnd* pParent = NULL);   // standard constructor
   ~ListAttribs(); 

// Dialog Data
   //{{AFX_DATA(ListAttribs)
   enum { IDD = IDD_LIST_ATTRIBS };
   CTreeCtrl   m_tree;
   BOOL  m_showApertures;
   BOOL  m_showFreepads;
   BOOL  m_showTools;
   BOOL  m_showVias;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ListAttribs)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("ListAttribsDialog"); }

// Implementation
protected:
   void FillTree();
   void InsertBlock(BlockStruct *block, HTREEITEM root, BOOL IsFile);
   // Generated message map functions
   //{{AFX_MSG(ListAttribs)
   afx_msg void OnEdit();
   virtual BOOL OnInitDialog();
   afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnShowApertures();
   afx_msg void OnShowFreepads();
   afx_msg void OnShowTools();
   afx_msg void OnShowVias();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditAttribs dialog
class EditAttribs : public CResizingDialog
{
private:
   CAttributeMapWrap* m_map;
   static int CALLBACK sortByNameAndInstanceNumber(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);
   CTypedPtrListContainer<CNamedAttributeInstance*> m_namedAttributeInstanceList;

// Construction
public:
   EditAttribs(CWnd* pParent = NULL);   // standard constructor
   ~EditAttribs();
   void setMap(CAttributes*& map);

// constants
   static const int m_keywordColumnIndex     = 0;
   static const int m_valueColumnIndex       = 1;
   static const int m_instanceColumnIndex    = 2;
   static const int m_xOffsetColumnIndex     = 3;
   static const int m_yOffsetColumnIndex     = 4;
   static const int m_angleColumnIndex       = 5;
   static const int m_heightColumnIndex      = 6;
   static const int m_layerColumnIndex       = 7;
   static const int m_neverMirrorColumnIndex = 8;
   static const int m_mirrorFlipColumnIndex  = 9;
   static const int m_maxColumnIndex         = 9;

// Dialog Data
   //{{AFX_DATA(EditAttribs)
   enum { IDD = IDD_EDIT_ATTRIBUTES };
   CListCtrl   m_list;
   CString  m_item;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   //CAttributes** map;
   CImageList *m_imageList;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditAttribs)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditAttributesDialog"); }

// Implementation
protected:
   void FillListCtrl();
   CNamedAttributeInstance& getListControlAttributeInstance(int index);

   // Generated message map functions
   //{{AFX_MSG(EditAttribs)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnNew();
   afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDelete();
   afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedDuplicate();
};

/////////////////////////////////////////////////////////////////////////////
// EditAttrib dialog
class EditAttrib : public CDialog
{
// Construction
public:
   EditAttrib(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditAttrib)
   enum { IDD = IDD_EDIT_ATTRIB };
   CListBox m_groupsLB;
   CComboBox   m_penCB;
   CComboBox   m_layerCB;
   CComboBox   m_keywordCB;
   CHorizontalPositionComboBox m_horizontalAnchorPosition;
   CVerticalPositionComboBox m_verticalAnchorPosition;
   CString  m_value;
   CString  m_keyword;
   CString  m_angle;
   CString  m_height;
   BOOL  m_visible;
   CString  m_x;
   CString  m_y;
   BOOL  m_neverMirror;
   BOOL  m_mirrorFlip;
   BOOL  m_proportional;
   CString  m_width;
   CString  m_valuetype;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CString keywordName;
   int layer;
   short penWidthIndex;
   BOOL groups[AttribGroupTagMax+1];

private:
   HorizontalPositionTag m_horizontalPosition;
   VerticalPositionTag m_verticalPosition;

public:
   HorizontalPositionTag getHorizontalPosition();
   void setHorizontalPosition(HorizontalPositionTag horizontalPosition);
   VerticalPositionTag getVerticalPosition();
   void setVerticalPosition(VerticalPositionTag verticalPosition);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditAttrib)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillKeywords();

   // Generated message map functions
   //{{AFX_MSG(EditAttrib)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSelchangeKeyword();
   afx_msg void OnSelchangeGroupLb();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnCbnSelChangeHorizontalAnchorPosition();
   afx_msg void OnCbnSelChangeVerticalAnchorPosition();
};

/////////////////////////////////////////////////////////////////////////////
// AttribVisDlg dialog

class AttribVisDlg : public CDialog
{
   DECLARE_DYNAMIC(AttribVisDlg)

public:
   AttribVisDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~AttribVisDlg();

// Dialog Data
   enum { IDD = IDD_VISIBLE_ATTRIBS };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()
public:
   CComboBox m_keywordCB;
   CListBox m_groupsLB;
   BOOL m_visible;
   BOOL m_neverMirror;
   BOOL m_mirrorFlip;
   BOOL m_proportional;
   CString m_x;
   CString m_y;
   CString m_height;
   CString m_width;
   CString m_angle;
   CComboBox m_penCB;
   CComboBox m_layerCB;

   CCEtoODBDoc *doc;
   BOOL groups[AttribGroupTagMax+1];

   void FillKeywords();
   afx_msg void OnSelchangeGroupLb();
   afx_msg void OnOK();
   afx_msg BOOL OnInitDialog();
   afx_msg void OnHideAll();
};

#endif
