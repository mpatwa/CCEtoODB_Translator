// $Header: /CAMCAD/5.0/Settings.h 36    12/10/06 3:57p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#if !defined(__Settings_h__)
#define __Settings_h__

#pragma once

#include "format_s.h"
#include "PinLabelSettings.h"
#include "general.h"
#include "afxwin.h"
#include "Units.h"
#include "DbUtil.h"

//#if defined(__CcDoc_h__)
//#error CcDoc.h included in __FILE__
//#endif

class CCamCadDataSettings;

enum FilterModes
{
   Filter_Endpoint,
   Filter_Entity,
   Filter_Center,
};

enum FilterTypes
{
   Filter_All,
   Filter_Pins,
};

void SetPageSize(CCEtoODBDoc *doc, double xmin, double xmax, double ymin, double ymax);

//_____________________________________________________________________________
class SettingsStruct
{
public:
   SettingsStruct(CCamCadDataSettings& camCadDataSettings);
   SettingsStruct();
   ~SettingsStruct();

   void init();

   // any members added to this structure that use dynamice storage (include CString) must be
   // considered in the operator= member function.
   SettingsStruct& operator=(const SettingsStruct& other);

private:
   const static int m_dDistanceTolerance = 1; // This is in micron
   int m_maxZoom;
   bool m_showFillsDefault;
   bool m_showApFillsDefault;
   bool m_showAttribsDefault;
	bool m_writeVerificationLog;
   CInsertTypeFilter m_insertTypeFilterDefault;
   CGraphicsClassFilter m_graphicsClassFilterDefault;

   // Future CCamCadDataSettings members
   CCamCadDataSettings* m_camCadDataSettings;
   CCamCadDataSettings* m_allocatedCamCadDataSettings;

   //PageUnitsTag m_pageUnits;
   //double m_xMin;
   //double m_xMax;
   //double m_yMin;
   //double m_yMax;

   CString m_directoryMacro;

public:
   short ExtentMargin;
   double CompExtentMargin;
   int SearchRange;
   int TestAccessSize;
   double CentroidSize;
   double m_dMinimumTHDrillSize;

   char FillStyle;

   char HideDetail;
   double HideDetailRatio; // ratio of size of page to size of insert to draw the insert

   COLORREF Bckgrnd;
   COLORREF Foregrnd;
   COLORREF CrossColor;
   COLORREF UndefinedColor;
   COLORREF ToolColor;
   COLORREF UndefToolColor;
   COLORREF SelectColor;
   COLORREF MarkedColor;
   COLORREF HighlightColor;
   COLORREF GridColor;
   COLORREF HideDetailColor;
   COLORREF TestAccessColor;

   short ColorMixMode;

   int SnapCursor;
   double GridSpaceX, GridSpaceY, GridOffX, GridOffY;
   double SnapSpaceX, SnapSpaceY, SnapOffX, SnapOffY;

   unsigned char ShowPoints:1;
   unsigned char Translucent:1;
   unsigned char Crosshairs:1;
   unsigned char Grid:1;
   unsigned char Snap:1;
   unsigned char GridStyle:1; // 1-dots 0-lines
   unsigned char ShowUndefined:1;
   unsigned char ShowUndefinedNums:1;
   unsigned char ShowAllAttribs:1;
	unsigned char AlwaysShowOverrides:1;
   unsigned char FilterOn:1;
   unsigned char FilterInserts:1;
   unsigned char FilterLines:1;
   unsigned char FilterPoints:1;
   unsigned char FilterMode:2;
   unsigned char FilterTypes;
   bool m_filterOnSingleTime;

   //double SmallWidth;
   int UndefinedApSize;
   int ComponentSMDrule;

   // CAMCAD Navigator settings
   bool navigatorZoomMode; // true == zoom, false == pan
   bool navigatorFlipView; // true == flipview on comp selection, false == leave it be
   bool navigatorShowSide; // true == show component side in list box
   bool navigatorShowType; // true == show insert type in list box

   int TextSpaceRatio; // percent of character width for space

   // Redlining
   double RL_textHeight;   // default text height for Redlining
   double RL_arrowSize;    // Arrow Size for Redlining
   double RL_WM, RL_WS, RL_WL;   // redline width medium, small, large
   // Join Lines
   double JOIN_chamfer;       // chamfer size
   double JOIN_radius;        // radius size
   double JOIN_autoSnapSize;  // variance for autojoin
   //
   double Current_TextSize;   // for Current Settings Dialog

   // DRC_Arrow size
   double DRC_MAX_ARROWSIZE;  // max an minimum arrow size for DRC markers
   double DRC_MIN_ARROWSIZE;

   // DXF, Gerber, HP, File extensions Settings
   struct GRstruct Gerbersettings;     
   struct DXFstruct DXFsettings;    
   struct EXstruct Excellonsettings;      
   struct HPstruct Hpsettings;      // in mm, needs to be normalized for a page
   struct FileExt  Fileextensions;
   struct IPCstruct IPCsettings;

