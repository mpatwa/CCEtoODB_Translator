// $Header: /CAMCAD/4.6/Lic.h 79    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__Lic_h__)
#define __Lic_h__

#pragma once

// With define LicenseBypass disabled the debug builds require license to run.
// That is how I want it for now. Too many ills have been not noticed in dev mode when
// licensing is disabled.
#if defined(_RDEBUG)
// #define LicenseBypass
#endif

#define LIC_VERSION                 "4.0"

// PRINT
#define LIC_PRINT                   0
#define LIC_PRINT_S                 "CPRINT"

// QUERY
#define LIC_QUERY                   1
#define LIC_QUERY_S                 "Query"

// CEDIT
#define LIC_CEDIT                   2
#define LIC_CEDIT_S                 "CEDIT"

// PEDIT
#define LIC_PEDIT                   3        // needed for redline
#define LIC_PEDIT_S                 "PEDIT"

// API
#define LIC_API_LEVEL_1             4
#define LIC_API_LEVEL_2             5
#define LIC_API_LEVEL_3             6
#define LIC_API_LEVEL_4             7
#define LIC_API_LEVEL_1_S           "API Level 1"
#define LIC_API_LEVEL_2_S           "API Level 2"
#define LIC_API_LEVEL_3_S           "API Level 3"
#define LIC_API_LEVEL_4_S           "API Level 4"

// REDLINE
#define LIC_REDLINE                 8  
#define LIC_REDLINE_S               "Redlining"

// TA
#define LIC_TA                      9
#define LIC_TA_S                    "Design For Test (DFT)"

// DFM
#define LIC_DFM                     10
#define LIC_VISION_DFM              10
#define LIC_DFM_S                   "Design For Manufacturing (DFM)"

// RealPart Package Assign
#define LIC_RP_PKG                  116
#define LIC_RP_PKG_S                "RealPart Package Assignment"

// RealPart Device Assign
#define LIC_RP_DEV                  117
#define LIC_RP_DEV_S                "RealPart Device Assignment"

// Stencil Generator
#define LIC_StencilGenerator        119
#define LIC_StencilGenerator_S      "Stencil Generator"

// Gerber Educator
#define LIC_GerberEducator          122
#define LIC_GerberEducator_S        "Gerber Educator"

// Stencil Stepper
#define LIC_StencilStepper          138
#define LIC_StencilStepper_S        "Stencil Stepper"


/***** IN *****/

// ACCELPCB
#define LIC_ACCELPCBIN              11
#define LIC_ACCELPCBIN_S            "ACCEL, PCAD200x (.pcb) Layout Read"

// Agilent SJ PLX
#define LIC_AgilentSjPlx            120
#define LIC_AgilentSjPlx_S          "Agilent SJ PLX Centroid Read"

// ALCATEL
#define LIC_ALCATEL_DOCICA          113
#define LIC_ALCATEL_DOCICA_S        "Alcatel Docica Read"

// ALCATEL GMF
#define LIC_ALCATEL_GMF             114
#define LIC_ALCATEL_GMF_S           "Alcatel GMF+ Read"

// ALLEGRO
#define LIC_ALLEGROIN               12
#define LIC_ALLEGROIN_S             "CADENCE Allegro (Extract) Read"

// APER
#define LIC_APERIN                  13
#define LIC_APERIN_S                "Aperture Read"

// AUTOCAD_ADI
#define LIC_AUTOCAD_ADIIN           14
#define LIC_AUTOCAD_ADIIN_S         "Autocad ADI Read"

// BARCO
#define LIC_BARCOIN                 15
#define LIC_BARCOIN_S               "Barco (.dpf) Read"

// BNR
#define LIC_BNRIN                   16
#define LIC_BNRIN_S                 "Unicad/BNR Read"

// BOM Delimited
#define LIC_BOMCSV_READ					137
#define LIC_BOMCSV_READ_S				"Attribute/BOM Delimited Import"	

