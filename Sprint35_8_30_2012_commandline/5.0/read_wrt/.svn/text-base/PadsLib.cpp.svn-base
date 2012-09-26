// $Header: /CAMCAD/4.5/read_wrt/PadsLib.cpp 13    1/31/05 2:59p Dean Admin $

/*****************************************************************************/
/*    
   Output for PADS-Files
*/

#include "StdAfx.h"
#include "General.h"

#include "padslib.h"
#include "math.h"

#define  TEXTCORRECT       1.4      // adjust PADS height to real height

static FILE    *OFP;                // Output Filepointer
static int     cur_level;
static char    *cur_type;           // COPPER
                                    // BOARD
                                    // otherwise

static int     PADSVERSION = 2;     // 
static int     output_units = 3;
static double  small_delta;         // smallest output
static void    init();
static void    deinit();

static long    circlecnt = 0;
static long    arccnt = 0;
static long    linecnt = 0;
static long    polycnt = 0;

/*****************************************************************************/
/*
   Pads does not allow -0.000 which can happen in fprintf
*/
double wPads_Units(int format, double x)
{
   CString  t;
   t.Format("%1.*lf", format, x);
   return atof(t);
}

//--------------------------------------------------------------------------

static double clean_rot(double degree)
{
   while (degree < 0)      degree += 360;
   while (degree >= 360)   degree -= 360;

   return degree;
}

/*****************************************************************************
* 
*/
void init()
{
   circlecnt = arccnt = linecnt = 0;
   polycnt = 0;
   cur_type = "LINES";
}
void deinit()
{
}

//
// Layer names can be Cxxxx[space]number
//                    Bxxxx
//                otherwise
//

/*****************************************************************************
* 
*/
void wPADS_Graph_Level(const char *l )
{
   if (l[0] == 'C' || l[0] == 'c')
   {
      cur_type = "COPPER";
   }
   else
   if (l[0] == 'B' || l[0] == 'B')
   {
      cur_type = "BOARD";
   }
   else
   {
      cur_type = "LINES";
   }

   // here clean level
   while (l[0] && !isdigit(l[0]))
      l++;

   if (strlen(l))
      cur_level = atoi(l);
   else
      cur_level = 1;
}

/*****************************************************************************
* 
*/
void wPADS_Graph_LevelNum(int l )
{
   cur_level = l;
}

/*****************************************************************************
* 
*/
int wPADS_Graph_Units(int output_units_accuracy, double smalldelta)
{
   output_units = output_units_accuracy;
   small_delta  = smalldelta;
   return 1;
}

/*****************************************************************************
* 
*/
int wPADS_Graph_Init(int output_units_accuracy, double smalldelta)
{
   wPADS_Graph_Units(output_units_accuracy, smalldelta);
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

/*****************************************************************************
* 
*/
int wPADS_GraphSchem_Init()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

/*****************************************************************************
* 
*/
int wPADS_Graph_File_Open( FILE *file, int version, const char *unitstring )
{
   OFP=file;
   PADSVERSION = version;

   if (version == 3)
   {
      fprintf(OFP,"!PADS-POWERPCB-V3.0-%s! DESIGN DATABASE ASCII FILE 2.0\n", unitstring);
   }
   else
   {
      CString  tmp;
      tmp.Format("Unsupported PADS Version [%d]", version);
      ErrorMessage(tmp, "Version Error");
      return 0;
   }
   return 1;
}

/*****************************************************************************/
/*
*/
void wPADS_GraphSchem_File_Open( FILE *file )
{
   OFP=file;

   fprintf(OFP,"*PADS-LOGIC-V4* FILE INFORMATION   RELEASE VERSION: 4.01\n");
   fprintf(OFP,"*SCH*        GENERAL PARAMETERS OF THE SCHEMATIC DESIGN\n");
   fprintf(OFP,"\n");
   fprintf(OFP,"*SHT*   1 $$$NONE -1 $$$NONE\n");
   fprintf(OFP,"\n");
}

/*****************************************************************************
* 
*/
void wPADS_Graph_File_Close()
{
   fprintf(OFP,"*END*     OF ASCII OUTPUT FILE\n");
   deinit();      /*deinitialize the Memory for
              the Layers ...*/
}

/*****************************************************************************
* 
*/
void wPADS_GraphSchem_File_Close()
{
   fprintf(OFP,"*END*     OF ASCII OUTPUT FILE\n");
   deinit();      /*deinitialize the Memory for
              the Layers ...*/
}

/*****************************************************************************
* 
*/
void wPADS_Graph_Circle( double center_x, double center_y, double radius, double width,
                         const char *linetype, const char *piecetype, const char *restriction)
{
   if (width > 0.25) width = 0.25;

   fprintf(OFP,"CIRCLE%ld %s %lg %lg 1\n", circlecnt++, linetype, 
      wPads_Units(output_units, center_x), wPads_Units(output_units, center_y));
   fprintf(OFP, "%s  2  %lg %d %s\n",piecetype, wPads_Units(output_units, width), cur_level, restriction);
   fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, 0.0), wPads_Units(output_units, -radius));
   fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, 0.0), wPads_Units(output_units, radius));
}

