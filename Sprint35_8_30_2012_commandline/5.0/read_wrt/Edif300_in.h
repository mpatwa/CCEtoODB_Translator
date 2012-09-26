// $Header: /CAMCAD/4.6/read_wrt/Edif300_in.h 62    4/12/07 3:37p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__Edif300_In_h__)
#define __Edif300_In_h__

#pragma once


/* Define Section *********************************************************/
#define MAX_LINE                 4000        // Max line length; Case 1674 increased buf to 4000 from 2000, sample data had 3107 char line
#define MAX_TOKEN                MAX_LINE
#define MAX_LAYERS               255         // Max number of layers
#define MAX_SHAPE                1000
#define MAX_EDIFUNITS            20
#define SCH_HOTSPOT_RULE         "________RULE"
#define SCH_HOTSPOT_PIN          "________PIN"

// These are the list sizes
#define SIZ_PARENTHESIS_LST                              (sizeof(parenthesis_lst) / sizeof(List))
#define SIZ_STARTEDIT_LST                                (sizeof(startEdit_lst) / sizeof(List))
#define SIZ_EDIF300_LST                                  (sizeof(edif300_lst) / sizeof(List))
#define SIZ_EDIFHEADER_LST                               (sizeof(edifHeader_lst) / sizeof(List))
#define SIZ_STATUS_LST												(sizeof(status_lst) / sizeof(List))
#define SIZ_WRITTEN_LST												(sizeof(written_lst) / sizeof(List))
#define SIZ_DESIGN_LST                                   (sizeof(design_lst) / sizeof(List))
#define SIZ_LIBRARY_LST                                  (sizeof(library_lst) / sizeof(List))
#define SIZ_KEYWORDMAP_LST                               (sizeof(keywordMap_lst) / sizeof(List))
#define SIZ_UNITDEFINITIONS_LST                          (sizeof(unitDefinitions_lst) / sizeof(List))
#define SIZ_UNIT_LST                                     (sizeof(unit_lst) / sizeof(List))
#define SIZ_FONTDEFINITIONS_LST                          (sizeof(fontDefinitions_lst) / sizeof(List))
#define SIZ_FONTS_LST                                    (sizeof(fonts_lst) / sizeof(List))
#define SIZ_SETUNIT_LST                                  (sizeof(setUnit_lst) / sizeof(List))
#define SIZ_FONT_LST                                     (sizeof(font_lst) / sizeof(List))
#define SIZ_FONTPROPORTIONS_LST                          (sizeof(fontProportions_lst) / sizeof(List))
#define SIZ_PHYSICALDEFAULTS_LST                         (sizeof(physicalDefaults_lst) / sizeof(List))
#define SIZ_SCHEMATICREQUIREDEFAULTS_LST                 (sizeof(schematicRequireDefaults_lst) / sizeof(List))
#define SIZ_SCHEMATICMETRIC_LST                          (sizeof(schematicMetric_lst) / sizeof(List))
#define SIZ_GLOBALPORTDEFINITIONS_LST                    (sizeof(globalPortDefinitions_lst) / sizeof(List))
#define SIZ_GLOBALPORT_LST                               (sizeof(globalPort_lst) / sizeof(List))
#define SIZ_PORTLIST_LST                                 (sizeof(portList_lst) / sizeof(List))
#define SIZ_NAMEINFORMATION_LST                          (sizeof(nameInformation_lst) / sizeof(List))
#define SIZ_PRIMARYNAME_LST                              (sizeof(primaryName_lst) / sizeof(List))
#define SIZ_PROPERTY_LST                                 (sizeof(property_lst) / sizeof(List))
#define SIZ_PHYSICALSCALING_LST                          (sizeof(physicalScaling_lst) / sizeof(List))
#define SIZ_SCHEMATICUNITS_LST                           (sizeof(schematicUnits_lst) / sizeof(List))
#define SIZ_LIBRARYHEADER_LST                            (sizeof(libraryHeader_lst) / sizeof(List))
#define SIZ_TECHNOLOGY_LST                               (sizeof(technology_lst) / sizeof(List))
#define SIZ_FIGUREGROUP_LST                              (sizeof(figureGroup_lst) / sizeof(List))
#define SIZ_DISPLAYATTRIBUTES_LST                        (sizeof(displayAttributes_lst) / sizeof(List))
#define SIZ_JUSTIFICATION_LST                            (sizeof(justification_lst) / sizeof(List))
#define SIZ_GEOMETRYMACRO_LST                            (sizeof(geometryMacro_lst) / sizeof(List))
#define SIZ_GEOMETRYMACROHEADER_LST                      (sizeof(geometryMacroHeader_lst) / sizeof(List))
#define SIZ_GEOMETRYMACROUNITS_LST                       (sizeof(geometryMacroUnits_lst) / sizeof(List))
#define SIZ_PAGEBORDERTEMPLATE_LST                       (sizeof(pageBorderTemplate_lst) / sizeof(List))
#define SIZ_COMMENTGRAPHICS_LST                          (sizeof(commentGraphics_lst) / sizeof(List))
#define SIZ_PAGETITLEBLOCKTEMPLATE_LST                   (sizeof(pageTitleBlockTemplate_lst) / sizeof(List))
#define SIZ_SCHEMATICFIGUREMACRO_LST                     (sizeof(schematicFigureMacro_lst) / sizeof(List))
#define SIZ_SCHEMATICTEMPLATEHEADER_LST                  (sizeof(schematicTemplateHeader_lst) / sizeof(List))
#define SIZ_SCHEMATICGLOBALPORTTEMPLATE_LST              (sizeof(schematicGlobalPortTemplate_lst) / sizeof(List))
#define SIZ_GETNAMEDISPLAY_LST                           (sizeof(getNameDisplay_lst) / sizeof(List))
#define SIZ_SCHEMATICMASTERPORTTEMPLATE_LST              (sizeof(schematicMasterPortTemplate_lst) / sizeof(List))
#define SIZ_GETTEMPLATE_LST                              (sizeof(getTemplate_lst) / sizeof(List))
#define SIZ_SCHEMATICSYMBOLPORTTEMPLATE_LST              (sizeof(schematicSymbolPortTemplate_lst) / sizeof(List))
#define SIZ_CELL_LST                                     (sizeof(cell_lst) / sizeof(List))
#define SIZ_CLUSTER_LST                                  (sizeof(cluster_lst) / sizeof(List))
#define SIZ_INTERFACE_LST                                (sizeof(interface_lst) / sizeof(List))
#define SIZ_PORT_LST                                     (sizeof(port_lst) / sizeof(List))
#define SIZ_CLUSTERHEADER_LST                            (sizeof(clusterHeader_lst) / sizeof(List))
#define SIZ_CLUSTERCONFIGURATION_LST                     (sizeof(clusterConfiguration_lst) / sizeof(List))
#define SIZ_INSTANCECONFIGURATION_LST                    (sizeof(instanceConfiguration_lst) / sizeof(List))
#define SIZ_SCHEMATICSYMBOL_LST                          (sizeof(schematicSymbol_lst) / sizeof(List))
#define SIZ_SCHEMATICSYMBOLHEADER_LST                    (sizeof(schematicSymbolHeader_lst) / sizeof(List))
#define SIZ_SCHEMATICSYMBOLBORDER_LST                    (sizeof(schematicSymbolBorder_lst) / sizeof(List))
#define SIZ_FIGURE_LST                                   (sizeof(figure_lst) / sizeof(List))
#define SIZ_COMPLEXGEOMETRY_LST                          (sizeof(complexGeometry_lst) / sizeof(List))
#define SIZ_OPENSHAPE_LST                                (sizeof(openShape_lst) / sizeof(List))
#define SIZ_CURVE_LST                                    (sizeof(curve_lst) / sizeof(List))
#define SIZ_DISPLAY_LST                                  (sizeof(display_lst) / sizeof(List))
#define SIZ_TRANSFORM_LST                                (sizeof(transform_lst) / sizeof(List))
#define SIZ_GETDISPLAY_LST                               (sizeof(getDisplay_lst) / sizeof(List))
#define SIZ_DISPLAYNAMEOVERRIDE_LST                      (sizeof(displayNameOverride_lst) / sizeof(List))
#define SIZ_SCHEMATICCOMPLEXFIGURE_LST                   (sizeof(schematicComplexFigure_lst) / sizeof(List))
#define SIZ_SCHEMATICSYMBOLPORTIMPLEMENTATION_LST        (sizeof(schematicSymbolPortImplementation_lst) / sizeof(List))
#define SIZ_PORTATTRIBUTEDISPLAY_LST                     (sizeof(portAttributeDisplay_lst) / sizeof(List))
#define SIZ_SCHEMATICVIEW_LST                            (sizeof(schematicView_lst) / sizeof(List))
#define SIZ_SCHEMATICVIEWHEADER_LST                      (sizeof(schematicViewHeader_lst) / sizeof(List))
#define SIZ_LOGICALCONNECTIVITY_LST                      (sizeof(logicalConnectivity_lst) / sizeof(List))
#define SIZ_INSTANCE_LST                                 (sizeof(instance_lst) / sizeof(List))
#define SIZ_CLUSTERREF_LST                               (sizeof(clusterRef_lst) / sizeof(List))
#define SIZ_GETOBJECTREF_LST                             (sizeof(getObjectRef_lst) / sizeof(List))
#define SIZ_INSTANCEPORTATTRIBUTES_LST                   (sizeof(instancePortAttributes_lst) / sizeof(List))
#define SIZ_SIGNAL_LST                                   (sizeof(signal_lst) / sizeof(List))
#define SIZ_SIGNALJOINED_LST                             (sizeof(signalJoined_lst) / sizeof(List))
#define SIZ_PORTINSTANCEREF_LST                          (sizeof(portInstanceRef_lst) / sizeof(List))
#define SIZ_SIGNALGROUP_LST                              (sizeof(signalGroup_lst) / sizeof(List))
#define SIZ_SIGNALLIST_LST                               (sizeof(signalList_lst) / sizeof(List))
#define SIZ_SCHEMATICIMPLEMENTATION_LST                  (sizeof(schematicImplementation_lst) / sizeof(List))
#define SIZ_PAGE_LST                                     (sizeof(page_lst) / sizeof(List))
#define SIZ_PAGEHEADER_LST                               (sizeof(pageHeader_lst) / sizeof(List))
#define SIZ_PAGEBORDER_LST                               (sizeof(pageBorder_lst) / sizeof(List))
#define SIZ_PAGECOMMENTGRAPHICS_LST                      (sizeof(pageCommentGraphics_lst) / sizeof(List))
#define SIZ_PAGETITLEBLOCK_LST                           (sizeof(pageTitleBlock_lst) / sizeof(List))
#define SIZ_PAGETITLEBLOCKATTRIBUTEDISPLAY_LST           (sizeof(pageTitleBlockAttributeDisplay_lst) / sizeof(List))
#define SIZ_PAGETITLEBLOCKATTRIBUTES_LST                 (sizeof(pageTitleBlockAttributes_lst) / sizeof(List))
#define SIZ_SCHEMATICGLOBALPORTIMPLEMENTATION_LST        (sizeof(schematicGlobalPortImplementation_lst) / sizeof(List))
#define SIZ_GLOBALPORTPROPERTYDISPLAY_LST                (sizeof(globalPortPropertyDisplay_lst) / sizeof(List))
#define SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST          (sizeof(schematicInstanceImplementation_lst) / sizeof(List))
#define SIZ_INSTANCEPORTATTRIBUTEDISPLAY_LST             (sizeof(instancePortAttributeDisplay_lst) / sizeof(List))
#define SIZ_SCHEMATICMASTERPORTIMPLEMENTATION_LST        (sizeof(schematicMasterPortImplementation_lst) / sizeof(List))
#define SIZ_SCHEMATICNET_LST                             (sizeof(schematicNet_lst) / sizeof(List))
#define SIZ_SCHEMATICINTERCONNECTHEADER_LST              (sizeof(schematicInterconnectHeader_lst) / sizeof(List))
#define SIZ_SCHEMATICINTERCONNECTTERMINATORIMPLEMENTATION_LST  (sizeof(schematicInterconnectTerminatorImplementation_lst) / sizeof(List))
#define SIZ_SCHEMATICJUNCTIONIMPLEMENTATION_LST          (sizeof(schematicJunctionImplementation_lst) / sizeof(List))
#define SIZ_SCHEMATICNETJOINED_LST                       (sizeof(schematicNetJoined_lst) / sizeof(List))
#define SIZ_RIPPERHOTSPOTREF_LST									(sizeof(ripperHotSpotRef_lst) / sizeof(List))
#define SIZ_SCHEMATICINTERCONNECTATTRIBUTEDISPLAY_LST    (sizeof(schematicInterconnectAttributeDisplay_lst) / sizeof(List))
#define SIZ_INTERCONNECTATTACHEDTEXT_LST                 (sizeof(interconnectAttachedText_lst) / sizeof(List))
#define SIZ_SCHEMATICNETDETAILS_LST                      (sizeof(schematicNetDetails_lst) / sizeof(List))
#define SIZ_SCHEMATICNETGRAPHICS_LST                     (sizeof(schematicNetGraphics_lst) / sizeof(List))
#define SIZ_SCHEMATICSUBNETSET_LST                       (sizeof(schematicSubNetSet_lst) / sizeof(List))
#define SIZ_SCHEMATICSUBNET_LST                          (sizeof(schematicSubNet_lst) / sizeof(List))
#define SIZ_SCHEMATICSUBINTERCONNECTHEADER_LST           (sizeof(schematicSubInterconnectHeader_lst) / sizeof(List))
#define SIZ_SCHEMATICOFFPAGECONNECTORIMPLEMENTATION_LST  (sizeof(schematicOffPageConnectorImplementation_lst) / sizeof(List))
#define SIZ_SCHEMATICONPAGECONNECTORIMPLEMENTATION_LST   (sizeof(schematicOnPageConnectorImplementation_lst) / sizeof(List))
#define SIZ_SCHEMATICRIPPERIMPLEMENTATION_LST            (sizeof(schematicRipperImplementation_lst) / sizeof(List))
#define SIZ_DESIGNHEADER_LST                             (sizeof(designHeader_lst) / sizeof(List))
#define SIZ_DESIGNUNITS_LST                              (sizeof(designUnits_lst) / sizeof(List))
#define SIZ_DESIGNHIERARCHY_LST                          (sizeof(designHierarchy_lst) / sizeof(List))
#define SIZ_DESIGNHIERARCHYHEADER_LST                    (sizeof(designHierarchyHeader_lst) / sizeof(List))
#define SIZ_OCCURRENCEHIERARCHYANNOTATE_LST              (sizeof(occurrenceHierarchyAnnotate_lst) / sizeof(List))
#define SIZ_LEAFOCCURRENCEANNOTATE_LST                   (sizeof(leafOccurrenceAnnotate_lst) / sizeof(List))
#define SIZ_PORTANNOTATE_LST                             (sizeof(portAnnotate_lst) / sizeof(List))
#define SIZ_OCCURRENCEANNOTATE_LST                       (sizeof(occurrenceAnnotate_lst) / sizeof(List))
#define SIZ_PAGEANNOTATE_LST                             (sizeof(pageAnnotate_lst) / sizeof(List))
#define SIZ_INTERCONNECTANNOTATE_LST                     (sizeof(interconnectAnnotate_lst) / sizeof(List))
#define SIZ_SIGNALANNOTATE_LST                           (sizeof(signalAnnotate_lst) / sizeof(List))
#define SIZ_SCHEMATICBUS_LST                             (sizeof(schematicBus_lst) / sizeof(List))
#define SIZ_SCHEMATICBUSJOINED_LST                       (sizeof(schematicBusJoined_lst) / sizeof(List))
#define SIZ_SCHEMATICBUSDETAILS_LST                      (sizeof(schematicBusDetails_lst) / sizeof(List))
#define SIZ_SCHEMATICBUSGRAPHICS_LST                     (sizeof(schematicBusGraphics_lst) / sizeof(List))
#define SIZ_SCHEMATICSUBBUSSET_LST                       (sizeof(schematicSubBusSet_lst) / sizeof(List))
#define SIZ_SCHEMATICSUBBUS_LST                          (sizeof(schematicSubBus_lst) / sizeof(List))


  /* Lists Section *********************************************************/
