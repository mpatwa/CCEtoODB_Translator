// $Header: /CAMCAD/4.3/Lyr_Lyr.h 7     8/12/03 9:06p Kurt Van Ness $

/****************************************************************************
* Project CAMCAD
* Router Solutions Inc.
* Copyright © 1994-98. All Rights Reserved.
*/

#pragma once

#include "layer.h"
#include "ccdoc.h"

struct LayerLayerStruct 
{
  CString *name;
  CString *newName;
  int layerNum;
  BOOL visible;
};

struct LayerPenStruct 
{
  CString name;
  int PenNum;
  int layerNum;
};

const char *Get_Layer_Layername(int layernum);
void Free_Layer_Layername();
void LoadLayers(CCamCadData& camCadData);

/////////////////////////////////////////////////////////////////////////////
// Layer_LayerMap dialog
class Layer_LayerMap : public CDialog
{
// Construction
public:
   Layer_LayerMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(Layer_LayerMap)
   enum { IDD = IDD_LAYER_LAYER };
   CComboBox   m_newLayer8;
   CComboBox   m_newLayer7;
   CComboBox   m_newLayer6;
   CComboBox   m_newLayer5;
   CComboBox   m_newLayer4;
   CComboBox   m_newLayer3;
   CComboBox   m_newLayer2;
   CComboBox   m_newLayer1;
   CScrollBar  m_scroll;
   CString  m_layer1;
   CString  m_layer2;
   CString  m_layer3;
   CString  m_layer4;
   CString  m_layer5;
   CString  m_layer6;
   CString  m_layer7;
   CString  m_layer8;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   CString keyword;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Layer_LayerMap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void WriteData(int pos);
   void ReadData();
   void FillComboBoxes();
   void NoShow();

   // Generated message map functions
   //{{AFX_MSG(Layer_LayerMap)
   virtual BOOL OnInitDialog();
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual void OnCancel();
   virtual void OnOK();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
