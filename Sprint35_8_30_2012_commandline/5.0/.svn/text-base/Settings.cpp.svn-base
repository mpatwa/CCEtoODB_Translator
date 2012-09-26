// $Header: /CAMCAD/5.0/Settings.cpp 57    3/19/07 4:31a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Settings.h"
#include "ccdoc.h"
#include "ccview.h"
#include <math.h>
#include "graph.h"
#include "pcbutil.h"
#include "Draw.h"
#include "PinLabelSettings.h"
#include ".\settings.h"
#include "MainFrm.h"
#include "EnumIterator.h"
#include "PersistantColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString unitsString(int units);

extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern CDialogBar *CurrentSettings; // from MAINFRM.CPP
extern CString NotePadProgram;   // from CAMCAD.CPP
extern CString LogReaderProgram; // from CAMCAD.CPP

//void UpdateLayers(CCEtoODBDoc *doc);
void SetPageSize(CCEtoODBDoc *doc, double xmin, double xmax, double ymin, double ymax);

static BOOL Initialized;
static CCEtoODBDoc *doc;
bool getShowHiddenAttributes();
bool setShowHiddenAttributes(bool flag);

//_____________________________________________________________________________
SettingsStruct::SettingsStruct()
{
   m_allocatedCamCadDataSettings = new CCamCadDataSettings();
   m_camCadDataSettings = m_allocatedCamCadDataSettings;

   init();
}

SettingsStruct::SettingsStruct(CCamCadDataSettings& camCadDataSettings)
{
   m_allocatedCamCadDataSettings = NULL;
   m_camCadDataSettings = &camCadDataSettings;

   init();
}

SettingsStruct::~SettingsStruct()
{
   delete m_pinLabelSettings;
   delete m_allocatedCamCadDataSettings;
}

void SettingsStruct::init()
{
	m_dMinimumTHDrillSize = 0.0;
   m_pinLabelSettings = NULL;
	m_writeVerificationLog = false;
   m_maxZoom = 1000;
   m_filterOnSingleTime = true;
   m_showFillsDefault   = true;
   m_showApFillsDefault = true;
   m_showAttribsDefault = true;

   m_insertTypeFilterDefault.setToDefault();
   m_graphicsClassFilterDefault.setToDefault();
}

SettingsStruct& SettingsStruct::operator=(const SettingsStruct& other)
{
   if (&other != this)
   {
      if (other.m_pinLabelSettings == NULL)
      {
         delete m_pinLabelSettings;
         m_pinLabelSettings = NULL;
      }
      else
      {
         getPinLabelSettings() = *(other.m_pinLabelSettings);
      }

      setDirectoryMacro(other.getDirectoryMacro());

      // non-dynamic storage members;
      setXmin(other.getXmin());
      setXmax(other.getXmax());
      setYmin(other.getYmin());
      setYmax(other.getYmax());;

      setPageUnits(other.getPageUnits());
      
      ExtentMargin    		 = other.ExtentMargin;
      CompExtentMargin		 = other.CompExtentMargin;
      SearchRange     		 = other.SearchRange;
      TestAccessSize  		 = other.TestAccessSize;
      CentroidSize    		 = other.CentroidSize;
		m_dMinimumTHDrillSize = other.m_dMinimumTHDrillSize;

      FillStyle = other.FillStyle;

      HideDetail      = other.HideDetail;
      HideDetailRatio = other.HideDetailRatio;

      Bckgrnd         = other.Bckgrnd;
      Foregrnd        = other.Foregrnd;
      CrossColor      = other.CrossColor;
      UndefinedColor  = other.UndefinedColor;
      ToolColor       = other.ToolColor;
      UndefToolColor  = other.UndefToolColor;
      SelectColor     = other.SelectColor;
      MarkedColor     = other.MarkedColor;
      HighlightColor  = other.HighlightColor;
      GridColor       = other.GridColor;
      HideDetailColor = other.HideDetailColor;
      TestAccessColor = other.TestAccessColor;

      ColorMixMode    = other.ColorMixMode;

      SnapCursor      = other.SnapCursor;
       
      GridSpaceX = other.GridSpaceX;
      GridSpaceY = other.GridSpaceY;
      GridOffX   = other.GridOffX;
      GridOffY   = other.GridOffY;
      SnapSpaceX = other.SnapSpaceX;
      SnapSpaceY = other.SnapSpaceY;
      SnapOffX   = other.SnapOffX;
      SnapOffY   = other.SnapOffY;

      Translucent				= other.Translucent;
      ShowPoints				= other.ShowPoints;
      Crosshairs				= other.Crosshairs;
      Grid						= other.Grid;
      Snap						= other.Snap;
      GridStyle				= other.GridStyle;
      ShowUndefined			= other.ShowUndefined;
      ShowUndefinedNums		= other.ShowUndefinedNums;
      ShowAllAttribs			= other.ShowAllAttribs;
		AlwaysShowOverrides	= other.AlwaysShowOverrides;
      FilterOn					= other.FilterOn;
      FilterInserts			= other.FilterInserts;
      FilterLines				= other.FilterLines;
      FilterPoints			= other.FilterPoints;
      FilterMode				= other.FilterMode;

      FilterTypes = other.FilterTypes;

      setSmallWidth(other.getSmallWidth());
      UndefinedApSize  = other.UndefinedApSize;
      ComponentSMDrule = other.ComponentSMDrule;

      TextSpaceRatio = other.TextSpaceRatio;
		m_writeVerificationLog = other.m_writeVerificationLog;

      // Redlining
      RL_textHeight = other.RL_textHeight;
      RL_arrowSize  = other.RL_arrowSize;
      RL_WM         = other.RL_WM;
      RL_WS         = other.RL_WS;
      RL_WL         = other.RL_WL;

      // Join Lines
      JOIN_chamfer      = other.JOIN_chamfer;
      JOIN_radius       = other.JOIN_radius;
      JOIN_autoSnapSize = other.JOIN_autoSnapSize;

      //
      Current_TextSize = other.Current_TextSize;

      // DRC_Arrow size
      DRC_MAX_ARROWSIZE = other.DRC_MAX_ARROWSIZE;
      DRC_MIN_ARROWSIZE = other.DRC_MIN_ARROWSIZE;

      // DXF, Gerber, HP, File extensions Settings
      Gerbersettings   = other.Gerbersettings;
      DXFsettings      = other.DXFsettings;
      Excellonsettings = other.Excellonsettings;
      Hpsettings       = other.Hpsettings;
      Fileextensions   = other.Fileextensions;

		// Fixture Reuse
		FixtureReuseFileType = other.FixtureReuseFileType;

      // IPC settings
      IPCsettings = other.IPCsettings;

      m_maxZoom        = other.m_maxZoom;

      // Navigator
      navigatorFlipView = other.navigatorFlipView;
      navigatorZoomMode = other.navigatorZoomMode;
      navigatorShowSide = other.navigatorShowSide;
      navigatorShowType = other.navigatorShowType;

      // default document settings
      m_showFillsDefault   = other.m_showFillsDefault;
      m_showApFillsDefault = other.m_showApFillsDefault;
      m_showAttribsDefault = other.m_showAttribsDefault;

      m_insertTypeFilterDefault    = other.m_insertTypeFilterDefault;
      m_graphicsClassFilterDefault = other.m_graphicsClassFilterDefault;
      m_normalizeRefNameRotations = other.m_normalizeRefNameRotations;
   }

   return *this;
}

void SettingsStruct::initializeSettings()
{
   // initialize settings
   setXmax(20.);
   setYmax(15.);
   setXmin(-20.);
   setYmin(-15.);
   FillStyle         = -1;
   Grid              = 1;
   ComponentSMDrule  = 1;
   ShowPoints        = 0;
   Crosshairs        = 1;
   GridStyle         = 1;
   GridSpaceX        = (float)1.0;
   GridOffX          = (float)0.0;
   GridSpaceY        = (float)1.0;
   GridOffY          = (float)0.0;
   SnapSpaceX        = (float)1.0;
   SnapOffX          = (float)0.0;
   SnapSpaceY        = (float)1.0;
   SnapOffY          = (float)0.0;
   CrossColor        = RGB(255, 255, 255);
   GridColor         = RGB(200, 200, 200);
   Foregrnd          = RGB(0, 0, 0);
   Bckgrnd           = RGB(192, 192, 192);
   UndefinedColor    = RGB(0, 255, 0);
   ToolColor         = RGB(255, 255, 0);
   UndefToolColor    = RGB(255, 0, 0);
   MarkedColor       = RGB(128, 128, 0);
   SelectColor       = RGB(192, 192, 192);
   HighlightColor    = RGB(255, 255, 255);
   HideDetailColor   = RGB(192, 192, 192);
   TestAccessColor   = RGB(192, 192, 192);
   TestAccessSize    = 300;
   CentroidSize      = 0.050;
   HideDetail        = TRUE;
   HideDetailRatio   = 0.01;
   setPageUnits(pageUnitsInches);
   UndefinedApSize   = 1000;
   ShowUndefinedNums = TRUE;
   ShowUndefined     = TRUE;
   ExtentMargin      = 3;
   CompExtentMargin  = 5;
	m_dMinimumTHDrillSize = 0.011;	// inches
   setSmallWidth(.001);
   Translucent       = 0;
   ColorMixMode      = R2_MERGEPEN;
   SearchRange       = 300;
   TextSpaceRatio    = 25;
	m_writeVerificationLog = false;
   RL_textHeight     = 0.1;
   RL_arrowSize      = 0.2;
   RL_WS             = 0.001;
   RL_WM             = 0.02;
   RL_WL             = 0.05;
   JOIN_chamfer      = 0.25;
   JOIN_radius       = 0.25;
   JOIN_autoSnapSize = 0.001;

   DRC_MAX_ARROWSIZE = 0.1; // max an minimum arrow size for DRC markers
   DRC_MIN_ARROWSIZE = 0.002;

   Current_TextSize  = 0.01;

   FilterOn          = FALSE;
   FilterMode        = Filter_Endpoint;
   FilterLines       = TRUE;
   FilterPoints      = TRUE;
   FilterInserts     = TRUE;

   DXFsettings.Circles_2_Aps = FALSE;
   DXFsettings.Donuts_2_Aps  = FALSE;
   DXFsettings.Proportional  = FALSE;
   DXFsettings.PolyEnds      = FALSE;
   DXFsettings.Units         = 0;
   DXFsettings.XscaleText    = FALSE;
   DXFsettings.UnMirrorText  = FALSE;

   // here set wolfs defaults Gerbersettings
   Gerbersettings.format             = 1;
   Gerbersettings.type               = 0;
   Gerbersettings.zero               = 0;
   Gerbersettings.units              = 0;
   Gerbersettings.digits             = 2;
   Gerbersettings.decimal            = 3;
   Gerbersettings.circleMode         = 0;
   Gerbersettings.thermalMacros      = 1;
   Gerbersettings.sectorizationAngle = 15;
   Gerbersettings.PolyFill           = TRUE;
   Gerbersettings.layerStencilThickness = 0;

   // here set wolfs defaults Excellonsettings
   Excellonsettings.format = 1;   // excellon format 2
   Excellonsettings.type   = 0;
   Excellonsettings.zeroSuppression   = 0;
	Excellonsettings.includeDecimalPoint = 0;
   Excellonsettings.units  = 0;
	Excellonsettings.wholeDigits = 2;
	Excellonsettings.significantDigits = 3;
	Excellonsettings.drillTypes = 0;
	Excellonsettings.probeDrillSurface = 0;
   Excellonsettings.includePilotHoles = TRUE;
	Excellonsettings.includeToolingHoles = TRUE;

   Hpsettings.HPspace = TRUE;

   for (int i=0;i<MAX_HPPENS;i++)
   {
      Hpsettings.HpPens[i].width = 0.5; // in mm needs to be normalized to page settings
      Hpsettings.HpPens[i].layer.Format("PEN_%d",i+1);
   }

   Fileextensions.Gerber   = "*.gbr;*.ger;";
   Fileextensions.HPGL     = "*.hp;*.hpl;";
   Fileextensions.EXCELLON = "*.fr;*.drl;*.ex";
   Fileextensions.IGES     = "*.igs;";
   Fileextensions.APERTURE = "*.gap;";

   //IPC default settings
   IPCsettings.IPC_Format = (Product != PRODUCT_GRAPHIC)?2:0; //0-IPC 350
   IPCsettings.Units = pageUnitsMilliMeters;

	// Fixture Reuse
	FixtureReuseFileType = 0; // CAMCAD Fixture File

   // Navigator
   navigatorFlipView = false;
   navigatorZoomMode = true;
   navigatorShowSide = false;
   navigatorShowType = false;

   // default document settings
   m_showFillsDefault   = true;
   m_showApFillsDefault = true;
   m_showAttribsDefault = true;

   m_insertTypeFilterDefault.setToDefault();
   m_graphicsClassFilterDefault.setToDefault();
   m_normalizeRefNameRotations = false;
}

