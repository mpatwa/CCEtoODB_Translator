
#include "stdafx.h"
#include "CCEtoODB.h"
#include "graph.h"
#include "pcbutil.h"
#include "PolyLib.h"
#include "Polygon.h"
#include "Vector2d.h"
#include "Drc_util.h"
#include "rgn.h"
#include "Apertur2.h"
#include "RealPart.h"
#include "lyrmanip.h"
#include "DFT.h"
#include "Gauge.h"
#include "CamCadDatabase.h"
#include "AccessAnalysis.h"
#include "DcaBbLib.h"
#include "outln_rb.h"
//-----------------------------------------------------------------------------
// Function Prototype
//-----------------------------------------------------------------------------
static CString GenerateSortKey(CString name);
static DataStruct* extraLayerFromPadstack(CCEtoODBDoc& doc, const BlockStruct& padstack, const CTMatrix transformMatrix, const LayerTypeTag extraLayerType,
                const int insertLayerIndex, const int mirrorFlag);
static void extraLayer(CCEtoODBDoc& doc, CDataList& extractedDataList, const BlockStruct& padstack, const CTMatrix transformMatrix, const LayerTypeTag extraLayerType,
                const int insertLayerIndex, const int mirrorFlag);
static bool isLayerType(const LayerTypeTag expectedLayerType, const int mirrorFlag, LayerStruct& layer);
static DataStruct* getCombinedRegion(CCEtoODBDoc& doc, DataStruct& data1, DataStruct& data2);
static ERPStruct* getERPStruct(CString refname, CPolyList* polyList, CCEtoODBDoc& doc);
static bool creatBoardOutlineDistance(CCEtoODBDoc& doc, FileStruct& pcbFile, CSMList& boardOutlineList, const double distance, bool isBottom);
static bool creatComponentOutlineDistance(CCEtoODBDoc& doc, FileStruct& pcbFile, CString incomingRefname, CSMList& boardOutlineList, const double distance, bool isBottom);
static void extraSoldermask(CCEtoODBDoc& doc, const BlockStruct& padstack, CSMList& soldermaskList, const CTMatrix transformMatrix, const LayerTypeTag extraLayerType,
                            const int insertLayerIndex, const int mirrorFlag );
static int TryToConnect(BOOL Head, SelectStruct *s, double variance, CCEtoODBDoc *doc);
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance);
static int CheckForBadVariance(CPoly *poly1, CPoly* poly2, double variance);
static int getMetalExposedRegion(CCEtoODBDoc& doc, DataStruct& copperData, CSMList& smList);
static int getDiffMetalExposedRegion(CCEtoODBDoc& doc, DataStruct& copperData, CSMList& smList, CStringArray& violatingRefnames);
extern CPoly *GetSegmentOutline(double ax, double ay, double bx, double by, double bulge, double widthRadius, int widthIndex);
CPntList *Outline_GetOutline(CPolyList* partList, int *returnCode);
//-----------------------------------------------------------------------------
static double getNumberOfPixels()
{
   return 50.0;
}

static CString GenerateSortKey(CString name)
{
   // create sort name
   CString temp, buf;
   bool alpha = true, lastAlpha = true;

   name.MakeUpper();
   for (int i=0; i<name.GetLength(); i++)
   {
      if (isalpha(name[i]))
         alpha = true;
      else if (isdigit(name[i]))
         alpha = false;
      else
         continue;

      if (i == 0)
      {
         lastAlpha = alpha;
         temp = name[i];
         continue;
      }

      if (lastAlpha != alpha)
      {
         if (lastAlpha)
         {
            // add string
            int len = temp.GetLength();
            temp = temp + (CString)"AAAAAAAAAAAAAAA";
            temp.Delete(15, len);
            buf += temp; 
         }
         else
         {
            // add number
            temp = (CString)"000000000000000" + temp;
            temp.Delete(0, temp.GetLength() - 15);
            buf += temp;
         }

         temp.Empty();
      }
      temp += name[i];
      lastAlpha = alpha;
   }

   if (lastAlpha)
   {
      // add string
      int len = temp.GetLength();
      temp = temp + (CString)"AAAAAAAAAAAAAAA";
      temp.Delete(15, len);
      buf += temp;
   }
   else
   {
      // add number
      temp = (CString)"000000000000000" + temp;
      temp.Delete(0, temp.GetLength() - 15);
      buf += temp;
   }

   return buf;
}

static DataStruct* extraLayerFromPadstack(CCEtoODBDoc& doc, const BlockStruct& padstack, const CTMatrix transformMatrix, const LayerTypeTag extraLayerType,
                const int insertLayerIndex, const int mirrorFlag)
{
   CCamCadData& camCadData = doc.getCamCadData();

   DataStruct* extractedData = NULL;
   CDataList extractedDataList(true);
   extraLayer(doc, extractedDataList, padstack, transformMatrix, extraLayerType, insertLayerIndex, mirrorFlag);

   // Need to combine all data in list into one single electrical data
   for (CDataListIterator dataListIterator(extractedDataList, dataTypePoly); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      if (extractedData != NULL)
      {
         DataStruct* combinedData = getCombinedRegion(doc, *extractedData, *data);

         delete extractedData;

         extractedData = combinedData;
			//extractedData->transform(transformMatrix)
      }
      else
      {
         extractedData = camCadData.getNewDataStruct(*data);
      }
   }

   return extractedData;
}

static void extraLayer(CCEtoODBDoc& doc, CDataList& extractedDataList, const BlockStruct& padstack, const CTMatrix transformMatrix, const LayerTypeTag extraLayerType,
                const int insertLayerIndex, const int mirrorFlag)
{
   CCamCadData& camCadData = doc.getCamCadData();

   for (CDataListIterator dataListIterator(padstack); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      
      LayerStruct* layer = NULL;
      if (doc.IsFloatingLayer(data->getLayerIndex()) && insertLayerIndex != -1)
         layer = doc.getLayerAt(insertLayerIndex);
      else
         layer = doc.getLayerAt(data->getLayerIndex());

      if (data->getDataType() == dataTypePoly && layer != NULL)
      {
         if (mirrorFlag & MIRROR_LAYERS)
            layer = doc.getLayerAt(layer->getMirroredLayerIndex());

         if (layer != NULL && isLayerType(extraLayerType, mirrorFlag, *layer))
         {
            DataStruct *newData = camCadData.getNewDataStruct(*data);

            bool addNewData = true; // We may or may not build an okay newData to keep.

            // Check to see if exposed data is a poly with non-zero width
            if (newData->getPolyList()->HasNonZeroWidthPoly(doc.getCamCadData()))
            {
               addNewData = false; // The following may not work, make sure it does before adding the newData.

               // This is done for case dts0100453282.  Not many files have padstack with apertures like this.
               // Therefore, this is a quick and dirty way of converting a non-zero with poly to a zero width closed poly.
               // 1st - Convert non-zero width poly to region
               // 2nd - Retrieve the poly from the region
               // 3rd - Close the poly to get a zero width closed poly

               float scaleFactor = (float)(1000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));

               Region* region = RegionFromPolylist(&doc, newData->getPolyList(), scaleFactor);

               if (region->getArea() > 0.0)
               {
                  // This gets null for a number of condition, mostly having to do with empty or zero area conditions.
                  CPolyList *newPolylist = region->GetPolys(scaleFactor);

                  if (newPolylist != NULL)
                  {
                     addNewData = true;
                     FreePolyList(newData->getPolyList());
                     newData->getPolyList() = newPolylist;

                     POSITION pos = newData->getPolyList()->GetHeadPosition();
                     while (pos)
                     {
                        CPoly *poly = newData->getPolyList()->GetNext(pos);
                        if (poly)
                        {
                           poly->setWidthIndex(doc.getZeroWidthIndex());
                           poly->setFilled(poly->isClosed());
                        }
                     }
                  }
               }
            }

            if (addNewData)
            {
               newData->transform(transformMatrix);
               extractedDataList.AddTail(newData);
            }
            else
            {
               // Something wrong with shape, probably zero area, skip it.
               delete newData;
            }
         }
      }
      else if (data->getDataType() == dataTypeInsert  && layer != NULL)
      {
         int blockMirrorFlag = mirrorFlag ^ data->getInsert()->getMirrorFlags();
         CTMatrix dataTMatrix = data->getInsert()->getTMatrix() * transformMatrix;
         BlockStruct *subBlock = doc.getBlockAt(data->getInsert()->getBlockNumber());

         // if aperture
         if ( (subBlock->getFlags() & BL_APERTURE) || (subBlock->getFlags() & BL_BLOCK_APERTURE) )
         {
				if (subBlock->getShape() == apertureComplex)
				{
					BlockStruct *complexBlock =  doc.getBlockAt(subBlock->getComplexApertureSubBlockNumber());
               if (complexBlock != NULL)
               {
                  CTMatrix complexTMatrix = complexBlock->getApertureTMatrix() * subBlock->getApertureTMatrix() * dataTMatrix;
                  extraLayer(doc, extractedDataList, *complexBlock, complexTMatrix, extraLayerType, layer->getLayerIndex(), blockMirrorFlag);
               }
            }
            else
            {
               if (mirrorFlag & MIRROR_LAYERS)
                  layer = doc.getLayerAt(layer->getMirroredLayerIndex());

               if (layer != NULL && isLayerType(extraLayerType, mirrorFlag, *layer))
               {
                  DataStruct* newData = doc.getCamCadData().getNewDataStruct(dataTypePoly);
						FreePolyList(newData->getPolyList());
						newData->getPolyList() = ApertureToPolyTranspose(subBlock, data->getInsert(), 0.0, 0.0, 0.0, 1, 0);
                  
                  if (newData->getPolyList() != NULL)
                  {
                     newData->transform(transformMatrix);
                     extractedDataList.AddTail(newData);
                  }
                  else
                  {
                     delete newData;
                     newData = NULL;
                  }
               }
            }
         }
         else
         {
            extraLayer(doc, extractedDataList, *subBlock, dataTMatrix, extraLayerType, layer->getLayerIndex(), blockMirrorFlag);
         }
      }
   }
}

static bool isLayerType(const LayerTypeTag expectedLayerType, const int mirrorFlag, LayerStruct& layer)
{   
   bool retval = false;
   if (expectedLayerType == layer.getLayerType())
   {
      retval = true;
   }
   else
   {
      switch (expectedLayerType)
      {
      case layerTypeMaskTop:
         retval = (layer.getLayerType() == layerTypeMaskAll);
         break;
      case layerTypeMaskBottom:
         retval = (layer.getLayerType() == layerTypeMaskAll); 
         break;
      case layerTypePadTop: // in this function layerTypePadTop represent all electrical top layers
         retval = layer.isElectricalTop();
         break;
      case layerTypePadBottom: // in this function layerTypePadBottom represent all electrical bottom layers
         retval = layer.isElectricalBottom();
         break;
      }
   }

   // Even if the layer type match, still need to check for layer mirror flag
	if ((mirrorFlag && layer.getNeverMirror()) || (!mirrorFlag && layer.getMirrorOnly()))
      retval = false;

   return retval;
}

static DataStruct* getCombinedRegion(CCEtoODBDoc& doc, DataStruct& data1, DataStruct& data2)
{
	DataStruct* combinedData = NULL;
	CPolyList* polyList1 = data1.getPolyList();
	CPolyList* polyList2 = data2.getPolyList();

	if (polyList1 == NULL && polyList2 == NULL)
	{
		combinedData = NULL;
	}
	else if (polyList1 == NULL)
	{		
		combinedData = doc.getCamCadData().getNewDataStruct(data2, true);
	}
	else if (polyList2 == NULL)
	{
		combinedData = doc.getCamCadData().getNewDataStruct(data1, true);
	}
	else
	{
		bool areCircles = false;
		if (polyList1->GetCount() == 1 && polyList2->GetCount() == 1)
		{
			double cx1 = 0.0;
			double cy1 = 0.0;
			double radius1 = 0.0;

			double cx2 = 0.0;
			double cy2 = 0.0;
			double radius2 = 0.0;

			if (PolyIsCircle(polyList1->GetHead(), &cx1, &cy1, &radius1) &&
				PolyIsCircle(polyList2->GetHead(), &cx2, &cy2, &radius2))
			{
				double dx = cx1 - cx2;
				double dy = cy1 - cy2;
				double distance = sqrt(dx*dx + dy*dy);

				if (radius2 >= distance + radius1)
				{
					// data2 completely covers data1
					combinedData = doc.getCamCadData().getNewDataStruct(data2, true);
					areCircles = true;
				}
				else if (radius1 >= distance + radius2)
				{
					// data1 completely covers data2
					combinedData = doc.getCamCadData().getNewDataStruct(data1, true);
					areCircles = true;
				}
			}
			else if(polyList1->GetHead()->contains(*polyList2->GetHead(),doc.getSettings().getPageUnits()))
			{
				combinedData = doc.getCamCadData().getNewDataStruct(data1, true);
				areCircles = true;
			}
			else if(polyList2->GetHead()->contains(*polyList1->GetHead(),doc.getSettings().getPageUnits()))
			{
				combinedData = doc.getCamCadData().getNewDataStruct(data2, true);
				areCircles = true;
			}
		}

		if (!areCircles)
		{
			// Either one or both of the polylist are not circle
			// For case #1909 changed scaleFactor to multiple by 100000.0 instead of 1000.00 to increase accuracy
			float scaleFactor = (float)(100000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));

         double maxExtent1 = polyList1->getExtent().getMaxSize(); 
         double maxExtent2 = polyList2->getExtent().getMaxSize();
         double maxExtent = max(maxExtent1, maxExtent2);
         if (maxExtent <= 0.) maxExtent = 1.;

         float pixelsPerPageUnit = (float)(getNumberOfPixels()/maxExtent);

			Region* region1 = RegionFromPolylist(&doc, polyList1, pixelsPerPageUnit);
			Region* region2 = RegionFromPolylist(&doc, polyList2, pixelsPerPageUnit);

			if (region1 == NULL && region2 == NULL)
			{
				combinedData = NULL;
			}
			else if (region1 == NULL)
			{
				combinedData = doc.getCamCadData().getNewDataStruct(data2, true);
			}
			else if (region2 == NULL)
			{
				combinedData = doc.getCamCadData().getNewDataStruct(data1, true);
			}
			else
			{
				region1->CombineWith(region2);

				CPolyList *newPolylist = region1->GetPolys(pixelsPerPageUnit);

            //CRegionPolygon regionPolygon(doc,*region1,pixelsPerPageUnit);
            //CPolyList* newPolylist = regionPolygon.getSmoothedPolys();

				if (newPolylist)
				{
				   combinedData = doc.getCamCadData().getNewDataStruct(dataTypePoly);
				   FreePolyList(combinedData->getPolyList());
					combinedData->getPolyList() = newPolylist;

					POSITION pos = combinedData->getPolyList()->GetHeadPosition();
					while (pos)
					{
						CPoly *poly = combinedData->getPolyList()->GetNext(pos);
						if (poly)
						{
							poly->setWidthIndex(doc.getZeroWidthIndex());
							poly->setFilled(poly->isClosed());
						}
					}
            } 
			}

         delete region1;
         delete region2;

         region1 = NULL;
         region2 = NULL;
		}
	}

	return combinedData;
}

static ERPStruct* getERPStruct(CString refname, CPolyList* polyList, CCEtoODBDoc& doc)
{
	float scaleFactor = (float)(1000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));

   ERPStruct* erpStruct = NULL;
   if (polyList != NULL)
   {
      double maxExtent = polyList->getExtent().getMaxSize(); 
      if (maxExtent <= 0.) maxExtent = 1.;

      float pixelsPerPageUnit = (float)(getNumberOfPixels()/maxExtent);

      erpStruct = new ERPStruct;
      erpStruct->refname = refname;
      erpStruct->pPolyList = polyList;
      erpStruct->extents = polyList->getExtent();
      erpStruct->region = RegionFromPolylist(&doc, polyList, scaleFactor);
   }

   return erpStruct;
}

static bool creatBoardOutlineDistance(CCEtoODBDoc& doc, FileStruct& pcbFile, CSMList& boardOutlineList, const double distance, bool isBottom)
{
   bool retval = false;
   
   BlockStruct* block = pcbFile.getBlock();
   if (block != NULL)
   {
      for (POSITION pos = block->getHeadDataPosition(); pos != NULL;)
      {
         // Do not do a get next here because the POS can be change in the section of the code that 
         // does the joining of polylist.  Get next at the end of loop
         DataStruct *data = block->getAtData(pos);
		   if (data != NULL)
         {
            LayerStruct* layer = doc.getLayerAt(data->getLayerIndex());
            if (layer != NULL && pcbFile.isMirrored())
               layer = doc.getLayerAt(layer->getMirroredLayerIndex());

            if (layer != NULL && data->getGraphicClass() == graphicClassBoardOutline)
            {
				   CPoly* tmpPoly = (CPoly*)data->getPolyList()->GetHead();
				   if (tmpPoly->isClosed() == false)
				   {
					   SelectStruct *s = new SelectStruct;
					   s->setData(data);
					   s->filenum = pcbFile.getFileNumber();
					   s->insert_x = pcbFile.getInsertX();
					   s->insert_y = pcbFile.getInsertY();
					   s->scale = pcbFile.getScale();
					   s->rotation = pcbFile.getRotation();
					   s->mirror = pcbFile.isMirrored();
					   s->layer = -1;
					   s->setParentDataList(&pcbFile.getBlock()->getDataList());

					   double variance = doc.getSettings().JOIN_autoSnapSize;
					   if (variance < SMALLNUMBER)
					   {
						   int decimals = GetDecimals(doc.getSettings().getPageUnits());
						   variance = 1;
						   for (int i=0; i<decimals; i++)
							   variance *= 0.1;
					   }

					   BOOL Head = TRUE;
					   while (TRUE)
					   {
						   int result = TryToConnect(Head, s, variance, &doc);

						   if (!result) // couldn't connect anymore to this end
						   {
							   if (Head)
								   Head = FALSE;
							   else
								   break;
						   }

						   if (result == 2) // closed
							   break;
					   }
				   }

               CPolyList* boardOutlinePolyList = new CPolyList(*data->getPolyList());
               if (boardOutlinePolyList != NULL)
               {
					   if (boardOutlinePolyList->GetCount() > 1)
					   {
						   ErrorMessage("More than one poly found in Board outline during CreateBoardOutlineDistant, no board outline distance check is created.", "Error");
						   retval = false;
					   }
					   else
					   {
						   CPoly *poly = boardOutlinePolyList->GetHead();
						   if (poly)
						   {
                        retval = poly->shrink(distance,doc.getPageUnits());

							   if (retval == true)
							   {
								   poly->setFilled(FALSE);
								   poly->setClosed(true);

                           boardOutlineList.AddTail(getERPStruct("Board Outline", boardOutlinePolyList, doc));
                        }
							   else
							   {
                           int decimals = GetDecimals(doc.getPageUnits());
								   CString errorMessage;
								   if (isBottom)
									   errorMessage.Format("The value (%0.*f) of bottom Board Outline Distance check is too large.  Enter a smaller value.", decimals, distance);
								   else
									   errorMessage.Format("The value (%0.*f) of top Board Outline Distance check is too large.  Enter a smaller value.", decimals, distance);

								   ErrorMessage(errorMessage, "Error");
							   }
						   }
					   }
               }             

               if (retval == false)
               {
                  delete boardOutlinePolyList;
                  boardOutlinePolyList = NULL;
               }
            }
         }

         
         // Now move to next position
         block->getNextData(pos);
      }
   }

   return retval;
}

static bool creatComponentOutlineDistance(CCEtoODBDoc& doc, BlockStruct& block, CString incomingRefname, CSMList& componentOutlineList, const CTMatrix transformMatrix,
                                          const int insertLayerIndex, const int extractLayerIndex, const GraphicClassTag graphicClass,
                                          const bool ignore1PinCompOutline, const bool ignoreUnloadedCompOutline, const int mirrorFlag,
                                          const double distance, const double heightAnalysisDistance, CHeightAnalysis& heightAnalysis)
{
   bool retval = false;
	float scaleFactor = (float)(1000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));
   
   for (CDataListIterator dataListIterator(block); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();

      LayerStruct* layer = NULL;
      if (doc.IsFloatingLayer(data->getLayerIndex()) && insertLayerIndex != -1)
         layer = doc.getLayerAt(insertLayerIndex);
      else
         layer = doc.getLayerAt(data->getLayerIndex());

      if (data->getDataType() == dataTypePoly && data->getGraphicClass() == graphicClass && layer != NULL)
      {
         if (mirrorFlag & MIRROR_LAYERS)
            layer = doc.getLayerAt(layer->getMirroredLayerIndex());

         if (layer != NULL && layer->getLayerIndex() == extractLayerIndex)
         {
            CPolyList* componentOutlinePolyList = new CPolyList(*data->getPolyList());
            if (componentOutlinePolyList != NULL)
            {
				   if (componentOutlinePolyList->GetCount() > 1)
				   {
						ErrorMessage("More than one poly found in Package outline during CreateComponentOutlineDistant", "Error");
					   retval = false;
				   }
				   else
				   {
                  componentOutlinePolyList->transform(transformMatrix);
					   CPoly *poly = componentOutlinePolyList->GetHead();
					   if (poly)
					   {
							if (heightAnalysisDistance > 0)
								poly->shrink(-heightAnalysisDistance,doc.getPageUnits());
							else
								poly->shrink(-distance,doc.getPageUnits());
							
                     poly->setFilled(FALSE);

                     componentOutlineList.AddTail(getERPStruct(incomingRefname, componentOutlinePolyList, doc));
                  }
               }
            }
         }
      }
		else if (data->getDataType() == dataTypeInsert && data->getInsert() != NULL)
		{
			InsertStruct* insert = data->getInsert();
         BlockStruct *subBlock = doc.getBlockAt(insert->getBlockNumber());

			if (subBlock == NULL)
				continue;

			if (subBlock->getBlockType() != blockTypePcbComponent && 
				 subBlock->getBlockType() != blockTypeRealPart && subBlock->getBlockType() != blockTypePackage)
				continue;

			// Check to see if inside the PCB block or Package block that is single pin 
			if (subBlock->getBlockType() == blockTypePcbComponent || subBlock->getBlockType() == blockTypePackage)
			{
				if (ignore1PinCompOutline == true && subBlock->getPinCount() == 1)
				{
					// Check to see it is a single pin component, if so then ignore it
					continue;
				}
			}

         // Only want to update refname at top level of insert recursion
         CString refname = incomingRefname;
         if (refname.IsEmpty())
            refname = insert->getRefname();

			// If ignoring unloaded components then skip item if related component is not loaded.
         // For PCB Component check the LOADED attrib.
         // For Real Part, if related component is not loaded then Real Part will be set to hidden.
         if (ignoreUnloadedCompOutline == true)
         {
            if (subBlock->getBlockType() == blockTypePcbComponent)
            {
               WORD loadedKey = doc.RegisterKeyWord(ATT_LOADED, 0, VT_STRING);
               Attrib *attrib = NULL;
               if (data->getAttributes() && data->getAttributes()->Lookup(loadedKey, attrib))
               {
                  if (!attrib->getStringValue().CompareNoCase("FALSE"))
                     continue;
               }
            }
            else if (subBlock->getBlockType() == blockTypeRealPart && data->isHidden())
            {
               continue;
            }
         }

         // Get the proper height analysis value 
			double tmpHeightAnalysisDistance = -1;	
			if (insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypeRealPart)
			{
            if (heightAnalysis.IsUse())
            {
				   WORD compHeightKw = doc.RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
				   Attrib* attrib = NULL;
				   if (data->getAttributes() && data->getAttributes()->Lookup(compHeightKw, attrib))
				   {
					   double compHeight = attrib->getDoubleValue();
					   tmpHeightAnalysisDistance = heightAnalysis.GetPackageOutlineDistance(compHeight);
				   }
            }
			}
         else if (insert->getInsertType() == insertTypePackage)
			{
				tmpHeightAnalysisDistance = heightAnalysisDistance;
			}
          
         // insert if mirror is either global mirror or block_mirror, but not if both.
         int blockMirrorFlag = mirrorFlag ^ data->getInsert()->getMirrorFlags();
         CTMatrix dataTMatrix = data->getInsert()->getTMatrix() * transformMatrix;

         retval = creatComponentOutlineDistance(doc, *subBlock, refname, componentOutlineList, dataTMatrix, layer!=NULL?layer->getLayerIndex():-1,
               extractLayerIndex, graphicClass, ignore1PinCompOutline, ignoreUnloadedCompOutline, blockMirrorFlag,
               distance, tmpHeightAnalysisDistance, heightAnalysis);
      }
   }

   return retval;
}

static void extraSoldermask(CCEtoODBDoc& doc, const BlockStruct& padstack, CSMList& soldermaskList, const CTMatrix transformMatrix, const LayerTypeTag extraLayerType,
                            const int insertLayerIndex, const int mirrorFlag)
{
	float scaleFactor = (float)(1000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));

   for (CDataListIterator dataListIterator(padstack); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      
      LayerStruct* layer = NULL;
      if (doc.IsFloatingLayer(data->getLayerIndex()) && insertLayerIndex != -1)
         layer = doc.getLayerAt(insertLayerIndex);
      else
         layer = doc.getLayerAt(data->getLayerIndex());

      if (data->getDataType() == dataTypePoly && layer != NULL)
      {
         if (mirrorFlag & MIRROR_LAYERS)
            layer = doc.getLayerAt(layer->getMirroredLayerIndex());

         if (layer != NULL && isLayerType(extraLayerType, mirrorFlag, *layer))
         {
            CPolyList* soldermaskPolyList = new CPolyList(*data->getPolyList());
            if (soldermaskPolyList != NULL && soldermaskPolyList->GetHead()->isFilled())
            {
               soldermaskPolyList->transform(transformMatrix);
               soldermaskList.AddTail(getERPStruct("Solder Mask", soldermaskPolyList, doc));
            }
         }
      }
      else if (data->getDataType() == dataTypeInsert  && layer != NULL)
      {
         InsertTypeTag insertType = data->getInsert()->getInsertType();
         if (insertType != insertTypePcbComponent && insertType != insertTypeTestPoint &&
             insertType != insertTypeVia && insertType != insertTypeTestPad)
         {
            int blockMirrorFlag = mirrorFlag ^ data->getInsert()->getMirrorFlags();
            CTMatrix dataTMatrix = data->getInsert()->getTMatrix() * transformMatrix;
            BlockStruct *subBlock = doc.getBlockAt(data->getInsert()->getBlockNumber());

            // if aperture
            if ( (subBlock->getFlags() & BL_APERTURE) || (subBlock->getFlags() & BL_BLOCK_APERTURE) )
            {
				   if (subBlock->getShape() == apertureComplex)
				   {
					   BlockStruct *complexBlock =  doc.getBlockAt(subBlock->getComplexApertureSubBlockNumber());
                  if (complexBlock != NULL)
                  {
                     CTMatrix complexTMatrix = complexBlock->getApertureTMatrix() * subBlock->getApertureTMatrix() * dataTMatrix;
                     extraSoldermask(doc, *complexBlock, soldermaskList, complexTMatrix, extraLayerType, layer->getLayerIndex(), blockMirrorFlag);
                  }
               }
               else
               {
                  if (mirrorFlag & MIRROR_LAYERS)
                     layer = doc.getLayerAt(layer->getMirroredLayerIndex());

                  if (layer != NULL && isLayerType(extraLayerType, mirrorFlag, *layer))
                  {
                     CPolyList* soldermaskPolyList = ApertureToPolyTranspose(subBlock, data->getInsert(), 0.0, 0.0, 0.0, 1, 0);
                     if (soldermaskPolyList != NULL && soldermaskPolyList->GetHead()->isFilled())
                     {
                        soldermaskPolyList->transform(transformMatrix);
                        soldermaskList.AddTail(getERPStruct("Solder Mask", soldermaskPolyList, doc));
                     }
                  }
               }
            }      
         }
      }
   }
}

static int TryToConnect(BOOL Head, SelectStruct *s, double variance, CCEtoODBDoc *doc)
{
   SelectStruct temp(*s);
   //memcpy(&temp, s, sizeof(SelectStruct));

   CPoly *poly = s->getData()->getPolyList()->GetHead();
   CPnt *pnt;

   if (Head)
      pnt = poly->getPntList().GetHead();
   else 
      pnt = poly->getPntList().GetTail();

   DataStruct *connectData;
   CPoly *connectPoly = NULL;
   BOOL connectHead;

   POSITION dataPos = s->getParentDataList()->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = s->getParentDataList()->GetNext(dataPos);

      if (data->getDataType() != T_POLY || data->getLayerIndex() != s->getData()->getLayerIndex())
         continue;

      if (data == s->getData())
         continue;

      CPoly *testPoly = data->getPolyList()->GetHead();

      if (poly->getWidthIndex() != testPoly->getWidthIndex())
         continue;

      CPnt *testPnt = testPoly->getPntList().GetHead();
      if (PntsOverlap(pnt, testPnt, variance))
      {
         if (connectPoly)
         {
            int decimals = GetDecimals(doc->getSettings().getPageUnits());
            if (CheckForBadVariance(connectPoly, testPoly, variance))
            {
               CString  tmp;
               tmp.Format("It appears that the Auto Join Snap Size %1.*lf is too large.\nGo to System Settings to change",
                  decimals, variance);
               ErrorMessage(tmp, "Warning");
            }

            CString buf;
            buf.Format("Reached a fork at (%+.*lf, %+.*lf).\nUse manual join functions to continue.\n(If there doesn't seem to be a fork, there may be overlapping lines.)", 
                  decimals, pnt->x, decimals, pnt->y);
            ErrorMessage(buf, "Reached a Decision");
            return 0; // decision
         }
         else
         {
            temp.setData(data);
            connectPoly = testPoly;
            connectHead = TRUE;
            connectData = data;
         }
      }
      else
      {
         testPnt = testPoly->getPntList().GetTail();
         if (PntsOverlap(pnt, testPnt, variance))
         {
            if (connectPoly)
            {
               int decimals = GetDecimals(doc->getSettings().getPageUnits());
               if (CheckForBadVariance(connectPoly, testPoly, variance))
               {
                  CString  tmp;
                  tmp.Format("It appears that the Auto Join Snap Size %1.*lf is too large.\nGo to System Settings to change",
                     decimals, variance);
                  ErrorMessage(tmp, "Warning");
               }
               CString buf;
               buf.Format("Reached a fork at (%+.*lf, %+.*lf).\nUse manual join functions to continue.\n(If there doesn't seem to be a fork, there may be overlapping lines.)", 
                     decimals, pnt->x, decimals, pnt->y);
               ErrorMessage(buf, "Reached a Decision");
               return 0; // decision
            }
            else
            {
               temp.setData(data);
               connectPoly = testPoly;
               connectHead = FALSE;
               connectData = data;
            }
         }
      }
   }

   if (!connectPoly)
      return 0;

   if (Head)
   {
      if (connectHead)
         ReversePoly(&connectPoly->getPntList());

      POSITION pos = connectPoly->getPntList().GetTailPosition();
      if (pos)
         connectPoly->getPntList().GetPrev(pos);
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(connectPoly->getPntList().GetPrev(pos)));
         poly->getPntList().AddHead(newPnt);
      }
   }
   else
   {
      if (!connectHead)
         ReversePoly(&connectPoly->getPntList());

      poly->getPntList().GetTail()->bulge = connectPoly->getPntList().GetHead()->bulge;

      POSITION pos = connectPoly->getPntList().GetHeadPosition();
      if (pos)
         connectPoly->getPntList().GetNext(pos);
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(connectPoly->getPntList().GetNext(pos)));
         poly->getPntList().AddTail(newPnt);
      }
   }

	// Reomoved connected data
   dataPos = s->getParentDataList()->Find(connectData);
   s->getParentDataList()->RemoveAt(dataPos);
	delete connectData;
	connectData = NULL;

   // close poly?
   CPnt *headPnt = poly->getPntList().GetHead(), *tailPnt = poly->getPntList().GetTail();
   if (PntsOverlap(headPnt, tailPnt, variance))
   {
      tailPnt->x = headPnt->x;
      tailPnt->y = headPnt->y;
      poly->setClosed(true);
      doc->DrawEntity(s, 1, TRUE);
      return 2;
   }

   doc->DrawEntity(s, 1, TRUE);

   return 1;
}

static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance)
{ 
   return ((fabs(p1->x - p2->x) < variance) && (fabs(p1->y - p2->y) < variance)); 
}
static int CheckForBadVariance(CPoly *poly1, CPoly* poly2, double variance)
{
   CPnt *p1a, *p1b, *p2a, *p2b;
   p1a = poly1->getPntList().GetHead();
   p1b = poly1->getPntList().GetTail();
   p2a = poly2->getPntList().GetHead();
   p2b = poly2->getPntList().GetTail();
   if ((sqrt(pow((p1a->x - p1b->x), 2) + pow((p1a->y - p1b->y), 2)) < variance) ||
         (sqrt(pow((p2a->x - p2b->x), 2) + pow((p2a->y - p2b->y), 2)) < variance))
   {
      return 1;
   }
   return 0;
}

