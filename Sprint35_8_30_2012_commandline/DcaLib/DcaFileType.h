// $Header: /CAMCAD/DcaLib/DcaFileType.h 6     5/22/07 1:37a Rick Faltersack $

#if !defined(__DcaFileType_h__)
#define __DcaFileType_h__

#pragma once

// These really are CADSystemType more than FileType, probably should rename the enum.
// Mostly this is used to designate what CAD System was source of CCZ <File> data.
// Appears as value for sourceCAD tag in CCZ.

enum FileTypeTag
{
   fileTypeUnknown               =  0 ,   //  #define Type_Unknown                 0    
   fileTypeDxf                   =  1 ,   //  #define Type_DXF                     1
   fileTypeHpgl                  =  2 ,   //  #define Type_HPGL                    2
   fileTypeGerber                =  3 ,   //  #define Type_Gerber                  3
   fileTypeAperture              =  4 ,   //  #define Type_Aperture                4
   fileTypeUnicam                =  5 ,   //  #define Type_UNICAM                  5
   fileTypeCdi                   =  6 ,   //  #define Type_CDI                     6
   fileTypePadsLayout            =  7 ,   //  #define Type_PADS_Layout             7
   fileTypePdifLayout            =  8 ,   //  #define Type_PDIF_Layout             8
   fileTypePdifSchematic         =  9 ,   //  #define Type_PDIF_Schematic          9 
   fileTypeProtelLayout          = 10 ,   //  #define Type_PROTEL_Layout          10
   fileTypeTangoLayout           = 11 ,   //  #define Type_Tango_Layout           11
   fileTypeMentorLayout          = 12 ,   //  #define Type_Mentor_Layout          12
   fileTypeProtelSchematic       = 13 ,   //  #define Type_Protel_Schematic       13
   fileTypePadsSchematic         = 14 ,   //  #define Type_PADS_Schematic         14
   fileTypeExcellon              = 15 ,   //  #define Type_Excellon               15
   fileTypeIges                  = 16 ,   //  #define Type_IGES                   16
   fileTypeIpc                   = 17 ,   //  #define Type_IPC                    17
   fileTypeEdif                  = 18 ,   //  #define Type_EDIF                   18
   fileTypeVb99Layout            = 19 ,   //  #define Type_VB99_Layout            19
   fileTypeCadencePlot           = 20 ,   //  #define Type_CADENCE_Plot           20
   fileTypeGencad                = 21 ,   //  #define Type_GENCAD                 21
   fileTypeBarco                 = 22 ,   //  #define Type_Barco                  22
   fileTypeOrcadLtdLayout        = 23 ,   //  #define Type_ORCAD_LTD_Layout       23
   fileTypeCasePlot              = 24 ,   //  #define Type_CASE_Plot              24
   fileTypeAllegroLayout         = 25 ,   //  #define Type_Allegro_Layout         25
   fileTypeMentorNeutralLayout   = 26 ,   //  #define Type_Mentor_Neutral_Layout  26
   fileTypeBom                   = 27 ,   //  #define Type_BOM                    27
   fileTypeHp3070                = 28 ,   //  #define Type_HP3070                 28
   fileTypeCadifLayout           = 29 ,   //  #define Type_CADIF_Layout           29
   fileTypeAccelLayout           = 30 ,   //  #define Type_ACCEL_Layout           30
   fileTypeEifLayout             = 31 ,   //  #define Type_EIF_Layout             31
   fileTypeQuadLayout            = 32 ,   //  #define Type_QUAD_Layout            32
   fileTypeHyperlynxLayout       = 33 ,   //  #define Type_HYPERLYNX_Layout       33
   fileTypeCcmLayout             = 34 ,   //  #define Type_CCM_Layout             34
   fileTypeThedaLayout           = 35 ,   //  #define Type_THEDA_Layout           35
   fileTypeHpegs                 = 36 ,   //  #define Type_HPEGS                  36
   fileTypeAutocadAdi            = 37 ,   //  #define Type_Autocad_ADI            37
   fileTypeScicardsEncore        = 38 ,   //  #define Type_CII_Layout             38  
   fileType3DLayout              = 39 ,   //  #define Type_3D_Layout              39
   fileTypeRdcDbframe            = 40 ,   //  #define Type_RDC_Dbframe            40
   fileTypeCr3000Layout          = 41 ,   //  #define Type_CR3000_Layout          41    
   fileTypeCctLayout             = 42 ,   //  #define Type_CCT_Layout             42     
   fileTypeRtn                   = 43 ,   //  #define Type_RTN                    43     
   fileTypeHp5dx                 = 44 ,   //  #define Type_HP5DX                  44
   fileTypeGencam                = 45 ,   //  #define Type_GENCAM                 45          
   fileTypeBnrLayout             = 46 ,   //  #define Type_BNR_Layout             46
   fileTypeTakaya8               = 47 ,   //  #define Type_TAKAYA8                47
   fileTypeOdbPlusPlus           = 48 ,   //  #define Type_ODB_PP                 48
   fileTypeDmis                  = 49 ,   //  #define Type_DMIS                   49
   fileTypeHughes                = 50 ,   //  #define Type_HUGHES                 50
   fileTypeViewdraw              = 51 ,   //  #define Type_VIEWDRAW               51            
   fileTypeFabMaster             = 52 ,   //  #define Type_FABMASTER              52          
   fileTypeTriMda                = 53 ,   //  #define Type_INGUN                  53             
   fileTypeAgilentAoi            = 54 ,   //  #define Type_Agilent_AOI            54
   fileTypeTeradyne7200          = 55 ,   //  #define Type_TERADYNE_7200          55
   fileTypeTeradyne7300          = 56 ,   //  #define Type_TERADYNE_7300          56       
   fileTypeDdeLayout             = 57 ,   //  #define Type_DDE_Layout             57          
   fileTypeXmlSvg                = 58 ,   //  #define Type_XMLSVG                 58 --- deleted
   fileTypeMxt                   = 59 ,   //  #define Type_MXT                    59                
   fileTypeCr5000Layout          = 60 ,   //  #define Type_CR5000_Layout          60       
   fileTypeFabNail               = 61 ,   //  #define Type_FAB_NAIL               61            
   fileTypeUltiboard             = 62 ,   //  #define Type_ULTIBOARD              62           
   fileTypeCalayPrismaLayout     = 63 ,   //  #define Type_CALAY_PRISMA_Layout    63 
   fileTypeUnidat                = 64 ,   //  #define Type_UNIDAT                 64              
   fileTypeTriAoi                = 65 ,   //  #define Type_TRI_AOI                65            
   fileTypeHuntron               = 66 ,   //  #define Type_Huntron                66             
   fileTypeTakaya9               = 67 ,   //  #define Type_TAKAYA9                67             
   fileTypeSiemensQd             = 68 ,   //  #define Type_Siemens_QD             68          
   fileTypeAlcatelDocica         = 69 ,   //  #define Type_Alcatel_DOCICA         69     
   fileTypeAlcatelGmf            = 70 ,   //  #define Type_Alcatel_GMF            70         
   fileTypePadsLibrary           = 71 ,   //  #define Type_PADS_Library           71        
   fileTypeSpea4040              = 72 ,   //  #define Type_SPEA_4040              72           
   fileTypeIpl                   = 73 ,   //  #define Type_IPL                    73                
   fileTypeAgilentSjPlx          = 74 ,   //  #define Type_AgilentSjPlx           74        
   fileTypeTeradyne228xCkt       = 75 ,   //  #define Type_CKT                    75              
   fileTypeFixture               = 76 ,   //  #define Type_FIXTURE                76           
   fileTypeViscomAoi             = 77 ,   //  #define Type_ViscomAOI              77           
   fileTypeAsymtek               = 78 ,   //  #define Type_Asymtek                78           
   fileTypeNar                   = 79 ,   //  #define Type_Nar                    79                 
   fileTypeAeroflexCb            = 80 ,   //  #define Type_AeroflexCB             80        
   fileTypeMvp3DPasteAoi         = 81 ,   //  #define Type_MVP_3D_Paste_AOI       81  
   fileTypeMvpComponentAoi       = 82 ,   //  #define Type_MVP_Component_AOI      82  
   fileTypeOrbotech              = 83 ,   //  #define Type_Orbotech               83
   fileTypeScorpion              = 84 ,   //  #define Type_Scorpion               84           
   fileTypeRti                   = 85 ,   //  #define Type_RTI                    85                 
   fileTypeFabMasterDevice       = 86 ,   //  #define Type_FabMasterDevice        86   
   fileTypeSeicaParNod           = 87 ,   //  #define Type_SeicaParNod            87
   fileTypeDigitalTest           = 88 ,   //  #define Type_DigitalTest            88
   fileTypeBOMCSV                = 89 ,   //  #define Type_BOMCSV                 89
   fileTypeTeradyneSpectrum      = 90 ,   //  #define Type_TeradyneSpectrum       90
   fileTypeSonyAoi               = 91 ,   //  #define Type_Sony_AOI               91
   fileTypeSiemensBoard          = 92 ,   //  #define Type_SiemensBoard           92
   fileTypeRoyonicsOut           = 93 ,   //  #define Type_RoyonicsOut            93
   fileTypeGenericCentroid       = 94 ,   //  #define Type_GenericCentroid        94
   fileTypeJuki                  = 95 ,   //  #define Type_Juki                   95
   fileTypeVITech                = 96 ,   //  #define Type_VITech                 96
   fileTypeSpectrumNailreq       = 97 ,   //  #define Type_SpectrumNailreq        97
   fileTypeKonradICT             = 98 ,   //  #define Type_KonradICT              98
   fileTypeAcculogic             = 99 ,   //  #define Type_Acculogic              99
   fileTypeTeradyne228xNav       = 100,   //  Teradyne 228x Navigator
   fileTypeAgilentI1000ATD       = 101,   //  Agilent I1000 Writer
   fileTypeTestronicsICT         = 102,   //  Testronics ICT
   fileTypeAeroflexNail          = 103,   //  Aeroflex Nail
   fileTypeAeroflexNailWire      = 104,   //  Aeroflex Nail Wire
   fileTypeTestabilityReport     = 105,   //  CamCad Testability Report
   fileTypeSchematicNetlist      = 106,   //  Schematic Netlist, or Generic Netlist, is really a family of file formats
   fileTypeCamcadNetlist         = 107,   //  CAMCAD Netlist (.ccn)
   fileTypeVbKeyinNetlist        = 108,   //  VB Keyin Netlist
   fileTypeBoardstationNetlist   = 109,   //  Mentor Boardstation Netlist
   fileTypeViewlogicNetlist      = 110,   //  Viewlogic Netlist  (aka DxDesigner Netlist ?)
   fileTypePadsPowerPcbNetlist   = 111,   //  PADS POWERPCB NETLIST
   fileTypeCamcadUser            = 112,   //  User created with CAMCAD, e.g. Panel Files created with Panelization tool
   fileTypeMYDATAPCB             = 113,   //  #define Type_MYDATAPCB             106
   fileTypeCustomAssembly        = 114,   //  #define Type_CustomAssembly        107
   fileTypeFujiFlexa             = 115,   //  #define Type_FujiFlexa             108
   fileTypeExpedition            = 116,
   fileTypeFablink               = 117,
   fileTypeCellEditor            = 118,
   fileTypeSymbolicFloorPlanner  = 119,
   fileTypeVisECAD               = 120,
   fileTypeConstraintEditorSystem= 121,
   fileTypeCamcadData            = 122,   // Good old CC or CCZ, not really a CAD type like the others, used for CMDLine Import/Export feature
   fileTypeSakiAOI               = 123,   // Saki AOI
   fileTypeHiokiICT              = 124,   // Hioki ICT
   fileTypeCyberOpticsAOI        = 125,   // CyberOptics AOI
   fileTypeOmronAOI              = 126,   // Omron AOI
   fileTypeYestechAOI            = 127,   // YesTech AOI
   fileTypeBoardStationXE        = 128,   // BoardStation XE
   fileTypeBoardsim              = 129,   // Boardsim
   fileTypeLinesym               = 130,   // Linesim
   fileTypeHLDRC                 = 131,   // HyperLynx DRC
   

   fileTypeLowerBound            =   0,   //
   fileTypeUpperBound            = 131,   // Maintain same as highest actual file type value

   FileTypeTagMin                = fileTypeLowerBound,  // for use by EnumIterator
   FileTypeTagMax                = fileTypeUpperBound   // for use by EnumIterator
};

FileTypeTag intToFileTypeTag(int fileType);
FileTypeTag intToFileTypeTag(long fileType);
CString fileTypeTagToString(FileTypeTag tagValue);
CString fileTypeTagToPrettyString(FileTypeTag tagValue);
FileTypeTag stringToFileTypeTag(CString someString);


#endif
