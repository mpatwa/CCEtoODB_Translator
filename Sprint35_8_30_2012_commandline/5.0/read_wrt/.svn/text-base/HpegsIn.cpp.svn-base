// $Header: /CAMCAD/4.6/read_wrt/HpegsIn.cpp 14    2/14/07 4:07p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include <math.h>
#include "gauge.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
extern   CProgressDlg *progress;

#define  EGSERR      "egs.log"

/*Defines *******************************************************************/

#define  TRUE        1
#define  FALSE       0

#define  MAX_TOK     80
#define  MAX_LINE    64000    // large poly lines will cause a large command line
#define  MAX_POLY    10000
#define  MAX_LAYERS  255

#define  DETAIL      68
#define  SYMBOLIC    83
#define  INTERCONN   73
#define  LSTYLE      76
#define  COLOR       67
#define  PEN         80
#define  BOUND       66

#define  ADD_DIM     1           // last_add flag

#define  ARC         'A'   //65
#define  CIRCLE      'C'   //67
#define  DIMENSION   'D'   //68
#define  HATCH       'H'   //72
#define  LINE        'L'   //76
#define  MARKER      'M'   //77
#define  OVAL        'O'   //79
#define  POLYGON     'P'   //80
#define  RECTANGLE   'R'   //82
#define  EGS_TEXT    'T'   //84
#define  NOTE        'N'   //78

#define  ASSOCIATE   'A'
#define  WIDTH       'W'   //87
#define  RES         'R'   //82
#define  FILL        'F'   //70
#define  FONT        'F'   //70
#define  STEP        'I'   //73
#define  ANGLE       'R'   //82
#define  CONSTR      'C'   //67
#define  SFACTOR     'X'   //88
#define  LEADER      'L'   //76
#define  MIRROR      'M'   

/*Typedef *******************************************************************/

typedef struct
{
  CString   drawing_name;
} EGSFilelist;
typedef CTypedPtrArray<CPtrArray, EGSFilelist*> CFilelistArray;

typedef struct
{
  char  *layer_name;
  int   layer;
} Layerlist;

typedef struct
{
  char *command;
} EGS_Command;

typedef  struct
{
  char  *token;
  int   (*function)();
} List;


/* Start of EGS-File file.  */
static   int   egs_add();
static   int   egs_equ();
static   int   egs_grid();
static   int   egs_level();
static   int   egs_resolution();
static   int   egs_save();
static   int   egs_show();
static   int   egs_lock();
static   int   egs_tslant();
static   int   egs_window();
static   int   egs_begin();
static   int   egs_end();
static   int   egs_edit();
static   int   egs_dim_fsize();
static   int   egs_dim_tfsize();
static   int   egs_dim_tslant();
static   int   egs_dim_dec();
static   int   egs_dim_scale();
static   int   egs_dim_radix();
static   int   egs_dim_extend();
static   int   egs_dim_dir();
static   int   egs_dim_units();
static   int   egs_rem();
static   int   egs_fsize();
static   int   egs_decimals();
static   int   egs_iname();
static   int   egs_exit();
static   int   egs_units();
static   int   egs_files();

static   List  egs_lst[] =
{
   "ADD",               egs_add,
   "EQU",               egs_equ,
   "GRID",              egs_grid,
   "LEVEL",             egs_level,
   "RESOLUTION",        egs_resolution,
   "SAVE",              egs_save,
   "SHOW",              egs_show,
   "TSLANT",            egs_tslant,
   "WINDOW",            egs_window,
   "BEGIN_ASSOC",       egs_begin,
   "END_ASSOC",         egs_end,
   "LOCK",              egs_lock,
   "EDIT",              egs_edit,
   "DIM_FSIZE",         egs_dim_fsize,
   "DIM_TFSIZE",        egs_dim_tfsize,
   "DIM_TSLANT",        egs_dim_tslant,
   "DIM_DEC",           egs_dim_dec,
   "DIM_SCALE",         egs_dim_scale,
   "DIM_RADIX",         egs_dim_radix,
   "DIM_EXTEND",        egs_dim_extend,
   "DIM_DIR",           egs_dim_dir,
   "DIM_UNITS",         egs_dim_units,
   "$$",                egs_rem,
   "FSIZE",             egs_fsize,
   "DECIMALS",          egs_decimals,
   "INAME",             egs_iname,
   "TNAME",             egs_iname,
   "EXIT",              egs_exit,
   "UNITS",             egs_units,
   "$FILES",            egs_files,

};

#define  SIZ_EGS_LST             (sizeof(egs_lst) / sizeof(List))

/*Globals *******************************************************************/
/* Function Def */

static   FILE        *ferr;

static   int         saving_to_file = FALSE;
static   void        c_exit();
static   int         go_command(List *,int);
static   int         Get_KeyWord(FILE *fp,char *tmp,int n);
static   long        Get_Linecnt();
static   void        update_linecnt(char c);

static   int         tok_search(List *,int);
static   double      cnv_unit(double);       /* converts EGS any unit into inch */
static   int         get_command(FILE *fp,char *n,int num);
static   int         add_instance(char *lp);
static   int         add_arc(char *lp);  
static   int         add_circle(char *lp);   
static   int         add_dimension(char *lp); 
static   int         add_hatch(char *lp);     
static   int         add_line(char *lp);     
static   int         add_marker(char *lp);    
static   int         add_oval(char *lp);      
static   int         add_polygon(char *lp);   
static   int         add_rectangle(char *lp); 
static   int         add_text(char *lp);      

static   FILE        *ifp; 
static   long        ifp_linecnt;                     /* input file pointer */
static   char        token[MAX_TOK];

static   int         eof_egsfile = FALSE;
static   EGS_Command egs_command;
static   char        *cmd_line;

static   CFilelistArray filelistarray;
static   int         filelist_cnt = 0;

static   Layerlist   *layerlist;
static   int         layerlist_cnt = 0;
static   char        akt_layer[80];
static   int         find_in_layerlist(int);

static   int         find_in_filelist(char *);
static   int         file_nr=-1;
static   int         display_error = 0;
static   int         cur_filenum;
static   int         dimblockcnt;
static   int         last_add;
static   int         begin_assoc;
static   int         assoc_blockon; // was a block opened because of begin_assoc
static   int         block_on;

static   double      scale;
static   int         page_units;

static DataStruct *lastAddedSymbolData = NULL;