/*****************************************************************************
* 
*/
void wPADS_GraphSchem_Circle( double center_x, double center_y, double radius, double width )
{
   if (width > 0.25) width = 0.25;

   fprintf(OFP,"CIRCLE%ld LINES %1.0lf %1.0lf 1\n",circlecnt++,0.0,0.0);
   fprintf(OFP,"CIRCLE  2  %1.0lf 255\n",width);
   fprintf(OFP,"%1.0lf %1.0lf\n",center_x, center_y - radius);
   fprintf(OFP,"%1.0lf %1.0lf\n",center_x, center_y + radius);
}

/*****************************************************************************
* 
*/
void wPADS_Graph_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle,  double width )
{
   double x,y, xe,ye,sa;
   char  *l = "OPEN";

   if (cur_type[0] == 'C')  l = "COPOPN";

   if (width > 0.25) width = 0.25;

   x = center_x + radius * cos (startangle);
   y = center_y + radius * sin (startangle);
   xe = center_x + radius * cos (startangle+deltaangle);
   ye = center_y + radius * sin (startangle+deltaangle);

   sa = RadToDeg(startangle);
   if (sa < 0) sa += 360;

   fprintf(OFP,"ARC%ld %s %lg %lg 1\n",arccnt++,cur_type,
      wPads_Units(output_units, 0.0) , wPads_Units(output_units, 0.0));
   fprintf(OFP,"%s 2 %lg %d\n",l, wPads_Units(output_units, width), cur_level);
   fprintf(OFP,"%lg %lg %1.0lf %1.0lf ",
      wPads_Units(output_units, x), wPads_Units(output_units, y), sa * 10, RadToDeg(deltaangle) * 10);
   fprintf(OFP,"%lg %lg ",
      wPads_Units(output_units, center_x - radius), wPads_Units(output_units, center_y - radius));
   fprintf(OFP,"%lg %lg\n",
      wPads_Units(output_units, center_x + radius), wPads_Units(output_units, center_y + radius));
   fprintf(OFP,"%lg %lg\n",
      wPads_Units(output_units, xe), wPads_Units(output_units, ye));
}

/*****************************************************************************
* 
*/
void wPADS_GraphSchem_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle,  double width )
{
   double x,y, xe,ye, sa;

   if (width > 250) width = 250;

   x = center_x + radius * cos (startangle);
   y = center_y + radius * sin (startangle);
   xe = center_x + radius * cos (startangle+deltaangle);
   ye = center_y + radius * sin (startangle+deltaangle);

   sa = RadToDeg(startangle);
   if (sa < 0) sa += 360;

   fprintf(OFP,"ARC%ld LINES %1.0lf %1.0lf 1\n",arccnt++,0.0,0.0);
   fprintf(OFP,"OPEN 2 %1.0lf 255\n",width);
   fprintf(OFP,"%1.0lf %1.0lf %1.0lf %1.0lf %1.0lf ", x, y, sa * 10, RadToDeg(deltaangle) * 10, RadToDeg(deltaangle) * 10);
   fprintf(OFP,"%1.0lf %1.0lf ",center_x - radius, center_y - radius);
   fprintf(OFP,"%1.0lf %1.0lf\n",center_x + radius, center_y + radius);
   fprintf(OFP,"%1.0lf %1.0lf\n",xe, ye);
}

