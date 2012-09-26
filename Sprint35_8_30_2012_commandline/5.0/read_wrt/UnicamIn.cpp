// $Header: /CAMCAD/5.0/read_wrt/UnicamIn.cpp 45    6/17/07 9:00p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.

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
#include "lyrmanip.h"
#include "unicamin.h"
#include "CCEtoODB.h"
#include "centroid.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
//extern char *layertypes[];

/* Function Define *********************************************************/
static bool fgetcs(CString &string, FILE *ifp);  // A replacement for fgets only uses CString, no fixed buffer nuisances.
static double cnv_unit(const char *);
static int go_command(List *,int);
static void free_all_mem();
static int get_tok();
static int p_error();
static int tok_search(List *,int);
static int push_tok();
static int loop_command(List *,int);
static int update_pdef(int,int);
static int get_next(CString &bug, bool newbuffer);
static int get_line(CString &buf);
static int get_pincnt(const char *,int,int *);
static int do_planenet(char *);
static GraphicClassTag get_layerclass(const char *layername);
static int load_unicamsettings(const CString fname);
static int assign_layer();
static void setFileReadProgress(const CString& status);

/* Globals ****************************************************************/

typedef  struct
{
   int      cur_layer;                    /* Layer.      */
   int      cur_widthindex;               //derived linewidth index
   double   cur_width;
   int      visibility;                   // visibility of vias
	double	sizex;
	double	sizey;
} Global;

typedef struct
{
   int      major;
   int      minor;
   int      revision;
}U_VERSION;

typedef CArray<Point2, Point2&> CPolyArray;

static CAttrmapArray    attrmaparray;
static int              attrmapcnt;

static CPolyArray       poly_l;
static int              polycnt = 0;

static CPadformArray    padformarray;
static int              padformcnt;

static CPcblevelArray   pcblevelarray;
static int              pcblevelcnt;

static U_VERSION        u_version;
static int              unicamversion;                // 1 = 1.1.4
                                                      // 2 = 1.1.5
static UFiducial        cur_fiducial;                /* current used library name */
static UComponent       cur_component;                /* current used library name */
static CString          cur_shapename;               /* current used library name */
static CString          cur_pinname;                 /* current used library name */
static CString          cur_padname;                 /* current used library name */
static CString          cur_netname;                 /* current used library name */

static FileStruct       *file = NULL;
static FileStruct			*boardFile = NULL;
static CCEtoODBDoc       *doc;
static DataStruct       *cur_compdata;    // global pointer to the current component.
static FILE             *ifp;                         /* File pointers.    */
static long             ifp_line = 0;                 /* Current line number. */
static FILE             *ferr;
static int              cur_filenum = 0;
static CString          token;              /* Current token.       */
static char             unit_flag = 'B';              /* I = inch M = metric A = mil B = basic */
static int              display_error = 0;
static int              token_name = FALSE;
static int              PageUnits;
static double           attheight;
static double           attwidth;
static char             keep_unrout_layer;
static int              Push_tok = FALSE;
static CString          cur_line;
static bool             cur_new = true;
static BOOL				   UseCLocation = FALSE;
static Global           G;                            /* global const. */
static int              ComponentSMDrule;    // 0 = take existing ATT_SMD
                                             // 1 = if all pins of comp are marked as SMD
                                             // 2 = if most pins of comp are marked as SMD
                                             // 3 = if one pin of comp are marked as SMD

enum ENetlistMode
{
	eNetConnected,
	eNetUnconnected
};

static ENetlistMode Netlist_Mode = eNetConnected;

static CFileReadProgress*	fileReadProgress = NULL; 

// This is used to keep track of the object context to which the graphic belongs.
// Or more simply put, used to decide what graphic class to assign to vector or arc.
static GraphicClassTag curGraphicClass = graphicClassNormal;

//---------------------------------------------------------------

void WriteStandardLogHeader(FILE *fp)
{
   CTime time = CTime::GetCurrentTime();

   fprintf(fp, "; ---------------------------------------------------\n");
   fprintf(fp, "; Unicam PDW Import Log\n");
   fprintf(fp, "; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
   fprintf(fp, "; %s\n", time.Format("%A, %B %d, %Y"));
   fprintf(fp, "; ---------------------------------------------------\n");
}

/******************************************************************************
* ReadUnicamPdw
*/
void ReadUnicamPdw(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits)
{
   file = NULL;
	boardFile = NULL;
   doc = Doc;
   display_error = 0;

   PageUnits = pageUnits;

   Push_tok = FALSE;
   cur_new = true;
   cur_line.Empty();
   ifp_line = 0;

   attheight = 0.07;
   attwidth  = 0.05;

   curGraphicClass = graphicClassNormal;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(filename, "rt")) == NULL)
   {
      CString err;
      err.Format("Error open [%s] file", filename);
      ErrorMessage(err, "Error");
      return;
   }

   CString unicamLogFile;
   ferr = getApp().OpenOperationLogFile(UNICAMERR, unicamLogFile);
   if (ferr == NULL) // error message already issued, just return.
      return;

   WriteStandardLogHeader(ferr);

   poly_l.SetSize(100, 100);
   polycnt = 0;

   attrmaparray.SetSize(100, 100);
   attrmapcnt = 0;

   padformarray.SetSize(100, 100);
   padformcnt = 0;

   pcblevelarray.SetSize(100, 100);
   pcblevelcnt = 0;

   /* initialize scanning parameters */
   cur_netname = "";
   memset(&G, 0, sizeof(Global));
   cur_shapename = "";                 
   cur_pinname = "";                
   cur_padname = "";                
   
   CString settingsFile = getApp().getImportSettingsFilePath("unicam.in");
   {
      CString msg;
      msg.Format("\nUNICAM: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_unicamsettings(settingsFile);

   // 
   BlockStruct *block = Graph_Block_On(GBO_APPEND, "NULLPAD", -1, 0);
   block->setBlockType(BLOCKTYPE_PADSTACK);
   Graph_Block_Off();

   rewind(ifp);

   setFileReadProgress("Reading PDW file");

   if (go_command(start_lst, SIZ_START_LST) < 0)
   {
      // printf big prob.
      ErrorMessage("UNICAM read error", "Error");
   }

   setFileReadProgress("Processing Data");

   fclose(ifp);

   if (!keep_unrout_layer)
   {
      int layerNum = Graph_Level("UNROUT", "", 0);
      LayerStruct *layer = doc->FindLayer(layerNum);
      DeleteLayer(doc, layer, TRUE);
   }

   assign_layer();

	// There is a bug here. The PDW may contain more than one "board". This whole
	// importer is assuming there will be only one. We read mult-board PDW's okay,
	// that is, we don't choke on them. But we don't generate unique names for each
	// board, and here we only post-process the last board read.
	// Apparantly no one has complained about this so there seems no return on
	// investment to fix it at the moment. No such sample data to validate with, either.

   if (boardFile)
   {
      double accuracy = get_accuracy(doc);
      generate_NCnet(doc, boardFile); // this function generates the NC (non connect) net.

      EliminateSinglePointPolys(doc);                
      BreakSpikePolys(boardFile->getBlock()); 

      //progress->SetStatus("Crack");
      Crack(doc, boardFile->getBlock(), TRUE);         
      //progress->SetStatus("Elim traces");
      EliminateOverlappingTraces(doc, boardFile->getBlock(), TRUE, accuracy);               

      update_smdpads(doc);
      update_smdrule_geometries(doc, ComponentSMDrule);
      update_smdrule_components(doc, boardFile, ComponentSMDrule);
      RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

      generate_PADSTACKACCESSFLAG(doc, 1);
      generate_PINLOC(doc, boardFile, 1);   // this function generates the PINLOC argument for all pins.

      int layerNum = Graph_Level("SILKSCREEN_TOP", "", 0);
      double minref = 0.05 * Units_Factor(UNIT_INCHES, pageUnits);
      double maxref = 0.05 * Units_Factor(UNIT_INCHES, pageUnits);
      update_manufacturingrefdes(doc, boardFile,*(boardFile->getBlock()), minref, maxref, layerNum, FALSE);
   }

   poly_l.RemoveAll();
   polycnt = 0;

   for (int i=0; i<attrmapcnt; i++)
      delete attrmaparray[i];  
   attrmaparray.RemoveAll();
   attrmapcnt = 0;

   padformarray.RemoveAll();  

   free_all_mem();

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!unicamLogFile.IsEmpty())
      fclose(ferr);

   if (display_error && !unicamLogFile.IsEmpty())
      Logreader(unicamLogFile);

   CUnicamComponents::empty();
}

//_____________________________________________________________________________
static void setFileReadProgress(const CString& status)
{
   delete fileReadProgress;

   if (status.IsEmpty())
   {
      fileReadProgress = NULL;
   }
   else
   {
      fileReadProgress = new CFileReadProgress(ifp);
      fileReadProgress->updateStatus(status);
   }
}

/******************************************************************************
* update_pcblevel
*/
static int update_pcblevel(int level)
{
   UPcblevel pcbLevel;

   for (int i=0; i<pcblevelcnt; i++)
   {
      pcbLevel = pcblevelarray.ElementAt(i);
      if (pcbLevel.level == level)
         return i;         
   }

   pcbLevel.level = level;
   pcblevelarray.SetAtGrow(pcblevelcnt++, pcbLevel);  

   return pcblevelcnt-1;
}

