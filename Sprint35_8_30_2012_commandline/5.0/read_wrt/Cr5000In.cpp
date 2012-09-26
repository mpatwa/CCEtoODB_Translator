
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved. 


   Limitations:
   - no mixed linewidth by vertex. (CR5000 can do it like Autocad)
   - no local component redefintions
   - no blind/buried via support
   - there is a arc definition error in the file. the center x y koos do not seem to be correct.
     I am now used start/end/radius. this makes 2 arcs, a smaller and a bigger. I use the smaller.
   - fillstyles are not supported


*/

/**************************RECENT UPDATES******************************/
/*
*
*  TSR 3659: 06-17-02 - Sadek Noureddine
*
*
*
*
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
#include "cr5000in.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LineStatusUpdateInterval 200

extern	CProgressDlg			*progress;
//extern	LayerTypeInfoStruct	layertypes[];
extern	char						*testaccesslayers[];

static   double   cnv_unit(const char *);
static   double   cnv_unit_noCorrection(const char *);
static   int      go_command(List *,int);
static   void     free_all_mem();
static   void     init_all_mem();

static   int      get_tok();
static   int      p_error();
static   int      tok_search(List *,int);
static   int      push_tok();
static   int      loop_command(List *,int);
static   int      tok_layer();
static   int      get_padnumber(char *);
static   int      get_next(char *,int);
static   int      get_line(char *,int);
static   int      get_fontptr(const char *f);

static   int      get_signalnameptr(char *n);
static   int      Cr5000LoadSettings(const CString fname);
static	void		inherit_footprint_layer_types();
static	void		register_cad_layers();
static   int      update_mirrorlayers();
static	void		purge_unused_layers();

static   int      get_drillindex(double size);

static   Global      G;                            /* global const. */

static   FILE        *ferr;
static   FILE        *ifp;                         /* File pointers.    */
static   long        ifp_line = 0;                 /* Current line number. */
static   CString     cur_filename;
static   CFileReadProgress*  fileReadProgress = NULL;

static   CR5000PinInst  cur_pin;
static   CR5000TextInst cur_text_params;
static   CR5000ArcInst  cur_arc;
static   CR5000ApertureInst   cur_aperture;

static   char        token[MAX_TOKEN];             /* Current token.       */
static   CString     cur_compname;                 /* current used component name */
static   CString     cur_padname;                  /* current used padshape name */
static   CString     cur_netname;                  /* current used netname name */

/* different pins with same name can not exist */
static   int         Push_tok = FALSE;
static   char        cur_line[MAX_LINE];
static   int         cur_new = TRUE;

static   CCompInstArray compinstarray;
static   int         compinstcnt = 0;

static	CCR5000LayerDefArray cr5000FootprintLayerArray;
static	int			cr5000FootprintLayerCount = 0;

static	CCR5000LayerDefArray cr5000BoardLayerArray;
static	int			cr5000BoardLayerCount = 0;

static   CADIFAdef   layer_attr[MAX_LAYERS];       /* Array of layers from cr5000.in   */
static   int         layer_attr_cnt;

static   CR5000Mirror   layer_mirror[MAX_LAYERS];  /* Array of layers from cr5000.in   */
static   int         layer_mirrorcnt;

static   CDrillArray drillarray;
static   int         drillcnt;

static   CAttribmapArray   attribmaparray;
static   int         attribmapcnt = 0;

typedef CArray<CCr5000Pt, CCr5000Pt&> CPolyArray;

static   int         polycnt =0 ;

static   CPolyArray  polyarray;

static   int         cur_filenum = 0;
static   FileStruct  *ActiveFile = NULL;
static   CCEtoODBDoc  *doc;
static   int         display_error = 0;

static   NetStruct   *curnetdata;
static   DataStruct  *curdata; 
static   CPoly       *curpoly;
static   TypeStruct  *curtype;
static   LayerStruct *curlayer;
static   BlockStruct *curblock;
static   double      faktor;

static   CString     LayerRange1;
static   CString     LayerRange2;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   int         IGNORE_EMPTYPADCODE_PINS;
static   int         IGNORE_OUTOFBOARD;
static	bool			PURGE_UNUSED_BLOCKS = true;
static   int         token_name;

static   CString     cur_testside;
static   int         cur_derived;   // for component padexception
static   int         cur_powernet;
static   int         page_units;
static   int         string_token = FALSE;

static   CString     cur_layerref;
static   CR5000_Version cr5000_version;
static   double      maxCenterCoordinateValue;

static void convertToComplexAperture(BlockStruct *block);

static CStringList CompOutlineLayerNameList; // name for primary comp outline layers from .in file

static bool SuppressClearancePads = true;
static bool SuppressThermalPads = true;
static bool	ShowHiddenRefdes = true; // true means ignore drawRefDes setting in cad file and always make refname a visible attribute

static CCR5000AttrSettings *AttrSettings = NULL;

static double DefaultPadApertureDia = 0.100;
static double DefaultPadDrillDia = 0.075;
static void defineDefaultPadstack(CString name, int fromLayer = -1, int toLayer = -1);
static void defineDefaultPadstackPads(BlockStruct *b, int fromLayer, int toLayer);
static void defineDefaultPadstackDrillhole(BlockStruct *b, int fromLayer, int toLayer);

static CString G_curLayerType;
static CString G_curLayerComment;
static CString G_curFootLayerName;
static CString G_curBoardLayerCategory;
static CString G_curBoardLayerName;
static int		G_curConductiveLayerNumber = 0;
static int		G_layerMapCnt = 0;
static CString	G_curLayerMapName;
static CString G_curSymbolTextType;
static CString G_curInsertedBoardID;
static bool		G_defineFootprintNow = false;
static CString	G_curFootprintName;
static FileStruct *G_curPcbFile = NULL;

static CLayerMap *MapA = NULL;
static CLayerMap *MapB = NULL;
static CLayerMap *MapThruA = NULL;
static CLayerMap *MapThruB = NULL;

static void resetPolyCnt()
{
   polycnt = 0;

#ifdef EnableVertexTracking
   CVertexEntry::reset();
#endif
}

static CString currentInputFileType()
{
   switch (G.curInputFileMode)
   {
   case CR5000_FTF_FILE:
      return "FTF File";
   case CR5000_PCF_FILE:
      return "PCF File";
   case CR5000_PNF_FILE:
      return "PNF File";
   case CR5000_NO_FILE:
      return "NO File";
   default:
      return "BOGUS File";
   }
}

static CR5000LayerDef *newCR5000LayerDef(CString layername)
{
	CR5000LayerDef *ld = new CR5000LayerDef;
	ld->boardLayerName = layername;
	//ld->boardLayerType = "CONDUCTIVE";
	//ld->comment = "";
	ld->conductive = false;
	ld->conductiveLayerNumber = -1;
	
	return ld;
}

static CR5000LayerDef *addCr5000FootLayer(CString layername)
{
	CR5000LayerDef *ld = newCR5000LayerDef(layername);

	cr5000FootprintLayerArray.SetAtGrow(cr5000FootprintLayerCount, ld);
	cr5000FootprintLayerCount++;

	return ld;
}

static CR5000LayerDef *addCr5000BoardLayer(CString layername)
{
	CR5000LayerDef *ld = newCR5000LayerDef(layername);

	cr5000BoardLayerArray.SetAtGrow(cr5000BoardLayerCount, ld);
	cr5000BoardLayerCount++;

	return ld;
}

static CR5000LayerDef *getCr5000FootLayer(CString layername)
{
	for (int i = 0; i < cr5000FootprintLayerCount; i++)
	{
		if (cr5000FootprintLayerArray[i]->footprintLayerName.Compare(layername) == 0)
			return cr5000FootprintLayerArray[i];
	}

	return NULL;
}

static CR5000LayerDef *getCr5000BestLayer(CString camcadlayername)
{
	// There are two sets of layer names in play in the cad data, there are
	// footprint layers and board layers. Most geometry in the import is based
	// on footprint definitions, and use footprint layer names. Some comes
	// from the board def, though, and uses board layer names. Now we have
	// to find camcad's layer name in the dual cad oist of layers.

	CR5000LayerDef *bestmatch = NULL;

	for (int i = 0; i < cr5000BoardLayerCount; i++)
	{
		if (cr5000BoardLayerArray[i]->boardLayerName.Compare(camcadlayername) == 0)
		{
			bestmatch = cr5000BoardLayerArray[i];

			// If it matches on footprint layer name and matches on type, that
			// is the best we will be able to do, go with it.
			//if (bestmatch->boardLayerType.CompareNoCase(bestmatch->footprintLayerType) == 0)
				return bestmatch;
		}
	}

	// If found one that matches footprint layer name, use it, even though
	// if we got here then layer type does not match.
	if (bestmatch != NULL)
		return bestmatch;


	return NULL;
}

static CR5000LayerDef *getCr5000BoardLayer(CString layername)
{
	for (int i = 0; i < cr5000BoardLayerCount; i++)
	{
		if (cr5000BoardLayerArray[i]->boardLayerName.Compare(layername) == 0)
			return cr5000BoardLayerArray[i];
	}

	return NULL;
}

static void getPositionTags(CString cr5000justify, HorizontalPositionTag *horiz, VerticalPositionTag *vert)
{
	// LO_L = Bottom Left
	// LO_C = Bottom Center
	// LO_R = Bottom Right
	// CE_L = Center Left
	// CE_C = Center Center
	// CE_R = Center Right
	// UP_L = Top Left
	// UP_C = Top Center
	// UP_R = Top Right

	*horiz = horizontalPositionLeft;
	*vert = verticalPositionBaseline;

	cr5000justify.MakeUpper();

	if (cr5000justify.Find("LO_") > -1)
		*vert = verticalPositionBottom;
	else if (cr5000justify.Find("CE_") > -1)
		*vert = verticalPositionCenter;
	else if (cr5000justify.Find("UP_") > -1)
		*vert = verticalPositionTop;

	if (cr5000justify.Find("_L") > -1)
		*horiz = horizontalPositionLeft;
	else if (cr5000justify.Find("_C") > -1)
		*horiz = horizontalPositionCenter;
	else if (cr5000justify.Find("_R") > -1)
		*horiz = horizontalPositionRight;

}

static Point2 correctCenterCoordinate(Point2 center,Point2 begin,double radius)
{
   bool xOutOfRange = (fabs(center.x) > maxCenterCoordinateValue);
   bool yOutOfRange = (fabs(center.y) > maxCenterCoordinateValue);

   if (xOutOfRange && center.y == 0. ||
       yOutOfRange && center.x == 0.     )
   {
      if (center.y == 0.)
      {
         if (center.x > 0.)
         {
            center.x = radius;
         }
         else if (center.x < 0.)
         {
            center.x = -radius;
         }
      }
      else if (center.x == 0.)
      {
         if (center.y > 0.)
         {
            center.y = radius;
         }
         else if (center.y < 0.)
         {
            center.y = -radius;
         }
      }
      else
      {
         if (center.x < 0.)
         {
            if (xOutOfRange)
            {
               //center.x = begin.x - fabs(begin.x);
               center.x = -begin.x;
            }
         }
         else if (center.x > 0.)
         {
            if (xOutOfRange)
            {
               //center.x = begin.x + fabs(begin.x);
               center.x = begin.x;
            }
         }

         if (center.y < 0.)
         {
            if (yOutOfRange)
            {
               //center.y = begin.y - fabs(begin.y);
               center.y = -begin.y;
            }
         }
         else if (center.y > 0.)
         {
            if (yOutOfRange)
            {
               //center.y = begin.y + fabs(begin.y);
               center.y = begin.y;
            }
         }
      }

      center.x += begin.x;
      center.y += begin.y;
   }
   else  
   {
      // original code result
      center.x = begin.x + radius;
      center.y = begin.y;
   }

   return center;
}

/****************************************************************************/
/*
*/
static   int   get_graphicclass(const char *layername)
{
   if (!STRCMPI(layername,"Comp-inh(plc)1")) return GR_CLASS_PLACEKEEPOUT;
   if (!STRCMPI(layername,"Comp-inh(plc)2")) return GR_CLASS_PLACEKEEPOUT;

   if (!STRCMPI(layername,"Comp-inh(via)1")) return GR_CLASS_VIAKEEPOUT;
   if (!STRCMPI(layername,"Comp-inh(via)2")) return GR_CLASS_VIAKEEPOUT;

   if (!STRCMPI(layername,"Comp-inh(wir)1")) return GR_CLASS_ROUTKEEPOUT;
   if (!STRCMPI(layername,"Comp-inh(wir)2")) return GR_CLASS_ROUTKEEPOUT;

   if (!STRCMPI(layername,"Keepout_Plc-A"))  return GR_CLASS_PLACEKEEPOUT;
   if (!STRCMPI(layername,"Keepout_Plc-B"))  return GR_CLASS_PLACEKEEPOUT;

   if (!STRCMPI(layername,"Keepout_Via-A"))  return GR_CLASS_VIAKEEPOUT;
   if (!STRCMPI(layername,"Keepout_Via-B"))  return GR_CLASS_VIAKEEPOUT;

   if (!STRCMPI(layername,"Keepout_Wir-A"))  return GR_CLASS_ROUTKEEPOUT;
   if (!STRCMPI(layername,"Keepout_Wir-B"))  return GR_CLASS_ROUTKEEPOUT;

   if (!STRCMPI(layername,"Keepout_Wir-All"))   return GR_CLASS_ROUTKEEPOUT;
   if (!STRCMPI(layername,"Keepout_Plc-All"))   return GR_CLASS_PLACEKEEPOUT;
   if (!STRCMPI(layername,"Keepout_Via-All"))   return GR_CLASS_VIAKEEPOUT;

   return 0;
}

/******************************************************************************
* ReadCR5000
*/
void ReadCR5000(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits )
{
   ActiveFile = NULL;
   doc = Doc;

   char  drive[_MAX_DRIVE];
   char  dir[_MAX_DIR];
   char  fname[_MAX_FNAME];
   char  ext[_MAX_EXT];

   char  ftf_name[_MAX_PATH];
   char  pcf_name[_MAX_PATH];
	char	pnf_name[_MAX_PATH];

   maxCenterCoordinateValue = doc->convertToPageUnits(pageUnitsInches,50.);

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );

   _makepath( ftf_name, drive, dir, fname, ".ftf" );
   _makepath( pcf_name, drive, dir, fname, ".pcf" );
	_makepath( pnf_name, drive, dir, fname, ".pnf" );
   
   faktor = Units_Factor(UNIT_TNMETER, pageunits); // redac units to page units

   page_units = pageunits;
   cur_line[0] = '\0';
   display_error = 0;
   curdata = NULL;
   curtype = NULL;
   curnetdata = NULL;
   curlayer = NULL;
   Push_tok = FALSE;
   cur_new = TRUE;
   layer_attr_cnt = 0;
   ComponentSMDrule = 0;

	DefaultPadApertureDia = doc->convertToPageUnits(pageUnitsInches, 0.100);
	DefaultPadDrillDia = doc->convertToPageUnits(pageUnitsInches, 0.075);

   CString cr5000LogFile = GetLogfilePath(CR5000ERR);

   if ((ferr = fopen(cr5000LogFile,"wt")) == NULL)
   {
     ErrorMessage("Error open file", cr5000LogFile);
     return;
   }

   CTime t;
   t = t.GetCurrentTime();
   fprintf(ferr,"Start loading file at %s\n",t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   init_all_mem();

	AttrSettings = new CCR5000AttrSettings;
   CString settingsFile( getApp().getImportSettingsFilePath("cr5000.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nCR5000 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   Cr5000LoadSettings(settingsFile);


   /* initialize scanning parameters */
   CCr5000LineNumberStamp::resetKeywordIndices();

   ActiveFile = Graph_File_Start(fname, fileTypeCr5000Layout);
   ActiveFile->setBlockType(blockTypePcb);
   ActiveFile->getBlock()->setBlockType(ActiveFile->getBlockType());
   cur_filenum = ActiveFile->getFileNumber();
	G_curPcbFile = ActiveFile;

   Graph_Level("0","",1);           // just make floating layer first.

   G.curInputFileMode = CR5000_NO_FILE;

	FileStruct *PcbFile = ActiveFile;

   //FTF file
   if ((ifp = fopen(ftf_name,"r")) == NULL)
   {
      CString  t;
      t.Format("Error opening [%s] file",ftf_name);
      ErrorMessage(t, "Error");
      return;
   }
   else
   {
      CString  f;
      f.Format("Processing %s",ftf_name);
      if (progress != NULL)
         progress->SetStatus(f);
      G.curInputFileMode = CR5000_FTF_FILE;


		rewind(ifp);
		cur_line[0] = '\0';
		ifp_line = 0;                    /* Current line number. */
		cur_filename = ftf_name;
		Push_tok = FALSE;

		delete fileReadProgress;
		fileReadProgress = new CFileReadProgress(ifp);

		if (loop_command(start_lst,SIZ_START_LST) < 0)
		{
			ErrorMessage("CR5000 read error", "Error");
		}

		fclose(ifp);
	}


   //PCF file
   if ((ifp = fopen(pcf_name,"r")) == NULL)
   {
      CString  t;
      t.Format("Error opening [%s] file",pcf_name);
      ErrorMessage(t, "Error");
      return;
   }
   else
   {
      CString  f;
      f.Format("Processing %s",ftf_name);
      if (progress != NULL)
         progress->SetStatus(f);
      G.curInputFileMode = CR5000_PCF_FILE;


		rewind(ifp);
		cur_line[0] = '\0';
		ifp_line = 0;                    /* Current line number. */
		cur_filename = pcf_name;
		Push_tok = FALSE;


		MapA = new CLayerMap("MapA", true, false); // SMD, top
		MapB = new CLayerMap("MapB", true, true);  // SMD, bottom
		MapThruA = new CLayerMap("MapAThru", false, false); // thru, top
		MapThruB = new CLayerMap("MapBThru", false, true);  // thru, bottom

		delete fileReadProgress;
		fileReadProgress = new CFileReadProgress(ifp);

		if (loop_command(start_lst,SIZ_START_LST) < 0)
		{
			// printf big prob.
			ErrorMessage("CR5000 read error", "Error");
		}

		fclose(ifp);
	}



   //PNF file
   if ((ifp = fopen(pnf_name,"r")) == NULL)
   {
		// Not an error for panel file to be absent, ignore this condition
      //CString  t;
      //t.Format("Error opening [%s] file",pcf_name);
      //ErrorMessage(t, "Error");
      //return;
   }
   else
   {
      CString  f;
      f.Format("Processing %s",ftf_name);
      if (progress != NULL)
         progress->SetStatus(f);
      G.curInputFileMode = CR5000_PNF_FILE;

		CString panelname = fname;
		panelname = "Panel_" + panelname;
		ActiveFile = Graph_File_Start(panelname, fileTypeCr5000Layout);
		ActiveFile->setBlockType(blockTypePanel);
		ActiveFile->getBlock()->setBlockType(ActiveFile->getBlockType());
		cur_filenum = ActiveFile->getFileNumber();

		// The PCB file already created will be the Shown file upon import completion.
		// Mark this Panel file as Not Shown.
		ActiveFile->setShow(FALSE);

		rewind(ifp);
		cur_line[0] = '\0';
		ifp_line = 0;                    /* Current line number. */
		cur_filename = pnf_name;
		Push_tok = FALSE;


		delete fileReadProgress;
		fileReadProgress = new CFileReadProgress(ifp);

		if (loop_command(start_lst,SIZ_START_LST) < 0)
		{
			// printf big prob.
			ErrorMessage("CR5000 read error", "Error");
		}

		fclose(ifp);
	}


   delete fileReadProgress;
   fileReadProgress = NULL;

	purge_unused_layers();

   update_smdpads(doc);
   update_smdrule_geometries(doc, ComponentSMDrule);
   update_smdrule_components(doc, PcbFile, ComponentSMDrule);

   RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.
   
   OptimizePadstacks(doc, pageunits, 1);
   generate_PADSTACKACCESSFLAG(doc, 1);
   generate_PINLOC(doc, PcbFile, 1);   // this function generates the PINLOC argument for all pins.

   // this can happen on strange via placements.
   EliminateSinglePointPolys(doc);                

  	if (PURGE_UNUSED_BLOCKS)
		doc->purgeUnusedWidthsAndBlocks(false);


   free_all_mem();

	CompOutlineLayerNameList.RemoveAll();

	delete AttrSettings;

   t = t.GetCurrentTime();
   fprintf(ferr,"End loading file at %s\n",t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   fclose(ferr);
   
   if (display_error)
      Logreader(cr5000LogFile);

   return;
}


/****************************************************************************/
/*
   Call function associated with next tolen.
   Tokens enclosed by () are searched for on the local
   token list.
*/
int go_command(List *tok_lst,int lst_size)
{
   int i, brk;

   if (!get_tok()) 
      return p_error();

   switch((brk = tok_search(brk_lst, SIZ_BRK_LST)))
   {
   case BRK_ROUND:
      {
         if (get_tok() && (i = tok_search(tok_lst, lst_size)) >= 0)
            (*tok_lst[i].function)();
         else
            fnull(); // unknown command, log and skip
      }
      break;
   case BRK_B_ROUND:
      push_tok();
      return 1;
   default:
      return p_error();
   }

   if (!get_tok())
      return FALSE;
   if (brk != tok_search(brk_lst,SIZ_BRK_LST) - 1)
      return p_error();
   return 1;
}

/****************************************************************************/
/*
   Loop through a section.
*/
static int loop_command(List *list,int size)
{
   int      repeat = TRUE;

   while (repeat)
   {
      if (go_command(list,size) < 0)
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


/******************************************************************************
* fnull
   NULL function.
   Skips over any tokens to next ) endpoint.
*/
static int fnull()
{
   int brk_count = 1; // comes in with the open bracket

//#define DEBUG_UNKNOWN_TOKEN
#ifdef DEBUG_UNKNOWN_TOKEN
   fprintf(ferr,"Token [%s] in %s at %ld unknown  (%s) (foot:%s) (net:%s)\n", token, cur_filename, ifp_line, currentInputFileType(), G_curFootprintName, cur_netname);
   display_error++;
#endif


   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name) // do not check bracket as part of token
            continue;
         
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_ROUND:
            ++brk_count;
            break;
         case BRK_B_ROUND:
            if (--brk_count == 0)
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

/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next () endpoint.
*/
static int fskip()
{
   int      brk_count = 1; // starts with an open bracket

//#define DEBUG_SKIPPED_TOKENS
#ifdef  DEBUG_SKIPPED_TOKENS
   fprintf(ferr,"Token [%s] at %ld skipped  (%s) (foot:%s) (net:%s)\n",token,ifp_line, currentInputFileType(), G_curFootprintName, cur_netname);
   display_error++;
#endif

   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   continue;   // do not check bracket as part of token  
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_ROUND:
               ++brk_count;
            break;
            case BRK_B_ROUND:
               if (--brk_count == 0)
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

/****************************************************************************/
/*
*/
static int start_ftf()
{
   return loop_command(ftf_lst,SIZ_FTF_LST);
}

/****************************************************************************/
/*
*/
static int start_pcf()
{
   return loop_command(pcf_lst,SIZ_PCF_LST);
}

/****************************************************************************/
/*
*/
static int start_pnf()
{
   return loop_command(pnf_lst,SIZ_PNF_LST);
}



/****************************************************************************/
/*
*/
static int cr5000_angle()
{
   if (!get_tok())   return p_error();   
   G.cur_rotation = atof(token);
   return 1;
}

/****************************************************************************/
/*
*/
static int ftf_header()
{
   return loop_command(ftfheader_lst,SIZ_FTFHEADER_LST);
}

/****************************************************************************/
/*
*/
static int pcf_header()
{
   return loop_command(pcfheader_lst,SIZ_PCFHEADER_LST);
}

/****************************************************************************/
/*
*/

static void applyLayerMapToPad(CLayerMap *layermap, DataStruct *oldData, BlockStruct *newBlock)
{
	if (layermap != NULL && oldData != NULL && newBlock != NULL)
	{
		InsertStruct *insert = oldData->getInsert();
		int insBlockNum = oldData->getInsert()->getBlockNumber();
		BlockStruct *insBlock = doc->getBlockAt(insBlockNum);

		if (insBlock != NULL)
		{
			if ((insBlock->getFlags() & BL_APERTURE) ||
				(insBlock->getFlags() & BL_BLOCK_APERTURE) ||
				insBlock->getBlockType() == blockTypeDrillHole)
			{
				int ftfLayerIndx = oldData->getLayerIndex();
				LayerStruct *ftflp = doc->getLayerArray()[ftfLayerIndx];
				CString ftfLayerName = ftflp->getName();

				CLayerMapEntry *me = NULL;
				if (layermap->lookup(ftfLayerName, me))
				{
					for (int namei = 0; namei < me->m_boardlayerNames.GetCount(); namei++)
					{
						CString boardLayerName = me->m_boardlayerNames.GetAt(namei);

						DataStruct *newData = doc->getCamCadData().getNewDataStruct(*oldData, true);

						int boardLayerIndx;
						int mirIndx;
						LayerStruct *brdlp = doc->FindLayer_by_Name(boardLayerName);
						if (brdlp != NULL)
						{
							boardLayerIndx = brdlp->getLayerIndex();
							mirIndx = brdlp->getMirroredLayerIndex();
						}
						else
						{
							boardLayerIndx = Graph_Level(boardLayerName, "", 0);
							mirIndx = boardLayerIndx;
						}

						newData->setLayerIndex(boardLayerIndx);
						newBlock->getDataList().AddTail(newData);

					}
				}
				else
				{
///#define DEBUG_LAYER_MAP
#ifdef  DEBUG_LAYER_MAP
					int insertedBlockNum = insert->getBlockNumber();
					BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
					fprintf(ferr, "Dropped insert. Layer (%s) not in layermap (%s). Block(%s) InsertOf(%s)(%d) (%s)\n", 
						ftfLayerName, layermap->getMapName(),
						newBlock->getName(), insertedBlock != NULL ? insertedBlock->getName() : "missing block",
						insertedBlockNum, blockTypeToString(insertedBlock != NULL ? insertedBlock->getBlockType() : blockTypeUnknown));

#endif
				}
			}
		}
	}

}
/****************************************************************************/
/*
*/

static void applyLayerMapToData(CLayerMap *layermap, DataStruct *oldData, BlockStruct *newBlock)
{
	if (layermap != NULL && oldData != NULL && newBlock != NULL)
	{
		int ftfLayerIndx = oldData->getLayerIndex();
		LayerStruct *ftflp = doc->getLayerArray()[ftfLayerIndx];
		CString ftfLayerName = ftflp->getName();

		CLayerMapEntry *me = NULL;
		if (layermap->lookup(ftfLayerName, me))
		{
			for (int namei = 0; namei < me->m_boardlayerNames.GetCount(); namei++)
			{
				CString boardLayerName = me->m_boardlayerNames.GetAt(namei);

				DataStruct *newData = doc->getCamCadData().getNewDataStruct(*oldData, true);

				int boardLayerIndx;
				int mirIndx;
				LayerStruct *brdlp = doc->FindLayer_by_Name(boardLayerName);
				if (brdlp != NULL)
				{
					boardLayerIndx = brdlp->getLayerIndex();
					mirIndx = brdlp->getMirroredLayerIndex();
				}
				else
				{
					boardLayerIndx = Graph_Level(boardLayerName, "", 0);
					mirIndx = boardLayerIndx;
				}

				newData->setLayerIndex(boardLayerIndx);
				newBlock->getDataList().AddTail(newData);

			}
		}
		else
		{
///#define DEBUG_LAYER_MAP
#ifdef  DEBUG_LAYER_MAP
			//int insertedBlockNum = insert->getBlockNumber();
			//BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
			fprintf(ferr, "Dropped Data. Layer (%s) not in layermap (%s). Block(%s)\n", 
				ftfLayerName, layermap->getMapName(),
				newBlock->getName());

#endif
		}
	}

}

/****************************************************************************/
/*
*/

static void applyLayerMapToPadstack(CLayerMap *layermap, BlockStruct *oldBlock, BlockStruct *newBlock)
{

	if (layermap != NULL && oldBlock != NULL && newBlock != NULL)
	{

		bool isSMD = false;

		POSITION pos = oldBlock->getDataList().GetHeadPosition();
		while (pos != NULL)
		{
			DataStruct *oldData = oldBlock->getDataList().GetNext(pos);
			if (oldData != NULL)
			{
				if (is_attvalue(doc, oldData->getAttributesRef(), ATT_SMDSHAPE, 1))
					isSMD = true;

				int dtype = oldData->getDataType();
				CString dtypes = dataStructTypeToString(dtype);

				if (oldData->getDataType() != dataTypeInsert)
				{
					// Just copy anything that is not an insert
					////newBlock->getDataList().AddTail(oldData);
					applyLayerMapToData(layermap, oldData, newBlock);
				}
				else
				{
					InsertStruct *insert = oldData->getInsert();

					int insBlockNum = oldData->getInsert()->getBlockNumber();
					BlockStruct *insBlock = doc->getBlockAt(insBlockNum);

					if (insBlock != NULL)
					{
						if ((insBlock->getFlags() & BL_APERTURE) ||
							(insBlock->getFlags() & BL_BLOCK_APERTURE) ||
							insBlock->getBlockType() == blockTypeDrillHole)
						{
							applyLayerMapToPad(layermap, oldData, newBlock);
						}
						else
						{
							int insertedBlockNum = insert->getBlockNumber();
							BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
							fprintf(ferr, "Dropped insert. Not an aperture. Block(%s) InsertOf(%s)(%d) (%s)\n", 
								newBlock->getName(), insertedBlock != NULL ? insertedBlock->getName() : "missing block",
								insertedBlockNum, blockTypeToString(insertedBlock != NULL ? insertedBlock->getBlockType() : blockTypeUnknown));
						}
					}

				}

			}
		}
	}
}

/****************************************************************************/
/*
*/

static void applyLayerMapsToPadstacks()
{
	// Convert from using footprint layers to board layers.
	// In some cases this means adding geometry, e.g. if a footprint layer
	// is an "inner" layer, then a copy of the geometry is made on each
	// inner board layer.

	// Process only blocks that already exist, not new blocks we'll be adding
	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *oldBlock = doc->getBlockAt(i);
		
		if (oldBlock != NULL)
		{
			bool isSMD = false;
			if (is_attvalue(doc, oldBlock->getAttributesRef(), ATT_SMDSHAPE, 1))
				isSMD = true;

			if (oldBlock->getBlockType() == blockTypePadstack)
			{
				
				CString basePadstackName = oldBlock->getName();
				basePadstackName.Replace("FTF_", ""); // get rid of footprint prefix
				CAttributes *ftfAttrs = oldBlock->getAttributesRef();

				// TOP
				CString newPadstackName = basePadstackName;
				BlockStruct *newBlock = Graph_Block_On(GBO_APPEND, newPadstackName, -1, 0);
				Graph_Block_Off();
				newBlock->setBlockType(BLOCKTYPE_PADSTACK);
				newBlock->setProperties(*oldBlock, true);
				newBlock->setName(newPadstackName);
				newBlock->getDataList().empty();

				if (isSMD)
					applyLayerMapToPadstack(MapA, oldBlock, newBlock);
				else
					applyLayerMapToPadstack(MapThruA, oldBlock, newBlock);


				// BOTTOM
				newPadstackName = basePadstackName + "_BOTTOM";
				newBlock = Graph_Block_On(GBO_APPEND, newPadstackName, -1, 0);
				Graph_Block_Off();
				newBlock->setBlockType(BLOCKTYPE_PADSTACK);
				newBlock->setProperties(*oldBlock, true);
				newBlock->setName(newPadstackName);
				newBlock->getDataList().empty();

				if (isSMD)
					applyLayerMapToPadstack(MapB, oldBlock, newBlock);
				else
					applyLayerMapToPadstack(MapThruB, oldBlock, newBlock);

			}
		}

	}

}

/****************************************************************************/
/*
*/

static void makeBottomPadstacksTopDefined()
{
	// i.e. change all the layers called out in _BOTTOM padstacks to be
	// their mirror layer

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePadstack)
		{
			if (block->getName().Find("_BOTTOM") > -1)
			{
				POSITION pos = block->getDataList().GetHeadPosition();
				while (pos != NULL)
				{
					DataStruct *data = block->getDataList().GetNext(pos);
					if (data != NULL)
					{
						LayerStruct *lp = doc->getLayer(data->getLayerIndex());
						if (lp != NULL)
							data->setLayerIndex(lp->getMirroredLayerIndex());
					}
				}
			}
		}

	}
}

/****************************************************************************/
/*
*/

static BlockStruct *findBottomPadstack(CString topPadstackName)
{
	CString bottomPadstackName = topPadstackName + "_BOTTOM";

	return doc->Find_Block_by_Name(bottomPadstackName, -1, blockTypePadstack);
}

/****************************************************************************/
/*
*/

static BlockStruct *findBottomComponentFootprint(CString topName)
{
	CString bottomName = topName + "_BOTTOM";

	return doc->Find_Block_by_Name(bottomName, -1, blockTypePcbComponent);
}

/****************************************************************************/
/*
*/
static bool blockHasThisInsert(BlockStruct *b, int layerIndx, int insertedBlockNum)
{
	if (b != NULL)
	{
		POSITION pos = b->getDataList().GetHeadPosition();
		while (pos != NULL)
		{
			DataStruct *data = b->getDataList().GetNext(pos);
			if (data != NULL && data->getDataType() == dataTypeInsert)
			{
				int lx = data->getLayerIndex();
				int ibn = data->getInsert()->getBlockNumber();

				if (lx == layerIndx && ibn == insertedBlockNum)
				{
					return true;
				}
			}
		}
	}

	return false;
}

/****************************************************************************/
/*
*/
static bool blockHasThisData(BlockStruct *b, int layerIndx, DataStruct *lookfordata)
{
	// Comparisons implemented only for data types encountered so far in sample data.
	// Belief (hope) is that importer does not generate the other types.
	// It will "fail safe", that is, an "unsupported" type will return a
	// "no match' result, causing separate top and bottome footprints to be
	// made and used, when possibly just a top footprint is needed and could
	// be mirrored. The geometry will come out right for top and bottom this way,
	// the only drawback being a possibly unneeded bottom footprint definition.

	if (b != NULL && lookfordata != NULL)
	{
		POSITION pos = b->getDataList().GetHeadPosition();
		while (pos != NULL)
		{
			DataStruct *data = b->getDataList().GetNext(pos);
			if (data != NULL && data->getDataType() == lookfordata->getDataType())
			{
				if (data->getDataType() == dataTypeInsert)
				{
					int lx = data->getLayerIndex();
					int ibn = data->getInsert()->getBlockNumber();

					if (lx == layerIndx && ibn == lookfordata->getInsert()->getBlockNumber())
					{
						return true;
					}
				}
				else if (data->getDataType() == dataTypeDraw)
				{
					fprintf(ferr, "Comparison of data type %s not implemented\n", "Draw");
					display_error++;
				}
				else if (data->getDataType() == dataTypePoly)
				{
					bool same = false;
					int lx = data->getLayerIndex();
					CPolyList *dataPL = data->getPolyList();
					CPolyList *lookPL = lookfordata->getPolyList();
					int cnt1 = dataPL->GetCount();
					int cnt2 = lookPL->GetCount();
					if (lx == layerIndx && dataPL->GetCount() == lookPL->GetCount())
					{
						same = true; // assume, and prove if otherswise
						POSITION datapolypos = dataPL->GetHeadPosition();
						POSITION lookpolypos = lookPL->GetHeadPosition();
						while (same && datapolypos && lookpolypos)
						{
							CPoly *dataPoly = dataPL->GetNext(datapolypos);
							CPoly *lookPoly = lookPL->GetNext(lookpolypos);
							if (dataPoly->getVertexCount() == lookPoly->getVertexCount())
							{
								POSITION dataPolyPos = dataPoly->getHeadVertexPosition();
								POSITION lookPolyPos = lookPoly->getHeadVertexPosition();
								while (same && dataPolyPos && lookPolyPos)
								{
									CPnt *dataPnt = dataPoly->getNextVertex(dataPolyPos);
									CPnt *lookPnt = lookPoly->getNextVertex(lookPolyPos);

									if (dataPnt->x != lookPnt->x ||
										dataPnt->y != lookPnt->y)
										same = false;
								}
							}
						}
					}

					if (same)
						return true;
				}
				else if (data->getDataType() == dataTypeText)
				{
					int lx = data->getLayerIndex();
					TextStruct *dataTxt = data->getText();
					TextStruct *lookTxt = lookfordata->getText();
					CString dataTxtStr = dataTxt->getText();
					CString lookTxtStr = lookTxt->getText();
					if (lx == layerIndx && dataTxtStr.Compare(lookTxtStr) == 0)
						return true;
				}
				else if (data->getDataType() == dataTypePoint)
				{
					fprintf(ferr, "Comparison of data type %s not implemented\n", "Point");
					display_error++;
				}
				else if (data->getDataType() == dataTypeBlob)
				{
					fprintf(ferr, "Comparison of data type %s not implemented\n", "Blob");
					display_error++;
				}
				else
				{
					// data type unknown
					fprintf(ferr, "Comparison of data type %s not implemented\n", "Unknown");
					display_error++;
				}
			}
		}
	}

	return false;
}

/****************************************************************************/
/*
*/

static void eliminateRedundantPadstacks()
{
	// Redundant padstacks are _BOTTOM padstacks that are same as
	// their top padstack counterpart

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePadstack)
		{
			if (block->getName().Find("FTF_") < 0  &&		// skip FootprinTFile padstacks
				block->getName().Find("_BOTTOM") < 0)		// skip bottom padstacks
			{
				// So we should have a top padstack, now get the matching bottom
				BlockStruct *bottomPS = findBottomPadstack(block->getName());
				if (bottomPS != NULL)
				{
					bool same = true;
					POSITION pos = block->getDataList().GetHeadPosition();
					while (pos != NULL && same)
					{
						DataStruct *data = block->getDataList().GetNext(pos);
						if (data != NULL && data->getDataType() == dataTypeInsert)
						{
							int layerIndx = data->getLayerIndex();
							int insertedBlockNum = data->getInsert()->getBlockNumber();

							if (!blockHasThisInsert(bottomPS, layerIndx, insertedBlockNum))
							{
								same = false;
							}
						}
					}

					if (same)	// then we don't need both
					{
						doc->RemoveBlock(bottomPS);
					}
				}
			}
		}
	}
}

/****************************************************************************/
/*
*/

static void purgeFootprintPadstacks()
{
	// We created both top side and (if needed) bottom side padstacks using
	// the footprint padstacks as templates. When finished with that operation
	// the footprint padstacks are just extra geometry in the file.

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePadstack)
		{
			if (block->getName().Find("FTF_") == 0) // footprint padstack begins with FTF_
			{
				doc->RemoveBlock(block);
			}
		}
	}
}

/****************************************************************************/
/*
*/

static bool isSMDComp(BlockStruct *block)
{
	// Depends on kind of padstacks in pins.
	// If it has at least one pin that is insert of smd padstack we'll call it smd.

	if (block != NULL  && block->getBlockType() == blockTypePcbComponent)
	{
		POSITION pos = block->getDataList().GetHeadPosition();
		while (pos != NULL)
		{
			DataStruct *data = block->getDataList().GetNext(pos);
			if (data != NULL && data->getDataType() == dataTypeInsert)
			{
				if (data->getInsert()->getInsertType() == insertTypePin)
				{
					int insBlockNum = data->getInsert()->getBlockNumber();
					BlockStruct *insBlock = doc->getBlockAt(insBlockNum);

					if (is_attvalue(doc, insBlock->getAttributesRef(), ATT_SMDSHAPE, 1))
						return true;
				}
			}
		}
	}
	return false;
}

/****************************************************************************/
/*
*/
static bool requiresBottomDefinition(BlockStruct *block /*pcb component*/)
{
	// A bottom definition is required if any inserted padstack has
	// a bottom define counterpart.
	// Or, if there is geometry defined on both top and bottom layers.

#define ALWAYS_MAKE_BOTTOM
#ifdef  ALWAYS_MAKE_BOTTOM
	// The stuff on the other side of ifdef seems to work, but there is the
	// posibility of a mistake in that complexity. It seems to work fine to
	// just always make the bottom def and then weed out the redundancies later.
	return true;
#else
	if (block != NULL)
	{
		bool isSMD = isSMDComp(block);

		POSITION pos = block->getDataList().GetHeadPosition();
		while (pos != NULL)
		{
			DataStruct *data = block->getDataList().GetNext(pos);
			if (data != NULL)
			{
				int layindx = data->getLayerIndex();
				LayerStruct *lp = doc->getLayer(layindx);
				CString layername = lp->getName();
				int miridx = lp->getMirroredLayerIndex();

				if (data->getDataType() == dataTypeInsert)
				{
					int insertedBlockNum = data->getInsert()->getBlockNumber();
					BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
					if (insertedBlock != NULL)
					{
						CString insertedName = insertedBlock->getName();
						BlockStruct *bottomBlock = findBottomPadstack(insertedName);
						if (bottomBlock != NULL)
						{
							// Padstack has bottom definition
							return true;
						}
					}
				}
				
				CLayerMap *topMap = NULL;
				CLayerMap *botMap = NULL;
				if (isSMD)
				{
					topMap = MapA;
					botMap = MapB;
				}
				else
				{
					topMap = MapThruA;
					botMap = MapThruB;
				}

				bool hasTop = false;
				bool hasBot = false;

				CLayerMapEntry *meT = NULL;
				CLayerMapEntry *meB = NULL;
				if (topMap->lookup(layername, meT))
				{
					hasTop = true;
				}
				if (botMap->lookup(layername, meB))
				{
					hasBot = true;
				}

				// If mapped for one side and not other, separate definitions are required
				if (hasTop != hasBot)
					return true;

				// We know now they are not equal, so if one is not mapped, neither is
				// the other, and we do not need separate definitions
				if (!hasTop)
					return false;

				// Still here, so has both top and bot mappings.
				// If bot is not mirror layer of top, separate definitions are required.
				// Layers in meT must have 1:1 correspondence with a mirror layer in meB.
				if (meT->m_boardlayerNames.GetCount() == meB->m_boardlayerNames.GetCount())
				{
					for (int meTi = 0; meTi < meT->m_boardlayerNames.GetCount(); meTi++)
					{
						CString tname =  meT->m_boardlayerNames.GetAt(meTi);
						LayerStruct *tlp = doc->FindLayer_by_Name(tname);
						bool found = false;
						for (int meBi = 0; meBi < meB->m_boardlayerNames.GetCount(); meBi++)
						{
							CString bname =  meB->m_boardlayerNames.GetAt(meBi);
							LayerStruct *blp = doc->FindLayer_by_Name(bname);

							if (tlp && blp && tlp->getMirroredLayerIndex() == blp->getLayerIndex())
								found = true;
						}
						if (!found)
							return true; // non-mirror match, requires separate definitions

					}
				}
			}
		}
	}

	return false;
#endif
}

/****************************************************************************/
/*
*/

static void createBottomCompFootprints()
{
	// A footprint is the whole part.
	// A toeprint is the pin set.
	// These are terms/keywords in the cad data, I didn't make this up.

	// If a footprint has a toeprint that references a padstack that has
	// a bottom definition, then we need to make a "bottom" footprint
	// counterpart to the top footprint.

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{
			if (requiresBottomDefinition(block))
			{
				CString newBlockName = block->getName() + "_BOTTOM";
				BlockStruct *newBlock = Graph_Block_On(GBO_APPEND, newBlockName, -1, 0);
				Graph_Block_Off();
				newBlock->setBlockType(blockTypePcbComponent);
				newBlock->setProperties(*block, true);
				newBlock->setName(newBlockName);

				POSITION pos = newBlock->getDataList().GetHeadPosition();
				while (pos != NULL)
				{
					DataStruct *data = newBlock->getDataList().GetNext(pos);
					if (data != NULL && data->getDataType() == dataTypeInsert)
					{
						int insertedBlockNum = data->getInsert()->getBlockNumber();
						BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
						if (insertedBlock != NULL)
						{
							BlockStruct *bottomBlock = findBottomPadstack(insertedBlock->getName());
							if (bottomBlock != NULL)
							{
								data->getInsert()->setBlockNumber(bottomBlock->getBlockNumber());
							}
						}
					}
				}
			}
		}
	}
}

/****************************************************************************/
/*
*/

static void updateComponentToeprints()
{
	// Toeprints, that's the keyword cr5000 cad uses for the collection of pins in
	// the component footprint. The "raw" footprints are referencing "FTF" toeprint padstacks 
	// in the inserts. Update these to use the "board" padstacks.

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{
			POSITION pos = block->getDataList().GetHeadPosition();
			while (pos != NULL)
			{
				DataStruct *data = block->getDataList().GetNext(pos);
				if (data != NULL && data->getDataType() == dataTypeInsert)
				{
					int insertedBlockNum = data->getInsert()->getBlockNumber();
					BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
					if (insertedBlock != NULL)
					{
						CString insertedName = insertedBlock->getName();
						if (insertedName.Find("FTF_") == 0)
						{
							insertedName.Replace("FTF_", "");
							BlockStruct *updateBlock = doc->Find_Block_by_Name(insertedName, -1);
							if (updateBlock != NULL)
							{
								data->getInsert()->setBlockNumber(updateBlock->getBlockNumber());
							}
						}
					}
				}
			}
		}
	}
}


/****************************************************************************/
/*
*/

static GraphicClassTag getLayerGraphicClass(LayerStruct *lp)
{
   if (lp != NULL)
   {
      if (CompOutlineLayerNameList.Find(lp->getName()))
         return graphicClassComponentOutline;


      LayerTypeTag layerType = lp->getLayerType();

      if (layerType == layerTypeComponentOutline)
         return graphicClassComponentOutline;

      if (layerType == layerTypeBoardOutline)
         return graphicClassBoardOutline;

      if (layerType == layerTypePanelOutline)
         return graphicClassPanelOutline;
   }

   return graphicClassUndefined;
}

/****************************************************************************/
/*
*/

static void adjustFootprintGraphicClass()
{
	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{

			POSITION pos = block->getDataList().GetTailPosition();
			while (pos != NULL)
			{
				DataStruct *data = block->getDataList().GetPrev(pos);
				if (data != NULL && data->getDataType() != dataTypeInsert)
            {
               int layerIndx = data->getLayerIndex();
               LayerStruct *lp = doc->getLayerAt(layerIndx);
               GraphicClassTag newClass = getLayerGraphicClass(lp);
               if (newClass != graphicClassUndefined)
               {
                  GraphicClassTag oldClass = data->getGraphicClass();
                  data->setGraphicClass(newClass);
               }            
            }
         }
      }
   }
}

/****************************************************************************/
/*
*/

static void applyLayerMapsToCompFootprints()
{
	// This is for updating anything in the component that is not an insert.
	// E.g. the poly list.

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{
			bool isSMD = isSMDComp(block);

			CString blockname = block->getName();
			bool isBottom = false;
			if (blockname.Find("_BOTTOM") > -1)
				isBottom = true;

			CLayerMap *layermap = NULL;
			if (isSMD)
			{
				if (isBottom)
					layermap = MapB;
				else
					layermap = MapA;
			}
			else
			{
				if (isBottom)
					layermap = MapThruB;
				else
					layermap = MapThruA;
			}

			POSITION pos = block->getDataList().GetTailPosition();
			while (pos != NULL)
			{
				DataStruct *newData = NULL;
				DataStruct *oldData = block->getDataList().GetPrev(pos);
            if (oldData != NULL && !oldData->isInsertType(insertTypePin)) //oldData->getDataType() != dataTypeInsert)
				{
					int ftfLayerIndx = oldData->getLayerIndex();
					LayerStruct *ftflp = doc->getLayer(ftfLayerIndx);
					CString ftfLayerName = ftflp != NULL ? ftflp->getName() : "random hopefully invalid layer name";

					CLayerMapEntry *me = NULL;
					if (layermap->lookup(ftfLayerName, me))
					{
						for (int namei = 0; namei < me->m_boardlayerNames.GetCount(); namei++)
						{
							CString boardLayerName = me->m_boardlayerNames.GetAt(namei);

							if (namei > 0)
							{
								// Footprint layer maps to more than one board layer, dup data for new layer
								newData = doc->getCamCadData().getNewDataStruct(*oldData, true);
							}

							int boardLayerIndx;
							int mirIndx;
							LayerStruct *brdlp = doc->FindLayer_by_Name(boardLayerName);
							if (brdlp != NULL)
							{
								boardLayerIndx = brdlp->getLayerIndex();
								mirIndx = brdlp->getMirroredLayerIndex();
							}
							else
							{
								boardLayerIndx = Graph_Level(boardLayerName, "", 0);
								mirIndx = boardLayerIndx;
							}

							if (newData != NULL)
							{
								newData->setLayerIndex(boardLayerIndx);
								block->getDataList().AddTail(newData);
							}
							else
							{
								oldData->setLayerIndex(boardLayerIndx);
							}

						}
					}
					else
					{
						// layer not mapped, delete datum
						//block->RemoveDataFromList(doc, oldData);
                  doc->removeDataFromDataList(*block,oldData);
					}
				}
				else
				{
					// is insert
					// example data had only inserts of pins, only insert of pins
					// are handled, flag if we encounter something else.
					// Layer maps are applied pins separately from this, leave pins alone here.
					int instype = oldData->getInsert()->getInsertType();
					CString inserttype = insertTypeToString(instype);
					
					int ftfLayerIndx = oldData->getLayerIndex();
					LayerStruct *ftflp = doc->getLayer(ftfLayerIndx);
					CString ftfLayerName = ftflp ? ftflp->getName() : "unknown";

					if (instype != insertTypePin)
					{
						fprintf(ferr, "Component layer mapping error, insert type (%s) on layer (%s)\n", inserttype, ftfLayerName);
						display_error++;
					}
				}
			}
		}
	}
}

