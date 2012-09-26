
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__ThedaIn_h__)
#define __ThedaIn_h__

#pragma once

#include "CamCadDatabase.h"

#define  MAX_LINE                      2000  /* Max line length.           */
#define  MAX_TOKEN                     MAX_LINE

#define  MAX_LAYERS                    255   /* Max number of layers.   */
#define  MAX_SMDTYPE                   10

// mode of section
#define  SECTION_PACKAGE               1
#define  SECTION_BOARDOUTLINE          2
#define  SECTION_KEEPOUT               3
#define  SECTION_KEEPIN                4
#define  SECTION_CONDUCTION            5
#define  SECTION_SHAPES                6
#define  SECTION_PADSTACKGRAPHIC       7
#define  SECTION_SYMBOLS               8
#define  SECTION_COPPER_AREAS          9
#define  SECTION_DIMENSION            10

#define  FILESTATUS_BOARD              1
#define  FILESTATUS_PANEL              2

typedef struct
{
   CString layername;
   int   layertype;
}MaterialLayerType;


typedef struct
{
   CString  attrib;
   CString  mapattrib;
}TLAttribmap;
typedef CTypedPtrArray<CPtrArray, TLAttribmap*> CAttribmapArray;

typedef struct
{
   CString  name;
}TLGeom;
typedef CTypedPtrArray<CPtrArray, TLGeom*> CGeomArray;

typedef struct
{
   double   tl_revision;
   CString  tl_comment;
   CString  tl_board_comment;
   double   software_revision;
   CString  software_comment;
   double   library_revision;
   double   board_revision;
   CString  creation_date;
}TLVersion;

struct TLTextpen
{
   int number;          
   double height;       // "HEIGHT"
   double width;        // "WIDTH"
   double slant;        // "SLANT"  
   double typeface;     // "TYPEFACE" - line width
   double charSpacing;  // "CHARACTER_SPACING"

   TLTextpen();
   double getCharBoxWidth() { return width + charSpacing; }
};

typedef CTypedPtrArray<CPtrArray, TLTextpen*> CTextpenArray;

typedef struct
{
   double   x,y, bulge;
   int      arc_point;
} TLPoly;
typedef CArray<TLPoly, TLPoly&> CPolyArray;

typedef struct
{
   double d;
   int    toolindex;
}TLDrill;
typedef CArray<TLDrill, TLDrill&> CDrillArray;

typedef struct
{
   char     form;
   double   sizeA;
   double   sizeB;
   double   offsetx, offsety;
   double   rotation;
}TLPadform;
typedef CArray<TLPadform, TLPadform&> CPadformArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  identifier;
   CString  package_id;
   CString  device_id;
} TLDeviceComp;
typedef CTypedPtrArray<CPtrArray, TLDeviceComp*> CDeviceCompArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   int   devicecompindex;
   CString  keyword;
   CString  value;
} TLCompProp;
typedef CTypedPtrArray<CPtrArray, TLCompProp*> CCompPropArray;

typedef struct
{
   CString  shape_id;
   double   radius;
}TLShapeCircle;
typedef CTypedPtrArray<CPtrArray, TLShapeCircle*> CShapeCircleArray;

//_____________________________________________________________________________
class CPinNameWithContactLayer : public CObject
{
private:
   CString m_pinName;
   CString m_contactLayerName;
   CString m_mirroredContactLayerName;

public:
   CPinNameWithContactLayer(const CString& pinName,const CString& contactLayerName,
      const CString& mirroredContactLayerName);

   CString getPinName()                  const { return m_pinName; }
   CString getContactLayerName()         const { return m_contactLayerName; }
   CString getMirroredContactLayerName() const { return m_mirroredContactLayerName; }
   CString getContactLayerName(bool mirrorFlag) const;
   void setContactLayerNames(const CString& contactLayerName,const CString& mirroredContactLayerName);
};

//_____________________________________________________________________________
class CPinNameToContactLayerMap
{
private:
   CTypedMapSortStringToObContainer<CPinNameWithContactLayer> m_map;

public:
   CPinNameToContactLayerMap();   
   void empty();

   void addPin(const CString& pinName,const CString& contactLayerName,const CString& mirroredContactLayerName);
   void changePinToThruHole(const CString& pinName);
   bool lookup(const CString& pinName,CString& contactLayerName);

   CString getSignature(bool mirrorFlag);
   CString getSignatureInfix(bool mirrorFlag);
   CString getDescriptor();
};

//_____________________________________________________________________________
/*
!  !  @  COMPONENT[ 1138 ]
!  !  @  !  REFERENCE_DESIGNATOR    := 'U107';
!  !  @  !  TYPE                    := NORMAL_COMPONENT;
!  !  @  !  LIBRARY_IDENTIFIER      := '180-2031-05-002';
!  !  @  !  PIN_IDS                 := PHYSICAL;
!  !  @  !..FIN_COMPONENT[ 1138 ];
!  !  @  COMPONENT[ 1139 ]
!  !  @  !  REFERENCE_DESIGNATOR    := 'U108';
!  !  @  !  TYPE                    := NORMAL_COMPONENT;
!  !  @  !  PACKAGE_ID              := 'osc-4p-m3h-co12';
!  !  @  !  PIN_IDS                 := PHYSICAL;
!  !  @  !..FIN_COMPONENT[ 1139 ];
*/
class TLNetlistComp         // this is from DESIGN_OBJECTS | COMPONENT
{
public:
   CString  reference;
   CString  library_id;
   CString  package_id;       // I have seen this in 
   int      comptype;
   int      pin_ids_physical; // 
   CString  pulled_pins;

private:
   CPinNameToContactLayerMap  m_pinNameToContactLayerMap;
   CString m_componentLayerName;
   DataStruct* m_componentData;

public:
   TLNetlistComp();

   void emptyPinNameToContactLayerMap();
   void addPin(const CString& pinName,CString contactLayerName,CString mirroredContactLayerName);

   CString getComponentLayerName() const;
   void setComponentLayerName(const CString& layerName);

   DataStruct* getComponentData() const;
   void setComponentData(DataStruct* componentData);

   CPinNameToContactLayerMap& getPinNameToContactLayerMap() { return m_pinNameToContactLayerMap; }
};

typedef CTypedPtrArray<CPtrArray, TLNetlistComp*> CNetlistCompArray;

class TLComp
{
public:
   CString  refdes;
   char     placed;
   char     already_placed;   // place_comp routine
   int      mir;
   char     x_mirror;
   char     y_mirror;
   double   x,y,rot;          // rot in degrees

private:
   CString  m_componentLayerName;
   TLNetlistComp* m_netlistComp;
   BlockStruct* m_instanceSpecificGeometry;
   DataStruct* m_component;

public:
   TLComp();
   ~TLComp();
   void initialize();
   void releaseData();

   TLNetlistComp* getNetlistComp() const;
   void setNetlistComp(TLNetlistComp* netlistComp);

   CString getComponentLayerName() const;
   void setComponentLayerName(const CString& layerName);

   BlockStruct* getInstanceSpecificGeometry() const { return m_instanceSpecificGeometry; }
   void setInstanceSpecificGeometry(BlockStruct* instanceSpecificGeometry);

   DataStruct& getComponent();
};

typedef struct
{
   CString  compname;
   CString  pinname;
}TLComppin;

typedef struct
{
   CString refDes;
   CString layerName;
}TLProbe;

struct AText
{
private:
   char m_horizontalAlignment;
   char m_verticalAlignment;
   HorizontalPositionTag m_horizontalPosition;
   VerticalPositionTag m_verticalPosition;

public:
   int   textpen;
   CString  string;
   CString  type;
   double   x,y,rotation;
   double   h,w, charw;
   int      mirror, x_mirror;

public:
   char getHorizontalAlignment() const { return m_horizontalAlignment; }
   void setHorizontalAlignment(char horizontalAlignment);
   HorizontalPositionTag getHorizontalPosition() const { return m_horizontalPosition; }

   char getVerticalAlignment() const { return m_verticalAlignment; }
   void setVerticalAlignment(char vertivalAlignment);
   VerticalPositionTag getVerticalPosition() const { return m_verticalPosition; }

};

