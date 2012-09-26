// $Header: /CAMCAD/4.3/read_wrt/TanLib.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
*
*/

#include "stdio.h"
#include "geomlib.h"

void wtan_Graph_Level(const char *l);
void wtan_Graph_Width( double line_width );

int wtan_Graph_Init();
void wtan_Graph_File_Open( FILE *file);

int wtan_Graph_Init_Layout();
void wtan_Graph_File_Open_Layout( FILE *file);

void wtan_Graph_File_Close_Layout();
void wtan_Graph_Aperture(double x, double y, double sizea, double sizeb, int form, double rot);

void wtan_Graph_Circle( double center_x, double center_y, double radius,double width);

void wtan_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width );

void wtan_Graph_Line( double x1, double y1, double x2, double y2, double width );

void wtan_Graph_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror );
void wtan_Graph_Polyline( Point2 *coordinates, int pairs, int filled);

// end TANLIB.H
