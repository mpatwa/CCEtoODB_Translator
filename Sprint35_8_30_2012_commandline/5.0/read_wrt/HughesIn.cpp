// $Header: /CAMCAD/5.0/read_wrt/HughesIn.cpp 16    3/12/07 12:53p Kurt Van Ness $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

// all units are translated into mil

#include "stdafx.h"
#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include <math.h>
#include <LIMITS.H>
#include "gauge.h"
#include "attrib.h"
#include "pcbutil.h"
#include "RwUiLib.h"
extern CProgressDlg     *progress;

#define  HUGHESERR      "hughes.log"
#define  SILK_TOP       "SILKSCREEN_1"
#define  SILK_BOT       "SILKSCREEN_2"
#define  PAD_TOP        "PAD_1"
#define  PAD_BOT        "PAD_2"
#define  PROBE_TOP      "PROBE_1"
#define  PROBE_BOT      "PROBE_2"

#define  MAX_LINE       255
#define  MAX_POLY       2000
    
/****************************************************************************/
/*
*/

typedef struct
{
   double   x,y;
   int      fill;    // 0 not
                     // 1 yes
} Poly;

/****************************************************************************/
/*
   Global Commands
*/

static   Poly        *polyline;
static   int         polycnt;

static   CCEtoODBDoc  *doc;

static   FILE        *ifp;
static   char        ifp_line[MAX_LINE];
static   long        ifp_linecnt = 0;

static   int         cur_layer;
static   int         cur_entity;

static   double      scale_factor = 1;
static   int         cur_filenum = 0;

static   FILE        *ferr;
static   int         display_error = 0;
static   CString     cur_filename;

static   double      refnameheight, refnamewidth;

/****************************************************************************/
/*
*/
static int  write_poly(int layer, double width,int fill)
{
    int i;
    int widthindex = 0;
    int err;

    if (polycnt == 0)   return 0;

    if (width == 0)
        widthindex = 0; // make it small width.
    else
        widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

    if (polycnt > 2)
    {
      Graph_PolyStruct(layer, 0, 0);
      int close = (polyline[0].x == polyline[polycnt-1].x && polyline[0].y == polyline[polycnt-1].y);
      Graph_Poly(NULL, widthindex, fill, 0, close);
      // here write lines.
      for (i=0;i<polycnt;i++)
         Graph_Vertex(polyline[i].x, polyline[i].y,0.0);
    }
    else
         Graph_Line(layer,
                  polyline[0].x, polyline[0].y,
                  polyline[1].x, polyline[1].y,  
                  0L, widthindex , FALSE); // index of width table

    polycnt = 0;
    return 0;
}

/****************************************************************************/
/*
   return inch
*/
static double   cnv_tok(char *l)
{
    double x;

    if (l && strlen(l))
    {
        x = atof(l);
    }
    else
        return 0.0;

    return x * scale_factor;  // makes it into page units
}

/****************************************************************************/
/*
*/
static char *get_nextline(char *string,int n,FILE *fp)
{
   char  *res;

   ifp_linecnt++;
   res = fgets(string,n,fp);

   if (strlen(string) && string[strlen(string)-1] == '\n')
      string[strlen(string)-1] = '\0';

   return res;
}

