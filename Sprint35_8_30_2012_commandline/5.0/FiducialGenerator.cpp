
#include "StdAfx.h"
#include "FiducialGenerator.h"
#include "DcaEnumIterator.h"
#include "DcaDataListIterator.h"
#include "DcaCamCadData.h"
#include "DcaInsert.h"
#include "DcaLayerType.h"
#include "DcaStandardAperture.h"
#include "DcaDataType.h"

//_________________________________________________________________________________________________
CString FiducialTypeTagToString(FiducialTypeTag tagValue)
{
   const char* pValue;

   switch (tagValue)
   {
   case fiducialTypeRound:   pValue = "Round";   break;
   case fiducialTypeSquare:  pValue = "Square";  break;
   case fiducialTypeCross:   pValue = "Cross";   break;
   default:                  pValue = "?";       break;
   }

   return pValue;
}

FiducialTypeTag StringToFiducialTypeTag(const CString& tagValue)
{
   FiducialTypeTag retval = fiducialTypeUndefined;

   for (EnumIterator(FiducialTypeTag,iterator);iterator.hasNext();)
   {
      FiducialTypeTag fiducialType = iterator.getNext();

      if (FiducialTypeTagToString(fiducialType).Compare(tagValue) == 0)
      {
         retval = fiducialType;
         break;
      }
   }

   return retval;
}

