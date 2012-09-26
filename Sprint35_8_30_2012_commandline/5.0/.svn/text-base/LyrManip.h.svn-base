// $Header: /CAMCAD/4.5/LyrManip.h 13    12/23/05 1:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"

void SendLayerToBack(CCEtoODBDoc *doc, int layerNum);
void BringLayerToFront(CCEtoODBDoc *doc, int layerNum);

void MoveLayer(CCEtoODBDoc *doc, LayerStruct *origLayer, LayerStruct *newLayer, BOOL Remove);
void CopyLayer(CCEtoODBDoc *doc, LayerStruct *origLayer, LayerStruct *newLayer);
void DeleteLayer(CCEtoODBDoc *doc, LayerStruct *layer, BOOL Remove);

/////////////////////////////////////////////////////////////////////////////
// LayerManipulation dialog
class LayerManipulation : public CDialog
{
// Construction
public:
   LayerManipulation(CWnd* pParent = NULL);   // standard constructor
   ~LayerManipulation();

// Dialog Data
   //{{AFX_DATA(LayerManipulation)
   enum { IDD = IDD_LAYER_MANIPULATION };
   CComboBox   m_newCB;
   CListBox m_origLB;
   int      m_function;
   BOOL  m_removeLayer;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   LayerStruct *origLayer, *newLayer;
   CToolTipCtrl tooltip;
   CString hint;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerManipulation)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LayerManipulation)
   virtual BOOL OnInitDialog();
   afx_msg void OnMoveSelected();
   afx_msg void OnCopySelected();
   afx_msg void OnDeleteSelected();
   afx_msg void OnGo();
   afx_msg BOOL UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnPurgeUnusedLayers();
};

/////////////////////////////////////////////////////////////////////////////
// PurgeUnusedLayers dialog
class PurgeUnusedLayers : public CDialog
{
// Construction
public:
   PurgeUnusedLayers(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PurgeUnusedLayers)
   enum { IDD = IDD_PURGE_UNUSED_LAYERS };
   CListBox m_layersLB;
   BOOL  m_purge_electrical;
   //}}AFX_DATA
   CCEtoODBDoc *doc;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PurgeUnusedLayers)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PurgeUnusedLayers)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnPurgeElectrical();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

