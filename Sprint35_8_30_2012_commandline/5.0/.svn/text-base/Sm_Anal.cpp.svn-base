// $Header: /CAMCAD/5.0/Sm_Anal.cpp 88    6/30/07 2:35a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "sm_anal.h"
#include "ccdoc.h"
#include "graph.h"
#include "lyrmanip.h"
#include "floodfil.h"
#include "attrib.h"
#include "pcbutil.h"
#include "net_util.h"
#include "rgn.h"
#include "Apertur2.h"
#include "RealPart.h"
#include "AccessAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *testaccesslayers[];

extern bool CollectAllRCs;

/* Function Prototypes *********************************************************/

static void ExplodeMetalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int newExMetalTop, int newExMetalBot);
static void ExplodeSoldermaskLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int newSMTop, int newSMBot);
static void CopyDataFromTopAndBottomLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int fromTopIndex, int fromBotIndex, int toTopIndex, int toBotIndex);

static void ExplodeSoldermaskAndMentalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int newExMetalTop, int newExMetalBot,
							int newSMTop, int newSMBot);
static void SM_CallRecursion(CCEtoODBDoc* doc, FileStruct* fileExplodeTo, FileStruct* fileExplodeFrom, int explodeLayer, int newLayer);
static void SM_ExplodeBlock(CCEtoODBDoc *doc, FileStruct *filePtr, CDataList *FileDataList, CDataList *DataList,
							double insert_x, double insert_y, double rotation, int mirror, double scale, int insertLayer,
							int recurseLevel, int explodeLayer,	int newLayer, int entityNum, CString compRef, BlockTypeTag blockType);

static void CreatComponetOutlineDistance(CCEtoODBDoc *doc, FileStruct *filePtr, CDataList *FileDataList, CDataList *DataList, double insert_x, double insert_y,
							double rotation, int mirror, double scale, int insertLayer, int recurseLevel, int explodeLayer,
							int newLayer, double distance, bool ignore1PinCompOutline, bool ignoreUnloadedCompOutline, 
							GraphicClassTag graphicClass, CHeightAnalysis* heightAnalysis, double heightAnalysisDistance);
static int CreatBoardOutlineDistance(CCEtoODBDoc *doc, FileStruct *filePtr, CDataList *FileDataList, CDataList *DataList, double insert_x, double insert_y,
							double rotation, int mirror, double scale, int insertLayer, int recurseLevel, int newLayer, double distance, bool bottom);

static int TryToConnect(BOOL Head, SelectStruct *s, double variance, CCEtoODBDoc *doc);
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance);
static int CheckForBadVariance(CPoly *poly1, CPoly* poly2, double variance);

static void CreateListOfRegion(CCEtoODBDoc *doc, int topLayer, int botLayer, CSMList &smTopList, CSMList &smBotList, bool boardOutline);
static void FreeListOfRegion(CSMList &smList);

static void ProcessExposeMetal(CCEtoODBDoc *doc, FileStruct *pcbFile, int topExposeMetalLayer,
							int botExposeMetalLayer, CSMList &smTopList, CSMList &smBotList);
static void xxxDeadCodexxxProcessExposeMetalByCompOutline(CCEtoODBDoc *doc, FileStruct *pcbFile, int topExposeMetalLayer, int botExposeMetalLayer,
							CSMList &smTopList, CSMList &smBotList, CExposedDataMap *exposedDataMap);

static int GetMetalExposedRegion(CCEtoODBDoc *doc, DataStruct *copperData, CSMList *smList);
static int GetDiffMetalExposedRegion(CCEtoODBDoc *doc, DataStruct *copperData, CSMList *smList);
static DataStruct* GetCombinedRegion(CCEtoODBDoc* doc, DataStruct* data1, DataStruct* data2);

static void SoldermaskSurfaceCheck(CCEtoODBDoc *doc, BlockStruct *block, BOOL Top, BOOL Negative);
static void SoldermaskCheck(CCEtoODBDoc *doc, BOOL Negative);
static void SoldermaskPostProcess(CCEtoODBDoc *doc, FileStruct *file);
static void explode_soldermask(CCEtoODBDoc *doc, CString topLayer, CString bottomLayer, BOOL deleteOrig);
static void graphicRegion(CCEtoODBDoc *doc, FileStruct *pcbFile, CSMList &smList, int layer);

////////////////////////////////////////////////////////////////////////////////
// New Soldermask Analysis Implementation
////////////////////////////////////////////////////////////////////////////////
static double getNumberOfPixels()
{
   return 50.0;
}
void CreateExplodeMetalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
	// Delete the layer and everything on it
	LayerStruct *tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_METAL_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_METAL_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);


	// Create the Combine Metal layers
	int combineTop = Graph_Level(DFT_LAYER_COMBIN_METAL_TOP, "", FALSE);
   int combineBot = Graph_Level(DFT_LAYER_COMBIN_METAL_BOT, "", FALSE);

	doc->getLayerArray()[combineTop]->setLayerType(LAYTYPE_PAD_TOP);
   doc->getLayerArray()[combineBot]->setLayerType(LAYTYPE_PAD_BOTTOM);
   doc->getLayerArray()[combineTop]->setMirroredLayerIndex(combineBot);
   doc->getLayerArray()[combineBot]->setMirroredLayerIndex(combineTop);
   doc->getLayerArray()[combineTop]->setVisible(false);
   doc->getLayerArray()[combineBot]->setVisible(false);

	ExplodeMetalLayers(doc, pcbFile, combineTop, combineBot);
}

void SolderMaskAnalysis(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
	if (doc == NULL)
		return;

	// Delete the layer and everything on it
	LayerStruct *tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_SM_ANALYSYS_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_SM_ANALYSYS_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_SOLDERMASK_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_SOLDERMASK_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	// Create the Soldermask Analysis and Combine Soldermask layers
   int smAnalysisTop = Graph_Level(DFT_LAYER_SM_ANALYSYS_TOP, "", FALSE);
   int smAnalysisBot = Graph_Level(DFT_LAYER_SM_ANALYSYS_BOT, "", FALSE);
   int newSMTop = Graph_Level(DFT_LAYER_COMBIN_SOLDERMASK_TOP, "", FALSE);
   int newSMBot = Graph_Level(DFT_LAYER_COMBIN_SOLDERMASK_BOT, "", FALSE);
			
   doc->getLayerArray()[smAnalysisTop]->setLayerType(LAYTYPE_UNKNOWN);
   doc->getLayerArray()[smAnalysisBot]->setLayerType(LAYTYPE_UNKNOWN);
   doc->getLayerArray()[smAnalysisTop]->setMirroredLayerIndex(smAnalysisBot);
   doc->getLayerArray()[smAnalysisBot]->setMirroredLayerIndex(smAnalysisTop);
   doc->getLayerArray()[smAnalysisTop]->setVisible(false);
   doc->getLayerArray()[smAnalysisBot]->setVisible(false);

   doc->getLayerArray()[newSMTop]->setLayerType(LAYTYPE_MASK_TOP);
   doc->getLayerArray()[newSMBot]->setLayerType(LAYTYPE_MASK_BOTTOM);
   doc->getLayerArray()[newSMTop]->setMirroredLayerIndex(newSMBot);
   doc->getLayerArray()[newSMBot]->setMirroredLayerIndex(newSMTop);
   doc->getLayerArray()[newSMTop]->setVisible(false);
   doc->getLayerArray()[newSMBot]->setVisible(false);

	// Explode soldermask
	ExplodeSoldermaskLayers(doc, pcbFile, newSMTop, newSMBot);

   CSMList smTopList;
   CSMList smBotList;
	CreateListOfRegion(doc, newSMTop, newSMBot, smTopList, smBotList, false);
	if (smTopList.GetCount() == 0 && smBotList.GetCount() == 0)
	{

		return;
	}

	// Copy the Combine Metal layers on to the Soldermask Analysis layers
	int exMetalTop = Graph_Level(DFT_LAYER_COMBIN_METAL_TOP, "", FALSE);
   int exMetalBot = Graph_Level(DFT_LAYER_COMBIN_METAL_BOT, "", FALSE);
	CopyDataFromTopAndBottomLayers(doc, pcbFile, exMetalTop, exMetalBot, smAnalysisTop, smAnalysisBot);


	// Processes to analysis soldermask
	ProcessExposeMetal(doc, pcbFile, smAnalysisTop, smAnalysisBot, smTopList, smBotList);

	FreeListOfRegion(smTopList);
	FreeListOfRegion(smBotList);
}

void CopyDataFromTopAndBottomLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int fromTopIndex,
												int fromBotIndex, int toTopIndex, int toBotIndex)
{
	POSITION pos = pcbFile->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(pos);
		if (data->getDataType() != dataTypePoly)
			continue;

		if (data->getLayerIndex() == fromTopIndex)
		{
			DataStruct* newData = doc->getCamCadData().getNewDataStruct(*data, true);
			newData->setLayerIndex(toTopIndex);
			pcbFile->getBlock()->getDataList().AddTail(newData);
		}
		else if (data->getLayerIndex() == fromBotIndex)
		{
			DataStruct* newData = doc->getCamCadData().getNewDataStruct(*data, true);
			newData->setLayerIndex(toBotIndex);
			pcbFile->getBlock()->getDataList().AddTail(newData);
		}
	}
}

void CopyExplodeMetalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, CExposedDataMap *exposedDataMap)
{
	//Delete the layer and everything on it
	LayerStruct *tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_EXPOSE_METAL_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_EXPOSE_METAL_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

   int combineTop = Graph_Level(DFT_LAYER_COMBIN_METAL_TOP, "", FALSE);
   int combineBot = Graph_Level(DFT_LAYER_COMBIN_METAL_BOT, "", FALSE);
   int exMetalTop = Graph_Level(DFT_LAYER_EXPOSE_METAL_TOP, "", FALSE);
   int exMetalBot = Graph_Level(DFT_LAYER_EXPOSE_METAL_BOT, "", FALSE);

   doc->getLayerArray()[exMetalTop]->setLayerType(LAYTYPE_UNKNOWN);
   doc->getLayerArray()[exMetalBot]->setLayerType(LAYTYPE_UNKNOWN);
   doc->getLayerArray()[exMetalTop]->setMirroredLayerIndex(exMetalBot);
   doc->getLayerArray()[exMetalBot]->setMirroredLayerIndex(exMetalTop);

	// Add attribute to indicate Expose Metal layers
	int layerKey = doc->RegisterKeyWord(DFT_LAY_EXPOSE_METAL_ATTRIB, 0, valueTypeString);

	LayerStruct *exMetalToplayer = doc->getLayerAt(exMetalTop);
	LayerStruct *exMetalBotlayer = doc->getLayerAt(exMetalBot);

	exMetalToplayer->setVisible(false);
	exMetalBotlayer->setVisible(false);

	CString topValue = DFT_LAY_EXPOSE_METAL_VALUE_TOP;
	CString botValue = DFT_LAY_EXPOSE_METAL_VALUE_BOT;

	doc->SetAttrib(&exMetalToplayer->getAttributesRef(), layerKey, valueTypeString, topValue.GetBuffer(0), attributeUpdateOverwrite, NULL);
	doc->SetAttrib(&exMetalBotlayer->getAttributesRef(), layerKey, valueTypeString, botValue.GetBuffer(0), attributeUpdateOverwrite, NULL);

	WORD keyword = (WORD)doc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
	POSITION pos = pcbFile->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(pos);
		if (data->getDataType() != dataTypePoly)
			continue;

		DataStruct* newData = NULL;
      POSITION newDataPosition = NULL;
		CString surface = "";

		if (data->getLayerIndex() == combineTop)
		{
			surface = "top";
			newData = doc->getCamCadData().getNewDataStruct(*data, true);
			newData->setLayerIndex(exMetalTop);
			newDataPosition = pcbFile->getBlock()->getDataList().AddTail(newData);
		}
		else if (data->getLayerIndex() == combineBot)
		{
			surface = "bot";
			newData = doc->getCamCadData().getNewDataStruct(*data, true);
			newData->setLayerIndex(exMetalBot);
			newDataPosition = pcbFile->getBlock()->getDataList().AddTail(newData);
		}
		else
		{
			continue;
		}

		Attrib* attrib = NULL;
		if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(keyword, attrib))
			continue;
		if (attrib == NULL)
			continue;

		CString key = "";
		key.Format("%d%s", attrib->getIntValue(), surface);
		CExposedData *exposedData = NULL;
		if (exposedDataMap->Lookup(key, exposedData))
		{
			if (newData)
			{
				// There is metal
				exposedData->SetExposedData(newData);
				exposedData->SetFailureRC(DFT_AA_RC_NONE);
			}
		}
	}
}

void CopySoldermaskAnalysisLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, CExposedDataMap *exposedDataMap)
{
	// Delete the layer and everything on it
	LayerStruct *tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_EXPOSE_METAL_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_EXPOSE_METAL_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);


   int combineTop = Graph_Level(DFT_LAYER_COMBIN_METAL_TOP, "", FALSE);
   int combineBot = Graph_Level(DFT_LAYER_COMBIN_METAL_BOT, "", FALSE);
   int smAnalysisTop = Graph_Level(DFT_LAYER_SM_ANALYSYS_TOP, "", FALSE);
   int smAnalysisBot = Graph_Level(DFT_LAYER_SM_ANALYSYS_BOT, "", FALSE);
   int exMetalTop = Graph_Level(DFT_LAYER_EXPOSE_METAL_TOP, "", FALSE);
   int exMetalBot = Graph_Level(DFT_LAYER_EXPOSE_METAL_BOT, "", FALSE);

   doc->getLayerArray()[exMetalTop]->setLayerType(LAYTYPE_UNKNOWN);
   doc->getLayerArray()[exMetalBot]->setLayerType(LAYTYPE_UNKNOWN);
   doc->getLayerArray()[exMetalTop]->setMirroredLayerIndex(exMetalBot);
   doc->getLayerArray()[exMetalBot]->setMirroredLayerIndex(exMetalTop);

	// Add attribute to indicate Expose Metal layers
	int layerKey = doc->RegisterKeyWord(DFT_LAY_EXPOSE_METAL_ATTRIB, 0, valueTypeString);

	LayerStruct *exMetalToplayer = doc->getLayerAt(exMetalTop);
	LayerStruct *exMetalBotlayer = doc->getLayerAt(exMetalBot);

	exMetalToplayer->setVisible(false);
	exMetalBotlayer->setVisible(false);

	CString topValue = DFT_LAY_EXPOSE_METAL_VALUE_TOP;
	CString botValue = DFT_LAY_EXPOSE_METAL_VALUE_BOT;

	doc->SetAttrib(&exMetalToplayer->getAttributesRef(), layerKey, valueTypeString, topValue.GetBuffer(0), attributeUpdateOverwrite, NULL);
	doc->SetAttrib(&exMetalBotlayer->getAttributesRef(), layerKey, valueTypeString, botValue.GetBuffer(0), attributeUpdateOverwrite, NULL);

	WORD keyword = (WORD)doc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
	POSITION pos = pcbFile->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(pos);
		if (data->getDataType() != dataTypePoly)
			continue;

		DataStruct* newData = NULL;
      POSITION newDataPosition = NULL;
		CString surface = "";

		if (data->getLayerIndex() == combineTop)
		{
			surface = "top";
		}
		else if (data->getLayerIndex() == combineBot)
		{
			surface = "bot";
		}
		else if (data->getLayerIndex() == smAnalysisTop)
		{
			surface = "top";
			newData = doc->getCamCadData().getNewDataStruct(*data, true);
			newData->setLayerIndex(exMetalTop);
			newDataPosition = pcbFile->getBlock()->getDataList().AddTail(newData);
		}
		else if (data->getLayerIndex() == smAnalysisBot)
		{
			surface = "bot";
			newData = doc->getCamCadData().getNewDataStruct(*data, true);
			newData->setLayerIndex(exMetalBot);
			newDataPosition = pcbFile->getBlock()->getDataList().AddTail(newData);
		}
		else
		{
			continue;
		}

		Attrib* attrib = NULL;
		if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(keyword, attrib))
			continue;
		if (attrib == NULL)
			continue;

		CString key = "";
		key.Format("%d%s", attrib->getIntValue(), surface);
		CExposedData *exposedData = NULL;
		if (exposedDataMap->Lookup(key, exposedData))
		{
			if (newData)
			{
				// There is metal exposed by soldermask
				exposedData->SetExposedData(newData);
//            exposedData->SetExposedDataPosition(newDataPosition);
				exposedData->SetFailureRC(DFT_AA_RC_NONE); //*rcf SetErrorMessage("");
			}
			else if (exposedData->GetExposedData() == NULL)
			{
				// There is metal but no soldermask
				exposedData->SetFailureRC(DFT_AA_RC_NO_SOLDERMASK);  //*rcfSetErrorMessage(DFT_AA_RC_NO_SOLDERMASK);
			}
		}
	}
}