/******************************************************************************
* ReadHughes
*/
void ReadHughes(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt)
{
   FileStruct  *file;
   int         widthindex = 0;

   scale_factor = Units_Factor(UNIT_MILS, pageunits); // just to initialize

   doc = Doc;

   if ((polyline = (Poly  *)calloc(MAX_POLY,sizeof(Poly))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer,"rt")) == NULL)
   {
      ErrorMessage("Error open file", path_buffer);
      return;
   }
   ifp_linecnt = 0;

   if (cur_filecnt > 0)
   {
      if ((ferr = fopen(GetLogfilePath(HUGHESERR),"a+t")) == NULL)
      {
         CString  t;
         t.Format("Error open [%s] file", HUGHESERR);
         ErrorMessage(t, "Error");
         return;
      }
   }
   else
   {
      if ((ferr = fopen(GetLogfilePath(HUGHESERR),"wt")) == NULL)
      {
         CString  t;
         t.Format("Error open [%s] file", HUGHESERR);
         ErrorMessage(t, "Error");
         return;
      }
      display_error = 0;
   }

   fprintf(ferr,"File : [%s]\n",path_buffer);

   // get filename
   get_nextline(ifp_line, MAX_LINE,ifp);

   // here find a file->getBlockType() which is marked as the BLOCKTYPE_PCB
   // the return fill be a NULL file, because this only updates
   // an existing file and does not create a new one.
   cur_filenum = 0;
   // write file blocks as exploded

/**** Always make a seperate file ****/
#ifdef MERGE_FILE
   POSITION pos;

   pos = doc->FileList.GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->FileList.GetNext(pos);
      // check if a version of this file is already loaded.
      if (file->getName().CompareNoCase(ifp_line) == 0)
      {
         cur_filenum = file->getFileNumber();
         doc->PrepareAddEntity(file);
         break;
      }
   }

   if (cur_filenum == 0)
   {
      file = Graph_File_Start(ifp_line);
      file->setBlockType(blockTypePcb);
      file->getBlock()->blocktype = file->getBlockType();
      cur_filenum = file->getFileNumber();      
   }
#else
      file = Graph_File_Start(path_buffer, Type_HUGHES);
      file->setBlockType(blockTypePcb);
      file->getBlock()->setBlockType(file->getBlockType());
      cur_filenum = file->getFileNumber();      
