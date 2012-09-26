// $Header: /CAMCAD/4.5/read_wrt/AllegIn.h 20    6/20/06 11:09a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#define  ALLEGRO_UNKNOWN         0
#define  ALLEGRO_CAMCAD          1
#define  ALLEGRO_FABMASTER       2
#define  ALLEGRO_LAYDBASE        3
#define  ALLEGRO_ALL					4  // This format apply to all format files

int tst_allegtoextractfiletype(const char *filename);

#define  MAX_LINE                         1024
#define  MAX_TOKEN                        255
#define  MAX_LAYERS                       255
#define  MAX_MASTER                       1000
#define  MAX_PADSHAPE                     1000
#define  MAX_ATTR                         2000

#define  UNKNOWN_EXTRACT                  0
#define  EXTRACT_PAD                      1     // this is the same for CAMCAD and FABMASTER
#define  EXTRACT_PAD_SHAPE_NAME           2     // so is this

// CAMCAD Extract
#define  CAMCAD_EXTRACT_LAYER             3
#define  CAMCAD_EXTRACT_SYM               4
#define  CAMCAD_EXTRACT_RTE               5

// LAYDBASE Extract
#define  CAMCAD_EXTRACT_CLASSIC_SYM       6
#define  CAMCAD_EXTRACT_CLASSIC_RTE       7

// FABMASTER Extract
#define  FABMASTER_EXTRACT_CLASS_VIA      8
#define  FABMASTER_EXTRACT_PINS           9
#define  FABMASTER_EXTRACT_PINS1          10     // this can do pin rotation
#define  FABMASTER_EXTRACT_CLASS_GRAPHIC  11
#define  FABMASTER_EXTRACT_GRAPHIC        12
#define  FABMASTER_EXTRACT_LAYERS         13
#define  FABMASTER_EXTRACT_NET_NAME       14
#define  FABMASTER_EXTRACT_REFDES         15


typedef  struct
{
   char  *token;
   int   index;
} ValList;