/******************************************************************************
* ReadHpEGS
*/
void ReadHpEGS(const char *fname, FormatStruct *Format, double scaleUnitsFactor, int PageUnits)
{
   FileStruct *file;
   int   done = FALSE;
   int   error = FALSE;

   scale = Format->Scale * scaleUnitsFactor;
   display_error = 0;
   page_units = PageUnits;

   if ((ifp = fopen(fname,"rt")) == NULL)
   {
      return;
   }

   CString egsLogFile = GetLogfilePath(EGSERR);
   if ((ferr = fopen(egsLogFile,"wt")) == NULL)
   {
         CString  t;
         t.Format("Error open [%s] file", egsLogFile);
         ErrorMessage(t, "Error");
      return;
   }

   file = Graph_File_Start(fname,Type_HPEGS);
   cur_filenum = file->getFileNumber();   

   if ((cmd_line = (char *)calloc(MAX_LINE,sizeof(char))) == NULL)
   {
      ErrorMessage("No Memory");
      return;
   }

   filelistarray.SetSize(100,100);
   filelist_cnt = 0;

   if ((layerlist = (Layerlist *)calloc(MAX_LAYERS, sizeof(Layerlist))) == NULL)
   {
      ErrorMessage("No Memory");
      return;
   }
   layerlist_cnt = 0;
   dimblockcnt = 0;
   last_add = 0;
   begin_assoc = 0;
   assoc_blockon = 0;
   block_on = 0;

   rewind(ifp);
   eof_egsfile = FALSE;
   ifp_linecnt = 1;  // starts at line 1

   go_command(egs_lst,SIZ_EGS_LST);
   free(cmd_line);

   int i;
   for (i=0;i<filelist_cnt;i++)
   {
      delete filelistarray[i];
   }
   filelistarray.RemoveAll();
   filelist_cnt = 0;

   for(i=0;i<layerlist_cnt;i++)
   {
      free(layerlist[i].layer_name);
   }
   free(layerlist);

   fclose(ifp);

   fclose(ferr); 

   if (display_error)
      Logreader(egsLogFile);

   return;
}

/****************************************************************************/
/*
   Call function associated with next token.
*/
static int go_command(List *tok_lst,int lst_size)
{
   int   i;

   while (Get_KeyWord(ifp, token, MAX_TOK)  && !eof_egsfile)
   {
      if (strlen(token) == 0)    continue;

/* This is a good routine to trap an error - if you have no clue where to look.
fclose(ferr);
ferr = fopen(EGSERR,"at");
fprintf(ferr,"Token [%s] at %ld\n",token,Get_Linecnt());
fclose(ferr);
*/
      if ((i = tok_search(tok_lst,lst_size)) >= 0)
      {
         (*tok_lst[i].function)();
      }
      else
      {
         if (!eof_egsfile)
         {
            fprintf(ferr,"Warning: Structure error in line [%ld], [%s] unknown !\n",
               Get_Linecnt(),token);
            display_error++;
            get_command(ifp,cmd_line,MAX_LINE);
         }
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

   if (eof_egsfile) return(0);

   for (i = 0; i < tok_size; ++i)
      if (!_strnicmp(token,tok_lst[i].token,strlen(tok_lst[i].token)))
    return(i);

   return(-1);
}

/****************************************************************************/
/*
   Get a token but does not skip over end of line

*/
static int get_command(FILE *fp,char *n,int num)
{
   int   cnt = 0;
   int   c;
   int   texton = FALSE;
   char  textstart = ' ';

   while ((c = fgetc(fp)) != EOF)
   {  
      if (c == '"' || c == '`' || c == '\'') 
      {
         if (!texton)   
         {
            textstart = c;
            texton = TRUE;
         }
         else
         {
            if (c == textstart)
               texton = FALSE;
         }
      }
      update_linecnt(c);
      if (!texton && c == ';')   break;
      if (c == '\n' || c == 10 || c == 13)   
         c = ' '; 
      if (cnt < num)
         n[cnt++] = c;
      else
      {
         fprintf(ferr,"Command error\n");
         return 0;
      }
   }

   if (c == EOF)
   {
      eof_egsfile = TRUE;
      return -1;
   }

   n[cnt] = '\0';
   return 1;
}

/****************************************************************************/

static int find_in_filelist(char *lp)
{
   int  i;

   for(i=0; i<filelist_cnt; i++)
   {
      if ((STRCMPI(lp,filelistarray[i]->drawing_name)) == 0)
      {
         // string found in filelist
         return(i);
      }
   }
   return(-1);
}

/****************************************************************************/

static int find_in_layerlist(int layer)
{
   int  i;

   for(i=0; i<layerlist_cnt; i++)
   {
      if (layerlist[i].layer == layer)
      {
    // string found in filelist
    strcpy(akt_layer,layerlist[i].layer_name);
    return(0);
      }
   }
   strcpy(akt_layer,"UNKNOWN");
   return(-1);
}

/****************************************************************************/
/*   FILES definition
*/
static int egs_files()
{
   char      *lp, *cm;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   while ((lp = strtok(cm, " ,$\n")) != NULL)
   {
      EGSFilelist *c = new EGSFilelist;
      filelistarray.SetAtGrow(filelist_cnt,c); 
      filelist_cnt++;
      c->drawing_name = lp;
      cm = NULL;
   }
   return 0;
}

/****************************************************************************/
/*    UNITS; definition
*/
static int egs_units()
{
   char   *lp, *cm, *unit;
   double iupu;  // internal units per user units

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;
   if ((unit = strtok(cm, " ,")) != NULL)
   {
      if ((lp = strtok(NULL, ", ")) != NULL)
      {
         iupu = atof(lp);
      }

      if (!STRCMPI(unit,"MM"))
      {
         scale = Units_Factor(UNIT_MM, page_units);
      }
      else
      if (!STRCMPI(unit,"MIL"))
      {
         scale = Units_Factor(UNIT_MILS, page_units);
      }
      else
      if (!STRCMPI(unit,"INCH"))
      {
         scale = Units_Factor(UNIT_INCHES, page_units);
      }
      else
      {
         fprintf(ferr, "Unknown UNITS [%s,%f] at %ld\n", unit, iupu, ifp_linecnt);
         display_error++;
      }
   }
   else
   {
      fprintf(ferr,"Unit explected at %ld\n",Get_Linecnt());
   }

   return(0);
}

/****************************************************************************/
/*    EXIT; definition
*/
static int egs_exit()
{
   get_command(ifp,cmd_line,MAX_LINE);

   if (block_on > 0 && file_nr > 0)
   {
      Graph_Block_Off();
      file_nr = -1;
      block_on--;
   }
   return(0);
}

/****************************************************************************/
/*     INAME (TNAME) definition
*/
static int egs_iname()
{
    char   *lp, *cm;
    double fsize=0, tvalue=0;
    int    layer, display=0;

    get_command(ifp,cmd_line,MAX_LINE);
    cm = cmd_line;
    lp = strtok(cm, " ");
    cm = NULL;

    if ((lp) && (strlen(lp) > 2))
    {
       lp[0] = '0';
       lp[1] = '0';
       layer = atoi(lp);
    }
    else
    {
       c_exit();
    }

// seek for D (display), F (font size) and T (tag value)

   while ((lp = strtok(NULL, " ,")) != NULL)
   {
//    if lenght < 2 -> EXIT
      if (strlen(lp) < 2)
         c_exit();

//    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case 'D':
               lp[0] = '0';
               lp[1] = '0';
               display = atoi(lp);
            break;
            case 'F':
               lp[0] = '0';
               lp[1] = '0';
               fsize   = atof(lp);
            break;
            case 'T':
               lp[0] = '0';
               lp[1] = '0';
               tvalue  = atof(lp);
            break;
         }
      }
   }

   return(0);
} // end ()

