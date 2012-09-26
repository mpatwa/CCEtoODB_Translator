// $Header: /CAMCAD/5.0/Format.cpp 61    5/22/07 6:32p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Format.h"
#include "resource.h"
#include "DFT.h"
#include "Variant.h"
#include ".\format.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Probably should move these convenience funcs to RwLib with rest of CRegistry stuff

static bool SaveSettingToRegistry(CString readerwritername, CString valuename, CString value)
{
   // Save setting in registry, return true if is appears to work, otherwise false.
   // Always goes under "Settings" at upper level, then under reader/writer name.

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey( readerwritername );

      if (registryKey.isOpen())
      { 
         if (registryKey.setValue(valuename, value))
         {
            return true;
         }
      }
   }

   return false;
}

static bool SaveSettingToRegistry(CString readerwritername, CString valuename, bool value)
{
   // Save setting in registry, return true if is appears to work, otherwise false.
   // Always goes under "Settings" at upper level, then under reader/writer name.

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey( readerwritername );

      if (registryKey.isOpen())
      { 
         if (registryKey.setValue(valuename, value))
         {
            return true;
         }
      }
   }

   return false;
}


static bool SaveSettingToRegistry(CString readerwritername, CString valuename, int value)
{
   // Save setting in registry, return true if is appears to work, otherwise false.
   // Always goes under "Settings" at upper level, then under reader/writer name.

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey( readerwritername );

      if (registryKey.isOpen())
      { 
         if (registryKey.setValue(valuename, value))
         {
            return true;
         }
      }
   }

   return false;
}

static bool LoadSettingFromRegistry(CString readerwritername, CString valuename, CString &value)
{
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey( readerwritername );

      if (registryKey.isOpen())
      { 
         if (registryKey.getValue(valuename, value))
         {
            return true;
         }
      }
   }

   return false;
}

static bool LoadSettingFromRegistry(CString readerwritername, CString valuename, bool &value)
{
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey( readerwritername );

      if (registryKey.isOpen())
      { 
         if (registryKey.getValue(valuename, value))
         {
            return true;
         }
      }
   }

   return false;
}

static bool LoadSettingFromRegistry(CString readerwritername, CString valuename, int &value)
{
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey( readerwritername );

      if (registryKey.isOpen())
      { 
         if (registryKey.getValue(valuename, value))
         {
            return true;
         }
      }
   }

   return false;
}

/////////////////////////////////////////////////////////////////////////////
// CDXF_Format dialog
CDXF_Format::CDXF_Format(CWnd* pParent /*=NULL*/)
   : CDialog(CDXF_Format::IDD, pParent)
{
   //{{AFX_DATA_INIT(CDXF_Format)
   m_Circ_Ap = FALSE;
   m_Proportional = TRUE;
   m_Layer_Prefix = _T("");
   m_Units = -1;
   m_Polyline_Ends = 0;
   m_Scale = 1.0;
   m_Donut_Ap = FALSE;
   m_fillPoly = FALSE;
   //}}AFX_DATA_INIT
}

void CDXF_Format::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDXF_Format)
   DDX_Control(pDX, IDC_UNITS, m_Unit_CB);
   DDX_Check(pDX, IDC_DXF_CIRC_AP, m_Circ_Ap);
   DDX_Check(pDX, IDC_DXF_PROPORTIONAL, m_Proportional);
   DDX_Text(pDX, IDC_DXF_LAYER_PREFIX, m_Layer_Prefix);
   DDX_CBIndex(pDX, IDC_UNITS, m_Units);
   DDX_Radio(pDX, IDC_DXF_POLY_ENDS, m_Polyline_Ends);
   DDX_Text(pDX, IDC_SCALE, m_Scale);
   DDV_MinMaxDouble(pDX, m_Scale, 1.e-010, 999999999999.);
   DDX_Check(pDX, IDC_DXF_DONUT_AP, m_Donut_Ap);
   DDX_Check(pDX, IDC_FILL_POLY, m_fillPoly);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDXF_Format, CDialog)
   //{{AFX_MSG_MAP(CDXF_Format)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDXF_Format message handlers

BOOL CDXF_Format::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (int i=0; i < MAX_UNITS; i++)
      m_Unit_CB.AddString(GetUnitName(i));

   m_Scale = format->Scale;
   m_Layer_Prefix = format->prefix;
   m_Polyline_Ends = format->DXF.PolyEnds;
   m_Proportional = format->DXF.Proportional;
   m_Circ_Ap = format->DXF.Circles_2_Aps;
   m_Donut_Ap = format->DXF.Donuts_2_Aps;
   m_fillPoly = format->FillPolys;
   m_Units = format->DXF.Units;

   m_Unit_CB.SetCurSel(m_Units);
   
   UpdateData(FALSE); // update dialog controls

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDXF_Format::OnOK() 
{
   CDialog::OnOK();

   format->Scale = m_Scale;
   strcpy(format->prefix, m_Layer_Prefix);

   switch (m_Polyline_Ends) 
   {
      case 0:
         format->DXF.PolyEnds = T_ROUND;
      break;
      case 1:
         format->DXF.PolyEnds = 0;
      break;
   }

   format->DXF.Proportional = m_Proportional;
   format->DXF.Circles_2_Aps = m_Circ_Ap;
   format->DXF.Donuts_2_Aps = m_Donut_Ap;
   format->FillPolys = m_fillPoly;

   format->DXF.Units = m_Unit_CB.GetCurSel();
}

/////////////////////////////////////////////////////////////////////////////
// CGerbFormat dialog
CGerbFormat::CGerbFormat(CWnd* pParent /*=NULL*/)
   : CDialog(CGerbFormat::IDD, pParent)
{
   //{{AFX_DATA_INIT(CGerbFormat)
   m_Decimal = 0;
   m_G75_Circle = 0;
   m_Aperture_Prefix = _T("");
   m_Type = 0;
   m_Units = 0;
   m_Zero_Suppression = 0;
   m_Scale = 1.0;
   m_Digits = 0;
   m_thermalMacros = 0;
   m_fire9xxx = FALSE;
   //}}AFX_DATA_INIT
}

void CGerbFormat::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CGerbFormat)
   DDX_Control(pDX, IDC_SPIN3, m_spin2);
   DDX_Control(pDX, IDC_SPIN2, m_spin1);
   DDX_Text(pDX, IDC_GERB_DECIMAL, m_Decimal);
   DDV_MinMaxInt(pDX, m_Decimal, 0, 2147483647);
   DDX_Check(pDX, IDC_G75_CIRCLE, m_G75_Circle);
   DDX_Text(pDX, IDC_GERB_APERTURE_PREFIX, m_Aperture_Prefix);
   DDX_Radio(pDX, IDC_GERB_TYPE, m_Type);
   DDX_Radio(pDX, IDC_GERB_UNITS, m_Units);
   DDX_Radio(pDX, IDC_GERB_ZERO_SUPPRESSION, m_Zero_Suppression);
   DDX_Text(pDX, IDC_SCALE, m_Scale);
   DDV_MinMaxDouble(pDX, m_Scale, 1.e-010, 1.e+017);
   DDX_Text(pDX, IDC_GERB_INTEGERS, m_Digits);
   DDV_MinMaxInt(pDX, m_Digits, 0, 2147483647);
   DDX_Check(pDX, IDC_THERMALS, m_thermalMacros);
   DDX_Check(pDX, IDC_FIRE9XXX, m_fire9xxx);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGerbFormat, CDialog)
   //{{AFX_MSG_MAP(CGerbFormat)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGerbFormat message handlers
BOOL CGerbFormat::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_spin1.SetRange(0, 20);
   m_spin2.SetRange(0, 20);
   
   // wolf
   m_Scale = format->Scale;
   m_Type = format->GR.type; // 0-Absolute, 1-Incremental
   m_Zero_Suppression = format->GR.zero; // 0-Leading, 1-Trailing, 2-None
   m_Units = format->GR.units; // 0-English, 1-Metric
   m_Digits = format->GR.digits;
   m_Decimal = format->GR.decimal;
   m_G75_Circle = format->GR.circleMode; // 0=sectorize 1=g75
   m_thermalMacros = format->GR.thermalMacros;
   m_fire9xxx = (format->GR.format ==  2);      // 0-G RS-274; 1-G RS-274-X; 2-FIRE 9XXX 

   UpdateData(FALSE); // update dialog controls
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CGerbFormat::OnOK() 
{
   CDialog::OnOK();

   format->Scale = m_Scale;
   format->GR.type = m_Type; // 0-Absolute, 1-Incremental
   format->GR.zero = m_Zero_Suppression; // 0-Leading, 1-Trailing, 2-None
   format->GR.units = m_Units; // 0-English, 1-Metric
   format->GR.digits = m_Digits;
   format->GR.decimal = m_Decimal;
   format->GR.circleMode = m_G75_Circle; // 0=sectorize 1=g75
   format->GR.thermalMacros = m_thermalMacros;

   if (m_fire9xxx)
      format->GR.format = 2; // Fire 9XXX
   else
      format->GR.format = 1;  // RS-274X

   CpyStr(format->prefix, m_Aperture_Prefix, MAXPREFIX+1);
}

/////////////////////////////////////////////////////////////////////////////
// DXFExportFormat dialog
DXFExportFormat::DXFExportFormat(CWnd* pParent /*=NULL*/)
   : CDialog(DXFExportFormat::IDD, pParent)
{
   //{{AFX_DATA_INIT(DXFExportFormat)
   m_realWidth = 0.010;
   m_roundEnds = FALSE;
   m_scale = 1.0;
   m_units = -1;
   m_explode = 0;
   m_hollow = FALSE;
   m_export_hidden_entities = TRUE;
   m_use_probetype  = TRUE;
   m_text_xscale = FALSE;
   m_text_unmirror = FALSE;
   //}}AFX_DATA_INIT
}