// BOM
#define LIC_BOMREAD                 17
#define LIC_BOMREAD_S               "Attribute/BOM Read"

// CADENCEPLOT
#define LIC_CADENCEPLOT             18
#define LIC_CADENCEPLOT_S           "CADENCE Allegro IPF Read"

// CADIF
#define LIC_CADIFIN                 19
#define LIC_CADIFIN_S               "Redac CADIF (.paf) Layout Read"

// CASEPLOT
#define LIC_CASEPLOTIN              20
#define LIC_CASEPLOTIN_S            "CASE Plot Read"

// CALAY PRISMA
#define LIC_CALAYPRISMAIN           21
#define LIC_CALAYPRISMAIN_S         "CALAY PRISMA Layout Read"

// CCT
#define LIC_CCTIN                    22
#define LIC_CCTIN_S                 "CCT Design File Read"

// CII
#define LIC_CIIIN                   23
#define LIC_CIIIN_S                 "Scicards/Encore (CII) Layout Read"

// CR3000 PWS files
#define LIC_CR3000IN                24 
#define LIC_CR3000IN_S              "Zuken PWS (CR3000/CR5000) Layout Read"

// CR5000 pcf, ftf files
#define LIC_CR5000IN                25 
#define LIC_CR5000IN_S              "Zuken CR5000 Board Designer Read"

// DDE Dansk Data 
#define LIC_DDEIN                   26 
#define LIC_DDEIN_S                 "DDE Layout Read"

// DXF
#define LIC_DXFIN                   27
#define LIC_DXFIN_S                 "DXF Read"

// DMIS
#define LIC_DMISIN                  28
#define LIC_DMISIN_S                "DMIS V3 Read"

// EAGLE - Unidat
#define LIC_UNIDATIN                29
#define LIC_UNIDATIN_S              "UNIDAT Read"

// EDIF
#define LIC_EDIFIN                  30
#define LIC_EDIFIN_S                "EDIF V 200...400 Read"

// EIF
#define LIC_EIFIN                   31 
#define LIC_EIFIN_S                 "VERIBEST EIF Layout Read"

// EXCELLON
#define LIC_EXCELLONIN					32
#define LIC_EXCELLONIN_S				"Excellon Drill&Route Read"

// FABMASTER
#define LIC_FATFIN                  33
#define LIC_FATFIN_S                "FABMASTER FATF Read"

// FABMASTER ASCII Probe file
#define LIC_FAB_NAIL_IN             34
#define LIC_FAB_NAIL_IN_S           "FABMASTER NAIL Read"

// GENCAD
#define LIC_GENCADIN                35
#define LIC_GENCADIN_S              "GENCAD Read"

// GENCAM
#define LIC_GENCAMIN                36
#define LIC_GENCAMIN_S              "GenCAM V1.5 Read"


// GERBER
#define LIC_GERBERIN                37
#define LIC_GERBERIN_S              "Gerber Read"

// HUGHES / Raytheon propriatory format
#define LIC_HUGHESIN                38
#define LIC_HUGHESIN_S              "Hughes/Raytheon Read"

// HPEGS
#define LIC_HPEGSIN                 39
#define LIC_HPEGSIN_S               "HP EGS Archive Read"

// HPGL
#define LIC_HPGLIN                  40
#define LIC_HPGLIN_S                "HPGL/HPGL2 Read"

// HP3070
#define LIC_HP3070IN                41
#define LIC_HP3070IN_S              "Agilent 3070 Read"

// HP5DX
#define LIC_HP5DXIN                 42
#define LIC_HP5DXIN_S               "Agilent 5DX Read"

// IPC
#define LIC_IPCIN                   43
#define LIC_IPCIN_S                 "IPC350/356/356A Read"

// IGES
#define LIC_IGESIN                  44
#define LIC_IGESIN_S                "IGES Read"

// MENNEUT
#define LIC_MENNEUTIN               45
#define LIC_MENNEUTIN_S             "Mentor Neutral File Read"

