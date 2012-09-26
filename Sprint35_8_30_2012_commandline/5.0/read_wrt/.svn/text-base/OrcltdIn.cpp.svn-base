// $Header: /CAMCAD/4.6/read_wrt/OrcltdIn.cpp 73    2/14/07 4:08p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

/*
   Rules for USE_DERIVE_SYM

   Orcad makes a local copy of each placed components

   used_derive_sym yes 
      find the orginal defined symbol (same as ORCAD footprint editor)
      mirrors a component correctly

   use_derive_sym no
      keeps the derived component
      find the non -M padstack and places it. This is needed because derived symbols to not have their
      own sym obs. In order to get the obs from the original and the derived mirror command syncronized,
      we have to "unmirror" the padstack and mirror the complete component.
      When a derived symbol is mirrored, all pins are placed -x.

*/           

#include "stdafx.h"
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
#include "orcltdin.h"
#include "CamCadDatabase.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)
#define mapOrcadLevelToPcbSide(level)  ((level) == 1 ? pcbSideTop : (level) == 2 ? pcbSideBottom : pcbSideUnknown)

extern CProgressDlg        *progress;

/* Static Variables *********************************************************/

static FileStruct          *GlobalPcbFile = NULL;
static FILE                *ifp;                         /* File pointers.    */
static long                ifp_line = 0;                 /* Current line number. */
static char                token[MAX_LINE];                 /* Current token.       */
static CString             cur_line;
static bool                cur_new = true;
static int                 Push_tok = FALSE;
static int                 heightunit;
static int                 page_unit;
static double              scaleFactor;
static FILE                *fLog;
static int                 display_error;
static CString             orcad_in_file;
static int                 drillchart = TRUE;
static int                 width_in_mil;           // error in Version 7.x
static int                 topLayerIndex = -1;
static int                 bottomLayerIndex = -1;

static MINVersion          min_ver;                      // version 
static MINGlobal           G;                            /* global const. */
static MINObs              cur_obs;                      // current obstical bag
static MINText             cur_text;
static long                cur_uid;
static char                cur_netname[40];        // current used netname name
static int                 cur_filenum = 0;

static int                 LayerNameFieldNumber;   // == 1 is old original behavior, == 2 is to help vPlan compatibility
static int                 NormalizeRefdes;
static int                 ComponentSMDrule;       // 0 = take existing ATT_SMD
                                                   // 1 = if all pins of comp are marked as SMD
                                                   // 2 = if most pins of comp are marked as SMD
                                                   // 3 = if one pin of comp are marked as SMD
static ORCADDrillArray     drillArray;
static ORCADCompPinArray   comppinarray;
static ORCADPadformArray   padformarray;


static ORCADPoly_l         *poly_l;
static int                 polycnt =0 ;

static ORCADAdef           layer_attr[MAX_LAYERS];       /* Array of layers from pdif.in   */
static int                 layer_attr_cnt;

static CCamCadDatabase* s_camCadDatabase = NULL;
static CCamCadDatabase& getCamCadDatabase() { return *s_camCadDatabase; }
static CCEtoODBDoc* getCamCadDoc() { return &(getCamCadDatabase().getCamCadDoc()); }

//_____________________________________________________________________________
static ORCADShapeArray* s_shapeArray = NULL;

static ORCADShapeArray& getOrcadShapes()
{
   if (s_shapeArray == NULL)
   {
      s_shapeArray = new ORCADShapeArray();
      s_shapeArray->SetSize(0,400);
   }

   return *s_shapeArray;
}

static ORCADShape* getOrcadShape(int index)
{
   ORCADShape* shape = getOrcadShapes().GetAt(index);

   return shape;
}

static ORCADShape* getLastOrcadShape()
{
   ORCADShape* shape = getOrcadShapes().GetAt(getOrcadShapes().GetSize() - 1);

   return shape;
}

//_____________________________________________________________________________
static ORCADCompInstArray* s_componentInstanceArray = NULL;

static ORCADCompInstArray& getOrcadComponentInstances()
{
   if (s_componentInstanceArray == NULL)
   {
      s_componentInstanceArray = new ORCADCompInstArray();
      s_componentInstanceArray->SetSize(0,400);
   }

   return *s_componentInstanceArray;
}

static ORCADCompInst& getOrcadComponentInstance(int index)
{
   ORCADCompInst* compInst = getOrcadComponentInstances().GetAt(index);

   return *compInst;
}

static ORCADCompInst& getLastOrcadComponentInstance()
{
   ORCADCompInst* compInst = getOrcadComponentInstances().GetAt(getOrcadComponentInstances().GetSize() - 1);

   return *compInst;
}

//_____________________________________________________________________________
static ORCADNetArray* s_netArray = NULL;

static ORCADNetArray& getOrcadNets()
{
   if (s_netArray == NULL)
   {
      s_netArray = new ORCADNetArray();
      s_netArray->SetSize(0,400);
   }

   return *s_netArray;
}

static ORCADNet& getLastOrcadNet()
{
   ORCADNet* net = getOrcadNets().GetAt(getOrcadNets().GetSize() - 1);

   return *net;
}

//_____________________________________________________________________________
static MINPackageArray* s_packageArray = NULL;

static MINPackageArray& getOrcadPackages()
{
   if (s_packageArray == NULL)
   {
      s_packageArray = new MINPackageArray();
      s_packageArray->SetSize(0,100);
   }

   return *s_packageArray;
}

static MINPackage& getLastOrcadPackage()
{
   MINPackage* package = getOrcadPackages().GetAt(getOrcadPackages().GetSize() - 1);

   return *package;
}

//_____________________________________________________________________________
static CPadstackArray* s_padstackArray = NULL;

static CPadstackArray& getOrcadPadstacks()
{
   if (s_padstackArray == NULL)
   {
      s_padstackArray = new CPadstackArray();
      s_padstackArray->SetSize(0,100);
   }

   return *s_padstackArray;
}

static MINPadstack& getOrcadPadstack(int index)
{
   MINPadstack* padstack = getOrcadPadstacks().GetAt(index);

   return *padstack;
}

static MINPadstack& getLastOrcadPadstack()
{
   MINPadstack* padstack = getOrcadPadstacks().GetAt(getOrcadPadstacks().GetSize() - 1);

   return *padstack;
}

//_____________________________________________________________________________
static ORCADAttribArray* s_attributeArray = NULL;

static ORCADAttribArray& getOrcadAttributes()
{
   if (s_attributeArray == NULL)
   {
      s_attributeArray = new ORCADAttribArray();
      s_attributeArray->SetSize(0,100);
   }

   return *s_attributeArray;
}

//_____________________________________________________________________________
static ORCADLayerArray* s_layerArray = NULL;

static ORCADLayerArray& getOrcadLayers()
{
   if (s_layerArray == NULL)
   {
      s_layerArray = new ORCADLayerArray();
      s_layerArray->SetSize(0,100);
   }

   return *s_layerArray;
}

static COrcadLayer& getOrcadLayer(int index)
{
   COrcadLayer& layer = getOrcadLayers().getDefinedLayer(index);

   return layer;
}

static COrcadLayer& getLastOrcadLayer()
{
   int lastLayerIndex = getOrcadLayers().GetSize() - 1;

   if (lastLayerIndex < 0)
   {
      lastLayerIndex = 0;
   }

   COrcadLayer& layer = getOrcadLayer(lastLayerIndex);

   return layer;
}

COrcadLayer& ORCADLayerArray::getDefinedLayer(int index)
{
   if (index < 0)
   {
      index = 0;
   }

   if (index >= GetSize())
   {
      for (int newIndex = GetSize();newIndex <= index;newIndex++)
      {
         COrcadLayer* layer = new COrcadLayer();
         Add(layer);
      }
   }

   COrcadLayer* layer = GetAt(index);

   return *layer;
}

/* Function Prototypes *********************************************************/

static double cnv_unit(char *);
static double cnv_unit(double);
static double cnv_conn(char *);
static double cnv_degree(char *);
static int go_command(List *,int);
static void free_all_mem();
static void init_all_mem();
static int get_tok();
static int p_error();
static int tok_search(List *,int);
static int push_tok();
static int loop_command(List *,int);
static int tok_layer();
static void clean_text(char *);
static int get_padnumber(char *);
static int get_next(CString &buf, bool newlp);
static int get_line(CString &buf);
static int get_pincnt(char *,int,int *);
static int write_text();
static DataStruct  *write_poly(char *l,int closed, int poured, int fill, double offx, double offy, int grclass, BOOL mirror);
static int write_obs(int index);
static DataStruct *write_circle(char *l, double offx, double offy, int grclass, BOOL mirror);
static DataStruct *write_arc(char *l, int closed, int fill, double offx, double offy, int grclass, BOOL mirror);
static int get_netptr(int index);
static int get_compptr(int index);
static int get_shapeptr(int index);
static int get_padstackptr(int index);
static int get_shapeptr_from_name(const char *s);
static int get_padstacknameptr(const char *name);
static void load_orcadltdsettings(const CString fname);
static int place_sym_pins();
static void delete_unused_viadefinition();
static void delete_unused_symbolgraphic();
static void do_layers();
//static int get_layertype(const char *l);
static void RemoveIdenticalSYM();
static void RemoveUnusedDevices();
static BOOL IsPinLocOrNameDiff(int masterIndex, int compareIndex);
static void ReplaceSYM(int masterIndex, int compareIndex);
static int GetMasterSYM(int index);
static bool IsMasterSYMmirrored(int masterIndex, int compareIndex);
static void MirrorPolyOnX(CPolyList *polylist);
static BlockStruct *CreateReversePadstack(BlockStruct *padstack, CString newPadstackName);

//_____________________________________________________________________________
void debugDump()
{
   //getCamCadDoc()->SaveDataFileAs("c:\\OrcadDebug.cc");
}

/******************************************************************************
* ReadOrcad
*/
void ReadOrcad(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   if (Doc == NULL)
   {
      return;
   }

   // Not sure if header "origin" command will always be present, so ensure origin
   // is init to zero zero.
   G.rawOrcadOriginX = 0.;
   G.rawOrcadOriginY = 0.;

   s_camCadDatabase = new CCamCadDatabase(*Doc);

   GlobalPcbFile = NULL;
   //doc = Doc;
   display_error = 0;

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f, fname);
   strcat(f, ext);

   Push_tok = FALSE;
   cur_new = true;
   cur_line.Empty();
   heightunit  = pageunits;
   page_unit = pageunits;
   scaleFactor = Units_Factor(UNIT_INCHES, pageunits); // just to initialize

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer,"r")) == NULL)
   {
     ErrorMessage("Error open file", "Error");
     return;
   }

   CString orcadLogFile;
   fLog = getApp().OpenOperationLogFile("orcad.log", orcadLogFile);
   if (fLog == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(fLog, "ODB++");
	fprintf(fLog, "---------------------------------------\n\n");

   GlobalPcbFile = Graph_File_Start(f, Type_ORCAD_LTD_Layout);
   GlobalPcbFile->setBlockType(blockTypePcb);
   GlobalPcbFile->getBlock()->setBlockType(GlobalPcbFile->getBlockType());

   if (GlobalPcbFile->getTypeList().GetCount() != 0)
   {
      int iii = 3;
   }

   Graph_Level("0", "", 1);
   init_all_mem();

   CString settingsFile( getApp().getImportSettingsFilePath("orcad.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nORCAD Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_orcadltdsettings(settingsFile);

   orcad_in_file = settingsFile;

   /* initialize scanning parameters */
   rewind(ifp);
   ifp_line = 0;

   if (loop_command(min_lst, SIZ_MIN_LST) < 0)
   {
      ErrorMessage("ORCAD Layout NOT Loaded", "Read Error");
      delete s_camCadDatabase;
      s_camCadDatabase = NULL;
   }
   else
   {
      delete s_camCadDatabase;
      s_camCadDatabase = new CCamCadDatabase(*Doc);

      if (progress != NULL)
         progress->SetStatus("Optimizing Database");
      place_sym_pins();

      delete_unused_viadefinition();
      delete_unused_symbolgraphic();   // in orcad, every PCB component has a SYM graphic block reference attached.
                                    // here I delete them if they are empty

      // this can happen on strange via placements.
      double accuracy = get_accuracy(getCamCadDoc());
      EliminateSinglePointPolys(getCamCadDoc());                
      BreakSpikePolys(GlobalPcbFile->getBlock());

      Crack(getCamCadDoc(), GlobalPcbFile->getBlock(), TRUE);          
      EliminateOverlappingTraces(getCamCadDoc(),GlobalPcbFile->getBlock(), TRUE, accuracy);                

      do_layers();

      //debugDump();

      getOrcadComponentInstances().normalizeGeometriesAndInserts(getOrcadShapes());
      getOrcadComponentInstances().reduceGeometries(getOrcadShapes());
      getOrcadComponentInstances().createDevices(getCamCadDatabase(),GlobalPcbFile, getOrcadShapes(), getOrcadPackages());
      getOrcadComponentInstances().makeGeometriesTopDefined();

      // This must be after the normalizeGeometriesAndInserts stuff. In DR 670329 some vias come in
      // as components geometries. Doing this EliminateDups was deleting stuff that the getOrcadComponentInstances
      // still had references to, so normalize operation then crashed.
      EliminateDuplicateVias(getCamCadDoc(), GlobalPcbFile->getBlock());
      
      //----------------------
      CTMatrix originMatrix;
      double originX = cnv_unit(G.rawOrcadOriginX);
      double originY = cnv_unit(G.rawOrcadOriginY);
      originMatrix.translate(-originX, -originY);
      GlobalPcbFile->getBlock()->transform(originMatrix, &Doc->getCamCadData());
      //----------------------

      update_smdpads(getCamCadDoc());
      RefreshInheritedAttributes(getCamCadDoc(), SA_OVERWRITE);  // smd pins are after the pin instance is done.

      update_smdrule_geometries(getCamCadDoc(), ComponentSMDrule);
      update_smdrule_components(getCamCadDoc(), GlobalPcbFile, ComponentSMDrule);

      generate_PADSTACKACCESSFLAG(getCamCadDoc(), 1);
      generate_PINLOC(getCamCadDoc(), GlobalPcbFile, 1);   // this function generates the PINLOC argument for all pins.

      // update compheight
      for (int i=0;i < getOrcadComponentInstances().GetSize();i++)
      {
         if (getOrcadComponentInstance(i).getHeight() > 0.0)
         {  
            DataStruct *data = getOrcadComponentInstance(i).getComponentInsert(); // datastruct_from_refdes(doc, file->getBlock(), getOrcadComponentInstance(i).name);

            if (data)
            {
               double height = getOrcadComponentInstance(i).getHeight();
               getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_COMPHEIGHT, 0), 
                  VT_UNIT_DOUBLE, &height, SA_OVERWRITE, NULL);
            }
         }


      }

      if (!getCamCadDoc()->verifyAndRepairData())
      {
         //int iii = 3;
      }

      delete s_camCadDatabase;
      s_camCadDatabase = NULL;

      Doc->purgeUnusedBlocks(); // use after optimize_PADSTACKS

      // This function must be called after purgeUnusedBlocks()
      RemoveUnusedDevices();
   }

   free_all_mem();

   fclose(ifp);

   fprintf(fLog, "\nCopper Pour: Orcad stores only the pour boundary in the database and\n");
   fprintf(fLog, "dynamically pours copper during draw. This means that only the boundary\n");
   fprintf(fLog, "is shown in CAMCAD (unfilled) and not the flooded copper.\n");
   display_error++;

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!orcadLogFile.IsEmpty())
      fclose(fLog);

   if (display_error && !orcadLogFile.IsEmpty())
      Logreader(orcadLogFile);
}

bool isOuterLayer(int level)
{
   bool retval = (level == 1 || level == 2);

   return retval;
}

/******************************************************************************
* load_orcadltdsettings
*/
static void load_orcadltdsettings(const CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;

   width_in_mil = TRUE;
   ComponentSMDrule = 0;
   LayerNameFieldNumber = 1;
   NormalizeRefdes = 0;
   layer_attr_cnt = 0;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found 
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "ORCAD Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".WIDTH_IN_MIL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'N' || lp[0] == 'n')
               width_in_mil = FALSE;
         }
         else if (!STRICMP(lp, ".HEIGHTUNIT"))
         {
            char  *lp1;

            if ((lp1 = get_string(NULL, " \t\n")) != NULL)
            {
               if (!STRNICMP(lp1, "IN", 2))
                  heightunit = UNIT_INCHES;
               else if (!STRNICMP(lp1, "MM", 2))
                  heightunit = UNIT_MM;
               else if (!STRNICMP(lp1, "MI", 2))
                  heightunit = UNIT_MILS;
               else if (!STRNICMP(lp1, "TN", 2))
                  heightunit = UNIT_TNMETER;
            }
         }
         else if (!STRCMPI(lp, ".Normalize_Refdes"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               NormalizeRefdes = TRUE;
         }
         else if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString attr = _strupr(lp);
            attr.TrimLeft();
            attr.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString ccattr = _strupr(lp);
            ccattr.TrimLeft();
            ccattr.TrimRight();

            ORCADAttrib *oAttrib = new ORCADAttrib;
            getOrcadAttributes().Add(oAttrib);  
            oAttrib->orcad_name = attr;
            oAttrib->cc_name = ccattr;
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString olayer = lp;
            olayer.TrimLeft();
            olayer.TrimRight();
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);

            if (layer_attr_cnt < MAX_LAYERS)
            {
               layer_attr[layer_attr_cnt].setName(olayer);
               layer_attr[layer_attr_cnt].setAttr(laytype);
               layer_attr_cnt++;
            }
            else
            {
               fprintf(fLog, "Too many layer attributes\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".LAYER_NAME_FIELD"))
         {
            // .LAYER_NAME_FIELD  FIRST    (or SECOND)
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            CString val(lp);
            if (val.CompareNoCase("first") == 0)
               LayerNameFieldNumber = 1; // Traditional RSI style.
            else if (val.CompareNoCase("second") == 0)
               LayerNameFieldNumber = 2; // Traditional vPlan style.
            // else ignore other values, leave as original default.
         }
      }
   }

   fclose(fp);
}

/******************************************************************************
* get_attrmap
*/
static const char* get_attrmap(const char *a)
{
   for (int i=0; i < getOrcadAttributes().GetSize(); i++)
   {
      if (getOrcadAttributes().GetAt(i)->orcad_name.CompareNoCase(a) == 0)
         return getOrcadAttributes().GetAt(i)->cc_name.GetBuffer(0);
   }

   return a;
}

/******************************************************************************
* get_layernameindex
*/
static int get_layernameindex(const char *name)
{
   for (int i=0; i < getOrcadLayers().GetSize(); i++)
   {
      if (getOrcadLayer(i).getName().IsEmpty())
         continue;

      if (getOrcadLayer(i).getName().CompareNoCase(name) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* do_layers
   char  *name;         // name
   char  *name2;        // nickname
   char  *name3;        // library name
   int   mirror;
   int   index;
   char  unused;
   char  drill;
*/
static void do_layers()
{
   int   i;
   int   stackcnt = 0;
   CString  toplayer, botlayer;
   LayerStruct *layer;

   if (NormalizeRefdes)
   {
      i = Graph_Level("REFDES_1", "", 0);
      layer = getCamCadDoc()->FindLayer(i);
      layer->setLayerType(LAYTYPE_TOP);

      i = Graph_Level("REFDES_2", "", 0);
      layer = getCamCadDoc()->FindLayer(i);
      layer->setLayerType(LAYTYPE_BOTTOM);

      Graph_Level_Mirror("REFDES_1", "REFDES_2", "");
   }

   i = Graph_Level("COMPOUTLINE_1", "", 0);
   layer = getCamCadDoc()->FindLayer(i);
   layer->setLayerType(LAYTYPE_TOP);

   i = Graph_Level("COMPOUTLINE_2", "", 0);
   layer = getCamCadDoc()->FindLayer(i);
   layer->setLayerType(LAYTYPE_BOTTOM);

   Graph_Level_Mirror("COMPOUTLINE_1", "COMPOUTLINE_2", "");

   for (i=0; i < getOrcadLayers().GetSize(); i++)
   {
      if (getOrcadLayer(i).unused)
         continue;

      if (getOrcadLayer(i).index == 1)
      {
         // make top and solder layers
         toplayer = getOrcadLayer(i).getName();
         topLayerIndex = Graph_Level(toplayer, "", 0);
         LayerStruct *l = getCamCadDoc()->FindLayer(topLayerIndex);
         l->setLayerType(LAYTYPE_SIGNAL_TOP);
         l->setElectricalStackNumber(++stackcnt);

         botlayer = getOrcadLayer(getOrcadLayer(i).mirror).getName();
         bottomLayerIndex = Graph_Level(botlayer,"",0);
         l = getCamCadDoc()->FindLayer(bottomLayerIndex);
         l->setLayerType(LAYTYPE_SIGNAL_BOT);
      }
      else
      {
         if (getOrcadLayer(i).etch)
         {
            if (!botlayer.Compare(getOrcadLayer(i).getName()))
               continue;

            int li = Graph_Level(getOrcadLayer(i).getName(), "", 0);
            LayerStruct *l = getCamCadDoc()->FindLayer(li);
            l->setElectricalStackNumber(++stackcnt);
            l->setLayerType(LAYTYPE_SIGNAL_INNER);
         }

         if (getOrcadLayer(i).plane)
         {
            int li = Graph_Level(getOrcadLayer(i).getName(), "", 0);
            LayerStruct *l = getCamCadDoc()->FindLayer(li);
            l->setElectricalStackNumber(++stackcnt);
            l->setLayerType(LAYTYPE_POWERNEG);
         }
      }
      if (getOrcadLayer(i).mirror)
      {
         Graph_Level(getOrcadLayer(i).getName(), "", 0);
         Graph_Level(getOrcadLayer(getOrcadLayer(i).mirror).getName(), "", 0);
         Graph_Level_Mirror(getOrcadLayer(i).getName(), getOrcadLayer(getOrcadLayer(i).mirror).getName(), "");
      }
   }

   int li = Graph_Level(botlayer, "", 0);
   LayerStruct *l = getCamCadDoc()->FindLayer(li);
   l->setElectricalStackNumber(++stackcnt); 

   // here attributes from orcadltd.in
   for (int j=0; j< getCamCadDoc()->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = getCamCadDoc()->getLayerArray()[j];
      if (layer == NULL) 
         continue; // could have been deleted.

      // Special layers that importer  made up, not from CAD.
      if (!STRCMPI(layer->getName(), "BOARD"))
         layer->setLayerType(LAYTYPE_BOARD_OUTLINE);

      if (!STRCMPI(layer->getName(), "REFDES_1") || !STRCMPI(layer->getName(), "REFDES_2"))
      {
         for (i=0; i<layer_attr_cnt; i++)
         {
            // here get layername from nickname 
            if (layer->getName().CompareNoCase(layer_attr[i].getName()) == 0)
            {
               layer->setLayerType(layer_attr[i].getAttr());
               break;
            }
         }
      }

      // Layers from CAD
      int li = get_layernameindex(layer->getName());
      if (li < 0)
         continue;

      CString  lname = getOrcadLayer(li).getName2();   // nick name

      if (getOrcadLayer(li).unused)
         continue;

      for (i=0; i<layer_attr_cnt; i++)
      {
         // here get layername from nickname 
         if (lname.CompareNoCase(layer_attr[i].getName()) == 0)
         {
            layer->setLayerType(layer_attr[i].getAttr());
            break;
         }
      }
   }
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (!_STRICMP(layerTypeToString(i), l))
//         return i;
//   }
//
//   return LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* RemoveIdenticalSYM
*/
static void RemoveIdenticalSYM()
{
   for (int i=0;i < getOrcadShapes().GetSize(); i++)
   {
      ORCADShape *masterShape = getOrcadShape(i);

      // TRUE means it is already checked, 0 means it is the master geometry
      if (masterShape->isDuplicate() || masterShape->getUsageCount() == 0)
         continue;

      for (int j=i+1;j < getOrcadShapes().GetSize(); j++)
      {
         ORCADShape *compareShape = getOrcadShape(j);

         // TRUE means it is already checked, 0 means it is the master geometry
         if (compareShape->isDuplicate())
            continue;

         // If shape and compareShape is not derived from the same SYM parent,
         // then it is not going to be the same anyway, so skip checking it
         if (compareShape->getDerivedShapeIndex() != masterShape->getDerivedShapeIndex())
            continue;

         // Pin location is different, skip
         if (IsPinLocOrNameDiff(i, j))
            continue;

         // set to -1 to indicate it is already checked and it is the same
         compareShape->setDuplicate(true);

         // Change all insert of compareShape to point to masterShape
         ReplaceSYM(i, j);
      }     

      // change it to be the master SYM
      masterShape->setDuplicate(false);
   }
}

/******************************************************************************
* RemoveUnusedDevices
   - Remove any device that is no longer tied to a geometry
   - It can only be called after the CCEtoODBDoc::purgeUnusedBlocks() function
*/
static void RemoveUnusedDevices()
{
#if CamCadMajorMinorVersion > 406  //  > 4.6
   GlobalPcbFile->getTypeList().removeUnusedDeviceTypes();
#else
   POSITION typePos = GlobalPcbFile->getTypeList().GetHeadPosition();
   while (typePos)
   {
      POSITION previousPos = typePos;
      TypeStruct *type = GlobalPcbFile->getTypeList().GetNext(typePos);
      if (!type)
         continue;

      if (type->getBlockNumber() > -1)
         continue;

      GlobalPcbFile->getTypeList().RemoveAt(previousPos);
   }
#endif
}

/******************************************************************************
* IsPinLocOrNameDiff
*/
static BOOL IsPinLocOrNameDiff(int masterIndex, int compareIndex)
{
   ORCADShape *masterShape = getOrcadShape(masterIndex);
   ORCADShape *compareShape = getOrcadShape(compareIndex);

   for (int i=0; i<masterShape->getPinCnt(); i++)
   {
      MINPin *pin = &masterShape->getPin(i);

      BOOL diffLoc = FALSE;
      BOOL diffName = TRUE;
      double rot = pin->getRot();
      double x = pin->getX();
      double y = pin->getY();

      for (int j=0; j<compareShape->getPinCnt(); j++)
      {
         MINPin *comparePin = &compareShape->getPin(j);
         if (comparePin->getPinName().CompareNoCase(pin->getPinName()) == 0)
            diffName = FALSE;
         else  
            continue;  // pin name is not the same, so skip

         double compareRot = comparePin->getRot();
         double compareX = comparePin->getX();
         double compareY = comparePin->getY();

         if (fabs(compareX - x) > SMALLNUMBER || fabs(compareY - y) > SMALLNUMBER || fabs(compareRot - rot) > SMALLNUMBER)
            diffLoc = TRUE;
      }

      if (diffLoc || diffName)
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* ReplaceSYM
*/
static void ReplaceSYM(int masterIndex, int compareIndex)
{
   ORCADShape *masterShape = getOrcadShape(masterIndex);
   ORCADShape *compareShape = getOrcadShape(compareIndex);

   POSITION pos = compareShape->GetDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = compareShape->GetDataList().GetNext(pos);
      if (!data || data->getDataType() != T_INSERT)
         continue;

      if (masterShape->getBlock() != NULL)
      {
         data->getInsert()->setBlockNumber(masterShape->getBlock()->getBlockNumber());

         getCamCadDoc()->SetAttrib(&data->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_TYPELISTLINK, 0), VT_STRING,
            masterShape->getDevTypeName().GetBuffer(0), SA_OVERWRITE, NULL);
      }
   }
   compareShape->GetDataList().RemoveAll();
}

/******************************************************************************
* GetMasterSYM
*/
static int GetMasterSYM(int index)
{
   int masterIndex = index;
   ORCADShape *shape = getOrcadShape(index);
   while (shape->getDerivedShapeIndex() > 0)
   {
      masterIndex = get_shapeptr(shape->getDerivedShapeIndex());     
      shape = getOrcadShape(masterIndex);
   }

   return masterIndex;
}

bool performPadstackContentCheck(MINPin* pin,MINPin* mirroredPin,bool& mirroredFlag)
{
   bool retval = false;

   int padstackIndex = pin->getPadStackIndex();
   CString padstackGeometryName = getOrcadPadstack(padstackIndex).getName();
   BlockStruct* padstackGeometry = getCamCadDoc()->Find_Block_by_Name(padstackGeometryName,-1);

   int mirroredPadstackIndex = mirroredPin->getPadStackIndex();
   CString mirroredPadstackGeometryName = getOrcadPadstack(mirroredPadstackIndex).getName();
   BlockStruct* mirroredPadstackGeometry = getCamCadDoc()->Find_Block_by_Name(mirroredPadstackGeometryName,-1);

   bool topPadFlag    = false;
   bool bottomPadFlag = false;
   bool toolFlag      = false;

   if (padstackGeometry != NULL && mirroredPadstackGeometry != NULL)
   {
      for (POSITION pos = padstackGeometry->getHeadDataPosition();pos != NULL;)
      {
         DataStruct* data = padstackGeometry->getNextData(pos);

         if (data->getDataType() == dataTypeInsert)
         {
            BlockStruct* toolBlock = getCamCadDoc()->getBlockAt(data->getInsert()->getBlockNumber());

            if (toolBlock->isTool())
            {
               toolFlag = true;
            }
         }
         else
         {
            if (data->getLayerIndex() == topLayerIndex)
            {
               topPadFlag = true;
            }
            else if (data->getLayerIndex() == bottomLayerIndex)
            {
               bottomPadFlag = true;
            }
         }
      }

      for (POSITION pos = mirroredPadstackGeometry->getHeadDataPosition();pos != NULL && !retval;)
      {
         DataStruct* data = mirroredPadstackGeometry->getNextData(pos);

         if (data->getDataType() == dataTypeInsert)
         {
            BlockStruct* toolBlock = getCamCadDoc()->getBlockAt(data->getInsert()->getBlockNumber());

            if (toolBlock->isTool())
            {
               toolFlag = true;
            }
         }
         else
         {
            if (data->getLayerIndex() == topLayerIndex)
            {
               if (topPadFlag)
               {
                  retval = true;
                  mirroredFlag = true;
               }
            }
            else if (data->getLayerIndex() == bottomLayerIndex)
            {
               if (bottomPadFlag)
               {
                  retval = true;
                  mirroredFlag = true;
               }
            }
         }
      }
   }

   if (toolFlag)
   {
      retval = false;
   }

   return retval;
}

/*
   mirrorShape        masterShape       mirroredFlag 
   data on layer      data on layer
      n    n'           n    n'
      f    f            x    x               x
      f    t            f    f               x
      f    t            f    t               t
      f    t            t    f               f
      f    t            t    t               x
      t    f            f    f               x
      t    f            f    t               f
      t    f            t    f               t
      t    f            t    t               x
      t    t            x    x               x
*/
bool performDataContentCheck(const ORCADShape* masterShape,const ORCADShape* mirroredShape,bool& mirroredFlag)
{
   bool retval = false;
   mirroredFlag = true;

   BlockStruct* masterGeometry   = masterShape->getBlock();
   BlockStruct* mirroredGeometry = mirroredShape->getBlock();

   if (masterGeometry != NULL && mirroredGeometry != NULL)
   {
      CByteArray mirroredShapeDataLayerFlags;
      CByteArray masterShapeDataLayerFlags;

      mirroredShapeDataLayerFlags.SetSize(getCamCadDoc()->getLayerCount());
      masterShapeDataLayerFlags.SetSize(getCamCadDoc()->getLayerCount());

      for (POSITION pos = mirroredGeometry->getHeadDataPosition();pos != NULL;)
      {
         DataStruct* data = mirroredGeometry->getNextData(pos);

         if (data->getDataType() != dataTypeInsert)
         {
            mirroredShapeDataLayerFlags.SetAt(data->getLayerIndex(),1);
         }
      }

      for (POSITION pos = masterGeometry->getHeadDataPosition();pos != NULL;)
      {
         DataStruct* data = masterGeometry->getNextData(pos);

         if (data->getDataType() != dataTypeInsert)
         {
            masterShapeDataLayerFlags.SetAt(data->getLayerIndex(),1);
         }
      }

      for (int layerIndex = 0;layerIndex < mirroredShapeDataLayerFlags.GetSize();layerIndex++)
      {
         LayerStruct* layer = getCamCadDoc()->getLayerAt(layerIndex);

         if (layer != NULL && layer->hasMirroredLayer())
         {
            int mirroredLayerIndex = layer->getMirroredLayerIndex();

            if (mirroredShapeDataLayerFlags.GetAt(layerIndex) != mirroredShapeDataLayerFlags.GetAt(mirroredLayerIndex))
            {
               if (masterShapeDataLayerFlags.GetAt(layerIndex) != masterShapeDataLayerFlags.GetAt(mirroredLayerIndex))
               {
                  retval = true;

                  if (mirroredShapeDataLayerFlags.GetAt(layerIndex)         != masterShapeDataLayerFlags.GetAt(layerIndex)        ||
                      mirroredShapeDataLayerFlags.GetAt(mirroredLayerIndex) != masterShapeDataLayerFlags.GetAt(mirroredLayerIndex)    )
                  {
                     mirroredFlag = false;
                  }
               }
            }
         }

         if (retval && !mirroredFlag)
         {
            break;
         }
      }
   }

   return retval;
}

/******************************************************************************
* IsMasterSYMmirrored
   - Check to see if the original(master) SYM is a created mirrored.
   - It is created mirrored if it is the same as a mirrored derived SYM
*/
static bool IsMasterSYMmirrored(int masterIndex, int mirroredIndex)
{
   bool retval = true;

   if (masterIndex < 0 || mirroredIndex < 0 || masterIndex == mirroredIndex)
   {
      return false;
   }

   ORCADShape *masterShape     = getOrcadShape(masterIndex);
   ORCADShape *mirroredShape   = getOrcadShape(mirroredIndex);
   bool validCheckFlag         = false;
   bool validPadstackCheckFlag = false;
   bool padstackMirroredFlag   = false;

   for (int i=0; i < masterShape->getPinCnt(); i++)
   {
      MINPin *pin = &masterShape->getPin(i);

      BOOL diffLoc = FALSE;
      BOOL diffName = TRUE;

      for (int j=0; j < mirroredShape->getPinCnt(); j++)
      {
         MINPin *mirroredPin = &mirroredShape->getPin(j);

         if (mirroredPin->getPinName().CompareNoCase(pin->getPinName()) == 0)
         {
            diffName = FALSE;

            if (fabs(mirroredPin->getX() - pin->getX()) > SMALLNUMBER)
            {
               diffLoc = TRUE;
               validCheckFlag = true;
            }
            else if (!validPadstackCheckFlag && fabs(pin->getX()) <= SMALLNUMBER)
            {
               validPadstackCheckFlag = performPadstackContentCheck(pin,mirroredPin,padstackMirroredFlag);
            }

            break;
         }
      }

      if (diffLoc || diffName)
      {
         retval = false;
         break;
      }
   }

   if (retval && !validCheckFlag)
   {
      if (validPadstackCheckFlag)
      {
         retval = padstackMirroredFlag;
      }
      else
      {
         performDataContentCheck(masterShape,mirroredShape,retval);
      }
   }

   return retval;
}

void changeGeometryToTopDefined(BlockStruct* block)
{   
   for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data == NULL)
      {
         continue;
      }

      switch (data->getDataType())
      {
      case T_INSERT:
         {
            BlockStruct *subBlock = getCamCadDoc()->getBlockAt(data->getInsert()->getBlockNumber());

            if (subBlock->getBlockType() == BLOCKTYPE_PADSTACK/* && shape->isBottomVersion*/)
            {
               // Change the insert to the "TOP" version of the padstack
               CString topPadstackName = (CString)subBlock->getName() + "_Top";
               BlockStruct *topPadstack = Graph_Block_Exists(getCamCadDoc(), topPadstackName, subBlock->getFileNumber());

               if (!topPadstack)
               {
                  // Create the "TOP" version of the padstack
                  topPadstack = CreateReversePadstack(subBlock, topPadstackName);
               }

               data->getInsert()->setBlockNumber(topPadstack->getBlockNumber());
            }
            else if (subBlock->getName().Left(3).MakeUpper() == "SYM")
            {
               // Mirror the poly in this SYM block, and we know only poly is in SYM block.
               // Because SYM block is created by OBS, and OBS only create poly
               POSITION subDataPos = subBlock->getDataList().GetHeadPosition();

               while (subDataPos)
               {
                  DataStruct *subData = subBlock->getDataList().GetNext(subDataPos);

                  if (subData->getDataType() != T_POLY)
                     continue;

                  MirrorPolyOnX(subData->getPolyList());

                  if (!getCamCadDoc()->IsFloatingLayer(subData->getLayerIndex()) && subData->getLayerIndex() != -1)
                  {
                     LayerStruct *layer = getCamCadDoc()->getLayerArray()[subData->getLayerIndex()];

                     if (layer->getMirroredLayerIndex() > 0)
                        subData->setLayerIndex(layer->getMirroredLayerIndex());
                  }
               }
            }

            data->getInsert()->setOriginX(-data->getInsert()->getOriginX());
         }
         break;
      case T_POLY:
         {
            // Change the polylist X coordinate and layer
            MirrorPolyOnX(data->getPolyList());

            if (!getCamCadDoc()->IsFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
            {
               LayerStruct *layer = getCamCadDoc()->getLayerArray()[data->getLayerIndex()];

               if (layer->getMirroredLayerIndex() > 0)
                  data->setLayerIndex(layer->getMirroredLayerIndex());
            }
         }
         break;
      case T_TEXT:
         {
            // Change the text X coordinate and layer
            data->getText()->setMirrored(false);
            //if (data->getText()->isMirrored())
            // data->getText()->mirror &= ~data->getText()->mirror;
            data->getText()->setPntX(-data->getText()->getPnt().x);

            if (!getCamCadDoc()->IsFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
            {
               LayerStruct *layer = getCamCadDoc()->getLayerArray()[data->getLayerIndex()];

               if (layer->getMirroredLayerIndex() > 0)
                  data->setLayerIndex(layer->getMirroredLayerIndex());
            }
         }
         break;
      default:
         break;
      }
   }
}

/******************************************************************************
* MirrorPolyOnX
   - change all the X coordinatior of the polylist to -X
*/
static void MirrorPolyOnX(CPolyList *polylist)
{
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = polylist->GetNext(polyPos);
      if (!poly)
         continue;

      POSITION pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos)
      {
         CPnt *pnt = poly->getPntList().GetNext(pntPos);
         if (!pnt)
            continue;

         pnt->x = -pnt->x;
      }
   }
}

/******************************************************************************
* CreateTopPadstack
   - Create a reverse version of the padstack
*/
static BlockStruct *CreateReversePadstack(BlockStruct *padstack, CString newPadstackName)
{
   BlockStruct *newPadstack = Graph_Block_On(GBO_APPEND, newPadstackName, padstack->getFileNumber(), 0);
   newPadstack->setBlockType(padstack->getBlockType());
   getCamCadDoc()->CopyAttribs(&newPadstack->getAttributesRef(), padstack->getAttributesRef());
   
   POSITION pos = padstack->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = padstack->getDataList().GetNext(pos);
      if (!data)
         continue;
#if CamCadMajorMinorVersion > 406  //  > 4.6
      DataStruct *copy = getCamCadDoc()->getCamCadData().getNewDataStruct(*data,true);
#else
      DataStruct *copy = new DataStruct(*data, TRUE);
#endif
      newPadstack->getDataList().AddTail(copy);

      if (!getCamCadDoc()->IsFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
      {
         LayerStruct *layer = getCamCadDoc()->getLayerArray()[data->getLayerIndex()];
         if (layer->getMirroredLayerIndex() > 0)
            copy->setLayerIndex(layer->getMirroredLayerIndex());
         }
   }

   Graph_Block_Off();
   return newPadstack;
}

/******************************************************************************
* delete_empty_references
*/
static void delete_empty_references(CCEtoODBDoc *doc, CDataList *DataList)
{
   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
         continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         continue;

      BlockStruct *block = getCamCadDoc()->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      if (block->getDataList().GetCount() == 0 && block->getFlags() == 0 && block->getBlockType() == 0)   
      {
         // if the referenced block has no contens and it is not an aperture or other flagged or typed
         // entity, kill it
         RemoveOneEntityFromDataList(doc, DataList, np);
      }
   } 

   return;
} 

/******************************************************************************
* delete_unused_symbolgraphic
*  - this explodes symbol references (silkscreens) from PCB components.
*/
static void delete_unused_symbolgraphic()
{
   // delete empty silkscreen.
   for (int i=0; i<getCamCadDoc()->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getCamCadDoc()->getBlockAt(i);
      if (block == NULL)
         continue;

      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
      {
         delete_empty_references(getCamCadDoc(), &(block->getDataList()));
      }
   }
}

/******************************************************************************
* delete_unused_viadefinition
*/
static void delete_unused_viadefinition()
{
   for (int i=0; i < getOrcadPadstacks().GetSize(); i++)
   {
      MINPadstack *pp = &(getOrcadPadstack(i));

      if (!pp->getUsed() && pp->getSmdLayer() == 0)
      {
         BlockStruct *block = Graph_Block_Exists(getCamCadDoc(), pp->getName(), GlobalPcbFile->getFileNumber());
         if (block)
            getCamCadDoc()->RemoveBlock(block);
      }
   }

   return;
}

/******************************************************************************
* place_sym_pins
*/
static int place_sym_pins()
{
   for (int i=0; i < getOrcadShapes().GetSize(); i++)
   {
      ORCADShape *shape = getOrcadShape(i);
      if (shape->isVia() || shape->isTestPt()) //case 927:if test point, do not use pin in the sym, treat just like via
         continue;

      Graph_Block_On(GBO_APPEND, shape->getName(), GlobalPcbFile->getFileNumber(), 0); // here dummy name

      for (int ii=0; ii<shape->getPinCnt(); ii++)
      {
         // just make sure it is there.
         // a padstack may not be defined, because it was empty !
         int  padstackptr = shape->getPin(ii).getPadStackIndex();

         if (getOrcadPadstack(padstackptr).getGraphicCnt() == 0)  
         {
            // padstack is empty
            fprintf(fLog, "Symbol [%s] uses an empty padstack [%s] -> ignored!\n", shape->getName(), getOrcadPadstack(padstackptr).getName());
            display_error++;
            continue;
         }

         // here need to check that a padstack does not have the same name as a symbol !
         // this can only happen on deleted padstacks !

         BlockStruct *block = Graph_Block_On(GBO_APPEND, getOrcadPadstack(padstackptr).getName(), GlobalPcbFile->getFileNumber(), 0);
         Graph_Block_Off();

         double x = shape->getPin(ii).getX();
         double y = shape->getPin(ii).getY();

         DataStruct *data = Graph_Block_Reference(getOrcadPadstack(padstackptr).getName(), shape->getPin(ii).getPinName(), GlobalPcbFile->getFileNumber(), 
            x, y, DegToRad(0), 0, 1.0, Graph_Level("0", "", 1), TRUE);
         data->getInsert()->setInsertType(insertTypePin);
      }
      Graph_Block_Off();
   }

   return 1;
}

/******************************************************************************
* go_command
   Call function associated with next token.
   Tokens enclosed by () are searched for on the local
   token list.  
*/
int go_command(List *tok_lst, int lst_size)
{
   int i, brk;

   if (!get_tok()) 
   {
      return p_error();
      return -1;
   }

   switch((brk = tok_search(brk_lst, SIZ_BRK_LST)))
   {
   case BRK_ROUND:
      if (get_tok() && (i = tok_search(tok_lst, lst_size)) >= 0)
      {
         int res = (*tok_lst[i].function)();
         if (res < 0)
         {
            return -1;
         }
      }
      else
      {
         if (is_number(token))   // coos in massteck do not have a keyword
         {
            if (polycnt >= MAX_POLY)
            {
               fprintf(fLog, "Too many vertex at %ld\n", ifp_line);
               display_error++;
               polycnt = 0;
               break;
            }

            poly_l[polycnt].f = 0;  // later is here CCW or CW
            poly_l[polycnt].bulge = 0;
            poly_l[polycnt].x = cnv_unit(token);
            if (!get_tok()) 
               return p_error();  // y

            if (!is_number(token))   // coos in massteck do not have a keyword
            {
               fprintf(fLog, "Parsing Error : Token [%s] on line %ld\n", token, ifp_line);
               display_error++;
            }
            else
            {
               poly_l[polycnt].y = cnv_unit(token);
               poly_l[polycnt].layerindex = G.cur_layernum;
               poly_l[polycnt].w = G.cur_width;
               poly_l[polycnt].via = -1;

               // do not do duplicates
               if (polycnt)
               {
                  if (poly_l[polycnt-1].f == poly_l[polycnt].f &&
                        poly_l[polycnt-1].bulge == poly_l[polycnt].bulge &&
                        poly_l[polycnt-1].x == poly_l[polycnt].x &&
                        poly_l[polycnt-1].y == poly_l[polycnt].y &&
                        poly_l[polycnt-1].layerindex == poly_l[polycnt].layerindex &&
                        poly_l[polycnt-1].w == poly_l[polycnt].w &&
                        poly_l[polycnt-1].via == poly_l[polycnt].via)
                        break;
               }


               polycnt++;
            }
         }
         else
         {
            fnull();
         }
      }
      break;

   case BRK_B_ROUND:
      push_tok();
      return 1;
      break;

   default:
      return p_error();
      break;
   }

   if (!get_tok())
      return p_error();
   if (brk != tok_search(brk_lst, SIZ_BRK_LST) - 1)
      return p_error();

   return 1;
}

/******************************************************************************
* loop_command
*/
static int loop_command(List *list, int size)
{
   int repeat = TRUE;

   while (repeat)
   {
      if (go_command(list, size) < 0)   
         return -1;

      if (!get_tok())
      {
         if (feof(ifp))
            return 0;
         return p_error();
      }

      if (tok_search(brk_lst, SIZ_BRK_LST) == BRK_B_ROUND)
         repeat = FALSE;
      push_tok();
   }
   return 1;
}


/******************************************************************************
* fnull
   Skips over any tokens to next }] endpoint.
*/
static int fnull()
{
   int brk_count = 0;
   
   fprintf(fLog, "DEBUG: Token [%s] at %ld unknown\n", token, ifp_line);

   while (TRUE)
   {
      if (get_tok())
      {
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_ROUND:
            ++brk_count;
            break;
         case BRK_B_ROUND:
            if (brk_count-- == 0)
            {
               push_tok();
               return 1;
            }
            break;
         default:
            fprintf(fLog, " -> Token [%s] at %ld unknown\n", token, ifp_line);
            break;
         }
      }
      else
      {
         return p_error();
      }
   }
}

