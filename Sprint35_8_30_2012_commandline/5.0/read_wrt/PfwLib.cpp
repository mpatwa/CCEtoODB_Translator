// $Header: /CAMCAD/4.5/read_wrt/PfwLib.cpp 10    1/27/05 7:43p Kurt Van Ness $

/****************************************************************************
*     Output for PFW-Files Protel for Windows 2.7 Layout and Schematic

      Schematic units are in mil / 10

      todo in Schematic
      - cur_color update
      - font size for label
      - linecode
*/

#include "StdAfx.h"
#include "General.h"
#include <math.h>
#include "geomlib.h"
#include "pfwlib.h"

static FILE    *OFP = NULL;   /*Output Filepointer*/
static int     cur_layer = 1;
static double  cur_width = -1;
static int     below_zero = 0;
static long    cur_color = 16711680L;
static int     cur_polygonnumber = 0;  // local for line
static int     cur_polycnt = 0;        // current polygon

/* The Functions and Procedures ...*/
static void init();
static void deinit();
static long cnv_pfw(double inch);
static long cnv_pfw_width(double inch);
static void sort_box(double *x1,double *y1,double *x2,double *y2);

/****************************************************************************/
/*
*/
long cnv_pfw(double inch)
{
   // PCB units are in 0.000001 inches.
   long x = (long)floor(inch * 1000000.0 + 0.5);
   return x;
}

long cnv_pfwsch(double inch)
{
   long x = (long)floor(inch * 100.0 + 0.5);
   return x;
}

long cnv_pfw_width(double width)
{
   long x;
   if (width < 0.001)   width = 0.001;
   x = (long)floor(width * 1000000.0 + 0.5);
   return x;
}


void wpfw_Graph_Width(double w)
{
   cur_width = w;
   return;
}

void init()
{
   cur_polygonnumber = 0;
   cur_polycnt = 0;
}

void deinit()
{
}

/* sort box for lowerleft, topright */
void sort_box(double *x1,double *y1,double *x2,double *y2)
{
   double   lx = ((*x1) < (*x2)) ? (*x1) : (*x2);
   double   ly = ((*y1) < (*y2)) ? (*y1) : (*y2);
   double   tx = ((*x1) > (*x2)) ? (*x1) : (*x2);
   double   ty = ((*y1) > (*y2)) ? (*y1) : (*y2);

   *x1 = lx;
   *y1 = ly;
   *x2 = tx;
   *y2 = ty;
}

void wpfw_Graph_LevelNum(int num)
{
   cur_layer = num;
   if (cur_layer < 1)   cur_layer = 1;
   if (cur_layer > 34)  cur_layer = 34;
}