// MENTOR
#define LIC_MENTORIN                46
#define LIC_MENTORIN_S              "Mentor Board Station V8 Read"

// ODB++ VALOR
#define LIC_ODBPPIN                 47
#define LIC_ODBPPIN_S               "ODB++ Read"

// ORCADLTD
#define LIC_ORCADLTDIN              48 
#define LIC_ORCADLTDIN_S            "Orcad (.min) Layout Plus Read"

// PADSPCB
#define LIC_PADSPCBIN               49
#define LIC_PADSPCBIN_S             "PADS (.asc) Layout Read"

// PADSLIB (.D)
#define LIC_PADSLIBIN               115
#define LIC_PADSLIBIN_S             "Pads Library (.d) Read"

// PDIFPCB
#define LIC_PDIFPCBIN               50
#define LIC_PDIFPCBIN_S             "PCAD PDIF Layout Read"

// PDIFSCH
#define LIC_PDIFSCHIN               51
#define LIC_PDIFSCHIN_S             "PCAD PDIF Schematic Read"

// PFWSCH
#define LIC_PFWSCHIN                52    
#define LIC_PFWSCHIN_S              "Protel PFW Schematic Read"

// PROTEL layout
#define LIC_PROTELPCBIN             53
#define LIC_PROTELPCBIN_S           "Protel PCB ASCII Read"

// THEDA
#define LIC_THEDAIN                 54
#define LIC_THEDAIN_S               "THEDA (.tl) Layout, Panel Read"

// ULTIBOARD
#define LIC_ULTIBOARDIN             55
#define LIC_ULTIBOARDIN_S           "ULTIBOARD (.edf) Layout Read"

// UNICAM
#define LIC_UNICAMIN                56
#define LIC_UNICAMIN_S              "UNICAM PDW Read"

// VB99
#define LIC_VB99IN                  57 
#define LIC_VB99IN_S                "VB ASCII (.hkp) Layout Read"

// VIEWDRAW
#define LIC_VIEWDRAWIN              58
#define LIC_VIEWDRAWIN_S            "VIEWDRAW Read"



/***** OUT *****/

// IDF V2.0 PCB
#define LIC_3D_PCBOUT               59
#define LIC_3D_PCBOUT_S             "IDF V2.0 Layout Write"

// ACCELPCB
#define LIC_ACCELPCBOUT             60
#define LIC_ACCELPCBOUT_S           "ACCEL Write, PCAD200x (.pcb) Layout"

// ALLEGRO
#define LIC_ALLEGROOUT              61
#define LIC_ALLEGROOUT_S            "CADENCE Allegro (Script) Write"

// AGILENT AOI
#define LIC_AGILENT_AOI_OUT         62 
#define LIC_AGILENT_AOI_OUT_S       "Agilent AOI Write"

// APER
#define LIC_APEROUT                 63
#define LIC_APEROUT_S               "Aperture Write"

// BARCO
#define LIC_BARCOOUT                64
#define LIC_BARCOOUT_S              "Barco (.dpf) Write"

// BOM
#define LIC_BOMWRITE                65
#define LIC_BOMWRITE_S              "Attribute/BOM Write"

// CADIF
#define LIC_CADIFOUT                66
#define LIC_CADIFOUT_S              "Redac CADIF (.paf) Layout Write"

// CCM
#define LIC_CCMOUT                  67    // RSI intermediate CAM format
#define LIC_CCMOUT_S                "CCM Format Write"

// CDI
#define LIC_CDIOUT                  68
#define LIC_CDIOUT_S                "CADSTAR-DOS CDI Graphic Write"

// CR3000
#define LIC_CR3000OUT               69
#define LIC_CR3000OUT_S             "Zuken CR3000 PWS Layout Write"

// DDE Dansk Data 
#define LIC_DDEOUT                  70    
#define LIC_DDEOUT_S                "DDE Layout Write"

// DMIS
#define LIC_DMISOUT                 71
#define LIC_DMISOUT_S               "DMIS V3 Write"

