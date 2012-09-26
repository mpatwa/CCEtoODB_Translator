// $Header: /CAMCAD/4.6/Format.h 44    4/25/07 4:31p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if ! defined (__Format_h__)
#define __Format_h__

#pragma once

#include "format_s.h"
#include "ccdoc.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CDXF_Format dialog

class CDXF_Format : public CDialog
{
// Construction
public:
   CDXF_Format(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CDXF_Format)
   enum { IDD = IDD_FORMAT_DXF_IN };
   CComboBox   m_Unit_CB;
   BOOL  m_Circ_Ap;
   BOOL  m_Proportional;
   CString  m_Layer_Prefix;
   int      m_Units;
   int      m_Polyline_Ends;
   double   m_Scale;
   BOOL  m_Donut_Ap;
   BOOL  m_fillPoly;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDXF_Format)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CDXF_Format)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGerbFormat dialog

class CGerbFormat : public CDialog
{
// Construction
public:
   CGerbFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CGerbFormat)
   enum { IDD = IDD_FORMAT_GERBER_IN };
   CSpinButtonCtrl   m_spin2;
   CSpinButtonCtrl   m_spin1;
   int      m_Decimal;
   BOOL  m_G75_Circle;
   CString  m_Aperture_Prefix;
   int      m_Type;
   int      m_Units;
   int      m_Zero_Suppression;
   double   m_Scale;
   int      m_Digits;
   BOOL  m_thermalMacros;
   BOOL  m_fire9xxx;
   //}}AFX_DATA

   FormatStruct *format;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGerbFormat)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CGerbFormat)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// DXFExportFormat dialog

class DXFExportFormat : public CDialog
{
// Construction
public:
   DXFExportFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(DXFExportFormat)
   enum { IDD = IDD_FORMAT_DXF_OUT };
   CComboBox   m_Unit_CB;
   double   m_realWidth;
   BOOL  m_roundEnds;
   double   m_scale;
   int      m_units;
   int      m_explode;
   BOOL  m_hollow;
   BOOL  m_export_hidden_entities;
   BOOL  m_use_probetype;
   BOOL  m_text_xscale;
   BOOL  m_text_unmirror;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DXFExportFormat)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(DXFExportFormat)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// GerberExportFormat dialog

class GerberExportFormat : public CDialog
{
// Construction
public:
   GerberExportFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GerberExportFormat)
   enum { IDD = IDD_FORMAT_GERBER_OUT };
   CSpinButtonCtrl   m_spin2;
   CSpinButtonCtrl   m_spin1;
   int      m_decimal;
   int      m_digits;
   int      m_type;
   int      m_units;
   int      m_zero;
   int      m_274;
   double   m_sect_ang;
   double   m_scale;
   int      m_fillDCode;
   BOOL     m_polyAreaFill;
   int      m_sectorize;
   BOOL     m_layerStencilThickness;
   //}}AFX_DATA

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GerberExportFormat)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(GerberExportFormat)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////
// FormatProtelIn dialog
class FormatProtelIn : public CDialog
{
// Construction
public:
   FormatProtelIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatProtelIn)
   enum { IDD = IDD_FORMAT_PROTEL_IN };
   CString  m_large;
   int      m_layer;
   CString  m_medium;
   CString  m_small;
   CString  m_smallest;
   double   m_scale;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatProtelIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatProtelIn)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatPDIFlay dialog

class FormatPDIFlay : public CDialog
{
// Construction
public:
   FormatPDIFlay(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatPDIFlay)
   enum { IDD = IDD_FORMAT_PDIF_LAY_OUT };
   double   m_scale;
   BOOL  m_TextRot;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatPDIFlay)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatPDIFlay)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatPDIFsch dialog

class FormatPDIFsch : public CDialog
{
// Construction
public:
   FormatPDIFsch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatPDIFsch)
   enum { IDD = IDD_FORMAT_PDIF_SCH_OUT };
   double   m_scale;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatPDIFsch)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatPDIFsch)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// TangoExportFormat dialog
class TangoExportFormat : public CDialog
{
// Construction
public:
   TangoExportFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TangoExportFormat)
   enum { IDD = IDD_FORMAT_TANGO_OUT };
   double   m_scale;
   //}}AFX_DATA
   CBitmapButton bitmap;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TangoExportFormat)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(TangoExportFormat)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// ORCADLTDExportFormat dialog
