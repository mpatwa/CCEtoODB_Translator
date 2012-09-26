// $Header: /CAMCAD/4.3/read_wrt/GenCad.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
*
*/

#include "stdio.h"
#include "geomlib.h"

void wGENCAD_Graph_Layer(FILE *fp, const char *l);
void wGENCAD_Graph_MaskLevel(FILE *fp, const char *l);
void wGENCAD_Graph_Artwork(FILE *fp, const char *l, long artworkcnt);
void wGENCAD_Graph_Artwork1(FILE *fp, const char *l, long artworkcnt);
void wGENCAD_Graph_WidthCode(FILE *fp,  int w);

int  wGENCAD_Graph_Init_Layout(int output_units_accuracy);
void wGENCAD_Graph_File_Open_Layout();

void wGENCAD_Graph_File_Close_Layout();

void wGENCAD_Graph_Header(FILE *fp, char *gencad, double Version, const char *filename,CString OutputUnits="USER 1000");

void wGENCAD_Graph_Aperture(FILE *fp, double x, double y, double sizea, double sizeb, 
                            double xoffset, double yoffset, int form, double rot);
void wGENCAD_Graph_ComplexAperture(FILE *fp, double x, double y, double sizea, double sizeb, 
                            double xoffset, double yoffset, int form, double rot);

void wGENCAD_Graph_Circle(FILE *fp, double center_x, double center_y, double radius);

void wGENCAD_Graph_Arc(FILE *fp,  double center_x, double center_y, double radius,
               double startangle, double deltaangle);

void wGENCAD_Graph_Line(FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2);
void wGENCAD_Graph_Rectangle(FILE *fp, double x1, double y1, double x2, double y2);

void wGENCAD_Graph_Text(FILE *fp, char *text, double x1, double y1,
      double height, double width, double angle, int mirror, const char *layer );

// end GENCAD.H