enum AllegroFieldTag
{
   VAL_UNKNOWN                         =  0,
   VAL_LAYER_SORT                      =  1,    // Layer file 
   VAL_LAYER_SUBCLASS                  =  2,
   VAL_LAYER_ARTWORK                   =  3,
   VAL_PAD_NAME                        =  4,   // padstack file
   VAL_REC_NUMBER                      =  5,
   VAL_LAYER                           =  6,
   VAL_FIXFLAG                         =  7,
   VAL_VIAFLAG                         =  8,
   VAL_PADSHAPE1                       =  9,
   VAL_PADWIDTH                        = 10,
   VAL_PADHGHT                         = 11,
   VAL_PADXOFF                         = 12,
   VAL_PADYOFF                         = 13,
   VAL_PADFLASH                        = 14,
   VAL_PADSHAPENAME                    = 15,
   VAL_TRELSHAPE1                      = 16,
   VAL_TRELWIDTH                       = 17,
   VAL_TRELHGHT                        = 18,
   VAL_TRELXOFF                        = 19,
   VAL_TRELYOFF                        = 20,
   VAL_TRELFLASH                       = 21,
   VAL_TRELSHAPENAME                   = 22,
   VAL_APADSHAPE1                      = 23,
   VAL_APADWIDTH                       = 24,
   VAL_APADHGHT                        = 25,
   VAL_APADXOFF                        = 26,
   VAL_APADYOFF                        = 27,
   VAL_APADFLASH                       = 28,
   VAL_APADSHAPENAME                   = 29,
   VAL_SYM_TYPE                        = 30,
   VAL_SYM_NAME                        = 31,
   VAL_REFDES                          = 32,
   VAL_SYM_X                           = 33,
   VAL_SYM_Y                           = 34,
   VAL_SYM_ROTATE                      = 35,
   VAL_SYM_MIRROR                      = 36,
   VAL_NET_NAME                        = 37,
   VAL_CLASS                           = 38,
   VAL_SUBCLASS                        = 39,
   VAL_RECORD_TAG                      = 40,
   VAL_GRAPHIC_DATA_NAME               = 41,
   VAL_GRAPHIC_DATA_NUMBER             = 42,
   VAL_GRAPHIC_DATA_1                  = 43,
   VAL_GRAPHIC_DATA_2                  = 44,
   VAL_GRAPHIC_DATA_3                  = 45,
   VAL_GRAPHIC_DATA_4                  = 46,
   VAL_GRAPHIC_DATA_5                  = 47,
   VAL_GRAPHIC_DATA_6                  = 48,
   VAL_GRAPHIC_DATA_7                  = 49,
   VAL_GRAPHIC_DATA_8                  = 50,
   VAL_GRAPHIC_DATA_9                  = 51,
   VAL_GRAPHIC_DATA_10                 = 52,
   VAL_COMP_DEVICE_TYPE                = 53,
   VAL_COMP_PACKAGE                    = 54,
   VAL_COMP_PART_NUMBER                = 55,
   VAL_COMP_VALUE                      = 56,
   VAL_VIA_X                           = 57,
   VAL_VIA_Y                           = 58,
   VAL_PIN_NUMBER_SORT                 = 59,
   VAL_PAD_STACK_NAME                  = 60,
   VAL_PIN_NUMBER                      = 61,
   VAL_PIN_NAME                        = 62,
   VAL_PIN_X                           = 63,
   VAL_PIN_Y                           = 64,
   VAL_LAYER_USE                       = 65,    // continue Layerfile
   VAL_LAYER_CONDUCTOR                 = 66,
   VAL_LAYER_DIELECTRIC_CONSTANT       = 67,
   VAL_LAYER_ELECTRICAL_CONDUCTIVITY   = 68,
   VAL_LAYER_MATERIAL                  = 69,
   VAL_LAYER_SHIELD_LAYER              = 70,
   VAL_LAYER_THERMAL_CONDUCTIVITY      = 71,
   VAL_LAYER_THICKNESS                 = 72,
   VAL_PIN_ROTATION                    = 73,
   VAL_PIN_EDITED                      = 74,    // this shows if a component pin was modified after the definition
   VAL_TEST_POINT                      = 75,
   VAL_COMP_CLASS                      = 76,
   VAL_NET_PROBE_NUMBER		            = 77,
   VAL_VIA_MIRROR		                  = 78
};