void DXFExportFormat::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DXFExportFormat)
   DDX_Control(pDX, IDC_UNITS, m_Unit_CB);
   DDX_Text(pDX, IDC_REAL_WIDTH, m_realWidth);
   DDX_Check(pDX, IDC_ROUND_ENDS, m_roundEnds);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-043, 1.e+037);
   DDX_CBIndex(pDX, IDC_UNITS, m_units);
   DDX_Radio(pDX, IDC_EXPLODE, m_explode);
   DDX_Check(pDX, IDC_HOLLOW, m_hollow);
   DDX_Check(pDX, IDC_HIDDEN_ENTITIES, m_export_hidden_entities);
   DDX_Check(pDX, IDC_USE_PROBETYPE, m_use_probetype);
   DDX_Check(pDX, IDC_TEXT_XSCALE, m_text_xscale);
   DDX_Check(pDX, IDC_TEXT_UNMIRROR, m_text_unmirror);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DXFExportFormat, CDialog)
   //{{AFX_MSG_MAP(DXFExportFormat)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DXFExportFormat message handlers
BOOL DXFExportFormat::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i < MAX_UNITS; i++)
      m_Unit_CB.AddString(GetUnitName(i));
   m_Unit_CB.SetCurSel(format->DXF.Units);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void DXFExportFormat::OnOK() 
{
   CDialog::OnOK();

   format->DXF.Units = m_Unit_CB.GetCurSel();
}

/////////////////////////////////////////////////////////////////////////////
// GerberExportFormat dialog
GerberExportFormat::GerberExportFormat(CWnd* pParent /*=NULL*/)
   : CDialog(GerberExportFormat::IDD, pParent)
{
   //{{AFX_DATA_INIT(GerberExportFormat)
   m_decimal = 3;
   m_digits = 2;
   m_type = 0;
   m_units = 0;
   m_zero = 0;
   m_274 = 1;
   m_sect_ang = 15;
   m_scale = 1.0;
   m_fillDCode = 0;
   m_polyAreaFill = FALSE;
   m_sectorize = 0;
   m_layerStencilThickness = FALSE;
   //}}AFX_DATA_INIT
}

void GerberExportFormat::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GerberExportFormat)
   DDX_Control(pDX, IDC_SPIN3, m_spin2);
   DDX_Control(pDX, IDC_SPIN2, m_spin1);
   DDX_Text(pDX, IDC_GERB_DECIMAL, m_decimal);
   DDV_MinMaxInt(pDX, m_decimal, 0, 99);
   DDX_Text(pDX, IDC_GERB_INTEGERS, m_digits);
   DDV_MinMaxInt(pDX, m_digits, 0, 99);
   DDX_Radio(pDX, IDC_GERB_TYPE, m_type);
   DDX_Radio(pDX, IDC_GERB_UNITS, m_units);
   DDX_Radio(pDX, IDC_GERB_ZERO_SUPPRESSION, m_zero);
   DDX_Radio(pDX, IDC_274, m_274);
   DDX_Text(pDX, IDC_SECTORIZATION_ANGLE, m_sect_ang);
   DDV_MinMaxDouble(pDX, m_sect_ang, 0.1, 15.);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-010, 9999999999.);
   DDX_Text(pDX, IDC_FILL_DCODE, m_fillDCode);
   DDV_MinMaxInt(pDX, m_fillDCode, 0, 999);
   DDX_Check(pDX, IDC_POLY_AREA_FILL, m_polyAreaFill);
   DDX_Radio(pDX, IDC_SECTORIZE, m_sectorize);
   DDX_Check(pDX, IDC_LAYER_STENCILTHICKNESS, m_layerStencilThickness);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GerberExportFormat, CDialog)
   //{{AFX_MSG_MAP(GerberExportFormat)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GerberExportFormat message handlers

BOOL GerberExportFormat::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_spin1.SetRange(0, 20);
   m_spin2.SetRange(0, 20);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// FormatProtelIn dialog
FormatProtelIn::FormatProtelIn(CWnd* pParent /*=NULL*/)
   : CDialog(FormatProtelIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatProtelIn)
   m_large = _T("");
   m_layer = -1;
   m_medium = _T("");
   m_small = _T("");
   m_smallest = _T("");
   m_scale = 0.0;
   //}}AFX_DATA_INIT
}


void FormatProtelIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatProtelIn)
   DDX_Text(pDX, IDC_LARGE, m_large);
   DDX_Radio(pDX, IDC_LAYER, m_layer);
   DDX_Text(pDX, IDC_MEDIUM, m_medium);
   DDX_Text(pDX, IDC_SMALL, m_small);
   DDX_Text(pDX, IDC_SMALLEST, m_smallest);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-028, 1.e+024);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatProtelIn, CDialog)
   //{{AFX_MSG_MAP(FormatProtelIn)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// FormatPDIFlay dialog
FormatPDIFlay::FormatPDIFlay(CWnd* pParent /*=NULL*/)
   : CDialog(FormatPDIFlay::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatPDIFlay)
   m_scale = 1.0;
   m_TextRot = TRUE;
   //}}AFX_DATA_INIT
}


void FormatPDIFlay::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatPDIFlay)
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-025, 1.e+030);
   DDX_Check(pDX, IDC_TEXT_ROT, m_TextRot);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatPDIFlay, CDialog)
   //{{AFX_MSG_MAP(FormatPDIFlay)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatPDIFlay message handlers
/////////////////////////////////////////////////////////////////////////////
// FormatPDIFsch dialog


FormatPDIFsch::FormatPDIFsch(CWnd* pParent /*=NULL*/)
   : CDialog(FormatPDIFsch::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatPDIFsch)
   m_scale = 1.0;
   //}}AFX_DATA_INIT
}


void FormatPDIFsch::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatPDIFsch)
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-023, 1.e+033);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatPDIFsch, CDialog)
   //{{AFX_MSG_MAP(FormatPDIFsch)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatPDIFsch message handlers

BOOL FormatPDIFlay::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_scale = format->Scale;
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL FormatPDIFsch::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_scale = format->Scale;
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatPDIFsch::OnOK() 
{
   format->Scale = m_scale;
   
   CDialog::OnOK();
}

void FormatPDIFlay::OnOK() 
{
   format->Scale = m_scale;
   format->PDIF_True_TextRot = m_TextRot;
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// TangoExportFormat dialog
TangoExportFormat::TangoExportFormat(CWnd* pParent /*=NULL*/)
   : CDialog(TangoExportFormat::IDD, pParent)
{
   //{{AFX_DATA_INIT(TangoExportFormat)
   m_scale = 0.0;
   //}}AFX_DATA_INIT
}

void TangoExportFormat::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TangoExportFormat)
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-022, 1.e+023);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TangoExportFormat, CDialog)
   //{{AFX_MSG_MAP(TangoExportFormat)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL TangoExportFormat::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   bitmap.AutoLoad(IDC_BITMAP, this);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// ORCADLTDExportFormat dialog
ORCADLTDExportFormat::ORCADLTDExportFormat(CWnd* pParent /*=NULL*/)
   : CDialog(ORCADLTDExportFormat::IDD, pParent)
{
   //{{AFX_DATA_INIT(ORCADLTDExportFormat)
   m_scale = 0.0;
   //}}AFX_DATA_INIT
}

void ORCADLTDExportFormat::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ORCADLTDExportFormat)
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-022, 1.e+023);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ORCADLTDExportFormat, CDialog)
   //{{AFX_MSG_MAP(ORCADLTDExportFormat)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL ORCADLTDExportFormat::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   bitmap.AutoLoad(IDC_BITMAP, this);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// FormatPadsOut dialog


FormatPadsOut::FormatPadsOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatPadsOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatPadsOut)
   m_scale = 0.0;
   m_components = FALSE;
   m_units = -1;
   //}}AFX_DATA_INIT
}


void FormatPadsOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatPadsOut)
   DDX_Text(pDX, IDC_EDIT1, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-021, 1.e+023);
   DDX_Check(pDX, IDC_CHECK1, m_components);
   DDX_Radio(pDX, IDC_UNITS, m_units);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatPadsOut, CDialog)
   //{{AFX_MSG_MAP(FormatPadsOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatPadsOut message handlers
/////////////////////////////////////////////////////////////////////////////
// Format_IGES_Out dialog


Format_IGES_Out::Format_IGES_Out(CWnd* pParent /*=NULL*/)
   : CDialog(Format_IGES_Out::IDD, pParent)
{
   //{{AFX_DATA_INIT(Format_IGES_Out)
   m_explode = 0;
   m_realWidth = 0.010;
   m_roundEnds = FALSE;
   m_scale = 1.0;
   //}}AFX_DATA_INIT
}


void Format_IGES_Out::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Format_IGES_Out)
   DDX_Control(pDX, IDC_UNITS, m_Unit_CB);
   DDX_Radio(pDX, IDC_EXPLODE, m_explode);
   DDX_Text(pDX, IDC_REAL_WIDTH, m_realWidth);
   DDX_Check(pDX, IDC_ROUND_ENDS, m_roundEnds);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-019, 1.e+021);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Format_IGES_Out, CDialog)
   //{{AFX_MSG_MAP(Format_IGES_Out)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Format_IGES_Out message handlers

BOOL Format_IGES_Out::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
	// Case 1671
	// IGES Export does not support all of the camcad units, e.g. "HP Plotter"
   //for (int i=0; i < MAX_UNITS; i++)
   //   m_Unit_CB.AddString(GetUnitName(i));
	m_Unit_CB.AddString("Inches");
	m_Unit_CB.AddString("Mils");
	m_Unit_CB.AddString("mm");

   m_Unit_CB.SetCurSel(0);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void Format_IGES_Out::OnOK() 
{
   UpdateData(FALSE);

   format->RealWidth = m_realWidth;
   format->AddLineEnds = m_roundEnds;
   format->Scale = (m_scale?m_scale:1.0);

	// Case 1671
	CString unitsTxt;
	m_Unit_CB.GetWindowText(unitsTxt);
	format->PortFileUnits = GetUnitIndex(unitsTxt);
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// FormatIPCin dialog
FormatIPCin::FormatIPCin(CWnd* pParent /*=NULL*/)
   : CDialog(FormatIPCin::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatIPCin)
   m_prefix = _T("");
   m_scale = 1.0;
   m_newline = FALSE;
   //}}AFX_DATA_INIT
}

void FormatIPCin::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatIPCin)
   DDX_Control(pDX, IDC_UNITS, m_unitsCB);
   DDX_Text(pDX, IDC_PREFIX, m_prefix);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDX_Check(pDX, IDC_NEWLINE, m_newline);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatIPCin, CDialog)
   //{{AFX_MSG_MAP(FormatIPCin)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL FormatIPCin::OnInitDialog() 
{
   CDialog::OnInitDialog();

   int index;
   
   index = m_unitsCB.AddString(GetUnitName(UNIT_MILS));
   m_unitsCB.SetItemData(index,UNIT_MILS);
   
   index = m_unitsCB.AddString(GetUnitName(UNIT_INCHES));
   m_unitsCB.SetItemData(index,UNIT_INCHES);
   
   index = m_unitsCB.AddString(GetUnitName(UNIT_MM));
   m_unitsCB.SetItemData(index,UNIT_MM);

   m_unitsCB.SetCurSel(0);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatIPCin::OnOK() 
{
   int currentSelection = m_unitsCB.GetCurSel();
   int units;

   if (currentSelection != CB_ERR)
   {
      units = m_unitsCB.GetItemData(currentSelection);
   }
   else
   {
      units = UNIT_MILS;
   }

   format->PortFileUnits = units;

   format->Scale = m_scale;
   CpyStr(format->prefix, m_prefix, MAXPREFIX+1);
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// FormatEdifIn dialog


FormatEdifIn::FormatEdifIn(CWnd* pParent /*=NULL*/)
   : CDialog(FormatEdifIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatEdifIn)
   m_prefix = _T("");
   m_scale = 0.0;
   //}}AFX_DATA_INIT
}


void FormatEdifIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatEdifIn)
   DDX_Control(pDX, IDC_UNITS, m_unitsCB);
   DDX_Text(pDX, IDC_PREFIX, m_prefix);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatEdifIn, CDialog)
   //{{AFX_MSG_MAP(FormatEdifIn)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatEdifIn message handlers

BOOL FormatEdifIn::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i < MAX_UNITS; i++)
      m_unitsCB.AddString(GetUnitName(i));
   m_unitsCB.SetCurSel(0);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatEdifIn::OnOK() 
{
   format->PortFileUnits = m_unitsCB.GetCurSel();
   format->Scale = m_scale;
   CpyStr(format->prefix, m_prefix, MAXPREFIX+1);
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// ProtelLayout dialog
ProtelLayout::ProtelLayout(CWnd* pParent /*=NULL*/)
   : CDialog(ProtelLayout::IDD, pParent)
{
   //{{AFX_DATA_INIT(ProtelLayout)
   m_scale = 0.0;
   m_type = -1;
   //}}AFX_DATA_INIT
}

void ProtelLayout::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ProtelLayout)
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDX_Radio(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ProtelLayout, CDialog)
   //{{AFX_MSG_MAP(ProtelLayout)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProtelLayout message handlers
BOOL ProtelLayout::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   bitmap.AutoLoad(IDC_BITMAP, this);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// ProtelSchematic dialog
ProtelSchematic::ProtelSchematic(CWnd* pParent /*=NULL*/)
   : CDialog(ProtelSchematic::IDD, pParent)
{
   //{{AFX_DATA_INIT(ProtelSchematic)
   m_scale = 0.0;
   //}}AFX_DATA_INIT
}


void ProtelSchematic::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ProtelSchematic)
   DDX_Control(pDX, IDC_UNIT_CB, m_unitsCB);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ProtelSchematic, CDialog)
   //{{AFX_MSG_MAP(ProtelSchematic)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProtelSchematic message handlers

BOOL ProtelSchematic::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i < MAX_UNITS; i++)
      m_unitsCB.AddString(GetUnitName(i));
   m_unitsCB.SetCurSel(0);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ProtelSchematic::OnOK() 
{
   format->PortFileUnits = m_unitsCB.GetCurSel();
   format->Scale = m_scale;
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// FormatAllegroIn dialog
FormatAllegroIn::FormatAllegroIn(CWnd* pParent /*=NULL*/)
   : CDialog(FormatAllegroIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatAllegroIn)
   m_explodeFormat = FALSE;
   m_suppressClass = FALSE;
   m_configFile = _T("");
   m_detected_format = _T("");
   //}}AFX_DATA_INIT
}

void FormatAllegroIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatAllegroIn)
   DDX_Check(pDX, IDC_EXPLODE_FORMAT, m_explodeFormat);
   DDX_Check(pDX, IDC_SUPPRESS_CLASS, m_suppressClass);
   DDX_Text(pDX, IDC_CONFIG_FILE, m_configFile);
   DDX_Text(pDX, IDC_DETECTED_FORMAT, m_detected_format);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatAllegroIn, CDialog)
   //{{AFX_MSG_MAP(FormatAllegroIn)
   ON_BN_CLICKED(IDC_CHANGE_CONFIG_FILE, OnChangeConfigFile)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatAllegroIn message handlers
void FormatAllegroIn::OnChangeConfigFile() 
{
   UpdateData();

   CFileDialog FileDialog(TRUE, "IN", "*.IN",
         OFN_FILEMUSTEXIST, 
         "Config File (*.IN)|*.IN|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   m_configFile = FileDialog.GetPathName();

   UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// FormatGencadOut dialog


FormatGencadOut::FormatGencadOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatGencadOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatGencadOut)
   m_createPanelFile = FALSE;
   m_version = -1;
   //}}AFX_DATA_INIT
}


void FormatGencadOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatGencadOut)
   DDX_Check(pDX, IDC_CREATE_PANEL_FILE, m_createPanelFile);
   DDX_Radio(pDX, IDC_VERSION, m_version);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatGencadOut, CDialog)
   //{{AFX_MSG_MAP(FormatGencadOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatHP3070Out dialog

FormatHP3070Out::FormatHP3070Out(CCEtoODBDoc* document, FormatStruct *format, CWnd* pParent)
	: CResizingDialog(FormatHP3070Out::IDD, pParent)
   , m_writerName("AG 3070 Write")
{
	m_doc = document;
   m_format = format;
	m_ncNet = TRUE;
   //{{AFX_DATA_INIT(FormatHP3070Out)
   m_vias = FALSE;
   m_ruleFile = _T("");
   m_bdf = _T("");
   m_ncf = _T("");
   m_log = _T("");
   m_uPins = FALSE;
   m_delete_3070_attributes = TRUE;
   m_exportVariant = FALSE;
	m_generate_DFT = 0;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_RULE_FILE      ,anchorLeft,growHorizontal);
   addFieldControl(IDC_BOARD_DEFAULTS ,anchorLeft,growHorizontal);
   addFieldControl(IDC_NAME_CHECK_FILE,anchorLeft,growHorizontal);
   addFieldControl(IDC_OUTPUT_LOG     ,anchorLeft,growHorizontal);

   addFieldControl(IDC_CHANGE_RULE_FILE,anchorRight);
   addFieldControl(IDC_CHANGE_BDF      ,anchorRight);
   addFieldControl(IDC_CHANGE_NCF      ,anchorRight);
   addFieldControl(IDC_CHANGE_LOG      ,anchorRight); 

   LoadFormatSettings();
}

void FormatHP3070Out::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatHP3070Out)
   DDX_Check(pDX, IDC_VIAS, m_vias);
   DDX_Text(pDX, IDC_RULE_FILE, m_ruleFile);
   DDX_Text(pDX, IDC_BOARD_DEFAULTS, m_bdf);
   DDX_Text(pDX, IDC_NAME_CHECK_FILE, m_ncf);
   DDX_Text(pDX, IDC_OUTPUT_LOG, m_log);
   DDX_Check(pDX, IDC_U_PINS, m_uPins);
   DDX_Check(pDX, IDC_DELETE_HP3070_ATTRIBUTES, m_delete_3070_attributes);
   DDX_Check(pDX, IDC_ExportVariant, m_exportVariant);
	DDX_Radio(pDX, IDC_RADIO_3070_RULES, m_generate_DFT);	
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_NC_NET, m_ncNet);
}

BEGIN_MESSAGE_MAP(FormatHP3070Out, CResizingDialog)
   //{{AFX_MSG_MAP(FormatHP3070Out)
   ON_BN_CLICKED(IDC_CHANGE_RULE_FILE, OnChangeRuleFile)
   ON_BN_CLICKED(IDC_CHANGE_BDF, OnChangeBdf)
   ON_BN_CLICKED(IDC_CHANGE_NCF, OnChangeNcf)
   ON_BN_CLICKED(IDC_CHANGE_LOG, OnChangeLog)
   //}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_3070_RULES, OnBnClickedRadio3070Rules)
	ON_BN_CLICKED(IDC_RADIO_3070_FROM_DFT, OnBnClickedRadio3070FromDft)
	ON_BN_CLICKED(IDC_DELETE_HP3070_ATTRIBUTES, OnBnClickedDeleteHp3070Attributes)
	ON_BN_CLICKED(IDC_U_PINS, OnBnClickedUPins)
	ON_BN_CLICKED(IDC_VIAS, OnBnClickedVias)
	ON_BN_CLICKED(IDC_NC_NET, OnBnClickedNcNet)
   ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
   ON_BN_CLICKED(IDC_ExportVariant, OnBnClickedExportvariant)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FormatHP3070Out message handlers

