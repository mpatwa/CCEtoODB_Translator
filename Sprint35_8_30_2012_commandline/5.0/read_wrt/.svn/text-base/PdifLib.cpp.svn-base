// $Header: /CAMCAD/4.5/read_wrt/PdifLib.cpp 9     1/27/05 7:43p Kurt Van Ness $

/*****************************************************************************/
/*    
   Output for PDIF-Files
*/

#include "StdAfx.h"
#include "General.h"
#include "math.h"
#include "pdiflib.h"

#define  MAX_LABEL   2000

typedef struct
{
   char  *text;
   double x,y;
   double height;
   char   mirror;
   int    rotation;  // in degree
   char  *layername;
}Label;

static FILE    *OFP = NULL;   /*Output Filepointer*/
static int     ActPenNr;      /*Actual Pennumber...*/
static char    cur_layer[80];
static char    ident[80];
static int     identcnt = 0;
static double  cur_width = -1;
static int     compdefcnt = 0;

static Label   *label;
static int     labelcnt;

/* The Functions and Procedures ...*/
static void init();
static void deinit();

/****************************************************************************/
/*
*/
int   wpdif_plusident()
{
   if (identcnt < 80)
   {
      ident[identcnt] = ' ';
      ident[++identcnt] = '\0';
   }
   else
   {
      //printf("Error in plus ident\n");
      exit(1);
   }
   return 1;
}

int   wpdif_minusident()
{
   if (identcnt)
   {
      ident[--identcnt] = '\0';
   }
   else
   {
      //printf("Error in minus ident\n");
      exit(1);
   }
   return 1;
}

void wpdif_Graph_Width(double w)
{
   if (cur_width == w)  return;

   if (w > 255)   w = 255; // Pcad has limit of 255 mil 
   fprintf(OFP,"%s[Wd %1.2lf]\n",ident,w);
   cur_width = w;
   return;
}

void init()
{
   compdefcnt = 0;
   ActPenNr=255;
   ident[0] = '\0';
   identcnt = 0;
   cur_width = -1;

   if ((label = (Label *)calloc(MAX_LABEL,sizeof(Label))) == NULL)
   {
         MessageBox(NULL,"Out of Memory","dxflib",MB_ICONEXCLAMATION | MB_OK);
         exit(1);
   }
   labelcnt = 0;
}

void deinit()
{
   int i;

   for (i=0;i<labelcnt;i++)
   {
      free(label[i].text);
      free(label[i].layername);
   }
   free(label);
   labelcnt = 0;
}

void wpdif_Graph_Level(const char *l)
{
   if (l == NULL) return;
   if (STRCMPI(cur_layer,l))
   {
     strcpy(cur_layer,l);
     fprintf(OFP,"%s[Ly \"%s\"]\n",ident,l);
   }
}

int wpdif_Graph_Init()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

void wpdif_Graph_File_Open( FILE *file)
{
   OFP=file;
}

void wpdif_Graph_File_Open_Layout( FILE *file, char *extra_header_info )
{
   OFP=file;

   fprintf(OFP,"{COMPONENT %s.pcb\n",extra_header_info);
   wpdif_plusident();

}

void wpdif_Graph_File_Open_Schematic( FILE *file, char *extra_header_info )
{
   OFP=file;

   fprintf(OFP,"{COMPONENT %s.sch\n",extra_header_info);
   wpdif_plusident();

}

void wpdif_Graph_File_Close()
{

   wpdif_minusident(); // COMPONENTS
   fprintf(OFP,"%s}\n",ident);

   deinit();      /*deinitialize the Memory for
              the Layers ...*/
}

void wpdif_Graph_Aperture(double x, double y, int dcode)
{
   fprintf(OFP,"%s{Fl %1.2lf %1.2lf %d 0}\n",ident,x,y,dcode);
}

void wpdif_Graph_Circle( double center_x, double center_y, double radius, double width )
{
   wpdif_Graph_Width(width);
   fprintf(OFP,"%s{C %1.2lf %1.2lf %1.2lf}\n",ident,center_x, center_y, radius);

   return;
}

