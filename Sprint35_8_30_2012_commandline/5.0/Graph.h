// $Header: /CAMCAD/5.0/Graph.h 35    6/30/07 2:29a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#ifndef ___GRAPH_H
#define ___GRAPH_H

#include "dbutil.h"
#include "block.h"
#include "file.h"
#include "ccdoc.h"

#define GFP_MODIFIED 2
#define GFP_COPIED      1
#define GFP_FAILED      0

#define GBO_APPEND      2
#define GBO_OVERWRITE   1
#define GBO_RETURN      0

CDataList *GetCurrentDataList();
CCEtoODBDoc* GraphGetDoc();

void SetCurrentDataList(CDataList *dataList);
bool RemoveOneEntityFromDataList(CCEtoODBDoc *doc, CDataList *dataList, DataStruct *data, POSITION dataPos = NULL);
int MoveEntityToAnotherDataList(DataStruct *data, CDataList *oldList, CDataList *newList);
void RefreshInheritedAttributes(CCEtoODBDoc *doc, int mode, BOOL overwriteNotInherited = FALSE);
void RefreshTypetoCompAttributes(CCEtoODBDoc *doc, int mode);
//DataStruct *CopyEntity(DataStruct *data, BOOL CopyAttribs);
DataStruct *AddEntity(DataStruct *data);  // add a datastruct to current datalist
DataStruct *CopyTransposeEntity(DataStruct *data, double x, double y, double angle, int mirror, double scale, int layer, BOOL CopyAttribs,int entityNumber = -1);

// this gets the layer of an aperture or a block labeled as an aperture.
int Get_ApertureLayer(CCEtoODBDoc *doc, DataStruct *data, BlockStruct *block, int insertLayer);

/*
APERTURES:
   Rotating of files containing apertures draws rotated apertures.
   Rotating a particular insert of an aperture draws a rotated aperture.

   Scaling of a file draws a scaled aperture.
   A particular insert of an aperture con not have a scale.

   Mirroring a file has no effect on the aperture (but of course it still changes the insert point).

   When not available, Exporters will generate new apertures with new dcodes to represent 
      modified apertures.
*/

// FILE
FileStruct *Graph_File_Start(const char *name, int source_cad);

void Graph_File_Reference(FileStruct *file, double insert_x, double insert_y,
          double angle, int mirror, double scale);


// BLOCK
BlockStruct *Graph_Block_Exists(CCEtoODBDoc *d, const char *block_name, int filenum, BlockTypeTag blockType = blockTypeUndefined, bool compareNoCase = false); // returns block ptr or NULL

BlockStruct *Graph_Block_On(int method, const char *block_name, int filenum, DbFlag flg, BlockTypeTag blockType = blockTypeUndefined); // declares and begins defining a block
BlockStruct *Graph_Block_On(BlockStruct* block); 

BlockStruct *Declare_Block(const char *block_name, int filenum, DbFlag flg, BOOL Aperture, BlockTypeTag blockType = blockTypeUndefined);

void Graph_Block_Off();

void Graph_Block_Copy(BlockStruct *block, double x, double y,
      double angle, int mirror, double scale, int layer, int copy_attributes, int hideData = FALSE);


// APERTURE / WIDTH
int Graph_Complex(const char *name, int dcode, const char *subBlockName,
                  double xoffset, double yoffset, double rotation, BlockTypeTag blockType = blockTypeUndefined);

int Graph_Complex(int filenum, const char *name, int dcode, const char *subBlockName,
                  double xoffset, double yoffset, double rotation, BlockTypeTag blockType = blockTypeUndefined);

int Graph_Complex(const char *name, int dcode, BlockStruct *subBlock, 
                              double xoffset, double yoffset, double rotation);


// overwrite error is set to TRUE if a same name aperture exist and the values 
// are different.
int Graph_Aperture(const char *name, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation,
      int dcode, DbFlag flag, int overwrite_values, int *overwrite_err);

// For case 2158
int Graph_Aperture(int filenum, const char *name, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation,
      int dcode, DbFlag flag, int overwrite_values, int *overwrite_err);


