// $Header: /CAMCAD/4.3/read_wrt/Edif_in.h 11    8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved. */

// EDIF_IN.H

#pragma once

#define  EDIF_UNKNOWN            0
#define  EDIF200                 1
#define  EDIF300                 2
#define  EDIF400                 3

#define  U_UNKNOWN               0
#define  U_DISTANCE              1

#define  TYPEVAL_UNKNOWN         0
#define  TYPEVAL_STRING          1

#define  EDIFERR                "edif.log"
#define  EDIFTEC                "edif.in"

#define  FNULL                   fnull

#define  MAX_LINE                2000 // Max line length
#define  MAX_TOKEN               MAX_LINE

#define  MAX_LAYERS              255 // Max number of layers
#define  MAX_SHAPE               1000
#define  MAX_EDIFUNITS           20

#define  LAY_SIGNAL              1   // Layer codes

#define  EDIF_SILKSCREEN         1
#define  EDIF_SOLDERMASK         2

#define  STAT_UNKNOWN            0
#define  STAT_FOOTPRINT          1

#define  FILETYPE_UNKNOWN        0
#define  FILETYPE_VIEWLOGIC      1

#define  BRK_ROUND               0
#define  BRK_B_ROUND             1

#define  SIZ_START_LST                          (sizeof(start_lst) / sizeof(List))
#define  SIZ_BRK_LST                            (sizeof(brk_lst) / sizeof(List))
#define  SIZ_EDIF_NAME_LST                      (sizeof(edif_name_lst) / sizeof(List))
#define  SIZ_EDIF_TYPEDVALUE_LST                (sizeof(edif_typedvalue_lst) / sizeof(List))
#define  SIZ_EDIF_STRINGDISPLAY_LST             (sizeof(edif_stringdisplay_lst) / sizeof(List))
#define  SIZ_DISPLAY_LST                        (sizeof(display_lst) / sizeof(List))
#define  SIZ_PROPDISPLAY_LST                    (sizeof(propdisplay_lst) / sizeof(List))
#define  SIZ_EDIF_DISPLAY_LST                   (sizeof(edif_display_lst) / sizeof(List))
#define  SIZ_EDIF_LST                           (sizeof(edif_lst) / sizeof(List))
#define  SIZ_HEADER_LST                         (sizeof(header_lst) / sizeof(List))
#define  SIZ_STATUS_LST                         (sizeof(status_lst) / sizeof(List))
#define  SIZ_WRITTEN_LST                        (sizeof(written_lst) / sizeof(List))
#define  SIZ_UNITDEFINITIONS_LST                (sizeof(unitDefinitions_lst) / sizeof(List))
#define  SIZ_PHYSICALSCALING_LST                (sizeof(physicalScaling_lst) / sizeof(List))
#define  SIZ_PHYSICALDEFAULTS_LST               (sizeof(physicalDefaults_lst) / sizeof(List))
#define  SIZ_PCBGEOMETRICATTRIBUTDEFAULT_LST    (sizeof(pcbGeometricAttributeDefault_lst) / sizeof(List))
#define  SIZ_PCBLAYOUTUNITS_LST                 (sizeof(pcbLayoutUnits_lst) / sizeof(List))
#define  SIZ_SCHEMATICUNITS_LST                 (sizeof(schematicUnits_lst) / sizeof(List))
#define  SIZ_SCHEMATICMETRIC_LST                (sizeof(schematicMetric_lst) / sizeof(List))
#define  SIZ_SCHEMATICSYMBOLHEADER_LST          (sizeof(schematicSymbolHeader_lst) / sizeof(List))
#define  SIZ_SCHEMATICREQUIREDDEFAULTS_LST      (sizeof(schematicRequiredDefaults_lst) / sizeof(List))
#define  SIZ_SETDISTANCE_LST                    (sizeof(setDistance_lst) / sizeof(List))
#define  SIZ_SETANGLE_LST                       (sizeof(setAngle_lst) / sizeof(List))
#define  SIZ_LIBRARY400_LST                     (sizeof(library400_lst) / sizeof(List))
#define  SIZ_LIBRARY300_LST                     (sizeof(library300_lst) / sizeof(List))
#define  SIZ_LIBRARY200_LST                     (sizeof(library200_lst) / sizeof(List))
#define  SIZ_FIGUREGR_LST                       (sizeof(figuregr_lst) / sizeof(List))
#define  SIZ_DISPLAYATTRIBUTES_LST              (sizeof(displayattributes_lst) / sizeof(List))
#define  SIZ_CELL400_LST                        (sizeof(cell400_lst) / sizeof(List))
#define  SIZ_LIBRARYHEADER400_LST               (sizeof(libraryheader400_lst) / sizeof(List))
#define  SIZ_LIBRARYHEADER300_LST               (sizeof(libraryheader300_lst) / sizeof(List))
#define  SIZ_VIEW200_LST                        (sizeof(view200_lst) / sizeof(List))
#define  SIZ_INTERFACE200_LST                   (sizeof(interface200_lst) / sizeof(List))
#define  SIZ_SYMBOL200_LST                      (sizeof(symbol200_lst) / sizeof(List))
#define  SIZ_PORTIMPLEMENTATION_LST             (sizeof(portimplementation_lst) / sizeof(List))
#define  SIZ_PAGE_PORTIMPLEMENTATION_LST        (sizeof(page_portimplementation_lst) / sizeof(List))
#define  SIZ_CONNECTLOCATION_LST                (sizeof(connectlocation_lst) / sizeof(List))
#define  SIZ_CONTENTS200_LST                    (sizeof(contents200_lst) / sizeof(List))
#define  SIZ_COMMENTGRAPHICS_LST                (sizeof(commentgraphics_lst) / sizeof(List))
#define  SIZ_NET200_LST                         (sizeof(net200_lst) / sizeof(List))
#define  SIZ_E_CONSTRUCT_LST                    (sizeof(e_construct_lst) / sizeof(List))
#define  SIZ_BOOLEANVALUE_LST                   (sizeof(booleanvalue_lst) / sizeof(List))
#define  SIZ_INSTANCE200_LST                    (sizeof(instance200_lst) / sizeof(List))
#define  SIZ_PORTINSTANCE200_LST                (sizeof(portinstance200_lst) / sizeof(List))
#define  SIZ_CELL200_LST                        (sizeof(cell200_lst) / sizeof(List))
#define  SIZ_TECHNOLOGY200_LST                  (sizeof(technology200_lst) / sizeof(List))
#define  SIZ_TECHNOLOGY400_LST                  (sizeof(technology400_lst) / sizeof(List))
#define  SIZ_TECHNOLOGY300_LST                  (sizeof(technology300_lst) / sizeof(List))
#define  SIZ_SCHEMATICFIGUREMACRO_LST           (sizeof(schematicFigureMacro_lst) / sizeof(List))
#define  SIZ_PAGETITLEBLOCKATTRIBUTES_LST       (sizeof(pageTitleBlockAttributes_lst) / sizeof(List))
#define  SIZ_PAGETITLEBLOCKATTRIBUTEDISPLAY_LST (sizeof(pageTitleBlockAttributeDisplay_lst) / sizeof(List))
#define  SIZ_ADDDISPLAY_LST                     (sizeof(addDisplay_lst) / sizeof(List))
#define  SIZ_SCHEMATICPORTMACRO_LST             (sizeof(schematicPortMacro_lst) / sizeof(List))
#define  SIZ_NUMBERDEFINITION_LST               (sizeof(numberdefinition_lst) / sizeof(List))
#define  SIZ_SCALE_LST                          (sizeof(scale_lst) / sizeof(List))
#define  SIZ_PAGE200_LST                        (sizeof(page200_lst) / sizeof(List))
#define  SIZ_PAGE300_LST                        (sizeof(page300_lst) / sizeof(List))
#define  SIZ_PAGEHEADER_LST                     (sizeof(pageheader_lst) / sizeof(List))
#define  SIZ_CLUSTER_LST                        (sizeof(cluster_lst) / sizeof(List))
#define  SIZ_PCBMCMVIEW_LST                     (sizeof(pcbMcmView_lst) / sizeof(List))
#define  SIZ_SCHEMATICSYMBOL_LST                (sizeof(schematicsymbol_lst) / sizeof(List))
#define  SIZ_PCBMCMIMPLEMENTATION_LST           (sizeof(pcbMcmImplementation_lst) / sizeof(List))
#define  SIZ_SCHEMATICVIEW_LST                  (sizeof(schematicview_lst) / sizeof(List))
#define  SIZ_LOGICALINSTANCE300_LST             (sizeof(logicalinstance300_lst) / sizeof(List))
#define  SIZ_SCHEMATICIMPLEMENTATION_LST        (sizeof(schematicImplementation_lst) / sizeof(List))
#define  SIZ_LOGICALCONNECTIVITY_LST            (sizeof(logicalConnectivity_lst) / sizeof(List))
#define  SIZ_PCBASSEMBLEDBOARD_LST              (sizeof(pcbAssembledBoard_lst) / sizeof(List))
#define  SIZ_PCBBAREBOARD_LST                   (sizeof(pcbBareBoard_lst) / sizeof(List))
#define  SIZ_PCBSTACKEDLAYERGROUP_LST           (sizeof(pcbStackedLayerGroup_lst) / sizeof(List))
#define  SIZ_PCBBAREBOARDLAYERS_LST             (sizeof(pcbBareBoardLayers_lst) / sizeof(List))
#define  SIZ_PCBDRAWING_LST                     (sizeof(pcbDrawing_lst) / sizeof(List))
#define  SIZ_PCBDRAWINGANNOTATION_LST           (sizeof(pcbDrawingAnnotation_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALLAYERHEADER_LST         (sizeof(pcbPhysicalLayerHeader_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALNETHEADER_LST           (sizeof(pcbPhysicalNetHeader_lst) / sizeof(List))
#define  SIZ_NAMEINFORMATION_LST                (sizeof(nameInformation_lst) / sizeof(List))
#define  SIZ_FOOTPRINT_LST                      (sizeof(footprint_lst) / sizeof(List))
#define  SIZ_PADSTACK_LST                       (sizeof(padstack_lst) / sizeof(List))
#define  SIZ_TOEPRINT_LST                       (sizeof(toeprint_lst) / sizeof(List))
#define  SIZ_PCBLAYOUTFEATURE_LST               (sizeof(pcbLayoutFeature_lst) / sizeof(List))
#define  SIZ_PCBINSTANCENETS_LST                (sizeof(pcbInstanceNets_lst) / sizeof(List))
#define  PCBSUBLAYOUTINSTANCEPHYSICALNETREF_LST (sizeof(pcbSubLayoutInstancePhysicalNetRef_lst) / sizeof(List))
#define  SIZ_PCBLAYOUTTEXT_LST                  (sizeof(pcbLayoutText_lst) / sizeof(List))
#define  SIZ_PCBNONSPECIFICLAYOUTSHAPE_LST      (sizeof(pcbNonSpecificLayoutShape_lst) / sizeof(List))
#define  SIZ_PCBFABRICATETEXTSET_LST            (sizeof(pcbFabricateTextSet_lst) / sizeof(List))
#define  SIZ_PCBBAREBOARDDEFINITION_LST         (sizeof(pcbbareboarddefinition_lst) / sizeof(List))
#define  SIZ_PCBBAREBOARDTECHNOLOGY_LST         (sizeof(pcbbareboardtechnology_lst) / sizeof(List))
#define  SIZ_PCBBAREBOARDOUTLINE_LST            (sizeof(pcbBareBoardOutline_lst) / sizeof(List))
#define  SIZ_PCBPLANE_LST                       (sizeof(pcbPlane_lst) / sizeof(List))
#define  SIZ_PCBTRACE_LST                       (sizeof(pcbTrace_lst) / sizeof(List))
#define  SIZ_PCBPAD_LST                         (sizeof(pcbPad_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALNET_LST                 (sizeof(pcbPhysicalNet_lst) / sizeof(List))
#define  SIZ_PCBPADSTACKINSTANCE_LST            (sizeof(pcbPadStackInstance_lst) / sizeof(List))
#define  SIZ_PCBTOEPRINTINSTANCE_LST            (sizeof(pcbToeprintInstance_lst) / sizeof(List))
#define  SIZ_PCBBAREBOARDPHYSICALNET_LST        (sizeof(pcbBareBoardPhysicalNet_lst) / sizeof(List))
#define  SIZ_PCBFOOTPRINTINSTANCE_LST           (sizeof(pcbFootprintInstance_lst) / sizeof(List))
#define  SIZ_FIXEDSCALETRANSFORM_LST            (sizeof(fixedScaleTransform_lst) / sizeof(List))
#define  SIZ_TRANSFORM_LST                      (sizeof(transform_lst) / sizeof(List))
#define  SIZ_FIGUREGROUPOVERRIDE_LST            (sizeof(figuregroupoverride_lst) / sizeof(List))
#define  SIZ_MIRRORING_LST                      (sizeof(mirroring_lst) / sizeof(List))
#define  SIZ_PCBFABRICATEFIGURE_LST             (sizeof(pcbFabricateFigure_lst) / sizeof(List))
#define  SIZ_PCBFABRICATETEXT_LST               (sizeof(pcbFabricateText_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALTEXT_LST                (sizeof(pcbPhysicalText_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALTEXTSET_LST             (sizeof(pcbPhysicalTextSet_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALTEXTIMPLEMENTATION_LST  (sizeof(pcbPhysicalTextImplementation_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALTEXTAPPEARANCE_LST      (sizeof(pcbTextAppearance_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALFIGUREMACRO_LST         (sizeof(pcbPhysicalFigureMacro_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALFIGURE_LST              (sizeof(pcbPhysicalFigure_lst) / sizeof(List))
#define  SIZ_PCBFIGUREATTRIBUTEOVERRIDE_LST     (sizeof(pcbFigureAttributeOverride_lst) / sizeof(List))
#define  SIZ_PCBDRAWINGTEXTDRAWINGATTRIBUTEOVERRIDE_LST (sizeof(pcbDrawingTextDrawingAttributeOverride_lst) / sizeof(List))
#define  SIZ_PCBDRAWINGFIGURE_LST               (sizeof(pcbDrawingFigure_lst) / sizeof(List))
#define  SIZ_PCBPHYSICALLAYER_LST               (sizeof(pcbPhysicalLayer_lst) / sizeof(List))
#define  SIZ_PCBNONCONDUCTINGLAYER_LST          (sizeof(pcbNonConductingLayer_lst) / sizeof(List))
#define  SIZ_PCBDRAWINGTEXT_LST                 (sizeof(pcbDrawingText_lst) / sizeof(List))
#define  SIZ_PCBDRAWINGTEXTDISPLAY_LST          (sizeof(pcbDrawingTextDisplay_lst) / sizeof(List))
#define  SIZ_PCBCOMPLEXPHYSICALFIGURE_LST       (sizeof(pcbComplexPhysicalFigure_lst) / sizeof(List))
#define  SIZ_SCHEMATICCOMPLEXFIGURE_LST         (sizeof(schematicComplexFigure_lst) / sizeof(List))
#define  SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST (sizeof(schematicInstanceImplementation_lst) / sizeof(List))
#define  SIZ_SCHEMATICNET_LST                   (sizeof(schematicNet_lst) / sizeof(List))
#define  SIZ_SCHEMATICBUS_LST                   (sizeof(schematicBus_lst) / sizeof(List))
#define  SIZ_SCHEMATICNETGRAPHICS_LST           (sizeof(schematicNetGraphics_lst) / sizeof(List))
#define  SIZ_SCHEMATICBUSGRAPHICS_LST           (sizeof(schematicBusGraphics_lst) / sizeof(List))
#define  SIZ_SCHEMATICSUBNETSET_LST             (sizeof(schematicSubNetSet_lst) / sizeof(List))
#define  SIZ_PCBCONTRIBUTESTOFABRICATION_LST    (sizeof(pcbContributesToFabrication_lst) / sizeof(List))
#define  SIZ_GEOMETRYELEMENTSET_LST             (sizeof(geometryElementSet_lst) / sizeof(List))
#define  SIZ_DESIGN_LST                         (sizeof(design_lst) / sizeof(List))
#define  SIZ_CELLREF_LST                        (sizeof(cellRef_lst) / sizeof(List))
#define  SIZ_VIEWREF200_LST                     (sizeof(viewref200_lst) / sizeof(List))
#define  SIZ_CLUSTERREF_LST                     (sizeof(clusterref_lst) / sizeof(List))
#define  SIZ_PT_LST                             (sizeof(pt_lst) / sizeof(List))
#define  SIZ_POINTLIST_LST                      (sizeof(pointList_lst) / sizeof(List))
#define  SIZ_CURVELIST_LST                      (sizeof(curveList_lst) / sizeof(List))
#define  SIZ_ARC_LST                            (sizeof(arc_lst) / sizeof(List))
#define  SIZ_SHAPEPT_LST                        (sizeof(shapept_lst) / sizeof(List))
#define  SIZ_LAYERSTACKLAYER_LST                (sizeof(layerstacklayer_lst) / sizeof(List))
#define  SIZ_DRILL_LST                          (sizeof(drill_lst) / sizeof(List))
#define  SIZ_PACKAGE_LST                        (sizeof(package_lst) / sizeof(List))
#define  SIZ_LOCATION_LST                       (sizeof(location_lst) / sizeof(List))
#define  SIZ_DESIGNHIERARCHY_LST                (sizeof(designHierarchy_lst) / sizeof(List))
#define  SIZ_OCCURRENCEHIERARCHYANNOTATE_LST    (sizeof(occurrenceHierarchyAnnotate_lst) / sizeof(List))
#define  SIZ_OCCURRENCEANNOTATE_LST             (sizeof(occurrenceAnnotate_lst) / sizeof(List))
#define  SIZ_LEAFOCCURRENCEANNOTATE_LST         (sizeof(leafOccurrenceAnnotate_lst) / sizeof(List))
#define  SIZ_HORIZONTALJUSTIFICATION_LST        (sizeof(horizontalJustification_lst) / sizeof(List))
#define  SIZ_VERTICALJUSTIFICATION_LST          (sizeof(verticalJustification_lst) / sizeof(List))
#define  SIZ_PROPERTYDISPLAYOVERRIDE_LST        (sizeof(propertyDisplayOverride_lst) / sizeof(List))
#define  SIZ_PORTATTRIBUTEDISPLAY_LST           (sizeof(portAttributeDisplay_lst) / sizeof(List))
#define  SIZ_INSTANCEPORTATTRIBUTES_LST         (sizeof(instancePortAttributes_lst) / sizeof(List))
#define  SIZ_INSTANCEPORTATTRIBUTEDISPLAY_LST   (sizeof(instancePortAttributeDisplay_lst) / sizeof(List))

