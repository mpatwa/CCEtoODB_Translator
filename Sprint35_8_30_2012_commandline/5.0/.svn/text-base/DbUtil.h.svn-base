// $Header: /CAMCAD/5.0/DbUtil.h 163   5/29/07 6:13p Lynn Phung $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

/*****************************************************************************/
/*
   How to build a PCB Layers

  Every electrical layer must get

  LAYATT_SIGNAL to indicate if this is a signal layer or
  LAYATT_POWRNEG, LAYATT_POWERPOS or LAYATT_SPLITPLANE

  also
   LAYATT_SIGNAL_TOP or LAYATT_SIGNAL_BOT or LAYATT_SIGNAL_INNER

  also
   LAYATT_SIGNALSTACK    this is followed by a integer which
                         starts from top(1) to bottom(n)

  also if avail
   LAYATT_ARTWORKSTACK   param : string with layername
                         this is the artwork stackup, which
                         might be 1 silkscreen top
                                  2 comp
                                  3 solder
                                  4 paste bottom

   Single shape and Multi shape padshape definitions:
   Some sytems allow multishapes, such as Mentor...
   other systems only allow single shape padstack.

*/

/*****************************************************************************/
/*
   How to build a Plane net

   Netattribute ATT_POWERNET
   Layerattribute ATT_NETNAME, multiple netnames are allowed.
   Layer type must be a LAYTYPE_SPLITPLANE or LAY_POWERNEG

*/

/*****************************************************************************/
/*
   How to build a PCB Component

   Graph_Block_On    PCBCOMPONENT
   {
      .. can have hierachical graphic
      .. Graph_Circle
      .. other graphic
      .. needs primary SILKSCREEN_OUTLINE

      Graph_Block_Reference   INSERTTYPE_PIN pinname (as it appears in the netlist) is the refname.
      need a pinnr attribute
   }

   Graph_Block_On    PADSTACK -> this has an toolindex -> to drill
   {
      Graph_Block_Reference      PADSHAPE for complex  or simple Aperture defintion
      Attributes ...
   }

   Graph_Block_On    PADSHAPE
   {
      should be aperture definitions
   }

   Drill Holes:
   ============
   A drill hole is defined with a Graph_Tool() and then inserted with graph_blockref()
   and the insert type INSERTTYPE_DRILLHOLE

   Test_Probe:
   ===========

   a probe must have a datalink to a feature (compPin or CC Entity).
   the accesspoint is not of interest after a probe is placed.

*/

/****************************************************************************/
/*
   GENERAL DEFFITINION of PCB data


   Definition of a PCB_PIN
   =======================

   graph_block = BLOCKTYPE_PADSTACK
                 // BLOCKTYPE_PADSTACKGRAPHIC does not exist anymore and use PADSHAPE instead

   Definition of a PCB_COMPONENT
   =============================
   graph_block = BLOCKTYPE_PCBCOMPONENT      electrical component
                 BLOCKTYPE_MECHCOMPONENT     mechanical component
                 BLOCKTYPE_GENERICCOMPONENT  graphic component
                 BLOCKTYPE_FIDUCIAL          fiducial component
                 BLOCKTYPE_TOOLING           tooling component
   attrib ATT_REFNAME

   pins are defined
      graph_block_ref = INSERTTYPE_PIN
      attrib          = ATT_PINNR
      ther pinname used in netlist is in the refname of the insert definition

   Definition of Netgraphic
   ========================
   graphic_type  = GR_CLASS_ETCH

   Centroids
      centroid is a geometry defined in PCBUTIL.cpp. It only is defined once.
      every reader needs to make sure to test if defined before placing.
      the placement then is  done as a reference with x,y and rotation.

   DRCMarker
      centroid is a geometry defined in PCBUTIL.cpp. It only is defined once.
      every reader needs to make sure to test if defined before placing.
      the placement then is  done as a reference with x,y and a layer. The layer is a must,
      because the drc definition has not a layer defintion.

   TEST_POINT is defined like a component, with pins and netlist. Testpoints do not need a TYPELIST entry.

   TEST_PROBES are now a subset of TESTPOINTS, selected in the TEST_PROBE ASSIGNMENT function in TA.
   INSERTTYPE_TEST_PROBE are final assigned points for a probe to touch.

   We also need a INSERTTYPE_TEST_NAIL for a nail location in a bed-of-nail fixture.

*/

/****************************************************************************/
/*
   Complex padstacks

   A Padstacks
   block_on
      Drill
      (drill) multiple allowed
      Aperture -- only one per layer
      Complex  -- only one per layer
         aperture -- must be floating layer
         (aperture) -- multiple allowed
         graphic  -- must be floating layer
         (graphic)   -- multiple allowed

      Attributes....

   block_off

*/

/****************************************************************************/
/*
   Polystruct
   A polystruct should contain:
   1. only one and first the parent polygon (close)
   2. child voids
   3. possible thermal segments

*/

#if !defined(__DbUtil_h__)
#define __DbUtil_h__

#pragma once

#include "Dca.h"
#include "DcaApertureShape.h"
#include "DcaBlockType.h"
#include "DcaInsertType.h"
#include "DcaGraphicClass.h"
#include "DcaFileType.h"
#include "DcaLayerType.h"
#include "DcaLayer.h"
#include "DcaAttributeValues.h"
#include "DcaMask64.h"
#include "DcaMaskBool.h"
//#include "DcaBlock.h"  // for block flag definitions

/*****************************************************************************/
/*
*/

// For company name in registry tree
#define  REGISTRY_COMPANY           "Mentor Graphics"

/*
*/
/*****************************************************************************/


#define MAX_HPPENS                  256      // maximum 256 logical pen according to files (32 according to HP Reference Guide)
#define MAXDCODE                    4
#define MAXPREFIX                   8
#define MAXAPNAME                   MAXPREFIX+MAXDCODE

// Insert Mirror definition
// graphically flip (mirror & MIRROR_FLIP)   // flips X coordinates - does not affect color and visibility.
// place on bottom (mirror & MIRROR_LAYERS)  // does affect mirror layers for color and visibility,

// attribute flags
#define  ATTRIB_NO_INHERETANCE      0x00000001  // do not inheret this attribute
#define  ATTRIB_TMP_FLAG            0x00000002  // this is for temporaty use only.
// these following three flags are used by Edif300_in.cpp
#define  ATTRIB_CHANGE_LOCATION     0x00000004  // indicate only x, y, rotation, angle, and visibility have changed
#define  ATTRIB_CHANGE_VALUE        0x00000008  // indicate only value has changed
#define  ATTRIB_CHANGE_VISIBLE      0x00000010  // indicate only visibility has changed

// insert flags
#define  INSERT_PINLOC              0x00000001  // generate_pinloc was done

// netlist flags
#define  NETLIST_CHECKED            0x00000001  // netlist was checked that all pins exist in data.

// special character defintions
// underbar is %U
// overbar  is %O
// percent  is %%
#define  CHAR_UMLAUT_A              ((unsigned char)196)
#define  CHAR_UMLAUT_O              ((unsigned char)214)
#define  CHAR_UMLAUT_U              ((unsigned char)220)
#define  CHAR_UMLAUT_a              ((unsigned char)226)
#define  CHAR_UMLAUT_o              ((unsigned char)246)
#define  CHAR_UMLAUT_u              ((unsigned char)252)

#define  CHAR_PLUSMINUS             ((unsigned char)177)
#define  CHAR_DEGREE                ((unsigned char)176)
#define  CHAR_DIAMETER              ((unsigned char)248)