void xxxDeadCodexxxComponentOutlineDistanceAnalysis(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top, bool bottom,
												  double topCompOutlineDistance, double botCompOutlineDistance,
												  CExposedDataMap *exposedDataMap, bool ignore1PinCompOutline, 
												  bool ignoreUnloadedCompOutline, bool useRealPartOutline,
												  CHeightAnalysis* topHeightAnalysis, CHeightAnalysis* botHeightAnalysis)
{
	if (top == false && bottom == false)
		return;

	// Create the component outline distance layers just incase it is not created yet
	int newOutlineTop = Graph_Level(DFT_LAYER_T_OUTLINE_TOP, "", FALSE);
   int newOutlineBot = Graph_Level(DFT_LAYER_T_OUTLINE_BOT, "", FALSE);
   			
   doc->getLayerArray()[newOutlineTop]->setLayerType(LAYTYPE_TOP);
   doc->getLayerArray()[newOutlineBot]->setLayerType(LAYTYPE_BOTTOM);
   doc->getLayerArray()[newOutlineTop]->setMirroredLayerIndex(newOutlineBot);
   doc->getLayerArray()[newOutlineBot]->setMirroredLayerIndex(newOutlineTop);

	int packageOutineTop = -1;
	int packageOutineBot = -1;
	GraphicClassTag graphicClass = graphicClassUndefined;
	if (useRealPartOutline)
	{
		packageOutineTop = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
		packageOutineBot = Graph_Level(RP_LAYER_PKG_BODY_BOTTOM, "", FALSE);
		graphicClass = graphicClassPackageBody;
	}
	else
	{
		packageOutineTop = Graph_Level(DFT_OUTLINE_TOP, "", FALSE);
		packageOutineBot = Graph_Level(DFT_OUTLINE_BOTTOM, "", FALSE);
		graphicClass = graphicClassPackageOutline;
	}

   CSMList smTopList;
   CSMList smBotList;

   int mirror = 0; 
   if (pcbFile->isMirrored())
		mirror = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic

	if (top == true && bottom == true)
	{
		CreatComponetOutlineDistance(doc, pcbFile, &pcbFile->getBlock()->getDataList(), &pcbFile->getBlock()->getDataList(),
					0, 0, 0, mirror, pcbFile->getScale(), -1, 0, packageOutineTop, newOutlineTop, 
					topCompOutlineDistance, ignore1PinCompOutline, ignoreUnloadedCompOutline, graphicClass,
					topHeightAnalysis, -1);

		CreatComponetOutlineDistance(doc, pcbFile, &pcbFile->getBlock()->getDataList(), &pcbFile->getBlock()->getDataList(),
					0, 0, 0, mirror, pcbFile->getScale(), -1, 0,	packageOutineBot, newOutlineBot, 
					botCompOutlineDistance, ignore1PinCompOutline, ignoreUnloadedCompOutline, graphicClass,
					botHeightAnalysis, -1);

		CreateListOfRegion(doc, newOutlineTop, newOutlineBot, smTopList, smBotList, false);
	}
	else if (top == true)
	{
		CreatComponetOutlineDistance(doc, pcbFile, &pcbFile->getBlock()->getDataList(), &pcbFile->getBlock()->getDataList(),
					0, 0, 0, mirror, pcbFile->getScale(), -1, 0, packageOutineTop, newOutlineTop, 
					topCompOutlineDistance, ignore1PinCompOutline, ignoreUnloadedCompOutline, graphicClass,
					topHeightAnalysis, -1);

		CreateListOfRegion(doc, newOutlineTop, -1, smTopList, smBotList, false);
	}
	else if (bottom == true)
	{
		CreatComponetOutlineDistance(doc, pcbFile, &pcbFile->getBlock()->getDataList(), &pcbFile->getBlock()->getDataList(),
					0, 0, 0, mirror, pcbFile->getScale(), -1, 0,	packageOutineBot, newOutlineBot, 
					botCompOutlineDistance, ignore1PinCompOutline, ignoreUnloadedCompOutline, graphicClass,
					botHeightAnalysis, -1);

		CreateListOfRegion(doc, -1, newOutlineBot, smTopList, smBotList, false);
	}

	if (smTopList.GetCount() == 0 && smBotList.GetCount() == 0)
		return;

   int exMetalTop = Graph_Level(DFT_LAYER_EXPOSE_METAL_TOP, "", FALSE);
   int exMetalBot = Graph_Level(DFT_LAYER_EXPOSE_METAL_BOT, "", FALSE);

	xxxDeadCodexxxProcessExposeMetalByCompOutline(doc, pcbFile, exMetalTop, exMetalBot, smTopList, smBotList, exposedDataMap);

	FreeListOfRegion(smTopList);
	FreeListOfRegion(smBotList);
}

/******************************************************************************
* graphicRegion
	- Use for debug propuse
*/
static void graphicRegion(CCEtoODBDoc *doc, FileStruct *pcbFile, CSMList &smList, int layer)
{
	Graph_Block_On(pcbFile->getBlock());

   POSITION smPos = smList.GetHeadPosition();
   while (smPos) // for each soldermask
   {
      ERPStruct *sm = smList.GetNext(smPos);
		DataStruct *data = Graph_PolyStruct(layer, 0L, FALSE);

		FreePolyList(data->getPolyList());
		data->getPolyList() = new CPolyList(*sm->pPolyList);
	}

	Graph_Block_Off();
}

/******************************************************************************
* ExplodeMetalLayers
*/
static void ExplodeMetalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int newExMetalTop, int newExMetalBot)
{
	if (doc == NULL)
		return;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      for (int i=0; i<doc->getMaxLayerIndex(); i++)
      {
         if (i == newExMetalTop || i == newExMetalBot)
            continue;

         LayerStruct *layer = doc->getLayerArray()[i];

         if (!layer)
            continue;
         
         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_ALL:
			case LAYTYPE_PAD_OUTER:
			case LAYTYPE_SIGNAL_ALL:
            SM_CallRecursion(doc, pcbFile, file, i, newExMetalTop);
            SM_CallRecursion(doc, pcbFile, file, i, newExMetalBot);
            break;

			case LAYTYPE_PAD_TOP:
			case LAYTYPE_SIGNAL_TOP:
            SM_CallRecursion(doc, pcbFile, file, i, newExMetalTop);
				break;
			
			case LAYTYPE_PAD_BOTTOM:
			case LAYTYPE_SIGNAL_BOT:
            SM_CallRecursion(doc, pcbFile, file, i, newExMetalBot);
            break;
			}
      }
   }

	// Eliminate multiple metal data on same layer with the same entity number
	WORD dataLinkKw = doc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
   CString dataMapKey;
   CTypedMapStringToPtrContainer<DataStruct*> metalDataMap(nextPrime2n(20), false);
	POSITION pos = pcbFile->getBlock()->getDataList().GetHeadPosition();
	while (pos != NULL)
	{
		POSITION curPos = pos;
		DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypePoly || data->getAttributes() == NULL ||
         (data->getLayerIndex() != newExMetalTop && data->getLayerIndex() != newExMetalBot))
			continue;

		Attrib* attrib = NULL;
		if (!data->getAttributes()->Lookup(dataLinkKw, attrib))
			continue;

      DataStruct* existingMetalData = NULL;
      dataMapKey.Format("%d_%s", attrib->getIntValue(), data->getLayerIndex() == newExMetalTop?"TOP":"BOT");
      metalDataMap.Lookup(dataMapKey, existingMetalData);

      if (existingMetalData == NULL)
      {
         // Move the current metal data from PCB file to a temperary map
         metalDataMap.SetAt(dataMapKey, data);
         pcbFile->getBlock()->getDataList().RemoveAt(curPos);
      }
      else
		{
		   // If a metal data with the same entity number on the same surface already exited,
         // then combine the existing one with the current one
			DataStruct* newData = GetCombinedRegion(doc, existingMetalData, data);
			if (newData != NULL)
         {
            // Replace the existing metal data in the temperary map with the combined metal
            // and remove the current metal data from PCb file

            metalDataMap.SetAt(dataMapKey, newData);
            pcbFile->getBlock()->getDataList().RemoveAt(curPos);

            delete existingMetalData;
            existingMetalData = NULL;

            delete data;
            data = NULL;
         }
      }
	}

   // Moved all the metal data from temperary map back to the block of PCB file
   for (POSITION pos = metalDataMap.GetStartPosition(); pos != NULL;)
   {
      DataStruct* metalData = NULL;
      CString key;
      metalDataMap.GetNextAssoc(pos, key, metalData);

      if (metalData != NULL)
      {
         pcbFile->getBlock()->getDataList().AddTail(metalData);
      }
   }
   metalDataMap.empty();
}

/******************************************************************************
* ExplodeSoldermaskLayers
*/
static void ExplodeSoldermaskLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, int newSMTop, int newSMBot)
{
	if (doc == NULL)
		return;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      for (int i=0; i<doc->getMaxLayerIndex(); i++)
      {
         if (i == newSMTop || i == newSMBot)
            continue;

         LayerStruct *layer = doc->getLayerArray()[i];

         if (!layer)
            continue;
         
         switch (layer->getLayerType())
         {
			case LAYTYPE_MASK_TOP:
            SM_CallRecursion(doc, pcbFile, file, i, newSMTop);
            break;
         
         case LAYTYPE_MASK_BOTTOM:
            SM_CallRecursion(doc, pcbFile, file, i, newSMBot);
            break;
         
         case LAYTYPE_MASK_ALL:
            SM_CallRecursion(doc, pcbFile, file, i, newSMTop);
            SM_CallRecursion(doc, pcbFile, file, i, newSMBot);
            break;
         }
      }
   }
}

/******************************************************************************
* SM_CallRecursion
*/
static void SM_CallRecursion(CCEtoODBDoc* doc, FileStruct* fileExplodeTo, FileStruct* fileExplodeFrom, int explodeLayer, int newLayer)
{
   int mirror = 0;
   if (fileExplodeTo->isMirrored())
		mirror = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic

   SM_ExplodeBlock(doc, fileExplodeTo, &(fileExplodeTo->getBlock()->getDataList()), &(fileExplodeFrom->getBlock()->getDataList()), 
          0, 0, 0, mirror, fileExplodeTo->getScale(), -1, 0, explodeLayer, newLayer, 0, "", fileExplodeTo->getBlock()->getBlockType());
}