//
// NO UNDERSCORES ALLOWED IN THIS TABLE'S STRING VALUES !!!!!!!!!
// Example of cad data have been found with and without underscores.
// Leave underscore out here, we squeeze out the underscore in 
// incoming data, to match to this table.
//
static   ValList  RecognizedCommandAry[] = 
{
   {"UNKNOWN",                         VAL_UNKNOWN        },
   {"LAYERSORT",                       VAL_LAYER_SORT     },    // Layer file 
   {"LAYERSUBCLASS",                   VAL_LAYER_SUBCLASS },
   {"LAYERARTWORK",                    VAL_LAYER_ARTWORK  },
   {"PADNAME",                         VAL_PAD_NAME       },   // padstack file
   {"RECNUMBER",                       VAL_REC_NUMBER     },
   {"LAYER",                           VAL_LAYER          },
   {"FIXFLAG",                         VAL_FIXFLAG        },
   {"VIAFLAG",                         VAL_VIAFLAG        },
   {"PADSHAPE1",                       VAL_PADSHAPE1      },
   {"PADWIDTH",                        VAL_PADWIDTH       },
   {"PADHGHT",                         VAL_PADHGHT        },
   {"PADXOFF",                         VAL_PADXOFF        },
   {"PADYOFF",                         VAL_PADYOFF        },
   {"PADFLASH",                        VAL_PADFLASH       },
   {"PADSHAPENAME",                    VAL_PADSHAPENAME   },
   {"TRELSHAPE1",                      VAL_TRELSHAPE1     },
   {"TRELWIDTH",                       VAL_TRELWIDTH      },
   {"TRELHGHT",                        VAL_TRELHGHT       },
   {"TRELXOFF",                        VAL_TRELXOFF       },
   {"TRELYOFF",                        VAL_TRELYOFF       },
   {"TRELFLASH",                       VAL_TRELFLASH      },
   {"TRELSHAPENAME",                   VAL_TRELSHAPENAME  },
   {"APADSHAPE1",                      VAL_APADSHAPE1     },
   {"APADWIDTH",                       VAL_APADWIDTH      },
   {"APADHGHT",                        VAL_APADHGHT       },
   {"APADXOFF",                        VAL_APADXOFF       },
   {"APADYOFF",                        VAL_APADYOFF       },
   {"APADFLASH",                       VAL_APADFLASH      },
   {"APADSHAPENAME",                   VAL_APADSHAPENAME  },
   {"SYMTYPE",                         VAL_SYM_TYPE       },
   {"SYMNAME",                         VAL_SYM_NAME       },
   {"REFDES",                          VAL_REFDES         },  
   {"SYMX",                            VAL_SYM_X          },
   {"SYMY",                            VAL_SYM_Y          },
   {"SYMROTATE",                       VAL_SYM_ROTATE     },
   {"SYMMIRROR",                       VAL_SYM_MIRROR     },
   {"NETNAME",                         VAL_NET_NAME       },  
   {"CLASS",                           VAL_CLASS          },
   {"SUBCLASS",                        VAL_SUBCLASS       },
   {"RECORDTAG",                       VAL_RECORD_TAG     },
   {"GRAPHICDATANAME",                 VAL_GRAPHIC_DATA_NAME},
   {"GRAPHICDATANUMBER",               VAL_GRAPHIC_DATA_NUMBER},
   {"GRAPHICDATA1",                    VAL_GRAPHIC_DATA_1 },
   {"GRAPHICDATA2",                    VAL_GRAPHIC_DATA_2 },
   {"GRAPHICDATA3",                    VAL_GRAPHIC_DATA_3 },
   {"GRAPHICDATA4",                    VAL_GRAPHIC_DATA_4 },
   {"GRAPHICDATA5",                    VAL_GRAPHIC_DATA_5 },
   {"GRAPHICDATA6",                    VAL_GRAPHIC_DATA_6 },
   {"GRAPHICDATA7",                    VAL_GRAPHIC_DATA_7 },
   {"GRAPHICDATA8",                    VAL_GRAPHIC_DATA_8 },
   {"GRAPHICDATA9",                    VAL_GRAPHIC_DATA_9 },
   {"GRAPHICDATA10",                   VAL_GRAPHIC_DATA_10},
   {"COMPDEVICETYPE",                  VAL_COMP_DEVICE_TYPE},
   {"COMPPACKAGE",                     VAL_COMP_PACKAGE   },
   {"COMPPARTNUMBER",                  VAL_COMP_PART_NUMBER},
   {"COMPVALUE",                       VAL_COMP_VALUE     },
   {"VIAX",                            VAL_VIA_X          },
   {"VIAY",                            VAL_VIA_Y          },
   {"PINNUMBERSORT",                   VAL_PIN_NUMBER_SORT},
   {"PADSTACKNAME",                    VAL_PAD_STACK_NAME },
   {"PINNUMBER",                       VAL_PIN_NUMBER     },
   {"PINNAME",                         VAL_PIN_NAME       },
   {"PINX",                            VAL_PIN_X          },
   {"PINY",                            VAL_PIN_Y          },
   {"LAYERUSE",                        VAL_LAYER_USE      },     // continue Layerle
   {"LAYERCONDUCTOR",                  VAL_LAYER_CONDUCTOR},
   {"LAYERDIELECTRICCONSTANT",         VAL_LAYER_DIELECTRIC_CONSTANT},
   {"LAYERELECTRICALCONDUCTIVITY",     VAL_LAYER_ELECTRICAL_CONDUCTIVITY},
   {"LAYERMATERIAL",                   VAL_LAYER_MATERIAL },
   {"LAYERSHIELDLAYER",                VAL_LAYER_SHIELD_LAYER},
   {"LAYERTHERMALCONDUCTIVITY",        VAL_LAYER_THERMAL_CONDUCTIVITY},
   {"LAYERTHICKNESS",                  VAL_LAYER_THICKNESS},
   {"PINROTATION",                     VAL_PIN_ROTATION   },
   {"PINEDITED",                       VAL_PIN_EDITED     },
   {"TESTPOINT",                       VAL_TEST_POINT     },
   {"COMPCLASS",                       VAL_COMP_CLASS     },
   {"NETPROBENUMBER",		            VAL_NET_PROBE_NUMBER},
   {"VIAMIRROR",		                  VAL_VIA_MIRROR     },
// user attributes - user defined and passed through as attributes
// !BARCODE!BOARD_FIDUCIAL!BOARD_TOOLING!BREAKAWAY!CUTOUT!PANEL_FIDUCIAL!PANEL_TOOLING!ROBBER_BAR!
};
#define  SIZ_CMD_ARY (sizeof(RecognizedCommandAry) / sizeof(ValList))

