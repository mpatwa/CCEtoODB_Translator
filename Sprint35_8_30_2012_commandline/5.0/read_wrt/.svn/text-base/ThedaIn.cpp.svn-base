
/****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.


   Padstack rotation ??? rotates the padstack only but not
   the the xy koos
*/

/* 20060302.1540 - knv
   Special processing for component and solder side placed comp instances.
1) Component side rectified geometry names should have _<PlacementSpecificLayer> removed.
2) Solder side instances should use component side rectified geometries inserted on bottom with layers and graphics mirrored.
3) Placement Specific layer names for component and solder side layers should use _CS or _SS instead of _<PlacementSpecificLayer> -
   these _CS layers should be mirrored with their _SS counterpart layer.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "ThedaIn.h"
#include "RwLib.h"
#include "TypedContainer.h"
#include "Xform.h"
#include "WriteFormat.h"
#include "TMState.h"
#include "CamCadDatabase.h"
#include "CCEtoODB.h"
#include "CrcCalculator.h"
#include "RwUiLib.h"

#define QComponentSide                    "COMPONENT_SIDE"
#define QSolderSide                       "SOLDER_SIDE"
#define Q_PS                              "_PS"
#define Q_OPS                             "_OPS"
#define QComp                             "COMP"
#define QPcbComponentTemplatePrefix       "$PkgTemplate_"
#define QPcbComponentPrefix               "$Pkg_"
#define QPadStackPrefix                   "$Pst_"
#define QBondLandPrefix                   "$BondLand_"
#define QWirePrefix                       "$Wire_"
#define QPadStackTemplatePrefix           "$PstTemplate_"
#define QShapePrefix                      "$Shp_"
#define QComponentSideNormalizedLayerName "CS"
#define QSolderSideNormalizedLayerName    "SS"
#define QTopBuiltGeometrySuffix           "$top$"
#define QBottomBuiltGeometrySuffix        "$bot$"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define LineStatusUpdateInterval 200
//#define EnableCommandStackTracing

#ifdef EnableCommandStackTracing
static CCommandListMap* s_commandListMap = NULL;
#endif

#ifdef EnableCommandStackTracing
static CCommandListStack s_commandListStack;
#endif

extern CProgressDlg *progress;
//extern LayerTypeInfoStruct layertypes[];

/* Static Variables *********************************************************/

static CString             cur_graphic_place;
static CString             cur_filename;
static CString             cur_netname;
static TLCircle            cur_circle;
static TLArc               cur_arc;
static TLComp              cur_comp;
static TLPin               cur_pin;
static TLProp              cur_prop;
static TLVia               cur_via;
static TLRefPad            cur_refPad;
static TLBoard             cur_board;
static TLComppin           cur_comppin;
static TLProbe             cur_probe;
static AText               cur_atext;
static CString             cur_layer;
static CString             cur_sym_type;              // cur_sym_type is the original type
//static CString             cur_layer_materials;
//static CString             padStackBlockName;
static CString             cur_blockname;
static CString             cur_boardname;
static CString             cur_line_pen;
static int                 cur_layerindex;
static int                 cur_symbol_type;
static int                 cur_widthindex;
static int                 cur_tracewidthindex;
static int                 cur_defaulttracewidthindex;
static int                 cur_closed;
static int                 cur_filled;
static int                 cur_x_mirror;
static double              cur_offset_x;
static double              cur_offset_y;
static double              cur_rotation;
static double              PhysPinX, PhysPinY;
static char                cur_technique, design_technique;
static int                 cur_filled_area_set;
static CString             cur_package_type;
static int                 cur_number_of_pins;
static int                 cur_ignore_shape_id;
static bool                netTableDetected = false;
static CString             orig_tl_line;              // needed to detect LONGFORM vs SHORTFORM
static CCEtoODBDoc          *doc;
static FileStruct          *file = NULL;
static int                 display_error = 0;
static FILE                *ifp;                      // File pointers
static int                 cur_filenum = 0;
static int                 Push_tok = FALSE;
static char                cur_line[MAX_LINE];
static int                 cur_new = TRUE;
static char                token[MAX_TOKEN];          // Current token.
static int                 pageUnits;
static double              faktor;
static double              one_mil;
static double              CIRCLE_2_APERTURE = 1000;
static int                 widthindex0;
static int                 refNameKeyword;
static int                 cur_section;
static int                 cur_find_any_board;        // if a panel can not find the named PCB design
static int                 cur_filestatus;
static int                 cur_ate_probe;
static int                 cur_diagnostic_probe;
static int                 cur_bareboard_probe;
static int                 test_point_cnt;
static int                 smooth_polygon_error;
static int                 comp_prop;
static FILE                *logFp = NULL;

//CPinNameToContactLayerMap  s_pinNameToContactLayerMap;

static CFileReadProgress*  fileReadProgress = NULL;
static CPoly               *curpoly;
static TypeStruct          *curtype;
static LayerStruct         *curlayer;
static BlockStruct         *curblock;
static TLVersion           tlversion;

static CStringList s_layerStructureLayerNames;

static CCamCadData& getCamCadData()
{
   return doc->getCamCadData();
}

//___________________________________________________________________________________________________
static DataStruct* s_curdata = NULL;

static DataStruct* getCurData()
{
   return s_curdata;
}

static void setCurData(DataStruct* curdata)
{
   s_curdata = curdata;
}

//___________________________________________________________________________________________________
static CAttributes* s_testProbeAttributes = NULL;

static CAttributes& getTestProbeAttributes()
{
   if (s_testProbeAttributes == NULL)
   {
      s_testProbeAttributes = new CAttributes();
   }

   return *s_testProbeAttributes;
}

static void releaseTestProbeAttributes()
{
   delete s_testProbeAttributes;

   s_testProbeAttributes = NULL;
}

//___________________________________________________________________________________________________
static CDrillArray         drillarray;
static int                 drillcnt;

static CPadformArray       padformarray;
static int                 padformcnt;

static TLAdef              layer_attr[MAX_LAYERS];    // Array of layers from pdif.in
static int                 layer_attr_cnt;

static RenameLayer         layer_rename[MAX_LAYERS];  // Array of layers from pdif.in
static int                 layer_rename_cnt;

static MaterialLayerType   material_layer_type[MAX_LAYERS];
static int                 material_layer_type_cnt;

static CString             smdtype[MAX_SMDTYPE];
static int                 smdtypecnt;

static CPolyArray          polyarray;
static int                 polycnt;

static CDeviceCompArray    devicecomparray;           // this is the device - mapping
static int                 devicecompcnt;

static CCompPropArray      compproparray;             // this is the device - mapping
static int                 comppropcnt;

static CPinNameArray       pinnamearray;              // this is the device - mapping
static int                 pinnamecnt;

static CShapeCircleArray   shapecirclearray;          // needed to find drill tools
static int                 shapecirclecnt;

static CNetlistCompArray   netlistcomparray;          // this is the device - mapping
static int                 netlistcompcnt;

static CTextpenArray       textpenarray;              // last one is used for temp text definition
static int                 textpencnt;

static CAttribmapArray     attribmaparray;
static int                 attribmapcnt = 0;

static CGeomArray          geomarray;
static int                 geomcnt = 0;

static TLBoardoutline      boardoutline[30];
static int                 boardoutlinecnt;

static CString             compoutline[30];           // allow upto 30 different layers for component outline
static int                 compoutlinecnt;

static CIgnoreArray        ignorearray;
static int                 ignorecnt = 0;

// Variables from load setting
static int                 ComponentSMDrule;          // 0 = take existing ATT_SMD
                                                      // 1 = if all pins of comp are marked as SMD
                                                      // 2 = if most pins of comp are marked as SMD
                                                      // 3 = if one pin of comp are marked as SMD
static int                 convert_complex_power;
static int                 use_pin_ids_physical;
static int                 IGNORE_LAYER_SYMBOLTYPE;
static int                 IGNORE_LAYER_MATERIALS;
static BOOL                BAREBOARD_PROBES;
static BOOL                WIREBONDPEN;
static bool                updateRefdesFlag=true;
static double              probeSize;
static int                 probeSizeUnit;
static double              drillSize;
static int                 drillSizeUnit;

CStdioFileWriteFormat* s_debugWriteFormat = NULL;
CThedaTableParser* tableParser = NULL;

CThedaReader* s_thedaReader = NULL;

CThedaReader& getThedaReader()
{
   if (s_thedaReader == NULL)
   {
      s_thedaReader = new CThedaReader(*doc);
   }

   return *s_thedaReader;
}

/* Function Prototypes *********************************************************/

void initCommandListMap();
static void DoThedaRead();
static void get_newline();
static double cnv_unit(char *);
static int go_command(List *,int, const CString&);
static void init_all_mem();
static void free_all_mem();
static bool get_next(char *,int);
static int get_line(char *,int);
static bool get_tok();
static int parsingError();
static int tok_search(List *,int);
static int push_tok();
static int loop_command(List *,int, const char *);
static int load_thedasettings(CString fname);
static int clear_graphicsettings();
static int assign_layers();
static GraphicClassTag get_layerclass(const char *layername);
static int already_comppin(FileStruct *fl, const char *net_name, const char *comp, const char *pin);
static int ignore(const char *s, int typ, int bareboard, int diagnostic, int ate);
static int do_shape_id_inserttype();
static int clean_padstackprefix();
static int clean_padprefix();
static int do_vertex(int close);
static void check_duplicated_blocknames();
static int do_layerstructure();
void FlattenBlock(CCEtoODBDoc *doc, BlockStruct *block, FileStruct *file, int flattenLayer, CDataList *datalist, BOOL TopLevelOnly,
      int graphicClass, DTransform *xform, int insertLayer, CString refdes, CString netname, CMapWordToPtr *attribMap);
static TLTextpen *FindFirstTextPen();

inline bool tokenIs(const char* string) { return (STRCMPI(token,string) == 0); }
static int CalculateArcByRadius(TLPoly *p1, TLPoly *p, TLPoly *p3, CPnt *pnt1, CPnt *pnt2);
static int GetXYofPointOnSegment(double x1, double y1, double x2, double y2, double distanceFromPoint2, double &newX, double &newY);

static void addLayerStructureLayerName(const CString& layerName);
static void processLayerStructureLayers();

//_____________________________________________________________________________
static int s_inputFileLineNumber = 0;

static int getInputFileLineNumber()
{
   return s_inputFileLineNumber;
}

static void resetInputFileLineNumber()
{
   s_inputFileLineNumber = 0;
}

static void incrementInputFileLineNumber()
{
   ++s_inputFileLineNumber;

#ifdef EnableCommandStackTracing
   if (s_inputFileLineNumber == 106851)
   {
      int iii = 3;
   }
#endif
}


/******************************************************************************
* ReadTHEDA
*/
void ReadTHEDA(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   s_layerStructureLayerNames.RemoveAll();

   file = NULL;
   doc = Doc;
   pageUnits = PageUnits;
   refNameKeyword = doc->IsKeyWord(ATT_REFNAME, 0);

#ifdef EnableCommandStackTracing
   s_commandListMap = new CCommandListMap();
   initCommandListMap();
#endif

   init_all_mem();

   // need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath( path_buffer, drive, dir, fname, ext );
   cur_filename = fname;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer, "r")) == NULL)
   {
      CString t;
      t.Format("Error open [%s] file", path_buffer);
      ErrorMessage(t, "Error");
   }

   CString thedaLogFile;
   logFp = getApp().OpenOperationLogFile("theda.log", thedaLogFile);
   if (logFp == NULL) // error message already issued, just return.
      return;

   display_error = 0;

   CString settingsFile = getApp().getImportSettingsFilePath("theda.in");
   {
      CString msg;
      msg.Format("\nTHEDA: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_thedasettings(settingsFile);

   /* initialize scanning parameters */
   rewind(ifp);
   cur_comp.refdes.Empty();            // must be reset here !!!
   cur_comp.setNetlistComp(NULL);

   resetInputFileLineNumber();      /* Current line number. */
   get_newline();                   // reset all stuff

   delete fileReadProgress;
   fileReadProgress = new CFileReadProgress(ifp);

   DoThedaRead();

   //getThedaReader().instantiateRectifiedComponentGeometries();
   //getThedaReader().instantiateRectifiedPadStackGeometries();

   free_all_mem();
   fclose(ifp);

   ProcessLoadedProbes(doc);

   // there are layes created, which are not needed !
   if (getThedaReader().getOptionPurgeUnusedLayers())
   {
      doc->RemoveUnusedLayers();
   }

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!thedaLogFile.IsEmpty())
      fclose(logFp);

   if (display_error && !thedaLogFile.IsEmpty())
   {
      Logreader(thedaLogFile);
   }

#ifdef EnableCommandStackTracing
   delete s_commandListMap;
#endif

   delete s_debugWriteFormat;
   s_debugWriteFormat = NULL;

   delete s_thedaReader;
   s_thedaReader = NULL;

   delete fileReadProgress;
   fileReadProgress = NULL;

   releaseTestProbeAttributes();
   s_layerStructureLayerNames.RemoveAll();

   cur_comp.releaseData();
}

double normalizeToPositiveDegrees(double& angle)
{
   angle = fmod(angle,360.);

   if (angle < 0) angle += 360.;

   return angle;
}

/******************************************************************************
* getDebugWriteFormat
*/
CWriteFormat* getDebugWriteFormat()
{
   if (s_debugWriteFormat == NULL)
   {
      CFilePath filePath(getActiveView()->GetDocument()->GetProjectPath(getApp().getUserPath()));
      filePath.pushLeaf("ThedaInDebug.txt");

      s_debugWriteFormat = new CStdioFileWriteFormat(filePath.getPath(),512);
      s_debugWriteFormat->setNewLineMode(true);
   }

   return s_debugWriteFormat;
}

/******************************************************************************
* flushDebugWriteFormat
*/
void flushDebugWriteFormat()
{
   if (s_debugWriteFormat != NULL)
   {
      s_debugWriteFormat->flush();
   }
}

/******************************************************************************
* DoThedaRead
*/
static void DoThedaRead()
{
   if (!get_tok())
   {
      ErrorMessage("THEDA read error", "Error: file corrupt");
      return;
   }

   CString tok = token;

   if (!tok.CollateNoCase("PC_BOARD"))
   {
      cur_filestatus = FILESTATUS_BOARD;
      get_newline();

      if (!get_line(cur_line, MAX_LINE))
      {
         ErrorMessage("THEDA read error", "Error");
         return;
      }

      // here test for long form
      orig_tl_line.TrimLeft();
      orig_tl_line.TrimRight();

      if (orig_tl_line.Left(1) != "!")
      {
         ErrorMessage("This TL file is in SHORTFORM!\n\n Please create THEDA TL file in LONGFORM format!\n\nFile will not be imported!", "TL File Format Error");
         return;
      }

      if (loop_command(theda_lst, SIZ_THEDA_LST, tok) < 0)
      {
         ErrorMessage("THEDA read error", "Error");
      }

      //doc->SaveDataFileAs("\\Cases\\2037\\ThedaIn.cc");

      getThedaReader().restructurePadStacks();

      if (getThedaReader().getOptionRectifyComponents())
      {
         getThedaReader().rectifyComponents(netlistcomparray);
      }

      if (getThedaReader().getOptionRectifyViasAndTestPoints())
      {
         getThedaReader().rectifyViasAndTestPoints();
      }

      int assemblyDrawingComponentSideLayerIndex = getThedaReader().getDefinedLayerIndex("ASSEMBLY_DRAWING_CS");
      int assemblyDrawingSolderSideLayerIndex    = getThedaReader().getDefinedLayerIndex("ASSEMBLY_DRAWING_SS");

      assign_layers();

      rewind(ifp);
      resetInputFileLineNumber();      /* Current line number. */

      processLayerStructureLayers();

      update_smdpads(doc);
      update_smdrule_geometries(doc, ComponentSMDrule);
      update_smdrule_components(doc, file, ComponentSMDrule);

      CThedaPartialVias::getThedaPartialVias(*doc,cur_filenum).generatePartialVias();
      CThedaPartialVias::releasePartialVias();

      //clean_padprefix();

      //clean_padstackprefix();

      if (getThedaReader().getOptionFixupGeometryNames())
      {
         getThedaReader().fixupGeometryNames();
      }

      //This is commented out because the padstack is created incorrectly when this function is called
      //OptimizePadstacks(doc, pageUnits, convert_complex_power); // this can create unused blocks
      generate_PADSTACKACCESSFLAG(doc, TRUE);

      double minref = 0.05 * Units_Factor(UNIT_INCHES, pageUnits);
      double maxref = 0.05 * Units_Factor(UNIT_INCHES, pageUnits);

      getThedaReader().releaseRestructuredPadStacksBlock();

      if (updateRefdesFlag)
      {
         update_manufacturingrefdes(doc, file,*(file->getBlock()), minref, maxref, assemblyDrawingComponentSideLayerIndex, FALSE);
      }

      if (getThedaReader().getOptionPurgeUnusedWidthsAndGeometries())
      {
         doc->purgeUnusedWidthsAndBlocks(false);
      }

      if (getThedaReader().getOptionPurgeUnusedLayers())
      {
         doc->RemoveUnusedLayers(true);
      }
   }
   else if (!tok.CompareNoCase("PANEL"))
   {
      cur_filestatus = FILESTATUS_PANEL;
      get_newline();

      if (!get_line(cur_line, MAX_LINE))
      {
         ErrorMessage("THEDA read error", "Error");
         return;
      }

      // here test for long form
      orig_tl_line.TrimLeft();
      orig_tl_line.TrimRight();

      if (orig_tl_line.Left(1) != "!")
      {
         ErrorMessage("This TL file is in SHORTFORM!\n\n Please create THEDA TL file in LONGFORM format!\n\nFile will not be imported!", "TL File Format Error");
      }
      else if (loop_command(panel_lst, SIZ_PANEL_LST, tok) < 0)
      {
         ErrorMessage("THEDA read error", "Error");
      }

      assign_layers();
      check_duplicated_blocknames();

      if (getThedaReader().getOptionFixupGeometryNames())
      {
         getThedaReader().fixupGeometryNames();
      }

      // switch off the display for boards.
      for (POSITION filePos = doc->getFileList().GetHeadPosition(); filePos != NULL;)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (file->getBlockType()  == BLOCKTYPE_PCB)
         {
            file->setShow(false);
         }
      }

      if (getThedaReader().getOptionPurgeUnusedWidthsAndGeometries())
      {
         doc->purgeUnusedWidthsAndBlocks(false);
      }

      if (getThedaReader().getOptionPurgeUnusedLayers())
      {
         doc->RemoveUnusedLayers(true);
      }
   }
   else
   {
      ErrorMessage("Not a TL file", "Error: [PC_BOARD or PANEL] expected");
   }
}

/******************************************************************************
* get_attribmap
*/
static const char *get_attribmap(const char *c)
{
   static CString tmp;

   for (int i=0; i<attribmapcnt; i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   tmp = c;
   tmp.MakeUpper();

   return tmp.GetBuffer(0);
}

/******************************************************************************
* make_layerstring
*/
static int make_layerstring()
{
   if (strlen(cur_layer) == 0)
   {
      return getThedaReader().getFloatingLayerIndex();
   }

   CString l = cur_layer;

   // do not make every layer a symtype. Some symtypes are classes,
   // the strlen(cur_sym_type_string) is used to see if a sym_type was known.
   if (!IGNORE_LAYER_SYMBOLTYPE && strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      l += ":";
      l += getThedaReader().getCurrentSymbolTypeString();

      if (STRCMPI(getThedaReader().getCurrentSymbolTypeString(), "WIRE_BOND") == 0 && WIREBONDPEN)
      {
         l += "_";
         l += cur_line_pen;
      }
   }

   if (!IGNORE_LAYER_MATERIALS)
   {
      CString material = getThedaReader().getMaterial();

      if (! material.IsEmpty())
      {
         l += ":";
         l += material;
      }
   }

   if (strlen(cur_graphic_place))
   {
      if (!STRNICMP(cur_layer, "ALL", 3))
      {
         // do not allow COMP|SOLDER side restrictions for all layers.
      }
      else if (!STRNICMP(cur_layer, "INNER", 5))
      {
         // do not allow COMP|SOLDER side restrictions for inner layers.
      }
      else
      {
         l += "|";
         l += cur_graphic_place;
      }
   }

   return getThedaReader().getDefinedLayerIndex(l);
}

/******************************************************************************
* get_padformindex
*/
static int get_padformindex(int f,double sizeA, double sizeB, double rotation, double offset)
{
   TLPadform p;
   double offsetx = 0;
   double offsety = 0;

   for (int i=0; i<padformcnt; i++)
   {
      p = padformarray.GetAt(i);

      if (p.form == f &&
          p.sizeA == sizeA &&
          p.sizeB == sizeB &&
          p.rotation == rotation &&
          p.offsetx == offsetx &&
          p.offsety == offsety)
      {
         return i;
      }
   }

   int err;
   CString name;
   name.Format("PADSHAPE_%d", padformcnt);
   Graph_Aperture(name, f, sizeA, sizeB, 0.0, 0.0, DegToRad(rotation), 0, BL_APERTURE, TRUE, &err);

   p.form = f;
   p.sizeA = sizeA;
   p.sizeB = sizeB;
   p.rotation = rotation;
   p.offsetx = 0;
   p.offsety = 0;
   padformarray.SetAtGrow(padformcnt++, p);

   return padformcnt -1;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size, int layernum)
{
   if (size == 0)
   {
      return -1;
   }

   TLDrill p;

   for (int i=0; i<drillcnt; i++)
   {
      p = drillarray.ElementAt(i);

      if (p.d == size)
      {
         return i;
      }
   }

   CString name;
   name.Format("DRILL_%d", drillcnt);
   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   p.d = size;
   drillarray.SetAtGrow(drillcnt++, p);

   return drillcnt -1;
}

/******************************************************************************
* assign_layers
*/
static int assign_layers()
{
   LayerStruct *l;

   // here do mirror layers
   for (int i=0; i<layer_attr_cnt; i++)
   {
      CString layerName = layer_attr[i].name;

      if ((l = doc->getLayerNoCase(layerName)) == NULL)
      {
         continue;
      }

      if (layer_attr[i].mirror < 0)
      {
         continue;
      }

      LayerStruct& layer = getThedaReader().getDefinedLayer(layerName);
      LayerStruct& mirrorLayer = getThedaReader().getDefinedLayer(layer_attr[layer_attr[i].mirror].name);
      Graph_Level_Mirror(layer.getName(),mirrorLayer.getName(), "");
   }

   // now attributes after mirror layers
	int j=0;
   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      l = doc->getLayerArray()[j];

      if (l == NULL)
         continue; // could have been deleted.

      for (int i=0; i<layer_attr_cnt; i++)
      {
         if (l->getName().CompareNoCase(layer_attr[i].name) == 0)
         {
            l->setLayerType(layer_attr[i].attr);
            break;
         }
      }

      if (l->getName().Right(10) == "|COMP_SIDE")
      {
         l->setFlagBits(LY_NEVERMIRROR);
      }
      else if (l->getName().Right(10) == "|SOLD_SIDE")
      {
         l->setFlagBits(LY_MIRRORONLY);
      }

      if (!STRCMPI(l->getName(),"DRILLHOLE"))
         l->setLayerType(LAYTYPE_DRILL);
   }

   int startstacknum = -1;
   int endstacknum = -1;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not the next index.
   {
      l = doc->getLayerArray()[j];

      if (l == NULL)
      {
         continue;
      }

      if (!l->isVisible())
      {
         continue;
      }

      if(l->getElectricalStackNumber())
      {
         if (startstacknum < 0)
            startstacknum = l->getElectricalStackNumber();

         if (endstacknum < 0)
            endstacknum = l->getElectricalStackNumber();

         if (startstacknum > l->getElectricalStackNumber())
            startstacknum = l->getElectricalStackNumber();

         if (endstacknum < l->getElectricalStackNumber())
            endstacknum = l->getElectricalStackNumber();
      }
   }

   if (startstacknum < 0 || endstacknum < 0)
      return -1;

   // This is to assign electrical stack number to all signal layers
   CString SignalTop, SignalBot, PadTop, PadBot;

   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      l = doc->getLayerArray()[j];

      if (l == NULL)
         continue;

      if (!l->isVisible())
         continue;

      if (l->getLayerType() == LAYTYPE_SIGNAL_TOP && l->getName().Find("SIGNALS") != -1)
      {
         SignalTop = l->getName();
      }
      else if (l->getLayerType() == LAYTYPE_SIGNAL_BOT && l->getName().Find("SIGNALS") != -1)
      {
         SignalBot = l->getName();
      }
      else if (l->getLayerType() == LAYTYPE_PAD_TOP && l->getName().Find(":") == -1 && l->getName().Find("|") == -1)
      {
         PadTop = l->getName();
      }
      else if (l->getLayerType() == LAYTYPE_PAD_BOTTOM && l->getName().Find(":") == -1 && l->getName().Find("|") == -1)
      {
         PadBot = l->getName();
      }
   }

   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      l = doc->getLayerArray()[j];

      if (l == NULL)
         continue;

      if (!l->isVisible())
         continue;

      if (SignalTop != "" && !l->getElectricalStackNumber() && !STRNICMP(l->getName(), SignalTop, strlen(SignalTop)))
      {
         l->setElectricalStackNumber(startstacknum);
      }
      else if (SignalBot != "" && !l->getElectricalStackNumber() && !STRNICMP(l->getName(), SignalBot, strlen(SignalBot)))
      {
         l->setElectricalStackNumber(endstacknum);
      }
      else if (PadTop != "" && !l->getElectricalStackNumber() && !STRNICMP(l->getName(), PadTop, strlen(PadTop)))
      {
         l->setElectricalStackNumber(startstacknum);
      }
      else if (PadBot != "" && !l->getElectricalStackNumber() && !STRNICMP(l->getName(), PadBot, strlen(PadBot)))
      {
         l->setElectricalStackNumber(endstacknum);
      }

      for (int i=0; i<material_layer_type_cnt; i++)
      {
         if (!l->getName().CompareNoCase(material_layer_type[i].layername))
         {
            l->setLayerType(material_layer_type[i].layertype);
         }
      }
   }

   return 1;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//      {
//         return i;
//      }
//   }
//
//   return LAYTYPE_UNKNOWN;
//}

/******************************************************************************
* geom_already_exist
*/
static int geom_already_exist(const char *b)
{
   for (int i=0; i<geomcnt; i++)
   {
      if (geomarray[i]->name.Compare(b) == 0)
      {
         return TRUE;
      }
   }

   return FALSE;
}

/******************************************************************************
* check_duplicated_blocknames
   Theda can have the same block names in the PANEL and in the BOARD.
   this is seperated by filenumber, but is will cause probles later.
*/
static void check_duplicated_blocknames()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;   // width can have no name

      if (geom_already_exist(block->getName()))
      {
         CString tmp;

         if (strlen(block->getName()))
         {
            tmp.Format("%s_%d", block->getName(), block->getFileNumber());
         }
         else
         {
            tmp.Format("BLOCK_%d_%d", block->getBlockNumber(), block->getFileNumber());
         }

         fprintf(logFp, "Duplicated Blockname [%s] [%d] found -> renamed to [%s].\n", block->getName(), block->getBlockNumber(), tmp);
         display_error++;
         block->setName(tmp);
      }

      TLGeom *geom = new TLGeom;
      geomarray.SetAtGrow(geomcnt++, geom);
      geom->name = block->getName();
   }
}

/****************************************************************************/
/*
   Call function associated with next tolen.
   Tokens start at a line until := are searched for on the local
   token list.
*/
int go_command(List *tok_lst,int lst_size, const CString& startt)
{
   int  i, res;
   CString ws;   // just debug

   strcpy(token, startt);

#ifdef EnableCommandStackTracing
   static bool traceFlag = false;

   if (traceFlag)
   {
      CString trace = s_commandListStack.trace();
      int currentLineNumber = getInputFileLineNumber();
   }
#endif

   if ((i = tok_search(tok_lst,lst_size)) >= 0)
   {
      res = (*tok_lst[i].function)();
   }
   else
   {
      fnull(startt, TRUE); // unknown command, log and skip
      return 0;
   }

   return res;
}

/******************************************************************************
* loop_command
*/
int loop_command(List *list,int size, const char *startt)
{
#ifdef EnableCommandStackTracing
   s_commandListStack.push(getInputFileLineNumber(),startt,list);
   static int hitCount = 0;

   if (getInputFileLineNumber() >= 8120 && hitCount == 0)
   {
      CString debugString = s_commandListStack.trace();
      hitCount++;
   }
#endif

   getThedaReader().pushSection(startt);

   int retval = -1;
   CString fint;
   CString ws = startt;
   fint = "..FIN_";
   fint += startt;

   while (true)
   {
      // start with the first token.
      // loop until the token is not the FIN_ of the start token.
      if (!get_tok())
      {
         retval = parsingError();
         break;
      }

      // if token == #---------- <=== this is a remark line
      // here I have to go to the end of a section, because I do not know anymore
      // when this #---
      //            stuff
      //           #----
      // section ends.

      if (tokenIs("#------"))
      {
#ifdef EnableCommandStackTracing
         CString trace = s_commandListStack.trace();
         OutputDebugString(trace);
#endif

         int functionIndex = tok_search(list,size);

         if (functionIndex >= 0)
         {
            int res = (*list[functionIndex].function)();
         }

         if (tableParser == NULL)
         {
            tableParser = new CThedaTableParser();
         }

         retval = (tableParser->parse() ? 1 : -1);

         delete tableParser;
         tableParser = NULL;
      }
      else
      {
         ws = strtok(token," \t\n[];");   // there are KEEPINS[ x ] this all must be cleaned

         if (!STRNICMP(ws, fint, strlen(fint)))
         {
            retval = 1;
            break;
         }
         else if ((go_command(list, size, ws)) < 0)
         {
            retval = -1;
            break;
         }
      }
   }

#ifdef EnableCommandStackTracing
   s_commandListStack.pop();
#endif

   getThedaReader().popSection();

   return retval;
}

/******************************************************************************
* get_newline
*/
static void get_newline()
{
   cur_line[0] = '\0';  // get a new line;
   cur_new = TRUE;
   Push_tok = FALSE;

   return;
}

/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next matching FIN_ line.
*/
static int fnull(const char *startt, int log)
{
   CString nl = cur_line;

   // if a start line ends in a ; that's a oneline command and does not end in FIN_???
   nl.TrimRight();
   CString r = nl.Right(1);

   if (nl.Left(6) != "..FIN_" && (r == ";"||r==','))
   {
      // one line command
      if(log)
      {
#ifdef _DEBUG
         fprintf(logFp, "DEBUG: One Line Token [%s] at %ld unknown -> skipped.\n", token, getInputFileLineNumber());
         display_error++;
#endif
      }

      get_newline();
      return 0;
   }

   // these tokens end in FIN_ section
   CString t = "..FIN_";
   t += startt;

   if (log)
   {
#ifdef _DEBUG
      fprintf(logFp, "DEBUG: Token [%s] at %ld unknown -> ",token,getInputFileLineNumber());
      display_error++;
#endif
   }

   while (TRUE)
   {
      if (get_tok())
      {
         char  ws[MAX_LINE];  // just for debug
         strcpy(ws,token);
         char *lp = strtok(ws,";\n");

         if (lp == NULL)
         {
            continue;
         }

         if (!STRICMP(ws,t))  // exact
         {
            if (log)
            {
#ifdef _DEBUG
               fprintf(logFp, "skipped to [%s] at %ld\n",t,getInputFileLineNumber());
#endif
            }

            get_newline();
            return 0;
         }
      }
      else
      {
         return parsingError();
      }

      get_newline();
   }
   return 0;
}

/******************************************************************************
* start_theda
*/
static int start_theda()
{
   if (!get_tok())
      return parsingError();

   CString ws = token;

   return loop_command(theda_lst, SIZ_THEDA_LST, "PC_BOARD");
}

/******************************************************************************
* start_panel
*/
static int start_panel()
{
   if (!get_tok())
      return parsingError();

   CString ws = token;

   return loop_command(panel_lst, SIZ_PANEL_LST, "PANEL");
}

/******************************************************************************
* theda_identifier
*/
static int theda_identifier()
{
   if (!get_tok())
      return parsingError();

   CString ident = token;

   if (!strlen(ident))  // if no filename given
      ident = cur_filename;

   file = Graph_File_Start(ident, Type_THEDA_Layout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
   cur_filenum = file->getFileNumber();

   getThedaReader().setPcbFile(file);

   getThedaReader().getFloatingLayerIndex();  // set the first layer 0 to floating;

   int err;
   widthindex0 = Graph_Aperture("", T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   get_newline();

   return 1;
}

/******************************************************************************
* panel_identifier
*/
static int panel_identifier()
{
   if (!get_tok())
      return parsingError();

   CString ident = token;

   if (!strlen(ident))  // if no filename given
      ident = cur_filename;

   file = Graph_File_Start(ident, Type_THEDA_Layout);
   file->setBlockType(blockTypePanel);
   file->getBlock()->setBlockType(file->getBlockType());
   cur_filenum = file->getFileNumber();

   getThedaReader().setPcbFile(file);

   getThedaReader().getFloatingLayerIndex();  // set the first layer 0 to floating;
   get_newline();

   return 1;
}

/******************************************************************************
* theda_version
*/
static int theda_version()
{
   int res = loop_command(version_lst, SIZ_VERSION_LST, "VERSION");

   // check for TL_REVISION
   if (tlversion.tl_revision != 1 && tlversion.tl_revision != 2)
   {
      fprintf(logFp, "TL_REVISION found [%1.2lf] -> expected [2.x]\n", tlversion.tl_revision);
      display_error++;
   }

   // LIBRARY_REVISION
   if (tlversion.library_revision != 4  && tlversion.library_revision != 6  &&
       tlversion.library_revision != 7 && tlversion.library_revision != 9)
   {
      fprintf(logFp, "LIBRARY_REVISION found [%1.2lf] -> expected [4.x, 6.x, 7.x, 9.x]\n", tlversion.library_revision);
      display_error++;
   }

   // BOARD_REVISION
   if (tlversion.board_revision != 3 && tlversion.board_revision != 4 &&
       tlversion.board_revision != 5 && tlversion.board_revision != 6 && tlversion.board_revision != 8)
   {
      fprintf(logFp, "BOARD_REVISION found [%1.2lf] -> expected [3.x, 4.x, 5.x, 6.x, 8.x]\n", tlversion.board_revision);
      display_error++;
   }

   return res;
}

/******************************************************************************
* theda_description
*/
static int theda_description()
{
   if (!get_tok())
      return parsingError();

   CString ident = token;

   get_newline();

   return 1;
}

/******************************************************************************
* theda_working_area
*/
static int theda_working_area()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* tl_file_technique
*/
static int tl_file_technique()
{
   if (!get_tok())
      return parsingError();

   if (strlen(token))
   {
      design_technique = token[0];
   }
   else
   {
      design_technique = ' ';
   }

   get_newline();

   return 1;
}

/******************************************************************************
* theda_unit
*/
static int theda_unit()
{
   if (!get_tok())
      return parsingError();

   double maj = atof(token);

   if (!get_tok())
      return parsingError();

   CString ident = strtok(token,";\n");

   if (!ident.CompareNoCase("MM"))
   {
      faktor = Units_Factor(UNIT_MM, pageUnits) * maj;

      if (probeSize == 0.0)
         probeSize = 0.3;

      if (probeSizeUnit == -1)
         probeSizeUnit = UNIT_MM;

      if (drillSize == 0.0)
         drillSize = 0.15;

      if (drillSizeUnit == -1)
         drillSizeUnit = UNIT_MM;
   }
   else if (!ident.CompareNoCase("INCH"))
   {
      faktor = Units_Factor(UNIT_INCHES, pageUnits) * maj;

      if (probeSize == 0.0)
         probeSize = 0.012;

      if (probeSizeUnit == -1)
         probeSizeUnit = UNIT_MM;

      if (drillSize == 0.0)
         drillSize = 0.006;

      if (drillSizeUnit == -1)
         drillSizeUnit = UNIT_MM;
   }
   else
   {
      fprintf(logFp, "Unknown Units [%s] at %ld\n", token, getInputFileLineNumber());
      display_error++;

      return -1;
   }

   one_mil = 0.001 * Units_Factor(UNIT_INCHES, pageUnits) * maj;
   get_newline();

   return 1;
}

/******************************************************************************
* theda_properties
*/
static int theda_properties()
{
   fnull("PROPERTIES", TRUE);
   return 1;
}

/******************************************************************************
* theda_design_objects
*/
static int theda_design_objects()
{
   return loop_command(design_objects_lst, SIZ_DESIGN_OBJECTS_LST, "DESIGN_OBJECTS");
}

/******************************************************************************
* theda_machine_objects
*/
static int theda_machine_objects()
{
   fnull("MACHINE_OBJECTS", FALSE);
   return 1;
}

/******************************************************************************
* theda_set_up
*/
static int theda_set_up()
{
   fnull("SET_UP", FALSE);
   return 1;
}

/******************************************************************************
* theda_net_list
*/
static int theda_net_list()
{
   return loop_command(net_list_lst, SIZ_NET_LIST_LST, "NET_LIST");
}

/******************************************************************************
* theda_physical_layout
*/
static int theda_physical_layout()
{
   return loop_command(physical_layout_lst, SIZ_PHYSICAL_LAYOUT_LST, "PHYSICAL_LAYOUT");
}

/******************************************************************************
* theda_postprocess
*/
static int theda_postprocess()
{
   fnull("POSTPROCESS", FALSE);
   return 1;
}

/******************************************************************************
* theda_postprocess_sets
*/
static int theda_postprocess_sets()
{
   fnull("POSTPROCESS_SETS", FALSE);
   return 1;
}

/******************************************************************************
* theda_design_defaults
*/
static int theda_design_defaults()
{
   fnull("DESIGN_DEFAULTS", FALSE);
   return 1;
}

/******************************************************************************
* theda_parameters
*/
static int theda_parameters()
{
   return loop_command(parameter_lst, SIZ_PARAMETER_LST, "PARAMETERS");
}

/******************************************************************************
* panel_pc_boards
*/
static int panel_pc_boards()
{
   return loop_command(pc_boards_lst, SIZ_PC_BOARDS_LST, "PC_BOARDS");
}

/******************************************************************************
* theda_library
   this is compatible to Version 1
*/
static int theda_library()
{
   return loop_command(library_lst, SIZ_LIBRARY_LST, "LIBRARY");
}

/******************************************************************************
* version_revision
*/
static int version_revision()
{
   if (!get_tok())
      return parsingError();

   tlversion.tl_revision = atof(token);
   get_newline();

   return 1;
}

/******************************************************************************
* version_comment
*/
static int version_comment()
{
   if (!get_tok())
      return parsingError();

   tlversion.tl_comment = token;
   get_newline();

   return 1;
}

/******************************************************************************
* version_board_comment
*/
static int version_board_comment()
{
   if (!get_tok())
      return parsingError();

   tlversion.tl_board_comment = token;
   get_newline();

   return 1;
}

/******************************************************************************
* version_software_revision
*/
static int version_software_revision()
{
   double rev = -1;

   if (!get_tok())
   {
      return parsingError();
   }

   if (sscanf(token,"THEDA %lf",&rev) != 1)
   {
      fprintf(logFp, "SOFTWARE REVISION Scan error at %ld\n", getInputFileLineNumber());
      display_error++;
   }

   // only 1.1 3.0, 3.1, 3.2, 5.0, 5.2, 6.0, 6.1 tested.
   if (rev != 1.1 &&
       rev != 2.0 &&
       rev != 3.0 && rev != 3.1 && rev != 3.2 &&
       rev != 4.0 && rev != 4.1 &&
       rev != 5.0 && rev != 5.1 && rev != 5.2 &&
       rev != 6.0 && rev != 6.1)
   {
      fprintf(logFp, "TL Software Revision %1.1lf not supported!\n\
         Unsupported Revision may cause software problems.\n",rev);
      display_error++;
   }

   tlversion.software_revision = rev;
   get_newline();

   return 1;
}

/******************************************************************************
* version_software_comment
*/
static int version_software_comment()
{
   if (!get_tok())
      return parsingError();

   tlversion.software_comment = token;
   get_newline();

   return 1;
}

/******************************************************************************
* version_library_comment
*/
static int version_library_comment()
{
   if (!get_tok())
      return parsingError();

   get_newline();

   return 1;
}

/******************************************************************************
* version_library_revision
*/
static int version_library_revision()
{
   if (!get_tok())
      return parsingError();

   tlversion.library_revision = atof(token);
   get_newline();

   return 1;
}

/******************************************************************************
* version_board_revision
*/
static int version_board_revision()
{
   if (!get_tok())
      return parsingError();

   tlversion.board_revision = atof(token);
   get_newline();

   return 1;
}

/******************************************************************************
* version_creation_date
*/
static int version_creation_date()
{
   if (!get_tok())
      return parsingError();

   tlversion.creation_date = token;
   get_newline();

   return 1;
}

/******************************************************************************
* physical_layout_set_up
*/
static int physical_layout_set_up()
{
   fnull("SET_UP", FALSE);

   return 1;
}

/******************************************************************************
* physical_layout_conduction_dividers
*/
static int physical_layout_conduction_dividers()
{
   // clipped from phys_signal_conductions
   int res  = loop_command(conductions_lst,SIZ_CONDUCTIONS_LST, "CONDUCTIONS");

   return res;
}

/******************************************************************************
* assign_layer
*/
static int assign_layer(const char *lname, const char *lappend, int electcnt, int ltype)
{
   CString layername = lname;
   layername += lappend;

   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *l = doc->getLayerArray()[j];

      if (l == NULL)
         continue; // could have been deleted.

      // for checking symbol type
      if (l->getName().CompareNoCase(layername) == 0)
      {
         l->setLayerType(ltype);
         l->setElectricalStackNumber(electcnt);
      }

      // for checking layer material, material layer gets the immediate parent electrical stack number
      CString arrLayerName = "";
      CString parentLayerName = lname;
      int separatorIndex = l->getName().Find(":");

      if (separatorIndex > 0)
         arrLayerName = l->getName().Left(separatorIndex);

      if (!arrLayerName.CompareNoCase(parentLayerName))
         l->setElectricalStackNumber(electcnt);
   }
   return 1;
}

/******************************************************************************
* do_layerstructure
*/
static int do_layerstructure()
{
   int electcnt = 0;
   int stackupcnt = 0;
   int insulationcnt = 0;
   int do_stack = 0;

   get_newline();

   while (TRUE)
   {
      char w[MAX_LINE];
      char *lp;

      if (!get_tok())
      {
         return parsingError();
      }

      strcpy(w, cur_line);
      lp = strtok(w, " ;\t\n");

      if (lp && !STRNICMP(lp, "..FIN_LAYER_STRUCTURE", 11))   // x y radius
      {
         break;
      }
      else if (lp && lp[0] == '#')
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp, "PROPORTIONAL"))
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp, "UNIT"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "MAX_THICKNESS"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "CURRENT_THICKNESS"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "MAX_SIGNAL_LAYERS"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "CURRENT_SIGNAL_LAYERS"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "MAX_PWR_GND_LAYERS"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "CURRENT_PWR_GND_LAYERS"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "FIXED_SIZE"))
      {
         // do nothing V 1.1
      }
      else if (lp && !STRICMP(lp, "LAYER"))
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp, "FIXED_SIZE"))
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp, "PARTIAL_VIAS")) // x y radius
      {
         fnull("PARTIAL_VIAS", FALSE);
      }
      else if (lp && !STRICMP(lp, "SINGLE_TRANSMISSION_LINES"))   // x y radius
      {
         fnull("SINGLE_TRANSMISSION_LINES", FALSE);
      }
      else
      {
         CString lname = lp;

         addLayerStructureLayerName(lname);

         //if (do_stack >= 2)
         //   continue;   // solderside found, end of stacking

         //stackupcnt++;

         //if (!strncmp(lname, "INSULATION", 4))
         //{
         //   // ALL insulation layers have the same name --
         //   lname.Format("%s_%d", lp, ++insulationcnt);
         //}

         //int l = getThedaReader().getDefinedLayerIndex(lname);
         //LayerStruct *ll = doc->FindLayer(l);
         //ll->setArtworkStackNumber(stackupcnt);

         //if (!strncmp(lname, "INSULATION", 4))
         //{
         //   ll->setLayerType(LAYTYPE_DIALECTRIC);
         //}
         //else if (!strcmp(lname, QComponentSide))
         //{
         //   electcnt++;
         //   ll->setLayerType(LAYTYPE_SIGNAL_TOP);
         //   ll->setElectricalStackNumber(electcnt);
         //   do_stack++;

         //   // here check for lname:PAD
         //   assign_layer(lname, ":PAD", electcnt, LAYTYPE_PAD_TOP);
         //   assign_layer(lname, ":PAD|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         //   assign_layer(lname, ":PAD|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
         //   assign_layer(lname, "|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         //   assign_layer(lname, "|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
         //}
         //else if (!strcmp(lname, QSolderSide))
         //{
         //   electcnt++;
         //   ll->setLayerType(LAYTYPE_SIGNAL_BOT);
         //   ll->setElectricalStackNumber(electcnt);
         //   do_stack++;

         //   // here check for lname:PAD
         //   assign_layer(lname, ":PAD", electcnt, LAYTYPE_PAD_BOTTOM);
         //   assign_layer(lname, ":PAD|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         //   assign_layer(lname, ":PAD|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
         //   assign_layer(lname, "|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         //   assign_layer(lname, "|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
         //}
         //else if (!strncmp(lname, "PWR_GND", 7))
         //{
         //   electcnt++;
         //   ll->setLayerType(LAYTYPE_POWERNEG);
         //   ll->setElectricalStackNumber(electcnt);

         //   // here check for lname:PAD
         //   assign_layer(lname, ":PAD", electcnt, LAYTYPE_POWERNEG);
         //}
         //else if (!strncmp(lname, "INNER", 5))
         //{
         //   electcnt++;
         //   ll->setLayerType(LAYTYPE_SIGNAL_INNER);
         //   ll->setElectricalStackNumber(electcnt);

         //   // here check for lname:PAD
         //   assign_layer(lname, ":PAD", electcnt, LAYTYPE_PAD_INNER);
         //}

         //if (lp = strtok(NULL," \t"))  // thickness
         //{
         //}
         //if (lp = strtok(NULL," \t"))  // tolerance
         //{
         //}
         //if (lp = strtok(NULL," \t"))  // name
         //{
         //}
      }

      get_newline();
   }

   get_newline();

   processLayerStructureLayers();

   return 1;
}