/******************************************************************************
* SM_ExplodeBlock
*/
void SM_ExplodeBlock(CCEtoODBDoc* doc, FileStruct* filePtr, CDataList* FileDataList, CDataList *DataList, double insert_x, double insert_y,
							double rotation, int mirror, double scale, int insertLayer, int recurseLevel, int explodeLayer,
							int newLayer, int entityNum, CString compRef, BlockTypeTag blockType)
{
   if (explodeLayer == newLayer && FileDataList == DataList)
   {
      ErrorMessage("Explode layer and New Layer are the same", "Infinite Recusion");
      return;
   }

   Mat2x2 m;
   int layerIndex;

   RotMat2(&m, rotation);
	int keyword = doc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      // figure LAYER
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layerIndex = insertLayer;
      else
         layerIndex = data->getLayerIndex();

      //if ((mirror & MIRROR_LAYERS) && layerIndex >= 0)
      //   layerIndex = doc->LayerArray[layerIndex]->getMirroredLayerIndex();

      switch(data->getDataType())
      {
         case dataTypePoly:
         {
				if ((mirror & MIRROR_LAYERS) && layerIndex >= 0)
					layerIndex = doc->getLayerArray()[layerIndex]->getMirroredLayerIndex();

            if (layerIndex == explodeLayer)
            {
					LayerStruct *layer = doc->getLayerAt(layerIndex);
					if (layer == NULL)
						continue;
					if ((mirror && layer->getNeverMirror()) && (!mirror && layer->getMirrorOnly()))
						continue;


					bool isLayerTypeSoldermask = false;
					if (layer->getLayerType() == layerTypeMaskAll || layer->getLayerType () == layerTypeMaskTop || layer->getLayerType() == layerTypeMaskBottom)
						isLayerTypeSoldermask = true;						

					if (!isLayerTypeSoldermask)
					{
						// If the poly is not inside a PCB Component block then ignore
						if (blockType != blockTypePcbComponent && blockType != blockTypePadshape)
							continue;

						// If the poly is not of type ETCH and is not inside a padshape then ignore
						if (data->getGraphicClass() != GR_CLASS_ETCH && blockType != blockTypePadshape)
								continue;	
					}

               DataStruct *newData = CopyTransposeEntity(data, insert_x, insert_y, rotation, mirror, scale, newLayer, TRUE);
               newData->setLayerIndex(newLayer);
               FileDataList->AddTail(newData);

					int newEntityNum = entityNum;
					if (newEntityNum < 1)
						newEntityNum = newData->getEntityNumber();

					doc->SetAttrib(&newData->getAttributesRef(), keyword, valueTypeInteger, (void*)&newEntityNum, attributeUpdateOverwrite, NULL);
            }
         }
         break; 

         case dataTypeInsert:
         {
				bool skipInsert = false;
				if (data->getInsert()->getInsertType() != insertTypePcbComponent &&
					 data->getInsert()->getInsertType() != insertTypeTestPoint &&  
					 data->getInsert()->getInsertType() != insertTypeVia &&
					 data->getInsert()->getInsertType() != insertTypeTestPad &&
					 data->getInsert()->getInsertType() != insertTypePin &&
					 data->getInsert()->getInsertType() != insertTypeUnknown)
				{
					skipInsert = true;

					if (data->getInsert()->getInsertType() == insertTypeFreePad)
					{
						LayerStruct *layer = doc->getLayerAt(explodeLayer);
						if (layer != NULL)
						{
							if (layer->getLayerType() == layerTypeMaskAll || layer->getLayerType () == layerTypeMaskTop || layer->getLayerType() == layerTypeMaskBottom)
							{
								// If the layer to be explode is Soldermask then allow insert type Freepad
								skipInsert = false;
							}
						}
					}
				}

				if (skipInsert)
					continue;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();

            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror & MIRROR_FLIP) point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            // if aperture
            if ( (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE) )
            {
					if (subblock->getShape() == apertureComplex)
					{
						double block_rot;
						if (mirror & MIRROR_FLIP)
							block_rot = rotation - data->getInsert()->getAngle();
						else
							block_rot = rotation + data->getInsert()->getAngle();

						double complex_rot;
						if (block_mirror & MIRROR_FLIP)
							complex_rot = block_rot - subblock->getRotation();
						else
							complex_rot = block_rot + subblock->getRotation();

						BlockStruct *complexBlock = doc->getBlockAt((int)subblock->getSizeA());

						// Set the blockType to Padshape just incase the complex is of an geometry without a type.
						// If the complex has a blockType, then use it
						BlockTypeTag complexBlockType = blockTypePadshape;
						if (complexBlock->getBlockType() != blockTypeUnknown)
							complexBlockType = complexBlock->getBlockType();

						Mat2x2 complex_m;
						RotMat2(&complex_m, block_rot);

						Point2 offsetPoint;
						offsetPoint.x = complexBlock->getXoffset();
						offsetPoint.y = complexBlock->getYoffset();
						TransPoint2(&offsetPoint, 1, &complex_m, point2.x, point2.y);

						int newEntityNum = entityNum;
						if (newEntityNum < 1)
							newEntityNum = data->getEntityNumber();
						
						SM_ExplodeBlock(doc, filePtr, FileDataList, &(complexBlock->getDataList()), offsetPoint.x, offsetPoint.y, 
								complex_rot, block_mirror, scale * data->getInsert()->getScale(), layerIndex, recurseLevel+1, explodeLayer, 
								newLayer, newEntityNum, compRef, complexBlockType);
					}
               else
					{
						if ((mirror & MIRROR_LAYERS) && layerIndex >= 0)
							layerIndex = doc->getLayerArray()[layerIndex]->getMirroredLayerIndex();

						if (layerIndex == explodeLayer)
						{
							LayerStruct *layer = doc->getLayerAt(layerIndex);
							if (layer == NULL)
								continue;
							if ((mirror && layer->getNeverMirror()) || (!mirror && layer->getMirrorOnly()))
								continue;

							Graph_Block_On(filePtr->getBlock());					
							DataStruct *newData = Graph_PolyStruct(newLayer, 0L, FALSE);
							Graph_Block_Off();

							FreePolyList(newData->getPolyList());
							newData->getPolyList() = ApertureToPolyTranspose(subblock, data->getInsert(), insert_x, insert_y, rotation, scale, mirror);

							if (newData->getPolyList())
							{
								POSITION pos = newData->getPolyList()->GetHeadPosition();
								while (pos)
								{
									CPoly *poly = newData->getPolyList()->GetNext(pos);
									if (poly)
										poly->setWidthIndex(-1);
								}
							}
							else
							{
								// Can't create poly so delete the data
								//filePtr->getBlock()->RemoveDataFromList(doc, newData);
                        doc->removeDataFromDataList(*(filePtr->getBlock()),newData);
							}

							int newEntityNum = entityNum;
							if (newEntityNum < 1)
								newEntityNum = newData->getEntityNumber();

							doc->SetAttrib(&newData->getAttributesRef(), keyword, valueTypeInteger, (void*)&newEntityNum, attributeUpdateOverwrite, NULL);
						} 
					}
            }
            else
            {
					int newEntityNum = entityNum;
					if (data->getInsert()->getInsertType() == insertTypePcbComponent || data->getInsert()->getInsertType() == insertTypeTestPoint)
					{
						compRef = data->getInsert()->getRefname();
						if (newEntityNum < 1)
							newEntityNum = data->getEntityNumber();
					}
					else if (data->getInsert()->getInsertType() == insertTypePin)
					{
						NetStruct *net = NULL;
						CString pinRef = data->getInsert()->getRefname();
						CompPinStruct *comppin = FindCompPin(filePtr, compRef, pinRef, &net);
						if (comppin)
							newEntityNum = comppin->getEntityNumber();
						else
                     break;
							//newEntityNum = data->getEntityNumber();
					}
					else
					{
						if (newEntityNum < 1)
							newEntityNum = data->getEntityNumber();
					}

               double block_rot;
               if (mirror & MIRROR_FLIP)
                  block_rot = rotation - data->getInsert()->getAngle();
               else
                  block_rot = rotation + data->getInsert()->getAngle();

               SM_ExplodeBlock(doc, filePtr, FileDataList, &(subblock->getDataList()), point2.x, point2.y, block_rot, 
							block_mirror, scale * data->getInsert()->getScale(), layerIndex, recurseLevel+1, explodeLayer, 
							newLayer, newEntityNum, compRef, subblock->getBlockType());

					if (data->getInsert()->getInsertType() == insertTypePcbComponent || data->getInsert()->getInsertType() == insertTypeTestPoint)
						compRef = "";
            }
         } 
         break;
      } // end T_INSERT
   }
} 

static void CreatComponetOutlineDistance(CCEtoODBDoc *doc, FileStruct *filePtr, CDataList *FileDataList, CDataList *DataList, double insert_x, double insert_y,
							double rotation, int mirror, double scale, int insertLayer, int recurseLevel, int explodeLayer,	int newLayer, 
							double distance, bool ignore1PinCompOutline, bool ignoreUnloadedCompOutline, GraphicClassTag graphicClass,
							CHeightAnalysis* heightAnalysis, double heightAnalysisDistance)
{
   if (explodeLayer == newLayer && FileDataList == DataList)
   {
      ErrorMessage("Explode layer and New Layer are the same", "Infinite Recusion");
      return;
   }

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

		int layer = data->getLayerIndex();
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layer = insertLayer;

      if (mirror & MIRROR_LAYERS && layer >= 0)
         layer = doc->getLayerArray()[layer]->getMirroredLayerIndex();

		if (data->getDataType() == dataTypePoly)
		{
			if (layer == explodeLayer && data->getGraphicClass() == graphicClass)
			{
            DataStruct *newData = CopyTransposeEntity(data, insert_x, insert_y, rotation, mirror, scale, newLayer, TRUE);
            newData->setLayerIndex(newLayer);
				newData->setGraphicClass(graphicClassNormal);
            FileDataList->AddTail(newData);

				// Expand the package outline by the size of Component Outline Distance
				if (newData->getPolyList())
				{
					if (newData->getPolyList()->GetCount() > 1)
						ErrorMessage("More than one poly found in Package outline during CreateComponentOutlineDistant", "Error");
					else
					{
						CPoly *poly = newData->getPolyList()->GetHead();
						if (poly)
						{
							if (heightAnalysisDistance > 0)
                        poly->shrink(-heightAnalysisDistance,doc->getPageUnits());
							else
								poly->shrink(-distance,doc->getPageUnits());
							poly->setFilled(FALSE);
						}
					}
				}
				else
				{
					// Can't copy the package outlien
					//filePtr->getBlock()->RemoveDataFromList(doc, newData);
               doc->removeDataFromDataList(*(filePtr->getBlock()),newData);
				}
			}
		}
		else if (data->getDataType() == dataTypeInsert)
		{
         BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

			if (subblock == NULL)
				continue;

			if (subblock->getBlockType() != blockTypePcbComponent && 
				 subblock->getBlockType() != blockTypeRealPart && subblock->getBlockType() != blockTypePackage)
				continue;

			// Check to see if inside the PCB block or Package block that is single pin or loaded false
			if (subblock->getBlockType() == blockTypePcbComponent || subblock->getBlockType() == blockTypePackage)
			{
				if (ignore1PinCompOutline == true)
				{
					// Check to see it is a single pin component
					if (subblock->getPinCount() == 1)
						continue;
				}
			}

			if (subblock->getBlockType() == blockTypePcbComponent)
			{
				if (ignoreUnloadedCompOutline == true)
				{
					WORD loadedKey = doc->RegisterKeyWord(ATT_LOADED, 0, valueTypeString);
					Attrib* attrib = NULL;
					if (data->getAttributes() && data->getAttributes()->Lookup(loadedKey, attrib))
					{
						if (!attrib->getStringValue().CompareNoCase("FALSE"))
							continue;
					}
				}
			}

			double tmpHeightAnalysisDistance = -1;			
			InsertStruct* insert = data->getInsert();
			if (heightAnalysis != NULL)
			{
				if (insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypeRealPart)
				{
					WORD compHeightKw = doc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
					Attrib* attrib = NULL;
					if (data->getAttributes() && data->getAttributes()->Lookup(compHeightKw, attrib))
					{
						double compHeight = attrib->getDoubleValue();
						tmpHeightAnalysisDistance = heightAnalysis->GetPackageOutlineDistance(compHeight);
					}
				}
				else if (insert->getInsertType() == insertTypePackage)
				{
					tmpHeightAnalysisDistance = heightAnalysisDistance;
				}
			}

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();

         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror & MIRROR_FLIP)
				point2.x = -point2.x;
         point2.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x,insert_y);

         double block_rot;
         if (mirror & MIRROR_FLIP)
            block_rot = rotation - data->getInsert()->getAngle();
         else
            block_rot = rotation + data->getInsert()->getAngle();

         CreatComponetOutlineDistance(doc, filePtr, FileDataList, &(subblock->getDataList()), point2.x, point2.y, block_rot, block_mirror, 
               scale * data->getInsert()->getScale(), layer, recurseLevel+1, explodeLayer, newLayer, distance, ignore1PinCompOutline,
					ignoreUnloadedCompOutline, graphicClass, heightAnalysis, tmpHeightAnalysisDistance);
		}
   }
} 

/******************************************************************************
* CreatBoardOutlineDistance

	Funtion return 0 if no error
*/
static int CreatBoardOutlineDistance(CCEtoODBDoc *doc, FileStruct *filePtr, CDataList *FileDataList, CDataList *DataList, double insert_x, double insert_y,
							double rotation, int mirror, double scale, int insertLayer, int recurseLevel, int newLayer, double distance, bool bottom)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

	int boardOutlineKey = doc->RegisterKeyWord("Board Outline", 0, valueTypeNone);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      // Do not do a get next here because the POS can be change in the section of the code that 
      // does the joining of polylist
      DataStruct *data = DataList->GetAt(pos);
		if (data == NULL)
			continue;

		int layer = data->getLayerIndex();
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layer = insertLayer;

      if (mirror & MIRROR_LAYERS && layer >= 0)
         layer = doc->getLayerArray()[layer]->getMirroredLayerIndex();

		if (data->getDataType() == dataTypePoly)
		{
			if (data->getGraphicClass() == graphicClassBoardOutline)
			{
				CPoly *tmpPoly = (CPoly *)data->getPolyList()->GetHead();
				if (tmpPoly->isClosed() == false)
				{
					SelectStruct *s = new SelectStruct;
					s->setData(data);
					s->filenum = filePtr->getFileNumber();
					s->insert_x = (DbUnit)insert_x;
					s->insert_y = (DbUnit)insert_y;
					s->scale = (DbUnit)scale;
					s->rotation = (DbUnit)rotation;
					s->mirror = mirror;
					s->layer = insertLayer;
					s->setParentDataList(FileDataList);

					double variance = doc->getSettings().JOIN_autoSnapSize;
					if (variance < SMALLNUMBER)
					{
						int decimals = GetDecimals(doc->getSettings().getPageUnits());
						variance = 1;
						for (int i=0; i<decimals; i++)
							variance *= 0.1;
					}

					BOOL Head = TRUE;
					while (TRUE)
					{
						int result = TryToConnect(Head, s, variance, doc);

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

            DataStruct *newData = CopyTransposeEntity(data, insert_x, insert_y, rotation, mirror, scale, newLayer, TRUE);
            newData->setLayerIndex(newLayer);
				newData->setGraphicClass(graphicClassNormal);
            FileDataList->AddTail(newData);

				// Expand the package outline by the size of Component Outline Distance
				bool res = false;
				if (newData->getPolyList())
				{
					if (newData->getPolyList()->GetCount() > 1)
					{
						ErrorMessage("More than one poly found in Board outline during CreateBoardOutlineDistant, no board outline distance check is created.", "Error");
						return -1;
					}
					else
					{
						CPoly *poly = newData->getPolyList()->GetHead();
						if (poly)
						{
							res = poly->shrink(distance,doc->getPageUnits());

							if (res == true)
							{
								poly->setFilled(FALSE);
								newData->setAttrib(doc->getCamCadData(), boardOutlineKey, valueTypeNone, NULL, attributeUpdateOverwrite, NULL);
								poly->setClosed(true);
							}
							else
							{
								CString errorMessage = "";
								if (bottom)
									errorMessage.Format("The value (%0.3f) of bottom Board Outline Distance check is too large.  Enter a smaller value.", distance);
								else
									errorMessage.Format("The value (%0.3f) of top Board Outline Distance check is too large.  Enter a smaller value.", distance);
								ErrorMessage(errorMessage, "Error");
								return -1;
							}
						}
					}
				}

				if (res == false)
				{
					// Can't copy the package outline
					//filePtr->getBlock()->RemoveDataFromList(doc, newData);
               doc->removeDataFromDataList(*(filePtr->getBlock()),newData);
				}
			}
		}

      DataList->GetNext(pos);
   }

	return 0;
} 

int TryToConnect(BOOL Head, SelectStruct *s, double variance, CCEtoODBDoc *doc)
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

/******************************************************************************
* PntsOverlap
*/
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance)
{ return ((fabs(p1->x - p2->x) < variance) && (fabs(p1->y - p2->y) < variance)); }

/******************************************************************************
* CheckForBadVariance
*/
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

static void CreateListOfRegion(CCEtoODBDoc *doc, int topLayer, int botLayer, CSMList &smTopList, CSMList &smBotList, bool boardOutline)
{
	if (doc == NULL)
		return;

	Mat2x2 m;
   RotMat2(&m, 0);

	float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));
	WORD boardOutlineKey = (WORD)doc->RegisterKeyWord("Board Outline", 0, valueTypeNone);

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      if (!file->isShown())
         continue;

      for (CDataListIterator dataList(*file->getBlock(), dataTypePoly); dataList.hasNext();)
      {
			DataStruct *data = dataList.getNext();
			if (boardOutline == true)
			{
				Attrib* attrib = NULL;
				if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(boardOutlineKey, attrib))
					continue;
			}

         //double maxExtent = data->getPolyList()->getExtent().getMaxSize();
         //if (maxExtent <= 0.) maxExtent = 1.;
         //float pixelsPerPageUnit = (float)(getNumberOfPixels()/maxExtent);

			if (topLayer > 0 && data->getLayerIndex() == topLayer)
			{
				ERPStruct *sm = new ERPStruct;

				PolyExtents(doc, data->getPolyList(), &sm->extents, 1, 0, 0, 0, &m, 0);
				sm->region = RegionFromPolylist(doc, data->getPolyList(), scaleFactor); //pixelsPerPageUnit);
				sm->pPolyList = data->getPolyList();

				smTopList.AddTail(sm);
			}
			else if (botLayer > 0 && data->getLayerIndex() == botLayer)
			{
				ERPStruct *sm = new ERPStruct;

				PolyExtents(doc, data->getPolyList(), &sm->extents, 1, 0, 0, 0, &m, 0);
				sm->region = RegionFromPolylist(doc, data->getPolyList(), scaleFactor); // pixelsPerPageUnit);
				sm->pPolyList = data->getPolyList();

				smBotList.AddTail(sm);
			}
		}
	}
}

