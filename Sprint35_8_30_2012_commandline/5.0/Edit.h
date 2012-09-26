// $Header: /CAMCAD/4.6/Edit.h 27    1/17/07 12:30a Kurt Van Ness $

#if ! defined (__Edit_h__)
#define __Edit_h__

#pragma once

#include "StdAfx.h"
#include "select.h"
#include "ccdoc.h"
#include "ResizingDialog.h"

// Previous includes cause this to be defined but out Ultimate Grid is not built with OLE enabled
// so having this symbol defined before this include causes three undefined references at link time.
// It is not previous includes right here necessarily, it can be previous includes where this .h gets included.
#undef __AFXOLE_H__
#include "UGCtrl.h"

void StopEditing();

//_____________________________________________________________________________
class CHorizontalPositionComboBox : public CComboBox
{
private:
   bool m_initialized;

public:
   CHorizontalPositionComboBox();

   HorizontalPositionTag getValue();
   void setValue(HorizontalPositionTag value);

private:
   bool initialized();
};

//_____________________________________________________________________________
class CVerticalPositionComboBox : public CComboBox
{
private:
   bool m_initialized;

public:
   CVerticalPositionComboBox();

   VerticalPositionTag getValue();
   void setValue(VerticalPositionTag value);

private:
   bool initialized();
};

/////////////////////////////////////////////////////////////////////////////
// CEditDialog dialog
class CEditDialog : public CResizingDialog
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   CEditDialog(UINT nIDTemplate,CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);
   virtual void OnCancel();
   virtual CString GetDialogProfileEntry() { return CString("EditDialog"); }

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   bool isEditable(SelectStruct& selectStruct);
};

/////////////////////////////////////////////////////////////////////////////
// EditLine dialog
class EditLine : public CEditDialog
{
// Construction
public:
   EditLine(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditLine)
   enum { IDD = IDD_EDIT_LINE };
   CComboBox   m_classCB;
   CListCtrl   m_attribLC;
   CComboBox   m_layerCB;
   CComboBox   m_widthCB;
   CString  m_endX;
   CString  m_endY;
   CString  m_startX;
   CString  m_startY;
   BOOL  m_negative;
   int      m_class;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   CPoly *poly;
   CToolTipCtrl tooltip;
   CString hint;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditLine)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditLineDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditLine)
   afx_msg void OnModify();
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   afx_msg void OnAttribs();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnFlags();
   afx_msg void OnPolyline();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditPoly dialog
class EditPoly : public CEditDialog
{
// Construction
public:
   EditPoly(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor
   ~EditPoly();

// Dialog Data
   //{{AFX_DATA(EditPoly)
   enum { IDD = IDD_EDIT_POLYSTRUCT };
   CTreeCtrl   m_tree;
   CComboBox   m_classCB;
   CListCtrl   m_attribLC;
   CComboBox   m_widthCB;
   CComboBox   m_layerCB;
   CComboBox   m_startlayerCB;
   CComboBox   m_endlayerCB;
   CButton m_chkSpanlayer;
   BOOL  m_negative;
   int      m_class;
   BOOL  m_spanlayer;

   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   int decimals;
   BOOL Erase;
   CToolTipCtrl tooltip;
   CString hint;
   CPoly *highlightPoly;
   CPnt *highlightPnt;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditPoly)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditPolyDialog"); }
   void EnableSpanlayer(BOOL hasSpanLayer);
   void OnSetSpanLayerToPoly();

// Implementation
protected:
   void FillTree(POSITION polyPos);
   LRESULT OnDrawPolyItem(WPARAM WParam, LPARAM LParam);

   // Generated message map functions
   //{{AFX_MSG(EditPoly)
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnModify();
   afx_msg void OnAttribs();
   afx_msg void OnEditPolyItem();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnFlags();
   afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnBnClickedSpanlayer();
   afx_msg void OnCbnSelchangeClass();
};

/////////////////////////////////////////////////////////////////////////////
// EditArc dialog
class EditArc : public CEditDialog
{
// Construction
public:
   EditArc(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditArc)
   enum { IDD = IDD_EDIT_ARC };
   CComboBox   m_classCB;
   CListCtrl   m_attribLC;
   CComboBox   m_widthCB;
   CComboBox   m_layerCB;
   CComboBox   m_startlayerCB;
   CComboBox   m_endlayerCB;
   CString  m_centerX;
   CString  m_centerY;
   CString  m_delta;
   CString  m_radius;
   CString  m_start;
   BOOL  m_negative;
   int      m_class;

   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   CPoly *poly;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditArc)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditArcDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditArc)
   virtual BOOL OnInitDialog();
   afx_msg void OnModify();
   virtual void OnCancel();
   afx_msg void OnAttribs();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnEditAsPolyline();
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditInsert dialog
class EditInsert : public CEditDialog
{
//private:
//   CCEtoODBDoc& m_camCadDoc;

// Construction
public:
   EditInsert(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor
   ~EditInsert();

// Dialog Data
   //{{AFX_DATA(EditInsert)
   enum { IDD = IDD_EDIT_INSERT };
   CComboBox   m_typeCB;
   CListCtrl   m_attribLC;
   CComboBox   m_layerCB;
   CComboBox   m_block;
   CString  m_angle;
   CString  m_x;
   CString  m_y;
   CString m_documentX;
   CString m_documentY;
   CString m_documentAngle;
   CString  m_shape;
   CString  m_sizea;
   CString  m_sizeb;
   CString  m_refname;
   int      m_type;
   int      m_top_bottom;
   CString  m_scale;
   BOOL  m_negative;
   BOOL  m_mirrorLayers;
   BOOL  m_mirrorFlip;
   //}}AFX_DATA

   SelectStruct *m_selectStruct;
   //CCEtoODBDoc *doc;
   CToolTipCtrl m_tooltip;
   CString hint;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditInsert)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditInsertDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditInsert)
   virtual BOOL OnInitDialog();
   afx_msg void OnModify();
   virtual void OnCancel();
   afx_msg void OnAttribs();
   afx_msg void OnTop();
   afx_msg void OnBottom();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditCircle dialog