void wpfw_Graph_Level(const char *l)
{
   if (l == NULL) return;
   if (!isdigit(l[0]) && l[0] != '-')
   {
      cur_layer = 28;   // default
      // here are layer names
      if (!STRCMPI(l,"TOP LAYER"))     cur_layer = 1;
      else
      if (!STRCMPI(l,"MID LAYER 1"))   cur_layer = 2;
      else
      if (!STRCMPI(l,"MID LAYER 2"))   cur_layer = 3;
      else
      if (!STRCMPI(l,"MID LAYER 3"))   cur_layer = 4;
      else
      if (!STRCMPI(l,"MID LAYER 4"))   cur_layer = 5;
      else
      if (!STRCMPI(l,"MID LAYER 5"))   cur_layer = 6;
      else
      if (!STRCMPI(l,"MID LAYER 6"))   cur_layer = 7;
      else                             
      if (!STRCMPI(l,"MID LAYER 7"))   cur_layer = 8;
      else
      if (!STRCMPI(l,"MID LAYER 8"))   cur_layer = 9;
      else
      if (!STRCMPI(l,"MID LAYER 9"))   cur_layer = 10;
      else
      if (!STRCMPI(l,"MID LAYER 10"))  cur_layer = 11;
      else
      if (!STRCMPI(l,"MID LAYER 11"))  cur_layer = 12;
      else
      if (!STRCMPI(l,"MID LAYER 12"))  cur_layer = 13;
      else
      if (!STRCMPI(l,"MID LAYER 13"))  cur_layer = 14;
      else
      if (!STRCMPI(l,"MID LAYER 14"))  cur_layer = 15;
      else
      if (!STRCMPI(l,"BOTTOM LAYER"))  cur_layer = 16;
      else
      if (!STRCMPI(l,"TOP SILKSCREEN"))cur_layer = 17;
      else
      if (!STRCMPI(l,"BOTTOM SILKSCREEN"))   cur_layer = 18;
      else
      if (!STRCMPI(l,"TOP PASTE MASK"))   cur_layer = 19;
      else
      if (!STRCMPI(l,"BOTTOM PASTE MASK"))   cur_layer = 20;
      else
      if (!STRCMPI(l,"TOP SOLDER MASK"))  cur_layer = 21;
      else
      if (!STRCMPI(l,"BOTTOM SOLDER MASK"))  cur_layer = 22;
      else
      if (!STRCMPI(l,"PLANE 1")) cur_layer = 23;
      else
      if (!STRCMPI(l,"PLANE 2")) cur_layer = 24;
      else
      if (!STRCMPI(l,"PLANE 3")) cur_layer = 25;
      else
      if (!STRCMPI(l,"PLANE 4")) cur_layer = 26;
      else
      if (!STRCMPI(l,"DRILL GUIDE"))   cur_layer = 27;
      else
      if (!STRCMPI(l,"KEEP OUT LAYER"))   cur_layer = 28;
      else
      if (!STRCMPI(l,"MECHANICAL LAYER 1"))  cur_layer = 29;
      else
      if (!STRCMPI(l,"MECHANICAL LAYER 2"))  cur_layer = 30;
      else
      if (!STRCMPI(l,"MECHANICAL LAYER 3"))  cur_layer = 31;
      else
      if (!STRCMPI(l,"MECHANICAL LAYER 4"))  cur_layer = 32;
      else
      if (!STRCMPI(l,"DRILL DRAWING")) cur_layer = 33;
      else
      if (!STRCMPI(l,"MULTI LAYER"))   cur_layer = 34;
}
   else
   {
      cur_layer = atoi(l);
      if (cur_layer < 1)   cur_layer = 1;
      if (cur_layer > 34)  cur_layer = 34;
   }
}

int wpfw_Graph_Init()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

void wpfw_Graph_File_Open( FILE *file )
{
   OFP=file;
}

int wpfw_Graph_Init_Layout()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

int wpfw_Graph_Init_Schematic()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

void wpfw_Graph_File_Open_Layout( FILE *file)
{
   OFP=file;
   below_zero = FALSE;
   fprintf(OFP,"PCB FILE 6 VERSION 2.70\n");
   fprintf(OFP,"0 0 0 0 0 0 0 0 0 0\n");
}

void wpfw_Graph_File_Open_Schematic( FILE *file )
{
   OFP=file;

   fprintf(OFP,"Protel for Windows - Schematic Capture Ascii File Version 1.2 - 2.0\n");
   fprintf(OFP,"6\n");
   fprintf(OFP,"[Font_Table]\n");
   fprintf(OFP,"     1\n");
   fprintf(OFP,"     10 0 0 0 0 0 Times New Roman\n");
   fprintf(OFP,"EndFont\n");
   fprintf(OFP,"Library\n");
   fprintf(OFP,"   0\n");
   fprintf(OFP,"EndLibrary\n");
   fprintf(OFP,"  Organization\n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  \n");
   fprintf(OFP,"  0 0\n");
   fprintf(OFP,"  End\n");
   fprintf(OFP,"Future  1 8 6\n");
   fprintf(OFP,"EndFuture  1\n");
   fprintf(OFP,"Sheet\n");
   fprintf(OFP," 0 6 0 1 1 0 15269887 1 10 1 10 1000 800 0\n");
}

void wpfw_Graph_File_Close_Layout()
{
   fprintf(OFP,"ENDPCB\n");
   deinit();      /*deinitialize the Memory for the Layers ...*/
   if (below_zero)
   {
      CString  tmp;
      tmp.Format("Coordinate below 0,0 found.");
      MessageBox(NULL, tmp,"Error Origin position", MB_OK | MB_ICONHAND);
   }
}

void wpfw_Graph_File_Close_Schematic()
{
   fprintf(OFP,"EndSheet  \n");
   fprintf(OFP,"Version 2.0 Sheet\n");
   fprintf(OFP,"Library Version 2.0\n");
   fprintf(OFP,"   0\n");
   fprintf(OFP,"EndLibrary\n");
   fprintf(OFP,"1 4 4 20 0 \n");
   fprintf(OFP,"\n");
   fprintf(OFP,"EndSheet\n");
   deinit();      /*deinitialize the Memory for the Layers ...*/
}

void wpfw_Graph_Aperture(double x, double y, double sizea, double sizeb, int form, double rot, const char *cp)
{
   int   i;
   fprintf(OFP,"%s\n", cp);
   fprintf(OFP,"0 0 %ld %ld ", cnv_pfw(x), cnv_pfw(y));

   if (sizeb < 0.001)   sizeb = sizea;

   for (i=0;i<3;i++)
   {
      fprintf(OFP,"%ld %ld %d ",cnv_pfw(sizea), cnv_pfw(sizea), form);
   }
   fprintf(OFP, " 0 0 %d 0 0",cur_layer);
   fprintf(OFP, " %1.3lf 0 0 0 0 0 0 0",RadToDeg(rot));
   fprintf(OFP,"\n");
   fprintf(OFP,"\n"); // name
   return;
}

void wpfw_via(double x, double y, double diam, double drill, int netindex )
{
   fprintf(OFP,"FV\n");
   // select drc x y diam drill routestat layerpair netnum connectnum soldermaskexpansion powerexpansion movalble
   fprintf(OFP,"0 0 %ld %ld %ld %ld 1 0 %d 0 2000 10000 1\n",
         cnv_pfw(x), cnv_pfw(y), cnv_pfw(diam), cnv_pfw(drill), netindex);
   return;              
}

void wpfw_Graph_Circle( double center_x, double center_y, double radius, double width )
{
   wpfw_Graph_Arc(center_x,center_y, radius,0.0, PI2,width, 0 );
   return;
}

void wpfwcomp_Graph_Circle( double center_x, double center_y, double radius, double width )
{
   wpfwcomp_Graph_Arc(center_x,center_y, radius,0.0, PI2,width );
   return;
}

void wpfwschem_Graph_Circle( double center_x, double center_y, double radius, int widthcode )
{
   wpfwschem_Graph_Arc(center_x,center_y, radius,0.0, PI2,widthcode );
   return;
}

/****************************************************************************
* wpfw_Graph_Arc
*/
void wpfw_Graph_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle, double width, int netindex )
{
   double sa,ea;

   if (deltaangle < 0)
   {
      sa = RadToDeg(startangle) + RadToDeg(deltaangle);
      ea = RadToDeg(startangle);
   }
   else
   {
      sa = RadToDeg(startangle);
      ea = RadToDeg(startangle) + RadToDeg(deltaangle);
   }
   if (sa >= 360)  sa = sa - 360;
   if (sa < 0)     sa = sa + 360;

   if (ea >= 360)  ea = ea - 360;
   if (ea < 0)     ea = ea + 360;

   fprintf(OFP,"FA\n");
   fprintf(OFP,"0 0 %ld %ld %ld %1.3lf %1.3lf %ld %d %d 0\n",
      cnv_pfw(center_x),cnv_pfw(center_y),cnv_pfw(radius),
      sa,ea,cnv_pfw_width(width),cur_layer, netindex);
   fprintf(OFP," 0 1\n");
   if (cnv_pfw(center_x) < 0) 
      below_zero = TRUE;
   if (cnv_pfw(center_y) < 0) 
      below_zero = TRUE;
}
/****************************************************************************
* wpfw_Graph_Arc
*/
void wpfwcomp_Graph_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle, double width )
{
   double sa,ea;

   if (deltaangle < 0)
   {
      sa = RadToDeg(startangle) + RadToDeg(deltaangle);
      ea = RadToDeg(startangle);
   }
   else
   {
      sa = RadToDeg(startangle);
      ea = RadToDeg(startangle) + RadToDeg(deltaangle);
   }
   if (sa >= 360)  sa = sa - 360;
   if (sa < 0)     sa = sa + 360;

   if (ea >= 360)  ea = ea - 360;
   if (ea < 0)     ea = ea + 360;

   fprintf(OFP,"CA\n");
   fprintf(OFP,"0 0 %ld %ld %ld %1.3lf %1.3lf %ld %d 0 0\n",
      cnv_pfw(center_x),cnv_pfw(center_y),cnv_pfw(radius),
      sa,ea,cnv_pfw_width(width),cur_layer);
   fprintf(OFP," 0 1\n");
   if (cnv_pfw(center_x) < 0) 
      below_zero = TRUE;
   if (cnv_pfw(center_y) < 0) 
      below_zero = TRUE;
}

/****************************************************************************
* wpfw_Graph_Arc
*/
void wpfwschem_Graph_Arc( double center_x, double center_y, double radius,
                          double startangle, double deltaangle, int widthcode )
{
   double sa,ea;

   if (deltaangle < 0)
   {
      sa = RadToDeg(startangle) + RadToDeg(deltaangle);
      ea = RadToDeg(startangle);
   }
   else
   {
      sa = RadToDeg(startangle);
      ea = RadToDeg(startangle) + RadToDeg(deltaangle);
   }
   if (sa >= 360)  sa = sa - 360;
   if (sa < 0)     sa = sa + 360;

   if (ea >= 360)  ea = ea - 360;
   if (ea < 0)     ea = ea + 360;

   fprintf(OFP,"     Arc %ld %ld %ld %d %1.3lf %1.3lf %ld 0\n",
      cnv_pfwsch(center_x),cnv_pfwsch(center_y),cnv_pfwsch(radius),widthcode,
      sa,ea,cur_color);
}

/*****************************************************************************
* wpfw_Graph_Line
*/
void wpfw_Graph_Line( double x1, double y1,double bulge1, double x2, double y2,double bulge2, 
                      double width, int netindex)
{
   int     hilite = 0;

   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      wpfw_Graph_Arc(cx,cy, r,sa, da,width, netindex );
   }
   else
   {
      if (cur_polygonnumber)
         hilite = 1;

      wpfw_Graph_Width(width);
      fprintf(OFP,"FT\n");
      fprintf(OFP,"%d 0 %ld %ld %ld %ld %ld %d 0 %d 0\n",hilite,
         cnv_pfw(x1),cnv_pfw(y1),cnv_pfw(x2),cnv_pfw(y2), cnv_pfw_width(width),cur_layer, netindex);
      fprintf(OFP," %d 0 1\n", cur_polygonnumber);
      if (cnv_pfw(x1) < 0) 
         below_zero = TRUE;
      if (cnv_pfw(y1) < 0) 
         below_zero = TRUE;
      if (cnv_pfw(x2) < 0) 
         below_zero = TRUE;
      if (cnv_pfw(y2) < 0) 
         below_zero = TRUE;
   }

   return;
}

/*****************************************************************************/
/* 
   rotation in rad   
*/
void wpfwcomp_Header( double xref, double yref,
                      double x1, double y1, double x2, double y2, double rot,
                      int show_ref, int show_comment)
{
   fprintf(OFP,"0 0 %ld %ld %d %d 0 %ld %ld %ld %ld %1.3lf 1 1\n",
      cnv_pfw(xref), cnv_pfw(yref), show_ref, show_comment,
      cnv_pfw(x1), cnv_pfw(y1),cnv_pfw(x2), cnv_pfw(y2),
      RadToDeg(rot) );
}