typedef struct
{
   CString tok;
   int label; // this is the index into val_lst. -1 is a known attribute, > 0 is a known label
} Record;
typedef CTypedPtrArray<CPtrArray, Record*> RecordArray;


typedef struct
{
   CString tok;
   CString compressedTok;
   int label;  // this is the index into val_lst. -1 is a known attribute, > 0 is a known label
} Command;
typedef CTypedPtrArray<CPtrArray, Command*> CommandArray;

typedef struct
{
   char     *name;
   int      mirror;
   int      used;
   int      physicalstack;    // 1..all dielectric and electrical
   double   thickness;
   int      typ;              // 0 no type, 1 conductor, 2 dialec    
} ALLEGROLayer;

typedef struct
{
   CString name;        // layer name
   char attr;           // layer attributes as defined in dbutil.h
} ALLEGROAdef;

typedef struct         
{
   CString allegro_name;
   CString cc_name;
} ALLEGROAttrNameMap;
typedef CTypedPtrArray<CPtrArray, ALLEGROAttrNameMap*> ALLEGROAttrNameMapArray;


class CAllegroMaster
{
public:
	CAllegroMaster(CString geomName, CString refName, bool pinEdited=false)
	{
		m_sGeomName = geomName;
		m_sRefName = refName;
		m_bPinEdited = pinEdited;
	}
	~CAllegroMaster()
	{}

private:
	CString m_sGeomName;
	CString m_sRefName;
	bool m_bPinEdited;

public:
	CString GetGeomName() const				{ return m_sGeomName;		}
	CString GetRefName() const					{ return m_sRefName;			}
	bool IsPinEdited() const					{ return m_bPinEdited;		}

	void SetRefName(const CString refName)	{ m_sRefName = refName;		}
	void SetPinEdited(const bool edited)	{ m_bPinEdited = edited;	}
};
typedef CTypedPtrListContainer<CAllegroMaster*> CAllegroMasterList;


// all stored in the attribute list (attr[i])

#define  PANEL_ATT                     1     // graphic with thos att will go into panel file and attrib
                                             // is added to GRAPH_BLOCK_REFERENCE
#define  COMP_ATT                      2     // attibutes are added to GRAPH_BLOCK_REFERENCE
#define  GEOM_ATT                      3     // attibutes are floated up to Geometry from Data
#define  SMD_ATT                       4     // this is an attribute to add to PAD and BLOCK_PCBCOMP
#define  PANEL_SYMBOL                  5     // this symbol is part of panel, not board
#define  BOARD_SYMBOL                  6     // symbol is the actual pcb 
#define  TYPE_ATT                      7     // attributes added to type list
#define  EXPLODE_SYMBOL                8
#define  PANEL_GRAPHIC                 9
#define  CLASS_2_COMPATT               10
#define  MECHANICAL_SYMBOL             11

