// $Header: /CAMCAD/4.3/Ogp.h 10    8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.
*/           
 
#pragma once

struct RTN_struct
{
   char report[51], feature[51];
   int bounds, extents, nearest, first, last, firstOf2, lastOf2, 
         contrast, weakStrong, roughSmooth, percentFeature, percentCoverage,
         profile, surface, ring,
         fPrint, fStats, xPrint, xStats, yPrint, yStats, zPrint, zStats,
         zoomCounts, threshold, multiple, filter, illumination;
   double fUpper, fLower, xUpper, xLower, yUpper, yLower, zUpper, zLower, zLoc, FOVinches;
   BOOL IsCentroid, Multiple, Fill, TouchBoundary;
};

// Tolerance and Output
#define Tol_ReportText           0
#define Tol_FeatureText          1
#define Tol_fUpper               2
#define Tol_fLower               3
#define Tol_xUpper               4
#define Tol_xLower               5
#define Tol_yUpper               6 
#define Tol_yLower               7 
#define Tol_zUpper               8 
#define Tol_zLower               9 
#define Tol_zLoc                 10
#define Tol_fPrint               11
#define Tol_fStats               12
#define Tol_xPrint               13
#define Tol_xStats               14
#define Tol_yPrint               15
#define Tol_yStats               16
#define Tol_zPrint               17
#define Tol_zStats               18

// Edge Detection
#define Edge_Bounds              19 
#define Edge_Extents             20
#define Edge_NearestNominal      21
#define Edge_First               22
#define Edge_Last                23 
#define Edge_FirstOf2            24
#define Edge_LastOf2             25 
#define Edge_Contrast            26
#define Edge_WeakStrong          27
#define Edge_RoughSmooth         28
#define Edge_PercentFeature      29
#define Edge_PercentCoverage     30

// Lights
#define Light_Profile            31
#define Light_Surface            32
#define Light_Ring               33

// FOV
#define FOV_Inches               34
#define FOV_ZoomCounts           35

// Point Type
#define PointType                36

// Centroid
#define Centroid_IsCentroid      37
#define Centroid_Threshold       38
#define Centroid_MultipleValue   39
#define Centroid_Filter          40
#define Centroid_MultipleOn      41
#define Centroid_Fill            42
#define Centroid_TouchBoundary   43
#define Centroid_Illumination    44

#define MAX_FEATURE_ATT          44

// Tolerance and Output
#define ATT_Tol_ReportText          "Report Text"
#define ATT_Tol_FeatureText         "Feature Text"
#define ATT_Tol_fUpper              "Feature Upper Tol"
#define ATT_Tol_fLower              "Feature Lower Tol"
#define ATT_Tol_fPrint              "Feature Print"
#define ATT_Tol_fStats              "Feature Stats"
#define ATT_Tol_xUpper              "X Upper Tol"
#define ATT_Tol_xLower              "X Lower Tol"
#define ATT_Tol_xPrint              "X Print"
#define ATT_Tol_xStats              "X Stats"
#define ATT_Tol_yUpper              "Y Upper Tol"
#define ATT_Tol_yLower              "Y Lower Tol"
#define ATT_Tol_yPrint              "Y Print"
#define ATT_Tol_yStats              "Y Stats"
#define ATT_Tol_zUpper              "Z Upper Tol"
#define ATT_Tol_zLower              "Z Lower Tol"
#define ATT_Tol_zPrint              "Z Print"
#define ATT_Tol_zStats              "Z Stats"
#define ATT_Tol_zLoc                "Z Loc"

