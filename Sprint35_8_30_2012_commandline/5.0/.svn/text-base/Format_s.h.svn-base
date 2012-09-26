// $Header: /CAMCAD/4.6/Format_s.h 37    3/12/07 3:36p Lynn Phung $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

#pragma once

#include "dbutil.h"

// SPECIAL NOTE
//    if you want to use CStrings in FormatStruct, initialization of structure variables
//    can NOT be done with memset

struct HPPenstruct 
{
   double   width;
   CString  layer;
   unsigned long color;
};

struct HPstruct 
{
   struct   HPPenstruct HpPens[MAX_HPPENS]; // in mm, needs to be normalized for a page
   int      HPspace; 
   CString  HPFooterText;  // HPGL Out
   char     HPGL2;         // HPGL Out
   char     hollow;
   char     Circles_2_Aps; // HPGL In
   char     Proportional;  // HPGL In
};

struct FileExt
{
   CString  Gerber;
   CString  HPGL; 
   CString  EXCELLON;   
   CString  IGES;
   CString  APERTURE;
};

struct GRstruct 
{
   int format;    // 0-G RS-274; 1-G RS-274-X; 2-FIRE 9XXX
   int type;      // 0-Absolut; 1-Incremental
   int zero;      // 0-Leading; 1-Trailing; 2-None
   int units;     // 0-English; 1-Metric
   int digits;    // Number of Digits before decimal
   int decimal;   // Number of decimal places
   int circleMode; // 0=Sectorize  1=360° circluar interpolation
   int PolyFill;  // Special data processor for writing filled polys
   int thermalMacros; // Read macros for apertures
   int fillDCode; // dcode used to fill polys
   double sectorizationAngle; // degrees to break arcs to vectors
   int layerStencilThickness; //0-only Top/Bot 1-Top/Bot with thickness attributes
};

struct DXFstruct 
{
   char Circles_2_Aps;           // DXF In
   char Donuts_2_Aps;            // DXF In
   char Proportional;            // DXF In
   char PolyEnds;                // DXF In
   int  Units;                   // DXF In, out CAMCAD Units
   bool XscaleText;              // DXF Out
   bool UnMirrorText;            // DXF Out
};

struct IPCstruct
{
   int  Units;           // IPC Out (Inches, Mils, Millimeters)
   char IPC_Format;      // IPC Out (0-IPC 350, 1-IPC 356, 2-IPC-D-356A)
};

enum TriDatFormatTag {
	TriDatFormatMDA = 0, // number are forced so they align with gui radio buttons
	TriDatFormatICT = 1
};

struct TRIstruct 
{
   // byte 1
   char TopPopulated:1;
   char BottomPopulated:1;
   char Capacitor:1;
   char Diode:1;
   char dummy1:1;
   char dummy2:1;
   char dummy3:1;
   char dummy4:1;
   // byte 2
   char Inductor:1;
   char Jumper:1;
   char Photocoupler:1;
   char Resistor:1;
   char Triac:1;
   char Transistor:1;
   char Transistorhfe:1;
   char Unknown:1;

	TriDatFormatTag datFormat;
};

struct Takayastruct 
{
   char TestSurface;    // 0=top, 1=bottom, 2=both
   char SidePreference; // 0 = top, 1=bottom
   // byte 1
   char TopPopulated:1;
   char BottomPopulated:1;
   char Capacitor:1;
   char dummy1:1;
   char dummy2:1;
   char dummy3:1;
   char dummy4:1;
   char dummy5:1;
   // byte 2
   char Diode:1;
   char Inductor:1;
   char IC_Capacitor:1;
   char IC_Diode:1;
   char Resistor:1;
   char Transistor:1;
   char ZenerDiode:1;
   char Short:1;
   int  MaxHitCount;
};

struct QDStruct
{
   bool glueTop;
   bool glueBot;
   int boardFiducialSymbolNum;
   int panelFiducialSymbolNum;
   int boardThickness;
   int panelThickness;
	CString boardPartNumber;
   int panelTolerance;
   int boardTolerance;
};

struct EXstruct 
{
   int format;    // 0->Excellon1, 1->Excellon2
   int type;      // 0-Absolute; 1-Incremental
   int zeroSuppression;      // 0-Leading; 1-Trailing; 2-None
   int units;     // 0-inches; 1-mm
	BOOL includeDecimalPoint;
	int wholeDigits;
	int significantDigits;
   BOOL AutoAp;   // Automatically assign apertures to tools
	short drillTypes;				// 0-PCB Drills; 1-Probe Drills
	short probeDrillSurface;	// Top-0; Bottom-1; Both-2
   BOOL includePilotHoles;		// include pilot holes if exporting probes
	BOOL includeToolingHoles;	// applies to both PCB and Probe drill export
};