   CPinLabelSettings& getPinLabelSettings();

	// Fixture Reuse Settings
	int FixtureReuseFileType;  // 0 = camcad, 1 = agilent
   bool m_normalizeRefNameRotations;
private:
   CPinLabelSettings* m_pinLabelSettings;

public:
   void LoadSettings(const char *Filename);
   void SaveSettings(CString Filename);
   void initializeSettings();

   // accessors
   CCamCadDataSettings& getCamCadDataSettings() { return *m_camCadDataSettings; }
   const CCamCadDataSettings& getCamCadDataSettings() const { return *m_camCadDataSettings; }

   PageUnitsTag getPageUnits() const;
   void setPageUnits(PageUnitsTag pageUnits);

   double getXmin() const ;
   void setXmin(double xMin);

   double getYmin() const;
   void setYmin(double yMin);

   double getXmax() const;
   void setXmax(double xMax);

   double getYmax() const;
   void setYmax(double yMax);

   CExtent getExtent() const;
   void setExtent(const CExtent& extent);

   double getSmallWidth() const;
   void setSmallWidth(double width);

   CString getDirectoryMacro() const { return m_directoryMacro; }
   void setDirectoryMacro(const CString& directoryMacro) { m_directoryMacro = directoryMacro; }

   double getTextSpaceRatio()  const { return TextSpaceRatio/100.;    }

   int getMaxZoom() const { return m_maxZoom; }
   void setMaxZoom(int maxZoom) { m_maxZoom = maxZoom; }

   // Default Document Settings
   bool getShowFillsDefault() const;
   void setShowFillsDefault(bool flag);

   bool getShowApFillsDefault() const;
   void setShowApFillsDefault(bool flag);

   bool getShowAttribsDefault() const;
   void setShowAttribsDefault(bool flag);

   bool getWriteVerificationLog() const;
   void setWriteVerificationLog(bool flag);

   CInsertTypeFilter SettingsStruct::getInsertTypeFilterDefault() const;
   void SettingsStruct::setInsertTypeFilterDefault(const CInsertTypeFilter& insertTypeFilterDefault);

   CGraphicsClassFilter SettingsStruct::getGraphicsClassFilterDefault() const;
   void SettingsStruct::setGraphicsClassFilterDefault(const CGraphicsClassFilter& graphicsClassFilterDefault);

   static double getDistanceTolerance(PageUnitsTag pageUnit);
   void setNormalizeRefNameRotations(bool flag) { m_normalizeRefNameRotations = flag;}
   bool getNormalizeRefNameRotations() const { return m_normalizeRefNameRotations;}
};

/////////////////////////////////////////////////////////////////////////////
// SystemColorButton window
class SystemColorButton : public CButton
{
public:
   SystemColorButton();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SystemColorButton)
   public:
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~SystemColorButton();

   // Generated message map functions
protected:
   //{{AFX_MSG(SystemColorButton)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// SettingsGrid dialog
class SettingsGrid : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsGrid)

// Construction
public:
   SettingsGrid();
   ~SettingsGrid();

// Dialog Data
   //{{AFX_DATA(SettingsGrid)
   enum { IDD = IDD_SETTINGS_GRID };
   int      m_dots;
   BOOL  m_showGrid;
   BOOL  m_snapOn;
   CString  m_XGridOffset;
   CString  m_XGridSpace;
   CString  m_XSnapOffset;
   CString  m_XSnapSpace;
   CString  m_YGridOffset;
   CString  m_YGridSpace;
   CString  m_YSnapOffset;
   CString  m_YSnapSpace;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(SettingsGrid)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SettingsGrid)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// SettingsAperture dialog
class SettingsAperture : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsAperture)

// Construction
public:
   SettingsAperture();
   ~SettingsAperture();

// Dialog Data
   //{{AFX_DATA(SettingsAperture)
   enum { IDD = IDD_SETTINGS_APERTURE };
   BOOL  m_ShowApNum;
   BOOL  m_ShowUAp;
   int      m_UApSize;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(SettingsAperture)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SettingsAperture)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// SettingsPage dialog
class SettingsPage : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsPage)

// Construction
public:
   SettingsPage();
   ~SettingsPage();

// Dialog Data
   //{{AFX_DATA(SettingsPage)
   enum { IDD = IDD_SETTINGS_PAGE };
   CSpinButtonCtrl   m_spin;
   CEdit m_DecimalsEdit;
   CComboBox   m_UnitsLB;
   CString  m_maxX;
   CString  m_maxY;
   CString  m_minX;
   CString  m_minY;
   int      m_Decimals;
   //}}AFX_DATA

   int units;

private:
   int m_lastUnit;

public:
// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(SettingsPage)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SettingsPage)
   afx_msg void OnChangeDecimals();
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeUnits();
   afx_msg void OnFitPageToImage();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// SettingsSystem dialog