// Edge Detection
#define ATT_Edge_Bounds             "Bounds"
#define ATT_Edge_Extents            "Extents"
#define ATT_Edge_NearestNominal     "Nearest Nominal"
#define ATT_Edge_First              "First Edge"
#define ATT_Edge_Last               "Last Edge"
#define ATT_Edge_FirstOf2           "First Of 2"
#define ATT_Edge_LastOf2            "Last Of 2"    
#define ATT_Edge_Contrast           "Contrast"
#define ATT_Edge_WeakStrong         "Edge Weak/Strong"
#define ATT_Edge_RoughSmooth        "Rough/Smooth"
#define ATT_Edge_PercentFeature     "Percent of Feature"
#define ATT_Edge_PercentCoverage    "Percent Coverage"

// Lights
#define ATT_Light_Profile           "Profile"
#define ATT_Light_Surface           "Surface"
#define ATT_Light_Ring              "Ring"

// FOV
#define ATT_FOV_Inches              "FOV Inches"
#define ATT_FOV_ZoomCounts          "FOV Zoom Counts"

// Point Type
#define ATT_PointType               "Point Type"

#define PT_STRONG_EDGE           "Strong Edge (uni-directional)"
#define PT_STRONG_EDGE_RIGHT     "Strong Edge Right"
#define PT_STRONG_EDGE_LEFT      "Strong Edge Left"
#define PT_STRONG_EDGE_UP        "Strong Edge Up"
#define PT_STRONG_EDGE_DOWN      "Strong Edge Down"
#define PT_STRONG_EDGE_LOCATE    "Locate"

#define PT_FOCUS_TOP_DOWN        "Top Down Focus"
#define PT_FOCUS_BOTTOM_UP       "Bottom Up Focus"
#define PT_FOCUS_M_C_FOCUS       "Max Contrast Focus"
#define PT_FOCUS_M_C_GRID        "Max Constrast Grid"

// Centroid
#define ATT_Centroid_IsCentroid     "IsCentroid"
#define ATT_Centroid_Threshold      "Threshold"
#define ATT_Centroid_MultipleValue  "MultipleValue"
#define ATT_Centroid_Filter         "Filter"
#define ATT_Centroid_MultipleOn     "MultipleOn"
#define ATT_Centroid_Fill           "Fill"
#define ATT_Centroid_TouchBoundary  "TouchBoundary"
#define ATT_Centroid_Illumination   "Illumination"



/////////////////////////////////////////////////////////////////////////////
// OGP_FOV dialog
class OGP_FOV : public CDialog
{
// Construction
public:
   double inches;
   int zoomCounts;
   double opticalMag;
   OGP_FOV(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OGP_FOV)
   enum { IDD = IDD_OGP_FOV };
   CScrollBar  m_zoomMagSC;
   BOOL  m_fixed;
   CString  m_opticalMag;
   int      m_zoomCounts;
   int      m_frontLens;
   int      m_extTube;
   CString  m_FOV_Size;
   CString  m_zoomCount;
   //}}AFX_DATA
   int scrollPos;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OGP_FOV)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(OGP_FOV)
   virtual BOOL OnInitDialog();
   afx_msg void Calculate();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual void OnOK();
   afx_msg void OnSaveDefaults();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// OGP_Lights dialog
class OGP_Lights : public CDialog
{
// Construction
public:
   OGP_Lights(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OGP_Lights)
   enum { IDD = IDD_OGP_LIGHTS };
   CScrollBar  m_surfaceSC;
   CScrollBar  m_ringSC;
   CScrollBar  m_profileSC;
   CString  m_ring;
   CString  m_surface;
   CString  m_profile;
   //}}AFX_DATA
   int ring;
   int surface;
   int profile;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OGP_Lights)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void GetData();
   void PutData();

   // Generated message map functions
   //{{AFX_MSG(OGP_Lights)
   virtual BOOL OnInitDialog();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual void OnOK();
   afx_msg void OnSaveDefaults();
   afx_msg void OnLoadDefaults();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// OGP_Edge dialog
class OGP_Edge : public CDialog
{
// Construction
public:
   OGP_Edge(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OGP_Edge)
   enum { IDD = IDD_OGP_EDGE };
   CScrollBar  m_percentCoverageSC;
   CScrollBar  m_percentFeatureSC;
   CScrollBar  m_contrastSC;
   CScrollBar  m_weakStrongSC;
   CScrollBar  m_roughSmoothSC;
   CScrollBar  m_nearestSC;
   CScrollBar  m_lastSC;
   CScrollBar  m_lastOf2SC;
   CScrollBar  m_firstOf2SC;
   CScrollBar  m_firstSC;
   CScrollBar  m_extentsSC;
   CScrollBar  m_boundsSC;
   CString  m_bounds;
   CString  m_contrast;
   CString  m_extents;
   CString  m_first;
   CString  m_firstOf2;
   CString  m_last;
   CString  m_lastOf2;
   CString  m_nearest;
   CString  m_roughSmooth;
   CString  m_weakStrong;
   CString  m_percentFeature;
   CString  m_percentCoverage;
   //}}AFX_DATA
   int bounds;
   int extents;
   int nearest;
   int first;
   int last;
   int firstOf2;
   int lastOf2;
   int contrast;
   int weakStrong;
   int roughSmooth;
   int percentFeature;
   int percentCoverage;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OGP_Edge)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void GetData();
   void PutData();

   // Generated message map functions
   //{{AFX_MSG(OGP_Edge)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnSaveDefaults();
   afx_msg void OnLoadDefaults();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// OGP_Tol dialog
class OGP_Tol : public CDialog
{
// Construction
public:
   OGP_Tol(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OGP_Tol)
   enum { IDD = IDD_OGP_TOL };
   CString  m_fLower;
   BOOL  m_fPrint;
   BOOL  m_fStats;
   CString  m_fUpper;
   CString  m_feature;
   CString  m_report;
   CString  m_xLower;
   BOOL  m_xPrint;
   BOOL  m_xStats;
   CString  m_xUpper;
   CString  m_yLower;
   BOOL  m_yPrint;
   BOOL  m_yStats;
   CString  m_yUpper;
   CString  m_zLoc;
   CString  m_zLower;
   BOOL  m_zPrint;
   BOOL  m_zStats;
   CString  m_zUpper;
   //}}AFX_DATA
   CString report;
   CString feature;
   double fUpper;
   double fLower;
   int fPrint;
   int fStats;
   double xUpper;
   double xLower;
   int xPrint;
   int xStats;
   double yUpper;
   double yLower;
   int yPrint;
   int yStats;
   double zUpper;
   double zLower;
   int zPrint;
   int zStats;
   double zLoc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OGP_Tol)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void GetData();
   void PutData();

   // Generated message map functions
   //{{AFX_MSG(OGP_Tol)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnSaveDefaults();
   afx_msg void OnLoadDefaults();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// OGP_Point dialog
class OGP_Point : public CDialog
{
// Construction
public:
   OGP_Point(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OGP_Point)
   enum { IDD = IDD_OGP_SET_POINT_TYPE };
   int      m_type;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OGP_Point)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(OGP_Point)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// OGP_Centroid dialog
class OGP_Centroid : public CDialog
{
// Construction
public:
   OGP_Centroid(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(OGP_Centroid)
   enum { IDD = IDD_OGP_CENTROID };
   CScrollBar  m_thresholdSC;
   CScrollBar  m_multipleSC;
   CScrollBar  m_filterSC;
   BOOL  m_fill;
   CString  m_filter;
   int      m_illumination;
   CString  m_multiple;
   BOOL  m_multipleCheck;
   CString  m_threshold;
   BOOL  m_touchBoundary;
   //}}AFX_DATA
   int threshold;
   int multiple;
   int filter;
   BOOL Multiple;
   BOOL Fill;
   BOOL TouchBoundary;
   int illumination;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OGP_Centroid)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void GetData();
   void PutData();

   // Generated message map functions
   //{{AFX_MSG(OGP_Centroid)
   virtual BOOL OnInitDialog();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnSaveDefaults();
   afx_msg void OnLoadDefaults();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
