// $Header: /CAMCAD/5.0/read_wrt/AccelIn.cpp 72    3/12/07 12:48p Kurt Van Ness $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
          
   Vias always get the V: prefix

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
#include "accelin.h"
#include "StandardAperture.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
//extern LayerTypeInfoStruct layertypes[];

/* Function Define *********************************************************/
static bool fgetcs(CString &string, FILE *ifp);  // A replacement for fgets only uses CString, no fixed buffer nuisances.
static double cnv_unit(const char *);
static int go_command(List *, int);
static void free_all_mem();
static void init_all_mem();
static void zero_attcnt();
static void update_pin_track();
static void update_track_track();
static int get_tok();
static int p_error();
static int tok_search(List *, int);
static int push_tok();
static int loop_command(List *, int);
static int tok_layer();
static void elim_ext(char *);
static int get_padnumber(char *);
static int get_next(CString &linebuf, int);
static bool get_line(CString &linebuf);
static void sort_libpin();
static int copy_net();
static int write_text(char *prosa);
static int write_arc(int widthindex, int gr_class, const char *nname);
static int write_arc(int widthindex, int gr_class, const char *nname, double cntrx, double cntry, double radius, double startangle, double endangle);
static int write_poly(int fill, int layernr, int widthindex, int gr_class, const char *nname, int thermal, int boundary);
static int write_libpin(int use_pad_mirror);
static int load_libpin(int pinnum, const char *pindes, const char *padstack, double x, double y, double rot, int mirrored, int typ);
static int get_pinnum(int compdefptr,char *padname);
static bool test_asciiheader(CString buf);
static double tan_unit();
static int update_net(CString n);
static int get_compinst(const char *p);
static int get_compdef(const char *p);
static int do_netplane();
static int load_accelsettings(const CString fname);
static double calc_rotation(double rot);
static int update_libraryclasses();
static int do_layers1();
static int do_layers2();
static int update_accelGeomType(const char *, int);
static int getGeomTypeFromMap(const char *, int *);
static void update_geom_and_insert_types();
static ACCELPatterndef* getNewPatternDef(CString patternName, BlockStruct *geomBlock = NULL);
static ACCELPatterndef* setLastPatternGraphicAndGeomName(CString graphicName);
static ACCELPatterndef* getLastPattern();
static ACCELPatterndef* findPatternDef(CString patterName, CString graphicName);
static ACCELPatterndef* findNextPatternDef(CString patternName, int &index);
static ACCELCompdef* getNewCompDef(CString compDefName);
static ACCELCompdef* getLastCompDef();
static ACCELCompdef* findCompDef(CString compDefName);
static CString findCompPinRefByPinNumber(ACCELCompdef *compDef, int pinNumber);
static ACCELCompInst* getNewCompInst(CString compInstName);
static ACCELCompInst* getLastCompInst();
static ACCELCompInst* findCompInst(CString compInstName);
int createRoundedRectPad( int layernr, const char *nname, int thermal);


/* Globals ****************************************************************/
typedef  struct
{
   int      electrical_layers;      /* number of electrical layers from lyrphid */
   int      cur_status;

   /* Current values that are changed with global  */
   int      cur_layernum;                 /* Layer.      */
   double   cur_width;                    /* Line  */
   double   cur_height;                   /* Line height     */

   /* Current Component info used for Instance.    */
   double   cur_pos_x;                    /* position.   */
   double   cur_pos_y;
   double   cur_rotation;                 /* Rotation.   */
   int      cur_mirror;                   /* Current mirror.   */
   int      cur_fixed;
   /* Current pad stack info.    */
   int      cur_pad;                      /* Current pad.         */
   int      cur_pstack;                   /* Current padstack number. */
   int      cur_pincnt;
   /* Pad stacks */
   int      cur_textmirror;
   int      cur_textjust;
   int      cur_textrot;
   //
   int      cur_type;                     // used by layer and padshape 
   int      cur_smdpad;                   // 1 top, 2 bot, 3 SIGNAL, 4, drill
   double   cur_extent_x, cur_extent_y;   // text extents
   AccelLayerTypeTag cur_layerType;
} Global;

static   Global   G;                            /* global const. */
//static   Adef     layer_attr[MAX_LAYERS];       /* Array of default layers.   */
//static   int      layer_attr_cnt;
//static   CAccelDefaultLayerArray accelDefaultLayerArray;

static   FILE     *ferr;
static   FILE     *ifp;                            /* File pointers.    */
static   long     ifp_line = 0;                    /* Current line number. */

static   FileStruct *file = NULL;
static   CCEtoODBDoc *doc;
static   int      display_error = 0;

static   CString  compoutline[MAX_COMPOUTLINE];
static   int      compoutlinecnt;

static   CString  boardoutline;
static   CString  compheight;

static   double   cur_drill;
static   int      cur_isplated;
static   int      cur_fillmode = TRUE;

static   int      shapeoutlinecnt;

static   int      graphic_class;

static   CDrillArray drillarray;
static   int      drillcnt;

static   CPadformArray  padformarray;
static   int      padformcnt;

static   CString  token;                           /* Current token.       */
static   CString  cur_originalname;
static	CString	cur_refdesRef;
static   CString  cur_padname;                     /* current used padshape name */
static   CString  cur_libname;                     /* current used library name */
static   int      cur_pinnum;                      /* current used pinnumber */
static   CString  cur_pindes;
static   CString  cur_layerDefName;                /* current defined layer name */
static   int      titleSheetCnt;                   /* holds the count of titlesheet within a layer def */

static   CString     cur_netname;                  /* current used netname name */
static   double   cur_padrotation;
static   TypeStruct  *curtype;

/* different pins with same name can not exist */
static   int      tangounit = UNIT_MILS;

static   int      Push_tok = FALSE;
static   CString  cur_line;
static   int      cur_new = TRUE;

static   ACCELShapeName   *shapename;
static   int      shapenamecnt = 0;
static   int      trans_routes = TRUE;

static   CPatternDefArray patterndefarray;
static	CCompDefArray compdefArray;
static   CCompInstArray compinstarray;

static   ACCELLibpin   *libpin;
static   int      libpincnt = 0;

//static   CAccelLayerArray accelLayerArray;
//static   ACCELLayerlist *layerlist;
//static   int      layerlistcnt = 0;
static   CAccelLayers accelLayers;

static   CTextStyleArray textstylearray;
static   int      textstylecnt = 0;


typedef CArray<Point2, Point2&> CPolyArray;

static   CPolyArray polyarray;
static   int      polycnt =0 ;

static   CAttrmapArray attrmaparray;
static   int      attrmapcnt;

static   CAttArray attarray;
static   int      attcnt;

static   ACCELNetl *netlist;
static   int      netlistcnt = 0;

static   int      arctoline = FALSE;
static   int      boardoutlinelayernum = 0;

static   int      ComponentSMDrule;    // 0 = take existing ATT_SMD
                                       // 1 = if all pins of comp are marked as SMD
                                       // 2 = if most pins of comp are marked as SMD
                                       // 3 = if one pin of comp are marked as SMD

static AccelGeomTypeSwap   accelGeomTypeArray[MAX_ATTR];
static int						accelGeomTypeArrayCount;

// this are used for optimizing polyline vertexes.
static   char     oldnname[80];
static   int      oldlayernr;
static   double   oldpolyx;
static   double   oldpolyy;
static   int      oldwidthindex;
static   int      startnew;   // forces a new one.

static   int      pageunits;

static   double   TEXT_CORRECT  = 0.7;       // accel height to real height
static   double   TEXT_RATIO    = 0.66;      // width to height ratio

static BOOL			PatternBegin = FALSE;
static BOOL			GetAttr = TRUE;
static CString		PatternGraphicNameRef = "";
static int			convert_freepad;
static int			convert_freepad_count;
static int			current_file_pass;
static bool			do_normalize_bottom_build_geometries;
static bool       read_pour_boundary;
static bool       clear_current_width = true;
const double      swellCompareTolerance( 0.0000000001 );


/******************************************************************************
* ReadACCEL PCAD
*/
void ReadACCEL(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits)
{
   CFilePath ccFilePath(path_buffer);
   CString f = ccFilePath.getFileName();
   doc = Doc;
   pageunits = pageUnits;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(path_buffer, "r")) == NULL)
   {
      CString t;
      t.Format("Error open [%s] file", path_buffer);
      ErrorMessage(t, "Error");
      return;
   }

   CString accelLogFile = GetLogfilePath(ACCELERR);
   ferr = fopen(accelLogFile, "wt");
   if (ferr == NULL)
   {
      CString message;
      message.Format("Error opening [%s] file", accelLogFile);
      ErrorMessage(message, "Error");
      return;
   }

   //CDebugWriteFormat::setFilePath("c:\\AccelIn_debug.txt");
  
   PatternBegin = FALSE;
   GetAttr = TRUE;

   attrmaparray.SetSize(100, 100);
   attrmapcnt = 0;

   attarray.SetSize(100, 100);
   attcnt = 0;

   drillarray.SetSize(100, 100);
   drillcnt = 0;

   padformarray.SetSize(100, 100);
   padformcnt = 0;

   read_pour_boundary = false;

   file = Graph_File_Start(f, Type_ACCEL_Layout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
   
   Graph_Level("0", "", 1);
   ComponentSMDrule = 0;

   CString settingsFilename( getApp().getImportSettingsFilePath("accel.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nAccel Reader: Settings file [%s].\n", settingsFilename);
   getApp().LogMessage(settingsFileMsg);
   load_accelsettings(settingsFilename);

   init_all_mem();

   for (current_file_pass = 1;current_file_pass <= 2;current_file_pass++)
   {    
      display_error = 0;
      graphic_class = 0;
      curtype = NULL;
     
      // Reset file and parser state
      rewind(ifp);
      ifp_line = 0; 
      cur_line.Empty();
      cur_new = TRUE;   
      token.Empty();
      Push_tok = FALSE;


      // could have empty lines !
      CString fileIdLine;
      while (TRUE)
      {
         if (!fgetcs(fileIdLine, ifp))
         {
            CString message;
            message.Format("Error reading 1. line in [%s] file", path_buffer);
            ErrorMessage(message, "Read Error");
            return;
         }
         ifp_line++;

         if (!fileIdLine.Trim().IsEmpty())
         {
            break;
         }
      }

      fprintf(ferr, "**\n** PASS %d\n**\n", current_file_pass);

      if (current_file_pass == 1)
      {
         if (test_asciiheader(fileIdLine))
         {
            loop_command(str_pass1_lst, SIZ_STR_Pass1_LST);
         }

         accelLayers.instantiateCamCadLayers(*doc);
      }
      else
      {
         if (test_asciiheader(fileIdLine))
         {
            loop_command(str_lst, SIZ_STR_LST);

            do_layers2();

            accelLayers.instantiateCamCadLayers(*doc);

            // here need to update primary component outline layer
            update_libraryclasses();
            RefreshInheritedAttributes(doc, SA_RETURN);  // smd pins are after the pin instance is done.

            update_smdrule_geometries(doc, ComponentSMDrule);
            update_smdrule_components(doc, file, ComponentSMDrule);

            update_geom_and_insert_types();

            do_netplane();

            generate_PADSTACKACCESSFLAG(doc, 1);
            generate_PINLOC(doc, file, 1);   // this function generates the PINLOC argument for all pins.

         }  // valid ASCII file
      }
   }

	if (do_normalize_bottom_build_geometries)
		doc->OnToolsNormalizeBottomBuildGeometries();

   padformarray.RemoveAll();  // 60 mil round
   drillarray.RemoveAll();

   for (int i=0;i<attrmapcnt;i++)
   {
      delete attrmaparray[i];  
   }

   attrmaparray.RemoveAll();
   attrmapcnt = 0;
   zero_attcnt();
   attarray.RemoveAll();   // this is done after the attributes are used.
   free_all_mem();
   fclose(ferr);
   fclose(ifp);

   if (display_error)
      Logreader(accelLogFile);

   CAccelPadDefState::releaseAccelPadDefState();

   return;
}

/******************************************************************************
* getBlockTypeFromAttrType
*/
static int getBlockTypeFromAttrType(int attrType)
{
   switch(attrType)
   {
      case FIDUCIAL_GEOM:
            return BLOCKTYPE_FIDUCIAL;
      case TOOLING_GEOM:
            return BLOCKTYPE_TOOLING;
   }
   return BLOCKTYPE_UNKNOWN;
}

/******************************************************************************
* getInsertTypeFromAttrType
*/
static int getInsertTypeFromAttrType(int attrType)
{
   switch(attrType)
   {
      case FIDUCIAL_GEOM:
            return INSERTTYPE_FIDUCIAL;
      case TOOLING_GEOM:
            return INSERTTYPE_TOOLING;
   }
   return INSERTTYPE_UNKNOWN;
}

/******************************************************************************
* update_geom_and_insert_types
*/
static void update_geom_and_insert_types()
{
   // change block type based on attr list.
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      int blockType = 0;
      //if (block == NULL)
      //   continue;

      if ( block != NULL )
      {
         if (getGeomTypeFromMap(block->getName(), &blockType))
         {
            block->setBlockType(getBlockTypeFromAttrType(blockType));
         }
         // here loop through inserts and change insert types as needed
         DataStruct *data;
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            data = block->getDataList().GetNext(dataPos);
         
            if (data->getDataType() == T_INSERT)         
            {
               BlockStruct *thisBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
               int insertType = 0;
  
               if ( thisBlock != NULL )
               {
                  if (getGeomTypeFromMap(thisBlock->getName(), &insertType))
                  {
                     thisBlock->setBlockType(getBlockTypeFromAttrType(insertType));
                     data->getInsert()->setInsertType(getInsertTypeFromAttrType(insertType));
                  }
               }
            }
         }
      }
   }
}

/******************************************************************************
* get_attrmap
*/
static CString get_attrmap(const char *a)
{
   for (int i=0; i<attrmapcnt; i++)
   {
      if (attrmaparray[i]->accel_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name;
   }
   
   CString tmp = a;
   tmp.MakeUpper();

   return tmp;
}

/******************************************************************************
* update_accelGeomType
*/
static int update_accelGeomType(const char *name, int type)
{
   if (accelGeomTypeArrayCount < MAX_ATTR)
   {
      if ((accelGeomTypeArray[accelGeomTypeArrayCount].name = STRDUP(name)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      accelGeomTypeArray[accelGeomTypeArrayCount].geomType = type;
      accelGeomTypeArrayCount++;
   }

   return accelGeomTypeArrayCount-1;
}

/******************************************************************************
* getGeomTypeFromMap
*/
static int getGeomTypeFromMap(const char *name, int *geomType)
{
   for (int i=0; i<accelGeomTypeArrayCount; i++)
   {
      if (STRCMPI(accelGeomTypeArray[i].name, name) == 0)
      {
         *geomType = accelGeomTypeArray[i].geomType;
         return TRUE;
      }
   }

   return FALSE;
}

/******************************************************************************
* go_command
*   Call function associated with next tolen.
*   Tokens enclosed by () are searched for on the local
*   token list.  
*/
int go_command(List *tok_lst,int lst_size)
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

/******************************************************************************
* loop_command
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

      if (tok_search(brk_lst, SIZ_BRK_LST) == BRK_B_ROUND)
         repeat = FALSE;
      push_tok();
   }
   return 1;
}

/******************************************************************************
* fnull
*   Skips over any tokens to next }] endpoint.
*/
static int fnull()
{
   int brk_count = 0;

#ifdef _DEBUG   
   fprintf(ferr, "DEBUG: Token [%s] at %ld unknown\n", token, ifp_line);
   display_error++;
#endif

   //display_error++;
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
#ifdef _DEBUG
            fprintf(ferr, " -> Token [%s] at %ld unknown\n", token, ifp_line);
#endif
            break;
         }
      }
      else
         return p_error();
   }
}

/******************************************************************************
* fskip
*   Skips over any tokens to next () endpoint.
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
         return p_error();
   }
}

/******************************************************************************
* get_padformindex
*/
static int get_padformindex(int form, double sizeA, double sizeB, double rotation, double offset)
{
   double offsetx = 0;
   double offsety = 0;


	// Make sure to change the form FIRST before comparision to ensure the forms match
	if (form == T_OBLONG && (fabs(sizeA - sizeB) < SMALLNUMBER))
   {
      form = T_ROUND;
      sizeB = 0;
   }
   else if (form == T_RECTANGLE && sizeA == sizeB)
   {
      form = T_SQUARE;
      sizeB = 0;
   }

   for (int i=0; i<padformcnt; i++)
   {
      AccelPadform padform = padformarray.GetAt(i);
      if (padform.form == form &&
          fabs(padform.sizeA - sizeA) < SMALLNUMBER &&
          fabs(padform.sizeB - sizeB) < SMALLNUMBER &&
          fabs(padform.rotation - rotation) < SMALLANGLE &&
          fabs(padform.offsetx - offsetx) < SMALLNUMBER &&
          fabs(padform.offsety - offsety) < SMALLNUMBER)
         return i;         
   }

   int AlreadyExists = TRUE;
   int padshapeNum=0;
   int widthIndex;
   CString name = "";
   while (AlreadyExists)
   {
		padshapeNum++;
		if (padshapeNum == 1)
		{
			if (form == apertureRound || form == apertureSquare)
				name.Format("%s_%.3f", apertureShapeToName(form), sizeA);
			else
				name.Format("%s_%.3f_%.3f", apertureShapeToName(form), sizeA, sizeB);
		}
		else
		{
			CString prevName = name;

			// The name is the still the after change to 6 decimal place so attach a number
			if (form == apertureRound || form == apertureSquare)
				name.Format("%s_%.3f_%d", apertureShapeToString(form), sizeA, padshapeNum);
			else
				name.Format("%s_%.3f_%.3f_%d", apertureShapeToString(form), sizeA, sizeB, padshapeNum);

			CString errMsg = "";
			errMsg.Format("Note:  Aperture with same name [%s] but with slight difference in width, height, rotation, Xoffset, or Yoffset.  Renamed to [%s].\n", prevName, name);
			fprintf(ferr, errMsg);
		}

      if( form == apertureThermal ){   //for thermal pads we need to swap inside and outside diameter
         widthIndex = Graph_Aperture(name, form, sizeB, sizeA, 0.0, 0.0, DegToRad(rotation), 0, BL_APERTURE, FALSE, &AlreadyExists);
      } else {
         widthIndex = Graph_Aperture(name, form, sizeA, sizeB, 0.0, 0.0, DegToRad(rotation), 0, BL_APERTURE, FALSE, &AlreadyExists);
      }
   }

   BlockStruct *block = doc->getWidthTable()[widthIndex];
   int blockNum = block->getBlockNumber();

   AccelPadform padform;
   padform.form = form;
   padform.sizeA = sizeA;
   padform.sizeB = sizeB;
   padform.rotation = rotation;
   padform.offsetx = 0;
   padform.offsety = 0;
   padform.blockNum = blockNum;
   padformarray.SetAtGrow(padformcnt++, padform);  

   return padformcnt -1;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size, int layernum, int plated)
{
   AccelDrill p;

   if (size == 0)
      return -1;

   for (int i=0; i<drillcnt; i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size && p.plated == plated)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d", drillcnt);
   Graph_Tool(name, 0, size, 0, 0, 0, 0L, plated);
   p.d = size;
   p.plated = plated;
   drillarray.SetAtGrow(drillcnt, p);  
   drillcnt++;

   return drillcnt -1;
}

/******************************************************************************
* tan_asciiheader
*/
static int tan_asciiheader()
{
   return loop_command(asc_lst, SIZ_ASC_LST);
}

/******************************************************************************
* tan_library
*/
static int tan_library()
{
   if (!get_tok())
      return p_error();
   int res = loop_command(lib_lst, SIZ_LIB_LST);
   

// Note(Lynn): This will probably be removed since the command ".USECOMPDEF" will be remove.
//					However, we might still need to clean up "$" name

   //if (USE_COMPDEF)
   //{
   //   // delete all patterndef, they got copied into the COMPDEF defintions.
   //   for (int i=0; i<patterndefcnt; i++)
   //   {
   //      BlockStruct *block = Graph_Block_Exists(doc, patterndefarray[i]->name, file->getFileNumber());
   //      if (block)
   //         doc->RemoveBlock(block);   // delete the current one.
   //   }
   //   // also not change name from $ %s to %s
   //   for (i=0; i<doc->getMaxBlockIndex(); i++)
   //   {
   //      BlockStruct *block = doc->getBlockAt(i);
   //      if (block == NULL)
   //         continue;
   //      if (block->getName().Left(2) == "$ ")
   //         block->getNameRef().Delete(0, 2);
   //   }
   //}


   return res;
}

/******************************************************************************
* tan_text
*  the text definition does not follow the correct LISP syntax.
*/
static int tan_text()
{
   int i;
   int brk;
   int repeat = TRUE;
   CString prosa;

   G.cur_mirror = FALSE;
   G.cur_rotation  = 0;
   G.cur_textjust = 0;
   G.cur_extent_x = -1;
   G.cur_extent_y = -1;

   polycnt = 0;
   while (repeat)
   {
      int text_found = FALSE;
      if (!get_tok())
         return p_error();

      switch((brk = tok_search(brk_lst, SIZ_BRK_LST)))
      {
      case BRK_ROUND:
         if (get_tok() && (i = tok_search(graph_lst, SIZ_GRAPH_LST)) >= 0)
            (*graph_lst[i].function)();
         else
            return p_error();
         break;
      case BRK_B_ROUND:
         repeat = FALSE;
         push_tok();
         break;
      default:
         text_found = TRUE;
         prosa = token;
         break;
      }

      if (!repeat)
         break;

      if ( text_found == FALSE )
      {
         if (!get_tok())
            return p_error();
         if (brk != tok_search(brk_lst, SIZ_BRK_LST) - 1)
            return p_error();
      }
   }
   write_text(prosa.GetBuffer(0));
   
   return 1;
}

/******************************************************************************
* tan_arc
*/
static int tan_arc()
{
   polycnt = 0;
   cur_netname .Empty();
   loop_command(graph_lst, SIZ_GRAPH_LST);

   int widthindex;
   int err;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   write_arc(widthindex, graphic_class, cur_netname);

   return 1;
}

/******************************************************************************
* tan_triple_pt_arc
*/
static int tan_triple_pt_arc()
{
	polycnt = 0;
   cur_netname .Empty();
   loop_command(graph_lst, SIZ_GRAPH_LST);

	if (polycnt == 3)
	{
		Point2   p0,p1,p2;
      p0 = polyarray.ElementAt(0);
      p1 = polyarray.ElementAt(1);
      p2 = polyarray.ElementAt(2);

		double cx, cy, r, sa, da;
		cx = cy = r = sa = da = 0.0;
		
		// I thought it was going to be typical 3 point arc. It is not.
		///ArcPoint3(p0.x,p0.y, p1.x,p1.y, p2.x,p2.y, &cx, &cy, &r, &sa, &da);

		// It is actually center and two points.
		cx = p0.x;
		cy = p0.y;
		ArcCenter2(p1.x,p1.y, p2.x,p2.y, cx, cy, &r, &sa, &da, false);

		if (r != 0.0)
		{
			int widthindex;
			int err;
			if (G.cur_width == 0)
            widthindex = doc->getZeroWidthIndex();
			else
				widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

			write_arc(widthindex, graphic_class, cur_netname, cx, cy, r, RadToDeg(sa), RadToDeg(da));
		}
	}

	polycnt = 0;
   return 1;
}

/******************************************************************************
* tan_dimension
*/
static int tan_dimension()
{
   loop_command(dimension_lst, SIZ_DIMENSION_LST);
   return 1;
}

/******************************************************************************
* tan_poly
*/
static int tan_poly()
{
   int lindex;

   polycnt = 0;
   cur_netname .Empty();
   loop_command(poly_lst, SIZ_POLY_LST);

   if (G.cur_layernum == -2)
   {
      lindex = accelLayers.getAllLayer().getLayerIndex();
   }
   else
   {
      lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();
   }

   write_poly(TRUE, lindex, doc->getZeroWidthIndex(), 0, cur_netname, 0, 0);
   polycnt = 0;

   return 1;
}

/******************************************************************************
* poly_pcbpoly
*/
static int poly_pcbpoly()
{
   polycnt = 0;
   loop_command(pcbpoly_lst, SIZ_PCBPOLY_LST);

   Point2 p1 = polyarray.ElementAt(0);
   Point2 p2 = polyarray.ElementAt(polycnt - 1);
   if (p1.x != p2.x || p1.y != p2.y)
   {
      polyarray.SetAtGrow(polycnt, p1);
      polycnt++;
   }

   return 1;
}

