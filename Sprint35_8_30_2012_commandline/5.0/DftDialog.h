// $Header: /CAMCAD/5.0/DftDialog.h 2     3/09/07 9:43p Kurt Van Ness $

#if !defined(__DftDialog_h__)
#define __DftDialog_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "find.h"
#include "qfelib.h"
#include "region.h"
#include "xmldomwrapper.h"
#include "DFTShared.h"
#include "AccessAnalysis.h"
#include "DcaDft.h"

//#define DebugAccessAnalysisOffset

/////////////////////////////////////////////////////////////////////////////
// TestProbeSequence dialog
class TestProbeSequence : public CDialog
{
// Construction
public:
   TestProbeSequence(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TestProbeSequence)
   enum { IDD = IDD_TESTPROBE_SEQUENCE };
   int      m_algorithm;
   int      m_startingPoint;
   CString  m_width;
   CString  m_prefix;
   int      m_name_increment;
   int      m_iStartNumber;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TestProbeSequence)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(TestProbeSequence)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CKelvinAnalysisDialog dialog
class CKelvinAnalysisDialog : public CDialog
{
   DECLARE_DYNAMIC(CKelvinAnalysisDialog)

public:
   CKelvinAnalysisDialog(CCEtoODBDoc *doc, CWnd* pParent = NULL);   // standard constructor
   virtual ~CKelvinAnalysisDialog();

// Dialog Data
   enum { IDD = IDD_KELVIN_ANALYSIS };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()
private:
   BOOL m_useResistor;
   BOOL m_useCapacitor;
   BOOL m_useInductor;
   double m_resistorValue;
   double m_capacitorValue;

   CCEtoODBDoc *pDoc;

   void loadDefaultValues();
   void saveDefaultValues();

public:
   afx_msg void OnBnClickedResistorChk();
   afx_msg void OnBnClickedCapacitorChk();
   afx_msg void OnBnClickedProcess();
};

int doKelvinAnalysis(CCEtoODBDoc *doc, bool useResistors, double resistorValue, bool useCapacitors, double capacitorValue, bool useInductor);

#endif // !defined(__Dft_h__)
