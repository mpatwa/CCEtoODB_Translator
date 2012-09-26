
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*
*     Padforms are derived by PADSHAPE padOblong, padround ..
*  and not from the subsequent graphic definition 
*
*  The packAlt ALT1 (name "xxx") can not use the name, because the name is not
*  guarantied to be unique. It is possible (as seen in examples) that 
*  ALT1 (name "xxx") is the same as ALT5 (name "xxx") but different alt definitions.
*
*/

/*
* All changes done for TSRs are marked by "TSR ####" with a brief explanation
* of the changes done and their aim.
*
*  Recent Updates: 
*
*  05-06-02: TSR 3495 & Revamping.
*
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "pcbutil2.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "Cadifin.h"
#include "CCEtoODB.h"
#include "CamCadDatabase.h"
#include "InFile.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LineStatusUpdateInterval 200

extern CProgressDlg *progress;
//extern LayerTypeInfoStruct layertypes[];
extern char *testaccesslayers[];

static double cnv_unit(const char *);
static void free_all_mem();
static void init_all_mem();
static int delete_unused_packalt();
static int go_command(List *,int);
static int get_tok();
static int get_drillindex(double size);
static int get_padnumber(char *);
static int get_next(char *,int);
static int get_line(char *,int);
static int get_fontptr(const char *f);
static int get_partnameptr(const char *partn);
static int get_netnameptr(const char *n);
static int get_signalnameptr(char *n);
static int get_nexttextline(char *orig,char *l,int maxl);
static int get_layerptr(const char *l);
static int get_globallayerptr(const char *l);
static CString get_packagenameptr_from_altname(CString altName);
static int loop_command(List *,int);
static int p_error();
static int push_tok();
static int tok_layer();
static int tok_search(List *,int);
static int update_mirrorlayers();
static int update_netpowerlayers();
static int update_boardoutline(CCEtoODBDoc *Doc, FileStruct *file);
static int update_testaccess(CCEtoODBDoc *Doc, FileStruct *file);
static int loadCadifSettings(CString settingsFileName);
static int use_pack_alt_name();
static void fixThruHoleComponents();
static void ConvertFilledPolyOnCopperToMechanicalPin();
static DataStruct *findComponentData(FileStruct *file, CString refdes);
static void update_comppin_side(CCEtoODBDoc *doc, FileStruct *file);

static CFileReadProgress* fileReadProgress = NULL;

static   Global      G;                            /* global const. */

static   FILE        *ferr;
static   FILE        *ifp;                         /* File pointers.    */
static   long        ifp_line = 0;                 /* Current line number. */

static   CADIF_Attrib cur_attrib;

static   char        token[MAX_TOKEN];             /* Current token.       */
static   CString     cur_compname;                 /* current used component name */
static   CString     cur_padname;                  /* current used padshape name */
static   CString     cur_partdetailname;
static   CString     cur_libname;                  /* current used library name */
static   CString     cur_altname;
static   CString     cur_packref;
static   CString     cur_netname;                  /* current used netname name */
static   char        cur_text[MAX_LINE];              // yes, this has to be big.

static   CString     cur_layerusage;

/* different pins with same name can not exist */
static   int         Push_tok = FALSE;
static   char        cur_line[MAX_LINE];
static   int         cur_new = TRUE;

static CCadifPackageMap packageNameMap;

static   CPartNameArray partnamearray;
static   int         partnamecnt = 0;

static CCadifPackalts packaltarray;

static   CCompInstArray compinstarray;
static   int         compinstcnt = 0;

//static   CADIFAttr   *attr;
//static   int         attrcnt;

static   CADIFAdef   layer_attr[MAX_LAYERS];       /* Array of layers from pdif.in   */
static   int         layer_attr_cnt;

static   CADIFLayerlist layerlist[MAX_LAYERS];
static   int         layerlistcnt;

static   CADIFLayerlist globallayerlist[MAX_LAYERS];
static   int         globallayerlistcnt;

static   CCadifPadstackArray padstackarray;

static   CDrillArray drillarray;
static   int         drillcnt;

static   TextStyleArray textstylearray;
static   int         textstylecnt = 0;

static   AltPinArray ignore_altname_pinname_array;
static   int         ignore_altname_pinname_cnt = 0;

static   CAttribmapArray   attribmaparray;
static   int         attribmapcnt = 0;

typedef CArray<Point2, Point2&> CPolyArray;

static   int         polycnt =0 ;
static   CPolyArray  polyarray;

static   CNetlArray  netlistarray;
static   int         netlistcnt = 0;

static   CCompPinTestAccessArray testaccessarray;
static   int         testaccesscnt = 0;

static   CADIFArc    arc;

static   int         ppsignal[10]; // netptr+1 array for layer list
static   int         ppsignalcnt = 0;

static   int         cur_filenum = 0;
static   FileStruct  *file = NULL;
static   CCEtoODBDoc  *doc;
static   int         display_error = 0;

static   NetStruct   *curnetdata;
static   DataStruct  *curdata; 
static   CPoly       *curpoly;
static   TypeStruct  *curtype;
static   LayerStruct *curlayer;
static   BlockStruct *curblock;
static   double      faktor;
static   double      attribute_height;

static   CString     LayerRange1;
static   CString     LayerRange2;

static   CString     TopElecLayer, TopSurfLayer, TopElecLnum /* needed for buried vias */;
static   CString     BotElecLayer, BotSurfLayer, BotElecLnum /* needed for bureid vias */;

static CStringList compOutlineLayerNameList; // name for primary comp outline layers from .in file

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   bool         IGNORE_NOPADCODE_PINS;  // it can happen that in CADIF there are pins without
                                             // any padcodes. If set to false, I create a dummy pad.
static   bool         IGNORE_EMPTYPADCODE_PINS;
static   bool         IGNORE_COMPPINS;
static   bool         USE_LAYERNAME;
static   bool         CHANGEALT_PARTDETAIL_ALTNAME;
static   int         token_name;

static   CString     cur_testside;
static   int         cur_derived;   // for component padexception
static   int         cur_powernet;
static   int         page_units;

// here name use flags
static   bool         USE_PADCODE_NAME = true;
static   bool         USE_PACKALT_NAME = true;
static   bool         PADUSRDEF_ROTATION = false;
static   bool         PADUSRDEF_SCALE = false;
static   bool         DO_ISLANDS = false;

static   int         CADIF_FORMAT = CADIF_CADSTAR;

static   CString     cur_layerref;
static   CString     cur_bareboard;
static   CADIF_Version  cadif_version;
static   int         COMPHEIGHT_UNIT;
static   bool        COPPER_TO_MECHPIN;
static   bool        READSYMLABEL;
static   bool        CONVERT_COMPLEX_APERTURES;

static CCadifReader* s_cadifReader = NULL;

static CTypeChanger MakeInto_CommandProcessor; ////change made here

CCadifReader& getCadifReader()
{
   if (s_cadifReader == NULL)
   {
      s_cadifReader = new CCadifReader(*doc);
   }

   return *s_cadifReader;
}

// Case 1500
// For defCompPosn  (default component position)
static Point2  DefCompPosn;

/******************************************************************************
* ReadCADIF
*/
void ReadCADIF(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   file = NULL;
   doc = Doc;
   double minimumTHDrillSize = doc->getSettings().m_dMinimumTHDrillSize;
   doc->getSettings().m_dMinimumTHDrillSize = 0.;

   getCadifReader().setStartingBlockIndex(doc->getMaxBlockIndex());

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f,fname);
   strcat(f,ext);

   faktor = Units_Factor(UNIT_TNMETER, pageunits); // redac units to page units
   attribute_height = 0.07 * Units_Factor(UNIT_INCHES, pageunits);   // redac units to page units

   page_units = pageunits;
   cur_line[0] = '\0';
   display_error = 0;
   curdata = NULL;
   curtype = NULL;
   curnetdata = NULL;
   curlayer = NULL;
   Push_tok = FALSE;
   cur_new = TRUE;
   layerlistcnt = 0;
   layer_attr_cnt = 0;
   globallayerlistcnt = 0;
   ComponentSMDrule = 0;
   cur_bareboard = "";
   COMPHEIGHT_UNIT = -1;

   // Case 1500
   DefCompPosn.x = 0.0;
   DefCompPosn.y = 0.0;
   DefCompPosn.bulge = 0.0;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer, "r")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", path_buffer);
      ErrorMessage(t, "Error");

      return;
   }
   
   CString logFile = GetLogfilePath(CADIFERR);

   if ((ferr = fopen(logFile, "wt")) == NULL)
   {
     ErrorMessage("Error open file", logFile);

     return;
   }

   CTime t;
   t = t.GetCurrentTime();
   fprintf(ferr,"Start loading file at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   init_all_mem();
   file = NULL;

   CString settingsFileName = Format->filename;

   loadCadifSettings(settingsFileName);

   /* initialize scanning parameters */

   fileReadProgress = new CFileReadProgress(ifp);

   rewind(ifp);
   ifp_line = 0;                    /* Current line number. */

   file = Graph_File_Start(f, Type_CADIF_Layout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
   getCadifReader().setPcbFile(file);

   cur_filenum = file->getFileNumber();

   Graph_Level("0", "", 1);            // just make it first.
   
   if (loop_command(start_lst, SIZ_START_LST) < 0)
   {
      // printf big prob.
      ErrorMessage("CADIF read error", "Error");
   }

   if (COPPER_TO_MECHPIN)
      ConvertFilledPolyOnCopperToMechanicalPin();

   update_netpowerlayers();
   update_mirrorlayers();

   RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

   // need to mark cur_bareboard to GR_CLASS_BOARDOUTLINE
   update_boardoutline(doc, file);
   update_testaccess(doc, file);
   update_comppin_side(doc, file);
   delete_unused_packalt();

   if (USE_PACKALT_NAME && CHANGEALT_PARTDETAIL_ALTNAME)
        use_pack_alt_name();

   // this can happen on strange via placements.
   EliminateSinglePointPolys(doc);                

   OptimizePadstacks(doc, pageunits, 1);

   fixThruHoleComponents();

   doc->purgeUnusedWidthsAndBlocks(false);

   getCadifReader().fixGeometryNames();

   doc->purgeUnusedWidthsAndBlocks(false);

   MakeInto_CommandProcessor.Apply(doc, file); /// change made here

   if (CONVERT_COMPLEX_APERTURES)
   {
      doc->OnConvertComplexApertureNonZeroWidthPolys();
   }

   generate_PINLOC(doc, file, 0);   // this function generates the PINLOC argument for all pins.

   delete fileReadProgress;
   fileReadProgress = NULL;

   fclose(ifp);

   free_all_mem();

   t = t.GetCurrentTime();
   fprintf(ferr, "End loading file at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   fclose(ferr);

   MakeInto_CommandProcessor.Reset(); //// change made here
   
   if (display_error)
      Logreader(logFile);

   doc->getSettings().m_dMinimumTHDrillSize = minimumTHDrillSize;

   delete s_cadifReader;
   s_cadifReader = NULL;
}

//-------------------------------------------------------------------------------------

void fixThruHoleComponents()
{
   CCadifCompGeometries compGeometries(nextPrime2n(500));
   CCadifPadstackGeometries padstackGeometries(nextPrime2n(500));
   int techKeyword = doc->getStandardAttributeKeywordIndex(standardAttributeTechnology);

   for (int index = 0;index < compinstarray.GetCount();index++)
   {
      CADIFCompInst* compInstance = compinstarray.GetAt(index);
      if (compInstance == NULL)
         continue;

      DataStruct& compInstanceData = compInstance->getCompInstance();
      InsertStruct* compInstanceInsert = compInstanceData.getInsert();
      if (compInstanceInsert == NULL)
         continue;

      BlockStruct* componentGeometry = doc->getBlockAt(compInstanceInsert->getBlockNumber());
      if (componentGeometry == NULL)
         continue;

      // Cases 1744, 1708, 1625, 1299
      // Get package's Lead Form, this tells us if part is THRU or SMD
      ECadifLeadform leadform = eCadifLeadformUNKNOWN;
      CString altname = compInstance->altname;
      CADIFPackAlt* packalt = packaltarray.getPackalt(altname);
      if (packalt)
      {
         CString packageName = packalt->pacname;
         CCadifPackage *cadifPkg = packageNameMap.FindPackage(packageName);
         if (cadifPkg)
         {
            leadform = cadifPkg->GetLeadform();
         }
      }


      if (cadif_version.maj >= 7 && compInstanceInsert->getPlacedBottom())
      {
         // The problem involves components that are in actuality SMD components, but their 
         // LeadForm is THRU. 
         // (Essentially, if LeadForm is SMD you can trust it, if it is THRU you can not.) 

         // If the cad “CompPin” processing is enabled, it has made a “THRU-looking” SMD part, 
         // and we want to go ahead and mirror the geometry. 

         // If it is not enabled, our SMD part (that had LeadForm THRU) is already top defined, 
         // which is what we want, and so we do not want to mirror the geometry. 
         // In this case, we use the presence of a DRILL to decide if this THRU is a 
         // real THRU (and hence go ahead and mirror, if no Drill do not mirror). 

         if (leadform == eCadifLeadformTHRU && (!IGNORE_COMPPINS || compGeometries.isGeometryDrilled(componentGeometry, padstackarray)))
         {
            componentGeometry = compGeometries.getMirroredGeometry(componentGeometry,padstackGeometries);
            compInstanceInsert->setBlockNumber(componentGeometry->getBlockNumber());
         }

      }
   }
}

/****************************************************************************
* start_cadif
*/
static int start_cadif()
{
   return loop_command(cadif_lst, SIZ_CADIF_LST);
}

/****************************************************************************
* cadif_format
*/
static int cadif_format()
{
   int majorVersion;
   int minorVersion;

   if (!get_tok())
      return p_error();  // CADIF

   if (STRCMPI(token,"CADIF"))
   {
      fprintf(ferr, "Format [%s] wrong -> CADIF expected at %ld\n", token, ifp_line);
      display_error++;

      return -1;
   }

   if (!get_tok()) 
      p_error();  // major

   majorVersion = atoi(token);

   if (majorVersion < 3 || majorVersion > 7)
   {
      fprintf(ferr, "Warning: CADIF Software Revision %d!  Unrecognized revision may cause software problems.\n", majorVersion);
      display_error++;
   }

   if (!get_tok())
      p_error();  // minor

   minorVersion = atoi(token);
   fprintf(ferr,"Version: CADIF %d %d\n", majorVersion, minorVersion);
   cadif_version.maj = majorVersion;
   cadif_version.min = minorVersion;

   return 1;
}

/****************************************************************************/
/* cadif_design
*/
static int cadif_design()
{
   return loop_command(design_lst, SIZ_DESIGN_LST);
}

/****************************************************************************/
/* cadif_fileinfo
*/
static int cadif_fileinfo()
{
   return loop_command(fileinfo_lst, SIZ_FILEINFO_LST);
}

/****************************************************************************
* CheckName
*/
static int CheckName(CString *SS,CString *BN)
{
   int loc, loc2, pos, retVal;
      
   retVal = 0;
   loc = -1;
   loc2 = -1;
   pos = -1;
   
   CString SearchStr = *SS;
   CString BlockName = *BN;
   CString subStr = "";

   CString arg1, arg2;

   SearchStr.MakeLower();
   BlockName.MakeLower();

   loc = SearchStr.Find('*');

   if (loc == -1) 
   {
      // No '*' in SearchString
      if (SearchStr.Compare(BlockName) == 0)
         return 1;
   }
   else
   {
      // if * is first char
      if ((loc == 0) && (SearchStr.GetLength() > 1))
      {
         // get location of next '*'
         SearchStr = SearchStr.Right(SearchStr.GetLength() - 1);
         loc2 = SearchStr.Find('*');

         if (loc2 > -1)
         {
            // another wildcard found
            if (loc2 == 0)
            {
               return CheckName(&SearchStr, &BlockName);
            }
            else
            {
                  // need to search for subStr in BlockName
               subStr = SearchStr.Left(loc2);
               SearchStr = SearchStr.Right(SearchStr.GetLength() - loc2);
               pos = BlockName.Find(subStr);

               if (pos > -1) // found subStr
               {
                  arg1 = SearchStr.GetString();
                  arg2 = BlockName.Right(BlockName.GetLength() - (pos + subStr.GetLength()));

                  return CheckName(&arg1, &arg2);
               }
            }
         }
         else
         {
            // no more wildcards, check against right of BlockName
            if (SearchStr.Compare(BlockName.Right(SearchStr.GetLength())) == 0)
               return 1;
         }
     }
     else if ((loc > 0) && (SearchStr.GetLength() > 1))
     {
         // if * is not 1st char
         // check against left of blockname, SearchString does not start with '*'
         subStr = SearchStr.Left(loc);
         SearchStr = SearchStr.Right(SearchStr.GetLength() - loc);

         if (subStr.Compare(BlockName.Left(subStr.GetLength())) == 0)
         {
            arg1 = SearchStr.GetString();
            arg2 = BlockName.Right(BlockName.GetLength() - (subStr.GetLength()));

            return CheckName(&arg1, &arg2);
         }
     }
     else if ((loc == 0) && (SearchStr.GetLength() == 1))
     {
        return 1; // in recursive call last '*' match with remaining BlockName
     }
   }

   return retVal;
}

/****************************************************************************
* is_attrib
*/
//static int is_attrib(const char *s, int atttype)
//{
//   for (int i=0; i<attrcnt; i++)
//   {
//      CString SearchString, BlockName;
//      SearchString = attr[i].name;
//      BlockName = s;
//
//      if (attr[i].atttype == atttype && CheckName(&SearchString, &BlockName))
//         return 1;
//   }
//
//   return 0;
//}

/****************************************************************************
* update_testaccess
*/
static int update_testaccess(CCEtoODBDoc *Doc, FileStruct *file)
{
   for (int i=0; i<testaccesscnt; i++)
   {
      if (testaccessarray[i]->testaccess < 0) 
         continue;

      if (testaccessarray[i]->testaccess > 3)
         continue;

      CompPinStruct *c = test_add_comppin(testaccessarray[i]->compname, testaccessarray[i]->pinname, file);

      if (c)
      {
         doc->SetAttrib(&c->getAttributesRef(), doc->IsKeyWord(ATT_TESTACCESS, TRUE), VT_STRING, 
              testaccesslayers[testaccessarray[i]->testaccess], SA_OVERWRITE, NULL); // x, y, rot, height
      }
   }

   return 1;
}

/****************************************************************************
* update_compOutline
*/
static int update_compOutline(CCEtoODBDoc *Doc, DataStruct *np)
{
   BlockStruct *block = (np->getInsert())?doc->getBlockAt(np->getInsert()->getBlockNumber()):NULL;
   for(POSITION polyPos = (block)?block->getDataList().GetHeadPosition():NULL;polyPos;)
   {
      DataStruct *poly = block->getDataList().GetNext(polyPos);
      if(poly && poly->getDataType() == T_POLY)
      {
         int layerIndex = poly->getLayerIndex();
         LayerStruct* layer = doc->getLayerAt(layerIndex);
         if(layer && layer->getLayerType() == layerTypeComponentOutline )
         {
            poly->setGraphicClass(GR_CLASS_COMPOUTLINE);
         }
      }
   }

   return 1;
}

/****************************************************************************
* update_boardoutline
*/
static int update_boardoutline(CCEtoODBDoc *Doc, FileStruct *file)
{
   if (!strlen(cur_bareboard)) 
      return 0;

   DataStruct *np;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);

      if (np->getDataType() == T_POLY)
      {
         Attrib *a =  is_attvalue(Doc, np->getAttributesRef(), ATT_NAME, 0);

         if (a)
         {
            CString l = get_attvalue_string(Doc, a);

            if (strlen(l) && l.Compare(cur_bareboard) == 0)
            {
               np->setGraphicClass(GR_CLASS_BOARDOUTLINE);
               return 1;
            }
         }
      }
      else if(np->isInsertType(insertTypePcbComponent))
      {
         update_compOutline(Doc, np);
      }
   }

   return 0;
}

/****************************************************************************
* update_netpowerlayers
*/
static int update_netpowerlayers()
{
   for (int l=0; l<layerlistcnt; l++)
   {
      for (int i=0; i<layerlist[l].ppsignalcnt; i++)
      {
         CADIFNetl  *n = netlistarray[layerlist[l].ppsignal[i]-1];
         LayerStruct *lptr;

         if (USE_LAYERNAME)
            lptr = doc->FindLayer_by_Name(layerlist[l].username);
         else
            lptr = doc->FindLayer_by_Name(layerlist[l].name);
      
         // assign power attribute to net
         NetStruct *nn = add_net(file, n->net_name);
         doc->SetUnknownAttrib(&nn->getAttributesRef(), ATT_POWERNET, "", SA_OVERWRITE, NULL);

         // assign net to layer
         doc->SetUnknownAttrib(&lptr->getAttributesRef(), ATT_NETNAME, n->net_name, SA_APPEND, NULL); // x, y, rot, height
      }
   }

   return 1;
}

/****************************************************************************
* Create_LayerrangePADSTACKData
*/
static int Create_LayerrangePADSTACKData(CDataList *DataList, const char *lay1, const char *lay2)                    
{
   DataStruct *np;
   int typ = 0;   

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_TOOL))
            {
               // always do the drill
               DataStruct  *d = Graph_Block_Reference(block->getName(), np->getInsert()->getRefname(), 
                                block->getFileNumber(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), np->getInsert()->getAngle(), 
                                np->getInsert()->getMirrorFlags(), np->getInsert()->getScale(), np->getLayerIndex(), TRUE);
               d->getInsert()->setInsertType(np->getInsert()->getInsertType());
            }
            else
            {
               // here check the layer.
               LayerStruct *l = doc->FindLayer(np->getLayerIndex());
         
               if (l->getName().CompareNoCase("elecLayers") == 0)
               {
                  int start = FALSE;

                  for (int i=0; i<layerlistcnt; i++)
                  {
                     if (STRCMPI(layerlist[i].name, lay1) == 0) start = TRUE;

                     if (start)
                     {
                        int lay = Graph_Level(layerlist[i].username, "", 0);
                        DataStruct  *d = Graph_Block_Reference(block->getName(), np->getInsert()->getRefname(), 
                                         block->getFileNumber(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), np->getInsert()->getAngle(), 
                                         np->getInsert()->getMirrorFlags(), np->getInsert()->getScale(),lay, TRUE);
                        d->getInsert()->setInsertType(np->getInsert()->getInsertType());
                     }

                     // it has to include endlayer
                     if (STRCMPI(layerlist[i].name, lay2) == 0) start = FALSE;
                  }
                  return 0;
               }
               else
               {
                  fprintf(ferr,"Expanding of blind/buried via layer failed for [%s] [%s] [%s] at %ld\n", block->getName(), lay1, lay2, ifp_line);
                  display_error++;

                  int start = FALSE;

                  for (int i=0; i<layerlistcnt; i++)
                  {
                     if (STRCMPI(layerlist[i].name, lay1) == 0) start = TRUE;

                     if (start)
                     {
                        CString lname = layerlist[i].username;
                        int lay = Graph_Level(lname, "", 0);
                        DataStruct  *d = Graph_Block_Reference(block->getName(), np->getInsert()->getRefname(), 
                                         block->getFileNumber(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), np->getInsert()->getAngle(), 
                                         np->getInsert()->getMirrorFlags(), np->getInsert()->getScale(),lay, TRUE);
                        d->getInsert()->setInsertType(np->getInsert()->getInsertType());
                     }

                     // include endlayer
                     if (STRCMPI(layerlist[i].name, lay2) == 0) start = FALSE;
                  }
                  return 0;
               }
            }
         }
      }
   }

   return typ;
} /* end Create_LayerrangePadstackData */


