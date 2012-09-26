// $Header: /CAMCAD/4.3/read_wrt/CdiLib.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
*
*/

#include "geomlib.h"

void wCDI_Graph_Level(const char *l);
void wCDI_Graph_Line_Width( double line_width );
int wCDI_Graph_Init();
void wCDI_Graph_File_Open( FILE *file, char *extra_header_info );
void wCDI_Graph_File_Close(double xmin, double ymin, double xmax, double ymax);

void wCDI_Graph_Circle( double center_x, double center_y, double radius, int widthcode);
void wCDI_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, int widthcode );
void wCDI_Graph_Line( double x1, double y1, double x2, double y2, int widthcode );

void wCDI_Graph_Text( char *text, double x1, double y1,int code,
               double textheight, double angle, int mirror );
void wCDI_Graph_Polyline( Point2 *coordinates, int pairs, int filled, int widthcode);

// end CDILIB.H