typedef struct
{
   CString  name;
   double   n1,n2;
}EdifUnit;

typedef struct
{
   double   x,y;
} Coor;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
}EDIFAttribmap;
typedef CTypedPtrArray<CPtrArray, EDIFAttribmap*> CAttribmapArray;

typedef struct
{
   double   textheightcorrect;
   double   textratio;
   int      attributeplacement;
   CString  programname;
}EDIFSettings;
typedef CTypedPtrArray<CPtrArray, EDIFSettings*> CSettingsArray;

typedef struct
{
   int      visible;
   int      cur_layerindex;
   double   x,y;
   double   rotation; 
   int      mirror;
   double   height,width;
}EDIF_Attrib;

typedef struct
{
   int      renameindex;
   int      orignameindex;
   int      loop_depth;    // only renames of the same level can be evaluated.
}EDIFRename;
typedef CArray<EDIFRename, EDIFRename&> CRenameArray;

typedef struct
{
   double d;
   int    toolindex;
}EDIFDrill;
typedef CArray<EDIFDrill, EDIFDrill&> CDrillArray;

typedef struct
{
   int   shapeindex;
   int   pinindex;
   int   netpinindex;
}EDIFShapePinNet;
typedef CArray<EDIFShapePinNet, EDIFShapePinNet&> CShapePinNet;