/****************************************************************************
* go_command
*
*   Call function associated with next tolen.
*   Tokens enclosed by () are searched for on the local
*   token list.
*/
int go_command(List *tok_lst, int lst_size)
{
   int i;
   int brk;

   if (!get_tok()) 
      return p_error();

   switch((brk = tok_search(brk_lst, SIZ_BRK_LST)))
   {
   case BRK_ROUND:
      if (get_tok() && (i = tok_search(tok_lst, lst_size)) >= 0)
      {
         (*tok_lst[i].function)();
      }
      else
      {
         fnull(); // unknown command, log and skip
         //return 1;
      }

      break;
   case BRK_B_ROUND:
      push_tok();
      return 1;
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

/****************************************************************************
* loop_command
*   Loop through a section.
*/
static int loop_command(List *list,int size)
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

      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_ROUND)
         repeat = FALSE;

      push_tok();
   }

   return 1;
}


/****************************************************************************
* fnull
*
*   NULL function.
*   Skips over any tokens to next ) endpoint.
*/
static int fnull()
{
   int brk_count = 0;

   fprintf(ferr, "Token [%s] at %ld unknown\n", token, ifp_line);
   display_error++;

   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   // do not check bracket as part of token  
            continue;
         
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
            // fprintf(ferr," -> Token [%s] at %ld unknown\n",token,ifp_line);
            break;
         }
      }
      else
         return p_error();
   }

   return 0;
}

/****************************************************************************
* fskip
*
*   NULL function.
*   Skips over any tokens to next () endpoint.
*/
static int fskip()
{
   int      brk_count = 0;

/*
   fprintf(ferr,"Token [%s] at %ld skipped\n",token,ifp_line);
   display_error++;
*/
   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)  // do not check bracket as part of token
            continue;  

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
         return p_error();
   }

   return 0;
}

/****************************************************************************
* padusr_padnormal
*/
static int padusr_padnormal()
{
   loop_command(padnormal_lst, SIZ_PADNORMAL_LST);
   return 1;
}

/****************************************************************************
* systemgrid
*/
static int systemgrid()
{
   double c;

   if (!get_tok()) 
      return p_error();  // CADIF

   c = cnv_unit(token);

   return 1;
}

/****************************************************************************
* padtopad
*/
static int padtopad()
{
   double c;

   if (!get_tok()) 
      return p_error();  // CADIF

   c = cnv_unit(token);

   return 1;
}

/****************************************************************************
* padtotrack
*/
static int padtotrack()
{
   if (!get_tok()) 
      return p_error();  // CADIF

   double c = cnv_unit(token);

   return 1;
}

/****************************************************************************
* tracktotrack
*/
static int tracktotrack()
{
   double c;

   if (!get_tok())
      return p_error();  // CADIF

   c = cnv_unit(token);

   return 1;
}

/****************************************************************************
* swaplayer_swaplayer
*/
static int swaplayer_swaplayer()
{
   CString layerListName1;
   CString layerListName2;
   int layerListIndex;

   if (!get_tok())
      return p_error();  // L1

   layerListName1 = token;
   layerListIndex = get_layerptr(layerListName1);

   if (USE_LAYERNAME && strlen(layerlist[layerListIndex].username))
   {
      layerListName1 = layerlist[layerListIndex].username;
   }
   
   if (!get_tok())  
      return p_error();  // L2

   layerListName2 = token;
   layerListIndex = get_layerptr(layerListName2);

   if (USE_LAYERNAME && strlen(layerlist[layerListIndex].username))
   {
      layerListName2 = layerlist[layerListIndex].username;
   }

   Graph_Level(layerListName1,"", 0);
   Graph_Level(layerListName2,"", 0);
   Graph_Level_Mirror(layerListName1, layerListName2, "");

   return 1;
}

/****************************************************************************
* wildcardnet
*/
static int wildcardnet()
{
   G.cur_left = G.cur_right;  // used for Normal routing width and necked width
   loop_command(wildcardnet_lst, SIZ_WILDCARDNET_LST);
   // * net is default for all nets
   //rc = get_widthcode(G.cur_left);
   //nc = get_widthcode(G.cur_right);
/*
   if (!STRCMPI(G.name,"*"))
   {
      for (i=0;i<netlistcnt;i++)
      {
         if (!netlist[i].routcodeset)
            netlist[i].routcode = rc;
         if (!netlist[i].neckcodeset)
            netlist[i].neckcode = nc;
      }      
   }
   else
   {
      if((netptr = get_netnameptr(G.name)) > -1)
      {
         netlist[netptr].routcode = rc;
         netlist[netptr].routcodeset = TRUE;
         netlist[netptr].neckcode = nc;
         netlist[netptr].neckcodeset = TRUE;
      }
   }
*/
   return 1;
}

/****************************************************************************
* swaplayer
*/
static int swaplayer()
{
   loop_command(swaplayer_lst, SIZ_SWAPLAYER_LST);

   return 1;
}

/****************************************************************************
* comp_location
*/
static int comp_location()
{
   G.cur_bottom = 0;
   G.cur_mirror = 0;
   G.cur_rotation = 0.0;

   return loop_command(location_lst, SIZ_LOCATION_LST);
}

/****************************************************************************
* cadif_name
*/
static int cadif_name()
{
   if (!get_tok())   
      return p_error();  // name

   CString tmp = token;
   tmp.TrimLeft();
   tmp.TrimRight();
   strcpy(G.name, tmp);

   return 1;
}

/****************************************************************************
* update_padstack
*/
static int update_padstack(BlockStruct *block, int pinNumber, const char *padstackName)
{
   int found = 0;

   CString pinNumberString;
   pinNumberString.Format("%d", pinNumber);

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
      
      if (data->getDataType() == T_INSERT)        
      {
         if (data->getInsert()->getInsertType() == insertTypePin)
         {
            // pinname is in refname if a insert

            // these are absolute instructions.
            Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_COMPPINNR, 0);

            if (attrib)
            {
               CString pinNumberAttrib = get_attvalue_string(doc, attrib);

               if (pinNumberString.Compare(pinNumberAttrib) == 0)
               {
                  BlockStruct *padStackBlock = Graph_Block_On(GBO_APPEND, padstackName, file->getFileNumber(), 0);
                  Graph_Block_Off();

                  data->getInsert()->setBlockNumber(padStackBlock->getBlockNumber());

                  found++;
                  break;
               }
            }
         }
      }
   } 

   if (!found)
   {
      fprintf(ferr,"Update error : Geometry [%s] Pinnr [%d] Padstack [%s]\n", block->getName(), pinNumber, padstackName);
   }

   return found;
}

/****************************************************************************
* comppin_padcoderef
*/
static int comppin_padcoderef()
{
   if (!get_tok())
      return p_error();  // name

   if (IGNORE_COMPPINS)
      return 1;

   CADIFPadstack* padstack = padstackarray.getDefinedAt(token);

   if (USE_PADCODE_NAME)
   {
      if (strlen(padstack->padname) != 0)
      {
         strcpy(token,padstack->padname);
      }
   }

   // curdata is the current insert
   BlockStruct *origBlock = doc->Find_Block_by_Num(curdata->getInsert()->getBlockNumber());

   if (cur_derived == FALSE)
   {
      CString newgeom;
      newgeom.Format("%s_%s", origBlock->getName(), curdata->getInsert()->getRefname());
      BlockStruct *newBlock = Graph_Block_On(GBO_APPEND, newgeom, file->getFileNumber(), 0);

      //if (is_attrib(newBlock->getName(), FIDUCIAL_ATTR) != 0)
      //   newBlock->setBlockType(blockTypeFiducial);
      //else if (is_attrib(newBlock->getName(), TEST_ATTR) != 0)
      //   newBlock->setBlockType(blockTypeTestPoint);
      //else
      newBlock->setBlockType(blockTypePcbComponent);

      // loop through every pinnr and update the pinname
      Graph_Block_Copy(origBlock, 0, 0, 0, 0, 1, -1, TRUE);
      Graph_Block_Off();

      doc->SetUnknownAttrib(&newBlock->getAttributesRef(), ATT_DERIVED, origBlock->getName(), SA_APPEND, NULL); // x, y, rot, height
      curdata->getInsert()->setBlockNumber(newBlock->getBlockNumber()); // here assign new block to comp.
      cur_derived = TRUE;

      // also copy to comp
      doc->SetUnknownAttrib(&curdata->getAttributesRef(), ATT_DERIVED, origBlock->getName(), SA_APPEND, NULL); // x, y, rot, height

      // ATT_COMPPINNR is the pin number == G.cur_id
      update_padstack(newBlock, G.cur_id, token);
   }
   else
   {  
      // ATT_COMPPINNR is the pin number == G.cur_id
      update_padstack(origBlock, G.cur_id, token);
   }

/*
   fprintf(ferr, "Can not add padexception [%s] to component [%s] pinid [%d] at %ld\n",
      token, G.name, G.cur_id, ifp_line);
   display_error++;
*/
   return 1;
}

/****************************************************************************
* comppin_testside
*/
static int comppin_testside()
{
   if (!get_tok())
      return p_error();  // TST_BOTTOM, TST_TOP,  

   CADIFCompPinTestAccess *a = new CADIFCompPinTestAccess;
   testaccessarray.SetAtGrow(testaccesscnt, a);
   testaccesscnt++;
   
   a->compname = G.name;
   a->pinnr = G.cur_id;
   a->pinname = "";

   if (!STRCMPI(token, "TST_BOTTOM"))
      a->testaccess = ATT_TEST_ACCESS_BOT;
   else if (!STRCMPI(token, "TST_TOP"))
      a->testaccess = ATT_TEST_ACCESS_TOP;
   else
   {
      fprintf(ferr, "Unknown TestSide [%s]\n", cur_testside);
      display_error++;
      a->testaccess = -1;
   }

   return 1;
}

/****************************************************************************
* cadif_padcoderef
*/
static int cadif_padcoderef()
{
   if (!get_tok())
      return p_error();  // name

   CADIFPadstack* padstack = padstackarray.getDefinedAt(token);

   if (USE_PADCODE_NAME)
   {
      if (strlen(padstack->padname) != 0)
      {
         strcpy(token, padstack->padname);
      }
   }

   // CADIFPadstack *p = padstackarray[ptr];
   // do not assign a padstack if it is empty
   if (IGNORE_EMPTYPADCODE_PINS)
   {
      if (padstack->padassign || padstack->paddrill)
      {
         cur_padname = token;
      }
   }
   else
   {
      cur_padname = token;
   }

   return 1;
}

/****************************************************************************
* cadif_testside
*/
static int cadif_testside()
{
   if (!get_tok())
      return p_error();  // test side TST_BOTTOM etc...

   cur_testside = token;

   return 1;
}

/****************************************************************************
* viapadcode
*/
static int viapadcode()
{
   //int   ptr;

   if (!get_tok())
      return p_error();  // name

   CADIFPadstack* padstack = padstackarray.getDefinedAt(token);
   padstack->used_as_via = TRUE;

   fskip();
   return 1;
}

/****************************************************************************
* wcnwidth
*/
static int wcnwidth()
{
   if (!get_tok())
      return p_error();  // normal width

   G.cur_left = cnv_unit(token);

   if (!get_tok())
      return p_error();  // necked width

   G.cur_right = cnv_unit(token);
   fskip();

   return 1;
}

/****************************************************************************
* cadif_area_type
*/
static int cadif_area_type()
{
   if (!get_tok())
      return p_error();  // type

   if (curdata)
      doc->SetAttrib(&curdata->getAttributesRef(),doc->RegisterKeyWord(token, 0, VT_NONE), VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height

   if (!STRICMP(token,"BOARD_AREA"))
      curdata->setGraphicClass(GR_CLASS_BOARD_GEOM);

   return 1;
}

/****************************************************************************
* cadif_cutout
*/
static int cadif_cutout()
{
   G.cur_filltype = FIG_CUTOUT;

   int res = loop_command(pathfigure_lst, SIZ_PATHFIGURE_LST);

   return res;
}

/****************************************************************************
* cadif_island
*/
static int cadif_island()
{
   if (!DO_ISLANDS)
      return fskip(); 

   G.cur_filltype = FIG_CUTOUT;

   int res = loop_command(pathfigure_lst, SIZ_PATHFIGURE_LST);

   return res;
}

/****************************************************************************
* cadif_filltype
*/
static int cadif_filltype()
{
   if (!get_tok())
      return p_error();  // type

   G.cur_filltype = 0;

   if (!STRCMPI(token, "CLEAR"))
   {
      G.cur_filltype = FILLTYPE_CLEAR;
   }
   else if (!STRCMPI(token, "SOLID"))
   {
      G.cur_filltype = FILLTYPE_SOLID;
   }
   else
      fprintf(ferr, "Unknown fill type [%s] at %ld\n", token, ifp_line);


   // Case 2063 - In the cad file sample for this case, the FILLTYPE command
   // is at the end of a filled area, after all the geometric items. This CAMCAD
   // reader has already read and instantiated all those graphics, and now
   // after they all exist, they finally give us the fill parameter. So it is
   // too late to just set the flag and march on. We have to process the
   // current item and reset fill flags accordingly.

   if (curdata != NULL && curdata->getDataType() == dataTypePoly)
   {
      POSITION pos = curdata->getPolyList()->GetHeadPosition();
      while (pos != NULL)
      {
         CPoly *poly = curdata->getPolyList()->GetNext(pos);
         poly->setFilled( G.cur_filltype == FILLTYPE_SOLID );
      }
   }

   return 1;
}

/****************************************************************************
* update_testaccesspinname
*/
static int update_testaccesspinname(const char *compname, int pinnr, const char *pinname)
{
   for (int i=0; i<testaccesscnt; i++)
   {
      CADIFCompPinTestAccess *tstAccess = testaccessarray[i];

      if (tstAccess->compname.Compare(compname) == 0 && tstAccess->pinnr == pinnr)
      {
         tstAccess->pinname = pinname;

         return 1;
      }
   }

   return 1;
}

/****************************************************************************
* ignore_altpack_pinname
*/
static int ignore_altpack_pinname(const char *altname, const char *pinname)
{
   for (int i=0; i<ignore_altname_pinname_cnt; i++)
   {
      if (!ignore_altname_pinname_array[i]->altname.Compare(altname) &&
          !ignore_altname_pinname_array[i]->pinname.Compare(pinname))
      {
         return 1;
      }
   }

   return 0;
}

/****************************************************************************
* comppinref
*/
static int comppinref()
{
   if (!get_tok())
      return p_error();  // name

   CString pinID = token;

   int res = loop_command(compref_lst, SIZ_COMPREF_LST); // must be before getting the pinname
   CString packageID = get_packagenameptr_from_altname(cur_altname);

   if (packageID.IsEmpty())
   {
      fprintf(ferr,"Can not find PackAlt [%s] in definition.\n", cur_altname);
      display_error++;
      return -1;
   }

   CCadifPin* pin = packageNameMap.FindPackagePin(packageID, pinID);
   if (pin == NULL)
      return 0;

   if(ignore_altpack_pinname(cur_altname, pin->GetName()))
   {
      fprintf(ferr,"Component [%s] Pin [%s] ignored due to IGNORED Padstack.\n", cur_compname, pin->GetName());
      display_error++;
      return 0;
   }

   update_testaccesspinname(cur_compname, pin->GetPinNumber(), pin->GetName());
   NetStruct *net = add_net(file, cur_netname);
   add_comppin(file, net, cur_compname, pin->GetName());

   return res;
}

/****************************************************************************
* cadif_pt
*/
static int cadif_pt()
{
   Point2 firstPoint;

   if (!get_tok())
      return p_error();  // x

   firstPoint.x = cnv_unit(token);

   if (!get_tok())
      return p_error();  // y

   firstPoint.y = cnv_unit(token);
   firstPoint.bulge = 0;

   if (polycnt)
   {
      Point2 secondPoint = polyarray.ElementAt(polycnt-1);

      // if this is the same coo as the last one, do not write it.
      // this seems to happen a lot on pt arc combinations.
      if (firstPoint.x == secondPoint.x && firstPoint.y == secondPoint.y)  
         return 1;
   }

   polyarray.SetAtGrow(polycnt, firstPoint);
   polycnt++;

   return 1;
}

/****************************************************************************
* arc_pt
*/
static int arc_pt()
{
   if (!get_tok())
      return p_error();  // x

   arc.x = cnv_unit(token);

   if (!get_tok())
      return p_error();  // y

   arc.y = cnv_unit(token);

   return 1;
}

/****************************************************************************
* arc_bulge
*
*  A is multiplied by 10 to the power of B
*  314159 -5 = 3.14159
*/
static int arc_bulge()
{
   if (!get_tok()) 
      return p_error();  // A

   double a = atof(token);

   if (!get_tok()) 
      return p_error();  // B

   int b = atoi(token);

   if (b < 0)
   {
      for (int i=0; i<abs(b); i++)
          a /= 10;
   }
   else if (b > 0)
   {
      for (int i=0; i<abs(b); i++)
          a *= 10;
   }
   else
       a = 0;

   arc.bulge= a;

   return 1;
}

/****************************************************************************
* cadif_arc
*
*  first_arc situation
*  (shape
*   (arc (e -6373558 -5) (pt -53561 -12500))
*   (pt -80000 -12500)
*   (pt -80000 -80000)
*   (pt -12500 -80000)
*   (pt -12500 -53561)))
*/
static int cadif_arc()
{
   // this fill the arc structure
   // arc in cadif is from point to bulge and in
   // CAMCAD the first coo gets the bulge
   Point2 firstPoint;   

   loop_command(arc_lst ,SIZ_ARC_LST);

   // (arc (e 36 1) (pt 50800 50800))
   if (fabs(fabs(arc.bulge) - 360) < 1)
      arc.bulge = 0;

   if (polycnt == 0)
   {
      //fprintf(ferr,"Arc in start of Poly at %ld\n",ifp_line);
      double b = DegToRad(arc.bulge);
      double c = tan(b / 4);
      firstPoint.x = arc.x;
      firstPoint.y = arc.y;
      firstPoint.bulge = c;   
      polyarray.SetAtGrow(polycnt, firstPoint); // do it double because this point is used
                                       // later to get the start point from the last point.
      polycnt++;
      firstPoint.bulge = 0.0;
      polyarray.SetAtGrow(polycnt, firstPoint);
      polycnt++;
      G.first_arc = TRUE;
   }
   else
   {
      double b = DegToRad(arc.bulge);
      double c = tan(b / 4);
      firstPoint.x = arc.x;
      firstPoint.y = arc.y;

      Point2 secondPoint = polyarray.ElementAt(polycnt - 1);
      secondPoint.bulge = c;
      polyarray.SetAt(polycnt - 1, secondPoint);

      firstPoint.bulge = 0.0;
      polyarray.SetAtGrow(polycnt, firstPoint);
      polycnt++;
   }

   return 1;
}

/****************************************************************************
* cadif_mirrored
*/
static int cadif_mirrored()
{
   G.cur_mirror = MIRROR_FLIP | MIRROR_LAYERS;

   return 1;
}

/****************************************************************************
* cadif_side
*/
static int cadif_side()
{
   if (!get_tok())
      return p_error();  // 

   if (!STRCMPI(token, "BOTTOM"))
      G.cur_bottom = TRUE;

   return 1;
}

/****************************************************************************
* cadif_position
*/
static int cadif_position()
{
   polyarray.RemoveAll();
   polycnt = 0;

   int res = loop_command(pt_lst, SIZ_PT_LST);

   if (polycnt == 1)
   {
      Point2   p = polyarray.ElementAt(0);
      G.cur_pos_x = p.x;
      G.cur_pos_y = p.y;
   }
   else
   {
      fprintf(ferr,"Position error\n");
   }

   return res;
}

/******************************************************************************
* attrib_position
*/
static int attrib_position()
{
   polyarray.RemoveAll();
   polycnt = 0;

   int res = loop_command(pt_lst, SIZ_PT_LST);

   if (polycnt == 1)
   {
      Point2   p = polyarray.ElementAt(0);
      cur_attrib.x = p.x;
      cur_attrib.y = p.y;
   }
   else
   {
      fprintf(ferr,"Position error\n");
   }

   return res;
}

/****************************************************************************
* cadif_string
*/
static int cadif_string()
{
   if (!get_tok()) 
      return p_error();  // name

   strcpy(cur_text,token);

   return 1;
}

/****************************************************************************
* padsymref
*/
static int padsymref()
{
   if (!get_tok())
      return p_error();  // name

   cur_padname = token;

   return 1;
}

/****************************************************************************
* packalt_packref
*/
static int packalt_packref()
{
   if (!get_tok())
      return p_error();  // name

   cur_packref = token;

   CCadifPackage* package = packageNameMap.FindPackage(cur_packref);
   if (package == NULL)
   {
      fprintf(ferr, "Package name [%s] not found in package list at %ld\n", cur_packref, ifp_line);
      display_error++;
   }
   else
   {
       cur_libname = package->GetName();
   }

   G.cur_pincnt = 0;
   return 1;
}

/****************************************************************************
* partref
*/
static int partref()
{
   if (!get_tok())
      return p_error();  // name

   cur_libname = token;

   return 1;
}

/****************************************************************************
* signalref
*/
static int signalref()
{
   if (!get_tok())
      return p_error();  // name

   int nptr = get_signalnameptr(token);

   if (nptr < 0)
   {
      fprintf(ferr, "Signal Ref [%s] not found in Signal List\n", token);
      display_error++;
      cur_netname = token;
   }
   else
      cur_netname = netlistarray[nptr]->net_name;

   return 1;
}

/****************************************************************************
* cadif_planetype
*/
static int cadif_planetype()
{
   if (!get_tok())
      return p_error();  // FULL, SPLIT
   else
      return 1;
}

/****************************************************************************
* signalnetjoins
*/
static int signalnetjoins()
{
   loop_command(netjoins_lst, SIZ_NETJOINS_LST);

   return 1;
}

/****************************************************************************
* treespacing
*/
static int treespacing()
{
   loop_command(treespacing_lst, SIZ_TREESPACING_LST);

   return 1;
}

/****************************************************************************
* compref
*/
static int compref()
{
   if (!get_tok())
      return p_error();  // name

   for (int i=0; i<compinstcnt; i++)
   {
      CADIFCompInst *cmpInst = compinstarray[i];

      if (!strcmp(cmpInst->cname, token))
      {
         cur_compname = cmpInst->compname;
         cur_altname =  cmpInst->altname;
         return 1;
      }
   }

   fprintf(ferr, "Componentinstance [%s] not found\n", token);
   display_error++;

   return 1;
}

/****************************************************************************
* packref
*/
static int packref()
{
   if (!get_tok())
      return p_error();  // name

   cur_libname = token;

   return 1;
}

/****************************************************************************
* symbolref
*/
static int symbolref()
{
   if (!get_tok())
      return p_error();  // name

   cur_libname = token;

   return 1;
}

/****************************************************************************
* partdes_description
*/
static int partdes_description()
{
   if (!get_tok())
      return p_error();  // name

   if (curtype)
   {
      doc->SetAttrib(&curtype->getAttributesRef(),doc->RegisterKeyWord("description", 0, VT_STRING), 
                     VT_STRING, token, SA_APPEND, NULL); // x, y, rot, height
   }

   return 1;
}

/****************************************************************************
* partdes_detailname
*/
static int partdes_detailname()
{
   if (!get_tok())
      return p_error();  // name

   cur_partdetailname = token;

   if (curtype)
   {
      doc->SetAttrib(&curtype->getAttributesRef(),doc->RegisterKeyWord("detailname", 0, VT_STRING),
                     VT_STRING,
                     token,    // this makes a "real" char *
                     SA_APPEND, NULL); // x, y, rot, height
   }

   return 1;
}

/****************************************************************************
* partdes_prefix
*/
static int partdes_prefix()
{
   if (!get_tok())
      return p_error();  // name

   if (curtype)
   {
      doc->SetAttrib(&curtype->getAttributesRef(),doc->RegisterKeyWord("prefix", 0, VT_STRING),
                     VT_STRING,
                     token,    // this makes a "real" char *
                     SA_OVERWRITE, NULL); //  
   }

   return 1;
}

/****************************************************************************
* packaltref
*/
static int packaltref()
{
   if (!get_tok())
      return p_error();  // name

   CADIFPackAlt* packalt = packaltarray.getPackalt(token,false);

   if (USE_PACKALT_NAME && packalt != NULL && !packalt->getRealName().IsEmpty() && !packalt->already_exist)
      cur_altname = packalt->getRealName();
   else
      cur_altname = token;

   return 1;
}

/****************************************************************************
* cadif_powersignal
*/
static int cadif_powersignal()
{
   cur_powernet = TRUE;
   doc->SetUnknownAttrib(&curnetdata->getAttributesRef(),ATT_POWERNET, "", SA_OVERWRITE, NULL); //

   return 1;
}

/****************************************************************************
* cadif_layerlist
*/
static int cadif_layerlist()
{
   // G.cur_layer[0] = '\0';

   while (TRUE)
   {
      if (!get_tok())
         return p_error();

      if (!STRCMPI(token, ")"))
      {
         push_tok();
         break;
      }

      //if (strlen(G.cur_layer))   strcat(G.cur_layer,"-");
      //strcat(G.cur_layer, token);
   }

   return 1;
}

/****************************************************************************
* layerref
*/
static int layerref()
{
   if (!get_tok())
      return p_error();  // name

   CString lay = token;
   int l = get_layerptr(lay);

   if (USE_LAYERNAME && strlen(layerlist[l].username))
   {
      lay = layerlist[l].username;
   }

   G.cur_layerindex = Graph_Level(lay, "", 0);

   if (curdata)
   {
      switch (layerlist[l].usage)
      {
      case L_NO_TRACKS:
         curdata->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
         break;
      case L_NO_VIAS:
         curdata->setGraphicClass(GR_CLASS_VIAKEEPOUT);
         break;
      case L_PROFILING:
         curdata->setGraphicClass(GR_CLASS_MILLINGPATH);
         break;
      }
   }

   return 1;
}

/****************************************************************************
* attrib_layerref
*/
static int attrib_layerref()
{
   if (!get_tok())
      return p_error();  // name

   CString lname = token;

   int l = get_layerptr(lname);

   if (USE_LAYERNAME && strlen(layerlist[l].username))
      lname = layerlist[l].username;

   if (!STRCMPI(token, "L0")) // this is token, because L0 is treated different
   {
      cur_attrib.cur_layerindex = 0;
   }
   else if (!STRCMPI(token, "GL0"))   // this is token, because GL0 is treated different
   {
      cur_attrib.cur_layerindex = 0;
   }
   else
   {
      //if (lname.CompareNoCase("Silkscreen Bot") == 0)
      //{
      //   int iii = 3;
      //}

      cur_attrib.cur_layerindex = Graph_Level(lname, "", 0);
   }

   return 1;
}

/****************************************************************************
* layerstacklayer_layerref
*/
static int layerstacklayer_layerref()
{
   if (!get_tok())
      return p_error();  // name

   cur_layerref = token;

   return 1;
}

/****************************************************************************
* do_layerclass
*/
static int do_layerclass(int i)
{
   CString layerName = layerlist[i].name;

   if (USE_LAYERNAME && strlen(layerlist[i].username))
      layerName = layerlist[i].username;

   G.cur_layerindex = Graph_Level(layerName, "", 0);

   if (curdata)
   {
      if (compOutlineLayerNameList.Find(layerName))
         curdata->setGraphicClass(GR_CLASS_COMPOUTLINE);
      else
      {
         switch (layerlist[i].usage)
         {
         case L_NO_TRACKS:
            curdata->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
            break;
         case L_NO_VIAS:
            curdata->setGraphicClass(GR_CLASS_VIAKEEPOUT);
            break;
         case L_PROFILING:
            curdata->setGraphicClass(GR_CLASS_MILLINGPATH);
            break;
         }
      }
   }

   return 1;
}

/****************************************************************************
* symlayerref
*
*   here we need to use layerRef to really assign the
*   right global layer to layer.
*/
static int symlayerref()
{
   if (!get_tok())
      return p_error();  // name

   // get the global index number
   int i = get_globallayerptr(token);
   int index = globallayerlist[i].id;

   // if we found a layerref, use this
   // get the layer ref name, not the global layer.
   if (globallayerlist[i].layerref > -1)
      return do_layerclass(globallayerlist[i].layerref);

   // now find the L layer with the same index. In CADIF version 4 there is a layerref on the global layer.
   // also if use of layer names - layer get merged by names, not GLxx or Lxx
   if (cadif_version.maj < 4)
   {
      for (i=0;i<layerlistcnt;i++)
      {
         if (layerlist[i].id == index)
            return do_layerclass(i);
      }
   }

   if (USE_LAYERNAME && strlen(globallayerlist[i].username))
   {
      strcpy(token, globallayerlist[i].username);
   }
   else
   {
      fprintf(ferr, "Global layer [%s] - no index found\n", token);
      display_error++;
   }

   G.cur_layerindex = Graph_Level(token, "", 0);

   return 1;
}

/****************************************************************************/
/* eleclayers
*/
static int eleclayers()
{
   G.cur_layerindex = Graph_Level("elecLayers", "", 0);
   LayerStruct *l = doc->FindLayer(G.cur_layerindex);
   l->setLayerType(LAYTYPE_PAD_ALL);

   return 1;
}

/****************************************************************************/
/* alllayers
*/
static int alllayers()
{
   G.cur_layerindex = Graph_Level("allLayers", "", 0);
   LayerStruct *l = doc->FindLayer(G.cur_layerindex);
   l->setLayerType(LAYTYPE_PAD_ALL);

   return 1;
}

/****************************************************************************/
/* cadif_orientation
*/
static int cadif_orientation()
{
   if (!get_tok())
      return p_error();  //

   G.cur_rotation = atof(token);

   return 1;
}

/****************************************************************************/
/* cadif_scale
*/
static int cadif_scale()
{
   loop_command(scale_lst, SIZ_SCALE_LST);

   return 1;
}

/****************************************************************************/
/* cadif_layerrange
*/
static int cadif_layerrange()
{
   if (!get_tok())
      return p_error();  //

   LayerRange1 = token;

   if (!get_tok())
      return p_error();  //

   LayerRange2 = token;

   return 1;
}

/****************************************************************************/
/* cadif_height
*/
static int cadif_height()
{
   if (!get_tok())
      return p_error();  //

   G.cur_height = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/* cadif_width
*/
static int cadif_width()
{
   if (!get_tok())
      return p_error();  //

   int err;
   double w = cnv_unit(token);
   G.cur_widthindex = Graph_Aperture("", T_ROUND, w, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   return 1;
}

/****************************************************************************/
/* topeleclayer
*/
static int topeleclayer()
{
   if (!get_tok())
      return p_error();  //

   TopElecLayer = token;
   TopElecLnum = token;    // needed for buried vias

   int i = get_layerptr(TopElecLayer);

   if (USE_LAYERNAME && strlen(layerlist[i].username))
   {
      TopElecLayer = layerlist[i].username;
   }

   i = Graph_Level(TopElecLayer, "", 0);

   LayerStruct *l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_SIGNAL_TOP);

   return 1;
}

/****************************************************************************/
/* boteleclayer
*/
static int boteleclayer()
{
   if (!get_tok())
      return p_error();  //

   BotElecLayer = token;
   BotElecLnum = token; // needed for buried vias

   int i = get_layerptr(BotElecLayer);

   if (USE_LAYERNAME && strlen(layerlist[i].username))
   {
      BotElecLayer = layerlist[i].username;
   }

   i = Graph_Level(BotElecLayer,"",0);
   LayerStruct *l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_SIGNAL_BOT);

   return 1;
}

/****************************************************************************/
/* topsurflayer
*/
static int topsurflayer()
{
   if (!get_tok())
      return p_error();  //

   TopSurfLayer = token;
   int i = get_layerptr(TopSurfLayer);

   if (USE_LAYERNAME && strlen(layerlist[i].username))
   {
      TopSurfLayer = layerlist[i].username;
   }
   
   i = Graph_Level(TopSurfLayer,"",0);
   LayerStruct *l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_TOP);

   return 1;
}

