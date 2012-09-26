// $Header: /CAMCAD/5.0/Attrib.h 61    5/22/07 6:31p Rick Faltersack $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__Attrib_h__)
#define __Attrib_h__

#pragma once

#include "DcaAttributes.h"
#include "Resource.h"
#include "Data.h"
#include "ResizingDialog.h"
#include "Point2D.h"
#include "WriteFormat.h"
#include "keyword.h"

class CCEtoODBDoc;

ValueTypeTag intToValueTypeTag(int valueType);
CString valueTypeTagToString(ValueTypeTag valueTypeTag);
void KeywordMerge(CCEtoODBDoc *doc, CAttributes* map, WORD from, WORD to, int method);

// this only applies for VT_STRING type
#define SA_APPEND       1        // attributes gets CAT on the end of the string
#define SA_OVERWRITE    2        // old att gets deleted, new gets added 
#define SA_RETURN       3        // if old exist, new value is not accepted.

//_____________________________________________________________________________
Attrib *get_attvalue(CAttributes* map,WORD keytok);

Attrib *is_attvalue(CCEtoODBDoc *doc, CAttributes* map,
                  const char *keyword, int keywordtyp); // 0 = camcad
                                                        // 1 = input
                                                        // 2 = output
void  RemoveAttrib(WORD keyword, CAttributes** attribMap);

// this converts every attribute value into a string
const char *get_attvalue_string(CCEtoODBDoc *doc, Attrib *a);

/////////////////////////////////////////////////////////////////////////////
// KeywordMap dialog
class KeywordMap : public CResizingDialog
{
// Construction
public:
   KeywordMap(CWnd* pParent = NULL);   // standard constructor
   ~KeywordMap();

// Dialog Data
   //{{AFX_DATA(KeywordMap)
   enum { IDD = IDD_KEYWORD_MAP };
   CListBox m_groupsLB;
   CListCtrl   m_listCtrl;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
// CImageList *m_imageList;
   BOOL groups[AttribGroupTagMax+1];

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(KeywordMap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("KeywordMapDialog"); }

// Implementation
protected:
   void FillListCtrl();

   // Generated message map functions
   //{{AFX_MSG(KeywordMap)
   afx_msg void OnEdit();
   virtual BOOL OnInitDialog();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   afx_msg void OnColumnclickListControl(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnRemove();
   afx_msg void OnDblclkListControl(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnAdd();
   afx_msg void OnMerge();
   afx_msg void OnSelchangeGroupsLb();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// EditKeywordMap dialog
class EditKeywordMap : public CDialog
{
// Construction
public:
   EditKeywordMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(EditKeywordMap)
   enum { IDD = IDD_EDIT_KEYWORD_MAP };
   CString  m_cc;
   CString  m_in;
   CString  m_out;
   int      m_valueType;
   BOOL     m_hidden;
   //}}AFX_DATA
   int   group; // internal keyword
   BOOL  adding; // new keyword
   CCEtoODBDoc *doc;
   int index; // index of the keyword we're working on (-1 for adding)
   int valueType; // valueType (using defines: VT_NONE, VT_STRING, etc.)


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(EditKeywordMap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(EditKeywordMap)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// OverwriteAttrib dialog
class OverwriteAttrib : public CDialog
{
// Construction
public:
   OverwriteAttrib(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OverwriteAttrib)
   enum { IDD = IDD_OVERWRITE_ATTRIB };
   CButton  m_appendBtn;
   CString  m_keyword;
   CString  m_value;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   Attrib *attrib;
   int method;
   BOOL All;
   int valueType;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OverwriteAttrib)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void OnCancel();

   // Generated message map functions
   //{{AFX_MSG(OverwriteAttrib)
   afx_msg void OnOverwrite();
   afx_msg void OnSkip();
   afx_msg void OnAppend();
   virtual BOOL OnInitDialog();
   afx_msg void OnAppendAll();
   afx_msg void OnOverwriteAll();
   afx_msg void OnSkipAll();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// MergeKeywordsDlg dialog
class MergeKeywordsDlg : public CDialog
{
// Construction
public:
   MergeKeywordsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MergeKeywordsDlg)
   enum { IDD = IDD_MERGE_KEYWORDS };
   CListBox m_list;
   int      m_choice;
   //}}AFX_DATA
   BOOL DisableAppend;
   CString fromString;
   WORD from, to;
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MergeKeywordsDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MergeKeywordsDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSelchangeKWList();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// MoveAttribDlg dialog
class MoveAttribDlg : public CDialog
{
// Construction
public:
   MoveAttribDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(MoveAttribDlg)
   enum { IDD = IDD_MOVE_ATTRIB };
   CListBox m_list;
   //}}AFX_DATA
   CAttributes* map;
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MoveAttribDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(MoveAttribDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif
