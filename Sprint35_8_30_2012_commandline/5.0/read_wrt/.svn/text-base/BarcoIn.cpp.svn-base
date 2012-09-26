// $Header: /CAMCAD/4.6/read_wrt/BarcoIn.cpp 18    4/05/07 3:12p Rick Faltersack $

/*****************************************************************************/
/*  
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   patterns are not evaluated.
   box corners are not evaluated

  needs to be done
  - Vector text
  - text mirror, Block mirror M=X, M=Y command
  - linewidth-aperture for _seg, _arc, _circle

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
#include "logwrite.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
extern CProgressDlg *progress;

/* Defines *****************************************************************/

#define  MAX_LINE          1024     /* Max line length. This includes macros */
#define  MAX_WIDTH         1000
#define  MAX_POLY          10000
#define  MAX_POINTLIST     1000
#define  MAX_APPNR         10000

#define  BARCOERR          "barco.log"

#define  XMIRROR           1
#define  YMIRROR           2

/* Structures **************************************************************/
typedef struct
{
   double   x,y;
}Pointlist;

typedef struct
{
   int   barco;      // Barco can have same aperture numbers for different defintions
   int   unique;     // this here remaps to a unique number
}Appnr;

/* Globals *****************************************************************/

static   FILE        *ferr;
static   FILE        *fp;
static   double      cnv_unit(double);
static   int         do_barco(int filled);
static   void        write_poly(int neg, int fill);
static   int         load_existing_apertures();

static   Point2      *ppoly;
static   int         ppolycnt;

static   Appnr       *appnr;
static   int         appnrcnt;
static   int         tot_appnr;

static   int         pageUnits; 
static   double      unitscale;
static   double      unitpermil = 1;
static   Pointlist   pointlist[MAX_POINTLIST];

static   long        linecnt;

static   int         cur_layernum;
static   int         cur_widthindex;
static   char        last_char;
static   double      default_char_height;
static   double      default_char_width;
static   int         cur_filenum;

static   int         display_error;

static   int         FLOATLAYER;

static   CCEtoODBDoc  *doc;
static   double      cur_x,cur_y;

/****************************************************************************/
/*
*/
static char FGETC(FILE *fp)
{
   char  c;
   
   if ((c = fgetc(fp)) == EOF)   return EOF;
   if (c == '\n') 
      linecnt++;

   return c;
}

/****************************************************************************/
/*
*/
static char UNGETC(char c, FILE *fp)
{
   int   r;
   if (c == '\n') 
      linecnt--;
   if ((r = ungetc(c, fp)) == EOF)  return EOF;

   return r;
}