/****************************************************************************
* wpdif_Graph_Arc
*/
void wpdif_Graph_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle, double width )
{
   double sx,sy,ex,ey;

   sx = cos(startangle) * radius + center_x;
   sy = sin(startangle) * radius + center_y;
   ex = cos(startangle + deltaangle) * radius + center_x;
   ey = sin(startangle + deltaangle) * radius + center_y;

   if (width > -1)   // arc in poly does not want width again.
      wpdif_Graph_Width(width);

   if (deltaangle < 0)
      fprintf(OFP,"%s{Arc %1.2lf %1.2lf %1.2lf %1.2lf %1.2lf %1.2lf}\n",ident,
         center_x, center_y, ex, ey, sx, sy);
   else
      fprintf(OFP,"%s{Arc %1.2lf %1.2lf %1.2lf %1.2lf %1.2lf %1.2lf}\n",ident,
         center_x, center_y, sx, sy, ex, ey);
}

/*****************************************************************************
* wpdif_Graph_Line
*/
void wpdif_Graph_Line( double x1, double y1, double x2, double y2, double width )
{
   wpdif_Graph_Width(width);
   fprintf(OFP,"%s{L %1.2lf %1.2lf %1.2lf %1.2lf}\n",ident,x1,y1,x2,y2);
   return;
}

/*****************************************************************************
* 
   PCCARDS can not rotate text other than 90 degrees. to overcome this, i make
   and pseudo comp.
*/
void wpdif_Write_OddAngleText()
{
   int   i;

   wpdif_Write_Line("{SUBCOMP");
   wpdif_plusident();

   for (i=0;i<labelcnt;i++)
   {
      fprintf(OFP,"// Real text %s on %s\n",label[i].text,label[i].layername);
      fprintf(OFP,"%s{COMP_DEF XL%05d.prt\n",ident,++compdefcnt);
      wpdif_plusident();

      fprintf(OFP,"%s{PIC\n",ident);
      wpdif_plusident();
      wpdif_Graph_Level(label[i].layername);
      wpdif_Graph_Text( label[i].text, 0.0, 0.0,
                        label[i].height, 0.0, 0.0,(int)label[i].mirror );

      wpdif_minusident();  // end of pic
      wpdif_Write_Line("}");
 
      wpdif_minusident();  // end of compdef
      wpdif_Write_Line("}");
      
      fprintf(OFP,"%s{I XL%05d.prt XC%05d\n",ident,compdefcnt,compdefcnt);
      wpdif_plusident();
     
    
      fprintf(OFP,"%s{ATR\n",ident);
      wpdif_plusident();
      fprintf(OFP,"%s{IN\n",ident);
      wpdif_plusident();
      fprintf(OFP,"%s{Pl %1.2lf %1.2lf}\n",ident,label[i].x,label[i].y);
      fprintf(OFP,"%s{Ro %d}\n",ident,label[i].rotation / 90);
      fprintf(OFP,"%s{Pa %d}\n",ident,label[i].rotation % 90);
      wpdif_minusident();  // end of In
      wpdif_Write_Line("}");
      wpdif_minusident();  // end of Atr
      wpdif_Write_Line("}");
   
      wpdif_minusident();  // end of compdef
      wpdif_Write_Line("}");
   }

   wpdif_minusident();
   wpdif_Write_Line("}");

   return;
}

