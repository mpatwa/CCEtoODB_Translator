// $Header: /CAMCAD/5.0/Centroid.cpp 34    3/12/07 12:38p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "centroid.h"
#include "graph.h"
#include "extents.h"
#include <float.h>
#include ".\centroid.h"


bool GetComponentOutlineExtents(CCEtoODBDoc *doc, BlockStruct *block, bool includePins, bool includeBody, CExtent &compExtents);

static bool isThisProperSOT23Layout(InsertStruct *pin1, InsertStruct *pin2, InsertStruct *pin3, double &outRotationAsFound);
static bool GetSOT23SpecialCaseRotation(BlockStruct *block, double &outRotationOffset);

/****************************************************************************
* OnGenerateComponentCentroid
*/
void CCEtoODBDoc::OnGenerateComponentCentroid() 
{
   generate_CENTROIDS(this);
   UpdateAllViews(NULL);
}

/******************************************************************************
* generate_CENTROIDS
   This function generated centroids for all PCB Components, which have none
*/
void generate_CENTROIDS(CCEtoODBDoc *doc)
{
   doc->StoreDocForImporting();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      DataStruct *data = centroid_exist_in_block(block);

      if (!data)
      {
         double x = 0, y = 0;
         CalculateCentroid_PinCenters(doc, block, &x, &y);

         Attrib* attrib;

         if (block->getAttributesRef() && block->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_CENTROID_X, 0), attrib))
         {
            x = attrib->getDoubleValue();
         }

         if (block->getAttributesRef() && block->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_CENTROID_Y, 0), attrib))
         {
            y = attrib->getDoubleValue();
         }

         BlockStruct *centroid = doc->GetCentroidGeometry();

         Graph_Block_On(GBO_APPEND, block->getName(), block->getFileNumber(), 0);
         data = Graph_Block_Reference(centroid->getName(), NULL, -1, x, y, 0, 0, 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeCentroid);
         Graph_Block_Off();
      }
   }
}

/******************************************************************************
* GenerateComponentCentroid
*/
int GenerateComponentCentroid(CCEtoODBDoc *doc, FileStruct *file, short method)
{
   if (file)
   {
	   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
	   while (dataPos)
	   {
		   DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);
			if (!data || !data->getInsert())
				continue;
		   if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
				continue;

   		BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
			if (!block)
				continue;

			DataStruct *centroidData = centroid_exist_in_block(block);

  			double x, y;
			if (!CalculateCentroid(doc, block, method, &x, &y))
				return -1;

			if (centroidData)
			{
				centroidData->getInsert()->setOriginX(x);
				centroidData->getInsert()->setOriginY(y);
			}
			else
			{
            BlockStruct *centroid = doc->GetCentroidGeometry();

				Graph_Block_On(GBO_APPEND, block->getName(), block->getFileNumber(), 0);
				centroidData = Graph_Block_Reference(centroid->getName(), NULL, -1, x, y, 0, 0, 1.0, -1, TRUE);
				centroidData->getInsert()->setInsertType(insertTypeCentroid);
				Graph_Block_Off();
			}
	   }
   }

	return 0;
}


/******************************************************************************
* centroid_exist_in_block
*/
DataStruct *centroid_exist_in_block(BlockStruct *block)
{
   if (block != NULL)
      return block->GetCentroidData();

   return NULL;
}

//----------------------------------------------------------------------------

