// $Header: /CAMCAD/4.6/PcbUtil.h 39    3/19/07 2:42p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-2001. All Rights Reserved.
*/

#if !defined(__PcbUtil_H__)
#define __PcbUtil_H__

#include "file.h"
#include "geomlib.h"

#define DRCMARKERGEOM   "$drcmarker"
#define GLUEPOINTGEOM   "$gluepoint"
#define ACCESSPLACEGEOM "$accessmarker"

void set_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file);
void set_UnloadedComponentPNAttribute(CCEtoODBDoc *doc, FileStruct *file, CString DefaultPN);
void set_ComponentLoadedData(CCEtoODBDoc *doc, FileStruct *file);
void color_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file, int red, int green, int blue);

void AutoBoardOrigin(CCEtoODBDoc *doc, FileStruct *file, BOOL Left, BOOL Bottom);

void data_mem_calc(CCEtoODBDoc *doc);

// accuracy is the value from the page settings and the decimal accuracy.
double   get_accuracy(CCEtoODBDoc *Doc);

void GenerateMissingTypes(CCEtoODBDoc *doc, FileStruct *file);

// add_typelist checks if a typename already exists
TypeStruct *AddType(FileStruct *file, CString typeName);

// checks if a type is in the typelist and return it or returns NULL
TypeStruct *FindType(FileStruct *file, CString typeName);

// after a type is assigned to a geom, the typepinlist gets filled with the physical pin name.
// tjhe function returns the numbner of pins updated, -1 is blocknr not found
int update_typepin_from_geompin(TypeStruct *tl, int blocknr);

// get a physical device pin pointer 
TypePinStruct *add_typepin(TypeStruct *tl, const char *pin);

NetStruct *find_net(FileStruct *fl, const char *net_name);
NetStruct *add_net(FileStruct *fl,const char *net_name);

CompPinStruct *add_comppin(FileStruct *fl, NetStruct *n, const char *comp, const char *pin);

CompPinStruct *test_add_comppin(const char *c, const char *p, FileStruct *f);

DRCStruct *add_drc(FileStruct *fl,int drcCode, int algIndex);

int ProcessLoadedProbes(CCEtoODBDoc *doc);
BlockStruct* generate_TestAccessGeometry(CCEtoODBDoc *doc, const char *name, double size);
BlockStruct* generate_TestProbeGeometry(CCEtoODBDoc *doc, const char *name, double probediam, 
                                        const char *tool, double drillsize);
BlockStruct* generate_Gluepoint_Geom(CCEtoODBDoc *Doc);

void generate_NCnet(CCEtoODBDoc *Doc, FileStruct *f,COperationProgress* progress=NULL);
void generate_PINLOC(CCEtoODBDoc *Doc, FileStruct *f, int always,COperationProgress* progress=NULL); // this also performs generate_NCnet
void generate_PADSTACKACCESSFLAG(CCEtoODBDoc *Doc, BOOL Always,CDcaOperationProgress* progress=NULL);
void generate_PADSTACKSMDATTRIBUTE(CCEtoODBDoc *doc,bool recalculateAccessFlags=true,COperationProgress* progress=NULL);
bool generate_PADSTACKSMDATTRIBUTE(CCEtoODBDoc& doc,BlockStruct& block,bool recalculateAccessFlags=true);
void generate_SOLDERMASKATTRIBUTE(CCEtoODBDoc *doc, int always);
void generate_CENTROIDS(CCEtoODBDoc *doc);
void GeneratePinNumbers(CCEtoODBDoc *doc);
void GeneratePinPitch(CCEtoODBDoc *doc);

// this looks for complex apertures and finds out if these complex apertures are
// single polys or complex multi poly, line, breakout etc...
void generate_TRUESHAPEAPERTUREFLAG(CCEtoODBDoc *Doc);

void EliminateDuplicateVias(CCEtoODBDoc *Doc, BlockStruct *block);

// if etch_only then only polys with netnames are evaluated.
void EliminateOverlappingTraces(CCEtoODBDoc *Doc, BlockStruct *block, int etch_only, double accuracy);

void EliminateSinglePointPolys(CCEtoODBDoc *Doc);
void EliminateSmallPolys(CCEtoODBDoc *Doc, double accuracy);

void BreakSpikePolys(BlockStruct *Block);
void Crack(CCEtoODBDoc *doc, BlockStruct *block, int etch_only, COperationProgress* operationProgress=NULL);
void switch_on_pcbfiles_used_in_panels(CCEtoODBDoc *doc, int show);

DataStruct *datastruct_from_refdes(CCEtoODBDoc *doc, BlockStruct *block, const char *refdes);
DataStruct *get_pindata_from_comppinnr(CCEtoODBDoc  *doc, BlockStruct *b, int pinnr);