/*****************************************************************************
* wPADS_Graph_Line
*/
void wPADS_Graph_Line( double x1, double y1, double x2, double y2, double width )
{
   char  *l = "OPEN";
   if (cur_type[0] == 'C')  l = "COPOPN";

   if (width > 0.25) width = 0.25;

   fprintf(OFP,"LIN%ld %s %lg %lg 1\n",linecnt++,cur_type, 
      wPads_Units(output_units, x1), wPads_Units(output_units, y1));
   fprintf(OFP,"%s 2 %lg %d\n", l, wPads_Units(output_units, width), cur_level);
   fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, 0.0), wPads_Units(output_units, 0.0));
   fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, x2-x1), wPads_Units(output_units, y2-y1));
}

/*****************************************************************************
* wPADS_Graph_Line
*/
void wPADS_GraphSchem_Line( double x1, double y1, double x2, double y2, double width )
{
   if (width > 250) width = 250;

   fprintf(OFP,"LIN%ld LINES %1.0lf %1.0lf 1\n",linecnt++,x1, y1);
   fprintf(OFP,"OPEN 2 %1.0lf 255\n", width);
   fprintf(OFP,"0 0\n%1.0lf %1.0lf\n",x2-x1, y2-y1);
}

/******************************************************************************
* wPADS_Graph_Text
*/
void wPADS_Graph_Text(char *text, double x1, double y1, double height, double width,
                      double angle, int mirror, double penwidth, int header, char HJust, char VJust )
{
   if (strlen(text) == 0)
      return;

   if (penwidth == 0)
      penwidth = width / 10;

   if (header)
      fprintf(OFP, "*TEXT*       FREE TEXT\n");

   CString newText = text;
   newText.Replace('\n', '_');
   newText = newText.Left(79);

   if (PADSVERSION >= 3)
   {
      /* width = character width. */
      fprintf(OFP, "%lg %lg %1.1lf %d %lg %lg %c",
         wPads_Units(output_units, x1),
         wPads_Units(output_units, y1),
         clean_rot(RadToDeg(angle)), 
         cur_level, 
         wPads_Units(output_units, height * TEXTCORRECT), 
         wPads_Units(output_units, penwidth), 
         (mirror)?'M':'N');
      
      // Added support for writing justified text      
      switch((int)HJust)
      {
         case 2:
            fprintf(OFP, " RIGHT");
            break;
         case 1:
            fprintf(OFP, " CENTER");
            break;
         case 0:
         default:
            fprintf(OFP, " LEFT");
            break;
      }
         
      switch((int)VJust)
      {
         case '2':
            fprintf(OFP, " UP\n");
            break;
         case '1':
            fprintf(OFP, " CENTER\n");
            break;
         case 0:
         default:
            fprintf(OFP, " DOWN\n");
            break;
      }
         
      fprintf(OFP, "%s\n", newText);
   }
   else
   {
      /* width = character width. */
      // Should normalize justified text here.
      fprintf(OFP, "%lg %lg %1.1lf %d %lg %lg %c\n",
         wPads_Units(output_units, x1),
         wPads_Units(output_units, y1), 
         clean_rot(RadToDeg(angle)),
         cur_level,
         wPads_Units(output_units, height * TEXTCORRECT), 
         wPads_Units(output_units, penwidth), 
         (mirror)?'M':' ');
      
      fprintf(OFP, "%s\n", newText);
   }
}

/*****************************************************************************/
/*
*/
void wPADS_GraphSchem_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror )
{
   double penwidth = 10;

   if (strlen(text) == 0)  return;

   /* width = character width. */
   fprintf(OFP,"*TEXT*       FREE TEXT\n");
   fprintf(OFP,"%1.0lf %1.0lf %1.0lf 0 %1.0lf %1.0lf %c\n",
      x1,y1,clean_rot(RadToDeg(angle)),height*TEXTCORRECT, penwidth,(mirror)?'M':' ');
   fprintf(OFP,"%s\n",text);
   return;
}