void defineStandardCentroid(CCEtoODBDoc *doc, BlockStruct *block)
{
   // Add standard CAMCAD centroid geometry to given block

   if (doc != NULL && block != NULL)
   {
      // Make sure the layers exist
      int botLayerNum = Graph_Level("CENTROID_BOT", "", 0);
      LayerStruct *botLayer = doc->FindLayer(botLayerNum);

  
      if ( botLayer != NULL )
      {
         botLayer->setLayerType(layerTypeCentroidBottom);
         botLayer->setVisible(true);
      }

      int topLayerNum = Graph_Level("CENTROID_TOP", "", 0);
      LayerStruct *topLayer = doc->FindLayer(topLayerNum);

      if ( topLayer != NULL )
      {
         topLayer->setLayerType(layerTypeCentroidTop);
         topLayer->setVisible(true);
      }

      Graph_Level_Mirror("CENTROID_TOP", "CENTROID_BOT", "");

      // Make the graphic
      ////centroidGeom = Graph_Block_On(GBO_OVERWRITE /*GBO_APPEND*/, CENTROIDGEOM, -1, 0);
      BlockStruct *centroidGeom = Graph_Block_On(block);

      if ( centroidGeom != NULL )
      {
         centroidGeom->setBlockType(BLOCKTYPE_CENTROID);
         centroidGeom->setFlagBits(BL_SPECIAL | BL_GLOBAL);
      }

      // O-->  this is the centroid graphic
      double size = doc->getSettings().CentroidSize;
	   double factor = 1;
	   if (doc->getSettings().getPageUnits() != 0)
	   {
         factor = Units_Factor(0, doc->getSettings().getPageUnits());
	   }

      Graph_Circle(topLayerNum, 0, 0, size*factor, 0L, 0, FALSE, TRUE);

      Graph_PolyStruct(topLayerNum, 0, FALSE);
      Graph_Poly(NULL, 0, 0, 0, 1);
      Graph_Vertex(size*factor, 0.5 * size*factor, 0.0);
      Graph_Vertex(size*factor, -0.5 * size*factor, 0.0);
      Graph_Vertex(2.0 * size*factor, 0, 0.0);
      Graph_Vertex(size*factor, 0.5 * size*factor, 0.0);
      
      Graph_Block_Off();
   }
}

//----------------------------------------------------------------------------

void redefineStandardCentroid(CCEtoODBDoc *doc, BlockStruct *block)
{
   // Given a block destined to be $centroid, empty out what ever is
   // in it and fill it with what CAMCAD puts in a standard centroid item.

   if (doc != NULL && block != NULL)
   {

      // Remove anything that is there                 
      for (POSITION pos = block->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(pos);
         RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
      }
      block->getDataList().RemoveAll();

      // Fill block with standard centroid content
      defineStandardCentroid(doc, block);
   }
}

//----------------------------------------------------------------------------

BlockStruct *createStandardCentroid(CCEtoODBDoc *doc)
{
   // Create from scratch

   BlockStruct *centroidGeom = Graph_Block_On(GBO_APPEND, CENTROIDGEOM, -1, 0);
   Graph_Block_Off();

   defineStandardCentroid(doc, centroidGeom);

   return centroidGeom;
}

//----------------------------------------------------------------------------

void validateCentroidVisibility(CCEtoODBDoc *doc)
{
   // If centroid block exists, check to see if it contains a graphical definition.
   // If it does not, inform the user and offer to leave it as-is (maybe they did
   // it on purpose) and offer to regenerate it with standard graphics.
   // If the centroid does not exist, do nothing but return an "okay" value.
   // If a centroid does not exist, presume one will be generated when needed.
   // A call to this function does not imply one is needed, but if one does
   // exist, it will be inspected.

   BlockStruct *centroidGeom = Graph_Block_Exists(doc, CENTROIDGEOM, -1);

   if (centroidGeom != NULL)
   {
#ifdef BE_THOROUGH
      /// When the cheap solution proves inadequate, finish this section
      int dataCount = 0;
      for (POSITION pos = centroidGeom->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct *data = centroidGeom->getDataList().GetNext(pos);
         if (data != NULL)
         {
            dataCount++;
            // Apply some test of data 
         }
      }
#else
      // Cheap, just check if it is empty.
      // This does not guarantee what is there will be something visible.
      // When we get called to the mat on it, implement the section above.

      if (centroidGeom->getDataCount() < 1)
      {
         // It is empty
         CCentroidValidateDlg dlg(doc, centroidGeom);
         dlg.DoModal();
      }
#endif
   }
}

/******************************************************************************
*/
BlockStruct* CCEtoODBDoc::GetCentroidGeometry()
{
   // Get existing centroid geometry or define new one if none exists.
   // Defines a global geometry for all centroid usage.

   BlockStruct *centroidGeom = Graph_Block_Exists(this, CENTROIDGEOM, -1);

   if (!centroidGeom)
   {
      centroidGeom = createStandardCentroid(this);
   }

   return centroidGeom;
}

/******************************************************************************
* CalculateCentroid
*/
bool CalculateCentroid(CCEtoODBDoc *doc, BlockStruct *block, int method, double *x, double *y)
{
   switch (method)
   {
   case Centroid_PinCenters:
      return CalculateCentroid_PinCenters(doc, block, x, y);

   case Centroid_PinExtents:
      return CalculateCentroid_Extents(doc, block, true, false, x, y);

   case Centroid_CompOutline:
      return CalculateCentroid_Extents(doc, block, false, true, x, y);

   case Centroid_PinBodyExtents:
      return CalculateCentroid_Extents(doc, block, true, true, x, y);

   default:
      return false;
   }
}

