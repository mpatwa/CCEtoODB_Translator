// $Header: /CAMCAD/4.3/Lyr_Pen.h 7     8/12/03 9:06p Kurt Van Ness $

#include "layer.h"
#include "ccdoc.h"

void Free_Layer_PenMap();
int Get_Layer_PenNum(int layerNum);

/////////////////////////////////////////////////////////////////////////////
// LayerPenMap dialog

class LayerPenMap : public CDialog
{
// Construction
public:
   LayerPenMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LayerPenMap)
   enum { IDD = IDD_LAYER_PEN };
   CComboBox   m_pen8;
   CComboBox   m_pen7;
   CComboBox   m_pen6;
   CComboBox   m_pen5;
   CComboBox   m_pen4;
   CComboBox   m_pen3;
   CComboBox   m_pen2;
   CComboBox   m_pen1;
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

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerPenMap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void WriteData(int pos);
   void ReadData();

   // Generated message map functions
   //{{AFX_MSG(LayerPenMap)
   virtual BOOL OnInitDialog();
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
