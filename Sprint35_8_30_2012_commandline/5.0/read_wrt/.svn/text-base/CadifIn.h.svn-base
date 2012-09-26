// $Header: /CAMCAD/4.5/read_wrt/CadifIn.h 33    7/10/06 5:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__CadifIn_h__)
#define __CadifIn_h__

#pragma once

#include "RegularExpression.h"

#undef   DISPLAY_JUNCTIONPOINT
//#define   DISPLAY_JUNCTIONPOINT

#define  CADIF_CADSTAR        1
#define  CADIF_VISULA         2

#define  TEXTCORRECT          0.875
#define  TEXTHEIGHT           0.8
#define  LINESPACING          1.5

// must start with 1
#define  L_UNLAYERED          1
#define  L_ELECTRICAL         2
#define  L_LAMINATE           3
#define  L_SPLIT_PLANE        4
#define  L_SILKSCREEN         5
#define  L_DRILL_DWG          6
#define  L_DRILL_ID           7
#define  L_TOP_RESIST         8
#define  L_BOTTOM_RESIST      9
#define  L_DOCUMENT           10
#define  L_ASSY_DWG           11
#define  L_PROFILING          12
#define  L_PLACEMENT          13
#define  L_NO_TRACKS          14
#define  L_NO_VIAS            15
#define  L_RESERVED           16
#define  L_CLEARANCE          17
#define  L_POWER_PLANE        18
#define  L_PROHIBITED         19

#define  CADIFERR                "cadif.log"
#define  CADIFTEC                "cadif.in"

enum ECadifLeadform
{
	eCadifLeadformUNKNOWN,
	eCadifLeadformTHRU,
	eCadifLeadformSMD
};

#define  FNULL                   fnull

#define  MAX_LINE                30000  /* Max line length.           */
#define  MAX_TOKEN               MAX_LINE

#define  MAX_LAYERS              512   /* Max number of layers.   */
//#define  MAX_PACKAGE             1000
#define  MAX_PINPERCOMP          2000

/* Layer codes.      */
#define  LAY_SIGNAL              1

#define  LAY_ALL_LAYERS          -1
#define  LAY_NULL                -2

#define  STAT_UNKNOWN            0
#define  STAT_PADSTACK           1

#define  FIG_PATHFIGURE          1  // this updates G.cur_filltype
#define  FIG_FILLEDFIGURE        2
#define  FIG_CLEARFIGURE         3
#define  FIG_OPENSHAPE           4
#define  FIG_CUTOUT              5
#define  FIG_BOARD_AREA          6

#define  FILLTYPE_CLEAR          1  // this updates G.cur_filltype too
#define  FILLTYPE_SOLID          2

#define  MAX_ATTR             2000

//#define  FIDUCIAL_ATTR        1
//#define  GENERIC_ATTR         2
//#define  TOOLING_ATTR         3  
//#define  TEST_ATTR            4  

#define  BRK_ROUND            0
#define  BRK_B_ROUND          1

/****************************************************************************
* Structure Section
*/

//_____________________________________________________________________________
typedef struct
{
   int   maj, min;
}CADIF_Version;

//_____________________________________________________________________________
typedef struct
{
   char  *name;
   int   atttype;
}CADIFAttr;

//_____________________________________________________________________________
typedef struct
{
   int   visible;
   int   cur_layerindex;
   double   x,y;
   double rotation; 
   int   mirror;
   double   height,width;
   int length;
}CADIF_Attrib;

//_____________________________________________________________________________
typedef struct
{
   CString  attrib;
   CString  mapattrib;
}CADIFAttribmap;

typedef CTypedPtrArray<CPtrArray, CADIFAttribmap*> CAttribmapArray;

//_____________________________________________________________________________
typedef struct
{
   CString  name;       // cadif layer name
   char  attr;          // layer attributes as defined in dbutil.h
} CADIFAdef;

//_____________________________________________________________________________
class CCadifMirroredLayerPair
{
private:
   CString m_layerName;
   CString m_mirroredLayerName;

public:
   CCadifMirroredLayerPair(const CString& layerName,const CString& mirroredLayerName);

   CString getLayerName() const { return m_layerName; }
   CString getMirroredLayerName() const { return m_mirroredLayerName; }
};

//_____________________________________________________________________________
class CCadifMirroredLayerPairs
{
private:
   CTypedPtrListContainer<CCadifMirroredLayerPair*> m_layerPairs;
   CTypedMapStringToPtrContainer<CCadifMirroredLayerPair*> m_layerPairMap;

public:
   CCadifMirroredLayerPairs();

   bool addLayerPair(const CString& layerName,const CString& mirroredLayerName);

   POSITION getHeadPosition();
   CCadifMirroredLayerPair* getNext(POSITION& pos);
};

//_____________________________________________________________________________
typedef struct
{
   double x,y; // end points
   double bulge;
}CADIFArc;

//_____________________________________________________________________________
typedef struct
{
   double d;
   int    toolindex;
}CADIFDrill;

typedef CArray<CADIFDrill, CADIFDrill&> CDrillArray;

//_____________________________________________________________________________
typedef  struct
{
   CString  net_name;
   CString  signal_name;
   //int   netnr;
   int   routcode;      // routing line width
   char  routcodeset;
   int   neckcode;
   char  neckcodeset;
} CADIFNetl;

typedef CTypedPtrArray<CPtrArray, CADIFNetl*> CNetlArray;

//_____________________________________________________________________________
typedef  struct
{
   CString  compname;
   CString  pinname;       // updated in update_testaccesspin...
   int      pinnr;
   int      testaccess;    //from DbUtuil.h ATT_TEST_ACCESS_TOP ...
} CADIFCompPinTestAccess;

typedef CTypedPtrArray<CPtrArray, CADIFCompPinTestAccess*> CCompPinTestAccessArray;

//_____________________________________________________________________________
typedef struct
{
   CString  fontname;
   double height;
   double orientation;
   char   mirror;
   DbFlag just;   //
} CADIFTextStyle;

