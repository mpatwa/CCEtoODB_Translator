// $Header: /CAMCAD/5.0/read_wrt/ViewDrIn.cpp 29    2/27/07 1:58p Kurt Van Ness $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.

   What needs to be done:
   colors from ini file
   pin attributes
   


*/           

// all units are in 10 MIL          

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <io.h>
#include <direct.h>
#include <time.h>
#include <math.h>
#include "gauge.h"
#include "attrib.h"
#include "viewdrin.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

/* External Variables ***************************************************************************/
extern   CProgressDlg *progress;

/* Static Variables ***************************************************************************/
static CCEtoODBDoc    *doc;
static FILE          *ferr;
static int           display_error = 0;

static FILE          *ifp;
static char          ifp_line[MAX_LINE];
static long          ifp_linecnt = 0;

static int           cur_layer;
static int           cur_entity;

static double        scale_factor = 1;
static int           cur_filenum = 0;

static Joint         *joint;
static int           jointcnt;

static CInsertArray  insertarray;
static int           insertcnt = 0;

static CNetNameArray netnamearray;
static int           netnamecnt = 0;

static CLibpathArray libpatharray;
static int           libpathcnt = 0;

static CString       primarypath;
static int           wirewidth, buswidth, bubblesize;

/* Fucntion Prototype *****************************************************************************/

static char *get_sheet_dir(const char *ini_path, char *sheetdir);
static char *get_sym_dir(const char *ini_path, char *symdir);
static int get_ini(const char *fname, int *wir, int *bus, int *dot, int *box, int *bubblesize);
static int do_scanfile(const char *filename, int component_mode, BlockStruct *mainblock);
static int rename_netnames(CCEtoODBDoc *doc, FileStruct *file);
static int get_libpath(const char *libname);