/******************************************************************************
* get_padformindex
*/
static int get_padformindex(ApertureShapeTag form, double sizeA, double sizeB)
{
   UPadform padform;

   for (int i=0; i<padformcnt; i++)
   {
      padform = padformarray.ElementAt(i);
      if (padform.form == form && padform.sizeA == sizeA && padform.sizeB == sizeB)
         return i;         
   }

   int err;
   CString name;
   name.Format("PADSHAPE_%d", padformcnt);
   Graph_Aperture(name, form, sizeA, sizeB, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

   padform.form = form;
   padform.sizeA = sizeA;
   padform.sizeB = sizeB;
   padformarray.SetAtGrow(padformcnt++, padform);  

   return padformcnt -1;
}

/******************************************************************************
* get_drilltoolindex
*/
static int get_drilltoolindex(ApertureShapeTag form, double sizeA, double sizeB)
{
   UPadform padform;

   for (int i=0; i<padformcnt; i++)
   {
      padform = padformarray.ElementAt(i);
      if (padform.form == form && padform.sizeA == sizeA && padform.sizeB == sizeB)
         return i;         
   }

   int err;
   CString name;
   name.Format("PADSHAPE_%d",padformcnt);
   Graph_Aperture(name, form, sizeA, sizeB, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

   padform.form = form;
   padform.sizeA = sizeA;
   padform.sizeB = sizeB;
   padformarray.SetAtGrow(padformcnt++, padform);  

   return padformcnt -1;
}

/******************************************************************************
* get_attrmap
*/
static const char* get_attrmap(const char *a)
{
   for (int i=0; i<attrmapcnt; i++)
   {
      if (attrmaparray[i]->uni_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name.GetBuffer(0);
   }

   return a;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* do_global
*/
static int do_global()
{
   int i, brk;
   int res;
   int done = FALSE;

   while (!done && get_tok())
   {
      switch(brk = tok_search(brk_lst, SIZ_BRK_LST))
      {
      case BRK_B_SQUARE:   
         done = TRUE;
         break;
      default:
         if ((i = tok_search(glb_lst, SIZ_GLB_LST)) >= 0)
            res = (*glb_lst[i].function)();
         break;
      }
   }

   return 1;
}

/******************************************************************************
* go_command
   Call function associated with next token.
   The token must be enclosed by brackets ( {} or [] ).
   Tokens enclosed by {} are searched for on the local
   token list.  Tokens enclosed by [] are searched for
   in the global token list.
*/
int go_command(List *tok_lst,int lst_size)
{
   int i, brk, res = 0;
   int square_bracket = 0;

   if (!get_tok())
      return p_error();

   switch((brk = tok_search(brk_lst, SIZ_BRK_LST)))
   {
      case BRK_SQUIGLY:
      {
         if (get_tok()) 
         {
            switch(brk = tok_search(brk_lst, SIZ_BRK_LST))
            {
            case BRK_B_SQUIGLY:  
               {
                  push_tok();
                  return 1;
               }
               break;
            case BRK_SQUARE:
               {
                  do_global();
               }
               break;
            default:
               {
                  if ((i = tok_search(tok_lst, lst_size)) >= 0)
                  {
                     res = (*tok_lst[i].function)();
                     if (res < 0)
                     {
                        CString ls = tok_lst[i].token;
                        int w = 0;
                     }
                  }
                  else
                  {
                     fnull();
                  }
               }
               break;
            } 
         }
      }
      break;
   case BRK_SQUARE:
      {
         do_global();
      }
      break;
   case BRK_B_SQUIGLY:
      {
         push_tok();
         return 1;
      }
      break;
   case BRK_B_SQUARE:   
      {
         // and [] are always nodal commands (no hierachies). 
         if (square_bracket)
         {
            square_bracket--;
            push_tok();
         }
         else
         {
            // close a square bracket, even that it was not open ???
            int w = 0;
         }
         return 1;
      }
      break;
   default:
      {
         if ((i = tok_search(tok_lst, lst_size)) >= 0)
         {
            res = (*tok_lst[i].function)();
            if (res == -2) // logical end of file
               return -2;
         }
         else
         {
            // unknown token - just skip this line
#ifdef _DEBUG
            fprintf(ferr,"DEBUG: Unsupported Token [%s] at %ld -> skipped\n", token, ifp_line);
            display_error++;
#endif
            if (!get_tok())
               return p_error();    // get next token value      

            return 1;
         }
      }
      break;
   }

   if (res == -1)
   {
      CString message;
      message.Format("UNICAM Read error : Token [%s] at %ld", token, ifp_line);
      ErrorMessage(message, "Error");
      return -1;
   }

   return res;
}                                   

/******************************************************************************
* loop_command
   Loop through a section.
*/
int loop_command(List *list,int size)
{
   int repeat = TRUE;

   while (repeat)
   {
      if (go_command(list, size) < 0)  
         return -1;
      if (!get_tok())
         return p_error();
      if (tok_search(brk_lst, SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      else
         push_tok();
   }
   return 1;
}


/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next }] endpoint.
*/
int fnull()
{
   int brk_count = 0;
   int square_brk_count = 0;
   
#ifdef _DEBUG
   fprintf(ferr, "DEBUG: Unsupported Token [%s] at %ld -> skipped\n", token, ifp_line);
   display_error++;
#endif

   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)
            continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_SQUIGLY:
            ++brk_count;
            break;
         case BRK_SQUARE:
            ++square_brk_count; // [
            break;
         case BRK_B_SQUIGLY:
            if (--brk_count == 0)
            {
               return 1;
            }
            break;
         case BRK_B_SQUARE:
               // if in a global, also stop
            if (square_brk_count == 0 && brk_count == 0) // ]
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
   return 1;
}

/******************************************************************************
* fskip
   NULL function.
   Skips over any tokens to next }] endpoint.
*/
int fskip()
{
   int brk_count = 0;
   int square_brk_count = 0;
   
   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)
            continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_SQUIGLY:
            ++brk_count;
            break;
         case BRK_SQUARE:
               //++brk_count;
            ++square_brk_count; // [
            break;
         case BRK_B_SQUIGLY:
            if (--brk_count == 0)
            {
               return 1;
            }
            break;
         case BRK_B_SQUARE:
            // if in a global, also stop
            if (--square_brk_count == 0 && brk_count == 0) // ]
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
   return 1;
}

/******************************************************************************
* get_location
*/
static int get_location(double *x, double *y, double *rot, int *side)
{
   if (!get_tok())
      return p_error();    // get {       
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // x   
   *x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   *y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // rot   
   *rot = atof(token) / 10;

   if (!get_tok())
      return p_error();    //     
   *side = atoi(token);

   if (*side == 1)
      *rot = 360 - *rot;

   if (!get_tok())
      return p_error();    // get }       

   return 1;
}

/******************************************************************************
* uni_project
*/
int uni_project()
{
   if (!get_tok())   
      return p_error(); /* Original file name.  */

   // make it the first layer
   Graph_Level("0", "", 1);

   int res = loop_command(project_lst, SIZ_PROJECT_LST);

   return res;
}

/******************************************************************************
* uni_board
*/
int uni_board()
{
   if (!get_tok())
      return p_error();    // board number

	file = Graph_File_Start("Board", fileTypeUnicam);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
	boardFile = file;

   cur_filenum = file->getFileNumber();

	if (!get_tok())
      return p_error();    // get {  
   
	G.cur_layer = Graph_Level("BOARD", "", 0); 
   G.cur_widthindex = 0;
   
   loop_command(board_lst, SIZ_BOARD_LST);

	Graph_Block_Off();

   return 1;
}

/******************************************************************************
* uni_note
*/
int uni_note()
{
   if (!get_tok())
      return p_error();    // part ???
   if (!get_tok())
      return p_error();    // number  
   
   fskip();

   return 1;
}

/******************************************************************************
* uni_panel
*/
int uni_panel()
{
	file = Graph_File_Start("Panel", fileTypeUnicam);
   file->setBlockType(blockTypePanel );
   file->getBlock()->setBlockType(file->getBlockType());
	file->setShow(false);

   cur_filenum = file->getFileNumber();

	if (!get_tok())
      return p_error();    // get {  

	G.cur_layer = Graph_Level("PANEL", "", 0); 
   G.cur_widthindex = 0;

	loop_command(panel_lst, SIZ_PANEL_LST);

	Graph_Block_Off();

	return 1;
}

/******************************************************************************
* uni_param
*/
int uni_param()
{
	if (!get_tok())
      return p_error();    // get {  

   loop_command(param_lst, SIZ_PARAM_LST);

   DataStruct *data = Graph_PolyStruct(G.cur_layer, 0, 0);
   Graph_Poly(NULL, G.cur_widthindex, 0 , 0, 1);   // fillflag , negative, closed
   Graph_Vertex(0.0, 0.0, 0.0);
   Graph_Vertex(G.sizex, 0.0, 0.0);
   Graph_Vertex(G.sizex, G.sizey, 0.0);
   Graph_Vertex(0.0, G.sizey, 0.0);
   Graph_Vertex(0.0, 0.0, 0.0);

	data->setGraphicClass(graphicClassPanelOutline);

	return 1;
}

/******************************************************************************
* uni_boardinst
*/
int uni_boardinst()
{
	int boardCnt = 0;

	if (!get_tok())
		return p_error();    // get }

	// board instance loop
   while (TRUE)
   {
      if (!get_tok())
         return p_error();    // get board number
		if (!STRCMPI(token, "}"))
      {
         push_tok();
         break;
      }

      if (!get_tok())
         return p_error();    // get {
   
		boardCnt++;

      if (!get_tok())
         return p_error();    // board number b, must be 1
      int boardNumB = atoi(token);

      if (!get_tok())
         return p_error();    // originx
      double x = cnv_unit(token);

      if (!get_tok())
         return p_error();    // originy
      double y = cnv_unit(token);

      if (!get_tok())
         return p_error();    // rotation
      double rot = atof(token) / 10;

      if (!get_tok())
         return p_error();    // mirror, must be 0
      int mirror = atoi(token);

      if (!get_tok())
         return p_error();    // location_relative
		CString locRelative = token;

		CString boardName;
		boardName.Format("Board_%d", boardCnt);
		DataStruct *data = Graph_Block_Reference("Board", boardName, 0, x, y, DegToRad(rot), 0, 1.0, -1, FALSE);
		data->getInsert()->setInsertType(insertTypePcb);

		if (!get_tok())
			return p_error();    // get }
	}

	if (!get_tok())
		return p_error();    // get }

   return 1;
}

/******************************************************************************
* uni_sizex
*/
int uni_sizex()
{
	if (!get_tok())
      return p_error();    // get sizex

	G.sizex = cnv_unit(token);

	return 1;
}

/******************************************************************************
* uni_sizey
*/
int uni_sizey()
{
	if (!get_tok())
      return p_error();    // get sizey

	G.sizey = cnv_unit(token);

	return 1;
}

/******************************************************************************
* uni_thick
*/
int uni_thick()
{
	if (!get_tok())
      return p_error();    // get thickness

	double thickness = cnv_unit(token);

	return 1;
}

/******************************************************************************
* uni_jig
*/
int uni_jig()
{
	if (!get_tok())
      return p_error();    // get jig

	CString jig = token;

	return 1;
}

/******************************************************************************
* uni_view
*/
int uni_view()
{
#ifdef _DEBUG
   fprintf(ferr, "DEBUG: View not implemented\n");
#endif

   fnull();

   return 1;
}

/******************************************************************************
* uni_linedef
*/
int uni_linedef()
{
#ifdef _DEBUG
   fprintf(ferr, "DEBUG: Linedef not implemented\n");
#endif

   fnull();

   return 1;
}

/******************************************************************************
* uni_board_outline
*/
int uni_board_outline()
{
   curGraphicClass = graphicClassBoardOutline;
   int retval = uni_outline_parser();
   curGraphicClass = graphicClassNormal;

   return retval;
}

/******************************************************************************
* uni_shape_outline
*/
int uni_shape_outline()
{
   curGraphicClass = graphicClassComponentOutline;
   int retval = uni_outline_parser();
   curGraphicClass = graphicClassNormal;

   return retval;
}

/******************************************************************************
* uni_outline_parser
*/
int uni_outline_parser()
{
   if (!get_tok())
      return p_error();    // get {    
   
   loop_command(graphic_lst, SIZ_GRAPHIC_LST);

   return 1;
}

/******************************************************************************
* uni_shape
*/
int uni_shape()
{
   if (!get_tok())
      return p_error();    // get shape name   
   cur_shapename = token;
   Graph_Block_On(GBO_APPEND, token, -1, 0);

   G.cur_layer = Graph_Level("SILKSCREEN_TOP", "", 0); 
   G.cur_widthindex = 0;

   loop_command(shape_lst, SIZ_SHAPE_LST);
   
   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* clear_compstruct

   CString  name;
   CString  cclass;     // class      : 2,
   CString  ctype;      // type       : "49-210019",
   CString  cadtype;    // %cadtype   : "49210019.PRT",
   CString  shapeid;    // shapeid    : "49210019.PRT",
   CString  descr;      // descr      : "CAP, .01uF, 50V, +/-10%",
   CString  insclass;   // insclass   : "SMT",
   CString  gentype;    // gentype    : "1206C-08",
   CString  machine;    // machine    : "TP11_P",
   CString  test;       // test       : "49-210019",
   int      updated;    // updated    : 1,
   int      insorder;   // insorder   : 2,
   int      mtechn;     // %mtechn    : 2,
   Location clocation;  // %clocation : {85344061,280035202,0,0},
   Location location;   // location   : {85344061,280035202,0,0},
   CString  marking;    // %_marking  : ".01uF",
   CString  item;       // %_item     : "036" 
*/
static void clear_compstruct()
{
   cur_component.name = "";
   cur_component.cclass = "";
   cur_component.ctype = "";
   cur_component.cadtype = "";
   cur_component.shapeid = "";
   cur_component.package = "";
   cur_component._carry_mode = "";

   cur_component._pmabar = "";
   cur_component._main_mark = "";
   cur_component._sub_mark = "";
   cur_component._term_seq = "";
   cur_component._feeder_style = "";
   cur_component.polarized = -1;

   cur_component.pinref = "";
   cur_component.descr = "";
   cur_component.insclass = "";
   cur_component.gentype = "";
   cur_component.machine = "";
   cur_component.comment = "";
   cur_component.schname = "";
   cur_component.test = "";
   cur_component.updated = -1;
   cur_component.insorder = -1;
   cur_component.bom = -1;
   cur_component.mtechn = -1;
   cur_component.marking = "";
   cur_component.item = "";
   cur_component.ttolpos = -1;
   cur_component.ttolneg = -1;
   cur_component.tvalue = "";

   cur_component._direction = ""; // %_
   cur_component._polarized = ""; // %_
   cur_component._package = "";  // %_
   cur_component._traypickup = "";// %_

   cur_component.hasCLocation = FALSE;
   return;
}

/******************************************************************************
* clear_fiducialstruct
*/
static void clear_fiducialstruct()
{
   cur_fiducial.name = "";
   return;
}

/******************************************************************************
* comp_type
*/
int comp_type()
{
   if (!get_tok())
      return p_error();   
   cur_component.ctype = token;
   return 1;
}

/******************************************************************************
* fiducial_type
*/
int fiducial_type()
{
   if (!get_tok())
      return p_error();   
   cur_fiducial.type = token;
   return 1;
}

/******************************************************************************
* fiducial_f_type
*/
int fiducial_f_type()
{
   if (!get_tok())
      return p_error();   
   cur_fiducial.f_type = atoi(token);
   return 1;
}

/******************************************************************************
* fiducial_f_class
*/
int fiducial_f_class()
{
   if (!get_tok())
      return p_error();   
   cur_fiducial.f_class = atoi(token);
   return 1;
}

/******************************************************************************
* fiducial_f_sel
*/
int fiducial_f_sel()
{
   if (!get_tok())
      return p_error();   
   cur_fiducial.f_sel = atoi(token);
   return 1;
}

/******************************************************************************
* comp_cadtype
*/
int comp_cadtype()
{
   if (!get_tok())
      return p_error();     
   cur_component.cadtype = token;
   return 1;
}

/******************************************************************************
* comp_class
*/
int comp_class()
{
   if (!get_tok())
      return p_error();     
   cur_component.cclass = token;
   return 1;
}

/******************************************************************************
* comp_shapeid
*/
int comp_shapeid()
{
   if (!get_tok())
      return p_error();       
   cur_component.shapeid = token;
   return 1;
}

/******************************************************************************
* fiducial_shapeid
*/
int fiducial_shapeid()
{
   if (!get_tok())
      return p_error();       
   cur_fiducial.shapeid = token;
   return 1;
}

/******************************************************************************
* comp_package
*/
int comp_package()
{
   if (!get_tok())
      return p_error();       
   cur_component.package = token;
   return 1;
}

/******************************************************************************
* comp_pmabar
*/
int comp_pmabar()
{
   if (!get_tok())
      return p_error();       
   cur_component._pmabar = token;
   return 1;
}

/******************************************************************************
* comp_main_mark
*/
int comp_main_mark()
{
   if (!get_tok())
      return p_error();       
   cur_component._main_mark = token;
   return 1;
}

/******************************************************************************
* comp_sub_mark
*/
int comp_sub_mark()
{
   if (!get_tok())
      return p_error();       
   cur_component._sub_mark = token;
   return 1;
}

/******************************************************************************
* comp_term_seq
*/
int comp_term_seq()
{
   if (!get_tok())
      return p_error();       
   cur_component._term_seq = token;
   return 1;
}

/******************************************************************************
* comp_direction
*/
int comp_direction()
{
   if (!get_tok())
      return p_error();       
   cur_component._direction = token;
   return 1;
}

/******************************************************************************
* comp__polarized
*/
int comp__polarized()
{
   if (!get_tok())
      return p_error();       
   cur_component._polarized = token;
   return 1;
}

/******************************************************************************
* comp__package
*/
int comp__package()
{
   if (!get_tok())
      return p_error();       
   cur_component._package = token;
   return 1;
}

/******************************************************************************
* comp__traypickup
*/
int comp__traypickup()
{
   if (!get_tok())
      return p_error();       
   cur_component._traypickup = token;
   return 1;
}

/******************************************************************************
* comp_polarized
*/
int comp_polarized()
{
   if (!get_tok())
      return p_error();       
   cur_component.polarized = atoi(token);
   return 1;
}

/******************************************************************************
* comp_feeder_style
*/
int comp_feeder_style()
{
   if (!get_tok())
      return p_error();       
   cur_component._feeder_style = token;
   return 1;
}

/******************************************************************************
* comp_carry_mode
*/
int comp_carry_mode()
{
   if (!get_tok())
      return p_error();       
   cur_component._carry_mode = token;
   return 1;
}

/******************************************************************************
* comp_pinref
*/
int comp_pinref()
{
   if (!get_tok())
      return p_error();       
   cur_component.pinref = token;
   return 1;
}

/******************************************************************************
* comp_descr
*/
int comp_descr()
{
   if (!get_tok())
      return p_error();    
   cur_component.descr = token;
   return 1;
}

/******************************************************************************
* comp_insclass
*/
int comp_insclass()
{
   if (!get_tok())
      return p_error();   
   cur_component.insclass = token;
   return 1;
}

/******************************************************************************
* comp_comment
*/
int comp_comment()
{
   if (!get_tok())
      return p_error();   
   cur_component.comment = token;
   return 1;
}

/******************************************************************************
* comp_schname
*/
int comp_schname()
{
   if (!get_tok())
      return p_error();   
   cur_component.schname = token;
   return 1;
}

/******************************************************************************
* fiducial_schname
*/
int fiducial_schname()
{
   if (!get_tok())
      return p_error();   
   cur_fiducial.schname = token;
   return 1;
}

/******************************************************************************
* comp_gentype
*/
int comp_gentype()
{
   if (!get_tok())
      return p_error();       
   cur_component.gentype = token;
   return 1;
}

/******************************************************************************
* comp_machine
*/
int comp_machine()
{
   if (!get_tok())
      return p_error();    // get compname name   
   cur_component.machine = token;
   return 1;
}

/******************************************************************************
* comp_test
*/
int comp_test()
{
   if (!get_tok())
      return p_error();       
   cur_component.test = token;
   return 1;
}

/******************************************************************************
* comp_updated
*/
int comp_updated()
{
   if (!get_tok())
      return p_error();      
   cur_component.updated = atoi(token);
   return 1;
}

/******************************************************************************
* comp_bom
*/
int comp_bom()
{
   if (!get_tok())
      return p_error();      
   cur_component.bom = atoi(token);
   return 1;
}

/******************************************************************************
* comp_insorder
*/
int comp_insorder()
{
   if (!get_tok())
      return p_error();       
   return 1;
}

/******************************************************************************
* comp_mtechn
*/
int comp_mtechn()
{
   if (!get_tok())
      return p_error();    // get compname name   
   return 1;
}

/******************************************************************************
* comp_clocation
*/
int comp_clocation()
{
   double x, y, rot;
   int side;

   get_location(&x, &y, &rot, &side);

   cur_component.clocation.x = x;
   cur_component.clocation.y = y;
   cur_component.clocation.rot = rot;
   cur_component.clocation.side = side;
   cur_component.hasCLocation = TRUE;//(TRUE && UseCLocation) || !side;

   return 1;
}

/******************************************************************************
* comp_location
*/
int comp_location()
{
   double x, y, rot;
   int side;

   get_location(&x, &y, &rot, &side);

   cur_component.location.x = x;
   cur_component.location.y = y;
   cur_component.location.rot = rot;
   cur_component.location.side = side;

   return 1;
}

/******************************************************************************
* fiducial_location
*/
int fiducial_location()
{
   double x, y, rot;
   int side;

   get_location(&x, &y, &rot, &side);

   cur_fiducial.location.x = x;
   cur_fiducial.location.y = y;
   cur_fiducial.location.rot = rot;
   cur_fiducial.location.side = side;

   return 1;
}

/******************************************************************************
* fiducial_clocation
*/
int fiducial_clocation()
{
   double x, y, rot;
   int side;

   get_location(&x, &y, &rot, &side);

   cur_fiducial.clocation.x = x;
   cur_fiducial.clocation.y = y;
   cur_fiducial.clocation.rot = rot;
   cur_fiducial.clocation.side = side;

   return 1;
}

/******************************************************************************
* comp_marking
*/
int comp_marking()
{
   if (!get_tok())
      return p_error();      
   cur_component.marking = token;

   return 1;
}

/******************************************************************************
* comp_ttolneg
*/
int comp_ttolneg()
{
   if (!get_tok())
      return p_error();      
   cur_component.ttolneg = atof(token);

   return 1;
}

/******************************************************************************
* comp_tvalue
*/
int comp_tvalue()
{
   if (!get_tok())
      return p_error();      
   cur_component.tvalue = token;

   return 1;
}

/******************************************************************************
* comp_ttolpos
*/
int comp_ttolpos()
{
   if (!get_tok())
      return p_error();      
   cur_component.ttolpos = atof(token);

   return 1;
}

/******************************************************************************
* comp_item
*/
int comp_item()
{
   if (!get_tok())
      return p_error();       
   cur_component.item = token;

   return 1;
}

/******************************************************************************
* uni_part

   CString  name;
   CString  cclass;     // class      : "49-210019",
   CString  ctype;      // type       : "49-210019",
   CString  cadtype;    // %cadtype   : "49210019.PRT",
   CString  shapeid;    // shapeid    : "49210019.PRT",
   CString  descr;      // descr      : "CAP, .01uF, 50V, +/-10%",
   CString  insclass;   // insclass   : "SMT",
   CString  gentype;    // gentype    : "1206C-08",
   CString  machine;    // machine    : "TP11_P",
   CString  test;       // test       : "49-210019",
   int      updated;    // updated    : 1,
   int      insorder;   // insorder   : 2,
   int      mtechn;     // %mtechn    : 2,
   Location clocation;  // %clocation : {85344061,280035202,0,0},
   Location location;   // location   : {85344061,280035202,0,0},
   CString  marking;    // %_marking  : ".01uF",
   CString  item;       // %_item     : "036" 
*/
int uni_part()
{
   clear_compstruct();

   if (!get_tok())
      return p_error();    // get compname name   

   cur_component.name = token;

   loop_command(component_lst, SIZ_COMPONENT_LST);

	// get the pin centroid of the block because cLocation is the location of the component
	// based on the pin centroid
	BlockStruct *shapeBlock = doc->Find_Block_by_Name(cur_component.shapeid, -1);
	if (shapeBlock == NULL)
		return 0;

	// Case 1730, do not use the DataList->getPinCentroid function to get the centroid.
	// As of this writing the DataList->getPinCentroid will return the AVERAGE of xy values.
	// We want the MEAN of xy values, as is done for Generate Component Centroids.
	// So use the same function that Gen Comp Centroids is using.
	//CPoint2d point = shapeBlock->getPinCentroid();
	double centx = 0.0, centy = 0.0;
	CalculateCentroid_PinCenters(doc, shapeBlock, &centx, &centy);
	CPoint2d point(centx, centy);
	double rotation;
	int side;

	
	if (cur_component.hasCLocation /*&& UseCLocation*/)
	{

		rotation = cur_component.clocation.rot;
		side = cur_component.clocation.side;
	
		CTMatrix t;
		t.scale((side != 0) ? -1 : 1, 1);
		t.rotateDegrees(rotation);
		t.translate(cur_component.clocation.x, cur_component.clocation.y);
		// calcuate the origin based on the pin extent center
		point.x *= -1;
		point.y *= -1;
		t.transform(point);
		
	}
	else
	{
		// Previous code used "location" in same fashion as "clocation" above, to make a transform 
		// and apply it to the component centroid. That is wrong, at least, in data for case 1730 
		// it made bad locations. The "location' is the origin of the "shape", it is not the origin 
		// of the "pins centroid". ("clocation" is the origin of the pins centroid.)
		// Just using the location directly makes case 1730 data import okay using "location" 
		// (tested by disabling the "clocation" side, in "real life" case 1730 data will use "clocation").
		rotation = cur_component.location.rot;
		side = cur_component.location.side;
		point.x = cur_component.location.x;
		point.y = cur_component.location.y;
	}

	DataStruct *data = NULL;
	data = Graph_Block_Reference(cur_component.shapeid, cur_component.name, 0,
					point.x, point.y, DegToRad(rotation), side, 1.0, -1, TRUE);

	BlockStruct *block = Graph_Block_Exists(doc, cur_component.shapeid, -1);
   data->getInsert()->setInsertType(default_inserttype(block->getBlockType()));

   // refname
   doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_REFNAME, cur_component.name, SA_OVERWRITE, NULL); 

   if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT && strlen(cur_component.ctype))
   {
      CUnicamComponents::getComponents().setAt(cur_component.name,*data);

      if (!cur_component.ctype.CompareNoCase("NO-LOAD"))
      {

      }
      else if (!cur_component.ctype.CompareNoCase("UNDEF"))
      {
         // no type ???
         CString typname;
         typname.Format("%s_%s", cur_component.shapeid, cur_component.name);
         TypeStruct *curtype = AddType(file, typname);
      
         curtype->setBlockNumber( data->getInsert()->getBlockNumber());
         doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_TYPELISTLINK, cur_component.ctype, SA_OVERWRITE, NULL);

      }
      else if (!cur_component.ctype.CompareNoCase("TESTPOINT"))
      {
         data->getInsert()->setInsertType(insertTypeTestPoint);
      }
      else if (!cur_component.ctype.CompareNoCase("TOOLING_HOLE"))
      {
         data->getInsert()->setInsertType(insertTypeDrillTool);
      }
      else
      {
         // it is allowed to attach a different pattern here than in compdef
         TypeStruct *curtype = AddType(file, cur_component.ctype);
   
         // here update compdef list
         if (curtype->getBlockNumber() > -1 && curtype->getBlockNumber() != data->getInsert()->getBlockNumber())
         {
            BlockStruct *already_assigned = doc->Find_Block_by_Num(curtype->getBlockNumber());
            fprintf(ferr, "Try to assign Pattern [%s] to Device [%s] -> already a different Pattern [%s] assigned!\n",
               cur_component.shapeid, cur_component.ctype, already_assigned->getName());
            display_error++;

            CString typname;
            typname.Format("%s_%s", cur_component.shapeid, cur_component.name);
            curtype = AddType(file, typname);
      
            curtype->setBlockNumber( data->getInsert()->getBlockNumber());
            doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_TYPELISTLINK, cur_component.ctype, SA_OVERWRITE, NULL);
         }
         else
         {
            curtype->setBlockNumber( data->getInsert()->getBlockNumber());
            doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_TYPELISTLINK, cur_component.ctype, SA_OVERWRITE, NULL);
         }
      }
   }

   if (strlen(cur_component.cclass))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("class"), cur_component.cclass, SA_OVERWRITE, NULL);

   if (strlen(cur_component.cadtype))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("cadtype"), cur_component.cadtype, SA_OVERWRITE, NULL);

   if (strlen(cur_component.descr))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("descr"), cur_component.descr, SA_OVERWRITE, NULL);

   if (strlen(cur_component.insclass))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("insclass"), cur_component.insclass, SA_OVERWRITE, NULL);

   if (strlen(cur_component.gentype))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("gentype"), cur_component.gentype, SA_OVERWRITE, NULL);

   if (strlen(cur_component.machine))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("machine"), cur_component.machine, SA_OVERWRITE, NULL); 

   if (strlen(cur_component.test))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("utest"), cur_component.test, SA_OVERWRITE, NULL); 

   if (strlen(cur_component.marking))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("marking"), cur_component.marking, SA_OVERWRITE, NULL); 

   if (strlen(cur_component.item))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("item"), cur_component.item, SA_OVERWRITE, NULL); 

   if (strlen(cur_component.package))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("package"), cur_component.package, SA_OVERWRITE, NULL); 

   if (strlen(cur_component.pinref))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("pinref"), cur_component.pinref, SA_OVERWRITE, NULL);

   if (strlen(cur_component._pmabar))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_pmabar"), cur_component._pmabar, SA_OVERWRITE, NULL);

   if (strlen(cur_component._main_mark))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_main_mark"), cur_component._main_mark, SA_OVERWRITE, NULL);

   if (strlen(cur_component._sub_mark))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_sub_mark"), cur_component._sub_mark, SA_OVERWRITE, NULL);

   if (strlen(cur_component._sub_mark))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_sub_mark"), cur_component._sub_mark, SA_OVERWRITE, NULL);

   if (strlen(cur_component._term_seq))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_term_seq"), cur_component._term_seq, SA_OVERWRITE, NULL); 

   if (strlen(cur_component._feeder_style))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_feeder_style"), cur_component._feeder_style, SA_OVERWRITE, NULL); 

   if (strlen(cur_component._carry_mode))
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_carry_mode"), cur_component._carry_mode, SA_OVERWRITE, NULL);

   if (cur_component.polarized)
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("polarized"), "", SA_OVERWRITE, NULL);

   if (cur_component._polarized)
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_polarized"), cur_component._polarized, SA_OVERWRITE, NULL);

   if (cur_component._direction)
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_direction"), cur_component._direction, SA_OVERWRITE, NULL);

   if (cur_component._traypickup)
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_traypickup"), cur_component._traypickup, SA_OVERWRITE, NULL); 

   if (cur_component._package)
      doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap("_package"), cur_component._package, SA_OVERWRITE, NULL);

   // combine tolerance if same
   if (cur_component.ttolpos > -1 && cur_component.ttolpos == cur_component.ttolneg)
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_TOLERANCE, TRUE),
               VT_DOUBLE, &(cur_component.ttolpos), SA_OVERWRITE, NULL);
   }
   else if (cur_component.ttolpos > -1)
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE),
               VT_DOUBLE, &(cur_component.ttolpos), SA_OVERWRITE, NULL);
   }
   if (cur_component.ttolneg > -1)
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE),
               VT_DOUBLE, &(cur_component.ttolneg), SA_OVERWRITE, NULL);
   }

   if (strlen(cur_component.tvalue))
      doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_VALUE, cur_component.tvalue, SA_OVERWRITE, NULL);

   return 1;
}