typedef CTypedPtrArray<CPtrArray, CADIFTextStyle*> TextStyleArray;

//_____________________________________________________________________________
typedef struct
{
   CString  altname;
   CString  pinname;
} CADIFAltPin;

typedef CTypedPtrArray<CPtrArray, CADIFAltPin*>AltPinArray;

//_____________________________________________________________________________
class CADIFPadstack
{
public:
   CString  pcname;     // PCxxx
   CString  padname;    // this is the namefield of the padcode
   char     used_as_via;
   char     padassign;
   char     paddrill;

public:
   CADIFPadstack(const CString& name);
};

//_____________________________________________________________________________
class CCadifPadstackArray
{
private:
   CTypedPtrArrayWithMap<CADIFPadstack> m_padStacks;

public:
   CCadifPadstackArray();

   void empty();

   //CADIFPadstack* getAt(int index);
	CADIFPadstack* find(const CString& geometryName);
   CADIFPadstack* getDefinedAt(const CString& geometryName);
   CADIFPadstack* getLast();
};

//_____________________________________________________________________________
typedef struct
{
   CString  prtname;          // prt1 ..
   CString  partname;         // namefield of Part
   CString  partdetailname;   // detail of prt1
   int   pacptr;              // pointer into packagename
} CADIFPartName;

typedef CTypedPtrArray<CPtrArray, CADIFPartName*> CPartNameArray;

//_____________________________________________________________________________
class CCadifPin
{
public:
	CCadifPin(CString pinID, CString name)
	{
		m_sPinID = pinID;
		m_sName = name;
		m_iPinNumber = atoi(pinID.Right(pinID.GetLength() - 1));
	}
	~CCadifPin() {}

private:
	CString m_sPinID;
	CString m_sName;
	int m_iPinNumber;

public:
	CString GetPinID() const				{ return m_sPinID;			}
	CString GetName() const					{ return m_sName;				}
	int GetPinNumber() const				{ return m_iPinNumber;		}
};

typedef CTypedMapStringToPtrContainer<CCadifPin*> CCadifPinMap;

//_____________________________________________________________________________
class CCadifPackage
{
public:
	CCadifPackage(CString packageID, CString name)
	{
		m_sPackageID = packageID;
		m_sName = name;
		m_eLeadform = eCadifLeadformSMD;
		m_pinMap.empty();
	}
	~CCadifPackage()
	{
		m_pinMap.empty();
	}

private:
	CString m_sPackageID;
	CString m_sName;
	ECadifLeadform m_eLeadform;
	CCadifPinMap m_pinMap;

public:
	CString GetPackageID() const			{ return m_sPackageID;		}
	CString GetName() const					{ return m_sName;				}
	ECadifLeadform GetLeadform() const	{ return m_eLeadform;		}

	void SetName(const CString name) { m_sName = name;				}
	void SetLeadform(const ECadifLeadform lf)	{ m_eLeadform = lf; }


	CCadifPin* AddPin(CString pinID, CString name);
	CCadifPin* FindPin(CString pinID);
};

//_____________________________________________________________________________
class CCadifPackageMap : public CTypedMapStringToPtrContainer<CCadifPackage*>
{
public:
	CCadifPackageMap()	{ m_lastPackage = NULL; }
	~CCadifPackageMap()	{ this->empty();			}

private:
	CCadifPackage* m_lastPackage;

public:
	CCadifPackage* GetLastPackage()		{ return m_lastPackage;		}

	CCadifPackage* AddPackage(CString packageID, CString name);
	CCadifPackage* FindPackage(CString packageID);
	CCadifPin* FindPackagePin(CString packageID, CString pinID);
};

//_____________________________________________________________________________
class CADIFPackAlt
{
private:
   CString  m_name;          // alt1 ..
   CString  m_realName;      // namefield from name under packalt, defaulted to ALT

public:
   CString  pacname;       // pac1 ..
   int      used;          // used in component placement
   int      already_exist; // a PACKALT with same (name "xx") already defined. Only important if
                           // USE_ALTNAME is set to Y

public:
   CADIFPackAlt(const CString& name,const CString& realname);

   CString getName()     const { return m_name; }
   CString getRealName() const { return m_realName; }
};

//_____________________________________________________________________________
class CCadifPackalts
{
private:
   CTypedMapStringToPtrContainer<CADIFPackAlt*> m_packalts;
   CTypedMapStringToPtrContainer<CADIFPackAlt*> m_realPackAlts;

public:
   CCadifPackalts();
   void empty();

   CADIFPackAlt* getPackalt(const CString& pacaltName,bool usePackaltName=true);
   CADIFPackAlt* add(const CString& name,const CString& realName);
   POSITION GetStartPosition();
   CADIFPackAlt* GetNextAssoc(POSITION& pos);
};

//_____________________________________________________________________________
class CCadifPadstackGeometries
{
private:
   CTypedMapStringToPtrContainer<BlockStruct*> m_geometries;

public:
   CCadifPadstackGeometries(int blockSize);

   BlockStruct* getMirroredGeometry(BlockStruct* geometry);
};

//_____________________________________________________________________________
class CCadifCompGeometries
{
private:
   CTypedMapStringToPtrContainer<BlockStruct*> m_geometries;
	CMapStringToString m_drilledGeomNames;
	
public:
   CCadifCompGeometries(int blockSize);

	bool isGeometryDrilled(BlockStruct* geometry, CCadifPadstackArray& padstackarray);
   BlockStruct* getMirroredGeometry(BlockStruct* geometry,
      CCadifPadstackGeometries& padstackGeometries);
};

//_____________________________________________________________________________
class CADIFCompInst
{
private:
   DataStruct& m_compInstance;

public:
   CString  cname;         // C%d
   CString  compname;      // name
   CString  altname;       // need this to get to shapenamearray to get to pinnames.
   CString  type;
   CString  value;

public:
   CADIFCompInst(DataStruct& compInstance);