class ORCADLTDExportFormat : public CDialog
{
// Construction
public:
   ORCADLTDExportFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ORCADLTDExportFormat)
   enum { IDD = IDD_FORMAT_TANGO_OUT };
   double   m_scale;
   //}}AFX_DATA
   CBitmapButton bitmap;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ORCADLTDExportFormat)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ORCADLTDExportFormat)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FormatPadsOut dialog

class FormatPadsOut : public CDialog
{
// Construction
public:
   FormatPadsOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatPadsOut)
   enum { IDD = IDD_FORMAT_PADS };
   double   m_scale;
   BOOL  m_components;
   int      m_units;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatPadsOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatPadsOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// Format_IGES_Out dialog

class Format_IGES_Out : public CDialog
{
// Construction
public:
   Format_IGES_Out(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(Format_IGES_Out)
   enum { IDD = IDD_FORMAT_IGES_OUT };
   CComboBox   m_Unit_CB;
   int      m_explode;
   double   m_realWidth;
   BOOL  m_roundEnds;
   double   m_scale;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Format_IGES_Out)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(Format_IGES_Out)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatIPCin dialog

class FormatIPCin : public CDialog
{
// Construction
public:
   FormatIPCin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatIPCin)
   enum { IDD = IDD_FORMAT_IPC_IN };
   CComboBox   m_unitsCB;
   CString  m_prefix;
   double   m_scale;
   BOOL  m_newline;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatIPCin)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatIPCin)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatEdifIn dialog

class FormatEdifIn : public CDialog
{
// Construction
public:
   FormatEdifIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatEdifIn)
   enum { IDD = IDD_FORMAT_EDIF_IN };
   CComboBox   m_unitsCB;
   CString  m_prefix;
   double   m_scale;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatEdifIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatEdifIn)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// ProtelLayout dialog
class ProtelLayout : public CDialog
{
// Construction
public:
   ProtelLayout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ProtelLayout)
   enum { IDD = IDD_FORMAT_PROTEL_LAYOUT };
   double   m_scale;
   int      m_type;
   //}}AFX_DATA
   CBitmapButton bitmap;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ProtelLayout)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ProtelLayout)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// ProtelSchematic dialog
class ProtelSchematic : public CDialog
{
// Construction
public:
   ProtelSchematic(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ProtelSchematic)
   enum { IDD = IDD_FORMAT_PROTEL_SCHEMATIC };
   CComboBox   m_unitsCB;
   double   m_scale;
   //}}AFX_DATA
   FormatStruct *format;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ProtelSchematic)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ProtelSchematic)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CExcellonIn dialog
class CExcellonIn : public CDialog
{
// Construction
public:
   CExcellonIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CExcellonIn)
   enum { IDD = IDD_FORMAT_EXCELLON_IN };
   int      m_Type;
   int      m_Units;
   int      m_Zero_Suppression;
   BOOL  m_autoAp;
   int      m_format;
   //}}AFX_DATA
   FormatStruct *format;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CExcellonIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CExcellonIn)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatAllegroIn dialog

class FormatAllegroIn : public CDialog
{
// Construction
public:
   FormatAllegroIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatAllegroIn)
   enum { IDD = IDD_FORMAT_ALLEGRO_IN };
   BOOL  m_explodeFormat;
   BOOL  m_suppressClass;
   CString  m_configFile;
   CString  m_detected_format;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatAllegroIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatAllegroIn)
   afx_msg void OnChangeConfigFile();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatGencadOut dialog

class FormatGencadOut : public CDialog
{
// Construction
public:
   FormatGencadOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatGencadOut)
   enum { IDD = IDD_FORMAT_GENCAD_OUT };
   BOOL  m_createPanelFile;
   int      m_version;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatGencadOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatGencadOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatHP3070Out dialog

class FormatHP3070Out : public CResizingDialog
{
// Construction
public:
   //FormatHP3070Out(CWnd* pParent = NULL);   // standard constructor
	FormatHP3070Out(CCEtoODBDoc* doc, FormatStruct* format, CWnd* pParent = NULL);

   CString m_writerName;

// Dialog Data
   //{{AFX_DATA(FormatHP3070Out)
   enum { IDD = IDD_FORMAT_HP3070 };
   //}}AFX_DATA

