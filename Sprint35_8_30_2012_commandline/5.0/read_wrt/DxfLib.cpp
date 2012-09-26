// $Header: /CAMCAD/5.0/read_wrt/DxfLib.cpp 16    3/22/07 12:56a Kurt Van Ness $

/****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

/****************************************************************************/
/*
   All function return 1 for SUCCESS and 0 for ERROR

   Pen roundings
   All Angles are in Radians.

*/
/****************************************************************************/

#include "stdafx.h"
#include "string.h"
#include "dbutil.h"
#include <math.h>
#include "dxflib.h"
#include "font.h"
#include "Graph.h"
#include "polylib.h"
#include "apertur2.h"


/* Constant Definition *********************************************************/

#define   MAX_LAYER  255
#define   MAX_BLOCK  1000
#define   MAX_POLY   10000

/* Array zum merken der Linienzuege*/
#define ARMINI  0     /* Min Wert */
#define ARSIZE  400   /* Max Wert */


/* Type Definition *********************************************************/

typedef struct
{
   char  *name;                 
} Block_List;

typedef struct
{
   double   ymax;
   double   ymin;
   double   xa;
   double   dx;
} scr_ply_t;


/* Static Variables *********************************************************/

static FILE				*Dxf_Main_File;
static FILE				*Dxf_Block_File;
static FILE    		*Dxf_Entity_File;
static int     		blockmode;
static int     		Dxf_Check_Block = 0;
static char    		Dxf_Level[80];			//current layer
static double  		Dxf_Linewidth;
static int     		dxflayers;
static char    		**dxflay;
static int     		lineend = 0;			//0 = none, 1 = round, 2 = square
static int     		dxf_polyclosed;
static int     		handle;					// record 5
static long    		cur_handle;
static int     		nachkomma = 3;
static double  		realwidth = 0.0001;
static scr_ply_t		*sply;
static bool          text_xscale = TRUE;
static bool          text_unmirror = FALSE;

static Block_List		*block_list;
static int				block_list_cnt;

static Point2			*coo_list;
static int				coo_list_cnt;


/* Function Prototypes *********************************************************/

static char    *clean_text(const char *);
static int     Dxf_Layer(FILE *, const char *);
static int     Dxf_Koo(FILE *, int, double, double);
static int     Dxf_Rot50(FILE *, double);
static int     update_dxflay(const char *);
static int     legal_layer(char *l);
static int     in_block_list(const char *n);


/******************************************************************************
* wdxf_Graph_Line_End

   0 = no end
   1 = round
   2 = square
*/
void wdxf_Graph_Line_End( int onoff )
{
   lineend = onoff;
}

/******************************************************************************
* wdxf_Graph_WriteALine
*/
void wdxf_Graph_WriteALine(const char *line )
{
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   fprintf(fp, line);
   return;
}

/******************************************************************************
* wdxf_Graph_Level
*/
void wdxf_Graph_Level(const char *level )
{
   if (level != NULL)
      strcpy(Dxf_Level,level);
   else
      strcpy(Dxf_Level, "NULL");
}

/******************************************************************************
* wdxf_Set_Mirroring_On
*/
void wdxf_Set_Mirroring_On()
{
}

/******************************************************************************
* wdxf_Set_Mirroring_Off
*/
void wdxf_Set_Mirroring_Off()
{
}

/******************************************************************************
* wdxf_Graph_Line_Width
*/
void wdxf_Graph_Line_Width( double width )
{
   Dxf_Linewidth = width;
}

/******************************************************************************
* Dxf_Command
*/
static int Dxf_Command(FILE *fp, const char *c)
{
   if (fprintf(fp, "%s", c) == EOF)
   {
      MessageBox(NULL, "Out of disk space or disk failure", "ERROR", MB_OK | MB_ICONHAND);
      return -1;
   }
   return(1);
}

/******************************************************************************
* Dxf_Handle
*/
static int Dxf_Handle(FILE *fp)
{
   if (!handle)
		return 1;

	char tmp[80];
   sprintf(tmp, " 5\n%X\n", ++cur_handle);
   Dxf_Command(fp, tmp);
   return 1;
}

/******************************************************************************
* Dxf_Width
*/
static int Dxf_Width(FILE *fp, double w)
{
   char tmp[80];
   sprintf(tmp, " 40\n%2.6lf\n", w);
   Dxf_Command(fp, tmp);

   sprintf(tmp, " 41\n%2.6lf\n", w);
   Dxf_Command(fp, tmp);
   return 1;
}  

/******************************************************************************
* Dxf_Poly
*/
static int Dxf_Poly(FILE *fp, char *level, int closed)
{
   Dxf_Command(fp, "  0\nPOLYLINE\n");
   Dxf_Handle(fp);
   Dxf_Layer(fp, level);
   Dxf_Command(fp, " 66\n1\n");
   if (closed)
		Dxf_Command(fp, " 70\n1\n");

   return 1;
}

/******************************************************************************
* Dxf_Arc
*/
static int Dxf_Arc(FILE *fp, char *level, double x, double y, double radius, double sa, double ea)
{
   char  tmp[80];

   Dxf_Command(fp ,"  0\nARC\n");
   Dxf_Handle(fp);
   Dxf_Layer(fp, level);
   Dxf_Koo(fp, 10, x, y);

   sprintf(tmp, " 40\n%2.6lf\n", radius); // radius of thermal
   Dxf_Command(fp, tmp);

   sprintf(tmp, " 50\n%2.6lf\n", sa); // start angle
   Dxf_Command(fp, tmp);

   sprintf(tmp, " 51\n%2.6lf\n", ea); // end angle
   Dxf_Command(fp, tmp);
   return 1;
}

/******************************************************************************
* Dxf_Vertex
*/
static int Dxf_Vertex(FILE *fp,char *l,double x,double y, double bulge)
{
   char tmp[80];
   Dxf_Command(fp, "  0\nVERTEX\n");
   Dxf_Handle(fp);
   Dxf_Layer(fp, l);
   Dxf_Koo(fp, 10, x ,y);
   if (fabs(bulge) > 0)
   {
      sprintf(tmp, " 42\n%2.6lf\n", bulge); // radius of thermal
      Dxf_Command(fp, tmp);
   }
   return(1);
}

/******************************************************************************
* Dxf_Layer
*/
static int Dxf_Layer(FILE *fp, const char *l)
{
   char tmp[80];
   char temp_layer[80];
   int layernr = update_dxflay(l);

   CpyStr(temp_layer, l, 80);
   if (legal_layer(temp_layer))
      sprintf(tmp, "  8\n%s\n", temp_layer);
   else
      sprintf(tmp, "  8\nLAYNR%d\n",layernr);

   Dxf_Command(fp, tmp);
   return(1);
}