   DataStruct& getCompInstance() const { return m_compInstance; }
};

typedef CTypedPtrArray<CPtrArray,CADIFCompInst*> CCompInstArray;

//_____________________________________________________________________________
typedef struct
{
   CString  name;          // L1..
   int      id;            // layer id.
   int      layerref;      // layerref is -1 if a normal layer, otherwise it is the layerref in a global section.
   CString  username;
   DbFlag   flg;
   int      ppsignalcnt;
   int      ppsignal[10];  // powerplane signal number to mark
                           // plane signals
   int      usage;
}CADIFLayerlist;

//_____________________________________________________________________________
typedef  struct
{
   char     *token;
   int      (*function)();
} List;

//_____________________________________________________________________________
typedef  struct   // can not have CString because of menset.
{
   int      cur_id;
   int      cur_status;
   
   /* Current values that are changed with global  */
   int      cur_layerindex;               /* Layer index */
   int      cur_widthindex;               /* Line width     */
   double   cur_width;
   double   cur_height;                   /* Line height     */
   double   cur_left, cur_right;
   
   /* Current Component info used for Instance.    */
   double   cur_pos_x;                    /* position.   */
   double   cur_pos_y;
   double   cur_rotation;                 /* Rotation.   */
   char     cur_mirror;                   /* Current mirror.   */
   char     cur_bottom;
   
   /* Current pad stack info.    */
   int      cur_pincnt;                   // counts number of pins on a packaltdef
   
   /* Pad stacks */
   int      cur_textmirror;
   int      cur_textjust;
   int      cur_textrot;
   
   char     cur_type;                     // used by layer and padshape
   char     cur_filltype;                 // CLEAR or not
   char     cur_figtype;
   char     first_arc;                    // help flag for
                                          /*
                                            (shape
                                             (arc (e -6373558 -5) (pt -53561 -12500))
                                             (pt -80000 -12500)
                                             (pt -80000 -80000)
                                             (pt -12500 -80000)
                                             (pt -12500 -53561)))
                                          */
   char     name[80];                     // from cadif_name routine.
   int      electrical_stackup;
} Global;

//=============================================================================

//_____________________________________________________________________________
class CCadifAttributeVisibilityMap
{
private:
   CMapStringToString m_attributeMap;

public:
   CCadifAttributeVisibilityMap();

   void setVisibility(CString attributeName,const CString& visibilityValue);
   Bool3Tag getVisible(CString attributeName) const;
};

//_____________________________________________________________________________
class CCadifPcbComponentGeometryMap
{
private:
   CTypedMapStringToPtrContainer<BlockStruct*> m_pcbComponentGeometryMap;
   CMapStringToString m_nonPreferredGeometryNameMap;

public:
   CCadifPcbComponentGeometryMap();

   CMapStringToString& getNonPreferredGeometryNameMap() { return m_nonPreferredGeometryNameMap; }

   void addGeometry(BlockStruct& pcbGeometry);
   void renameGeometry(BlockStruct& pcbGeometry,const CString& newName);
   void fixGeometryNames(CCEtoODBDoc& camCadDoc);
   BlockStruct* lookup(const CString& geometryName);
};

//_____________________________________________________________________________
class CCadifReader
{
private:
   CCEtoODBDoc& m_camCadDoc;
   FileStruct* m_pcbFile;
   CCadifAttributeVisibilityMap m_attributeVisibilityMap;
   CCadifPcbComponentGeometryMap m_pcbGeometryMap;
   int m_startingBlockIndex;
   CCadifMirroredLayerPairs m_cadifMirroredLayerPairs;
   CRegularExpressionList m_fiducialExpressionList;
   CRegularExpressionList m_testPointExpressionList;

   bool m_optionFixGeometryNamesFlag;
   bool m_optionMergeFixedGeometriesFlag;

public:
   CCadifReader(CCEtoODBDoc& camCadDoc);

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   void setPcbFile(FileStruct* pcbFile) { m_pcbFile = pcbFile; }

   CCadifAttributeVisibilityMap& getAttributeVisibilityMap() { return m_attributeVisibilityMap; }
   CCadifPcbComponentGeometryMap& getPcbGeometryMap() { return  m_pcbGeometryMap; }
   CCadifMirroredLayerPairs& getCadifMirroredLayerPairs() { return m_cadifMirroredLayerPairs; }

   bool getOptionFixGeometryNamesFlag() const { return m_optionFixGeometryNamesFlag; }
   void setOptionFixGeometryNamesFlag(bool flag) { m_optionFixGeometryNamesFlag = flag; }

   bool getOptionMergeFixedGeometriesFlag() const { return m_optionMergeFixedGeometriesFlag; }
   void setOptionMergeFixedGeometriesFlag(bool flag) { m_optionMergeFixedGeometriesFlag = flag; }

   void setAttributeVisibility(CAttributes& attributes,const CString& attributeName);

   void addFiducialExpression(const CString& regularExpressionString);
   bool isFiducialEntityName(const CString& entityName);

   void addTestPointExpression(const CString& regularExpressionString);
   bool isTestPointEntityName(const CString& entityName);

   void setStartingBlockIndex(int blockIndex) { m_startingBlockIndex = blockIndex; }
   void fixGeometryNames();
};

/****************************************************************************
* List Section
*/