typedef struct
{
   CString  name;
   double   textheight;
   int      pathwidthindex;
}Figuregrp;
typedef CTypedPtrArray<CPtrArray, Figuregrp*> CFiguregrArray;

typedef struct
{
   CString  name;
   int      widthindex;
}Geometrygrp;
typedef CTypedPtrArray<CPtrArray, Geometrygrp*> CGeometrygrpArray;

typedef struct
{
   CString  name;
}EDIFName;
typedef CTypedPtrArray<CPtrArray, EDIFName*> CNameArray;

typedef struct
{
   CString     instance;
   CString     cellref;
   CString     libraryref;
   CString     nameinformation;
   DataStruct  *data;         // holder for attributes
   CDataList   *datalist;     // need to remove
}EDIFLogicalInstance;
typedef CTypedPtrArray<CPtrArray, EDIFLogicalInstance*> CLogicalInstanceArray;

typedef struct
{
   CString  name;                // L1..
   CString  username;            // name information
   int      artworkstacknumber;  // from pcbStackedLayerGroup
   int      conductive;
   int      nonconductivetype;   // silk, etc...
}EDIFLayerlist;

typedef  struct
{
   char  *token;
   int      (*function)();
} List;


/*** Start of EDIF file ***/

static int fnull();     // this writes to log file
static int fskip();     // fskip does not write to log file

/* start_lst */
static int      start_edif();

static List  start_lst[] =
{
   "edif",                 start_edif,
};

/* brk_lst */
static List brk_lst[] =
{
   "(",                    FNULL,
   ")",                    FNULL,
};

/* edif_name_lst */
static int edif_rename();
static int edif_name();

static List edif_name_lst[] =
{
   "rename",               edif_rename,
   "name",                 edif_name,
};

/* edif_typedvalue_lst */
static int edif_string();

static List edif_typedvalue_lst[] =
{
   "string",               edif_string,
};

/* edif_stringdisplay_lst */
static int stringdisplay();

static List edif_stringdisplay_lst[] =
{
   "stringDisplay",        stringdisplay,
};

/* display_lst */
static int display();

static List display_lst[] =
{
   "Display",              display,
};

/* propdisplay_lst */
static int propdisplay();

static List propdisplay_lst[] =
{
   "Display",              propdisplay,
};

/* edif_display_lst */
static int edif_origin();
static int edif_mirroring();
static int edif_orientation();
static int edif_justify();
static int edif_figuregroupoverride();
static int edif_transform();

static List edif_display_lst[] =
{
   "figureGroupOverride",  edif_figuregroupoverride,
   "origin",               edif_origin,
   "orientation",          edif_orientation,
   "justify",              edif_justify,
   "transform",            edif_transform,
};

/* edif_lst */
static int edif_version();
static int edif_level();
static int edif_header();
static int edif_header();
static int edif_library();
static int edif_design();
static int edif_status();

static List edif_lst[] =
{
   "edifVersion",          edif_version,
   "edifLevel",            edif_level,
   "edifHeader",           edif_header,
   "comment",              fskip,
   "external",             fskip,
   "keywordMap",           fskip,
   "status",               edif_status,
   "library",              edif_library,        
   "userdata",             fskip,
   "design",               edif_design,
};

/* header_lst */
static int unitDefinitions();
static int physicalScaling();
static int physicalDefaults();