//_________________________________________________________________________________________________
CFiducialGenerator::CFiducialGenerator(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_padTopLayer(NULL)
, m_maskTopLayer(NULL)
, m_foundPadTopLayer(NULL)
, m_foundPadOuterLayer(NULL)
, m_foundPadAllLayer(NULL)
, m_foundSignalTopLayer(NULL)
, m_foundSignalOuterLayer(NULL)
, m_foundSignalAllLayer(NULL)
, m_foundMaskTopLayer(NULL)
, m_foundMaskAllLayer(NULL)
{
}

CCamCadData& CFiducialGenerator::getCamCadData() const
{
   return m_camCadData;
}

LayerStruct& CFiducialGenerator::getPadTopLayer()
{
   if (m_padTopLayer == NULL)
   {
      selectTopLayerFromAnalysis();
   }

   return *m_padTopLayer;
}

void CFiducialGenerator::setPadTopLayer(LayerStruct* layer)
{
   m_padTopLayer = layer;
}

LayerStruct& CFiducialGenerator::getMaskTopLayer()
{
   if (m_maskTopLayer == NULL)
   {
      selectTopLayerFromAnalysis();
   }

   return *m_maskTopLayer;
}

void CFiducialGenerator::setMaskTopLayer(LayerStruct* layer)
{
   m_maskTopLayer = layer;
}

void CFiducialGenerator::initializeLayerAnalysis()
{
   m_foundPadTopLayer      = NULL;
   m_foundPadOuterLayer    = NULL;
   m_foundPadAllLayer      = NULL;
   m_foundSignalTopLayer   = NULL;
   m_foundSignalOuterLayer = NULL;
   m_foundSignalAllLayer   = NULL;
   m_foundMaskTopLayer     = NULL;
   m_foundMaskAllLayer     = NULL;
}

void CFiducialGenerator::analyzeLayers(BlockStruct& pcbGeometry)
{
   if (m_foundPadTopLayer == NULL || m_maskTopLayer == NULL)
   {
      for (CDataListIterator fiducialIterator(pcbGeometry,insertTypeFiducial);fiducialIterator.hasNext();)
      {
         DataStruct* fiducialInsertData = fiducialIterator.getNext();
         BlockStruct* fiducialGeometry = getCamCadData().getBlock(fiducialInsertData->getInsert()->getBlockNumber());

         for (CDataListIterator padIterator(*fiducialGeometry,insertTypeUnknown);padIterator.hasNext();)
         {
            DataStruct* padInsertData = padIterator.getNext();

            if (padInsertData->getLayerIndex() >= 0)
            {
               LayerStruct* layer = getCamCadData().getLayer(padInsertData->getLayerIndex());

               if (layer != NULL && layer->hasMirroredLayer())
               {
                  switch (layer->getLayerType())
                  {
                  case layerTypePadTop:       if (m_foundPadTopLayer      == NULL) m_foundPadTopLayer      = layer;  break;
                  case layerTypePadOuter:     if (m_foundPadOuterLayer    == NULL) m_foundPadOuterLayer    = layer;  break;
                  case layerTypePadAll:       if (m_foundPadAllLayer      == NULL) m_foundPadAllLayer      = layer;  break;
                  case layerTypeSignalTop:    if (m_foundSignalTopLayer   == NULL) m_foundSignalTopLayer   = layer;  break;
                  case layerTypeSignalOuter:  if (m_foundSignalOuterLayer == NULL) m_foundSignalOuterLayer = layer;  break;
                  case layerTypeSignalAll:    if (m_foundSignalAllLayer   == NULL) m_foundSignalAllLayer   = layer;  break;
                  case layerTypeMaskTop:      if (m_foundMaskTopLayer     == NULL) m_foundMaskTopLayer     = layer;  break;
                  case layerTypeMaskAll:      if (m_foundMaskAllLayer     == NULL) m_foundMaskAllLayer     = layer;  break;
                  }
               }
            }
         }
      }
   }
}

bool CFiducialGenerator::selectTopLayerFromAnalysis()
{
   bool retval = true;

   m_padTopLayer = NULL;

   if      (m_foundPadTopLayer      != NULL) m_padTopLayer = m_foundPadTopLayer;
   else if (m_foundPadOuterLayer    != NULL) m_padTopLayer = m_foundPadOuterLayer;
   else if (m_foundPadAllLayer      != NULL) m_padTopLayer = m_foundPadAllLayer;
   else if (m_foundSignalTopLayer   != NULL) m_padTopLayer = m_foundSignalTopLayer;
   else if (m_foundSignalOuterLayer != NULL) m_padTopLayer = m_foundSignalOuterLayer;
   else if (m_foundSignalAllLayer   != NULL) m_padTopLayer = m_foundSignalAllLayer;

   if (m_padTopLayer == NULL)
   {
      m_padTopLayer = &(getCamCadData().getDefinedLayer(ccLayerPadTop));
      
      retval = false;
   }

   m_maskTopLayer = NULL;

   if      (m_foundMaskTopLayer != NULL) m_maskTopLayer = m_foundMaskTopLayer;
   else if (m_foundMaskAllLayer != NULL) m_maskTopLayer = m_foundMaskAllLayer;

   if (m_maskTopLayer == NULL)
   {
      m_maskTopLayer = &(getCamCadData().getDefinedLayer(ccLayerMaskTop));
      
      retval = false;
   }

   return retval;
}

BlockStruct& CFiducialGenerator::getDefinedFiducialGeometry(FiducialTypeTag fiducialTypeTag,double size,int fileNumber)
{
   StandardApertureTypeTag fiducialType;

   switch (fiducialTypeTag)
   {
   case fiducialTypeRound:   fiducialType = standardApertureCircle;           break;
   case fiducialTypeSquare:  fiducialType = standardApertureSquare;           break;
   case fiducialTypeCross:   fiducialType = standardApertureTabbedRectangle;  break;
   }

   BlockStruct* fiducialGeometry    = NULL;
   BlockStruct* fiducialPadGeometry = NULL;

   CStandardAperture fiducialAperture(fiducialType,getCamCadData().getPageUnits());

   if (size <= 0.)
   {
      size = getCamCadData().convertToPageUnits(pageUnitsMils,1.);
   }

   switch (fiducialType)
   {
   case standardApertureCircle:
   case standardApertureSquare:
      fiducialAperture.setDimensions(size);

      break;
   case standardApertureTabbedRectangle:
      fiducialAperture.setDimensions(size/4.,size/4.,size/4.,3.*size/8.);

      break;
   }

   fiducialPadGeometry = fiducialAperture.getDefinedAperture(getCamCadData(),fileNumber);

   if (fiducialPadGeometry != NULL)
   {
      CString fiducialGeometryName;
      fiducialGeometryName.Format("FidGeom_%s",fiducialPadGeometry->getName());
      fiducialGeometry = getCamCadData().getBlock(fiducialGeometryName,fileNumber);

      if (fiducialGeometry == NULL)
      {
         fiducialGeometry = getCamCadData().getNewBlock(fiducialGeometryName,blockTypeFiducial,fileNumber);

         int padTopLayerIndex = getPadTopLayer().getLayerIndex();

         DataStruct* copperPad = getCamCadData().getNewDataStruct(dataTypeInsert);
         copperPad->getInsert()->setInsertType(insertTypeUnknown);
         copperPad->setLayerIndex(padTopLayerIndex);
         copperPad->getInsert()->setBlockNumber(fiducialPadGeometry->getBlockNumber());

         int maskTopLayerIndex = getMaskTopLayer().getLayerIndex();

         DataStruct* maskPad = getCamCadData().getNewDataStruct(dataTypeInsert);
         maskPad->getInsert()->setInsertType(insertTypeUnknown);
         maskPad->setLayerIndex(maskTopLayerIndex);
         maskPad->getInsert()->setBlockNumber(fiducialPadGeometry->getBlockNumber());

         fiducialGeometry->getDataList().AddTail(copperPad);
         fiducialGeometry->getDataList().AddTail(maskPad);
      }
   }

   //CString fiducialGeomName;
   //fiducialGeomName.Format("FidGeom_%0.3f_%s", size,   apertureShapeToName(shape));
   //if (!m_fiducialGeomNameMap.Lookup(fiducialGeomName, fiducialBlock))
   //{
   //   CString apertureName;
   //   apertureName.Format("Aperture_%0.3f_%s", size, apertureShapeToName(shape));

   //   int err;
   //   int widthIndex = Graph_Aperture(apertureName, shape, size, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
   //   BlockStruct* apertureBlock = m_doc.getWidthBlock(widthIndex);

   //   if (apertureBlock != NULL)
   //   {
   //      fiducialBlock = Graph_Block_On(GBO_APPEND, fiducialGeomName, -1, 0L, blockTypeFiducial);
   //      Graph_Block_Reference(apertureBlock->getName(), "", apertureBlock->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, getPadTopLayerIndex(), FALSE, apertureBlock->getBlockType()); 
   //      Graph_Block_Reference(apertureBlock->getName(), "", apertureBlock->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, getMaskTopLayerIndex(), FALSE, apertureBlock->getBlockType()); 
   //      Graph_Block_Off();

   //      m_fiducialGeomNameMap.SetAt(fiducialGeomName, fiducialBlock);
   //   }
   //}

   if (fiducialGeometry == NULL)
   {
      int fidcialGeometryWidthIndex = getCamCadData().getDefinedWidthIndex(size);
      fiducialGeometry = &(getCamCadData().getDefinedWidthBlock(fidcialGeometryWidthIndex));
   }

   return *fiducialGeometry;
}