//HP3070 oninitdialog method
BOOL FormatHP3070Out::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	if (!HasPcbFileWithDFT())
	{
		GetDlgItem(IDC_RADIO_3070_FROM_DFT)->EnableWindow(FALSE);
      m_generate_DFT = 0; // Override any setting made so far, there is no DFT to use
	}
   else
   {
		GetDlgItem(IDC_RADIO_3070_FROM_DFT)->EnableWindow(TRUE);
      //m_generate_DFT = 1;  Leave setting alone, user may choose to use either DFT or 3070.out when DFT is present
   }

   // Initial it to false
   m_exportVariant = FALSE;
   FileStruct* fileStruct = m_doc->getFileList().GetFirstShown(blockTypeUndefined); //blockTypePcb);
   if (fileStruct != NULL)
   {
      if (fileStruct->getVariantList().GetCount() > 0)
      {
         m_exportVariant = TRUE;
      }
      else if (fileStruct->getBlockType() == blockTypePanel && fileStruct->getBlock() != NULL)
      {
         // Check to see if any board on the panel has variant, if one deos then enable export variant option
         for (POSITION pos=fileStruct->getBlock()->getHeadDataInsertPosition(); pos!=NULL;)
         {
            DataStruct* data = fileStruct->getBlock()->getNextDataInsert(pos);
            if (data != NULL)
            {
               FileStruct* pcbFileStruct = m_doc->getFileList().FindByBlockNumber(data->getInsert()->getBlockNumber());
               if (pcbFileStruct != NULL && pcbFileStruct->getVariantList().GetCount() > 0)
               {
                  m_exportVariant = TRUE;
                  break;
               }
            }
         }
      }
   }
   
   GetDlgItem(IDC_ExportVariant)->EnableWindow(m_exportVariant);

	m_format->IncludeVias = m_vias;
	m_format->IncludeUnconnectedPins = m_uPins;
	m_format->OneNCNet = m_ncNet;
	m_format->Delete3070Attributes = m_delete_3070_attributes;
	m_format->GenerateDFT3070 = m_generate_DFT;
   m_format->ExportVariant = m_exportVariant==TRUE?true:false;

   UpdateData(FALSE);

   return TRUE;
}

void FormatHP3070Out::OnChangeRuleFile() 
{
   CFileDialog FileDialog(TRUE, "OUT", m_ruleFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Rule Files (*.out)|*.out|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_ruleFile = FileDialog.GetPathName();
   UpdateData(FALSE);
}

void FormatHP3070Out::OnChangeBdf() 
{
   CFileDialog FileDialog(TRUE, "BDF", m_bdf,
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Board Default Files (*.bdf)|*.bdf|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_bdf = FileDialog.GetPathName();
   UpdateData(FALSE);
}

void FormatHP3070Out::OnChangeNcf() 
{
   CFileDialog FileDialog(TRUE, "CHK", m_ncf,
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Name Check Files (*.chk)|*.chk|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_ncf = FileDialog.GetPathName();
   UpdateData(FALSE);
}

void FormatHP3070Out::OnChangeLog() 
{
   CFileDialog FileDialog(FALSE, "LOG", m_log, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Output Log (*.log)|*.log|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_log = FileDialog.GetPathName();
   UpdateData(FALSE);
}

void FormatHP3070Out::OnBnClickedRadio3070Rules()
{
	m_format->GenerateDFT3070 = m_generate_DFT = 0;
	UpdateData(FALSE);
}

void FormatHP3070Out::OnBnClickedRadio3070FromDft()
{
	m_format->GenerateDFT3070 = m_generate_DFT = 1;
	UpdateData(FALSE);
}

void FormatHP3070Out::OnBnClickedDeleteHp3070Attributes()
{
	if (((CButton*)GetDlgItem(IDC_DELETE_HP3070_ATTRIBUTES))->GetCheck() == BST_CHECKED)
	{
		m_format->Delete3070Attributes = m_delete_3070_attributes = 1;
		UpdateData(FALSE);
	}
	else
	{
		m_format->Delete3070Attributes = m_delete_3070_attributes = 0;
		UpdateData(FALSE);
	}
}

void FormatHP3070Out::OnBnClickedExportvariant()
{
	if (((CButton*)GetDlgItem(IDC_ExportVariant))->GetCheck() == BST_CHECKED)
	{
		m_format->ExportVariant = true;
      m_exportVariant = 1;
		UpdateData(FALSE);
	}
	else
	{
		m_format->ExportVariant = false;
      m_exportVariant = 0;
		UpdateData(FALSE);
	}
}

void FormatHP3070Out::OnBnClickedUPins()
{
	m_format->IncludeUnconnectedPins = m_uPins = !m_uPins;
	UpdateData(FALSE);
}

void FormatHP3070Out::OnBnClickedVias()
{
	m_format->IncludeVias = m_vias = !m_vias;
	UpdateData(FALSE);
}

void FormatHP3070Out::OnBnClickedNcNet()
{
	m_format->OneNCNet = m_ncNet = !m_ncNet;
	UpdateData(FALSE);
}

bool FormatHP3070Out::HasPcbFileWithDFT()
{
   for (POSITION pos = m_doc->getFileList().GetHeadPosition();pos != NULL;)
   {
      FileStruct *curFile = m_doc->getFileList().GetNext(pos);
      /*if (curFile->getBlockType() == blockTypePcb && 
         m_doc->GetCurrentDFTSolution(*curFile) != NULL &&
         m_doc->GetCurrentDFTSolution(*curFile)->GetTestPlan() != NULL)
      {
         return true;
      }*/
   }

   return false;
}


void FormatHP3070Out::OnBnClickedReset()
{
	SetFormatDefaults();

	UpdateData(FALSE);
}

void FormatHP3070Out::SetFormatDefaults()
{
   m_ruleFile  = getApp().getExportSettingsFilePath("3070.out");
   m_bdf       = getApp().getSystemSettingsFilePath("3070.bdf");
   m_ncf       = getApp().getSystemSettingsFilePath("3070.chk");
   m_log       = GetLogfilePath("3070.log");
   m_vias      = TRUE;
   m_uPins     = TRUE;
   m_ncNet     = TRUE;
   m_delete_3070_attributes = TRUE;
   m_generate_DFT = HasPcbFileWithDFT() ? 1 : 0;
}

void FormatHP3070Out::LoadFormatSettings()
{
   SetFormatDefaults();

   // Load settings if available
   LoadSettingFromRegistry(m_writerName, "Unconnected Pins In One Net", m_ncNet);
   LoadSettingFromRegistry(m_writerName, "Include Vias",                m_vias);
   LoadSettingFromRegistry(m_writerName, "Include Unconnected Pins",    m_uPins);
   LoadSettingFromRegistry(m_writerName, "Recreate 3070 Attributes",    m_delete_3070_attributes);

   CString accessOpt;
   if (LoadSettingFromRegistry(m_writerName, "Accessibility Option", accessOpt))
   {
      if (accessOpt.CompareNoCase("Use DFT") == 0)
         m_generate_DFT = 1;
      else
         m_generate_DFT = 0;
   }

   LoadSettingFromRegistry(m_writerName, "Rule File",           m_ruleFile);
   LoadSettingFromRegistry(m_writerName, "Board Defaults File", m_bdf);
   LoadSettingFromRegistry(m_writerName, "Name Check File",     m_ncf);
   LoadSettingFromRegistry(m_writerName, "Log File",            m_log);
}

void FormatHP3070Out::SaveFormatSettings()
{
   SaveSettingToRegistry(m_writerName, "Unconnected Pins In One Net", m_ncNet);
   SaveSettingToRegistry(m_writerName, "Include Vias",                m_vias);
   SaveSettingToRegistry(m_writerName, "Include Unconnected Pins",    m_uPins);
   SaveSettingToRegistry(m_writerName, "Recreate 3070 Attributes",    m_delete_3070_attributes);

   CString accessOpt("Use DFT"); // m_generate_DFT == 1
   if (m_generate_DFT == 0)
      accessOpt = "Use 3070.out";
   SaveSettingToRegistry(m_writerName, "Accessibility Option", accessOpt);

   SaveSettingToRegistry(m_writerName, "Rule File",           m_ruleFile);
   SaveSettingToRegistry(m_writerName, "Board Defaults File", m_bdf);
   SaveSettingToRegistry(m_writerName, "Name Check File",     m_ncf);
   SaveSettingToRegistry(m_writerName, "Log File",            m_log);
}

/////////////////////////////////////////////////////////////////////////////
// FormatInsertionOut dialog
FormatInsertionOut::FormatInsertionOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatInsertionOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatInsertionOut)
   m_type = -1;
   //}}AFX_DATA_INIT
}

void FormatInsertionOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatInsertionOut)
   DDX_Radio(pDX, IDC_RADIO1, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatInsertionOut, CDialog)
   //{{AFX_MSG_MAP(FormatInsertionOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FormatTestOut dialog
FormatTestOut::FormatTestOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatTestOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatTestOut)
   m_type = -1;
   //}}AFX_DATA_INIT
}

void FormatTestOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatTestOut)
   DDX_Radio(pDX, IDC_RADIO1, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatTestOut, CDialog)
   //{{AFX_MSG_MAP(FormatTestOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcellonIn dialog
CExcellonIn::CExcellonIn(CWnd* pParent /*=NULL*/)
   : CDialog(CExcellonIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(CExcellonIn)
   m_Type = -1;
   m_Units = -1;
   m_Zero_Suppression = -1;
   m_autoAp = FALSE;
   m_format = -1;
   //}}AFX_DATA_INIT
}

void CExcellonIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CExcellonIn)
   DDX_Radio(pDX, IDC_GERB_TYPE, m_Type);
   DDX_Radio(pDX, IDC_GERB_UNITS, m_Units);
   DDX_Radio(pDX, IDC_GERB_ZERO_SUPPRESSION, m_Zero_Suppression);
   DDX_Check(pDX, IDC_AUTO_AP, m_autoAp);
   DDX_Radio(pDX, IDC_FORMAT, m_format);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExcellonIn, CDialog)
   //{{AFX_MSG_MAP(CExcellonIn)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcellonIn message handlers
BOOL CExcellonIn::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_Type   = format->EX.type; // 0-Absolute, 1-Incremental
   m_Zero_Suppression = format->EX.zeroSuppression; // 0-Leading, 1-Trailing, 2-None
   m_Units  = format->EX.units; // 0-English, 1-Metric
   m_autoAp = format->EX.AutoAp;
   m_format = format->EX.format; // 0->Excellon1, 1->Excellon2

   UpdateData(FALSE); // update dialog controls
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CExcellonIn::OnOK() 
{
   CDialog::OnOK();

   format->EX.type = m_Type; // 0-Absolute, 1-Incremental
   format->EX.zeroSuppression = m_Zero_Suppression; // 0-Leading, 1-Trailing, 2-None
	format->EX.includeDecimalPoint = 0; // not actually used on Import
   format->EX.units = m_Units; // 0-English, 1-Metric
   format->EX.AutoAp = m_autoAp;
   format->EX.format = m_format; // 0->Excellon1, 1->Excellon2
}

/////////////////////////////////////////////////////////////////////////////
// FormatExcellonOut dialog
FormatExcellonOut::FormatExcellonOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatExcellonOut::IDD, pParent)
	, m_drillTypes(FALSE)
	, m_usePilotHoles(TRUE)
	, m_probeSurface(FALSE)
	, m_includeDecimalPoint(FALSE)
	, m_zeroSuppression(0)
	, m_includeToolingHoles(FALSE)
{
   //{{AFX_DATA_INIT(FormatExcellonOut)
   m_Units = -1;
   //}}AFX_DATA_INIT
}

void FormatExcellonOut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FormatExcellonOut)
	DDX_Control(pDX, IDC_TOOLS_LB, m_toolsLB);
	DDX_Radio(pDX, IDC_EXCELLON_UNITS_INCHES, m_Units);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_EXCELLON_DRILLS_PCB, m_drillTypes);
	DDX_Radio(pDX, IDC_EXCELLON_PROBESURFACE, m_probeSurface);
	DDX_Check(pDX, IDC_CHECK_EXCELLON_DRILLS_PILOTHOLES, m_usePilotHoles);
	DDX_Check(pDX, IDC_EXCELLON_INCLUDE_DEC_POINT, m_includeDecimalPoint);
	DDX_Radio(pDX, IDC_EXCELLON_ZEROSUPPRESS, m_zeroSuppression);
	DDX_Check(pDX, IDC_INCLUDE_TOOLINGHOLES, m_includeToolingHoles);
}

BEGIN_MESSAGE_MAP(FormatExcellonOut, CDialog)
   //{{AFX_MSG_MAP(FormatExcellonOut)
   //}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_DECIMAL_SPIN, OnDeltaposDecimalSpin)
	ON_BN_CLICKED(IDC_EXCELLON_UNITS_INCHES, OnBnClickedExcellonUnits)
	ON_BN_CLICKED(IDC_EXCELLON_UNITS_METRIC, OnBnClickedExcellonUnits)
	ON_BN_CLICKED(IDC_EXCELLON_DRILLS_PCB, OnBnClickedExcellonDrillsPcb)
	ON_BN_CLICKED(IDC_EXCELLON_DRILLS_PROBE, OnBnClickedExcellonDrillsProbe)
	ON_BN_CLICKED(IDC_EXCELLON_INCLUDE_DEC_POINT, OnBnClickedExcellonIncludeDecPoint)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FormatExcellonOut message handlers
BOOL FormatExcellonOut::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i = 0; i < doc->getNextWidthIndex(); i++)
   {
      BlockStruct *block = doc->getWidthTable()[i];
      if (!block) continue;

      if (!(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL)) continue;

      block->setMarked(FALSE);

      m_toolsLB.SetItemDataPtr(m_toolsLB.AddString(block->getName()), block);
   }
   m_toolsLB.SelItemRange(TRUE, 0, m_toolsLB.GetCount()-1);

   m_Units  = m_format->EX.units; // 0-English, 1-Metric
	m_zeroSuppression = m_format->EX.zeroSuppression;
	m_includeDecimalPoint = m_format->EX.includeDecimalPoint;
	m_drillTypes = m_format->EX.drillTypes;
	m_probeSurface = m_format->EX.probeDrillSurface;
	m_usePilotHoles = m_format->EX.includePilotHoles;
	m_includeToolingHoles = m_format->EX.includeToolingHoles;

#ifdef RESET_DIGITS_TO_DEFAULT
	setDigitFields(m_Units); // will reset to defaults instead of saved values
#else
	CString numStr;
	numStr.Format("%d", m_format->EX.wholeDigits);
	GetDlgItem(IDC_INTEGER_EDIT)->SetWindowText(numStr);
	numStr.Format("%d", m_format->EX.significantDigits);
	GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText(numStr);
#endif

	GetDlgItem(IDC_CHECK_EXCELLON_DRILLS_PILOTHOLES)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE1)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE2)->EnableWindow(m_drillTypes == 1);


   UpdateData(FALSE); // update dialog controls

	OnBnClickedExcellonIncludeDecPoint();  // to enable/disable associated controls

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatExcellonOut::OnOK() 
{
	UpdateData();
   int count = m_toolsLB.GetSelCount();

   if (count)
   {
      int *array = (int*)calloc(count, sizeof(int));
      m_toolsLB.GetSelItems(count, array);

      for (int i=0; i<count; i++)
      {
         BlockStruct *block = (BlockStruct*)m_toolsLB.GetItemDataPtr(array[i]);
         block->setMarked(TRUE);
      }
   }
   
   m_format->EX.type = 0; // 0-Absolute, 1-Incremental
   m_format->EX.zeroSuppression = m_zeroSuppression; // 0-Leading, 1-Trailing, 2-None
	m_format->EX.includeDecimalPoint = m_includeDecimalPoint;  // true/false
   m_format->EX.units = m_Units; // 0-English, 1-Metric
   m_format->EX.format = 1; // 0->Excellon1, 1->Excellon2
   m_format->EX.drillTypes = m_drillTypes; // 0->PCB Drills, 1->Probe Drills
   m_format->EX.probeDrillSurface = m_probeSurface; // Top-0; Bottom-1; Both-2
	m_format->EX.includePilotHoles = m_usePilotHoles;
	m_format->EX.includeToolingHoles = m_includeToolingHoles;
	m_format->EX.includeDecimalPoint = m_includeDecimalPoint;

	CString sWholeDigit, sSignificantDigit;
	GetDlgItem(IDC_INTEGER_EDIT)->GetWindowText(sWholeDigit);
	GetDlgItem(IDC_DECIMAL_EDIT)->GetWindowText(sSignificantDigit);
	m_format->EX.wholeDigits = atoi(sWholeDigit);
   m_format->EX.significantDigits = atoi(sSignificantDigit);

	CDialog::OnOK();
}

void FormatExcellonOut::setDigitFields(int unit)  // 0-English, 1-Metric
{
	if (unit == 0)
	{
		GetDlgItem(IDC_INTEGER_EDIT)->SetWindowText("2");
		GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText("4");
	}
	else
	{
		GetDlgItem(IDC_INTEGER_EDIT)->SetWindowText("3");
		GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText("3");
	}
}

void FormatExcellonOut::OnBnClickedExcellonUnits()
{
	UpdateData();
	setDigitFields(m_Units);
}

void FormatExcellonOut::OnDeltaposDecimalSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

   if (pNMUpDown->iDelta != 0)
   {
		if (m_Units == 0)
		{
			if (pNMUpDown->iDelta < 0)
				GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText("4");
			else
				GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText("3");
		}
		else
		{
			if (pNMUpDown->iDelta < 0)
				GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText("3");
			else
				GetDlgItem(IDC_DECIMAL_EDIT)->SetWindowText("2");
		}
   }

	*pResult = 0;
}

void FormatExcellonOut::OnBnClickedExcellonIncludeDecPoint()
{
	UpdateData(TRUE);

	if (m_includeDecimalPoint)
		m_zeroSuppression = 0; // Leading zero supression automatic when decimal point included

	// Number of digits controls available only in "include decimal point" mode
	GetDlgItem(IDC_INTEGER_EDIT)->EnableWindow(m_includeDecimalPoint);
	GetDlgItem(IDC_DECIMAL_EDIT)->EnableWindow(m_includeDecimalPoint);
	GetDlgItem(IDC_DECIMAL_SPIN)->EnableWindow(m_includeDecimalPoint);
	GetDlgItem(IDC_INTEGERS_LABEL)->EnableWindow(m_includeDecimalPoint);
	GetDlgItem(IDC_DECIMALS_LABEL)->EnableWindow(m_includeDecimalPoint);


	// Zero supression controls available only in "no decimal point" mode
	GetDlgItem(IDC_EXCELLON_ZEROSUPPRESS)->EnableWindow(!m_includeDecimalPoint);
	GetDlgItem(IDC_EXCELLON_ZEROSUPPRESS1)->EnableWindow(!m_includeDecimalPoint);
	GetDlgItem(IDC_EXCELLON_ZEROSUPPRESS2)->EnableWindow(!m_includeDecimalPoint);

   UpdateData(FALSE); // update dialog controls
}

