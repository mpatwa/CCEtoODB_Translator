// $Header: /CAMCAD/4.5/read_wrt/IgesIn.cpp 10    5/23/05 10:56a Lynn Phung $

/*****************************************************************************/
/*
    Project CAMCAD                          
    Router Solutions Inc.
    Copyright © 1994, 1998. All Rights Reserved.
*/

#include "stdafx.h"
#include "dbutil.h"
#include "format_s.h"
#include "file.h"
#include "geomlib.h"
#include "graph.h"
#include <math.h>
#include "gauge.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// Building Block includes
// needed for BuildingBlock software
#define SPLINE
//#define BBS_PRINT // debug to log file

extern "C"
{
#include <bbsdefs.h>
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <vpidefs.h>
#include <vp2defs.h>
#include <grrdefs.h>
#include <v2ddefs.h>
#include <dmldefs.h>
#include <dmdefs.h>
}

extern CProgressDlg *progress;

/****************************************************************************/
/*
*/
union vb
{
   char     buffer[100];
   struct Dir_type
   {
      char     ent[8];
      char     par[8];
      char     strct[8];
      char     font[8];
      char     level[8];
      char     view[8];
      char     matrix[8];
      char     lda[8];
      char     bs[2];
      char     ses[2];
      char     euf[2];
      char     hierarchy[2];
   } d;
   struct Dir2_type
   {
      char     ent[8];
      char     line_weight[8];
      char     color[8];
      char     line_count[8];
      char     form_num[8];
      char     res1[8];
      char     res2[8];
   } d2;
};

/*********************** Prototypes *****************************************/
/*
*/
static char *get_block_name(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start);
static void process_entity(FILE *fp, long D_Start, long P_Start);
static void do_text(  FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form);

static void do_subfigure(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight);
static void do_drawing_ent( FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight);
static void do_drawing_ent( FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight);

static void get_dir1_data(int *ent_typ,
                   long *p_ptr,
                   long *line_font,
                   long *view,
                   long *level,
                   long *matrix,
                   char ses[2],
                   union vb buff);
static void get_dir2_data(int *line_weight,
                   int *color,
                   long *line_count,
                   int *form,
                   union vb buff);

static int do_directory(FILE *fp,long D_Start, long P_Start, int *fp_cnt, int number_of_directory);

static void init_data(FILE *fp, long *D_Start, long *P_Start, 
                      int *number_of_global, int *number_of_directory);