static List header_lst[] =
{
   "edifLevel",                     fskip,
   "keywordMap",                    fskip,
   "unitDefinitions",               unitDefinitions,
   "fontDefinitions",               fskip,
   "physicalDefaults",              physicalDefaults,
   "characterEncoding",             fskip,
   "characterValues",               fskip,
   "documentation",                 fskip,
   "globalPortDefinitions",         fskip,
   "nameCaseSensitivity",           fskip,
   "nameInformation",               fskip,
   "pcbFigureCollectorDefinitions", fskip,
   "physicalScaling",               physicalScaling,
   "property",                      fskip,
   "status",                        edif_status,
};

/* status_lst */
static int status_written();

static List status_lst[] =
{
   "written",                       status_written,
};

/* written_lst */
static int written_program();    // used to set parameters

static List written_lst[] =
{
   "timestamp",                     fskip,
   "program",                       written_program,
};

/* unitDefinitions_lst */
static int unit400();

static List unitDefinitions_lst[] =
{
   "unit",                    unit400,
};

/* physicalScaling_lst */
static int pcbLayoutUnits();
static int schematicUnits();

static List physicalScaling_lst[] =
{
   "pcbLayoutUnits",          pcbLayoutUnits,
   "schematicUnits",          schematicUnits,
};

/* physicalDefaults_lst */
static int schematicRequiredDefaults();

static List physicalDefaults_lst[] =
{
   "schematicRequiredDefaults", schematicRequiredDefaults,
};

/* pcbGeometricAttributeDefault_lst */
static int edif_width();

static List pcbGeometricAttributeDefault_lst[] =
{
   "pathWidth",            edif_width,
};

/* pcbLayoutUnits_lst */
static int setDistance();
static int setAngle();

static List pcbLayoutUnits_lst[] =
{
   "setDistance",          setDistance,
};

/* schematicUnits_lst */
static int schematicMetric();

static List schematicUnits_lst[] =
{
   "schematicMetric",      schematicMetric,
};

/* schematicMetric_lst */
static List schematicMetric_lst[] =
{
   "setDistance",          setDistance,
};

/* schematicSymbolHeader_lst */
static int edif_property();

static List schematicSymbolHeader_lst[] =
{
   "schematicUnits",       schematicUnits,
   "property",             edif_property,
};

/* schematicRequiredDefaults_lst */
static List schematicRequiredDefaults_lst[] =
{
   "schematicMetric",      schematicMetric,
};

/* setDistance_lst */
static int unitRef();

static List setDistance_lst[] =
{
   "unitRef",              unitRef,
};

/* setAngle_lst */
static List setAngle_lst[] =
{
   "unitRef",              unitRef,
};

/* library400_lst */
static int library400_cell(); 
static int library_pcbfootprint();  
static int library_pcbbareboarddefinition();
static int library_pcbbareboardtechnology();
static int library_pcbphysicalfiguremacro();
static int library_pcbpadstackdefintion();
static int library_pcbtoeprintdefintion();
static int library400_header();

static List library400_lst[] =
{
   "libraryHeader",           library400_header,
   "cell",                    library400_cell,
   "comment",                 fskip,
   "pcbPhysicalFigureMacro",  library_pcbphysicalfiguremacro,
   "pcbBondPadDrawingTemplate",fskip,
   "pcbBondPadTemplate",      fskip,
   "pcbDielectricalPart",     fskip,
   "pcbDieFiducialTemplate",  fskip,
   "pcbPinTemplate",          fskip,
   "pcbBareBoardTechnology",  library_pcbbareboardtechnology,
   "pcbMountingMethod",       fskip,
   "pcbMountablePinPackage",  fskip,
   "pcbFootprintDefinition",  library_pcbfootprint,
   "pcbBareBoardDefinition",  library_pcbbareboarddefinition,
   "pcbPadstackDefinition",   library_pcbpadstackdefintion,
   "pcbToeprintDefinition",   library_pcbtoeprintdefintion,
};

/* library300_lst */
static int library300_header();
static int library300_cell();
static int library300_schematicFigureMacro();
static int library300_pageTitleBlockTemplate();
static int library300_pageBorderTemplate();
static int library300_schematicGlobalPortTemplate();
static int library300_schematicSymbolPortTemplate();
static int library300_schematicOffPageConnectorTemplate();
static int library300_schematicOnPageConnectorTemplate();

static List library300_lst[] =
{
   "cell",                             library300_cell,
   "comment",                          fskip,
   "libraryHeader",                    library300_header,
   "schematicGlobalPortTemplate",      library300_schematicGlobalPortTemplate,
   "schematicSymbolPortTemplate",      library300_schematicSymbolPortTemplate,
   "schematicMasterPortTemplate",      library300_schematicSymbolPortTemplate,
   "schematicFigureMacro",             library300_schematicFigureMacro,
   "pageTitleBlockTemplate",           library300_pageTitleBlockTemplate,
   "pageBorderTemplate",               library300_pageBorderTemplate,
   "schematicOffPageConnectorTemplate",library300_schematicOffPageConnectorTemplate,
   "schematicOnPageConnectorTemplate", library300_schematicOnPageConnectorTemplate,
};

/* library200_lst */
static int library200_cell(); 
static int library200_technology();

static List library200_lst[] =
{
   "cell",                    library200_cell,
   "comment",                 fskip,
   "edifLevel",               edif_level,
   "technology",              library200_technology,
   "userData",                fskip,
};

/* figuregr_lst */
static int edif_textheight();
static int edif_color();
static int edif_visible();
static int edif_displayattributes();

static List figuregr_lst[] =
{
   "textheight",              edif_textheight,
   "color",                   edif_color,
   "pathWidth",               edif_width,
   "visible",                 edif_visible,
   "displayAttributes",       edif_displayattributes,
   "property",                fskip,
};

/* displayattributes_lst */
static int horizontalJustification();
static int verticalJustification();

static List displayattributes_lst[] =
{
   "textheight",              edif_textheight,
   "color",                   edif_color,
   "horizontalJustification", horizontalJustification,
   "verticalJustification",   verticalJustification,
   "visible",                 edif_visible,
};

/* horizontalJustification_lst */
static int leftJustify();
static int centerJustify();
static int rightJustify();

static List horizontalJustification_lst[] =
{
   "leftJustify",             leftJustify,
   "centerJustify",           centerJustify,
   "rightJustify",            rightJustify,
};

/* verticalJustification_lst */
static int topJustify();
static int middleJustify();
static int bottomJustify();

static List verticalJustification_lst[]=
{
   "caplineJustify",          topJustify,
   "topJustify",              topJustify,
   "middleJustify",           middleJustify,
   "bottomJustify",           bottomJustify,
   "baselineJustify",         bottomJustify,
};

/* cell400_lst */
static int cluster();

static List cell400_lst[] =
{
   "cellHeader",              fskip,
   "cluster",                 cluster,
};

/* libraryheader400_lst */
static int technology400();

static List libraryheader400_lst[] =
{
   "technology",              technology400,
};

/* libraryheader300_lst */
static int technology300();

static List libraryheader300_lst[] =
{
   "edifLevel",               edif_level,
   "nameCaseSensitivity",     fskip,
   "technology",              technology300,
};

/* view200_lst */
static int view_viewtype();
static int view_contents();
static int view_interface();

static List view200_lst[] =
{
   "viewtype",                view_viewtype,
   "interface",               view_interface,
   "contents",                view_contents,
};

/* interface200_lst */
static int interface_port();
static int interface_symbol();

static List interface200_lst[] =
{
   "port",                    interface_port,
   "symbol",                  interface_symbol,
   "property",                edif_property,
};

/* symbol200_lst */
static int figure200();
static int edif_annotate();
static int portimplementation200();
static int edif_propertydisplay();
static int commentgraphics();