/******************************************************************************
*
*    %partid
*    { {"BA1",1},{"BA2",2},{"BA3",3},{"BA4",4},{"BA5",5},{"BA6",6},{"BA7",7},{"BA8",8},{"BA9",9},
*      {"BA10",10},{"BARCODE1",11},{"BR100",12},{"BR101",13},{"BR321",14},{"BR322",15},{"BR480",16},
*      {"BR501",17},{"BR700",18},{"BR701",19},{"C1",20},{"C2",21},{"C3",22},{"C4",23},{"C5",24},
*      {"C6",25},{"C7",26},{"C8",27},{"C9",28},{"C10",29},{"C11",30},{"C12",31},{"C13",32}
*      {"VDR102",1201},{"VM1",1202},{"VM2",1203},{"VM3",1204},{"VM4",1205},{"VM5",1206},
*      {"VM6",1207},{"X1",1208},{"ZD400",1209},{"ZD401",1210},{"ZD402",1211},{"ZD500",1212},
*      {"ZD501",1213},{"ZD700",1214},{"ZD701",1215},{"ZD730",1216},{"ZD731",1217} }
*/
int uni_partid()
{
   // %partid section is not used.
   // DR 837025
   // Appears to contain mapping of an int ID to refdes. We could save this as
   // an attribute on component insert, but no one has asked for this or anything else.
   // We got a sample that had this section and caused parsing error, so now we
   // recognize and purposely skip the section.

   fnull();

   return 1;
}