PageUnitsTag SettingsStruct::getPageUnits() const
{
   return getCamCadDataSettings().getPageUnits();
}

void SettingsStruct::setPageUnits(PageUnitsTag pageUnits)
{
   getCamCadDataSettings().setPageUnits(pageUnits);
}

double SettingsStruct::getXmin() const
{
   return getCamCadDataSettings().getXmin();
}

void SettingsStruct::setXmin(double xMin)
{
   getCamCadDataSettings().setXmin(xMin);
}

double SettingsStruct::getYmin() const
{
   return getCamCadDataSettings().getYmin();
}

void SettingsStruct::setYmin(double yMin)
{
   getCamCadDataSettings().setYmin(yMin);
}

double SettingsStruct::getXmax() const
{
   return getCamCadDataSettings().getXmax();
}

void SettingsStruct::setXmax(double xMax)
{
   getCamCadDataSettings().setXmax(xMax);
}

double SettingsStruct::getYmax() const
{
   return getCamCadDataSettings().getYmax();
}

void SettingsStruct::setYmax(double yMax)
{
   getCamCadDataSettings().setYmax(yMax);
}

CExtent SettingsStruct::getExtent() const
{
   return getCamCadDataSettings().getExtent();
}

void SettingsStruct::setExtent(const CExtent& extent)
{
   getCamCadDataSettings().setExtent(extent);
}

double SettingsStruct::getSmallWidth() const
{
   return getCamCadDataSettings().getSmallWidth();
}

void SettingsStruct::setSmallWidth(double width)
{
   getCamCadDataSettings().setSmallWidth(width);
}

bool SettingsStruct::getShowFillsDefault() const
{
   return m_showFillsDefault;
}

void SettingsStruct::setShowFillsDefault(bool flag)
{
   m_showFillsDefault = flag;
}

bool SettingsStruct::getShowApFillsDefault() const
{
   return m_showApFillsDefault;
}

void SettingsStruct::setShowApFillsDefault(bool flag)
{
   m_showApFillsDefault = flag;
}

bool SettingsStruct::getShowAttribsDefault() const
{
   return m_showAttribsDefault;
}

void SettingsStruct::setShowAttribsDefault(bool flag)
{
   m_showAttribsDefault = flag;
}

bool SettingsStruct::getWriteVerificationLog() const
{
   return m_writeVerificationLog;
}

void SettingsStruct::setWriteVerificationLog(bool flag)
{
   m_writeVerificationLog = flag;
}

CPinLabelSettings& SettingsStruct::getPinLabelSettings()
{
   if (m_pinLabelSettings == NULL)
   {
      m_pinLabelSettings = new CPinLabelSettings();
   }

   return *m_pinLabelSettings;
}

CInsertTypeFilter SettingsStruct::getInsertTypeFilterDefault() const
{
   return m_insertTypeFilterDefault;
}

void SettingsStruct::setInsertTypeFilterDefault(const CInsertTypeFilter& insertTypeFilterDefault)
{
   m_insertTypeFilterDefault = insertTypeFilterDefault;
}

CGraphicsClassFilter SettingsStruct::getGraphicsClassFilterDefault() const
{
   return m_graphicsClassFilterDefault;
}

void SettingsStruct::setGraphicsClassFilterDefault(const CGraphicsClassFilter& graphicsClassFilterDefault)
{
   m_graphicsClassFilterDefault = graphicsClassFilterDefault;
}