/****************************************************************************/
/*
*/

static void makeBottomCompFootprintsTopDefined()
{

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{
			if (block->getName().Find("_BOTTOM") > -1)
			{
				POSITION pos = block->getDataList().GetHeadPosition();
				while (pos != NULL)
				{
					DataStruct *data = block->getDataList().GetNext(pos);
					if (data != NULL)
					{
						int datatype = data->getDataType();
						CString datatypestr = dataStructTypeToString(datatype);

						LayerStruct *lp = doc->getLayer(data->getLayerIndex());
						CString layername = lp->getName();

						if (lp != NULL)
							data->setLayerIndex(lp->getMirroredLayerIndex());
					}
				}
			}
		}

	}
}

/****************************************************************************/
/*
*/

static void eliminateRedundantCompFootprints()
{

	int blockCnt = doc->getMaxBlockIndex();
   for (int i = 0; i < blockCnt; i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{
			if (block->getName().Find("FTF_") < 0  &&		// skip FootprinTFile padstacks
				block->getName().Find("_BOTTOM") < 0)		// skip bottom padstacks
			{
				// So we should have a top component footprint, now get the matching bottom
				BlockStruct *bottomFP = findBottomComponentFootprint(block->getName());
				if (bottomFP != NULL)
				{
					bool same = true;
					POSITION pos = block->getDataList().GetHeadPosition();
					while (pos != NULL /*&& same*/)
					{
						DataStruct *data = block->getDataList().GetNext(pos);
						if (data != NULL /*&& data->getDataType() == dataTypeInsert*/)
						{
							int layerIndx = data->getLayerIndex();
							int insertedBlockNum = data->getInsert()->getBlockNumber();

							if (!blockHasThisData(bottomFP, layerIndx, data))
							{
								same = false;
							}
						}
					}

					if (same)	// then we don't need both
					{
						doc->RemoveBlock(bottomFP);
					}
				}
			}
		}
	}
}

/****************************************************************************/
/*
*/

static void convert_footprints()
{
	applyLayerMapsToPadstacks();
	makeBottomPadstacksTopDefined();
	eliminateRedundantPadstacks();

	updateComponentToeprints();	// change FTF padstack references to board padstack refs
	createBottomCompFootprints();
	applyLayerMapsToCompFootprints();
   adjustFootprintGraphicClass();
	makeBottomCompFootprintsTopDefined();
	eliminateRedundantCompFootprints();

	purgeFootprintPadstacks();
}

/****************************************************************************/
/*
*/
static int pcf_technologyContainer()
{
   int val = loop_command(pcftechnologyContainer_lst,SIZ_PCFTECHNOLOGYCONTAINER_LST);

	// Register the board layers found in cad, i.e. make them real in ccz.
	// Then set mirroring. We want all layers legit for second part of processing.
	// Unused layers will be removed at end of import.
	///////inherit_footprint_layer_types();  doesn't seem to improve anything
	register_cad_layers();
	update_mirrorlayers();
	convert_footprints();

	return val;
}

/****************************************************************************/
/*
*/
static int ftf_technologyContainer()
{
	// ignore this, the pcf tech container has what we want
   fskip();
   return 1;
   //return loop_command(ftftechnologyContainer_lst,SIZ_FTFTECHNOLOGYCONTAINER_LST);
}

/****************************************************************************/
/*
*/
static int pcf_boardContainer()
{
   return loop_command(pcfboardContainer_lst,SIZ_PCFBOARDCONTAINER_LST);
}

/****************************************************************************/
/*
*/
static int pnf_boardContainer()
{
   return loop_command(pnfboardContainer_lst,SIZ_PNFBOARDCONTAINER_LST);
}

/******************************************************************************
*/
static bool checkReferencedBlock(CString blname, CString bltype = "", bool logMsg = true)
{
	// For checking that a block that is giong to be referenced in an insert
	// actually does exist. If it does not exist, log message in log file.

	if (!blname.IsEmpty())
	{
		if (doc->Find_Block_by_Name(blname, -1) == NULL)
		{
         if (logMsg)
         {
			   fprintf(ferr, "Undefined block referenced: %s", blname);
			   if (!bltype.IsEmpty())
				   fprintf(ferr, " (%s)\n", bltype);
         }

			return false;
		}

		return true; // Okay
	}

	return false; // Empty name is not okay
}

