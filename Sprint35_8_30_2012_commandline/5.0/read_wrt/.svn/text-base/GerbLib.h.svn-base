// $Header: /CAMCAD/4.6/read_wrt/GerbLib.h 14    9/29/06 5:03p Kurt Van Ness $

/*******************************************************************************
*
*/

#include "geomlib.h"
#include "format_s.h"

void wgerb_Set_Format(FormatStruct *format);
int  wgerb_Graph_File_Open(const char *out_file);
void wgerb_Graph_File_Close();
void wgerb_Load_Aperture(int appnum);
void wgerb_Flash_App(const Point2& point, int app_num);
void wgerb_Line(Point2 start, Point2 end, int app_num,
                           int apshape, double width);
void wgerb_Arc(const Point2& center, double radius, double startradians, 
               double deltaradians, int app_num, int cont);
void wgerb_Circle(const Point2& center, double radius,int app_num);
void wgerb_write2file(const char *prosa);
void wgerb_write(const char* format,...);
void wgerb_Graph_Polyline( const Point2& p, int dcode,int apshape, double width);
void wgerb_Graph_Vertex(const Point2& p);
CPoint wgerb_convertPoint(const Point2& p);
void wgerb_Pen_Up(const Point2& p);
void wgerb_Pen_Down(const Point2& p);
void wgerb_comment(const CString& comment);
int  wgerb_getCurrentAperture();
FILE* wgerb_getFile();
 
// end GERBLIB.H