void SettingsStruct::LoadSettings(const char *Filename)
{
   FILE     *stream;
   COLORREF color;
   int      err;
   int      k=1;
   int      i;
   double   d;
   const int lineSize = 1024;
   char     line[lineSize];
   // char     *KeyWord;
	CString keyword;
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();

	frame->getNavigator().highlightColors[0] = 8421631;
	frame->getNavigator().highlightColors[1] = 8454016;
	frame->getNavigator().highlightColors[2] = 4227327;
	frame->getNavigator().highlightColors[3] = 16744576;
	frame->getNavigator().highlightColors[4] = 8388863;

   stream = fopen(Filename, "r"); // text mode by default

   if (stream == NULL)
   {
      ErrorMessage(Filename, "Settings File not found!");
      return;
   }

   fgets(line,lineSize,stream);

   if ( STRICMP(line, "! CAMCAD Settings File\n") != 0 && STRICMP(line,"; Begin CAMCAD Settings File\n") != 0 )
   {
      ErrorMessage(Filename, "Wrong File Format!");
      return;
   }

   CSupString lineString;
   CStringArray params;

   while (fgets(line, lineSize, stream)!=NULL)
   {
      if (line[0] != '.') 
			continue;

      lineString = line;

      int numPar = lineString.ParseQuote(params," \t");

      keyword = strtok(line, " \t");

      if (keyword.CompareNoCase(".OldSave") == 0)
      {
			// Obsolete - Was a control for cc file output format, useful back
         // when conversion to XML format was taking place. As of today (21 July 2011)
         // the old format save is entirely nuked. We are firmly entrenched in
         // the XML style CC/CCZ file now.
         // Approved by Mark in 7/21/2011 email.
         // Occurred in conjunction with adding CCZ.IN support.
      }

      if (keyword.CompareNoCase(".NavigatorZoomPan") == 0)
      {
			// get token value
			keyword = strtok(NULL, " \n=");
         if (keyword.CompareNoCase("Zoom") == 0)
            this->navigatorZoomMode = true;
         else if (keyword.CompareNoCase("Pan") == 0)
            this->navigatorZoomMode = false;
         continue;
      }
      if (keyword.CompareNoCase(".NavigatorFlipView") == 0)
      {
			// get token value
			keyword = strtok(NULL, " \n=");
         if (keyword.CompareNoCase("yes") == 0)
            this->navigatorFlipView = true;
         else if (keyword.CompareNoCase("no") == 0)
            this->navigatorFlipView = false;
         continue;
      }
      if (keyword.CompareNoCase(".NavigatorShowSide") == 0)
      {
			// get token value
			keyword = strtok(NULL, " \n=");
         if (keyword.CompareNoCase("yes") == 0)
            this->navigatorShowSide = true;
         else if (keyword.CompareNoCase("no") == 0)
            this->navigatorShowSide = false;
         continue;
      }
      if (keyword.CompareNoCase(".NavigatorShowType") == 0)
      {
			// get token value
			keyword = strtok(NULL, " \n=");
         if (keyword.CompareNoCase("yes") == 0)
            this->navigatorShowType = true;
         else if (keyword.CompareNoCase("no") == 0)
            this->navigatorShowType = false;
         continue;
      }
      if (keyword.CompareNoCase(".FillStyle") == 0)
      {
			// get token value
			keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->FillStyle = i;
         continue;
      }

      if (keyword.CompareNoCase(".Color") == 0)
      {
         keyword = strtok(NULL, " ");
			
         if (keyword.CompareNoCase("NavHighlight1") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               frame->getNavigator().highlightColors[0] = color;
         }
         else if (keyword.CompareNoCase("NavHighlight2") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               frame->getNavigator().highlightColors[1] = color;
         }
         else if (keyword.CompareNoCase("NavHighlight3") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               frame->getNavigator().highlightColors[2] = color;
         }
         else if (keyword.CompareNoCase("NavHighlight4") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               frame->getNavigator().highlightColors[3] = color;
         }
         else if (keyword.CompareNoCase("NavHighlight5") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               frame->getNavigator().highlightColors[4] = color;
         }
         else if (keyword.CompareNoCase("Background") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               frame->getNavigator().highlightColors[4] = color;
         }
         else if (keyword.CompareNoCase("Page") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->Foregrnd = color;
         }
         else if (keyword.CompareNoCase("Crosshairs") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->CrossColor = color;
         }
         else if (keyword.CompareNoCase("Grid") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->GridColor = color;
         }
         else if (keyword.CompareNoCase("UndefinedApertures") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->UndefinedColor = color;
         }
         else if (keyword.CompareNoCase("Tools") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->ToolColor = color;
         }
         else if (keyword.CompareNoCase("UndefinedTools") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->UndefToolColor = color;
         }
         else if (keyword.CompareNoCase("Marked") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->MarkedColor = color;
         }
         else if (keyword.CompareNoCase("Selected") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->SelectColor = color;
         }
         else if (keyword.CompareNoCase("Highlighted") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->HighlightColor = color;
         }
         else if (keyword.CompareNoCase("HideDetail") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->HideDetailColor = color;
         }
         else if (keyword.CompareNoCase("TestAccess") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %ld", &color);
            if ((err != EOF) && (err != 0))
               this->TestAccessColor = color;
         }
         continue;
      }

      if (keyword.CompareNoCase(".Grid") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Show") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->Grid = i;
         }
         else if (keyword.CompareNoCase("Snap") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->Snap = i;
         }
         else if (keyword.CompareNoCase("Style") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->GridStyle = i;
         }
         else if (keyword.CompareNoCase("SpaceX") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->GridSpaceX = d;
         }
         else if (keyword.CompareNoCase("SpaceY") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->GridSpaceY = d;
         }
         else if (keyword.CompareNoCase("OffsetX") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->GridOffX = d;
         }
         else if (keyword.CompareNoCase("OffsetY") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->GridOffY = d;
         }
         continue;
      }

      if (keyword.CompareNoCase(".Snap") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("SpaceX") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->SnapSpaceX = d;
         }
         else if (keyword.CompareNoCase("SpaceY") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->SnapSpaceY = d;
         }
         else if (keyword.CompareNoCase("OffsetX") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->SnapOffX = d;
         }
         else if (keyword.CompareNoCase("OffsetY") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->SnapOffY = d;
         }
         continue;
      }

      if (keyword.CompareNoCase(".Units") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->setPageUnits(intToPageUnitsTag(i));
         continue;
      }

      if (keyword.CompareNoCase(".MaxZoom") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->setMaxZoom(i);
         continue;
      }

      if (keyword.CompareNoCase(".Decimals") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("INCH") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               SetDecimals(UNIT_INCHES, i);
         }
         else if (keyword.CompareNoCase("MIL") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               SetDecimals(UNIT_MILS, i);
         }
         else if (keyword.CompareNoCase("MM") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               SetDecimals(UNIT_MM, i);
         }
         else if (keyword.CompareNoCase("HPPLOTTER") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               SetDecimals(UNIT_HPPLOTTER, i);
         }
         else if (keyword.CompareNoCase("TNMETER") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               SetDecimals(UNIT_TNMETER, i);
         }
         continue;
      }

      if (keyword.CompareNoCase(".SearchRange") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->SearchRange = i;
         continue;
      }

      if (keyword.CompareNoCase(".TestAccessSize") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->TestAccessSize = i;
         continue;
      }

      if (keyword.CompareNoCase(".CentroidSize") == 0)
      {
         keyword = strtok(NULL, "\n");
         err = sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->CentroidSize = d;
         continue;
      }

      if (keyword.CompareNoCase(".SnapCursor") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->SnapCursor = i;
         continue;
      }

      if (keyword.CompareNoCase(".PageSize") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Xmin") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->setXmin(d);
         }
         else if (keyword.CompareNoCase("Xmax") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->setXmax(d);
         }
         else if (keyword.CompareNoCase("Ymin") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->setYmin(d);
         }
         else if (keyword.CompareNoCase("Ymax") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->setYmax(d);
         }
         continue;
      }

      if (keyword.CompareNoCase(".SmallWidth") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->setSmallWidth(d);
         continue;
      }

      if (keyword.CompareNoCase(".CurrentTextSize") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->Current_TextSize = d;
         continue;
      }

      if (keyword.CompareNoCase(".Translucent") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("On") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->Translucent = i;
         }
         else if (keyword.CompareNoCase("FillMode") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->ColorMixMode = i;
         }
         continue;
      }

      if (keyword.CompareNoCase(".UndefinedApertures") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Show") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->ShowUndefined = i;
         }
         else if (keyword.CompareNoCase("ShowNumbers") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->ShowUndefinedNums = i;
         }
         continue;
      }

      if (keyword.CompareNoCase(".UndefinedApSize") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->UndefinedApSize = i;
         continue;
      }

      if (keyword.CompareNoCase(".ExtentsMargin") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->ExtentMargin = i;
         continue;
      }

      if (keyword.CompareNoCase(".CompExtentsMargin") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->CompExtentMargin = d;
         continue;
      }

      if (keyword.CompareNoCase(".MinTHDrillSize") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->m_dMinimumTHDrillSize = d;
         continue;
      }

      if (keyword.CompareNoCase(".HideDetail") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->HideDetail = i;
         continue;
      }

      if (keyword.CompareNoCase(".HideDetailRatio") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->HideDetailRatio = d;
         continue;
      }

      if (keyword.CompareNoCase(".TextSpaceRatio") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->TextSpaceRatio = i;
         continue;
      }

      if (keyword.CompareNoCase(".ComponentSMDrule") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %d", &i);
         if ((err != EOF) && (err != 0))
            this->ComponentSMDrule = i;
         continue;
      }

      if (keyword.CompareNoCase(".Show") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Crosshairs") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->Crosshairs = i;
         }
         else if (keyword.CompareNoCase("ShowPoints") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->ShowPoints = i;
         }
         else if (keyword.CompareNoCase("AllAttribs") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %d", &i);
            if ((err != EOF) && (err != 0))
               this->ShowAllAttribs = i;
         }
         continue;
      }

      if (keyword.CompareNoCase(".Directory") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Macro") == 0)
         {
            keyword = strtok(NULL, "\n");

            CString directoryMacro(keyword);
            directoryMacro.TrimLeft("\t =\"");
            directoryMacro.TrimRight("\t =\"");

            this->setDirectoryMacro(directoryMacro);
         }
         continue;
      }

      if (keyword.CompareNoCase(".Redlining") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("TextHeight") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->RL_textHeight = d;
         }
         else if (keyword.CompareNoCase("ArrowSize") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->RL_arrowSize = d;
         }
         else if (keyword.CompareNoCase("WidthSmall") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->RL_WS = d;
         }
         else if (keyword.CompareNoCase("WidthMedium") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->RL_WM = d;
         }
         else if (keyword.CompareNoCase("WidthLarge") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->RL_WL = d;
         }
         continue;
      }

      if (keyword.CompareNoCase(".Pinlabels") == 0)
      {
         keyword = strtok(NULL, " ");

         if (keyword.CompareNoCase("TextHeightMils") == 0)
         {
            keyword = strtok(NULL, "\n");
            err = sscanf(keyword, "= %lg", &d);

            if ((err == EOF) || (err == 0))
               d = DefaultNominalPinLabelTextHeightMils;

            getPinLabelSettings().setTextHeightInches(d/1000.);
         }
         else if (keyword.CompareNoCase("MinTextHeightPixels") == 0)
         {
            keyword = strtok(NULL, "\n");
            err = sscanf(keyword, "= %lg", &d);

            if ((err == EOF) || (err == 0))
               d = DefaultMinPinLabelTextHeightPixels;

            getPinLabelSettings().setMinTextHeightPixels(round(d));
         }
         else if (keyword.CompareNoCase("MaxTextHeightPixels") == 0)
         {
            keyword = strtok(NULL, "\n");
            err = sscanf(keyword, "= %lg", &d);

            if ((err == EOF) || (err == 0))
               d = DefaultMaxPinLabelTextHeightPixels;

            getPinLabelSettings().setMaxTextHeightPixels(round(d));
         }
         else if (keyword.CompareNoCase("TextBoxPixels") == 0)
         {
            keyword = strtok(NULL, "\n");
            err = sscanf(keyword, "= %lg", &d);

            if ((err == EOF) || (err == 0))
               d = DefaultPinLabelTextBoxPixels;

            getPinLabelSettings().setTextBoxPixels(round(d));
         }
         else if (keyword.CompareNoCase("TextBoxPixels") == 0)
         {
            keyword = strtok(NULL, "\n");
            err = sscanf(keyword, "= %lg", &d);

            if ((err == EOF) || (err == 0))
               d = DefaultPinLabelTextBoxPixels;

            getPinLabelSettings().setTextBoxPixels(round(d));
         }
         else if (keyword.CompareNoCase("TextAngle") == 0)
         {
            keyword = strtok(NULL, "\n");
            err = sscanf(keyword, "= %lg", &d);

            if ((err == EOF) || (err == 0))
               d = DefaultPinLabelTextBoxPixels;

            getPinLabelSettings().setTextDegrees(d);
         }
         else if (keyword.CompareNoCase("FontFaceName") == 0)
         {
            CString delimeter = strtok(NULL, "'\n");
            CString fontFace  = strtok(NULL, "'\n");

            if (fontFace.IsEmpty())
               fontFace = DefaultPinLabelFontFaceName;

            getPinLabelSettings().setFontFaceName(fontFace);
         }
         continue;
      }

      if (keyword.CompareNoCase(".JoinLines") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Chamfer") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->JOIN_chamfer = d;
         }
         else if (keyword.CompareNoCase("Radius") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->JOIN_radius = d;
         }
         else if (keyword.CompareNoCase("AutoSnapSize") == 0)
         {
            keyword = strtok(NULL, "\n");
            err=sscanf(keyword, "= %lg", &d);
            if ((err != EOF) && (err != 0))
               this->JOIN_autoSnapSize = d;
         }
         continue;
      }

      if (keyword.CompareNoCase(".DRC_ARROW_MAXSIZE") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->DRC_MAX_ARROWSIZE = d;
         continue;
      }

      if (keyword.CompareNoCase(".DRC_ARROW_MINSIZE") == 0)
      {
         keyword = strtok(NULL, "\n");
         err=sscanf(keyword, "= %lg", &d);
         if ((err != EOF) && (err != 0))
            this->DRC_MIN_ARROWSIZE = d;
         continue;
      }

      if (keyword.CompareNoCase(".DXF") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Circles_2_Aps") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->DXFsettings.Circles_2_Aps = i;
         }
         else if (keyword.CompareNoCase("Donuts_2_Aps") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->DXFsettings.Donuts_2_Aps = i;
         }
         else if (keyword.CompareNoCase("Proportional") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->DXFsettings.Proportional = i;
         }
         else if (keyword.CompareNoCase("Units") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->DXFsettings.Units = i;
         }
         else if (keyword.CompareNoCase("PolyEnds") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->DXFsettings.PolyEnds = i;
         }
         continue;
      }

      if (keyword.CompareNoCase(".Gerber") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Format") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.format = i;
         }
         else if (keyword.CompareNoCase("Type") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.type = i;
         }
         else if (keyword.CompareNoCase("Zero") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.zero = i;
         }
         else if (keyword.CompareNoCase("Units") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.units = i;
         }
         else if (keyword.CompareNoCase("Digits") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.digits = i;
         }
         else if (keyword.CompareNoCase("Decimal") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.decimal = i;
         }
         else if (keyword.CompareNoCase("CircleMode") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.circleMode = i;
         }
         else if (keyword.CompareNoCase("thermalMacros") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.thermalMacros = i;
         }                 
         else if (keyword.CompareNoCase("sectorizationAngle") == 0)
         {
            double w;
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%lg", &w);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.sectorizationAngle = w;
         }
         else if (keyword.CompareNoCase("layerStencilThickness") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Gerbersettings.layerStencilThickness = i;
         }
         continue;
      }
      
      if (keyword.CompareNoCase(".Excellon") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Type") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Excellonsettings.type = i;
         }
         else if (keyword.CompareNoCase("Zero") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Excellonsettings.zeroSuppression = i;
         }
         else if (keyword.CompareNoCase("IncludeDecimalPoint") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.includeDecimalPoint = i;
         }
         else if (keyword.CompareNoCase("Units") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->Excellonsettings.units = i;
         }
         else if (keyword.CompareNoCase("WholeDigits") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.wholeDigits = i;
         }
         else if (keyword.CompareNoCase("FractionDigits") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.significantDigits = i;
         }
         else if (keyword.CompareNoCase("DrillTypes") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.drillTypes = i;
         }
         else if (keyword.CompareNoCase("ProbeDrillSurface") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.probeDrillSurface = i;
         }
         else if (keyword.CompareNoCase("IncludePilotHoles") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.includePilotHoles = i;
         }
         else if (keyword.CompareNoCase("IncludeToolingHoles") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
					this->Excellonsettings.includeToolingHoles = i;
         }
         continue;
      }
      
      if (keyword.CompareNoCase(".Extension") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL," ");
         if (keyword.IsEmpty())   
				continue;

			// get token value
			// verify token value is present
			CString valueKey = strtok(NULL, " =\n");
			if (valueKey.IsEmpty())   
				continue;

         if (keyword.CompareNoCase("Gerber") == 0)
            this->Fileextensions.Gerber = valueKey;
         
			else if (keyword.CompareNoCase("HPGL") == 0)
            this->Fileextensions.HPGL = valueKey;
         
			else if (keyword.CompareNoCase("EXCELLON") == 0)
            this->Fileextensions.EXCELLON = valueKey;
         
			else if (keyword.CompareNoCase("IGES") == 0)
            this->Fileextensions.IGES = valueKey;
         
			else if (keyword.CompareNoCase("APERTURE") == 0)
            this->Fileextensions.APERTURE = valueKey;

         continue;
      }

      if (keyword.CompareNoCase(".HPGL") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("WHITESPACE") == 0)
         {
				// get token value
				// verify token value is present
				keyword = strtok(NULL, " =\n");
				if (!keyword.IsEmpty())
               this->Hpsettings.HPspace = atoi(keyword);
         }
         continue;
      }
      
      if (keyword.CompareNoCase(".IPC") == 0)
      {
         keyword = strtok(NULL, " ");
         if (keyword.CompareNoCase("Units") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->IPCsettings.Units = i;
         }
         else if (keyword.CompareNoCase("Format") == 0)
         {
            keyword = strtok(NULL, " =\n");
            err=sscanf(keyword, "%d", &i);
            if ((err != EOF) && (err != 0))
               this->IPCsettings.IPC_Format = i;
         }

         continue;
      }

      if (keyword.CompareNoCase(".HPPen") == 0)
      {
         int i;
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())
				continue;

			i = atoi(keyword);
         if (i < 1 || i > MAX_HPPENS)
				continue;

			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())
				continue;
			this->Hpsettings.HpPens[i-1].width = atof(keyword);

			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())
				continue;
			this->Hpsettings.HpPens[i-1].layer = keyword;

			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())
				continue;			
			this->Hpsettings.HpPens[i-1].color = atol(keyword);
         continue;
      }
      
      if (keyword.CompareNoCase(".NotePad") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;
			NotePadProgram = keyword;
         continue;
      }

      if (keyword.CompareNoCase(".LogReader") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;
			LogReaderProgram = keyword;
         continue;
      }

      if (keyword.CompareNoCase(".WriteVerificationLog") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;

			// we want to write the verification log only if the value
			// is: Yes, True, or 1
			keyword.MakeLower();
         m_writeVerificationLog = keyword[0] == 't' || keyword[0] == 'y' || keyword[0] == '1';
         continue;
      }

		if (keyword.CompareNoCase(".FIXTURE_REUSE_FILE_TYPE") == 0)
		{
			keyword = strtok(NULL, " \t\n");
			if (keyword.IsEmpty())
				continue;

			if (keyword.CompareNoCase("CAMCAD") == 0)
				FixtureReuseFileType = 0;
			else if (keyword.CompareNoCase("AGILENT") == 0)
				FixtureReuseFileType = 1;
		}

      if (keyword.CompareNoCase(".ShowFillsDefault") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;

			// we want to write the verification log only if the value
			// is: Yes, True, or 1
			keyword.MakeLower();
         setShowFillsDefault(keyword[0] == 't' || keyword[0] == 'y' || keyword[0] == '1');
         continue;
      }

      if (keyword.CompareNoCase(".ShowApFillsDefault") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;

			// we want to write the verification log only if the value
			// is: Yes, True, or 1
			keyword.MakeLower();
         setShowApFillsDefault(keyword[0] == 't' || keyword[0] == 'y' || keyword[0] == '1');
         continue;
      }

      if (keyword.CompareNoCase(".ShowAttribsDefault") == 0)
      {
			// get token value
			// verify token value is present
			keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;

			// we want to write the verification log only if the value
			// is: Yes, True, or 1
			keyword.MakeLower();
         setShowAttribsDefault(keyword[0] == 't' || keyword[0] == 'y' || keyword[0] == '1');
         continue;
      }

      if (keyword.CompareNoCase(".InsertTypesFilterDefault") == 0)
      {
         if (numPar > 1 && (! params.GetAt(1).IsEmpty()))
         {
            m_insertTypeFilterDefault.setFromString(params.GetAt(1),",");
         }

         continue;
      }

      if (keyword.CompareNoCase(".GraphicsClassFilterDefault") == 0)
      {
         if (numPar > 1 && (! params.GetAt(1).IsEmpty()))
         {
            m_graphicsClassFilterDefault.setFromString(params.GetAt(1),",");
         }

         continue;
      }
      else if (keyword.CompareNoCase(".NormalizeRefnameRotations") == 0)
      {
         keyword = strtok(NULL, " \t\n");
         if (keyword.IsEmpty())   
				continue;

         setNormalizeRefNameRotations(0 == keyword.CompareNoCase("y"));
         continue;
      }
   }

   fclose(stream);

   // update current settings
   int u = GetDecimals(this->getPageUnits());
   CEdit *CurrentTextHeight = (CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT);
   CString  buf;
   buf.Format("%.*lf", u, this->Current_TextSize);
   CurrentTextHeight->SetWindowText(buf);
}