#define  SIZ_BRK_LST                (sizeof(brk_lst)              / sizeof(List))
#define  SIZ_START_LST              (sizeof(start_lst)            / sizeof(List))
#define  SIZ_CADIF_LST              (sizeof(cadif_lst)            / sizeof(List))
#define  SIZ_SIGNALLIST_LST         (sizeof(signallist_lst)       / sizeof(List))
#define  SIZ_PT_LST                 (sizeof(pt_lst)               / sizeof(List))
#define  SIZ_ARC_LST                (sizeof(arc_lst)              / sizeof(List))
#define  SIZ_SCALE_LST              (sizeof(scale_lst)            / sizeof(List))
#define  SIZ_SHAPEPT_LST            (sizeof(shapept_lst)          / sizeof(List))
#define  SIZ_ATTRIB_LST             (sizeof(attrib_lst)           / sizeof(List))
#define  SIZ_ATTRIB_DISP_LST        (sizeof(attrib_disp_lst)      / sizeof(List))
#define  SIZ_LAYERREF_LST           (sizeof(layerref_lst)         / sizeof(List))
#define  SIZ_SIGNAL_LST             (sizeof(signal_lst)           / sizeof(List))
#define  SIZ_TEXTINFO_LST           (sizeof(textinfo_lst)         / sizeof(List))
#define  SIZ_TECHNOLOGY_LST         (sizeof(technology_lst)       / sizeof(List))
#define  SIZ_TECHDESC_LST           (sizeof(techdesc_lst)         / sizeof(List))
#define  SIZ_LAYERSTACK_LST         (sizeof(layerstack_lst)       / sizeof(List))
#define  SIZ_GLOBALLAYERSTACK_LST   (sizeof(globallayerstack_lst) / sizeof(List))
#define  SIZ_TREESPACING_LST        (sizeof(treespacing_lst)      / sizeof(List))
#define  SIZ_LAYERSTACKLAYER_LST    (sizeof(layerstacklayer_lst)  / sizeof(List))
#define  SIZ_LAYERDESC_LST          (sizeof(layerdesc_lst)        / sizeof(List))
#define  SIZ_TEXTSTYLE_LST          (sizeof(textstyle_lst)        / sizeof(List))
#define  SIZ_PADCODELIB_LST         (sizeof(padcodelib_lst)       / sizeof(List))
#define  SIZ_PADCODEDESC_LST        (sizeof(padcodedesc_lst)      / sizeof(List))
#define  SIZ_DRILL_LST              (sizeof(drill_lst)            / sizeof(List))
#define  SIZ_PACKAGE_LST            (sizeof(package_lst)          / sizeof(List))
#define  SIZ_PACKDESC_LST           (sizeof(packdesc_lst)         / sizeof(List))
#define  SIZ_PACKTERM_LST           (sizeof(packterm_lst)         / sizeof(List))
#define  SIZ_PART_LST               (sizeof(part_lst)             / sizeof(List))
#define  SIZ_COMPPIN_LST            (sizeof(comppin_lst)          / sizeof(List))
#define  SIZ_PARTDESC_LST           (sizeof(partdesc_lst)         / sizeof(List))
#define  SIZ_COMPONENT_LST          (sizeof(component_lst)        / sizeof(List))
#define  SIZ_WILDCARDNET_LST        (sizeof(wildcardnet_lst)      / sizeof(List))
#define  SIZ_SWAPLAYER_LST          (sizeof(swaplayer_lst)        / sizeof(List))
#define  SIZ_LOCATION_LST           (sizeof(location_lst)         / sizeof(List))
#define  SIZ_PACKALTDESC_LST        (sizeof(packaltdesc_lst)      / sizeof(List))
#define  SIZ_SYMBOL_LST             (sizeof(symbol_lst)           / sizeof(List))
#define  SIZ_SYMDATA_LST            (sizeof(symdata_lst)          / sizeof(List))
#define  SIZ_INSERTDATA_LST         (sizeof(insertdata_lst)       / sizeof(List))
#define  SIZ_PACKALTTERM_LST        (sizeof(packaltterm_lst)      / sizeof(List))
#define  SIZ_PACKALT_LST            (sizeof(packalt_lst)          / sizeof(List))
#define  SIZ_COMPDEFN_LST           (sizeof(compdefn_lst)         / sizeof(List))
#define  SIZ_PADSHAPE_LST           (sizeof(padshape_lst)         / sizeof(List))
#define  SIZ_PADCODE_LST            (sizeof(padcode_lst)          / sizeof(List))
#define  SIZ_NET_LST                (sizeof(net_lst)              / sizeof(List))
#define  SIZ_KEEPOUT_LST            (sizeof(keepout_lst)          / sizeof(List))
#define  SIZ_PADUSR_LST             (sizeof(padusr_lst)           / sizeof(List))
#define  SIZ_PADNORMAL_LST          (sizeof(padnormal_lst)        / sizeof(List))
#define  SIZ_PADSYMBOL_LST          (sizeof(padsymbol_lst)        / sizeof(List))
#define  SIZ_ANNO_LST               (sizeof(anno_lst)             / sizeof(List))
#define  SIZ_COPPER_LST             (sizeof(copper_lst)           / sizeof(List))
#define  SIZ_ANNOHATCH_LST          (sizeof(annohatch_lst)        / sizeof(List))
#define  SIZ_COPHATCH_LST           (sizeof(cophatch_lst)         / sizeof(List))
#define  SIZ_ANNOSYMBOL_LST         (sizeof(annosymbol_lst)       / sizeof(List))
#define  SIZ_AREA_LST               (sizeof(area_lst)             / sizeof(List))
#define  SIZ_SYMFIG_LST             (sizeof(symfig_lst)           / sizeof(List))
#define  SIZ_SYMLABEL_LST           (sizeof(symLabel_lst)         / sizeof(List))
#define  SIZ_SYMTEX_LST             (sizeof(symtex_lst)           / sizeof(List))
#define  SIZ_TEXT_LST               (sizeof(text_lst)             / sizeof(List))
#define  SIZ_VIA_LST                (sizeof(via_lst)              / sizeof(List))
#define  SIZ_TESTPT_LST             (sizeof(testpt_lst)           / sizeof(List))
#define  SIZ_PATHFIGURE_LST         (sizeof(pathfigure_lst)       / sizeof(List))
#define  SIZ_ROUTE_LST              (sizeof(route_lst)            / sizeof(List))
#define  SIZ_NETJOINS_LST           (sizeof(netjoins_lst)         / sizeof(List))
#define  SIZ_COMPREF_LST            (sizeof(compref_lst)          / sizeof(List))
#define  SIZ_DESIGN_LST             (sizeof(design_lst)           / sizeof(List))
#define  SIZ_FILEINFO_LST           (sizeof(fileinfo_lst)         / sizeof(List))
#define  SIZ_DEFCOMPPOSN_LST        (sizeof(defcompposn_lst)      / sizeof(List))