static int getMetalExposedRegion(CCEtoODBDoc& doc, DataStruct& copperData, CSMList& smList)
{
	int res = 0;
	Mat2x2 m;
   RotMat2(&m, 0);

   float scaleFactor = (float)(1000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));

	ERPStruct copper;
	copper.region = NULL;
	copper.pPolyList = NULL;
   PolyExtents(&doc, copperData.getPolyList(), &copper.extents, 1, 0, 0, 0, &m, 0);

	ERPStruct solderable;
	solderable.region = new Region();
	solderable.pPolyList = NULL;
	solderable.extents.reset();

	bool isCopperCircle = false;
	int combineRegCnt = 0;
	double cx = 0.0;
	double cy = 0.0;
	double radius = 0.0;

   POSITION smPos = smList.GetHeadPosition();
   while (smPos) // for each soldermask
   {
      ERPStruct *sm = smList.GetNext(smPos);
      if (sm == NULL || sm->pPolyList == NULL || sm->region == NULL)
         continue;

      if (sm->extents.left > copper.extents.right || copper.extents.left > sm->extents.right || 
          sm->extents.bottom > copper.extents.top || copper.extents.bottom > sm->extents.top)
         continue;  

		if (copper.region == NULL)
		{
			copper.pPolyList = copperData.getPolyList();

			if (copper.pPolyList->GetCount() == 1)
         {
				if (PolyIsCircle(copper.pPolyList->GetHead(), &cx, &cy, &radius))
            {
					isCopperCircle = true;

               //Make sure the circle is close because we did come across file that has circle that is not close
               CPoly* poly = copper.pPolyList->GetHead();
               if (poly != NULL)
               {
                  poly->setClosed(true);
               }
            }
         }

         // Create a region after checking if poly is a circle and closing the circle
         copper.region = RegionFromPolylist(&doc, copperData.getPolyList(), scaleFactor); // scaleFactor); // pixelsPerPageUnit);
		}

		// calculate region and intersect with all soldermask pieces and take the union of the intersections
		bool done = false;

		if (sm->pPolyList->GetCount() == 1 && isCopperCircle == true)
		{
			double smCx = 0.0;
			double smCy = 0.0;
			double smRadius = 0.0;
			CPoly *smPoly = sm->pPolyList->GetHead();
			if (PolyIsCircle(smPoly, &smCx, &smCy, &smRadius))
			{
				double dx = cx - smCx;
				double dy = cy - smCy;
				double distance = sqrt(dx*dx + dy*dy);

				if (distance >= radius + smRadius)
				{
					// the two circle is not intersecting
					done = true;
					continue;
				}
				else if (smRadius >= distance + radius)
				{
					// copper is completely inside soldermask, so exposed metal is size of copper
					delete solderable.region;
					solderable.region = copper.region->Clone();
					solderable.pPolyList = copper.pPolyList;
					done = true;
					break;
				}
				else if (radius >= distance + smRadius)
				{
					// soldermask is completly inside copper, so exposed metal is size of soldermask
					if (combineRegCnt == 0)
					{
						// Set solderable polylist to be soldermask polylist when the 1st round soldermask is found
						delete solderable.region;
						solderable.region = sm->region->Clone();
						solderable.pPolyList = sm->pPolyList;
						combineRegCnt++;
					}
					else
					{
						// More than one soldermask intersect the copper

						if (solderable.pPolyList != NULL)
						{
							// There is only one solderable piece of copper
							// so check to see if the current intersecting piece of copper
							// is cover than the previous piece, if it is then replace it
						
							CPoly *solderablePoly = solderable.pPolyList->GetHead();
							double tmpCx = solderablePoly->getExtent().getCenter().x;
							double tmpCy = solderablePoly->getExtent().getCenter().y;
							double tmpRadius = solderablePoly->getExtent().getXsize()/2;
							
							dx = tmpCx - smCx;
							dy = tmpCy - smCy;
							distance = sqrt(dx*dx + dy*dy);

							if (distance >= radius + smRadius)
							{
								// the two circle is not intersecting
								solderable.region->CombineWith(sm->region);
								combineRegCnt++;
								solderable.pPolyList = NULL;
							}
							else if (smRadius >= distance + tmpRadius)
							{
								// solderable soldermask is completely inside soldermask
								// so replace solderable soldermask
								delete solderable.region;
								solderable.region = sm->region->Clone();
								solderable.pPolyList = sm->pPolyList;
							}
						}
						else
						{
							// More than one soldermask intersect the copper so no solderable polylist
							solderable.region->CombineWith(sm->region);
							combineRegCnt++;
							solderable.pPolyList = NULL;
						}
					}
					done = true;
				}
			}
		}
		
		if (done == false)
		{
			Region *intersectRegion = IntersectRegions(copper.region, sm->region);

			if (intersectRegion)
			{
				solderable.region->CombineWith(intersectRegion);
				delete intersectRegion;

				if (AreRegionsEqual(solderable.region, copper.region)) // copper completely covered -> don't need to check anymore soldermask
					break;
			}
		}
   }

   if (!copper.region)
	{
		// if we didn't create it, it doesn't touch soldermask
      res = -1;
	}
   else
   {
		if (solderable.region->IsEmpty())
		{	
			// was within extents of some soldermask, but no common area with soldermask
			// if we didn't create it, it doesn't touch soldermask
         res = -1;							
		}
		else if (combineRegCnt == 1 && solderable.pPolyList != NULL)
		{
			if (copperData.getPolyList() != solderable.pPolyList)
			{
				FreePolyList(copperData.getPolyList());
				copperData.getPolyList() = new CPolyList(*solderable.pPolyList);
			}
		}
      else if (!AreRegionsEqual(solderable.region, copper.region))
		{
			// copper not completely covered -> replace copper entity with solderableRegion
			CPolyList *newPolylist = solderable.region->GetPolys(scaleFactor); // pixelsPerPageUnit); // scaleFactor);

         //CRegionPolygon regionPolygon(doc,*(solderable.region),scaleFactor); // pixelsPerPageUnit);
         //CPolyList *newPolylist = regionPolygon.getSmoothedPolys();

         if (newPolylist)
         {
            FreePolyList(copperData.getPolyList());
            copperData.getPolyList() = newPolylist;

				POSITION pos = copperData.getPolyList()->GetHeadPosition();
				while (pos)
				{
					CPoly *poly = copperData.getPolyList()->GetNext(pos);
					if (poly)
					{
						poly->setWidthIndex(doc.getZeroWidthIndex());
						poly->setFilled(poly->isClosed());
					}
				}
         }
         else
			{
				// no solderable region
            res = -1; 
			}
      }

		copper.pPolyList = NULL;
      delete copper.region; // only free if we created it
   }

	solderable.pPolyList = NULL;
	delete solderable.region;

   return res;
}

static int getDiffMetalExposedRegion(CCEtoODBDoc& doc, DataStruct& copperData, CSMList& smList, CStringArray& violatingRefnames)
{
	int res = 0;
	Mat2x2 m;
   RotMat2(&m, 0);

   violatingRefnames.RemoveAll();

   float scaleFactor = (float)(1000.0 * Units_Factor(doc.getSettings().getPageUnits(), UNIT_INCHES));
	
   ExtentRect copperExtents;
   PolyExtents(&doc, copperData.getPolyList(), &copperExtents, 1, 0, 0, 0, &m, 0);

   Region *copperRegion = NULL;
   Region *solderableRegion = NULL;

   POSITION smPos = smList.GetHeadPosition();
   while (smPos) // for each soldermask
   {
      ERPStruct *sm = smList.GetNext(smPos);

      if (sm->extents.left > copperExtents.right || copperExtents.left > sm->extents.right || 
          sm->extents.bottom > copperExtents.top || copperExtents.bottom > sm->extents.top)
         continue; 
		
      // calculate region and intersect with all soldermask pieces and take the union of the intersections
      if (!copperRegion) // only create the region once
		{
         copperRegion = RegionFromPolylist(&doc, copperData.getPolyList(), scaleFactor); // scaleFactor); //pixelsPerPageUnit);
			solderableRegion = RegionFromPolylist(&doc, copperData.getPolyList(), scaleFactor); // scaleFactor); //pixelsPerPageUnit);
		}

      Region *diffRegion = DiffRegions(solderableRegion, sm->region);
      if (diffRegion) // I'm assuming this means regions was clipped
      {
			delete solderableRegion;
			solderableRegion = diffRegion;
         violatingRefnames.Add(sm->refname);
      }
		else if (AreRegionsEqual(solderableRegion, sm->region))
		{
			// copper is complelely covered, no need to continue checking
			delete solderableRegion;
			delete copperRegion;
			res = -1;
         violatingRefnames.Add(sm->refname);
			return res;
		}
   }

	if (solderableRegion != NULL)
	{
		if (!solderableRegion->IsEmpty())
		{
			if (!AreRegionsEqual(solderableRegion, copperRegion))
			{
				// copper is partially covered -> replace copper entity with solderableRegion

				CPolyList *newPolylist = solderableRegion->GetPolys(scaleFactor); // scaleFactor);
            //CRegionPolygon regionPolygon(doc, *solderableRegion,scaleFactor); //pixelsPerPageUnit);
            //CPolyList *newPolylist = regionPolygon.getSmoothedPolys();

				if (newPolylist)
				{
					FreePolyList(copperData.getPolyList());
					copperData.getPolyList() = newPolylist;

					POSITION pos = copperData.getPolyList()->GetHeadPosition();
					while (pos)
					{
						CPoly *poly = copperData.getPolyList()->GetNext(pos);
						if (poly)
						{
							poly->setWidthIndex(doc.getZeroWidthIndex());
							poly->setFilled(poly->isClosed());
						}
					}
				}
				else
				{
					// no solderable region
					res = -1; 
				}
			}
		}
		else
		{
			res = -1;
		}
	}

	if (copperRegion)
      delete copperRegion; // only free if we created it

	if (solderableRegion)
		delete solderableRegion;

	return res;
}


//-----------------------------------------------------------------------------
// CHeightRange
//-----------------------------------------------------------------------------
CHeightAnalysis::CHeightAnalysis(PageUnitsTag pageUnits):CTypedMapStringToPtrContainer<CHeightRange*>()
, m_pageUnitsTag(pageUnits)
{
   SetDefault(pageUnits);
}

CHeightAnalysis::~CHeightAnalysis()
{
   this->empty();
}

CHeightAnalysis& CHeightAnalysis::operator=(const CHeightAnalysis& other)
{
   if (&other != this)
   {
      this->empty();

      m_bUse = other.m_bUse;
      m_bModified = other.m_bModified;

      POSITION pos = other.GetStartPosition();
      while (pos != NULL)
      {
         CHeightRange* newRange = new CHeightRange(*other.GetNext(pos));
         this->AddHeightRange(newRange);
      }
   }

   return *this;
}

 void CHeightAnalysis::SetDefault(PageUnitsTag pageUnits)
{
   m_bModified = false;
   m_bUse = false;

   // Max height is set to -1 or less than zero to indicate infinity
   // The default height range is from zero to infinity
   empty();
   AddHeightRange("H1", 0.010 * Units_Factor(DFT_DEFAULT_UNIT, pageUnits), 0, -1);
}

void CHeightAnalysis::Scale(double factor)
{
   POSITION pos = GetStartPosition();
   while (pos != NULL)
   {
      CHeightRange* heightRange = GetNext(pos);
      if (heightRange == NULL)
         continue;

      heightRange->SetOutlineDistance(heightRange->GetOutlineDistance() * factor);
      heightRange->SetMinHeight(heightRange->GetMinHeight() * factor);

      // If max height is not infinity then scale it
      if (heightRange->GetMaxHeight() > 0)
         heightRange->SetMaxHeight(heightRange->GetMaxHeight() * factor);
   }
}

void CHeightAnalysis::SetUse(bool isUse)
{ 
   m_bModified = m_bUse != isUse;
   m_bUse = isUse;            
}

void CHeightAnalysis::AddHeightRange(CHeightRange* heightRange)
{
   if (heightRange == NULL || heightRange->GetName().IsEmpty())
      return;

   if (FindHeightRange(heightRange->GetName()) != NULL)
      return;
   
   m_bModified = true;
   this->SetAt(heightRange->GetName().MakeUpper(), heightRange);
}

CHeightRange* CHeightAnalysis::AddHeightRange(CString name, CString outlineDistance, CString minHeight, CString maxHeight)
{
   double distance = atof(outlineDistance);
   double minH = atof(minHeight);
   double maxH = atof(maxHeight);

   return AddHeightRange(name, distance, minH, maxH);
}

CHeightRange* CHeightAnalysis::AddHeightRange(CString name, double outlineDistance, double minHeight, double maxHeight)
{
   if (name.IsEmpty())
      return NULL;
   if (outlineDistance < 0)
      return NULL;
   if (minHeight < 0)
      return NULL;
   if (maxHeight > 0 && (maxHeight < minHeight))
      return NULL;

   CHeightRange* heightRange = FindHeightRange(name);
   if (heightRange == NULL)
   {
      heightRange = new CHeightRange(name, outlineDistance, minHeight, maxHeight);
      this->SetAt(name.MakeUpper(), heightRange);
   }
   else
   {
      heightRange->SetOutlineDistance(outlineDistance);
      heightRange->SetMinHeight(minHeight);
      heightRange->SetMaxHeight(maxHeight);
   }

   m_bModified = true;
   return heightRange;
}

CHeightRange* CHeightAnalysis::FindHeightRange(CString name)
{
   CHeightRange* heightRange = NULL;
   this->Lookup(name.MakeUpper(), heightRange);
   return heightRange;
}

void CHeightAnalysis::DeleteHeightRange(CString name)
{
   CHeightRange* heightRange = NULL;
   this->Lookup(name.MakeUpper(), heightRange);
   if (heightRange != NULL)
   {
      this->RemoveKey(name);
      delete heightRange;
      heightRange = NULL;

      m_bModified = true;
   }
}

POSITION CHeightAnalysis::GetStartPosition() const
{
   return CTypedPtrMap<CMapStringToPtr, CString, CHeightRange*>::GetStartPosition();
}

CHeightRange* CHeightAnalysis::GetNext(POSITION& pos) const
{
   CHeightRange* heightRange = NULL;
   CString name;
   this->GetNextAssoc(pos, name, heightRange);
   return heightRange;
}

int CHeightAnalysis::GetCount() const
{
   return CTypedPtrMap<CMapStringToPtr, CString, CHeightRange*>::GetCount();
}

void CHeightAnalysis::RemoveAllRange()
{
   empty();
}

CString CHeightAnalysis::GetNewHeightName()
{
   CString newName;
   int count = GetCount();

   newName.Format("H%d", count++);
   while (FindHeightRange(newName) != NULL)
   {
      newName.Format("H%d", count++);
   }
   
   return newName;
}

double CHeightAnalysis::GetPackageOutlineDistance(const double compHeight)
{
   double packageOutlineDistance = -1;

   POSITION pos = GetStartPosition();
   while (pos != NULL)
   {
      CHeightRange* heightRange = GetNext(pos);
      if (heightRange == NULL)
         continue;



      if (compHeight >= heightRange->GetMinHeight() && (compHeight < heightRange->GetMaxHeight() || heightRange->GetMaxHeight() < 0))
         packageOutlineDistance = heightRange->GetOutlineDistance();
   }

   return packageOutlineDistance;
}

void CHeightAnalysis::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bUseHeightAnalysis = %s\n", indent, " ", m_bUse?"True":"False");

   int decimals = GetDecimals(m_pageUnitsTag);

   POSITION pos = GetStartPosition();
   while (pos != NULL)
   {
      CHeightRange* heightRange = GetNext(pos);
      if (heightRange == NULL)
         continue;

      file.WriteString("%*s::HeightRange %s::\n", indent, " ", heightRange->GetName());
      {
         indent += 3;
         file.WriteString("%*sm_dOutlineDistance = %0.*f\n", indent, " ", decimals, heightRange->GetOutlineDistance());
         file.WriteString("%*sm_dMinHeight = %0.*f\n", indent, " ", decimals, heightRange->GetMinHeight());
         if (heightRange->GetMaxHeight() > 0)
            file.WriteString("%*sm_dMaxHeight = %0.*f\n", indent, " ", decimals, heightRange->GetMaxHeight());
         else
            file.WriteString("%*sm_dMaxHeight = Infinity\n", indent, " ");
         indent -= 3;
      }
   }
}

//-----------------------------------------------------------------------------
// CPhysicalConst
//-----------------------------------------------------------------------------
CPhysicalConst::CPhysicalConst(int pageUnit)
{
   SetDefault(pageUnit);
}

void CPhysicalConst::SetDefault(int pageUnit)
{
   m_bModified = false;
   m_bEnableBoardOutine = true;
   m_bEnableCompOutline = true;
   m_bEnableMinFeatureSize = true;
   m_bEnableBeadProbeSize = true;
   m_dValueBoardOutline   = 0.100 * Units_Factor(DFT_DEFAULT_UNIT, pageUnit);
   m_dValueCompOutline    = 0.010 * Units_Factor(DFT_DEFAULT_UNIT, pageUnit);
   m_dValueMinFeatureSize = 0.040 * Units_Factor(DFT_DEFAULT_UNIT, pageUnit);
   m_dValueBeadProbeSize  = 0.004 * Units_Factor(DFT_DEFAULT_UNIT, pageUnit);
}

double CPhysicalConst::GetMinValueFeatureSize(int pageUnit) const
{
   return 0.001 * Units_Factor(DFT_DEFAULT_UNIT, pageUnit);
}

double CPhysicalConst::GetMinValueBeadProbeSize(int pageUnit) const
{
   return 0.001 * Units_Factor(DFT_DEFAULT_UNIT, pageUnit);
}

void CPhysicalConst::ResetModified()
{
   m_bModified = false;
}

void CPhysicalConst::SetEnableBoardOutline(bool enable)
{
   m_bModified |= m_bEnableBoardOutine != enable;
   m_bEnableBoardOutine = enable;
}

void CPhysicalConst::SetEnableCompOutline(bool enable)
{
   m_bModified |= m_bEnableCompOutline != enable;
   m_bEnableCompOutline = enable;
}

void CPhysicalConst::SetEnableMinFeatureSize(bool enable)
{
   m_bModified |= m_bEnableMinFeatureSize != enable;
   m_bEnableMinFeatureSize = enable;
}

void CPhysicalConst::SetEnableBeadProbeSize(bool enable)
{
   m_bModified |= m_bEnableBeadProbeSize != enable;
   m_bEnableBeadProbeSize = enable;
}

void CPhysicalConst::SetValueBoardOutline(double value)
{
   m_bModified |= m_dValueBoardOutline != value;
   m_dValueBoardOutline = value;
}

void CPhysicalConst::SetValueCompOutline(double value)
{
   m_bModified |= m_dValueCompOutline != value;
   m_dValueCompOutline = value;
}

void CPhysicalConst::SetValueMinFeatureSize(double value)
{
   m_bModified |= m_dValueMinFeatureSize != value;
   m_dValueMinFeatureSize = value;
}

void CPhysicalConst::SetValueBeadProbeSize(double value)
{
   m_bModified |= m_dValueBeadProbeSize != value;
   m_dValueBeadProbeSize = value;
}

void CPhysicalConst::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");
   file.WriteString("%*sm_bEnableBoardOutine = %s\n", indent, " ", m_bEnableBoardOutine?"True":"False");
   file.WriteString("%*sm_bEnableCompOutline = %s\n", indent, " ", m_bEnableCompOutline?"True":"False");
   file.WriteString("%*sm_bEnableMinFeatureSize = %s\n", indent, " ", m_bEnableMinFeatureSize?"True":"False");
   file.WriteString("%*sm_dValueBoardOutline = %.2f\n", indent, " ", m_dValueBoardOutline);
   file.WriteString("%*sm_dValueCompOutline = %.2f\n", indent, " ", m_dValueCompOutline);
   file.WriteString("%*sm_dValueMinFeatureSize = %.2f\n", indent, " ", m_dValueMinFeatureSize);
}

//-----------------------------------------------------------------------------
// CTargetType
//-----------------------------------------------------------------------------
CTargetType::CTargetType()
{
   SetDefault();
}

CTargetType::CTargetType(CTargetType &targetType)
{
   *this = targetType;
}

CTargetType& CTargetType::operator =(const CTargetType &targetType)
{
   if (&targetType != this)
   {
      // Copy everything in targetType to this
      m_bModified = targetType.m_bModified;
      m_bEnableTestAttrib = targetType.m_bEnableTestAttrib;
      m_bEnableVia = targetType.m_bEnableVia;
      m_bEnableConnector = targetType.m_bEnableConnector;
      m_bEnableSMD = targetType.m_bEnableSMD;
      m_bEnableSinglePinSMD = targetType.m_bEnableSinglePinSMD;
      m_bEnableTHUR = targetType.m_bEnableTHUR;
      m_bEnableCADPadstack = targetType.m_bEnableCADPadstack;
      m_padstackList.RemoveAll();

      POSITION pos = targetType.m_padstackList.GetHeadPosition();
      while (pos)
         m_padstackList.AddTail(targetType.m_padstackList.GetNext(pos));
   }

   return *this;
}

void CTargetType::SetDefault()
{
   m_bModified = false;
   m_bEnableTestAttrib = true;
   m_bEnableVia = true;
   m_bEnableConnector = true;
   m_bEnableSMD = true;
   m_bEnableSinglePinSMD = true;
   m_bEnableTHUR = true;
   m_bEnableCADPadstack = true;
   m_padstackList.RemoveAll();
}

void CTargetType::ResetModified()
{
   m_bModified = false;
}

void CTargetType::SetEnableTestAttrib(bool enable)
{
   m_bModified |= m_bEnableTestAttrib != enable;
   m_bEnableTestAttrib = enable;
}

void CTargetType::SetEnableVia(bool enable)
{
   m_bModified |= m_bEnableVia != enable;
   m_bEnableVia = enable;
}

void CTargetType::SetEnableConnector(bool enable)
{
   m_bModified |= m_bEnableConnector != enable;
   m_bEnableConnector = enable;
}

void CTargetType::SetEnableSMD(bool enable)
{  
   m_bModified |= m_bEnableSMD != enable;
   m_bEnableSMD = enable;
}

void CTargetType::SetEnableSinglePinSMD(bool enable)
{  
   m_bModified |= m_bEnableSinglePinSMD != enable;
   m_bEnableSinglePinSMD = enable;
}

void CTargetType::SetEnableTHUR(bool enable) 
{  
   m_bModified |= m_bEnableTHUR != enable;
   m_bEnableTHUR = enable;
}

void CTargetType::SetEnableCADPadstack(bool enable)
{  
   m_bModified |= m_bEnableCADPadstack != enable;
   m_bEnableCADPadstack = enable;
}

void CTargetType::AddPadstackHead(int geomNum)
{  
   m_bModified = true;
   m_padstackList.AddHead(geomNum);
}

void CTargetType::AddPadstackTail(int geomNum)
{  
   m_bModified = true;
   m_padstackList.AddTail(geomNum);
}

void CTargetType::SetPadstackAt(POSITION pos, int geomNum)
{  
   m_bModified = true;
   m_padstackList.SetAt(pos, geomNum);
}

void CTargetType::RemovePadstackAt(POSITION pos)
{  
   m_bModified = true;
   m_padstackList.RemoveAt(pos);
}

void CTargetType::RemovePadstackAll()
{  
   m_bModified |= m_padstackList.GetCount() > 0;
   m_padstackList.RemoveAll();
}

void CTargetType::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");
   file.WriteString("%*sm_bEnableTestAttrib = %s\n", indent, " ", m_bEnableTestAttrib?"True":"False");
   file.WriteString("%*sm_bEnableVia = %s\n", indent, " ", m_bEnableVia?"True":"False");
   file.WriteString("%*sm_bEnableConnector = %s\n", indent, " ", m_bEnableConnector?"True":"False");
   file.WriteString("%*sm_bEnableSMD = %s\n", indent, " ", m_bEnableSMD?"True":"False");
   file.WriteString("%*sm_bEnableSinglePinSMD = %s\n", indent, " ", m_bEnableSinglePinSMD?"True":"False");
   file.WriteString("%*sm_bEnableTHUR = %s\n", indent, " ", m_bEnableTHUR?"True":"False");
   file.WriteString("%*sm_bEnableCADPadstack = %s\n", indent, " ", m_bEnableCADPadstack?"True":"False");

   file.WriteString("%*s::Pad Stacks::\n", indent, " ");
   indent += 3;
   int count = 0;
   POSITION pos = m_padstackList.GetHeadPosition();
   while (pos)
      file.WriteString("%*s%d --> %d\n", indent, " ", count++, m_padstackList.GetNext(pos));
   indent -= 3;
}

//-----------------------------------------------------------------------------
// CPrioritizedAccessOffsetDirectionsList
//-----------------------------------------------------------------------------
CPrioritizedAccessOffsetDirectionsList::CPrioritizedAccessOffsetDirectionsList()
{
}

CPrioritizedAccessOffsetDirectionsList::CPrioritizedAccessOffsetDirectionsList(const CPrioritizedAccessOffsetDirectionsList& other)
{
   *this = other;
}

CPrioritizedAccessOffsetDirectionsList& CPrioritizedAccessOffsetDirectionsList::operator=(const CPrioritizedAccessOffsetDirectionsList& other)
{
   empty();

   for (POSITION pos = other.m_list.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag accessOffsetDirection = other.m_list.GetNext(pos);

      m_list.AddTail(accessOffsetDirection);
   }

   return *this;
}

bool CPrioritizedAccessOffsetDirectionsList::operator==(const CPrioritizedAccessOffsetDirectionsList& other) const
{
   bool retval = (m_list.GetCount() == other.m_list.GetCount());

   if (retval)
   {
      POSITION pos      = m_list.GetHeadPosition();
      POSITION otherPos = other.m_list.GetHeadPosition();

      while (retval)
      {
         if (pos == NULL || otherPos == NULL)
         {
            retval = (pos == otherPos);

            break;
         }

         AccessOffsetDirectionTag accessOffsetDirection      = m_list.GetNext(pos);
         AccessOffsetDirectionTag otherAccessOffsetDirection = other.m_list.GetNext(otherPos);

         retval = (accessOffsetDirection == otherAccessOffsetDirection);
      }      
   }

   return retval;
}

bool CPrioritizedAccessOffsetDirectionsList::operator!=(const CPrioritizedAccessOffsetDirectionsList& other) const
{
   return !(*this == other);
}

void CPrioritizedAccessOffsetDirectionsList::empty()
{
   m_list.RemoveAll();
}

void CPrioritizedAccessOffsetDirectionsList::addAll()
{
   m_list.RemoveAll();

   m_list.AddTail(accessOffsetDirectionNorth);
   m_list.AddTail(accessOffsetDirectionEast);
   m_list.AddTail(accessOffsetDirectionWest);
   m_list.AddTail(accessOffsetDirectionSouth);
   m_list.AddTail(accessOffsetDirectionUndefined);
}

void CPrioritizedAccessOffsetDirectionsList::setToComplement(const CPrioritizedAccessOffsetDirectionsList& other)
{
   addAll();

   for (POSITION pos = other.m_list.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag accessOffsetDirection = other.m_list.GetNext(pos);

      remove(accessOffsetDirection);
   }
}

bool CPrioritizedAccessOffsetDirectionsList::contains(AccessOffsetDirectionTag accessOffsetDirection) const
{
   bool retval = false;

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag existingAccessOffsetDirection = m_list.GetNext(pos);

      if (existingAccessOffsetDirection == accessOffsetDirection)
      {
         retval = true;
         break;
      }
   }

   return retval;
}

void CPrioritizedAccessOffsetDirectionsList::add(AccessOffsetDirectionTag accessOffsetDirection)
{
   for (POSITION pos = m_list.GetHeadPosition();;)
   {
      if (pos == NULL)
      {
         m_list.AddTail(accessOffsetDirection);
         break;
      }

      POSITION oldPos = pos;
      AccessOffsetDirectionTag existingAccessOffsetDirection = m_list.GetNext(pos);

      if (existingAccessOffsetDirection == accessOffsetDirection)
      {
         m_list.RemoveAt(oldPos);
      }
   }
}

void CPrioritizedAccessOffsetDirectionsList::add(const CPrioritizedAccessOffsetDirectionsList& other)
{
   for (POSITION pos = other.m_list.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag accessOffsetDirection = other.m_list.GetNext(pos);

      add(accessOffsetDirection);
   }
}

void CPrioritizedAccessOffsetDirectionsList::remove(AccessOffsetDirectionTag accessOffsetDirection)
{
   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      AccessOffsetDirectionTag existingAccessOffsetDirection = m_list.GetNext(pos);

      if (existingAccessOffsetDirection == accessOffsetDirection)
      {
         m_list.RemoveAt(oldPos);
      }
   }
}

void CPrioritizedAccessOffsetDirectionsList::remove(const CPrioritizedAccessOffsetDirectionsList& other)
{
   for (POSITION pos = other.m_list.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag accessOffsetDirection = other.m_list.GetNext(pos);

      remove(accessOffsetDirection);
   }
}

POSITION CPrioritizedAccessOffsetDirectionsList::GetHeadPosition() const
{
   return m_list.GetHeadPosition();
}

AccessOffsetDirectionTag CPrioritizedAccessOffsetDirectionsList::GetNext(POSITION& pos) const
{
   return m_list.GetNext(pos);
}

int CPrioritizedAccessOffsetDirectionsList::GetCount() const
{
   return m_list.GetCount();
}

bool CPrioritizedAccessOffsetDirectionsList::moveTowardsHead(int index)
{
   bool retval = false;

   POSITION pos = m_list.FindIndex(index);

   if (pos != NULL)
   {
      POSITION prevPos = pos;
      m_list.GetPrev(prevPos);

      if (prevPos != NULL)
      {
         AccessOffsetDirectionTag accessOffsetDirection = m_list.GetAt(pos);

         m_list.InsertBefore(prevPos,accessOffsetDirection);
         m_list.RemoveAt(pos);

         retval = true;
      }
   }

   return retval;
}

bool CPrioritizedAccessOffsetDirectionsList::moveTowardsTail(int index)
{
   bool retval = false;

   POSITION pos = m_list.FindIndex(index);

   if (pos != NULL)
   {
      POSITION nextPos = pos;
      m_list.GetNext(nextPos);

      if (nextPos != NULL)
      {
         AccessOffsetDirectionTag accessOffsetDirection = m_list.GetAt(pos);

         m_list.InsertAfter(nextPos,accessOffsetDirection);
         m_list.RemoveAt(pos);

         retval = true;
      }
   }

   return retval;
}

CString CPrioritizedAccessOffsetDirectionsList::getEncodedString() const
{
   CString encodedString;

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag accessOffsetDirection = m_list.GetNext(pos);

      encodedString += accessOffsetDirectionsTagToAbbreviatedString(accessOffsetDirection);
   }

   return encodedString;
}

void CPrioritizedAccessOffsetDirectionsList::setFromEncodedString(const CString& encodedString)
{
   empty();

   for (int index = 0;index < encodedString.GetLength();index++)
   {
      CString abbreviatedString = encodedString.GetAt(index);

      AccessOffsetDirectionTag accessOffsetDirection = stringToAccessOffsetDirectionsTag(abbreviatedString);

      add(accessOffsetDirection);
   }
}

//-----------------------------------------------------------------------------
// CAccessOffsetItem
//-----------------------------------------------------------------------------
const CString CAccessOffsetItem::m_trueString("True");
const CString CAccessOffsetItem::m_falseString("False");
CAccessOffsetItem::CAccessOffsetItem(AccessItemTypeTag itemType)
: m_itemType(itemType)
{
   initialize();
}

CAccessOffsetItem::CAccessOffsetItem(const CAccessOffsetItem& other)
{
   *this = other;
}

CAccessOffsetItem& CAccessOffsetItem::operator=(const CAccessOffsetItem& other)
{
   m_enableSmdOffsetFlag   = other.m_enableSmdOffsetFlag;
   m_enableThOffsetFlag    = other.m_enableThOffsetFlag;
   m_smdOffsetPlacement    = other.m_smdOffsetPlacement;
   m_directionsList        = other.m_directionsList;

   return *this;
}

bool CAccessOffsetItem::operator==(const CAccessOffsetItem& other) const
{
   bool retval = ( m_enableSmdOffsetFlag   == other.m_enableSmdOffsetFlag &&
                   m_enableThOffsetFlag    == other.m_enableThOffsetFlag  &&
                   m_smdOffsetPlacement    == other.m_smdOffsetPlacement  &&
                   m_directionsList        == other.m_directionsList         );

   return retval;
}

bool CAccessOffsetItem::operator!=(const CAccessOffsetItem& other) const
{
   return !(*this == other);
}

void CAccessOffsetItem::initialize()
{
   m_enableSmdOffsetFlag   = true;
   m_enableThOffsetFlag    = true;
   m_smdOffsetPlacement    = accessOffsetPlacementUndefined;

   m_directionsList.empty();
   m_directionsList.add(accessOffsetDirectionUndefined);
}

CString CAccessOffsetItem::getThOffsetDirectionMaskString() const
{
   return m_directionsList.getEncodedString();
}

void CAccessOffsetItem::setThOffsetDirectionMaskString(const CString& maskString)
{
   m_directionsList.setFromEncodedString(maskString);
}

AccessOffsetPlacementTag CAccessOffsetItem::getSmdOffsetPlacement() const
{
   return m_smdOffsetPlacement;
}

void CAccessOffsetItem::setSmdOffsetPlacement(const CString& tagValue)
{
   m_smdOffsetPlacement = stringToAccessOffsetPlacementTag(tagValue);
}

void CAccessOffsetItem::setSmdOffsetPlacement(AccessOffsetPlacementTag tagValue)
{
   m_smdOffsetPlacement = tagValue;
}

CString CAccessOffsetItem::getSmdOffsetPlacementString() const
{
   return accessOffsetPlacementTagToSimpleString(m_smdOffsetPlacement);
}

//-----------------------------------------------------------------------------
// CAccessOffsetOptions
//-----------------------------------------------------------------------------
CAccessOffsetOptions::CAccessOffsetOptions()
: m_pinOffsetItem(accessItemTypePin)
, m_viaOffsetItem(accessItemTypeVia)
, m_tpOffsetItem(accessItemTypeTp)
{
   initialize();
}

CAccessOffsetOptions::CAccessOffsetOptions(const CAccessOffsetOptions& other)
: m_pinOffsetItem(accessItemTypePin)
, m_viaOffsetItem(accessItemTypeVia)
, m_tpOffsetItem(accessItemTypeTp)
{
   *this = other;
}

CAccessOffsetOptions& CAccessOffsetOptions::operator=(const CAccessOffsetOptions& other)
{
   m_enableOffsetFlag = other.m_enableOffsetFlag;
   m_pinOffsetItem    = other.m_pinOffsetItem;
   m_viaOffsetItem    = other.m_viaOffsetItem;
   m_tpOffsetItem     = other.m_tpOffsetItem;

   return *this;
}

bool CAccessOffsetOptions::operator==(const CAccessOffsetOptions& other) const
{
   bool retval = ( m_enableOffsetFlag == other.m_enableOffsetFlag &&
                   m_pinOffsetItem    == other.m_pinOffsetItem    &&
                   m_viaOffsetItem    == other.m_viaOffsetItem    &&
                   m_tpOffsetItem     == other.m_tpOffsetItem         );

   return retval;
}

bool CAccessOffsetOptions::operator!=(const CAccessOffsetOptions& other) const
{
   return !(*this == other);
}

void CAccessOffsetOptions::initialize()
{
   m_enableOffsetFlag = true;

   m_pinOffsetItem.initialize();
   m_viaOffsetItem.initialize();
   m_tpOffsetItem.initialize();

   m_pinOffsetItem.setSmdOffsetPlacement(accessOffsetPlacementDistal);
}

CAccessOffsetItem& CAccessOffsetOptions::getAccessOffsetItem(AccessItemTypeTag accessOffsetItem)
{
   CAccessOffsetItem* item;

   switch (accessOffsetItem)
   {
   case accessItemTypePin:  item = &m_pinOffsetItem;  break;
   case accessItemTypeVia:  item = &m_viaOffsetItem;  break;
   default:
   case accessItemTypeTp:   item = &m_tpOffsetItem;   break;
   }

   return *item;
}

const CAccessOffsetItem& CAccessOffsetOptions::getAccessOffsetItem(AccessItemTypeTag accessOffsetItem) const
{
   const CAccessOffsetItem* item;

   switch (accessOffsetItem)
   {
   case accessItemTypePin:  item = &m_pinOffsetItem;  break;
   case accessItemTypeVia:  item = &m_viaOffsetItem;  break;
   default:
   case accessItemTypeTp:   item = &m_tpOffsetItem;   break;
   }

   return *item;
}

bool CAccessOffsetOptions::isOffsetAccessAllowed(AccessItemTypeTag itemType,int targetType) const
{
   bool accessAllowedFlag = false;

   if (m_enableOffsetFlag)
   {
      bool targetTypeIsTh    = ((targetType & DFT_TARGETTYPE_THRUHOLE     ) != 0);
      bool targetTypeIsSmd   = ((targetType & DFT_TARGETTYPE_SMD          ) != 0);

      bool targetTypeIsVia   = (itemType == accessItemTypeVia);
      bool targetTypeIsTp    = (itemType == accessItemTypeTp );
      bool targetTypeIsPin   = (itemType == accessItemTypePin);

      if (!accessAllowedFlag && targetTypeIsVia)
      {
         const CAccessOffsetItem& accessOffsetItem = getAccessOffsetItem(accessItemTypeVia);

         accessAllowedFlag = ((accessOffsetItem.getEnableThOffsetFlag()  && targetTypeIsTh ) ||
                              (accessOffsetItem.getEnableSmdOffsetFlag() && targetTypeIsSmd)    );
      }

      if (!accessAllowedFlag && targetTypeIsTp)
      {
         const CAccessOffsetItem& accessOffsetItem = getAccessOffsetItem(accessItemTypeTp);

         accessAllowedFlag = ((accessOffsetItem.getEnableThOffsetFlag()  && targetTypeIsTh ) ||
                              (accessOffsetItem.getEnableSmdOffsetFlag() && targetTypeIsSmd)    );
      }

      if (!accessAllowedFlag && targetTypeIsPin)
      {
         const CAccessOffsetItem& accessOffsetItem = getAccessOffsetItem(accessItemTypePin);

         accessAllowedFlag = ((accessOffsetItem.getEnableThOffsetFlag()  && targetTypeIsTh ) ||
                              (accessOffsetItem.getEnableSmdOffsetFlag() && targetTypeIsSmd)    );
      }
   }

   return accessAllowedFlag;
}

AccessItemTypeTag CAccessOffsetOptions::getAccessOffsetItemTypeFromTargetType(int targetType) const
{
   AccessItemTypeTag accessOffsetItemType = accessItemTypePin;

   if ((targetType & DFT_TARGETTYPE_VIA          ) != 0)
   {
      accessOffsetItemType = accessItemTypeVia;
   }
   else if ((targetType & DFT_TARGETTYPE_TESTATTRIBUTE) != 0)
   {
      accessOffsetItemType = accessItemTypeTp;
   }

   return accessOffsetItemType;
}

//-----------------------------------------------------------------------------
// CExposedData
//-----------------------------------------------------------------------------
int CExposedData::m_nextId = 1;
CExposedData::CExposedData(CCEtoODBDoc& camCadDoc,AccessItemTypeTag itemType,
   DataStruct* data, BlockStruct* padStackGeometry,
   CPoint2d compCentriod, CPoint2d dataCentriod,
   int entityNum, int targetType, bool singlePin, bool unloaded)
: m_camCadDoc(camCadDoc)
, m_itemType(itemType)
, m_data(data)
, m_padStackGeometry(padStackGeometry)
{
   m_id = m_nextId++;

   m_ptCompCentroid = compCentriod;       
   m_ptDataCentroid = dataCentriod;       
   m_iDataEntityNum = entityNum;       
   m_eDataShape = apertureUndefined;
   SetTargetType(targetType);            
   m_dMetalDiameter = 0.0;    
   m_bSinglePin = singlePin;
   m_bUnloaded = unloaded;             
   m_failureReason.SetReasonCode(DFT_AA_RC_NO_METAL);
   m_pExposedData = NULL;     
}

CExposedData::~CExposedData()
{
   m_pExposedData = NULL;
}

void CExposedData::SetTargetType(int targetType)
{
   m_iTargetType = targetType;
}

bool CExposedData::GetAccessLocation(CCEtoODBDoc* doc, FileStruct* pPCBFile, const CAccessOffsetOptions& accessOffsetOptions, 
                                     double featureSize, double& x, double& y, double& metalDiameter, double tolerance)
{
   metalDiameter = 0.0;
   x = 0.0;
   y = 0.0;

   bool res = false;

   if (doc == NULL || pPCBFile == NULL || m_pExposedData == NULL || m_pExposedData->getDataType() != dataTypePoly)
      return res;

   double inCircleTolerance = doc->convertToPageUnits(pageUnitsMils,.5);
   double featureSizeTolerance = doc->convertToPageUnits(pageUnitsMils,0.1);
   bool offsetAccessAllowedFlag = accessOffsetOptions.isOffsetAccessAllowed(getItemType(),GetTargetType());

   return res;
}

bool CExposedMetal::calculateLocationAndMaxSize(double featureSize, double& x, double& y, double& metalDiameter, double tolerance)
{
   metalDiameter = 0.0;
   x = 0.0;
   y = 0.0;

   if (m_pExposedData == NULL || m_pExposedData->getDataType() != dataTypePoly)
      return false;

   CPolyList *polyList = m_pExposedData->getPolyList();



   if (polyList == NULL)
      return false;

   CPoly *poly = new CPoly;
   poly->setWidthIndex(0);
   poly->setFilled(true);
   poly->setVoid(false);
   poly->setClosed(true);
   poly->setThermalLine(false);
   poly->setFloodBoundary(false);
   poly->setHidden(0);
   poly->setHatchLine(false);

   int returnCode;
   CPntList *CPntList = Outline_GetOutline(polyList,&returnCode);
   if(CPntList)
   {
      POSITION pos = CPntList->GetHeadPosition();
      while (pos)
      {
         CPnt *pnt = CPntList->GetNext(pos);
         poly->getPntList().AddTail(pnt);
      }
      m_pExposedData->getPolyList()->empty();
      m_pExposedData->getPolyList()->AddTail(poly);
   }

   if (polyList->isPointInside(m_ptDataCentroid,getCamCadDoc().getPageUnits(),tolerance) == false)
      return false;
   
   metalDiameter = polyList->distanceTo(m_ptDataCentroid) * 2; 
   x = m_ptDataCentroid.x;
   y = m_ptDataCentroid.y;

   return true;
}

double CExposedMetal::getDrillSize() const
{
   double drillSize = 0.;

   for (CDataListIterator toolIterator(m_padStackGeometry,dataTypeInsert);toolIterator.hasNext();)
   {
      DataStruct* toolData = toolIterator.getNext();
      BlockStruct* toolGeometry = getCamCadDoc().getBlockAt(toolData->getInsert()->getBlockNumber());

      if (toolGeometry->isTool())
      {
         double toolSize = toolGeometry->getToolSize();

         if (toolSize > drillSize)
         {
            drillSize = toolSize;
         }
      }
   }

   return drillSize;
}

CPoly* CExposedMetal::getExposedPoly()
{
   CPoly* exposedPoly = NULL;

   if (m_pExposedData->getDataType() == dataTypePoly &&
       m_pExposedData->getPolyList() != NULL            )
   {
      exposedPoly = m_pExposedData->getPolyList()->getMaximumAreaClosedPoly(getCamCadDoc().getPageUnits());
   }

   return exposedPoly;
}

bool CExposedData::isSmd() const
{
   bool retval = ((GetTargetType() & DFT_TARGETTYPE_SMD) != 0);

   return retval;
}

bool CExposedData::isThruHole() const
{
   bool retval = ((GetTargetType() & DFT_TARGETTYPE_THRUHOLE) != 0);

   return retval;
}

bool CExposedMetal::calculateOffsetLocation(CCEtoODBDoc* doc, FileStruct* pPCBFile, 
   const CAccessOffsetOptions& accessOffsetOptions, double featureSize,
   double& x, double& y, double& metalDiameter, double tolerance)
{
   metalDiameter = 0.0;
   x = 0.0;
   y = 0.0;

   CPoly* poly;
   bool res = false;

   if (doc == NULL || 
       pPCBFile == NULL || 
       m_pExposedData == NULL || 
       m_pExposedData->getDataType() != dataTypePoly ||
       m_pExposedData->getPolyList() == NULL ||
       (poly = m_pExposedData->getPolyList()->getMaximumAreaClosedPoly(getCamCadDoc().getPageUnits())) == NULL)
   {
      return res;
   }

   //AccessItemTypeTag accessOffsetItemTag   = accessOffsetOptions.getAccessOffsetItemTypeFromTargetType(GetTargetType());
   const CAccessOffsetItem& accessOffsetItem = accessOffsetOptions.getAccessOffsetItem(getItemType());

   // Check if poly is a circle
   double radius = 0.0;
   double cx = 0.0, cy = 0.0;
   bool isCircle = PolyIsCircle(poly, &cx, &cy, &radius);
   double drillSize = getDrillSize();
   bool isThruHole  = (drillSize > 0.);

   CExtent exposedExtent = poly->getExtent();

#ifdef DebugAccessAnalysisOffset
   // debug
   CPolygon debugPolygon(*poly);
   CDebug::addDebugPolygon(getCamCadDoc(),debugPolygon,"db calculateOffsetLocation()");
#endif

   if (isCircle)
   {
      if (isThruHole)
      {
         double ringWidth = radius - drillSize/2.;
         res = findFarthestOffsetInCircle(radius, ringWidth, x, y,accessOffsetItem);

         if (res) metalDiameter = ringWidth;
      }
      else
      {
         if ((featureSize - exposedExtent.getXsize()) > SMALLNUMBER || 
             (featureSize - exposedExtent.getYsize()) > SMALLNUMBER    )
         {
            metalDiameter = radius * 2;

            return res;
         }
         else
         {
            res = findFarthestOffsetInCircle(radius, featureSize, x, y,accessOffsetItem);

            if (res) metalDiameter = featureSize;
         }
      }
   }
   else
   {
      CPolyList* polyList = m_pExposedData->getPolyList();  

      if (polyList != NULL)
      {
         CPoint2d inCircleCenter;
         double inCircleRadius;

         bool foundInCircleFlag = false;

         if (isThruHole)
         {
            foundInCircleFlag = getCentralPointWithHoleAvoidance(*polyList,inCircleCenter,inCircleRadius,m_ptDataCentroid,
                                   drillSize/2.,featureSize,tolerance,&accessOffsetItem);
         }
         else
         {
            foundInCircleFlag = getCentralPoint(*polyList,inCircleCenter,inCircleRadius,featureSize,tolerance,&accessOffsetItem);
         }

         if (foundInCircleFlag)
         {
            x = inCircleCenter.x;
            y = inCircleCenter.y;
            metalDiameter = 2. * inCircleRadius;

            res = true;
         }
      }
   }

   return res;
}

bool CExposedMetal::getCentralPoint(CPolyList& polyList, CPoint2d& inCircleCenter, double& inCircleRadius,
   double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval = false;
   inCircleRadius = 0.;
   double radius;
   CPoint2d center;

   for (POSITION pos = polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = polyList.GetNext(pos);
      CPolygon polygon(*poly,getCamCadDoc().getPageUnits());

      //addDebugPolygon(*(getActiveDocument()),polygon,"db getMaximumInCircle()");

      if (getCentralPoint(polygon,center,radius,minFeatureSize,tolerance,accessOffsetItem))
      {
         if (radius > inCircleRadius)
         {
            inCircleRadius = radius;
            inCircleCenter = center;
            retval = true;
         }
      }
   }

   return retval;
}

bool CExposedMetal::getCentralPointWithHoleAvoidance(CPolyList& polyList,CPoint2d& inCircleCenter,double& inCircleRadius,
   CPoint2d holeCenter,double holeRadius,double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval = false;
   inCircleRadius = 0.;
   double radius;
   CPoint2d center;

   for (POSITION pos = polyList.GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = polyList.GetNext(pos);
      CPolygon polygon(*poly,getCamCadDoc().getPageUnits());

#ifdef DebugAccessAnalysisOffset
      // debug
      CDebug::addDebugPolygon(*(getActiveDocument()),polygon,"db getMaximumInCircleWithHoleAvoidance()");
#endif

      if (getCentralPointWithHoleAvoidance(polygon,center,radius,minFeatureSize,tolerance,holeCenter,holeRadius,accessOffsetItem))
      {
         if (radius > inCircleRadius)
         {
            inCircleRadius = radius;
            inCircleCenter = center;
            retval = true;
         }
      }
   }

   return retval;
}

bool CExposedMetal::getCentralPoint(CPolygon& polygon,CPoint2d& inCircleCenter,double& circleRadius,
   double minFeatureSize,double tolerance,const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval = false;

   if (m_pExposedData->getEntityNumber() == 1231)
   {
      int iii = 3;
   }

   polygon.makeCcw();
   CExtent extent = polygon.getExtent();
   CSize2d grid   = extent.getSize() * 1.5;
   CPoint2d point = extent.getCenter();
   int maxDepth = 2;

   if (breadthSearchNearestCentralPoint(polygon,inCircleCenter,circleRadius,0,point,grid,maxDepth,minFeatureSize,tolerance,accessOffsetItem))
   {
      CPoint2d dsInCircleCenter = inCircleCenter;
      double dsCircleRadius     = circleRadius;

      point = inCircleCenter;
      grid = grid / (1 << (maxDepth + 1));

      if (depthSearchNearestCentralPoint(polygon,dsInCircleCenter,dsCircleRadius,maxDepth,point,grid,minFeatureSize,tolerance,accessOffsetItem))
      {
         inCircleCenter = dsInCircleCenter;
         circleRadius   = dsCircleRadius;
      }

      retval = true;
   }

   return retval;
}

bool CExposedMetal::getCentralPointWithHoleAvoidance(CPolygon& polygon,CPoint2d& inCircleCenter,double& circleRadius,
   double minFeatureSize,double tolerance,
   CPoint2d holeCenter,double holeRadius,const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval = false;

   polygon.makeCcw();
   CExtent extent = polygon.getExtent();

   if (accessOffsetItem != NULL)
   {  // adjust extent so that it is centered on an orhogonal direction if it spans that direction
      CPoint2d extentCenter = extent.getCenter();

      bool northSpan = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionNorth) &&
                        extent.getYmax() >= holeCenter.y + holeRadius &&
                        extent.isOnOrInside(CPoint2d(holeCenter.x,extentCenter.y)));

      bool southSpan = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionSouth) &&
                        extent.getYmin() <= holeCenter.y - holeRadius &&
                        extent.isOnOrInside(CPoint2d(holeCenter.x,extentCenter.y)));

      bool westSpan  = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionWest) &&
                        extent.getXmax() >= holeCenter.x + holeRadius &&
                        extent.isOnOrInside(CPoint2d(extentCenter.x,holeCenter.y)));

      bool eastSpan  = (accessOffsetItem->getDirectionList().contains(accessOffsetDirectionEast) &&
                        extent.getXmin() <= holeCenter.x - holeRadius &&
                        extent.isOnOrInside(CPoint2d(extentCenter.x,holeCenter.y)));

      if (northSpan || southSpan)
      {
         double dx = max(holeCenter.x - extent.getXmin(),extent.getXmax() - holeCenter.x);

         extent.update(CPoint2d(holeCenter.x + dx,extentCenter.y));
         extent.update(CPoint2d(holeCenter.x - dx,extentCenter.y));
      }

      if (westSpan || eastSpan)
      {
         double dy = max(holeCenter.y - extent.getYmin(),extent.getYmax() - holeCenter.y);

         extent.update(CPoint2d(extentCenter.x,holeCenter.y + dy));
         extent.update(CPoint2d(extentCenter.x,holeCenter.y - dy));
      }
   }

   CSize2d grid   = extent.getSize() * 1.5;
   CPoint2d point = extent.getCenter();
   int maxDepth = 2;

   if (breadthSearchNearestCentralPointWithHoleAvoidance(polygon,inCircleCenter,circleRadius,
          holeCenter,holeRadius,0,point,grid,maxDepth,minFeatureSize,tolerance,accessOffsetItem))
   {
      point = inCircleCenter;
      grid = grid / (1 << (maxDepth + 1));

      if (depthSearchNearestCentralPointWithHoleAvoidance(polygon,inCircleCenter,circleRadius,
             holeCenter,holeRadius,maxDepth,point,grid,minFeatureSize,tolerance,accessOffsetItem))
      {
         retval = true;
      }
   }

   return retval;
}

CPoint2d CExposedMetal::getTargetPoint(const CAccessOffsetItem* accessOffsetItem) const
{
   CPoint2d targetPoint;

   if (accessOffsetItem != NULL)
   {
      switch (accessOffsetItem->getSmdOffsetPlacement())
      {
      case accessOffsetPlacementProximal:
         targetPoint = m_ptCompCentroid;
         break;
      case accessOffsetPlacementMedial:
         targetPoint = m_ptDataCentroid;
         break;
      case accessOffsetPlacementDistal:
         {
            CVector2d directionVector(m_ptDataCentroid,m_ptCompCentroid);
            directionVector.makePerpendicular(true);
            directionVector.makePerpendicular(true);
            targetPoint = directionVector.getTip();
         }
         break;
      }
   }

   return targetPoint;
}

CPoint2d CExposedMetal::getGridOffsetPoint(const CPoint2d& probePoint,int index,const CSize2d& searchGrid) const
{
   CPoint2d offsetPoint(probePoint);

   switch (index)
   {
   case 0:
      offsetPoint.x = probePoint.x - searchGrid.cx;
      offsetPoint.y = probePoint.y + searchGrid.cy;
      break;
   case 1:
      offsetPoint.x = probePoint.x;
      offsetPoint.y = probePoint.y + searchGrid.cy;
      break;
   case 2:
      offsetPoint.x = probePoint.x + searchGrid.cx;
      offsetPoint.y = probePoint.y + searchGrid.cy;
      break;

   case 3:
      offsetPoint.x = probePoint.x - searchGrid.cx;
      offsetPoint.y = probePoint.y;
      break;
 //case 4:
 //   offsetPoint.x = probePoint.x;
 //   offsetPoint.y = probePoint.y;
 //   break;
   case 5:
      offsetPoint.x = probePoint.x + searchGrid.cx;
      offsetPoint.y = probePoint.y;
      break;

   case 6:
      offsetPoint.x = probePoint.x - searchGrid.cx;
      offsetPoint.y = probePoint.y - searchGrid.cy;
      break;
   case 7:
      offsetPoint.x = probePoint.x;
      offsetPoint.y = probePoint.y - searchGrid.cy;
      break;
   case 8:
      offsetPoint.x = probePoint.x + searchGrid.cx;
      offsetPoint.y = probePoint.y - searchGrid.cy;
      break;
   }

   return offsetPoint;
}

bool CExposedMetal::breadthSearchNearestCentralPoint(CPolygon& polygon,
   CPoint2d& pointNearestInCentralPoint,double& inCircleRadius,
   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double minFeatureSize,double tolerance,
   const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval  = false;
   CPoint2d targetPoint = getTargetPoint(accessOffsetItem);

#ifdef DebugAccessAnalysisOffset
   // debug
   CDebug::addDebugPoint(probePoint,"db breadthSearchNearestCentralPoint(%d) - probePoint",depth);

   if (depth == 0 && accessOffsetItem != NULL)
   {
      CDebug::addDebugPoint(targetPoint,"db breadthSearchNearestCentralPoint(%d) - targetPoint",depth);
   }
#endif

   if (depth == maxDepth)
   {
      double probeDistance = polygon.distanceTo(probePoint);

      // inside polygon
      if (probeDistance < 0.)
      {
         inCircleRadius = -probeDistance;
         pointNearestInCentralPoint = probePoint;
         retval = true;
      }
   }
   else if (depth < maxDepth)
   {
      const double minProbeDistanceRatio = .9;
      double maxInCircleRadius = 0.;
      double minOffsetDistance = DBL_MAX;

      inCircleRadius = 0.;
      CSize2d searchGrid = gridSize/3.;

      for (int index = 0;index < 9;index++)
      {
         CPoint2d point = getGridOffsetPoint(probePoint,index,searchGrid);
         CPoint2d foundPoint;
         double distance;
         double offsetDistance;

#ifdef DebugAccessAnalysisOffset
         // debug
         //CDebug::addDebugPoint(point,"db breadthSearchNearestCentralPoint(depth=%d) - %s",depth,
         //            (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
#endif

         if (breadthSearchNearestCentralPoint(polygon,foundPoint,distance,
                  depth + 1,point,searchGrid,maxDepth,minFeatureSize,tolerance,accessOffsetItem))
         {
            bool foundBetterPointFlag = false;
            offsetDistance = DBL_MAX;

            if (accessOffsetItem != NULL)
            {
               offsetDistance = foundPoint.distance(targetPoint);
            }

            if (maxInCircleRadius == 0.)
            {
               foundBetterPointFlag = true;
               minOffsetDistance = offsetDistance;
            }
            else if (distance > 0.)
            {
               if ((maxInCircleRadius / distance) < minProbeDistanceRatio)
               {
                  foundBetterPointFlag = true;
                  minOffsetDistance = offsetDistance;
               }
               else if ((distance / maxInCircleRadius) >= minProbeDistanceRatio)
               {
                  if (offsetDistance < minOffsetDistance)
                  {
                     foundBetterPointFlag = true;
                     minOffsetDistance = offsetDistance;
                  }
               }
               else if (distance > maxInCircleRadius)
               {
                  foundBetterPointFlag = true;
                  minOffsetDistance = offsetDistance;
               }
            }

            if (foundBetterPointFlag)
            {
               inCircleRadius = distance;
               pointNearestInCentralPoint = foundPoint;
               retval = true;

               if (distance > maxInCircleRadius)
               {
                  maxInCircleRadius = distance;
               }
            }
         }
      }

#ifdef DebugAccessAnalysisOffset
      if (retval)
      {
         // debug
         CDebug::addDebugPoint(pointNearestInCentralPoint,"db breadthSearchNearestCentralPoint(%d) - Found Point",depth);
      }
#endif
   }

   return retval;
}

bool CExposedMetal::breadthSearchNearestCentralPointWithHoleAvoidance(CPolygon& polygon,
   CPoint2d& pointNearestInCentralPoint,double& inCircleRadius,
   CPoint2d holeCenter,double holeRadius,
   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,int maxDepth,double minFeatureSize,double tolerance,
   const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval  = false;

#ifdef DebugAccessAnalysisOffset
   // debug
   CDebug::addDebugPoint(probePoint,"db breadthSearchNearestCentralPointWithHoleAvoidance(%d)",depth);
#endif

   if (depth == maxDepth)
   {
      double probeDistance = polygon.distanceTo(probePoint);
      double holeDistance  = holeCenter.distance(probePoint) - holeRadius;

      // inside polygon and outside hole
      if (probeDistance < 0. && holeDistance >= 0.)
      {
         inCircleRadius = min(-probeDistance,holeDistance);
         pointNearestInCentralPoint = probePoint;
         retval = true;

#ifdef DebugAccessAnalysisOffset
         // debug
         CDebug::addDebugPoint(pointNearestInCentralPoint,"db breadthSearchNearestCentralPointWithHoleAvoidance(%d) - Found Point",depth);
#endif
      }
   }
   else if (depth < maxDepth)
   {
      inCircleRadius = 0.;
      CSize2d searchGrid = gridSize/3.;
      bool validDirectionFlag = true;

      for (int index = 0;index < 9;index++)
      {
         CPoint2d point = getGridOffsetPoint(probePoint,index,searchGrid);

         if (accessOffsetItem != NULL)
         {
            AccessOffsetDirectionTag loopDirection = calcAccessOffsetDirection(holeCenter,point,tolerance);
            validDirectionFlag = accessOffsetItem->getDirectionList().contains(loopDirection);
         }

         if (validDirectionFlag)
         {
            CPoint2d foundPoint;
            double distance;

#ifdef DebugAccessAnalysisOffset
            // debug
            //CDebug::addDebugPoint(point,"db breadthSearchNearestCentralPointWithHoleAvoidance(depth=%d) - Valid Direction - %s",depth,
            //            (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
#endif

            if (breadthSearchNearestCentralPointWithHoleAvoidance(polygon,foundPoint,distance,
                   holeCenter,holeRadius,depth + 1,point,searchGrid,maxDepth,minFeatureSize,tolerance,accessOffsetItem))
            {
               if (distance > inCircleRadius)
               {
                  inCircleRadius = distance;
                  pointNearestInCentralPoint = foundPoint;
                  retval = true;

                  // debug
                  //if (depth == 0)
                  //{
                  //   addDebugPoint(foundPoint,"db breadthSearchNearestCentralPointWithHoleAvoidance(nearest)");
                  //}
               }
            }
         }
         else
         {
#ifdef DebugAccessAnalysisOffset
            // debug
            //CDebug::addDebugPoint(point,"db breadthSearchNearestCentralPointWithHoleAvoidance(depth=%d) - Invalid Direction - %s",depth,
            //           (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
#endif
         }
      }
   }

   return retval;
}

bool CExposedMetal::depthSearchNearestCentralPoint(CPolygon& polygon,
   CPoint2d& pointNearestInCentralPoint,double& inCircleRadius,
   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double minFeatureSize,double tolerance,
   const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval = false;
   CPoint2d targetPoint = getTargetPoint(accessOffsetItem);

#ifdef DebugAccessAnalysisOffset
   // debug
   CDebug::addDebugPoint(probePoint,"db depthSearchNearestCentralPoint(%d) - probePoint",depth);
#endif

   double probeDistance = polygon.distanceTo(probePoint);
   double minFeatureRadius = minFeatureSize / 2.;

   if (probeDistance < 0.)
   {
      double distance = -probeDistance;
      double gridMaxSize = gridSize.getMaxSize();

      if (gridMaxSize <= tolerance)
      {
         inCircleRadius = distance;
         pointNearestInCentralPoint = probePoint;
         retval = true;

#ifdef DebugAccessAnalysisOffset
         // debug
         CDebug::addDebugPoint(probePoint,"db depthSearchNearestCentralPoint(%d) - Found Point",depth);
#endif
      }
      else /*if (minFeatureSize < distance + 2. * gridMaxSize)*/
      {
         const double minProbeDistanceRatio = .9;
         double maxInCircleRadius = 0.;
         double minOffsetDistance = DBL_MAX;

         inCircleRadius = 0.;
         CSize2d searchGrid = gridSize/3.;

         for (int index = 0;index < 9;index++)
         {
            CPoint2d point = getGridOffsetPoint(probePoint,index,searchGrid);

            //double distance;
            double offsetDistance;
            probeDistance = -polygon.distanceTo(point);

            //if (depthSearchNearestCentralPoint(polygon,foundPoint,distance,
            //      depth + 1,point,searchGrid,minFeatureSize,tolerance,accessOffsetItem))
            if (probeDistance > minFeatureRadius)
            {
               bool foundBetterPointFlag = false;
               offsetDistance = DBL_MAX;

               if (accessOffsetItem != NULL)
               {
                  offsetDistance = point.distance(targetPoint);
               }

               if (maxInCircleRadius == 0.)
               {
                  foundBetterPointFlag = true;
                  minOffsetDistance = offsetDistance;
               }
               else if ((maxInCircleRadius / probeDistance) < minProbeDistanceRatio)
               {
                  foundBetterPointFlag = true;
                  minOffsetDistance = offsetDistance;
               }
               else if ((probeDistance / maxInCircleRadius) >= minProbeDistanceRatio)
               {
                  if (offsetDistance < minOffsetDistance)
                  {
                     foundBetterPointFlag = true;
                     minOffsetDistance = offsetDistance;
                  }
               }
               else if (probeDistance > maxInCircleRadius)
               {
                  foundBetterPointFlag = true;
                  minOffsetDistance = offsetDistance;
               }

               if (foundBetterPointFlag)
               {
                  inCircleRadius = probeDistance;
                  pointNearestInCentralPoint = point;
                  retval = true;

                  if (distance > maxInCircleRadius)
                  {
                     maxInCircleRadius = distance;
                  }

#ifdef DebugAccessAnalysisOffset
                  // debug
                  //CDebug::addDebugPoint(pointNearestInCentralPoint,
                  //            "db depthSearchNearestCentralPoint(depth=%d) - Valid Direction - %s",depth,
                  //            (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
#endif
               }
            }
         }

         if (retval)
         {
            double dsInCircleRadius = inCircleRadius;
            CPoint2d dsProbePoint   = pointNearestInCentralPoint;

            if (depthSearchNearestCentralPoint(polygon,pointNearestInCentralPoint,dsInCircleRadius,
                  depth + 1,dsProbePoint,searchGrid,minFeatureSize,tolerance,accessOffsetItem))
            {
               inCircleRadius = dsInCircleRadius;
            }
         }
      }
   }

   return retval;
}

bool CExposedMetal::depthSearchNearestCentralPointWithHoleAvoidance(CPolygon& polygon,
   CPoint2d& pointNearestInCentralPoint,double& inCircleRadius,
   CPoint2d holeCenter,double holeRadius,
   int depth,const CPoint2d& probePoint,const CSize2d& gridSize,double minFeatureSize,double tolerance,
   const CAccessOffsetItem* accessOffsetItem) const
{
   bool retval = false;

#ifdef DebugAccessAnalysisOffset
   // debug
   CDebug::addDebugPoint(probePoint,"db depthSearchNearestCentralPointWithHoleAvoidance(%d)",depth);
#endif

   double probeDistance = polygon.distanceTo(probePoint);
   double holeDistance  = holeCenter.distance(probePoint) - holeRadius;

   if (probeDistance < 0. && holeDistance >= 0.)
   {
      double distance = min(-probeDistance,holeDistance);
      double gridMaxSize = gridSize.getMaxSize();

      if (gridMaxSize <= tolerance)
      {
         inCircleRadius = distance;
         pointNearestInCentralPoint = probePoint;
         retval = true;

#ifdef DebugAccessAnalysisOffset
         // debug
         CDebug::addDebugPoint(probePoint,"db depthSearchNearestCentralPointWithHoleAvoidance(%d) - Found Point",depth);
#endif
      }
      else if (inCircleRadius < distance + 2. * gridMaxSize)
      {
         inCircleRadius = 0.;
         CSize2d searchGrid = gridSize/3.;
         bool validDirectionFlag = true;

         for (int index = 0;index < 9;index++)
         {
            CPoint2d point = getGridOffsetPoint(probePoint,index,searchGrid);

            if (accessOffsetItem != NULL)
            {
               AccessOffsetDirectionTag loopDirection = calcAccessOffsetDirection(holeCenter,point,tolerance);
               validDirectionFlag = accessOffsetItem->getDirectionList().contains(loopDirection);
            }

            if (validDirectionFlag)
            {
               CPoint2d foundPoint;

               if (depthSearchNearestCentralPointWithHoleAvoidance(polygon,foundPoint,distance,
                     holeCenter,holeRadius,depth + 1,point,searchGrid,minFeatureSize,tolerance,accessOffsetItem))
               {
                  if (distance > inCircleRadius)
                  {
                     inCircleRadius = distance;
                     pointNearestInCentralPoint = foundPoint;
                     retval = true;

#ifdef DebugAccessAnalysisOffset
                     // debug
                     //CDebug::addDebugPoint(pointNearestInCentralPoint,
                     //           "db depthSearchNearestCentralPointWithHoleAvoidance(depth=%d) - Valid Direction - %s",depth,
                     //           (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
#endif
                  }
               }
            }
            else
            {
#ifdef DebugAccessAnalysisOffset
               // debug
               //CDebug::addDebugPoint(point,
               //         "db depthSearchNearestCentralPointWithHoleAvoidance(depth=%d) - Invalid Direction - %s",depth,
               //         (accessOffsetItem != NULL) ? accessItemTypeTagToString(accessOffsetItem->getItemType()) : "null item");
#endif
            }
         }
      }
   }

   return retval;
}