//
// index into cadsystems see dbutil.cpp -- do not change sequence CCLaunch is dependend
// on these nummbers
//
/// THESE ARE OFFICIALLY OBSOLETE, REPLACED WITH FileTypeTag in DcaFileType.h.
/// However, the change over is incomplete, there are still some references.
/// Eventually we will go through the code and convert all those usages to the equivalent FileTypeTag.
/// Nothing new should be added here, new stuff goes in DcaFileType.h.

#define Type_Unknown                 0

#define Type_HPGL                    2
#define Type_Gerber                  3

#define Type_Mentor_Layout          12

#define Type_EDIF                   18
#define Type_VB99_Layout            19
#define Type_CADENCE_Plot           20
#define Type_GENCAD                 21
#define Type_Barco                  22
#define Type_ORCAD_LTD_Layout       23
#define Type_CASE_Plot              24
#define Type_Allegro_Layout         25
#define Type_Mentor_Neutral_Layout  26
#define Type_BOM                    27
#define Type_HP3070                 28
#define Type_CADIF_Layout           29
#define Type_ACCEL_Layout           30
#define Type_EIF_Layout             31
#define Type_QUAD_Layout            32
#define Type_HYPERLYNX_Layout       33
#define Type_CCM_Layout             34
#define Type_THEDA_Layout           35
#define Type_HPEGS                  36
#define Type_Autocad_ADI            37
#define Type_CII_Layout             38

#define Type_CR3000_Layout          41
#define Type_CCT_Layout             42

#define Type_HP5DX                  44
#define Type_GENCAM                 45

#define Type_ODB_PP                 48 // directory structure
#define Type_DMIS                   49
#define Type_HUGHES                 50
#define Type_VIEWDRAW               51
#define Type_FABMASTER              52

#define Type_TERADYNE_7200          55

#define Type_DDE_Layout             57



// Aperture Shape
#define T_UNDEFINED                 0
#define T_ROUND                     1
#define T_SQUARE                    2
#define T_RECTANGLE                 3
#define T_TARGET                    4
#define T_THERMAL                   5
#define T_COMPLEX                   6
#define T_DONUT                     7
#define T_OCTAGON                   8
#define T_OBLONG                    9
#define T_BLANK                     10
#define T_ROUNDRECT                 11

#define MAX_SHAPES                  12    // Number of different aperture shapes

////
////////////////////////////////////////////////////////////////////////////////////////////////
//// OBSOLETE - DEPRECATED
//// These GR_CLASS symbols are being replaced by an enumeration, see DcaGraphicClass.h.
//// Code is being updated and these symbols deleted, as we find time to get to them.
//// No new code should be written using these symbols, new code should use only GraphicClassTag enum values.
//// New graphic classes are to be added to DcaGraphicClass.h/cpp, not here.
////
//// When one of these symbols has been completely replaced it is to be commented out
//// using the comment //OBSOLETE.
//// Once all of them are obsoleted then this entire section will be deleted.
////

// GR_CLASS_... max 127 classes because 6 bits in DataStruct
// GR_ graphic_class are only used for Graphic elements, Lines, Arcs, Circle, Text, Polygon
// class for Graphic Entities - special treatment of entities - CAD system dependend.
#define GR_CLASS_NORMAL             0 // default init.
//OBSOLETE   #define GR_CLASS_ANALYSIS           1
#define GR_CLASS_ANTI_ETCH          2
#define GR_CLASS_BOARD_GEOM         3  // must be used to get a graphic element into the board outline section
#define GR_CLASS_DRAWING            4
//OBSOLETE  #define GR_CLASS_DRC                5
#define GR_CLASS_ETCH               6  // must be used to get a graphic element into a netlist
                                       // a line can have a netname, but not _ETCH. This means a line is a
                                       // copper line with a netname, but not part of a route (i.e. PADS, MENTOR)
//OBSOLETE  #define GR_CLASS_MANUFACTURING      7
#define GR_CLASS_ROUTKEEPIN         8
#define GR_CLASS_ROUTKEEPOUT        9
#define GR_CLASS_PLACEKEEPIN        10
#define GR_CLASS_PLACEKEEPOUT       11 //
#define GR_CLASS_VIAKEEPIN          12
#define GR_CLASS_VIAKEEPOUT         13
#define GR_CLASS_ALLKEEPIN          14
#define GR_CLASS_ALLKEEPOUT         15
#define GR_CLASS_BOARDOUTLINE       16 // this is a primary, good boardoutline
#define GR_CLASS_COMPOUTLINE        17 // this is a primary good component silkscreen outline
#define GR_CLASS_PANELOUTLINE       18 // this is a primary good paneloutline
#define GR_CLASS_MILLINGPATH        19 // for excellon milling output
#define GR_CLASS_UNROUTE            20 // for rubberbands, open connections.
#define GR_CLASS_TRACEFANOUT        21 // this are graphic elements, used for SMD fanout
//OBSOLETE  #define GR_CLASS_PANELCUTOUT        22 // this are graphic elements
//OBSOLETE  #define GR_CLASS_BOARDCUTOUT        23 // this are graphic elements
//OBSOLETE  #define GR_CLASS_TESTPOINTKEEPOUT   24
//OBSOLETE  #define GR_CLASS_TESTPOINTKEEPIN    25
#define GR_CLASS_COMPONENT_BOUNDARY 26 // this must be a closed polyline.
//OBSOLETE  #define GR_CLASS_PLACEMENT_REGION   27 // this must be a closed polyline.
#define GR_CLASS_PACKAGEOUTLINE     28 // this is the outline, which DFT uses
#define GR_CLASS_PACKAGEBODY        29 // Added 07/25/03, Used for new package information from RealPart
#define GR_CLASS_PACKAGEPIN         30 // Added 07/25/03, Used for new package information from RealPart
//OBSOLETE  #define GR_CLASS_SIGNAL             31 // Added 07/19/04, used for Schematic file
//OBSOLETE  #define GR_CLASS_BUS                32 // Added 07/19/04, used for Schematic file
#define GR_CLASS_PANELBORDER        33 // Part of fix for dts0100505067
#define GR_CLASS_MANUFACTURINGOUTL  34 // Part of fix for dts0100505065
#define GR_CLASS_CONTOUR            35 // dts0100457517
#define GR_CLASS_PLANEKEEPOUT       36 // dts0100552575
#define GR_CLASS_PLANENOCONNECT     37 // dts0100552575
#define GR_CLASS_BOARDKEEPOUT       38 // dts0100552575
#define GR_CLASS_PADKEEPOUT         39 // dts0100552575
#define GR_CLASS_EPBODY             40 // dts0100552575
#define GR_CLASS_EPMASK             41 // dts0100552575
#define GR_CLASS_EPOVERGLAZE        42 // dts0100552575

#define MAX_CLASSTYPE               43 // start with 0, so always one more than highest gr class value

////
////  End of deprecated GR_CLASS section
////
//////////////////////////////////////////////////////////////////////////////////
////


// Define the different ETCH types
#define ETCH_TYPE_TRACE          "TRACE"
#define ETCH_TYPE_CONDUCTIVE     "CONDUCTIVE"
#define ETCH_TYPE_PLANE          "PLANE"
#define ETCH_TYPE_PLANE_HOLE     "PLANE_HOLE"
#define ETCH_TYPE_PLANE_OUTLINE  "PLANE_OUTLINE"