typedef  struct
{
   char  *token;
   int      (*function)();
} List;

/* parenthesis_lst section */
static int skipCommand();

static List parenthesis_lst[] =
{
   "(",                       skipCommand,   // don't do anything
   ")",                       skipCommand,   // don't do anything
};

/* startEdit_lst section */
static int edif300();

static List startEdit_lst[]=
{
   "edif",                    edif300,
};

/* edif300_lst section */
static int edifHeader();
static int design();
static int external();
static int library();

static List edif300_lst[] =
{
   "edifVersion",             skipCommand,   // don't care
   "edifHeader",              edifHeader,
   "external",                external,
   "library",                 library,
   "design",                  design,
};

/* edifHeader_lst section */
static int edifLevel();
static int keywordMap();
static int unitDefinitions();
static int fontDefinitions();
static int physicalDefaults();
static int globalPortDefinitions();
static int physicalScaling();
static int status();

static List edifHeader_lst[] =
{
   "edifLevel",               edifLevel,
   "keywordMap",              keywordMap,
   "unitDefinitions",         unitDefinitions,
   "fontDefinitions",         fontDefinitions,
   "physicalDefaults",        physicalDefaults,
   "globalPortDefinitions",   globalPortDefinitions,
   "nameInformation",         skipCommand,
   "physicalScaling",         physicalScaling,
   "property",                skipCommand,      // not sure if needed or not
	"status",						status,
};

/* status_lst section */
static int written();

static List status_lst[] = 
{	
	"written",						written,
};

/* written_lst section */
static int program();

static List written_lst[] =
{
	"program",						program,
};

/* library_lst section */
static int libraryHeader();
static int geometryMacro();
static int pageBorderTemplate();
static int pageTitleBlockTemplate();
static int schematicFigureMacro();
static int schematicGlobalPortTemplate();
static int schematicInterconnectTerminatorTemplate();
static int schematicJunctionTemplate();
static int schematicMasterPortTemplate();
static int schematicOffPageConnectorTemplate();  
static int schematicOnPageConnectorTemplate();
static int schematicRipperTemplate();
static int schematicSymbolPortTemplate();
static int cell();

static List library_lst[] =
{
   "libraryHeader",                             libraryHeader,
   "geometryMacro",                             geometryMacro,
   "pageBorderTemplate",                        pageBorderTemplate,
   "pageTitleBlockTemplate",                    pageTitleBlockTemplate,
   "schematicFigureMacro",                      schematicFigureMacro,
   "schematicGlobalPortTemplate",               schematicGlobalPortTemplate,
   "schematicInterconnectTerminatorTemplate",   schematicInterconnectTerminatorTemplate,  
   "schematicJunctionTemplate",                 schematicJunctionTemplate, 
   "schematicMasterPortTemplate",               schematicMasterPortTemplate,
   "schematicOffPageConnectorTemplate",         schematicOffPageConnectorTemplate,
   "schematicOnPageConnectorTemplate",          schematicOnPageConnectorTemplate,
   "schematicRipperTemplate",                   schematicRipperTemplate,   
   "schematicSymbolBorderTemplate",             skipCommand,
   "schematicSymbolPortTemplate",               schematicSymbolPortTemplate,
   "cell",                                      cell,
};

/* design_lst section */
static int getObjectRef();
static int designHeader();
static int designHierarchy();

static List design_lst[] =
{
   "cellRef",                 getObjectRef,
   "designHeader",            skipCommand, // designHeader, // don't card
   "designHierarchy",         designHierarchy,
};

/* cellRef_lst section */
static int libraryRef();

static List getObjectRef_lst[] =
{
   "libraryRef",              libraryRef,
};

/* keywordMap_lst section */
static int notSupportKeywordLevel();

static List keywordMap_lst[] =
{
   "k0KeywordLevel",          skipCommand,
   "k1KeywordLevel",          notSupportKeywordLevel,
   "k2KeywordLevel",          notSupportKeywordLevel,
   "k3KeywordLevel",          notSupportKeywordLevel,
};

/* unitDefinitions_lst section */
static int unit();

static List unitDefinitions_lst[] =
{
   "unit",                 unit,
};

/* unit_lst section */
static int unitExponent();

static List unit_lst[] =
{
   // The following callouts can occur at most once
   "ampere",                  unitExponent,
   "candela",                 unitExponent,
   "celsius",                 unitExponent,
   "coulomb",                 unitExponent,
   "degree",                  unitExponent,
   "fahrenheit",              unitExponent,
   "farad",                   unitExponent,
   "henry",                   unitExponent,
   "hertz",                   unitExponent,
   "joule",                   unitExponent,
   "kelvin",                  unitExponent,
   "kilogram",                unitExponent,
   "meter",                   unitExponent,
   "mole",                    unitExponent,
   "ohm",                     unitExponent,
   "radian",                  unitExponent,
   "second",                  unitExponent,
   "siemens",                 unitExponent,
   "volt",                    unitExponent,
   "watt",                    unitExponent,
   "weber",                   unitExponent,
};

/* fontDefinitions_lst section */
static int fonts();

static List fontDefinitions_lst[] = 
{
   "fonts",                   fonts,
};

/* font_lst section */
static int setDistance();
static int font();

static List fonts_lst[] =
{
   "setDistance",             setDistance,
   "font",                    font,
};

/* setUnit_lst section */
static int unitRef();

static List setUnit_lst[] =
{
   "unitRef",                 unitRef,
};

/* font_lst section */
static int fontProportions();

static List font_lst[] =
{
   "fontProportions",         fontProportions,
};

/* fontProportions_lst section */
static int fontHeight();
static int fontWidth();

static List fontProportions_lst[] =
{
   "fontHeight",              fontHeight,
   "fontDescent",             skipCommand,   // not sure if needed
   "fontCapitalHeight",       skipCommand,   // not sure if needed
   "fontWidth",               fontWidth,
};

/* physicalDefaults_lst section */
static int schematicRequireDefaults();

static List physicalDefaults_lst[] =
{
   "schematicRequireDefaults",   schematicRequireDefaults,
};

/* schematicRequireDefaults_lst section */
static int schematicMetric();
static int fontRef();
static int textHeight();

static List schematicRequireDefaults_lst[] =
{
   "schematicMetric",         schematicMetric,
   "fontRef",                 fontRef,
   "textHeight",              textHeight,
};

/* schematicMetric_lst section */
static List schematicMetric_lst[] =
{
   "setDistance",             setDistance,
};

/* globalPortDefinitions_lst section */
static int globalPort();

static List globalPortDefinitions_lst[] =
{
   "globalPort",              globalPort,
   "globalPortBundle",        globalPort,
};

/* globalPort_lst section */
static int portList();
static int nameInformation();
static int property();

static List globalPort_lst[] =
{
   "globalPortList",                portList,
   "nameInformation",               nameInformation,
   "property",                      property,
   "schematicGlobalPortAttributes", skipCommand,   // describes the electric usage, don't care for now
};

/* portList_lst section */
static int globalPortRef();
static int portRef();

static List portList_lst[] =
{
   "globalPortRef",           globalPortRef,
   "portRef",                 portRef,
};

/* nameInformation_lst section */
static int primaryName();

static List nameInformation_lst[] =
{
   "primaryName",             primaryName,
};

/* primaryName_lst section */
static int displayName();

static List primaryName_lst[] =
{
   "displayName",             displayName,
};

/* property_lst section */
static int booleanPropertyValue();
static int integerPropertyValue();
static int miNoMaxPropertyValue();
static int numberPropertyValue();
static int pointPropertyValue();
static int stringPropertyValue();

static List property_lst[] =
{
   "untyped",                    skipCommand,   // don't do anything
   "boolean",                    booleanPropertyValue,
   "integer",                    integerPropertyValue,
   "miNoMax",                    miNoMaxPropertyValue,
   "number",                     numberPropertyValue,
   "point",                      pointPropertyValue,
   "string",                     stringPropertyValue,
   "nameInformation",            skipCommand,   // ignore, use reference name as attribute keyword & display name
   "property",                   skipCommand,   // ignore, don't support complex property
   "propertyInheritanceControl", skipCommand,   // ignore, don't need
   "unitRef",                    unitRef,
};

/* physicalScaling_lst section */
static int schematicUnits();
 
static List physicalScaling_lst[] =    // need to think about this 
{
   "connectivityUnits",       skipCommand,
   "interfaceUnits",          skipCommand,
   "schematicUnits",          schematicUnits,
};

/* schematicUnits_lst section */
static int setAngle();

static List schematicUnits_lst[] =
{
   "schematicMetric",         schematicMetric,
   "setAngle",                setAngle,
   "setCapacitance",          skipCommand,
   "setFrequency",            skipCommand,
   "setTime",                 skipCommand,
   "setVoltage",              skipCommand,
};