static int fnull();     // this writes to log file
static int fskip();     // fskip does not write to log file

/* Start of CADIF file.  */
static int start_cadif();

static List start_lst[] =
{
   "cadif",                  start_cadif,
};

/* Bracket definition */
static List brk_lst[] =
{
   "(",     FNULL,
   ")",     FNULL,
};

/* CADIF section  */
static int cadif_format();
static int cadif_design();
static int cadif_fileinfo();
static int cadif_attribute();

static List cadif_lst[] =
{
   "format",               cadif_format,
   "design",               cadif_design,
   "fileInfo",             cadif_fileinfo,
};

static int cadif_pt();
static int cadif_arc();
static List shapept_lst[] =
{
   "pt",                   cadif_pt,
   "arc",                  cadif_arc,
};

static List pt_lst[] =
{
   "pt",                   cadif_pt,
};

/* signallist section */
static int signallist_signal();
static int signal_attribute();

static List signallist_lst[] =
{
   "signal",               signallist_signal,
};

/* Arc section */
static int arc_pt();
static int arc_bulge();

static List arc_lst[] =
{
   "pt",                   arc_pt,
   "e",                    arc_bulge,
};

/* Scale section */
static List scale_lst[] =
{
   "e",                    arc_bulge,
};

static int defcompposn_pt();

static List defcompposn_lst[] =
{
   "pt",                   defcompposn_pt,
};

/* Attrib section */
static int attrib_visible();
static int attrib_disp();

static List attrib_lst[] =
{
   "visible",              attrib_visible,
   "fixed",                fskip,
   "attrDisp",             attrib_disp,
};

static int attrib_layerref();
static int attrib_position();

static List attrib_disp_lst[] =
{
   "layerRef",             attrib_layerref,
   "position",             attrib_position,
   "box",                  fskip,
};

/* Layerreg section */
static int layerref();
static int eleclayers();
static int alllayers();

static List layerref_lst[] =
{
   "layerRef",               layerref,
   "elecLayers",             eleclayers,
   "allLayers",              alllayers,
};

/* signal section */
static int cadif_name();
static int cadif_powersignal();
static int cadif_height();

static List signal_lst[] =
{
   "name",               cadif_name,
   "powersignal",        cadif_powersignal,
   "spurMiter",          fskip,
   "guardSpace",         fskip,
   "preDefined",         fskip,  // Version 5
   "attribute",          signal_attribute,
};

/* Textinfo section */
static int font_textstyle();

static List textinfo_lst[] =
{
   "fontSymbol",           fskip,
   "font",                 fskip,     
   "textStyle",            font_textstyle,
};

/* Technology section */
static int technology_padcodelib();
static int technology_techdesc();
static int technology_layerstack();
static int technology_globallayerstack();

static List technology_lst[] =
{
   "name",                 cadif_name,
   "materialLib",          fskip,
   "layerStack",           technology_layerstack,
   "globalLayers",         technology_globallayerstack,
   "padCodeLib",           technology_padcodelib,
   "techDesc",             technology_techdesc,
};

/* Techdesc section */
static int topeleclayer();
static int boteleclayer();
static int topsurflayer();
static int botsurflayer();
static int treespacing();
static int systemgrid();
static int optspacing();
static int wildcardnet();
static int swaplayer();

static List  techdesc_lst[] =
{
   "boardTech",            fskip,
   "packTech",             fskip,
   "padTech",              fskip,
   "techRules",            fskip,
   "swapLayerList",        swaplayer,
   "drillLayerList",       fskip,
   "topSurfLayer",         topsurflayer,
   "botSurfLayer",         botsurflayer,
   "topElecLayer",         topeleclayer,
   "botElecLayer",         boteleclayer,
   "plcCat",               fskip,
   "decouple",             fskip,
   "powerTree",            fskip,
   "treeSpacing",          treespacing,
   "packAltClass",         fskip,
   "compSpace",            fskip,
   "optSpacing",           fskip,
   "termNetwork",          fskip,
   "netTerminate",         fskip,
   "spacingClass",         fskip,
   "viasUnderCmp",         fskip,
   "tracksUnderCmp",       fskip,
   "systemGrid",           systemgrid,
   "trackGrid",            fskip,
   "viaGrid",              fskip,
   "defAlternate",         fskip,
   "altSwap",              fskip,
   "wildcardNet",          wildcardnet,
   "netClass",             fskip,
};

/* Layerstack section */
static int layerstack_layer();

static List layerstack_lst[] =
{
   "layer",                layerstack_layer,
};

/* Globallayerstack section */
static int globallayerstack_layer();

static List globallayerstack_lst[] =
{
   "glbLayer",             globallayerstack_layer,
};

/* Treespacing section */
static int padtopad();
static int padtotrack();
static int tracktotrack();

static List treespacing_lst[] =
{
   "allLayers",                  fskip,   // need to doit for all layers only
   "layerRef",                   fskip,
   "padToPad",                   padtopad,
   "padToVia",                   fskip,
   "padToTrack",                 padtotrack,
   "padToProf",                  fskip,
   "viaToVia",                   fskip,
   "viaToTrack",                 fskip,
   "viaToProf",                  fskip,
   "trackToTrack",               tracktotrack,
   "trackToProf",                fskip,
};

/* Layerstacklayer section */
static int layerdesc();
static int cadif_id();
static int layerstacklayer_layerref();
static int layerusage();