/******************************************************************************
* CalculateCentroid_PinCenters
*/
bool CalculateCentroid_PinCenters(CCEtoODBDoc *doc, BlockStruct *block, double *x, double *y)
{
   *x = 0.0;
   *y = 0.0;

   double xmin, ymin, xmax, ymax;
   int pinsFound = 0;

   xmin = ymin = FLT_MAX;
   xmax = ymax = -FLT_MAX;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN && data->getInsert()->getInsertType() != insertTypeDiePin)
         continue;

      if (xmin > data->getInsert()->getOriginX())
         xmin = data->getInsert()->getOriginX();
      if (ymin > data->getInsert()->getOriginY())
         ymin = data->getInsert()->getOriginY();
      if (xmax < data->getInsert()->getOriginX())
         xmax = data->getInsert()->getOriginX();
      if (ymax < data->getInsert()->getOriginY())
         ymax = data->getInsert()->getOriginY();
      pinsFound++;
   }

   if (!pinsFound)
      return false;

   *x = (xmax + xmin) / 2;
   *y = (ymax + ymin) / 2;

   return true;
}

/******************************************************************************
* CalculateCentroid_Extents
*/
bool CalculateCentroid_Extents(CCEtoODBDoc *doc, BlockStruct *block, bool includePins, bool includeBody, double *x, double *y)
{
   *x = 0.0;
   *y = 0.0;

	CExtent compOutlineExtent;
	if (!GetComponentOutlineExtents(doc, block, includePins, includeBody, compOutlineExtent))
		return false;

	CPoint2d centroid = compOutlineExtent.getCenter();
	*x = centroid.x;
   *y = centroid.y;

   return true;
}

bool GetComponentOutlineExtents(CCEtoODBDoc *doc, BlockStruct *block, bool includePins, bool includeBody, CExtent &compExtents)
{
   bool extentsFound = false;
	CExtent extent;
   compExtents.reset();

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      switch (data->getDataType())
      {
      case T_POLY:
         if (!includeBody)
            continue;

         if (data->getGraphicClass() != GR_CLASS_COMPOUTLINE)
            continue;

         extent = data->getPolyList()->getExtent(doc->getCamCadData());
         extentsFound = true;

         break;

      case T_INSERT:
         {
            if (!includePins && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               continue;

				BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
				if (!subblock)
					continue;

				if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
				{
               extent = subblock->getExtent(doc->getCamCadData());
					// Case 1453 - the extent above is about the pin in isolation,
					// need to translate that extent to the pin location relative
					// to component origin.
					InsertStruct *pinInsert = data->getInsert();
					double xmin = extent.getXmin() + pinInsert->getOriginX();
					double ymin = extent.getYmin() + pinInsert->getOriginY();
					double xmax = extent.getXmax() + pinInsert->getOriginX();
					double ymax = extent.getYmax() + pinInsert->getOriginY();

					extent.set(xmin, ymin, xmax, ymax);

				}
				else
				{
					//if (!GetComponentOutlineExtents(doc, subblock, includePins, includeBody, extent))
					//	continue;
					if (!GetComponentOutlineExtents(doc, subblock, includePins, includeBody, extent))
						continue;
				}

				extentsFound = true;
         }
         break;

      default:
         continue;
      }

		compExtents.update(extent);
   }

   return extentsFound;
}

bool GenerateCentroidRotationOffset(BlockStruct *inputReferenceBlock, DataStruct *inputCentroid, double &outputRotation)
{
   bool isSingleRow = true; // assume single row, prove otherwise
   int  pinCount = 0;
   CString throwAwayMessage;
   return AnalyzeCentroidRotationOffset(inputReferenceBlock,inputCentroid,outputRotation,isSingleRow, pinCount, throwAwayMessage);
}