/******************************************************************************
* fskip
   Skips over any tokens to next () endpoint.
*/
static int fskip()
{
   int brk_count = 0;

   while (TRUE)
   {
     if (get_tok())
      {
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_ROUND:
            ++brk_count;
            break;
         case BRK_B_ROUND:
            if (brk_count-- == 0)
            {
               push_tok();
               return 1;
            }
            break;
         default:
            break;
         }
      }
      else
      {
         return p_error();
      }
   }
}

/******************************************************************************
* mass_minheader
*/
static int mass_minheader()
{
   return loop_command(str_lst, SIZ_STR_LST);
}

/******************************************************************************
*/
const CString& COrcadLayer::getName() const 
{
   if (LayerNameFieldNumber == 2)
      return getName2();

   return getName1(); 
}

/******************************************************************************
* mass_name
*/
static int mass_name()
{
   G.namecnt++;
   if (!get_tok())
      return p_error();
   CString name = token;
   name.TrimLeft();
   name.TrimRight();

   switch (G.cur_status)
   {
   case STAT_LAYER:
      {
         switch (G.namecnt)
         {
         case 1:
            getLastOrcadLayer().setName1(name);
            break;
         case 2:
            getLastOrcadLayer().setName2(name);
            break;
         case 3:
            getLastOrcadLayer().setName3(name);
            break;
         default:
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
            break;
         }
      }
      break;

   case STAT_THRU:
      {
         if (G.namecnt == 1)
         {
            BlockStruct *block = Graph_Block_Exists(getCamCadDoc(), name, GlobalPcbFile->getFileNumber());
            if (block)
            {
               fprintf(fLog, " Padstack [%s] already defined ->changed to %s_%d\n",
                  name, name, getLastOrcadPadstack().getIndex());

               CString tmp1;
               tmp1.Format("THRU_%d", getLastOrcadPadstack().getIndex());
               name = tmp1;
            }
            getLastOrcadPadstack().setMirrorDefined(false);

            CString newName = token;
            // Remove adding of mirrorDefined = TRUE because true pad stacks must be used for accurate post processing
//#define UN_MIR_PADSTACK
#ifdef  UN_MIR_PADSTACK
				if (newName.Right(2) == "-M")
               getLastOrcadPadstack().mirrorDefined = TRUE;
#endif

            getLastOrcadPadstack().setName(name);
         }
         else
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
      }
      break;

   case STAT_SHAPE:
      {
         if (G.namecnt == 1)
         {
            getLastOrcadShape()->setName(name);
            //if ((getLastOrcadShape()->name = strdup(name)) == NULL)
            //   MemErrorMessage(__FILE__, __LINE__);
         }
         else
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
      }
      break;

   case STAT_COMP:
      {
         if (G.namecnt == 1)
            getLastOrcadComponentInstance().setName( name );
         else if (G.namecnt == 2)
            getLastOrcadComponentInstance().setPartName( name );
         else
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
      }
      break;

   case STAT_OBS:
      {
         if (G.namecnt == 1)
            strcpy(cur_obs.name, name);
         else
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
      }
      break;

   case STAT_PACKAGE:
      {
         if (G.namecnt == 1)
         {
            getLastOrcadPackage().setName(name);

         }
         else
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
      }
      break;

   case STAT_NET:
      {
         if (G.namecnt == 1)
         {
            getLastOrcadNet().setName(name);
            add_net(GlobalPcbFile, token);
         }
         else
            fprintf(fLog, "Undefined namecnt at %ld\n", ifp_line);
      }
      break;

   default:
      {
         fprintf(fLog, "Undefined status in mass_name [%s]\n", name);
      }
      break;
   }

   return 1;
}

/******************************************************************************
* mass_prosa
*/
static int mass_prosa()
{
   if (!get_tok())
      return p_error();

   if (strlen(cur_text.prosa) == 0)
      strcpy(cur_text.prosa,token);

   return 1;
}

/******************************************************************************
* mass_begin
*/
static int mass_begin()
{
   return loop_command(begin_lst, SIZ_BEGIN_LST);
}

/******************************************************************************
* begin_header
*/
static int begin_header()
{
   return loop_command(header_lst, SIZ_HEADER_LST);
}