// this are the attribute names for DEVICETYPE attribute as listed in devicetypearray
// (dbutil.cpp) attached to an typelist (preffered) or componentinsert
// the DEVICETYPE is used in Electrical testing etc...
#define ATT_DEVICETYPE_UNKNOWN               0
#define ATT_DEVICETYPE_AMPLIFIER             1
#define ATT_DEVICETYPE_BATTERY               2
#define ATT_DEVICETYPE_CAPACITOR             3
#define ATT_DEVICETYPE_CAPACITOR_POLARIZED   4
#define ATT_DEVICETYPE_COIL                  5
#define ATT_DEVICETYPE_CONNECTOR             6
#define ATT_DEVICETYPE_CRYSTAL               7
#define ATT_DEVICETYPE_DIODE                 8
#define ATT_DEVICETYPE_DIODE_SCHOTTKY        9
#define ATT_DEVICETYPE_DIODE_ZENER           10
#define ATT_DEVICETYPE_FERRITE_BEAD          11
#define ATT_DEVICETYPE_FET                   12
#define ATT_DEVICETYPE_FILTER                13
#define ATT_DEVICETYPE_FREEPAD               14
#define ATT_DEVICETYPE_FUSE                  15
#define ATT_DEVICETYPE_IC                    16
#define ATT_DEVICETYPE_IC_ANALOG             17
#define ATT_DEVICETYPE_IC_DIGITAL            18
#define ATT_DEVICETYPE_INDUCTOR              19
#define ATT_DEVICETYPE_JUMPER                20
#define ATT_DEVICETYPE_LED                   21
#define ATT_DEVICETYPE_MECHANICAL            22
#define ATT_DEVICETYPE_PADMODULE             23
#define ATT_DEVICETYPE_POTENTIOMETER         24
#define ATT_DEVICETYPE_POWER_SUPPLY          25
#define ATT_DEVICETYPE_RELAY                 26
#define ATT_DEVICETYPE_RESISTOR              27
#define ATT_DEVICETYPE_RESISTOR_PASTE        28
#define ATT_DEVICETYPE_SCR                   29
#define ATT_DEVICETYPE_SPEAKER               30
#define ATT_DEVICETYPE_SWITCH                31
#define ATT_DEVICETYPE_TESTPOINT             32
#define ATT_DEVICETYPE_TRANSFORMER           33
#define ATT_DEVICETYPE_TRANSISTOR            34
#define ATT_DEVICETYPE_TRANSISTOR_NPN        35
#define ATT_DEVICETYPE_TRANSISTOR_PNP        36
#define ATT_DEVICETYPE_VARISTOR              37
#define ATT_DEVICETYPE_VOLTAGE_REGULATOR     38

#define MAX_DEVICETYPE                       38


// update in dbutil.cc *device_subclass[]
#define ATT_DEVICE_SUBCLASS_NON_FUNCTIONAL_PINS    0
#define ATT_DEVICE_SUBCLASS_TREAT_AS_TWO_PIN       1
#define ATT_DEVICE_SUBCLASS_SIP_ISOLATED           2
#define ATT_DEVICE_SUBCLASS_SIP_BUSSED             3
#define ATT_DEVICE_SUBCLASS_DIP_ISOLATED           4
#define ATT_DEVICE_SUBCLASS_DIP_BUSSED             5
#define ATT_DEVICE_SUBCLASS_POT_1                  6
#define ATT_DEVICE_SUBCLASS_POT_2                  7
#define ATT_DEVICE_SUBCLASS_POT_3                  8
#define ATT_DEVICE_SUBCLASS_VARIABLE_CAPACITOR_1   9
#define ATT_DEVICE_SUBCLASS_VARIABLE_CAPACITOR_2   10
#define ATT_DEVICE_SUBCLASS_VARIABLE_CAPACITOR_3   11
#define ATT_DEVICE_SUBCLASS_SOT23_COMMON_ANODE     12
#define ATT_DEVICE_SUBCLASS_SOT23_COMMON_CATHODE   13
#define ATT_DEVICE_SUBCLASS_SOT23_SINGLE_DIODE     14
#define ATT_DEVICE_SUBCLASS_DUAL_DIODE             15
#define ATT_DEVICE_SUBCLASS_QUAD_BRIDGE_RECTIFIER  16
#define ATT_DEVICE_SUBCLASS_SMT_COMMON             17
#define ATT_DEVICE_SUBCLASS_SMT_ISOLATED           18
#define ATT_DEVICE_SUBCLASS_DIP_OSCILLATOR_4       19
#define ATT_DEVICE_SUBCLASS_DIP_OSCILLATOR_8       20
#define ATT_DEVICE_SUBCLASS_DIP_OSCILLATOR_14      21
#define ATT_DEVICE_SUBCLASS_TRANSISTOR_ARRAY       22

#define MAX_DEVICE_SUBCLASS                        22


// FLAG Definitions for data sequencing
#define DATA_SEQ_HEAD                  0x00000010
#define DATA_SEQ_CONT                  0x00000020

// FLAG Defintions for a PCB_FILE BLock
#define FILE_PCB_HAS_NONCONNNET        0x00000001 // file has all non connected pins in a net.
#define FILE_PCB_TRACE_CENTER          0x00000002 // all traces end in centers and are cracked.

// a complex padstack is build
// the insr aperture should always be the connect point of the padstack. There should not be more than
// one aperture per layer, all other elements should be placed as a padshape or polylines. The connect point
// must always be an aperture, including a NULL aperture. The BL_ACCESS_ flag should be set from the reader.
// padstack
//    ----> INSR hole
//    ----> INSR Aperture Top
//    ----> INSR Aperture bottom
//    ----> polylines, copper.... or INSR PADSHAPE block

//_____________________________________________________________________________
// this are block types to make blocks intelligent
// if new blocks are added, it needs to be also added in dbutil.cpp
#define BLOCKTYPE_GEOMETRY_EDIT     -1 // File only used for Geometry Edit

#define BLOCKTYPE_UNKNOWN           0  // these are blocktype markers, needed to add
#define BLOCKTYPE_PCB               1  // intelligence to a block

#define BLOCKTYPE_PADSHAPE          2  // Graphic, which is used to build a PADSTACK

                                       // padshape graphic can be used instead of a complex aperture. All this graphic belongs to a padstack.
                                       // PADSTACKGRAPHIC will be elimitanted and PADSHAPE used instead.
//#define BLOCKTYPE_PADSTACKGRAPHIC 3  // Padstack, which has graphic following

#define BLOCKTYPE_PADSTACK          4  // Padstack and Viastack, which has PADSHAPE graphic
#define BLOCKTYPE_PCBCOMPONENT      5  // electrical component
#define BLOCKTYPE_MECHCOMPONENT     6  // mechanical component
#define BLOCKTYPE_GENERICCOMPONENT  7  // graphic component
#define BLOCKTYPE_PANEL             8  // this is like a new sheet
#define BLOCKTYPE_DRAWING           9  // this is like a new sheet
#define BLOCKTYPE_FIDUCIAL          10
#define BLOCKTYPE_TOOLING           11 // this is a drill only, can have attributes and a toolgraphic
                                       // assigned.

#define BLOCKTYPE_TESTPOINT         12 // this must be defined like a PCBCOMPONENT with Padstack underneath.

#define BLOCKTYPE_DIMENSION         13 // dimensions are grouped into blocks.
#define BLOCKTYPE_LIBRARY           14 // this fileblock is a library definition, this allows
                                       // multiple librarys with same name-definitions.
#define BLOCKTYPE_LOCALPCBCOMP      15 // this fileblock is a library definition copy
#define BLOCKTYPE_TOOLGRAPHIC       16 // Graphic, which is used to show a drill
#define BLOCKTYPE_SHEET             17 // Schematic Page - Changed from BLOCKTYPE_SCHEMATICPAGE to BLOCKTYPE_SHEET on 05/02/03
#define BLOCKTYPE_SYMBOL            18 // Schematic Symbol - Changed from BLOCKTYPE_GATE to BLOCKTYPE_SYMBOL on 05/02/03
#define BLOCKTYPE_GATEPORT          19 // Schematic Pin (Port) on Gate
#define BLOCKTYPE_DRILLHOLE         20 // Free standing drill hole, this is like a padstack, but just a drill
#define BLOCKTYPE_REDLINE           21 // Redline File