   virtual CString GetDialogProfileEntry() { return CString("FormatHP3070OutDialog"); }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatHP3070Out)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(FormatHP3070Out)
   afx_msg void OnChangeRuleFile();
   afx_msg void OnChangeBdf();
   afx_msg void OnChangeNcf();
   afx_msg void OnChangeLog();
	virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   bool HasPcbFileWithDFT();

public:
   BOOL m_ncNet;
   BOOL  m_vias;
   CString  m_ruleFile;
   CString  m_bdf;
   CString  m_ncf;
   CString  m_log;
   BOOL  m_uPins;
   BOOL  m_delete_3070_attributes;
   BOOL m_exportVariant;
	int   m_generate_DFT;
	CCEtoODBDoc* m_doc;
	FormatStruct *m_format;
	afx_msg void OnBnClickedRadio3070Dft();
	afx_msg void OnBnClickedRadio3070Out();
	afx_msg void OnBnClickedRadio3070Rules();
	afx_msg void OnBnClickedRadio3070FromDft();
	afx_msg void OnBnClickedDeleteHp3070Attributes();
	afx_msg void OnBnClickedUPins();
	afx_msg void OnBnClickedVias();
	afx_msg void OnBnClickedNcNet();
   afx_msg void OnBnClickedReset();

   void SetFormatDefaults();
   void LoadFormatSettings();
   void SaveFormatSettings();
   afx_msg void OnBnClickedExportvariant();
};
/////////////////////////////////////////////////////////////////////////////
// FormatInsertionOut dialog

class FormatInsertionOut : public CDialog
{
// Construction
public:
   FormatInsertionOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatInsertionOut)
   enum { IDD = IDD_FORMAT_INSERTION_OUT };
   int      m_type;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatInsertionOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatInsertionOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatTestOut dialog

class FormatTestOut : public CDialog
{
// Construction
public:
   FormatTestOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatTestOut)
   enum { IDD = IDD_FORMAT_TEST_OUT };
   int      m_type;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatTestOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatTestOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatExcellonOut dialog

class FormatExcellonOut : public CDialog
{
// Construction
public:
   FormatExcellonOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatExcellonOut)
   enum { IDD = IDD_FORMAT_EXCELLON_OUT };
   CListBox m_toolsLB;
   int      m_Units;
   //}}AFX_DATA
   FormatStruct *m_format;
   CCEtoODBDoc *doc;

protected:
	BOOL m_drillTypes;
	BOOL m_probeSurface;
	BOOL m_includeToolingHoles;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatExcellonOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatExcellonOut)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

	afx_msg void OnDeltaposDecimalSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedExcellonUnits();
	afx_msg void OnBnClickedExcellonDrillsPcb();
	afx_msg void OnBnClickedExcellonDrillsProbe();

	void setDigitFields(int unit);  // 0-English, 1-Metric
public:
	BOOL m_usePilotHoles;
	afx_msg void OnBnClickedExcellonIncludeDecPoint();
	BOOL m_includeDecimalPoint;
	int m_zeroSuppression;
	
};
/////////////////////////////////////////////////////////////////////////////
// FormatMentorOut dialog

class FormatMentorOut : public CDialog
{
// Construction
public:
   FormatMentorOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatMentorOut)
   enum { IDD = IDD_FORMAT_MENTOR_OUT };
   int      m_units;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatMentorOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatMentorOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatIPCOut dialog

class FormatIPCOut : public CDialog
{
// Construction
public:
   FormatIPCOut(FormatStruct *format = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatIPCOut)
   enum { IDD = IDD_FORMAT_IPC_OUT };
   CComboBox   m_Unit_CB;

   int      m_radio;
   int      m_units;
   //}}AFX_DATA
   FormatStruct *m_format;

public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatIPCOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatIPCOut)
      // NOTE: the ClassWizard will add member functions here
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// Format_GenCAM_In dialog