typedef struct
{
   CString  origname;
   CString  rename;
} RenameLayer;

typedef struct
{
   CString  name;       // theda layer name
   char  attr;          // layer attributes as defined in dbutil.h
   int   mirror;        // Adef index for mirror
} TLAdef;

typedef struct
{
   CString  number;
   CString  identifier;
   double   x, y, rotation;
   int      x_mirror;
   CString  padstack_id;
   CString m_contactLayerName;
}TLPin;

// used in TL 1.x
typedef struct
{
   CString  number;
   CString  identifier;
}TLPinname;
typedef CTypedPtrArray<CPtrArray, TLPinname*> CPinNameArray;

typedef struct
{
   CString  number;
   CString  identifier;
   CString  description;
}TLProp;

typedef struct
{
   double   x, y;
}TLBoard;

class TLVia
{
public:
   CString  number;     // used in TL1 for testpoints and vias.
   double   x, y, rotation;
   int      x_mirror;
   CString  padstack_id;
   CString  layer;
   CString  needed;  // this is for blind, buried vias.
};

typedef struct
{
   double   x, y, rotation;
   int      x_mirror;
   CString  padstack_id;
   CString  layer;
}TLRefPad;

typedef struct
{
   double   centerx, centery;
   double   radius;
}TLCircle;

typedef struct
{
   double   firstx, firsty;
   double   lastx, lasty;
   double   radius;
   char     counterclock;
   char     smallersemicircle;
}TLArc;

typedef struct
{
   int      section;
   CString  layer;
} TLBoardoutline;

typedef struct
{
   CString  name;
   int      type;    // only padstack is defined.
}TLIGNORE;
typedef CTypedPtrArray<CPtrArray, TLIGNORE*> CIgnoreArray;

struct List
{
   char  *token;
   int      (*function)();
};

//_____________________________________________________________________________
class CThedaTableParser
{
protected:
   //CTypedPtrMap<CMapStringToPtr,CString,int(*)()> m_handlersByColumnName;
   //CTypedPtrArray<CPtrArray,int(*)()> m_handlersByColumnIndex;
   int m_numColumns;
   CStringArray m_columnNames;
   CStringArray m_rowValues;

public:
   CThedaTableParser();

   virtual bool parse();
   virtual bool processColumns();
   virtual bool processRow();
   //void addColumnHandler(const char* columnName,int (*function)());
};

//_____________________________________________________________________________
class CThedaNetListTableParser : public CThedaTableParser
{
private:
   int m_referenceDesignatorColumn;
   int m_pinIdColumn;
   int m_seqNoColumn;
   int m_schSymbolIdColumn;
   int m_schPinIdColumn;
   int m_probeRefDesColumn;
   int m_probeColumn;

public:
   CThedaNetListTableParser();

   virtual bool processColumns();
   virtual bool processRow();
};

//_____________________________________________________________________________
class CCommandListEntry
{
private:
   int m_lineNumber;
   CString m_command;
   List* m_commandList;

public:
   CCommandListEntry(int lineNumber,const CString& command,List* commandList);

   int getLineNumber() { return m_lineNumber; }
   CString getCommand() { return m_command; }
   List* getCommandList() { return m_commandList; }
};

//_____________________________________________________________________________
class CCommandListStack
{
private:
   int m_sp;
   CTypedPtrArray<CPtrArray,CCommandListEntry*> m_stack;

public:
   CCommandListStack();

   int push(int lineNumber,const CString& command,List* commandList);
   CCommandListEntry* pop();

   CString trace();
};

//_____________________________________________________________________________
class CCommandListMap
{
private:
   CTypedPtrMap<CMapPtrToWord,List*,WORD> m_commandListMap;
   CStringArray m_commandListNames;

public:
   CCommandListMap();
   ~CCommandListMap();

   void setAt(const CString& listName,List* commandList);
   CString getCommandListName(List* commandList);
};

//_____________________________________________________________________________
class CThedaPadStack
{
private:
   CCEtoODBDoc& m_camCadDoc;
   BlockStruct* m_block;
   CTypedPtrListContainer<DataStruct*>* m_dataList;

public:
   CThedaPadStack(CCEtoODBDoc& camCadDoc,BlockStruct* block);
   ~CThedaPadStack();

   POSITION getHeadPosition();
   DataStruct* getNext(POSITION& pos);
   void restructure();

private:
   void flattenData();
};

//_____________________________________________________________________________
class CThedaPartialVia
{
private:
   CString m_id;
   CString m_layersDescriptor;
   CString m_fullViaStackName;

   CString m_startLayer;
   CString m_endLayer;

public:
   CThedaPartialVia(const CString& id,const CString& layersDescriptor,
      const CString& fullViaStackName,BlockStruct* viaBlock);
   //~CThedaPartialVia();

   CString getId() const { return m_id; }
   CString getViaStackName() const;
   CString getFullViaStackName() const { return m_fullViaStackName; }
   CString getLayersDescriptor() const { return m_layersDescriptor; }
};

//_____________________________________________________________________________
class CThedaPartialVias
{
private:
   CCEtoODBDoc& m_camCadDoc;
   int m_fileNum;
   CTypedMapStringToPtrContainer<CThedaPartialVia*> m_partialVias;

public:
   CThedaPartialVias(CCEtoODBDoc& camCadDoc,int fileNum);
   //~CThedaPartialVias();

   static CThedaPartialVias* m_thedaPartialVias;
   static CThedaPartialVias& getThedaPartialVias(CCEtoODBDoc& camCadDoc,int fileNum);
   static void releasePartialVias();

   CThedaPartialVia* getDefinedPartialVia(const CString& layersDescriptor,const CString& padStackName);
   void generatePartialVias();

private:
};

//_____________________________________________________________________________
class CThedaComponentPin
{
private:
   CString m_refDes;
   CString m_pinName;
   CString m_netName;
   CompPinStruct* m_compPin;

public:
   CThedaComponentPin(const CString& netName,const CString& refDes,const CString& pinName,CompPinStruct* m_compPin);

   // accessors;
   CString getRefDes()  const { return m_refDes; }
   CString getPinName() const { return m_pinName; }
   CString getNetName() const { return m_netName; }
   CompPinStruct* getCompPin() const { return m_compPin; }
};

//_____________________________________________________________________________
class CThedaComponentPins
{
private:
   CTypedMapStringToPtrContainer<CThedaComponentPin*> m_componentPins;

public:
   CThedaComponentPins();

   //
   CThedaComponentPin& addComponentPin(const CString& netName,const CString& refDes,const CString& pinName);
};

//_____________________________________________________________________________
class CThedaTestProbe
{
private:
   CString m_refDes;
   DataStruct* m_probeInsertData;

public:
   CThedaTestProbe(const CString& refDes,DataStruct* probeInsertData);

   // accessors;
   CString getRefDes()  const { return m_refDes; }
   DataStruct* getProbeInsertData() const { return m_probeInsertData; }
};

//_____________________________________________________________________________
class CThedaTestProbes
{
private:
   CTypedMapStringToPtrContainer<CThedaTestProbe*> m_testProbes;

public:
   CThedaTestProbes();

   //
   CThedaTestProbe& addTestProbe(const CString& refDes,DataStruct* probeInsertData);
   CThedaTestProbe* getTestProbe(const CString& refDes);
};

//_____________________________________________________________________________
class CThedaReader
{
private:
   static int m_pinNameToContactLayerMapDescriptorKeywordIndex;

   CCEtoODBDoc& m_camCadDoc;
   CCamCadBlockMap m_blockMap;
   CDataList m_viaDataList;
   CDataList m_testPointDataList;
   CDataList m_referencePadDataList;
   FileStruct* m_pcbFile;
   BlockStruct* m_restructuredPadStacksBlock;
   CMapStringToString m_idealPackageIdMap;
   CMapStringToString m_rectifiedPackageIdMap;
   CStringList m_sectionStack;
   CStringArray m_materialStack;

   CString m_currentSymbolTypeString;
   CThedaComponentPins m_thedaComponentPins;
   CThedaTestProbes m_thedaTestProbes;

