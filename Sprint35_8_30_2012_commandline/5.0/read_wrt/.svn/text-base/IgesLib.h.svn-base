// $Header: /CAMCAD/4.5/read_wrt/IgesLib.h 9     9/29/05 7:57p Rick Faltersack $

/***************************************************************************
* IGESLIB.H
*/

#include "geomlib.h"

void wiges_Graph_Line_End(int onoff);
void wiges_Graph_Line_Width(double width);
int  wiges_Graph_File_Open(FILE *wfp, char *npe);
int  wiges_Graph_File_Close();
int  wiges_Graph_Polyline(Point2 *coos, int cnt, double z, int filled);
int  wiges_Graph_Fill_Polyline(Point2 *coos, int cnt, double z, int filled);
int  wiges_Graph_Init(int unitsFlag);
void wiges_Graph_Level(const char *level);
int  wiges_Graph_Aperture( int type, double x, double y, double sizeA, double sizeB);
int  wiges_Graph_Arc(double center_x, double center_y, double center_z,
                              double radius, double startangle,double deltaangle);
int  wiges_Graph_Circle(double center_x, double center_y,
                                 double center_z, double radius);
int  wiges_Graph_Block_On(const char *block_name);
int  wiges_Graph_Block_Off( );
int  wiges_Graph_Block_Reference(const char *block_name, double x1, double y1,
                                        double z1, double angle, int mirror,
                                        double scale);
int  wiges_Graph_Block_Reference_Layer(const char *layername,
                             const char *block_name, double x1, double y1,
                             double angle, int mirror, double scale);
int  wiges_Graph_Line(double x1, double y1, double z1,
                           double x2, double y2, double z2);
int  wiges_Graph_Text(char *text, double x1, double y1, double z1,
                              double height, double width, double angle, int mirror);
// end IGESLIB.H