/*****************************************************************************/
/*
*/
void wPADS_out_line(double maxdesignspace, double x1, double y1, double bulge, double x2, double y2)
{
   double da = atan(bulge) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

      int isa, ida;

      isa = round(RadToDeg(sa) * 10);
      ida = round(RadToDeg(da) * 10);

      while (isa < 0)      isa += 3600;
      while (isa >= 3600)  isa -= 3600;

      // arc exceeds design space.
      if (fabs(cx-r) > maxdesignspace || fabs(cx+r) > maxdesignspace || fabs(cy-r)> maxdesignspace || fabs(cy + r) > maxdesignspace)
      {
         fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, x1), wPads_Units(output_units, y1));
      }
      else
      {
         fprintf(OFP,"%lg %lg %d %d ", wPads_Units(output_units, x1), wPads_Units(output_units, y1), isa, ida);
         fprintf(OFP,"%lg %lg ", wPads_Units(output_units, cx - r), wPads_Units(output_units, cy - r));
         fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, cx + r), wPads_Units(output_units, cy + r));
      }
   }
   else
      fprintf(OFP,"%lg %lg\n", wPads_Units(output_units, x1), wPads_Units(output_units, y1));
   return;
}

/*****************************************************************************/
/*
*/
static void schpout_line(double x1, double y1, double bulge, double x2, double y2)
{
   double da = atan(bulge) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

      fprintf(OFP,"%1.0lf %1.0lf %d %d %d ",
         x1,y1, round(RadToDeg(sa) * 10), round(RadToDeg(da) * 10), round(RadToDeg(da) * 10));
      fprintf(OFP,"%1.0lf %1.0lf ",cx - r, cy - r);
      fprintf(OFP,"%1.0lf %1.0lf\n",cx + r, cy + r);
   }
   else
      fprintf(OFP,"%1.0lf %1.0lf\n",x1,y1);
   return;
}