bool CExposedMetal::findFarthestOffsetInCircle(double radius, double featureSize, double &x, double &y,const CAccessOffsetItem& accessOffsetItem)
{
   x = 0.0;
   y = 0.0;

   int targetType = GetTargetType();

   bool targetTypeIsTh    = ((targetType & DFT_TARGETTYPE_THRUHOLE     ) != 0);
   bool targetTypeIsSmd   = ((targetType & DFT_TARGETTYPE_SMD          ) != 0);
   bool targetTypeIsVia   = ((targetType & DFT_TARGETTYPE_VIA          ) != 0);
   bool targetTypeIsTp    = ((targetType & DFT_TARGETTYPE_TESTATTRIBUTE) != 0);
   bool targetTypeIsPin   = (!targetTypeIsVia && !targetTypeIsTp);
   bool anyThDirectionFlag = (accessOffsetItem.getEnableThOffsetFlag() && accessOffsetItem.getDirectionList().contains(accessOffsetDirectionUndefined));

   if (targetTypeIsPin && 
      (m_ptCompCentroid != m_ptDataCentroid) && 
       (targetTypeIsSmd || (targetTypeIsTh && anyThDirectionFlag)))
   {
      double hyp    = m_ptCompCentroid.distance(m_ptDataCentroid);
      double tmpX   = m_ptDataCentroid.x - m_ptCompCentroid.x;
      double tmpY   = m_ptDataCentroid.y - m_ptCompCentroid.y;
      double cosine = tmpX/hyp;
      double sine   = tmpY/hyp;

      hyp = radius - (featureSize/2);
      x =  m_ptDataCentroid.x + (hyp * cosine);
      y =  m_ptDataCentroid.y + (hyp * sine);
   }
   else
   {
      AccessOffsetDirectionTag accessOffsetDirection = accessOffsetDirectionUndefined;

      if (targetTypeIsTh && accessOffsetItem.getEnableThOffsetFlag())
      {
         const CPrioritizedAccessOffsetDirectionsList& directionsList = accessOffsetItem.getDirectionList();

         for (POSITION pos = directionsList.GetHeadPosition();pos != NULL;)
         {
            accessOffsetDirection = directionsList.GetNext(pos);

            if (accessOffsetDirection != accessOffsetDirectionUndefined)
            {
               break;
            }
         }
      }

      if (accessOffsetDirection == accessOffsetDirectionUndefined)
      {
         accessOffsetDirection = accessOffsetDirectionEast;
      }

      x = m_ptDataCentroid.x;
      y = m_ptDataCentroid.y;

      double delta = radius - (featureSize/2);

      switch (accessOffsetDirection)
      {
      case accessOffsetDirectionWest:   x -= delta;  break;
      case accessOffsetDirectionEast:   x += delta;  break;
      case accessOffsetDirectionNorth:  y += delta;  break;
      case accessOffsetDirectionSouth:  y -= delta;  break;
      }
   }

   return true;
}

bool CExposedData::findFarthestOffset(CCEtoODBDoc *doc, FileStruct *pPCBFile, double featureSize, double &x, double &y)
{
   x = 0.0;
   y = 0.0;

   bool res = false;
   ECorner dataCorner;

   // left
   if (m_ptDataCentroid.y < m_ptCompCentroid.y && m_ptDataCentroid.x < m_ptCompCentroid.x)
      dataCorner = cornerLL;
   else if (fabs(m_ptDataCentroid.y - m_ptCompCentroid.y) < SMALLNUMBER && m_ptDataCentroid.x < m_ptCompCentroid.x)
      dataCorner = cornerCL;
   else if (m_ptDataCentroid.y > m_ptCompCentroid.y && m_ptDataCentroid.x < m_ptCompCentroid.x)
      dataCorner = cornerUL;

   // center
   else if (m_ptDataCentroid.y < m_ptCompCentroid.y && fabs(m_ptDataCentroid.x - m_ptCompCentroid.x) < SMALLNUMBER)
      dataCorner = cornerLC;
   else if (fabs(m_ptDataCentroid.y - m_ptCompCentroid.y) < SMALLNUMBER && fabs(m_ptDataCentroid.x - m_ptCompCentroid.x) < SMALLNUMBER)
      dataCorner = cornerCC;
   else if (m_ptDataCentroid.y > m_ptCompCentroid.y && fabs(m_ptDataCentroid.x - m_ptCompCentroid.x) < SMALLNUMBER)
      dataCorner = cornerUC;

   // right
   else if (m_ptDataCentroid.y > m_ptCompCentroid.y && m_ptDataCentroid.x > m_ptCompCentroid.x)
      dataCorner = cornerUR;
   else if (fabs(m_ptDataCentroid.y - m_ptCompCentroid.y) < SMALLNUMBER && m_ptDataCentroid.x > m_ptCompCentroid.x)
      dataCorner = cornerCR;
   else if (m_ptDataCentroid.y < m_ptCompCentroid.y && m_ptDataCentroid.x > m_ptCompCentroid.x)
      dataCorner = cornerLR;

   if (dataCorner == cornerLL || dataCorner == cornerCL || dataCorner == cornerUL)
   {
      // Find the offset using the left side of the poly first
      // if cannot find it, then use the right side of the poly
      res = findOffsetInPolyList(doc, pPCBFile, true, featureSize, x, y);

      if (res == false)
         res = findOffsetInPolyList(doc, pPCBFile, false, featureSize, x, y);
   }
   else 
   {
      // Find the offset using the right side of the poly first
      // if cannot find it, then use the left side of the poly
      res = findOffsetInPolyList(doc, pPCBFile, false, featureSize, x, y);

      if (res == false)
         res = findOffsetInPolyList(doc, pPCBFile, true, featureSize, x, y);
   }

   return res;
}

bool CExposedData::findOffsetInPolyList(CCEtoODBDoc *doc, FileStruct *pPCBFile, bool useLeftSide, double featureSize, double &x, double &y)
{
   x = 0.0;
   y = 0.0;

   bool res = false;
   double distance = 0.0;
   double tmpX = 0.0;
   double tmpY = 0.0;
   double radius = featureSize/2;

   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));
   Region *exposeRegion = RegionFromPolylist(doc, m_pExposedData->getPolyList(), scaleFactor);

   //CPolyList *polyList = exposeRegion->BuildRectPolys(scaleFactor);
   CPolyList *polyList = exposeRegion->GetSmoothedPolys(scaleFactor);   

   //CPolyList *polyList = exposeRegion->GetPolys(scaleFactor);
   POSITION pos = polyList->GetHeadPosition();
   while (pos)
   {
      CPoly *poly = polyList->GetNext(pos);

      if (poly == NULL)
         continue;

      double tmpDistance = 0.0;

      if (useLeftSide == true)
      {
         tmpX = poly->getExtent().getUL().x + radius;
         tmpY = poly->getExtent().getUL().y + radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }

         tmpX = poly->getExtent().getUL().x + radius;
         tmpY = poly->getExtent().getUL().y - radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }

         tmpX = poly->getExtent().getLL().x + radius;
         tmpY = poly->getExtent().getLL().y + radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }

         tmpX = poly->getExtent().getLL().x + radius;
         tmpY = poly->getExtent().getLL().y - radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }
      }
      else
      {
         //tmpX = poly->getExtent().getUR().x - radius;
         //tmpY = poly->getExtent().getUR().y + radius;
         tmpX = poly->getExtent().getUR().x;
         tmpX -= radius;
         tmpY = poly->getExtent().getUR().y;
         tmpY += radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }

         //tmpX = poly->getExtent().getUR().x - radius;
         //tmpY = poly->getExtent().getUR().y - radius;
         tmpX = poly->getExtent().getUR().x;
         tmpX -= radius;
         tmpY = poly->getExtent().getUR().y;
         tmpY -= radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }

         //tmpX = poly->getExtent().getLR().x - radius;
         //tmpY = poly->getExtent().getLR().y + radius;
         tmpX = poly->getExtent().getLR().x;
         tmpX -= radius;
         tmpY = poly->getExtent().getLR().y;
         tmpY += radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }

         //tmpX = poly->getExtent().getLR().x - radius;
         //tmpY = poly->getExtent().getLR().y - radius;
         tmpX = poly->getExtent().getLR().x;
         tmpX -= radius;
         tmpY = poly->getExtent().getLR().y;
         tmpY -= radius;
         tmpDistance = m_ptCompCentroid.distance(tmpX, tmpY);

         if (tmpDistance > distance)
         {
            if (isFeatureInPoly(doc, pPCBFile, exposeRegion, radius, tmpX, tmpY) == true)
            {
               distance = tmpDistance;
               res = true;
               x = tmpX;
               y = tmpY;
            }
         }
      }
   }

   if (exposeRegion)
      delete exposeRegion;

   if (res == false)
   {
      distance = 0.0;
      x = 0.0;
      y = 0.0;
   }

   return res;
}

bool CExposedData::isFeatureInPoly(CCEtoODBDoc *doc, FileStruct *pPCBFile, Region *exposeRegion, double radius, double x, double y)
{
   bool res = false;
   if (exposeRegion == NULL)
      return res;

   // Get the layer for where teh feature size will be insert on
   int featureSizeLayer = Graph_Level(DFT_LAYER_FEATURE_SIZE, "", 0L);
   LayerStruct *layer = doc->getLayerAt(featureSizeLayer);
   layer->setVisible(false);

   // Temperary graph a circle to represent the feature size
   Graph_Block_On(pPCBFile->getBlock());
   DataStruct *featureData= Graph_Circle(featureSizeLayer, x, y, radius, 0L, 0, FALSE, FALSE);
   Graph_Block_Off();

   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));
   Region *featureRegion = RegionFromPolylist(doc, featureData->getPolyList(), scaleFactor);
   Region *intersectRegion = IntersectRegions(exposeRegion, featureRegion);

   if (intersectRegion)
   {
      if (AreRegionsEqual(intersectRegion, featureRegion))
         res = true;
   }

   delete featureRegion;

   // remove the temperary graphic circle
   //pPCBFile->getBlock()->RemoveDataFromList(doc, featureData);
   doc->removeDataFromDataList(*(pPCBFile->getBlock()),featureData);

   return res;
}

//-----------------------------------------------------------------------------
// CExposedDataMap
//-----------------------------------------------------------------------------
CExposedDataMap::CExposedDataMap(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
{
   this->RemoveAll();
}

CExposedDataMap::~CExposedDataMap()
{
   //POSITION pos = this->GetStartPosition();
   //while (pos)
   //{
   // CString key;
   // CExposedData *exposedData = NULL;
   // this->GetNextAssoc(pos, key, exposedData);
   // delete exposedData;
   //}
   //this->RemoveAll();
}

void CExposedDataMap::RemoveAll()
{
   POSITION pos = this->GetStartPosition();
   while (pos)
   {
      CString key;
      CExposedData *exposedData = NULL;
      this->GetNextAssoc(pos, key, exposedData);
      delete exposedData;
      exposedData = NULL;
   }
   CTypedPtrMap<CMapStringToPtr, CString, CExposedData*>::RemoveAll();
}

void CExposedDataMap::AddExposedViaData(int targetType, ETestSurface accessSurface, DataStruct *via)
{
   if (via == NULL || via->getInsert() == NULL)
      return;

   int entityNum = via->getEntityNumber();
   bool singlePin = false;

   CPoint2d compCentroid;
   if (via->getInsert()->getCentroidLocation(getCamCadData(), compCentroid) == false)
      compCentroid = via->getInsert()->getOrigin2d();
   CPoint2d dataCentroid = compCentroid;

   bool unloaded = false;
   Attrib *attrib = NULL;
   WORD loadedKey = getCamCadDoc().RegisterKeyWord(ATT_LOADED, 0, VT_STRING);
   if (via->getAttributes() && via->getAttributes()->Lookup(loadedKey, attrib))
   {
      if (!attrib->getStringValue().CompareNoCase("FALSE"))
         unloaded = true;
   }

   BlockStruct* viaGeometry = getCamCadDoc().getBlockAt(via->getInsert()->getBlockNumber());

   CExposedData *exposedData = new CExposedData(getCamCadDoc(),accessItemTypeVia,via,viaGeometry,
                                      compCentroid, dataCentroid, entityNum, targetType, singlePin, unloaded);
   CString key;
   key.Format("%d%s", entityNum, (accessSurface==testSurfaceTop)?"top":"bot");
   this->SetAt(key, exposedData);
}

void CExposedDataMap::AddExposedPinData(int targetType,AccessItemTypeTag accessItemType, ETestSurface accessSurface, CompPinStruct *compPin, DataStruct *compData)
{
   if (compData == NULL || compData->getInsert() == NULL || compPin == NULL)
      return;

   int entityNum = compPin->getEntityNumber();
   CPoint2d dataCentroid = compPin->getOrigin();

   CPoint2d compCentroid;
   if (compData->getInsert()->getCentroidLocation(getCamCadData(), compCentroid) == false)
      compCentroid = compData->getInsert()->getOrigin2d();

   BlockStruct *block = getCamCadDoc().getBlockAt(compData->getInsert()->getBlockNumber());
   bool singlePin = (block->getPinCount()>1)?false:true;

   bool unloaded = false;
   Attrib *attrib = NULL;
   WORD loadedKey = getCamCadDoc().RegisterKeyWord(ATT_LOADED, 0, VT_STRING);
   if (compData->getAttributes() && compData->getAttributes()->Lookup(loadedKey, attrib))
   {
      if (!attrib->getStringValue().CompareNoCase("FALSE"))
         unloaded = true;
   }

   BlockStruct* padStackGeometry = getCamCadDoc().getBlockAt(compPin->getPadstackBlockNumber());

   CExposedData *exposedData = new CExposedData(getCamCadDoc(),accessItemType,compData,
                                      padStackGeometry,compCentroid, dataCentroid, entityNum, targetType, singlePin, unloaded);
   CString key;
   key.Format("%d%s", entityNum, (accessSurface==testSurfaceTop)?"top":"bot");
   this->SetAt(key, exposedData);
}

void CExposedDataMap::ResetErrorMessageAndExposedData() //FileStruct &pcbFile)
{
   POSITION pos = this->GetStartPosition();
   while (pos)
   {
      CString key;
      CExposedData *exposedData = NULL;
      this->GetNextAssoc(pos, key, exposedData);

      exposedData->SetFailureRC(DFT_AA_RC_NO_METAL);
      exposedData->SetExposedData(NULL);
   }
}

//-----------------------------------------------------------------------------
// CAAAccessLocation
//-----------------------------------------------------------------------------
int CAAAccessLocation::m_nextId = 1;
CAAAccessLocation::CAAAccessLocation(const CEntity& featureEntity, CString refDes, int targetType, AccessItemTypeTag itemType,
                                     ETestSurface mountSurface, ETestSurface accessSurface, CString netName, CPoint2d location,
                                     PageUnitsTag pageUnits, Bool3Tag singlePin, Bool3Tag unloaded)
{
   m_id = m_nextId++;
   m_exposedDataId = 0;
   m_exposedMetalId = 0;

   m_featureEntity = featureEntity;
   m_bAccessable = true;   // initialize to true, but will set to false if not accessible during analysis  
   m_singlePin = singlePin;
   m_unloaded = unloaded;
   m_itemType = itemType;
   m_eMountSurface = mountSurface;
   m_eAccessSurface = accessSurface;
   m_ptFeatureLocation = location;
   m_ptAccessLocation.x = DBL_MAX;
   m_ptAccessLocation.y = DBL_MAX;
   m_sRefDes = refDes;
   m_surfaceSpecificRefdes.Format("%s_%s", refDes, accessSurface==testSurfaceTop?"Top":"Bot");
   m_sSortKey = generateSortKey(m_surfaceSpecificRefdes);
   m_sNetName = netName;
   //m_sName = "";  // Is already blank, newly constructed an' all
   m_dExposedMetalDiameter = 0.0;
   m_eTargetType = targetType;
   m_pageUnits = pageUnits;
}

CAAAccessLocation::CAAAccessLocation(ETestSurface accessSurface, CString accessName, CPoint2d location, CString netName)
{
   m_id = m_nextId++;
   m_exposedDataId = 0;
   m_exposedMetalId = 0;

   //m_featureEntity;// = NULL;
   m_bAccessable = true;   // initialize to true, but will set to false if not accessible during analysis  
   m_singlePin = boolUnknown;
   m_unloaded = boolUnknown;
   m_itemType = accessItemTypePin; // Is actually unknown type, but no enum for unknown.
   m_eMountSurface = testSurfaceUnset;
   m_eAccessSurface = accessSurface;
   m_ptFeatureLocation = CPoint2d(0,0);
   m_ptAccessLocation.x = location.x;
   m_ptAccessLocation.y = location.y;
   //m_sRefDes = "";
   //m_surfaceSpecificRefdes.Format("%s_%s", refDes, accessSurface==testSurfaceTop?"Top":"Bot");
   //m_sSortKey = generateSortKey(m_surfaceSpecificRefdes);
   m_sSortKey = accessName;
   m_sNetName = netName;
   m_sName = accessName;
   m_dExposedMetalDiameter = 0.0;
   m_eTargetType = DFT_TARGETTYPE_UNKNOWN;
   m_pageUnits = pageUnitsInches; //*rcf DFT_CR Not correct, probably need to support this for real.
}

CAAAccessLocation::CAAAccessLocation(const CAAAccessLocation& other)
{
   m_id = m_nextId++;
   m_exposedDataId = 0;

   m_featureEntity     = other.m_featureEntity;
   m_bAccessable       = other.m_bAccessable;
   m_singlePin         = other.m_singlePin;
   m_unloaded          = other.m_unloaded;
   m_itemType          = other.m_itemType;
   m_eMountSurface     = other.m_eMountSurface;
   m_eAccessSurface    = other.m_eAccessSurface;
   m_ptFeatureLocation = other.m_ptFeatureLocation;
   m_ptAccessLocation  = other.m_ptAccessLocation;
   m_sRefDes           = other.m_sRefDes;
   m_surfaceSpecificRefdes = other.m_surfaceSpecificRefdes;
   m_sSortKey          = other.m_sSortKey;
   m_sNetName          = other.m_sNetName;
   m_sName             = other.m_sName;
   m_dExposedMetalDiameter = other.m_dExposedMetalDiameter;
   m_eTargetType       = other.m_eTargetType;
   m_pageUnits         = other.m_pageUnits;

   m_reasonCodes.CloneRCList(other.m_reasonCodes);

}

CAAAccessLocation::~CAAAccessLocation()
{
}

void CAAAccessLocation::ResetAccessLocation()
{
   m_bAccessable = true;   // reset to true, but will set to false if not accessible during analysis  
   this->GetRCList().Reset();
   m_ptAccessLocation.x = DBL_MAX;
   m_ptAccessLocation.y = DBL_MAX;
   m_dExposedMetalDiameter = 0.0;
   m_sName.Empty();
}

CString CAAAccessLocation::GetErrorMessage(const char* separator)
{
   CString msg;

   POSITION pos = this->GetRCList().GetStartPosition();
   while (pos != NULL)
   {
      CAccessFailureReason *r;
      WORD key;
      this->GetRCList().GetNextAssoc(pos, key, r);
      CString msg1 = r->GetMsgStr();
      if (!msg.IsEmpty())
         msg += (CString)separator;
      msg += msg1;
   }

   return msg;
}

CString CAAAccessLocation::GetTargetTypeToString() const
{
   CString targetType;
   if (m_eTargetType == DFT_TARGETTYPE_UNKNOWN)
   {
      targetType = "Unknown";
   }
   else
   {
      if (m_eTargetType & DFT_TARGETTYPE_TESTATTRIBUTE)
         targetType += "TestAttribute, ";
      if (m_eTargetType & DFT_TARGETTYPE_VIA)
         targetType += "Via, ";
      if (m_eTargetType & DFT_TARGETTYPE_CONNECTOR)
         targetType += "Connector, ";
      if (m_eTargetType & DFT_TARGETTYPE_SMD)
         targetType += "SMD, ";
      if (m_eTargetType & DFT_TARGETTYPE_THRUHOLE)
         targetType += "THRU, ";
      if (m_eTargetType & DFT_TARGETTYPE_CADPADSTACK)
         targetType += "CADPadstack, ";   

      targetType.Delete(targetType.GetLength() - 2, 2);
   }
   return targetType;
}

void CAAAccessLocation::DumpToFile(CFormatStdioFile &file, int indent)
{
   CString accLoc;

   if (m_featureEntity.getEntityType() == entityTypeCompPin)
   {
      CompPinStruct* cp = m_featureEntity.getCompPin();
      accLoc.Format("CompPin %s [%d] at 0x%08x", cp->getPinRef(), cp->getEntityNumber(), this);
   }
   else if (m_featureEntity.getEntityType() == entityTypeData)
   {
      DataStruct* data = m_featureEntity.getData();
      accLoc.Format("Via %s [%d] at 0x%08x", data->getInsert()->getRefname(), data->getEntityNumber(), this);
   }

   file.WriteString("%*s::Access Location on %s::\n", indent, " ", accLoc);
   indent += 3;

   file.WriteString("%*sm_sRefDes = %s\n", indent, " ", m_sRefDes);
   file.WriteString("%*sfeatureType = %s\n", indent, " ", entityTypeToString(m_featureEntity.getEntityType()));
   file.WriteString("%*sfeature = 0x%08x\n", indent, " ", m_featureEntity.getEntity());

   if (m_featureEntity.getEntityType() == entityTypeCompPin)
   {
      CompPinStruct *compPin = m_featureEntity.getCompPin();
      file.WriteString("%*sentityNum = %d\n", indent, " ", compPin->getEntityNumber());
   }
   else
   {
      DataStruct* via = m_featureEntity.getData();
      file.WriteString("%*sentityNum = %d\n", indent, " ", via->getEntityNumber());
   }

   //file.WriteString("%*sm_eFeatureType = %s\n", indent, " ", (m_eFeatureType==entityTypeCompPin)?"CompPin":"Data (Via)");

   file.WriteString("%*sm_eMountSurface = %s\n", indent, " ", (m_eMountSurface==testSurfaceTop)?"Top":"Bottom");
   file.WriteString("%*sm_eAccessSurface = %s\n", indent, " ", (m_eAccessSurface==testSurfaceTop)?"Top":"Bottom");
   file.WriteString("%*sm_ptFeatureLocation = %f, %f\n", indent, " ", m_ptFeatureLocation.x, m_ptFeatureLocation.y);
   file.WriteString("%*sm_sNetName = %s\n", indent, " ", m_sNetName);
   file.WriteString("%*sm_bAccessable = %s\n", indent, " ", m_bAccessable?"True":"False");

   if (m_bAccessable)
      file.WriteString("%*sm_ptAccessLocation = %f, %f\n", indent, " ", m_ptAccessLocation.x, m_ptAccessLocation.y);
   else
      file.WriteString("%*sm_ptAccessLocation = NULL\n", indent, " ");

   CString targetType = GetTargetTypeToString();
   file.WriteString("%*sm_eTargetType = %s\n", indent, " ", targetType);

   file.WriteString("%*sm_sErrorMessage = %s\n", indent, " ", this->GetErrorMessage());
   file.WriteString("%*sm_dExposedMetalDiameter = %d\n", indent, " ", m_dExposedMetalDiameter);
   indent -= 3;
}

void CAAAccessLocation::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<AccessPoint");

   switch (m_eMountSurface)
   {
   case testSurfaceTop:
      writeFormat.writef(" MountSurface=\"Top\"");
      break;
   case testSurfaceBottom:
      writeFormat.writef(" MountSurface=\"Bottom\"");
      break;
   }

   switch (m_eAccessSurface)
   {
   case testSurfaceTop:
      writeFormat.writef(" AccessSurface=\"Top\"");
      break;
   case testSurfaceBottom:
      writeFormat.writef(" AccessSurface=\"Bottom\"");
      break;
   }  

   writeFormat.writef(" TargetType=\"%d\"", m_eTargetType);

   long entityNum = 0;
   if (m_featureEntity.getEntityType() == entityTypeCompPin)
      entityNum = m_featureEntity.getCompPin()->getEntityNumber();
   else if (m_featureEntity.getEntityType() == entityTypeData)
      entityNum = m_featureEntity.getData()->getEntityNumber();

   writeFormat.writef(" Entity=\"%d\"", entityNum);

   int decimals = GetDecimals(m_pageUnits);

   writeFormat.writef(" LargestDiameter=\"%.*f\"", decimals, m_dExposedMetalDiameter);
   writeFormat.writef(" Accessiblity=\"%s\"", m_bAccessable?"True":"False");

   if (m_bAccessable)
   {
      writeFormat.writef(" Name=\"%s\"", SwapSpecialCharacters(m_sName));
      writeFormat.writef(" X=\"%.*f\"", decimals, m_ptAccessLocation.x);
      writeFormat.writef(" Y=\"%.*f\"/>\n", decimals, m_ptAccessLocation.y);
   }
   else
   {
      if(this->GetRCList().GetCount())
      {
         writeFormat.writef(">\n");

         //Write Error Messages
         writeFormat.pushHeader("  ");
         WriteXML(writeFormat,this->GetRCList());
         writeFormat.popHeader();

         writeFormat.writef("</AccessPoint>\n");
      }
      else
         writeFormat.writef("/>\n");
   }
}

void CAAAccessLocation::WriteXML(CWriteFormat& writeFormat, CAccessFailureReasonList &ClnList)
{
   POSITION pos = this->GetRCList().GetStartPosition();
   while (pos != NULL)
   {
      WORD key;
      CAccessFailureReason *r;
      ClnList.GetNextAssoc(pos, key, r);     
      if (r)
      {
        writeFormat.writef("<ErrorMessage rc=\"%d\" param1=\"%s\" param2=\"%s\" dparam1=\"%f\" dparam2=\"%f\"/>\n",
          r->GetReasonCode(),SwapSpecialCharacters(r->GetParam1()),
          SwapSpecialCharacters(r->GetParam2()),r->GetDParam1(),r->GetDParam2()); 
      }         
   }
}

CString CAAAccessLocation::generateSortKey(CString name) const 
{ 
   return GenerateSortKey(name);                         
}

//-----------------------------------------------------------------------------
// CAAAccessLocationList
//-----------------------------------------------------------------------------
CAAAccessLocationList::CAAAccessLocationList()
	: CTypedMapSortStringToObContainer<CAAAccessLocation>(nextPrime2n(200), true)
{
}

CAAAccessLocationList::~CAAAccessLocationList()
{
}

void CAAAccessLocationList::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   for (POSITION pos = GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAAAccessLocation* accessLocation = NULL;
      GetNextAssoc(pos, key, accessLocation);

      if (accessLocation != NULL)
      {
         accessLocation->WriteXML(writeFormat, progress);
      }
   }
}

void CAAAccessLocationList::CopyAcccesLocationList(const CAAAccessLocationList &fromList)
{
   for (POSITION pos = fromList.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAAAccessLocation* accLoc = NULL;
      fromList.GetNextAssoc(pos, key, accLoc);
      if (accLoc != NULL)
      {
         CAAAccessLocation* newAccLoc = new CAAAccessLocation(*accLoc);
         this->SetAt(newAccLoc->GetSortKey(), newAccLoc);
      }
   }
}

void CAAAccessLocationList::RemoveAccessLocations()
{
   this->empty();
}


//-----------------------------------------------------------------------------
// CExposedMetal
//-----------------------------------------------------------------------------
int CExposedMetal::m_nextId = 1;
CExposedMetal::CExposedMetal(CCEtoODBDoc& doc, BlockStruct& padstackBlock, const ETestSurface surface,
                             AccessItemTypeTag itemType, CPoint2d compCentriod, CPoint2d dataCentriod, int targetType)
   : m_doc(doc)
   , m_padStackGeometry(padstackBlock)
   , m_surface(surface)
   , m_itemType(itemType)
{
   m_id = m_nextId++;

   m_ptCompCentroid = compCentriod;       
   m_ptDataCentroid = dataCentriod;       
   m_iTargetType = targetType;

   setMembersToNull();
}

CExposedMetal::~CExposedMetal()
{
   empty();
}

void CExposedMetal::empty()
{
   delete m_metalOriginal;
   delete m_metalByBoardOutline;
   delete m_metalByComponentOutline;
   delete m_metalByBoardComponentOutline;
   delete m_metalBySolder;
   delete m_metalBySolderBoardOutline;
   delete m_metalBySolderComponentOutline;
   delete m_metalBySolderBoardComponentOutline;

   setMembersToNull();
}

void CExposedMetal::setMembersToNull()
{
   m_pExposedData = NULL;

   m_metalOriginal = NULL;
   m_metalByBoardOutline = NULL;
   m_metalByComponentOutline = NULL;
   m_metalByBoardComponentOutline = NULL;
   m_metalBySolder = NULL;
   m_metalBySolderBoardOutline = NULL;
   m_metalBySolderComponentOutline = NULL;
   m_metalBySolderBoardComponentOutline = NULL;

   m_metalOriginalError.Reset();
   m_metalByBoardOutlineError.Reset();
   m_metalByComponentOutlineError.Reset();
   m_metalByBoardComponentOutineError.Reset();
   m_metalBySolderError.Reset();
   m_metalBySolderBoardOutlineError.Reset();
   m_metalBySolderComponentOutlineError.Reset();
   m_metalBySolderBoardComputerOutlineError.Reset();
}

ExposedMetalTypeTag CExposedMetal::getMetalTypeTage(CTestPlan& testPlan)
{
   // This flag is for finding out what metal type is needed
   // 1 - soldermask
   // 2 - boardoutline
   // 4 - componentoutline

   ExposedMetalTypeTag tag = exposedMetalOriginal;
   int flag = 0; 
   CPhysicalConst* physicalConst = m_surface==testSurfaceTop?testPlan.GetTopConstraints():testPlan.GetBotConstraints();
   CHeightAnalysis* heightAnalysis = m_surface==testSurfaceTop?testPlan.GetTopHeightAnalysis():testPlan.GetBotHeightAnalysis();

   if (testPlan.GetEnableSoldermaskAnalysis())     flag |= 1;
   if (physicalConst != NULL)
   {
      if (physicalConst->GetEnableBoardOutline())  flag |= 2;
      if (physicalConst->GetEnableCompOutline())   flag |= 4;
   }
   if (heightAnalysis != NULL)
   {
      if (heightAnalysis->IsUse())                 flag |= 4;
   }

   switch (flag)
   {
   case 0: tag = exposedMetalOriginal;                         break;
   case 1: tag = exposedMetalBySolder;                         break;
   case 2: tag = exposedMetalByBoardOutline;                   break;
   case 3: tag = exposedMetalBySolderBoardOutline;             break;
   case 4: tag = exposedMetalByComponentOutline;               break;
   case 5: tag = exposedMetalBySolderComponentOutline;         break;
   case 6: tag = exposedMetalByBoardComponentOutline;          break;
   case 7: tag = exposedMetalBySolderBoardComponentOutline;    break;
   }

   return tag;
}

void CExposedMetal::resetExposedMetal(ExposedMetalTypeTag exposedMetalType)
{
   switch (exposedMetalType)
   {
   case exposedMetalOriginal:
      delete m_metalOriginal;
      m_metalOriginal = NULL;
      m_metalOriginalError.Reset();
      break;

   case exposedMetalByBoardOutline:
      delete m_metalByBoardOutline;
      m_metalByBoardOutline = NULL;
      m_metalByBoardOutlineError.Reset();
      break;

   case exposedMetalByComponentOutline:
      delete m_metalByComponentOutline;
      m_metalByComponentOutline = NULL;
      m_metalByComponentOutlineError.Reset();
      break;

   case exposedMetalByBoardComponentOutline:
      delete m_metalByBoardComponentOutline;
      m_metalByBoardComponentOutline = NULL;
      m_metalByBoardComponentOutineError.Reset();
      break;

   case exposedMetalBySolder:                         
      delete m_metalBySolder;
      m_metalBySolder = NULL;
      m_metalBySolderError.Reset();
      break;

   case exposedMetalBySolderBoardOutline:
      delete m_metalBySolderBoardOutline;
      m_metalBySolderBoardOutline = NULL;
      m_metalBySolderBoardOutlineError.Reset();
      break;

   case exposedMetalBySolderComponentOutline:
      delete m_metalBySolderComponentOutline;
      m_metalBySolderComponentOutline = NULL;
      m_metalBySolderComponentOutlineError.Reset();
      break;

   case exposedMetalBySolderBoardComponentOutline:
      delete m_metalBySolderBoardComponentOutline;
      m_metalBySolderBoardComponentOutline = NULL;
      m_metalBySolderBoardComputerOutlineError.Reset();
      break;
   }
}

bool CExposedMetal::isAccessible(CMetalAnalysis& metalAnalysis, CAAAccessLocation& accessLocation, CTestPlan& testPlan, const CAccessOffsetOptions& accessOffsetOptions,
                                 FileStruct& pcbFile, const double tolerance)
{
   bool accessible = false;
   bool isBeadProbeAttrPresent = false;
   ExposedMetalTypeTag exposedMetalType = getMetalTypeTage(testPlan);

   int mirrorFlag = 0;
   if (pcbFile.isMirrored())
		mirrorFlag = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic

   long featureEntityNumber = -1;
   CTMatrix matrix;
   if (accessLocation.getFeatureEntity().getEntityType() == entityTypeCompPin)
   {
      CompPinStruct* compPin = accessLocation.getFeatureEntity().getCompPin();
      if (compPin != NULL)
      {
         featureEntityNumber = compPin->getEntityNumber();
         mirrorFlag |= compPin->getMirror();
       
         matrix.translateCtm(compPin->getOrigin());
         matrix.rotateRadiansCtm(compPin->getRotationRadians());
         matrix.scaleCtm((compPin->getMirror() & MIRROR_FLIP)!=0?-1:1, 1);
      }
   }
   else if (accessLocation.getFeatureEntity().getEntityType() == entityTypeData)
   {
      DataStruct* data = accessLocation.getFeatureEntity().getData();
      if (data != NULL && data->getInsert() != NULL)
      {
         featureEntityNumber = data->getEntityNumber();
         mirrorFlag |= data->getInsert()->getMirrorFlags();
         matrix = data->getInsert()->getTMatrix();
      }
   }

   if ((accessLocation.getFeatureEntity().getEntityType() == entityTypeCompPin)
      ||(accessLocation.getFeatureEntity().getEntityType() == entityTypeData))
   {
      int BeadProbeKeywordIndex = -1;
      BeadProbeKeywordIndex = m_doc.IsKeyWord("BEAD_PROBE",0);
      Attrib *attrib = NULL;
      DataStruct *data = NULL;
      if(accessLocation.getFeatureEntity().getEntityType() == entityTypeCompPin)
      {
         CompPinStruct* compPin = accessLocation.getFeatureEntity().getCompPin();
         data = datastruct_from_refdes(&m_doc, pcbFile.getBlock(), compPin->getRefDes());
         if(!compPin->getRefDes().CompareNoCase("ICT158"))
            int i = 0;
      }
      else if(accessLocation.getFeatureEntity().getEntityType() == entityTypeData)
      {
         data = accessLocation.getFeatureEntity().getData();
      }

      if ((NULL != data) 
         && data->getAttributesRef() 
         && (NULL != data->getInsert())
         && (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT 
             || data->getInsert()->getInsertType() == INSERTTYPE_VIA
             || data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
         && (-1 != BeadProbeKeywordIndex)
         && data->getAttributesRef()->Lookup(BeadProbeKeywordIndex, attrib))
      {
               if (NULL != attrib)
               {
                  isBeadProbeAttrPresent = true;
               }
      }
   }

   bool collectAllRCs = testPlan.GetMultipleReasonCodes();
   CAccessFailureReasonList errorMessages;
   m_pExposedData = getExposedMetalData(metalAnalysis, matrix, mirrorFlag, exposedMetalType, errorMessages, collectAllRCs);
   accessLocation.GetRCList().CopyRCList(errorMessages);

   if (m_pExposedData != NULL && m_pExposedData->getDataType() == dataTypePoly)
   {
      double metalDiameter = 0.0;
      double x = 0.0;
      double y = 0.0;
      double featureSizes[MinProbeSizeLast];     

      //Feature Size
      featureSizes[MinProbeSizeFeature] = 0.0;
      if (m_surface==testSurfaceTop && testPlan.GetTopConstraints()->GetEnableMinFeatureSize())
      {
         featureSizes[MinProbeSizeFeature] = testPlan.GetTopConstraints()->GetValueMinFeatureSize();
      }
      else if (m_surface == testSurfaceBottom && testPlan.GetBotConstraints()->GetEnableMinFeatureSize())
      {
         featureSizes[MinProbeSizeFeature] = testPlan.GetBotConstraints()->GetValueMinFeatureSize();
      }

      //BeadProbe Size
      featureSizes[MinProbeSizeBeadProbe] = 0.0;
      if(isBeadProbeAttrPresent)
      {
         if (m_surface==testSurfaceTop && testPlan.GetTopConstraints()->GetEnableBeadProbeSize())
         {
            featureSizes[MinProbeSizeBeadProbe] = testPlan.GetTopConstraints()->GetValueBeadProbeSize();
         }
         else if (m_surface == testSurfaceBottom && testPlan.GetBotConstraints()->GetEnableBeadProbeSize())
         {
            featureSizes[MinProbeSizeBeadProbe] = testPlan.GetBotConstraints()->GetValueBeadProbeSize();
         }     
      }

      bool isEnableMinFeatSz = (m_surface==testSurfaceTop)?testPlan.GetTopConstraints()->GetEnableMinFeatureSize():testPlan.GetBotConstraints()->GetEnableMinFeatureSize();
      bool isEnableBeadProbe = (m_surface==testSurfaceTop)?testPlan.GetTopConstraints()->GetEnableBeadProbeSize():testPlan.GetBotConstraints()->GetEnableBeadProbeSize();
      double inCircleTolerance = m_doc.convertToPageUnits(pageUnitsMils,.5);
      double featureSizeTolerance = m_doc.convertToPageUnits(pageUnitsMils,0.1);
      bool offsetAccessAllowedFlag = accessOffsetOptions.isOffsetAccessAllowed(getItemType(), accessLocation.GetTargetType());

#ifdef DebugAccessAnalysisOffset
      // debug
      CPoly* debugPoly = getExposedPoly();

      if (debugPoly != NULL)
      {
         CPolygon debugPolygon(*debugPoly);
         DataStruct* debugPolygonData = CDebug::addDebugPolygon(doc,debugPolygon,"db isAccessible() - %s %s %s",
            accessItemTypeTagToString(getItemType()),
            isSmd() ? "smd" : (isThruHole() ? "th" : "?"),
            offsetAccessAllowedFlag ? "allowed" : "disallowed");

         WORD exposedDataEntityNumberKeyword = getCamCadDoc().RegisterKeyWord("ExposedDataEntityNumber", 0, VT_INTEGER);
         getCamCadDoc().SetAttrib(&(debugPolygonData->getDefinedAttributes()),exposedDataEntityNumberKeyword,m_pExposedData->getEntityNumber());

         WORD dataEntityNumberKeyword = getCamCadDoc().RegisterKeyWord("DataEntityNumber", 0, VT_INTEGER);
         getCamCadDoc()SetAttrib(&(debugPolygonData->getDefinedAttributes()),dataEntityNumberKeyword, featureEntityNumber);
      }
#endif

      int res = true;
      bool exposedMetalTooSmallFlag = false;
      double featureSize = 0.0;
      int assignedProbeSize = MinProbeSizeFeature;

      for(int probeIdx = MinProbeSizeFeature; probeIdx < MinProbeSizeLast; probeIdx++)
      {
         // if MFS Pass -> Pass
         // if MFS Fail, MFS checked, BeadProbeAttr exists and MBPS checked, test BeadProbe Size
         // if MFS Fail, BeadProbeAttr does not exist or MFS not checked or MBPS not checked -> Fail
         if( probeIdx == MinProbeSizeFeature || (isEnableMinFeatSz && isBeadProbeAttrPresent && isEnableBeadProbe && 
            (!res || exposedMetalTooSmallFlag)))
         {
            featureSize = featureSizes[probeIdx];
            assignedProbeSize = probeIdx;

            res = (offsetAccessAllowedFlag)?calculateOffsetLocation(&m_doc, &pcbFile, accessOffsetOptions, featureSize, x, y, metalDiameter,inCircleTolerance)
               :calculateLocationAndMaxSize(featureSize, x, y, metalDiameter, inCircleTolerance);
            exposedMetalTooSmallFlag = ((featureSize - metalDiameter) > featureSizeTolerance);  
         }
      }
      

      if (!res || exposedMetalTooSmallFlag)
      {
         // Even if res is true, it could be that calculateLocationAndMaxSize() return okay
         // because the centroid is inside the exposed metal but the metal diameter is smaller than featureSize
         if(assignedProbeSize == MinProbeSizeBeadProbe)
            accessLocation.GetRCList().AddRC(DFT_AA_RC_SMALL_METAL_BEAD_PROBE, metalDiameter, featureSize);
         else
            accessLocation.GetRCList().AddRC(DFT_AA_RC_SMALL_METAL, metalDiameter, featureSize);         
            
         accessLocation.SetAccessible(false);
         accessible = false;
      }
      else
      {  
         accessLocation.SetAccessLocation(x, y);
         accessLocation.SetExposedMetalDiameter(metalDiameter);

         //*rcf Potentially could make an UpdateAccessible(bool) func that takes true or false, but
         //will only update to true if there are no fatal access errors. Or maybe better yet, just
         // get rid of accessible as a member var and use the query on fatal access error codes
         // to determine if accessible.
         if (!accessLocation.GetRCList().ContainsFatalAccessError())
         {
            accessLocation.SetAccessible(true);
            accessible = true;
         }
         else
         {
            if(assignedProbeSize == MinProbeSizeBeadProbe)
               accessLocation.GetRCList().AddRC(DFT_AA_RC_SMALL_METAL_BEAD_PROBE, metalDiameter, featureSize);               
            else
               accessLocation.GetRCList().AddRC(DFT_AA_RC_SMALL_METAL, metalDiameter, featureSize);               
            accessLocation.SetAccessible(false);
            accessible = false;
         }
      }
   }
   else
   {
      if(isBeadProbeAttrPresent)
         accessLocation.GetRCList().AddRC(DFT_AA_RC_SMALL_METAL_BEAD_PROBE);         
      else
         accessLocation.GetRCList().AddRC(DFT_AA_RC_SMALL_METAL);
                  
      accessLocation.SetAccessible(false);
      accessible = false;
   }

   if (m_pExposedData != NULL)
   {
      // Added feature entity onto exposed metal as attribute "DATALINK"
      int dataLinkKw = m_doc.getStandardAttributeKeywordIndex(standardAttributeDataLink);
      m_pExposedData->setAttrib(getCamCadData(), dataLinkKw, valueTypeInteger, &featureEntityNumber, attributeUpdateOverwrite, NULL);
   }

   return accessible;
}

DataStruct* CExposedMetal::getExposedMetalData(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, 
                                               const ExposedMetalTypeTag exposedMetalType, CAccessFailureReasonList& errorMessages, bool collectAllRCs)
{
   DataStruct* exposedData = NULL;
   errorMessages.Reset();
   switch (exposedMetalType)
   {
   case exposedMetalOriginal:                         
      exposedData = getMetalOriginal(matrix, mirrorFlag, collectAllRCs);                                      
      errorMessages.CopyRCList(m_metalOriginalError);
      break;

   case exposedMetalByBoardOutline:                   
      exposedData = getMetalByBoardOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);                 
      errorMessages.CopyRCList(m_metalByBoardOutlineError);
      break;

   case exposedMetalByComponentOutline:               
      exposedData = getMetalByComponentOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);             
      errorMessages.CopyRCList(m_metalByComponentOutlineError);
      break;

   case exposedMetalByBoardComponentOutline:          
      exposedData = getMetalByBoardComponentOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);        
      errorMessages.CopyRCList(m_metalByBoardComponentOutineError);
      break;

   case exposedMetalBySolder:                         
      exposedData = getMetalSolder(metalAnalysis, matrix, mirrorFlag, collectAllRCs);                         
      errorMessages.CopyRCList(m_metalBySolderError);
      break;

   case exposedMetalBySolderBoardOutline:             
      exposedData = getMetalBySolderBoardOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);           
      errorMessages.CopyRCList(m_metalBySolderBoardOutlineError);
      break;

   case exposedMetalBySolderComponentOutline:         
      exposedData = getMetalBySolderComponentOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);       
      errorMessages.CopyRCList(m_metalBySolderComponentOutlineError);
      break;

   case exposedMetalBySolderBoardComponentOutline:    
      exposedData = getMetalBySolderBoardComponentOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);  
      errorMessages.CopyRCList(m_metalBySolderBoardComputerOutlineError);
      break;
   }

   return exposedData;
}

DataStruct* CExposedMetal::getMetalOriginal(const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalOriginal == NULL)
   {
      // Extra electrical data from padstack, using layerTypePadTop to represent all electrial top layers
      // and layerTypePadBottom to represent all electrical bottom layers
      m_metalOriginal = extraLayerFromPadstack(m_doc, m_padStackGeometry, matrix, m_surface==testSurfaceTop?layerTypePadTop:layerTypePadBottom, -1, mirrorFlag);
      if (m_metalOriginal == NULL)
      {
         m_metalOriginalError.AddRC(DFT_AA_RC_NO_METAL);
      }
   }

   return m_metalOriginal;
}

DataStruct* CExposedMetal::getMetalByBoardOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalByBoardOutline == NULL && m_metalByBoardOutlineError.IsEmpty()) 
   {
      // The metal has have been exposed by board outline so do it
      DataStruct* originalMetal = getMetalOriginal(matrix, mirrorFlag, collectAllRCs);
      if (originalMetal != NULL)
      {
         m_metalByBoardOutline = getCamCadData().getNewDataStruct(*originalMetal);
         if (m_metalByBoardOutline != NULL)
         {
            if (!metalAnalysis.analysisBoardOutline(*m_metalByBoardOutline, m_surface, m_metalByBoardOutlineError))
            {
               if (!collectAllRCs) // keep this in separate if to guarantee analysisBoardOutline runs
               {
                  delete m_metalByBoardOutline;
                  m_metalByBoardOutline = NULL;
               }
            }
         }
      }
      
      // Always collect the error, if not error we spin some time, no harm other than that
      m_metalByBoardOutlineError.CopyRCList(m_metalOriginalError);
   }

   return m_metalByBoardOutline;
}

DataStruct* CExposedMetal::getMetalByComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalByComponentOutline == NULL && m_metalByComponentOutlineError.IsEmpty())
   {
      DataStruct* originalMetal = getMetalOriginal(matrix, mirrorFlag, collectAllRCs);
      if (originalMetal != NULL)
      {
         m_metalByComponentOutline = getCamCadData().getNewDataStruct(*originalMetal);
         if (m_metalByComponentOutline != NULL)
         {
            if (!metalAnalysis.analysisComponentOutline(*m_metalByComponentOutline, m_surface, m_metalByComponentOutlineError))
            {
               if (!collectAllRCs)  // keep this in separate if to guarantee analysisComponentOutline runs
               {
                  delete m_metalByComponentOutline;
                  m_metalByComponentOutline = NULL;
               }
            }
         }
      }
         
      m_metalByComponentOutlineError.CopyRCList(m_metalOriginalError);
   }

   return m_metalByComponentOutline;
}

DataStruct* CExposedMetal::getMetalByBoardComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalByBoardComponentOutline == NULL && m_metalByBoardComponentOutineError.IsEmpty())
   {
      DataStruct* metalByBoardOutline = getMetalByBoardOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);
      if (metalByBoardOutline != NULL)
      {
         m_metalByBoardComponentOutline = getCamCadData().getNewDataStruct(*metalByBoardOutline);
         if (m_metalByBoardComponentOutline != NULL)
         {
            if (!metalAnalysis.analysisComponentOutline(*m_metalByBoardComponentOutline, m_surface, m_metalByBoardComponentOutineError))
            {
               if (!collectAllRCs) // keep this in separate if, to guarantee analysisComponentOutline runs
               {
                  delete m_metalByBoardComponentOutline;
                  m_metalByBoardComponentOutline = NULL;
               }
            }
         }
      }
         
      m_metalByBoardComponentOutineError.CopyRCList(m_metalByBoardOutlineError);
   }

   return m_metalByBoardComponentOutline;
}

DataStruct* CExposedMetal::getMetalSolder(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalBySolder == NULL && m_metalBySolderError.IsEmpty())
   {
      DataStruct* metalByOriginal = getMetalOriginal(matrix, mirrorFlag, collectAllRCs);
      if (metalByOriginal != NULL)
      {
         // Extra soldermask from padstack
         DataStruct* solderData = extraLayerFromPadstack(m_doc, m_padStackGeometry, matrix, m_surface==testSurfaceTop?layerTypeMaskTop:layerTypeMaskBottom, -1, mirrorFlag);

         // Expose metal by soldermask in padstack
         DataStruct* metalByPadstackSolder = NULL;
         if (solderData != NULL)
         {
            if (solderData->getPolyList() != NULL)
            {
               // Intersect metal data with solder mask in padstacks
               metalByPadstackSolder = getCamCadData().getNewDataStruct(*metalByOriginal);

               CSMList padstackSolderList;
               padstackSolderList.AddTail(getERPStruct("Solder Mask", solderData->getPolyList(), m_doc));

               getMetalExposedRegion(m_doc, *metalByPadstackSolder, padstackSolderList);
            }

            delete solderData;
            solderData = NULL;
         }

         // Expose metal by floating soldermask at board level
         DataStruct* metalByFloatingSolder = getCamCadData().getNewDataStruct(*metalByOriginal);
         if (metalByFloatingSolder != NULL)
         {
            if (!metalAnalysis.analysisSoldermask(*metalByFloatingSolder, m_surface))
            {
               delete metalByFloatingSolder;
               metalByFloatingSolder = NULL;
            }         
         }

         if (metalByPadstackSolder != NULL && metalByFloatingSolder != NULL)
         {
            // If there are metals exposed by both padstack and floating soldermask, then combine them
            m_metalBySolder = getCombinedRegion(m_doc, *metalByPadstackSolder, *metalByFloatingSolder);

            delete metalByPadstackSolder;
            delete metalByFloatingSolder;
            metalByPadstackSolder = NULL;
            metalByFloatingSolder = NULL;
         }
         else if (metalByPadstackSolder != NULL)
         {
            // Metal only exposed by padstack soldermask
            m_metalBySolder = metalByPadstackSolder;
         }
         else if (metalByFloatingSolder !=  NULL)
         {
            // Metal only exposed by floating soldermask
            m_metalBySolder = metalByFloatingSolder;
         }
         else
         {
            // No metal is exposed by soldermask
            m_metalBySolderError.AddRC(DFT_AA_RC_NO_SOLDERMASK);
            
            // If we want to collect more error messages then we'll need some copper to examine
            if (collectAllRCs)
               m_metalBySolder = getCamCadData().getNewDataStruct(*metalByOriginal);
         }
      }
      else
      {
         m_metalBySolderError.CopyRCList(m_metalOriginalError);
      }
   }

   return m_metalBySolder;
}

DataStruct* CExposedMetal::getMetalBySolderBoardOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalBySolderBoardOutline == NULL && m_metalBySolderBoardOutlineError.IsEmpty())
   {
      DataStruct* metalBySolder = getMetalSolder(metalAnalysis, matrix, mirrorFlag, collectAllRCs);
      if (metalBySolder != NULL)
      {
         m_metalBySolderBoardOutline = getCamCadData().getNewDataStruct(*metalBySolder);
         if (m_metalBySolderBoardOutline != NULL)
         {
            if (!metalAnalysis.analysisBoardOutline(*m_metalBySolderBoardOutline, m_surface, m_metalBySolderBoardOutlineError))
            {
               if (!collectAllRCs)  // keep this in separate if, to guarantee analysisBoardOutline runs
               {
                  delete m_metalBySolderBoardOutline;
                  m_metalBySolderBoardOutline = NULL;
               }
            }
         }
      }

      m_metalBySolderBoardOutlineError.CopyRCList(m_metalBySolderError);
   }

   return m_metalBySolderBoardOutline;
}

DataStruct* CExposedMetal::getMetalBySolderComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalBySolderComponentOutline == NULL && m_metalBySolderComponentOutlineError.IsEmpty())
   {
      DataStruct* metalBySolder = getMetalSolder(metalAnalysis, matrix, mirrorFlag, collectAllRCs);
      if (metalBySolder != NULL)
      {
         m_metalBySolderComponentOutline = getCamCadData().getNewDataStruct(*metalBySolder);
         if (m_metalBySolderComponentOutline != NULL)
         {
            if (!metalAnalysis.analysisComponentOutline(*m_metalBySolderComponentOutline, m_surface, m_metalBySolderComponentOutlineError))
            {
               if (!collectAllRCs)  // keep this in separate if, to guarantee analysisComponentOutline runs
               {
                  delete m_metalBySolderComponentOutline;
                  m_metalBySolderComponentOutline = NULL;
               }
            }
         }
      }

      m_metalBySolderComponentOutlineError.CopyRCList(m_metalBySolderError);
   }

   return m_metalBySolderComponentOutline;
}

DataStruct* CExposedMetal::getMetalBySolderBoardComponentOutline(CMetalAnalysis& metalAnalysis, const CTMatrix matrix, const int mirrorFlag, bool collectAllRCs)
{
   if (m_metalBySolderBoardComponentOutline == NULL && m_metalBySolderBoardComputerOutlineError.IsEmpty())
   {
      DataStruct* metalBySolderBoardOutline = getMetalBySolderBoardOutline(metalAnalysis, matrix, mirrorFlag, collectAllRCs);
      if (metalBySolderBoardOutline)
      {
         m_metalBySolderBoardComponentOutline = getCamCadData().getNewDataStruct(*metalBySolderBoardOutline);
         if (m_metalBySolderBoardComponentOutline != NULL)
         {
            if (!metalAnalysis.analysisComponentOutline(*m_metalBySolderBoardComponentOutline, m_surface, m_metalBySolderBoardComputerOutlineError))
            {
               if (!collectAllRCs)  // keep this in separate if, to guarantee analysisComponentOutline runs
               {
                  delete m_metalBySolderBoardComponentOutline;
                  m_metalBySolderBoardComponentOutline = NULL;
               }
            }
         }
      }
         
      m_metalBySolderBoardComputerOutlineError.CopyRCList(m_metalBySolderBoardOutlineError);
   }

   return m_metalBySolderBoardComponentOutline;
}

//-----------------------------------------------------------------------------
// CMetalAnalysis
//-----------------------------------------------------------------------------
CMetalAnalysis::CMetalAnalysis(CCEtoODBDoc& doc, FileStruct& pcbFile, CTestPlan& testPlan)
   : m_doc(doc)
   , m_pcbFile(pcbFile)
   , m_testPlan(testPlan)
{
   empty();
}

CMetalAnalysis::~CMetalAnalysis()
{
   empty();
}

void CMetalAnalysis::empty()
{
   emptyList(m_soldermaskTopList);
   emptyList(m_soldermaskBottomList);
   emptyList(m_boardOutlineTopList);
   emptyList(m_boardOutlineBottomList);
   emptyList(m_componentOutlineTopList);
   emptyList(m_componentOutlineBottomList);

   m_soldermaskTopListDone = false;
   m_soldermaskBottomListDone = false;
   m_boardOutlineTopListDone = false;
   m_boardOutlineBottomListDone = false;
   m_componentOutlineTopListDone = false;
   m_componentOutlineBottomListDone = false;

   m_boardOutlineTopValue = 0.0;
   m_boardOutlineBottomValue = 0.0;
   m_componentOutlineTopValue = 0.0;
   m_componentOutlineBottomValue = 0.0;
}

void CMetalAnalysis::emptyList(CSMList& smList, const bool removePolyList)
{
   for (POSITION pos = smList.GetHeadPosition(); pos != NULL;)
   {
      ERPStruct* erpStruct = smList.GetNext(pos);
      if (erpStruct != NULL)
      {
         delete erpStruct->region;
         erpStruct->region = NULL;

         if (removePolyList)
            delete erpStruct->pPolyList;
         erpStruct->pPolyList = NULL;

         delete erpStruct;
         erpStruct = NULL;
      }
   }
   smList.RemoveAll();
}

int CMetalAnalysis::addExposedMetal(BlockStruct& padstackBlock, const ETestSurface surface, AccessItemTypeTag itemType, CPoint2d compCentroid, CPoint2d dataCentroid, int targetType)
{
   CExposedMetal* exposedMetal = new CExposedMetal(m_doc, padstackBlock, surface, itemType, compCentroid, dataCentroid, targetType);
   m_exposedMetalMap.SetAt(exposedMetal->getId(), exposedMetal);

   return exposedMetal->getId();
}

bool CMetalAnalysis::getAccessibleMetal(CAAAccessLocation& accessLocation, CTestPlan& testPlan, const CAccessOffsetOptions& accessOffsetOptions,
                                        CCEtoODBDoc& pDoc, FileStruct& pFile, const double tolerance)
{
   bool accessible = false;
   CExposedMetal* exposedMetal = getExposedMetal(accessLocation.getExposedMetalId());
   if (exposedMetal != NULL)
   {
      accessible = exposedMetal->isAccessible(*this, accessLocation, testPlan, accessOffsetOptions, pFile, tolerance);
   }

   return accessible;
}

CExposedMetal* CMetalAnalysis::getExposedMetal(const int id)
{
   CExposedMetal* exposedMetal = NULL;
   m_exposedMetalMap.Lookup(id, exposedMetal);
   return exposedMetal;
}

void CMetalAnalysis::prepareForAnalysis()
{
   bool topBoardOutlineModified = isBoardOutlineModified(testSurfaceTop);
   bool bottomBoardOutlineModified = isBoardOutlineModified(testSurfaceBottom);
   bool topComponentOutlineModified = isComponentOutlineModified(testSurfaceTop);
   bool bottomComponentOutlineModified = isComponentOutlineModified(testSurfaceBottom);

   bool somethingIsModified = topBoardOutlineModified || bottomBoardOutlineModified || topComponentOutlineModified || bottomComponentOutlineModified;

   if (somethingIsModified)
   {
      for (POSITION pos = m_exposedMetalMap.GetStartPosition(); pos != NULL;)
      {
         int key;
         CExposedMetal* exposedMetal = NULL;
         m_exposedMetalMap.GetNextAssoc(pos, key, exposedMetal);
         if (exposedMetal != NULL)
         {
            if ((exposedMetal->getSurface() == testSurfaceTop && topBoardOutlineModified) ||
                (exposedMetal->getSurface() == testSurfaceBottom && bottomBoardOutlineModified))
            {
               exposedMetal->resetExposedMetal(exposedMetalByBoardOutline);
               exposedMetal->resetExposedMetal(exposedMetalBySolderBoardOutline);
            }

            if ((exposedMetal->getSurface() == testSurfaceTop && topComponentOutlineModified) ||
                (exposedMetal->getSurface() == testSurfaceBottom && bottomComponentOutlineModified))
            {
               exposedMetal->resetExposedMetal(exposedMetalByComponentOutline);
               exposedMetal->resetExposedMetal(exposedMetalBySolderComponentOutline);
            }

            if ((exposedMetal->getSurface() == testSurfaceTop && (topBoardOutlineModified  || topComponentOutlineModified)) ||
                (exposedMetal->getSurface() == testSurfaceBottom && (bottomBoardOutlineModified || bottomComponentOutlineModified)))
            {
               exposedMetal->resetExposedMetal(exposedMetalByBoardComponentOutline);
               exposedMetal->resetExposedMetal(exposedMetalBySolderBoardComponentOutline);
            }
         }
      }
   }
}

bool CMetalAnalysis::isBoardOutlineModified(const ETestSurface surface)
{
   bool retval = false;
   CPhysicalConst* physicalConst = surface==testSurfaceTop?m_testPlan.GetTopConstraints():m_testPlan.GetBotConstraints();

   if (physicalConst != NULL)
   {
      if (surface == testSurfaceTop && physicalConst->GetValueBoardOutline() != m_boardOutlineTopValue)
      {
         m_boardOutlineTopValue = physicalConst->GetValueBoardOutline();
         m_boardOutlineTopListDone = false;
         retval = true;
      }
      else if (surface == testSurfaceBottom && physicalConst->GetValueBoardOutline() !=  m_boardOutlineBottomValue)
      {
         m_boardOutlineBottomValue = physicalConst->GetValueBoardOutline();
         m_boardOutlineBottomListDone = false;
         retval = true;
      }
   }

   return retval;
}

bool CMetalAnalysis::isComponentOutlineModified(const ETestSurface surface)
{
   bool retval = false;
   CPhysicalConst* physicalConst = surface==testSurfaceTop?m_testPlan.GetTopConstraints():m_testPlan.GetBotConstraints();
   CHeightAnalysis* heightAnalysis = surface==testSurfaceTop?m_testPlan.GetTopHeightAnalysis():m_testPlan.GetBotHeightAnalysis();

   if (physicalConst != NULL)
   {
      if (surface == testSurfaceTop) 
      {
         if (physicalConst->GetValueCompOutline() !=  m_componentOutlineTopValue ||
             heightAnalysis->IsModified() || m_testPlan.GetPackageOutlineModified())
         {
            m_componentOutlineTopValue = physicalConst->GetValueCompOutline();        
            m_componentOutlineTopListDone = false;
            retval = true;
         }
      }
      else if (surface == testSurfaceBottom)
      {
         if (physicalConst->GetValueCompOutline() !=  m_componentOutlineBottomValue ||
             heightAnalysis->IsModified() || m_testPlan.GetPackageOutlineModified() )
         {
            m_componentOutlineBottomValue = physicalConst->GetValueCompOutline();
            m_componentOutlineBottomListDone = false;
            retval = true;
         }
      }
   }

   return retval;
}

bool CMetalAnalysis::analysisSoldermask(DataStruct& metal, ETestSurface analysisSurface)
{
   bool retval = false;
   if (m_testPlan.GetEnableSoldermaskAnalysis())
   {
      if (analysisSurface == testSurfaceTop)
      {
         if (!m_soldermaskTopListDone)
         {
            emptyList(m_soldermaskTopList);

            for (POSITION pos = m_doc.getFileList().GetHeadPosition(); pos != NULL;)
            {
               FileStruct* file = m_doc.getFileList().GetNext(pos);
               if (file != NULL && file->isShown() && file->getBlockType() != blockTypePanel && file->getBlock() != NULL)
               {
                  int mirrorFlag = 0;
                  if (file->isMirrored())
		               mirrorFlag = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic

                  CTMatrix transformMatrix = file->getTMatrix();
                  extraSoldermask(m_doc, *file->getBlock(), m_soldermaskTopList, transformMatrix, layerTypeMaskTop, -1, mirrorFlag);
               }
            }

            m_soldermaskTopListDone = true;
         }

         retval = (getMetalExposedRegion(m_doc, metal, m_soldermaskTopList) != -1);

      }
      else if (analysisSurface == testSurfaceBottom)
      {
         if (!m_soldermaskBottomListDone)
         {
            emptyList(m_soldermaskBottomList);

            for (POSITION pos = m_doc.getFileList().GetHeadPosition(); pos != NULL;)
            {
               FileStruct* file = m_doc.getFileList().GetNext(pos);
               if (file != NULL && file->isShown() && file->getBlockType() != blockTypePanel && file->getBlock() != NULL)
               {
                  int mirrorFlag = 0;
                  if (file->isMirrored())
		               mirrorFlag = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic

                  CTMatrix transformMatrix = file->getTMatrix();
                  extraSoldermask(m_doc, *file->getBlock(), m_soldermaskBottomList, transformMatrix, layerTypeMaskBottom, -1, mirrorFlag);
               }
            }         

            m_soldermaskBottomListDone = true;
         }

         retval = (getMetalExposedRegion(m_doc, metal, m_soldermaskBottomList) != -1);
      }
   }

   return retval;
}

bool CMetalAnalysis::analysisBoardOutline(DataStruct& metal, ETestSurface analysisSurface, CAccessFailureReasonList& errorMessages)
{
   bool retval = false;
   if (analysisSurface == testSurfaceTop && m_testPlan.GetTopConstraints() != NULL)
   {
      if (!m_boardOutlineTopListDone)
      {
         emptyList(m_boardOutlineTopList);
         creatBoardOutlineDistance(m_doc, m_pcbFile, m_boardOutlineTopList, m_testPlan.GetTopConstraints()->GetValueBoardOutline(), false);      
         m_boardOutlineTopListDone = true;
      }

      int res = getMetalExposedRegion(m_doc, metal, m_boardOutlineTopList);
      if (m_boardOutlineTopList.GetCount() > 0 && res == -1)
      {
         // If there is board outline and no metal is exposed then set error message
         errorMessages.AddRC(DFT_AA_RC_BOARD_OUTLINE, m_testPlan.GetTopConstraints()->GetValueBoardOutline(), 0.);
      }
      else
      {
         retval = true;
      }
   }
   else if (analysisSurface == testSurfaceBottom && m_testPlan.GetBotConstraints() != NULL)
   {
      if (!m_boardOutlineBottomListDone)
      {
         emptyList(m_boardOutlineBottomList);
         creatBoardOutlineDistance(m_doc, m_pcbFile, m_boardOutlineBottomList, m_testPlan.GetBotConstraints()->GetValueBoardOutline(), true);
         m_boardOutlineBottomListDone = true;
      }

      int res = getMetalExposedRegion(m_doc, metal, m_boardOutlineBottomList);
      if (m_boardOutlineBottomList.GetCount() > 0 && res == -1)
      {
         // If there is board outline and no metal is exposed then set error message
         errorMessages.AddRC(DFT_AA_RC_BOARD_OUTLINE, m_testPlan.GetBotConstraints()->GetValueBoardOutline(), 0.);
      }
      else
      {
         retval = true;
      }
   }

   return retval;
}

DataStruct *GetRealPartPackageData(CCEtoODBDoc *doc, DataStruct *realpartData)
{
   if (doc != NULL && realpartData != NULL)
   {
      BlockStruct *realpartBlock = doc->getBlockAt( realpartData->getInsert()->getBlockNumber() );

      if (realpartBlock != NULL)
      {
         for (CDataListIterator dataListIterator(*realpartBlock); dataListIterator.hasNext();)
         {
            DataStruct* data = dataListIterator.getNext();

            if (data->isInsertType(insertTypePackage))
            {
               return data;
            }
         }
      }
   }

   return NULL;
}

void ResetRealPartPkgAttribCOMPHEIGHT(CCEtoODBDoc *doc, BlockStruct *pcbBlock)
{
   // Set COMPHEIGHT attrib in Real Part from setting in related PCB Component insert.
   // Always reset if Real Part exists, even if Real Part already has value set.
   // Users will edit this value on the PCB Component, not the Real Part.

   // It makes some logical sense that COMPHEIGHT would be on the Package in RealPart, since
   // that is where the geometry is.
   // However, since the COMPHEIGHT is on the PCB Component INSERT, and can have different
   // values per insert even when same geometry is used, in order to be absolutely consistent
   // we have to put this attrib on the top level Real Part insert.

   if (doc != NULL && pcbBlock != NULL)
   {
      for (CDataListIterator dataListIterator(*pcbBlock); dataListIterator.hasNext();)
      {
         DataStruct* data = dataListIterator.getNext();

         if (data->isInsertType(insertTypePcbComponent))
         {
            WORD compHeightKw = doc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
            Attrib* attrib = NULL;
            if (data->getAttributes() && data->getAttributes()->Lookup(compHeightKw, attrib))
            {
               double pcbComp_CompHeight = attrib->getDoubleValue();

               DataStruct *realpartInsertData = pcbBlock->FindData(data->getInsert()->getRefname(), insertTypeRealPart);

               if (realpartInsertData != NULL)
               {
                  realpartInsertData->setAttrib(doc->getCamCadData(), compHeightKw, valueTypeUnitDouble, &pcbComp_CompHeight, attributeUpdateOverwrite, NULL);
               }
            }
         }
      }
   }
}

bool CMetalAnalysis::analysisComponentOutline(DataStruct& metal, ETestSurface analysisSurface, CAccessFailureReasonList& errorMessages)
{
   bool retval = false;
   if (m_pcbFile.getBlock() != NULL)
   {
      // Make sure Real Part Hide/UnHide status is in sync with PCB Component LOADED attrib.
      // The convention prodided by Data Doctor is when a PCB Component is set to LOADED=TRUE
      // the related Real Part is set to visible (UnHidden). If the PCB Component is LOADED=FALSE
      // then the Real Part is set to invisible (Hidden). This is automatic when user uses Data Doctor
      // to alter LOADED attrib.
      // BUT if user applies some other means, such as changing the LOADED attrib in the query/edit window,
      // or altering the cc file with a text editor, the Real Part Hide/UnHide status may be left out of sync.
      // So we are taking an init step here to re-sync the Real Part and PCB Component status.
      // This is the same function that Data Doc uses for the same task.
      //this->m_doc.HideRealpartsForUnloadedComponents(m_pcbFile);
      

      int mirrorFlag = 0;
      if (m_pcbFile.isMirrored())
		   mirrorFlag = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic

      bool useRealPartOutline = false;
      if (m_testPlan.GetOutlineToUse() == DFT_OUTLINE_REALPART)
         useRealPartOutline = true;
      else if (m_testPlan.GetOutlineToUse() == DFT_OUTLINE_BOTH && m_testPlan.GetOutlinePriority() == DFT_OUTLINE_REALPART_PRIORITY)
         useRealPartOutline = true;

	   int packageOutineTopIndex = -1;
	   int packageOutineBotIndex = -1;
	   GraphicClassTag graphicClass = graphicClassUndefined;
      //*rcf BUG this part is not backward compatible. If realpart is on but there is no realpart it should use DFT outline
      // but it does not.
	   if (useRealPartOutline)
	   {
		   packageOutineTopIndex = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
		   packageOutineBotIndex = Graph_Level(RP_LAYER_PKG_BODY_BOTTOM, "", FALSE);
		   graphicClass = graphicClassPackageBody;
	   }
	   else
	   {
		   packageOutineTopIndex = Graph_Level(DFT_OUTLINE_TOP, "", FALSE);
		   packageOutineBotIndex = Graph_Level(DFT_OUTLINE_BOTTOM, "", FALSE);
		   graphicClass = graphicClassPackageOutline;
	   }
      

      if (analysisSurface == testSurfaceTop && m_testPlan.GetTopConstraints() != NULL && m_testPlan.GetTopHeightAnalysis() != NULL)
      {
         if (!m_componentOutlineTopListDone)
         {
            emptyList(m_componentOutlineTopList);

            CTMatrix transformMatrix;
            creatComponentOutlineDistance(m_doc, *m_pcbFile.getBlock(), "", m_componentOutlineTopList, transformMatrix, -1, packageOutineTopIndex,
                  graphicClass, m_testPlan.GetIgnore1PinCompOutline(), m_testPlan.GetIgnoreUnloadedCompOutline(), mirrorFlag,
                  m_testPlan.GetTopConstraints()->GetValueCompOutline(), -1, *m_testPlan.GetTopHeightAnalysis());

            m_componentOutlineTopListDone = true;
         }

         CStringArray violatingRefnames;
         int res = getDiffMetalExposedRegion(m_doc, metal, m_componentOutlineTopList, violatingRefnames);
         if (res == -1)
         {
            CString collectedNames;  //*rcf CLEAN THIS UP -- Use CStringSupArray, move from ODBC_Lib, and add a concat function, Or change it all to a map, or something
            for (int i = 0; i < violatingRefnames.GetCount(); i++)
            {
               if (!collectedNames.IsEmpty())
                  collectedNames += ", ";
               collectedNames += violatingRefnames.GetAt(i);
            }
            errorMessages.AddRC(DFT_AA_RC_COMP_OUTLINE, collectedNames);
         }
         else
         {
            retval = true;
         }
      }
      else if (analysisSurface == testSurfaceBottom && m_testPlan.GetBotConstraints() != NULL && m_testPlan.GetBotHeightAnalysis() != NULL)
      {
         if (!m_componentOutlineBottomListDone)
         {
            emptyList(m_componentOutlineBottomList);

            CTMatrix transformMatrix;
            creatComponentOutlineDistance(m_doc, *m_pcbFile.getBlock(), "", m_componentOutlineBottomList, transformMatrix, -1, packageOutineBotIndex,
                  graphicClass, m_testPlan.GetIgnore1PinCompOutline(), m_testPlan.GetIgnoreUnloadedCompOutline(), mirrorFlag,
                  m_testPlan.GetBotConstraints()->GetValueCompOutline(), -1, *m_testPlan.GetBotHeightAnalysis());

            m_componentOutlineBottomListDone = true;
         }

         CStringArray violatingRefnames; //*rcf Probably a map would be better since we only want one instance of any given name
         int res = getDiffMetalExposedRegion(m_doc, metal, m_componentOutlineBottomList, violatingRefnames);
         if (res == -1)
         {
            CString collectedNames;  //*rcf Use CStringSupArray, moved from ODBC_Lib, and add a concat function, Or change it all to a map, or something
            for (int i = 0; i < violatingRefnames.GetCount(); i++)
            {
               if (!collectedNames.IsEmpty())
                  collectedNames += ", ";
               collectedNames += violatingRefnames.GetAt(i);
            }
            errorMessages.AddRC(DFT_AA_RC_COMP_OUTLINE, collectedNames);
         }
         else
         {
            retval = true;
         }
      }
   }

   return retval;
}

void CMetalAnalysis::graphicData()
{
   LayerStruct *tmpLayer = m_doc.FindLayer_by_Name(DFT_LAYER_T_OUTLINE_TOP);
   if (tmpLayer)
      DeleteLayer(&m_doc, tmpLayer, TRUE);

   tmpLayer = m_doc.FindLayer_by_Name(DFT_LAYER_T_OUTLINE_BOT);
   if (tmpLayer)
      DeleteLayer(&m_doc, tmpLayer, TRUE);

	tmpLayer =m_doc.FindLayer_by_Name(DFT_LAYER_SM_ANALYSYS_TOP);
	if (tmpLayer)
		DeleteLayer(&m_doc, tmpLayer, TRUE);

	tmpLayer = m_doc.FindLayer_by_Name(DFT_LAYER_SM_ANALYSYS_BOT);
	if (tmpLayer)
		DeleteLayer(&m_doc, tmpLayer, TRUE);

	tmpLayer = m_doc.FindLayer_by_Name(DFT_LAYER_EXPOSE_METAL_TOP);
	if (tmpLayer)
		DeleteLayer(&m_doc, tmpLayer, TRUE);

	tmpLayer = m_doc.FindLayer_by_Name(DFT_LAYER_EXPOSE_METAL_BOT);
	if (tmpLayer)
		DeleteLayer(&m_doc, tmpLayer, TRUE);

   int outlineTop = Graph_Level(DFT_LAYER_T_OUTLINE_TOP, "", FALSE);
   int outlineBot = Graph_Level(DFT_LAYER_T_OUTLINE_BOT, "", FALSE);

   m_doc.getLayerAt(outlineTop)->setLayerType(LAYTYPE_TOP);
   m_doc.getLayerAt(outlineBot)->setLayerType(LAYTYPE_BOTTOM);
   m_doc.getLayerAt(outlineTop)->setMirroredLayerIndex(outlineBot);
   m_doc.getLayerAt(outlineBot)->setMirroredLayerIndex(outlineTop);
   
   int exMetalTop = Graph_Level(DFT_LAYER_EXPOSE_METAL_TOP, "", FALSE);
   int exMetalBot = Graph_Level(DFT_LAYER_EXPOSE_METAL_BOT, "", FALSE);

   m_doc.getLayerAt(exMetalTop)->setLayerType(LAYTYPE_UNKNOWN);
   m_doc.getLayerAt(exMetalBot)->setLayerType(LAYTYPE_UNKNOWN);
   m_doc.getLayerAt(exMetalTop)->setMirroredLayerIndex(exMetalBot);
   m_doc.getLayerAt(exMetalBot)->setMirroredLayerIndex(exMetalTop);
   m_doc.getLayerAt(exMetalTop)->setVisible(false);
   m_doc.getLayerAt(exMetalBot)->setVisible(false);

   graphicList(m_boardOutlineTopList, outlineTop);
   graphicList(m_boardOutlineBottomList, outlineBot);

   graphicList(m_componentOutlineTopList, outlineTop);
   graphicList(m_componentOutlineBottomList, outlineBot);

   graphicExposedMetal(exMetalTop, exMetalBot);

#if !defined (_DEBUG) && !defined (_RDEBUG)
   int smAnalysisTop = Graph_Level(DFT_LAYER_SM_ANALYSYS_TOP, "", FALSE);
   int smAnalysisBot = Graph_Level(DFT_LAYER_SM_ANALYSYS_BOT, "", FALSE);

   m_doc.getLayerAt(smAnalysisTop)->setLayerType(LAYTYPE_UNKNOWN);
   m_doc.getLayerAt(smAnalysisBot)->setLayerType(LAYTYPE_UNKNOWN);
   m_doc.getLayerAt(smAnalysisTop)->setMirroredLayerIndex(smAnalysisBot);
   m_doc.getLayerAt(smAnalysisBot)->setMirroredLayerIndex(smAnalysisTop);
   m_doc.getLayerAt(smAnalysisTop)->setVisible(false);
   m_doc.getLayerAt(smAnalysisBot)->setVisible(false);
   
   graphicList(m_soldermaskTopList, smAnalysisTop);
   graphicList(m_soldermaskBottomList, smAnalysisBot);
#endif
}

void CMetalAnalysis::graphicList(CSMList& list, const int layerIndex)
{
   if (m_pcbFile.getBlock() != NULL)
   {
      Graph_Block_On(m_pcbFile.getBlock());

      for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
      {
         ERPStruct* erpStruct = list.GetNext(pos);
         if (erpStruct != NULL)
         {
            DataStruct* data = Graph_PolyStruct(layerIndex, 0L, FALSE);
            FreePolyList(data->getPolyList());
            data->getPolyList() = new CPolyList(*erpStruct->pPolyList);
            data->setGraphicClass(graphicClassNormal);
         }
      }

      Graph_Block_Off();
   }
}

void CMetalAnalysis::graphicExposedMetal(const int topLayerIndex, const int bottomLayerIndex)
{
   if (m_pcbFile.getBlock() != NULL)
   { 
      CCamCadData& camCadData = m_doc.getCamCadData();

      for (POSITION pos = m_exposedMetalMap.GetStartPosition(); pos != NULL;)
      {
         int key;
         CExposedMetal* exposedMetal = NULL;
         m_exposedMetalMap.GetNextAssoc(pos, key, exposedMetal);
         if (exposedMetal != NULL && exposedMetal->getExposedData() != NULL && 
             exposedMetal->getExposedData()->getDataType() == dataTypePoly)
         {
            DataStruct* data = camCadData.getNewDataStruct(*exposedMetal->getExposedData());// Graph_PolyStruct(layerIndex, 0L, FALSE);
            data->setGraphicClass(graphicClassNormal);
            data->setLayerIndex(exposedMetal->getSurface()==testSurfaceTop?topLayerIndex:bottomLayerIndex);
            m_pcbFile.getBlock()->getDataList().AddTail(data);
         }
      }
   }
}

//-----------------------------------------------------------------------------
// CAAFeatureLost
//-----------------------------------------------------------------------------
CAAFeatureLost::CAAFeatureLost()
{ 
   Reset();
};

CAAFeatureLost::CAAFeatureLost(CAAFeatureLost &featureLost)
{
   *this = featureLost;
}

CAAFeatureLost::~CAAFeatureLost()
{
}

CAAFeatureLost& CAAFeatureLost::operator=(const CAAFeatureLost &featureLost)
{
   if (&featureLost != this)
   {
      m_iTestAttrib     = featureLost.m_iTestAttrib   ;
      m_iVia            = featureLost.m_iVia          ;
      m_iConnector      = featureLost.m_iConnector    ;
      m_iMultiPinSMD    = featureLost.m_iMultiPinSMD  ;
      m_iSinglePinSMD   = featureLost.m_iSinglePinSMD ;
      m_iTHRU           = featureLost.m_iTHRU         ;
      m_iCADPadstack    = featureLost.m_iCADPadstack  ;
      m_iBoardOutline   = featureLost.m_iBoardOutline ;
      m_iCompOutline    = featureLost.m_iCompOutline  ;
      m_iFeatureSize    = featureLost.m_iFeatureSize  ;
      m_iBeadProbeSize  = featureLost.m_iBeadProbeSize;
   }

   return *this;
}

CAAFeatureLost CAAFeatureLost::operator+(const CAAFeatureLost &featureLost)
{
   CAAFeatureLost f;
   f.m_iTestAttrib   = m_iTestAttrib   + featureLost.m_iTestAttrib   ;
   f.m_iVia          = m_iVia          + featureLost.m_iVia          ;
   f.m_iConnector    = m_iConnector    + featureLost.m_iConnector    ;
   f.m_iMultiPinSMD  = m_iMultiPinSMD  + featureLost.m_iMultiPinSMD  ;
   f.m_iSinglePinSMD = m_iSinglePinSMD + featureLost.m_iSinglePinSMD ;
   f.m_iTHRU         = m_iTHRU         + featureLost.m_iTHRU         ;
   f.m_iCADPadstack  = m_iCADPadstack  + featureLost.m_iCADPadstack  ;
   f.m_iBoardOutline = m_iBoardOutline + featureLost.m_iBoardOutline ;
   f.m_iCompOutline  = m_iCompOutline  + featureLost.m_iCompOutline  ;
   f.m_iFeatureSize  = m_iFeatureSize  + featureLost.m_iFeatureSize  ;
   f.m_iBeadProbeSize = m_iBeadProbeSize + featureLost.m_iBeadProbeSize;

   return f;
}

void CAAFeatureLost::Reset()
{
   m_iTestAttrib = 0;
   m_iVia = 0;
   m_iConnector = 0;
   m_iMultiPinSMD = 0;
   m_iSinglePinSMD = 0;
   m_iTHRU = 0;
   m_iCADPadstack = 0;
   m_iBoardOutline = 0;
   m_iCompOutline = 0;
   m_iFeatureSize = 0;
   m_iBeadProbeSize = 0;
};

//-----------------------------------------------------------------------------
// CAAErrorMessage
//-----------------------------------------------------------------------------
int CAAErrorMessage::LoadXML(CAAAccessLocation *ascLoc, CXMLNode *node)
{
  if (ascLoc == NULL || node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "ErrorMessage")
      return 0;

   CString m_param1,m_param2;
   double m_dparam1,m_dparam2;
   EAAReasonCode m_rc;
   CString Value;

   m_rc        = (node->GetAttrValue("rc", Value))?(EAAReasonCode)atoi(Value):DFT_AA_RC_NONE; 
   m_param1    = (node->GetAttrValue("param1", Value))?Value:"";
   m_param2    = (node->GetAttrValue("param2", Value))?Value:"";
   m_dparam1   = (node->GetAttrValue("dparam1", Value))?atof(Value):0.;
   m_dparam2   = (node->GetAttrValue("dparam2", Value))?atof(Value):0.;

   ascLoc->GetRCList().AddRC(m_rc, m_param1, m_param2, m_dparam1, m_dparam2);
   return 0;
}

//-----------------------------------------------------------------------------
// CAANetAccess
//-----------------------------------------------------------------------------
CAANetAccess::CAANetAccess(CCEtoODBDoc *doc, FileStruct *PCBFile, CTestPlan *testPlan, CString netName)
{
   m_pDoc = doc;
   m_pPCBFile = PCBFile;
   m_pTestPlan = testPlan;
   m_sNetName = netName;
   m_sSortKey = generateSortKey(m_sNetName);
   m_eIncludedNetType = includeUnknown;
   m_eSurface = testSurfaceUnset;
   m_iTopAccessibleCount = 0;
   m_iBotAccessibleCount = 0;
   m_iPinCount = 0;
   m_accessibleLocationList.empty();
}

CAANetAccess::CAANetAccess(const CAANetAccess& other, CTestPlan *testPlan)
{
   m_pDoc = other.m_pDoc;
   m_pPCBFile = other.m_pPCBFile;
   m_pTestPlan = testPlan;
   m_sNetName = other.m_sNetName;
   m_sSortKey = other.m_sSortKey;
   m_eIncludedNetType = other.m_eIncludedNetType;
   m_eSurface = other.m_eSurface;
   m_iPinCount = other.m_iPinCount;
   m_iTopAccessibleCount = other.m_iTopAccessibleCount;
   m_iBotAccessibleCount = other.m_iBotAccessibleCount;

   // Reset accessible count and feature lost count and access location
   m_accessibleLocationList.empty();
   m_accessibleLocationList.CopyAcccesLocationList(other.m_accessibleLocationList);
}

CAANetAccess::~CAANetAccess()
{
   m_accessibleLocationList.empty();
}

void CAANetAccess::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Net %s at 0x%08x::\n", indent, " ", m_sNetName, this);
   indent += 3;

   CString netType = "Unknown";
   if (m_eIncludedNetType == includeMultiPinNet)
      netType = "Multiple Pin Net";
   else if (m_eIncludedNetType == includeSinglePinNet)
      netType = "Single Pin Net";
   else if (m_eIncludedNetType == includeUnconnecedNet)
      netType = "Nonconnected Net";

   CString netSurface = "Not Set";
   if (m_eSurface == testSurfaceTop)
      netSurface = "Top";
   else if (m_eSurface == testSurfaceBottom)
      netSurface = "Bottom";
   else if (m_eSurface == testSurfaceBoth)
      netSurface = "Both";
   
   file.WriteString("%*sm_eIncludedNetType = %s::\n", indent, " ", netType);
   file.WriteString("%*sm_iTopAccessibleCount = %d::\n", indent, " ", m_iTopAccessibleCount);
   file.WriteString("%*sm_iBotAccessibleCount = %d::\n", indent, " ", m_iBotAccessibleCount);
   file.WriteString("%*sm_iPinCount = %d::\n", indent, " ", m_iPinCount);
   file.WriteString("%*sm_eIncludedNetType = %s::\n", indent, " ", netSurface);

   for (POSITION pos = m_accessibleLocationList.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAAAccessLocation* accessLocation = NULL;
      m_accessibleLocationList.GetNextAssoc(pos, key, accessLocation);

      if (accessLocation)
         accessLocation->DumpToFile(file, indent);
   }
   indent -= 3;
}

void CAANetAccess::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<NetAccess Name=\"%s\"", SwapSpecialCharacters(m_sNetName));

   switch (m_eSurface)
   {
   case testSurfaceTop:
      writeFormat.writef(" Surface=\"Top\"");
      break;
   case testSurfaceBottom:
      writeFormat.writef(" Surface=\"Bottom\"");
      break;
   case testSurfaceBoth:
      writeFormat.writef(" Surface=\"Both\"");
      break;
   }
   
   switch (m_eIncludedNetType)
   {
   case includeSinglePinNet:
      writeFormat.writef(" Type=\"Single-pin\"");
      break;
   case includeMultiPinNet:
      writeFormat.writef(" Type=\"Multi-pin\"");
      break;
   case includeUnconnecedNet:
      writeFormat.writef(" Type=\"Unconnected\"");
      break;
   }
   writeFormat.writef(">\n");
   
   writeFormat.pushHeader("  ");
   m_accessibleLocationList.WriteXML(writeFormat, progress);
   writeFormat.popHeader();

   writeFormat.writef("</NetAccess>\n");
}

int CAANetAccess::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CString nodeName;

   if (!node->GetName(nodeName) || nodeName != "NetAccess")
      return 0;

   CString name, buf;

   if (!node->GetAttrValue("Name", name))
      return 0;

   m_sNetName = name;

   if (node->GetAttrValue("Surface", buf))
   {
      if (buf == "Top")
         m_eSurface = testSurfaceTop;
      else if (buf == "Bottom")
         m_eSurface = testSurfaceBottom;
      else if (buf == "Both")
         m_eSurface = testSurfaceBoth;
   }

   if (node->GetAttrValue("Type", buf))
   {
      if (buf == "Single-pin")
         m_eIncludedNetType = includeSinglePinNet;
      else if (buf == "Multi-pin")
         m_eIncludedNetType = includeMultiPinNet;
      else if (buf == "Unconnected")
         m_eIncludedNetType = includeUnconnecedNet;
   }

   m_iTopAccessibleCount = 0;
   m_iBotAccessibleCount = 0;

   CXMLNode *subNode = NULL;
   node->ResetChildList();

   while (subNode = node->NextChild())
   {
      CString nodeName;

      if (!subNode->GetName(nodeName) || nodeName != "AccessPoint")
         continue;

      CString buf;
      ETestSurface mountedSurface, accessSurface;

      if (!subNode->GetAttrValue("MountSurface", buf))
         continue;

      if (buf == "Top")
         mountedSurface = testSurfaceTop;
      else if (buf == "Bottom")
         mountedSurface = testSurfaceBottom;

      if (!subNode->GetAttrValue("AccessSurface", buf))
         continue;
      if (buf == "Top")
         accessSurface = testSurfaceTop;
      else if (buf == "Bottom")
         accessSurface = testSurfaceBottom;

      if (!subNode->GetAttrValue("TargetType", buf))
         continue;
      int targetType = atoi(buf);


      if (!subNode->GetAttrValue("Entity", buf))
         continue;
      long entityNum = atoi(buf);

      CEntity featureEntity = CEntity::findEntity(getCamCadData(),entityNum);
      if (featureEntity.getEntityType() == entityTypeUndefined)
         continue;

      CString refDes;
      AccessItemTypeTag accessItemType = accessItemTypePin;
      switch (featureEntity.getEntityType())
      {
      case entityTypeCompPin:
         {
            refDes = featureEntity.getCompPin()->getPinRef();
            accessItemType = accessItemTypePin;
         }
         break;
      case entityTypeData:
         {
            DataStruct* data = featureEntity.getData();

            if (data->getDataType() != dataTypeInsert)
            {
               CString msg;

					// Case 1921, encountered ccz with duplicate entity numbers.
					// Look for another entity with same number but acceptable type.
					// This is not a fix (the real fix for the real cause is in EntityNumber.cpp)
					// but it does treat a subset of the problem ccz files. It treats all the
					// examples that were provided with the case, but there probably can be ccz 
					// arrangements that this will not treat.
					featureEntity = CEntity::findEntity(getCamCadData(),entityNum,entityTypeCompPin);
					if (featureEntity.getEntityType() == entityTypeCompPin)
					{
						// If we want to issues a warning... but since users can't do anything about
						// it, it seems best to keep silent if we are able to find a suitable item.
						////msg.Format("Error processing NetAccess section for net '%s', entity number '%d' is not unique.  Import will proceed, but CC file contains errors.",
                  ////        m_sNetName, entityNum);
						////ErrorMessage(msg, "Error in CAMCAD file\n");

						refDes = featureEntity.getCompPin()->getPinRef();
						break;						
					}

               msg.Format("Error in NetAccess section for net '%s', unexpected data type for entity number '%d'.  Import of file will be terminated.",
                          m_sNetName, entityNum);
               ErrorMessage(msg, "Error in CAMCAD file\n");
               return -1;
            }

            refDes = data->getInsert()->getRefname();

            if (data->getInsert()->getInsertType() == insertTypeVia)
               accessItemType = accessItemTypeVia;
         }
         break;
      }

      if (!subNode->GetAttrValue("LargestDiameter", buf))
         continue;
      double exposedMetalDiameter = atof(buf);

      if (!subNode->GetAttrValue("Accessiblity", buf))
         continue;
      bool accessible = (buf == "True");

      CString name;
      if (subNode->GetAttrValue("Name", buf))
      {
         if (buf == "??")
            name.Empty();
         else 
            name = buf;
      }

      CAAAccessLocation* accLoc = new CAAAccessLocation(featureEntity, refDes, targetType, accessItemType, mountedSurface, accessSurface, m_sNetName, CPoint2d(), m_pDoc->getPageUnits());

      if (accessible)
      {
         CPoint2d loc;
         //if (node->GetAttrValue("Name", buf))
         // name = buf;
         if (subNode->GetAttrValue("X", buf))
            loc.x = atof(buf);
         if (subNode->GetAttrValue("Y", buf))
            loc.y = atof(buf);
         accLoc->SetAccessLocation(loc.x, loc.y);
      }
      else
      {               
         subNode->ResetChildList();
         CXMLNode *errmsg_Node = NULL;
         while (errmsg_Node = subNode->NextChild())
         {
            CAAErrorMessage m_errormessage;
            m_errormessage.LoadXML(accLoc,errmsg_Node);
            delete errmsg_Node;
         }         
      }

      accLoc->SetAccessible(accessible);
      accLoc->SetExposedMetalDiameter(exposedMetalDiameter);
      accLoc->SetName(name);

      m_accessibleLocationList.SetAt(accLoc->GetSortKey(), accLoc);

      // Keep count of accessibles
      if (accessible)
      {
         if (accessSurface == testSurfaceTop)
            m_iTopAccessibleCount++;
         else
            m_iBotAccessibleCount++;
      }

      delete subNode;
   }

   return 0;
}

CAAAccessLocation * CAAAccessLocation::LoadXML(CXMLNode *node)
{
   // This is for vPlan style AccessPoint only, not good old RSI AccessPoint.
   // Good old RSI AccessPoint is handled within CAANetAccess::LoadXML().

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "AccessPoint")
      return NULL;

   CString buf;
   ETestSurface mountedSurface = testSurfaceUnset;
   ETestSurface accessSurface = testSurfaceUnset;

   if (node->GetAttrValue("MountSurface", buf))
   {
      if (buf == "Top")
         mountedSurface = testSurfaceTop;
      else if (buf == "Bottom")
         mountedSurface = testSurfaceBottom;
   }

   if (node->GetAttrValue("AccessSurface", buf))
   {
      if (buf == "Top")
         accessSurface = testSurfaceTop;
      else if (buf == "Bottom")
         accessSurface = testSurfaceBottom;
   }

   int targetType = -1;
   if (node->GetAttrValue("TargetType", buf))
   {
      targetType = atoi(buf);
   }

   long entityNum = -1;
#ifdef ENTITY_SUPPORT
   if (subNode->GetAttrValue("Entity", buf))
   {
      entityNum = atoi(buf);

      CEntity featureEntity = CEntity::findEntity(getCamCadData(),entityNum);

      if (featureEntity.getEntityType() != entityTypeUndefined)
      {
         CString refDes;
         AccessItemTypeTag accessItemType = accessItemTypePin;
         switch (featureEntity.getEntityType())
         {
         case entityTypeCompPin:
            {
               refDes = featureEntity.getCompPin()->getPinRef();
               accessItemType = accessItemTypePin;
            }
            break;
         case entityTypeData:
            {
               DataStruct* data = featureEntity.getData();

               if (data->getDataType() != dataTypeInsert)
               {
                  CString msg;

                  // Case 1921, encountered ccz with duplicate entity numbers.
                  // Look for another entity with same number but acceptable type.
                  // This is not a fix (the real fix for the real cause is in EntityNumber.cpp)
                  // but it does treat a subset of the problem ccz files. It treats all the
                  // examples that were provided with the case, but there probably can be ccz 
                  // arrangements that this will not treat.
                  featureEntity = CEntity::findEntity(getCamCadData(),entityNum,entityTypeCompPin);
                  if (featureEntity.getEntityType() == entityTypeCompPin)
                  {
                     // If we want to issues a warning... but since users can't do anything about
                     // it, it seems best to keep silent if we are able to find a suitable item.
                     ////msg.Format("Error processing NetAccess section for net '%s', entity number '%d' is not unique.  Import will proceed, but CC file contains errors.",
                     ////        m_sNetName, entityNum);
                     ////ErrorMessage(msg, "Error in CAMCAD file\n");

                     refDes = featureEntity.getCompPin()->getPinRef();
                     break;						
                  }

                  msg.Format("Error in NetAccess section for net '%s', unexpected data type for entity number '%d'.  Import of file will be terminated.",
                     m_sNetName, entityNum);
                  ErrorMessage(msg, "Error in CAMCAD file\n");
                  return -1;
               }

               refDes = data->getInsert()->getRefname();

               if (data->getInsert()->getInsertType() == insertTypeVia)
                  accessItemType = accessItemTypeVia;
            }
            break;
         }
      }
   }
#endif

   double exposedMetalDiameter = 0;
   if (node->GetAttrValue("LargestDiameter", buf))
   {
      exposedMetalDiameter = atof(buf);
   }

   bool accessible = true;
   if (node->GetAttrValue("Accessiblity", buf))
   {
      accessible = (buf.CompareNoCase("True") == 0);
   }

   CString accessName;
   if (node->GetAttrValue("Name", buf))
   {
      if (buf == "??")
         accessName.Empty();
      else 
         accessName = buf;
   }

   CString netName;
   if (node->GetAttrValue("NetName", buf))
   {
      netName = buf;
   }

   CPoint2d location;
   if (node->GetAttrValue("X", buf))
         location.x = atof(buf);
   if (node->GetAttrValue("Y", buf))
         location.y = atof(buf);

   CAAAccessLocation* accLoc = new CAAAccessLocation(accessSurface, accessName, location, netName);

   /*deadcode ?
   if (accessible)
   {
      CPoint2d loc;
      //if (node->GetAttrValue("Name", buf))
      // name = buf;
      if (subNode->GetAttrValue("X", buf))
         loc.x = atof(buf);
      if (subNode->GetAttrValue("Y", buf))
         loc.y = atof(buf);
      accLoc->SetAccessLocation(loc.x, loc.y);
   }
   else
   {               
      subNode->ResetChildList();
      CXMLNode *errmsg_Node = NULL;
      while (errmsg_Node = subNode->NextChild())
      {
         CAAErrorMessage m_errormessage;
         m_errormessage.LoadXML(accLoc,errmsg_Node);
         delete errmsg_Node;
      }         
   }

   accLoc->SetAccessible(accessible);
   accLoc->SetExposedMetalDiameter(exposedMetalDiameter);
   accLoc->SetName(name);
   */

   return accLoc;
}

void CAANetAccess::AddAccessLocation(CAAAccessLocation *accLoc)
{
   if (accLoc != NULL)
   {
      m_accessibleLocationList.SetAt(accLoc->GetSortKey(), accLoc);

      // Keep count of accessibles
      if (accLoc->GetAccessible())
      {
         if (accLoc->GetAccessSurface() == testSurfaceTop)
            m_iTopAccessibleCount++;
         else if (accLoc->GetAccessSurface() == testSurfaceBottom)
            m_iBotAccessibleCount++;
         // else unknown surface, don't count it.
      }
   }
}

int CAANetAccess::AddViaAccessLocation(CMetalAnalysis& metalAnalysis, DataStruct *via, CExposedDataMap *exposedDataMap)
{
   int retval = DFT_ERROR_NONE;
   if (via != NULL && via->getDataType() == dataTypeInsert && via->getInsert()->getInsertType() == insertTypeVia)
   {
      InsertStruct* viaInsert = via->getInsert();
      BlockStruct* block = m_pDoc->getBlockAt(viaInsert->getBlockNumber());
      if (block != NULL)
      {
         // Add via access location to Net Acess
         int targetType = GetTargetType(via) | DFT_TARGETTYPE_VIA;
         bool unloaded = false;
         Attrib *attrib = NULL;
         WORD loadedKey = m_pDoc->RegisterKeyWord(ATT_LOADED, 0, VT_STRING);
         if (via->getAttributes() && via->getAttributes()->Lookup(loadedKey, attrib))
         {
            if (!attrib->getStringValue().CompareNoCase("FALSE"))
               unloaded = true;
         }

         CPoint2d compCentroid;
         if (via->getInsert()->getCentroidLocation(getCamCadData(), compCentroid) == false)
            compCentroid = via->getInsert()->getOrigin2d();
         CPoint2d dataCentroid = compCentroid;

         addAccessLocation(metalAnalysis, CEntity(via), viaInsert->getRefname(), viaInsert->getOrigin2d(), 
               accessItemTypeVia, targetType, viaInsert->getGraphicMirrored(), false, unloaded, *block,
               accessItemTypeVia, compCentroid, dataCentroid); 
      }
   }

   return retval;
}

int CAANetAccess::AddNetListAccessLocation(CMetalAnalysis& metalAnalysis, NetStruct *net,CComponentMap& componentMap, CExposedDataMap *exposedDataMap, bool breakOnError)
{
   int retval = DFT_ERROR_NONE;

   POSITION pos = net->getHeadCompPinPosition();
   while (pos)
   {
      CompPinStruct *compPin = net->getNextCompPin(pos);
      if (compPin == NULL)
         continue;

      BlockStruct* padstackBlock = m_pDoc->getBlockAt(compPin->getPadstackBlockNumber());
      DataStruct* compData = NULL;
		componentMap.Lookup(compPin->getRefDes(), compData);

      if (compData != NULL && compData->getInsert() !=  NULL)
		{
			// Case 2171 - The commented out "If" statement has a bug.  If breakOnError is false, then nothing
			// was stopping the code in the "else" statement from executing when padstackBlock was NULL. 
			// Eventually this lead to a crash downstream from addAccessLocation, because padstackBlock is
			// derefenced in the call and later it will try to access the instance but there is no instance.
			//
			// if ((compPin->getPinCoordinatesComplete() == false || padstackBlock == NULL) && breakOnError)

			if (compPin->getPinCoordinatesComplete() == false || padstackBlock == NULL)
			{
				if (breakOnError)
				{
					retval = DEF_ERROR_COMPPIN_LOCATION;
					break;
				}
				else
				{
					// Just skip over this comppin if breakOnError is not set
				}
			}
			else
			{
				BlockStruct* compBlock = m_pDoc->getBlockAt(compData->getInsert()->getBlockNumber());
				if (compBlock != NULL)
				{

					int targetType = GetTargetType(compData, compPin);;
					bool singlePin = (compBlock->getPinCount()>1)?false:true;
					bool unloaded = false;
					AccessItemTypeTag accessItemType = accessItemTypePin;

					Attrib *attrib = NULL;
					WORD loadedKey = m_pDoc->RegisterKeyWord(ATT_LOADED, 0, VT_STRING);
					if (compData->getAttributes() && compData->getAttributes()->Lookup(loadedKey, attrib))
					{
						if (!attrib->getStringValue().CompareNoCase("FALSE"))
							unloaded = true;
					}

					if (compData->getDataType() == dataTypeInsert)
					{
						switch (compData->getInsert()->getInsertType())
						{
						case insertTypePcbComponent:  accessItemType = accessItemTypePin;  break;
						case insertTypeTestPoint:     accessItemType = accessItemTypeTp;   break;
						}
					}


					CPoint2d dataCentroid = compPin->getOrigin();
					CPoint2d compCentroid;
					if (compData->getInsert()->getCentroidLocation(getCamCadData(), compCentroid) == false)
						compCentroid = compData->getInsert()->getOrigin2d();

					bool accessLocationAdded = addAccessLocation(metalAnalysis, CEntity(compPin), compPin->getPinRef(), compPin->getOrigin(),
						accessItemType, targetType, compPin->getMirror()?true:false, singlePin, unloaded, *padstackBlock,
						accessItemType, compCentroid, dataCentroid);
	            
					if (accessLocationAdded)
						m_iPinCount ++;
				}
			}
		}


   }

   return retval;
}