/* libraryHeader_lst section */
static int technology();

static List libraryHeader_lst[] =
{
   "edifLevel",               edifLevel,
   "nameCaseSensitivity",     skipCommand,
   "technology",              technology,
   "nameInformation",         skipCommand,   // not sure if needed or not
   "property",                skipCommand,   // not sure if needed or not
};

/* technology_lst section */
static int figureGroup();

static List technology_lst[] =
{
   "physicalScaling",         physicalScaling,
   "figureGroup",             figureGroup,
};

/* figureGroup_lst section */
static int displayAttributes();
static int pathWidth();

static List figureGroup_lst[] =
{
   "displayAttributes",       displayAttributes,
   "pathWidth",               pathWidth,
   "cornerType",              skipCommand,
   "endType",                 skipCommand,
};

/* displayAttributes_lst section */
static int color();
static int horizontalJustification();
static int verticalJustification();
static int visible();

static List displayAttributes_lst[] =
{
   "color",                   color,
   "fontRef",                 fontRef,
   "horizontalJustification", horizontalJustification,
   "textHeight",              textHeight,
   "verticalJustification",   verticalJustification,
   "visible",                 visible,
};

/* justification_lst section */
static int getJustification();

static List justification_lst[] =
{
   "leftJustify",             getJustification,
   "centerJustify",           getJustification,
   "rightJustify",            getJustification,
   "bottomJustify",           getJustification,
   "baselineJustify",         getJustification,
   "middleJustify",           getJustification,
   "caplineJustify",          getJustification,
   "topJustify",              getJustification,
};

/* geometryMacro_lst section */
static int complexGeometry();
static int geometryMacroHeader();
static int circle();
static int dot();
static int openShape();
static int path();
static int shape();
static int polygon();
static int rectangle();

static List geometryMacro_lst[] =
{
   "geometryMacroHeader",     geometryMacroHeader,
   "circle",                  circle,
   "complexGeometry",         complexGeometry,
   "dot",                     dot,
   "openShape",               openShape,
   "path",                    path,
   "polygon",                 polygon,
   "rectangle",               rectangle,
   "shape",                   shape,
};

/* geometryMacroHeader_lst section */
static int geometryMacroUnits();

static List geometryMacroHeader_lst[] =
{
   "geometryMacroUnits",      geometryMacroUnits,
   "nameInformation",         nameInformation,  // not sure if needed or not
   "property",                property,
};

/* geometryMacroUnits_lst section */
static List geometryMacroUnits_lst[] =
{
   "setAngle",                setAngle,   // not sure if needed or not
};

/* pageBorderTemplate_lst section */
static int schematicTemplateHeader();
static int annotate();
static int figure();
static int commentGraphics();
static int propertyDisplay();
static int schematicComplexFigure();

static List pageBorderTemplate_lst[] =
{
   "schematicTemplateHeader", schematicTemplateHeader,   // not sure if needed or not
   "propertyDisplay",         propertyDisplay,  // not needed since schematicTemplateHeader is not implement
   "annotate",                annotate,
   "figure",                  figure,
   "commentGraphics",         commentGraphics,
   "schematicComplexFigure",  schematicComplexFigure,
};

/* display_lst section */
static int figureGroupOverride();
static int transform();

static List display_lst[] =
{
   "figureGroupOverride",     figureGroupOverride,
   "transform",               transform,
};

/* transform_lst section */
static int origin();
static int scaleX();
static int scaleY();
static int rotation();

static List transform_lst[] =
{
   "scaleX",                  scaleX,
   "scaleY",                  scaleY,
   "rotation",                rotation,
   "origin",                  origin,
};

/* figure_lst section */
static List figure_lst[] =
{
   "figureGroupOverride",     figureGroupOverride,
   "complexGeometry",         complexGeometry,
   "circle",                  circle,
   "dot",                     dot,
   "openShape",               openShape,
   "path",                    path,
   "polygon",                 polygon,
   "rectangle",               rectangle,
   "shape",                   shape,
}; 

/* complexGeometry_lst section */
static List complexGeometry_lst[] =
{
   "geometryMacroRef",        getObjectRef,
   "transform",               transform,
};

/* commentGraphics_lst section */
static List commentGraphics_lst[] =
{
   "annotate",                annotate,
   "figure",                  figure,
};

/* getDisplay_lst section */
static int display();
static int addDisplay();
static int replaceDisplay();
static int removeDisplay();
static int propertyNameDisplay();

static List getDisplay_lst[] =
{
   "display",                 display,
   "addDisplay",              addDisplay,
   "replaceDisplay",          replaceDisplay,
   "removeDisplay",           removeDisplay,
   "propertyNameDisplay",     skipCommand,
};

/* schematicComplexFigure_lst section */
static List schematicComplexFigure_lst[] =
{
   "schematicFigureMacroRef", getObjectRef,
   "transform",               transform,
   "propertyDisplayOverride", propertyDisplay,
   "propertyOverride",        property,
};

/* pageTitleBlockTemplate_lst section */
static int pageTitleBlockAttributeDisplay();
static int pageTitleBlockAttributes();

static List pageTitleBlockTemplate_lst[] =
{
   "schematicTemplateHeader",          schematicTemplateHeader,
   "propertyDisplay",                  propertyDisplay,  
   "annotate",                         annotate,
   "commentGraphics",                  commentGraphics,
   "figure",                           figure,
   "pageTitleBlockAttributes",         pageTitleBlockAttributes,  
   "pageTitleBlockAttributeDisplay",   pageTitleBlockAttributeDisplay,  
   "schematicComplexFigure",           schematicComplexFigure,
};

/* schematicFigureMacro_lst section */
static List schematicFigureMacro_lst[] =
{
   "schematicTemplateHeader", schematicTemplateHeader,   
   "propertyDisplay",         propertyDisplay,           
   "annotate",                annotate,
   "commentGraphics",         commentGraphics,
   "figure",                  figure,
   "schematicComplexFigure",  schematicComplexFigure,
};

/* schematicTemplateHeader_lst section */
static List schematicTemplateHeader_lst[] =
{
   "schematicUnits",          schematicUnits,
   "property",                property,
};

/* schematicGlobalPortTemplate_lst section */
static int nameDisplay();

static List schematicGlobalPortTemplate_lst[] =
{
   "schematicTemplateHeader",          schematicTemplateHeader,   // not sure if needed or not
   "propertyDisplay",                  propertyDisplay,  // not needed since schematicTemplateHeader is not implement
   "annotate",                         annotate,
   "commentGraphics",                  commentGraphics,
   "figure",                           figure,
   "globalPortNameDisplay",            nameDisplay,   
   "implementationNameDisplay",        skipCommand,   // not sure if needed or not
   "schematicComplexFigure",           schematicComplexFigure,
   "schematicGlobalPortAttribute",     skipCommand,   // not sure if needed or not
};

/* getNameDisplay_lst section */
static int displayNameOverride();

static List getNameDisplay_lst[] =
{
   "display",                 display,
   "displayNameOverride",     displayNameOverride,
};

/* displayNameOverride_lst section */
static List displayNameOverride_lst[] =
{
   "display",                 display,
   "addDisplay",              addDisplay,
   "replaceDisplay",          replaceDisplay,
   "removeDisplay",           removeDisplay,
};

/* getTemplate_lst section */
static List getTemplate_lst[] =
{
   "annotate",                   annotate,
   "schematicTemplateHeader",    schematicTemplateHeader,
   "commentGraphics",            commentGraphics,
   "figure",                     figure,
   "implementationNameDisplay",  skipCommand,   // not sure if needed or not
   "propertyDisplay",            propertyDisplay,
   "schematicComplexFigure",     schematicComplexFigure,
};

/* schematicMasterPortTemplate_lst section */
static int portAttributeDisplay();
static int input();
static int output();
static int bidirectional();
static int unspecified();
static int unrestricted();
static int mixedDirection();

static List schematicMasterPortTemplate_lst[] = 
{
   "schematicTemplateHeader",    schematicTemplateHeader,
   "annotate",                   annotate,
   "commentGraphics",            commentGraphics,
   "figure",                     figure,
   "implementationNameDisplay",  skipCommand,   // not sure if needed or not
   "portAttributeDisplay",       portAttributeDisplay,
   "propertyDisplay",            propertyDisplay,  
   "schematicComplexFigure",     schematicComplexFigure,
   "input",                      input,
   "output",                     output,
   "bidirectional",              bidirectional,
   "unspecified",                unspecified,               
   "unrestricted",               unrestricted,              
   "mixedDirection",             mixedDirection,            
};

/* portAttributeDisplay_lst section */
static int portNameDisplay();
static int designatorDisplay();

static List portAttributeDisplay_lst[] =
{
   "designatorDisplay",       designatorDisplay,
   "portNameDisplay",         portNameDisplay,
   "portPropertyDisplay",     propertyDisplay,
};

/* schematicSymbolPortTemplate_lst section */
static List schematicSymbolPortTemplate_lst[] =
{
   "schematicTemplateHeader",    schematicTemplateHeader,
   "annotate",                   annotate,
   "commentGraphics",            commentGraphics,
   "figure",                     figure,
   "implementationNameDisplay",  skipCommand,   // not sure if needed or not
   "portAttributeDisplay",       portAttributeDisplay,
   "propertyDisplay",            skipCommand,   // not sure if needed or not
   "schematicComplexFigure",     schematicComplexFigure,
   "schematicPortAttribute",     skipCommand,   // not sure if needed or not
   "input",                      input,
   "output",                     output,
   "bidirectional",              bidirectional,
   "unspecified",                unspecified,               
   "unrestricted",               unrestricted,              
   "mixedDirection",             mixedDirection,            
};

/* cell_lst section */
static int cluster();

static List cell_lst[] =
{
   "cellHeader",              skipCommand,
   "cluster",                 cluster,
   "viewGroup",               skipCommand,   // not sure if needed or not
};

/* cluster_lst section */
static int Interface();
static int clusterHeader();
static int clusterConfiguration();
static int defaultClusterConfiguration();
static int schematicSymbol();
static int schematicView();

static List cluster_lst[] =
{
   "interface",                     Interface,
   "clusterHeader",                 skipCommand,
   "clusterConfiguration",          clusterConfiguration,
   "connectivityView",              skipCommand,   // not sure if needed or not
   "defaultClusterConfiguration",   defaultClusterConfiguration,   // not sure if needed or not
   "schematicSymbol",               schematicSymbol,
   "schematicView",                 schematicView,
};

/* interface_lst section */
static int designator();
static int port();

static List interface_lst[] =
{
   "interfaceUnits",          skipCommand,
   "designator",              designator,
   "port",                    port,
   "portBundle",              port,
};

/* port_lst section */
static List port_lst[] = 
{
   "portList",                   portList,
   "designator",                 designator,
   "nameInformation",            nameInformation,
   "property",                   property,
   "inputPort",                  input,
   "outputPort",                 output,
   "bidirectionalPort",          bidirectional,
   "unspecifiedDirectionPort",   unspecified
};

/* clusterHeader_lst section */
static List clusterHeader_lst[] =
{
   "nameInformation",         nameInformation,
   "property",                property,
};

/* clusterConfiguration_lst */
static int viewRef();
static int instanceConfiguration();

static List clusterConfiguration_lst[] =
{
   "viewRef",                 viewRef,
   "leaf",                    skipCommand,
   "unconfigured",            skipCommand,
   "globalPortScope",         skipCommand,
   "instanceConfiguration",   instanceConfiguration,
   "nameInformation",         skipCommand,
   "property",                skipCommand,
};

/* instanceConfiguration_lst section */
static int clusterConfigurationRef();

static List instanceConfiguration_lst[] =
{
   "clusterConfigurationRef", clusterConfigurationRef,
};

/* schematicSymbol_lst section */
static int schematicSymbolHeader();
static int cellNameDisplay();
static int instanceNameDisplay();
static int schematicSymbolPortImplementation();

static List schematicSymbol_lst[] =
{
   "schematicSymbolHeader",               schematicSymbolHeader,
   "figure",                              figure,
   "annotate",                            annotate,
   "designatorDisplay",                   designatorDisplay,
   "propertyDisplay",                     propertyDisplay,
   "cellNameDisplay",                     cellNameDisplay,
   "instanceNameDisplay",                 instanceNameDisplay,
   "schematicComplexFigure",              schematicComplexFigure,
   "schematicSymbolPortImplementation",   schematicSymbolPortImplementation,
};

/* schematicSymbolHeader_lst section */
static int schematicSymbolBorder();