static void FreeListOfRegion(CSMList &smList)
{
   // Free soldermask regions
   POSITION smPos = smList.GetHeadPosition();
   while (smPos) // for each soldermask
   {
      ERPStruct *sm = smList.GetNext(smPos);
      delete sm->region;
		sm->pPolyList = NULL;
      delete sm;
   }
}

/******************************************************************************
* ProcessExposeMetal
*/
static void ProcessExposeMetal(CCEtoODBDoc *doc, FileStruct *pcbFile, int topExposeMetalLayer,
														 int botExposeMetalLayer, CSMList &smTopList, CSMList &smBotList)
{
	if (doc == NULL)
		return;

	// Get exposed metal on the PCB file
	POSITION pos = pcbFile->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		POSITION tmpPos = pos;
		DataStruct *copperData = pcbFile->getBlock()->getDataList().GetNext(pos);		

		// -2 = initialized value
		// -1 = no soldermask to expose metal so delete the metal
		//  0 = has soldermask to expose metal
		int res = -2;	

		if (copperData->getLayerIndex() == topExposeMetalLayer)
			res = GetMetalExposedRegion(doc, copperData, &smTopList);
		else if (copperData->getLayerIndex() == botExposeMetalLayer)
			res = GetMetalExposedRegion(doc, copperData, &smBotList);

		if (res == -1)
		{
			// remove entire copper if no exposed region
			RemoveOneEntityFromDataList(doc, &pcbFile->getBlock()->getDataList(), copperData, tmpPos); 
		}
	}
}

/******************************************************************************
* ProcessExposeMetalByCompOutline
*/
static void xxxDeadCodexxxProcessExposeMetalByCompOutline(CCEtoODBDoc *doc, FileStruct *pcbFile, int topExposeMetalLayer, int botExposeMetalLayer,
														  CSMList &smTopList, CSMList &smBotList, CExposedDataMap *exposedDataMap)
{
	if (doc == NULL)
		return;

	WORD keyword = (WORD)doc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);

	POSITION pos = exposedDataMap->GetStartPosition();
	while (pos)
	{
		CString key = "";
		CExposedData *exposedData = NULL;
		exposedDataMap->GetNextAssoc(pos, key, exposedData);
		if (exposedData == NULL)
			continue;

		DataStruct *copperData = exposedData->GetExposedData();
		if (copperData == NULL)
			continue;

		// -2 = initialized value
		// -1 = no soldermask to expose metal so delete the metal
		//  0 = has soldermask to expose metal
		int res = -2;	  
		if (copperData->getLayerIndex() == topExposeMetalLayer)
			res = GetDiffMetalExposedRegion(doc, copperData, &smTopList);
		else if (copperData->getLayerIndex() == botExposeMetalLayer)
			res = GetDiffMetalExposedRegion(doc, copperData, &smBotList);

		if (res == -1)
		{
			// remove entire copper if no exposed region
         RemoveOneEntityFromDataList(doc, &pcbFile->getBlock()->getDataList(), exposedData->GetExposedData());
//			RemoveOneEntityFromDataList(doc, &pcbFile->getBlock()->getDataList(), exposedData->GetExposedData(),  exposedData->GetExposedDataPosition()); 

         exposedData->SetFailureRC(DFT_AA_RC_COMP_OUTLINE); //*rcfSetErrorMessage(DFT_AA_RC_COMP_OUTLINE);
			exposedData->SetExposedData(NULL);
//         exposedData->SetExposedDataPosition(NULL);
		}
	}
}

/******************************************************************************
* GetMetalExposedRegion
*/
static int GetMetalExposedRegion(CCEtoODBDoc *doc, DataStruct *copperData, CSMList *smList)
{
	int res = 0;
	Mat2x2 m;
   RotMat2(&m, 0);

   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));

   //double maxExtent = copperData->getPolyList()->getExtent().getMaxSize();
   //if (maxExtent <= 0.) maxExtent = 1.;
   //float pixelsPerPageUnit = (float)(getNumberOfPixels()/maxExtent);

	ERPStruct copper;
	copper.region = NULL;
	copper.pPolyList = NULL;
   PolyExtents(doc, copperData->getPolyList(), &copper.extents, 1, 0, 0, 0, &m, 0);

	ERPStruct solderable;
	solderable.region = new Region();
	solderable.pPolyList = NULL;
	solderable.extents.reset();

	bool isCopperCircle = false;
	int combineRegCnt = 0;
	double cx = 0.0;
	double cy = 0.0;
	double radius = 0.0;

   POSITION smPos = smList->GetHeadPosition();
   while (smPos) // for each soldermask
   {
      ERPStruct *sm = smList->GetNext(smPos);

      if (sm->extents.left > copper.extents.right || copper.extents.left > sm->extents.right || 
          sm->extents.bottom > copper.extents.top || copper.extents.bottom > sm->extents.top)
         continue;  

		if (copper.region == NULL)
		{
         copper.region = RegionFromPolylist(doc, copperData->getPolyList(), scaleFactor); // pixelsPerPageUnit);
			copper.pPolyList = copperData->getPolyList();

			if (copper.pPolyList->GetCount() == 1)
				if (PolyIsCircle(copper.pPolyList->GetHead(), &cx, &cy, &radius))
					isCopperCircle = true;
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
			if (copperData->getPolyList() != solderable.pPolyList)
			{
				FreePolyList(copperData->getPolyList());
				copperData->getPolyList() = new CPolyList(*solderable.pPolyList);
			}
		}
      else if (!AreRegionsEqual(solderable.region, copper.region))
		{
			// copper not completely covered -> replace copper entity with solderableRegion

			//CPolyList *newPolylist = solderable.region->GetPolys(scaleFactor);
         CRegionPolygon regionPolygon(*doc,*(solderable.region),scaleFactor); // pixelsPerPageUnit);
         CPolyList *newPolylist = regionPolygon.getSmoothedPolys();

         if (newPolylist)
         {
            FreePolyList(copperData->getPolyList());
            copperData->getPolyList() = newPolylist;

				POSITION pos = copperData->getPolyList()->GetHeadPosition();
				while (pos)
				{
					CPoly *poly = copperData->getPolyList()->GetNext(pos);
					if (poly)
					{
						poly->setWidthIndex(-1);
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
//   delete solderableRegion;
	return res;
}

/******************************************************************************
* GetDiffMetalExposedRegion
*/
static int GetDiffMetalExposedRegion(CCEtoODBDoc *doc, DataStruct *copperData, CSMList *smList)
{
	int res = 0;
	Mat2x2 m;
   RotMat2(&m, 0);

   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));
	
   //double maxExtent = copperData->getPolyList()->getExtent().getMaxSize();
   //if (maxExtent <= 0.) maxExtent = 1.;
   //float pixelsPerPageUnit = (float)(getNumberOfPixels()/maxExtent);

   ExtentRect copperExtents;
   PolyExtents(doc, copperData->getPolyList(), &copperExtents, 1, 0, 0, 0, &m, 0);

   Region *copperRegion = NULL;
   Region *solderableRegion = NULL;

   POSITION smPos = smList->GetHeadPosition();
   while (smPos) // for each soldermask
   {
      ERPStruct *sm = smList->GetNext(smPos);

      if (sm->extents.left > copperExtents.right || copperExtents.left > sm->extents.right || 
          sm->extents.bottom > copperExtents.top || copperExtents.bottom > sm->extents.top)
         continue; 
		
      // calculate region and intersect with all soldermask pieces and take the union of the intersections

      if (!copperRegion) // only create the region once
		{
         copperRegion = RegionFromPolylist(doc, copperData->getPolyList(), scaleFactor); //pixelsPerPageUnit);
			solderableRegion = RegionFromPolylist(doc, copperData->getPolyList(), scaleFactor); //pixelsPerPageUnit);
		}

      Region *diffRegion = DiffRegions(solderableRegion, sm->region);
      if (diffRegion)
      {
			delete solderableRegion;
			solderableRegion = diffRegion;
      }
		else if (AreRegionsEqual(solderableRegion, sm->region))
		{
			// copper is complelely covered, no need to continue checking
			delete solderableRegion;
			delete copperRegion;
			res = -1;
			return res;
		}
   }

	if (solderableRegion != NULL)
	{
		if (!solderableRegion->IsEmpty())
		{
			if (!AreRegionsEqual(solderableRegion, copperRegion))
			{
				// copper is partically covered -> replace copper entity with solderableRegion

				//CPolyList *newPolylist = solderableRegion->GetPolys(scaleFactor);
            CRegionPolygon regionPolygon(*doc,*solderableRegion,scaleFactor); //pixelsPerPageUnit);
            CPolyList *newPolylist = regionPolygon.getSmoothedPolys();

				if (newPolylist)
				{
					FreePolyList(copperData->getPolyList());
					copperData->getPolyList() = newPolylist;

					POSITION pos = copperData->getPolyList()->GetHeadPosition();
					while (pos)
					{
						CPoly *poly = copperData->getPolyList()->GetNext(pos);
						if (poly)
						{
							poly->setWidthIndex(-1);
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

/******************************************************************************
* GetCombinedRegion
*/
static DataStruct* GetCombinedRegion(CCEtoODBDoc* doc, DataStruct* data1, DataStruct* data2)
{
	DataStruct* combinedData = NULL;
	CPolyList* polyList1 = data1->getPolyList();
	CPolyList* polyList2 = data2->getPolyList();

	if (polyList1 == NULL && polyList2 == NULL)
	{
		combinedData = NULL;
	}
	else if (polyList1 == NULL)
	{		
		combinedData = doc->getCamCadData().getNewDataStruct(*data2, true);
	}
	else if (polyList2 == NULL)
	{
		combinedData = doc->getCamCadData().getNewDataStruct(*data1, true);
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
					combinedData = doc->getCamCadData().getNewDataStruct(*data2, true);
					areCircles = true;
				}
				else if (radius1 >= distance + radius2)
				{
					// data1 completely covers data2
					combinedData = doc->getCamCadData().getNewDataStruct(*data1, true);
					areCircles = true;
				}
			}
		}

		if (!areCircles)
		{
			// Either one or both of the polylist are not circle
			// For case #1909 changed scaleFactor to multiple by 100000.0 instead of 1000.00 to increase accuracy
			float scaleFactor = (float)(100000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));

         double maxExtent1 = data1->getPolyList()->getExtent().getMaxSize(); 
         double maxExtent2 = data2->getPolyList()->getExtent().getMaxSize();
         double maxExtent = max(maxExtent1, maxExtent2);
         if (maxExtent <= 0.) maxExtent = 1.;

         float pixelsPerPageUnit = (float)(getNumberOfPixels()/maxExtent);

			Region* region1 = RegionFromPolylist(doc, data1->getPolyList(), pixelsPerPageUnit);
			Region* region2 = RegionFromPolylist(doc, data2->getPolyList(), pixelsPerPageUnit);

			if (region1 == NULL && region2 == NULL)
			{
				combinedData = NULL;
			}
			else if (region1 == NULL)
			{
				combinedData = doc->getCamCadData().getNewDataStruct(*data2, true);
			}
			else if (region2 == NULL)
			{
				combinedData = doc->getCamCadData().getNewDataStruct(*data1, true);
			}
			else
			{
				combinedData = doc->getCamCadData().getNewDataStruct(*data1, true);
				region1->CombineWith(region2);

				//CPolyList *newPolylist = region1->GetPolys(scaleFactor);

            CRegionPolygon regionPolygon(*doc,*region1,pixelsPerPageUnit);
            CPolyList *newPolylist = regionPolygon.getSmoothedPolys();

				if (newPolylist)
				{
					FreePolyList(combinedData->getPolyList());
					combinedData->getPolyList() = newPolylist;

					POSITION pos = combinedData->getPolyList()->GetHeadPosition();
					while (pos)
					{
						CPoly *poly = combinedData->getPolyList()->GetNext(pos);
						if (poly)
						{
							poly->setWidthIndex(-1);
							poly->setFilled(poly->isClosed());
						}
					}
				}
 
			}
		}
	}

	return combinedData;
}

void CleanUpUnusedLayer(CCEtoODBDoc *doc)
{
	// delete Combine Metal Top
	LayerStruct *tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_METAL_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	// delete Combine Metal Bottom
	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_METAL_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	// delete Combine Soldermask Top
	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_SOLDERMASK_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);
	
	// delete Combine Soldermask Bottom
	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_COMBIN_SOLDERMASK_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	// delete Soldermask Analysis Top
	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_SM_ANALYSYS_TOP);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);

	// delete Soldermask Analysis Bot
	tmpLayer = doc->FindLayer_by_Name(DFT_LAYER_SM_ANALYSYS_BOT);
	if (tmpLayer)
		DeleteLayer(doc, tmpLayer, TRUE);
}

/******************************************************************************
* OnSmPrep
*/
void CCEtoODBDoc::OnSmPrep() 
{
   POSITION pos = getFileList().GetHeadPosition();
   if (!pos)
   {
      ErrorMessage("No File Loaded!", "SolderMask Process");
      return;
   }

   SoldermaskAnalyzer dlg;
   dlg.doc =this;
   dlg.m_deleteAttr = TRUE;
   dlg.m_explode = TRUE;
   if (dlg.DoModal() != IDOK)
      return;
}

/******************************************************************************
* OnSmProcess
*/
void CCEtoODBDoc::OnSmProcess() 
{
   FlushQueue();
   SoldermaskCheck(this, FALSE);
}

/******************************************************************************
* OnSmProcessNegative
*/
void CCEtoODBDoc::OnSmProcessNegative() 
{
   FlushQueue();
   SoldermaskCheck(this, TRUE);
}

/******************************************************************************
* SoldermaskCheck
*/
void SoldermaskCheck(CCEtoODBDoc *doc, BOOL Negative)
{
   FileStruct *file = NULL;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *fp = doc->getFileList().GetNext(filePos);
      if (fp->getBlockType() == BLOCKTYPE_PCB)
      {
         file = fp;
         break;
      }
   }

   if (!file)
   {
      ErrorMessage("No PCB File Loaded!", "SolderMask Process");
      return;
   }

   if (Platform != WINNT)
   {
      if (ErrorMessage("We recommend only using Soldermask Analyzer on Windows NT and Windows 2000.\nDo you want to continue anyways?", "WARNING!", MB_ICONEXCLAMATION | MB_YESNO) != IDYES)
         return;
   }

   CWaitCursor wait;

   generate_PINLOC(doc, file, 1);
   generate_PADSTACKACCESSFLAG(doc, 1);

   doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_SOLDERMASK, 0));

   SoldermaskSurfaceCheck(doc, file->getBlock(), TRUE, Negative);

   SoldermaskSurfaceCheck(doc, file->getBlock(), FALSE, Negative);

   SoldermaskPostProcess(doc, file);
}