void SettingsStruct::SaveSettings(CString Filename)
{
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
   FILE *stream;

   stream = fopen(Filename, "w+t");

   if (stream == NULL)
   {
      ErrorMessage("Unable to open file for writing.\nMake sure it is not Read-Only.", 
         "Unable to Save Settings");
      return;
   }

   fputs("; Begin CAMCAD Settings File\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; FILL STYLE\n", stream);
   fputs("; This is the Fill Style for filled poly features\n", stream);
   fputs("; and Apertures\n", stream);
   fputs("; -1 => SOLID, 0 => ===, 1 => |||,\n", stream);
   fputs(";  2 => \\\\\\, 3 => ///, 4 => +++, 5 => XXX\n", stream);
   fprintf(stream,".FillStyle = %d\n", this->FillStyle);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; CAMCAD COLORS\n", stream);
   fputs("; This section defines the coloring of the CAMCAD\n", stream);
   fputs("; environment. In CAMCAD, go to\n", stream);
   fputs("; Settings | Modify Settings to change\n", stream);
   fprintf(stream,".Color Background = %ld\n", this->Bckgrnd);
   fprintf(stream,".Color Page = %ld\n", this->Foregrnd);
   fprintf(stream,".Color CrossHairs = %ld\n", this->CrossColor);
   fprintf(stream,".Color Grid = %ld\n", this->GridColor);
   fprintf(stream,".Color UndefinedApertures = %ld\n", this->UndefinedColor);
   fprintf(stream,".Color Tools = %ld\n", this->ToolColor);
   fprintf(stream,".Color UndefinedTools = %ld\n", this->UndefToolColor);
   fprintf(stream,".Color Selected = %ld\n", this->SelectColor);
   fprintf(stream,".Color Marked = %ld\n", this->MarkedColor);
   fprintf(stream,".Color Highlighted = %ld\n", this->HighlightColor);
   fprintf(stream,".Color HideDetail = %ld\n", this->HideDetailColor);
   fprintf(stream,".Color TestAccess = %ld\n", this->TestAccessColor);
	fprintf(stream,".Color NavHighlight1 = %ld\n", frame->getNavigator().highlightColors[0]);
   fprintf(stream,".Color NavHighlight2 = %ld\n", frame->getNavigator().highlightColors[1]);
   fprintf(stream,".Color NavHighlight3 = %ld\n", frame->getNavigator().highlightColors[2]);
   fprintf(stream,".Color NavHighlight4 = %ld\n", frame->getNavigator().highlightColors[3]);
   fprintf(stream,".Color NavHighlight5 = %ld\n", frame->getNavigator().highlightColors[4]);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; PAGE UNITS\n", stream);
   fputs("; This is the Default project page units.\n", stream);
   fputs("; (0-INCHES 1-MILS 2-MM 3-HPPLOTTER 4-TNMETER)\n", stream);
   fprintf(stream,".Units = %d\n", this->getPageUnits());
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; DECIMALS\n", stream);
   fputs("; Decimal Places shown in CAMCAD for specific units.\n", stream);
   fputs("; Example 6 decimal looks like -> 0.000000\n", stream);
   fprintf(stream,".Decimals INCH = %d\n", GetDecimals(UNIT_INCHES));
   fprintf(stream,".Decimals MIL = %d\n", GetDecimals(UNIT_MILS));
   fprintf(stream,".Decimals MM = %d\n", GetDecimals(UNIT_MM));
   fprintf(stream,".Decimals HPPLOTTER = %d\n", GetDecimals(UNIT_HPPLOTTER));
   fprintf(stream,".Decimals TNMETER = %d\n", GetDecimals(UNIT_TNMETER));
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; PAGE SIZE\n", stream);
   fputs("; Min and Max coordinates of a page.\n", stream);
   fputs("; Without loading a design into CAMCAD, the page will\n", stream);
   fputs("; be the size defined here. When a design is loaded\n", stream);
   fputs("; CAMCAD will auto fit the page to the design.\n", stream);
   fprintf(stream,".PageSize Xmin = %lg\n", this->getXmin());
   fprintf(stream,".PageSize Xmax = %lg\n", this->getXmax());
   fprintf(stream,".PageSize Ymin = %lg\n", this->getYmin());
   fprintf(stream,".PageSize Ymax = %lg\n", this->getYmax());
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; TRANSLUCENT\n", stream);
   fputs("; Translucent Mode Flag\n", stream);
   fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Translucent On = %d\n", this->Translucent);
   fputs("; Translucent Fill Mode\n", stream);
   fprintf(stream,".Translucent FillMode = %d\n", this->ColorMixMode);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; GRID & SNAP SETTINGS\n", stream);
   fputs("; Show Grid in Project Window?\n", stream);
   fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Grid Show = %d\n", this->Grid);
   fputs("; Snap to Grid On?\n", stream);
   fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Grid Snap = %d\n", this->Snap);
   fputs("; Grid Style (1-Dots 0-Lines).\n", stream);
   fprintf(stream,".Grid Style = %d\n", this->GridStyle);
   fprintf(stream,".Grid SpaceX = %lg\n", this->GridSpaceX);
   fprintf(stream,".Grid SpaceY = %lg\n", this->GridSpaceY);
   fprintf(stream,".Grid OffsetX = %+lg\n", this->GridOffX);
   fprintf(stream,".Grid OffsetY = %+lg\n", this->GridOffY);
   fprintf(stream,".Snap SpaceX = %lg\n", this->SnapSpaceX);
   fprintf(stream,".Snap SpaceY = %lg\n", this->SnapSpaceY);
   fprintf(stream,".Snap OffsetX = %+lg\n", this->SnapOffX);
   fprintf(stream,".Snap OffsetY = %+lg\n", this->SnapOffY);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; CROSSHAIRS\n", stream);
   fputs("; Show X and Y Axes in Draw Window\n", stream);
   fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Show Crosshairs = %d\n", this->Crosshairs);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; POINT ENTITIES\n", stream);
   fputs("; Show Point Entities\n", stream);
   fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Show ShowPoints = %d\n", this->ShowPoints);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; SMD ATTRIBUTE ASSIGNMENT\n", stream);
   fputs("; 0-nothing, 1-all pins SMD, 2-majority of pins SMD\n", stream);
   fputs("; 3-at least one pin SMD\n", stream);
   fprintf(stream,".ComponentSMDrule = %d\n", this->ComponentSMDrule);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; UNDEFINED APERTURES\n", stream);
   fputs("; Undefined Apertures: Size? Draw? DrawNumber?\n", stream);
   fputs("; ApSize is in Pixels\n", stream);
   fprintf(stream,".UndefinedApSize = %d\n", this->UndefinedApSize);
   fputs("; Show? <0-NO 1-YES\n", stream);
   fprintf(stream,".UndefinedApertures Show = %d\n", this->ShowUndefined);
   fputs("; ShowNumbers? <0-NO 1-YES\n", stream);
   fprintf(stream,".UndefinedApertures ShowNumbers = %d\n", this->ShowUndefinedNums);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; SEARCH RANGE\n", stream);
   fputs("; Margin of Error in Device Units when\n", stream);
   fputs("; Searching for Entities.\n", stream);
   fputs("; Value of this command in pixels.\n", stream);
   fprintf(stream,".SearchRange = %d\n", this->SearchRange);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; SMALL WIDTH\n", stream);
   fputs("; Actual size of pen/aperture for entities\n", stream);
   fputs("; drawn with zero width.\n", stream);
   fprintf(stream,".SmallWidth = %lg\n", this->getSmallWidth());
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; CURRENT TEXT SIZE\n", stream);
   fputs("; Default Text size for Current Settings.\n", stream);
   fprintf(stream,".CurrentTextSize = %lg\n", this->Current_TextSize);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; TEST ACCESS SIZE\n", stream);
   fputs("; Test Access Marker Size in Device Units.\n", stream);
   fprintf(stream,".TestAccessSize = %d\n", this->TestAccessSize);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; CENTROID MARKER SIZE\n", stream);
   fputs("; Centroid Marker Size.\n", stream);
   fprintf(stream,".CentroidSize = %lg\n", this->CentroidSize);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; SNAP CURSOR SIZE\n", stream);
   fputs("; Size of snap cursor in pixels.\n", stream);
   fprintf(stream,".SnapCursor = %d\n", this->SnapCursor);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; EXTENTS MARGIN\n", stream);
   fputs("; Percent of Drawing for margin when Zoom Extents.\n", stream);
   fprintf(stream,".ExtentsMargin = %d%%\n", this->ExtentMargin);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; COMPONENT EXTENTS MARGIN\n", stream);
   fputs("; Percent of Drawing for margin when Zoom to\n", stream);
   fputs("; Component Extents.\n", stream);
   fprintf(stream,".CompExtentsMargin = %lg%%\n", this->CompExtentMargin);
   fputs(";-----------------------------------------------------;\n", stream);
	fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; MINIMUM THRU-HOLE DRILL SIZE\n", stream);
   fputs("; Minimum Drill Size (in inches) used to determine if a feature is a thru-hole or SMD.\n", stream);
   fprintf(stream,".MinTHDrillSize = %lg\n", this->m_dMinimumTHDrillSize);
   fputs(";-----------------------------------------------------;\n", stream);
	fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; HIDE DETAIL\n", stream);
   fputs("; Hide Detail by not drawing small inserts.\n", stream);
   fputs("; <0-NO 1-YES - Default = 0\n", stream);
   fprintf(stream,".HideDetail = %d\n", this->HideDetail);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; HIDE DETAIL RATIO\n", stream);
   fputs("; Percent of Page Size to not draw small inserts.\n", stream);
   fprintf(stream,".HideDetailRatio = %lg\n", this->HideDetailRatio);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; TEXT SPACING\n", stream);
   fputs("; Percent of Character Width for Space.\n", stream);
   fprintf(stream,".TextSpaceRatio = %d%%\n", this->TextSpaceRatio);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; VERIFICATION LOG FLAG\n", stream);
   fputs("; True/False flags whether or not to write the verification log.\n", stream);
   fputs("; Default is false.\n", stream);
   fprintf(stream,".WriteVerificationLog %s\n", this->m_writeVerificationLog ? "TRUE" : "FALSE");
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; REDLINING\n", stream);
   fputs("; Default Redline Text Height\n", stream);
   fprintf(stream,".Redlining TextHeight = %lg\n", this->RL_textHeight);
   fputs("; Default Redline Arrow Size\n", stream);
   fprintf(stream,".Redlining ArrowSize = %lg\n", this->RL_arrowSize);
   fprintf(stream,".Redlining WidthSmall = %lg\n", this->RL_WS);
   fprintf(stream,".Redlining WidthMedium = %lg\n", this->RL_WM);
   fprintf(stream,".Redlining WidthLarge = %lg\n", this->RL_WL);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; PIN LABELS\n", stream);
   fputs("; Nominal Text Height (Mils)\n", stream);
   fprintf(stream,".Pinlabels TextHeightMils = %lg\n",getPinLabelSettings().getTextHeightInches()*1000.);
   fputs("; Minimum Text Height (Pixels)\n", stream);
   fprintf(stream,".Pinlabels MinTextHeightPixels = %d\n",getPinLabelSettings().getMinTextHeightPixels());
   fputs("; Maximum Text Height (Pixels)\n", stream);
   fprintf(stream,".Pinlabels MaxTextHeightPixels = %d\n",getPinLabelSettings().getMaxTextHeightPixels());
   fputs("; Text Box Height (Pixels)\n", stream);
   fprintf(stream,".Pinlabels TextBoxPixels = %d\n",getPinLabelSettings().getTextBoxPixels());
   fputs("; Font Face Name\n", stream);
   fprintf(stream,".Pinlabels FontFaceName = '%s'\n",(const char*)getPinLabelSettings().getFontFaceName());
   fputs("; Text Angle (Radians)\n", stream);
   fprintf(stream,".Pinlabels TextAngle = %lg\n", getPinLabelSettings().getTextDegrees());
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; JOIN LINES\n", stream);
   fputs("; Chamfer Size\n", stream);
   fprintf(stream,".JoinLines Chamfer = %lg\n", this->JOIN_chamfer);
   fputs("; Radius Size\n", stream);
   fprintf(stream,".JoinLines Radius = %lg\n", this->JOIN_radius);
   fputs("; Auto Join Snap Size\n", stream);
   fprintf(stream,".JoinLines AutoSnapSize = %lg\n", this->JOIN_autoSnapSize);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; DRC ARROW SIZE\n", stream);
   fprintf(stream,".DRC_ARROW_MAXSIZE = %lg\n", this->DRC_MAX_ARROWSIZE);
   fprintf(stream,".DRC_ARROW_MINSIZE = %lg\n", this->DRC_MIN_ARROWSIZE);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; ATTRIBS\n", stream);
	fputs("; Show Invisible Attribs?\n", stream);
	fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Show AllAttribs = %d\n", this->ShowAllAttribs);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; OVERRIDES\n", stream);
	fputs("; Always show override regardless of visibilty settings?\n", stream);
	fputs("; <0-NO 1-YES\n", stream);
   fprintf(stream,".Show Overrides = %d\n", this->AlwaysShowOverrides);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; DIRECTORY\n", stream);
   fputs("; Hard Coded Starting Directory\n", stream);
   fputs("; Blank is default which uses current working\n", stream);
   fputs("; directory.\n", stream);
   fprintf(stream,".Directory Macro = %s\n", this->getDirectoryMacro());
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   // Navigator Options are Global settings. Effort is made elsewhere to keep Local settings
   // in sync with Global settings, but there are sequences of events that can get them out
   // of sync anyway. Navigator itself always acts on the Global settings.
   // So upon Save Settings, we re-sync them here too, to ensure save of what the user is seeing.
   this->navigatorZoomMode = GlSettings.navigatorZoomMode;
   this->navigatorFlipView = GlSettings.navigatorFlipView;
   this->navigatorShowSide = GlSettings.navigatorShowSide;
   this->navigatorShowType = GlSettings.navigatorShowType;

   fputs("; NAVIGATOR\n", stream);
   fputs("; Zoom/Pan mode, values Zoom or Pan\n", stream);
   fprintf(stream,".NavigatorZoomPan = %s\n", this->navigatorZoomMode ? "Zoom" : "Pan");
   fputs("; Flip to component side upon selection, Yes or No\n", stream);
   fprintf(stream,".NavigatorFlipView = %s\n", this->navigatorFlipView ? "Yes" : "No");
   fputs("; Show insert side next to Refname in list box, Yes or No\n", stream);
   fprintf(stream,".NavigatorShowSide = %s\n", this->navigatorShowSide ? "Yes" : "No");
   //not used yet  fputs("; Show item type next to Refname in list box, Yes or No\n", stream);
   //not used yet  fprintf(stream,".NavigatorShowType = %s\n", this->navigatorShowType ? "Yes" : "No");
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; DXF SETTINGS\n",stream);
   fputs("; Settings for DXF Format dialog during DXF Import\n", stream);
   fprintf(stream,".DXF CIRCLES_2_APS %d\n",this->DXFsettings.Circles_2_Aps);
   fprintf(stream,".DXF DONUTS_2_APS %d\n",this->DXFsettings.Donuts_2_Aps);
   fprintf(stream,".DXF PROPORTIONAL %d\n",this->DXFsettings.Proportional);
   fputs("; DXF Polyends <0=None 1=Round\n",stream);
   fprintf(stream,".DXF POLYENDS %d\n",this->DXFsettings.PolyEnds);
   fputs("; DXF Units\n",stream);
   fputs("; <0-INCHES 1-MILS 2-MM 3-HPPLOTTER 4-TNMETER\n",stream);
   fprintf(stream,".DXF Units %d\n",this->DXFsettings.Units);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; GERBER SETTINGS\n",stream);
   fputs("; Gerber Format <0=RS-274 1=RS-274-X\n",stream);
   fprintf(stream,".GERBER Format %d\n",this->Gerbersettings.format);
   fputs("; Gerber Type <0=Absolute 1=Incremental\n",stream);
   fprintf(stream,".GERBER Type %d\n",this->Gerbersettings.type);
   fputs("; Gerber Zero <0=Leading 1=Trailing 2=None\n",stream);
   fprintf(stream,".GERBER Zero %d\n",this->Gerbersettings.zero);
   fputs("; Gerber Units <0=English 1=Metric\n",stream);
   fprintf(stream,".GERBER Units %d\n",this->Gerbersettings.units);
   fputs("; Gerber Digits\n",stream);
   fprintf(stream,".GERBER Digits %d\n",this->Gerbersettings.digits);
   fputs("; Gerber Decimals\n",stream);
   fprintf(stream,".GERBER Decimal %d\n",this->Gerbersettings.decimal);
   fputs("; Gerber CircleMode\n",stream);
	fputs("; <0=Sectorize 1=360° circluar interpolation\n",stream);
   fprintf(stream,".GERBER CircleMode %d\n",this->Gerbersettings.circleMode);
   fputs("; Gerber PolyFill\n",stream);
   fprintf(stream,".GERBER PolyFill %d\n",this->Gerbersettings.PolyFill);
   fputs("; Gerber ThermalMacros\n",stream);
   fprintf(stream,".GERBER ThermalMacros %d\n",this->Gerbersettings.thermalMacros);
   fputs("; Gerber FillDcode\n",stream);
   fprintf(stream,".GERBER FillDcode %d\n",this->Gerbersettings.fillDCode);
   fputs("; Gerber SectorizationAngle\n",stream);
   fprintf(stream,".GERBER SectorizationAngle %lg\n",this->Gerbersettings.sectorizationAngle);
   fputs("; Gerber Layer with StencilThickness\n",stream);
   fprintf(stream,".GERBER layerStencilThickness %d\n",this->Gerbersettings.layerStencilThickness);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; EXCELLON SETTINGS\n",stream);
   fputs("; Excellon Type <0=Absolute 1=Incremental\n",stream);
   fprintf(stream,".EXCELLON Type %d\n",this->Excellonsettings.type);
   fputs("; Excellon Zero <0=Leading 1=Trailing 2=None\n",stream);
   fprintf(stream,".EXCELLON Zero %d\n",this->Excellonsettings.zeroSuppression);
   fputs("; Excellon IncludeDecimalPoint <0=No 1=Yes\n",stream);
	fprintf(stream,".EXCELLON IncludeDecimalPoint %d\n",this->Excellonsettings.includeDecimalPoint);
   fputs("; Excellon Units <0=English 1=Metric\n",stream);
   fprintf(stream,".EXCELLON Units %d\n",this->Excellonsettings.units);
   fputs("; Excellon Whole Digits\n",stream);
	fprintf(stream,".EXCELLON WholeDigits %d\n",this->Excellonsettings.wholeDigits);
   fputs("; Excellon Fraction Digits\n",stream);
	fprintf(stream,".EXCELLON FractionDigits %d\n",this->Excellonsettings.significantDigits);
   fputs("; Excellon Drill Types <0=PCB Drills 1=Probe Drills\n",stream);
	fprintf(stream,".EXCELLON DrillTypes %d\n",this->Excellonsettings.drillTypes);
   fputs("; Excellon Probe Drill Surface <0=Top 1=Bottom 2=Both\n",stream);
	fprintf(stream,".EXCELLON ProbeDrillSurface %d\n",this->Excellonsettings.probeDrillSurface);
   fputs("; Excellon Include Pilot Holes < 0=No 1=Yes\n",stream);
	fprintf(stream,".EXCELLON IncludePilotHoles %d\n",this->Excellonsettings.includePilotHoles);
   fprintf(stream,".EXCELLON IncludeToolingHoles %d\n",this->Excellonsettings.includeToolingHoles);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; HPGL : Is Whitespace a delimeter\n", stream);
   fprintf(stream,".HPGL WHITESPACE %d\n",this->Hpsettings.HPspace);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; HP FORMAT IMPORT SETTINGS\n", stream);
   fputs("; HP Pen Width (in MM) HP_Pen_Layer_Name Pen_Color\n", stream);
   for (int i=0;i<MAX_HPPENS;i++)
      fprintf(stream,".HPPEN %d %lg %s %ld\n",
         i+1,this->Hpsettings.HpPens[i].width,this->Hpsettings.HpPens[i].layer,
         this->Hpsettings.HpPens[i].color);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; FILE EXTENSIONS (when browsing for files)\n",stream);
   fprintf(stream,".EXTENSION GERBER %s\n",this->Fileextensions.Gerber);
   fprintf(stream,".EXTENSION HPGL %s\n",this->Fileextensions.HPGL);
   fprintf(stream,".EXTENSION EXCELLON %s\n",this->Fileextensions.EXCELLON);
   fprintf(stream,".EXTENSION IGES %s\n",this->Fileextensions.IGES);
   fprintf(stream,".EXTENSION APERTURE %s\n",this->Fileextensions.APERTURE);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; IPC SETTINGS\n",stream);
   fputs("; IPC Format Types <0=IPC 350, 1=IPC 356, 2=IPC-D-356A\n",stream);
   fputs("; IPC Units <0=Inches, 1=Mils, 2=Millimeters\n",stream);
   fprintf(stream,".IPC Format %d\n",this->IPCsettings.IPC_Format);
   fprintf(stream,".IPC Units %d\n",this->IPCsettings.Units);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; PROGRAM CALLS\n",stream);
   fprintf(stream,".NOTEPAD %s\n", NotePadProgram);
   fprintf(stream,".LOGREADER %s\n", LogReaderProgram);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; FIXTURE REUSE\n",stream);
	fprintf(stream,".FIXTURE_REUSE_FILE_TYPE %s\n", this->FixtureReuseFileType == 1 ? "AGILENT" : "CAMCAD");

   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs("; Default Document Settings\n",stream);
	fprintf(stream,".ShowFillsDefault %d\n"   ,this->getShowFillsDefault());
	fprintf(stream,".ShowApFillsDefault %d\n" ,this->getShowApFillsDefault());
	fprintf(stream,".ShowAttribsDefault %d\n",this->getShowAttribsDefault());
	fprintf(stream,".InsertTypesFilterDefault '%s'\n"  ,this->getInsertTypeFilterDefault().getSetString(","));
	fprintf(stream,".GraphicsClassFilterDefault '%s'\n",this->getGraphicsClassFilterDefault().getSetString(","));

   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs(";-----------------------------------------------------;\n", stream);
   fputs(";\n", stream);
   fputs(";\n", stream);
   fputs("; End CAMCAD Settings File\n", stream);

   fclose(stream);
}