static List schematicSymbolHeader_lst[] =
{
   "schematicUnits",          schematicUnits,
   "nameInformation",         nameInformation,
   "property",                property,
   "schematicSymbolBorder",   skipCommand,   // come back later
};

static List schematicSymbolBorder_lst[] =
{
   "schematicSymbolBorderTemplateRef", getObjectRef,
   "transform",                        transform,
   "propertyDisplayOverride",          propertyDisplay,
   "propertyOverride",                 property,
};

/* schematicSymbolPortImplementation_lst */
static List schematicSymbolPortImplementation_lst[] =
{
   "portRef",                          portRef,
   "schematicSymbolPortTemplateRef",   getObjectRef,  // schematicSymbolPortTemplate is not yet implemented
   "transform",                        transform,
   "portAttributeDisplay",             portAttributeDisplay,
   "propertyDisplayOverride",          propertyDisplay,
   "propertyOverride",                 property,
};

/* schematicView_lst section */
static int schematicViewHeader();
static int logicalConnectivity();
static int schematicImplementation();

static List schematicView_lst[] =
{
   "schematicViewHeader",     schematicViewHeader,
   "logicalConnectivity",     logicalConnectivity,
   "schematicImplementation", schematicImplementation,
};

/* schematicViewHeader_lst section */
static List schematicViewHeader_lst[] =
{
   "schematicUnits",          schematicUnits,
   "nameInformation",         skipCommand,   // not sure if needed or not
   "property",                skipCommand,   // not sure if needed or not
};

/* logicalConnectivity_lst section */
static int instance();
static int signal();
static int signalGroup();

static List logicalConnectivity_lst[] =
{
   "instance",                instance,
   "signal",                  signal,
   "signalGroup",             signalGroup,   // not sure if need or not since there is not sample
};

/* instance_lst section */
static int clusterRef();
static int instancePortAttributes();

static List instance_lst[] =
{
   "clusterRef",              clusterRef,
   "designator",              designator,
   "instancePortAttributes",  instancePortAttributes,
   "nameInformation",         nameInformation,
   "property",                property,
};

/* clusterRef_lst section */
static List clusterRef_lst[] =
{
   "cellRef",                 getObjectRef,
};

/* instancePortAttributes_lst section */
static int portPropertyOverride();

static List instancePortAttributes_lst[] =
{
   "designator",                       designator,
   "directionalPortAttributeOverride", skipCommand,   //  not sure if needed or not, port direction are skip
   "portPropertyOverride",             property,
   "property",                         property,
};

/* signal_lst section */
static int signalJoined();

static List signal_lst[] =
{
   "signalJoined",            signalJoined, 
   "nameInformation",         nameInformation,
   "property",                property,
};
                        
/* signalJoined_lst section */
static int portInstanceRef();

static List signalJoined_lst[] =
{
   "globalPortRef",           globalPortRef,		// reference the global port
   "portInstanceRef",         portInstanceRef,  // reference the instance port
   "portRef",                 portInstanceRef,  // portRef the master port,
};

/* portInstanceRef_lst section */
static int instanceRef();

static List portInstanceRef_lst[] =
{
   "instanceRef",             instanceRef,
   "instanceMemberRef",       skipCommand, // currently don't support width instance so skip this
};

/* signalGroup_lst section */
static int signalList();

static List signalGroup_lst[] =
{
   "signalList",              signalList,
   "nameInformation",         nameInformation,
   "property",                property,
};

/* signalList_lst section */
static int signalGroupRef();
static int signalRef();

static List signalList_lst[] =
{
   "signalGroupRef",          signalGroupRef,
   "signalRef",               signalRef,
};

/* schematicImplementation_lst section */
static int totalPages();
static int page();

static List schematicImplementation_lst[] =
{
   "totalPages",              totalPages, // not sure if needed or not
   "page",                    page,
};

/* page_lst section */
static int pageHeader();
static int pageCommentGraphics();
static int pageTitleBlock();
static int schematicBus();
static int schematicGlobalPortImplementation();
static int schematicInstanceImplementation();
static int schematicMasterPortImplementation();
static int schematicNet();
static int schematicOffPageConnectorImplementation();
static int schematicOnPageConnectorImplementation();
static int schematicRipperImplementation();

static List page_lst[] =
{
   "pageHeader",                                pageHeader,
   "localPortGroup",                            skipCommand,   // no sample
   "pageCommentGraphics",                       pageCommentGraphics,
   "pageTitleBlock",                            pageTitleBlock,   
   "propertyDisplay",                           propertyDisplay,
   "schematicBus",                              schematicBus,
   "schematicGlobalPortImplementation",         schematicGlobalPortImplementation,
   "schematicInstanceImplementation",           schematicInstanceImplementation,
   "schematicMasterPortImplementation",         schematicMasterPortImplementation,
   "schematicNet",                              schematicNet,  //schematicNet,
   "schematicOffPageConnectorImplementation",   schematicOffPageConnectorImplementation,
   "schematicOnPageConnectorImplementation",    schematicOnPageConnectorImplementation,
   "schematicRipperImplementation",             schematicRipperImplementation,
   "viewPropertyDisplay",                       skipCommand,
};

/* pageHeader_lst section */
static int pageBorder();

static List pageHeader_lst[] =
{
   "nameInformation",         nameInformation,
   "pageBorder",              pageBorder, // just skip for now, no sample
   "pageSize",                skipCommand,   // not sure if needed or not
   "property",                property,
};

/* pageBorder_lst section */
static List pageBorder_lst[] =
{
   "pageBorderTemplateRef",   getObjectRef,
   "transform",               transform,
   "propertyDisplayOverride", propertyDisplay,
   "propertyOverride",        property,
};

/* pageCommentGraphics_lst section */
static List pageCommentGraphics_lst[] =
{
   "annotate",                annotate,
   "figure",                  figure,
   "schematicComplexFigure",  schematicComplexFigure,
};

/* pageTitleBlock_lst section */
static int pagePropertyDisplay();

static List pageTitleBlock_lst[] =
{
   "pageTitleBlockTemplateRef",        getObjectRef,
   "transform",                        transform,
   "nameInformation",                  nameInformation,
   "pagePropertyDisplay",              propertyDisplay,  // display property defined in pageHeader
   "pageTitleBlockAttributeDisplay",   pageTitleBlockAttributeDisplay,
   "pageTitleBlockAttributes",         pageTitleBlockAttributes,     
   "property",                         property,
   "propertyDisplay",                  propertyDisplay,
   "propertyDisplayOverride",          propertyDisplay,
   "propertyOverride",                 property,
};

/* pageTitleBlockAttributeDisplay_lst section */
static int approvedDateDisplay();
static int checkDateDisplay();
static int companyNameDisplay();
static int contractDisplay();
static int drawingDescriptionDisplay();
static int drawingIdentificationDisplay();
static int drawingSizeDisplay();
static int engineeringDateDisplay();
static int originalDrawingDateDisplay();
static int pageIdentificationDisplay();
static int pageTitleDisplay();
static int revisionDisplay();
static int totalPageDisplay();

static List pageTitleBlockAttributeDisplay_lst[] =
{
   "approvedDateDisplay",           approvedDateDisplay,
   "checkDateDisplay",              checkDateDisplay,
   "companyNameDisplay",            companyNameDisplay,
   "contractDisplay",               contractDisplay,
   "drawingDescriptionDisplay",     drawingDescriptionDisplay,
   "drawingIdentificationDisplay",  drawingIdentificationDisplay,
   "drawingSizeDisplay",            drawingSizeDisplay,
   "engineeringDateDisplay",        engineeringDateDisplay,
   "originalDrawingDateDisplay",    originalDrawingDateDisplay,
   "pageIdentificationDisplay",     pageIdentificationDisplay,
   "pageTitleDisplay",              pageTitleDisplay,
   "revisionDisplay",               revisionDisplay,
   "totalPageDisplay",              totalPageDisplay,
};

/* pageTitleBlockAttributes_lst section */
static int approvedDate();
static int checkDate();
static int companyName();
static int contract();
static int drawingDescription();
static int drawingIdentification();
static int drawingSize();
static int engineeringDate();
static int originalDrawingDate();
static int pageIdentification();
static int pageTitle();
static int revision();

static List pageTitleBlockAttributes_lst[] =
{
   "approvedDate",            approvedDate,
   "checkDate",               checkDate,
   "companyName",             companyName,
   "contract",                contract,
   "drawingDescription",      drawingDescription,
   "drawingIdentification",   drawingIdentification,
   "drawingSize",             drawingSize,
   "engineeringDate",         engineeringDate,
   "originalDrawingDate",     originalDrawingDate,
   "pageIdentification",      pageIdentification,
   "pageTitle",               pageTitle,
   "revision",                revision,
};

/* schematicGlobalPortImplementation_lst section */
static int globalPortPropertyDisplay();

static List schematicGlobalPortImplementation_lst[] =
{
   "schematicGlobalPortTemplateRef",   getObjectRef,
   "globalPortRef",                    globalPortRef,
   "transform",                        transform,
   "globalPortNameDisplay",            nameDisplay,
   "globalPortPropertyDisplay",        globalPortPropertyDisplay,
   "nameInformation",                  nameInformation,
   "propertyDisplayOverride",          propertyDisplay,
   "propertyDisplay",                  propertyDisplay,
   "propertyOverride",              property,
};

/* globalPortPropertyDisplay_lst section */
static List globalPortPropertyDisplay_lst[] =
{
   "display",                 display,
   "propertyNameDisplay",     skipCommand,
};


/* schematicInstanceImplementation_lst section */
static int schematicSymbolRef();
static int instancePortAttributeDisplay();

static List schematicInstanceImplementation_lst[] =
{
   "instanceRef",                   instanceRef,
   "schematicSymbolRef",            schematicSymbolRef,
   "transform",                     transform,
   "designatorDisplay",             designatorDisplay,
   "cellNameDisplay",               cellNameDisplay,
   "instanceNameDisplay",           instanceNameDisplay,    //instanceNameDisplay,  // location to display instance name
   "instancePortAttributeDisplay",  instancePortAttributeDisplay,
   "instancePropertyDisplay",       propertyDisplay,
   "nameInformation",               nameInformation,
   "pageCommentGraphics",           pageCommentGraphics,
   "propertyDisplayOverride",       propertyDisplay,
   "propertyDisplay",               propertyDisplay,
   "propertyOverride",              property,
};

static List instancePortAttributeDisplay_lst[] =
{
   "portRef",                       portRef,
   "portAttributeDisplay",          portAttributeDisplay,
   "portPropertyDisplayOverride",   propertyDisplay,
};

/* schematicMasterPortImplementation_lst section */
static List schematicMasterPortImplementation_lst[] =
{
   "schematicMasterPortTemplateRef",   getObjectRef,
   "portRef",                          portRef,
   "localPortGroupRef",                skipCommand,   // not sure if needed or not
   "transform",                        transform,
   "nameInformation",                  nameInformation,
   "portAttributeDisplay",             portAttributeDisplay,
   "propertyDisplayOverride",          propertyDisplay,
   "propertyOverride",                 property,
};

/* schematicNet_lst section */
static int schematicInterconnectHeader();
static int schematicNetJoined();
static int schematicInterconnectAttributeDisplay();
static int schematicNetDetails();

static List schematicNet_lst[] =
{
   "signalRef",                              signalRef,
   "schematicInterconnectHeader",            schematicInterconnectHeader,
   "schematicNetJoined",                     schematicNetJoined,
   "schematicInterconnectAttributeDisplay",  schematicInterconnectAttributeDisplay, // display location for property in schematicInterconnectHeader
   "schematicNetDetails",                    schematicNetDetails,
};

/* schematicInterconnectHeader_lst section */
static int schematicInterconnectTerminatorImplementation();
static int schematicJunctionImplementation();

static List schematicInterconnectHeader_lst[] =
{
   "nameInformation",                                 nameInformation,
   "property",                                        property,   // property of schematicNet
   "schematicInterconnectTerminatorImplementation",   schematicInterconnectTerminatorImplementation,
   "schematicJunctionImplementation",                 schematicJunctionImplementation,
};

/* schematicInterconnectTerminatorImplementation_lst section */
static List schematicInterconnectTerminatorImplementation_lst[] =
{
   "schematicInterconnectTerminatorTemplateRef",   getObjectRef,
   "transform",                                    transform,
   "nameInformation",                              nameInformation, //nameInformation,
   "propertyDisplayOverride",                      propertyDisplay,
   "propertyOverride",                             property,
};