/******************************************************************************
*/
int node_pins()
{

   if (!get_tok())
      return p_error();    // get {       
   if (STRCMPI(token,"{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   // pin loop
   while (TRUE)
   {
      if (!get_tok())
         return p_error();    // get {
      if (STRCMPI(token, "{"))
      {
         push_tok();
         break;
      }
   
      if (!get_tok())
         return p_error();    // part  
      CString partname = token;

      if (!get_tok())
         return p_error();    // pin
      CString pinnum = token;

      if (!get_tok())
         return p_error();    // name   
      CString loginame = token;

      if (!get_tok())
         return p_error();    // logical name   

      if (!get_tok())
         return p_error();    // visible  0 none 1 top 2 bottom 3 all

      int visibility = atoi(token);

      if (!get_tok())
         return p_error();    // nailable

      NetStruct *net = add_net(file, cur_netname);
      CompPinStruct *compPin = add_comppin(file, net, partname, pinnum);
      if (strlen(loginame))
         doc->SetUnknownAttrib(&compPin->getAttributesRef(), "LOGINAME", loginame, SA_OVERWRITE, NULL);

      CUnicamComponents::getComponents().fixPadStack(partname,pinnum,visibility);

      // get extra info
      while (TRUE)
      {
         if (!get_tok())
            return p_error();    // get }
         if (!STRCMPI(token,"}"))
            break;
      }
   }

   if (!get_tok())
      return p_error();    // get }

   return 1;
}

/******************************************************************************
* node_vias
*/
int node_vias()
{
   loop_command(via_lst, SIZ_VIA_LST);

   return 1;
}

/******************************************************************************
* node_traces
*/
int node_traces()
{
   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token,"{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   G.cur_layer = Graph_Level("UNROUT", "", 0);
   loop_command(graphic_lst, SIZ_GRAPHIC_LST);

   return 1;
}

/******************************************************************************
* node_nail
*/
int node_nail()
{
   fnull();

   return 1;
}

/******************************************************************************
* node_short
*/
int node_short()
{
   if (!get_tok())
      return p_error();       

   return 1;
}

/******************************************************************************
* netlist_node
*/
static int netlist_node()
{
   clear_compstruct();

   if (!get_tok())
      return p_error();      
   cur_netname = token;

	if (Netlist_Mode == eNetUnconnected)
		cur_netname =  NET_UNUSED_PINS;

   NetStruct *net = add_net(file, cur_netname);

	if (Netlist_Mode == eNetUnconnected)
		net->setFlags(NETFLAG_UNUSEDNET);

   loop_command(nodelist_lst, SIZ_NODELIST_LST);

   cur_netname = "";

   return 1;
}

/******************************************************************************
* get_textpar
*/
static int get_textpar(double *x, double *y, double *rot, char *just)
{
   if (!get_tok())
      return p_error();    // get {       
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // x   
   *x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   *y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // rot   
   *rot = atof(token) / 10;

   if (!get_tok())
      return p_error();    // just   
   just[0] = token[0];
   just[1] = token[1];

   if (!get_tok())
      return p_error();    // get }       

   return 1;
}

/******************************************************************************
* shape_ref
*/
int shape_ref()
{
   double x, y, rot;
   char just[2];
   get_textpar(&x, &y, &rot, just);

   BlockStruct *block = Graph_Block_Exists(doc, cur_shapename, -1);

   if (block)
   {
      int attlayernr = G.cur_layer;
      doc->SetUnknownVisAttrib(&block->getAttributesRef(), ATT_REFNAME, "", x, y, DegToRad(rot),
            attheight, attwidth, 0, 0, TRUE, SA_OVERWRITE, 0L, attlayernr, 0, 0, 0); 
   }

   return 1;
}

/******************************************************************************
* shape_partno
*/
int shape_partno()
{
   double x, y, rot;
   char just[2];
   get_textpar(&x, &y, &rot, just);

   BlockStruct *block = Graph_Block_Exists(doc, cur_shapename, -1);

   if (block)
   {
      int   attlayernr = G.cur_layer;
      doc->SetUnknownVisAttrib(&block->getAttributesRef(), get_attrmap("partno"), "", x, y, DegToRad(rot),
            attheight, attwidth, 0, 0, TRUE, SA_OVERWRITE, 0L, attlayernr, 0, 0, 0); 
   }

   return 1;
}

/******************************************************************************
* shape_desc
*/
int shape_desc()
{
   double x, y, rot;
   char just[2];
   get_textpar(&x, &y, &rot, just);

   BlockStruct *block = Graph_Block_Exists(doc, cur_shapename, -1);

   if (block)
   {
      int   attlayernr = G.cur_layer;
      doc->SetUnknownVisAttrib(&block->getAttributesRef(), get_attrmap("descr"), "", x, y, DegToRad(rot),
            attheight, attwidth, 0, 0, TRUE, SA_OVERWRITE, 0L, attlayernr, 0, 0, 0); 
   }

   return 1;
}

/******************************************************************************
* shape_comm
*/
int shape_comm()
{
   double x, y, rot;
   char just[2];
   get_textpar(&x, &y, &rot, just);

   BlockStruct *block = Graph_Block_Exists(doc, cur_shapename, -1);

   if (block)
   {
      int   attlayernr = G.cur_layer;
      doc->SetUnknownVisAttrib(&block->getAttributesRef(), get_attrmap("comm"), "", x, y, DegToRad(rot),
            attheight, attwidth, 0, 0, TRUE, SA_OVERWRITE, 0L, attlayernr, 0, 0, 0); 
   }

   return 1;
}

/******************************************************************************
* uni_fiducial
*/
int uni_fiducial()
{
   clear_fiducialstruct();

   if (!get_tok())
      return p_error();    // get shape name   
   cur_fiducial.name = token;
   
   loop_command(fiducial_lst, SIZ_FIDUCIAL_LST);
   
   BlockStruct *block = Graph_Block_On(GBO_APPEND, token, -1, 0);
   block->setBlockType(BLOCKTYPE_FIDUCIAL);
   Graph_Block_Off();

   DataStruct *data = Graph_Block_Reference(cur_fiducial.shapeid, cur_fiducial.name, 0, cur_fiducial.location.x, 
            cur_fiducial.location.y, DegToRad(cur_fiducial.location.rot), cur_fiducial.location.side, 1.0, -1, TRUE);
   data->getInsert()->setInsertType(default_inserttype(block->getBlockType()));

   return 1;
}

/******************************************************************************
* uni_fidlinks
*/
int uni_fidlinks()
{
#ifdef _DEBUG
   fprintf(ferr, "DEBUG: Fidlinks not implemented.\n");
#endif   

   fnull();

   return 1;
}

/******************************************************************************
* uni_netlist
*/
int uni_netlist()
{
	Netlist_Mode = eNetConnected;
   loop_command(netlist_lst, SIZ_NETLIST_LST);

   return 1;
}

/******************************************************************************
* uni_unconnected
*/
int uni_unconnected()
{
	// This is disabled. The new "%unconnected" section in PDW was presented in
	// case 1745. It stopped the parser from continuing on to the "panel" (it did
	// more than that, but that is what the user noticed).
	// In the case data, there are unconnected pins that are not included in the
	// unconnected section of the data. So we find this is either unreliable data, or
	// it means something other than what we intuitively find it to me. So we
	// are going to skip this section. Note that we already build our own
	// "~unused_pins~" net anyway.

#ifdef ADD_UNCONNECTED_NET_TO_CC
	Netlist_Mode = eNetUnconnected;
   loop_command(netlist_lst, SIZ_NETLIST_LST);
#endif

   return 1;
}

/******************************************************************************
* uni_vias_numbered
*/
int uni_vias_numbered()
{
   return 1;
}

/******************************************************************************
* uni_nextpartid
*
* %nextpartid : 1218
*/
int uni_nextpartid()
{
   // The ":" seems to get swallowed up on its own.
   // Swallow the number.
   if (!get_tok())
      return p_error();

   return 1;
}

/******************************************************************************
*/
static void add_pad_to_padstack(BlockStruct *padstackBlock, double insertX, double insertY, int padform, int layers, double drill)
{
   // Determine pad layer index
   int layerindex;
   if ((layers & 3) == 3)
      layerindex = Graph_Level("PAD_ALL", "", 0);
   else if ((layers & 1) == 1)
      layerindex = Graph_Level("PAD_TOP", "", 0);
   else if ((layers & 2) == 2)
      layerindex = Graph_Level("PAD_BOT", "", 0);

   CString padname;
   padname.Format("PADSHAPE_%d", padform);

   // Add pad insert to padstack
   // If padstackBlock is NULL then we are adding to existing open block, if not 
   // NULL then add to that block.

   // Option open block for editing.
   if (padstackBlock != NULL)
      Graph_Block_On(padstackBlock);

   // Add pad insert
   DataStruct *data = Graph_Block_Reference(padname, "", 0, insertX, insertY, 0.0, 0, 1.0, layerindex, TRUE);

   // Add drill to padstack if drill is non-zero
   if (drill > 0)
   {
      int drilllayernum = Graph_Level("DRILLHOLE", "", 0);

      BlockStruct *block2 = Graph_FindTool(drill, 0, TRUE);
      // must be filenum 0, because apertures are global.
      DataStruct* toolData = Graph_Block_Reference(block2->getName(), NULL, -1, insertX, insertY, 0.0, 0, 1.0, drilllayernum, TRUE);
      toolData->getInsert()->setInsertType(insertTypeDrillHole);
   }

   // Option close block for editing.
   if (padstackBlock != NULL)
      Graph_Block_Off();
}

/******************************************************************************
*/
static BlockStruct *make_padstack(int padform, int layers, double drill)
{
   CString padstackName;
   int drilltool = 0;

   if (drill > 0) 
      layers = 3;  // all layers
   //else
   //   layer = 0;  // top always, old PDW defintion, no drill is known

   if (drill > 0)
   {
      BlockStruct *block = Graph_FindTool(drill, 0, TRUE);
      drilltool = block->getBlockNumber();
   }

   padstackName.Format("PADSTACK_%d_%d_%d", padform, layers, drilltool);

   // Get existing if there is one.
   BlockStruct *padstackBlock = Graph_Block_Exists(doc, padstackName, -1);

   // If not existing then create it.
   if (padstackBlock == NULL)
   {
      // Create padstack block
      padstackBlock = Graph_Block_On(GBO_APPEND, padstackName, -1, 0);
      padstackBlock->setBlockType(BLOCKTYPE_PADSTACK);

      add_pad_to_padstack(NULL, 0., 0., padform, layers, drill);

      Graph_Block_Off();
   }

   return padstackBlock;
}

static BlockStruct *make_padstack(ApertureShapeTag padshape, int layers, double xsize, double ysize, double drill)
{
   int padform = get_padformindex(padshape, xsize, ysize);

   return make_padstack(padform,layers,drill);
}

static DataStruct *make_pin_insert(CString pinnr, double x, double y, int layers, ApertureShapeTag shape, double w, double h, double drill)
{
   // See if pin already exists. If yes then check if is unique padstack name already.
   // If not, copy current padstack to make new pin specific padstack, then add this
   // pad.

   // Get pin insert if it already exists.
   CDataList *compDataList = GetCurrentDataList();
   DataStruct *pinInsertData = compDataList->FindInsertData(pinnr, insertTypePin);
   if (pinInsertData != NULL)
   {
      // This pin already has an insert.
      // Arguably a PDW format error, spec says "terminals are to have unique names".
      // But we'll accept it, and consider all such terminals to be adding pads to same pin.
      CString pinSpecificPadstackName;
      pinSpecificPadstackName.Format("%s_PIN_%s", cur_shapename, pinnr);

      double pinInsertX = pinInsertData->getInsert()->getOriginX();
      double pinInsertY = pinInsertData->getInsert()->getOriginY();

      BlockStruct *pinSpecificPadstackBlock = NULL;
      // If pin is already inserting pin-specific padstack then we can just add this
      // pad to that padstack. Otherwise, clone the current padstack to make one
      // with pin-specific name and then add this pad.
      BlockStruct *insertedBlock = doc->getBlockAt(pinInsertData->getInsert()->getBlockNumber());
      if (insertedBlock != NULL)
      {
         if (insertedBlock->getName().CompareNoCase(pinSpecificPadstackName) == 0)
         {
            // Is already pin specific, can use same block.
            pinSpecificPadstackBlock = insertedBlock;
         }
         else
         {
            // Is generic, clone to make pin-specific
            pinSpecificPadstackBlock = Graph_Block_On(SA_APPEND, pinSpecificPadstackName, insertedBlock->getFileNumber(), 0);
            Graph_Block_Copy(insertedBlock, 0., 0., 0., 0, 1., -1, TRUE);
            Graph_Block_Off();

            // Change pin insert to use this new padstack block.
            pinInsertData->getInsert()->setBlockNumber(pinSpecificPadstackBlock->getBlockNumber());
         }

         // Add the new terminal's pad to the padstack
         // Adjust pin origin for pin insert location. I.e. this pad is placed into an existing padstack.
         // The original pad will be centered, and we are not changing the pin's insert location.
         // The xy we get from data is relative to component center, we need to shift it so it is
         // relative to the pin's origin.
         int padform = get_padformindex(shape, w, h);
         add_pad_to_padstack(pinSpecificPadstackBlock, (x - pinInsertX), (y - pinInsertY), padform, layers, drill);
      }
   }
   else
   {
      // New pin, create simple padstack, add pin insert.
      BlockStruct *padstackBlock = make_padstack(shape, layers, w, h, drill);
      pinInsertData = Graph_Block_Reference(padstackBlock->getBlockNumber(), pinnr, x, y, DegToRad(0.0), 0, 1.0, -1);
      pinInsertData->getInsert()->setInsertType(insertTypePin);
   }

   return pinInsertData;
}

/******************************************************************************
* terminal_p1
*/
int terminal_p1()
{
   double x, y, dia, drill = -1;
   int nr;

   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // pin 
   nr = atoi(token);

   if (!get_tok())
      return p_error();    // x   
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // d   
   dia = cnv_unit(token);
   
   if (unicamversion >= VER115)
   {
      if (!get_tok())
         return p_error();    // ???
       drill = cnv_unit(token);
   }

   CString pinnr;
   pinnr.Format("%d", nr);

   //BlockStruct *padstackBlock = make_padstack(T_ROUND, 1, dia, 0.0, drill);

   //DataStruct *data = Graph_Block_Reference(padstackBlock->getBlockNumber(), pinnr, x, y, DegToRad(0.0), 0, 1.0, -1);
   //data->getInsert()->setInsertType(insertTypePin);

   DataStruct *pinInsertData = make_pin_insert(pinnr, x, y, 1 /*layers*/, apertureRound, dia, 0.0, drill);

   // do pinnr here
   if (pinInsertData != NULL)
      doc->SetAttrib(&pinInsertData->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &(pinnr),SA_OVERWRITE, NULL);

   if (!get_tok())
      return p_error();    // get } 

   return 1;
}

/******************************************************************************
* terminal_p2
*/
int terminal_p2()
{
   double x, y, dia, drill = -1;
   int nr;

   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // x 
   nr = atoi(token);

   if (!get_tok())
      return p_error();    // x   
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // d   
   dia = cnv_unit(token);

   if (unicamversion >= VER115)
   {
      if (!get_tok())
         return p_error();    // ???
      drill = cnv_unit(token);
   }

   CString pinnr;
   pinnr.Format("%d", nr);
   
   DataStruct *pinInsertData = make_pin_insert(pinnr, x, y, 1 /*layers*/, apertureSquare, dia, dia, drill);

   // do pinnr here
   if (pinInsertData != NULL)
      doc->SetAttrib(&pinInsertData->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &(pinnr),SA_OVERWRITE, NULL);

   if (!get_tok())
      return p_error();    // get } 

   return 1;
}

/******************************************************************************
* terminal_p3
*/
int terminal_p3()
{
   double x, y, w, h, drill = -1;
   int nr;

   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // pinnr 
   nr = atoi(token);

   if (!get_tok())
      return p_error();    // x   
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // w   
   w = cnv_unit(token);

   if (!get_tok())
      return p_error();    // h   
   h = cnv_unit(token);

   if (unicamversion >= VER115)
   {
      if (!get_tok())
         return p_error();    // ???
       drill = cnv_unit(token);
   }

   CString  pinnr;
   pinnr.Format("%d", nr); 

   DataStruct *pinInsertData = make_pin_insert(pinnr, x, y, 1 /*layers*/, apertureRectangle, w, h, drill);

   // do pinnr here
   if (pinInsertData != NULL)
      doc->SetAttrib(&pinInsertData->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &(pinnr),SA_OVERWRITE, NULL);

   if (!get_tok())
      return p_error();    // get } 

   return 1;
}

/******************************************************************************
* via_p1
*/
int via_p1()
{
   double x, y, dia, drill = -1;
   int nr;

   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // x 
   nr = atoi(token);

   if (!get_tok())
      return p_error();    // x   
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // d   
   dia = cnv_unit(token);

   if (G.visibility == 3)  // via is visible on TOP and Bottom, force a drill
      drill = dia/2;

   if (unicamversion >= VER115)
   {
      if (!get_tok())
         return p_error();    // ???
   }

   CString pinnr;    
   pinnr.Format("%d", nr);

   BlockStruct *padstackBlock = make_padstack(apertureRound, G.visibility, dia, 0.0, drill);

   DataStruct *data = Graph_Block_Reference(padstackBlock->getBlockNumber(), pinnr, x, y, DegToRad(0.0), 0, 1.0, 2);
   data->getInsert()->setInsertType(insertTypeVia);

   if (strlen(cur_netname))
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING,
            (void *)cur_netname.GetBuffer(0),SA_OVERWRITE, NULL);
   }

   if (!get_tok())
      return p_error();    // get } 

   return 1;
}

