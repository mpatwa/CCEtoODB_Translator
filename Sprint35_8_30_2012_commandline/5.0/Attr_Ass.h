// $Header: /CAMCAD/4.5/Attr_Ass.h 14    1/27/05 7:42p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__Attr_Ass_H__)
#define __Attr_Ass_H__

#pragma once

#include "Resource.h"
#include "ResizingDialog.h"
#include "horz_lb.h"

class CCEtoODBDoc;
class CAttributes;

/////////////////////////////////////////////////////////////////////////////
// AttributeAssignment dialog
class AttributeAssignment : public CResizingDialog
{
// Construction
public:
   AttributeAssignment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(AttributeAssignment)
   enum { IDD = IDD_ATTRIBUTE_ASSIGNMENT };
   CHorzListBox   m_qualifierLB;
   CHorzListBox   m_selectionLB;
   CListBox m_itLB;
   CComboBox   m_itemCB;
   CComboBox   m_attribCB;
   CComboBox   m_keywordCB;
   CString  m_keyword;
   CString  m_value;
   int      m_type;
   CString  m_qualifierText;
   CString  m_selectionText;
   BOOL  m_itPanel;
   CString  m_valueType;
   int      m_assignCount;
   int      m_deleteCount;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   int decimals;
   BOOL AssigningTestAttrib;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AttributeAssignment)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
   void OnCancel();

   virtual CString GetDialogProfileEntry() { return CString("AttributeAssignmentDialog"); }

// Implementation
protected:
   BOOL InsertFilter(int inserttype);
   void AddItemToStorageLB(CListBox *lb, const char *string, void *ptr);
   void CleanLBMem(CListBox *lb);
   void FillAttribKeywords(CAttributes** map);
   void EnableInsertTypes(BOOL enable);
   void OnUpdateByInserttype();
   void FillInsertTypeLB();

   // Generated message map functions
   //{{AFX_MSG(AttributeAssignment)
   virtual BOOL OnInitDialog();
   afx_msg void OnAssign();
   afx_msg void OnTypeAttrib();
   afx_msg void OnTypeItem();
   afx_msg void FillSelections();
   afx_msg void OnSelchangeSelection();
   afx_msg void OnItOff();
   afx_msg void OnItOn();
   afx_msg void OnSelchangeKeywordCb();
   afx_msg void OnSelchangeQualifier();
   afx_msg void OnDelete();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif
