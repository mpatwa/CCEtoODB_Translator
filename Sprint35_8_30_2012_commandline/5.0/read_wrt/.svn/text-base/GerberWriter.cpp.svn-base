// $Header: /CAMCAD/5.0/read_wrt/GerberWriter.cpp 38    5/21/07 7:52p Kurt Van Ness $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#include "StdAfx.h"
#include "GerberWriter.h"
#include "GerbLib.h"
#include "RwLib.h"
#include "CCEtoODB.h"
#include "Gauge.h"
#include "StandardAperture.h"
#include "PolyLib.h"
#include "Font.h"
#include "DcaMessageFilter.h"
#include "RwUiLib.h"

// Name of attribute used to track original base aperture as an attribute
// in normalized aperture geometry blocks.
#define QDERIVED_FROM_APERTURE "DERIVED_FROM_APERTURE"

//_____________________________________________________________________________
CMapWidthIndexToBlock::CMapWidthIndexToBlock()
: m_widthIndexToBlockMap(10,false)
{
   m_widthIndexToBlockMap.InitHashTable(nextPrime2n(500));
}

void CMapWidthIndexToBlock::empty()
{
   m_widthIndexToBlockMap.empty();
}

void CMapWidthIndexToBlock::setAt(int index,BlockStruct* block)
{
   m_widthIndexToBlockMap.SetAt(index,block);
}

bool CMapWidthIndexToBlock::contains(int widthIndex) const
{
   BlockStruct* block;

   bool retval = (m_widthIndexToBlockMap.Lookup(widthIndex,block) != 0);

   return retval;
}

BlockStruct* CMapWidthIndexToBlock::getAt(int widthIndex) const
{
   BlockStruct* block = NULL;

   m_widthIndexToBlockMap.Lookup(widthIndex,block);

   return block;
}

int CMapWidthIndexToBlock::getDcode(int widthIndex) const
{
   int dcode = -1;
   BlockStruct* block;

   if (m_widthIndexToBlockMap.Lookup(widthIndex,block))
   {
      dcode = block->getDcode();
   }

   return dcode;
}

static int write_gerber_text(CCEtoODBDoc* doc,const char *text, double insert_x, double insert_y,
                  double charheight, double charwidth, double angle,
                  int proportional, int oblique, bool mirrorFlag, int dcode, FontStruct* fontStruct);

static void Gerb_WriteChar(CPolyList *PolyList, double insert_x, double insert_y,
            double rotation, double scale, bool mirrorFlag, int oblique, int dcode);

//_____________________________________________________________________________
CGerberApertures::CGerberApertures(CCamCadDatabase& camCadDatabase,
   CMapWidthIndexToBlock& widthIndexToNormalizedApertureMap,double gerberUnitsPerPageUnit,
   CStencilThicknessMap *stencilThicknessMap)
: m_camCadDatabase(camCadDatabase)
, m_widthIndexToNormalizedApertureMap(widthIndexToNormalizedApertureMap)
, m_gerberUnitsPerPageUnit(gerberUnitsPerPageUnit)
, m_nextComplexApertureIndex(999)
,m_StencilThicknessMap(stencilThicknessMap)
{
   m_gerberApertures.InitHashTable(nextPrime2n(300));
   m_widthIndexToNormalizedApertureMap.empty();
}

void CGerberApertures::empty()
{
   m_gerberApertures.RemoveAll();
}

void CGerberApertures::setGerberUnitsPerPageUnits(double gerberUnitsPerPageUnit)
{
   m_gerberUnitsPerPageUnit = gerberUnitsPerPageUnit;
}

bool CGerberApertures::getNormalizedAperture(BlockStruct& apertureBlock,
   const CTMatrix& insertMatrix,BlockStruct*& normalizedApertureGeometry,
   CBasesVector& apertureBasesVector,bool defineApertureFlag)
{
   bool retval = false;

   int apertureWidthIndex = m_camCadDatabase.getWidthIndex(&apertureBlock);
   CString originalApertureName = apertureBlock.getName();

   BlockStruct* normalizedApertureBlock = m_widthIndexToNormalizedApertureMap.getAt(apertureWidthIndex);

   if (normalizedApertureBlock == &apertureBlock)
   {
      retval = true;
   }
   else if (apertureBlock.isSimpleAperture())
   {
      CCEtoODBDoc& camCadDoc = m_camCadDatabase.getCamCadDoc();

      CStandardAperture standardAperture(apertureBlock.getShape(),
         apertureBlock.getSizeA(),apertureBlock.getSizeB(),
         apertureBlock.getSizeC(),apertureBlock.getSizeD(),camCadDoc.getPageUnits());

      apertureBasesVector = apertureBlock.getApertureBasesVector();
      apertureBasesVector.transform(insertMatrix);

      bool apertureMirrorFlag = apertureBasesVector.getMirror();
      double apertureDegrees = apertureBasesVector.getRotationDegrees();
      apertureBasesVector.set(apertureBasesVector.getOrigin().x,apertureBasesVector.getOrigin().y);

      if (apertureMirrorFlag)
      {
         apertureDegrees *= -1.;
      }

      int nameDegrees = normalizeDegrees(round(apertureDegrees));
      int symmetryDegrees = standardAperture.getSymmetry();

      if (symmetryDegrees == 0)
      {
         nameDegrees = 0;
      }
      else
      {
         while (symmetryDegrees > 0 && nameDegrees > symmetryDegrees)
         {
            nameDegrees -= symmetryDegrees;
         }
      }

      if (nameDegrees >= 90)
      {
         if (apertureBlock.getShape() == apertureRectangle ||
             apertureBlock.getShape() == apertureOblong        )
         {
            if (nameDegrees == 180)
            {
               nameDegrees = 0;
            }
            else
            {
               standardAperture.setDimensions(apertureBlock.getSizeB(),apertureBlock.getSizeA());
               nameDegrees -= 90;
            }
         }
         else if (apertureBlock.getShape() == apertureSquare)
         {
            nameDegrees -= 90;
         }
         else if (apertureBlock.getShape() == apertureRound)
         {
            nameDegrees = 0;
         }
      }

      CString blockName = standardAperture.getApertureBlockName(m_camCadDatabase.getCamCadData());
      CString apertureName(blockName);
      apertureName.AppendFormat("_%d",nameDegrees);

      normalizedApertureGeometry = m_camCadDatabase.getBlock(apertureName);

      if (normalizedApertureGeometry == NULL && defineApertureFlag)
      {
         if (nameDegrees == 0)
         {
            normalizedApertureGeometry = standardAperture.getDefinedAperture(m_camCadDatabase.getCamCadData(),apertureName);
         }
         else
         {
            normalizedApertureGeometry = m_camCadDatabase.getDefinedAperture(apertureName,apertureComplex);

            if (normalizedApertureGeometry->getComplexApertureSubBlockNumber() == 0)
            {
               const DbFlag dbFlag = 0;
               const bool positive = false;
               const bool negative = true;

               CString complexApertureBlockNamePrefix = apertureName + "-Cmplx";

               BlockStruct* complexApertureBlock =
                  m_camCadDatabase.getNewBlock(complexApertureBlockNamePrefix,"-%d",blockTypeUnknown);

               normalizedApertureGeometry->setComplexApertureSubBlockNumber(complexApertureBlock->getBlockNumber());

               int floatLayerIndex = m_camCadDatabase.getFloatingLayerIndex();

               DataStruct* polyStruct = m_camCadDatabase.addPolyStruct(
                                          complexApertureBlock,floatLayerIndex,dbFlag,positive, graphicClassNormal);

               standardAperture.getAperturePoly(m_camCadDatabase.getCamCadData(),complexApertureBlock->getDataList(),polyStruct);

               CTMatrix rotationMatrix;
               rotationMatrix.rotateDegrees(nameDegrees);
               polyStruct->transform(rotationMatrix);
            }
         }
      }

      if (normalizedApertureGeometry != NULL)
      {
         int apertureWidthIndex = m_camCadDatabase.getWidthIndex(&apertureBlock);
         m_widthIndexToNormalizedApertureMap.setAt(apertureWidthIndex,normalizedApertureGeometry);
         
         m_camCadDatabase.getCamCadDoc().RegisterKeyWord(QDERIVED_FROM_APERTURE, 0, VT_INTEGER);
         CString blkNumStr;
         blkNumStr.Format("%d", apertureBlock.getBlockNumber());
         m_camCadDatabase.getCamCadDoc().SetUnknownAttrib(&normalizedApertureGeometry->getDefinedAttributes(), QDERIVED_FROM_APERTURE, blkNumStr, SA_OVERWRITE, NULL);
      }

      retval = (normalizedApertureGeometry != NULL);

      if (retval)
      {
         CString temp;

         if (! m_gerberApertures.Lookup(apertureName,temp))
         {
            m_gerberApertures.SetAt(apertureName,temp);
         }
      }
   }
   else if (normalizedApertureBlock != NULL)
   {
      retval = true;
   }
   else if (defineApertureFlag)
   {
      retval = true;

      CString apertureName = ((apertureBlock.getShape() == apertureUndefined) ? "UndefinedAperture" : "ComplexAperture");
      apertureName.AppendFormat("_%d",m_nextComplexApertureIndex);
      double apertureDiameter = apertureBlock.getSizeA();

      normalizedApertureGeometry = m_camCadDatabase.getDefinedAperture(apertureName,apertureRound,apertureDiameter);
      normalizedApertureGeometry->setDcode(m_nextComplexApertureIndex);

      int apertureWidthIndex = m_camCadDatabase.getWidthIndex(&apertureBlock);
      m_widthIndexToNormalizedApertureMap.setAt(apertureWidthIndex,normalizedApertureGeometry);

      CString temp;

      if (! m_gerberApertures.Lookup(apertureName,temp))
      {
         m_gerberApertures.SetAt(apertureName,temp);
      }

      m_nextComplexApertureIndex--;
   }

   return retval;
}