static List layerstacklayer_lst[] =
{
   "name",                 cadif_name,  
   "id",                   cadif_id,
   "layerDesc",            layerdesc,
   "layerRef",             layerstacklayer_layerref,  // only on global layers -> why ????
   "layerUsage",           layerusage,                // only on global layers -> why ????
   "allows",               fskip,                     // only on global layers -> why ????
   "ppSignal",             fskip,                     // 
};

/* Layerdesc section */
static int layerppsignal();
static int layervisible();

static List layerdesc_lst[] =
{
   "layerUsage",           layerusage,
   "allows",               fskip,
   "routeBias",            fskip,
   "visible",              layervisible,
   "zPosition",            fskip,
   "materialRef",          fskip,
   "ppSignal",             layerppsignal, // plane signal: need to update
   "thickness",            fskip,         // need to update in db
   "attribute",            cadif_attribute,
};

/* Textstyle section */
static   int   cadif_orientation();
static   int   textstyle_justify();
static   int   cadif_mirrored();

static List  textstyle_lst[] =
{
   "fontRef",              fskip,
   "height",               cadif_height,
   "justify",              textstyle_justify,
   "orientation",          cadif_orientation,
   "mirrored",             cadif_mirrored,
};

/* Padecode section */
static int padshape(), padcode();
static int padsymbol();

static List padcodelib_lst[] =
{
   "padShape",             padshape,
   "tearDrop",             fskip,
   "padSymbol",            padsymbol,  //  need to find out, what this is for.
   "padCode",              padcode,
};

/* Padcodedesc section */
static int padassign();
static int paddrill();

static List padcodedesc_lst[] =
{
   "padAssign",            padassign,
   "padStack",             fskip,
   "drill",                paddrill,
   "tdAssign",             fskip,
};

/* Drill section */
static int drillsize();
static int drillplated();

static List drill_lst[] =
{
   "name",                 fskip,   // always skip the name, otherwise is messes up the padname
   "drillSize",            drillsize,
   "drillLetter",          fskip,
   "drillSymbol",          fskip,
   "plated",               drillplated,
};

/* Package section */
static int packdesc();

static List package_lst[] =
{
   "name",                 cadif_name,
   "packDesc",             packdesc,
};

/* Packdesc section */
static int packdesc_packterm();
static int packdesc_leadform();


static List packdesc_lst[] =
{
   "symName",              fskip,
   "leadForm",             packdesc_leadform,
   "packTerm",             packdesc_packterm,
};

/* Packterm section */
static List packterm_lst[] =
{
   "name",                 cadif_name,
   "id",                   fskip,
};

/* Part section */
static int partdesc();
static int part_attribute();

static List part_lst[] =
{
   "name",                 cadif_name,
   "partDesc",             partdesc,
   "attribute",            part_attribute,
};

/* Comppin section */
static int comppin_padcoderef();
static int comppin_testside();

static List comppin_lst[] =
{
   "id",                   cadif_id,
   "pinSwap",              fskip,
   "padCodeRef",           comppin_padcoderef,
   "checkingErrors",       fskip,
   "testSide",             comppin_testside,
   "attribute",            fskip,
   "checkingErrors",       fskip,
   "compCopper",           fskip,
};

/* Partdesc section */
static int packref();
static int partdes_description();
static int partdes_detailname();
static int partdes_prefix();

static List partdesc_lst[] =
{
   "name",                 cadif_name,
   "description",          partdes_description,
   "detailname",           partdes_detailname,
   "prefix",               partdes_prefix,
   "packRef",              packref,
   "catRef",               fskip,
   "subElem",              fskip,
   "viaRestrict",          fskip,
   "trackRestrict",        fskip,
   "polarized",            fskip,
   "swappable",            fskip,
   "partTerm",             fskip,
   "padSuppress",          fskip,
   "thermJntThick",        fskip,
};

/* Component section */
static int compdefn();
static int comp_location();
static int cadif_comppin();

static List component_lst[] =
{
   "name",                 cadif_name,
   "compDefn",             compdefn,
   "location",             comp_location,
   "box",                  fskip,
   "checkingErrors",       fskip,
   "viaRestrict",          fskip,
   "trackRestrict",        fskip,
   "compPin",              cadif_comppin,
   "attribute",            cadif_attribute,
   "compCopper",           fskip,
   "groupRef",             fskip,
   "jumperComp",           fskip,
};

/* Wildcardnet section */
static int viapadcode();
static int wcnwidth();

static List wildcardnet_lst[] =
{
   "name",                 cadif_name,
   "id",                   fskip,
   "viaPadCode",           viapadcode,
   "wcnWidth",             wcnwidth,
   "minSpur",              fskip,
   "guardSpace",           fskip,
   "wcnAttr",              fskip,
};

/* Swaplayer section */
static   int   swaplayer_swaplayer();

static List  swaplayer_lst[] =
{
   "swaplayer",            swaplayer_swaplayer,
};

/* Location section */
static int cadif_position();
static int cadif_side();

static List location_lst[] =
{
   "position",             cadif_position,
   "fixed",                fskip,
   "orientation",          cadif_orientation,
   "side",                 cadif_side,
   "mirrored",             cadif_mirrored,
   "zone",                 fskip,
};

/* Packaltdesc section */
static int packaltterm();
static int symdata();
static int insertdata();

static List packaltdesc_lst[] =
{
   "box",                  fskip,
   "insertData",           insertdata,
   "symData",              symdata,
   "symSilk",              fskip,
   "packAltTerm",          packaltterm,
   "thermalBox",           fskip,
   "symVra",               fskip,
   "symPlace",             fskip,
   "symProfile",           fskip,
   "symTra",               fskip,
   "symClear",             fskip,
};

/* Symbol section */
static int symdesc();

static List symbol_lst[] =
{
   "symName",              fskip,
   "alternate",            fskip,
   "symDesc",              symdesc,
};

/* Symdata section */
static int symfig();
static int symLabel();
static int symtext();