/****************************************************************************/
/*    DECIMALS definition
*/
static int egs_decimals()
{
   char   *lp, *cm;
   double decimals;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      decimals = atof(lp);
   }
   else
      c_exit();

// printf("\nDECIMALS %f",decimals);
   return(0);
}

/****************************************************************************/
/*    DIM_FSIZE definition
*/
static int egs_fsize()
{
   char   *lp, *cm;
   double fsize;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      fsize = atof(lp);
   }
   else
      c_exit();

// printf("\nFSIZE %s", fsize);
   return(0);
}

/****************************************************************************/
/*    REMARK definition
*/
static int egs_rem()
{
   int   c;

   while ((c = fgetc(ifp)) != EOF)
   {
      update_linecnt(c);
      if (c == '\n' || c == 10 || c == 13)   
         break;
   }

   if (c == EOF)
   {
      eof_egsfile = TRUE;
      return -1;
   }

   return(0);
}

/****************************************************************************/
/*    DIM_DIR definition
*/
static int egs_dim_dir()
{
   char   *lp, *cm, *dir;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      dir = (lp);
   }
   else
      c_exit(); 

// printf("\nDIM_DIR %s", dir);
   return(0);
}

/****************************************************************************/
/*    DIM_EXTEND definition
*/
static int egs_dim_extend()
{
   char   *lp, *cm;
   double extend;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      extend = atof(lp);
   }
   else
      c_exit();

// printf("\nDIM_EXTEND %f", extend);
   return(0);
}

/****************************************************************************/
/*    DIM_RADIX definition
*/
static int egs_dim_radix()
{
   char   *cm;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

// printf("\nDIM_RADIX %s", radix);
   return(0);
}

/****************************************************************************/
/*    DIM_UNITS definition
*/
static int egs_dim_units()
{
   char   *lp, *cm, *units;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      units = (lp);
   }
   else
       c_exit();

// printf("\nDIM_UNITS %s", units);
   return(0);
}

/****************************************************************************/
/*    DIM_SCALE definition
*/
static int egs_dim_scale()
{
   char   *lp, *cm;
   double scale;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      scale = atof(lp);
   }
   else
       c_exit();

// printf("\nDIM_SCALE %f",scale);
   return(0);
}

/****************************************************************************/
/*    DIM_DEC definition
*/
static int egs_dim_dec()
{
   char   *lp, *cm;
   double dec;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      dec = atof(lp);
   }
   else
      c_exit();

// printf("\nDIM_DEC %f",dec);
   return(0);
}

/****************************************************************************/
/*    DIM_TSLANT definition
*/
static int egs_dim_tslant()
{
   char   *lp, *cm;
   double tslant;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;


   if ((lp = strtok(cm, " ")) != NULL)
   {
      tslant = atof(lp);
   }
   else
      c_exit(); 

// printf("\nDIM_TSLANT %f",tslant);
   return(0);
}

/****************************************************************************/
/*    DIM_TFSIZE definition
*/
static int egs_dim_tfsize()
{
   char   *lp, *cm;
   double tfsize;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      tfsize = atof(lp);
   }
   else
      c_exit(); 

// printf("\nDIM_TFSIZE %f",tfsize);
   return(0);
}

/****************************************************************************/
/*    DIM_FSIZE definition
*/
static int egs_dim_fsize()
{
   char   *lp, *cm;
   double fsize;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      fsize = atof(lp);
   }
   else
      c_exit();

// printf("\nDIM_FSIZE %f",fsize);
   return(0);
}

/****************************************************************************/
/*    EDIT definition
*/
static int egs_edit()
{
   char   *lp, *cm;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;
   if ((lp = strtok(cm, " ")) == NULL) 
      return 0;
   cm = NULL;

   if (STRCMPI(lp,"EDIT") != 0)
   {
      if (block_on != 0)
      {
         // here is main and no block should be open. If a block is open, 
         // there was no save or exit 
         ErrorMessage("Previous Block - still an open block");
         Graph_Block_Off();
      }

      if ((file_nr = find_in_filelist(lp)) != -1)
      {
         if (file_nr != 0)
         {
            //fprintf(ferr,"Write block [%s]\n",filelist[file_nr].drawing_name);
            Graph_Block_On(GBO_APPEND,filelistarray[file_nr]->drawing_name,-1,0);   
            block_on++;
         }
         else
         {
            //fprintf(ferr,"Write main block...\n");
            if (block_on != 0)
            {
               // here is main and no block should be open. If a block is open, 
               // there was no save or exit 
               ErrorMessage("Main Block - still an open block");
               Graph_Block_Off();
            }
         }
      }
      else
      {
         fprintf(ferr,"Error: Library not found in line [%ld]\n",Get_Linecnt());
      }
   }
   return 0;
}

/****************************************************************************/
/*    SAVE definition
*/
static int egs_save()
{
   get_command(ifp,cmd_line,MAX_LINE);

   if (block_on > 0 && file_nr > 0)
   {
      Graph_Block_Off();
      file_nr = -1;
      block_on--;
   }
   return(0);
}

/****************************************************************************/
/*    BEGIN_ASSOC definition
*/
static int egs_begin()
{  
   // begin of assoc
   if (begin_assoc == 0 && last_add == ADD_DIM)
   {
      CString  dimname;
      dimname.Format("_DIM_%d",dimblockcnt);
      Graph_Block_On(GBO_APPEND,dimname,cur_filenum,0);
      assoc_blockon++;  // block was opened because of begin_assoc
   }
   begin_assoc++;
   return(0);
}

/****************************************************************************/
/*    END definition
*/
static int egs_end()
{  
   // end of assoc
   if (assoc_blockon)   // block was opened in begin_assoc
   {
      Graph_Block_Off();
      assoc_blockon--;
   }

	lastAddedSymbolData = NULL;

   begin_assoc--;
   return(0);
}

/****************************************************************************/
/*    WINDOW definition
*/
static int egs_window()
{
   char   *lp, *cm;
   double left_x, left_y, right_x, right_y;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ,")) != NULL)
   {
      left_x = atof(lp);

      if ((lp = strtok(NULL, " ,")) != NULL)
         left_y = atof(lp);
      else
         c_exit();

      if ((lp = strtok(NULL, " ,")) != NULL)
         right_x = atof(lp);
      else
         c_exit();

      if ((lp = strtok(NULL, " ,")) != NULL)
         right_y = atof(lp);
      else
    c_exit();
   }      
   else
      c_exit();

// printf("\nWINDOW %f,%f %f,%f",left_x,left_y,right_x,right_y);
   return(0);
}

/****************************************************************************/
/*    TSLANT definition
*/
static int egs_tslant()
{
   char   *lp, *cm;
   double tslant;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;


   if ((lp = strtok(cm, " ")) != NULL)
   {
      tslant = atof(lp);
   }
   else
      c_exit();

// printf("\nTSLANT %f",tslant);
   return(0);
}