bool CAANetAccess::addAccessLocation(CMetalAnalysis& metalAnalysis, const CEntity& featureEntity, const CString featureRefdes, const CPoint2d featureOrigin,
                                    const AccessItemTypeTag accessItemType, const int targetType, const bool isMirrored,
                                    const bool singlePin, const bool unloaded, BlockStruct& padstackBlock, AccessItemTypeTag itemType,
                                    CPoint2d compCentroid, CPoint2d dataCentroid)
{
   bool retval = false;
   ETestSurface mountSurface = isMirrored?testSurfaceBottom:testSurfaceTop;
   ETestSurface accessSurface = testSurfaceUnset;

   bool topAccess = padstackBlock.IsAccessibleFromTop(getCamCadData(), isMirrored);
   bool botAccess = padstackBlock.IsAccessibleFromBottom(getCamCadData(), isMirrored);

   if (topAccess && botAccess)
      accessSurface = testSurfaceBoth;
   else if (!topAccess && !botAccess)
      accessSurface = mountSurface;
   else if (topAccess)
      accessSurface = testSurfaceTop;
   else
      accessSurface = testSurfaceBottom;

   CAAAccessLocation* accessLocation = InsertToList(featureEntity, featureRefdes, featureOrigin, 
      accessItemType, targetType, mountSurface, (accessSurface==testSurfaceBoth)?testSurfaceTop:accessSurface, 
      m_sNetName, singlePin?boolTrue:boolFalse, unloaded?boolTrue:boolFalse);      

   if (accessLocation != NULL)
   {
      int exposedMetalId = metalAnalysis.addExposedMetal(padstackBlock, (accessSurface==testSurfaceBoth)?testSurfaceTop:accessSurface, 
               itemType, compCentroid, dataCentroid, targetType);
      accessLocation->setExposedMetaId(exposedMetalId);
      retval = true;
   }

   if ((targetType & DFT_TARGETTYPE_THRUHOLE) && accessSurface == testSurfaceBoth)
   {
       // If both surfaces are allow, then through hole component should be test top in the above statement and bottom in the following statements
      accessLocation = InsertToList(featureEntity, featureRefdes, featureOrigin, accessItemType, targetType,
         mountSurface, testSurfaceBottom, m_sNetName, singlePin?boolTrue:boolFalse, unloaded?boolTrue:boolFalse);

      if (accessLocation != NULL)
      {
         int exposedMetalId = metalAnalysis.addExposedMetal(padstackBlock, testSurfaceBottom, itemType, compCentroid, dataCentroid, targetType);
         accessLocation->setExposedMetaId(exposedMetalId);
         retval = true;
      }
   }

   if (m_eSurface != testSurfaceBoth)
   {
      if (targetType & DFT_TARGETTYPE_THRUHOLE)
      {
         m_eSurface = testSurfaceBoth;
      }
      else if (m_eSurface == testSurfaceTop && accessSurface == testSurfaceBottom ||
               m_eSurface == testSurfaceBottom && accessSurface == testSurfaceTop) 
      {
         m_eSurface = testSurfaceBoth;
      }
      else
      {
         m_eSurface = accessSurface;
      }
   }

   return retval;
}

CAAAccessLocation* CAANetAccess::InsertToList(const CEntity& featureEntity, const CString featureRefdes, const CPoint2d featureOrigin,
                                const AccessItemTypeTag accessItemType, const int targetType, const ETestSurface mountSurface,
                                const ETestSurface accessSurface, const CString netName, const bool singlePin, const bool unloaded)
{
   CAAAccessLocation* accessLocation = new CAAAccessLocation(featureEntity, featureRefdes,  
      targetType, accessItemType, mountSurface, accessSurface, netName, featureOrigin, m_pDoc->getPageUnits(),
      singlePin?boolTrue:boolFalse, unloaded?boolTrue:boolFalse);

   CAAAccessLocation* existingLocation = NULL;
   if (!m_accessibleLocationList.Lookup(accessLocation->GetSortKey(), existingLocation))
   {
      m_accessibleLocationList.SetAt(accessLocation->GetSortKey(), accessLocation);
   }
   else
   {
      delete accessLocation;
      accessLocation = NULL;
   }

   return accessLocation;
}

bool CAANetAccess::IsNetAnalyzed() const
{
   if (m_pTestPlan == NULL)
      return false;

   if ((m_eIncludedNetType == includeMultiPinNet   && m_pTestPlan->GetIncludeMulPinNet()) ||
       (m_eIncludedNetType == includeSinglePinNet  && m_pTestPlan->GetIncludeSngPinNet()) ||
       (m_eIncludedNetType == includeUnconnecedNet && m_pTestPlan->GetIncludeNCNet())     )
      return true;
   else
      return false;
}

void CAANetAccess::AnalizeNetAccess(CMetalAnalysis& metalAnalysis, CExposedDataMap *exposedDataMap, const CAccessOffsetOptions& accessOffsetOptions, double tolerance)
{
   ResetNetAccess();

	if (m_pDoc != NULL && m_pPCBFile != NULL && m_pTestPlan != NULL && isNetAccessible())
   {
      UpdateCADPadstackTargetType();

      // debug  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      bool filterFlag = false; //true;
      //CString filterPinRef = "J1.11";
      CString filterPinRef = "TP302.1";
      // debug  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

      for (POSITION pos = m_accessibleLocationList.GetStartPosition(); pos != NULL;)
      {
         CString key;
         CAAAccessLocation* accLoc = NULL;
         m_accessibleLocationList.GetNextAssoc(pos, key, accLoc);

         // debug  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
         if (accLoc != NULL && filterFlag)
         {
            if (accLoc->GetRefDes().CompareNoCase(filterPinRef) != 0)
            {
               continue;
            }
            else
            {
               int jj = 0;
            }
         }
         // debug  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

         if (accLoc != NULL && isSurfaceAccessible(*accLoc) && isTargetTypeAccessible(*accLoc))
         {
				if (metalAnalysis.getAccessibleMetal(*accLoc, *m_pTestPlan, accessOffsetOptions, *m_pDoc, *m_pPCBFile, tolerance) &&
               !accLoc->GetRCList().ContainsFatalAccessError())
            {         
               int featureSizeLayer = Graph_Level(DFT_LAYER_FEATURE_SIZE, "", 0L);
               LayerStruct *layer = m_pDoc->getLayerAt(featureSizeLayer);
               layer->setVisible(false);

               Graph_Block_On(m_pPCBFile->getBlock());
               DataStruct *featureData= Graph_Circle(featureSizeLayer, accLoc->GetAccessLocation().x, accLoc->GetAccessLocation().y,
                     accLoc->GetExposedMetalDiameter()/2, 0L, 0, FALSE, FALSE);
               Graph_Block_Off();

               // When the code make it here, then the feature is accessible
               if (accLoc->GetAccessSurface() == testSurfaceTop)
                  m_iTopAccessibleCount++;
               else
                  m_iBotAccessibleCount++;
            }
            else
            {
               if (accLoc->GetRCList().HasRC(DFT_AA_RC_COMP_OUTLINE))
               {
                  accLoc->GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iCompOutline++:m_botFeatureLost.m_iCompOutline++;
               }
               else if (accLoc->GetRCList().HasRC(DFT_AA_RC_BOARD_OUTLINE))
               {
                  accLoc->GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iBoardOutline++:m_botFeatureLost.m_iBoardOutline++;
               }
               else if(accLoc->GetRCList().HasRC(DFT_AA_RC_SMALL_METAL))
               {
                  accLoc->GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iFeatureSize++:m_botFeatureLost.m_iFeatureSize++;
               }
               else if(accLoc->GetRCList().HasRC(DFT_AA_RC_SMALL_METAL_BEAD_PROBE))
               {
                  accLoc->GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iBeadProbeSize++:m_botFeatureLost.m_iBeadProbeSize++;
               }
            }
         }
      }
   }

}

bool CAANetAccess::isNetAccessible()
{
   bool netAccessible = false;
   if (m_iPinCount > 1) 
   {
      m_eIncludedNetType = includeMultiPinNet;
      netAccessible = m_pTestPlan->GetIncludeMulPinNet();
   }
   else if (m_iPinCount == 1 && m_sNetName.Left(4).CompareNoCase("NC__") == 0)
   {
      m_eIncludedNetType = includeUnconnecedNet;
      netAccessible = m_pTestPlan->GetIncludeNCNet();
   }
   else if (m_iPinCount == 1)
   {
      m_eIncludedNetType = includeSinglePinNet;
      netAccessible = m_pTestPlan->GetIncludeSngPinNet();
   }

   if (netAccessible == false)
   {
      EAAReasonCode reasonCode = DFT_AA_RC_NONE;
      switch (m_eIncludedNetType)
      {
      case includeMultiPinNet:      reasonCode = DFT_AA_RC_NO_MULTI_PIN_NET;          break;
      case includeUnconnecedNet:    reasonCode = DFT_AA_RC_NO_UNCONNECTED_NET;        break;
      case includeSinglePinNet:     reasonCode = DFT_AA_RC_NO_SINGLE_PIN_NET;         break;
      }

      POSITION pos = m_accessibleLocationList.GetStartPosition();
      while (pos)
      {
         CString key;
         CAAAccessLocation* accLoc = NULL;
         m_accessibleLocationList.GetNextAssoc(pos, key, accLoc);

         if (accLoc != NULL)
         {
            accLoc->SetAccessible(false);
            accLoc->GetRCList().AddRC(reasonCode);
         }
      }
   }

   return netAccessible;
}

bool CAANetAccess::isSurfaceAccessible(CAAAccessLocation& accLoc)
{
   bool accessible = true;
   if ((accLoc.GetAccessSurface() == testSurfaceBottom && m_pTestPlan->GetSurface() == testSurfaceTop) ||
       (accLoc.GetAccessSurface() == testSurfaceTop && m_pTestPlan->GetSurface() == testSurfaceBottom))
   {
      accLoc.SetAccessible(false);
      accLoc.GetRCList().AddRC(DFT_AA_RC_NO_SURFACE);
      accessible = false;
   }
   else if (accLoc.GetMountSurface() == accLoc.GetAccessSurface() && accLoc.GetTargetType() & DFT_TARGETTYPE_THRUHOLE)
   {
      if (!accLoc.IsSinglePin() && !accLoc.IsUnloaded() && 
          !(accLoc.GetTargetType() & DFT_TARGETTYPE_VIA) && m_pTestPlan->GetTesterType() == testerTypeFixture)
      {
         accLoc.SetAccessible(false);
         accLoc.GetRCList().AddRC(DFT_AA_RC_THRU_NOT_ALLOW);
         accessible = false;
      }
   }

   return accessible;
}

bool CAANetAccess::isTargetTypeAccessible(CAAAccessLocation& accLoc)
{
   bool accessible = false;
   CTargetType *tpTargetType = accLoc.GetAccessSurface()==testSurfaceTop?m_pTestPlan->GetTopTargetTypes():m_pTestPlan->GetBotTargetTypes();
   if (tpTargetType != NULL)
   {
      int lastTargetTypeCheck = -1;

      // analysis target type
      if (accessible == false && accLoc.GetTargetType() & DFT_TARGETTYPE_TESTATTRIBUTE)
      {
         accessible = tpTargetType->GetEnableTestAttrib();
         lastTargetTypeCheck = DFT_TARGETTYPE_TESTATTRIBUTE;
      }

      if (accessible == false && accLoc.GetTargetType() & DFT_TARGETTYPE_VIA)
      {
         accessible = tpTargetType->GetEnableVia();
         lastTargetTypeCheck = DFT_TARGETTYPE_VIA;
      }

      if (accessible == false && accLoc.GetTargetType() & DFT_TARGETTYPE_CONNECTOR)
      {
         accessible = tpTargetType->GetEnableConnector();
         lastTargetTypeCheck = DFT_TARGETTYPE_CONNECTOR;
      }

      if (accessible == false && accLoc.GetTargetType() & DFT_TARGETTYPE_SMD)
      {
         if (!accLoc.IsSinglePin())
         {
            // Multiple pin SMD need to check if target type is enable before it is included
            accessible = tpTargetType->GetEnableSMD();
            lastTargetTypeCheck = DFT_TARGETTYPE_SMD;
         }
         else if (accLoc.GetTargetType() & DFT_TARGETTYPE_TESTATTRIBUTE)
         {
            // Single pin SMD with "Test" attribute is accepted even if SinglePinSMD is not enabled
            accessible = true;
         }
         else
         {
            // Single pin SMD need to check if target type is enable before it is included
            accessible = tpTargetType->GetEnableSinglePinSMD();
            lastTargetTypeCheck = DFT_TARGETTYPE_SINGLEPINSMD;
         }
      }

      if (accessible == false && accLoc.GetTargetType() & DFT_TARGETTYPE_THRUHOLE)
      {
         // Only THRU HOLE component is consider as THRU HOLE when check against allowed target type
         // via are only consider as VIA even though it is a type of THRU HOLE
         if (!(accLoc.GetTargetType() & DFT_TARGETTYPE_VIA))
         {
            accessible = tpTargetType->GetEnableTHUR();
            lastTargetTypeCheck = DFT_TARGETTYPE_THRUHOLE;
         }
      }

      if (accessible == false && accLoc.GetTargetType() & DFT_TARGETTYPE_CADPADSTACK)
      {
         accessible = tpTargetType->GetEnableCADPadstack();
         lastTargetTypeCheck = DFT_TARGETTYPE_CADPADSTACK;
      }

      if (accessible == false)
      {

         accLoc.SetAccessible(false);
         accLoc.GetRCList().AddRC(DFT_AA_RC_NO_TARGET_TYPE);

         // Update feature lost for target type
         switch (lastTargetTypeCheck)
         {
         case DFT_TARGETTYPE_TESTATTRIBUTE:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iTestAttrib++:m_botFeatureLost.m_iTestAttrib++;
            break;
         case DFT_TARGETTYPE_VIA:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iVia++:m_botFeatureLost.m_iVia++;
            break;
         case DFT_TARGETTYPE_CONNECTOR:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iConnector++:m_botFeatureLost.m_iConnector++;
            break;
         case DFT_TARGETTYPE_SMD:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iMultiPinSMD++:m_botFeatureLost.m_iMultiPinSMD++;
            break;
         case DFT_TARGETTYPE_SINGLEPINSMD:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iSinglePinSMD++:m_botFeatureLost.m_iSinglePinSMD++;
            break;
         case DFT_TARGETTYPE_THRUHOLE:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iTHRU++:m_botFeatureLost.m_iTHRU++;
            break;
         case DFT_TARGETTYPE_CADPADSTACK:  
            accLoc.GetAccessSurface()==testSurfaceTop?m_topFeatureLost.m_iCADPadstack++:m_botFeatureLost.m_iCADPadstack++;
            break;
         }
      }

   }

   return accessible;
}

CAAAccessLocation *CAANetAccess::FindNearestAccess(CPoint2d location, double &distance)
{
   distance = DBL_MAX;

   CAAAccessLocation *selectedAccess = NULL;

   POSITION accessPos = this->GetHeadPosition_AccessibleLocations();
   while (accessPos != NULL)
   {
      CAAAccessLocation *accLoc = this->GetNext_AccessibleLocations(accessPos);

      CPoint2d accPnt = accLoc->GetAccessLocation();

      double trialDistance = accPnt.distance(location);

      if (trialDistance < distance)
      {
         selectedAccess = accLoc;
         distance = trialDistance;
      }
   }

   return selectedAccess;
}

void CAANetAccess::ApplyToNet(NetStruct *net, FileStruct *pcbFile)
{
   if (net != NULL && pcbFile != NULL)
   {
      POSITION accessPos = this->GetHeadPosition_AccessibleLocations();
      while (accessPos != NULL)
      {
         CAAAccessLocation *accLoc = this->GetNext_AccessibleLocations(accessPos);

         // What we really want here is a search that considers area of pad
         // of the comppin. But for now we'll settle for aligned insert location.
         CompPinStruct *cp = net->findCompPin(accLoc->GetAccessLocation());

         if (cp != NULL)
         {
            // Connect access mark data to comppin.
            int dataLink = cp->getEntityNumber();
            accLoc->ResetTargetType(DFT_TARGETTYPE_SMD); // The standard target type for comppins.
            accLoc->SetFeatureEntity(cp);                // Could use entity to get datalinke instead of passing datalink.

            // Place the access mark insert.
            DataStruct* accData = PlaceTestAccessPoint(m_pDoc, pcbFile->getBlock(), accLoc, 
               dataLink, m_pDoc->getSettings().getPageUnits());
         }
         else
         {
            // Not on cp, look for via.

            CPoint2d accXY = accLoc->GetAccessLocation();

            double distance = DBL_MAX;
            DataStruct *selectedViaData = NULL;

            // This could be made into a function to find by nearest location and insert type, return datastruct and distance.
            POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = pcbFile->getBlock()->getNextDataInsert(dataPos);

               if (data != NULL && data->isInsertType(insertTypeVia))
               {
                  CPoint2d viaXY = data->getInsert()->getOrigin2d();

                  double trialDistance = viaXY.distance(accXY);

                  if (trialDistance < distance)
                  {
                     distance = trialDistance;
                     selectedViaData = data;
                  }
               }
            }

            if (distance < 0.001) //*rcf BUG need better control here. Like comppins, need to use area of pad for basis of distance.
            {
               // Connect access mark data to comppin.
               int dataLink = selectedViaData->getEntityNumber();
               accLoc->ResetTargetType(DFT_TARGETTYPE_SMD); //*rcf BUG should be somethign else for vias, go figure it out.
               accLoc->SetFeatureEntity(selectedViaData);                // Could use entity to get datalinke instead of passing datalink.

               // Place the access mark insert.
               DataStruct* accData = PlaceTestAccessPoint(m_pDoc, pcbFile->getBlock(), accLoc, 
                  dataLink, m_pDoc->getSettings().getPageUnits());

               //*rcf DFT_CR Need to validate netname attrib on via.
            }
            else
            {
               // Did not find via either. Issue log message about access mark not having a home.
               CString msg;
               msg.Format("Could not find a target for Access Mark [%s]\n", accLoc->GetName());
               getApp().LogMessage(msg);
            }

         }
      }

   }
}

void CAANetAccess::AddMaverick(DataStruct *accessPointData)
{
	// In support of fix for case 1773.
	// Disregard all the access rules, just add the Access Point.
	// For user functions that do the same when adding probes, e.g. Navigator Place Probe function.

	// Just incrementing the count makes the Navigator update as desired.
	// Not actually creating the rest of the DFT data keeps Probe Placement from using
	// the maverick access points. Have not yet found where this would cause a problem.

	if (accessPointData != NULL && accessPointData->getInsert() != NULL)
	{
		InsertStruct *accessInsert = accessPointData->getInsert();

		ETestSurface accessSurface = (accessInsert->getPlacedTop() ? testSurfaceTop : testSurfaceBottom);

      if (accessSurface == testSurfaceTop)
         m_iTopAccessibleCount++;
      else
			m_iBotAccessibleCount++;
	}
}

void CAANetAccess::PlaceNetAccess(int &accessCount, BlockStruct *testAccessBlock, CMapWordToPtr* accessPointerMap, CRetainedProbeMap& retainedProbeMap)
{
   int testAccessKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeTestAccess);
   int testAccessFailTopKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeTestAccessFailTop);
   int testAccessFailBottomKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeTestAccessFailBottom);
   int dataLinkKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeDataLink);
   int probePlacementKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeProbePlacement);


   if (m_iTopAccessibleCount == 0 && m_iBotAccessibleCount == 0)
   {
      // Add drc for net that has no access
      drc_net_without_access(m_pDoc, m_pPCBFile, m_sNetName);
   }

   for (POSITION pos = m_accessibleLocationList.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAAAccessLocation* accLoc = NULL;
      m_accessibleLocationList.GetNextAssoc(pos, key, accLoc);

      if (accLoc == NULL)
         continue;
         
      ETestSurface accessSurface = accLoc->GetAccessSurface();

      int dataLink = 0;
      CAttributes **attribMap = NULL;

      if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
      {
         CompPinStruct *compPin = accLoc->getFeatureEntity().getCompPin();
         dataLink = compPin->getEntityNumber();
         attribMap = &compPin->getAttributesRef();
      }
      else if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
      {
         DataStruct *data = accLoc->getFeatureEntity().getData();
         dataLink = data->getEntityNumber();
         attribMap = &data->getAttributesRef();
      }

      // Udpate the ATT_TESTACCESS and ATT_TESTACCESS_FAIL
      // These two attribute is being used by writer to determine if a feature (via or comppin) is accessible or not
      // ATT_TESTACCESS can have a value of "NONE", "TOP", "BOTTOM", "BOTH"
      // ATT_TESTACCESS_FAIL is the error message when ATT_TESTACCESS is "NONE"
      {
         CString preTestAccess;
         if ((*attribMap))
         {
            Attrib *testAccessAttrib = NULL;
            if ((*attribMap)->Lookup((WORD)testAccessKw, testAccessAttrib))
               preTestAccess = testAccessAttrib->getStringValue();
         }

         if (accLoc->GetAccessSurface() == testSurfaceTop)
         {
            if (accLoc->GetAccessible() == false)
               m_pDoc->SetAttrib(attribMap, testAccessFailTopKw, accLoc->GetErrorMessage(), SA_OVERWRITE, NULL);
            else
               RemoveAttrib((WORD)testAccessFailTopKw, attribMap);
         }
         else
         {
            if (accLoc->GetAccessible() == false)
               m_pDoc->SetAttrib(attribMap, testAccessFailBottomKw, accLoc->GetErrorMessage(), SA_OVERWRITE, NULL);
            else
               RemoveAttrib((WORD)testAccessFailBottomKw, attribMap);
         }

         CString curTestAccess;
         if (accLoc->GetAccessible() == false)
         {
            curTestAccess = DFT_TESTACCESS_NONE;
         }
         else if (accessSurface == testSurfaceTop)
         {
            curTestAccess = DFT_TESTACCESS_TOP;
         }
         else if (accessSurface == testSurfaceBottom)
         {
            curTestAccess = DFT_TESTACCESS_BOTTOM;
         }

         if (curTestAccess.IsEmpty() || (curTestAccess.CompareNoCase(DFT_TESTACCESS_NONE) == 0 && !preTestAccess.IsEmpty()))
         {
            // Current test access is empty or NONE and there is a previous test access for the same feature
            // so assign the previous access ot the feature
            curTestAccess = preTestAccess;
         }
         else if (!preTestAccess.IsEmpty() && preTestAccess.CompareNoCase(DFT_TESTACCESS_NONE) != 0)
         {
            // There is a previous test access on the same feature and it is not empty or NONE
            // so check it against the current test access to see if this feature is accessible from both surface

            if ((!preTestAccess.CompareNoCase(DFT_TESTACCESS_TOP) && !curTestAccess.CompareNoCase(DFT_TESTACCESS_BOTTOM)) ||
               (!preTestAccess.CompareNoCase(DFT_TESTACCESS_BOTTOM) && !curTestAccess.CompareNoCase(DFT_TESTACCESS_TOP)))
            {
               // Feature is accessible from both surface
               curTestAccess = DFT_TESTACCESS_BOTH;
            }
         }

         m_pDoc->SetAttrib(attribMap, testAccessKw, curTestAccess, SA_OVERWRITE, NULL);

         //if (!curTestAccess.CompareNoCase(DFT_TESTACCESS_NONE))
         //   m_pDoc->SetAttrib(attribMap, testAccessFailKey, accLoc->GetErrorMessage(), SA_OVERWRITE, NULL);
         //else
         //   RemoveAttrib(testAccessFailKey, attribMap);
      }   

      if (accLoc->GetAccessible() == false)
      {
         // Do not place access if accessible is false
         continue;
      }

      CString refname = accLoc->GetName();
      if (refname.IsEmpty() || refname == "??")
         refname.Format("$$ACCESS_%d",++accessCount);

      CString netName = accLoc->GetNetName();
      CString targetType = accLoc->GetTargetTypeToString();
      double exposeMetalDiameter = accLoc->GetExposedMetalDiameter();
      double x = accLoc->GetAccessLocation().x;
      double y = accLoc->GetAccessLocation().y;

      if (x == DBL_MAX && y == DBL_MAX)
      {
         //ErrorMessage("Invalid access location for access point", "Error");
      }
      else
      {
         DataStruct* accData = PlaceTestAccessPoint(m_pDoc, m_pPCBFile->getBlock(), refname, x, y, accessSurface, netName, targetType, 
                                  dataLink, exposeMetalDiameter, m_pDoc->getSettings().getPageUnits(),accLoc->getId(),accLoc->getExposedDataId());

         accLoc->SetName(refname);

         if (accData != NULL)
         {
            if (accessPointerMap != NULL)
               accessPointerMap->SetAt((int)accData->getEntityNumber(), accData);

            DataStruct* probe = retainedProbeMap.findProbe(dataLink, accData->getInsert()->getGraphicMirrored());
            if (probe != NULL)
            {
               // Find the probe and reconnected it to Test Access point
               long probeDataLink = accData->getEntityNumber();
               probe->setAttrib(m_pDoc->getCamCadData(), dataLinkKw, valueTypeInteger, (void*)&probeDataLink, attributeUpdateOverwrite, NULL);
               probe->setAttrib(m_pDoc->getCamCadData(), probePlacementKw, valueTypeString, "Placed", attributeUpdateOverwrite, NULL);

               // Update the probe location to where the access point is
               probe->getInsert()->setOrigin(accData->getInsert()->getOrigin());
               probe->setHidden(false);
            }
         }
      }
   }
}

void CAANetAccess::WriteNetAccess(CFormatStdioFile &file)
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example of how the Test Access section looks like
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TEST ACCESS: Multiple Pin Nets
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ============================================================================================================
// NET                  : AGND
// TOP ACCESS POINTS    : 76
// BOTTOM ACCESS POINTS : 44
//
// ACCESSIBLE LOCATIONS
//     C16.P               TOP        6.384     3.950     0.040     COMPONENT    SMD     
//     C2.1                TOP        5.083     4.900     0.040     COMPONENT    SMD     
//
// INACCESSIBLE LOCATIONS
//     TP32.1              BOTTOM     7.100     5.900     0.000     COMPONENT    SMD     Exposed metal is smaller than feature size
//     TP47.1              BOTTOM     6.300     6.000     0.000     COMPONENT    SMD     Exposed metal is smaller than feature size
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   if (m_accessibleLocationList.GetCount() == 0)
      return;

   file.WriteString("============================================================================================================\n");
   file.WriteString("NET                  : %s\n", m_sNetName);

   if (m_pTestPlan->GetAAWriteIgnoreSurface() == true || m_pTestPlan->GetSurface() == testSurfaceBoth)
   {
      file.WriteString("TOP ACCESS POINTS    : %d\n", m_iTopAccessibleCount);
      file.WriteString("BOTTOM ACCESS POINTS : %d\n", m_iBotAccessibleCount);
   }
   else
   {
      if (m_pTestPlan->GetSurface() == testSurfaceTop)
         file.WriteString("TOP ACCESS POINTS    : %d\n", m_iTopAccessibleCount);
      else
         file.WriteString("BOTTOM ACCESS POINTS : %d\n", m_iBotAccessibleCount);
   }
   
   if (this->GetTopAccessibleCount() > 0 || this->GetBotAccessiblecount() > 0)
   {
      // Only write if net has some accessible location
      // Write out all the accessible location first
      file.WriteString("\nACCESSIBLE LOCATIONS\n");
      WriteAccessLocation(file, true, entityTypeCompPin);
      WriteAccessLocation(file, true, entityTypeData);
   }

   // Then write out all the not accessible location next
   file.WriteString("\nINACCESSIBLE LOCATIONS\n");
   WriteAccessLocation(file, false, entityTypeCompPin);
   WriteAccessLocation(file, false, entityTypeData);

   file.WriteString("\n");
}

void CAANetAccess::WriteAccessLocation(CFormatStdioFile &file, bool accessible, EEntityType entityType)
{
   m_accessibleLocationList.Sort();

   int decimals = GetDecimals(m_pDoc->getPageUnits());

   CString* key;
   CAAAccessLocation* accLoc = NULL;
   for (m_accessibleLocationList.GetFirstSorted(key, accLoc); accLoc != NULL;)
   {

      if (accLoc->GetAccessible() == accessible && accLoc->getFeatureEntity().getEntityType() == entityType)
      {
         if (m_pTestPlan->GetAAWriteIgnoreSurface() || m_pTestPlan->GetSurface() == testSurfaceBoth ||
            m_pTestPlan->GetSurface() == accLoc->GetAccessSurface())
         {
            CString refdes;
            CString surface = "NONE";
            CString x;
            CString y;
            CString metalDiamter;
            CString featureType;
            CString technology;

            if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            {
               DataStruct *via = accLoc->getFeatureEntity().getData();
               InsertStruct *insert = via->getInsert();
               refdes = insert->getRefname();
               x.Format("%0.*f", decimals, insert->getOrigin().x);
               y.Format("%0.*f", decimals, insert->getOrigin().y);
               featureType = "VIA";
            }
            else if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            {
               CompPinStruct *compPin = accLoc->getFeatureEntity().getCompPin();
               refdes = compPin->getPinRef();
               x.Format("%0.*f", decimals, compPin->getOrigin().x);
               y.Format("%0.*f", decimals, compPin->getOrigin().y);
               featureType = "COMPONENT";
            }
            
            metalDiamter.Format("%0.*f", decimals, accLoc->GetExposedMetalDiameter());

            if (accLoc->GetAccessSurface() == testSurfaceTop)
               surface = "TOP";
            else if (accLoc->GetAccessSurface() == testSurfaceBottom)
               surface = "BOTTOM";

            if (accLoc->GetTargetType() & DFT_TARGETTYPE_THRUHOLE)
               technology = "THRU";
            else
               technology = "SMD";

            if (accessible == true)
            {
               file.WriteString("%*s%-*s%-*s%*s%*s%*s%*s%-*s%-*s\n", 4, "", 20, refdes, 6, surface, 10, x, 10, y,
                        10, metalDiamter, 5, "", 13, featureType, 8, technology);
            }
            else
            {
               file.WriteString("%*s%-*s%-*s%*s%*s%*s%*s%-*s%-*s%s\n", 4, "", 20, refdes, 6, surface, 10, x, 10, y,
                        10, metalDiamter, 5, "", 13, featureType, 8, technology, accLoc->GetErrorMessage());
            }
         }
      }

      m_accessibleLocationList.GetNextSorted(key, accLoc);
   }
}

int CAANetAccess::GetTargetType(DataStruct *data, CompPinStruct *compPin)
{
   int targetType = DFT_TARGETTYPE_UNKNOWN;

   //BlockStruct *block = NULL;
   if (data != NULL && data->getInsert()->getInsertType() == insertTypeTestPoint)
   {
      targetType = DFT_TARGETTYPE_TESTATTRIBUTE;
      targetType |= CheckAttribForTargetType(data->getAttributes());
   }
   else if (data != NULL && (data->getInsert()->getInsertType() == insertTypeVia || 
                             data->getInsert()->getInsertType() == insertTypeTestPad) )
   {
      //if (data->getEntityNumber() == 616)
      //{
      //   int iii = 3;
      //}

      //block = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());
      targetType = DFT_TARGETTYPE_VIA;
      targetType |= CheckAttribForTargetType(data->getAttributes());
   }
   else if (compPin != NULL)
   {
      //block = m_pDoc->getBlockAt(compPin->getPadstackBlockNumber());
      targetType = CheckAttribForTargetType(compPin->getAttributes());

      if (data != NULL)
      {
         int compTargetType = CheckAttribForTargetType(data->getAttributes());
         targetType |= compTargetType & DFT_TARGETTYPE_TESTATTRIBUTE;
         targetType |= compTargetType & DFT_TARGETTYPE_CONNECTOR;
      }
   }

   return targetType;
}

int CAANetAccess::CheckAttribForTargetType(CAttributes *attribMap)
{
   if (attribMap == NULL)
      return DFT_TARGETTYPE_UNKNOWN;

   int testKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeTest);
   int deviceTypekw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeDeviceType);
   int technologyKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeTechnology);
   int targetType = DFT_TARGETTYPE_UNKNOWN;
   Attrib *attrib = NULL;

   if (attribMap->Lookup((WORD)testKw, attrib))
      targetType = targetType | DFT_TARGETTYPE_TESTATTRIBUTE;

   if (attribMap->Lookup((WORD)deviceTypekw, attrib))
   {
      CString deviceType = attrib->getStringValue();
      if (!deviceType.CompareNoCase("Connector"))
         targetType |= DFT_TARGETTYPE_CONNECTOR;
   }

   if (attribMap->Lookup((WORD)technologyKw, attrib))
   {
      CString technology = attrib->getStringValue();
      if (technology.CompareNoCase("THRU") == 0)
         targetType |= DFT_TARGETTYPE_THRUHOLE;
      else
         targetType |= DFT_TARGETTYPE_SMD;
   }

   return targetType;
}

