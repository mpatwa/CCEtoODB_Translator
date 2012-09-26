// $Header: /CAMCAD/4.4/read_wrt/OrcltdIn.h 26    8/13/04 6:23p Sadek Noureddine $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

static   double                  TEXTCORRECT  = 0.677;
class CCamCadDatabase;

/* Define Section *********************************************************/

#define SHAPENAME						"SHAPENAME"

#define MAX_LINE                	512   /* Max line length. Infopoints can be very long.  */
#define MAX_LAYERS              	255   /* Max number of layers.   */
#define MAX_POLY                	2000
#define MAX_PINPERCOMP          	4000  // max number of pins per comp
#define MAX_SHAPE               	20000 // yes, 20.000

#define STAT_UNKNOWN            	0
#define STAT_LAYER              	1
#define STAT_THRU               	2
#define STAT_SHAPE              	3
#define STAT_COMP               	4
#define STAT_OBS                	5
#define STAT_NET                	6
#define STAT_PACKAGE            	7

#define OBS_KIND_KEEPOUT        	1
#define OBS_KIND_AVOID          	2
#define OBS_KIND_NOVIA          	3
#define OBS_KIND_KEEPIN         	4
#define OBS_KIND_TEXT           	5
#define OBS_KIND_NULL           	6
#define OBS_KIND_COMP_DETAIL    	7
#define OBS_KIND_COMP_KEEPIN    	8
#define OBS_KIND_COMP_KEEPOUT   	9
#define OBS_KIND_COMP_INNER     	10
#define OBS_KIND_COMP_OUTER     	11
#define OBS_KIND_MATRIX         	12
#define OBS_KIND_GROUP_KEEPIN   	13
#define OBS_KIND_GROUP_KEEPOUT  	14
#define OBS_KIND_HEIGHT_KEEPIN  	15
#define OBS_KIND_HEIGHT_KEEPOUT 	16
#define OBS_KIND_HOLE           	17
#define OBS_KIND_POURED_COPPER  	18
#define OBS_KIND_DRILLCHART     	19
#define OBS_KIND_DETAIL         	20
#define OBS_KIND_COMP_OUTLINE   	21
#define OBS_KIND_OUTLINE        	22

#define OBS_FLAGS_GENERATED     	0x0000
#define OBS_FLAGS_ARC           	0x0001
#define OBS_FLAGS_CIRCLE        	0x0002
#define OBS_FLAGS_ORIG          	0x0004
#define OBS_FLAGS_CREATED       	0x0008
#define OBS_FLAGS_SHAVABLE      	0x0010
#define OBS_FLAGS_SPARE         	0x0020
#define OBS_FLAGS_SHRINKED      	0x0040
#define OBS_FLAGS_HEAT_HOLE     	0x0080

#define OBS_HATCHKIND_HOLLOW		0
#define OBS_HATCHKIND_LINE			1
#define OBS_HATCHKIND_CROSS		2     // this is not used, but OBS_HATCHKIND_SOLID fill is used
#define OBS_HATCHKIND_SOLID		4

#define OBS_NO_NET					-(0x7fff)
#define OBS_ANY_NET					0

#define OBS_CCW						1
#define OBS_CW							2

#define CONN_ARC						3

#define PAD_ROUND						1
#define PAD_SQUARE					2
#define PAD_RECT						3
#define PAD_OBLONG					4
#define PAD_THERMAL					5
#define PAD_ANNULAR					6

#define BRK_ROUND						0
#define BRK_B_ROUND					1

#define SIZ_BRK_LST					(sizeof(brk_lst) / sizeof(List))
#define SIZ_MIN_LST					(sizeof(min_lst) / sizeof(List))
#define SIZ_STR_LST					(sizeof(str_lst) / sizeof(List))
#define  SIZ_BEGIN_LST				(sizeof(begin_lst) / sizeof(List))
#define  SIZ_HEADER_LST				(sizeof(header_lst) / sizeof(List))
#define  SIZ_LAYER_LST				(sizeof(layer_lst) / sizeof(List))
#define  SIZ_PADFORM_LST			(sizeof(padform_lst) / sizeof(List))
#define  SIZ_THRU_LST				(sizeof(thru_lst) / sizeof(List))
#define  SIZ_SYM_LST					(sizeof(sym_lst) / sizeof(List))
#define  SIZ_NET_LST					(sizeof(net_lst) / sizeof(List))
#define  SIZ_TEXT_LST				(sizeof(text_lst) / sizeof(List))
#define  SIZ_PACKAGE_LST			(sizeof(package_lst) / sizeof(List))
#define  SIZ_COMP_LST				(sizeof(comp_lst) / sizeof(List))
#define  SIZ_OBS_LST					(sizeof(obs_lst) / sizeof(List))
#define  SIZ_CONN_LST				(sizeof(conn_lst) / sizeof(List))

