// $Header: /CAMCAD/4.3/LyrGroup.h 7     8/12/03 9:06p Kurt Van Ness $

#if !defined(AFX_LYRGROUP_H__A80D5F03_FBC6_11D1_BA40_0080ADB36DBB__INCLUDED_)
#define AFX_LYRGROUP_H__A80D5F03_FBC6_11D1_BA40_0080ADB36DBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LyrGroup.h : header file
//

#include "layer.h"

struct LayerGroupStruct;
typedef CTypedPtrList<CPtrList, LayerGroupStruct*> CLayerGroupList;

struct LayerGroupStruct
{
   CString name;
   short    artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
   short    electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
   short    physicalstacknumber;    // physical manufacturing stacking of layers, 
   CLayerList LayerList;
   CLayerGroupList GroupList;
};

struct GroupOrLayer
{
   BOOL IsAGroup;
   union
   {
      LayerGroupStruct *group;
      LayerStruct *layer;
   }; 
};

class CCEtoODBDoc;

/////////////////////////////////////////////////////////////////////////////
// LayerGroupsCreate dialog
class LayerGroupsCreate : public CDialog
{
// Construction
public:
   void AddItemToGroup(LayerGroupStruct *group, GroupOrLayer *add);
   void FillTree();
   CCEtoODBDoc *doc;
   LayerGroupsCreate(CWnd* pParent = NULL);   // standard constructor
   void OnCancel();  
   void DeleteAGroup(LayerGroupStruct *remove);

// Dialog Data
   //{{AFX_DATA(LayerGroupsCreate)
   enum { IDD = IDD_LAYER_GROUPS_CREATE };
   CTreeCtrl   m_tree;
   CListCtrl   m_list;
   //}}AFX_DATA
   CImageList *m_imageList;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerGroupsCreate)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LayerGroupsCreate)
   virtual BOOL OnInitDialog();
   afx_msg void OnNewGroup();
   afx_msg void OnAdd();
   afx_msg void OnRemove();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// LayerGroupsUse dialog
class LayerGroupsUse : public CDialog
{
// Construction
public:
   LayerGroupsUse(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LayerGroupsUse)
   enum { IDD = IDD_LAYER_GROUPS_USE };
   CListBox m_list;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerGroupsUse)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LayerGroupsUse)
   virtual BOOL OnInitDialog();
   afx_msg void OnTurnOn();
   afx_msg void OnTurnOff();
   afx_msg void OnChangeColor();
   afx_msg void OnApply();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LYRGROUP_H__A80D5F03_FBC6_11D1_BA40_0080ADB36DBB__INCLUDED_)
