// $Header: /CAMCAD/4.5/read_wrt/GerberWriter.h 17    2/15/07 10:08p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__GerberWriter_h__)
#define __GerberWriter_h__

#pragma once

#include "CamCadDatabase.h"
#include "Lyr_File.h"
//_____________________________________________________________________________

enum GerberLayerType_Tag
{
   NonStencilLayer,
   StencilLayer_NoThickness,
   StencilLayer_HasThickness,
   StencilLayer_AnyThickness   // includes NoThickness and HasThickness, together.  DR 741085
};

class CStencilThicknessMap : public CTypedMapIntToPtrContainer<CStringArray*>
{
private:
   CCEtoODBDoc& m_camCadDoc;
   bool m_useStencilThickness;
   GerberLayerType_Tag m_GerberLayerType;
   CString m_targetThickness;

private:
   CString getThicknessAttribute(DataStruct *data);
   bool LookupThickness(CStringArray *layerlist,CString thickness);

public:
   CStencilThicknessMap(CCEtoODBDoc &camCadDoc, bool thicknessflag);
   ~CStencilThicknessMap();

   bool IsStencilThicknessLayer(int layerType);
   bool VerifyThicknessData(DataStruct *data);

   int hasThicknessLayer(FileLayerStruct* fileLayer);
   void SetTargetThickness(CString thickness){m_targetThickness = thickness;}

   CStringArray* AddThicknessLayer(int layerIndex, CString thickness);
   CStringArray* AddThicknessLayer(DataStruct *data);

   void setlayerThicknessflag(bool thicknessflag){m_useStencilThickness = thicknessflag;}
   bool getlayerThicknessflag(){return m_useStencilThickness;}

   void setGerberLayerType(GerberLayerType_Tag type){m_GerberLayerType = type;}
   GerberLayerType_Tag getGerberLayerType(){ return m_GerberLayerType;}
};

//_____________________________________________________________________________
class CMapWidthIndexToBlock
{
private:
   CTypedMapIntToPtrContainer<BlockStruct*> m_widthIndexToBlockMap;

public:
   CMapWidthIndexToBlock();
   void empty();

   void setAt(int index,BlockStruct* block);
   BlockStruct* getAt(int widthIndex) const;
   bool contains(int index) const;
   int getDcode(int widthIndex) const;
};

//_____________________________________________________________________________
class CGerberApertures 
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CMapWidthIndexToBlock& m_widthIndexToNormalizedApertureMap;

   double m_gerberUnitsPerPageUnit;
   CMapStringToString m_gerberApertures;
   int m_nextComplexApertureIndex;
   CStencilThicknessMap *m_StencilThicknessMap;

public:
   CGerberApertures(CCamCadDatabase& camCadDatabase,
      CMapWidthIndexToBlock& widthIndexToNormalizedApertureMap,double gerberUnitsPerPageUnit,
      CStencilThicknessMap *stencilThicknessMap);

   virtual void empty();

public:
   void setGerberUnitsPerPageUnits(double gerberUnitsPerPageUnit);

   bool getNormalizedAperture(BlockStruct& apertureBlock,
      const CTMatrix& insertMatrix,BlockStruct*& normalizedApertureGeometry,
      CBasesVector& apertureBasesVector,bool defineApertureFlag=true);

   bool getNormalizedAperture(DataStruct& apertureData,const CTMatrix& matrix,
      int insertLayerIndex,bool mirrorLayersFlag,FileLayerStruct& fileLayer,
      BlockStruct*& normalizedApertureGeometry,CBasesVector& apertureBasesVector,
      bool defineApertureFlag=false);

   CString getBaseApertureDescriptor(BlockStruct *normalizedApertureBlk);

   bool createNormalizedWidthApertue(int widthIndex);

   void createGerberApertures(CDataList& dataList,CTMatrix& matrix,bool mirrorLayersFlag,
      int insertLayerIndex,FileLayerStruct& fileLayer);

   void assignUniqueDCodes(CWriteFormat& logfile);
   void writeApertureMacros(FILE* file, CWriteFormat& logfile);
   void writeApertureMacro(CDataList& dataList,const CTMatrix& matrix);
   void writeApertureMacroThickPolyLine(CPoly& poly,const CTMatrix& matrix,
      ApertureShapeTag apertureShape,double widthInGerberUnits,bool onFlag);
   void writeApertureMacroClosedPoly(CPoly& poly,const CTMatrix& matrix,bool onFlag);

   CString getLegalMacroName(const CString& macroName);
};