static List symbol200_lst[] =
{
   "portImplementation",      portimplementation200,
   "figure",                  figure200,
   "annotate",                edif_annotate,
   "propertydisplay",         edif_propertydisplay,
   "commentGraphics",         commentgraphics,
   "pagesize",                fskip,
   "boundingbox",             fskip,
};

/* portimplementation_lst */
static int connectlocation();
static int portnameref();

static List portimplementation_lst[] =
{
   "name",                    portnameref,   // like edif_nameref, but updates curblock
   "connectLocation",         connectlocation,
   "propertydisplay",         edif_propertydisplay,
   "figure",                  figure200,
};

/* page_portimplementation_lst */
static int instance200();

static List page_portimplementation_lst[] =
{
   "instance",                instance200,
};

/* connectlocation_lst */
static List connectlocation_lst[] =
{
   "figure",                  figure200,
};

/* contents200_lst */
static   int   page200();

static List  contents200_lst[] =
{
   "figure",                  figure200,
   "page",                    page200,
   "commentGraphics",         commentgraphics,
   "offpageconnector",        fskip,
};

/* commentgraphics_lst */
static int schematicComplexFigure();

static List commentgraphics_lst[] =
{
   "figure",                  figure200,
   "annotate",                edif_annotate,
   "instance",                instance200,
   "schematicComplexFigure",  schematicComplexFigure,
};

/* net200_lst */
static int joined200();

static List net200_lst[] =
{
   "figure",                  figure200,
   "instance",                instance200,
   "joined",                  joined200,
   "property",                edif_property,
};

/* e_construct_lst */
static int edif_e();

static List e_construct_lst[] =
{
   "e",                       edif_e,
};

/* booleanvalue_lst */
static int booleanvalue_true();
static int booleanvalue_false();

static List booleanvalue_lst[] =
{
   "true",                    booleanvalue_true,
   "false",                   booleanvalue_false,
};

/* instance200_lst */
static int instance_transform();
static int viewref200();
static int edif_designator();
static int edif_portinstance();

static List instance200_lst[] =
{
   "viewref",                 viewref200,
   "transform",               instance_transform,
   "property",                edif_property,
   "designator",              edif_designator,
   "portinstance",            edif_portinstance,
};

/* portinstance200_lst */
static int edif_portproperty();
static int edif_portdesignator();

static List portinstance200_lst[] =
{
   "property",                edif_portproperty,
   "designator",              edif_portdesignator,
};

/* cell200_lst */
static int cell200_view();
static int cell200_celltype();

static List cell200_lst[] =
{
   "cellType",                cell200_celltype,
   "view",                    cell200_view,
};

/* technology200_lst */
static int edif_numberdefinition();
static int edif_figuregroup();

static List technology200_lst[] =
{
   "numberDefinition",        edif_numberdefinition,
   "figuregroup",             edif_figuregroup,
};

/* technology400_lst */
static int pcbGeometricAttributeDefault();

static List technology400_lst[] =
{
   "pcbGeometricAttributeDefault",  pcbGeometricAttributeDefault,
};

/* technology300_lst */
static List  technology300_lst[] =
{
   "physicalScaling",         physicalScaling,
   "figureGroup",             edif_figuregroup,
};

/* schematicFigureMacro_lst */
static int pageTitleBlockAttributes();
static int pageTitleBlockAttributeDisplay();

static List schematicFigureMacro_lst[] =
{
   "schematicTemplateHeader",    fskip,
   "figure",                     figure200,
   "annotate",                   edif_annotate,
   "schematicComplexFigure",     schematicComplexFigure,
   "pageTitleBlockAttributes",   pageTitleBlockAttributes,
   "pageTitleBlockAttributeDisplay",   pageTitleBlockAttributeDisplay,
};

/* pageTitleBlockAttributes_lst */
static int companyname();
static int drawingsize();
static int drawingDescription();
static int pageIdentification();
static int pageTitle();
static int revision();

static List pageTitleBlockAttributes_lst[] =
{
   "companyName",             companyname,
   "drawingSize",             drawingsize,
   "drawingDescription",      drawingDescription,
   "pageIdentification",      pageIdentification,
   "pageTitle",               pageTitle,
   "revision",                revision,
};

/* pageTitleBlockAttributeDisplay_lst */
static int companynamedisplay();
static int drawingsizedisplay();
static int drawingDescriptiondisplay();
static int pageIdentificationDisplay();
static int pageTitleDisplay();
static int revisionDisplay();

static List pageTitleBlockAttributeDisplay_lst[] =
{
   "companyNameDisplay",         companynamedisplay,
   "drawingSizeDisplay",         drawingsizedisplay,
   "drawingDescriptionDisplay",  drawingDescriptiondisplay,
   "pageIdentificationDisplay",  pageIdentificationDisplay,
   "pageTitleDisplay",           pageTitleDisplay,
   "revisionDisplay",            revisionDisplay,
};

/* addDisplay_lst */
static int addDisplay();

static List addDisplay_lst[] =
{
   "addDisplay",              addDisplay,
};

/* schematicPortMacro_lst */
static int schematicSymbolPortTemplateRef();
static int portAttributeDisplay();

static List schematicPortMacro_lst[] =
{
   "schematicSymbolPortTemplateRef",   schematicSymbolPortTemplateRef,
   "transform",                  edif_transform,
   "portAttributeDisplay",       portAttributeDisplay,
};    

/* portAttributeDisplay_lst */
static int portNameDisplay();
static int designatorDisplay();
static int portDesignatorDisplay300();

static List portAttributeDisplay_lst[] =
{
   "portNameDisplay",            portNameDisplay,
   "designatorDisplay",          portDesignatorDisplay300,
};

/* numberdefinition_lst */
static int edif_scale();

static List numberdefinition_lst[] =
{
   "scale",                   edif_scale,
};

/* scale_lst */
static int edif_unit();

static List scale_lst[] =
{
   "unit",                    edif_unit,
};

/* page200_lst */
static int net200();
static int netbundle200();
static int page_portimplementation();
static int pagesize();

static List page200_lst[] =
{
   "pageSize",                pagesize,
   "instance",                instance200,
   "net",                     net200,
   "netbundle",               netbundle200,
   "commentGraphics",         commentgraphics,
   "portImplementation",      page_portimplementation,
};

/* page300_lst */
static int pageHeader();
static int pageTitleBlock();
static int schematicInstanceImplementation();
static int schematicMasterPortImplementation();
static int schematicGlobalPortImplementation();
static int schematicOffPageConnectorImplementation();
static int schematicOnPageConnectorImplementation();
static int schematicOnPageConnectorImplementation();
static int schematicNet();
static int schematicBus();

static List page300_lst[] =
{
   "pageSize",                pagesize,
   "instance",                instance200,
   "net",                     net200,
   "pageCommentGraphics",     commentgraphics,
   "portImplementation",      page_portimplementation,
   "schematicInstanceImplementation",     schematicInstanceImplementation, 
   "schematicMasterPortImplementation",   schematicMasterPortImplementation,  
   "schematicGlobalPortImplementation",   schematicGlobalPortImplementation,  
   "schematicOffPageConnectorImplementation",   schematicOffPageConnectorImplementation,
   "schematicOnPageConnectorImplementation",    schematicOnPageConnectorImplementation,
   "schematicNet",            schematicNet,
   "schematicBus",            schematicBus,
   "pageTitleBlock",          pageTitleBlock,
   "pageHeader",              pageHeader,
};

/* pageheader_lst */
static int pageBorder();

static List pageheader_lst[] =
{
   "pageBorder",              pageBorder,
};

/* cluster_lst */
static int pcbMcmView();
static int schematicSymbol();
static int schematicView();

static List cluster_lst[] =
{
   "interface",               fskip,
   "clusterHeader",           fskip,
   "pcbMcmView",              pcbMcmView,
   "schematicSymbol",         schematicSymbol,
   "schematicView",           schematicView,
};

/* pcbMcmView_lst */
static int pcbMcmImplementation();