/* Structures Section *********************************************************/

typedef struct
{
   int      	electrical_layers;	// number of electrical layers from lyrphid 
   int      	number_of_vias;
   double   	inchfactor;
   int      	gridunit;				// in mass units
   int      	userdiv;
   int      	namecnt;
   int      	cur_status;

   double      rawOrcadOriginX;
   double      rawOrcadOriginY;

   /* Current values that are changed with global  */
   int      	cur_layernum;			// Layer index in layerarray
   double   	cur_width;           // Line/Text width
   double   	cur_height;          // Line/Text height   
   double   	cur_rotation;        // this is for pad rotation

   /* Current Component info used for Instance.    */
   double   	cur_pos_x;           // position
   double   	cur_pos_y;

   /* Current pad stack info.    */
   int      	cur_pad;             // Current pad
   int      	cur_pstack;          // Current padstack number
   int      	cur_pincnt;

   /* Pad stacks */
   int      	cur_textmirror;
   int      	cur_textjust;
   int      	cur_textrot;
   //
   int      	cur_type;				// used by layer and padshape
   int      	cur_padstackgraphic;
} MINGlobal;

//---------------------------------------------------------------------

class ORCADAdef
{
private:
   CString  	m_name;				// orcad layer name
   int			m_attr;          // layer attributes as defined in dbutil.h

public:
   ORCADAdef()    { m_attr = 0; }

   CString getName()       { return m_name; }
   void setName(CString n) { m_name = n; }

   int getAttr()           { return m_attr; }
   void setAttr(int a)     { m_attr = a; }

};

//---------------------------------------------------------------------

typedef struct         
{
   CString  	orcad_name;
   CString  	cc_name;
} ORCADAttrib;

typedef CTypedPtrArray<CPtrArray, ORCADAttrib*> ORCADAttribArray;

//---------------------------------------------------------------------

class ORCADDrill
{
private:
   double		m_diameter;

public:
   ORCADDrill(double dia) { m_diameter = dia; }
   double getDiameter()    { return m_diameter; }
};

class ORCADDrillArray : public CTypedPtrArrayContainer<ORCADDrill*>
{
};

//---------------------------------------------------------------------

class ORCADPadform
{
private:
   int     	   m_form;
   double   	m_sizeA;
   double   	m_sizeB;
   double   	m_offsetx;
	double		m_offsety;
   double   	m_rotation;

public:
   ORCADPadform(int f, double sa, double sb, double ox, double oy, double r)
   { m_form = f; m_sizeA = sa; m_sizeB = sb, m_offsetx = ox; m_offsety = oy, m_rotation = r; }

   int getForm()           { return m_form; }
   double getSizeA()       { return m_sizeA; }
   double getSizeB()       { return m_sizeB; }
   double getOffsetX()     { return m_offsetx; }
   double getOffsetY()     { return m_offsety; }
   double getRotation()    { return m_rotation; }
};

class ORCADPadformArray : public CTypedPtrArrayContainer<ORCADPadform*>
{
};

//---------------------------------------------------------------------

class MINVersion
{
public:
   int			m_version;
   int			m_majorrev;
   int			m_minorrev;

   MINVersion()   { m_version = 0; m_majorrev = 0; m_minorrev = 0; }
};

//---------------------------------------------------------------------

class ORCADPoly_l
{
public:
   double   	x;
	double		y;
   double   	w;
   double   	bulge;         // calc in write_arc
   int      	f;             // for arc
   int      	layerindex;    //
   int      	via;           // -1 no via

   ORCADPoly_l()  { x = 0.0; y = 0.0; w = 0.0; bulge = 0.0; f = 0; layerindex = -1; via = -1; }
};