/****************************************************************************/
/*    SHOW definition
*/
static int egs_show()
{
   egs_rem();
   return(0);
}

/****************************************************************************/
/*    RESOLUTION definition
*/
static int egs_resolution()
{
   char   *lp, *cm;
   int    resolution;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ")) != NULL)
   {
      resolution = atoi(lp);
   }
   else
      c_exit();   

// printf("\nRESOLUTION %i",resolution);
   return(0);
}

/****************************************************************************/
/*    LOCK definition
*/
static int egs_lock()
{
   char    *lp, *cm;
   double  lock;
   int     enforce=0;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;


   while ((lp = strtok(cm, " ")) != NULL)
   {
      if (lp[0] == ':')
         enforce = 1;
      else
         lock = atof(lp);

      cm = NULL;
   }

// printf("\nLOCK %f E%i",lock,enforce);
   return(0);
}

/****************************************************************************/
/*    LEVEL definition
*/
static int egs_level()
{
   char    *lp, *cm;
   int     level;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ,")) != NULL)
   {
      level = atoi(lp);
   }
   else
      c_exit();    

// printf("\nLEVEL %i", level);
   return(0);
}

/****************************************************************************/
/*    GRID definition
*/
static int egs_grid()
{
   char    *lp, *cm;
   //
   // upg      > units per grid point
   // rnodg    > relative number of displayed grid points
   // ali      > aligment point
   //
   double  upg_point, rnodg_point, ali_point_x, ali_point_y;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;

   if ((lp = strtok(cm, " ,")) != NULL)
   {
      upg_point = atof(lp);
      cm = NULL;

      if ((lp = strtok(NULL," ,")) != NULL)
         rnodg_point = atof(lp);
      else
         c_exit();

      if ((lp = strtok(NULL," ,")) != NULL)
         ali_point_x = atof(lp);
      else
         c_exit();

      if ((lp = strtok(NULL," ,")) != NULL)
         ali_point_y = atof(lp);
      else
    c_exit();
   }
   else
       c_exit();

   // printf("\nGRID U%f R%f A%f,%f", upg_point, rnodg_point, ali_point_x, ali_point_y);

   return(0);
}

/****************************************************************************/
/*
*/
static int egs_equ()
{
   char   *lp, *cm;
   int    line_style=0, color_number=0, pen_number=0;
   int    bound_layer=0, layer_number=0;
   int    detail=0, symbolic=0, interconn=0;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;
   lp = strtok(cm, " ");
   cm = NULL;
//
   if (layerlist_cnt >= MAX_LAYERS)
   {
      fprintf(ferr,"Layer index error\n");
      display_error++;
      return 1;
   }

   if ((layerlist[layerlist_cnt].layer_name = STRDUP(lp)) == NULL)
   {
      ErrorMessage("Error: No memory for layer saving...");
      return -1;
   }

   while ((lp = strtok(NULL, " ,")) != NULL)
   {
//    if first char of string == ":" = command
      if (lp[0] == ':')
      {
         if (strlen(lp) < 2)
            c_exit();
         switch (lp[1])
         {
            case DETAIL:
               lp[0] = '0';
               lp[1] = '0';
                detail = atoi(lp);
            break;
            case SYMBOLIC:
               lp[0] = '0';
               lp[1] = '0';
               symbolic = atoi(lp);
            break;
            case INTERCONN:
               lp[0] = '0';
               lp[1] = '0';
               interconn = atoi(lp);
            break;
            case LSTYLE:
               lp[0] = '0';
               lp[1] = '0';
               line_style = atoi(lp);
            break;
            case COLOR:
               lp[0] = '0';
               lp[1] = '0';
               color_number = atoi(lp);
            break;
            case PEN:
               lp[0] = '0';
               lp[1] = '0';
               pen_number = atoi(lp);
            break;
            case BOUND:
               lp[0] = '0';
               lp[1] = '0';
               bound_layer = atoi(lp);
            break;
         }
      }
//    not? It are layer number
      else
          layer_number = atoi(lp);
   };

   layerlist[layerlist_cnt].layer = layer_number;
   layerlist_cnt++;

// printf("\nEQU %s D%i S%i I%i L%i C%i P%i B%i",layer_label,detail,symbolic,interconn,line_style,color_number,pen_number,bound_layer);
   return(0);
}


/****************************************************************************/
/*
*/
static int egs_add()
{
   char *lp, *cm;

   get_command(ifp,cmd_line,MAX_LINE);
   cm = cmd_line;
   lp = strtok(cm, " ");
   cm = NULL;

   if (find_in_filelist(lp) != -1)
   {
      add_instance(lp);
   }
   else
   {
      switch (lp[0])
      {
         case ARC:           
            add_arc(lp);       
            last_add = 0;
         break;  // OK!
         case CIRCLE:        
            add_circle(lp);    
            last_add = 0;
         break;  // OK!
         case DIMENSION:     
            add_dimension(lp); 
            last_add = ADD_DIM;
         break;  //
         case HATCH:         
            add_hatch(lp);     
            last_add = 0;
         break;  //
         case LINE: 
            add_line(lp);    
            last_add = 0;
         break;  // OK!
         case MARKER:
            add_marker(lp);   
            last_add = 0;
         break;  // OK!
         case OVAL: 
            add_oval(lp); 
            last_add = 0;
         break;  // OK!
         case POLYGON:       
            add_polygon(lp);   
            last_add = 0;
         break;  // OK!
         case RECTANGLE:     
            add_rectangle(lp); 
            last_add = 0;
         break;  // OK!
         case EGS_TEXT:      
            add_text(lp);      
            last_add = 0;
         break;  //
         case NOTE:          
            add_text(lp);      
            // do not update if in dim mode
            if (begin_assoc != 1 || last_add != ADD_DIM)
               last_add = 0;
         break;  //
         default:
            fprintf(ferr,"[%s] unknown in line [%ld]\n",lp,Get_Linecnt());
            display_error++;
            last_add = 0;
         break;
      }
   }
   return(0);
}

/****************************************************************************/
/*  add ADD_INSTANCE in to database
*/
static int add_instance(char *lp)
{
   char    *name;
   double  rotation=0, origin_x, origin_y, scale=1.0;
   int     mx=0, my=0, nd=0;

   name = (lp);

   while ((lp = strtok(NULL, " ,")) != NULL)
   {
      if (strlen(lp) < 2)
         c_exit();

//    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case 77:
               if (lp[1] == 'X')
                  mx = 1;
               else
                  my = 1;
            break;
            case 78:    nd = 1; break;
            case 88:
               lp[0] = '0';
               lp[1] = '0';
               scale = atof(lp);
            break;
            case 82:
               // there can be a - sign
               rotation = atof(&(lp[2]));
               if (rotation < 0) rotation += 360;
            break;
            case STEP : /* information of the step */ break;
         }
      }
//    not? It are points
      else
      {  // first point
         origin_x = cnv_unit(atof(lp));
         if ((lp = strtok(NULL, ", ")) != NULL)
            origin_y = cnv_unit(atof(lp));
      }
   };  // end of do


   lastAddedSymbolData = Graph_Block_Reference( name, "", -1 , origin_x, origin_y, DegToRad(rotation), 0, scale, -1, TRUE);