enum GerberFormatTag
{
   gerberFormatRs274     =  0,
   gerberFormatRs274x    =  1,
   gerberFormatFire9xxx  =  2,
   gerberFormatUndefined = -1
};

//_____________________________________________________________________________
class CGerberWriter 
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CMapWidthIndexToBlock m_widthIndexToNormalizedApertureMap;
   CWriteFormat* m_logFile;
   CMessageFilter* m_messageFilter;

   // settings
   bool m_fillPolygons;
   int m_polygonFillDcode;
   double m_scale;
   int m_ordinateDigits;
   int m_ordinateDecimalPlaces;
   PageUnitsTag m_gerberUnits;
   GerberFormatTag m_gerberFormat;
   double m_gerberUnitsPerPageUnit;
   double m_smallGerberWidth;

   // gerber state
   Bool3Tag m_gerberModeNegativePolarity;
   bool m_gerberModeFill;
   bool m_useStencilThickness;
   CStencilThicknessMap *m_StencilThicknessMap;

public:
   CGerberWriter(CCEtoODBDoc& m_camCadDoc);
   virtual ~CGerberWriter();

   void set(FormatStruct& format);
   void writeGerberFiles(CFileLayerList& fileLayerList);

private:
   CWriteFormat& getLogFile();
   CProgressDlg& getProgressDialog();
   CMessageFilter& getMessageFilter();

   void writeGerberFiles(FileLayerStruct* fileLayer, CString fileName);
   void writeGerberFilesByThickness(FileLayerStruct* fileLayer, int thicknessLayerCnt);

   void scanForApertures(CGerberApertures& gerberApertures,FileLayerStruct& fileLayer);
   double getSmallGerberWidth();

   void writeGerberData(CGerberApertures& gerberApertures, FileLayerStruct& fileLayer, BlockStruct *block, CTMatrix &mat, bool mirrorLayersFlag, COperationProgress *progress);
   void writeGerberData(CGerberApertures& gerberApertures, FileLayerStruct& fileLayer);
   void writeGerberDataList(CDataList& dataList,const CTMatrix& matrix,
      CGerberApertures& gerberApertures,FileLayerStruct& fileLayer,bool mirrorLayersFlag,int insertLayerIndex,
      COperationProgress* progress=NULL);
   void writeApertureMacros(CGerberApertures& gerberApertures);

   void drawGerberPoly(DataStruct& data,const CTMatrix matrix);
   void drawGerberPoly(CPoly& poly,const CTMatrix& matrix,bool fillFlag);
   void drawGerberText(TextStruct& text,CTMatrix matrix,bool negativeFlag);
   void drawGerberText(DataStruct& data,CTMatrix matrix);
   void drawGerberAttributes(CAttributes& attributes,const CTMatrix& matrix,FileLayerStruct& fileLayer,
      bool mirrorLayersFlag);
   int getDcode(int widthIndex);

   void initializeGerberState();
   void setGerberModeNegativeLayerPolarity(bool negativePolarityFlag);
   void setGerberModePolyFill(bool fillFlag);

   bool hasVoidPoly(DataStruct *np);
   void SortPolyStructs(CDataList &DataList, CDataList& targetDataList);
   void BuildThicknessLayerMap(); // top level, app should call this one
   void BuildThicknessLayerMap(BlockStruct *block);  // worker bee, called only by top level, block is panel or pcb block
   CStencilThicknessMap *getStencilThicknessMap(){return m_StencilThicknessMap;}

};

//_____________________________________________________________________________

#endif