//---------------------------------------------------------------------

class ORCADNet
{
private:
   CString  	m_name;
   int      	m_index;

public:
   ORCADNet(CString name, int indx)    { m_name = name; m_index = indx; }
   CString getName()          { return m_name; }
   void setName(CString nm)   { m_name = nm; }
   int getIndex()             { return m_index; }
};


class ORCADNetArray : public CTypedPtrArrayContainer<ORCADNet*>
{
};

//---------------------------------------------------------------------

typedef struct
{
   int   		index;
   int   		kind;
   int   		hatchkind;
   int   		poured;
   int   		keepout;
   long  		flags;      // bit mask
   int   		net;        //  > 0 obstacle has net assigned
         		            //  = 0 ANY-NET
         		            // < 0  pin number
   int   		symid;      // = 0  board obstical
         		            // < 0  component
         		            // > 0  symbol
   int			obslink;
   int			level;      // this is current obs layer
   double		width;
   char			name[100+1];
} MINObs;

typedef struct
{
   char			prosa[100+1];
   int			compindex;
   int			mirror;
   double		rotation;
   int			symindex;
   int			compname;
   int			refdes;
   int			symname;
   int			compvalue;
   int			free;
   int			aspect;     // 75 means 0.75
} MINText;

//---------------------------------------------------------------------

class MINPadstack
{
private:
   CString  	m_name;
   bool       	m_used;
   int      	m_index;
   int      	m_smdlayer;      // 1 = top, 2 = bot 3 = inner
   bool      	m_testpt;
   int      	m_graphic_cnt;   // if a padstack has no graphics, it is deleted.
   bool			m_mirror_defined;

public:
   MINPadstack(CString name, bool used, int index, int smdlayer, bool testpt, int graphiccnt, bool mirror)
   { m_name = name; m_used = used; m_index = index; m_smdlayer = smdlayer; m_testpt = testpt; m_graphic_cnt = graphiccnt; m_mirror_defined = mirror; }

   CString getName()          { return m_name; }
   bool getUsed()             { return m_used; }
   int getIndex()             { return m_index; }
   int getSmdLayer()          { return m_smdlayer; }
   bool getTestPt()           { return m_testpt; }
   int getGraphicCnt()        { return m_graphic_cnt; }
   bool getMirrorDefined()    { return m_mirror_defined; }

   void setName(CString nm)         { m_name = nm; }
   void setGraphicCnt(int cnt)      { m_graphic_cnt = cnt; }
   void setMirrorDefined(bool flg)  { m_mirror_defined = flg; }
   void setSmdLayer(int lyr)        { m_smdlayer = lyr; }
   void setTestPt(bool flg)         { m_testpt = flg; }
   void setUsed(bool flg)           { m_used = flg; }

};

class CPadstackArray : public CTypedPtrArrayContainer<MINPadstack*>
{
};

//---------------------------------------------------------------------

class MINPackage
{
private:
   CString  	m_name;
   int      	m_index;
   bool      	m_append_symname;   // if the package has already a geom assigned, make a new one

public:
   MINPackage(CString nm, int indx, bool append)   { m_name = nm; m_index = indx; m_append_symname = append; }

   const CString& getName() const { return m_name; }
   int getIndex() const { return m_index; }
   bool getAppendSymName() const { return m_append_symname; }

   void setName(CString nm)            { m_name = nm; }
   void setAppendSymName(bool flg)     { m_append_symname = flg; }
};

class MINPackageArray : public CTypedPtrArray<CPtrArray, MINPackage*>
{
public:
   MINPackage* getPackageWithIndex(int packageIndex);
};

//---------------------------------------------------------------------

class MINPin
{
private:
   CString     m_pinName;
   int			m_number;
   int			m_padstackindex;
   double   	m_x;
	double		m_y;
	double		m_rot;

public:
   MINPin() { m_number = m_padstackindex = 0; m_x = m_y = m_rot = 0.0; }

   const CString& getPinName() const { return m_pinName; }
   void setPinName(const CString& pinName) { m_pinName = pinName; }

   int getNumber()               { return m_number; }
   void setNumber(int n)         { m_number = n; }

   int getPadStackIndex()        { return m_padstackindex; }
   void setPadStackIndex(int n)  { m_padstackindex = n; }