class Format_GenCAM_In : public CDialog
{
// Construction
public:
   Format_GenCAM_In(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(Format_GenCAM_In)
   enum { IDD = IDD_FORMAT_GENCAM_IN };
   BOOL  m_0;
   BOOL  m_1;
   BOOL  m_2;
   BOOL  m_3;
   BOOL  m_4;
   BOOL  m_5;
   BOOL  m_6;
   BOOL  m_7;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Format_GenCAM_In)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(Format_GenCAM_In)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// Format_GenCAM_Out dialog

class Format_GenCAM_Out : public CDialog
{
// Construction
public:
   Format_GenCAM_Out(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(Format_GenCAM_Out)
   enum { IDD = IDD_FORMAT_GENCAM_OUT };
   BOOL  m_0;
   BOOL  m_1;
   BOOL  m_2;
   BOOL  m_3;
   BOOL  m_4;
   BOOL  m_5;
   BOOL  m_6;
   BOOL  m_7;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Format_GenCAM_Out)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(Format_GenCAM_Out)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FormatHughesIn dialog

class FormatHughesIn : public CDialog
{
// Construction
public:
   FormatHughesIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatHughesIn)
   enum { IDD = IDD_FORMAT_HUGHES };
   CString  m_file1;
   CString  m_file2;
   int      m_side1;
   int      m_side2;
   //}}AFX_DATA
   BOOL twoFiles;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatHughesIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatHughesIn)
   afx_msg void OnTop1();
   afx_msg void OnTop2();
   afx_msg void OnBottom1();
   afx_msg void OnBottom2();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatAllegroOut dialog

class FormatAllegroOut : public CDialog
{
// Construction
public:
   FormatAllegroOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatAllegroOut)
   enum { IDD = IDD_FORMAT_ALLEGRO_OUT };
   CString  m_check;
   CString  m_out;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatAllegroOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatAllegroOut)
   afx_msg void OnChangeLog();
   afx_msg void OnChangeNcf();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatIgesIn dialog

class FormatIgesIn : public CDialog
{
// Construction
public:
   FormatIgesIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatIgesIn)
   enum { IDD = IDD_FORMAT_IGES_IN };
   CComboBox   m_unitCB;
   BOOL  m_fill;
   double   m_scale;
   int      m_units;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatIgesIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatIgesIn)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FormatVB99Out dialog
class FormatVB99Out : public CDialog
{
// Construction
public:
   FormatVB99Out(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatVB99Out)
   enum { IDD = IDD_FORMAT_VB99_OUT };
   CString  m_ncf;
   CString  m_log;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatVB99Out)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatVB99Out)
   afx_msg void OnChangeLog();
   afx_msg void OnChangeNcf();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// ODB_dlg dialog

class ODB_dlg : public CDialog
{
// Construction
public:
   ODB_dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ODB_dlg)
   enum { IDD = IDD_ODB_DLG };
   int      m_read;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ODB_dlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ODB_dlg)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatDMISOut dialog

class FormatDMISOut : public CDialog
{
// Construction
public:
   FormatDMISOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatDMISOut)
   enum { IDD = IDD_FORMAT_DMIS_OUT };
   CString  m_realWidth;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatDMISOut)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatDMISOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FormatTakayaOut1 dialog
class FormatTakayaOut1 : public CDialog
{
// Construction
public:
   FormatTakayaOut1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatTakayaOut1)
   enum { IDD = IDD_FORMAT_TAKAYA_OUT1 };
   BOOL  m_bottommountedcomps;
   BOOL  m_bottomsurfaceallow;
   int   m_maxhitcount;
   int   m_preferedsurface;
   BOOL  m_testtypecapacitors;
   BOOL  m_testtypediodes;
   BOOL  m_typetestinductors;
   BOOL  m_testtyperesistor;
   BOOL  m_testtypeshorts;
   BOOL  m_testtypetransistors;
   BOOL  m_testtypezener;
   BOOL  m_topmountedcomps;
   BOOL  m_topsurfaceallow;
   CString  m_filenamePrefix;
   //}}AFX_DATA
   BOOL CA9;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatTakayaOut1)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatTakayaOut1)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   CString m_extension;
   BOOL m_IC_diode;
   BOOL m_IC_capacitance;
};

/////////////////////////////////////////////////////////////////////////////
// FormatCADIFIn dialog
class FormatCADIFIn : public CDialog
{
// Construction
public:
   FormatCADIFIn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatCADIFIn)
   enum { IDD = IDD_FORMAT_CADIF_IN };
   CString  m_configFile;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatCADIFIn)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatCADIFIn)
   afx_msg void OnChangeConfigFile();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// FormatTRIExport dialog

