// $Header: /CAMCAD/4.5/read_wrt/MenLib.cpp 11    5/20/05 5:20p Lynn Phung $

/****************************************************************************/
/*
   Output for MENTOR-Files Version 8

In Mentor librarian we can have minimum .0001 inch
grid OR .00001 mm grid. Below this prcision lost.

It is essential to have some work around conversion
of values from inches to mm or vice versa.

We can use highly precise values in Librarian. But
if same pricision level used in layout / fablink, 
creates problems like memory requirement,large size
of GERBER files, defining apertures etc.

Please have a look to example given in below table:

Conversion         tol.    Conversion         tol.
inches to mm      0.01mm   mm to inches       0.001"
- ----------------------------------------------------
.001 = .0254      0.025     0.025 = .000984   0.001
- ----------------------------------------------------
3.333 = 84.6582   84.66    84.66 = 3.3330709  3.333
- ----------------------------------------------------
2.021 = 51.3334   51.33    51.33 = 2.0208661  2.021
- ----------------------------------------------------

*/

#include "stdafx.h"
#include "ccdoc.h"
#include "math.h"
#include "menlib.h"
#include "CCEtoODB.h"

#define  MAX_POLY       1000     // this is only needed for arc2poly function


static FILE    *OFP = NULL;   /*Output Filepointer*/
static char    cur_layer[80];
static double  cur_width = -1;
static int     output_units_accuracy;
static CString output_units;
static double  MEN_SMALLNUMBER;

/* The Functions and Procedures ...*/
static void init();
static void deinit();

/****************************************************************************/
/*
*/
void wmen_Graph_Width(double w)
{
   cur_width = w;
   return;
}

/****************************************************************************/
/*
*/
void init()
{
}

/****************************************************************************/
/*
*/
void deinit()
{
}

/****************************************************************************/
/*
*/
void wmen_Graph_Level(const char *l)
{
   if (l == NULL) return;
   strcpy(cur_layer,l);
}