   LayerStruct* m_floatingLayer;
   CString m_componentPlacementLayerName;
   CString m_contactLayerName;
   bool m_constructingComponentGeometryFlag;
   bool m_constructingPadstackGeometryFlag;
   bool m_optionAdjustLayerNames;
   bool m_optionRectifyComponents;
   bool m_optionRectifyViasAndTestPoints;
   bool m_optionPurgeUnusedWidthsAndGeometries;
   bool m_optionPurgeUnusedLayers;
   bool m_optionFixupGeometryNames;

	CMessageFilter* m_messageFilter;
	CMessageFilter& getMessageFilter();

public:
   CThedaReader(CCEtoODBDoc& camCadDoc);
	~CThedaReader();

   // accessors;
   CString getCurrentSymbolTypeString() const;
   void    setCurrentSymbolTypeString(const CString& currentSymbolTypeString);
   CThedaComponentPins& getThedaComponentPins() { return m_thedaComponentPins; }
   CThedaTestProbes& getThedaTestProbes() { return m_thedaTestProbes; }

   FileStruct* getPcbFile() const { return m_pcbFile; }
   void setPcbFile(FileStruct* pcbFile) { m_pcbFile = pcbFile; }

   int getDrillHoleLayerIndex() const;
   bool isThruHolePadStackGeometry(BlockStruct& geometry) const;
   //void updateComponentGeometryContactLayerMap(const CString& packageId,
   //   const CString& componentLayerName,CPinNameToContactLayerMap& pinNameToContactLayerMap);
   CString getRectifiedPackageId(const CString& packageId,
      const CString& componentLayerName,CPinNameToContactLayerMap& pinNameToContactLayerMap);
   BlockStruct* getRectifiedComponentGeometry(const CString& rectifiedPackageId,
      const CString& componentLayerName,CPinNameToContactLayerMap& pinNameToContactLayerMap);
   //CString getRectifiedPadStackGeometryName(const CString& padStackId,const CString& floatingLayerName);
   //CString getRectifiedPadStackGeometryName(const CString& padStackId);
   //void updateRectifiedPackageId();

   //void instantiateRectifiedComponentGeometries();
   BlockStruct& getRectifiedPadStackGeometry(BlockStruct& padStackGeometry,const CString& contactLayerName);
   BlockStruct& getRectifiedPadGeometry(BlockStruct& padGeometry,const CString& contactLayerName);
   BlockStruct& getRectifiedGeometry(BlockStruct& geometry,const CString& floatToLayerName,
      bool padStackDescendantFlag);
   void addPinNameToContactLayerMapDescriptorAttribute(DataStruct& component,const CString& descriptor);

   //void instantiateRectifiedPadStackGeometries();
   bool calcPlacementSpecificLayerName(CString& placementSpecifiedLayerName,
      const CString& layerName,const CString& floatToLayerName);
   void changePlacementSpecificLayers(BlockStruct& geometry,const CString& floatToLayerName);
   void changePlacementSpecificLayers(CAttributes*& attributes,const CString& floatToLayerName);
   bool referencesPlacementSpecificLayer(BlockStruct& geometry);
   CString getEnclosedLayerName(const CString& layerName) const;
   CString getInfixLayerName(const CString& layerName) const;
   CString getNormalizedInfixLayerName(CString layerName,bool oppositeFlag=false) const;
   CString getComponentSideNormalizedInfixLayerName(CString layerName) const;
   CString getPlacementSpecificInfixLayerName(CString layerName) const;

   BlockStruct& getDefinedBlock(const CString& blockName,int fileNumber,BlockTypeTag blockType);
   BlockStruct& graphBlockOn(const CString& blockName,int fileNumber,BlockTypeTag blockType);

   LayerStruct& getDefinedLayer(const CString& layerName) const;
   LayerStruct& getDefinedLayer(CString layerName,const CString& floatToLayerName) const;
   int getDefinedLayerIndex(const CString& layerName) const;
   int getFloatingLayerIndex();
   CString getFloatToLayerName() const;
   void setComponentPlacementLayerName(const CString& layerName);
   void clearComponentPlacementLayerName();
   void setContactLayerName(const CString& layerName);
   void clearContactLayerName();
   bool isComponentSideName(const CString& name) const;
   bool isSolderSideName(const CString& name) const;
   CString getNormalizedLayerName(const CString layerName) const;
   CString getDenormalizedLayerName(const CString layerName) const;
   void setConstructingComponentGeometryFlag(bool flag);
   void setConstructingPadstackGeometryFlag(bool flag);
   CString adjustCase(const CString& string) const;
   CString removeSignature(CString& geometryName) const;
   void fixupGeometryNames();
   bool containsBlock(const CString& blockName);

   void setOptionAdjustLayerNames(bool option);

   bool getOptionRectifyComponents() const;
   void setOptionRectifyComponents(bool option);

   bool getOptionRectifyViasAndTestPoints() const;
   void setOptionRectifyViasAndTestPoints(bool option);

   bool getOptionPurgeUnusedWidthsAndGeometries() const;
   void setOptionPurgeUnusedWidthsAndGeometries(bool option);

   bool getOptionPurgeUnusedLayers() const;
   void setOptionPurgeUnusedLayers(bool option);

   bool getOptionFixupGeometryNames() const;
   void setOptionFixupGeometryNames(bool option);

   CString getComponentTemplateGeometryName(const CString& suffix) const;
   CString getPadStackTemplateGeometryName(const CString& suffix) const;
   CString getPadStackGeometryName(const CString& suffix) const;
   CString getBondLandGeometryName(const CString& componentGeometryName,const CString& pinName,int wireNumber) const;
   CString getWireGeometryName(const CString& componentGeometryName,const CString& pinName,int wireNumber) const;
   CString removeComponentPrefixes(const CString& geometryName) const;

   void addVia(DataStruct& viaData);
   void addTestPoint(DataStruct& testPointData);
   void addReferencePad(DataStruct& referencePadData);
   void rectifyComponents(CNetlistCompArray& netlistcomparray);
   void rectifyViasAndTestPoints();

   BlockStruct& getRestructuredPadStacksBlock();
   void releaseRestructuredPadStacksBlock();
   void restructurePadStacks();
   void restructurePadStack(BlockStruct& block);

   void pushSection(const CString& sectionName);
   void popSection();
   void setMaterial(const CString& material);
   CString getMaterial() const;

};

//_____________________________________________________________________________

static int fnull(const char *s, int log);  

static int      start_theda();
static int      start_panel();