static List pcbMcmView_lst[] =
{
   "pcbMcmViewHeader",        fskip,
   "logicalConnectivity",     fskip,
   "pcbMcmImplementation",    pcbMcmImplementation,
};

/* schematicsymbol_lst */
static int schematicSymbolHeader();
static int schematicSymbolPortImplementation();
static int instanceNameDisplay();

static List schematicsymbol_lst[] =
{
   "schematicSymbolHeader",   schematicSymbolHeader,  // needed to get the units
   "figure",                  figure200,
   "annotate",                edif_annotate,
   "schematicSymbolPortImplementation",   schematicSymbolPortImplementation,
   "instanceNameDisplay",     instanceNameDisplay,
   "designatorDisplay",       designatorDisplay,
   "propertyDisplay",         edif_propertydisplay,
};

/* pcbMcmImplementation_lst */
static int pcbAssembledBoard();

static List pcbMcmImplementation_lst[] =
{
   "pcbAssembledBoard",       pcbAssembledBoard,
};

/* schematicview_lst */
static int schematicImplementation();
static int logicalConnectivity();

static List schematicview_lst[] =
{
   "schematicViewHeader",     fskip,
   "logicalConnectivity",     logicalConnectivity,
   "schematicImplementation", schematicImplementation,
};

/* logicalinstance300_lst */
static int clusterref();
static int nameInformation();
static int instancePortAttributes();

static List logicalinstance300_lst[] =
{
   "clusterRef",              clusterref,
   "nameInformation",         nameInformation,
   "property",                edif_property,
   "designator",              edif_designator,
   "instancePortAttributes",  instancePortAttributes, //edif_portinstance,
};

/* instancePortAttributes_lst */
static int portDesignator300();

static List instancePortAttributes_lst[] =
{
   "designator",              portDesignator300,
};

/* schematicImplementation_lst */
static int page300();

static List schematicImplementation_lst[] =
{
   "page",                    page300,
};

/* logicalConnectivity_lst */
static int logicalConnectivity_instance300();

static List logicalConnectivity_lst[] =
{
   "instance",                logicalConnectivity_instance300,
};

/* pcbAssembledBoard_lst */
static int pcbBareBoard();
static int pcbDrawing();

static List pcbAssembledBoard_lst[] =
{
   "pcbBareBoard",         pcbBareBoard,
   "pcbDrawing",           pcbDrawing,
};

/* pcbBareBoard_lst */
static int pcbBareBoardDefinitionRef();

static List pcbBareBoard_lst[] =
{
   "pcbBareBoardDefinitionRef",        pcbBareBoardDefinitionRef,
};

/* pcbStackedLayerGroup_lst */
static int Stackup_pcbPhysicalLayerRef();

static List pcbStackedLayerGroup_lst[] =
{
   "pcbPhysicalLayerRef",        Stackup_pcbPhysicalLayerRef,
};

/* pcbBareBoardLayers_lst */
static int pcbPhysicalLayer();

static List  pcbBareBoardLayers_lst[] =
{
   "pcbPhysicalLayer",        pcbPhysicalLayer,
};

/* pcbDrawing_lst */
static int pcbDrawingAnnotation();

static List pcbDrawing_lst[] =
{
   "pcbDrawingHeader",        fskip,
   "pcbDrawingAnnotation",    pcbDrawingAnnotation,
};

/* pcbDrawingAnnotation_lst */
static int pcbdrawingfigure();
static int pcbdrawingtext();

static List pcbDrawingAnnotation_lst[] =
{
   "pcbDrawingFigure",        pcbdrawingfigure,
   "pcbDrawingText",          pcbdrawingtext,
};

/* pcbPhysicalLayerHeader_lst */
static List pcbPhysicalLayerHeader_lst[] =
{
   "nameInformation",         nameInformation,
};

/* pcbPhysicalNetHeader_lst */
static List pcbPhysicalNetHeader_lst[] =
{
   "nameInformation",         nameInformation,
};

/* nameInformation_lst */
static int primaryName();

static List nameInformation_lst[] =
{
   "primaryName",       primaryName,
};

/* footprint_lst */
static int pcbLayoutText();
static int pcbLayoutFeature();
static int pcbToeprintInstance();
static int pcbPhysicalNet();

static List footprint_lst[] =
{
   "pcbSubLayoutDefinitionHeader",  fskip,
   "pcbBareBoardOutline",           fskip,
   "pcbLayoutFeature",              pcbLayoutFeature,
   "pcbToeprintInstance",           pcbToeprintInstance,
   "pcbPhysicalNet",                pcbPhysicalNet,
};

/* padstack_lst */
static List padstack_lst[] =
{
   "pcbSubLayoutDefinitionHeader",  fskip,
   "pcbPhysicalNet",                pcbPhysicalNet,
};

/* toeprint_lst */
static int pcbPadstackInstance();

static List toeprint_lst[] =
{
   "pcbSubLayoutDefinitionHeader",  fskip,
   "pcbPhysicalNet",                fskip,
   "pcbPadstackInstance",           pcbPadstackInstance,
};

/* pcbLayoutFeature_lst */
static int pcbNonSpecificLayoutShape();
static int pcbTrace();  
static int pcbPad(); 

static List pcbLayoutFeature_lst[] =
{
   "pcbNonSpecificLayoutShape",     pcbNonSpecificLayoutShape,
   "pcbPad",                        pcbPad,
   "pcbTrace",                      pcbTrace,
};

/* pcbInstanceNets_lst */
static   int   pcbSubLayoutInstancePhysicalNetRef();

static List  pcbInstanceNets_lst[] =
{
   "pcbSubLayoutInstancePhysicalNetRef",  pcbSubLayoutInstancePhysicalNetRef,
};

/* pcbSubLayoutInstancePhysicalNetRef_lst */
static int pcbSubLayoutInstanceRef();

static List pcbSubLayoutInstancePhysicalNetRef_lst[] =
{
   "pcbSubLayoutInstanceRef",       pcbSubLayoutInstanceRef,
};

/* pcbLayoutText_lst */
static int pcbFabricateTextSet();

static List pcbLayoutText_lst[] =
{
   "pcbFabricateTextSet",           pcbFabricateTextSet,
};

/* pcbNonSpecificLayoutShape_lst */ 
static int pcbFabricateFigure();

static List pcbNonSpecificLayoutShape_lst[] =
{
   "pcbFabricateFigure",            pcbFabricateFigure,
};

/* pcbFabricateTextSet_lst */
static int  pcbFabricateText();

static List  pcbFabricateTextSet_lst[] =
{
   "pcbFabricateText",           pcbFabricateText,
};

/* pcbbareboarddefinition_lst */
static int pcbBareBoardOutline();
static int pcbBareBoardPhysicalNet();
static int pcbFootprintInstance();

static List pcbbareboarddefinition_lst[] =
{
   "pcbSubLayoutDefinitionHeader",  fskip,
   "pcbBareBoardOutline",           pcbBareBoardOutline,
   "pcbBareBoardPhysicalNet",       pcbBareBoardPhysicalNet,
   "pcbFootprintInstance",          pcbFootprintInstance,
   "pcbPadstackInstance",           pcbPadstackInstance,
};

/* pcbbareboardtechnology_lst */
static int pcbStackedLayerGroup();
static int pcbBareBoardLayers();

static List pcbbareboardtechnology_lst[] =
{
   "pcbBareBoardLayers",            pcbBareBoardLayers,
   "pcbStackedLayerGroup",          pcbStackedLayerGroup,
};

/* pcbBareBoardOutline_lst */
static List pcbBareBoardOutline_lst[] =
{
   "pcbFabricateFigure",            pcbFabricateFigure,
};

/* pcbPlane_lst */
static List pcbPlane_lst[] =
{
   "pcbFabricateFigure",            pcbFabricateFigure,
};

/* pcbTrace_lst */
static int pcbFabricateFigure();

