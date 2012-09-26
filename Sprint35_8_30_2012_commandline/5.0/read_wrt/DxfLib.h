// $Header: /CAMCAD/4.3/read_wrt/DxfLib.h 6     8/12/03 9:06p Kurt Van Ness $

/****************************************************************************
* Project CAMCAD
* Router Solutions Inc.
* Copyright © 1994-97. All Rights Reserved.
*/

#ifndef __dxflib_h
#define __dxflib_h

#include "geomlib.h"
#include "format_s.h"
#include "DcaVerticalPosition.h"
#include "DcaHorizontalPosition.h"

struct DXF_Header
{
   CString VersionNumber;
};

void wdxf_Graph_Line_End( int onoff );  // 0 no end, 1 round, 2 sqaure
void wdxf_Graph_Line_Width( double width );
int  wdxf_Graph_File_Open(FILE *wfp);
int  wdxf_Graph_File_Close(FILE *logfile, int *display_error_cnt, DXF_Header header);
int  wdxf_Graph_Polyline( Point2 p, int closed);
int  wdxf_Graph_Vertex(Point2 p);
int  wdxf_Graph_PolyEnd();
int  wdxf_Graph_SeqEnd();
int  wdxf_Graph_Init(int i, int nach_komma, double realwidth, int handle, bool xscale, bool unmirrortext);

void wdxf_Graph_Level(const char *level );

void wdxf_Graph_WriteALine(const char *line );

int  wdxf_Graph_Aperture( int type, double x, double y, double sizeA, double sizeB, double rotation, 
                              double xoffset, double yoffset);

int  wdxf_Graph_Arc(double center_x, double center_y,
                    double radius, double startangle,double deltaangle);

int  wdxf_Graph_Circle(double center_x, double center_y,double radius );

int  wdxf_Graph_Block_On(const char *block_name,int attflg);

int  wdxf_Graph_Block_Off( );

int  wdxf_Graph_Block_Reference(const char *block_name, double x1, double y1,
                                 double angle, int mirror,
                                 double scale,int attflg );

int  wdxf_Graph_Block_Reference_Layer(const char *layername,
                             const char *block_name, double x1, double y1,
                             double angle, int mirror, double scale, int flg );

int  wdxf_Graph_Line( double x1, double y1, double x2, double y2);

int  wdxf_Graph_Text( const char *text, double x1, double y1,
                        double height, double width, double angle, int mirror, int oblique, 
                        VerticalPositionTag vertical = verticalPositionBaseline, HorizontalPositionTag horizontal = horizontalPositionLeft);

int  wdxf_Graph_HollowLineRoundEnd(double x1, double y1, double x2, double y2);

int  wdxf_Graph_HollowArcRoundEnd(double center_x, double center_y,
                        double radius, double startangle,double deltaangle);

int   wdxf_Graph_Attins( const char *value, const char *tag, int flg,
                        double x1, double y1, double height, double angle, int mirror );

int   wdxf_Graph_Attdef( const char *def_text, const char *prompt, const char *text, int flg,
         double x1, double y1, double height, double angle );

long wdxf_get_handle();

/////////////////////////////////////////////////////////////////////////////
// CDXFOutSetting
#define DXFOUTCMD_USELAYERINDEX     ".USE_LAYERINDEX"
#define DXFOUTCMD_HANDLE            ".HANDLE"
#define DXFOUTCMD_POLYCRACK         ".POLYCRACK"
#define DXFOUTCMD_POLYCRACKWIDTH    ".POLYCRACKWIDTH"
#define DXFOUTCMD_MINUMUMTOOLDIAM   ".MINUMUMTOOLDIAM"
#define DXFOUTCMD_TOOLLAYER         ".TOOLLAYER"
#define DXFOUTCMD_TOOLCODELAYER     ".TOOLCODELAYER"
#define DXFOUTCMD_ACADVER           ".ACADVER"

class CDXFOutSetting
{
private:
   bool m_uselayerindex;
   bool m_splitpolylines;
   bool m_handle;
   double m_splitwidth;
   double m_mintooldiam;
   CString m_toollayer;
   CString m_toolcodelayer;
   DXF_Header  m_dxfHeader;

   // modified by format
   int      m_format_line_ends;
   double   m_format_RealWidth;
   int      m_format_ExportHiddenEntities;
   bool     m_format_UseProbeType;
   int      m_format_explodeStyle;
   bool     m_format_TextXscale;
   bool     m_format_UnMirrorText;

public:
   CDXFOutSetting();
   void LoadDefaultSettings();
   int load_DXFsettings(const CString FileName, FormatStruct *format);
   void LoadFormatSettings(FormatStruct *format);

   //get options
   bool     getUserLayerIndex(){return m_uselayerindex;}
   bool     getSplitPolylines(){return m_splitpolylines;}
   bool     getHandle(){return m_handle;}
   double   getSplitWidth(){return m_splitwidth;}
   double   getMinToolDiam(){return m_mintooldiam;}
   CString& getToolLayer(){return m_toollayer;}
   CString& getToolCodeLayer(){return m_toolcodelayer;}
   DXF_Header& getDXFHeader(){return m_dxfHeader;}
   
   int      getFormatLineEnds(){return m_format_line_ends;}
   double   getFormatRealWidth(){return m_format_RealWidth;}
   int      getFormatExportHiddenEntities(){return m_format_ExportHiddenEntities;}
   bool     getFormatUseProbeType(){return m_format_UseProbeType;}
   int      getFormatExplodeStyle(){return m_format_explodeStyle;}

   bool     getFormatTextXscale(){return m_format_TextXscale;}
   bool     getFormatUnMirrorText(){return m_format_UnMirrorText;}

   //set options
   void  setUserLayerIndex(bool val){m_uselayerindex = val;}
   void  setSplitPolylines(bool val){m_splitpolylines = val;}
   void  setHandle(bool val){m_handle = val;}
   void  setSplitWidth(double val){m_splitwidth = val;}
   void  setMinToolDiam(double val){m_mintooldiam = val;}
   void  setToolLayer(CString val){m_toollayer = val;}
   void  setToolCodeLayer(CString val){m_toolcodelayer = val;}

   void  setFormatLineEnds(int val){m_format_line_ends = val;}
   void  setFormatRealWidth(double val){m_format_RealWidth = val;}
   void  setFormatExportHiddenEntities(int val){m_format_ExportHiddenEntities = val;}
   void  setFormatUseProbeType(bool val){m_format_UseProbeType = val;}
   void  setFormatExplodeStyle(int val){m_format_explodeStyle = val;}
   void  setFormatTextXscale(bool val){ m_format_TextXscale = val;}
   void  setFormatUnMirrorText(bool val){ m_format_UnMirrorText = val;}

};

#endif 
// end DXFLIB.H