/*****************************************************************************/
/*
*/
void wPADS_Graph_Polyline_V2( double maxdesignspace, Point2 *coordinates, int pairs, int filled, 
                             double width, const char *linetype )
{
   int   cnt, count, is_closed = FALSE;
   char  *l = "COPCLS";
   double x1,y1, x2,y2;
   int   i;

   if (width > 0.25) width = 0.25;

// here do correct polys

   x1 = coordinates[0].x;
   y1 = coordinates[0].y;
   x2 = coordinates[pairs-1].x;
   y2 = coordinates[pairs-1].y;

   if (fabs(x1-x2) < 0.001 && fabs(y1-y2) < 0.001)
      is_closed = TRUE;

   if (pairs > 99)   
   {
      filled = FALSE;
      is_closed = FALSE;
   }

   if (cur_type[0] == 'C')
   {
      if (is_closed)
         l = "CLOSED";
      else
         l = "OPEN";
   }

   if (filled)
   {
      cur_type = "COPPER";
      if (is_closed)
         l = "COPCLS";
      else
         l = "COPOPN";
   }
   else
   {
      cur_type = (char *)linetype;
      if (is_closed)
         l = "CLOSED";
      else
         l = "OPEN";
   }

   count = 0;
   
   int number_of_graphic = (pairs/100) +1;
   int mid_piece = 0;
   int pcnt = polycnt++;
   if (number_of_graphic > 2)
   mid_piece = number_of_graphic-2;

   fprintf(OFP,"POLY%ld %s %lg %lg %d\n",pcnt,cur_type, 
      wPads_Units(output_units, x1), 
      wPads_Units(output_units, y1), number_of_graphic+mid_piece);

   for (i=0;i<(number_of_graphic-1);i++)
   {
      cnt = 100;

      // here need to close gap from ..100 101...
      if (i > 0)
      {
         fprintf(OFP,"%s %d %lg %d\n",l,2, wPads_Units(output_units, width), cur_level);
         // here figure out bulge
         wPADS_out_line(maxdesignspace, coordinates[count-1].x - x1,coordinates[count-1].y - y1,0.0, 0, 0);
         wPADS_out_line(maxdesignspace, coordinates[count].x - x1,coordinates[count].y - y1, 0.0, 0, 0);
      }

      fprintf(OFP,"%s %d %lg %d\n",l,cnt, wPads_Units(output_units, width), cur_level);
      for (;count<(i*100+cnt);count++)
      {
         // here figure out bulge
         wPADS_out_line(maxdesignspace, coordinates[count].x - x1,coordinates[count].y - y1, coordinates[count].bulge,
            coordinates[count+1].x - x1,coordinates[count+1].y - y1);
      }

   }

   cnt = pairs % 100;
   if (cnt)
   {
      if (count)
      {
         fprintf(OFP,"%s %d %lg %d\n",l,cnt+1, wPads_Units(output_units, width), cur_level);
         // this is for mid piece
         // here figure out bulge
         wPADS_out_line(maxdesignspace, coordinates[count-1].x - x1,coordinates[count-1].y - y1,coordinates[count-1].bulge,
            coordinates[count].x - x1,coordinates[count].y - y1);
      }
      else
      {
         fprintf(OFP,"%s %d %lg %d\n",l,cnt, wPads_Units(output_units, width), cur_level);
      }
      for (;count<pairs;count++)
      {
         // here figure out bulge
         wPADS_out_line(maxdesignspace, coordinates[count].x - x1,coordinates[count].y - y1,coordinates[count].bulge,
            coordinates[count+1].x - x1,coordinates[count+1].y - y1);
      }
   }
   fprintf(OFP,"\n");

   return;
}
/*****************************************************************************/
/*
*/
void wPADS_Graph_Polyline_V3( double maxdesignspace, Point2 *coordinates, int koocnt, int filled, 
                             double width, const char *linetype, const char *piecetype,
                             const char *netname_restriction)  // can not have both
{
   int      is_closed = FALSE;
   double   x1,y1, x2,y2;
   int      i;

   if (width < 0.001)width = 0.001; // limits in PADS
   if (width > 250)  width = 250;
   // here do correct polys

   x1 = coordinates[0].x;
   y1 = coordinates[0].y;
   x2 = coordinates[koocnt-1].x;
   y2 = coordinates[koocnt-1].y;

   if (fabs(x1-x2) < small_delta && fabs(y1-y2) < small_delta)
      is_closed = TRUE;

   int pcnt = polycnt++;

   // only copper can have a netname, but not a textcnt
   if (!STRCMPI(linetype, "COPPER"))
   {
      fprintf(OFP,"POLY%ld %s %lg %lg %d %s\n", pcnt, linetype, 
         wPads_Units(output_units, x1), wPads_Units(output_units, y1), 1, netname_restriction);
   }
   else
   {
      // all other can not have a netname
      fprintf(OFP,"POLY%ld %s %lg %lg %d 0\n", pcnt, linetype, 
         wPads_Units(output_units, x1), wPads_Units(output_units, y1), 1);
   }

   fprintf(OFP,"%s %d %lg %d %s\n", piecetype, koocnt, wPads_Units(output_units, width), cur_level,
      netname_restriction);

   for (i=0;i<koocnt-1;i++)
   {
      // here figure out bulge, but only the first koo gets written.
      wPADS_out_line(maxdesignspace, coordinates[i].x - x1,coordinates[i].y - y1,coordinates[i].bulge,
         coordinates[i+1].x - x1,coordinates[i+1].y - y1);
   }
   // the last can never have a bulge, so set the +1 coo to 0,0
   wPADS_out_line(maxdesignspace, coordinates[i].x - x1,coordinates[i].y - y1,coordinates[i].bulge, 0, 0);

   fprintf(OFP,"\n");

   return;
}

/******************************************************************************
* wPADS_Graph_Polyline 
*/
void wPADS_Graph_Polyline(double maxdesignspace, Point2 *coordinates, int pairs, int filled, double width, 
      const char *linetype, const char *piecetype, const char *restriction)
{
   if (pairs < 2) 
      return;

   if (PADSVERSION >= 3)
      wPADS_Graph_Polyline_V3(maxdesignspace, coordinates, pairs, filled, width, linetype, 
            (strlen(piecetype)?piecetype:"OPEN"), restriction);
   else
      wPADS_Graph_Polyline_V2(maxdesignspace, coordinates, pairs, filled, width, linetype);
}