   double getX()                 { return m_x; }
   void setX(double x)           { m_x = x; }

   double getY()                 { return m_y; }
   void setY(double y)           { m_y = y; }

   double getRot()               { return m_rot; }
   void setRot(double rot)       { m_rot = rot; }
};

//---------------------------------------------------------------------

class ORCADShape
{
private:
   static int m_nextId;

	CDataList	m_datalist;

   CString      m_name;
   CString		 m_devtypename;			// name of device type
   int   		 m_index;
   int   		 m_uid;
   int   		 m_pincnt;
   int			 m_layernum;
   bool   		 m_via;		// this is a via defintion
   bool   		 m_testpt;	// this is a testpoint defintion
   int   		 m_usage_cnt;
   bool			 m_isDuplicate;		// used in the function RemoveIdenticalSYM() to indicate duplicate SYM
   bool			 m_isBottomVersion;	// used in the function CheckMirroredMasterSYM() to indicate that a SYM is a bottom verison
   bool         m_isBottomDefined;
   double       m_origX;
   double       m_origY;
   int          m_id;
   int   		 m_derivedShapeIndex;				// graphic is derived from
   int          m_outlineLevel;
	BlockStruct* m_block;
   CTypedPtrArrayContainer<MINPin*> m_pins;
   CTypedPtrListContainer<BlockStruct*> m_derivedGeometries;

public:
	ORCADShape(int shapeIndex);
   ~ORCADShape();

   CDataList &GetDataList()   { return m_datalist; }

   int getIndex() const { return m_index; }
   void setUid(int uid)    { m_uid = uid; }
   int getPinCnt()         { return m_pincnt; }
   void incrementPinCnt()  { m_pincnt++; }
   int getUsageCount()     { return m_usage_cnt; }
   void insrementUsageCount() { m_usage_cnt++; }

   bool isVia()            { return m_via; }
   void setVia(bool flg)   { m_via = flg; }

   bool isTestPt()            { return m_testpt; }
   void setTestPt(bool flg)   { m_testpt = flg; }

   bool isDuplicate()            { return m_isDuplicate; }
   void setDuplicate(bool flg)   { m_isDuplicate = flg; }

   bool isBottomVersion()            { return m_isBottomVersion; }
   void setBottomVersion(bool flg)   { m_isBottomVersion = flg; }

   bool isBottomDefined()            { return m_isBottomDefined; }
   void setBottomDefined(bool flg)   { m_isBottomDefined= flg; }

   int getDerivedShapeIndex() const { return m_derivedShapeIndex; }
   void setDerivedShapeIndex(int shapeIndex) { m_derivedShapeIndex = shapeIndex; }
   bool isMaster() const { return (m_derivedShapeIndex == m_index); }

   int getOutlineLevel() const { return m_outlineLevel; }
   void setOutlineLevel(int outlineLevel) { m_outlineLevel = outlineLevel; }

   int getLayerNum()          { return m_layernum; }
   void setLayerNum(int n)    { m_layernum = n; }

   double getOrigX()          { return m_origX; }
   void setOrigX(double x)    { m_origX = x; }

   double getOrigY()          { return m_origY; }
   void setOrigY(double y)    { m_origY = y; }

   BlockStruct* getBlock() const { return m_block; }
   void setBlock(BlockStruct* block);

   MINPin& getPin(int pinIndex);
   void setNumPins(int pinCount);

   const CString& getName() const { return m_name; }
   void setName(const CString& name) { m_name = name; }

   CString getDevTypeName()                { return m_devtypename; }
   void setDevTypeName(CString name)       { m_devtypename = name; }

   bool pinLocationsMatch(ORCADShape& other,bool mirrorFlag);
   bool allXPinLocationsZero();

   BlockStruct& appendOrMatch(BlockStruct& block);
};

class ORCADShapeArray : public CTypedPtrArrayContainer<ORCADShape*>
{
public:
   ORCADShape* getMaster(ORCADShape& shape);
   ORCADShape* getShapeWithIndex(int shapeIndex);
};

//---------------------------------------------------------------------

enum PcbSide
{
     pcbSideUnknown,
     pcbSideTop,
     pcbSideBottom,
     pcbSideBoth
};


struct ORCADCompInst
{
private:
   CString     m_name;
   CString     m_partname;
   CString     m_value;

