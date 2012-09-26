// $Header: /CAMCAD/4.3/read_wrt/MenLib.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
*
*/

#include "stdio.h"
#include "geomlib.h"

void wmen_Graph_Date( FILE *file,const char *rem);

void wmen_Graph_Level(const char *l);
void wmen_Graph_Width( double line_width );

int wmen_Graph_Init(int output_units_accuracy, double men_smallnumber, const char *unit_string);
void wmen_Graph_File_Open( FILE *file);

void wmen_Graph_File_Open_Layout( FILE *file);

void wmen_Graph_Block_On(const char *fname);

void wmen_Graph_CreateGeneric_On(const char *fname);
void wmen_Graph_CreateBoard_On(const char *fname);
void wmen_Graph_CreatePin_On(const char *fname);
void wmen_Graph_CreateComponent_On(const char *fname);
void wmen_Graph_CreateStackup_On(const char *fname);
void wmen_Graph_CreateVia_On(const char *fname);

void wmen_Graph_File_Close_Layout();
void wmen_Graph_Aperture(double x, double y, double sizea, double sizeb, 
                         int form, double rot);

void wmen_Graph_Circle( double center_x, double center_y, double radius,double width);

void wmen_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width );

void wmen_Graph_Line( double x1, double y1, double x2, double y2, double width );

void wmen_Graph_Text( char *text, double x1, double y1,
      double height, double width, double strokewidth, double angle, int mirror );
void wmen_Graph_Polyline( Point2 *coordinates, int pairs, int filled, int closed);
void wmen_Graph_Attribute(const char *layer, Point2 *coordinates, int pairs);

int  wmen_Graph_Block_Reference(const char *block_name, double x1, double y1,
                                 double angle, int mirror,
                                 double scale,int attflg );

// end MENLIB.H