#define  PART_FIDUCIAL                 12
#define  PART_FIDUCIALTOP              13
#define  PART_FIDUCIALBOT              14
#define  PART_TOOL                     15
#define  PADSTACK_TOOL                 16
#define  PADSTACK_FIDUCIAL             17
#define  PADSTACK_FIDUCIALTOP          18
#define  PADSTACK_FIDUCIALBOT          19

// watch out that is does not conflict with GR_CLASS
#define  COMPHEIGHT_CLASS  -1
#define  REFDES_CLASS      -2
#define  TEXTCOMPATTR_CLASS -3

#define  VAL_NONE          0
#define  VAL_CONNECT       1
#define  VAL_NOTCONNECT    2
#define  VAL_SHAPE         3
#define  VAL_VOID          4
#define  VAL_POLYGON       5

typedef struct
{
   char *name;
   int atttype;
} ALLEGROAttr;

typedef struct
{
   double w, h;
} ALLEGROPadshape;

typedef struct
{
   CString  Class;
   CString  SubClass;
   int      typ;
} ALLEGROPrimout;
typedef CTypedPtrArray<CPtrArray, ALLEGROPrimout*> CPrimArray;

typedef struct
{
   CString  Class;
   CString  SubClass;
   CString  Attribute;
} ALLEGROTextCompAttr;
typedef CTypedPtrArray<CPtrArray, ALLEGROTextCompAttr*> CTextCompAttrArray;

typedef struct
{
   double x;
   double y;
   double rot;
   int mir;
} ALLEGROStepRepeat;
typedef CTypedPtrArray<CPtrArray, ALLEGROStepRepeat*> ALLEGROStepRepeatArray;

typedef struct
{
   double minx, miny;
   double maxx, maxy;
   int    extent_found;
} ALLEGROExtent;

typedef struct         
{
   CString           refname;
   CString           symname;
   CString           devicename;
   CString           usedsymname;      // this is the symname_refdes, because every component gets exploded once
                                    // and later optimized.
   int               pin_edited;
   double            x,y,rot;
   int               mir;
   CMapStringToPtr   pinEditedGeomMap;
} ALLEGROComp;
typedef CTypedPtrArray<CPtrArray, ALLEGROComp*> CompArray;

typedef struct
{
   CString           originalGeomName;
   CMapStringToPtr   pinEditedGeomMap;
} ALLEGROPinEditGeom;
typedef CTypedPtrArray<CPtrArray, ALLEGROPinEditGeom*> CPinEditGeomArray;


//---------------------------------------------------------------------------
// CAllegroRefdes::CAllegroRefdes
//---------------------------------------------------------------------------
class CAllegroRefdes
{
public:
	CAllegroRefdes(CString refdes);
	~CAllegroRefdes();
	
private:
	CString m_refdes;
	CMapStringToString m_uniquerRefdesMap;

public:
	bool GetUniqueRefdesByCoordinate(double x, double y, CString &uniqueRefdes);
	//bool GetDuplicatedRefdesUniqueName(CString refdes, double x, double y, CString &uniqueName);
};


//---------------------------------------------------------------------------
// CAllegroRefdes::CAllegroRefdes
//---------------------------------------------------------------------------
class CAllegroRefdesMap
{
public:
	CAllegroRefdesMap();
	~CAllegroRefdesMap();

private:
	CTypedMapStringToPtrContainer<CAllegroRefdes*> m_refdesMap;

public:
	void Empty();
	bool GetUniqueRefdesByCoordinate(CString refdes, double x, double y, CString &uniqueRefdes);
};