/*****************************************************************************/
/* 
   rotation in rad   
*/
void wpfwcomp_pin( double xref, double yref, const char *pinname,
                  double topx, double topy, int topform,
                  double innerx, double innery, int innerform,
                  double botx, double boty, int botform,
                  double holesize, int netindex, double degree, int layernum)
{
   fprintf(OFP,"CP\n");
   fprintf(OFP,"0 0 %ld %ld %ld %ld %d %ld %ld %d  %ld %ld %d %ld 0 %d %d 0 %1.1lf 2000 0 10000 10000 10000 4 1\n",
      cnv_pfw(xref), cnv_pfw(yref),
      cnv_pfw(topx), cnv_pfw(topy), topform, 
      cnv_pfw(innerx), cnv_pfw(innery), innerform, 
      cnv_pfw(botx), cnv_pfw(boty), botform,
      cnv_pfw(holesize), layernum, netindex, degree);

   fprintf(OFP,"%s\n",pinname);
}

/*****************************************************************************
* wpfw_Graph_Line
*/
void wpfwcomp_Graph_Line( double x1, double y1, double bulge1, 
                          double x2, double y2, double bulge2, double width )
{
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      wpfwcomp_Graph_Arc(cx,cy, r,sa, da,width );
   }
   else
   {
      wpfw_Graph_Width(width);
      fprintf(OFP,"CT\n");
      fprintf(OFP,"0 0 %ld %ld %ld %ld %ld %d 0 0 0\n",
         cnv_pfw(x1),cnv_pfw(y1),cnv_pfw(x2),cnv_pfw(y2), cnv_pfw_width(width),cur_layer);
      fprintf(OFP," 0 0 1\n");
      if (cnv_pfw(x1) < 0) 
         below_zero = TRUE;
      if (cnv_pfw(y1) < 0) 
         below_zero = TRUE;
      if (cnv_pfw(x2) < 0) 
         below_zero = TRUE;
      if (cnv_pfw(y2) < 0) 
         below_zero = TRUE;
   }
   return;
}

/*****************************************************************************
* wpfw_Graph_Line
*/
void wpfwschem_Graph_Line( double x1, double y1, double x2, double y2, int widthcode )
{
   fprintf(OFP,"     Line %ld %ld %ld %ld %d 0 %ld 0\n",
      cnv_pfwsch(x1),cnv_pfwsch(y1),cnv_pfwsch(x2),cnv_pfwsch(y2),widthcode,cur_color);
   return;
}

/*****************************************************************************
*
*/
void wpfw_Graph_Text( char *text, double x, double y,
      double height, double width, double angle, int mirror )
{
   double rot = RadToDeg(angle);
   double   x1,y1,x2,y2;

   if (strlen(text) == 0)  return;

   // no rotation greater 360
   if (rot > 360) rot -= 360;
   // no rotation smaller 0
   if (rot < 0) rot += 360;

   x1 = 0.0;y1 = 0.0;
   Rotate(height,strlen(text)*height*6.0/8.0,rot,&x2,&y2);
   sort_box(&x1,&y1,&x2,&y2);

   fprintf(OFP,"FS\n");
   fprintf(OFP,"0 0 %ld %ld %ld %1.3lf %d 1 %d ",
      cnv_pfw(x), cnv_pfw(y), cnv_pfw(height), rot, mirror, cur_layer);
   fprintf(OFP,"%ld %ld %ld %ld %ld 0\n",
      cnv_pfw(x1+x), cnv_pfw(y1+y), cnv_pfw(x2+x), cnv_pfw(y2+y),6000L);
   fprintf(OFP,"%s\n", text);

}

/*****************************************************************************
*
*/
void wpfwcomp_Graph_Text( char *text, double x, double y,
      double height, double width, double angle, int mirror )
{
   double rot = RadToDeg(angle);
   double   x1,y1,x2,y2;

   // must allow this
   //if (strlen(text) == 0)  return;

   // no rotation greater 360
   if (rot > 360) rot -= 360;
   // no rotation smaller 0
   if (rot < 0) rot += 360;

   x1 = 0.0;y1 = 0.0;
   Rotate(height,strlen(text)*height*6.0/8.0,rot,&x2,&y2);
   sort_box(&x1,&y1,&x2,&y2);

   fprintf(OFP,"CS\n");
   fprintf(OFP,"0 0 %ld %ld %ld %1.3lf %d 1 %d ",
      cnv_pfw(x), cnv_pfw(y), cnv_pfw(height), rot, mirror, cur_layer);
   fprintf(OFP,"%ld %ld %ld %ld %ld 0\n",
      cnv_pfw(x1+x), cnv_pfw(y1+y), cnv_pfw(x2+x), cnv_pfw(y2+y),6000L);
   fprintf(OFP,"%s\n", text);

}