/******************************************************************************
* tan_pcbpoly
*/
static int tan_pcbpoly()
{
   if( clear_current_width )
   {
      G.cur_width = 0;
   }
   polycnt = 0;
   cur_netname .Empty();
   loop_command(pcbpoly_lst, SIZ_PCBPOLY_LST);

   // here close it.
   Point2 p1 = polyarray.ElementAt(0);
   Point2 p2 = polyarray.ElementAt(polycnt - 1);
   if (p1.x != p2.x || p1.y != p2.y)
   {
      polyarray.SetAtGrow(polycnt, p1);
      polycnt++;
   }

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   int widthindex;
   int err;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   write_poly(cur_fillmode, lindex, widthindex, 0, cur_netname, 0, 0);
   polycnt = 0;

   return 1;
}

/******************************************************************************
* tan_planeoutline
*/
static int tan_planeoutline()
{
   polycnt = 0;
   cur_netname .Empty();
   loop_command(pcbpoly_lst, SIZ_PCBPOLY_LST);

   // here close it.
   Point2 p1 = polyarray.ElementAt(0);
   Point2 p2 = polyarray.ElementAt(polycnt - 1);
   if (p1.x != p2.x || p1.y != p2.y)
   {
      polyarray.SetAtGrow(polycnt, p1);
      polycnt++;
   }

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   int widthindex;
   int err;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   write_poly(cur_fillmode, lindex, widthindex, 0, cur_netname, 0, 0);
   polycnt = 0;

   return 1;
}

/******************************************************************************
* tan_poly_cut_out
*/
static int tan_poly_cut_out()
{
   int fillSave( cur_fillmode );
   cur_fillmode = FALSE;
   int ret( loop_command(polyCutOutcontents_lst, SIZ_POLYCUTOUT_LST) );
   cur_fillmode = fillSave;
   return ret;
}

/******************************************************************************
* tan_planeobj
*/
static int tan_planeobj()
{
   polycnt = 0;
   cur_fillmode = FALSE;
   G.cur_width = 0;
   clear_current_width = false;
   loop_command(planeobj_lst, SIZ_PLANEOBJ_LST);
   clear_current_width = true;
   cur_fillmode = TRUE;

   return 1;
}

/******************************************************************************
* island_cutout
*/
static int island_cutout()
{
   polycnt = 0;
   loop_command(cutout_lst, SIZ_CUTOUT_LST);
   return 1;
}

/******************************************************************************
* dimgraphics
*/
static int dimgraphics()
{
   loop_command(dimgraphics_lst, SIZ_DIMGRAPHICS_LST);
   return 1;
}

/******************************************************************************
* island_islandoutline
*/
static int island_islandoutline()
{
   polycnt = 0;
   loop_command(poly_lst, SIZ_POLY_LST);

   Point2 p1 = polyarray.ElementAt(0);
   Point2 p2 = polyarray.ElementAt(polycnt - 1);
   if (p1.x != p2.x || p1.y != p2.y)
   {
      polyarray.SetAtGrow(polycnt, p1);
      polycnt++;
   }

   int filled = 1;
   int close = 1;
   Graph_Poly(NULL, doc->getZeroWidthIndex(), filled, 0, close);

   // here write lines.
   for (int i=0; i<polycnt; i++)
   {
      Point2 p1 = polyarray.ElementAt(i);
      Graph_Vertex(p1.x, p1.y,0.0);
   }
   polycnt = 0;

   return 1;
}

/******************************************************************************
* cutout_cutoutoutline
*/
static int cutout_cutoutoutline()
{
   polycnt = 0;
   loop_command(poly_lst, SIZ_POLY_LST);

   Point2 p1 = polyarray.ElementAt(0);
   Point2 p2 = polyarray.ElementAt(polycnt - 1);
   if (p1.x != p2.x || p1.y != p2.y)
   {
      polyarray.SetAtGrow(polycnt, p1);
      polycnt++;
   }

   int   filled = 1;
   int   close = 1;
   int   voidpoly = 1;
   Graph_Poly(NULL, doc->getZeroWidthIndex(), filled, voidpoly, close);

   // here write lines.
   for (int i=0; i<polycnt; i++)
   {
      Point2   p1 = polyarray.ElementAt(i);
      Graph_Vertex(p1.x, p1.y, 0.0);
   }
   polycnt = 0;

   return 1;
}

/******************************************************************************
* dimgraphics_dimgraphic
*/
static int dimgraphics_dimgraphic()
{
   loop_command(dimgraphic_lst, SIZ_DIMGRAPHIC_LST);

   return 1;
}

/******************************************************************************
* dimgraphic
*/
static int dimgraphic()
{
   startnew = TRUE;
   loop_command(layercontents_lst, SIZ_LAYERCONTENTS_LST);

   return 1;
}

/******************************************************************************
* padstyle_shapeoutline
*/
static int padstyle_shapeoutline()
{
   polycnt = 0;
   loop_command(poly_lst, SIZ_POLY_LST);

   return 1;
}

/******************************************************************************
* tan_polykeepout
*/
static int tan_polykeepout()
{
   cur_netname .Empty();
   loop_command(poly_lst, SIZ_POLY_LST);

   return 1;
}

/******************************************************************************
* tan_island
*/
static int tan_island()
{
   if( read_pour_boundary == true )    //islands should not be read, defined in accel.in file
   {
      return fskip();
   }

   polycnt = 0;

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   // here is netname
   DataStruct *data = Graph_PolyStruct(lindex, 0L, 0);
   data->setGraphicClass(graphic_class);

   if (!cur_netname.IsEmpty()) //(strlen(cur_netname))
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, cur_netname.GetBuffer(0), attributeUpdateOverwrite, NULL);

   loop_command(island_lst, SIZ_ISLAND_LST);

   return 1;
}

/******************************************************************************
* tan_boundaryPoly
*/
static int tan_boundaryPoly()
{
   if( read_pour_boundary != true )
   {
      return fskip();
   }

   G.cur_width = 0;
   polycnt = 0;
   loop_command(pcbpoly_lst, SIZ_PCBPOLY_LST);

   // here close it.
   Point2 p1 = polyarray.ElementAt(0);
   Point2 p2 = polyarray.ElementAt(polycnt - 1);
   if (p1.x != p2.x || p1.y != p2.y)
   {
      polyarray.SetAtGrow(polycnt, p1);
      polycnt++;
   }

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   int widthindex;
   int err;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   write_poly( FALSE, lindex, widthindex, GR_CLASS_ETCH, cur_netname, 0, 1 );
   polycnt = 0;

   return 1;
}

/******************************************************************************
* tan_copperpour95
*/
static int tan_copperpour95()
{
   polycnt = 0;
   cur_netname .Empty();
   loop_command(copperpour95_lst, SIZ_COPPERPOUR95_LST);
   polycnt = 0;
   startnew = TRUE;

   return 1;
}

/******************************************************************************
* tan_originalname
*/
static int tan_originalname()
{
   if (!get_tok())
      return p_error();  // name
   cur_originalname = token;

   return 1;
}

/******************************************************************************
* char *find_blockname_from_original
*  make sure that the original name is unique
*/
static const char *find_blockname_from_original(const char *o, int *found)
{
   BlockStruct *block;

   *found = 0;
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      block = doc->getBlockAt(i);
   
      if (block != NULL && block->getOriginalName().Compare(o) == 0)
         *found += 1;
   }

   if (*found == 1)
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         block = doc->getBlockAt(i);
         
         if ( block != NULL && block->getOriginalName().Compare(o) == 0)
            return block->getName();
      } 
   }
   else
   {
		fprintf(ferr, "Patterndef Original Name [%s] is used in multiple Patterndefinitions", o);
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         block = doc->getBlockAt(i);

         if ( block != NULL && block->getOriginalName().Compare(o) == 0 )
            fprintf(ferr, " [%s]", block->getName());
      }
      fprintf(ferr, "\n");
      display_error++;
   }

   return o;
}

/******************************************************************************
* attached_patternNum
*/
static int attached_patternNum()
{
   if (!get_tok())
      return p_error();  // pattern Number
	getLastCompDef()->patternNum = token;

	return 1;
}

/******************************************************************************
* attached_patternname
*/
static int attached_patternname()
{
   if (!get_tok())
      return p_error();  // name
   getLastCompDef()->patternName = token;

   return 1;
}

/******************************************************************************
* tan_compref
*/
static int tan_compref()
{
   if (!get_tok())
      return p_error();  // name
   getLastCompInst()->compRef = token;

   return 1;
}

/******************************************************************************
* tan_compvalue
*/
static int tan_compvalue()
{

   if (!get_tok())
      return p_error();  // name

   if (token.GetLength() > 0 && token.CompareNoCase("{Value}") != 0) // if not generic
		getLastCompInst()->value = token;

   return 1;
}

/******************************************************************************
* tan_textstyleref
*/
static int tan_textstyleref()
{
   if (!get_tok())
      return p_error();  // name

   for (int i=0; i<textstylecnt; i++)
   {
      if (token.CompareNoCase(textstylearray[i]->font) == 0) //!STRCMPI(textstylearray[i]->font, token))
      {
         G.cur_height = textstylearray[i]->height;
         G.cur_width = textstylearray[i]->width;
         return 1;
      }
   }
   fprintf(ferr, "Can not find textstyle [%s]\n", token);
   display_error++;

   return 1;
}

/******************************************************************************
* attr_textstyleref
*/
static int attr_textstyleref()
{
   if (!get_tok())
     return p_error();  // name

   if (GetAttr)
   {
      for (int i=0; i<textstylecnt; i++)
      {
         if (textstylearray[i]->font.CompareNoCase(token) == 0) //(!STRCMPI(textstylearray[i]->font,token))
         {
            attarray[attcnt-1]->height = textstylearray[i]->height;
            attarray[attcnt-1]->width = textstylearray[i]->width;
            return 1;
         }
      }
      fprintf(ferr, "Can not find textstyle [%s]\n", token);
      display_error++;
   }
   return 1;
}

/******************************************************************************
* tan_pad2padclear
*/
int layerdef_pad2padclear()
{
   double c =  tan_unit();

#ifdef WOLF
   if (G.cur_layernum == 1)
      L_TClearancePP(inch_2_dbunit(c));
#endif

   return 1;
}

/******************************************************************************
* tan_pad2lineclear
*/
int layerdef_pad2lineclear()
{
   double c =  tan_unit();

#ifdef WOLF
   if (G.cur_layernum == 1)
      L_TClearancePT(inch_2_dbunit(c));
#endif

   return 1;
}

/******************************************************************************
* tan_line2lineclear
*/
int layerdef_line2lineclear()
{
   double c =  tan_unit();

#ifdef WOLF
   if (G.cur_layernum == 1)
      L_TClearanceTT(inch_2_dbunit(c));
#endif

   return 1;
}

/******************************************************************************
* tan_layertype
*/
static int tan_layertype()
{
   if (!get_tok())
      return p_error();  // name

   G.cur_layerType = stringToAccelLayerType(token);
   //strcpy(cur_layertype, token);

   return 1;
}

/******************************************************************************
* layerdef_titlesheet
*/
static int layerdef_titlesheet()
{
   if (!get_tok())
      return p_error();  // name

   CString titlename = token;

   if (!get_tok())
      return p_error();  // scale ???

   double scale = atof(token);

   if (titlename.IsEmpty())
   {
      titlename.Format("%s_titlesheet_%d", cur_layerDefName, ++titleSheetCnt);
   }

   BlockStruct *b = Graph_Block_On(GBO_APPEND, titlename, file->getFileNumber(), 0);

   graphic_class = 0;
   zero_attcnt();
   loop_command(titlesheet_lst, SIZ_TITLESHEET_LST);

   Graph_Block_Off();
   DataStruct *d = Graph_Block_Reference(titlename, "", file->getFileNumber(), 0.0, 0.0, DegToRad(0.0), 0 , scale, -1, TRUE);

   return 1;
}

/******************************************************************************
* tan_netnameref
*/
static int tan_netnameref()
{
   if (!get_tok())
      return p_error();  // name
   cur_netname = token;
   update_net(cur_netname);

   return 1;
}

/******************************************************************************
* update_net
*/
static int update_net(CString n)
{
   for (int i=0;i<netlistcnt;i++)
   {
      if (n.Compare(netlist[i].net_name) == 0) //(!strcmp(netlist[i].net_name, n))
         return i;      
   }

   if (netlistcnt < MAX_NETS)
   {
      if ((netlist[netlistcnt].net_name = STRDUP(cur_netname)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      netlistcnt++;
   }
   else
   {
      fprintf(ferr, "Too many nets [%d]\n", MAX_NETS);
      display_error++;
   }

   return netlistcnt-1;
}

/******************************************************************************
* tan_viastyleref
*/
static int tan_viastyleref()
{
   if (!get_tok())
      return p_error();  // padname
   cur_padname = token;

   return 1;
}

/******************************************************************************
* tan_patternref
*/
static int tan_patternref()
{
   if (!get_tok())
      return p_error();  // name
   cur_libname = token;

   return 1;
}

/******************************************************************************
* tan_refdesref
*/
static int tan_refdesref()
{
   if (!get_tok())
      return p_error();  // name
   cur_refdesRef = token;

   return 1;
}

/******************************************************************************
* get_layerclass
*/
static GraphicClassTag get_layerclass(const char *layername)
{
   for (int i=0; i<compoutlinecnt; i++)
   {
      if (!STRCMPI(layername,compoutline[i]))
         return graphicClassComponentOutline; 
   }

   if (!STRCMPI(layername, boardoutline))
      return graphicClassBoardOutline;         // a closed, unique board outline is not guarantied in ACCEL

   return graphicClassNormal;
}

/******************************************************************************
* tan_layernumref
*/
static int tan_layernumref()
{
   if (!get_tok())
      return p_error();  // name

   G.cur_layernum = atoi(token);

   // update used.
   for (int i=0;i < accelLayers.getSize();i++)
   {
      CAccelLayer* accelLayer = accelLayers.getAt(i);

      if ( accelLayer != NULL && accelLayer->getNumber() == G.cur_layernum)
      {
         accelLayer->setUsed(true);

         if (accelLayer->getAccelLayerType() == accelLayerTypeSignal)
         {
            graphic_class = GR_CLASS_ETCH;
         }
         else
         {
            graphic_class = get_layerclass(accelLayer->getName());
         }

         break;
      }
   }

   return 1;
}

/******************************************************************************
* tan_layernumref
*/
int layerdef_layernumref()
{
   if (!get_tok())
      return p_error();  // name

	G.cur_layernum = atoi(token);
   CAccelLayer& accelLayer = accelLayers.getCurrentAccelLayer();
   accelLayer.setNumber(G.cur_layernum);

   return 1;
}

/******************************************************************************
* tan_layernumref
*/
int layerdef_netnameref()
{
   if (!get_tok())
      return p_error();  // name

	// TOM - Case 1253 - The current accel layer gets stuck on the last read layer from
	// the first pass through.  Because of this, and the fact that the first pass through set
	// the names correctly that time, we can simply bypass the naming portion to prevent 
	// improper mapping of information to the last layer read on the first pass.
	if (current_file_pass > 1)
		return 1;

   CAccelLayer& accelLayer = accelLayers.getCurrentAccelLayer();
   accelLayer.setNetName(token);

   return 1;
}

/******************************************************************************
* layerdef_layertype
*/
static int layerdef_layertype()
{
   if (!get_tok())
      return p_error();  // name

   CAccelLayer& accelLayer = accelLayers.getCurrentAccelLayer();
   accelLayer.setAccelLayerType(token);

   return 1;
}

/******************************************************************************
* tan_pt
*  (pt number [unit] number [unit])
*/
static int tan_pt()
{
   Point2 p1;
   p1.x = tan_unit();
   p1.y = tan_unit();
   polyarray.SetAtGrow(polycnt, p1);
   polycnt++;

   return 1;
}

/******************************************************************************
* tan_extent
*/
static int tan_extent()
{
   G.cur_extent_x = tan_unit();
   G.cur_extent_y = tan_unit();
   return 1;
}

/******************************************************************************
* attr_pt
*  (pt number [unit] number [unit])
*/
static int attr_pt()
{
   if (GetAttr)
   {
      attarray[attcnt-1]->x = tan_unit();
      attarray[attcnt-1]->y = tan_unit();
   }
   else
   {
      if (!get_tok())
         return p_error();
      if (!get_tok())
         return p_error();
      if (token.CompareNoCase("mm") != 0) // if (STRCMPI(token, "mm"))
         Push_tok = TRUE;

      if (!get_tok())
         return p_error();
      if (!get_tok())
         return p_error();
      if (token.CompareNoCase("mm") != 0) //if (STRCMPI(token, "mm"))
         Push_tok = TRUE;
   }


   return 1;
}

/******************************************************************************
* tan_unit
*  (pt number [unit] number [unit])
*/
double tan_unit()
{
   CString tmp;
   int oldunit = tangounit;

   if (!get_tok())
      return p_error();				// is number
   tmp = token;

   if (tmp.Find("mm") > -1) //if (strstr(tmp, "mm"))			// 15.6mm
   {
      tangounit = UNIT_MM;
   }
   else if (tmp.Find("mil") > -1) //(strstr(tmp, "mil"))	// 15.6mil
   {
      tangounit = UNIT_MILS;
   }
   else if (tmp.Find("in") > -1) // (strstr(tmp, "in"))	// 15.6in
   {
      tangounit = UNIT_INCHES;
   }
   else
   {
      if (!get_tok())
         return p_error();			// maybe unit

      if (token.CompareNoCase("mm") == 0) //(!STRCMPI(token, "mm"))
         tangounit = UNIT_MM;
      else if (token.CompareNoCase("mil") == 0) //(!STRCMPI(token, "mil"))
			tangounit = UNIT_MILS;
      else if (token.CompareNoCase("in") == 0) //(!STRCMPI(token, "in"))
			tangounit = UNIT_INCHES;
      else
         Push_tok = TRUE;
   }

   double x = cnv_unit(tmp);
   tangounit = oldunit;

   return x;
}

/******************************************************************************
* tan_startangle
*/
static int tan_startangle()
{
   if (!get_tok())
      return p_error();  //

   Point2 p = polyarray.ElementAt(2);
   p.x = atof(token);
   polyarray.SetAt(2, p);

   return 1;
}

/******************************************************************************
* tan_rotation
*/
static int tan_rotation()
{
   if (!get_tok())
      return p_error();  //
   G.cur_rotation = atof(token);

   return 1;
}

/******************************************************************************
* attr_rotation
*/
static int attr_rotation()
{
   if (!get_tok())
      return p_error();  //
   if (GetAttr)
      attarray[attcnt-1]->rotation = atof(token);

   return 1;
}

/******************************************************************************
* attr_justify
*/
static int attr_justify()
{
   if (GetAttr)
   {
      int oldjust = G.cur_textjust;
      tan_justify();
      attarray[attcnt-1]->justify = G.cur_textjust;
      G.cur_textjust = oldjust;
   }
   else
   {
      if (!get_tok())
         return p_error();
   }

   return 1;
}

/******************************************************************************
* get_attr_units
*/
static int get_attr_units(const char *t)
{
   for (int i=0; i<SIZ_ATTR_UNITS; i++)
   {
      if (!STRCMPI(attr_units[i], t))
         return i;
   }
   return -1;
}

/******************************************************************************
* attr_constraintunits
*
*  constraintUnits ::= '(' 'constraintUnits'
*  'mil' | 'inch' |
*  'millimeter' | 'centimeter' | 'micrometer' | 'nanometer' | 'picometer' | 'meter' |
*  'layername' |
*  'viastyle' |
*  'radian' | 'degree' |
*  'ohm' | 'mho' |
*  'volt' | 'millivolt' | 'microvolt' | 'nanovolt' | 'picovolt' |
*  'ampere' | 'milliampere' | 'microampere' | 'nanoampere' | 'picoampere' |
*  'henry' | 'millihenry' | 'microhenry' | 'nanohenry' | 'picohenry' |
*  'farad' | 'millifarad' | 'microfarad' | 'nanofarad' | 'picofarad' |
*  'second' | 'millisecond' | 'microsecond' | 'nanosecond' | 'picosecond' |
*  'bool' |
*  'hertz' | 'kilohertz' | 'megahertz' | 'gigahertz'
*  'watt' | 'milliwatt' | 'microwatt' | 'nanowatt' | 'picowatt' |
*  'quantity' |
*  'string'
*  ')'
*/
static int attr_constraintunits()
{
   if (!get_tok())
      return p_error();  //
   if (attcnt && GetAttr)
      attarray[attcnt-1]->attr_units = get_attr_units(token);

   return 1;
}

/******************************************************************************
* tan_justify
*/
static int tan_justify()
{
   if (!get_tok())
      return p_error();  //

   if (token.CompareNoCase("Left") == 0) //(!STRCMPI(token, "Left"))
   {
      G.cur_textjust = 0;
   }
   else if (token.CompareNoCase("Center") == 0) //(!STRCMPI(token, "Center"))
   {
      G.cur_textjust = GRTEXT_H_C | GRTEXT_W_C;
   }
   else if (token.CompareNoCase("Right") == 0) //(!STRCMPI(token, "Right"))
   {
      G.cur_textjust = GRTEXT_H_C | GRTEXT_W_R;
   }
   else if (token.CompareNoCase("Left") == 0) // looks like a bug to me, should be LowerLeft ?  ORIGINAL: (!STRCMPI(token, "Left"))
   {
      G.cur_textjust = GRTEXT_H_B | GRTEXT_W_L;
   }
   else if (token.CompareNoCase("LowerRight") == 0) //(!STRCMPI(token, "LowerRight"))
   {
      G.cur_textjust = GRTEXT_H_B | GRTEXT_W_R;
   }
   else if (token.CompareNoCase("UpperRight") == 0) //(!STRCMPI(token, "UpperRight"))
   {
      G.cur_textjust = GRTEXT_H_T | GRTEXT_W_R;
   }
   else if (token.CompareNoCase("UpperLeft") == 0) //(!STRCMPI(token, "UpperLeft"))
   {
      G.cur_textjust = GRTEXT_H_T | GRTEXT_W_L;
   }
   else if (token.CompareNoCase("UpperCenter") == 0) //(!STRCMPI(token, "UpperCenter"))
   {
      G.cur_textjust = GRTEXT_H_T | GRTEXT_W_C;
   }
   else if (token.CompareNoCase("LowerCenter") == 0) //(!STRCMPI(token, "LowerCenter"))
   {
      G.cur_textjust = GRTEXT_H_B | GRTEXT_W_C;
   }
   else
   {
      fprintf(ferr, "Text justificaction [%s] not implemented at %ld\n", token, ifp_line);
      display_error++;
   }
   return 1;
}

/******************************************************************************
* tan_isflipped
*/
static int tan_isflipped()
{
   if (!get_tok())
      return p_error();  //
                       
   if (token.CompareNoCase("TRUE") == 0) // (!STRCMPI(token, "TRUE"))
      G.cur_mirror = TRUE;
   else
      G.cur_mirror = FALSE;

   return 1;
}

/******************************************************************************
* tan_isfixed
*/
static int tan_isfixed()
{
   if (!get_tok())
      return p_error();  //

   if (token.CollateNoCase("TRUE") == 0) //(!STRCMPI(token, "TRUE"))
      G.cur_fixed = TRUE;
   else
      G.cur_fixed = FALSE;

   return 1;
}

/******************************************************************************
* attr_isflipped
*/
static int attr_isflipped()
{
   if (!get_tok())
      return p_error();  //

   if (GetAttr)
   {
      if (token.CompareNoCase("TRUE") == 0) //(!STRCMPI(token, "TRUE"))
         attarray[attcnt-1]->mirror = TRUE;
      else
         attarray[attcnt-1]->mirror = FALSE;
   }

   return 1;
}

/******************************************************************************
* attr_isvisible
*/
static int attr_isvisible()
{
   if (!get_tok())
         return p_error();  //
   if (GetAttr)
   {
      if (token.CompareNoCase("TRUE") == 0) //(!STRCMPI(token, "TRUE"))
         attarray[attcnt-1]->visible = TRUE;
      else
         attarray[attcnt-1]->visible = FALSE;
   }
   return 1;
}

/******************************************************************************
* tan_sweepangle
*/
static int tan_sweepangle()
{
   Point2 p = polyarray.ElementAt(2);
   if (!get_tok())
      return p_error();  //
   p.y = atof(token);
   polyarray.SetAt(2, p);

   return 1;
}

/******************************************************************************
* tan_radius
*/
static int tan_radius()
{
   Point2 p = polyarray.ElementAt(1);
   p.x = tan_unit();
   polyarray.SetAt(1, p);

   return 1;
}

/******************************************************************************
* font_type
*/
static int font_type()
{
   if (!get_tok())
      return p_error();  //

   return 1;
}

/******************************************************************************
* font_family
*/
static int font_family()
{
   if (!get_tok())
      return p_error();  //

   return 1;
}

/******************************************************************************
* font_face
*/
static int font_face()
{
   if (!get_tok())
      return p_error();  //

   return 1;
}

/******************************************************************************
* tan_height
*/
static int tan_height()
{
   G.cur_height = tan_unit();
   return 1;
}

/******************************************************************************
* tan_width
*/
static int tan_width()
{
   G.cur_width = tan_unit();
   return 1;
}

/******************************************************************************
* tan_netlist
*/
static int tan_netlist()
{
   if (!get_tok())
      return p_error();  // libraryname

   return loop_command(net_lst, SIZ_NET_LST);
}

/******************************************************************************
* tan_pcbdesign
*/
static int tan_pcbdesign()
{
   G.cur_status = STAT_DESIGN;
   if (!get_tok())
      return p_error();  // designname

   return loop_command(pcb_lst, SIZ_PCB_LST);
}

static int tan_pcbdesign_pass1()
{
   if (!get_tok())
      return p_error();  // designname

   return loop_command(pcb_pass1_lst, SIZ_PCB_Pass1_LST);
}

/******************************************************************************
* pcb_pcbdesignheader
*/
int pcb_pcbdesignheader()
{
   return loop_command(pcbDesignHeader_lst,SIZ_PcbDesignHeader_LST);
}

int pcbDesignHeader_pasteSwell()
{
   CAccelPadDefState::getAccelPadDefState().setPasteSwell(tan_unit());

   return 1;
}

int pcbDesignHeader_solderSwell()
{
   CAccelPadDefState::getAccelPadDefState().setSolderSwell(tan_unit());

   return 1;
}

int pcbDesignHeader_planeSwell()
{
   CAccelPadDefState::getAccelPadDefState().setPlaneSwell(tan_unit());

   return 1;
}


/******************************************************************************
* pcb_pcbmultilayer
*/
int pcb_pcbmultilayer()
{
   G.cur_status = STAT_DESIGN;
   return loop_command(pcbmulti_lst, SIZ_PCBMULTI_LST);
}

/******************************************************************************
* libpatternextended_originalname
*/
static int libpatternextended_originalname()
{
   if (!get_tok())
      return p_error();  // refname

	cur_originalname = token;
}

/******************************************************************************
* libpatternextended_patterngraphicsnameref
*/
static int libpatternextended_patterngraphicsnameref()
{
   if (!get_tok())
      return p_error();  // refname

   return 1;
}

/******************************************************************************
* pattern_patterngraphicsnameref
*/
static int pattern_patterngraphicsnameref()
{
   if (!get_tok())
      return p_error();  // refname
   if (PatternBegin)
   {
      PatternGraphicNameRef = token;
      PatternBegin = FALSE;
   }
   else
   {
      if (!PatternGraphicNameRef.CompareNoCase(token))
         GetAttr = TRUE;
      else
         GetAttr = FALSE;
   }
   return 1;
}

/******************************************************************************
* libpatternextended_patterngraphicsnamedef
*/
static int libpatternextended_patterngraphicsnamedef()
{
   if (!get_tok())
      return p_error();  // refname

	CString graphicName = token;
	ACCELPatterndef *patternDef = setLastPatternGraphicAndGeomName(graphicName);

   return 1;
}

/******************************************************************************
* libpatternextended_patterngraphicsdef
*/
static int libpatternextended_patterngraphicsdef()
{
	// This name is use to temperary name the geometry until it is renamed by the function setLastPatternGraphicAndGeomName()
	// It is need because more than one "patternGraphicsDef" is possible for the same "patternDefExtended" 
	CString tempBlockName = cur_libname + "_patternDef";

	BlockStruct *block = Graph_Block_On(GBO_APPEND, tempBlockName, file->getFileNumber(), 0L);
	block->setOriginalName(cur_originalname);

	ACCELPatterndef *patternDef = getNewPatternDef(cur_libname, block);	// cur_libname is the pattern name
	patternDef->original = cur_originalname;

   loop_command(patterngraphicsdef_lst, SIZ_PATTERNGRAPHICSDEF_LST);
		
   if (G.cur_pincnt)	// if it has no pins, it can not be a PCB Component, but a logo 
      block->setBlockType(BLOCKTYPE_PCBCOMPONENT);

   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* pattern_multi
*/
static int pattern_multi()
{

   G.cur_pincnt = 0;  // count for pin per comp.
   loop_command(patternmulti_lst, SIZ_PATTERNMULTI_LST);
   G.cur_pincnt = libpincnt;

   ACCELPatterndef *patternDef = getLastPattern();
   patternDef->number_of_pads = G.cur_pincnt;

   int use_pad_mirror = FALSE;
   if (patternDef->pads_flipped)
   {
      if (patternDef->pads_flipped == patternDef->number_of_pads && patternDef->number_of_pads == 1)
      {
         // all pins are flipped in this definition, this is done on testpads and I will mirror the component
         use_pad_mirror = FALSE;
         patternDef->flip_component = TRUE;
      }
      else
      {
         // some pins are flipped ???
         use_pad_mirror = TRUE;
      }
   }
   write_libpin(use_pad_mirror);

   return 1;
}

/******************************************************************************
* pcb_pcblayerdef
*/
static int pcb_pcblayerdef_pass1()
{
   if (!get_tok())
      return p_error();  // layername

   cur_layerDefName = token;
   cur_layerDefName.MakeUpper();

   accelLayers.addAccelLayer(cur_layerDefName);

   loop_command(layerdef_lst, SIZ_LAYERDEF_LST);

   return 1;
}

/******************************************************************************
* pcb_pcblayerdef
*/
static int pcb_pcblayerdef()
{
   if (!get_tok())
      return p_error();  // layername

   cur_layerDefName = token;

   // initialize here because there could be multiple titlesheet defined within one layer def
   titleSheetCnt = 0;   

   cur_netname .Empty();

   loop_command(layerdef_lst, SIZ_LAYERDEF_LST);

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   LayerStruct *curlayer = doc->FindLayer(lindex);
   curlayer->setComment(cur_layerDefName);

   if (!cur_netname.IsEmpty()) //(strlen(cur_netname))
   {
      // netname is marked
      int nr = update_net(cur_netname);
      netlist[nr].layernumber = G.cur_layernum;
      doc->SetUnknownAttrib(&curlayer->getAttributesRef(), LAYATT_NETNAME, cur_netname, SA_APPEND, NULL);
   }

   return 1;
}

/******************************************************************************
* zero_attcnt
*/
static void zero_attcnt()
{
   for (int i=0;i<attcnt;i++)
      delete attarray[i];
   attcnt = 0;
}

/******************************************************************************
* tan_layercontents
*/
static int tan_layercontents()
{
   // here set status SCLG_???
   graphic_class = 0;
   zero_attcnt();
   startnew = TRUE;
   G.cur_width = 0;

   loop_command(layercontents_lst, SIZ_LAYERCONTENTS_LST);

   if (G.cur_status == STAT_SHAPE)
   {
		ACCELPatterndef* patternDef = getLastPattern();

      BlockStruct *b = NULL;
		if (patternDef == NULL)
			b = Graph_Block_On(GBO_APPEND, cur_libname, file->getFileNumber(), 0);
		else
			b = Graph_Block_On(GBO_APPEND, patternDef->geomName, file->getFileNumber(), 0);

      Graph_Block_Off();
      for (int i=0; i<attcnt; i++)
      {  
         ACCELAtt *accelAtt = attarray[i];

         if (accelAtt->mirror)
            accelAtt->rotation = calc_rotation(360 - accelAtt->rotation);

         // for justification, we assume 3 char len if there is no value.
         // this value may be attributeUpdateOverwrite later, but than I do not know the justification flag 
         // anymore.
         int v = strlen(accelAtt->val);
         if (v < 3)
            v = 3;

         normalize_text(&accelAtt->x, &accelAtt->y, accelAtt->justify, accelAtt->rotation,
               accelAtt->mirror, accelAtt->height, accelAtt->height * TEXT_RATIO * v);

         if (compheight.CompareNoCase(attarray[i]->key) != 0) // component height not done.
         {
            int attlayernr = accelLayers.getLayer(G.cur_layernum).getLayerIndex();
            accelAtt->height *= TEXT_CORRECT;

            if (!STRCMPI(accelAtt->key, "RefDes"))
            {
               if (strlen(accelAtt->val))
                  doc->SetVisAttrib(&b->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, 0), valueTypeString, accelAtt->val.GetBuffer(0),
                        accelAtt->x, accelAtt->y, DegToRad(accelAtt->rotation), accelAtt->height, accelAtt->height * TEXT_RATIO,
                        0, 0, accelAtt->visible, attributeUpdateOverwrite, 0L, attlayernr, 0, 0, 0);
            }
            else if (!STRCMPI(accelAtt->key, ATT_VALUE))
            {
               if (STRCMPI(accelAtt->val,"{Value}"))
               {
                  if (strlen(accelAtt->val))
                     doc->SetVisAttrib(&b->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 0), valueTypeString, accelAtt->val.GetBuffer(0),
                           accelAtt->x, accelAtt->y, DegToRad(accelAtt->rotation), accelAtt->height, accelAtt->height * TEXT_RATIO,
                           0, 0, accelAtt->visible, attributeUpdateOverwrite, 0L, attlayernr, 0, 0, 0);
               }
            }
            else
            {
               doc->SetUnknownVisAttrib(&b->getAttributesRef(), accelAtt->key, accelAtt->val, accelAtt->x, accelAtt->y,
                     DegToRad(accelAtt->rotation), accelAtt->height, accelAtt->height * TEXT_RATIO, 
                     0, 0, accelAtt->visible, attributeUpdateOverwrite, 0L, attlayernr, 0, 0, 0);
            }
         }
      }
      zero_attcnt();
   }

   graphic_class = 0;
   return 1;
}

/******************************************************************************
* pcb_layercontents
*/
static int pcb_layercontents()
{
//   G.cur_status = STAT_DESIGN;
//   SCLG_Netlist("NONET");
   startnew = TRUE;
   loop_command(layercontents_lst, SIZ_LAYERCONTENTS_LST);
   return 1;
}

/******************************************************************************
* pcb_pcbprintsettings
*/
static int pcb_pcbprintsettings()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pcb_drillsymsettings
*/
static int pcb_drillsymsettings()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pcb_gerbersettings
*/
static int pcb_gerbersettings()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pcb_ncdrillsettings
*/
static int pcb_ncdrillsettings()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pcb_programstate
*/
static int pcb_programstate()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* compdef_compheader
*/
static int compdef_compheader()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* compdef_comppin
*/
static int compdef_comppin()
{
   fskip();
   return 1; //loop_command(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* compdef_attachedpattern
*/
static int compdef_attachedpattern()
{
   loop_command(attachedpattern_lst, SIZ_ATTACHEDPATTERN_LST);
   return 1;
}

/******************************************************************************
* calc_rotation
*/
static double calc_rotation(double rot)
{
   while (rot < 0)
      rot += 360;
   while (rot >= 360)
      rot -= 360;

   return rot;
}

/******************************************************************************
* get_patterndef_ptr
*/
static int get_patterndef_ptr(const char *pp)
{  

// Note(Lynn):  Might need to done need this function anymore

   //for (int i=0; i<patterndefcnt; i++)
   //{
   //   if (patterndefarray[i]->name.CompareNoCase(pp) == 0)
   //      return i;
   //}

   return -1;
}

/******************************************************************************
* pattern_patterngraphicsref
*/
static int pattern_patterngraphicsref()
{
   loop_command(pcbpatterngraphicsref_lst, SIZ_PCBPATTERNGRAPHICSREF_LST);
   return 1;
}

/******************************************************************************
* pcbmulti_pattern
*   here is the component placement
*/
static int pcbmulti_pattern()
{
   PatternBegin = TRUE;
	PatternGraphicNameRef = "";
	GetAttr = FALSE;
	cur_refdesRef	= "";
   G.cur_fixed    = 0;
   G.cur_rotation = 0.0;
   G.cur_mirror   = 0;
   polycnt        = 0;
   attcnt         = 0;

   loop_command(pcbpattern_lst, SIZ_PCBPATTERN_LST);
   GetAttr = TRUE;

	CString patternRef = cur_libname;
	CString refdesRef = cur_refdesRef;

	ACCELPatterndef *patternDef = findPatternDef(patternRef, PatternGraphicNameRef);
 	if (patternDef != NULL)
	{
		// Make sure the block exist
		BlockStruct *block = patternDef->geomBlock;
		if (block == NULL)
		{
			fprintf(ferr, "Error : can not find PatternDef [%s] at %ld\n", patternDef->geomName, ifp_line);
			display_error++;
			return 1;
		}


		TypeStruct *type = NULL;
		ACCELCompInst *compInst = findCompInst(refdesRef);

		if (compInst == NULL)
		{
			fprintf(ferr, "Error : can not find component name [%s] at %ld\n", refdesRef, ifp_line);
			display_error++;
		}
		else
		{
			// Find the compDef
			ACCELCompdef* compDef = findCompDef(compInst->compRef);
			if (compDef == NULL)
			{
				fprintf(ferr, "Error : can not find compDef [%s] at %ld\n", compInst->compRef, ifp_line);
				display_error++;
			}
			else
			{
				CString compDefName = "";
				if (compDef->compDefName.Right(2) == "_1")
					compDefName = compDef->original;
				else
					compDefName = compDef->compDefName;


				if (compDef->pincnt == 0)
				{
					WORD refNameKey = (WORD)doc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);
					WORD valueKey = (WORD)doc->RegisterKeyWord(ATT_VALUE, 0, valueTypeString);
					WORD typeKey = (WORD)doc->RegisterKeyWord("TYPE", 0, valueTypeString);

					RemoveAttrib(refNameKey, &block->getAttributesRef());
					RemoveAttrib(valueKey, &block->getAttributesRef());
					RemoveAttrib(typeKey, &block->getAttributesRef());
				}
				else
				{
					// Check to see if there is a copy of the block created by compDef
					// If there is one then use it
					CString newBlockName = patternRef + "$$" + compDefName;
					BlockStruct *newBlock = Graph_Block_Exists(doc, newBlockName, file->getFileNumber());
					if (newBlock == NULL)
					{
						// If there is none, then check to see if the pin refdes in the compDef is the same as those on the pin of the block
						// If they are different then need to create a copy of the block and change the pin refdes to those 
						// in teh compDef and insert the new block


						bool isPinRefdesDiff = false;
						WORD pinnrKey = (WORD)doc->RegisterKeyWord(ATT_COMPPINNR, 0, valueTypeInteger);
						POSITION pos = block->getHeadDataInsertPosition();
						while (pos)
						{
							DataStruct *data = block->getNextDataInsert(pos);

                     if (data != NULL && data->getAttributes() != NULL && data->getInsert() != NULL)
                     {
							   InsertStruct *insert = data->getInsert();

                        if (insert->getInsertType() == insertTypePin)
                        {
							      Attrib *attrib = NULL;
							      if (data->getAttributes()->Lookup(pinnrKey, attrib) && attrib != NULL)
							      {
								      int pinNum = attrib->getIntValue();
								      CString compPinRef = findCompPinRefByPinNumber(compDef, pinNum);
								      if (insert->getRefname().CompareNoCase(compPinRef) && !compPinRef.Trim().IsEmpty())
								      {
									      isPinRefdesDiff = true;
									      break;
								      }
							      }
                        }
                     }
						}

						if (isPinRefdesDiff == true)
						{
							// Make a copy of the block and rename pin refdes
							newBlock = Graph_Block_On(GBO_APPEND, newBlockName, file->getFileNumber(), 0L);
							Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, TRUE, FALSE);
							Graph_Block_Off();

                     newBlock->setBlockType(block->getBlockType());

							POSITION pos = newBlock->getHeadDataInsertPosition();
							while (pos)
							{
								DataStruct *data = newBlock->getNextDataInsert(pos);

                        if (data != NULL && data->getAttributes() != NULL && data->getInsert() != NULL)
                        {
								   InsertStruct *insert = data->getInsert();

                           if (insert->getInsertType() == insertTypePin)
                           {
								      Attrib *attrib = NULL;
								      if (data->getAttributes()->Lookup(pinnrKey, attrib) && attrib != NULL)
								      {
									      int pinNum = attrib->getIntValue();
									      CString compPinRef = findCompPinRefByPinNumber(compDef, pinNum);
									      if (insert->getRefname().CompareNoCase(compPinRef) && !compPinRef.Trim().IsEmpty())
									      {
										      insert->setRefname(STRDUP(compPinRef));
									      }
								      }
                           }
                        }
							}
						}
					}

					if (newBlock != NULL)
					{
						// A new block is create, so change the block to the newly created block
						block = newBlock;
					}
				}			


				// Create a device type for the block
				CString typeName = compDefName;
				if (patternDef->graphicName.IsEmpty() == false)
				{
					if (patternDef->graphicName.CompareNoCase("Primary") != 0)
						typeName = compDefName + "_" + patternDef->graphicName;
				}

				type = AddType(file, typeName);
				if (type->getBlockNumber() == -1)
				{
					type->setBlockNumber(block->getBlockNumber());
				}
				else if (type->getBlockNumber() != block->getBlockNumber())
				{
					BlockStruct *tmpBlock = doc->getBlockAt(type->getBlockNumber());
					CString newTypeName = typeName + "_" + block->getName();

					type = AddType(file, newTypeName);
					if (type->getBlockNumber() == -1)
					{
						fprintf(ferr, "Device [%s] is already tied to a different Pattern [%s], create new device [%s]\n",
								typeName, tmpBlock->getName(), newTypeName);
						display_error++;

						type->setBlockNumber(block->getBlockNumber());
					}
				}
			}
		}


		if (patternDef->flip_component)
			G.cur_mirror = ~G.cur_mirror;

		if (G.cur_mirror)
			G.cur_rotation = calc_rotation(360 - G.cur_rotation);

		Point2 p = polyarray.ElementAt(0);
      DataStruct *data = Graph_Block_Reference(block->getBlockNumber(), refdesRef, p.x, p.y, DegToRad(G.cur_rotation), G.cur_mirror , 1.0, -1);

		if (type != NULL)
		{
			WORD deviceKey = (WORD)doc->RegisterKeyWord(ATT_TYPELISTLINK, 0, valueTypeString);
			CString deviceType = type->getName();
         data->setAttrib(doc->getCamCadData(), deviceKey, valueTypeString, deviceType.GetBuffer(0), attributeUpdateOverwrite, NULL);
		}

		if (block != NULL && block->getBlockType() == blockTypePcbComponent)
		{
			// Set insert as PCB component
			data->getInsert()->setInsertType(insertTypePcbComponent);
			

			// Set REFNAME attributes
			WORD refnameKey = doc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);
			Attrib *attrib = NULL;
			if (data->getAttributes() && data->getAttributes()->Lookup(refnameKey, attrib) && attrib != NULL)
				attrib->setStringValueIndex(doc->RegisterValue(refdesRef));
			else
				doc->SetAttrib(&data->getAttributesRef(), refnameKey, valueTypeString, refdesRef.GetBuffer(0), attributeUpdateOverwrite, NULL);				

			// Apply attributes in compInst to insert;
			ACCELCompInst *compInst = findCompInst(refdesRef);
			if (compInst == NULL)
			{
				fprintf(ferr, "Error : can not find component name [%s] at %ld\n", refdesRef, ifp_line);
				display_error++;
				return -1;      
			}
			else
			{
				if (compInst->height > 0)
					doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE,
							&(compInst->height), attributeUpdateOverwrite, NULL);

				if (!compInst->value.Trim().IsEmpty())
				{
					// here seperate values
					// Accel allows .01uf 10V 
					//              1 Meg  = 1M
					//             100K 1%
					//             1 Ohm = 1
					char tmp[80];
					strcpy(tmp, compInst->value);
					char *lp = strtok(tmp, " \t");

					while (lp)
					{
						if (strlen(lp) >= 3 &&
							tolower(lp[strlen(lp)-1]) == 'm' && 
							tolower(lp[strlen(lp)-2]) == 'h' &&
							tolower(lp[strlen(lp)-3]) == 'o') // convert ohm to nothing
						{
							lp[strlen(lp)-3] = '\0';
							if (strlen(lp))
								doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 1), valueTypeString, lp, attributeUpdateOverwrite, NULL);
						}
						else if (strlen(lp) >= 3 &&
							tolower(lp[strlen(lp)-1]) == 'g' && 
							tolower(lp[strlen(lp)-2]) == 'e' &&
							tolower(lp[strlen(lp)-3]) == 'm') // convert Meg to M
						{
							lp[strlen(lp)-3] = 'M';
							lp[strlen(lp)-2] = '\0';
							if (strlen(lp))
								doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 1), valueTypeString, lp, attributeUpdateOverwrite, NULL);
						}  
						else if (lp[strlen(lp)-1] == '%')
						{
							double tol = atof(lp);
							doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, 1), valueTypeDouble, &tol, attributeUpdateOverwrite,NULL);
						}
						else if (lp[strlen(lp)-1] == 'V')
						{
							double vol = atof(lp);
							doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VOLTAGE, 1), valueTypeDouble, &vol, attributeUpdateOverwrite, NULL);
						}
						else
						{
							doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 1), valueTypeString, lp, attributeUpdateOverwrite, NULL);
						}
						lp = strtok(NULL, " \t");
					}
				}
			}
		} // if pcbcomponent

		for (int i=0; i<attcnt; i++)
		{
			ACCELAtt *accelAtt = attarray[i];

			if (!STRCMPI(accelAtt->key, "RefDes"))
				accelAtt->val = refdesRef;

			double xoff = 0;
			double yoff = 0;
			int dir = 1;
			if (G.cur_mirror)
			{
				accelAtt->rotation = calc_rotation(360 - accelAtt->rotation);
				Rotate(accelAtt->x, accelAtt->y, 360 - G.cur_rotation, &xoff, &yoff);
				xoff = -xoff;
				if (accelAtt->mirror)   
					accelAtt->mirror = 0;
				else
					accelAtt->mirror = 1;
				dir = -1;
			}
			else
			{
				Rotate(accelAtt->x, accelAtt->y, 360 - G.cur_rotation, &xoff, &yoff);
			}

			accelAtt->rotation = calc_rotation(360 - (accelAtt->rotation + G.cur_rotation));
			if (accelAtt->mirror)
				accelAtt->rotation = calc_rotation(360 - accelAtt->rotation);

			accelAtt->x = 0;
			accelAtt->y = 0;
			normalize_text(&accelAtt->x, &accelAtt->y, accelAtt->justify, accelAtt->rotation, accelAtt->mirror,
					accelAtt->height, accelAtt->height * TEXT_RATIO * strlen(accelAtt->val));

         if (compheight.CompareNoCase(accelAtt->key) != 0)  // component height not done.
         {
			   accelAtt->height *= TEXT_CORRECT;

			   if (!STRCMPI(accelAtt->key, "RefDes"))
			   {
				   if (strlen(refdesRef))
				   {
					   // here check if the visibility flag was inhereted from patterndef
					   int attlayernr;
					   Attrib *attr =  is_attvalue(doc, data->getAttributesRef(), ATT_REFNAME, 0);
					   if (attr)
					   {
						   if (attr->isVisible())
							   accelAtt->visible = TRUE;
						   attlayernr = attr->getLayerIndex();
					   }
					   else
					   {
						   //attlayernr = Graph_Level("REFDES_1", "", 0);
                     attlayernr = accelLayers.getLayer("REFDES_1").getLayerIndex();
					   }

					   doc->SetVisAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, 0), valueTypeString, refdesRef.GetBuffer(0),
						   accelAtt->x + xoff, accelAtt->y + yoff, DegToRad(accelAtt->rotation),   accelAtt->height, 
						   accelAtt->height * TEXT_RATIO, 0, 0, accelAtt->visible, attributeUpdateOverwrite, 0L, attlayernr, 0, 0, 0);
				   }
			   }  
			   else if (!STRCMPI(accelAtt->key, "Value"))
			   {
				   int attlayernr;
				   if (strlen(accelAtt->val))
				   {
					   // here check if the visibility flag was inhereted from patterndef
					   Attrib *attr =  is_attvalue(doc, data->getAttributesRef(), ATT_VALUE, 0);
					   if (attr)
					   {
						   if (attr->isVisible())
							   accelAtt->visible = TRUE;
						   attlayernr = attr->getLayerIndex();
					   }
					   else
					   {
						   //attlayernr = Graph_Level("VALUE_1", "", 0);
                     attlayernr = accelLayers.getLayer("VALUE_1").getLayerIndex();
					   }

					   doc->SetVisAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 0), valueTypeString, accelAtt->val.GetBuffer(0),
						   	accelAtt->x + xoff, accelAtt->y + yoff, DegToRad(accelAtt->rotation), accelAtt->height, 
							   accelAtt->height * TEXT_RATIO, 0, 0, accelAtt->visible, attributeUpdateOverwrite, 0L, attlayernr, 0, 0, 0);
				   }
			   }
			   else
			   {
				   // here check if the visibility flag was inhereted from patterndef
				   int attlayernr;
				   Attrib *attr =  is_attvalue(doc, data->getAttributesRef(), accelAtt->key, 0);
				   if (attr)
				   {
					   if (attr->isVisible())
						   accelAtt->visible = TRUE;
					   attlayernr = attr->getLayerIndex();
				   }
				   else
				   {
					   if (accelAtt->visible)
                  {
						   //attlayernr = Graph_Level("ATTRIBUTE_1", "", 0);
                     attlayernr = accelLayers.getLayer("ATTRIBUTE_1").getLayerIndex();
                  }
					   else
                  {
						   attlayernr = -1;
                  }
				   }

				   doc->SetUnknownVisAttrib(&data->getAttributesRef(), accelAtt->key, accelAtt->val, accelAtt->x + xoff, accelAtt->y + yoff, 
					   	DegToRad(accelAtt->rotation), accelAtt->height, accelAtt->height * TEXT_RATIO, 0, 0, accelAtt->visible,
						   attributeUpdateOverwrite, 0L, attlayernr, 0, 0, 0);
			   }
         }
		}
	}

   zero_attcnt();
   return 1;
}

/******************************************************************************
* pcbmulti_polykeepout
*/
static int pcbmulti_polykeepout()
{
   int oldlayernum = G.cur_layernum;

   G.cur_layernum = -2;
   polycnt = 0;
   cur_netname .Empty();
   loop_command(poly_lst, SIZ_POLY_LST);

   //int lindex = Graph_Level("ALL", "", 0);
   int lindex = accelLayers.getAllLayer().getLayerIndex();
   int err;
   int widthindex;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   write_poly(cur_fillmode, lindex, widthindex, 0, cur_netname, 0, 0);
   polycnt = 0;
   G.cur_layernum = oldlayernum;

   return 1;
}

/******************************************************************************
* pcbmulti_linekeepout
*/
static int pcbmulti_linekeepout()
{
   fprintf(ferr, " lineKeepOut not impl. at %ld\n", ifp_line);
   display_error++;
   fskip();
   return 1;
}

/******************************************************************************
* pcbmulti_infopoint
*/
static int pcbmulti_infopoint()
{
   fskip();
   // loop_command(infopoint_lst,SIZ_INFOPOINT_LST);
   return 1;
}

/******************************************************************************
* pattern_padpinmap
*  padpincnt is advanced on padNum record
*/
static int pattern_padpinmap()
{
   loop_command(padpin_lst, SIZ_PADPIN_LST);

   return 1;
}

/******************************************************************************
* pattern_numpads
*/
static int pattern_numpads()
{
   if (!get_tok())
      return p_error();
   getLastCompDef()->pincnt = atoi(token);
   return 1;
}

/******************************************************************************
* pcbmulti_fromto
*  This is the ratsnets  description. The traces are in layerContents
*/
static int pcbmulti_fromto()
{
   polycnt = 0;
   cur_netname .Empty();
   loop_command(fromto_lst, SIZ_FROMTO_LST);
   polycnt = 0;
   return 1;
}

/******************************************************************************
* pcbmulti_via
*/
static int pcbmulti_via()
{
   int netnr = 0;

   cur_netname .Empty();
   polycnt = 0;
   loop_command(via_lst, SIZ_VIA_LST);
   CString t(cur_padname);
   cur_padname.Format("V:%s", t); //sprintf(cur_padname, "V:%s", t);

   if (polycnt)
   {
      Point2 p = polyarray.ElementAt(0);
      int floatingLayerIndex = accelLayers.getFloatingLayer().getLayerIndex();
      DataStruct *data = Graph_Block_Reference(cur_padname, NULL, file->getFileNumber(), p.x, p.y, DegToRad(0), 0 , 1.0,floatingLayerIndex, TRUE);
      data->getInsert()->setInsertType(insertTypeVia);
      if (!cur_netname.IsEmpty()) //(strlen(cur_netname))
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, cur_netname.GetBuffer(0), attributeUpdateOverwrite, NULL);
   }
   polycnt = 0;
   return 1; 
}

/******************************************************************************
* pcbmulti_pad
*   this are free pads
*/
static int pcbmulti_pad()
{
   int netnr = 0;

   cur_netname .Empty();
   polycnt = 0;
   cur_padrotation = 0;
   cur_pindes = "";
   G.cur_mirror = 0;
   loop_command(pad_lst, SIZ_PAD_LST);

   CString t(cur_padname);
   if (t.CompareNoCase( "(Default)" ) == 0) //(!STRCMPI(t, "(Default)"))
      cur_padname.Format("P:%s", t); //sprintf(cur_padname, "P:%s", t);
   else
      cur_padname = t; //strcpy(cur_padname, t);

   if (polycnt)
   {
		if (convert_freepad == 1)
		{	
			DataStruct *data = NULL;
			CString blockName = "";
			blockName.Format("$FPGeom_%s", cur_padname);
			if (!Graph_Block_Exists(doc, blockName, file->getFileNumber()))
			{
				BlockStruct *block = Graph_Block_On(GBO_APPEND, blockName, file->getFileNumber(), 0L);
				block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
				data = Graph_Block_Reference(cur_padname, "1", 0, 0, 0, 0, 0 , 1.0,accelLayers.getFloatingLayer().getLayerIndex(), TRUE);
				data->getInsert()->setInsertType(insertTypePin);      
				Graph_Block_Off();
			}

			CString refdes = "";
			refdes.Format("FreePad_%d", convert_freepad_count++);
			Point2 p = polyarray.ElementAt(0);
			data = Graph_Block_Reference(blockName, refdes, file->getFileNumber(), p.x, p.y, 
							DegToRad(cur_padrotation), G.cur_mirror, 1.0, accelLayers.getFloatingLayer().getLayerIndex(), TRUE);
			data->getInsert()->setInsertType(insertTypePcbComponent);

			// dummy device
			CString tmp;
			tmp.Format("$DevType_%s", cur_padname);
			TypeStruct *type = AddType(file, tmp);
			if (type->getBlockNumber() < 0)
				type->setBlockNumber( data->getInsert()->getBlockNumber());
			doc->SetAttrib(&data->getAttributesRef(), doc->RegisterKeyWord(ATT_TYPELISTLINK, TRUE, valueTypeString), 
					valueTypeString, tmp.GetBuffer(0), attributeUpdateOverwrite, NULL); 

         if (!cur_netname.IsEmpty()) //(strlen(cur_netname))
			{
				NetStruct *net = add_net(file, cur_netname);
				add_comppin(file, net, refdes, "1");
			}
		}
		else
		{
			Point2 p = polyarray.ElementAt(0);
			DataStruct *data = Graph_Block_Reference(cur_padname, NULL, file->getFileNumber(), p.x, p.y,
					DegToRad(cur_padrotation), G.cur_mirror, 1.0, accelLayers.getFloatingLayer().getLayerIndex(), TRUE);

			if (convert_freepad == 0)
				data->getInsert()->setInsertType(insertTypeFreePad);
			else if (convert_freepad == 2)
				data->getInsert()->setInsertType(insertTypeVia);

         if (!cur_netname.IsEmpty()) //(strlen(cur_netname))
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, cur_netname.GetBuffer(0), attributeUpdateOverwrite, NULL);
		}
   }

   polycnt = 0;
   return 1; 
}

/******************************************************************************
* patternmulti_pad
*/
static int patternmulti_pad()
{
   cur_padname.Empty(); //cur_padname[0] = '\0';
   cur_padrotation = 0;
   polycnt = 0;
   G.cur_mirror = 0;
   cur_pindes = "";

   loop_command(pad_lst, SIZ_PAD_LST);
   Point2 p = polyarray.ElementAt(0);
   load_libpin(cur_pinnum, cur_pindes, cur_padname, p.x, p.y, cur_padrotation, G.cur_mirror, 0);

	ACCELPatterndef* patternDef = getLastPattern();
   patternDef->pads_flipped += G.cur_mirror;

   return 1;
}

/******************************************************************************
* patternmulti_via
*/
static int patternmulti_via()
{
   //
   CString pinname;

   G.cur_pincnt++;
   cur_padname.Empty(); //cur_padname[0] = '\0';

   pinname.Format("VIA_%d", G.cur_pincnt);
   polycnt = 0;
   loop_command(pad_lst, SIZ_PAD_LST);
   Point2 p1 = polyarray.ElementAt(0);
   load_libpin(32000, pinname, cur_padname, p1.x, p1.y, 0, 0, 1); // padtyp == via

   return 1;
}

/******************************************************************************
* pad_padStyleRef
*/
static int pad_padStyleRef()
{
   if (!get_tok())
      return p_error();
//   sprintf(cur_padname, "P:%s", token);
   cur_padname = token; //strcpy(cur_padname, token);

   return 1;
}

/******************************************************************************
* pad_padNum
*/
static int pad_padNum()
{
   if (!get_tok())
      return p_error();
   cur_pinnum = atoi(token);

   return 1;
}

/******************************************************************************
* pad_defaultPinDes
*/
static int pad_defaultPinDes()
{
   if (!get_tok())
      return p_error();
   cur_pindes = token;

   return 1;
}

/******************************************************************************
* pad_rotation
*/
static int pad_rotation()
{
   if (!get_tok())
      return p_error();
   cur_padrotation = atof(token);

   return 1;
}

/******************************************************************************
* tan_line
*  This is the ratsnets  description. The traces are in layerContents
*/
static int tan_line()
{
   int netnr = 0;

   polycnt = 0;
   cur_netname.Empty();
   loop_command(graph_lst, SIZ_GRAPH_LST);

   int widthindex;
   int err;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();
   write_poly(FALSE, lindex, widthindex, graphic_class, cur_netname, 0, 0);

   polycnt = 0;
   return 1;
}

/******************************************************************************
* island_thermal
*/
static int island_thermal()
{
   polycnt = 0;
   loop_command(graph_lst, SIZ_GRAPH_LST);

   int widthindex;
   int err;
   if (G.cur_width == 0)
      widthindex = doc->getZeroWidthIndex();
   else
      widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   int lindex = accelLayers.getLayer(G.cur_layernum).getLayerIndex();
   write_poly(FALSE, lindex, widthindex, graphic_class, cur_netname, 1, 0);
   polycnt = 0;

   return 1;
}

/******************************************************************************
* tan_attr
*/
static int tan_attr()
{
   fskip();
   return 1;
}

/******************************************************************************
* asc_ascversion
*/
static int asc_ascversion()
{
   if (!get_tok())
      return p_error();
   int majv = atoi(token);

   if (!get_tok())
      return p_error();
   int minv = atoi(token);

   if (majv > 3)
   {
      fprintf(ferr, " Warning : Untested Version [%d %d] -> Tested [1 1] [2 1] [3 0]\n", majv, minv);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* asc_program
*/
static int asc_program()
{
	// Get the program name that writes this ascii file, not needed for anything just making sure it is there
   if (!get_tok())
      return p_error();
   if (tok_search(brk_lst, SIZ_BRK_LST) == BRK_B_ROUND)
	   push_tok();

	// Get the program version, not need for anything just making sure it is there
   if (!get_tok())
      return p_error();
   if (tok_search(brk_lst, SIZ_BRK_LST) == BRK_B_ROUND)
		push_tok();

   return 1;
}

/******************************************************************************
* asc_fileunits
*/
static int asc_fileunits()
{
   if (!get_tok())
      return p_error();

   if (token.CompareNoCase("IN") == 0) //(!STRCMPI("IN",token))
      tangounit = UNIT_INCHES;
   else if (token.CompareNoCase("MM") == 0) //(!STRCMPI("MM",token))
      tangounit = UNIT_MM;
   else if (token.CompareNoCase("MIL") == 0) //(!STRCMPI("MIL",token))
      tangounit = UNIT_MILS;
   else
   {
      fprintf(ferr, "Unknown Units [%s]\n", token);
      display_error++;
      return -1;
   }
   return 1;
}

/******************************************************************************
* lib_padstyledef
*/
static int lib_padstyledef()
{
   if (!get_tok())
      return p_error();

   CString origname = token;
 
//   sprintf(cur_padname, "P:%s", token);
   cur_padname = token; //strcpy(cur_padname, token);
   cur_drill = 0;
   cur_isplated = TRUE;
   G.cur_smdpad = 0;

   CAccelPadDefState::getAccelPadDefState().initialize();
   CAccelPadDefState::getAccelPadDefState().setPadstackName(cur_padname);


   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, cur_padname, file->getFileNumber(), 0, blockTypePadstack);
   curblock->setOriginalName(origname);
   loop_command(padstyle_lst, SIZ_PADSTYLE_LST);

   if (cur_drill > 0)
   {
      CString drillname;
      int layernum = accelLayers.getLayer("DRILLHOLE").getLayerIndex();
      int drillindex = get_drillindex(cur_drill, layernum, cur_isplated);  
      
      if (drillindex < 0)
         return 1;

      G.cur_smdpad |= 4;
      drillname.Format("DRILL_%d", drillindex);
      Graph_Block_Reference(drillname, NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
   }  

   CAccelPadDefState::getAccelPadDefState().instantiateImpliedPads((G.cur_smdpad & 1) != 0,(G.cur_smdpad & 2) != 0);

   Graph_Block_Off();
   if (G.cur_smdpad == 1)
      doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, 0), valueTypeNone, NULL, attributeUpdateOverwrite, NULL); 

   return 1;
}

CString getNullPadName()
{
   return "nullPad";
}

int getNullPadApertureIndex()
{
   int err;
   int apertureIndex = Graph_Aperture(getNullPadName(),apertureRound,0.,0.,0.,0.,0.,0,0,false,&err);

   return apertureIndex;
}

/******************************************************************************
* padstyle_padshape
*/
static int padstyle_padshape()
{
   CString apertureName;

   G.cur_layerType = accelLayerTypeNull;
   G.cur_layernum = 0;
   G.cur_width = 0.0;
   G.cur_height = 0.0;

   loop_command(padshape_lst, SIZ_PADSHAPE_LST);

   bool nullPadFlag = false;

   if (G.cur_type == T_UNDEFINED)
   {
      nullPadFlag = true;
   }
   else if (G.cur_type == T_COMPLEX)
   {
      if (polycnt == 0)
      {
         nullPadFlag = true;
      }
      else
      {
         CString shapeGeomName;
         shapeGeomName.Format("PADOUTLINE_%d", ++shapeoutlinecnt);

         BlockStruct *shapeGeom = Graph_Block_On(GBO_APPEND, shapeGeomName, file->getFileNumber(), 0);
         shapeGeom->setBlockType(BLOCKTYPE_PADSHAPE);
         //int lindex = Graph_Level("0", "", 1);
         int lindex = accelLayers.getFloatingLayer().getLayerIndex();
         write_poly(TRUE, lindex, doc->getZeroWidthIndex(), 0, "", 0, 0 );
         Graph_Block_Off();

         apertureName.Format("PADCOMPLEX_%d", shapeoutlinecnt);
         Graph_Complex(apertureName, 0, shapeGeom, 0.0, 0.0, 0.0);
         polycnt = 0;
      }
   }
   else if (G.cur_type == T_ROUNDRECT)
   {
      // If width or height of rectangle is zero then area is zero, skip it.
      if (G.cur_width == 0 || G.cur_height == 0)
      {
         nullPadFlag = true;
      }
      else
      {
         // Non-zero width and height, it's a keeper.

         G.cur_type = T_COMPLEX;

         CString shapeGeomName;
         shapeGeomName.Format("PADOUTLINE_%d", ++shapeoutlinecnt);

         BlockStruct *shapeGeom = Graph_Block_On(GBO_APPEND, shapeGeomName, file->getFileNumber(), 0);
         shapeGeom->setBlockType(BLOCKTYPE_PADSHAPE);
         int lindex = accelLayers.getFloatingLayer().getLayerIndex();
         createRoundedRectPad( lindex, "", 0);
         Graph_Block_Off();

         apertureName.Format("PADCOMPLEX_%d", shapeoutlinecnt);
         Graph_Complex(apertureName, 0, shapeGeom, 0.0, 0.0, 0.0);
         polycnt = 0;
      }
   }
   else
   {
      if (G.cur_width == 0 && G.cur_height == 0)
      {
         nullPadFlag = true;
      }
      else
      {
         int padformIndex = get_padformindex(G.cur_type, G.cur_width, G.cur_height, 0, 0);
         
         int blockNum = padformarray[padformIndex].blockNum;
         BlockStruct *block = doc->getBlockAt(blockNum);
         apertureName = block->getName();
      }
   }

   if (nullPadFlag)
   {
      apertureName = getNullPadName();
      int apertureIndex = getNullPadApertureIndex();
   }

   int layernum = -1;

   if (G.cur_layerType == accelLayerTypeSignal)
   {
      layernum = accelLayers.getSignalLayer().getLayerIndex();

      if (!nullPadFlag)
      {
         G.cur_smdpad |= 3;
      }
   }
   else if (G.cur_layerType == accelLayerTypePlane)
   {
      layernum = accelLayers.getPlaneLayer().getLayerIndex();
   }
   else if (G.cur_layerType == accelLayerTypeNonSignal)
   {
      //layernum = accelLayers.getPlaneLayer().getLayerIndex();
      layernum = -1;
   }
   else
   {
      layernum = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

      if (!nullPadFlag)
      {
         if (G.cur_layernum == 1)
            G.cur_smdpad |= 1;

         if (G.cur_layernum == 2)
            G.cur_smdpad |= 2;
      }
   }

   if (layernum >= 0)
   {
      DataStruct *data = Graph_Block_Reference(apertureName, NULL, file->getFileNumber(),
         0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);

      if (G.cur_layerType == accelLayerTypePlane)
         data->getInsert()->setInsertType(insertTypeThermalPad);

      CAccelPadDefState::getAccelPadDefState().addPad(data);
   }

   return 1;
}

/******************************************************************************
* padstyle_hole
*/
static int padstyle_hole()
{
   cur_drill = tan_unit();
   return 1;
}

/******************************************************************************
* padstyle_holeoffset
*/
static int padstyle_holeoffset()
{
   fprintf(ferr, " Hole Offset not impl. at %ld\n", ifp_line);
   display_error++;
   fskip();
   return 1;
}

/******************************************************************************
* padstyle_localswell
*/
static int padstyle_localswell()
{
   CAccelPadDefState::getAccelPadDefState().setLocalSwell(tan_unit());

   return 1;
}

/******************************************************************************
* padstyle_isplated
*/
static int padstyle_isplated()
{
   if (!get_tok())
      return p_error();
   if (token.CompareNoCase("false") == 0) //(!STRCMPI(token, "false"))
      cur_isplated = FALSE;

   return 1;
}

/******************************************************************************
* padstyle_padshapetype
*/
static int padstyle_padshapetype()
{
   
   if (!get_tok())
      return p_error();

   if (token.CompareNoCase("MtHole") == 0) //(!STRCMPI(token, "MtHole"))
      G.cur_type = T_OBLONG;
   else if (token.CompareNoCase("Rect") == 0) //(!STRCMPI(token, "Rect"))
      G.cur_type = T_RECTANGLE;
   else if (token.CompareNoCase("Oval") == 0) //(!STRCMPI(token, "Oval"))
      G.cur_type = T_OBLONG;
   else if (token.CompareNoCase("RndRect") == 0) //(!STRCMPI(token, "RndRect"))   // round rectangle is rounded, so a square would make errors 
      G.cur_type = T_ROUNDRECT;
   else if (token.CompareNoCase("Ellipse") == 0) //(!STRCMPI(token, "Ellipse"))
      G.cur_type = T_OBLONG;
   else if (token.CompareNoCase("Target") == 0) //(!STRCMPI(token, "Target"))
      G.cur_type = T_TARGET;
   else if (token.Left(4).CompareNoCase("Thrm") == 0) //(!strnicmp(token, "Thrm", 4))
      G.cur_type = T_THERMAL;
   else if (token.CompareNoCase("Polygon") == 0) //(!STRCMPI(token, "Polygon"))
      G.cur_type = T_COMPLEX;
   else if (token.CompareNoCase("Direct") == 0) //(!STRCMPI(token, "Direct"))
      G.cur_type = T_UNDEFINED;
   else if (token.CompareNoCase("NoConnect") == 0) //(!STRCMPI(token, "NoConnect"))
      G.cur_type = T_UNDEFINED;
   else
   {
      G.cur_type = T_UNDEFINED;
#ifdef _DEBUG
      fprintf(ferr,"Unknown padtype [%s]\n",token);
      display_error++;
#endif
   }

   return 1;
}