/******************************************************************************
* Dxf_Koo
*/
static int Dxf_Koo(FILE *fp, int s, double x, double y)
{
   char tmp[80];
   sprintf(tmp, " %d\n%.*lf\n", s, nachkomma, x);
   Dxf_Command(fp,tmp);

   sprintf(tmp, " %d\n%.*lf\n", s+10, nachkomma, y);
   Dxf_Command(fp, tmp);
   return(1);
}

/******************************************************************************
* Dxf_Seqend
*/
static int Dxf_Seqend(FILE *fp, char *l)
{
   Dxf_Command(fp, "  0\nSEQEND\n");
   Dxf_Layer(fp, l);
   return 1;
}

/******************************************************************************
* wdxf_roundend
*/
int wdxf_roundend(FILE *fp, char *Dxf_Level, double Dxf_Linewidth, double x, double y)
{
   char tmp[80];

   if (Dxf_Linewidth >= realwidth)   // only if the linewidth is greater realwidth
   {
      Dxf_Poly(fp, Dxf_Level, 1);
      Dxf_Width(fp, Dxf_Linewidth/2);
      Dxf_Vertex(fp, Dxf_Level,x-Dxf_Linewidth/4, y, 0.0);

      sprintf(tmp, " 42\n1.0\n");
      Dxf_Command(fp, tmp);
      Dxf_Vertex(fp, Dxf_Level,x+Dxf_Linewidth/4, y, 0.0);

      sprintf(tmp, " 42\n1.0\n");
      Dxf_Command(fp, tmp);
      Dxf_Seqend(fp, Dxf_Level);
   }
   return 0;
}

/******************************************************************************
* wdxf_get_handle
*/
long wdxf_get_handle()
{
   return cur_handle;
}

/******************************************************************************
* wdxf_Graph_Init
*/
int wdxf_Graph_Init(int check_block, int nach_komma, double real_width, int handel, bool xscale, bool unmirrortext)
{
   blockmode = 0;
   cur_handle = 0;
   Dxf_Check_Block = check_block;
   nachkomma = nach_komma; // how many decimal points.
   realwidth = real_width;
   handle = handel;
   dxf_polyclosed = 0;
   text_xscale = xscale;
   text_unmirror = unmirrortext;
   return 1;
}