/******************************************************************************
* layer_fpad
*/
static int layer_fpad()
{
   if (!get_tok())
      return p_error();

   CString padName = token;

   G.name[0] = '\0';
   polyarray.RemoveAll();
   resetPolyCnt();
   G.cur_rotation = 0;

   CCr5000LineNumberStamp lineNumberStamp;

   int res = loop_command(ftffpad_lst, SIZ_FTFFPAD_LST);

   if (polycnt)
   {
      cur_pin.padstackGeomName = "PADST_PAD_" + padName;
      cur_pin.x = cur_pin.padstackx = polyarray[0].x;
      cur_pin.y = cur_pin.padstacky = polyarray[0].y;
      cur_pin.padrotation = G.cur_rotation;
      cur_pin.pinName = "FreePad";

      cur_pin.padstackGeomName = "USR_PAD_" + padName;
	   if (!checkReferencedBlock(cur_pin.padstackGeomName, "pad", false))
				   cur_pin.padstackGeomName = "PADST_PAD_" + padName;
      if (!checkReferencedBlock(cur_pin.padstackGeomName, "pad", false))
				   cur_pin.padstackGeomName = "PAD_" + padName;

      if (!checkReferencedBlock(cur_pin.padstackGeomName, "pad"))
				   defineDefaultPadstack(cur_pin.padstackGeomName);

      DataStruct *insertData = lineNumberStamp.graphBlockReference(cur_pin.padstackGeomName, NULL /*cur_pin.pinName*/, 0, 
            cur_pin.x, cur_pin.y, DegToRad(cur_pin.padrotation), 0, 1.0, 
            G.cur_layerindex /*Graph_Level("0", "", 1)*/, TRUE);
   }
   else
   {
      // error
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 0;
}

/******************************************************************************
* ftffootprint_heelprint
*/
static int ftffootprint_heelprint()
{
   return loop_command(ftfheelprint_lst, SIZ_FTFHEELPRINT_LST);
}

/****************************************************************************/
/*
*/
static int ftfpad_geometry()
{ 
   G.cur_layerindex = Graph_Level("0","", 1);   // floating layer
	G.cur_geomcnt = 0;
   return loop_command(padgeometry_lst,SIZ_PADGEOMETRY_LST);
}

/****************************************************************************/
/*
*/
static int ftfhole_geometry()
{ 
   return loop_command(holegeometry_lst,SIZ_HOLEGEOMETRY_LST);
}

/******************************************************************************
* ftffootprint_toeprint
*/
static int ftffootprint_toeprint()
{
   return loop_command(ftftoeprint_lst, SIZ_FTFTOEPRINT_LST);
}

/******************************************************************************
* ftfheelprint_layout
*/
static int ftfheelprint_layout()
{
   return loop_command(ftfheelprintlayout_lst, SIZ_FTFHEELPRINTLAYOUT_LST);
}


/******************************************************************************
* ftftoeprint_pin
*  - pin under toeprint under footprint in ftf
*  - this is where pins get added to component geometries
*/
static int ftftoeprint_pin()
{
   if (!get_tok())
      return p_error();

   // init pinstruct
   cur_pin.pinName = token;
   cur_pin.x = 0.0;
   cur_pin.y = 0.0;
   cur_pin.padstackGeomName = "NOPIN";
   cur_pin.padstackx = 0.0;
   cur_pin.padstacky = 0.0;
   cur_pin.padrotation = 0.0;

   CCr5000LineNumberStamp lineNumberStamp;

   int res = loop_command(ftftoeprintpin_lst, SIZ_FTFTOEPRINTPIN_LST);

   // write pin
	if (!checkReferencedBlock(cur_pin.padstackGeomName, "padstack"))
				defineDefaultPadstack(cur_pin.padstackGeomName);
   DataStruct *pinInsert = lineNumberStamp.graphBlockReference(cur_pin.padstackGeomName, cur_pin.pinName, 0, 
         cur_pin.x, cur_pin.y, DegToRad(cur_pin.padrotation), 0, 1.0, Graph_Level("0", "", 1), TRUE);
   
	pinInsert->getInsert()->setInsertType(insertTypePin);

   G.cur_pincnt++;

   return 1;
}

/****************************************************************************/
/*
*/
static int ftf_footprintContainer()
{
   return loop_command(ftffootprintContainer_lst,SIZ_FTFFOOTPRINTCONTAINER_LST);
}

/****************************************************************************/
/*
*/
static int pcfboard_boardlayout()
{
   return loop_command(pcfboard_boardlayout_lst,SIZ_PCFBOARD_BOARDLAYOUT_LST);
}

/****************************************************************************/
/*
*/
static int pnfboard_boardlayout()
{
   return loop_command(pnfboard_boardlayout_lst,SIZ_PNFBOARD_BOARDLAYOUT_LST);
}

/****************************************************************************/
/*
*/
static int pcfboard_components()
{
   return loop_command(pcfboard_components_lst,SIZ_PCFBOARD_COMPONENTS_LST);
}

/****************************************************************************/
/*
*/
static int pcfboard_nets()
{
   return loop_command(pcfboard_nets_lst,SIZ_PCFBOARD_NETS_LST);
}

/****************************************************************************/
/*
*/
static int pcfboardlayout_layout()
{
   return loop_command(pcfboardlayout_lst,SIZ_PCFBOARDLAYOUT_LST);
}

/****************************************************************************/
/*
*/
static int pnfboardlayout_layout()
{
   return loop_command(pnfboardlayout_lst,SIZ_PNFBOARDLAYOUT_LST);
}

/******************************************************************************
* ftfpin_layout
*/
static int ftfpin_layout()
{
   return loop_command(ftfpinlayout_lst, SIZ_FTFPINLAYOUT_LST);
}

/****************************************************************************/
/*
*/
static int ftf_footprints()
{
   int res = loop_command(ftffootprints_lst,SIZ_FTFFOOTPRINTS_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int ftf_pads()
{
   int res = loop_command(ftfpads_lst,SIZ_FTFPADS_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int ftf_padstacks()
{
   int res = loop_command(ftfpadstacks_lst,SIZ_FTFPADSTACKS_LST);
   return res;
}

/******************************************************************************
* ftf_footprint
*/
static int ftf_footprint()
{
   if (!get_tok())
      return p_error();

   CString footprint = token;
   G_curFootprintName = token;

   BlockStruct *compGeom = Graph_Block_On(GBO_APPEND, footprint, -1, 0);
	compGeom->setBlockType(blockTypePcbComponent);

   G.cur_pincnt = 0;

   int res = loop_command(ftffootprint_lst, SIZ_FTFFOOTPRINT_LST);

	if (G.cur_pincnt == 0 && compGeom->getBlockType() == blockTypePcbComponent)
      compGeom->setBlockType(blockTypeGenericComponent);

   Graph_Block_Off();

   return res;
}

/****************************************************************************/
/*
*/
static int ftf_pad()
{
   if (!get_tok())   return p_error();  
   sprintf(G.name, "PAD_%s", token);

   G.cur_pincnt = 0; // this is used to count the apertures defined.
                     // cr5000 allows to defined multiple apertures in 1 padshape

   int res = loop_command(ftfpad_lst,SIZ_FTFPAD_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadstackgroup_padstack()
{
   if (!get_tok())   return p_error();  
   strcpy(G.name,  token);
   return 1;
}

/****************************************************************************/
/*
*/
static int ftfpadstack_padset()
{
   int res = loop_command(ftfpadset_lst,SIZ_FTFPADSET_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadstack_hole()
{
   int res = loop_command(ftfhole_lst,SIZ_FTFHOLE_LST);

   return res;
}

static int pcf_hole()
{
   Graph_Block_On(ActiveFile->getBlock());
   int res = loop_command(ftfhole_lst,SIZ_FTFHOLE_LST);
   Graph_Block_Off();

   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadset_connect()
{
   CCr5000LineNumberStamp lineNumberStamp;

   int res = loop_command(ftfpadset_connect_lst,SIZ_FTFPADSET_CONNECT_LST);

	checkReferencedBlock(G.name);
   DataStruct* data = lineNumberStamp.graphBlockReference(G.name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, G.cur_layerindex, TRUE);

   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadset_clearance()
{
	int res = 1;

	if (SuppressClearancePads)
	{
		fskip();
	}
	else
	{
      CCr5000LineNumberStamp lineNumberStamp;

		res = loop_command(ftfpadset_clearance_lst,SIZ_FTFPADSET_CLEARANCE_LST);

		checkReferencedBlock(G.name);
		DataStruct *d = lineNumberStamp.graphBlockReference(G.name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, G.cur_layerindex, TRUE);
		d->getInsert()->setInsertType(insertTypeClearancePad);
	}


   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadset_thermal()
{
	int res = 1;

	if (SuppressThermalPads)
	{
		fskip();
	}
	else
	{
      CCr5000LineNumberStamp lineNumberStamp;

		res = loop_command(ftfpadset_thermal_lst,SIZ_FTFPADSET_THERMAL_LST);

		checkReferencedBlock(G.name);
		DataStruct *d = lineNumberStamp.graphBlockReference(G.name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, G.cur_layerindex, TRUE);
		d->getInsert()->setInsertType(insertTypeThermalPad);
	}

   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadset_pad_connect()
{
   if (!get_tok())   return p_error();  
   sprintf(G.name, "PAD_%s", token);

   return 1;
}

/****************************************************************************/
/*
*/
static int ftfpadset_pad_clearance()
{
   if (!get_tok())   return p_error();  
   sprintf(G.name, "PAD_%s", token);

   return 1;
}

/****************************************************************************/
/*
*/
static int ftfpadset_pad_thermal()
{
   if (!get_tok())   return p_error();  
   sprintf(G.name, "PAD_%s", token);

   return 1;
}

/******************************************************************************
* ftf_padstack
*/
static int ftf_padstack()
{
   if (!get_tok())
      return p_error();

   CString padstack;
   padstack.Format("FTF_PADST_%s", token);

   curblock = Graph_Block_On(GBO_APPEND, padstack, -1, 0);
	curblock->setBlockType(blockTypePadstack);


   int res = loop_command(ftfpadstack_lst, SIZ_FTFPADSTACK_LST);
   Graph_Block_Off();
   curblock = NULL;

   return res;
}

/****************************************************************************/
/*
*/
static int ftfpadstack_throughmode()
{
   if (!get_tok())   return p_error();  

   if (!STRCMPI(token, "NONTHROUGH"))
   {
      doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, 0),
         VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
   }

   return 1;
}


/****************************************************************************/
/*
*/
static int ftf_padstackgroup()
{
   if (!get_tok())   return p_error();  

   int res = loop_command(ftfpadstackgroup_lst,SIZ_FTFPADSTACKGROUP_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcfcomponents_component()
{
   // init component struct
   CR5000CompInst *c = new CR5000CompInst;
   compinstarray.SetAtGrow(compinstcnt,c);
   c->reference = "";
   c->gateCount = 0;
   c->pinCount = 0;
   c->part = "";
   c->package = "";
   c->footprint = "";
   c->originalReference = "";
   c->placed = FALSE;
   c->placementSide = 0;      // 0 is top, 1 bottom
   c->locationLock = 0;
   c->angleLock = 0;
   c->placementSideLock = 0;
   c->packageSymbol = 0;
   c->outOfBoard = 0;
   //Point2 outOfBoardLocation;  <== dependend on outOfBoard
   //Point2 location;            <== dependend on placed
   c->angle = 0;
   c->drawRefDes = 0;

	// Refdes text params
	cur_text_params.layerIndex = -1;
	cur_text_params.x = 0.0;
	cur_text_params.y = 0.0;
	cur_text_params.angle = 0.0;
	cur_text_params.height = doc->convertToPageUnits(pageUnitsInches, 1./32.);
	cur_text_params.width = cur_text_params.height * 0.60;

	G_defineFootprintNow = false;  // most footprint already came from FTF file, but some do not, e.g. Fid geom for PNL file

   CCr5000LineNumberStamp lineNumberStamp;

   int res = loop_command(pcfcomponent_lst,SIZ_PCFCOMPONENT_LST);

   if (IGNORE_OUTOFBOARD && c->outOfBoard)
   {
      fprintf(ferr,"OutOfBoard Component [%s] ignored.\n", compinstarray[compinstcnt]->reference);
      display_error++;
   }
   else
   {
		CString footprint = compinstarray[compinstcnt]->footprint;
		CString rawFootprint = footprint;
		if (compinstarray[compinstcnt]->placementSide == 1 /*bottom*/)
		{
			CString bottomfootprint = footprint + "_BOTTOM";
			if (doc->Find_Block_by_Name(bottomfootprint, -1) != NULL)
				footprint = bottomfootprint;
		}
   
      // place component
		checkReferencedBlock(footprint, "component");
      DataStruct *d = lineNumberStamp.graphBlockReference(
         footprint, 
         compinstarray[compinstcnt]->reference, -1, 
         compinstarray[compinstcnt]->location.x, 
         compinstarray[compinstcnt]->location.y, 
         DegToRad(compinstarray[compinstcnt]->angle), 
         compinstarray[compinstcnt]->placementSide , 1.0, -1, TRUE);

		InsertTypeTag insertType = insertTypePcbComponent;
		insertType = AttrSettings->GetInsertType(rawFootprint, insertType);

		CString refname = compinstarray[compinstcnt]->reference;
		insertType = AttrSettings->GetInsertType(refname, insertType);
		d->getInsert()->setInsertType(insertType);

      if (insertType == insertTypeTestPoint)
      {
         // If the geometry name is said to use as testpoint then neet to add the "TEST" attribute on it
         CString value =  "";
         d->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeTest), valueTypeString, value.GetBuffer(0), attributeUpdateFenced, NULL);
      }

		
		// set refname attrib
		int visible = compinstarray[compinstcnt]->drawRefDes;
		if (ShowHiddenRefdes)
			visible = true;

		int layerindex = cur_text_params.layerIndex >= 0 ? cur_text_params.layerIndex : Graph_Level("0","",1);
   
		if (layerindex >= 0 && compinstarray[compinstcnt]->placementSide == 1 /*bottom*/)
		{
			// Bottom side; cad file is "face value", camcad data is "top defined", so
			// need to get mirror of layer in cad file for camcad usage.
			LayerStruct* facevaluelayer = doc->getLayerAt(layerindex);
			if (facevaluelayer != NULL)
				layerindex = facevaluelayer->getMirroredLayerIndex();			
		}

		HorizontalPositionTag horiz;
		VerticalPositionTag vert;
		getPositionTags(cur_text_params.justify, &horiz, &vert);

		double attrx = cur_text_params.x; 
		double attry = cur_text_params.y;
		double attrTheta = cur_text_params.angle;

		// If they are zero, then they probably were not set in a text param block.
		// If they are not zero, then they are absolute, so make them relative to
		// component insert.
		if (attrx != 0.0 || attry != 0.0)
		{
			attrx = attrx - compinstarray[compinstcnt]->location.x;
			attry = attry - compinstarray[compinstcnt]->location.y;

			// Refdes coords are absolute, so delta so far is absolute.
			// Make them relative. Rotate opposite direction of component insert.
			if (compinstarray[compinstcnt]->angle != 0.0)
			{
				double rawdx = attrx;
				double rawdy = attry;

				Rotate(rawdx, rawdy,  - compinstarray[compinstcnt]->angle, &attrx, &attry);
			}

			// Make text angle relative to component insert
			attrTheta = attrTheta - compinstarray[compinstcnt]->angle;

			if (compinstarray[compinstcnt]->placementSide)  // is on bottom
			{
				// the usual mirror thing
				attrx = -attrx;
				attrTheta = 360.0 - attrTheta;
			}
		}

		int penWidthIndex = Graph_Aperture("", T_ROUND, cur_text_params.strokeWidth, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);

		doc->SetUnknownVisAttrib(&d->getAttributesRef(), ATT_REFNAME, 
         compinstarray[compinstcnt]->reference,
			attrx, attry, DegToRad(attrTheta), cur_text_params.height, cur_text_params.width, 
			0, penWidthIndex, visible, SA_OVERWRITE, 0L, layerindex, 0, horiz, vert);

      BlockStruct *b = doc->Find_Block_by_Name(compinstarray[compinstcnt]->footprint, -1);

      if (strlen(compinstarray[compinstcnt]->part))
      {
         // add a device
         TypeStruct *curtype = AddType(ActiveFile, compinstarray[compinstcnt]->part);
   
         // here update compdef list
         if (curtype->getBlockNumber() > -1 && curtype->getBlockNumber() != b->getBlockNumber())
         {
            BlockStruct *already_assigned = doc->Find_Block_by_Num(curtype->getBlockNumber());
            fprintf(ferr,"Try to assign Pattern [%s] to Device [%s] -> already a different Pattern [%s] assigned!\n",
               b->getName(), compinstarray[compinstcnt]->reference, already_assigned->getName());
            display_error++;

            CString  typname;
            typname.Format("%s_%s", b->getName(), compinstarray[compinstcnt]->reference );
            curtype = AddType(ActiveFile, typname);
      
            curtype->setBlockNumber( b->getBlockNumber());

            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TYPELISTLINK, typname,
               SA_OVERWRITE, NULL); // x, y, rot, height

         }
         else
         {
            curtype->setBlockNumber( b->getBlockNumber());
            doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_TYPELISTLINK, compinstarray[compinstcnt]->part,
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
      }
   }

   compinstcnt++;

   return res;
}

/****************************************************************************/
/*
*/
static int pcfnets_net()
{
   if (!get_tok())   p_error();  //  
   cur_netname = token;

   NetStruct *n = add_net(ActiveFile,cur_netname);

   int res = loop_command(pcfnet_lst,SIZ_PCFNET_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcf_technology()
{
   CString  technologyname;

   if (!get_tok())   p_error();  //  
   technologyname = token;

   int res = loop_command(pcftechnology_lst,SIZ_PCFTECHNOLOGY_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_reference()
{
   if (!get_tok())   p_error();  //  
   compinstarray[compinstcnt]->reference = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_gatecount()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->gateCount = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_pincount()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->pinCount = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_part()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->part = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_numberofconductorlayer()
{
   if (!get_tok())   p_error();  //   
   G.numberoflayers = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping()
{
	G_layerMapCnt = 0;
   if (loop_command(pcftechnology_layermapping_lst, SIZ_PCFTECHNOLOGY_LAYERMAPPING_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping_map()
{
   if (!get_tok())   p_error();  //   
   G_curLayerMapName = token;

	// only act on first map
	///if (G_layerMapCnt == 0)
	{
		if (loop_command(pcftechnology_layermapping_map_lst, SIZ_PCFTECHNOLOGY_LAYERMAPPING_MAP_LST) < 0)
			return -1;
	}
	///else
	///{
	///	fskip();
	///}

	G_layerMapCnt++;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping_map_correspondence()
{
	G_curFootLayerName = "";
	G_curBoardLayerCategory = "";
	G_curBoardLayerName = "";

   if (loop_command(pcftechnology_layermapping_map_corr_lst, SIZ_PCFTECHNOLOGY_LAYERMAPPING_MAP_CORR_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping_map_corr_footlayer()
{
   G_curFootLayerName = "";
   if (!get_tok())   p_error();  //   
   G_curFootLayerName = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping_map_corr_boardlayer()
{
	G_curBoardLayerCategory = "";
	G_curBoardLayerName = "";

   if (loop_command(pcftechnology_layermapping_map_corr_boardlayer_lst, SIZ_PCFTECHNOLOGY_LAYERMAPPING_MAP_CORR_BOARDLAYER_LST) < 0)
      return -1;

	return 1;
}

static CLayerMap *getLayerMap(CString mapname)
{
	CLayerMap *curMap = NULL;

	if (mapname.CompareNoCase("A") == 0)
		curMap = MapA;
	else if (mapname.CompareNoCase("B") == 0)
		curMap = MapB;
	else if (mapname.CompareNoCase("A_THRU") == 0)
		curMap = MapThruA;
	else if (mapname.CompareNoCase("B_THRU") == 0)
		curMap = MapThruB;

	return curMap;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping_map_corr_boardlayer_conductive()
{  
   G_curBoardLayerCategory = "conductive";

   if (!get_tok())   p_error();  //   
   G_curBoardLayerName = token;

	CLayerMap *curMap = getLayerMap(G_curLayerMapName);

	if (curMap != NULL)
	{
		CLayerMapEntry *me = NULL;
		if (!curMap->lookup(G_curFootLayerName, me))
		{
			me = new CLayerMapEntry;
			me->m_footlayerName = G_curFootLayerName;
			curMap->add(G_curFootLayerName, me);
		}
		me->m_boardlayerNames.Add(G_curBoardLayerName);
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layermapping_map_corr_boardlayer_nonconductive()
{  
   G_curBoardLayerCategory = "nonConductive";

   if (!get_tok())   p_error();  //   
   G_curBoardLayerName = token;

	CR5000LayerDef *boardld = getCr5000BoardLayer(G_curBoardLayerName);
	if (boardld == NULL)
	{
		boardld = addCr5000BoardLayer(G_curBoardLayerName);
		boardld->boardLayerType = "nonConductive";
		boardld->conductive = false;
		boardld->conductiveLayerNumber = -1;
	}

	if (boardld != NULL && boardld->boardLayerType.IsEmpty())
	{
		CR5000LayerDef *footld = getCr5000FootLayer(G_curFootLayerName);
		if (footld != NULL)
		{
			boardld->footprintLayerType = footld->footprintLayerType;
		}
	}

	CLayerMap *curMap = getLayerMap(G_curLayerMapName);

	if (curMap != NULL)
	{
		CLayerMapEntry *me = NULL;
		if (!curMap->lookup(G_curFootLayerName, me))
		{
			me = new CLayerMapEntry;
			me->m_footlayerName = G_curFootLayerName;
			curMap->add(G_curFootLayerName, me);
		}
		me->m_boardlayerNames.Add(G_curBoardLayerName);
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_conductivelayer()
{
	G_curConductiveLayerNumber = -1;

   if (loop_command(pcftechnology_conductivelayer_lst, SIZ_PCFTECHNOLOGY_CONDUCTIVELAYER_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_conductivelayer_layernumber()
{
	if (!get_tok())   p_error();  //   
   G_curConductiveLayerNumber = atoi(token);

	// Create implied board layer
	CR5000LayerDef *ld = new CR5000LayerDef;
	ld->boardLayerName = token;
	ld->boardLayerType = "CONDUCTIVE";
	ld->comment = "";
	ld->conductive = true;
	ld->conductiveLayerNumber = G_curConductiveLayerNumber;

	cr5000BoardLayerArray.SetAtGrow(cr5000BoardLayerCount, ld);
	cr5000BoardLayerCount++;

   if (loop_command(pcftechnology_conductivelayer_layernumber_lst, SIZ_PCFTECHNOLOGY_CONDUCTIVELAYER_LAYERNUMBER_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_conductivelayer_layernumber_reefer()
{
	if (!get_tok())   p_error();  //   
   CString boardLayerName = token;

	G_curLayerType = "";

   if (loop_command(pcftechnology_conductivelayer_layernumber_ref_lst, SIZ_PCFTECHNOLOGY_CONDUCTIVELAYER_LAYERNUMBER_REF_LST) < 0)
      return -1;

	CR5000LayerDef *ld = getCr5000BoardLayer(boardLayerName);

	if (ld == NULL)
	{
		ld = addCr5000BoardLayer(boardLayerName);
		ld->conductive = false;  // assume not conductive
	}

	if (ld != NULL)
	{
		ld->conductiveLayerNumber = G_curConductiveLayerNumber;
		ld->boardLayerType = G_curLayerType;
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_conductivelayer_layernumber_ref_type()
{
	if (!get_tok())   p_error();  //   
   G_curLayerType = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_nonconductivelayer()
{
   if (loop_command(pcftechnology_nonconductivelayer_lst, SIZ_PCFTECHNOLOGY_NONCONDUCTIVELAYER_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_footprintlayer()
{
   if (loop_command(pcftechnology_footprintlayer_lst, SIZ_PCFTECHNOLOGY_FOOTPRINTLAYER_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_footprintlayer_layer()
{
   CString  layername;
   if (!get_tok())   p_error();  //   
   layername = token;

	G_curLayerType = "";
	G_curLayerComment = "";
   if (loop_command(pcftechnology_layer_lst, SIZ_PCFTECHNOLOGY_LAYER_LST) < 0)
      return -1;

	CR5000LayerDef *ld = new CR5000LayerDef;
	ld->footprintLayerName = layername;
	ld->footprintLayerType = G_curLayerType;
	ld->conductive = (G_curLayerType.CompareNoCase("conductive") == 0);
	ld->conductiveLayerNumber = -1;

	cr5000FootprintLayerArray.SetAtGrow(cr5000FootprintLayerCount, ld);
	cr5000FootprintLayerCount++;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_nonconductivelayer_layer()
{
   CString  layername;
   if (!get_tok())   p_error();  //   
   layername = token;

	G_curLayerType = "";
	G_curLayerComment = "";
   if (loop_command(pcftechnology_layer_lst, SIZ_PCFTECHNOLOGY_LAYER_LST) < 0)
      return -1;

	CR5000LayerDef *ld = new CR5000LayerDef;
	ld->boardLayerName = layername;
	ld->boardLayerType = G_curLayerType;
	ld->comment = G_curLayerComment;
	ld->conductive = false;
	ld->conductiveLayerNumber = -1;

	cr5000BoardLayerArray.SetAtGrow(cr5000BoardLayerCount, ld);
	cr5000BoardLayerCount++;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layer_type()
{
   G_curLayerType = "";
   if (!get_tok())   p_error();  //   
   G_curLayerType = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcftechnology_layer_propertyS()
{
   G_curLayerComment = "";
   if (!get_tok())   p_error();
	CString kw = token;

   if (!get_tok())   p_error(); 
	CString txt = token;

	if (kw.CompareNoCase("comment") == 0)
		G_curLayerComment = txt;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfnet_comp()
{
   CString  compname;
   if (!get_tok())   p_error();  //   
   compname = token;

   // here now get pin
   G.name[0] = '\0';
   int res = loop_command(pcfnetcomp_lst,SIZ_PCFNETCOMP_LST);

   NetStruct *n = add_net(ActiveFile,cur_netname);
   add_comppin(ActiveFile, n, compname, G.name);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfnet_type()
{
   if (!get_tok())   p_error();  //   

   if (!STRCMPI(token,"POWER"))
   {
      NetStruct *n = add_net(ActiveFile,cur_netname);
      doc->SetAttrib(&n->getAttributesRef(),doc->IsKeyWord(ATT_POWERNET, 0),
         VT_NONE, NULL, SA_APPEND, NULL); // x, y, rot, height
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pfc_comppin()
{
   if (!get_tok())   p_error();  //   
   strcpy(G.name, token);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_package()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->package = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_footprint()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->footprint = token;
   G_curFootprintName = token;

	if (doc->Find_Block_by_Name(token, -1) == NULL)
	{
		G_curFootprintName = token;
		G_defineFootprintNow = true;
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_reversefootprint()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->reversefootprint = token;

   return 1;
}


/****************************************************************************/
/*
*/
static int pcfcomponent_originalreference()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->originalReference = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_placed()
{
   if (!get_tok())   p_error();  //   
   compinstarray[compinstcnt]->placed = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_placementside()
{
   int   bottom;

   if (!get_tok())   p_error();  //   A = top, B = b ottom

   if (token[0] == 'A' || token[0] == 'a')
      bottom = 0;
   else
      bottom = 1;

   compinstarray[compinstcnt]->placementSide = bottom;

   return 1;

}

/****************************************************************************/
/*
*/
static int pcfcomponent_locationlock()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->locationLock = STRCMPI(token,"NO");

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_anglelock()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->angleLock = STRCMPI(token,"NO");

   return 1;

}

/****************************************************************************/
/*
*/
static int pcfcomponent_placementsidelock()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->placementSideLock = STRCMPI(token,"NO");

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_packagesymbol()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->packageSymbol = STRCMPI(token,"NO");

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_outofboard()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->outOfBoard = STRCMPI(token,"NO");

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_arc_radius()
{
   if (!get_tok())   p_error();  //  
   cur_arc.radius = cnv_unit(token);
   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_arc_begin()
{
   return  loop_command(pt_lst,SIZ_PT_LST);
}

/****************************************************************************/
/*
*/
static int cr5000_arc_center()
{
   if (!get_tok())   p_error();  //  
   cur_arc.centerx = cnv_unit_noCorrection(token);
   if (!get_tok())   p_error();  //  
   cur_arc.centery = cnv_unit_noCorrection(token);
   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_arc_end()
{
   return  loop_command(pt_lst,SIZ_PT_LST);
}

/****************************************************************************/
/*
*/
static int pcfcomponent_outofboardlocation()
{

   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt)
   {
      compinstarray[compinstcnt]->outOfBoardLocation.x = polyarray[0].x;
      compinstarray[compinstcnt]->outOfBoardLocation.y = polyarray[0].y;
   }
   else
   {
      // error
   }
   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_location()
{

   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt)
   {
      compinstarray[compinstcnt]->location.x = polyarray[0].x;
      compinstarray[compinstcnt]->location.y = polyarray[0].y;
   }
   else
   {
      // error
   }
   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_angle()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->angle = atof(token);

   return 1;
}
/***************************************************************************/
/*
*/
static int pcfcomponent_layout()
{
	// Component layout redefines the whole geometry, but we get that from 
	// the footprint.  All we want from here are refdes display params, which
	// are not available from the footprint.
   
   // That was until case 2095, in which we received cr5000 data which had
   // the footprint redefined in the component insert area, i.e. defined differently
   // than in the footprint. What a nuisance.

	cur_text_params.layerIndex = -1;

   if (loop_command(pcfcomponent_layout_lst, SIZ_PCFCOMPONENTLAYOUT_LST) < 0)
      return -1;


   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer()
{

	if (loop_command(pcfcomponent_layout_layer_lst, SIZ_PCFCOMPONENTLAYOUTLAYER_LST) < 0)
      return -1;

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_symboltext()
{

	G_curSymbolTextType = "";

	if (loop_command(pcfcomponent_layout_layer_symboltext_lst, SIZ_PCFCOMPONENTLAYOUTLAYERSYMBOLTEXT_LST) < 0)
      return -1;

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_refer()
{

	if (G_defineFootprintNow)
	{
		BlockStruct *b = Graph_Block_On(GBO_APPEND, G_curFootprintName, -1, 0);
      b->setBlockType(AttrSettings->GetBlockType(G_curFootprintName, blockTypeUnknown));
		
		loop_command(pcfcomponent_layout_layer_refer_lst, SIZ_PCFCOMPONENTLAYOUTLAYERREFER_LST);

		Graph_Block_Off();
	}
	else
	{
		fskip();
	}

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_padstack()
{

	if (true)//(G_defineFootprintNow)
	{
		if (!get_tok())
			return p_error();

		CString padstack;
		padstack.Format("PADST_%s", token);

		G.cur_from_layer = -1;
		G.cur_to_layer = -1;

	   bool referencedBlockAlreadyExists = checkReferencedBlock(padstack, "padstack");

      CCr5000LineNumberStamp lineNumberStamp;

		// Reference padstack in parent item
		BlockStruct *b = Graph_Block_On(GBO_APPEND, G_curFootprintName, -1, 0);
		b->setBlockType(AttrSettings->GetBlockType(G_curFootprintName, blockTypeUnknown));		
      curdata = lineNumberStamp.graphBlockReference(padstack, "", 0/*-1*/, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

		loop_command(pcfpadstack_lst, SIZ_PCFPADSTACK_LST);

		// Create padstack if it does not exist		
		if (!checkReferencedBlock(padstack, "padstack"))
			defineDefaultPadstack(padstack, G.cur_from_layer, G.cur_to_layer);


      // Case 2095
		//this line worked fine til case 2095: DataStruct *d = Graph_Block_Reference(padstack, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, G.cur_layerindex, TRUE);
      //til then this section was mostly about padstacks on fids. Now we have something that is centered
      //on the part, but the part's 0,0 is not the center of the part. Seems we should have been offsetting
      //these inserts by the parent insert position all along, though offset seems to usually come out 0,0.
      
      // PROBABLY should be creating a copy of the geometry if G_defineFootprintNow is false, but we
      // might get here multiple times for same base geometry, don't want to make multiple copies each
      // with one pad added. Would have to start tracking for which Refname we are working. I'll
      // let this slide for now, see if problems come up. And is further complicated by possibility
      // of separate top and bottom geoms in camcad from one "raw" cr5000 footprint name.

	   CCr5000Pt insertPt;
      if (polycnt > 0)
      {
         Point2 basePt(compinstarray[compinstcnt]->location.x, compinstarray[compinstcnt]->location.y);
         CCr5000Pt thisPt = polyarray.ElementAt(polycnt-1);
         insertPt.cr5_x = thisPt.cr5_x - basePt.x;
         insertPt.cr5_y = thisPt.cr5_y - basePt.y;
         if (compinstarray[compinstcnt]->placementSide /*is bottom*/)
            insertPt.cr5_x = -insertPt.cr5_x;         
      }
   
      curdata->getInsert()->setOriginX(insertPt.cr5_x);
      curdata->getInsert()->setOriginY(insertPt.cr5_y);
      curdata->setLayerIndex(G.cur_layerindex);

		Graph_Block_Off();
      curdata = NULL;
	}
	else
	{
		fskip();
	}

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_refer_area()
{
   if (loop_command(pcfcomponent_layout_layer_refer_area_lst, SIZ_PCFCOMPONENTLAYOUTLAYERREFERAREA_LST) < 0)
		return -1;

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_refer_area_geometry()
{
   //if (loop_command(pcfcomponent_layout_layer_refer_area_lst, SIZ_PCFCOMPONENTLAYOUTLAYERREFERAREA_LST) < 0))
	//	return -1;
	return graphicarea_geometry();

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_refer_area_line()
{
   //if (!get_tok())   p_error();  //  
   //G_curSymbolTextType = token;
	fskip();

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_symboltext_type()
{
   if (!get_tok())   p_error();  //  
   G_curSymbolTextType = token;

   return 1;
}

/***************************************************************************/
/*
*/
static int pcfcomponent_layout_layer_symboltext_geometry()
{
	// Layer should be a Board Layer name at this point.

	// Currently only interested in text params that define the reference
	// designator. In order to support other types, need to change from this
	// global text param style to making a list of symbol text encountered
	// so they can be added to component insert when component is instantiated.
	// Component insert does not exist yet, so we can't just add the attribs
	// on the fly here.
	
	if (G_curSymbolTextType.CompareNoCase("REFERENCE") == 0)
	{
		cur_text_params.layerIndex = G.cur_layerindex;

		cur_text_params.angle = 0.0;

		graphictext_geometry();

		cur_text_params.angle = G.cur_rotation;
	}
	else
	{
		// We skip it so that the cur_text_params don't get overwritten for some
		// attrib that is not the refdes
		fskip();
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfcomponent_drawrefdes()
{
   if (!get_tok())   p_error();  //  NO
   compinstarray[compinstcnt]->drawRefDes = STRCMPI(token,"NO");

   return 1;
}

/****************************************************************************/
/*
*/
static int pcflayout_layer()
{
   return loop_command(cr5000layer_lst,SIZ_CR5000LAYER_LST);
}

/****************************************************************************/
/*
*/
static int pnfboardlayout_layer()
{
   return loop_command(cr5000layer_lst,SIZ_CR5000LAYER_LST);
}

/****************************************************************************/
/*
*/
static int pnflayer_boardassembly()
{
	// We only process "boardassembly" for panel definitions.
	// If not currently processing a panel, skip boardassembly.
	if (G.curInputFileMode != CR5000_PNF_FILE)
	{
		fskip();
	}
	else
	{
		if (!get_tok()) return p_error();
		CString boardInstanceName = token;

      CCr5000LineNumberStamp lineNumberStamp;

		G_curInsertedBoardID = "";
		loop_command(pnflayer_boardassembly_lst, SIZ_PNFLAYERBOARDASSEMBLY_LST);

		// Inserted board ID doesn't really matter, I'm told Cr5000 does not handle
		// heterogeneous panels. As well, the PCF reader just makes one board file.
		// So, there is no choice. We're going to make an insert of the one and
		// only board. But the board ID (aka subBoard) value is parsed and
		// available in G_curInsertedBoardID.

		if (polycnt > 0 && G_curPcbFile != NULL)
		{
			Point2  p = polyarray.ElementAt(0);

			checkReferencedBlock(G_curPcbFile->getBlock()->getName(), "pcb file");
			DataStruct *d = lineNumberStamp.graphBlockReference(G_curPcbFile->getBlock()->getName(),
				boardInstanceName, 0/*filenum*/,
				p.x, p.y, DegToRad(G.cur_rotation), 0/*mir*/, 1.0/*scale*/, G.cur_layerindex, FALSE/*global*/);

			d->getInsert()->setInsertType(insertTypePcb);
		}

		polyarray.RemoveAll();
		resetPolyCnt();
	}

	return 0;
}

/****************************************************************************/
/*
*/
static int pnflayer_boardassembly_subboard()
{
   if (!get_tok()) return p_error();
   G_curInsertedBoardID = token;

	return 0;
}

/****************************************************************************/
/*
*/
static int pnflayer_boardassembly_angle()
{
	return cr5000_angle();
}

/****************************************************************************/
/*
*/
static int pnflayer_boardassembly_pt()
{
	return cr5000_pt();
}

/******************************************************************************
* ftfpinlayout_layer
*/
static int ftfpinlayout_layer()
{
/* if (!get_tok())
      p_error();

   fskip(); // here is this funny layername which is identical to the next token

   Push_tok = FALSE; // there is a push token after fskip();*/

   int res = loop_command(ftfpinlayer_lst, SIZ_FTFPINLAYER_LST);

   return res;
}

/******************************************************************************
* ftf_fpadstack
*/
static int ftf_fpadstack()
{
   G.name[0] = '\0';
   polyarray.RemoveAll();
   resetPolyCnt();
   G.cur_rotation = 0;

   int res = loop_command(ftffpadstack_lst,SIZ_FTFFPADSTACK_LST);

   if (polycnt)
   {
      cur_pin.padstackGeomName.Format("FTF_PADST_%s", G.name); // tag as footprint padstack
      cur_pin.padstackx = polyarray[0].x;
      cur_pin.padstacky = polyarray[0].y;
      cur_pin.padrotation = G.cur_rotation;
   }
   else
   {
      // error
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return res;
}

/******************************************************************************
* ftf_fpad
*/
static int ftf_fpad()
{
   if (!get_tok())
      return p_error();

   CString padName = token;

   G.name[0] = '\0';
   polyarray.RemoveAll();
   resetPolyCnt();
   G.cur_rotation = 0;

   CCr5000LineNumberStamp lineNumberStamp;

   int res = loop_command(ftffpad_lst, SIZ_FTFFPAD_LST);

   if (polycnt)
   {
      cur_pin.padstackGeomName = "PADST_PAD_" + padName;
      cur_pin.padstackx = polyarray[0].x;
      cur_pin.padstacky = polyarray[0].y;
      cur_pin.padrotation = G.cur_rotation;

      BlockStruct *padstackGeom = Graph_Block_On(GBO_APPEND, cur_pin.padstackGeomName, -1, 0);
		padstackGeom->setBlockType(blockTypePadstack);

      CString padGeomName = "PAD_" + padName;
		checkReferencedBlock(padGeomName, "pad");
      DataStruct* data = lineNumberStamp.graphBlockReference(padGeomName, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, G.cur_layerindex, TRUE);
      Graph_Block_Off();
   }
   else
   {
      // error
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 0;
}

/******************************************************************************
* ftfheelprint_layer
*/
static int ftfheelprint_layer()
{
   return loop_command(cr5000layer_lst, SIZ_CR5000LAYER_LST);
}

/****************************************************************************/
/*
*/
static int graphic_line()
{
   int   res;

   cur_netname = "";
   res = loop_command(graphicline_lst,SIZ_GRAPHICLINE_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int graphic_layer_surface()
{
   int   res;

   cur_netname = "";
   res = loop_command(GRAPHIC_layer_surface_lst,SIZ_GRAPHICLAYERSURFACE_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int graphic_area()
{
   int   res;

   cur_netname = "";
   res = loop_command(graphicarea_lst,SIZ_GRAPHICAREA_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int graphic_text()
{
   int   res;

   cur_text_params.string = "";
   polyarray.RemoveAll();
   resetPolyCnt();
   G.cur_rotation = 0;

   CCr5000LineNumberStamp lineNumberStamp;

   res = loop_command(graphictext_lst,SIZ_GRAPHICTEXT_LST);

   if (polycnt)
   {
      char        *lp;
      int         linecnt = 0;
      double      xoff = 0, yoff = 0;
      int         prop = TRUE;
      int         maxchar = 0;
   
      if (!strlen(cur_text_params.string))
      {
         resetPolyCnt();
         return 1;
      }

      char  *tst = STRDUP(cur_text_params.string);

      lp = strtok(tst,"\n\r");
      // here cnt the number of lines
      while (lp)
      {
         if ((int)strlen(lp) > maxchar)
            maxchar = strlen(lp);
         linecnt++;
         lp = strtok(NULL,"\n\r");
      }

      free(tst);

      Rotate(0, cur_text_params.height, G.cur_rotation, &xoff, &yoff);

      // here write text
      double x = polyarray[0].x, y = polyarray[0].y;
      normalize_text(&x,&y,G.cur_textjust,
                  G.cur_rotation,
                  G.cur_mirror,
                  cur_text_params.height*linecnt*1.2,     // linespacing is 120 on a 100 text
                  cur_text_params.width*0.8*maxchar);

      x += linecnt * xoff + (xoff*0.2);   // start top to bottom 
      y += linecnt * yoff + (yoff*0.2);

      // height correct after the adjustment and line spacing
      G.cur_height *= TEXT_CORRECT; // a 175 textheight is only 130, but text
                                 // height needed for linefeed

      tst = STRDUP(cur_text_params.string);
      lp = strtok(tst,"\n\r");

      while (lp)
      {
         // normalize
         x -= xoff;
         y -= yoff;
         DataStruct *d = Graph_Text(G.cur_layerindex, lp, x, y,
               cur_text_params.height, cur_text_params.width, DegToRad(G.cur_rotation),
               0, prop,    // accel has proportional text
               G.cur_mirror,  
               0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg

         lp = strtok(NULL,"\n\r");

         lineNumberStamp.stamp(d);
      }
      free(tst);
   }
   else
   {
      // error
   }

   polyarray.RemoveAll();
   resetPolyCnt();


   return res;
}

/****************************************************************************/
/*
*/
static int cr5000_net()
{
   if (!get_tok())   p_error();  //  netname
   cur_netname = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int graphicline_geometry()
{
   return loop_command(linegeometry_lst,SIZ_LINEGEOMETRY_LST);
}

/****************************************************************************/
/*
*/
static int layer_surface_geometry()
{
   return loop_command(layersurfacegeometry_lst,SIZ_LAYERSURFACEGEOMETRY_LST);
}

/****************************************************************************/
/*
*/
static int graphicarea_geometry()
{
   return loop_command(areageometry_lst,SIZ_AREAGEOMETRY_LST);
}

/****************************************************************************/
/*
*/
static int graphictext_geometry()
{
   return loop_command(textgeometry_lst,SIZ_TEXTGEOMETRY_LST);
}

/****************************************************************************/
/*
*/
static int cr5000text_string()
{
   if (!get_tok())   p_error();  //  
      cur_text_params.string = token;
   if (strstr(token, "SPECIFICATIONS"))
      cur_text_params.string = token;      
   string_token = TRUE;
   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_font()
{
   if (!get_tok())   p_error();  //  
   cur_text_params.font = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_kfont()
{  
   if (!get_tok())   p_error();  //  
   cur_text_params.kFont = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_space()
{
   if (!get_tok())   p_error();  //  
   cur_text_params.space = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_vspace()
{
   if (!get_tok())   p_error();  //  
   cur_text_params.vSpace = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_strokewidth()
{
   if (!get_tok())   p_error();  //  
   cur_text_params.strokeWidth = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_angle()
{
   cr5000_angle();

	cur_text_params.angle = G.cur_rotation;

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_pt()
{
	cur_text_params.x = 0.0;
	cur_text_params.y = 0.0;

   cr5000_pt();

   if (polycnt >= 1)
   {
      Point2   p = polyarray.ElementAt(0);

      cur_text_params.x = p.x;
      cur_text_params.y = p.y;
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_dir()
{
   if (!get_tok())   p_error();  //  
   cur_text_params.dir = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_justify()
{
   if (!get_tok())   p_error();  // 
	cur_text_params.justify = token;
   

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_flip()
{
   if (!get_tok())   p_error();  //  

   return 1;

}

/****************************************************************************/
/*
*/
static int cr5000text_reverse()
{
   if (!get_tok())   p_error();  //  

   return 1;
}

/******************************************************************************
* layer_conductive
*/
static int layer_conductive()
{
   if (!get_tok())   p_error();  //  
   G.cur_layerindex = Graph_Level(token,"", 0);
   return 1;
}

/****************************************************************************/
/*
*/
static int systemlayer_type()
{
   if (!get_tok())   p_error();  //  

	CString layername = token;

	if (layername.CompareNoCase("BOARD_FIGURE") == 0)
	{
		if (G.curInputFileMode == CR5000_PNF_FILE)
			layername = "Panel Outline";
		else
			layername = "Board Outline";
	}

   G.cur_layerindex = Graph_Level(layername,"", 0);

	LayerStruct *lp = doc->getLayer(G.cur_layerindex);
	if (layername.CompareNoCase("Board Outline") == 0)
		lp->setLayerType(layerTypeBoardOutline);
	else if (layername.CompareNoCase("Panel Outline") == 0)
		lp->setLayerType(layerTypePanelOutline);



   return 1;
}

/****************************************************************************/
/*
*/
static int layer_nonconductive()
{
   if (!get_tok())   p_error();  //  
   G.cur_layerindex = Graph_Level(token,"", 0);
   return 1;
}

/****************************************************************************/
/*
*/
static int layer_footlayer()
{
   if (!get_tok())   p_error();  //  
   G.cur_layerindex = Graph_Level(token,"", 0);

   return 1;
}

/****************************************************************************/
/*
*/
static int layer_systemlayer()
{
   return loop_command(systemlayer_lst,SIZ_SYSTEMLAYER_LST);
}

/****************************************************************************/
/*
*/
static int layer_drawof()
{
   return loop_command(systemlayer_lst,SIZ_SYSTEMLAYER_LST);
}

/****************************************************************************/
/*
*/
static int layer_infoof()
{
   return loop_command(systemlayer_lst,SIZ_SYSTEMLAYER_LST);
}

/****************************************************************************/
/*
   this is a via in the pcf file
*/
static int pcfpadstack_fromto()
{
   if (!get_tok())   p_error();  //  
   G.cur_from_layer = atoi(token);

   if (!get_tok())   p_error();  //  
   if (!STRCMPI(token,")"))
   {
      push_tok();
      G.cur_to_layer = G.cur_from_layer;
   }
   else
   {
      G.cur_to_layer = atoi(token);
   }

   if (G.cur_from_layer > G.cur_to_layer)
   {
      int tmp;
      tmp = G.cur_from_layer;
      G.cur_from_layer = G.cur_to_layer;
      G.cur_to_layer = tmp;
   }

	/// By virtue of how referenced padstacks should be defined,
	/// I think these are indeed supported. At least, some are. The example in case 1832 
	/// with (fromTo 4) is supported, the blind via references a SMD padstack.
	/// Seems correct. If all such blind/buried padstacks are defiend with proper 
	/// pad layer references, then these are okay. I'm not sure if that is so.
	/// What does seem safe at teh moment is if the "via" in only on top or bottom,
	/// then we'll assume it refs a SMD pad, and it is okay. 
	/// SHOULD be able to check here that ref'd padstack is SMD, but not doing so at the moment.
   if ( (G.cur_from_layer == 1 && G.cur_to_layer == G.numberoflayers) ||  // regular THRU
		  (G.cur_from_layer == G.cur_to_layer && (G.cur_from_layer == 1 || G.cur_from_layer == G.numberoflayers))) // on top or on bottom
	{
		// Ok
	}
	else
   {
      fprintf(ferr,"Blind/Buried Vias from [%d] to [%d] not implemented in %s at %ld\n", G.cur_from_layer, G.cur_to_layer, cur_filename, ifp_line);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
   this is a via in the pcf file
*/
static int pcfpadstack_propertys()
{
   CString  propname, propval;

   if (!get_tok())   p_error();  //  
   propname = token;

   if (!get_tok())   p_error();  //  
   propval = token;

   doc->SetUnknownAttrib(&curdata->getAttributesRef(), propname, propval, SA_OVERWRITE, NULL); //  

   return 1;
}

/****************************************************************************/
/*
   TSR 3659: (side ) command added.
   
   pcfpadstack_side()

*/
static int pcfpadstack_side()
{

   if (!get_tok())   p_error();  //  
   G.cur_side = token[0];

   return 1;
}

/****************************************************************************/
/*
   TSR 3659: (flip ) command added.

   pcfpadstack_flip()
*/
static int pcfpadstack_flip()
{

   if (!get_tok())   p_error();  //  
   G.cur_mirror = token[0];

   return 1;
}


/****************************************************************************/
/*
   this is a via in the pcf file
*/
static int pcfpadstack_istestpad()
{
   int      testpad = FALSE;
   CString  gname = G.name;

   if (!get_tok())   p_error();  //  
   if (!STRCMPI(token, "YES"))
      testpad = TRUE;

   // side
   G.name[0] = '\0'; // id
   G.cur_side = ' '; // a = top, b = bottom

   int res = loop_command(pcfpadstack_istestpad_lst,SIZ_PCFPADSTACK_ISTESTPAD_LST);

   if (testpad)
   {
      int testaccessindex = ATT_TEST_ACCESS_BOT;
      if (G.cur_side == 'A')
         testaccessindex = ATT_TEST_ACCESS_TOP;
     
      curdata->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeTest), valueTypeString, testaccesslayers[testaccessindex], attributeUpdateOverwrite, NULL);

		// case 1896 - save value of "id" record as Refname
		if (gname.Compare(G.name) != 0) // a crude way to figure if ID was set in cad data "id" record
		{
			if (curdata->getInsert() != NULL)
				curdata->getInsert()->setRefname(STRDUP(G.name));

			// Add visible attribute, so it is there if someone wants to mess with it.
			// But make it Not visible, it has no "graphic" information is the cad
			// file, and trial shows it to add a lot of clutter when visible.
			double txtheight = doc->convertToPageUnits(pageUnitsMilliMeters, 0.80);
			double penwidth =  doc->convertToPageUnits(pageUnitsMilliMeters, 0.10);
			int penWidthIndex = Graph_Aperture("", T_ROUND, penwidth, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);

			doc->SetUnknownVisAttrib(&curdata->getAttributesRef(), ATT_REFNAME, 
				G.name,
				0.0, 0.0, DegToRad(0.0), txtheight, txtheight * 0.65, 
				0, penWidthIndex, FALSE /*visible*/, SA_OVERWRITE, 0L, 0 /*layerindex*/, 0, 
				horizontalPositionCenter, verticalPositionCenter);
		}
   }

   strcpy(G.name, gname);
   return res;
}

/****************************************************************************/
/*
*/
static int istestpad_side()
{
   if (!get_tok())   p_error();  //  
   G.cur_side = token[0];

   return 1;
}

/****************************************************************************/
/*
*/
static int istestpad_id()
{
   if (!get_tok())   p_error();  //  
   strcpy(G.name, token);

   return 1;
}

/****************************************************************************
*/
static LayerStruct *findDrillLayer()
{
	for (int i = 0; i < doc->getLayerCount(); i++)
	{
		LayerStruct *lp = doc->getLayerAt(i);
		if (lp != NULL && lp->getLayerType() == layerTypeDrill)
			return lp;
	}

	return NULL;
}

/****************************************************************************
*/
static void defineDefaultPadstackDrillhole(BlockStruct *b, int fromLayer, int toLayer)
{
	if (b != NULL)
	{
		// Add a drill hole if this looks like a THRU padstack, i.e. if there
		// are pads on more than one layer
		if (fromLayer == -1 || (fromLayer != toLayer))
		{
			int drillindex = get_drillindex(DefaultPadDrillDia); 
			CString drillname;
			drillname.Format("DRILL_%d",drillindex);

			// Use any layer that is type drill
			LayerStruct *lpDrill = findDrillLayer();

			// If no drill layer found then put it on floating layer 0.
			if (lpDrill == NULL)
				lpDrill = doc->getLayer("0");

			if (lpDrill != NULL)
			{
				// must be filenum 0, because apertures are global.
				checkReferencedBlock(drillname, "drill");
				DataStruct *d = Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, lpDrill->getLayerIndex(), TRUE);
				d->getInsert()->setInsertType(insertTypeDrillHole);
			}
		}
	}
}

/****************************************************************************
*/
static void defineDefaultPadstackPads(BlockStruct *b, int fromLayer, int toLayer)
{
	if (b != NULL)
	{

		CString defaultApName = "Default_Pad_Aperture";
		int err;
		Graph_Aperture(defaultApName, T_ROUND, DefaultPadApertureDia, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

		// Add a pad on each layer
		// This importer makes electrical layers based on layer number,
		// if that is changed, this becomes broken due to assumption.
		for (int layernum = 1; layernum <= G.numberoflayers; layernum++)
		{
			if (fromLayer == -1 || (layernum >= fromLayer && layernum <= toLayer))
			{
				CString layername;
				layername.Format("%d", layernum);
				LayerStruct *lp = doc->getLayer(layername);
				if (lp != NULL)
				{
					DataStruct *d = Graph_Block_Reference(defaultApName, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, lp->getLayerIndex(), TRUE);
					d->getInsert()->setInsertType(insertTypeUnknown);
				}
			}
		}
	}
}

/****************************************************************************
*/
static void defineDefaultPadstack(CString name, int fromLayer, int toLayer)
{
	// Define a padstack based on parameters from IN file

	if (!name.IsEmpty())
	{
      BlockStruct *b = Graph_Block_On(GBO_OVERWRITE, name, -1, 0);
		b->setBlockType(blockTypePadstack);

		defineDefaultPadstackPads(b, fromLayer, toLayer);
		defineDefaultPadstackDrillhole(b, fromLayer, toLayer);

		Graph_Block_Off();
	}
}

/****************************************************************************/
/*
   this is a via in the pcf file
*/
static int pcf_padstack()
{
   CString  padstack;
   if (!get_tok())   p_error();  //  
   padstack.Format("PADST_%s", token);
	CString rawname = token;

	G.cur_mirror = '\0';
	G.cur_side = '\0';
   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();
   cur_netname = "";

	// Chicken and egg problem.
	// In most cases the padstack is already defined, it came from the FTF file.
	// But sometimes it is not there.
	// We need to have the "cur_data" now, as some of the existing parser will expect
	// to be able to put things on it, e.g. attributes. The Graph_Block_Reference
	// call will automatically create an empty referenced block if it does not already
	// exist. We need some of the info from the pcfpadstack_lst processing in order
	// to correctly build the block.
	// So make note of whether the block already exists, and if it does not, then
	// let the GBR go, and then redefine the block after.

	bool referencedBlockAlreadyExists = checkReferencedBlock(padstack, "padstack");

   CCr5000LineNumberStamp lineNumberStamp;

   curdata = lineNumberStamp.graphBlockReference(padstack, "", -1, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   if (loop_command(pcfpadstack_lst,SIZ_PCFPADSTACK_LST) < 0)
      return -1;

	if (!referencedBlockAlreadyExists)
		defineDefaultPadstack(padstack, G.cur_from_layer, G.cur_to_layer);

   if (polycnt == 1)
   {
      Point2   p = polyarray.ElementAt(0);

      curdata->getInsert()->setOriginX(p.x);
      curdata->getInsert()->setOriginY(p.y);

		InsertTypeTag insertType = insertTypeVia;
		insertType = AttrSettings->GetInsertType(padstack, insertType);  // check modified camcad geom name
		insertType = AttrSettings->GetInsertType(rawname, insertType);   // check original name from cad file
      if (insertType == insertTypeTestPoint)
      {
         // If the geometry name is said to use as testpoint, don't change the insert type, let it as via
         // but add the "TEST" attribute on it
         insertType = insertTypeVia;
         CString value =  "";
         curdata->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeTest), valueTypeString, value.GetBuffer(0), attributeUpdateFenced, NULL);
      }
		curdata->getInsert()->setInsertType(insertType);

      /*TSR 3659: The (side B) and (Flip X) handling added.
      If flip x then mirror x; if y, mirror y*/
      if(G.cur_mirror == 'X')
         curdata->getInsert()->setMirrorFlags(3);
      if(G.cur_mirror == 'Y')
      {
         curdata->getInsert()->setMirrorFlags(3);
         curdata->getInsert()->setAngle(DegToRad(180));
      }
      // if side B, then place on bottom.
      if(G.cur_side == 'B')
         curdata->getInsert()->setPlacedBottom(true);
      if (strlen(cur_netname))
      {
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
      }
   }
   else
   {
      fprintf(ferr,"Via error at %ld\n", ifp_line);
      display_error++;
   }

   polyarray.RemoveAll();
   resetPolyCnt();
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int ftfpadset_footlayer()
{
   if (!get_tok())   p_error();  //  
   G.cur_layerindex = Graph_Level(token,"", 0);
   return 1;
}

/****************************************************************************/
/*
*/
static int linegeometry_line()
{
   G.cur_width = 0;
   G.cur_linepenshape = T_ROUND;

   CCr5000LineNumberStamp lineNumberStamp;

   curdata = lineNumberStamp.graphPolyStruct(G.cur_layerindex, 0L, 0);  // this must be a floating layer.

	LayerStruct *lp = doc->getLayer(G.cur_layerindex);

	if (lp != NULL && CompOutlineLayerNameList.Find(lp->getName()))
      curdata->setGraphicClass(graphicClassComponentOutline);

   if (strlen(cur_netname))
   {
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
         VT_STRING,
         cur_netname.GetBuffer(0),
         SA_OVERWRITE, NULL); //  
      curdata->setGraphicClass( graphicClassEtch );
   }

   int retval =  loop_command(linegeometryline_lst,SIZ_LINEGEOMETRYLINE_LST);

   curdata = NULL;

   return retval;
}

/****************************************************************************/
/*
*/
static int layersurfacegeometry_surface()
{
   CCr5000LineNumberStamp lineNumberStamp;

   curdata = lineNumberStamp.graphPolyStruct(G.cur_layerindex, 0L, 0);  // this must be a floating layer.

	LayerStruct *lp = doc->getLayer(G.cur_layerindex);

	if (lp != NULL)
	{
      if (lp->getLayerType() == layerTypeBoardOutline)
         curdata->setGraphicClass( graphicClassBoardOutline );
		else if (lp->getLayerType() == layerTypePanelOutline)
			curdata->setGraphicClass( graphicClassPanelOutline );
		else if (CompOutlineLayerNameList.Find(lp->getName()))
			curdata->setGraphicClass( graphicClassComponentOutline );
	} 

   if (strlen(cur_netname) > 0 &&
		curdata->getGraphicClass() != graphicClassBoardOutline &&
		curdata->getGraphicClass() != graphicClassPanelOutline &&
		curdata->getGraphicClass() != graphicClassComponentOutline)
   {
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
         VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); //  
      curdata->setGraphicClass( graphicClassEtch );
   }

   G.cur_filltype = FALSE;

   int retval = loop_command(surfacegeometrysurface_lst,SIZ_SURFACEGEOMETRYSURFACE_LST);

   curdata = NULL;

   return retval;
}

/****************************************************************************/
/*
*/
static int graphsurface_fillwidth()
{
   if (!get_tok())   return p_error();  // x
   double width = cnv_unit(token);

   // case #1700 - surfaces should always be filled
   //if (width > 0)
   //   G.cur_filltype = TRUE;
	// case #1853 - No they shouldn't !
   //G.cur_filltype = TRUE;

	// Do not fill surfaces that are Panel Outline or Board Outline
	// Fill all other surfaces.

	// Assume not panel or board outline
	G.cur_filltype = TRUE;

	// Override if is panel or board outline
	LayerStruct *lp = doc->getLayer(G.cur_layerindex);
	if (lp != NULL)
	{
		if (lp->getLayerType() == layerTypeBoardOutline ||
			lp->getLayerType() == layerTypePanelOutline)
		{
			G.cur_filltype = FALSE;
		}
	}

   return 1;
}

/****************************************************************************/
/*
*/
static int graphsurface_openshape()
{
   return loop_command(surfacegeometryopenshape_lst,SIZ_SURFACEGEOMETRYOPENSHAPE_LST);
}

/****************************************************************************/
/*
*/
static int padgeometry_circle()
{
   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();

   if (!get_tok())   return p_error();  // x
   double rad = cnv_unit(token);

   CCr5000LineNumberStamp lineNumberStamp;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;


	if (polycnt == 1)
   {
		G.cur_geomcnt++;

		// If block does not exist, then just create simple aperture
		BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);

		if (block == NULL)
		{
			// Create simple aperture
			Point2   p = polyarray.ElementAt(0);
			int   err;
			Graph_Aperture(G.name, T_ROUND, rad*2, 0.0, p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);
		}
		else
		{
			// Block already exists
			// If it is not a complex aperture, then convert it to one.
			if (!block->isComplexAperture())
			{
				convertToComplexAperture(block);
			}

			// Append new item to complex aperture
			BlockStruct *complexApBlock = doc->Find_Block_by_Name(G.name, -1);
			if (complexApBlock != NULL && complexApBlock->isComplexAperture() /*jic*/)
			{
				int insertedBlockNum = complexApBlock->getComplexApertureSubBlockNumber();
				BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
				if (insertedBlock != NULL)
				{
					CString insertedBlockName = insertedBlock->getName();

					CString apName;
					apName.Format("%s_Geom_%d", G.name, G.cur_geomcnt);
					Point2   p = polyarray.ElementAt(0);
					int   err;
					Graph_Aperture(apName, T_ROUND, rad*2, 0.0, p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);

					int floatinglayerindx = 0;
					BlockStruct *block = Graph_Block_On(GBO_APPEND, insertedBlockName, -1, 0);
					DataStruct* data = lineNumberStamp.graphBlockReference(apName, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, floatinglayerindx/*G.cur_layerindex*/, TRUE);
					Graph_Block_Off();
				}
			}
		}
   }
   else
   {
      fprintf(ferr,"Circle error at %ld\n", ifp_line);
      display_error++;
   }


   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
*/
static int holegeometry_circle()
{
   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();

   if (!get_tok())   return p_error();  // x
   double rad = cnv_unit(token);

   CCr5000LineNumberStamp lineNumberStamp;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt == 1)
   {
      Point2   p = polyarray.ElementAt(0);

      CString  drillname;
      int drilllayernum = G.cur_layerindex; //Graph_Level("DRILLHOLE","",0);
		LayerStruct *lp = doc->getLayer(G.cur_layerindex);
		if (lp != NULL)
			lp->setLayerType(layerTypeDrill);

      int drillindex = get_drillindex(rad*2);   
      drillname.Format("DRILL_%d",drillindex);
      // must be filenum 0, because apertures are global.
		checkReferencedBlock(drillname, "drill");
      DataStruct* data = lineNumberStamp.graphBlockReference(drillname, NULL, 0, p.x, p.y, 0.0, 0, 1.0, drilllayernum, TRUE);
   }
   else
   {
      fprintf(ferr,"Circle error at %ld\n", ifp_line);
      display_error++;
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
*/
static void convertToComplexAperture(BlockStruct *block)
{
	if (block != NULL)
	{
		CString originalBlockName = block->getName();

		// Rename existing block
		CString apName_1;
		apName_1.Format("%s_Geom_%d", G.name, 1);
		block->setName(apName_1);

		// Create block with data list that inserts the previous block
		CString insertedblockname;
		insertedblockname.Format("USR_%s", originalBlockName); // naming convention found existing elsewhere in this code
		int floatinglayerindx = 0;
		BlockStruct *newblock = Graph_Block_On(GBO_OVERWRITE, insertedblockname, -1, 0);
		Graph_Block_Reference(apName_1, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, floatinglayerindx, TRUE);
		Graph_Block_Off();

		// Create complex aperture
		Graph_Complex(originalBlockName, 0, insertedblockname, 0.0, 0.0, 0.0);
	}
}

/****************************************************************************/
/*
*/
static int padgeometry_rectangle()
{
   G.cur_widthindex = 0;
   G.cur_width = 0;
   G.cur_height= 0;                    

   polyarray.RemoveAll();
   resetPolyCnt();

   CCr5000LineNumberStamp lineNumberStamp;

   if (loop_command(rectangle_lst,SIZ_RECTANGLE_LST) < 0)
      return -1;



   if (polycnt == 1)
   {
		G.cur_geomcnt++;

		// If block does not exist, then just create simple aperture
		BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);

		if (block == NULL)
		{
			// Create simple aperture
			Point2   p = polyarray.ElementAt(0);
			int   err;
			Graph_Aperture(G.name, T_RECTANGLE, G.cur_width, G.cur_height, p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);
		}
		else
		{
			// Block already exists
			// If it is not a complex aperture, then convert it to one.
			if (!block->isComplexAperture())
			{
				convertToComplexAperture(block);
			}

			// Append new item to complex aperture
			BlockStruct *complexApBlock = doc->Find_Block_by_Name(G.name, -1);
			if (complexApBlock != NULL && complexApBlock->isComplexAperture() /*jic*/)
			{
				int insertedBlockNum = complexApBlock->getComplexApertureSubBlockNumber();
				BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
				if (insertedBlock != NULL)
				{
					CString insertedBlockName = insertedBlock->getName();

					CString apName;
					apName.Format("%s_Geom_%d", G.name, G.cur_geomcnt);
					Point2   p = polyarray.ElementAt(0);
					int   err;
					Graph_Aperture(apName, T_RECTANGLE, G.cur_width, G.cur_height, p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);
					
					int floatinglayerindx = 0;
					BlockStruct *block = Graph_Block_On(GBO_APPEND, insertedBlockName, -1, 0);
					DataStruct* data = lineNumberStamp.graphBlockReference(apName, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, floatinglayerindx/*G.cur_layerindex*/, TRUE);
					Graph_Block_Off();
				}
			}
		}


	}
   else
   {
      fprintf(ferr,"Rectangle error at %ld\n", ifp_line);
      display_error++;
   }


   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
       (out 152400)
       (in 139700)
       (pt 0 0)
       (nBridge 4)
       (bridgeWidth 38100)
       (bridgeAngle 0)

*/
static int padgeometry_roundthermal()
{
   // cur_roundthermal

   polyarray.RemoveAll();
   resetPolyCnt();

   CCr5000LineNumberStamp lineNumberStamp;

   if (loop_command(roundthermal_lst,SIZ_ROUNDTHERMAL_LST) < 0)
      return -1;

   if (polycnt == 1)
   {
		G.cur_geomcnt++;

		// If block does not exist, then just create simple aperture
		BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);

		if (block == NULL)
		{
	      Point2   p = polyarray.ElementAt(0);

			int   err;
			int code = Graph_Aperture(G.name, T_THERMAL, cur_aperture.in, cur_aperture.out,
				p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);

			BlockStruct *bb = doc->getWidthTable()[code];
			bb->setSizeC((DbUnit)cur_aperture.bridgewidth);
			bb->setSizeD((DbUnit)DegToRad(cur_aperture.bridgeangle));
			bb->setSpokeCount(cur_aperture.nbridge);	
		}
		else
		{
			// Block already exists
			// If it is not a complex aperture, then convert it to one.
			if (!block->isComplexAperture())
			{
				convertToComplexAperture(block);
			}

			// Append new item to complex aperture
			BlockStruct *complexApBlock = doc->Find_Block_by_Name(G.name, -1);
			if (complexApBlock != NULL && complexApBlock->isComplexAperture() /*jic*/)
			{
				int insertedBlockNum = complexApBlock->getComplexApertureSubBlockNumber();
				BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
				if (insertedBlock != NULL)
				{
					CString insertedBlockName = insertedBlock->getName();

					CString apName;
					apName.Format("%s_Geom_%d", G.name, G.cur_geomcnt);
					Point2   p = polyarray.ElementAt(0);
					int   err;
					int code = Graph_Aperture(apName, T_THERMAL, cur_aperture.in, cur_aperture.out,
						p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);

					BlockStruct *bb = doc->getWidthTable()[code];
					bb->setSizeC((DbUnit)cur_aperture.bridgewidth);
					bb->setSizeD((DbUnit)DegToRad(cur_aperture.bridgeangle));
					bb->setSpokeCount(cur_aperture.nbridge);


					int floatinglayerindx = 0;
					BlockStruct *block = Graph_Block_On(GBO_APPEND, insertedBlockName, -1, 0);
					DataStruct* data = lineNumberStamp.graphBlockReference(apName, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, floatinglayerindx/*G.cur_layerindex*/, TRUE);
					Graph_Block_Off();
				}
			}
		}


	}
   else
   {
      fprintf(ferr,"Roundthermal error at %ld\n", ifp_line);
      display_error++;
   }


   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}
/****************************************************************************/
/*
       (out 152400)
       (in 139700)
       (pt 0 0)
*/
static int padgeometry_donut()
{
   // cur_roundthermal

   polyarray.RemoveAll();
   resetPolyCnt();

   CCr5000LineNumberStamp lineNumberStamp;

   if (loop_command(donut_lst,SIZ_DONUT_LST) < 0)
      return -1;

   if (polycnt == 1)
   {
		G.cur_geomcnt++;

		// If block does not exist, then just create simple aperture
		BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);

		if (block == NULL)
		{
			// Create simple aperture
			Point2   p = polyarray.ElementAt(0);
			int   err;
			///Graph_Aperture(G.name, T_RECTANGLE, G.cur_width, G.cur_height, p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);
			int code = Graph_Aperture(G.name, T_DONUT, cur_aperture.in, cur_aperture.out,
				p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);
		}
		else
		{
			// Block already exists
			// If it is not a complex aperture, then convert it to one.
			if (!block->isComplexAperture())
			{
				convertToComplexAperture(block);
			}

			// Append new item to complex aperture
			BlockStruct *complexApBlock = doc->Find_Block_by_Name(G.name, -1);
			if (complexApBlock != NULL && complexApBlock->isComplexAperture() /*jic*/)
			{
				int insertedBlockNum = complexApBlock->getComplexApertureSubBlockNumber();
				BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
				if (insertedBlock != NULL)
				{
					CString insertedBlockName = insertedBlock->getName();

					CString apName;
					apName.Format("%s_Geom_%d", G.name, G.cur_geomcnt);
					Point2   p = polyarray.ElementAt(0);
					int   err;
					int code = Graph_Aperture(apName, T_DONUT, cur_aperture.in, cur_aperture.out,
						p.x, p.y, 0.0, 0, BL_APERTURE, TRUE, &err);

					int floatinglayerindx = 0;
					BlockStruct *block = Graph_Block_On(GBO_APPEND, insertedBlockName, -1, 0);
					DataStruct* data = lineNumberStamp.graphBlockReference(apName, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, floatinglayerindx/*G.cur_layerindex*/, TRUE);
					Graph_Block_Off();
				}
			}
		}


   }
   else
   {
      fprintf(ferr,"Roundthermal error at %ld\n", ifp_line);
      display_error++;
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
*/
static int padgeometry_oblong()
{
   G.cur_width = 0;

   polyarray.RemoveAll();
   resetPolyCnt();

   CCr5000LineNumberStamp lineNumberStamp;

   if (loop_command(oblong_lst,SIZ_OBLONG_LST) < 0)
      return -1;

	// If aperture block apready exists, make sure it is a complex aperture
	BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);
	if (block != NULL && !block->isComplexAperture())
	{
		convertToComplexAperture(block);
	}
	G.cur_geomcnt++;

   CString  usr_name;
   usr_name.Format("USR_%s", G.name);

   int err;
   G.cur_widthindex = Graph_Aperture("", T_ROUND, G.cur_width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   curblock = Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   curdata = lineNumberStamp.graphPolyStruct(G.cur_layerindex, 0L, 0);  // this must be a floating layer.
   curpoly = Graph_Poly(NULL,G.cur_widthindex, 0, 0, 0); // fillflag , negative, closed

   for (int i=0;i<polycnt;i++)
   {
      Point2   p = polyarray.ElementAt(i);
      Graph_Vertex(p.x, p.y, p.bulge);
   }

   Graph_Block_Off();
   curblock = NULL;

   Graph_Complex(G.name, 0, usr_name, 0.0, 0.0, 0.0);

   polyarray.RemoveAll();
   resetPolyCnt();
   curdata = NULL;

   return 1;

}

/****************************************************************************/
/*
*/
static int padgeometry_line()
{
   G.cur_width = 0;
   G.cur_linepenshape = T_ROUND;

   polyarray.RemoveAll();
   resetPolyCnt();

	// If aperture block apready exists, make sure it is a complex aperture
	BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);
	if (block != NULL && !block->isComplexAperture())
	{
		convertToComplexAperture(block);
	}
	G.cur_geomcnt++;

   CString  usr_name;
   usr_name.Format("USR_%s", G.name);
   curblock = Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   CCr5000LineNumberStamp lineNumberStamp;

   curdata = lineNumberStamp.graphPolyStruct(G.cur_layerindex, 0L, 0);  // this must be a floating layer.

   loop_command(linegeometryline_lst,SIZ_LINEGEOMETRYLINE_LST);

   Graph_Block_Off();
   curblock = NULL;

   Graph_Complex(G.name, 0, usr_name, 0.0, 0.0, 0.0);

   polyarray.RemoveAll();
   resetPolyCnt();
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int padgeometry_surface()
{
   G.cur_widthindex = 0;
   G.cur_filltype = FALSE;

   polyarray.RemoveAll();
   resetPolyCnt();

	// If aperture block apready exists, make sure it is a complex aperture
	BlockStruct *block = doc->Find_Block_by_Name(G.name, -1);
	if (block != NULL && !block->isComplexAperture())
	{
		convertToComplexAperture(block);
	}
	G.cur_geomcnt++;

	// Create new geometry
   CString  usr_name;
   usr_name.Format("USR_%s", G.name);

   CCr5000LineNumberStamp lineNumberStamp;

   curblock = Graph_Block_On(GBO_APPEND,usr_name, -1, 0);
   curdata = lineNumberStamp.graphPolyStruct(G.cur_layerindex, 0L, 0);  // this must be a floating layer.
   loop_command(surfacegeometrysurface_lst,SIZ_SURFACEGEOMETRYSURFACE_LST);
   Graph_Block_Off();
   curblock = NULL;

   Graph_Complex(G.name, 0, usr_name, 0.0, 0.0, 0.0);

   polyarray.RemoveAll();
   resetPolyCnt();
   curdata = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int roundthermal_in()
{
   if (!get_tok())   return p_error();  // x
   cur_aperture.in = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int roundthermal_out()
{
   if (!get_tok())   return p_error();  // x
   cur_aperture.out = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int roundthermal_nbridge()
{
   if (!get_tok())   return p_error();  // x
   cur_aperture.nbridge = atoi(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int roundthermal_bridgewidth()
{
   if (!get_tok())   return p_error();  // x
   cur_aperture.bridgewidth = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int roundthermal_bridgeangle()
{
   if (!get_tok())   return p_error();  // x
   cur_aperture.bridgeangle = atof(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_width()
{
   if (!get_tok())   return p_error();  // x
   G.cur_width = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_height()
{
   if (!get_tok())   return p_error();  // x
   G.cur_height = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_width()
{
   if (!get_tok())   return p_error();  // x
   cur_text_params.width = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000text_height()
{
   if (!get_tok())   return p_error();  // x
   cur_text_params.height = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int areageometry_surface()
{
   CCr5000LineNumberStamp lineNumberStamp;

   curdata = lineNumberStamp.graphPolyStruct(G.cur_layerindex, 0L, 0);  // this must be a floating layer.

	LayerStruct *lp = doc->getLayer(G.cur_layerindex);

	if (lp != NULL && CompOutlineLayerNameList.Find(lp->getName()))
      curdata->setGraphicClass(graphicClassComponentOutline);

	G.cur_filltype = FALSE;

   int retval = loop_command(areageometrysurface_lst,SIZ_AREAGEOMETRYSURFACE_LST);

   curdata = NULL;

   return retval;
}

/****************************************************************************/
/*
*/
static int textgeometry_text()
{
   return loop_command(texttext_lst,SIZ_TEXTTEXT_LST);
}

/****************************************************************************/
/*
*/
static int graphline_vertex()
{
   int   i, err;

   polyarray.RemoveAll();
   resetPolyCnt();

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   // linewidth is attached to the (pt) record and theoreticalyl can change ! 
   // News flash, it is not theoretical. Case 2001 demonstrates it can change.
   G.cur_widthindex = Graph_Aperture("", G.cur_linepenshape, G.cur_width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if (curdata)
	{
      curpoly = Graph_Poly(NULL,G.cur_widthindex, 0, 0, 0); // fillflag , negative, closed
	}

   for (i=0;i<polycnt;i++)
   {
      CCr5000Pt   p = polyarray.ElementAt(i);

      // Proceed at current width
      if (curdata)
         Graph_Vertex(p.x, p.y, p.bulge);

      // If width changed then current line has ended at this point.
      // Activate new width and start new line.
      if (p.width != G.cur_width && curdata)
      {
         G.cur_widthindex = Graph_Aperture("", G.cur_linepenshape, p.width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         G.cur_width = p.width;
         curpoly = Graph_Poly(NULL,G.cur_widthindex, 0, 0, 0); // fillflag , negative, closed
         Graph_Vertex(p.x, p.y, p.bulge);
      }
   }
   polyarray.RemoveAll();
   resetPolyCnt();



   return 1;

}

/****************************************************************************/
/*
*/
static double angleBetween(Point2 &commonPt, Point2 &ptA, Point2 &ptB)
{
   double theta = 0.0;  // default, same slope

   double dx1 = (ptA.x - commonPt.x);
   double dy1 = (ptA.y - commonPt.y);

   double dx2 = (ptB.x - commonPt.x);
   double dy2 = (ptB.y - commonPt.y);

   if ((fabs(dx1) > .0001) && (fabs(dx2) > .0001))  // Can't be even close to vertical
   {
      // Neither are vertical
      double m1 = dy1 / dx1;
      double m2 = dy2 / dx2;

      // Check for perpendicular
      // magic fuzz, .005 difference in slope we'll consider negligible, found by experimentation with case 1952 data
      if (m1 != m2 && dy1 != 0.0 && dy2 != 0.0 && fabs(m1 - (-1.0 / m2)) < 0.005) // neither horizontal nor parallel, and have reciprocol slopes
      {
         theta = DegToRad(90.0);
      }
      else
      {
         // Get acute angle between vectors
         double tantheta = (m1 - m2) * (1 + (m1 * m2));
         theta = atan(tantheta);
      }
   }
   else if (fabs(dx1) > .0001)
   {
      double m1 = dy1 / dx1;
      theta = DegToRad(90.0) - fabs(atan(m1));
   }
   else if (fabs(dx2) > .0001)
   {
      double m2 = dy2 / dx2;
      theta = DegToRad(90.0) - fabs(atan(m2));
   }
   else
   {
      // Both lines essentially vertical
   }

   theta = fabs(theta);

   // Check if we want acute angle or obtuse
   // If vector from ptA to ptB is less than hypoteneuse using pts to make right
   // triangle, then we want acute angle (we already have acute angle).
   // If it is greater then we want the obtuse angle.
   CPoint2d ppt0(commonPt.x, commonPt.y);
   CPoint2d pptA(ptA.x, ptA.y);
   CPoint2d pptB(ptB.x, ptB.y);
   double leg0A = ppt0.distance(pptA);
   double leg0B = ppt0.distance(pptB);
   double legAB = pptA.distance(pptB);
   double hypot = sqrt((leg0A * leg0A) + (leg0B * leg0B));
   if (theta != 0.0 && legAB > hypot)
      theta = PI - theta;

   return theta;
}

/****************************************************************************/
/*
*/
static int graphsurface_vertex()
{
   int   i;

   G.cur_widthindex = doc->getZeroWidthIndex();
   polyarray.RemoveAll();
   resetPolyCnt();

   if (curdata != NULL)
   {
      curpoly = Graph_Poly(NULL,G.cur_widthindex, G.cur_filltype, 0, 1);   // fillflag , negative, closed	
	}

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

#define TARC_ENABLED true

   if (polycnt && curdata)
   {
      // Process TARC commands
      // We are assuming polyarray defines a closed shape if poly starts with tarc

      for (i = 0; i < polycnt; i++)
      {
         int thispoint = i;

         int prevpoint = 0;
         int nextpoint = 0;

         if (thispoint > 0)
            prevpoint = thispoint - 1; // previous point
         else
            prevpoint = polycnt - 1;  // last point (is previous point in closed shape)

         if (thispoint < (polycnt - 1))
            nextpoint = thispoint + 1;
         else
            nextpoint = 0;  // back to beginning


			CCr5000Pt  &p1 = polyarray.ElementAt(thispoint);
			
         if (TARC_ENABLED && p1.isTarc)
         {  
            CCr5000Pt &p0 = polyarray.ElementAt(prevpoint);
            CCr5000Pt  &p2 = polyarray.ElementAt(nextpoint);

            // Get the setback from the common point.
            // Point P1 is a common point between two vectors, P1->P0 and P1->P2
            // Get the angle between them.
            // Half this angle creates a right-triangle, where the tarc radius is
            // a leg (and is the leg opposite this angle) and the setback is a leg.
            // The setback leg starts at p1 and goes to where the tarc radius leg
            // meets line p1->p0, which is the tangent point of the arc on this line.
            // We'll get the hypoteneuse later, when we have use for it.
            Point2 cr5pt0(p0.cr5_x, p0.cr5_y, 0.0);
            Point2 cr5pt1(p1.cr5_x, p1.cr5_y, 0.0);
            Point2 cr5pt2(p2.cr5_x, p2.cr5_y, 0.0);

            double theta = angleBetween(cr5pt1, cr5pt0, cr5pt2);

            if (theta == 0.0)
            {
               // Parallel lines, cancel TARC, use original pt in polyline
               p1.isTarc = false;
            }
            else
            {
               double len;
               if (fabs(RadToDeg(theta) - 90.0) < 0.1)
               {
                  len = p1.tarcRadius;
               }
               else
               {
                  double halftheta = theta / 2.0;

                  // tan(theta) = legB / legA, legA * tan(theta) = legB, legA = legB / tan(theta)
                  // legB is the radius, the setback is legA
                  len = p1.tarcRadius;
                  if (halftheta != 0.0)
                  {
                     len = p1.tarcRadius / tan(halftheta); // aka legA
                  }
               }
               double setback = abs(len);

               CPoint2d p02d(p0.cr5_x, p0.cr5_y);
               CPoint2d p12d(p1.cr5_x, p1.cr5_y);
               CPoint2d p22d(p2.cr5_x, p2.cr5_y);
               double p1p0dist = p12d.distance(p02d);
               double p1p2dist = p12d.distance(p22d);

               // Arc start point is set back from p1 toward p0
               double arcStartX = p0.cr5_x;
               double arcStartY = p0.cr5_y;
               if (p1p0dist > setback)
               {
                  if (!FindPointOnLine(p1.cr5_x, p1.cr5_y, p0.cr5_x, p0.cr5_y, setback, &arcStartX, &arcStartY))
                  {
                     p1.isTarc = false; // abort tarc
                  }
               }

               // Arc end point is set back from p1 toward p2.
               double arcEndX = p2.cr5_x;
               double arcEndY = p2.cr5_y;
               if (p1p2dist > setback)
               {
                  if (!FindPointOnLine(p1.cr5_x, p1.cr5_y, p2.cr5_x, p2.cr5_y, setback, &arcEndX, &arcEndY))
                  {
                     p1.isTarc = false; // abort tarc
                  }
               }

               /// Find arc center point
               /// The following is done in a couple different styles. It is surprising how
               // different the results look in numbers, but on the camcad display they all
               // look the same. I suppose it is roundoff error in the various types of calcs
               // done. It is not clear at the moment if one way is "a lot better" than another.
               // They are both being kept on hand for awhile, in case more TARC issues come up,
               // particularly if it is an accuracy issue.


               // Find arc center point
               /// ------------------------------------------------------------------------
//#define STYLE_1
#ifdef  STYLE_1
               /// STYLE1 Works okay, but is applying some guess work to decide between
               /// two possible center pts. Has worked okay for all samples tried, but
               /// I got another idea.

               // The point given in the data is always on the outside of the arc,
               // we want the center of the arc that would not contain this point.
               // This turns out to be the center pt that is furthest from the pt in the data.
               double centXstyle1 = 0.0;
               double centYstyle1 = 0.0;
               double cx1, cy1, cx2, cy2;
               int ret = 0;

               // Get arc centers, might need to fudge radius if roundoff in previous calcs
               // bumped the start and end pts further apart than arc diameter.
               int fudgecnt = 0; // prevent infinite loop
               while (fudgecnt < 100000 && !(ret = ArcFrom2Pts(arcStartX, arcStartY, arcEndX, arcEndY, p1.tarcRadius,
                  &cx1, &cy1, &cx2, &cy2)))
               {
                  p1.tarcRadius *= 1.01;
                  fudgecnt++;
               }

               if (ret == 0)
               {
                  // We're hosed, no arc center
                  centXstyle1 = p1.cr5_x;
                  centYstyle1 = p1.cr5_y;
                  p1.isTarc = false; // abort tarc
               }
               else if (ret == 1)
               {
                  // Only 1, always the first one then
                  centXstyle1 = cx1;
                  centYstyle1 = cy1;
               }
               else if (ret == 2)
               {
                  // Take center furthest from pt in cad data.
                  // The tarc definition point p1 in cad is always outside the arc, so
                  // arc center furthest from p1 is the one we want.
                  CPoint2d cadPt(p1.cr5_x, p1.cr5_y);
                  if (cadPt.distance(cx1, cy1) > cadPt.distance(cx2, cy2))
                  {
                     centXstyle1 = cx1;
                     centYstyle1 = cy1;
                  }
                  else
                  {
                     centXstyle1 = cx2;
                     centYstyle1 = cy2;
                  }

               }

#else
               /// -----------------------------------------------------------------------

               // Style 2
               // The setback and tarc radius define two legs of a right triangle.
               // The hypoteneuse then is the distance from p1 to center of arc.
               double hypot = sqrt((p1.tarcRadius * p1.tarcRadius) + (setback * setback));

               // Get two vectors of same length, p0prime along p1->p0, p2prime along p1->p2.
               // Then get the midpoint between p0prime and p2prime, this midpoint will lie on 
               // the line that bisects the angle between p1p0 and p1p2.
               double arbLength = 100.0; // arbitrary length
               double p0primeX = 0.0, p0primeY = 0.0;
               if (!FindPointOnLine(p1.cr5_x, p1.cr5_y, p0.cr5_x, p0.cr5_y, arbLength, &p0primeX, &p0primeY))
               {
                  p1.isTarc = false; // abort tarc
               }
               double p2primeX = 0.0, p2primeY = 0.0;
               if (!FindPointOnLine(p1.cr5_x, p1.cr5_y, p2.cr5_x, p2.cr5_y, arbLength, &p2primeX, &p2primeY))
               {
                  p1.isTarc = false; // abort tarc
               }
               Point2 midpoint( ((p0primeX + p2primeX) / 2.0), ((p0primeY + p2primeY) / 2.0) );

               // Center of arc is hypot distance from p1 towards midpoint
               double centX = 0.0, centY = 0.0;
               if (!FindPointOnLine(p1.cr5_x, p1.cr5_y, midpoint.x, midpoint.y, hypot, &centX, &centY))
               {
                  p1.isTarc = false; // abort tarc
               }

               // Midpoint on arc is one radius from arc center toward p1
               double arcMiddleX = 0.0, arcMiddleY = 0.0;
               if (!FindPointOnLine(centX, centY, p1.cr5_x, p1.cr5_y, p1.tarcRadius, &arcMiddleX, &arcMiddleY))
               {
                  p1.isTarc = false; // abort tarc
               }
#endif


               // Find delta angle
               /// -------------------------------
#ifdef STYLE_1
               /// Style 1
               // There are two possible arcs, clockwise or counter clockwise.
               // Check both, take smallest delta angle
               double centX = centXstyle1;
               double centY = centYstyle1;
               double anotherRadius, sa1, da1, sa2, da2;
               ArcCenter2(arcStartX, arcStartY, arcEndX, arcEndY, centX, centY,
                  &anotherRadius, &sa1, &da1, TRUE /*CW*/ ); 
               ArcCenter2(arcStartX, arcStartY, arcEndX, arcEndY, centX, centY,
                  &anotherRadius, &sa2, &da2, FALSE /*CCW*/ );
               double deltaangle = fabs(da1) < fabs(da2) ? da1 : da2;

#else
               /// --------------------------------
               /// Style 2
               double aaCx, aaCy, aaR, startangle, deltaangle;
               ArcPoint3( arcStartX, arcStartY, arcMiddleX, arcMiddleY, arcEndX, arcEndY,
                  &aaCx, &aaCy, &aaR, &startangle, &deltaangle );

               /// --------------------------------
#endif


               p1.tarcPt1.x = arcStartX;
               p1.tarcPt1.y = arcStartY;
               p1.tarcPt1.bulge = tan(deltaangle / 4.0);

               p1.tarcPt2.x = arcEndX;
               p1.tarcPt2.y = arcEndY;
               p1.tarcPt2.bulge = 0.0;
            }
         }
      }


      // Create camcad graphics
      for (i=0;i<polycnt;i++)
      {
         CCr5000Pt p = polyarray.ElementAt(i);

         if (TARC_ENABLED && p.isTarc)
         {
            Graph_Vertex(p.tarcPt1.x, p.tarcPt1.y, p.tarcPt1.bulge);
            Graph_Vertex(p.tarcPt2.x, p.tarcPt2.y, p.tarcPt2.bulge);
         }
         else
         {
            // Normal pt
            Graph_Vertex(p.x, p.y, p.bulge);
         }
      }
      // close last
      CCr5000Pt p = polyarray.ElementAt(0);
      if (TARC_ENABLED && p.isTarc)
         Graph_Vertex(p.tarcPt1.x, p.tarcPt1.y, 0.0); // only draw to beginning of arc
      else
         Graph_Vertex(p.x, p.y, 0.0); // normal point

   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
*/
static int graphopenshape_vertex()
{
   int   i;

   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();

	if (curdata)
	{
      curpoly = Graph_Poly(NULL,G.cur_widthindex, G.cur_filltype, 1, 1);   // fillflag , negative, closed
	}

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt && curdata)
   {
      for (i=0;i<polycnt;i++)
      {
         Point2   p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);
      }
      // close last
      Point2   p = polyarray.ElementAt(0);
      Graph_Vertex(p.x, p.y, 0.0);
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
*/
static int grapharea_vertex()
{
   int   i;

   G.cur_widthindex = 0;
   polyarray.RemoveAll();
   resetPolyCnt();

   if (curdata)
	{
      curpoly = Graph_Poly(NULL,G.cur_widthindex, G.cur_filltype, 0, 1); //  negative, closed
	}

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt && curdata)
   {
      for (i=0;i<polycnt;i++)
      {
         Point2   p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);
      }
      // close last
      Point2   p = polyarray.ElementAt(0);
      Graph_Vertex(p.x, p.y, 0.0);
   }

   polyarray.RemoveAll();
   resetPolyCnt();

   return 1;

}

/****************************************************************************/
/*
*/

static int grapharea_fillwidth()
{
	// same as..
	return graphsurface_fillwidth();
}

/****************************************************************************/
/*
*/
static int graphline_penshape()
{
   if (!get_tok())   p_error();  // 

   if (!STRCMPI(token,"SQUARE"))
      G.cur_linepenshape = T_SQUARE;
   else
   {
#ifdef _DEBUG
      fprintf(ferr,"Unknown PENSHAPE [%s]\n", token);
      display_error++;
#endif
      G.cur_linepenshape = T_ROUND;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pcfheader_version()
{
   double   version;

   if (!get_tok())   p_error();  // major
   version = atof(token);

   if (version != 2.0)
   {
      CString tmp;
      tmp.Format("CR5000 Software Revision %1.1lf not supported!\n\
Unsupported Revision may cause software problems.\n",version);
      ErrorMessage(tmp,"CR5000 Version Warning", MB_OK | MB_ICONHAND);
   }

   cr5000_version.pcf = version;

   return 1;
}

/****************************************************************************/
/*
*/
static int ftfheader_version()
{
   double   version;

   if (!get_tok())   p_error();  // major
   version = atof(token);

   if (version != 2.0)
   {
      CString tmp;
      tmp.Format("CR5000 Software Revision %1.1lf not supported!\n\
Unsupported Revision may cause software problems.\n",version);
      ErrorMessage(tmp,"CR5000 Version Warning", MB_OK | MB_ICONHAND);
   }

   cr5000_version.ftf = version;

   return 1;
}

/****************************************************************************/
/*
*/
static int update_padstack(BlockStruct *b, int pinnr, const char *padstackname)
{
   int      found = 0;

   CString  pinnumber;
   pinnumber.Format("%d",pinnr);

   DataStruct *np;
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            // pinname is in refname if a insert

            // these are absolute instructions.
            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_COMPPINNR, 0);
            if (a)
            {
               CString l;
               l = get_attvalue_string(doc, a);

               if (pinnumber.Compare(l) == 0)
               {
                  BlockStruct *block = Graph_Block_On(GBO_APPEND,padstackname,-1,0);
                  Graph_Block_Off();

                  np->getInsert()->setBlockNumber(block->getBlockNumber());
                  found++;
                  break;
               }
            }
         }
      }
   } 
   if (!found)
   {
      fprintf(ferr,"Update error : Geometry [%s] Pinnr [%d] Padstack [%s]\n",
         b->getName(), pinnr, padstackname);
   }

   return found;
}

/****************************************************************************/
/*
*/
static int cr5000_unit()
{
   if (!get_tok())   return p_error();  // x

   if (!STRCMPI(token,"MM"))
      faktor = Units_Factor(UNIT_MM, page_units);  // cr5000 units to page units
   else
   if (!STRCMPI(token,"INCH"))
      faktor = Units_Factor(UNIT_INCHES, page_units); // cr5000 units to page units
   else
   if (!STRCMPI(token,"MIL"))
      faktor = Units_Factor(UNIT_MILS, page_units);   // cr5000 units to page units
   else
   if (!STRCMPI(token,"DBUNIT"))
      faktor = Units_Factor(UNIT_TNMETER, page_units);   // cr5000 units to page units
   else
   {
      fprintf(ferr,"Unknown UNIT [%s]\n", token);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_pt_width()
{
   if (!get_tok())   return p_error();  // x
   G.cur_width = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int tarc_radius()
{
   if (!get_tok())   return p_error();  // radius
   G.cur_tarc_radius = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
*/
static int pt_tarc()
{
	// tarc aka tangent arc

	if (!get_tok())   return p_error();  // ON, OFF, or SIMPLE
	CString tarcSwitch = token;
	G.cur_tarc_on = tarcSwitch.CompareNoCase("ON") == 0 || tarcSwitch.CompareNoCase("SIMPLE") == 0;

   if (loop_command(tarc_lst, SIZ_TARC_LST) < 0)
      return -1;

	return 1;
}

/****************************************************************************/
/*
*/
static int cr5000_pt()
{
#ifdef EnableVertexTracking
   CVertexEntry entry(vertexEntryPt);
#endif

   CCr5000Pt   p2;

   if (!get_tok())   return p_error();  // x

   p2.x = cnv_unit(token);

   if (!get_tok())   return p_error();  // y

   p2.y = cnv_unit(token);

   p2.bulge = 0;

   // only change width if a new width is encountered.
   // G.cur_width = 0;

	G.cur_tarc_on = false;
   if (loop_command(ptparam_lst,SIZ_PTPARAM_LST) < 0)
      return -1;

   p2.cr5_x = p2.x;
   p2.cr5_y = p2.y;
   p2.width = G.cur_width; // case 2001, yep, cr5000 has width per point in polyline
   p2.isTarc = G.cur_tarc_on;
   p2.tarcRadius = G.cur_tarc_radius;

   if (polycnt)
   {
      CCr5000Pt   p1 = polyarray.ElementAt(polycnt-1);
      // if this is the same coo as the last one, do not write it.
      // this seems to happen a lot on pt arc combinations.
      if (p1.x == p2.x && p1.y == p2.y && p1.isTarc == p2.isTarc)  
         return 1;
   }

   polyarray.SetAtGrow(polycnt, p2);
   polycnt++;

   return 1;
}

/****************************************************************************/
/*
   CCW / CW
   radius
   begin (pt...)
   center x y        <=== it look like the center points are wrong
   end ( pt ...)
*/
static int cr5000_arc()
{
#ifdef EnableVertexTracking
   CVertexEntry entry(vertexEntryArc);
#endif

   if (!get_tok())
		return p_error();

   bool clockwise = false;
   if (!STRCMPI(token, "CW"))
      clockwise = true;

   G.cur_width = 0;
   cur_arc.centerx = 0.;
   cur_arc.centery = 0.;

   // arc has begin pt.. and end pt.. list
   if (loop_command(arc_lst,SIZ_ARC_LST) < 0)
      return -1;

   // here I need to update the bulge of begin pt...

   if (polycnt == 1) // full circle
   {
      CCr5000Pt startPoint = polyarray.ElementAt(polycnt-1);   // start koo
      startPoint.bulge = 1.0;
      polyarray.SetAt(polycnt-1,startPoint);

      Point2 centerPoint(cur_arc.centerx,cur_arc.centery);
      centerPoint = correctCenterCoordinate(centerPoint,startPoint,cur_arc.radius);

      CCr5000Pt oppositePoint;

      // (centerPoint - startPoint) + centerPoint == 2*centerPoint - startPoint
      oppositePoint.x = 2.*centerPoint.x - startPoint.x;
      oppositePoint.y = 2.*centerPoint.y - startPoint.y;

      oppositePoint.bulge = 1;
      polyarray.SetAtGrow(polycnt,oppositePoint);                 // mid koo
      polycnt++;

      startPoint.bulge = 0.0;
      polyarray.SetAtGrow(polycnt,startPoint);                // end koo
      polycnt++;
   }
   else
   {
		// Calculate bulge
      CCr5000Pt p2 = polyarray.ElementAt(polycnt-1);   // 
      CCr5000Pt p1 = polyarray.ElementAt(polycnt-2);   // update bulge on first koo
      
      double sa = 0.0;
		double da = 0.0;
		double cx = 0.0;
		double cy = 0.0;

		double tolerance = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());

      int res = ArcFromStartEndRadiusDeltaXY(p1.x, p1.y, p2.x, p2.y, cur_arc.radius, clockwise, 
			cur_arc.centerx, cur_arc.centery,  &cx, &cy, &sa, &da, tolerance);

		if (res > 0)
		{
			p1.bulge = (DbUnit)tan(da/4);
			polyarray.SetAt(polycnt-2,p1);
		}
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pin_pt()
{

   if (!get_tok())   return p_error();  // x
   cur_pin.x = cnv_unit(token);
   if (!get_tok())   return p_error();  // y
   cur_pin.y = cnv_unit(token);

   fskip();

   return 1;
}

/****************************************************************************/
/*
*/
static int compdef_compheader()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int compdef_comppin()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int textstyle_justify()
{
   if (!get_tok())   return p_error();

   if (!STRCMPI(token, "JUST_START"))
      G.cur_textjust = GRTEXT_W_L;
   else
   if (!STRCMPI(token, "JUST_CENTER"))
      G.cur_textjust = GRTEXT_W_C;
   else
   if (!STRCMPI(token, "JUST_END"))
      G.cur_textjust = GRTEXT_W_R;
   else
   {
      fprintf(ferr,"Unknown Text Justification [%s] at %ld\n", token, ifp_line);
   }

   return 1;
}

/******************************************************************************
* font_textstyle
*/
static int font_textstyle()
{
   return 1;
}

/****************************************************************************/

/******************************************************************************
* tok_search
*/
static int tok_search(List *tok_lst,int tok_size)
{
   for (int i=0; i<tok_size; ++i)
	{
      if (!STRCMPI(token,tok_lst[i].token))
         return(i);
	}

   return(-1);
}

/******************************************************************************
* get_tok
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
   static char *curp;
   int i = 0;

   token_name = FALSE;

	bool escape = false;

   if (newlp)
      curp = lp;

   for (; isspace(*curp) && *curp != '\0'; ++curp) ;

   switch(*curp)
   {
   case '\0':
      return 0;
   case '(':
   case ')':
      token[i++] = *(curp++);
      break;
   case '\"':
      {
         token[i] = '\0';
         token_name = TRUE;
         for (++curp; (*curp != '\"' || escape); ++curp, ++i)
         {
				escape = false;

            if (i >= MAX_TOKEN-1)
            {
					CString msg;
					msg.Format("Error in Get Token - Token too long at line %ld.", ifp_line);
					fprintf(ferr, "%s\n", msg);
					ErrorMessage(msg);                   
               display_error++;
					return FALSE;
            }

            if (*curp == '\n')
            {
               token[i] = '\n';
               i++;

               /* text spans over 1 line */
               do
               {
                  if (!get_line(cur_line, MAX_LINE))
                  {
                     ErrorMessage("Error in Line Read.  File ended at unexpected place.  Please check file for invalid string.\n");
                     return FALSE;
                  }
               } while (cur_line[0] == '\n');
               curp = cur_line;

               if (*curp == '"')
                  break;
					else if (*curp == '\\')
						escape = true;
					else
						token[i] = *curp;
            }
            else if (*curp == '\\')
            {
               ++curp;
               if (*curp == '\n')
               {
                  /* text spans over 1 line */
                  if (!get_line(cur_line, MAX_LINE))
                  {
                     fprintf(ferr, "Error in Line Read\n");
                     display_error++;
                  }
                  curp = cur_line;
               }
               else if (*curp == '0')
               {
                  // here is the decimal char number
                  char num[4];
                  ++curp;
                  num[0] = *curp;
                  ++curp;
                  num[1] = *curp;
                  ++curp;
                  num[2] = *curp;
                  num[3] = '\0';
                  token[i] = atoi(num);
               }
               else if (*curp == 'n')
                  token[i] = '\n';
               else
                  token[i] = *curp;
            }
            else if (*curp != '\0')
               token[i] = *curp;
         }
         ++curp;
      }
      break;
   default:
      {
         for (; !isspace(*curp) && *curp != '\"' && *curp != '(' && *curp != ')' && *curp != '\0'; ++curp, ++i)
         {
            if (*curp == '\\')
               ++curp;
            token[i] = *curp;
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
   from the ; to end of line is a remark, but not in text
*/
static int get_line(char *cp, int size)
{
   if (!fgets(cp, size, ifp))
   {
      if (!feof(ifp))
      {
         ErrorMessage("Read Error", "Fatal CR5000 Parse Error", MB_OK | MB_ICONHAND);
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
      if (cp[strlen(cp)-1] != '\n')
      {
         fprintf(ferr, " No end of line found at %ld\n", ifp_line);
         return (FALSE);
      }
   }

   if ((ifp_line % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
   {
      fileReadProgress->updateProgress();
   }

   ++ifp_line;

   //if (ifp_line == 32367)
   //{
   //   int iii = 3;
   //}

   // ; not as text \;
   for (int i=0; i<(int)strlen(cp); i++)
   {
      if (cp[i] == '\\')   
      {
         i++;
         continue;
      }
      if (cp[i] == ';') // this is remark
      {
         cp[i] = '\0';
         return TRUE;
      }
   }

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
   tmp.Format("Fatal Parsing Error : Token \"%s\" in file [%s] on line %ld\n", 
      token, cur_filename, ifp_line);
   t  = tmp;
   
   ErrorMessage(t, "Fatal CR5000 Parse Error", MB_OK | MB_ICONHAND);
   
   return -1;
}

/****************************************************************************/
/*
*/
void init_all_mem()
{

   polyarray.SetSize(100,100);
   resetPolyCnt();

   drillarray.SetSize(100,100);
   drillcnt = 0;

   compinstarray.SetSize(100,100);
   compinstcnt = 0;

	cr5000FootprintLayerArray.SetSize(100,100);
	cr5000FootprintLayerCount = 0;

	cr5000BoardLayerArray.SetSize(100,100);
	cr5000BoardLayerCount = 0;

   attribmaparray.SetSize(100,100);
   attribmapcnt = 0;

   memset(&G,0,sizeof(Global));

   return;
}

void free_all_mem()
{
	delete MapA;
	delete MapB;
	delete MapThruA;
	delete MapThruB;

   int   i;

   for (i=0;i<compinstcnt;i++)
   {
      delete compinstarray[i];
   }
   compinstarray.RemoveAll();


	for (i=0;i<cr5000FootprintLayerCount;i++)
	{
		delete cr5000FootprintLayerArray[i];
	}
	cr5000FootprintLayerArray.RemoveAll();

	for (i=0;i<cr5000BoardLayerCount;i++)
	{
		delete cr5000BoardLayerArray[i];
	}
	cr5000BoardLayerArray.RemoveAll();


   for (i=0;i<attribmapcnt;i++)
   {
      delete attribmaparray[i];
   }
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   drillarray.RemoveAll();

   return;
}


static int get_drillindex(double size)
{
   CR5000Drill p;
   int         i;

   if (size == 0) return -1;

   for (i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
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
double   cnv_unit_noCorrection(const char *s)
{
   double x;
   if (STRLEN(s) == 0)  return 0.0;
   x = atof(s);

   return x * faktor;
}

double   cnv_unit(const char *s)
{
   double x;

   if (STRLEN(s) == 0)  return 0.0;

   x = atof(s);

   // here check for unreasonable large cadifunit
   if (fabs(x) > 100 * 2540000)  // if X greater 100 inch -- something is wrong
   {
      fprintf(ferr,"Error in CR5000 file [%s]: Uninitialized Coordinate [%s] at %ld\n",
         cur_filename, s,ifp_line);    
      display_error++;
      x = 0;
   }

   return x * faktor;
}

/****************************************************************************/
/*
*/
static int get_fontptr(const char *f)
{

   return -1;
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
      layer_attr_cnt++;
   }
   else
   {
      fprintf(ferr,"Too many layers\n");
      return -1;
   }

   return layer_attr_cnt-1;
}


/*
*/
static int Cr5000LoadSettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

	CompOutlineLayerNameList.RemoveAll();
   IGNORE_OUTOFBOARD = FALSE;
   IGNORE_EMPTYPADCODE_PINS = FALSE;
	PURGE_UNUSED_BLOCKS = true;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "CR5000 Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".UNDEFINED_PADSTACK_SIZE"))
         {
				// .UNDEFINED_PADSTACK_SIZE <units> <pad size> <drill size>
				// Units, MM or IN
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            CString padsizeunits = lp;
				PageUnitsTag padunits = pageUnitsUndefined;
				if (padsizeunits.CompareNoCase("mm") == 0)
					padunits = pageUnitsMilliMeters;
				else if (padsizeunits.CompareNoCase("in") == 0)
					padunits = pageUnitsInches;
				else
					fprintf(ferr, "IN file settings, .UNDEFINED_PADSTACK_SIZE, Units \"%s\" not recognized.\n", padsizeunits);


				// Pad dia
				if ((lp = get_string(NULL," \t\n")) == NULL) continue;
				CString padDiaStr = lp;

				// Drill dia
				if ((lp = get_string(NULL," \t\n")) == NULL) continue;
				CString drillDiaStr = lp;

				if (padunits != pageUnitsUndefined && !padDiaStr.IsEmpty())
				{
					if (drillDiaStr.IsEmpty())
						drillDiaStr = padDiaStr;

					double padDia = atof(padDiaStr);
					double drillDia = atof(drillDiaStr);
					if (padDia > 0.0 && drillDia > 0.0)
					{
						DefaultPadApertureDia = doc->convertToPageUnits(padunits, padDia);
						DefaultPadDrillDia = doc->convertToPageUnits(padunits, drillDia);
					}
				}

         }
         else
         if (!STRCMPI(lp,".PURGE_UNUSED_BLOCKS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            PURGE_UNUSED_BLOCKS = (lp[0] == 'Y' || lp[0] == 'y');
         }
         else
         if (!STRCMPI(lp,".IGNORE_OUTOFBOARD"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               IGNORE_OUTOFBOARD = TRUE;
         }
         else 
         if (!STRCMPI(lp,".IGNORE_EMPTYPADCODE_PINS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               IGNORE_EMPTYPADCODE_PINS = TRUE;
         }
         else
         if (!STRCMPI(lp,".SHOW_HIDDEN_REFDES"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            ShowHiddenRefdes = (lp[0] == 'Y' || lp[0] == 'y');
         }
         else         
			if (!STRCMPI(lp,".SUPPRESS_CLEARANCE_PADS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            SuppressClearancePads = (lp[0] == 'Y' || lp[0] == 'y');
         }
         else
         if (!STRCMPI(lp,".SUPPRESS_THERMAL_PADS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            SuppressThermalPads = (lp[0] == 'Y' || lp[0] == 'y');
         }
         else
         if (!STRCMPI(lp,".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            ComponentSMDrule = atoi(lp);
         }
         else
			if (!STRICMP(lp, ".FIDUCIALATTR") || !STRICMP(lp, ".MAKE_INTO_FIDUCIAL"))
			{
				if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

				AttrSettings->UpdateAttr(lp, attrSettingFiducial);
			}
			else
			if (!STRICMP(lp, ".TESTPOINTATTR") || !STRICMP(lp, ".MAKE_INTO_TESTPOINT"))
			{
				if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

				AttrSettings->UpdateAttr(lp, attrSettingTestPoint);
			}
         else
			if (!STRICMP(lp, ".TOOLINGATTR") || !STRICMP(lp, ".MAKE_INTO_TOOLING"))
			{
				if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

				AttrSettings->UpdateAttr(lp, attrSettingTooling);
			}
         else
			if (!STRICMP(lp, ".COMPOUTLINE"))
			{
				if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

				if (!CompOutlineLayerNameList.Find(lp))
					CompOutlineLayerNameList.AddTail(lp);
			}
			else
         if (!STRICMP(lp,".ATTRIBMAP"))
         {
            CString  a1,a2;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            a1 = lp;
            a1.MakeUpper();
            a1.TrimLeft();
            a1.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            a2 = lp;
            a2.MakeUpper();
            a2.TrimLeft();
            a2.TrimRight();

            CR5000Attribmap   *c = new CR5000Attribmap;
            attribmaparray.SetAtGrow(attribmapcnt,c);

            c->attrib = a1;
            c->mapattrib = a2;
            attribmapcnt++;
            
         }
         else
         if (!STRICMP(lp,".LAYERATTR"))
         {
            CString  tllayer;
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            tllayer = _strupr(lp);
            tllayer.TrimLeft();
            tllayer.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);
            int layindex = get_lay_index(tllayer);
            layer_attr[layindex].attr = laytype;
         }
         else
         if (!STRICMP(lp,".MIRRORLAYER"))
         {
            CString  lay1,lay2;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            lay1 = lp;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            lay2 = lp;

            if (layer_mirrorcnt < 255)
            {
               layer_mirror[layer_mirrorcnt].lay1 = lay1;
               layer_mirror[layer_mirrorcnt].lay2 = lay2;
               layer_mirrorcnt++;
            }
         }
      }
   }

   fclose(fp);
   return 1;
}

static CString findInFileLayerAttrType(CString cr5layername)
{
   for (int i=0;i<layer_attr_cnt;i++)
   {
      if (cr5layername.CompareNoCase(layer_attr[i].name) == 0)
      {
         CString layertypeStr = layerTypeToString(layer_attr[i].attr);
         return layertypeStr;
      }
   }

   return "";
}

/****************************************************************************/
/*
*/
static void validateSurfaceLayers()
{
   // During some layer manipulations some layers may have been assigned an
   // xxxxx_TOP or xxxxx_BOT that are not actually a surface layer. Use the
   // layer maps from cad to find these and set them to UNKNOWN GRAPHIC.

   // Just processing SMD layer maps, that's where the problem has been seen.

	int mapaSize = MapA->getSize();
	for (int i = 0; i < MapA->getSize(); i++)
	{
		CLayerMapEntry *meA = MapA->getAt(i);
		if (meA != NULL)
		{
			CString footlayername = meA->m_footlayerName;

         CLayerMapEntry *meB = NULL;
         if (MapB->lookup(footlayername, meB))
         {
			   for (int j = 0; j < meA->m_boardlayerNames.GetCount(); j++)
			   {
				   CString mapA_brdlayer = meA->m_boardlayerNames.GetAt(j);

			      for (int k = 0; k < meB->m_boardlayerNames.GetCount(); k++)
			      {
                  CString mapB_brdlayer = meB->m_boardlayerNames.GetAt(k);

                  // If footprint layer is mapped to same board layer on top and
                  // bottom, then the layer can not be a surface layer type in
                  // camcad.
                  if (mapA_brdlayer.CompareNoCase(mapB_brdlayer) == 0)
                  {
                     CR5000LayerDef *brdld = getCr5000BoardLayer(mapA_brdlayer);
                     if (brdld != NULL)
                     {
                        brdld->boardLayerType = "";
                     }
                     LayerStruct *lp = doc->FindLayer_by_Name(mapA_brdlayer);
                     if (lp != NULL)
                     {
                        int oldtype = lp->getLayerType();
                        CString oldtypeStr = layerTypeToString(oldtype);
                        oldtypeStr.MakeUpper();
                        if (oldtypeStr.Find("TOP") > -1 || oldtypeStr.Find("BOT") > -1)
                        {
                           lp->setLayerType(layerTypeUnknown);
                           // Update it's mirror too, if there is one
                           LayerStruct *mirlp = doc->FindLayer(lp->getMirroredLayerIndex());
                           if (mirlp != NULL && mirlp != lp)
                              lp->setLayerType(layerTypeUnknown);

                           CString usersBadType = findInFileLayerAttrType(mapA_brdlayer);
                           if (!usersBadType.IsEmpty())
                           {
                              fprintf(ferr, "IN File Error: .LAYERATTR %s %s is invalid, layer is not a surface layer.\n",
                                 mapA_brdlayer, usersBadType);
                           }
                        }
                        
                        
                     }

                  }
               }
            }
			}
		}
	}
}

/****************************************************************************/
/*
*/
static int camcadLayerType(CR5000LayerDef *ld)
{
	if (ld != NULL)
	{
		CString cr5000LayerType = ld->footprintLayerType;
		if (cr5000LayerType.IsEmpty())
			cr5000LayerType = ld->boardLayerType;

		if (ld->conductiveLayerNumber < 1)
		{
			// Punt. Is probably non-conductive layer, and we've not found an explicit
			// way to tell top from bottom. There is a suffix on some of the names. 
			// Try using that to get a layer number.
			if (ld->boardLayerName.Find("-A") > -1)
				ld->conductiveLayerNumber = 1;
			else if (ld->boardLayerName.Find("-B") > -1)
				ld->conductiveLayerNumber = G.numberoflayers;
			else
				ld->conductiveLayerNumber = 1;
		}

		cr5000LayerType.MakeUpper();

		if (cr5000LayerType.CompareNoCase("METALMASK") == 0)
			return ld->conductiveLayerNumber == 1 ? LAYTYPE_PASTE_TOP : LAYTYPE_PASTE_BOTTOM;

		if (cr5000LayerType.CompareNoCase("SOLDERRESIST") == 0)
			return ld->conductiveLayerNumber == 1 ? LAYTYPE_MASK_TOP : LAYTYPE_MASK_BOTTOM;

		if (cr5000LayerType.CompareNoCase("SYMBOLMARK") == 0)
			return ld->conductiveLayerNumber == 1 ? LAYTYPE_SILK_TOP : LAYTYPE_SILK_BOTTOM;

		if (cr5000LayerType.CompareNoCase("HOLE") == 0)
			return LAYTYPE_DRILL;

		if (cr5000LayerType.CompareNoCase("VARIANTHOLE") == 0)
			return LAYTYPE_DRILL;

		// From .IN file
      /// Case 2062 Rebug - Though this code was not changed for case 2062, notes in
      /// rebug say to stop doing this. Was confirmed with David and Dean via email.
      ///
		///if (CompOutlineLayerNameList.Find(ld->boardLayerName))
      ///   return LAYTYPE_COMPONENTOUTLINE;

		// Last ditch - if it is a conductive layer then make is a SIGNAL layer
		if (ld->conductive)
		{
			if (ld->conductiveLayerNumber == 1)
				return LAYTYPE_SIGNAL_TOP;
			if (ld->conductiveLayerNumber == G.numberoflayers)
				return LAYTYPE_SIGNAL_BOT;
			if (ld->conductiveLayerNumber > 1 && ld->conductiveLayerNumber < G.numberoflayers)
				return LAYTYPE_SIGNAL_INNER;
			
			// If still here then error situation, we have conductive layer type
			// with no correlation to which layer
			fprintf(ferr, "Conductive layer %s (%s) has no mapping to electrical layer number.\n",
				ld->footprintLayerName, ld->boardLayerName);
		}

	}

	return LAYTYPE_UNKNOWN;
}

/****************************************************************************/
/*
*/
static void set_mirror_layer(LayerStruct *lp)
{
	// Work from the top side, scan for top side layer types and set
	// bottom side as mirror. Don't need to also scan bottoms and set
	// top side.

	if (lp != NULL)
	{
		if (lp->getLayerIndex() != lp->getMirroredLayerIndex())
			return;	// mirror already set

#ifdef NICE_TRY_BUT_MAKES_BAD_MATCHES
		int mir_laytype = LAYTYPE_UNKNOWN;
		int laytype = lp->getLayerType();
		switch (laytype)
		{
		case LAYTYPE_SIGNAL_TOP:
			mir_laytype = LAYTYPE_SIGNAL_BOT;
			break;
		case LAYTYPE_PAD_TOP:
			mir_laytype = LAYTYPE_PAD_BOTTOM;
			break;
		case LAYTYPE_PASTE_TOP:
			mir_laytype = LAYTYPE_PASTE_BOTTOM;
			break;
		case LAYTYPE_MASK_TOP:
			mir_laytype = LAYTYPE_MASK_BOTTOM;
			break;
		case LAYTYPE_SILK_TOP:
			mir_laytype = LAYTYPE_SILK_BOTTOM;
			break;
		case LAYTYPE_TOP:
			mir_laytype = LAYTYPE_BOTTOM;
			break;
		case LAYTYPE_COMPONENT_DFT_TOP:
			mir_laytype = LAYTYPE_COMPONENT_DFT_BOTTOM;
			break;
		case LAYTYPE_FLUX_TOP:
			mir_laytype = LAYTYPE_FLUX_BOTTOM;
			break;
		case LAYTYPE_PKG_PIN_LEG_TOP:
			mir_laytype = LAYTYPE_PKG_PIN_LEG_BOT;
			break;
		case LAYTYPE_PKG_PIN_FOOT_TOP:
			mir_laytype = LAYTYPE_PKG_PIN_FOOT_BOT;
			break;
		case LAYTYPE_STENCIL_TOP:
			mir_laytype = LAYTYPE_STENCIL_BOTTOM;
			break;
		case LAYTYPE_CENTROID_TOP:
			mir_laytype = LAYTYPE_CENTROID_BOTTOM;
			break;
		case LAYTYPE_PKG_BODY_TOP:
			mir_laytype = LAYTYPE_PKG_BODY_BOT;
			break;
		case LAYTYPE_DFT_TOP:
			mir_laytype = LAYTYPE_DFT_BOTTOM;
			break;
		default:
			mir_laytype = LAYTYPE_UNKNOWN;
			break;
		}

		if (mir_laytype != LAYTYPE_UNKNOWN)
		{
			// Scan for a layer of mir_laytype, set the mirror if found.
			// There can be more than one layer of given layer types, try to
			// get the best layer name match too.
			LayerStruct *bestmirlp = NULL;
			for (int i = 0; i< doc->getMaxLayerIndex(); i++)
			{
				LayerStruct *mirlp = doc->LayerArray[i];

				if (mirlp != NULL && mirlp->getLayerType() == mir_laytype && // right kind of layer
					mirlp->getLayerIndex() == mirlp->getMirroredLayerIndex()) // mirror not set
				{
					if (bestmirlp == NULL)
					{
						bestmirlp = mirlp;
					}
					else
					{
						// Check if mirlp has a better name match than bestmirlp.
						// Assume names are more likely to be simlar at start of
						// name and different at end of name.
						CString lpname = lp->getName();
						CString mirlpname = mirlp->getName();
						CString bestmirlpname = bestmirlp->getName();
						bool foundInMirlp = false;
						bool foundInBestmirlp = false;

						while (!foundInMirlp && !foundInBestmirlp && !lpname.IsEmpty())
						{
							foundInMirlp = (mirlpname.Find(lpname, 0) > -1);
							foundInBestmirlp = (bestmirlpname.Find(lpname, 0) > -1);
							if (foundInMirlp)
							{
								bestmirlp = mirlp;
							}
							else
							{
								lpname.Truncate(lpname.GetLength() - 1);
							}
						}
					}
				}
			}
			if (bestmirlp != NULL)
			{
				Graph_Level_Mirror(lp->getName(), bestmirlp->getName(), "");
			}
		}
#endif

		if (lp->getLayerIndex() == lp->getMirroredLayerIndex())
		{
			// Mirror still not set, try simple name matching.
			// Work from top, i.e. given a "top looking" name, find a likely matching bottom
			CString layerName = lp->getName();
			int indx;
			if ((indx = layerName.Find("-A")) > -1) // looks like top layer name
			{
				layerName.Replace("-A", "-B");
				bool mirSet = false;
				for (int i = 0; i < doc->getMaxLayerIndex() && !mirSet; i++)
				{
					LayerStruct *mirlp = doc->getLayerArray()[i];

					if (mirlp != NULL && mirlp->getName().Compare(layerName) == 0 &&
						mirlp->getLayerIndex() == mirlp->getMirroredLayerIndex())
					{
						Graph_Level_Mirror(lp->getName(), mirlp->getName(), "");
                  //// Case 2062 Rebug
                  //// Following line has basicaly good idea, to set the mirror layer
                  //// pair to have same type. But some types have a _TOP and _BOTTOM
                  //// incarnation, can not just set both to exactly same type.
                  //// mirlp->setLayerType(lp->getLayerType()); 
						mirSet = true;
					}
				}
			}

		}
	}
}

/****************************************************************************/
/*
*/
static void dumpLayerMap()
{
	fprintf(ferr, "=Layer Map=========================================================\n");
	fprintf(ferr, " MapA size (%d)\n", MapA->getSize());
	for (int k = 0; k < MapA->getSize(); k++)
	{
		CLayerMapEntry *me = MapA->getAt(k);
		if (me != NULL)
		{
			fprintf(ferr, "(%d)\n", k);
			fprintf(ferr, "    Foot Layer Name (%s)\n", me->m_footlayerName);
			for (int kk = 0; kk < me->m_boardlayerNames.GetCount(); kk++)
			{
				fprintf(ferr, "        Brd Lay Name (%s)\n", me->m_boardlayerNames.GetAt(kk));
			}
		}
	}
	fprintf(ferr, " - - - - - - - - - -\n");
	fprintf(ferr, " MapB size (%d)\n", MapB->getSize());
	for (int k = 0; k < MapB->getSize(); k++)
	{
		CLayerMapEntry *me = MapB->getAt(k);
		if (me != NULL)
		{
			fprintf(ferr, "(%d)\n", k);
			fprintf(ferr, "    Foot Layer Name (%s)\n", me->m_footlayerName);
			for (int kk = 0; kk < me->m_boardlayerNames.GetCount(); kk++)
			{
				fprintf(ferr, "        Brd Lay Name (%s)\n", me->m_boardlayerNames.GetAt(kk));
			}
		}
	}
	fprintf(ferr, " - - - - - - - - - -\n");
	fprintf(ferr, " MapThruA size (%d)\n", MapThruA->getSize());
	for (int k = 0; k < MapThruA->getSize(); k++)
	{
		CLayerMapEntry *me = MapThruA->getAt(k);
		if (me != NULL)
		{
			fprintf(ferr, "(%d)\n", k);
			fprintf(ferr, "    Foot Layer Name (%s)\n", me->m_footlayerName);
			for (int kk = 0; kk < me->m_boardlayerNames.GetCount(); kk++)
			{
				fprintf(ferr, "        Brd Lay Name (%s)\n", me->m_boardlayerNames.GetAt(kk));
			}
		}
	}
	fprintf(ferr, " - - - - - - - - - -\n");
	fprintf(ferr, " MapThruB size (%d)\n", MapThruB->getSize());
	for (int k = 0; k < MapThruB->getSize(); k++)
	{
		CLayerMapEntry *me = MapThruB->getAt(k);
		if (me != NULL)
		{
			fprintf(ferr, "(%d)\n", k);
			fprintf(ferr, "    Foot Layer Name (%s)\n", me->m_footlayerName);
			for (int kk = 0; kk < me->m_boardlayerNames.GetCount(); kk++)
			{
				fprintf(ferr, "        Brd Lay Name (%s)\n", me->m_boardlayerNames.GetAt(kk));
			}
		}
	}
	fprintf(ferr, " - - - - - - - - - -\n");

	fprintf(ferr, "==========================================================\n");
	fprintf(ferr,"PCF Footprint Layers  n = %d\n", cr5000FootprintLayerCount);
	for (int ii = 0; ii < cr5000FootprintLayerCount; ii++)
	{
		fprintf(ferr, "\n");
		CR5000LayerDef *cr5ld = cr5000FootprintLayerArray[ii];
		fprintf(ferr, "(%d) FOOTPRINT\n", ii);
		fprintf(ferr, "      foot name  (%s)\n", cr5ld->footprintLayerName);
		fprintf(ferr, "      foot type  (%s)\n", cr5ld->footprintLayerType);
		fprintf(ferr, "      board name (%s)\n", cr5ld->boardLayerName);
		fprintf(ferr, "      board type (%s)\n", cr5ld->boardLayerType);
		fprintf(ferr, "      conductive (%s)\n", cr5ld->conductive ? "True" : "False");
		fprintf(ferr, "      layer num  (%d)\n", cr5ld->conductiveLayerNumber);
		fprintf(ferr, "      comment (%s)\n", cr5ld->comment);
	}
	fprintf(ferr, "---\n");
	fprintf(ferr,"PCF Board Layers  n = %d\n", cr5000BoardLayerCount);
	for (int ii = 0; ii < cr5000BoardLayerCount; ii++)
	{
		fprintf(ferr, "\n");
		CR5000LayerDef *cr5ld = cr5000BoardLayerArray[ii];
		fprintf(ferr, "(%d) BOARD\n", ii);
		fprintf(ferr, "      foot name  (%s)\n", cr5ld->footprintLayerName);
		fprintf(ferr, "      foot type  (%s)\n", cr5ld->footprintLayerType);
		fprintf(ferr, "      board name (%s)\n", cr5ld->boardLayerName);
		fprintf(ferr, "      board type (%s)\n", cr5ld->boardLayerType);
		fprintf(ferr, "      conductive (%s)\n", cr5ld->conductive ? "True" : "False");
		fprintf(ferr, "      layer num  (%d)\n", cr5ld->conductiveLayerNumber);
		fprintf(ferr, "      comment (%s)\n", cr5ld->comment);
	}
	fprintf(ferr, "==========================================================\n");
	
}

/****************************************************************************/
/*
*/
static int update_mirrorlayers()
{
///#define DEBUG_LAYER_MAPPING
#ifdef DEBUG_LAYER_MAPPING
	dumpLayerMap();
#endif

   int         i, j;
   LayerStruct *lp;
	LayerStruct *lpTop = NULL;
	LayerStruct *lpBot = NULL;

   for (i=1;i<=G.numberoflayers;i++)
   {  
      CString  lname;
      lname.Format("%d", i);

      int lptr = Graph_Level(lname,"", 0);
      lp = doc->FindLayer(lptr);

      if (i == 1)
      {
         lp->setLayerType(LAYTYPE_SIGNAL_TOP);
         lp->setElectricalStackNumber(1);
			lpTop = lp;
      }
      else
      if (i == G.numberoflayers)
      {
         lp->setLayerType(LAYTYPE_SIGNAL_BOT);
         lp->setElectricalStackNumber(i);
			lpBot = lp;
      }
      else
      {
         lp->setLayerType(LAYTYPE_SIGNAL_INNER);
         lp->setElectricalStackNumber(i);

      }
   }

	if (lpTop != NULL && lpBot != NULL)
		Graph_Level_Mirror(lpTop->getName(), lpBot->getName(), "");


   for (i=0;i<layer_mirrorcnt;i++)
   {
      // check if either the layer or its mirror layer is found
      if (doc->FindLayer_by_Name(layer_mirror[i].lay1) == NULL &&
          doc->FindLayer_by_Name(layer_mirror[i].lay2) == NULL)         continue;

      Graph_Level(layer_mirror[i].lay1,"", 0);
      Graph_Level(layer_mirror[i].lay2,"", 0);
      Graph_Level_Mirror(layer_mirror[i].lay1, layer_mirror[i].lay2, "");
   }


	// Set layer types, some special based on layer names
	// Then mapping from CR5000 layer types to Layer types
   for (j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      lp = doc->getLayerArray()[j];
      if (lp == NULL)   continue; // could have been deleted.

      // done by CAMCAD.
      if (lp->getName().CompareNoCase("DRILLHOLE") == 0)
      {
         lp->setLayerType(LAYTYPE_DRILL);
      }
      else
      if (lp->getName().CompareNoCase("Board") == 0)
      {
         lp->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }

		// There are two sets of layer names in play in the cad data, there are
		// footprint layers and board layers. Most geometry in the import is based
		// on footprint definitions, and use footprint layer names. Some comes
		// from the board def, though, and uses board layer names. Now we have
		// to find camcad's layer name in the dual cad list of layers.
		CString layername = lp->getName();
		CR5000LayerDef *cr5ld = getCr5000BestLayer(layername);

		if (cr5ld != NULL)
		{
			if (lp->getLayerType() == layerTypeUnknown)
				lp->setLayerType(camcadLayerType(cr5ld));
			lp->setComment(cr5ld->comment);
		}


		// Apply .out file settings
      for (i=0;i<layer_attr_cnt;i++)
      {
         if (lp->getName().CompareNoCase(layer_attr[i].name) == 0)
         {
            lp->setLayerType(layer_attr[i].attr);
            break;
         }
      }

#ifdef CUTE_IDEA__HAS_DEFECTS
      //// Case 2062 Rebug
      //// The code below was added on a whim of my own, while working case 2062.
      //// Turns out it wasn't quite right, and rebug request is to remove it.
      //// The idea is basically goo, I think, to align types on mirror layers.
      //// But some layer types can't be shared directly like this, some are _TOP and _BOTTOM.
      //// Also probably should not change a layer unless it is currently type UNKNOWN

      // If this layer has a mirror layer set, make sure both this and the mirror
      // are set to same type. But skip it if "this" layer is UNKNOWN_GRAPHIC. We
      // might get a better type from the other layer when it is it's turn through
      // here.
      int mirIndx = lp->getMirroredLayerIndex();
      if (mirIndx != lp->getLayerIndex())
      {
         LayerStruct *mirLp = doc->getLayerAt(mirIndx);
         if (lp->getLayerType() != layerTypeUnknown && mirLp != NULL)
            mirLp->setLayerType(lp->getLayerType());
      }
#endif


   }

   validateSurfaceLayers();

	// Last chance mirror layer mapping, layer types may have been changed above
	// so take a pass and set mirror layers for recognized layer types.
	for (i = 0; i < doc->getMaxLayerIndex(); i++)
   {
      lp = doc->getLayerArray()[i];
		if (lp != NULL && (lp->getLayerIndex() == lp->getMirroredLayerIndex())) // if mirror to self, aka not set
		{
			set_mirror_layer(lp);
		}
	}

#ifdef DEBUG_LAYER_MAPPING
	dumpLayerMap();
#endif

   return 1;
}


/****************************************************************************/
/*
*/

static void register_cad_layers()
{
	// Register all board layers found in cad file.
	
	for (int i = 0; i < cr5000BoardLayerCount; i++)
	{
		CR5000LayerDef *ld = cr5000BoardLayerArray[i];
		Graph_Level(ld->boardLayerName, "", 0);
	}
}

/****************************************************************************/
/*
*/

static void inherit_footprint_layer_types()
{
	// If the board layer type is not set then look in the maps
	// for a usage of that footprint layer. The map may contain more than
	// one board layer mapped to a footprint, in which case the layer type
	// is ambiguous. E.g. there may be both conductive and nonconductive board
	// board layers mapped to a single footprint.
	// But if the mapping contains only one, we can suppose the that board layer
	// is of the same type as the footprint layer.
	
	for (int i = 0; i < cr5000FootprintLayerCount; i++)
	{
		CR5000LayerDef *ftfld = cr5000FootprintLayerArray[i];
		CString footlayername = ftfld->footprintLayerName;

		CLayerMapEntry *me = NULL;
		if (MapA->lookup(footlayername, me))
		{
			if (me->m_boardlayerNames.GetCount() == 1)
			{
				CR5000LayerDef *brdld = getCr5000BoardLayer(me->m_boardlayerNames.GetAt(0));
				if (brdld != NULL && brdld->boardLayerType.IsEmpty())
					brdld->boardLayerType = ftfld->footprintLayerType;
			}
		}
		else if (MapB->lookup(footlayername, me))
		{
			if (me->m_boardlayerNames.GetCount() == 1)
			{
				CR5000LayerDef *brdld = getCr5000BoardLayer(me->m_boardlayerNames.GetAt(0));
				if (brdld != NULL && brdld->boardLayerType.IsEmpty())
					brdld->boardLayerType = ftfld->footprintLayerType;
			}
		}
		else if (MapThruA->lookup(footlayername, me))
		{
			if (me->m_boardlayerNames.GetCount() == 1)
			{
				CR5000LayerDef *brdld = getCr5000BoardLayer(me->m_boardlayerNames.GetAt(0));
				if (brdld != NULL && brdld->boardLayerType.IsEmpty())
					brdld->boardLayerType = ftfld->footprintLayerType;
			}
		}
		else if (MapThruB->lookup(footlayername, me))
		{
			if (me->m_boardlayerNames.GetCount() == 1)
			{
				CR5000LayerDef *brdld = getCr5000BoardLayer(me->m_boardlayerNames.GetAt(0));
				if (brdld != NULL && brdld->boardLayerType.IsEmpty())
					brdld->boardLayerType = ftfld->footprintLayerType;
			}
		}
	}
}

/****************************************************************************/
/*
*/
static void purge_unused_layers()
{
	// We could do this, but then we get a pop-up to confirm, we want
	// to just do it.
	//doc->OnPurgeUnusedLayers();

	// So instead ... just do it

	doc->MarkUnusedLayers();

	for (int i = 0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *lp = doc->getLayerArray()[i];
		if (lp != NULL && !lp->isUsed())
		{
			doc->RemoveLayer(lp);
		}
	}
}

/****************************************************************************
*/
bool CCR5000AttrSettings::CheckName(CString *SS,CString *BN)
{
   int loc, loc2, pos;

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
		   return true;
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
				   return true;
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
		  return true; // in recursive call last '*' match with remaining BlockName
     }
   }

   return false;
}

/****************************************************************************
*/
bool CCR5000AttrSettings::IsAttrib(CString s, EAttrSettingTag atttype)
{
#ifdef ORIGINAL_CADIF
   for (int i=0; i<Attrcnt; i++)
   {
      CString SearchString, BlockName;
	   SearchString = Attr[i].name;
	   BlockName = s;

	   if (Attr[i].atttype == atttype && CheckName(&SearchString, &BlockName))
		   return 1;
   }
   return 0;
#endif

	for (int i = 0; i < this->GetCount(); i++)
	{
		CCR5000AttrSettingEntry *entry = this->GetAt(i);
		if (entry->GetType() == atttype)
		{
			CString SearchString = entry->GetName();
			CString BlockName = s;

			if (CheckName(&SearchString, &BlockName))
				return true;
		}
	}

	return false;
}

/****************************************************************************
*/
void CCR5000AttrSettings::UpdateAttr(CString str, EAttrSettingTag typ)
{
#ifdef ORIGINAL_CADIF
	if (attrcnt < MAX_ATTR)
   {
      if ((attr[attrcnt].name = STRDUP(lp)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      attr[attrcnt].atttype = typ;
      attrcnt++;
   }
   else
   {
      // error
   }

   return attrcnt-1;
#endif

	CCR5000AttrSettingEntry *entry = new CCR5000AttrSettingEntry(str, typ);
	this->Add(entry);

}

/****************************************************************************
*/
BlockTypeTag CCR5000AttrSettings::GetBlockType(CString name, BlockTypeTag defaultBlockType)
{
	BlockTypeTag tag = defaultBlockType;

	if (this->IsAttrib(name, attrSettingFiducial))
		tag = blockTypeFiducial;
	else if (this->IsAttrib(name, attrSettingTestPoint))
		tag = blockTypeTestPoint;
	else if (this->IsAttrib(name, attrSettingTooling))
		tag = blockTypeTooling;

	return tag;
}

/****************************************************************************
*/
InsertTypeTag CCR5000AttrSettings::GetInsertType(CString name, InsertTypeTag defaultInsertType)
{
	InsertTypeTag tag = defaultInsertType;

	if (this->IsAttrib(name, attrSettingFiducial))
		tag = insertTypeFiducial;
	else if (this->IsAttrib(name, attrSettingTestPoint))
		tag = insertTypeTestPoint;
	else if (this->IsAttrib(name, attrSettingTooling))
		tag = insertTypeDrillTool;

	return tag;
}

/****************************************************************************
*/
InsertTypeTag CCR5000AttrSettings::GetInsertType(BlockStruct *b, InsertTypeTag defaultInsertType)
{
	InsertTypeTag tag = defaultInsertType;

	if (b != NULL)
	{
		if (b->getBlockType() == blockTypeFiducial)
			tag = insertTypeFiducial;
		else if (b->getBlockType() == blockTypeTestPoint)
			tag = insertTypeTestPoint;
		else if (b->getBlockType() == blockTypeTooling)
			tag = insertTypeDrillTool;
	}

	return tag;
}

/****************************************************************************
*/
void CCR5000AttrSettings::Dump(FILE *fp)
{
	fprintf(fp, "--- Attr Settings ---\n");
	
	for (int i = 0; i < this->GetCount(); i++)
	{
		CCR5000AttrSettingEntry *entry = this->GetAt(i);
		fprintf(fp, "(%s) (%d)\n", entry->GetName(), entry->GetType());
	}

	fprintf(fp, "---------------------\n");
}

//_____________________________________________________________________________
int CCr5000LineNumberStamp::m_sourceLineNumberKeywordIndex = -1;

CCr5000LineNumberStamp::CCr5000LineNumberStamp()
{
   m_lineNumber = ifp_line;
}

void CCr5000LineNumberStamp::stamp(DataStruct* data)
{
   if (data != NULL)
   {
      if (m_sourceLineNumberKeywordIndex < 0)
      {
         m_sourceLineNumberKeywordIndex = doc->RegisterKeyWord("SourceLineNumber",0,valueTypeInteger);
      }

      doc->SetAttrib(&(data->getAttributesRef()),m_sourceLineNumberKeywordIndex,m_lineNumber);
   }
}

DataStruct* CCr5000LineNumberStamp::graphPolyStruct(int layer, DbFlag flg, BOOL negative)
{
   DataStruct* data = Graph_PolyStruct(layer,flg,negative);

   stamp(data);

   return data;
}

DataStruct* CCr5000LineNumberStamp::graphBlockReference(const char *block_name, const char *refname, int filenum, double x, double y,
      double angle, int mirror, double scale, int layer, int global, BlockTypeTag blockType)
{
   DataStruct* data = Graph_Block_Reference(block_name,refname,filenum,x,y,angle,mirror,scale,layer,global,blockType);

   stamp(data);

   return data;
}

void CCr5000LineNumberStamp::resetKeywordIndices()
{
   m_sourceLineNumberKeywordIndex = -1;
}

#ifdef EnableVertexTracking
//_____________________________________________________________________________
int CVertexEntry::m_ptDepth  = 0;
int CVertexEntry::m_arcDepth = 0;
int CVertexEntry::m_ptCnt    = 0;
int CVertexEntry::m_arcCnt   = 0;
int CVertexEntry::m_expectedArcPts = 0;
bool CVertexEntry::m_mixedFlag = false;

CVertexEntry::CVertexEntry(VertexEntryTag type)
: m_type(type)
{
   if (ifp_line >= 170375)
   {
      int iii = 3;
   }

   switch (m_type)
   {
   case vertexEntryPt:
      m_ptDepth++;

      if (m_expectedArcPts > 0)
      {
         m_expectedArcPts--;
      }
      else
      {
         m_ptCnt++;
      }
      break;
   case vertexEntryArc:
      m_arcDepth++;
      m_arcCnt++;
      m_expectedArcPts = 2;
      break;
   }

   if (m_ptCnt > 0 && m_arcCnt > 0)
   {
      if (!m_mixedFlag)
      {
         int iii = 3;
      }

      m_mixedFlag = true;
   }
}

CVertexEntry::~CVertexEntry()
{
   switch (m_type)
   {
   case vertexEntryPt:
      m_ptDepth--;
      break;
   case vertexEntryArc:
      m_arcDepth--;
      m_expectedArcPts = 0;
      break;
   }
}

void CVertexEntry::reset()
{
   if (m_ptDepth != 0 || m_arcDepth != 0)
   {
      int iii = 3;
   }

   m_ptDepth  = 0;
   m_arcDepth = 0;
   m_ptCnt    = 0;
   m_arcCnt   = 0;
   m_expectedArcPts = 0;
   m_mixedFlag = false;
}
#endif