/******************************************************************************
* SoldermaskPostProcess

SM Graphics | POS TH | POS T SMD | POS B SMD | NEG TH | NEG T SMD | NEG B SMD
 TOP  BOT   |        |           |           |        |           |
-----------------------------------------------------------------------------
  Y    -    |  TOP   |   TOP     |   NONE    |  BOT   |   NONE    |   BOT
  -    Y    |  BOT   |   NONE    |   BOT     |  TOP   |   TOP     |   NONE
  Y    Y    |  BOTH  |   TOP     |   NONE    |  NONE  |   NONE    |   NONE
  -    -    |  NONE  |   NONE    |   BOT     |  BOTH  |   TOP     |   BOT
*/
void SoldermaskPostProcess(CCEtoODBDoc *doc, FileStruct *file)
{
   WORD kwSMD = doc->IsKeyWord(ATT_SMDSHAPE, 0);
   WORD kwSM = doc->IsKeyWord(ATT_SOLDERMASK, 0);

   const int TOP = doc->RegisterValue(testaccesslayers[2]);
   const int BOT = doc->RegisterValue(testaccesslayers[1]);
   const int NONE = doc->RegisterValue(testaccesslayers[3]);
   const int BOTH = doc->RegisterValue(testaccesslayers[0]);

   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos)
      {
         CompPinStruct *cp = net->getNextCompPin(cpPos);

         Attrib* smdAttrib;
         Attrib* attrib = get_attvalue(cp->getAttributesRef(), kwSM);

         if (!attrib)
            continue;

         // SMD
         if (cp->getPadstackBlockNumber() != -1 && doc->getBlockAt(cp->getPadstackBlockNumber())->getAttributesRef() && 
               doc->getBlockAt(cp->getPadstackBlockNumber())->getAttributesRef()->Lookup(kwSMD, smdAttrib))
         {
            if (cp->getMirror()) // Bottom SMD
            {
               if (attrib->getStringValueIndex() == BOTH)
                  attrib->setStringValueIndex(BOT);
               else if (attrib->getStringValueIndex() == TOP)
                  attrib->setStringValueIndex(NONE);
            }
            else // Top SMD
            {
               if (attrib->getStringValueIndex() == BOTH)
                  attrib->setStringValueIndex(TOP);
               else if (attrib->getStringValueIndex() == BOT)
                  attrib->setStringValueIndex(NONE);
            }
         }
      }
   }
}

/******************************************************************************
* explode_soldermask
*/
static void explode_soldermask(CCEtoODBDoc *doc, CString topLayer, CString bottomLayer, BOOL deleteOrig)
{
   //int newSMTop = Graph_Level(topLayer, "", FALSE);
   //doc->LayerArray[newSMTop]->setLayerType(LAYTYPE_MASK_TOP);
   //int newSMBottom = Graph_Level(bottomLayer, "", FALSE);
   //doc->LayerArray[newSMBottom]->setLayerType(LAYTYPE_MASK_BOTTOM);
   //doc->LayerArray[newSMTop]->setMirroredLayerIndex(newSMBottom);
   //doc->LayerArray[newSMBottom]->setMirroredLayerIndex(newSMTop);

   //POSITION filePos = doc->FileList.GetHeadPosition();
   //while (filePos)
   //{
   //   FileStruct *file = doc->FileList.GetNext(filePos);

   //   if (!file->isShown())
   //      continue;

   //   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   //   {
   //      if (i == newSMTop || i == newSMBottom)
   //         continue;

   //      LayerStruct *layer = doc->LayerArray[i];

   //      if (!layer)
   //         continue;
   //      
   //      switch (layer->getLayerType())
   //      {
   //      case LAYTYPE_MASK_TOP:
   //         SM_CallRecursion(doc, file, i, newSMTop);
   //         break;
   //      
   //      case LAYTYPE_MASK_BOTTOM:
   //         SM_CallRecursion(doc, file, i, newSMBottom);
   //         break;
   //      
   //      case LAYTYPE_MASK_ALL:
   //         SM_CallRecursion(doc, file, i, newSMTop);
   //         SM_CallRecursion(doc, file, i, newSMBottom);
   //         break;
   //      }
   //   }
   //}

   //if (deleteOrig)
   //{
   //   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   //   {
   //      if (i == newSMTop || i == newSMBottom)
   //         continue;

   //      LayerStruct *layer = doc->LayerArray[i];

   //      if (!layer)
   //         continue;
   //      
   //      switch (layer->getLayerType())
   //      {
   //      case LAYTYPE_MASK_TOP:
   //      case LAYTYPE_MASK_BOTTOM:
   //      case LAYTYPE_MASK_ALL:
   //         DeleteLayer(doc, layer, TRUE);
   //         break;
   //      }
   //   }
   //}
}