static List pcbTrace_lst[] =
{
   "pcbPadIntendedUse",             fskip,
   "pcbPadRoutingTarget",           fskip,
   "pcbFabricateFigure",            pcbFabricateFigure,
   "pcbClearanceGeometry",          fskip,
};

/* pcbPad_lst */
static List pcbPad_lst[] =
{
   "pcbFabricateFigure",            pcbFabricateFigure,
};

/* pcbPhysicalNet_lst */
static int pcbInstanceNets();

static List pcbPhysicalNet_lst[] =
{
   "pcbInstanceNets",            pcbInstanceNets,
   "pcbLayoutFeature",           pcbLayoutFeature,
};

/* pcbPadStackInstance_lst */
static int pcbPadstackDefinitionRef();
static int fixedScaleTransform();   
static int pcbPadstackInstanceRoutingViaPlace();

static List pcbPadStackInstance_lst[] =
{
   "pcbPadstackDefinitionRef",      pcbPadstackDefinitionRef,
   "fixedScaleTransform",           fixedScaleTransform,
   "pcbPadstackInstanceRoutingViaPlace", pcbPadstackInstanceRoutingViaPlace,  
};

/* pcbToeprintInstance_lst */
static int pcbToeprintDefinitionRef();

static List pcbToeprintInstance_lst[] =
{
   "pcbToeprintDefinitionRef",      pcbToeprintDefinitionRef,
   "fixedScaleTransform",           fixedScaleTransform,
};

/* pcbBareBoardPhysicalNet_lst */ 
static int pcbPlane();
static int pcbPhysicalNetHeader();

static List pcbBareBoardPhysicalNet_lst[] =
{
   "pcbPhysicalNetHeader",          pcbPhysicalNetHeader,
   "pcbLayoutFeature",              pcbLayoutFeature,
   "pcbPlane",                      pcbPlane,
   "pcbPhysicalNet",                pcbPhysicalNet,
   "pcbLayoutText",                 pcbLayoutText,
};

/* pcbFootprintInstance_lst */
static int pcbFootprintDefinitionRef();

static List pcbFootprintInstance_lst[] =
{
   "pcbFootprintDefinitionRef",     pcbFootprintDefinitionRef,
   "fixedScaleTransform",           fixedScaleTransform,
};

/* fixedScaleTransform_lst */
static int edif_rotation();

static List fixedScaleTransform_lst[] =
{
   "origin",               edif_origin,
   "mirroring",            edif_mirroring,
   "rotation",             edif_rotation,
};

/* transform_lst */
static int edif_scalex();
static int edif_scaley();

static List transform_lst[] =
{
   "origin",               edif_origin,
   "orientation",          edif_orientation,
   "scaleX",               edif_scalex,
   "scaleY",               edif_scaley,
   "rotation",             edif_rotation,
};

/* figuregroupoverride_lst */
static   int   edif_textwidth();

static List  figuregroupoverride_lst[] =
{
   "textHeight",           edif_textheight,
   "textWidth",            edif_textwidth,
   "visible",              edif_visible,
   "color",                edif_color,
   "pathWidth",            edif_width,
   "displayAttributes",    edif_displayattributes,
   "property",             fskip,
};

/* mirroring_lst */
static int mirror_noMirroring(), mirror_Mirroring();  

static List mirroring_lst[] =
{
   "noMirroring",          mirror_noMirroring,
   "Mirroring",            mirror_Mirroring,
   "MirrorX",              mirror_Mirroring,       // ????
};

/* pcbFabricateFigure_lst */
static int pcbPhysicalFigure(), pcbComplexPhysicalFigure();
static int pcbContributesToFabrication(); 

static List pcbFabricateFigure_lst[] =
{
   "pcbContributesToFabrication",            pcbContributesToFabrication,
   "pcbPhysicalFigure",                      pcbPhysicalFigure,               
   "pcbComplexPhysicalFigure",               pcbComplexPhysicalFigure,              
};

/* pcbFabricateText_lst */
static int pcbPhysicalTextSet();

static List pcbFabricateText_lst[] =
{
   "pcbContributesToFabrication",            pcbContributesToFabrication,
   "pcbPhysicalTextSet",                     pcbPhysicalTextSet,              
};

/* pcbPhysicalText_lst */
static int pcbPhysicalTextImplementation();

static List pcbPhysicalText_lst[] =
{
   "pcbPhysicalTextImplementation",          pcbPhysicalTextImplementation,
};

/* pcbPhysicalTextSet_lst */
static int pcbPhysicalText();

static List pcbPhysicalTextSet_lst[] =
{
   "pcbPhysicalText",                        pcbPhysicalText,
};

/* pcbPhysicalTextImplementation_lst */
static int pcbTextAppearance();

static List pcbPhysicalTextImplementation_lst[] =
{
   "pcbTextAppearance",                      pcbTextAppearance,
};

/* pcbTextAppearance_lst */
static int pcbFigureAttributeOverride();

static List pcbTextAppearance_lst[] =
{
   "pcbFigureAttributeOverride",    pcbFigureAttributeOverride,
   "pcbTextAttributeOverride",      fskip,
   "stringExtent",                  fskip,
   "unscaledTransform",             edif_transform,
};

/* pcbPhysicalFigureMacro_lst */
static List pcbPhysicalFigureMacro_lst[] =
{
   "pcbTemplateHeader",                      fskip,
   "pcbPhysicalFigure",                      pcbPhysicalFigure,               
};

/* pcbPhysicalFigure_lst */
static int geometryElementSet();

static List pcbPhysicalFigure_lst[] =
{
   "pcbFigureAttributeOverride",    pcbFigureAttributeOverride,
   "geometryElementSet",            geometryElementSet,
};

/* pcbFigureAttributeOverride_lst */
static List pcbFigureAttributeOverride_lst[] =
{
   "color",                         fskip,
   "pathWidth",                     edif_width,
};

/* pcbDrawingTextDrawingAttributeOverride_lst */
static List pcbDrawingTextDrawingAttributeOverride_lst[] =
{
   "textHeight",                    edif_textheight,
};

/* pcbDrawingFigure_lst */
static List pcbDrawingFigure_lst[] =
{
   "pcbDrawingFigureDrawingAttributeOverride",     
                                    pcbFigureAttributeOverride,
   "geometryElementSet",            geometryElementSet,
};

/* pcbPhysicalLayer_lst */
static int pcbConductingLayer();
static int pcbPhysicalLayerHeader();
static int pcbNonConductingLayer();

static List pcbPhysicalLayer_lst[] =
{
   "pcbPhysicalLayerHeader",        pcbPhysicalLayerHeader,
   "pcbNonConductingLayer",         pcbNonConductingLayer,
   "pcbConductingLayer",            pcbConductingLayer,
   "thickness",                     fskip,
};

/* pcbNonConductingLayer_lst */
static int pcbNonConductingLayerSilkscreen();
static int pcbNonConductingLayerSolderMask();

static List pcbNonConductingLayer_lst[] =
{
   "pcbNonConductingLayerSilkscreen",  pcbNonConductingLayerSilkscreen, 
   "pcbNonConductingLayerSolderMask",  pcbNonConductingLayerSolderMask,
};

/* pcbDrawingText_lst */
static int pcbdrawingtextdisplay();

static List pcbDrawingText_lst[] =
{
   "pcbDrawingTextDisplay",         pcbdrawingtextdisplay,
};

/* pcbDrawingTextDisplay_lst */
static int pcbDrawingTextDrawingAttributeOverride();

static List pcbDrawingTextDisplay_lst[] =
{
   "pcbDrawingTextDrawingAttributeOverride",    
                           pcbDrawingTextDrawingAttributeOverride,
   "unscaledTransform",    edif_transform,
};

/* pcbComplexPhysicalFigure_lst */
static int pcbPhysicalFigureMacroRef();