/******************************************************************************
* wdxf_Graph_File_Open
*/
int wdxf_Graph_File_Open( FILE *dxf_out_file)
{
   Dxf_Main_File = dxf_out_file;
   Dxf_Block_File = fopen("$BLOCK.$$$", "wt");
   if( Dxf_Block_File == NULL )
   {
      MessageBox(NULL, "Can not open file [$BLOCK.$$$]", "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }

   Dxf_Entity_File = fopen( "$ENT.$$$", "wt" );
   if( Dxf_Block_File == NULL )
   {
      MessageBox(NULL, "Can not open file [$ENT.$$$]", "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }

   if ((sply = (scr_ply_t *)calloc(ARSIZE, sizeof(scr_ply_t))) == NULL)
   {
      MessageBox(NULL, "No memory in DXFLIB", "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }

   if ((dxflay = (char **)calloc(MAX_LAYER, sizeof(char *))) == NULL)
   {
      MessageBox(NULL, "No memory in DXFLIB", "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }
   dxflayers = 0;

   if ((block_list = (Block_List *)calloc(MAX_BLOCK, sizeof(Block_List))) == NULL)
   {
      MessageBox(NULL, "No memory in DXFLIB", "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }
   block_list_cnt = 0;

   if ((coo_list = (Point2 *)calloc(MAX_POLY, sizeof(Point2))) == NULL)
   {
      MessageBox(NULL, "No memory in DXFLIB", "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }
   coo_list_cnt = 0;

   return 1;
}

/******************************************************************************
* Write_Header
*/
static int Write_Header(FILE *fp, DXF_Header header)
{
   char tmp[80];
   char tmp1[80];

   /* write header */
   Dxf_Command(fp, "  0\nSECTION\n2\nHEADER\n");

   if(!header.VersionNumber.IsEmpty())
      Dxf_Command(fp, "9\n$ACADVER\n1\n"+ header.VersionNumber + "\n" );

   Dxf_Command(fp, "  0\nENDSEC\n");
   Dxf_Command(fp, "  0\nSECTION\n2\nTABLES\n");

   /* write tables */
   Dxf_Command(fp, "  0\nTABLE\n");
   sprintf(tmp, "  2\nLAYER\n70\n%d\n", dxflayers);
   Dxf_Command(fp, tmp);

   for (int i=0; i<dxflayers; i++)
   {
      if (legal_layer(dxflay[i]))
         sprintf(tmp1, "%s", dxflay[i]);
      else
         sprintf(tmp1, "LAYNR%d", i);

      sprintf(tmp,"  0\nLAYER\n2\n%s\n70\n0\n62\n%d\n6\nCONTINUOUS\n", tmp1, (i % 15)+1);
      Dxf_Command(fp, tmp);
   }

   Dxf_Command(fp, "  0\nENDTAB\n");
   Dxf_Command(fp, "  0\nENDSEC\n");
   return(1);
}

/******************************************************************************
* copy_file
*/
static int copy_file(FILE *fto,char *fname)
{
   FILE *ffrom;
   int c;

   ffrom = fopen(fname, "rt");
   if( ffrom == NULL )
   {
      char buf[80];
      sprintf(buf, "Can not open file [%s]", fname);
      MessageBox(NULL, buf, "ERROR", MB_OK | MB_ICONHAND);
      exit(1);
   }

   while((c = fgetc(ffrom)) != EOF)
   {
      if (fputc(c , fto) == EOF)
      {
         MessageBox(NULL, "Out of disk space or disk failure", "ERROR", MB_OK | MB_ICONHAND);
         exit(1);
      }
   }
   fclose(ffrom);
   return 1;
}

/******************************************************************************
* wdxf_Graph_File_Close
*/
int wdxf_Graph_File_Close(FILE *log, int *display_error_cnt, DXF_Header header)
{
   fclose(Dxf_Block_File);
   fclose(Dxf_Entity_File);
   Write_Header(Dxf_Main_File,header);
   Dxf_Command(Dxf_Main_File, "  0\nSECTION\n2\nBLOCKS\n");

   /* copy blocks */
   copy_file(Dxf_Main_File, "$BLOCK.$$$");
   Dxf_Command(Dxf_Main_File, "  0\nENDSEC\n");
   Dxf_Command(Dxf_Main_File, "  0\nSECTION\n2\nENTITIES\n");

   /* copy blocks */
   copy_file(Dxf_Main_File, "$ENT.$$$");
   Dxf_Command(Dxf_Main_File, "  0\nENDSEC\n");
   Dxf_Command(Dxf_Main_File, "  0\nEOF\n");

   UNLINK("$BLOCK.$$$");
   UNLINK("$ENT.$$$");
   free(sply);

	int i=0;
   for (i=0; i<block_list_cnt; i++)
      free(block_list[i].name);
   free(block_list);
   block_list_cnt = 0;

   free(coo_list);
   coo_list_cnt = 0;

   for (i=0; i<dxflayers; i++)
   {
      char tmp[80];
      strcpy(tmp, dxflay[i]);
      int layernr = update_dxflay(tmp);
      if (!legal_layer(tmp))
      {
         sprintf(tmp, "LAYNR%d", i);
         fprintf(log, "Layername [%s] changed to legal DXF Layername [%s]\n", dxflay[i], tmp);
         *display_error_cnt = (*display_error_cnt) + 1;
      }
      free(dxflay[i]);
   }

   free(dxflay);
   return(1);
}

/*****************************************************************************/
/*
   Graph Hollow Aperture guaranties that a aperture is written as an outline
   Thermal, Complex are ignored,
   Donut is written as round
*/
int wdxf_Graph_Aperture(int type, double x, double y, double sizeA, double sizeB, double rotation, 
                    double xoffset, double yoffset)
{
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   switch (type)
   {
   case T_DONUT:
      {
         double w = sizeA - sizeB; 
         Dxf_Poly(fp, Dxf_Level,0);
         Dxf_Width(fp, w/2);
         Dxf_Vertex(fp, Dxf_Level,x-sizeA/2+(w/4), y, 1);
         Dxf_Vertex(fp, Dxf_Level,x+sizeA/2-(w/4), y, 1);
         Dxf_Vertex(fp, Dxf_Level,x-sizeA/2+(w/4), y, 0);
         Dxf_Seqend(fp, Dxf_Level);
      }
      break;
   case T_ROUND:
      {
         wdxf_roundend(fp, Dxf_Level,sizeA,x+xoffset, y+yoffset);
      }
      break;
   case T_SQUARE:
		sizeB = sizeA; // rectangle where sizeB = sizeA
   case T_RECTANGLE:
      {
         double x0, y0, x1, y1, x2, y2, x3, y3, x4, y4;
         double xx1, xx2, yy1, yy2;

         xx1 = -sizeA/2 + xoffset;
         xx2 =  sizeA/2 + xoffset;
         yy1 = -sizeB/2 + yoffset;
         yy2 =  sizeB/2 + yoffset;

         Rotate(xx1, yy1, RadToDeg(rotation), &x0, &y0);
         x1 = x+x0;
         y1 = y+y0;
         Rotate(xx2, yy1, RadToDeg(rotation), &x0, &y0);
         x2 = x+x0;
         y2 = y+y0;
         Rotate(xx2, yy2, RadToDeg(rotation), &x0, &y0);
         x3 = x+x0;
         y3 = y+y0;
         Rotate(xx1, yy2, RadToDeg(rotation), &x0, &y0);
         x4 = x+x0;
         y4 = y+y0;

         Dxf_Command(fp, "  0\nSOLID\n");
         Dxf_Handle(fp);
         Dxf_Layer(fp, Dxf_Level);
         Dxf_Koo(fp, 10, x1, y1);
         Dxf_Koo(fp, 11, x2, y2);
         Dxf_Koo(fp, 12, x4, y4);
         Dxf_Koo(fp, 13, x3, y3);
      }
      break;
   case T_OBLONG:
      {
         double x0, y0, x1, y1, x2, y2, x3, y3, x4, y4;
         double xx1, xx2, yy1, yy2;

         if (sizeA == sizeB) // horizontal oblong
            wdxf_Graph_Circle( x+xoffset, y+yoffset, sizeA/2);

         if (sizeA > sizeB) // horizontal oblong
         {
            sizeA = sizeA - sizeB;
            xx1 = -sizeA/2 + xoffset;
            xx2 =  sizeA/2 + xoffset;
            yy1 = -sizeB/2 + yoffset;
            yy2 =  sizeB/2 + yoffset;

            Rotate(xx1, yy1, RadToDeg(rotation), &x0, &y0);
            x1 = x+x0;
            y1 = y+y0;
            Rotate(xx2, yy1, RadToDeg(rotation), &x0, &y0);
            x2 = x+x0;
            y2 = y+y0;
            Rotate(xx2, yy2, RadToDeg(rotation), &x0, &y0);
            x3 = x+x0;
            y3 = y+y0;
            Rotate(xx1, yy2, RadToDeg(rotation) ,&x0, &y0);
            x4 = x+x0;
            y4 = y+y0;

            Dxf_Command(fp, "  0\nSOLID\n");
            Dxf_Handle(fp);
            Dxf_Layer(fp, Dxf_Level);
            Dxf_Koo(fp, 10, x1, y1);
            Dxf_Koo(fp, 11, x2, y2);
            Dxf_Koo(fp, 12, x4, y4);
            Dxf_Koo(fp, 13, x3, y3);

            xx1 = -sizeA/2 + xoffset;
            xx2 =  sizeA/2 + xoffset;
            yy1 =  yoffset;
            yy2 =  yoffset;

            Rotate(xx1, yy1, RadToDeg(rotation), &x0, &y0);
            x1 = x+x0;
            y1 = y+y0;
            wdxf_roundend(fp, Dxf_Level, sizeB, x1, y1);

            Rotate(xx2, yy2, RadToDeg(rotation), &x0, &y0);
            x2 = x+x0;
            y2 = y+y0;
            wdxf_roundend(fp, Dxf_Level, sizeB, x2, y2);
         }
         else
         {
            sizeB = sizeB - sizeA;
            xx1 = -sizeA/2 + xoffset;
            xx2 =  sizeA/2 + xoffset;
            yy1 = -sizeB/2 + yoffset;
            yy2 =  sizeB/2 + yoffset;

            Rotate(xx1, yy1, RadToDeg(rotation), &x0, &y0);
            x1 = x+x0;
            y1 = y+y0;
            Rotate(xx2, yy1, RadToDeg(rotation), &x0, &y0);
            x2 = x+x0;
            y2 = y+y0;
            Rotate(xx2, yy2, RadToDeg(rotation), &x0, &y0);
            x3 = x+x0;
            y3 = y+y0;
            Rotate(xx1, yy2, RadToDeg(rotation), &x0, &y0);
            x4 = x+x0;
            y4 = y+y0;

            Dxf_Command(fp, "  0\nSOLID\n");
            Dxf_Handle(fp);
            Dxf_Layer(fp, Dxf_Level);
            Dxf_Koo(fp, 10, x1, y1);
            Dxf_Koo(fp, 11, x2, y2);
            Dxf_Koo(fp, 12, x4, y4);
            Dxf_Koo(fp, 13, x3, y3);

            xx1 = xoffset;
            xx2 = xoffset;
            yy1 = -sizeB/2 + yoffset;
            yy2 =  sizeB/2 + yoffset;

            Rotate(xx1, yy1, RadToDeg(rotation), &x0, &y0);
            x1 = x+x0;
            y1 = y+y0;
            wdxf_roundend(fp, Dxf_Level, sizeA, x1, y1);

            Rotate(xx2, yy2, RadToDeg(rotation), &x0, &y0);
            x2 = x+x0;
            y2 = y+y0;
            wdxf_roundend(fp, Dxf_Level, sizeA, x2, y2);
         }
      }
      break;
   case T_OCTAGON:
      {
         Dxf_Poly(fp, Dxf_Level, 1);
         Dxf_Width(fp, Dxf_Linewidth);
         double radius = sizeA/2;
         double halfSide = sizeA/4.83; // size of half of one side of octagon
         Dxf_Vertex(fp, Dxf_Level, x - radius, y + halfSide, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x - halfSide, y + radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + halfSide, y + radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + radius, y + halfSide, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + radius, y - halfSide, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + halfSide, y - radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x - halfSide, y - radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x - radius, y - halfSide, 0.0);
         Dxf_Seqend(fp, Dxf_Level);
      }
      break;
   case T_TARGET:
      {
         BlockStruct block;
         block.setFlags(BL_APERTURE);
         block.setShape(type);
         block.setSizeA((DbUnit)sizeA);
         block.setSizeB((DbUnit)sizeB);
         block.setRotation((DbUnit)rotation);
         block.setXoffset((DbUnit)xoffset);
         block.setYoffset((DbUnit)yoffset);

         CPolyList *polylist = ApertureToPoly_Base(&block, x, y, 0.0, 0);
         if (!polylist)
            break;

         int oldlineend = lineend;
         lineend = 0;   

         // loop thru polys
         POSITION polyPos = polylist->GetHeadPosition();
         while (polyPos != NULL)
         {
            Dxf_Poly(fp, Dxf_Level, 0);
            Dxf_Width(fp, 0.0);

            CPoly *poly = polylist->GetNext(polyPos);

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);
               // here deal with bulge
               Dxf_Vertex(fp, Dxf_Level, pnt->x,pnt->y, pnt->bulge);
            }
            Dxf_Seqend(fp, Dxf_Level);
         }
         lineend = oldlineend;   

         FreePolyList(polylist);//this function in Polylib.cpp
      }
      break;
   case T_THERMAL:
      {
         // this is a filled aperture
         double width = (sizeA - sizeB)*.6; // this linewidth calc on aperture_to_poly 
                                            // on thermals is funny ???

         BlockStruct block;
         block.setFlags(BL_APERTURE);
         block.setShape(type);
         block.setSizeA((DbUnit)(sizeA-width/2));
         block.setSizeB((DbUnit)(sizeA-width/2));
         block.setRotation((DbUnit)rotation);
         block.setXoffset((DbUnit)xoffset);
         block.setYoffset((DbUnit)yoffset);

         CPolyList *polylist = ApertureToPoly_Base(&block, x, y, 0.0, 0);
         if (!polylist)
            break;

         int oldlineend = lineend;
         lineend = 0;   

         // loop thru polys
         POSITION polyPos = polylist->GetHeadPosition();
         while (polyPos != NULL)
         {
            Dxf_Poly(fp,Dxf_Level,0);
            Dxf_Width(fp, width);
            CPoly *poly = polylist->GetNext(polyPos);

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);
               Dxf_Vertex(fp,Dxf_Level,pnt->x,pnt->y, pnt->bulge);
            }
            Dxf_Seqend(fp,Dxf_Level);
         }
         lineend = oldlineend;   

         FreePolyList(polylist);//this function in Polylib.cpp
      }
      break;
   case T_UNDEFINED:
   case T_COMPLEX:      // this should never be here !, complex are treated different in DXFout.cpp
   default:
      break;
   }
   return(1);
}

/******************************************************************************
* wdxf_Graph_Circle
*/
int wdxf_Graph_Circle( double center_x, double center_y, double radius )
{
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
     fp = Dxf_Entity_File;

   if (Dxf_Linewidth < realwidth)
   {
      Dxf_Command(fp, "  0\nCIRCLE\n");
      Dxf_Handle(fp);
      Dxf_Layer(fp, Dxf_Level);
      Dxf_Koo(fp, 10, center_x, center_y);

		char tmp[80];
      sprintf(tmp, " 40\n%2.6lf\n", radius);
      Dxf_Command(fp,tmp);
   }
   else
   {
      Dxf_Poly(fp, Dxf_Level,0);
      Dxf_Width(fp, Dxf_Linewidth);
      Dxf_Vertex(fp, Dxf_Level, center_x-radius, center_y, 1);
      Dxf_Vertex(fp, Dxf_Level, center_x+radius, center_y, 1);
      Dxf_Vertex(fp, Dxf_Level, center_x-radius, center_y, 0);
      Dxf_Seqend(fp, Dxf_Level);
   }

   return(1);
}

/******************************************************************************
* wdxf_Graph_Arc
*/
int wdxf_Graph_Arc( double center_x, double center_y, double radius, double startangle,double deltaangle)
{
   double x1 = center_x + radius * cos(startangle);
   double y1 = center_y + radius * sin(startangle);

   double x2 = center_x + radius * cos(startangle+deltaangle);
   double y2 = center_y + radius * sin(startangle+deltaangle);

   double bulge = 0.0;
   double a = radius * sin(deltaangle / 2);
   if (fabs(a) > 0.00000001)
   {
      double b = radius - radius * cos(deltaangle/2);
      bulge =  b / a;
   }
   
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   if (Dxf_Linewidth < realwidth)
   {
      if (deltaangle < 0)  // clockwise
         Dxf_Arc(fp, Dxf_Level, center_x, center_y, radius, RadToDeg(startangle+deltaangle), RadToDeg(startangle));
      else
         Dxf_Arc(fp, Dxf_Level, center_x, center_y, radius, RadToDeg(startangle), RadToDeg(startangle+deltaangle));
   }
   else
   {
      Dxf_Poly(fp, Dxf_Level,0);
      Dxf_Width(fp, Dxf_Linewidth);
      Dxf_Vertex(fp, Dxf_Level, x1, y1, bulge);
      Dxf_Vertex(fp, Dxf_Level, x2, y2, 0.0);
      Dxf_Seqend(fp, Dxf_Level);

      if (lineend == 1) // round
      {
         wdxf_roundend(fp, Dxf_Level, Dxf_Linewidth, x1, y1);
         wdxf_roundend(fp, Dxf_Level, Dxf_Linewidth, x2, y2);
      }
   }
   return(1);
}

/******************************************************************************
* wdxf_Graph_Line
*/
int wdxf_Graph_Line( double x1, double y1, double x2, double y2)
{
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   if (Dxf_Linewidth < realwidth)
   {
      Dxf_Command(fp, "  0\nLINE\n");
      Dxf_Layer(fp, Dxf_Level);
      Dxf_Koo(fp, 10, x1, y1);
      Dxf_Koo(fp, 11, x2, y2);
   }
   else
   {
      Dxf_Poly(fp, Dxf_Level,0);
      Dxf_Width(fp, Dxf_Linewidth);
      Dxf_Vertex(fp, Dxf_Level, x1, y1, 0.0);
      Dxf_Vertex(fp, Dxf_Level, x2, y2, 0.0);
      Dxf_Seqend(fp, Dxf_Level);

      if (lineend == 1) // round or square
      {
         wdxf_roundend(fp, Dxf_Level, Dxf_Linewidth, x1, y1);
         wdxf_roundend(fp, Dxf_Level, Dxf_Linewidth, x2, y2);         
      }
   }
   return(1);
}

/*********************************************************
* 
*int make_arc_outpoly(double cx, double cy ,double radius, double start,
                  double delta, double width,Point2 pp[5])        
*Parameters:
*     --- np -> the data of arc
*     --- width -> the width of the arc 
*
*Description:
*
*     --- find the  points around the arc, and make them a close
*         poly 
*
*Return:
*
*     --- pp[5] -> the points which around the arc.
*     
*
***********************************************************/
int make_arc_outpoly(double cx, double cy,double radius, double start, double delta, double width, Point2 pp[5])
{
   double end; // the angle of the endpoint of the arc
   if (fabs(delta) < SMALLNUMBER
      || fabs(delta) == 2 * PI) // arc is a point
   {
      pp[0].bulge = 1;
      pp[1].bulge = 1;
      pp[0].x = pp[1].x = cx ;
      pp[0].y = cy + radius - width/2;
      pp[1].y = cy + radius + width/2;
      pp[0].bulge = pp[1].bulge;
      pp[2].x = pp[0].x;
      pp[2].y = pp[0].y;
      return 3;
   }
   else
   {
      if (delta > 0)
         end = start + delta;
      else //delta < 0
      {
         end = start;
         start = start + delta;
      }
      pp[0].bulge = tan(delta / 4);
      pp[3].bulge = tan(-delta / 4);
      pp[1].bulge = pp[2].bulge = 1;
      
      pp[0].x = cx + (radius + width) * cos(start);
      pp[0].y = cy + (radius + width) * sin(start);
      pp[1].x = cx + (radius + width) * cos(end);
      pp[1].y = cy + (radius + width) * sin(end);
      pp[2].x = cx + (radius - width) * cos(end);
      pp[2].y = cy + (radius - width) * sin(end);
      pp[3].x = cx + (radius - width) * cos(start);
      pp[3].y = cy + (radius - width) * sin(start);
      pp[4].x = pp[0].x ;
      pp[4].y = pp[0].y ;

      if (delta < 0)
         start = end;
      return 5;
   }
}

/******************************************************************************
* wdxf_Graph_HollowArcRoundEnd
*/
int wdxf_Graph_HollowArcRoundEnd( double center_x, double center_y,
                double radius, double startangle,double deltaangle)
{

   FILE *fp;
   Point2 pp[5];
   
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   int cnt = make_arc_outpoly(center_x, center_y, radius,  startangle, deltaangle,  Dxf_Linewidth,  pp);
   if (cnt < 5)
		return(1);

	// here hollow arc
   Dxf_Poly(fp, Dxf_Level,1);
   Dxf_Vertex(fp, Dxf_Level, pp[0].x, pp[0].y, pp[0].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[1].x, pp[1].y, pp[1].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[2].x, pp[2].y, pp[2].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[3].x, pp[3].y, pp[3].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[4].x, pp[4].y, pp[4].bulge);
   Dxf_Seqend(fp, Dxf_Level);

   return(1);
}

/*********************************************************
* 
*void make_line_outpoly(double x1,double y1,double x2,double y2, double width,
                  Point2 pp[5])        
*Parameters:
*     --- x1, y1, x2,y2 -> start & end points of the line 
*     --- width   -> the width of the line 
*
*Description:
*
*     --- find the 4 points around the line, and make them a close
*         outpoly
*
*Return:
*
*     --- pp[5] -> the points with the bulge which around the line.
*     
*                 
*
***********************************************************/
int make_line_outpoly(double x1, double y1,double x2,double y2, double width, Point2 pp[5])
{
   double p1p2,  // distance between p1 and p2
          lxn,   // x_distance between p1 and p1a,p1b or p2 and p2a,p2b
          lyn;  // y_distance between p1 and p1a,p2b or p2 and p2a,p2b

   if (fabs(x1 - x2) < SMALLNUMBER && fabs(y1 - y2) < SMALLNUMBER) //P1P2 IS A POINT
   {
      pp[0].bulge = 1;
      pp[1].bulge = 1;
      pp[0].x = pp[1].x = x1;
      pp[0].y = y1 - width/2;
      pp[1].y = y1 + width/2;
      pp[2].x = pp[0].x;
      pp[2].y = pp[0].y;     
      return 2;
   }
   else
   {
      pp[0].bulge = pp[2].bulge = pp[4].bulge = 0;
      pp[1].bulge = pp[3].bulge = 1;
      p1p2 = sqrt(pow(y1 - y2,2) + pow(x1 - x2,2));
      
      lxn = sqrt((pow(width/2,2) * pow(y2 - y1,2)) / (pow(y2 - y1,2) + pow(x2 - x1,2)));
      lyn = sqrt(pow(width/2,2) - pow(lxn,2));

      if ((y1 == y2) && (x1 > x2))   // line has no slope
      {
         pp[3].y = y1 - width/2;    pp[0].y = y1 + width/2;
         pp[2].y = y2 - width/2;    pp[1].y = y2 + width/2;
         pp[3].x = x1 ;    pp[0].x = x1 ;
         pp[2].x = x2 ;    pp[1].x = x2 ;
      }
      if ((y1 == y2) && (x2 > x1))   // line has no slope
      {
         pp[3].y = y1 + width/2;    pp[0].y = y1 - width/2;
         pp[2].y = y2 + width/2;    pp[1].y = y2 - width/2;
         pp[3].x = x1 ;    pp[0].x = x1 ;
         pp[2].x = x2 ;    pp[1].x = x2 ;
      }

      else if ((x1 == x2) && (y1 > y2)) // Undefined slope
      {  
         pp[3].x = x1 + width/2;    pp[0].x = x1 - width/2;
         pp[2].x = x2 + width/2;    pp[1].x = x2 - width/2;
         pp[3].y = y1 ; pp[0].y = y1 ;
         pp[2].y = y2 ; pp[1].y = y2 ;
      }
      else if ((x1 == x2) && (y2 > y1)) // Undefined slope
      {  
         pp[3].x = x1 - width/2;    pp[0].x = x1 + width/2;
         pp[2].x = x2 - width/2;    pp[1].x = x2 + width/2;
         pp[3].y = y1 ; pp[0].y = y1 ;
         pp[2].y = y2 ; pp[1].y = y2 ;
      }

      else if ((x2 > x1) && (y2 < y1))   // slope < 0
      {     
         pp[3].x = x1 + lxn; pp[0].x = x1 - lxn;
         pp[3].y = y1 + lyn; pp[0].y = y1 - lyn;
         pp[2].x = x2 + lxn; pp[1].x = x2 - lxn;
         pp[2].y = y2 + lyn; pp[1].y = y2 - lyn;
      }
      
      else if ((x2 < x1) && (y2 > y1))  // slope < 0
      {     
         pp[3].x = x1 - lxn; pp[0].x = x1 + lxn;
         pp[3].y = y1 - lyn; pp[0].y = y1 + lyn;
         pp[2].x = x2 - lxn; pp[1].x = x2 + lxn;
         pp[2].y = y2 - lyn; pp[1].y = y2 + lyn;
      }

      else if ((x2 < x1) && (y2 < y1))    // slope > 0
      {     
         pp[3].x = x1 + lxn; pp[0].x = x1 - lxn;
         pp[3].y = y1 - lyn; pp[0].y = y1 + lyn;
         pp[2].x = x2 + lxn; pp[1].x = x2 - lxn;
         pp[2].y = y2 - lyn; pp[1].y = y2 + lyn;
      }

      else if ((x2 > x1) && (y2 > y1)) // slope > 0
      {     
         pp[3].x = x1 - lxn; pp[0].x = x1 + lxn;
         pp[3].y = y1 + lyn; pp[0].y = y1 - lyn;
         pp[2].x = x2 - lxn; pp[1].x = x2 + lxn;
         pp[2].y = y2 + lyn; pp[1].y = y2 - lyn;
      }

      pp[4].x = pp[0].x;
      pp[4].y = pp[0].y;
      return 5;
   }
}

/******************************************************************************
* wdxf_Graph_HollowLineRoundEnd
*/
int wdxf_Graph_HollowLineRoundEnd( double x1, double y1, double x2, double y2)
{
   FILE *fp;
   Point2 pp[5];

   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   int cnt = make_line_outpoly(x1, y1, x2, y2, Dxf_Linewidth, pp);
   if (cnt < 5)
		return(1); 
   
	// here hollow line
   Dxf_Poly(fp,Dxf_Level,1);
   Dxf_Vertex(fp, Dxf_Level, pp[0].x, pp[0].y, pp[0].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[1].x, pp[1].y, pp[1].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[2].x, pp[2].y, pp[2].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[3].x, pp[3].y, pp[3].bulge);
   Dxf_Vertex(fp, Dxf_Level, pp[4].x, pp[4].y, pp[4].bulge);
   Dxf_Seqend(fp, Dxf_Level);

   return(1);
}

/******************************************************************************
* wdxf_Graph_Vertex
*/
int wdxf_Graph_Vertex( Point2 p)
{
   if (coo_list_cnt < MAX_POLY)
   {
      coo_list[coo_list_cnt].x = p.x;
      coo_list[coo_list_cnt].y = p.y;
      coo_list[coo_list_cnt].bulge = p.bulge;
      coo_list_cnt++;
   }
   
   return 1;
}

/******************************************************************************
* wdxf_Graph_SeqEnd
*/
int wdxf_Graph_SeqEnd()
{
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   Dxf_Seqend(fp, Dxf_Level);
   return 1;
}

/******************************************************************************
* wdxf_Graph_PolyEnd
*/
int wdxf_Graph_PolyEnd()
{
   FILE  *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   // make a special if only 2 ends square.
   Dxf_Poly(fp, Dxf_Level, dxf_polyclosed);
   if (Dxf_Linewidth >= realwidth)
      Dxf_Width(fp, Dxf_Linewidth);

   for (int i=0; i<coo_list_cnt; i++)
      Dxf_Vertex(fp, Dxf_Level, coo_list[i].x, coo_list[i].y, coo_list[i].bulge);

   Dxf_Seqend(fp, Dxf_Level);

   if (!dxf_polyclosed)
   {
      if (lineend == 1) // round 
      {
         wdxf_roundend(fp, Dxf_Level, Dxf_Linewidth, coo_list[0].x, coo_list[0].y);
         wdxf_roundend(fp, Dxf_Level, Dxf_Linewidth, coo_list[coo_list_cnt-1].x, coo_list[coo_list_cnt-1].y);
      }
      else if (lineend == 2 && coo_list_cnt > 1) // square
      {
         double headAngle, tailAngle;
         Point2 head, tail;

         headAngle = ArcTan2(coo_list[1].y - coo_list[0].y, coo_list[1].x - coo_list[0].x);
         tailAngle = ArcTan2(coo_list[coo_list_cnt-2].y - coo_list[coo_list_cnt-1].y, 
                             coo_list[coo_list_cnt-2].x - coo_list[coo_list_cnt-1].x);

         head.x = coo_list[0].x;
         head.y = coo_list[0].y;
         tail.x = coo_list[coo_list_cnt-1].x;
         tail.y = coo_list[coo_list_cnt-1].y;

         Point2 points[4];
         int arc = 0;
         double size, sine, cosine;

         // only flash on head if head is not an arc
         if (!arc)
         {
            size = Dxf_Linewidth / 2 * sqrt(2.);
            sine = sin(45*PI/180 + headAngle);
            cosine = cos(45*PI/180 + headAngle);
   
            points[0].x = head.x + cosine*size;
            points[0].y = head.y + sine*size;
               
            points[1].x = head.x + sine*size;
            points[1].y = head.y - cosine*size;
               
            points[2].x = head.x - cosine*size;
            points[2].y = head.y - sine*size;
               
            points[3].x = head.x - sine*size;
            points[3].y = head.y + cosine*size;
   
            Dxf_Command(fp, "  0\nSOLID\n");
            Dxf_Handle(fp);
            Dxf_Layer(fp, Dxf_Level);
            Dxf_Koo(fp, 10, points[0].x, points[0].y);
            Dxf_Koo(fp, 11, points[1].x, points[1].y);
            Dxf_Koo(fp, 12, points[3].x, points[3].y);
            Dxf_Koo(fp, 13, points[2].x, points[2].y);
         }

         if (!arc)
         {
            sine = sin(45*PI/180 + tailAngle);
            cosine = cos(45*PI/180 + tailAngle);
   
            points[0].x = tail.x + cosine*size;
            points[0].y = tail.y + sine*size;
               
            points[1].x = tail.x + sine*size;
            points[1].y = tail.y - cosine*size;
               
            points[2].x = tail.x - cosine*size;
            points[2].y = tail.y - sine*size;
            
            points[3].x = tail.x - sine*size;
            points[3].y = tail.y + cosine*size;
   
            Dxf_Command(fp, "  0\nSOLID\n");
            Dxf_Handle(fp);
            Dxf_Layer(fp, Dxf_Level);
            Dxf_Koo(fp, 10, points[0].x, points[0].y);
            Dxf_Koo(fp, 11, points[1].x, points[1].y);
            Dxf_Koo(fp, 12, points[3].x, points[3].y);
            Dxf_Koo(fp, 13, points[2].x, points[2].y);
         }
      }
   }
   coo_list_cnt = 0;

   return 1;
}

/******************************************************************************
* wdxf_Graph_Polyline
*/
int wdxf_Graph_Polyline( Point2 p, int closed)
{
   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;
   
   coo_list_cnt = 0;
   coo_list[coo_list_cnt].x = p.x;
   coo_list[coo_list_cnt].y = p.y;
   coo_list[coo_list_cnt].bulge = p.bulge;
   coo_list_cnt++;

   dxf_polyclosed = closed;
   return(1);
}

/******************************************************************************
* wdxf_Graph_Convert_VerticalTextPosition
  covert 
*/
static int wdxf_Graph_CCToDXFVerticalJustificationType(VerticalPositionTag vertical)
{
   switch(vertical)
   {
   case verticalPositionCenter   :
      return 2;
   case verticalPositionTop      :
      return 3;
   case verticalPositionBottom   :
      return 1;
   }
   return 0;
}

/******************************************************************************
* wdxf_Graph_Text
  height is height of char
  width is width of char
*/
int wdxf_Graph_Text( const char *text, double x1, double y1,
         double height, double width, double angle, int text_mirror, int oblique, 
         VerticalPositionTag vertical, HorizontalPositionTag horizontal)
{
   char *lp;
   if (text == NULL)
		return 0;
   lp = clean_text(text);

   if (height < SMALLNUMBER)
		return 0;

   FILE *fp;
   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   Dxf_Command(fp, "  0\nTEXT\n");
   Dxf_Layer(fp, Dxf_Level);
   Dxf_Koo(fp, 10, x1, y1);

	CString tmp = "";
	tmp.Format(" 40\n%2.6lf\n", height);
   Dxf_Command(fp, tmp);

   /* x scale width div. be height of char. */
   // this 0.8 is tested ! and works
   if(text_xscale)
   {
	   tmp.Format(" 41\n%2.6lf\n", (width/0.8)/height);
      Dxf_Command(fp, tmp);
   }

	tmp.Format("  1\n%s\n", lp);
   Dxf_Command(fp, tmp);

   // Horizontal text justification type 
   if(horizontal)
   {
      tmp.Format(" 72\n %d\n", horizontal);
      Dxf_Command(fp, tmp);
   }

   // Vertical text justification type 
   int dfxvertical = wdxf_Graph_CCToDXFVerticalJustificationType(vertical);
   if(dfxvertical)
   {
      tmp.Format(" 73\n %d\n", dfxvertical);
      Dxf_Command(fp, tmp);
   }

   // Second alignment point
   // This value is meaningful only if the value of a 72 or 73 group is nonzero 
   if(horizontal || dfxvertical)
   {
      Dxf_Koo(fp, 11, x1, y1);
   }

	Dxf_Rot50(fp, RadToDeg(angle));

   if (oblique)
   {
		tmp.Format(" 51\n%d\n", oblique);
      Dxf_Command(fp, tmp);
   }

   if (text_mirror && !text_unmirror)
      Dxf_Command(fp, " 71\n2\n");

   return 1;
}

/******************************************************************************
* wdxf_Graph_Attdef
*/
int wdxf_Graph_Attdef( const char *def_text, const char *prompt, const char *text, int flg,
         double x1, double y1, double height, double angle )
{
   FILE *fp;
   CString tmp;

   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   Dxf_Command(fp, "  0\nATTDEF\n");
   Dxf_Layer(fp, Dxf_Level);
   Dxf_Koo(fp, 10, x1, y1);

   tmp.Format(" 40\n%2.6lf\n", height);
   Dxf_Command(fp, tmp);

   tmp.Format("  1\n%s\n", def_text);
   Dxf_Command(fp, tmp);

   tmp.Format("  2\n%s\n", clean_text(text));   // limit in acad
   Dxf_Command(fp, tmp);

   tmp.Format("  3\n%s\n", prompt);
   Dxf_Command(fp, tmp);

   tmp.Format(" 70\n%d\n", flg);
   Dxf_Command(fp, tmp);
   Dxf_Rot50(fp, RadToDeg(angle));
   return 1;
}

/******************************************************************************
* wdxf_Graph_Attins
*/
int wdxf_Graph_Attins( const char *value, const char *tag, int flg,
         double x1, double y1, double height, double angle, int mirror)
{
   FILE *fp;
   CString tmp;

   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   Dxf_Command(fp, "  0\nATTRIB\n");
   Dxf_Layer(fp, Dxf_Level);
   Dxf_Koo(fp, 10, x1, y1);

   tmp.Format(" 40\n%2.6lf\n", height);
   Dxf_Command(fp, tmp);

   if (mirror)
      tmp.Format(" 41\n-1\n");
   else
      tmp.Format(" 41\n1\n");
   Dxf_Command(fp, tmp);

   if (strlen(value) > 255)   // limit in autocad
   {
      char t[256];
      strncpy(t, value, 255);
      t[255] = '\0';
      tmp.Format("  1\n%s\n", t);
   }
   else
	{
      tmp.Format("  1\n%s\n", value);
	}
   Dxf_Command(fp, tmp);

   tmp.Format("  2\n%s\n", tag);
   Dxf_Command(fp, tmp);

   tmp.Format(" 70\n%d\n", flg);
   Dxf_Command(fp, tmp);
   Dxf_Rot50(fp, RadToDeg(angle));
   return 1;
}

/******************************************************************************
* update_blocklist
*/
int update_blocklist(const char *block_name)
{
   // first check if block is in list
   int i = in_block_list(block_name);
   if (i)
		return i-1;

   if (block_list_cnt < MAX_BLOCK)
   {
      if ((block_list[block_list_cnt].name = STRDUP(block_name)) == NULL)
      {
         MessageBox(NULL, "Out of Memory", "dxflib", MB_ICONEXCLAMATION | MB_OK);
         exit(1);
      }
      block_list_cnt++;
   }
   return block_list_cnt -1;
}

/******************************************************************************
* wdxf_Graph_Block_On
*/
int wdxf_Graph_Block_On(const char *block_name,int attflg)
{
   CString tmp;
   blockmode++;

   update_blocklist(block_name);

   Dxf_Command(Dxf_Block_File, "  0\nBLOCK\n");
   Dxf_Layer(Dxf_Block_File, "0");

   tmp.Format("  2\n%s\n", block_name);
   Dxf_Command(Dxf_Block_File, tmp);

   if (attflg)
      Dxf_Command(Dxf_Block_File, " 70\n2\n");
   else
      Dxf_Command(Dxf_Block_File, " 70\n64\n");

   Dxf_Koo(Dxf_Block_File, 10, 0.0, 0.0);
   return  1;
}

/******************************************************************************
* wdxf_Graph_Block_Off
*/
int wdxf_Graph_Block_Off( )
{
   Dxf_Command(Dxf_Block_File, "  0\nENDBLK\n");
   Dxf_Layer(Dxf_Block_File, "0");
   blockmode--;
   return 1;
}

/******************************************************************************
* wdxf_Graph_Block_Reference
*/
int wdxf_Graph_Block_Reference(const char *block_name, double x1, double y1,
               double angle, int mirror,  double scale,int attflg )
{
   FILE *fp;
   CString tmp;

   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   if (Dxf_Check_Block)
   {
      if (!in_block_list(block_name))
			return 1;
   }

   Dxf_Command(fp, "  0\nINSERT\n");
   Dxf_Layer(fp, "0");

   if (attflg)
   {
      tmp.Format(" 66\n%d\n", attflg);
      Dxf_Command(fp, tmp);
   }

   update_blocklist(block_name); 

   tmp.Format("  2\n%s\n", block_name);
   Dxf_Command(fp, tmp);
   Dxf_Koo(fp, 10, x1, y1);

   if (mirror)
      tmp.Format(" 41\n%2.6lf\n", -scale);
   else
      tmp.Format(" 41\n%2.6lf\n", scale);
   Dxf_Command(fp, tmp);

   tmp.Format(" 42\n%2.6lf\n", scale);
   Dxf_Command(fp, tmp);
   Dxf_Rot50(fp, RadToDeg(angle));
   return 1;
}

/******************************************************************************
* wdxf_Graph_Block_Reference_Layer
*/
int wdxf_Graph_Block_Reference_Layer(const char *layername, const char *block_name, double x1, double y1,
											double angle, int mirror, double scale, int flg )
{
   FILE *fp;
   CString tmp;

   if (blockmode)
      fp = Dxf_Block_File;
   else
      fp = Dxf_Entity_File;

   Dxf_Command(fp, "  0\nINSERT\n");
   Dxf_Layer(fp, layername);

   tmp.Format("  2\n%s\n", block_name);
   Dxf_Command(fp, tmp);
   Dxf_Koo(fp, 10, x1, y1);

   tmp.Format(" 41\n%2.6lf\n42\n%2.6lf\n", scale, scale);
   Dxf_Command(fp, tmp);
   Dxf_Rot50(fp, RadToDeg(angle));
   return 1;
}

/******************************************************************************
* clean_text
*/
char  *clean_text(const char *t)
{
   int tcnt = 0;
   static char tt[255+2];

   for (int i=0; i<(int)strlen(t); i++)
   {
      // elimitate all things smaller than blank
      if (t[i] < ' ')
			continue;
      if (tcnt < 255)
      {
         tt[tcnt] = t[i];
         tcnt++;
      }
   }
   
   // if the last character is a ^ it must be followed by a blank
   if (tcnt)
   {
      if (tt[tcnt-1] == '^')
      {
         tt[tcnt] = ' ';
         tcnt++;
      }
   }

   tt[tcnt] = '\0';
   return tt;
}

/******************************************************************************
* invalid_blockname
*/
static int invalid_blockname(char *blk_tmp)
{
   int invalid = 0;
   if (strlen(blk_tmp) > 31)
		invalid = TRUE;

   for (int i=0; i<(int)strlen(blk_tmp); i++)
   {
      if (isdigit(blk_tmp[i]))   continue;
      if (isalpha(blk_tmp[i]))   continue;
      if (blk_tmp[i] == '$')		continue;
      if (blk_tmp[i] == '-')		continue;
      if (blk_tmp[i] == '_')		continue;

      blk_tmp[i] = '_';
      invalid++;
   }
   return invalid;
}

/******************************************************************************
* update_dxflay
*/
int update_dxflay(const char *l)
{
   // if aperature is not an any layer, do it on ACAD layer
   if (strlen(l) == 0)
      l = "0";

   for (int i=0; i<dxflayers; i++)
   {
      if (!STRCMPI(dxflay[i], l))
         return(i);
   }

   if (dxflayers < MAX_LAYER)
   {
      if ((dxflay[dxflayers] = STRDUP(l)) == NULL)
      {
         MessageBox(NULL, "Out of Memory", "ERROR", MB_OK | MB_ICONHAND);
         exit(1);
      }
      dxflayers++;
   }
   else
   {
		MessageBox(NULL, "Too many layers", "ERROR", MB_OK | MB_ICONHAND);
      return(0);
   }
   return(dxflayers-1);
}

/******************************************************************************
* Dxf_Rot50
*/
int Dxf_Rot50(FILE *fp,double degree)
{
   CString tmp;

   /* if near 0 do not write */
   if (fabs(degree) < 0.000000001)
		return 1;

   /* if near 360 do not write */
   if (fabs(360.0 - degree) < 0.000000001)
		return 1;

   while (degree < 0)
		degree += 360;
   while (degree >= 360)
		degree -= 360;

   tmp.Format("  50\n%2.6lf\n", degree);
   Dxf_Command(fp, tmp);
   return 1;
}

/******************************************************************************
* fillin
*/
int fillin(double xa,double xe,double y)
{
   wdxf_Graph_Line(xa, y, xe, y);
   return 1;
}

/******************************************************************************
* legal_layer
   Legal layer is only A..Z 0..9 $_-
*/
int legal_layer(char *l)
{
   if (strlen(l) >= 30)
		return 0;

   for (int i=0; i<(int)strlen(l); i++)
   {
      if (isalpha(l[i]))   continue;
      if (isdigit(l[i]))   continue;
      if (l[i] == '$')     continue;
      if (l[i] == '_')     continue;
      return 0;
   }
   return 1;
}

/****************************************************************************/
/*
   return i+1 in block list
*/
static int in_block_list(const char *n)
{
   for (int i=0;i <block_list_cnt; i++)
   {
      if (!strcmp(n, block_list[i].name))
         return i+1;
   }

   return 0;
}

/****************************************************************************/
/*
   End DXFLIB.C
*/
/****************************************************************************/

