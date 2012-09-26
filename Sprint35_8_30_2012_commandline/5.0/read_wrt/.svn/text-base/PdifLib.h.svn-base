// $Header: /CAMCAD/4.3/read_wrt/PdifLib.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
* PDIFLIB.H
*/

#include "stdio.h"
#include "geomlib.h"

void wpdif_Graph_Level(const char *l);
void wpdif_Graph_Width(double line_width);

int wpdif_Graph_Init();
void wpdif_Graph_File_Open(FILE *file);

void wpdif_Graph_File_Open_Layout(FILE *file, char *extra_header);
void wpdif_Graph_File_Open_Schematic(FILE *file, char *extra_header);


void wpdif_Graph_File_Close();
void wpdif_Graph_Aperture(double x, double y, int dcode);

void wpdif_Graph_Circle(double center_x, double center_y, double radius,double width);

void wpdif_Graph_Arc(double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width);

void wpdif_Graph_Line(double x1, double y1, double x2, double y2, double width);

void wpdif_Graph_Text(char *text, double x1, double y1,
                       double height, double width, double angle, int mirror);
void wpdif_Graph_Polyline( Point2 *coordinates, int pairs, int filled);
void wpdif_Write_Line(char *l);

void wpdif_Write_OddAngleText();

int wpdif_plusident();
int wpdif_minusident();

// end PDIFLIB.H