bool CGerberApertures::getNormalizedAperture(DataStruct& apertureData,const CTMatrix& matrix,
   int insertLayerIndex,bool mirrorLayersFlag,FileLayerStruct& fileLayer,
   BlockStruct*& normalizedApertureGeometry,CBasesVector& apertureBasesVector,
   bool defineApertureFlag)
{
   bool retval = false;

   normalizedApertureGeometry = NULL;
   apertureBasesVector.set();

   CCEtoODBDoc& camCadDoc = m_camCadDatabase.getCamCadDoc();

   if (apertureData.getDataType() == dataTypeInsert)
   {
      InsertStruct* insert = apertureData.getInsert();
      BlockStruct* block = m_camCadDatabase.getBlock(insert->getBlockNumber());

      // use premultiplication
      CTMatrix insertMatrix = insert->getTMatrix() * matrix;
      //bool mirrorFlag = insertMatrix.getMirror();

      if (block->isSimpleAperture())
      {
         int layerIndex = Get_ApertureLayer(&camCadDoc,&apertureData,block,insertLayerIndex);
         int destinationLayerIndex = camCadDoc.getMirroredLayerIndex(layerIndex,mirrorLayersFlag);

         if (camCadDoc.isLayerVisible(layerIndex,mirrorLayersFlag) &&
             LayerInFile(&fileLayer,destinationLayerIndex) )
         {
            retval = getNormalizedAperture(*block,insertMatrix,normalizedApertureGeometry,apertureBasesVector);
         }
      }
   }

   return retval;
}

bool CGerberApertures::createNormalizedWidthApertue(int widthIndex)
{
   bool retval = false;

   if (widthIndex >= 0)
   {
      CTMatrix matrix;
      BlockStruct* normalizedApertureGeometry;
      CBasesVector apertureBasesVector;

      BlockStruct* widthBlock = m_camCadDatabase.getCamCadDoc().getWidthBlock(widthIndex);
      getNormalizedAperture(*widthBlock,matrix,normalizedApertureGeometry,apertureBasesVector);

      retval = true;
   }

   return retval;
}

void CGerberApertures::createGerberApertures(
   CDataList& dataList,CTMatrix& matrix,bool mirrorLayersFlag,
   int insertLayerIndex,FileLayerStruct& fileLayer)
{
   static int depth = 0;
   depth++;
   
   CCEtoODBDoc& camCadDoc = m_camCadDatabase.getCamCadDoc();

   for (CDataListIterator dataIterator(dataList);dataIterator.hasNext();)
   {
      DataStruct* data = dataIterator.getNext();

      if (data->getDataType() == dataTypeInsert)
      {
         InsertTypeTag insertType = data->getInsert()->getInsertType();
         if(insertType != insertTypePcb && !m_StencilThicknessMap->VerifyThicknessData(data))
            continue;

         InsertStruct* insert = data->getInsert();
         BlockStruct* block = m_camCadDatabase.getBlock(insert->getBlockNumber());

         if (block->isSimpleAperture())
         {
            BlockStruct* normalizedApertureGeometry;
            CBasesVector apertureBasesVector;

            getNormalizedAperture(*data,matrix,insertLayerIndex,mirrorLayersFlag,fileLayer,
               normalizedApertureGeometry,apertureBasesVector,true);
         }
         else
         {
            // use premultiplication
            CTMatrix insertMatrix = insert->getTMatrix() * matrix;

            int layerIndex = data->getLayerIndex();

            if (camCadDoc.IsFloatingLayer(layerIndex) || layerIndex == -1)
            {
               layerIndex = insertLayerIndex;
            }

            bool blockMirrorLayersFlag = (mirrorLayersFlag != insert->getLayerMirrored());

            if (block->isComplexAperture())
            {
               insertMatrix = block->getApertureTMatrix() * insertMatrix;
               block = m_camCadDatabase.getBlock(block->getComplexApertureSubBlockNumber());
            }

            createGerberApertures(block->getDataList(),insertMatrix,
               blockMirrorLayersFlag,layerIndex,fileLayer);

            for (POSITION attributePos = data->attributes().GetStartPosition();attributePos != NULL;)
            {
               WORD keywordIndex;
               Attrib* attribute;

               data->attributes().GetNextAssoc(attributePos,keywordIndex,attribute);

               if (attribute->isVisible())
               {
                  createNormalizedWidthApertue(attribute->getPenWidthIndex());
               }
            }
         }
      }
      else if (! data->isHidden())
      {
         // poly and text

         int layerIndex = data->getLayerIndex();

         if (camCadDoc.IsFloatingLayer(layerIndex) && insertLayerIndex != -1)
         {
            layerIndex = insertLayerIndex;
         }

         if (camCadDoc.isLayerVisible(layerIndex,mirrorLayersFlag))
         {
            layerIndex = camCadDoc.getMirroredLayerIndex(layerIndex,mirrorLayersFlag);

            // only write data item if it goes in this file
            if (LayerInFile(&fileLayer,layerIndex))
            {
               if (data->getDataType() == dataTypePoly)
               {
                  CPolyList* polyList = data->getPolyList();

                  for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
                  {
                     CPoly* poly = polyList->GetNext(polyPos);
                     createNormalizedWidthApertue(poly->getWidthIndex());
                  }
               }
               else if (data->getDataType() == dataTypeText)
               {
                  createNormalizedWidthApertue(data->getText()->getPenWidthIndex());
               }
            }
         }
      }
   }

   depth--;
}

CString CGerberApertures::getBaseApertureDescriptor(BlockStruct *normalizedApertureBlk)
{
   CString apNameOrDesc("Unknown");

   if (normalizedApertureBlk != NULL && normalizedApertureBlk->getAttributesRef() != NULL)
   {
      Attrib *a = NULL;
      if ((a = is_attvalue(&m_camCadDatabase.getCamCadDoc(), normalizedApertureBlk->getAttributesRef(), QDERIVED_FROM_APERTURE, 0)) != NULL)
      {
         int blknum = a->getIntValue();
         BlockStruct *derivedFromAp = m_camCadDatabase.getBlock(blknum);
         if (derivedFromAp != NULL)
         {
            apNameOrDesc = derivedFromAp->getName();

            // If derived from an unnamed aperture then build a description
            if (apNameOrDesc.IsEmpty())
            {
               int decimals = GetDecimals(m_camCadDatabase.getCamCadDoc().getSettings().getPageUnits());
               ApertureShapeTag shapeTag = derivedFromAp->getShape();
               CString shapeName = "Unnamed " + apertureShapeToName(shapeTag);
               CString sizeStr;
               if (shapeTag == T_COMPLEX)
               {
                  BlockStruct *cmplxSubBlk = m_camCadDatabase.getBlock( derivedFromAp->getComplexApertureSubBlockNumber() );
                  if (cmplxSubBlk != NULL)
                     sizeStr = cmplxSubBlk->getName();
               }
               else
               {
                  sizeStr.Format("%.*lf", decimals, derivedFromAp->getSizeA());
               }

               switch (shapeTag)
               {
               case T_SQUARE:
               case T_RECTANGLE:
               case T_THERMAL:
               case T_DONUT:
               case T_OBLONG:
                  {
                     CString sizeBstr;
                     sizeBstr.Format("%.*lf", decimals, derivedFromAp->getSizeB());
                     sizeStr += " x " + sizeBstr;
                     break;
                  }
               }

               apNameOrDesc = shapeName + " " + sizeStr;
            }
         }
      }
   }

   return apNameOrDesc;
}

void CGerberApertures::assignUniqueDCodes(CWriteFormat& logfile)
{
   // Case dts0100380269
   // Prior to case 380269 this exporter defined all its normalized apertures and then
   // gave them all new, unique D codes here. The case says we need to use the "Export As"
   // D codes as seen in the Aperture List. 
   // But that can result in duplicate D codes, user might see this as loss of aperture rotation.
   // Exactly that became the subject of ...
   // Case dts0100387139
   // Users aren't liking the "honor D codes in Aperture List" so well. So we'll
   // make a comprimise. A D code in the aperture list will be honored so long as it does not
   // interfere with D code uniqueness. The D codes that matter are the D codes of the
   // normalized apertures. The normalized apertures are derived from the apertures the user
   // sees in the Aperture List, but are not exactly the same apertures. It is possible more
   // than one normalized aperture is created from a single camcad aperture as seen in the
   // Aperture List, and they can not both have the same D code (they did for awhile, which is
   // exactly what this case dts0100387139 is about).

   CString apertureName,temp;
   CArray<int> dcodeTable;
   dcodeTable.SetSize(0,500);
   const int minDcode = 10;
   CString duplicateList;


   // This is nice in that it keeps the whole
   // Initialize D codes by propagating D code from the base camcad aperture that the gerber apertures
   // that are derived form that base. Another step down below will resolve duplicate D codes.
   for (POSITION pos = m_gerberApertures.GetStartPosition();pos != NULL;)
   {
      m_gerberApertures.GetNextAssoc(pos,apertureName,temp);

      BlockStruct* gerberApertureGeometry = m_camCadDatabase.getBlock(apertureName);

      if (gerberApertureGeometry != NULL && gerberApertureGeometry->getAttributesRef() != NULL)
      {
         CString derivedFromGeomName;
         int derivedFromBlockNum = 0;
         Attrib *a = NULL;
         if ((a = is_attvalue(&m_camCadDatabase.getCamCadDoc(), gerberApertureGeometry->getAttributesRef(), QDERIVED_FROM_APERTURE, 0)) != NULL)
            derivedFromBlockNum = a->getIntValue();
         if (derivedFromBlockNum != 0)
         {
            BlockStruct *derivedFromGeometry = m_camCadDatabase.getBlock(derivedFromBlockNum);
            if (derivedFromGeometry != NULL)
            {
               gerberApertureGeometry->setDcode( derivedFromGeometry->getDcode() );
            }
         }
      }
   }

   // Put all existing used D codes and mapped-to geometry block in a table.
   // There are potentially duplicates, we save the first instance of usage.
   for (POSITION pos = m_gerberApertures.GetStartPosition();pos != NULL;)
   {
      m_gerberApertures.GetNextAssoc(pos,apertureName,temp);

      BlockStruct* gerberApertureGeometry = m_camCadDatabase.getBlock(apertureName);

      if (gerberApertureGeometry != NULL)
      {
         int dcode = gerberApertureGeometry->getDcode();

         if (dcode >= minDcode)
         {
            if ((dcode < dcodeTable.GetSize() && dcodeTable.GetAt(dcode) == 0) ||
               dcode >= dcodeTable.GetSize() )
            {
               dcodeTable.SetAtGrow(dcode,gerberApertureGeometry->getBlockNumber());
            }
         }
      }
   }

   // Assign new D codes to those that are zero or those that are duplicate usage
   int nextCandidateDcode = minDcode;

   for (POSITION pos = m_gerberApertures.GetStartPosition();pos != NULL;)
   {
      m_gerberApertures.GetNextAssoc(pos,apertureName,temp);

      BlockStruct* gerberApertureGeometry = m_camCadDatabase.getBlock(apertureName);

      if (gerberApertureGeometry == NULL)
         continue; // yukk

      int dcode = gerberApertureGeometry->getDcode();

      // If D code is out or range or is a duplicate then reset it to zero and
      // subsequently it will get a new D code assigned.

      if (dcode < minDcode)
         dcode = 0;

      // Detect non-unique D codes
      if (dcode >= minDcode && dcode < dcodeTable.GetSize())
      {
         if (dcodeTable.GetAt(dcode) != gerberApertureGeometry->getBlockNumber())
         {
            BlockStruct* alreadyUsingDcodeApGeom = m_camCadDatabase.getBlock(dcodeTable.GetAt(dcode));
            CString      alreadyUsingDcodeApName = alreadyUsingDcodeApGeom != NULL ? alreadyUsingDcodeApGeom->getName() : "Unknown";

            CString wantsToUseDcodeApName = gerberApertureGeometry->getName();

            CString alreadyUsingBaseApName = getBaseApertureDescriptor( alreadyUsingDcodeApGeom );
            CString wantsToUseBaseApName   = getBaseApertureDescriptor( gerberApertureGeometry  );

            CString msg;
            msg.Format("Cannot honor D Code %d for \"%s\" (derived from: %s), it is already used by \"%s\" (derived from: %s).\n",
               dcode,   
               wantsToUseDcodeApName,   wantsToUseBaseApName.IsEmpty()   ? "Unknown" : wantsToUseBaseApName,
               alreadyUsingDcodeApName, alreadyUsingBaseApName.IsEmpty() ? "Unknown" : alreadyUsingBaseApName);
            logfile.write(msg);

            dcode = 0;
         }
      }

      // Assign new unique D code is dcode is now zero

      while (dcode == 0)
      {
         if (nextCandidateDcode >= dcodeTable.GetSize() || dcodeTable.GetAt(nextCandidateDcode) == 0)
         {
            dcode = nextCandidateDcode;
            gerberApertureGeometry->setDcode(dcode);
            dcodeTable.SetAtGrow(dcode,gerberApertureGeometry->getBlockNumber());

            CString derivedFromBaseApNamePhrase;
            CString baseApName = getBaseApertureDescriptor( gerberApertureGeometry  );
            if (!baseApName.IsEmpty())
               derivedFromBaseApNamePhrase = " (derived from: " + baseApName + ")";

            logfile.writef("Assigned D code %d to \"%s\"%s.\n", dcode, gerberApertureGeometry->getName(), derivedFromBaseApNamePhrase);
         }

         ++nextCandidateDcode;
      }

   }
}

CString CGerberApertures::getLegalMacroName(const CString& macroName)
{
   if (macroName.GetLength() > 0 && isdigit(macroName[0]))
   {
      return "_" + macroName;
   }
   else
   {
      return macroName;
   }
}

void CGerberApertures::writeApertureMacros(FILE* file, CWriteFormat& logfile)
{
   CCEtoODBDoc& camCadDoc = m_camCadDatabase.getCamCadDoc();

   assignUniqueDCodes(logfile);

   CString apertureName,temp;

   for (POSITION pos = m_gerberApertures.GetStartPosition();pos != NULL;)
   {
      m_gerberApertures.GetNextAssoc(pos,apertureName,temp);

      BlockStruct* gerberApertureGeometry = m_camCadDatabase.getBlock(apertureName);

      if (gerberApertureGeometry == NULL)
         continue;

      int dcode = gerberApertureGeometry->getDcode();

      if (gerberApertureGeometry->getShape() == apertureComplex)
      {
         BlockStruct* complexApertureGeometry = m_camCadDatabase.getBlock(gerberApertureGeometry->getComplexApertureSubBlockNumber());

         fprintf(file,"%%AM%s*\n",getLegalMacroName(apertureName));

         CTMatrix matrix;
         writeApertureMacro(complexApertureGeometry->getDataList(),matrix);

         fprintf(file,"%%\n");
      }
      else
      {
         double sizeA = gerberApertureGeometry->getSizeA() * m_gerberUnitsPerPageUnit;
         double sizeB = gerberApertureGeometry->getSizeB() * m_gerberUnitsPerPageUnit;

         switch (gerberApertureGeometry->getShape())
         {
         case apertureThermal:
            fprintf(file,"%%AMTHERMALD%d*\n",dcode);
            fprintf(file,"1,1,%1.5lf,0,0*\n",sizeA);
            fprintf(file,"1,0,%1.5lf,0,0*\n",sizeB);
            fprintf(file,"21,0,%1.5lf,0.02,0.0,0.0,45.0*\n" ,sizeA + 0.02);
            fprintf(file,"21,0,%1.5lf,0.02,0.0,0.0,135.0*\n",sizeA + 0.02);
            fprintf(file,"%%\n");

            break;
         case apertureDonut:
            fprintf(file,"%%AMDOD%d*\n"     ,dcode);
            fprintf(file,"1,1,%1.5lf,0,0*\n",sizeA);
            fprintf(file,"1,0,%1.5lf,0,0*\n",sizeB);
            fprintf(file,"%%\n");

            break;
         case apertureTarget:
            fprintf(file,"%%AMTARGET%d*\n"  ,dcode);
            fprintf(file,"1,1,%1.5lf,0,0*\n",sizeA/2);
            fprintf(file,"1,0,%1.5lf,0,0*\n",0.625 * (sizeA/2));
            fprintf(file,"20,1,%1.5lf,0.0,%1.5lf,0.0,%1.5lf,0.0*\n",sizeA * 0.08,sizeA/2.,-sizeA/2.);
            fprintf(file,"20,1,%1.5lf,%1.5lf,0.0,%1.5lf,0.0,0.0*\n",sizeA * 0.08,sizeA/2.,-sizeA/2.);
            fprintf(file,"%%\n");

            break;
         }
      }
   }

   int smallWidthIndex = camCadDoc.getSmallWidthIndex();
   BlockStruct* smallWidthBlock = camCadDoc.getWidthBlock(smallWidthIndex);
   double smallGerberWidth = smallWidthBlock->getSizeA() * m_gerberUnitsPerPageUnit;

   for (POSITION pos = m_gerberApertures.GetStartPosition();pos != NULL;)
   {
      m_gerberApertures.GetNextAssoc(pos,apertureName,temp);

      BlockStruct* gerberApertureGeometry = m_camCadDatabase.getBlock(apertureName);

      if (gerberApertureGeometry == NULL)
         continue;

      int dcode    = gerberApertureGeometry->getDcode();
      double sizeA = gerberApertureGeometry->getSizeA() * m_gerberUnitsPerPageUnit;
      double sizeB = gerberApertureGeometry->getSizeB() * m_gerberUnitsPerPageUnit;

      switch (gerberApertureGeometry->getShape())
      {
      case apertureComplex:
         fprintf(file,"%%ADD%d%s*%%\n",dcode,getLegalMacroName(gerberApertureGeometry->getName()));
         break;
      case apertureRectangle:
         fprintf(file,"%%ADD%dR,%1.5lfX%1.5lf*%%\n",dcode,sizeA,sizeB);
         break;
      case apertureSquare:
         fprintf(file,"%%ADD%dR,%1.5lfX%1.5lf*%%\n",dcode,sizeA,sizeA);
         break;
      case apertureTarget:
         fprintf(file,"%%ADD%dTARGET%d*%%\n",dcode,dcode);
         break;
      case apertureThermal:
         fprintf(file,"%%ADD%dTHERMALD%d*%%\n",dcode,dcode);
         break;
      case apertureDonut:
         fprintf(file,"%%ADD%dDOD%d*%%\n",dcode,dcode);
         break;
      case apertureOctagon:
         fprintf(file,"%%ADD%dP,%1.5lfX8X22.5*%%\n",dcode,sizeA);
         break;
      case apertureOblong:
         fprintf(file,"%%ADD%dO,%1.5lfX%1.5lf*%%\n",dcode,sizeA,sizeB);
         break;
      case apertureRound:
         // do not allow an aperture to be smaller than small width
         fprintf(file,"%%ADD%dC,%1.5lf*%%\n",dcode,((sizeA < smallGerberWidth) ? smallGerberWidth : sizeA));
         break;
      }
   }
}

void CGerberApertures::writeApertureMacro(CDataList& dataList,const CTMatrix& matrix)
{
   CTMatrix gerberUnitMatrix(matrix);
   gerberUnitMatrix.scale(m_gerberUnitsPerPageUnit);
   bool mirrorFlag = matrix.getMirror();
   CCEtoODBDoc& camCadDoc = m_camCadDatabase.getCamCadDoc();

   for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = dataList.GetNext(pos);

      if (data->getDataType() == dataTypePoly)
      {
         int layerIndex = data->getLayerIndex();
         layerIndex = camCadDoc.getMirroredLayerIndex(layerIndex,mirrorFlag);

         CPoly *poly;

         // loop thru polys
         for (POSITION polyPos = data->getPolyList()->GetHeadPosition();polyPos != NULL;)
         {
            poly = data->getPolyList()->GetNext(polyPos);

            if (poly->isHidden()) continue;

            BlockStruct* widthBlock        = m_camCadDatabase.getCamCadDoc().getWidthBlock(poly->getWidthIndex());
            ApertureShapeTag apertureShape = widthBlock->getShape();
            double widthInPageUnits        = widthBlock->getSizeA();

            bool onFlag = !(poly->isVoid()) && !(data->isNegative());

            double cx, cy, radius;

            if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
            {
               double diameter = (2.*radius + widthInPageUnits) * m_gerberUnitsPerPageUnit;
               gerberUnitMatrix.transform(cx,cy);

               wgerb_write("1,%d,%1.5lf,%1.5lf,%1.5lf*\n",onFlag,diameter,cx,cy);
            }
            else if (! poly->isClosed())
            {
               writeApertureMacroThickPolyLine(*poly,gerberUnitMatrix,apertureShape,widthInPageUnits,onFlag);
            }
            else
            {
               if (widthInPageUnits > 0.)
               {
                  writeApertureMacroThickPolyLine(*poly,gerberUnitMatrix,apertureShape,widthInPageUnits,onFlag);
               }

               writeApertureMacroClosedPoly(*poly,gerberUnitMatrix,onFlag);
            }
         }
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         InsertStruct* insert = data->getInsert();
         BlockStruct* block = m_camCadDatabase.getBlock(insert->getBlockNumber());

         // use premultiplication
         CTMatrix insertMatrix = insert->getTMatrix() * matrix;

         if (! block->isAperture())
         {
            writeApertureMacro(block->getDataList(),insertMatrix);
         }
      }
   }
}

void CGerberApertures::writeApertureMacroThickPolyLine(CPoly& poly,const CTMatrix& gerberUnitMatrix,
   ApertureShapeTag apertureShape,double widthInPageUnits,bool onFlag)
{
   // For use in Gerber "aperture macro" definition
   int cnt = 0;
   Point2 point,previousPoint;
   double widthInGerberUnits     = widthInPageUnits * m_gerberUnitsPerPageUnit;
   double halfWidthInGerberUnits = widthInGerberUnits/2.;

   for (POSITION pntPos = poly.getPntList().GetHeadPosition();pntPos != NULL;cnt++)
   {
      point = *(poly.getPntList().GetNext(pntPos));
      point.transform(gerberUnitMatrix);

      if (cnt > 0)
      {
         // add line end length
         if (apertureShape == apertureSquare || apertureShape == apertureRectangle)
         {
            double newP1x, newP1y, newP2x, newP2y;
            FindPointOnLine(previousPoint.x,previousPoint.y,point.x        ,point.y        ,-halfWidthInGerberUnits,&newP1x,&newP1y);
            FindPointOnLine(point.x        ,point.y        ,previousPoint.x,previousPoint.y,-halfWidthInGerberUnits,&newP2x,&newP2y);

            wgerb_write("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
               onFlag,widthInGerberUnits,newP1x,newP1y,newP2x,newP2y);
         }
         else
         {
            if (fabs(previousPoint.bulge) > SMALLNUMBER)
            {
               // Cannot output arc so break the arc down to little segment
               double cx,cy,r,sa;
               double da = atan(previousPoint.bulge) * 4.;
               ArcPoint2Angle(previousPoint.x,previousPoint.y,point.x,point.y,da,&cx,&cy,&r,&sa);

               // make positive start angle.
               sa = normalizeRadians(sa);

               int ppolycnt = 255;
               Point2 ppoly[255];

               // start center
               ArcPoly2(previousPoint.x,previousPoint.y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(5));

               for (int i=0; i<ppolycnt-1; i++)
               {
                  Point2 *p1 = &ppoly[i];
                  Point2 *p2 = &ppoly[i+1];

                  wgerb_write("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
                     onFlag,widthInGerberUnits,p1->x,p1->y,p2->x,p2->y);

                  // circle on top
                  wgerb_write("1,%d,%1.5lf,%1.5lf,%1.5lf*\n",onFlag,widthInGerberUnits,p1->x,p1->y);

                  // circle on bottom
                  wgerb_write("1,%d,%1.5lf,%1.5lf,%1.5lf*\n",onFlag,widthInGerberUnits,p2->x,p2->y);
               }
            }
            else
            {
               wgerb_write("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
                  onFlag,widthInGerberUnits, previousPoint.x,previousPoint.y,point.x,point.y);

               // circle on top
               wgerb_write("1,%d,%1.5lf,%1.5lf,%1.5lf*\n",onFlag,widthInGerberUnits,previousPoint.x,previousPoint.y);

               // circle on bottom
               wgerb_write("1,%d,%1.5lf,%1.5lf,%1.5lf*\n",onFlag,widthInGerberUnits,point.x,point.y);
            }
         }
      }

      previousPoint = point;
   }
}

void CGerberApertures::writeApertureMacroClosedPoly(CPoly& poly,const CTMatrix& gerberUnitMatrix,bool onFlag)
{
   // For use in Gerber "aperture macro" definition
   int cnt = 0;
   Point2 point,previousPoint;
   CString output;

   // first draw a outline with the correct Gerber aperture, because the
   // polyfill only fills to the outline of the poly.
   for (POSITION pntPos = poly.getPntList().GetHeadPosition();pntPos != NULL;)
   {
      point = *(poly.getPntList().GetNext(pntPos));
      point.transform(gerberUnitMatrix);

      if (cnt > 0 && fabs(previousPoint.bulge) > SMALLNUMBER)
      {
         double cx, cy, r, sa;
         double da = atan(previousPoint.bulge) * 4.;
         ArcPoint2Angle(previousPoint.x,previousPoint.y,point.x,point.y,da,&cx,&cy,&r,&sa);

         // make positive start angle.
         sa = normalizeRadians(sa);

         int ppolycnt = 255;
         Point2 ppoly[255];

         // start center
         ArcPoly2(previousPoint.x,previousPoint.y,cx,cy,da,1.0,ppoly,&ppolycnt,degreesToRadians(5.));

         for (int i = 0;i < ppolycnt;i++)
         {
            Point2* pp = &ppoly[i];

            output.AppendFormat(",%1.5lf,%1.5lf",pp->x,pp->y);

            cnt++;
         }
      }
      else
      {
         output.AppendFormat(",%1.5lf,%1.5lf",point.x,point.y);

         cnt++;
      }

      previousPoint = point;
   }

   wgerb_write("4,%d,%d",onFlag,cnt - 1);
   wgerb_write(output);
   wgerb_write(",0.*\n");
}

//_____________________________________________________________________________
CGerberWriter::CGerberWriter(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_smallGerberWidth(0.)
{
   m_logFile       = NULL;
   m_messageFilter = NULL;

   // settings
   m_fillPolygons           = false;
   m_polygonFillDcode       = 0;
   m_ordinateDigits         = 2;
   m_ordinateDecimalPlaces  = 3;
   m_gerberUnits            = pageUnitsInches;
   m_gerberFormat           = gerberFormatRs274x;
   m_gerberUnitsPerPageUnit = 1.;
   m_useStencilThickness = false;
   m_StencilThicknessMap = new CStencilThicknessMap(m_camCadDoc,m_useStencilThickness);
}

CGerberWriter::~CGerberWriter()
{
   delete m_messageFilter;
   delete m_logFile;
   delete m_StencilThicknessMap;
}

void CGerberWriter::set(FormatStruct& format)
{
   m_fillPolygons          = (format.GR.PolyFill != 0);
   m_polygonFillDcode      = format.GR.fillDCode;
   m_ordinateDigits        = format.GR.digits;
   m_ordinateDecimalPlaces = format.GR.decimal;
   m_gerberUnits           = ((format.GR.units == 0) ? pageUnitsInches : pageUnitsMilliMeters);
   m_useStencilThickness = (format.GR.layerStencilThickness)?true:false;

   switch (format.GR.format)
   {
   case gerberFormatRs274:     m_gerberFormat = gerberFormatRs274;     break;
   case gerberFormatRs274x:    m_gerberFormat = gerberFormatRs274x;    break;
   case gerberFormatFire9xxx:  m_gerberFormat = gerberFormatFire9xxx;  break;
   default:                    m_gerberFormat = gerberFormatRs274x;    break;
   }

   wgerb_Set_Format(&format);

   m_gerberUnitsPerPageUnit = 1. / m_camCadDoc.convertToPageUnits(m_gerberUnits,1.);
   m_StencilThicknessMap->setlayerThicknessflag(m_useStencilThickness);

}

CWriteFormat& CGerberWriter::getLogFile()
{
   if (m_logFile == NULL)
   {
      CFilePath logFilePath(GetLogfilePath("Gerber.log"));

      CStdioFileWriteFormat* logFile = new CStdioFileWriteFormat(logFilePath.getPath(),512);
      m_logFile = logFile;
   }

   return *m_logFile;
}

CMessageFilter& CGerberWriter::getMessageFilter()
{
   if (m_messageFilter == NULL)
   {
      m_messageFilter = new CMessageFilter(messageFilterTypeMessage);
   }

   return *m_messageFilter;
}

CProgressDlg& CGerberWriter::getProgressDialog()
{
   extern CProgressDlg* progress;

   return *progress;
}

int CGerberWriter::getDcode(int widthIndex)
{
   int dcode = m_widthIndexToNormalizedApertureMap.getDcode(widthIndex);

   if (dcode < 0)
   {
      dcode = 999;
 
      getMessageFilter().formatMessageBoxApp("Could not find normalized aperture for width %d",widthIndex);
   }

   return dcode;
}

void CGerberWriter::writeGerberFiles(CFileLayerList& fileLayerList)
{
   BuildThicknessLayerMap();

   for (POSITION fileLayerPos = fileLayerList.GetHeadPosition();fileLayerPos != NULL;)
   {
      FileLayerStruct* fileLayer = fileLayerList.GetNext(fileLayerPos);
      
      int layercnt = m_StencilThicknessMap->hasThicknessLayer(fileLayer);
      if (layercnt == 0)
      {
         //Layers don't consider thickness 
         m_StencilThicknessMap->setGerberLayerType(NonStencilLayer);
         writeGerberFiles(fileLayer, fileLayer->getFilePath());
      }
      else
      {
         //Stencil Layers has data without thickness attributes
         // Orignal:  m_StencilThicknessMap->setGerberLayerType(StencilLayer_NoThickness);
         // The original MOVES stencil holes with thickness to thickness-specific output file.
         // DR 741085 says the with-thickness holes should be COPIED to thickness-specific lyaer file,
         // and all holes thickness or not should be in original surface layer file.
         m_StencilThicknessMap->setGerberLayerType(StencilLayer_AnyThickness);  // Including no thickness.
         writeGerberFiles(fileLayer, fileLayer->getFilePath());

         //Stencil Layers thickness has data with thickness attributes
         writeGerberFilesByThickness(fileLayer, layercnt);         
      }
   }//for
}

void CGerberWriter::writeGerberFiles(FileLayerStruct* fileLayer, CString fileName)
{
   getProgressDialog().SetStatus(fileName);

   getLogFile().writef("\n--------------------------------------------------------------------------------\n");
   getLogFile().writef("Processing file: %s\n\n", fileName);

   int status = wgerb_Graph_File_Open(fileName);

   if (status != 1)
   {
      formatMessageBoxApp("Could not open the file '%s' for writing.",fileName);

      return;
   }

   m_camCadDoc.purgeUnusedWidthsAndBlocks(false);

   CCamCadDatabase camCadDatabase(m_camCadDoc);
   CGerberApertures gerberApertures(camCadDatabase,m_widthIndexToNormalizedApertureMap,m_gerberUnitsPerPageUnit,m_StencilThicknessMap);

   scanForApertures(gerberApertures,*fileLayer);

   wgerb_write("G04 CAMCAD photoplot definition file *\n");

   wgerb_write("%%FSLAX%d%dY%d%d*%%\n",
      m_ordinateDigits,m_ordinateDecimalPlaces,
      m_ordinateDigits,m_ordinateDecimalPlaces);

   wgerb_write("%%MO%s*%%\n",m_gerberUnits == pageUnitsInches ? "IN" : "MM");

   wgerb_write("%%IPPOS*%%\n");

   if (m_gerberFormat == gerberFormatRs274x)
   {
      writeApertureMacros(gerberApertures);
   }

   initializeGerberState();
   writeGerberData(gerberApertures,*fileLayer);

   wgerb_Graph_File_Close();
}

void CGerberWriter::writeGerberFilesByThickness(FileLayerStruct* fileLayer, int thicknessLayerCnt)
{
   for(POSITION LayerPos = fileLayer->LayerList.GetHeadPosition();LayerPos != NULL;)
   {
      LayerStruct *layer = fileLayer->LayerList.GetNext(LayerPos);
      CStringArray *stencilLayerList = NULL;
      if(layer && m_StencilThicknessMap->Lookup(layer->getLayerIndex(),stencilLayerList) && stencilLayerList)
      {
         for(int idx = 0; idx < stencilLayerList->GetCount(); idx++)
         {                     
            CString thickness = stencilLayerList->GetAt(idx);                     
            if(!thickness.IsEmpty())
            {
               int filePos = fileLayer->getFilePath().Find(".gbr");
               CString fileName;
               fileName.Format("%s_%s%s.gbr",fileLayer->getFilePath().Mid(0,filePos),
                  (thicknessLayerCnt > 1)?layer->getName():"",thickness);
               
               m_StencilThicknessMap->setGerberLayerType(StencilLayer_HasThickness);
               m_StencilThicknessMap->SetTargetThickness(thickness);

               writeGerberFiles(fileLayer, fileName);
            }
         }//for
      }
   }//for
}

bool CGerberWriter::hasVoidPoly(DataStruct *np)
{
   // traverse through polystruct to see if it contains void polys
   POSITION polyPos = np->getPolyList()->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = np->getPolyList()->GetNext(polyPos);
      if(poly && poly->isVoid())
         return true;

   }//while

   return false;
}

// Sort PolyStructs for DR dts0100559864
// PolyStruct with void polys placed front position
// PolyStruct without void polys placed later postion
void CGerberWriter::SortPolyStructs(CDataList &DataList, CDataList& targetDataList)
{
   int nVoidPosHead = 0, nVoidPosTail = 0;
   CArray<POSITION> posList;
   
   for (POSITION dataPos = DataList.GetHeadPosition(); dataPos; DataList.GetNext(dataPos))
   {
      DataStruct  *data = DataList.GetAt(dataPos);
      if(data)
      {
         POSITION targetPos = targetDataList.AddTail(data);
         if(data->getDataType() == T_POLY)
         {
            posList.Add(targetPos);
            nVoidPosHead = posList.GetCount();

            //Sort DataList based on Void/non-Void
            if(hasVoidPoly(data))
            {   
               // When void polystruct is found and behind non-void polystruct
               // exchange position of void and non-viod polystructs
               if(nVoidPosTail < nVoidPosHead - 1)
               {                        
                  POSITION nVoidPos = posList.GetAt(nVoidPosTail);
                  DataStruct *nVoidData = targetDataList.GetAt(nVoidPos);
                  targetDataList.SetAt(nVoidPos,data);
                  targetDataList.SetAt(targetPos,nVoidData);
               }
               
               nVoidPosTail += (nVoidPosTail < nVoidPosHead)? 1:0;

            }/*if isVoid*/      
         }//if T_POLY         
      }//if data      
   }//while

   posList.RemoveAll();
}


void CGerberWriter::writeGerberData(CGerberApertures& gerberApertures,FileLayerStruct& fileLayer)
{
   COperationProgress progress;
   int count = 0;

   for (int pass = 1;pass <= 2;pass++)
   {
      for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
      {
         FileStruct* file = m_camCadDoc.getFileList().GetNext(filePos);

         if (file->isShown())
         {
            BlockStruct* fileBlock = file->getBlock();

            if (fileBlock != NULL)
            {
               if (pass == 1)
               {
                  count += fileBlock->getDataList().GetCount();
               }
               else
               {
                  bool mirrorLayersFlag = file->isMirrored();
                  writeGerberData(gerberApertures, fileLayer, fileBlock, file->getTMatrix(), mirrorLayersFlag, &progress);
               }
            }
         }
      }

      if (pass == 1)
      {
         progress.setLength(count);
      }
   }
}

void CGerberWriter::writeGerberData(CGerberApertures& gerberApertures, FileLayerStruct& fileLayer, BlockStruct *block, CTMatrix &mat, bool mirrorLayersFlag, COperationProgress *progress)
{
   CDataList targetDataList(true);
   SortPolyStructs(block->getDataList(), targetDataList);

   writeGerberDataList(targetDataList, mat, gerberApertures, fileLayer, mirrorLayersFlag, -1, progress);

   targetDataList.RemoveAll();
}

void CGerberWriter::writeGerberDataList(CDataList& dataList,const CTMatrix& matrix,
   CGerberApertures& gerberApertures,FileLayerStruct& fileLayer,
   bool mirrorLayersFlag,int insertLayerIndex,COperationProgress* progress)
{
   for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = dataList.GetNext(pos);
      
      if (progress != NULL)
      {
         progress->incrementProgress();
      }

      if (data->isHidden())
      {
         continue;
      }

      int layerIndex = data->getLayerIndex();

      if (m_camCadDoc.IsFloatingLayer(layerIndex) && insertLayerIndex != -1)
      {
         layerIndex = insertLayerIndex;
      }

      if (data->getDataType() == dataTypeInsert)
      {
         InsertTypeTag insertType = data->getInsert()->getInsertType();
         if(insertType != insertTypePcb && !m_StencilThicknessMap->VerifyThicknessData(data))
            continue;

         InsertStruct* insert = data->getInsert();
         int blockNumber = insert->getBlockNumber();
         BlockStruct* block = m_camCadDoc.getBlockAt(blockNumber);

         if (block->isSimpleAperture())
         {
            bool apertureMirrorLayersFlag = (mirrorLayersFlag != data->getInsert()->getLayerMirrored());

            layerIndex = Get_ApertureLayer(&m_camCadDoc,data,block,insertLayerIndex);
            int destinationLayerIndex = m_camCadDoc.getMirroredLayerIndex(layerIndex,apertureMirrorLayersFlag);

            // only write data item if it goes in this file
            if (m_camCadDoc.isLayerVisible(layerIndex,apertureMirrorLayersFlag) &&
                LayerInFile(&fileLayer,destinationLayerIndex)           )
            {
               BlockStruct* normalizedApertureGeometry;
               CBasesVector apertureBasesVector;

               gerberApertures.getNormalizedAperture(*data,matrix,insertLayerIndex,apertureMirrorLayersFlag,fileLayer,
                                                         normalizedApertureGeometry,apertureBasesVector);

               if (normalizedApertureGeometry != NULL)
               {
                  CPoint2d origin(apertureBasesVector.getOrigin());
                  Point2 point(origin.x * m_gerberUnitsPerPageUnit,origin.y * m_gerberUnitsPerPageUnit);

                  setGerberModeNegativeLayerPolarity(data->isNegative());
                  wgerb_Flash_App(point,normalizedApertureGeometry->getDcode());
               }
               else
               {
                  getMessageFilter().formatMessageBoxApp("Could not find normalized aperture for geometry %d, '%s'",
                     block->getBlockNumber(),block->getName());
               }
            }
         }
         else
         {
            // use premultiplication
            CTMatrix insertMatrix = insert->getTMatrix() * matrix;

            bool blockMirrorLayersFlag = (mirrorLayersFlag != insert->getLayerMirrored());

            drawGerberAttributes(data->attributes(),insertMatrix,fileLayer,blockMirrorLayersFlag);

            if (block->isComplexAperture())
            {
               insertMatrix = block->getApertureTMatrix() * insertMatrix;
               block = m_camCadDoc.getBlockAt(block->getComplexApertureSubBlockNumber());
            }

            writeGerberDataList(block->getDataList(),insertMatrix,gerberApertures,fileLayer,
               blockMirrorLayersFlag,layerIndex);
         }
      }
      else
      {
         if (! m_camCadDoc.isLayerVisible(layerIndex,mirrorLayersFlag))
         {
            continue;
         }

         layerIndex = m_camCadDoc.getMirroredLayerIndex(layerIndex,mirrorLayersFlag);

          // only write data item if it goes in this file
         if (! LayerInFile(&fileLayer,layerIndex))
         {
            continue;
         }

         if (data->getDataType() == dataTypePoly)
         {
            drawGerberPoly(*data,matrix);
         }
         else if (data->getDataType() == dataTypeText)
         {
            drawGerberText(*data,matrix);
         }
      }
   }
}

double CGerberWriter::getSmallGerberWidth()
{
   if (m_smallGerberWidth <= 0.)
   {
      int smallWidthIndex = m_camCadDoc.getSmallWidthIndex();
      BlockStruct* smallWidthBlock = m_camCadDoc.getWidthBlock(smallWidthIndex);
      double m_smallGerberWidth = smallWidthBlock->getSizeA() * m_gerberUnitsPerPageUnit;
   }

   return m_smallGerberWidth;
}

void CGerberWriter::drawGerberPoly(DataStruct& data,CTMatrix matrix)
{
   CPolyList* polyList = data.getPolyList();
   matrix.scale(m_gerberUnitsPerPageUnit);
   bool mirrorFlag = matrix.getMirror();
   double tolerance = m_camCadDoc.convertToPageUnits(pageUnitsInches,.00001);
   int zeroWidthIndex = m_camCadDoc.getZeroWidthIndex();

   if (data.getEntityNumber() == 598052)
   {
      int iii = 3;
   }

   for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
   {
      CPoly* poly = polyList->GetNext(polyPos);

      if (poly->isHidden())
      {
         continue;
      }

      int widthIndex = poly->getWidthIndex();
      double width = m_camCadDoc.getWidth(widthIndex);
      int dcode = getDcode(widthIndex);

      setGerberModeNegativeLayerPolarity(data.isNegative() != poly->isVoid());

      bool outlinePolyFlag = true;  // for testing
      bool polyOutlinedFlag = false;

      if (outlinePolyFlag && poly->isFilled() && width > 0.)
      {
         CPoly outlinePoly(*poly);
#ifdef ORIGINAL_STUFF
         if (outlinePoly.convertToOutline(width,tolerance,zeroWidthIndex))
         {
            polyOutlinedFlag = true;

            drawGerberPoly(outlinePoly,matrix,true);
         }
#else
         // Fixes DR 500065
         // the CPoly->convertToOutline does a bad job on certain curves that
         // Stencil Generator makes. They are wobbly curves, the SG really needs
         // work in that area, but that is a big deal. The simple CPoly->shrink function
         // is adequate for closed shapes, and seems to do a better job than
         // convertToOutline, so we'll go with that.
         if (outlinePoly.shrink(-width/2.0, m_camCadDoc.getPageUnits(), true))
         {
            outlinePoly.setWidthIndex(zeroWidthIndex);
            polyOutlinedFlag = true;
            drawGerberPoly(outlinePoly,matrix,true);
         }

#endif

      }

      if (! polyOutlinedFlag)
      {
         if (poly->isFilled())
         {
            drawGerberPoly(*poly,matrix,true);
         }

         if (width > getSmallGerberWidth() || !poly->isFilled())
         {
            wgerb_Load_Aperture(dcode);

            drawGerberPoly(*poly,matrix,false);
         }
      }
   }
}

void CGerberWriter::drawGerberPoly(CPoly& poly,const CTMatrix& matrix,bool fillFlag)
{
   bool mirrorFlag = matrix.getMirror();
   CPnt pnt,previousPnt;
   CPoint point,previousPoint;
   int pointIndex = 0;

   setGerberModePolyFill(fillFlag);

   for (POSITION vertexPos = poly.getHeadVertexPosition();vertexPos != NULL;pointIndex++)
   {
      pnt = *(poly.getNextVertex(vertexPos));
      pnt.transform(matrix);
      point = wgerb_convertPoint(Point2(pnt));

      if (pointIndex > 0)
      {
         // if point == previousPoint then a complete circle will be drawn
         // Case #2178 - knv 20060929.1655
         if (fabs(previousPnt.bulge) > SMALLNUMBER && point != previousPoint)
         {
            double cx, cy, r, sa;
            double da = atan(previousPnt.bulge) * 4;
            ArcPoint2Angle(previousPnt.x, previousPnt.y, pnt.x, pnt.y, da, &cx, &cy, &r, &sa);

            wgerb_Arc(Point2(cx,cy),r,sa,da,wgerb_getCurrentAperture(),true);
         }
         else
         {
            wgerb_Pen_Down(pnt);
         }
      }
      else
      {
         wgerb_Pen_Up(pnt);
      }

      previousPnt   = pnt;
      previousPoint = point;
   }

   setGerberModePolyFill(false);
}

void CGerberWriter::drawGerberAttributes(CAttributes& attributes,const CTMatrix& matrix,FileLayerStruct& fileLayer,
   bool mirrorLayersFlag)
{
   for (POSITION pos = attributes.GetStartPosition();pos != NULL;)
   {
      WORD keyword;
      Attrib* attribute = NULL;
      attributes.GetNextAssoc(pos, keyword, attribute);

      CString attributeValue = get_attvalue_string(&m_camCadDoc, attribute);

      if (! attribute->isVisible())
      {
         continue;
      }

      if (! m_camCadDoc.isLayerVisible(attribute->getLayerIndex(), mirrorLayersFlag))
      {
         continue;
      }

      int layer = m_camCadDoc.getMirroredLayerIndex(attribute->getLayerIndex(), mirrorLayersFlag);

      if (! LayerInFile(&fileLayer, layer)) // only write data item if it goes in this file
      {
         continue;
      }

      double height = attribute->getHeight() * m_gerberUnitsPerPageUnit;
      double width  = attribute->getWidth()  * m_gerberUnitsPerPageUnit;

      if (height < SMALLNUMBER || width < SMALLNUMBER)
         continue;

      CBasesVector basesVector = attribute->getBasesVector();
      CTMatrix textMatrix = basesVector.getTransformationMatrix() * matrix;

      switch (attribute->getValueType())
      {
      case VT_INTEGER:
         attributeValue.Format("%d", attribute->getIntValue());
         break;

      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         attributeValue.Format("%lg", attribute->getDoubleValue());
         break;

      case VT_STRING:
         break;
      }

      // Normalize the text alignment
      TextStruct text(attributeValue, attribute->isProportionallySpaced());
      text.setWidth(width);
      text.setHeight(height);
      text.setHorizontalPosition(attribute->getHorizontalPosition());
      text.setVerticalPosition(attribute->getVerticalPosition());

      drawGerberText(text,textMatrix,false);
   }

   return;
}

void CGerberWriter::BuildThicknessLayerMap()
{
   if(!m_useStencilThickness)
      return;

   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = m_camCadDoc.getFileList().GetNext(filePos);

      // We check isShown here, so we process files the user sees.
      // In the second level call with fileBlock, we may process file's block for
      // a file that is not "shown" per se. E.g. if panel is shown the pcb's will be
      // drawn for user, but the pcb's file maybe not be "shown" in sense of file list on/off.

      if (file && file->isShown())
      {
         BlockStruct* fileBlock = file->getBlock();
         BuildThicknessLayerMap(fileBlock);
      }
      //else
      //{
      //   if (file) file->setShow(true);
      //}
   }
}

void CGerberWriter::BuildThicknessLayerMap(BlockStruct *block)
{
   // We aren't looking at the block type and handling them differently, but
   // we are expecting panel and pcb blocks. If it is a panel block then we
   // need to also process the inserted pcb blocks. We just do this by looking
   // for pcb inserts, regardless of what is owning the pcb insert.

   if (block != NULL)
   {
      for(POSITION dataPos = block->getHeadDataInsertPosition(); dataPos != NULL; /*-*/)
      {
         DataStruct *data = block->getNextDataInsert(dataPos);
         if (data != NULL)
         {
            if (data->isInsertType(insertTypePcb))
            {
               // Get pcb's block and call recursively to process it. Note that is panel "is shown"
               // then typically the inserted pcb's files are not "shown" as per the file lists. But
               // they are drawn for the user as part of the panel. That the parent is shown and we 
               // get to this insert is enough to cause inclusion.
               BlockStruct *insertedBlock = this->m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
               BuildThicknessLayerMap(insertedBlock);
            }
            else
            {
               // Normal data, could be insert, poly, whatever. All execpt pcb insert are handled the same.
               m_StencilThicknessMap->AddThicknessLayer(data);
            }
         }
      }
   }
}

void write_text(CCEtoODBDoc* doc,TextStruct& text,double insert_x, double insert_y,
                double rotation,bool mirror,double scale,int dcode)
{
   Mat2x2      m;
   RotMat2(&m,rotation * (mirror?-1:1));

   Point2 point;

   point.x = text.getPnt().x * scale;

   if (mirror) point.x = -point.x;

   point.y = text.getPnt().y * scale;
   TransPoint2(&point, 1, &m, insert_x, insert_y);

   double text_rot = rotation + text.getRotation();

	CDoubleArray lineLengths;
	double spaceratio = doc->getSettings().getTextSpaceRatio(); // turn percent into a decimal
	double maxLineLength = text.getMaxLineLengthInFontUnits(spaceratio, &lineLengths);
	int lineCount = lineLengths.GetSize();
	CPoint2d initialCharacterPosition = text.getInitialCharacterPosition(maxLineLength, lineCount, spaceratio);

	Point2 initialPosPoint;
	initialPosPoint.x = initialCharacterPosition.x;

	if (mirror)
		initialPosPoint.x = -initialPosPoint.x;

	initialPosPoint.y = initialCharacterPosition.y;

	Mat2x2      mtext_rot;
	RotMat2(&mtext_rot, text_rot);
	TransPoint2(&initialPosPoint, 1, &mtext_rot, 0, 0);

	point.x += initialPosPoint.x * scale;
	point.y += initialPosPoint.y * scale;

   // text is mirrored if mirror is set or text.mirror but not if none or both
   bool textMirrorFlag = text.getResultantMirror(mirror);

   if (text.getMirrorDisabled())
		textMirrorFlag = false;

	FontStruct* fontStruct = CFontList::getFontList().getFont(text.getFontNumber());

	write_gerber_text(doc,text.getText(), point.x, point.y,
         text.getHeight() * scale, text.getWidth() * scale, text_rot,
         text.isProportionallySpaced(), text.getOblique(), textMirrorFlag,
         dcode, fontStruct);
}

void CGerberWriter::drawGerberText(TextStruct& text,CTMatrix matrix,bool negativeFlag)
{
   int widthIndex = text.getPenWidthIndex();
   int dcode = getDcode(widthIndex);

   wgerb_Load_Aperture(dcode);

   setGerberModeNegativeLayerPolarity(negativeFlag);

   CBasesVector textBasesVector;
   textBasesVector.transform(matrix);

   write_text(&m_camCadDoc,text,textBasesVector.getOrigin().x,textBasesVector.getOrigin().y,
                textBasesVector.getRotationRadians(),textBasesVector.getMirror(),
                textBasesVector.getScale() * m_gerberUnitsPerPageUnit,dcode);
}

void CGerberWriter::drawGerberText(DataStruct& data,CTMatrix matrix)
{
   TextStruct* text = data.getText();

   drawGerberText(*text,matrix,data.isNegative());
}

void CGerberWriter::initializeGerberState()
{
   m_gerberModeNegativePolarity = boolUnknown;
   m_gerberModeFill             = false;

   setGerberModeNegativeLayerPolarity(false);
}

void CGerberWriter::setGerberModeNegativeLayerPolarity(bool negativePolarityFlag)
{
   if (negativePolarityFlag && m_gerberModeNegativePolarity != boolTrue)
   {
      wgerb_write("%%LPC*%%\n");
      m_gerberModeNegativePolarity = boolTrue;
   }
   else if (!negativePolarityFlag && m_gerberModeNegativePolarity != boolFalse)
   {
      wgerb_write("%%LPD*%%\n");
      m_gerberModeNegativePolarity = boolFalse;
   }
}

void CGerberWriter::setGerberModePolyFill(bool fillFlag)
{
   if (fillFlag != m_gerberModeFill)
   {
      m_gerberModeFill = fillFlag;

      wgerb_write(m_gerberModeFill ? "G36*\n" : "G37*\n");
   }
}

void CGerberWriter::scanForApertures(CGerberApertures& gerberApertures,FileLayerStruct& fileLayer)
{
   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = m_camCadDoc.getFileList().GetNext(filePos);
      BlockStruct* fileBlock = file->getBlock();

      if (file->isShown() && fileBlock != NULL)
      {
         bool mirrorLayersFlag = file->isMirrored();

         gerberApertures.createGerberApertures(fileBlock->getDataList(),file->getTMatrix(),
            mirrorLayersFlag,-1,fileLayer);
      }
   }
}

void CGerberWriter::writeApertureMacros(CGerberApertures& gerberApertures)
{
   for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = m_camCadDoc.getFileList().GetNext(filePos);
      BlockStruct* fileBlock = file->getBlock();

      if (file->isShown() && fileBlock != NULL)
      {
         gerberApertures.writeApertureMacros(wgerb_getFile(), this->getLogFile());
      }
   }
}

/******************************************************************************
* write_gerber_text
*/
static int write_gerber_text(CCEtoODBDoc* doc,const char *text, double insert_x, double insert_y,
                  double charheight, double charwidth, double angle,
                  int proportional, int oblique, bool mirrorFlag, int dcode, FontStruct* fontStruct)
{
   const double spaceratio = doc->getSettings().getTextSpaceRatio(); // turn percent into a decimal

   Mat2x2 m;
   RotMat2(&m, angle);

   double x_offset = 0;
   double y_offset = 0;

   for (unsigned int i=0; i<strlen(text); i++)
   {
      if (text[i] == ' ')
      {
         x_offset += (charwidth) + (SPACE * charheight);
         continue;
      }

      if (text[i] == '\n')
      {
         x_offset = 0;
         y_offset -= charheight * (1.0 + spaceratio);
         continue;
      }

      // do not do control char.
      if (text[i] < ' ')
         continue;

      Point2 offset;
      offset.x = x_offset;
      offset.y = y_offset;

      if (mirrorFlag)
         offset.x = -offset.x;

      TransPoint2(&offset, 1, &m, 0, 0);

      Gerb_WriteChar(fontStruct->getCharacterData(text[i]), insert_x + offset.x, insert_y + offset.y,
                  angle, charheight, mirrorFlag, oblique, dcode);

      // Find offset from insertion point for this character
      if (proportional)
         x_offset += (fontStruct->getCharacterWidth(text[i]) + SPACE) * charheight;
      else // not proprtional
         x_offset += charwidth;
   }

   return 1;
}

/*****************************************************************************/
/*
   Gerb_WriteChar
   flag mirror
   int oblique; // angle to slant - deg - 0 < oblique < 90]
*/
void Gerb_WriteChar(CPolyList *polylist, double insert_x, double insert_y,
      double rotation, double scale, bool mirrorFlag, int oblique, int dcode)
{
   Mat2x2   m;
   RotMat2(&m, rotation);

   int   first;
   CPoly *poly;
   CPnt  *pnt;
   Point2   p, firstp;

   // loop thru polys
   for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
   {
      poly = polylist->GetNext(polyPos);
      first = TRUE;

      for (POSITION pntPos = poly->getPntList().GetHeadPosition();pntPos != NULL;)
      {
         pnt = poly->getPntList().GetNext(pntPos);
         p.x = pnt->x * scale;

         if (mirrorFlag) p.x = -p.x;

         p.y = pnt->y * scale;
         p.bulge = pnt->bulge;
         TransPoint2(&p, 1, &m, insert_x, insert_y);

         // here deal with bulge
         if (first)
         {
            wgerb_Graph_Polyline(p, dcode, T_ROUND, 1);
            first = FALSE;
            firstp = p;
         }
         else
            wgerb_Graph_Vertex(p);
      }
   }

   return;
} // end DrawChar

//_____________________________________________________________________________
CStencilThicknessMap::CStencilThicknessMap(CCEtoODBDoc &camCadDoc, bool thicknessflag)
: m_camCadDoc(camCadDoc)
, m_useStencilThickness(thicknessflag)
, m_GerberLayerType(NonStencilLayer)
, m_targetThickness("")
{
}

CStencilThicknessMap::~CStencilThicknessMap()
{
   for(POSITION pos = GetStartPosition(); pos != NULL;)
   {
       CStringArray *layerlist = NULL;
       int layerIndex; 
       GetNextAssoc(pos,layerIndex,layerlist);
       if(layerlist)
          delete layerlist;
   }
   RemoveAll();
}


CString CStencilThicknessMap::getThicknessAttribute(DataStruct *data)
{   
   CString thickness("");
   if(data && data->getDataType() == dataTypeInsert && data->getAttributes())
   {
      Attrib* attrib = data->getAttributes()->lookupAttribute(standardAttributeStencilThickness);
      if(attrib)
         thickness.Format("%0.5f",attrib->getDoubleValue());
   }

   return thickness;
}

bool CStencilThicknessMap::IsStencilThicknessLayer(int layerType)
{
   if(layerType == layerTypePasteTop  ||
      layerType == layerTypePasteBottom ||
      layerType == layerTypeStencilTop  ||
      layerType == layerTypeStencilBottom ||
      layerType == layerTypePasteAll       )
      return true;

   return false;
}
int CStencilThicknessMap::hasThicknessLayer(FileLayerStruct* fileLayer)
{
   if(!fileLayer || !m_useStencilThickness)
      return 0;

   int layercount = 0;
   for(POSITION filepos = fileLayer->LayerList.GetHeadPosition(); filepos;)
   {
      CStringArray *layerlist = NULL;
      LayerStruct *layer = fileLayer->LayerList.GetNext(filepos);
      if(layer && Lookup(layer->getLayerIndex(),layerlist))
      {
         //layers contain data thickness attributes
         if(layerlist && layerlist->GetCount())
            layercount ++;
      }
   }

   return layercount;
}

bool CStencilThicknessMap::LookupThickness(CStringArray *layerlist,CString thickness)
{
   if(!layerlist)
      return false;

   for(int i = 0; i < layerlist->GetCount(); i++)
      if(!thickness.CompareNoCase(layerlist->GetAt(i)))
         return true;

   return false;
}

CStringArray* CStencilThicknessMap::AddThicknessLayer(DataStruct *data)
{
   if (!data)
      return NULL;

   LayerStruct* layer = m_camCadDoc.getLayerAt(data->getLayerIndex());   
   CString thickness = getThicknessAttribute(data);
   
   CStringArray *thicknesslayer = NULL;   
   if (layer && !thickness.IsEmpty())
      thicknesslayer = AddThicknessLayer(layer->getLayerIndex(),thickness);

   return thicknesslayer;
}

CStringArray* CStencilThicknessMap::AddThicknessLayer(int layerIndex, CString thickness)
{
   CStringArray *layerlist = NULL;
   if(!Lookup(layerIndex,layerlist) || !layerlist)
   {
      layerlist = new CStringArray;
      SetAt(layerIndex,layerlist);
   }
  
   if(!LookupThickness(layerlist,thickness))      
      layerlist->Add(thickness);
   
   return layerlist;
}

bool CStencilThicknessMap::VerifyThicknessData(DataStruct *data)
{
   //Don't care Thickness
   if(!m_useStencilThickness || m_GerberLayerType == NonStencilLayer)
      return true;

   // DR 741085
   // Still here, so we are using stencil thickness. If currently looking
   // for any thickness, we can just return true now.
   if (m_GerberLayerType == StencilLayer_AnyThickness)
      return true;
   
   // Still here, then we are looking for layer specifically with no
   // thickness or non-zero thickness. See which it is.
   CString attribValue = getThicknessAttribute(data);
   if(m_GerberLayerType == StencilLayer_NoThickness)
      return attribValue.IsEmpty();
   else if(m_GerberLayerType == StencilLayer_HasThickness) 
      return (!attribValue.CompareNoCase(m_targetThickness));

   return false;   
}