void FormatExcellonOut::OnBnClickedExcellonDrillsPcb()
{
	UpdateData();

	// Enable if we are exporting probe drills
	GetDlgItem(IDC_CHECK_EXCELLON_DRILLS_PILOTHOLES)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE1)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE2)->EnableWindow(m_drillTypes == 1);
}

void FormatExcellonOut::OnBnClickedExcellonDrillsProbe()
{
	UpdateData();

	// Enable if we are exporting probe drills
	GetDlgItem(IDC_CHECK_EXCELLON_DRILLS_PILOTHOLES)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE1)->EnableWindow(m_drillTypes == 1);
	GetDlgItem(IDC_EXCELLON_PROBESURFACE2)->EnableWindow(m_drillTypes == 1);
}

/////////////////////////////////////////////////////////////////////////////
// FormatMentorOut dialog
FormatMentorOut::FormatMentorOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatMentorOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatMentorOut)
   m_units = -1;
   //}}AFX_DATA_INIT
}

void FormatMentorOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatMentorOut)
   DDX_Radio(pDX, IDC_RADIO1, m_units);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatMentorOut, CDialog)
   //{{AFX_MSG_MAP(FormatMentorOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FormatIPCOut dialog
FormatIPCOut::FormatIPCOut(FormatStruct *format, CWnd* pParent /*=NULL*/)
   : CDialog(FormatIPCOut::IDD, pParent)
   , m_format(format)
{
   //{{AFX_DATA_INIT(FormatIPCOut)
   m_radio = 2;
   m_units = pageUnitsMilliMeters;
   //}}AFX_DATA_INIT
}

void FormatIPCOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatIPCOut)
   DDX_Radio(pDX, IDC_RADIO1, m_radio);
   DDX_Control(pDX, IDC_UNITS, m_Unit_CB);
   DDX_CBIndex(pDX, IDC_UNITS, m_units);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatIPCOut, CDialog)
   //{{AFX_MSG_MAP(FormatIPCOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL FormatIPCOut::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (int i = pageUnitsInches; i <= pageUnitsMilliMeters; i++)
      m_Unit_CB.AddString(GetUnitName(i));

   m_radio = 2; // default to IPC356A
   m_units = (m_format != NULL)?m_format->IPCSettings.Units:pageUnitsMilliMeters;
   m_Unit_CB.SetCurSel(m_units);

   UpdateData(FALSE); // update dialog controls

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatIPCOut::OnOK() 
{
   UpdateData(TRUE); // update variable

   if(m_format)
   {
      m_format->IPCSettings.IPC_Format = m_radio;
      m_format->IPCSettings.Units = m_units;
   }

   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// Format_GenCAM_In dialog


Format_GenCAM_In::Format_GenCAM_In(CWnd* pParent /*=NULL*/)
   : CDialog(Format_GenCAM_In::IDD, pParent)
{
   //{{AFX_DATA_INIT(Format_GenCAM_In)
   m_0 = FALSE;
   m_1 = FALSE;
   m_2 = FALSE;
   m_3 = FALSE;
   m_4 = FALSE;
   m_5 = FALSE;
   m_6 = FALSE;
   m_7 = FALSE;
   //}}AFX_DATA_INIT
}


void Format_GenCAM_In::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Format_GenCAM_In)
   DDX_Check(pDX, IDC_CHECK0, m_0);
   DDX_Check(pDX, IDC_CHECK1, m_1);
   DDX_Check(pDX, IDC_CHECK2, m_2);
   DDX_Check(pDX, IDC_CHECK3, m_3);
   DDX_Check(pDX, IDC_CHECK4, m_4);
   DDX_Check(pDX, IDC_CHECK5, m_5);
   DDX_Check(pDX, IDC_CHECK6, m_6);
   DDX_Check(pDX, IDC_CHECK7, m_7);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Format_GenCAM_In, CDialog)
   //{{AFX_MSG_MAP(Format_GenCAM_In)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Format_GenCAM_In message handlers
/////////////////////////////////////////////////////////////////////////////
// Format_GenCAM_Out dialog


Format_GenCAM_Out::Format_GenCAM_Out(CWnd* pParent /*=NULL*/)
   : CDialog(Format_GenCAM_Out::IDD, pParent)
{
   //{{AFX_DATA_INIT(Format_GenCAM_Out)
   m_0 = FALSE;
   m_1 = FALSE;
   m_2 = FALSE;
   m_3 = FALSE;
   m_4 = FALSE;
   m_5 = FALSE;
   m_6 = FALSE;
   m_7 = FALSE;
   //}}AFX_DATA_INIT
}


void Format_GenCAM_Out::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Format_GenCAM_Out)
   DDX_Check(pDX, IDC_CHECK0, m_0);
   DDX_Check(pDX, IDC_CHECK1, m_1);
   DDX_Check(pDX, IDC_CHECK2, m_2);
   DDX_Check(pDX, IDC_CHECK3, m_3);
   DDX_Check(pDX, IDC_CHECK4, m_4);
   DDX_Check(pDX, IDC_CHECK5, m_5);
   DDX_Check(pDX, IDC_CHECK6, m_6);
   DDX_Check(pDX, IDC_CHECK7, m_7);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Format_GenCAM_Out, CDialog)
   //{{AFX_MSG_MAP(Format_GenCAM_Out)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Format_GenCAM_Out message handlers
/////////////////////////////////////////////////////////////////////////////
// FormatHughesIn dialog
FormatHughesIn::FormatHughesIn(CWnd* pParent /*=NULL*/)
   : CDialog(FormatHughesIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatHughesIn)
   m_file1 = _T("");
   m_file2 = _T("");
   m_side1 = 0;
   m_side2 = 1;
   //}}AFX_DATA_INIT
}

void FormatHughesIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatHughesIn)
   DDX_Text(pDX, IDC_FILE1, m_file1);
   DDX_Text(pDX, IDC_FILE2, m_file2);
   DDX_Radio(pDX, IDC_TOP1, m_side1);
   DDX_Radio(pDX, IDC_TOP2, m_side2);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatHughesIn, CDialog)
   //{{AFX_MSG_MAP(FormatHughesIn)
   ON_BN_CLICKED(IDC_TOP1, OnTop1)
   ON_BN_CLICKED(IDC_TOP2, OnTop2)
   ON_BN_CLICKED(IDC_BOTTOM1, OnBottom1)
   ON_BN_CLICKED(IDC_BOTTOM2, OnBottom2)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatHughesIn message handlers
BOOL FormatHughesIn::OnInitDialog() 
{
   CDialog::OnInitDialog();

   if (!twoFiles)
   {
      GetDlgItem(IDC_FILE2GROUP)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_FILE2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TOP2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_BOTTOM2)->ShowWindow(SW_HIDE);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatHughesIn::OnTop1() 
{
   UpdateData();
   if (twoFiles)
      m_side2 = 1;
   UpdateData(FALSE);   
}

void FormatHughesIn::OnBottom1() 
{
   UpdateData();
   if (twoFiles)
      m_side2 = 0;
   UpdateData(FALSE);   
}

void FormatHughesIn::OnTop2() 
{
   UpdateData();
   m_side1 = 1;
   UpdateData(FALSE);   
}

void FormatHughesIn::OnBottom2() 
{
   UpdateData();
   m_side1 = 0;
   UpdateData(FALSE);   
}

/////////////////////////////////////////////////////////////////////////////
// FormatAllegroOut dialog
FormatAllegroOut::FormatAllegroOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatAllegroOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatAllegroOut)
   m_check = _T("");
   m_out = _T("");
   //}}AFX_DATA_INIT
}

void FormatAllegroOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatAllegroOut)
   DDX_Text(pDX, IDC_NAME_CHECK_FILE, m_check);
   DDX_Text(pDX, IDC_OUTPUT_LOG, m_out);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatAllegroOut, CDialog)
   //{{AFX_MSG_MAP(FormatAllegroOut)
   ON_BN_CLICKED(IDC_CHANGE_LOG, OnChangeLog)
   ON_BN_CLICKED(IDC_CHANGE_NCF, OnChangeNcf)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void FormatAllegroOut::OnChangeLog() 
{
   CFileDialog FileDialog(TRUE, "OUT", m_out, OFN_FILEMUSTEXIST, 
         "Rule File (*.out)|*.out|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_out = FileDialog.GetPathName();
   UpdateData(FALSE);
}

void FormatAllegroOut::OnChangeNcf() 
{
   CFileDialog FileDialog(TRUE, "CHK", m_check, OFN_FILEMUSTEXIST, 
         "Name Check Files (*.chk)|*.chk|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_check = FileDialog.GetPathName();
   UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// FormatVB99Out dialog
FormatVB99Out::FormatVB99Out(CWnd* pParent /*=NULL*/)
   : CDialog(FormatVB99Out::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatVB99Out)
   m_ncf = _T("");
   m_log = _T("");
   //}}AFX_DATA_INIT
}

void FormatVB99Out::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatVB99Out)
   DDX_Text(pDX, IDC_NAME_CHECK_FILE, m_ncf);
   DDX_Text(pDX, IDC_OUTPUT_LOG, m_log);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatVB99Out, CDialog)
   //{{AFX_MSG_MAP(FormatVB99Out)
   ON_BN_CLICKED(IDC_CHANGE_LOG, OnChangeLog)
   ON_BN_CLICKED(IDC_CHANGE_NCF, OnChangeNcf)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void FormatVB99Out::OnChangeLog() 
{
   CFileDialog FileDialog(TRUE, "OUT", m_log, OFN_FILEMUSTEXIST, 
         "Rule File (*.out)|*.out|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_log = FileDialog.GetPathName();
   UpdateData(FALSE);
}

void FormatVB99Out::OnChangeNcf() 
{
   CFileDialog FileDialog(TRUE, "CHK", m_ncf, OFN_FILEMUSTEXIST, 
         "Name Check Files (*.chk)|*.chk|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_ncf = FileDialog.GetPathName();
   UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// FormatIgesIn dialog
FormatIgesIn::FormatIgesIn(CWnd* pParent /*=NULL*/)
   : CDialog(FormatIgesIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatIgesIn)
   m_fill = FALSE;
   m_scale = 0.0;
   m_units = 0;
   //}}AFX_DATA_INIT
}

void FormatIgesIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatIgesIn)
   DDX_Control(pDX, IDC_UNITS, m_unitCB);
   DDX_Check(pDX, IDC_FILL_POLY, m_fill);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDX_CBIndex(pDX, IDC_UNITS, m_units);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatIgesIn, CDialog)
   //{{AFX_MSG_MAP(FormatIgesIn)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatIgesIn message handlers