/******************************************************************************
* lib_viastyledef
*/
static int lib_viastyledef()
{
   if (!get_tok())
      return p_error();

   CString origname = token;
   cur_drill = 0;
   cur_isplated = TRUE;
   cur_padname.Format("V:%s", token); //sprintf(cur_padname, "V:%s", token);   // vias can have the same name as padstyledef;
   G.cur_smdpad = 0;

   CAccelPadDefState::getAccelPadDefState().initialize();
   CAccelPadDefState::getAccelPadDefState().setPadstackName(cur_padname);

   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, cur_padname, file->getFileNumber(), 0, blockTypePadstack);
   curblock->setOriginalName(origname);
   loop_command(padstyle_lst, SIZ_PADSTYLE_LST);

   if (cur_drill > 0)
   {
      CString drillname;
      int layernum = accelLayers.getLayer("DRILLHOLE").getLayerIndex();
      int drillindex = get_drillindex(cur_drill, layernum, cur_isplated);

      if (drillindex < 0)
         return 1;

      G.cur_smdpad |= 4;
      drillname.Format("DRILL_%d", drillindex);
      Graph_Block_Reference(drillname, NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
   }

   CAccelPadDefState::getAccelPadDefState().instantiateImpliedPads((G.cur_smdpad & 1) != 0,(G.cur_smdpad & 2) != 0);

   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* lib_textstyledef
*/
static int lib_textstyledef()
{
   if (!get_tok())
      return p_error();

   CString textstylename(token);
   loop_command(textstyle_lst, SIZ_TEXTSTYLE_LST);

   ACCELTextStyle *c = new ACCELTextStyle;
   textstylearray.SetAtGrow(textstylecnt, c); 
   textstylecnt++;
   c->font = textstylename;
   c->height = G.cur_height;
   c->width  = G.cur_width;

   return 1;
}

/******************************************************************************
* textstyle_font
*/
static int textstyle_font()
{
   loop_command(font_lst, SIZ_FONT_LST);
   return 1;
}

/******************************************************************************
* lib_patterndef
*/
static int lib_patterndef()
{
   if (!get_tok())
      return p_error();

   cur_libname = token;
	cur_originalname = "";
   G.cur_status = STAT_SHAPE;

	BlockStruct *block = Graph_Block_On(GBO_APPEND, cur_libname, file->getFileNumber(), 0L);
	ACCELPatterndef *patternDef = getNewPatternDef(cur_libname, block);	// cur_libname is the pattern name

   loop_command(libpattern_lst, SIZ_LIBPATTERN_LST);

	// Set the original first, then set the graphic name after
	patternDef->original = cur_originalname;
	setLastPatternGraphicAndGeomName("");

   if (G.cur_pincnt)	// if it has no pins, it can not be a PCB Component, but a logo 
      block->setBlockType(BLOCKTYPE_PCBCOMPONENT);

	block->setOriginalName(cur_originalname);
   Graph_Block_Off();
   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* lib_patterndefextended
*/
static int lib_patterndefextended()
{
   if (!get_tok())
      return p_error();

	cur_originalname = "";
   cur_libname = token;
   G.cur_status = STAT_SHAPE;


// Note(Lynn): remove this and create patterndef in libpatternextended_patterngraphicsdef()

 //  G.cur_pincnt = 0;
 //  ACCELPatterndef *patterDef = new ACCELPatterndef;
 //  patterndefarray.SetAtGrow(patterndefcnt, patterDef); 
 //  patterndefcnt++;
 //  patterDef->name = cur_libname;
 //  patterDef->original = cur_libname;
	//patterDef->graphicName = "";
 //  patterDef->number_of_pads = 0;
 //  patterDef->pads_flipped = 0;
 //  patterDef->flip_component = 0;

   loop_command(libpatternextended_lst, SIZ_LIBPATTERNEXTENDED_LST);

	//ACCELPatterndef *patternDef = getLastPattern();
	//patternDef->original = cur_originalname;

	//BlockStruct *block = Graph_Block_On(GBO_APPEND, patternDef->geomName, file->getFileNumber(), 0L);
 //  block->setOriginalName(cur_originalname);

 //  if (G.cur_pincnt) // if it has no pins, it can not be a PCB Component, but a logo 
 //     block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
 //  Graph_Block_Off();

   //if (USE_ORIGINAL_PATTERN)
   //{
   //   // check if the original pattern exist
   //   // if yes, delete the current one
   //   // else make the current the original 
   //   if (Graph_Block_Exists(doc, cur_originalname, file->getFileNumber()))
   //      doc->RemoveBlock(block);   // delete the current one.
   //   else
   //      block->setName(cur_originalname);
   //}

   G.cur_status = STAT_UNKNOWN;

   return 1;
}

/******************************************************************************
* do_rename_pins
*/
static int do_rename_pins(BlockStruct *block)
{
   // all pins must be set to noname and only the padpinmap pins are electrical and get a name
   // an example shows that a patterndef pin 18 was not renamed and a pattern pin 15 was renamed
   // to a pin 18 -- leaving 2 pin 18s .

// Note(Lynn): remove this code

   //DataStruct *data;
   //POSITION dataPos = block->getDataList().GetHeadPosition();
   //while (dataPos)
   //{
   //   data = block->getDataList().GetNext(dataPos);
   //   
   //   if (data->getDataType() == T_INSERT)         
   //   {
   //      if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
   //      {
   //         if (data->getInsert()->getRefname())
   //         {
   //            data->getInsert()->setRefname(NULL);
   //         }
   //         data->getInsert()->setInsertType(insertTypeMechanicalPin);
   //      }
   //   }
   //}

   //for (int i=0; i<cur_padpincnt; i++)
   //{
   //   ACCELPadPin padpin = cur_padpin[i];
   //   DataStruct *data = get_pindata_from_comppinnr(doc, block, padpin.pinnumber);
   //   if (data)
   //   {
   //      data->getInsert()->setRefname(STRDUP(padpin.compPinRef));
   //      data->getInsert()->setInsertType(insertTypePin);
   //   }
   //}

   return 1;
}

/******************************************************************************
* lib_compdef
*/
static int lib_compdef()
{
   if (!get_tok())
      return p_error();
	CString compDefName = token;

	ACCELCompdef* compDef = getNewCompDef(compDefName);	
   curtype = NULL;
	cur_originalname = "";

   loop_command(compdef_lst, SIZ_COMPDEF_LST);
	compDef->original = cur_originalname;
	
	return 1;
}

/******************************************************************************
* net_compinst
*/
static int net_compinst()
{
   if (!get_tok())
      return p_error();	
	CString compInstName = token;

	ACCELCompInst* compInst = getNewCompInst(compInstName);
   loop_command(compinst_lst, SIZ_COMPINST_LST);   // cur_originalname

	if (compInst->height <= 0)
	{
		ACCELCompdef *compDef = findCompDef(compInst->compRef);
		if (compDef != NULL)
			compInst->height = compDef->height;
	}

	if (compInst->value.Trim().IsEmpty())
	{
		ACCELCompdef *compDef = findCompDef(compInst->compRef);
		if (compDef != NULL)
			compInst->value = compDef->value;
	}

	compInst->compRefOriginal = cur_originalname;

   return 1;
}

/******************************************************************************
* padpin_padnum
*/
static int padpin_padnum()
{
   if (!get_tok())
      return p_error();

	ACCELPadPin *padPin = new ACCELPadPin();
	padPin->pinNumber = atoi(token);
	padPin->compPinRef = "";

	ACCELCompdef *compDef = getLastCompDef();
	compDef->padPinArray.SetAtGrow(compDef->padPinArray.GetSize(), padPin);
   
   return 1;
}

/******************************************************************************
* padpin_comppinref
*/
static int padpin_comppinref()
{
   if (!get_tok())
      return p_error();

	ACCELCompdef *compDef = getLastCompDef();
	ACCELPadPin *padPin = compDef->padPinArray.GetAt(compDef->padPinArray.GetSize()-1);
	padPin->compPinRef = token;

   return 1;
}

/****************************************************************************/
/*
*/
static int net_net()
{
   if (!get_tok())   return p_error();
   cur_netname = token;
   loop_command(netnet_lst,SIZ_NETNET_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int netnet_attribute()
{
   if (!get_tok())
      return p_error();  // keyword
   CString key = get_attrmap(token);

   if (!get_tok())
      return p_error();  // value
   CString val = token;
   val.TrimLeft();
   val.TrimRight();

   ACCELAtt *c = new ACCELAtt;
   attarray.SetAtGrow(attcnt,c);  
   attcnt++;
   c->val = val;
   c->key = key;
   c->visible = FALSE;
   c->x = 0;
   c->y = 0;
   c->rotation = 0;
   c->mirror = 0;
   c->justify = 0;
   c->attr_units = -1;

   loop_command(attr_lst,SIZ_ATTR_LST);

   return 1;
}

/****************************************************************************/
/*
*/
static int tan_attribute()
{
   if (!get_tok())
      return p_error();  // keyword
   CString key = get_attrmap(token);

   if (!get_tok())
      return p_error();  // value
   CString val = token;
   val.TrimLeft();
   val.TrimRight();

   if (!key.CompareNoCase("RefDes"))   
      key = ATT_REFNAME;
   else
   if (!key.CompareNoCase("Value")) 
      key = ATT_VALUE;

   ACCELAtt *c = new ACCELAtt;
   attarray.SetAtGrow(attcnt,c);  
   attcnt++;
   c->val = val;
   c->key = key;
   c->visible = FALSE;
   c->x = 0;
   c->y = 0;
   c->rotation = 0;
   c->mirror = 0;
   c->justify = 0;
   c->attr_units = -1;

   loop_command(attr_lst,SIZ_ATTR_LST);

   return 1;
}

/****************************************************************************/
/*
*/
static int pattern_attribute()
{
   if (!get_tok())
      return p_error();  // keyword
   CString key = get_attrmap(token);

   if (!get_tok())
      return p_error();  // value
   CString val = token;
   val.TrimLeft();
   val.TrimRight();

   if (GetAttr)
   {

      ACCELAtt *c = new ACCELAtt;
      attarray.SetAtGrow(attcnt,c);  
      attcnt++;
      c->val = val;
      c->key = key;
      c->visible = FALSE;
      c->x = 0;
      c->y = 0;
      c->rotation = 0;
      c->mirror = 0;
      c->justify = 0;
      c->attr_units = -1;
   }

   loop_command(attr_lst,SIZ_ATTR_LST);
   return 1;
}

/******************************************************************************
* compinst_attr
*/
static int compinst_attr()
{
   if (!get_tok())
      return p_error();  // keyword
   CString key = get_attrmap(token);

   if (!get_tok())
      return p_error();  // value
   CString val = token;
   val.TrimLeft();
   val.TrimRight();   
   fskip();

   if (compheight.CompareNoCase(key) == 0)
   {
		getLastCompInst()->height = cnv_unit(val);
   }
   else if (!STRCMPI(key, ATT_VALUE) && STRCMPI(val, "{Value}"))  
	{
		getLastCompInst()->value = val;
   }

   return 1;
}

/******************************************************************************
* compdef_attr
*/
static int compdef_attr()
{
   if (!get_tok())
      return p_error();  // keyword
   CString key = get_attrmap(token);

   if (!get_tok())
      return p_error();  // value
   CString val = token;
   val.TrimLeft();
   val.TrimRight();
   fskip();

   if (compheight.CompareNoCase(key) == 0)
   {
		getLastCompDef()->height = cnv_unit(val);
   }
   else if (!STRCMPI(key, ATT_VALUE) && STRCMPI(val, "{Value}"))  
	{
		getLastCompDef()->value = val;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int netnet_node()
{   
   if (!get_tok())
		return p_error();
	CString compName = token;

   if (!get_tok())
		return p_error();
	CString compPinRef = token;

   NetStruct *net = add_net(file, cur_netname);
	add_comppin(file, net, compName, compPinRef);
   return 1;
}

/****************************************************************************/
/*
*/
static int netnet_isplane()
{
   if (!get_tok())   return p_error();
fprintf(ferr,"isplane not implemented\n");
display_error++;

#ifdef WOLF
   if (!STRCMPI(token,"TRUE"))
   {
      L_NAttr(cur_netname,NE_PLANE);
   }
#endif
   return 1;
}

/****************************************************************************/
/*
   Careful, this is preatty complicated. - Do not change easily
*/
static int write_text(char *prosa)
{
   int         lay;
   char        *lp;
   int         linecnt = 0;
   double      xoff, yoff;
   int         prop = TRUE;
   int         maxchar = 0;
   
   if (!strlen(prosa))
   {
      polycnt = 0;
      return 1;
   }

   char  *tst = STRDUP(prosa);

   lp = strtok(tst,"\n\r");
   // here cnt the number of lines
   while (lp)
   {
      if ((int)strlen(lp) > maxchar)   maxchar = strlen(lp);
      linecnt++;
      lp = strtok(NULL,"\n\r");
   }

   free(tst);

   // text is lower left corner of a multi line string
   lay = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   lp = strtok(prosa,"\n\r");

   Rotate(0,G.cur_height*1.2,G.cur_rotation,&xoff, &yoff);

   Point2   p1 = polyarray.ElementAt(0);
   double x = p1.x, y = p1.y;
   normalize_text(&x,&y,G.cur_textjust,
                  G.cur_rotation,
                  G.cur_mirror,
                  G.cur_height*linecnt*1.2,     // linespacing is 120 on a 100 text
                  G.cur_height*TEXT_RATIO*0.8*maxchar);

   x += linecnt * xoff + (xoff*0.2);   // start top to bottom 
   y += linecnt * yoff + (yoff*0.2);

   // height correct after the adjustment and line spacing
   G.cur_height *= TEXT_CORRECT; // a 175 textheight is only 130, but text
                                 // height needed for linefeed

   while (lp)
   {
      // normalize
      x -= (xoff + (xoff*0.2));
      y -= (yoff + (yoff*0.2));

      DataStruct *d = Graph_Text(lay, lp, x, y,
               G.cur_height, G.cur_height*TEXT_RATIO, DegToRad(G.cur_rotation),
               0, prop,    // accel has proportional text
               G.cur_mirror,  
               0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg

      lp = strtok(NULL,"\n\r");
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  Poly line
*/
int write_poly(int filled, int layernr, int widthindex, 
               int graphic_class, const char *nname, int thermal, int boundary)
{
   int   i, close = 0;

   if (filled)
   {
      if (polycnt < 3)
      {
         polycnt = 0;
         return 1;
      }
      Point2   p1, p2;
      p1 = polyarray.ElementAt(0);
      p2 = polyarray.ElementAt(polycnt-1);
      if (p1.x != p2.x || p1.y != p2.y)
      {
         polyarray.SetAtGrow(polycnt,p1);
         polycnt++;
      }
      close = TRUE;
   }
   else
   {
      if (polycnt < 2)
      {
         polycnt = 0;
         return 1;
      }

      Point2   p1, p2;
      p1 = polyarray.ElementAt(0);
      p2 = polyarray.ElementAt(polycnt-1);

      close = (p1.x == p2.x && p1.y == p2.y);
   }

   Point2   p1;
   p1 = polyarray.ElementAt(0);

   if (startnew || layernr != oldlayernr ||
       STRCMPI(nname, oldnname) ||  // not equal
       p1.x != oldpolyx ||  p1.y != oldpolyy ||
       close || filled || widthindex != oldwidthindex)
   {

      DataStruct *d = Graph_PolyStruct(layernr,0L,0);
      d->setGraphicClass(graphic_class);

      if (strlen(nname))
      {
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), valueTypeString,
            (void*)nname, attributeUpdateOverwrite, NULL); // x, y, rot, height
      }

      //int   close = (polyline[0].x == polyline[polycnt-1].x && polyline[0].y == polyline[polycnt-1].y);
   
      CPoly *cp = Graph_Poly(NULL, widthindex, filled, 0, close);
      if (thermal)
         cp->setThermalLine(true);

      if (boundary)
         cp->setFloodBoundary(true);

      // here write lines.
      for (i=0;i<polycnt;i++)
      {
         Point2 p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);
      }
   }
   else
   {
      // here write lines.
      for (i=1;i<polycnt;i++) // do not write first one
      {
         Point2   p;
         p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);
      }
   }

   strcpy(oldnname, nname);
   oldlayernr = layernr;
   Point2   p = polyarray.ElementAt(polycnt-1);
   oldpolyx = p.x;
   oldpolyy = p.y;
   oldwidthindex = widthindex;
   startnew = FALSE;

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  Arc
*/
static int write_arc(int widthindex, int graphic_clas,const char *nname)
{
	int lay = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   if (polycnt)                            
   {
      DataStruct *d;
      Point2   p0,p1,p2;
      p0 = polyarray.ElementAt(0);  // center point
      p1 = polyarray.ElementAt(1);  // .x is radius, .y is nothing
      p2 = polyarray.ElementAt(2);  // .x is start angle, .y is delta angle

		// Case 1620 brought us a new arc drawer for "triple point arc" support.
		// Overall code could be simplified if this funcion called that one, as shown
		// in else part of this ifdef.
		// However, the test data on hand does not make any arcs that pass through
		// here, and I don't want to make any change that I can't test.
		// So this shall linger.
#define USE_OLD_ARC_STUFF
#ifdef  USE_OLD_ARC_STUFF
      if (fabs(p2.y - 360) < 1)
      {
         d = Graph_Circle(lay,p0.x,p0.y,p1.x, 
            0L, widthindex , FALSE,FALSE); 
      }
      else
      {
         d = Graph_Arc(lay,
               p0.x,p0.y, // center coords
               p1.x,                          // radius
               DegToRad(p2.x),DegToRad(p2.y),0, // start & delta angle,flag
               widthindex, FALSE); // index of width table
      }
      d->setGraphicClass(graphic_class);
   

      if (strlen(nname))
      {
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
               valueTypeString,
               (void*)nname,
               attributeUpdateOverwrite, NULL); // x, y, rot, height
      }
#else
		write_arc(widthindex, graphic_clas, nname, p0.x,p0.y, p1.x, p2.x, p2.y);
#endif


   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  Arc
*/
static int write_arc(int widthindex, int graphic_clas, const char *nname, double cntrx, double cntry, double radius, double startangle, double deltaangle)
{
   int lay = accelLayers.getLayer(G.cur_layernum).getLayerIndex();

   if (true)                            
   {
      DataStruct *d;

      if (fabs(deltaangle - 360) < 1)
      {
         d = Graph_Circle(lay, cntrx, cntry, radius, 
            0L, widthindex , FALSE,FALSE); 
      }
      else
      {
         d = Graph_Arc(lay,
               cntrx, cntry, // center coords
               radius,                          // radius
               DegToRad(startangle),DegToRad(deltaangle),0, // start & delta angle,flag
               widthindex, FALSE); // index of width table
      }
      d->setGraphicClass(graphic_class);
   

      if (strlen(nname))
      {
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
               valueTypeString,
               (void*)nname,
               attributeUpdateOverwrite, NULL); // x, y, rot, height
      }
   }

   return 1;
}


/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(List *tok_lst,int tok_size)
{
   int      i;

   for (i = 0; i < tok_size; ++i)
      if (token.CompareNoCase(tok_lst[i].token) == 0) //(!STRCMPI(token,tok_lst[i].token))
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
      while (!get_next(cur_line, cur_new))
         if (!get_line(cur_line))
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

static int get_next(CString &linebuf, int newlp)
{
   static   CString local_linebuf;
   static   int bufindx = 0;
   int      token_name = FALSE;
   
   token.Empty();

   if (newlp)  
   {
      local_linebuf = linebuf;
      bufindx = 0;
   }

   for (; isspace(local_linebuf.GetAt(bufindx)) && bufindx < local_linebuf.GetLength(); bufindx++) ;

   switch(local_linebuf.GetAt(bufindx))
   {
      case '\0':
         return 0;
      case '%':
         return 0;
      case '(':
      case ')':
         token += local_linebuf.GetAt(bufindx++);
      break;
      case '\"':
         token_name = TRUE;

         bufindx++; // Swallow opening quote

         //while (*cp != '"')
         while (local_linebuf.GetAt(bufindx) != '"')
         {
            if (local_linebuf.GetAt(bufindx) == '\n')
            {
               // Got to end of line and end quote not yet reached, get next line.
               // The newline is within the quotes, so add it to the token.
               token += (char)('\n');

               if (!get_line(linebuf))
               {
                  fprintf(ferr,"Error in Line Read at %ld\n", ifp_line);
                  return -1;
               }

               local_linebuf = linebuf;
               bufindx = 0;
            }
            else
            {
               if (local_linebuf.GetAt(bufindx) == '\\')
               {
                  // Handle escape chars
                  //++cp;
                  local_linebuf.Delete(bufindx);
                  if (local_linebuf.GetAt(bufindx) == 'n')
                     token += '\n';
                  else if (local_linebuf.GetAt(bufindx) == 'r')
                     token += '\r';
                  else
                     token += local_linebuf.GetAt(bufindx);
               }
               else
               {
                  // Regular non-escape char
                  if (local_linebuf.GetAt(bufindx) != '\0')
                     token += local_linebuf.GetAt(bufindx);
               }

               // Next char
               ++bufindx;

            }         
         }

         ++bufindx; // Swallow closing quote

      break;
      default:
         for (; !isspace(local_linebuf.GetAt(bufindx)) && 
            local_linebuf.GetAt(0) != '\"' &&
            local_linebuf.GetAt(bufindx) != '(' &&
            local_linebuf.GetAt(bufindx) != ')' &&
            local_linebuf.GetAt(bufindx) != '\0' &&
            bufindx < local_linebuf.GetLength(); bufindx++)
         {
            if (local_linebuf.GetAt(bufindx) == '\\')
               bufindx++;
            token += local_linebuf.GetAt(bufindx);
         }
      break;
   }

   if (strlen(token) == 0 && !token_name)
      return(FALSE);

   return(TRUE);
}

/****************************************************************************/
/*
      Like fgets only better, no pain in the neck arbitrary fixed buffers
*/

static bool fgetcs(CString &string, FILE *ifp)
{
   string.Empty();

   char c;
   while ( (c = fgetc(ifp)) != EOF )
   {
      string += c;

      if (c == '\n')
         return true;
   }
 
   // Even apparantly blank lines in the file will have a newline, so we should
   // never get a truly empty string unless we hit EOF.

   return !string.IsEmpty();
}

/****************************************************************************/
/*
   Get a line from the input file.
*/
bool get_line(CString &linebuf)
{
   linebuf.Empty();

   if (!fgetcs(linebuf, ifp))
   {
      if (!feof(ifp))
      {
         fprintf(ferr,"Read error\n");
         return false;
      }
      else
      {
         return false;  // but eof is not an error
      }
   }

   ++ifp_line;

   return true;
}


/****************************************************************************/
/*
   Parsing error.
*/
int p_error()
{
   fprintf(ferr,"Fatal Parsing Error : Token [%s] on line %ld\n",token,ifp_line);
   display_error++;
   return -1;
}

/*****************************************************************************/
/*
*/
//static int get_lay_index(const char *l)
//{
//   int   i;
//
//   for (i=0;i<layer_attr_cnt;i++)
//   {
//      if (!STRCMPI(l,layer_attr[i].name))
//      {
//         return i;
//      }
//   }
//
//   if (layer_attr_cnt < MAX_LAYERS)
//   {
//      layer_attr[layer_attr_cnt].name = l;
//      layer_attr[layer_attr_cnt].mirrorindex = -1;
//      layer_attr[layer_attr_cnt].attr = 0;
//      layer_attr_cnt++;
//   }
//   else
//   {
//      fprintf(ferr,"Too many layers\n");
//      return -1;
//   }
//
//   return layer_attr_cnt-1;
//}

/****************************************************************************/
/*
*/
void init_all_mem()
{
   tangounit = UNIT_MILS;
   shapeoutlinecnt = 0;
   Push_tok = FALSE;
   cur_new = TRUE;
   cur_line.Empty();
   startnew = TRUE;
   //layer_attr_cnt = 0;
	convert_freepad_count = 0;
   
   if ((netlist = (ACCELNetl *) calloc(MAX_NETS, sizeof(ACCELNetl))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   netlistcnt = 0;

   polyarray.SetSize(100,100);
   polycnt = 0;

   textstylearray.SetSize(100,100);
   textstylecnt = 0;

   patterndefarray.SetSize(0,100);
	compdefArray.SetSize(0, 100);
   compinstarray.SetSize(0,100);

   if ((shapename = (ACCELShapeName *)calloc(MAX_SHAPE,sizeof(ACCELShapeName))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   shapenamecnt = 0;

   //accelDefaultLayerArray.empty();
   accelLayers.empty();

   if ((libpin = (ACCELLibpin *)calloc(MAX_LIBPIN, sizeof(ACCELLibpin))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   libpincnt = 0;

   return;
}

/****************************************************************************/
/*
*/
void free_all_mem()
{
   int i, ii;

   for (i=0;i<netlistcnt;i++)
      free(netlist[i].net_name);
   free(netlist);
   free(libpin);

   accelLayers.empty();

   polyarray.RemoveAll();
   polycnt = 0;

   for (i=0;i<textstylecnt;i++)
   {
      delete textstylearray[i];
   }
   textstylearray.RemoveAll();
   textstylecnt = 0;

   for (i=0; i<patterndefarray.GetSize(); i++)
   {
		ACCELPatterndef *patternDef = patterndefarray.GetAt(i);
      delete patternDef;
		patternDef = NULL;
   }
   patterndefarray.RemoveAll();

   for (i=0; i<compinstarray.GetSize(); i++)
   {
		ACCELCompInst *compInst = compinstarray.GetAt(i);
      delete compInst;
		compInst = NULL;
   }
   compinstarray.RemoveAll();

   for (i=0; i<compdefArray.GetSize(); i++)
   {
		ACCELCompdef* compDef = compdefArray[i];

		for (int k=0; k<compDef->padPinArray.GetSize(); k++)
		{
			ACCELPadPin *padPin = compDef->padPinArray.GetAt(k);
			delete padPin;
			padPin = NULL;
		}
		compDef->padPinArray.RemoveAll();
		delete compDef;
		compDef = NULL;
   }
   compdefArray.RemoveAll();


   for (i=0;i<shapenamecnt;i++)
   {
      for (ii=0;ii<shapename[i].pincnt;ii++)
         free(shapename[i].pinname[ii]);
      if (shapename[i].pincnt)
         free(shapename[i].pinname);
      free(shapename[i].name);
   }
   free(shapename);

}

/****************************************************************************/
/*
*/
void elim_ext(char *n)
{
   unsigned int   i;

   for (i=0;i<strlen(n);i++)
   {
      if (n[i] == '.')
      {
         n[i] = '\0';
         break;
      }
   }  
}

/****************************************************************************/
/*
   NULL string will return 0  
*/
double   cnv_unit(const char *s)
{
   if (STRLEN(s) == 0)  return 0.0;

   if (isalpha(s[0]))
   {
      fprintf(ferr,"Unit token [%s] expected at %ld\n",s,ifp_line);
      display_error++;
      return 0.0;
   }

   // if a unit is 0.1234" -> hardcode inch
   if (s[strlen(s)-1] == '"')
   {
      return atof(s) * Units_Factor(UNIT_INCHES, pageunits);   // inch unit
   }

   return atof(s) * Units_Factor(tangounit, pageunits);

}

/****************************************************************************/
/*
   non electrical components are not in the compdef list. 
   these are components without pins
*/
static int get_compdef(const char *p)
{

// Note(Lynn): remove this code

   //int      i;
   //CString  pp;

   //pp.Format("$ %s", p); // this is the compdef name, so that it is guaranteed uniquwe from pattern.
   //                                    // this is cleaned up in the tan_library end of routine.

   //for (i=0;i<compdefcnt;i++)
   //{
   //   if (!STRCMPI(compdef[i].compdef,pp))
   //      return i;
   //}

   return -1;
}

/****************************************************************************/
/*
*/
static int get_compinst(const char *p)
{
   //int   i;

   //for (i=0;i<compinstcnt;i++)
   //{
   //   if (!STRCMPI(compinstarray[i]->compname,p))
   //      return i;
   //}

   //fprintf(ferr,"Can not find pattern [%s] in compinst\n",p);
   //display_error++;

   return -1;
}

/****************************************************************************/
/*
*/
static int write_libpin(int use_pad_mirror)
{
   int   i,pincnt=0;

   if (libpincnt == 0)   return 0;

   /* write sorted if possible */
   sort_libpin();

   for (i=0;i<libpincnt;i++)
   {
      /* check if already used */
      //if (libpin[i].padtype == -1)   continue;
      if (libpin[i].padtype != -1)
      {
         pincnt++;

         if (!use_pad_mirror) libpin[i].mirrored = 0;

         DataStruct *d = Graph_Block_Reference(libpin[i].padstack, libpin[i].pindes, file->getFileNumber(), libpin[i].x, libpin[i].y, 
               DegToRad(libpin[i].rotation), libpin[i].mirrored , 1.0, accelLayers.getFloatingLayer().getLayerIndex(), TRUE);

         if (libpin[i].padtype == 0)
         {
            d->getInsert()->setInsertType(insertTypePin);
            // do pinnr here

            doc->SetAttrib(&d->getAttributesRef(),
                           doc->IsKeyWord(ATT_COMPPINNR, 0),
                           valueTypeInteger,
                           &libpin[i].pinnum,
                           attributeUpdateOverwrite, NULL); // x, y, rot, height
         }
         else
         {
            d->getInsert()->setInsertType(insertTypeVia); // fanout via
         }
         libpin[i].padtype = -1;
      }
   }

   for (i=0;i<libpincnt;i++)
   {
      free(libpin[i].pindes);
      free(libpin[i].padstack);
   }
   libpincnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
void sort_libpin()
{
   ACCELLibpin l;
   int         i, done = FALSE;

   while (!done)
   {
      done = TRUE;
      for (i=1;i<libpincnt;i++)
      {
         if (libpin[i].pinnum < libpin[i-1].pinnum)
         {
            memcpy(&l,&(libpin[i]),sizeof(ACCELLibpin));
            memcpy(&(libpin[i]),&(libpin[i-1]),sizeof(ACCELLibpin));
            memcpy(&(libpin[i-1]),&l,sizeof(ACCELLibpin));
            done = FALSE;
         }
      }
   }
}

/****************************************************************************/
/*
*/
static int load_libpin(int pinnum, const char *pindes, const char *padstack,
            double x,double y,double rot,int mirrored, int typ)
{
   /* create  new */
   if (libpincnt < MAX_LIBPIN)
   {
      libpin[libpincnt].pinnum = pinnum;

      if (strlen(pindes))
      {
         if ((libpin[libpincnt].pindes = STRDUP(pindes)) == NULL)
            MemErrorMessage(__FILE__, __LINE__);
      }
      else
      {
         CString p;
         p.Format("%d", pinnum);
         if ((libpin[libpincnt].pindes = STRDUP(p)) == NULL)
            MemErrorMessage(__FILE__, __LINE__);
      }

      if ((libpin[libpincnt].padstack = STRDUP(padstack)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      libpin[libpincnt].x = x;
      libpin[libpincnt].y = y;
      libpin[libpincnt].padtype = typ;
      libpin[libpincnt].rotation = rot;
      libpin[libpincnt].mirrored = mirrored;
      libpincnt++;
   }
   else
   {
      fprintf(ferr,"Too many pins defined at %ld\n",ifp_line);
      display_error++;
   }
   return 1;
}

/****************************************************************************/
/* 
*/
static int electric_layer(int l)
{
   int retval = FALSE;

   for (int layerIndex = 0;layerIndex < accelLayers.getSize();layerIndex++)
   {
      CAccelLayer* accelLayer = accelLayers.getAt(layerIndex);

      if ( accelLayer != NULL && accelLayer->getNumber() == l)
      {
         retval = (accelLayer->getAccelLayerType() == accelLayerTypeSignal);
         break;
      }
   }

   //int   i;
   //for (i=0;i<layerlistcnt;i++)
   //{
   //   if (layerlist[i].number == l)
   //      return (layerlist[i].type == accelLayerTypeSignal);
   //}
   //return FALSE;

   return retval;
}

/****************************************************************************/
/*
*/
static int get_pinnum(int compdefptr,char *padname)
{
   //int   i;

// Note(Lynn): remove this

   //for (i=0;i<compdef[compdefptr].pincnt;i++)
   //{
   //   if (!STRCMPI(compdef[compdefptr].padpin[i].compPinRef, padname))
   //      return i;
   //}

   //fprintf(ferr,"Pinnumber for [%s] not found\n",padname);
   //display_error++;

   return -1;
}

/****************************************************************************/
/*
*/
static bool test_asciiheader(CString buf)
{
   buf.MakeUpper();

   //if (!strnicmp(l,"TangoPRO_ASCII",strlen("TangoPRO_ASCII"))) return 1;
   if (buf.Find("TANGOPRO_ASCII") > -1) 
      return true;

   //if (!strnicmp(l,"ACCEL_ASCII",strlen("ACCEL_ASCII"))) return 1;
   if (buf.Find("ACCEL_ASCII") > -1)
      return true;

	//if (!strnicmp(l,"PCAD_ASCII",strlen("PCAD_ASCII"))) return 1;
   if (buf.Find("PCAD_ASCII") > -1)
      return true;

   ErrorMessage("ASCII PCB save file expected!", "Error");

   return false;
}

/****************************************************************************/
/*
*/
static int get_physnr(int num)
{
   int retval = 0;

   for (int layerIndex = 0;layerIndex < accelLayers.getSize();layerIndex++)
   {
      CAccelLayer* accelLayer = accelLayers.getAt(layerIndex);

      if ( accelLayer != NULL && accelLayer->getNumber() == num)
      {
         retval = accelLayer->getPhysicalNumber();
         break;
      }
   }

   //int   i;
   //for (i=0;i<layerlistcnt;i++)
   //{
   //   if (layerlist[i].number == l)
   //      return layerlist[i].physnr;
   //}
   //return 0;

   return retval;
}

/****************************************************************************/
/*
*/
static int do_netplane()
{
   int   i;
   int   l;

   for (i=0;i<netlistcnt;i++)
   {

      if (netlist[i].layernumber != 0 )
      {
         NetStruct *n = add_net(file,netlist[i].net_name);

         l = get_physnr(netlist[i].layernumber);

         doc->SetAttrib(&n->getAttributesRef(),
                        doc->IsKeyWord(ATT_POWERNET, 0),
                        valueTypeNone,
                        NULL, 
                        SA_APPEND, NULL); // x, y, rot, height

         int r = doc->IsKeyWord(NETATT_SIGNALSTACK, 0);

         doc->SetAttrib(&n->getAttributesRef(),
                        r,
                        valueTypeInteger,
                        &l, 
                        attributeUpdateOverwrite, 
                        NULL); // x, y, rot, height
      }
   }
   return TRUE;
}

//--------------------------------------------------------------
// Here I need to update the graphic class for libraries, because when
// libraries are defined, I do not know the layer definition yet.
static int ReadSHAPEData(CDataList *DataList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         LayerStruct *l = doc->FindLayer(np->getLayerIndex());
         if (l)
            np->setGraphicClass(get_layerclass(l->getComment())); // here comment, because
                        // the layername is a number and the comment is "Top Silk" ...
      }
      else
      {
         if ( np->getInsert()->getInsertType() != INSERTTYPE_PIN ) 
         {
         
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            // do nothing
            }
            else // not aperture
            {
               ReadSHAPEData(&(block->getDataList()));
            } // end else not aperture
         }
      } // if INSERT

   } // end ReadSHAPEData */

   return 1;
}

/****************************************************************************/
/*
*/
static int  update_libraryclasses()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if ( block != NULL && !(block->getFlags() & BL_WIDTH) && !(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (!(block->getFlags() & BL_FILE) || 
              (block->getFlags() & BL_FILE && doc->is_file_inserted(block->getBlockNumber()))) // fileblock in fileblock is now allowed.
         {
            switch (block->getBlockType())
            {
               case BLOCKTYPE_PCBCOMPONENT:  
               {
                  ReadSHAPEData(&(block->getDataList()));
               }
               break;
               default:
               break;
            }
         }
      }
   } // while
   return 1;
}

/****************************************************************************/
/*
*/
#ifdef OldAccelImplmentation
static int do_layers1()
{
   int         i, layerIndex;
   LayerStruct *layerStruct;

   CWriteFormat& debugWriteFormat = CDebugWriteFormat::getWriteFormat();

   debugWriteFormat.write("____________________________________________________________\n");
   doc->LayerArray.dump(debugWriteFormat,-1);

   debugWriteFormat.write("____________________________________________________________\n");
   accelLayerArray.dump(debugWriteFormat,-1);

   debugWriteFormat.write("____________________________________________________________\n");
   accelDefaultLayerArray.dump(debugWriteFormat,-1);

   // At this point, all LayerStruct layers should have integer string names like "1", "2", "87"
   // The LayerStruct comments contain the Accel layerdef names.
   // In this loop, the LayerStructs are renamed to the values stored in their comments.
   for (i=0;i < doc->getMaxLayerIndex(); i++)
   {
      layerStruct = doc->LayerArray[i];

      if ( layerStruct != NULL )
      {
         CString layerName = layerStruct->getName();

         if (! layerStruct->getComment().IsEmpty())
         {
            layerStruct->setName( layerStruct->getComment());
            layerStruct->setComment("");
         }
      }
   }

   debugWriteFormat.write("After rename ____________________________________________________________\n");
   doc->LayerArray.dump(debugWriteFormat,-1);

   // Accel physical layers are assigned a physical number in this loop
   int physnr = 1;
   CAccelLayer* bottomAccelLayer = NULL;

   for (i=0;i < accelLayerArray.getSize();i++)
   {
      CAccelLayer* accelLayer = accelLayerArray.getAt(i);
      CString accelLayerName = accelLayer->getName();

      if (accelLayer->getAccelLayerType() == accelLayerTypeSignal)
      {
         if (accelLayer->getName().CompareNoCase("TOP") == 0)
         {
            accelLayer->setPhysicalNumber(1);
         }
         else if (accelLayer->getName().CompareNoCase("BOTTOM") == 0)
         {
            bottomAccelLayer = accelLayer;
         }
         else
         {
            accelLayer->setPhysicalNumber(++physnr);
         }
      }
      else if (accelLayer->getAccelLayerType() == accelLayerTypePlane)
      {
         accelLayer->setPhysicalNumber(++physnr);
      }
   }

   if (bottomAccelLayer != NULL)
   {
      bottomAccelLayer->setPhysicalNumber(++physnr);
   }

   debugWriteFormat.write("After physical number ____________________________________________________________\n");
   accelLayerArray.dump(debugWriteFormat,-1);

   // always PAD_1
   // always PAD_2
   //layerIndex = Graph_Level("PAD_1","", 0);
   layerIndex = accelLayers.getLayer("PAD_1").getLayerIndex();
   layerStruct = doc->FindLayer(layerIndex);
   layerStruct->setLayerType(LAYTYPE_PAD_TOP);
   layerStruct->setElectricalStackNumber(1);

   //layerIndex = Graph_Level("PAD_2","", 0);
   layerIndex = accelLayers.getLayer("PAD_2").getLayerIndex();
   layerStruct = doc->FindLayer(layerIndex);
   layerStruct->setLayerType(LAYTYPE_PAD_BOTTOM);
   layerStruct->setElectricalStackNumber(physnr);

   //layerIndex = Graph_Level("SIGNAL","", 0);
   layerIndex = accelLayers.getLayer("SIGNAL").getLayerIndex();
   layerStruct = doc->FindLayer(layerIndex);
   layerStruct->setLayerType(LAYTYPE_PAD_ALL);

   //layerIndex = Graph_Level("PLANE","", 0);
   layerIndex = accelLayers.getLayer("PLANE").getLayerIndex();
   layerStruct = doc->FindLayer(layerIndex);
   layerStruct->setLayerType(LAYTYPE_PLANECLEARANCE);

   //layerIndex = Graph_Level("DRILLHOLE","", 0);
   layerIndex = accelLayers.getLayer("DRILLHOLE").getLayerIndex();
   layerStruct = doc->FindLayer(layerIndex);
   layerStruct->setComment("Drill");
   layerStruct->setLayerType(LAYTYPE_DRILL);

   Graph_Level_Mirror("PAD_1", "PAD_2", "");

   debugWriteFormat.write("After standard layers and mirroring ____________________________________________________________\n");
   doc->LayerArray.dump(debugWriteFormat,-1);

   // pad_signal
   // pad_plane

   // find or create LayerStruct layers with names matching CAccelLayer layers.
   // transfer layer types and physical number from CAccelLayer to LayerStruct
   for (int accelLayerIndex=0;accelLayerIndex < accelLayerArray.getSize();accelLayerIndex++)
   {
      CAccelLayer* accelLayer = accelLayerArray.getAt(accelLayerIndex);
      CString accelLayerName = accelLayer->getName();

      // if (accelLayer->used == FALSE)  continue;      
      if (!accelLayerName.IsEmpty())
      {

         layerIndex = Graph_Level(accelLayerName,"", 0);
         layerStruct = doc->FindLayer(layerIndex);

         if (accelLayer->getAccelLayerType() == accelLayerTypeSignal)
         {
            if (accelLayerName.CompareNoCase("TOP") == 0)
            {
               layerStruct->setLayerType(LAYTYPE_SIGNAL_TOP);
               layerStruct->setElectricalStackNumber(1);
            }
            else if (accelLayerName.CompareNoCase("BOTTOM") == 0)
            {
               layerStruct->setLayerType(LAYTYPE_SIGNAL_BOT);
               layerStruct->setElectricalStackNumber(physnr);
            }
            else
            {
               layerStruct->setLayerType(LAYTYPE_SIGNAL_INNER);
               layerStruct->setElectricalStackNumber(accelLayer->getPhysicalNumber());
            }
         }
         else if (accelLayer->getAccelLayerType() == accelLayerTypePlane)
         {
            layerStruct->setLayerType(LAYTYPE_POWERNEG);
            layerStruct->setElectricalStackNumber(accelLayer->getPhysicalNumber());
         }
         // also need to do layer for PASTE etc...

         // here assign layer
         // PAD_1
         // PAD_2
         // Top is always top
         // Bottom is always bottom
         // all others are intern

         // mirror layers specified in .in file
         CAccelDefaultLayer* accelDefaultLayer = accelDefaultLayerArray.getAt(accelLayer->getName());

         if (accelDefaultLayer->getMirrorIndex() > -1)
         {
            CString defaultLayerName         = accelDefaultLayer->getName();
            CString mirroredDefaultLayerName = accelDefaultLayerArray.getAt(accelDefaultLayer->getMirrorIndex())->getName();
            Graph_Level_Mirror(defaultLayerName,mirroredDefaultLayerName,"");
         }
      }
   }

   debugWriteFormat.write("After set of layer type and electrical stack number ____________________________________________________________\n");
   doc->LayerArray.dump(debugWriteFormat,-1);

   debugWriteFormat.write("____________________________________________________________\n");
   accelDefaultLayerArray.dump(debugWriteFormat,-1);

   // here fix up PAD layers numbers to Names PAD_%d PAD_1 to PAD_TOP

   // match LayerStruct layers to CAccelLayer layers 
   // by matching the CAccelLayer's number to the LayerStruct name (e.g. 1 matches "PAD_1").
   // rename the LayerStruct to "PAD_<CAccelLayer::getName()> 
   for (int layerIndex = 0;layerIndex < doc->getMaxLayerIndex();layerIndex++)
   {
      LayerStruct *layerStruct = doc->LayerArray[layerIndex];

      if ( layerStruct != NULL ) // not been deleted.
      {
         CString ccLayerName = layerStruct->getName();

         for (int accelLayerIndex = 0;accelLayerIndex < accelLayerArray.getSize();accelLayerIndex++)
         {
            CAccelLayer* accelLayer = accelLayerArray.getAt(accelLayerIndex);
            CString accelLayerName = accelLayer->getName();

            if (!accelLayerName.IsEmpty())
            {
               CString  padLayerName;
               padLayerName.Format("PAD_%d",accelLayer->getNumber());

               if (ccLayerName.CompareNoCase(padLayerName) == 0)
               {
                  CString newPadLayerName;
                  newPadLayerName.Format("PAD_%s", accelLayer->getName());

                  layerStruct->setComment(layerStruct->getName());
                  layerStruct->setName(newPadLayerName);

                  // if a LayerStruct exists with the same name as the CAccelLayer,
                  // then copy properties from the old match layer to the renamed layer.
                  LayerStruct* oldMatchLayer;

                  if ((oldMatchLayer = doc->FindLayer_by_Name(accelLayer->getName())) != NULL)
                  {
                     layerStruct->setLayerFlags(oldMatchLayer->getLayerFlags());
                     layerStruct->setElectricalStackNumber(oldMatchLayer->getElectricalStackNumber());
                     layerStruct->setLayerType(oldMatchLayer->getLayerType());
                     layerStruct->setPhysicalStackNumber(oldMatchLayer->getPhysicalStackNumber());
                  }

                  break;
               }
            }
         }
      }
   }

   debugWriteFormat.write("After fixup of PAD_%d  ____________________________________________________________\n");
   doc->LayerArray.dump(debugWriteFormat,-1);

   // now attributes after mirror layers
   for (int layerIndex = 0;layerIndex < doc->getMaxLayerIndex();layerIndex++)
   {
      LayerStruct *layerStruct = doc->LayerArray[layerIndex];

      if ( layerStruct != NULL ) // not been deleted.
      {
         CString ccLayerName = layerStruct->getName();

         for (int accelDefaultLayerIndex = 0;accelDefaultLayerIndex < accelDefaultLayerArray.getSize();accelDefaultLayerIndex++)
         {
            CAccelDefaultLayer* accelDefaultLayer = accelDefaultLayerArray.getAt(accelDefaultLayerIndex);
            CString accelDefaultLayerName = accelDefaultLayer->getName();

            if ( !accelDefaultLayerName.IsEmpty() )
            {
               if (ccLayerName.CompareNoCase(accelDefaultLayerName) == 0)
               {
                  if (accelDefaultLayer->getType() != layerTypeUnknown) // only if the attribute is set
                  {
                     layerStruct->setLayerType(accelDefaultLayer->getType());
                  }
                  break;
               }
            }
         }
      }
   }

   debugWriteFormat.write("Transfer of layertype after mirroring  ____________________________________________________________\n");
   doc->LayerArray.dump(debugWriteFormat,-1);

   return 1;
}
#endif

static int do_layers2()
{
   int layerIndex;
   LayerStruct* layerStruct;

   // this is from component insert attributes.
   if (doc->FindLayer_by_Name("REFDES_1") || doc->FindLayer_by_Name("REFDES_2"))
   {
      //layerIndex = Graph_Level("REFDES_1","", 0);
      layerIndex = accelLayers.getLayer("REFDES_1").getLayerIndex();
      layerStruct = doc->FindLayer(layerIndex);

      if ( layerStruct != NULL )
         layerStruct->setLayerType(LAYTYPE_TOP);

      //layerIndex = Graph_Level("REFDES_2","", 0);
      layerIndex = accelLayers.getLayer("REFDES_2").getLayerIndex();
      layerStruct = doc->FindLayer(layerIndex);

      if ( layerStruct != NULL )
         layerStruct->setLayerType(LAYTYPE_BOTTOM);

      Graph_Level_Mirror("REFDES_1", "REFDES_2", "");
   }

   if (doc->FindLayer_by_Name("VALUE_1") || doc->FindLayer_by_Name("VALUE_2"))
   {
      //layerIndex = Graph_Level("VALUE_1","", 0);
      layerIndex = accelLayers.getLayer("VALUE_1").getLayerIndex();
      layerStruct = doc->FindLayer(layerIndex);

      if ( layerStruct != NULL )
         layerStruct->setLayerType(LAYTYPE_TOP);

      //layerIndex = Graph_Level("VALUE_2","", 0);
      layerIndex = accelLayers.getLayer("VALUE_2").getLayerIndex();
      layerStruct = doc->FindLayer(layerIndex);

      if ( layerStruct != NULL )
         layerStruct->setLayerType(LAYTYPE_BOTTOM);

      Graph_Level_Mirror("VALUE_1", "VALUE_2", "");
   }

   if (doc->FindLayer_by_Name("ATTRIBUTE_1") || doc->FindLayer_by_Name("ATTRIBUTE_2"))
   {
      //layerIndex = Graph_Level("ATTRIBUTE_1","", 0);
      layerIndex = accelLayers.getLayer("ATTRIBUTE_1").getLayerIndex();
      layerStruct = doc->FindLayer(layerIndex);

      if ( layerStruct != NULL )
         layerStruct->setLayerType(LAYTYPE_TOP);

      //layerIndex = Graph_Level("ATTRIBUTE_2","", 0);
      layerIndex = accelLayers.getLayer("ATTRIBUTE_2").getLayerIndex();
      layerStruct = doc->FindLayer(layerIndex);

      if ( layerStruct != NULL )
         layerStruct->setLayerType(LAYTYPE_BOTTOM);

      Graph_Level_Mirror("ATTRIBUTE_1", "ATTRIBUTE_2", "");
   }

   return 1;
}

/******************************************************************************
//* get_layertype
*/
//static LayerTypeTag get_layertype(const char *l)
//{
//   int iVal = LAYTYPE_UNKNOWN;
//
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//      {
//         iVal = i;
//         break;
//      }
//   }
//
//   return intToLayerTypeTag(iVal); 
//}

/******************************************************************************
* getLayerWithType
*/
//static LayerStruct* getLayerWithType(int layerType)
//{
//   LayerStruct* layer = NULL;
//
//   for (int i=0;i < layer_attr_cnt;i++)
//   {
//      Adef& adef = layer_attr[i];
//
//      if (adef.attr == layerType)
//      {
//         layer = doc->getLayer(adef.name);
//         break;
//      }
//   }
//
//   return layer; 
//}

/******************************************************************************
* load_accelsettings
*/
static int load_accelsettings(const CString fname)
{
   char line[255];
   compoutlinecnt = 0;
   boardoutline = "";
   compheight = "";
	convert_freepad = 0;
	do_normalize_bottom_build_geometries = false;

   FILE *fp = fopen(fname, "rt");
   if ((fp ) == NULL)
   {
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "Accel Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      CString w = line;
      char *lp = get_string(line, " \t\n");

      if ( lp != NULL )
      {
         if (lp[0] == '.')
         {
            if (!STRICMP(lp, ".HEIGHTATTR"))
            {
               lp = get_string(NULL, " ;\n");
 
               if ( lp != NULL )
               {
                  compheight = lp;
                  compheight.TrimLeft();
                  compheight.TrimRight();
               }
            }
            else if (!STRICMP(lp, ".ATTRIBMAP"))
            {
               lp = get_string(NULL, " \t\n");

               if ( lp != NULL )
               {
                  CString attr = _strupr(lp);
                  attr.TrimLeft();
                  attr.TrimRight();

                  lp = get_string(NULL, " \t\n");
               
                  if (lp != NULL)
                  {
                     CString ccattr = _strupr(lp);
                     ccattr.TrimLeft();
                     ccattr.TrimRight();

                     ACCELAttrmap *attrMap = new ACCELAttrmap;
                     attrmaparray.SetAtGrow(attrmapcnt, attrMap);  
                     attrmapcnt++;
                     attrMap->accel_name = attr;
                     attrMap->cc_name = ccattr;
                  }
               }
            }
            else if (!STRICMP(lp, ".COMPOUTLINE"))
            {
               if (compoutlinecnt < MAX_COMPOUTLINE)
               {
                  lp = get_string(NULL, " \t\n");
 
                  if (lp != NULL)
                  {
                     compoutline[compoutlinecnt] = lp;
                     compoutline[compoutlinecnt].TrimLeft();
                     compoutline[compoutlinecnt].TrimRight();
                     compoutlinecnt++;
                  }
               }
               else
               {
                  // too many compoutlines
               }
            }
            else if (!STRICMP(lp, ".BOARDOUTLINE"))
            {
               lp = get_string(NULL, " \t\n");
 
               if ( lp!=NULL )
               {
                  boardoutline = lp;
                  boardoutline.TrimLeft();
                  boardoutline.TrimRight();
               }
            }
            else if (!STRICMP(lp, ".LAYERATTR"))
            {
               lp = get_string(NULL, " \t\n");

               if ( lp != NULL )
               {
                  CString accelLayerName(lp);

                  lp = get_string(NULL, " \t\n");

                  if ( lp != NULL )
                  {
                     CString camCadLayerTypeString(lp);

                     accelLayerName.MakeUpper();
                     accelLayerName.Trim();
                     camCadLayerTypeString.Trim();

                     accelLayers.addDefaultLayer(accelLayerName,camCadLayerTypeString);

                     //CString tllayer = _strupr(lp);
                     //tllayer.TrimLeft();
                     //tllayer.TrimRight();

                     //CString cclayer = lp;
                     //cclayer.TrimLeft();
                     //cclayer.TrimRight();

                     //LayerTypeTag laytype = get_layertype(cclayer);
                     //int layindex = accelDefaultLayerArray.getLayerIndex(tllayer);
                     //accelDefaultLayerArray.getAt(layindex)->setType(laytype);
                     ////layer_attr[layindex].attr = laytype;
                  }
               }
            }
            else if (!STRICMP(lp, ".MIRRORLAYER"))
            {
               lp = get_string(NULL, " ;\t\n");

               if ( lp != NULL )
               {
                  CString layerName1(lp);

                  lp = get_string(NULL, " \t\n");

                  if ( lp != NULL )
                  {
                     CString layerName2(lp);

                     accelLayers.addMirroredLayers(layerName1,layerName2);

                     //CString layer1 = _strupr(lp);
                     //layer1.TrimLeft();
                     //layer1.TrimRight();

                     //lp = get_string(NULL, " \t\n");
                     //if (lp == NULL)
                     //   continue; 
                     //CString layer2 = _strupr(lp);
                     //layer2.TrimLeft();
                     //layer2.TrimRight();

                     //int layindex1 = accelDefaultLayerArray.getLayerIndex(layer1);
                     //int layindex2 = accelDefaultLayerArray.getLayerIndex(layer2);
                     //accelDefaultLayerArray.getAt(layindex1)->setMirrorIndex(layindex2);
                     //accelDefaultLayerArray.getAt(layindex2)->setMirrorIndex(layindex1);
                     ////layer_attr[layindex1].mirrorindex = layindex2;
                     ////layer_attr[layindex2].mirrorindex = layindex1;
                  }
               }
            }
            else if (!STRCMPI(lp, ".ComponentSMDrule"))
            {
               lp = get_string(NULL," \t\n");
 
               if ( lp != NULL )
                  ComponentSMDrule = atoi(lp);
            }
   			else if (!STRCMPI(lp, ".CONVERT_FREEPAD"))
	   		{
               lp = get_string(NULL," \t\n");

               if ( lp != NULL )
               {
				      if (toupper(lp[0]) == 'C')
					      convert_freepad = 1;
				      else if (toupper(lp[0]) == 'V')
					      convert_freepad = 2;
               }
			   }
            else if (STRCMPI(lp, ".FIDUCIAL_GEOM") == 0)
            {
               if ((lp = get_string(NULL, " \t\n")) != NULL)
                  update_accelGeomType(lp, FIDUCIAL_GEOM);
            }
            else if (STRCMPI(lp, ".TOOLING_GEOM") == 0)
            {
               if ((lp = get_string(NULL, " \t\n")) != NULL)
                  update_accelGeomType(lp, TOOLING_GEOM);
            }
            else if (!STRICMP(lp, ".NORMALIZE_BOTTOM_BUILD_GEOM"))
            {
               if ((lp = get_string(NULL, " \t\n")) != NULL)
               {
                  if (toupper(lp[0]) == 'Y')
                     do_normalize_bottom_build_geometries = true;
               }
               else if (!STRICMP(lp, ".COPPER_SOURCE"))
               {
                  if ((lp = get_string(NULL, " \t\n")) == NULL)
                     continue;
                  if (!STRICMP(lp, "POUR_BOUNDARY"))
                     read_pour_boundary = true;
               }
            }
         }
       }
   }
   fclose(fp);

   return 1;
}

/******************************************************************************
* getNewPatternDef
*/
static ACCELPatterndef* getNewPatternDef(CString patternName, BlockStruct *geomBlock)
{
	patternName = patternName.Trim();

   ACCELPatterndef *patternDef = new ACCELPatterndef;
   patterndefarray.SetAtGrow(patterndefarray.GetSize(), patternDef); 
	patternDef->patternName = patternName;
	patternDef->graphicName = "";
   patternDef->original = patternName;
   patternDef->number_of_pads = 0;
   patternDef->pads_flipped = 0;
   patternDef->flip_component = 0;
	patternDef->geomBlock = geomBlock;

	// At first set geomName to patterName
	// later when setLastPatternGraphicAndGeomName() is called
	// then reset geomName to the true name use for the geometry(block)
	patternDef->geomName = patternName;

	return patternDef;
}

/******************************************************************************
* setLastPatternGraphicAndGeomName

	If patternName ends with "_1" then geomName is created by original name + "_" + graphicName
	Else geomName is created by patternName + "_" + graphicName
*/
static ACCELPatterndef* setLastPatternGraphicAndGeomName(CString graphicName)
{
	ACCELPatterndef *patternDef = patterndefarray.GetAt(patterndefarray.GetSize() - 1);
	patternDef->graphicName = graphicName;

	if (!graphicName.Trim().IsEmpty())
	{
		if (patternDef->patternName.Right(2) == "_1")
			patternDef->geomName = patternDef->patternName.Left(patternDef->patternName.GetLength() - 2);
		else
			patternDef->geomName = patternDef->patternName;

		// Do not append the graphicName if it is "Primary"
		if (graphicName.CompareNoCase("Primary") != 0)
			patternDef->geomName.AppendFormat("_%s", graphicName);
	}
	else
	{
		if (patternDef->patternName.Right(2) == "_1")
			patternDef->geomName = patternDef->original;
		else
			patternDef->geomName = patternDef->patternName;
	}

	if (patternDef->geomBlock != NULL)
		patternDef->geomBlock->setName(patternDef->geomName);

	return patternDef;
}

/******************************************************************************
* findPatternDef
*/
static ACCELPatterndef* findPatternDef(CString patternName, CString graphicName)
{
	CString geomName = "";
	if (!graphicName.Trim().IsEmpty())
	{
		if (patternName.Right(2) == "_1")
			geomName = patternName.Left(patternName.GetLength() - 2);
		else
			geomName = patternName;

		// Do not append the graphicName if it is "Primary"
		if (graphicName.CompareNoCase("Primary") != 0)
			geomName.AppendFormat("_%s", graphicName);
	}
	else
	{
		if (patternName.Right(2) == "_1")
			geomName = patternName.Left(patternName.GetLength() - 2);
		else
			geomName = patternName;
	}

   for (int i=0; i<patterndefarray.GetSize(); i++)
   {
		ACCELPatterndef *patternDef = patterndefarray[i];
      if (!patternDef->geomName.CompareNoCase(geomName))
         return patternDef;
   }

	return NULL;
}

/******************************************************************************
* findNextPatternDef
*/
static ACCELPatterndef* findNextPatternDef(CString patternName, int &index)
{
	if (index < 0)
		index = 0;

	for (int i=index; i<patterndefarray.GetSize(); i++)
	{
		ACCELPatterndef *patternDef = patterndefarray[i];
      if (!patternDef->patternName.CompareNoCase(patternName))
		{
			index = i + 1;
         return patternDef;
		}
	}

	return NULL;
}

/******************************************************************************
* getLastPattern
*/
static ACCELPatterndef* getLastPattern()
{
	return patterndefarray.GetAt(patterndefarray.GetSize() - 1);
}

/******************************************************************************
* getNewCompDef
*/
static ACCELCompdef* getNewCompDef(CString compDefName)
{

	ACCELCompdef *compDef = new ACCELCompdef();
	compdefArray.SetAtGrow(compdefArray.GetSize(), compDef);

	compDef->compDefName = compDefName;
	compDef->original = "";
	compDef->patternName = "";
	compDef->patternNum = "";
	compDef->height = 0.0;
	compDef->pincnt = 0;
	compDef->value = "";
	compDef->padPinArray.SetSize(0, 20);

	return compDef;
}

/******************************************************************************
* findCompDef
*/
static ACCELCompdef* findCompDef(CString compDefName)
{
	for (int i=0; i<compdefArray.GetSize(); i++)
	{
		ACCELCompdef *compDef = compdefArray.GetAt(i);
		if (!compDef->compDefName.CompareNoCase(compDefName))
			return compDef;
	}

	return NULL;
}

/******************************************************************************
* getLastCompDef
*/
static ACCELCompdef* getLastCompDef()
{
	return compdefArray.GetAt(compdefArray.GetSize() - 1);
}

/******************************************************************************
* findCompPinRefByPinNumber
*/
static CString findCompPinRefByPinNumber(ACCELCompdef *compDef, int pinNumber)
{
	for (int i=0; i<compDef->padPinArray.GetSize(); i++)
	{
		ACCELPadPin *padPin = compDef->padPinArray.GetAt(i);
		if (padPin->pinNumber == pinNumber)
			return padPin->compPinRef;
	}

	return "";
}

/******************************************************************************
* getNewCompInst
*/
static ACCELCompInst* getNewCompInst(CString compInstName)
{
   ACCELCompInst *compInst = new ACCELCompInst;
   compinstarray.SetAtGrow(compinstarray.GetSize(), compInst);
	compInst->compInst = compInstName;
	compInst->compRef = "";
	compInst->compRefOriginal = "";
	compInst->height = 0.0;
	compInst->value = "";

	return compInst;
}

/******************************************************************************
* getLastCompInst
*/
static ACCELCompInst* getLastCompInst()
{
	return compinstarray.GetAt(compinstarray.GetSize() - 1);
}

/******************************************************************************
* findCompInst
*/
static ACCELCompInst* findCompInst(CString compInstName)
{
	for (int i=0; i<compinstarray.GetSize(); i++)
	{
		ACCELCompInst *compInst = compinstarray.GetAt(i);
		if (!compInst->compInst.CollateNoCase(compInstName))
			return compInst;
	}

	return NULL;
}

/******************************************************************************
creates complex shape of Rounded corner pad
*/
int createRoundedRectPad( int layernr, const char *nname, int thermal)
{
   const double padYDiameter( G.cur_height ), padXDiameter( G.cur_width );
   const double cornerRadius( (G.cur_height < G.cur_width) ? G.cur_height/4.0 : G.cur_width/4.0 );
   const double bulge(-0.414214);

   double xloc[9];
   double yloc[9];

   xloc[0] = - ( padXDiameter / 2 );
   yloc[0] = - ( padYDiameter / 2 - cornerRadius );

   xloc[1] = xloc[0];
   yloc[1] = yloc[0] + padYDiameter - 2 * cornerRadius;

   xloc[2] = xloc[1] + cornerRadius;
   yloc[2] = yloc[1] + cornerRadius;

   xloc[3] = xloc[2] + padXDiameter - 2 * cornerRadius;
   yloc[3] = yloc[2];

   xloc[4] = xloc[3] + cornerRadius;
   yloc[4] = yloc[3] - cornerRadius;

   xloc[5] = xloc[4];
   yloc[5] = yloc[4] - padYDiameter + 2 * cornerRadius;

   xloc[6] = xloc[5] - cornerRadius;
   yloc[6] = yloc[5] - cornerRadius;

   xloc[7] = xloc[6] - padXDiameter + 2 * cornerRadius;
   yloc[7] = yloc[6];

   xloc[8] = xloc[7] - cornerRadius;
   yloc[8] = yloc[7] + cornerRadius;

   DataStruct *d = Graph_PolyStruct(layernr,0L,0);
   d->setGraphicClass( GR_CLASS_NORMAL );

   if (strlen(nname))
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING,
         (void*)nname, SA_OVERWRITE, NULL); // x, y, rot, height
   }

   CPoly *cp = Graph_Poly(NULL, doc->getZeroWidthIndex(), TRUE, 0, TRUE);
   if (thermal)
      cp->setThermalLine(true);

   // here write lines.
   Graph_Vertex( xloc[0], yloc[0], 0.0);
   Graph_Vertex( xloc[1], yloc[1], bulge);
   Graph_Vertex( xloc[2], yloc[2], 0.0);
   Graph_Vertex( xloc[3], yloc[3], bulge);
   Graph_Vertex( xloc[4], yloc[4], 0.0);
   Graph_Vertex( xloc[5], yloc[5], bulge);
   Graph_Vertex( xloc[6], yloc[6], 0.0);
   Graph_Vertex( xloc[7], yloc[7], bulge);
   Graph_Vertex( xloc[8], yloc[8], 0.0);

   strcpy(oldnname, nname);
   oldlayernr = layernr;
   startnew = TRUE;

   return 1;
}

AccelLayerTypeTag stringToAccelLayerType(const CString& layerTypeString)
{
   AccelLayerTypeTag layerType = accelLayerTypeNull;

   if (layerTypeString.CompareNoCase("Signal") == 0)
   {
      layerType = accelLayerTypeSignal;
   }
   else if (layerTypeString.CompareNoCase("NonSignal") == 0)
   {
      layerType = accelLayerTypeNonSignal;
   }
   else if (layerTypeString.CompareNoCase("Plane") == 0)
   {
      layerType = accelLayerTypePlane;
   }

   return layerType;
}

CString accelLayerTypeToString(AccelLayerTypeTag layerType)
{
   const char* retval;

   switch (layerType)
   {
   case accelLayerTypeNonSignal:  retval = "accelLayerTypeNonSignal";  break;
   case accelLayerTypeSignal:     retval = "accelLayerTypeSignal";     break;
   case accelLayerTypePlane:      retval = "accelLayerTypePlane";      break;
   default:                       retval = "Undefined";                break;
   }

   return CString(retval);
}

LayerTypeTag accelLayerNameToLayerTypeTag(const CString& layerName)
{
   LayerTypeTag layerType = layerTypeUnknown;

   if (layerName.CompareNoCase("Top") == 0)
   {
      layerType = layerTypePadTop;
   }
   else if (layerName.CompareNoCase("Top Paste") == 0)
   {
      layerType = layerTypePasteTop;
   }
   else if (layerName.CompareNoCase("Top Silk") == 0)
   {
      layerType = layerTypeSilkTop;
   }
   else if (layerName.CompareNoCase("Top Mask") == 0)
   {
      layerType = layerTypeMaskTop;
   }
   else if (layerName.CompareNoCase("Bottom") == 0)
   {
      layerType = layerTypePadBottom;
   }
   else if (layerName.CompareNoCase("Bot Paste") == 0)
   {
      layerType = layerTypePasteBottom;
   }
   else if (layerName.CompareNoCase("Bot Silk") == 0)
   {
      layerType = layerTypeSilkBottom;
   }
   else if (layerName.CompareNoCase("Bot Mask") == 0)
   {
      layerType = layerTypeMaskBottom;
   }

   return layerType;
}

//_____________________________________________________________________________
CAccelLayer::CAccelLayer(int id)
{
   m_id     = id;

   m_number = 0;
   m_accelLayerType   = accelLayerTypeNonSignal;
   m_used             = false;
   m_physicalNumber   = 0;
   m_camCadLayer      = NULL;
}

void CAccelLayer::setAccelLayerType(AccelLayerTypeTag layerType) 
{ 
   m_accelLayerType = layerType; 
}

void CAccelLayer::setAccelLayerType(const CString& layerTypeString) 
{ 
   if (layerTypeString.CompareNoCase("NonSignal") == 0)
   {
      m_accelLayerType = accelLayerTypeNonSignal;
   }
   else if (layerTypeString.CompareNoCase("Signal") == 0)
   {
      m_accelLayerType = accelLayerTypeSignal;
   }
   else if (layerTypeString.CompareNoCase("Plane") == 0)
   {
      m_accelLayerType = accelLayerTypePlane;
   }
   else
   {
      m_accelLayerType = accelLayerTypeNonSignal;
   }
}

void CAccelLayer::setNetName(const CString& netName) 
{ 
	m_netName = netName; 

	WORD attrNetNameKey = doc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
   m_camCadLayer->setAttrib(doc->getCamCadData(), attrNetNameKey, valueTypeString, (void *)m_netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
}

void CAccelLayer::setName(const CString& name) 
{ 
   m_name = name; 

   if (getLayerType() == layerTypeUnknown)
   {
      setLayerType(accelLayerNameToLayerTypeTag(name));
   }
}

LayerStruct& CAccelLayer::getDefinedCamCadLayer() const
{
   if (m_camCadLayer == NULL)
   {
      int layerIndex = Graph_Level(m_name,"",false);
      m_camCadLayer = doc->FindLayer(layerIndex);
   }

   return *m_camCadLayer;
}

void CAccelLayer::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CAccelLayer\n"
"{\n"
"   m_id=%d\n"
"   m_number=%d\n"
"   m_used=%d\n"
"   m_physicalNumber=%d\n"
"   m_name='%s'\n"
"   m_type='%s'\n",
m_id,
m_number,
m_used,
m_physicalNumber,
m_name,
accelLayerTypeToString(m_accelLayerType)
);


   //if (depth != 0)
   //{
   //   writeFormat.pushHeader("   ");

   //   writeFormat.popHeader();
   //}

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CAccelLayerArray::CAccelLayerArray()
{
   m_layers.SetSize(0,100);
}

CAccelLayer* CAccelLayerArray::getAt(int layerIndex)
{
   CAccelLayer* accelLayer = NULL;

   if (layerIndex >= 0 && layerIndex < m_layers.GetSize())
   {
      accelLayer = m_layers.GetAt(layerIndex);
   }

   return accelLayer;
}

CAccelLayer* CAccelLayerArray::add()
{
   CAccelLayer* accelLayer = new CAccelLayer(m_layers.GetSize());

   m_layers.Add(accelLayer);

   return accelLayer;
}

void CAccelLayerArray::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CAccelLayerArray\n"
"{\n"
"   Count=%d\n",
m_layers.GetSize());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (int layerIndex = 0;layerIndex < m_layers.GetSize();layerIndex++)
      {
         CAccelLayer* layer = m_layers.GetAt(layerIndex);

         if (layer != NULL)
         {
            layer->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CAccelLayers::CAccelLayers()
{
   m_defaultLayerMap.InitHashTable(nextPrime2n(50));
   m_mirroredLayerMap.InitHashTable(nextPrime2n(50));

   empty();
}

CAccelLayers::~CAccelLayers()
{
   releaseAccelLayerArray();
   releaseAccelLayerMap();
   empty();
}

void CAccelLayers::empty()
{
   m_accelLayers.empty();

   m_currentAccelLayer = NULL;
   m_allLayer          = NULL;
   m_floatingLayer     = NULL;
   m_signalLayer       = NULL;
   m_planeLayer        = NULL;
   m_pasteTopLayer     = NULL;
   m_pasteBottomLayer  = NULL;
   m_maskTopLayer      = NULL;
   m_maskBottomLayer   = NULL;
   m_accelLayerArray   = NULL;
   m_accelLayerMap     = NULL;
}

void CAccelLayers::addDefaultLayer(const CString& accelLayerName,const CString& layerTypeString)
{
   m_defaultLayerMap.SetAt(accelLayerName,layerTypeString);
}

void CAccelLayers::addMirroredLayers(CString layerName1,CString layerName2)
{
   layerName1.MakeUpper();
   layerName2.MakeUpper();

   m_mirroredLayerMap.SetAt(layerName1,layerName2);
}

CAccelLayer& CAccelLayers::getCurrentAccelLayer()
{
   if (m_currentAccelLayer == NULL)
   {
      releaseAccelLayerArray();
      releaseAccelLayerMap();

      m_currentAccelLayer = m_accelLayers.add();
   }

   return *m_currentAccelLayer;
}

void CAccelLayers::addAccelLayer(const CString& accelLayerName)
{
   releaseAccelLayerArray();
   releaseAccelLayerMap();

   if (accelLayerName.IsEmpty())
   {
      int iii = 3;
   }

   m_currentAccelLayer = m_accelLayers.add();
   m_currentAccelLayer->setName(accelLayerName);
}

void CAccelLayers::syncAccelLayerArray()
{
   if (m_accelLayerArray == NULL)
   {
      m_accelLayerArray = new CTypedPtrArray<CPtrArray,CAccelLayer*>();
      m_accelLayerArray->SetSize(0,50);

      for (int layerIndex = 0;layerIndex < m_accelLayers.getSize();layerIndex++)
      {
         CAccelLayer* accelLayer = m_accelLayers.getAt(layerIndex);

         if ( accelLayer != NULL )
         {
            m_accelLayerArray->SetAtGrow(accelLayer->getNumber(),accelLayer);
         }
      }
   }
}

void CAccelLayers::syncAccelLayerMap()
{
   if (m_accelLayerMap == NULL)
   {
      m_accelLayerMap = new CTypedPtrMap<CMapStringToPtr,CString,CAccelLayer*>();
      m_accelLayerMap->InitHashTable(nextPrime2n(100));

      for (int layerIndex = 0;layerIndex < m_accelLayers.getSize();layerIndex++)
      {
         CAccelLayer* accelLayer = m_accelLayers.getAt(layerIndex);

         if ( accelLayer != NULL )
         {
            m_accelLayerMap->SetAt(accelLayer->getName(),accelLayer);
         }
      }
   }
}

void CAccelLayers::releaseAccelLayerArray()
{
   delete m_accelLayerArray;
   m_accelLayerArray = NULL;
}

void CAccelLayers::releaseAccelLayerMap()
{
   delete m_accelLayerMap;
   m_accelLayerMap = NULL;
}

LayerStruct& CAccelLayers::getLayer(int layerNumber)
{
   syncAccelLayerArray();
   syncAccelLayerMap();

   CAccelLayer* accelLayer = NULL;

   if (layerNumber < 0) layerNumber = 0;

   if (layerNumber < m_accelLayerArray->GetSize())
   {
      accelLayer = m_accelLayerArray->GetAt(layerNumber);
   }

   if (accelLayer == NULL)
   {
      CString name;
      name.Format("Unnamed_%d",layerNumber);
      accelLayer = m_accelLayers.add();
      accelLayer->setNumber(layerNumber);
      accelLayer->setName(name);
      m_accelLayerArray->SetAtGrow(layerNumber,accelLayer);
      m_accelLayerMap->SetAt(name,accelLayer);
   }

   return accelLayer->getDefinedCamCadLayer();
}

LayerStruct& CAccelLayers::getLayer(const CString& layerName)
{
   syncAccelLayerArray();
   syncAccelLayerMap();

   CAccelLayer* accelLayer = NULL;

   if (! m_accelLayerMap->Lookup(layerName,accelLayer))
   {
      accelLayer = NULL;
   }

   if (accelLayer == NULL)
   {
      int layerNumber = m_accelLayerArray->GetSize();

      accelLayer = m_accelLayers.add();
      accelLayer->setNumber(layerNumber);
      accelLayer->setName(layerName);
      m_accelLayerArray->SetAtGrow(layerNumber,accelLayer);
      m_accelLayerMap->SetAt(layerName,accelLayer);
   }

   return accelLayer->getDefinedCamCadLayer();
}

LayerStruct& CAccelLayers::getAllLayer()
{
   if (m_allLayer == NULL)
   {
      m_allLayer = &(getLayer("ALL"));
   }

   return *m_allLayer;
}

LayerStruct& CAccelLayers::getFloatingLayer()
{
   if (m_floatingLayer == NULL)
   {
      m_floatingLayer = &(getLayer("0"));
   }

   return *m_floatingLayer;
}

LayerStruct& CAccelLayers::getSignalLayer()
{
   if (m_signalLayer == NULL)
   {
      m_signalLayer = &(getLayer("SIGNAL"));
      m_signalLayer->setLayerType(layerTypeSignalAll);
   }

   return *m_signalLayer;
}

LayerStruct& CAccelLayers::getPlaneLayer()
{
   if (m_planeLayer == NULL)
   {
      m_planeLayer = &(getLayer("PLANE"));
      m_planeLayer->setLayerType(layerTypePowerNegative);
   }

   return *m_planeLayer;
}

LayerStruct& CAccelLayers::getPasteTopLayer()
{
   if (m_pasteTopLayer == NULL)
   {
      m_pasteTopLayer = &(getLayer("TOP PASTE"));
   }

   return *m_pasteTopLayer;
}

LayerStruct& CAccelLayers::getPasteBottomLayer()
{
   if (m_pasteBottomLayer == NULL)
   {
      m_pasteBottomLayer = &(getLayer("BOT PASTE"));
   }

   return *m_pasteBottomLayer;
}

LayerStruct& CAccelLayers::getMaskTopLayer()
{
   if (m_maskTopLayer == NULL)
   {
      m_maskTopLayer = &(getLayer("TOP MASK"));
   }

   return *m_maskTopLayer;
}

LayerStruct& CAccelLayers::getMaskBottomLayer()
{
   if (m_maskBottomLayer == NULL)
   {
      m_maskBottomLayer = &(getLayer("BOT MASK"));
   }

   return *m_maskBottomLayer;
}

void CAccelLayers::instantiateCamCadLayers(CCEtoODBDoc& camCadDoc)
{
   // implement layer typing from .in file and from accel layer type
   CString layerTypesString;

   for (int layerIndex = 0;layerIndex < m_accelLayers.getSize();layerIndex++)
   {
      CAccelLayer* accelLayer = m_accelLayers.getAt(layerIndex);

      if ( accelLayer != NULL )
      {
         if ( m_defaultLayerMap.Lookup(accelLayer->getName(),layerTypesString))
         {
            LayerTypeTag layerType = stringToLayerTypeTag(layerTypesString);

            if (layerType != layerTypeUnknown)
            {
               accelLayer->setLayerType(layerType);
            }
         }

         if (accelLayer->getLayerType() == layerTypeUnknown)
         {
            if (accelLayer->getAccelLayerType() == accelLayerTypeSignal)
            {
               accelLayer->setLayerType(layerTypeSignalInner);
            }
            else if (accelLayer->getAccelLayerType() == accelLayerTypePlane)
            {
               accelLayer->setLayerType(layerTypePowerNegative);
            }
            else if (accelLayer->getName().CompareNoCase("DrillHole") == 0)
            {
               accelLayer->setLayerType(layerTypeDrill);
            }
            else if (accelLayer->getName().CompareNoCase("Plane") == 0)
            {
               accelLayer->setLayerType(layerTypePowerNegative);
            }
         }
      }
   }

   // determine electrical layer numbering
   int physicalLayerNumber = 1;
   CAccelLayer* bottomAccelLayer = NULL;

   for (int layerIndex = 0;layerIndex < m_accelLayers.getSize();layerIndex++)
   {
      CAccelLayer* accelLayer = m_accelLayers.getAt(layerIndex);

      if ( accelLayer != NULL )
      {
         if (accelLayer->getAccelLayerType() == accelLayerTypeSignal)
         {
            if (accelLayer->getName().CompareNoCase("TOP") == 0)
            {
               accelLayer->setPhysicalNumber(1);
            }
            else if (accelLayer->getName().CompareNoCase("BOTTOM") == 0)
            {
               bottomAccelLayer = accelLayer;
            }
            else
            {
               accelLayer->setPhysicalNumber(++physicalLayerNumber);
            }
         }
         else if (accelLayer->getAccelLayerType() == accelLayerTypePlane)
         {
            accelLayer->setPhysicalNumber(++physicalLayerNumber);
         }
      }
   }

   if (bottomAccelLayer != NULL)
   {
      bottomAccelLayer->setPhysicalNumber(++physicalLayerNumber);
   }

   // instantiate layers
   for (int layerIndex = 0;layerIndex < m_accelLayers.getSize();layerIndex++)
   {
      CAccelLayer* accelLayer = m_accelLayers.getAt(layerIndex);

      if ( accelLayer != NULL )
      {
         LayerStruct& camCadLayer = accelLayer->getDefinedCamCadLayer();

         camCadLayer.setLayerType(accelLayer->getLayerType());
         camCadLayer.setElectricalStackNumber(accelLayer->getPhysicalNumber());
      }
   }

   // mirror layers
   CString mirroredLayerName;
   CAccelLayer* topLayer    = NULL;
   CAccelLayer* bottomLayer = NULL;

   for (int layerIndex = 0;layerIndex < m_accelLayers.getSize();layerIndex++)
   {
      CAccelLayer* accelLayer = m_accelLayers.getAt(layerIndex);
      CString accelLayerName;

      if( accelLayer != NULL )
      {
         accelLayerName = accelLayer->getName();
         accelLayerName.MakeUpper();
      }

      if (accelLayerName == "TOP"   ) topLayer    = accelLayer;
      if (accelLayerName == "BOTTOM") bottomLayer = accelLayer;

      if (m_mirroredLayerMap.Lookup(accelLayerName,mirroredLayerName))
      {
         Graph_Level_Mirror(accelLayerName,mirroredLayerName,"");
      }
   }

   if (topLayer != NULL && bottomLayer != NULL)
   {
      Graph_Level_Mirror(topLayer->getName(),bottomLayer->getName(),"");
   }
}

////_____________________________________________________________________________
//CAccelDefaultLayer::CAccelDefaultLayer(int layerIndex)
//{
//   m_layerIndex = layerIndex;
//
//   m_mirrorIndex = -1;
//   m_type        = layerTypeUnknown;
//}
//
//void CAccelDefaultLayer::setType(LayerTypeTag type) 
//{ 
//   m_type = intToLayerTypeTag(type); 
//
//   if (m_type != layerTypeUndefined && m_type != layerTypeUnknown)
//   {
//      int iii = 3;
//   }
//}
//
//void CAccelDefaultLayer::dump(CWriteFormat& writeFormat,int depth) const
//{
//   depth--;
//
//   writeFormat.writef(
//"CAccelDefaultLayer\n"
//"{\n"
//"   m_layerIndex=%d\n"
//"   m_mirrorIndex=%d\n"
//"   m_name='%s'\n"
//"   m_type='%s'\n",
//m_layerIndex,
//m_mirrorIndex,
//m_name,
//layerTypeToString(m_type)
//);
//
//
//   //if (depth != 0)
//   //{
//   //   writeFormat.pushHeader("   ");
//
//   //   writeFormat.popHeader();
//   //}
//
//   writeFormat.writef("}\n");
//}
//
////_____________________________________________________________________________
//CAccelDefaultLayerArray::CAccelDefaultLayerArray()
//{
//   m_layers.SetSize(0,100);
//}
//
//CAccelDefaultLayer* CAccelDefaultLayerArray::getAt(int layerIndex)
//{
//   CAccelDefaultLayer* accelDefaultLayer = NULL;
//
//   if (layerIndex >= 0 && layerIndex < m_layers.GetSize())
//   {
//      accelDefaultLayer = m_layers.GetAt(layerIndex);
//   }
//
//   return accelDefaultLayer;
//}
//
//CAccelDefaultLayer* CAccelDefaultLayerArray::getAt(const char* layerName)
//{
//   CAccelDefaultLayer* accelDefaultLayer = NULL;
//
//   for (int layerIndex = 0;layerIndex < m_layers.GetSize();layerIndex++)
//   {
//      accelDefaultLayer = m_layers.GetAt(layerIndex);
//
//      if (accelDefaultLayer->getName().CompareNoCase(layerName) == 0)
//      {
//         break;
//      }
//
//      accelDefaultLayer = NULL;
//   }
//
//   if (accelDefaultLayer == NULL)
//   {
//      accelDefaultLayer = add();
//      accelDefaultLayer->setName(layerName);
//      accelDefaultLayer->setMirrorIndex(-1);
//      accelDefaultLayer->setType(layerTypeUnknown);
//   }
//
//
//   if (layerIndex >= 0 && layerIndex < m_layers.GetSize())
//   {
//      accelDefaultLayer = m_layers.GetAt(layerIndex);
//   }
//
//   return accelDefaultLayer;
//}
//
//LayerStruct* CAccelDefaultLayerArray::getLayerWithType(LayerTypeTag layerType)
//{
//   LayerStruct* layer = NULL;
//   CAccelDefaultLayer* accelDefaultLayer = NULL;
//
//   for (int layerIndex = 0;layerIndex < m_layers.GetSize();layerIndex++)
//   {
//      accelDefaultLayer = m_layers.GetAt(layerIndex);
//
//      if (accelDefaultLayer->getType() == layerType)
//      {
//         layer = doc->getLayer(accelDefaultLayer->getName());
//         break;
//      }
//   }
//
//   return layer; 
//}
//
//CAccelDefaultLayer* CAccelDefaultLayerArray::add()
//{
//   CAccelDefaultLayer* accelDefaultLayer = new CAccelDefaultLayer(m_layers.GetSize());
//
//   m_layers.Add(accelDefaultLayer);
//
//   return accelDefaultLayer;
//}
//
//int CAccelDefaultLayerArray::getLayerIndex(const char* layerName)
//{
//   CAccelDefaultLayer* accelDefaultLayer = getAt(layerName);
//
//   return accelDefaultLayer->getLayerIndex();
//}
//
//void CAccelDefaultLayerArray::dump(CWriteFormat& writeFormat,int depth) const
//{
//   depth--;
//
//   writeFormat.writef(
//"CAccelDefaultLayerArray\n"
//"{\n"
//"   Count=%d\n",
//m_layers.GetSize());
//
//   if (depth != 0)
//   {
//      writeFormat.pushHeader("   ");
//
//      for (int layerIndex = 0;layerIndex < m_layers.GetSize();layerIndex++)
//      {
//         CAccelDefaultLayer* layer = m_layers.GetAt(layerIndex);
//
//         if (layer != NULL)
//         {
//            layer->dump(writeFormat,depth);
//         }
//      }
//
//      writeFormat.popHeader();
//   }
//
//   writeFormat.writef("}\n");
//}

//_____________________________________________________________________________
CAccelPadDefState* CAccelPadDefState::m_accelPadDefState = NULL;

CAccelPadDefState& CAccelPadDefState::getAccelPadDefState()
{
   if (m_accelPadDefState == NULL)
   {
      m_accelPadDefState = new CAccelPadDefState();
   }

   return *m_accelPadDefState;
}

void CAccelPadDefState::releaseAccelPadDefState()
{
   delete m_accelPadDefState;
   m_accelPadDefState = NULL;
}

CAccelPadDefState::CAccelPadDefState() :
   m_pads(false)
{
   m_pasteSwell   = 0.;
   m_solderSwell  = 0.;
   m_planeSwell   = 0.;
   m_localSwell   = 0.;
	m_pads.RemoveAll();
}

void CAccelPadDefState::initialize()
{
   m_localSwell = 0.;
	m_pads.RemoveAll();
}

void CAccelPadDefState::setPasteSwell(double swell)
{
   m_pasteSwell = swell;
}

void CAccelPadDefState::setSolderSwell(double swell)
{
   m_solderSwell = swell;
}

double CAccelPadDefState::getPlaneSwell()
{
   double planeSwell = m_planeSwell;

   if (m_localSwell != 0.)
   {
      planeSwell = m_localSwell;
   }

   return planeSwell;
}

void CAccelPadDefState::setPlaneSwell(double swell)
{
   m_planeSwell = swell;
}

void CAccelPadDefState::setLocalSwell(double swell)
{
   m_localSwell = swell;
}

void CAccelPadDefState::addPad(DataStruct* pad)
{
   m_pads.AddTail(pad);
}

BlockStruct* CAccelPadDefState::getApertureBlock(CStandardAperture& padAperture)
{
   CString apertureName = "AP_" + padAperture.getDescriptor();

   BlockStruct* apertureBlock = doc->Find_Block_by_Name(apertureName,file->getFileNumber());

   if (apertureBlock == NULL)
   {
      int err;

      int apertureIndex = doc->Graph_Aperture(apertureName,padAperture.getApertureShape(),
         padAperture.getDimension(0),padAperture.getDimension(1),
         0.,0.,0.,0,0,false,&err);

      apertureBlock = doc->getWidthTable().GetAt(apertureIndex);
   }

   return apertureBlock;
}

void CAccelPadDefState::instantiateImpliedPads(bool topPadFlag,bool bottomPadFlag)
{
   if (m_padstackName.CompareNoCase("THRU_042/024_RD(P)") == 0)
   {
      int iii = 3;
   }

   DataStruct* pastePadTop      = NULL;
   DataStruct* pastePadBottom   = NULL;
   DataStruct* maskPadTop       = NULL;
   DataStruct* maskPadBottom    = NULL;
   DataStruct* planePad         = NULL;
   DataStruct* maxElectricalPad = NULL;
   double maxElectricalPadArea  = 0.;

   // scan for implied pads
   for (POSITION pos = m_pads.GetHeadPosition();pos != NULL;)
   {
      DataStruct* pad = m_pads.GetNext(pos);
      InsertStruct* padInsert = pad->getInsert();
      int layerIndex = pad->getLayerIndex();

      if (layerIndex >= 0)
      {
         LayerStruct* layer = doc->getLayerAt(layerIndex);
			
         if ( layer )
         {
            switch (layer->getLayerType())
            {
            case layerTypePasteTop:        pastePadTop    = pad;  break;
            case layerTypePasteBottom:     pastePadBottom = pad;  break;
            case layerTypeMaskTop:         maskPadTop     = pad;  break;
            case layerTypeMaskBottom:      maskPadBottom  = pad;  break;
            case layerTypePowerNegative:  planePad       = pad;  break;
            }

            if (layer->getElectricalStackNumber() > 0)
            {
               BlockStruct* apertureBlock = doc->getBlockAt(padInsert->getBlockNumber());
               CStandardAperture padAperture(apertureBlock,doc->getPageUnits());

               double padArea = padAperture.getArea();

               if (maxElectricalPad == NULL || padArea > maxElectricalPadArea)
               {
                  maxElectricalPad     = pad;
                  maxElectricalPadArea = padArea;
               }
            }
         }
      }
   }

   // remove null pads
   int nullPadBlockNumber = doc->getWidthTable().GetAt(getNullPadApertureIndex())->getBlockNumber();
   CDataList* currentDataList = GetCurrentDataList();

   for (POSITION pos = currentDataList->GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      DataStruct* pad = currentDataList->GetNext(pos);

      if (pad->getDataType() == dataTypeInsert)
      {
         InsertStruct* padInsert = pad->getInsert();

         if (padInsert->getBlockNumber() == nullPadBlockNumber)
         {
            RemoveOneEntityFromDataList(doc,currentDataList,pad,oldPos);

				if (maxElectricalPad == pad)
					maxElectricalPad = NULL;
         }
      }
   }

   // add needed implied pads
   if (maxElectricalPad != NULL && maxElectricalPad->getInsert() != NULL)
   {
      InsertStruct* maxElectricalPadInsert = maxElectricalPad->getInsert();
      BlockStruct* maxElectricalPadApertureBlock = doc->getBlockAt(maxElectricalPadInsert->getBlockNumber());
      CStandardAperture maxElectricalPadAperture(maxElectricalPadApertureBlock,doc->getPageUnits());

      bool needPasteTop    = (topPadFlag    && !bottomPadFlag && pastePadTop    == NULL);
      bool needPasteBottom = (bottomPadFlag && !topPadFlag    && pastePadBottom == NULL);
      bool needMaskTop     = (topPadFlag                      && maskPadTop     == NULL);
      bool needMaskBottom  = (bottomPadFlag                   && maskPadBottom  == NULL);
      bool needPlane       = (topPadFlag    &&  bottomPadFlag && planePad       == NULL);

      if (maxElectricalPadAperture.isStandardAperture())
      {
         bool isComlexAperture = maxElectricalPadApertureBlock?( maxElectricalPadApertureBlock->getShape() == apertureComplex ):false;
         double pasteSwell( getPasteSwell() );
         double maskSwell( getSolderSwell() );
         double planeSwell( getPlaneSwell() );
         CString pasteComplexBlockName(""), maskComplexBlockName("");

         int complexShapeBlockNum(-1);
         if( isComlexAperture )
         {
            complexShapeBlockNum = static_cast<int>( maxElectricalPadApertureBlock->getSizeA() );
         }

         if (needPasteTop || needPasteBottom)
         {
            bool noPasteSwell( pasteSwell < swellCompareTolerance && pasteSwell > -swellCompareTolerance );
            CString apertureName;
            BlockStruct* apertureBlock( NULL );

            if( isComlexAperture && noPasteSwell )
            {
               apertureName = maxElectricalPadApertureBlock->getName();
               apertureBlock = maxElectricalPadApertureBlock;
            }
            else if( isComlexAperture )
            {
               pasteComplexBlockName = createComplexExtendedPad( maxElectricalPadApertureBlock, pasteSwell );
               apertureName = maskComplexBlockName;
               apertureBlock = doc->Find_Block_by_Name(apertureName,file->getFileNumber());              
            }
            else
            {
               CStandardAperture pasteAperture(maxElectricalPadAperture);            
               pasteAperture.inset( pasteSwell );

               apertureBlock = getApertureBlock(pasteAperture);
               apertureName = apertureBlock->getName();
            }

            if (needPasteTop)
            {
               int layerIndex = accelLayers.getPasteTopLayer().getLayerIndex();
               
               if ( apertureBlock != NULL )
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
               }
               else
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true);
               }

            }

            if (needPasteBottom)
            {
               int layerIndex = accelLayers.getPasteBottomLayer().getLayerIndex();

               if ( apertureBlock != NULL )
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
               }

               else
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true);
               }

            }
         }

         if (needMaskTop || needMaskBottom)
         {
            bool noPasteSwell( maskSwell < swellCompareTolerance && maskSwell > -swellCompareTolerance );
            CString apertureName;
            BlockStruct* apertureBlock( NULL );

            if( isComlexAperture && noPasteSwell )
            {
               apertureName = maxElectricalPadApertureBlock->getName();
               apertureBlock = maxElectricalPadApertureBlock;
            }
            else if( isComlexAperture )
            {
               double swellDiff( pasteSwell + maskSwell );
               if( swellDiff < swellCompareTolerance && swellDiff > -swellCompareTolerance )
               {
                  maskComplexBlockName = pasteComplexBlockName;
               }
               else
               {
                  maskComplexBlockName = createComplexExtendedPad( maxElectricalPadApertureBlock, -maskSwell );
               }               
               apertureName = maskComplexBlockName;
               apertureBlock = doc->Find_Block_by_Name(apertureName,file->getFileNumber());              
            }
            else
            {
               CStandardAperture maskAperture(maxElectricalPadAperture);            
               maskAperture.inset( -maskSwell );

               apertureBlock = getApertureBlock(maskAperture);
               apertureName = apertureBlock->getName();
            }

            if (needMaskTop)
            {
               int layerIndex = accelLayers.getMaskTopLayer().getLayerIndex();

               if ( apertureBlock != NULL )
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
               }
               else
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true);
               }
            }

            if (needMaskBottom)
            {
               int layerIndex = accelLayers.getMaskBottomLayer().getLayerIndex();

               if ( apertureBlock != NULL )
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
               }
               else
               {
                  Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                     maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                     maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                     maxElectricalPadInsert->getScale(),layerIndex,true,blockTypeUnknown);
               }
            }
         }

         if (needPlane)
         {
            bool noPasteSwell( planeSwell < swellCompareTolerance && planeSwell > -swellCompareTolerance );
            CString apertureName;
            BlockStruct* apertureBlock( NULL );

            if( isComlexAperture && noPasteSwell )
            {
               apertureName = maxElectricalPadApertureBlock->getName();
               apertureBlock = maxElectricalPadApertureBlock;
            }
            else if( isComlexAperture )
            {
               double swellDiff( planeSwell - maskSwell );
               if( swellDiff < swellCompareTolerance && swellDiff > -swellCompareTolerance )
               {
                  apertureName = maskComplexBlockName;
               }
               else
               {
                  apertureName = createComplexExtendedPad( maxElectricalPadApertureBlock, -planeSwell );
               }
               apertureBlock = doc->Find_Block_by_Name(apertureName,file->getFileNumber());              
            }
            else
            {
               CStandardAperture planeAperture(maxElectricalPadAperture);            
               planeAperture.inset( -planeSwell );

               apertureBlock = getApertureBlock(planeAperture);
               apertureName = apertureBlock->getName();
            }

            int layerIndex = accelLayers.getPlaneLayer().getLayerIndex();

            if ( apertureBlock != NULL )
            {
                Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                  maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                  maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                  maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
            }
            else
            {
                Graph_Block_Reference(apertureName,"",file->getFileNumber(),
                  maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
                  maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
                  maxElectricalPadInsert->getScale(),layerIndex,true,blockTypeUnknown);
            }
         }
      }
      else if (needPasteTop || needPasteBottom || needMaskTop || needMaskBottom || needPlane)
      {
         BlockStruct* apertureBlock = getApertureBlock(maxElectricalPadAperture);
         CString apertureName = apertureBlock->getName();

         if (needPasteTop)
         {
            int layerIndex = accelLayers.getPasteTopLayer().getLayerIndex();

            Graph_Block_Reference(apertureName,"",file->getFileNumber(),
               maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
               maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
               maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
         }

         if (needPasteBottom)
         {
            int layerIndex = accelLayers.getPasteBottomLayer().getLayerIndex();

            Graph_Block_Reference(apertureName,"",file->getFileNumber(),
               maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
               maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
               maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
         }

         if (needMaskTop)
         {
            int layerIndex = accelLayers.getMaskTopLayer().getLayerIndex();

            Graph_Block_Reference(apertureName,"",file->getFileNumber(),
               maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
               maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
               maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
         }

         if (needMaskBottom)
         {
            int layerIndex = accelLayers.getMaskBottomLayer().getLayerIndex();

            Graph_Block_Reference(apertureName,"",file->getFileNumber(),
               maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
               maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
               maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
         }

         if (needPlane)
         {
            int layerIndex = accelLayers.getPlaneLayer().getLayerIndex();

            Graph_Block_Reference(apertureName,"",file->getFileNumber(),
               maxElectricalPadInsert->getOrigin().x,maxElectricalPadInsert->getOrigin().y,
               maxElectricalPadInsert->getAngle(),maxElectricalPadInsert->getMirrorFlags(),
               maxElectricalPadInsert->getScale(),layerIndex,true,apertureBlock->getBlockType());
         }
      }
   }
   else
   {
      int iii = 3;
   }
}

//void CAccelPadDefState::instantiateImpliedPads(bool topFlag)
//{
//   LayerStruct* pasteLayer  = getLayerWithType(topFlag ? LAYTYPE_PASTE_TOP : LAYTYPE_PASTE_BOTTOM);
//   LayerStruct* maskLayer   = getLayerWithType(topFlag ? LAYTYPE_MASK_TOP  : LAYTYPE_MASK_BOTTOM );
//
//}

CString CAccelPadDefState::createComplexExtendedPad(BlockStruct* const block, double swell )
{
   CPolyList* tempPolyList( block->getAperturePolys( doc->getCamCadData() ) );
   POSITION polyPos = tempPolyList->GetHeadPosition();
   polycnt = 0;
   while (polyPos)
   {
      CPoly *poly = tempPolyList->GetNext(polyPos);
      poly->shrink( swell, doc->getPageUnits() );
      POSITION pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos)
      {
         Point2 p1( *poly->getPntList().GetNext(pntPos) );
         polyarray.SetAtGrow(polycnt, p1);
         polycnt++;
      }
   }

   delete tempPolyList;

   CString shapeGeomName;
   shapeGeomName.Format("PADOUTLINE_%d", ++shapeoutlinecnt);

   BlockStruct *shapeGeom = Graph_Block_On(GBO_APPEND, shapeGeomName, file->getFileNumber(), 0);
   shapeGeom->setBlockType(BLOCKTYPE_PADSHAPE);
   int lindex = accelLayers.getFloatingLayer().getLayerIndex();
   write_poly(TRUE, lindex, doc->getZeroWidthIndex(), 0, "", 0, 0);
   Graph_Block_Off();

   CString apertureName;
   apertureName.Format("PADCOMPLEX_%d", shapeoutlinecnt);
   Graph_Complex(apertureName, 0, shapeGeom, 0.0, 0.0, 0.0); 
   polycnt = 0;

   return apertureName;
}


/*End **********************************************************************/