class FormatTRIExport : public CDialog
{
// Construction
public:
   FormatTRIExport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatTRIExport)
   enum { IDD = IDD_FORMAT_TRI };
   BOOL  m_bottommountedcomps;
   BOOL  m_testtypecapacitors;
   BOOL  m_testtypediodes;
   BOOL  m_testtypeinductors;
   BOOL  m_testtypejumpers;
   BOOL  m_testtypephotocouplers;
   BOOL  m_testtyperesitors;
   BOOL  m_testtypesrc_triac;
   BOOL  m_testtypetransistors;
   BOOL  m_testtypetransistorshfe;
   BOOL  m_testtypeunknown;
   BOOL  m_topmountedcomps;
   BOOL  m_testtypecrystal;
   BOOL  m_testtypemosfet_jfet;
   CString  m_filename;
   BOOL  m_testtypeIC;
   //}}AFX_DATA
	int m_datformat;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatTRIExport)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatTRIExport)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FormatTeradyneAoi dialog
class FormatTeradyneAoi : public CDialog
{
// Construction
public:
   FormatTeradyneAoi(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatTeradyneAoi)
   enum { IDD = IDD_FORMAT_TEDARYNE_AOI };
   BOOL  m_fiducials;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatTeradyneAoi)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(FormatTeradyneAoi)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// FormatAgilentAOIOut dialog
class FormatAgilentAOIOut : public CDialog
{
// Construction
public:
   FormatAgilentAOIOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(FormatAgilentAOIOut)
   enum { IDD = IDD_FORMAT_AGILENT_AOI_OUT };

private:
   BOOL  m_smd_only;
   int      m_machine_type;
   BOOL m_useZeroRotationForComplexApertures;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(FormatAgilentAOIOut)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
public:
   AoiMachineTag getMachineType() const;
   void setMachineType(AoiMachineTag machineType) { m_machine_type = machineType; }

   bool getSmdOnlyFlag() const { return (m_smd_only != 0); }
   void setSmdOnlyFlag(bool flag) { m_smd_only = flag; }

   bool getUseZeroRotationForComplexAperturesFlag() const { return (m_useZeroRotationForComplexApertures != 0); }
   void setUseZeroRotationForComplexAperturesFlag(bool flag) { m_useZeroRotationForComplexApertures = flag; }

protected:

   // Generated message map functions
   //{{AFX_MSG(FormatAgilentAOIOut)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// FormatSiemensQDOut dialog
class FormatSiemensQDOut : public CDialog
{
   DECLARE_DYNAMIC(FormatSiemensQDOut)

public:
   FormatSiemensQDOut(CWnd* pParent = NULL);   // standard constructor
   virtual ~FormatSiemensQDOut();
   enum { IDD = IDD_FORMAT_SIEMENS_QD };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   DECLARE_MESSAGE_MAP()

public:
   BOOL m_glueTop;
   BOOL m_glueBot;
   CString m_fileName;
   int m_boardFidNum;
   int m_panelFidNum;
   CString m_panelThickness;
   CString m_boardThickness;
	CString m_boardPartNumber;
   CString m_boardTolerance;
   CString m_panelTolerance;
   bool m_enableFileName;

   afx_msg void OnOK();
   virtual BOOL OnInitDialog();
};


///////////////////////////////////////////////////////////////////////////////
// FormatPCBFileDlg dialog
class FormatPCBFileDlg : public CDialog
{
   DECLARE_DYNAMIC(FormatPCBFileDlg)

public:
   FormatPCBFileDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~FormatPCBFileDlg();

// Dialog Data
   enum { IDD = IDD_FORMAT_PCBFILE };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   CListBox m_list;
   CCEtoODBDoc *doc;
   FileStruct *pcbFile;
   afx_msg void OnOK();
   int LegalFileCount();
};

//_____________________________________________________________________________
class CFormatMvpAoiCompOutDialog : public CDialog
{
	DECLARE_DYNAMIC(CFormatMvpAoiCompOutDialog)

private:
   BOOL m_smdComponentsFlag;
   BOOL m_thComponentsFlag;
	BOOL m_optComponentFieldSix;

public:
	CFormatMvpAoiCompOutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFormatMvpAoiCompOutDialog();

// Dialog Data
	enum { IDD = IDD_FormatMvpAoiCompOut };

   void setSettings(const CMvpAoiSettings& mvpAoiSettings);
   void getSettings(CMvpAoiSettings& mvpAoiSettings);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedSmdComponents();
   afx_msg void OnBnClickedThComponents();
	afx_msg void OnBnClickedOk();
};

#endif