bool AnalyzeCentroidRotationOffset(BlockStruct *inputReferenceBlock, DataStruct *inputCentroid, double &outputRotation, bool& isSingleRow, int& pinCount, CString &outMessage)
{
   // Centroid rotation is based on relationship of pin 1 position to centroid position.

   outMessage.Empty();

   // Count pins and find pin 1.
   // And count pin rows.
	DbUnit pin1X = 0.0, pin1Y = 0.0;
   bool pin1Found = false;
   double stdDx = 0.0;
   double stdDy = 0.0;
   double pinAX = 0.0;
   double pinAY = 0.0;
   int matchState = 0; // 0 = not yet set, 1 = match X, 2 = match Y
   
   outputRotation = 0.;
   isSingleRow = true;
   pinCount = 0;

   // Determine number of rows by checking coord deltas.
   // Either all the dx's or all the dy's should be close to zero if single row.
   // Obviously, we are assuming either horizontal or vertical orientation.
   // This won't work for a diagonal single-row. We'll deal with that when somone reports one.

	POSITION pinPos = inputReferenceBlock->getDataList().GetHeadPosition();
	while (pinPos)
	{
      DataStruct* data = inputReferenceBlock->getDataList().GetNext(pinPos);
      if (data != NULL && data->isInsertType(insertTypePin))
      {
         pinCount++;

         // Save location if pin 1.

         // By the way... we tried using PINNR attribute as a backup to using Refname, in case there is no
         // pin with Refname "1". However, we found inconsistent usage of PINNR in data, even among our own
         // importers. PINNR is supposed to start at 1, but some importers are starting at 0. That
         // makes it unreliable, and led to identifying pin with refname "2" as pin 1 by PINNR = 1.
         // In that data, pin refname "1" had PINNR = 0. So that support has been disabled, but code
         // left as reminder. It would work great if the data in the wild was reliable, but it is not
         // and right now we are not going to force that issue. Just drop attempt to use PINNR.
      
         bool isPinnr1 = false;
         bool isRefname1 = false;

         // By PINNR attribute.
         //CAttribute* pinnrAttrib = data->attributes().lookupAttribute(standardAttributePinNumber);
         //if (pinnrAttrib != NULL)
         //{
         //   int pinnr = pinnrAttrib->getIntValue();
         //   isPinnr1 = (pinnr == 1);
         //}

         // By Refname
         isRefname1 = (data->getInsert()->getRefname().CompareNoCase("1") == 0);

         // If either indicate pin 1 then take it. If we already found a
         // different pin 1 then that is trouble.
         if (isPinnr1 || isRefname1)
         {
            if (pin1Found)
            {
               // Then we already found a previous pin 1. Two pin 1's in the data.
               // Put into output message, it is up to caller to present it.
               if (!outMessage.IsEmpty())
                  outMessage += "\n";
               outMessage += "More than one pin 1 found using refname and/or PINNR attribute.";
            }
            pin1Found = true;
            pin1X = data->getInsert()->getOriginX();
            pin1Y = data->getInsert()->getOriginY();
         }

         // Determine number of rows
         if (pinCount == 1) 
         {
            // save first pin xy encountered, does not matter if it is pin 1
            pinAX = data->getInsert()->getOriginX();
            pinAY = data->getInsert()->getOriginY();
         }
         else if (pinCount == 2)
         {
            // set the standard for dx, dy
            stdDx = data->getInsert()->getOriginX() - pinAX;
            stdDy = data->getInsert()->getOriginY() - pinAY;

            // Need to be sure to always match dx or dy, once we find which is the candidate.
            // Can't match dx on some and dy on others, two-row parallel would not be caught correctly.
            if (abs(stdDx) < 0.00001)
               matchState = 1;
            else if (abs(stdDy) < 0.00001)
               matchState = 2;
               
            if (matchState == 0)
               isSingleRow = false;
         }
         else if (isSingleRow) // no need to keep checking once we have determnined is multi-row
         {
            // match current dx,dy to standard
            double dx = data->getInsert()->getOriginX() - pinAX;
            double dy = data->getInsert()->getOriginY() - pinAY;

            if (matchState == 1)
            {
               if (abs(stdDx - dx) > 0.00001)
                  isSingleRow = false;
            }
            else if (matchState == 2)
            {
               if (abs(stdDy - dy) > 0.00001)
                  isSingleRow = false;

            }
         }

      }
	}

   // SOT23 is acting up... special case. For SOT23 there is a specific layout of pins, and we want to find rotation
   // offset based on pin posistions rather than what anybody gave the pins for refname of PINNR.
   if (pinCount == 3)
   {
      double sotRotationOffset = 0.;
      if (GetSOT23SpecialCaseRotation(inputReferenceBlock, sotRotationOffset))
      {
         outputRotation = sotRotationOffset;
         return true;
      }
   }

   // Check for pin1found after processing all pins. Pin 1 is not necessarily
   // going to be the first one in the list up there so can't quit early.
	if (!pin1Found)
		return false;

	// Get centroid position
	DbUnit cenX = inputCentroid->getInsert()->getOriginX();
	DbUnit cenY = inputCentroid->getInsert()->getOriginY();

   // Case dts0100375771
   // This case involved contacting IPC to confirm what should be done with single-row-multi-pin
   // (i.e. more tan 2 pins) parts. Previously the code here made teh distinction between
   // 2-pin parts and all other parts. The real distinction accoring to the IPC info is
   // between single-row parts and all other parts.

   // Case 2153
   // This is all about on-the-line situations.
   // Centroid rotation is based on the quadrant pin 1 is located in.
   // The centroid position is used as an origin. Which quadrant an axis falls into
   // is different for 2-pin parts and all other parts. One can think of it as selecting
   // a leg of an axis, and then determining if this axis belongs to the quadrant on the clockwise
   // side or counterclockwise side of that axis leg.

   // For single-row (was 2-pin) parts, the axis belongs in the quadrant on the clockwise 
   // side, i.e. from this axis leg up to but not including positions on the next axis 
   // leg in the clockwise direction.

   // For multi-row (was multi-pin) parts, and axis belongs to the quadrant on the counter-clockwise side.

   // The rotation offset is the same for a given quadrant, for all parts, regardless of 
   // pin count. The difference is in determining the quadrant.

   // Equating position with compass directions, a West pin on a single-row (was 2-pin) part is in quadrant II,
   // i.e. upper left. A West facing pin on a multi-row (was multi-pin) part is in quadrant III, i.e. lower left.
   // A North pin in a multi-row part is in quadrant II.

   // Using standard conventions for quadrant numbers, i.e.
   //  Quadrant 1 (I)   == Upper Right
   //           2 (II)  == Upper Left
   //           3 (III) == Lower Left
   //           4 (IV)  == Lower Right

   int quadrant = 2;  // Default, results in 0 rotation offset

   if (isSingleRow /*pinCount == 2*/)
   {
      if (pin1X >= cenX && pin1Y > cenY)  // UR, includes North axis
         quadrant = 1;
      else if (pin1X < cenX && pin1Y >= cenY)  // UL, includes West axis
         quadrant = 2; 
      else if (pin1X <= cenX && pin1Y < cenY)  // LL, includes South axis
         quadrant = 3;
      else if (pin1X > cenX && pin1Y <= cenY)  // LR, includes East axis
         quadrant = 4;
   }
   else
   {
      if (pin1X > cenX && pin1Y >= cenY)  // UR, includes East axis
         quadrant = 1;
      else if (pin1X <= cenX && pin1Y > cenY)  // UL, includes North axis
         quadrant = 2; 
      else if (pin1X < cenX && pin1Y <= cenY)  // LL, includes West axis
         quadrant = 3;
      else if (pin1X >= cenX && pin1Y < cenY)  // LR, includes South axis
         quadrant = 4;
   }

   // By SMD rules (apparantly IPC standards) the standard position (rotation offset 0)
   // is with pin in upper left, i.e. quadrant II. 
   // We want a centroid rotation offset such that when the whole part (with centroid)
   // is rotated so that the centroid points to 0 degrees, the pin 1 end up in quadrant II.
   // E.g. if pin 1 is in quadrant 1, then it needs 90 degree CCW rotation to get to quad II.
   // Therefore the centroid rotation should be 270, as it takes 90 degree CCW rotation to
   // get from 270 to 0.
   // In other words, we rotate the centroid in the opposite direction by the amount we
   // want to rotate the part to get pin 1 to quad II. In the example about, we want to end
   // up with 90 rotation, so we rotate -90, which is the same as 270.
   switch (quadrant)
   {
      case 2:
         outputRotation = 0;   // aka   -0
         break;
      case 1:
         outputRotation = 270; // aka  -90
         break;
      case 4:
         outputRotation = 180; // aka -180
         break;
      case 3:
         outputRotation = 90;  // aka -270
         break;
   }

   // Punt
	return true;
}