BOOL FormatIgesIn::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i < MAX_UNITS; i++)
      m_unitCB.AddString(GetUnitName(i));
   m_unitCB.SetCurSel(m_units);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// ODB_dlg dialog
ODB_dlg::ODB_dlg(CWnd* pParent /*=NULL*/)
   : CDialog(ODB_dlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(ODB_dlg)
   m_read = -1;
   //}}AFX_DATA_INIT
}


void ODB_dlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ODB_dlg)
   DDX_Radio(pDX, IDC_RADIO1, m_read);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ODB_dlg, CDialog)
   //{{AFX_MSG_MAP(ODB_dlg)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ODB_dlg message handlers
/////////////////////////////////////////////////////////////////////////////
// FormatDMISOut dialog


FormatDMISOut::FormatDMISOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatDMISOut::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatDMISOut)
   m_realWidth = _T("");
   //}}AFX_DATA_INIT
}


void FormatDMISOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatDMISOut)
   DDX_Text(pDX, IDC_REAL_WIDTH, m_realWidth);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatDMISOut, CDialog)
   //{{AFX_MSG_MAP(FormatDMISOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatDMISOut message handlers


/////////////////////////////////////////////////////////////////////////////
// FormatTakayaOut1 dialog
FormatTakayaOut1::FormatTakayaOut1(CWnd* pParent /*=NULL*/)
   : CDialog(FormatTakayaOut1::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatTakayaOut1)
   m_bottommountedcomps = FALSE;
   m_bottomsurfaceallow = FALSE;
   m_maxhitcount = 0;
   m_preferedsurface = -1;
   m_testtypecapacitors = FALSE;
   m_testtypediodes = FALSE;
   m_typetestinductors = FALSE;
   m_testtyperesistor = FALSE;
   m_testtypeshorts = FALSE;
   m_testtypetransistors = FALSE;
   m_testtypezener = FALSE;
   m_topmountedcomps = FALSE;
   m_topsurfaceallow = FALSE;
   m_filenamePrefix = _T("");
   //}}AFX_DATA_INIT
   m_extension = ".ca8";
   m_IC_diode = FALSE;
   m_IC_capacitance = FALSE;
}

void FormatTakayaOut1::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatTakayaOut1)
   DDX_Check(pDX, IDC_BOTTOM_MOUNTED_COMPS, m_bottommountedcomps);
   DDX_Check(pDX, IDC_BOTTOMSURFACE_ALLOW, m_bottomsurfaceallow);
   DDX_Text(pDX, IDC_MAX_HIT_COUNT, m_maxhitcount);
   DDX_Radio(pDX, IDC_PREFERED_SURFACE, m_preferedsurface);
   DDX_Check(pDX, IDC_TESTTYPE_CAPACITORS, m_testtypecapacitors);
   DDX_Check(pDX, IDC_TESTTYPE_DIODES, m_testtypediodes);
   DDX_Check(pDX, IDC_TESTTYPE_INDUCTORS, m_typetestinductors);
   DDX_Check(pDX, IDC_TESTTYPE_RESISTORS, m_testtyperesistor);
   DDX_Check(pDX, IDC_TESTTYPE_SHORTS, m_testtypeshorts);
   DDX_Check(pDX, IDC_TESTTYPE_TRANSISTORS, m_testtypetransistors);
   DDX_Check(pDX, IDC_TESTTYPE_ZENERS, m_testtypezener);
   DDX_Check(pDX, IDC_TOP_MOUNTED_COMPS, m_topmountedcomps);
   DDX_Check(pDX, IDC_TOPSURFACE_ALLOW, m_topsurfaceallow);
   DDX_Text(pDX, IDC_FILENAME_PREFIX, m_filenamePrefix);
   //}}AFX_DATA_MAP
   DDX_Text(pDX, IDC_EXTENSION, m_extension);
   DDX_Check(pDX, IDC_IC_DIODE, m_IC_diode);
   DDX_Check(pDX, IDC_IC_CAPACITANCE, m_IC_capacitance);
}


BEGIN_MESSAGE_MAP(FormatTakayaOut1, CDialog)
   //{{AFX_MSG_MAP(FormatTakayaOut1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatTakayaOut1 message handlers
BOOL FormatTakayaOut1::OnInitDialog() 
{
   CDialog::OnInitDialog();

   if (CA9)
      m_extension = ".ca9";

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatTakayaOut1::OnOK() 
{
   UpdateData();

   if (m_filenamePrefix.IsEmpty())
   {
      ErrorMessage("Filename Prefix is required", "Filename Prefix");
      return;
   }
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// FormatCADIFIn dialog
FormatCADIFIn::FormatCADIFIn(CWnd* pParent /*=NULL*/)
   : CDialog(FormatCADIFIn::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatCADIFIn)
   m_configFile = _T("");
   //}}AFX_DATA_INIT
}

void FormatCADIFIn::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatCADIFIn)
   DDX_Text(pDX, IDC_CONFIG_FILE, m_configFile);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatCADIFIn, CDialog)
   //{{AFX_MSG_MAP(FormatCADIFIn)
   ON_BN_CLICKED(IDC_CHANGE_CONFIG_FILE, OnChangeConfigFile)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void FormatCADIFIn::OnChangeConfigFile() 
{
   UpdateData();

   CFileDialog FileDialog(TRUE, "IN", "*.IN",
         OFN_FILEMUSTEXIST, 
         "Config File (*.IN)|*.IN|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   m_configFile = FileDialog.GetPathName();

   UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// FormatTRIExport dialog


FormatTRIExport::FormatTRIExport(CWnd* pParent /*=NULL*/)
   : CDialog(FormatTRIExport::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatTRIExport)
   m_bottommountedcomps = FALSE;
   m_testtypecapacitors = FALSE;
   m_testtypediodes = FALSE;
   m_testtypeinductors = FALSE;
   m_testtypejumpers = FALSE;
   m_testtypephotocouplers = FALSE;
   m_testtyperesitors = FALSE;
   m_testtypesrc_triac = FALSE;
   m_testtypetransistors = FALSE;
   m_testtypetransistorshfe = FALSE;
   m_testtypeunknown = FALSE;
   m_topmountedcomps = FALSE;
   m_testtypecrystal = FALSE;
   m_testtypemosfet_jfet = FALSE;
   m_filename = _T("");
   m_testtypeIC = FALSE;
   //}}AFX_DATA_INIT
	m_datformat = 0;
}


void FormatTRIExport::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatTRIExport)
   DDX_Check(pDX, IDC_BOTTOM_MOUNTED_COMPS, m_bottommountedcomps);
   DDX_Check(pDX, IDC_TESTTYPE_CAPACITORS, m_testtypecapacitors);
   DDX_Check(pDX, IDC_TESTTYPE_DIODES, m_testtypediodes);
   DDX_Check(pDX, IDC_TESTTYPE_INDUCTORS, m_testtypeinductors);
   DDX_Check(pDX, IDC_TESTTYPE_JUMPERS, m_testtypejumpers);
   DDX_Check(pDX, IDC_TESTTYPE_PHOTO_COUPLERS, m_testtypephotocouplers);
   DDX_Check(pDX, IDC_TESTTYPE_RESISTORS, m_testtyperesitors);
   DDX_Check(pDX, IDC_TESTTYPE_SCR_TRIAC, m_testtypesrc_triac);
   DDX_Check(pDX, IDC_TESTTYPE_TRANSISTORS, m_testtypetransistors);
   DDX_Check(pDX, IDC_TESTTYPE_TRANSISTORS_HFE, m_testtypetransistorshfe);
   DDX_Check(pDX, IDC_TESTTYPE_UNKNOWN, m_testtypeunknown);
   DDX_Check(pDX, IDC_TOP_MOUNTED_COMPS, m_topmountedcomps);
   DDX_Check(pDX, IDC_TESTTYPE_CRYSTAL, m_testtypecrystal);
   DDX_Check(pDX, IDC_TESTTYPE_MOSFET_JFET, m_testtypemosfet_jfet);
   DDX_Text(pDX, IDC_FILENAME, m_filename);
   DDX_Check(pDX, IDC_TESTTYPE_IC, m_testtypeIC);
	DDX_Radio(pDX, IDC_RADIO_FMT_MDA, m_datformat);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FormatTRIExport, CDialog)
   //{{AFX_MSG_MAP(FormatTRIExport)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatTRIExport message handlers