#define BLOCKTYPE_TEST_PROBE        22
#define BLOCKTYPE_CENTROID          23
#define BLOCKTYPE_DRCMARKER         24

#define BLOCKTYPE_GEOM_LIBRARY      25

#define BLOCKTYPE_TESTPAD           26 // this like a padstack
#define BLOCKTYPE_TEST_ACCESSPOINT  27
#define BLOCKTYPE_SCHEM_JUNCTION    28
#define BLOCKTYPE_GLUEPOINT         29

#define BLOCKTYPE_REJECTMARK        30

#define BLOCKTYPE_XOUT              31

#define BLOCKTYPE_REALPART          32
#define BLOCKTYPE_PACKAGE           33
#define BLOCKTYPE_PACKAGEPIN        34
#define BLOCKTYPE_COMPLEXDRILLHOLE  35
#define BLOCKTYPE_COMPOSITECOMP     36

#define MAX_BLOCKTYPE               37 // start with 0

CString blockTypeToString(int blockType);
BlockTypeTag intToBlockTypeTag(int blockType);

//_____________________________________________________________________________
#define INSERTTYPE_UNKNOWN                0  // this is a graphic, non intellignet insert
#define INSERTTYPE_VIA                    1  // this insert is a via, if defined in a Geom, it is a FANOUT VIA
#define INSERTTYPE_PIN                    2  // the insert.refname must be the pinname
#define INSERTTYPE_PCBCOMPONENT           3  // the insert.refname must be the component name.
#define INSERTTYPE_MECHCOMPONENT          4  // should appear in PCB BOM's, CCM but is not electrical.
#define INSERTTYPE_GENERICCOMPONENT       5  // this is a format, graphic or other.
#define INSERTTYPE_PCB                    6
#define INSERTTYPE_FIDUCIAL               7
#define INSERTTYPE_TOOLING                8  // this is a special drill tool

// testpoint is the same like a testpad, excepct a testpoint looks like a component and a
// testpad looks like a via. Accesspoints and Testprobes are likely to be placed on Testpoints/TestPads.
#define INSERTTYPE_TESTPOINT              9  // testpoint is defined like a PCBCOMPONENT with netlist etc..

#define INSERTTYPE_FREEPAD                10
#define INSERTTYPE_SYMBOL                 11 // Logic Symbol - Changed from INSERTTYPE_GATE to INSERTTYPE_SYMBOL on 05/02/03
#define INSERTTYPE_PORTINSTANCE           12
#define INSERTTYPE_DRILLHOLE              13 // free standing drill hole defintions placement. (like a FREEPAD)

#define INSERTTYPE_MECHANICALPIN          14 // pin , but not electrical (i.e. mouting hole on a connector)

#define INSERTTYPE_TEST_PROBE             15 // testprobe is the external physical probe placement.
                                             // a probe must have a datalink to a feature (compPin or CC Entity).
                                             // the accesspoint is not of interest after a probe is placed.
                                             // -> netname attribute
                                             // DDLINK <- to know from which access_point it came from.

#define INSERTTYPE_DRILLSYMBOL            16

#define INSERTTYPE_CENTROID               17 // 
#define INSERTTYPE_CLEARPAD               18 // this is the plane pad, used for plane-clear.
#define INSERTTYPE_RELIEFPAD              19 // this is the thermal pad used to build padstacks
#define INSERTTYPE_OBSTACLE               20 // generic routing obstacle, the graphic_class will determin what type of
                                             // obstacle this is.

#define INSERTTYPE_DRCMARKER              21

#define INSERTTYPE_TEST_ACCESSPOINT       22 // generated by Test Access Analysis - this is a point which has
                                             // -> netname attribute
                                             // -> TESTPREFERENCE attribute
                                             // DDLINK <- to know where it came from. (comppin or entity)
#define INSERTTYPE_TESTPAD                23 // testpoint is defined like a VIA with ATT_NETNAME
#define INSERTTYPE_SCHEM_JUNCTION         24 // junction point in schematic
#define INSERTTYPE_GLUEPOINT              25 //

#define INSERTTYPE_REJECTMARK             26

#define INSERTTYPE_XOUT                   27

#define INSERTTYPE_HIERARCHYSYMBOL        28 // Schematic symbol that points to an hierarchical sheet
#define INSERTTYPE_SHEETCONNECTOR         29
#define INSERTTYPE_TIEDOT                 30
#define INSERTTYPE_RIPPER                 31
#define INSERTTYPE_GROUND                 32
#define INSERTTYPE_TERMINATOR             33

#define INSERTTYPE_APERTURE               34

#define INSERTTYPE_REALPART               35
#define INSERTTYPE_PAD                    36
#define INSERTTYPE_PACKAGE                37
#define INSERTTYPE_PACKAGEPIN             38
#define INSERTTYPE_STENCILHOLE            39
#define INSERTTYPE_COMPLEXEDRILLHOLE      40
#define INSERTTYPE_COMPOSITECOMP          41
#define INSERTTYPE_ROUTETARGET            42
#define MAX_INSERTTYPE                    (insertTypeUpperBound + 1) // start with 0

InsertTypeTag insertTypeTag(int insertType); 
InsertTypeTag insertTypeTag(short insertType); 
CString insertTypeToString(int insertType);

//_____________________________________________________________________________
#define UNIT_INCHES                 0
#define UNIT_MILS                   1
#define UNIT_MM                     2
#define UNIT_HPPLOTTER              3
#define UNIT_TNMETER                4  // 10 nanometers this is used by Mentor, Redac etc...
#define UNIT_MICRONS                5
#define UNIT_NANOMETERS             6
#define UNIT_CENTIMETERS            7
#define MAX_UNITS                   8  // Centimeters

#define DRAW_BREAK                  0
#define DRAW_OK                     1

#define NO_EXPLODE                  1
#define EXPLODE_ROOT                2
#define EXPLODE_ALL                 3


// Attribute Keywords
// If an internal attrinute is added, it also needs to be added in
// attrib.cpp : RegisterInternalKeywords
//

#define  ATT_DDLINK                 "DATALINK"     // link to a CC entity number
#define  ATT_DERIVED                "DERIVED_FROM" // general use, if CC copies information

#define ATT_DPMO                    "DPMO"         // Defects Per Million Opportunites, relevant to Testability Report

#define  ATT_ALTERNATESHAPE         "ALTERNATE_SHAPE" // some typelists can have alternate shapes.
                                                   // this is not allowed in CAMCAD - only for info

#define  ATT_PAD_USAGE              "PAD_USAGE"    // this is a integer which can be attached to Apertures.
                                                   // this is evaluated in generate_padstack routine.
                                                   // only same pad_usage values are combined.

#define  ATT_NAME                   "NAME"         // this is a generic object name

#define  ATT_NETNAME                "NETNAME"      // this is a string for netname

#define  ATT_PATH                   "PATH"         // this is a string for file directory


#define  ATT_REFNAME                "REFNAME"      // this is a string for refdes
#define  ATT_GATEINSTANCENAME       "INSTANCENAME"    // this is a string for schematic instance name

// this is in addition to a pinname, which is the refname
#define  ATT_COMPPINNR              "PINNR"        // this is a integer and should be the pinnr

// COMPPINNAME is in refname if the insert
// #define  ATT_COMPPINNAME            "PINNAME"      // this is a string of any kind