/*****************************************************************************/
/*
*/
void wPADS_Graph_PourPoly( Point2 *coordinates, int pairs, int filled, double width)
{
   int      i;
   int      arccount = 0;

   for (i=0;i<pairs;i++)
   {
      double da = atan(coordinates[i].bulge) * 4;
      if (fabs(da) > SMALLANGLE) arccount++;
   }

   if (width < 0.001)width = 0.001; // limits in PADS
   if (width > 250)  width = 250;

   double x1 = coordinates[0].x;
   double y1 = coordinates[0].y;

   fprintf(OFP,"POLY %d %d %lg %d\n", pairs, arccount, 
      wPads_Units(output_units, width), cur_level);

   for (i=0;i<pairs;i++)
   {
      double da = atan(coordinates[i].bulge) * 4;

      fprintf(OFP,"%lg %lg\n", 
         wPads_Units(output_units, coordinates[i].x - x1), 
         wPads_Units(output_units, coordinates[i].y - y1));

      if (fabs(da) > SMALLANGLE)
      {
         int   sdegree, deltadegree;
         double cx,cy,r,sa;
         ArcPoint2Angle(coordinates[i].x,coordinates[i].y,coordinates[i+1].x,coordinates[i+1].y,da,&cx,&cy,&r,&sa);
         sdegree = round(RadToDeg(sa)*10);

         while (sdegree < 3600)  sdegree += 3600;
         while (sdegree >= 3600) sdegree -= 3600;

         deltadegree = round(RadToDeg(da)*10);
         fprintf(OFP,"%lg %lg %d %d\n", 
            wPads_Units(output_units, cx - x1), wPads_Units(output_units, cy - y1), sdegree, deltadegree);
      }
   }

   fprintf(OFP,"\n");

   return;
}

/*****************************************************************************/
/*
*/
void wPADS_GraphSchem_Polyline( Point2 *coordinates, int pairs, int filled, double width )
{
   int count, is_closed = FALSE;
   double x1,y1, x2,y2;
   char  *l;

   if (width > 250) width = 250;

   x1 = coordinates[0].x;
   y1 = coordinates[0].y;
   x2 = coordinates[pairs-1].x;
   y2 = coordinates[pairs-1].y;

   if (fabs(x1-x2) < 1 && fabs(y1-y2) < 1)
      is_closed = TRUE;

   if (is_closed)
      l = "CLOSED";
   else
      l = "OPEN";

   int number_of_graphic = (pairs/100) +1;
   int mid_piece = 0;
   int pcnt = polycnt++;
   if (number_of_graphic > 2)
   mid_piece = number_of_graphic-2;

   fprintf(OFP,"*REMARK*     Total %d Major %d Mid %d\n",pairs,number_of_graphic, mid_piece);
   fprintf(OFP,"POLY%ld %s %lg %lg %d\n",pcnt,cur_type, 
      wPads_Units(output_units, x1), wPads_Units(output_units, y1), number_of_graphic+mid_piece);

   int   i, cnt;
   
   count = 0;
   for (i=0;i<(number_of_graphic-1);i++)
   {
      cnt = 100;

      // here need to close gap from ..100 101...
      if (i > 0)
      {
         fprintf(OFP,"%s %d %1.0lf %d\n",l,2, width, cur_level);
         // here figure out bulge
         schpout_line(coordinates[count-1].x - x1,coordinates[count-1].y - y1,0.0, 0, 0);
         schpout_line(coordinates[count].x - x1,coordinates[count].y - y1, 0.0, 0, 0);
      }

      fprintf(OFP,"%s %d %1.0lf %d\n",l,cnt, width, cur_level);
      for (;count<(i*100+cnt);count++)
      {
         // here figure out bulge
         schpout_line(coordinates[count].x - x1,coordinates[count].y - y1, coordinates[count].bulge,
            coordinates[count+1].x - x1,coordinates[count+1].y - y1);
      }

   }

   cnt = pairs % 100;
   if (cnt)
   {
      if (count)
      {
         fprintf(OFP,"%s %d %lg %d\n",l,cnt+1, wPads_Units(output_units, width), cur_level);
         // this is for mid piece
         // here figure out bulge
         schpout_line(coordinates[count-1].x - x1,coordinates[count-1].y - y1,coordinates[count-1].bulge,
            coordinates[count].x - x1,coordinates[count].y - y1);
      }
      else
      {
         fprintf(OFP,"%s %d %lg %d\n",l,cnt, wPads_Units(output_units, width), cur_level);
      }
      for (;count<pairs;count++)
      {
         // here figure out bulge
         schpout_line(coordinates[count].x - x1,coordinates[count].y - y1,coordinates[count].bulge,
            coordinates[count+1].x - x1,coordinates[count+1].y - y1);
      }
   }
   return;

   return;
}

// end PADSLIB.C