class SettingsSystem : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsSystem)

// Construction
public:
   SettingsSystem();
   ~SettingsSystem();

// Dialog Data
   //{{AFX_DATA(SettingsSystem)
   enum { IDD = IDD_SETTINGS_SYSTEM };
   int      m_searchRange;
   CString  m_join_chamfer;
   CString  m_join_radius;
   CString  m_join_autoSnapSize;
   int      m_testAccessSize;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(SettingsSystem)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SettingsSystem)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// SettingsView dialog
class SettingsView : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsView)

// Construction
public:
   SettingsView();
   ~SettingsView();

// Dialog Data
   //{{AFX_DATA(SettingsView)
   enum { IDD = IDD_SETTINGS_VIEW };
   CComboBox   m_negMixModeCB;
   CComboBox   m_mixModeCB;
   CComboBox   m_fillModeCB;
   int      m_margin;
   BOOL  m_translucent;
   int      m_fillMode;
   CString  m_mixMode;
   CString  m_negMixMode;
   BOOL  m_showOrigin;
   double   m_hideDetailRatio;
   BOOL  m_hideDetail;
   double   m_margin2;
   //}}AFX_DATA
   int colorMixMode;
   CString mixMode;


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(SettingsView)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SettingsView)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   BOOL m_showHiddenAttributes;
   BOOL m_showPoints;
   BOOL m_alwaysShowOverrides;
};

/////////////////////////////////////////////////////////////////////////////
// SettingsColor dialog
class SettingsColor : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsColor)

// Construction
public:
   SettingsColor();
   ~SettingsColor();

// Dialog Data
   //{{AFX_DATA(SettingsColor)
   enum { IDD = IDD_SETTINGS_COLOR };
   SystemColorButton m_utools;
   SystemColorButton m_testOffset;
   SystemColorButton m_hideDetail;
   SystemColorButton m_tools;
   SystemColorButton m_uaps;
   SystemColorButton m_selected;
   SystemColorButton m_highlighted;
   SystemColorButton m_highlight;
   SystemColorButton m_marked;
   SystemColorButton m_grid;
   SystemColorButton m_background;
   SystemColorButton m_page;
   SystemColorButton m_axes;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(SettingsColor)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(SettingsColor)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedColorNavigator();
};

/////////////////////////////////////////////////////////////////////////////
// SettingsRedline dialog
class SettingsRedline : public CPropertyPage
{
   DECLARE_DYNCREATE(SettingsRedline)

// Construction
public:
   SettingsRedline(); 
   ~SettingsRedline();

// Dialog Data
   //{{AFX_DATA(SettingsRedline)
   enum { IDD = IDD_SETTINGS_REDLINE };
   CString  m_arrowSize;
   CString  m_textHeight;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SettingsRedline)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SettingsRedline)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
#pragma once


// CPinLabelSettingsPropertyPage dialog

class CPinLabelSettingsPropertyPage : public CPropertyPage
{
   DECLARE_DYNAMIC(CPinLabelSettingsPropertyPage)

public:
   CPinLabelSettingsPropertyPage();
   virtual ~CPinLabelSettingsPropertyPage();

// Dialog Data
   enum { IDD = IDD_PinLabelSettings };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()

private:
   double m_nominalTextHeight;
   UINT m_minTextHeight;
   UINT m_maxTextHeight;
   UINT m_textBoxHeight;
   CString m_fontFace;
   LOGFONT m_logFont;
   int m_pageUnits;
   double m_textAngle;
   CPinLabelSettings m_pinLabelSettings;

public:
   virtual BOOL OnInitDialog();
   void setPageUnits(int pageUnits) { m_pageUnits = pageUnits; }
   const CPinLabelSettings& getPinLabelSettings() { return m_pinLabelSettings; }
   void setPinLabelSettings(const CPinLabelSettings& pinLabelSettings) { m_pinLabelSettings = pinLabelSettings; }

protected:
   virtual void OnOK();
public:
   afx_msg void OnBnClickedBrowseFont();
private:
};

///////////////////////////////////////////////////////////////
// CColorSettingsNavigator dialog

class CColorSettingsNavigator : public CDialog
{
   DECLARE_DYNAMIC(CColorSettingsNavigator)

public:
   CColorSettingsNavigator(CWnd* pParent = NULL);   // standard constructor
   virtual ~CColorSettingsNavigator();

// Dialog Data
   //{{AFX_DATA(CColorSettingsNavigator)
   enum { IDD = IDD_SETTINGS_COLOR_NAVIGATOR };
   SystemColorButton NavColor1;
   SystemColorButton NavColor2;
   SystemColorButton NavColor3;
   SystemColorButton NavColor4;
   SystemColorButton NavColor5;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CColorSettingsNavigator)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   DECLARE_MESSAGE_MAP()

   virtual BOOL OnInitDialog();
};

#endif