void wpfwschem_Graph_Text( char *text, double x, double y,int textfont,
                         double angle, int mirror )
{
   double rot = RadToDeg(angle);
   int   introt;

   if (strlen(text) == 0)  return;

   // no rotation greater 360
   if (rot > 360) rot -= 360;
   // no rotation smaller 0
   if (rot < 0) rot += 360;

   introt = round(rot / 90);

   fprintf(OFP,"     Label %ld %ld %d %ld %d 0 '%s'\n",
           cnv_pfwsch(x), cnv_pfwsch(y), introt, cur_color, textfont, text);
}

void wpfw_Graph_Polyline( Point2 *coordinates, int pairs, int filled, int netindex )
{
   int count;
   int   i;

   if (filled)
   {
      ++cur_polycnt;

      fprintf(OFP,"PG\n%d\n %d\n0 0 0 0 0 0\n10000 10000\n", cur_layer, netindex);
      fprintf(OFP,"%d\n",pairs-1);
      for (count=0;count<pairs;count++)
      {
         fprintf(OFP,"%ld %ld\n",
            cnv_pfw(coordinates[count].x),cnv_pfw(coordinates[count].y));
      }
      for (count=0;count<pairs;count++)
      {
         double da = atan(coordinates[count].bulge) * 4;

         if (fabs(da) > SMALLANGLE)
            fprintf(OFP," 1");
         else
            fprintf(OFP," 0");
      }
      fprintf(OFP,"\n");
      for (i=1;i<pairs;i++)
      {
         cur_polygonnumber = cur_polycnt;
         wpfw_Graph_Line( coordinates[i-1].x, coordinates[i-1].y, coordinates[i-1].bulge,
                       coordinates[i].x,   coordinates[i].y, coordinates[i].bulge, cur_width, netindex) ;
         cur_polygonnumber = 0;
      }

   }
   else
   {
      for (i=1;i<pairs;i++)
      {
         wpfw_Graph_Line( coordinates[i-1].x, coordinates[i-1].y, coordinates[i-1].bulge,
                       coordinates[i].x,   coordinates[i].y, coordinates[i].bulge, cur_width, netindex ) ;
      }
   }
}

void wpfwcomp_Graph_Polyline( Point2 *coordinates, int pairs, int filled )
{
   int   i;

//   if (filled)
   for (i=1;i<pairs;i++)
   {
       wpfwcomp_Graph_Line( coordinates[i-1].x, coordinates[i-1].y,coordinates[i-1].bulge,
                     coordinates[i].x, coordinates[i].y, coordinates[i].bulge, cur_width ) ;
   }
}

void wpfwschem_Graph_Polyline( Point2 *coordinates, int pairs, int filled )
{
   int count;
   int   i;

   if (!pairs) return;

//   if (filled)
   if (1)
   {
      // this is not filled
      fprintf(OFP,"     Polyline %d 0 %ld 0 %d",0,cur_color,pairs);
      for (count=0;count<pairs;count++)
      {
         fprintf(OFP," %ld %ld",
            cnv_pfwsch(coordinates[count].x),cnv_pfwsch(coordinates[count].y));
      }
      fprintf(OFP,"\n");
   }
   else
   {
      for (i=1;i<pairs;i++)
      {
         wpfw_Graph_Line( coordinates[i-1].x, coordinates[i-1].y, coordinates[i-1].bulge,
                       coordinates[i].x, coordinates[i].y, coordinates[i].bulge, cur_width, 0) ;
      }
   }
}

// end PFWLIB.C