// this assignes a SMD flag to components.
int update_smdrule_components(CCEtoODBDoc *Doc, FileStruct *file, int SMDrule,COperationProgress* operationProgress=NULL);

// this assignes a SMD flag to PCB geometries.
int update_smdrule_geometries(CCEtoODBDoc *Doc, int SMDrule,COperationProgress* progress=NULL);
bool update_smdGEOMETRYData(CCEtoODBDoc& doc,BlockStruct& block,int smdRule);

// this assignes a SMD flag to PCB geometries and components
int update_smdrule(CCEtoODBDoc *Doc, int SMDrule);

// checks a padstack according to layertype.
int update_smdpads(CCEtoODBDoc *doc,COperationProgress* progress=NULL);

double get_drill_from_block_no_scale(CCEtoODBDoc *doc, BlockStruct *block);

// return the geom containing the drill info
BlockStruct *GetDrillInGeometry(CCEtoODBDoc *doc, BlockStruct *block);
BlockStruct *GetDrillInGeometry(CCamCadData& ccdata, BlockStruct *block);

// return the InsertStruct that inserting the drill
InsertStruct* GetDrillInsertInGeometry(CCEtoODBDoc* doc, BlockStruct* block);


// this gets the first padstack from the block. Use for getting fiducial or testpoint layers
BlockStruct *get_padstack_block(CCEtoODBDoc *doc, BlockStruct *block);

int Generate_Unique_DCodes(CCEtoODBDoc *Doc);
int Clear_DCodes(CCEtoODBDoc *Doc);
int Generate_Unique_TCodes(CCEtoODBDoc *Doc);
int check_tolerance(const char *att, double *mtol, double *ptol, double *tol);

int default_inserttype(int blocktype);

//DataStruct *FindDataEntity(CCEtoODBDoc *doc, unsigned long entityNum, CDataList **DataList);

void change_inserttypes_by_block(CCEtoODBDoc *doc, BlockStruct *block,int inserttype); 

// this returns if a layertype is considered electrical in CAMCAD.
int is_electricallayer(int layerType);
int is_layerbottom(int layerType);

// returns layer index
int Get_PADSTACKLayer(CCEtoODBDoc *doc, CDataList *DataList);
// returns 0..3 none, top, bottom, both
int Get_PADSTACKACCESSLayer(CCEtoODBDoc *doc, CDataList *DataList);

void remove_zero_size_pads(CCEtoODBDoc *doc);

// this flattens a padstack to toplevel apertures.
void flatten_padstack(CCEtoODBDoc *doc, BlockStruct *block);

// this optimizes complex padstacks into possible apertures.
void OptimizePadstacks(CCEtoODBDoc  *doc, int pageunits, BOOL DoPowerLayers,COperationProgress* operationProgress=NULL);

// this optimizes complex apertures into simple if possible. from inserts of a file
void OptimizeComplexApertures(CCEtoODBDoc *doc, CDataList *dataList, double accuracy, BOOL DoPowerLayers,COperationProgress* operationProgress=NULL);

// collects equal apertures and uses only 1 and deletes the other ones
void OptimizeDuplicateApertures(CCEtoODBDoc *doc, double accuracy);

// removes flashes that are completely covered by other flashes
void OptimizeCoveredApertures(CCEtoODBDoc *doc, CDataList *dataList);

// tests a block
int OptimizeComplexApertures_Blocks(CCEtoODBDoc *doc, BlockStruct *block, int nplayer, double accuracy, BOOL DoPowerLayers);

void update_manufacturingrefdes(CCEtoODBDoc *doc, FileStruct *file,BlockStruct& pcbBlock, 
                                    double minref, double maxref, int layernr, bool nevermirror);

void update_proberefdes(CCEtoODBDoc *doc, FileStruct *file,BlockStruct& pcbBlock, bool nevermirror);

void DoMakeManufacturingRefDes(CCEtoODBDoc *doc, BOOL ShowDialog, bool applyToComps = true, bool applyToProbes = false);

int get_pincnt(CDataList *b); 
int GetPinExtents(CDataList *dataList, double *xmin, double *ymin, double *xmax, double *ymax);
int GetPinCenterExtents(const CDataList& dataList, CInsertTypeMask includeInsertTypeMask, double *xmin, double *ymin, double *xmax, double *ymax);
int get_insidepadsextents(CCEtoODBDoc *doc, CDataList *b, 
                          double *xmin, double *ymin, double *xmax, double *ymax);

double get_pinpitch(BlockStruct *b, double accuracy, CCEtoODBDoc *doc);

void check_value_attributes(CCEtoODBDoc *doc);

void init_realpart_info(CCEtoODBDoc *doc);

// rounds x to accuracy
double round_accuracy(double x, double accuracy);

int scale_default_units(SettingsStruct *Settings, double unitfactor);

int GetMaxStackupNum(CCEtoODBDoc *doc);