// Find an aperture with same shape and size without care about name or file association
// If found return pointer or create one.
BlockStruct *Graph_FindAperture(int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation, DbFlag flag, int named, int width_allowed);

// Find an aperture with same shape and size without care about name, that is, not looking for particular name.
// If mustBeNamed is false, then a global unnamed aperture with correct dimensions is acceptable.
// Otherwise, if aperture is named, it must match requested file number.
// If no existing aperture fits the bill, then a named aperture is created.
BlockStruct *Graph_FindAperture(int filenum, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation, bool mustBeNamed = false, bool widthAllowed = false);

// TOOL
int Graph_Tool(const char *name, int tcode, double size, int type, int geomnum, BOOL display, DbFlag flag, BOOL plated = TRUE, BOOL punched = FALSE);

BlockStruct *Graph_FindTool(double size, DbFlag flag, int named,bool platedFlag=false);

int Graph_ComplexTool(const char *name, int tcode, int type, int geomnum, 
                        BOOL display, DbFlag flag, BOOL plated, BOOL punched,
                        int Shape, double width , double height);


// LAYER
int Graph_Level(CCEtoODBDoc& camCadDocument,const char *layerName, const char *prefix, BOOL Floating);
int Graph_Level(const char *layerName, const char *prefix, BOOL Floating);
void Graph_Level_Mirror(const char* layerName1,const char* layerName2,const char* prefix=NULL);

LayerGroupStruct *Graph_LayerGroup(const char *name);
void Graph_LayerInLayerGroup(LayerGroupStruct *group, LayerStruct *layer);
void Graph_GroupInLayerGroup(LayerGroupStruct *group, LayerGroupStruct *subgroup);


// DATA
DataStruct *Graph_Block_Reference(const char *block_name, const char *refname, 
      int filenum, double x, double y,
      double angle, int mirror, double scale, int layer, int global, BlockTypeTag blockType = blockTypeUndefined);

DataStruct *Graph_Block_Reference(int blocknumber, const char *refname, 
      double x, double y,
      double angle, int mirror, double scale, int layer);


DataStruct *Graph_Line(int layer, double x1, double y1, double x2, double y2, DbFlag flg, 
      int widthIndex, BOOL negative);

DataStruct *Graph_PolyStruct(int layer, DbFlag flg, BOOL negative,int entityNumber = -1);
CPoly *Graph_Poly_Arc(double center_x, double center_y, double radius, double startangle, double deltaangle);
CPoly *Graph_Poly_Circle(double center_x, double center_y, double radius);
// Graph_Poly() - polyStruct should be NULL unless you want to specify a polystruct to add a poly to
CPoly *Graph_Poly(DataStruct *polyStruct, int widthIndex, BOOL Filled, BOOL VoidPoly, BOOL Closed);
CPnt *Graph_Vertex(double x, double y, double bulge);

DataStruct *Graph_Circle(int layer, double center_x, double center_y, 
      double radius, DbFlag flg, int widthIndex, BOOL negative, BOOL filled);

DataStruct *Graph_Arc(int layer, double center_x, double center_y, 
      double radius, double startangle, double deltaangle,
      DbFlag flg, int widthIndex, BOOL negative);

DataStruct *Graph_Text(int layer, const char *text, double x, double y,
      double height, double width, double angle, DbFlag flg,
      char proportional, char mirror, int oblique, BOOL negative,    
      int penWidthIndex,int specialchar, int textAlignment = 0, int lineAlignment = 0);

DataStruct *Graph_Point(double x, double y, int layer, DbFlag flg, BOOL negative);

DataStruct *Graph_Blob(double left, double bottom, double width, double height, double rotation, 
      BOOL TopView, BOOL BottomView, CString filename);

// returns TRUE is ok, FALSE is could not attach
int Graph_Attach_Blob(DataStruct *d, const char *filename);

// FONT LINE
void Graph_Font_Line(CPolyList *PolyList, double x1, double y1,
      double x2, double y2, DbFlag flg);

// Diagnostic
void setGraphTraceWriteFormat(CWriteFormat* traceWriteFormat);
void graphTrace(const char* format,...);
void graphAppendFormat(const char* format,...);

#endif
// end GRAPH.H