   int         m_symid;
   int         m_mirid;
   int         m_index;
   int         m_packageindex;

   double      m_rotation;
   bool        m_mirror;
   bool        m_nonelectric;

   bool        m_via;           // this is used as a via
   bool        m_testpt;        // this is used as a testpoint. (this also has the via flag set)
   double      m_height;

   int         m_derivedSymId;
   int         m_layernum;
   bool			m_normalize;

   int         m_outlineLevel;
   DataStruct* m_componentInsertData;

   enum PcbSide m_padloc;

public:
   ORCADCompInst();

   CString getName()                { return m_name; }
   void setName(CString str)        { m_name = str; }

   CString getPartName()            { return m_partname; }
   void setPartName(CString str)    { m_partname = str; }

   CString getValue()               { return m_value; }
   void setValue(CString str)       { m_value = str; }

   int getSymId()                   { return m_symid; }
   void setSymId(int id)            { m_symid = id; }

   int getMirId()                   { return m_mirid; }
   void setMirId(int id)            { m_mirid = id; }

   int getIndex()                   { return m_index; }
   void setIndex(int indx)          { m_index = indx; }

   int getPackageIndex()            { return m_packageindex; }
   void setPackageIndex(int pki)    { m_packageindex = pki; }

   double getRotation()             { return m_rotation; }
   void setRotation(double r)       { m_rotation = r; }

   bool getMirror()                 { return m_mirror; }
   void setMirror(bool tf)          { m_mirror = tf; }

   bool isNonElectric()             { return m_nonelectric; }
   void setIsNonElectric(bool tf)   { m_nonelectric = tf; }

   void setPcbSide(PcbSide padside);       
   PcbSide getPcbSide()             { return m_padloc; }

   bool isVia()                     { return m_via; }
   void setIsVia(bool tf)           { m_via = tf; }

   bool isTestPt()                  { return m_testpt; }
   void setIsTestPt(bool tf)        { m_testpt = tf; }

   double getHeight()               { return m_height; }
   void setHeight(double hght)      { m_height = hght; }

   int getDerivedSymId()            { return m_derivedSymId; }
   void setDerivedSymId(int id)     { m_derivedSymId = id; }

   int getLayerNum()               { return m_layernum; }
   void setLayerNum(int layernum)  { m_layernum = layernum; }

   int getOutlineLevel() const { return m_outlineLevel; }
   void setOutlineLevel(int outlineLevel) { m_outlineLevel = outlineLevel; }

   DataStruct* getComponentInsert() const { return m_componentInsertData; }
   void setComponentInsert(DataStruct* componentInsert);

   int getShapeIndex() const { return m_symid; }
   int getPackageIndex() const { return m_packageindex; }
   void setBlock(BlockStruct& block);

	void setNormalize(bool norm) { m_normalize = norm;}
	bool getNormalize() { return m_normalize;}
};

class ORCADCompInstArray : public CTypedPtrArrayContainer<ORCADCompInst*>
{
public:
   void normalizeGeometriesAndInserts(ORCADShapeArray& orcadShapes);
   void reduceGeometries(ORCADShapeArray& orcadShapes);
   void createDevices(CCamCadDatabase& camCadDatabase,FileStruct* file,
      ORCADShapeArray& orcadShapes,MINPackageArray& orcadPackages);
   void makeGeometriesTopDefined();

private:
   bool ValidateTestpoint(ORCADCompInst* componentInstance);
   void BuildViaGeometry(CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> &viaGeometryMap);
   void changeGeometryTopDefined(bool setTopDefined, CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> &bottomGeometryMap, DataStruct* componentInsert, CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> &viaGeometryMap);
};

//---------------------------------------------------------------------

class COrcadLayer
{
private:
   CString  		m_name1;        // name
   CString  		m_name2;        // nickname
   CString  		m_name3;        // library name

public:
   int   		mirror;
   int   		index;
   int   		unused:1;
   int   		etch:1;
   int   		plane:1;
   int   		drill:1;

   COrcadLayer();

   const CString& getName() const;  // Chooses name1 or name2 depending on user .out file setting.

   const CString& getName1() const { return m_name1; }
   void setName1(const CString& layerName) { m_name1 = layerName; }