// this is not used anymore
/*
static List  start_lst[] =
{
   "PC_BOARD",                start_theda,
   "PANEL",                   start_panel,
};

#define  SIZ_START_LST       (sizeof(start_lst) / sizeof(List))
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   theda_identifier();
static   int   theda_description();
static   int   theda_unit();
static   int   theda_working_area();
static   int   theda_properties();
static   int   theda_design_objects();
static   int   theda_machine_objects();
static   int   theda_set_up();
static   int   theda_net_list();
static   int   theda_physical_layout();
static   int   theda_postprocess();
static   int   theda_postprocess_sets();
static   int   theda_parameters();
static   int   theda_version();
static   int   theda_library();
static   int   tl_file_technique();
static   int   theda_design_defaults();

static List  theda_lst[] =
{
   "IDENTIFIER",              theda_identifier,
   "DESCRIPTION",             theda_description,
   "VERSION",                 theda_version,
   "UNIT",                    theda_unit,
   "WORKING_AREA",            theda_working_area,
   "TECHNIQUE",               tl_file_technique,// new version library 7 and board 6
   "LIBRARY",                 theda_library,    // tl version 1
   "PROPERTIES",              theda_properties,
   "DESIGN_DEFAULTS",         theda_design_defaults,
   "DESIGN_OBJECTS",          theda_design_objects,
   "MACHINE_OBJECTS",         theda_machine_objects,
   "SET_UP",                  theda_set_up,
   "NET_LIST",                theda_net_list,
   "PHYSICAL_LAYOUT",         theda_physical_layout,
   "POSTPROCESS",             theda_postprocess,
   "PARAMETERS",              theda_parameters,
   "POSTPROCESS_SETS",        theda_postprocess_sets,
};

#define  SIZ_THEDA_LST       (sizeof(theda_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   panel_identifier();
static   int   panel_pc_boards();

static List  panel_lst[] =
{
   "IDENTIFIER",              panel_identifier,
   "VERSION",                 theda_version,
   "UNIT",                    theda_unit,
   "DESIGN_DEFAULTS",         theda_design_defaults,  // tl version 2
   "DESIGN_OBJECTS",          theda_design_objects,   // tl version 2
   "LIBRARY",                 theda_library,          // tl version 1
   "NET_LIST",                theda_net_list,
   "PHYSICAL_LAYOUT",         theda_physical_layout,
   "POSTPROCESS",             theda_postprocess,
   "PC_BOARDS",               panel_pc_boards,        // tl version 1
};

#define  SIZ_PANEL_LST       (sizeof(panel_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   version_revision();
static   int   version_comment();
static   int   version_software_revision();
static   int   version_software_comment();
static   int   version_library_revision();
static   int   version_library_comment();
static   int   version_board_revision(), version_board_comment();
static   int   version_creation_date();

static List  version_lst[] =
{
   "TL_REVISION",             version_revision,
   "TL_COMMENT",              version_comment,
   "SOFTWARE_REVISION",       version_software_revision,
   "SOFTWARE_COMMENT",        version_software_comment,
   "LIBRARY_REVISION",        version_library_revision,
   "LIBRARY_COMMENT",         version_library_comment,
   "BOARD_REVISION",          version_board_revision,
   "BOARD_COMMENT",           version_board_comment,
   "CREATION_DATE",           version_creation_date,
};

#define  SIZ_VERSION_LST       (sizeof(version_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   physical_layout_conduction_dividers();
static   int   physical_layout_set_up();
static   int   physical_layout_layer_structure();
static   int   physical_layout_outlines();
static   int   physical_layout_cutting_marks();
static   int   physical_layout_keepins();
static   int   physical_layout_keepouts();
static   int   physical_layout_components();
static   int   physical_layout_signals(), physical_layout_copper_areas();
static   int   physical_layout_reference_pads(), physical_layout_inverted_conductions();
static   int   physical_layout_drawings();
static   int   physical_layout_conductions();
static   int   physical_layout_violations();
static   int   physical_layout_drill_holes();
static   int   physical_layout_measurements();
static   int   physical_layout_post_processing();

static List  physical_layout_lst[] =
{
   "SET_UP",                  physical_layout_set_up,
   "LAYER_STRUCTURE",         physical_layout_layer_structure,
   "OUTLINES",                physical_layout_outlines,
   "CUTTING_MARKS",           physical_layout_cutting_marks,
   "KEEPINS",                 physical_layout_keepins,
   "KEEPOUTS",                physical_layout_keepouts,
   "DRILL_HOLES",             physical_layout_drill_holes,
   "COMPONENTS",              physical_layout_components,
   "SIGNALS",                 physical_layout_signals,
   "COPPER_AREAS",            physical_layout_copper_areas,
   "INVERTED_CONDUCTIONS",    physical_layout_inverted_conductions,
   "REFERENCE_PADS",          physical_layout_reference_pads,
   "DRAWINGS",                physical_layout_drawings,
   "CONDUCTIONS",             physical_layout_conductions,
   "VIOLATIONS",              physical_layout_violations,
   "MEASUREMENTS",            physical_layout_measurements,
   "POST_PROCESSING",         physical_layout_post_processing,
   "PC_BOARDS",               panel_pc_boards,                    // tl version 2
};

#define  SIZ_PHYSICAL_LAYOUT_LST       (sizeof(physical_layout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   net_list_set_up();
static   int   net_list_components();
static   int   net_list_nets();

static List  net_list_lst[] =
{
   "SET_UP",                  net_list_set_up,
   "COMPONENTS",              net_list_components,
   "NETS",                    net_list_nets,
};

#define  SIZ_NET_LIST_LST       (sizeof(net_list_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   keepins_keepin();

static List  keepins_lst[] =
{
   "KEEPIN",                  keepins_keepin,
};

#define  SIZ_KEEPINS_LST       (sizeof(keepins_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   text_pens_text_pen();
static   int   text_pens_text_pen_1();

static List  text_pens_lst[] =
{
   "TEXT_PEN",                text_pens_text_pen,
   "PEN",                     text_pens_text_pen_1,   // version 1
};

#define  SIZ_TEXT_PENS_LST       (sizeof(text_pens_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   keepouts_keepout();

static List  keepouts_lst[] =
{
   "KEEPOUT",                 keepouts_keepout,
};

#define  SIZ_KEEPOUTS_LST       (sizeof(keepouts_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   dimensions_dimension();

static List  dimensions_lst[] =
{
   "DIMENSION",                 dimensions_dimension,
};

#define  SIZ_DIMENSIONS_LST       (sizeof(dimensions_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   outlines_outline();

static List  outlines_lst[] =
{
   "OUTLINE",                 outlines_outline,
};

#define  SIZ_OUTLINES_LST       (sizeof(outlines_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   cutting_marks_cutting_mark();

static List  cutting_marks_lst[] =
{
   "CUTTING_MARK",                  cutting_marks_cutting_mark,
};

#define  SIZ_CUTTING_MARKS_LST       (sizeof(cutting_marks_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   drawings_drawing();

static List  drawings_lst[] =
{
   "DRAWING",                       drawings_drawing,
};

#define  SIZ_DRAWINGS_LST       (sizeof(drawings_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   text_nodes_text_node();

static List  text_nodes_lst[] =
{
   "TEXT_NODE",                     text_nodes_text_node,
};

#define  SIZ_TEXT_NODES_LST     (sizeof(text_nodes_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pins_pin();

static List  pins_lst[] =
{
   "PIN",                        pins_pin,
};

#define  SIZ_PINS_LST       (sizeof(pins_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   wires_wire();

static List  wires_lst[] =
{
   "WIRE",                       wires_wire,
};

#define  SIZ_WIRES_LST       (sizeof(wires_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   chip_bond_lands_chip_bond_land();

static List  chip_bond_lands_lst[] =
{
   "CHIP_BOND_LAND",          chip_bond_lands_chip_bond_land,
};

#define  SIZ_CHIP_BOND_LANDS_LST (sizeof(chip_bond_lands_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   properties_property();

static List  properties_lst[] =
{
   "PROPERTY",                   properties_property,
};

#define  SIZ_PROPERTIES_LST (sizeof(properties_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   vias_via();

static List  vias_lst[] =
{
   "VIA",                        vias_via,
};

#define  SIZ_VIAS_LST       (sizeof(vias_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   test_points_test_point();

static List  test_points_lst[] =
{
   "TEST_POINT",                 test_points_test_point,
};

#define  SIZ_TEST_POINTS_LST       (sizeof(test_points_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   net_pins_pin();

static List  net_pins_lst[] =
{
   "PIN",                        net_pins_pin,
};

#define  SIZ_NET_PINS_LST       (sizeof(net_pins_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   conductions_conduction();

static List  conductions_lst[] =
{
   "CONDUCTION",                 conductions_conduction,
};

#define  SIZ_CONDUCTIONS_LST  (sizeof(conductions_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   conductions_inverted_conduction();

static List  inverted_conductions_lst[] =
{
   "INVERTED_CONDUCTION",                 conductions_inverted_conduction,
};

#define  SIZ_INVERTED_CONDUCTIONS_LST  (sizeof(inverted_conductions_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphics_graphic();

static List  graphics_lst[] =
{
   "GRAPHIC",                       graphics_graphic,
};

#define  SIZ_GRAPHICS_LST       (sizeof(graphics_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   net_components_component();

static List  net_components_lst[] =
{
   "COMPONENT",                     net_components_component,
};

#define  SIZ_NET_COMPONENTS_LST       (sizeof(net_components_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   net_nets_net();
static   int   net_nets_merged();

static List  net_nets_lst[] =
{
   "NET",                     net_nets_net,
   "MERGED",                  net_nets_merged,
};

#define  SIZ_NET_NETS_LST       (sizeof(net_nets_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   theda_attributes();
static   int   theda_layer();
static   int   theda_symbol_type(), theda_symbol_type_list();
static   int   theda_filled_areas_set();

static   int   theda_need();
static   int   theda_material();
static   int   theda_height();
static   int   theda_z_offset();
static   int   theda_fixed();
static   int   theda_x_mirror();
static   int   theda_pen();
static   int   theda_area_pen();
static   int   theda_line_pen();
static   int   theda_close_contour(), theda_open_contour();
static   int   theda_polygon();
static   int   theda_filled_polygon();
static   int   theda_line();
static   int   theda_cspline();
static   int   theda_circle(), theda_filled_circle(), theda_arc();
static   int   theda_offset();
static   int   theda_rotation();
static   int   theda_shape_id(), theda_padstack_id();
static   int   theda_rectangle(), theda_triangle(), theda_parallelogram();
static   int   theda_filled_rectangle(), theda_filled_triangle();
static   int   theda_point();
static   int   theda_smooth_polygon();
static   int   theda_filled_smooth_polygon();
static   int   theda_text();
static   int   theda_technique();
static   int   symbol_identifier();
static   int   graphic_place(), graphic_contact();

static   int   theda_ate_probe();
static   int   theda_bareboard_probe();
static   int   theda_diagnostic_probe();

static List  tlgraphic_lst[] =
{
   "IDENTIFIER",              symbol_identifier,   // Version 1
   "ATTRIBUTES",              theda_attributes,
   "TECHNIQUE",               theda_technique,
   "LAYER",                   theda_layer,
   "SYMBOL_TYPE",             theda_symbol_type,
   "SYMBOL_TYPE_LIST",        theda_symbol_type_list,
   "PEN",                     theda_pen,           // Version 1
   "AREA_PEN",                theda_area_pen,
   "LINE_PEN",                theda_line_pen,
   "TRACE_PEN",               theda_line_pen,      // Version 1
   "CLOSE_CONTOUR",           theda_close_contour,
   "OPEN_CONTOUR",            theda_open_contour,
   "POLYGON",                 theda_polygon,
   "FILLED_POLYGON",          theda_filled_polygon,
   "LINE",                    theda_line,
   "CSPLINE",                 theda_cspline,
   "ARC",                     theda_arc,
   "CIRCLE",                  theda_circle,
   "FILLED_CIRCLE",           theda_filled_circle,
   "RECTANGLE",               theda_rectangle,
   "PARALLELOGRAM",           theda_parallelogram,
   "TRIANGLE",                theda_triangle,
   "FILLED_TRIANGLE",         theda_filled_triangle,
   "FILLED_RECTANGLE",        theda_filled_rectangle,
   "POINT",                   theda_point,
   "OFFSET",                  theda_offset,
   "ROTATION",                theda_rotation,
   "SHAPE_ID",                theda_shape_id,
   "SYMBOL_ID",               theda_shape_id,   // symbol_id and shape_id seems
                                                // to be the same
   "PADSTACK_ID",             theda_padstack_id,
   "SMOOTH_POLYGON",          theda_smooth_polygon,
   "FILLED_SMOOTH_POLYGON",   theda_filled_smooth_polygon,
   "FILLED_AREAS_SET",        theda_filled_areas_set,
   "TEXT",                    theda_text,
   "PLACE",                   graphic_place,    // version 1
   "CONTACT",                 graphic_contact,     // version 1
   "ATE_PROBE",               theda_ate_probe,
   "BAREBOARD_PROBE",         theda_bareboard_probe,
   "DIAGNOSTIC_PROBE",        theda_diagnostic_probe,
   "FIXED",                   theda_fixed,
   "NEED",                    theda_need,
   "MATERIAL",                theda_material,
   "HEIGHT",                  theda_height,
   "Z_OFFSET",                theda_z_offset,
   "X_MIRROR",                theda_x_mirror,
};

#define  SIZ_TLGRAPHIC_LST       (sizeof(tlgraphic_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   circle_line_pen();
static   int   circle_center();
static   int   circle_radius();

static List  tlcircle_lst[] =
{
   "AREA_PEN",                theda_area_pen,
   "LINE_PEN",                theda_line_pen,
   "CENTER",                  circle_center,
   "RADIUS",                  circle_radius,
};

#define  SIZ_TLCIRCLE_LST       (sizeof(tlcircle_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   arc_first_point(), arc_last_point(), arc_radius(), arc_counter_clockwise(), arc_clockwise();
static   int   arc_larger_semi(), arc_smaller_semi();

static List  tlarc_lst[] =
{
   "LINE_PEN",                theda_line_pen,
   "FIRST_POINT",             arc_first_point,
   "LAST_POINT",              arc_last_point,
   "RADIUS",                  arc_radius,
   "COUNTER_CLOCKWISE",       arc_counter_clockwise,
   "CLOCKWISE",               arc_clockwise,
   "SMALLER_SEMICIRCLE",      arc_smaller_semi,
   "LARGER_SEMICIRCLE",       arc_larger_semi,
};

#define  SIZ_TLARC_LST       (sizeof(tlarc_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   text_textpen();
static   int   text_string();
static   int   text_type();
static   int   text_location();
static   int   text_rotation();
static   int   text_xAlign();
static   int   text_yAlign();
static   int   text_x_mirror();
static   int   text_place();
static   int   text_attributes();

static List  tltext_lst[] =
{
   "LAYER",                   theda_layer,
   "TEXT_PEN",                text_textpen,
   "PEN",                     text_pens_text_pen_1,   // version 1
   "STRING",                  text_string,
   "TYPE",                    text_type,
   "LOCATION",                text_location,
   "ROTATION",                text_rotation,
   "X_ALIGN",                 text_xAlign,
   "Y_ALIGN",                 text_yAlign,
   "X_MIRROR",                text_x_mirror,
   "SYMBOL_TYPE",             theda_symbol_type,
   "PLACE",                   text_place,
   "ATTRIBUTES",              theda_attributes,
};

#define  SIZ_TLTEXT_LST       (sizeof(tltext_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   textpen_number();
static   int   textpen_font();
static   int   textpen_height();
static   int   textpen_width();
static   int   textpen_slant();
static   int   textpen_typeface();
static   int   textpen_character_spacing();
static   int   textpen_add_clearance();
static   int   textpen_line_spacing();
static   int   textpen_min_device_height();
static   int   textpen_max_device_height();

static List  tltextpen_lst[] =
{
   "NUMBER",                  textpen_number,
   "ATTRIBUTES",              theda_attributes,
   "FONT",                    textpen_font,
   "HEIGHT",                  textpen_height,
   "WIDTH",                   textpen_width,
   "SLANT",                   textpen_slant,
   "TYPEFACE",                textpen_typeface,
   "CHARACTER_SPACING",       textpen_character_spacing,
   "LINE_SPACING",            textpen_line_spacing,
   "ADD_CLEARANCE",           textpen_add_clearance,
   "MIN_DEVICE_HEIGHT",       textpen_min_device_height,
   "MAX_DEVICE_HEIGHT",       textpen_max_device_height,
};

#define  SIZ_TLTEXTPEN_LST       (sizeof(tltextpen_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   design_objects_pens();
static   int   design_objects_shapes(), design_objects_symbols();
static   int   design_objects_padstacks();
static   int   design_objects_packages();
static   int   design_objects_devices();
static   int   design_objects_components();
static   int   design_objects_templates();
static   int   design_objects_rules();

static List  design_objects_lst[] =
{
   "PENS",                    design_objects_pens,
   "SHAPES",                  design_objects_shapes,
   "SYMBOLS",                 design_objects_symbols,
   "PADSTACKS",               design_objects_padstacks,
   "PACKAGES",                design_objects_packages,
   "DEVICES",                 design_objects_devices,
   "COMPONENTS",              design_objects_components,
   "TEMPLATES",               design_objects_templates,
   "RULES",                   design_objects_rules,
};

#define  SIZ_DESIGN_OBJECTS_LST  (sizeof(design_objects_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   library_symbols();
static   int   library_padstacks();

static List  library_lst[] =           // tl version 1.1
{
   "PENS",                    design_objects_pens,
   "SYMBOLS",                 library_symbols,
   "PADSTACKS",               library_padstacks,
   "PACKAGES",                design_objects_packages,
   "DEVICES",                 design_objects_devices,
   "COMPONENTS",              design_objects_components,
};

#define  SIZ_LIBRARY_LST  (sizeof(library_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   parameters_current_parameters();
static   int   parameters_parameter_sets();

static List  parameter_lst[] =            // this is needed, because inside PARAMETER is a READ which has a parameter
{
   "CURRENT_PARAMETERS",      parameters_current_parameters,
   "PARAMETER_SETS",          parameters_parameter_sets,
};

#define  SIZ_PARAMETER_LST  (sizeof(parameter_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   current_parameters_analyze();
static   int   current_parameters_autoplace();
static   int   current_parameters_autorename();
static   int   current_parameters_autoroute();
static   int   current_parameters_check();
static   int   current_parameters_cutout();
static   int   current_parameters_define();
static   int   current_parameters_display();
static   int   current_parameters_move();
static   int   current_parameters_postprocess();
static   int   current_parameters_postroute();
static   int   current_parameters_postplace();
static   int   current_parameters_read();
static   int   current_parameters_write();
static   int   current_parameters_swap();
static   int   current_parameters_select();
static   int   current_parameters_search();
static   int   current_parameters_resolve();
static   int   current_parameters_route();

static List  current_parameter_lst[] =    // this is needed, because inside PARAMETER is a READ which has a parameter
{
   "ANALYZE",     current_parameters_analyze,
   "AUTOPLACE",   current_parameters_autoplace,
   "AUTORENAME",  current_parameters_autorename,
   "AUTOROUTE",   current_parameters_autoroute,
   "CHECK",       current_parameters_check,
   "CUTOUT",      current_parameters_cutout,
   "DEFINE",      current_parameters_define,
   "DISPLAY",     current_parameters_display,
   "MOVE",        current_parameters_move,
   "POSTPROCESS", current_parameters_postprocess,
   "POSTPLACE",   current_parameters_postplace,
   "POSTROUTE",   current_parameters_postroute,
   "READ",        current_parameters_read,
   "WRITE",       current_parameters_write,
   "SWAP",        current_parameters_swap,
   "SELECT",      current_parameters_select,
   "SEARCH",      current_parameters_search,
   "RESOLVE",     current_parameters_resolve,
   "ROUTE",       current_parameters_route,
};

#define  SIZ_CURRENT_PARAMETER_LST  (sizeof(current_parameter_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   parameterset_parameter();

static List  parametersets_lst[] =     // this is needed, because inside PARAMETER is a READ which has a parameter
{
   "PARAMETER_SET",     parameterset_parameter,
};

#define  SIZ_PARAMETERSETS_LST  (sizeof(parametersets_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   write_tl(), write_dxf();

static List  write_lst[] =    // this is needed, because inside WRITE is a  which has a parameter
{
   "TL",          write_tl,
   "DXF",         write_dxf,
};

#define  SIZ_WRITE_LST  (sizeof(write_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int  pc_boards_pc_board();

static List  pc_boards_lst[] =            // tl version 1.1
{
   "PC_BOARD",                pc_boards_pc_board,
};

#define  SIZ_PC_BOARDS_LST  (sizeof(pc_boards_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pc_board_location();
static   int   pc_board_id();

static List  pc_board_lst[] =          // tl version 1.1
{
   "ROTATION",                theda_rotation,
   "LOCATION",                pc_board_location,
   "BOARD_ID",                pc_board_id,
};

#define  SIZ_PC_BOARD_LST  (sizeof(pc_board_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pens_line_pens();
static   int   pens_text_pens();

static List  pens_lst[] =
{
   "LINE_PENS",                  pens_line_pens,
   "TEXT_PENS",                  pens_text_pens,
};

#define  SIZ_PENS_LST       (sizeof(pens_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   shapes_shape();

static List  shapes_lst[] =
{
   "SHAPE",                shapes_shape,
};

#define  SIZ_SHAPES_LST       (sizeof(shapes_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   symbols_symbol();

static List  symbols_lst[] =
{
   "AREA_PEN",             theda_area_pen,
   "LINE_PEN",             theda_line_pen,
   "TEXT_PEN",             text_textpen,
   "SYMBOL",               symbols_symbol,
};

#define  SIZ_SYMBOLS_LST       (sizeof(symbols_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   symbols_1_symbol();

static List  symbols_1_lst[] =
{
   "AREA_PEN",             theda_area_pen,
   "LINE_PEN",             theda_line_pen,
   "TEXT_PEN",             text_textpen,
   "SYMBOL",               symbols_1_symbol,
};

#define  SIZ_SYMBOLS_1_LST       (sizeof(symbols_1_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   shape_identifier();
static   int   shape_geometries();
static   int   shape_properties();

static List  tlshape_lst[] =
{
   "IDENTIFIER",           shape_identifier,
   "GEOMETRIES",           shape_geometries,
   "PROPERTIES",           shape_properties,
};

#define  SIZ_TLSHAPE_LST       (sizeof(tlshape_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   packages_package();

static List  packages_lst[] =
{
   "AREA_PEN",             theda_area_pen,
   "LINE_PEN",             theda_line_pen,
   "TEXT_PEN",             text_textpen,
   "PACKAGE",              packages_package,
};

#define  SIZ_PACKAGES_LST       (sizeof(packages_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   package_identifier();
static   int   package_drawings();
static   int   package_pins(), package_terminal_areas();
static   int   package_text_nodes();
static   int   package_type();
static   int   package_properties();
static   int   package_number_of_pins();
static   int   package_pin_names();
static   int   package_dimensions();
static   int   package_conductions();
static   int   package_insertion();
static   int   package_mounting_holes();
static   int   package_keepouts();

static List  tlpackage_lst[] =
{
   "IDENTIFIER",           package_identifier,
   "TYPE",                 package_type,
   "DRAWINGS",             package_drawings,
   "PINS",                 package_pins,
   "PROPERTIES",           package_properties,
   "TERMINAL_AREAS",       package_terminal_areas, 
   "TEXT_NODES",           package_text_nodes,
   "NUMBER_OF_PINS",       package_number_of_pins,
   "PIN_NAMES",            package_pin_names,
   "DIMENSIONS",           package_dimensions,
   "CONDUCTIONS",          package_conductions,
   "INSERTION",            package_insertion,
   "MOUNTING_HOLES",       package_mounting_holes,
   "KEEPOUTS",             package_keepouts,
};

#define  SIZ_TLPACKAGE_LST       (sizeof(tlpackage_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   comp_package_pins();
static   int   comp_package_text_nodes();
static   int   comp_package_drawings();

static List  comp_package_lst[] =
{
   "PINS",                 comp_package_pins,   
   "TEXT_NODES",           comp_package_text_nodes,
   "DRAWINGS",             comp_package_drawings,
};

#define  SIZ_COMP_PACKAGE_LST       (sizeof(comp_package_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   comp_package_pin();

static List  comp_package_pins_lst[] =
{
   "PIN",                 comp_package_pin,   
};

#define  SIZ_COMP_PACKAGE_PINS_LST       (sizeof(comp_package_pins_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pin_identifier();
static   int   pin_number();
static   int   pin_location();
static   int   pin_rotation();
static   int   comp_package_pin_contacted();
static   int   comp_package_pin_probePads();

static List  comp_package_pin_lst[] =
{
   "IDENTIFIER",           pin_identifier,
   "NUMBER",               pin_number,
   "CONTACTED",            comp_package_pin_contacted,
   "LOCATION",             pin_location,
   "ROTATION",             pin_rotation,
   "PROBE_PADS",           comp_package_pin_probePads,   
};

#define  SIZ_COMP_PACKAGE_PIN_LST       (sizeof(comp_package_pin_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   comp_package_pin_probePad();

static List  comp_package_probePads_lst[] =
{
   "PROBE_PAD",               comp_package_pin_probePad,   
};

#define  SIZ_COMP_PACKAGE_PIN_PROBEPADS_LST       (sizeof(comp_package_probePads_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   via_probePad();

static List  via_probePads_lst[] =
{
   "PROBE_PAD",               via_probePad,   
};

#define  SIZ_VIA_PROBEPADS_LST       (sizeof(via_probePads_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   comp_package_pin_probePad_refdes();
static   int   comp_package_pin_probePad_layer();
static   int   comp_package_pin_probePad_textNodes();

static List  comp_package_probePad_lst[] =
{
   "REFERENCE_DESIGNATOR",    comp_package_pin_probePad_refdes,   
   "LAYER",                   comp_package_pin_probePad_layer,   
   "TEXT_NODES",              comp_package_pin_probePad_textNodes,   
};

#define  SIZ_COMP_PACKAGE_PIN_PROBEPAD_LST       (sizeof(comp_package_probePad_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   probe_pad_text_node();

static List  comp_package_probePads_textNodes_lst[] =
{
   "TEXT_NODE",                     probe_pad_text_node,
};

#define  SIZ_COMP_PACKAGE_PIN_PROBEPADS_TEXTNODES_LST       (sizeof(comp_package_probePads_textNodes_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// this will do the new pin definition, and not skip them !
static List  comp_package1_lst[] =
{
   "PINS",                 package_pins,  
   "TEXT_NODES",           comp_package_text_nodes,
   "DRAWINGS",             comp_package_drawings,
};

#define  SIZ_COMP_PACKAGE1_LST       (sizeof(comp_package1_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   components_component();

static List  components_lst[] =
{
   "COMPONENT",            components_component,
};

#define  SIZ_COMPONENTS_LST       (sizeof(components_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   component_identifier();
static   int   component_package_id();
static   int   component_device_id();
static   int   component_properties();

static List  tlcomponent_lst[] =
{
   "IDENTIFIER",           component_identifier,
   "PACKAGE_ID",           component_package_id,
   "DEVICE_ID",            component_device_id,
   "PROPERTIES",           component_properties,
};

#define  SIZ_TLCOMPONENT_LST       (sizeof(tlcomponent_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   via_needed();
static   int   via_contacted(); 

static   int   pin_padstack_id();
static   int   pin_wires();
static   int   pin_graphics();
static   int   pin_chip_bond_lands();
static   int   pin_text_nodes();
static   int   via_tp_probes_probe();
static   int   probes_probe();
static   void  makePhysPinLoc();
static   int   pin_probe_pads();
static   int   pin_x_mirror();
static   int   pin_mark();

static List  tlpin_lst[] =
{
   "IDENTIFIER",           pin_identifier,
   "NUMBER",               pin_number,
   "LOCATION",             pin_location,
   "ROTATION",             pin_rotation,
   "PADSTACK_ID",          pin_padstack_id,
   "WIRES",                pin_wires,              // do not write if pulled_pins
   "GRAPHICS",             pin_graphics,           // do not write if pulled_pins
   "CHIP_BOND_LANDS",      pin_chip_bond_lands,    // do not write if pulled_pins
   "TEXT_NODES",           pin_text_nodes,
   "NEEDED",               via_needed,
   "CONTACTED",            via_contacted,
   "PROBE_PADS",           comp_package_pin_probePads,
   "X_MIRROR",             pin_x_mirror,
   "MARK",                 pin_mark,
};

#define  SIZ_TLPIN_LST       (sizeof(tlpin_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   prop_number();
static   int   prop_identifier();
static   int   prop_description();

static List  tlproperty_lst[] =
{
   "NUMBER",               prop_number,
   "IDENTIFIER",           prop_identifier,
   "DESCRIPTION",          prop_description,
};

#define  SIZ_TLPROPERTY_LST  (sizeof(tlproperty_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   via_number();
static   int   via_x_mirror();
static   int   via_layer();
static   int   via_location();
static   int   via_rotation();
static   int   via_graphics(); 
// static   int   via_needed(); defined upper.
// static   int   via_contacted(); defined upper
static   int   via_padstack_id();
static   int   via_testpoint_probe_pads();
static   int   via_probePads();

static List  tlvia_lst[] =
{
   "NUMBER",               via_number,
   "LAYER",                via_layer,
   "LOCATION",             via_location,
   "ROTATION",             via_rotation,
   "NEEDED",               via_needed,
   "CONTACTED",            via_contacted,
   "PADSTACK_ID",          via_padstack_id,
   "FIXED",                theda_fixed,
   "X_MIRROR",             via_x_mirror,
   "PROBE_PADS",           via_probePads,
   "GRAPHICS",             via_graphics,
};

#define  SIZ_TLVIA_LST       (sizeof(tlvia_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   testPoint_layer();
static   int   testPoint_contacted();

static List  tltestPoint_lst[] =
{
   "NUMBER",               via_number,
   "LAYER",                testPoint_layer,
   "LOCATION",             via_location,
   "ROTATION",             via_rotation,
   "NEEDED",               via_needed,
   "CONTACTED",            testPoint_contacted,
   "PADSTACK_ID",          via_padstack_id,
   "FIXED",                theda_fixed,
   "X_MIRROR",             via_x_mirror,
   "PROBE_PADS",           via_probePads,
   "GRAPHICS",             via_graphics,
};

#define  SIZ_TLTestPoint_LST       (sizeof(tltestPoint_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   refPad();

static List  tlRefPads_lst[] =
{
   "REFERENCE_PAD",        refPad,
};

#define  SIZ_TLREFPADS_LST       (sizeof(tlRefPads_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   refPad_mark();
static   int   refPad_layer();
static   int   refPad_location();
static   int   refPad_rotation();
static   int   refPad_padstack_id();
static   int   refPad_x_mirror();
static   int   refPad_graphics();

static List  tlRefPad_lst[] =
{
   "MARK",                 refPad_mark,
   "LAYER",                refPad_layer,
   "LOCATION",             refPad_location,
   "ROTATION",             refPad_rotation,
   "PADSTACK_ID",          refPad_padstack_id,
   "X_MIRROR",             refPad_x_mirror,
   "GRAPHICS",             refPad_graphics,
};

#define  SIZ_TLREFPAD_LST       (sizeof(tlRefPad_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*****************************************************************************/
/*
!  !  @  NET[ 14 ]
!  !  @  !  IDENTIFIER              := '$4N182';
!  !  @  !  PINS
!  !  @  !  !  PIN[ 1 ]
!  !  @  !  !  @  REFERENCE_DESIGNATOR    := 'L8';
!  !  @  !  !  @  PIN_ID                  := '2';
!  !  @  !  !  @..FIN_PIN[ 1 ];
!  !  @  !  !  PIN[ 2 ]
!  !  @  !  !  @  REFERENCE_DESIGNATOR    := 'T2';
!  !  @  !  !  @  PIN_ID                  := '16';
!  !  @  !  !  @..FIN_PIN[ 2 ];
!  !  @  !  !  PIN[ 3 ]
!  !  @  !  !  @  REFERENCE_DESIGNATOR    := 'U4';
!  !  @  !  !  @  PIN_ID                  := '1';
!  !  @  !  !  @  PROBE_REFDES_TYPE       := ( 'ATE68', ATE );
!  !  @  !  !  @..FIN_PIN[ 3 ];
!  !  @  !  !  PIN[ 4 ]
!  !  @  !  !  @  REFERENCE_DESIGNATOR    := 'U4';
!  !  @  !  !  @  PIN_ID                  := '8';
!  !  @  !  !  @..FIN_PIN[ 4 ];
!  !  @  !  !..FIN_PINS;
!  !  @  !  #-----------------------------#
!  !  @  !     PROBE_REF_DES       PROBE   
!  !  @  !  #-----------------------------#
!  !  @  !        'ATE13'           ATE    
!  !  @  !  #-----------------------------#
!  !  @  !..FIN_NET[ 14 ];

alternative format

!  !  NETS
!  !  @  NET[ 1 ]
!  !  @  !  IDENTIFIER              := 'n#100';
!  !  @  !  #-----------------------------------------------------------------#
!  !  @  !       REFERENCE_DESIGNATOR        PIN_ID   SCH_SYMBOL_ID  SCH_PIN_ID
!  !  @  !  #-----------------------------------------------------------------#
!  !  @  !              'B11'                 'C'           50            2
!  !  @  !              'A11'                 'C'           23            2
!  !  @  !  #-----------------------------------------------------------------#
!  !  @  !  #----------------------------------------------------------------#
!  !  @  !  REFERENCE_DESIGNATOR    PIN_ID        PROBE_REF_DES       PROBE   
!  !  @  !  #----------------------------------------------------------------#
!  !  @  !         'B11'              'C'            '*ate*'           ATE    
!  !  @  !         'A11'              'C'            '*ate*'           ATE    
!  !  @  !  #----------------------------------------------------------------#
!  !  @  !..FIN_NET[ 1 ];

*/