double SettingsStruct::getDistanceTolerance(PageUnitsTag pageUnit)
{ 
	return getUnitsFactor(pageUnitsCentiMicroMeters, pageUnit) * m_dDistanceTolerance;	
};


/******************************************************************************
* OnFitPageToImage
*/
void CCEtoODBDoc::OnFitPageToImage() 
{
   static bool testFlag = true;

   if (testFlag)
   {
      resetBlockExtents();
   }

   CExtent extent = getExtent();

   double h, w;
   h = extent.getYsize();
   w = extent.getXsize();

   double xMargin = 0.;
   double yMargin = 0.;

   if (h == 0. || (w/h) > 4) // if greater than 4:1, then make it 3:2
   {
      double newH = w * 2/3;
      yMargin = (newH - h);
   }
   else if (w == 0. || (h/w) > 4) // if greater than 4:1, then make it 3:2
   {
      double newW = h * 2/3;
      xMargin = (newW - w);
   }

   extent.expand(xMargin,yMargin);

   SetPageSize(this,extent.getXmin(),extent.getXmax(),extent.getYmin(),extent.getYmax());
}

void SetPageSize(CCEtoODBDoc *doc, double xmin, double xmax, double ymin, double ymax)
{
   if (xmin < xmax && ymin < ymax) 
   {
      // include origin point in size
      double margin;
      margin = 0.01 * doc->getSettings().ExtentMargin * (xmax - xmin);
      if (xmin > 0.0) xmin = -margin;
      if (xmax < 0.0) xmax = margin;
      margin = 0.01 * doc->getSettings().ExtentMargin * (ymax - ymin);
      if (ymin > 0.0) ymin = -margin;
      if (ymax < 0.0) ymax = margin;

      doc->getSettings().setXmax(xmax);
      doc->getSettings().setYmax(ymax);
      doc->getSettings().setXmin(xmin);
      doc->getSettings().setYmin(ymin);
      doc->SetMaxCoords();
      POSITION viewPos = doc->GetFirstViewPosition();
      while (viewPos)
      {
         CCEtoODBView *view = (CCEtoODBView*)doc->GetNextView(viewPos);

         view->ScaleNum = 1;
         view->ScaleDenom = 1.0;
         view->SetScroll();
      }
   }

   doc->UpdateAllViews(NULL);
}