/******************************************************************************
* via_p2
*/
int via_p2()
{
   double x, y, dia, drill = -1;;
   int nr;

   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // x 
   nr = atoi(token);

   if (!get_tok())
      return p_error();    // x   
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // d   
   dia = cnv_unit(token);

   if (G.visibility == 3)  // via is visible on TOP and Bottom, force a drill
      drill = dia/2;

   if (unicamversion >= VER115)
   {
      if (!get_tok())
         return p_error();    // ???
   }

   CString pinnr;
   pinnr.Format("%d", nr);

   BlockStruct *padstackBlock = make_padstack(apertureSquare, G.visibility, dia, 0.0, drill);

   DataStruct *data = Graph_Block_Reference(padstackBlock->getBlockNumber(), pinnr, x, y, DegToRad(0.0), 0, 1.0, -1);
   data->getInsert()->setInsertType(insertTypeVia);
   if (strlen(cur_netname))
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, (void *)cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); 
   }

   if (!get_tok())
      return p_error();    // get } 

   return 1;
}

/******************************************************************************
* via_p3
*/
int via_p3()
{
   double x, y, w, h, drill = -1;
   int nr;

   if (!get_tok())
      return p_error();    // get {    
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // pinnr 
   nr = atoi(token);

   if (!get_tok())
      return p_error();    // x   
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y   
   y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // w   
   w = cnv_unit(token);

   if (!get_tok())
      return p_error();    // h   
   h = cnv_unit(token);

   if (G.visibility == 3)  // via is visible on TOP and Bottom, force a drill
   {
      if (w < h)
         drill = w/2;
      else
         drill = h/2;
   }

   if (unicamversion >= VER115)
   {
      if (!get_tok())
         return p_error();    // ???
   }

   CString pinnr;
   pinnr.Format("%d", nr);

   BlockStruct *padstackBlock = make_padstack(apertureRectangle, G.visibility, w, h, drill);

   DataStruct *data = Graph_Block_Reference(padstackBlock->getBlockNumber(), pinnr, x, y, DegToRad(0.0), 0, 1.0, -1);
   data->getInsert()->setInsertType(insertTypeVia);
   if (strlen(cur_netname))
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING,
            (void *)cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); 
   }

   if (!get_tok())
      return p_error();    // get } 

   return 1;
}

