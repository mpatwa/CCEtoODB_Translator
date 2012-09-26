// $Header: /CAMCAD/4.3/read_wrt/PfwLib.h 7     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************/
/* PFWLIB.H
*/

#include "geomlib.h"

void wpfw_Graph_Level(const char *l);
void wpfw_Graph_LevelNum(int num);
void wpfw_Graph_Width( double line_width );

int wpfw_Graph_Init();
void wpfw_Graph_File_Open( FILE *file);

int wpfw_Graph_Init_Layout();
void wpfw_Graph_File_Open_Layout( FILE *file);

int wpfw_Graph_Init_Schematic();
void wpfw_Graph_File_Open_Schematic( FILE *file);

void wpfwcomp_pin( double xref, double yref, const char *pinname,
                  double topx, double topy, int topform,
                  double innerx, double innery, int innerform,
                  double botx, double boty, int botform,
                  double holesize, int netindex, double degree, int layernum = 34);

void wpfw_via(double x, double y, double diam, double drill, int netindex );

void wpfwcomp_Header( double xref, double yref,
                      double x1, double y1, double x2, double y2, double rot,
                      int show_ref, int show_comment);

void wpfw_Graph_File_Close_Layout();
void wpfw_Graph_File_Close_Schematic();
void wpfw_Graph_Aperture(double x, double y, double sizea, double sizeb, int form, double rot,
                         const char *cpfp);

void wpfw_Graph_Circle( double center_x, double center_y, double radius,double width);

void wpfw_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width, int netindex );

void wpfw_Graph_Line( double x1, double y1, double bulge1, 
                      double x2, double y2, double bulge2, double width, int netindex );

void wpfw_Graph_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror );
void wpfw_Graph_Polyline( Point2 *coordinates, int pairs, int filled, int netindex);

void wpfwcomp_Graph_Circle( double center_x, double center_y, double radius,double width);

void wpfwcomp_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, double width );

void wpfwcomp_Graph_Line( double x1, double y1, double bulge1, 
                          double x2, double y2, double bulge2, double width );
void wpfwcomp_Graph_Text( char *text, double x1, double y1,
      double height, double width, double angle, int mirror );
void wpfwcomp_Graph_Polyline( Point2 *coordinates, int pairs, int filled);

void wpfwschem_Graph_Line( double x1, double y1, double x2, double y2, int widthcode );
void wpfwschem_Graph_Polyline( Point2 *coordinates, int pairs, int filled);
void wpfwschem_Graph_Arc( double center_x, double center_y, double radius,
               double startangle, double deltaangle, int widthcode );
void wpfwschem_Graph_Circle( double center_x, double center_y, double radius,int widthcode);
void wpfwschem_Graph_Text( char *text, double x1, double y1,int font,
                           double angle, int mirror );
// end PFWLIB.H