static void addLayerStructureLayerName(const CString& layerName)
{
   s_layerStructureLayerNames.AddTail(layerName);
}

static void processLayerStructureLayers()
{
   int electcnt = 0;
   int stackupcnt = 0;
   int insulationcnt = 0;
   int do_stack = 0;

   for (POSITION pos = s_layerStructureLayerNames.GetHeadPosition();pos != NULL;)
   {
      CString lname = s_layerStructureLayerNames.GetNext(pos);
      CString lp(lname);

      if (do_stack >= 2)
         continue;   // solderside found, end of stacking

      stackupcnt++;

      if (!strncmp(lname, "INSULATION", 4))
      {
         // ALL insulation layers have the same name --
         lname.Format("%s_%d", lp, ++insulationcnt);
      }

      int l = getThedaReader().getDefinedLayerIndex(lname);
      LayerStruct *ll = doc->FindLayer(l);
      ll->setArtworkStackNumber(stackupcnt);

      if (!strncmp(lname, "INSULATION", 4))
      {
         ll->setLayerType(LAYTYPE_DIALECTRIC);
      }
      else if (!strcmp(lname, QComponentSide))
      {
         electcnt++;
         ll->setLayerType(LAYTYPE_SIGNAL_TOP);
         ll->setElectricalStackNumber(electcnt);
         do_stack++;

         // here check for lname:PAD
         assign_layer(lname, ":PAD", electcnt, LAYTYPE_PAD_TOP);
         assign_layer(lname, ":PAD|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         assign_layer(lname, ":PAD|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
         assign_layer(lname, "|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         assign_layer(lname, "|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
      }
      else if (!strcmp(lname, QSolderSide))
      {
         electcnt++;
         ll->setLayerType(LAYTYPE_SIGNAL_BOT);
         ll->setElectricalStackNumber(electcnt);
         do_stack++;

         // here check for lname:PAD
         assign_layer(lname, ":PAD", electcnt, LAYTYPE_PAD_BOTTOM);
         assign_layer(lname, ":PAD|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         assign_layer(lname, ":PAD|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
         assign_layer(lname, "|COMP_SIDE", electcnt, LAYTYPE_PAD_TOP);
         assign_layer(lname, "|SOLD_SIDE", electcnt, LAYTYPE_PAD_BOTTOM);
      }
      else if (!strncmp(lname, "PWR_GND", 7))
      {
         electcnt++;
         ll->setLayerType(LAYTYPE_POWERNEG);
         ll->setElectricalStackNumber(electcnt);

         // here check for lname:PAD
         assign_layer(lname, ":PAD", electcnt, LAYTYPE_POWERNEG);
      }
      else if (!strncmp(lname, "INNER", 5))
      {
         electcnt++;
         ll->setLayerType(LAYTYPE_SIGNAL_INNER);
         ll->setElectricalStackNumber(electcnt);

         // here check for lname:PAD
         assign_layer(lname, ":PAD", electcnt, LAYTYPE_PAD_INNER);
      }
   }
}

/******************************************************************************
* physical_layout_layer_structure
   Layerstructure does not follow a scannable methode
*/
static int physical_layout_layer_structure()
{
   do_layerstructure();

   return 1;
}

/******************************************************************************
* physical_layout_outlines
*/
static int physical_layout_outlines()
{
   cur_netname = "";

   return loop_command(outlines_lst,SIZ_OUTLINES_LST, "OUTLINES");
}

/******************************************************************************
* physical_layout_cutting_marks
*/
static int physical_layout_cutting_marks()
{
   return loop_command(cutting_marks_lst, SIZ_CUTTING_MARKS_LST, "CUTTING_MARKS");
}

/******************************************************************************
* physical_layout_keepins
*/
static int physical_layout_keepins()
{
   return loop_command(keepins_lst, SIZ_KEEPINS_LST, "KEEPINS");
}

/******************************************************************************
* physical_layout_keepouts
*/
static int physical_layout_keepouts()
{
   return loop_command(keepouts_lst, SIZ_KEEPOUTS_LST, "KEEPOUTS");
}

/******************************************************************************
* physical_layout_components
*/
static int physical_layout_components()
{
   return loop_command(phys_components_lst, SIZ_PHYS_COMPONENTS_LST, "COMPONENTS");
}

/******************************************************************************
* physical_layout_signals
*/
static int physical_layout_signals()
{
   return loop_command(phys_signals_lst, SIZ_PHYS_SIGNALS_LST, "SIGNALS");
}

/******************************************************************************
* physical_layout_copper_areas
*/
static int physical_layout_copper_areas()
{
   return loop_command(phys_copper_areas_lst, SIZ_PHYS_COPPER_AREAS_LST, "COPPER_AREAS");
}

/******************************************************************************
* old_physical_layout_reference_pads
*/
static int old_physical_layout_reference_pads()
{
   get_newline();

   while (TRUE)
   {
      if (!get_tok())
      {
         return parsingError();
      }

      char w[MAX_LINE];
      strcpy(w, cur_line);
      char *lp = strtok(w, " ;\t\n");

      if (lp && !STRNICMP(lp, "..FIN_REFERENCE_PADS", 4))   // x y radius
      {
         break;
      }
      else if (lp && lp[0] == '#')
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp, "PAD_NUMBER"))
      {
         // do nothing
      }
      else
      {
         CString padnumber = lp;

         if ((lp = strtok(NULL, " \t")) == NULL)
            continue; // padnumber

         double x = cnv_unit(lp);

         if ((lp = strtok(NULL, " \t")) == NULL)
            continue; // padnumber

         double y = cnv_unit(lp);

         if ((lp = strtok(NULL, " '\t")) == NULL)
            continue; // padnumber

         CString padstack_id = getThedaReader().getPadStackTemplateGeometryName(lp);

         DataStruct *data = Graph_Block_Reference(padstack_id, padnumber, cur_filenum, x, y, 0, 0, 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeFreePad);
      }

      get_newline();
   }

   get_newline();

   return 1;
}

/******************************************************************************
* refPad_layer
*/
static int refPad_layer()
{
   if (!get_tok())
      return parsingError();

   cur_refPad.layer = token;

   get_newline();

   return 1;
}

/******************************************************************************
* refPad_location
*/
static int refPad_location()
{
   if (!get_tok())
      return parsingError();

   cur_refPad.x = cnv_unit(token);

   if (!get_tok())
      return parsingError();

   cur_refPad.y = cnv_unit(token);

   //getThedaReader().setTestProbeLocation(cur_refPad.x,cur_refPad.y);

   get_newline();

   return 1;
}

/******************************************************************************
* refPad_rotation
*/
static int refPad_rotation()
{
   if (!get_tok())
      return parsingError();

   cur_refPad.rotation = atof(token);

   get_newline();

   return 1;
}

/******************************************************************************
* refPad_graphics
*/
static int refPad_graphics()
{
   fnull("GRAPHICS", FALSE);

   return 1;
}

/******************************************************************************
* refPad_x_mirror
*/
static int refPad_x_mirror()
{
   cur_refPad.x_mirror = TRUE;
   get_newline();

   return 1;
}

/******************************************************************************
* refPad_padstack_id
*/
static int refPad_padstack_id()
{
   if (!get_tok())
      return parsingError();

   cur_refPad.padstack_id = token;

   get_newline();

   return 1;
}

/******************************************************************************
* refPad_mark
*/
static int refPad_mark()
{
   if (!get_tok())
      return parsingError();

   get_newline();

   return 1;
}

/******************************************************************************
* refPad
*/
static int refPad()
{
   get_newline();

   cur_refPad.x = 0;
   cur_refPad.y = 0;
   cur_refPad.rotation = 0;
   cur_refPad.x_mirror = 0;
   cur_refPad.padstack_id = "";

   int res = loop_command(tlRefPad_lst, SIZ_TLREFPAD_LST, "REFERENCE_PAD");

   CString padstackName = getThedaReader().getPadStackTemplateGeometryName(cur_refPad.padstack_id);

   bool mirrorFlag = getThedaReader().isSolderSideName(cur_refPad.layer);

   DataStruct *data = Graph_Block_Reference(padstackName, NULL, cur_filenum, cur_refPad.x, cur_refPad.y,
      DegToRad(cur_refPad.rotation), mirrorFlag ? MIRROR_ALL : 0, 1, -1, TRUE);

   data->getInsert()->setInsertType(insertTypeFreePad);

   getThedaReader().addReferencePad(*data);

   get_newline();

   return res;
}

/******************************************************************************
* physical_layout_reference_pads
*/
static int physical_layout_reference_pads()
{
   int res;

   if (tlversion.software_revision >= 4)
   {
      res = loop_command(tlRefPads_lst, SIZ_TLREFPADS_LST, "REFERENCE_PADS");
   }
   else
   {
      res = old_physical_layout_reference_pads();
   }

   return res;
}

/******************************************************************************
* physical_layout_drawings
*/
static int physical_layout_drawings()
{
   return loop_command(drawings_lst, SIZ_DRAWINGS_LST, "DRAWINGS");
}

/******************************************************************************
* physical_layout_inverted_conductions
*/
static int physical_layout_inverted_conductions()
{
   // clipped from phys_signal_inverted_conductions
   int res  = loop_command(conductions_lst, SIZ_CONDUCTIONS_LST, "INVERTED_CONDUCTIONS");

   return res;
}

/******************************************************************************
* parameters_current_parameters
*/
static int parameters_current_parameters()
{
   return loop_command(current_parameter_lst, SIZ_CURRENT_PARAMETER_LST, "CURRENT_PARAMETERS");
}

/******************************************************************************
* parameters_parameter_sets
*/
static int parameters_parameter_sets()
{
   return loop_command(parametersets_lst, SIZ_PARAMETER_LST, "PARAMETER_SETS");
}

/******************************************************************************
* current_parameters_analyze
*/
static int current_parameters_analyze()
{
   fnull("ANALYZE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_autoplace
*/
static int current_parameters_autoplace()
{
   fnull("AUTOPLACE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_autorename
*/
static int current_parameters_autorename()
{
   fnull("AUTORENAME", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_autoroute
*/
static int current_parameters_autoroute()
{
   fnull("AUTOROUTE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_check
*/
static int current_parameters_check()
{
   fnull("CHECK", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_cutout
*/
static int current_parameters_cutout()
{
   fnull("CUTOUT", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_define
*/
static int current_parameters_define()
{
   fnull("DEFINE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_display
*/
static int current_parameters_display()
{
   fnull("DISPLAY", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_move
*/
static int current_parameters_move()
{
   fnull("MOVE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_postprocess
*/
static int current_parameters_postprocess()
{
   fnull("POSTPROCESS", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_postroute
*/
static int current_parameters_postroute()
{
   fnull("POSTROUTE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_postplace
*/
static int current_parameters_postplace()
{
   fnull("POSTPLACE", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_read
*/
static int current_parameters_read()
{
   fnull("READ", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_write
*/
static int current_parameters_write()
{
   return loop_command(write_lst, SIZ_WRITE_LST, "WRITE");
}

/******************************************************************************
* current_parameters_select
*/
static int current_parameters_select()
{
   fnull("SELECT", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_search
*/
static int current_parameters_search()
{
   fnull("SEARCH", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_swap
*/
static int current_parameters_swap()
{
   fnull("SWAP", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_resolve
*/
static int current_parameters_resolve()
{
   fnull("RESOLVE", FALSE);

   return 1;
}

/******************************************************************************
* parameterset_parameter
*/
static int parameterset_parameter()
{
   fnull("PARAMETER_SET", FALSE);

   return 1;
}

/******************************************************************************
* write_tl
*/
static int write_tl()
{
   fnull("TL", FALSE);

   return 1;
}

/******************************************************************************
* write_dxf
*/
static int write_dxf()
{
   fnull("DXF", FALSE);

   return 1;
}

/******************************************************************************
* current_parameters_route
*/
static int current_parameters_route()
{
   fnull("ROUTE", FALSE);

   return 1;
}

/******************************************************************************
* physical_layout_conductions
*/
static int physical_layout_conductions()
{
   // clipped from phys_signal_conductions
   int res  = loop_command(conductions_lst,SIZ_CONDUCTIONS_LST, "CONDUCTIONS");
   return res;
}

/******************************************************************************
* physical_layout_violations
*/
static int physical_layout_violations()
{
   fnull("VIOLATIONS", FALSE);

   return 1;
}

/******************************************************************************
* physical_layout_drill_holes
*/
static int physical_layout_drill_holes()
{
   fnull("DRILL_HOLES", FALSE);

   return 1;
}

/******************************************************************************
* physical_layout_post_processing
*/
static int physical_layout_post_processing()
{
   fnull("POST_PROCESSING", FALSE);

   return 1;
}

/******************************************************************************
* package_insertion
*/
static int package_insertion()
{
   fnull("INSERTION", FALSE);

   return 1;
}

/******************************************************************************
* package_mounting_holes
*/
static int package_mounting_holes()
{
   fnull("MOUNTING_HOLES", FALSE);

   return 1;
}

/******************************************************************************
* package_dimensions
*/
static int package_dimensions()
{
   return loop_command(dimensions_lst, SIZ_DIMENSIONS_LST, "DIMENSIONS");

   //fnull("DIMENSIONS", FALSE);

   //return 1;
}

/******************************************************************************
* package_conductions
*/
static int package_conductions()
{
   // Clipped from phys_signal_conductions
   int res  = loop_command(conductions_lst,SIZ_CONDUCTIONS_LST, "CONDUCTIONS");

   return res;
}

/******************************************************************************
* package_keepouts
*/
static int package_keepouts()
{
   return loop_command(keepouts_lst, SIZ_KEEPOUTS_LST, "KEEPOUTS");

   //fnull("KEEPOUTS", FALSE);

   //return 1;
}

/******************************************************************************
* physical_layout_measurements
*/
static int physical_layout_measurements()
{
   fnull("MEASUREMENTS", FALSE);

   return 1;
}

/******************************************************************************
* net_list_set_up
*/
static int net_list_set_up()
{
   fnull("SET_UP", FALSE);

   return 1;
}

/******************************************************************************
* net_list_components
*/
static int net_list_components()
{
   return loop_command(net_components_lst, SIZ_NET_COMPONENTS_LST, "COMPONENTS");
}

/******************************************************************************
* net_list_nets
*/
static int net_list_nets()
{
   return loop_command(net_nets_lst, SIZ_NET_NETS_LST, "NETS");
}

/******************************************************************************
* clear_graphicsettings
*/
static int clear_graphicsettings()
{
   cur_technique = ' ';
   cur_layer = "";
   cur_layerindex = getThedaReader().getFloatingLayerIndex();   // is the NO layer, which allows shapes and symbols
                                             // to inherent the placed layer.
   cur_symbol_type = 0;
   cur_widthindex = 0;
   cur_closed = 0;
   getThedaReader().setCurrentSymbolTypeString("");
   //cur_layer_materials = "";
   cur_sym_type = "";
   cur_offset_x = cur_offset_y = 0;
   cur_rotation = 0;
   cur_graphic_place = "";
   cur_filled = FALSE;
   cur_ate_probe = FALSE;
   cur_diagnostic_probe = FALSE;
   cur_bareboard_probe = FALSE;
   cur_ignore_shape_id = FALSE;
   cur_x_mirror = FALSE;

   return 1;
}

/******************************************************************************
* keepins_keepin
*/
static int keepins_keepin()
{
   cur_section = SECTION_KEEPIN;

   get_newline();
   clear_graphicsettings();
   int res  = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "KEEPIN");
   get_newline();

   cur_section = 0;

   return res;
}

/******************************************************************************
* FindFirstTextPen
*/
static TLTextpen *FindFirstTextPen()
{
   for (int i=0; i<textpencnt; i++)
   {
      TLTextpen *textPen = textpenarray[i];

      if (textPen != NULL)
      {
         return textPen;
      }
   }

   return NULL;
}

/******************************************************************************
* text_pens_text_pen
*/
static int text_pens_text_pen()
{
   get_newline();
   clear_graphicsettings();

   TLTextpen *c = new TLTextpen;
   textpenarray.SetAtGrow(textpencnt, c);

   int res  = loop_command(tltextpen_lst, SIZ_TLTEXTPEN_LST, "TEXT_PEN");

   textpencnt++;
   get_newline();

   return res;
}

/******************************************************************************
* text_pens_text_pen_1
*/
static int text_pens_text_pen_1()
{
   get_newline();
   clear_graphicsettings();

   TLTextpen *c = new TLTextpen;
   textpenarray.SetAtGrow(textpencnt,c);

   int res  = loop_command(tltextpen_lst, SIZ_TLTEXTPEN_LST, "PEN");

   textpencnt++;
   get_newline();

   return res;
}

/******************************************************************************
* cutting_marks_cutting_mark
*/
static int cutting_marks_cutting_mark()
{
   get_newline();
   clear_graphicsettings();
   int res = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "CUTTING_MARK");
   get_newline();

   return res;
}

/******************************************************************************
* drawings_drawing
*/
static int drawings_drawing()
{
   get_newline();
   clear_graphicsettings();

   int res = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "DRAWING");

   get_newline();

   return res;
}

/******************************************************************************
* get_netlistcomp_ptr
*/
static int get_netlistcomp_ptr(const char *refname)
{
   for (int i=0; i<netlistcompcnt; i++)
   {
      if (!strcmp(netlistcomparray[i]->reference,refname))
         return i;
   }

   return -1;
}

/******************************************************************************
* comp_pulled_pin
*/
static int comp_pulled_pin(const char *refname, const char *pin)
{
   int netlistptr = get_netlistcomp_ptr(refname);

   if (netlistptr < 0)
      return FALSE;

   TLNetlistComp* netListComp = netlistcomparray[netlistptr];

   if (strlen(netListComp->pulled_pins) == 0)
      return FALSE;

   CString pinname;
   pinname.Format("'%s'", pin);

   if (netListComp->pulled_pins.Find(pinname, 0) > -1)
      return TRUE;

   return FALSE;
}

/******************************************************************************
* pins_pin
*/
static int pins_pin()
{
   get_newline();

   cur_pin.rotation = 0;
   cur_pin.x = 0;
   cur_pin.y = 0;
   cur_pin.x_mirror = FALSE;

   int res = loop_command(tlpin_lst, SIZ_TLPIN_LST, "PIN");

   // here place a pin, the rotation of a padstack is not the
   if (cur_pin.x_mirror)
   {
      cur_pin.rotation = cur_pin.rotation + 180;
   }

   normalizeToPositiveDegrees(cur_pin.rotation);

   if (!comp_pulled_pin(cur_comp.refdes, cur_pin.identifier))
   {
      DataStruct *data = Graph_Block_Reference(cur_pin.padstack_id, cur_pin.identifier, cur_filenum,
            cur_pin.x, cur_pin.y, DegToRad(cur_pin.rotation), 0 , 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypePin);

      if (strlen(cur_pin.number))
      {
         doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_COMPPINNR,cur_pin.number, SA_OVERWRITE, NULL);
      }
   }

   get_newline();

   return res;
}

/******************************************************************************
* pin_wires
*/
static int pin_wires()
{
   if (!comp_pulled_pin(cur_comp.refdes, cur_pin.identifier))
   {
      return loop_command(wires_lst, SIZ_WIRES_LST, "WIRES");
   }
   else
   {
      fnull("WIRES", FALSE);
   }

   return 1;
}

/******************************************************************************
* pin_text_nodes
*/
static int pin_text_nodes()
{
   fnull("TEXT_NODES", FALSE);

   return 1;
}

/******************************************************************************
* pin_probe_pads
*/
static int pin_probe_pads()
{
   if (BAREBOARD_PROBES == FALSE)
   {
      fnull("PROBE_PADS", FALSE);

      return 1;
   }

   return probes_probe();
}

/******************************************************************************
* pin_chip_bond_lands
*/
static int pin_chip_bond_lands()
{
   if (!comp_pulled_pin(cur_comp.refdes, cur_pin.identifier))
   {
      return loop_command(chip_bond_lands_lst, SIZ_CHIP_BOND_LANDS_LST, "CHIP_BOND_LANDS");
   }
   else
   {
      fnull("CHIP_BOND_LANDS", FALSE);
   }

   return 1;
}

/******************************************************************************
* pin_graphics
*/
static int pin_graphics()
{
   fnull("GRAPHICS", FALSE);

   return 1;
}

/******************************************************************************
* chip_bond_lands_chip_bond_land
   this is a bonding wire attached to a pin.
*/
static int chip_bond_lands_chip_bond_land()
{
   if (!get_tok())
      return parsingError();

   int wirenum = atoi(token);

   CString bonddef = getThedaReader().getBondLandGeometryName(cur_blockname, cur_pin.identifier, wirenum);
   //bonddef.Format("%s_%s_%d", cur_blockname, cur_pin.identifier, wirenum);

   BlockStruct *b = &(getThedaReader().graphBlockOn(bonddef, cur_filenum,blockTypeUnknown));
   int res  = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "CHIP_BOND_LAND");
   Graph_Block_Off();

   // here place a wire, the rotation of a padstack is not the
   DataStruct *data = Graph_Block_Reference(bonddef, "", cur_filenum,
         cur_pin.x, cur_pin.y, DegToRad(cur_pin.rotation), 0 , 1.0, -1, TRUE);

   doc->SetUnknownAttrib(&data->getAttributesRef(),"CHIP_BOND_LAND", "", SA_OVERWRITE, NULL);

   get_newline();

   return res;
}

/******************************************************************************
* wires_wire
   this is a bonding wire attached to a pin.
*/
static int wires_wire()
{
   if (!get_tok())
      return parsingError();

   int wirenum = atoi(token);

   CString wiredef = getThedaReader().getWireGeometryName(cur_blockname, cur_pin.identifier, wirenum);
   //wiredef.Format("%s_%s_%d", cur_blockname, cur_pin.identifier, wirenum);

   BlockStruct *b = &(getThedaReader().graphBlockOn(wiredef, cur_filenum,blockTypeUnknown));
   int res  = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "WIRE");
   Graph_Block_Off();

   // here place a wire, the rotation of a padstack is not the
   DataStruct *data = Graph_Block_Reference(wiredef, "", cur_filenum,
         cur_pin.x, cur_pin.y, DegToRad(cur_pin.rotation), 0 , 1.0, -1, TRUE);

   doc->SetUnknownAttrib(&data->getAttributesRef(),"PIN_WIRE", "", SA_OVERWRITE, NULL);

   get_newline();

   return res;
}

/******************************************************************************
* properties_property
*/
static int properties_property()
{
   get_newline();

   cur_prop.number = "";
   cur_prop.identifier = "";
   cur_prop.description = "";

   int res = loop_command(tlproperty_lst, SIZ_TLPROPERTY_LST, "PROPERTY");

   if (comp_prop)
   {
      TLCompProp  *c = new TLCompProp;
      compproparray.SetAtGrow(comppropcnt++, c);
      c->devicecompindex = devicecompcnt;
      c->keyword = cur_prop.identifier;
      c->value = cur_prop.description;
   }
   else
   {
      // here place a property
      if (curblock && strlen(cur_prop.identifier))
         doc->SetUnknownAttrib(&curblock->getAttributesRef(), get_attribmap(cur_prop.identifier), cur_prop.description, SA_OVERWRITE, NULL);
   }

   get_newline();

   return res;
}

/******************************************************************************
* has_already_layer
*/
static int has_already_layer(int *expandlayer, int *expandlayercnt, int laynr)
{
   for (int i=0; i<*expandlayercnt; i++)
   {
      if (expandlayer[i] == laynr)
         return TRUE;
   }

   expandlayer[*expandlayercnt] = laynr;
   *expandlayercnt +=1;

   return FALSE;
}

/******************************************************************************
* Create_LayerrangePADSTACKData
*/
static int Create_LayerrangePADSTACKData(CDataList *DataList, const char *lay)
{
   int typ = 0;
   int expandlayer[1000];
   int expandlayercnt = 0;

   for (POSITION pos = DataList->GetHeadPosition();pos != NULL;)
   {
      DataStruct *np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      if (block != NULL && ((block->getFlags() & BL_TOOL)))
      {
         // always do the drill
         DataStruct  *data = Graph_Block_Reference(block->getName(), np->getInsert()->getRefname(), block->getFileNumber(),
               np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), np->getInsert()->getAngle(), np->getInsert()->getMirrorFlags(), np->getInsert()->getScale(), np->getLayerIndex(), TRUE);
         data->getInsert()->setInsertType(np->getInsert()->getInsertType());
      }
      else
      {
         // here check the layer.
         int layer = np->getLayerIndex();

         if (doc->IsFloatingLayer(np->getLayerIndex()))
         {
            // copy
            if ( block != NULL && ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE)) )
            {
               if (block->getShape() == T_COMPLEX)
                  block = doc->Find_Block_by_Num((int)block->getSizeA());

               if (block != NULL)
                  layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
            }
         }

         LayerStruct *l = doc->FindLayer(layer);

         if (l == NULL)
            break;

         int start = FALSE;
         char *lp;
         char tmp[1000];
         strcpy(tmp, lay);

         lp = strtok(tmp, ",");

         while (lp)
         {
            int doit = FALSE;

            if (!STRNICMP(lp, "INNER", strlen("INNER")))        // inner
            {
               if (!STRNICMP(l->getName(), "ALL_INNER", strlen("ALL_INNER")))
                  doit = TRUE;

               if (!STRNICMP(l->getName(), "ALL_SIG_PWRGND", strlen("ALL_SIG_PWRGND")))
                  doit = TRUE;
            }

            if (!STRNICMP(lp, "COMPONENT", strlen("COMPONENT")))   // comp
            {
               if (!STRNICMP(l->getName(), "COMPONENT", strlen("COMPONENT")))
                  doit = TRUE;
            }

            if (!STRNICMP(lp, QSolderSide, strlen(QSolderSide)))  // comp
            {
               if (!STRNICMP(l->getName(), QSolderSide, strlen(QSolderSide)))
                  doit = TRUE;
            }

            if (doit)
            {
               int laynr = getThedaReader().getDefinedLayerIndex(lp);

               if (block != NULL && !has_already_layer(expandlayer, &expandlayercnt, laynr))
               {
                  DataStruct *data = Graph_Block_Reference(block->getName(), np->getInsert()->getRefname(),
                        block->getFileNumber(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), np->getInsert()->getAngle(),
                        np->getInsert()->getMirrorFlags(), np->getInsert()->getScale(),laynr, TRUE);
                  data->getInsert()->setInsertType(np->getInsert()->getInsertType());
               }
            }

            lp = strtok(NULL, ",");
         }
      }
   }

   return typ;
}

/******************************************************************************
* make_blind_via
*/
static CString make_blind_via(const char *lay, const char *padstack_id)
{
   CString new_padstack_id;
   char *lp;
   char tmp[1000];

   strcpy(tmp, lay);
   lp = strtok(tmp, ",");
   CString startlayer = lp;

   CString endlayer;

   while (lp)
   {
      endlayer = lp;
      lp = strtok(NULL,",");
   }

   new_padstack_id.Format("%s_(%s_%s)", padstack_id, startlayer, endlayer);

   CString oldvia = getThedaReader().getPadStackTemplateGeometryName(padstack_id);
   CString newvia = getThedaReader().getPadStackTemplateGeometryName(new_padstack_id);

   if (Graph_Block_Exists(doc, newvia, cur_filenum) == NULL)
   {
      BlockStruct *block = Graph_Block_Exists(doc, oldvia, cur_filenum);

      BlockStruct* b1 = &(getThedaReader().graphBlockOn(newvia,cur_filenum,blockTypePadstack));
      //BlockStruct *b1 = Graph_Block_On(GBO_APPEND,newvia, cur_filenum, 0);
      //b1->setBlockType(BLOCKTYPE_PADSTACK);
      b1->setOriginalName(new_padstack_id);

      Create_LayerrangePADSTACKData(&(block->getDataList()), lay);
      Graph_Block_Off();

      doc->SetAttrib(&b1->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE,NULL,SA_OVERWRITE, NULL);
   }

   return new_padstack_id;
}

/******************************************************************************
* is_blind_via
*/
static int is_blind_via(const char *lay)
{
   char *lp;
   char tmp[1000];

   if (strlen(lay) == 0)
      return FALSE;

   strcpy(tmp, lay);

   lp = strtok(tmp, ",");
   CString startlayer = lp;
   CString endlayer;

   while (lp)
   {
      endlayer = lp;
      lp = strtok(NULL, ",");
   }

   // if it starts and ends in comp and solder layer, it is not a blind/buried via.
   if (!startlayer.CompareNoCase(QComponentSide) && !endlayer.CompareNoCase(QSolderSide))
      return FALSE;

   return TRUE;
}

/******************************************************************************
* vias_via
*/
static int vias_via()
{
   get_newline();

   cur_via.x = 0;
   cur_via.y = 0;
   cur_via.rotation = 0;
   cur_via.x_mirror = 0;
   cur_via.needed = "";

   int res = loop_command(tlvia_lst, SIZ_TLVIA_LST, "VIA");

   CString viaPadStackName = getThedaReader().getPadStackTemplateGeometryName(cur_via.padstack_id);

   if (is_blind_via(cur_via.needed))
   {
      CThedaPartialVia* partialVia =
         CThedaPartialVias::getThedaPartialVias(*doc, cur_filenum).getDefinedPartialVia(cur_via.needed,cur_via.padstack_id);

      cur_via.padstack_id = partialVia->getId();
      viaPadStackName = partialVia->getViaStackName();
   }

   DataStruct *viaData = Graph_Block_Reference(viaPadStackName, NULL, cur_filenum, cur_via.x, cur_via.y, DegToRad(cur_via.rotation), 0 , 1.0, -1, TRUE);
   viaData->getInsert()->setInsertType(insertTypeVia);

   getThedaReader().addVia(*viaData);

   if (strlen(cur_netname))
      doc->SetAttrib(&viaData->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);

   get_newline();

   return res;
}

/******************************************************************************
* test_points_test_point
*/
static int test_points_test_point()
{
   CString  geomname, pinname, placename;

   get_newline();

   cur_via.x = 0;
   cur_via.y = 0;
   cur_via.layer.Empty();
   cur_via.x_mirror = 0;
   cur_via.needed.Empty();
   getThedaReader().clearContactLayerName();

   test_point_cnt++;
   int res = loop_command(tltestPoint_lst,SIZ_TLTestPoint_LST, "TEST_POINT");

   geomname.Format("TEST_POINT_DEFINITION_%s", cur_via.padstack_id);
   CString padstackname = getThedaReader().getPadStackTemplateGeometryName(cur_via.padstack_id);
   pinname = "1";

   placename.Format("TEST_POINT_%d", test_point_cnt);

   BlockStruct* padStackGeometry = Graph_Block_Exists(doc,padstackname,cur_filenum);

   //if (padStackGeometry != NULL)
   //{
   //   CString floatToLayerName = getThedaReader().getFloatToLayerName();
   //   BlockStruct& rectifiedPadStackGeometry = getThedaReader().getRectifiedGeometry(*padStackGeometry,floatToLayerName,true);
   //   padstackname = rectifiedPadStackGeometry.getName();
   //}

   BlockStruct *block = Graph_Block_Exists(doc, geomname, cur_filenum);

   if (block == NULL)
   {
      // define a geometry
      block = &(getThedaReader().graphBlockOn(geomname,cur_filenum,blockTypeTestPoint));

      // make a one pin component
      DataStruct *data = Graph_Block_Reference(padstackname, pinname, cur_filenum, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypePin);

      Graph_Block_Off();
   }

   // need to check mirror
   int mir = 0;

   if (!STRCMPI(cur_via.layer, QSolderSide))
      mir = 1;

   DataStruct* tpData = Graph_Block_Reference(geomname, placename, cur_filenum, cur_via.x, cur_via.y, DegToRad(0.0), mir , 1.0,-1, TRUE);
   tpData->getInsert()->setInsertType(insertTypeTestPoint);

   getThedaReader().addTestPoint(*tpData);

   /* add to netlist */
   //NetStruct *net = add_net(file, cur_netname);
   if (!comp_pulled_pin(placename, pinname))
   {
      CThedaComponentPin& componentPin = getThedaReader().getThedaComponentPins().addComponentPin(cur_netname,placename,pinname);

      //CompPinStruct *compPin = add_comppin(file, net, placename, pinname);
      //knv - add attribute to duplicate comppins
      doc->SetAttrib(&(componentPin.getCompPin()->getAttributesRef()), doc->IsKeyWord(ATT_TEST, 0), VT_STRING, "", SA_OVERWRITE, NULL);
   }

   get_newline();

   getThedaReader().clearComponentPlacementLayerName();

   return res;
}

/******************************************************************************
* net_pins_pin
*/
static int net_pins_pin()
{
   get_newline();

   cur_comppin.compname = "";
   cur_comppin.pinname = "";
   int res = loop_command(net_comppin_lst, SIZ_NET_COMPPIN_LST, "PIN");

   //NetStruct *n = add_net(file, cur_netname);

// here netlist add_n
   if (strlen(cur_netname) && strlen(cur_comppin.compname) && strlen(cur_comppin.pinname))
   {
      if (!already_comppin(file, cur_netname, cur_comppin.compname, cur_comppin.pinname))
      {
         if (!comp_pulled_pin(cur_comppin.compname, cur_comppin.pinname))
         {
            getThedaReader().getThedaComponentPins().addComponentPin(cur_netname,cur_comppin.compname,cur_comppin.pinname);
            //add_comppin(file, n, cur_comppin.compname, cur_comppin.pinname);
         }
      }
   }

   get_newline();

   return res;
}

/******************************************************************************
* clear_padgraphic_layer
*/
static int clear_padgraphic_layer(const char *bname, int layerindex)
{
   int err = 0;
   int floatlayer = getThedaReader().getFloatingLayerIndex();
   BlockStruct *block = Graph_Block_Exists(doc, bname, cur_filenum);

   POSITION pos = block->getDataList().GetHeadPosition();

   while (pos != NULL)
   {
      DataStruct *np = block->getDataList().GetNext(pos);

      if (np->getLayerIndex() == layerindex)
         np->setLayerIndex(floatlayer);
      else
         err = 1; // complex padstack with multiple graphic layers found !
   }

   return err;
}

/******************************************************************************
* graphics_graphic
*/
static int graphics_graphic()
{
   CString pshapename;
   BlockStruct *b;

   if (!get_tok())
      return parsingError();

   int grnum = atoi(token);

   clear_graphicsettings();

   // start an aperture. Every graphic under a padstack
   //is always an aperture, because I do not know what comes later.
   if (cur_section == SECTION_PADSTACKGRAPHIC)
   {
      pshapename.Format("%s_PADSHAPE_%d", curblock->getOriginalName(), grnum);
      b = &(getThedaReader().graphBlockOn(pshapename, cur_filenum,blockTypeUnknown));
      b->setBlockType(BLOCKTYPE_PADSHAPE);
   }

   int res = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "GRAPHIC");
   get_newline();

   if (cur_section == SECTION_PADSTACKGRAPHIC)  // end it.
   {
      Graph_Block_Off();

      // if the graphic was a graphic list, vs a shape_id or symbol_id
      if (ignore(cur_sym_type, cur_section, 0, cur_diagnostic_probe, cur_ate_probe) || ignore(cur_layer, cur_section, 0 , 0 , 0))
         cur_ignore_shape_id = TRUE;

      // ignore if technique is not the same
      if (cur_technique != ' ' && cur_technique != design_technique)
         cur_ignore_shape_id = TRUE;

      // this whole thing is empty.
      if (b->getDataList().IsEmpty() && b->getFlags() == 0)
         cur_ignore_shape_id = TRUE;

      // delete this block entry : pshapename
      if (cur_ignore_shape_id)
      {
         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block == b)
            {
               // call FreeBlock to clean all memory
               doc->FreeBlock(b);
               break;
            }
         }
      }
      else
      {
         // here I attach the block, which can be another padstack.
         // in optimize padstack, I resolve this !

         int mirror = 0; //((cur_x_mirror != 0) ? MIRROR_ALL : 0);

         DataStruct *data = Graph_Block_Reference(pshapename, NULL, cur_filenum, 0.0, 0.0, 0.0,mirror, 1.0, -1, TRUE);
         // needs cur_sym_tpye_string and cur_symbol_type
         data->getInsert()->setInsertType(do_shape_id_inserttype());
      }

   }

   return res;
}

/******************************************************************************
* net_components_component
*/
static int net_components_component()
{
   get_newline();

   TLNetlistComp* c = new TLNetlistComp();
   netlistcomparray.SetAtGrow(netlistcompcnt, c);

   int res = loop_command(net_comp_lst,SIZ_NET_COMP_LST, "COMPONENT");
   netlistcompcnt++;

   get_newline();
   return res;
}

/******************************************************************************
* get_devicecomp_from_library_id
*/
static int get_devicecomp_from_library_id(const char *library_id)
{
   for (int i=0; i<devicecompcnt; i++)
   {
      if (!strcmp(devicecomparray[i]->identifier, library_id))
         return i;
   }
   return -1;
}

/******************************************************************************
* place_comp

!  !  @  COMPONENT[ 1139 ]
!  !  @  !  REFERENCE_DESIGNATOR    := 'U108';
!  !  @  !  TYPE                    := NORMAL_COMPONENT;
!  !  @  !  PACKAGE_ID              := 'osc-4p-m3h-co12';
!  !  @  !  PIN_IDS                 := PHYSICAL;
!  !  @  !..FIN_COMPONENT[ 1139 ];
!  !  @  COMPONENT[ 1140 ]
!  !  @  !  REFERENCE_DESIGNATOR    := 'U108A';
!  !  @  !  TYPE                    := NORMAL_COMPONENT;
!  !  @  !  LIBRARY_IDENTIFIER      := 'SMC-OSC-LCC4';
!  !  @  !  PIN_IDS                 := PHYSICAL;
!  !  @  !..FIN_COMPONENT[ 1140 ];

Here is a mix of PACKAGE_ID and LIBRARY_IDENTIFIER
*/
static int place_comp()
{
   int found = -1, netlistptr = -1;
   CString package_id;

   if (cur_comp.already_placed)
      return 1;

   if (cur_comp.placed)
   {
      package_id.Empty();
      found = -1;
      netlistptr = get_netlistcomp_ptr(cur_comp.refdes);

      if (netlistptr > -1)
      {
         TLNetlistComp* netListComp = netlistcomparray[netlistptr];

         if (strlen(netListComp->library_id))
         {
            found = get_devicecomp_from_library_id(netListComp->library_id);

            if (found > -1)
            {
               TLDeviceComp* deviceComp = devicecomparray[found];
               package_id = deviceComp->package_id;
            }
            else
            {
               package_id = netListComp->library_id;
            }
         }
         else
         {
            package_id = netListComp->package_id;
         }
      }

      if (!strlen(package_id))
      {
         fprintf(logFp, "Can not find libshape for comp [%s]\n",cur_comp.refdes);
         display_error++;
      }
      else
      {
         // here place
         if (cur_comp.mir == MIRROR_LAYERS)
         {
            // if PLACED on bottom, it is always mirrored around X in THEDA Version 1.1!
            if (tlversion.software_revision < 2)
            {
               if (!cur_comp.x_mirror && !cur_comp.y_mirror)
                  cur_comp.x_mirror = TRUE;
            }
         }

         if (cur_comp.x_mirror)
         {
            cur_comp.rot = 180 + cur_comp.rot;

            normalizeToPositiveDegrees(cur_comp.rot);

            cur_comp.mir |= MIRROR_FLIP;
         }
         else if (cur_comp.y_mirror)
         {
            cur_comp.mir |= MIRROR_FLIP;
         }

         //////////////////////////////
         CString componentLayerName(cur_comp.getComponentLayerName());

         if (getThedaReader().isSolderSideName(componentLayerName))
         {
            componentLayerName = QComponentSide;
         }

         //CString rectifiedPackageId = getThedaReader().getRectifiedPackageId(package_id,componentLayerName,s_pinNameToContactLayerMap);
         CString componentTemplateGeometryName = getThedaReader().getComponentTemplateGeometryName(package_id);

         cur_comp.already_placed = TRUE;
         setCurData(Graph_Block_Reference(componentTemplateGeometryName, cur_comp.refdes , cur_filenum, cur_comp.x, cur_comp.y,
                           DegToRad(cur_comp.rot), cur_comp.mir , 1.0,-1, TRUE));
         getCurData()->getInsert()->setInsertType(insertTypePcbComponent);
         getCurData()->getInsert()->setMirrorFlags(cur_comp.mir);
         getCurData()->attributes().CopyAll(cur_comp.getComponent().attributes());

         TLNetlistComp* netListComp = netlistcomparray[netlistptr];
         netListComp->setComponentData(getCurData()); // remember the pointer, so that I can react the local redefinition.

         if (netListComp->pin_ids_physical && use_pin_ids_physical && getCurData() != NULL)
         {
            BlockStruct* instanceSpecificGeometry = cur_comp.getInstanceSpecificGeometry();

            if (getCurData() != NULL && instanceSpecificGeometry != NULL)
            {
               getCurData()->getInsert()->setBlockNumber(instanceSpecificGeometry->getBlockNumber());
            }
         }

         //d->getInsert().c = netlistcomparray[found]->comptype;

         // add refdes
         if (strlen(cur_comp.refdes))
         {
            Attrib* refdesAttrib;

            if (getCurData()->getAttributesRef() != NULL &&   getCurData()->getAttributesRef()->Lookup(refNameKeyword,refdesAttrib))
               refdesAttrib->setStringValueIndex(doc->RegisterValue(cur_comp.refdes));
            else
               doc->SetAttrib(&getCurData()->getAttributesRef(),refNameKeyword, VT_STRING, cur_comp.refdes.GetBuffer(0),  SA_OVERWRITE, NULL);
         }

         if (found > -1)
         {
            if (strlen(devicecomparray[found]->identifier)) // device_id is not unique
            {
               doc->SetAttrib(&getCurData()->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, 0), VT_STRING,
                     devicecomparray[found]->identifier.GetBuffer(0),SA_OVERWRITE, NULL);
            }

            if (strlen(devicecomparray[found]->device_id))  // device_id is not unique
            {
               doc->SetAttrib(&getCurData()->getAttributesRef(),doc->RegisterKeyWord("DEVICE_ID", 0, VT_STRING), VT_STRING,
                     devicecomparray[found]->device_id.GetBuffer(0),SA_OVERWRITE, NULL);
            }
         }
         else
         {
            CString typ;
            typ.Format("%s_%s", package_id, cur_comp.refdes);

            doc->SetAttrib(&getCurData()->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, 0),
                     VT_STRING, typ.GetBuffer(0), SA_OVERWRITE, NULL);

            TypeStruct *t = AddType(file,typ); // changed from device_id. this is not unique
            BlockStruct *b = &(getThedaReader().graphBlockOn(package_id, cur_filenum,blockTypeUnknown));
            Graph_Block_Off();

            if (t->getBlockNumber() == -1)
            {
               t->setBlockNumber( b->getBlockNumber());
            }
            else if (t->getBlockNumber() != b->getBlockNumber())
            {
               fprintf(logFp, "Device [%s] has already a different Package [%s] assigned!\n", t->getName(), b->getName());
               display_error++;
            }
         }

         if (netlistptr > -1)
         {
            doc->SetAttrib(&getCurData()->getAttributesRef(),doc->RegisterKeyWord("LIBRARY_ID", 0, VT_STRING),
               VT_STRING,
               netlistcomparray[netlistptr]->library_id.GetBuffer(0),    // this makes a "real" char *
               SA_OVERWRITE, NULL); //
         }

         for (int ii=0; ii<comppropcnt; ii++)
         {
            TLCompProp *c = compproparray[ii];

            if (compproparray[ii]->devicecompindex == found)
            {
               doc->SetUnknownAttrib(&getCurData()->getAttributesRef(), get_attribmap(compproparray[ii]->keyword),
                  compproparray[ii]->value, SA_OVERWRITE, NULL);
            }
         }
      }
   }
   else
   {
      // unplaced component
      fprintf(logFp, "Component [%s] is not placed\n",cur_comp.refdes);
   }

   return 1;
}

/******************************************************************************
* phys_components_component
*/
static int phys_components_component()
{
   get_newline();

   cur_comp.initialize();
   setCurData(&(cur_comp.getComponent()));

   int res = loop_command(phys_comp_lst, SIZ_PHYS_COMP_LST, "COMPONENT");
   place_comp();
   getThedaReader().clearComponentPlacementLayerName();

   get_newline();

   return res;
}

/******************************************************************************
* phys_comp_placement

   V 1.1 placement
*/
static int phys_comp_placement()
{
   get_newline();
   cur_comp.placed = TRUE;
   int res = loop_command(phys_comp_placement_lst, SIZ_PHYS_COMP_PLACEMENT_LST, "PLACEMENT");

   get_newline();

   return res;
}

/******************************************************************************
* phys_signals_signal
*/
static int phys_signals_signal()
{
   get_newline();
   cur_netname = "";
   cur_filled = FALSE;
   clear_graphicsettings();
   cur_widthindex = cur_defaulttracewidthindex;

   int res = loop_command(phys_signal_lst, SIZ_PHYS_SIGNAL_LST, "SIGNAL");

   cur_netname = "";
   cur_filled = FALSE;
   get_newline();

   return res;
}

/******************************************************************************
* outlines_outline
*/
static int outlines_outline()
{
   cur_section = SECTION_BOARDOUTLINE;
   get_newline();
   clear_graphicsettings();

   int res  = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "OUTLINE");

   get_newline();
   cur_section = 0;

   return res;
}

/******************************************************************************
* keepouts_keepout
*/
static int keepouts_keepout()
{
   cur_section = SECTION_KEEPOUT;
   get_newline();
   clear_graphicsettings();

   int res  = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "KEEPOUT");

   get_newline();
   cur_section = 0;

   return res;
}

/******************************************************************************
* dimensions_dimension
*/
static int dimensions_dimension()
{
   cur_section = SECTION_DIMENSION;
   get_newline();
   clear_graphicsettings();

   int res  = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "DIMENSION");

   get_newline();
   cur_section = 0;

   return res;
}

/******************************************************************************
* rename_layer
*/
static const char  *rename_layer(const char *l)
{
   for (int i=0; i<layer_rename_cnt; i++)
   {
      if (!STRCMPI(l,layer_rename[i].origname))
         return layer_rename[i].rename;
   }

   return l;
}

/******************************************************************************
* adjust_graphic_place
*/
static const char  *adjust_graphic_place(const char *l)
{
   static CString  l1;

/*
   if (strlen(cur_graphic_place))   // only do it if a PLACE is COMPONENT or SOLDER
   {
      if (strstr(l,"SOLD"))
      {
         l1 = l;
         l1 +="_SS";
         return l1;
      }
      else
      if (strstr(l,"COMP"))
      {
         l1 = l;
         l1 +="_CS";
         return l1;
      }
   }
*/
   return l;
}

/******************************************************************************
* theda_layer
*/
static int theda_layer()
{
   if (!get_tok())
      return parsingError();

   cur_layer = rename_layer(strtok(token, ";"));
   cur_layer = adjust_graphic_place(cur_layer);

   int i;

   if (i = get_layerclass(cur_layer))
      cur_symbol_type = i;

   return 1;
}

/******************************************************************************
* theda_technique
*/
static int theda_technique()
{
   if (!get_tok())
      return parsingError();

   CString t = token;
   cur_technique = token[0];
   get_newline();

   return 1;
}

/******************************************************************************
* theda_fixed
*/
static int theda_fixed()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* theda_x_mirror
*/
static int theda_x_mirror()
{
   // do nothing
   cur_x_mirror = TRUE;
   get_newline();

   return 1;
}

/******************************************************************************
* net_probepad
*/
static int net_probepad()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* net_voltage
*/
static int net_voltage()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* net_power
*/
static int net_power()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* net_ground
*/
static int net_ground()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* theda_need
*/
static int theda_need()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* theda_material
*/
static int theda_material()
{
   char  *lp;
   //cur_layer_materials = "";

   if (!get_tok())
      return parsingError();

   lp = strtok(token, ";");
   //cur_layer_materials = lp;

   getThedaReader().setMaterial(lp);

   return 1;

}

/******************************************************************************
* theda_height
*/
static int theda_height()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* theda_z_offset
*/
static int theda_z_offset()
{
   // do nothing
   get_newline();

   return 1;
}

/******************************************************************************
* graphic_place
*/
static int graphic_place()
{
   // do nothing
   if (!get_tok())
      return parsingError();

   cur_graphic_place = token;
   get_newline();

   return 1;
}

/******************************************************************************
* graphic_contact
*/
static int graphic_contact()
{
   // do nothing
   if (!get_tok())
      return parsingError();

   get_newline();

   return 1;
}

/******************************************************************************
* is_layer_compoutline
*/
static int is_layer_compoutline(const char *l)
{
   for (int i=0; i<compoutlinecnt; i++)
   {
      if (compoutline[i].CompareNoCase(l) == 0)
         return 1;
   }

   return 0;
}

/******************************************************************************
* is_boardoutline
*/
static int is_boardoutline(const char *l)
{
   for (int i=0; i<boardoutlinecnt; i++)
   {
      if (boardoutline[i].section == cur_section && !boardoutline[i].layer.CompareNoCase(l))
         return i+1;
   }

   return 0;
}

/******************************************************************************
* do_symboltype
*/
static int do_symboltype(const char *lp)
{
   cur_sym_type = lp;

   getThedaReader().setCurrentSymbolTypeString(lp);

   if (cur_filestatus == FILESTATUS_BOARD)
   {
      if (is_boardoutline(cur_layer))
      {
         cur_symbol_type = GR_CLASS_BOARDOUTLINE;
         return 1;
      }

      if (is_layer_compoutline(cur_layer))
      {
         cur_symbol_type = GR_CLASS_COMPOUTLINE;
         return 1;
      }
   }
   else if (cur_filestatus == FILESTATUS_PANEL)
   {
      if (is_boardoutline(cur_layer))
      {
         cur_symbol_type = GR_CLASS_PANELOUTLINE;
         return 1;
      }

      if (is_layer_compoutline(cur_layer))
      {
         cur_symbol_type = GR_CLASS_BOARDOUTLINE;
         return 1;
      }
   }

   CString symbolType(lp);
   symbolType.MakeUpper();

   if (!STRCMPI(lp, "BD_OUTLINE"))
   {
      cur_symbol_type = GR_CLASS_BOARD_GEOM;
   }
   else if (!STRCMPI(lp, "BD_COMPKEEPOUT"))
   {
      cur_symbol_type = GR_CLASS_PLACEKEEPOUT;
   }
   else if (!STRCMPI(lp, "BD_COMPKEEPIN"))
   {
      cur_symbol_type = GR_CLASS_PLACEKEEPIN;
   }
   else if (!STRCMPI(lp, "BD_TRACEKEEPIN") || !STRCMPI(lp, "BD_TRACEKEEPIN_BD_VIAKEEPIN"))
   {
      cur_symbol_type = GR_CLASS_ROUTKEEPIN;
   }
   else if (!STRCMPI(lp, "BD_TRACEKEEPIN_BD_VIAKEEPIN_BD_COMPKEEPIN"))
   {
      cur_symbol_type = GR_CLASS_ALLKEEPIN;
   }
   else if (!STRCMPI(lp, "BD_TRACEKEEPOUT"))
   {
      cur_symbol_type = GR_CLASS_ROUTKEEPOUT;
   }
   else if (!STRCMPI(lp, "BD_VIAKEEPOUT"))
   {
      cur_symbol_type = GR_CLASS_VIAKEEPOUT;
   }
   else if (symbolType.Find("DIMENSION") >= 0)
   {
      cur_symbol_type = GR_CLASS_DRAWING;
   }
   else if (!STRCMPI(lp, "BD_TRACEKEEPOUT_BD_VIAKEEPOUT_BD_COMPKEEPOUT"))
   {
      cur_symbol_type = GR_CLASS_ALLKEEPOUT;
   }
   else if (!STRCMPI(lp, "OBSTACLE_VIA") || !STRCMPI(lp, "OBSTACLE_VIA_COMP"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = GR_CLASS_VIAKEEPIN;
      else
         cur_symbol_type = GR_CLASS_VIAKEEPOUT;
   }
   else if (!STRCMPI(lp, "TRACE_VIA_RESTRICTION") || !STRCMPI(lp, "TRACE_VIA_COMP_RESTRICTION"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = GR_CLASS_ALLKEEPIN;
      else
         cur_symbol_type = GR_CLASS_ALLKEEPOUT;
   }
   else if (!STRCMPI(lp, "TRACE_RESTRICTION"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = GR_CLASS_ROUTKEEPIN;
      else
         cur_symbol_type = GR_CLASS_ROUTKEEPOUT;
   }
   else if (!STRCMPI(lp, "VIA_RESTRICTION"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = GR_CLASS_VIAKEEPIN;
      else
         cur_symbol_type = GR_CLASS_VIAKEEPOUT;
   }
   else if (!STRCMPI(lp, "ATE_RESTRICTION"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = graphicClassTestPointKeepIn;
      else
         cur_symbol_type = graphicClassTestPointKeepOut;
   }
   else if (!STRCMPI(lp, "OBSTACLE_TRACE_VIA") || !STRCMPI(lp, "OBSTACLE_TRACE_VIA_COMP"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = GR_CLASS_ALLKEEPIN;
      else
         cur_symbol_type = GR_CLASS_ALLKEEPOUT;
   }
   else if (!STRCMPI(lp, "COMPONENT_RESTRICTION") || !STRCMPI(lp, "OBSTACLE_COMPONENT"))
   {
      if (cur_section == SECTION_KEEPIN)
         cur_symbol_type = GR_CLASS_PLACEKEEPIN;
      else
         cur_symbol_type = GR_CLASS_PLACEKEEPOUT;
   }
   else if (!STRCMPI(lp, "BD_COPPER_AREA"))
   {
      // only etch if a netname is associated.
      //cur_symbol_type = GR_CLASS_ETCH;
   }
   else if (!STRCMPI(lp, "RELIEF_LINE"))
   {
      // gets the thermal flag set in CPOLY in THEDA_LINE
   }
   else if (!STRCMPI(lp, "CONDUCTION"))
   {
      // this shows a filled copper area - nothing special. But should not be mixed !
   }
   else
   {
      //fprintf(logFp, "Symbol Type [%s]\n", lp);
   }

   return 1;
}

/******************************************************************************
* theda_symbol_type
*/
static int theda_symbol_type()
{
   if (!get_tok())
      return parsingError();

   char *lp = strtok(token, ";");
   do_symboltype(lp);

   return 1;
}

/******************************************************************************
* theda_filled_areas_set
*/
static int theda_filled_areas_set()
{
   cur_filled_area_set = 0;   // the first is the parent poly, than voids.
   cur_filled = TRUE;   // this is only for current area

   int res = loop_command(tlgraphic_lst, SIZ_TLGRAPHIC_LST, "FILLED_AREAS_SET");

   cur_filled = FALSE;

   return res;
}

/******************************************************************************
* theda_area_pen
*/
static int theda_area_pen()
{
   fnull("AREA_PEN", FALSE);

   return 1;
}

/******************************************************************************
* theda_pen
   Version 1
*/
static int theda_pen()
{
   fnull("PEN", FALSE);

   return 1;
}

/******************************************************************************
* theda_line_pen
*/
static int theda_line_pen()
{
   if (!get_tok())
      return parsingError();

   int err;
   cur_line_pen = token;
   cur_widthindex = Graph_Aperture(token, T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   cur_tracewidthindex = cur_widthindex;  // Version 1

   return 1;
}

/******************************************************************************
* theda_ate_probe
*/
static int theda_ate_probe()
{
   if (!get_tok())
      return parsingError();

   if (!STRCMPI(token, "YES"))
      cur_ate_probe = TRUE;

   return 1;
}

/******************************************************************************
* theda_bareboard_probe
*/
static int theda_bareboard_probe()
{
   if (!get_tok())
      return parsingError();

   if (!STRCMPI(token,"YES"))
      cur_bareboard_probe = TRUE;

   return 1;
}

/******************************************************************************
* theda_diagnostic_probe
*/
static int theda_diagnostic_probe()
{
   if (!get_tok())
      return parsingError();

   if (!STRCMPI(token,"YES"))
      cur_diagnostic_probe = TRUE;

   return 1;
}

/******************************************************************************
* theda_line_default_pen
*/
static int theda_line_default_pen()
{
   if (!get_tok())
      return parsingError();

   int err;
   cur_widthindex = Graph_Aperture(token, T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   cur_defaulttracewidthindex = cur_tracewidthindex = cur_widthindex;   // Version 1

   return 1;
}

/******************************************************************************
* curcircle_reset
*/
static void curcircle_reset()
{
   cur_circle.centerx = 0;
   cur_circle.centery = 0;
   cur_circle.radius = 0;

   return;
}

/******************************************************************************
* theda_circle
*/
static int theda_circle()
{
   cur_filled_area_set++;  // the first is the parent poly, than voids.
   curcircle_reset();

   int res = loop_command(tlcircle_lst, SIZ_TLCIRCLE_LST, "CIRCLE");

   // here do circle
   if (getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_TOT_NPLAT") == 0 ||
       getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_PAR_NPLAT") == 0 ||
       getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_PAR_PLAT") == 0 ||
       getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_TOT_PLAT") == 0 ||
       getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_TOT") == 0)
   {
      int drillindex;
      CString drillname;

      cur_layerindex = getThedaReader().getDefinedLayerIndex("DRILLHOLE");

      if ((drillindex =get_drillindex(cur_circle.radius*2, cur_layerindex)) < 0)
         return 1;

      drillname.Format("DRILL_%d", drillindex);
      Graph_Block_Reference(drillname, NULL, cur_filenum, cur_circle.centerx, cur_circle.centery,
            0.0, 0, 1.0, cur_layerindex, TRUE);
   }
   else
   {
      cur_layerindex = make_layerstring();

      if (cur_section == SECTION_CONDUCTION || cur_section == SECTION_COPPER_AREAS)
      {
         int voidpoly = FALSE;

         if (cur_filled_area_set > 1)
         {
            // knv - 20040830 - all filled areas will not be voids
            // rcf - 20070424 - case dts0100384587 - Apparantly on the date above Kurt commented out
            // the line of code below, that sets voidpoly = TRUE. That was done for old case 919,
            // now in CQ as dts0100376555. That case only talks about rectangles so it is not
            // clear at all why this stuff for circles was changed. TO fix case dts0100384587 this
            // line is reactivated.
            voidpoly = TRUE;
         }
         else
         {
            setCurData(Graph_PolyStruct(cur_layerindex,  0, 0));
            getCurData()->setGraphicClass(cur_symbol_type);

            if (strlen(cur_netname))
            {
               getCurData()->setGraphicClass(GR_CLASS_ETCH);
               doc->SetAttrib(&getCurData()->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                     VT_STRING,  cur_netname.GetBuffer(0),SA_OVERWRITE, NULL);
            }
         }

         Graph_Poly(NULL, cur_widthindex, 1, voidpoly, 1);  // void
         Graph_Vertex(cur_circle.centerx - cur_circle.radius, cur_circle.centery, 1);
         Graph_Vertex(cur_circle.centerx + cur_circle.radius, cur_circle.centery, 1);
         Graph_Vertex(cur_circle.centerx - cur_circle.radius, cur_circle.centery, 0);
      }
      else
      {
         DataStruct *data = Graph_Circle(cur_layerindex,cur_circle.centerx,cur_circle.centery,
                cur_circle.radius, 0L, cur_widthindex , FALSE, FALSE);
         data->setGraphicClass(cur_symbol_type);
      }
   }

   if (cur_section == SECTION_SHAPES || cur_section == SECTION_SYMBOLS)
   {
      TLShapeCircle *c = new TLShapeCircle;
      shapecirclearray.SetAtGrow(shapecirclecnt++, c);
      c->shape_id = cur_blockname;
      c->radius = cur_circle.radius;
   }

   return res;
}

/******************************************************************************
* theda_arc
*/
static int theda_arc()
{
   cur_arc.counterclock = 1;
   cur_arc.smallersemicircle = 1;
   int res = loop_command(tlarc_lst, SIZ_TLARC_LST, "ARC");

   double   cx, cy, da, sa;
   ArcFromStartEndRadius(cur_arc.firstx, cur_arc.firsty, cur_arc.lastx, cur_arc.lasty, cur_arc.radius,
                         (cur_arc.counterclock)?0:1, cur_arc.smallersemicircle, &cx, &cy, &sa, &da);

   cur_layerindex = make_layerstring();

   DataStruct *data = Graph_Arc(cur_layerindex, cx, cy, cur_arc.radius, sa, da,0L, cur_widthindex , FALSE);
   data->setGraphicClass(cur_symbol_type);

   return res;
}

/******************************************************************************
* theda_text
*/
static int theda_text()
{
   // clear current text
   cur_atext.textpen = 0;
   cur_atext.string = "";
   cur_atext.type = "";
   cur_atext.x = 0;
   cur_atext.y = 0;
   cur_atext.rotation = 0;
   cur_atext.mirror = 0;
   cur_atext.setHorizontalAlignment('N');
   cur_atext.setVerticalAlignment('N');
   cur_atext.x_mirror = FALSE;
   cur_atext.h = -1;

   int res = loop_command(tltext_lst, SIZ_TLTEXT_LST, "TEXT");
   // do not use cur_atext.number because it is -1 for local definition

   if (cur_atext.h <0)
   {
      TLTextpen *textPen = FindFirstTextPen();
		// Case 1963, crashed when textPen not defined
      if (textPen != NULL)
		{
			cur_atext.h = textPen->height;
			cur_atext.w = textPen->getCharBoxWidth();
			cur_atext.charw = textPen->width;
		}
   }


   if (cur_atext.h > 0) // no text pen found and no height found
   {
      // here do justification
      int   textjust = 0;

      // here do justification
      if (cur_atext.getHorizontalAlignment() == 'L' || cur_atext.getHorizontalAlignment() == 'N')
         textjust = 0;
      else if (cur_atext.getHorizontalAlignment() == 'M')
         textjust = GRTEXT_W_C;
      else if (cur_atext.getHorizontalAlignment() == 'R')
         textjust = GRTEXT_W_R;

      if (cur_atext.getVerticalAlignment() == 'B' || cur_atext.getVerticalAlignment() == 'N')
         textjust |= 0;
      else if (cur_atext.getVerticalAlignment() == 'M')
         textjust |= GRTEXT_H_C;
      else if (cur_atext.getVerticalAlignment() == 'T')
         textjust |= GRTEXT_H_T;

      double x = cur_atext.x;
      double y = cur_atext.y;
      normalize_text(&x, &y, textjust, cur_atext.rotation, cur_atext.mirror, cur_atext.h, cur_atext.w * strlen(cur_atext.string));

      cur_atext.x = x;
      cur_atext.y = y;

      // here do text
      int prop = TRUE;

      cur_layerindex = make_layerstring();

      if (strlen(cur_atext.string))
      {
         Graph_Text(cur_layerindex, cur_atext.string, cur_atext.x, cur_atext.y, cur_atext.h ,cur_atext.charw,
               DegToRad(cur_atext.rotation), 0L, prop,  cur_atext.mirror, 0, FALSE, -1, 0);
      }
   }

   get_newline();

   return res;
}

/******************************************************************************
* text_nodes_text_node
*/
static int text_nodes_text_node()
{
   // clear current text
   cur_atext.textpen = 0;
   cur_atext.string = "";
   cur_atext.type = "";
   cur_atext.x = 0;
   cur_atext.y = 0;
   cur_atext.rotation = 0;
   cur_atext.mirror = 0;
   cur_atext.setHorizontalAlignment('N');    // Normal, Left, Middle
   cur_atext.setVerticalAlignment('N');    // Normal, Base
   cur_atext.x_mirror = FALSE;
   cur_atext.h = -1;

   get_newline();

   int res = loop_command(tltext_lst, SIZ_TLTEXT_LST, "TEXT_NODE");
   // do not use cur_atext.number because it is -1 for local definition

   if (cur_atext.h <0)
   {
      TLTextpen *textPen = FindFirstTextPen();
      cur_atext.h = textPen->height;
      cur_atext.w = textPen->getCharBoxWidth();
      cur_atext.charw = textPen->width;
   }

   if (cur_atext.x_mirror)
   {
      cur_atext.rotation = 180 + cur_atext.rotation;
      normalizeToPositiveDegrees(cur_atext.rotation);
      cur_atext.mirror = TRUE;
   }

   // here do text
   if (strlen(cur_atext.type) > 0)
   {
      if (cur_atext.type.CompareNoCase("REF_DESIGNATOR") == 0)
         cur_atext.type = ATT_REFNAME;

      cur_layerindex = make_layerstring();
      int attributeLayerIndex = cur_layerindex;

      if (! cur_comp.refdes.IsEmpty())
      {
         if ((cur_comp.mir & MIRROR_LAYERS) != 0)
         {
            LayerStruct* layer = doc->getLayerAt(attributeLayerIndex);

            attributeLayerIndex = layer->getMirroredLayerIndex();
         }
      }

      if (getCurData() != NULL && cur_comp.refdes.Trim() != "")
      {
         doc->SetUnknownVisAttrib(&getCurData()->getAttributesRef(), get_attribmap(cur_atext.type),
               cur_atext.string, cur_atext.x, cur_atext.y, DegToRad(cur_atext.rotation),
               cur_atext.h, cur_atext.w, 1, 0, TRUE, SA_OVERWRITE, 0L,attributeLayerIndex, 0, 
               cur_atext.getHorizontalPosition(), cur_atext.getVerticalPosition());
      }
      else if (curblock != NULL)
      {
         doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), get_attribmap(cur_atext.type),
               cur_atext.string, cur_atext.x, cur_atext.y, DegToRad(cur_atext.rotation),
               cur_atext.h, cur_atext.w, 1, 0, TRUE, SA_OVERWRITE, 0L,attributeLayerIndex, 0, 
               cur_atext.getHorizontalPosition(), cur_atext.getVerticalPosition());
      }
   }

   get_newline();

   return res;
}


/******************************************************************************
* probe_pad_text_node
*/
static int probe_pad_text_node()
{
   // clear current text
   cur_atext.textpen = 0;
   cur_atext.string = "";
   cur_atext.type = "";
   cur_atext.x = 0;
   cur_atext.y = 0;
   cur_atext.rotation = 0;
   cur_atext.mirror = 0;
   cur_atext.setHorizontalAlignment('N');    // Normal, Left, Middle
   cur_atext.setVerticalAlignment('N');    // Normal, Base
   cur_atext.x_mirror = FALSE;
   cur_atext.h = -1;

   get_newline();

   int res = loop_command(tltext_lst, SIZ_TLTEXT_LST, "TEXT_NODE");
   // do not use cur_atext.number because it is -1 for local definition

   if (cur_atext.h <0)
   {
      TLTextpen *textPen = FindFirstTextPen();
      cur_atext.h = textPen->height;
      cur_atext.w = textPen->getCharBoxWidth();
      cur_atext.charw = textPen->width;
   }

   //if (cur_atext.x_mirror)
   //{
   //   cur_atext.rotation = 180 + cur_atext.rotation;
   //   normalizeToPositiveDegrees(cur_atext.rotation);
   //   cur_atext.mirror = TRUE;
   //}

   cur_atext.rotation = fmod(normalizeDegrees(cur_atext.rotation),180.);

   // here do text
   if (strlen(cur_atext.type) > 0)
   {
      cur_layerindex = make_layerstring();

      CAttributes* testProbeAttributes = &(getTestProbeAttributes());

      doc->SetUnknownVisAttrib(&testProbeAttributes,get_attribmap(cur_atext.type),
            cur_atext.string, cur_atext.x, cur_atext.y, DegToRad(cur_atext.rotation),
            cur_atext.h, cur_atext.w, 1, 0, TRUE, SA_OVERWRITE, 0L,cur_layerindex, 0, 
            cur_atext.getHorizontalPosition(), cur_atext.getVerticalPosition());
   }

   get_newline();

   return res;
}

/******************************************************************************
* theda_filled_circle
*/
static int theda_filled_circle()
{
   // cur_circle
   curcircle_reset();
   int res = loop_command(tlcircle_lst, SIZ_TLCIRCLE_LST, "FILLED_CIRCLE");

   cur_layerindex = make_layerstring();

   // here do circle
   if (cur_circle.radius < CIRCLE_2_APERTURE)
   {
      CString  padshapename;
      // create an aperture
      int pindex = get_padformindex(T_ROUND, cur_circle.radius*2, 0.0, 0,0);
      padshapename.Format("PADSHAPE_%d", pindex);
      Graph_Block_Reference(padshapename, NULL, cur_filenum, cur_circle.centerx,cur_circle.centery,
               0.0, 0, 1.0, cur_layerindex, TRUE);
   }
   else
   {
      Graph_Circle(cur_layerindex, cur_circle.centerx, cur_circle.centery,
                cur_circle.radius, 0L, cur_widthindex , FALSE, TRUE);
   }

   if (cur_section == SECTION_SHAPES || cur_section == SECTION_SYMBOLS)
   {
      TLShapeCircle *c = new TLShapeCircle;
      shapecirclearray.SetAtGrow(shapecirclecnt++, c);
      c->shape_id = cur_blockname;
      c->radius = cur_circle.radius;
   }

   return res;
}

/******************************************************************************
* theda_close_contour
*/
static int theda_close_contour()
{
   get_newline();
   cur_closed = TRUE;

   return 1;
}

/******************************************************************************
* theda_open_contour
*/
static int theda_open_contour()
{
   get_newline();
   cur_closed =FALSE;

   return 1;
}

/******************************************************************************
* theda_filled_polygon
*/
static int theda_filled_polygon()
{
   // on keepins, such as board outline, there is no fill
   if (cur_section == SECTION_BOARDOUTLINE)
      cur_filled = FALSE;
   else if (cur_section == SECTION_KEEPIN)
      cur_filled = FALSE;
   else
      cur_filled = TRUE;

   theda_polygon();
   cur_filled = FALSE;

   return 1;
}

/******************************************************************************
* update_filled
*/
static int update_filled(int symbol_type, int filled)
{
   if (cur_section == SECTION_BOARDOUTLINE)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_BOARD_GEOM || symbol_type == GR_CLASS_BOARDOUTLINE)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_ROUTKEEPIN)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_ROUTKEEPOUT)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_VIAKEEPOUT)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_VIAKEEPIN)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_PLACEKEEPOUT)
      filled = FALSE;
   else if (symbol_type == GR_CLASS_PLACEKEEPIN)
      filled = FALSE;

   return filled;
}

/******************************************************************************
* theda_polygon
*/
static int theda_polygon()
{
   CString t = cur_line;
   long flg = 0;

   cur_filled = update_filled(cur_symbol_type, cur_filled);
   cur_layerindex = make_layerstring();
   setCurData(Graph_PolyStruct(cur_layerindex,  flg, 0));
   getCurData()->setGraphicClass(cur_symbol_type);

   if (strlen(cur_netname))
   {
      getCurData()->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&getCurData()->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
   }

   CPoly *cp = Graph_Poly(NULL, cur_widthindex, cur_filled, 0, cur_closed);

   do_vertex(cur_closed);
   setCurData(NULL);
   get_newline();

   return 1;
}

/******************************************************************************
* theda_filled_smooth_polygon

   The scanner is here so much different that a tokenize scanner.
   Look at this stupid structure
*/
static int theda_filled_smooth_polygon()
{
   cur_filled = TRUE;
   theda_smooth_polygon();
   cur_filled = FALSE;

   return 1;
}

/******************************************************************************
* theda_smooth_polygon

   The scanner is here so much different that a tokenize scanner.
   Look at this stupid structure
*/
static int theda_smooth_polygon()
{
   CString t = cur_line;
   long flg = 0;
   TLPoly p;
   int close = FALSE;
   int voidpoly = 0;

   cur_filled_area_set++;  // the first is the parent poly, than voids.
   cur_filled = update_filled(cur_symbol_type, cur_filled);
   cur_layerindex = make_layerstring();

   if ((cur_section == SECTION_CONDUCTION || cur_section == SECTION_COPPER_AREAS) && cur_filled_area_set > 1)
   {
      voidpoly = TRUE;  // can not have a void without parents.
   }
   else
   {
      setCurData(Graph_PolyStruct(cur_layerindex, flg, 0));
      getCurData()->setGraphicClass(cur_symbol_type);

      if (strlen(cur_netname))
      {
         getCurData()->setGraphicClass(GR_CLASS_ETCH);
         doc->SetAttrib(&getCurData()->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
      }
   }

   // filled areas are always drawn with PEN 0
   cur_widthindex = widthindex0;

   // close is updates later
   CPoly *cp = Graph_Poly(NULL, cur_widthindex, cur_filled, voidpoly, 0);

   polycnt = 0;
   get_newline();

   while (TRUE)
   {
      if (!get_tok())
         return parsingError();

      char w[MAX_LINE];
      strcpy(w, cur_line);
      char *lp = strtok(w, " ;\t\n");

      if (lp && !STRNICMP(lp, "..FIN_SMOOTH_POLYGON", 4))  // x y radius
      {
         break;
      }
      else if (lp && !STRCMPI(lp, "X"))   // x y radius
      {
         // do nothing
      }
      else if (lp && !STRCMPI(lp, "LINE_PEN"))
      {
         int err;
         lp = strtok(NULL, " :=\t\n;");
         cur_line_pen = token;
         cur_widthindex = Graph_Aperture(lp, T_ROUND, 0.0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      }
      else if (lp && !STRCMPI(lp, "AREA_PEN"))
      {
         // do nothing
      }
      else if (lp && !STRCMPI(lp, "CLOSE_CONTOUR"))
      {
         close = TRUE;
      }
      else if (lp && !STRCMPI(lp, "OPEN_CONTOUR"))
      {
         close = FALSE;
      }
      else if (lp && lp[0] == '#')
      {
         // comment
      }
      else if (lp && !isalpha(lp[0]))
      {
         // must be a number
         p.x = cnv_unit(lp);
         lp = strtok(NULL, " \t\n");
         p.y = cnv_unit(lp);
         lp = strtok(NULL, " \t\n");

         if (lp && !STRCMPI(lp, "ARC_POINT"))
         {
            p.bulge = 0.0;
            p.arc_point = 1;

            if (polycnt == 0)
            {
               // the first is an arc_point.
               // make a double entry, so that later I can convert it to a corner point.
               polyarray.SetAtGrow(polycnt++, p);  // fill, type
            }
         }
         else if (lp && !STRCMPI(lp, "CORNER"))
         {
            p.bulge = 0.0;
            p.arc_point = 0;
         }
         else
         {
            // here is a radius ???
            p.bulge = cnv_unit(lp); // this is not bulge but radius
            p.arc_point = 2;
         }

         polyarray.SetAtGrow(polycnt++, p);  // fill, type
      }
      else
      {
         fprintf(logFp, " Unknown token [%s] at %ld\n", lp, getInputFileLineNumber());
      }

      get_newline();
   }

   // need to close last
   // filled areas are always drawn with PEN 0
   cur_widthindex = widthindex0;
   cp->setWidthIndex(cur_widthindex);

   // if the first is an arcpoint, it is a double entry. and need to copy the last into the position
   // of the first.
   if (close)
   {
      cp->setClosed(true);
      TLPoly p1 = polyarray.ElementAt(0);
      TLPoly p2 = polyarray.ElementAt(polycnt-1);

      if (p1.arc_point == 1)
      {
         TLPoly p2 = polyarray.ElementAt(polycnt-1);
         polyarray.SetAt(0, p2);
      }
      else
      {
         if (p1.x != p2.x || p1.y != p2.y)
            polyarray.SetAtGrow(polycnt++, p1);
      }
   }

   CPnt *plast;

   for (int i=0; i<polycnt; i++)
   {
      p = polyarray.ElementAt(i);

      if (p.arc_point == 1)   // can not be first or last
      {
         if (i == 0 || i == polycnt - 1)
         {
#ifdef _DEBUG
            fprintf(logFp, "DEBUG: Smooth polygon Arc_Point error at %ld\n",getInputFileLineNumber());
#endif
         }
         else
         {
            double cx, cy, rad, sa, da;
            TLPoly p1 = polyarray.ElementAt(i-1);
            TLPoly p3 = polyarray.ElementAt(i+1);

            // this is an mid point
            ArcPoint3(p1.x, p1.y, p.x, p.y, p3.x, p3.y, &cx, &cy, &rad, &sa, &da);
            plast->bulge = (DbUnit)tan(da/4);

            if (fabs(plast->bulge) > 572) // max bulge <== see graph_vertex message
            {
               if (plast->bulge > 0)
                  plast->bulge =  571;
               else
                  plast->bulge = -571;
            }
         }
      }
      else if (p.arc_point == 2)
      {
         TLPoly p1;

         if (i == 0)
         {
            // Current point is first point, so use the last point as previous point
            p1 = polyarray.ElementAt(polycnt - 1);    // get last point

            // If last point is the same as first point then get next to last point
            if (fabs(p1.x - p.x) < SMALLNUMBER && fabs(p1.y - p.y) < SMALLNUMBER && polycnt >= 2)
               p1 = polyarray.ElementAt(polycnt - 2);
         }
         else
         {
            p1 = polyarray.ElementAt(i-1);
         }

         TLPoly p3;

         if (i < polycnt - 1)
         {
            // current poly is not last poly, so p3 is the next poly
            p3 = polyarray.ElementAt(i+1);
         }
         else
         {
            // current poly is last poly, so p3 to first poly to close the poly struct
            p3 = polyarray.ElementAt(0);

            // If last point is the same as first point then skip it
            if (fabs(p3.x - p.x) < SMALLNUMBER && fabs(p3.y - p.y) < SMALLNUMBER)
            {
					CPnt *firstPnt = cp->getPntList().GetCount()>0?cp->getPntList().GetHead():NULL;
					if (!firstPnt)
						continue;
               plast = Graph_Vertex(firstPnt->x, firstPnt->y, 0.0);
               continue;
            }
         }

         CPnt pnt1;
         CPnt pnt2;

         if (CalculateArcByRadius(&p1, &p, &p3, &pnt1, &pnt2))
         {
            // Graph the two points
            plast = Graph_Vertex(pnt1.x, pnt1.y, pnt1.bulge);
            plast = Graph_Vertex(pnt2.x, pnt2.y, pnt2.bulge);
         }
         else
         {
            // graph the point as a corner
            plast = Graph_Vertex(p.x,p.y,0.0);
         }

         //plast = Graph_Vertex(xA, yA, bulge);
         //plast = Graph_Vertex(xB, yB, 0.0);
      }
      else
      {
         plast = Graph_Vertex(p.x,p.y,p.bulge);
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   setCurData(NULL);
   get_newline();

   return 1;
}

/******************************************************************************
* CalculateArcByRadius
*/
static int CalculateArcByRadius(TLPoly *p1, TLPoly *p, TLPoly *p3, CPnt *pnt1, CPnt *pnt2)
{
   // Get the angle of the intersecting segments
   double sizeA = sqrt((p1->y - p->y) * (p1->y - p->y) + (p1->x - p->x) * (p1->x - p->x));
   double sizeB = sqrt((p->y - p3->y) * (p->y - p3->y) + (p->x - p3->x) * (p->x - p3->x));
   double sizeC = sqrt((p3->y - p1->y) * (p3->y - p1->y) + (p3->x - p1->x) * (p3->x - p1->x));
   double cosX = (sizeA * sizeA + sizeB * sizeB - sizeC * sizeC) / (2 * sizeA * sizeB);
   double angleX = acos(cosX);

   // Get the distance from the intersection of segments to where the starting point of the arc.
   // The distance for the ending point of the arc is the same as the distance for starting point
   double radius = p->bulge;
   double sizeX = fabs(radius / (tan(angleX/2)));

   // return 0 as an error where the angle is so small that the bulge should just be 0
   if (fabs(radius) > 1e10)
      return 0;

   // Get the X and Y of the starting point
   double xA = 0.0;
   double yA = 0.0;
   GetXYofPointOnSegment(p1->x, p1->y, p->x, p->y, sizeX, xA, yA);

   // Get the X and Y of the endping point
   double xB = 0.0;
   double yB = 0.0;
   GetXYofPointOnSegment(p3->x, p3->y, p->x, p->y, sizeX, xB, yB);

   // Get the X and Y of the center point
   double xCenter = 0.0;
   double yCenter = 0.0;
   BOOL foundCenter = FALSE;

   if (fabs(p1->x - p->x) < SMALLNUMBER )
   {
      // Slope of sizeA is undefine
      if (p1->x > p3->x)
         xCenter = xA - radius;
      else
         xCenter = xA + radius;

      yCenter = yA;

      foundCenter = TRUE;
   }
   else if (fabs(p1->y - p->y) < SMALLNUMBER)
   {
      // Slope of sizeA is 0
      xCenter = xA;

      if (p1->y > p3->y)
         yCenter = yA - radius;
      else
         yCenter = yA + radius;

      foundCenter = TRUE;
   }
   else
   {
      //Slope of sizeA is not undefine or zero, so check if it is very close to undefine or zero
      double tmpAngle = RadToDeg(atan((p1->y - p->y) / (p1->x - p->x)));

      while (tmpAngle >= 360)
         tmpAngle -= 360;

      while (tmpAngle < 0)
         tmpAngle += 360;

      if (fabs(90 - tmpAngle) < SMALLANGLE || fabs(270 - tmpAngle) < SMALLANGLE)
      {
         // Slope of sizeA is close to undefine
         if (p1->x > p3->x)
            xCenter = xA - radius;
         else
            xCenter = xA + radius;

         yCenter = yA;

         foundCenter = TRUE;
      }
      else if (tmpAngle < SMALLANGLE || fabs(180 - tmpAngle) < SMALLANGLE || fabs(360 - tmpAngle) < SMALLANGLE)
      {
         // Slop of sizeA is close to zero
         xCenter = xA;

         if (p1->y > p3->y)
            yCenter = yA - radius;
         else
            yCenter = yA + radius;

         foundCenter = TRUE;
      }
   }

   if (!foundCenter && fabs(p3->x - p->x) < SMALLNUMBER)
   {
      // Slope of sizeB is undefine
      if (p1->x > p3->x)
         xCenter = xB + radius;
      else
         xCenter = xB - radius;

      yCenter = yB;

      foundCenter = TRUE;
   }
   else if (!foundCenter && fabs(p3->y - p->y) < SMALLNUMBER)
   {
      // Slope of sizeB is 0
      xCenter = xB;

      if (p1->y > p3->y)
         yCenter = yB + radius;
      else
         yCenter = yB - radius;

      foundCenter = TRUE;
   }
   else if (!foundCenter)
   {
      //Slope of sizeB is not undefine or zero, so check if it is very close to undefine or zero
      double tmpAngle = RadToDeg(atan((p3->y - p->y) / (p3->x - p->x)));

      while (tmpAngle >= 360)
         tmpAngle -= 360;

      while (tmpAngle < 0)
         tmpAngle += 360;

      if (fabs(90 - tmpAngle) < SMALLANGLE || fabs(270 - tmpAngle) < SMALLANGLE)
      {
         // Slope of sizeB is close to undefine
         if (p1->x > p3->x)
            xCenter = xB + radius;
         else
            xCenter = xB - radius;

         yCenter = yA;

         foundCenter = TRUE;
      }
      else if (tmpAngle < SMALLANGLE || fabs(180 - tmpAngle) < SMALLANGLE || fabs(360 - tmpAngle) < SMALLANGLE)
      {
         // Slop of sizeB is close to zero
         xCenter = xB;

         if (p1->y > p3->y)
            yCenter = yB + radius;
         else
            yCenter = yB - radius;

         foundCenter = TRUE;
      }
   }

   if (!foundCenter)
   {
      // Use the equation of a line "Y = MX + b" and solve for b, which is "b = Y - MX"
      // Get X and Y of the center point
      double M1 = -(p1->x - p->x) / (p1->y - p->y);
      double M2 = -(p3->x - p->x) / (p3->y - p->y);
      double b1 = yA - M1 * xA;
      double b2 = yB - M2 * xB;

      // Find the center X by the equation "M1 * X + b1 = M2 * X + b2"
      // solve for X, which is X = (b2 - b1) / (M2 - M1)
      xCenter = (b1 - b2) / (M2 - M1);

      // Find the center Y by the equation "Y = MX + b", using either M1 and b1 or M2 and b2
      yCenter = M1 * xCenter + b1;
   }


   // Get the delta angle
   double rad = 0.0;
   double sa = 0.0;
   double da = 0.0;
   ArcCenter2(xA, yA, xB, yB, xCenter, yCenter, &rad, &sa, &da, TRUE);

   // Get the bulge
   double bulge = 0.0;

   if (fabs(da) > PI)
      bulge = tan((PI2 + da)/4);
   else
      bulge = tan(da/4);

   pnt1->x = (DbUnit)xA;
   pnt1->y = (DbUnit)yA;
   pnt1->bulge = (DbUnit)bulge;

   pnt2->x = (DbUnit)xB;
   pnt2->y = (DbUnit)yB;
   pnt2->bulge = 0.0;

   return 1;
}

/******************************************************************************
* GetXYofPointOnSegment
*/
static int GetXYofPointOnSegment(double x1, double y1, double x2, double y2, double distanceFromPoint2, double &newX, double &newY)
{
   newX = x2;
   newY = y2;
   double xDistance = 0.0;
   double yDistance = 0.0;

   if (fabs(x1 - x2) < SMALLNUMBER)
   {
      // Slope of sizeA is undefined
      xDistance = 0.0;
      yDistance = distanceFromPoint2;
   }
   else if (fabs(y1 - y2) < SMALLNUMBER)
   {
      // Slope of sizeA is 0
      xDistance = distanceFromPoint2;
      yDistance = 0.0;
   }
   else
   {
      //Slope of sizeA is not undefine or zero, so check if it is very close to undefine or zero
      double tmpArcTan = atan((y1 - y2) / (x1 - x2));
      double tmpAngle = RadToDeg(tmpArcTan);

      while (tmpAngle >= 360)
         tmpAngle -= 360;

      while (tmpAngle < 0)
         tmpAngle += 360;

      if (fabs(90 - tmpAngle) < SMALLANGLE || fabs(270 - tmpAngle) < SMALLANGLE)
      {
         // Slope of sizeA is close to undefine
         xDistance = 0.0;
         yDistance = distanceFromPoint2;
      }
      else if (tmpAngle < SMALLANGLE || fabs(180 - tmpAngle) < SMALLANGLE || fabs(360 - tmpAngle) < SMALLANGLE)
      {
         // Slop of sizeA is close to zero
         xDistance = distanceFromPoint2;
         yDistance = 0.0;
      }
      else
      {
         xDistance = cos(tmpArcTan) * distanceFromPoint2;
         yDistance = fabs(sin(tmpArcTan) * distanceFromPoint2);
      }
   }


   // Add the distance of X and Y to X and Y points
   if (x1 > x2)
      newX = newX + xDistance;
   else
      newX = newX - xDistance;

   if (y1 > y2)
      newY = newY + yDistance;
   else
      newY = newY - yDistance;

   return 1;
}

/******************************************************************************
* already_comppin
*/
static int already_comppin(FileStruct *fl, const char *net_name, const char *comp, const char *pin)
{
   POSITION netPos = fl->getNetList().GetHeadPosition();

   while (netPos)
   {
      NetStruct *net = fl->getNetList().GetNext(netPos);
      if (net->getNetName().Compare(net_name))
         continue;

      POSITION compPinPos = net->getHeadCompPinPosition();

      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (compPin->getRefDes().Compare(comp) == 0 && compPin->getPinName().Compare(pin) == 0)
             return 1;
      }
   }

   return 0;
}

/******************************************************************************
* old_net_nets_net

   The scanner is here so much different that a tokenize scanner.
   Look at this stupid structure
*/
static int old_net_nets_net()
{
   NetStruct *n = NULL;
   cur_netname = "";
   CString t = cur_line;
   int netlist_cont = FALSE;

   get_newline();

   while (TRUE)
   {
      if (!get_tok())
         return parsingError();

      char w[MAX_LINE];
      strcpy(w,cur_line);
      char *lp = strtok(w, " ;\t\n");

      if (lp && !STRNICMP(lp, "..FIN_NET",4))
      {
         cur_netname = "";
         break;
      }
      else if (lp && !strcmp(lp, "ROUTING_RULES"))
      {
         fnull("ROUTING_RULES", FALSE);
      }
      else if (lp && !strcmp(lp, "NET_RULES"))
      {
         fnull("NET_RULES", FALSE);
      }
      else if (lp && !strcmp(lp, "IDENTIFIER"))
      {
         lp = strtok(NULL, " :=;\t\n'");
         cur_netname = lp;
         n = add_net(file,cur_netname);
      }
      else if (lp && !strcmp(lp, "NUMBER"))
      {
         lp = strtok(NULL, " :=;\t\n'");
      }
      else if (lp && !strcmp(lp, "TYPE"))
      {
         lp = strtok(NULL, " :=;\t\n'");

         if (!STRCMPI(lp, "UNCONNECTED"))
         {
            cur_netname = NET_UNUSED_PINS;
            n = add_net(file ,cur_netname);
            n->setFlagBits(NETFLAG_UNUSEDNET);
         }
      }
      else if (lp && !strcmp(lp, "UNCONNECTED"))
      {
         cur_netname = NET_UNUSED_PINS;
         n = add_net(file, cur_netname);
         n->setFlagBits(NETFLAG_UNUSEDNET);
      }
      else if (lp && !strcmp(lp, "NO_POWER_GROUND"))
      {
         // do nothing
      }
      else if (lp && !strcmp(lp, "POWER_LAYER"))
      {
         // do nothing
      }
      else if (lp && !strcmp(lp, "VOLTAGE"))
      {
         // do nothing
      }
      else if (lp && !strcmp(lp, "PRIORITY_LEVEL"))
      {
         // do nothing
      }
      else if (lp && !strcmp(lp, "MERGED"))
      {
         // do nothing
      }
      else if (lp && !strcmp(lp, "POWER_GROUND"))
      {
         doc->SetAttrib(&n->getAttributesRef(),doc->IsKeyWord(ATT_POWERNET, 0), VT_NONE,NULL, SA_OVERWRITE, NULL);
      }
      else if (lp && !strcmp(lp, "REFERENCE_DESIGNATOR"))
      {
         netlist_cont = TRUE;
      }
      else if (lp && !strcmp(lp, "PROBE_REF_DES"))
      {
         if (BAREBOARD_PROBES == 0)
            netlist_cont = FALSE;
         else
            netlist_cont = TRUE;
      }
      else if (lp && lp[0] == '#')
      {
         // comment
      }
      else if (lp && lp[0] == '\'')
      {
         // kill quotes around the name

         if (netlist_cont)
         {
            if (lp[0] == '\'')
               ++lp;

            if (lp[strlen(lp)-1] == '\'')
               lp[strlen(lp)-1] = '\0';

            CString refdes = lp;

            if (lp = strtok(NULL, " :=;\t\n'"))
            {
               CString pinname = lp;

               if (!already_comppin(file,cur_netname, refdes, pinname))
               {
                  if (!comp_pulled_pin(refdes, pinname))
                  {
                     getThedaReader().getThedaComponentPins().addComponentPin(n->getNetName(),refdes,pinname);
                     //add_comppin(file, n, refdes, pinname);
                  }
               }
            }
         }
      }
      else
      {
         // version 1.1 has no quotes around the names !
         CString t = cur_line;

         if (t.Right(1) != ';')
         {
            if (tlversion.software_revision == 1.1)
            {
               if (netlist_cont)
               {
                  CString refdes = lp;

                  if (lp = strtok(NULL, " :=;\t\n'"))
                  {
                     CString pinname = lp;

                     if (!already_comppin(file, cur_netname, refdes, pinname))
                     {
                        if (!comp_pulled_pin(refdes, pinname))
                        {
                           getThedaReader().getThedaComponentPins().addComponentPin(n->getNetName(),refdes,pinname);
                           //add_comppin(file, n, refdes, pinname);
                        }
                     }
                  }
               }  // if netlist_cont
            }
         }
         else
         {
            // unknown line
            fprintf(logFp, "Unknown TL command [%s] at %ld\n", lp, getInputFileLineNumber());
            display_error++;
         }
      }

      get_newline();
   }

   get_newline();
   cur_netname = "";

   return 1;
}

/******************************************************************************
* net_routing_rules
*/
static int net_routing_rules()
{
   fnull("ROUTING_RULES", FALSE);
   return 1;
}

/******************************************************************************
* net_probe_ref_des
   The scanner is here so much different that a tokenize scanner.
   Look at this stupid structure
*/
static int net_probe_ref_des()
{
   get_newline();

   if (BAREBOARD_PROBES == FALSE)
   {
      while (TRUE)
      {
         if (!get_tok())
            return parsingError();

         char w[MAX_LINE];
         strcpy(w, cur_line);
         char *lp = strtok(w, " ;\t\n");

         if (lp && !STRNICMP(lp, "..FIN_NET", 4))
         {
            push_tok();
            cur_netname = "";
            break;
         }

         get_newline();
      }

      return 1;
   }

   // need to add other condition here
   // Reads/parses the first and second columns of a table
   while (TRUE)
   {
      if (!get_tok())
         return parsingError();

      char w[MAX_LINE];
      strcpy(w, cur_line);
      char *lp = strtok(w, " ;\t\n'");

      if (lp && !STRNICMP(lp, "#-------", 5))
      {
         get_newline();

         if (!get_tok())
            return parsingError();

         strcpy(w, cur_line);
         lp = strtok(w, " ;\t\n'");
      }

      if (lp && !STRNICMP(lp, "..FIN_NET",4))
      {
         push_tok();
         cur_netname = "";
         break;
      }

      CString ProbeRef = lp;
      CString ProbeType;

      if ((lp = strtok(NULL, "[()] \t,;")) != NULL)
	   {
         ProbeType = lp;
	   }
      else
      {
		  ProbeType = "Generic";
	   }

	  CString ProbeName;

	  ProbeName = "Theda " + ProbeType + " Probe";

      double pSize = probeSize * Units_Factor(probeSizeUnit, pageUnits);
      double dSize = drillSize * Units_Factor(drillSizeUnit, pageUnits);
      BlockStruct *probeGeom = generate_TestProbeGeometry(doc, ProbeName, pSize, "Test Drill TL", dSize);

      DataStruct *probeData = Graph_Block_Reference(probeGeom->getName(), ProbeRef, 0, 0.0, 0.0, 0, 0, 1, 0, TRUE);
      probeData->getInsert()->setInsertType(insertTypeTestProbe);

      getThedaReader().getThedaTestProbes().addTestProbe(ProbeRef,probeData);

      doc->SetUnknownAttrib(&probeData->getAttributesRef(), get_attribmap("Probe Type"), ProbeType, SA_OVERWRITE, NULL);
      doc->SetAttrib(&probeData->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);

      get_newline();
   }

   return 1;
}

/******************************************************************************
* generateProbeReference
*/
static DataStruct *generateProbeReference(CCEtoODBDoc& camCadDoc, FileStruct* file, const CString probeRefDes,
                                          const CString& pType, const CString& netName)
{
   DataStruct* probeData = datastruct_from_refdes(&camCadDoc, file->getBlock(), probeRefDes);

   if (probeData == NULL)
   {
	  CString ProbeName;

	  if (pType.IsEmpty() == true)
	  {
		  ProbeName = "Theda Generic Probe";
	  }
     else
     {
		  ProbeName = "Theda " + pType + " Probe";
	  }

	  ProbeName = "Theda " + pType + " Probe";

      double pSize = probeSize * Units_Factor(probeSizeUnit, pageUnits);
      double dSize = drillSize * Units_Factor(drillSizeUnit, pageUnits);
      BlockStruct* probeGeom = generate_TestProbeGeometry(&camCadDoc, ProbeName, pSize, "Test Drill TL", dSize);

      probeData = Graph_Block_Reference(probeGeom->getName(), probeRefDes, 0, 0.0, 0.0, 0, 0, 1, 0, TRUE);
      probeData->getInsert()->setInsertType(insertTypeTestProbe);

      getThedaReader().getThedaTestProbes().addTestProbe(probeRefDes,probeData);

      camCadDoc.SetUnknownAttrib(&probeData->getAttributesRef(), get_attribmap("Probe Type"), pType, SA_OVERWRITE, NULL);
      camCadDoc.SetAttrib(&probeData->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, (void*)(const char*)netName, SA_OVERWRITE, NULL);
   }

   return probeData;
}

/******************************************************************************
* net_nets_net
*/
static int net_nets_net()
{
   cur_netname = "";
   netTableDetected = false;

   get_newline();
   cur_netname = "";
   int res = loop_command(tlnet_lst, SIZ_TLNET_LST, "NET");

   get_newline();
   cur_netname = "";
   return 1;
}

/******************************************************************************
* net_parseTable
*/
int net_parseTable()
{
   delete tableParser;
   tableParser = new CThedaNetListTableParser();

   return 1;
}

/******************************************************************************
* get_shapecircle
*/
static int get_shapecircle(const char *t)
{
   for (int i=0; i<shapecirclecnt; i++)
   {
      if (shapecirclearray[i]->shape_id.Compare(t) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* do_shape_id_inserttype
*/
static int do_shape_id_inserttype()
{
   int gclass = 0;

   if (getThedaReader().getCurrentSymbolTypeString().CompareNoCase("CLEAR_PAD") == 0)
   {
      gclass = INSERTTYPE_CLEARPAD;
   }
   else if (getThedaReader().getCurrentSymbolTypeString().CompareNoCase("RELIEF_PAD") == 0)
   {
      gclass = INSERTTYPE_RELIEFPAD;
   }
   else if (getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRILL_SYMBOL") == 0)
   {
      gclass = INSERTTYPE_DRILLSYMBOL;
   }
   else if (STRNICMP(getThedaReader().getCurrentSymbolTypeString(), "OBSTACLE", 8) == 0)
   {
      gclass = INSERTTYPE_OBSTACLE;
   }
   else
   {
      switch (cur_symbol_type)
      {
      case GR_CLASS_ROUTKEEPIN:
      case GR_CLASS_ROUTKEEPOUT:
      case GR_CLASS_PLACEKEEPIN:
      case GR_CLASS_PLACEKEEPOUT:
      case GR_CLASS_VIAKEEPIN:
      case GR_CLASS_VIAKEEPOUT:
      case GR_CLASS_ALLKEEPIN:
      case GR_CLASS_ALLKEEPOUT:
         gclass = INSERTTYPE_OBSTACLE;
         break;
      default:
         gclass = 0;
         break;
      }
   }

   return gclass;
}

/******************************************************************************
* ignore
*/
static int ignore(const char *s, int typ, int bareboard, int diagnostic, int ate)
{
   for (int i=0; i<ignorecnt; i++)
   {
      TLIGNORE *ii = ignorearray[i];   // debig only

      if (ignorearray[i]->name.CompareNoCase(s) == 0 && ignorearray[i]->type == typ)
      {
         return i+1;
      }

      if (bareboard)
      {
         if (ignorearray[i]->name.CompareNoCase("BAREBOARD_PROBE") == 0 && ignorearray[i]->type == typ)
         {
            return i+1;
         }
      }

      if (diagnostic)
      {
         if (ignorearray[i]->name.CompareNoCase("DIAGNOSTIC_PROBE") == 0 && ignorearray[i]->type == typ)
         {
            return i+1;
         }
      }

      if (ate)
      {
         if (ignorearray[i]->name.CompareNoCase("ATE_PROBE") == 0 && ignorearray[i]->type == typ)
         {
            return i+1;
         }
      }
   }

   return FALSE;
}

/******************************************************************************
* theda_shape_id
*/
static int theda_shape_id()
{
   if (!get_tok())
      return parsingError();

   CString shapeID = QShapePrefix;
   shapeID += token;

// here check the ignore type
// use cur_sym_type  = this is the original symbol type.
   if (!ignore(cur_sym_type, cur_section, cur_bareboard_probe, cur_diagnostic_probe, cur_ate_probe) &&
       !ignore(cur_layer, cur_section, 0 , 0 , 0))
   {
      if (cur_technique == ' ' || cur_technique == design_technique)
      {
         if (getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_TOT_NPLAT") == 0 ||
             getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_TOT_PLAT") == 0 ||
             getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_PAR_PLAT") == 0 ||
             getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_PAR_NPLAT") == 0 ||
             getThedaReader().getCurrentSymbolTypeString().CompareNoCase("DRL_TOT") == 0)
         {
            // hierachical drill tool
            int w = get_shapecircle(shapeID);

            if (w > -1)
            {
               CString drillname;
               int drillindex;
               cur_layerindex = getThedaReader().getDefinedLayerIndex("DRILLHOLE");

               if ((drillindex =get_drillindex(shapecirclearray[w]->radius*2, cur_layerindex)) < 0)
                  return 1;

               drillname.Format("DRILL_%d", drillindex);
               Graph_Block_Reference(drillname, NULL, cur_filenum, cur_offset_x, cur_offset_y, 0.0, 0, 1.0, cur_layerindex, TRUE);
            }
            else
            {
               fprintf(logFp, "Drill hole found [%s] at %ld, which is not round -> skipped!\n", shapeID, getInputFileLineNumber());
               display_error++;
            }
         }
         else
         {
            cur_layerindex = make_layerstring();
            // make sure it's there
            getThedaReader().graphBlockOn(shapeID, cur_filenum,blockTypeUnknown);
            Graph_Block_Off();

            int mir = 0;

            if (cur_x_mirror)
            {
               mir = 1;
               cur_rotation -= 180;
            }

            normalizeToPositiveDegrees(cur_rotation);

            DataStruct *data = Graph_Block_Reference(shapeID, NULL, cur_filenum, cur_offset_x, cur_offset_y,
                  DegToRad(cur_rotation), mir, 1.0, cur_layerindex, TRUE);
            data->getInsert()->setInsertType(do_shape_id_inserttype());
         }
      }
      else
      {
         // ignored because of non matching technique
         cur_ignore_shape_id = TRUE;
      }
   } // ignore
   else
   {
      // ignore because of theda.in
      cur_ignore_shape_id = TRUE;
   }

   // offset and rotation only good for the next shape_id or symbol_id
   cur_offset_x = cur_offset_y = 0;
   cur_rotation = 0;

   get_newline();

   return 1;
}

/******************************************************************************
* theda_padstack_id
*/
static int theda_padstack_id()
{
   if (!get_tok())
      return parsingError();

   // make sure it's there
   CString padstack_id = getThedaReader().getPadStackTemplateGeometryName(token);
   cur_layerindex = make_layerstring();

   getThedaReader().graphBlockOn(padstack_id, cur_filenum,blockTypeUnknown);
   Graph_Block_Off();

   int mirror = 0;
   double angle = cur_rotation;

   if (cur_x_mirror != 0)
   {
      mirror = MIRROR_ALL;
      angle  = cur_rotation + 180.;
   }

   DataStruct *data = Graph_Block_Reference(padstack_id, NULL , cur_filenum, cur_offset_x, cur_offset_y,
      DegToRad(angle),mirror, 1.0, -1, TRUE);

   get_newline();

   return 1;
}

/******************************************************************************
* theda_offset
*/
static int theda_offset()
{
   if (!get_tok())
      return parsingError();  // x

   cur_offset_x = cnv_unit(token);

   if (!get_tok())
      return parsingError();  // y

   cur_offset_y = cnv_unit(token);

   get_newline();

   return 1;
}

/******************************************************************************
* theda_rotation
*/
static int theda_rotation()
{
   if (!get_tok())
      return parsingError();  // x

   cur_rotation = atof(token);

   get_newline();

   return 1;
}

/******************************************************************************
* do_vertex
*/
static int do_vertex(int closed)
{
   int first = TRUE;
   double firstx, firsty, x,y;

   CString t = "";

   while (TRUE)
   {
      t += cur_line;
      t.TrimLeft();
      t.TrimRight();

      if (t.Right(1) == ";")
         break;

      // if not end of line
      get_newline();
      get_tok(); // this make the cur_line update
   }

   char *tmpline = STRDUP(t);
   char *lp;
   lp = strtok(tmpline,"[");

   while (TRUE)
   {
      if ((lp = strtok(NULL, "[()] \t,;")) == NULL)
         break;

      x = cnv_unit(lp);

      if ((lp = strtok(NULL, "[()] \t,;")) == NULL)
         break;

      y = cnv_unit(lp);

      Graph_Vertex(x, y, 0.0);

      if (first)
      {
         firstx = x;
         firsty = y;
      }

      first = FALSE;
   }

   free(tmpline);

   if (!first)
   {
      if (closed && (firstx !=x || firsty != y))
         Graph_Vertex(firstx, firsty, 0.0);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_line()
{
   CString  t;

   cur_layerindex = make_layerstring();

   setCurData(Graph_PolyStruct(cur_layerindex,0, 0));
   getCurData()->setGraphicClass(cur_symbol_type);

   if (strlen(cur_netname))
   {
      getCurData()->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&getCurData()->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
         VT_STRING,cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
   }

   CPoly *cp = Graph_Poly(NULL,  cur_widthindex, 0, 0, 0);

   if (cur_sym_type.Compare("RELIEF_LINE") == 0)
      cp->setThermalLine(true);

   do_vertex(0);

   if (strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      doc->SetUnknownAttrib(&getCurData()->getAttributesRef(),get_attribmap(getThedaReader().getCurrentSymbolTypeString()), "",SA_OVERWRITE, NULL); // x, y, rot, height
   }

   get_newline();

   setCurData(NULL);

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_cspline()
{
   CString t;

   fprintf(logFp, "CSPLINE not implemented at %ld\n", getInputFileLineNumber());
   display_error++;

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t = cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_rectangle()
{
   double   x1,y1, x2, y2;

   cur_layerindex = make_layerstring();

   setCurData(Graph_PolyStruct(cur_layerindex,  0, 0));
   getCurData()->setGraphicClass(cur_symbol_type);

   bool filledFlag = (cur_filled != 0);

   Graph_Poly(NULL,cur_widthindex,filledFlag, 0, 1);

   if (!get_tok())   return parsingError();  // x

   x1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   y1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   x2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   y2 = cnv_unit(token);

   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x1,y2,0.0);
   Graph_Vertex(x1,y1,0.0);

   if (strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      doc->SetUnknownAttrib(&getCurData()->getAttributesRef(),get_attribmap(getThedaReader().getCurrentSymbolTypeString()),"",SA_OVERWRITE, NULL); // x, y, rot, height
   }

   get_newline();

   setCurData(NULL);

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_parallelogram()
{
   CString  t = cur_line;
   double   x1,y1, x2, y2, x3, y3;

   cur_layerindex = make_layerstring();

   setCurData(Graph_PolyStruct(cur_layerindex,  0, 0));
   Graph_Poly(NULL,cur_widthindex, 0, 0, 1);
   getCurData()->setGraphicClass(cur_symbol_type);

   if (!get_tok())   return parsingError();  // x

   x1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   y1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   x2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   y2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   x3 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   y3 = cnv_unit(token);

   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x3,y3,0.0);

   if (strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      doc->SetUnknownAttrib(&getCurData()->getAttributesRef(),get_attribmap(getThedaReader().getCurrentSymbolTypeString()), "",SA_OVERWRITE, NULL); // x, y, rot, height
   }

   get_newline();

   setCurData(NULL);

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_triangle()
{
   CString  t = cur_line;
   double   x1,y1, x2, y2, x3, y3;

   cur_layerindex = make_layerstring();

   setCurData(Graph_PolyStruct(cur_layerindex,  0, 0));
   getCurData()->setGraphicClass(cur_symbol_type);


   Graph_Poly(NULL,cur_widthindex, 0, 0, 1);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x3 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y3 = cnv_unit(token);

   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x3,y3,0.0);
   Graph_Vertex(x1,y1,0.0);

   if (strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      doc->SetUnknownAttrib(&getCurData()->getAttributesRef(),get_attribmap(getThedaReader().getCurrentSymbolTypeString()), "",
             SA_OVERWRITE, NULL); //
   }

   get_newline();

   setCurData(NULL);

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_filled_triangle()
{
   CString  t = cur_line;
   double   x1,y1, x2, y2, x3, y3;

   cur_filled = TRUE;
   cur_filled = update_filled(cur_symbol_type, cur_filled);

   cur_layerindex = make_layerstring();

   setCurData(Graph_PolyStruct(cur_layerindex,  0, 0));
   Graph_Poly(NULL,cur_widthindex, cur_filled, 0, 1);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x3 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y3 = cnv_unit(token);

   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x3,y3,0.0);
   Graph_Vertex(x1,y1,0.0);

   if (strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      doc->SetUnknownAttrib(&getCurData()->getAttributesRef(),get_attribmap(getThedaReader().getCurrentSymbolTypeString()),"",
             SA_OVERWRITE, NULL); //
   }

   get_newline();

   setCurData(NULL);

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_filled_rectangle()
{
   CString  t = cur_line;
   double   x1, y1, x2, y2;
   double   radius_rectangle;

   cur_filled = TRUE; // preset for filled rectangle

   if (cur_section == SECTION_KEEPIN)
   {
      cur_filled = FALSE;
   }
   else
   {
      // this takes cur_filled and masked it if needed.
      cur_filled = update_filled(cur_symbol_type, cur_filled);
   }

   cur_layerindex = make_layerstring();

   if (!get_tok())   return parsingError();  // x

   t = token;
   x1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y1 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   t = token;
   x2 = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   t = token;
   y2 = cnv_unit(token);
   
   // finding the radius of a rectangle which is half of the diagonal 
   radius_rectangle = sqrt((((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2))))/2;

    // here do rectangle
   if (radius_rectangle < CIRCLE_2_APERTURE && cur_filled)   
   {
      CString  padshapename;
      // create an aperture     
      int pindex = get_padformindex(T_RECTANGLE, abs(x1-x2), abs(y1-y2), 0,0);
      padshapename.Format("PADSHAPE_%d", pindex);
      setCurData(Graph_Block_Reference(padshapename, NULL, cur_filenum, (x1+x2)/2.0,(y1+y2)/2.0,
               0.0, 0, 1.0, cur_layerindex, TRUE));
   }
   else
   {
      setCurData(Graph_PolyStruct(cur_layerindex, 0, 0));
      getCurData()->setGraphicClass(cur_symbol_type);    

   // filled areas are always drawn with PEN 0
      cur_widthindex = widthindex0;

      Graph_Poly(NULL,  cur_widthindex, cur_filled, 0, 1);   

      Graph_Vertex(x1,y1,0.0);
      Graph_Vertex(x2,y1,0.0);
      Graph_Vertex(x2,y2,0.0);
      Graph_Vertex(x1,y2,0.0);
      Graph_Vertex(x1,y1,0.0);
   }
   
   if (strlen(getThedaReader().getCurrentSymbolTypeString()))
   {
      doc->SetUnknownAttrib(&getCurData()->getAttributesRef(),get_attribmap(getThedaReader().getCurrentSymbolTypeString()), "",SA_OVERWRITE, NULL); // x, y, rot, height
   }
   
   get_newline();

   setCurData(NULL);
      
   return 1;      
}

/****************************************************************************/
/*
*/
static int theda_point()
{
   CString  t = cur_line;

   //fprintf(logFp, "POINT not implemented at %ld\n", getInputFileLineNumber());
   //display_error++;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int design_objects_pens()
{
   int   res = loop_command(pens_lst,SIZ_PENS_LST, "PENS");

   return res;
}

/****************************************************************************/
/*
*/
static int design_objects_shapes()
{
   int   res = loop_command(shapes_lst,SIZ_SHAPES_LST, "SHAPES");

   return res;
}

/****************************************************************************/
/*
*/
static int design_objects_symbols()
{
   cur_section = SECTION_SYMBOLS;

   int   res = loop_command(symbols_lst,SIZ_SYMBOLS_LST, "SYMBOLS");
   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
*/
static int library_symbols()
{
   cur_section = SECTION_SYMBOLS;

   int   res = loop_command(symbols_1_lst,SIZ_SYMBOLS_1_LST, "SYMBOLS");
   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
*/
static int is_smdtype(const char *s)
{
   int   i;

   for (i=0;i<smdtypecnt;i++)
   {
      if (!STRCMPI(smdtype[i],s))
         return 1;
   }

   return 0;
}

/******************************************************************************
* packages_package
*/
static int packages_package()
{
   cur_blockname = cur_line;

   getThedaReader().setConstructingComponentGeometryFlag(true);

   //curblock = Graph_Block_On(GBO_APPEND,t, cur_filenum,0);   // start shape here, but it gets
   // renamed with identifier
   cur_section = SECTION_PACKAGE;
   cur_package_type = "";
   cur_number_of_pins = 0;
   pinnamecnt = 0;

   get_newline();
   clear_graphicsettings();
   int res = loop_command(tlpackage_lst,SIZ_TLPACKAGE_LST, "PACKAGE");

   for (int i=0;i<pinnamecnt;i++)
   {
      delete pinnamearray[i];
   }

   pinnamecnt = 0;

   if (curblock && strlen(cur_package_type))
   {
      if (is_smdtype(cur_package_type))
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, 0),
            VT_NONE,NULL,SA_OVERWRITE, NULL); // x, y, rot, height
      }
      else
      {
         doc->SetUnknownAttrib(&curblock->getAttributesRef(),get_attribmap("THEDA_PACKAGE_TYPE"),
            cur_package_type,SA_OVERWRITE, NULL); // x, y, rot, height
      }
   }

   if (curblock)
      Graph_Block_Off();

   get_newline();
   curblock = NULL;
   cur_section = 0;

   getThedaReader().setConstructingComponentGeometryFlag(false);

   return res;
}

/****************************************************************************/
/*
*/
static int padstacks_padstack()
{
   cur_blockname = cur_line;

   getThedaReader().setConstructingPadstackGeometryFlag(true);

   curblock = NULL;
   //curblock = Graph_Block_On(GBO_APPEND,t, cur_filenum,0);   // start shape here, but it gets
   // the curblock is opened with padstack_identifier

   get_newline();

   clear_graphicsettings();
   int res = loop_command(tlpadstack_lst,SIZ_TLPADSTACK_LST, "PADSTACK");

   if (curblock)
   {
      Graph_Block_Off();

      //restructurePadStack(*doc,curblock,padStackBlockName,cur_blockname);
   }

   get_newline();
   curblock = NULL;

   getThedaReader().setConstructingPadstackGeometryFlag(false);

   return res;
}

/****************************************************************************/
/*
*/
static int padstacks_1_padstack()
{
   curblock = NULL;

   get_newline();

   while (TRUE)
   {
      char  w[MAX_LINE];
      char  *lp;

      if (!get_tok())
      {
         return parsingError();
      }

      strcpy(w,cur_line);
      lp = strtok(w," :=;\t\n");

      if (lp && !STRNICMP(lp,"..FIN_PADSTACK",4))  // x y radius
      {
         cur_section = 0;
         break;
      }
      else if (lp && lp[0] == '#')
      {
         // do nothing
      }
      else if (lp && !STRCMPI(lp,"TECHNIQUE"))
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp,"IDENTIFIER"))
      {
         lp = strtok(NULL," :=;\t\n");
         cur_blockname = getThedaReader().getPadStackTemplateGeometryName(lp);
         //padStackBlockName = "tmp" + cur_blockname;

         curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypePadstack));
         //curblock = Graph_Block_On(GBO_APPEND,cur_blockname,cur_filenum,0);
         //curblock->setBlockType(BLOCKTYPE_PADSTACK);

         curblock->setOriginalName(lp);
         cur_section = SECTION_PADSTACKGRAPHIC;

         //curblock->name = t;   // just change name here
      }
      else if (lp && !STRICMP(lp,"GRAPHICS"))
      {
         //cur_section = SECTION_PADSTACKGRAPHIC;
         loop_command(graphics_lst,SIZ_GRAPHICS_LST, "GRAPHICS");
         //cur_section = 0;
      }
      else
      {
         clear_graphicsettings();

         CString  symtyp;
         // here define padstack
         // first lp is  technique
         lp = strtok(NULL," :=;\t\n");    // layer
         cur_layer = lp;
         lp = strtok(NULL," :=;\t\n");    // place
         cur_graphic_place = "";

         lp = strtok(NULL," :=;\t\n");    // contact
         lp = strtok(NULL," :=;\t\n");    // need
         lp = strtok(NULL," :=;\t\n");    // symboltype
         symtyp = lp;
         do_symboltype(lp);
         lp = strtok(NULL," :=;\t\n");    // symbol

         //if (cur_technique == ' ' || cur_technique == design_technique)
         if (lp)
         {
            if (!ignore(symtyp, cur_section, 0, 0, 0) && !ignore(cur_layer, cur_section, 0 , 0 , 0))
            {
               // make sure it's there
               getThedaReader().graphBlockOn(lp,cur_filenum,blockTypeUnknown);
               Graph_Block_Off();

               if (symtyp.CompareNoCase("DRL_TOT_NPLAT") == 0 ||
                   symtyp.CompareNoCase("DRL_TOT_PLAT") == 0 ||
                   symtyp.CompareNoCase("DRL_PAR_PLAT") == 0 ||
                   symtyp.CompareNoCase("DRL_PAR_NPLAT") == 0 ||
                   symtyp.CompareNoCase("DRL_TOT") == 0)
               {
                  // hierachical drill tool
                  int  w = get_shapecircle(lp);

                  if (w > -1)
                  {
                     CString  drillname;
                     int      drillindex;
                     cur_layerindex = getThedaReader().getDefinedLayerIndex("DRILLHOLE");

                     if ((drillindex =get_drillindex(shapecirclearray[w]->radius*2, cur_layerindex)) < 0)
                     {
                        continue;
                     }

                     drillname.Format("DRILL_%d",drillindex);

                     Graph_Block_Reference(drillname, NULL, cur_filenum,
                               cur_circle.centerx, cur_circle.centery, 0.0,
                               0, 1.0, cur_layerindex, TRUE);
                  }
                  else
                  {
                     fprintf(logFp, "Drill hole found [%s] at %ld, which is not round -> skipped!\n", lp, getInputFileLineNumber());
                     display_error++;
                  }
               }
               else
               {
                  CString  pshapename, pxshapename;
                  cur_layerindex = make_layerstring();

                  pshapename.Format("PADSHAPE_%s",lp);
                  BlockStruct *b = Graph_Block_Exists(doc, pshapename, -1);

                  if (b == NULL)
                  {
                     b = &(getThedaReader().graphBlockOn(pshapename,cur_filenum,blockTypeUnknown));
                     b->setBlockType(BLOCKTYPE_PADSHAPE);
                     b->setOriginalName(lp);
                     Graph_Block_Reference(lp, NULL , cur_filenum, 0.0, 0.0, 0.0, 0, 1.0, -1, TRUE);
                     Graph_Block_Off();
                  }

                  pxshapename.Format("COMPLEX_%s", lp);

                  if ((b = Graph_Block_Exists(doc, pxshapename, cur_filenum)) == NULL)
                  {
                     Graph_Complex(pxshapename, 0, pshapename, 0.0, 0.0, 0.0);
                  }

                  DataStruct *d = Graph_Block_Reference(pxshapename, NULL, cur_filenum, 0.0, 0.0, 0.0, 0 , 1.0, cur_layerindex, TRUE);
                  // needs cur_sym_tpye_string and cur_symbol_type
                  d->getInsert()->setInsertType(do_shape_id_inserttype());
               }
            }  // ignore symbol_type
         }
      }

      get_newline();
   }

   if (curblock)
   {
      Graph_Block_Off();

      //restructurePadStack(*doc,curblock,padStackBlockName,cur_blockname);
   }

   get_newline();
   curblock = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int package_pin_names()
{
   get_newline();

   while (TRUE)
   {
      char  w[MAX_LINE];
      char  *lp;

      if (!get_tok())   return parsingError();

      strcpy(w,cur_line);
      lp = strtok(w," :=;\t\n");

      if (lp && !STRNICMP(lp,"..FIN_PIN_NAMES",4)) //
      {
         break;
      }
      else if (lp && lp[0] == '#')
      {
         // do nothing
      }
      else
      {
         CString  pinnum, pinname;
         // here define padstack
         // first lp is  technique
         pinnum = lp;
         lp = strtok(NULL," :=;\t\n");    // pin_id
         pinname = lp;

         if (pinnum.CompareNoCase("PIN_NUMBER") == 0)
         {
            // do nothing
         }
         else if (pinname.CompareNoCase("PIN_ID") == 0)
         {
            //
         }
         else
         {
            // here load pinname array
            TLPinname   *c = new TLPinname;
            pinnamearray.SetAtGrow(pinnamecnt,c);
            pinnamecnt++;
            c->number = pinnum;
            c->identifier = pinname;
         }
      }

      get_newline();
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int components_component()
{
   get_newline();

   TLDeviceComp   *c = new TLDeviceComp;
   devicecomparray.SetAtGrow(devicecompcnt,c);
   int res = loop_command(tlcomponent_lst,SIZ_TLCOMPONENT_LST, "COMPONENT");

   TypeStruct *t = AddType(file,devicecomparray[devicecompcnt]->identifier); // changed from device_id. this is not unique
   BlockStruct *b = &(getThedaReader().graphBlockOn(devicecomparray[devicecompcnt]->package_id,cur_filenum,blockTypeUnknown));
   Graph_Block_Off();

   if (t->getBlockNumber() == -1)
   {
      t->setBlockNumber( b->getBlockNumber());
   }
   else if (t->getBlockNumber() != b->getBlockNumber())
   {
      fprintf(logFp, "Device [%s] has already a different Package [%s] assigned!\n", t->getName(), b->getName());
      display_error++;
   }
   devicecompcnt++;

   get_newline();

   return res;
}

/****************************************************************************/
/*
*/
static int pens_line_pens()
{
   int   err;
   int   res =0;

   while (TRUE)
   {
      char  w[MAX_LINE];
      char  *lp;

      if (!get_tok())   return parsingError();

      strcpy(w,cur_line);
      lp = strtok(w," ;\t\n");

      if (lp && !STRNICMP(lp,"..FIN_LINE_PENS",4))
      {
         break;
      }
      else if (lp && isdigit(lp[0]))
      {
         double width;
         CString  name = lp;
         lp = strtok(NULL," ;\t\n");   // width
         width = cnv_unit(lp);
         lp = strtok(NULL," ;\t\n");   // form
         int form = T_ROUND;

         if (!STRCMPI(lp,"SQUARE"))
            form = T_SQUARE;

         if (width > 0)
            Graph_Aperture(name, form, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      }
   }

   return res;
}

/****************************************************************************/
/*
*/
static int pens_text_pens()
{
   int res = loop_command(text_pens_lst,SIZ_TEXT_PENS_LST, "TEXT_PENS");

   return res;
}

/****************************************************************************/
/*
*/
static int shapes_shape()
{
   cur_blockname = cur_line;
   cur_section = SECTION_SHAPES;

   // this can not be done because the shapename gets called with shape_id before and
   // than the syntetic name does not find the double entry.

   // curblock = Graph_Block_On(GBO_APPEND,t,cur_filenum,0);   // start shape here, but it gets
      // renamed with identifier
   curblock = NULL;

   get_newline();
   clear_graphicsettings();
   int res = loop_command(tlshape_lst,SIZ_TLSHAPE_LST, "SHAPE");

   if (curblock)
      Graph_Block_Off();

   get_newline();
   curblock = NULL;
   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
   this looks to be the same as shapes.
*/
static int symbols_symbol()
{
   cur_blockname = cur_line;

   // this can not be done because the shapename gets called with shape_id before and
   // than the syntetic name does not find the double entry.

   // curblock = Graph_Block_On(GBO_APPEND,t,cur_filenum,0);   // start shape here, but it gets
      // renamed with identifier
   curblock = NULL;

   get_newline();
   clear_graphicsettings();
   int res = loop_command(tlshape_lst,SIZ_TLSHAPE_LST, "SYMBOL");

   if (curblock)
      Graph_Block_Off();

   get_newline();
   curblock = NULL;

   return res;
}
/****************************************************************************/
/*
*/
static int pc_boards_pc_board()
{
   CString  board_name;

   get_newline();
   cur_board.x = 0;
   cur_board.y = 0;
   cur_rotation = 0;
   cur_boardname = "";

   int res = loop_command(pc_board_lst,SIZ_PC_BOARD_LST, "PC_BOARD");

   BlockStruct *b = NULL;

   if (strlen(cur_boardname) == 0)  // if no identifier found, take the first one.
   {
      FileStruct *f;
      POSITION pos = doc->getFileList().GetHeadPosition();

      while (pos != NULL)
      {
         f = doc->getFileList().GetNext(pos);

         if (f->getBlockType()  == BLOCKTYPE_PCB)
         {
            cur_boardname = f->getBlock()->getName();
            b = f->getBlock();

            break;
         }
      }
   }
   else
      b = doc->Find_Block_by_Name(cur_boardname, -1);

   if (b)
   {
      int pcb_filenum = b->getFileNumber();
      DataStruct *d = Graph_Block_Reference(cur_boardname, NULL, pcb_filenum, cur_board.x, cur_board.y,
         DegToRad(cur_rotation),0,1.0,-1,FALSE);
         d->getInsert()->setInsertType(insertTypePcb);
   }
   else
   {
      CString  tmp;
      tmp.Format("Can not find a loaded PCB design [%s]!\nFind any loaded Board?", cur_boardname);

      if (cur_find_any_board || ErrorMessage(tmp, "THEDA Panel Error", MB_YESNO | MB_DEFBUTTON2)==IDYES)
      {
         FileStruct *f;
         POSITION pos = doc->getFileList().GetHeadPosition();

         while (pos != NULL)
         {
            f = doc->getFileList().GetNext(pos);

            if (f->getBlockType() == BLOCKTYPE_PCB)
            {
               cur_boardname = f->getBlock()->getName();
               b = f->getBlock();
               int pcb_filenum = b->getFileNumber();
               DataStruct *d = Graph_Block_Reference(cur_boardname, NULL, pcb_filenum, cur_board.x, cur_board.y,
                  DegToRad(cur_rotation),0,1.0,-1,FALSE);
               d->getInsert()->setInsertType(insertTypePcb);

               break;
            }
         }

         cur_find_any_board = TRUE;
      }
   }

   get_newline();

   return res;
}

/****************************************************************************/
/*
   this looks to be the same as shapes.
*/
static int symbols_1_symbol()
{
   cur_blockname = cur_line;

   // this can not be done because the shapename gets called with shape_id before and
   // than the syntetic name does not find the double entry.

   // curblock = Graph_Block_On(GBO_APPEND,t,cur_filenum,0);   // start shape here, but it gets
   // renamed with identifier
   curblock = NULL;

   get_newline();
   clear_graphicsettings();

   int res = loop_command(tlgraphic_lst,SIZ_TLGRAPHIC_LST, "SYMBOL");

   if (curblock)
      Graph_Block_Off();

   get_newline();
   curblock = NULL;

   return res;
}

/****************************************************************************/
/*
*/
static int package_drawings()
{
   if (curblock == NULL)
   {
      fprintf(logFp, "PACKAGE geometries without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   return loop_command(drawings_lst,SIZ_DRAWINGS_LST, "DRAWINGS");
}

/****************************************************************************/
/*
*/
static int package_text_nodes()
{
   if (curblock == NULL)
   {
      fprintf(logFp, "PACKAGE geometries without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   return loop_command(text_nodes_lst,SIZ_TEXT_NODES_LST, "TEXT_NODES");
}

/****************************************************************************/
/*
*/
static int comp_package_text_nodes()
{
   return loop_command(text_nodes_lst,SIZ_TEXT_NODES_LST, "TEXT_NODES");
}

/****************************************************************************/
/*
*/
static int package_pins()
{
   if (curblock == NULL)
   {
      fprintf(logFp, "PACKAGE pins without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   return loop_command(pins_lst,SIZ_PINS_LST, "PINS");
}

/****************************************************************************/
/*
*/
static int package_properties()
{
   if (curblock == NULL)
   {
      fprintf(logFp, "PACKAGE properties without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   return loop_command(properties_lst,SIZ_PROPERTIES_LST, "PROPERTIES");
}

/****************************************************************************/
/*
*/
static int component_properties()
{
   comp_prop = TRUE;
   int res = loop_command(properties_lst,SIZ_PROPERTIES_LST, "PROPERTIES");
   comp_prop = FALSE;
   return res;
}

/****************************************************************************/
/*
*/
static const char *get_pinname(const char *pn)
{
   int   i;

   for (i=0;i<pinnamecnt;i++)
   {
      if (pinnamearray[i]->number.Compare(pn) == 0)
         return pinnamearray[i]->identifier.GetBuffer(0);
   }

   return pn;
}

/****************************************************************************/
/*
*/
static int package_terminal_areas()
{

   if (curblock == NULL)
   {
      fprintf(logFp, "PACKAGE pins without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   get_newline();

   while (TRUE)
   {
      char  w[MAX_LINE];
      char  *lp;

      if (!get_tok())   return parsingError();

      strcpy(w,cur_line);
      lp = strtok(w," ;\t\n");

      if (lp && !STRNICMP(lp,"..FIN_TERMINAL_AREAS",4))  // x y radius
      {
         break;
      }
      else if (lp && lp[0] == '#')
      {
         // do nothing
      }
      else if (lp && !STRICMP(lp,"PIN_NUMBER"))
      {
         // do nothing
      }
      else
      {
         // here place pin

         // first is pinnumber
         cur_pin.identifier = get_pinname(lp);
         cur_pin.number = lp;

         if (lp = strtok(NULL," \t"))  // x
         {
            cur_pin.x = cnv_unit(lp);
         }

         if (lp = strtok(NULL," \t"))  // y
         {
            cur_pin.y = cnv_unit(lp);
         }

         if (lp = strtok(NULL," '\t")) // padstackid
         {
            cur_pin.padstack_id = getThedaReader().getPadStackTemplateGeometryName(lp);
         }

         if (lp = strtok(NULL," \t"))  // mirror
         {

         }

         if (lp = strtok(NULL," \t"))  // rotation
         {
            cur_pin.rotation = atof(lp);
         }


         DataStruct *d = Graph_Block_Reference(cur_pin.padstack_id, cur_pin.identifier, cur_filenum,
            cur_pin.x, cur_pin.y,
            DegToRad(cur_pin.rotation), 0 , 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypePin);

         // do pinnr here
         if (strlen(cur_pin.number))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_COMPPINNR, cur_pin.number,
             SA_OVERWRITE, NULL); //
         }
      }

      get_newline();
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int comp_package_pins()
{
   int res = loop_command(comp_package_pins_lst,SIZ_COMP_PACKAGE_PINS_LST, "PINS");

   return res;
}

/****************************************************************************/
/*
*/
static int comp_package_drawings()
{
   fnull("DRAWINGS", FALSE);

   return 1;
}

/****************************************************************************/
/*
   Local component pins, not evaluated.
*/
static int phys_comp_pins()
{
   if (BAREBOARD_PROBES == FALSE)
   {
      fnull("PINS", FALSE);

      return 1;
   }

   char  delim[20];
   char  tmp[MAX_LINE];
   char  *lp;
   strcpy(delim," ;\t\n:=',");
   get_newline();

   if (!get_tok())   return parsingError();

   strcpy(tmp,cur_line);
   lp = strtok(tmp,delim);

   while (lp && !STRICMP(lp,"..FIN_PINS"))
   {
      if (lp && !STRICMP(lp,"LOCATION"))
      {
         if (!get_tok())   return parsingError();  // x

         PhysPinX = cnv_unit(token);

         if (!get_tok())   return parsingError();  // y

         PhysPinY = cnv_unit(token);
      }
      else if (lp && !STRICMP(lp,"PROBE_PADS"))
      {
         probes_probe();
      }

      get_newline();

      if (!get_tok())   return parsingError();

      strcpy(tmp,cur_line);
      lp = strtok(tmp,delim);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_graphics()
{
   if (curblock == NULL)
   {
      fprintf(logFp, "PADSTACK geometries without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   cur_section = SECTION_PADSTACKGRAPHIC;
   int res = loop_command(graphics_lst,SIZ_GRAPHICS_LST, "GRAPHICS");
   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
*/
static int shape_identifier()
{
   BlockStruct *b = curblock;

   if (!get_tok())
      return parsingError();  // x

   cur_blockname = QShapePrefix;   // attach the prefix "$SHP_" to make the name unique
   cur_blockname += token;

   curblock = &(getThedaReader().graphBlockOn(cur_blockname, cur_filenum,blockTypeUnknown));
   //curblock->original_name = curblock->name;
   //curblock->name = t;   // just change name here
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pc_board_id()
{
   if (!get_tok())   return parsingError();  // x

   cur_boardname = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int symbol_identifier()
{
   BlockStruct *b = curblock;

   if (!get_tok())
      return parsingError();

   // attach the prefix "$SHP_" to make the name unique, symbol_identifier is the same as shape_identifier
   cur_blockname = QShapePrefix;
   cur_blockname += token;

   curblock = &(getThedaReader().graphBlockOn(cur_blockname, cur_filenum,blockTypeUnknown));
   //curblock->original_name = curblock->name;
   //curblock->name = t;   // just change name here
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int package_identifier()
{
   BlockStruct *b = curblock;

   if (!get_tok())
      return parsingError();

   cur_blockname = getThedaReader().getComponentTemplateGeometryName(token);

   curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypePcbComponent));
   //curblock = Graph_Block_On(GBO_APPEND,cur_blockname,cur_filenum,0);
   //curblock->setBlockType(BLOCKTYPE_PCBCOMPONENT);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int package_number_of_pins()
{
   if (!get_tok())   return parsingError();  // x
   cur_number_of_pins = atoi(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int package_type()
{
   if (!get_tok())   return parsingError();  //
   cur_package_type = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
   // there can be same name SHAPES, PADSTACKS etc...

*/
static int padstack_identifier()
{
   BlockStruct *b = curblock;

   if (!get_tok())
      return parsingError();

   cur_blockname = getThedaReader().getPadStackTemplateGeometryName(token);
   //padStackBlockName = "tmp" + cur_blockname;

   curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypePadstack));
   //curblock = Graph_Block_On(GBO_APPEND,cur_blockname,cur_filenum,0);
   //curblock->setBlockType(BLOCKTYPE_PADSTACK);
   // completed in padstacks_padstack()

   curblock->setOriginalName(token);
   //curblock->name = t;   // just change name here
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int shape_geometries()
{
   if (curblock == NULL)
   {
      // big bad error.
      fprintf(logFp, "SHAPE geometries without an IDENTIFIER at %ld\n", getInputFileLineNumber());
      curblock = &(getThedaReader().graphBlockOn(cur_blockname,cur_filenum,blockTypeUnknown));
   }

   get_newline();
   clear_graphicsettings();
   int res = loop_command(tlgraphic_lst,SIZ_TLGRAPHIC_LST, "GEOMETRIES");
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int shape_properties()
{
   fnull("PROPERTIES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_properties()
{
   fnull("PROPERTIES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int design_objects_padstacks()
{
   int   res = loop_command(padstacks_lst,SIZ_PADSTACKS_LST, "PADSTACKS");

   //This is commented out because the padstack is created incorrectly when this function is called
   //OptimizePadstacks(doc, pageUnits, convert_complex_power); // this can create unused blocks

   return res;
}

/****************************************************************************/
/*
*/
static int library_padstacks()
{
   int   res = loop_command(padstacks_1_lst,SIZ_PADSTACKS_1_LST, "PADSTACKS");

   //This is commented out because the padstack is created incorrectly when this function is called
   //OptimizePadstacks(doc, pageUnits, convert_complex_power); // this can create unused blocks

   return res;
}

/****************************************************************************/
/*
*/
static int design_objects_packages()
{
   int   res = loop_command(packages_lst,SIZ_PACKAGES_LST, "PACKAGES");

   return res;
}

/****************************************************************************/
/*
*/
static int design_objects_devices()
{
   fnull("DEVICES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int design_objects_components()
{
   int   res = loop_command(components_lst,SIZ_COMPONENTS_LST, "COMPONENTS");

   return res;
}

/****************************************************************************/
/*
*/
static int component_identifier()
{
   if (!get_tok())
      return parsingError();

   devicecomparray[devicecompcnt]->identifier = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_unconnected()
{
   cur_netname = NET_UNUSED_PINS;
   NetStruct *n = add_net(file,cur_netname);
   n->setFlagBits(NETFLAG_UNUSEDNET);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_identifier()
{
   if (!get_tok())
      return parsingError();

   cur_netname = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
int net_tableSeparator()
{
   netTableDetected = true;

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
int net_tableHeader()
{
   get_newline();

   int referenceDesignatorColumn = -1;
   int pinIdColumn               = -1;
   int seqNoColumn               = -1;
   int schSymbolIdColumn         = -1;
   int schPinIdColumn            = -1;
   int probeRefDesColumn         = -1;
   int probeColumn               = -1;

   for (int columnIndex = 0;get_tok();columnIndex++)
   {
      if (tokenIs("REFERENCE_DESIGNATOR"))
      {
         referenceDesignatorColumn = columnIndex;
      }
      else if (tokenIs("PIN_ID"))
      {
         pinIdColumn = columnIndex;
      }
      else if (tokenIs("SEQ_NO"))
      {
         seqNoColumn = columnIndex;
      }
      else if (tokenIs("SCH_SYMBOL_ID"))
      {
         schSymbolIdColumn = columnIndex;
      }
      else if (tokenIs("SCH_PIN_ID"))
      {
         schPinIdColumn = columnIndex;
      }
      else if (tokenIs("PROBE_REF_DES"))
      {
         probeRefDesColumn = columnIndex;
      }
      else if (tokenIs("PROBE"))
      {
         probeColumn = columnIndex;
      }
      else if (tokenIs("#------"))
      {
         break;
      }
   }

   get_newline();

   for (int columnIndex = 0;get_tok();columnIndex++)
   {
      if (tokenIs("#------"))
      {
         get_newline();
         break;
      }

      if (columnIndex == referenceDesignatorColumn)
      {
      }
      else if (columnIndex == pinIdColumn)
      {
      }
      else if (columnIndex == seqNoColumn)
      {
      }
      else if (columnIndex == schSymbolIdColumn)
      {
      }
      else if (columnIndex == schPinIdColumn)
      {
      }
      else if (columnIndex == probeRefDesColumn)
      {
      }
      else if (columnIndex == probeColumn)
      {
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int component_package_id()
{
   if (!get_tok())
      return parsingError();  // x

   devicecomparray[devicecompcnt]->package_id = getThedaReader().getComponentTemplateGeometryName(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int component_device_id()
{
   if (!get_tok())   return parsingError();  // x

   devicecomparray[devicecompcnt]->device_id = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int design_objects_templates()
{
   fnull("TEMPLATES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int design_objects_rules()
{
   fnull("RULES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_reference()
{
   if (!get_tok())   return parsingError();  // x

   netlistcomparray[netlistcompcnt]->reference = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_number()
{
   if (!get_tok())   return parsingError();  // x

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comppin_reference()
{
   if (!get_tok())   return parsingError();  // x

   cur_comppin.compname = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comppin_pin_id()
{
   if (!get_tok())   return parsingError();  // x

   cur_comppin.pinname = token;
   get_newline();

   return 1;
}

/******************************************************************************
* get_comptype
*/
static int get_comptype(const char *t)
{
   if (!STRCMPI(t, "ONSCREEN"))
   {
      return ATT_DEVICETYPE_CONNECTOR;
   }
   else if (!STRCMPI(t, "NORMAL_COMPONENT"))
   {
      return ATT_DEVICETYPE_UNKNOWN;
   }
   else if (!STRCMPI(t, "CONNECTOR"))
   {
      return ATT_DEVICETYPE_CONNECTOR;
   }
   else if (!STRCMPI(t, "JUMPER"))
   {
      return ATT_DEVICETYPE_JUMPER;
   }
   else
   {
      fprintf(logFp, "ATT_DEVICETYPE %s not implemented\n", t);
   }

   return ATT_DEVICETYPE_UNKNOWN;
}

/****************************************************************************/
/*
*/
static int net_comp_type()
{
   CString  t;

   if (!get_tok())   return parsingError();  // x

   t = strtok(token,";");
   netlistcomparray[netlistcompcnt]->comptype = get_comptype(t);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_lib_id()
{
   if (!get_tok())   return parsingError();  // x

   netlistcomparray[netlistcompcnt]->library_id = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_package_id()
{
   if (!get_tok())
      return parsingError();  // x

   netlistcomparray[netlistcompcnt]->package_id = getThedaReader().getComponentTemplateGeometryName(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_pin_id()
{
   if (!get_tok())   return parsingError();  //

   if (!STRCMPI(token, "PHYSICAL"))
      netlistcomparray[netlistcompcnt]->pin_ids_physical = TRUE;

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_comp_rules()
{
   fnull("COMPONENT_RULES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_insertion_rules()
{
   fnull("INSERTION_RULES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_properties()
{
   fnull("PROPERTIES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_reference()
{
   if (!get_tok())   return parsingError();  // x

   cur_comp.refdes = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_number()
{
   if (!get_tok())   return parsingError();  // x

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_placed()
{
   cur_comp.placed = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_fixed()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_unplaced()
{
   cur_comp.placed = FALSE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_layer()
{
   if (!get_tok())   return parsingError();  //

   cur_comp.setComponentLayerName(token);
   getThedaReader().setComponentPlacementLayerName(token);

   if (getThedaReader().isSolderSideName(cur_comp.getComponentLayerName()))
   {
      cur_comp.mir |= MIRROR_LAYERS;
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_placement_layer()
{
   if (!get_tok())   return parsingError();  //

   if (STRCMPI(token,"PLACED_CS"))
      cur_comp.mir |= MIRROR_LAYERS;

   get_newline();

   return 1;
}

/****************************************************************************/
/*
   ON_GRID
   OFF_GRID_MIXED
*/
static int phys_comp_placement_grid()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comppin_probe_pad()
{
   if (BAREBOARD_PROBES == FALSE)
   {
      fnull("PROBE_PAD", FALSE);
   }
   else
   {
      if (!get_tok())
         return parsingError();

      CString probeName = token;

      if (!get_tok())
         return parsingError();

      CString probeType = token;

      generateProbeReference(*doc, file, probeName, probeType, cur_netname);

      get_newline();
   }

   return 1;
}
/****************************************************************************/
/*
*/
static int net_comppin_schematic_sym_pin()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int Copy_Geom_Non_Pin(CCamCadData& camCadData,BlockStruct *toblock, BlockStruct *fromblock)
{
   DataStruct *np;
   POSITION pos;

   pos = fromblock->getDataList().GetHeadPosition();

   while (pos != NULL)
   {
      np = fromblock->getDataList().GetNext(pos);

      // do not copy pins and PIN_WIRE attributes
      if (np->getDataType() == T_INSERT)
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;

         if (is_attvalue(doc, np->getAttributesRef(), "PIN_WIRE", 0))      continue;

         if (is_attvalue(doc, np->getAttributesRef(), "CHIP_BOND_LAND", 0))continue;
      }

      DataStruct *newData = camCadData.getNewDataStruct(*np);
      toblock->getDataList().AddTail(newData);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_package()  // needed for CompPinLoc
{
   CCamCadData& camCadData = doc->getCamCadData();

   // This place_comp() call was removed to implement case #1805
   //// here place component
   //place_comp();
   //s_pinNameToContactLayerMap.empty();

   int netlistptr = get_netlistcomp_ptr(cur_comp.refdes);
   
   TLNetlistComp* netListComp = netlistcomparray[netlistptr];
   cur_comp.setNetlistComp(netListComp);

   int devptr = get_devicecomp_from_library_id(netListComp->library_id);

   netListComp->emptyPinNameToContactLayerMap();

   if (devptr < 0)
   {
      fprintf(logFp, "Component Library_Identifier [%s] for component [%s] not found at %ld.\n",
            netListComp->library_id, cur_comp.refdes, getInputFileLineNumber());
      display_error++;

      return loop_command(comp_package_lst,SIZ_COMP_PACKAGE_LST, "PACKAGE");
   }

   int res;

   // only do it if the component is placed
   if (cur_comp.placed && netListComp->pin_ids_physical && use_pin_ids_physical)
   {
      BlockStruct *b = doc->Find_Block_by_Name(devicecomparray[devptr]->package_id, cur_filenum);
      cur_blockname.Format("%s_%s", b->getName(), cur_comp.refdes);
      curblock = &(getThedaReader().graphBlockOn(cur_blockname, cur_filenum,blockTypeUnknown));
      curblock->setBlockType(b->getBlockType());

      // copy the old geom contents and attributes, but not pins and wires.
      Copy_Geom_Non_Pin(camCadData,curblock, b);
      doc->CopyAttribs(&curblock->getAttributesRef(), b->getAttributesRef());

      // on pulled pins, I can not write any pin contents
      res = loop_command(comp_package1_lst,SIZ_COMP_PACKAGE1_LST, "PACKAGE");
      Graph_Block_Off();

      cur_comp.setInstanceSpecificGeometry(curblock);

      //DataStruct* netListCompData = netListComp->getComponentData();

      //if (netListCompData != NULL)
      //{
      //   netListCompData->getInsert()->setBlockNumber(curblock->getBlockNumber());
      //}
   }
   else
   {
      res = loop_command(comp_package_lst,SIZ_COMP_PACKAGE_LST, "PACKAGE");
   }

   //getThedaReader().updateRectifiedPackageId();

   return res;
}

/****************************************************************************/
/*
*/
static int phys_comp_location() // needed for CompPinLoc
{
   if (!get_tok())   return parsingError();  // x

   cur_comp.x = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   cur_comp.y = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pc_board_location()  //
{
   if (!get_tok())   return parsingError();  // x

   cur_board.x = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   cur_board.y = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_rotation() // needed for CompPinLoc
{
   if (!get_tok())   return parsingError();  // x

   cur_comp.rot = atof(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_nets_merged()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_x_mirror() // needed for CompPinLoc
{
   cur_comp.x_mirror = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_x_mirror() //
{
   cur_via.x_mirror = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int text_x_mirror()
{
   cur_atext.x_mirror = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_comp_y_mirror()  // needed for CompPinLoc
{
   cur_comp.y_mirror = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int circle_center()
{
   if (!get_tok())   return parsingError();  // x

   cur_circle.centerx = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   cur_circle.centery = cnv_unit(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_first_point()
{
   if (!get_tok())   return parsingError();  // x

   cur_arc.firstx = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   cur_arc.firsty = cnv_unit(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_last_point()
{
   if (!get_tok())   return parsingError();  // x

   cur_arc.lastx = cnv_unit(token);

   if (!get_tok())   return parsingError();  // y

   cur_arc.lasty = cnv_unit(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int circle_radius()
{
   if (!get_tok())   return parsingError();  // x

   cur_circle.radius = cnv_unit(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_radius()
{
   if (!get_tok())   return parsingError();  // x

   cur_arc.radius = cnv_unit(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_smaller_semi()
{
   cur_arc.smallersemicircle = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_larger_semi()
{
   cur_arc.smallersemicircle = FALSE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_counter_clockwise()
{
   cur_arc.counterclock = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int arc_clockwise()
{
   cur_arc.counterclock = FALSE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_signal_identifier()
{
   if (!get_tok())
      return parsingError();  // x

   cur_netname = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_signal_number()
{
   if (!get_tok())   return parsingError();  // x

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_signal_traces()
{
   get_newline();
   clear_graphicsettings();
   cur_widthindex = cur_tracewidthindex; // Version 1
   cur_filled = FALSE;
   cur_closed = FALSE;
   int res  = loop_command(tlgraphic_lst,SIZ_TLGRAPHIC_LST, "TRACES");
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_signal_polygon()
{
   cur_filled = FALSE;
   cur_closed = FALSE;
   theda_polygon();

   return 1;
}

/****************************************************************************/
/*
*/
static int phys_signal_copper_areas()
{
   int   res = loop_command(phys_copper_areas_lst,SIZ_PHYS_COPPER_AREAS_LST, "COPPER_AREAS");

   return res;
}

/****************************************************************************/
/*
*/
static int phys_copper_area()
{
   cur_section = SECTION_COPPER_AREAS;
   cur_filled_area_set = 0;   // the first is the parent poly, than voids.
   get_newline();
   clear_graphicsettings();

   int res  = loop_command(tlgraphic_lst,SIZ_TLGRAPHIC_LST, "COPPER_AREA");

   get_newline();

   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
*/
static int conductions_conduction()
{
   cur_section = SECTION_CONDUCTION;

   get_newline();
   clear_graphicsettings();
   cur_filled_area_set = 0;   // the first is the parent poly, than voids.
   int res  = loop_command(tlgraphic_lst,SIZ_TLGRAPHIC_LST, "CONDUCTION");
   get_newline();

   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
*/
static int conductions_inverted_conduction()
{
   cur_section = SECTION_CONDUCTION; // kevin says these are the same as conductions.

   get_newline();
   clear_graphicsettings();
   cur_filled_area_set = 0;   // the first is the parent poly, than voids.
   int res  = loop_command(tlgraphic_lst,SIZ_TLGRAPHIC_LST, "INVERTED_CONDUCTION");
   get_newline();

   cur_section = 0;

   return res;
}

/****************************************************************************/
/*
*/
static int phys_signal_vias()
{
   int res  = loop_command(vias_lst,SIZ_VIAS_LST, "VIAS");

   return res;
}

/****************************************************************************/
/*
*/
static int phys_signal_test_points()
{
   int res  = loop_command(test_points_lst,SIZ_TEST_POINTS_LST, "TEST_POINTS");

   return res;
}

/****************************************************************************/
/*
*/
static int net_pins()
{
   int res  = loop_command(net_pins_lst,SIZ_NET_PINS_LST, "PINS");

   return res;
}

/****************************************************************************/
/*
*/
static int phys_signal_conductions()
{
   int res  = loop_command(conductions_lst,SIZ_CONDUCTIONS_LST, "CONDUCTIONS");

   return res;
}

/****************************************************************************/
/*
*/
static int phys_signal_inverted_conductions()
{
   int res  = loop_command(inverted_conductions_lst,SIZ_INVERTED_CONDUCTIONS_LST, "INVERTED_CONDUCTIONS");

   return res;
}

/****************************************************************************/
/*
*/
static int phys_signal_incomplete_traces()
{
   fnull("INCOMPLETE_TRACES", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_identifier()
{
   if (!get_tok())
      return parsingError();

   cur_pin.identifier = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_x_mirror()
{
   cur_pin.x_mirror = TRUE;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_number()
{
   if (!get_tok())
      return parsingError();

   cur_pin.number = strtok(token,";\n");
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int prop_identifier()
{
   if (!get_tok())
      return parsingError();

   cur_prop.identifier = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int prop_description()
{
   if (!get_tok())
      return parsingError();

   cur_prop.description = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int prop_number()
{
   if (!get_tok())
      return parsingError();

   cur_prop.number = strtok(token,";\n");
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_number()
{
   if (!get_tok())
      return parsingError();

   cur_via.number = strtok(token,";\n");
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_location()
{
   if (!get_tok())
      return parsingError();

   cur_pin.x = cnv_unit(token);

   if (!get_tok())
      return parsingError();

   cur_pin.y = cnv_unit(token);

   //getThedaReader().setTestProbeLocation(cur_pin.x,cur_pin.y);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_rotation()
{
   if (!get_tok())
      return parsingError();

   cur_pin.rotation = atof(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_mark()
{
   if (!get_tok())
      return parsingError();

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_padstack_id()
{
   if (!get_tok())
      return parsingError();

   cur_pin.padstack_id = getThedaReader().getPadStackTemplateGeometryName(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_number()
{
   if (!get_tok())
      return parsingError();

   textpenarray[textpencnt]->number = atoi(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
ATTRIBUTES := may span multiple lines until a ;
*/
static int theda_attributes()
{
   CString t;

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t = cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_font()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_height()
{
   if (!get_tok())   return parsingError();  // x

   textpenarray[textpencnt]->height = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_width()
{
   if (!get_tok())   return parsingError();  //

   textpenarray[textpencnt]->width = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_slant()
{
   if (!get_tok())   return parsingError();  //

   textpenarray[textpencnt]->slant = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_typeface()
{
   if (!get_tok())   return parsingError();  //

   textpenarray[textpencnt]->typeface = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_character_spacing()
{
   if (!get_tok())   return parsingError();  //

   textpenarray[textpencnt]->charSpacing = cnv_unit(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_add_clearance()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_line_spacing()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_min_device_height()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int textpen_max_device_height()
{
   get_newline();

   return 1;
}

/****************************************************************************/
/*
   Text pen can have 2 syntax
1.
!  !  @  !  !  @  TEXT_PEN                := 35;
2.
!  !  @  !  !  @  TEXT_PEN
!  !  @  !  !  @  !  ATTRIBUTES              := [ PROPORTIONAL_SPACED,
!  !  @  !  !  @  !                               RECTANGLE_IF_SMALL ];
!  !  @  !  !  @  !  FONT                    :=  1;
!  !  @  !  !  @  !  HEIGHT                  :=         6;
!  !  @  !  !  @  !  WIDTH                   :=       3.5;
!  !  @  !  !  @  !  SLANT                   :=         0;
!  !  @  !  !  @  !  TYPEFACE                :=       .28;
!  !  @  !  !  @  !  CHARACTER_SPACING       :=       1.2;
!  !  @  !  !  @  !  LINE_SPACING            :=        12;
!  !  @  !  !  @  !  ADD_CLEARANCE           :=         0;
!  !  @  !  !  @  !  MIN_DEVICE_HEIGHT       :=       1.4;
!  !  @  !  !  @  !  MAX_DEVICE_HEIGHT       :=         0;
!  !  @  !  !  @  !..FIN_TEXT_PEN;
*/
static int text_textpen()
{
   CString  t = cur_line;
   int   i;

   if (t.Right(1) == ";")
   {
      if (!get_tok())   return parsingError();  // x

      t = strtok(token," \t\n;");
      cur_atext.textpen = atoi(token);
      cur_atext.h = -1;

      for (i=0;i<textpencnt;i++)
      {
         if (textpenarray[i]->number == cur_atext.textpen)
         {
            cur_atext.h = textpenarray[i]->height;
            cur_atext.w = textpenarray[i]->getCharBoxWidth();
            cur_atext.charw = textpenarray[i]->width;

            break;
         }
      }

      if (cur_atext.h < 0)
      {
         fprintf(logFp, "Text pen [%d] not defined at %ld\n",cur_atext.textpen, getInputFileLineNumber());
      }

      get_newline();
   }
   else
   {
      // other definition. This is only a temporary definition, not needed later.
      TLTextpen   *c = new TLTextpen;
      textpenarray.SetAtGrow(textpencnt,c);

      // here the add_space and width is added.

      int res  = loop_command(tltextpen_lst,SIZ_TLTEXTPEN_LST, "TEXT_PEN");

      cur_atext.textpen = -1;
      cur_atext.h = textpenarray[textpencnt]->height;
      cur_atext.w = textpenarray[textpencnt]->getCharBoxWidth();
      cur_atext.charw = textpenarray[textpencnt]->width;

      delete textpenarray[textpencnt]; // delete temp pen definition
      textpenarray[textpencnt] = NULL;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int text_location()
{
   if (!get_tok())   return parsingError();  // x

   cur_atext.x = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   cur_atext.y = cnv_unit(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int text_string()
{
   if (!get_tok())   return parsingError();  // x

   cur_atext.string = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int text_type()
{
   if (!get_tok())   return parsingError();  // x

   cur_atext.type = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int text_place()
{
   if (!get_tok())   return parsingError();  //

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int text_rotation()
{
   if (!get_tok())   return parsingError();  // x

   cur_atext.rotation = atof(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
   L = Left
   M = middle
   R = right
*/
static int text_xAlign()
{
   if (!get_tok())   return parsingError();  // x

   cur_atext.setHorizontalAlignment( token[0]);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
   B = bottom
   M = middle
   T = top
*/
static int text_yAlign()
{
   if (!get_tok())   return parsingError();  // x

   if (!STRCMPI(token,"HALF"))
      cur_atext.setVerticalAlignment('M');
   else
      cur_atext.setVerticalAlignment( token[0]);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_layer()
{
   if (!get_tok())   return parsingError();  // x

   cur_via.layer = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int testPoint_layer()
{
   if (!get_tok())   return parsingError();  // x

   cur_via.layer = token;

   getThedaReader().setComponentPlacementLayerName(token);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_location()
{
   if (!get_tok())   return parsingError();  // x

   cur_via.x = cnv_unit(token);

   if (!get_tok())   return parsingError();  // x

   cur_via.y = cnv_unit(token);

   //getThedaReader().setTestProbeLocation(cur_via.x,cur_via.y);

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_rotation()
{
   if (!get_tok())   return parsingError();  // x

   cur_via.rotation = atof(token);
   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_needed()
{
   CString t;
   char    tmp[1000];
   char    *lp;

   cur_via.needed = "";

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t = cur_line;
      cur_via.needed += t;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   // delete needed
   // delete :=
   // delete  [ ] ;
   if (strlen(cur_via.needed))
   {
      strcpy(tmp, cur_via.needed);
      cur_via.needed = "";
      lp = strtok(tmp,":=[], \t;");

      while (lp)
      {
         if (STRCMPI(lp,"NEEDED"))     // if not needed
         {
            cur_via.needed += lp;
            cur_via.needed += ",";
         }

         lp = strtok(NULL,":=[], \t;");
      }
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_pulled_pins()
{
   CString t;
   t = "";

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t += cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   if (t.CompareNoCase("NONE_PULLED"))
   {
      // string is PULLED_PINS  := [ 'xxx' ];
      int find1 = t.Find("[", 0);
      int find2 = t.ReverseFind(']');

      if (find1 > -1 && find2 > -1)
         netlistcomparray[netlistcompcnt]->pulled_pins = t.Mid(find1+1,find2-find1-1);
      else
         netlistcomparray[netlistcompcnt]->pulled_pins = t;

      netlistcomparray[netlistcompcnt]->pulled_pins.TrimLeft();
      netlistcomparray[netlistcompcnt]->pulled_pins.TrimRight();
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int net_comp_pulled_mounting_holes()
{
   CString t;

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t = cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_contacted()
{
   CString t;

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t = cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   get_newline();

   return 1;
}

static int testPoint_contacted()
{
   CString t;

   while (TRUE)
   {
      if (!get_tok())   return parsingError();

      getThedaReader().setContactLayerName(token);

      t = cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_testpoint_probe_pads()
{
   if (BAREBOARD_PROBES == FALSE)
   {
      fnull("PROBE_PADS", FALSE);
      return 1;
   }
   // need to add other condition here

   return via_tp_probes_probe();
}

/****************************************************************************/
/* probes_probes
*/
static int via_tp_probes_probe()
{
   // Declare needed variables
   char  *tmpline, endStruct[20], delim[20];
   CString ProbeRef;
   BOOL mirror = FALSE;
   BOOL GoAgain = TRUE;
   DataStruct *ProbeData = NULL;

   strcpy(delim, " ':=\t\n;[]");
   strcpy(endStruct, "..FIN_PROBE_PADS");
   // parse contained structures
   get_newline();
   get_tok();
   tmpline = STRDUP(cur_line);
   char  *lp;
   lp = strtok(tmpline,delim);

   do
   {
      if (STRICMP(token, endStruct) == 0)
      {
         return 1;
      }
      else if (STRICMP(token, "REFERENCE_DESIGNATOR") == 0)
      {
         if (lp = strtok(NULL, delim))
         {
            ProbeRef = lp;
            ProbeData = generateProbeReference(*doc, file, ProbeRef, "", "");

            if (ProbeData == NULL)
            {
               fprintf(logFp, "Error in PROBE_PAD.  REFERENCE_DESIGNATOR of %s does not exist at line %ld\n", ProbeRef, getInputFileLineNumber());
               display_error++;
               continue;
            }

            ProbeData->getPoint()->x = (float)cur_via.x;
            ProbeData->getPoint()->y = (float)cur_via.y;

            if (!STRCMPI(cur_via.layer, QSolderSide))
            {
               ProbeData->getInsert()->setPlacedBottom(true);
               ProbeData->getInsert()->setMirrorFlags(MIRROR_ALL);
            }
         }
      }
      //else if (STRICMP(token, "LAYER") == 0)
      //{
      //   if (lp = strtok(NULL,delim))
      //   {
      //      if (STRICMP(lp,QSolderSide) == 0)
      //         mirror = TRUE;

      //      if (ProbeData)
      //         ProbeData->getInsert()->setMirrorFlags(mirror);
      //   }
      //}
      else if (STRICMP(token, "PROBE") == 0)
      {
         // do nothing
      }
      else if ((STRICMP(token, "PROBE_PAD") == 0)||(STRICMP(token, "..FIN_PROBE_PAD") == 0))
      {
         // do nothing
      }
      else if (STRICMP(token, "TEXT_NODES") == 0)
      {
         fnull("TEXT_NODES", FALSE);
      }

      get_newline();
      get_tok();
      tmpline = STRDUP(cur_line);
      lp = strtok(tmpline,delim);

   } while (GoAgain);

   return 1;
}

/****************************************************************************/
/* probes_probes
*/
static int probes_probe()
{
   // Declare needed variables
   char  *tmpline, endStruct[20], delim[20];
   CString ProbeRef;
   BOOL mirror = FALSE;
   BOOL GoAgain = TRUE;
   DataStruct *ProbeData;

   strcpy(delim, " ':=\t\n;[]");
   strcpy(endStruct, "..FIN_PROBE_PADS");
   // parse contained structures
   get_newline();
   get_tok();
   tmpline = STRDUP(cur_line);
   char  *lp;
   lp = strtok(tmpline,delim);

   do
   {
      if (STRICMP(token, endStruct) == 0)
      {
         return 1;
      }
      else if (STRICMP(token, "REFERENCE_DESIGNATOR") == 0)
      {
         if (lp = strtok(NULL, delim))
         {
            ProbeRef = lp;
            ProbeData = generateProbeReference(*doc, file, ProbeRef, "", "");

            if (ProbeData == NULL)
            {
               fprintf(logFp, "Error in PROBE_PAD.  REFERENCE_DESIGNATOR of %s does not exist at line %ld\n",
                  ProbeRef, getInputFileLineNumber());

               display_error++;
               continue;
            }

            if (PhysPinX == 0.0 && PhysPinY == 0.0)
            {
               makePhysPinLoc();
            }

            ProbeData->getPoint()->x = (float)PhysPinX;
            ProbeData->getPoint()->y = (float)PhysPinY;
            PhysPinX = 0.0;
            PhysPinY = 0.0;
         }
      }
      else if (STRICMP(token, "LAYER") == 0)
      {
         if (lp = strtok(NULL,delim))
         {
            if (STRICMP(lp,QSolderSide) == 0)
               mirror = TRUE;

            if (ProbeData)
               ProbeData->getInsert()->setMirrorFlags(mirror);
         }
      }
      else if (STRICMP(token, "PROBE") == 0)
      {
         // do nothing
      }
      else if ((STRICMP(token, "PROBE_PAD") == 0)||(STRICMP(token, "..FIN_PROBE_PAD") == 0))
      {
         // do nothing
      }
      else if (STRICMP(token, "TEXT_NODES") == 0)
      {
         fnull("TEXT_NODES", FALSE);
      }

      get_newline();
      get_tok();
      tmpline = STRDUP(cur_line);
      lp = strtok(tmpline,delim);
   } while (GoAgain);
   // process data and place probe, note that in some
   // cases we will not yet have the net name.

   // doc->SetAttrib(&probeData->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, netname.GetBuffer(0), SA_OVERWRITE, NULL);
   return 1;
}

CString getMirroredContactLayerName(const CString& contactLayerName)
{
   CString mirroredContactLayerName = contactLayerName;

   if (! contactLayerName.IsEmpty())
   {
      if (contactLayerName.CompareNoCase(QComponentSide) == 0)
      {
         mirroredContactLayerName = QSolderSide;
      }
      else if (contactLayerName.CompareNoCase(QSolderSide) == 0)
      {
         mirroredContactLayerName = QComponentSide;
      }
      else
      {
         LayerStruct* contactLayer = doc->getLayerNoCase(contactLayerName);

         if (contactLayer != NULL)
         {
            LayerStruct* mirroredContactLayer = doc->getLayer(contactLayer->getMirroredLayerIndex());
            mirroredContactLayerName  = mirroredContactLayer->getName();
         }
      }
   }

   return mirroredContactLayerName;
}

/****************************************************************************/
/* comp_package_pin
*/
int comp_package_pin()
{
   get_newline();

   cur_pin.rotation = 0;
   cur_pin.x = 0;
   cur_pin.y = 0;
   cur_pin.x_mirror = FALSE;
   cur_pin.number.Empty();
   cur_pin.identifier.Empty();
   cur_pin.padstack_id.Empty();
   cur_pin.m_contactLayerName.Empty();

   cur_probe.refDes.Empty();
   cur_probe.layerName.Empty();

   int res = loop_command(comp_package_pin_lst, SIZ_COMP_PACKAGE_PIN_LST, "PIN");

   //s_pinNameToContactLayerMap.addPin(cur_pin.number,cur_pin.m_contactLayerName);
   CString contactLayerName = cur_pin.m_contactLayerName;
   CString mirroredContactLayerName = getMirroredContactLayerName(contactLayerName);

   cur_comp.getNetlistComp()->addPin(cur_pin.identifier,contactLayerName,mirroredContactLayerName);
   getThedaReader().clearContactLayerName();

   get_newline();

   return res;
}

/****************************************************************************/
/* comp_package_pin_probePad_refdes
*/
int comp_package_pin_contacted()
{
   if (!get_tok())   return parsingError();  // x

   cur_pin.m_contactLayerName = token;

   getThedaReader().setContactLayerName(token);

   if (getThedaReader().isSolderSideName(cur_comp.getComponentLayerName()))
   {
      cur_pin.x_mirror = TRUE;
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/* comp_package_pin_probePads
*/
int comp_package_pin_probePads()
{
   int res = loop_command(comp_package_probePads_lst, SIZ_COMP_PACKAGE_PIN_PROBEPADS_LST, "PROBE_PADS");

   return res;
}

int via_probePads()
{
   int res = loop_command(via_probePads_lst, SIZ_VIA_PROBEPADS_LST, "PROBE_PADS");

   return res;
}

/****************************************************************************/
/* comp_package_pin_probePad
*/
int comp_package_pin_probePad()
{
   get_newline();

   int res = loop_command(comp_package_probePad_lst, SIZ_COMP_PACKAGE_PIN_PROBEPAD_LST, "PROBE_PAD");

   get_newline();

   CThedaTestProbe* testProbe = getThedaReader().getThedaTestProbes().getTestProbe(cur_probe.refDes);

   if (testProbe != NULL)
   {
      CTMatrix matrix;

      matrix.translateCtm(cur_comp.x,cur_comp.y);
      matrix.rotateDegreesCtm(cur_comp.rot);

      CBasesVector probeBasesVector(cur_pin.x,cur_pin.y,1.,cur_pin.rotation,false);
      probeBasesVector.transform(matrix);

      testProbe->getProbeInsertData()->getInsert()->setBasesVector(probeBasesVector);

      bool mirrorFlag = (cur_comp.x_mirror != 0) != (cur_pin.x_mirror != 0) != (cur_probe.layerName.Left(4).CompareNoCase("COMP") != 0);
      testProbe->getProbeInsertData()->getInsert()->setMirrorFlagBits(mirrorFlag ? MIRROR_ALL : 0);

      // always set probe insert angles to 0. - knv
      testProbe->getProbeInsertData()->getInsert()->setAngleDegrees(0.);
   }
   else
   {
      fprintf(logFp, "TestProbe instance with refDes '%s' could not be found.\n", cur_probe.refDes);

      display_error++;
   }

   return res;
}

int via_probePad()
{
   get_newline();

   int res = loop_command(comp_package_probePad_lst, SIZ_COMP_PACKAGE_PIN_PROBEPAD_LST, "PROBE_PAD");

   get_newline();

   CThedaTestProbe* testProbe = getThedaReader().getThedaTestProbes().getTestProbe(cur_probe.refDes);

   if (testProbe != NULL)
   {
      DataStruct* probeData = testProbe->getProbeInsertData();

      probeData->setLayerIndex(-1);

      probeData->getInsert()->setOriginX(cur_via.x);
      probeData->getInsert()->setOriginY(cur_via.y);
      probeData->getInsert()->setAngleDegrees(0.);  // always set probe insert angles to 0. - knv

      if (cur_via.layer.CompareNoCase(QSolderSide) == 0)
      {
         probeData->getInsert()->setPlacedBottom(true);
         probeData->getInsert()->setMirrorFlags(MIRROR_ALL);

         // mirror attribute layers
         CAttributes& probeAttributes = probeData->attributes();
         Attrib* attrib;
         WORD key;

         for (POSITION pos = probeAttributes.GetStartPosition();pos != NULL;)
         {
            probeAttributes.GetNextAssoc(pos,key,attrib);

            if (attrib->isVisible())
            {
               int mirroredLayerIndex = doc->getMirroredLayerIndex(attrib->getLayerIndex(),true);
               attrib->setLayerIndex(mirroredLayerIndex);
            }
         }
      }
   }
   else
   {
      fprintf(logFp, "TestProbe instance with refDes '%s' could not be found.\n", cur_probe.refDes);

      display_error++;
   }

   return res;
}

/****************************************************************************/
/* comp_package_pin_probePad_refdes
*/
int comp_package_pin_probePad_refdes()
{
   if (!get_tok())   return parsingError();  // x

   cur_probe.refDes = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/* comp_package_pin_probePad_refdes
*/
int comp_package_pin_probePad_layer()
{
   if (!get_tok())   return parsingError();  // x

   cur_probe.layerName = token;
   get_newline();

   return 1;
}

/****************************************************************************/
/* comp_package_pin_probePad_textNodes
*/
int comp_package_pin_probePad_textNodes()
{
   getTestProbeAttributes().empty();

   int res = loop_command(comp_package_probePads_textNodes_lst, SIZ_COMP_PACKAGE_PIN_PROBEPADS_TEXTNODES_LST, "TEXT_NODES");

   CThedaTestProbe* testProbe = getThedaReader().getThedaTestProbes().getTestProbe(cur_probe.refDes);

   if (testProbe != NULL)
   {
      CAttributes*& testProbeAttributes = testProbe->getProbeInsertData()->getAttributesRef();

      for (POSITION pos = getTestProbeAttributes().GetStartPosition();pos != NULL;)
      {
         WORD keywordIndex;
         Attrib* attribute;

         getTestProbeAttributes().GetNextAssoc(pos,keywordIndex,attribute);

         CString value = doc->getAttributeStringValue(attribute);

         if (value.CompareNoCase(testProbe->getRefDes()) == 0)
         {
            keywordIndex = doc->IsKeyWord(ATT_REFNAME,0);
         }

         Attrib* existingTestProbeAttribute;
         Attrib* testProbeAttribute = new Attrib(*attribute);

         if (testProbeAttributes->Lookup(keywordIndex,existingTestProbeAttribute))
         {
            existingTestProbeAttribute->addInstance(testProbeAttribute);
         }
         else
         {
            testProbeAttributes->SetAt(keywordIndex,testProbeAttribute);
         }
      }
   }
   else
   {
      fprintf(logFp, "TestProbe instance with refDes '%s' could not be found.\n", cur_probe.refDes);

      display_error++;
   }

   return res;
}

/****************************************************************************/
/* makePhysPinLoc
*/
void makePhysPinLoc()
{
   // Here we take the symbol pin location and the comp location
   // and calculate the pin's physical location on the board.
   CTMatrix packageMatrix;
   CPoint2d t_pin (cur_pin.x, cur_pin.y);
   // get actual component mirror and rotation
   int c_mir, y_mir, x_mir;
   double c_x, c_y, c_rot;
   c_x = cur_comp.x;
   c_y = cur_comp.y;
   c_mir = cur_comp.mir;
   x_mir = cur_comp.x_mirror;
   y_mir = cur_comp.y_mirror;
   c_rot = cur_comp.rot;

   if (c_mir == MIRROR_LAYERS)
   {
      // if PLACED on bottom, it is always mirrored around X in THEDA Version 1.1!
      if (tlversion.software_revision < 2)
      {
         if (!x_mir && !y_mir)
            x_mir = TRUE;
      }
   }

   if (x_mir)
   {
      c_rot = 180 + c_rot;
      normalizeToPositiveDegrees(c_rot);

      c_mir |= MIRROR_FLIP;
   }
   else if (y_mir)
   {
      c_mir |= MIRROR_FLIP;
   }

   packageMatrix.rotateDegrees(c_rot);
   packageMatrix.scale(((c_mir & MIRROR_FLIP) != 0) ? -1. : 1.,1.);
   packageMatrix.translate(c_x, c_y);
   packageMatrix.transform(t_pin);

   PhysPinX = t_pin.x;
   PhysPinY = t_pin.y;
}

/****************************************************************************/
/*
*/
static int via_graphics()
{
   fnull("GRAPHICS", FALSE);

   return 1;
}

/****************************************************************************/
/*
*/
static int theda_symbol_type_list()
{
   CString t, t1;
   t = "";

   while (TRUE)
   {
      if (!get_tok())   return parsingError();  // x

      t += cur_line;
      get_newline();

      if (t.Right(1) == ";")
         break;
   }

   char  tmp[255], *lp;

   if (strlen(t) < 255)
   {
      strcpy(tmp, t);

      // SYMBOL_TYPE_LIST := [ xxx, yyy ]
      lp = strtok(tmp,"[");
      lp = strtok(NULL," \t[];,");
      t1 = "";

      while (lp)
      {
         if (strlen(t1))
            t1 += "_";

         t1 += lp;
         lp = strtok(NULL," \t[];,");
      }

      do_symboltype(t1);
   }

   get_newline();

   return 1;
}

/****************************************************************************/
/*
*/
static int via_padstack_id()
{
   if (!get_tok())   return parsingError();  // x

   cur_via.padstack_id = token;
   get_newline();

   return 1;
}

/****************************************************************************/

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(List *tok_lst,int tok_size)
{
   int      i;

   for (i = 0; i < tok_size; ++i)
   {
      if (!STRCMPI(token,tok_lst[i].token))
      {
         return(i);
      }
   }

   return(-1);
}

/***************************************************************************/
/*
   Get a token from the input file.
*/
bool get_tok()
{
   if (!Push_tok)
   {
      while (!get_next(cur_line,cur_new))
      {
         if (!get_line(cur_line,MAX_LINE))
         {
            return false;
         }
         else
         {
            cur_new = TRUE;
         }
      }

      cur_new = FALSE;
   }
   else
   {
      Push_tok = FALSE;
   }

   return true;
}

/****************************************************************************/
/*
   Push back last token.
*/

static int push_tok()
{
   return (Push_tok = TRUE);
}

/****************************************************************************/
/*
   Get the next token from line.
*/
bool get_next(char *lp,int newlp)
{
   static   char  *cp;
   int      i = 0;
   int      token_name = FALSE;

   if (newlp)  cp = lp;

   while ((*cp == '[' || *cp == '(' || *cp == ',' ||
           *cp == ')' || *cp == ']' ||
           *cp == ':' || *cp == '=' ||isspace(*cp)) && *cp != '\0')
   {
      cp++;
   }

   switch(*cp)
   {
   case '\0':
      return false;
   case '\'':  // this is text
      token[i] = '\0';
      token_name = TRUE;

      for (++cp; *cp != '\''; ++cp, ++i)
      {
         if (i >= MAX_TOKEN-1)
         {
            fprintf(logFp, "Error in Get Token - Token too long at %ld\n", getInputFileLineNumber());
            display_error++;
         }

         if (*cp == '\n')
         {
            /* text spans over 1 line */
            if (!get_line(cur_line,MAX_LINE))
            {
               fprintf(logFp, "Error in Line Read\n");
               display_error++;
            }

            cp = cur_line;

            /* the quote can be the first character in the new line */
            if (*cp == '\'' && token[i] != '\\')
            {
               break;
            }

            if (*cp == '\\')
            {
               ++cp;
            }

            token[i] = *cp;
         }
         else if (*cp == '\\')
         {
            //  := 'DOE\'; <== here is a error, a \ without a following command.
            if ((*cp + 1) == '\\')
            {
               ++cp;
            }
            else if ((*cp + 1) == '\b')
            {
               ++cp;
            }
            else if ((*cp + 1) == '\n')
            {
               ++cp;

               if (*cp == '\n')
               {
                  /* text spans over 1 line */
                  if (!get_line(cur_line,MAX_LINE))
                  {
                     fprintf(logFp, "Error in Line Read\n");
                     display_error++;
                  }

                  cp = cur_line;
               }
            }

            token[i] = *cp;
         }
         else if (*cp != '\0')
         {
            token[i] = *cp;
         }
      }

      ++cp;
      break;
   default:
      for (;*cp != '(' && *cp != '[' && *cp != ')' && *cp != ']' && *cp != ',' &&
            !isspace(*cp) && *cp != '\'' && *cp != '\n' && *cp != '\0'; ++cp, ++i)
      {
         if (*cp == '\\')
         {
            ++cp;
         }

         token[i] = *cp;
      }

      // eliminate end if line ;
      if (token[i-1] == ';')
      {
         token[i-1] = '\0';
      }

      break;
   }

   if (!i && !token_name)
   {
      return false;
   }

   token[i] = '\0';

   if (strncmp(token,"#------",7) == 0)
   {
      token[7] = '\0';
   }

   return true;
}

/******************************************************************************
* get_line()
*  - Get a line from the input file.
*/
static int get_line(char *cp,int size)
{
   if (fgets(cp, size, ifp) == NULL)
   {
      if (!feof(ifp))
      {
         ErrorMessage("Read Error !", "Fatal THEDA Parse Error", MB_OK | MB_ICONHAND);
         return FALSE;
      }
      else
      {
         cp[0] = '\0';
         return(FALSE);
      }
   }
   else
   {
      if ((getInputFileLineNumber() % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }

      incrementInputFileLineNumber();

      orig_tl_line = cp;

      if (cp[strlen(cp)-1] != '\n')
      {
         //printf(" No end of line found at %ld\n",getInputFileLineNumber());
         fprintf(logFp, " No end of line found at %ld\n",getInputFileLineNumber());
         return (FALSE);
      }

      // take white space out
      CString t = cp;
      t.TrimLeft();
      t.TrimRight();
      strcpy(cp,t);

      // kill all until isalnum form the beginning of a line.
      // but also allow -.2 or .2 as koos that happens if there is a ()
      STRREV(cp);

      //while (strlen(cp) && !isalnum(cp[strlen(cp)-1]))
      while (strlen(cp) && (isspace(cp[strlen(cp)-1]) || cp[strlen(cp)-1] == '!' ||
             cp[strlen(cp)-1] == '@' || cp[strlen(cp)-1] == '('))
      {
         cp[strlen(cp)-1] = '\0';
      }

      STRREV(cp);
      t = cp;        // take white space out
      t.TrimLeft();
      t.TrimRight();
      strcpy(cp,t);
   }

   return(TRUE);
}


/******************************************************************************
* parsingError() - Parsing Error
*/
int parsingError()
{
   CString tmp;
   tmp.Format("Fatal Parsing Error : Token \"%s\" on line %ld\n", token, getInputFileLineNumber());

   ErrorMessage(tmp, "Fatal THEDA Parse Error", MB_OK | MB_ICONHAND);

#ifdef EnableCommandStackTracing
   CString traceInfo = s_commandListStack.trace();

   CStdioFile traceFile;

   if (traceFile.Open("c:\\ThedaTraceFile.txt",CFile::modeCreate | CFile::modeWrite) != NULL)
   {
      traceFile.WriteString(traceInfo);
   }
#endif

   return -1;
}

/******************************************************************************
* init_all_mem
*/
void init_all_mem()
{
   display_error = 0;
   file = NULL;
   setCurData(NULL);
   curtype = NULL;
   curlayer = NULL;
   Push_tok = FALSE;
   cur_new = TRUE;
   cur_netname = "";
   cur_filled = FALSE;
   textpencnt = 0;
   design_technique = 'A';
   test_point_cnt = 0;
   smooth_polygon_error = 0;
   cur_defaulttracewidthindex = 1;
   cur_find_any_board = FALSE;
   cur_section = 0;
   comp_prop = 0;

   ignorearray.SetSize(100, 100);
   ignorecnt = 0;

   devicecomparray.SetSize(100, 100);
   devicecompcnt = 0;

   compproparray.SetSize(100, 100);
   comppropcnt = 0;

   shapecirclearray.SetSize(100, 100);
   shapecirclecnt = 0;

   netlistcomparray.SetSize(100, 100);
   netlistcompcnt = 0;

   attribmaparray.SetSize(100, 100);
   attribmapcnt = 0;

   geomarray.SetSize(100, 100);
   geomcnt = 0;

   textpenarray.SetSize(100, 100);
   textpencnt = 0;

   polyarray.SetSize(100, 100);
   polycnt = 0;

   pinnamearray.SetSize(100, 100);
   pinnamecnt = 0;

   padformarray.SetSize(100, 100);
   padformcnt = 0;

   drillarray.SetSize(100, 100);
   drillcnt = 0;

   return;
}

/******************************************************************************
* free_all_mem
*/
void free_all_mem()
{
	int i=0;
   for (i=0; i<ignorecnt; i++)
      delete ignorearray[i];
   ignorearray.RemoveAll();
   ignorecnt = 0;

   for (i=0; i<devicecompcnt; i++)
      delete devicecomparray[i];
   devicecomparray.RemoveAll();
   devicecompcnt = 0;

   for (i=0; i<comppropcnt; i++)
      delete compproparray[i];
   compproparray.RemoveAll();
   comppropcnt = 0;

   for (i=0; i<shapecirclecnt; i++)
      delete shapecirclearray[i];
   shapecirclearray.RemoveAll();
   shapecirclecnt = 0;

   for (i=0; i<netlistcompcnt; i++)
      delete netlistcomparray[i];
   netlistcomparray.RemoveAll();
   netlistcompcnt = 0;

   for (i=0; i<attribmapcnt; i++)
      delete attribmaparray[i];
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   for (i=0; i<geomcnt; i++)
      delete geomarray[i];
   geomarray.RemoveAll();
   geomcnt = 0;

   for (i=0; i<textpencnt; i++)
      delete textpenarray[i];
   textpenarray.RemoveAll();
   textpencnt = 0;

   polyarray.RemoveAll();
   pinnamearray.RemoveAll();  // that is always deleted on cur_number_of_pins
   padformarray.RemoveAll();  // 60 mil round
   drillarray.RemoveAll();

   return;
}

/****************************************************************************/
/*
   NULL string will return 0
   This units come in inches.
   cadif file is in 10-8 meters = 0.01 micros = inch * 2540000

   Units_Factor(format.PortFileUnits, Settings.PageUnits)

*/
double   cnv_unit(char *s)
{
   double x;

   if (STRLEN(s) == 0)  return 0.0;

   x = atof(s);

   return x * faktor;
}

/*****************************************************************************/
/*
*/
static int get_lay_att(const char *l)
{
   int   i;

   for (i=0;i<layer_attr_cnt;i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
      {
         return layer_attr[i].attr;
      }
   }

   return LAYTYPE_UNKNOWN;
}

/*****************************************************************************/
/*
*/
static int get_lay_index(const char *l)
{
   int   i;

   for (i=0;i<layer_attr_cnt;i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
      {
         return i;
      }
   }

   if (layer_attr_cnt < MAX_LAYERS)
   {
      layer_attr[layer_attr_cnt].name = l;
      layer_attr[layer_attr_cnt].attr = 0;
      layer_attr[layer_attr_cnt].mirror = -1;
      layer_attr_cnt++;
   }
   else
   {
      fprintf(logFp, "Too many layers\n");

      return -1;
   }

   return layer_attr_cnt-1;
}

/*****************************************************************************/
/*
*/
static GraphicClassTag get_layerclass(const char *layername)
{
   if (is_layer_compoutline(layername))
      return graphicClassComponentOutline;

   if (is_boardoutline(layername))
      return graphicClassBoardOutline;

   return graphicClassNormal;
}

/****************************************************************************/
/*
*/
static int load_thedasettings(CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   boardoutlinecnt = 0;
   compoutlinecnt  = 0;
   layer_attr_cnt = 0;
   layer_rename_cnt = 0;
   material_layer_type_cnt = 0;
   smdtypecnt = 0;
   ComponentSMDrule = 0;
   convert_complex_power = TRUE;
   use_pin_ids_physical = 0;
   IGNORE_LAYER_SYMBOLTYPE = FALSE;
   IGNORE_LAYER_MATERIALS = FALSE;
   BAREBOARD_PROBES = FALSE;
   WIREBONDPEN = FALSE;
   updateRefdesFlag = true;
   probeSize = 0.0;
   probeSizeUnit = -1;
   drillSize = 0.0;
   drillSizeUnit = -1;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp,"THEDA Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".IGNORE_LAYER_SYMBOLTYPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
            {
               continue;
            }

            if (toupper(lp[0]) == 'Y')
            {
               IGNORE_LAYER_SYMBOLTYPE = TRUE;
            }
         }
         else if (!STRICMP(lp, ".IGNORE_LAYER_MATERIALS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               IGNORE_LAYER_MATERIALS = TRUE;
         }
         else if (!STRICMP(lp, ".AdjustLayerNames"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            getThedaReader().setOptionAdjustLayerNames(toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".RectifyComponents"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            getThedaReader().setOptionRectifyComponents(toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".RectifyViasAndTestPoints"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            getThedaReader().setOptionRectifyViasAndTestPoints(toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".PurgeUnusedWidthsAndGeometries"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            getThedaReader().setOptionPurgeUnusedWidthsAndGeometries(toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".PurgeUnusedLayers"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            getThedaReader().setOptionPurgeUnusedLayers(toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".FixupGeometryNames"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            getThedaReader().setOptionFixupGeometryNames(toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".BAREBOARD_PROBES"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               BAREBOARD_PROBES = TRUE;
         }
         else if (!STRICMP(lp, ".WIRE_BOND_PEN"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               WIREBONDPEN = TRUE;
         }
         else if (!STRICMP(lp, ".USE_PIN_IDS_PHYSICAL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               use_pin_ids_physical = TRUE;
         }
         else if (!STRICMP(lp, ".UPDATE_REFDES"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            updateRefdesFlag = (toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".CONVERTCOMPLEXPOWER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'N')
               convert_complex_power = FALSE;
         }
         else if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            ComponentSMDrule = atoi(lp);
         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            if (compoutlinecnt < 30)
            {
               compoutline[compoutlinecnt] = lp;
               compoutlinecnt++;
            }
            else
            {
               fprintf(logFp, "Too many compoutlines defined.\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".SMDTYPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (smdtypecnt < MAX_SMDTYPE)
            {
               smdtype[smdtypecnt] = lp;
               smdtypecnt++;
            }
            else
            {
               fprintf(logFp, "Too many smdtypes defined.\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".IGNORE_PADSTACK_SYMBOL_TYPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            TLIGNORE *c = new TLIGNORE;
            ignorearray.SetAtGrow(ignorecnt++, c);
            c->type = SECTION_PADSTACKGRAPHIC;
            c->name = lp;
         }
         else if (!STRICMP(lp, ".IGNORE_PADSTACK_LAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            TLIGNORE *c = new TLIGNORE;
            ignorearray.SetAtGrow(ignorecnt++, c);
            c->type = SECTION_PADSTACKGRAPHIC;
            c->name = lp;
         }
         else if (!STRICMP(lp,".BOARDOUTLINE"))
         {
            int s;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (!STRCMPI(lp, "PACKAGES"))
            {
               s = SECTION_PACKAGE;
            }
            else if (!STRCMPI(lp ,"OUTLINES"))
            {
               s = SECTION_BOARDOUTLINE;
            }
            else if (!STRCMPI(lp, "KEEPOUTS"))
            {
               s = SECTION_KEEPOUT;
            }
            else if (!STRCMPI(lp, "DIMENSIONS"))
            {
               s = SECTION_DIMENSION;
            }
            else if (!STRCMPI(lp, "KEEPINS"))
            {
               s = SECTION_KEEPIN;
            }
            else
            {
               fprintf(logFp, "Unknown .BOARDOUTLINE section [%s] defined.\n", lp);
               display_error++;

               continue;
            }

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString l = lp;

            if (boardoutlinecnt < 30)
            {
               boardoutline[boardoutlinecnt].section = s;
               boardoutline[boardoutlinecnt].layer = l;
               boardoutlinecnt++;
            }
            else
            {
               fprintf(logFp, "Too many boardoutlines defined.\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".LAYERRENAME"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString olayer = _strupr(lp);
            olayer.TrimLeft();
            olayer.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString rlayer = lp;
            rlayer.TrimLeft();
            rlayer.TrimRight();

            if (layer_rename_cnt < MAX_LAYERS)
            {
               layer_rename[layer_rename_cnt].origname = olayer;
               layer_rename[layer_rename_cnt].rename = rlayer;
               layer_rename_cnt++;
            }
            else
            {
               fprintf(logFp, "Too many layer renames\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            CString tllayer = _strupr(lp);
            tllayer.TrimLeft();
            tllayer.TrimRight();

            if ((lp = get_string(NULL," \t;\n")) == NULL)
               continue;

            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);
            int layindex = get_lay_index(tllayer);
            layer_attr[layindex].attr = laytype;
         }
         else if (!STRICMP(lp, ".MIRRORLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString l1 = _strupr(lp);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString l2 = _strupr(lp);

            int layindex1 = get_lay_index(l1);
            int layindex2 = get_lay_index(l2);
            layer_attr[layindex1].mirror = layindex2;
            layer_attr[layindex2].mirror = layindex1;
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString a1 = lp;
            a1.MakeUpper();
            a1.TrimLeft();
            a1.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString a2 = lp;
            a2.MakeUpper();
            a2.TrimLeft();
            a2.TrimRight();

            TLAttribmap *c = new TLAttribmap;
            attribmaparray.SetAtGrow(attribmapcnt++, c);
            c->attrib = a1;
            c->mapattrib = a2;
         }
         else if (!STRICMP(lp, ".LAYERATTER"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            material_layer_type[material_layer_type_cnt].layername = lp;

            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            CString layerType = lp;

            if (!layerType.CompareNoCase("Signal Top"))
               material_layer_type[material_layer_type_cnt].layertype = LAYTYPE_SIGNAL_TOP;
            else if (!layerType.CompareNoCase("Signal Bottom"))
               material_layer_type[material_layer_type_cnt].layertype = LAYTYPE_SIGNAL_BOT;

            material_layer_type_cnt++;
         }
         else if (!STRICMP(lp, ".PROBE_SIZE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;


            probeSize = atof(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            if (!STRCMPI(lp, "mm"))
               probeSizeUnit = UNIT_MM;
            else if (!STRCMPI(lp, "in"))
               probeSizeUnit = UNIT_INCHES;
         }
         else if (!STRICMP(lp, ".DRILL_SIZE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            drillSize = atof(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            if (!STRCMPI(lp, "mm"))
               drillSizeUnit = UNIT_MM;
            else if (!STRCMPI(lp, "in"))
               drillSizeUnit = UNIT_INCHES;
         }

      }
   }

   fclose(fp);

   return 1;
}

/****************************************************************************/
/*
   In THEDA, a PAD can have the same name as a padstack.
*/
static int clean_padprefix()
{
   int   i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
      {
         continue;
      }

      if (block->getFlags() & BL_APERTURE)
      {
         if (block->getOriginalName().GetLength() > 0)
         {
            // here now check if a block with the original name exist. If not - change the real name and
            // kill the original name
            if (Graph_Block_Exists(doc, block->getOriginalName(), cur_filenum) == NULL)
            {
               block->setName(block->getOriginalName());
               block->setOriginalName("");
            }
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
   In THEDA, a PADSTACK can have the same name as other things
*/
static int clean_padstackprefix()
{
   int   i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
      {
         continue;
      }

      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         if (block->getOriginalName().GetLength() > 0)
         {
            // here now check if a block with the original name exist. If not - change the real name and
            // kill the original name
            if (Graph_Block_Exists(doc, block->getOriginalName(), cur_filenum) == NULL)
            {
               block->setName(block->getOriginalName());
               block->setOriginalName("");
            }
         }
      }
   }

   return 1;
}

//=============================================================================

TLTextpen::TLTextpen()
{
   number      = 0;
   height      = 0.;
   width       = 0.;
   slant       = 0.;
   typeface    = 0.;
   charSpacing = 0.;
}

#ifdef EnableCommandStackTracing
//_____________________________________________________________________________
CCommandListEntry::CCommandListEntry(int lineNumber,const CString& command,List* commandList)
{
   m_lineNumber  = lineNumber;
   m_command     = command;
   m_commandList = commandList;
}

//_____________________________________________________________________________
CCommandListStack::CCommandListStack()
{
   m_sp = 0;
   m_stack.SetSize(0,10);
}

int CCommandListStack::push(int lineNumber,const CString& command,List* commandList)
{
   CCommandListEntry* entry = new CCommandListEntry(lineNumber,command,commandList);

   m_stack.SetAtGrow(m_sp,entry);
   m_sp++;

   return m_sp;
}

CCommandListEntry* CCommandListStack::pop()
{
   CCommandListEntry* retval = NULL;

   if (m_sp > 0)
   {
      m_sp--;

      retval = m_stack.GetAt(m_sp);

      m_stack.SetAt(m_sp,NULL);
   }

   return retval;
}

CString CCommandListStack::trace()
{
   CString retval;
   CString line;
   CString indent;

   for (int sp = 0;sp < m_sp;sp++)
   {
      CCommandListEntry* entry = m_stack.GetAt(sp);

      line.Format("%s%s {%s} - %d\n",indent,
         (const char*)entry->getCommand(),
         (const char*)s_commandListMap->getCommandListName(entry->getCommandList()),
         entry->getLineNumber());

      //::OutputDebugString(line);
      //getDebugWriteFormat()->write(line);
      //getDebugWriteFormat()->flush();

      retval += line;
      indent += ((sp % 3) == 2 ? "@  " : "!  ");
   }

   return retval;
}

//_____________________________________________________________________________
CCommandListMap::CCommandListMap()
{
   m_commandListMap.InitHashTable(nextPrime2n(100),FALSE);
   m_commandListNames.SetSize(0,100);
}

CCommandListMap::~CCommandListMap()
{
}

void CCommandListMap::setAt(const CString& listName,List* commandList)
{
   int commandListIndex = m_commandListNames.GetCount();

   m_commandListMap.SetAt(commandList,commandListIndex);
   m_commandListNames.SetAtGrow(commandListIndex,listName);
}

CString CCommandListMap::getCommandListName(List* commandList)
{
   CString commandListName("Unknown");
   WORD commandListIndex;

   if (m_commandListMap.Lookup(commandList,commandListIndex))
   {
      commandListName = m_commandListNames.GetAt(commandListIndex);
   }

   return commandListName;
}

#define nameParameter(a) #a,a

void initCommandListMap()
{
   s_commandListMap->setAt(nameParameter(theda_lst));
   s_commandListMap->setAt(nameParameter(panel_lst));
   s_commandListMap->setAt(nameParameter(version_lst));
   s_commandListMap->setAt(nameParameter(physical_layout_lst));
   s_commandListMap->setAt(nameParameter(net_list_lst));
   s_commandListMap->setAt(nameParameter(keepins_lst));
   s_commandListMap->setAt(nameParameter(text_pens_lst));
   s_commandListMap->setAt(nameParameter(keepouts_lst));
   s_commandListMap->setAt(nameParameter(outlines_lst));
   s_commandListMap->setAt(nameParameter(cutting_marks_lst));
   s_commandListMap->setAt(nameParameter(drawings_lst));
   s_commandListMap->setAt(nameParameter(text_nodes_lst));
   s_commandListMap->setAt(nameParameter(pins_lst));
   s_commandListMap->setAt(nameParameter(wires_lst));
   s_commandListMap->setAt(nameParameter(chip_bond_lands_lst));
   s_commandListMap->setAt(nameParameter(properties_lst));
   s_commandListMap->setAt(nameParameter(vias_lst));
   s_commandListMap->setAt(nameParameter(test_points_lst));
   s_commandListMap->setAt(nameParameter(net_pins_lst));
   s_commandListMap->setAt(nameParameter(conductions_lst));
   s_commandListMap->setAt(nameParameter(inverted_conductions_lst));
   s_commandListMap->setAt(nameParameter(graphics_lst));
   s_commandListMap->setAt(nameParameter(net_components_lst));
   s_commandListMap->setAt(nameParameter(net_nets_lst));
   s_commandListMap->setAt(nameParameter(tlgraphic_lst));
   s_commandListMap->setAt(nameParameter(tlcircle_lst));
   s_commandListMap->setAt(nameParameter(tlarc_lst));
   s_commandListMap->setAt(nameParameter(tltext_lst));
   s_commandListMap->setAt(nameParameter(tltextpen_lst));
   s_commandListMap->setAt(nameParameter(design_objects_lst));
   s_commandListMap->setAt(nameParameter(library_lst));
   s_commandListMap->setAt(nameParameter(parameter_lst));
   s_commandListMap->setAt(nameParameter(current_parameter_lst));
   s_commandListMap->setAt(nameParameter(parametersets_lst));
   s_commandListMap->setAt(nameParameter(write_lst));
   s_commandListMap->setAt(nameParameter(pc_boards_lst));
   s_commandListMap->setAt(nameParameter(pc_board_lst));
   s_commandListMap->setAt(nameParameter(pens_lst));
   s_commandListMap->setAt(nameParameter(shapes_lst));
   s_commandListMap->setAt(nameParameter(symbols_lst));
   s_commandListMap->setAt(nameParameter(symbols_1_lst));
   s_commandListMap->setAt(nameParameter(tlshape_lst));
   s_commandListMap->setAt(nameParameter(packages_lst));
   s_commandListMap->setAt(nameParameter(tlpackage_lst));
   s_commandListMap->setAt(nameParameter(comp_package_lst));
   s_commandListMap->setAt(nameParameter(comp_package1_lst));
   s_commandListMap->setAt(nameParameter(components_lst));
   s_commandListMap->setAt(nameParameter(tlcomponent_lst));
   s_commandListMap->setAt(nameParameter(tlpin_lst));
   s_commandListMap->setAt(nameParameter(tlproperty_lst));
   s_commandListMap->setAt(nameParameter(tlvia_lst));
   s_commandListMap->setAt(nameParameter(tltestPoint_lst));   
   s_commandListMap->setAt(nameParameter(tlRefPads_lst));
   s_commandListMap->setAt(nameParameter(tlRefPad_lst));
   s_commandListMap->setAt(nameParameter(tlnet_lst));
   s_commandListMap->setAt(nameParameter(net_comp_lst));
   s_commandListMap->setAt(nameParameter(net_comppin_lst));
   s_commandListMap->setAt(nameParameter(phys_components_lst));
   s_commandListMap->setAt(nameParameter(phys_signals_lst));
   s_commandListMap->setAt(nameParameter(phys_comp_lst));
   s_commandListMap->setAt(nameParameter(phys_comp_placement_lst));
   s_commandListMap->setAt(nameParameter(phys_signal_lst));
   s_commandListMap->setAt(nameParameter(phys_copper_areas_lst));
   s_commandListMap->setAt(nameParameter(padstacks_lst));
   s_commandListMap->setAt(nameParameter(padstacks_1_lst));
   s_commandListMap->setAt(nameParameter(tlpadstack_lst));
}

#endif // EnableCommandStackTracing


LayerStruct* getLayer(int layerNumber,CCEtoODBDoc& camCadDoc)
{
   LayerStruct* layer = NULL;

   if (layerNumber >= 0 && layerNumber < camCadDoc.getLayerArray().GetSize())
   {
      layer = camCadDoc.getLayerArray().GetAt(layerNumber);
   }

   return layer;
}

void flattenInsert(CDataList& flatDataList,DataStruct& parentDataInsert,int parentLayer,CCEtoODBDoc& camCadDoc,const CTMatrix& parentMatrix)
{
   //CWriteFormat* writeFormat = getDebugWriteFormat();

   //if (writeFormat != NULL)
   //{
   //   writeFormat->writef("flattenInsert() -- Begin ----------------------------------\n");
   //   writeFormat->pushHeader("   ");
   //   parentDataInsert.dump(*writeFormat);
   //}

   CCamCadData& camCadData = camCadDoc.getCamCadData();

   InsertStruct* parentInsert = parentDataInsert.getInsert();
   BlockStruct* subBlock = camCadDoc.getBlockAt(parentInsert->getBlockNumber());

   CTMatrix matrix(parentMatrix);
   matrix.translateCtm(parentInsert->getOriginX(),parentInsert->getOriginY());
   matrix.rotateRadiansCtm(parentInsert->getAngle());
   matrix.scaleCtm(parentInsert->getGraphicMirrored() ? -1 : 1,1);

   // for debugging
   //CBasesVector basesVector;
   //basesVector.transform(matrix);
   //CPoint2d parentMatrixTranslation = basesVector.getOrigin();
   //double parentMatrixRotation      = basesVector.getRotation();
   //double parentMatrixScale         = basesVector.getScale();
   //bool parentMatrixMirror          = basesVector.getMirror();

   bool drillFlag = (subBlock->getBlockType() == blockTypeTooling ||
                     subBlock->getBlockType() == blockTypeDrillHole   );

   if (subBlock->isAperture() || drillFlag)
   {
      DataStruct* newData = camCadData.getNewDataStruct(parentDataInsert);
      newData->getInsert()->initializePosition();
      newData->transform(matrix);
      newData->setLayerIndex(parentLayer);
      flatDataList.AddTail(newData);
   }
   else
   {
      for (POSITION pos = subBlock->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = subBlock->getDataList().GetNext(pos);
         int dataLayer = (camCadDoc.IsFloatingLayer(data->getLayerIndex()) ? FloatingLayer : data->getLayerIndex());

         if (data->getDataType() == dataTypeInsert)
         {
            flattenInsert(flatDataList,*data,propagateLayer(parentLayer,dataLayer),camCadDoc,matrix);
         }
         else if (data->getDataType() != dataTypeBlob)
         {
            DataStruct* newData = camCadData.getNewDataStruct(*data);
            newData->transform(matrix);
            newData->setLayerIndex(propagateLayer(parentLayer,dataLayer));

            flatDataList.AddTail(newData);
         }
      }
   }

   //if (writeFormat != NULL)
   //{
   //   writeFormat->popHeader();
   //   writeFormat->writef("flattenInsert() -- End ----------------------------------\n");
   //   flushDebugWriteFormat();
   //}
}

void flattenPadstack(CDataList& flatDataList,BlockStruct& block,CCEtoODBDoc& camCadDoc)
{
   CCamCadData& camCadData = camCadDoc.getCamCadData();
   //CWriteFormat* writeFormat = getDebugWriteFormat();

   //if (writeFormat != NULL)
   //{
   //   writeFormat->writef("flattenPadstack() -- Begin ----------------------------------\n");
   //   writeFormat->pushHeader("   ");
   //   block.dump(*writeFormat);
   //}

   CTMatrix matrix;

   for (POSITION pos = block.getDataList().GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = block.getDataList().GetNext(pos);

      if (data->getDataType() == dataTypeInsert)
      {
         int dataLayer = (camCadDoc.IsFloatingLayer(data->getLayerIndex()) ? FloatingLayer : data->getLayerIndex());
         flattenInsert(flatDataList,*data,dataLayer,camCadDoc,matrix);
      }
      else
      {
         DataStruct* newData = camCadData.getNewDataStruct(*data);
         flatDataList.AddTail(newData);
      }
   }

   //if (writeFormat != NULL)
   //{
   //   writeFormat->popHeader();
   //   writeFormat->writef("flattenPadstack() -- End ----------------------------------\n");
   //   flushDebugWriteFormat();
   //}
}

BlockStruct& CThedaReader::getRestructuredPadStacksBlock()
{
   if (m_restructuredPadStacksBlock == NULL)
   {
      m_restructuredPadStacksBlock = &(getDefinedBlock("$thedaReaderRestructuredPadStacksBlock$",m_pcbFile->getFileNumber(),blockTypePadstack));
   }

   return *m_restructuredPadStacksBlock;
}

void CThedaReader::releaseRestructuredPadStacksBlock()
{
   if (m_restructuredPadStacksBlock != NULL)
   {
      m_camCadDoc.RemoveBlock(m_restructuredPadStacksBlock);
   }

   m_restructuredPadStacksBlock = NULL;
}

void CThedaReader::restructurePadStacks()
{
   int maxBlockIndex = m_camCadDoc.getMaxBlockIndex();

   for (int blockIndex = 0;blockIndex < maxBlockIndex;blockIndex++)
   {
      BlockStruct* padStack = m_camCadDoc.getBlockAt(blockIndex);

      if (padStack != NULL && padStack->getBlockType() == blockTypePadstack)
      {
         restructurePadStack(*padStack);
      }
   }
}

void CThedaReader::restructurePadStack(BlockStruct& block)
{
   //CWriteFormat* writeFormat = getDebugWriteFormat();
   //CWriteFormat* writeFormat = NULL;

   //if (writeFormat != NULL)
   //{
   //   writeFormat->writef("restructurePadStack() -- Begin ----------------------------------\n");
   //   writeFormat->writef("padStackBlockName='%s'\n",(const char*)padStackBlockName);
   //   writeFormat->writef("cur_blockname='%s'\n",(const char*)cur_blockname);
   //   block->dump(*writeFormat);
   //}

   int floatingLayerIndex = m_camCadDoc.getDefinedFloatingLayer()->getLayerIndex();
   CTypedPtrArrayContainer<CDataList*> layeredDataLists;

   // the new DataStructs will be contained in the CDataList elements in the layeredDataLists variable
   CDataList flatDataList(false);

   CTMatrix matrix;
   flattenPadstack(flatDataList,block,m_camCadDoc);

   DataStruct* data;

   for (POSITION pos = flatDataList.GetHeadPosition();pos != NULL;)
   {
      data = flatDataList.GetNext(pos);

      int layerIndex = data->getLayerIndex() + 2;

      if (layerIndex < 0) layerIndex = 0;

      CDataList* dataListForLayer = NULL;

      if (layerIndex < layeredDataLists.GetSize())
      {
         dataListForLayer = layeredDataLists.GetAt(layerIndex);
      }

      if (dataListForLayer == NULL)
      {
         dataListForLayer = new CDataList(true);
         layeredDataLists.setAtGrow(layerIndex,dataListForLayer);
      }

      dataListForLayer->AddTail(data);
   }

   BlockStruct& newBlock = getRestructuredPadStacksBlock();
   Graph_Block_On(&newBlock);

   CString apertureName;

   for (int ind = 0;ind < layeredDataLists.GetSize();ind++)
   {
      CDataList* dataListForLayer = layeredDataLists.GetAt(ind);

      if (dataListForLayer != NULL)
      {
         bool singleApertureFlag = false;
         int layerNumber = ind - 2;

         //if (writeFormat != NULL)
         //{
         //   writeFormat->writef("restructurePadStack() -- DataList for layerNumber=%d ----------\n",layerNumber);
         //   dataListForLayer->dump(*writeFormat);
         //}

         int apertureIndex;

         if (dataListForLayer->GetCount() == 1)
         {
            data = dataListForLayer->GetHead();

            if (data->getDataType() == T_INSERT)
            {
               InsertStruct* apertureInsert = data->getInsert();
               BlockStruct* apertureBlock = m_camCadDoc.Find_Block_by_Num(apertureInsert->getBlockNumber());

               if ( apertureBlock != NULL )
               {
                  bool drillFlag = (apertureBlock->getBlockType() == BLOCKTYPE_TOOLING ||
                                    apertureBlock->getBlockType() == BLOCKTYPE_DRILLHOLE   );

                  if ( apertureBlock->isAperture() || drillFlag )
                  {
                     singleApertureFlag = true;

                     Graph_Block_Reference(apertureBlock->getName(),NULL,apertureBlock->getFileNumber(),
                        apertureInsert->getOriginX(),apertureInsert->getOriginY(),apertureInsert->getAngle(),
                        apertureInsert->getMirrorFlags(),apertureInsert->getScale(),layerNumber,0);
                  }
               }
            }
         }

         if (!singleApertureFlag)
         {
            CString apertureBaseName;
            apertureBaseName.Format("AP_%s_%d",(const char*)block.getName(),layerNumber);
            apertureName = apertureBaseName;

            for (int suffix = 1;;suffix++)
            {
               if (!Graph_Block_Exists(&m_camCadDoc,apertureName,-1))
               {
                  break;
               }

               apertureName.Format("%s-%d",(const char*)apertureBaseName,suffix);
            }

            BlockStruct* complexApertureBlock = &(getThedaReader().graphBlockOn(apertureName,-1,blockTypeUnknown));
            Graph_Block_Off();

            complexApertureBlock->getDataList().takeData(*dataListForLayer);
            complexApertureBlock->setBlockType(BLOCKTYPE_PADSHAPE);

            for (CDataListIterator dataIterator(*complexApertureBlock);dataIterator.hasNext();)
            {
               DataStruct* data = dataIterator.getNext();
               data->setLayerIndex(floatingLayerIndex);
            }

            int err;
            apertureIndex = Graph_Aperture(apertureName,T_COMPLEX,complexApertureBlock->getBlockNumber(),0.,0.,0.,0.,0,0,0,&err);

            Graph_Block_Reference(apertureName,NULL,block.getFileNumber(),0.,0.,0.,0,1.,layerNumber,0);
         }
      }
   }

   Graph_Block_Off();

   block.getDataList().empty();
   block.getDataList().takeData(newBlock.getDataList());
}

//_____________________________________________________________________________
void AText::setHorizontalAlignment(char horizontalAlignment)
{
   m_horizontalAlignment = horizontalAlignment;

   switch (m_horizontalAlignment)
   {
   case 'L':  m_horizontalPosition = horizontalPositionLeft;    break;
   case 'M':  m_horizontalPosition = horizontalPositionCenter;  break;
   case 'R':  m_horizontalPosition = horizontalPositionRight;   break;
   case 'N':
   default:   m_horizontalPosition = horizontalPositionLeft;    break;
   }
}

void AText::setVerticalAlignment(char verticalAlignment)
{
   m_verticalAlignment = verticalAlignment;

   switch (m_verticalAlignment)
   {
   case 'B':  m_verticalPosition = verticalPositionBottom;  break;
   case 'M':  m_verticalPosition = verticalPositionCenter;  break;
   case 'T':  m_verticalPosition = verticalPositionTop;     break;
   case 'N':
   default:   m_verticalPosition = verticalPositionBottom;  break;
   }
}

//_____________________________________________________________________________
CThedaTableParser::CThedaTableParser()
{
}

bool CThedaTableParser::parse()
{
   bool retval = false;

   CSupString line;
   CStringArray params;

   if (get_line(cur_line,MAX_LINE))
   {
      line = cur_line;

      m_numColumns = line.Parse(params);

      for (int columnIndex = 0;columnIndex < m_numColumns;columnIndex++)
      {
         CString columnName = params.GetAt(columnIndex);
         columnName.MakeLower();

         m_columnNames.SetAtGrow(columnIndex,columnName);

         //int (*handler)() = NULL;

         //m_handlersByColumnName.Lookup(columnName,handler);
         //m_handlersByColumnIndex.SetAtGrow(columnIndex,handler);
      }

      processColumns();
   }

   if (get_line(cur_line,MAX_LINE))
   {
      retval = (strncmp(cur_line,"#------",7) == 0);
   }

   while (get_line(cur_line,MAX_LINE))
   {
      if (strncmp(cur_line,"#------",7) == 0)
      {
         get_newline();
         break;
      }

      line = cur_line;

      int numPar = line.ParseQuote(m_rowValues," ");

      if (retval && (numPar == m_numColumns))
      {
         retval = processRow() && retval;
      }
      else
      {
         retval = false;
      }
   }

   return retval;
}

bool CThedaTableParser::processColumns()
{
   return true;
}

bool CThedaTableParser::processRow()
{
   return true;
}

//_____________________________________________________________________________
CThedaNetListTableParser::CThedaNetListTableParser()
{
   m_referenceDesignatorColumn = -1;
   m_pinIdColumn               = -1;
   m_seqNoColumn               = -1;
   m_schSymbolIdColumn         = -1;
   m_schPinIdColumn            = -1;
   m_probeRefDesColumn         = -1;
   m_probeColumn               = -1;
}

bool CThedaNetListTableParser::processColumns()
{
   for (int columnIndex = 0;columnIndex < m_numColumns;columnIndex++)
   {
      CString columnName = m_columnNames.GetAt(columnIndex);

      if (columnName.CompareNoCase("REFERENCE_DESIGNATOR") == 0)
      {
         m_referenceDesignatorColumn = columnIndex;
      }
      else if (columnName.CompareNoCase("PIN_ID") == 0)
      {
         m_pinIdColumn = columnIndex;
      }
      else if (columnName.CompareNoCase("SEQ_NO") == 0)
      {
         m_seqNoColumn = columnIndex;
      }
      else if (columnName.CompareNoCase("SCH_SYMBOL_ID") == 0)
      {
         m_schSymbolIdColumn = columnIndex;
      }
      else if (columnName.CompareNoCase("SCH_PIN_ID") == 0)
      {
         m_schPinIdColumn = columnIndex;
      }
      else if (columnName.CompareNoCase("PROBE_REF_DES") == 0)
      {
         m_probeRefDesColumn = columnIndex;
      }
      else if (columnName.CompareNoCase("PROBE") == 0)
      {
         m_probeColumn = columnIndex;
      }
   }

   return true;
}

bool CThedaNetListTableParser::processRow()
{
   bool retval = true;

   if (m_probeRefDesColumn >= 0 && m_probeColumn >= 0)
   {
      generateProbeReference(*doc,file,
         m_rowValues.GetAt(m_probeRefDesColumn),
         m_rowValues.GetAt(m_probeColumn),
         cur_netname);
   }

   if (m_referenceDesignatorColumn >= 0 && m_pinIdColumn >= 0)
   {
      cur_comppin.compname = m_rowValues.GetAt(m_referenceDesignatorColumn);
      cur_comppin.pinname = m_rowValues.GetAt(m_pinIdColumn);

      //NetStruct* n = add_net(file, cur_netname);

   // here netlist add_n
      if (strlen(cur_netname) > 0 && strlen(cur_comppin.compname) > 0 && strlen(cur_comppin.pinname) > 0)
      {
         if (!already_comppin(file, cur_netname, cur_comppin.compname, cur_comppin.pinname))
         {
            if (!comp_pulled_pin(cur_comppin.compname, cur_comppin.pinname))
            {
               getThedaReader().getThedaComponentPins().addComponentPin(cur_netname,cur_comppin.compname,cur_comppin.pinname);
               //add_comppin(file, n, cur_comppin.compname, cur_comppin.pinname);
            }
         }
      }
   }

   return retval;
}

//void CThedaTableParser::addColumnHandler(const char* columnName,int (*function)())
//{
//   m_handlersByColumnName.SetAt(columnName,function);
//}

//_____________________________________________________________________________
CThedaPartialVia::CThedaPartialVia(const CString& id,const CString& layersDescriptor,
   const CString& fullViaStackName,BlockStruct* viaBlock) :
      m_id(id),m_layersDescriptor(layersDescriptor),m_fullViaStackName(fullViaStackName)
{
}

CString CThedaPartialVia::getViaStackName() const
{
   return QPadStackPrefix + m_id;
}

//_____________________________________________________________________________
CThedaPartialVias* CThedaPartialVias::m_thedaPartialVias = NULL;

CThedaPartialVias& CThedaPartialVias::getThedaPartialVias(CCEtoODBDoc& camCadDoc,int fileNum)
{
   if (m_thedaPartialVias == NULL)
   {
      m_thedaPartialVias = new CThedaPartialVias(camCadDoc,fileNum);
   }

   return *m_thedaPartialVias;
}

void CThedaPartialVias::releasePartialVias()
{
   delete m_thedaPartialVias;
   m_thedaPartialVias = NULL;
}

CThedaPartialVias::CThedaPartialVias(CCEtoODBDoc& camCadDoc,int fileNum) :
   m_camCadDoc(camCadDoc),
   m_fileNum(fileNum)
{
}

CThedaPartialVia* CThedaPartialVias::getDefinedPartialVia(const CString& layersDescriptor,
    const CString& fullViaStackBaseName)
{
   CSupString descriptor(layersDescriptor);
   descriptor.Trim();
   CString startLayer = descriptor.Scan(",");
   CString endLayer;

   while (! descriptor.IsEmpty())
   {
      endLayer = descriptor.Scan(",");
   }

   CString id;
   id.Format("%s_(%s_%s)",(const char*)fullViaStackBaseName,(const char*)startLayer,(const char*)endLayer);

   CString viaStackName = getThedaReader().getPadStackTemplateGeometryName(id);

   CThedaPartialVia* partialVia = NULL;

   if (! m_partialVias.Lookup(id,partialVia))
   {
      BlockStruct* viaBlock = Graph_Block_Exists(&m_camCadDoc,viaStackName,m_fileNum);

      if (viaBlock == NULL)
      {
         viaBlock = &(getThedaReader().graphBlockOn(viaStackName,m_fileNum,blockTypePadstack));
         //viaBlock = Graph_Block_On(GBO_APPEND,viaStackName,m_fileNum,0);
         //viaBlock->setBlockType(BLOCKTYPE_PADSTACK);
         viaBlock->setOriginalName(id);

         Graph_Block_Off();
      }

      partialVia = new CThedaPartialVia(id,layersDescriptor,getThedaReader().getPadStackTemplateGeometryName(fullViaStackBaseName),viaBlock);
      m_partialVias.SetAt(id,partialVia);
   }

   return partialVia;
}

void CThedaPartialVias::generatePartialVias()
{
   CString id;
   CThedaPartialVia* partialVia;
   //CDataList* savedCurrentDataList = GetCurrentDataList();
   int blindShapeKeywordIndex = m_camCadDoc.IsKeyWord(ATT_BLINDSHAPE,TRUE);

   for (POSITION pos = m_partialVias.GetStartPosition();pos != NULL;)
   {
      m_partialVias.GetNextAssoc(pos,id,partialVia);

      //SetCurrentDataList(&(partialVia->getViaBlock()->DataList));
      BlockStruct* viaBlock = &(getThedaReader().graphBlockOn(partialVia->getViaStackName(),m_fileNum,blockTypeUnknown));
      BlockStruct* fullVia =
         Graph_Block_Exists(&m_camCadDoc,partialVia->getFullViaStackName(),m_fileNum);
      Create_LayerrangePADSTACKData(&(fullVia->getDataList()),partialVia->getLayersDescriptor());
      Graph_Block_Off();

      doc->SetAttrib(&(viaBlock->getAttributesRef()),blindShapeKeywordIndex,
         VT_NONE,NULL,SA_OVERWRITE, NULL);
   }

   //SetCurrentDataList(savedCurrentDataList);
}

//_____________________________________________________________________________
CThedaComponentPin::CThedaComponentPin(const CString& netName,const CString& refDes,
                                       const CString& pinName,CompPinStruct* compPin)
: m_netName(netName)
, m_refDes(refDes)
, m_pinName(pinName)
, m_compPin(compPin)
{
}

//_____________________________________________________________________________
CThedaComponentPins::CThedaComponentPins()
{
   m_componentPins.InitHashTable(nextPrime2n(1000),false);
}

CThedaComponentPin& CThedaComponentPins::addComponentPin(const CString& netName,const CString& refDes,const CString& pinName)
{
   CThedaComponentPin* componentPin = NULL;
   CString pinRef = refDes + "." + pinName;

   m_componentPins.Lookup(pinRef,componentPin);

   if (componentPin != NULL && componentPin->getNetName().CompareNoCase(netName) != 0)
   {
      fprintf(logFp, 
         "Pinref '%s' was previousely assigned to signal '%s', ignoring new signal assignment of '%s'.\n",
         pinRef,componentPin->getNetName(),netName);
   }

   if (componentPin == NULL)
   {
      NetStruct* net = add_net(file,netName);
      CompPinStruct* compPin = add_comppin(file,net,refDes,pinName);

      componentPin = new CThedaComponentPin(netName,refDes,pinName,compPin);
      m_componentPins.SetAt(pinRef,componentPin);
   }

   return *componentPin;
}

//_____________________________________________________________________________
CThedaTestProbe::CThedaTestProbe(const CString& refDes,DataStruct* probeInsertData) :
   m_refDes(refDes),
   m_probeInsertData(probeInsertData)
{
}

//_____________________________________________________________________________
CThedaTestProbes::CThedaTestProbes()
{
   m_testProbes.InitHashTable(nextPrime2n(1000),false);
}

CThedaTestProbe& CThedaTestProbes::addTestProbe(const CString& refDes,DataStruct* probeInsertData)
{
   CThedaTestProbe* testProbe = NULL;

   m_testProbes.Lookup(refDes,testProbe);

   if (testProbe == NULL)
   {
      testProbe = new CThedaTestProbe(refDes,probeInsertData);
      m_testProbes.SetAt(refDes,testProbe);
   }

   return *testProbe;
}

CThedaTestProbe* CThedaTestProbes::getTestProbe(const CString& refDes)
{
   CThedaTestProbe* testProbe = NULL;

   if (!m_testProbes.Lookup(refDes,testProbe))
   {
      testProbe = NULL;
   }

   return testProbe;
}

//_____________________________________________________________________________
CPinNameWithContactLayer::CPinNameWithContactLayer(const CString& pinName,const CString& contactLayerName,
   const CString& mirroredContactLayerName)
: m_pinName(pinName)
, m_contactLayerName(contactLayerName)
, m_mirroredContactLayerName(mirroredContactLayerName)
{
}

CString CPinNameWithContactLayer::getContactLayerName(bool mirrorFlag) const
{
   CString contactLayerName = (mirrorFlag ? m_mirroredContactLayerName : m_contactLayerName);

   return contactLayerName;
}

void CPinNameWithContactLayer::setContactLayerNames(const CString& contactLayerName,const CString& mirroredContactLayerName)
{
   m_contactLayerName         = contactLayerName;
   m_mirroredContactLayerName = mirroredContactLayerName;
}

//_____________________________________________________________________________
int sortByContactLayerAndPinName(const void* element0,const void* element1)
{
   const CPinNameWithContactLayer* pinNameWithContactLayer0 = (CPinNameWithContactLayer*)(((SElement*)element0)->pObject->m_object);
   const CPinNameWithContactLayer* pinNameWithContactLayer1 = (CPinNameWithContactLayer*)(((SElement*)element1)->pObject->m_object);

   int retval = pinNameWithContactLayer0->getContactLayerName().Compare(pinNameWithContactLayer1->getContactLayerName());

   if (retval == 0)
   {
      retval = pinNameWithContactLayer0->getPinName().Compare(pinNameWithContactLayer1->getPinName());
   }

   return retval;
}

CPinNameToContactLayerMap::CPinNameToContactLayerMap()
{
   m_map.InitHashTable(nextPrime2n(50));
   m_map.setSortFunction(sortByContactLayerAndPinName);
}

void CPinNameToContactLayerMap::empty()
{
   m_map.empty();
}

void CPinNameToContactLayerMap::addPin(const CString& pinName,const CString& contactLayerName,const CString& mirroredContactLayerName)
{
   CPinNameWithContactLayer* pinNameWithContactLayer;

   if (!m_map.Lookup(pinName,pinNameWithContactLayer))
   {
      CString normalizedLayerName         = getThedaReader().getNormalizedLayerName(contactLayerName);
      CString normalizedMirroredLayerName = getThedaReader().getNormalizedLayerName(mirroredContactLayerName);

      pinNameWithContactLayer = new CPinNameWithContactLayer(pinName,normalizedLayerName,normalizedMirroredLayerName);

      m_map.SetAt(pinName,pinNameWithContactLayer);
   }
}

void CPinNameToContactLayerMap::changePinToThruHole(const CString& pinName)
{
   CPinNameWithContactLayer* pinNameWithContactLayer;
   CString descriptor = getDescriptor(); // for debug

   if (m_map.Lookup(pinName,pinNameWithContactLayer))
   {
      pinNameWithContactLayer->setContactLayerNames(QComponentSideNormalizedLayerName,QSolderSideNormalizedLayerName);
   }
}

bool CPinNameToContactLayerMap::lookup(const CString& pinName,CString& contactLayerName)
{
   CPinNameWithContactLayer* pinNameWithContactLayer;

   bool retval = (m_map.Lookup(pinName,pinNameWithContactLayer) != 0);

   if (retval)
   {
      contactLayerName = pinNameWithContactLayer->getContactLayerName();
   }

   return retval;
}

CString CPinNameToContactLayerMap::getSignature(bool mirrorFlag)
{
   CPinNameWithContactLayer* pinNameWithContactLayer;
   CString* key;
   CString layerName;
   CString descriptor;
   bool allComponentSideLayerFlag = true;
   bool allSolderSideLayerFlag    = true;

   for (m_map.GetFirstSorted(key,pinNameWithContactLayer);pinNameWithContactLayer != NULL;
        m_map.GetNextSorted(key,pinNameWithContactLayer))
   {
      layerName = pinNameWithContactLayer->getContactLayerName(mirrorFlag);
      descriptor += layerName + ":" + pinNameWithContactLayer->getPinName() + " ";

      if (allComponentSideLayerFlag)
      {
         allComponentSideLayerFlag = (layerName.CompareNoCase(QComponentSideNormalizedLayerName) == 0);
      }

      if (allSolderSideLayerFlag)
      {
         allSolderSideLayerFlag = (layerName.CompareNoCase(QSolderSideNormalizedLayerName) == 0);
      }
   }

   CString signature;

   if (allComponentSideLayerFlag || allSolderSideLayerFlag)
   {
      signature = QTopBuiltGeometrySuffix;
   }
   //else if (allSolderSideLayerFlag)
   //{
   //   signature = QBottomBuiltGeometrySuffix;
   //}
   else
   {
      CCrcCalculator crcCalculator;
      crcCalculator.input(descriptor);

      signature.Format("%08x",crcCalculator.getCrc());
   }

   return signature;
}

CString CPinNameToContactLayerMap::getSignatureInfix(bool mirrorFlag)
{
   CString signature = getSignature(mirrorFlag);

   if (!signature.IsEmpty())
   {
      signature = "_" + signature;
   }

   return signature;
}

CString CPinNameToContactLayerMap::getDescriptor()
{
   CString descriptor;
   CPinNameWithContactLayer* pinNameWithContactLayer;
   CString layerName;
   CString* key;

   for (m_map.GetFirstSorted(key,pinNameWithContactLayer);pinNameWithContactLayer != NULL;
        m_map.GetNextSorted(key,pinNameWithContactLayer))
   {
      layerName = pinNameWithContactLayer->getContactLayerName();
      descriptor += layerName + ":" + pinNameWithContactLayer->getPinName() + " ";
   }

   return descriptor;
}

//_____________________________________________________________________________
TLNetlistComp::TLNetlistComp()
: pin_ids_physical(FALSE)
, comptype(ATT_DEVICETYPE_UNKNOWN)
, m_componentData(NULL)
{
}

void TLNetlistComp::emptyPinNameToContactLayerMap()
{
   m_pinNameToContactLayerMap.empty();
}

void TLNetlistComp::addPin(const CString& pinName,CString contactLayerName,CString mirroredContactLayerName)
{
   if (contactLayerName.IsEmpty())
   {
      contactLayerName = QComponentSideNormalizedLayerName;
   }

   if (mirroredContactLayerName.IsEmpty())
   {
      mirroredContactLayerName = QSolderSideNormalizedLayerName;
   }

   m_pinNameToContactLayerMap.addPin(pinName,contactLayerName,mirroredContactLayerName);
}

CString TLNetlistComp::getComponentLayerName() const
{
   return m_componentLayerName;
}

void TLNetlistComp::setComponentLayerName(const CString& layerName)
{
   m_componentLayerName = layerName;
}

DataStruct* TLNetlistComp::getComponentData() const
{
   return m_componentData;
}

void TLNetlistComp::setComponentData(DataStruct* componentData)
{
   m_componentData = componentData;
}

//_____________________________________________________________________________
TLComp::TLComp()
: m_component(NULL)
{
   initialize();
}

TLComp::~TLComp()
{
   releaseData();
}

void TLComp::releaseData()
{
   delete m_component;
   m_component = NULL;
}

void TLComp::initialize()
{
   refdes.Empty();

   already_placed = false;
   placed         = false;

   mir      = 0;
   x_mirror = 0;
   y_mirror = 0;
   rot      = 0;

   m_netlistComp = NULL;
   m_instanceSpecificGeometry = NULL;
   //m_component->attributes().empty();
}

DataStruct& TLComp::getComponent()
{
   if (m_component == NULL)
   {
      m_component = getCamCadData().getNewDataStruct(dataTypeInsert);
   }

   return *m_component;
}

TLNetlistComp* TLComp::getNetlistComp() const
{
   return m_netlistComp;
}

void TLComp::setNetlistComp(TLNetlistComp* netlistComp)
{
   m_netlistComp = netlistComp;

   if (m_netlistComp != NULL)
   {
      m_netlistComp->setComponentLayerName(m_componentLayerName);
   }
}

CString TLComp::getComponentLayerName() const
{
   return m_componentLayerName;
}

void TLComp::setComponentLayerName(const CString& layerName)
{
   m_componentLayerName = layerName;
}

void TLComp::setInstanceSpecificGeometry(BlockStruct* instanceSpecificGeometry)
{
   m_instanceSpecificGeometry = instanceSpecificGeometry;
}

//_____________________________________________________________________________
int CThedaReader::m_pinNameToContactLayerMapDescriptorKeywordIndex = -1;

CThedaReader::CThedaReader(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_pcbFile(NULL)
, m_floatingLayer(NULL)
, m_constructingComponentGeometryFlag(false)
, m_constructingPadstackGeometryFlag(false)
, m_viaDataList(false)
, m_testPointDataList(false)
, m_referencePadDataList(false)
, m_messageFilter(NULL)
, m_restructuredPadStacksBlock(NULL)
, m_optionAdjustLayerNames(false)
, m_optionRectifyComponents(true)
, m_optionRectifyViasAndTestPoints(true)
, m_optionPurgeUnusedWidthsAndGeometries(true)
, m_optionPurgeUnusedLayers(true)
, m_optionFixupGeometryNames(true)
{
   m_blockMap.InitHashTable(nextPrime2n(8000),false);

   m_pinNameToContactLayerMapDescriptorKeywordIndex = -1;
}

CThedaReader::~CThedaReader()
{
   releaseRestructuredPadStacksBlock();

	delete m_messageFilter;
}

CMessageFilter& CThedaReader::getMessageFilter()
{
   if (m_messageFilter == NULL)
   {
      m_messageFilter = new CMessageFilter(messageFilterTypeMessage);
   }

   return *m_messageFilter;
}

void CThedaReader::setConstructingComponentGeometryFlag(bool flag)
{
   m_constructingComponentGeometryFlag = flag;
}

void CThedaReader::setConstructingPadstackGeometryFlag(bool flag)
{
   m_constructingPadstackGeometryFlag = flag;
}

//CString CThedaReader::getSpecificLayerName(CString layerName)
//{
//   layerName = getNormalizedLayerName(layerName);
//
//
//}

CString CThedaReader::adjustCase(const CString& string) const
{
   CString adjustedCaseString(string);
   bool alphaFlag = false;
   char* pp  = NULL;
   char* ppp = NULL;

   for (char* p = adjustedCaseString.GetBuffer();*p != '\0';p++)
   {
      if (isalpha(*p))
      {
         *p = toupper(*p);

         bool specialFlag = (ppp != NULL && *ppp == '_' && pp != NULL && (*pp == 'C' || *pp == 'S') &&
                             *p == 'S');

         if (alphaFlag && !specialFlag)
         {
            *p = tolower(*p);
         }
      }

      alphaFlag = (isalpha(*p) != 0);
      ppp = pp;
      pp  = p;
   }


   return adjustedCaseString;
}

LayerStruct& CThedaReader::getDefinedLayer(const CString& layerName) const
{
   return getDefinedLayer(layerName,getFloatToLayerName());
}

LayerStruct& CThedaReader::getDefinedLayer(CString layerName,const CString& floatToLayerName) const
{
   static bool oldBehaviorFlag = false;
   LayerStruct* layer = NULL;

   if (m_optionAdjustLayerNames)
   {
      layerName = adjustCase(layerName);
   }

   bool componentSideFlag   = isComponentSideName(floatToLayerName);
   bool solderSideFlag      = isSolderSideName(floatToLayerName);

   if (m_constructingComponentGeometryFlag || m_constructingPadstackGeometryFlag || oldBehaviorFlag)
   {
      layer = &(m_camCadDoc.getDefinedLayer(layerName));
   }
   else
   {
      CSupString layerNameString(layerName);
      CString layerNamePrefix = layerNameString.Scan(":");

      for (int pass = 1;;pass++)
      {
         if (pass > 2)
         {
            layer = &(m_camCadDoc.getDefinedLayer(layerName));
            break;
         }

         CString suffix((pass == 1) ? Q_PS : Q_OPS);
         int suffixLen = suffix.GetLength();

         if (layerNamePrefix.GetLength() > suffixLen && layerNamePrefix.Right(suffixLen).CompareNoCase(suffix) == 0)
         {
            CString specificLayerName = layerNamePrefix.Mid(0,layerNamePrefix.GetLength() - suffixLen) + 
                                       getNormalizedInfixLayerName(floatToLayerName,pass == 2);

            if (layerNameString.GetLength() > 0)
            {
               specificLayerName += ":" + layerNameString;
            }

            layer = &(m_camCadDoc.getDefinedLayer(specificLayerName));
            break;
         }
      }
   }

   if (componentSideFlag || solderSideFlag)
   {
      if (layer->getMirroredLayerIndex() == layer->getLayerIndex())
      {
         CString underscore("_");
         CString sideName      = underscore + (componentSideFlag ? QComponentSideNormalizedLayerName : QSolderSideNormalizedLayerName   );
         CString otherSideName = underscore + (componentSideFlag ? QSolderSideNormalizedLayerName    : QComponentSideNormalizedLayerName);
         CString layerName     = layer->getName();

         layerName.Replace(sideName,otherSideName);

         LayerStruct* mirroredLayer = &(m_camCadDoc.getDefinedLayer(layerName));

         m_camCadDoc.Graph_Level_Mirror(layer->getName(),mirroredLayer->getName(),NULL);
      }
   }

   return *layer;
}

int CThedaReader::getDefinedLayerIndex(const CString& layerName) const
{
   return getDefinedLayer(layerName).getLayerIndex();
}

int CThedaReader::getFloatingLayerIndex()
{
   if (m_floatingLayer == NULL)
   {
      int floatingLayerIndex = Graph_Level("0","",TRUE);
      m_floatingLayer = m_camCadDoc.getLayerAt(floatingLayerIndex);
   }

   return m_floatingLayer->getLayerIndex();
}

CString CThedaReader::getFloatToLayerName() const
{
   CString floatToLayerName;

   if (! m_contactLayerName.IsEmpty())
   {
      floatToLayerName = m_contactLayerName;
   }
   else if (! m_componentPlacementLayerName.IsEmpty())
   {
      floatToLayerName = m_componentPlacementLayerName;
   }
   else
   {
      floatToLayerName = "DefaultFloatToLayer";
   }

   return floatToLayerName;
}

bool CThedaReader::isComponentSideName(const CString& name) const
{
   bool nameFlag = (name.CompareNoCase(QComp) == 0 || name.CompareNoCase(QComponentSide) == 0);

   return nameFlag;
}

bool CThedaReader::isSolderSideName(const CString& name) const
{
   bool nameFlag = (name.CompareNoCase(QSolderSide) == 0);

   return nameFlag;
}

CString CThedaReader::getNormalizedLayerName(const CString layerName) const
{
   CString normalizedLayerName(layerName);

   if (isComponentSideName(normalizedLayerName))
   {
      normalizedLayerName = QComponentSideNormalizedLayerName;
   }
   else if (isSolderSideName(normalizedLayerName))
   {
      normalizedLayerName = QSolderSideNormalizedLayerName;
   }

   return normalizedLayerName;
}

CString CThedaReader::getDenormalizedLayerName(const CString layerName) const
{
   CString denormalizedLayerName(layerName);

   if (layerName.CompareNoCase(QComponentSideNormalizedLayerName) == 0)
   {
      denormalizedLayerName = QComponentSide;
   }
   else if (layerName.CompareNoCase(QSolderSideNormalizedLayerName) == 0)
   {
      denormalizedLayerName = QSolderSide;
   }

   return denormalizedLayerName;
}

void CThedaReader::setComponentPlacementLayerName(const CString& layerName)
{
   m_componentPlacementLayerName.Empty();

   if (m_componentPlacementLayerName.Find(Q_PS ) >= 0 ||
       m_componentPlacementLayerName.Find(Q_OPS) >= 0    )
   {
		getMessageFilter().formatMessageBoxApp("Floating layer specified for component placement layer.");
		fprintf(logFp, "Floating layer specified for component placement layer.\n");
   }
   else
   {
      m_componentPlacementLayerName = layerName;
   }

   if (m_componentPlacementLayerName.IsEmpty())
   {
      m_componentPlacementLayerName = "DefaultComponentPlacementLayer";
   }
}

void CThedaReader::clearComponentPlacementLayerName()
{
   m_componentPlacementLayerName.Empty();
   clearContactLayerName();
}

void CThedaReader::setContactLayerName(const CString& layerName)
{
   m_contactLayerName.Empty();

   if (m_contactLayerName.Find(Q_PS ) >= 0 ||
       m_contactLayerName.Find(Q_OPS) >= 0    )
   {
		getMessageFilter().formatMessageBoxApp("Floating layer specified for contact layer.");
		fprintf(logFp, "Floating layer specified for contact layer.\n");
   }
   else
   {
      m_contactLayerName = layerName;
   }

   if (m_contactLayerName.IsEmpty())
   {
      m_contactLayerName = "DefaultContactLayer";
   }
}

void CThedaReader::clearContactLayerName()
{
   m_contactLayerName.Empty();
}

CString CThedaReader::getCurrentSymbolTypeString() const
{
   return m_currentSymbolTypeString;
}

void CThedaReader::setCurrentSymbolTypeString(const CString& currentSymbolTypeString)
{
   m_currentSymbolTypeString = currentSymbolTypeString;
}

BlockStruct& CThedaReader::getDefinedBlock(const CString& blockName,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct* block;

   if (! m_blockMap.Lookup(blockName,block))
   {   
      block = Graph_Block_On(GBO_APPEND,blockName,fileNumber,0,blockType);
      Graph_Block_Off();

      m_blockMap.SetAt(blockName,block);
   }

   return *block;
}

BlockStruct& CThedaReader::graphBlockOn(const CString& blockName,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct& block = getDefinedBlock(blockName,fileNumber,blockType);

   Graph_Block_On(&block);

   return block;
}

CString CThedaReader::getRectifiedPackageId(const CString& packageId,
   const CString& componentLayerName,CPinNameToContactLayerMap& pinNameToContactLayerMap)
{
   bool mirrorFlag = isSolderSideName(componentLayerName);

   CString pcbComponentTemplatePrefix(QPcbComponentTemplatePrefix);
   CString idealPackageId = packageId + getPlacementSpecificInfixLayerName(componentLayerName);
   CString rectifiedPackageId = idealPackageId + pinNameToContactLayerMap.getSignatureInfix(mirrorFlag);

   if (rectifiedPackageId.Find(pcbComponentTemplatePrefix,0) == 0)
   {
      rectifiedPackageId = rectifiedPackageId.Mid(pcbComponentTemplatePrefix.GetLength());
   }
   else
   {
      int iii = 3;
   }

   return rectifiedPackageId;
}

void CThedaReader::addPinNameToContactLayerMapDescriptorAttribute(DataStruct& component,const CString& descriptor)
{
   if (m_pinNameToContactLayerMapDescriptorKeywordIndex < 0)
   {
      m_pinNameToContactLayerMapDescriptorKeywordIndex = m_camCadDoc.RegisterKeyWord("PinNameToContactLayerMapDescriptor",valueTypeString);
   }

   m_camCadDoc.SetAttrib(&(component.getAttributesRef()),m_pinNameToContactLayerMapDescriptorKeywordIndex,descriptor);
}

int CThedaReader::getDrillHoleLayerIndex() const
{
   return getDefinedLayerIndex("DRILLHOLE");
}

bool CThedaReader::isThruHolePadStackGeometry(BlockStruct& geometry) const
{
   bool retval = false;
   int drillHoleLayerIndex = getDrillHoleLayerIndex();

   for (CDataListIterator insertIterator(geometry,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* insertData = insertIterator.getNext();

      if (insertData->getLayerIndex() == drillHoleLayerIndex)
      {
         retval = true;

         break;
      }
   }

   return retval;
}

//void CThedaReader::updateComponentGeometryContactLayerMap(const CString& packageId,
//   const CString& componentLayerName,CPinNameToContactLayerMap& pinNameToContactLayerMap)
//{
//}

BlockStruct* CThedaReader::getRectifiedComponentGeometry(const CString& packageId,
   const CString& componentLayerName,CPinNameToContactLayerMap& pinNameToContactLayerMap)
{
   BlockStruct* embeddedPackageGeometry = NULL;

   CString rectifiedPackageId = getRectifiedPackageId(packageId,componentLayerName,pinNameToContactLayerMap);

   if (! m_blockMap.Lookup(rectifiedPackageId,embeddedPackageGeometry))
   {
      BlockStruct* packageGeometry;

      if (! m_blockMap.Lookup(packageId,packageGeometry))
      {
         BlockStruct* foundBlock = m_camCadDoc.Find_Block_by_Name(packageId,-1);

			getMessageFilter().formatMessageBoxApp("Imported file does not have geometry definitions for all component inserts. Please see the theda.log file for details.");
			fprintf(logFp, "Could not find geometry '%s' for embedded package.\n",packageId); 
      }
      else
      {
         embeddedPackageGeometry = &(getDefinedBlock(rectifiedPackageId,packageGeometry->getFileNumber(),blockTypePcbComponent));
         m_blockMap.SetAt(rectifiedPackageId,embeddedPackageGeometry);

         embeddedPackageGeometry->setProperties(*packageGeometry);
         embeddedPackageGeometry->setName(rectifiedPackageId);

         int thruHolePadStackCount = 0;

         // replace all padstacks with rectified padstacks
         CInsertTypeMask padStackInsertTypeMask(insertTypePin,insertTypeMechanicalPin);

         for (CDataListIterator insertIterator(*embeddedPackageGeometry,dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            BlockStruct* dataGeometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
            CString floatToLayerName(componentLayerName);
            CString pinName = data->getInsert()->getRefname();

            if (padStackInsertTypeMask.contains(data->getInsert()->getInsertType()))
            {
               pinNameToContactLayerMap.lookup(pinName,floatToLayerName);
               floatToLayerName = getDenormalizedLayerName(floatToLayerName);

               if (floatToLayerName.Trim().IsEmpty())
               {
                  floatToLayerName = componentLayerName;
               }
            }

            /*   componentLayerName  
                 __________________________________________________________________
                                     |              Contacted Layer
                 PcbComponentSide    |  componentSide | solderSide    | inner
                 --------------------+----------------+---------------+------------
                 componentSide       |  componentSide | solderSide    | inner
                 solderSide          |  solderSide    | componentSide | inner mirrored
                 inner               |  componentSide | solderSide    | inner
            */

            if (componentLayerName.CompareNoCase(QSolderSide) == 0)
            {
               LayerStruct& floatToLayer = getThedaReader().getDefinedLayer(floatToLayerName);

               floatToLayerName = getMirroredContactLayerName(floatToLayer.getName());
            }

            InsertTypeTag insertType = data->getInsert()->getInsertType();
            bool padStackDescendantFlag = (insertType == insertTypePin || insertType == insertTypeMechanicalPin);

            BlockStruct* rectifiedGeometry;

            if (padStackDescendantFlag)
            {
               CString padStackFloatToLayerName(floatToLayerName);

               if (isThruHolePadStackGeometry(*dataGeometry))
               {
                  thruHolePadStackCount++;
                  padStackFloatToLayerName = QComponentSide;
                  pinNameToContactLayerMap.changePinToThruHole(pinName);
               }

               rectifiedGeometry = &(getRectifiedPadStackGeometry(*dataGeometry,padStackFloatToLayerName));
            }
            else
            {
               rectifiedGeometry = &(getRectifiedGeometry(*dataGeometry,floatToLayerName,padStackDescendantFlag));
            }

            data->getInsert()->setBlockNumber(rectifiedGeometry->getBlockNumber());
         }

         CString componentPlacementSpecificLayer = (isSolderSideName(componentLayerName) ? QComponentSide : componentLayerName);

         changePlacementSpecificLayers(*embeddedPackageGeometry,componentPlacementSpecificLayer);

         if (thruHolePadStackCount != 0)
         {
            BlockStruct* existingEmbeddedPackageGeometry = NULL;

            CString updatedRectifiedPackageId = getRectifiedPackageId(packageId,componentLayerName,pinNameToContactLayerMap);

            if (m_blockMap.Lookup(updatedRectifiedPackageId,existingEmbeddedPackageGeometry))
            {
               embeddedPackageGeometry = existingEmbeddedPackageGeometry;
            }
            else
            {
               m_blockMap.SetAt(updatedRectifiedPackageId,embeddedPackageGeometry);
            }
         }
      }
   }

   return embeddedPackageGeometry;
}

BlockStruct& CThedaReader::getRectifiedPadStackGeometry(BlockStruct& geometry,const CString& floatToLayerName)
{
   CString padstackTemplatePrefix(QPadStackTemplatePrefix);
   CString rectifiedGeometryName = geometry.getName() + getComponentSideNormalizedInfixLayerName(floatToLayerName);

   if (rectifiedGeometryName.Find(padstackTemplatePrefix,0) == 0)
   {
      rectifiedGeometryName = QPadStackPrefix + rectifiedGeometryName.Mid(padstackTemplatePrefix.GetLength());
   }
   else
   {
      int iii = 3;
   }

   BlockStruct* rectifiedGeometry = &geometry;

   if (! m_blockMap.Lookup(rectifiedGeometryName,rectifiedGeometry))
   {
      rectifiedGeometry = &(getDefinedBlock(rectifiedGeometryName,geometry.getFileNumber(),geometry.getBlockType()));

      if (rectifiedGeometry == &geometry)
      {
         int iii = 3;
      }
      else
      {
         m_blockMap.SetAt(rectifiedGeometryName,rectifiedGeometry);

         rectifiedGeometry->setProperties(geometry);
         rectifiedGeometry->setName(rectifiedGeometryName);

         // replace all geometries with rectified geometries
         for (CDataListIterator insertIterator(*rectifiedGeometry,dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            BlockStruct* dataGeometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
            BlockStruct& rectifiedDataGeometry = getRectifiedGeometry(*dataGeometry,floatToLayerName,true);

            data->getInsert()->setBlockNumber(rectifiedDataGeometry.getBlockNumber());
         }

         changePlacementSpecificLayers(*rectifiedGeometry,floatToLayerName);
      }
   }

   return *rectifiedGeometry;
}

BlockStruct& CThedaReader::getRectifiedPadGeometry(BlockStruct& padGeometry,const CString& contactLayerName)
{
   BlockStruct* rectifiedPadGeometry = &padGeometry;

   if (referencesPlacementSpecificLayer(padGeometry))
   {
      CString rectifiedPadGeometryName = padGeometry.getName() + getComponentSideNormalizedInfixLayerName(contactLayerName);

      if (! m_blockMap.Lookup(rectifiedPadGeometryName,rectifiedPadGeometry))
      {
         rectifiedPadGeometry = &(getDefinedBlock(rectifiedPadGeometryName,padGeometry.getFileNumber(),blockTypePadshape));

         if (rectifiedPadGeometry == &padGeometry)
         {
            int iii = 3;
         }
         else
         {
            m_blockMap.SetAt(rectifiedPadGeometryName,rectifiedPadGeometry);

            rectifiedPadGeometry->setProperties(padGeometry);
            rectifiedPadGeometry->setName(rectifiedPadGeometryName);

            changePlacementSpecificLayers(*rectifiedPadGeometry,contactLayerName);
         }
      }
   }

   return *rectifiedPadGeometry;
}

BlockStruct& CThedaReader::getRectifiedGeometry(BlockStruct& geometry,const CString& floatToLayerName,bool padStackDescendantFlag)
{
   BlockStruct* rectifiedGeometry = &geometry;

   if (referencesPlacementSpecificLayer(geometry))
   {
      CString rectifiedGeometryName = geometry.getName() + 
         (padStackDescendantFlag ? getComponentSideNormalizedInfixLayerName(floatToLayerName) : 
                                   getNormalizedInfixLayerName(floatToLayerName));

      if (! m_blockMap.Lookup(rectifiedGeometryName,rectifiedGeometry))
      {
         rectifiedGeometry = &(getDefinedBlock(rectifiedGeometryName,geometry.getFileNumber(),geometry.getBlockType()));

         if (rectifiedGeometry == &geometry)
         {
            int iii = 3;
         }
         else
         {
            m_blockMap.SetAt(rectifiedGeometryName,rectifiedGeometry);

            rectifiedGeometry->setProperties(geometry);
            rectifiedGeometry->setName(rectifiedGeometryName);

            // replace all geometries with rectified geometries
            for (CDataListIterator insertIterator(*rectifiedGeometry,dataTypeInsert);insertIterator.hasNext();)
            {
               DataStruct* data = insertIterator.getNext();

               BlockStruct* dataGeometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
               BlockStruct& rectifiedDataGeometry = getRectifiedGeometry(*dataGeometry,floatToLayerName,padStackDescendantFlag);

               data->getInsert()->setBlockNumber(rectifiedDataGeometry.getBlockNumber());
            }

            changePlacementSpecificLayers(*rectifiedGeometry,floatToLayerName);
         }
      }
   }

   return *rectifiedGeometry;
}

bool CThedaReader::referencesPlacementSpecificLayer(BlockStruct& geometry)
{
   bool retval = false;

   for (POSITION pos = geometry.getHeadDataPosition();pos != NULL;)
   {
      DataStruct* data = geometry.getNextData(pos);
      int layerIndex = data->getLayerIndex();

      if (layerIndex >= 0)
      {
         LayerStruct* layer = m_camCadDoc.getLayer(layerIndex);
         CSupString layerNameString = layer->getName();

         CString layerName = layerNameString.Scan(":");

         {
            CString suffix(Q_PS);
            int suffixLen = suffix.GetLength();

            if (layerName.GetLength() > suffixLen && layerName.Right(suffixLen).CompareNoCase(suffix) == 0)
            {
               retval = true;

               break;
            }
         }

         {
            CString suffix(Q_OPS);
            int suffixLen = suffix.GetLength();

            if (layerName.GetLength() > suffixLen && layerName.Right(suffixLen).CompareNoCase(suffix) == 0)
            {
               retval = true;

               break;
            }
         }
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* subBlock = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());

         if (referencesPlacementSpecificLayer(*subBlock))
         {
            retval = true;

            break;
         }
      }
   }

   return retval;
}

CString CThedaReader::getEnclosedLayerName(const CString& layerName) const
{
   if (layerName.IsEmpty())
   {
      int iii = 3;
   }

   CString retval = "{" + layerName + "}";

   return retval;
}

CString CThedaReader::getInfixLayerName(const CString& layerName) const
{
   CString retval;

   if (!layerName.IsEmpty())
   {
      retval = "_" + layerName;
   }
   else
   {
      int iii = 3;
   }

   return retval;
}

CString CThedaReader::getPlacementSpecificInfixLayerName(CString layerName) const
{
   CString retval;

   if (!layerName.IsEmpty())
   {
      if (!isComponentSideName(layerName) && !isSolderSideName(layerName))
      {
         retval = "_" + layerName;
      }
   }

   return retval;
}

CString CThedaReader::getNormalizedInfixLayerName(CString layerName,bool oppositeFlag) const
{
   CString retval;
   CString         sideName(oppositeFlag ? QSolderSideNormalizedLayerName    : QComponentSideNormalizedLayerName);
   CString oppositeSideName(oppositeFlag ? QComponentSideNormalizedLayerName : QSolderSideNormalizedLayerName   );

   if (!layerName.IsEmpty())
   {
      if (isComponentSideName(layerName))
      {
         layerName = sideName;
      }
      else if (isSolderSideName(layerName))
      {
         layerName = oppositeSideName;
      }

      retval = "_" + layerName;
   }
   else
   {
      int iii = 3;
   }

   return retval;
}

CString CThedaReader::getComponentSideNormalizedInfixLayerName(CString layerName) const
{
   CString retval;

   if (!layerName.IsEmpty())
   {
      if (! isComponentSideName(layerName))
      {
         if (isSolderSideName(layerName))
         {
            layerName = "SS";
         }

         retval = "_" + layerName;
      }
   }
   else
   {
      int iii = 3;
   }

   return retval;
}

bool CThedaReader::calcPlacementSpecificLayerName(CString& placementSpecificLayerName,
   const CString& layerName,const CString& floatToLayerName)
{
   bool retval = false;

   CSupString layerNameString(layerName);
   CString layerNamePrefix = layerNameString.Scan(":");

   for (int pass = 1;pass < 3;pass++)
   {
      CString suffix((pass == 1) ? Q_PS : Q_OPS);
      int suffixLen = suffix.GetLength();

      if (layerNamePrefix.GetLength() > suffixLen && layerNamePrefix.Right(suffixLen).CompareNoCase(suffix) == 0)
      {
         placementSpecificLayerName = layerNamePrefix.Mid(0,layerNamePrefix.GetLength() - suffixLen) + 
                                       getNormalizedInfixLayerName(floatToLayerName,pass == 2);

         if (layerNameString.GetLength() > 0)
         {
            placementSpecificLayerName += ":" + layerNameString;
         }

         retval = true;
         break;
      }
   }

   return retval;
}

void CThedaReader::changePlacementSpecificLayers(BlockStruct& geometry,const CString& floatToLayerName)
{
   for (POSITION pos = geometry.getHeadDataPosition();pos != NULL;)
   {
      DataStruct* data = geometry.getNextData(pos);
      int layerIndex = data->getLayerIndex();

      if (layerIndex >= 0)
      {
         LayerStruct* layer = m_camCadDoc.getLayer(layerIndex);
         CString layerName = layer->getName();
         CString placementSpecificLayerName;

         if (calcPlacementSpecificLayerName(placementSpecificLayerName,layerName,floatToLayerName))
         {
            LayerStruct& placementSpecificLayer = getDefinedLayer(layerName,floatToLayerName);
            data->setLayerIndex(placementSpecificLayer.getLayerIndex());
         }
      }

      changePlacementSpecificLayers(data->getAttributeMap(),floatToLayerName);      
   }

   changePlacementSpecificLayers(geometry.getAttributesRef(),floatToLayerName);
}

void CThedaReader::changePlacementSpecificLayers(CAttributes*& attributes,const CString& floatToLayerName)
{
   if (attributes != NULL)
   {
      WORD keywordIndex;
      Attrib* attribute;
      CString placementSpecificLayerName;

      for (POSITION pos = attributes->GetStartPosition();pos != NULL;)
      {
         attributes->GetNextAssoc(pos,keywordIndex,attribute);
         int layerIndex = attribute->getLayerIndex();

         // Debug
         const KeyWordStruct* keyWordStruct = m_camCadDoc.getKeyword(keywordIndex);
         keyWordStruct->getCCKeyword();
         CString keywordName = keyWordStruct->getCCKeyword();

         if (layerIndex >= 0)
         {
            CString layerName = m_camCadDoc.getLayerAt(layerIndex)->getName();

            if (calcPlacementSpecificLayerName(placementSpecificLayerName,layerName,floatToLayerName))
            {
               LayerStruct& placementSpecificLayer = getDefinedLayer(layerName,floatToLayerName);
               attribute->setLayerIndex(placementSpecificLayer.getLayerIndex());
            }
         }
      }
   }
}

CString CThedaReader::removeSignature(CString& geometryName) const
{
   CString retval;
   const int signatureLength = 9;

   if (geometryName.GetLength() > signatureLength)
   {
      CString signature = geometryName.Right(signatureLength);
      bool hasSignature = false;

      const char* p = signature;

      if (*p == '_')
      {
         p++;

         for (hasSignature = true;hasSignature && *p != '\0';p++)
         {
            hasSignature = (isxdigit(*p) != 0);
         }
      }

      if (hasSignature)
      {
         geometryName.GetBufferSetLength(geometryName.GetLength() - signatureLength);
         retval = signature;
      }
   }

   return retval;
}

void CThedaReader::fixupGeometryNames()
{
   CString underscore("_");
   CString topBuiltGeometrySuffix(underscore + QTopBuiltGeometrySuffix);
   CString bottomBuiltGeometrySuffix(underscore + QBottomBuiltGeometrySuffix);
   CString componentSideSuffix(underscore + QComponentSideNormalizedLayerName);
   CString solderSideSuffix(underscore + QSolderSideNormalizedLayerName);
   CString geometryName;
   BlockStruct* geometry;
   BlockStruct* existingGeometry;
   int topSuffixLength    = topBuiltGeometrySuffix.GetLength();
   int bottomSuffixLength = bottomBuiltGeometrySuffix.GetLength();
   int componentSideSuffixLength = componentSideSuffix.GetLength();
   int solderSideSuffixLength    = solderSideSuffix.GetLength();

   for (POSITION pos = m_blockMap.GetStartPosition();pos != NULL;)
   {
      m_blockMap.GetNextAssoc(pos,geometryName,geometry);

      CString originalGeometryName = geometryName;
      CString signature = removeSignature(geometryName);

      if (geometryName.GetLength() > topSuffixLength && geometryName.Right(topSuffixLength).CompareNoCase(topBuiltGeometrySuffix) == 0)
      {
         geometryName = geometryName.Left(geometryName.GetLength() - topSuffixLength);
      }
      else if (geometryName.GetLength() > bottomSuffixLength && geometryName.Right(bottomSuffixLength).CompareNoCase(bottomBuiltGeometrySuffix) == 0)
      {
         geometryName = geometryName.Left(geometryName.GetLength() - bottomSuffixLength);
      }

      if (geometryName.GetLength() > componentSideSuffixLength && geometryName.Right(componentSideSuffixLength).CompareNoCase(componentSideSuffix) == 0)
      {
         geometryName = geometryName.Left(geometryName.GetLength() - componentSideSuffixLength);
      }
      else if (geometryName.GetLength() > solderSideSuffixLength && geometryName.Right(solderSideSuffixLength).CompareNoCase(solderSideSuffix) == 0)
      {
         geometryName = geometryName.Left(geometryName.GetLength() - solderSideSuffixLength);
      }

      if (! m_blockMap.Lookup(geometryName,existingGeometry))
      {
         geometry->setName(geometryName);
         m_blockMap.SetAt(geometryName,geometry);
      }
      else
      {
         geometryName += signature;

         if (! m_blockMap.Lookup(geometryName,existingGeometry))
         {
            geometry->setName(geometryName);
            m_blockMap.SetAt(geometryName,geometry);
         }
      }
   }
}

CString CThedaReader::getComponentTemplateGeometryName(const CString& suffix) const
{
   CString componentTemplateGeometryName(suffix);

   if (componentTemplateGeometryName.Find(QPcbComponentTemplatePrefix) != 0)
   {
      componentTemplateGeometryName = QPcbComponentTemplatePrefix + suffix;
   }

   return componentTemplateGeometryName;
}

CString CThedaReader::getPadStackTemplateGeometryName(const CString& suffix) const
{
   CString padStackTemplateGeometryName(QPadStackTemplatePrefix + suffix);

   return padStackTemplateGeometryName;
}

CString CThedaReader::getPadStackGeometryName(const CString& suffix) const
{
   CString padStackGeometryName(QPadStackPrefix + suffix);

   return padStackGeometryName;
}

void CThedaReader::addVia(DataStruct& viaData)
{
   m_viaDataList.AddTail(&viaData);
}

void CThedaReader::addTestPoint(DataStruct& testPointData)
{
   m_testPointDataList.AddTail(&testPointData);
}

void CThedaReader::addReferencePad(DataStruct& referencePadData)
{
   m_referencePadDataList.AddTail(&referencePadData);
}

void CThedaReader::rectifyComponents(CNetlistCompArray& netListCompArray)
{
   for (int index = 0;index < netListCompArray.GetSize();index++)
   {
      TLNetlistComp* netListComp = netListCompArray.GetAt(index);

		if (netListComp != NULL)
		{
			DataStruct* componentData = netListComp->getComponentData();

			// Case 1963, crashed when componentData not found
			if (componentData != NULL)
			{
            CString refDes = componentData->getInsert()->getRefname();

				BlockStruct* componentGeometry = m_camCadDoc.getBlockAt(componentData->getInsert()->getBlockNumber());
				CString componentGeometryName = componentGeometry->getName();

				CString componentLayerName = netListComp->getComponentLayerName();
				CPinNameToContactLayerMap& pinNameToContactLayerMap = netListComp->getPinNameToContactLayerMap();
            CString descriptor = pinNameToContactLayerMap.getDescriptor();

				BlockStruct* rectifiedComponentGeometry = getRectifiedComponentGeometry(componentGeometryName,componentLayerName,pinNameToContactLayerMap);
            addPinNameToContactLayerMapDescriptorAttribute(*componentData,pinNameToContactLayerMap.getDescriptor());

				// Case 1963, crashed when rectifiedComponentGeometry not found
				if (rectifiedComponentGeometry != NULL)
				{
					componentData->getInsert()->setBlockNumber(rectifiedComponentGeometry->getBlockNumber());

               if (isSolderSideName(componentLayerName))
               {
                  componentLayerName = QComponentSide;
               }

					changePlacementSpecificLayers(componentData->getAttributeMap(),componentLayerName);
				}
			}
      }
   }
}

void CThedaReader::rectifyViasAndTestPoints()
{
   CString layerName(QComponentSide);
   CString mirroredLayerName(QSolderSide);

   for (CDataListIterator viaIterator(m_viaDataList,dataTypeInsert);viaIterator.hasNext();)
   {
      DataStruct* data = viaIterator.getNext();
      BlockStruct* geometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());

      BlockStruct& rectifiedGeometry = getRectifiedPadStackGeometry(*geometry,layerName);

      data->getInsert()->setBlockNumber(rectifiedGeometry.getBlockNumber());
   }

   // for now assume test points always contact the component side layer
   CPinNameToContactLayerMap testPointPinNameToContactLayerNameMap;
   testPointPinNameToContactLayerNameMap.addPin("1",layerName,mirroredLayerName);

   for (CDataListIterator testPointIterator(m_testPointDataList,dataTypeInsert);testPointIterator.hasNext();)
   {
      DataStruct* data = testPointIterator.getNext();
      BlockStruct* geometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());

      BlockStruct* rectifiedGeometry = getRectifiedComponentGeometry(geometry->getName(),layerName,testPointPinNameToContactLayerNameMap);

      data->getInsert()->setBlockNumber(rectifiedGeometry->getBlockNumber());
   }

   for (CDataListIterator referencePadIterator(m_referencePadDataList,dataTypeInsert);referencePadIterator.hasNext();)
   {
      DataStruct* data = referencePadIterator.getNext();
      BlockStruct* geometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());

      BlockStruct& rectifiedGeometry = getRectifiedPadStackGeometry(*geometry,QComponentSide);

      data->getInsert()->setBlockNumber(rectifiedGeometry.getBlockNumber());
   }
}

CString CThedaReader::removeComponentPrefixes(const CString& geometryName) const
{
   CString newGeometryName(geometryName);
   CString pcbComponentTemplatePrefix(QPcbComponentTemplatePrefix);
   CString pcbComponentPrefix(QPcbComponentPrefix);

   while (newGeometryName.Find(pcbComponentTemplatePrefix) == 0)
   {
      newGeometryName = newGeometryName.Mid(pcbComponentTemplatePrefix.GetLength());
   }

   while (newGeometryName.Find(pcbComponentPrefix) == 0)
   {
      newGeometryName = newGeometryName.Mid(pcbComponentPrefix.GetLength());
   }

   return newGeometryName;
}

CString CThedaReader::getBondLandGeometryName(const CString& componentGeometryName,const CString& pinName,int wireNumber) const
{
   CString geometryName;
   geometryName.Format("%s%s_%s_%d",QBondLandPrefix,removeComponentPrefixes(componentGeometryName),pinName,wireNumber);

   return geometryName;
}

CString CThedaReader::getWireGeometryName(const CString& componentGeometryName,const CString& pinName,int wireNumber) const
{
   CString geometryName;
   geometryName.Format("%s%s_%s_%d",QWirePrefix,removeComponentPrefixes(componentGeometryName),pinName,wireNumber);

   return geometryName;
}

bool CThedaReader::containsBlock(const CString& blockName)
{
   bool retval = m_blockMap.contains(blockName);

   return retval;
}

void CThedaReader::setOptionAdjustLayerNames(bool option)
{
   m_optionAdjustLayerNames = option;
}

bool CThedaReader::getOptionRectifyComponents() const
{
   return m_optionRectifyComponents;
}

void CThedaReader::setOptionRectifyComponents(bool option)
{
   m_optionRectifyComponents = option;
}

bool CThedaReader::getOptionRectifyViasAndTestPoints() const
{
   return m_optionRectifyViasAndTestPoints;
}

void CThedaReader::setOptionRectifyViasAndTestPoints(bool option)
{
   m_optionRectifyViasAndTestPoints = option;
}

bool CThedaReader::getOptionPurgeUnusedWidthsAndGeometries() const
{
   return m_optionPurgeUnusedWidthsAndGeometries;
}

void CThedaReader::setOptionPurgeUnusedWidthsAndGeometries(bool option)
{
   m_optionPurgeUnusedWidthsAndGeometries = option;
}

bool CThedaReader::getOptionPurgeUnusedLayers() const
{
   return m_optionPurgeUnusedLayers;
}

void CThedaReader::setOptionPurgeUnusedLayers(bool option)
{
   m_optionPurgeUnusedLayers = option;
}

bool CThedaReader::getOptionFixupGeometryNames() const
{
   return m_optionFixupGeometryNames;
}

void CThedaReader::setOptionFixupGeometryNames(bool option)
{
   m_optionFixupGeometryNames = option;
}

void CThedaReader::pushSection(const CString& sectionName)
{
   int index = m_sectionStack.GetCount();

   m_sectionStack.AddTail(sectionName);

   m_materialStack.SetAtGrow(index,"");
}

void CThedaReader::popSection()
{
   if (m_sectionStack.GetCount() > 0)
   {
      m_sectionStack.RemoveTail();
   }
}

void CThedaReader::setMaterial(const CString& material)
{
   int index = m_sectionStack.GetCount() - 1;

   if (index >= 0)
   {
      m_materialStack.SetAtGrow(index,material);
   }
}

CString CThedaReader::getMaterial() const
{
   CString material;
   int index = m_sectionStack.GetCount() - 1;

   if (index >= 0 && index < m_materialStack.GetSize())
   {
      material = m_materialStack.GetAt(index);
   }

   return material;
}








