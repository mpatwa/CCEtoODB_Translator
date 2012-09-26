// $Header: /CAMCAD/4.3/FormatHP.h 7     8/12/03 9:06p Kurt Van Ness $

#include "format_s.h"
#include "block.h"
#include "ccdoc.h"

/////////////////////////////////////////////////////////////////////////////
// FormatHPin dialog

class FormatHPin : public CDialog
{
// Construction
public:
   FormatHPin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatHPin)
   enum { IDD = IDD_FORMAT_HP_IN };
   CScrollBar  m_scroll;
   CComboBox   m_unitsCB;
   BOOL  m_circ2ap;
   CString  m_layer1;
   CString  m_layer2;
   CString  m_layer3;
   CString  m_layer4;
   CString  m_layer5;
   CString  m_layer6;
   CString  m_layer7;
   CString  m_layer8;
   CString  m_pen1;
   CString  m_pen2;
   CString  m_pen3;
   CString  m_pen4;
   CString  m_pen5;
   CString  m_pen6;
   CString  m_pen7;
   CString  m_pen8;
   CString  m_prefix;
   BOOL  m_prop_text;
   CString  m_width1;
   CString  m_width2;
   CString  m_width3;
   CString  m_width4;
   CString  m_width5;
   CString  m_width6;
   CString  m_width7;
   CString  m_width8;
   double   m_scale;
   BOOL  m_space;
   BOOL  m_ignorePenWidth;
   //}}AFX_DATA
   int decimals;
   FormatStruct *format;
   int pennum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatHPin)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void ReadPens();
   void WritePens();

   // Generated message map functions
   //{{AFX_MSG(FormatHPin)
   virtual BOOL OnInitDialog();
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDI_Out dialog
class CDI_Out : public CDialog
{
// Construction
public:
   CDI_Out(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CDI_Out)
   enum { IDD = IDD_FORMAT_CDI_OUT };
   CScrollBar  m_scroll;
   CString  m_size1;
   CString  m_size2;
   CString  m_size3;
   CString  m_size4;
   CString  m_size5;
   CString  m_size6;
   CString  m_size7;
   CString  m_size8;
   int      m_width1;
   int      m_width2;
   int      m_width3;
   int      m_width4;
   int      m_width5;
   int      m_width6;
   int      m_width7;
   int      m_width8;
   double   m_scale;
   //}}AFX_DATA
   FormatStruct *format;
   CCEtoODBDoc *doc;
   int widths;
   int widthNum;
   int decimals;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDI_Out)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void ReadWidths();
   void WriteWidths();

   // Generated message map functions
   //{{AFX_MSG(CDI_Out)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// Format_HP_Out dialog

class Format_HP_Out : public CDialog
{
// Construction
public:
   Format_HP_Out(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(Format_HP_Out)
   enum { IDD = IDD_FORMAT_HP_OUT };
   CScrollBar  m_scroll;
   BOOL  m_HPGL2;
   CString  m_pageFeed;
   CString  m_pen1;
   CString  m_pen2;
   CString  m_pen3;
   CString  m_pen4;
   CString  m_pen5;
   CString  m_pen6;
   CString  m_pen7;
   CString  m_pen8;
   CString  m_width1;
   CString  m_width2;
   CString  m_width3;
   CString  m_width4;
   CString  m_width5;
   CString  m_width6;
   CString  m_width7;
   CString  m_width8;
   BOOL  m_pageFeedOn;
   double   m_scale;
   double   m_realWidth;
   BOOL  m_hollow;
   //}}AFX_DATA
   int decimals;
   FormatStruct *format;
   int pennum;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Format_HP_Out)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void ReadPens();
   void WritePens();

   // Generated message map functions
   //{{AFX_MSG(Format_HP_Out)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