/******************************************************************************
* OnChangeSettings
*/
void CCEtoODBDoc::OnChangeSettings() 
{
   doc = this;
   CPropertySheet propSheet("Settings");
   SettingsGrid gridPage;
   SettingsAperture aperturePage;
   SettingsPage pagePage;
   SettingsView viewPage;
   SettingsColor colorsPage;
   SettingsRedline redlinePage;
   SettingsSystem systemPage;
   CPinLabelSettingsPropertyPage pinLabelSettingsPage;

   propSheet.AddPage(&pagePage);
   propSheet.AddPage(&aperturePage);
   propSheet.AddPage(&gridPage);
   propSheet.AddPage(&viewPage);
   propSheet.AddPage(&colorsPage);
   propSheet.AddPage(&redlinePage);
   propSheet.AddPage(&systemPage);
   propSheet.AddPage(&pinLabelSettingsPage);

   pagePage.units = getSettings().getPageUnits();
   pagePage.m_Decimals = GetDecimals(getSettings().getPageUnits());
   int oldPageUnitsDecimals = pagePage.m_Decimals;
   pinLabelSettingsPage.setPageUnits(getSettings().getPageUnits());
   pinLabelSettingsPage.setPinLabelSettings(getSettings().getPinLabelSettings());

   gridPage.m_dots = !getSettings().GridStyle;
   gridPage.m_showGrid = getSettings().Grid;
   gridPage.m_snapOn = getSettings().Snap;
   gridPage.m_XGridOffset.Format("%.*lf", pagePage.m_Decimals, getSettings().GridOffX);
   gridPage.m_XGridSpace.Format("%.*lf", pagePage.m_Decimals, getSettings().GridSpaceX);
   gridPage.m_XSnapOffset.Format("%.*lf", pagePage.m_Decimals, getSettings().SnapOffX);
   gridPage.m_XSnapSpace.Format("%.*lf", pagePage.m_Decimals, getSettings().SnapSpaceX);
   gridPage.m_YGridOffset.Format("%.*lf", pagePage.m_Decimals, getSettings().GridOffY);
   gridPage.m_YGridSpace.Format("%.*lf", pagePage.m_Decimals, getSettings().GridSpaceY);
   gridPage.m_YSnapOffset.Format("%.*lf", pagePage.m_Decimals, getSettings().SnapOffY);
   gridPage.m_YSnapSpace.Format("%.*lf", pagePage.m_Decimals, getSettings().SnapSpaceY);

   aperturePage.m_UApSize = getSettings().UndefinedApSize;
   aperturePage.m_ShowApNum = getSettings().ShowUndefinedNums;
   aperturePage.m_ShowUAp = getSettings().ShowUndefined;

   pagePage.m_maxX.Format("%.*lf", pagePage.m_Decimals, getSettings().getXmax());
   pagePage.m_maxY.Format("%.*lf", pagePage.m_Decimals, getSettings().getYmax());
   pagePage.m_minX.Format("%.*lf", pagePage.m_Decimals, -getSettings().getXmin());
   pagePage.m_minY.Format("%.*lf", pagePage.m_Decimals, -getSettings().getYmin());
   Initialized = FALSE;

   systemPage.m_searchRange = getSettings().SearchRange;
   systemPage.m_testAccessSize = getSettings().TestAccessSize;
   systemPage.m_join_chamfer.Format("%.*lf", pagePage.m_Decimals, getSettings().JOIN_chamfer);
   systemPage.m_join_radius.Format("%.*lf", pagePage.m_Decimals, getSettings().JOIN_radius);
   systemPage.m_join_autoSnapSize.Format("%.*lf", pagePage.m_Decimals, getSettings().JOIN_autoSnapSize);

   redlinePage.m_textHeight.Format("%.*lf", pagePage.m_Decimals, getSettings().RL_textHeight);
   redlinePage.m_arrowSize.Format("%.*lf", pagePage.m_Decimals, getSettings().RL_arrowSize);

   viewPage.m_margin = getSettings().ExtentMargin;
   viewPage.m_margin2 = getSettings().CompExtentMargin;
   viewPage.m_hideDetail = getSettings().HideDetail;
   viewPage.m_hideDetailRatio = getSettings().HideDetailRatio;
   viewPage.m_translucent = getSettings().Translucent;
   viewPage.m_showOrigin = getSettings().Crosshairs;
	viewPage.m_showPoints = getSettings().ShowPoints;
	viewPage.m_alwaysShowOverrides = getSettings().AlwaysShowOverrides;
   viewPage.m_showHiddenAttributes = getShowHiddenAttributes();

   switch (getSettings().ColorMixMode) 
   {
      case R2_XORPEN:
         viewPage.mixMode = "xor pen";
        break;
      case R2_NOTXORPEN:
         viewPage.mixMode = "not xor pen";
        break;
      case R2_MASKPEN:
         viewPage.mixMode = "mask pen";
        break;
      case R2_MASKPENNOT:
         viewPage.mixMode = "mask pen not";
        break;
      case R2_MASKNOTPEN:
         viewPage.mixMode = "mask not pen";
      break;
      case R2_NOTMASKPEN:
         viewPage.mixMode = "not mask pen";
        break;
      case R2_MERGEPEN:
         viewPage.mixMode = "merge pen";
        break;
      case R2_MERGEPENNOT:
         viewPage.mixMode = "merge pen not";
      break;
      case R2_MERGENOTPEN:
         viewPage.mixMode = "merge not pen";
        break;
      case R2_NOTMERGEPEN:
         viewPage.mixMode = "not merge pen";
        break;
      case R2_NOT:
         viewPage.mixMode = "not pen";
        break;
   }

   propSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
   propSheet.m_psh.dwFlags &= ~PSH_HASHELP;

   pagePage.m_psp.dwFlags             &= ~PSP_HASHELP;
   gridPage.m_psp.dwFlags             &= ~PSP_HASHELP;
   aperturePage.m_psp.dwFlags         &= ~PSP_HASHELP;
   viewPage.m_psp.dwFlags             &= ~PSP_HASHELP;
   colorsPage.m_psp.dwFlags           &= ~PSP_HASHELP;
   redlinePage.m_psp.dwFlags          &= ~PSP_HASHELP;
   systemPage.m_psp.dwFlags           &= ~PSP_HASHELP;
   pinLabelSettingsPage.m_psp.dwFlags &= ~PSP_HASHELP;

   if (propSheet.DoModal() == IDOK)
   {
      getSettings().GridStyle = !gridPage.m_dots;
      getSettings().Grid = gridPage.m_showGrid;
      getSettings().Snap = gridPage.m_snapOn;
      getSettings().GridOffX = atof(gridPage.m_XGridOffset);
      getSettings().GridSpaceX = atof(gridPage.m_XGridSpace);
      getSettings().SnapOffX = atof(gridPage.m_XSnapOffset);
      getSettings().SnapSpaceX = atof(gridPage.m_XSnapSpace);
      getSettings().GridOffY = atof(gridPage.m_YGridOffset);
      getSettings().GridSpaceY = atof(gridPage.m_YGridSpace);
      getSettings().SnapOffY = atof(gridPage.m_YSnapOffset);
      getSettings().SnapSpaceY = atof(gridPage.m_YSnapSpace);

      getSettings().UndefinedApSize = aperturePage.m_UApSize;
      getSettings().ShowUndefinedNums = aperturePage.m_ShowApNum;
      getSettings().ShowUndefined = aperturePage.m_ShowUAp;

      getSettings().RL_textHeight = atof(redlinePage.m_textHeight);
      getSettings().RL_arrowSize = atof(redlinePage.m_arrowSize);

      getSettings().SearchRange = systemPage.m_searchRange;
      getSettings().TestAccessSize = systemPage.m_testAccessSize;
      getSettings().JOIN_chamfer = atof(systemPage.m_join_chamfer);
      getSettings().JOIN_radius = atof(systemPage.m_join_radius);
      getSettings().JOIN_autoSnapSize = atof(systemPage.m_join_autoSnapSize);

      getSettings().ExtentMargin = viewPage.m_margin;
      getSettings().CompExtentMargin = viewPage.m_margin2;
      getSettings().HideDetail = viewPage.m_hideDetail;
      getSettings().HideDetailRatio = viewPage.m_hideDetailRatio;
      getSettings().Translucent = viewPage.m_translucent;
      getSettings().Crosshairs = viewPage.m_showOrigin;
      getSettings().ShowPoints = viewPage.m_showPoints;
		getSettings().AlwaysShowOverrides = viewPage.m_alwaysShowOverrides;
      getSettings().ColorMixMode = R2_MERGEPEN;
      setShowHiddenAttributes(viewPage.m_showHiddenAttributes != 0);

      if (!viewPage.m_mixMode.Compare("xor pen"))
         getSettings().ColorMixMode = R2_XORPEN;
      else if (!viewPage.m_mixMode.Compare("not xor pen"))
         getSettings().ColorMixMode = R2_NOTXORPEN;
      else if (!viewPage.m_mixMode.Compare("mask pen"))
         getSettings().ColorMixMode = R2_MASKPEN;
      else if (!viewPage.m_mixMode.Compare("mask pen not"))
         getSettings().ColorMixMode = R2_MASKPENNOT;
      else if (!viewPage.m_mixMode.Compare("mask not pen"))
         getSettings().ColorMixMode = R2_MASKNOTPEN;
      else if (!viewPage.m_mixMode.Compare("not mask pen"))
         getSettings().ColorMixMode = R2_NOTMASKPEN;
      else if (!viewPage.m_mixMode.Compare("merge pen"))
         getSettings().ColorMixMode = R2_MERGEPEN;
      else if (!viewPage.m_mixMode.Compare("merge pen not"))
         getSettings().ColorMixMode = R2_MERGEPENNOT;
      else if (!viewPage.m_mixMode.Compare("merge not pen"))
         getSettings().ColorMixMode = R2_MERGENOTPEN;
      else if (!viewPage.m_mixMode.Compare("not merge pen"))
         getSettings().ColorMixMode = R2_NOTMERGEPEN;
      else if (!viewPage.m_mixMode.Compare("not pen"))
         getSettings().ColorMixMode = R2_NOT;

      getSettings().getPinLabelSettings() = pinLabelSettingsPage.getPinLabelSettings();

      if (getSettings().getPageUnits() != pagePage.units ||
         oldPageUnitsDecimals != GetDecimals(pagePage.units))
      {
         // here update settings variables with new scale, so that redline etc... all changes.
			double unitfactor = Units_Factor(getSettings().getPageUnits(), pagePage.units);
         Scale(unitfactor, GetDecimals(pagePage.units));

         getSettings().setXmin(getSettings().getXmin() * unitfactor);
         getSettings().setYmin(getSettings().getYmin() * unitfactor);
         getSettings().setXmax(getSettings().getXmax() * unitfactor);
         getSettings().setYmax(getSettings().getYmax() * unitfactor);

         // update text in Current settings Dialog
         CEdit *CurrentTextHeight = (CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT);
         CString  buf;
         buf.Format("%.*lf", pagePage.m_Decimals, getSettings().Current_TextSize);
         CurrentTextHeight->SetWindowText(buf);

         scale_default_units(&getSettings(), unitfactor);

         getSettings().setPageUnits(intToPageUnitsTag(pagePage.units));

         //pagePage.m_maxX.Format("%.*lf", pagePage.m_Decimals, Settings.Xmax);
         //pagePage.m_maxY.Format("%.*lf", pagePage.m_Decimals, Settings.Ymax);
         //pagePage.m_minX.Format("%.*lf", pagePage.m_Decimals, -Settings.Xmin);
         //pagePage.m_minY.Format("%.*lf", pagePage.m_Decimals, -Settings.Ymin);

			OnRegen();
		}

      // update UNITS on status bar
      if (StatusBar)
         StatusBar->SetPaneText(StatusBar->CommandToIndex(IDS_UNITS), GetUnitName(getSettings().getPageUnits()));

      double resolution = pow(10.0, -GetDecimals(getSettings().getPageUnits()));
      if (fabs(getSettings().getXmax() - fabs(atof(pagePage.m_maxX))) > resolution || 
          fabs(getSettings().getYmax() - fabs(atof(pagePage.m_maxY))) > resolution ||
          fabs(getSettings().getXmin() - -fabs(atof(pagePage.m_minX))) > resolution || 
          fabs(getSettings().getYmin() - -fabs(atof(pagePage.m_minY))) > resolution)
      {
         getSettings().setXmax(fabs(atof(pagePage.m_maxX)));
         if (fabs(getSettings().getXmax()) < SMALLNUMBER)
            getSettings().setXmax(1.0);

         getSettings().setYmax(fabs(atof(pagePage.m_maxY)));
         if (fabs(getSettings().getYmax()) < SMALLNUMBER)
            getSettings().setYmax(1.0);

         getSettings().setXmin(-fabs(atof(pagePage.m_minX)));
         getSettings().setYmin(-fabs(atof(pagePage.m_minY)));

         SetMaxCoords();

         POSITION viewPos = GetFirstViewPosition();
         while (viewPos)
         {
            CCEtoODBView *view = (CCEtoODBView *)GetNextView(viewPos);
            view->ScaleNum = 1;
            view->ScaleDenom = 1.0;
            view->SetScroll();
         }
      }

      UpdateAllViews(NULL);
      //UpdateLayers(this);
   }
}

/////////////////////////////////////////////////////////////////////////////
// SettingsGrid property page
IMPLEMENT_DYNCREATE(SettingsGrid, CPropertyPage)

SettingsGrid::SettingsGrid() : CPropertyPage(SettingsGrid::IDD)
{
   //{{AFX_DATA_INIT(SettingsGrid)
   m_dots = -1;
   m_showGrid = FALSE;
   m_snapOn = FALSE;
   m_XGridOffset = _T("");
   m_XGridSpace = _T("");
   m_XSnapOffset = _T("");
   m_XSnapSpace = _T("");
   m_YGridOffset = _T("");
   m_YGridSpace = _T("");
   m_YSnapOffset = _T("");
   m_YSnapSpace = _T("");
   //}}AFX_DATA_INIT
}