class EditCircle : public CEditDialog
{
// Construction
public:
   EditCircle(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditCircle)
   enum { IDD = IDD_EDIT_CIRCLE };
   CComboBox   m_classCB;
   CListCtrl   m_attribLC;
   CComboBox   m_widthCB;
   CComboBox   m_layerCB;
   CString  m_radius;
   CString  m_x;
   CString  m_y;
   BOOL  m_negative;
   BOOL  m_filled;
   int      m_class;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   CPoly *poly;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditCircle)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditCircleDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditCircle)
   virtual BOOL OnInitDialog();
   afx_msg void OnModify();
   virtual void OnCancel();
   afx_msg void OnAttribs();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnEditAsPolyline();
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditText dialog
class EditText : public CEditDialog
{
// Construction
public:
   EditText(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor
   ~EditText();

// Dialog Data
   //{{AFX_DATA(EditText)
   enum { IDD = IDD_EDIT_TEXT };
   CEdit m_textCtrl;
   CComboBox   m_classCB;
   CListCtrl   m_attribLC;
   CComboBox   m_fontCB;
   CComboBox   m_widthCB;
   CComboBox   m_layerCB;
   CHorizontalPositionComboBox m_horizontalAnchorPosition;
   CVerticalPositionComboBox m_verticalAnchorPosition;
   CString  m_height;
   CString  m_angle;
   BOOL  m_mirror;
   BOOL  m_proportional;
   CString  m_text;
   CString  m_x;
   CString  m_y;
   int      m_oblique;
   BOOL  m_negative;
   int      m_class;
   CString  m_width;
   BOOL  m_neverMirror;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditText)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditTextDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditText)
   virtual BOOL OnInitDialog();
   afx_msg void OnModify();
   virtual void OnCancel();
   afx_msg void OnAttribs();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditNothing dialog
class EditNothing : public CEditDialog
{
// Construction
public:
   EditNothing(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditNothing)
   enum { IDD = IDD_EDIT_NOTHING };
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditNothing)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditNothingDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditNothing)
   virtual void OnCancel();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditApInsert dialog
class EditApInsert : public CEditDialog
{
// Construction
public:
   EditApInsert(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor
   ~EditApInsert();

// Dialog Data
   //{{AFX_DATA(EditApInsert)
   enum { IDD = IDD_EDIT_AP_INSERT };
   CComboBox   m_typeCB;
   CListCtrl   m_attribLC;
   CComboBox   m_shapeCB;
   CComboBox   m_layerCB;
   CComboBox   m_block;
   CString  m_x;
   CString  m_y;
   CString m_documentX;
   CString m_documentY;
   CString  m_sizea;
   CString  m_sizeb;
   CString  m_angle;
   CString  m_rot;
   int      m_type;
   BOOL  m_negative;
   CString  m_refname;

   int      m_top_bottom;
   BOOL  m_mirrorLayers;
   BOOL  m_mirrorFlip;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   BlockStruct *block;
   int shape;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditApInsert)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditApInsertDialog"); }

// Implementation
protected:
   void FillApData();
   void HideSizes();

   // Generated message map functions
   //{{AFX_MSG(EditApInsert)
   virtual BOOL OnInitDialog();
   afx_msg void OnModify();
   afx_msg void OnTop();
   afx_msg void OnBottom();
   virtual void OnCancel();
   afx_msg void OnSelchangeAp();
   afx_msg void OnSelchangeShape();
   afx_msg void OnAttribs();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditPolyItem dialog
class EditPolyItem : public CDialog
{
// Construction
public:
   EditPolyItem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditPolyItem)
   enum { IDD = IDD_EDIT_POLY_ITEM };
   CComboBox   m_widthCB;
   BOOL  m_closed;
   BOOL  m_filled;
   BOOL  m_void;
   BOOL  m_hidden;
   BOOL  m_thermal;
   BOOL  m_boundary;
   BOOL  m_hatch;
   //}}AFX_DATA
   short widthIndex;
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditPolyItem)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditPolyItem)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditPntItem dialog
class EditPntItem : public CDialog
{
// Construction
public:
   EditPntItem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditPntItem)
   enum { IDD = IDD_EDIT_PNT_ITEM };
   CString  m_chord;
   CString  m_num;
   CString  m_rad;
   CString  m_x;
   CString  m_y;
   int      m_type;
   CString  m_da;
   //}}AFX_DATA
   CPnt *next, *pnt;
   int decimals;
   CCEtoODBDoc *doc;
   SelectStruct *s;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditPntItem)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