static bool GetSOT23SpecialCaseRotation(BlockStruct *block, double &outCentroidRotation)
{
   // Handle special case of SOT23.
   // Comp must has 3 pins in layout where two of them have same X or same Y
   // and third is different in both X and Y. And that third pin will have
   // and X or Y in between the value that is the "other one" from the two
   // that match.
   // I.e. if two Y match on 2 pins then X on third is between X on those two.
   // Or if the two Xs match on 2 pins then Y on third will be between Y of the other two.

   outCentroidRotation = 0.;

   int pinCount = 0;
   InsertStruct *pinA = NULL; // Avoid confusion, don't call these pin1 pin2 pin3.
   InsertStruct *pinB = NULL;
   InsertStruct *pinC = NULL;

   POSITION pinPos = block->getDataList().GetHeadPosition();
   while (pinPos)
   {
      DataStruct* data = block->getDataList().GetNext(pinPos);
      if (data != NULL && data->isInsertType(insertTypePin))
      {
         pinCount++;
         if (pinCount == 1)
            pinA = data->getInsert();
         else if (pinCount == 2)
            pinB = data->getInsert();
         else if (pinCount == 3)
            pinC = data->getInsert();
         else // more than 3 pins, we can quite right now.
            return false;
      }
   }

   if (pinCount == 3)
   {
      // Try the pin combos, ake first arrangment that passes for a SOT23.

      double outRotationAsIs = 0.;

      // It does not really matter which pin combo turns out to be the right one.
      // All that gets sent back is the rotation offset. So we don't need to keep track of which pin combo did it.
      if (isThisProperSOT23Layout(pinA, pinB, pinC, outRotationAsIs))
      {
         outCentroidRotation = outRotationAsIs;
         return true;
      }
      if (isThisProperSOT23Layout(pinA, pinC, pinB, outRotationAsIs))
      {
         outCentroidRotation = outRotationAsIs;
         return true;
      }
      if (isThisProperSOT23Layout(pinB, pinA, pinC, outRotationAsIs))
      {
         outCentroidRotation = outRotationAsIs;
         return true;
      }
      if (isThisProperSOT23Layout(pinB, pinC, pinA, outRotationAsIs))
      {
         outCentroidRotation = outRotationAsIs;
         return true;
      }
      if (isThisProperSOT23Layout(pinC, pinA, pinB, outRotationAsIs))
      {
         outCentroidRotation = outRotationAsIs;
         return true;
      }
      if (isThisProperSOT23Layout(pinC, pinB, pinA, outRotationAsIs))
      {
         outCentroidRotation = outRotationAsIs;
         return true;
      }
      
   }

   // Failed some test someplace, not a SOT23.
   return false;
}