static   int   net_unconnected();
static   int   net_identifier();
static   int   net_pins();
static   int   net_probe_ref_des();
static   int   net_voltage(), net_power(), net_ground();
static   int   net_probepad();
static   int   net_routing_rules();
static   int   net_parseTable();

static List  tlnet_lst[] =
{
   "IDENTIFIER",           net_identifier,
   "PINS",                 net_pins,
   "PROBE_REF_DES",        net_probe_ref_des,
   "VOLTAGE",              net_voltage,
   "POWER",                net_power,
   "GROUND",               net_ground,
   "ROUTING_RULES",        net_routing_rules,
   "UNCONNECTED",          net_unconnected,
   "#------",              net_parseTable,
};

#define  SIZ_TLNET_LST       (sizeof(tlnet_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   net_comp_number(), net_comp_reference();
static   int   net_comp_type();
static   int   net_comp_lib_id();
static   int   net_comp_package_id();
static   int   net_comp_pin_id();
static   int   net_comp_comp_rules();
static   int   net_comp_pulled_pins();
static   int   net_comp_properties();
static   int   net_comp_pulled_mounting_holes();

static List  net_comp_lst[] =
{
   "NUMBER",               net_comp_number,
   "REFERENCE_DESIGNATOR", net_comp_reference,
   "TYPE",                 net_comp_type,
   "LIBRARY_IDENTIFIER",   net_comp_lib_id,
   "PACKAGE_ID",           net_comp_package_id,
   "PIN_IDS",              net_comp_pin_id,
   "COMPONENT_RULES",      net_comp_comp_rules,
   "PULLED_PINS",          net_comp_pulled_pins,
   "PULLED_MOUNTING_HOLES",net_comp_pulled_mounting_holes,
   "PROPERTIES",           net_comp_properties,
};