/////////////////////////////////////////////////////////////////////////////
// SoldermaskAnalyzer dialog
SoldermaskAnalyzer::SoldermaskAnalyzer(CWnd* pParent /*=NULL*/)
   : CDialog(SoldermaskAnalyzer::IDD, pParent)
{
   //{{AFX_DATA_INIT(SoldermaskAnalyzer)
   m_bottomLayer = _T("SM Bottom");
   m_deleteAttr = FALSE;
   m_deleteOrig = TRUE;
   m_explode = FALSE;
   m_graphicsFromAttr = FALSE;
   m_topLayer = _T("SM Top");
   //}}AFX_DATA_INIT
}

void SoldermaskAnalyzer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SoldermaskAnalyzer)
   DDX_Text(pDX, IDC_BOTTOM_LAYER, m_bottomLayer);
   DDX_Check(pDX, IDC_DELETE_ATTR, m_deleteAttr);
   DDX_Check(pDX, IDC_DELETE_ORIG, m_deleteOrig);
   DDX_Check(pDX, IDC_EXPLODE, m_explode);
   DDX_Check(pDX, IDC_GRAPHICS_FROM_ATTR, m_graphicsFromAttr);
   DDX_Text(pDX, IDC_TOP_LAYER, m_topLayer);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SoldermaskAnalyzer, CDialog)
   //{{AFX_MSG_MAP(SoldermaskAnalyzer)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoldermaskAnalyzer message handlers
void SoldermaskAnalyzer::OnOK() 
{
   //if (!doc->FileList.GetCount())
   //{
   //   ErrorMessage("No File Loaded!", "SolderMask Process");
   //   return;
   //}

   //UpdateData();
   //if (m_explode)
   //{
   //   if (m_topLayer.IsEmpty() || m_bottomLayer.IsEmpty())
   //   {
   //      ErrorMessage("Enter a layer name");
   //      return;
   //   }

   //   doc->UnselectAll(FALSE);
   //   explode_soldermask(doc, m_topLayer, m_bottomLayer, m_deleteOrig);
   //   // ErrorMessage("Explode Soldermask done.", "SolderMask");  
   //}
   //if (m_deleteAttr)
   //{
   //   // delete all soldermask attributes.
   //   doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_SOLDERMASK, 0));
   //}

   //doc->UpdateAllViews(NULL);

   //CDialog::OnOK();
}

void FF_SM_DrawBlock(SMApList *smApList, BlockStruct *block, double insert_x, double insert_y, double rotation, double scale, int mirror, 
      int insertLayer, int drawLayer, BOOL FoundFlash, CString compName, CString pinName, DataStruct *padstack);
/******************************************************************************
* SoldermaskSurfaceCheck
*/
void SoldermaskSurfaceCheck(CCEtoODBDoc *doc, BlockStruct *block, BOOL Top, BOOL Negative)
{
   double resolution = 2 * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());

   if (!FF_Start(doc, resolution))
      return;

   CList<int,int> padList;
   CList<int,int> smList;

   // get pad and sm layers
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (!layer)
         continue;

      switch (layer->getLayerType())
      {
      case LAYTYPE_PAD_ALL:
      case LAYTYPE_PAD_OUTER:
         padList.AddTail(i);
         break;

      case LAYTYPE_PAD_TOP:
      case LAYTYPE_SIGNAL_TOP:
         if (Top)
            padList.AddTail(i);
         break;

      case LAYTYPE_PAD_BOTTOM:
      case LAYTYPE_SIGNAL_BOT:
         if (!Top)
            padList.AddTail(i);
         break;

      case LAYTYPE_MASK_TOP:
         if (Top)
            smList.AddTail(i);
         break;

      case LAYTYPE_MASK_BOTTOM:
         if (!Top)
            smList.AddTail(i);
         break;
      }
   }

   SMApList smApList;

   // draw pads
   POSITION pos = padList.GetHeadPosition();
   while (pos)
      FF_SM_DrawBlock(&smApList, block, 0, 0, 0, 1, 0, -1, padList.GetNext(pos), FALSE, "", "", NULL);

//FF_ShowBitmap();

   // draw sm
   pos = smList.GetHeadPosition();
   while (pos)
      FF_DrawBlock(block, 0, 0, 0, 1, 0, FALSE, "", "", -1, smList.GetNext(pos), TRUE);

//FF_ShowBitmap();

   int count = 0;
   pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (smList.Find(data->getLayerIndex()))
         count++;
   }

   // fill sm
   pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (smList.Find(data->getLayerIndex()))
      {
         switch (data->getDataType())
         {
            case T_INSERT:
               if (!FF_Query(data->getInsert()->getOriginX(), data->getInsert()->getOriginY()))
                     FF_Fill(data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), FALSE);
               break;

            case dataTypePoly:
               if (!FF_Query(data->getPolyList()->GetHead()->getPntList().GetHead()->x, data->getPolyList()->GetHead()->getPntList().GetHead()->y))
                  FF_Fill(data->getPolyList()->GetHead()->getPntList().GetHead()->x, data->getPolyList()->GetHead()->getPntList().GetHead()->y, FALSE);
               break;
         }
      }
   }

//FF_ShowBitmap();

   WORD kw = doc->IsKeyWord(ATT_SOLDERMASK, 0);
   // query pads
   pos = smApList.GetHeadPosition();
   while (pos)
   {
      SMApStruct *smAp = smApList.GetNext(pos);

      if (Negative)
      {
         if (Top)
         {
            if (!FF_Query(smAp->x, smAp->y)) //  SM not on Top
               doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[0], attributeUpdateOverwrite, NULL); // BOTH
            else
               doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[1], attributeUpdateOverwrite, NULL); // BOTTOM
         }
         else
         {
            Attrib* attrib = get_attvalue(*smAp->map, kw);

            if (!attrib) // doesn't have an attribute from Top
            {
               if (!FF_Query(smAp->x, smAp->y)) // SM not on Bottom
                  doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[0], attributeUpdateOverwrite, NULL); // BOTH
               else
                  doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[2], attributeUpdateOverwrite, NULL); // TOP
            }

            else if (attrib->getStringValue() != testaccesslayers[3]) // if not already NONE
            {
               if (attrib->getStringValue() == testaccesslayers[0]) // BOTH
               {
                  if (!FF_Query(smAp->x, smAp->y)) // SM not on Bottom
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[0], attributeUpdateOverwrite, NULL); // BOTH
                  else
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[2], attributeUpdateOverwrite, NULL); // TOP
               }
               else // result from top was BOTTOM
               {
                  if (!FF_Query(smAp->x, smAp->y)) // SM not on Bottom
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[1], attributeUpdateOverwrite, NULL); // BOTTOM
                  else
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[3], attributeUpdateOverwrite, NULL); // NONE
               }
            }
         }
      }
      else // Positive
      {
         if (Top)
         {
            if (FF_Query(smAp->x, smAp->y)) //  SM on Top
               doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[2], attributeUpdateOverwrite, NULL); // TOP
            else
               doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[3], attributeUpdateOverwrite, NULL); // NONE
         }
         else
         {
            Attrib* attrib = get_attvalue(*smAp->map, kw);

            if (!attrib) // doesn't have an attribute from Top
            {
               if (FF_Query(smAp->x, smAp->y)) // SM on Bottom
                  doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[1], attributeUpdateOverwrite, NULL); // BOTTOM
               else
                  doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[3], attributeUpdateOverwrite, NULL); // NONE
            }
            else if (attrib->getStringValue() != testaccesslayers[0]) // if not already BOTH
            {
               if (attrib->getStringValue() == testaccesslayers[2]) // TOP
               {
                  if (FF_Query(smAp->x, smAp->y)) // SM on Bottom
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[0], attributeUpdateOverwrite, NULL); // BOTH
                  else
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[2], attributeUpdateOverwrite, NULL); // TOP
               }
               else // result from top was NONE
               {
                  if (FF_Query(smAp->x, smAp->y)) // SM on Bottom
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[1], attributeUpdateOverwrite, NULL); // BOTTOM
                  else
                     doc->SetAttrib(smAp->map, kw, valueTypeString, testaccesslayers[3], attributeUpdateOverwrite, NULL); // NONE
               }
            }
         }
      }

      delete smAp;
   }

   FF_End();
}