static bool isThisProperSOT23Layout(InsertStruct *pin1, InsertStruct *pin2, InsertStruct *pin3, double &outRotationAsFound)
{
   // outRotationAsFound is set if this pin arrangement looks like an SOT23 at some rotation.
   // It is the rotation of this SOT23 as we see it.

   // Proper layout, any rotation allowed, the relative positions are what matters.
   //
   //  Case 1             Case 2             Case 3           Case 4
   //   1               2       1                2              3
   //       3   or          3         or     3         or   1        2
   //   2                                        1
   //
   //    0 angle          -90                 -180              -270
   //       aka           270                  180                90

   outRotationAsFound = 0.;

   // If pin1 is pin 1 then it will have same Xs or same Ys as pin2.
   bool abSameX = fabs(pin1->getOrigin2d().x - pin2->getOrigin2d().x) < SMALLNUMBER;
   bool abSameY = fabs(pin1->getOrigin2d().y - pin2->getOrigin2d().y) < SMALLNUMBER;

   bool bcSameX = fabs(pin2->getOrigin2d().x - pin3->getOrigin2d().x) < SMALLNUMBER;
   bool bcSameY = fabs(pin2->getOrigin2d().y - pin3->getOrigin2d().y) < SMALLNUMBER;

   bool caSameX = fabs(pin3->getOrigin2d().x - pin1->getOrigin2d().x) < SMALLNUMBER;
   bool caSameY = fabs(pin3->getOrigin2d().y - pin1->getOrigin2d().y) < SMALLNUMBER;

   // 3rd point should have different XY than both pin 1 and pin 2.
   // This is true regardless of orientation, so can be checked up front.
   if (bcSameX || bcSameY || caSameX || caSameY)
      return false;

   if (abSameX)
   {
      // Possibly case 1 or case 3   (0 or +/-180 rotation correction)

      // Both XYs the same, coincident point, not an SOT23.
      if (abSameY)
         return false; // both XYs are coincident

      // 3rd pnt Y should lie between other two Y's.
      double lowY = min(pin1->getOrigin2d().y, pin2->getOrigin2d().y);
      double hiY  = max(pin1->getOrigin2d().y, pin2->getOrigin2d().y);
      double pin3y = pin3->getOrigin2d().y;
      if (pin3y < lowY || pin3y > hiY)
         return false;

      if (pin1->getOrigin2d().y > pin2->getOrigin2d().y)
      {
         // Pin1 is above pin2.                     1
         // Pin3 to left is not valid.         3?  or  3?
         // Pin3 to right is orient  0.             2
         if (pin3->getOrigin2d().x < pin1->getOrigin2d().x)
            return false;
         else
            outRotationAsFound = 0.;   // Case 1
      }
      else
      {
         // Pin1 is below pin2.                  2
         // Pin3 to left is orient 180.      3?  or  3?
         // Pin3 to right is invalid.            1
         if (pin3->getOrigin2d().x < pin1->getOrigin2d().x)
            outRotationAsFound = 180.;   // Case 3
         else
            return false;
      }

      // If we did not return false just above then we are okay.
      return true;
   }
   else if (abSameY)
   {
      // Possibly case 2 or case 4    (-90 or -270 correction, aka 270 and 90)

      // Couldn't get here if abSameX is true, so don't need to check again.

      // 3rd pnt X should lie between other two X's.
      double lowX = min(pin1->getOrigin2d().x, pin2->getOrigin2d().x);
      double hiX  = max(pin1->getOrigin2d().x, pin2->getOrigin2d().x);
      double pin3x = pin3->getOrigin2d().x;
      if (pin3x < lowX || pin3x > hiX)
         return false;

      if (pin1->getOrigin2d().x > pin2->getOrigin2d().x)
      {
         // Pin1 is to right of pin2.           3?
         // Pin3 above is not valid.         2  or  1
         // Pin3 below is 270                   3?
         if (pin3->getOrigin2d().y > pin1->getOrigin2d().y)
            return false;
         else
            outRotationAsFound = 270.;   // Case 2
      }
      else
      {
         // Pin1 is to left of pin2.         3?
         // Pin3 above is orient 90.      1  or  2
         // Pin3 below is invalid .          3?
         if (pin3->getOrigin2d().y > pin1->getOrigin2d().y)
            outRotationAsFound = 90.;   // Case 4
         else
            return false;
      }

      // If we did not return false just above then we are okay.
      return true;

   }

   // Didn't fall into a SOT23 accepted case.
   return false;
}