   const CString& getName2() const { return m_name2; }
   void setName2(const CString& layerName) { m_name2 = layerName; }

   const CString& getName3() const { return m_name3; }
   void setName3(const CString& layerName) { m_name3 = layerName; }

};

class ORCADLayerArray : public CTypedPtrArrayContainer<COrcadLayer*>
{
public:
   COrcadLayer& getDefinedLayer(int index);
};

//---------------------------------------------------------------------

class ORCADCompPin
{
private:
   int			m_c;
   int			m_p;

public:
   ORCADCompPin(int c, int p) { m_c = c; m_p = p; }
   int getC()  { return m_c; }
   int getP()  { return m_p; }
};

class ORCADCompPinArray : public CTypedPtrArrayContainer<ORCADCompPin*>
{
};

//---------------------------------------------------------------------


//***********************************************************************

typedef struct
{
   char		*token;
   int		(*function)();
} List;

//***********************************************************************

/* Lists Section *********************************************************/

/* Bracket definitions */
static int fskip();     // silently skips.
static int fnull();     // skip and reports to err-file

static List brk_lst[] =
{
   "(",						fnull,
   ")",						fnull,
};

/* min_lst section */
static int mass_minheader();

static List min_lst[] =
{
   "MIN",					mass_minheader,
};

/* str_lst section. Start of MIN file */
static int mass_version();
static int mass_majorrev();
static int mass_minorrev();
static int mass_l();
static int mass_v();
static int mass_begin();

static List str_lst[] =
{
   "Version",     		mass_version,
   "MajorRev",    		mass_majorrev,
   "MinorRev",    		mass_minorrev,
   "L",           		mass_l,
   "V",           		mass_v,
   "Begin",       		mass_begin,
};

/* begin_lst section */
static int begin_header();
static int begin_layer();
static int begin_thru();
static int begin_sym();
static int begin_package();
static int begin_comp();
static int begin_net();
static int begin_c();
static int begin_seg();
static int begin_obs();
static int begin_text();
static int begin_color();
static int begin_attr();

static List begin_lst[] =
{
   "Header",      		begin_header,
   "Strat",       		fskip,
   "Placestrat",  		fskip,
   "L",           		begin_layer,
   "Thru",        		begin_thru,
   "Sym",         		begin_sym,
   "Package",     		begin_package,
   "Comp",        		begin_comp,
   "Net",         		begin_net,
   "C",           		begin_c,
   "Seg",         		begin_seg,
   "Obs",         		begin_obs,
   "Text",        		begin_text,
   "Color",       		begin_color,
   "Attr",        		begin_attr,
   "Err",         		fskip,
   "Report",      		fskip,
   "Aperture",    		fskip,
   "Note",        		fskip,
   "End",         		fskip,
};

/* header_lst section */
static int header_grid();
static int header_userdiv();
static int header_inchfactor();
static int header_origin();
static int header_drlchartloc();

static List header_lst[] =
{
   "Grid",              header_grid,
   "Viagrid",           fskip,
   "UserDiv",           header_userdiv,
   "DisplayGrid",       fskip,
   "PlaceGrid",         fskip,
   "DetailGrid",        fskip,
   "DotGrid",           fskip,
   "DisplayPrecision",  fskip,
   "Origin",            header_origin,
   "InchFactor",        header_inchfactor,
   "View",              fskip,
   "Resume",            fskip,
   "OffGrid",           fskip,
   "NoObsEdit",         fskip,
   "NoCompEdit",        fskip,
   "MultiVia",          fskip,
   "Metric",            fskip,
   "ConnectThruPours",  fskip,
   "BackupSweep",       fskip,
   "BackupInterval",    fskip,
   "TimeUsed",          fskip,
   "Translucent",       fskip,
   "DoPG",              fskip,
   "PgLock",            fskip,
   "PgDisable",         fskip,
   "RotateAngle",       fskip,
   "AngleSnap",         fskip,
   "PinTestPoints",     fskip,
   "DrlChartLoc",       header_drlchartloc,
   "DrlChartTextHeight",fskip,
   "DrlChartLineWidth", fskip,
   "DrillSize",         fskip,
   "DoSigs",            fskip,
   "SigLock",           fskip,
   "SigShare",          fskip,
   "DirInside",         fskip,
   "DirOutside",        fskip,
   "OnlyOffGrid",       fskip,
   "PgFrVia",           fskip,
   "PgShare",           fskip,
   "SigFrVia",          fskip,
   "FanoutVia",         fskip,
   "Override",          fskip,
   "MaxDistance",       fskip,
};

