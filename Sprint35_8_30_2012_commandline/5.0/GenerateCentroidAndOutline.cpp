// $Header: /CAMCAD/5.0/GenerateCentroidAndOutline.cpp 87    5/29/07 6:13p Lynn Phung $

// 
// Source file name is misleading, Generate Centroids is not here anymore.
// It has moved in with the Data Doctor.
// Other non-GenCentroids stuff that was here is still here.
//

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "ccview.h"
#include <math.h>
#include "graph.h"
#include "DFT_Outl.h"
#include "GenerateCentroidAndOutline.h"
#include <float.h>
#include "outline.h"
#include "extents.h"
#include "dft_func.h"
#include "pcbutil.h"
#include "Attrib.h"
#include "polylib.h"
#include "api.h"
#include "Centroid.h"

#include "DataDoctor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static short UNIT = 1;
static short DEFAULT_ALGORITHM = algBodyOutline;
static CMapPtrToPtr map; // map used to check if rotation offset is already done

static DataStruct *AddPackageOutline(CCEtoODBDoc *doc, CPntList *pntList, BlockStruct *block, int rectangle, bool fillFlag);

bool block_extents(CCEtoODBDoc *doc, double *lxmin, double *lxmax, double *lymin, double *lymax, CDataList *DataList,     
      double insert_x, double insert_y, double rotation, int mirror, double scale, int insertLayer, BOOL onlyVisible);

void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);

////////////////////////////////////////////////////////////////////////////////////////////
// Other Functions
////////////////////////////////////////////////////////////////////////////////////////////

// defined in Sch_Link.cpp
BOOL GetLicenseString(CLSID clsid, BSTR &licStr);


/////////////////////////////////////////////////////////////////////////////

OutlineAlgorithmTag stringToOutlineAlgTag(CString str)
{
   // For incoming string we support space or dash as equivalent
   str.Replace("-", " ");

   if (str.CompareNoCase("Pin Center") == 0 || str.CompareNoCase("Pin Centers") == 0)  // Both plural and singular floating around out there
      return algPinCenters;
   if (str.CompareNoCase("Pin Extents") == 0)
      return algPinExtents;
   if (str.CompareNoCase("Inside Pads") == 0)
      return algInsidePads;
   if (str.CompareNoCase("Body Outline") == 0 || str.CompareNoCase("Component Outline") == 0)
      return algBodyOutline;
   if (str.CompareNoCase("Custom") == 0)
      return algCustom;
   if (str.CompareNoCase("XY Value") == 0)
      return algUserXY;
   if (str.CompareNoCase("Pin Body Extents") == 0)
      return algPinBodyExtents;

   return algUnknown;
}

CString outlineAlgTagToString(OutlineAlgorithmTag tag)
{
   // Favors the form with space, not dash

   switch(tag)
   {
      case algBodyOutline:
         return "Body Outline";
      case algPinCenters:
         return "Pin Center"; // not plural for backward compatibility
      case algPinExtents:
         return "Pin Extents";
      case algInsidePads:
         return "Inside Pads";
      case algCustom:
         return "Custom";
      case algUserXY:
         return "XY Value";
   }

   return "Unknown";
}

/////////////////////////////////////////////////////////////////////////////