/* schematicJunctionImplementation_lst section */
static List schematicJunctionImplementation_lst[] =
{
   "schematicJunctionTemplateRef",  getObjectRef,                 
   "transform",                     transform,
   "nameInformation",               nameInformation, //nameInformation,
   "propertyDisplayOverride",       propertyDisplay,
   "propertyOverride",              property,
};

/* schematicNetJoined_lst section */
static int portJoined();
static int ripperHotSpotRef();
static int schematicGlobalPortImplementationRef();
static int schematicMasterPortImplementationRef ();
static int schematicOnPageConnectorImplementationRef();
static int schematicOffPageConnectorImplementationRef();

static List schematicNetJoined_lst[] =
{
   // currently we only care about listing the portJoined in the net list
   // and skip the rest of the callouts until request by customers
   "portJoined",												skipCommand, 
	"ripperHotSpotRef",										ripperHotSpotRef,
   "schematicGlobalPortImplementationRef",        schematicGlobalPortImplementationRef,
   "schematicMasterPortImplementationRef",         schematicMasterPortImplementationRef,
	"schematicOffPageConnectorImplementationRef",	schematicOffPageConnectorImplementationRef,
	"schematicOnPageConnectorImplementationRef",		schematicOnPageConnectorImplementationRef,
};

/* ripperHotSpotRef_lst section */
static int schematicRipperImplementationRef();
static List ripperHotSpotRef_lst[] =
{
	"schematicRipperImplementationRef",					schematicRipperImplementationRef,
};

/* schematicInterconnectAttributeDisplay_lst section */
static int interconnectAttachedText();
static int interconnectNameDisplay();
static int interconnectPropertyDisplay();

/* schematicInterconnectAttributeDisplay_lst section */
static List schematicInterconnectAttributeDisplay_lst[] =
{
   "interconnectAttachedText",      interconnectAttachedText,
   "interconnectNameDisplay",       interconnectNameDisplay,
   "interconnectPropertyDisplay",   interconnectPropertyDisplay,
};

/* interconnectAttachedText_lst */
static List interconnectAttachedText_lst[] =
{
   "annotate",                annotate,
   "interconnectNameDisplay",    interconnectNameDisplay,
   "interconnectPropertyDisplay",   interconnectPropertyDisplay,
};

/* schematicNetDetails_lst section */
static int schematicNetGraphics();
static int schematicSubNetSet();

static List schematicNetDetails_lst[] =
{
   "schematicNetGraphics",    schematicNetGraphics,
   "schematicSubNetSet",      schematicSubNetSet,
};

/* schematicNetGraphics_lst section */
static List schematicNetGraphics_lst[] =
{
   "figure",                  figure,
   "schematicComplexFigure",  schematicComplexFigure,
};

/* schematicSubNetSet section */ 
static int schematicSubNet();

static List schematicSubNetSet_lst[] =
{
   "schematicSubNet",         schematicSubNet,
};

/* schematicSubNet section */
static int schematicSubInterconnectHeader();

static List schematicSubNet_lst[] =
{                   
   "schematicSubInterconnectHeader",         skipCommand, // schematicSubInterconnectHeader,
   "schematicNetJoined",                     schematicNetJoined,
   "schematicInterconnectAttributeDisplay",  skipCommand, // schematicInterconnectAttributeDisplay,   
   "schematicNetDetails",                    schematicNetDetails,
};

/* schematicSubInterconnectHeader_lst section */
static List schematicSubInterconnectHeader_lst[] =
{
   "nameInformation",         nameInformation,
   "property",                property,
};

/* schematicOffPageConnectorImplementation_lst section */
static List schematicOffPageConnectorImplementation_lst[] =
{
   "schematicOffPageConnectorTemplateRef",   getObjectRef,
   "transform",                              transform,
   "associatedInterconnectNameDisplay",      skipCommand,   // not sure if needed or not
   "nameInformation",                        nameInformation,
   "property",                               property,
   "propertyDisplay",                        propertyDisplay,
   "propertyDisplayOverride",                propertyDisplay,
   "propertyOverride",                       property,
};

/* schematicOnPageConnectorImplementation_lst section */
static List schematicOnPageConnectorImplementation_lst[] =
{
   "schematicOnPageConnectorTemplateRef",    getObjectRef,
   "transform",                              transform,
   "associatedInterconnectNameDisplay",      skipCommand,   // not sure if needed or not
   "nameInformation",                        nameInformation,
   "property",                               property,
   "propertyDisplay",                        propertyDisplay,
   "propertyDisplayOverride",                propertyDisplay,
   "propertyOverride",                       property,
};

static List schematicRipperImplementation_lst[] =
{
   "schematicRipperTemplateRef",    getObjectRef,
   "transform",                     transform,
   "nameInformation",               nameInformation,
   "property",                      property,
   "propertyDisplay",               propertyDisplay,
   "propertyDisplayOverride",       propertyDisplay,
   "propertyOverride",              property,
};

/* designHeader_lst section */
static int designUnits();

static List designHeader_lst[] =
{
   "designUnits",             designUnits,
   "nameInformation",         nameInformation,
   "property",                property,
};                

/* designUnits_lst section */
static List designUnits_lst[] =
{
   "setCapacitance",          skipCommand,
   "setTime",                 skipCommand,
};

/* designHierarchy_lst section */
static int designHierarchyHeader();
static int occurrenceHierarchyAnnotate();

static List designHierarchy_lst[] =
{
   "clusterRef",                    clusterRef,
   "clusterConfigurationRef",       clusterConfigurationRef,
   "designHierarchyHeader",         skipCommand, // designHierarchyHeader, // don't need
   "occurrenceHierarchyAnnotate",	occurrenceHierarchyAnnotate,
};

static List designHierarchyHeader_lst[] =
{
   "nameInformation",         nameInformation,
   "property",                property,
};

/* occurrenceHierarchyAnnotate_lst section */
static int interconnectAnnotate();
static int leafOccurrenceAnnotate();
static int occurrenceAnnotate();
static int pageAnnotate();
static int portAnnotate();
static int signalAnnotate();

static List occurrenceHierarchyAnnotate_lst[] =
{
   "interconnectAnnotate",    skipCommand, // interconnectAnnotate,
   "leafOccurrenceAnnotate",  leafOccurrenceAnnotate,
   "occurrenceAnnotate",      occurrenceAnnotate,
   "pageAnnotate",            skipCommand, // pageAnnotate,
   "portAnnotate",            skipCommand, // portAnnotate, // annotate for master port
   "signalAnnotate",          signalAnnotate, // signalAnnotate,
   "signalGroupAnnotate",     skipCommand, // not sure if needed or not since signalGroup is skip
};

/* leafOccurrenceAnnotate_lst section */
static int instancePropertyOverride();
static int portAnnotate();

static List leafOccurrenceAnnotate_lst[] =
{
   "designator",                 designator,
   "instancePropertyOverride",   property,
   "portAnnotate",               portAnnotate,
   "property",                   property,
};

/* portAnnotate_lst section */
static List portAnnotate_lst[] =
{
   "designator",                       designator,
   "directionalPortAttributeOverride", skipCommand,   // not sure if needed or not, so skip anything associate with port direction
   "portPropertyOverride",             property,
   "property",                         property,
};

/* occurrenceAnnotate_lst section */
static List occurrenceAnnotate_lst[] =
{
   "designator",                 designator,
   "instancePropertyOverride",   instancePropertyOverride,
   "interconnectAnnotate",       skipCommand, // interconnectAnnotate,
   "leafOccurrenceAnnotate",     leafOccurrenceAnnotate,
   "occurrenceAnnotate",         occurrenceAnnotate,
   "pageAnnotate",               skipCommand,
   "portAnnotate",               portAnnotate,
   "property",                   property,
   "signalAnnotate",             signalAnnotate,
   "signalGroupAnnotate",        skipCommand,   // not sure if needed or not since signalGroup is skip
};

/* pageAnnotate_lst section */
static List pageAnnotate_lst[] =
{
   "interconnectAnnotate",    interconnectAnnotate,
};

/* interconnectAnnotate_lst section */
static List interconnectAnnotate_lst[] =
{
   "interconnectAnnotate",    interconnectAnnotate,
   "property",                property,
   "propertyOverride",        property,
};

/* signalAnnotate section */
static List signalAnnotate_lst[] =
{
   "property",                property,
   "propertyOverride",        property,
};

/* schematicBus_lst section */
static int schematicBusJoined();
static int schematicBusDetails();
static int schematicBusSlice();

static List schematicBus_lst[] =
{
   "signalGroupRef",                         signalGroupRef,
   "schematicBusDetails",                    schematicBusDetails,
   "schematicBusJoined",                     schematicBusJoined,
   "schematicBusSlice",                      schematicBusSlice,
   "schematicInterconnectHeader",            schematicInterconnectHeader,
   "schematicInterconnectAttributeDisplay",  schematicInterconnectAttributeDisplay,
};

/* schematicBusJoined_lst section */
static List schematicBusJoined_lst[] =
{
   // currently we only care about listing the portJoined in the net list
   // and skip the rest of the callouts until request by customers
   "portJoined",												skipCommand, 
	"ripperHotSpotRef",										ripperHotSpotRef,
   "schematicGlobalPortImplementationRef",        schematicGlobalPortImplementationRef,
   "schematicMasterPortImplementationRef",         schematicMasterPortImplementationRef,
	"schematicOffPageConnectorImplementationRef",	schematicOffPageConnectorImplementationRef,
	"schematicOnPageConnectorImplementationRef",		schematicOnPageConnectorImplementationRef,
};

/* schematicBusDetails_lst section */
static int schematicBusGraphics();
static int schematicSubBusSet();

static List schematicBusDetails_lst[] =
{
   "schematicBusGraphics",    schematicBusGraphics,
   "schematicSubBusSet",      schematicSubBusSet,
};   

/* schematicBusGraphics_lst section */
static List schematicBusGraphics_lst[] =
{
   "figure",                  figure,
   "schematicComplexFigure",  schematicComplexFigure,
};

/* schematicSubBusSet_lst section */
static int schematicSubBus();

static List schematicSubBusSet_lst[] =
{
   "schematicSubBus",         schematicSubBus,
};

/* schematicSubBus_lst section */
static List schematicSubBus_lst[] =
{
   "schematicBusDetails",                    schematicBusDetails,
   "schematicBusJoined",                     schematicBusJoined,
   "schematicBusSlice",                      schematicBusSlice,
   "schematicSubInterconnectHeader",         skipCommand, // schematicSubInterconnectHeader, // don't care
   "schematicInterconnectAttributeDisplay",  skipCommand, // schematicInterconnectAttributeDisplay,   
}; 


class CSignal;
class CEdif;
class CNet;
class CBus;
class CInstance;
class CEdifView;
class CClusterConfiguration;
enum EConnectorType
{
	connectorTypeGlobalPort,
	connectorTypeMasterPort,
	connectorTypeInstancePort,
	connectorTypeOnPageConnector,
	connectorTypeOffPageConnecotor,
	connectorTypeRipper,
	connectorTypeUndefined,
};

enum ESignalType
{
	signalTypeNet,
	signalTypeBus,
	signalTypeUndefined,
};

enum ERenamedBy
{
	renamedByGlobalPort,
	renamedByOnPageConnector,
	renamedByOffPageConnector,
	renamedByMasterPort,
	renamedByBus,
	renamedByAlias,
	renamedByUndefined,
};


/* Structures Section *********************************************************/

typedef struct
{
   CString  basicUnitName;
   double   numOfNewUnits;
   double   numOfBasicUnits;
}EdifUnitDef;

typedef struct
{
   double   fontHeight;
   double   fontWidth;
}EdifFontDef;

typedef struct
{
   CString  unitRef;
   double   fontHeight;
   double   fontWidth;
   double   textHeight;
}EdifPhysicalDefault;

typedef CTypedPtrList<CPtrList, CAttributes*> CMapWordToAttributesList;

typedef struct
{
   CString           clusterRef;
   CString           cellRef;
   CString           libraryRef;
   CString           displayName;
   CString           refdes;           // this is the implementation name of schematicInstanceImplementation
   CAttributes*      AttribMap;        // a map of and instance's attributes
   CMapStringToPtr   portAttribMap;    // a map of ports' attribute map which contains overwriting value only
                                       // the key is the port ref
   CMapStringToPtr   portAttribDisplayMap;   // a map of ports' attribute map which contains overwriting location only
                                             // the key is the port implementation ref
   CAttributes*& getAttributesRef()					{ return AttribMap; }
}EdifInstance;