/******************************************************************************
* uni_terminals
*/
int uni_terminals()
{
   BlockStruct *block = Graph_Block_Exists(doc, cur_shapename, -1);
   if (block)
      block->setBlockType(BLOCKTYPE_PCBCOMPONENT); // <== this is only a PCBCOMPONENT if it has pins

   loop_command(terminal_lst, SIZ_TERMINAL_LST);

   return 1;
}

/******************************************************************************
* gra_arc

   ellipse inside a box
*/
int gra_arc()
{
   if (!get_tok())
      return p_error();    // get { or [ 
   if (!STRCMPI(token, "["))
   {
      while (TRUE)
      {
         push_tok();
         go_command(glb_lst, SIZ_GLB_LST);   // makes a push_tok on the end
         Push_tok = FALSE;

         if (!get_tok())
            return p_error();    // next  
         if (!STRCMPI(token, "{"))
            break;
      }
   }

   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // x1   
   double x1 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y1   
   double y1 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // x2   
   double x2 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y2   
   double y2 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // startangle   
   double sa = atof(token) / 10;

   if (!get_tok())
      return p_error();    // deltaangle   
   double da = atof(token) / 10;

   double cx = (x1 + x2)/2;
   double cy = (y1 + y2)/2;
   double r = fabs(x2 - x1) / 2;

   DataStruct *data = NULL;

   if (da == 360)
   {
      data = Graph_Circle(G.cur_layer, cx, cy, r, 0l, G.cur_widthindex, 0, 0);
   }
   else
   {
      double bulge = tan(DegToRad(da)/4);
      double xs = cx + r * cos(DegToRad(sa));
      double ys = cy + r * sin(DegToRad(sa));
      double xe = cx + r * cos(DegToRad(sa+da));
      double ye = cy + r * sin(DegToRad(sa+da));

      data = Graph_PolyStruct(G.cur_layer, 0, 0);
      Graph_Poly(NULL, G.cur_widthindex, 0 , 0, 0);   // fillflag , negative, closed
      Graph_Vertex(xs, ys, bulge);
      Graph_Vertex(xe, ye, 0.0);
   }

   if (data != NULL && curGraphicClass != graphicClassNormal)
      data->setGraphicClass(curGraphicClass);

   if (data != NULL && strlen(cur_netname))
   {
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING,
            (void *)cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
   }

   if (!get_tok())
      return p_error();    // get }       

   return 1;
}

/******************************************************************************
* gra_pline
*/
static int gra_pline()
{
   double   x, y;

   if (!get_tok())
      return p_error();    // get {    

   if (!STRCMPI(token, "["))
   {
      while (TRUE)
      {
         push_tok();
         go_command(glb_lst, SIZ_GLB_LST);   // makes a push_tok on the end
         Push_tok = FALSE;

         if (!get_tok())
            return p_error();    // next  
         if (!STRCMPI(token,"{"))
            break;
      }
   }

   if (!get_tok())
      return p_error();    // int  

   DataStruct *data = Graph_PolyStruct(G.cur_layer, 0, 0);

   if (curGraphicClass != graphicClassNormal)
      data->setGraphicClass(curGraphicClass);

   if (strlen(cur_netname))
   {
      data->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING,
            (void *)cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); 
   }

   Graph_Poly(NULL, G.cur_widthindex, 0 , 0, 0);   // fillflag , negative, closed

   if (!get_tok())
      return p_error();    // get {  

   while (!STRCMPI(token, "{"))
   {
      if (!get_tok())
         return p_error();    // y1   
      x = cnv_unit(token);

      if (!get_tok())
         return p_error();    // x2   
      y = cnv_unit(token);

      if (!get_tok())
         return p_error();    // get }       

      Graph_Vertex(x, y, 0.0);

      if (!get_tok())
         return p_error();    // get }       
   }

   return 1;
}

/******************************************************************************
* get_textfont
*/
static int get_textfont( char *font, double *size)
{
   if (!get_tok())
      return p_error();    // get {       
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // font  
   strcpy(font, token);

   if (!get_tok())
      return p_error();    // ??   
   if (!get_tok())
      return p_error();    // size
   *size = cnv_unit(token);
   
   if (!get_tok())
      return p_error();    // get }       

   return 1;
}

/****************************************************************************/
/*
      { T[B] {{1905000,1905000,0},CC,10,{"Arial","",3200400}, "1"} }
*/
int gra_text()
{
   if (!get_tok())
      return p_error();    // get {    

   if (!STRCMPI(token, "["))
   {
      while (TRUE)
      {
         push_tok();
         go_command(glb_lst, SIZ_GLB_LST);   // makes a push_tok on the end
         Push_tok = FALSE;

         if (!get_tok())
            return p_error();    // next  
         if (!STRCMPI(token,"{"))
            break;
      }
   }

   if (!get_tok())
      return p_error();    // get {
   if (!get_tok())
      return p_error();    // x
   double x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y  
   double y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // rotation
   double rot = atof(token);
   
   if (!get_tok())
      return p_error();    // get }       
   if (!get_tok())
      return p_error();    // get CC text just     
   if (!get_tok())
      return p_error();    // get ???      
   
   // {"Arial","",3200400}
   double size;
   char font[255];
   get_textfont(font, &size);

   CString  prosa;
   if (!get_tok())
      return p_error();    // get prosa
   prosa = token;
   prosa.TrimLeft();
   prosa.TrimRight();

   if (strlen(prosa))
   {
      int mir = 0;
      int prop = TRUE;

      DataStruct *data = Graph_Text(G.cur_layer, prosa, x, y, size, size*TEXT_RATIO, DegToRad(rot), 0, prop, mir, 0, FALSE, -1, 0); 
   }

   if (!get_tok())
      return p_error();    // get }    

   return 1;
}