#define  ATT_COMPHEIGHT             "COMPHEIGHT"   // component height in current units
                                                   // compheight on a placekeepout is a compheight keepout
#define  ATT_POWERNET               "POWERNET"     // nothing following
#define  ATT_SINGLEPINNET           "SINGLEPINNET" // nothing following

// if no att_vialayer is there, via is throu all layers
#define  ATT_VIALAYER               "VIALAYER"     // this is a string startlayer , endlayer

// this can not be named DEVICE, because a lot of CAD systems have a device attr. and that would
// overwrite this link and break it !!!
#define  ATT_TYPELISTLINK           "$$DEVICE$$"   // Device is the typename, as found in the typelist
                                                   // and this is the link between component and type list

#define  ATT_PACKAGELINK            "$$PACKAGE$$"  // Link between Geom and Package in Realpart

#define  ATT_PIN_PITCH              "PIN_PITCH"
#define  ATT_PIN_COUNT              "PIN_COUNT"

#define  ATT_DESIGNED_SURFACE       "DESIGNED_SURFACE"                  // string ("OPPOSITE_SURFACE" or "PLACED_SURFACE")
#define  ATT_TOP_DESIGNED_SURFACE   "TOP_PADSTACK_DESIGN_SURFACE"       // string ("OPPOSITE_SURFACE" or "PLACED_SURFACE") on a padstack only
#define  ATT_BOT_DESIGNED_SURFACE   "BOT_PADSTACK_DESIGN_SURFACE"       // string ("OPPOSITE_SURFACE" or "PLACED_SURFACE") on a padstack only

#define  ATT_HEIGHT                 "HEIGHT"       // Height. Needed for placement keepout. Generic name - can reuse.
#define  ATT_KEEPOUT_TYPE           "KEEPOUT_TYPE" // Type of Keepout - VIA, TRACE, TRACE_VIA, TESTPOINT, PLANE

#define  ATT_VOLTAGE                "VOLTAGE"      // double
#define  ATT_TOLERANCE              "TOLERANCE"    // double
#define  ATT_PLUSTOLERANCE          "+TOL"         // double
#define  ATT_MINUSTOLERANCE         "-TOL"         // double
#define  ATT_VALUE                  "VALUE"        // string
#define  ATT_PARTNUMBER             "PARTNUMBER"   // string
#define  ATT_DEVICETYPE             "DEVICETYPE"   // this is a normalized devicetypes list is dbutil.cpp
#define  ATT_SUBCLASS               "SUBCLASS"     // string
#define  ATT_SPEA4040_SUBCLASS      "Spea4040_Subclass"
#define  ATT_TERADYNE1800_SUBCLASS  "Teradyne1800_Subclass"
#define  ATT_AEROFLEX_SUBCLASS      "Aeroflex_Subclass"
#define  ATT_PART_DESCRIPTION       "DESCRIPTION"
#define  ATT_GEOM_REVIEW_STATUS     "GeomReviewStatus"
#define  ATT_ETCH_TYPE              "ETCH_TYPE"
#define  ATT_BEAD_PROBE             "BEAD_PROBE"
#define  ATT_CB_NETNAME             "CB_NETNAME"

#define  ATT_PROBEPLACEMENT         "PROBE_PLACEMENT"  // string ("Placed" or "Unplaced")
#define  ATT_TESTRESOURCE           "TEST_RESOURCE"  // string ("Test" or "Power Injection")
#define  ATT_TESTERINTERFACE        "TIN"          // ID obtained through the NAR file
#define  ATT_WIRE                   "WIRE"         

#define  ATT_CENTROID_X             "CENTROID_X"
#define  ATT_CENTROID_Y             "CENTROID_Y"

// this idetifies a polyline not to be converted in the global Draw->Flash conversion
#define  ATT_KEEPDRAW               "KEEPDRAW"     // no value

#define  ATT_PINFUNCTION            "PINFUNC"      // attribute on NetCompPins

#define  ATT_PINLOGICNAME           "PINDESC"      // only is allowed to go under a DEVICE, not a GEOMETRY.
                              // a DEVICE make the GEOM electrical smart and a DIP14 can have different PINDESC
                              // dependend on the TYPE. The syntax is a string <whitespace>pinrefname=pinlogicname
                              // pinlogicname is the pin name from a schematic gate.

#define ATT_CREATED_BY        "Created_By"
#define ATT_PACKAGE_ALIAS     "Package_Alias"
#define ATT_PACKAGE_SOURCE    "Package_Source"
#define ATT_OUTLINE_METHOD    "Outline Algorithm"  // name precedence set by historical usage in Access Analysis, was once #define ALG_KEYWORD, note the space instead of underscore

#define  ATT_DEVICETOPACKAGEPINMAP "DEVICE_TO_PACKAGE_PINMAP"
// not needed anymore, because all is done with one testpoint attribute
//#define   ATT_TESTPIN                "TESTPIN"      // this is part of netlist
//#define   ATT_TESTVIA                "TESTVIA"      // this is part of block references.

// a testpoint can be attached to a VIA - this is like a TESTVIA
// a testpoint can be attached to a pin in the Geometry selection - TESTPIN
// a testpoint can be attached to a Component geometry - all components of this GEOM are for test
// a testpoint can be attached to a Component - this is like all PINS are TESTPIN
// a testpoint can be attached to a compin in the netlist.

// the soldermask attribute is on PADSTACKS and/or CompPins in Netlist and Inserts on
// VIAS, TESTPOINTS, FREEPADS etc...
// This is evaluated by Test_Access_Analysis and others. The Graphic soldermask can be
// converted to an Attribute and vica versa.

// a  Soldermask is resolved in generate_PADSTACKACCESSFLAG in PCBUTIL.CPP
#define  ATT_SOLDERMASK             "SOLDERMASK"   // use testaccesslayers[] in DBUTIL.CPP
                                                   // ALL == BOTH, no entry is same as BOTH

#define  ATT_UNPLACED               "UNPLACED"     // this hase no value and indicates that this is an
                                                   // unplaced insert.

// ATT_FIDUCIAL_PLACE
#define  ATT_FIDUCIAL_PLACE_BOTH    "FIDUCIAL_BOTH"   // this means a fiducial is placed on both layers.
                                             // has VALUE NONE. If this is not there, the fiducial is placed
                                             // on ther placement layer - top or bottom.

// test is a mandatory test point
#define  ATT_TEST                   "TEST"   // this is a stringvalue
                                             // no value is ALL
                                             // TOP is TOP ACCESS
                                             // BOTTOM is BOTTOM ACCESS
                                             // NONE is no test

// testaccess is a possible feature access. Features are CompPin / Via / TestPoint / FreePad
#define  ATT_TESTACCESS             "TEST_ACCESS"  // this is a stringvalue
                                             // BOTH is top and bottom
                                             // TOP is TOP ACCESS
                                             // BOTTOM is BOTTOM ACCESS
                                             // NONE is no test

#define  ATT_TESTACCESS_FAIL        "TEST_ACCESS_FAIL"   // a string value error message on
                                    // the pins or features.

#define  ATT_TESTPROBE_FAIL         "TEST_PROBE_FAIL" // a string value error message on
                                    // the pins or features.

// distance of an access point to the next component or board outline.
#define  ATT_ACCESS_DISTANCE        "ACCESS_DISTANCE"    // unit double

// testpreference is a prefered test location. (if available);
#define  ATT_TESTPREFERENCE         "TEST_PREFERENCE" // this is a integer where 0 is the highes
                                                // preference and the higher integers are lower
                                                // preferences.