#endif
   // here do a while loop
   char  token[80];
   CString  cur_shapename;
   BlockStruct *cur_block = NULL;
   int      already_exist = FALSE;
   double   center_x, center_y;
   int      err;
   int      zerowidth = Graph_Aperture("", T_ROUND, 0.0, 0.0,0.0, 0.0, 0.0,0,BL_WIDTH,FALSE, &err);
   int      mirror;
   int      outline_continue = FALSE;
   CString  last_otype = "";  // last outline type E or I
   CPnt     *lastvertex;
   CString  padname = "ROUND_35";
   CString  probename = "ROUND_25";
   int      pincnt;

   Graph_Aperture(padname, T_ROUND, 35*scale_factor, 0.0 , 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
   Graph_Aperture(probename, T_ROUND, 25*scale_factor, 0.0 , 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

   int   lptr;
   LayerStruct *l;

   lptr = Graph_Level(PROBE_TOP,"", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_TOP);

   lptr = Graph_Level(PROBE_BOT,"", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   Graph_Level_Mirror(PROBE_TOP, PROBE_BOT, "");

   lptr = Graph_Level(SILK_TOP,"", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_SILK_TOP);

   lptr = Graph_Level(SILK_BOT,"", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_SILK_BOTTOM);

   Graph_Level_Mirror(SILK_TOP, SILK_BOT, "");

   lptr = Graph_Level(PAD_TOP,"", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_TOP);

   lptr = Graph_Level(PAD_BOT,"", 0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   Graph_Level_Mirror(PAD_TOP, PAD_BOT, "");

   lptr = Graph_Level("BOARDOUTLINE","",0);
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_BOARD_OUTLINE);


   refnameheight = 75 * scale_factor;
   refnamewidth  = 50 * scale_factor;

   if (Format->HughesFirstFileTop)
   {
      // first file is top, second file must be bottom
      if (cur_filecnt == 0)
         mirror = FALSE;
      else
         mirror = TRUE;
   }
   else
   {
      // first file is mirror, sencond must be top
      if (cur_filecnt == 0)
         mirror = TRUE;
      else
         mirror = FALSE;
   }

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      strncpy(token,&ifp_line[0],1);
      token[1] = '\0';
      switch (token[0])
      {
         case 'C':
            // remark line
            last_otype = "";  // needed to know when a new vertex starts.
         break;
         case 'L':
         {
            // library shape
            CString  tmp;

            // get shape name
            strncpy(token,&ifp_line[5],14);
            token[14] = '\0';
            tmp = token;
            tmp.TrimLeft();
            tmp.TrimRight();
            if (strlen(tmp))
            {
               cur_shapename = tmp;
               // find out if shapename already exist
               if (Graph_Block_Exists(doc, cur_shapename, -1))
               {
                  already_exist = TRUE;
               }
               else
               {
                  double   body_x, body_y;
                  pincnt = 0;                
                  already_exist = FALSE;
                  cur_block = Graph_Block_On(GBO_APPEND,cur_shapename,-1,0);
                  cur_block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
                  Graph_Block_Off();

                  // centerpoint
                  strncpy(token,&ifp_line[93],10);
                  token[10] = '\0';
                  center_x = cnv_tok(token);
                  strncpy(token,&ifp_line[104],10);
                  token[10] = '\0';
                  center_y = cnv_tok(token);

                  // bodysize
                  strncpy(token,&ifp_line[73],10);
                  token[10] = '\0';
                  body_x = cnv_tok(token);
                  strncpy(token,&ifp_line[83],10);
                  token[10] = '\0';
                  body_y = cnv_tok(token);

                  doc->SetVisAttrib(&cur_block->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE),
                     VT_STRING,
                     "^$ref",
                     center_x, center_y, 0.0, refnameheight, refnamewidth, 0 ,0 , FALSE, SA_APPEND,  0L, 
                     Graph_Level(SILK_TOP,"",0),0, 0, 0);

                  // draw body around component
                  Graph_Block_On(GBO_APPEND,cur_shapename,-1,0);
                  DataStruct *d = Graph_PolyStruct(Graph_Level(SILK_TOP,"",0), 0, FALSE);
                  d->setGraphicClass(GR_CLASS_COMPOUTLINE);
                  Graph_Poly(NULL, zerowidth, 0,0,1);
                  Graph_Vertex(center_x - body_x/2, center_y - body_y/2, 0.0);
                  Graph_Vertex(center_x + body_x/2, center_y - body_y/2, 0.0);
                  Graph_Vertex(center_x + body_x/2, center_y + body_y/2, 0.0);
                  Graph_Vertex(center_x - body_x/2, center_y + body_y/2, 0.0);
                  Graph_Vertex(center_x - body_x/2, center_y - body_y/2, 0.0);
                  Graph_Block_Off();
               }
            }

            if (!already_exist)
            {
               // testprobe
               // pinpos
               double   pinx, piny;
               strncpy(token,&ifp_line[51],10);
               token[10] = '\0';
               pinx = cnv_tok(token);
               strncpy(token,&ifp_line[62],10);
               token[10] = '\0';
               piny = cnv_tok(token);

               double   probex, probey;
               strncpy(token,&ifp_line[19],10);
               token[10] = '\0';
               probex = cnv_tok(token);
               strncpy(token,&ifp_line[31],10);
               token[10] = '\0';
               probey = cnv_tok(token);
               Graph_Block_On(GBO_APPEND,cur_shapename,-1,0);

               CString  pinname;
               pinname.Format("%d",++pincnt);
               DataStruct *d = Graph_Block_Reference(padname, pinname, cur_filenum, 
                  pinx, piny, 0.0, 0, 1.0, Graph_Level(PAD_TOP,"",0), TRUE);
               d->getInsert()->setInsertType(insertTypePin);

               CString  pname;
               pname = "PROBE_";
               pname += pinname;
               Graph_Block_Reference(probename, pname, cur_filenum, 
                  probex, probey, 0.0, 0, 1.0, Graph_Level(PROBE_TOP,"",0), TRUE);

               Graph_Line(Graph_Level(PROBE_TOP,"",0), pinx, piny, probex, probey, 0, 0, 0);

               Graph_Block_Off();
            }
         }
         break;
         case 'P':
         {
            // placement
            CString  tmp;
            CString  shapename, partname, refname;
            double   x,y;
            int      rotation = 0;

            // get shape name
            strncpy(token,&ifp_line[5],14);
            token[14] = '\0';
            shapename = token;
            shapename.TrimLeft();
            shapename.TrimRight();

            // get ref name
            strncpy(token,&ifp_line[39],14);
            token[14] = '\0';
            refname = token;
            refname.TrimLeft();
            refname.TrimRight();

            // get part name
            strncpy(token,&ifp_line[19],20);
            token[20] = '\0';
            partname = token;
            partname.TrimLeft();
            partname.TrimRight();

            // place
            strncpy(token,&ifp_line[60],10);
            token[10] = '\0';
            x = cnv_tok(token);
            strncpy(token,&ifp_line[71],10);
            token[10] = '\0';
            y = cnv_tok(token);

            // rotation
            strncpy(token,&ifp_line[53],7);
            token[7] = '\0';
            rotation = atoi(token) * 90;

            if (mirror)
            {
               rotation = 360 - rotation;
               if (rotation < 0) rotation += 360;
               rotation = rotation % 360;
               x = -x;
            }

            DataStruct *d = Graph_Block_Reference(shapename, refname, 
               0, x, y, DegToRad(rotation), mirror , 1.0, 0, TRUE);
            d->getInsert()->setInsertType(insertTypePcbComponent);

            doc->SetVisAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE),
                  VT_STRING,
                  refname.GetBuffer(0),
                  0.0, 0.0, 0.0, refnameheight, refnamewidth, 0 ,0 , TRUE, SA_OVERWRITE,  0L, 
                  Graph_Level(SILK_TOP,"",0),0, 0, 0);

            // partname is not unique !
            if (strlen(partname))
            {
               doc->SetUnknownAttrib(&d->getAttributesRef(),"PARTNAME", partname, SA_OVERWRITE, NULL); //  
            }
         }
         break;         
         case 'O':
         {
            // outline
            CString  ctype, otype;
            double   xs,ys, xe, ye, bulge;
            int      circle = FALSE;

            // otype;
            strncpy(token,&ifp_line[3],5);
            token[5] = '\0';
            otype = token;
            otype.TrimLeft();
            otype.TrimRight();

            // ctype
            strncpy(token,&ifp_line[9],5);
            token[5] = '\0';
            ctype = token;
            ctype.TrimLeft();
            ctype.TrimRight();

            if (ctype.Compare("L") == 0)
            {
               // line 
               strncpy(token,&ifp_line[16],9);
               token[9] = '\0';
               xs = cnv_tok(token);
               strncpy(token,&ifp_line[25],9);
               token[9] = '\0';
               ys = cnv_tok(token);
               strncpy(token,&ifp_line[34],9);
               token[9] = '\0';
               xe = cnv_tok(token);
               strncpy(token,&ifp_line[43],9);
               token[9] = '\0';
               ye = cnv_tok(token);
               bulge = 0.0;

               if (mirror)
               {
                  xs = -xs;
                  xe = -xe;
               }

            }
            else
            if (ctype.Compare("A") == 0)
            {
               double startangle, endangle, centerx, centery;

               // arc
               strncpy(token,&ifp_line[16],9);
               token[9] = '\0';
               xs = cnv_tok(token);
               strncpy(token,&ifp_line[25],9);
               token[9] = '\0';
               ys = cnv_tok(token);
               strncpy(token,&ifp_line[34],9);
               token[9] = '\0';
               xe = cnv_tok(token);
               strncpy(token,&ifp_line[43],9);
               token[9] = '\0';
               ye = cnv_tok(token);

               // angles here
               strncpy(token,&ifp_line[52],9);
               token[9] = '\0';
               startangle = atof(token);
               strncpy(token,&ifp_line[62],9);
               token[9] = '\0';
               endangle = atof(token);

               strncpy(token,&ifp_line[72],9);
               token[9] = '\0';
               centerx = cnv_tok(token);
               strncpy(token,&ifp_line[83],9);
               token[9] = '\0';
               centery = cnv_tok(token);

               if (mirror)
               {
                  xs = -xs;
                  xe = -xe;
                  centerx = -centerx;
               }

               if (startangle == endangle)
               {
                  // this is a circle
                  circle = TRUE;
                  last_otype = "";
                  // start and end is the same
                  if (centerx != xs)
                     xe = xs + (centerx-xs)*2;
                  else
                     ye = ys + (centery-ys)*2;
               }
               else
               {
                  double   r, sa, da;
                  ArcCenter2( xs, ys, xe, ye, centerx, centery, &r, &sa, &da,0);

                  if (da > PI)
                     da = da - PI2;

                  bulge = tan(da/4);
/*
                  //if (startangle < 0)   startangle +=360;
                  double sa = startangle, ea = endangle;
                  //if (endangle < 0)     endangle +=360;
                  //if (startangle < 0)   startangle +=360;
                  if (startangle > endangle) 
                     bulge = tan(DegToRad(endangle - startangle)/4);
                  else
                     bulge = tan(DegToRad(360 - (startangle-endangle))/4);
*/
               }
            }
            else
            {
               fprintf(ferr,"Unknown Outline CTYPE [%s] Line [%s] at %ld\n", ctype, ifp_line, ifp_linecnt);
               display_error++;
               outline_continue = FALSE;
               break;
            }

            if (!outline_continue)
            {
               DataStruct *d = Graph_PolyStruct(Graph_Level("BOARDOUTLINE","",0) , 0, 0);
               d->setGraphicClass(GR_CLASS_BOARDOUTLINE);
               outline_continue = TRUE;
            }

            // Vertexes have a rounding error. but 
            // if (xs != oldx || ys != oldy)
            //   last_otype = "";
   
            if (last_otype != otype)
            {
               int   voidpoly;
               if (otype.Compare("E") == 0)
                  voidpoly = FALSE;
               else
               if (otype.Compare("I") == 0)
                  voidpoly = TRUE;
               else
               {
                  fprintf(ferr,"Unknown OTYPE [%s] Line [%s] at %ld\n", otype, ifp_line, ifp_linecnt);
                  display_error++;
                  break;
               }
               Graph_Poly(NULL, zerowidth, 0, voidpoly, TRUE);
               if (circle)
               {
                  lastvertex = Graph_Vertex( xs, ys, 1.0);
                  lastvertex = Graph_Vertex( xe, ye, 1.0);
                  lastvertex = Graph_Vertex( xs, ys, 0.0);
                  last_otype = "";
                  break;
               }
               else
               {
                  lastvertex = Graph_Vertex( xs, ys, 0.0);
                  last_otype = otype;
               }
            }           

            lastvertex->bulge = (DbUnit)bulge;
            lastvertex = Graph_Vertex( xe, ye, 0.0);
         }
         break;
         default:
            fprintf(ferr,"Unknown Column 1 [%s] Line [%s] at %ld\n", token, ifp_line, ifp_linecnt);
            display_error++;
         break;
      }

   }

   fclose(ifp);

   fprintf(ferr,"End : [%s]\n\n",path_buffer);
   fclose(ferr); 

   free(polyline);

   generate_PINLOC(doc,file,1);  // this function generates the PINLOC argument for all pins.
   
   file = NULL;

   if (display_error && cur_filecnt == (tot_filecnt-1))
      Logreader(GetLogfilePath(HUGHESERR));
   
   return;
}

// end HUGHES1IN.CPP
