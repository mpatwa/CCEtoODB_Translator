// $Header: /CAMCAD/4.3/read_wrt/BarcoLib.h 6     8/12/03 9:06p Kurt Van Ness $

/*******************************************************************************
*
*/

#include "geomlib.h"
#include "format_s.h"

int  wbarco_Graph_File_Open(const char *out_file);
void wbarco_Graph_File_Close();
void wbarco_Flash_App(Point2 point, int app_num);
void wbarco_Line(Point2 start, Point2 end, int app_num,
                           int apshape, double width);
void wbarco_Arc(Point2 center, double radius, double startradians, 
               double deltaradians, int app_num);
void wbarco_Circle(Point2 center, double radius,int app_num);
void wbarco_write2file(const char *prosa);
int  wbarco_Graph_Polyline( Point2 p, int closed, int filled, int docde, int apshape);
int  wbarco_Graph_Vertex(Point2 p);

int  wbarco_Graph_Text( char *text, double x1, double y1,
                        double height, double width, double angle, 
                        int mirror, int oblique);

// end BarcoLIB.H