/////////////////////////////////////////////////////////////////////////////
// FormatTeradyneAoi dialog
FormatTeradyneAoi::FormatTeradyneAoi(CWnd* pParent /*=NULL*/)
   : CDialog(FormatTeradyneAoi::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatTeradyneAoi)
   m_fiducials = FALSE;
   //}}AFX_DATA_INIT
}

void FormatTeradyneAoi::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatTeradyneAoi)
   DDX_Check(pDX, IDC_FIDUCIALS, m_fiducials);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatTeradyneAoi, CDialog)
   //{{AFX_MSG_MAP(FormatTeradyneAoi)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FormatAgilentAOIOut dialog
FormatAgilentAOIOut::FormatAgilentAOIOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatAgilentAOIOut::IDD, pParent)
   , m_useZeroRotationForComplexApertures(FALSE)
{
   //{{AFX_DATA_INIT(FormatAgilentAOIOut)
   m_smd_only = FALSE;
   m_machine_type = -1;
   //}}AFX_DATA_INIT
}

void FormatAgilentAOIOut::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatAgilentAOIOut)
   DDX_Check(pDX, IDC_EXPORT_SMD_ONLY, m_smd_only);
   DDX_Radio(pDX, IDC_MACHINE_TYPE, m_machine_type);
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_UseZeroRotation, m_useZeroRotationForComplexApertures);
}

AoiMachineTag FormatAgilentAOIOut::getMachineType() const
{
   AoiMachineTag machineType = AoiMachineUndefined;

   switch (m_machine_type)
   {
   case 0:  machineType = AoiMachineSj;     break;
   case 1:  machineType = AoiMachineSp;     break;
   case 2:  machineType = AoiMachineSp50;   break;
   case 3:  machineType = AoiMachineFx;     break;
   case 4:  machineType = AoiMachineVia;    break;
   case 5:  machineType = AoiMachineMixed;  break;
   }

   return machineType;
}

BEGIN_MESSAGE_MAP(FormatAgilentAOIOut, CDialog)
   //{{AFX_MSG_MAP(FormatAgilentAOIOut)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FormatSiemensQDOut dialog
IMPLEMENT_DYNAMIC(FormatSiemensQDOut, CDialog)
FormatSiemensQDOut::FormatSiemensQDOut(CWnd* pParent /*=NULL*/)
   : CDialog(FormatSiemensQDOut::IDD, pParent)
   , m_glueTop(FALSE)
   , m_glueBot(FALSE)
   , m_fileName(_T(""))
   , m_boardFidNum(0)
   , m_panelFidNum(0)
   , m_panelThickness(_T(""))
   , m_boardThickness(_T(""))
	, m_boardPartNumber(_T(""))
   , m_boardTolerance(_T(""))
   , m_panelTolerance(_T(""))
{
}

FormatSiemensQDOut::~FormatSiemensQDOut()
{
}

void FormatSiemensQDOut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_GLUE_TOP, m_glueTop);
	DDX_Check(pDX, IDC_GLUE_BOT, m_glueBot);
	DDX_Text(pDX, IDC_FILENAME, m_fileName);
	DDX_Text(pDX, IDC_BoardFidNum, m_boardFidNum);
	DDX_Text(pDX, IDC_PanelFidNum, m_panelFidNum);
	DDX_Text(pDX, IDC_PANEL_THICKNESS, m_panelThickness);
	DDX_Text(pDX, IDC_BOARD_THICKNESS, m_boardThickness);
	DDX_Text(pDX, IDC_PANEL_TOLERANCE, m_panelTolerance);
	DDX_Text(pDX, IDC_BOARD_TOLERANCE, m_boardTolerance);
	DDX_Text(pDX, IDC_BOARD_PARTNUMBER, m_boardPartNumber);
}

BOOL FormatSiemensQDOut::OnInitDialog()
{
   CDialog::OnInitDialog();

   // TODO:  Add extra initialization here
   GetDlgItem(IDC_FILENAME)->EnableWindow(m_enableFileName?TRUE:FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(FormatSiemensQDOut, CDialog)
   ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

BOOL IsInteger(CString value)
{
   int count = 0;
   while (count < (int)strlen(value) )
   {
      if (isalpha(value[count]) || value[count] == '.')
         return FALSE;
      count ++;
   }

   return TRUE;
}

void FormatSiemensQDOut::OnOK()
{
   if (!UpdateData())
      return;

   if (m_enableFileName && m_fileName.IsEmpty())
   {
      ErrorMessage("Please enter a file name", "No File Name");
      return;
   }
   else if (!IsInteger(m_boardThickness))
   {
      ErrorMessage("Please enter a whole number for Board Thickness");
      return;
   }
   else if (!IsInteger(m_panelThickness))
   {
      ErrorMessage("Please enter a whole number for Panel Thickness");
      return;
   }
   else if (!IsInteger(m_boardTolerance))
   {
      ErrorMessage("Please enter a whole number for Board Tolerance");
      return;
   }
   else if (!IsInteger(m_panelTolerance))
   {
      ErrorMessage("Please enter a whole number for Panel Tolerance");
      return;
   }

   CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// FormatPCBFileDlg dialog
IMPLEMENT_DYNAMIC(FormatPCBFileDlg, CDialog)
FormatPCBFileDlg::FormatPCBFileDlg(CWnd* pParent /*=NULL*/)
   : CDialog(FormatPCBFileDlg::IDD, pParent)
{
}

FormatPCBFileDlg::~FormatPCBFileDlg()
{
}

void FormatPCBFileDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(FormatPCBFileDlg, CDialog)
END_MESSAGE_MAP()

BOOL FormatPCBFileDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      if (file->getCadSourceFormat() != Type_Mentor_Layout && file->getCadSourceFormat() != Type_Mentor_Neutral_Layout)
         continue;

      m_list.SetItemDataPtr(m_list.AddString(file->getName()), file);
   }

   m_list.SetCurSel(0);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

int FormatPCBFileDlg::LegalFileCount()
{
   int count = 0;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      if (file->getCadSourceFormat() != Type_Mentor_Layout && file->getCadSourceFormat() != Type_Mentor_Neutral_Layout)
         continue;

      count++;
      pcbFile = file;
   }

   return count;
}

void FormatPCBFileDlg::OnOK() 
{
   CDialog::OnOK();

   int sel = m_list.GetCurSel();

   if (sel == LB_ERR)
      return;

   pcbFile = (FileStruct*)m_list.GetItemDataPtr(sel);
}

//_____________________________________________________________________________
CMvpAoiSettings::CMvpAoiSettings()
{
   m_exportSmdComponentsFlag = true;
   m_exportThComponentsFlag  = true;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CFormatMvpAoiCompOutDialog, CDialog)

CFormatMvpAoiCompOutDialog::CFormatMvpAoiCompOutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFormatMvpAoiCompOutDialog::IDD, pParent)
   , m_smdComponentsFlag(FALSE)
   , m_thComponentsFlag(FALSE)
	, m_optComponentFieldSix(FIELD_USE_PACKAGE)
{
}

CFormatMvpAoiCompOutDialog::~CFormatMvpAoiCompOutDialog()
{
}

void CFormatMvpAoiCompOutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SmdComponents, m_smdComponentsFlag);
	DDX_Check(pDX, IDC_ThComponents, m_thComponentsFlag);
	DDX_Radio(pDX, IDC_RADIO_PACKAGE, m_optComponentFieldSix);
}

void CFormatMvpAoiCompOutDialog::setSettings(const CMvpAoiSettings& mvpAoiSettings)
{
   m_smdComponentsFlag = mvpAoiSettings.getExportSmdComponentsFlag();
   m_thComponentsFlag  = mvpAoiSettings.getExportThComponentsFlag();

	if (mvpAoiSettings.getCompFieldSixUsage() == FIELD_USE_PARTNUMBER)
		m_optComponentFieldSix = FIELD_USE_PARTNUMBER;
	else
		m_optComponentFieldSix = FIELD_USE_PACKAGE;
}

void CFormatMvpAoiCompOutDialog::getSettings(CMvpAoiSettings& mvpAoiSettings)
{
   mvpAoiSettings.setExportSmdComponentsFlag(m_smdComponentsFlag != 0);
   mvpAoiSettings.setExportThComponentsFlag(m_thComponentsFlag != 0);
	mvpAoiSettings.setCompFieldSixUsage((int)m_optComponentFieldSix);
}

BEGIN_MESSAGE_MAP(CFormatMvpAoiCompOutDialog, CDialog)
   ON_BN_CLICKED(IDC_SmdComponents, OnBnClickedSmdComponents)
   ON_BN_CLICKED(IDC_ThComponents, OnBnClickedThComponents)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CFormatMvpAoiCompOutDialog message handlers

BOOL CFormatMvpAoiCompOutDialog::OnInitDialog()
{
   BOOL retval = CDialog::OnInitDialog();

   UpdateData(false);

   return retval;
}

void CFormatMvpAoiCompOutDialog::OnBnClickedSmdComponents()
{
   UpdateData(true);

   if (!m_smdComponentsFlag)
   {
      m_thComponentsFlag = true;
   }

   UpdateData(false);
}

void CFormatMvpAoiCompOutDialog::OnBnClickedThComponents()
{
   UpdateData(true);

   if (!m_thComponentsFlag)
   {
      m_smdComponentsFlag = true;
   }

   UpdateData(false);
}

void CFormatMvpAoiCompOutDialog::OnBnClickedOk()
{
   UpdateData(true);

	OnOK();
}