// this is relative to the pin/via origin, it needs to be rotated for Netlist Pin entries!
#define  ATT_TEST_OFFSET_TOP_X      "TEST_OFFSET_TOP_X" // double value representing offset for test access points
#define  ATT_TEST_OFFSET_TOP_Y      "TEST_OFFSET_TOP_Y"
#define  ATT_TEST_OFFSET_BOT_X      "TEST_OFFSET_BOT_X" // double value representing offset for test access points
#define  ATT_TEST_OFFSET_BOT_Y      "TEST_OFFSET_BOT_Y"

// this is a special geometry
#define  ATT_SHORT_TEST             "TEST_SHORT"   // this is an group number. All probable shorts
                                                   // with the same group integer number are potential shorting.

#define  ATT_TEST_CONNECTOR         "TEST_CONNECTOR"        // this is a test connector type
#define  ATT_TEST_IGNORE_OUTLINE    "TEST_IGNORE_OUTLINE"   // do not check for component outline

#define  ATT_CAP_OPENS              "CapacitiveOpens"
#define  ATT_DIODE_OPENS            "DiodeOpens"

// hp5dx
#define  ATT_5DX_PACKAGEID          "5DX_PACKAGE_ID"
#define  ATT_5DX_PINFAMILY          "5DX_PIN_FAMILY"
#define  ATT_5DX_PITCH              "5DX_PITCH"
#define  ATT_5DX_ALIGNMENT_1        "5DX_ALIGNMENT_1" // none
#define  ATT_5DX_ALIGNMENT_2        "5DX_ALIGNMENT_2" // none
#define  ATT_5DX_ALIGNMENT_3        "5DX_ALIGNMENT_3" // none
#define  ATT_5DX_TEST               "5DX_TEST"     // FALSE is false attribute
                                                   // PARTIAL is partial attribute
                                                   // any other or no value is true attribute.
#define  ATT_5DX_VIATEST            "5DX_VIATEST"  // use this via like a Possible short marker.
                                                   // the values are char *testaccesslayers[] in dbutil.cpp


#define ATT_WIRE_COLOR              "WIRE_COLOR"
#define ATT_WIRE_SIZE               "WIRE_SIZE"


// 3070 attributes - here all attributes needed to write a complete 3070 output.
#define  ATT_3070_COMMENT           "3070_COMMENT"
#define  ATT_3070_COMMON_PIN        "3070_COMMON_PIN"
#define  ATT_3070_CONTACT_PIN       "3070_CONTACT_PIN"
#define  ATT_3070_DEVICECLASS       "3070_DEVICECLASS"
#define  ATT_3070_FAIL_MESSAGE      "3070_FAILURE_MESSAGE"
#define  ATT_3070_HI_VALUE          "3070_HI_VAL"
#define  ATT_3070_LOW_VALUE         "3070_LOW_VAL"
#define  ATT_3070_NTOL              "3070_NEG_TOL"
#define  ATT_3070_PTOL              "3070_POS_TOL"
#define  ATT_3070_TYPE              "3070_TYPE"
#define  ATT_3070_VALUE             "3070_VALUE"
// on features and pins
#define  ATT_3070_PROBESIDE         "3070_PROBESIDE"
#define  ATT_3070_PROBEACCESS       "3070_PROBEACCESS"
#define  ATT_3070_NETNAME           "3070_NETNAME"

// AOI attributes
#define  ATT_AGILENT_AOI_PLACEMENT_MACHINE   "AOI_PLACEMENT_MACHINE"
#define  ATT_AGILENT_AOI_PLACEMENT_FEEDER    "AOI_PLACEMENT_FEEDER"
#define  ATT_AGILENT_AOI_TEST                "AOI_TEST"  // this is a stringvalue
                                                         // no value is ALL
                                                         // TOP is TOP ACCESS
                                                         // BOTTOM is BOTTOM ACCESS
                                                         // NONE is no test

#define  ATT_TERADYNE_7200_TEST     "TERADYNE_7200_TEST" // this is a stringvalue
                                                // no value is ALL
                                                // NONE is no test

#define  ATT_TERADYNE_AOI_PINTEST   "TERADYNE_AOI_PINTEST"


#define  ATT_HAF_REFNAME            "HAF_REFNAME"
#define  ATT_HAF_DEVICECLASS        "HAF_DEVICECLASS"
#define  ATT_HAF_PARTNUMBER         "HAF_PARTNUMBER"
#define  ATT_HAF_PACKAGE            "HAF_PACKAGE"
#define  ATT_HAF_VALUE              "HAF_VALUE"
#define  ATT_HAF_PTOL               "HAF_PTOL"
#define  ATT_HAF_NTOL               "HAF_NTOL"
#define  ATT_HAF_COMPHEIGHT         "HAF_COMPHEIGHT"
#define  ATT_HAF_TEST               "HAF_TEST"
#define  ATT_HAF_BOUNDARYSCAN       "HAF_BOUNDARYSCAN"
#define  ATT_HAF_NETPOTENTIAL       "HAF_NETPOTENTIAL"
#define  ATT_HAF_PINDIRECTION       "HAF_PINDIRECTION"
#define  ATT_HAF_PINFUNCTION        "HAF_PINFUNCTION"
#define  ATT_HAF_COMMENT            "HAF_COMMENT"


#define  ATT_CKT_NETNAME            "CKT_NETNAME"

// insertion support in CCM
#define  INSERTION_PROCESS          "INSERTION PROCESS"

// variant assembly or test
#define  ATT_LOADED                 "LOADED"                // value "FALSE" will not load this

// Gencam stuff
#define  ATT_GENCAMTYPE             "GENCAM_TYPE"

// Takaya stuff
#define  ATT_TAKAYA_DEVICECLASS           "TAKAYA_DEVICECLASS"
#define  ATT_TAKAYA_REFDES                "TAKAYA_REFDES"
#define  ATT_TAKAYA_DEVICECLASS_SUBCLASS  "TAKAYA_DEVICECLASS_SUBCLASS"

// Teradyne stuff
#define  ATT_TERADYNE_AOI_DEVICECLASS     "TERADYNEAOI_DEVICECLASS"

// TRI-MDA/INGUN stuff
#define  ATT_INGUN_DEVICECLASS            "TRI_DEVICECLASS"

#define  NETSTATUS_NORMAL     0     // see dbutil.cpp and assigned in camcad.rc
#define  NETSTATUS_NOPROBE    1
#define  NETSTATUS_CRITICAL   2
#define  NETSTATUS_GROUND     3
#define  NETSTATUS_POWER      4

#define  ATT_TEST_NET_STATUS        "TEST_NET_STATUS"    // see dbutil.cpp -> char  *netstatus[]
#define  ATT_TEST_NET_PROBES        "TEST_NET_PROBES"    // integer: number of probes needed

// all is the 0 index into testaccesslayers[]
#define  ATT_TEST_ACCESS_ALL        0
#define  ATT_TEST_ACCESS_BOT        1
#define  ATT_TEST_ACCESS_TOP        2
#define  ATT_TEST_ACCESS_NONE       3

// this is used to define a Geometry definition as SMD. This
// works for PADSTACK and PCBCOMPONENT
// here we also should include RADIAL, DIP, ZIP, CONN, and other manufacturing types.
#define  ATT_SMDSHAPE                  "SMD"    // none

// Techonology currently has a value of "THRU" or "SMD" to indicate whether
// the component is a surface mount or through hole
#define  ATT_TECHNOLOGY                "TECHNOLOGY"

// blind is a PADSTACK, which is not and SMD or THRU. BLIND may be a pin which is only placed on
// the bottom layer example on a EGDE connector. SMD is always correct on TOP.
// This is also used to flag BLIND and BURIED Vias. There is no difference between BLIND and BURIED in CAMCAD.
#define  ATT_BLINDSHAPE                "BLIND"  // none