////////////////////////////////////////////////////////////////////////////////
// CFigureGroup
////////////////////////////////////////////////////////////////////////////////
class CFigureGroup
{
public:
	CFigureGroup();
	CFigureGroup(const CFigureGroup& other);
	~CFigureGroup();

private:
   int m_iColor;
   int m_iHorizontalAlignment;
   int m_iVerticalAlignement;
   int m_iWidthIndex;
   double m_dFontHeight;
   double m_dFontWidth;
	bool m_bVisible;

public:
	void Reset();

	int GetColor() const											{ return m_iColor;							}
	int GetHorizontalAlignment() const						{ return m_iHorizontalAlignment;			}
	int GetVerticalAlignment() const							{ return	m_iVerticalAlignement;			}
	int GetWidthIndex() const									{ return m_iWidthIndex;						}
	double GetFontHeight() const								{ return m_dFontHeight;						}
	double GetFontWidth() const								{ return m_dFontWidth;						}
	bool GetVisible() const										{ return m_bVisible;							}

	void SetColor(const int color)							{ m_iColor = color;							}
	void SetHorizontalAlignment(const int alignment)	{ m_iHorizontalAlignment = alignment;	}
	void SetVerticalAlignment(const int alignment)		{ m_iVerticalAlignement = alignment;	}
	void SetWidthIndex(int widthIndex)						{ m_iWidthIndex = widthIndex;				}
	void SetFontHeight(double height)						{ m_dFontHeight = height;					}
	void SetFontWidth(double width)							{ m_dFontWidth = width;						}
	void SetVisible(bool visible)								{ m_bVisible = visible;						}
};
typedef CTypedPtrMap<CMapStringToPtr, CString, CFigureGroup*> CFigureGroupMap;


////////////////////////////////////////////////////////////////////////////////
// CEdifTransform
////////////////////////////////////////////////////////////////////////////////
class CTransform
{
public:
	CTransform();
	CTransform(const CTransform& other);
	~CTransform();

private:
	double m_dScaleX;
	double m_dScaleY;
	double m_dRotation;
	CPnt* m_pOrigin;

public:
	void Reset();

	double GetScaleX() const									{ return m_dScaleX;							}
	double GetScaleY() const									{ return m_dScaleY;							}
	double GetRotation() const									{ return m_dRotation;						}
	CPnt* GetOrigin() const										{ return m_pOrigin;							}

	void SetScaleX(const double scaleX)						{ m_dScaleX = scaleX;						}
	void SetScaleY(const double scaleY)						{ m_dScaleY = scaleY;						}
	void SetRotation(const double rotation)				{ m_dRotation = rotation;					}
	void SetOrigin(const CPnt& origin);
};
typedef CTypedPtrList<CPtrList, CTransform*> CTransformList;


////////////////////////////////////////////////////////////////////////////////
// CDisplay
////////////////////////////////////////////////////////////////////////////////
class CDisplay
{
public:
	CDisplay();
	CDisplay(const CDisplay& other);
	~CDisplay();

private:
	CFigureGroup* m_pFigureGroup;
	CTransform* m_pTransform;

	CDisplay* m_pNext;

public:
	CDisplay* GetNextDisplay() const							{ return m_pNext;							}
	CFigureGroup* GetFigureGroup() const					{ return m_pFigureGroup;					}
	CTransform* GetTransform() const							{ return m_pTransform;						}

	void SetNextDisplay(CDisplay* display);
	void SetFigureGroup(CFigureGroup* figureGroup);
	void SetTransform(CTransform* transform);
};
typedef CTypedPtrList<CPtrList, CDisplay*> CDisplayList;


//-----------------------------------------------------------------------------
// EdifPortDef
//-----------------------------------------------------------------------------
class CEdifPort
{
public:
	CEdifPort();
	~CEdifPort();

   CAttributes*   attribMap;
   BOOL           isBundle;
   CStringArray   portRefArr;
};
typedef CTypedMapStringToPtrContainer<CEdifPort*> CEdifPortMap;

//-----------------------------------------------------------------------------
// CLeafAnnotate
//-----------------------------------------------------------------------------
class CLeafAnnotate
{
public:
	CLeafAnnotate();
	~CLeafAnnotate();

private:
	CString m_sName;
	CString m_sDesignator;
	CAttributes* m_pAttribMap;
	CEdifPortMap m_portMap;

	void reset();

public:
	CString GetName() const			{ return m_sName; };

	int DoLeafAnnotate();
	void UpdateLogicSymbol(DataStruct* logicSymbol);
	void UpdateLogicSymbol(CInstance* instance);
};

//-----------------------------------------------------------------------------
// CSignalAnnotate
//-----------------------------------------------------------------------------
class CSignalAnnotate
{
public:
   CSignalAnnotate();
   ~CSignalAnnotate();

private:
	CString m_sName;
	CAttributes* m_pAttribMap;

public:
	CString GetName() const			{ return m_sName; };

   int doSignalAnnotate();
   void updateSingal(CSignal* signal);
}; 

//-----------------------------------------------------------------------------
// COccurrenceAnnotate
//-----------------------------------------------------------------------------
class COccurrenceAnnotate
{
public:
	COccurrenceAnnotate(bool useAsHierarchyAnnotate);
	~COccurrenceAnnotate();

private:
	CString m_sName;
	CString m_sDesignator;
	CString m_sHierarchGeomNums;
	bool m_bUseAsHierarchyAnnotate;

	CAttributes* m_pAttribMap;
	DataStruct* m_pHierarchySymbol;
	CEdifPortMap m_portMap;
	CTypedMapStringToPtrContainer<CLeafAnnotate*> m_leafAnnotateMap;
   CTypedMapStringToPtrContainer<CSignalAnnotate*> m_signalfAnnotateMap;
	CStringList m_netNameList;

   CInstance* m_refInstance;
   CEdifView* m_refView;
   CClusterConfiguration* m_clusterConfiguration;

	void reset();
   void applyOccurrenceAnnotateToView(CEdifView* view);
   void applyOccurrenceAnnotateToInstance(CInstance* instance);

public:
   CInstance* getInstance()                              { return m_refInstance;             }
   void setInstance(CInstance* instance)                 { m_refInstance = instance;         }

   CClusterConfiguration* getClusterConfiguration()      { return m_clusterConfiguration;    }
   void setClusterConfiguration(CClusterConfiguration* configuration) { m_clusterConfiguration = configuration; }

	void AddLeafAnnotate(CLeafAnnotate* leafAnnotate);
   void addSignalAnnotate(CSignalAnnotate* signalAnnotate);
	int DoOccurrenceAnnotate();
};
typedef CTypedPtrListContainer<COccurrenceAnnotate*> COcurrenceAnnotateList;

//-----------------------------------------------------------------------------
// CConnectorObject
//-----------------------------------------------------------------------------
class CConnectorObject
{
public:
	CConnectorObject(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType)
		: m_edif(edif)
		, m_libraryName(libraryName)
		, m_cellName(cellName)
		, m_clusterName(clusterName)
		, m_viewName(viewName)
		, m_pageName(pageName)
		, m_name(name)
		, m_connectorType(connectorType)
	{
		m_primaryName.IsEmpty();
		m_designator.IsEmpty();
	};
	~CConnectorObject() {};

protected:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_viewName;
	CString m_pageName;
	CString m_name;			// instance name of the port			
	CString m_primaryName;
	CString m_designator;
	EConnectorType m_connectorType;

public:
	void setPrimaryName(const CString primaryName)		{ m_primaryName = primaryName;			}
	void setDesignator(const CString designator)			{ m_designator = designator;				}
	CString getName() const										{ return m_name;								}
	CString getPrimaryName() const							{ return m_primaryName;						}
	CString getDesignator() const								{ return m_designator;						}
	EConnectorType getcConnectorType() const				{ return m_connectorType;					}
};

//-----------------------------------------------------------------------------
// CPort
//	 - CPort is used for global and master port
//-----------------------------------------------------------------------------
class CPort : public CConnectorObject
{
public:
	CPort(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType);
	~CPort();

private:
	CString m_portRef;		// reference name of global or master port defined at interface
	DataStruct* m_portDataStruct;
	CSignal* m_connectedToSignal;

	void reset();

public:
	CString getPortRef()	const									{ return m_portRef;							}
	void setPortRef(const CString portRef)					{ m_portRef = portRef;						}

	DataStruct* getPortDataStruct()							{ return m_portDataStruct;					}
	void setPortDataStruct(DataStruct* portDataStruct)	{ m_portDataStruct = portDataStruct;	}

	CSignal* getConnectedToSignal()							{ return m_connectedToSignal;				}
	void setConnectedToSignal(CSignal* signal)			{ m_connectedToSignal = signal;			}
};

//-----------------------------------------------------------------------------
// CInstancePort
//  - CInstancePort is used for port inside each instance of symbol
//-----------------------------------------------------------------------------
class CInstancePort : public CConnectorObject
{
public:
	CInstancePort(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString instanceName, const CString name, const EConnectorType connectorType);
   CInstancePort(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString instanceName, const CInstancePort& instancePort);
	~CInstancePort();

private:
	CString m_instanceName;
	CString m_designator;            // be from instancePortAttribute() in instance() or from attribute on DataStruct
	CSignal* m_connectedToSignal;
   //CAttributes* m_attribMap;				// attribute map which contains overwriting value only
   //CAttributes* m_attribDisplayMap;    // attribute map which contains overwriting location only

	void reset();

public:
	CString getInstnaceName() const						{ return m_instanceName;				}

	CString getDesignator() const							{ return m_designator;					}
	void setDesignator(const CString designator)		{ m_designator = designator;			}	

	CSignal* getConnectedToSignal()						{ return m_connectedToSignal;			}
	void setConnectedToSignal(CSignal* signal)		{ m_connectedToSignal = signal;		} 
};

//-----------------------------------------------------------------------------
// CPageConnector
//-----------------------------------------------------------------------------
class CPageConnector : public CConnectorObject
{
public:
	CPageConnector(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType);
	~CPageConnector();

private:
	DataStruct* m_connectorDataStruct;
	CSignal* m_connectedToSignal;

	void reset();

public:
	DataStruct* getConnectorDataStruct()						{ return m_connectorDataStruct;						}
	void setConnectorDataStruct(DataStruct* dataStruct)	{ m_connectorDataStruct = dataStruct;				}

	CSignal* getConnectedToSignal()								{ return m_connectedToSignal;							}
	void setConnectedToSignal(CSignal* signal)				{ m_connectedToSignal = signal;						}
};

//-----------------------------------------------------------------------------
// CRipperHotspot
//-----------------------------------------------------------------------------
class CRipperHotspot
{
public:
   CRipperHotspot(const CString name);
   ~CRipperHotspot();

private:
   CString m_name;
   CString m_pin;
   CString m_rule;
   CSignal* m_connectedToSignal;

   void reset();

public:
   CString getName() const                               { return m_name;                                }
   CString getPin() const                                { return m_pin;                                 }
   void setPin(const CString pin)                        { m_pin = pin;                                  }
   CString getRule() const                               { return m_rule;                                }
   void setRule(const CString rule)                      { m_rule = rule;                                }
	CSignal* getConnectedToSignal()								{ return m_connectedToSignal;		   				}
	void setConnectedToSignal(CSignal* signal)				{ m_connectedToSignal = signal;						}
};

//-----------------------------------------------------------------------------
// CRipper
//-----------------------------------------------------------------------------
class CRipper : public CConnectorObject
{
public:
	CRipper(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType);
	~CRipper();

private:
	CString m_wireRule;
   CString m_hotspotConnectedToBus;
	CSignal* m_connectedToSignal1;
	CSignal* m_connectedToSignal2;
	CTypedMapStringToPtrContainer<CRipperHotspot*> m_ripperHotspotMap;
   CAttributes* m_attribMap;									

	void reset();

public:
   CAttributes*& getAttributesRef()							   { return m_attribMap; }

	CSignal* getConnectedToSignal1()								{ return m_connectedToSignal1;						}
	void setConnectedToSignal1(CSignal* signal)				{ m_connectedToSignal1 = signal;						}
	CSignal* getConnectedToSignal2()								{ return m_connectedToSignal2;						}
	void setConnectedToSignal2(CSignal* signal)				{ m_connectedToSignal2 = signal;						}
	CNet* getConnectedToNet(CNet* fromNet);