// DXF
#define LIC_DXFOUT                  72
#define LIC_DXFOUT_S                "DXF Write"

// EDIF
#define LIC_EDIFOUT                 73
#define LIC_EDIFOUT_S               "EDIF 200 Graphic Write"

// EIF
#define LIC_EIFOUT                  74
#define LIC_EIFOUT_S                "VERIBEST EIF Layout Write"

// Veribest Vb99
#define LIC_VB99OUT                 75
#define LIC_VB99OUT_S               "VB ASCII (.hkp) Layout Write"

// EXCELLON
#define LIC_EXCELLONOUT             76
#define LIC_EXCELLONOUT_S           "Excellon Drill&Route Write"

// FABMASTER
#define LIC_FATFOUT                  77
#define LIC_FATFOUT_S               "FABMASTER FATF Write"

// GENCAD
#define LIC_GENCADOUT               78
#define LIC_GENCADOUT_S             "GENCAD Write"

// GENCAM
#define LIC_GENCAMOUT               79
#define LIC_GENCAMOUT_S             "GenCAM V1.5 Write"

// GERBER
#define LIC_GERBEROUT               80
#define LIC_GERBEROUT_S             "Gerber Write"

// HP3070
#define LIC_HP3070OUT               81
#define LIC_HP3070OUT_S             "Agilent 3070 Write"

// HP5DX
#define LIC_HP5DXOUT                82
#define LIC_HP5DXOUT_S              "Agilent 5DX Write"

// HPGL
#define LIC_HPGLOUT                 83
#define LIC_HPGLOUT_S               "HPGL/HPGL2 Write"

// HUNTRON
#define LIC_HUNTRON                 84
#define LIC_HUNTRON_S               "HAF Write"

// HYPERLYNX
#define LIC_HYPERLYNXOUT            85
#define LIC_HYPERLYNXOUT_S          "HyperLynx BoardSim Write"

// IGES
#define LIC_IGESOUT                 86
#define LIC_IGESOUT_S               "IGES Write"

// INGUN TRI-MDA
#define LIC_INGUNOUT                87    
#define LIC_INGUNOUT_S              "TRI-MDA Write"

// IPC
#define LIC_IPCOUT                  88
#define LIC_IPCOUT_S                "IPC350/356/356A Write"

// IPL
#define LIC_IPLOUT                  121
#define LIC_IPLOUT_S                "Teradyn Z18xxx IPL Write"

// MENTBRD
#define LIC_MENTBRDOUT              89
#define LIC_MENTBRDOUT_S            "Mentor Board Station V8 Write"

// MENTOR NEUTRAL
#define LIC_MENTNEUTOUT             90
#define LIC_MENTNEUTOUT_S           "Mentor Neutral File Write"

// MXT
#define LIC_MXTOUT                  91
#define LIC_MXTOUT_S                "MXT Write"

// ODB++ VALOR
#define LIC_ODBPPOUT                 92
#define LIC_ODBPPOUT_S              "ODB++ Write"

// ORCADLTD
#define LIC_ORCADLTDOUT             93
#define LIC_ORCADLTDOUT_S           "Orcad Layout Plus Write"

// PADSPCB
#define LIC_PADSPCBOUT              94
#define LIC_PADSPCBOUT_S            "Pads Power V3 Graphic Write"

// PADSSCH
#define LIC_PADSSCHOUT              95
#define LIC_PADSSCHOUT_S            "Pads Logic Graphic Write"

// PDIFPCB
#define LIC_PDIFPCBOUT              96
#define LIC_PDIFPCBOUT_S            "PCAD PDIF Layout Graphic Write"

// PDIFSCH
#define LIC_PDIFSCHOUT              97
#define LIC_PDIFSCHOUT_S            "PDIF Schematic Graphic Write"

// PFW
#define LIC_PFWOUT                  98
#define LIC_PFWOUT_S                "Protel PFW V2.8 Layout Write"