static List pcbComplexPhysicalFigure_lst[] =
{
   "pcbPhysicalFigureMacroRef",  pcbPhysicalFigureMacroRef,
   "fixedScaleTransform",        fixedScaleTransform,
};

/* schematicComplexFigure_lst */
static int schematicFigureMacroRef();

static List schematicComplexFigure_lst[] =
{
   "schematicFigureMacroRef",    schematicFigureMacroRef,
   "transform",                  edif_transform,
};

/* schematicInstanceImplementation_lst */
static int schematicSymbolRef();
static int instanceRef();
static int instance_propertydisplay();
static int instance_nameinformation();
static int pageTitleBlockTemplateRef();
static int pageBorderTemplateRef();
static int schematicOffPageConnectorTemplateRef();
static int schematicOnPageConnectorTemplateRef();
static int schematicMasterPortTemplateRef();
static int schematicGlobalPortTemplateRef();
static int propertyDisplayOverride();
static int propertyOverride();
static int instancePortAttributeDisplay();

static List schematicInstanceImplementation_lst[] =
{
   "instanceRef",                         instanceRef,
   "schematicSymbolRef",                  schematicSymbolRef,
   "transform",                           instance_transform,
   "instancePropertyDisplay",             instance_propertydisplay,
   "pageTitleBlockTemplateRef",           pageTitleBlockTemplateRef,
   "pageBorderTemplateRef",               pageBorderTemplateRef,
   "schematicOffPageConnectorTemplateRef",schematicOffPageConnectorTemplateRef,
   "schematicOnPageConnectorTemplateRef", schematicOnPageConnectorTemplateRef,
   "schematicMasterPortTemplateRef",      schematicMasterPortTemplateRef,
   "schematicGlobalPortTemplateRef",      schematicGlobalPortTemplateRef,
   "nameInformation",                     instance_nameinformation,
   "pageTitleBlockAttributes",            pageTitleBlockAttributes,
   "propertyDisplayOverride",             propertyDisplayOverride,
   "propertyOverride",                    propertyOverride,
   "instancePortAttributeDisplay",        instancePortAttributeDisplay,
};

/* instancePortAttributeDisplay_lst */
static List instancePortAttributeDisplay_lst[] =
{
   "portAttributeDisplay", portAttributeDisplay,
};

/* propertyDisplayOverride_lst */
static int addDisplay();
static int replaceDisplay();
static int removeDisplay();

static List propertyDisplayOverride_lst[] =
{
   "addDisplay",        addDisplay,
   "replaceDisplay",    replaceDisplay,
   "removeDisplay",     removeDisplay,
   "display",           propdisplay
};

/* schematicNet_lst */
static int schematicNetDetails();

static List schematicNet_lst[] =
{
   "schematicNetDetails",  schematicNetDetails,
};

/* schematicBus_lst */
static int schematicBusDetails();

static List schematicBus_lst[] =
{
   "schematicBusDetails",  schematicBusDetails,
};

/* schematicNetGraphics_lst */
static int schematicNetGraphics();
static int schematicSubNetSet();

static List  schematicNetGraphics_lst[] =
{
   "schematicNetGraphics", schematicNetGraphics,
   "schematicSubNetSet",   schematicSubNetSet,
};

/* schematicBusGraphics_lst */
static int schematicBusGraphics();

static List schematicBusGraphics_lst[] =
{
   "schematicBusGraphics", schematicBusGraphics,
};

/* schematicSubNetSet_lst */
static   int   schematicSubNet();

static List  schematicSubNetSet_lst[] =
{
   "schematicSubNet",      schematicSubNet,
};

/* pcbContributesToFabrication_lst */
static int pcbSimpleFabricateFigureCollectorRef();

static List  pcbContributesToFabrication_lst[] =
{
   "pcbSimpleFabricateFigureCollectorRef",            pcbSimpleFabricateFigureCollectorRef,
};

/* geometryElementSet_lst */
static int edif_polygon();
static int edif_shape();
static int edif_openshape();
static int edif_unfilledRectangle(), edif_rectangle();
static int edif_path();
static int edif_circleByCenter();
static int edif_circle200();
static int edif_dot200();

static List  geometryElementSet_lst[] =
{
   "figureGroupOverride",  edif_figuregroupoverride,
   "shape",                edif_shape,
   "openShape",            edif_openshape,
   "polygon",              edif_polygon,
   "unfilledRectangle",    edif_unfilledRectangle,
   "circleByCenter",       edif_circleByCenter,
   "rectangle",            edif_rectangle,
   "path",                 edif_path,
   "circle",               edif_circle200,
   "dot",                  edif_dot200,
};

/* design_lst */
static int cellRef();
static int designHierarchy();

static List design_lst[] =
{
   "cellRef",              cellRef,
   "designHeader",         fskip,
   "designHierarchy",      designHierarchy,
};

/* cellRef_lst */
static int libraryRef();

static List cellRef_lst[] =
{
   "libraryRef",           libraryRef,
};

/* designHierarchy_lst */
static int occurrenceHierarchyAnnotate();

static List designHierarchy_lst[] =
{
   "occurrenceHierarchyAnnotate",   occurrenceHierarchyAnnotate,
};

/* occurrenceHierarchyAnnotate_lst */
static int leafOccurrenceAnnotate();
static int occurrenceAnnotate();

static List occurrenceHierarchyAnnotate_lst[] =
{
   "leafOccurrenceAnnotate",        leafOccurrenceAnnotate,
   "occurrenceAnnotate",            occurrenceAnnotate,
};

/* occurrenceAnnotate_lst */
static int instancePropertyOverride();

static List occurrenceAnnotate_lst[] =
{
   "leafOccurrenceAnnotate",        leafOccurrenceAnnotate,
   "occurrenceAnnotate",            occurrenceAnnotate,
   "instancePropertyOverride",      instancePropertyOverride,
   "property",                      edif_property,
};

/* leafOccurrenceAnnotate_lst */
static List leafOccurrenceAnnotate_lst[] =
{
   "instancePropertyOverride",      instancePropertyOverride,
   "property",                      edif_property,
};

/* viewref200_lst */
static List viewref200_lst[] =
{
   "cellRef",              cellRef,
};

/* clusterref_lst */
static List  clusterref_lst[] =
{
   "cellRef",              cellRef,
};

/* pt_lst */
static int edif_pt();
static int edif_arc();

static List pt_lst[] =
{
   "pt",                   edif_pt,
};

/* pointList_lst */
static int edif_pointList();

static List pointList_lst[] =
{
   "pointList",            edif_pointList,
};

/* curveList_lst */
static int edif_curveList();

static List curveList_lst[] =
{
   "curve",                edif_curveList,
};

/* arc_lst */
static   int   edif_numberpoint();

static List  arc_lst[] =
{
   "pt",                   edif_pt,
   "numberPoint",          edif_numberpoint,
};

/* shapept_lst */
static List  shapept_lst[] =
{
   "pt",                   edif_pt,
   "arc",                  edif_arc,
};

/* layerstacklayer_lst */
static   int   edif_id();

static List  layerstacklayer_lst[] =
{
   "name",                 edif_name,  
   "id",                   edif_id,
};

/* drill_lst */
static   int   drillsize();
static   int   drillplated();

static List  drill_lst[] =
{
   "name",                 fskip,   // always skip the name, otherwise is messes up the padname
   "drillSize",            drillsize,
   "drillLetter",          fskip,
   "drillSymbol",          fskip,
   "plated",               drillplated,
};

/* package_lst */
static List  package_lst[] =
{
   "name",                 edif_name,
   "packDesc",             fskip,
};

/* location_lst */
static   int   edif_position();
static   int   edif_mirrored();

static List  location_lst[] =
{
   "position",             edif_position,
   "fixed",                fskip,
   "orientation",          edif_orientation,
   "side",                 fskip,
   "mirrored",             edif_mirrored,
   "zone",                 fskip,
};

// signal sections
static int                 edif_powersignal();
static int                 edif_height();