   CRipperHotspot* addRipperHotspot(const CString ripperHotspotName);
   CRipperHotspot* findRipperHotspot(const CString ripperHotspotName);
   int getRipperHotspotCount() const                     { return m_ripperHotspotMap.GetCount();         }
   POSITION getRipperHotspotStartPosition() const        { return m_ripperHotspotMap.GetStartPosition(); }
   CRipperHotspot* getNextRipperHotspot(POSITION& pos)
   {
      if (pos == NULL)
         return NULL;

      CRipperHotspot* hotspot = NULL;
      CString key;
      m_ripperHotspotMap.GetNextAssoc(pos, key, hotspot);
      return hotspot;
   }
};

//-----------------------------------------------------------------------------
// CSignalObject
//-----------------------------------------------------------------------------
class CSignal
{
public:
	CSignal(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name, const ESignalType signalType);
	~CSignal();

protected:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_viewName;
	CString m_name;
	CString m_primaryName;
	CString m_annotateName;
   CString m_renamedName;
	ESignalType m_signalType;
	bool m_isNameUserDefined;
	bool m_isRenameDone;
	ERenamedBy m_renamedBy;
   CAttributes* m_attribMap;

	// Place holder of instance specific ports created in CInstance, the key is instance name and port ref
	CTypedMapStringToPtrContainer<CInstancePort*> m_instancePortMap;		

	// The following maps are place holder of point to objects created in CSchematicsView, the key is implementationRef
	CTypedMapStringToPtrContainer<CPort*> m_globalPortMap; 
	CTypedMapStringToPtrContainer<CPort*> m_masterPortMap;
	CTypedMapStringToPtrContainer<CPageConnector*> m_onPageConnectorMap;
	CTypedMapStringToPtrContainer<CPageConnector*> m_offPageConnectorMap;
	CTypedMapStringToPtrContainer<CRipper*> m_ripperMap;

	void reset();
	void applyRenamedNameToConnectedNets(CNet* connectedFromNet, const CString renamedName, const ERenamedBy renamedBy);
	void resetRenameDoneOnConnectedNets(CNet* connectedFromNet);

public:
   CAttributes*& getAttributesRef()							{ return m_attribMap;                  }

   CString getName() const										{ return m_name;								}
	ESignalType getSignalType() const						{ return m_signalType;						}
//	bool isNameUserDefined() const							{ return m_isNameUserDefined;				}

	CString getPrimaryName() const							{ return m_primaryName;						}
	void setPrimaryName(const CString primaryName)		{ m_primaryName = primaryName;			}

	CString getAnnotateName() const							{ return m_annotateName;					}
	void setAnnotateName(const CString annotateName)	{ m_annotateName = annotateName;			}

   CString getRenamedName() const                     { return m_renamedName;                }
   void setRenamedName(const CString renamedName, const ERenamedBy renamedBy);

	bool isRenameDone() const									{ return m_isRenameDone;					}
	void setRenameDone(const bool done)						{ m_isRenameDone = done;					}

	ERenamedBy getRenamedBy() const							{ return m_renamedBy;							}
	bool hasMasterPort() const									{ return m_masterPortMap.GetCount() > 0;	}
	bool hasGlobalPort() const									{ return m_globalPortMap.GetCount() > 0;	}

	void addInstancePort(CInstancePort*);
   void addGlobalPort(CPort*);
	void addMasterPort(CPort*);
	void addOnPageConnector(CPageConnector*);
	void addOffPageConnector(CPageConnector*);
	void addRipper(CRipper*);
	void prepareForRename();
};

//-----------------------------------------------------------------------------
// CNet
//-----------------------------------------------------------------------------
class CNet : public CSignal
{
public:
	CNet(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name, const ESignalType signalType);
   CNet(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CNet& net);
	~CNet();

   void reset();

private:
	bool isUserDefineName();
	CNet* getAliasRenamedNet(CNet* connectedFromNet);

public:
	CString getAliasRename();
};

//-----------------------------------------------------------------------------
// CBus
//-----------------------------------------------------------------------------
class CBus : public CSignal
{
public:
	CBus(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name, const ESignalType signalType);
   CBus(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, CEdifView& view, const CBus& bus);
//   CBus(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CBus& bus);
	~CBus();

private:
	CTypedMapStringToPtrContainer<CNet*> m_netMap;
	CTypedMapStringToPtrContainer<CBus*> m_busMap;
	CStringArray m_enumNameArr;

	void reset();
	void enumerateBusName();

public:
	void addBus(CBus* bus);
	void addNet(CNet* net);

   CString getEnumeratedName(CString wireRule);
   void applyBusDerivedNametoConnectedNets();
   void createNetLis();
};

//-----------------------------------------------------------------------------
// CInstance
//-----------------------------------------------------------------------------
class CInstance
{
public:
	CInstance(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name);
   CInstance(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CInstance& instance);
   CInstance& operator =(const CInstance& instance);
	~CInstance();
	
private:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_viewName;
	CString m_name;
	CString m_displayName;
	CString m_primaryName;
	CString m_designator;
	CString m_libraryRef;
	CString m_cellRef;
	CString m_clusterRef;
	CString m_symbolRef;
	DataStruct* m_instanceDataStruct;
	CTypedMapStringToPtrContainer<CInstancePort*> m_instancePortMap; // Key is port ref
   CAttributes* m_attribMap;									// a map of and instance's attributes

	void reset();

public:
   CAttributes*& getAttributesRef()							{ return m_attribMap; }

   CMapStringToPtr   portAttribMap;							// a map of ports' attribute map which contains overwriting value only
																		// the key is the port ref
   CMapStringToPtr   portAttribDisplayMap;				// a map of ports' attribute map which contains overwriting location only
																		// the key is the port implementation ref

	CString getName() const										{ return m_name;								}

   CString getParentCellName()                        { return m_cellName;                   }  // parent in tree, owner in hierarchy
   CString getParentLibraryName()                     { return m_libraryName;                }  // ditto
   CString getParentClusterName()                     { return m_clusterName;                }  // ditto
	
	CString getDisplayName() const							{ return m_displayName;						}
	void setDisplayName(const CString displayName)		{ m_displayName = displayName;			}

	CString getPrimaryName() const							{ return m_primaryName;						}
	void setPrimaryName(const CString primaryName)		{ m_primaryName = primaryName;			}

	CString getDesignator() const								{ return m_designator;						}
	void setDesignator(const CString designator)			{ m_designator = designator;				}

	CString getLibraryRef() const								{ return m_libraryRef;						}
	void setLibraryRef(const CString libraryRef)			{ m_libraryRef = libraryRef.IsEmpty()?m_libraryName:libraryRef;	}

	CString getCellRef() const									{ return m_cellRef;							}
	void setCellRef(const CString cellRef)					{ m_cellRef = cellRef;						}

	CString getClusterRef() const								{ return m_clusterRef;						}
	void setClusterRef(const CString clusterRef)			{ m_clusterRef = clusterRef;				}

	CString getSymbolRef() const								{ return m_symbolRef;						}
	void setSymbolRef(const CString symbolRef)			{ m_symbolRef = symbolRef;					}

	CString getSymbolBlockName() const						
	{
		CString blockName;
		blockName.Format("%s%c%s%c%s%c%s", m_libraryRef, SCH_DELIMITER, m_cellRef, SCH_DELIMITER, m_clusterRef, SCH_DELIMITER, m_symbolRef);
		blockName.MakeUpper();
		return blockName;
	}

	DataStruct* getInstanceDataStruct()						{ return m_instanceDataStruct;			}
	void setInstanceDataStruct(DataStruct* instance)	{ m_instanceDataStruct = instance;		}

	CInstancePort* addInstancePort(CString portRef);
	CInstancePort* findInstancePort(CString portRef);
   POSITION getInstancePortStartPosition() const      { return m_instancePortMap.GetStartPosition(); }
   CInstancePort* getNextInstancePort(POSITION& pos)
   {
      if (pos == NULL)
         return NULL;

      CInstancePort* instancePort = NULL;
      CString key;
      m_instancePortMap.GetNextAssoc(pos, key, instancePort);
      return instancePort;
   }

   bool isHierarchSymbol();
};

//-----------------------------------------------------------------------------
// CEdifView
//-----------------------------------------------------------------------------
class CEdifView
{
public:
	CEdifView(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name);
   CEdifView(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CEdifView& view);
   CEdifView& operator =(const CEdifView& view);
	~CEdifView();

private:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_name;
   CTypedPtrListContainer<BlockStruct*> m_pageMap;
	CTypedMapStringToPtrContainer<CInstance*> m_instanceMap;
	CTypedMapStringToPtrContainer<CPort*> m_globalPortMap;
	CTypedMapStringToPtrContainer<CPort*> m_masterPortMap;
	CTypedMapStringToPtrContainer<CPageConnector*> m_onPageConnectorMap;
	CTypedMapStringToPtrContainer<CPageConnector*> m_offPageConnectorMap;
	CTypedMapStringToPtrContainer<CRipper*> m_ripperMap;
	CTypedMapStringToPtrContainer<CNet*> m_netMap;
	CTypedMapStringToPtrContainer<CBus*> m_busMap;
   BlockStruct* m_firstSchematicPageBlock;
	BlockStruct* m_currentPageBlock;
	CNet* m_currentNet;
	CBus* m_currentBus;
	CString m_currentPageName;

	void reset();
	void prepareBusAndNetForRename();
	void applyGlobalPortNameToBussesAndNets();
	void applyMasterPortNameToBussesAndNets(CInstance* instance);
	void applyDoneToBussesAndNetsOnPageConnector();
	void applyBussesDerivedNameToNets();
	void applyNetAliasNameToNets();
	void applyUniqueBusNameToLocalBusses();
	void applyNetsOnInstanceToHierarchyStructure();
	void createNetListFromRenamedNetsAndBusses();
	void applyRenamedNetAndBusNameToEtch();

public:
	CString getName() const									{ return m_name;							}

	// page functions
   CString getUniquePageName(const CString pageName);
	void addPage(BlockStruct* pageBlock);
	BlockStruct* getFirstPage();
	POSITION getPageStartPosition();
	BlockStruct* getNextPage(POSITION& pos);
	BlockStruct* getCurrentPage()							{ return m_currentPageBlock;			}
	void setCurrentPage(BlockStruct* pageBlock, const CString pageName)		
	{ 
		m_currentPageBlock = pageBlock; 	
		if (pageBlock == NULL)
			m_currentPageName.Empty();
      else
	      m_currentPageName = pageName;
	}
	CString generatePageBlockName(const CString pageName);

   void setFirstSchematicPage(BlockStruct* pageBlock)
   {
      if (m_firstSchematicPageBlock == NULL)
         m_firstSchematicPageBlock = pageBlock;
   }
   BlockStruct* getFirstSchematicPageBlock()       { return m_firstSchematicPageBlock;   }

	// instance functions
	CInstance* addInstance(const CString instanceName);
	CInstance* findInstance(const CString instanceName);
   POSITION getInstanceStartPosition()             { return m_instanceMap.GetStartPosition(); }
   CInstance* getNextInstance(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CInstance* instance = NULL;
      CString key;
      m_instanceMap.GetNextAssoc(pos, key, instance);
      return instance;
   }

	// globalPort functions
	CPort* addGlobalPort(const CString portInstanceName);
	CPort* findGlobalPort(const CString portInstanceName);
   CPort* findGlobalPortByPortName(const CString portName);
   POSITION getGlobalPortStartPosition()            { return m_globalPortMap.GetStartPosition(); }
   CPort* getNextGlobalPort(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CPort* port = NULL;
      CString key;
      m_globalPortMap.GetNextAssoc(pos, key, port);
      return port;
   }

	// masterPort functions
	CPort* addMasterPort(const CString portInstanceName);
	CPort* findMasterPort(const CString portInstanceName);
   CPort* findMasterPortByPortName(const CString portName);
   POSITION getMasterPortStartPosition()            { return m_masterPortMap.GetStartPosition(); }
   CPort* getNextMasterPort(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CPort* port = NULL;
      CString key;
      m_masterPortMap.GetNextAssoc(pos, key, port);
      return port;
   }

	// onPageConnector functions
	CPageConnector* addOnPageConnector(const CString connectorName);
	CPageConnector* findOnPageConnector(const CString connectorName);

	// offPageConnector functions
	CPageConnector* addOffPageConnector(const CString connectorName);
	CPageConnector* findOffPageConnector(const CString connectorName);

	// ripper functions
	CRipper* addRipper(const CString ripperName);
	CRipper* findRipper(const CString ripperName);
   void applyRipperRulesToHotspot();

	// net functions
	CNet* addNet(const CString netName);
	CNet* findNet(const CString netName);
   POSITION getNetStartPosition()            { return m_netMap.GetStartPosition(); }
   CNet* getNextNet(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CNet* net = NULL;
      CString key;
      m_netMap.GetNextAssoc(pos, key, net);
      return net;
   }