/******************************************************************************
* OnTestabilityOutlinegenerator
   algorithm 0 = classic
             1 = inside pins
             2 = cover pins
             3 = from realpart
*/
void CCEtoODBDoc::OnTestabilityOutlinegenerator() 
{
   // Generic message informing feature has moved to DD tab. No offer of old interface.
   CString msg;
   msg += "Generate Package Outline has moved to a \"Packages\" tab in the Data Doctor.\n";
   msg += "Click OK to go there now.\n";
   if (MessageBox(NULL, msg, "Generate Package Outlines", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
   {
      // Clicked OK
      CDataDoctor dataDoctor(*this);
      dataDoctor.doModal(DdPackagesPage);
      return;
   }

   // Did not click OK, do nothing
}

/******************************************************************************
* OnGeometryEnhancer
*/
void CCEtoODBDoc::OnGeometryEnhancer() 
{
   // Generic message informing feature has moved to DD tab. No offer of old interface.
   CString msg;
   msg += "Generate Component Centroid has moved to a \"Centroids\" tab in the Data Doctor.\n";
   msg += "Click OK to go there now.\n";
   if (MessageBox(NULL, msg, "Generate Centroids", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
   {
      // Clicked OK
      CDataDoctor dataDoctor(*this);
      dataDoctor.doModal(DdCentroidsPage);
   }

   // Did not click OK, do nothing.
}

/******************************************************************************
* GetPackageOutline
*/
DataStruct* GetPackageOutline(CDataList *DataList)
{
   // This used to iterate through all data and count outline entities, but
   // returned value as BOOL. Waste of time to iterate all data.
   // Really only need to find one outline entity, if it has at least one is has an outline.

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == dataTypePoly && 
         (data->getGraphicClass() == graphicClassPackageOutline || // Old style DFT Package Outline
         data->getGraphicClass() == graphicClassPackageBody))      // New style Real Part based, uses Body outline class
      {
         return data;
      }
   }

   return NULL;
}


/******************************************************************************
* HasPackageOutline
*/
bool HasPackageOutline(CDataList *DataList)
{
   return (GetPackageOutline(DataList) != NULL);
}

/******************************************************************************
* DeletePackageOutline
*/
void DeletePackageOutline(CCEtoODBDoc *doc, CDataList *DataList)
{
   // Just in case more than one ended up in there, delete until none are left.

   DataStruct *outline = NULL;

   while ( (outline = GetPackageOutline(DataList)) != NULL)
   {
      RemoveOneEntityFromDataList(doc, DataList, outline);
   }
}

/******************************************************************************
* DFT_OutlineAddPins
*/
int DFT_OutlineAddPins(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int outline_found = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == dataTypeText)
         continue;

      if (data->getDataType() == dataTypeInsert)
      {
			if (data->getInsert()->getInsertType() == insertTypeCentroid)
				continue;

         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
         {
				outline_found += Outline_Add_Aperture(data, scale, insert_x, insert_y, rotation, mirror);
         }
         else
         {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (data->getLayerIndex() != -1)
               block_layer = data->getLayerIndex();

            outline_found += DFT_OutlineAddPins(doc, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror,
						scale * data->getInsert()->getScale(), embeddedLevel+1, block_layer);
         }
      }

   }

   return outline_found;
}

int DFT_OutlineAddPins(CCEtoODBDoc& doc, CDataList& DataList, CInsertTypeMask includeInsertTypeMask, 
      double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   // Always include electrical pin
   includeInsertTypeMask.add(insertTypePin);

   int outline_found = 0;
   for (CDataListIterator dataListIterator(DataList, includeInsertTypeMask); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      if (mirror)
         block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

      BlockStruct *block = doc.getBlockAt(data->getInsert()->getBlockNumber());
      if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
			outline_found += Outline_Add_Aperture(data, scale, insert_x, insert_y, rotation, mirror);
      }
      else
      {
         int block_layer = -1;
         if (insertLayer != -1)
            block_layer = insertLayer;
         else if (data->getLayerIndex() != -1)
            block_layer = data->getLayerIndex();

            outline_found += DFT_OutlineAddPins(&doc, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror,
						scale * data->getInsert()->getScale(), embeddedLevel+1, block_layer);
      }
   }

   return outline_found;
}