// printf("\nINSTANCE %s :R%f :MX%i :MY%i :X%f :ND%i  %f,%f",name,rotation,mx,my,scale,nd,origin_x,origin_y);

   return(0);
}

/****************************************************************************/
/*  add ARC in to database
*/
static int add_arc(char *lp)
{
   char     arc_layer[5];
   int      arc_fill;
   double   arc_res, arc_width;
   double   arc_first_x , arc_first_y;
   double   arc_second_x, arc_second_y;
   double   arc_circum_x, arc_circum_y;
   double   cx, cy, r, sa, da;
   int      layernr;

// arc layer
   lp[0]='0';
   strcpy(arc_layer,lp);
   layernr = Graph_Level(arc_layer,"", 0);

// seek for arc_width, arc_res, arc_fill and points
   while ((lp = strtok(NULL," ,")) != NULL)
   {

//    if first char of string == ":" = command
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case WIDTH:
               lp[0] = '0';
               lp[1] = '0';
               arc_width = cnv_unit(atof(lp));
            break;
            case RES  :
               lp[0] = '0';
               lp[1] = '0';
               arc_res   = atof(lp);
            break;
            case FILL :
               arc_fill = 1;
            break;
            case STEP : /* information of the step */ break;
         }
      }
//    not? It are points
      else
      {  // first point
         arc_first_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         arc_first_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         // second ponit
         arc_second_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         arc_second_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
    // circumference point
         arc_circum_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         arc_circum_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
      }
   }

   ArcPoint3( arc_first_x,arc_first_y,
              arc_circum_x, arc_circum_y,
              arc_second_x, arc_second_y,
              &cx, &cy, &r, &sa, &da );

   find_in_layerlist(atoi(arc_layer));
   //Graph_Level(akt_layer);

   int   err, widthindex = 0;
   if(arc_width > SMALLNUMBER)
      widthindex = Graph_Aperture("", T_ROUND, arc_width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   Graph_Arc(layernr, cx, cy,r,sa,da,0L,widthindex, FALSE);

// printf("\nARC L%i W%f R%f F%i ",arc_layer,arc_width,arc_res,arc_fill);
// printf("FI=%f,%f ",arc_first_x, arc_first_y);
// printf("SE=%f,%f ",arc_second_x, arc_second_y);
// printf("CI=%f,%f\n",arc_circum_x, arc_circum_y);

   return(0);
} // end of add_arc()

/****************************************************************************/
/*  add CIRCLE in to datebase
*/
static int add_circle(char *lp)
{
   char     cir_layer[10];
   int      cir_fill = 0;
   double   cir_center_x, cir_center_y, cir_width;
   double   cir_circum_x, cir_circum_y;
   double   c, cir_res;
   int      layernr;

// circle layer
   lp[0]='0';
   strcpy(cir_layer,lp);
   layernr = Graph_Level(cir_layer,"", 0);

// seek for cir_width, cir_res, cir_fill
// and points (center point, circumference point and step spcification
   while ((lp = strtok(NULL, " ,")) != NULL)
   {
      
//    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case WIDTH:
               lp[0] = '0';
               lp[1] = '0';
               cir_width = cnv_unit(atof(lp));
            break;
            case RES  :
               lp[0] = '0';
               lp[1] = '0';
               cir_res   = atof(lp);
            break;
            case FILL :
               cir_fill = 1;
            break;
            case STEP : /* information of the step */ break;
         }
      }
//    not? It are points
      else
      {  // center point
         cir_center_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         cir_center_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         // circumference point
         cir_circum_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         cir_circum_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
      }
   }  


   find_in_layerlist(atoi(cir_layer));
   //Graph_Level(akt_layer);
   
   //Graph_Line_Width(cir_width);

   c = sqrt(((cir_circum_y - cir_center_y) * (cir_circum_y - cir_center_y))
          + ((cir_circum_x - cir_center_x) * (cir_circum_x - cir_center_x)));

   int   err, widthindex = 0;
   if(cir_width > SMALLNUMBER)
      widthindex = Graph_Aperture("", T_ROUND, cir_width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   Graph_Circle(layernr, cir_center_x, cir_center_y, 
      c, 0, widthindex, FALSE, cir_fill);

// printf("\nCIRCLE L%i W%f R%f F%i ",cir_layer,cir_width,cir_res,cir_fill);
// printf("CE=%f,%f ",cir_center_x, cir_center_y);
// printf("CI=%f,%f\n",cir_circum_x, cir_circum_y);

   return(0);
} // end of add_circle()

/****************************************************************************/
/*   add LINE in to datebase
*/
static int add_line(char *lp)
{
   char      line_layer[20];
   int       line_fill=0, line_constr=0, line_leader=0;
   double    line_width;
   double    line_sfactor=0;
   int       polyline_cnt,i, layernr;
   Point2    *polyline;
   long      lcnt = Get_Linecnt();

   if ((polyline = (Point2 *)calloc(MAX_POLY, sizeof(Point2))) == NULL)
   {
      ErrorMessage("Error : No Memory");
      return -1;
   }
   polyline_cnt = 0;

// line layer
   lp[0]='0';
   strcpy(line_layer,lp);

   find_in_layerlist(atoi(line_layer));
   layernr = Graph_Level(line_layer,"", 0);

   while ((lp = strtok(NULL, " ,")) != NULL)
   {
      //    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case WIDTH :
               lp[0] = '0';
               lp[1] = '0';
               line_width = cnv_unit(atof(lp));
            break;
            case FILL  :
               line_fill = 1;
            break;
            case CONSTR:
               line_constr = 1;
            break;
            case LEADER:
               line_leader = 1;
            break;
            case SFACTOR:
               lp[0] = '0';
               lp[1] = '0';
               line_sfactor = atof(lp);
            break;
            case STEP : /* information of the step */ break;
         }
      }
//    not? It are points
      else
      {  // vertex point
         if ( polyline_cnt < MAX_POLY)
         {
            polyline[polyline_cnt].x = cnv_unit(atof(lp));
            if ((lp = strtok(NULL, ", ")) == NULL)
            {
               fprintf(ferr,"Error in LINE record at %ld\n",Get_Linecnt());
               display_error++;
               return -1;
            }
            polyline[polyline_cnt].y = cnv_unit(atof(lp));
            polyline_cnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines at %ld\n",Get_Linecnt());
            display_error++;
            return -1;
         }
      }
   }

   int err, widthindex = 0;

   if(line_width > SMALLNUMBER)
      widthindex = Graph_Aperture("", T_ROUND, line_width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   Graph_PolyStruct(layernr,0,FALSE);
   Graph_Poly(NULL, widthindex,0,0,0);
   for (i=0;i<polyline_cnt;i++)
      Graph_Vertex(polyline[i].x, polyline[i].y,0.0);

   free(polyline);
   return(0);
} // end of add_line()

