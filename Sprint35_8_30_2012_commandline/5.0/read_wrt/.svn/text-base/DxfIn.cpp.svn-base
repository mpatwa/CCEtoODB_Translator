// $Header: /CAMCAD/4.5/read_wrt/DxfIn.cpp 32    4/28/06 2:19p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.


  MText : Multiline Text:
  In DXF, some multiline text gets a linebreack do to a rectangle limit 41. This can not be always
  correctly calculated. The effect is that a complete word can move to the next line.

*/           

// POLYENDS 0 is no poly ends

#include "stdafx.h"
#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include <math.h>
#include "gauge.h"
#include "pcbutil.h"
#include "DXFIN.h"
#include "extents.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include <bbsdefs.h>
#include <c2vdefs.h>
#include <c2ddefs.h>
#include <c2cdefs.h>
#include <vpidefs.h>
#include <vp2defs.h>
#include <grrdefs.h>
#include <v2ddefs.h>
#include <dmldefs.h>
#include <dmdefs.h>
}

extern CProgressDlg *progress;

/* Static Variables ***********************************************************/

static   double            TEXTLENGTHCORRECT = 0.9;   // calc the length of a text 

static   int            insert_flag;

static   DXFLocal       *G;
static   char           proportional;
static   char           prefix[MAXPREFIX+1];
static   int            circle_2_app;
static   int            donut_2_app;
static   int            acad_status;
static   CString        polyline_layer;   // vertex layers are not always correct - need to store polyline layer

static   DXFPolyArray   dxfpolyarray;
static   int            polycnt;

static   int            polyline_70flag;
static   int            closed; // check closed polyline
static   char           message[64],buffer[20];

static   double         block_ref_x, block_ref_y;
static   double         cur_230  = 0.0;
static   double         globalwidth = 0.0, globalwidth41 = 0.0;
static   int            poly_ends = 0;    // 0 is endcap, T_ROUND is round
static   int            fill_zeropoly = 0;  
static   int            dxf_entitysection = FALSE;
static   char           lastchar = 0;     // needed for special line end delimeters
static   int            cur_filenum = 0;
static   int            widthindex0;      // index for linewidth 0 (not smallwidth)

static   int            display_error;
static   int            display_log;
static   int            autoclose;
static   int            DETECT_PARITY;
static   int            ATTRIBUTE2TEXT;

static   CCEtoODBDoc     *doc;
static   FILE           *flog;

static   CString        acad_3;  // multiline text

static   CDXFLayerArray dxflayerarray;
static   int            dxflayercnt = 0;

static   COLORREF       colorref[256];

static   int            textlinewidth = 0;   //0 is none, 1 is textlinewidth, 2 is textratio, 3 is textrange
static   double         TEXTRATIO = 10;

static   int            acad_handle = -1;    // acad values are in HEX
static   BlockStruct    *cur_block = NULL;

/****************************************************************************/

static   int            get_acad(FILE *);
static   int            do_acadcommand(double scale);
static   int            get_acadtype(char *);
static   void           get_error_line(long line_number);
static   char           *xfgets(char *s, int len, FILE *fp);
static   char           *dxf_text(char *s);

/****************************************************************************/
/*
   This is to be able to read dxf files which only have 0D and not 0A line ends
   return *s or NULL of EOF
*/
char * xfgets(char *s, int len, FILE *fp)
{
   int   wc, origwc;
   int   wcnt = 0;

   s[0] = '\0';
   while ((origwc = fgetc(fp)) != EOF)
   {
      if (DETECT_PARITY)
         wc = origwc & 0x7F;  // no parity bit
      else
         wc = origwc;         // no parity bit

      // here is end of line - this is to detect 2 empty lines after each other, which indicates
      // a mixed 0A 0D unix/dos combination.
      if (lastchar == 0x0D && wc == 0x0A) continue;
      if (lastchar == 0x0A && wc == 0x0D) continue;
      lastchar = wc;
      if (wcnt < len-1)
      {
         if (wc == 0x0D || wc == 0x0A)
         {
            G->acad_linecnt++;
            s[wcnt]   = '\0';
            return s;
         }
         else
         {
            // do not allow funny characters to be part of any names, layers etc...
            s[wcnt++] = wc;
         }
      }
      else
      {
         s[wcnt] = '\0';
         return s;
      }
   } // while TRUE;

   // the EOF was the last char in the DXF file without a 0a or 0d.
   s[wcnt] = '\0';
   if (!STRCMPI(s,"EOF"))
      return s;

   return NULL;
}

/****************************************************************************/
/*
   If an ENTITY layer is 0 and not an INSERT do make a real
   layer.
*/
static int dxf_level(const char *acad_8, const char *pref)
{
   int   lnr = Graph_Level(acad_8, pref, !strcmp(acad_8, "0"));
   return lnr;
}

/****************************************************************************/
/*
*/
static void clean_poly()
{
   // here make sure that there are no zero length elements
   int   i, done = FALSE;

   while (!done)
   {
      done = TRUE;
      for (i=1;i<polycnt;i++)
      {
         DxfPoly p1,p2;
         p1 = dxfpolyarray.ElementAt(i-1);  
         p2 = dxfpolyarray.ElementAt(i);  

         double len = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));

         if (fabs(len) < SMALLNUMBER)  // zero length element found
         {
            dxfpolyarray.RemoveAt(i,1);
            polycnt--;
            done = FALSE;
            break;
         }
      }
   }

   return;
}