// PFWSCH
#define LIC_PFWSCHOUT               99
#define LIC_PFWSCHOUT_S             "Protel PFW Schematic Write"

// QUAD
#define LIC_QUADOUT                 100
#define LIC_QUADOUT_S               "QUAD Write"

// RTN
#define LIC_RTN9OUT                 101
#define LIC_RTN9OUT_S               "RTN Rev 9 Write"
#define LIC_RTN11OUT                102
#define LIC_RTN11OUT_S              "RTN Rev 11 Write"

// RDC - Research Development Center s.r.l
#define LIC_RDCOUT                  103
#define LIC_RDCOUT_S                "RDC-Dbframe Write"

// SIEMENS QD
#define LIC_SIEMENS_QDOUT           104
#define LIC_SIEMENS_QDOUT_S         "Siemens QD Write"

// TAN
#define LIC_TANOUT                  105
#define LIC_TANOUT_S                "TANGO Series II Write"

//Takaya
#define LIC_TAKAYA8OUT              106   
#define LIC_TAKAYA8OUT_S            "TAKAYA .CA8 Write"
#define LIC_TAKAYA9OUT              107      
#define LIC_TAKAYA9OUT_S            "TAKAYA .CA9 Write"

// Teradyne AOI 7300 IPL
#define LIC_TERADYNE_7300OUT        108
#define LIC_TERADYNE_7300OUT_S      "Teradyne 7300 IPL"

// Teradyne AOI 7200 IPL
#define LIC_TERADYNE_7200OUT        109
#define LIC_TERADYNE_7200OUT_S      "Teradyne 7200 I2I"

// THEDA
#define LIC_THEDAOUT                110
#define LIC_THEDAOUT_S              "THEDA (.tl) Layout, Panel Write"

// TRI-AOI 
#define LIC_TRI_AOIOUT              111
#define LIC_TRI_AOIOUT_S            "TRI-AOI Write"

// UNICAM PDW
#define LIC_UNICAMOUT               112
#define LIC_UNICAMOUT_S             "UNICAM PDW Write"

// SPEA 4040
#define LIC_SPEA_4040               118
#define LIC_SPEA_4040_S             "SPEA 4040 Write"

// Teradyne (GenRad)228X/TestStation CKT
#define LIC_CKT							123
#define LIC_CKT_S							"Teradyne (GenRad) 228X/TestStation CKT & NAV"

// Fixture File
#define LIC_FIXTURE						124
#define LIC_FIXTURE_S					"Fixture File"

// Viscom
#define LIC_VISCOM_AOI					125
#define LIC_VISCOM_AOI_S				"Viscom AOI"

// Asymtex
#define LIC_ASYMTEK						126
#define LIC_ASYMTEK_S					"Asymtek Dispenser"

#define LIC_NAR_IN						127
#define LIC_NAR_IN_S						"Nail Assignment Report Read (.NAR)"

// Aeroflex
#define LIC_AEROFLEX_OUT				128
#define LIC_AEROFLEX_OUT_S				"Aeroflex .CB Write"

// MVP 3D Paste AOI
#define LIC_MVP_3D_PASTE_AOI_OUT		129
#define LIC_MVP_3D_PASTE_AOI_OUT_S	"MVP 3D Paste AOI Write"

// MVP Component AOI
#define LIC_MVP_COMPONENT_AOI_OUT	130
#define LIC_MVP_COMPONENT_AOI_OUT_S	"MVP Component AOI Write"

// Orbotech
#define LIC_ORBOTECH_OUT				131
#define LIC_ORBOTECH_OUT_S				"Orbotech AOI Write"

// Scorpion
#define LIC_SCORPION_OUT				132
#define LIC_SCORPION_OUT_S				"Scorpion Write"

// RTI
#define LIC_RTI_OUT						133
#define LIC_RTI_OUT_S				   "CR Technology RTI-6500 AOI Write"

// Fab Master Device
#define LIC_FabMasterDeviceRead		134
#define LIC_FabMasterDeviceRead_S	"FabMaster Device.asc Read"