/****************************************************************************/
/*   add MARKER in to database
*/
static int add_marker(char *lp)
{
   int     mar_layer;
   double  mar_location_x, mar_location_y;

   fprintf(ferr,"Marker record\n");

// marker layer
   lp[0] = '0';
   mar_layer = atoi(lp);
// seek of location point and step specification
   do
   {
      lp = strtok(NULL, " ,");

      if (strlen(lp) < 2)
         c_exit();

//    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
       case STEP : /* information of the step */ break;
         }
      }
//    not? It's a point
      else
      {  // location point
         mar_location_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         mar_location_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
      }
   }  while (lp);  // end of do


// printf("\nMARKER L%i lOC=%f,%f",mar_layer,mar_location_x,mar_location_y);
   return(0);

} // end of add_marker()

/****************************************************************************/
/*   add RECTANGLE in to database
*/
static int add_rectangle(char *lp)
{
   int     rect_layer, rect_fill=0;
   double  rect_width;   
   double  rect_left_x, rect_left_y, rect_right_x, rect_right_y;

// rectangle layer
   lp[0] = '0';
   rect_layer = atoi(lp);
   int   layernr = Graph_Level(lp,"", 0);

// seek for rect_width, for rect_left and right points
   while ((lp = strtok(NULL, " ,")) != NULL)
   {
//    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case WIDTH :
               lp[0] = '0';
               lp[1] = '0';
               rect_width = cnv_unit(atof(lp));
            break;
            case FILL  :
               rect_fill = 1;
            break;
            case STEP :
                 /* information of the step */
            break;
         }
      }
//    not? It are points
      else
      {  // left point
         rect_left_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         rect_left_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         // right point
         rect_right_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         rect_right_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
      }
   }

   int   widthindex = 0;
   Graph_PolyStruct(layernr,0,FALSE);
   Graph_Poly(NULL, widthindex, rect_fill,0,1);
   Graph_Vertex(rect_left_x, rect_left_y, 0.0);
   Graph_Vertex(rect_right_x, rect_left_y, 0.0);
   Graph_Vertex(rect_right_x, rect_right_y, 0.0);
   Graph_Vertex(rect_left_x, rect_right_y, 0.0);
   Graph_Vertex(rect_left_x, rect_left_y, 0.0);

// printf("\nRECTANGLE L%i W%i ",rect_layer,rect_width);
// printf("LEFT = %f,%f ",rect_left_x, rect_left_y);
// printf("RIGHT = %f,%f",rect_right_x, rect_right_y);

   return(0);
}

/****************************************************************************/
/*   add OVAL in to database
*/
static int add_oval(char *lp)
{
   int     oval_layer, oval_fill;
   double  oval_res;
   double  oval_width;
   double  oval_major;
   double  oval_minor;
   double  oval_start;
   double  oval_finish;
   double  oval_angle;
   double  oval_center_x, oval_center_y;

   fprintf(ferr,"Oval record\n");

// oval layer
   lp[0] = '0';
   oval_layer = atoi(lp);
// seek for cir_width, cir_res, cir_fill
// and points
   while (lp = strtok(NULL, " ,"))
   {
      //    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case WIDTH:
               lp[0] = '0';
               lp[1] = '0';
               oval_width = cnv_unit(atof(lp));
            break;
            case RES  :
               lp[0] = '0';
               lp[1] = '0';
               oval_res   = atof(lp);
            break;
            case FILL :
               oval_fill = 1;
            break;
            case STEP :
               /* information of the step */
            break;
         }
      }
//    not? It are points
      else
      {  // oval major radius
         oval_major = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         // oval minor radius
         oval_minor = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         // oval start angle
         oval_start = atof(lp);
         lp = strtok(NULL, ", ");
         // oval finish angle
         oval_finish= atof(lp);
         lp = strtok(NULL, ", ");
         // oval angle of orientation
         oval_angle = atof(lp);
         lp = strtok(NULL, ", ");
         // oval center point X
         oval_center_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         // oval center point Y
         oval_center_y = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
      }
   }
   
   fprintf(ferr, "Add_Oval not implemented at %ld\n", ifp_linecnt);
   display_error++;

   return(0);
}



/****************************************************************************/
/*   add DIMENSION in to database
*/
static int add_dimension(char *lp)
{
   char     dim_layer[10];
   double   dim_auxpoint1_x, dim_auxpoint1_y;
   double   dim_auxpoint2_x, dim_auxpoint2_y;
   double   dim_refvertex_point1_x, dim_refvertex_point1_y;
   double   dim_refvertex_point2_x, dim_refvertex_point2_y;
   double   dim_reftext_point_x, dim_reftext_point_y;
   int      layernr;
   char     dim_type = ' ';

   lp[0]='0';
   strcpy(dim_layer,lp);
   layernr = Graph_Level(dim_layer,"", 0);

   while ((lp = strtok(NULL, " ,")) != NULL)
   {
      //    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case 'N':
               // inside may be a :X
            break;
            case 'X':
            break;
            case 'L':
            {
               // inside here may be a :Z
               // also a :B
               switch (dim_type = lp[2])
               {
                  case 'H':
                  case 'V':
                     lp = strtok(NULL," ,");
                     if (lp[0] == ':')
                     {
                        // 
                        fprintf(ferr,"Dim error at %ld\n", Get_Linecnt());
                        return 1;
                     }
                     else
                     {
                        dim_refvertex_point1_x = cnv_unit(atof(lp));
                        lp = strtok(NULL," ,");
                        dim_refvertex_point1_y = cnv_unit(atof(lp));
                        lp = strtok(NULL," ,");
                        dim_refvertex_point2_x = cnv_unit(atof(lp));
                        lp = strtok(NULL," ,");
                        dim_refvertex_point2_y = cnv_unit(atof(lp));
                        lp = strtok(NULL," ,");
                        dim_reftext_point_x = cnv_unit(atof(lp));
                        lp = strtok(NULL," ,");
                        dim_reftext_point_y = cnv_unit(atof(lp));
                     }
                  break;
                  case 'A':
                  break;
               }

            }
            break;
            case 'A':
            break;
            case 'R':
            case 'D':
               // diameter dimension
               dim_type = lp[1];
               lp = strtok(NULL," ,");
               dim_refvertex_point1_x = cnv_unit(atof(lp));
               lp = strtok(NULL," ,");
               dim_refvertex_point1_y = cnv_unit(atof(lp));
               lp = strtok(NULL," ,");
               dim_refvertex_point2_x = cnv_unit(atof(lp));
               lp = strtok(NULL," ,");
               dim_refvertex_point2_y = cnv_unit(atof(lp));
               lp = strtok(NULL," ,");
               dim_reftext_point_x = cnv_unit(atof(lp));
               lp = strtok(NULL," ,");
               dim_reftext_point_y = cnv_unit(atof(lp));
            break;
            case STEP : /* information of the step */ 
            break;
         }
      }
