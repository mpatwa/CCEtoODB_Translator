// $Header: /CAMCAD/4.5/read_wrt/GenCamIn.h 15    4/12/06 10:57a Rick Faltersack $

/****************************************************************************/
/*  Project CAMCAD                                     
	 Router Solutions Inc.
	 Copyright © 1994-2001. All Rights Reserved.

*/

#if !defined(__GenCamIn_h__)
#define __GenCamIn_h__

#pragma once

#include "CamCadDatabase.h"

#define	MAX_ATT		100			// max attributes per component
#define	MAX_LINE		500

#define  GENCAMERR	"gencam.log"

#define	STAT_NONE				0
#define	STAT_ROUTE				1
#define	STAT_PANEL				2
#define	STAT_BOARD				3
#define	STAT_PANELOUTLINE		4
#define	STAT_BOARDOUTLINE		5
#define	STAT_ASSEMBLY			6
#define	STAT_BOARDCUTOUT		7		
#define	STAT_PANELCUTOUT		8		

#define	PAINT_HOLLOW			0
#define	PAINT_FILL				1
#define	PAINT_VOID				2
#define	PAINT_MESH				3
#define	PAINT_HATCH				4

static char *gencam_layertype[] =
{
	"LEGEND",
	"GLUE",
	"SOLDERMASK",
	"BOARDOUTLINE",
	"COATINGCONDD",
	"COATINGNONCOND",
	"CONDUCTOR",
	"COURTYARD",
	"DIELBASE",
	"DIELCORE",
	"DIELPREG",
	"DIELADHV",
	"SOLDERBUMP",
	"PASTEMASK",
	"HOLEFILL",
	"PIN",
	"COMPONENT",
	"RESISTIVE",
	"CAPACITIVE",
	"PROBE",
	"FIXTURE",
	"GRAPHIC",
};

typedef	struct
{
	char	*token;
	int	(*function)();
} List;

typedef struct
{
	CString	refdes;
	CString	layergroup, layername;
	CString	device_ref, device_group;
	CString	pattern_ref, pattern_group;
	double	x, y, rotation;
	int		mirror;
	int		type;		// mechanical, pcb etc...
}GENCAMComp;

typedef struct
{
	CString	name;
	int		typ;			
}GENCAMPaint;

//typedef CTypedPtrArray<CPtrArray, GENCAMPaint*> CPaintArray;
class CPaintArray : public CTypedPtrArrayContainer<GENCAMPaint*>
{
public:
	bool isFill(CString paintdesc);
};

typedef struct
{
	CString	name;
	CString	surface;
	CString	gencamtyp;

   CString getName() const { return name; }
}GENCAMLayer;

//typedef CTypedPtrArray<CPtrArray, GENCAMLayer*> CLLayerArray;
class CLLayerArray : public CTypedPtrArrayContainer<GENCAMLayer*>
{
};


class CGENCAMAtt
{
private:
	CString m_key;
	CString m_val;
public:
	CGENCAMAtt(CString key, CString val) {m_key = key; m_val = val;}
	CString GetKey() { return m_key; }
	CString GetVal() { return m_val; }
};

class CAttrArray : public CTypedPtrArrayContainer<CGENCAMAtt*>
{
};


//_____________________________________________________________________________

class CGenCamReader
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;
   CTypedMapStringToPtrContainer<BlockStruct*> m_primitiveMap;
	void LoadSettings(CString settingsFile);
	CMapStringToString AttribNameMap;  // map gcam file attrib name to name to be used in camcad
	CString m_settingsFilename;

public:
   CGenCamReader(CCEtoODBDoc& camCadDoc);
	CAttrArray QueuedAttributes;