static void do_lines( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_bsplines( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight,
               int  form);

static void do_copius( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_arc(   FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_compositecurve(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_associatitive_instance(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form);

static void do_sub_instance(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_transmatrix( FILE *fp,
                  long p_ptr,
                  long P_Start);

static void do_lindim(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_genlabel(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  lin_weight);

static void do_transmatrix( FILE *fp,
                  long p_ptr,
                  long P_Start);

static void do_leader( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form);

static int par_seek(FILE *fp, long p_ptr, long P_Start);
static int dir_seek(FILE *fp, long p_ptr, long P_Start);
static void do_global(FILE *fp, char *iline, int number_of_global);
static char *get_par(int par_num);
static char *get_buff_par(int par_num,char *buff);

/****************************************************************************/
/*
*/
#define MAX_LINE           165
#define MAX_DATA_RECORD    500000   // needs to be really big

#define REC_LEN            82

#define REC_TYPE           72
#define MAX_BUFF           2000
#define DEFAULT_LINE_WIDTH 0.0

/******************* Globals ************************************************/
/*
*/
static   char  Global_Buff[MAX_BUFF];
static   int   Unit_Flag;
static   int   Max_Grad;
static   double Max_Width;
static   char   *data_record;

#define  MAX_POLY          4096     // I have seen over 255 vertex cnts.

typedef struct
{
   double   x,y, radius;
   char     typ;     //
   char     plot;
   char     plotabs; // plot absolute
} Polyg;

/*****************************************************************************/

static   FILE     *ifp;
static int  display_error, display_log;
static FILE *flog;

/****************************************************************************/
/*
   Global definitions
*/
#define  SMALLUNIT         0.0000001

static   Mat3x3 transform;    /* this is the current transformation matrix */
static   Point3 translate;    /* this is the current offset */

static    int  rec_len = REC_LEN;   /* this is for debug */

/****************************************************************************/

static   int   get_record(FILE *fp);

/****************************************************************************/
/*
   get a number of characters without stopping on 0A, 0D
*/
static   char *FGETS( char *string, int n, FILE *stream )
{
   int   cnt = 0;
   int   ch;

   while (cnt < n)
   {
      ch = fgetc(stream );
      string[cnt] = ch;
      cnt++;
   }
   string[cnt] = '\0';

   return string;
}

/****************************************************************************/
/*
*/
static char *get_entityname(int num)
{
   switch (num)
   {
      case 116:
         return "Point";
      break;
      case 120:
         return "Surface of Revolution";
      break;
      case 128:
         return "Rational B-spline Surface";
      break;
      case 144:
         return "Trimmed Parametric Surface";
      break;
      case 320:
         return "Network Subfigure Definition";
      break;
      case 420:
         return "Network Subfigure Instance";
      break;
   }

   return   "Unknown";
}

/****************************************************************************/
/*
*/
static int load_igessettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"HPGL Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
      }
   }

   fclose(fp);
   return 1;
}

/******************************************************************************
* ReadIGES
*/
void ReadIGES(const char *f, FormatStruct *Format, double scaleUnitsFactor, int PageUnits)
{
   FileStruct *file;
   long  command_cnt = 0;

   display_error = FALSE;
   display_log   = TRUE;

   CString logFile = GetLogfilePath("iges.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage(logFile, "Can not open Logfile !", MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   CString settingsFile( getApp().getImportSettingsFilePath("iges.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nIGES Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_igessettings(settingsFile);

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(f,"rb")) == NULL)
   {
      ErrorMessage("Error open file", f);
      return;
   }

   file = Graph_File_Start(f, fileTypeIges);

   char  iline[MAX_LINE];
   long  D_Start = 0,P_Start = 0;
   int   fp_cnt= 0,ss=0;
   int   number_of_global = 0;
   int   number_of_directory = 0;
   char  *lp;
   int   res;

   if ((data_record = (char *)calloc(MAX_DATA_RECORD, sizeof(char))) == NULL)
   {
      ErrorMessage("Memory error\n");
      return;
   }

   init_data(ifp, &D_Start, &P_Start, &number_of_global, &number_of_directory);

   fseek(ifp, 0, SEEK_SET);   // reset to beginning

   while ((lp=FGETS(iline, rec_len, ifp)) != NULL)
   {
      iline[80] = '\0';
      if (iline[0] == '\n') continue;
      if (iline[REC_TYPE] == 'P') break;

      switch (iline[REC_TYPE])
      {
         case 'S':
            if ( ss == 0)
            {
               //printf("Begin IGES in for [%s].\n",f);
            }
            ss++;
         break;
         case 'G':
            do_global(ifp,iline, number_of_global);
         break;
         case 'D':
            //printf("Processing Directory Entries:\n");
            res = do_directory(ifp,D_Start,P_Start, &fp_cnt, number_of_directory);
            if (res == 1)  break;
            if (res < 0)
            {
               // big error in do_directory
               CString  tmp;
               tmp.Format("Error in Directory Structure in sub do_directory\n");
               ErrorMessage(tmp);
               break;
            }
         break;
         default :
            CString  tmp;
            tmp.Format("Error unknown SubDirectory [%c]\n",iline[REC_TYPE]);
            ErrorMessage(tmp);
         break;
      }   /* end switch */
   }

   free(data_record);

   fprintf(flog,"Total IGES Records Read = %d\n",fp_cnt);

   fclose(ifp);

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*    init_data - Read the Terminate record to get the record counts
                  to determine start position for Directory and Parameter
                  SubDirectories.
*/

/****************************************************************************/
/*  
*/
void init_data(FILE *fp, long *D_Start, long *P_Start, int *G_num, int *D_num)
{
   int   i;
   char  iline[MAX_LINE];
   unsigned int   S,G,D,P;
   char  *lp;

   S = G = D = P = 0;

   /* self test record length */
   lp=fgets(iline,MAX_LINE,fp);

   if (strlen(iline) > REC_LEN)  // if line is longer 82, than it has no line end.
                                 // this is allowed in IGES, 80 char continuously
      rec_len = 80;
   else
      rec_len = strlen(iline);

   if ((i=fseek(fp,-rec_len,SEEK_END)) != NULL)
   {
      CString  tmp;
      tmp.Format("Error in fseek in sub init_data.\n");
      ErrorMessage(tmp);
      return;
   }

   lp=FGETS(iline,rec_len,fp);
   iline[80] = '\0';

   if (iline[0] == '\n')
   {
      lp=fgets(iline,rec_len,fp);
      iline[80] = '\0';
   }
   if (iline[0] == 'S')
   {
      sscanf(iline,"S%dG%dD%dP%d",&S,&G,&D,&P);
   }
   else
   {
      rewind(fp);
      while ((lp=fgets(iline,rec_len,fp)) != NULL)
      {
         iline[80] = '\0';
         if (iline[0] == '\n') continue;
         if (iline[REC_TYPE] == 'S') S++;
         if (iline[REC_TYPE] == 'G') G++;
         if (iline[REC_TYPE] == 'D') D++;
         if (iline[REC_TYPE] == 'P') P++;
      }
   }
   *P_Start = (long)(S + G + D) * rec_len;
   *D_Start = (long)(S + G) * rec_len;
   *G_num = G;
   *D_num = D;
}

/****************************************************************************/
/*    do_directory() - Read the Directory Entries and Process them
*/
/****************************************************************************/
/*  */
int do_directory(FILE *fp, long D_Start, long P_Start, int *fp_cnt, int number_of_directory)
{
   int   i;
   char  *lp;
   long  p_ptr,line_font,level,view,matrix;
   int   ent_typ,form;
   int   line_weight;
   int   color;
   long  line_count;
   char  ses[2];
   union vb buff;
   union vb buff2;
   long  param_number, param_number2;
   int   dir_cnt=0;

   if ((i=fseek(fp,D_Start,SEEK_SET)) != NULL)
   {
      CString  tmp;
      tmp.Format("Error in fseek in sub do_directory.\n");
      ErrorMessage(tmp);
      return -1;
   }

   while (dir_cnt < number_of_directory)  // loop thru all directory until a break;
   {
      if ((lp=FGETS(buff.buffer, rec_len, fp)) == NULL)
      {
         CString  tmp;
         tmp.Format("Error in Directory Structure in sub do_directory\n");
         ErrorMessage(tmp);
         return -1;
      }

      dir_cnt++;
      buff.buffer[80] = '\0';
      param_number = atol(&(lp[73]));

      if (param_number != dir_cnt)  // this checks that the lines are consequtive
      {
         CString  tmp;
         tmp.Format("Error in Directory Structure in sub do_directory\n");
         ErrorMessage(tmp);
         return -1;
      }

      if (buff.buffer[REC_TYPE] != 'D') 
      {
         CString  tmp;
         tmp.Format("Error in Directory Structure in sub do_directory\n");
         ErrorMessage(tmp);
         return -1;
      }

      if ((lp=FGETS(buff2.buffer, rec_len, fp)) == NULL)
      {
         CString  tmp;
         tmp.Format("Error in Directory Structure in sub do_directory\n");
         ErrorMessage(tmp);
         return -1;
      }
      dir_cnt++;
      buff2.buffer[80] = '\0';
      param_number2 = atol(&(lp[73]));
      if (param_number2 != dir_cnt)
      {
         CString  tmp;
         tmp.Format("Error in Directory Structure in sub do_directory\n");
         ErrorMessage(tmp);
         return -1;
      }

      get_dir1_data(&ent_typ,&p_ptr,&line_font,&view,&level,&matrix,ses,buff);
      get_dir2_data(&line_weight,&color,&line_count,&form,buff2);

      *fp_cnt=(*fp_cnt) + 1;

      //printf(" Ent #%-6d = %-6d : %-8d\r", *fp_cnt , ent_typ,param_number);

/****************************************************************************/
/*
    If Subordinate Entity Switch (ses)
    is set to 1 or 3 this is part of a Subfigure.
    Therefore do not Process it.
*/
/****************************************************************************/

      // this is entity use flag 5-6 (6). 
      if (buff.buffer[71] == '1')   
      {
         // do not evaluate the ses subordinate entity switch if the dependency is annotate
      }
      else
      {
         if ((ses[1] == '1') || (ses[1] == '3'))
            continue;
      }

      long cur_pos=ftell(fp); // save directory file pointer

      do_transmatrix(fp,matrix,P_Start);
      switch (ent_typ)
      {
         case 100: /* Arc */
            do_arc(fp,p_ptr,P_Start,level,line_font,color,line_weight);
         break;
         case 102: /* composite curve */
            do_compositecurve(fp,p_ptr,P_Start,D_Start, level,line_font,color,line_weight);
         break;
         case 106:  /* Copius Data */
            do_copius(fp,p_ptr,P_Start,level,line_font,color,line_weight);
         break;
         case 110:  /* Lines */
            do_lines(fp,p_ptr,P_Start,level,line_font,color,line_weight);
         break;
         case 124:
            // transformation matrix. This is never done alone, but only in do_transmatrix
         break;
         case 126:  /* Bsplines */
            do_bsplines(fp,p_ptr,P_Start,level,line_font,color,line_weight, form);
         break;
         case 210:  /* General Label */
            do_genlabel(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 212:  /* General Note */
            do_text(fp,p_ptr,P_Start,level,line_font,color,line_weight,form);
         break;
         case 214:  /* Leader Entity */
            do_leader(fp,p_ptr,P_Start,level,line_font,color,line_weight,form);
         break;
         case 216:  /* Linear Dimension */
            do_lindim(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 304:  /* Line Font Definition */
         break;
         case 308:  /* Subfigure */
            do_subfigure(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 314:
            // color definition
         break;
         case 402:  /* Associativity Instance Entity */
            do_associatitive_instance(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight, form);
         break;
         case 404:  /* Drawing Entity */
            do_drawing_ent(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 406:  /* Property */
         break;
         case 408:  /* Singular Subfigure Instance */
            do_sub_instance(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 410:  /* View */
         break;
         default :
            fprintf(flog,"Unknown Entity=[%d] [%s]\n",ent_typ, get_entityname(ent_typ));
            display_error++;
         break;
      }  /* end switch */
      fseek(fp,cur_pos,SEEK_SET);   // set back to directory file pointer
   }
   return 1;
}

/****************************************************************************/
/*    process_entity() - Read the Directory Entries and Process them
*/
/****************************************************************************/
/*  */
void process_entity(FILE *fp, long D_Start, long P_Start)
{
   char  *lp;
   long  p_ptr,line_font,level,view,matrix;
   int   ent_typ,form;
   int   line_weight,color;
   long  line_count;
   char  ses[2];
   union vb buff;
   union vb buff2;

   if((lp=FGETS(buff.buffer, rec_len,fp)) != NULL)
   {
      buff.buffer[80] = '\0';    
      if ((lp=FGETS(buff2.buffer, rec_len,fp)) == NULL)
      {
         CString  tmp;
         tmp.Format("Error in Directory Structure in sub process_entity\n");
         ErrorMessage(tmp);
         return;
      }
      buff2.buffer[80] = '\0';      

      get_dir1_data(&ent_typ,&p_ptr,&line_font,&view,&level,&matrix,ses,buff);
      get_dir2_data(&line_weight,&color,&line_count,&form,buff2);

      switch (ent_typ)
      {
         case 100: /* Arc */
            do_arc(fp,p_ptr,P_Start,level,line_font,color,line_weight);
         break;
         case 102: /* composite curve */
            do_compositecurve(fp,p_ptr,P_Start,D_Start, level,line_font,color,line_weight);
         break;
         case 106:  /* Copius Data */
            do_copius(fp,p_ptr,P_Start,level,line_font,color,line_weight);
         break;
         case 110:  /* Lines */
            do_lines(fp,p_ptr,P_Start,level,line_font,color,line_weight);
         break;
         case 124:
            // transformation matrix. This is never done alone, but only in do_transmatrix
         break;
         case 126:  /* Bsplines */
            do_bsplines(fp,p_ptr,P_Start,level,line_font,color,line_weight, form);
         break;
         case 212:  /* General Note */
            do_text(fp,p_ptr,P_Start,level,line_font,color,line_weight,form);
         break;
         case 214:  /* Leader Entity */
            do_leader(fp,p_ptr,P_Start,level,line_font,color,line_weight,form);
         break;
         case 304:  /* Line Font Definition */
         break;
         case 308:  /* Subfigure */
            do_subfigure(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 314:
            // color definition
         break;
         case 402:  /* Associativity Instance Entity */
            do_associatitive_instance(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight, form);
         break;
         case 406:  /* Property */
         break;
         case 408:  /* Singular Subfigure Instance */
            do_sub_instance(fp,p_ptr,P_Start,D_Start,level,line_font,color,line_weight);
         break;
         case 410:  /* View */
         break;
         default :
            fprintf(flog,"Unknown Entity=[%d] [%s]\n",ent_typ, get_entityname(ent_typ));
            display_error++;
         break;
      }  /* end switch */
   }
   else
   {
      return;  /* EOF */
   }

   return;
}

/****************************************************************************/
/*    do_lines() - Write out line segments
*/
/****************************************************************************/
/*  */
void do_lines( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i;
   long  cur_pos;
   int   ent;
   char  clevel[20];
   double   x1,y1,z1,x2,y2,z2;
   double   width;
   char     *lp;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_lines\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);
   while((lp=strchr(data_record,'D')) != NULL) *lp='E';
   i=sscanf(data_record,"%d,%lE,%lE,%lE,%lE,%lE,%lE",&ent,&x1,&y1,&z1,&x2,&y2,&z2);

   if (i == 7)
   {
      sprintf(clevel,"%d",level);
/*
      Graph_Color(color);
*/
      int cl= Graph_Level(clevel, "", 0);
/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
         width=DEFAULT_LINE_WIDTH;

      int   err;
      int   wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_WIDTH, FALSE, &err);

      Graph_Line(cl, x1, y1, x2, y2, 0, wi, FALSE);
   }
   else
   {
      CString  tmp;
      tmp.Format("Error Bad Data format in sub do_lines().\nData=[%s]\n",data_record);
      ErrorMessage(tmp);
      return;
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_bsplines() - Write out line segments

#define __MCRSFTC__ 
#define __WINDOWS__ 
#define __WIN32__
#define SPLINE

#include <conio.h>
#include <math.h>

extern "C"
{
#include <c2cdefs.h>
#include <c2ddefs.h>
}

void FillArrays ( HPT2 *ctrlpt, INT a, REAL *knot, INT k );

void main ( void )
{
   C2_CURVE spline;
   HPT2 *ctrlpt; // homogeneous control points
   INT n, d, k, m, a;
   REAL *knot;

   k = 6; // IGES data; number of control points (0-based)
   m = 3; // IGES data
   n = k - m + 1; // derived from IGES data

   a = n + 2 * m; // number of knots
   d = m + 1; // order

   // 'a' knots:

   knot = MALLOC ( a, REAL );

   // 'k' homogeneous control points

   ctrlpt = MALLOC ( k, HPT2 );

   FillArrays ( ctrlpt, a, knot, k );
   
   spline = c2d_nurb ( ctrlpt, n, d, knot );

   c2c_info_curve ( spline, NULL );

   getch();
}

void FillArrays ( HPT2 *ctrlpt, INT a, REAL *knot, INT k )
{
   // knots

   knot[0] = 43.082763671875e0;
   knot[1] = 43.082763671875e0; 
   knot[2] = 43.082763671875e0;
   knot[3] = 43.082763671875e0;
   knot[4] = 147.764343261719e0;
   knot[5] = 261.910491943359e0;
   knot[6] = 351.596771240234e0;
   knot[7] = 450.747619628906e0;
   knot[8] = 450.747619628906e0;
   knot[9] = 450.747619628906e0;
   knot[10] = 450.747619628906e0;

   // weights
   
   ctrlpt[0][2] = 1.0e0;
   ctrlpt[1][2] = 1.0e0;
   ctrlpt[2][2] = 1.0e0;
   ctrlpt[3][2] = 1.0e0;
   ctrlpt[4][2] = 1.0e0;
   ctrlpt[5][2] = 1.0e0;
   ctrlpt[6][2] = 1.0e0;

   // control points

   ctrlpt[0][0] = 1080.666015625e0;
   ctrlpt[0][1] = 0.0e0;
   // 968.703857421875e0; // z coordinate; not used
   ctrlpt[1][0] = 1080.66638183594e0;
   ctrlpt[1][1] = 34.8954658508301e0;
   // 968.703979492188e0; // z coordinate; not used
   ctrlpt[2][0] = 1081.63134765625e0;
   ctrlpt[2][1] = 107.834938049316e0;
   // 968.211975097656e0; // z coordinate; not used
   ctrlpt[3][0] = 1085.61926269531e0;
   ctrlpt[3][1] = 210.586059570313e0;
   // 965.958068847656e0; // z coordinate; not used
   ctrlpt[4][0] = 1092.08312988281e0;
   ctrlpt[4][1] = 311.295104980469e0;
   // 961.692749023438e0; // z coordinate; not used
   ctrlpt[5][0] = 1097.75012207031e0;
   ctrlpt[5][1] = 373.846984863281e0;
   // 957.254150390625e0; // z coordinate; not used
   ctrlpt[6][0] = 1101.24584960938e0;
   ctrlpt[6][1] = 406.551177978516e0;
   // 953.997802734375e0; // z coordinate; not used

}

*/

/****************************************************************************/
/*  
*/
DML_LIST spline_smash ( C2_CURVE spline )
{
   PT2 pt0, pt1;
   PARM_S parm;
   INT i;
   DML_LIST list = dml_create_list();
   C2_CURVE line;

   PARM_T(&parm) = 0.0;

   c2c_eval_pt ( spline, &parm, pt0 );

   //printf ( "%lf\t%lf\n", PT2_X(pt0), PT2_Y(pt0) );

   for ( i = 0; i <= 16; i++ )
   {
      PARM_T(&parm) = ( REAL ) i / 16.0;

      c2c_eval_pt ( spline, &parm, pt1 );

      //printf ( "%lf\t%lf\n", PT2_X(pt1), PT2_Y(pt1) );

      line = c2d_line ( pt0, pt1 );

      dml_append_data ( list, ( ANY ) line );

      c2v_copy ( pt1, pt0 );
   }

   return list;
}

/****************************************************************************/
/*  
*/
void do_bsplines( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form)
{
   int      i;
   long     cur_pos;
   char     clevel[20];
   double   x,y,z;
   double   width;
   char     *lp;
   int      k, m, n;
   int      prop1, prop2, prop3, prop4;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_lines\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);
   while((lp=strchr(data_record,'D')) != NULL) *lp='E';

   lp = strtok(data_record,";,");

   lp = strtok(NULL,";,");
   k = atoi(lp) +1;  // 0 based in IGES

   lp = strtok(NULL,";,");
   m = atoi(lp);

   lp = strtok(NULL,";,");
   prop1 = atoi(lp);

   lp = strtok(NULL,";,");
   prop2 = atoi(lp);

   lp = strtok(NULL,";,");
   prop3 = atoi(lp);

   lp = strtok(NULL,";,");
   prop4 = atoi(lp);

   n = k-m+1;

   REAL *knot;
   HPT2 *ctrlpt; // homogeneous control points

// double   knots[255];
// double   weights[255];
// int      kcnt = 0;
// int      wcnt = 0;
   int      a, d;
   a = n + 2 * m; // number of knots
   d = m + 1; // order

   // 'a' knots:
   knot = MALLOC ( a, REAL );

   // 'k' homogeneous control points
   ctrlpt = MALLOC ( k, HPT2 );

   // get all knots
   //for (i=-m;i<=n+m;i++)
   for (i=0;i<a;i++)
   {
      lp = strtok(NULL,";,");
      //knot[kcnt++] = atof(lp);
      knot[i] = atof(lp);
   }

   // get all weights
   for (i=0;i<k;i++)
   {
      lp = strtok(NULL,";,");
      ctrlpt[i][2] = atof(lp);
   }

   // get all control points
   for (i=0;i<k;i++)
   {
      lp = strtok(NULL,";,");
      x = atof(lp);
      lp = strtok(NULL,";,");
      y = atof(lp);
      lp = strtok(NULL,";,");
      z = atof(lp);
      ctrlpt[i][0] = x;
      ctrlpt[i][1] = y;
      // ctrlpt[i][2] done in weights
   }


   C2_CURVE spline;
   //HPT2 *ctrlpt; // homogeneous control points
   //INT n, d, k, m, a;
   //REAL *knot;
   spline = c2d_nurb ( ctrlpt, n, d, knot );

/*
   //#define BBS_PRINT // debug to log file

   c2c_info_curve ( spline, flog );
   display_error++;
*/

   PT2         pt0, pt1;
   DML_LIST    seg = NULL;

   if ( C2_CURVE_IS_PCURVE(spline) == TRUE )
   {
      seg = c2d_pcurve_smash(spline , NULL);
   }
   else 
   if ( C2_CURVE_IS_SPLINE(spline) == TRUE )
   {
      seg = spline_smash ( spline );
   }
   else // are there other types of curves you want to support?
   {
      ErrorMessage("Other Curve not supported in bspline");
   }

   if (seg)
   {

      PT2   center;
      REAL  radius, st_angle, sweep;
      INT   dir;
      BOOLEAN  is_arc;
      DML_ITEM item;
      C2_CURVE curve;
   
      // walk down seg to examine each segment
      width=0.0;
      //Graph_Line_Width(width);

      sprintf(clevel,"%d",level);
      int cl= Graph_Level(clevel, "", 0);
/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/

      int   err;
      int   wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_WIDTH, FALSE, &err);

      Graph_PolyStruct(cl, 0, FALSE);
      Graph_Poly(NULL, wi, FALSE,0,1);
      
      DML_WALK_LIST ( seg, item )
      {
         curve = (C2_CURVE) dml_record ( item );

         if ( C2_CURVE_IS_LINE(curve) )
         {
            is_arc = FALSE;
            c2c_ept0 ( curve, pt0 );
            c2c_ept1 ( curve, pt1 );
            Graph_Vertex(pt0[0], pt0[1], 0.0); // p1 coords
            // do not write the last one
            //Graph_Vertex(pt1[0], pt1[1], 0.0); // p1 coords
         }
         else if ( C2_CURVE_IS_ARC(curve) )
         {
            double   bulge;
            is_arc = TRUE;
   
            c2c_ept0 ( curve, pt0 );
            c2c_ept1 ( curve, pt1 );

            c2c_get_arc_data ( curve, center, &radius, &st_angle, &sweep, &dir );
            bulge = tan(sweep/4);
            Graph_Vertex(pt0[0], pt0[1], bulge); // p1 coords
            // do not write the last one
            //Graph_Vertex(pt1[0], pt1[1], 0.0); // p1 coords
         }
         else if ( C2_CURVE_IS_PCURVE(curve) )
         {
            // should never happen
         }
         else
         {
            // also should never happen
         }
      }
      // last one
      Graph_Vertex(pt1[0], pt1[1], 0.0); // p1 coords

      dml_destroy_list( seg, (PF_ACTION)c2d_free_curve);
   }  // seg == NULL

   c2d_free_curve( spline );

   free(knot);
   free(ctrlpt);

/*
   CString  f;
   switch (form)
   {
      case 0 :
         f = "bspline";
      break;
      case 1 :
         f = "line";
      break;
      case 2 :
         f = "circle";
      break;
      case 3 :
         f = "ellipse";
      break;
      case 4 :
         f = "parable";
      break;
      case 5 :
         f = "hyperbol";
      break;
      default:
         f = "unknown";
      break;
   }


   // starting value
   // end value
   // x, y, z unit normal
   fprintf(flog, "BSpline [Type=%s, %d] - not implementation\n", f, form);
   display_error++;
*/
#ifdef WOLF
   if (i == 7)
   {
      sprintf(clevel,"%d",level);
/*
      Graph_Color(color);
*/
      int cl= Graph_Level(clevel, "", 0);
/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
         width=DEFAULT_LINE_WIDTH;

      int   err;
      int   wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_WIDTH, FALSE, &err);

      Graph_Line(cl, x1, y1, x2, y2, 0, wi, FALSE);
   }
   else
   {
      CString  tmp;
      tmp.Format("Error Bad Data format in sub do_lines().\nData=[%s]\n",data_record);
      ErrorMessage(tmp);
      return;
   }
#endif
   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_lines() - Write out line segments
*/
/****************************************************************************/
/*  */
void do_copius( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i,ip;
   long  cur_pos;
   int   ent;
   char  clevel[20];
   Point3   p1,p2;
   double   width;
   char     *lp;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_lines\n");
      ErrorMessage(tmp);
   }

   if (get_record(fp) < 0) return;  // error in get_record

   while((lp=strchr(data_record,'D')) != NULL) *lp='E';
   lp = strtok(data_record,";,");

   lp = strtok(NULL,";,");
   ip = atoi(lp);

   /* only ip 1 of supported */
   if (ip != 1)
   {
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }
   lp = strtok(NULL,";,");
   ent = atoi(lp);

   lp = strtok(NULL,";,");
   p1.z = atof(lp);
   lp = strtok(NULL,";,");
   p1.x = atof(lp);
   lp = strtok(NULL,";,");
   p1.y = atof(lp);
   ent--; /* one already done ! */

/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
      width=0.0;
   //Graph_Line_Width(width);

   sprintf(clevel,"%d",level);
   int cl= Graph_Level(clevel, "", 0);
/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
        width=DEFAULT_LINE_WIDTH;

   int   err;
   int   wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_WIDTH, FALSE, &err);

   Graph_PolyStruct(cl, 0, 0 );
   Graph_Poly(NULL, wi, 0,0,0);
   Graph_Vertex(p1.x, p1.y, 0);

   for (i=0;i<ent;i++)
   {
      lp = strtok(NULL,";,");
      p2.x = atof(lp);
      lp = strtok(NULL,";,");
      p2.y = atof(lp);
      p2.z = p1.z;

      Graph_Vertex(p2.x, p2.y, 0.0);
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_lines() - Write out line segments
*/
/****************************************************************************/
/*  */
void do_leader( FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form)
{
   int      i,segnr;
   long     cur_pos;
   char     clevel[20];
   Point3   p1,p2;
   double   width,arrow_height,arrow_width,a,x;
   char     *lp;
   Point2   arrow[4];
   Mat2x2   m;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_lines\n");
      ErrorMessage(tmp);
   }

   get_record(fp);
   while((lp=strchr(data_record,'D')) != NULL) *lp='E';
   lp = strtok(data_record,",;");

   if ((lp = strtok(NULL,",;")) == NULL) /* number of segments */
   {
      fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
      display_error++;
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   segnr = atoi(lp);
   if ((lp = strtok(NULL,",;")) == NULL) /* height */
   {
      fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
      display_error++;
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   arrow_height = atof(lp);
   if ((lp = strtok(NULL,",;")) == NULL) /* width */
   {
      fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
      display_error++;
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   arrow_width = atof(lp);
   if ((lp = strtok(NULL,",;")) == NULL) /* z */
   {
      fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
      display_error++;
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   p2.z = p1.z = atof(lp);
   if ((lp = strtok(NULL,",;")) == NULL) /* x Arrow head */
   {
      fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
      display_error++;
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   p1.x = atof(lp);
   if ((lp = strtok(NULL,",;")) == NULL) /* y */
   {
      fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
      display_error++;
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   p1.y = atof(lp);

/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
         width=0.0;
      //Graph_Line_Width(width);

   sprintf(clevel,"%d",level);
   int cl= Graph_Level(clevel, "", 0);
/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
        width=DEFAULT_LINE_WIDTH;

   int   err;
   int   wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_WIDTH, FALSE, &err);

   Graph_PolyStruct(cl, 0, 0 );
   Graph_Poly(NULL, wi, 0,0,0);
   Graph_Vertex(p1.x, p1.y, 0);

   /* dimension line from Point1 to Point2 and than
      from point1 to point3 to point4 etc */

   for (i=0;i<segnr;i++)
   {
      if ((lp = strtok(NULL,",;")) == NULL) /* x Seg tail */
      {
         fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
         display_error++;
         break; 
      }
      p2.x = atof(lp);
      if ((lp = strtok(NULL,",;")) == NULL)  /* Y */
      {
         fprintf(flog,"Invalid IGES file - Data Error in [%ld]\n", P_Start);
         display_error++;
         break; 
      }
      p2.y = atof(lp);
      TransPoint3( &p2, 1, &transform, translate.x, translate.y, translate.z );
      sprintf(clevel,"%d",level);
/*
       Graph_Color(color);
*/

      if (i == 0)
      {
         /* make arrowhead */
         switch (form)
         {
      
            case  11:   /* open triangle */
            default:
               arrow[0].x = arrow[0].y = 0;
               arrow[1].x = arrow_height;
               arrow[1].y = -arrow_width/2;
               arrow[2].x = arrow_height;
               arrow[2].y = arrow_width/2;
               arrow[3].x = arrow[3].y = 0;
               a = ArcTan2((p2.y - p1.y),(p2.x-p1.x));
               RotMat2( &m, a );
               TransPoint2( arrow, 4, &m, p1.x,p1.y );
               //Graph_Polyline(arrow,4,p2.z);
               /* push back p1 */
               x = cos(a)*arrow_height;
               p1.x += x;
               x = sin(a)*arrow_height;
               p1.y += x;
            break;
         }
      }
      Graph_Vertex(p2.x, p2.y, 0);
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_text() - Write out text
*/
/****************************************************************************/
/*  */
void do_text(  FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form)
{
   int   i;
   long  cur_pos;
   char  clevel[20];
   double   x1,y1,z1,rot_angle;
   double   height,width,fc,sl1;
   int   ns,nc1,mirror,text_rot;
   char  *first_H,*lp;
   char  text_string[MAX_LINE];
   char  *lp1,*lp2;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_text\n");
      ErrorMessage(tmp);

      fseek(fp,cur_pos,SEEK_SET);
      return;
   }

   get_record(fp);

   if (strlen(data_record) > 1999)
   {
      CString  tmp;
      tmp.Format("Length error in do_text\n");
      ErrorMessage(tmp);
      return;
   }

   strcpy(Global_Buff,data_record);
   if((first_H=strchr(Global_Buff,'H')) == NULL)
   {
      fseek(fp,cur_pos,SEEK_SET);
      return;
   }
   for (lp=Global_Buff; lp < first_H; lp++)
   {
      if (*lp == 'D') *lp = 'E';
   }
   lp=get_par(2);
   ns=atoi(lp);   /* number of strings */
   lp=get_par(3);
   nc1=atoi(lp);  /* number of characters in the first string */
   lp=get_par(9);
   mirror=atoi(lp);  /* mirror flag : */
                     /* 0 = No Mirror */
                     /* 1 = mirror perpendicular to the text base line axis */
                     /* 2 = mirror is the text base line axis */

   lp=get_par(10);
   text_rot=atoi(lp);/* Rotate internal text flag  */
                     /* 0 = Text Horizontal        */
                     /* 1 = Text Vertical          */

   lp=get_par(4);
   width=atof(lp);   /* Text Width */

   lp=get_par(5);
   height=atof(lp);  /* Text Height */

   lp=get_par(6);
   fc=atof(lp);      /* Font Characteristic */
   lp=get_par(7);
   sl1=atof(lp);     /* Slant Angle for Text in radians */

   lp=get_par(8);
   rot_angle=atof(lp); /* angle of rotation for text in radians */

   lp=get_par(11);
   x1=atof(lp);      /* x co-ord */

   lp=get_par(12);
   y1=atof(lp);      /* y co-ord */

   lp=get_par(13);
   z1=atof(lp);      /* z co-ord */
   lp=get_par(14);
   for (i=0; i < ns; i++)
   {
//    lp=data_record;
      lp=first_H;
      lp++;
      if (nc1 > MAX_LINE)
      {
         fprintf(flog,"Text larger than buffer. Truncated to %d characters.\n",MAX_LINE);
         display_error++;
      }

      if (nc1 < 1)  /* No charaters to print */
      {
         break;
      }
      strncpy(text_string,lp,nc1);
      text_string[nc1] = '\0';

      sprintf(clevel,"%d",level);
      int cl= Graph_Level(clevel, "", 0);

      lp1 = text_string;

      while ((lp2 = strtok(lp1,"\n")) != NULL)
      {
         int   proportional = 1;
         int   mir = 0;
         int   textoblique = 0;
         double   charwidth = width/strlen(lp2);

         if (mirror == 1)  mir = TRUE;

         Graph_Text(cl, lp2, x1, y1, 
                  height, charwidth, rot_angle, 0, proportional, mir, 
                  textoblique, FALSE, -1, 0); // supposed to be h,w,ang,charwidth,flg

         lp1 = NULL;
         y1 -= height;
      }
      lp=get_buff_par(1, first_H);
      nc1=atoi(lp);
      first_H=strchr(first_H,'H');
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_arc() - Write out Circles and Arcs
*/
/****************************************************************************/
/*  */
void do_arc(   FILE *fp,
               long p_ptr,
               long P_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i;
   long  cur_pos;
   int   ent;
   char  clevel[20];
   double   y3,x1,y1,zt,x2,y2,x3;
   double   radius,start_angle,delta_angle;
   double   width;
   char     *lp;
   Point3 arccenter;
   Point3 arcstart;
   Point3 arcstop;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_circle\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);
   while((lp=strchr(data_record,'D')) != NULL) *lp='E';

   i=sscanf(data_record,"%d,%lE,%lE,%lE,%lE,%lE,%lE,%lE",&ent,&zt,&x1,&y1,&x2,&y2,&x3,&y3);

   arccenter.z = arcstart.z = arcstop.z = zt;
   arccenter.x = x1;
   arccenter.y = y1;
   arcstart.x = x2;
   arcstart.y = y2;
   arcstop.x = x3;
   arcstop.y = y3;

   if (i == 8)
   {
      sprintf(clevel,"%d",level);
/*
      Graph_Color(color);
*/
      //Graph_Level(clevel);
/*
      if (line_weight > 0)
         width=line_weight * (Max_Width / Max_Grad);
      else
*/
         width=DEFAULT_LINE_WIDTH;
      //Graph_Line_Width(width);

      int cl= Graph_Level(clevel, "", 0);

      int   err;
      int   wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_WIDTH, FALSE, &err);

      TransPoint3( &arccenter, 1, &transform, translate.x, translate.y, translate.z );
      TransPoint3( &arcstart, 1, &transform, translate.x, translate.y, translate.z );
      TransPoint3( &arcstop, 1, &transform, translate.x, translate.y, translate.z );

      if( transform.m3[2][2] < 0 ) 
      {
         /* arc mirrored about the xy plane, flip start and stop */
         ArcCenter2(arcstart.x,arcstart.y,
                    arcstop.x,arcstop.y,
                    arccenter.x,arccenter.y,
                    &radius,&start_angle,&delta_angle,1);
      }
      else
      {
         ArcCenter2(arcstart.x,arcstart.y,
                    arcstop.x,arcstop.y,
                    arccenter.x,arccenter.y,
                    &radius,&start_angle,&delta_angle,0);
      }

      if (fabs(arcstart.x - arcstop.x) < SMALLUNIT &&
          fabs(arcstart.y - arcstop.y) < SMALLUNIT)
      {
         Graph_Circle(cl, arccenter.x, arccenter.y, radius, 0, wi, 0, FALSE);
      }
      else
      {
         Graph_Arc(cl,arccenter.x,arccenter.y, 
               radius, start_angle,delta_angle, 0, wi, FALSE);
      }
   }
   else
   {
      CString  tmp;
      tmp.Format("Error Bad Data format in sub do_lines().\nData=[%s]\n",data_record);
      ErrorMessage(tmp);
      return;
   }

   fseek(fp,cur_pos,SEEK_SET);
}


/****************************************************************************/
/*    do_compositecurve() - Write out Circles and Arcs
*/
void do_compositecurve(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i,j,k;
   long  cur_pos;
   int   num_ent;
   char  *lp;
   long  *data_array;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_compositecurve4\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);

   lp=get_buff_par(2, data_record);
   num_ent=atoi(lp);

   if (num_ent > 0)
   {
      if ((data_array = (long *)calloc(num_ent,sizeof(long))) == NULL)
      {
         ErrorMessage("No memory\n");
         return;
      }

      j=3;
      for (i=0; i<num_ent; i++)
      {
         lp=get_buff_par(j,data_record);
         j++;
         data_array[i] = atol(lp);
      }

      for (i=0;i<num_ent;i++)
      {
         p_ptr=data_array[i];

         if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
         {
            CString  tmp;
            tmp.Format("Error in dir_seek in sub do_sub\n");
            ErrorMessage(tmp);
            return;
         }
         process_entity(fp,D_Start,P_Start);
      }

      free(data_array);
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_associatitive_instance() 
*/
void do_associatitive_instance(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight,
               int  form)
{
   int   i,j,k;
   long  cur_pos;
   int   num_ent;
   char  *lp;
   long  *data_array;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_associatitive_instance\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);

   if (form == 1 || form == 7)
   {
      lp=get_buff_par(2, data_record);
      num_ent=atoi(lp);

      if (num_ent > 0)
      {
         if ((data_array = (long *)calloc(num_ent,sizeof(long))) == NULL)
         {
            ErrorMessage("No memory\n");
            return;
         }
   
         j=3;
         for (i=0; i<num_ent; i++)
         {
            lp=get_buff_par(j,data_record);
            j++;
            data_array[i] = atol(lp);
         }

         for (i=0;i<num_ent;i++)
         {
            p_ptr=data_array[i];

            if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
            {
               CString  tmp;
               tmp.Format("Error in dir_seek in sub do_sub\n");
               ErrorMessage(tmp);
               return;
            }
            process_entity(fp,D_Start,P_Start);
         }
         free(data_array);
      }
   }
   else
   {
      fprintf(flog, " 402 associatitive_instance form [%d] not implemented\n", form);
      display_error++;
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_sub_instance() - Write out SubFigure Instance
*/
/****************************************************************************/
/*  */
void do_sub_instance(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int      i;
   long     cur_pos;
   char     *block_name;
   int      ent;
   double   x1,y1,z1;
   double   scale;
   long     de;
   char     *lp;

   cur_pos=ftell(fp);

   if (( i = par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_sub_instance\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);
   while((lp=strchr(data_record,'D')) != NULL) *lp='E';

   i=sscanf(data_record,"%d,%ld,%lE,%lE,%lE,%lE",&ent,&de,&x1,&y1,&z1,&scale);

   if (i == 6)
   {
      block_name=get_block_name(fp,de,P_Start,D_Start);
      Graph_Block_Reference(block_name, NULL, 0, x1, y1, 0.0, 0, scale, -1, TRUE);
   }
   else
   {
      CString  tmp;
      tmp.Format("Error Bad Data format in sub do_sub_instance().\nData=[%s]\n",data_record);
      ErrorMessage(tmp);
      return;
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_subfigure() - Write out Subfigures
*/
/****************************************************************************/
/*  */
void do_subfigure(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i,j,k;
   long  cur_pos;
   int   num_ent,nc1;
   char  block_name[MAX_LINE];
   char  *lp;
   long  *data_array;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_subfigure\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);

   lp=get_buff_par(4, data_record);
   num_ent=atoi(lp);

/************************************
   Begin a SubFigure Block
*************************************/
   lp=get_buff_par(3, data_record);   /* Get the Block Name */
   nc1 = atoi(lp);    /* make atoi until first error */
   lp=strchr(data_record,'H')+1;
   strncpy(block_name,lp,nc1);
   block_name[nc1] = '\0';

   Graph_Block_On(GBO_APPEND,block_name,-1,0);  // acad allows the reference of a block before defintion

   if (num_ent > 0)
   {
      if ((data_array = (long *)calloc(num_ent,sizeof(long))) == NULL)
      {
         ErrorMessage("No memory\n");
         return;
      }

      j=5;
      for (i=0; i<num_ent; i++)
      {
         lp=get_buff_par(j,data_record);
         j++;
         data_array[i] = atol(lp);
      }
      for (i=0;i<num_ent;i++)
      {
         p_ptr=data_array[i];
         if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
         {
            CString  tmp;
            tmp.Format("Error in dir_seek in sub do_sub\n");
            ErrorMessage(tmp);
            return;
         }
         process_entity(fp,D_Start,P_Start);
      }
      free(data_array);
   }

   Graph_Block_Off();

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_lineardim() - Write out Linear dimension
*/
/****************************************************************************/
/*  */
void do_lindim(   FILE *fp,
          long p_ptr,
          long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i,k;
   long  cur_pos;
   char  block_name[MAX_LINE];
   char  *lp;
   long  data_arr[5];
   Point3 p1;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_lindim\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);

/************************************
   Begin a Linear Dim Block
*************************************/

   sprintf(block_name,"DIM%ld",p_ptr);

   Graph_Block_On(GBO_APPEND,block_name,-1,0);  // acad allows the reference of a block before defintion

   lp = strtok(data_record,";,");
   for (i=0;i<5;i++)
   {
      if (lp = strtok(NULL,";,"))
         data_arr[i]=atol(lp);
      else
      {
         data_arr[i]=-1;
         fprintf(flog,"Invalid IGES file - Data Error in [%s]\n", block_name);
         display_error++;
      }
   }

   for (i=0; i<5; i++) 
   {
      if (data_arr[i] < 0) continue;
      p_ptr=data_arr[i];
      
      if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
      {
         CString  tmp;
         tmp.Format("Error in dir_seek in sub do_sub\n");
         ErrorMessage(tmp);
         return;
      }
      process_entity(fp,D_Start,P_Start);
   }

   Graph_Block_Off();
   /* here place block */
   p1.x = 0;
   p1.y = 0;
   p1.z = 0;

   TransPoint3( &p1, 1, &transform, translate.x, translate.y, translate.z );
   Graph_Block_Reference(block_name, NULL, 0, p1.x, p1.y, 0.0, 0, 1.0, -1, TRUE);

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    do_gelabel() - Write out General Label
*/
/****************************************************************************/
/*  */
void do_genlabel(   FILE *fp,
          long p_ptr,
          long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i,k;
   long  cur_pos;
   int   num_ent;
   char  block_name[MAX_LINE];
   char  *lp;
   long  *data_arr;
   Point3 p1;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_lindim\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);

/************************************
   Begin a General Label Block
*************************************/

   sprintf(block_name,"LAB%ld",p_ptr);

   Graph_Block_On(GBO_APPEND,block_name,-1,0);  // acad allows the reference of a block before defintion

   if ((data_arr = (long *)calloc(100,sizeof(long))) == NULL)
   {
      ErrorMessage("No memory\n");
      return;
   }

   lp = strtok(data_record,";,");
   lp = strtok(NULL,";,");
   data_arr[0]=atol(lp);

   lp = strtok(NULL,";,");
   num_ent = atoi(lp) + 1; /* do first also */

   if (num_ent > 99)
   {
      CString  tmp;
      tmp.Format("Overflow in 210\n");
      ErrorMessage(tmp);
      return;
   }

   for (i=1;i<num_ent;i++)
   {
      lp = strtok(NULL,";,");
      data_arr[i]=atol(lp);
   }

   for (i=0; i<num_ent; i++)
   {
      p_ptr=data_arr[i];
      if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
      {
         CString  tmp;
         tmp.Format("Error in dir_seek in sub do_sub\n");
         ErrorMessage(tmp);
         return;
      }
      process_entity(fp,D_Start,P_Start);
   }

   Graph_Block_Off();
   /* here place block */
   p1.x = 0;
   p1.y = 0;
   p1.z = 0;

   TransPoint3( &p1, 1, &transform, translate.x, translate.y, translate.z );
   Graph_Block_Reference(block_name, NULL, 0, p1.x, p1.y, 0.0, 0, 1.0, -1, TRUE);

   fseek(fp,cur_pos,SEEK_SET);
   free(data_arr);
}

/****************************************************************************/
/*    do_drawing_ent() - Write out Drawing Entities
*/
/****************************************************************************/
/*  */
void do_drawing_ent( FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start,
               long level,
               long line_font,
               int  color,
               int  line_weight)
{
   int   i,k,cnt;
   long  cur_pos,fp_pos;
   char  *lp;
   char  iline[MAX_LINE];
   char  iline2[MAX_LINE];
   int   num_ent,entities_processed;

   cur_pos=ftell(fp);

   if ((i=par_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub do_subfigure\n");
      ErrorMessage(tmp);
      return;
   }

   if ((lp=FGETS(iline,MAX_LINE,fp)) == NULL)
   {
      CString  tmp;
      tmp.Format("Error reading Parameter Data in sub do_subfigure\n");
      ErrorMessage(tmp);
      return;
   }

   lp=get_buff_par(2, iline);
   num_ent=atoi(lp);
   cnt=0;
   i=0;
   while(!feof(fp))
   {
      if(iline[i] == ';')
         return;
      if(iline[i] == ',') cnt++;
      if(cnt == (num_ent * 3 + 2))
         break;
      if(i == rec_len)
      {
         if ((lp=FGETS(iline,rec_len,fp)) == NULL)
         {
            CString  tmp;
            tmp.Format("Error reading Parameter Data in sub do_subfigure\n");
            ErrorMessage(tmp);
            return;
         }
         i=0;
      }
      else
         i++;
   }

   lp=(char *)&iline[i+1];
   num_ent=atoi(lp);
   fp_pos=ftell(fp);
   entities_processed=0;

   if (num_ent > 0)
   {
      while (TRUE)
      {
         entities_processed++;
         if (entities_processed > num_ent)
         {
            if ((lp=strchr(lp,',')) == NULL)
              break;
            lp++;
            num_ent=atoi(lp);
            if (num_ent == 0)
               continue;
            entities_processed=0;
         }
         if ((lp=strchr(lp,',')) == NULL)
           break;
         lp++;
         if (*lp == ' ')
         {
            fseek(fp,fp_pos,SEEK_SET);
            if ((lp=FGETS(iline2,rec_len,fp)) == NULL)
            {
               CString  tmp;
               tmp.Format("Error reading Parameter Data in sub do_sub\n");
               ErrorMessage(tmp);
               return;
            }
            iline2[rec_len] = '\0';
            fp_pos=ftell(fp);
            lp=(char *)&iline2;
         }

         p_ptr=atol(lp);
         if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
         {
            CString  tmp;
            tmp.Format("Error in dir_seek in sub do_sub\n");
            ErrorMessage(tmp);
            return;
         }

         process_entity(fp,D_Start,P_Start);
      }
   }

   fseek(fp,cur_pos,SEEK_SET);
}

/****************************************************************************/
/*    get_block_name() - Returns Pointer to the Block Name Associated
                         with the Directory Pointer Passed
*/
/****************************************************************************/
/*  */
char *get_block_name(   FILE *fp,
               long p_ptr,
               long P_Start,
               long D_Start)
{
   int   i,k;
   long  cur_pos;
   char  *lp;
   char  iline[MAX_LINE];
   static char block_name[MAX_LINE];
   long  s_ptr,line_font,level,view,matrix;
   int   ent_typ,form,nc1;
   int   line_weight,color;
   long  line_count;
   union vb buff;
   union vb buff2;
   char  ses[2];

   cur_pos=ftell(fp);

   if ((k=dir_seek(fp,p_ptr,D_Start)) != NULL)
   {
      CString  tmp;
      tmp.Format("Error in dir_seek in sub get_block_name()\n");
      ErrorMessage(tmp);
      return NULL;
   }

   if ((lp=FGETS(buff.buffer, rec_len,fp)) == NULL)
   {
      CString  tmp;
      tmp.Format("Error in Directory Structure in sub get_block_name\n");
      ErrorMessage(tmp);
      return NULL;
   }
   buff.buffer[rec_len] = '\0';     

   if ((lp=FGETS(buff2.buffer, rec_len,fp)) == NULL)
   {
      CString  tmp;
      tmp.Format("Error in Directory Structure in sub get_block_name\n");
      ErrorMessage(tmp);
      return NULL;
   }
   buff2.buffer[rec_len] = '\0';    

   get_dir1_data(&ent_typ,&s_ptr,&line_font,&view,&level,&matrix,ses,buff);
   get_dir2_data(&line_weight,&color,&line_count,&form,buff2);

   if ((i=par_seek(fp,s_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in par_seek in sub get_block_name()\n");
      ErrorMessage(tmp);
      return NULL;
   }

   if ((lp=FGETS(iline, rec_len,fp)) == NULL)
   {
      CString  tmp;
      tmp.Format("Error reading Parameter Data in sub get_block_name()\n");
      ErrorMessage(tmp);
      return NULL;
   }
   iline[rec_len] = '\0';     

   lp=get_buff_par(3, iline);   /* Get the Block Name */
   nc1 = atoi(lp);    /* make atoi until first error */
   lp=strchr(iline,'H')+1;
   strncpy(block_name,lp,nc1);
   block_name[nc1] = '\0';
   fseek(fp,cur_pos,SEEK_SET);

   return(block_name);
}

/****************************************************************************/
/*    par_seek() - Seeks to the Parameter line specified by p_ptr
      Returns zero on success.
*/
/****************************************************************************/
int   par_seek(FILE *fp, long p_ptr, long P_Start)
{
   int   i;

   if ((i=fseek(fp,(P_Start + ((p_ptr - 1) * rec_len)),SEEK_SET)) != 0)
      return -1;

   return 0;
}

/****************************************************************************/
/*  
*/
int   mat_seek(FILE *fp, long p_ptr, long P_Start)
{
   char  tmp[MAX_LINE];
   long  last_pos,cur_ptr;
   char  tmp1[MAX_LINE];

   /* set to start */
   fseek(fp,P_Start,SEEK_SET);
   last_pos = ftell(fp);
   while (FGETS(tmp,rec_len,fp) != NULL)
   {
      strcpy(tmp1,&(tmp[66]));
      tmp1[72-66] = '\0';
      cur_ptr = atol(tmp1);
      if (cur_ptr == p_ptr)
      {
         fseek(fp,last_pos,SEEK_SET);
         return 0;
      }
      last_pos = ftell(fp);
   }
   return 1;
}

/****************************************************************************/
/*    dir_seek() - Seeks to the Directory line specified by p_ptr
      Returns zero on success and -1 on failure.
*/
int   dir_seek(FILE *fp, long p_ptr, long D_Start)
{
   int i;

   if ((i=fseek(fp,(D_Start + ((p_ptr - 1) * rec_len)),SEEK_SET)) != 0)
      return(-1);
   return(0);
}

/****************************************************************************/
/*    get_dir1_data() - Gets the data from the first line of
                        the directory entry
*/
/****************************************************************************/
/*  */
void get_dir1_data(int *ent_typ,
                   long *p_ptr,
                   long *line_font,
                   long *view,
                   long *level,
                   long *matrix,
                   char ses[2],
                   union vb buff)
{
   char  tmp[20];

   *ent_typ=atoi(buff.d.ent);

   *p_ptr=atol(buff.d.par);

   strncpy(ses,buff.d.ses,2); // in status field, subordinate entity switch

   strncpy(tmp,buff.d.font,8);
   tmp[sizeof(buff.d.font)]='\0';
   *line_font=atol(tmp);

   strncpy(tmp,buff.d.view,8);
   tmp[sizeof(buff.d.view)]='\0';
   *view=atol(tmp);

   strncpy(tmp,buff.d.level,8);
   tmp[sizeof(buff.d.level)]='\0';
   *level=atol(tmp);

   strncpy(tmp,buff.d.matrix,8);
   tmp[sizeof(buff.d.matrix)]='\0';
   *matrix=atol(tmp);
}

/****************************************************************************/
/*    get_dir2_data() - Gets the data from the second line of
                        the directory entry
*/
/****************************************************************************/
/*  */
void get_dir2_data(int *line_weight,
                   int *color,
                   long *line_count,
                   int *form,
                   union vb buff)
{
   char  tmp[20];

   strncpy(tmp,buff.d2.line_weight,8);
   tmp[sizeof(buff.d2.line_weight)]='\0';
   *line_weight=atoi(tmp);

   strncpy(tmp,buff.d2.color,8);
   tmp[sizeof(buff.d2.color)]='\0';
   *color=atoi(tmp);

   strncpy(tmp,buff.d2.line_count,8);
   tmp[sizeof(buff.d2.line_count)]='\0';
   *line_count=atol(tmp);

   strncpy(tmp,buff.d2.form_num,8);
   tmp[sizeof(buff.d2.form_num)]='\0';
   *form=atoi(tmp);
}

/****************************************************************************/
/*    do_global() - Gets the data from the Global Section
*/
/****************************************************************************/
/*  */
void do_global(FILE *fp,char *iline, int number_of_global)
{
   int   i;
   char  *lp;
   char  *ll;
   char  *lj;
   int   g_cnt = 0;

   //printf("Processing Global Data Section:\n");

   Global_Buff[0]='\0';

   while (TRUE)
   {
      g_cnt++;

      if ((i=(strlen(Global_Buff) + strlen(iline))) > (MAX_BUFF - 1))
      {
         CString  tmp;
         tmp.Format("Error - Too much Global Data for Buffer.\n");
         ErrorMessage(tmp);
         return;
      }
   
      // copy only 0..71 position, not the 73..80
      char  tmp[MAX_LINE];
      strncpy(tmp,iline,72);
      tmp[72] = '\0';
      strcat(Global_Buff,tmp);

      if (g_cnt == number_of_global)  // ??
      {
        break; /* End of Global Data */
      }
      
      // rec_len+1, because fgets gets n-1 chars, 1 needed for \0
      if (FGETS(iline,rec_len,fp) == NULL)
      {
         CString  tmp;
         tmp.Format("Error reading Parameter Data in sub do_global\n");
         ErrorMessage(tmp);
         return;
      }
      iline[rec_len] = '\0';
      i = strlen(iline);
   }

   i=strlen(Global_Buff);
   if ((Global_Buff[0] != ',') && (Global_Buff[1] != ','))
   {
      if ((Global_Buff[2] != ',') && (Global_Buff[6] != ';'))
      {
         CString  tmp;
         tmp.Format("Non-Standard delimiters used. Program Terminated.\n");
         ErrorMessage(tmp);
         return;
      }
   }

   lp=get_par(14);   /* Units of Measure */
   Unit_Flag = atoi(lp);
   lp=get_par(16);   /* Max Number of Line Weight Gradations */
   Max_Grad = atoi(lp);
   lp=get_par(17);   /* Max Line Width in Units */
   ll=strchr(lp,',');
   if (ll != NULL)
      for (lj=lp; lj <= ll; lj++)
      {
        if (*lj == 'D') *lj='E';
      }
   Max_Width=atof(lp);

}

/****************************************************************************/
/*    get_par() - Returns the pointer to the parameter number requested
                  from the Global Section.
*/
/****************************************************************************/
/*  */
char *get_par(int par_num)
{
   char  *lp;
   int   i;

   lp = Global_Buff;
   for(i=0;i<(par_num - 1);i++)
   {
      lp=strchr(lp,',');
      lp++;
   }
   return(lp);
}

/****************************************************************************/
/*    get_buff_par() - Returns the pointer to the parameter number requested
                       from the buffer passed.
*/
/****************************************************************************/
/*  */
char *get_buff_par(int par_num,char *buff)
{
   char  *lp;
   int   i;

   lp=buff;
   for(i=0;i<(par_num - 1);i++)
   {
      lp=strchr(lp,',');
      lp++;
   }
   return(lp);
}

/****************************************************************************/
/*
*/
static int get_record(FILE *fp)
{
   char  iline[MAX_LINE];
   int   end = FALSE;
   int   totlength = 0;
   char  *lp;

   data_record[0] = '\0';
   while (!end)
   {
      if ((lp=FGETS(iline, rec_len,fp)) == NULL)
      {
         CString  tmp;
         tmp.Format("Error reading Parameter Data in sub do_circle\n");
         ErrorMessage(tmp);
         return -1;
      }
      /* an igesline is max 64 char long */
      iline[64] = '\0';
      /* now cut down on whitespace */
      while (isspace(iline[strlen(iline)-1]))
            iline[strlen(iline)-1] = '\0';
      if (iline[strlen(iline)-1] == ';')  end = TRUE;
      totlength += strlen(iline);
      if (totlength < MAX_DATA_RECORD)
      {
         strcat(data_record,iline);
      }
      else
      {
         CString  tmp;
         tmp.Format("Data record too long\n");
         ErrorMessage(tmp);
         return -1;
      }
   }
   return 0;
}

/****************************************************************************/
/*
*/
void do_transmatrix( FILE *fp,
                  long p_ptr,
                  long P_Start)
{
   int   i;
   long  cur_pos;
   double ttemp[3];
   int row,col;
   char  *lp;

   if (p_ptr == 0)
   {
      transform.m3[0][0] = 1;
      transform.m3[0][1] = transform.m3[0][2] = 0;
      transform.m3[1][1] = 1;
      transform.m3[1][0] = transform.m3[1][2] = 0;
      transform.m3[2][2] = 1;
      transform.m3[2][0] = transform.m3[2][1] = 0;
      translate.x = 0; translate.y = 0; translate.z = 0;
      return;
   }

   cur_pos=ftell(fp);

   if (( i = mat_seek(fp,p_ptr,P_Start)) != 0)
   {
      CString  tmp;
      tmp.Format("Error in mat_seek in sub do_transmatrix\n");
      ErrorMessage(tmp);
      return;
   }

   get_record(fp);
   CString  w;
   w = data_record;

   while((lp=strchr(data_record,'D')) != NULL) *lp='E';

   /* parameter data starts with second parameter.  first is always entity */
   /*    type code. */
   lp = strtok(data_record,",;");
   /* read data for all matrix rows R1-,R2-,R3- (also vector data T1,T2,T3) */

   int err = FALSE;

   for( row=0; row<3; row++ )
   {
      /* read data for all columns per row  R-1,R-2,R-3 */
      for( col=0; col<3; col++ )
      {
         if ((lp = strtok(NULL,",;")) == NULL)
            err = TRUE;
         else
            transform.m3[row][col] = atof(lp);
      }
      if ((lp = strtok(NULL,",;")) == NULL)  
         err = TRUE;
      else
         ttemp[row] = atof(lp);
   }

   /* assign translation vector in temp array to 3D point structure */
   if (err == FALSE)
   {
      translate.x = ttemp[0];
      translate.y = ttemp[1];
      translate.z = ttemp[2];
   }
   else
   {
      // reset to init.
      transform.m3[0][0] = 1;
      transform.m3[0][1] = transform.m3[0][2] = 0;
      transform.m3[1][1] = 1;
      transform.m3[1][0] = transform.m3[1][2] = 0;
      transform.m3[2][2] = 1;
      transform.m3[2][0] = transform.m3[2][1] = 0;
      translate.x = 0; translate.y = 0; translate.z = 0;
      fprintf(flog, "Matrix error at p_ptr %ld P_Start %ld\n", p_ptr, P_Start);
      display_error++;
   }

   fseek(fp,cur_pos,SEEK_SET);

   return;
}

// end IGESIN.C