/****************************************************************************/
/*
*/
int wmen_Graph_Init(int out_units, double men_smallnumber, const char *unit_string)
{
   output_units_accuracy = out_units;
   output_units = unit_string;
   MEN_SMALLNUMBER = men_smallnumber;
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

/****************************************************************************/
/*
*/
void wmen_Graph_File_Open( FILE *file)
{
   OFP=file;
}

/****************************************************************************/
/*
*/
void wmen_Graph_Date( FILE *file, const char *rem)
{
   char *months[] = { "January", "February", "March", "April", "May", "June", "July", 
      "August", "September", "October", "November", "December" };
   char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

   CTime time = CTime::GetCurrentTime();
   int hour = time.GetHour();

   fprintf(file,"%s date :%s, %s %d, %d at %d:%02d%s\n",rem,
         days[time.GetDayOfWeek()-1], months[time.GetMonth()-1], 
         time.GetDay(), time.GetYear(), (hour>12 ? hour-12 : hour), time.GetMinute(),
         (hour>11 ? "pm" : "am"));
   return;
}

/****************************************************************************/
/*
*/
void wmen_Graph_File_Open_Layout( FILE *file)
{
   //CString camcad;
   //camcad.LoadString(IDR_MAINFRAME);
   OFP=file;
   fprintf(OFP,"// write ascii -all\n");
   fprintf(OFP,"// created by %s\n", getApp().getCamCadTitle());
   wmen_Graph_Date(OFP,"//");
   fprintf (OFP,"// To proceed on load errors : $abort_enable = @false ;\n//\n");

   fprintf (OFP,"$abort_enable = @false;\n"); // only one $ sign.
   fprintf (OFP,"$$lock_windows(@on) ;\n");
   fprintf (OFP,"$$snap_diagonal(@off) ;\n");
   fprintf (OFP,"$$snap_orthogonal(@off) ;\n");
   // do not do this.
   // fprintf (OFP,"$$delete_parts(, @all, @force) ;\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_Block_On(const char *fname)
{
   fprintf(OFP,"$$create_generic_part(\"%s\", @replace);\n",fname);
   fprintf(OFP,"$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, 'E$%s']);\n",
      output_units, fname);
   fprintf(OFP,"$$point_mode(@vertex);\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_CreateGeneric_On(const char *fname)
{
   fprintf(OFP,"$$create_generic_part(\"%s\", @replace);\n",fname);
   fprintf(OFP,"$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, 'E$%s']);\n",
      output_units, fname);
   fprintf(OFP,"$$point_mode(@vertex);\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_CreateBoard_On(const char *fname)
{
   fprintf(OFP,"$$create_board(\"%s\", @replace);\n",fname);
   fprintf(OFP,"$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, 'BO$%s']);\n",
      output_units, fname);
   fprintf(OFP,"$$point_mode(@vertex);\n");
}

/******************************************************************************
* wmen_Graph_CreatePin_On
*/
void wmen_Graph_CreatePin_On(const char *fname)
{
   fprintf(OFP, "$$create_pin(\"%s\", @replace);\n", fname);
   fprintf(OFP, "$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, 'PI$%s']);\n", output_units, fname);
   fprintf(OFP, "$$point_mode(@vertex);\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_CreateComponent_On(const char *fname)
{
   fprintf(OFP,"$$create_component(\"%s\", @replace);\n",fname);
   fprintf(OFP,"$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, 'CO$%s']);\n",
      output_units, fname);
   fprintf(OFP,"$$point_mode(@vertex);\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_CreateStackup_On(const char *fname)
{
   fprintf(OFP,"$$create_stackup(\"%s\", @replace);\n",fname);
   fprintf(OFP,"$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, '$E%s']);\n",
      output_units, fname);
   fprintf(OFP,"$$point_mode(@vertex);\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_CreateVia_On(const char *fname)
{
   fprintf(OFP,"$$create_via(\"%s\", @replace);\n",fname);
   fprintf(OFP,"$$page(0.0,0.0,0.0, @%s, 0.0, 0.0, [0.0, 0.0, 'VI$%s']);\n",
      output_units, fname);
   fprintf(OFP,"$$point_mode(@vertex);\n");
}

/****************************************************************************/
/*
*/
void wmen_Graph_File_Close_Layout()
{
   fprintf(OFP,"// end prt file\n");
   deinit();      /*deinitialize the Memory for the Layers ...*/
}

/******************************************************************************
* wmen_Graph_Aperture
*/
void wmen_Graph_Aperture(double x, double y, double sizea, double sizeb, int form, double rot)
{
   Point2 box[5];
   memset(&box, 0, 5*sizeof(Point2));

   switch (form)
   {
   case T_OBLONG:
   case T_RECTANGLE:
      {
         cur_width = 0.001;
         box[0].x = x - sizea/2;
         box[0].y = y - sizeb/2;
         box[1].x = x + sizea/2;
         box[1].y = y - sizeb/2;
         box[2].x = x + sizea/2;
         box[2].y = y + sizeb/2;
         box[3].x = x - sizea/2;
         box[3].y = y + sizeb/2;
         box[4].x = x - sizea/2;
         box[4].y = y - sizeb/2;

         // here rotate
         for (int i=0; i<5; i++)
         {
            double newX, newY;
            Rotate(box[i].x, box[i].y, RadToDeg(rot), &newX, &newY);
            box[i].x = newX;
            box[i].y = newY;
         }
         wmen_Graph_Polyline(box, 5, 1, 1); // filled path
      }
      break;

   case T_SQUARE:
      {
         cur_width = 0.001;
         box[0].x = x - sizea/2;
         box[0].y = y - sizea/2;
         box[1].x = x + sizea/2;
         box[1].y = y - sizea/2;
         box[2].x = x + sizea/2;
         box[2].y = y + sizea/2;
         box[3].x = x - sizea/2;
         box[3].y = y + sizea/2;
         box[4].x = x - sizea/2;
         box[4].y = y - sizea/2;

         // here rotate
         for (int i=0; i<5; i++)
         {
            double newX, newY;
            Rotate(box[i].x, box[i].y, RadToDeg(rot), &newX, &newY);
            box[i].x = newX;
            box[i].y = newY;
         }
         wmen_Graph_Polyline(box, 5, 1, 1); // filled path
      }
      break;

   case T_COMPLEX:
         // this is done outside.
      break;

   default:
      // unknown form is CIRCLE
      wmen_Graph_Circle(x, y, sizea/2, 0.00);
      break;
   }

   return;
}

/****************************************************************************/
/*
*/
int  wmen_Graph_Block_Reference(const char *block_name, double x1, double y1,
                                 double angle, int mirror,
                                 double scale,int attflg )
{
   int   r = round(RadToDeg(angle) / 45);
   int   rot;
   int   mir = 1;

   
   if (mirror) mir = 2;

   switch (r % 8)
   {
      case 0:  // 0
         if (mirror)
            rot = 2;
         else
            rot = 1;
      break;
      case 1:  // 45
         if (mirror)
            rot = 13;
         else
            rot = 9;
      break;
      case 2:  // 90
         if (mirror)
            rot = 5;
         else
            rot = 6;
      break;
      case 3:  // 135
         if (mirror)
            rot = 11;
         else
            rot = 15;
      break;
      case 4:  // 180
         if (mirror)
            rot = 3;
         else
            rot = 4;
      break;
      case 5:  // 225
         if (mirror)
            rot = 16;
         else
            rot = 12;
      break;
      case 6:  // 270
         if (mirror)
            rot = 8;
         else
            rot = 7;
      break;
      case 7:  // 315
         if (mirror)
            rot = 10;
         else
            rot = 14;
      break;
   }
   // add
   fprintf(OFP,"$$add(\"%s\", %1.*lf, %1.*lf, \"%s\", %d, 1, %d);\n",
      block_name, output_units_accuracy, x1, output_units_accuracy, y1,block_name,rot, mir);
   return 1;
}

/******************************************************************************
* wmen_Graph_Circle
*/
void wmen_Graph_Circle(double center_x, double center_y, double radius, double width)
{
   fprintf(OFP,"$$circle( \"%s\", %1.*lf, %1.*lf, %1.*lf, %1.*lf); \n",
         cur_layer, output_units_accuracy, center_x, output_units_accuracy, center_y, 
         output_units_accuracy, radius*2,  output_units_accuracy, width);
}

/****************************************************************************/
/* 
   wmen_Graph_Arc
*/
void wmen_Graph_Arc( double xc, double yc, double radius,
                 double startarc, double deltaarc, double width )
{
   double sx,sy, mx,my, ex,ey;

   if (radius < MEN_SMALLNUMBER)  return;

   sx = xc + radius * cos (startarc);
   sy = yc + radius * sin (startarc);

   ex = xc + radius * cos (startarc+deltaarc);
   ey = yc + radius * sin (startarc+deltaarc);

   mx = xc + radius * cos (startarc+deltaarc/2);
   my = yc + radius * sin (startarc+deltaarc/2);

   fprintf(OFP,"$$arc( \"%s\", %1.*lf, %1.*lf, %1.*lf, %1.*lf, %1.*lf, %1.*lf, %1.*lf); \n",
      cur_layer, output_units_accuracy, sx, output_units_accuracy, sy, 
      output_units_accuracy, mx, output_units_accuracy, my, 
      output_units_accuracy, ex, output_units_accuracy, ey, 
      output_units_accuracy, width);
   return;
}

/*****************************************************************************/
/* 
   wmen_Graph_Line
*/
void wmen_Graph_Line( double x1, double y1, double x2, double y2, double width )
{
   // do not write null segments
   if (fabs(x2-x1) < MEN_SMALLNUMBER && fabs(y2-y1) < MEN_SMALLNUMBER) return;

   fprintf(OFP,"$$path( \"%s\", %1.*lf , , [%1.*lf, %1.*lf, %1.*lf, %1.*lf] );\n",
      cur_layer, output_units_accuracy, width, 
      output_units_accuracy, x1, output_units_accuracy, y1,
      output_units_accuracy, x2, output_units_accuracy, y2);
   return;
}

/****************************************************************************/
/*
*/
void wmen_Graph_Text( char *text, double x, double y,
      double height, double width, double strokewidth, double angle, int mirror )
{
   unsigned int   i;
   double rot = RadToDeg(angle);
   char  justx = 'l';
   char  justy = 'b';
   double   ratio = width / height;
   
   if (strlen(text) == 0)  return;

   fprintf(OFP,"$$text(\"%s\", \"",cur_layer);

   for (i=0;i<strlen(text);i++)
   {
      if (text[i] == '\n')
      {
         fprintf(OFP,"\\n");
      }
      else
      {
         if (text[i] == '"')  fprintf(OFP,"\\");
         if (text[i] == '\\')  fprintf(OFP,"\\");
         fprintf(OFP,"%c",text[i]);
      }
   }

   fprintf(OFP,"\", %1.*lf, %1.*lf, %1.*lf, @%c%c,%1.0lf, %1.2lf , %1.*lf,\"std\",\"none\",0,0,,%s);\n",
          output_units_accuracy, x, output_units_accuracy, y, 
          output_units_accuracy, height,justy,justx,
          rot, ratio, output_units_accuracy, strokewidth, (mirror) ? "@mirror" : "@nomirror");
   return;
}

/****************************************************************************/
/*
*/
void wmen_Graph_Polyline( Point2 *coordinates, int pairs, int filled, int closed)
{
   int   i;

   // here also look at bulges
   if (closed && cur_width < MEN_SMALLNUMBER)      // in Mentor there the Polyline is used for filled and closed
   {
      // do not close polygon.
      //if (fabs(coordinates[0].x-coordinates[pairs-1].x) < 0.0001 &&
      //  fabs(coordinates[0].y-coordinates[pairs-1].y) < 0.0001)
      // pairs--;

      fprintf(OFP,"$$initial([ %1.*lf, %1.*lf], ,@nosnap);\n",
         output_units_accuracy, coordinates[0].x, output_units_accuracy, coordinates[0].y);

      for (i=1;i<pairs;i++)
      {
         double da = atan(coordinates[i-1].bulge) * 4;
         if (fabs(da) > SMALLANGLE)
         {
            double cx,cy,r,sa;
            double x1,y1,x2,y2;   

            x1 = coordinates[i-1].x;
            y1 = coordinates[i-1].y;
            x2 = coordinates[i].x;
            y2 = coordinates[i].y;

            ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
            // make positive start angle.
            if (sa < 0) sa += PI2;
            // here arc to poly
            int ii, ppolycnt = MAX_POLY;
            // start center
            Point2 *ppoly = (Point2 *)calloc(MAX_POLY,sizeof(Point2));

            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(15));
            for (ii=0;ii<ppolycnt;ii++)
               fprintf(OFP,"$$terminal([%1.*lf, %1.*lf]);\n",
                  output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);

            free(ppoly);
         }
         else
         {
            fprintf(OFP,"$$terminal([%1.*lf, %1.*lf]);\n",
               output_units_accuracy, coordinates[i].x, output_units_accuracy, coordinates[i].y);
         }
      }
      fprintf(OFP,"$$polygon(\"%s\");\n",cur_layer);
   }
   else
   {
      for (i=1;i<pairs;i++)
      {
         double da = atan(coordinates[i-1].bulge) * 4;
         if (fabs(da) > SMALLANGLE)
         {
            double cx,cy,r,sa;
            double x1,y1,x2,y2;

            x1 = coordinates[i-1].x;
            y1 = coordinates[i-1].y;
            x2 = coordinates[i].x;
            y2 = coordinates[i].y;

            ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
            // make positive start angle.
            if (sa < 0) sa += PI2;
            wmen_Graph_Arc( cx,cy, r, sa, da, cur_width );
         }
         else
         {
            wmen_Graph_Line( coordinates[i-1].x, coordinates[i-1].y,
                       coordinates[i].x, coordinates[i].y, cur_width ) ;
         }
      }
   }
}

/****************************************************************************/
/*
*/
void wmen_Graph_Attribute(const char *l, Point2 *coordinates, int pairs)
{
   int   i;

   if (fabs(coordinates[0].x-coordinates[pairs-1].x) < MEN_SMALLNUMBER &&
       fabs(coordinates[0].y-coordinates[pairs-1].y) < MEN_SMALLNUMBER)
      pairs--;

   fprintf(OFP,"$$initial([ %1.*lf, %1.*lf], ,@nosnap);\n",
        output_units_accuracy, coordinates[0].x, output_units_accuracy, coordinates[0].y);

   for (i=1;i<pairs;i++)
   {
      double da = atan(coordinates[i-1].bulge) * 4;
      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;   

         x1 = coordinates[i-1].x;
         y1 = coordinates[i-1].y;
         x2 = coordinates[i].x;
         y2 = coordinates[i].y;

         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         // here arc to poly
         int ii, ppolycnt = MAX_POLY;
         // start center
         Point2 *ppoly = (Point2 *)calloc(MAX_POLY,sizeof(Point2));
         ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(15));
         for (ii=0;ii<ppolycnt;ii++)
            fprintf(OFP,"$$terminal([%1.*lf, %1.*lf]);\n",
               output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);

         free(ppoly);
      }
      else
         fprintf(OFP,"$$terminal([%1.*lf, %1.*lf]);\n",
            output_units_accuracy, coordinates[i].x, output_units_accuracy, coordinates[i].y);
   }
   fprintf(OFP,"$$attribute(\"%s\", \"%s\", @mark, @scale );\n",l, cur_layer);
}

// end MENLIB.CPP