/******************************************************************************
* begin_layer
*/
static int begin_layer()
{
   G.cur_status = STAT_LAYER;

   if (!get_tok())
      return p_error();  // layer number
   int index = atoi(token);

   G.namecnt = 0;
   if (getOrcadLayers().GetSize() < MAX_LAYERS)
   {
      COrcadLayer& layer = getOrcadLayers().getDefinedLayer(getOrcadLayers().GetSize());
      layer.index = index;
      loop_command(layer_lst,SIZ_LAYER_LST);
   }
   else
   {
      fprintf(fLog, "Too many layers\n");
      display_error++;
      return -1;
   }

   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* begin_thru
*/
static int begin_thru()
{
   G.cur_status = STAT_THRU;

   if (!get_tok())
      return p_error();  // thru
   int index = atoi(token);

   G.namecnt = 0;
   G.cur_layernum = 0;  // is set to -1 than UNDEF was encountered.

   MINPadstack *padstack = new MINPadstack("", false, index, 0, false, 0, false);
   getOrcadPadstacks().Add(padstack);
   // increment on the end, because loop uses padstackcnt

   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, "THRU", GlobalPcbFile->getFileNumber(), 0);
   curblock->setBlockType(BLOCKTYPE_PADSTACK);

   G.cur_padstackgraphic = 0;
   loop_command(thru_lst, SIZ_THRU_LST);

   curblock->setName(padstack->getName());

   if (padstack->getSmdLayer() == 1)
      getCamCadDoc()->SetAttrib(&curblock->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);
   else if (getLastOrcadPadstack().getSmdLayer() == 2)
      getCamCadDoc()->SetAttrib(&curblock->getAttributesRef(), getCamCadDoc()->RegisterKeyWord("ODDSMD", 0, VT_NONE), VT_NONE, NULL, SA_OVERWRITE, NULL);

   if (padstack->getTestPt())
      getCamCadDoc()->SetAttrib(&curblock->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_TEST, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);

   Graph_Block_Off();

   // delete the padstack if no graphic is defined in the padstack. All UNDEFS
   padstack->setGraphicCnt( G.cur_padstackgraphic );

   if (G.cur_padstackgraphic == 0)
      getCamCadDoc()->RemoveBlock(curblock);

   // increment on the end, because loop uses padstackcnt
   //padstackcnt++;

   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* geom_already_exist
*/
static int geom_already_exist(const char *n)
{
   BlockStruct *block = Graph_Block_Exists(getCamCadDoc(), n, GlobalPcbFile->getFileNumber());
   if (block)
      return TRUE;

   return FALSE;
}

/******************************************************************************
* begin_sym
*/
static int begin_sym()
{
   G.cur_status = STAT_SHAPE;

   if (!get_tok())
      return p_error(); // sym
   int index = atoi(token);

   G.namecnt = 0;

   ORCADShape *shape = new ORCADShape(index);
   getOrcadShapes().Add(shape);

   //if ((shape->pin = (MINPin*)calloc(MAX_PINPERCOMP, sizeof(MINPin))) == NULL)
   //   MemErrorMessage(__FILE__, __LINE__);
   
   // here make symbol block and graphic. This is needed for the derive statement.
   CString symname;
   symname.Format("SYM %d", index);
   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, symname, GlobalPcbFile->getFileNumber(), 0);
   Graph_Block_Off();

   curblock = Graph_Block_On(GBO_APPEND, "SYM", GlobalPcbFile->getFileNumber(), 0); // here dummy name
   shape->setBlock(curblock);

   if (loop_command(sym_lst, SIZ_SYM_LST) < 0)
   {
      //shapeArrayCount++;
      return -1;
   }

   if (shape->isVia() || shape->isTestPt()) //case 927:if test point, do not use pin in the sym, treat just like via
   {
      Graph_Block_Off();
      
      getCamCadDoc()->RemoveBlock(curblock); // delete block
      shape->setBlock(NULL);
      
      BlockStruct *block = Graph_Block_Exists(getCamCadDoc(), symname, GlobalPcbFile->getFileNumber());
      if (block)
         getCamCadDoc()->RemoveBlock(block);
   }
   else
   {
      CString shapeName = shape->getName();
      
      if (Graph_Block_Exists(getCamCadDoc(), shapeName, GlobalPcbFile->getFileNumber()))
      {
         int prev_index = get_shapeptr_from_name(shapeName);
         if (prev_index < 0)
         {
            fprintf(fLog," Name [%s] [Sym %d] already used ->changed to %s_%d\n", 
               shapeName, index, shapeName, shape->getIndex());
            display_error++;
         }
         else
         {
            fprintf(fLog," Name [%s] [Sym %d] already used in [Sym %d] ->changed to %s_%d\n", 
               shapeName, index, getOrcadShape(prev_index)->getIndex(), shapeName, shape->getIndex());
            display_error++;
         }

         CString newShapeName;
         newShapeName.Format("%s_%d", shapeName, shape->getIndex());
         shapeName = newShapeName;
         shape->setName(shapeName);
         //free(shape->name);
         //if ((shape->name = strdup(shapeName)) == NULL)
         //   MemErrorMessage(__FILE__, __LINE__);
      }

      if (!geom_already_exist(shapeName)) // a padstack can have the same name as a SYMBOL.
         curblock->setName(shapeName);
      else
      {
         CString tmp;
         tmp.Format("SYM_%s", shapeName);
         curblock->setName(tmp);
         fprintf(fLog, "Symbol Name [%s] for [SYM %d] already defined.\n", shapeName, index);
         display_error++;
      }

      if (shape->getPinCnt() != 0)
      {
         // make sure the pins are not getting kicked out later
         BOOL HasPins = FALSE;
         for (int i=0; i<shape->getPinCnt(); i++)
         {
            int padstackIndex = shape->getPin(i).getPadStackIndex();
            if (getOrcadPadstack(padstackIndex).getGraphicCnt() != 0)
            {
               HasPins = TRUE;
               break;
            }
         }

         if (HasPins)
         {
            if (shape->isTestPt())
				{
               curblock->setBlockType(BLOCKTYPE_PADSTACK); //case 927 :make test points block type as padstacks
				}
            else
               curblock->setBlockType(BLOCKTYPE_PCBCOMPONENT);
         }
      }

      // make sure it exist, because forward pointering (derive from a symbol which is not yet defined) is allowed
      Graph_Block_On(GBO_APPEND, symname, GlobalPcbFile->getFileNumber(), 0); 
      Graph_Block_Off();
      // the graphic is always from the derived symbol.
      Graph_Block_Reference(symname, NULL, GlobalPcbFile->getFileNumber(), 0.0, 0.0, DegToRad(0), 0, 1.0, 0, TRUE); // this is the silkscreen
      Graph_Block_Off();
   }

   shape->setNumPins(shape->getPinCnt());

   //if (shape->pincnt == 0)
   //   free(shape->pin);
   //else
   //   shape->pin = (MINPin *)realloc(shape->pin, shape->pincnt*sizeof(MINPin));

   //shapeArrayCount++;

   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* begin_net
*/
static int begin_net()
{
   G.cur_status = STAT_NET;
   if (!get_tok()) 
      return p_error();  //
   int index = atoi(token);

   G.namecnt = 0;

   ORCADNet *n = new ORCADNet("", index);
   getOrcadNets().Add(n);

   loop_command(net_lst, SIZ_NET_LST);
   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* net_width
*/
static int net_width()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* net_basewidth
*/
static int net_basewidth()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* net_min
*/
static int net_min()
{
   if (!get_tok())
      return p_error();
   
   return 1;
}

/******************************************************************************
* net_max
*/
static int net_max()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* net_basemin
*/
static int net_basemin()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* net_basemax
*/
static int net_basemax()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* begin_text
*/
static int begin_text()
{
   G.cur_status = STAT_NET;
   if (!get_tok())
      return p_error();  //
   int index = atoi(token);

   G.namecnt = 0;
   polycnt = 0;
   cur_obs.kind = 0;

   // prosa is NULLED here.
   memset(&cur_text, 0, sizeof(MINText));
   cur_text.aspect = 100;

   loop_command(text_lst, SIZ_TEXT_LST);

   // cur_obs.kind is correct in text
   if (cur_obs.kind == OBS_KIND_DRILLCHART && drillchart == FALSE)
   {
      G.cur_status = STAT_UNKNOWN;
      polycnt = 0;
      return 1;
   }

   write_text();
   G.cur_status = STAT_UNKNOWN;
   return 1;
}

/******************************************************************************
* begin_color
*/
static int begin_color()
{
   fskip();
   return 1;
}

/****************************************************************************/
/*
(Attr Rpt 24 Noattr 0 ReportComment "Assembly Bottom" 0 Uid 0)
(Attr Rpt 25 Noattr 0 ReportComment "Drill Drawing" 0 Uid 0)
(Attr Comp 9 Count 3 Named "DB25" 0 Uid 0)
(Attr Comp 9 Count 4 Named "JDBB000025" 0 Uid 0)
*/
static int begin_attr()
{
   if (!get_tok())
      return p_error();
   CString majkind = token;

   if (!get_tok())
      return p_error();  
   int index = atoi(token);

   if (!get_tok())
      return p_error();  
   CString minkind = token;

   if (!get_tok())
      return p_error();  
   CString key = token;

   if (!get_tok())
      return p_error();  
   CString named = token;

   if (!get_tok())
      return p_error();  
   CString val = token;

   if (!get_tok())
      return p_error();  
   CString spare = token;

   if (!get_tok())
      return p_error();  
   CString uid = token;

   if (!get_tok())
      return p_error();  
   CString uidnum = token;

   if (!majkind.CompareNoCase("Comp"))
   {
      // attach to component.
      int cptr = get_compptr(index);
      if (cptr > -1)
      {
         DataStruct *data = getOrcadComponentInstance(cptr).getComponentInsert();
         if (data)
            getCamCadDoc()->SetUnknownAttrib(&data->getAttributesRef(),get_attrmap(key), val, SA_OVERWRITE, NULL); // x, y, rot, height
      }
   }
   else if (!majkind.CompareNoCase("Net"))
   {
      // attach a net to a plane layer
      int netptr = get_netptr(index);

      if (netptr < 0)
         sprintf(cur_netname, "Net %d", index);
      else
         strcpy(cur_netname, getOrcadNets().GetAt(netptr)->getName());

      if (!minkind.CompareNoCase("Lev"))
      {
         int lay = atoi(key);
         if (lay >= 1 && lay <= G.electrical_layers)
         {
            if (getOrcadLayer(lay).plane)
            {
               LayerStruct *layer = getCamCadDoc()->FindLayer_by_Name(getOrcadLayer(lay).getName());
               if (layer)
               {
                  getCamCadDoc()->SetUnknownAttrib(&layer->getAttributesRef(), ATT_NETNAME,cur_netname, SA_APPEND, NULL); 

                  // update layer as a powerplane - powernet
                  NetStruct *net = add_net(GlobalPcbFile, cur_netname);
                  getCamCadDoc()->SetAttrib(&net->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_POWERNET, 0), VT_NONE, NULL,SA_OVERWRITE, NULL); 
               }
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* begin_package
*/
static int begin_package()
{
   G.cur_status = STAT_PACKAGE;
   G.namecnt = 0;

   if (!get_tok())
      return p_error();  
   int index = atoi(token);

   MINPackage *a = new MINPackage("", index, false);
   getOrcadPackages().Add(a);

   loop_command(package_lst,SIZ_PACKAGE_LST);
   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* begin_c
*/
static int begin_c()
{
   polycnt = 0;
   loop_command(conn_lst, SIZ_CONN_LST);
   if (polycnt == 0)
      return 1;

   int cont = FALSE;
   CPnt *last_vertex = NULL;

   for (int i=0; i<polycnt; i++)
   {
      if (poly_l[i].f)
      {
         // i-1 is start
         // i   is  center
         // i+1 is end
         double rad, sa, da;
         ArcCenter2(poly_l[i-1].x, poly_l[i-1].y, poly_l[i+1].x, poly_l[i+1].y, poly_l[i].x, poly_l[i].y, &rad, &sa, &da, 0);

         if (fabs(da) > PI)   // center can have 2 solutions. take the smaller one.
            da = da - PI2;
         if (last_vertex)
            last_vertex->bulge = (DbUnit)tan(da/4);

         continue;
      }

      if (poly_l[i].via > -1)
      {
         if (cont)   // finish the from last point to the via.
         {
            if (poly_l[i-1].layerindex)   // do not translate CONN layer
               last_vertex = Graph_Vertex(poly_l[i].x, poly_l[i].y, 0);
         }

         int p = get_padstackptr(poly_l[i].via);
         if (p < 0)
         {
            fprintf(fLog, "Padstack Error for Via at line %ld\n", ifp_line);
         }
         else
         {
            getOrcadPadstack(p).setUsed(true);
            DataStruct *data = Graph_Block_Reference(getOrcadPadstack(p).getName(), NULL, GlobalPcbFile->getFileNumber(), poly_l[i].x, poly_l[i].y, 0, 0, 1.0,
                  Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypeVia);

            if (strlen(cur_netname))
               getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname, SA_APPEND, NULL);
         }
         
         // only do it if the via is not the last entry.
         if (i < polycnt-1 && poly_l[i].layerindex)
         {
            int layer = Graph_Level(getOrcadLayer(poly_l[i].layerindex).getName(),"",0);
            int widthindex = 0; // poly_l[i].w
            int err = 0;
      
            if (poly_l[i].w == 0)
               widthindex = 0;
            else
               widthindex = Graph_Aperture("", T_ROUND, poly_l[i].w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

            DataStruct *data = Graph_PolyStruct(layer, 0L, 0);
            data->setGraphicClass(GR_CLASS_ETCH);

            if (strlen(cur_netname))
               getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname, SA_APPEND, NULL); 
   
            Graph_Poly(NULL,widthindex, 0, 0, 0);
            if (poly_l[i].layerindex)  // do not translate CONN layer
            {
               last_vertex = Graph_Vertex(poly_l[i].x, poly_l[i].y, 0);
               cont = TRUE;
            }
         }
      }
      else if (!cont && poly_l[i].layerindex)
      {
         int layer = Graph_Level(getOrcadLayer(poly_l[i].layerindex).getName(), "", 0);
         int widthindex = 0; 
         int err = 0;
   
         if (poly_l[i].w == 0)
            widthindex = 0;
         else
            widthindex = Graph_Aperture("", T_ROUND, poly_l[i].w , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

         DataStruct *data = Graph_PolyStruct(layer,0L,0);
         data->setGraphicClass(GR_CLASS_ETCH);

         if (strlen(cur_netname))
            getCamCadDoc()->SetAttrib(&data->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname, SA_APPEND, NULL); 

         Graph_Poly(NULL, widthindex, 0, 0, 0);
         cont = TRUE;
         last_vertex = Graph_Vertex(poly_l[i].x, poly_l[i].y, 0);
      }
      else
      {
         if (cont && poly_l[i-1].layerindex) // do not translate CONN layer
            last_vertex = Graph_Vertex(poly_l[i].x, poly_l[i].y, 0);
      }

      
      if (!poly_l[i].layerindex) // do not translate CONN layer
         cont = FALSE;

      if (i && poly_l[i-1].w != poly_l[i].w)
      {
         int layer = Graph_Level(getOrcadLayer(poly_l[i].layerindex).getName(),"",0);
         int widthindex = 0; // poly_l[i].w
         int err = 0;
   
         if (poly_l[i].w == 0)
            widthindex = 0;
         else
            widthindex = Graph_Aperture("", T_ROUND, poly_l[i].w , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

         DataStruct *data = Graph_PolyStruct(layer, 0L, 0);
         data->setGraphicClass(GR_CLASS_ETCH);

         if (strlen(cur_netname))
            getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING,cur_netname,SA_APPEND, NULL); 

         Graph_Poly(NULL,widthindex, 0, 0, 0);
         cont = TRUE;
         last_vertex = Graph_Vertex(poly_l[i].x, poly_l[i].y, 0);
      }
   }

   polycnt = 0;
   return 1;
}

/******************************************************************************
* begin_seg
*/
static int begin_seg()
{
   polycnt = 0;
   loop_command(conn_lst, SIZ_CONN_LST);

   if (polycnt == 0) 
      return 1;

   int cont = FALSE;

   for (int i=0; i<polycnt; i++)
   {
      if (!cont)
      {
         int layer = Graph_Level(getOrcadLayer(poly_l[i].layerindex).getName(), "", 0);
         int widthindex = 0; // poly_l[i].w
         int err = 0;
   
         if (G.cur_width == 0)
            widthindex = 0;
         else
            widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

         DataStruct *data = Graph_PolyStruct(layer,0L,0);
         data->setGraphicClass(GR_CLASS_ETCH);

         if (strlen(cur_netname))
            getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING,cur_netname,SA_APPEND, NULL); 

         Graph_Poly(NULL,widthindex, 0, 0, 0);
         cont = TRUE;
      }

      if (poly_l[i].layerindex)
         Graph_Vertex(poly_l[i].x, poly_l[i].y, 0);

      if (poly_l[i].via > -1)
      {
         int p = get_padstackptr(poly_l[i].via);
         if (p < 0)
         {
            fprintf(fLog, "Padstack Error for Via at line %ld\n", ifp_line);
         }
         else
         {
            getOrcadPadstack(p).setUsed(true);
            DataStruct *data = Graph_Block_Reference(getOrcadPadstack(p).getName(), NULL, GlobalPcbFile->getFileNumber(), poly_l[i].x, poly_l[i].y, 0, 0, 1.0,
                  Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypeVia);

            if (strlen(cur_netname))
               getCamCadDoc()->SetAttrib(&data->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname, SA_APPEND, NULL);
         }
         cont = FALSE;
      }
   }

   polycnt = 0;
   return 1;
}

/******************************************************************************
* conn_w
*/
static int conn_w()
{
   if (!get_tok())
      return p_error();

   if (width_in_mil)
      G.cur_width = cnv_conn(token);
   else
      G.cur_width = cnv_unit(token);

   if (polycnt > 0)
      poly_l[polycnt-1].w = G.cur_width;

   return 1;
}

/******************************************************************************
* conn_basew
*/
static int conn_basew()
{
   if (!get_tok())
      return p_error();  //
   G.cur_width = cnv_unit(token);

   if (polycnt > 0)
      poly_l[polycnt-1].w = G.cur_width;

   return 1;
}

/******************************************************************************
* comppin_alreadydone
   Check if a comp/pin already loaded.
*/
static int  comppin_alreadydone(int c, int p)
{
   for (int i=0; i < comppinarray.GetSize(); i++)
   {
      ORCADCompPin* cp = comppinarray.GetAt(i);
      if (cp->getC() == c && cp->getP() == p)
         return 1;         
   }

   ORCADCompPin* cp = new ORCADCompPin(c, p);
   comppinarray.Add(cp);  

   return 0;
}

/******************************************************************************
* conn_p
*/
static int conn_p()
{
   if (!get_tok())
      return p_error();  // comp index
   int cc = atoi(token);
   int c = get_compptr(atoi(token));
	if (cc == 4110)
		int xx = 0;

   if (!get_tok()) 
      return p_error();  // pin  index
   int pp = atoi(token);

   //case 927: we need to add the netname attrib to the test points
   if (getOrcadComponentInstance(c).isVia() || getOrcadComponentInstance(c).isTestPt())
   {
      DataStruct *data = getOrcadComponentInstance(c).getComponentInsert(); 
      if (data)
      {
         if (strlen(cur_netname))
            getCamCadDoc()->SetUnknownAttrib(&data->getAttributesRef(), ATT_NETNAME, cur_netname,SA_OVERWRITE, NULL); 
      }
   }
   else
   {
      int p;
      int s;

      if ((p = atoi(token)) < 1)
      {
         fprintf(fLog, "Error in pin index at %ld\n", ifp_line);
         return -1;  
      }
   
      if ((s = get_shapeptr(getOrcadComponentInstance(c).getSymId())) < 0)
      {
         fprintf(fLog, "Error in conn_p at %ld\n", ifp_line);
         return -1;
      }

      // check this is was not done before
		if (!comppin_alreadydone(cc, pp)/* && !getOrcadComponentInstance(c).testpt*/)
      {
         CString compname = getOrcadComponentInstance(c).getName();
         CString pinname = getOrcadShape(s)->getPin(p-1).getPinName();

         NetStruct *net = add_net(GlobalPcbFile, cur_netname);
         add_comppin(GlobalPcbFile, net, compname, pinname);

         if (getOrcadComponentInstance(c).isNonElectric())
         {
            DataStruct *data = getOrcadComponentInstance(c).getComponentInsert(); 
            //datastruct_from_refdes(getCamCadDoc(), file->getBlock(), getOrcadComponentInstance(c).name);

            // i have seen NonElectric components in the netlist
            data->getInsert()->setInsertType(insertTypePcbComponent); 
         }
      }
   }

   return 1;
}

/******************************************************************************
* conn_net
*/
static int conn_net()
{
   if (!get_tok())
      return p_error();  //
   int netptr = get_netptr(atoi(token));

   if (netptr < 0)
   {
      sprintf(cur_netname, "Net %d", atoi(token));
      NetStruct *net = add_net(GlobalPcbFile, cur_netname);
   }
   else
   {
      strcpy(cur_netname, getOrcadNets().GetAt(netptr)->getName());
   }

   return 1;
}

/******************************************************************************
* obs_net
*/
static int obs_net()
{
   if (!get_tok())
      return p_error();  //
   cur_obs.net = atoi(token);

   return 1;
}

/******************************************************************************
* get_layerindex
*/
static int get_layerindex(int li)
{
   for (int i=0;i < getOrcadLayers().GetSize(); i++)
   {
      if (getOrcadLayer(i).index == li)
         return i;
   }
   
   fprintf(fLog, "Layerindex [%d] not found at %ld\n", li, ifp_line);
   display_error++;

   return -1;
}

/******************************************************************************
* conn_l
*/
static int conn_l()
{
   if (!get_tok())
      return p_error();  
   G.cur_layernum = get_layerindex(atoi(token));

   if (polycnt > 0)
      poly_l[polycnt-1].layerindex = G.cur_layernum;

   return 1;
}

/******************************************************************************
* text_l
*/
static int text_l()
{
   if (!get_tok()) 
      return p_error();  
   G.cur_layernum = get_layerindex(atoi(token));

   return 1;
}

/******************************************************************************
* conn_v
*/
static int conn_v()
{
   if (!get_tok()) 
      return p_error();  
   if (polycnt > 0)
      poly_l[polycnt-1].via = atoi(token);

   return 1;
}

/******************************************************************************
* conn_arc
   the last coo is the center of the arc. The bulge needs to be calculated by
   the start, center, end.
*/
static int conn_arc()
{
   if (polycnt > 0)
      poly_l[polycnt-1].f = CONN_ARC;

   return 1;
}

/******************************************************************************
* get_padformindex
*/
static int get_padformindex(int f,double sizeA, double sizeB, double rotation, double xOffset, double yOffset)
{
   for (int i=0; i < padformarray.GetSize(); i++)
   {
      ORCADPadform& p = *(padformarray.GetAt(i));
      if (p.getForm() == f && p.getSizeA() == sizeA && p.getSizeB() == sizeB &&
         p.getRotation() == rotation && p.getOffsetX() == xOffset && p.getOffsetY() == yOffset)
         return i;         
   }

   CString name;
   name.Format("PADSHAPE_%d", padformarray.GetSize());
   int err;
   Graph_Aperture(GlobalPcbFile->getFileNumber(), name, f, sizeA, sizeB, xOffset, yOffset, DegToRad(rotation), 0, BL_APERTURE, TRUE, &err);

   ORCADPadform* p = new ORCADPadform(f, sizeA, sizeB, xOffset, yOffset, rotation);
   padformarray.Add(p);  

   return padformarray.GetSize() - 1;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size, int layernum)
{
   if (size == 0) 
      return -1;

   for (int i=0; i < drillArray.GetSize(); i++)
   {
      ORCADDrill* p = drillArray.GetAt(i);
      if (p->getDiameter() == size)
         return i;         
   }

   CString name;
   name.Format("DRILL_%d", drillArray.GetSize());

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   ORCADDrill* p = new ORCADDrill(size);
   drillArray.Add(p);  

   return drillArray.GetSize() - 1;
}

/******************************************************************************
* thru_testpt
*/
static int thru_testpt()
{
   if (!get_tok()) 
      return p_error(); 
   CString t = token;
   t.TrimLeft();
   t.TrimRight();

   if (!STRCMPI(token, ")"))
      push_tok();

   if (strlen(t) == 0 || !STRCMPI(t,"On"))
      getLastOrcadPadstack().setTestPt(true);

   return 1;
}

/******************************************************************************
* thru_l
  round : poly_l[0].x is type
          poly_l[1].x is radius
  UNDEF : no polycnt happened
  square: poly_l[1].x is side
*/
static int thru_l()
{
   if (!get_tok())  
      return p_error();  //
   int layerIndex = get_layerindex(atoi(token));

   CString layerName = getOrcadLayer(layerIndex).getName();
   int layerNum = Graph_Level(layerName, "", 0);

   polycnt = 0;
   G.cur_rotation = 0;
   G.cur_layernum = 0;  // if set to -1, UNDEF found

   loop_command(padform_lst, SIZ_PADFORM_LST);

   if (G.cur_layernum == -1 || getOrcadLayer(layerIndex).unused)
   {
      polycnt = 0;
      return 1;
   }

   G.cur_padstackgraphic++;

   if (getLastOrcadPadstack().getMirrorDefined())
   {
      int mirrorIndex = getOrcadLayer(layerIndex).mirror;
      layerName = getOrcadLayer(mirrorIndex).getName();
      layerNum = Graph_Level(layerName, "", 0);
   }

   int pad_type = -1;
   double pad_height, pad_width, xOffset = 0, yOffset = 0;

   if (polycnt == 0) return 1;
   
   switch (round(poly_l[0].x))
   {
   case PAD_ROUND:
      {
         pad_type = T_ROUND;
         pad_width = poly_l[1].x * 2;  // left right
         pad_height = 0;
      }
      break;
   case PAD_SQUARE:
      {
         pad_type = T_SQUARE;
         pad_width = poly_l[1].x * 2;  // left right
         pad_height = 0;
      }
      break;
   case PAD_THERMAL:
      {
         pad_type = T_THERMAL;
         pad_width = poly_l[2].x - poly_l[1].x;
         pad_height = poly_l[2].y - poly_l[1].y;
         xOffset = (pad_width)/2 + poly_l[1].x;
         yOffset = (pad_height)/2 + poly_l[1].y;
      }
      break;
   case PAD_RECT:
      {
         pad_type = T_RECTANGLE;
         pad_width = poly_l[2].x - poly_l[1].x;
         pad_height = poly_l[2].y - poly_l[1].y;
         xOffset = (pad_width)/2 + poly_l[1].x;
         yOffset = (pad_height)/2 + poly_l[1].y;
      }
      break;
   case PAD_OBLONG:
      {
         pad_type = T_OBLONG;
         pad_width = poly_l[2].x - poly_l[1].x;
         pad_height = poly_l[2].y - poly_l[1].y;
         xOffset = (pad_width)/2 + poly_l[1].x;
         yOffset = (pad_height)/2 + poly_l[1].y;
      }
      break;
   case PAD_ANNULAR:
      {
         pad_type = T_OBLONG;
         pad_width = poly_l[2].x - poly_l[1].x;
         pad_height = poly_l[2].y - poly_l[1].y;
         xOffset = (pad_width)/2 + poly_l[1].x;
         yOffset = (pad_height)/2 + poly_l[1].y;
      }
      break;
   }

   if (getOrcadLayer(layerIndex).drill)
   {
      CString drillname;
      double  drill = pad_width;
      if (pad_type != T_ROUND)
      {
         fprintf(fLog, "Drill form for THRU [%s] is not round at %ld!\n", getLastOrcadPadstack().getName(), ifp_line);
         display_error++;

         if (drill > pad_height)
            drill = pad_height;
      }

      drillname.Format("DRILL_%d", get_drillindex(drill, layerNum));
      Graph_Block_Reference(drillname, NULL, GlobalPcbFile->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layerNum, TRUE);
   }
   else
   {
      int padformIndex = get_padformindex(pad_type, pad_width, pad_height, 0, xOffset, yOffset);

      CString padshapename;
      padshapename.Format("PADSHAPE_%d", padformIndex);
      Graph_Block_Reference(padshapename, NULL, GlobalPcbFile->getFileNumber(), 0.0, 0.0, DegToRad(G.cur_rotation), 0, 1.0, layerNum, TRUE);

      if (getOrcadLayer(layerIndex).etch)
      {
         if (getOrcadLayer(layerIndex).index == 1)
            getLastOrcadPadstack().setSmdLayer( getLastOrcadPadstack().getSmdLayer() | 1 );
         else if (getOrcadLayer(layerIndex).mirror == 1)
            getLastOrcadPadstack().setSmdLayer( getLastOrcadPadstack().getSmdLayer() | 2 );
         else
            getLastOrcadPadstack().setSmdLayer( getLastOrcadPadstack().getSmdLayer() | 4 );
      }

   }
   polycnt = 0;

   return 1;
}

/******************************************************************************
* padround
*/
static int padround()
{
   if (!get_tok()) 
      return p_error();  //

   polycnt = 2;
   poly_l[0].x = PAD_ROUND;
   poly_l[1].x = cnv_unit(token);

   return 1;
}

/******************************************************************************
* padrect
*/
static int padrect()
{
   polycnt = 3;
   poly_l[0].x = PAD_RECT;

   if (!get_tok()) 
      return p_error();  
   poly_l[1].x = -cnv_unit(token);

   if (!get_tok())
      return p_error(); 
   poly_l[1].y = -cnv_unit(token);

   if (!get_tok()) 
      return p_error();  
   poly_l[2].x = cnv_unit(token);

   if (!get_tok())
      return p_error();  
   poly_l[2].y = cnv_unit(token);

   return 1;
}

/******************************************************************************
* padoblong
*/
static int padoblong()
{
   polycnt = 3;
   poly_l[0].x = PAD_OBLONG;

   if (!get_tok()) 
      return p_error(); 
   poly_l[1].x = -cnv_unit(token);

   if (!get_tok()) 
      return p_error();  
   poly_l[1].y = -cnv_unit(token);

   if (!get_tok()) 
      return p_error();  
   poly_l[2].x = cnv_unit(token);

   if (!get_tok()) 
      return p_error();
   poly_l[2].y = cnv_unit(token);

   return 1;
}

/******************************************************************************
* padannular
*/
static int padannular()
{
   polycnt = 3;
   poly_l[0].x = PAD_ANNULAR;

   if (!get_tok())   
      return p_error();
   poly_l[1].x = -cnv_unit(token);

   if (!get_tok()) 
      return p_error();
   poly_l[1].y = -cnv_unit(token);

   if (!get_tok())   
      return p_error(); 
   poly_l[2].x = cnv_unit(token);

   if (!get_tok())   
      return p_error();  
   poly_l[2].y = cnv_unit(token);

   return 1;
}

/******************************************************************************
* padthermal
*/
static int padthermal()
{
   polycnt = 3;
   poly_l[0].x = PAD_THERMAL;

   if (!get_tok())   
      return p_error();  
   poly_l[1].x = -cnv_unit(token);

   if (!get_tok())   
      return p_error(); 
   poly_l[1].y = -cnv_unit(token);

   if (!get_tok())   
      return p_error();
   poly_l[2].x = cnv_unit(token);

   if (!get_tok())
      return p_error();
   poly_l[2].y = cnv_unit(token);

   return 1;
}

/******************************************************************************
* padsquare
*/
static int padsquare()
{
   if (!get_tok()) 
      return p_error(); 

   polycnt = 2;
   poly_l[0].x = PAD_SQUARE;
   poly_l[1].x = cnv_unit(token);

   return 1;
}

/******************************************************************************
* get_packageptr
*/
static int get_packageptr(int index)
{
   for (INT i=0;i < getOrcadPackages().GetSize(); i++)
   {
      if (getOrcadPackages().GetAt(i)->getIndex() == index)
         return i;
   }

   return -1;
}

/******************************************************************************
* begin_comp
*/
static int begin_comp()
{
   G.cur_status = STAT_COMP;
   if (!get_tok())
      return p_error(); 
   int compNum = atoi(token);

   polycnt = 0;
   G.namecnt = 0;

   ORCADCompInst *compInst = new ORCADCompInst;
   // increment on the end, because loop uses padstackcnt
   getOrcadComponentInstances().Add(compInst); 
   compInst->setIndex( compNum );

   loop_command(comp_lst, SIZ_COMP_LST);

   if (!compInst->getName().Left(3).CompareNoCase("~TP"))
	{
		int x = 0;
	}

   ORCADShape *shape = NULL;
   if (compInst->isVia())
   {
      int shapeIndex = get_shapeptr(compInst->getSymId());
      if (shapeIndex < 0)
      {
         fprintf(fLog, "Can not find SYMBOL SYM %d for Component [%s]\n", compInst->getSymId(), compInst->getName());
         display_error++;
         G.cur_status = STAT_UNKNOWN;
         return 1;
      }

      shape = getOrcadShape(shapeIndex);

      compInst->setComponentInsert( Graph_Block_Reference(shape->getName(), compInst->getName(), GlobalPcbFile->getFileNumber(), poly_l[0].x, poly_l[0].y, 0, 0, 1,
            Graph_Level("0", "", 1), TRUE));

		compInst->getComponentInsert()->getInsert()->setInsertType(insertTypeVia);

      int padstackIndex = get_padstacknameptr(shape->getName());
      if (padstackIndex < 0)
         fprintf(fLog, "VIA Name [%s] not found in padlist\n", shape->getName());
      else
         getOrcadPadstack(padstackIndex).setUsed(true);
   }
   else
   {
      // here place the component symbol, not the derived one. 
      double rotation = compInst->getRotation();
      while (rotation < 0)
         rotation += 360;
      while (rotation >= 360)
         rotation -= 360;

      int shapeIndex = get_shapeptr(compInst->getSymId());
	
      compInst->setIndex(compNum);
      shape = getOrcadShape(shapeIndex);

      shape->insrementUsageCount();
      compInst->setComponentInsert( Graph_Block_Reference(shape->getName(), compInst->getName(), GlobalPcbFile->getFileNumber(), poly_l[0].x, poly_l[0].y, DegToRad(rotation), 
         compInst->getMirror(), 1, 0, TRUE));  

      if (compInst->getMirror())
         shape->setBottomDefined(true);

      shape->GetDataList().AddTail(compInst->getComponentInsert());
   }

   if (compInst->isVia())
   {
      compInst->getComponentInsert()->getInsert()->setInsertType(insertTypeVia);
   }
   else if (compInst->isNonElectric())
   {
      compInst->getComponentInsert()->getInsert()->setInsertType(insertTypeUnknown);
   }
   else if (compInst->isTestPt()) //case 927
	{
		//if test point, then do not normalize it
		compInst->setNormalize(FALSE);
		//add test attribute
		getCamCadDoc()->SetAttrib(&compInst->getComponentInsert()->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_TEST, TRUE), VT_STRING, NULL, SA_OVERWRITE, NULL);
		//set it to type via
		compInst->getComponentInsert()->getInsert()->setInsertType(insertTypeVia);
		//do not mirror it
		compInst->getComponentInsert()->getInsert()->setMirrorFlags(0);
		compInst->getComponentInsert()->getInsert()->setPlacedBottom(FALSE);
	}
   else
   {
      BlockStruct *block = getCamCadDoc()->Find_Block_by_Num(compInst->getComponentInsert()->getInsert()->getBlockNumber());
      compInst->getComponentInsert()->getInsert()->setInsertType(default_inserttype(block->getBlockType()));

      if (!compInst->getValue().IsEmpty())
      {
         getCamCadDoc()->SetAttrib(&compInst->getComponentInsert()->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_VALUE, 0), VT_STRING,
            compInst->getValue().GetBuffer(0), SA_OVERWRITE, NULL);
      }
   }

   if (!compInst->isVia())
   {
      // here add type
      if (compInst->getPackageIndex() != 0)
      {
         int packageIndex = get_packageptr(compInst->getPackageIndex());
         if (packageIndex > -1)
         {
            MINPackage *package = getOrcadPackages().GetAt(packageIndex);

            int shapeIndex = get_shapeptr(compInst->getSymId());
            ORCADShape *shape = getOrcadShape(shapeIndex);

            CString packageName;
            if (package->getAppendSymName())
               packageName.Format("%s_%s", package->getName(), shape->getName());
            else
               packageName = package->getName();

         }
      } 
   }

   // increment on the end, because loop uses padstackcnt
   //compinstcnt++;

   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* begin_obs
*/
static int begin_obs()
{
   G.cur_status = STAT_OBS;
   if (!get_tok())
      return p_error(); 
   int index = atoi(token);

   polycnt = 0;
   G.namecnt = 0;
   memset(&cur_obs, 0, sizeof(MINObs));
   loop_command(obs_lst, SIZ_OBS_LST);

   // it seems that outline is not used in ORCAD V9 anymore. A obs with outline flag is 
   // reported as a free track cur_obs.kind OBS_KIND_OUTLINE
   write_obs(index);
   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* obs_keepin
*/
static int obs_keepin()
{
   cur_obs.kind = OBS_KIND_KEEPIN;
   return 1;
}

/******************************************************************************
* mass_drillchart
*/
static int mass_drillchart()
{
   if (!get_tok()) 
      return p_error(); 

   if (!strcmp(token, ")"))
   {
      cur_obs.kind = OBS_KIND_DRILLCHART;
      push_tok();
   }
   else if (!STRCMPI(token, "Off"))
   {
      // do nothing
   }

   return 1;
}

/******************************************************************************
* obs_anynet
*/
static int obs_anynet()
{
   cur_obs.net = OBS_ANY_NET;
   return 1;
}

/******************************************************************************
* obs_nofill
*/
static int obs_nofill()
{
   cur_obs.hatchkind = OBS_HATCHKIND_HOLLOW;
   return 1;
}

/******************************************************************************
* obs_solidfill
*/
static int obs_solidfill()
{
   cur_obs.hatchkind = OBS_HATCHKIND_SOLID;
   return 1;
}

/******************************************************************************
* obs_crossfill
*/
static int obs_crossfill()
{
   cur_obs.hatchkind = OBS_HATCHKIND_SOLID;  // same as solidfill
   return 1;
}

/******************************************************************************
* obs_poured
*/
static int obs_poured()
{
   cur_obs.poured = TRUE;
   return 1;
}

/******************************************************************************
* obs_keepout
*/
static int obs_keepout()
{
   cur_obs.keepout = TRUE;
   return 1;
}

/******************************************************************************
* obs_nonet
*/
static int obs_nonet()
{
   cur_obs.net = OBS_NO_NET;
   return 1;
}

/******************************************************************************
* obs_outline
*/
static int obs_outline()
{
   cur_obs.kind = OBS_KIND_OUTLINE;
   return 1;
}

/******************************************************************************
* obs_compoutline
*/
static int obs_compoutline()
{
   cur_obs.kind = OBS_KIND_COMP_OUTLINE;
   return 1;
}

/******************************************************************************
* obs_compkeepout
*/
static int obs_compkeepout()
{
   cur_obs.kind = OBS_KIND_COMP_KEEPOUT;
   return 1;
}

/******************************************************************************
* obs_detail
*/
static int obs_detail()
{
   cur_obs.kind = OBS_KIND_DETAIL;
   return 1;
}

/******************************************************************************
* obs_sym
*/
static int obs_sym()
{
   if (!get_tok())
      return p_error(); 
   cur_obs.symid = atoi(token);
   return 1;
}

/******************************************************************************
* obs_comp
*/
static int obs_comp()
{
   if (!get_tok())
      return p_error();
   cur_obs.symid = -atoi(token);
   return 1;
}

/******************************************************************************
* obs_level
*/
static int obs_level()
{
   if (!get_tok())
      return p_error(); 
   cur_obs.level = get_layerindex(atoi(token));
   return 1;
}

/******************************************************************************
* obs_width

   there is a problem with the WIDTH statement (vs. the W)

(Obs 1574(N "4565")(Uid -35430)(Detail)(NoFill)(Grid 0)(Sym 432)
  (L 23)(W 1920)(CLEARANCE 4)(zOrder 1920)(NoNet) (39600 -42000)
  (Width 491520) (39600 6000)(Width 491520))
*/
static int obs_width()
{
   if (!get_tok())
      return p_error();
   if (cur_obs.width == 0)
      cur_obs.width = cnv_unit(token);
   return 1;
}

/******************************************************************************
* obs_w
*/
static int obs_w()
{

   if (!get_tok())
      return p_error(); 
   cur_obs.width = cnv_unit(token);
   return 1;
}

/******************************************************************************
* obs_basewidth
*/
static int obs_basewidth()
{
   if (!get_tok())
      return p_error();  
   cur_obs.width = cnv_unit(token);
   return 1;
}

/******************************************************************************
* mass_height
*/
static int mass_height()
{
   if (!get_tok())
      return p_error(); 
   G.cur_height = cnv_unit(token);
   return 1;
}

/******************************************************************************
* mass_mirrored
*/
static int mass_mirrored()
{
   if (!get_tok())
      return p_error();

   if (!STRCMPI(token, "On"))
      cur_text.mirror = TRUE;
   else if (!STRCMPI(token, "Off"))
      cur_text.mirror = FALSE;
   else if (!STRCMPI(token, ")"))
   {
      cur_text.mirror = TRUE;
      Push_tok = TRUE;
   }

   return 1;
}

/******************************************************************************
* mass_width
*/
static int mass_width()
{
   if (!get_tok())
      return p_error();
   G.cur_width = cnv_unit(token);
   return 1;
}

/******************************************************************************
* obs_generated
*/
static int obs_generated()
{
   cur_obs.flags |= OBS_FLAGS_GENERATED;
   return 1;
}

/******************************************************************************
* comp_nonelectric
*/
static int comp_nonelectric()
{
   getLastOrcadComponentInstance().setIsNonElectric(true);
   return 1;
}

/******************************************************************************
* obs_ccw
*/
static int obs_ccw()
{
   if (polycnt)
      poly_l[polycnt-1].f = OBS_CCW;
   else
      fprintf(fLog, "(CCW) ??? at %ld\n", ifp_line);

   return 1;
}

/******************************************************************************
* obs_cw
*/
static int obs_cw()
{
   if (polycnt)
      poly_l[polycnt-1].f = OBS_CW;
   else
      fprintf(fLog, "(CW) ??? at %ld\n", ifp_line);

   return 1;
}

/******************************************************************************
* obs_arc
*/
static int obs_arc()
{
   cur_obs.flags |= OBS_FLAGS_ARC;
   return 1;
}

/******************************************************************************
* obs_circle
*/
static int obs_circle()
{
   cur_obs.flags |= OBS_FLAGS_CIRCLE;
   return 1;
}

/******************************************************************************
* mass_version
*/
static int mass_version()
{
   if (!get_tok())
      return p_error();  // 5xxx
   min_ver.m_version = atoi(token);

   if (min_ver.m_version > 7000)
      width_in_mil = FALSE;

   return 1;
}           

/******************************************************************************
* mass_majorrev
*/
static int mass_majorrev()
{
   if (!get_tok())   
      return p_error();  // 5xxx
   min_ver.m_majorrev = atoi(token);
   return 1;
}

/******************************************************************************
* mass_minorrev
*/
static int mass_minorrev()
{
   if (!get_tok())
      return p_error();  // 5xxx
   min_ver.m_minorrev = atoi(token);
   return 1;
}

/******************************************************************************
* mass_l
*/
static int mass_l()
{
   if (!get_tok())
      return p_error();
   G.electrical_layers = atoi(token);
   return 1;
}

/******************************************************************************
* layer_etch
*/
static int layer_etch()
{
   getLastOrcadLayer().etch = TRUE;
   return 1;
}

/******************************************************************************
* layer_plane
*/
static int layer_plane()
{
   getLastOrcadLayer().plane = TRUE;
   return 1;
}

/******************************************************************************
* layer_unused
*/
static int layer_unused()
{
   getLastOrcadLayer().unused = TRUE;
   return 1;
}

/******************************************************************************
* layer_tt
*/
static int layer_tt()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* layer_pp
*/
static int layer_pp()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* layer_pt
*/
static int layer_pt()
{
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* layer_drill
*/
static int layer_drill()
{
   getLastOrcadLayer().drill = TRUE;
   return 1;
}

/******************************************************************************
* layer_mirror
*/
static int layer_mirror()
{
   if (!get_tok())
      return p_error();
   getLastOrcadLayer().mirror = atoi(token);

   return 1;
}

/******************************************************************************
* sym_p
*/
static int sym_p()
{
   if (!get_tok())
      return p_error();
   int index = atoi(token);

   if (!get_tok()) 
      return p_error(); // (
   if (!get_tok())
      return p_error(); // N
   if (!get_tok())
      return p_error(); // name
   char pinname[80];
   strcpy(pinname, token);

   if (!get_tok())
      return p_error(); // )
   if (!get_tok())
      return p_error(); // x
   double x = cnv_unit(token) - getLastOrcadShape()->getOrigX();


   if (!get_tok())
      return p_error(); // y
   double y = cnv_unit(token) - getLastOrcadShape()->getOrigY();

   if (!get_tok())
      return p_error(); // stack
   int thruNumber = atoi(token);

   int padstackIndex = get_padstackptr(thruNumber);
   if (padstackIndex < 0)
   {
      fprintf(fLog, "Padstack Error for PIN at line %ld\n", ifp_line);
   }
   else
   {
      MINPadstack *padstack = &(getOrcadPadstack(padstackIndex));

      // Remove changing mirrored pad stack referencing to non-mirrored padstack reference
      // because true pad stacks must be used for accurate post processing
#ifdef  UN_MIR_PADSTACK
      CString padstackName = padstack->name;
      if (padstackName.Right(2) == "-M")
      {
         // here find the original name
         padstackName = padstackName.Left(padstackName.GetLength()-2);
         int newPadstackIndex = get_padstacknameptr(padstackName);
         if (newPadstackIndex > -1)
            padstackIndex = newPadstackIndex;
         else
         {
            // could not find an un-mirrored name ????
            fprintf(fLog, "Can not find un-mirrored padstack for [%s]\n", padstack->name);
            display_error++;
         }
      }
#endif

      padstack->setUsed(true); 

      ORCADShape *shape = getLastOrcadShape();
      if (shape->getPinCnt() < MAX_PINPERCOMP)
      {
         int p = shape->getPinCnt(); // just shorter
         shape->getPin(p).setPinName(pinname);
         shape->getPin(p).setNumber(index);
         shape->getPin(p).setPadStackIndex(padstackIndex);
         shape->getPin(p).setX(x);
         shape->getPin(p).setY(y);
         shape->incrementPinCnt();
      }
      else
      {
         fprintf(fLog, "Too many pins per shape [%s] at %ld\n", shape->getName(), ifp_line);
         display_error++;
         return -1;
      }
   }

   // here do the rest if needed
   fskip();

   return 1;
}

/******************************************************************************
* sym_uid
  the symtax of P is (P 1 (N "a") x y stack) without proper bracketing
*/
static int sym_uid()
{
   if (!get_tok())
      return p_error();  // id num
   getLastOrcadShape()->setUid( atoi(token) );

   if (!get_tok())
      return p_error();  // off x

   // old format does not have 4 parameters
   if (!STRCMPI(token, ")"))
   {
      Push_tok = TRUE;
      return 0;
   }

   //getLastOrcadShape()->origx = cnv_unit(token);
   if (!get_tok())
      return p_error();  // off y
   //getLastOrcadShape()->origy = cnv_unit(token);

   if (!get_tok())
      return p_error();
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
* mass_uid
*/
static int mass_uid()
{
   if (!get_tok())
      return p_error();  // id num
   cur_uid = atol(token);
   return 1;
}

/******************************************************************************
* sym_derived
*/
static int sym_derived()
{
   if (!get_tok())
      return p_error();  // derived sym id.
   getLastOrcadShape()->setDerivedShapeIndex( atoi(token));
   return 1;
}

/******************************************************************************
* sym_via
*/
static int sym_via()
{
   getLastOrcadShape()->setVia( true );
   return 1;
}

/******************************************************************************
* sym_testpt
*/
static int sym_testpt()
{
   getLastOrcadShape()->setTestPt( true );
   return 1;
}

/******************************************************************************
* comp_sym
*/
static int comp_sym()
{
   if (!get_tok())
      return p_error();
   getLastOrcadComponentInstance().setSymId( atoi(token) );
   return 1;
}

/******************************************************************************
* comp_via
*/
static int comp_via()
{
   getLastOrcadComponentInstance().setIsVia(true);
   return 1;
}

/******************************************************************************
* comp_testpt
*/
static int comp_testpt()
{
   getLastOrcadComponentInstance().setIsTestPt(true);
	getLastOrcadComponentInstance().setNormalize(false);
   return 1;
}

/******************************************************************************
* comp_value
*/
static int comp_value()
{
   if (!get_tok())
      return p_error();
   getLastOrcadComponentInstance().setValue( token );
   return 1;
}

/******************************************************************************
* comp_package
*/
static int comp_package()
{
   if (!get_tok())
      return p_error();
   getLastOrcadComponentInstance().setPackageIndex( atoi(token) );
   return 1;
}

/******************************************************************************
* comp_mir
*/
static int comp_mir()
{
   getLastOrcadComponentInstance().setMirror(true);
   return 1;
}

/******************************************************************************
* comp_mirsym
*/
static int comp_mirsym()
{
   if (!get_tok()) 
      return p_error();
   getLastOrcadComponentInstance().setMirId( atoi(token) );
   return 1;
}

/******************************************************************************
* comp_r
*/
static int comp_r()
{
   if (!get_tok())
      return p_error();
   getLastOrcadComponentInstance().setRotation( cnv_degree(token) );
   return 1;
}

/******************************************************************************
* pad_r
*/
static int pad_r()
{
   if (!get_tok())
      return p_error();
   G.cur_rotation = cnv_degree(token);
   return 1;
}

/******************************************************************************
* pad_undef
*/
static int pad_undef()
{
   G.cur_layernum = -1;
   return 1;
}

/******************************************************************************
* text_r
*/
static int text_r()
{
   if (!get_tok())
      return p_error();
   cur_text.rotation = cnv_degree(token);
   return 1;
}

/******************************************************************************
* text_compname
*/
static int text_compname()
{
   cur_text.compname = TRUE;
   return 1;
}

/******************************************************************************
* text_symname
*/
static int text_symname()
{
   cur_text.symname = TRUE;
   return 1;
}

/******************************************************************************
* text_free
*/
static int text_free()
{
   cur_text.free = TRUE;
   return 1;
}

/******************************************************************************
* text_aspect
*/
static int text_aspect()
{
   if (!get_tok())
      return p_error();
   cur_text.aspect = atoi(token);
   return 1;
}

/******************************************************************************
* text_compvalue
*/
static int text_compvalue()
{
   cur_text.compvalue = TRUE;
   return 1;
}

/******************************************************************************
* text_sym
*/
static int text_sym()
{
   if (!get_tok()) 
      return p_error();
   cur_text.symindex = atoi(token);
   return 1;
}

/******************************************************************************
* text_comp
*/
static int text_comp()
{
   if (!get_tok())
      return p_error();
   cur_text.compindex = atoi(token);
   return 1;
}

/******************************************************************************
* mass_v
*/
static int mass_v()
{
   if (!get_tok())
      return p_error();  
   G.number_of_vias = atoi(token);
   return 1;
}

/******************************************************************************
* header_drlchartloc
   Drill chart is visible with DRLDWG layer
*/
static int header_drlchartloc()
{
   if (!get_tok())
      return p_error();
   long x = atol(token);

   if (!get_tok())
      return p_error();
   x = atol(token);

   return 1;
}

/******************************************************************************
* header_inchfactor
*/
static int header_inchfactor()
{
   if (!get_tok())
      return p_error();
   G.inchfactor = atof(token);
   return 1;
}

/******************************************************************************
* header_origin
*/
static int header_origin()
{
   // ! Can't use cnv_unit on token here, may not yet have read header_inchfactor.

   if (!get_tok())
      return p_error();
   G.rawOrcadOriginX = atof(token);

   if (!get_tok())
      return p_error();
   G.rawOrcadOriginY = atof(token);

   return 1;
}


/******************************************************************************
* header_grid
*/
static int header_grid()
{
   if (!get_tok())
      return p_error();
   G.gridunit = atoi(token);
   return 1;
}

/******************************************************************************
* header_userdiv
*/
static int header_userdiv()
{
   if (!get_tok())
      return p_error();
   G.userdiv = atoi(token);
   return 1;
}

//----------------------------------------------------------------------------

static void ReplaceWildcards(CString &textStr, ORCADCompInst &compInst)
{
   CString textStrUpper( textStr );
   textStrUpper.MakeUpper();

   // Value - Is value from Comp record, not necessarily the same as electrical value.
   int valIndx = textStrUpper.Find("&VALUE");
   if (valIndx > -1)
   {
      // This is to make is case-insensitive.
      CString actualWildcard = textStr.Mid(valIndx, strlen("&VALUE"));
      CString replacement( compInst.getValue() );
      textStr.Replace(actualWildcard, replacement);
   }
}

/******************************************************************************
* write_text
*/
static int write_text()
{
   int BlockOn = FALSE;
   double charWidth = G.cur_height * TEXTCORRECT * (0.01 * cur_text.aspect) * 0.75;
   CString layerName = getOrcadLayer(G.cur_layernum).getName();

   poly_l[0].x -= sin(DegToRad(cur_text.rotation)) * ( (G.cur_height - charWidth) / 2 );
   poly_l[0].y += cos(DegToRad(cur_text.rotation)) * ( (G.cur_height - charWidth) / 2 );

   if (cur_text.compname)
   {
      int compIndex = get_compptr(cur_text.compindex);

      if (compIndex > -1)  
      {
         ORCADCompInst *compInst = &(getOrcadComponentInstance(compIndex));
         if (compInst->getComponentInsert() == NULL)
         {
            polycnt = 0;
            return 1;
         }

         // need to unmirror layer
         if (compInst->getMirror() && (compInst->getComponentInsert()->getInsert()->getMirrorFlags() & MIRROR_LAYERS))
            layerName = getOrcadLayer(getOrcadLayer(G.cur_layernum).mirror).getName();
         int layerNum = Graph_Level(layerName, "", 0);

         CString attribValue(cur_text.prosa);
         int attribKw = getCamCadDoc()->IsKeyWord(ATT_REFNAME, TRUE);

         if (!STRCMPI(cur_text.prosa, "&Comp")) // placeholder for refdes name
         {
            attribValue = compInst->getName();

            if (NormalizeRefdes)
            {
               if (compInst->getMirror() && !(compInst->getComponentInsert()->getInsert()->getMirrorFlags() & MIRROR_LAYERS))
                  layerNum = Graph_Level("REFDES_2", "", 0);
               else
                  layerNum = Graph_Level("REFDES_1", "", 0);
            }
         }

         if (attribKw > -1)
         {
            Attrib *existingAttrib;
            if (compInst->getComponentInsert()->lookUpAttrib(attribKw, existingAttrib))
            {
               // exists. keep duplicates unless we are in NormalizeRefdes mode
               if (!NormalizeRefdes)
                  existingAttrib->duplicateInstance();
            }

            getCamCadDoc()->SetVisAttrib(&compInst->getComponentInsert()->getAttributesRef(), attribKw, VT_STRING,
               attribValue.GetBuffer(0), poly_l[0].x, poly_l[0].y, DegToRad(cur_text.rotation),
                  G.cur_height*TEXTCORRECT, charWidth, 1, 0, TRUE, SA_OVERWRITE, 0L, layerNum, 0, 0, 0);
         }
      }

      polycnt = 0;
      return 1;
   }

   if (cur_text.compindex > 0)
   {
      // Put text attached to component directly on whatever layer. Mostly this is
      // to get silkscreen supported, but may as well do all. I tried it as attributes, trouble there
      // is more than one instance (on mulitple layers) of same attribute name.
      // Can't put into component geometry, it can vary per instance insert of same geometry.

      int compIndex = get_compptr(cur_text.compindex);

      if (compIndex > -1)  
      {
         ORCADCompInst *compInst = &(getOrcadComponentInstance(compIndex));
         if (compInst->getComponentInsert() != NULL)
         {
            CTMatrix compMat = compInst->getComponentInsert()->getInsert()->getTMatrix();
            CPoint2d textPt( poly_l[0].x, poly_l[0].y );
            compMat.transform(textPt);

            int layerNum = Graph_Level(layerName, "", 0);

            // Not doing anything about rotation and mirror, not sure we need to. It seems to come out okay like this.
            double finalRotation = cur_text.rotation + compInst->getComponentInsert()->getInsert()->getAngleDegrees();

            CString textStr( cur_text.prosa );
            ReplaceWildcards(textStr, *compInst);

            DataStruct *data = Graph_Text(layerNum, textStr.GetBuffer(0), textPt.x, textPt.y, G.cur_height*TEXTCORRECT,
               charWidth, DegToRad(finalRotation), 0, 1, cur_text.mirror, 0, FALSE, 0, 0);
         }
      }

      polycnt = 0;
      return 1;
   }
   
   if (cur_text.symindex == 0)
   {
      // boardoutline if Keepin and layer 0
      if (G.cur_layernum == 0)
         layerName = "BOARD";
   }
   else if (cur_text.symindex > 0)
   {
      if (!cur_text.free)  // only show free text
      {
         // symid text is never visible
         polycnt = 0;
         return 1;
      }

      CString symbolName;
      symbolName.Format("SYM %d", cur_text.symindex);
      BlockStruct *curBlock = Graph_Block_On(GBO_APPEND, symbolName, GlobalPcbFile->getFileNumber(), 0);
      BlockOn = TRUE;
      if (cur_obs.kind == OBS_KIND_COMP_OUTLINE)
         layerName.Format("COMPOUTLINE_%d", G.cur_layernum);
      else
         layerName = getOrcadLayer(G.cur_layernum).getName();
   }
   else if (cur_obs.symid < 0 && cur_obs.net == OBS_NO_NET)
   {
      // comp outline  for individual comps.
      polycnt = 0;
      return 1;
   }
   else if (cur_obs.kind == OBS_KIND_DRILLCHART)
   {
      if (!drillchart)
      {
         polycnt = 0;
         return 1;
      }

      layerName = "DRLDWG";
   }
   else
   {
      fprintf(fLog, "Unknown OBS net [%d] sym [%d] at %ld\n", cur_obs.net, cur_obs.symid, ifp_line);
   }

   int layerNum = Graph_Level(layerName, "", 0);

   DataStruct *data = Graph_Text(layerNum, cur_text.prosa, poly_l[0].x, poly_l[0].y, G.cur_height*TEXTCORRECT,
         charWidth, DegToRad(cur_text.rotation), 0, 1, cur_text.mirror, 0, FALSE, 0, 0); // prop flag, mirror flag, oblique angle in deg

   if (BlockOn)
      Graph_Block_Off();

   if (data)
   {
      sprintf(cur_obs.name, "%ld", cur_uid);
      getCamCadDoc()->SetAttrib(&data->getAttributesRef(), getCamCadDoc()->IsKeyWord(ATT_NAME, TRUE), VT_STRING, cur_obs.name, SA_OVERWRITE, NULL);
   }

   polycnt = 0;
   return 1;
}

/******************************************************************************
* write_arc
*/
DataStruct *write_arc(char *l, int closed, int fill, double offx, double offy, int gr_class, BOOL mirror)
{
   int layer = Graph_Level(l, "", 0);
   int widthindex = 0;
   int err = 0;

   if (cur_obs.width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, cur_obs.width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *data = Graph_PolyStruct(layer,0L,0);
   data->setGraphicClass(gr_class);
   
   Graph_Poly(NULL, widthindex, fill,0,closed);
   // here write lines.
	int i=0;
   for (i=0; i<polycnt; i++)
   {
      double rad, sa, da;

      if (poly_l[i].f == OBS_CCW)
      {
         // 0 = start
         // 1 = center
         // 2 = end
         if (mirror)
            ArcCenter2(-poly_l[i-1].x, poly_l[i-1].y, -poly_l[i+1].x, poly_l[i+1].y, -poly_l[i].x, poly_l[i].y, &rad, &sa, &da, FALSE);
         else
            ArcCenter2(poly_l[i-1].x, poly_l[i-1].y, poly_l[i+1].x, poly_l[i+1].y, poly_l[i].x, poly_l[i].y, &rad, &sa, &da, FALSE);

         poly_l[i-1].bulge = tan(da/4);
         i++;
      }
      else if (poly_l[i].f == OBS_CW)
      {
         // 0 = start
         // 1 = center
         // 2 = end
         if (mirror)
            ArcCenter2(-poly_l[i-1].x, poly_l[i-1].y, -poly_l[i+1].x, poly_l[i+1].y, -poly_l[i].x, poly_l[i].y, &rad, &sa, &da, TRUE);
         else
            ArcCenter2(poly_l[i-1].x, poly_l[i-1].y, poly_l[i+1].x, poly_l[i+1].y, poly_l[i].x, poly_l[i].y, &rad, &sa, &da, TRUE);

         poly_l[i-1].bulge = tan(da/4);
         i++;
      }
      else if (poly_l[i].f == 0)
      {
         poly_l[i].bulge = 0.0;
      }
   }

   for (i=0; i<polycnt; i++)
   {
//    if (mirror)
//       poly_l[i].x = -poly_l[i].x;

      if (poly_l[i].f == 0)
         Graph_Vertex(poly_l[i].x - offx, poly_l[i].y - offy, poly_l[i].bulge);
   }

   polycnt = 0;
   return data;
}

/******************************************************************************
* tok_search
   Search for the token in a command token list.
*/
static int tok_search(List *tok_lst,int tok_size)
{
   for (int i=0; i<tok_size; ++i)
   {
      if (!STRCMPI(token, tok_lst[i].token))
         return(i);
   }

   return(-1);
}

/******************************************************************************
* get_tok
   Get a token from the input file.
*/
static int get_tok()
{

   if (!Push_tok)
   {
      while (!get_next(cur_line, cur_new))
      {
         if (!get_line(cur_line))
            return(FALSE);
         else
            cur_new = true;
      }
      cur_new = false;
   }
   else
   {
      Push_tok = false;
   }

   return(TRUE);
}

/******************************************************************************
* push_tok
*/
static int push_tok()
{
   return (Push_tok = TRUE);
}

/******************************************************************************
* get_next
*/
static int get_next(CString &lp, bool newlp)
{
   static char  *cp;
   static CString cpbuf;

   int i = 0;
   int token_name = FALSE;

   if (newlp)
   {
      cpbuf.ReleaseBuffer();
      cpbuf = lp;
      cp = cpbuf.GetBuffer(0);
   }

   for (; isspace(*cp) && *cp != '\0'; ++cp) ;

   switch(*cp)
   {
   case '\0':
   case '%':
      return 0;
   case '(':
   case ')':
      token[i++] = *(cp++);
      break;
   case '\"':
      {
         token[i] = '\0';
         token_name = TRUE;
         for (++cp; *cp != '\0'; ++cp, ++i)
         {
            if (*cp == '\n')
            {
               /* text spans over 1 line */
               if (!get_line(cur_line))
               {
                  fprintf(fLog, "Error in Line Read\n");
                  display_error++;
                  return (FALSE);
               }

               cpbuf.ReleaseBuffer();
               cpbuf = cur_line;
               cp = cpbuf.GetBuffer(0);

               /* the quote can be the first character in the new line */
               if (*cp == '"' && token[i] != '"')
                  break;
               token[i] = *cp;
            }
            else if (*(cp) == '"' && *(cp+1) != '"')
            {
               break; // text done;
            }
            else if (*(cp) == '"' && *(cp+1) == '"')
            {
               token[i] = *(cp);
               ++cp;
            }
            else if (*cp != '\0')
            {
               token[i] = *cp;
            }
         }
         ++cp;
      }
      break;
   default:
      {
         for (; !isspace(*cp) && *cp != '\"' && *cp != '(' && *cp != ')' && *cp != '\0'; ++cp, ++i)
         {
            if (*cp == '\\')
               ++cp;
            token[i] = *cp;
         }
      }
      break;
   }

   if (!i && !token_name)
      return(FALSE);

   token[i] = '\0';
   return(TRUE);
}

/******************************************************************************
* get_line
   Get a line from the input file.
*/
static int get_line(CString &buf)
{
   if (fgetcs(buf, ifp) == NULL)
   {
      if (!feof(ifp))
      {
         ErrorMessage("Read error", "Read ORCAD MIN file");
         return FALSE;
      }
      else
      {
         buf.Empty();
         return(FALSE);
      }
   }

   ++ifp_line;
   buf.Trim();

   return(TRUE);
}

/******************************************************************************
* p_error
*/
int p_error()
{
   fprintf(fLog, "Parsing Error : Token [%s] on line %ld\n", token, ifp_line);
   display_error++;
   return -1;
}

/******************************************************************************
* init_all_mem
*/
void init_all_mem()
{
   if (drillArray.GetCount() > 0 ||
      comppinarray.GetCount() > 0 ||
      padformarray.GetSize() > 0 ||
      s_shapeArray != NULL ||
      s_componentInstanceArray != NULL ||
      s_netArray != NULL ||
      s_packageArray != NULL ||
      s_padstackArray != NULL ||
      s_attributeArray != NULL ||
      s_layerArray != NULL)
   {
      int jj = 0;
   }

   drillArray.SetSize(0, 100);
   comppinarray.SetSize(0, 100);
   padformarray.SetSize(0, 100);

   if ((poly_l = (ORCADPoly_l *)calloc(MAX_POLY, sizeof(ORCADPoly_l))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

}


/******************************************************************************
* free_all_mem
*/
void free_all_mem()
{
   padformarray.empty();  // 60 mil round
   drillArray.empty();
   comppinarray.empty();

   free(poly_l);
   polycnt = 0;


   delete s_shapeArray;
   s_shapeArray = NULL;

   delete s_componentInstanceArray;
   s_componentInstanceArray = NULL;

   delete s_netArray;
   s_netArray = NULL;

   delete s_packageArray;
   s_packageArray = NULL;

   delete s_padstackArray;
   s_padstackArray = NULL;

   delete s_attributeArray;
   s_attributeArray = NULL;

   delete s_layerArray;
   s_layerArray = NULL;
}

/******************************************************************************
* clean_text
*/
static void clean_text(char *p)
{
   int t = 0;
   int l = strlen(p);
   char tmp[MAX_LINE];

   for (int i=0; i<l; i++)
   {
      if (p[i] == '\\')
      {
         i++;
         if (p[i] == 'b')
            p[i] = ' '; /* convert to blank */
      }
      tmp[t++] = p[i];
   }
   tmp[t] = '\0';
   strcpy(p, tmp);

   // elim blanks
   while (strlen(p) && isspace(p[strlen(p)-1]))
      p[strlen(p)-1] = '\0';
   STRREV(p);

   while (strlen(p) && isspace(p[strlen(p)-1]))
      p[strlen(p)-1] = '\0';
   STRREV(p);
}

/******************************************************************************
* cnv_unit
*/
double cnv_unit(char *s)
{
   return cnv_unit(atof(s));
}

/******************************************************************************
* cnv_unit
*/
double cnv_unit(double val)
{
   double convertedVal = G.inchfactor * val * scaleFactor;
   return convertedVal;
}

/******************************************************************************
* cnv_conn
*/
double cnv_conn(char *s)
{
   double y = 0.001 * atof(s) * scaleFactor;
   return y;
}

/******************************************************************************
* cnv_degree
*/
double cnv_degree(char *s)
{
   double y = atof(s) / 60;
   return y;
}

/******************************************************************************
* get_padstackptr
*/
static int get_padstackptr(int index)
{
   for (int i=0; i < getOrcadPadstacks().GetSize(); i++)
   {
      if (getOrcadPadstack(i).getIndex() == index)
         return i;
   }

   fprintf(fLog, "Could not find padstackindex [%d]\n", index);
   display_error++;
   return (-1);
}

/******************************************************************************
* get_padstacknameptr
*/
static int get_padstacknameptr(const char *name)
{
   for (int i=0; i < getOrcadPadstacks().GetSize(); i++)
   {
      if (getOrcadPadstack(i).getName().CompareNoCase(name) == 0)
         return i;
   }

   fprintf(fLog, "Could not find padstackname [%d]\n", name);
   display_error++;
   return (-1);
}

/******************************************************************************
* get_shapeptr
*/
static int get_shapeptr(int index)
{
   for (int i=0; i < getOrcadShapes().GetSize(); i++)
   {
      if (getOrcadShape(i)->getIndex() == index)
         return i;
   }

   fprintf(fLog, "Could not find shapeindex [%d] at %ld\n", index, ifp_line );
   display_error++;
   return -1;
}

/******************************************************************************
* get_shapeptr_from_name 
*/
static int get_shapeptr_from_name(const char *s)
{
   for (int i=0; i < getOrcadShapes().GetSize(); i++)
   {
      if (!strcmp(getOrcadShape(i)->getName(), s))
         return i;
   }

   return -1;
}

/******************************************************************************
* get_compptr
*/
static int get_compptr(int index)
{
   for (int i=0; i < getOrcadComponentInstances().GetSize(); i++)
   {
      if (getOrcadComponentInstance(i).getIndex() == index)
         return i;
   }

   return -1;
}

/******************************************************************************
* write_obs
*/
static int write_obs(int index)
{
   CString level = "NO_LEVEL";
   int closed = FALSE;
   int poured = FALSE;
   int fill = FALSE;
   int blockon = 0;
   int gr_class = 0;
   double offx =0;
   double offy = 0;
   double compheight = -1;
   BOOL mirror = FALSE;

   level = getOrcadLayer(cur_obs.level).getName();

   if (getOrcadLayer(cur_obs.level).unused)
   {
      polycnt = 0;
      return 1;   
   }

   if (polycnt)
   {
      if (poly_l[0].x == poly_l[polycnt-1].x &&
          poly_l[0].y == poly_l[polycnt-1].y)
         closed = TRUE;
   }

   if (cur_obs.kind == OBS_KIND_OUTLINE) // only keepout and poured is filled ! 
   {
      cur_obs.hatchkind = 0;
   }

   if (cur_obs.symid == 0 && cur_obs.net > 0)
   {
      //if (cur_obs.hatchkind) // must keep with if it's hachted. The width is the true width around the hatch.
      // cur_obs.width = 0;
   }
   else if (cur_obs.symid == 0 && cur_obs.net == OBS_NO_NET)
   {
      // boardoutline if Keepin and layer 0
      if (cur_obs.level == 0)
      {
         level = "BOARD";
         gr_class = GR_CLASS_BOARDOUTLINE;
      }

      if (cur_obs.kind == OBS_KIND_COMP_KEEPOUT)
      {
         compheight = cur_obs.width * Units_Factor(heightunit, page_unit);

         cur_obs.width = 0;
         if (cur_obs.level == 0)
            level = "BOARD";
         gr_class = GR_CLASS_PLACEKEEPOUT;
      }
   }
   else if (cur_obs.symid != 0 && (cur_obs.net == OBS_NO_NET || cur_obs.net == 0))
   {
      int shapeIndex = -1;
      int outlineLevel = -1;

      if (cur_obs.symid > 0)
      {
         // SYM obs
         shapeIndex = get_shapeptr(cur_obs.symid);
      }
      else if (cur_obs.symid < 0)
      {
         // COMP obs
         int cptr =  get_compptr(abs(cur_obs.symid));
         if (cptr < 0)
         {
            polycnt = 0;
            return 1;
         }  

         if (cur_obs.kind == OBS_KIND_COMP_OUTLINE)
         {
            double compheight = cur_obs.width * Units_Factor(heightunit, page_unit);
            ORCADCompInst* compInstance = &(getOrcadComponentInstance(cptr));

            // here get component name and make a height.
            if (compInstance->getHeight() < compheight)  
               compInstance->setHeight(compheight);

            // save component outline layer to determine if the component is mirrored
            outlineLevel = cur_obs.level;
   
            // Update the side of this component
            compInstance->setPcbSide(mapOrcadLevelToPcbSide(outlineLevel));

            // if this component is defined on both layers, we would check if its mirror flag is set
            // if yes, this component is put to the bottom side
            if (compInstance->getPcbSide() == pcbSideBoth)
            {
               if (compInstance->getMirror() == true)
               {
                  compInstance->setLayerNum(2);
                  compInstance->setOutlineLevel(2);
               }
               else
               {
                  compInstance->setLayerNum(1);
                  compInstance->setOutlineLevel(1);
               }
            }
            else 
            {
               compInstance->setLayerNum(outlineLevel);
               compInstance->setOutlineLevel(outlineLevel);
            }
         }

//       mirror = getOrcadComponentInstance(cptr).mirror;
         shapeIndex = get_shapeptr(getOrcadComponentInstance(cptr).getSymId());
      }

      ORCADShape *shape = getOrcadShape(shapeIndex);
      //shape->setOutlineLevel(outlineLevel);   // Move it to next if (cur_obs.kind == OBS_KIND_COMP_OUTLINE) because the non-outline statement will replace the correct value

      offx = shape->getOrigX();
      offy = shape->getOrigY();

      CString symname;
      symname.Format("SYM %d", shape->getIndex());
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND, symname, GlobalPcbFile->getFileNumber(), 0);
      blockon = TRUE;

      if (cur_obs.kind == OBS_KIND_COMP_OUTLINE)
      {  
         if ((shape->getOutlineLevel() == 1 && cur_obs.level == 2) ||   // If we found there is "isBoth" definition for a shape,
             (shape->getOutlineLevel() == 2 && cur_obs.level == 1))     // we would set this shape to "TOP"
         {
            shape->setOutlineLevel(1);
            shape->setLayerNum(1);  // save sym layer to determine if the component is mirrored or not
         }
         else     // If there is only one side definition, just set the shape to that side
         {
            shape->setOutlineLevel(cur_obs.level); 
            shape->setLayerNum( cur_obs.level );  // save sym layer to determine if the component is mirrored or not
         }
         
         // Set outlineLevel of mastershape.
         // DR 790211 - Issue was we were mirroring the geometry when we should not have. It started here.
         // In that data the outline is associated with the component instance, not the shape aka symbol.
         // We got a shape using a derived symbol. The master for that symbol had no instances, so it had no
         // explicit outline in the data. So this code detects that, and sets the master's outline based on
         // the instance's outline. (Setting the level is the part that matters.)
         // But that line of code (commented out now) sets the master to opposite of what the instance is.
         // In turn, later on in post-processing where we are trying to reduce the geometries and use the master
         // instead of instance specific, we find the outlines have opposite layer so we mirror it.
         // That is wrong, it puts the pins in the wrong place, etc.
         // Just setting the master to the "cur" makes that data work. We don't know why it was done
         // the commented out way. We fully expect to get a report in of "hey this used to work now
         // it is broken". And with that we'll get the data that works with that old commented out line.
         // Then maybe we can come up with something that works for both. I think the best thing would be
         // to scrap all this local "consolidation" stuff and use the real Geometry Consolidator.
         // We would keep the stuff here that turns bottom-built to top-built geometries, but otherwise
         // leave it all as instance-specific result and let Consolidator pare it down. To help with that
         // we'd want to make sure OriginalName is set in all the geometries.
         ORCADShape *mastershape = getOrcadShapes().getMaster(*shape);
         if(mastershape && mastershape->getOutlineLevel() == -1 && mastershape != shape)
         {
            // cause of the problem:  int outlineLevel = (shape->getOutlineLevel() == 1)?2:(shape->getOutlineLevel() == 2)?1:cur_obs.level;
            // It is not clear if we'd be better off using shape->getOutLineLevel or cur_obs.level.
            // This one works for this DR's data.
            // int outlineLevel = cur_obs.level; // Hack fix, this worked with DR's data.
            int outlineLevel = shape->getOutlineLevel(); // This works too. Seems the best choice to make it like "this" instance.
            mastershape->setOutlineLevel(outlineLevel);
         }
      
         compheight = cur_obs.width * Units_Factor(heightunit, page_unit);
         cur_obs.width = 0;
         level.Format("COMPOUTLINE_%d", cur_obs.level);
         gr_class = GR_CLASS_COMPOUTLINE;

         if (compheight > 0)
         {
            if (shapeIndex > -1)
               getCamCadDoc()->SetAttrib(&curblock->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &compheight, SA_OVERWRITE, NULL); 
         }
      }
      else if (cur_obs.kind == OBS_KIND_COMP_KEEPOUT)
      {
         compheight = cur_obs.width * Units_Factor(heightunit, page_unit);
         cur_obs.width = 0;
         level = "COMPKEEPOUT_1";
         gr_class = GR_CLASS_PLACEKEEPOUT;
      }
      else
      {
         level = getOrcadLayer(cur_obs.level).getName();
      }

      // there is no poured in Symbol definitions
      if (cur_obs.poured)
      {
         cur_obs.hatchkind = OBS_HATCHKIND_SOLID;
         cur_obs.poured = 0;
      }
   }

   else if (cur_obs.symid != 0 && cur_obs.net < 0)
   {
      fprintf(fLog, "Obs is pin at %ld\n", ifp_line);
   }
   else if (cur_obs.kind == OBS_KIND_DRILLCHART)
   {
      if (!drillchart)
      {
         polycnt = 0;
         return 1;
      }
      level = "DRLDWG";
   }
   else
   {
      fprintf(fLog, "Unknown OBS UID [%d] net [%d] sym [%d] at %ld\n", cur_uid, 
            cur_obs.net, cur_obs.symid, ifp_line);

      if (blockon)
         Graph_Block_Off();
      polycnt = 0;
      return 1;
   }

   G.cur_width = cur_obs.width;

   fill = (cur_obs.hatchkind == OBS_HATCHKIND_SOLID);
   poured = cur_obs.poured;

   DataStruct *data = NULL;
   if (cur_obs.flags == OBS_FLAGS_ARC)
      data = write_arc(level.GetBuffer(0), closed, fill, offx, offy, gr_class, mirror);
   else if (cur_obs.flags == OBS_FLAGS_CIRCLE)
      data = write_circle(level.GetBuffer(0), offx, offy, gr_class, mirror);
   else
      data = write_poly(level.GetBuffer(0), closed, poured, fill, offx, offy, gr_class, mirror);

   if (blockon)
      Graph_Block_Off();

   if (data && gr_class == GR_CLASS_PLACEKEEPOUT)  // attach to graphic, not to symbol or comp.
   {
      if (compheight > 0)
      {
         getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &compheight, SA_OVERWRITE, NULL); 
      }
   }

   if (data)
   {
      sprintf(cur_obs.name, "%ld", cur_uid);
      getCamCadDoc()->SetAttrib(&data->getAttributesRef(),getCamCadDoc()->IsKeyWord(ATT_NAME, TRUE), VT_STRING, cur_obs.name, SA_OVERWRITE, NULL);
   }

   return 1;
}

/******************************************************************************
* write_poly
*/
DataStruct *write_poly(char *l, int closed, int poured, int fill, double offx, double offy,int gr_class, BOOL mirror)
{
   if (polycnt == 0)
      return NULL;

   int layer = Graph_Level(l, "", 0);
   int widthindex = 0;
   int err = 0;

   if (cur_obs.width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, cur_obs.width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *data = Graph_PolyStruct(layer, 0L, 0);
   data->setGraphicClass(gr_class);

   if (!closed)      fill = FALSE;  // never fill if not closed.
   if (polycnt < 3)  fill = FALSE;
   if (poured)       fill = FALSE;  // pour makes a dynamioc copper pour, generated voids on the fly.

   // fill on ORCAD makes only a boundary
   CPoly *poly = Graph_Poly(NULL, widthindex, fill, 0, closed); // do not make a fill here, because ORCAD 
   if (poured)
      poly->setFloodBoundary(true);

   // here write lines.
   for (int i=0; i<polycnt; i++)
   {
//    if (mirror)
//       poly_l[i].x = -poly_l[i].x;

      Graph_Vertex(poly_l[i].x-offx, poly_l[i].y-offy,0.0);
   }

   polycnt = 0;
   return data;

}

/******************************************************************************
* write_circle
*/
DataStruct *write_circle(char *l, double offx, double offy, int gr_class, BOOL mirror)
{
   if (polycnt != 3)
   {
      fprintf(fLog, "Circle ??? at %ld\n", ifp_line);
      polycnt = 0;
      return NULL;
   }

   int layer = Graph_Level(l, "", 0);
   int widthindex = 0;
   int err = 0;

   if (cur_obs.width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, cur_obs.width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   double rad = sqrt(((poly_l[1].y - poly_l[0].y) * (poly_l[1].y - poly_l[0].y)) +
                     ((poly_l[1].x - poly_l[0].x) * (poly_l[1].x - poly_l[0].x)));

   DataStruct *data = Graph_PolyStruct(layer,0L,0);
   data->setGraphicClass(gr_class);
   
   int fill = 0;
   Graph_Poly(NULL,widthindex, fill, 0, 1);

   double x = poly_l[1].x;
   double y = poly_l[1].y;

   // here write lines.
   Graph_Vertex(x - offx - rad, y - offy, 1);
   Graph_Vertex(x - offx + rad, y - offy, 1);
   Graph_Vertex(x - offx - rad, y - offy, 0);
   polycnt = 0;

   return data;
}

/******************************************************************************
* get_netptr
*/
static int get_netptr(int index)
{
   for (int i=0;i < getOrcadNets().GetSize(); i++)
   {
      if (getOrcadNets().GetAt(i)->getIndex() == index)
         return i;
   }
   
   return -1;
}

/*Ende **********************************************************************/
bool geometryPinLocationsEqual(BlockStruct& block0,BlockStruct& block1)
{
   bool retval = true;
   int pinMatchCount = 0;
   CTypedMapStringToPtrContainer<DataStruct*> pinMap(nextPrime2n(50),false);
   DataStruct *data0,*data1;

   for (POSITION pos0 = block0.getHeadDataInsertPosition();pos0 != NULL;)
   {
      data0 = block0.getNextDataInsert(pos0);

      if (data0->getInsert()->getInsertType() == insertTypePin)
      {
         pinMap.SetAt(data0->getInsert()->getRefname(),data0);
      }
   }

   for (POSITION pos1 = block1.getHeadDataInsertPosition();pos1 != NULL;)
   {
      data1 = block1.getNextDataInsert(pos1);

      if (data1->getInsert()->getInsertType() == insertTypePin)
      {
         if (pinMap.Lookup(data1->getInsert()->getRefname(),data0))
         {
            if (!data0->getInsert()->getOrigin2d().fpeq(data1->getInsert()->getOrigin2d()))
            {
               retval = false;
               break;
            }
            else
            {
               pinMatchCount++;
            }
         }
      }
   }

   retval = (retval && (pinMatchCount > 0) && (pinMatchCount == pinMap.GetCount()));

   return retval;
}

//_____________________________________________________________________________
COrcadLayer::COrcadLayer()
{
   mirror = 0;
   index  = 0;
   unused = 0;
   etch   = 0;
   plane  = 0;
   drill  = 0;
}

//_____________________________________________________________________________
MINPackage* MINPackageArray::getPackageWithIndex(int packageIndex)
{
   MINPackage* package = NULL;

   for (int index = 0;index < GetSize();index++)
   {
      package = GetAt(index);

      if (package != NULL && package->getIndex() == packageIndex)
      {
         break;
      }

      package = NULL;
   }

   return package;
}

//_____________________________________________________________________________
int ORCADShape::m_nextId = 0;

ORCADShape::ORCADShape(int shapeIndex) : m_datalist(false), m_derivedGeometries(false)
{
   m_id                = m_nextId++;

   m_index             = shapeIndex;

   m_derivedShapeIndex = 0;
   m_outlineLevel      = -1;
   m_block             = NULL;

   m_layernum          = -1;
   m_origX             = 0.0;
   m_origY             = 0.0;
   m_pincnt            = 0;
   m_testpt            = false;
   m_uid               = 0;
   m_usage_cnt         = 0;
   m_via               = false;
   m_isDuplicate       = false;
   m_isBottomVersion   = false;
   m_isBottomDefined   = false;
}

ORCADShape::~ORCADShape()
{
}

void ORCADShape::setNumPins(int pinCount)
{
   int oldSize = m_pins.GetSize();

   if (pinCount < oldSize)
   {
      for (int index = m_pins.GetSize() - 1;index >= pinCount;index--)
      {
         m_pins.setAt(index,NULL);
      }
   }
   else if (pinCount > oldSize)
   {
      m_pins.SetSize(pinCount);

      for (int index = oldSize;index < pinCount;index++)
      {
         MINPin* pin = new MINPin;
         m_pins.setAt(index,pin);
      }
   }
}

MINPin& ORCADShape::getPin(int pinIndex) 
{ 
   if (pinIndex >= m_pins.GetSize())
   {
      setNumPins(pinIndex + 1);
   }

   return *(m_pins.GetAt(pinIndex)); 
}

void ORCADShape::setBlock(BlockStruct* block)
{
   //if (m_id == 2)
   //{
   //   int iii = 3;
   //}

   if (block != NULL)
   {
      BlockStruct* checkBlock = getCamCadDoc()->getBlockAt(block->getBlockNumber());

      //if (checkBlock != block)
      //{
      //   int iii = 3;
      //}

      //if (block->getBlockNumber() > 10000)
      //{
      //   int iii = 3;
      //}
   }

   m_block = block;
}

bool ORCADShape::pinLocationsMatch(ORCADShape& other, bool mirrorFlag)
{
   bool retval = (this->getPinCnt() > 0);



   for (int i=0; i<this->getPinCnt(); i++)
   {
      MINPin *pin = &(getPin(i));

      bool pinNameMatch = false;
		MINPin *otherPin;
      for (int j=0; j<other.getPinCnt(); j++)
      {
         otherPin = &(other.getPin(j));

         if (otherPin->getPinName().CompareNoCase(pin->getPinName()) == 0)
         {
            pinNameMatch = true;
            break;
         }
      }

      if (!pinNameMatch)
      {
         retval = false;
         break;
      }

      if ((mirrorFlag && !fpeq(otherPin->getX(), -pin->getX())) ||
          (!mirrorFlag && !fpeq(otherPin->getX(),  pin->getX())))
      {
         retval = false;
         break;
      }
   }

   return retval;
}

bool ORCADShape::allXPinLocationsZero()
{
   bool retval = true;

   MINPin* pin;

   for (int i=0; i<this->getPinCnt() && retval; i++)
   {
      pin = &(getPin(i));

      retval = fpeq(pin->getX(), 0.);
   }

   return retval;
}

BlockStruct& ORCADShape::appendOrMatch(BlockStruct& block)
{
   for (POSITION pos = m_derivedGeometries.GetHeadPosition(); pos != NULL; /**/)
   {
      BlockStruct* derivedBlock = m_derivedGeometries.GetNext(pos);

      if (geometryPinLocationsEqual(*derivedBlock, block))
      {
         return *derivedBlock;
      }
   }

   m_derivedGeometries.AddTail(&block);

   return block;
}

//_____________________________________________________________________________
ORCADShape* ORCADShapeArray::getMaster(ORCADShape& shape)
{
   ORCADShape* masterShape = &shape;

   while (masterShape->getDerivedShapeIndex() > 0)
   {
      ORCADShape* shape = getShapeWithIndex(masterShape->getDerivedShapeIndex()); 

      if (shape == NULL)
      {
         break;
      }

      masterShape = shape;
   }

   return masterShape;
}

ORCADShape* ORCADShapeArray::getShapeWithIndex(int shapeIndex)
{
   ORCADShape* shape = NULL;

   for (int index = 0;index < GetSize();index++)
   {
      shape = GetAt(index);

      if (shape != NULL && shape->getIndex() == shapeIndex)
      {
         break;
      }

      shape = NULL;
   }

   return shape;
}

//_____________________________________________________________________________
ORCADCompInst::ORCADCompInst()
{
   // name
   // partname
   // value

   m_symid = -1;
   m_mirid = -1;
   m_index = -1;
   m_packageindex = -1;

   m_rotation = 0.0;
   m_mirror = false;
   m_nonelectric = false;

   m_via = false;
   m_testpt = false;
   m_height = 0.0;

   m_derivedSymId = -1;
   m_layernum = -1;
   m_normalize = true;

   m_outlineLevel      = -1;
   m_componentInsertData   = NULL;

   m_padloc = pcbSideUnknown;
}

void ORCADCompInst::setBlock(BlockStruct& block)
{
   if (m_componentInsertData != NULL)
   {
      int blockNumber = block.getBlockNumber();
      m_componentInsertData->getInsert()->setBlockNumber(block.getBlockNumber());
   }
}

void ORCADCompInst::setComponentInsert(DataStruct* componentInsert)
{
   m_componentInsertData = componentInsert;

   if (m_componentInsertData != NULL)
   {
      //if (m_componentInsert->getEntityNumber() == 1416)
      //{
      //   int iii = 3;
      //}
   }
}

void ORCADCompInst::setPcbSide(PcbSide padside)
{
   switch(m_padloc)
   {
      case pcbSideUnknown: // There is no layer define, just take the value
            m_padloc = padside;
         break;
      case pcbSideTop:  // If there is a top define already, check if we have to set both define
         if ( padside ==  pcbSideBottom)
            m_padloc = pcbSideBoth;
         break;
      case pcbSideBottom:  // If there is a bottom define already, check if we have to set both define
         if ( padside == pcbSideTop )
            m_padloc = pcbSideBoth;
         break;
      case pcbSideBoth:
      default:
         break;   
   }
}
//_____________________________________________________________________________
#ifdef DEBUG_DR_790211
void ORCADShape::setOutlineLevel(int outlineLevel) 
{
   if (this->m_name.Find("CUSTOM") > 0)
   {
      int jj = 0;
   }
   if (this->m_name.Find("FLYBACK") > 0)
   {
      int jj = 0;
   }
   m_outlineLevel = outlineLevel; 
}

void ORCADCompInst::setOutlineLevel(int outlineLevel) 
{ 
   if (this->m_name == "T1")
   {
      int jj = 0;
   }
   m_outlineLevel = outlineLevel; 
}
#endif

void ORCADCompInstArray::normalizeGeometriesAndInserts(ORCADShapeArray& orcadShapes)
{
   CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> geometryMap;
   geometryMap.InitHashTable(nextPrime2n(GetSize()));
    int orcadLayerIndex1 = getOrcadLayer(1).index;  // 1 is TOP and 2 is BOTTOM
   int orcadLayerIndex2 = getOrcadLayer(2).index;

   for (int index=0; index<GetSize(); index++)
   {
      ORCADCompInst* componentInstance = GetAt(index);

		if (componentInstance != NULL && componentInstance->getNormalize())
      {
         DataStruct* componentInsert = componentInstance->getComponentInsert();
         BlockStruct* componentGeometry = getCamCadDoc()->getBlockAt(componentInsert->getInsert()->getBlockNumber());

         ORCADShape* shape       = orcadShapes.getShapeWithIndex(componentInstance->getShapeIndex());
         ORCADShape* masterShape = orcadShapes.getMaster(*shape);

         if (shape != masterShape)
         {
            int outlineLevel       = shape->getOutlineLevel();
            int masterOutlineLevel = masterShape->getOutlineLevel();
            bool placeMirroredFlag = false;
            bool useMasterFlag     = false;

            if ((outlineLevel == orcadLayerIndex1 && masterOutlineLevel == orcadLayerIndex2) ||
                (outlineLevel == orcadLayerIndex2 && masterOutlineLevel == orcadLayerIndex1) ||
                shape->pinLocationsMatch(*masterShape, true))
            {
               if (shape->allXPinLocationsZero())
               {
                  BlockStruct* masterGeometry = masterShape->getBlock();
                  if(masterGeometry)
                  {
                     componentGeometry->GenerateDesignSurfaceAttribute(getCamCadDoc()->getCamCadData(), true);
                     masterGeometry->GenerateDesignSurfaceAttribute(getCamCadDoc()->getCamCadData(), true);
                     bool designedOnSame = (componentGeometry->IsDesignedOnTop(getCamCadDoc()->getCamCadData()) == masterGeometry->IsDesignedOnTop(getCamCadDoc()->getCamCadData()));

                     if (designedOnSame)
                     {
                        useMasterFlag = true;
                     }
                  }
               }
               else
               {
                  placeMirroredFlag = true;
               }
            }
            else if (shape->pinLocationsMatch(*masterShape, false))
            {
               // pin locations match, use geometry for master sym
               useMasterFlag = true;
            }


            if (placeMirroredFlag)
            {
				   BlockStruct* foundComponentGeometry;
               if (!geometryMap.Lookup(componentGeometry, foundComponentGeometry))
               {
                  // make unmirrored geometry 
                  changeGeometryToTopDefined(componentGeometry);

                  geometryMap.SetAt(componentGeometry, componentGeometry);
               }

               // place mirrored
               componentInsert->getInsert()->setMirrorFlags(MIRROR_ALL);
               componentInsert->getInsert()->setPlacedBottom(true);
            }
            else if (useMasterFlag)
            {
               if (masterShape->getBlock() != NULL)
                  componentInsert->getInsert()->setBlockNumber(masterShape->getBlock()->getBlockNumber());
		         componentInsert->getInsert()->setMirrorFlags(0);
					componentInsert->getInsert()->setPlacedBottom(false);
            }
         }
      }
   }
}

void ORCADCompInstArray::reduceGeometries(ORCADShapeArray& orcadShapes)
{
   for (int index = 0;index < GetSize();index++)
   {
      ORCADCompInst* componentInstance = GetAt(index);

      //if (index == 140)
      //{
      //   int iii = 3;
      //}

      if (componentInstance != NULL)
      {
         ORCADShape* shape = orcadShapes.getShapeWithIndex(componentInstance->getShapeIndex());

         if (shape != NULL)
         {
            ORCADShape* masterShape = orcadShapes.getMaster(*shape);

            if (masterShape != NULL && masterShape != shape)
            {
               BlockStruct* geometry       = shape->getBlock();
               BlockStruct* masterGeometry = masterShape->getBlock();

               if (geometry != NULL && masterGeometry != NULL)
               {
                  if (geometryPinLocationsEqual(*geometry,*masterGeometry)) 
                  {
                     shape->setBlock(masterGeometry);
                  }
                  else
                  {
                     BlockStruct& shapeGeometry = masterShape->appendOrMatch(*geometry);
                     shape->setBlock(&shapeGeometry);
                  }
               }
            }

            if (shape->getBlock() != NULL)
            {
               componentInstance->setBlock(*(shape->getBlock()));
               //int iii = 3;
            }
         }
      }
   }
}

void ORCADCompInstArray::createDevices(CCamCadDatabase& camCadDatabase,FileStruct* file,
   ORCADShapeArray& orcadShapes,MINPackageArray& orcadPackages)
{
   int typeTestLinkKeywordIndex = camCadDatabase.getKeywordIndex(ATT_TYPELISTLINK);

   for (int index = 0;index < GetSize();index++)
   {
      ORCADCompInst* componentInstance = GetAt(index);

      if (componentInstance != NULL)
      {
         ORCADShape* shape   = orcadShapes.getShapeWithIndex(componentInstance->getShapeIndex());
         MINPackage* package = orcadPackages.getPackageWithIndex(componentInstance->getPackageIndex());

         if (shape != NULL && package != NULL)
         {
            int componentGeometryBlockNumber = componentInstance->getComponentInsert()->getInsert()->getBlockNumber();
            BlockStruct* componentGeometry = camCadDatabase.getBlock(componentGeometryBlockNumber);
            TypeStruct* typeStruct = camCadDatabase.getType(package->getName(),file);

            if (typeStruct != NULL && 
                typeStruct->getBlockNumber() != componentGeometryBlockNumber)
            {
               typeStruct = NULL;
            }

            if (typeStruct == NULL)
            {
               CString deviceTypeName;
               deviceTypeName.Format("%s_%s", package->getName(), componentGeometry->getName());

               typeStruct = camCadDatabase.getDefinedType(deviceTypeName,file);
               typeStruct->setBlockNumber(componentGeometryBlockNumber);
            }

            camCadDatabase.addAttribute(&(componentInstance->getComponentInsert()->getAttributesRef()),
               typeTestLinkKeywordIndex,package->getName());
         }
      } 
   }
}

void ORCADCompInstArray::makeGeometriesTopDefined()
{
   CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> topGeometryMap,bottomGeometryMap,viaGeometryMap;
   //topGeometryMap.InitHashTable(nextPrime2n(GetSize()));
   //bottomGeometryMap.InitHashTable(nextPrime2n(GetSize()));
   topGeometryMap.InitHashTable(GetSize() * 2);
   bottomGeometryMap.InitHashTable(GetSize() * 2);

   BuildViaGeometry(viaGeometryMap);
   for (int index = 0;index < GetSize();index++)
   {
      ORCADCompInst* componentInstance = GetAt(index);

      if (componentInstance != NULL && componentInstance->getNormalize())
      {
         DataStruct* componentInsert = componentInstance->getComponentInsert();
         BlockStruct* componentGeometry = getCamCadDoc()->getBlockAt(componentInsert->getInsert()->getBlockNumber());

         if (componentGeometry != NULL)
         {
            BlockStruct* foundComponentGeometry = NULL;
            bool designedOnTopFlag = false;

            if (bottomGeometryMap.Lookup(componentGeometry, foundComponentGeometry))
            {
               designedOnTopFlag = false;
            }
            else if (topGeometryMap.Lookup(componentGeometry, foundComponentGeometry))
            {
               designedOnTopFlag = true;
            }
            else
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               componentGeometry->GenerateDesignSurfaceAttribute(getCamCadDoc()->getCamCadData(),true);
               designedOnTopFlag = componentGeometry->IsDesignedOnTop(getCamCadDoc()->getCamCadData());
#else
               componentGeometry->GenerateDesignSurfaceAttribute(*getCamCadDoc(),true);
               designedOnTopFlag = componentGeometry->IsDesignedOnTop(*getCamCadDoc());
#endif

               if (designedOnTopFlag)
               {
                  topGeometryMap.SetAt(componentGeometry,componentGeometry);
               }
               else
               {
                  bottomGeometryMap.SetAt(componentGeometry,componentGeometry);

                  changeGeometryToTopDefined(componentGeometry);

                  // re-do design surface attribs, attribs leftover from above indicate
                  // a mirrored/bottom geometry, the geometry was just changed to top.
#if CamCadMajorMinorVersion > 406  //  > 4.6
                  componentGeometry->GenerateDesignSurfaceAttribute(getCamCadDoc()->getCamCadData(),true);
#else
                  componentGeometry->GenerateDesignSurfaceAttribute(*getCamCadDoc(),true);
#endif

               }
            }




            // Case 1550
            // Dean says what really determines the side of a component is the outline
            // layer assigned. The symbols definitions and subclassing do not behave
            // as one might expect. In particular, if a subclassed symbol is mirrored
            // and the base symbol is not, a mirrored insert of the subclass is not
            // doubled mirrored (as it would be in camcad rules). The insert mirror
            // applies to the base symbol layer definition.
            // By the time the code reaches  here, all of the geometry is supposed to
            // have been changed to top-defined. So we can just look at the outline
            // layer to determine if the symbol ought to be mirrored or not.
            // If the layer is indeterminate, then use the mirror flags of the component.

            int compinstLayerNum = componentInstance->getLayerNum();  // compoutline layer
            LayerStruct *layer = getCamCadDoc()->FindLayer(compinstLayerNum);
            int layertype = LAYTYPE_UNKNOWN;
            if (layer != NULL) 
               layertype = layer->getLayerType();

            if (layertype == LAYTYPE_SIGNAL_BOT)
            {      
               //case dts0100442218, geometry of Testpoints has to be changed to Top defined
               //so a mirrored Testpoints can be placed properly
               changeGeometryTopDefined(ValidateTestpoint(componentInstance), bottomGeometryMap, componentInsert, viaGeometryMap);

               componentInsert->getInsert()->setMirrorFlags(MIRROR_FLIP | MIRROR_LAYERS);
               componentInsert->getInsert()->setPlacedBottom(true);
            }
            else if (layertype == LAYTYPE_SIGNAL_TOP)
            {
               componentInsert->getInsert()->setMirrorFlags(0);
               componentInsert->getInsert()->setPlacedBottom(false);
            }
            else if (!designedOnTopFlag)
            {
               componentInsert->getInsert()->setMirrorFlags((~componentInsert->getInsert()->getMirrorFlags()) & 0x3);
               componentInsert->getInsert()->setPlacedBottom(! componentInsert->getInsert()->getPlacedBottom());
            }
         }
      }
   }
}

void ORCADCompInstArray::BuildViaGeometry(CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> &viaGeometryMap)
{
   for (int index = 0;index < GetSize();index++)
   {
      ORCADCompInst* componentInstance = GetAt(index);
      if(componentInstance && componentInstance->isVia())
      {
         DataStruct* componentInsert = componentInstance->getComponentInsert();
         if(componentInsert)
         {
            BlockStruct* viaBlock = getCamCadDoc()->getBlockAt(componentInsert->getInsert()->getBlockNumber());
            BlockStruct* foundComponentGeometry = NULL;   
            if(viaBlock && !viaGeometryMap.Lookup(viaBlock,foundComponentGeometry))
               viaGeometryMap.SetAt(viaBlock,viaBlock);
         }
      }
   }
}

bool ORCADCompInstArray::ValidateTestpoint(ORCADCompInst* componentInstance)
{
   CString PartName = componentInstance->getPartName();
   if(PartName.Find("TESTPOINT") > -1 || PartName.Find("TEST_POINT") > -1)
      return true;

   return false;
}

void ORCADCompInstArray::changeGeometryTopDefined(bool setTopDefined, CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> &bottomGeometryMap, 
                                                  DataStruct* componentInsert, CTypedPtrMap<CMapPtrToPtr,BlockStruct*,BlockStruct*> &viaGeometryMap)
{
   if(!setTopDefined || !componentInsert)
      return;

   BlockStruct* componentGeometry = getCamCadDoc()->getBlockAt(componentInsert->getInsert()->getBlockNumber());
   BlockStruct* foundComponentGeometry = NULL;   

   //If the geometry is also used by Via, Create a new geometry one
   if(componentGeometry && viaGeometryMap.Lookup(componentGeometry,foundComponentGeometry))
   {
      CString blockName = componentGeometry->getName() + "_$TP";
      BlockStruct* m_newBlock = Graph_Block_Exists(getCamCadDoc(), blockName, componentGeometry->getFileNumber());
      if(!m_newBlock)
      {
         m_newBlock = Graph_Block_On(GBO_APPEND, blockName, componentGeometry->getFileNumber(), 0L, componentGeometry->getBlockType());
         Graph_Block_Copy(componentGeometry, 0.0, 0.0, 0.0, 0, 1, -1, TRUE, FALSE);
         Graph_Block_Off();
      }
      componentGeometry = m_newBlock;
      componentInsert->getInsert()->setBlockNumber(componentGeometry->getBlockNumber());
   }

   foundComponentGeometry = NULL;
   if(!bottomGeometryMap.Lookup(componentGeometry, foundComponentGeometry))
   {
      changeGeometryToTopDefined(componentGeometry);
      componentGeometry->GenerateDesignSurfaceAttribute(getCamCadDoc()->getCamCadData(),true);
      bottomGeometryMap.SetAt(componentGeometry,componentGeometry);
   }
}