// ATT_PINLOC is absolute pin location for exploded, placed pin. A pin is mirrored or otherwise
// complete normalized.
//
#define  ATT_PINACCESS                 "PIN_ACCESS"      // this is a string testaccess[] in dbutil.cpp

// DFT Net Attrubutes
#define  ATT_NET_TYPE                  "NET_TYPE"        // "Signal", "Power", or "Ground"
#define  ATT_CURRENT                   "CURRENT"         // in Amps
#define  ATT_TR_REQUIRED               "TR_Required"
#define  ATT_PIR_REQUIRED              "PIR_Required"
#define  ATT_NOPROBE                   "No_Probe"


// this flag is set when a local redefinition of a component pin has occured.
// the correct pin placement and padstack etc.. is in the CompPin record.
#define  ATT_COMPONENT_PINS_MODIFIED   "PINS_MODIFIED"

// DRC/DFM Attributes. This is also used for DRC markers.
#define  ATT_DFM_TESTNAME              "DFM_TEST_NAME"
#define  ATT_DFM_FAILURE_RANGE         "DFM_FAILURE_RANGE"
#define  ATT_DFM_CHECK_VALUE           "DFM_CHECK_VALUE"
#define  ATT_DFM_ACTUAL_VALUE          "DFM_ACTUAL_VALUE"
#define  ATT_DFM_LAYERTYPE             "DFM_LAYER_TYPE"  // A Layertype a DFM/DRC is detected
#define  ATT_DFM_REF1                  "DFM_REF1"        // Ref Des first item came from
#define  ATT_DFM_REF2                  "DFM_REF2"        // Ref Des second item came from
#define  ATT_DFM_KEYWORD               "DFM_KEYWORD" //
#define  ATT_DFM_VALUE1                "DFM_VALUE1" //
#define  ATT_DFM_VALUE2                "DFM_VALUE2" //
#define  ATT_PACKAGE_FAMILY            "PACKAGE_FAMILY" //

#define  ATT_DFM_LAYERSTACKNUMBER      "DFM_LAYER_STACKNUMBER" // a stacknumber, only needed for INNER LAYERS

#define  ATT_DFX_OUTLINE_TYPE          "DFX_OUTLINE_TYPE"

#define  ATT_DIRECTION_OF_TRAVEL       "DIRECTION_OF_TRAVEL"

#define  ATT_GRID_LOCATION             "GRID_LOCATION"

// Siemens attributes
#define  ATT_SIEMENS_FIDSYM            "SIEMENS_FIDSYM"
#define  ATT_SIEMENS_THICKNESS_PANEL   "SIEMENS_THICKNESS_PANEL"
#define  ATT_SIEMENS_THICKNESS_BOARD   "SIEMENS_THICKNESS_BOARD"

// Inherit Gerber Attributes
#define  ATT_ECAD_BOARD_ON_PANEL       "ECAD_BOARD_ON_PANEL"
#define  ATT_ECAD_GEOMETRY_NAME        "ECAD_GEOMETRY_NAME"
#define  ATT_ECAD_REFDES               "ECAD_REFDES"
#define  ATT_ECAD_PIN                  "ECAD_PIN"

// Gerber Educator attributes
#define ATT_GerberEducatorSurface      "EducatorSurface"
#define ATT_MergeToolSurface           "MergeToolSurface"
#define ATT_DataType                   "DataType"
#define ATT_DataSource                 "DataSource"

// DFT Analysys attributes
#define ATT_DFT_TARGETTYPE             "TARGET_TYPE"


// These are value used for attributes
//#define ATT_VALUE_NO_CONNECT           "No Connect"

// This variable indicate what test to run
#define ATT_TEST_STRATEGY              "TEST_STRATEGY"

// These are the TEST_STRATEGY value
//#define ATT_VALUE_TEST_AOI             "AOI"    // Automatic Optical Inspection
//#define ATT_VALUE_TEST_AXI             "AXI"    // Automatic X-Ray Inspection
//#define ATT_VALUE_TEST_BST             "BST"    // Boundary Scan Test
//#define ATT_VALUE_TEST_FPT             "FPT"    // Flying Probe Test
//#define ATT_VALUE_TEST_ICT             "ICT"    // In-Circuit Test
//#define ATT_VALUE_TEST_NT              "NT"     // Not Tested

// Attributes from the VIA_SPANS_SECTION in JobPrefs.hkp
#define ATT_VIA_SPANS_GENERAL_CLEARANCE   "VIA_SPANS_GENERAL_CLEARANCE"    // ENABLE_GENERAL_CLEARANCES
#define ATT_VIA_SPANS_SAME_NET_CLEARANCE  "VIA_SPANS_SAME_NET_CLEARANCE"   // ENABLE_SAME_NET_CLEARANCES
#define ATT_VIA_SPANS_USE_MNT_OPP_PADS    "VIA_SPANS_USE_MNT_OPP_PADS"      // USE_MNT_OPP_PADS_FOR_BB_VIAS
#define ATT_VIA_SPAN_FROM_LAYER           "VIA_SPAN_LAYER_FROM"            // Via Span FromLayer
#define ATT_VIA_SPAN_TO_LAYER             "VIA_SPAN_LAYER_TO"              // Via Span ToLayer
#define ATT_VIA_SPAN_CAPACITANCE          "VIA_SPAN_CAPACITANCE"           // Via Span Capacitance
#define ATT_VIA_SPAN_INDUCTANCE           "VIA_SPAN_INDUCTANCE"            // Via Span Inductance
#define ATT_VIA_SPAN_DELAY                "VIA_SPAN_DELAY"                 // Via Span Delay
#define ATT_VIA_SPAN_GRID                 "VIA_SPAN_GRID"                  // Via Span Grid
#define ATT_VIA_SPAN_ATTRIBUTES           "VIA_SPAN_ATTRIBUTES"            // Via Span Attributes

// these are the layer attributes
// LAYATT_SIGNALSTACK is also used for POWER Nets as a NETATTRIBUTE
#define  NETATT_SIGNALSTACK            "STACKNUMBER"     // this is the layer, the signal is tied to

#define  LAYATT_NETNAME                   ATT_NETNAME          // string with netname
#define  LAYATT_THICKNESS                 "LAYER THICKNESS"    // double
#define  LAYATT_MATERIAL                  "LAYER MATERIAL"     // string with material name
#define  LAYATT_DIELECTRIC_CONSTANT       "LAYER DIELECTRIC_CONSTANT"   // double
#define  LAYATT_ELECTRICAL_CONDUCTIVITY   "LAYER ELECTRICAL_CONDUCTIVITY"  // string
#define  LAYATT_SHIELD_LAYER              "LAYER SHIELD_LAYER" // string
#define  LAYATT_THERMAL_CONDUCTIVITY      "LAYER THERMAL_CONDUCTIVITY"  // string
#define  LAYATT_COPPER_THICKNESS          "LAYER COPPER_THICKNESS"   // string
#define  LAYATT_RESISTIVITY               "LAYER RESISTIVITY"   // string


///////////////////////////////////////////////////////////////////////////////
// in pcbutil is a function is_electricallayer, which returns if a layertype
// is considered to be electrical.
///////////////////////////////////////////////////////////////////////////////

