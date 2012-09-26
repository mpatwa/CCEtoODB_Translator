// $Header: /CAMCAD/4.3/read_wrt/HpglLib.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
* HPGLLIB.H
*/

#include "stdio.h"
#include "geomlib.h"

void whpgl_Graph_Level( int penNum );
void whpgl_Graph_Line_Width( double line_width );
void whpgl2_PenWidth(double puwidth);
int  whpgl_Graph_Init(int hpgl2);
void whpgl_Graph_File_Open( FILE *file);
void whpgl_Graph_File_Close();
void whpgl_Graph_Aperture(int type, double x, double y, double sizeA, double sizeB, 
                          double rotation, double xoffset, double yoffset, double penWidth);
void whpgl_Graph_Circle( double center_x, double center_y, double radius );
void whpgl_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle );
void whpgl_Graph_Arc_with_Width( double center_x, double center_y, double radius,
               double startangle, double deltaangle,
               double lineWidth, double penWidth, int RoundEnd );
void whpgl_Graph_Line( double x1, double y1, double x2, double y2 );
void whpgl_Graph_Line_with_Width( double x1, double y1, double bulge1,
               double x2, double y2, double bulge2,
               double lineWidth, double penWidth, int RoundEnd );
void whpgl_Graph_Point( double x1, double y1 );
void whpgl_Graph_Text(const char *text, double x1, double y1,
               double height, double width, double angle, int slantangle, int mirror );
void whpgl_Graph_Polyline( Point2 p, int filled);
void whpgl_Graph_Vertex(Point2 p);
void whpgl_Graph_PolyClose(int filled);
// end HPGLLIB.H