#define  SIZ_NET_COMP_LST       (sizeof(net_comp_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   net_comppin_reference();
static   int   net_comppin_pin_id();
static   int   net_comppin_schematic_sym_pin();
static   int   net_comppin_probe_pad();

static List  net_comppin_lst[] =
{
   "REFERENCE_DESIGNATOR", net_comppin_reference,
   "PIN_ID",               net_comppin_pin_id,
   "SCHEMATIC_SYM_PIN",    net_comppin_schematic_sym_pin,
   "PROBE_PAD",            net_comppin_probe_pad,
};

#define  SIZ_NET_COMPPIN_LST       (sizeof(net_comppin_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   phys_components_component();

static List  phys_components_lst[] =
{
   "COMPONENT",            phys_components_component,
};

#define  SIZ_PHYS_COMPONENTS_LST       (sizeof(phys_components_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   phys_signals_signal();
static   int   theda_line_default_pen();

static List  phys_signals_lst[] =
{
   "SIGNAL",               phys_signals_signal,
   "TRACE_PEN",            theda_line_default_pen,       // Version 1
};

#define  SIZ_PHYS_SIGNALS_LST       (sizeof(phys_signals_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   phys_comp_number();  
static   int   phys_comp_reference();  
static   int   phys_comp_placed();  
static   int   phys_comp_fixed();   
static   int   phys_comp_unplaced();   
static   int   phys_comp_layer();   
static   int   phys_comp_package(); 
static   int   phys_comp_location();   
static   int   phys_comp_rotation();   
static   int   phys_comp_placement();
static   int   phys_comp_pins();
static   int   phys_comp_x_mirror();
static   int   phys_comp_insertion_rules();