void CAANetAccess::UpdateCADPadstackTargetType()
{
   for (POSITION pos = m_accessibleLocationList.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAAAccessLocation* accessLocation = NULL;
      m_accessibleLocationList.GetNextAssoc(pos, key, accessLocation);
      if (accessLocation == NULL)
         continue;
      
      int targetType = DFT_TARGETTYPE_UNKNOWN;
      int geomNum = -1;
      ETestSurface surface = testSurfaceTop;

      if (accessLocation->getFeatureEntity().getEntityType() == entityTypeData)
      {
         DataStruct *via = accessLocation->getFeatureEntity().getData();
         geomNum = via->getInsert()->getBlockNumber();
         // dts0100594084  Specific CAD Padstack in Access Analysis of CAMCAD not working
         // That was because vias are almost always inserted on top layers, and in this
         // case it was a via with a padstack with only one pad and it was on bottom layer.
         // This particular via was essentially really a testpad, not a via at all.
         // So basically, due to the general nature of vias, we are going to ignore the
         // insert surface of it, and let it get treated by both sets of rules regardless
         // of via insert side.
         // ORIGINAL -> DOES NOT WORK:  surface = via->getInsert()->getGraphicMirrored()?testSurfaceBottom:testSurfaceTop;
         surface = testSurfaceBoth;
      }
      else if (accessLocation->getFeatureEntity().getEntityType() == entityTypeCompPin)
      {
         CompPinStruct *compPin = accessLocation->getFeatureEntity().getCompPin();
         geomNum = compPin->getPadstackBlockNumber();
         surface = compPin->getMirror()?testSurfaceBottom:testSurfaceTop;
      }

      if (geomNum > -1)
      {
         // Check to see if it is in the CADPadstack list of the test plan

         if (surface == testSurfaceTop || surface == testSurfaceBoth)
         {
            if (m_pTestPlan->GetTopTargetTypes()->HasPadstackInList(geomNum))
            {
               accessLocation->AddTargetType(DFT_TARGETTYPE_CADPADSTACK);
            }
         }

         if (surface == testSurfaceBottom || surface == testSurfaceBoth)
         {
            if (m_pTestPlan->GetBotTargetTypes()->HasPadstackInList(geomNum))
            {
               accessLocation->AddTargetType(DFT_TARGETTYPE_CADPADSTACK);  // okay if both top and bot put it in
            }
         }



      }
   }
}

void CAANetAccess::ResetNetAccess()
{
   m_iTopAccessibleCount = 0;
   m_iBotAccessibleCount = 0;
   m_topFeatureLost.Reset();
   m_botFeatureLost.Reset();

   for (POSITION pos =  m_accessibleLocationList.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAAAccessLocation* accessLocation = NULL;
      m_accessibleLocationList.GetNextAssoc(pos, key, accessLocation);

      if (accessLocation)
         accessLocation->ResetAccessLocation();
   }
}

CString CAANetAccess::generateSortKey(CString name) const   
{ 
   return GenerateSortKey(name);                         
}

//-----------------------------------------------------------------------------
// CAANetResultCount
//-----------------------------------------------------------------------------
CAANetResultCount::CAANetResultCount()
{ 
   Reset();
};

CAANetResultCount::CAANetResultCount(CAANetResultCount &resultCount)
{
   *this = resultCount;
}

CAANetResultCount::~CAANetResultCount()
{
}

CAANetResultCount& CAANetResultCount::operator=(const CAANetResultCount &resultCount)
{
   if (&resultCount != this)
   {
      m_iMulPinNetTot   = resultCount.m_iMulPinNetTot;
      m_iMulPinNetAcc   = resultCount.m_iMulPinNetAcc;
      m_iSngPinNetTot   = resultCount.m_iSngPinNetTot;
      m_iSngPinNetAcc   = resultCount.m_iSngPinNetAcc;
      m_iNCPinNetTot    = resultCount.m_iNCPinNetTot;
      m_iNCPinNetAcc    = resultCount.m_iNCPinNetAcc;
      m_iAllNetTot      = resultCount.m_iAllNetTot;
      m_iAllNetAcc      = resultCount.m_iAllNetAcc;
   }

   return *this;
}

CAANetResultCount CAANetResultCount::operator+(const CAANetResultCount &resultCount)
{
   CAANetResultCount r;

   r.m_iMulPinNetTot = m_iMulPinNetTot + resultCount.m_iMulPinNetTot;
   r.m_iMulPinNetAcc = m_iMulPinNetAcc + resultCount.m_iMulPinNetAcc;
   r.m_iSngPinNetTot = m_iSngPinNetTot + resultCount.m_iSngPinNetTot;
   r.m_iSngPinNetAcc = m_iSngPinNetAcc + resultCount.m_iSngPinNetAcc;
   r.m_iNCPinNetTot  = m_iNCPinNetTot  + resultCount.m_iNCPinNetTot;
   r.m_iNCPinNetAcc  = m_iNCPinNetAcc  + resultCount.m_iNCPinNetAcc;
   r.m_iAllNetTot    = m_iAllNetTot    + resultCount.m_iAllNetTot;
   r.m_iAllNetAcc    = m_iAllNetAcc    + resultCount.m_iAllNetAcc;

   return r;
}

void CAANetResultCount::Reset()
{
   m_iMulPinNetTot = 0;
   m_iMulPinNetAcc = 0;
   m_iSngPinNetTot = 0;
   m_iSngPinNetAcc = 0;
   m_iNCPinNetTot = 0;
   m_iNCPinNetAcc = 0;
   m_iAllNetTot = 0;
   m_iAllNetAcc = 0;
}

//-----------------------------------------------------------------------------
// CAccessAnalysisSolution
//-----------------------------------------------------------------------------
CAccessAnalysisSolution::CAccessAnalysisSolution(CCEtoODBDoc *doc, FileStruct *PCBFile, CTestPlan *testPlan)
{
   m_pDoc = doc;
   m_pPCBFile = PCBFile;
   m_pTestPlan = testPlan;
   m_netAccessMap.setSortFunction(&CAccessAnalysisSolution::NetSortFunction);
}

CAccessAnalysisSolution::CAccessAnalysisSolution(const CAccessAnalysisSolution& other, CTestPlan *testPlan)
{
   RemoveNetAccess(NULL);

   m_pDoc = other.m_pDoc;
   m_pPCBFile = other.m_pPCBFile;
   m_pTestPlan = testPlan;
   m_netAccessMap.setSortFunction(&CAccessAnalysisSolution::NetSortFunction);

   POSITION pos = other.m_netAccessMap.GetStartPosition();
   while (pos != NULL)
   {
      CString netName;
      CAANetAccess* netAccess = NULL;
      other.m_netAccessMap.GetNextAssoc(pos, netName, (CObject*&)netAccess); 
      if (netAccess == NULL)
         continue;

      // Make a copy of the netAccess
      CAANetAccess *newNetAccess = new CAANetAccess(*netAccess, testPlan);
      m_netAccessMap.SetAt(newNetAccess->GetNetName(), newNetAccess);
   }
}

CAccessAnalysisSolution::~CAccessAnalysisSolution()
{
   RemoveNetAccess(NULL);
}

void CAccessAnalysisSolution::ApplyToPcb(FileStruct *pcaFile)
{
   if (pcaFile != NULL)
   {
      POSITION netPos = pcaFile->getHeadNetPosition();
      while (netPos != NULL)
      {
         NetStruct *net = pcaFile->getNextNet(netPos);
         if (net != NULL)
         {
            CAANetAccess *netAccess = this->GetNetAccess(net->getNetName());
            if (netAccess != NULL)
            {
               netAccess->ApplyToNet(net, pcaFile);
            }
         }
      }
   }
}

void CAccessAnalysisSolution::RemoveNetAccess(CExposedDataMap *exposedDataMap)
{
   POSITION pos = m_netAccessMap.GetStartPosition();
   while (pos)
   {
      CString netName;
      CAANetAccess *netAccess = NULL;
      m_netAccessMap.GetNextAssoc(pos, netName, (CObject*&)netAccess);
      delete netAccess;
      netAccess = NULL;
   }
   m_netAccessMap.RemoveAll();

   if (exposedDataMap != NULL)
      exposedDataMap->RemoveAll();
}

int CAccessAnalysisSolution::CreateNetAccess(CMetalAnalysis& metalAnalysis, CExposedDataMap *exposedDataMap, bool breakOnError,
															NetAccessTypeTag netAccessType, COperationProgress* progress)
{
   int res = DFT_ERROR_NONE;
   
   CComponentMap componentMap(false);
   int viaNameCount = 0;
   int bondpadNameCount = 0;
   //int testPointCount = 0;
   int testPadCount = 0;
   WORD netNameKey = m_pDoc->IsKeyWord(ATT_NETNAME, 0);


	if (progress != NULL)
	{
		progress->updateStatus("Preparing Data for Access Analysis: Gathering vias ...", m_pPCBFile->getBlock()->getDataCount());
	}

   for (CDataListIterator insertIterator(*(m_pPCBFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      if (progress != NULL) progress->incrementProgress();
      DataStruct* data = insertIterator.getNext();
      InsertTypeTag insertType = data->getInsert()->getInsertType();

      if (insertType == insertTypePcbComponent || insertType == insertTypeTestPoint)
      {
         componentMap.SetAt(data->getInsert()->getRefname(), data);
      }
      else if (insertType == insertTypeVia || insertType == insertTypeTestPad || insertType == insertTypeBondPad)
      {
         // Make the via has a net name
         Attrib *attrib = NULL;
         if (data->getAttributesRef() == NULL || !data->getAttributes()->Lookup(netNameKey, attrib) || attrib == NULL) 
            continue;

         CString netName = attrib->getStringValue();
         if (netName.IsEmpty())
            continue;

         bool netIsNonConnected = netName.Left(strlen("NC__")).CompareNoCase("NC__") == 0;
         if (netAccessType == netAccessAll || 
				(netAccessType == netAccessNonConnectedOnly && netIsNonConnected) ||
				(netAccessType == netAccessExcludeNonConnected && !netIsNonConnected))
         {
            // Create via refdes if there is not one
            if (data->getInsert()->getRefname().IsEmpty())
            {
               CString name;
               if (insertType == insertTypeVia)
                  name.Format("via_%d", ++viaNameCount);
               else if (insertType == insertTypeBondPad)
                  name.Format("bondpad_%d", ++bondpadNameCount);
               else
                  name.Format("testpad_%d", ++testPadCount);

               data->getInsert()->setRefname(name);
            }

            // Add access location of via
            CAANetAccess *netAccess = GetNetAccess(netName);
            if ((res = netAccess->AddViaAccessLocation(metalAnalysis, data, exposedDataMap)) < 0)
               break;
         }
      }
   }

   if (res >= 0)
   {
		if (progress != NULL)
		{
			progress->updateStatus("Preparing Data for Access Analysis: Gathering comppins ...", m_pPCBFile->getNetCount());
		}

      // Add access locations from net list
      for (POSITION pos = m_pPCBFile->getNetList().GetHeadPosition();pos != NULL;)
      {
			if (progress != NULL ) progress->incrementProgress();

         NetStruct *net = m_pPCBFile->getNetList().GetNext(pos);
         if (net == NULL)
            continue;

         bool netIsNonConnected = net->getNetName().Left(strlen("NC__")).CompareNoCase("NC__") == 0;
         if (netAccessType == netAccessAll || 
				(netAccessType == netAccessNonConnectedOnly && netIsNonConnected) ||
				(netAccessType == netAccessExcludeNonConnected && !netIsNonConnected))
         {
            CAANetAccess *netAccess = GetNetAccess(net->getNetName());
            res = netAccess->AddNetListAccessLocation(metalAnalysis, net,componentMap, exposedDataMap, breakOnError);

            if (res < 0 && breakOnError)
               break;
         }
      }
   }

   if (res == DEF_ERROR_COMPPIN_LOCATION)
   {
      // Do not continue if there is a comppin that do not have its cordinate completed
      // Ask the users if they want to do "Generate Pin Location Data"
      CString errorMsg;
      errorMsg.Format("Comppin locations are invalid.  Do you want to do \"Generate Pin Location Data\" and continue?\n");

      if (MessageBox(NULL, errorMsg, "Acccess Analysis - Error: Comppin location invalid", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
      {
         m_pDoc->OnGeneratePinloc();
         res = CreateNetAccess(metalAnalysis, exposedDataMap, false, netAccessType, progress);
      }
   }

	if (netAccessType == netAccessNonConnectedOnly)
	{
		// Created non-connected net for non-connected pin so remove the net of NET_UNUSED_PINS

      CAANetAccess *netAccess = NULL;
		if (m_netAccessMap.Lookup(NET_UNUSED_PINS, (CObject*&)netAccess) && netAccess != NULL)
		{
			m_netAccessMap.RemoveKey(NET_UNUSED_PINS);
			delete netAccess;
			netAccess = NULL;
		}
	}

   return res;
}

void CAccessAnalysisSolution::ProcessAccessAnalysis(CMetalAnalysis& metalAnalysis, CExposedDataMap *exposedDataMap,const CAccessOffsetOptions& accessOffsetOptions, double tolerance)
{
   m_topNetResultCount.Reset();
   m_botNetResultCount.Reset();  
   m_bothNetResultCount.Reset();
   m_totalNetResultCount.Reset();
   m_topFeatureLost.Reset();
   m_botFeatureLost.Reset();


   // dts0100570229 - Bug was caused by switch to Real Part as preferred outline carrier, but did not
   // aslo put the COMPHEIGHT attrib on Real Part's Package. This was interpreted as no height set and
   // so height processing took place. The PCB Component is still the likely place that user will set
   // COMPHEIGHT, so just propagate it to the Real Parts on the fly, every time.
   ResetRealPartPkgAttribCOMPHEIGHT(this->m_pDoc, this->m_pPCBFile->getBlock());

   // Analize all the NetAcess
   POSITION pos = m_netAccessMap.GetStartPosition();
   while (pos)
   {
      CString netName;

      CAANetAccess *netAccess = NULL;
      m_netAccessMap.GetNextAssoc(pos, netName, (CObject*&)netAccess);

      if (netAccess)
      {
         netAccess->AnalizeNetAccess(metalAnalysis, exposedDataMap, accessOffsetOptions, tolerance);

         // Get the result count
         CAANetResultCount *netResultCount;
         if (netAccess->GetSurface() == testSurfaceTop)
            netResultCount = &m_topNetResultCount;
         else if (netAccess->GetSurface() == testSurfaceBottom)
            netResultCount = &m_botNetResultCount;
         else if (netAccess->GetSurface() == testSurfaceBoth)
            netResultCount = &m_bothNetResultCount;
         else
            continue;

         int count = netAccess->GetTopAccessibleCount() + netAccess->GetBotAccessiblecount();
         
         if (netAccess->GetIncludeNetType() == includeMultiPinNet)
         {
            if (count > 0)
               netResultCount->m_iMulPinNetAcc++;
            netResultCount->m_iMulPinNetTot++;
         }
         else if (netAccess->GetIncludeNetType() == includeSinglePinNet)
         {
            if (count > 0)
               netResultCount->m_iSngPinNetAcc++;
            netResultCount->m_iSngPinNetTot++;
         }
         else if (netAccess->GetIncludeNetType() == includeUnconnecedNet)
         {
            if (count > 0)
               netResultCount->m_iNCPinNetAcc++;
            netResultCount->m_iNCPinNetTot++;
         }

         if (count > 0)
            netResultCount->m_iAllNetAcc++;
         netResultCount->m_iAllNetTot++;

         // Get the feature lost on net
         m_topFeatureLost = m_topFeatureLost + netAccess->GetTopFeatureLost();
         m_botFeatureLost = m_botFeatureLost + netAccess->GetBotFeatureLost();
      }
   }

   m_totalNetResultCount = m_topNetResultCount + m_botNetResultCount + m_bothNetResultCount;
}

void CAccessAnalysisSolution::PlaceAccessSolution(CMapWordToPtr* accessPointerMap)
{
   int accesscnt = 0;
   double size = 0.03 * Units_Factor(DFT_DEFAULT_UNIT, m_pDoc->getSettings().getPageUnits());
   BlockStruct *testaccessblock =  generate_TestAccessGeometry(m_pDoc, "TEST_ACCESS", size);

   CRetainedProbeMap retainedProbeMap;
   if (m_pTestPlan->GetRetainedExistingProbe())
   {
      retainedProbeMap.collectProbes(*m_pDoc, *m_pPCBFile);

      RemoveTestAccessAttributes(m_pDoc, m_pPCBFile);
      DeleteAllTestAccessPoints(m_pDoc, m_pPCBFile);
   }
   else
   {
      RemoveTestAccessAttributes(m_pDoc, m_pPCBFile);
      DeleteAllTestAccessPoints(m_pDoc, m_pPCBFile);
      DeleteAllProbes(m_pDoc, m_pPCBFile);   
   }

	POSITION pos = m_netAccessMap.GetStartPosition();
   while (pos)
   {
      CString netName;
      CAANetAccess *netAccess = NULL;
      m_netAccessMap.GetNextAssoc(pos, netName, (CObject*&)netAccess);
      
      if (netAccess)
         netAccess->PlaceNetAccess(accesscnt, testaccessblock, accessPointerMap, retainedProbeMap);
   }
}

bool CAccessAnalysisSolution::CreateAccessAnalysisReport(CString Filename)
{
   CFormatStdioFile file;
   CFileException err;
   CString tempBuf;

   if (!file.Open(Filename, CFile::modeCreate|CFile::modeWrite, &err))
      return false;

   if (m_netAccessMap.GetCount() == 0)
      return false;

   m_netAccessMap.Sort();
   int indent = 0;

   // Write header
   file.WriteString("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   file.WriteString("Access Analysis Report\n");
   file.WriteString("\n");
   file.WriteString("Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   file.WriteString("Version     : %s\n", getApp().getVersionString());
   CTime t = t.GetCurrentTime();
   file.WriteString("Date & Time : %s\n", t.Format("%A, %B %d, %Y at %H:%M:%S"));
   file.WriteString("PCB File    : %s\n", m_pPCBFile->getName());
   file.WriteString("Units       : %s\n", GetUnitName(m_pDoc->getSettings().getPageUnits()));
   file.WriteString("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

   // Write test plan settings
   file.WriteString("\n\n\n");
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString("TEST PLAN SETTINGS\n");
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   m_pTestPlan->WriteAAReport(file, 0);

   // Write the summary
   int totalWithAccCnt = 0;
   int zeroAccCnt = 0;
   int oneAccCnt = 0;
   int twoAccCnt = 0;
   int moreAccCnt = 0;

   file.WriteString("\n\n\n");
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString("SUMMARY:                      Nets with >0    Nets with 0     Nets with 1      Net with 2      Net with >2\n");      
   file.WriteString("                             Access Points   Access Points   Access Point     Access Points   Access Points\n");     
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   WriteNetSummary(file, includeMultiPinNet, totalWithAccCnt, zeroAccCnt, oneAccCnt, twoAccCnt, moreAccCnt);
   WriteNetSummary(file, includeSinglePinNet, totalWithAccCnt, zeroAccCnt, oneAccCnt, twoAccCnt, moreAccCnt);
   WriteNetSummary(file, includeUnconnecedNet, totalWithAccCnt, zeroAccCnt, oneAccCnt, twoAccCnt, moreAccCnt);

   // Write total of the summary
   int totalCnt = zeroAccCnt + oneAccCnt + twoAccCnt + moreAccCnt;

   CString totalWithAcc = "0(0.00%)";
   CString zeroAcc = "0(0.00%)";
   CString oneAcc = "0(0.00%)";
   CString twoAcc = "0(0.00%)";
   CString moreAcc = "0(0.00%)";

   if (totalCnt > 0)
   {
      totalWithAcc.Format("%d(%0.2f%%)", totalWithAccCnt, (double)totalWithAccCnt/(double)totalCnt * 100);
      zeroAcc.Format("%d(%0.2f%%)", zeroAccCnt, (double)zeroAccCnt/(double)totalCnt * 100);
      oneAcc.Format("%d(%0.2f%%)", oneAccCnt, (double)oneAccCnt/(double)totalCnt * 100);
      twoAcc.Format("%d(%0.2f%%)", twoAccCnt, (double)twoAccCnt/(double)totalCnt * 100);
      moreAcc.Format("%d(%0.2f%%)", moreAccCnt, (double)moreAccCnt/(double)totalCnt * 100);
   }

   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString("%-*s%*s%*s%*s%*s%*s\n", 25, "Total Nets", 16, totalWithAcc, 16, zeroAcc, 16, oneAcc, 16, twoAcc, 16, moreAcc);
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString("\n\n\n");

   // Write INACCESSIBLE Nets 
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString("TEST ACCESS: INACCESSIBLE NETS\n");
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   WriteNetTestAcess(file, includeMultiPinNet, true);
   WriteNetTestAcess(file, includeSinglePinNet, true);
   WriteNetTestAcess(file, includeUnconnecedNet, true);
   file.WriteString("\n\n\n");

   // Write ACCESSIBLE Nets
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString("TEST ACCESS: ACCESSIBLE NETS\n");
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   WriteNetTestAcess(file, includeMultiPinNet, false);
   WriteNetTestAcess(file, includeSinglePinNet, false);
   WriteNetTestAcess(file, includeUnconnecedNet, false);
   
   file.Close();
   return true;
}

void CAccessAnalysisSolution::WriteNetSummary(CFormatStdioFile &file, EIncludedNetType netType, int &totalWithAccCnt,
                                              int &zeroAccCnt, int &oneAccCnt, int &twoAccCnt, int &moreAccCnt)
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example of how the summary looks like
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//SUMMARY:                       Nets with >0    Nets with 0     Nets with 1      Net with 2      Net with >2      
//                              Access Points   Access Points   Access Point    Access Points   Access Points     
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Multiple Pin Nets              76(32.48%)       13(5.56%)      59(25.21%)      86(36.75%)    234(100.00%)
// Single Pin Nets              115(100.00%)        0(0.00%)        0(0.00%)        0(0.00%)    115(100.00%)
// Unconnected Nets                 0(0.00%)        0(0.00%)        0(0.00%)        0(0.00%)        0(0.00%)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Get the counts
   int tmpTotalWithAccCnt = 0;
   int tmpZeroAccCnt = 0;
   int tmpOneAccCnt = 0;
   int tmpTwoAccCnt = 0;
   int tmpMoreAccCnt = 0;
   int tmpTotalCnt = 0;
   
   POSITION pos = m_netAccessMap.GetStartPosition(); 
   while (pos)
   {
      CString key;
      CAANetAccess *netAccess = NULL;
      m_netAccessMap.GetNextAssoc(pos, key, (CObject*&)netAccess);
      if (netAccess == NULL || netAccess->GetIncludeNetType() != netType)
         continue;

      int count = netAccess->GetTopAccessibleCount() + netAccess->GetBotAccessiblecount();
      if (count == 0)
         tmpZeroAccCnt++;
      else if (count == 1)
         tmpOneAccCnt++;
      else if (count == 2)
         tmpTwoAccCnt++;
      else if (count > 0)
         tmpMoreAccCnt++;

      tmpTotalCnt++;
   }
   tmpTotalWithAccCnt = tmpTotalCnt - tmpZeroAccCnt;

   // Write the summary
   CString totalWithAcc = "0(0.00%)";
   CString zeroAcc = "0(0.00%)";
   CString oneAcc = "0(0.00%)";
   CString twoAcc = "0(0.00%)";
   CString moreAcc = "0(0.00%)";

   if (tmpTotalCnt > 0)
   {
      totalWithAcc.Format("%d(%0.2f%%)", tmpTotalWithAccCnt, (double)tmpTotalWithAccCnt/(double)tmpTotalCnt * 100);
      zeroAcc.Format("%d(%0.2f%%)", tmpZeroAccCnt, (double)tmpZeroAccCnt/(double)tmpTotalCnt * 100);
      oneAcc.Format("%d(%0.2f%%)", tmpOneAccCnt, (double)tmpOneAccCnt/(double)tmpTotalCnt * 100);
      twoAcc.Format("%d(%0.2f%%)", tmpTwoAccCnt, (double)tmpTwoAccCnt/(double)tmpTotalCnt * 100);
      moreAcc.Format("%d(%0.2f%%)", tmpMoreAccCnt, (double)tmpMoreAccCnt/(double)tmpTotalCnt * 100);
   }

   CString summaryName;
   if (netType == includeMultiPinNet) 
      summaryName = "Multiple Pin Nets";
   else if (netType == includeSinglePinNet)
      summaryName = "Single Pin Nets";
   else if (netType == includeUnconnecedNet)
      summaryName = "Unconnected Nets";

   file.WriteString("%-*s%*s%*s%*s%*s%*s\n", 25, summaryName, 16, totalWithAcc, 16, zeroAcc, 16, oneAcc, 16, twoAcc, 16, moreAcc);

   // Return the count
   totalWithAccCnt += tmpTotalWithAccCnt;
   zeroAccCnt += tmpZeroAccCnt;
   oneAccCnt += tmpOneAccCnt;
   twoAccCnt += tmpTwoAccCnt;
   moreAccCnt += tmpMoreAccCnt;
}

void CAccessAnalysisSolution::WriteNetTestAcess(CFormatStdioFile &file, EIncludedNetType netType, bool inaccessibleNet)
{
   CString accessName;
   if (netType == includeMultiPinNet) 
      accessName = "TEST ACCESS: Multiple Pin Nets\n";
   else if (netType == includeSinglePinNet)
      accessName = "TEST ACCESS: Single Pin Nets\n";
   else if (netType == includeUnconnecedNet)
      accessName = "TEST ACCESS: Unconnected Nets\n";

   file.WriteString("\n\n");
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   file.WriteString(accessName);
   file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

   CString *netname = NULL;
   CAANetAccess *netAccess = NULL;
   m_netAccessMap.GetFirstSorted(netname, (CObject*&)netAccess);
   while (netAccess != NULL)
   {
      if (netAccess->GetIncludeNetType() == netType)
      {
         if (inaccessibleNet == true)
         {
            // Write INACCESSIBLE NETS
            if (netAccess->GetTopAccessibleCount() == 0 && netAccess->GetBotAccessiblecount() == 0)
               netAccess->WriteNetAccess(file);
         }
         else
         {
            // Write ACCESSIBLE NETS
            if (netAccess->GetTopAccessibleCount() > 0 || netAccess->GetBotAccessiblecount() > 0)
               netAccess->WriteNetAccess(file);
         }
      }
      m_netAccessMap.GetNextSorted(netname, (CObject*&)netAccess);
   }
}

void CAccessAnalysisSolution::DumpToFile(CFormatStdioFile &file, int indent)
{
   POSITION pos = m_netAccessMap.GetStartPosition();
   while (pos)
   {
      CString netName;
      CAANetAccess *netAccess = NULL;
      m_netAccessMap.GetNextAssoc(pos, netName, (CObject*&)netAccess);
      
      if (netAccess)
         netAccess->DumpToFile(file, indent);
   }
}

CAANetAccess* CAccessAnalysisSolution::GetNetAccess(CString netName)
{
   CAANetAccess *netAccess = NULL;
   if (!m_netAccessMap.Lookup(netName, (CObject*&)netAccess))
   {
      netAccess = new CAANetAccess(m_pDoc, m_pPCBFile, m_pTestPlan, netName);
      m_netAccessMap.SetAt(netName, netAccess);
   }

   return netAccess;
}

int CAccessAnalysisSolution::NetSortFunction(const void* elem1, const void* elem2)
{
   SElement *element1 = (SElement*)elem1, *element2 = (SElement*)elem2;

   CAANetAccess *pin1 = (CAANetAccess*)element1->pObject->m_object, *pin2 = (CAANetAccess*)element2->pObject->m_object;

   return pin1->GetSortKey().CompareNoCase(pin2->GetSortKey());
}

void CAccessAnalysisSolution::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<AASolution>\n");

   writeFormat.pushHeader("  ");
   POSITION pos = m_netAccessMap.GetStartPosition();
   while (pos)
   {
      CString netName;
      CAANetAccess* netAccess = NULL;
      m_netAccessMap.GetNextAssoc(pos, netName, (CObject*&)netAccess);

      netAccess->WriteXML(writeFormat, progress);
   }
   writeFormat.popHeader();

   writeFormat.writef("</AASolution>\n");
}

extern bool OdbPpIn_TranslateOdbNetName(CString& netName);

int CAccessAnalysisSolution::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName))
         continue;

      if (nodeName == "NetAccess")
      {
         // Traditional, normal, historic RSI style AA Solution syntax. 
         CString netName;
         if (!subNode->GetAttrValue("Name", netName))
            continue;

         CAANetAccess *netAccess = new CAANetAccess(m_pDoc, m_pPCBFile, m_pTestPlan, netName);
         m_netAccessMap.SetAt(netName, netAccess);

         netAccess->LoadXML(subNode);
      }
      else if (nodeName == "AccessPoint")
      {
         // vPlan interface style syntax


         CAAAccessLocation *accLoc = CAAAccessLocation::LoadXML(subNode);
         
         if (accLoc != NULL)
         {
            CString netName = accLoc->GetNetName();

            // Concession to knowing this is coming from vPlan, hence ODB. Convert the ODB $None$ net name like ODB importer does.
            if (OdbPpIn_TranslateOdbNetName(netName))
               accLoc->SetNetName(netName);

            CAANetAccess *netAccess = NULL;
            if (!m_netAccessMap.Lookup(netName, (CObject*&)netAccess))
            {
               netAccess = new CAANetAccess(m_pDoc, m_pPCBFile, m_pTestPlan, netName);
               m_netAccessMap.SetAt(netName, netAccess);
            }

            netAccess->AddAccessLocation(accLoc);
         }
      }

      delete subNode;
   }

   return 0;
}

//_____________________________________________________________________________
CRetainedProbeMap::CRetainedProbeMap()
: m_retainedProbeMap(nextPrime2n(20), false)
{
}

CRetainedProbeMap::~CRetainedProbeMap()
{
   m_retainedProbeMap.empty();
}

CString CRetainedProbeMap::getKey(const long entityNumber, const bool isMirrored)
{
   CString key;
   key.Format("%d%s", entityNumber, isMirrored?"bot":"top");

   return key;
}

void CRetainedProbeMap::collectProbes(CCEtoODBDoc& camCadDoc, FileStruct& pcbFile)
{
   int dataLinkKw = camCadDoc.getStandardAttributeKeywordIndex(standardAttributeDataLink);
   int probePlacementKW = camCadDoc.getStandardAttributeKeywordIndex(standardAttributeProbePlacement);
   int netNameKw = camCadDoc.getStandardAttributeKeywordIndex(standardAttributeNetName);

   CCamCadPinMap camCadPinMap(&pcbFile);

   for (CDataListIterator dataList(*pcbFile.getBlock(), insertTypeTestProbe); dataList.hasNext();)
   {
      DataStruct* probe = dataList.getNext();      
      Attrib* attrib = NULL;

      if (probe->getAttributesRef() != NULL && probe->getAttributesRef()->Lookup(dataLinkKw, attrib))
      {

         // Get Test Access and the datalink attribute
         CEntity entity = CEntity::findEntity(camCadDoc.getCamCadData(), attrib->getIntValue());
         if (entity.getData() != NULL && entity.getData()->getAttributesRef() != NULL && entity.getData()->getAttributesRef()->Lookup(dataLinkKw, attrib))
         {
            // The key indicate what is the target and which side is the probe on.
            // Casual observer says: This looks wrong to me, at least not reliable. Graphic mirror is different than Layer mirror.
            CString key = getKey(attrib->getIntValue(), probe->getInsert()->getGraphicMirrored());
            m_retainedProbeMap.setAt(key, probe);
          }

         // Set probe to "Unplaced" and remove the DataLink
         probe->setAttrib(camCadDoc.getCamCadData(), probePlacementKW, valueTypeString, "Unplaced", attributeUpdateOverwrite, NULL);
         probe->removeAttrib(dataLinkKw);
         probe->setHidden(true);
      }
   }
}

DataStruct* CRetainedProbeMap::findProbe(const long entityNumber, const bool isMirrored)
{
   DataStruct* probe = NULL;
   CString key = getKey(entityNumber, isMirrored);
   m_retainedProbeMap.Lookup(key, probe);

   return probe;
}

/******************************************************************************
* CanUnion()
*/
BOOL CanUnion(CPolyList* partList)
{
   POSITION pos = partList->GetHeadPosition();
   while (pos != NULL)
   {
      CPoly *poly = partList->GetNext(pos);
      if (!poly->isClosed())
         return FALSE;

      if(!poly->getPntList().GetCount())
         return FALSE;

      if (PolySelfIntersects(&poly->getPntList()))
         return FALSE;
   }

   return TRUE;
}

/******************************************************************************
* Outline_GetOutline 
*
* returnCode
*   0 = generated good outline
*   1 = returned closed poly
*   2 = simple segment, returned segment outline
*  -1 = empty
*  -2 = no result
*  -3 = self-intersecting result
*  -4 = simple segment, no width
*/
CPntList *Outline_GetOutline(CPolyList* partList, int *returnCode)
{
   int count = partList->GetCount();
   CPntList *result;
   CCEtoODBDoc *doc = NULL;
   double accuracy;
   if(NULL != getActiveView())
      doc = getActiveView()->GetDocument();
   if(NULL != doc)
      accuracy = get_accuracy(doc);

   if (!count)
   {
      *returnCode = -1;

      //delete partList;
      //partList = NULL;

      return NULL;
   }

   if (count == 1)
   {
      CPoly *poly = partList->GetHead();


      if (poly->isClosed() && (poly->getPntList().GetCount() > 0) && !PolySelfIntersects(&poly->getPntList()))
      {
         result = new CPntList;

         while (poly->getPntList().GetCount())
            result->AddTail(poly->getPntList().RemoveHead());

         //delete partList;
         //partList = NULL;

         *returnCode = 1;
         return result;
      }
      else if (poly->getPntList().GetCount() == 2)   // is a simple segment or arc
      {
         // if it has no width, you are out of luck         
         if ((NULL != doc) && (doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() > SMALLNUMBER*2))
         {
            CPnt *head, *tail;
            head = poly->getPntList().GetHead();
            tail = poly->getPntList().GetTail();

            CPoly *newPoly = GetSegmentOutline(head->x, head->y, tail->x, tail->y, head->bulge, doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()/2, 0);

            result = new CPntList;
            while (newPoly->getPntList().GetCount())
               result->AddTail(newPoly->getPntList().RemoveHead());

            delete newPoly;
            //delete partList;
            //partList = NULL;

            *returnCode = 2;
            return result;
         }
         else
         {
            *returnCode = -4;
            return NULL; 
         }
      }
   }

   result = NULL;
   
   if (/*AllowUnion && */CanUnion(partList))
      result = BuildUnion(partList);

   /*if (!result && CanRubberBand(partList, accuracy))
      result = RubberBand(partList, accuracy);*/

   //FreePolyList(partList);

   if (!result)
   {
      *returnCode = -2;
      return NULL;
   }

   CleanPntList(result, accuracy);

   if (PolySelfIntersects(result))
   {
      FreePntList(result);
      result = NULL;
      *returnCode = -3;
      return NULL;
   }

   *returnCode = 0;
   return result;
}