// when chaning here, need to update pcbutil is_electricallayer and is_layerbottom !!!
// defined in dbutil.cpp
#define  LAYTYPE_UNKNOWN                  0
#define  LAYTYPE_SIGNAL_TOP               1  // component layer
#define  LAYTYPE_SIGNAL_BOT               2  // solder layer
#define  LAYTYPE_SIGNAL_INNER             3  // inner layer - number is from stackup
#define  LAYTYPE_SIGNAL_ALL               4  //
#define  LAYTYPE_DIELECTRIC               5  // dielectric layer
#define  LAYTYPE_DIALECTRIC               LAYTYPE_DIELECTRIC   // backward compatible misspelling

#define  LAYTYPE_PAD_TOP                  6  // pad top layer
#define  LAYTYPE_PAD_BOTTOM               7  // no param
#define  LAYTYPE_PAD_INNER                8  // no param
#define  LAYTYPE_PAD_ALL                  9  // no param
#define  LAYTYPE_PAD_THERMAL              10 // no param, a pads powerlayer.

// also need power, padall, padoutside,

#define  LAYTYPE_PASTE_TOP                11
#define  LAYTYPE_PASTE_BOTTOM             12
#define  LAYTYPE_MASK_TOP                 13
#define  LAYTYPE_MASK_BOTTOM              14
#define  LAYTYPE_SILK_TOP                 15
#define  LAYTYPE_SILK_BOTTOM              16

#define  LAYTYPE_POWERNEG                 17    // considered electrical
#define  LAYTYPE_POWERPOS                 18
#define  LAYTYPE_SPLITPLANE               19    // this is like a POWERNEG plane and can be used the same.
                                             // the theoretical difference is that POWER_NEG should only contain
                                                // one netname, and split multiple, but CAMCAD does not check for this.
#define  LAYTYPE_SIGNAL                   20    // general copper layer - do not use if possible

#define  LAYTYPE_MASK_ALL                 21
#define  LAYTYPE_PASTE_ALL                22
#define  LAYTYPE_REDLINE                  23
#define  LAYTYPE_DRILL                    24

#define  LAYTYPE_TOP                      25    // generic top
#define  LAYTYPE_BOTTOM                   26    // generic bottom
#define  LAYTYPE_ALL                      27    // generic all

#define  LAYTYPE_BOARD_OUTLINE            28

#define  LAYTYPE_PAD_OUTER                29    // no param - considered electrical
#define  LAYTYPE_SIGNAL_OUTER             30    // no param - electrical top and bottom

#define  LAYTYPE_DRC_CRITICAL             31
#define  LAYTYPE_PLANECLEARANCE           32
#define  LAYTYPE_COMPONENTOUTLINE         33
#define  LAYTYPE_PANEL_OUTLINE            34

#define  LAYTYPE_COMPONENT_DFT_TOP        35
#define  LAYTYPE_COMPONENT_DFT_BOTTOM     36
#define  LAYTYPE_DRC_MARGINAL             37
#define  LAYTYPE_DRC_ACCEPTABLE           38

#define  LAYTYPE_FLUX_TOP                 39
#define  LAYTYPE_FLUX_BOTTOM              40

#define  LAYTYPE_PKG_PIN_LEG_TOP          41
#define  LAYTYPE_PKG_PIN_LEG_BOT          42

#define  LAYTYPE_PKG_PIN_FOOT_TOP         43
#define  LAYTYPE_PKG_PIN_FOOT_BOT         44

#define  LAYTYPE_STENCIL_TOP              45
#define  LAYTYPE_STENCIL_BOTTOM           46

#define  LAYTYPE_CENTROID_TOP             47
#define  LAYTYPE_CENTROID_BOTTOM          48

#define  LAYTYPE_PKG_BODY_TOP             49
#define  LAYTYPE_PKG_BODY_BOT             50

#define  LAYTYPE_DFT_TOP                  51
#define  LAYTYPE_DFT_BOTTOM               52

//#define  LAYTYPE_GERBER_TOP               53
//#define  LAYTYPE_GERBER_BOTTOM            54

//#define  LAYTYPE_APERTURE_TOP             55
//#define  LAYTYPE_APERTURE_BOTTOM          56
#define  LAYTYPE_DOCUMENTATION            53 // Part of fix for dts0100505071
#define  LAYTYPE_SCORING_LINE             54 // Part of fix for dts0100505072
#define  LAYTYPE_SHEARING_LINE            55 // Part of fix for dts0100505073
#define  LAYTYPE_DRILL_DRAWING_THRU       56 // Part of fix for dts0100505074
///#define  LAYTYPE_COMPONENTOUTLINE_BOTTOM  57

#define  MAX_LAYTYPE                      (layerTypeUpperBound + 1)    // set to 1 higher than max laytype value.
                                                                       // it is done this way so we can maintain laytype enum
                                                                       // in DcaLayerType.h without having to continually come
                                                                       // here and update these #defines too. These symbols
                                                                       // are in process of being depricated, but many are still in
                                                                       // use. Don't need to add the new types here, just need to
                                                                       // keep this "MAX" up to date so loops include the new things.

// when adding a layer type, don't forget to add entries to the following:
// LayerTypeTag,
// layerTypeToMacroString(),
// layerTypeToDisplayString(),
// getLayerGroupForLayerType(),


#define  LAYGROUP_UNKNOWN                 0
#define  LAYGROUP_TOP                     1
#define  LAYGROUP_BOTTOM                  2
#define  LAYGROUP_ALL                     3
#define  LAYGROUP_INNER                   4
#define  LAYGROUP_OUTER                   5
#define  LAYGROUP_PLANES                  6
#define  LAYGROUP_DRC                     7
#define  LAYGROUP_MISC                    8

#define  MAX_LAYGROUP                     9    // start with 0

// they should be attached to the Tool
//#define   DRILL_PLATED               "DRILL PLATED"    //drill is always plated unless it says non plated
#define  DRILL_NONPLATED            "DRILL NONPLATED" // none
//#define   DRILL_THRU                 "DRILL THRU"      // drill is always thru unless it has a start end layer
//#define   DRILL_BLIND                "DRILL BLIND"     // none
//#define   DRILL_BURIED               "DRILL BURIED"    // none
#define  DRILL_STARTLAYER           "DRILL START LAYER"  // string
#define  DRILL_ENDLAYER             "DRILL END LAYER"    // string

// PCB file attribute - Global PCB technology
// These attribute are assigned to the PCB block

// example
// "DRILL_ORIGIN"
// "BOARD_DEFINITION_IDENTIFIER"
// "BOARD_WIRE_GRID_X"
// "BOARD_WIRE_GRID_Y"
// "BOARD_VIA_GRID_X"
// "BOARD_VIA_GRID_Y"
// "BOARD_PIN_GRID_X"
// "BOARD_PIN_GRID_Y"
// "BOARD_ROUTING_LAYERS"
// "POWER_NET_NAMES"
// "BOARD_PLACEMENT_GRID"
// "BOARD_MATERIAL"
// "BOARD_THICKNESS"

#define  BOARD_ORIGINAL_SOURCE            "Source CAD system"           // this is a complete string, including version etc..
#define  BOARD_THICKNESS                  "Board Thickness"             // double
#define  BOARD_NUMBER_OF_LAYERS           "Number of Routing Layers"    // int, this needs to match the electrical layer stackup
#define  BOARD_MATERIAL                   "Board Material"              // this is a complete string, including version etc..

#define  TEXT_UNDERLINE 1  // this is boolean
#define  TEXT_OVERLINE  2

struct LayerTypeInfoStruct
{
   char *name;
   int group;
};

class FileStruct;
class BlockStruct;
class CCEtoODBDoc;

double NormalizeAngle(double angle);
void RestructureComponentData(FileStruct *file, CCEtoODBDoc *doc);
BOOL compHasNewStruture(BlockStruct *compBlock, CCEtoODBDoc *doc);

#endif