static List  phys_comp_lst[] =
{
   "NUMBER",               phys_comp_number,
   "REFERENCE_DESIGNATOR", phys_comp_reference,
   "PLACED",               phys_comp_placed,
   "FIXED",                phys_comp_fixed,
   "PLACEMENT",            phys_comp_placement, // V1.1
   "PINS",                 phys_comp_pins,      // local comp pins, not eval
   "UN_PLACED",            phys_comp_unplaced,
   "LAYER",                phys_comp_layer,
   "PACKAGE",              phys_comp_package,
   "LOCATION",             phys_comp_location,
   "ROTATION",             phys_comp_rotation,
   "X_MIRROR",             phys_comp_x_mirror,
   "INSERTION_RULES",      phys_comp_insertion_rules,
};

#define  SIZ_PHYS_COMP_LST       (sizeof(phys_comp_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   phys_comp_placement_grid();
static   int   phys_comp_placement_layer();

static List  phys_comp_placement_lst[] =
{
   "ON_GRID",              phys_comp_placement_grid,
   "OFF_GRID",             phys_comp_placement_grid,
   "OFF_GRID_MIXED",       phys_comp_placement_grid,
   "FIXED",                theda_fixed,
   "LAYER",                phys_comp_placement_layer,
   "LOCATION",             phys_comp_location,
   "ROTATION",             phys_comp_rotation,
};