int Complex_2Circles1Rectangle(CDataList *DataList, double accuracy,
   double *sizeA, double *sizeB, double *orient, double *xoffset, double *yoffset);
int Complex_1Rectangle(CCEtoODBDoc *doc, CDataList *DataList, double accuracy,
   double *sizeA, double *sizeB, double *orient, double *xoffset, double *yoffset);
int Complex_1Circle(CCEtoODBDoc *doc, CDataList *dataList, double accuracy, 
  double *sizeA, double *xoffset, double *yoffset);
int Complex_2PointLine(CCEtoODBDoc *doc, CDataList *DataList, double accuracy, 
   double *sizeA, double *sizeB, double *orient, double *xoffset, double *yoffset, int *shape);
int Exchange_SubBlock(CCEtoODBDoc *doc, int subblocknum, BlockStruct *copyblock);
double OptimizeRotation(double rad);
bool IsEqualAperture(CCEtoODBDoc *doc, BlockStruct* block1, BlockStruct* block2, double accuracy);
void ExchangeAperture(CCEtoODBDoc *doc, int keepBlockNum, int killBlockNum, bool destroyKillBlock = true);
bool BlocksMatch(CCEtoODBDoc *doc, BlockStruct *b1, BlockStruct *b2, double accuracy);
bool PolysMatch(CCEtoODBDoc *doc, CPoly *poly1, CPoly *poly2, double accuracy);

int GetDataLink(CCamCadData &camCadData, DataStruct *data);
bool IsProbedDiePin(CCamCadData &camCadData, int probedatalink);
bool IsProbedDiePin(CCamCadData &camCadData, DataStruct *data);
BlockStruct *GetBondPadPadstackBlock(CCamCadData &camCadData, BlockStruct *block);

/****************************************************************************/
/*
   err = 1 Primary Boardoutline found, but not closed!

   err = 2 No Primary Boardoutline found!

   err = 3 One Primary closed Boardoutline and multiple open elements found!
           The open elements will be set to BOARD GEOMETRIES!

   err = 4 Multiple Primary closed Boardoutline and multiple open elements found!
           All elements will be set to BOARD GEOMETRIES!
*/
int check_primary_boardoutline(FileStruct *file);

/****************************************************************************/
/*
   Use this instead of strtok !!!

   delimeter is always a space
   " xxx " is done
   " xx\" " is ok
*/
char *get_string(char *tok, const char *del, BOOL BackslashIsEscape = TRUE);


/******************************************************************************
* get_string()
*  - Use this instead of strtok !!!
*  - Use this when the backslash character is an escape character only for 
*        specific characters
*  - Escape character is assumed to be a backslash
*
*  delimeter is always a space
*  " xxx " is done
*  " xx\" " is ok as long as " is in the list of charsToEnforceEscape
*/
char *get_string(char *line, const char *delimeter, char *charsToEnforceEscape);


/////////////////////////////////////////////////////////////////////////////
// PCB_HighlightNet dialog

class PCB_HighlightNet : public CDialog
{
// Construction
public:
   PCB_HighlightNet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PCB_HighlightNet)
   enum { IDD = IDD_PCB_HIGHLIHGT_NET };
   CComboBox   m_nameCB;
   BOOL  m_color;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   int index;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PCB_HighlightNet)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PCB_HighlightNet)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnApply();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// end PCBUTIL.H/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// DeleteTraces dialog

class DeleteTraces : public CDialog
{
// Construction
public:
   DeleteTraces(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(DeleteTraces)
   enum { IDD = IDD_DELETE_TRACE };
   int      m_traces;
   int      m_vias;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DeleteTraces)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(DeleteTraces)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// GenericDelete dialog

class GenericDelete : public CDialog
{
// Construction
public:
   GenericDelete(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GenericDelete)
   enum { IDD = IDD_GENERIC_DELETE };
   BOOL  m_component;
   BOOL  m_graphic;
   //}}AFX_DATA
   

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GenericDelete)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(GenericDelete)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// GeneratePadstackAccess dialog

class GeneratePadstackAccess : public CDialog
{
// Construction
public:
   GeneratePadstackAccess(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GeneratePadstackAccess)
   enum { IDD = IDD_GENERATE_PADSTACK_ACCESS };
   BOOL  m_smd;
   BOOL  m_soldermask;
   BOOL  m_create_soldermask;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GeneratePadstackAccess)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(GeneratePadstackAccess)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// GeneratePin1Marker dialog

class GeneratePin1Marker : public CDialog
{
// Construction
public:
   GeneratePin1Marker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(GeneratePin1Marker)
   enum { IDD = IDD_GENERATE_PIN1MARKER };
// int   m_square;
   int   m_circle;
   CString  m_size;
   //}}AFX_DATA
   CCEtoODBDoc *doc;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GeneratePin1Marker)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PCB_HighlightNet)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// end PCBUTIL.H/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#endif