/******************************************************************************
* DFT_OutlineAddData
*/
static int DFT_OutlineAddData(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, 
      long graphic_class, int layertype)
{
Mat2x2 m;
RotMat2(&m, rotation);

int outline_found = 0;

POSITION dataPos = DataList->GetHeadPosition();
while (dataPos)
{
   DataStruct *data = DataList->GetNext(dataPos);

   if (data->getDataType() == dataTypeText)
      continue;

   if (data->getDataType() != dataTypeInsert)
   {
      int layerNum;

      if (insertLayer != -1)
         layerNum = insertLayer;
      else
         layerNum = data->getLayerIndex();

      // insert has not always a layer definition.
      //if (!doc->get_layer_visible(layerNum, mirror))
      //   continue;

      LayerStruct *layer = doc->FindLayer(layerNum);
		if (layer == NULL)
			continue;

      if (layertype && layer->getLayerType() != layertype) // if layertype mentioned and layertype is not equal
         continue;
      
      if (graphic_class && data->getGraphicClass() != graphic_class) // if graphic_class mentioned and graphic class is not equal
         continue;

      Outline_Add(data, scale, insert_x, insert_y, rotation, mirror);
      outline_found++;
   }
   else
   {
      if (data->getInsert()->getInsertType() == insertTypePin || data->getInsert()->getInsertType() == insertTypeCentroid ||
         data->getInsert()->getInsertType() == insertTypeDiePin)
         continue;

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      if (mirror)
         block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (!(block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
      {
         int block_layer = -1;
         if (insertLayer != -1)
            block_layer = insertLayer;
         else if (data->getLayerIndex() != -1)
            block_layer = data->getLayerIndex();

         outline_found += DFT_OutlineAddData(doc, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(),
               embeddedLevel+1, block_layer, graphic_class, layertype);
      }
   }
}

   return outline_found;
}

/******************************************************************************
* DFTOUTL_prep_layers
*/
static int DFTOUTL_prep_layers(CCEtoODBDoc *doc)
{
   doc->StoreDocForImporting();

   // Old stuff
   int layerNum;
   layerNum = Graph_Level(DFT_OUTLINE_TOP, "", 0);
   doc->getLayerArray()[layerNum]->setLayerType(LAYTYPE_COMPONENT_DFT_TOP);
   layerNum = Graph_Level(DFT_OUTLINE_BOTTOM, "", 0);
   doc->getLayerArray()[layerNum]->setLayerType(LAYTYPE_COMPONENT_DFT_BOTTOM);

   // mirror layer
   Graph_Level_Mirror(DFT_OUTLINE_TOP, DFT_OUTLINE_BOTTOM, "");

   // New stuff
	int packageOutineTopIndex = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
	int packageOutineBotIndex = Graph_Level(RP_LAYER_PKG_BODY_BOTTOM, "", FALSE);
   Graph_Level_Mirror(RP_LAYER_PKG_BODY_TOP, RP_LAYER_PKG_BODY_BOTTOM, "");

   return 1;
}

/*****************************************************************************/
/* DFTOutlineGeometry

   returns 
      0  success
      -1 no outline found
		-2 invalid algorithm
      1	ignore outline attribute set
		2	can't overwrite already existing outline
*/
int GeneratePackageOutlineGeometry(CCEtoODBDoc *doc, BlockStruct *referenceBlk, BlockStruct *destinationBlk, OutlineAlgorithmTag algorithm, int rectangle, bool filled, int overwrite)
{
   // referenceBlk is the block that contains geometry that we are making an outline around.
   // destinationBlk is the block to put the outline poly in.

   // Note that this function will delete the existing outline before it checks if the
   // algorithm is recognized. This is "good", other code is depending on this, it is one
   // of the ways outlines are removed on purpose. Do not change it unless you change all 
   // that other code.


   if (referenceBlk == NULL || destinationBlk == NULL)
      return -2;

   int err = 0;
   double accuracy = get_accuracy(doc);

   if (is_attvalue(doc, referenceBlk->getAttributes(), ATT_TEST_IGNORE_OUTLINE, 2))
      return 1;
   
   if (referenceBlk->getBlockType() != BLOCKTYPE_PCBCOMPONENT && referenceBlk->getBlockType() != blockTypeDie)
      return -1;

   // see if Package Outline exists
   if (HasPackageOutline(&destinationBlk->getDataList()))
   {
      if (!overwrite)
         return 2;

      // This function can't generate "Custom" outlines, it only does the canned algorithms.
      // If the incoming algorithm is Custom then leave the present outline, otherwise nuke it.
      if (algorithm != algCustom)
         DeletePackageOutline(doc, &destinationBlk->getDataList());
   }

	int outlineMethodKW = doc->RegisterKeyWord(ATT_OUTLINE_METHOD, 0, valueTypeString);

   switch (algorithm)
   {
   case algPinCenters: // pin centers
      {
         double xmin, ymin, xmax, ymax;
         int pins = GetPinExtents(&referenceBlk->getDataList(), &xmin, &ymin, &xmax, &ymax);
         if (pins)
         {
            xmin = round_accuracy(xmin, accuracy);
            ymin = round_accuracy(ymin, accuracy);
            xmax = round_accuracy(xmax, accuracy);
            ymax = round_accuracy(ymax, accuracy);

				CPntList *pp = NULL;

            if (xmin < xmax && ymin < ymax)
            {
               pp = new CPntList;
               CPnt *pnt = new CPnt;
               pnt->x = (DbUnit)xmin;
               pnt->y = (DbUnit)ymin;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmax;
               pnt->y = (DbUnit)ymin;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmax;
               pnt->y = (DbUnit)ymax;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmin;
               pnt->y = (DbUnit)ymax;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmin;
               pnt->y = (DbUnit)ymin;
               pnt->bulge = 0;
               pp->AddTail(pnt);
				}
				else if (pins == 2)
				{
					double xmin2, ymin2, xmax2, ymax2;
					int outlineFound = get_insidepadsextents(doc, &referenceBlk->getDataList(), &xmin2, &ymin2, &xmax2, &ymax2);
					if (outlineFound)
					{
						xmin2  = round_accuracy(xmin2, accuracy);
						ymin2  = round_accuracy(ymin2, accuracy);
						xmax2  = round_accuracy(xmax2, accuracy);
						ymax2  = round_accuracy(ymax2, accuracy);

						if (xmin < xmax)
						{
							// the two pins are lined up horizontal
							pp = new CPntList;
							CPnt *pnt = new CPnt;
							pnt->x = (DbUnit)xmin;
							pnt->y = (DbUnit)ymin2;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmax;
							pnt->y = (DbUnit)ymin2;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmax;
							pnt->y = (DbUnit)ymax2;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmin;
							pnt->y = (DbUnit)ymax2;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmin;
							pnt->y = (DbUnit)ymin2;
							pnt->bulge = 0;
							pp->AddTail(pnt);
						}
						else if (ymin < ymax)
						{
							// the two pins are lined up vertical
							pp = new CPntList;
							CPnt *pnt = new CPnt;
							pnt->x = (DbUnit)xmin2;
							pnt->y = (DbUnit)ymin;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmax2;
							pnt->y = (DbUnit)ymin;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmax2;
							pnt->y = (DbUnit)ymax;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmin2;
							pnt->y = (DbUnit)ymax;
							pnt->bulge = 0;
							pp->AddTail(pnt);
							pnt = new CPnt;
							pnt->x = (DbUnit)xmin2;
							pnt->y = (DbUnit)ymin;
							pnt->bulge = 0;
							pp->AddTail(pnt);
						}
					}
				}
               
				if (pp != NULL)
				{
					CString value = outlineAlgTagToString(algPinCenters);
					DataStruct *data = AddPackageOutline(doc, pp, destinationBlk, rectangle, filled);
					if (data)
						doc->SetAttrib(&data->getAttributesRef(), outlineMethodKW, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL);

               FreePntList(pp);
            }
				else
					err = -1;
			}
			else
				err = -1;
      }
      break;

   case algPinExtents: // pad/body extents
      {
         Outline_Start(doc);
         
         int outlineFound = 0;

         outlineFound = DFT_OutlineAddPins(doc, &referenceBlk->getDataList(), 0, 0, 0, 0, 1, 0, -1);

         if (!outlineFound)
            err = -1;

         if (outlineFound)
         { 
            int returnCode;
            CPntList *pntList = Outline_GetOutline(&returnCode, accuracy);

				CString value = outlineAlgTagToString(algPinExtents);
				DataStruct *data = AddPackageOutline(doc, pntList, destinationBlk, rectangle, filled);
				if (data)
					data->setAttrib(doc->getCamCadData(), outlineMethodKW, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL); 
			}
			else
				err = -1;

         Outline_FreeResults();
      }
      break;

   case algInsidePads: // inside pads
      {
         // algorithm:
         // only works on 2 pin components and components w/ pins in 2 rows
         double xmin, ymin, xmax, ymax;
         doc->CalcBlockExtents(referenceBlk);

         xmin  = referenceBlk->getXmin();
         ymin  = referenceBlk->getYmin();
         xmax  = referenceBlk->getXmax();
         ymax  = referenceBlk->getYmax();

         int outlineFound = get_insidepadsextents(doc, &referenceBlk->getDataList(), &xmin, &ymin, &xmax, &ymax);
         if (outlineFound)
         {
            xmin  = round_accuracy(xmin, accuracy);
            ymin  = round_accuracy(ymin, accuracy);
            xmax  = round_accuracy(xmax, accuracy);
            ymax  = round_accuracy(ymax, accuracy);

            // no outline if all pins are in single line.
            if (xmin < xmax && ymin < ymax)
            {
               CPntList *pp = new CPntList;
               CPnt *pnt = new CPnt;
               pnt->x = (DbUnit)xmin;
               pnt->y = (DbUnit)ymin;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmax;
               pnt->y = (DbUnit)ymin;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmax;
               pnt->y = (DbUnit)ymax;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmin;
               pnt->y = (DbUnit)ymax;
               pnt->bulge = 0;
               pp->AddTail(pnt);
               pnt = new CPnt;
               pnt->x = (DbUnit)xmin;
               pnt->y = (DbUnit)ymin;
               pnt->bulge = 0;
               pp->AddTail(pnt);

					CString value = outlineAlgTagToString(algInsidePads);
					DataStruct *data = AddPackageOutline(doc, pp, destinationBlk, rectangle, filled);
					if (data)
						data->setAttrib(doc->getCamCadData(), outlineMethodKW, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL); 
               
					FreePntList(pp);
            }
            else
               err = -1;
         }
         else
            err = -1;
      }
      break;

	case algBodyOutline: // body outline
      {
         Outline_Start(doc);
         
         int outlineFound = 0;

         outlineFound = DFT_OutlineAddData(doc, &referenceBlk->getDataList(), 0, 0, 0, 0, 1, 0, -1, GR_CLASS_COMPOUTLINE, 0);

         if (!outlineFound)
            outlineFound = DFT_OutlineAddData(doc, &referenceBlk->getDataList(), 0, 0, 0, 0, 1, 0, -1, 0, LAYTYPE_COMPONENTOUTLINE);

         if (!outlineFound) // here now any graphic.
            outlineFound = DFT_OutlineAddData(doc, &referenceBlk->getDataList(), 0, 0, 0, 0, 1, 0, -1, 0, 0);

         if (!outlineFound)
            err = -1;
            
         int returnCode;
         CPntList *pntList = Outline_GetOutline(&returnCode, accuracy);
         if (pntList)
			{
				CString value = outlineAlgTagToString(algBodyOutline);
				DataStruct *data = AddPackageOutline(doc, pntList, destinationBlk, rectangle, filled);
				if (data)
					data->setAttrib(doc->getCamCadData(), outlineMethodKW, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL); 
			}

         Outline_FreeResults();
      }
      break;

   case algCustom:
      // Custom outline isn't generated here, it is made interactively by user.
      // But do update the current outline to have the "Custom" algorithm setting.
      {
         DataStruct *d = GetPackageOutline(&destinationBlk->getDataList());
         if (d != NULL)
            d->setAttrib(doc->getCamCadData(), outlineMethodKW, valueTypeString, outlineAlgTagToString(algCustom).GetBuffer(0), attributeUpdateOverwrite, NULL);
      }
      break;

   default:
		err = -2;
		break;
   }

   // Methods above create outline relative to referenceBlk origin.
   // We want the outline to be relative to the pins centroid location.
   // Shift the outline poly.
   // The algCustom method is exempt, its outline is not created here.
   // We are, in fact, doing this because custom outlines are aligned by pins centroid and it
   // has become a nuisance that not all outlines are aligned the same. So now they are.
   if (err == 0 && algorithm != algCustom)
   {
      double centX, centY;
      if (CalculateCentroid_PinCenters(doc, referenceBlk, &centX, &centY))
      {
         CTMatrix m;
         m.translate(-centX, -centY);
         destinationBlk->transform(m);
      }
   }

   return err;
}

/******************************************************************************
* AddPackageOutline
*/
static DataStruct* AddPackageOutline(CCEtoODBDoc *doc, CPntList *pntList, BlockStruct *block, int rectangle, bool fillFlag)
{
   if (!pntList)
      return NULL;

   // graph results for dft outline
   Graph_Block_On(GBO_APPEND, block->getName(), -1, 0);

	int packageOutineTopIndex = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
	int packageOutineBotIndex = Graph_Level(RP_LAYER_PKG_BODY_BOTTOM, "", FALSE);
   Graph_Level_Mirror(RP_LAYER_PKG_BODY_TOP, RP_LAYER_PKG_BODY_BOTTOM, "");	

   int layerNum = packageOutineTopIndex;

   /* OBSOLETE  --  Because new Real Part based outlines are always top built
   if (doc != NULL)
	{
		// Fixed case #1890
		// Only put DFT Outline on mirrored layer for SMD bottom build geometries		
      //if (!block->IsDesignedOnTop(doc->getCamCadData()) && !block->isTechnologyThruHole(doc->getCamCadData()))
			//layerNum = doc->getLayerAt(layerNum)->getMirroredLayerIndex();

      // Fixed case #dts0100405116
      // Only Bottom Build geometry will use the mirrored layer
      if (block->IsDesignedOnBottom(doc->getCamCadData()))
			layerNum = doc->getLayerAt(layerNum)->getMirroredLayerIndex();
	}
   */

	DataStruct *data = Graph_PolyStruct(layerNum, 0, FALSE); // poly is always with 0
   ///data->setGraphicClass(GR_CLASS_PACKAGEOUTLINE);
   data->setGraphicClass(graphicClassPackageBody);

   Graph_Poly(NULL, 0, fillFlag, FALSE, TRUE); // closed

   if (rectangle)
   {
      Mat2x2 m;
      RotMat2(&m, 0.0);

      ExtentRect extents;
      extents.left = extents.bottom = FLT_MAX;
      extents.right = extents.top = -FLT_MAX;
      
      PolyPntExtents(pntList, &extents, 1.0, 0, 0.0, 0.0, &m, 0.0);

      Graph_Vertex(extents.left,extents.bottom, 0);
      Graph_Vertex(extents.right, extents.bottom, 0);
      Graph_Vertex(extents.right, extents.top, 0);
      Graph_Vertex(extents.left, extents.top, 0);
      Graph_Vertex(extents.left, extents.bottom, 0);
   }
   else
   {
      POSITION pos = pntList->GetHeadPosition();
      while (pos)
      {
         CPnt *pnt = pntList->GetNext(pos);
         Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
      }
   }

   Graph_Block_Off();

   return data;
}

/*****************************************************************************/
/*
   algorithm 0 = classic
             1 = inside pins
             2 = cover pins
             3 = from realpart
   rectangle = or 256   

   return 0 for success
   return -1 for geom found but no outline
   return -2 for geom not found
*/
int DFT_CreateGeomDFxOutline(CCEtoODBDoc *doc, BlockStruct *block, int Algorithm, int overwrite)
{
   int res = -2;
   int rectangle = 0;
   OutlineAlgorithmTag algorithm = algBodyOutline;

   DFTOUTL_prep_layers(doc);
   rectangle = Algorithm && 256;
   algorithm = (OutlineAlgorithmTag)(Algorithm & 255);

   // PUNT! the following is wrong, it will make old style DFT package outlines
   return GeneratePackageOutlineGeometry(doc, block, block, algorithm, rectangle, false /*filled*/, overwrite);
}


