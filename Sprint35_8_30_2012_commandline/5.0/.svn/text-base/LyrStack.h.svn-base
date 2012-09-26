// $Header: /CAMCAD/4.3/LyrStack.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_LYRSTACK_H__3EAF9C41_F157_11D1_BA40_0080ADB36DBB__INCLUDED_)
#define AFX_LYRSTACK_H__3EAF9C41_F157_11D1_BA40_0080ADB36DBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// LayerStack dialog

class LayerStack : public CDialog
{
// Construction
public:
   LayerStack(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LayerStack)
   enum { IDD = IDD_LIST_LAYER_STACK };
   CListCtrl   m_list;
   int      m_show;
   int      m_layersFrom;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerStack)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillListCtrl();
   void LoopBlock(BlockStruct *block, BOOL AllInsertTypes, int layer);

   // Generated message map functions
   //{{AFX_MSG(LayerStack)
   virtual BOOL OnInitDialog();
   afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEdit();
   afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnLayertype();
   afx_msg void OnPadstacks();
   afx_msg void OnComponents();
   afx_msg void OnAllLayers();
   afx_msg void OnBoards();
   afx_msg void OnApply();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LYRSTACK_H__3EAF9C41_F157_11D1_BA40_0080ADB36DBB__INCLUDED_)
