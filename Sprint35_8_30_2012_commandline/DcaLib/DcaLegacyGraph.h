
#if !defined(__DcaLegacyGraph_h__)
#define __DcaLegacyGraph_h__

#pragma once

#include "Dca.h"
#include "DcaBlockType.h"

#define MaxBlockLevel 10

class CCamCadData;
class CDataList;
class BlockStruct;
class DataStruct;
class CPolyList;
class CPoly;
class FileStruct;
class CWriteFormat;

enum BlockTypeTag;
enum FileTypeTag;

enum GraphBlockOnMethodTag
{
   graphBlockOnMethodReturn    = 0,
   graphBlockOnMethodOverwrite = 1,
   graphBlockOnMethodAppend    = 2,
};

//_____________________________________________________________________________
class CDcaLegacyGraph
{
private:
   CCamCadData& m_camCadData;

   FileStruct* m_currentFile;
   CDataList*  m_currentDataList;
   CPolyList*  m_currentPolyList;
   CPoly*      m_currentPoly;
   CArray<int,int> m_blockArray;

   CWriteFormat* m_traceWriteFormat;
   CString     m_graphDebugText;

public:
   CDcaLegacyGraph(CCamCadData& camCadData);

   CCamCadData& getCamCadData() const { return m_camCadData; }

   FileStruct* graphFileStart(const CString& name,FileTypeTag sourceCadFileType);
   BlockStruct* graphBlockOn(GraphBlockOnMethodTag method,const CString& blockName,int fileNumber,DbFlag flags, BlockTypeTag blockType = blockTypeUndefined);
   void graphBlockOff();
   BlockStruct* graphBlockExists(const CString& blockName,int fileNumber,BlockTypeTag blockType = blockTypeUndefined);
   DataStruct* graphBlockReference(const CString& blockName,const char* refName,int fileNumber,double x,double y,
         double radians,bool mirror,double scale,int layerIndex,bool global,BlockTypeTag blockType = blockTypeUndefined);
   void graphBlockCopy(BlockStruct* block,double x,double y,double angle,int mirror,double scale,int layerIndex,int copyAttributes,int hideData=FALSE);
   int graphLevel(const char* layerName,const char* prefix,bool floatingFlag);
   DataStruct* graphPolyStruct(int layerIndex,DbFlag flags,bool negative,int entityNumber = -1);
   CPoly* graphPoly(DataStruct* polyStruct,int widthIndex,BOOL filledFlag,BOOL voidFlag,BOOL closedFlag);
   CPnt* graphVertex(double x,double y,double bulge=0.);
   DataStruct* graphCircle(int layerIndex,double xCenter,double yCenter,double radius,DbFlag flags,int widthIndex,BOOL negative,BOOL filled);
   DataStruct* graphArc(int layerIndex,double xCenter,double yCenter,double radius,
      double startAngleRadians,double deltaAngleRadians,DbFlag flags,int widthIndex,BOOL negative);
   DataStruct* graphText(int layerIndex, const char* text, double x, double y, 
      double height, double charwidth, double angleRadians, DbFlag flags,char proportional, char mirror, int oblique, BOOL negative, 
      int penWidthIndex, int specialChar, int textAlignment, int lineAlignment);

   int graphComplex(int fileNumber, const CString& name, int dcode, const CString& subBlockName,
      double xoffset, double yoffset, double rotation, BlockTypeTag blockType = blockTypeUndefined);
   int getBlockNum(const CString& blockName,int fileNumber,int global,BlockTypeTag blockType = blockTypeUndefined);

   DataStruct* copyTransposeEntity(DataStruct* data,double x,double y,double angle,int mirror,double scale,int layerIndex,BOOL CopyAttribs,int entityNumber = -1);

   void setTraceWriteFormat(CWriteFormat* traceWriteFormat);
   void trace(const char* format,...);
   void traceAppendFormat(const char* format,...);
};

#endif