/* layer_lst section */
static int mass_name();
static int mass_uid();
static int layer_etch();
static int layer_mirror();
static int layer_tt();
static int layer_pt();
static int layer_pp();
static int layer_plane();
static int layer_unused();
static int layer_drill();

static List layer_lst[] =
{
   "N",           		mass_name,
   "Uid",         		mass_uid,
   "Etch",        		layer_etch,
   "Mirror",      		layer_mirror,
   "TT",          		layer_tt,
   "TV",          		fskip,
   "TP",          		layer_pt,
   "VV",          		fskip,
   "VP",          		fskip,
   "PP",          		layer_pp,
   "Stair",       		fskip,
   "Plane",       		layer_plane,
   "Spare",       		fskip,
   "Unused",      		layer_unused,
   "Doc",         		fskip,
   "Drill",       		layer_drill,
};

/* padform_lst section */
static int thru_l();
static int padround();
static int padsquare();
static int padrect();
static int padoblong();
static int padthermal();
static int padannular();
static int pad_r();
static int pad_undef();

static List padform_lst[] =
{
   "R",           		pad_r,		// pad rotation
   "Round",       		padround,
   "UNDEF",       		pad_undef,
   "NoConnect",   		fskip,
   "Square",      		padsquare,
   "Rect",        		padrect,
   "Oblong",      		padoblong,
   "Annular",     		padannular,
   "Oval",        		padoblong,
   "ThermalRelief",		padthermal,
};

/* thru_lst section */
static int thru_testpt();

static List  thru_lst[] =
{
   "N",           		mass_name,
   "Uid",         		mass_uid,
   "TestPt",      		thru_testpt,
   "NonPlated",   		fskip,
   "LgThermal",   		fskip,
   "DirectHit",   		fskip,
   "L",           		thru_l,
};

/* sym_lst section */
static int sym_uid();
static int sym_p();
static int sym_derived();
static int sym_via();
static int sym_testpt();

static List sym_lst[] =
{
   "N",           		mass_name,
   "Uid",         		sym_uid,
   "P",           		sym_p,
   "Derived",     		sym_derived,
   "Via",         		sym_via,
   "TestPt",      		sym_testpt,
   "ExitRule",    		fskip,
};

/* net_lst section */
static int net_width();
static int net_basewidth();
static int net_min();
static int net_max();
static int net_basemin();
static int net_basemax();

static List net_lst[] =
{
   "N",           		mass_name,
   "Uid",         		mass_uid,
   "W",           		net_width,
   "BW",          		net_basewidth,
   "Min",         		net_min,
   "Max",         		net_max,
   "BMin",        		net_basemin,
   "BMax",        		net_basemax,
   "Weight",      		fskip,
   "Group",       		fskip,
   "Highlight",   		fskip,
   "TestPt",      		fskip,
   "Color",       		fskip,
   "Enabled",     		fskip,
   "Grid",        		fskip,
   "Shove",       		fskip,
   "Share",       		fskip,
   "Ripup",       		fskip,
   "L",           		fskip,
   "Reconnect",   		fskip,
   "Reconn",      		fskip,
   "NetViaRestrict",    fskip,
};

/* text_lst section */
static int mass_prosa();
static int mass_width();
static int mass_height();
static int text_r();
static int text_comp();
static int text_aspect();
static int text_l();
static int mass_drillchart();
static int mass_mirrored();
static int text_sym();
static int text_free();
static int text_compname();
static int text_symname();
static int text_compvalue();

static List text_lst[] =
{
   "N",           		mass_prosa,
   "Uid",         		mass_uid,
   "W",           		mass_width,
   "H",           		mass_height,
   "R",           		text_r,
   "Comp",        		text_comp,
   "Aspect",      		text_aspect,
   "L",           		text_l,
   "Static",      		fskip,
   "Dimension",   		fskip,
   "DrillChart",  		mass_drillchart,
   "Mirrored",    		mass_mirrored,
   "Sym",         		text_sym,
   "Package",     		fskip,
   "Free",        		text_free,
   "RefDes",      		text_compname, // new in Version 7
   "CompName",    		text_compname,
   "SymName",     		text_symname,
   "CompDetail",  		fskip,
   "CompValue",   		text_compvalue,
   "CharRot",     		fskip,
};