/****************************************************************************/
/*
*/
void update_app(int typ, double size_a, double size_b, char *n)
{
   int   err;

   if (typ == T_BLANK)         
      sprintf(n,"POINT");
   else
   if (typ == T_DONUT)         
      sprintf(n,"DONUT_%2.3lf_%2.3lf",size_a,size_b);
   else
      sprintf(n,"ROUND_%2.3lf",size_a);
   Graph_Aperture(n, typ, size_a, size_b,0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
}

/****************************************************************************/
/*
*/
static int load_dxfsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;
   
   autoclose = TRUE;
   DETECT_PARITY = TRUE;
   textlinewidth = 0;
   ATTRIBUTE2TEXT = TRUE;

   for (int i=0;i<256;i++)
      colorref[i] = 0;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".ATTRIBUTES2TEXT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (!STRNICMP(lp,"n",1))
               ATTRIBUTE2TEXT = FALSE;
         }
         else
         if (!STRCMPI(lp,".AUTOCLOSE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (!STRNICMP(lp,"n",1))
               autoclose = FALSE;
         }
         else
         if (!STRCMPI(lp,".PARITYBIT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (!STRNICMP(lp,"n",1))
               DETECT_PARITY = FALSE;
         }
         else
         if (!STRCMPI(lp,".TEXTWIDTHRATIO"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            TEXTLENGTHCORRECT = atof(lp);
         }
         else
         if (!STRCMPI(lp,".TEXTRATIO"))
         {
            textlinewidth = 2;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            TEXTRATIO = atof(lp);
         }
         else
         if (!STRCMPI(lp,".COLORREF"))
         {
            int   acad_color;
            int   c1, c2, c3;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            acad_color = atoi(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            c1 = atoi(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            c2 = atoi(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            c3 = atoi(lp);

            if (acad_color > 0 && acad_color < 256)
               colorref[acad_color] = RGB(c1,c2,c3);

         }
         else
         if (!STRCMPI(lp,".DISPLAYLOG"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (!STRNICMP(lp,"y",1))
               display_log = TRUE;
         }
      }
   }

   fclose(fp);
   return 1;
}

/****************************************************************************/
/*
*/
static int get_colorindex(const char *n)
{
   int   i;

   for (i=0;i<dxflayercnt;i++)
   {
      if (!STRCMPI(n,dxflayerarray[i]->name))
         return dxflayerarray[i]->color;
   }

   return 0;
}

/****************************************************************************/
/*
*/
static int assign_layercolor()
{
   LayerStruct *lp;
   COLORREF    color;

   // now attributes after mirror layers
   for (int j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      lp = doc->getLayerArray()[j];
      if (lp == NULL)   continue; // could have been deleted.

      int lindex = get_colorindex(lp->getName()); // get the color from an DXF file.
                                             // it can return 0 if a layer was not in the table, which can
                                             // happen in BYBLOCK acad_6
      int cindex = abs(lindex);  // cindex is the color number from dxf.
      int show = TRUE;

      if (lindex < 0)   // a negative color means the layer was switched off.
         show = FALSE;

      if (cindex > 0)
      {
         switch (cindex)
         {
            case 1:  
               color = RGB(255,0,0);      // Red
            break;
            case 2:  
               color = RGB(255,255,0);    // Yellow
            break;
            case 3:  
               color = RGB(0,255,0);      // Green
            break;
            case 4:  
               color = RGB(0,255,255);    // Cyan
            break;
            case 5:  
               color = RGB(0,0,255);      // Blue
            break;
            case 6:  
               color = RGB(255,0,255);    // Magenta
            break;
            case 7:  
               color = RGB(255,255,255);  // White
            break;
            default:
               // non standard color -> need to make a lookup table
               if (cindex > 7 && cindex < 256)
                  color = colorref[cindex];
            break;
         }

         if (color)
         {
            lp->setColor(color);
            lp->setVisible(show);
            lp->setOriginalColor(color);
         }
      }

   }

   return 1;
}

/****************************************************************************/
/*
   only A-Z, 0-9, $_- are allowed
*/
static char *clean_acadlayer(char *l)
{
   static   CString  layer;
   unsigned int      i;

   layer = "";

   for (i=0;i<strlen(l);i++)
   {
      if (isalnum(l[i]) || l[i] == '$' || l[i] == '_' || l[i] == '-')
         layer += l[i];
   }

   if (strlen(layer) == 0) 
      layer = "BLANK_LAYER";

   return layer.GetBuffer(0);
}

/******************************************************************************
* ReadDXF
*/
void ReadDXF(const char *fname, CCEtoODBDoc *Doc, FormatStruct *Format, double scaleUnitsFactor)
{
   FileStruct *file;
   int   done = FALSE;
   int   error = FALSE;
   FILE  *fp;
   double scale = Format->Scale * scaleUnitsFactor;
   proportional = Format->DXF.Proportional;
   circle_2_app = Format->DXF.Circles_2_Aps;
   donut_2_app  = Format->DXF.Donuts_2_Aps;
   poly_ends    = Format->DXF.PolyEnds;
   fill_zeropoly= Format->FillPolys;
   doc = Doc;
   cur_block = NULL;

   int err;
   widthindex0 = Graph_Aperture("", T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, &err);  

   strcpy(prefix, Format->prefix);

   display_error = 0;
   display_log   = FALSE;

   CString logFile = GetLogfilePath("DXF.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // This must be binary, because lineends only 0x0d are skipped in text mode.
   if ((fp = fopen(fname,"rb")) == NULL)
   {
      ErrorMessage("Error open file", fname);//Global->GText[GT_ERROR]);
      return;
   }

   // need to be MAX_POLY + 1 beacuse poly may close.
   dxfpolyarray.SetSize(100,100);
   polycnt = 0;

   if ((G = (DXFLocal *) calloc(1,sizeof(DXFLocal))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   dxflayerarray.SetSize(100,100);
   dxflayercnt = 0;

   CString settingsFile( getApp().getImportSettingsFilePath("dxf.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nDXF Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_dxfsettings(settingsFile);

   G->acad_type = 0;
   G->acad_linecnt = 0;
   dxf_entitysection = FALSE;

   file = Graph_File_Start(fname, fileTypeDxf);
   cur_filenum = file->getFileNumber();
   // ACAD always needs a layer 0
   Graph_Level("0", prefix, TRUE);

   while (!done && !error)
   {
      int retVal = get_acad(fp);
      if (retVal == EOF)
      {
         CString  tmp;
         tmp.Format("Big Problem in DXF File at line %ld\n",G->acad_linecnt);
         error = TRUE;
         break;
      }
      else if (retVal == 0)
         continue;

      switch (G->acad_nr)
      {
         // Graph_Line_Width(0.0); to tu nie dziala 19.03.94 jb.
         case  0:
            do_acadcommand(scale);
            switch (G->acad_0_type = get_acadtype(G->acad_line))
            {
               case  A_SECTION:
                  acad_status = A_UNKNOWN;
               break;
               case A_LAYER: // layer added
                   acad_status=A_LAYER;
               break;
               case A_BLOCK:
                   acad_status=A_BLOCK;
                   block_ref_x = block_ref_y = 0.0;
               break;
               case A_DIMENSION:
                   acad_status=A_DIMENSION;
                   //block_ref_x = block_ref_y = 0.0; // do not change it !!!
               break;
               case A_ENDBLK:
                   G->acad_type = 0;
                   acad_status=A_ENDBLK;
               break;
               case A_LWPOLYLINE:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_90 = 0;   // number of vertex in R14
                  G->acad_38 = 0;
                  G->acad_39 = 0;
                  G->acad_40 = globalwidth;
                  G->acad_41 = globalwidth41;
                  G->acad_42 = 0.0; // set bulge to zero
                  G->acad_43 = 0;
                  G->acad_70 = 0;
                  cur_230 = G->acad_230 = 0.0;
               break;
               case A_LEADER:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_90 = 0;   // number of vertex in R14
                  //G->acad_40 = globalwidth;   this is text association text height
                  //G->acad_41 = globalwidth41;
                  G->acad_42 = 0.0; // set bulge to zero
                  G->acad_71 = TRUE;   // arrow is set to TRUE
                  cur_230 = G->acad_230 = 0.0;
               break;
               case A_HATCH:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_40 = 0.0;
                  G->acad_41 = 0.0;
                  G->acad_42 = 0.0; // set bulge to zero
                  G->acad_91 = 0;   // 
                  G->acad_93 = 0;
                  cur_230 = G->acad_230 = 0.0;
               break;
               case A_ELLIPSE:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_40 = 0.0;
                  G->acad_41 = 0.0;
                  G->acad_42 = 0.0; // set bulge to zero
                  G->acad_91 = 0;   // 
                  G->acad_93 = 0;
                  cur_230 = G->acad_230 = 0.0;
               break;
               case A_POLYLINE:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_40 = 0.0;
                  G->acad_41 = 0.0;
                  G->acad_42 = 0.0; // set bulge to zero
                  G->acad_70 = 0;
                  cur_230 = G->acad_230 = 0.0;
               break;
               case A_VERTEX:
                  // this may not have 40 in Vertex, so it takes 40 from Poly
                  G->acad_40 = globalwidth; // this is the width from POLY 0
                  G->acad_41 = globalwidth41; // this is the width from POLY 0
                  G->acad_42 = 0.0; // set bulge to zero
               break;
               case A_MTEXT:  // acad_41 is not the text width/ height ration but a rectangle ???
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_41 = 1.0;  // text width ratio default.
                  G->acad_73 = 0;
                  G->acad_72 = 0;
                  acad_3 = "";      // multiline text
                  G->acad_11 = 0;   // x axis direction vector
                  G->acad_21 = 0;
               break;
               case A_TEXT:
               case A_ATTRIB:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_41 = 1.0;  // text width ratio default.
                  G->acad_73 = 0;
                  G->acad_72 = 0;
                  acad_3 = "";      // multiline text
               break;
               case A_INSERT:
                  polycnt = 0;      // the acad_10, 20, 40, 41, 42 are done in dxfpolyarray
                  G->acad_41 = 1.0;
                  G->acad_42 = 1.0;
                  G->acad_43 = 1.0;
                  G->acad_50 = 0; // rotation angle
               break;
               case A_ATTDEF:
                  polycnt = 0;
                  strcpy(G->acad_1, "");
                  strcpy(G->acad_2, "");
                  G->acad_11 = 0;
                  G->acad_12 = 0;
                  G->acad_41 = 1.0;
                  G->acad_50 = 0;      // rotation
                  G->acad_70 = 0;      // visible flag, 1= invisible, anything else is visible
                  G->acad_71 = 0;      // attribute flag, 2 = mirror in x, 4 = mirror in y
                  G->acad_72 = 0;      // horizontal justification flag
                  G->acad_74 = 0;      // vertical justification flag
                  break;
               case  A_EOF:
                  done = TRUE;
               break;
               default:
#ifdef _DEBUG
                  fprintf(flog,"DEBUG: Unknown DXF Type [%s]\n", G->acad_line);
                  display_error++;
#endif
               break;
            }
         break;
         case  1:
            strcpy(G->acad_1,G->acad_line);
         break;
         case  2:
            strcpy(G->acad_2,G->acad_line);
            if (G->acad_0_type == A_SECTION)
            {
               switch (G->acad_2_type = get_acadtype(G->acad_line))
               {
                  case  A_ENTITIES:
                     dxf_entitysection = TRUE;
                     acad_status = A_ENTITIES;
                  break;

               }
            }
         break;
         case  3:
            acad_3 += G->acad_line; // multiline text
         break;
         case  5: // handle is in hex
         {
            sscanf(G->acad_line,"%x", &acad_handle);
/*
if (acad_handle == 0x1FD8)
{
int r = 0;
}
*/
         }
         break;
         case  6:
            G->acad_6 = !STRCMPI("BYBLOCK",G->acad_line);
         break;
         case  8:
            strcpy(G->acad_8,clean_acadlayer(G->acad_line));
         break;
         case  10:
         {
            G->acad_10 = atof(G->acad_line) - block_ref_x;

            // 10 starts the polygon and increments
            // 20
            // 40
            // 41
            // 42 do not do it.
            DxfPoly p;
            p.x = G->acad_10;
            p.y = 0;
            // the width is always updated after the coos.
            p.w = 0;
            p.w41 = 0;
            p.bulge = 0;
            p.acad_72 = G->acad_72;
            p.acad_93 = G->acad_93;
            p.acad_73 = G->acad_73;
            G->acad_93 = 0;
            dxfpolyarray.SetAtGrow(polycnt,p);  
            polycnt++;
         }
         break;
         case  20:
         {
            G->acad_20 = atof(G->acad_line) - block_ref_y;
            // 10 starts the polygon and increments
            // 20
            // 40
            // 41
            // 42 do not do it.
            if (polycnt)
            {
               DxfPoly p = dxfpolyarray.ElementAt(polycnt-1);  // bulgeflag is not on last vertex, but one before.
               p.y = G->acad_20;
               dxfpolyarray.SetAt(polycnt-1,p);  
            }
         }
         break;
         case  30:
            G->acad_30 = atof(G->acad_line);
				// Case 1838 - Just becasue Z value is not zero does not mean it should replace the Y value.
				// The following two lines of code that claim to "fix TSR 3912" are bad.
            ////if (G->acad_0_type == A_LINE && G->acad_30  >  SMALLNUMBER )  // if 30 is defined, overwrite 20, cause 30 is defining y and it never gets used
            ////   G->acad_20 = atof(G->acad_line);                  // this if statement is written for TSR #3912 - 12/18/02
				// It is perhaps the case that some sample data used the XZ plane instead of XY.
				// I queried for but did not receive TSR 3912 info, so until this issue is made
				// obvious again, we're back to using Y for Y and ignoring Z.
         break;
         case  11:
            G->acad_11 = atof(G->acad_line) - block_ref_x;
         break;
         case  21:
            G->acad_21 = atof(G->acad_line) - block_ref_y;
         break;
         case  31:
            G->acad_31 = atof(G->acad_line);
				// Case 1838 - See note above re same case number.
            ////if (G->acad_0_type == A_LINE && G->acad_31  >  SMALLNUMBER) // if 31 is defined, overwrite 21, cause 31 is defining y and it never gets used
            ////   G->acad_21 = atof(G->acad_line);
         break;
         case  12:
            G->acad_12 = atof(G->acad_line) - block_ref_x;
         break;
         case  22:
            G->acad_22 = atof(G->acad_line) - block_ref_y;
         break;
         case  32:
            G->acad_32 = atof(G->acad_line);
         break;
         case  13:
            G->acad_13 = atof(G->acad_line) - block_ref_x;
         break;
         case  23:
            G->acad_23 = atof(G->acad_line) - block_ref_y;
         break;
         case  33:
            G->acad_33 = atof(G->acad_line);
         break;
         case  38:
            G->acad_38 = atof(G->acad_line);
         break;                  
         case  39:
            G->acad_39 = atof(G->acad_line);
         break;  
         case  40:
         {
            G->acad_40 = atof(G->acad_line);
            // 10 starts the polygon and increments
            // 20
            // 40
            // 41
            // 42 do not do it.
            if (polycnt)
            {
               DxfPoly p = dxfpolyarray.ElementAt(polycnt-1);  // bulgeflag is not on last vertex, but one before.
               p.w = G->acad_40;
               dxfpolyarray.SetAt(polycnt-1,p);  
            }
         }
         break;
         case  41:
         {
            G->acad_41 = atof(G->acad_line);
            // 10 starts the polygon and increments
            // 20
            // 40
            // 41
            // 42 do not do it.
            if (polycnt)
            {
               DxfPoly p = dxfpolyarray.ElementAt(polycnt-1);  // bulgeflag is not on last vertex, but one before.
               p.w41 = G->acad_41;
               dxfpolyarray.SetAt(polycnt-1,p);  
            }
         }
         break;
         case  42:
         {
            // here we have an accuracy problem with bulge
            char  tmp[100];
            double x = atof(G->acad_line);
            if (x > pow(10.0,50.0))  // this is a ridiculous big number
            {
               G->acad_42 = atof(G->acad_line);
            }
            else
            {
               sprintf(tmp,"%1.6lf",atof(G->acad_line));
               G->acad_42 = atof(tmp);
            }

            // 10 starts the polygon and increments
            // 20
            // 40
            // 41
            // 42 do not do it.
            if (polycnt && fabs(G->acad_42) > SMALLNUMBER)
            {
               DxfPoly p = dxfpolyarray.ElementAt(polycnt-1);  // bulgeflag is not on last vertex, but one before.
               if (G->acad_42 < 0)
                  G->acad_42 = G->acad_42; // before it was making bulge positive.  Any particular reason?  This was drawing inverted arcs, so leave the bulge as it is - joanne
               p.bulge = G->acad_42;
               dxfpolyarray.SetAt(polycnt-1,p);  
            }
         }
         break;
         case  43:
            G->acad_43 = atof(G->acad_line);
         break;
         case  50:
            G->acad_50 = atof(G->acad_line);
         break;
         case  51:
            G->acad_51 = atof(G->acad_line);
         break;
         case  62:
            G->acad_62=atoi(G->acad_line);
         break;
         case  66:
            G->acad_66 = atoi(G->acad_line);
         break;
         case  70:
            G->acad_70 = atoi(G->acad_line);
         break;
         case  71:
            G->acad_71 = atoi(G->acad_line);
         break;
         case  72:
            G->acad_72 = atoi(G->acad_line);
         break;
         case  73:
            G->acad_73 = atoi(G->acad_line);
         break;
         case  90:
            G->acad_90 = atoi(G->acad_line);
         case  91:
            G->acad_91 = atoi(G->acad_line);
         break;
         case  93:
            G->acad_93 = atoi(G->acad_line);
         break;
         case  210:
            G->acad_210 = atof(G->acad_line);
         break;
         case  220:
            G->acad_220 = atof(G->acad_line);
         break;
         case  230:
            G->acad_230 = atof(G->acad_line);
         break;
         default:
         break;
      }
   }

   if (!error)
      do_acadcommand(scale);

   fclose(fp);

   assign_layercolor();
   EliminateSinglePointPolys(doc);                

   /* free */
   free(G); // DXFGlobal 

   for (int i=0;i<dxflayercnt;i++)
   {
      delete dxflayerarray[i];
   }
   dxflayerarray.RemoveAll();

   fclose(flog);

   if (display_error && display_log)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
*/
static int do_polyload(int closed, double scale, const char *polyline_layer)
{
   int   i;
   int   arcflg = 0;
   int   donut = FALSE;
   int   widthchange = 0;
   int   fill = 0; 
   double largewidth=0;
   int   p = polycnt;   // for debug
   int   layerindex = dxf_level(G->acad_8, prefix);

   // if the polyline had a layer and the vertex did not, use the polyline layer
   if (STRCMPI(polyline_layer, "0") && !STRCMPI(G->acad_8,"0"))
      layerindex = dxf_level(polyline_layer, prefix);
   
   if (polycnt < 2)  
   {
      polycnt = 0;
      return 0;
   }

   int   pline_ends = T_ROUND;

   if(closed)  // here make sure the closed has the last koo repeated.
   {
      DxfPoly p1 = dxfpolyarray.ElementAt(0);  
      DxfPoly p2 = dxfpolyarray.ElementAt(polycnt-1);  
      if ((p1.x != p2.x) || (p1.y != p2.y))
      {
         p1.bulge = 0;  // no bulge on the last close
         dxfpolyarray.SetAtGrow(polycnt,p1);
         polycnt++;
      }
      if (fill_zeropoly && p1.w == 0)    
         fill = TRUE;
   }
   else
   {
      // check here, it is not allowed to have a bulge on the end, widthout
      // a next point.
      DxfPoly p = dxfpolyarray.ElementAt(polycnt-1);
      p.bulge = 0;
      dxfpolyarray.SetAt(polycnt-1,p);
   }

   // make sure the last vertex never has an arc
   if (polycnt)
   {
      DxfPoly p2 = dxfpolyarray.ElementAt(polycnt-1);  
      p2.bulge = 0;
      dxfpolyarray.SetAtGrow(polycnt-1,p2);
   }

   // if the width is never larger 0, does not make a difference
   if (!closed && largewidth > 0) // if closed, endcap does not make a difference.
   {
      if (polycnt == 2 && poly_ends == 0)
      {
         // here needs to be checked if line result (after shrinking the ends)
         // still has a length. If not make a Polyfill and break
         DxfPoly  pp, pp2;
         pp = dxfpolyarray.ElementAt(0);
         pp2 = dxfpolyarray.ElementAt(1);
         double len = sqrt((pp.x-pp2.x)*(pp.x-pp2.x)+(pp.y-pp2.y)*(pp.y-pp2.y));

         if (fabs(len) < SMALLNUMBER)  // zero length element found
         {
            polycnt = 0;
            dxfpolyarray.RemoveAll();
            cur_230 = 0;
            return 0;
         }

         if (len <= pp.w)  // if length of relsulting line is smaller/equal width
         {
            //fprintf(flog,"Result line will have no width\n");
            //display_error++;

            Point2 p1a,p1b,p2a,p2b;
            Point2 p1, p2;

            p1.x = pp.x;
            p1.y = pp.y;
            p2.x = pp2.x;
            p2.y = pp2.y;
            FindCorners(pp.w, pp.w41, &p1, &p2, &p1a, &p1b, &p2a, &p2b);
            Graph_PolyStruct(layerindex, 0L, FALSE);
            Graph_Poly(NULL, widthindex0, TRUE,0,1);
            Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
            Graph_Vertex(p1b.x*scale, p1b.y*scale, 0.0);   // p1 coords
            Graph_Vertex(p2b.x*scale, p2b.y*scale, 0.0);   // p1 coords
            Graph_Vertex(p2a.x*scale, p2a.y*scale, 0.0);   // p1 coords
            Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords

            polycnt = 0;
            dxfpolyarray.RemoveAll();
            cur_230 = 0;
            return 0;
         }
      }

      if (poly_ends == 0)
      {
         // adjust for endcap
         pline_ends = T_SQUARE;
         // get first poly
         DxfPoly  p;

         int ws = polycnt; // just for debug

         p = dxfpolyarray.ElementAt(0);
         // make sure this is not a start of arc
         if (fabs(p.bulge) > SMALLNUMBER)
         {
            //fprintf(flog,"ENDCAP on bulge start at %ld\n",G->acad_linecnt);
            //display_error++;
            // this is done downwards
         }
         else
         {
            DxfPoly  p2;
            p2 = dxfpolyarray.ElementAt(1);
            // check that shorten does not need to be converted to a Rectangle
            double len = sqrt((p.x-p2.x)*(p.x-p2.x)+(p.y-p2.y)*(p.y-p2.y));
            if (len < p.w/2)
            {
               // here i need to add the width to p2
               double dx, dy;
               dx = (p.w * (p2.x - p.x)) / (2 * len);
               dy = (p.w * (p2.y - p.y)) / (2 * len);

               // here I need to make a rectangle
               Point2 p1a,p1b,p2a,p2b;
               Point2 pp1, pp2;

               pp1.x = p.x - dx;
               pp1.y = p.y - dy;
               pp2.x = p2.x;
               pp2.y = p2.y;

               FindCorners(p.w, p.w41, &pp1, &pp2, &p1a, &p1b, &p2a, &p2b);

               Graph_PolyStruct(layerindex, 0L, FALSE);
               Graph_Poly(NULL,  widthindex0, TRUE,0,1);
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
               Graph_Vertex(p1b.x*scale, p1b.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p2b.x*scale, p2b.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p2a.x*scale, p2a.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords

/******************************************************************************
               fprintf(flog,"Segment length shorter than width/2 at %ld!\n",G->acad_linecnt);
               fprintf(flog,"Segment %lf %lf %lf %lf - Width %lf\n",p.x, p.y, p2.x, p2.y, p.w);
               display_error++;
******************************************************************************/
               // kill last poly out of dxfpolyarray
               dxfpolyarray.RemoveAt(0,1);   
               polycnt--;
            }
            else
            {
               // doit : short start segment by w/2.
               
               double dx, dy;
               dx = (p.w * (p2.x - p.x)) / (2 * len);
               dy = (p.w * (p2.y - p.y)) / (2 * len);
               p.x += dx;
               p.y += dy;
               dxfpolyarray.SetAtGrow(0,p);
            }
         }

         if (polycnt < 2)  // this can happen, if the systems removed end or start point of the 
                     // dxfpolyarray
         {
            polycnt = 0;
            dxfpolyarray.RemoveAll();
            cur_230 = 0;
            return 0;
         }

         // get one before last poly
         p = dxfpolyarray.ElementAt(polycnt-2); // bulgeflag is not on last vertex, but one before.
         // make sure this is not a start of arc
         if (fabs(p.bulge) > SMALLNUMBER)
         {
            //fprintf(flog,"ENDCAP on bulge end at %ld\n",G->acad_linecnt);
            //display_error++;
            // look down !
         }
         else
         {
            DxfPoly  p2;
            p = dxfpolyarray.ElementAt(polycnt-1); 
            p2 = dxfpolyarray.ElementAt(polycnt-2);
            // check that shorten does not need to be converted to a Rectangle
            double len = sqrt((p.x-p2.x)*(p.x-p2.x)+(p.y-p2.y)*(p.y-p2.y));
            if (len < p.w/2)
            {
               // here i need to add the width to p2
               double dx, dy;
               dx = (p.w * (p2.x - p.x)) / (2 * len);
               dy = (p.w * (p2.y - p.y)) / (2 * len);

               // here I need to make a rectangle
               Point2 p1a,p1b,p2a,p2b;
               Point2 pp1, pp2;

               pp1.x = p.x;
               pp1.y = p.y;
               pp2.x = p2.x - dx;
               pp2.y = p2.y - dy;

               FindCorners(p.w, p.w41, &pp1, &pp2, &p1a, &p1b, &p2a, &p2b);

               Graph_PolyStruct(layerindex,  0L, FALSE);
               Graph_Poly(NULL,widthindex0, TRUE,0,1);
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
               Graph_Vertex(p1b.x*scale, p1b.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p2b.x*scale, p2b.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p2a.x*scale, p2a.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords

/******************************************************************************
               fprintf(flog,"Segment length shorter than width/2 at %ld!\n",G->acad_linecnt);
               fprintf(flog,"Segment %lf %lf %lf %lf - Width %lf\n",p.x, p.y, p2.x, p2.y, p.w);
               display_error++;
******************************************************************************/
               // kill last poly out of dxfpolyarray
               dxfpolyarray.RemoveAt(polycnt-1,1); 
               polycnt--;
            }
            else
            {
               // doit : short start segment by w/2.
                     
               double dx, dy;
               dx = (p.w * (p2.x - p.x)) / (2 * len);
               dy = (p.w * (p2.y - p.y)) / (2 * len);
               p.x += dx;
               p.y += dy;
               dxfpolyarray.SetAtGrow(polycnt-1,p);
            }
         }
      }
   } // if not closed

   if (polycnt < 2)  // this can happen, if the systems removed end or start point of the 
                     // dxfpolyarray
   {
      polycnt = 0;
      dxfpolyarray.RemoveAll();
      cur_230 = 0;
      return 0;
   }

   // here check if no arc included
   for (i=0;i<polycnt;i++)
   {
      DxfPoly  p;
      p = dxfpolyarray.ElementAt(i);
      if (fabs(p.bulge) > SMALLNUMBER)
      {
         arcflg++;
      }
   }

   int width_is_zero = TRUE;
   // here check if different start and end width are used.
   for (i=0;i<polycnt;i++)
   {
      DxfPoly p;
      p = dxfpolyarray.ElementAt(i);  
      
      if (p.w > 0 || p.w41 > 0)
         width_is_zero = FALSE;
      if (fabs(p.w - p.w41) > 0.0001)
      {
         widthchange++;
      }
   }

   // here test for donut. Polycnt is 3, because last koo is also first.
   if (polycnt == 3 && arcflg == 2 && closed)
   {
      DxfPoly  p, p1;
      p = dxfpolyarray.ElementAt(0);  
      p1 = dxfpolyarray.ElementAt(1);  

      // This might be a donut = circle, if both bulges are 1.
      if (fabs(p.bulge) - 1 < 0.0001 && fabs(p1.bulge) - 1 < 0.0001)
      {
         // here make a circle with a line with
         donut = TRUE;
      }
   }

   // if fill, do not care about arcs, polyends etc..
   // poly_ends 0 == encap
   // if it is a donut, just draw it.
   // the else of this if is a normal polydraw.

   if (donut)
   {
      if (donut_2_app)
      {
         DxfPoly   pp1 = dxfpolyarray.ElementAt(0);
         DxfPoly   pp2 = dxfpolyarray.ElementAt(1);

         double da = atan(pp1.bulge) * 4;
         double cx, cy, r, sa;

         ArcPoint2Angle(pp1.x,pp1.y,pp2.x,pp2.y,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;

         // check if app done
         // make an insert
         char  name[80];
         update_app(T_DONUT, 2.0*r+pp1.w*scale, 2.0*r-pp1.w*scale, name); // outer rad, inner rad
         Graph_Block_On(GBO_APPEND,name,cur_filenum,0);  // acad allows the reference before definition
         Graph_Block_Off();
         Graph_Block_Reference(name, NULL, cur_filenum, cx*scale, cy*scale,
                           0.0, 0, 1.0,
                           layerindex, TRUE);
      }
      else
      {
         // here is no width change and true polygon
         // here we need to test donut to aperture
         int err;
         DxfPoly   p = dxfpolyarray.ElementAt(0);
         DataStruct *d = Graph_PolyStruct(layerindex,0L,FALSE);
#ifdef _DEBUG
         CString tmp;
         tmp.Format("%x", acad_handle);
         doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
#endif

         Graph_Poly(NULL,
            Graph_Aperture("", T_ROUND, p.w * scale, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, &err), // index of width table
            fill,0, closed);
         for (i=0;i<polycnt ;i++)
         {
            DxfPoly   pp = dxfpolyarray.ElementAt(i);
            Graph_Vertex(pp.x*scale, pp.y*scale,pp.bulge);
         }
      }
   }
   else
   if (fill || width_is_zero)
   {
      // here is no width change and true polygon
      // here we need to test donut to aperture
      int err;
      DxfPoly   p = dxfpolyarray.ElementAt(0);
      DataStruct *d = Graph_PolyStruct(layerindex,0L,FALSE);
#ifdef _DEBUG
         CString tmp;
         tmp.Format("%x", acad_handle);
         doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
#endif

      Graph_Poly(NULL,
         Graph_Aperture("", T_ROUND, p.w * scale, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, &err), // index of width table
         fill,0, closed);
      for (i=0;i<polycnt ;i++)
      {
         DxfPoly   pp = dxfpolyarray.ElementAt(i);
         Graph_Vertex(pp.x*scale, pp.y*scale,pp.bulge);
      }
   }
   else
   if (widthchange || (poly_ends == 0))   // no width change and roud ends
   {
      double oldwidth = -1;
      int    cont = FALSE;

      for (i=0;i<polycnt-1;i++)  // stops one before end
      {
         DxfPoly   pp1 = dxfpolyarray.ElementAt(i);
         DxfPoly   pp2 = dxfpolyarray.ElementAt(i+1);
         double w = pp1.w;

         if (oldwidth != w)   cont = FALSE;

         if (fabs(pp1.bulge) < SMALLNUMBER)  // straight line
         {
            // here widthchange - startwidth != endwidth
            if (pp1.w != pp1.w41)
            {
               Point2 p1a,p1b,p2a,p2b;
               Point2 p1, p2;
               cont = FALSE;
               // width has changed. startwidth != endwidth

               p1.x = pp1.x;
               p1.y = pp1.y;
               p2.x = pp2.x;
               p2.y = pp2.y;
               FindCorners(pp1.w, pp1.w41, &p1, &p2, 
                        &p1a, &p1b, &p2a, &p2b);
               Graph_PolyStruct(layerindex, 0L, FALSE);
               Graph_Poly(NULL,  widthindex0,TRUE,0,1);
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
               Graph_Vertex(p1b.x*scale, p1b.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p2b.x*scale, p2b.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p2a.x*scale, p2a.y*scale, 0.0);   // p1 coords
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
            }
            else // when there's no width change               
            {
               // here same width
               // here need to check if ENDCAP
               if (poly_ends == 0)
                  {
                  if (closed)  // if closed, endcap doesn't affect anything. If we don't use aperture to draw, then gives wrong shape when the slope is defined and less then 1
                  {
                     Graph_Poly(NULL,
                        Graph_Aperture("", pline_ends, pp1.w * scale, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, 0), // index of width table
                        fill,0, closed);
                     for (i=0;i<polycnt ;i++)
                     {
                        DxfPoly   pp = dxfpolyarray.ElementAt(i);
                        Graph_Vertex(pp.x*scale, pp.y*scale,pp.bulge);
                     }
                  }
                  else
                  {

                     Point2 p1a,p1b,p2a,p2b;
                     Point2 p1, p2;
                     cont = FALSE;
                     p1.x = pp1.x;
                     p1.y = pp1.y;
                     p2.x = pp2.x;
                     p2.y = pp2.y;
                     FindCorners(pp1.w, pp1.w41, &p1, &p2, &p1a, &p1b, &p2a, &p2b);

                     Graph_PolyStruct(layerindex, 0L, FALSE);
                     Graph_Poly(NULL, widthindex0, TRUE,0,1);
                     Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
                     Graph_Vertex(p1b.x*scale, p1b.y*scale, 0.0);   // p1 coords
                     Graph_Vertex(p2b.x*scale, p2b.y*scale, 0.0);   // p1 coords
                     Graph_Vertex(p2a.x*scale, p2a.y*scale, 0.0);   // p1 coords
                     Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
                  }
               }
               else
               {
                  int err;

                  if (pline_ends == T_ROUND)
                  {
                     if (!cont)
                     {
                        int windex =   Graph_Aperture("", T_ROUND, w * scale, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        Graph_PolyStruct(layerindex, 0L, FALSE);
                        Graph_Poly(NULL, windex, 0, 0, 0);
                        Graph_Vertex(pp1.x*scale, pp1.y*scale, 0.0);    // p1 coords
                     }
                     Graph_Vertex(pp2.x*scale, pp2.y*scale, 0.0);    // p2 coords
                     cont = TRUE;
                  }
                  else
                  {
                     cont = FALSE;
                     Graph_Line(layerindex,
                        pp1.x*scale, pp1.y*scale,    // p1 coords
                        pp2.x*scale, pp2.y*scale, 0,  // p2 coords, flag
                        Graph_Aperture("", pline_ends, w * scale, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err), 
                        FALSE); // index of width table
                  }
               }
            }
            oldwidth = w;
         }
         else // width change with bulge
         {
            cont = FALSE;
            if (pp2.w < w)
               w = pp2.w;

            // here widthchange - startwidth != endwidth make hoolow arc
            // also make it if flat end.
            if (pp1.w != pp2.w)
            {
               // width has changed. startwidth != endwidth
               //fprintf(flog,"Arc with width change\n");
               //display_error++;
               Point2 p1a,p1b,p2a,p2b;
               Point2 p1, p2;

               // width has changed. startwidth != endwidth

               p1.x = pp1.x;
               p1.y = pp1.y;
               p2.x = pp2.x;
               p2.y = pp2.y;
               FindCorners_arc(pp1.bulge,&p1, &p2, pp1.w, pp1.w41, 
                        &p1a, &p1b, &p2a, &p2b);
               if (fabs(pp1.bulge) < SMALLNUMBER)
                  p1a.bulge = 0;
               else
                  p1a.bulge = pp1.bulge;

               if (fabs(pp1.bulge) < SMALLNUMBER)
                  p2b.bulge = 0;
               else
                  p2b.bulge = -pp1.bulge;
               Graph_PolyStruct(layerindex, 0L, FALSE);
               Graph_Poly(NULL,  widthindex0, TRUE,0,1);
               Graph_Vertex(p1a.x*scale, p1a.y*scale, p1a.bulge);    // p1 coords
               Graph_Vertex(p2a.x*scale, p2a.y*scale, 0);   // p1 coords
               Graph_Vertex(p2b.x*scale, p2b.y*scale, p2b.bulge);   // p1 coords
               Graph_Vertex(p1b.x*scale, p1b.y*scale, 0);   // p1 coords
               Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
            }
            else
            {
               double da = atan(pp1.bulge) * 4;
               double cx, cy, r, sa;

               ArcPoint2Angle(pp1.x,pp1.y,pp2.x,pp2.y,da,&cx,&cy,&r,&sa);
               // make positive start angle.
               if (sa < 0) sa += PI2;
               
               // do it if ENDCAP and not in the middle
               if (poly_ends == 0 && (i == 0 || i == polycnt-1))  
               {
                  Point2 p1a,p1b,p2a,p2b;
                  Point2 p1, p2;

                  p1.x = pp1.x;
                  p1.y = pp1.y;
                  p2.x = pp2.x;
                  p2.y = pp2.y;
                  FindCorners_arc(pp1.bulge,&p1, &p2, pp1.w, pp1.w41, 
                              &p1a, &p1b, &p2a, &p2b);
                  if (pp1.bulge < SMALLNUMBER)
                     p1a.bulge = 0;
                  else
                     p1a.bulge = pp1.bulge;

                  if (pp1.bulge < SMALLNUMBER)
                     p2b.bulge = 0;
                  else
                     p2b.bulge = -pp1.bulge;
                  Graph_PolyStruct(layerindex, 0L, FALSE);
                  Graph_Poly(NULL,  widthindex0,TRUE,0,1);
                  Graph_Vertex(p1a.x*scale, p1a.y*scale, p1a.bulge);    // p1 coords
                  Graph_Vertex(p2a.x*scale, p2a.y*scale, 0);   // p1 coords
                  Graph_Vertex(p2b.x*scale, p2b.y*scale, p2b.bulge);   // p1 coords
                  Graph_Vertex(p1b.x*scale, p1b.y*scale, 0);   // p1 coords
                  Graph_Vertex(p1a.x*scale, p1a.y*scale, 0.0);    // p1 coords
               }
               else
               {
                  int err;
                  Graph_Arc(layerindex,
                     cx*scale,cy*scale,            // center coords
                     r*scale,    // radius
                     sa,da,0,              // start & delta angle , flag
                     Graph_Aperture("", T_ROUND, w * scale, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err), // index of width table
                     FALSE);
               }
            } // if arc has no width change
         }
      }
   }
   else
   {
      // it should never arrive here

      // here is no width change and true polygon
      // here we need to test donut to aperture
      int err;
      DxfPoly   p = dxfpolyarray.ElementAt(0);
      DataStruct *d = Graph_PolyStruct(layerindex,0L,FALSE);
#ifdef _DEBUG
      CString tmp;
      tmp.Format("%x", acad_handle);
      doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
#endif
      Graph_Poly(NULL,
         Graph_Aperture("", pline_ends, p.w * scale, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, &err), // index of width table
         fill,0, closed);
      for (i=0;i<polycnt ;i++)
      {
         DxfPoly   pp = dxfpolyarray.ElementAt(i);
         Graph_Vertex(pp.x*scale, pp.y*scale,pp.bulge);
      }
   }

   G->acad_210=G->acad_220=G->acad_230=0;
   polycnt = 0;
   dxfpolyarray.RemoveAll();
   cur_230 = 0;
   //G->acad_40 = globalwidth; // this is the width from POLY 0
   //G->acad_41 = globalwidth41; // this is the width from POLY 0
   return 1;
}

/****************************************************************************/
/*
*/
static int isworddelimeter(char c)
{
   if (isspace(c))   return TRUE;
   return FALSE;
}


/****************************************************************************/
/*
   return longest test
*/
static int clean_multilinetext(char *c, double charwidth, double boxwidth)
{
   // This removes the font information from the string, but retain the rest
   CString tmpText = c;
   tmpText.TrimLeft();
   tmpText.TrimRight();
   if (tmpText.Left(2) == "{\\")
   {
      int n = tmpText.Find(";");
      tmpText.Delete(0, n + 1);
      tmpText.TrimRight("}");
      strcpy(c, tmpText);
   }

   // if text has \A1; kill it - I do not know what this means.
   char  tmp[5000];
   if (!_strnicmp(c, "\\A1;", 4))
   {
      strcpy(tmp, &c[4]);
      strcpy(c, tmp);
   }

   if (!_strnicmp(c, "\\U+", 3))
   {
      // \U+00B1  CHAR_PLUSMINUS             ((unsigned char)177)
      // \U+00B0  CHAR_DEGREE                ((unsigned char)176)
      // \U+2205  CHAR_DIAMETER              ((unsigned char)248)

      if (!_strnicmp(c, "\\U+00B0", 7))
      {
         strcpy(tmp, &c[7]);
         c[0] = CHAR_DEGREE;
         c[1] = '\0';
         strcat(c, tmp);
      }
      else if (!_strnicmp(c, "\\U+2205", 7))
      {
         strcpy(tmp, &c[7]);
         c[0] = CHAR_DIAMETER;
         c[1] = '\0';
         strcat(c, tmp);
      }
      else if (!_strnicmp(c, "\\U+00B1", 7))
      {
         strcpy(tmp, &c[7]);
         c[0] = CHAR_PLUSMINUS;
         c[1] = '\0';
         strcat(G->acad_1, tmp);
      }
      else
      {
         // unknown unicode
         strcpy(tmp, &c[7]);
         strcat(c, tmp);
      }
   }

   int cnt = 0;
   int toklen = strlen(c);
   if (!_strnicmp(c, "\\f", 2))
   {
      for (int i=0; i<toklen; i++)
      {  
         cnt++;
         // advance to \fArial|b0|i0|c0|p34;
         if (c[i] == ';')
            break;
      }
   }

   // if no boxwidth given, always make it one line.
   int wordwrap = round(boxwidth/charwidth);
   if (boxwidth < charwidth)
      wordwrap = toklen;

   int lastword = 0;
   int curlinelength = 0;
   int tmpcnt = 0;
   for (int i=cnt; i<toklen;i++)
   {
      if (c[i] == '\\')
      {
         i++;
         if (c[i] == 'P')
         {
            tmp[tmpcnt] = '\n';
            tmpcnt++;
            curlinelength = 0;
         }
         else
         {
            tmp[tmpcnt] = c[i];
            tmpcnt++;
            curlinelength++;
         }
      }
      else
      {
         if (isworddelimeter(c[i])) 
            lastword = tmpcnt;

         tmp[tmpcnt] = c[i];
         tmpcnt++;
         curlinelength++;

         if ((curlinelength > wordwrap) && lastword)
         {
            tmp[lastword] = '\n';
            curlinelength = tmpcnt - lastword - 1;
         }

      }
   }

   tmp[tmpcnt] = '\0';
   strcpy(c, tmp);

   int longest = 0;
   char *lp;

   lp = strtok(tmp, "\n");
   while (lp)
   {
      if ((int)strlen(lp) > longest)
         longest = strlen(lp);
      lp = strtok(NULL, "\n");
   }

   return longest;
}

/*****************************************************************************/
/*
   0 is none, 1 is textlinewidth, 2 is textratio, 3 is textrange
*/
static int get_penwidth(double textheight)
{
   switch (textlinewidth)
   {
      case 0:
         return -1;
      break;
      case 1:
         return -1;
      break;
      case 2:
      {
         int   err;
         double w = textheight * TEXTRATIO / 100;
         return Graph_Aperture("", T_ROUND, w, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, &err); // index of width table
      }
      break;
      case 3:
         return -1;
      break;
      default:
         return -1;
   }

   return -1;
}

/*****************************************************************************/
/*
*/
static int Check_0_Layer()
{
   if (dxf_entitysection)
   {
      if (!STRCMPI(G->acad_8,"0"))
         strcpy(G->acad_8, "L0");
   }

   return TRUE;
}

/*****************************************************************************/
/*
*/
static int clean_controlchar(char *c)
{
   char  tmp[MAX_LINE];

   // if text has \A1; kill it - I do not know what this means.
   if (!_strnicmp(c,"\\A1;",4))
   {
      strcpy(tmp,&c[4]);
      strcpy(c, tmp);
   }

   if (!_strnicmp(c,"\\U+",3))
   {
      // \U+00B1  CHAR_PLUSMINUS             ((unsigned char)177)
      // \U+00B0  CHAR_DEGREE                ((unsigned char)176)
      // \U+2205  CHAR_DIAMETER              ((unsigned char)248)

      if (!_strnicmp(c,"\\U+00B0",7))
      {
         strcpy(tmp,&c[7]);
         c[0] = CHAR_DEGREE;
         c[1] = '\0';
         strcat(c, tmp);
      }
      else
      if (!_strnicmp(c,"\\U+2205",7))
      {
         strcpy(tmp,&c[7]);
         c[0] = CHAR_DIAMETER;
         c[1] = '\0';
         strcat(c, tmp);
      }
      else
      if (!_strnicmp(c,"\\U+00B1",7))
      {
         strcpy(tmp,&c[7]);
         c[0] = CHAR_PLUSMINUS;
         c[1] = '\0';
         strcat(G->acad_1, tmp);
      }
      else
      {
         // unknown unicode
         strcpy(tmp,&c[7]);
         strcat(c, tmp);
      }
   }

   if (!_strnicmp(c,"\\f",2))
   {
      int cnt = 0;
      int   toklen = strlen(c);
      for (int i=0;i<toklen;i++)
      {  
         cnt++;
         // advance to \fArial|b0|i0|c0|p34;
         if (c[i] == ';')
            break;
      }
      strcpy(tmp,&c[cnt]);
      strcpy(c, tmp);
   }

   return 1;
}

/*-----------------------------------------------------*/
static VerticalPositionTag DXFToCCVerticalJustificationType(int vertical)
{
   switch(vertical)
   {
   case 2:
      return verticalPositionCenter;
   case 3:
      return verticalPositionTop;
   case 1:
      return verticalPositionBottom;
   }
   return verticalPositionBaseline;
}

/*-----------------------------------------------------*/
static int do_acadcommand(double scale)
{
   int      i;

   /* here do what has to be done */
   switch(acad_status)
   {
      case A_ENTITIES:
      break;
      case A_LAYER:
      break;
      case A_BLOCK:
      break;
      case A_DIMENSION:
      break;
      case A_ENDBLK:
      break;
      case A_EOF:
         return -2;
      default:
         return(1);
   }

   // here change layer for color if acad_6 is set.

   if (G->acad_6)
   {
      CString  tmp;
      tmp.Format("COLOR_%d",G->acad_62);
      strcpy(G->acad_8, tmp);
   }

   switch (G->acad_0_type)
   {
   case A_BLOCK:
      {
         CString  w = G->acad_2; // just for debug
         cur_block = Graph_Block_On(GBO_APPEND,G->acad_2,cur_filenum,0);
         block_ref_x = G->acad_10;
         block_ref_y = G->acad_20;
      }
      break;
   case A_ENDBLK:
      Graph_Block_Off();
      cur_block = NULL;
      block_ref_x = block_ref_y = 0.0;
      break;
   case A_LAYER:
      {
         // only do layers when called for, not when
         // defined. acad_62 is color
         //dxf_level(G->acad_2, prefix);
         DXFLayerlist   *c = new DXFLayerlist;
         dxflayerarray.SetAtGrow(dxflayercnt,c);
         c->name = G->acad_2;
         c->color = G->acad_62;
         dxflayercnt++;
      }

      break;
   case A_ATTDEF:
      {
         if (cur_block == NULL)
            break;
            
         double x = G->acad_10 * scale;
         double y = G->acad_20 * scale;
         double rot = RadToDeg(G->acad_50);
         double height = G->acad_40 * scale;
         double width = G->acad_40 * G->acad_41 * scale * TEXTLENGTHCORRECT;
         double penWidth = G->acad_40 * scale * TEXTRATIO / 100;        
         BOOL visible = !(G->acad_70 & 1);

         int err;
         int widthIndex = Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err); // index of width table
         int layerIndex = dxf_level(G->acad_8, prefix);

         BOOL mirror = FALSE;
         switch (G->acad_71)
         {
            case 2: // BACKWARDS - mirror         
               mirror = TRUE; 
               break;
            case 4: // UPSIDE-DOWN - mirror & rotate
               mirror = TRUE; 
               rot += PI;
               break;
            case 6: // UPSIDE-DOWN & BACKWARDS - mirror, then mirror & rotate
               rot += PI;
               break;
         }

         int horizontal = G->acad_72;
         int vertical = G->acad_74;
         if (horizontal || vertical)
         {
            x = G->acad_11 * scale;
            y = G->acad_21 * scale;

            Point2 point;
            point.x = x;
            point.y = y;
            point.bulge = 0;

            CExtent extent = TextBox(doc,G->acad_1,&point,width,height,rot,mirror,TRUE);
            
            if (vertical == 2)         // center
               y -= fabs(extent.getYsize())/2;
            else if (vertical == 3)    // top
               y -= fabs(extent.getYsize());

            if (horizontal == 1)       // center
               x -= fabs(extent.getXsize())/2;
            else if (horizontal == 2)  // right
               x -= fabs(extent.getXsize());
         }

         if (!strcmp(G->acad_2, "") && !strlen(G->acad_2))
            fprintf(flog, "Invalid attribute keyword, attribute will not be added - Line #: %d\n", G->acad_linecnt);

         doc->SetUnknownVisAttrib(&cur_block->getAttributesRef(), G->acad_2, G->acad_1, x, y, rot, 
               height, width, TRUE, widthIndex, visible, SA_OVERWRITE, 0, layerIndex, TRUE, 0, 0);
      }
      break;
   case  A_ATTRIB:
      {
         double rot = DegToRad(G->acad_50);
         BOOL mirror = FALSE;
         switch (G->acad_71)
         {
            case 2: // BACKWARDS - mirror         
               mirror = TRUE; 
            break;
            case 4: // UPSIDE-DOWN - mirror & rotate
               mirror = TRUE; 
               rot += PI;
            break;
            case 6: // UPSIDE-DOWN & BACKWARDS - mirror, then mirror & rotate
               rot += PI;
            break;
         }
         int penwidth = get_penwidth(G->acad_40*scale);

         if (ATTRIBUTE2TEXT)
         {
            // this is an invisible attribute.
            if (G->acad_70 & 1)  break;

            Graph_Text(dxf_level(G->acad_8, prefix),
               dxf_text(G->acad_1),
               G->acad_10*scale, G->acad_20*scale,
               G->acad_40*scale,   // height
               G->acad_40*G->acad_41*scale * TEXTLENGTHCORRECT,     // width. The width scale is from a true char size,
                                                      // not from the height.
               rot, //DegToRad(G->acad_50), 
               0, proportional, 
               mirror, //G->acad_71, 
               round(G->acad_51), FALSE, penwidth, 0); // prop flag, mirror flag, oblique angle in deg
         }
         else
         {
            int r = 0;
         }
         polycnt = 0;
      }
      break;
   case  A_TEXT:
      {
         Check_0_Layer();

         double x,y;

         x = G->acad_10;
         y = G->acad_20;

         double rot = DegToRad(G->acad_50);
         BOOL mirror = FALSE;

         switch (G->acad_71)
         {
            case 2: // BACKWARDS - mirror         
               mirror = TRUE; 
            break;
            case 4: // UPSIDE-DOWN - mirror & rotate
               mirror = TRUE; 
               rot += PI;
            break;
            case 6: // UPSIDE-DOWN & BACKWARDS - mirror, then mirror & rotate
               rot += PI;
            break;
         }

         if (G->acad_73 || G->acad_72)
         {
            x = G->acad_11;
            y = G->acad_21;
            // here need to fix the text x,y according to the adjustment.
            // 72 0 = letf 1 = center 2 = right
            // 73 0 = baseline 1 bottom 2 middle 3 top


            if (G->acad_72)
            {
               double h;
               double l;
               // only length is counted because this is only for horizontal 
               Rotate(G->acad_40*G->acad_41*TEXTLENGTHCORRECT*strlen(G->acad_1), 0, G->acad_50, &l, &h);

               if (mirror) l = -l;

               switch (G->acad_72)
               {
                  case 0:  // left
                     x = G->acad_11;                     
                     y = G->acad_21;
                  break;
                  case 1:  // middle
                     x = G->acad_11;                    
                     y = G->acad_21;
                  break;
                  case 2:
                     x = G->acad_11;                   
                     y = G->acad_21;
                  break;
                  case 3:
                     if (G->acad_73 == 0)    // only if 73 is 0
                     {
                        // aligned
                        x = G->acad_10;
                        y = G->acad_20;
                     }
                  break;               
                  case 4:
                     if (G->acad_73 == 0)    // only if 73 is 0
                     {
                        // middle
                        x = G->acad_10;
                        y = G->acad_20;
                     }
                  break;
                  case 5:
                     if (G->acad_73 == 0)    // only if 73 is 0
                     {
                        // fit means allign text between 10,20 and 11,21
                        x = G->acad_10;
                        y = G->acad_20;
                     }
                  break;
               }
            }
            
            if (G->acad_73)
            {
               double h;
               double l;
               // only height is counted because this is only for vertical
               Rotate(0.0, G->acad_40, G->acad_50, &l, &h);

               switch (G->acad_73)
               {
                  case 0:  // baseline
                     y = G->acad_21;
                  break;
                  case 1:  // bottom
                     y = G->acad_21;
                  break;
                  case 2:  // middle
                     y = G->acad_21;
                  break;
                  case 3:  // top
                     y = G->acad_21;
                  break;
               }
            }
         }

         // these 72 and 73 flags are rotation dependend!
         if (G->acad_72 && G->acad_73)
         {
            x = G->acad_10;
            y = G->acad_20;
         }

         int penwidth = get_penwidth(G->acad_40*scale);
         Graph_Text(dxf_level(G->acad_8, prefix),
                     dxf_text(G->acad_1),
                     x*scale,y*scale,
                     G->acad_40*scale,   // height
                     G->acad_40*G->acad_41*scale * TEXTLENGTHCORRECT,// width. The width scale is from a true char size,
                                                      // not from the height.
                     rot, //DegToRad(G->acad_50), 
                     0, proportional, 
                     mirror, //G->acad_71, // mirror flag - TRUE if Upside-Down or Backwards
                     round(G->acad_51), FALSE, penwidth, 0, G->acad_72, DXFToCCVerticalJustificationType(G->acad_73)); // oblique angle in deg
      }
   break;
   case  A_MTEXT:
      {
         Check_0_Layer();

         double rot = DegToRad(G->acad_50);
         BOOL mirror = FALSE;
         double offset_x = 0;
         double offset_y = 0;
         char m_text[5000];
         double textlengthcorrect = TEXTLENGTHCORRECT;   // this can change on fit

         if (G->acad_11 != 0 || G->acad_21 != 0)
         {
            // according to Autocad description, both 50 (rotation) and
            // 11, 21 has the rotation for the text. Autocad desc. says the
            // "last" one wins ???
            rot = ArcTan2(G->acad_21, G->acad_11);
         }

         clean_controlchar(G->acad_1);

         if ((strlen(acad_3) + strlen(G->acad_1)) < 5000)
         {
            strcpy(m_text, acad_3); // multiline text
            strcat(m_text, G->acad_1);
         }
         else
            strcpy(m_text, G->acad_1);

         if (G->acad_73 == 0 && G->acad_72 == 5)   // fit text
         {
            // that is figured out by testing !
            double tlen = strlen(G->acad_1) * 1.3;
            if (proportional)
               tlen = strlen(G->acad_1) * 1.2;

            // 41 is total length, 40 is text height
            textlengthcorrect = (G->acad_41 / tlen) / G->acad_40;
            if (textlengthcorrect > TEXTLENGTHCORRECT)
               textlengthcorrect = TEXTLENGTHCORRECT; // this can change on fit
         }

         int longest = clean_multilinetext(m_text, G->acad_40 * textlengthcorrect, G->acad_41);
         acad_3 = "";

         switch (G->acad_71)
         {
            case 1:  // top left. First Char start Lower left in CAMCAD.
               offset_x = 0;
               offset_y -= G->acad_40; 
               break;   
            case 2:  // top center
            case 3:  // top right
               break;
            case 4:  // middle left
               offset_y -= G->acad_40/2; 
               break;
            case 5:  // middle center
               offset_x -= (longest * G->acad_40 * textlengthcorrect)/2;
               offset_y -= G->acad_40/2; 
               break;
            case 6:  // middle right
               offset_x -= (longest * G->acad_40 * textlengthcorrect);
               offset_y -= G->acad_40/2; 
               break;
            case 7:  // bottom left
            case 8:  // bottom center
            case 9:  // bottom right
               break;
         }

         double ox, oy;
         Rotate(offset_x, offset_y, RadToDeg(rot), &ox, &oy);
         
         // 41 is not the with height ratio
         int penwidth = get_penwidth(G->acad_40 * scale);
         DataStruct *d = Graph_Text(dxf_level(G->acad_8, prefix), dxf_text(m_text), (ox + G->acad_10) * scale, 
                  (oy + G->acad_20) * scale, G->acad_40 * scale, G->acad_40*scale * textlengthcorrect,
                  rot, 0, proportional, mirror, round(G->acad_51), FALSE, penwidth, 0); 
      }
      break;
   case  A_ARC:
      {
         Check_0_Layer();

         if (G->acad_51 < G->acad_50) G->acad_51+=360.0;
         double deltaarc =  G->acad_51- G->acad_50;
         if (fabs(G->acad_220) > SMALLNUMBER && G->acad_220 > EXTRUSION )
         {
            // Here mirror the arc. This is used in Microstation
            // this is not completely tested, but it works on the
            // supplied examples. 
            // This part didn't work for TSR #3912, the if statement was "if (G->acad_230 < EXTRUSION)", and 
            // now changed to "if (fabs(G->acad_220) > SMALLNUMBER && G->acad_220 > EXTRUSION )".  It works for this 
            // file, but not fully tested it. - 12/18/02
            G->acad_10 *= -1;
            deltaarc  *= -1;
            G->acad_50  = 180 - G->acad_50;
         }
         else if (G->acad_230 < EXTRUSION)
         {
            G->acad_10 *= -1;
            deltaarc  *= -1;
            G->acad_50  = 180 - G->acad_50;
         }

         if (fabs(deltaarc) > SMALL_DELTADEGREE)
         {
            Graph_Arc(dxf_level(G->acad_8, prefix),
               G->acad_10*scale,G->acad_20*scale, // center coords
               G->acad_40*scale,                          // radius
               DegToRad(G->acad_50),DegToRad(deltaarc),0, // start & delta angle,flag
               widthindex0, FALSE); // index of width table
         }
         else // graph line
         {
            Graph_Line(dxf_level(G->acad_8, prefix),
                  (G->acad_10 + G->acad_40 * cos(DegToRad(G->acad_50))) * scale,
                  (G->acad_20 + G->acad_40 * sin(DegToRad(G->acad_50))) * scale, 
                  (G->acad_10 + G->acad_40 * cos(DegToRad(G->acad_50 + deltaarc))) * scale,
                  (G->acad_20 + G->acad_40 * sin(DegToRad(G->acad_50 + deltaarc))) * scale,
                  0, widthindex0, FALSE); // index of width table
         }
         G->acad_210=G->acad_220=G->acad_230=0;
      }
      break;
   case  A_CIRCLE:
      Check_0_Layer();

      // if convert circle to aperature set
      if (G->acad_230 < EXTRUSION)
      {
         // Here mirror the arc. This is used in Microstation
         // this is not completely tested, but it works on the
         // supplied examples.
         G->acad_10 *= -1;
      }
      if (circle_2_app)
      {
         // check if app done
         // make an insert
         char  name[80];
         update_app(T_ROUND,G->acad_40*scale*2,0.0, name);
         Graph_Block_On(GBO_APPEND,name,-1,0);  // acad allows the reference of a block before defintion
         Graph_Block_Off();
         Graph_Block_Reference(name, NULL, cur_filenum, G->acad_10*scale, G->acad_20*scale,
                                 0.0, 0, 1.0,
                                 dxf_level(G->acad_8, prefix), 
                                 TRUE);

      }
      else
      {
         Graph_Circle(dxf_level(G->acad_8, prefix),
            G->acad_10*scale,G->acad_20*scale,     // center coords
            G->acad_40*scale,0,                       // radius
            widthindex0, 
            FALSE, FALSE); // index of width table
      }
      G->acad_210=G->acad_220=G->acad_230=0;
      break;
   case  A_POINT:
      Check_0_Layer();

      if (circle_2_app)
      {
         // check if app done
         // make an insert
         char  name[80];
         update_app(T_BLANK, 0.0, 0.0, name);
         Graph_Block_On(GBO_APPEND,name,-1,0);  // acad allows the reference of a block before defintion
         Graph_Block_Off();
         Graph_Block_Reference(name, NULL, cur_filenum, G->acad_10*scale, G->acad_20*scale,
                                 0.0, 0, 1.0,
                                 dxf_level(G->acad_8, prefix), TRUE);
      }
      else
      {
         Graph_Point(G->acad_10*scale,G->acad_20*scale, dxf_level(G->acad_8, prefix), 0L, 0);
      }
      G->acad_210=G->acad_220=G->acad_230=0;
      break;
   case  A_POLYLINE:
      Check_0_Layer();

      closed=0;
      dxfpolyarray.RemoveAll();
      polycnt = 0;
      polyline_70flag = G->acad_70;

      if (G->acad_70 & 1)
         closed = TRUE;

      cur_230 = G->acad_230;

      globalwidth  = G->acad_40;
      globalwidth41  = G->acad_41;

/* 39 is Polyline thickness. That is according to description the distance an object extrudes.
      if (G->acad_40 == 0 && G->acad_39 > 0)
      {
         globalwidth41 = globalwidth = G->acad_39;
      }
*/
      polyline_layer = G->acad_8;
      break;
   case  A_VERTEX:
      {
         if (G->acad_70 & 16)  
         {
            fprintf(flog, "Spline Fitting is not supported !\n");
            display_log = TRUE;
            display_error++;
            break;   // this is a spline control point
         }
         // the global variable is set to 0 all the time
         if (cur_230 < EXTRUSION)
         {
            // Here mirror the arc. This is used in Microstation
            // this is not completely tested, but it works on the
            // supplied examples.
            G->acad_10 *= -1;
         }

         DxfPoly p;
         int   wp = polycnt; // debug
         p.x = G->acad_10;
         p.y = G->acad_20;
         // the width is always updated after the coos.
         p.w = G->acad_40;
         p.w41 = G->acad_41;
         p.bulge = G->acad_42;

         if (fabs(p.bulge) < SMALLNUMBER)
            p.bulge = 0;

         // check for null segments. Here polycnt is ++ because 10,20 does it
         // automatically.
         if (polycnt > 1)
         {
            DxfPoly p1 = dxfpolyarray.ElementAt(polycnt-2);  
            if (p1.x == p.x && p1.y == p.y)           
            {
               // set it back to update the width
               if (fabs(p1.bulge) > SMALLNUMBER && fabs(p.bulge) > SMALLNUMBER)
               {
                  // both have bulge - do nothing
               }
               else
               if (fabs(p1.bulge) < SMALLNUMBER && fabs(p.bulge) > SMALLNUMBER)
               {
                  // new has bulge - old not - kill old
                  polycnt--;
               }
               else
               if (fabs(p1.bulge) < SMALLNUMBER && fabs(p.bulge) > SMALLNUMBER)
               {
                  // new has no bulge - old does 
                  p.bulge = p1.bulge;
                  polycnt--;
               }
            }
         }

         dxfpolyarray.SetAtGrow(polycnt-1,p);  
         // polycnt++;
      }
      break;
   case  A_SEQEND:
      {
         if (polycnt)   // close even if the closed flag is not set.
         {
            DxfPoly p1 = dxfpolyarray.ElementAt(0);  
            DxfPoly p2 = dxfpolyarray.ElementAt(polycnt-1);  

            // there is no linewidth on a 3D polyline
            if (polyline_70flag & 8 || polyline_70flag & 16)
            {
               for (int i=0;i<polycnt;i++)
               {
                  DxfPoly p;
                  p = dxfpolyarray.ElementAt(i);      
                  p.w = 0;
                  p.w41 = 0;
                  dxfpolyarray.SetAt(i, p);
               }
            }

            if (autoclose)
            {
               if (fabs(p1.x - p2.x) < SMALLNUMBER && fabs(p1.y - p2.y) < SMALLNUMBER)
                  closed = TRUE;

               // here now test for close to 360 degree bulges. even that autocad claim that this is
               // not allowed, it seems to happen. Autocad lists this as a circle even that the bulge
               // is only 359.899
               double da = RadToDeg(atan(p1.bulge) * 4);
               if (fabs(da - 360) < 0.5)
               {
                  closed = TRUE;
               }
            }
         }

         do_polyload(closed, scale, polyline_layer);
         polycnt = 0;
         polyline_70flag = 0;
      }
      break;
   case  A_MLINE:
      Check_0_Layer();

      fprintf(flog, "Multi line MLINE entity not implemented.\n");
      display_error++;
      polycnt = 0;
      break;
   case  A_3DFACE:
      Check_0_Layer();

      Graph_PolyStruct(dxf_level(G->acad_8, prefix), 0, FALSE);
      Graph_Poly(NULL, widthindex0, FALSE,0,1);
      Graph_Vertex(G->acad_10*scale,G->acad_20*scale, 0.0); // p1 coords
      Graph_Vertex(G->acad_11*scale,G->acad_21*scale, 0.0); // p1 coords
      Graph_Vertex(G->acad_12*scale,G->acad_22*scale, 0.0); // p1 coords
      Graph_Vertex(G->acad_13*scale,G->acad_23*scale, 0.0); // p1 coords
      Graph_Vertex(G->acad_10*scale,G->acad_20*scale, 0.0); // p1 coords
      polycnt = 0;
      break;
   case  A_LINE:
      Check_0_Layer();
      Graph_Line(dxf_level(G->acad_8, prefix),
               G->acad_10*scale,G->acad_20*scale,  // p1 coords
               G->acad_11*scale,G->acad_21*scale,0,  // p2 coords, flg
               widthindex0, FALSE); // always zero index
      polycnt = 0;
      break;
   case  A_ELLIPSE:
      {
         Check_0_Layer();

         PT2         c, pt0, pt1;
         C2_CURVE    p;
         DML_LIST    seg;

         c2v_set(0.0, 0.0, pt0);
         c2v_set(G->acad_11*scale, G->acad_21*scale, pt1);

         double      t0 = G->acad_41, t1 = G->acad_42;
         double      angle = ArcTan2(G->acad_21, G->acad_11);

         double      maj_axis_length = c2v_dist(pt0, pt1);
         double      min_axis_length = G->acad_40*maj_axis_length;

         c2v_set(G->acad_10*scale, G->acad_20*scale, c);

         // here is an error in c2d_ellipse_to_pcurve.
         // It crashes it t0 > t1
         // and it makes a mistake if t0 < 0 - need to wait for fix from BuildingBlock.
         if ( t0 > TWO_PI )
            t0 -= TWO_PI;

         if ( t1 > TWO_PI )
            t1 -= TWO_PI;

         if ( t0 < 0.0 )
            t0 += TWO_PI;

         if ( t1 < 0.0 )
            t1 += TWO_PI;

         if ( t1 > t0 )
         {
            p = c2d_ellipse_to_pcurve ( c, maj_axis_length, min_axis_length, angle, t0, t1 ) ;
            seg = c2d_pcurve_smash( p , NULL);
      
            PT2   center;
            REAL  radius, st_angle, sweep;
            INT   dir;
            BOOLEAN  is_arc;
            DML_ITEM item;
            C2_CURVE curve;

            // walk down seg to examine each segment
            DataStruct *d = Graph_PolyStruct(dxf_level(G->acad_8, prefix), 0, FALSE);
   #ifdef _DEBUG
            CString tmp;
            tmp.Format("%x", acad_handle);
            doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
   #endif
            Graph_Poly(NULL, widthindex0, FALSE,0,1);
      
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

                  c2c_get_arc_data ( curve, center, &radius,
                     &st_angle, &sweep, &dir );
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
            c2d_free_curve( p );
            polycnt = 0;
         }
         else
         {
            // spanns 4 quadrants
            p = c2d_ellipse_to_pcurve ( c, maj_axis_length, min_axis_length, angle, t0, TWO_PI ) ;
            seg = c2d_pcurve_smash( p , NULL);
      
            PT2   center;
            REAL  radius, st_angle, sweep;
            INT   dir;
            BOOLEAN  is_arc;
            DML_ITEM item;
            C2_CURVE curve;

            // walk down seg to examine each segment
            DataStruct *d = Graph_PolyStruct(dxf_level(G->acad_8, prefix), 0, FALSE);
   #ifdef _DEBUG
            CString tmp;
            tmp.Format("%x", acad_handle);
            doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
   #endif

            Graph_Poly(NULL, widthindex0, FALSE,0,1);
      
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

                  c2c_get_arc_data ( curve, center, &radius,
                     &st_angle, &sweep, &dir );
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


            // next one
            dml_destroy_list( seg, (PF_ACTION)c2d_free_curve);
            c2d_free_curve( p );
            polycnt = 0;

            p = c2d_ellipse_to_pcurve ( c, maj_axis_length, min_axis_length, angle, 0.0, t1 ) ;
            seg = c2d_pcurve_smash( p , NULL);
   /*    
            PT2   center;
            REAL  radius, st_angle, sweep;
            INT   dir;
            BOOLEAN  is_arc;
            DML_ITEM item;
            C2_CURVE curve;
   */ 
            // walk down seg to examine each segment
            d = Graph_PolyStruct(dxf_level(G->acad_8, prefix), 0, FALSE);
   #ifdef _DEBUG
            tmp.Format("%x", acad_handle);
            doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
   #endif

            Graph_Poly(NULL, widthindex0, FALSE,0,1);
      
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

                  c2c_get_arc_data ( curve, center, &radius,
                     &st_angle, &sweep, &dir );
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
            c2d_free_curve( p );
            polycnt = 0;
         }
      }
      break;
   case  A_LWPOLYLINE:
      {
         // if 40 and 41 not ser, use 43
         for (i=0;i<polycnt;i++)
         {
            DxfPoly p;
            p = dxfpolyarray.ElementAt(i);  
            if (p.w == 0)     p.w = G->acad_43;
            if (p.w41 == 0)   p.w41 = G->acad_43;
            dxfpolyarray.SetAt(i, p);
         }
         closed = (G->acad_70 & 1);
         clean_poly();
         do_polyload(closed, scale, G->acad_8);
      }
      break;
   case  A_HATCH:
      {
         int   pcnt = 1; // polycnt 0 is the hatch elevation point - do not use it.
         // _91 is the number of boundery loops
         // polyarray[].acad_93 is the number of vertex in a boundery loop.
         int   i, ii, fill = 0, voidp = 0; 
         int   layerindex = dxf_level(G->acad_8, prefix);

         //if (G->acad_70) this is pattern or solid fill. We only support solid fill.
         //fill = TRUE; <== the elements do not seemed to be orders from Outside Boundary and inside Voids.

         // hatch koos are 10/20 and 11/21, but we can skip the 21 part, because it must be always closed.

         DataStruct *d = Graph_PolyStruct(layerindex, 0L, FALSE);
   #ifdef _DEBUG
         CString tmp;
         tmp.Format("%x", acad_handle);
         doc->SetUnknownAttrib(&d->getAttributesRef(),"HANDLE", tmp, SA_OVERWRITE, NULL); // x, y, rot, height
   #endif

         for (i=0;i<G->acad_91;i++)
         {
            Graph_Poly(NULL, widthindex0, fill, voidp, 1);  // fill, voidpoly, closed
            //voidp = 1; <== the elements do not seemed to be orders from Outside Boundary and inside Voids.
            DxfPoly p, p0;
            p0 = dxfpolyarray.ElementAt(pcnt);  

            int i_93 = p0.acad_93;
            if (p0.acad_72 == 1) // line
            {
               for (ii=0;ii<i_93; ii++)
               {
                  p = dxfpolyarray.ElementAt(pcnt+ii);  
                  Graph_Vertex(p.x*scale, p.y*scale, p.bulge);   
               }
               // check if it was closed
               if (p0.x != p.x || p0.y != p.y)
               {
                  Graph_Vertex(p0.x*scale, p0.y*scale, 0.0);   // close without bulge
               }
            }
            else
            if (p0.acad_72 == 2) // circle
            {
               for (ii=0;ii<i_93; ii++)
               {
                  p = dxfpolyarray.ElementAt(pcnt+ii);  
                  // xy and 40 for radius.
                  Graph_Vertex((p.x-p.w)*scale, p.y*scale, 1);   
                  Graph_Vertex((p.x+p.w)*scale, p.y*scale, 1);   
                  Graph_Vertex((p.x-p.w)*scale, p.y*scale, 0);   
               }
            }
            else
            if (p0.acad_72 == 0) // seed points ????
            {
               // ignored.
               for (ii=0;ii<i_93; ii++)
               {
                  p = dxfpolyarray.ElementAt(pcnt+ii);  
                  Graph_Point(p.x*scale,p.y*scale, layerindex, 0L, 0);
               }
            }
            else
            {
   #ifdef _DEBUG
               fprintf(flog, "EdgeType 72 = %d not supported in HATCH at %ld\n", p0.acad_72, G->acad_linecnt);
               display_error++;
   #endif
            }
            pcnt +=i_93;
         }
         polycnt = 0;
      }
      break;
   case  A_LEADER:
      {
         Check_0_Layer();

         for (i=0;i<polycnt;i++)
         {
            DxfPoly p;
            p = dxfpolyarray.ElementAt(i);  
            p.w = 0;
            p.w41 = 0;
            dxfpolyarray.SetAt(i, p);
         }
         closed= (G->acad_70 & 1);
         clean_poly();

         if (G->acad_71)
         {
            // arrowhead is true.
         }

         do_polyload(closed, scale, G->acad_8);
      }
      break;
   case  A_SOLID:
      {
         Check_0_Layer();

         if (G->acad_230 < EXTRUSION)
         {
            G->acad_10 = -G->acad_10;
            G->acad_11 = -G->acad_11;
            G->acad_12 = -G->acad_12;
            G->acad_13 = -G->acad_13;
         }
         int layer = dxf_level(G->acad_8, prefix);

         Graph_PolyStruct(layer, 0, FALSE);
         Graph_Poly(NULL, widthindex0, TRUE,0,1);
         Graph_Vertex(G->acad_10*scale,G->acad_20*scale, 0.0);  // p1 coords
         Graph_Vertex(G->acad_11*scale,G->acad_21*scale, 0.0); // p1 coords
         Graph_Vertex(G->acad_13*scale,G->acad_23*scale, 0.0);  // p1 coords
         Graph_Vertex(G->acad_12*scale,G->acad_22*scale, 0.0);  // p1 coords
         Graph_Vertex(G->acad_10*scale,G->acad_20*scale, 0.0);  // p1 coords
      }
      break;
   case  A_TRACE:
      {
         Check_0_Layer();

         int layer = dxf_level(G->acad_8, prefix);
         Graph_PolyStruct(layer, 0, FALSE);
         Graph_Poly(NULL, widthindex0, TRUE,0,1);
         Graph_Vertex(G->acad_10*scale,G->acad_20*scale, 0.0); // p1 coords
         Graph_Vertex(G->acad_11*scale,G->acad_21*scale, 0.0); // p1 coords
         Graph_Vertex(G->acad_13*scale,G->acad_23*scale, 0.0); // p1 coords
         Graph_Vertex(G->acad_12*scale,G->acad_22*scale, 0.0); // p1 coords
         Graph_Vertex(G->acad_10*scale,G->acad_20*scale, 0.0); // p1 coords
      }
      break;
   case  A_INSERT:
      {
         int   mirror = FALSE;

         if (G->acad_43 < 0)  // if -1 in Z axis, the X gets mirrored.
         {
            G->acad_10 *= -1;
         }
         if (G->acad_41 < 0)
            mirror = TRUE;

         if (G->acad_42 < 0)
         {
            if (!mirror)
               mirror = TRUE;
            else
               mirror = FALSE;
            G->acad_50 += 180;
            if (G->acad_50 > 360) G->acad_50 = 360 - G->acad_50;
         }

         if (G->acad_66 == 2)
            insert_flag = 1;
         else
            insert_flag = 0;
         // here make sure that block is defined, because ACAD allows the insert of a block before definition
         Graph_Block_On(GBO_APPEND,G->acad_2,cur_filenum,0);   
         Graph_Block_Off();
         Graph_Block_Reference(G->acad_2, NULL,  cur_filenum,G->acad_10*scale, G->acad_20*scale,
                                    DegToRad(G->acad_50),mirror, fabs(G->acad_41),
                                    dxf_level(G->acad_8, prefix), FALSE);

         // check for uneven x,y scale factor
         if (fabs(G->acad_41) != fabs(G->acad_42))
         {
            fprintf(flog,"INSERT of Block [%s] has uneven X (%lf) and Y (%lf) scale factor -> not allowed\n",
               G->acad_2, G->acad_41, G->acad_42);
            display_error++;
         }
      }
      break;
   case  A_DIMENSION:
      {
         double   x , y;

         x = G->acad_12;   // this is the insertion point odf the dimension.
         y = G->acad_22; 

         Graph_Block_On(GBO_APPEND,G->acad_2,cur_filenum,0);   // acad allows the reference before definition
         Graph_Block_Off();
         Graph_Block_Reference(G->acad_2, NULL, cur_filenum, x*scale, y*scale, 0.0, 0, 1.0,
                                 dxf_level(G->acad_8, prefix), FALSE);
      }
      break;
   default:
      {
   // unknown DXF record ????
      }
      break;
   }

   G->acad_6  = 0;
   G->acad_39 = 0.0; // thickness
   G->acad_42 = 0.0;
   G->acad_50 = 0.0; // rotation
   G->acad_66 = 0;
   G->acad_70 = 0;      // flags for polyline
   G->acad_51 = 0; // text oblique angle
   G->acad_71 = 0; // text mirror flag
   G->acad_72 = 0;   // flags for text
   G->acad_73 = 0;   // flags for text
   G->acad_0_type = 0;
   G->acad_38 = 0;
   G->acad_210= 0;
   G->acad_220= 0;
   G->acad_230= 0;
   G->acad_12 = 0.0;
   G->acad_22 = 0.0;

   return(0);
}

/*******************************************************/
// function converts line_number into string and add it to error message
void get_error_line(long /*line_number*/)
{
   STRSET(buffer,'\0');
   STRSET(message,'\0');
   strcpy(message,"DXF Read error at line ");
   sprintf(buffer,"%ld",G->acad_linecnt);
   strcat(message,buffer);
}

/*-----------------------------------------------------*/
static int get_acad(FILE *fp)
{
   char  line[MAX_LINE];

   if (xfgets(line,MAX_LINE,fp) == NULL)
   {
      get_error_line(G->acad_linecnt);
      ErrorMessage(message);
      return EOF;
   }

   if (strlen(line) == 0)
   {
      /*get_error_line(G->acad_linecnt);
      ErrorMessage(message);
      return EOF;*/
      return 0;
   }

   G->acad_nr = atoi(line);

   if (xfgets(line,MAX_LINE,fp) == NULL)
   {
      // printf("Error at %ld\n",G->acad_linecnt);
      get_error_line(G->acad_linecnt);
      ErrorMessage(message);
      return EOF;
   }
   strcpy(G->acad_line,line);

/* this is allowed - empty contense
   if (strlen(line) == 0)
   {
      get_error_line(G->acad_linecnt);
      ErrorMessage(message);
      return EOF;
   }
*/
   return(1);
}

/****************************************************************************/
/*
*/
char *dxf_text(char *s)
{
   static   unsigned char  tmp[5000];
   unsigned int   i;
   int      tcnt = 0;

   if (s == NULL) return "";

   for (i=0;i<strlen(s);i++)
   {
      if (s[i] == '%' && s[i+1] == '%')
      {
         if (s[i+2] == 'd')   // degree
         {
            tmp[tcnt++] = CHAR_DEGREE;
         }
         else
         if (s[i+2] == '%')   // percent
         {
            tmp[tcnt++] = '%';
         }
         else
         if (s[i+2] == 'o')   // overbar
         {
            tmp[tcnt++] = '%';
            tmp[tcnt++] = 'O';
         }
         else
         if (s[i+2] == 'u')   // underbar
         {
            tmp[tcnt++] = '%';
            tmp[tcnt++] = 'U';
         }
         else
         if (s[i+2] == 'c')   // diameter
         {
            tmp[tcnt++] = CHAR_DIAMETER;
         }
         else
         if (s[i+2] == 'p')   // plus minus
         {
            tmp[tcnt++] = CHAR_PLUSMINUS;
         }
         i = i+2;
      }  
      else
         tmp[tcnt++] = s[i];
   }
   tmp[tcnt] = '\0';
   return (char *)tmp;
}

/****************************************************************************/
/*
*/
static int get_acadtype(char *t)
{
   if (!strcmp(t,"SECTION"))     return(A_SECTION);
   if (!strcmp(t,"HEADER"))      return(A_HEADER);
   if (!strcmp(t,"LAYER"))       return(A_LAYER);
   if (!strcmp(t,"ENTITIES"))    return(A_ENTITIES);
   if (!strcmp(t,"ENDSEC"))      return(A_ENDSEC);
   if (!strcmp(t,"CIRCLE"))      return(A_CIRCLE);
   if (!strcmp(t,"POINT"))       return(A_POINT);
   if (!strcmp(t,"EOF"))         return(A_EOF);
   if (!strcmp(t,"POLYLINE"))    return(A_POLYLINE);
   if (!strcmp(t,"VERTEX"))      return(A_VERTEX);
   if (!strcmp(t,"SEQEND"))      return(A_SEQEND);
   if (!strcmp(t,"ARC"))         return(A_ARC);
   if (!strcmp(t,"TEXT"))        return(A_TEXT);
   if (!strcmp(t,"MTEXT"))       return(A_MTEXT);
   if (!strcmp(t,"TRACE"))       return(A_TRACE);
   if (!strcmp(t,"LINE"))        return(A_LINE);
   if (!strcmp(t,"MLINE"))       return(A_MLINE);  
   if (!strcmp(t,"3DFACE"))      return(A_3DFACE);
   if (!strcmp(t,"SOLID"))       return(A_SOLID);
   if (!strcmp(t,"HATCH"))       return(A_HATCH);
   if (!strcmp(t,"ELLIPSE"))     return(A_ELLIPSE);
   if (!strcmp(t,"INSERT"))      return(A_INSERT);
   if (!strcmp(t,"ATTRIB"))      return(A_ATTRIB);
   if (!strcmp(t,"BLOCK"))       return(A_BLOCK);
   if (!strcmp(t,"DIMENSION"))   return(A_DIMENSION);
   if (!strcmp(t,"ENDBLK"))      return(A_ENDBLK);
   if (!strcmp(t,"LWPOLYLINE"))  return(A_LWPOLYLINE);
   if (!strcmp(t,"LEADER"))      return(A_LEADER);
   if (!strcmp(t,"ATTDEF"))      return(A_ATTDEF);

   return(A_UNKNOWN);
}

// end DXFIN.CPP
////////////////////////////////////////////////////////////////////////////////////////