/******************************************************************************
* gra_line
*/
int gra_line()
{
   if (!get_tok())
      return p_error();    // get {    

   if (!STRCMPI(token, "["))
   {
      while (TRUE)
      {
         push_tok();
         go_command(glb_lst, SIZ_GLB_LST);   // makes a push_tok on the end
         Push_tok = FALSE;

         if (!get_tok())
            return p_error();    // next  
         if (!STRCMPI(token,"{"))
            break;
      }
   }

   DataStruct *data = Graph_PolyStruct(G.cur_layer, 0, 0);

   if (curGraphicClass != graphicClassNormal)
      data->setGraphicClass(curGraphicClass);

   if (strlen(cur_netname))
   {
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING,
            (void *)cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
   }

   Graph_Poly(NULL, G.cur_widthindex, 0 , 0, 0);   // fillflag , negative, closed

   if (!get_tok())
      return p_error();    // x
   double x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y  
   double y = cnv_unit(token);

   Graph_Vertex(x, y, 0.0);

   if (!get_tok())
      return p_error();    // x
   x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y  
   y = cnv_unit(token);

   Graph_Vertex(x, y, 0.0);

   if (!get_tok())
      return p_error();    // get }       

   return 1;
}

/******************************************************************************
* gra_circle
*/
int gra_circle()
{
   if (!get_tok())
      return p_error();    // get {    

   if (!STRCMPI(token, "["))
   {
      while (TRUE)
      {
         push_tok();
         go_command(glb_lst, SIZ_GLB_LST);   // makes a push_tok on the end
         Push_tok = FALSE;

         if (!get_tok())
            return p_error();    // next  
         if (!STRCMPI(token, "{"))
            break;
      }
   }

   if (!get_tok())
      return p_error();    // y1   
   double x = cnv_unit(token);

   if (!get_tok())
      return p_error();    // x2   
   double y = cnv_unit(token);

   if (!get_tok())
      return p_error();    // r       
   double r = cnv_unit(token);

   DataStruct *data = Graph_PolyStruct(G.cur_layer, 0, 0);
   Graph_Poly(NULL, G.cur_widthindex, 0 , 0, 1);   // fillflag , negative, closed
   Graph_Vertex(x-r, y, 1.0);
   Graph_Vertex(x+r, y, 1.0);
   Graph_Vertex(x-r, y, 0.0);

   if (!get_tok())
      return p_error();    // get }      

   return 1;
}

/******************************************************************************
* gra_rectangle
*/
int gra_rectangle()
{
   if (!get_tok())
      return p_error();    // get {    

   if (!STRCMPI(token, "["))
   {
      while (TRUE)
      {
         push_tok();
         go_command(glb_lst, SIZ_GLB_LST);   // makes a push_tok on the end
         Push_tok = FALSE;

         if (!get_tok())
            return p_error();    // next  
         if (!STRCMPI(token,"{"))
            break;
      }
   }

   if (!get_tok())
      return p_error();    // y1   
   double x1 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // x2   
   double y1 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // y1   
   double x2 = cnv_unit(token);

   if (!get_tok())
      return p_error();    // x2   
   double y2 = cnv_unit(token);

   DataStruct *data = Graph_PolyStruct(G.cur_layer, 0, 0);
   Graph_Poly(NULL, G.cur_widthindex, 0 , 0, 1);   // fillflag , negative, closed
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

   if (!get_tok())
      return p_error();    // get }      

   return 1;
}

