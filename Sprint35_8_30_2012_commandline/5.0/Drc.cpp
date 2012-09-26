
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#include "stdafx.h"
#include "drc.h"
#include "find.h"
#include "graph.h"
#include "pcbutil.h"
#include "response.h"
#include "attrib.h"
#include "EntityNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *FailureRanges[] = 
{
   "Critical",
   "Marginal",
};

char *AlgTypes[] =
{
   "Generic",
   "Probable Short Pin to Pin",
   "Probable Short Pin to Feature",
   "Probable Short Feature to Feature",
   "Net without Probe",
   "Net without Access",
   "Component Shadow",
   "Net Collision",
   "Net Adjacency",
   "Component to Component Spacing",
   "Component to Boardoutline Spacing",
};
                                   
/******************************************************************************
* GetAlgorithmNameIndex
*/
int GetAlgorithmNameIndex(CCEtoODBDoc *doc, const char *algName)
{
   for (int i=0; i<doc->getDfmAlgorithmArraySize(); i++)
      if (!doc->getDfmAlgorithmNamesArray()[i].Compare(algName))
         return i;

   doc->getDfmAlgorithmNamesArray().SetAtGrow(doc->getDfmAlgorithmArraySize(), algName);  

   return doc->getDfmAlgorithmArraySize() - 1;
}

/******************************************************************************
* GenerateDRCMarker
*
*  this generates a global defined drcmarker geometry, which will be used in placement program
*  generation. It returns the drcmarker definition name
*/
BlockStruct* GenerateDRCMarker(CCEtoODBDoc *doc)
{         
   if (doc->currentMarker)
      return doc->currentMarker;

   doc->currentMarker = Graph_Block_Exists(doc, DRCMARKERGEOM, -1);

   if (doc->currentMarker)
      return doc->currentMarker;

   doc->currentMarker = Graph_Block_On(GBO_APPEND, DRCMARKERGEOM, -1, 0);
   doc->currentMarker->setBlockType(BLOCKTYPE_DRCMARKER);
   doc->currentMarker->setFlagBits(BL_SPECIAL);

   double f = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
   int layerNum = Graph_Level("0", "", 1);

   Graph_Circle(layerNum, 0, 0, 25*f, 0L, 0 , FALSE, FALSE);
   Graph_PolyStruct(layerNum , 0, FALSE);
   Graph_Poly(NULL,0, 0,0,0);
   Graph_Vertex(-25*f, -25*f,0.0);
   Graph_Vertex(25*f, 25*f,0.0);
   Graph_Poly(NULL,0, 0,0,0);
   Graph_Vertex(25*f, -25*f,0.0);
   Graph_Vertex(-25*f, 25*f,0.0);
   Graph_Block_Off();
   
   return doc->currentMarker;
}


/******************************************************************************
* CreateDRC
*/
DRCStruct *CreateDRC(FileStruct *file, const char *string, int drcClass, int failureRange, int algIndex, int algType)
{
   DRCStruct* drc = file->getDRCList().addDrc(string);

   //drc = new DRCStruct;
   //drc->setEntityNumber(CEntityNumber::allocate());

   //drc->getAttributesRef() = NULL;
   //drc->priority = 0;
   //drc->reviewed = 0;
   //drc->x = drc->y = 0;          // this x,y is the drc error occurcance, should match the T_INSERT
   //drc->insertEntity = 0;        // points to a T_INSERT
   drc->setAlgorithmType(algType);
   //drc->voidPtr = NULL;

   //drc->string = string;
   drc->setDrcClass(drcClass);
   drc->setFailureRange(failureRange);
   drc->setAlgorithmIndex(algIndex); // -1 is default
   //file->getDRCList().AddTail(drc);

   SendResponse("DRC Added", FailureRanges[failureRange]);

   return drc;
}


/*****************************************************************************/
/* AddDRC
   - Adds (DRCStruct / Insert) Pair
   - algType is Algorithm type from drc.h -- updated in drc.cpp AlgTypes[]
   - algIndex is a name index. 2 different names could have the same type
*/
DRCStruct *AddDRC(CCEtoODBDoc *doc, FileStruct *file, 
      double x, double y, const char *string, int drcClass, int failureRange, int algIndex, int algType)
{
   // DRC in DRCList
   DRCStruct *drc = CreateDRC(file, string, drcClass, failureRange, algIndex, algType);
   drc->setOriginX(x);
   drc->setOriginY(y);
   
   return drc;
}

/*****************************************************************************/
/* AddDRC
   - Adds (DRCStruct / Insert) Pair
   - algType is Algorithm type from drc.h -- updated in drc.cpp AlgTypes[]
   - algIndex is a name index. 2 different names could have the same type
*/
DRCStruct *AddDRCAndMarker(CCEtoODBDoc *doc, FileStruct *file, 
      double x, double y, const char *string, int drcClass, int failureRange, int algIndex, int algType, 
      const char *layerName, DataStruct **insert)
{
   // DRC Marker Geometry
   BlockStruct* drcBlock = GenerateDRCMarker(doc);
                             

   // DRC in DRCList
   DRCStruct *drc = CreateDRC(file, string, drcClass, failureRange, algIndex, algType);
   drc->setOriginX(x);
   drc->setOriginY(y);

   // Layer for Insert
   int layerNum;
   if (doc->FindLayer_by_Name(layerName))
      layerNum = Graph_Level(layerName, 0, 0); // already exists
   else
   {
      layerNum = Graph_Level(layerName, 0, 0);
      LayerStruct *layer = doc->FindLayer(layerNum);
      switch (failureRange)
      {
      case 0:
         layer->setColor( RGB(255, 0, 0));
         layer->setLayerType(LAYTYPE_DRC_CRITICAL);
         break;
      case 1:
         layer->setColor( RGB(255, 255, 0));
         layer->setLayerType(LAYTYPE_DRC_MARGINAL);
         break;
      }
      layer->setOriginalColor( layer->getColor());
   }


   // Insert
   doc->PrepareAddEntity(file);  
   DataStruct *data = Graph_Block_Reference(drcBlock->getName(), NULL, 0, x, y, 0.0, 0, 1.0, layerNum, TRUE);
   drc->setInsertEntityNumber(data->getEntityNumber());
   data->getInsert()->setInsertType(insertTypeDrcMarker);
   if (insert)
      *insert = data;


   return drc;
}