static List symdata_lst[] =
{
   "stdSymbol",            fskip,
   "symFig",               symfig,
   "symLabel",             symLabel,
   "symText",              symtext,
};

/* symLabel section */
static int cadif_string();
static int symlayerref();

static List symLabel_lst[] =
{
   "string",               cadif_string,
   "symLayer",             symlayerref,
   "position",             cadif_position,
};

/* Insertdata section */
static int insertheight();

static List insertdata_lst[] =
{
   "insertHeight",         insertheight,
   "insertBox",            fskip,
   "insertSpan",           fskip,
   "insertPoints",         fskip,
};

/* Pactaltterm section */
static int cadif_padcoderef();

static List packaltterm_lst[] =
{
   "id",                   fskip,
   "position",             cadif_position,
   "labelPosn",            fskip,
   "padCodeRef",           cadif_padcoderef,
   "exitDirn",             fskip,
};

/* Packalt section */
static   int   packalt_packref();
static   int   packaltdesc();

static List  packalt_lst[] =
{
   "name",                 cadif_name,
   "packRef",              packalt_packref,
   "packAltDesc",          packaltdesc,
};

/* Compodefn section */
static   int   partref();     // cur_libname
static   int   packaltref();  //

static List  compdefn_lst[] =
{
   "partRef",              partref,
   "packAltRef",           packaltref,
};

/* padShapeDef section */
static int padform_octgon();
static int padform_round();
static int padform_square();
static int padform_oblong();
static int padform_annulr();
static int padform_usrdef();
static int padform_finger();
static int padform_bullet();
static int padform_copper();

static List padshape_lst[] =
{
   "padRound",           padform_round,
   "padoctgon",          padform_octgon,
   "padDiamnd",          padform_round,
   "padAnnulr",          padform_annulr,
   "padSquare",          padform_square,
   "padOblong",          padform_oblong,
   "padUsrDef",          padform_usrdef,
   "padFinger",          padform_finger,
   "padBullet",          padform_bullet,
   "padCopper",          padform_copper,
};

/* padCode section*/
static int padcodedesc();

static List padcode_lst[] =
{
   "name",               cadif_name,
   "padCodeDesc",        padcodedesc,
};

/* net section */
static int signalref();
static int signalvia();
static int signalnetjoins();
static int signalroute();
static int signaltestpt();
static int signaljuncpt();

static List net_lst[] =
{
   "signalRef",            signalref,
   "netJoins",             signalnetjoins,
   "via",                  signalvia,
   "route",                signalroute,
   "juncPt",               signaljuncpt,
   "testPt",               signaltestpt,
   "connection",           fskip,
   "padTaper",             fskip,
};

/* Keepout section */
static int cadif_pathfigure();
static int cadif_rectangle();
static int cadif_filledfigure();
static int cadif_layerlist();
static int cadif_boundary();

static List keepout_lst[] =
{
   "layerList",            cadif_layerlist, // need to do
   "layerRef",             layerref,
   "apply",                fskip,   // need to do.
   "active",               fskip ,
   "groupRef",             fskip ,
   "pathfigure",           cadif_pathfigure,
   "filledFigure",         cadif_filledfigure,
};

/* Padurs section */
static int padusr_padnormal();

static List padusr_lst[] =
{
   "padNormal",            padusr_padnormal,
   "padConnect",           fskip,
   "padIsolate",           fskip,
   "padIsoSup",            fskip,
};

/* Padnormal section */
static int padsymref();

static List padnormal_lst[] =
{
   "padBack",              fskip,
   "padSymRef",            padsymref,
   "filledFigure",         cadif_filledfigure,
   "pathFigure",           cadif_pathfigure,
};

/* Padsymbol section */
static List padsymbol_lst[] =
{
   "generated",            fskip,
   "symName",              fskip,
   "filledFigure",         cadif_filledfigure,
   "pathFigure",           cadif_pathfigure,
};

/* Anno section */
static List  anno_lst[] =
{
   "layerRef",             layerref,
   "pathFigure",           cadif_pathfigure,
   "filledFigure",         cadif_filledfigure,
   "groupRef",             fskip,
};

/* Copper section */
static int cadif_planetype();

static List copper_lst[] =
{
   "layerRef",             layerref,
   "filledFigure",         cadif_filledfigure,
   "pathFigure",           cadif_pathfigure,
   "signalRef",            signalref,
   "partialPpRef",         fskip,   // partial powerplane
   "groupRef",             fskip,
   "checkingErrors",       fskip,
   "fixed",                fskip,
   "planeType",            cadif_planetype,
};

/* Annohatch section */
static int cadif_width();

static List annohatch_lst[] =
{
   "layerRef",             layerref,
   "pitch",                fskip,
   "orientation",          fskip,
   "position",             fskip,
   "hatch",                fskip,
   "boundary",             cadif_boundary,
   "width",                cadif_width,
   "groupRef",             fskip,
   "lineStyle",            fskip,
};

/* Cophatch section */
static List  cophatch_lst[] =
{
   "layerRef",             layerref,
   "pitch",                fskip,
   "orientation",          fskip,
   "position",             fskip,
   "hatch",                fskip,   // need to do
   "xhatch",               fskip,   // need to do
   "boundary",             cadif_boundary,
   "boundwidth",           cadif_width,
   "width",                cadif_width,
   "groupRef",             fskip,
   "lineStyle",            fskip,
   "signalRef",            signalref,
};

/* Annosymbol section */
static int symbolref();
static int cadif_scale();

static List annosymbol_lst[] =
{
   "layerRef",             layerref,
   "position",             cadif_position,
   "orientation",          cadif_orientation,
   "scale",                cadif_scale,
   "symbolRef",            symbolref,
   "box",                  fskip,
   "mirrored",             cadif_mirrored,
   "groupRef",             fskip,
};

/* Area section */
static int cadif_area_type();