#define  SIZ_PHYS_COMP_PLACEMENT_LST       (sizeof(phys_comp_placement_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   phys_signal_number(), phys_signal_identifier();
static   int   phys_signal_traces(), phys_signal_polygon();
static   int   phys_signal_copper_areas();
static   int   phys_signal_vias();
static   int   phys_signal_test_points();
static   int   phys_signal_conductions();
static   int   phys_signal_inverted_conductions();
static   int   phys_signal_incomplete_traces();

static List  phys_signal_lst[] =
{
   "NUMBER",               phys_signal_number,  // Version 1
   "IDENTIFIER",           phys_signal_identifier,
   "FIXED",                theda_fixed,
   "TRACES",               phys_signal_traces,
   "POLYGON",              phys_signal_polygon,    // Version 1
   "LINE",                 theda_line,             // Version 1
   "COPPER_AREAS",         phys_signal_copper_areas,
   "VIAS",                 phys_signal_vias,    
   "TEST_POINTS",          phys_signal_test_points,      
   "CONDUCTIONS",          phys_signal_conductions,// Version 5
   "INVERTED_CONDUCTIONS", phys_signal_inverted_conductions,// Version 5
   "LAYER",                theda_layer,
   "TRACE_PEN",            theda_line_pen,         // Version 1
   "INCOMPLETE_TRACES",    phys_signal_incomplete_traces,
};

#define  SIZ_PHYS_SIGNAL_LST       (sizeof(phys_signal_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   phys_copper_area();

static List phys_copper_areas_lst[] = 
{
   "COPPER_AREA",          phys_copper_area,
};

#define  SIZ_PHYS_COPPER_AREAS_LST       (sizeof(phys_copper_areas_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int padstacks_padstack();

static List  padstacks_lst[] =
{
   "AREA_PEN",             theda_area_pen,
   "LINE_PEN",             theda_line_pen,
   "TEXT_PEN",             text_textpen,
   "PADSTACK",             padstacks_padstack,
};

#define  SIZ_PADSTACKS_LST       (sizeof(padstacks_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int padstacks_1_padstack();

static List  padstacks_1_lst[] =             // tl version 1.1
{
   "AREA_PEN",             theda_area_pen,
   "LINE_PEN",             theda_line_pen,
   "TEXT_PEN",             text_textpen,
   "PADSTACK",             padstacks_1_padstack,
};

#define  SIZ_PADSTACKS_1_LST       (sizeof(padstacks_1_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   padstack_identifier();
static   int   padstack_graphics();
static   int   padstack_properties();

static List  tlpadstack_lst[] =
{
   "IDENTIFIER",           padstack_identifier,
   "GRAPHICS",             padstack_graphics,
   "PROPERTIES",           padstack_properties,
};

#define  SIZ_TLPADSTACK_LST       (sizeof(tlpadstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif
