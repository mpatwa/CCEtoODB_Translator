// $Header: /CAMCAD/4.5/Net_Hilt.h 12    4/07/05 7:20p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "dblrect.h"

class CCEtoODBDoc;

bool NetExtents(CCEtoODBDoc *doc, const char *netName, CDblRect *rect);
bool NetExtents(CCEtoODBDoc *doc, const char *netName, double *xmin, double *xmax, double *ymin, double *ymax);
void Hide_Nets_All(CCEtoODBDoc *doc, int hide);
int  Hide_Net(CCEtoODBDoc *doc, int netValueIndex, int hide);

/////////////////////////////////////////////////////////////////////////////
// SelectNets dialog
class SelectNets : public CDialog
{
// Construction
public:
   SelectNets(CWnd* pParent = NULL);   // standard constructor
   
// Dialog Data
   //{{AFX_DATA(SelectNets)
   enum { IDD = IDD_SELECT_NETS };
   CListBox m_netLB;
   CComboBox   m_fileCB;
   BOOL  m_show_selected_only;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   NetStruct *net;
   int width;
   BOOL AutoColor;
   
// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SelectNets)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL
   
// Implementation
public:
   void Refresh();
   void FillFileCB();
   void FillNetLB();
   void UnhighlightNets();
   void MarkNet(NetStruct *Net, COLORREF color);
   void Apply(BOOL Redraw);
   void ColorNet(COLORREF color);
   void do_show_selected_only();

   // Generated message map functions
   //{{AFX_MSG(SelectNets)
   afx_msg void OnSelchangeFile();
   afx_msg void OnClear();
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void OnZoomTo();
   afx_msg void OnNetUnhighlight();
   afx_msg void OnNetWhite();
   afx_msg void OnNetRed();
   afx_msg void OnNetOrange();
   afx_msg void OnNetYellow();
   afx_msg void OnNetGreen();
   afx_msg void OnNetLtBlue();
   afx_msg void OnNetBlue();
   afx_msg void OnNetPurple();
   afx_msg void OnNetCustom();
   afx_msg void OnNetOrig();
   afx_msg void OnNetShowSelectedOnly();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnColors();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// Netlist_RemoveNetnames dialog
class Netlist_RemoveNetnames : public CDialog
{
// Construction
public:
   Netlist_RemoveNetnames(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(Netlist_RemoveNetnames)
   enum { IDD = IDD_NETLIST_REMOVE_NETNAMES };
   CListBox m_netsLB;
   BOOL  m_unusedPins;
   BOOL  m_allFeatures;
   BOOL  m_allPins;
   BOOL  m_allTraces;
   BOOL  m_selFeatures;
   BOOL  m_selPins;
   BOOL  m_selTraces;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Netlist_RemoveNetnames)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(Netlist_RemoveNetnames)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnAllFeatures();
   afx_msg void OnAllPins();
   afx_msg void OnAllTraces();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