enum AoiMachineTag
{
   AoiMachineSj        = 0,
   AoiMachineSp        = 1,
   AoiMachineVia       = 2,
   AoiMachineFx        = 3,
   AoiMachineMixed     = 4,  // sj and sp
   AoiMachineSp50      = 5,
   AoiMachineUndefined = -1
};

struct AOIstruct
{
   AoiMachineTag machine;   // 0=SJ, 1=SP, 2=VIA, 3=FX, 4=Mixed(SJ&SP)
   bool smd_only;
   bool useZeroRotationForComplexAperturesFlag;
};

// Macro indicating whether to use package or partnumber as the 6th field in the component section
#define FIELD_USE_PACKAGE			0
#define FIELD_USE_PARTNUMBER		1

class CMvpAoiSettings
{
private:
   bool m_exportSmdComponentsFlag;
   bool m_exportThComponentsFlag;
	int m_componentFieldSix;			// 0 = package, 1 = partnumber

public:
   CMvpAoiSettings();

   bool getExportSmdComponentsFlag() const		{ return m_exportSmdComponentsFlag; }
   void setExportSmdComponentsFlag(bool flag)	{ m_exportSmdComponentsFlag = flag; }

   bool getExportThComponentsFlag() const			{ return m_exportThComponentsFlag;	}
   void setExportThComponentsFlag(bool flag)		{ m_exportThComponentsFlag = flag;	}

	int getCompFieldSixUsage() const					{ return m_componentFieldSix;			}
	void setCompFieldSixUsage(int usage)			{ m_componentFieldSix = usage;		}	
};


struct FormatStruct
{
private:
   CMvpAoiSettings m_mvpAoiSettings;

public:
   int FileType;
   int PortFileUnits;
   double Scale;
   char prefix[MAXPREFIX+1];     // layer name prefix (All Readers)

   char FillPolys;               // IGES In - Fill Zero Width Polys

   DXFstruct DXF;                // DXF In / Out
   GRstruct GR;                  // Gerber In 
   EXstruct EX;                  // Excellon In 
   HPstruct HP;                  // HPGL In / Out
   Takayastruct Tak;             // Takaya out
   TRIstruct TRI;                // TRI-MDA-ICT out
   AOIstruct AOI;                // AOI out
   QDStruct QD;
   IPCstruct IPCSettings;        //IPC Out

   char LayerByColor;            // Protel In
   double SmallestWidth;         // Protel In
   double SmallWidth;            // Protel In
   double MediumWidth;           // Protel In
   double LargeWidth;            // Protel In

   char ExplodeFormat;           // Allegro In
   char SuppressClass;           // Allegro In

   char PADS_Flashes;            // PADS Out
   char PDIF_True_TextRot;       // PDIF Out
   int *WidthCodes;              // CADL, CDI, Protel Out
   char AddLineEnds;             // DXF Out, IGES Out
   double RealWidth;             // DXF Out, IGES Out, HP Out
   char explodeStyle;            // DXF out
   char hollow;                  // DXF Out, DMIS Out
   char exporthiddenentities;    // DXF out, (later also Gerber and HPGL out)
   bool useprobetype;            // DXF out, Use probemane for tool layer 
   char CreatePanelFile;         // Gencad Out 
   char Output;                  // Gencad Out, Mentor Out, Protel Out 0-PCB 1-Graphic, IPC 0-350 1-356 2-356A
   char Version;                 // Gencad Out 0-v1.3 1-v1.4
   char IncludeVias;             // IPC out, HP3070 Out
   char IncludeUnconnectedPins;  // HP3070 Out
   char OneNCNet;                // HP3070 Out
   char Delete3070Attributes;    // HP3070 Out
	int  GenerateDFT3070;         // HP3070 Out
   bool ExportVariant;           // HP3070 Out
   unsigned int GenCAMFormat;    // GenCAM In/Out (bitmap)

   char TestType;                // Test Out
   char InsertionType;           // Insertion Out
   char ignorePenWidth;          // HP In 
   char newline_recordend;       // IPC in
   char Allegro_Format;          // Allegro in (0-Unknown, 1-CAMCAD, 2-FABMASTER, 3-ClassicLaybase)
   char HughesFirstFileTop;      // Hughes IN - if 2 files, second is opposite of first

   char filename[OFS_MAXPATHNAME]; 
   CString formatString;

private:
   bool m_enableGenerateSmdComponents;

public:
   FormatStruct()
   {
      m_enableGenerateSmdComponents = true;
   }

   bool getEnableGenerateSmdComponents() const { return m_enableGenerateSmdComponents; }
   void setEnableGenerateSmdComponents(bool enableFlag) { m_enableGenerateSmdComponents = enableFlag; }

   CMvpAoiSettings& getMvpAoiSettings() { return m_mvpAoiSettings; }

};

// end FORMAT_S.H