//    not? It are points
      else
      {  // aux points
         dim_auxpoint1_x = cnv_unit(atof(lp));
         lp = strtok(NULL, ", ");
         dim_auxpoint1_y = cnv_unit(atof(lp));
         if ((lp = strtok(NULL, ", ")) != NULL)
         {
            dim_auxpoint2_x = cnv_unit(atof(lp));
            lp = strtok(NULL, ", ");
            dim_auxpoint2_y = cnv_unit(atof(lp));
         }
      }
   }  


   int   widthIndex = 0;
   CString  dimname;
   dimname.Format("_DIM_%d",++dimblockcnt);

   switch (dim_type)
   {
      case 'D':
      {
         BlockStruct *b = Graph_Block_On(GBO_APPEND,dimname,cur_filenum,0);
         b->setBlockType(BLOCKTYPE_DIMENSION);
         Graph_Line(layernr, dim_refvertex_point1_x, dim_refvertex_point1_y, 
            dim_reftext_point_x, dim_reftext_point_y, 
            0L, widthIndex, FALSE);
         Graph_Block_Off();
         Graph_Block_Reference(dimname, NULL, cur_filenum,0,0,0.0,0,1.0,0,FALSE);
      }
      break;
      case 'V':
      {
         BlockStruct *b = Graph_Block_On(GBO_APPEND,dimname,cur_filenum,0);
         b->setBlockType(BLOCKTYPE_DIMENSION);
         Graph_Line(layernr, dim_refvertex_point1_x, dim_refvertex_point1_y, 
            dim_reftext_point_x, dim_refvertex_point1_y, 
            0L, widthIndex, FALSE);
         Graph_Line(layernr, dim_refvertex_point2_x, dim_refvertex_point2_y, 
            dim_reftext_point_x, dim_refvertex_point2_y, 
            0L, widthIndex, FALSE);
         

         Graph_Block_Off();
         Graph_Block_Reference(dimname, NULL, cur_filenum,0,0,0.0,0,1.0,0,FALSE);
      }
      break;
      case 'H':
      {
         BlockStruct *b = Graph_Block_On(GBO_APPEND,dimname,cur_filenum,0);
         b->setBlockType(BLOCKTYPE_DIMENSION);
         Graph_Line(layernr, dim_refvertex_point1_x, dim_refvertex_point1_y, 
            dim_refvertex_point1_x, dim_reftext_point_y,  
            0L, widthIndex, FALSE);
         Graph_Line(layernr, dim_refvertex_point2_x, dim_refvertex_point2_y, 
            dim_refvertex_point2_x, dim_reftext_point_y,  
            0L, widthIndex, FALSE);
         
         // need to do from dim_refvertex_point1_x, dim_reftext_point_y to text
         // need to do from dim_refvertex_point2_x, dim_reftext_point_y to text

         Graph_Block_Off();
         Graph_Block_Reference(dimname, NULL, cur_filenum,0,0,0.0,0,1.0,0,FALSE);
      }
      break;
      default:
         fprintf(ferr, "Unknown dimtype [%c] at %ld\n",dim_type,Get_Linecnt());
      break;
   }

   return(0);
}

/****************************************************************************/
/*   add TEXT in to database
*/
static int add_text(char *lp)
{
   char     text_layer[10];
   double   text_res=0;
   int      text_fill=0;
   double   text_fontsize;
   int      text_slant;
	int		text_type = 0;
   double   text_origin_x, text_origin_y;    
   CString  p;
   CString  text = "";

   long     line_ = Get_Linecnt();

   lp[0]='0';
   strcpy(text_layer, lp);

   lp = strtok(NULL,"\n"); // here needs to do some tricks, because text can not
                           // be strtok'ed

   p = lp;
   
   while (lp)
   {
      // always do this at the beginning
      while (isspace(*lp))  lp++;


      if (*lp == ':')
      {
         lp++;
         char cc = *lp++;
         CString w = "";
         while (!isspace(*lp))
            w += *lp++;
         switch (cc)
         {
            case 'S':   // slant
               text_slant = atoi(w);
            break;
            case 'D':
               // do nothing
            break;
            case 'T':
               // do nothing
					text_type = atoi(w);
            break;
            case FONT:
               if (!strcmp(w,"ILL"))   // remains from :FILL :F 
               {

               }
               else
                  text_fontsize = cnv_unit(atof(w));
            break;
            case RES:
               text_res   = atof(w);
            break;
            case ASSOCIATE:

            break;
            case MIRROR:
            {
               // x, y/ xy
					int r = 0;
            }
            break;
            default:
               fprintf(ferr,"Unknown command[%c] at %ld\n",cc,Get_Linecnt());
               display_error++;
            break;
         }
      }
      else
      if (*lp == '\'')
      {
         lp++;
         while (*lp != '\'')
            text += *lp++;
         lp++; // skip the ending token
      }
      else     
      if (*lp == '"')
      {
         lp++;
         while (*lp != '"')
            text += *lp++;
         lp++; // skip the ending token
      }
      else           
      if (*lp == '`')
      {
         lp++;
         while (*lp != '`')
            text += *lp++;
         lp++; // skip the ending token
      }
      else     
      if (*lp == '[')
      {
         lp++;
      }
      else
      {
         sscanf(lp,"%lf, %lf",&text_origin_x, &text_origin_y);
         text_origin_x = cnv_unit(text_origin_x);
         text_origin_y = cnv_unit(text_origin_y);
         break;   // end loop
      }
   }

   find_in_layerlist(atoi(text_layer));
   int layernr = Graph_Level(text_layer,"", 0);

   if (!strlen(text))
   {
      fprintf(ferr,"Text expected at %ld\n",line_);
      return 1;
   }
   
   int   mir = 0;
   int   Proportional = FALSE;

	if (text.CompareNoCase("C103") == 0 ||
		text.CompareNoCase("C675") == 0)
	{
		int jj = 0;
	}

	// Case 1538, If text is tagged as "refdes" (:T1001) then set refdes on
	// last insert (if there is one) and set REFNAME visible attribute.
	// If text is some other tag, or there is no insert, just add the text
	// to the board.
	if (text_type == 1001 && 
		lastAddedSymbolData != NULL && lastAddedSymbolData->getInsert() != NULL)
	{
		// Refdes
		lastAddedSymbolData->getInsert()->setRefname(text);
		// Refdes visible attrib
		CPoint2d insertpt = lastAddedSymbolData->getInsert()->getOrigin2d();
		double insertrot = lastAddedSymbolData->getInsert()->getAngleDegrees();
		double deltaX = text_origin_x - insertpt.x;
		double deltaY = text_origin_y - insertpt.y;
		int insertIntRot = DcaRound(insertrot);
		// Init text offset, assume odd angle insert, put text at center
		double attrOffsetX = 0.0;
		double attrOffsetY = 0.0;
		// If "nice" insert angle, fix up text offset
		switch (insertIntRot)
		{
		case 0:
			attrOffsetX = deltaX;
			attrOffsetY = deltaY;
			break;
		case 90:
			attrOffsetX = deltaY;
			attrOffsetY = -deltaX;
			break;
		case 180:
			attrOffsetX = -deltaX;
			attrOffsetY = -deltaY;
			break;
		case 270:
			attrOffsetX = -deltaY;
			attrOffsetY = deltaX;
			break;
		default:
			attrOffsetX = 0.0;  // redundant, but explicit
			attrOffsetY = 0.0;
			break;
		}

		// Attr text rotation relative to insert
		double attrRot = normalizeDegrees(text_res - insertrot);

		int err;
		int penWidthIndex =  Graph_Aperture("", T_ROUND, 0.1 * text_fontsize, 0.0, 
            0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);

		CCEtoODBDoc *doc = getActiveView()->GetDocument();
      doc->SetUnknownVisAttrib(&lastAddedSymbolData->getAttributesRef(), ATT_REFNAME, text,
			attrOffsetX, attrOffsetY, DegToRad(attrRot),
            text_fontsize, text_fontsize * 6.0/8.0, Proportional, penWidthIndex, 
				TRUE, SA_OVERWRITE, 0L, layernr, 0, 0, 0);
	}
	else
	{
		// Add as "board level" text
		Graph_Text(layernr, text, text_origin_x, text_origin_y, 
			text_fontsize,text_fontsize * 6.0/8.0, 
			DegToRad(text_res), 0l, Proportional, mir, text_slant, FALSE, -1, 0);
	}

   return(0);
}