SettingsGrid::~SettingsGrid()
{
}

void SettingsGrid::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SettingsGrid)
   DDX_Radio(pDX, IDC_DOTS, m_dots);
   DDX_Check(pDX, IDC_SHOW_GRID, m_showGrid);
   DDX_Check(pDX, IDC_SNAP_ON, m_snapOn);
   DDX_Text(pDX, IDC_X_GRID_OFFSET, m_XGridOffset);
   DDX_Text(pDX, IDC_X_GRID_SPACE, m_XGridSpace);
   DDX_Text(pDX, IDC_X_SNAP_OFFSET, m_XSnapOffset);
   DDX_Text(pDX, IDC_X_SNAP_SPACE, m_XSnapSpace);
   DDX_Text(pDX, IDC_Y_GRID_OFFSET, m_YGridOffset);
   DDX_Text(pDX, IDC_Y_GRID_SPACE, m_YGridSpace);
   DDX_Text(pDX, IDC_Y_SNAP_OFFSET, m_YSnapOffset);
   DDX_Text(pDX, IDC_Y_SNAP_SPACE, m_YSnapSpace);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SettingsGrid, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsGrid)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SettingsAperture property page
IMPLEMENT_DYNCREATE(SettingsAperture, CPropertyPage)

SettingsAperture::SettingsAperture() : CPropertyPage(SettingsAperture::IDD)
{
   //{{AFX_DATA_INIT(SettingsAperture)
   m_ShowApNum = FALSE;
   m_ShowUAp = FALSE;
   m_UApSize = 0;
   //}}AFX_DATA_INIT
}

SettingsAperture::~SettingsAperture()
{
}

void SettingsAperture::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SettingsAperture)
   DDX_Check(pDX, IDC_SHOW_AP_NUM, m_ShowApNum);
   DDX_Check(pDX, IDC_SHOW_U_AP, m_ShowUAp);
   DDX_Text(pDX, IDC_AP_SIZE, m_UApSize);
   DDV_MinMaxInt(pDX, m_UApSize, 1, 2147483647);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SettingsAperture, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsAperture)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SettingsPage property page
IMPLEMENT_DYNCREATE(SettingsPage, CPropertyPage)

SettingsPage::SettingsPage() : CPropertyPage(SettingsPage::IDD)
{
   //{{AFX_DATA_INIT(SettingsPage)
   m_maxX = _T("");
   m_maxY = _T("");
   m_minX = _T("");
   m_minY = _T("");
   m_Decimals = 0;
   //}}AFX_DATA_INIT
}

SettingsPage::~SettingsPage()
{
}

BOOL SettingsPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   m_spin.SetRange(0, 6);

   for (EnumIterator(PageUnitsTag,unitsIterator);unitsIterator.hasNext();)
   {
      PageUnitsTag unitsTag = unitsIterator.getNext();

      if (isValidCamCadUnit(unitsTag))
      {
         m_UnitsLB.AddString(pageUnitsTagToString(unitsTag));
      }
   }

	m_lastUnit = units;
   m_UnitsLB.SetCurSel(units);
   m_DecimalsEdit.SetReadOnly(TRUE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// save decimals when changed
void SettingsPage::OnChangeDecimals() 
{
   if (!Initialized) // sends EN_CHANGE message when initializing dialog
   {
      Initialized = TRUE;
      return;
   }  

   CString string;
   m_DecimalsEdit.GetWindowText(string);
   SetDecimals(m_UnitsLB.GetCurSel(), atoi(string));
}

// update decimals displayed when change units
void SettingsPage::OnSelchangeUnits() 
{
   UpdateData(); // get new units
   units = m_UnitsLB.GetCurSel();
   m_Decimals = GetDecimals(units);

   m_maxX.Format("%.*lf", m_Decimals, atof(m_maxX) * Units_Factor(m_lastUnit, units));
   m_maxY.Format("%.*lf", m_Decimals, atof(m_maxY) * Units_Factor(m_lastUnit, units));
   m_minX.Format("%.*lf", m_Decimals, -atof(m_minX) * Units_Factor(m_lastUnit, units));
   m_minY.Format("%.*lf", m_Decimals, -atof(m_minY) * Units_Factor(m_lastUnit, units));

	m_lastUnit = units;

	UpdateData(FALSE); // show new decimals
}

void SettingsPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SettingsPage)
   DDX_Control(pDX, IDC_SPIN1, m_spin);
   DDX_Control(pDX, IDC_DECIMALS, m_DecimalsEdit);
   DDX_Control(pDX, IDC_UNITS, m_UnitsLB);
   DDX_Text(pDX, IDC_MAX_X, m_maxX);
   DDX_Text(pDX, IDC_MAX_Y, m_maxY);
   DDX_Text(pDX, IDC_MIN_X, m_minX);
   DDX_Text(pDX, IDC_MIN_Y, m_minY);
   DDX_Text(pDX, IDC_DECIMALS, m_Decimals);
   DDV_MinMaxInt(pDX, m_Decimals, 0, 6);
   //}}AFX_DATA_MAP
}

void SettingsPage::OnFitPageToImage() 
{
   double xmin, xmax, ymin, ymax;

   ASSERT(doc != NULL);
   if (!doc)
		return;
	doc->OnFitPageToImage();

   doc->get_extents(&xmin, &xmax, &ymin, &ymax);
   if (xmin > xmax) return; 

   // include origin point in size
   double margin;
   margin = 0.01 * doc->getSettings().ExtentMargin * (xmax - xmin);
   if (xmin > 0.0) xmin = -margin;
   if (xmax < 0.0) xmax = margin;
   margin = 0.01 * doc->getSettings().ExtentMargin * (ymax - ymin);
   if (ymin > 0.0) ymin = -margin;
   if (ymax < 0.0) ymax = margin;

	units = doc->getSettings().getPageUnits();
	m_lastUnit = units;
   m_UnitsLB.SetCurSel(units);

   m_maxX.Format("%.*lf", m_Decimals, xmax);
   m_maxY.Format("%.*lf", m_Decimals, ymax);
   m_minX.Format("%.*lf", m_Decimals, -xmin);
   m_minY.Format("%.*lf", m_Decimals, -ymin);

   UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(SettingsPage, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsPage)
   ON_EN_CHANGE(IDC_DECIMALS, OnChangeDecimals)
   ON_CBN_SELCHANGE(IDC_UNITS, OnSelchangeUnits)
   ON_BN_CLICKED(IDC_FIT, OnFitPageToImage)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SettingsSystem property page
IMPLEMENT_DYNCREATE(SettingsSystem, CPropertyPage)

SettingsSystem::SettingsSystem() : CPropertyPage(SettingsSystem::IDD)
{
   //{{AFX_DATA_INIT(SettingsSystem)
   m_searchRange = 0;
   m_join_chamfer = _T("");
   m_join_radius = _T("");
   m_join_autoSnapSize = _T("");
   m_testAccessSize = 0;
   //}}AFX_DATA_INIT
}

SettingsSystem::~SettingsSystem()
{
}

void SettingsSystem::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SettingsSystem)
   DDX_Text(pDX, IDC_SEARCH_RANGE, m_searchRange);
   DDV_MinMaxInt(pDX, m_searchRange, 0, 10000);
   DDX_Text(pDX, IDC_JOIN_CHAMFER, m_join_chamfer);
   DDX_Text(pDX, IDC_JOIN_RADIUS, m_join_radius);
   DDX_Text(pDX, IDC_JOIN_AUTO_SNAP_SIZE, m_join_autoSnapSize);
   DDX_Text(pDX, IDC_TEST_ACCESS_SIZE, m_testAccessSize);
   DDV_MinMaxInt(pDX, m_testAccessSize, 0, 10000);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SettingsSystem, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsSystem)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////
// SettingsView property page
IMPLEMENT_DYNCREATE(SettingsView, CPropertyPage)
SettingsView::SettingsView() : CPropertyPage(SettingsView::IDD), m_showHiddenAttributes(FALSE)
{
   //{{AFX_DATA_INIT(SettingsView)
   m_margin = 0;
   m_translucent = FALSE;
   m_mixMode = _T("");
   m_showOrigin = FALSE;
   m_hideDetailRatio = 0.0;
   m_hideDetail = FALSE;
   m_margin2 = 0.0;
   //}}AFX_DATA_INIT
}

SettingsView::~SettingsView()
{
}

void SettingsView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SettingsView)
	DDX_Control(pDX, IDC_FILL_MODE, m_mixModeCB);
	DDX_Text(pDX, IDC_MARGIN, m_margin);
	DDV_MinMaxInt(pDX, m_margin, 0, 500);
	DDX_Check(pDX, IDC_TRANSLUCENT, m_translucent);
	DDX_CBString(pDX, IDC_FILL_MODE, m_mixMode);
	DDX_Check(pDX, IDC_SHOW_ORIGIN, m_showOrigin);
	DDX_Text(pDX, IDC_HIDE_DETAIL_RATIO, m_hideDetailRatio);
	DDV_MinMaxDouble(pDX, m_hideDetailRatio, 1.e-003, 0.5);
	DDX_Check(pDX, IDC_HIDE_DETAIL, m_hideDetail);
	DDX_Text(pDX, IDC_MARGIN2, m_margin2);
	DDV_MinMaxDouble(pDX, m_margin2, 0., 100.);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_ShowHiddenAttributes, m_showHiddenAttributes);
	DDX_Check(pDX, IDC_SHOW_POINT_ENTITIES, m_showPoints);
	DDX_Check(pDX, IDC_SHOW_OVERRIDES, m_alwaysShowOverrides);
}

BEGIN_MESSAGE_MAP(SettingsView, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsView)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SettingsView::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   m_mixModeCB.AddString("xor pen");
   m_mixModeCB.AddString("not xor pen");
   m_mixModeCB.AddString("mask pen");
   m_mixModeCB.AddString("mask pen not");
   m_mixModeCB.AddString("mask not pen");
   m_mixModeCB.AddString("not mask pen");
   m_mixModeCB.AddString("merge pen");
   m_mixModeCB.AddString("merge pen not");
   m_mixModeCB.AddString("merge not pen");
   m_mixModeCB.AddString("not merge pen");
   m_mixModeCB.AddString("not pen");

   m_mixMode = mixMode;
   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// SettingsColor property page
IMPLEMENT_DYNCREATE(SettingsColor, CPropertyPage)

SettingsColor::SettingsColor() : CPropertyPage(SettingsColor::IDD)
{
   //{{AFX_DATA_INIT(SettingsColor)
   //}}AFX_DATA_INIT
}

SettingsColor::~SettingsColor()
{
}

void SettingsColor::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SettingsColor)
   DDX_Control(pDX, IDC_COLOR_UNDEFTOOLS, m_utools);
   DDX_Control(pDX, IDC_COLOR_TEST_OFFSET, m_testOffset);
   DDX_Control(pDX, IDC_COLOR_HIDEDETAIL, m_hideDetail);
   DDX_Control(pDX, IDC_COLOR_TOOLS, m_tools);
   DDX_Control(pDX, IDC_COLOR_UAPS2, m_uaps);
   DDX_Control(pDX, IDC_COLOR_SELECTED, m_selected);
   DDX_Control(pDX, IDC_COLOR_HIGHLIGHTED, m_highlighted);
   DDX_Control(pDX, IDC_COLOR_MARKED, m_marked);
   DDX_Control(pDX, IDC_COLOR_GRID, m_grid);
   DDX_Control(pDX, IDC_COLOR_BACKGROUND, m_background);
   DDX_Control(pDX, IDC_COLOR_PAGE, m_page);
   DDX_Control(pDX, IDC_COLOR_AXES, m_axes);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SettingsColor, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsColor)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_COLOR_NAVIGATOR, OnBnClickedColorNavigator)
END_MESSAGE_MAP()

// SystemColorButton
SystemColorButton::SystemColorButton()
{
}

SystemColorButton::~SystemColorButton()
{
}