/****************************************************************************/
/* botsurflayer
*/
static int botsurflayer()
{

   if (!get_tok())
      return p_error();  //
   
   BotSurfLayer = token;
   int i = get_layerptr(BotSurfLayer);

   if (USE_LAYERNAME && strlen(layerlist[i].username))
   {
      BotSurfLayer = layerlist[i].username;
   }

   i = Graph_Level(BotSurfLayer,"",0);
   LayerStruct *l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   return 1;
}

/****************************************************************************
* cadif_rectangle
*
*  convert x1 y1 x2 y2 to a real poly
*/
static int cadif_rectangle()
{  
   int fill = (G.cur_filltype == FIG_FILLEDFIGURE);
   int voidtype = 0;
   
   if (G.cur_filltype == FIG_CUTOUT)
   {
      voidtype = TRUE;
      fill = TRUE;
   }

   polyarray.RemoveAll();
   polycnt = 0;
   loop_command(pt_lst, SIZ_PT_LST);

   if (curdata)
   {
      curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, voidtype, 1);  // fillflag , negative, closed

      if (polycnt < 2)
      {
         // this can happen if a non initialized rectangle (100 inch or greater) has happened !!
         curdata->setFlags(0);
      }
      else
      {
         Point2 p1 = polyarray.ElementAt(0);
         Point2 p2 = polyarray.ElementAt(1);
         Graph_Vertex(p1.x, p1.y , 0);
         Graph_Vertex(p2.x, p1.y , 0);
         Graph_Vertex(p2.x, p2.y , 0);
         Graph_Vertex(p1.x, p2.y , 0);
         Graph_Vertex(p1.x, p1.y , 0);
      }
   }

   if (polycnt)
   {
      polyarray.RemoveAll();
      polycnt = 0;
   }

   return 1;
}

