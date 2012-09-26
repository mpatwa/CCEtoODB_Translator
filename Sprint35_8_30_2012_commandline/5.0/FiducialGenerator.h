// $Header: /CAMCAD/4.6/read_wrt/Xxxxx.h 2     11/30/06 9:26p Kurt Van Ness $

#if !defined(__FiducialGenerator_h__)
#define __FiducialGenerator_h__

#pragma once

class CCamCadData;
class LayerStruct;
class BlockStruct;

//_________________________________________________________________________________________________
enum FiducialTypeTag
{
   fiducialTypeRound,
   fiducialTypeSquare,
   fiducialTypeCross,
   fiducialTypeUndefined,
   FiducialTypeTagMin = fiducialTypeRound,
   FiducialTypeTagMax = fiducialTypeUndefined - 1,
};

CString FiducialTypeTagToString(FiducialTypeTag tagValue);
FiducialTypeTag StringToFiducialTypeTag(const CString& tagValue);

//_________________________________________________________________________________________________
class CFiducialGenerator
{
private:
   CCamCadData& m_camCadData;

   LayerStruct* m_padTopLayer;
   LayerStruct* m_maskTopLayer;

   LayerStruct* m_foundPadTopLayer;
   LayerStruct* m_foundPadOuterLayer;
   LayerStruct* m_foundPadAllLayer;
   LayerStruct* m_foundSignalTopLayer;
   LayerStruct* m_foundSignalOuterLayer;
   LayerStruct* m_foundSignalAllLayer;
   LayerStruct* m_foundMaskTopLayer;
   LayerStruct* m_foundMaskAllLayer;

public:
   CFiducialGenerator(CCamCadData& camCadData);

   CCamCadData& getCamCadData() const;

   LayerStruct& getPadTopLayer();
   void setPadTopLayer(LayerStruct* layer);

   LayerStruct& getMaskTopLayer();
   void setMaskTopLayer(LayerStruct* layer);

   void initializeLayerAnalysis();
   void analyzeLayers(BlockStruct& pcbGeometry);
   bool selectTopLayerFromAnalysis();

   BlockStruct& getDefinedFiducialGeometry(FiducialTypeTag fiducialTypeTag,double size,int fileNumber);
};

#endif
