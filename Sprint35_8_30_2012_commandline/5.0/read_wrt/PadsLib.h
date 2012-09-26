// $Header: /CAMCAD/4.3/read_wrt/PadsLib.h 7     12/09/03 8:35p Dean Admin $

/**********************************************************************
* PADSLIB.H
*/

#include "stdio.h"   // typedef FILE needed.
#include "geomlib.h"

double wPads_Units(int format, double x);

void wPADS_Graph_Level(const char *l );
void wPADS_Graph_LevelNum( int l );
void wPADS_Graph_Line_Width( double line_width );
int  wPADS_Graph_Units(int output_units_accuracy, double smalldelta);
int  wPADS_Graph_Init(int output_units_accuracy, double SMALLDELTA);
int  wPADS_Graph_File_Open( FILE *file, int version, const char *unitsstring);
void wPADS_Graph_File_Close();

void wPADS_out_line(double maxdesignspace, double x1, double y1, double bulge, double x2, double y2);

void wPADS_Graph_Circle( double center_x, double center_y, double radius, double width,
               const char *linetype, const char *piecetype, const char *restriction);

void wPADS_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width );
void wPADS_Graph_Line( double x1, double y1, double x2, double y2, double width );

void wPADS_GraphSchem_Circle( double center_x, double center_y, double radius, double width);
void wPADS_GraphSchem_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width );
void wPADS_GraphSchem_Line( double x1, double y1, double x2, double y2, double width );

void wPADS_Graph_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror, 
      double penwidth, int header, char HJust, char VJust);

void wPADS_GraphSchem_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror );

void wPADS_Graph_Polyline(double maxdesignspace, Point2 *coordinates, int pairs, int filled, double width, 
      const char *linetype, const char *piecetype, const char *restriction);

void wPADS_Graph_PourPoly( Point2 *coordinates, int pairs, int filled, double width);

void wPADS_GraphSchem_Polyline( Point2 *coordinates, int pairs, int filled, double width);
int  wPADS_GraphSchem_Init();
void wPADS_GraphSchem_File_Open( FILE *file);
void wPADS_GraphSchem_File_Close();

// end PADSLIB.H