// CCentroidValidateDlg dialog

IMPLEMENT_DYNAMIC(CCentroidValidateDlg, CDialog)
CCentroidValidateDlg::CCentroidValidateDlg(CCEtoODBDoc *doc, BlockStruct *block, CWnd* pParent /*=NULL*/)
	: CDialog(CCentroidValidateDlg::IDD, pParent)
   , m_block(block)
   , m_doc(doc)
   , m_dispositionRadio(0)
   , m_messageTxt(_T(""))
{
}

CCentroidValidateDlg::~CCentroidValidateDlg()
{
}

void CCentroidValidateDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_DISPOSITION, m_dispositionRadio);
   DDX_Text(pDX, IDC_MESSAGE, m_messageTxt);
}

BOOL CCentroidValidateDlg::OnInitDialog()
{
   CString msg;
   msg  = "The currently existing centroid geometry item does not contain visible graphics.";
   msg += " Such a centroid is legitimate and can have location and rotation, and will be correctly processed by utilities using centroids (e.g. Exporters).";
   msg += " \n\nHowever, the centroid will not be visible on the CAMCAD screen, and so can not be visually reviewed.";

   m_messageTxt = msg;

   m_dispositionRadio = 0;

   UpdateData(FALSE);

   return TRUE;
}

BEGIN_MESSAGE_MAP(CCentroidValidateDlg, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCentroidValidateDlg message handlers

void CCentroidValidateDlg::OnBnClickedOk()
{
   OnOK();

   UpdateData(TRUE);

   if (this->m_dispositionRadio == 1 && m_doc != NULL && m_block != NULL)
   {
      // Regenerate visible centroid graphic
      redefineStandardCentroid(m_doc, m_block);
   }
}