BEGIN_MESSAGE_MAP(SystemColorButton, CButton)
   //{{AFX_MSG_MAP(SystemColorButton)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SystemColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   HBRUSH hBrush, orgBrush;
   HPEN hPen, orgPen;
   unsigned long color;
   char *string;
   int length;
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();

	switch (lpDrawItemStruct->CtlID)
   {
   case IDC_COLOR_PAGE:
      color = doc->getSettings().Foregrnd;
      string = "Page";
      break;
   case IDC_COLOR_BACKGROUND:
      color = doc->getSettings().Bckgrnd;
      string = "Background";
      break;
   case IDC_COLOR_AXES:
      color = doc->getSettings().CrossColor;
      string = "Origin";
      break;
   case IDC_COLOR_GRID:
      color = doc->getSettings().GridColor;
      string = "Grid";
      break;
   case IDC_COLOR_HIGHLIGHTED:
      color = doc->getSettings().HighlightColor;
      string = "Highlighted";
      break;
   case IDC_COLOR_SELECTED:
      color = doc->getSettings().SelectColor;
      string = "Selected";
      break;
   case IDC_COLOR_MARKED:
      color = doc->getSettings().MarkedColor;
      string = "Marked";
      break;
   case IDC_COLOR_UAPS2:
      color = doc->getSettings().UndefinedColor;
      string = "Undefined Apertures";
      break;
   case IDC_COLOR_TOOLS:
      color = doc->getSettings().ToolColor;
      string = "Generic Tools";
      break;
   case IDC_COLOR_UNDEFTOOLS:
      color = doc->getSettings().UndefToolColor;
      string = "Undefined Tools";
      break;
   case IDC_COLOR_HIDEDETAIL:
      color = doc->getSettings().HideDetailColor;
      string = "Hide Detail";
      break;
   case IDC_COLOR_TEST_OFFSET:
      color = doc->getSettings().TestAccessColor;
      string = "Test Offset";
      break;
	case IDC_BUTTON_COLOR1:
		color = frame->getNavigator().highlightColors[0];
		string = "First Highlight Color";
		break;
	case IDC_BUTTON_COLOR2:
		color = frame->getNavigator().highlightColors[1];
		string = "Second Highlight Color";
		break;
	case IDC_BUTTON_COLOR3:
		color = frame->getNavigator().highlightColors[2];
		string = "Third Highlight Color";
		break;
	case IDC_BUTTON_COLOR4:
		color = frame->getNavigator().highlightColors[3];
		string = "Fourth Highlight Color";
		break;
	case IDC_BUTTON_COLOR5:
		color = frame->getNavigator().highlightColors[4];
		string = "Fifth Highlight Color";
		break;
   }              
   length = strlen(string);

   switch (lpDrawItemStruct->itemAction)
   {
   case ODA_DRAWENTIRE: // draw button color
   {
      // paint button
      hBrush = CreateSolidBrush(color);
      orgBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
      Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, 
            lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, 
            lpDrawItemStruct->rcItem.bottom);
      DeleteObject(SelectObject(lpDrawItemStruct->hDC, orgBrush));

      // make 3D
      hPen = (HPEN) CreatePen(PS_SOLID, 0, 8421504);
      orgPen = (HPEN) SelectObject(lpDrawItemStruct->hDC, hPen);
      MoveToEx(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left+1, lpDrawItemStruct->rcItem.bottom-2, NULL);
      LineTo(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.right-2, lpDrawItemStruct->rcItem.bottom-2);
      LineTo(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.right-2, lpDrawItemStruct->rcItem.top+1);
      DeleteObject(SelectObject(lpDrawItemStruct->hDC, (HPEN) GetStockObject(WHITE_PEN)));
      MoveToEx(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left+1, lpDrawItemStruct->rcItem.bottom-2, NULL);
      LineTo(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left+1, lpDrawItemStruct->rcItem.top+1);
      LineTo(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.right-2, lpDrawItemStruct->rcItem.top+1);
      SelectObject(lpDrawItemStruct->hDC, orgPen);
      
      // write text
      TEXTMETRIC tm;
      GetTextMetrics(lpDrawItemStruct->hDC, &tm);
      TextOut(lpDrawItemStruct->hDC, 
            (lpDrawItemStruct->rcItem.right - tm.tmAveCharWidth*length) / 2, 
            (lpDrawItemStruct->rcItem.bottom - tm.tmHeight) / 2, string, length);

      if (lpDrawItemStruct->itemState & ODS_FOCUS)
      {
         RECT rect;
         rect.left = (lpDrawItemStruct->rcItem.right - tm.tmAveCharWidth*length) / 2;
         rect.right = rect.left + tm.tmAveCharWidth*length;
         rect.top = (lpDrawItemStruct->rcItem.bottom - tm.tmHeight) / 2;
         rect.bottom = rect.top + tm.tmHeight;
         DrawFocusRect(lpDrawItemStruct->hDC, &rect);
      }
   }
   break;
   
   case ODA_FOCUS: // button gains or loses focus
   {
      TEXTMETRIC tm;
      GetTextMetrics(lpDrawItemStruct->hDC, &tm);
      RECT rect;
      rect.left = (lpDrawItemStruct->rcItem.right - tm.tmAveCharWidth*length) / 2;
      rect.right = rect.left + tm.tmAveCharWidth*length;
      rect.top = (lpDrawItemStruct->rcItem.bottom - tm.tmHeight) / 2;
      rect.bottom = rect.top + tm.tmHeight;
      DrawFocusRect(lpDrawItemStruct->hDC, &rect);
   }
   break;

   case ODA_SELECT: // button clicked
   {
      if (lpDrawItemStruct->itemState & ODS_SELECTED) // button state changes twice(down then up) - only answer once
      {
         CPersistantColorDialog dialog(color);
         dialog.DoModal();
         color = dialog.GetColor();
         switch (lpDrawItemStruct->CtlID)
         {
         case IDC_COLOR_PAGE:
            doc->getSettings().Foregrnd = color;
            break;
         case IDC_COLOR_BACKGROUND:
            doc->getSettings().Bckgrnd = color;
            break;
         case IDC_COLOR_AXES:
            doc->getSettings().CrossColor = color;
            break;
         case IDC_COLOR_GRID:
            doc->getSettings().GridColor = color;
            break;
         case IDC_COLOR_SELECTED:
            doc->getSettings().SelectColor = color;
            break;
         case IDC_COLOR_HIGHLIGHTED:
            doc->getSettings().HighlightColor = color;
            break;
         case IDC_COLOR_MARKED:
            doc->getSettings().MarkedColor = color;
            break;
         case IDC_COLOR_UAPS2:
            doc->getSettings().UndefinedColor = color;
            break;
         case IDC_COLOR_TOOLS:
            doc->getSettings().ToolColor = color;
            break;
         case IDC_COLOR_UNDEFTOOLS:
            doc->getSettings().UndefToolColor = color;
            break;
         case IDC_COLOR_HIDEDETAIL:
            doc->getSettings().HideDetailColor = color;
            break;
         case IDC_COLOR_TEST_OFFSET:
            doc->getSettings().TestAccessColor = color;
            break;
			case IDC_BUTTON_COLOR1:
				frame->getNavigator().highlightColors[0] = color;
				break;
			case IDC_BUTTON_COLOR2:
				frame->getNavigator().highlightColors[1] = color;
				break;
			case IDC_BUTTON_COLOR3:
				frame->getNavigator().highlightColors[2] = color;
				break;
			case IDC_BUTTON_COLOR4:
				frame->getNavigator().highlightColors[3] = color;
				break;
			case IDC_BUTTON_COLOR5:
				frame->getNavigator().highlightColors[4] = color;
				break;
         }              
         Invalidate();
      }
   }
   break;
   }
}

void SettingsColor::OnBnClickedColorNavigator()
{
	CColorSettingsNavigator NavColorDlg;
	NavColorDlg.DoModal();
}

// SettingsColor property page
IMPLEMENT_DYNCREATE(SettingsRedline, CPropertyPage)

SettingsRedline::SettingsRedline() : CPropertyPage(SettingsRedline::IDD)
{
   //{{AFX_DATA_INIT(SettingsRedline)
   m_arrowSize = _T("");
   m_textHeight = _T("");
   //}}AFX_DATA_INIT
}

SettingsRedline::~SettingsRedline()
{
}

void SettingsRedline::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SettingsRedline)
   DDX_Text(pDX, IDC_ARROW_SIZE, m_arrowSize);
   DDX_Text(pDX, IDC_TEXT_HEIGHT, m_textHeight);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SettingsRedline, CPropertyPage)
   //{{AFX_MSG_MAP(SettingsRedline)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SettingsRedline message handlers
// E:\Development\CamCad\4.3\Settings.cpp : implementation file
//


IMPLEMENT_DYNAMIC(CPinLabelSettingsPropertyPage, CPropertyPage)
CPinLabelSettingsPropertyPage::CPinLabelSettingsPropertyPage()
   : CPropertyPage(CPinLabelSettingsPropertyPage::IDD)
   , m_nominalTextHeight(0)
   , m_minTextHeight(0)
   , m_maxTextHeight(0)
   , m_textBoxHeight(0)
   , m_fontFace(_T(""))
   , m_pageUnits(UNIT_INCHES)
   , m_textAngle(0)
{   
}

CPinLabelSettingsPropertyPage::~CPinLabelSettingsPropertyPage()
{
}

void CPinLabelSettingsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_NominalTextHeight, m_nominalTextHeight);
   DDX_Text(pDX, IDC_MinTextHeight, m_minTextHeight);
   DDV_MinMaxUInt(pDX, m_minTextHeight, 2, 1000);
   DDX_Text(pDX, IDC_MaxTextHeight, m_maxTextHeight);
   DDV_MinMaxUInt(pDX, m_maxTextHeight, 2, 1000);
   DDX_Text(pDX, IDC_TextBoxHeight, m_textBoxHeight);
   DDV_MinMaxUInt(pDX, m_textBoxHeight, 2, 1000);
   DDX_Text(pDX, IDC_FontFace, m_fontFace);
   DDX_Text(pDX, IDC_TextAngle, m_textAngle);
}


BEGIN_MESSAGE_MAP(CPinLabelSettingsPropertyPage, CPropertyPage)
   ON_BN_CLICKED(ID_BrowseFont, OnBnClickedBrowseFont)
END_MESSAGE_MAP()

BOOL CPinLabelSettingsPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here

   m_nominalTextHeight = m_pinLabelSettings.getTextHeightInches() * Units_Factor(UNIT_INCHES,m_pageUnits);
   m_minTextHeight     = m_pinLabelSettings.getMinTextHeightPixels();
   m_maxTextHeight     = m_pinLabelSettings.getMaxTextHeightPixels();
   m_textBoxHeight     = m_pinLabelSettings.getTextBoxPixels();
   m_textAngle         = m_pinLabelSettings.getTextDegrees();

	m_pinLabelSettings.getLogFont(m_logFont);
   m_fontFace          = m_logFont.lfFaceName;
   m_logFont.lfHeight  = 0;

   GetDlgItem(IDC_TextHeightUnits)->SetWindowText(unitsString(m_pageUnits));

   UpdateData(false);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CPinLabelSettingsPropertyPage::OnOK()
{
   // TODO: Add your specialized code here and/or call the base class
   UpdateData(true);

	while (m_textAngle >= 360)
		m_textAngle -= 360;
	while (m_textAngle < 0)
		m_textAngle += 360;

   m_pinLabelSettings.setTextHeightInches(m_nominalTextHeight * Units_Factor(m_pageUnits,UNIT_INCHES));
   m_pinLabelSettings.setMinTextHeightPixels(m_minTextHeight);
   m_pinLabelSettings.setMaxTextHeightPixels(m_maxTextHeight);
   m_pinLabelSettings.setTextBoxPixels(m_textBoxHeight);
   m_pinLabelSettings.setLogFont(m_logFont);

	// Need to call setTextAngle after setLogFont because setTextAngle change some member in m_logFont, see setTextAngle()
	m_pinLabelSettings.setTextDegrees(m_textAngle);

   CPropertyPage::OnOK();
}

void CPinLabelSettingsPropertyPage::OnBnClickedBrowseFont()
{
   // TODO: Add your control notification handler code here
   CFontDialog fontDialog;
   fontDialog.m_cf.lpLogFont = &m_logFont;
   fontDialog.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;

   if (fontDialog.DoModal() == IDOK)
   {
      m_fontFace = fontDialog.GetFaceName();

      UpdateData(false);
   }
}


// CPinLabelSettingsPropertyPage message handlers
//


////////////////////////////////////////////////////////
// CColorSettingsNavigator dialog

IMPLEMENT_DYNAMIC(CColorSettingsNavigator, CDialog)
CColorSettingsNavigator::CColorSettingsNavigator(CWnd* pParent /*=NULL*/)
	: CDialog(CColorSettingsNavigator::IDD, pParent)
{
   //{{AFX_DATA_INIT(CColorSettingsNavigator)
   //}}AFX_DATA_INIT
}

CColorSettingsNavigator::~CColorSettingsNavigator()
{
}

void CColorSettingsNavigator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CColorSettingsNavigator)
	DDX_Control(pDX, IDC_BUTTON_COLOR1, NavColor1);
	DDX_Control(pDX, IDC_BUTTON_COLOR2, NavColor2);
	DDX_Control(pDX, IDC_BUTTON_COLOR3, NavColor3);
	DDX_Control(pDX, IDC_BUTTON_COLOR4, NavColor4);
	DDX_Control(pDX, IDC_BUTTON_COLOR5, NavColor5);
   //}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CColorSettingsNavigator, CDialog)
   //{{AFX_MSG_MAP(CColorSettingsNavigator)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CColorSettingsNavigator message handlers
BOOL CColorSettingsNavigator::OnInitDialog()
{
	UpdateData();
	return TRUE;
}