public:
   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }

	CString getSettingsFilename()			{ return m_settingsFilename; }

   BlockStruct* graphBlockOn(const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct* getBlock(const CString& name,int fileNumber);

   BlockStruct* getPrimitive(const CString& primitiveName);
   void defineCirclePrimitive(const CString& primitiveName,int fileNumber,double radius);
   void defineRectanglePrimitive(const CString& primitiveName,int fileNumber,double width,double height);
   void defineOvalPrimitive(const CString& primitiveName,int fileNumber,double width,double height);

	CString mapName(CString gencamName) 
	{
		int count = AttribNameMap.GetCount();
		CString mappedName; 
		gencamName.MakeUpper();
		if (AttribNameMap.Lookup(gencamName, mappedName)) 
			return mappedName; 
      
		return gencamName; 
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	fskip(), fnull();
static	int	gcam_header();
static	int	gcam_boards();
static	int	gcam_panels();
static	int	gcam_primitives();
static	int	gcam_layers();
static	int	gcam_artworks();
static	int	gcam_padstacks();
static	int	gcam_routes();
static	int	gcam_components();
static	int	gcam_patterns();
static	int	gcam_packages();
static	int	gcam_devices();
static	int	gcam_administration();
static	int	gcam_testconnects();
static	int	gcam_changes();
static	int	gcam_mechanicals();
static	int	gcam_fixtures();
static	int	gcam_families();
static	int	gcam_drawings();
static	int	gcam_power();

static List	start_lst[] =
{
	{"$HEADER",				gcam_header},  
	{"$BOARDS",				gcam_boards},  
	{"$PANELS",				gcam_panels},  
	{"$PRIMITIVES",		gcam_primitives},  
	{"$LAYERS",				gcam_layers},  
	{"$PADSTACKS",			gcam_padstacks},  
	{"$PATTERNS",			gcam_patterns},  
	{"$PACKAGES",			gcam_packages},  
	{"$DEVICES",			gcam_devices},  
	{"$FAMILIES",			gcam_families},  
	{"$ARTWORKS",			gcam_artworks},  
	{"$COMPONENTS",		gcam_components},  
	{"$ROUTES",				gcam_routes},  
	{"$ADMINISTRATION",	gcam_administration},  
	{"$TESTCONNECTS",		gcam_testconnects},  
	{"$CHANGES",			gcam_changes},  
	{"$MECHANICALS",		gcam_mechanicals},  
	{"$FIXTURES",			gcam_fixtures},  
	{"$POWER",				gcam_power},  
	{"$DRAWINGS",			gcam_drawings},  
};

#define	SIZ_START_LST		(sizeof(start_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	header_gencam();
static	int	header_units();
static	int	header_angleunits();

static List	header_lst[] =
{	
	{"GENCAM",				header_gencam},  
	{"UNITS",				header_units},  
	{"ANGLEUNITS",			header_angleunits},  
   {"GENERATEDBY",		fnull},
   {"HISTORY",				fnull},
   {"BOARD",				fnull},
   {"PANEL",				fnull},
   {"LANGUAGE",			fnull},
   {"CERTIFICATION",		fnull},
   {"ASSEMBLY",			fnull},
};

#define	SIZ_HEADER_LST		(sizeof(header_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	boards_attribute();
static	int	boards_board();
static	int	boards_assembly();
static	int	gcam_outline();
static	int	gcam_cutout();
static	int	gcam_keepout();

static List	boards_lst[] =
{	
	{"ATTRIBUTE",	boards_attribute},  
	{"BOARD",		boards_board},  
	{"ASSEMBLY",	boards_assembly},  
};

#define	SIZ_BOARDS_LST		(sizeof(boards_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	panels_panel();

static List	panels_lst[] =
{	
	{"PANEL",		panels_panel},  
};

#define	SIZ_PANELS_LST		(sizeof(panels_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	routes_group();

static List	routes_lst[] =
{	
	{"GROUP",		routes_group},  
};

#define	SIZ_ROUTES_LST		(sizeof(routes_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	components_group();

static List	components_lst[] =
{	
	{"GROUP",		components_group},  
};

#define	SIZ_COMPONENTS_LST		(sizeof(components_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	patterns_group();

static List	patterns_lst[] =
{	
	{"GROUP",		patterns_group},  
};

#define	SIZ_PATTERNS_LST		(sizeof(patterns_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	padstacks_group();

static List	padstacks_lst[] =
{	
	{"GROUP",		padstacks_group},  
};

#define	SIZ_PADSTACKS_LST		(sizeof(padstacks_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	packages_group();

static List	packages_lst[] =
{	
	{"GROUP",		packages_group},  
};

#define	SIZ_PACKAGES_LST		(sizeof(packages_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	devices_group();

static List	devices_lst[] =
{	
	{"GROUP",		devices_group},  
};

#define	SIZ_DEVICES_LST		(sizeof(devices_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	administration_lst[] =
{	
	{"BOARD",					fnull},  // board, quantities
	{"PANEL",					fnull},  // panel, quantities
	{"TRANSACTION",			fnull},  
	{"PERSON",					fnull},  
	{"SENT",						fnull},  
	{"OWNER",					fnull},  
	{"ENTERPRISE",				fnull},  
	{"RECEIVED",				fnull},  
	{"ASSEMBLY",				fnull},  
	{"FIXTURE",					fnull},  
	{"COMMENT",					fnull},  
};

#define	SIZ_ADMINISTRATION_LST	(sizeof(administration_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	testconnects_lst[] =
{	
	{"TESTPROBE",				fnull},  
};

#define	SIZ_TESTCONNECTS_LST		(sizeof(testconnects_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	changes_lst[] =
{	
	{"WS",			fnull},  
};

#define	SIZ_CHANGES_LST			(sizeof(changes_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	fixtures_lst[] =
{	
	{"WS",			fnull},  
};

#define	SIZ_FIXTURES_LST			(sizeof(fixtures_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	families_lst[] =
{	
	{"WS",			fnull},  
};

#define	SIZ_FAMILIES_LST			(sizeof(families_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	power_lst[] =
{	
	{"WS",			fnull},  
};

#define	SIZ_POWER_LST				(sizeof(power_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	assembly_lst[] =
{	
	{"USING",		fnull},  
};

#define	SIZ_ASSEMBLY_LST			(sizeof(assembly_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	drawings_lst[] =
{	
	{"WS",			fnull},  
};

#define	SIZ_DRAWINGS_LST			(sizeof(drawings_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	mechanicals_lst[] =
{	
	{"WS",			fnull},  
};

#define	SIZ_MECHANICALS_LST		(sizeof(mechanicals_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	primitives_group();

static List	primitives_lst[] =
{	
	{"GROUP",		primitives_group},  
};

#define	SIZ_PRIMITIVES_LST	(sizeof(primitives_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	layers_group();

static List	layers_lst[] =
{	
	{"GROUP",		layers_group},  
};

#define	SIZ_LAYERS_LST		(sizeof(layers_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	primitive_barreldesc();
static	int	primitive_linedesc();
static	int	primitive_paintdesc();
static	int	primitive_circledef();
static	int	primitive_thermaldef();
static	int	primitive_rectcenterdef();
static	int	primitive_dshapedef();
static	int	primitive_rectcornerdef();
static	int	primitive_polygondef();
static	int	primitive_ovaldef();

static List	primitive_lst[] =
{	
	{"COLOR",			fnull},  
	{"BARREL",			fnull},  
	{"BARRELDESC",		primitive_barreldesc},  
	{"PAINTDESC",		primitive_paintdesc},  
	{"LINEDESC",		primitive_linedesc},  
	{"THERMALDEF",		primitive_thermaldef},  
	{"CIRCLEDEF",		primitive_circledef},  
	{"RECTCENTERDEF",	primitive_rectcenterdef},  
	{"RECTCORNERDEF",	primitive_rectcornerdef},  
	{"OVALDEF",			primitive_ovaldef},  
	{"POLYGONDEF",		primitive_polygondef},  
	{"DSHAPEDEF",		primitive_dshapedef},  
};

#define	SIZ_PRIMITIVE_LST	(sizeof(primitive_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	layer_layersingle();
static	int	layer_layerswap();

static List	layer_lst[] =
{	
	{"LAYERSINGLE",	layer_layersingle},  
	{"LAYERSWAP",		layer_layerswap},  
};

#define	SIZ_LAYER_LST		(sizeof(layer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	artworks_group();

static List	artworks_lst[] =
{	
	{"GROUP",		artworks_group},  
};

#define	SIZ_ARTWORKS_LST	(sizeof(artworks_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_artworkdef();  
static	int	gcam_targetdef();  
static	int	gcam_featuredef();  
static	int	gcam_logodef();  

static List	artworks_artwork_lst[] =
{	
	{"ARTWORKDEF",		gcam_artworkdef},  
	{"TARGETDEF",		gcam_targetdef},  
	{"FEATUREDEF",		gcam_featuredef},  
	{"LOGODEF",			gcam_logodef},  
};

#define	SIZ_ARTWORKS_ARTWORK_LST	(sizeof(artworks_artwork_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_route();

static List	routes_route_lst[] =
{	
	{"ROUTE",		gcam_route},  
};

#define	SIZ_ROUTES_ROUTE_LST		(sizeof(routes_route_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_component();

static List	components_component_lst[] =
{	
	{"COMPONENT",	gcam_component},  
};

#define	SIZ_COMPONENTS_COMPONENT_LST	(sizeof(components_component_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_patterndef();

static List	patterns_pattern_lst[] =
{	
	{"PATTERNDEF",		gcam_patterndef},  
};

#define	SIZ_PATTERNS_PATTERN_LST	(sizeof(patterns_pattern_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_padstack();
static	int	gcam_holedef();
static	int	gcam_paddef();

static List	padstacks_padstack_lst[] =
{	
	{"HOLEDEF",		gcam_holedef},  
	{"PADDEF",		gcam_paddef},  
	{"PADSTACK",	gcam_padstack},  
};

#define	SIZ_PADSTACKS_PADSTACK_LST	(sizeof(padstacks_padstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_package();

static List	packages_package_lst[] =
{	
	{"PACKAGE",		gcam_package},  
};

#define	SIZ_PACKAGES_PACKAGE_LST	(sizeof(packages_package_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_device();

static List	devices_device_lst[] =
{	
	{"DEVICE",		gcam_device},  
};

#define	SIZ_DEVICES_DEVICE_LST		(sizeof(devices_device_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	route_attribute();  
static	int	route_path();  
static	int	route_plane();  
static	int	route_via();  
static	int	route_testpad();  
static	int	route_comppin();  
static	int	route_connpin();  
static	int	route_highpottest();  

static List	route_lst[] =
{	
	{"ATTRIBUTE",	route_attribute},  
	{"PATH",			route_path},  
	{"PLANE",		route_plane},  
	{"VIA",			route_via},  
	{"TESTPAD",		route_testpad},  
	{"COMPPIN",		route_comppin},  
	{"CONNPIN",		route_connpin},  
	{"HIGHPOTTEST",route_highpottest},  
};

#define	SIZ_ROUTE_LST		(sizeof(route_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_artworkref();
static	int	pattern_padstackref();
static	int	pattern_artwork();

static List	pattern_lst[] =
{	
	{"PADSTACKREF",	pattern_padstackref},  
	{"ARTWORK",			pattern_artwork},  
	{"ARTWORKREF",		gcam_artworkref},  
};

#define	SIZ_PATTERN_LST	(sizeof(pattern_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	device_attribute();
static	int	device_part();
static	int	device_value();
static	int	device_pindesc();

static List	device_lst[] =
{	
	{"ATTRIBUTE",		device_attribute},  
	{"PART",				device_part},  
	{"VALUE",			device_value},  
	{"PINDESC",			device_pindesc},  
};

#define	SIZ_DEVICE_LST		(sizeof(device_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	padstack_pad();
static	int	padstack_holeref();
static	int	padstack_padref();

static List	padstack_lst[] =
{	
	{"PAD",			padstack_pad},  
	{"HOLEREF",		padstack_holeref},  
	{"PADREF",		padstack_padref},  
};

#define	SIZ_PADSTACK_LST	(sizeof(padstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	package_body();
static	int	package_pin();

static List	package_lst[] =
{	
	{"BODY",			package_body},  
	{"PIN",			package_pin},  
};

#define	SIZ_PACKAGE_LST	(sizeof(package_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	component_attribute();
static	int	component_deviceref();
static	int	component_patternref();
static	int	component_mechanical();
static	int	component_keepout();

static List	component_lst[] =
{	
	{"ATTRIBUTE",		component_attribute},  
	{"DEVICEREF",		component_deviceref},  
	{"PATTERNREF",		component_patternref},		// this is the placement shape
	{"MECHANICAL",		component_mechanical},
	{"KEEPOUT",			component_keepout},
};

#define	SIZ_COMPONENT_LST	(sizeof(component_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	board_hole();		
static	int	board_using();		

// this is a board
static List	board_lst[] =
{	
	{"USING",			board_using},		// needed to link groups (file blocks) to the design
	{"OUTLINE",			gcam_outline},
	{"CUTOUT",			gcam_cutout},	
	{"KEEPOUT",			gcam_keepout},
	{"HOLE",				board_hole},
	{"BAREBOARDTEST",	fnull},				// not needed
};

#define	SIZ_BOARD_LST		(sizeof(board_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	panel_placement();

// this is a panel
static List	panel_lst[] =
{	
	{"OUTLINE",			gcam_outline}, 
	{"CUTOUT",			gcam_cutout},
	{"PLACEMENT",		panel_placement},
};

#define	SIZ_PANEL_LST		(sizeof(panel_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_line();
static	int	gcam_polygon();

static List	polygon_lst[] =
{	
	{"POLYGON",			gcam_polygon},  
};

#define	SIZ_POLYGON_LST	(sizeof(polygon_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	closed_shape_rectcenter();

static List	closed_shape_lst[] =
{	
	{"POLYGON",			gcam_polygon},			// polystruct is made in outline
	{"RECTCENTER",		closed_shape_rectcenter},  
	{"THICKNESS",		fnull},  
};

#define	SIZ_CLOSED_SHAPE_LST	(sizeof(closed_shape_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_polyline();

static List	polyline_lst[] =
{	
	{"POLYLINE",		gcam_polyline},  
};

#define	SIZ_POLYLINE_LST	(sizeof(polyline_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	polygonlist_line();
static	int	polygonlist_circarc();
static	int	polygonlist_ellip();

static	int	polygonlist_startat();
static	int	polygonlist_lineto();
static	int	polygonlist_circarcto();
static	int	polygonlist_endline();
static	int	polygonlist_endcircarc();

// this is used for polygon and polyline lists
static List	polygonlist_lst[] =
{	
	{"STARTAT",		polygonlist_startat},
	{"LINETO",		polygonlist_lineto},
	{"CIRCARCTO",	polygonlist_circarcto},
	{"ENDLINE",		polygonlist_endline},
	{"ENDCIRCARC",	polygonlist_endcircarc},

	//{"LINE",			polygonlist_line},
	//{"CIRCARC",		polygonlist_circarc},
	//{"ELLIPARC",	polygonlist_ellip},
};

#define	SIZ_POLYGONLIST_LST	(sizeof(polygonlist_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List	outline_lst[] =
{	
	{"LINE",			gcam_line},  
};

#define	SIZ_OUTLINE_LST	(sizeof(outline_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static	int	gcam_polygonref();
static	int	gcam_rectcenterref();
static	int	gcam_rectcornerref();
static	int	gcam_circarc();
static	int	gcam_circle();
static	int	gcam_rectcenter();
static	int	gcam_rectcorner();
static	int	gcam_text();

static List	artworkbuilder_lst[] =
{	
	{"LINE",				gcam_line},  
	{"CIRCARC",			gcam_circarc},  
	{"CIRCLE",			gcam_circle},  
	{"RECTCENTERREF",	gcam_rectcenterref},  
	{"RECTCORNERREF",	gcam_rectcornerref},  
	{"RECTCENTER",		gcam_rectcenter},  
	{"RECTCORNER",		gcam_rectcorner},  
	{"POLYGON",			gcam_polygon},  
	{"POLYLINE",		gcam_polyline},  
	{"TEXT",				gcam_text},  
	// this is text, logoref, artworkref which is specific to artworkdef.
	{"ARTWORKREF",		gcam_artworkref},  
	{"POLYGONREF",		gcam_polygonref},  
};

#define	SIZ_ARTWORKBUILDER_LST	(sizeof(artworkbuilder_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#endif