/******************************************************************************
* DRC_FillSimple
*/
void DRC_FillSimple(DRCStruct *drc, const char *string, int drcClass, int algIndex)
{
   drc->setString(string);
   drc->setDrcClass(drcClass);
   drc->setAlgorithmIndex(algIndex);
}

/******************************************************************************
* DRC_FillMeasure
*/
void DRC_FillMeasure(DRCStruct *drc, int type1, long entity1, int type2, long entity2, DbUnit x1, DbUnit y1, DbUnit x2, DbUnit y2)
{
   drc->setDrcClass(DRC_CLASS_MEASURE);
   DRC_MeasureStruct *meas = new DRC_MeasureStruct;
   drc->setVoidPtr((void*)meas);

   meas->entity1 = entity1;
   meas->entity2 = entity2;
   meas->type1 = type1;
   meas->type2 = type2;
   meas->x1 = x1;
   meas->y1 = y1;
   meas->x2 = x2;
   meas->y2 = y2;
}

/******************************************************************************
* DRC_FillNets
*/
void DRC_FillNets(DRCStruct *drc, CString netname)
{
   drc->setDrcClass(DRC_CLASS_NETS);

   CStringList *stringList;

   if (!drc->getVoidPtr())
      drc->setVoidPtr(new CStringList);

   stringList = (CStringList*) drc->getVoidPtr();

   if (!stringList->Find(netname))
      stringList->AddTail(netname);
}

/******************************************************************************
* ClearDRCList
*/
void ClearDRCList(CCEtoODBDoc *doc)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos)
      {
         DRCStruct *drc = file->getDRCList().GetNext(drcPos);
         RemoveOneDRC(doc, drc, file);
      }
      file->getDRCList().empty();
   }
}

void ClearDRCListForAlogorithm(CCEtoODBDoc *doc,int algorithmIndex)
{  
   for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      
      for (POSITION drcPos = file->getDRCList().GetHeadPosition();drcPos != NULL;)
      {
         POSITION previousDrcPos = drcPos;

         DRCStruct *drc = file->getDRCList().GetNext(drcPos);

         if (drc->getAlgorithmIndex() == algorithmIndex)
         {
            RemoveOneDRC(doc,previousDrcPos,file);
         }
      }
   }
}

/******************************************************************************
* RemoveOneDRC
*/
int RemoveOneDRC(CCEtoODBDoc *doc, DRCStruct *drc, FileStruct *file)
{
   if (drc == NULL)
   {
      return -1;
   }

   POSITION drcPos = file->getDRCList().Find(drc);

   return RemoveOneDRC(doc,drcPos,file);
}

int RemoveOneDRC(CCEtoODBDoc *doc,POSITION drcPos,FileStruct *file)
{
   if (drcPos == NULL)
   {
      return -1;
   }

   DRCStruct* drc = file->getDRCList().GetAt(drcPos);

   if (drc == NULL)
   {
      return -1;
   }

   // remove from DRCList
   //file->getDRCList().RemoveAt(drcPos);

   // remove insert
   if (drc->getInsertEntityNumber())
   {
      CDataList *dataList;
      DataStruct *data = FindDataEntity(doc, drc->getInsertEntityNumber(), &dataList, NULL);
      if (data)
      {
         SelectStruct s;
         s.setData(data);
         s.setParentDataList(dataList);
         s.insert_x = file->getInsertX();
         s.insert_y = file->getInsertY();
         s.rotation = file->getRotation();
         s.mirror = file->isMirrored();
         s.scale = file->getScale();
         s.layer = -1;
         doc->DrawEntity(&s, -1, TRUE);
         RemoveOneEntityFromDataList(doc, dataList, data);
      }
   }

   //if (drc->getAttributesRef())
   //{
   //   delete drc->getAttributesRef();
   //   drc->getAttributesRef() = NULL;
   //}

   switch (drc->getDrcClass())
   {
   case DRC_CLASS_SIMPLE:
   case DRC_CLASS_NO_MARKER:
      break;
   case DRC_CLASS_MEASURE:
      {
         DRC_MeasureStruct *meas = (DRC_MeasureStruct*)drc->getVoidPtr();
         delete meas;
      }
      break;
   case DRC_CLASS_NETS:
      {
         CStringList *stringList = (CStringList*) drc->getVoidPtr();
         delete stringList;
      }
      break;
   }

   // free memory
   file->getDRCList().deleteAt(drcPos);
   //delete drc;

   // unselect all
   doc->UnselectAll(FALSE);

   return 0;
}

void ShowAllDRCs(CCEtoODBDoc *doc, BOOL Show)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;
         
         if (data->getInsert()->getInsertType() == INSERTTYPE_DRCMARKER)
            data->setHidden(!Show);
      }
   }
}