/****************************************************************************/
/* cadif_path
*/
static int cadif_path()
{
   polyarray.RemoveAll();
   polycnt = 0;

   if (curdata)
      curpoly = Graph_Poly(NULL,G.cur_widthindex, 0, 0, 0); // fillflag , negative, closed

   if (loop_command(pt_lst, SIZ_PT_LST) < 0)
      return -1;

   for (int i=0; i<polycnt; i++)
   {
      Point2 p = polyarray.ElementAt(i);

      if (curdata)
         Graph_Vertex(p.x, p.y, p.bulge);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/* cadif_polygon
*/
static int cadif_polygon()
{
   int fill = (G.cur_filltype == FIG_FILLEDFIGURE);
   int voidtype = 0;
   
   if (G.cur_filltype == FIG_CUTOUT)
   {
      voidtype = TRUE;
      fill = TRUE;
   }

   polyarray.RemoveAll();
   polycnt = 0;
   G.first_arc = FALSE;

   if (curdata)
      curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, voidtype, 1);  // fillflag , negative, closed

   if (loop_command(pt_lst, SIZ_PT_LST) < 0)
      return -1;

   if (polycnt < 2)
   {
      fprintf(ferr, " Invalid Polygon at %ld\n", ifp_line);
      display_error++;
      polyarray.RemoveAll();
      polycnt = 0;

      return 1;
   }

   if (G.first_arc)
   {
      // the poly 0 is incorrect and the last must be first
      Point2 p1 = polyarray.ElementAt(0);
      Point2 p2 = polyarray.ElementAt(polycnt-1);
      p1.x = p2.x;
      p1.y = p2.y;
      polyarray.SetAt(0, p1);
   }

   Point2 pointOne = polyarray.ElementAt(0);
   Point2 pointTwo = polyarray.ElementAt(polycnt-1);

   if (pointOne.x != pointTwo.x || pointOne.y != pointTwo.y)
   {
      pointTwo.x = pointOne.x;
      pointTwo.y = pointOne.y;
      pointTwo.bulge = 0;
      polyarray.SetAtGrow(polycnt, pointTwo);  // fill, type
      polycnt++;
   }

   for (int i=0; i<polycnt; i++)
   {
      Point2 point = polyarray.ElementAt(i);

      if (curdata)
         Graph_Vertex(point.x, point.y, point.bulge);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************
* cadif_shape
*
*  last koo must be closed
*/
static int cadif_shape()
{
   int voidtype = 0;
   int fill = (G.cur_filltype == FIG_FILLEDFIGURE);

   if (G.cur_filltype == FIG_CUTOUT)
   {
      voidtype = TRUE;
      fill = TRUE;
   }

   polyarray.RemoveAll();
   polycnt = 0;
   G.first_arc = FALSE;

   if (curdata)
      curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, voidtype, 1);  // fillflag , negative, closed

   if (loop_command(shapept_lst,SIZ_SHAPEPT_LST) < 0)
      return -1;

   if (G.first_arc)
   {
      // the poly 0 is incorrect and the last must be first
      Point2 p1 = polyarray.ElementAt(0);
      Point2 p2 = polyarray.ElementAt(polycnt-1);
      p1.x = p2.x;
      p1.y = p2.y;
      polyarray.SetAt(0, p1);
   }

   Point2 pointOne = polyarray.ElementAt(0);
   Point2 pointTwo = polyarray.ElementAt(polycnt-1);

   if (pointOne.x != pointTwo.x || pointOne.y != pointTwo.y)
   {
      pointTwo.x = pointOne.x;
      pointTwo.y = pointOne.y;
      pointTwo.bulge = 0;
      polyarray.SetAtGrow(polycnt, pointTwo);  // fill, type
      polycnt++;
   }

   for (int i=0; i<polycnt; i++)
   {
      Point2 point = polyarray.ElementAt(i);

      if (curdata)
         Graph_Vertex(point.x, point.y, point.bulge);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************
* cadif_openshape
*
*  Open shape is not closed by definition. 
*/
static int cadif_openshape()
{
   int   fill = 0;   // openshape should not be filled !!! (G.cur_filltype == FIG_FILLEDFIGURE);
   int   closed = 0; // close must be tested;

   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(shapept_lst, SIZ_SHAPEPT_LST) < 0)
      return -1;

   if (polycnt)
   {
      Point2 p1 = polyarray.ElementAt(0);
      Point2 p2 = polyarray.ElementAt(polycnt-1);
      
      if (fabs(p1.x - p2.x) < SMALLNUMBER && fabs(p1.y - p2.y) < SMALLNUMBER)
         closed = TRUE;
   }

   if (polycnt < 3)
   {
      fill = 0;
      closed = 0;
   }

   if (curdata)
      curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, 0, closed); // fillflag , negative, closed

   for (int i=0; i<polycnt; i++)
   {
      Point2 p = polyarray.ElementAt(i);
      if (curdata)
         Graph_Vertex(p.x, p.y, p.bulge);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/* cadif_circle
*/
static int cadif_circle()
{
   int      fill = (G.cur_filltype == FIG_FILLEDFIGURE);
   int      voidtype = 0;

   if (G.cur_filltype == FIG_CUTOUT)
   {                                    
      voidtype = TRUE;
      fill = TRUE;
   }

   polyarray.RemoveAll();
   polycnt = 0;

   int err;
   G.cur_widthindex = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if (!get_tok())   
      return p_error();  //

   double diam = cnv_unit(token);

   if (curdata)
   {
      curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, voidtype, 1);  // fillflag , negative, closed
   }

   int res = loop_command(pt_lst, SIZ_PT_LST);

   if (curdata && polycnt == 1)
   {
      Point2 p = polyarray.ElementAt(0);
      Graph_Vertex(p.x - diam / 2, p.y, 1);
      Graph_Vertex(p.x + diam / 2, p.y, 1);
      Graph_Vertex(p.x - diam / 2, p.y, 0);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return res;
}


/****************************************************************************/
/* pcb_pcbprintsettings
*/
static int pcb_pcbprintsettings()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* pcb_drillsymsettings
*/
static int pcb_drillsymsettings()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* pcb_gerbersettings
*/
static int pcb_gerbersettings()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* pcb_ncdrillsettings
*/
static int pcb_ncdrillsettings()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* pcb_programstate
*/
static int pcb_programstate()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* compdef_compheader
*/
static int compdef_compheader()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* compdef_comppin
*/
static int compdef_comppin()
{
   fskip();

   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/* signallist_signal
*/
static int signallist_signal()
{
   if (!get_tok())
      return p_error();  // S%d

   CString signal_name = token;
   curnetdata = add_net(file, signal_name);
   G.name[0] = '\0';
   cur_powernet = 0;

   loop_command(signal_lst, SIZ_SIGNAL_LST);

   curnetdata->getNetList().updateNetName(curnetdata, G.name);
   CADIFNetl *c = new CADIFNetl;
   netlistarray.SetAtGrow(netlistcnt, c);
   c->net_name = G.name;
   c->signal_name = signal_name;
   netlistcnt++;

   return 1;
}

/****************************************************************************/
/* textstyle_justify
*/
static int textstyle_justify()
{
   if (!get_tok())
      return p_error();

   if (!STRCMPI(token, "JUST_START"))
      G.cur_textjust = GRTEXT_W_L;
   else if (!STRCMPI(token, "JUST_CENTER"))
      G.cur_textjust = GRTEXT_W_C;
   else if (!STRCMPI(token, "JUST_END"))
      G.cur_textjust = GRTEXT_W_R;
   else
      fprintf(ferr, "Unknown Text Justification [%s] at %ld\n", token, ifp_line);

   return 1;
}

/****************************************************************************/
/* font_textstyle
*/
static int font_textstyle()
{
   char  textfont[20];

   if (!get_tok())
      return p_error();

   strcpy(textfont, token);
   G.cur_rotation = 0;
   G.cur_mirror = 0;
   G.cur_textjust = 0;

   loop_command(textstyle_lst, SIZ_TEXTSTYLE_LST);

   CADIFTextStyle *c = new CADIFTextStyle;
   textstylearray.SetAtGrow(textstylecnt, c);
   c->fontname = textfont;
   c->height = G.cur_height;
   c->orientation = G.cur_rotation;
   c->mirror = 0;

   if (G.cur_mirror)
      c->mirror = MIRROR_FLIP | MIRROR_LAYERS;

   c->just = G.cur_textjust;
   textstylecnt++;

   return 1;
}

/****************************************************************************/
/* padshape
*/
static int padshape()
{
   if (!get_tok())
      return p_error();

   strcpy(G.name,token);
   G.cur_rotation = G.cur_left = G.cur_right = G.cur_width = G.cur_height = 0.0;

   int res = loop_command(padshape_lst, SIZ_PADSHAPE_LST);

   curblock = NULL;

   return res;
}

/****************************************************************************/
/* padsymbol
*/
static int padsymbol()
{
   if (!get_tok())
      return p_error();

   strcpy(G.name,token);
   CString tmp = token;
   curblock = Graph_Block_On(GBO_APPEND, G.name,file->getFileNumber(), 0);

   //if (is_attrib(curblock->getName(), FIDUCIAL_ATTR) != 0)
   //   curblock->setBlockType(blockTypeFiducial);
   //else
   curblock->setBlockType(blockTypePadshape);

   curdata = Graph_PolyStruct(Graph_Level("0", "", 1), 0L, 0); // this must be a floating layer.

   int res = loop_command(padsymbol_lst, SIZ_PADSYMBOL_LST);

   Graph_Block_Off();
   curblock = NULL;
   curdata = NULL;
   
   // here no check if the pad symbol was a straight, single 2 pin line.
   return res;
}

/****************************************************************************/
/* cadif_id
*/
static int cadif_id()
{
   if (!get_tok())
      return p_error();

   G.cur_id = atoi(token);

   return 1;
}

/****************************************************************************/
/* padcodedesc
*/
static int padcodedesc()
{
   // change padcode name to original
   //
   CADIFPadstack* padstack = padstackarray.getLast();

   if (padstack != NULL && strlen(padstack->padname) != 0)
   {
      fprintf(ferr, "PadCode [%s] already has a name [%s] at %ld\n",padstack->pcname, 
              padstack->padname, ifp_line);

      display_error++;

      return -2;
   }

   padstack->padname = G.name;
   loop_command(padcodedesc_lst, SIZ_PADCODEDESC_LST);

   return 1;
}

/****************************************************************************/
/* layerdesc
*/
static int layerdesc()
{
   cur_layerusage = "";
   loop_command(layerdesc_lst, SIZ_LAYERDESC_LST);

   return 1;
}

/****************************************************************************/
/* padassign
*/
static int padassign()
{

   if (!get_tok())
      return p_error();

   int res;
   CString ps = token;

   // here check is a padshape exist. It could be not translated
   if (Graph_Block_Exists(doc, ps, -1))
   {
      CADIFPadstack* padstack = padstackarray.getLast();

      if (padstack != NULL)
      {
         padstack->padassign += 1;
      }

      res =  loop_command(layerref_lst, SIZ_LAYERREF_LST);

      // not yet known here
      Graph_Block_Reference(ps, NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, G.cur_layerindex, TRUE);
   }
   else
   {
      fprintf(ferr, "PadAssign [%s] is not defined in PADSHAPE.\n", ps);
      display_error++;
      fskip();
   }

   return res;
}

/****************************************************************************/
/* attrib_visible
*/
static int attrib_visible()
{
   cur_attrib.visible = TRUE;

   return 1;
}

/******************************************************************************
* attrib_disp
*/
static int attrib_disp()
{
   if (!get_tok())
      return p_error();

   int fontIndex = get_fontptr(token);
   CADIFTextStyle *textStyle; 

   if (fontIndex > -1)
   {
      textStyle = textstylearray[fontIndex];
   }
   else
   {
      textStyle = textstylearray[fontIndex];
      fprintf(ferr, "Font [%s] not found in FontList at %ld\n", token, ifp_line);
      display_error++;
   }

   cur_attrib.height = textStyle->height;
   cur_attrib.mirror = textStyle->mirror,
   cur_attrib.rotation = textStyle->orientation;
   cur_attrib.width = textStyle->height * 6.0 / 8.0;

   int res = loop_command(attrib_disp_lst, SIZ_ATTRIB_DISP_LST);

   normalize_text(&cur_attrib.x, &cur_attrib.y, textStyle->just, cur_attrib.rotation, cur_attrib.mirror, 
                  cur_attrib.height, cur_attrib.height * TEXTCORRECT * cur_attrib.length);
   return res;
}

/****************************************************************************/
/* paddrill
*/
static int paddrill()
{
   int res = loop_command(drill_lst, SIZ_DRILL_LST);

   CADIFPadstack* padstack = padstackarray.getLast();

   if (padstack != NULL)
   {
      padstack->paddrill += 1;
   }

   return res;
}

/****************************************************************************/
/* padform_usrdef
*/
static int padform_usrdef()
{                                      
   if (!get_tok()) 
      return p_error();  // name

   CString name = token; // this name is not unique !

   if (!get_tok())
      return p_error();  // X length aka extent
   CString targetXsizeStr(token);

   if (!get_tok())
      return p_error();  // rotation

   double psymangle = atof(token);
   double angle;

   if (PADUSRDEF_ROTATION)
      angle = psymangle;
   else
      angle = 0;

   cur_padname = ""; // this is the SYMREF from a padsymbol PSYMxxx
   CString usr_name = "USR_";
   usr_name += G.name;

   curblock = Graph_Block_On(GBO_APPEND, usr_name, file->getFileNumber(), 0);
   curdata = Graph_PolyStruct(Graph_Level("0", "", 1), 0L, 0); // this must be a floating layer.
   G.cur_status = STAT_PADSTACK;

   int res = loop_command(padusr_lst, SIZ_PADUSR_LST);
   G.cur_status = STAT_UNKNOWN;
   Graph_Block_Off();

   // Case 2149 - User Def pad may need scaling
   double padScale = 1.0;
   if (PADUSRDEF_SCALE)
   {
      curblock->calculateBlockExtents(doc->getCamCadData());
      CExtent blockextent = curblock->getExtent();
      if (blockextent.isValid())
      {
         double xsize = blockextent.getXsize();
         double targetXsize = cnv_unit(targetXsizeStr);
         padScale = targetXsize / xsize;
         curblock->Scale(padScale);
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   // this angle must be evaluated. - here is a sample, which is correctly drawn.
   //
   //  (padShape PS26500
   //  (padUsrDef "SHAPE560" 431800 270
   //   (padNormal (padBack 0) (padSymRef PSYM560)
   //    (filledFigure
   //     (rectangle (pt -95250 -59690) (pt 336550 59690))
   //     (fillType SOLID)))))

   if (strlen(cur_padname))   // check if it was defined.
   {
      BlockStruct *b =  Graph_Block_Exists(doc, cur_padname, -1);

      if (b == NULL)
         cur_padname = "";
   }

   // Case 2149 - If User Def pad is scaled then can't use the "standard"
   // (aka cur_padname) definition, must use local (usr_name) definition.
   if (strlen(cur_padname) && padScale == 1.0)
   {
      // The PSYM definition (cur_padname) exists, and current pad is not scaled,
      // use the PSYM definition and delete the usr_name definition we just made.
      BlockStruct *b = Graph_Block_Exists(doc, usr_name, -1);

      if (b)   
         doc->RemoveBlock(b);

      Graph_Complex(file->getFileNumber(), G.name, 0, cur_padname, 0.0, 0.0, DegToRad(psymangle));
   }
   else
   {
      Graph_Complex(file->getFileNumber(), G.name, 0, usr_name, 0.0, 0.0, DegToRad(angle));
   }

   return res;
}

/****************************************************************************/
/* padform_annulr
*/
static int padform_annulr()
{
   if (!get_tok())   
      return p_error();
   G.cur_width  = cnv_unit(token);

   if (!get_tok())
      return p_error(); // inner diameter
   G.cur_height  = cnv_unit(token);

   // only ignore this if it should be ignored.
   if ((G.cur_width > 0 && G.cur_height > 0) || (IGNORE_EMPTYPADCODE_PINS == FALSE) )
   {
      int err;
      Graph_Aperture(G.name, T_DONUT, G.cur_width, G.cur_height, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
   }
   else
   {
      fprintf(ferr, "Padshape [%s] ignored - Padform Annulr has no size.\n", G.name);
      display_error++;
   }

   fskip();
   return 1;
}

/****************************************************************************/
/* padform_round
*/
static int padform_round()
{
   if (!get_tok())
      return p_error();
   G.cur_width  = cnv_unit(token);

   // only ignore this if it should be ignored.
   if ((G.cur_width > 0) || (IGNORE_EMPTYPADCODE_PINS == FALSE) )
   {
      int err;
      Graph_Aperture(G.name, T_ROUND, G.cur_width, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
   }
   else
   {
      fprintf(ferr, "Padshape [%s] ignored - Padform Round has no size.\n", G.name);
      display_error++;
   }

   fskip();
   return 1;
}

/****************************************************************************/
/* padform_octgon
*/
static int padform_octgon()
{
   if (!get_tok())
      return p_error();
   G.cur_width  = cnv_unit(token);

   // only ignore this if it should be ignored.
   if ((G.cur_width > 0) || (IGNORE_EMPTYPADCODE_PINS == FALSE) )
   {
      int err;
      Graph_Aperture(G.name, T_OCTAGON, G.cur_width, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
   }
   else
   {
      fprintf(ferr, "Padshape [%s] ignored - Padform Octgon has no size.\n", G.name);
      display_error++;
   }

   fskip();
   return 1;
}

/****************************************************************************/
/* padform_copper
*/
static int padform_copper()
{
   if (!get_tok())
      return p_error();

   G.cur_width  = cnv_unit(token);
   fprintf(ferr, "padform_copper not impl at %ld.\n", ifp_line);
   display_error++;

   // xxx
   // G.cur_type   = PADFORM_COPPER;
   fskip();

   return 1;
}

/****************************************************************************/
/* get_attribmap
*/
static const char *get_attribmap(const char *c)
{
   static CString tmp;

   for (int i=0; i<attribmapcnt; i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return   attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   tmp = c;
   tmp.MakeUpper();

   return tmp.GetBuffer(0);
}

/******************************************************************************
* cadif_attribute
*/
static int cadif_attribute()
{
   if (!get_tok())
      return p_error();

   CString key = get_attribmap(token);

   if (!get_tok())
      return p_error();

   CString val = token;
   cur_attrib.visible = 0;
   cur_attrib.x = cur_attrib.y = cur_attrib.rotation = 0;
   cur_attrib.cur_layerindex = 0;
   cur_attrib.height = attribute_height;
   cur_attrib.length = val.GetLength();

   loop_command(attrib_lst, SIZ_ATTRIB_LST);

   // not visible or L0
   if (cur_attrib.cur_layerindex == 0)
      cur_attrib.visible = FALSE;

   if (!curdata)
      return 1;
   
   //here need to update layer
   //curdata->layer = G.cur_layerindex;
   //curdata->poly.widthIndex = G.cur_widthindex;

   // only write the real refname
// if (val.CompareNoCase(curdata->getInsert()->refname) == 0 && key != ATT_REFNAME)   
//     return 1;

   if (key == ATT_TOLERANCE)
   {
      double ptol, mtol, tol;
      check_tolerance(val, &mtol, &ptol, &tol);

      if (tol > -1)
      {
         double tt = tol;
         doc->SetAttrib(&curdata->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL); //  
      }

      if (mtol > -1)
      {
         double tt = -mtol;
         doc->SetAttrib(&curdata->getAttributesRef(), doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL); //  
      }  

      if (ptol > -1)
      {
         double tt = ptol;
         doc->SetAttrib(&curdata->getAttributesRef(), doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL); //  
      }
   }
   else if (key == ATT_COMPHEIGHT)
   {
      CString v = "";
      while (strlen(val))
      {
         if (!isalpha(val.GetAt(0))) 
         {
            v += val.GetAt(0);
            val.Delete(0, 1);
         }
         else
            break;
      }

      if (COMPHEIGHT_UNIT > -1)
      {
         double h = atof(v) * Units_Factor(COMPHEIGHT_UNIT, page_units);  
         val.Format("%lg", h);
      }
      else if (val.CompareNoCase("mm") == 0)
      {
         double h = atof(v) * Units_Factor(UNIT_MM, page_units);  
         val.Format("%lg", h);
      }
      else
      {
         if (strlen(val))
         {
            fprintf(ferr, "Unknown Height Unit [%s] at %ld\n", val, ifp_line );
            display_error++;
         }

         double h = cnv_unit(v); 

         if (h > 0)  // do not write if zero.            
            doc->SetAttrib(&curdata->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); //            

         return 1;
      }
   }

	bool placeOnSpecifiedLayer = true;
   int layerIndex = 0;
	if (key.CompareNoCase("assembly_name") == 0 && curdata->getDataType() == dataTypeInsert)
	{	
      // When the attribute "assembly_name" encounter, check to see if the attribute is on the geometry, 
      // and if it is on the same layer or is on the mirroring layer.  If it is then put the attribute 
      // on the layer used in geometry
		BlockStruct* block = doc->getBlockAt(curdata->getInsert()->getBlockNumber());
		Attrib* attrib = is_attvalue(doc, block->getAttributesRef(), key, 0);
		if (attrib != NULL)
		{
			LayerStruct* layer = doc->getLayerAt(attrib->getLayerIndex());
			if (cur_attrib.cur_layerindex == layer->getLayerIndex() || 
				 cur_attrib.cur_layerindex == layer->getMirroredLayerIndex())
			{
				layerIndex = layer->getLayerIndex();
				placeOnSpecifiedLayer = false;
			}
		}
	}
	
	if (placeOnSpecifiedLayer)
	{
		layerIndex = cur_attrib.cur_layerindex;

		if (G.cur_bottom)
		{
			LayerStruct* layer = doc->getLayerAt(layerIndex);
			layerIndex = layer->getMirroredLayerIndex();
		}
	}

	doc->SetUnknownVisAttrib(&curdata->getAttributesRef(), key, val, cur_attrib.x,cur_attrib.y, 
                            DegToRad(cur_attrib.rotation), cur_attrib.height, cur_attrib.width, 1, 0, 
                            cur_attrib.visible, SA_OVERWRITE, 0L, layerIndex, 0, 0, 0);

   getCadifReader().setAttributeVisibility(curdata->attributes(),key);

   return 1;
}

/******************************************************************************
* part_attribute
*/
static int part_attribute()
{
   if (!get_tok())
      return p_error();

   CString key = get_attribmap(token);

   if (!get_tok())
      return p_error();

   CString val =token;
   cur_attrib.visible = 0;
   cur_attrib.x = cur_attrib.y = cur_attrib.rotation = 0;
   cur_attrib.cur_layerindex = 0;
   cur_attrib.height = attribute_height;
   loop_command(attrib_lst, SIZ_ATTRIB_LST);

   // not visible or L0
   if (cur_attrib.cur_layerindex == 0)
      cur_attrib.visible = FALSE;

   if (!curtype)
      return 1;

   if (key == ATT_TOLERANCE)
   {
      double   ptol, mtol, tol;
      check_tolerance(val,&mtol,&ptol,&tol);

      if (tol > -1)
      {
         double tt = tol;
         doc->SetAttrib(&curtype->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL); //
      }

      if (mtol > -1)
      {
         double tt = -mtol;
         doc->SetAttrib(&curtype->getAttributesRef(), doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL); //
      }   

      if (ptol > -1)
      {
         double tt = ptol;
         doc->SetAttrib(&curtype->getAttributesRef(), doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL); // x, y, rot, height
      }
   }
   else
   {
      doc->SetUnknownVisAttrib(&curtype->getAttributesRef(),key, val.GetBuffer(0), cur_attrib.x, cur_attrib.y, 
                               DegToRad(cur_attrib.rotation), cur_attrib.height, cur_attrib.width, 1, 0,
                               cur_attrib.visible, SA_OVERWRITE, 0L, cur_attrib.cur_layerindex, 0, 0, 0);

      getCadifReader().setAttributeVisibility(curtype->attributes(),key);
   }

   return 1;
}

/****************************************************************************/
/* signal_attribute
*/
static int signal_attribute()
{
   if (!get_tok())
      return p_error();

   CString key = get_attribmap(token);

   if (!get_tok())
      return p_error();

   CString val = token;
   cur_attrib.visible = 0;

   loop_command(attrib_lst, SIZ_ATTRIB_LST);

   if (curnetdata)
   {
      doc->SetUnknownVisAttrib(&curnetdata->getAttributesRef(), key, val.GetBuffer(0), cur_attrib.x, cur_attrib.y, 
                               DegToRad(cur_attrib.rotation), cur_attrib.height, cur_attrib.width, 1, 0,
                               cur_attrib.visible, SA_OVERWRITE, 0L, cur_attrib.cur_layerindex, 0, 0, 0);

      getCadifReader().setAttributeVisibility(curnetdata->attributes(),key);
   }

   return 1;
}

/****************************************************************************/
/* drillsize
*/
static int drillsize()
{
   if (!get_tok())
      return p_error();

   G.cur_width  = cnv_unit(token);

   if (curblock && G.cur_width > 0)
   {
      double d = G.cur_width;
      int drilllayernum = Graph_Level("DRILLHOLE", "", 0);
      int drillindex = get_drillindex(G.cur_width);   
      CString  drillname;
      drillname.Format("DRILL_%d",drillindex);
      // must be filenum 0, because apertures are global.
      Graph_Block_Reference(drillname, NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, drilllayernum, TRUE);
   }

   return 1;
}

/****************************************************************************/
/* drillplated
*/
static int drillplated()
{
/* a drill is plated unless it says not plated.
   if (curblock)
   {
      doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(DRILL_PLATED, 0),
            VT_NONE,
            NULL,
            0.0,0.0,0.0,attribute_height, 0.0,0,0, FALSE, SA_OVERWRITE, 0L,0,0); // x, y, rot, height
   }
*/
   return 1;
}


/****************************************************************************/
/* padform_square
*/
static int padform_square()
{
   if (!get_tok())
      return p_error();
   G.cur_width  = cnv_unit(token);

   // only ignore this if it should be ignored.
   if ((G.cur_width > 0) || (IGNORE_EMPTYPADCODE_PINS == FALSE) )
   {
      int err;
      Graph_Aperture(G.name, T_SQUARE, G.cur_width, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
   }
   else
   {
      fprintf(ferr, "Padshape [%s] ignored - Padform Square has no size.\n", G.name);
      display_error++;
   }

   fskip();
   return 1;
}

/****************************************************************************
* padform_oblong
*
*  oblong is rectangle with endcap.
*/
static int padform_oblong()
{
   if (!get_tok())  
      return p_error();  // width
   G.cur_width  = cnv_unit(token);
   if (!get_tok()) 
      return p_error();  // left
   G.cur_left   = cnv_unit(token);
   if (!get_tok())  
      return p_error();  // right
   G.cur_right  = cnv_unit(token);
   if (!get_tok())  
      return p_error();  // rotation
   G.cur_rotation = atof(token);

   if ((G.cur_width > 0) || (IGNORE_EMPTYPADCODE_PINS == FALSE) )
   {
      int err;
      double offx = (G.cur_right - G.cur_left)/2;
      Graph_Aperture(G.name, T_RECTANGLE, G.cur_left+G.cur_right, G.cur_width, offx, 0.0, DegToRad(G.cur_rotation), 0, BL_APERTURE, TRUE, &err);
   }
   else
   {
      fprintf(ferr, "Padshape [%s] ignored - Padform Oblong has no size.\n", G.name);
      display_error++;
   }

   fskip();
   return 1;
}

/****************************************************************************
* padform_finger
*
*  finger is rectangle with round.
*/
static int padform_finger()
{
   if (!get_tok())  
      return p_error();  // width
   G.cur_width  = cnv_unit(token);
   if (!get_tok())  
      return p_error();  // left
   G.cur_left   = cnv_unit(token);
   if (!get_tok())  
      return p_error();  // right
   G.cur_right  = cnv_unit(token);
   if (!get_tok())  
      return p_error();  // rotation
   G.cur_rotation = atof(token);

   // only ignore this if it should be ignored.
   if ((G.cur_width > 0) || (IGNORE_EMPTYPADCODE_PINS == FALSE) )
   {
      int err;
      double offx = (G.cur_right - G.cur_left)/2;
      Graph_Aperture(G.name, T_OBLONG, G.cur_width + G.cur_left+G.cur_right, G.cur_width, offx, 0.0, DegToRad(G.cur_rotation), 0, BL_APERTURE, TRUE, &err);
   }
   else
   {
      fprintf(ferr, "Padshape [%s] ignored - Padform Finger has no size.\n", G.name);
      display_error++;
   }

   fskip();
   return 1;
}

/****************************************************************************
* padform_bullet
*
*  finger is rectangle with round.
*/
static int padform_bullet()
{
   CString  usr_name;

   int err;
   G.cur_widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if (!get_tok())  
      return p_error();  // height of rectangle

   G.cur_width  = cnv_unit(token);

   if (!get_tok()) 
      return p_error();  // left

   G.cur_left   = cnv_unit(token);

   if (!get_tok())  
      return p_error();  // right

   G.cur_right  = cnv_unit(token);

   if (!get_tok()) 
      return p_error();  // rotation

   G.cur_rotation = atof(token);

   usr_name = "BULLET_";
   usr_name += G.name;

   curblock = Graph_Block_On(GBO_APPEND, usr_name, file->getFileNumber(), 0);
   curdata = Graph_PolyStruct(Graph_Level("0", "", 1), 0L, 0); // this must be a floating layer.
   curpoly = Graph_Poly(NULL,G.cur_widthindex, TRUE, 0, 1); // fillflag , negative, closed

   // -------\    ^
   // |     | \   w
   // |     |  |  i
   // |     | /   d
   // -------/    th
   //             v
   // <left right>


   // Kevin's email from 17-Jun-99
   // So, here is the deal with the Bullet pads.
   // I spoke to Zuken-Redac about this. The error is in the CADIF reader of Visula. 
   // In Cadstar the pin is in the geometric middle of the pad. In fact, in Cadstar you cannot put the pin 
   // anywhere else. When Visula reads the CADIF file in, however, it makes an error and takes the center 
   // of the rectangle as the center of the pad. 

   if (CADIF_FORMAT == CADIF_VISULA)
   {
      Graph_Vertex(-G.cur_left, -G.cur_width / 2, 0);
      Graph_Vertex(G.cur_right, -G.cur_width / 2, 1);
      Graph_Vertex(G.cur_right,  G.cur_width / 2, 0);
      Graph_Vertex(-G.cur_left,  G.cur_width / 2, 0);
      Graph_Vertex(-G.cur_left, -G.cur_width / 2, 0);
   }
   else
   {
      Graph_Vertex(-G.cur_left-G.cur_width/4, -G.cur_width / 2, 0);
      Graph_Vertex(G.cur_right-G.cur_width/4, -G.cur_width / 2, 1);
      Graph_Vertex(G.cur_right-G.cur_width/4,  G.cur_width / 2, 0);
      Graph_Vertex(-G.cur_left-G.cur_width/4,  G.cur_width / 2, 0);
      Graph_Vertex(-G.cur_left-G.cur_width/4, -G.cur_width / 2, 0);
   }

   Graph_Block_Off();
   Graph_Complex(file->getFileNumber(), G.name, 0, usr_name, 0.0, 0.0, DegToRad(G.cur_rotation));

   fskip();

   return 1;
}

/****************************************************************************/
/* padcode
*/
static int padcode()
{
   if (!get_tok()) 
      return p_error();

   strcpy(G.name,token);
   cur_padname = token;
   CADIFPadstack* padstack = padstackarray.getDefinedAt(G.name);

   curblock = Graph_Block_On(GBO_APPEND, G.name, file->getFileNumber(), 0);
   curblock->setBlockType(blockTypePadstack);
   int res = loop_command(padcode_lst, SIZ_PADCODE_LST);

   if (USE_PADCODE_NAME)
   {
      if (strlen(padstack->padname) != 0)
      {
         curblock->setOriginalName(curblock->getName());
         curblock->setName(padstack->padname);
      }
   }

   //if (is_attrib(curblock->getName(), FIDUCIAL_ATTR) != 0)
   //   curblock->setBlockType(blockTypeFiducial);
   
   Graph_Block_Off();

   curblock = NULL;

   return res;
}

/****************************************************************************/
/* technology_padcodelib
*/
static int technology_padcodelib()
{
   loop_command(padcodelib_lst, SIZ_PADCODELIB_LST);

   return 1;
}

/****************************************************************************/
/* technology_techdesc
*/
static int technology_techdesc()
{
   loop_command(techdesc_lst, SIZ_TECHDESC_LST);

   return 1;
}

/****************************************************************************/
/* technology_layerstack
*/
static int technology_layerstack()
{
   loop_command(layerstack_lst, SIZ_LAYERSTACK_LST);

   return 1;
}

/****************************************************************************/
/* technology_globallayerstack
*/
static int technology_globallayerstack()
{
   loop_command(globallayerstack_lst, SIZ_GLOBALLAYERSTACK_LST);

   return 1;
}

/****************************************************************************/
/* get_layertype_from_usage
*/
static int get_layertype_from_usage(int usage)
{
   if (usage == L_SILKSCREEN) 
      return   LAYTYPE_SILK_TOP;

   if (usage == L_DRILL_DWG)  
      return   LAYTYPE_DRILL; 

   if (usage == L_DRILL_ID)   
      return   LAYTYPE_DRILL;    

   if (usage == L_TOP_RESIST) 
      return   LAYTYPE_MASK_TOP; 

   if (usage == L_BOTTOM_RESIST) 
      return   LAYTYPE_MASK_BOTTOM;

   return 0;
}

/****************************************************************************/
/* get_layerusage
*/
static int get_layerusage(const char *lu)
{
   if (!STRCMPI(lu,"UNLAYERED")) 
      return L_UNLAYERED;

   if (!STRCMPI(lu,"RESERVED"))  
      return L_RESERVED;

   if (!STRCMPI(lu,"CLEARANCE")) 
      return L_CLEARANCE;

   if (!STRCMPI(lu,"ELECTRICAL"))   
      return L_ELECTRICAL;

   if (!STRCMPI(lu,"LAMINATE"))  
      return L_LAMINATE;

   if (!STRCMPI(lu,"SPLIT_PLANE"))
      return L_SPLIT_PLANE;

   if (!STRCMPI(lu,"SILKSCREEN"))
      return L_SILKSCREEN;

   if (!STRCMPI(lu,"DRILL_DWG")) 
      return L_DRILL_DWG;

   if (!STRCMPI(lu,"DRILL_ID"))  
      return L_DRILL_ID;

   if (!STRCMPI(lu,"TOP_RESIST"))
      return L_TOP_RESIST;

   if (!STRCMPI(lu,"BOTTOM_RESIST"))
      return L_BOTTOM_RESIST;

   if (!STRCMPI(lu,"DOCUMENT"))  
      return L_DOCUMENT;

   if (!STRCMPI(lu,"ASSY_DWG"))  
      return L_ASSY_DWG;

   if (!STRCMPI(lu,"PROFILING")) 
      return L_PROFILING;

   if (!STRCMPI(lu,"PLACEMENT")) 
      return L_PLACEMENT;

   if (!STRCMPI(lu,"NO_TRACKS")) 
      return L_NO_TRACKS;

   if (!STRCMPI(lu,"NO_VIAS"))   
      return L_NO_VIAS;

   if (!STRCMPI(lu,"POWER_PLANE"))
      return L_POWER_PLANE;

   if (!STRCMPI(lu,"PROHIBITED"))
      return L_PROHIBITED;

   if (!STRCMPI(lu,"RESISTOR"))  
      return 0;

   if (!STRCMPI(lu,"NO_DIEL"))   
      return 0;

   if (!STRCMPI(lu,"WIREBOND"))  
      return 0;

   if (!STRCMPI(lu,"XOVER_DIEL"))
      return 0;

   if (!STRCMPI(lu,"SHEET_DIEL"))
      return 0;

   if (!STRCMPI(lu,"COVERGLAZE"))
      return 0;

   if (!STRCMPI(lu,"THERM_BOX")) 
      return 0;

   //ErrorMessage(lu, "Debug Error: Unknown LayerUsage");

   return 0;
}

/****************************************************************************/
/* layerstack_layer
*/
static int layerstack_layer()
{
   if (!get_tok())  
      return p_error();

   CString tmp = token;          // L1.. L81 etc
   int lindex = get_layerptr(tmp);

   if (lindex < 0 || lindex > (MAX_LAYERS - 1))
   {
      fprintf(ferr, "Layerindex [%s] wrong\n", tmp);
      display_error++;
      return -1;
   }

   layerlist[lindex].name = tmp;
   layerlist[lindex].username = tmp;
   G.cur_layerindex = Graph_Level(tmp, "", 0);
   curlayer = doc->FindLayer(G.cur_layerindex);
   G.name[0] = '\0'; // user layer name
   ppsignalcnt = 0;
   G.cur_id = 0;
   cur_layerusage = "";

   int res = loop_command(layerstacklayer_lst, SIZ_LAYERSTACKLAYER_LST);
   layerlist[lindex].usage = get_layerusage(cur_layerusage);
   layerlist[lindex].id = G.cur_id;
   layerlist[lindex].ppsignalcnt = 0;

   if (strlen(G.name))
   {
      if (layerlist[lindex].usage == L_SPLIT_PLANE || layerlist[lindex].usage == L_POWER_PLANE ||
          layerlist[lindex].usage == L_ELECTRICAL)
      {
         // plane nets  (name "+3.3V;AGND_TX0")
         char  *lp, tmp[1000];

         strcpy(tmp, G.name);
         lp = strtok(tmp,";");

         while (lp)
         {
            int n = get_netnameptr(lp);

            if (n > -1)
            {
               if (layerlist[lindex].ppsignalcnt < 10)
               {
                  layerlist[lindex].ppsignal[layerlist[lindex].ppsignalcnt] = n+1;
                  layerlist[lindex].ppsignalcnt++;
               }
               else
               {
                  fprintf(ferr, "Too many powernetnames [%s]\n", G.name);
                  display_error++;
               }
            }

            lp = strtok(NULL, ";");
         }
      } // if ELECTRICAL, SPLIT_PLANE or POWER

      if (USE_LAYERNAME)
      {
         curlayer->setComment(curlayer->getName());
         curlayer->setName(G.name);
      }

      layerlist[lindex].username = G.name;
   }
   
   if (ppsignalcnt)
   {
      for (int i=0; i<ppsignalcnt; i++)
         layerlist[lindex].ppsignal[i] = ppsignal[i];

      layerlist[lindex].ppsignalcnt = ppsignalcnt;
   }

   if (layerlist[lindex].ppsignalcnt)
      layerlist[lindex].usage = L_POWER_PLANE;

   curlayer = NULL;

   return res;
}

/****************************************************************************/
/* globallayerstack_layer
*/
static int globallayerstack_layer()
{
   if (!get_tok())  
      return p_error();

   CString tmp = token;          // GL1.. GL81 etc
   int lindex = get_globallayerptr(tmp);

   if (lindex < 0 || lindex > (MAX_LAYERS - 1))
   {
      fprintf(ferr, "Layerindex [%s] wrong\n", tmp);
      display_error++;

      return -1;
   }

   globallayerlist[lindex].name = tmp;
   globallayerlist[lindex].username = tmp;
   curlayer = NULL; 
   G.name[0] = '\0'; // user layer name
   ppsignalcnt = 0;
   G.cur_id = 0;
   cur_layerref = "";
   cur_layerusage = "";

   int res = loop_command(layerstacklayer_lst, SIZ_LAYERSTACKLAYER_LST);

   if (strlen(cur_layerref))
      globallayerlist[lindex].layerref = get_layerptr(cur_layerref);

   globallayerlist[lindex].usage = get_layerusage(cur_layerusage);
   globallayerlist[lindex].id = G.cur_id;

   if (strlen(G.name))
      globallayerlist[lindex].username = G.name;

   curlayer = NULL;

   return res;
}

/****************************************************************************/
/* design_signallist
*/
static int design_signallist()
{
   loop_command(signallist_lst, SIZ_SIGNALLIST_LST);

   return 1;
}

/****************************************************************************/
/* design_textinfo
*/
static int design_textinfo()
{
   loop_command(textinfo_lst, SIZ_TEXTINFO_LST);

   return 1;
}

/****************************************************************************/
/* altname_exist
*/
static int altname_exist(const char *origname, const char *n)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;
   
      if (block->getName().Compare(n) == 0)
      {
         if (!CHANGEALT_PARTDETAIL_ALTNAME)
         {
                fprintf(ferr, "PackAlt [%s] Name [%s] already defined!\n", origname, n);
            display_error++;
         }

         return TRUE;
      }
   }

   return FALSE;
}

/****************************************************************************/
/* design_packalt
*/
static int design_packalt()
{
   if (!get_tok())  
      return p_error();

   cur_altname = token;          // ALT1 -> realname in G.name
   strcpy(G.name,"");
   cur_packref = "";
   G.cur_pincnt = 0; // pins can not be converted, if no NONPADSTACK is defined;
   curblock = Graph_Block_On(GBO_APPEND, cur_altname, file->getFileNumber(), 0);
   curblock->setBlockType(blockTypePcbComponent);

   //if (curblock->getName().Left(7).CompareNoCase("S025451") == 0)
   //{
   //   int iii = 3;
   //}

   int res = loop_command(packalt_lst, SIZ_PACKALT_LST);

   if (G.cur_pincnt == 0)
   {
      fprintf(ferr, "PackAlt [%s] has no pins.\n", cur_altname);
      display_error++;
      curblock->setBlockType(blockTypeGenericComponent);
   }

   int already_exist = FALSE;

   if (strlen(G.name))
   {
      if (USE_PACKALT_NAME )
         already_exist = altname_exist(curblock->getName(), G.name);

      if (USE_PACKALT_NAME && !already_exist)
      {
         // need to check here that a block of this name does not exist
         curblock->setOriginalName(curblock->getName());
         curblock->setName(G.name);
      }
      else
      {
         doc->SetUnknownAttrib(&curblock->getAttributesRef(), get_attribmap("CADIF_PACKALT_NAME"), G.name, SA_OVERWRITE, NULL);
      }

      // here output PACKAGE name
      doc->SetUnknownAttrib(&curblock->getAttributesRef(),get_attribmap("CADIF_PACKAGE"), cur_packref, SA_OVERWRITE, NULL);
      doc->SetUnknownAttrib(&curblock->getAttributesRef(),get_attribmap("CADIF_PACKAGE_NAME"), cur_libname, SA_OVERWRITE, NULL);
   }

   BlockStruct& currentBlock = *curblock;

   //getCadifReader().getPcbGeometryMap().addGeometry(*curblock);

   //if (is_attrib(curblock->getName(), FIDUCIAL_ATTR) != 0)
   //   curblock->setBlockType(blockTypeFiducial);
   //else if (is_attrib(curblock->getName(), TEST_ATTR) != 0)
   //   curblock->setBlockType(blockTypeTestPoint);

   Graph_Block_Off();
   curblock = NULL;
   CADIFPackAlt* packalt = packaltarray.add(cur_altname,G.name);

   //packaltarray.SetAtGrow(packaltcnt, c);
   //packaltcnt++;
   //c->name = cur_altname;
   packalt->pacname = cur_packref;
   //c->realname = G.name;
   packalt->already_exist = already_exist;
   packalt->used = FALSE;

   return res;
}

/****************************************************************************/
/* design_symbol
*/
static int design_symbol()
{
   if (!get_tok()) 
      return p_error();

   CString cur_symname = token;          
   curblock = Graph_Block_On(GBO_APPEND,cur_symname, file->getFileNumber(),0);
   int res = loop_command(symbol_lst,SIZ_SYMBOL_LST);                            
   Graph_Block_Off();
   curblock = NULL;

   return res;
}

/****************************************************************************/
/* partdesc
*/
static int partdesc()
{
   loop_command(partdesc_lst, SIZ_PARTDESC_LST);

   return 1;
}

/****************************************************************************/
/* packdesc
*/
static int packdesc()
{
   loop_command(packdesc_lst, SIZ_PACKDESC_LST);

   return 1;
}

/****************************************************************************/
/* design_package
*/
static int design_package()
{
   if (!get_tok())  
      return p_error();
   CString packageID = token;

   CString packageName;
   CCadifPackage* package = packageNameMap.AddPackage(token, packageName);

   loop_command(package_lst, SIZ_PACKAGE_LST);
   cur_libname = G.name;  // overwrite with (name xxx)

   package->SetName(cur_libname);

   return 1;
}

/****************************************************************************/
/* design_part
*/
static int design_part()
{
   if (!get_tok())  
      return p_error();

   char tmp[80];
   strcpy(tmp,token);
   cur_libname = ""; // this is updated in packRef
   cur_partdetailname = "";
   curtype = AddType(file, G.name);
   loop_command(part_lst, SIZ_PART_LST);
   //curtype->setName(G.name);
   curtype = NULL;

   // here update partlist
   // tmp = PRT1
   // cur_libname = PAC8
   CADIFPartName  *c = new CADIFPartName;
   partnamearray.SetAtGrow(partnamecnt,c);

   c->prtname = tmp;
   c->partname = G.name;
   c->partdetailname = cur_partdetailname;

   if (strlen(cur_libname))
   {
      if (packageNameMap.FindPackage(cur_libname) == NULL)
      {
         fprintf(ferr, "Package name [%s] not found in package list at %ld\n", cur_libname, ifp_line);
         display_error++;
      }
   }
   else
   {
      // there was no packref under part
      fprintf(ferr,"No packRef found under part [%s] at %ld\n", G.name, ifp_line);
      display_error++;
   }

   partnamecnt++;

   return 1;
}

/****************************************************************************/
/* cadif_comppin
*/
static int cadif_comppin()
{
   if (!get_tok()) 
      return p_error();

   loop_command(comppin_lst, SIZ_COMPPIN_LST);

   return 1;
}

/****************************************************************************/
/* design_component
*/
static int design_component()
{
   if (!get_tok()) 
      return p_error();

   cur_compname = token;
   curdata = NULL;      // comp is opened in compdefn, but closed here.
   cur_derived = FALSE; // this is used for padexceptions
   cur_libname = "";    // this is partref

   // case 1500
   // Set "current" position to default position. This may be overridden by
   // "loop_command" encountering a "location". If not, then the default position
   // will be used for the insert.
   G.cur_pos_x = DefCompPosn.x;
   G.cur_pos_y = DefCompPosn.y;

   int res = loop_command(component_lst,SIZ_COMPONENT_LST);

   // cur_libname
   if (curdata)
   {
      // block may have changed on local pin redefinition
      BlockStruct *componentGeometry = doc->Find_Block_by_Num(curdata->getInsert()->getBlockNumber());
   
      CString devicename = cur_libname;

      // now link this part to a typelist
      TypeStruct *t = AddType(file, cur_libname);
      if (t->getBlockNumber() == -1)
      {
         t->setBlockNumber( curdata->getInsert()->getBlockNumber());
      }
      else if (t->getBlockNumber() != curdata->getInsert()->getBlockNumber())
      {
         // Base name on block that THIS device inserts
         BlockStruct *b1 = doc->Find_Block_by_Num(curdata->getInsert()->getBlockNumber());
         devicename.Format("%s_%s", cur_libname, b1->getName());
         TypeStruct *t_new = AddType(file, devicename);

         // Old code says if the block numbers are not the same then make them
         // the same. Result is they are always the same. So don't bother checking,
         // just make them the same. 
         t_new->setBlockNumber( curdata->getInsert()->getBlockNumber());

         // Copy attributes to new device
         doc->CopyAttribs(&t_new->getAttributesRef(), t->getAttributesRef());
      }

      doc->SetUnknownAttrib(&curdata->getAttributesRef(), ATT_TYPELISTLINK, devicename, SA_OVERWRITE, NULL);

      CADIFCompInst  *c = new CADIFCompInst(*curdata);
      compinstarray.SetAtGrow(compinstcnt++, c);
      c->cname = cur_compname;
      c->altname = cur_altname;
      c->compname = G.name;
      c->type = "";
      c->value = "";

      if (curdata)
      {
         curdata->getInsert()->setOriginX(G.cur_pos_x);
         curdata->getInsert()->setOriginY(G.cur_pos_y);

         if (G.cur_mirror)
            G.cur_rotation = 360 - G.cur_rotation;

         G.cur_rotation = normalizeDegrees(G.cur_rotation);

/* 
  (component C2 (name "R4401")
   (compDefn (partRef PRT2) (packAltRef ALT2))
   (location
    (orientation 180)
    (side BOTTOM)  <=== here is no mirror !!!
    (position (pt 9675000 3950000)))
*/
/*
  (component C159 (name "E1")
   (compDefn (partRef PRT6) (packAltRef ALT61))
   (location
    (side BOTTOM)
    (position (pt 1533100 18342150))
    (mirrored))  <== here it is !!!
   (box (pt -293440 16397652) (pt 1659600 18560816))
*/
         if (cadif_version.maj >= 7)   // it seems that on Version 7 the mirror is gone and bottom makes it happen
         {
            if (G.cur_bottom && !G.cur_mirror)
               G.cur_mirror = MIRROR_LAYERS;
         }

         curdata->getInsert()->setAngle(DegToRad(G.cur_rotation));
         curdata->getInsert()->setMirrorFlags(G.cur_mirror);
         curdata->getInsert()->setPlacedBottom(G.cur_bottom);

         // check block, because if it has no pins, it will not be defined as a pcbcomponent.
         componentGeometry = doc->Find_Block_by_Num(curdata->getInsert()->getBlockNumber());

         if (componentGeometry->getBlockType() == blockTypeFiducial)
         {
            curdata->getInsert()->setInsertType(insertTypeFiducial);
         }
         else if (componentGeometry->getBlockType() == blockTypeTestPoint)
         {   
            curdata->getInsert()->setInsertType(insertTypeTestPoint);

            doc->SetUnknownAttrib(&curdata->getAttributesRef(),ATT_TEST,"",
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else if (componentGeometry->getBlockType() == blockTypePcbComponent)
         {
            curdata->getInsert()->setInsertType(insertTypePcbComponent);
         }
      }
      else
      {
         CString tmp;
         tmp.Format("Error: Component [%s] placement failure at %ld", G.name, ifp_line);
         fprintf(ferr, "%s\n", tmp);
         display_error++;
      }
   } // curdata == NULL
   else
   {
      CString tmp;
      tmp.Format("Error: Component [%s] placement failure at %ld", G.name, ifp_line);
      fprintf(ferr,"%s\n", tmp);
      display_error++;
   }

   curdata = NULL;
   polyarray.RemoveAll();
   polycnt = 0;

   return res;
}

static int defcompposn_pt()
{
   if (!get_tok())         // "X"
      return p_error();
   double defX = cnv_unit(token);

   if (!get_tok())         // "Y"
      return p_error();
   double defY = cnv_unit(token);

   DefCompPosn.x = defX;
   DefCompPosn.y = defY;

   return 1;
}

/****************************************************************************/
/* design_defcompposn
 * For case 1500
*/
int design_defcompposn()
{
   loop_command(defcompposn_lst, SIZ_DEFCOMPPOSN_LST);

   return 1;
}

/****************************************************************************/
/* design_net
*/
static int design_net()
{
   if (!get_tok()) 
      return p_error();

   char  tmp[80];
   strcpy(tmp,token);

   if (loop_command(net_lst,SIZ_NET_LST) < 0) 
      return -1;

   // cur_netname is the net
   int nptr = get_netnameptr(cur_netname);

   if (nptr > -1)
   {
#ifdef WOLF
         int res = L_NetAttr(cur_netname,0L, netlist[nptr].routcode, netlist[nptr].neckcode, 0,0L)

         if (res != OK)
         {
            fprintf(ferr, "Error %d in L_NetAttr\n", res);
         }
#endif
   }
   return 1;
}

/****************************************************************************/
/* design_keepout
*/
static int design_keepout()
{
   if (!get_tok()) 
      return p_error();

   CString tmp = token;
   loop_command(keepout_lst, SIZ_KEEPOUT_LST);
   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/* design_anno
*/
static int design_anno()
{
   if (!get_tok())
      return p_error();

   CString  tmp = token;  

   // do not use G.name, because an area can have
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = 0;
   curdata = Graph_PolyStruct(0, 0L, 0);
   loop_command(anno_lst, SIZ_ANNO_LST);

   if (curdata)
   {
      // here need to update layer
      curdata->setLayerIndex(G.cur_layerindex);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0),
                     VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
   }

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/* fileinfo_application
*/
static int fileinfo_application()
{

   if (!get_tok())
      return p_error();
  
   if (STRCMPI(token, "PCB")) // if not PCB
   {
      CString  tmp;
      tmp.Format("Application [%s] not implemented.\nCheck layerstacking and layertyping!", token);
      ErrorMessage(tmp, "CADIF File Format Warning");
   }

   return 1;
}

/****************************************************************************/
/* ppsignal_exist
*/
static int ppsignal_exist(int layerindex, int netindex)  // netindex is plus 1
{
   for (int i=0; i<layerlist[layerindex].ppsignalcnt; i++)
   {
      if (layerlist[layerindex].ppsignal[i] == netindex)
         return TRUE;
   } 

   return FALSE;
}

/****************************************************************************/
/* design_plane
*/
static int design_plane()
{
   if (!get_tok())  
      return p_error();

   CString tmp = token;  

   // do not use G.name, because an area can have
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = 0;
   cur_netname = "";
   curdata = Graph_PolyStruct(0, 0L, 0);
   loop_command(copper_lst, SIZ_COPPER_LST);

   if (curdata)
   {
      // here need to update layer
      curdata->setLayerIndex(G.cur_layerindex);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0), VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL);

      if (strlen(cur_netname))
      {
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
      
         // if the layer is a powerplane or split plane, remember the layer 
         int ptr = get_netnameptr(cur_netname);

         if (ptr > -1)
         {
            LayerStruct *lp = doc->getLayerArray()[G.cur_layerindex];
            int   l;

            if (USE_LAYERNAME)
               l = get_layerptr(lp->getComment());
            else
               l = get_layerptr(lp->getName());

            if (!ppsignal_exist(l, ptr+1))
            {
               if (layerlist[l].ppsignalcnt < 10)
               {
                  layerlist[l].ppsignal[layerlist[l].ppsignalcnt] = ptr+1;
                  layerlist[l].ppsignalcnt++;
               }
               else
               {
                  fprintf(ferr, "Too many ppsignals [%d]\n", ppsignalcnt);
                  display_error++;
               }
            }
         }     
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/* design_cophatch
*/
static int design_cophatch()
{
   if (!get_tok())  
      return p_error();

   CString  tmp = token;  
   // do not use G.name, because an area can have
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = FIG_FILLEDFIGURE;
   cur_netname = "";
   curdata = Graph_PolyStruct(0, 0L, 0);
   loop_command(cophatch_lst, SIZ_COPHATCH_LST);

   // cur_netname is the attached signal
   if (curdata)
   {
      curdata->setLayerIndex(G.cur_layerindex);

      if (strlen(cur_netname))
      {
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                        VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
      }

      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0),
                     VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
   }

   curdata = NULL;

   return 1;
}

/****************************************************************************/
/* design_copper
*/
static int design_copper()
{
   if (!get_tok()) 
      return p_error();

   CString  tmp = token;  

   // do not use G.name, because an area can have
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = FIG_FILLEDFIGURE;
   cur_netname = "";
   curdata = Graph_PolyStruct(0, 0L, 0);
   loop_command(copper_lst,SIZ_COPPER_LST);

   if (curdata)
   {
      // here need to update layer
      curdata->setLayerIndex(G.cur_layerindex);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 1), VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL);

      if (strlen(cur_netname))
      {
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
         curdata->setGraphicClass(GR_CLASS_ETCH);
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/* design_annohatch
*/
static int design_annohatch()
{
   char  tmp[80];
   sprintf(tmp,"HATCH_%ld", ifp_line);
   G.cur_filltype = 0;

   loop_command(annohatch_lst ,SIZ_ANNOHATCH_LST);

   return 1;
}

/****************************************************************************/
/* design_annosymbol
*/
static int design_annosymbol()
{
   if (!get_tok())  
      return p_error();

   CString refname = token;  // do not use G.name, because an area can have

   G.cur_rotation = 0;
   G.cur_layerindex = -1;  // floating layer
   arc.bulge = 1; // this is the scale for a anno
   G.cur_mirror = 0;
   loop_command(annosymbol_lst, SIZ_ANNOSYMBOL_LST);
   
   if (G.cur_mirror) // symbol mirrors do not change layers ????
   {
      G.cur_mirror = MIRROR_FLIP;
      G.cur_rotation = 360 - G.cur_rotation;
   }

   while (G.cur_rotation < 0)
      G.cur_rotation += 360;
   while (G.cur_rotation >=360)
      G.cur_rotation -= 360;

   DataStruct *d = Graph_Block_Reference(cur_libname, refname, file->getFileNumber(), G.cur_pos_x, G.cur_pos_y,
                     DegToRad(G.cur_rotation), G.cur_mirror , arc.bulge, G.cur_layerindex, TRUE);
   return 1;
}

/****************************************************************************/
/* design_constr
*/
static int design_constr()
{
   polyarray.RemoveAll();
   polycnt = 0;
   loop_command(anno_lst, SIZ_ANNO_LST);
   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int design_area()
{
   if (!get_tok())  
      return p_error();

   CString  tmp = token;   

   // do not use G.name, because an area can have
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = 0;
   G.cur_width = 0.0;
   G.cur_layerindex = Graph_Level("Board", "", 0);
   curdata = Graph_PolyStruct(0, 0L, 0);
   loop_command(area_lst, SIZ_AREA_LST);
   
   if (curdata)
   {
      // here need to update layer
      curdata->setLayerIndex(G.cur_layerindex);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0), VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL);
   }

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/* design_bareboard
*/
static int design_bareboard()
{
   if (!get_tok()) 
      return p_error();

   cur_bareboard = token; 

   return 1;
}

/****************************************************************************/
/* symfig
*/
static int symfig()
{   
   if (!get_tok()) 
      return p_error();

   CString tmp = token;
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = 0;
   G.cur_width = 0.0;
   G.cur_layerindex = 0;
   curdata = Graph_PolyStruct(0, 0L, 0);
   loop_command(symfig_lst, SIZ_SYMFIG_LST);
   
   if (curdata)
      // here need to update layer
      curdata->setLayerIndex(G.cur_layerindex);

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   return 1;
}

/******************************************************************************
* symLabel
*/
static int symLabel()
{
   if (!get_tok()) 
      return p_error();
   if (!get_tok()) 
      return p_error();

   int fontIndex = get_fontptr(token);
   CADIFTextStyle *textStyle; 

   if (fontIndex > -1)
   {
      textStyle = textstylearray[fontIndex];
   }
   else
   {
      textStyle = textstylearray[fontIndex];
      fprintf(ferr, "Font [%s] not found in FontList at %ld\n", token, ifp_line);
      display_error++;
   }

   int res = loop_command(symLabel_lst, SIZ_SYMLABEL_LST);

   BOOL visible = FALSE;
   double x =  G.cur_pos_x;
   double y = G.cur_pos_y;
   double height = textStyle->height;
   double rotation = textStyle->orientation;
   double width = textStyle->height * 6.0 / 8.0;
   int layerIndex = G.cur_layerindex;

   CString val;

   if ((curblock != NULL) && ((READSYMLABEL == true) || ((READSYMLABEL == false) && (STRCMPI(cur_text,"assembly_name") == false))))
   {
	   doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), cur_text, val, x, y, DegToRad(rotation), height, width,
            TRUE, 0, visible, attributeUpdateOverwrite, 0L, layerIndex, FALSE, 0, 0);
   }

   return res;
}

/****************************************************************************/
/* insertheight
*/
static int insertheight()
{
   if (!get_tok())  
      return p_error();

   double h = cnv_unit(token);

   if (h > 0)  // do not write if zero.
   {
      doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_COMPHEIGHT, 0), 
                     VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); // x, y, rot, heigh
   }

   return 1;
} 
//Last restruct

/******************************************************************************
* do_text
*/
static int do_text(const char *fname, double x, double y)
{
   int      fptr,mirror;
   double   rot, rotation,width,height;
   char     t[MAX_LINE+1];

   // here write text.
   if ((fptr = get_fontptr(fname)) > -1)
   {
      height = textstylearray[fptr]->height;
      mirror = textstylearray[fptr]->mirror,
      rot = textstylearray[fptr]->orientation;
      width  = 0.01;
   }
   else
   {
      height = textstylearray[0]->height;
      mirror = textstylearray[0]->mirror,
      rot = textstylearray[0]->orientation;
      width  = 0.01;

      fprintf(ferr, "Font [%s] not found in FontList at %ld\n", token, ifp_line);
      display_error++;
   }

   while (get_nexttextline(cur_text, t, MAX_LINE))
   {
      rotation = rot;

      // just clean end of text blanks.
      while (strlen(t) && isspace(t[strlen(t)-1]))
         t[strlen(t)-1] = '\0';

      double jx = x, jy = y;
      // normalize      
      normalize_text(&jx,&jy,textstylearray[fptr]->just,
                  rotation,
                  mirror,
                  height,
                  height*TEXTCORRECT * strlen(t));

      if (strlen(t))
      {
         // yes, this is correct !!!
         if (mirror && !textstylearray[fptr]->just)
            rotation = 360 - rotation;

         CString temp = t;
         int count = temp.Replace("\n", " ");  // for multi line, we need to adjust y location

         if ((textstylearray[fptr]->just & GRTEXT_W_L) == GRTEXT_W_L || (textstylearray[fptr]->just & GRTEXT_W_R) == GRTEXT_W_R)
            jy = jy - height * count;
         else if ((textstylearray[fptr]->just & GRTEXT_W_C) == GRTEXT_W_C)
            jy = jy - height * count;

         DataStruct *d = Graph_Text(G.cur_layerindex,t,x,jy,height*TEXTHEIGHT,height*TEXTHEIGHT*TEXTCORRECT,
               DegToRad(rotation),
               0L, FALSE, 
               mirror,  
               0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg

         doc->SetUnknownAttrib(&d->getAttributesRef(),"TS",fname,SA_OVERWRITE, NULL); // x, y, rot, height
      }

      x += sin(DegToRad(rotation))*height*LINESPACING;
      y -= cos(DegToRad(rotation))*height*LINESPACING;
   }

   return 1;
}

/******************************************************************************
* design_text
*/
static int design_text()
{
   CString  fname;

   if (!get_tok())
      return p_error();

   fname = token;

   cur_text[0] = '\0';
   polyarray.RemoveAll();
   polycnt = 0;
   loop_command(text_lst,SIZ_TEXT_LST);

   do_text(fname, G.cur_pos_x, G.cur_pos_y);

   polyarray.RemoveAll();
   polycnt = 0; // need to reset.

   return 1;
}

/****************************************************************************/
/*
*/
static int symtext()
{
   CString  fname;

   if (!get_tok())  
      return p_error();  // SD ???

   if (!get_tok())  
      return p_error();  // TS text size

   fname = token;

   cur_text[0] = '\0';
   polyarray.RemoveAll();
   polycnt = 0;
   loop_command(symtex_lst,SIZ_SYMTEX_LST);

   do_text(fname, G.cur_pos_x, G.cur_pos_y);

   polyarray.RemoveAll();
   polycnt = 0; // need to reset.

   return 1;
}

/****************************************************************************/
/*
*/
static int signalvia()
{
   CString  name;
   DataStruct *d;

   if (!get_tok())  
      return p_error();

   name = token;

   // must be 
   LayerRange1 = TopElecLnum;
   LayerRange2 = BotElecLnum;

   if (loop_command(via_lst,SIZ_VIA_LST) < 0)
      return -1;

   // via layer range.
   if (LayerRange1 == TopElecLnum && LayerRange2 == BotElecLnum)
   {
      d = Graph_Block_Reference(cur_padname, NULL, file->getFileNumber(), G.cur_pos_x,G.cur_pos_y, 0.0, 0 , 1.0, -1, TRUE);
   }
   else
   {
      CString  via_padname;

      via_padname.Format("%s_(%s_%s)", cur_padname, LayerRange1, LayerRange2);

      if (Graph_Block_Exists(doc, via_padname, -1) == NULL)
      {
         BlockStruct *b = Graph_Block_Exists(doc, cur_padname, -1);
         BlockStruct *b1 = Graph_Block_On(GBO_APPEND,via_padname,file->getFileNumber(),0);
         b1->setBlockType(blockTypePadstack);
         Create_LayerrangePADSTACKData(&(b->getDataList()), LayerRange1, LayerRange2);
         //Graph_Block_Copy(b, 0, 0, 0, 0, 1, -1);

         Graph_Block_Off();

         doc->SetAttrib(&b1->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE),
            VT_NONE,
            NULL,
            SA_OVERWRITE, NULL); // x, y, rot, height

      }

      d = Graph_Block_Reference(via_padname, NULL, file->getFileNumber(), G.cur_pos_x,G.cur_pos_y, 0.0, 0 , 1.0, -1, TRUE);
/*
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_VIALAYER, TRUE),
            VT_STRING,
            vialayer.GetBuffer(0),
            0.0,0.0,0.0,attribute_height, 0.0,0,0, FALSE, SA_APPEND, 0L,0,0); // x, y, rot, height
*/
   }

   d->getInsert()->setInsertType(insertTypeVia);

   if (strlen(cur_netname))
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
   }

   polyarray.RemoveAll();
   polycnt = 0;   

   return 1;
}

/****************************************************************************/
/*
*/
static int signaltestpt()
{
   CString  tpname;

   if (!get_tok()) 
      return p_error();

   tpname.Format("%s(%s)", token, cur_netname);

   LayerRange1 = "";
   LayerRange2 = "";
   cur_testside = "";

   if (loop_command(testpt_lst,SIZ_TESTPT_LST) < 0)
      return -1;

   DataStruct *d = Graph_Block_Reference(cur_padname, tpname, file->getFileNumber(), G.cur_pos_x,G.cur_pos_y,
                        0.0, 0 , 1.0, -1, TRUE);

   d->getInsert()->setInsertType(insertTypeVia);

   if (strlen(cur_netname))
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
   }

   polyarray.RemoveAll();
   polycnt = 0;   

   int   testaccessindex = 0;
   /*
      0 = "ALL",
      1 = "BOTTOM",
      2 = "TOP",
      3 = "NONE",
   */

   if (cur_testside.CompareNoCase("TST_BOTTOM") == 0)
      testaccessindex = ATT_TEST_ACCESS_BOT;
   else if (cur_testside.CompareNoCase("TST_TOP") == 0)
      testaccessindex = ATT_TEST_ACCESS_TOP;
   else
   {
      fprintf(ferr, "Unknown TestSide [%s]\n", cur_testside);
      display_error++;
   }

   doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TEST, TRUE),
      VT_STRING, testaccesslayers[testaccessindex], SA_OVERWRITE, NULL); //  

   return 1;
}

/****************************************************************************/
/*
*/
static int signaljuncpt()
{
#ifdef DISPLAY_JUNCTIONPOINT
   CString  jname;

   if (!get_tok()) 
      return p_error();

   jname = token;

   if (loop_command(testpt_lst,SIZ_TESTPT_LST) < 0)
      return -1;

   int err;
   Graph_Aperture("JUNCTION", T_TARGET, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   DataStruct *d = Graph_Block_Reference("JUNCTION", jname, file->getFileNumber(), G.cur_pos_x,G.cur_pos_y,
                        0.0, 0 , 1.0, Graph_Level("JUNC","",0), TRUE);
   doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0),
            VT_STRING,
            jname.GetBuffer(0),
            0.0,0.0,0.0,0.0, 0.0,0,0, FALSE, SA_OVERWRITE, 0L,0,0); // x, y, rot, height

   polyarray.RemoveAll();
   polycnt = 0;   
#else
   fskip();
#endif

   return 1;
}

/****************************************************************************/
/*
*/
static int signalroute()
{
   // a name, and this is not translated.
   G.cur_type = 0;
   G.cur_filltype = 0;

   curdata = Graph_PolyStruct(0,0L,0);

   loop_command(route_lst,SIZ_ROUTE_LST);

   if (curdata)
   {
      // here need to update layer
      curdata->setLayerIndex(G.cur_layerindex);
      // curdata->poly.widthIndex = G.cur_widthindex;
      curdata->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); //  

   }

   polyarray.RemoveAll();
   polycnt = 0;
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int cadif_pathfigure()
{
   G.cur_figtype = FIG_PATHFIGURE;
   G.cur_filltype = 0;  // pathfigure does not fill

   int err;
   G.cur_widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   curpoly = NULL;

   int   res = loop_command(pathfigure_lst,SIZ_PATHFIGURE_LST);

   if (curpoly)
      curpoly->setWidthIndex(G.cur_widthindex);

   return res;
}

/****************************************************************************/
/*
*/
static int cadif_filledfigure()
{
   int   res;

   G.cur_filltype = FIG_FILLEDFIGURE;
   int err;
   G.cur_widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   
   if (curdata && curdata->getGraphicClass() == GR_CLASS_BOARD_GEOM)
   {
      G.cur_filltype = FIG_BOARD_AREA;
   }

   curpoly = NULL;

   res = loop_command(pathfigure_lst,SIZ_PATHFIGURE_LST);

   if (G.cur_filltype == FILLTYPE_CLEAR)
   {
      if (G.cur_status != STAT_PADSTACK)  // do not use clear in padstacks.
      {
         if (curpoly)   
            curpoly->setFilled(false);
      }
   }

   curpoly = NULL;

   return res;
}

/****************************************************************************/
/*
*/
static int cadif_boundary()
{
   int   typ = FIG_FILLEDFIGURE;

   // G.cur_filltype this is set in annohatch or coprhatch
   // it seems annohatch is always hollow, copper is filled.

   G.cur_width = 0.0;
   polyarray.RemoveAll();
   polycnt = 0;

   loop_command(pathfigure_lst,SIZ_PATHFIGURE_LST);

   // here write path
   if (G.cur_filltype == FILLTYPE_CLEAR)
      typ = FIG_CLEARFIGURE;

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int layervisible()
{
   //curlayer->show = TRUE;  <-- always visible
   return 1;
}

/****************************************************************************/
/*
*/
static int layerusage()
{   
   if (!get_tok())
      return p_error();

   cur_layerusage = token;

   // global layer has this switched off, because a layer should not be registered -> it may change
   // it's name with layer ref.
   if (curlayer == NULL)
      return 1;

   if (!STRICMP(token,"ELECTRICAL"))
   {
      int g = ++G.electrical_stackup;
      curlayer->setLayerType(LAYTYPE_SIGNAL_INNER);  // top and solder is done later
      curlayer->setElectricalStackNumber(g);
   }
   else if (!STRICMP(token,"SPLIT_PLANE"))
   {
      int g = ++G.electrical_stackup;
      curlayer->setLayerType(LAYTYPE_SPLITPLANE);
      curlayer->setElectricalStackNumber(g);
   }
   else if (!STRICMP(token,"POWER_PLANE"))
   {
      int g = ++G.electrical_stackup;
      curlayer->setLayerType(LAYTYPE_POWERNEG);
      curlayer->setElectricalStackNumber(g);
   }
   else if (!STRICMP(token,"SILKSCREEN"))
   {
      curlayer->setLayerType(LAYTYPE_SILK_TOP);
   }
   else if (!STRICMP(token,"PLACEMENT"))
   {
      curlayer->setLayerType(LAYTYPE_COMPONENTOUTLINE);
   }
   else if (!STRICMP(token,"DRILL_DWG"))
   {
      curlayer->setLayerType(LAYTYPE_DRILL);
   }
   else if (!STRICMP(token,"DRILL_ID"))
   {
      curlayer->setLayerType(LAYTYPE_DRILL);
   }
   else if (!STRICMP(token,"TOP_RESIST"))
   {
      curlayer->setLayerType(LAYTYPE_MASK_TOP);
   }
   else if (!STRICMP(token,"BOTTOM_RESIST"))
   {
      curlayer->setLayerType(LAYTYPE_MASK_BOTTOM);
   }
   else if (!STRICMP(token,"PROFILING"))
   {
      curlayer->setLayerType(LAYTYPE_ALL);
   }
   else if (!STRICMP(token,"LAMINATE"))
   {
      curlayer->setLayerType(LAYTYPE_DIALECTRIC);
   }
   else
   {
      int r = doc->RegisterKeyWord(token, 0, VT_NONE);
      doc->SetAttrib(&curlayer->getAttributesRef(),r,
         VT_NONE, NULL, SA_OVERWRITE, NULL); //
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int layerppsignal()
{
   int      p;
   DbFlag   flg = 0;

   if (!get_tok())  
      return p_error();

   // empty      (ppSignal "")
   if (strlen(token) == 0) 
      return 1;

   p = get_netnameptr(token);

   if (p < 0)
   {
      fprintf(ferr,"Can not find ppsignal [%s] in netlist at %ld\n", token, ifp_line);
      display_error++;
   }
   else
   {
      if (ppsignalcnt < 10)
      {
         ppsignal[ppsignalcnt] = p+1;
         ppsignalcnt++;
      }
      else
      {
         fprintf(ferr,"Too many ppsignals [%d]\n", ppsignalcnt);
         display_error++;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static void update_packaltused(const char *p)
{
   CADIFPackAlt* packalt = packaltarray.getPackalt(p);

   if (packalt != NULL)
   {
      packalt->used = true;
   }

   //for (int i=0;i<packaltcnt;i++)
   //{
   //   if (!packaltarray[i]->name.Compare(p))
   //   {
   //      packaltarray[i]->used = TRUE;
   //      return i;
   //   }
   //}

   //return -1;
}

/****************************************************************************/
/*
*/
static int delete_unused_packalt()
{
   //for (int i=0;i<packaltcnt;i++)
   for (POSITION pos = packaltarray.GetStartPosition();pos != NULL;)
   {
      CADIFPackAlt* packalt = packaltarray.GetNextAssoc(pos);

      if (!packalt->used)
      {
         // here 
         BlockStruct *b = Graph_Block_Exists(doc,packalt->getName(), -1);
         if (b)   doc->RemoveBlock(b);
      }
   }

   return 0;
}

/****************************************************************************/
/*
*/
static int compdefn()
{
   int      pPtr, res;
   CString  geomname;

   // packaltref
   cur_libname = ""; // this is partref
   cur_altname = ""; // this is packaltref

   res = loop_command(compdefn_lst,SIZ_COMPDEFN_LST);

   if (strlen(cur_libname) == 0)
   {
      // partRef is missing !
      fprintf(ferr, "Syntax Error: partRef is missing for component [%s] at %ld\n", cur_compname, ifp_line);
      display_error++;

      return 1;
   }


   // typelink is the prtref
   if ((pPtr = get_partnameptr(cur_libname)) < 0)
   {
      fprintf(ferr, "partRef [%s] not defined for component [%s] at %ld\n", cur_libname, cur_compname, ifp_line);
      display_error++;

      return 1;
   }

// CADIFPartName *p4 = partnamearray[pPtr];

   // here not make a better name 
   // Part_detailname|Alt_name
   geomname = cur_altname;

   if (CHANGEALT_PARTDETAIL_ALTNAME)
   {
      CString  newgeom;
      CADIFPartName *pName = partnamearray[pPtr];
      newgeom = "";

      if (strlen(pName->partdetailname))
      {
         newgeom = pName->partdetailname;

         BlockStruct *block = doc->Find_Block_by_Name(cur_altname,-1);
         Attrib *att;

         if (block != NULL && (att = is_attvalue(doc, block->getAttributesRef(),"PACKALT Name", 0)) != NULL)  
         {
            CString attValue;
            attValue = get_attvalue_string(doc, att);

            if (strlen(attValue) == 0)
            {
               fprintf(ferr,"No PACKALT NAME found for [%s] -> not changed.\n", cur_altname);
               display_error++;
            }
            else
            {
               // here do it.
               newgeom += "|";
               newgeom += attValue;
            }
         }
      }
      else
      {
         fprintf(ferr,"No PART DETAILNAME found for [%s] -> [%s] not changed.\n",
            pName->prtname, cur_altname);
         display_error++;
      }

      if (strlen(newgeom))
      {
         // now check if already defined
         newgeom = newgeom + "|" + geomname;

         if (Graph_Block_Exists(doc, newgeom, -1) == NULL)
         {
            BlockStruct *bExists = Graph_Block_Exists(doc, cur_altname, -1);
            BlockStruct *bOn = Graph_Block_On(GBO_APPEND,newgeom,file->getFileNumber(),0);

            //if (is_attrib(bOn->getName(), FIDUCIAL_ATTR) != 0)
            //{
            //   bOn->setBlockType(blockTypeFiducial);
            //   bExists->setBlockType(blockTypeFiducial);
            //}

            //if (is_attrib(bOn->getName(), TEST_ATTR) != 0)
            //{
            //   bOn->setBlockType(blockTypeTestPoint);
            //   bExists->setBlockType(blockTypeTestPoint);
            //}
            //else
            if (bExists != NULL)
            {
               bOn->setBlockType(bExists->getBlockType());
               Graph_Block_Copy(bExists, 0, 0, 0, 0, 1, -1, TRUE);
            }

            Graph_Block_Off();

            doc->SetUnknownAttrib(&bOn->getAttributesRef(),ATT_DERIVED,geomname,
               SA_OVERWRITE, NULL); // x, y, rot, height
         }

         geomname = newgeom;         
      }
   }

   // partnamearray has a packref
   // altname has a packref

   // if they are the same, cur_libname = partnamearray[pPtr]->prtname
   if (!STRCMPI(cur_libname, partnamearray[pPtr]->prtname))
   {
      cur_libname = partnamearray[pPtr]->partname;
   }
   else
   {
      cur_libname.Format("%s_%s", partnamearray[pPtr]->partname, geomname);
   }

   update_packaltused(geomname);

   // This is the initial PCBCOMP placement.
   curdata = Graph_Block_Reference(geomname, G.name , file->getFileNumber(),  0, 0, 0.0, 0, 1.0, -1, TRUE);
   
   // Set part name attrib as original part name, not potentially modified part name i.e. NOT cur_libname
   doc->SetUnknownAttrib(&curdata->getAttributesRef(),get_attribmap("CADIF_PART_NAME"), partnamearray[pPtr]->partname, SA_OVERWRITE, NULL);
   

   return res;
}

/****************************************************************************/
/*
*/
static int packaltdesc()
{
   int res = loop_command(packaltdesc_lst,SIZ_PACKALTDESC_LST);
   return res;
}

/******************************************************************************
* packaltterm
*/
static int packaltterm()
{
   if (!get_tok()) 
      return p_error();

   CString pinID = token;
   int pinNumber = atoi(&token[1]); // kill T%d

   if (pinNumber == 0)
   {
      fprintf(ferr, "Packterm [%s] is not a pinnr at %ld\n", token, ifp_line);
   }
   
   // init position.
   G.cur_pos_x = 0;
   G.cur_pos_y = 0;

   cur_padname = "";    // if packaltterm has no padcode, pin does not exist

   int res = loop_command(packaltterm_lst, SIZ_PACKALTTERM_LST);

   CCadifPin* pin = packageNameMap.FindPackagePin(cur_packref, pinID); 

   if (pin == NULL)
      return -1;

   if (!strlen(cur_padname))
   {
      if (IGNORE_NOPADCODE_PINS)
      {
         fprintf(ferr, "PackAltTerm [%s] PinName [%s] defined without a PADSTACK at %ld -> ignored\n", cur_altname, pinID, ifp_line);
         display_error++;

         CADIFAltPin *altPin = new CADIFAltPin;
         ignore_altname_pinname_array.SetAtGrow(ignore_altname_pinname_cnt, altPin);
         ignore_altname_pinname_cnt++;
         altPin->altname = cur_altname;
         altPin->pinname = pin->GetName();

         polyarray.RemoveAll();
         polycnt = 0;   

         return res;
      }
      else
      {
         fprintf(ferr, "PackAltTerm defined without a PADSTACK at %ld -> NULLPADCODE assigned\n", ifp_line);
         display_error++;

         cur_padname = "NULLPADCODE";

         if (Graph_Block_Exists(doc, cur_padname, -1) == NULL)
         {
            BlockStruct *b = Graph_Block_On(GBO_APPEND, cur_padname, file->getFileNumber(), 0);
            b->setBlockType(blockTypePadstack);
            Graph_Block_Off();
         }
      }
   }

   // here place pin
   DataStruct *data = Graph_Block_Reference(cur_padname, pin->GetName(), file->getFileNumber(), G.cur_pos_x, G.cur_pos_y, 0.0, 0, 1.0, -1, TRUE);

   //if (is_attrib(cur_padname, FIDUCIAL_ATTR))
   //   data->getInsert()->setInsertType(insertTypeFiducial);
   //else
   //   data->getInsert()->setInsertType(insertTypePin);

   if (getCadifReader().isFiducialEntityName(cur_padname))
   {
      data->getInsert()->setInsertType(insertTypeFiducial);
   }
   else
   {
      data->getInsert()->setInsertType(insertTypePin);
   }

   // do pinnr here
   if (pin->GetPinNumber() > 0)
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, 0), VT_INTEGER, &pinNumber, SA_OVERWRITE, NULL); //

   G.cur_pincnt++;

   polyarray.RemoveAll();
   polycnt = 0;  

   return res;
}

/******************************************************************************
* packdesc_leadform
*/
static int packdesc_leadform()
{
   if (!get_tok())   
      return p_error();

   CString form = token;

   CCadifPackage* package = packageNameMap.GetLastPackage();
   if (form.CompareNoCase("THRU") == 0)
      package->SetLeadform(eCadifLeadformTHRU);
   else if (form.CompareNoCase("SMD") == 0)
      package->SetLeadform(eCadifLeadformSMD);
   else
      return p_error(); // unrecognized leadform value


   return 1;
}

/******************************************************************************
* packdesc_packterm
*/
static int packdesc_packterm()
{
   char  oldname[80];

   strcpy(oldname, G.name);
   if (!get_tok())   
      return p_error();

   CString pinID = token;
   int res = loop_command(packterm_lst,SIZ_PACKTERM_LST);

   CCadifPackage* package = packageNameMap.GetLastPackage();
   package->AddPin(pinID, G.name);

   strcpy(G.name, oldname);

   return res;
}

/****************************************************************************/
/*
*/
static int symdata()
{
   int res = loop_command(symdata_lst,SIZ_SYMDATA_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int symdesc()
{
   int res = loop_command(symdata_lst,SIZ_SYMDATA_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int insertdata()
{
   loop_command(insertdata_lst,SIZ_INSERTDATA_LST);

   return 1;
}

/****************************************************************************/
/*
*/
static int design_technology()
{
   loop_command(technology_lst,SIZ_TECHNOLOGY_LST);

   return 1;
}

/****************************************************************************/

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(List *tok_lst,int tok_size)
{
   for (int i = 0; i < tok_size; ++i)
      if (!STRCMPI(token,tok_lst[i].token))
         return(i);

   return(-1);
}

/****************************************************************************/
/*
   Get a token from the input file.
*/
static int get_tok()
{
   if (!Push_tok)
   {
      while (!get_next(cur_line,cur_new))
         if (!get_line(cur_line,MAX_LINE))
            return(FALSE);
         else
            cur_new = TRUE;

      cur_new = FALSE;
   }
   else
      Push_tok = FALSE;

   return(TRUE);
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
static int get_next(char *lp,int newlp)
{
   static   char  *cp;
   int      i = 0;

   token_name = FALSE;

   if (newlp)  cp = lp;

   for (; isspace(*cp) && *cp != '\0'; ++cp) ;

   switch(*cp)
   {
   case '\0':
      return 0;
   case '(':
   case ')':
      token[i++] = *(cp++);
      break;
   case '\"':
      token[i] = '\0';
      token_name = TRUE;

      for (++cp; *cp != '\"'; ++cp, ++i)
      {
         if (i >= MAX_TOKEN-1)
         {
            fprintf(ferr,"Error in Get Token - Token too long at %ld\n", ifp_line);
            display_error++;
         }

         if (*cp == '\n')
         {
            /* text spans over 1 line */
            if (!get_line(cur_line,MAX_LINE))
            {
               fprintf(ferr,"Error in Line Read\n");
               display_error++;
            }

            cp = cur_line;

            /* the quote can be the first character in the new line */
            if (*cp == '"' && token[i] != '\\')
               break;

            if (*cp == '\\')
            {
               ++cp;

               if (*cp == 'b')
                  token[i] = ' ';
               else
                  token[i] = *cp;
            }
            else
               token[i] = *cp;
         }
         else if (*cp == '\\')
         {
            ++cp;

            if (*cp == '\n')
            {
               /* text spans over 1 line */
               if (!get_line(cur_line,MAX_LINE))
               {
                  fprintf(ferr,"Error in Line Read\n");
                  display_error++;
               }

               cp = cur_line;
            }
            else if (*cp == '0')
            {
               // here is the decimal char number
               char num[4];
               ++cp;
               num[0] = *cp;
               ++cp;
               num[1] = *cp;
               ++cp;
               num[2] = *cp;
               num[3] = '\0';
               token[i] = atoi(num);
            }
            else if (*cp == 'n')
               token[i] = '\n';
            else
               token[i] = *cp;
         }
         else if (*cp != '\0')
            token[i] = *cp;
      }

      ++cp;
      break;
   default:
      for (; !isspace(*cp) && *cp != '\"' &&
                        *cp != '(' && *cp != ')' &&
                        *cp != '\0'; ++cp, ++i)
      {
         if (*cp == '\\')
            ++cp;
         token[i] = *cp;
      }

      break;
   }

   if (!i && !token_name)
      return(FALSE);

   token[i] = '\0';

   return(TRUE);
}

/****************************************************************************/
/*
   Get a line from the input file.
*/
static int get_line(char *cp,int size)
{
   if (fgets(cp,size,ifp) == NULL)
   {
      if (!feof(ifp))
      {
         ErrorMessage("Read Error", "Fatal CADIF Parse Error", MB_OK | MB_ICONHAND);
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
      if ((ifp_line % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }

      if (cp[strlen(cp)-1] != '\n')
      {
         fprintf(ferr," No end of line found at %ld\n",ifp_line);
         return (FALSE);
      }
   }

   ++ifp_line;

   return(TRUE);
}


/****************************************************************************/
/*
   Parsing error.
*/
int p_error()
{
   CString  tmp;
   CString  t;
   tmp.Format("Fatal Parsing Error : Token \"%s\" on line %ld\n", token, ifp_line);
   t  = tmp;
   
   ErrorMessage(t, "Fatal CADIF Parse Error", MB_OK | MB_ICONHAND);
   
   return -1;
}

/****************************************************************************/
/*
*/
void init_all_mem()
{
   //if ((attr = (CADIFAttr  *)calloc(MAX_ATTR,sizeof(CADIFAttr))) == NULL)
   //   MemErrorMessage(__FILE__, __LINE__);

   //attrcnt = 0;

   netlistarray.SetSize(100,100);
   netlistcnt = 0;

   testaccessarray.SetSize(100,100);
   testaccesscnt = 0;

   polyarray.SetSize(100,100);
   polycnt = 0;

   drillarray.SetSize(100,100);
   drillcnt = 0;

   compinstarray.SetSize(100,100);
   compinstcnt = 0;

   textstylearray.SetSize(100,100);
   textstylecnt = 0;

   ignore_altname_pinname_array.SetSize(100,100);
   ignore_altname_pinname_cnt = 0;

   attribmaparray.SetSize(100,100);
   attribmapcnt = 0;

   packageNameMap.empty();

   partnamearray.SetSize(100,100);
   partnamecnt = 0;

   memset(&G,0,sizeof(Global));

   TopElecLayer = "";
   BotElecLayer = "";
   TopSurfLayer = "";
   BotSurfLayer = "";

   return;
}

void free_all_mem()
{
   //for (int i=0;i<attrcnt;i++)
   //   free(attr[i].name);

   //free(attr);

   for (int i=0;i<compinstcnt;i++)
   {
      delete compinstarray[i];
   }

   compinstarray.RemoveAll();

   for (int i=0;i<netlistcnt;i++)
   {
      delete netlistarray[i];
   }

   netlistarray.RemoveAll();

   for (int i=0;i<testaccesscnt;i++)
   {
      delete testaccessarray[i];
   }

   testaccessarray.RemoveAll();
   padstackarray.empty();

   for (int i=0;i<textstylecnt;i++)
   {
      delete textstylearray[i];
   }

   textstylearray.RemoveAll();
   textstylecnt = 0;

   for (int i=0;i<ignore_altname_pinname_cnt;i++)
   {
      delete ignore_altname_pinname_array[i];
   }

   ignore_altname_pinname_array.RemoveAll();
   ignore_altname_pinname_cnt = 0;

   for (int i=0;i<attribmapcnt;i++)
   {
      delete attribmaparray[i];
   }

   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   packageNameMap.empty();

   for (int i=0;i<partnamecnt;i++)
   {
      delete partnamearray[i];
   }

   partnamearray.RemoveAll();

   packaltarray.empty();
   //for (i=0;i<packaltcnt;i++)
   //{
   //   delete packaltarray[i];
   //}

   //packaltarray.RemoveAll();

   drillarray.RemoveAll();

   return;
}

/****************************************************************************/
/*
*/
static int get_drillindex(double size)
{
   CADIFDrill  p;

   if (size == 0) return -1;

   for (int i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);

      if (p.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,file->getFileNumber(),0);
   //curblock->blocktype = BLOCKTYPE_PADSHAPE;
   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

/*
   Graph_Aperture(name, T_ROUND, size, 0.0, 0.0, 0.0, 0.0, 0, 
            BL_DRILL, TRUE, &err);
*/
   p.d = size;
   drillarray.SetAtGrow(drillcnt,p);  
   drillcnt++;

   return drillcnt -1;
}

/****************************************************************************/
/*
   NULL string will return 0  
   This units come in inches.
   cadif file is in 10-8 meters = 0.01 micros = inch * 2540000

   Units_Factor(format.PortFileUnits, Settings.PageUnits)

*/
double cnv_unit(const char *s)
{
   if (STRLEN(s) == 0) 
      return 0.0;

   double x = atof(s) * faktor;
   return x;
}

/****************************************************************************/
/*
*/
static int get_netnameptr(const char *n)
{
   for (int i=0;i<netlistcnt;i++)
   {
      if (!STRCMPI(netlistarray[i]->net_name,n))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_signalnameptr(char *n)
{
   for (int i=0;i<netlistcnt;i++)
   {
      if (!STRCMPI(netlistarray[i]->signal_name,n))
         return i;
   }

   return -1;
}


/****************************************************************************/
/*
*/
static int get_fontptr(const char *f)
{
   for (int i=0;i<textstylecnt;i++)
   {
      if (!strcmp(textstylearray[i]->fontname,f))
         return i;
   }

   fprintf(ferr,"Can not find textstyle nr [%s]\n",f);
   display_error++;

   return -1;
}

/****************************************************************************/
/*
*/
static int get_partnameptr(const char *partn)
{
   for (int i=0;i<partnamecnt;i++)
   {
      CADIFPartName  *pName = partnamearray[i];

      if (!strcmp(pName->prtname,partn))
         return i;
   }

   fprintf(ferr,"Part name [%s] not found in part list\n",partn);
   display_error++;

   return -1;
}

/****************************************************************************/
/*
  the sequence \n makes a new line
*/
static int get_nexttextline(char *orig, char *l, int maxl)
{
   int   lcnt = 0;
   int   newline = FALSE;
   int   mess = FALSE;

   l[0] = '\0';

   for (unsigned int i=0; i < strlen(orig); i++)
   {
/*      if (orig[i] == '\n')
      {
         newline = TRUE;
         break;
      }
      else
      {
*/         
         if (lcnt < maxl)
         {
            l[lcnt++] = orig[i];
         }
         else
         {
            if (!mess)     // only write message once
            {
               fprintf(ferr, "Textstring [%d] too long at %ld\n", maxl, ifp_line);
               display_error++;
            }

            fprintf(ferr, "Textstring [%d] too long at %ld\n", maxl, ifp_line);
            l[maxl - 1] = '\0';
            mess = TRUE;
         }
//      }
   }

   if (lcnt) 
   {
      l[lcnt] = '\0';
/*      if (newline)
      {
         strrev(orig);
         orig[strlen(orig)-lcnt-1] = '\0';
         strrev(orig);
      }
      else
*/         orig[0] = '\0';

   }

   return lcnt;
}

/******************************************************************************
* get_packagenameptr_from_altname
*/
CString get_packagenameptr_from_altname(CString altName)
{
   CADIFPackAlt* packalt = packaltarray.getPackalt(altName,USE_PACKALT_NAME != 0);

   if (packalt != NULL)      
      return packalt->pacname;

   return "";
}

/****************************************************************************/
/*
*/
static int get_layerptr(const char *l)
{
   for (int i=0;i<layerlistcnt;i++)
   {
      if (!STRCMPI(layerlist[i].name,l))
         return i;
   }   

   if (layerlistcnt < MAX_LAYERS)
   {
      layerlist[layerlistcnt].name = l;
      layerlist[layerlistcnt].id = 0;
      layerlist[layerlistcnt].layerref = -1;
      layerlist[layerlistcnt].ppsignalcnt = 0;
      layerlistcnt++;

      return layerlistcnt-1;
   }
   else
   {
      fprintf(ferr, "Too many layers [%d]\n", layerlistcnt);
      display_error++;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_globallayerptr(const char *l)
{
   for (int i=0;i<globallayerlistcnt;i++)
   {
      if (!STRCMPI(globallayerlist[i].name,l))
         return i;
   }   

   if (globallayerlistcnt < MAX_LAYERS)
   {
      globallayerlist[globallayerlistcnt].name = l;
      globallayerlist[globallayerlistcnt].layerref = -1;
      globallayerlist[globallayerlistcnt].ppsignalcnt = 0;
      globallayerlistcnt++;

      return globallayerlistcnt-1;
   }
   else
   {
      fprintf(ferr, "Too many global layers [%d]\n", globallayerlistcnt );
      display_error++;
   }

   return -1;
}

/*****************************************************************************/
/*
*/
static int get_lay_index(const char *l)
{
   for (int i=0;i<layer_attr_cnt;i++)
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
      layer_attr_cnt++;
   }
   else
   {
      fprintf(ferr,"Too many layers\n");

      return -1;
   }

   return layer_attr_cnt-1;
}

/****************************************************************************/
/*
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0;i<MAX_LAYTYPE;i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return   LAYTYPE_UNKNOWN; 
//}

/****************************************************************************/
/*
*/
//static int update_attr(const char *lp,int typ)
//{
//   if (attrcnt < MAX_ATTR)
//   {
//      if ((attr[attrcnt].name = STRDUP(lp)) == NULL)
//         MemErrorMessage(__FILE__, __LINE__);
//
//      attr[attrcnt].atttype = typ;
//      attrcnt++;
//   }
//   else
//   {
//      // error
//   }
//
//   return attrcnt-1;
//}

//static bool parseYesNo(bool& flag,CString value)
//{
//   if (value.GetLength() > 0)
//   {
//      value = value.Left(1);
//
//      if (value.CompareNoCase("y") == 0)
//      {
//         flag = true;
//      }
//      else if (value.CompareNoCase("n") == 0)
//      {
//         flag = false;
//      }
//   }
//
//   return flag;
//}

/******************************************************************************
* loadCadifSettings
*/
static int loadCadifSettings(CString settingsFileName)
{
   compOutlineLayerNameList.RemoveAll();

   IGNORE_NOPADCODE_PINS        = false;
   IGNORE_COMPPINS              = false;
   IGNORE_EMPTYPADCODE_PINS     = false;
   USE_LAYERNAME                = false;
   CHANGEALT_PARTDETAIL_ALTNAME = false;
   USE_PADCODE_NAME             = false;
   USE_PACKALT_NAME             = false;
   CADIF_FORMAT                 = CADIF_CADSTAR;
   PADUSRDEF_ROTATION           = false;
   PADUSRDEF_SCALE              = false;
   DO_ISLANDS                   = false;
   COMPHEIGHT_UNIT              = page_units;
   COPPER_TO_MECHPIN            = false;
   READSYMLABEL                 = true;
   CONVERT_COMPLEX_APERTURES    = false;

   MakeInto_CommandProcessor.Reset(); /// change made here

   CInFile inFile;   

   if (!inFile.open(settingsFileName))
   {
      CString tmp;
      tmp.Format("File [%s] not found", settingsFileName);
      ErrorMessage(tmp, "Cadif Settings", MB_OK | MB_ICONHAND);

      return 0;
   }

   while (inFile.getNextCommandLine())
   {
      if (inFile.isCommand(".CADIF",2))
      {
         if (inFile.getParam(1).CompareNoCase("CADSTAR") == 0)
         {
            CADIF_FORMAT = CADIF_CADSTAR;
         }
         else if (inFile.getParam(1).CompareNoCase("VISULA") == 0)
         {
            CADIF_FORMAT = CADIF_VISULA;
         }
      }
      else if (inFile.isCommand(".CHANGEALTNAME",2))
      {
         if (inFile.getParam(1).CompareNoCase("PARTDETAIL_ALTNAME") == 0)
         {
            CHANGEALT_PARTDETAIL_ALTNAME = true;
         }
      }
      else if (inFile.isCommand(".USE_LAYERNAME",2))
      {
         inFile.parseYesNoParam(1,USE_LAYERNAME,false);
      }
      else if (inFile.isCommand(".USE_PADCODE_NAME",2))
      {
         inFile.parseYesNoParam(1,USE_PADCODE_NAME,false);
      }
      else if (inFile.isCommand(".PADUSRDEF_ROTATION",2))
      {
         inFile.parseYesNoParam(1,PADUSRDEF_ROTATION,false);
      }
      else if (inFile.isCommand(".PADUSRDEF_SCALE",2))
      {
         inFile.parseYesNoParam(1,PADUSRDEF_SCALE,false);
      }
      else if (inFile.isCommand(".DO_ISLANDS",2))
      {
         inFile.parseYesNoParam(1,DO_ISLANDS,false);
      }
      else if (inFile.isCommand(".USE_PACKALT_NAME",2))
      {
         inFile.parseYesNoParam(1,USE_PACKALT_NAME,false);
      }
      else if (inFile.isCommand(".IGNORE_NOPADCODE_PINS",2))
      {
         inFile.parseYesNoParam(1,IGNORE_NOPADCODE_PINS,false);
      }
      else if (inFile.isCommand(".IGNORE_EMPTYPADCODE_PINS",2))
      {
         inFile.parseYesNoParam(1,IGNORE_EMPTYPADCODE_PINS,false);
      }
      else if (inFile.isCommand(".IGNORE_COMPPINS",2))
      {
         inFile.parseYesNoParam(1,IGNORE_COMPPINS,false);
      }
      else if (inFile.isCommand(".ComponentSMDrule",2))
      {
         ComponentSMDrule = inFile.getIntParam(1);
      }
      else if (inFile.isCommand(".COMPOUTLINE",2))
      {
         CString layerName = inFile.getParam(1);

         if (!compOutlineLayerNameList.Find(layerName))
         {
            compOutlineLayerNameList.AddTail(layerName);
         }
      }
      else if (inFile.isCommand(".FIDUCIALATTR",2))  // need to process after geometry name fixup
      {
         //update_attr(inFile.getParam(1), FIDUCIAL_ATTR);
         getCadifReader().addFiducialExpression(inFile.getParam(1));
      }
      else if (inFile.isCommand(".MAKE_INTO_TESTPOINT",2))  // need to process after geometry name fixup
      {
         //update_attr(inFile.getParam(1), TEST_ATTR);
         getCadifReader().addTestPointExpression(inFile.getParam(1));
      }
      else if (inFile.isCommand(".ATTRIBMAP",3))
      {
         CString a1 = inFile.getParam(1).MakeUpper();
         CString a2 = inFile.getParam(2).MakeUpper();

         CADIFAttribmap *c = new CADIFAttribmap;
         attribmaparray.SetAtGrow(attribmapcnt,c);

         c->attrib    = a1;
         c->mapattrib = a2;
         attribmapcnt++;        
      }
      else if (inFile.isCommand(".COMPHEIGHT",3))
      {
         CString heightAttrib = inFile.getParam(1).MakeUpper();
         CString unit         = inFile.getParam(2).MakeUpper();

         CADIFAttribmap *c = new CADIFAttribmap;
         attribmaparray.SetAtGrow(attribmapcnt++, c);
         c->attrib    = heightAttrib;
         c->mapattrib = ATT_COMPHEIGHT;
         
         if (unit.CompareNoCase("inch") == 0)
         {
            COMPHEIGHT_UNIT = UNIT_INCHES;
         }
         else if (unit.CompareNoCase("mil") == 0)
         {
            COMPHEIGHT_UNIT = UNIT_MILS;
         }
         else if (unit.CompareNoCase("mm") == 0)
         {
            COMPHEIGHT_UNIT = UNIT_MM;
         }
      }
      else if (inFile.isCommand(".LAYERATTR",3))
      {
         CString tllayer = inFile.getParam(1).MakeUpper();
         CString cclayer = inFile.getParam(2).MakeUpper();

         LayerTypeTag laytype = stringToLayerTypeTag(cclayer);
         int layindex = get_lay_index(tllayer);
         layer_attr[layindex].attr = laytype;
      }
      else if (inFile.isCommand(".MIRRORLAYER",3))
      {
         CString lay1 = inFile.getParam(1);
         CString lay2 = inFile.getParam(2);

         if (!getCadifReader().getCadifMirroredLayerPairs().addLayerPair(lay1,lay2))
         {
            CString msg;
            msg.Format("Cannot mirror layer '%s' with '%s', one or both are mirrored with other layers.",lay1,lay2);
            ErrorMessage(msg);
         }
      }
      else if (inFile.isCommand(".AttributeVisibility",3))
      {
         CString attributeName   = inFile.getParam(1).MakeLower();
         CString visibilityValue = inFile.getParam(2);

         getCadifReader().getAttributeVisibilityMap().setVisibility(attributeName,visibilityValue);
      }
      else if (inFile.isCommand(".COPPER_TO_MECHPIN",2))
      {
         inFile.parseYesNoParam(1,COPPER_TO_MECHPIN,false);
      }
      else if (inFile.isCommand(".READSYMLABEL",2))
      {
         inFile.parseYesNoParam(1,READSYMLABEL,true);
      }
      else if (inFile.isCommand(".ConvertComplexApertures",2))
      {
         inFile.parseYesNoParam(1,CONVERT_COMPLEX_APERTURES,false);
      }
      else if (inFile.isCommand(".FixGeometryNames",2))
      {
         bool flag;
         inFile.parseYesNoParam(1,flag,false);
         getCadifReader().setOptionFixGeometryNamesFlag(flag);
      }
      else if (inFile.isCommand(".MergeFixedGeometries",2))
      {
         bool flag;
         inFile.parseYesNoParam(1,flag,false);
         getCadifReader().setOptionMergeFixedGeometriesFlag(flag);
      }

      ///MakeIntoTooling Command goes here
      else if (MakeInto_CommandProcessor.ParseCommand(inFile.getCommandLineString())) // send whole text line, not just command
      {
            // Already did what it needed to do
      }

      else
      {
         // 4/27/06 email from Chris says stop the pop-ups, write msg to log file instead.
         fprintf(ferr,"Unrecognized command line encountered, '%s', while reading '%s'\n",
            inFile.getCommandLineString(),inFile.getFilePath());
      }
   }

   if (getCadifReader().getOptionFixGeometryNamesFlag())
   {
      CHANGEALT_PARTDETAIL_ALTNAME = false;

      fprintf(ferr,"'.FixGeometryNames y' command caused the '.CHANGEALTNAME PARTDETAIL_ALTNAME' command to be ignored.\n");
   }

/* if (Product == PRODUCT_PCB_TRANSLATOR)
   {
      IGNORE_NOPADCODE_PINS = FALSE;
      IGNORE_EMPTYPADCODE_PINS = FALSE;
   }
*/
   return 1;
}

/****************************************************************************/
/*
   TopElecLayer
   BotElecLayer
   TopSurfLayer
   BotSurfLayer
*/
static int update_mirrorlayers()
{
   LayerStruct *lp;

   if (USE_LAYERNAME)
   {
      for (int i=0;i<layerlistcnt;i++)
      {
         if (strlen(layerlist[i].username) == 0)
            continue;

         if (TopElecLayer.Compare(layerlist[i].name) == 0) 
            TopElecLayer = layerlist[i].username;

         if (BotElecLayer.Compare(layerlist[i].name) == 0) 
            BotElecLayer = layerlist[i].username;

         if (TopSurfLayer.Compare(layerlist[i].name) == 0) 
            TopSurfLayer = layerlist[i].username;

         if (BotSurfLayer.Compare(layerlist[i].name) == 0) 
            BotSurfLayer = layerlist[i].username;
      }
   }
   else
   {
      for (int j = 0; j< doc->getMaxLayerIndex(); j++)
      {
         lp = doc->getLayerArray()[j];

         if (lp == NULL)   
            continue; // could have been deleted.

         for (int i=0;i<layerlistcnt;i++)
         {
            if (lp->getName().Compare(layerlist[i].name) == 0)
            {
               lp->setComment(layerlist[i].username);
               break;
            }
         }
      }
   }

   for (POSITION pos = getCadifReader().getCadifMirroredLayerPairs().getHeadPosition();pos != NULL;)
   {
      CCadifMirroredLayerPair* layerPair = getCadifReader().getCadifMirroredLayerPairs().getNext(pos);

      LayerStruct* layer         = doc->getLayerNoCase(layerPair->getLayerName());
      LayerStruct* mirroredLayer = doc->getLayerNoCase(layerPair->getMirroredLayerName());

      if (layer != NULL || mirroredLayer != NULL)
      {
         if (layer == NULL)
         {
            layer = doc->getLayerAt(Graph_Level(layerPair->getLayerName(),"", 0));
         }

         if (mirroredLayer == NULL)
         {
            mirroredLayer = doc->getLayerAt(Graph_Level(layerPair->getMirroredLayerName(),"", 0));
         }

         Graph_Level_Mirror(layer->getName(),mirroredLayer->getName());
      }
   }

   if (strlen(TopElecLayer) && strlen(BotElecLayer))
   {
      Graph_Level(TopElecLayer,"", 0);
      Graph_Level(BotElecLayer,"", 0);
      Graph_Level_Mirror(TopElecLayer, BotElecLayer, "");
   }

   if (strlen(TopSurfLayer) && strlen(BotSurfLayer))
   {
      Graph_Level(TopSurfLayer,"", 0);
      Graph_Level(BotSurfLayer,"", 0);
      Graph_Level_Mirror(TopSurfLayer, BotSurfLayer, "");
   }

   // update layer usage for global layer if they are used.
   for (int j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      lp = doc->getLayerArray()[j];

      if (lp == NULL)
         continue; // could have been deleted.

      for (int i=0;i<globallayerlistcnt;i++)
      {
         if (lp->getName().Compare(globallayerlist[i].name) == 0)
         {
            lp->setLayerType(get_layertype_from_usage(globallayerlist[i].usage));
            break;
         }
      }
   }

   // now attributes after mirror layers
   for (int j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      lp = doc->getLayerArray()[j];

      if (lp == NULL)
         continue; // could have been deleted.

      // done by CAMCAD.
      if (lp->getName().CompareNoCase("DRILLHOLE") == 0)
      {
         lp->setLayerType(LAYTYPE_DRILL);
      }
      else if (lp->getName().CompareNoCase("Board") == 0)
      {
         lp->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }

      for (int i=0;i < layer_attr_cnt;i++)
      {
         CString layerAttrName = layer_attr[i].name;

         if ((!USE_LAYERNAME && lp->getComment().CompareNoCase(layerAttrName) == 0) ||
             ( USE_LAYERNAME && lp->getName(   ).CompareNoCase(layerAttrName) == 0)     )
         {
            lp->setLayerType(layer_attr[i].attr);
            break;
         }
      }
   }

   return 1;
}

/****************************************************************************
   use_pack_alt_name()
*/
static int use_pack_alt_name()
{
   for (int i=0; i < doc->getMaxBlockIndex() ; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      int index = block->getName().FindOneOf("|");

      if (index == -1)
         continue;

      if (block->getName().Find("|ALT",0) != -1)
      {
         CString altName = block->getName().Right(block->getName().GetLength() - index -1);

         for (POSITION pos = packaltarray.GetStartPosition();pos != NULL;)
         {
            CADIFPackAlt* packalt = packaltarray.GetNextAssoc(pos);

            if (packalt->getName().CompareNoCase(altName) == 0 && packalt->already_exist)
            {
               CString blockName = block->getName();
               blockName.Replace(altName, packalt->getRealName());
               block->setName(blockName);

               //getCadifReader().getPcbGeometryMap().renameGeometry(*block,blockName);
            }
         }

         //for (int j=0; j < packaltarray.GetCount(); j++)
         //{
         //   CADIFPackAlt *packAlt = packaltarray[j];

         //   if (!packAlt)
         //      continue;

         //   if (packAlt->name == altName && packAlt->already_exist)
         //      block->getNameRef().Replace(altName, packAlt->realname);
         //}
      }
   }

   return 0;
}

/******************************************************************************
* ConvertFilledPolyOnCopperToMechinicalPin
*/
void ConvertFilledPolyOnCopperToMechanicalPin()
{
   CCamCadDatabase camcadDataBase(*doc);
   
   for(int i=0; i<camcadDataBase.getNumBlocks(); i++)
   {
      BlockStruct* block = camcadDataBase.getBlock(i);
      if (block == NULL || block->getBlockType() != blockTypePcbComponent)
         continue;      

      int count = 0;
      for (POSITION pos = block->getHeadDataPosition(); pos != NULL;)
      {
         POSITION curPos = pos;
         DataStruct* polyData = block->getNextData(pos);
         if (polyData == NULL || polyData->getDataType() != dataTypePoly)
            continue;
      
         CPolyList* polyList = polyData->getPolyList();
         if (polyList == NULL || polyList->GetCount() > 1)
            continue;

         CPoly* poly = polyList->GetHead();
         if (poly == NULL || !poly->isFilled())
            continue;

         LayerStruct* layer = camcadDataBase.getLayerAt(polyData->getLayerIndex());
         if (layer == NULL)
            continue;
         if (layer->getLayerType() != layerTypeSignalTop)
            continue;

         BlockStruct* widthBlock = camcadDataBase.getCamCadDoc().getWidthBlock(poly->getWidthIndex());
         double width = widthBlock->getSizeA();
         CTMatrix matrix;
         BlockStruct* apertureBlock = camcadDataBase.getDefinedPolyAperture(*poly, width, 0.0, matrix);

         if (apertureBlock != NULL)
         {
            // Create the padstack
            CString padstackName;
            padstackName.Format("Padstack_%s", apertureBlock->getName());
            BlockStruct* padstack = camcadDataBase.getBlock(padstackName);

            if (padstack == NULL)
            {
               padstack = camcadDataBase.getDefinedBlock(padstackName, blockTypePadstack);
               DataStruct* padData = camcadDataBase.insertBlock(apertureBlock, insertTypeUnknown, "", layer->getLayerIndex());
               padstack->getDataList().AddTail(padData);
            }

            // Remove the poly
            RemoveOneEntityFromDataList(&camcadDataBase.getCamCadDoc(), &block->getDataList(), polyData, curPos);

            // Insert the mechanical pin
            CBasesVector basesVector;
            basesVector.transform(matrix);
            DataStruct* mechanicalPinData = camcadDataBase.insertBlock(padstack, insertTypeMechanicalPin, "", -1,
                  basesVector.getX(),basesVector.getY(),basesVector.getRotationRadians(),basesVector.getMirror());

            CString mechanicalPinName;
            mechanicalPinName.Format("MP_%d", ++count);
            mechanicalPinData->getInsert()->setRefname(STRDUP(mechanicalPinName));
            block->getDataList().AddTail(mechanicalPinData);
         }
      }
   }
}

//_____________________________________________________________________________
CCadifMirroredLayerPair::CCadifMirroredLayerPair(const CString& layerName,const CString& mirroredLayerName)
: m_layerName(layerName)
, m_mirroredLayerName(mirroredLayerName)
{
}

//_____________________________________________________________________________
CCadifMirroredLayerPairs::CCadifMirroredLayerPairs()
: m_layerPairMap(10,false)
{
   m_layerPairMap.InitHashTable(nextPrime2n(300));
}

bool CCadifMirroredLayerPairs::addLayerPair(const CString& layerName,const CString& mirroredLayerName)
{
   CString layerKey(layerName);
   layerKey.MakeLower();

   CString mirroredLayerKey(mirroredLayerName);
   mirroredLayerKey.MakeLower();

   CCadifMirroredLayerPair* layerPair = NULL;
   m_layerPairMap.Lookup(layerKey,layerPair);

   CCadifMirroredLayerPair* mirroredLayerPair = NULL;
   m_layerPairMap.Lookup(mirroredLayerKey,mirroredLayerPair);

   bool retval = (layerPair == mirroredLayerPair);

   if (retval && layerPair == NULL)
   {
      CCadifMirroredLayerPair* layerPair = new CCadifMirroredLayerPair(layerName,mirroredLayerName);
      m_layerPairs.AddTail(layerPair);

      m_layerPairMap.SetAt(layerKey,layerPair);
      m_layerPairMap.SetAt(mirroredLayerKey,layerPair);
   }

   return retval;
}

POSITION CCadifMirroredLayerPairs::getHeadPosition()
{
   return m_layerPairs.GetHeadPosition();
}

CCadifMirroredLayerPair* CCadifMirroredLayerPairs::getNext(POSITION& pos)
{
   return m_layerPairs.GetNext(pos);
}

//_____________________________________________________________________________
CADIFPadstack::CADIFPadstack(const CString& name) :
   pcname(name)
{
   used_as_via = 0;
   padassign   = 0;
   paddrill    = 0;
}

//_____________________________________________________________________________
CCadifPadstackArray::CCadifPadstackArray() : 
   m_padStacks(100)
{
}

void CCadifPadstackArray::empty()
{
   m_padStacks.empty();
}

//CADIFPadstack* CCadifPadstackArray::getAt(int index)
//{
//   CADIFPadstack* padstack = m_padStacks.getAt(index);
//
//   return padstack;
//}

CADIFPadstack* CCadifPadstackArray::find(const CString& geometryName)
{
   CADIFPadstack* padstack = NULL;
   m_padStacks.lookup(geometryName,padstack);
   return padstack;
}

CADIFPadstack* CCadifPadstackArray::getDefinedAt(const CString& geometryName)
{
   CADIFPadstack* padstack = NULL;

   m_padStacks.lookup(geometryName,padstack);

   if (padstack == NULL)
   {
      padstack = new CADIFPadstack(geometryName);
      m_padStacks.add(geometryName,padstack);
   }

   return padstack;
}

CADIFPadstack* CCadifPadstackArray::getLast()
{
   CADIFPadstack* padstack = NULL;

   if (m_padStacks.getSize() > 0)
   {
      padstack = m_padStacks.getAt(m_padStacks.getSize() - 1);
   }

   return padstack;
}

//_____________________________________________________________________________
CADIFPackAlt::CADIFPackAlt(const CString& name,const CString& realName) :
   m_name(name),
   m_realName(realName)
{
}


CCadifPackalts::CCadifPackalts() :
   m_packalts(10,true),
   m_realPackAlts(10,false)
{
}

CADIFPackAlt* CCadifPackalts::getPackalt(const CString& pacaltName,bool usePackaltName)
{
   CADIFPackAlt* packalt = NULL;

   if (usePackaltName && m_realPackAlts.Lookup(pacaltName,packalt))
   {
      if (packalt->already_exist)
      {
         packalt = NULL;
      }
   }

   if (packalt == NULL)
   {
      m_packalts.Lookup(pacaltName,packalt);
   }

   return packalt;
}

void CCadifPackalts::empty()
{
   m_packalts.empty();
   m_realPackAlts.empty();
}

CADIFPackAlt* CCadifPackalts::add(const CString& name,const CString& realName)
{
   CADIFPackAlt* packalt;

   if (!m_packalts.Lookup(name,packalt))
   {
      packalt = new CADIFPackAlt(name,realName);
      m_packalts.SetAt(name,packalt);

      // case 1579, only add realName alt if not already there,
      // otherwise what's already there gets overridden and later
      // lookup is mismatch, ulitmately causing failure to find 
      // correct packageID.
      CADIFPackAlt* realalt;
      if (!m_realPackAlts.Lookup(realName, realalt))
         m_realPackAlts.SetAt(realName,packalt);
   }

   return packalt;
}

POSITION CCadifPackalts::GetStartPosition()
{
   POSITION pos = m_packalts.GetStartPosition();

   return pos;
}

CADIFPackAlt* CCadifPackalts::GetNextAssoc(POSITION& pos)
{
   CString key;
   CADIFPackAlt* packalt;
   m_packalts.GetNextAssoc(pos,key,packalt);

   return packalt;
}

static DataStruct* findComponentData(FileStruct *file, CString refdes)
{   
   for (POSITION pos = file->getBlock()->getDataList().GetHeadPosition();pos != NULL;)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() == T_INSERT)
      {
         InsertStruct *insert = data->getInsert();
         CString insertname = insert->getRefname();

         if (data->getInsert()->getRefname().Compare(refdes) == 0)
         {
            return data;
         }
      }
   }

   return NULL;
}

static void update_comppin_side(CCEtoODBDoc *doc, FileStruct *file)
{
   //FileStruct *file = doc->FileList.GetOnlyShown(blockTypePcb);
   
   for (POSITION netpos = file->getNetList().GetHeadPosition();netpos != NULL;)
   {
      NetStruct *net = file->getNetList().GetNext(netpos);
      
      for (POSITION comppinpos = net->getCompPinList().getHeadPosition();comppinpos != NULL;)
      {
         CompPinStruct *comppin = net->getCompPinList().getNext(comppinpos);
         CString refdes = comppin->getRefDes();

         DataStruct *compdata = findComponentData(file, refdes);

         if (compdata && compdata->getInsert())
         {
            VisibleTag side = compdata->getInsert()->getPlacedTop() ? visibleTop : visibleBottom;
            comppin->setVisible(side);
         }
      }     
   }
}

//_____________________________________________________________________________
CCadifCompGeometries::CCadifCompGeometries(int blockSize) :
   m_geometries(blockSize,false),
   m_drilledGeomNames(blockSize)
{
}

bool CCadifCompGeometries::isGeometryDrilled(BlockStruct* geometry, CCadifPadstackArray& padstackarray)
{
   bool drilled = false;
   CString drilledFlag;
   if (!m_drilledGeomNames.Lookup(geometry->getName(), drilledFlag))
   {
      // Geometry has not been check for drill so check it
      for (POSITION pos = geometry->getHeadDataPosition();pos != NULL;)
      {
         DataStruct* data = geometry->getNextData(pos);

         if (data->getDataType() == dataTypeInsert && 
             data->getInsert()->getInsertType() == insertTypePin)
         {
            BlockStruct* padstackGeometry = doc->getBlockAt(data->getInsert()->getBlockNumber());

            CString padcodeName;
            if (USE_PADCODE_NAME)
            {
               // The geometry was renamed, and it stored the padcode name in its original name
               padcodeName = padstackGeometry->getOriginalName();
            }
            else
            {
               // The geometry was not rename, so it used the padcode name as its name
               padcodeName = padstackGeometry->getName();
            }

            CADIFPadstack* padstack = padstackarray.find(padcodeName);
            if (padstack != NULL && padstack->paddrill > 0)
            {
               drilled = true;
               break;
            }
         }
      }

      m_drilledGeomNames.SetAt(geometry->getName(), drilled?"drilled":"");
   }
   else
   {
      drilled = (drilledFlag.CompareNoCase("drilled")==0)?true:false;
   }

   return drilled;
}

BlockStruct* CCadifCompGeometries::getMirroredGeometry(BlockStruct* geometry,
   CCadifPadstackGeometries& padstackGeometries)
{
   CCamCadData& camCadData = doc->getCamCadData();

   BlockStruct* mirroredGeometry = NULL;
   CString mirroredGeometryName = geometry->getName() + "_M";

   if (!m_geometries.Lookup(mirroredGeometryName,mirroredGeometry))
   {
      mirroredGeometry = Graph_Block_On(GBO_APPEND,mirroredGeometryName,file->getFileNumber(),
         geometry->getFlags(),geometry->getBlockType());

      m_geometries.SetAt(mirroredGeometryName,mirroredGeometry);

      for (POSITION pos = geometry->getHeadDataPosition();pos != NULL;)
      {
         DataStruct* data = geometry->getNextData(pos);
         DataStruct* mirroredData = camCadData.getNewDataStruct(*data);
         mirroredGeometry->getDataList().AddTail(mirroredData);

         if (mirroredData->getDataType() == dataTypeInsert && 
             mirroredData->getInsert()->getInsertType() == insertTypePin)
         {
            BlockStruct* padstackGeometry = doc->getBlockAt(mirroredData->getInsert()->getBlockNumber());
            BlockStruct* mirroredPadstackGeometry = padstackGeometries.getMirroredGeometry(padstackGeometry);
            mirroredData->getInsert()->setBlockNumber(mirroredPadstackGeometry->getBlockNumber());
         }
      }

      Graph_Block_Off();
		doc->CopyAttribs(&mirroredGeometry->getAttributesRef(), geometry->getAttributesRef());

		int packageNameKeywordIndex = doc->RegisterKeyWord("CADIF_PACKAGE_NAME",valueTypeString);
		CString packageName;
      if (doc->getAttributeStringValue(packageName,*(mirroredGeometry->getDefinedAttributes()),packageNameKeywordIndex))
		{
			packageName.Append("_M");
			mirroredGeometry->setAttrib(doc->getCamCadData(), packageNameKeywordIndex, valueTypeString, packageName.GetBuffer(0), attributeUpdateOverwrite, NULL);
		}

   }

   return mirroredGeometry;
}

//_____________________________________________________________________________
CCadifPadstackGeometries::CCadifPadstackGeometries(int blockSize) :
   m_geometries(blockSize,false)
{
}

BlockStruct* CCadifPadstackGeometries::getMirroredGeometry(BlockStruct* geometry)
{
   CCamCadData& camCadData = doc->getCamCadData();

   BlockStruct* mirroredGeometry;
   CString mirroredGeometryName = geometry->getName() + "_M";

   if (!m_geometries.Lookup(mirroredGeometryName,mirroredGeometry))
   {
      mirroredGeometry = Graph_Block_On(GBO_APPEND,mirroredGeometryName,file->getFileNumber(),
         geometry->getFlags(),geometry->getBlockType());

      m_geometries.SetAt(mirroredGeometryName,mirroredGeometry);

      for (POSITION pos = geometry->getHeadDataPosition();pos != NULL;)
      {
         DataStruct* data = geometry->getNextData(pos);
         DataStruct* mirroredData = camCadData.getNewDataStruct(*data);
         mirroredGeometry->getDataList().AddTail(mirroredData);

         LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
         LayerStruct* mirroredLayer = doc->getLayerAt(layer->getMirroredLayerIndex());
         mirroredData->setLayerIndex(mirroredLayer->getLayerIndex());
      }

      Graph_Block_Off();
   }

   return mirroredGeometry;
}

//_____________________________________________________________________________
CADIFCompInst::CADIFCompInst(DataStruct& compInstance) :
   m_compInstance(compInstance)
{
}

//_____________________________________________________________________________
CCadifPin* CCadifPackage::AddPin(CString pinID, CString name)
{
   CCadifPin* pin = this->FindPin(pinID);
   if (pin == NULL)
   {
      pin = new CCadifPin(pinID, name);
      m_pinMap.SetAt(pinID, pin);
   }

   return pin;
}

CCadifPin* CCadifPackage::FindPin(CString pinID)
{
   CCadifPin* pin = NULL;
   this->m_pinMap.Lookup(pinID, pin);

   return pin;
}

//_____________________________________________________________________________
CCadifPackage* CCadifPackageMap::AddPackage(CString packageID, CString name)
{
   CCadifPackage* package = this->FindPackage(packageID);
   if (package == NULL)
   {
      package = new CCadifPackage(packageID, name);
      this->SetAt(packageID, package);
   }

   m_lastPackage = package;

   return package;
}

CCadifPackage* CCadifPackageMap::FindPackage(CString packageID)
{
   CCadifPackage* package = NULL;
   if (this->Lookup(packageID, package))
      return package;
   else
      return NULL;
}

CCadifPin* CCadifPackageMap::FindPackagePin(CString packageID, CString pinID)
{
   CCadifPackage* package = this->FindPackage(packageID);
   if (package == NULL)
      return NULL;

   CCadifPin* pin = package->FindPin(pinID);
   return pin;
}

//_____________________________________________________________________________
CCadifAttributeVisibilityMap::CCadifAttributeVisibilityMap()
{
   m_attributeMap.InitHashTable(nextPrime2n(100));
}

void CCadifAttributeVisibilityMap::setVisibility(CString attributeName,const CString& visibilityValue)
{
   attributeName.MakeLower();

   m_attributeMap.SetAt(attributeName,visibilityValue);
}

Bool3Tag CCadifAttributeVisibilityMap::getVisible(CString attributeName) const
{
   attributeName.MakeLower();
   Bool3Tag visible = boolUnknown;
   CString visibilityValue;

   if (m_attributeMap.Lookup(attributeName,visibilityValue))
   {
      visible = ((visibilityValue.Left(1).CompareNoCase("y") == 0) ? boolTrue : boolFalse);
   }

   return visible;
}

//_____________________________________________________________________________
CCadifPcbComponentGeometryMap::CCadifPcbComponentGeometryMap()
: m_pcbComponentGeometryMap(10,false)
{
   m_pcbComponentGeometryMap.InitHashTable(nextPrime2n(300));
   m_nonPreferredGeometryNameMap.InitHashTable(nextPrime2n(300));
}

void CCadifPcbComponentGeometryMap::addGeometry(BlockStruct& pcbGeometry)
{
   CString name = pcbGeometry.getName();

   m_pcbComponentGeometryMap.SetAt(name,&pcbGeometry);
}

void CCadifPcbComponentGeometryMap::renameGeometry(BlockStruct& pcbGeometry,const CString& newName)
{
   BlockStruct* existingGeometry;
   CString geometryName = pcbGeometry.getName();

   if (m_pcbComponentGeometryMap.Lookup(geometryName,existingGeometry))
   {
      if (existingGeometry == &pcbGeometry)
      {
         m_pcbComponentGeometryMap.RemoveKey(geometryName);
      }
   }

   CString uniqueName(newName);

   for (int index = 0;;index++)
   {
      if (!m_pcbComponentGeometryMap.Lookup(uniqueName,existingGeometry))
      {
         pcbGeometry.setName(uniqueName);

         break;
      }

      uniqueName.Format("%s_%d",newName,index);
   }

   m_pcbComponentGeometryMap.SetAt(pcbGeometry.getName(),&pcbGeometry);
}

BlockStruct* CCadifPcbComponentGeometryMap::lookup(const CString& geometryName)
{
   BlockStruct* geometry = NULL;

   if (!m_pcbComponentGeometryMap.Lookup(geometryName,geometry))
   {
      geometry = NULL;
   }

   return geometry;
}

void CCadifPcbComponentGeometryMap::fixGeometryNames(CCEtoODBDoc& camCadDoc)
{
   CString existingName;
   BlockStruct* geometry;
   BlockStruct* existingGeometry;

   CTypedPtrListContainer<BlockStruct*> geometryList(false);

   for (POSITION pos = m_pcbComponentGeometryMap.GetStartPosition();pos != NULL;)
   {
      m_pcbComponentGeometryMap.GetNextAssoc(pos,existingName,geometry);

      geometryList.AddTail(geometry);
   }

   int packageNameKeywordIndex = camCadDoc.RegisterKeyWord("CADIF_PACKAGE_NAME",valueTypeString);
   int packAltNameKeywordIndex = camCadDoc.RegisterKeyWord("CADIF_PACKALT_NAME",valueTypeString);

   for (POSITION pos = geometryList.GetHeadPosition();pos != NULL;)
   {
      geometry = geometryList.GetNext(pos);

      CString packageName,packAltName;

      camCadDoc.getAttributeStringValue(packageName,*(geometry->getDefinedAttributes()),packageNameKeywordIndex);
      camCadDoc.getAttributeStringValue(packAltName,*(geometry->getDefinedAttributes()),packAltNameKeywordIndex);

      CString newName(packageName);

      if (newName.GetLength() > 2 && newName.Right(2).CompareNoCase("_M") == 0)
      {
         newName.GetBufferSetLength(newName.GetLength() - 2);

         //if (m_pcbComponentGeometryMap.Lookup(newName,existingGeometry))
         //{
         //   newName = packageName;
         //}
      }

      CString preferredName(newName);

      if (m_pcbComponentGeometryMap.Lookup(newName,existingGeometry) && !packAltName.IsEmpty())
      {
         newName = packageName + "_" + packAltName;
      }

      if (!newName.IsEmpty())
      {
         renameGeometry(*geometry,newName);

         CString geometryName = geometry->getName();

         if (geometryName != preferredName)
         {
            m_nonPreferredGeometryNameMap.SetAt(geometryName,preferredName);
         }
      }
   }
}

//_____________________________________________________________________________
CCadifReader::CCadifReader(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_pcbFile(NULL)
, m_optionFixGeometryNamesFlag(false)
, m_optionMergeFixedGeometriesFlag(false)
{
}

void CCadifReader::setAttributeVisibility(CAttributes& attributes,const CString& attributeName)
{
   Attrib* attribute;
   int keywordIndex = m_camCadDoc.IsKeyWord(attributeName,0);

   if (attributes.Lookup(keywordIndex,attribute))
   {
      if (attribute->isVisible())
      {
         Bool3Tag visibleTag = m_attributeVisibilityMap.getVisible(attributeName);

         if (visibleTag != boolUnknown)
         {
            attribute->setVisible(visibleTag == boolTrue);
         }
      }
   }
}

void CCadifReader::addFiducialExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);

   m_fiducialExpressionList.AddTail(regularExpression);
}

bool CCadifReader::isFiducialEntityName(const CString& entityName)
{
   bool retval = false;

   for (POSITION pos = m_fiducialExpressionList.GetHeadPosition();pos != NULL && !retval;)
   {
      CRegularExpression* regularExpression = m_fiducialExpressionList.GetNext(pos);

      retval = regularExpression->matches(entityName);
   }

   return retval;
}

void CCadifReader::addTestPointExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);

   m_testPointExpressionList.AddTail(regularExpression);
}

bool CCadifReader::isTestPointEntityName(const CString& entityName)
{
   bool retval = false;

   for (POSITION pos = m_testPointExpressionList.GetHeadPosition();pos != NULL && !retval;)
   {
      CRegularExpression* regularExpression = m_testPointExpressionList.GetNext(pos);

      retval = regularExpression->matches(entityName);
   }

   return retval;
}

void CCadifReader::fixGeometryNames()
{
   if (m_optionFixGeometryNamesFlag)
   {
      for (int blockIndex = m_startingBlockIndex;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
      {
         BlockStruct* geometry = m_camCadDoc.getBlockAt(blockIndex);

         if (geometry != NULL)
         {
            m_pcbGeometryMap.addGeometry(*geometry);
         }
      }

      m_pcbGeometryMap.fixGeometryNames(m_camCadDoc);
   }

   for (int blockIndex = m_startingBlockIndex;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* geometry = m_camCadDoc.getBlockAt(blockIndex);

      if (geometry != NULL)
      {
         CString geometryName = geometry->getName();

         if (isFiducialEntityName(geometryName))
         {
            geometry->setBlockType(blockTypeFiducial);
            fprintf(ferr,"Block type set to Fiducial for geometry '%s'\n",geometryName);
         }

         if (isTestPointEntityName(geometryName))
         {
            geometry->setBlockType(blockTypeTestPoint);
            fprintf(ferr,"Block type set to TestPoint for geometry '%s'\n",geometryName);
         }
      }
   }

   if (m_pcbFile != NULL)
   {
      BlockStruct* pcbBlock = m_pcbFile->getBlock();
      CMapStringToString& nonPreferredGeometryNameMap = m_pcbGeometryMap.getNonPreferredGeometryNameMap();
      CString preferredGeometryName;

      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         BlockStruct* geometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
         CString geometryName = geometry->getName();

         if (isFiducialEntityName(geometryName))
         {
            data->getInsert()->setInsertType(insertTypeFiducial);
         }

         if (isTestPointEntityName(geometryName))
         {
            data->getInsert()->setInsertType(insertTypeTestPoint);
            m_camCadDoc.validateAndRepair(*data);
         }

         if (m_optionMergeFixedGeometriesFlag && nonPreferredGeometryNameMap.Lookup(geometryName,preferredGeometryName))
         {
            BlockStruct* preferredGeometry = m_pcbGeometryMap.lookup(preferredGeometryName);

            if (preferredGeometry != NULL)
            {
               data->getInsert()->setBlockNumber(preferredGeometry->getBlockNumber());

               fprintf(ferr,"Geometry changed from '%s' to '%s' for component '%s'\n",
                  geometryName,preferredGeometry->getName(),data->getInsert()->getRefname());
            }
         }
      }
   }
}

/*Ende **********************************************************************/