/* package_lst section */
static List package_lst[] =
{
   "N",           		mass_name,
   "Uid",         		fskip,
   "P",           		fskip,
};

/* comp_lst section */
static int comp_nonelectric();
static int comp_value();
static int comp_sym();
static int comp_r();
static int comp_via();
static int comp_testpt();
static int comp_package();
static int comp_mir();
static int comp_mirsym();

static List comp_lst[] =
{
   "N",           		mass_name,
   "Uid",         		mass_uid,
   "Fixed",       		fskip,
   "NonElectric", 		comp_nonelectric,
   "Value",       		comp_value,
   "Enabled",     		fskip,
   "Locked",      		fskip,
   "Key",         		fskip,
   "NoRename",    		fskip,
   "Cluster",     		fskip,
   "Sym",         		comp_sym,
   "R",           		comp_r,
   "Via",         		comp_via,
   "TestPt",      		comp_testpt,
   "Package",     		comp_package,
   "Group",       		fskip,
   "Mirrored",    		comp_mir,
   "Mirror",      		comp_mirsym,     
};

/* obs_lst section */
static int obs_keepin();
static int obs_nofill();
static int obs_solidfill();
static int obs_nonet();
static int obs_detail();
static int obs_level();
static int obs_w();
static int obs_basewidth();
static int obs_width();
static int obs_outline();
static int obs_compoutline();
static int obs_generated();
static int obs_sym();
static int obs_circle();
static int obs_arc();
static int obs_ccw();
static int obs_cw();
static int obs_anynet();
static int obs_poured();
static int obs_crossfill();
static int obs_net();
static int obs_compkeepout();
static int obs_keepout();
static int obs_comp();

static List obs_lst[] =
{
   "N",           		mass_name,
   "Uid",         		mass_uid,
   "Keepin",      		obs_keepin,
   "NoFill",      		obs_nofill,
   "NoRoute",     		fskip,
   "Null",        		fskip,         // ???
   "Hole",        		fskip,         
   "SolidFill",   		obs_solidfill,
   "NoNet",       		obs_nonet,
   "Grid",        		fskip,
   "Orig",        		fskip,
   "Detail",      		obs_detail,
   "L",           		obs_level,
   "W",           		obs_w,
   "BW",          		obs_basewidth,
   "Width",       		obs_width,
   "Outline",     		obs_outline,
   "CompOutline", 		obs_compoutline,
   "Generated",   		obs_generated,
   "IsolateAll",  		fskip,
   "Created",     		fskip,
   "Avoid",       		fskip,
   "zOrder",      		fskip,
   "Sym",         		obs_sym,
   "Circle",      		obs_circle,
   "Arc",         		obs_arc,
   "CCW",         		obs_ccw,
   "CW",          		obs_cw,
   "DrillChart",  		mass_drillchart,
   "AnyNet",      		obs_anynet,
   "Poured",      		obs_poured,
   "LineFill",    		fskip,
   "Clearance",   		fskip,
   "Rot",         		fskip,
   "Text",        		fskip,
   "Crossfill",   		obs_crossfill,
   "Net",         		obs_net,
   "CompKeepout", 		obs_compkeepout,
   "Keepout",     		obs_keepout,
   "GroupKeepIn", 		fskip,
   "Dimension",   		fskip,
   "Grid",        		fskip,
   "InsertionOutline",	fskip,
   "P",						fskip,
   "Comp",					obs_comp,
};

/* conn_lst section */
static int conn_net();
static int conn_p();
static int conn_l();
static int conn_v();
static int conn_basew();
static int conn_w();
static int conn_arc();

static List conn_lst[] =
{
   "Net",         		conn_net,
   "P",           		conn_p,
   "L",           		conn_l,
   "W",           		conn_w,
   "BW",          		conn_basew,
   "V",           		conn_v,
   "Arc",         		conn_arc,
   "Locked",      		fskip,
   "AnyLock",     		fskip,
   "Grid",        		fskip,
};