	CNet* getCurrentNet()									{ return m_currentNet;					}
	void setCurrentNet(CNet* net)							{ m_currentNet = net;					}

	// bus functions
	CBus* addBus(const CString busName);
	CBus* findBus(const CString busName);
   POSITION getBusStartPosition()            { return m_busMap.GetStartPosition(); }
   CBus* getNextBus(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CBus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      return bus;
   }

	CBus* getCurrentBus()									{ return m_currentBus;					}
	void setCurentBus(CBus* bus)							{ m_currentBus = bus;					}

   void convertLogicalSchematicToPhysicalSchematic();
   void removedAllPageBlocks();
   void createNetList();
	void propagateNetsFromInstanceToHierarchyStructure(CInstance* instance);
};

//-----------------------------------------------------------------------------
// CSymbolPort
//-----------------------------------------------------------------------------
class CSymbolPort
{
public:
	CSymbolPort(CEdif &edif, CString libraryName, CString cellName, CString clusterName, CString symbolName, CString name);
   CSymbolPort(const CSymbolPort& symbolPort);
	~CSymbolPort();

private:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_symbolName;
	CString m_name;			// instance name of the port
	CString m_portRef;		// reference name of local port defined at interface
	DataStruct* m_portDataStruct;

public:
	CString getPortRef()	const									{ return m_portRef;							}
	void setPortRef(const CString portRef)					{ m_portRef = portRef;						}

	DataStruct* getPortDataStruct()							{ return m_portDataStruct;					}
	void setPortDataStruct(DataStruct* portDataStruct)	{ m_portDataStruct = portDataStruct;	}
};

//-----------------------------------------------------------------------------
// CEdifSymbol
//-----------------------------------------------------------------------------
class CEdifSymbol
{
public:
	CEdifSymbol(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name);
   CEdifSymbol(const CEdifSymbol& symbol);
   CEdifSymbol& operator =(const CEdifSymbol& symbol);
	~CEdifSymbol();

private:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_name;
	BlockStruct* m_symbolBlock;
	CTypedMapStringToPtrContainer<CSymbolPort*> m_portMap;

	void reset();

public:
	CString getLibraryName() const							{ return m_libraryName;						}
	CString getCellName() const								{ return m_cellName;							}
	CString getClusterName() const							{ return m_clusterName;						}
	CString getName() const										{ return m_name;								}

	BlockStruct* getSymbolBlock()								{ return m_symbolBlock;				      }
	void setSymbolBlock(BlockStruct* symbolBlock)	{ m_symbolBlock = symbolBlock;	}

	CSymbolPort* addPort(CString portInstanceName);
	CSymbolPort* findPort(CString portInstancetName);
   POSITION getSymbolPortStartPosition() const      { return m_portMap.GetStartPosition(); }
   CSymbolPort* getNextSymbolPort(POSITION& pos)
   {
      if (pos == NULL)
         return NULL;

      CSymbolPort* symbolPort = NULL;
      CString key;
      m_portMap.GetNextAssoc(pos, key, symbolPort);
      return symbolPort;
   }

	CString generateSymbolBlockName();
};

//-----------------------------------------------------------------------------
// CConfiguration
//-----------------------------------------------------------------------------
class CConfiguration
{
public:
	CConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name);
	~CConfiguration();

protected:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_clusterName;
	CString m_name;

public:
   CString getName() const                         { return m_name;              }
};

//-----------------------------------------------------------------------------
// CInstanceConfiguration
//-----------------------------------------------------------------------------
class CInstanceConfiguration : public CConfiguration
{
public:
	CInstanceConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name);
	~CInstanceConfiguration();

private:
   CString m_clusterConfigurationRef;

public:
   CString getClusterConfigurationRef() const               { return m_clusterConfigurationRef;             }
   void setClusterConfigurationRef(const CString configurationRef)  { m_clusterConfigurationRef = configurationRef; }
};

//-----------------------------------------------------------------------------
// CClusterConfiguration
//-----------------------------------------------------------------------------
class CClusterConfiguration : public CConfiguration
{
public:
	CClusterConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name);
   CClusterConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CClusterConfiguration& configuration);
	~CClusterConfiguration();

private:
	CString m_viewRef;
	CTypedMapStringToPtrContainer<CInstanceConfiguration*> m_instanceConfigurationMap;

public:
	CString getViewRef() const								{ return m_viewRef;				}
	void setViewRef(const CString viewRef)				{ m_viewRef = viewRef;			}

	CInstanceConfiguration* addInstanceConfiguration(const CString configurationName);
	CInstanceConfiguration* findInstanceConfiguration(const CString configurationName);

   CEdifView* getView();   
};

//-----------------------------------------------------------------------------
// CEdifCluster
//-----------------------------------------------------------------------------
class CEdifCluster
{
public:
	CEdifCluster(CEdif &edif, const CString libraryName, const CString cellName, const CString name);
   CEdifCluster(CEdifCluster& cluster);
   CEdifCluster& operator =(const CEdifCluster& cluster);
	~CEdifCluster();

private:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_name;
	CTypedMapStringToPtrContainer<CEdifView*> m_viewMap;
	CTypedMapStringToPtrContainer<CEdifSymbol*> m_symbolMap;
	CTypedMapStringToPtrContainer<CClusterConfiguration*> m_clusterConfigurationMap;
	CEdifView* m_currentView;
	CEdifSymbol* m_currentSymbol;
   CClusterConfiguration* m_currentConfiguration;
   CString m_defaultConfigurationName;
	bool m_isHierarchyStructure;
   int m_cloneCount;
	int m_derivedNumber;

	void reset();
   CString getNextCloneName();

public:
   CString getName() const                         { return m_name;                 }
   int getCloneCount() const                       { return m_cloneCount;           }
	int getDerivedNumber() const							{ return m_derivedNumber;			}
   bool isHierarchStructure() const                { return m_isHierarchyStructure; }

	CEdifView* addView(const CString viewName);
	CEdifView* findView(const CString viewName);	
	CEdifView* getFirstView();
   POSITION getViewStartPosition()                 { return m_viewMap.GetStartPosition(); }
   CEdifView* getNextView(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdifView* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      return view;
   }

	CEdifView* getCurrentView()							{ return m_currentView;			}
	void setCurrentView(CEdifView* view)				{ m_currentView = view;			}

	CEdifSymbol* addSymbol(const CString symbolName);
	CEdifSymbol* findSymbol(const CString symbolName);
	CEdifSymbol* getCurrentSymbol()						{ return m_currentSymbol;		}
	void setCurrentSymbol(CEdifSymbol* symbol)		{ m_currentSymbol = symbol;	}

	CClusterConfiguration* addClusterConfiguration(const CString configurationName);
	CClusterConfiguration* findClusterConfiguration(const CString configurationName);
   CClusterConfiguration* getCurrentClusterConfiguration()
   { return m_currentConfiguration; }
   void setCurrentClusterConfiguration(CClusterConfiguration* configuration)
   { m_currentConfiguration = configuration; }

   CClusterConfiguration* getDefaultConfiguration();
   void setDefaultConfiguration(const CString configurationName) { m_defaultConfigurationName = configurationName; }

   CString getDelimitedPageBlockNumber(const CString delimiter) const;

   void attachHierarchySymbolToView();
   void convertLogicalSchematicToPhysicalSchematic();
   void removedUnsedViewBlocks();
   void createNetList();
	void propagateNetsFromInstanceToHierarchyStructure(CInstance* instance);
};

//-----------------------------------------------------------------------------
// CEdifCell
//-----------------------------------------------------------------------------
class CEdifCell
{
public:
	CEdifCell(CEdif &edif, const CString libraryName, const CString name);
	~CEdifCell();

private:
	CEdif& m_edif;
	CString m_libraryName;
	CString m_name;
	CTypedMapStringToPtrContainer<CEdifCluster*> m_clusterMap;
	CEdifCluster* m_currentCluster;

	void reset();

public:
   CString getName()          { return m_name; }
   CString getLibraryName()   { return m_libraryName; }

	CEdifCluster* addCluster(const CString clusterName);
   CEdifCluster* addCluster(CEdifCluster* cluster);
	CEdifCluster* findCluster(const CString clusterName);
	CEdifCluster* getFirstCluster();
	CEdifCluster* getCurrentCluster()					{ return m_currentCluster;			}
	void setCurrentCluster(CEdifCluster* cluster)	{ m_currentCluster = cluster;		}

	BlockStruct* getFirstPage();

   void convertLogicalSchematicToPhysicalSchematic();
   void removedUnusedOriginalCluster();
   void createNetList();
};

//-----------------------------------------------------------------------------
// CEdifLibrary
//-----------------------------------------------------------------------------
class CEdifLibrary
{
public:
	CEdifLibrary(CEdif &edif, const CString name);
	~CEdifLibrary();

private:
	CEdif& m_edif;
	CString m_name;
	CTypedMapStringToPtrContainer<CEdifCell*> m_cellMap;
	CEdifCell* m_currentCell;

	void reset();

public:
	CString getName() const									{ return m_name;					}

	CEdifCell* addCell(const CString cellName);
	CEdifCell* fineCell(const CString cellName);
	CEdifCell* getCurrentCell()							{ return m_currentCell;			}
	void setCurrentCell(CEdifCell* cell)				{ m_currentCell = cell;			}

   void convertLogicalSchematicToPhysicalSchematic();
   void removedUnusedOriginalCluster();
   void createNetList();
};

//-----------------------------------------------------------------------------
// CEdif
//-----------------------------------------------------------------------------
class CEdif
{
public:
	CEdif();
	~CEdif();

	void reset();

private:
	CTypedMapStringToPtrContainer<CEdifLibrary*> m_libraryMap;
	CEdifLibrary* m_currentLibrary;
   bool m_isSchematicConvertionDone;

	// These two maps keep track and ensure that names of local net and bus name are unique among views
	// So if the same net or bus name is found on two different view; those net will have unique name.
	// The first occurrance will retain the original name; the second one will have "_n" append to it.
	CMapStringToString m_localUniqueNetNameMap;	
	CMapStringToString m_localUniqueBusNameMap;

public:
   bool isSchematicConvertionDone() const          { return m_isSchematicConvertionDone;  }

	CEdifLibrary* addLibrary(const CString libraryName);
	CEdifLibrary* getCurrentLibrary()					{ return m_currentLibrary;			      }
	void setCurrentLibrary(CEdifLibrary* library)	{ m_currentLibrary = library;		      }

	CEdifLibrary* findLibrary(const CString libraryName);
	CEdifCell* findCell(const CString libraryName, const CString cellName);
	CEdifCluster* findCluster(const CString libraryName, const CString cellName, const CString clusterName);
	CEdifView* findView(const CString libraryName, const CString cellName, const CString clusterName, const CString viewName);
	CClusterConfiguration* findClusterConfiguration(const CString libraryName, const CString cellName, const CString clusterName, const CString configurationName);

   CEdifCell* getCurrentCell();
   CEdifCluster* getCurrentCluster();
   CEdifView* getCurrentView();
   CEdifSymbol* getCurrentSymbol();
   CNet* getCurrentNet();
   CBus* getCurrentBus();

	CString getUniqueLocalNetName(const CString netName);
	CString getUniqueLocalBusName(const CString busName);

	void createPhysicalSchematicWithLogicalConnectivity();
   void convertLogicalSchematicToPhysicalSchematic();
   void createNetList();

};

//-----------------------------------------------------------------------------
// CAttributeVisibility
//-----------------------------------------------------------------------------
class CAttributeVisibility
{
public:
   CAttributeVisibility(const CString keyword, const double height, const double width)
   {
      m_keyword = keyword;
      m_height = height;
      m_width = width;
   }
   ~CAttributeVisibility(){}

private:
   CString m_keyword;
   double m_height;
   double m_width;

public:
   CString getKeyword() const { return m_keyword; }
   double getHeight() const { return m_height; }
   double getWidth() const { return m_width; }
};

//-----------------------------------------------------------------------------
// CAttributeVisibilityMap
//-----------------------------------------------------------------------------
class CAttributeVisibilityMap
{
public:
   CAttributeVisibilityMap() { empty(); }
   ~CAttributeVisibilityMap() { empty(); }

private:
   CTypedMapStringToPtrContainer<CAttributeVisibility*>  m_attribVisibleMap;

public:
   void empty() { m_attribVisibleMap.empty(); }
   void addAttributeVisibility(const CString keyword, const double height, const double width);
   void applyAttributeVisibility(CCEtoODBDoc& doc);
};

#endif