// Seica Par Nod
#define LIC_SeicaParNod_OUT			135
#define LIC_SeicaParNod_OUT_S			"Seica .par .nod Write"

// Digital Test
#define LIC_DIGITALTEST_OUT			136
#define LIC_DIGITALTEST_OUT_S			"Digital Test Write"

// Teradyne Spectrum
#define LIC_TERADYNE_SPECTRUM       139   // was originally 138 before porting LIC_StencilStepper from 4.4 - knv 
#define LIC_TERADYNE_SPECTRUM_S	   "Teradyne Spectrum Write"

// Fixture Reuse
#define Fixture_Reuse               140
#define Fixture_Reuse_S             "Fixture Reuse Analysis"

// Sony AOI
#define LIC_SONY_AOI_OUT				141
#define LIC_SONY_AOI_OUT_S				"Sony AOI Write"

// Siemens
#define LIC_SIEMENS_BOARD_WRITE		142
#define LIC_SIEMENS_BOARD_WRITE_S	"Siemens Board Write"

// Royonic
#define LIC_ROYONICS_OUT				143
#define LIC_ROYONICS_OUT_S				"Royonics 500 Write"

// Generic Centroid
#define LIC_GenericCentroid_IN		144	
#define LIC_GenericCentroid_IN_S		"Generic Centroid Read"

// Juki
#define LIC_JUKI_OUT						145
#define LIC_JUKI_OUT_S					"Juki Write"

// VITech
#define LIC_VITECH_OUT					146
#define LIC_VITECH_OUT_S				"VI Technology AOI Write"

// SPECTRUM NAILREQ
#define LIC_SPECTRUMNAILREQ_IN      147
#define LIC_SPECTRUMNAILREQ_IN_S    "Spectrum Nailreq Read"

// Konrad ICT
#define LIC_KONRAD_ICT_OUT			   148
#define LIC_KONRAD_ICT_OUT_S			"Konrad ICT Write"

#define LIC_MAXENTRY                149 // Largest license +1


#define  ACCESS_NONE                0
#define  ACCESS_DEFAULT             1
#define  ACCESS_INPUT               2
#define  ACCESS_OUTPUT              4
#define  ACCESS_PRINT               8
#define  ACCESS_EDIT                16
#define  ACCESS_FORTEST             32		// Used to filter out licenses from visibility in builds other than
														// the Release w/ Debug Info, Release Test and Debug


// flag so can OR a license to be available to more than one product (eg. CPRINT)
// any licenses that may get different prices should get their own license
#define  PRODUCT_GRAPHIC         0x00000001
#define  PRODUCT_PROFESSIONAL    0x00000002
#define  PRODUCT_PCB_TRANSLATOR  0x00000004
#define  PRODUCT_VISION          0x00000008

#define  PRODUCT_ALL             0xffffffff
#define  PRODUCT_NONE            0

#define  MAX_LICENSES            1000

#define NLF_String "CAMCAD Network License File"

struct NetLicense
{
   short dummy1,     // random
         num,        // licenses available
         dummy2,     // random
         checksum,   // num + max + dummy1 + dummy2
         max;        // licenses purchased
};

unsigned long hash(const char *c);
unsigned long crypt(unsigned long sn, int index);


//-----------------------------------------------------------------------------
// License
//-----------------------------------------------------------------------------
class License //typedef struct
{
public:
   License();
   ~License();

   unsigned long  originalaccesscode;
   CString        visibleName;   // licence name in CAMCAD
   CString        licname;       // license name used with FLEXlm (Product_licName)
   unsigned long  product;       // product
   char           type;          // type of license (in, out, print, edit, ...)
   int            fileType;      // CAD system type (defined in DBUTIL.H)
   char           api;           // flag if licensed for an API
   int            allowed;       // allowed flag
   unsigned long  accesscode;
};

//void init_licarray();   // this load the text strings into the lic array


#endif