/******************************************************************************
* uni_version
*/
int uni_version()
{
   if (!get_tok())
      return p_error();    // get {       
   if (STRCMPI(token, "{"))
   {
      fprintf(ferr, "{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())
      return p_error();    // major   
   u_version.major = atoi(token);

   if (!get_tok())
      return p_error();    // minor   
   u_version.minor = atoi(token);

   if (!get_tok())
      return p_error();    // revision    
   u_version.revision = atoi(token);

   if (u_version.major == 1 && u_version.minor == 1 && u_version.revision == 4)
      unicamversion = VER114;
   else if (u_version.major == 1 && u_version.minor == 1 && u_version.revision == 5)
      unicamversion = VER115;
   else if (u_version.major == 1 && u_version.minor == 2 && u_version.revision == 0)
      unicamversion = VER115;
   else
   {
      CString tmp;
      tmp.Format("UNICAM Software Revision %d %d %d not supported.", u_version.major, u_version.minor, u_version.revision);
      ErrorMessage(tmp, "UNICAM Version Error", MB_OK | MB_ICONHAND);
      return -1;
   }

   if (!get_tok())
      return p_error();    // get }       

   return 0;
}

/******************************************************************************
* uni_userdata
*/
int uni_userdata()
{
   return fskip();
}

/******************************************************************************
* uni_feeders
*/
int uni_feeders()
{
   return fskip();
}

/******************************************************************************
* uni_useshapeorg
*/
int uni_useshapeorg()
{
   if (!get_tok())
      return p_error();    //     

   return 0;
}

/******************************************************************************
* uni_ecn
*/
int uni_ecn()
{
   return fskip();
}

/******************************************************************************
* uni_deleted
*/
int uni_deleted()
{
   return fskip();
}

/******************************************************************************
* uni_options
*/
int uni_options()
{
   return loop_command(options_lst,SIZ_OPTIONS_LST);
}

/******************************************************************************
* uni_format
*/
int uni_format()
{
   if (!get_tok())
      return p_error();    //        

   return 0;
}

/******************************************************************************
* uni_unit
*/
int uni_unit()
{
   if (!get_tok())
      return p_error();    //     

   return 0;
}

/******************************************************************************
* uni_end
*/
int uni_end()
{
   return -2;
}

/******************************************************************************
* glb_width
   Set current line width.
*/
static int glb_width()
{
   if (!get_tok())
      return p_error();

   int err;
   G.cur_width = cnv_unit(token);
   if (G.cur_width == 0)
      G.cur_widthindex = 0;   // make it small width.
   else
      G.cur_widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   return 1;
}

/******************************************************************************
* glb_visibility
*/
static int glb_visibility()
{
   if (!get_tok())
      return p_error();
   G.visibility = atoi(token);

   return 1;
}

/******************************************************************************
* glb_pcblevel
   Set current layer
*/
static int glb_pcblevel()
{
   if (!get_tok())
      return p_error();

   G.cur_layer = Graph_Level(token, "", 0); 
   update_pcblevel(atoi(token));

   return 1;
}

/******************************************************************************
* generate_padstacks
*/
static int generate_padstacks(double tolerance)
{
/*
   int   i, ii, t;
   int   cur_id = 0;
   GPCB_padstack  tmppadstack[MAX_PADLAYER];
   int   tmppadstackcnt;

   // here put all padstack together which are potential connected
   for (i=0;i<appkoocnt;i++)
   {
      if (appkooarray[i]->curid > -1)  continue; // do not do one which is used
      appkooarray[i]->curid = ++cur_id;

      for (ii=i+1;ii<appkoocnt;ii++)
      {
         if (appkooarray[ii]->curid > -1) continue; // do not do one which is used

         if (fabs(appkooarray[i]->x - appkooarray[ii]->x) < tolerance &&
             fabs(appkooarray[i]->y - appkooarray[ii]->y) < tolerance &&
             appkooarray[i]->drill)
         {
            appkooarray[ii]->curid = appkooarray[i]->curid;
         }
      }
   }

   // sort by layer
   sort_appkoo_by_layer();

   // collect all same padstack ids
   for (i=0;i<appkoocnt;i++)
   {
      if (appkooarray[i]->padstackid > -1)   continue; // do not do one which is used
      if (appkooarray[i]->curid < 0)         continue; // do not do one which is used
      tmppadstackcnt = 0;
      tmppadstack[tmppadstackcnt].layer = appkooarray[i]->layer;
      tmppadstack[tmppadstackcnt].bnum = appkooarray[i]->bnum;
      tmppadstack[tmppadstackcnt].index = i;
      tmppadstackcnt++;

      for (ii=i+1;ii<appkoocnt;ii++)
      {
         if (appkooarray[ii]->curid != appkooarray[i]->curid)  
            continue; // do not do one which is used

         tmppadstack[tmppadstackcnt].layer = appkooarray[ii]->layer;
         tmppadstack[tmppadstackcnt].bnum = appkooarray[ii]->bnum;
         tmppadstack[tmppadstackcnt].index = ii;
         tmppadstackcnt++;
      }

      CString  pname;
      pname.Format("%d",tmppadstackcnt);

      // here is now a tmp padstack, which says how many layers one padstack has.
      for (t=0;t<tmppadstackcnt;t++)
      {
         CString  tmp;
         tmp.Format(",%d,%d",
            tmppadstack[t].layer ,tmppadstack[t].bnum);
         pname += tmp;
      }
      
      int padnameptr = get_padnameptr(pname);
      // check against an existing padstack

      pname.Format("GERBER_PST_%d",padnameptr);
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,pname,-1,0);
      Graph_Block_Off();

      // place i
      DataStruct *d = Graph_Block_Reference(pname, "", 0, 
               appkooarray[i]->x, appkooarray[i]->y, 0.0, 0 , 1.0, -1, TRUE);

      // mark all found with the padstack id.
      for (t=0;t<tmppadstackcnt;t++)
         appkooarray[tmppadstack[t].index]->padstackid = padnameptr; 
   }

   save_padstacks();

   for (i=0;i<appkoocnt;i++)
   {
      fprintf(flog,"X %g Y %g Entity %ld Layer %d Pad %d Block %d Drill %d Padname %s\n",
         appkooarray[i]->x, appkooarray[i]->y, appkooarray[i]->getEntityNumber(), 
         appkooarray[i]->layer, appkooarray[i]->padstackid, appkooarray[i]->bnum,
         appkooarray[i]->drill,
         (appkooarray[i]->padstackid > -1)?padnamearray[appkooarray[i]->padstackid]->stackname:"NULL");
   }
*/
   return 1;

}

/******************************************************************************
* tok_search
   Search for the token in a command token list.
*/
int tok_search(List *tok_lst, int tok_size)
{
   int retval = -1;
   CString searchToken(token);

   for (int i = 0;i < tok_size;++i)
   {
      CString listToken(tok_lst[i].token);

      if (listToken.CompareNoCase(searchToken) == 0)
      {
         retval = i;
         break;
      }
   }

   return retval;
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
         if (!get_line(cur_line))
            return(FALSE);
         else
            cur_new = true;

      cur_new = false;
   }
   else
      Push_tok = FALSE;

   return(TRUE);
}

/******************************************************************************
* push_tok
   Push back last token.
*/
static int push_tok()
{
   return (Push_tok = TRUE);
}

/******************************************************************************
* isdelimeter
   Unicam delimeters are whitespace and comma
*/
static int isdelimeter(char c)
{
   if (isspace(c))
      return TRUE;
   if (c == ':')
      return TRUE;

   return FALSE;
}

/******************************************************************************
* get_next
   Get the next token from line.
*/
static int get_next(CString &lp, bool newlp)
{
   token_name = FALSE;

   static char  *cp;
   static CString cpbuf;

   if (newlp)
   {
      cpbuf.ReleaseBuffer();
      cpbuf = lp;
      cp = cpbuf.GetBuffer(0);
   }
   
   token.Empty();
   for (; isdelimeter(*cp) && *cp != '\0'; ++cp) ;
   // here kill the next, but only 1 comma {"j21",21,,,3}
   if (*cp == ',')   ++cp;
   // there can be whitespace after the comma
   for (; isspace(*cp) && *cp != '\0'; ++cp) ;  

   switch(*cp)
   {
      case '\0':
      case '$':
         return 0;
      case '{':
      case '}':
      case '[':
      case ']':
         token += *(cp++);
      break;
      case '\"':
         //token[i] = '\0';
         token_name = TRUE;
         for (++cp; *cp != '\"'; ++cp)
         {
            if (*cp == '\n')
            {
               /* text spans over 1 line */
               if (!get_line(cur_line))
               {
                  fprintf(ferr, "Error in Line Read at %ld\n", ifp_line);
                  return -1;
               }

               cpbuf.ReleaseBuffer();
               cpbuf = cur_line;
               cp = cpbuf.GetBuffer(0);

               /* the quote can be the first character in the new line */
               if (*cp == '"' && token.Right(1) != '\\')
                  break;
               if (*cp == '\\')
               {
                  ++cp;
                  if (*cp == 'b')
                     token += ' ';
                  else
                     token += *cp;
               }
               else
                  token += *cp;
            }
            else if (*cp == '\\')
            {
               ++cp;
               if (*cp == '\n')
               {
                  /* text spans over 1 line */
                  if (!get_line(cur_line))
                  {
                     fprintf(ferr, "Error in Line Read\n");
                     return -1;
                  }
                  cpbuf.ReleaseBuffer();
                  cpbuf = cur_line;
                  cp = cpbuf.GetBuffer(0);
               }
               if (*cp == 'b')
                  token += ' ';
               else
                  token += *cp;
            }
            else if (*cp != '\0')
               token += *cp;
         }
         ++cp;
      break;
      default:
         for (; !isdelimeter(*cp) && *cp != ',' &&
                           *cp != '{' && *cp != '}' &&
                           *cp != '[' && *cp != ']' &&
                           *cp != '\0'; ++cp)
         {
            if (*cp == '\\')
               ++cp;
            token += *cp;
         }
      break;
   }
/*
   if (!i && !token_name)
      return(FALSE);
*/
   //token[i] = '\0';
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

/******************************************************************************
* get_line
   Get a line from the input file.
*/
static int get_line(CString &buf)
{
   do
   {
      if (!fgetcs(buf, ifp))
      {
         if (!feof(ifp))
         {
            fprintf(ferr, "Read error\n");
            return -1;
         }
         else
         {
            buf.Empty();
            return FALSE;
         }
      }

      ifp_line++;

      buf.Trim();

      if (((ifp_line % 200) == 1) && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }

   } while (buf.IsEmpty());

   return TRUE ;
}

/******************************************************************************
* p_error
   Parsing error.
*/
int p_error()
{
   CString  tmp;
   CString  t;
   tmp.Format("Fatal Parsing Error : Token \"%s\" on line %ld.", token, ifp_line);
   t  = tmp;
   tmp.Format("\nA Parsing Error indicates a corrupt or incompatible UNICAM file.");
   t += tmp;
   
   ErrorMessage(t, "Fatal UNICAM Read Error", MB_OK | MB_ICONHAND);
   
   return -1;
}

/******************************************************************************
* free_all_mem
*/
void free_all_mem()
{
   return;
}

/******************************************************************************
* cnv_unit
*/
double   cnv_unit(const char  *x)
{
   double   faktor;

   switch (unit_flag)
   {
   case  'I':
      /* inch */
      faktor = Units_Factor(UNIT_INCHES, PageUnits);
      break;
   case  'M':
      /* metric */
      faktor = Units_Factor(UNIT_MM, PageUnits);
      break;
   case  'P':  /* pads */
   case  'A':
      /* mil */
      faktor = Units_Factor(UNIT_MILS, PageUnits);
      break;
   case  'B':
      /* basic */
      faktor = Units_Factor(UNIT_MM, PageUnits);
      faktor = faktor / 1000000.0 * 2/3;
      break;
   }

   return(atof(x) * faktor);
}

/******************************************************************************
* assign_layer
*/
int assign_layer()
{
   int   lptr;
   LayerStruct *l;

   Graph_Level("SILKSCREEN_TOP", "", 0); 
   Graph_Level("SILKSCREEN_BOT", "", 0); 

   Graph_Level_Mirror("SILKSCREEN_TOP", "SILKSCREEN_BOT", "");

   Graph_Level("PAD_TOP", "", 0); 
   Graph_Level("PAD_BOT", "", 0); 

   Graph_Level_Mirror("PAD_TOP", "PAD_BOT", "");

   lptr = Graph_Level("SILKSCREEN_TOP", "", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_SILK_TOP);
   lptr = Graph_Level("SILKSCREEN_BOT", "", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_SILK_BOTTOM);

   lptr = Graph_Level("PAD_ALL", "", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_ALL);
   lptr = Graph_Level("PAD_TOP", "", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_TOP);
   lptr = Graph_Level("PAD_BOT", "", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   lptr = Graph_Level("BOARD", "", 0); 
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_BOARD_OUTLINE);


   // pcblevel has all layers. 1 = top rest is sorted by highest number
   int i;
   UPcblevel lpcb;
   int highest = 1;

   // find highest
   for (i=0; i<pcblevelcnt; i++)
   {
      lpcb = pcblevelarray.ElementAt(i);
      if (lpcb.level > highest)  
         highest = lpcb.level;
   }

   int stackcnt = 0;
   for (i=1; i<=highest; i++)
   {  
      CString lname;
      LayerStruct *l;

      lname.Format("%d", i);

      if (i == 1)
      {
         // top
         if ((l = doc->FindLayer_by_Name(lname)) == NULL)
            continue;
         stackcnt++;
         l->setElectricalStackNumber(stackcnt);
         l->setLayerType(LAYTYPE_SIGNAL_TOP);

         if ((l = doc->FindLayer_by_Name("PAD_TOP")) == NULL)
            continue;
         l->setElectricalStackNumber(stackcnt);
      }
      else if (i == highest)
      {
         // bottom
         if ((l = doc->FindLayer_by_Name(lname)) == NULL)
            continue;
         stackcnt++;
         l->setElectricalStackNumber(stackcnt);
         l->setLayerType(LAYTYPE_SIGNAL_BOT);

         if ((l = doc->FindLayer_by_Name("PAD_BOT")) == NULL)
            continue;
         l->setElectricalStackNumber(stackcnt);
      }
      else
      {
         // inner
         if ((l = doc->FindLayer_by_Name(lname)) == NULL)
            continue;
         stackcnt++;
         l->setElectricalStackNumber(stackcnt);
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
   }

   return 1;
}

/******************************************************************************
* load_unicamsettings
*/
static int load_unicamsettings(const CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;

   keep_unrout_layer = FALSE;
   ComponentSMDrule = 0;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "UNICAM Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      CString w = line;
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         // here do it.
         if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRCMPI(lp, ".UNROUTLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
               keep_unrout_layer = TRUE;
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString uniattr = _strupr(lp);
            uniattr.TrimLeft();
            uniattr.TrimRight();
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString ccattr = lp;
            ccattr.TrimLeft();
            ccattr.TrimRight();

            UAttrmap *c = new UAttrmap;
            attrmaparray.SetAtGrow(attrmapcnt++, c);  
            c->uni_name = uniattr;
            c->cc_name = ccattr;
         }
         else if (!STRCMPI(lp, ".UseCLocation"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
				UseCLocation = TRUE;
			else
				UseCLocation = FALSE;
         }
      }
   }

   fclose(fp);

   return 1;
}

//_____________________________________________________________________________
CUnicamComponents* CUnicamComponents::m_unicamComponents = NULL;

CUnicamComponents& CUnicamComponents::getComponents()
{
   if (m_unicamComponents == NULL)
   {
      m_unicamComponents = new CUnicamComponents();
   }

   return *m_unicamComponents;
}

void CUnicamComponents::empty()
{
   delete m_unicamComponents;

   m_unicamComponents = NULL;
}

CUnicamComponents::CUnicamComponents()
{
}

void CUnicamComponents::setAt(const CString& refDes,DataStruct& component)
{
   m_components.SetAt(refDes,&component);
}

void CUnicamComponents::fixPadStack(const CString& refDes,const CString& pinNumberStr,int visibility)
{
   bool pinTopFlag    = ((visibility & 1) != 0);
   bool pinBottomFlag = ((visibility & 2) != 0);
   bool thFlag        = pinTopFlag && pinBottomFlag;

	int pinNumber = atoi(pinNumberStr);

   DataStruct* component = NULL;

   if (m_components.Lookup(refDes,component))
   {
      if (component != NULL)
      {
			// Case 1727 -- Apply "pad fix" only to potential THRU components, not to SMD
			Attrib *attrib = NULL;
			WORD insclassKey = doc->RegisterKeyWord("insclass", 0, VT_STRING);
			if (component->getAttributes()->Lookup(insclassKey, attrib) && attrib != NULL)
			{
				CString insclassStr = attrib->getStringValue();
				insclassStr.MakeUpper();
				if (insclassStr.Find("SMD") > -1)
					return;
			}

         BlockStruct* componentGeometry = doc->getBlockAt(component->getInsert()->getBlockNumber());

         bool componentTopFlag = ! component->getInsert()->getPlacedBottom();

         for (POSITION pinPos = componentGeometry->getHeadDataInsertPosition();pinPos != NULL;)
         {
            DataStruct* pin = componentGeometry->getNextDataInsert(pinPos);
				
				// Case 1750, was applying this to all pins in list, should be only pin
				// designated in arg list, otherwise part that is mix of SMD and TRHU pins
				// (as in case 1750) gets made into all THRU pins.
				int insertPinNumber = atoi(pin->getInsert()->getRefname());
            if (pin->getInsert()->getInsertType() == insertTypePin && insertPinNumber == pinNumber)
            {
               BlockStruct* padStackGeometry = doc->getBlockAt(pin->getInsert()->getBlockNumber());
               CSupString padStackName = padStackGeometry->getName();
               CStringArray params;

               if (padStackName.Parse(params,"_") == 4)
               {
                  int padForm         = atoi(params[1]);
                  int layers          = atoi(params[2]);

                  bool padTopFlag,padBottomFlag;

                  if (componentTopFlag)
                  {
                     padTopFlag    = ((layers & 1) != 0);
                     padBottomFlag = ((layers & 2) != 0);
                  }
                  else
                  {
                     padTopFlag    = ((layers & 2) != 0);
                     padBottomFlag = ((layers & 1) != 0);
                  }

                  if (pinTopFlag && !padTopFlag || pinBottomFlag && !padBottomFlag)
                  {
                     BlockStruct* padGeometry = NULL;
                     DataStruct* pad;

                     for (POSITION padPos = padStackGeometry->getHeadDataInsertPosition();padPos != NULL;)
                     {
                        pad = padStackGeometry->getNextDataInsert(padPos);

                        padGeometry = doc->getBlockAt(pad->getInsert()->getBlockNumber());

                        if (padGeometry->isAperture())
                        {
                           break;
                        }

                        padGeometry = NULL;
                     }

                     if (padGeometry != NULL)
                     {
                        double drillSize = .5 * padGeometry->getSizeA();
                        BlockStruct* thruHolePadStackBlock = make_padstack(padForm,3,drillSize);

                        if (thruHolePadStackBlock != NULL)
                        {
                           pin->getInsert()->setBlockNumber(thruHolePadStackBlock->getBlockNumber());
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

/*Ende ***********************************************************************/