public:
   void GetValues();
   void SetValues();

protected:
   // Generated message map functions
   //{{AFX_MSG(EditPntItem)
   afx_msg void OnApply();
   virtual void OnOK();
   afx_msg void OnRadio1();
   afx_msg void OnRadio2();
   afx_msg void OnRadio3();
   afx_msg void OnRadio0();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////
// EditToolInsert dialog
class EditToolInsert : public CEditDialog
{
// Construction
public:
   EditToolInsert(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor
   ~EditToolInsert();

// Dialog Data
   //{{AFX_DATA(EditToolInsert)
   enum { IDD = IDD_EDIT_TOOL_INSERT };
   CComboBox   m_typeCB;
   CComboBox   m_layerCB;
   CComboBox   m_block;
   CListCtrl   m_attribLC;
   CString  m_x;
   CString  m_y;
   BOOL m_plated;
   CString m_toolsize;
   CString m_documentX;
   CString m_documentY;
   int      m_type;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   BlockStruct *block;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditToolInsert)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditToolInsertDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditToolInsert)
   afx_msg void OnAttribs();
   afx_msg void OnModify();
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditPoint dialog
class EditPoint : public CEditDialog
{
// Construction
public:
   EditPoint(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditPoint)
   enum { IDD = IDD_EDIT_POINT };
   CComboBox   m_layerCB;
   CListCtrl   m_attribLC;
   CComboBox   m_classCB;
   CString  m_x;
   CString  m_y;
   BOOL  m_negative;
   int      m_class;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditPoint)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditPointDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditPoint)
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   afx_msg void OnModify();
   afx_msg void OnAttribs();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditBlob dialog
class EditBlob : public CEditDialog
{
// Construction
public:
   EditBlob(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditBlob)
   enum { IDD = IDD_EDIT_BLOB };
   CListCtrl   m_attribLC;
   CString  m_x;
   CString  m_y;
   CString  m_width;
   CString  m_height;
   //}}AFX_DATA
   SelectStruct *s;
   //CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditBlob)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("EditBlobDialog"); }

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditBlob)
   afx_msg void OnModify();
   afx_msg void OnAttribs();
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnFlags();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
#pragma once


// CFilterByInsertType dialog

class CFilterByInsertType : public CDialog
{
	DECLARE_DYNAMIC(CFilterByInsertType)

public:
	CFilterByInsertType(CWnd* pParent = NULL);   // standard constructor
	CFilterByInsertType(CCEtoODBDoc *doc, CWnd* pParent = NULL);   // overloaded constructor
	virtual ~CFilterByInsertType();

// Dialog Data
	enum { IDD = IDD_FilterByInsertType_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void DeleteSelectionInsertType(int insertType);
	CCEtoODBDoc *doc;

	DECLARE_MESSAGE_MAP()
public:
	CListBox CInsertTypeList;
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnInitDialog();
	//afx_msg CUIntArray insertTypes;
	afx_msg void OnBnClickedButton1();
};


////////////////////////////////////////////////////////////////////////////////////////
// CFilterByGraphicClass dialog

class CFilterByGraphicClass : public CDialog
{
	DECLARE_DYNAMIC(CFilterByGraphicClass)

public:
	CFilterByGraphicClass(CWnd* pParent = NULL);   // standard constructor
	CFilterByGraphicClass(CCEtoODBDoc *doc, CWnd* pParent = NULL);   // overloaded constructor
	virtual ~CFilterByGraphicClass();

// Dialog Data
	enum { IDD = IDD_FilterByGraphicClass_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void DeleteSelectionGraphicClass(int GraphicClass);
	CCEtoODBDoc *doc;

	DECLARE_MESSAGE_MAP()
public:
	CListBox CGraphicClassList;
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnInitDialog();
	afx_msg void DeleteSelectionGrClass(int grClass);
	afx_msg void OnBnClickedButton1();
};

////////////////////////////////////////////////////////////////////////////////////////
// 

class CTableGrid : public CUGCtrl
{
public:
   virtual void OnSetup();
   virtual void OnTabSelected(int tabId);
};

//--------------------------------------------------------------------------------------

class CTableEditDialog : public CResizingDialog
{
private:
   CTableGrid m_tableGrid;
   bool m_valid;
   CListBox m_tableListOwners; // List of items that have table lists

   afx_msg void OnLbnSelChangeTableList();

   void ResetGrid();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
   enum { IDD = IDD_TABLES };

   CTableEditDialog();

   virtual BOOL OnInitDialog();


   void FillGrid();
   void FillTableListSelector();

};

//
////////////////////////////////////////////////////////////////////////////////////////


#endif