static List area_lst[] =
{
   "name",                 cadif_name,
   "areaType",             cadif_area_type,
   "areaSide",             fskip,
   "areaWeight",           fskip,
   "areaHeights",          fskip,
   "areaComps",            fskip,
   "fixed",                fskip,
   "keepin",               fskip,
   "filledFigure",         cadif_filledfigure,
   "checkingErrors",       fskip,
   "hideText",             fskip,
};

/* Symfig section */
static List symfig_lst[] =
{
   "symlayer",             symlayerref,
   "pathFigure",           cadif_pathfigure,
   "filledFigure",         cadif_filledfigure,
};

/* Symtex section */
static List symtex_lst[] =
{
   "symlayer",             symlayerref,
   "string",               cadif_string,
   "position",             cadif_position,
   "box",                  fskip,
   "groupRef",             fskip,
};

/* Text section */
static List text_lst[] =
{
   "string",               cadif_string,
   "position",             cadif_position,
   "layerRef",             layerref,
   "box",                  fskip,
   "groupRef",             fskip,
};

/* Via section */
static int cadif_layerrange();

static List via_lst[] =
{
   "position",             cadif_position,
   "padCodeRef",           cadif_padcoderef,
   "layerRange",           cadif_layerrange,
   "fixed",                fskip,
   "dropVia",              fskip,
   "groupRef",             fskip,
   "checkingErrors",       fskip,
};

/* Testpt section */
static int cadif_testside();

static List testpt_lst[] =
{
   "position",             cadif_position,
   "padCodeRef",           cadif_padcoderef,
   "layerRange",           cadif_layerrange,
   "testSide",             cadif_testside,
   "fixed",                fskip,
};

/* Pathfigure section */
static int cadif_polygon();
static int cadif_circle();
static int cadif_shape();
static int cadif_filltype();
static int cadif_cutout();
static int cadif_openshape();
static int cadif_path();
static int cadif_island();

static List pathfigure_lst[] =
{
   "path",                 cadif_path,
   "width",                cadif_width,
   "openShape",            cadif_openshape,
   "rectangle",            cadif_rectangle,
   "polygon",              cadif_polygon,
   "circle",               cadif_circle,
   "cutout",               cadif_cutout,  // need to be done
   "fillType",             cadif_filltype,
   "shape",                cadif_shape,
   "lineStyle",            fskip,
   "island",               cadif_island,
};

/* Route section */
static List route_lst[] =
{
   "joins",                fskip,
   "layerRef",             layerref,
   "pathfigure",           cadif_pathfigure,
   "fixed",                fskip,
   "startTaper",           fskip,
   "endTaper",             fskip,
   "groupRef",             fskip,
   "checkingErrors",       fskip,
};

/* Netjoins section */
static int comppinref();

static List netjoins_lst[] =
{
   "compPinRef",             comppinref,
};

/* Compref section */
static int compref();

static List compref_lst[] =
{
   "compRef",             compref,
};

/* design section */
static int design_signallist();
static int design_textinfo();
static int design_technology();
static int design_packalt();
static int design_package();
static int design_component();
static int design_part();
static int design_symbol();
static int design_net();
static int design_anno();
static int design_annohatch();
static int design_annosymbol();
static int design_copper();
static int design_cophatch();
static int design_constr();
static int design_keepout();
static int design_text();
static int design_area();
static int design_bareboard();
static int design_plane();
static int design_defcompposn();

static List design_lst[] =
{
   "dataSet",           fskip,
   "paper",             fskip,
   "signalList",        design_signallist,
   "textInfo",          design_textinfo,
   "zoneInfo",          fskip,
   "styleInfo",         fskip,
   "technology",        design_technology,
   "attrLib",           fskip,
   "designInfo",        fskip,
   "group",             fskip,
   "layerSettings",     fskip,
   "displaySettings",   fskip,
   "screenSettings",    fskip,
   "colorList",         fskip,
   "colorSettings",     fskip,
   "unit",              fskip,
   "opUnits",           fskip,
   "ipUnits",           fskip,
   "matrix",            fskip,
   "topmatrix",         fskip,
   "keepout",           design_keepout,   
   "symbol",            design_symbol,
   "partialPp",         fskip,   // Partial Plane ??
   "template",          fskip,   // ???
   "package",           design_package,
   "packAlt",           design_packalt,
   "elem",              fskip,
   "category",          fskip,
   "part",              design_part,   // this is device (typelist)
   "defCompPosn",       design_defcompposn,  // Case 1500
   "glbRouteArea",      fskip,
   "component",         design_component,
   "area",              design_area,
   "bareBoard",         design_bareboard, // marks outline
   "curRouteArea",      fskip,
   "topPlaceArea",      fskip,
   "botPlaceArea",      fskip,
   "plane",             design_plane,
   "sppOutline",        fskip,
   "copper",            design_copper,  
   "copHatch",          design_cophatch,  
   "net",               design_net,
   "annoFig",           design_anno,
   "annoHatch",         design_annohatch,    
   "annoSymbol",        design_annosymbol,  
   "constrPoint",       fskip,   
   "text",              design_text,
   "coppertext",        design_text,
   "constrFig",         design_constr,
   "dimParams",         fskip,
   "leaderDim",         fskip,
   "linearDim",         fskip,
   "angleDim",          fskip,
   "ppHatch",           fskip,
   "errorList",         fskip,
};

/* Fileinfo section */
static   int   fileinfo_application();

static List  fileinfo_lst[] =
{
   "jobFile",           fskip,   
   "jobName",           fskip,   
   "jobDesc",           fskip,   
   "timeStamp",         fskip,   
   "createTime",        fskip,   
   "lastEditTime",      fskip,   
   "elapsedTime",       fskip,   
   "cadifTime",         fskip,   
   "progName",          fskip,   
   "fileCheck",         fskip,   
   "saveInfo",          fskip,   
   "designChanges",     fskip,   
   "application",       fileinfo_application,
};

#endif