/******************************************************************************
* ReadBarco
*/
void ReadBarco(const char *fname, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{                 
   FileStruct *file;
   doc = Doc;
   pageUnits = PageUnits;

   display_error = 0;
   // default is MILS
   unitscale = Units_Factor(UNIT_MILS, pageUnits);
   default_char_height = 100 * unitscale;
   default_char_width = default_char_height * 0.8421;

   CString barcoLogFile = GetLogfilePath(BARCOERR);
   if ((ferr = fopen(barcoLogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file",barcoLogFile);
      ErrorMessage(t, "Error");
      return;
   }

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", fname);
      ErrorMessage(t, "Error");
      return;
   }
   log_set_filename(fname);
   linecnt = 1;

   if ((ppoly = (Point2 *) calloc(MAX_POLY,sizeof(Point2))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   ppolycnt = 0;

   if ((appnr = (Appnr *) calloc(MAX_APPNR,sizeof(Appnr))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   appnrcnt = 0;

   tot_appnr = 0;

   file = Graph_File_Start(fname, Type_Barco);
   cur_filenum = file->getFileNumber();
   char filename[OFS_MAXPATHNAME];
   // shorten layer name
   char fl[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(fname, NULL, NULL, fl, ext);
   strcpy(filename, fl);
   strcat(filename, ext);
   // strcpy(filename, fname);         

   // just in case
   FLOATLAYER = Graph_Level("0","",1);
   cur_layernum = Graph_Level(filename, "",0); // makes every gerber file a different color

   load_existing_apertures();

   cur_widthindex = 0;
   do_barco(0);

   fclose(fp);

   free((char *)ppoly);
   free((char *)appnr);

   fclose(ferr);

   if (display_error)
      Logreader(barcoLogFile);
}

/****************************************************************************
* 
   set_unique
   find barco number, up the tot_appnr and insert and return
*/
static int set_unique(int b)
{
   int   i;

   // first try to find it
   for (i=0;i<appnrcnt;i++)
   {
      if (appnr[i].barco == b)
      {
         appnr[i].unique = ++tot_appnr;
         return appnr[i].unique;
      }
   }

   if (appnrcnt < MAX_APPNR)
   {
      appnr[appnrcnt].barco  = b;
      appnr[appnrcnt].unique = ++tot_appnr;
      appnrcnt++;
   }
   else
   {
      ErrorMessage("Too many appnr","appnr");
   }
   return tot_appnr;
}

/****************************************************************************
* 
   set_unique
   find barco number, up the tot_appnr and insert and return
*/
static int load_existing_apertures()
{
   int   i;
   int   app;
   
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      // all new barco apertures are called A%d
      int res = sscanf(block->getName(),"A%d",&app);
      if (res != 1)     continue;   // not A%d in name
      if (app > tot_appnr) tot_appnr = app;
   }

   return tot_appnr;
}

/****************************************************************************
* 
   get_unique
   find barco number and return unique
*/
static int  get_unique(int b)
{
   int   i;

   for (i=0;i<appnrcnt;i++)
   {
      if (appnr[i].barco == b)
         return(appnr[i].unique);
   }

   log_undefinded_aperturenumber(ferr, b, linecnt);

   fprintf(ferr,"Aperture [%d] selected but not defined at %ld\n",b,linecnt);
   display_error++;

   return tot_appnr++;
}

/****************************************************************************
* 
   needs rotation in degree
*/
static int  make_complexaperture(const char *name, int typ,
                     double x, double y, double rot, int negative)
{
   //char   macroname[80];
   double   x1,y1,x2,y2,x3,y3,x4,y4;   
   double   xx,yy;

   //sprintf(macroname,"CPLX_%s",name);
   Graph_Block_On(GBO_APPEND, name, cur_filenum, BL_GLOBAL);   // generate sub block

   // here do graphic
   switch (typ)
   {
      case T_SQUARE:
         x1 = -x/2;
         y1 = -x/2;
         x2 =  x/2;
         y2 = -x/2;
         x3 =  x/2;
         y3 =  x/2;
         x4 = -x/2;
         y4 =  x/2;
         // rotate
         Rotate ( x1, y1, rot, &xx, &yy); 
         x1 = xx;
         y1 = yy;
         Rotate ( x2, y2, rot, &xx, &yy); 
         x2 = xx;
         y2 = yy;
         Rotate ( x3, y3, rot, &xx, &yy); 
         x3 = xx;
         y3 = yy;
         Rotate ( x4, y4, rot, &xx, &yy); 
         x4 = xx;
         y4 = yy;
         // draw
         Graph_PolyStruct(FLOATLAYER,  0, negative);
         Graph_Poly(NULL, 0, TRUE,0,1);
         Graph_Vertex(x1,y1,0.0);
         Graph_Vertex(x2,y2,0.0);
         Graph_Vertex(x3,y3,0.0);
         Graph_Vertex(x4,y4,0.0);
         Graph_Vertex(x1,y1,0.0);
      break;
      case T_RECTANGLE:
         x1 = -x/2;
         y1 = -y/2;
         x2 =  x/2;
         y2 = -y/2;
         x3 =  x/2;
         y3 =  y/2;
         x4 = -x/2;
         y4 =  y/2;
         // rotate
         Rotate ( x1, y1, rot, &xx, &yy); 
         x1 = xx;
         y1 = yy;
         Rotate ( x2, y2, rot, &xx, &yy); 
         x2 = xx;
         y2 = yy;
         Rotate ( x3, y3, rot, &xx, &yy); 
         x3 = xx;
         y3 = yy;
         Rotate ( x4, y4, rot, &xx, &yy); 
         x4 = xx;
         y4 = yy;
         // draw
         Graph_PolyStruct(FLOATLAYER, 0, 0);
         Graph_Poly(NULL, 0, TRUE, negative, 1);
         Graph_Vertex(x1,y1,0.0);
         Graph_Vertex(x2,y2,0.0);
         Graph_Vertex(x3,y3,0.0);
         Graph_Vertex(x4,y4,0.0);
         Graph_Vertex(x1,y1,0.0);
      break;
      default:
         // i do not know
      break;
   }

   Graph_Block_Off();


   return TRUE;
}

/****************************************************************************
* 
*/                      
static int get_real(FILE *fp, double *x)
{

   int   start = FALSE;
   char  t[80];
   char  c;

   t[0] = '\0';

   while ((c = FGETC(fp)) != EOF)
   {
      last_char = c;
      if (c == '-' || c == '+' || c == '.' || isdigit(c))
      {
         start = TRUE;
         if (strlen(t) < 80)
         {  
            char  tt[10];
            sprintf(tt,"%c",c);
            strcat(t,tt);           
         }  
         else
         {
            // illegal number
            if (c != ',')
               UNGETC(c,fp);
            return FALSE;
         }
      }
      else
      {
         if (start && strlen(t))
         {
            *x = atof(t);
            if (c != ',')
               UNGETC(c,fp);
            return TRUE;
         }
         else
         {
            // no legal number found
            if (c != ',')
               UNGETC(c,fp);
            return FALSE;
         }
      }

   }
   return FALSE;
}

/****************************************************************************
* 
*/
static int get_int(FILE *fp, int *x)
{
   int   start = FALSE;
   char  t[80];
   char  c;

   t[0] = '\0';

   while ((c = FGETC(fp)) != EOF)
   {
      last_char = c;
      if (c == '-' || c == '+' || isdigit(c))
      {
         start = TRUE;
         if (strlen(t) < 80)
         {  
            char  tt[10];
            sprintf(tt,"%c",c);
            strcat(t,tt);           
         }  
         else
         {
            // illegal number
            if (c != ',')
               UNGETC(c,fp);
            return FALSE;
         }
      }
      else
      {
         if (start && strlen(t))
         {
            *x = atoi(t);
            if (c != ',')
               UNGETC(c,fp);
            return TRUE;
         }
         else
         {
            // no legal number found
            if (c != ',')
               UNGETC(c,fp);
            return FALSE;
         }
      }

   }
   return FALSE;
}

/****************************************************************************
* 
*/
static int get_name(FILE *fp, char *n, int maxlen)
{
   int   start = FALSE;
   char  c;

   n[0] = '\0';

   while ((c = FGETC(fp)) != EOF)
   {
      if (isalnum(c))
      {
         if ((int)strlen(n) < maxlen)
         {  
            char  tt[10];
            sprintf(tt,"%c",c);
            strcat(n,tt);           
            start = TRUE;
         }  
         else
         {
            // illegal length
            return FALSE;
         }
      }
      else
      {
         if (start && strlen(n))
         {
            last_char = c;
            if (c != ',')
               UNGETC(c,fp);
            return TRUE;
         }
         else
         {
            // no legal number found
            return FALSE;
         }
      }

   }
   return FALSE;
}

/****************************************************************************
* 
   width height is only , height : width
*/
static int get_charheightwidth(FILE *fp, double *h, double *w)
{
   char  c;
   double x;

   *h = default_char_height;
   *w = default_char_width;

   while (TRUE)
   {
      if ((c = FGETC(fp)) == EOF)
         return FALSE;
      if (!isspace(c))  break;
      last_char = c;
   }

   // no options listed.
   if (c != ',')  
   {
      UNGETC(c,fp);
      last_char = c;
      return FALSE;
   }

   if (get_real(fp,&x)) // get height
   {
      *h = cnv_unit(x);
      *w = *h * 0.8421;
   }

   while (TRUE)
   {
      last_char = c;
      if ((c = FGETC(fp)) == EOF)
         return FALSE;
      if (!isspace(c))  break;
   }

   if (c == ':')
   {
      if (get_real(fp,&x)) // get width
         *w = cnv_unit(x);
   }
   else
   {
      UNGETC(c,fp);
   }
   return TRUE;
}

/****************************************************************************
* 
*/
static int get_prosa(FILE *fp, char *n, int maxlen)
{
   int   start = 0;
   char  c;

   n[0] = '\0';

   while ((c = FGETC(fp)) != EOF)   
   {
      if (c == '"')  // text 
      {
         c = FGETC(fp);
         if (c != '"')  // " is part of text  (" test "" quotes ")
            start++;
      }

      if (start == 0)
      {
         // text not started yet.
      }
      else
      if (start == 1)
      {
         if ((int)strlen(n) < maxlen)
         {
            char  t[10];
            // need to do special treatment if text contains "

            sprintf(t,"%c",c);
            strcat(n,t);
         }
         else
         {
            // text too long
         }
      }
      else
      if (start == 2)
      {
         // here is end of text;
         if (n[strlen(n)-1] == ')')
            n[strlen(n)-1] = '\0';
         if (n[strlen(n)-1] == '"')
            n[strlen(n)-1] = '\0';

         while (c != ')')
         {
            // go to end of bracket
            if ((c = FGETC(fp)) == EOF)   return FALSE;
         }

         return TRUE;
      }

   }

   return FALSE;
}

/****************************************************************************
* 
*/
static int get_attribval(FILE *fp, char *n, int maxlen)
{
   int   start = 0;
   char  c;

   n[0] = '\0';

   while ((c = FGETC(fp)) != EOF)   
   {
      if (c == '"')  // text 
      {
         c = FGETC(fp);
         if (c != '"')  // " is part of text  (" test "" quotes ")
            start++;
      }

      if (start == 0)
      {
         // text not started yet.
      }
      else
      if (start == 1)
      {
         if ((int)strlen(n) < maxlen)
         {
            char  t[10];
            // need to do special treatment if text contains "

            sprintf(t,"%c",c);
            strcat(n,t);
         }
         else
         {
            // text too long
         }
      }
      else
      if (start == 2)
      {
         last_char = c;
         // here is end of text;
         if (n[strlen(n)-1] == ')')
            n[strlen(n)-1] = '\0';
         if (n[strlen(n)-1] == '"')
            n[strlen(n)-1] = '\0';
         return TRUE;
      }

   }

   return FALSE;
}

/****************************************************************************
* 
*/
static int get_appoptions(FILE *fp,BlockStruct *b, 
                          double *rot, int *neg, int *mir, double *scale, char *optionstring)
{
   // if there is a komma, options will follow
   char     c;
   CString  opstring;

   opstring = "";
   if (optionstring != NULL)  strcpy(optionstring, opstring);
   *rot = 0;
   *neg = 0;
   *mir = 0;
   *scale = 1;

   if (isspace(last_char)) // test if there is a newline
   {
      while ((last_char = FGETC(fp)) != EOF)
      {
         if (isspace(last_char)) continue;
         if (last_char == ',')
         {
            break;
         }
         else
         {
            UNGETC(last_char,fp);
            return FALSE;
         }
      }
   }

   if (last_char != ',')
   {
      return FALSE;
   }

   while ((c = FGETC(fp)) != EOF)
   {
      int cont = FALSE;
      if (isspace(c) || (c != 'M' && c != 'R' && c != 'A' && c != 'S' && c != 'P' && c != 'N' /* 'RE' */))
      {
         if (!isspace(c))
            UNGETC(c,fp);
         return TRUE;
      }
      if (isalpha(c))
      {
         // P Pattern
         switch (c)
         {
            case 'P':
            {
               // advance until space or komma
               CString  pattern;
               pattern = 'P';
               while ((c = FGETC(fp)) != EOF)
               {
                  if (c == ',')
                  {
                     cont = TRUE;
                     break;
                  }
                  else
                  if (isspace(c))
                  {
                     UNGETC(c,fp);
                     break;
                  }
                  pattern += c;
               }
               fprintf(ferr,"Pattern [%s] at %ld not supported.\n", pattern, linecnt);
               display_error++;
            }
            break;
            case 'N':   // name ="ccc"
            {
               while ((c = FGETC(fp)) != EOF)
               {
                  if (c == '=')
                     break;
               }
               if (c == EOF)  return FALSE;

               char  prosa[255];
               get_attribval(fp,prosa,255);

               if (b)
               {
                  doc->SetUnknownAttrib(&b->getAttributesRef(),ATT_NAME, prosa, SA_APPEND, NULL); // x, y, rot, height
               }
               UNGETC(last_char,fp);

               while ((c = FGETC(fp)) != EOF)
               {
                  if (c == ',')
                  {
                     cont = TRUE;
                     break;
                  }
                  if (isspace(c))
                  {
                     UNGETC(c,fp);
                     break;
                  }
               }
            }
            break;
            case 'A':   // attribute A*TTRIBUTES=
            {
               int   attribute = 0; // 0 = first, 1 next 2 end
               while ((c = FGETC(fp)) != EOF)
               {
                  if (c == '=')
                     break;
               }
               if (c == EOF)  return FALSE;

               // ( name = "value" [,name = "value]
               while (c != '(')
               {
                  if ((c = FGETC(fp)) == EOF)
                     return FALSE;
               }

               // advance bracket.
               if ((c = FGETC(fp)) == EOF)
                     return FALSE;

               while (attribute < 2)
               {
                  CString  key, val;
                  key = "";
                  while (c != '=')
                  {
                     key += c;
                     if ((c = FGETC(fp)) == EOF)
                        return FALSE;
                  }
                  char  prosa[255];
                  get_attribval(fp,prosa,255);
                  val = prosa;
                  
                  if (last_char == ')')   
                     attribute = 2;
                  else
                  if (last_char == ',')   
                     attribute = 1;
                  else
                  {
                     // here need to test is there is a netx , for the next attribute
                     while ((c = FGETC(fp)) != EOF)
                     {
                        if (c == ')')
                        {
                           attribute = 2; // end of attribute
                           break;
                        }
                        else
                        if (c == ',')
                        {
                           attribute = 1; // one more to come.
                           break;
                        }
                     }
                  }
                  if (b)
                  {
                     doc->SetUnknownAttrib(&b->getAttributesRef(),key, val, SA_APPEND, NULL); // x, y, rot, height
                  }
               }
            }
            break;
            case 'R':   
               // R Rotation
               // RE negative
               if ((c = FGETC(fp)) == EOF)
                  return FALSE;
               if (c == '=')
               {
                  // rotation
                  double   r;
                  if (get_real(fp,&r))
                  {
                     while (r < 0)  r += 360;
                     while (r >= 360)  r -= 360;
                     *rot = r;
                  }
                  if (last_char == ',')
                  {
                     cont = TRUE;
                     break;
                  }
                  if (isspace(last_char))
                  {
                     UNGETC(last_char, fp);
                     break;
                  }
               }
               else
               if (c == 'E')
               {
                  // Negative - the full word can be REVERSE
                  // advance until space or komma
                  *neg = TRUE;
                  while ((c = FGETC(fp)) != EOF)
                  {
                     if (c == ',')
                     {
                        cont = TRUE;
                        break;
                     }
                     if (isspace(c))
                     {
                        UNGETC(c,fp);
                        break;
                     }
                  }
               }
               else
               {
                  // unknown R command
                  // advance until space or komma
                  opstring += 'R';  // started as R
                  opstring += c;
                  while ((c = FGETC(fp)) != EOF)
                  {
                     if (c == ',')
                     {
                        cont = TRUE;
                        break;
                     }
                     if (isspace(c))
                     {
                        UNGETC(c,fp);
                        break;
                     }
                     opstring += c;
                  }
               }
            break;
            case 'M':   
               // M mirror
               if ((c = FGETC(fp)) == EOF)
                  return FALSE;
               if (c == '=')
               {
                  while ((c = FGETC(fp)) != EOF)
                  {
                     if (c == 'X')  *mir |= XMIRROR;
                     if (c == 'Y')  *mir |= YMIRROR;
                     
                     if (c == ',')
                     {
                        cont = TRUE;
                        break;
                     }
                     if (isspace(c))
                     {
                        UNGETC(c,fp);
                        break;
                     }
                  }
               }
               else
               {
                  // unknown M command
                  // advance until space or komma
                  while ((c = FGETC(fp)) != EOF)
                  {
                     if (c == ',')
                     {
                        cont = TRUE;
                        break;
                     }
                     if (isspace(c))
                     {
                        UNGETC(c,fp);
                        break;
                     }
                  }
               }
            break;
            case 'S':
               // S Scale
               // advance until space or komma
               if ((c = FGETC(fp)) == EOF)
                  return FALSE;
               if (c == '=')
               {
                  // rotation
                  double   s;
                  if (get_real(fp,&s))
                  {
                     *scale = s;
                  }
               }
               else
               {
                  opstring += 'S';  // started as S
                  opstring += c;
                  while ((c = FGETC(fp)) != EOF)
                  {
                     if (c == ',')
                     {
                        cont = TRUE;
                        break;
                     }
                     if (isspace(c))
                     {
                        UNGETC(c,fp);
                        break;
                     }
                     opstring += c;
                  }
               }
            break;
            default:
               while ((c = FGETC(fp)) != EOF)
               {
                  if (c == ',')
                  {
                     cont = TRUE;
                     break;
                  }
                  if (isspace(c))
                  {
                     UNGETC(c,fp);
                     break;
                  }
               }
            break;
         }  // switch c
      }
      if (!cont)  
         break;
   }

   if (optionstring != NULL)  strcpy(optionstring, opstring);
   return TRUE;
}

/****************************************************************************
* 
*/
static int get_unit(FILE *fp)
{
   char  c;
   char  t[80];
   int   assign = FALSE;

   t[0] = '\0';
   // get to =
   while ((c = FGETC(fp)) != EOF)
   {
      if (isspace(c))   
      {  
         if (assign)
         {
            if (!STRCMPI(t,"MM"))
            {
               unitpermil = 40;
               unitscale = Units_Factor(UNIT_MM, pageUnits);
               return TRUE;
            }
            if (!STRCMPI(t,"INCH"))
            {
               unitpermil = 1000;
               unitscale = Units_Factor(UNIT_INCHES, pageUnits);
               return TRUE;
            }
            if (!STRCMPI(t,"MIL"))
            {
               unitpermil = 1;
               unitscale = Units_Factor(UNIT_MILS, pageUnits);
               return TRUE;
            }
            return FALSE;           // unknown UNIT
         }
         continue;
      }
      if (assign && isalpha(c))
      {
         if (strlen(t) < 80)
         {
            char  tt[10];
            sprintf(tt,"%c",c);
            strcat(t,tt);
         }
         else
         {
            // unit token tool long
            return FALSE;
         }

      }

      if (c == '=')
      {
         assign = TRUE; 
      }
   }
   return FALSE;
}

/****************************************************************************
* 
   get a point 0 .. 99 and remember x,y
*/
static int get_point(FILE *fp)
{
   char  c;
   char  t[80];
   int   pnr;
   double   x,y,xp,yp;

   t[0] = '\0';
   // get to =
   while ((c = FGETC(fp)) != EOF)
   {
      if (isdigit(c))
      {
      }
      else
      if (c == '=')
      {
         pnr = atoi(t);       
         if (get_real(fp,&x))
         {
            xp = cnv_unit(x);
         }
         if (get_real(fp,&y))
         {
            yp = cnv_unit(y);
         }
         // here update point list
         if (pnr > -1 && pnr < MAX_POINTLIST)
         {
            pointlist[pnr].x = xp;
            pointlist[pnr].y = yp;
            return TRUE;
         }
         // points not stored in Pointlist
         return FALSE;
      }
   }
   return FALSE;
}

/*****************************************************************************/
/* 
*/
static int do_move(int negative, int fill)
{
   double x,y;

   write_poly(negative, fill);
   if (get_real(fp,&x))
      cur_x = cnv_unit(x);
   if (get_real(fp,&y))
      cur_y = cnv_unit(y);
   ppoly[0].x = cur_x;
   ppoly[0].y = cur_y;
   ppoly[0].bulge = 0;
   ppolycnt = 1;
   
   return 1;
}

/*****************************************************************************/
/* 
*/
static int do_draw(int negative, int fill)
{
   double x,y;

   if (ppolycnt >= MAX_POLY-1)
      write_poly(negative, fill);
   if (get_real(fp,&x))
      cur_x = cnv_unit(x);
   if (get_real(fp,&y))
      cur_y = cnv_unit(y);
            
   if (ppolycnt && cur_x == ppoly[ppolycnt-1].x && cur_y == ppoly[ppolycnt-1].y)
   {
      // do not do double.
   }
   else
   {
      ppoly[ppolycnt].x = cur_x;
      ppoly[ppolycnt].y = cur_y;
      ppoly[ppolycnt].bulge = 0.0;
      ppolycnt++;
   }
   return 1;
}

/*****************************************************************************/
/* 
*/
static int do_circle()
{
   double   x,y,mx,my;
   double   rad, sa, da;
   CString  tmp;
   char     c;
            
   get_real(fp,&x);
   x = cnv_unit(x);
   get_real(fp,&y);
   y = cnv_unit(y);
   get_real(fp,&mx);
   mx = cnv_unit(mx);
   get_real(fp,&my);
   my = cnv_unit(my);

   tmp = "";
   if (last_char == ',')   // if a CW or CCW follow, there must have been a comma.
   {
      while ((c = FGETC(fp)) != EOF)
      {
         if (isspace(c))
            break;
         else
         {
            tmp += c;
         }
      }
   }
   // start cur_x, cur_y
   // end x,y
   // center mx, my
   // cw = clock, else countercloc
      
   // ArcCenter2 is always counter clock from old to cur
   int   cw = FALSE;
   if (tmp.CompareNoCase("CW") == 0)
      cw = TRUE;
   ArcCenter2(cur_x, cur_y, x, y,mx, my, &rad, &sa, &da, cw);

   //Graph_Arc(cur_layernum,mx, my,rad, sa, da, 0L,0, cur_negative);
   if (ppolycnt)
      ppoly[ppolycnt-1].bulge = tan(da/4);
   cur_x = x;
   cur_y = y;
   ppoly[ppolycnt].x = cur_x;
   ppoly[ppolycnt].y = cur_y;
   ppoly[ppolycnt].bulge = 0.0;
   ppolycnt++;

   return 1;
}

/*****************************************************************************/
/* 
*/
static int do_vectortext()
{
   double   x,y;
   CString  tmp;
   char     c;
            
   get_real(fp,&x);
   x = cnv_unit(x);
   get_real(fp,&y);
   y = cnv_unit(y);

   char  prosa[255];
   get_prosa(fp,prosa,255);

   double   h, w;
   get_charheightwidth(fp,&h,&w);

   // get past fontname
   while ((c = FGETC(fp)) != EOF)
   {
      if (c == ',')
      {
         break;
      }
   }

   // get past expanded text
   while ((last_char = FGETC(fp)) != EOF)
   {
      // here are all M,D etc...
      if (last_char == ')')
      {
         break;
      }
   }
   // vt options
   double rot, scale;
   int    neg, mir;
   get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, NULL);

   tmp = "";
   if (last_char == ',')   // if a CW or CCW follow, there must have been a comma.
   {
      while ((c = FGETC(fp)) != EOF)
      {
         if (isspace(c))
            break;
         else
         {
            tmp += c;
         }
      }
   }
   
   fprintf(ferr,"Vector text not implemented at %ld\n", linecnt);
   display_error++;
   return 1;
}

/****************************************************************************
* Do contour scan, only Move,Draw, Arc
*/
static void do_barcocontour(int fill, int neg)
{
   char     c;
   int      correction = FALSE;
   char     app_is_text = FALSE;
   int      cur_negative = neg;
   double   cur_scale = 1.0;
   int      cur_mirror = 0;
   int      cur_fill = fill;
   double   app_rot = 0;            // in degrees

   while ((c = FGETC(fp)) != EOF)
   {
      if (isspace(c))   continue;
      switch (toupper(c))
      {
         case ';':
            // remark goto end of line
            while ((c = FGETC(fp)) != EOF)
            {
               if (c == '\n' || c == '\r')
                  break;
            }
         break;
         case 'M':
            do_move(cur_negative, cur_fill);
         break;
         case 'D':
            do_draw(cur_negative, cur_fill);
         break;
         case 'C':
            do_circle();
         break;
         case 'V':
            do_vectortext();
         break;
         default:
            UNGETC(c,fp);  // backout last char
            write_poly(cur_negative, cur_fill);
            return;
         break;
      }
   }

   write_poly(cur_negative, cur_fill); // last thing to do
   return;
}

/****************************************************************************
* Do all scan
*/
int do_barco(int fill)
{
   char     cur_prosa[255];
   char     c;
   int      correction = FALSE;
   int      cur_negative = FALSE;
   int      cur_mirror = 0;
   int      cur_fill = fill;
   double   cur_scale = 1.0;
   CString  cur_apname;
   CString  original_apname;
   char     app_is_text = FALSE;
   double   char_width;
   double   char_height;
   double   app_rot = 0;            // in degrees

   while ((c = FGETC(fp)) != EOF)
   {
      if (isspace(c))   continue;

      // this is from an recursive block
      if (c == ')')     
      {
         write_poly(cur_negative, cur_fill);
         return TRUE;
      }

      switch (toupper(c))
      {
         case ';':
            // remark goto end of line
            while ((c = FGETC(fp)) != EOF)
            {
               if (c == '\n' || c == '\r')
                  break;
            }
         break;
         case 'U':
            // unit u= mm
            //       INCH
            //       MIL
            if (!get_unit(fp))
            {
               // error no correct unit command found
               return FALSE;
            }
         break;         
         case 'X':   // not needed
         {
            // xmin,max
            double x;
            get_real(fp,&x);
            get_real(fp,&x);
         }
         break;
         case 'Y':   // not needed
         {
            // ymin,max
            double y;
            get_real(fp,&y);
            get_real(fp,&y);
         }
         break;
         case 'N':   // netlist value
         {
            int   netnr;
            char  netname[80];

            if (get_int(fp,&netnr))
            {
               if (last_char == '=')
               {
                  get_name(fp,netname,80);
               }
            }
         }
         break;
         case 'P':
            get_point(fp);
         break;
         case 'A':
         {
            // aperture definition
            int   appnr = 0;
            char  appname[80];

            // here write last poly
            write_poly(cur_negative, cur_fill); // write last open poly

            app_rot = 0;
            app_is_text = FALSE;
            cur_fill = FALSE;
            get_int(fp,&appnr);
            // get the equal sign
            if ((c = FGETC(fp)) == EOF)
            {
               // unsuspected EOF
               return FALSE;
            }

            original_apname.Format("A%d",appnr);

            if (c != '=') // this is just a call, not a definition
            {
               cur_apname.Format("A%d",get_unique(appnr));
               int   err;
               cur_widthindex = Graph_Aperture(cur_apname, 0, 0.0, 0.0,0.0, 0.0, 0.0, 0,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database
            }
            else           
            if (get_name(fp,appname,80))
            {        
               if (!STRNICMP(appname,"COMPLEX",3))
               {
                  // complex
                  int i  = 0;
                  double scale, rot = 0;
                  int    neg = 0;
                  int    mir = 0;
                  double   lastx, lasty;

                  cur_widthindex = 0;
                  // make block
                  cur_apname.Format("A%d",set_unique(appnr));

                  BlockStruct *b = Graph_Block_On(GBO_APPEND,cur_apname,cur_filenum,0);   
                  // do barco until closed braket
                  if ((c = FGETC(fp)) == EOF)
                  {
                     // unsuspected EOF
                     return FALSE;
                  }
                  lastx = cur_x;
                  lasty = cur_y;
                  do_barco(1);
                  cur_x = lastx;
                  cur_y = lasty;

                  Graph_Block_Off();

                  // here get comma, if exist
                  if ((last_char = FGETC(fp)) == EOF)
                  {
                     // unexpected EOF
                     return FALSE;
                  }
                  get_appoptions(fp, b, &rot, &neg, &mir, &scale, NULL);
                  cur_negative = neg;
                  app_rot    = rot;
                  cur_scale  = scale;
                  cur_mirror = mir;
               }
               else
               if (!STRNICMP(appname,"CONTOUR",2))
               {
                  // contour
                  // complex
                  int i  = 0;
                  double rot, scale;
                  int    neg, mir;
                  cur_fill = 1;  
                  cur_widthindex = 0;  
                  // make block
                  cur_apname.Format("A%d",set_unique(appnr));

                  BlockStruct *b = Graph_Block_On(GBO_APPEND,cur_apname,cur_filenum, 0);  
                  get_appoptions(fp, b, &rot, &neg, &mir, &scale, NULL);

                  // here get to the next character
                  while ((c = FGETC(fp)) != EOF)
                  {
                     if (!isspace(c))
                     {
                        UNGETC(c,fp);
                        break;
                     }
                  }
   
                  do_barcocontour(1, neg);
                  Graph_Block_Off();

                  DataStruct *d = Graph_Block_Reference(cur_apname, NULL, cur_filenum, 0.0, 0.0,DegToRad(rot),
                        mir, 1.0, cur_layernum, TRUE);

               }
               else
               if (!STRNICMP(appname,"BOX",2))
               {
                  // box 
                  double x, y, rot, scale;

                  if(get_real(fp,&x))
                  {
                     int   err, neg, mir;

                     x = cnv_unit(x);
                     if (get_real(fp,&y))
                     {
                        y = cnv_unit(y);

                        // get cornerform
                        CString  cornerform;
                        cornerform = "";
                        while ((c = FGETC(fp)) != EOF)
                        {
                           if (isspace(c) || c == ',')
                           {
                              UNGETC(c,fp);
                              break;
                           }
                           cornerform += c;
                        }

                        double xcutoff = 0, ycutoff = 0;
                        char   cutform = '\0';
                        if (strlen(cornerform))
                        {
                           char  tmp[80];
                           char  *lp;
                           strcpy(tmp, cornerform);
                           if (lp = strtok(tmp,"=:"))
                           {
                              cutform = lp[0];
                              if (lp = strtok(NULL,"=:"))
                              {
                                 xcutoff = ycutoff = cnv_unit(atof(lp));
                                 if (lp = strtok(NULL,"=:"))
                                 {
                                    ycutoff = cnv_unit(atof(lp));
                                 }
                              }
                           }
                        }

                        cur_apname.Format("A%d",set_unique(appnr));

                        if (strlen(cornerform) == 0)
                        {
                           Graph_Aperture(cur_apname, T_RECTANGLE, x, y, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
                        }
                        else
                        if (cutform == 'R' && xcutoff == ycutoff && xcutoff == x/2)
                        {
                           Graph_Aperture(cur_apname, T_OBLONG, x, y, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
                        }
                        else
                        {
                           BlockStruct *b = Graph_Block_On(GBO_APPEND,cur_apname,cur_filenum, 0);  
                           int cw = cur_widthindex;

                           if (xcutoff == 0 || ycutoff == 0)   cutform = ' ';
                           
                           // build a box.
                           switch (cutform)
                           {
                              case 'R':   // round;
                                 /*
                                    Start from 1 counter clock                   
                                   /6-5\
                                   7   4
                                   |   | 
                                   8   3 
                                   \1-2/
                                 */
                                 ppolycnt = 0;
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = .5;   // 90 degree
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2 ;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = .5;
                                 ppolycnt++;
               
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = y/2 ;
                                 ppoly[ppolycnt].bulge = .5;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = .5;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                              break;
                              case 'S':   // straight
                                 /*
                                    Start from 1 counter clock                   
                                   /6-5\
                                   7   4
                                   |   | 
                                   8   3 
                                   \1-2/
                                 */
                                 ppolycnt = 0;
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2 ;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
               
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = y/2 ;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                              break;
                              case 'A':   // antique
                                 /*
                                    Start from 1 counter clock                   
                                   /6-5\
                                   7   4
                                   |   | 
                                   8   3 
                                   \1-2/
                                 */
                                 ppolycnt = 0;
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = -.5;  // -90 degree
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = x/2 ;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = -.5;
                                 ppolycnt++;
               
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = y/2 ;
                                 ppoly[ppolycnt].bulge = -.5;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = -.5;
                                 ppolycnt++;

                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                              break;
                              case 'C':   // cut
                                 /*
                                    Start from 1 counter clock                   
                                    8--7
                                    |  |
                                 10-9  6-5
                                 |       |
                                 11-12 3-4
                                    |  |
                                    1--2
                                 */

                                 //1
                                 ppolycnt = 0;
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 //2
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 //3
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 4
                                 ppoly[ppolycnt].x = x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 //5
                                 ppoly[ppolycnt].x = x/2 ;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 //6
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 7
                                 ppoly[ppolycnt].x = x/2 - xcutoff;
                                 ppoly[ppolycnt].y = y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 8
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = y/2 ;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 9
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 10
                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = y/2 - ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 11
                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 12
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 // 13
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2 + ycutoff;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 // 14
                                 ppoly[ppolycnt].x = -x/2 + xcutoff;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                              break;      
                              default:
                                 ppolycnt = 0;
                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 ppoly[ppolycnt].x = x/2;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 ppoly[ppolycnt].x = x/2;
                                 ppoly[ppolycnt].y = y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                                 ppoly[ppolycnt].x = -x/2;
                                 ppoly[ppolycnt].y = -y/2;
                                 ppoly[ppolycnt].bulge = 0;
                                 ppolycnt++;
                              break;
                           }
                           cur_widthindex = 0;
                           write_poly(0, 1);       
                           cur_widthindex = cw;
                           Graph_Block_Off();
                        }

                        // here get comma, if exist
                        if ((last_char = FGETC(fp)) == EOF)
                        {
                           // unexpected EOF
                           return FALSE;
                        }

                        get_appoptions(fp, NULL, &rot, &neg, &mir,&scale, NULL);
                        // rotation not evaluated in BOX
                        cur_negative = neg;
                        app_rot    = rot;
                        cur_scale  = scale;
                        cur_mirror = mir;
                     }
                  }     
               }
               else
               if (!STRNICMP(appname,"BLOCK",1))
               {
                  // complex
                  int      i  = 0;
                  double   rot, scale;
                  int      neg, mir;
                  double   lastx, lasty;
                  
                  // do barco until closed braket
                  if ((c = FGETC(fp)) == EOF)
                  {
                     // unsuspected EOF
                     return FALSE;
                  }
                  if (c == '(')
                  {
                     original_apname.Format("A%d",appnr);
                     cur_apname.Format("A%d",set_unique(appnr));
                     CString   block_apname = cur_apname;
                     BlockStruct *b = Graph_Block_On(GBO_APPEND,cur_apname,cur_filenum, 0);
                     b->setOriginalName(original_apname);
                     lastx = cur_x;
                     lasty = cur_y;
                     do_barco(0);
                     cur_x = lastx;
                     cur_y = lasty;
                     Graph_Block_Off();

                     // here get comma, if exist
                     if ((last_char = FGETC(fp)) == EOF)
                     {
                        // unexpected EOF
                        return FALSE;
                     }
                     get_appoptions(fp, b, &rot, &neg, &mir, &scale, NULL);
                     cur_negative = neg;
                     cur_mirror = mir;
                     app_rot    = rot;
                     cur_scale  = scale;
                     cur_apname = block_apname;
                  }
                  else
                  if (c == 'A')
                  {
                     // here is a block assigment
                     //A2001=BLOCK,A2001,R=180,ATTR=(uPCB="0063600015;A-20") F261.5,334
                     CString  refname, original_refname;
                     int refappnr;  
                     int assign_appnr = appnr;

                     original_apname.Format("A%d",appnr);   // store original name

                     get_int(fp,&refappnr);  // get old block, which gets assigned to a new block
                     original_refname.Format("A%d",refappnr);  // store original name

                     refname.Format("A%d",get_unique(refappnr));  // find name used for the used block

                     cur_apname.Format("A%d", set_unique(appnr));
                     get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, NULL);
                     cur_mirror = mir;
                     cur_negative = neg;
                     app_rot    = rot;
                     cur_scale = scale;
                     // here make a new block 
                     BlockStruct *b = Graph_Block_On(GBO_APPEND,cur_apname,cur_filenum, 0);  
/*
                     DataStruct *d = Graph_Block_Reference(refname, NULL, cur_filenum, 0.0, 0.0,DegToRad(rot),
                           mir, scale, cur_layernum, TRUE);
*/
                     DataStruct *d = Graph_Block_Reference(refname, NULL, cur_filenum, 0.0, 0.0,DegToRad(0.0),
                           0, 1.0, cur_layernum, TRUE);
                     doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0),
                        VT_STRING,
                        original_refname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
                     Graph_Block_Off();
                     doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0),
                        VT_STRING, original_apname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height

                  }
                  else
                  {
                     ErrorMessage("Something wrong in Block",cur_apname);
                  }
               }
               else
               if (!STRNICMP(appname,"CIRCLE",1))
               {
                  double   x;
                  double   rot, scale;
                  int      neg, mir;

                  if(get_real(fp,&x))
                  {
                     int   err;
                     x = cnv_unit(x);

                     get_appoptions(fp, NULL, &rot,&neg, &mir, &scale, NULL);
                     cur_negative = neg;
                     // rotation not evaluated in CIRCLE
                     // make a aperture CIRCLE with defined size.
                     cur_apname.Format("A%d",set_unique(appnr));
                     cur_widthindex = Graph_Aperture(cur_apname, T_ROUND, 
                           x, 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
                  }
               }
               else
               if (!STRNICMP(appname,"OCTAGON",1))
               {
                  double   x;
                  double   rot, scale;
                  int      neg, mir;

                  if(get_real(fp,&x))
                  {
                     int   err;
                     x = cnv_unit(x);

                     get_appoptions(fp, NULL, &rot,&neg, &mir, &scale, NULL);
                     cur_negative = neg;
                     cur_apname.Format("A%d",set_unique(appnr));
                     cur_widthindex = Graph_Aperture(cur_apname, T_OCTAGON, 
                           x, 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
                  }
               }
               else
               if (!STRNICMP(appname,"DONUT",1))
               {
                  double x, y, rot, scale;
                  char   opstring[80];

                  if(get_real(fp,&x))
                  {
                     int   err, neg, mir;

                     x = cnv_unit(x);
                     if (get_real(fp,&y))
                     {
                        y = cnv_unit(y);

                        get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, opstring);
                        cur_scale  = scale;
                     
                        // rotation not evaluated in DONUT
                        cur_apname.Format("A%d",set_unique(appnr));

                        // if no opstring or RR than it is a normal DONUT
                        if (strlen(opstring) && STRCMPI(opstring,"RR"))
                        {
                           // here make a complex aperture
                           Graph_Block_On(GBO_APPEND, cur_apname, cur_filenum, BL_GLOBAL);   // generate sub block

                           Graph_PolyStruct(FLOATLAYER,  0, 0);
                           Graph_Poly(NULL, 0, TRUE, 0, 1);
                           if (opstring[0] == 'S')
                           {
                              Graph_Vertex(-x/2,-x/2,0.0);
                              Graph_Vertex(x/2,-x/2,0.0);
                              Graph_Vertex(x/2,x/2,0.0);
                              Graph_Vertex(-x/2,x/2,0.0);
                              Graph_Vertex(-x/2,-x/2,0.0);
                           }
                           else
                           {
                              Graph_Vertex(-x/2,0,1.0);
                              Graph_Vertex(x/2,0,1.0);
                              Graph_Vertex(-x/2,0,0.0);
                           }

                           Graph_Poly(NULL, 0, TRUE, 1, 1);
                           if (opstring[1] == 'S')
                           {
                              Graph_Vertex(-y/2,-y/2,0.0);
                              Graph_Vertex(y/2,-y/2,0.0);
                              Graph_Vertex(y/2,y/2,0.0);
                              Graph_Vertex(-y/2,y/2,0.0);
                              Graph_Vertex(-y/2,-y/2,0.0);
                           }
                           else
                           {
                              Graph_Vertex(-y/2,0,1.0);
                              Graph_Vertex(y/2,0,1.0);
                              Graph_Vertex(-y/2,0,0.0);
                           }

                           Graph_Block_Off();
                        }
                        else
                        {
                           Graph_Aperture(cur_apname, T_DONUT, 
                              x, y, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
                        }
                     }
                  }
               }
               else
               if (!STRNICMP(appname,"SQUARE",1))
               {
                  double x, rot, scale;
                  if(get_real(fp,&x))
                  {
                     int   err, neg, mir;
                     int   makecomplex = FALSE;

                     if (x * unitpermil > 250)
                        makecomplex = TRUE;

                     x = cnv_unit(x);
                     get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, NULL);
                     cur_scale  = scale;
                     
                     cur_apname.Format("A%d",set_unique(appnr));
                     if (neg || makecomplex)
                     {
                        // if rotated, make it a complex aperture
                        make_complexaperture(cur_apname,
                                             T_SQUARE,
                                             x, 0.0, rot, neg);                                                
                     }
                     else      
                     {
                        cur_widthindex = Graph_Aperture(cur_apname, T_SQUARE, 
                           x, 0.0,0.0, 0.0, DegToRad(rot), 0, BL_APERTURE, FALSE, &err);
                     }
                  }
               }
               else
               if (!STRNICMP(appname,"RECTANGLE",1))
               {
                  double x, y, rot, scale;

                  if(get_real(fp,&x))
                  {
                     int   err, neg, mir;
                     int   makecomplex = FALSE;

                     if (x * unitpermil > 250)
                        makecomplex = TRUE;

                     x = cnv_unit(x);
                     if(get_real(fp,&y))
                     {        
                        if (y * unitpermil > 250)
                           makecomplex = TRUE;

                        y = cnv_unit(y);

                        get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, NULL);
                        cur_scale  = scale;

                        cur_apname.Format("A%d",set_unique(appnr));
                        if (neg || makecomplex) // we can not make negative apretures, 
                                                   //so make it a block
                        {
                           // if rotated make it a complex aperture
                           make_complexaperture(cur_apname,
                                             T_RECTANGLE,
                                             x, y, rot, neg);                                               
                        }
                        else
                        {
                           Graph_Aperture(cur_apname, T_RECTANGLE, 
                              x, y,0.0, 0.0, DegToRad(rot), 0, BL_APERTURE, FALSE, &err);
                        }
                     }
                  }
               }
               else
               if (!STRNICMP(appname,"THERMAL",3))
               {
                  double x, y, rot, scale;
                  double gap, angle;
                  int    numgap;
                  char   kind[20];

                  if(get_real(fp,&x))
                  {
                     int   err, neg, mir;
                     x = cnv_unit(x);
                     if(get_real(fp,&y))
                     {        
                        y = cnv_unit(y);

                        if(get_real(fp,&gap))
                        {
                           gap = cnv_unit(gap);

                           if (get_int(fp,&numgap))
                           {
                              if(get_real(fp,&angle))
                              {
                                 if (get_name(fp,kind,20))
                                 {
                                    get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, NULL);
                                    cur_scale  = scale;
                                                      
                                    cur_apname.Format("A%d",set_unique(appnr));
                                    int code = Graph_Aperture(cur_apname, T_THERMAL, 
                                       x, y, 0.0, 0.0, DegToRad(rot), 0, BL_APERTURE, FALSE, &err);
                                    BlockStruct *bb = doc->getWidthTable()[code];
                                    bb->setSizeC((DbUnit)gap);
                                    bb->setSizeD((DbUnit)DegToRad(angle));
                                    bb->setSpokeCount(numgap);
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               else
               if (!STRNICMP(appname,"TEXT",1))
               {
                  char     prosa[255];                
                  double   w,h, rot, scale;
                  int      neg, mir;

                  app_is_text = TRUE;        
                  get_prosa(fp,prosa,255);
                  strcpy(cur_prosa,prosa);
                  char_height = default_char_height;
                  char_width  = default_char_width;

                  // optional height default is 100 MIL
                  // optional width  default is 84.21% of height
                  get_charheightwidth(fp,&h,&w);
                  char_height = h;
                  char_width  = w;
                  get_appoptions(fp, NULL, &rot, &neg, &mir, &scale, NULL);
                  cur_scale  = scale;
                  cur_mirror = mir;
                  cur_negative = neg;
                  app_rot    = rot;
               }
               else
               {
                  // unknown aperture found;
                  fprintf(ferr, "Unknown Aperture [%s] at %ld\n", appname, linecnt);
                  display_error++;
               }
            } // get name
         }
         break;
         case 'M':
            do_move(cur_negative, cur_fill);
         break;
         case 'D':
            do_draw(cur_negative, cur_fill);
         break;
         case 'C':
            do_circle();
         break;
         case 'V':
            do_vectortext();
         break;
         case 'F':
         {
            double x,y;

            write_poly(cur_negative, cur_fill);             
            if (get_real(fp,&x))
               cur_x = cnv_unit(x);
            if (get_real(fp,&y))
               cur_y = cnv_unit(y);
            // flash
            if (app_is_text && strlen(cur_prosa))
            {
               // here do text
               int   prop = TRUE, mir = 0;
               double rot = 0;

               if (cur_mirror == XMIRROR)
               {
                  mir = MIRROR_FLIP;
               }
               else
               if (cur_mirror == YMIRROR)
               {
                  app_rot = 180 + app_rot;
                  mir = MIRROR_FLIP;
               }
               else
               if (cur_mirror == (XMIRROR | YMIRROR))
               {
                  mir = MIRROR_FLIP;
               }
               int penwidth = 0;
               Graph_Text(cur_layernum, cur_prosa, 
                           cur_x, cur_y,
                           char_height, char_width/**0.8421*/, 
                           DegToRad(app_rot), 0,
                           prop, (char)mir, 0,cur_negative, penwidth, 0);
            }
            else
            {
               int   mirror = 0;
               double scale = cur_scale;

               DataStruct *d = Graph_Block_Reference(cur_apname, NULL, cur_filenum, cur_x, cur_y,DegToRad(app_rot),
                     mirror, scale, cur_layernum, TRUE);
               if (cur_negative) d->setNegative(true);

               if (cur_mirror == XMIRROR)
               {
                  d->getInsert()->setMirrorFlags(MIRROR_FLIP);
               }
               else
               if (cur_mirror == YMIRROR)
               {
                  double   rot = 360 - app_rot; // or rot + 180 ?????
                  d->getInsert()->setAngle(DegToRad(rot));
                  d->getInsert()->setMirrorFlags(MIRROR_FLIP);
               }
               else
               if (cur_mirror == (XMIRROR | YMIRROR))
               {
                  d->getInsert()->setMirrorFlags(MIRROR_FLIP);
               }

               if (strlen(original_apname))
               {
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, 0),
                  VT_STRING, original_apname.GetBuffer(0), SA_OVERWRITE, NULL); //  
               }
            }
         }
         break;
         default:
         {
            // this is unknown
            fprintf(ferr, "Unknown Entity [%c] at %ld\n",c, linecnt);
            display_error++;
         }
         break;
      }
   }
   write_poly(cur_negative, cur_fill);
   return TRUE;
}

/****************************************************************************/
/*
*/
double   cnv_unit(double x)
{
   return(x*unitscale);
}

/****************************************************************************/
/*
*/
static void write_poly(int negative, int filled)
{
   int   i;
   int   widthIndex = cur_widthindex;

   if (ppolycnt == 0)   return;

   Graph_PolyStruct(cur_layernum, 0, negative);
   Graph_Poly(NULL, widthIndex, filled, 0, 0);
  
   for (i=0;i<ppolycnt;i++)
   {
      Graph_Vertex(ppoly[i].x, ppoly[i].y, ppoly[i].bulge);
   }
   ppolycnt = 0;
   return;
}

// end BARCOIN.CPP