/******************************************************************************
* ReadVIEWDRAW
   The reading works as follows
   
   under this directory, there must be 2 other directories
   1. sch
   2. sym

  the ini file gives the name of the starting sheet file in a sch directory.
  all the symbols are in the sym directory.

  Later I have to make the symbol directory INI file drive dependent.

dir [p] L:\sandbox\user_directory\gary_b\c549x_mod
dir [r] L:\ViewLib\excel_lib\discrete (discrete)
dir [r] L:\ViewLib\excel_lib\misc (misc)
dir [r] L:\ViewLib\excel_lib\74x (IC74x)
dir [r] C:\VLOGIC\avslib\CONNS (conns)

  Units are in 10 mil increments.

*/
void ReadVIEWDRAW(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *Format, double scaleUnitsFactor)
{
   FileStruct *file;
   CString f;

   doc = Doc;
   scale_factor = 10 * scaleUnitsFactor;

   // here now have to get a list of all files in the sheetdir
   char sheetdir[_MAX_PATH];
   char symdir[_MAX_PATH];
   get_sheet_dir(pathname, sheetdir);
   get_sym_dir(pathname, symdir);

   CString logFile = GetLogfilePath(VIEWDRAWERR);
   if ((ferr = fopen(logFile, "wt")) == NULL)
   {
      CString t;
      t.Format("Error open [%s] file", logFile);
      ErrorMessage(t, "Error");
      return;
   }
   display_error = 0;

   if ((joint = (Joint  *)calloc(MAX_JOINT, sizeof(Joint))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   jointcnt = 0;

   insertarray.SetSize(100, 100);
   insertcnt = 0;

   libpatharray.SetSize(100, 100);
   libpathcnt = 0;

   netnamearray.SetSize(100, 100);
   netnamecnt = 0;

   // read ini for sizes and layer colors
   int wire = 0, bus = 5, dot = 5, box = 3;
   bubblesize = 5;
   get_ini(pathname, &wire, &bus, &dot, &box, &bubblesize);

   int err;
   wirewidth = Graph_Aperture("NETWIDTH", T_ROUND, wire*scale_factor , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   buswidth =  Graph_Aperture("BUSWIDTH", T_ROUND, bus*scale_factor , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   
   // ensure these aperatures are in database
   Graph_Aperture("DOT", T_ROUND, dot * scale_factor, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err); 
   Graph_Aperture("BOX", T_ROUND, box * scale_factor, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);

   /* Get the current working directory: */
   char  curdir[_MAX_PATH];
   if( _getcwd( curdir, _MAX_PATH ) == NULL )   
   {
      CString tmp;
      tmp.Format( "Problem get current directory");
      MessageBox(NULL, "Error Get Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   struct _finddata_t c_file;
   int cur_pagecnt = 0;

   if( _chdir( sheetdir ) != 0  )
   {
      CString tmp;
      tmp.Format( "Problem reading directory '%s'" , sheetdir);
      MessageBox(NULL, "Error Reading Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // chdir
   long hFile;
   if ( (hFile = _findfirst("*.*", &c_file)) == -1)
   {
      CString t;
      t.Format("No *.* files found in current directory!\n");
      ErrorMessage(t, "Error");
   }
   else
   {
      do
      {
         // because all files are looked up, also directories appear.
         if (c_file.attrib & _A_SUBDIR)
            continue;

         CString sheetname = c_file.name;
         // here for all files in sheetdir, do it.
         f = sheetdir;
         f += sheetname;
   
         // this can not be rt (because of LB terminator
         if ((ifp = fopen(f, "rt")) == NULL)
         {
            ErrorMessage(f, "Error open file");
            continue;
         }

         file = Graph_File_Start(sheetname, Type_VIEWDRAW);
         cur_filenum = file->getFileNumber();      

         CPoly *lastgraphpoly = NULL;

         jointcnt = 0;
         ifp_linecnt = 0;
         netnamecnt = 0;

         int ymode = do_scanfile(f, 0, file->getBlock());

         // here update netname
         rename_netnames(doc, file);

         for (int i=0;i<netnamecnt;i++)
            delete netnamearray[i];
         netnamecnt = 0;

         if (ymode  == 0)
         {
            file->setBlockType(blockTypeSheet); 
            file->getBlock()->setBlockType(file->getBlockType());
            file->setShow(cur_pagecnt == 0); 
         }
         cur_pagecnt++;
         fclose(ifp);

      } while ( _findnext(hFile, &c_file) == 0);
   }

   netnamearray.RemoveAll();

   int i, oldinsertcnt = insertcnt;

   // change back to  current directory, because the sym is relative to the current dir.
   if( _chdir( curdir ) != 0  )
   {
      CString tmp;
      tmp.Format( "Problem reading directory '%s'" , curdir);
      MessageBox(NULL, "Error Reading Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   for (i=0; i<insertcnt; i++)
   {
      CString symname = insertarray[i]->name;
      CString libname = insertarray[i]->libname;
      CString sheetname = insertarray[i]->sheetname;

      // here for all files in sheetdir, do it.
      if (strlen(libname))
      {
         int lptr = get_libpath(libname);
         if (lptr < 0)
         {
            fprintf(ferr, "No Library path found for [%s] -> set to [%s]\n", libname, symdir);
            display_error++;
            f = symdir;
         }
         else
         {
            f = libpatharray[lptr]->path;
            f += "sym\\";
         }
      }
      else
      {
         // default symbol path
         f = symdir;
      }

      CString librarypath = f;
      f += symname;
      if (strlen(f) == 0)
         continue;

      char  cdir[_MAX_PATH];
   
      /* Get the current working directory: */
      _getcwd( cdir, _MAX_PATH );  

      // 
      if ((ifp = fopen(f, "rt")) == NULL)
      {
         fprintf(ferr, "Can not find SYMBOL [%s] used on sheet [%s]\n", f, sheetname);
         display_error++;
         continue;
      }

      // make a Graph_Block to 
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,symname, -1, 0);
      jointcnt = 0;
      ifp_linecnt = 0;

      int ymode = do_scanfile(f, 1, curblock);
      switch (ymode)
      {
         case 1:
            curblock->setBlockType(BLOCKTYPE_SYMBOL);
         break;
      }

      Graph_Block_Off();

      fclose(ifp);
   }

   if (oldinsertcnt != insertcnt)
   {
      fprintf(ferr, "Symbols inserted from within symbols\n");
      display_error++;
   }

   _chdir( curdir );

   fclose(ferr);

   free(joint);

   for (i=0; i<insertcnt; i++)
      delete insertarray[i];
   insertarray.RemoveAll();

   for (i=0; i<libpathcnt; i++)
      delete libpatharray[i];
   libpatharray.RemoveAll();

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
*/
static char *get_nextline(const char *curfilename, char *string,int n,FILE *fp)
{
   char  tmpline[MAX_LINE];

   char  *res;
   int   ch;

   string[0] = '\0';

   while (TRUE)
   {
      ifp_linecnt++;
      res = fgets(tmpline,n,fp);

      if (strlen(tmpline))
      {
         if(tmpline[strlen(tmpline)-1] == '\n')
         {
            tmpline[strlen(tmpline)-1] = '\0';
         }
      }
   
      if ((int)(strlen(string) + strlen(tmpline)) < n)
      {
         strcat(string,tmpline);
      }
      else
      {
         fprintf(ferr,"Line read error in [%s] at %ld\n", curfilename, ifp_linecnt);
         display_error++;
         return res;
      }

      // now check if next line continues with a + sign.
      if ((ch = fgetc(fp)) != '+')
      {
         ungetc(ch, fp);         
         break;   // break from while (TRUE)
      }
   }
   return res;
}

/****************************************************************************/
/*
*/
static int get_libpath(const char *libname)
{
   int   i;

   for (i=0;i<libpathcnt;i++)
   {
      if (libpatharray[i]->libname.CompareNoCase(libname) == 0)
      {
         return i;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int update_libpath(const char *path, const char *libname)
{
   int   i;

   for (i=0;i<libpathcnt;i++)
   {
      if (libpatharray[i]->libname.Compare(libname) == 0)
      {
         fprintf(ferr,"A library path for [%s] already exist !\n", libname);
         display_error++;
         return i;
      }
   }

   VIEWLibpath *c = new VIEWLibpath;
   libpatharray.SetAtGrow(libpathcnt,c);
   c->path = path;
   if (path[strlen(path)-1] != '\\')
      c->path += "\\";
   c->libname = libname;
   libpathcnt++;

   return libpathcnt-1;
}

/******************************************************************************
* get_ini
*/
static int get_ini(const char *fname, int *wir, int *bus, int *dot, int *box, int *bubblesize)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp, "Viewlogic Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   char  curdir[_MAX_PATH];

   /* Get the current working directory: */
   if( _getcwd( curdir, _MAX_PATH ) == NULL )   
   {
      CString tmp;
      tmp.Format( "Problem get current directory");
      MessageBox(NULL, "Error Get Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }
   primarypath = curdir;

   while (fgets(line, 255, fp))
   {
      CString w = line;
      if ((lp = strtok(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '|')                      
         continue;   // remark

      if (!STRCMPI(lp, "DOTSIZE"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         *dot = atoi(lp);
      }
      else if (!STRCMPI(lp, "BOXSIZE"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         *box = atoi(lp);
      }
      else if (!STRCMPI(lp, "BUSWIDTH"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         *bus = atoi(lp);
      }
      else if (!STRCMPI(lp, "BUBBLESIZE"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         *bubblesize = atoi(lp);
      }
      else if (!STRCMPI(lp, "DIR"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue; // [p] or other access flags
         CString flag = lp;

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue; // path
         CString path = lp;

         if (!flag.CompareNoCase("[p]"))
         {
            if (path != "." && path.CompareNoCase(curdir) ) // if not the same
            {
               CString tmp;
               tmp.Format("Current Path [%s] is different from Primary Path [%s].\nRelative Library Pathes are from Current Path.\nUpdate VIEWDRAW.INI if needed!",
                     curdir, path);
               ErrorMessage (tmp);
            }
            primarypath = path;
         }

         if ((lp = strtok(NULL, " ()\t\n")) == NULL)
            continue; // libname
         update_libpath(path, lp);
      }
   }
   fclose(fp);

   return 1;
}

/****************************************************************************/
/*
*/
static void format_error()
{
   CString  l;
   l.Format("File format invalid. Please check Format");
   MessageBox(NULL, l,"Format Error", MB_OK | MB_ICONHAND);

   return;
}

/******************************************************************************
* *get_sheet_dir
*/
static char *get_sheet_dir(const char *ini_path, char *sheetdir)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( ini_path, drive, dir, fname, ext );

   _makepath( sheetdir, drive, dir, "", "" );

   strcat(sheetdir, "sch\\");

   return sheetdir;
}

/******************************************************************************
* *get_sym_dir
*/
static char *get_sym_dir(const char *ini_path, char *symdir)
{

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( ini_path, drive, dir, fname, ext );

   _makepath( symdir, drive, dir, "", "" );

   strcat(symdir, "sym\\");

   return symdir;
}

/****************************************************************************/
/*
*/
static int make_sheet(double x, double y)
{
   int   layer = Graph_Level("SHEET","",0);

   Graph_PolyStruct(layer,0L,0);
   Graph_Poly(NULL,0, 0,0,TRUE);
   Graph_Vertex(0.0, 0.0,0.0);
   Graph_Vertex(x, 0.0,0.0);
   Graph_Vertex(x, y,0.0);
   Graph_Vertex(0.0, y,0.0);
   Graph_Vertex(0.0, 0.0,0.0);

   return 1;
}

/****************************************************************************/
/*
// needed for text normalization
#define  GRTEXT_W_L              0x0000
#define  GRTEXT_H_C              0x0001
#define  GRTEXT_W_R              0x0002
#define  GRTEXT_H_B              0x0000
#define  GRTEXT_H_C              0x0010
#define  GRTEXT_H_T              0x0020

*/
static DbFlag get_textjust(int n)
{
   DbFlag   flg = 0;


   switch (n)
   {
      case 1:
         flg = GRTEXT_H_T;
      break;
      case 2:
         flg = GRTEXT_H_C;
      break;
      case 3:
         flg = 0;
      break;
      case 4:
         flg = GRTEXT_H_T | GRTEXT_W_C;
      break;
      case 5:
         flg = GRTEXT_H_C | GRTEXT_W_C;
      break;
      case 6:
         flg = GRTEXT_W_C;
      break;
      case 7:
         flg = GRTEXT_H_T | GRTEXT_W_R;
      break;
      case 8:
         flg = GRTEXT_H_C | GRTEXT_W_R;
      break;
      case 9:
         flg = GRTEXT_W_R;
      break;
   }

   return flg;
}

/****************************************************************************/
/*
*/
static int update_insert(const char *nname, const char *sheetname)
{
   CString  libname;
   CString  name;

   libname = "";
   name = "";
   if (strchr(nname,':'))
   {
      int i;
      for (i=0; i<(int)strlen(nname); i++)
      {
         if (nname[i] == ':') break;
         libname += nname[i];
      }

      i++;  // advance :

      for (;i<(int)strlen(nname);i++)
      {
         name += nname[i];
      }
   }
   else
      name = nname;

   name.TrimLeft();
   name.TrimRight();

   libname.TrimLeft();
   libname.TrimRight();

   for (int i=0; i<insertcnt; i++)
   {
      if (insertarray[i]->name.Compare(name) == 0)
         return i;
   }


   VIEWInsert  *c = new VIEWInsert;
   insertarray.SetAtGrow(insertcnt,c);
   c->name = name;
   c->libname = libname;
   c->sheetname = sheetname;
   insertcnt++;

   // make a Graph_Block to 
   BlockStruct *curblock = Graph_Block_On(GBO_APPEND,name, -1, 0);
   Graph_Block_Off();

   return insertcnt-1;
}

/****************************************************************************/
/*
*/
static int get_netnameptr(int netnr)
{
   int   i;

   for (i=0;i<netnamecnt;i++)
   {
      if (netnamearray[i]->netnr == netnr)
         return i;
   }

   return -1;
}

/******************************************************************************
* do_scanfile
   component_mode 
   0 is for sheets
   1 is for symbols
*/
static int do_scanfile(const char *filename, int component_mode, BlockStruct *mainblock)
{
   // E command make a logical file end marker.
   int logical_fileend = FALSE;
   char *lp, lp1;
   DataStruct  *lastinsert = NULL;
   int ymode = -1;
   char lastchar = ' ';
   int lastnetnr = -1;

   // here do a while loop
   while (!logical_fileend && get_nextline(filename, ifp_line,MAX_LINE,ifp))
   {
      CString save_line = ifp_line;
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         break;        // end of file
      if (!strlen(lp))                               
         continue;     // empty line
      lp1 = lp[0];

      switch (lp[0])
      {
      case '|':
            // remark
         break;
      case 'V':
         {
            // Version number of database
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Version Number expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int ver = atoi(lp);

            if (ver != 50)
            {
               fprintf(ferr, "Unsupported Version Number [%d] -> Supported Versions [50] at %ld\n", ver, ifp_linecnt);
               display_error++;
               break;
            }
         }
         break;
      case 'b':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Box llx expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double llx = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Box lly expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double lly = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Box urx expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double urx = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Box urx expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double ury = atof(lp) * scale_factor;

            int   layer = Graph_Level("BOX", "", 0);
            Graph_PolyStruct(layer, 0L, 0);
            Graph_Poly(NULL, 0, 0, 0, TRUE);
            Graph_Vertex(llx, lly, 0.0);
            Graph_Vertex(urx, lly, 0.0);
            Graph_Vertex(urx, ury, 0.0);
            Graph_Vertex(llx, ury, 0.0);
            Graph_Vertex(llx, lly, 0.0);
         }
         break;
      case 'P':
         {
            lastinsert = NULL;
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin id expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x1 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin y expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y1 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x2 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin y expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y2 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin Type expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin Side expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int side = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Pin Inverted expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }

            int inv = atoi(lp);
            double diam = 2 * bubblesize * scale_factor;

            int layer = Graph_Level("PIN", "", 0);
            Graph_PolyStruct(layer, 0L, 0);

            if (!inv)
            {
               Graph_Poly(NULL, 0, 0, 0, 0);
               Graph_Vertex(x1, y1, 0.0);
               Graph_Vertex(x2, y2, 0.0);
            }
            else
            {
               switch (side)  // 0 = top, 1 = bottom, 2 left, 3 right
               {
               case 0:
                  Graph_Poly(NULL, 0, 0, 0, 0);
                  Graph_Vertex(x1, y1, 0.0);
                  Graph_Vertex(x2, y2 + diam, 0.0);
                  Graph_Poly(NULL, 0, 0,0,0);
                  Graph_Vertex(x2, y2 + diam, 1.0);
                  Graph_Vertex(x2, y2, 1.0);
                  Graph_Vertex(x2, y2 + diam, 0.0);
                  break;
               case 1:
                  Graph_Poly(NULL, 0, 0, 0, 0);
                  Graph_Vertex(x1, y1, 0.0);
                  Graph_Vertex(x2, y2 - diam, 0.0);
                  Graph_Poly(NULL, 0, 0,0,0);
                  Graph_Vertex(x2, y2 - diam, 1.0);
                  Graph_Vertex(x2, y2, 1.0);
                  Graph_Vertex(x2, y2 - diam, 0.0);
                  break;
               case 2:
                  Graph_Poly(NULL, 0, 0, 0, 0);
                  Graph_Vertex(x1, y1, 0.0);
                  Graph_Vertex(x2 - diam, y2, 0.0);
                  Graph_Poly(NULL, 0, 0,0,0);
                  Graph_Vertex(x2 - diam, y2, 1.0);
                  Graph_Vertex(x2, y2, 1.0);
                  Graph_Vertex(x2 - diam, y2, 0.0);
                  break;
               case 3:
                  Graph_Poly(NULL, 0, 0, 0, 0);
                  Graph_Vertex(x1, y1, 0.0);
                  Graph_Vertex(x2 + diam, y2, 0.0);
                  Graph_Poly(NULL, 0, 0,0,0);
                  Graph_Vertex(x2, y2, 1.0);
                  Graph_Vertex(x2 + diam, y2, 1.0);
                  Graph_Vertex(x2, y2, 0.0);
                  break;
               default: 
                  Graph_Poly(NULL, 0, 0, 0,0);
                  Graph_Vertex(x1, y1, 0.0);
                  Graph_Vertex(x2, y2, 0.0);
                  break;
               }
            }

            Graph_Block_On(GBO_APPEND, "$PIN", -1, 0);
            Graph_Block_Off();
            lastinsert = Graph_Block_Reference("$PIN", NULL, 0, x1, y1, DegToRad(0.0), 0 , 1.0, 0, TRUE);
         }
         break;
      case 'l':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Line vertexcnt expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int vertex = atoi(lp);

            if (vertex)
            {
               int layer = Graph_Level("LINE", "", 0);
               Graph_PolyStruct(layer, 0L, 0);
               Graph_Poly(NULL, 0, 0, 0, 0);
               for (int i=0; i<vertex; i++)
               {
                  if ((lp = strtok(NULL, " \t\n")) == NULL) 
                  {
                     if (!get_nextline(filename, ifp_line,MAX_LINE,ifp))
                     {
                        fprintf(ferr, "Vertex continuation expected in [%s] at %ld\n", filename, ifp_linecnt);
                        display_error++;
                        break;
                     }
                     if ((lp = strtok(ifp_line," \t\n")) == NULL) 
                     {
                        fprintf(ferr, "Line x expected in [%s] at %ld\n", filename, ifp_linecnt);
                        display_error++;
                        break;
                     }
                  }
                  double x = atof(lp) * scale_factor;

                  if ((lp = strtok(NULL," \t\n")) == NULL)  
                  {
                     fprintf(ferr,"Line y expected in [%s] at %ld\n", filename, ifp_linecnt);
                     display_error++;
                     break;
                  }
                  double y = atof(lp) * scale_factor;

                  Graph_Vertex(x, y, 0.0);
               }
            } // vertex
         }
         break;
      case 'Y':
         if ((lp = strtok(NULL, " \t\n")) == NULL) 
         {
            fprintf(ferr, "Block_type Number expected in [%s] at %ld\n", filename, ifp_linecnt);
            display_error++;
            break;
         }
         ymode = atoi(lp);
         break;
      case 'D':
         // size of symbol or schematic
         break;
      case 'Z':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Version Number expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int sheetsize = atoi(lp);

            switch (sheetsize)
            {
            case 0:
               make_sheet(850 * scale_factor, 1100 * scale_factor);
               break;
            case 1:
               make_sheet(1700 * scale_factor, 1100 * scale_factor);
               break;
            case 2:
               make_sheet(1700 * scale_factor, 2200 * scale_factor);
            case 3:
               make_sheet(3400 * scale_factor, 2200 * scale_factor);
               break;
            case 4:
               make_sheet(3400 * scale_factor, 4400 * scale_factor);
               break;
            case 10:
               {
/*
                  double   x ,y;

                  if ((lp = strtok(NULL," \t\n")) == NULL)  
                  {
                     fprintf(ferr,"Sheet size x expected in [%s] at %ld\n", filename, ifp_linecnt);
                     display_error++;
                     break;
                  }
                  x = atof(lp);

                  if ((lp = strtok(NULL," \t\n")) == NULL)  
                  {
                     fprintf(ferr,"Sheet size y expected in [%s] at %ld\n", filename, ifp_linecnt);
                     display_error++;
                     break;
                  }
                  y = atof(lp);
                  make_sheet(x*scale_factor, y*scale_factor);
*/
               }
               break;
            default:
               fprintf(ferr, "Unsupported Sheetsize [%d] at %ld\n", sheetsize, ifp_linecnt);
               display_error++;
               break;
            }
         }
         break;
      case 'N':
         {
            // start of a net
            jointcnt = 0;
            lastinsert = NULL;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Netnr expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            lastnetnr = atoi(lp);

            int netptr;
            if ((netptr = get_netnameptr(lastnetnr)) < 0)
            {
               VIEWNetName *n = new VIEWNetName;
               netnamearray.SetAtGrow(netnamecnt++, n);
               n->netnr = lastnetnr;
               n->netname = "";
            }
         }
         break;
      case 'S':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Joint 1 expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int j1 = atoi(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"Joint 2 expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }           
            int j2 = atoi(lp);

            if (j1 < 1 || j1 > jointcnt)  
            {
               fprintf(ferr, "Invalid Joint 1 [%d] in [%s] at %ld\n", j1, filename, ifp_linecnt);
               display_error++;
            }
            if (j2 < 1 || j2 > jointcnt)  
            {
               fprintf(ferr, "Invalid Joint 2 [%d] in [%s] at %ld\n", j1, filename, ifp_linecnt);
               display_error++;
            }

            int layer = Graph_Level("NET", "", 0);
            DataStruct *d = Graph_PolyStruct(layer, 0L, 0);

            if (lastnetnr > -1)
            {
               CString  netname;
               netname.Format("%d", lastnetnr);
               doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_NETNAME, netname, SA_OVERWRITE, NULL);
               d->setGraphicClass(GR_CLASS_ETCH);
            }

            Graph_Poly(NULL, wirewidth, 0, 0, TRUE);
            Graph_Vertex(joint[j1-1].x, joint[j1-1].y, 0.0);
            Graph_Vertex(joint[j2-1].x, joint[j2-1].y, 0.0);
         }
         break;
      case 'B':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Joint 1 expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int j1 = atoi(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr, "Joint 2 expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int j2 = atoi(lp);

            if (j1 < 1 || j1 > jointcnt)  
            {
               fprintf(ferr, "Invalid Joint 1 [%d] in [%s] at %ld\n", j1, filename, ifp_linecnt);
               display_error++;
            }
            if (j2 < 1 || j2 > jointcnt)  
            {
               fprintf(ferr, "Invalid Joint 2 [%d] in [%s] at %ld\n", j1, filename, ifp_linecnt);
               display_error++;
            }
      
            int   layer = Graph_Level("NET", "", 0);
            Graph_PolyStruct(layer, 0L, 0);
            Graph_Poly(NULL, buswidth, 0, 0, TRUE);
            Graph_Vertex(joint[j1-1].x, joint[j1-1].y, 0.0);
            Graph_Vertex(joint[j2-1].x, joint[j2-1].y, 0.0);
         }
         break;
      case 'J':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "X coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Y coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Type expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int typ = atoi(lp);

            if (jointcnt < MAX_JOINT)
            {
               joint[jointcnt].x = x;
               joint[jointcnt].y = y;
               joint[jointcnt].typ = typ;
               jointcnt++;
            }
            else
            {
               fprintf(ferr, "Too many joints in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
            }

            if (typ == 5)
            {
               // solder joint
               int layer = Graph_Level("NET", "", 0);
               Graph_Block_Reference("DOT", NULL, 0, x , y, 0.0, 0, 1.0, layer, 1);
            }
            else if (typ == 1)
            {
               // solder joint
               int layer = Graph_Level("NET", "", 0);
               Graph_Block_Reference("BOX", NULL, 0, x , y, 0.0, 0, 1.0, layer, 1);
            }
         }
         break;
      case 'L':
         {
            int layer = Graph_Level("LABEL", "" ,0);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "X coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor;
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Y coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor; 
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Size expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double siz = atof(lp) * scale_factor;  
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orie = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orig = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label View expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Inv expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int vi = atoi(lp);   

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Inv expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int inv = atoi(lp);  

            if ((lp = strtok(NULL, "\n")) == NULL) 
            {
               fprintf(ferr, "Label String expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            CString prosa = lp;
            prosa.TrimLeft();
            prosa.TrimRight();

            if (strlen(prosa) == 0)
            {
               fprintf(ferr, "Label has not string [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }

            int mirror = 0;
            double rotation = 0;

            switch (orie)
            {
            case 0:
               // normal
               rotation = 0;
               mirror = 0;
               break;
            case 1:
               rotation = 90;
               mirror = 0;
               break;
            case 2:
               rotation = 180;
               mirror = 0;
               break;
            case 3:
               rotation = 270;
               mirror = 0;
               break;
            case 4:
               rotation = 0;
               mirror = MIRROR_FLIP;
               x = x - siz*0.8 * strlen(prosa);
               break;
            case 5:
               rotation = 90;
               mirror = MIRROR_FLIP;
               x = x - siz*0.8 * strlen(prosa);
               break;
            case 6:
               rotation = 180;
               mirror = MIRROR_FLIP;
               x = x - siz*0.8 * strlen(prosa);
               break;
            case 7:
               rotation = 270;
               mirror = MIRROR_FLIP;
               x = x - siz*0.8 * strlen(prosa);
               break;
            }

            mirror = 0;
            normalize_text(&x, &y, get_textjust(orig), rotation, mirror, siz, siz*0.8 * strlen(prosa));

            if (lastinsert && lastchar == 'P')  // if the label follows immediately a Pin record
            {
               x = x - lastinsert->getInsert()->getOriginX();
               y = y - lastinsert->getInsert()->getOriginY();
/*
               if (lastinsert->getInsert()->getMirrorFlags())
                  x = -x;
*/
               doc->SetUnknownVisAttrib(&lastinsert->getAttributesRef(), ATT_REFNAME, prosa, x, y,
                     rotation, siz, siz * 0.6, 0, 0, (vi & 1), SA_OVERWRITE, 0L, layer, TRUE, 0, 0); 
            
               // update refname 
               if (!lastinsert->getInsert()->hasRefnameData())
                  lastinsert->getInsert()->setRefname(STRDUP(prosa));
            }
            else if (lastnetnr > -1 && lastchar == 'S')  // if the label follows immediately a Segment record
            {
               layer = Graph_Level("NETNAME", "" , 0);

               DataStruct *data = Graph_Block_Reference("NETNAMEREF", NULL, 0, x , y, DegToRad(rotation), 0, 1.0, -1, 1);
               data->getInsert()->setInsertType(insertTypeUnknown);    // later we must add a INSERTTYPE NETNAMEPLACEHOLDER
               doc->SetUnknownVisAttrib(&data->getAttributesRef(), ATT_NETNAME, prosa, 0.0, 0.0, 0.0, 
                     siz, siz * 0.6, 0, 0, (vi & 1), SA_OVERWRITE, 0L, layer, TRUE, 0, 0); 
            
               int netptr;
               if ((netptr = get_netnameptr(lastnetnr)) < 0)
               {
                  fprintf(ferr, "Netnr [%d] not found in netarray\n", lastnetnr);
                  display_error++;
               }
               else
               {
                  netnamearray[netptr]->netname = prosa;
               }
            }
            else
            {
               DataStruct *data = Graph_Text(layer, prosa, x, y, siz, siz * 0.6, DegToRad(rotation),
                        0, FALSE, mirror, 0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
               data->getText()->setMirrorDisabled(true);
            }
         }
         break;
      case 'A':
         {
            int layer = Graph_Level("ATTRIBUTE","" ,0);

            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"X coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor;
            
            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"Y coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor;
            
            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"Attribute Size expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double siz = atof(lp) * scale_factor;  
            
            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"Attribute Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orie = atoi(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"Attribute Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orig = atoi(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  
            {
               fprintf(ferr,"Attribute View expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int vi = atoi(lp);   

            if ((lp = strtok(NULL,"=\n")) == NULL) 
            {
               fprintf(ferr,"Attribute Keyword expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            CString key = lp;
            key.TrimLeft();
            key.TrimRight();

            if (strlen(key) == 0)
            {
               fprintf(ferr,"Attribute has not string [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }

            CString val = "";
            if ((lp = strtok(NULL, "=\n")) != NULL)   
            {
               val = lp;
               val.TrimLeft();
               val.TrimRight();
            }

            int mirror = 0;
            double rotation = 0;
            double origx = x, origy = y;

            normalize_text(&x, &y, get_textjust(orig), rotation, mirror, siz, siz * 0.8 * strlen(val));

            if (lastinsert)
            {
               x = x - lastinsert->getInsert()->getOriginX();
               y = y - lastinsert->getInsert()->getOriginY();
/*
               if (lastinsert->getInsert()->getMirrorFlags())
                  x = -x;
*/
               doc->SetUnknownVisAttrib(&lastinsert->getAttributesRef(), key, val, x, y, rotation,
                     siz, siz*0.6, 0, 0, (vi & 1), SA_OVERWRITE, 0L, layer, TRUE, 0, 0); 
            
               // update refname 
               if (!STRCMPI(key, "REFDES"))
               {
                  if (!lastinsert->getInsert()->hasRefnameData())
                     lastinsert->getInsert()->setRefname(STRDUP(val));
                  else
                  {
                     fprintf(ferr, "Multiple Refdes [%s] found in [%s] at %ld\n", val, filename, ifp_linecnt);
                     display_error++;
                  }
               }
            }
            else
            {
               if (!(vi & 1))
               {
                  layer = Graph_Level("ATTRIBUTE_INVISIVBLE", "" ,0);
                  LayerStruct *l = doc->FindLayer(layer);
                  l->setVisible(false);
               }

               if (strlen(val))
               {
                  DataStruct *data = Graph_Text(layer, val, x, y, siz, siz * 0.6, DegToRad(rotation),
                        0, FALSE, mirror, 0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
                  data->getText()->setMirrorDisabled(true);
               }
            }
         }
         break;
      case 'U':
         {
            int layer = Graph_Level("ATTRIBUTE", "" ,0);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "X coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor; 
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Y coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor; 
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Attribute Size expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double siz = atof(lp) * scale_factor;
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Attribute Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orie = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Attribute Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orig = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Attribute View expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int vi = atoi(lp);   

            if ((lp = strtok(NULL, "=\n")) == NULL)   
            {
               fprintf(ferr, "Attribute Keyword expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            CString key = lp;
            key.TrimLeft();
            key.TrimRight();

            if (strlen(key) == 0)
            {
               fprintf(ferr, "Attribute has not string [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }

            // attribute does not need a value
            CString val = "";
            if ((lp = strtok(NULL, "=\n")) != NULL)   
            {
               val = lp;
               val.TrimLeft();
               val.TrimRight();
            }

            int mirror = 0;
            double rotation = 0;
            double origx = x, origy = y;

            normalize_text(&x, &y, get_textjust(orig), rotation, mirror, siz, siz * 0.8 * strlen(val));

            if (mainblock)
            {
               doc->SetUnknownVisAttrib(&mainblock->getAttributesRef(),key, val, x, y, rotation,
                     siz, siz * 0.6, 0, 0, (vi & 1), SA_OVERWRITE, 0L, layer, TRUE, 0, 0); 
            }
            else
            {
               if (!(vi & 1))
               {
                  layer = Graph_Level("ATTRIBUTE_INVISIVBLE", "" ,0);
                  LayerStruct *l = doc->FindLayer(layer);
                  l->setVisible(false);
               }

               if (strlen(val))
               {
                  DataStruct *data = Graph_Text(layer, val, x, y, siz, siz * 0.6, DegToRad(rotation),
                        0, FALSE, mirror, 0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
                  data->getText()->setMirrorDisabled(true);
               }
            }
         }
         break;
      case 'T':
         {
            int layer = Graph_Level("TEXT","" ,0);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "X coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor; 
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Y coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor; 
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Size expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double siz = atof(lp) * scale_factor;
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orie = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orig = atoi(lp);

            if ((lp = strtok(NULL, "\n")) == NULL) 
            {
               fprintf(ferr, "Label String expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            CString prosa = lp;
            prosa.TrimLeft();
            prosa.TrimRight();

            if (strlen(prosa) == 0)
            {
               fprintf(ferr, "Text has not string [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }

            int mirror = 0;
            double rotation = 0;

            switch (orie)
            {
            case 0:
               // normal
               rotation = 0;
               mirror = 0;
               break;
            case 1:
               rotation = 90;
               mirror = 0;
               break;
            case 2:
               rotation = 180;
               mirror = 0;
               break;
            case 3:
               rotation = 270;
               mirror = 0;
               break;
            case 4:
               rotation = 0;
               mirror = MIRROR_FLIP;
               x = x - siz * 0.8 * strlen(prosa);
               break;
            case 5:
               rotation = 90;
               mirror = MIRROR_FLIP;
               x = x - siz * 0.8 * strlen(prosa);
               break;
            case 6:
               rotation = 180;
               mirror = MIRROR_FLIP;
               x = x - siz * 0.8 * strlen(prosa);
               break;
            case 7:
               rotation = 270;
               mirror = MIRROR_FLIP;
               x = x - siz * 0.8 * strlen(prosa);
               break;
            }

            mirror = 0;

            normalize_text(&x,&y, get_textjust(orig), rotation, mirror, siz, siz * 0.8 * strlen(prosa));

            DataStruct *data = Graph_Text(layer, prosa, x, y, siz, siz*0.6, DegToRad(rotation),
                        0, FALSE, mirror, 0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
            data->getText()->setMirrorDisabled(true);
         }
         break;
      case 'I':
         {
            lastnetnr = -1;

            if ((lp = strtok(NULL, " \t\n")) == NULL) // INSERT id
            {
               fprintf(ferr, "id expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "name expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            CString name = lp;
            name.TrimLeft();
            name.TrimRight();

            if ((lp = strtok(NULL, " \t\n")) == NULL) // lib file version ???
            {
               fprintf(ferr, "sheet nr expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            name += ".";
            name += lp;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "X coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor; 
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Y coordinate expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor;
            
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Size expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            int orientation = atoi(lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Label Orientation expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double scale = atof(lp);
            double rotation = 0;
            int mirror = 0;

            switch (orientation)
            {
            case 0:
               // normal
               rotation = 0;
               mirror = 0;
               break;
            case 1:
               rotation = 90;
               mirror = 0;
               break;
            case 2:
               rotation = 180;
               mirror = 0;
               break;
            case 3:
               rotation = 270;
               mirror = 0;
               break;
            case 4:
               rotation = 0;
               mirror = MIRROR_FLIP;
               break;
            case 5:
               rotation = 90;
               mirror = MIRROR_FLIP;
               break;
            case 6:
               rotation = 180;
               mirror = MIRROR_FLIP;
               break;
            case 7:
               rotation = 270;
               mirror = MIRROR_FLIP;
               break;
            }

            // this name changes here is it refers to other library pathes.
            int insertptr = update_insert(name, filename); 

            lastinsert = Graph_Block_Reference(insertarray[insertptr]->name, NULL, 0, x, y, DegToRad(rotation), mirror, scale, 0, TRUE);
            lastinsert->getInsert()->setMirrorFlags(mirror);   // overwrite the bottom
            lastinsert->getInsert()->setInsertType(insertTypeSymbol);
            BlockStruct *block = doc->Find_Block_by_Num(lastinsert->getInsert()->getBlockNumber());
            block->setBlockType(BLOCKTYPE_SYMBOL);
         }
         break;
      case 'Q':
          //  color.
         break;
      case 'K':
         // magic number
         break;
      case 'c':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr," Circle x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Circle x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Circle x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double rad = atof(lp) * scale_factor;

            Graph_Circle(Graph_Level("CIRCLE", "", 0), x, y, rad, 0, 0, FALSE, FALSE);
         }
         break;
      case 'a':
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Arc x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x1 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Arc y expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y1 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Arc x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x2 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Arc y expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y2 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Arc x expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double x3 = atof(lp) * scale_factor;

            if ((lp = strtok(NULL, " \t\n")) == NULL) 
            {
               fprintf(ferr, "Arc y expected in [%s] at %ld\n", filename, ifp_linecnt);
               display_error++;
               break;
            }
            double y3 = atof(lp)*scale_factor;

            double cx, cy, r, sa, da;
            ArcPoint3(x1, y1, x2, y2, x3, y3, &cx, &cy, &r, &sa, &da);

            int layer = Graph_Level("ARC", "", 0);
            double bulge = tan(da/4);
            int closed = FALSE;

            Graph_PolyStruct(layer, 0L, 0);  
            Graph_Poly(NULL,0, 0, 0, closed);
            Graph_Vertex(x1, y1, bulge);
            Graph_Vertex(x3, y3, 0.0);
         }
         break;
      case 'C':
         // component pin ???
         // not needed.
         lastinsert = NULL;
         break;
      case 'X':
         // unattached component pin
         // not needed.
         lastinsert = NULL;
         break;
      case 'i':
         // last used Id
         // not needed.
         break;
      case 'E':
         logical_fileend = TRUE;
         break;
      default:
         fprintf(ferr, "Unknown Key [%c] in [%s] at %ld -> [%s]\n", lp[0], filename, ifp_linecnt, save_line);
         display_error++;
         break;
      }
      lastchar = lp1;
   } 
   return ymode;
}

/****************************************************************************/
/*
*/
static CString get_symbolversion(const char *f)
{
   CString symname = "";
   long hFile;
   struct _finddata_t c_file;
   CString s = f;
   s += ".*";

   if ( (hFile = _findfirst(s, &c_file)) == -1)
   {
      CString t;
      t.Format("No *.* files found in current directory!\n");
      ErrorMessage(t, "Error");
   }
   else
   {
      do
      {
         // because all files are looked up, also directories appear.
         if (c_file.attrib & _A_SUBDIR)
            continue;

         if (strlen(symname) == 0)
            symname = c_file.name;
         else
         {
            // get the one with the highest .x number
            // res = 0 is equal
            // res > 0 first is larger than second
            // res < 0 second is larger than first
            int res = compare_name(symname, c_file.name);
            if (res < 0)
               symname = c_file.name;

         }
      } while ( _findnext(hFile, &c_file) == 0);
   }

   return symname;
}

/******************************************************************************
* rename_netnames
*/
static int rename_netnames(CCEtoODBDoc *doc, FileStruct *file)
{
   for (int i=0; i<netnamecnt; i++)
   {
      CString nname;

      if (strlen(netnamearray[i]->netname))
         nname = netnamearray[i]->netname;
      else
         nname.Format("%d", netnamearray[i]->netnr);
      NetStruct *n = add_net(file,nname);
   }

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);
      
      if (data->getDataType() != T_POLY)
         continue;         

      Attrib *a =  is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0);
      if (a == NULL)
         continue;

      CString l = get_attvalue_string(doc, a);
      int netptr = get_netnameptr(atoi(l));

      if (netptr > -1 && strlen(netnamearray[netptr]->netname))
         doc->SetUnknownVisAttrib(&data->getAttributesRef(), ATT_NETNAME, netnamearray[netptr]->netname, a->getX(), a->getY(),
            a->getRotationRadians(), a->getHeight(), a->getWidth(), 0, 0, a->isVisible(), SA_OVERWRITE, 0L, a->getLayerIndex(), TRUE, 0, 0); 
   } 

   return 1;
}

/******************************************************************************
* cnv_tok
*/
static double cnv_tok(char *l)
{
   double x = 0.0;
   if (l && strlen(l))
   {
      x = atof(l);
      // we need this for rounding
      x = floor(x * scale_factor * 1000000.0 + 0.5) / 1000000.0;
   }

   return x;
}