/*****************************************************************************
*
*/
void wpdif_Graph_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror )
{
   unsigned int   i;
   int   trot = round(RadToDeg(angle) / 90);  // allows only 90 degree angles
   int   realrot = round(RadToDeg(angle));

   if (strlen(text) == 0)  return;

   // here if rotation is other than 90 store in label
   if (realrot != trot * 90)
   {
      if (labelcnt < MAX_LABEL)
      {
         if ((label[labelcnt].text = STRDUP(text)) == NULL)
         {
            MessageBox(NULL,"Out of Memory","dxflib",MB_ICONEXCLAMATION | MB_OK);
            exit(1);
         }
         if ((label[labelcnt].layername = STRDUP(cur_layer)) == NULL)
         {
            MessageBox(NULL,"Out of Memory","dxflib",MB_ICONEXCLAMATION | MB_OK);
            exit(1);
         }
         label[labelcnt].x = x1;
         label[labelcnt].y = y1;
         label[labelcnt].height = height;
         label[labelcnt].mirror =   mirror;
         label[labelcnt].rotation = realrot;
         labelcnt++;
      }
   }
   else
   {

      // Ts Tj Tr Tm
      fprintf(OFP,"%s[Ts %1.2lf][Tj \"%s\"][Tr %d][Tm \"%c\"]\n",ident,height,"LB",
         trot,(mirror)?'M':'N');
      fprintf(OFP,"%s{T \"",ident);
      for (i=0;i<strlen(text);i++)
      {
         // here check for valid chars
         switch (text[i])
         {
            case ' ':   fprintf(OFP,"\\b");  break;
            case '(':   fprintf(OFP,"\\(");  break;
            case ')':   fprintf(OFP,"\\)");  break;
            case '{':   fprintf(OFP,"\\{");  break;
            case '}':   fprintf(OFP,"\\}");  break;
            case '[':   fprintf(OFP,"\\[");  break;
            case ']':   fprintf(OFP,"\\]");  break;
            case '"':   fprintf(OFP,"\\\"");  break;
            default:    fprintf(OFP,"%c",text[i]); break;
         }
      }
      fprintf(OFP,"\" %1.2lf %1.2lf}\n",x1,y1);
   }
}

void wpdif_Graph_Polyline( Point2 *coordinates, int pairs, int filled )
{
   int count;

   if (filled)
   {

      fprintf(OFP,"%s{Poly {Ol 1 ",ident);
      for (count=0;count<pairs;count++)
      {
         double da = atan(coordinates[count].bulge) * 4;

         if (fabs(da) > SMALLANGLE && count < (pairs-1))
         {
            Point2 *ppoly;
            int    ppolycnt;
            // need to convert Arcs to polylines and
            // then continue;
            // here init buffer for Polyline
            if ((ppoly = (Point2 *)calloc(255,sizeof(Point2))) == NULL)
            {
               return;
            }
            ppolycnt = 255;
            // start center
            double cx,cy,r,sa;
            ArcPoint2Angle(coordinates[count].x,coordinates[count].y,
                           coordinates[count+1].x,coordinates[count+1].y,da,&cx,&cy,&r,&sa);
            // make positive start angle.
            if (sa < 0) sa += PI2;
            Point2 c;
            c.x = cx; c.y = cy;

            ArcPoly2(coordinates[count].x,coordinates[count].y,
               cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(5));

            for (int i=0;i<ppolycnt;i++)
               fprintf(OFP,"%s %1.2lf %1.2lf\n",ident,ppoly[i].x,ppoly[i].y);

            free(ppoly);            
         }
         else
         {
            fprintf(OFP,"%s %1.2lf %1.2lf\n",ident,
               coordinates[count].x,coordinates[count].y);
         }
      }
      fprintf(OFP,"%s} }\n",ident);
   }
   else
   {
      // arcs and lines
      int   line_start = TRUE;
      for (count=0;count<pairs;count++)
      {
         double da = atan(coordinates[count].bulge) * 4;

         if (fabs(da) > SMALLANGLE && count < (pairs-1))
         {
            if (!line_start)
            {
               // need to close line. if started,
               fprintf(OFP,"%s}\n",ident);
            }
            // do arc
            double cx,cy,r,sa;
            ArcPoint2Angle(coordinates[count].x,coordinates[count].y,
               coordinates[count+1].x,coordinates[count+1].y,da,&cx,&cy,&r,&sa);
            wpdif_Graph_Arc( cx, cy, r, sa, da, -1 );

            // make sure next time line is opened.
            line_start = TRUE;
         }
         else
         {
            if (line_start)
               fprintf(OFP,"%s{L",ident);
            line_start = FALSE;
            fprintf(OFP,"%s %1.2lf %1.2lf\n",ident,
               coordinates[count].x,coordinates[count].y);
         }
      }
      fprintf(OFP,"%s}\n",ident);
   }
}

void wpdif_Write_Line(char *line)
{
   fprintf(OFP,"%s%s\n",ident,line);
}

// end PDIFLIB.C