/****************************************************************************/
/*   add POLYGON in to database
*/
static int add_polygon(char *lp)
{
   char      poly_layer[10];
   int       poly_fill=0;
   double    poly_width;
   Point2    *polyline;
   int       polyline_cnt;

   polyline_cnt = 0;

   if ((polyline = (Point2 *)calloc(MAX_POLY, sizeof(Point2))) == NULL)
   {
      ErrorMessage("Error : No Memory");
      return -1;
   }

// polygon layer
   lp[0]='0';
   strcpy(poly_layer,lp);

   find_in_layerlist(atoi(poly_layer));
   int layernr = Graph_Level(poly_layer,"", 0);

   int   widthindex = 0;
   Graph_PolyStruct(layernr,0,FALSE);
   Graph_Poly(NULL, widthindex,0,0,0);

// seek for poly_width, poly_fill and for points (vertex point)
   while ((lp = strtok(NULL, " ,")) != NULL)
   {

      //    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case WIDTH :
               lp[0] = '0';
               lp[1] = '0';
               poly_width = cnv_unit(atof(lp));
            break;
            case FILL  :
               poly_fill = 1;
            break;
            case STEP :
                 /* information of the step */
            break;
        }
      }
//    not? It are points
      else
      {  // vertex point
         if (polyline_cnt < MAX_POLY)
         {
            polyline[polyline_cnt].x = cnv_unit(atof(lp));
            if ((lp = strtok(NULL, ", ")) != NULL)
            {
               polyline[polyline_cnt].y = cnv_unit(atof(lp));
            }
            polyline_cnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines at %ld\n",Get_Linecnt());
            display_error++;
            return -1;
         }
      }
   }  // end of do


   for (int i=0;i<polyline_cnt;i++)
      Graph_Vertex(polyline[i].x, polyline[i].y, 0.0);

   free(polyline);

   return(0);
}

/****************************************************************************/
/*   add POLYGON in to database
*/
static int add_hatch(char *lp)
{
   char     poly_layer[10];
   double   poly_width;
   int      poly_fill=0;
   Point2   *polyline;
   int      polyline_cnt;
   char     *cm = NULL;

   polyline_cnt = 0;

   if ((polyline = (Point2 *)calloc(MAX_POLY, sizeof(Point2))) == NULL)
   {
      ErrorMessage("Error : No Memory");
      return -1;
   }

// polygon layer
   lp[0]='0';
   strcpy(poly_layer,lp);

// seek for poly_width, poly_fill and for points (vertex point)
   while ((lp = strtok(cm, " ,")) != NULL)
   {
      cm = NULL;
      if (strlen(lp) < 2)
         c_exit();

//    if first char of string == ":" = command
      if (lp[0] == '@')
      {
// I do not know what this is
      }
      else
      if (lp[0] == '.')
      {
// I do not know what this is
      }
      else
      if (lp[0] == ':')
      {
         switch (lp[1])
         {
            case 'H' :
               // here is a unfilled polygon terminated by ;   
               get_command(ifp,cmd_line,MAX_LINE);
               cm = cmd_line;
            break;
            case WIDTH :
               lp[0] = '0';
               lp[1] = '0';
               poly_width = cnv_unit(atof(lp));
            break;
            case FILL  :
               poly_fill = 1;
            break;
            case STEP :
                 /* information of the step */
            break;
        }
      }
//    not? It are points
      else
      {  // vertex point
         if (polyline_cnt < MAX_POLY)
         {
            polyline[polyline_cnt].x = cnv_unit(atof(lp));
            if ((lp = strtok(NULL, ", ")) != NULL)
            {
               polyline[polyline_cnt].y = cnv_unit(atof(lp));
            }
            polyline_cnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines at %ld\n",Get_Linecnt());
            display_error++;
            return -1;
         }

      }
   }  // end of do

   find_in_layerlist(atoi(poly_layer));
   //Graph_Level(akt_layer);
    
   //Graph_Line_Width(poly_width);
   //Graph_Polyline( polyline, polyline_cnt, 0.0);

   free(polyline);

   return(0);
}

/****************************************************************************/
/*
*/
long Get_Linecnt()
{
   return ifp_linecnt;
}

/****************************************************************************/
/*
*/
static void update_linecnt(char c)
{
   if (c == '\n') 
      ifp_linecnt++;
   else
   if (c == 10)   
      ifp_linecnt++; // Unix 
   else
   if (c == 13)   
      ifp_linecnt++; // Unix 
}

/****************************************************************************/
/*
*/
static int Get_KeyWord(FILE *fp,char *tmp,int n)
{
   int   cnt = 0;
   int   c;

   while ((c = fgetc(fp)) != EOF)
   {
      if (c == 0) // file is opened in binary mode !
      {
         c = EOF;
         break;
      }

      update_linecnt(c);
      if (isspace(c))   break;
      if (c == ';')
      {
         ungetc(c,fp);
         break;
      }
      if (cnt < n)
         tmp[cnt++] = c;
      else
      {
         fprintf(ferr,"Keyword error at %ld\n", ifp_linecnt);
         return 0;
      }
   }

   if (c == EOF)
   {
      eof_egsfile = TRUE;
      return -1;
   }

   tmp[cnt ] ='\0';
   CString  t = tmp;
   t.TrimLeft();
   t.TrimRight();
   
   strcpy(tmp,t);

   return 1;
}


/****************************************************************************/
/*
*/
double cnv_unit(double u)
{
   return u*scale;
}

/****************************************************************************/
/*
   Critical error in destination file
*/
void c_exit()
{
   fprintf(ferr,"Critical error: bad parameter in line [%ld] -> [%s]\n",Get_Linecnt(),token);
   display_error++;
   //exit(0);
   return;
}




// end HPEGDSIN.CPP

////////////////////////////////////////////////////////////////////////////////////////
