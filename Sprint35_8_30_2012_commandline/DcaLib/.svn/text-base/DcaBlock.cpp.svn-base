// $Header: /CAMCAD/DcaLib/DcaBlock.cpp 12    6/30/07 1:46a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "DcaBlock.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"
#include "DcaBasesVector.h"
#include "DcaDataType.h"
#include "DcaAttributeValues.h"
#include "DcaModeStack.h"
#include "DcaGraphicClass.h"
#include "DcaUnits.h"
#include "DcaDataListIterator.h"
#include "DcaLayerType.h"
#include "DcaWriteFormat.h"
#include "DcaTMatrix.h"
#include "DcaPoly.h"
#include "DcaInsert.h"
#include "DcaLib.h"

//_____________________________________________________________________________
CString DesignSurfaceValueTagToString(DesignSurfaceValueTag designSurfaceValueTag)
{
   CString retval;

   switch (designSurfaceValueTag)
   {
   case designSurfaceNone:                         retval = "NONE";                             break;   
   case designSurfaceBothSurface:                  retval = "BOTH_SURFACE";                     break;   
   case designSurfacePlacedSurface:                retval = "PLACED_SURFACE";                   break;   
   case designSurfaceTopOnlySurface:               retval = "TOP_ONLY_SURFACE";                 break;   
   case designSurfaceBottomOnlySurface:            retval = "BOTTOM_ONLY_SURFACE";              break;   
   case designSurfaceOppositeSurface:              retval = "OPPOSITE_SURFACE";                 break;   
   case designSurfacePlacedTopPlacedSurface:       retval = "PLACED_TOP_PLACED_SURFACE";        break;   
   case designSurfacePlacedTopOppositeSurface:     retval = "PLACED_TOP_OPPOSITE_SURFACE";      break;   
   case designSurfacePlacedBottomPlacedSurface:    retval = "PLACED_BOTTOM_PLACED_SURFACE";     break;   
   case designSurfacePlacedBottomOppositeSurface:  retval = "PLACED_BOTTOM_OPPOSITE_SURFACE";   break;   
   case designSurfaceBuriedSurface:                retval = "BURIED_SURFACE";                   break;   
   default:                                        retval = "Undefined";                        break;
   }

   return retval;
}

DesignSurfaceValueTag DesignSurfaceValueStringToTag(CString designSurfaceValueString)
{
   if (DesignSurfaceValueTagToString(designSurfaceNone).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfaceNone;

   if (DesignSurfaceValueTagToString(designSurfaceBothSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfaceBothSurface;

   if (DesignSurfaceValueTagToString(designSurfacePlacedSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfacePlacedSurface;

   if (DesignSurfaceValueTagToString(designSurfaceTopOnlySurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfaceTopOnlySurface;

   if (DesignSurfaceValueTagToString(designSurfaceBottomOnlySurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfaceBottomOnlySurface;

   if (DesignSurfaceValueTagToString(designSurfaceOppositeSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfaceOppositeSurface;

   if (DesignSurfaceValueTagToString(designSurfacePlacedTopPlacedSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfacePlacedTopPlacedSurface;

   if (DesignSurfaceValueTagToString(designSurfacePlacedTopOppositeSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfacePlacedTopOppositeSurface;

   if (DesignSurfaceValueTagToString(designSurfacePlacedBottomPlacedSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfacePlacedBottomPlacedSurface;

   if (DesignSurfaceValueTagToString(designSurfacePlacedBottomOppositeSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfacePlacedBottomOppositeSurface;

   if (DesignSurfaceValueTagToString(designSurfaceBuriedSurface).CompareNoCase(designSurfaceValueString) == 0)
      return designSurfaceBuriedSurface;

   return designSurfaceUndefine;
}

//_____________________________________________________________________________
CString outlineIndicatorOrientationTagToString(OutlineIndicatorOrientationTag tagValue)
{
   const char* retval;

   switch (tagValue)
   {
   case outlineIndicatorOrientationRight:         retval = "Right";        break;  
   case outlineIndicatorOrientationTop:           retval = "Top";          break;  
   case outlineIndicatorOrientationLeft:          retval = "Left";         break;  
   case outlineIndicatorOrientationBottom:        retval = "Bottom";       break;  
   case outlineIndicatorOrientationTopRight:      retval = "TopRight";     break;  
   case outlineIndicatorOrientationTopLeft:       retval = "TopLeft";      break;  
   case outlineIndicatorOrientationBottomRight:   retval = "BottomRight";  break;  
   case outlineIndicatorOrientationBottomLeft:    retval = "BottomLeft";   break; 
   default:                                       retval = "Undefined";    break;
   }

   return retval;
}

OutlineIndicatorOrientationTag stringToOutlineIndicatorOrientationTag(const CString& tagValue)
{
   OutlineIndicatorOrientationTag retval = outlineIndicatorOrientationUndefined;

   if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationRight).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationRight;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationTop).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationTop;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationLeft).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationLeft;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationBottom).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationBottom;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationTopRight).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationTopRight;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationTopLeft).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationTopLeft;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationBottomRight).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationBottomRight;
   }
   else if (outlineIndicatorOrientationTagToString(outlineIndicatorOrientationBottomLeft).CompareNoCase(tagValue) == 0)
   {
      retval = outlineIndicatorOrientationBottomLeft;
   }

   return retval;
}

//_____________________________________________________________________________
void getDesignSurfaceFromLayer(LayerStruct& layer, DesignSurfaceValueTag& topDesignSurfaceValueTag, DesignSurfaceValueTag& botDesignSurfaceValueTag);

void getDesignSurfaceValueTag(DesignSurfaceValueTag newTopDesignSurfaceValueTag, DesignSurfaceValueTag newBotDesignSurfaceValueTag,
                              DesignSurfaceValueTag& topDesignSurfaceValueTag, DesignSurfaceValueTag& botDesignSurfaceValueTag);

//_____________________________________________________________________________
BlockStruct::BlockStruct()
: m_camCadData(NULL)
, m_blockNumber(0)
, m_fileNumber(0)
, m_blockType(blockTypeUnknown)
, m_dataList(true)
{
   init();
}

BlockStruct::BlockStruct(CCamCadData& camCadData,int blockNumber)
: m_camCadData(&camCadData)
, m_blockNumber(blockNumber)
, m_fileNumber(0)
, m_blockType(blockTypeUnknown)
, m_dataList(true)
{
   init();
}

BlockStruct::BlockStruct(CCamCadData& camCadData,int blockNumber,const CString& name,int fileNumber,BlockTypeTag blockType)
: m_camCadData(&camCadData)
, m_blockNumber(blockNumber)
, m_fileNumber(fileNumber)
, m_blockType(blockType)
, m_dataList(true)
, m_name(name)
{
   init();
}

void BlockStruct::init()
{
   m_attributes = NULL;

   m_libraryBlockNumber =
   m_marked = 0;

   setFlags(0);
   m_extent.reset();
   m_shape = apertureUndefined;
   m_dCode = 0;

   m_sizeA = 
   m_sizeB = 
   m_sizeC = 
   m_sizeD =
   m_xOffset = 
   m_yOffset =
   m_rotationRadians = 0.;

   m_spokeCount = 4;

   m_tCode =
   m_toolBlockNumber =
   m_toolType = 0;
   m_toolSize = 0.0;
   
   m_toolDisplay =
   m_toolHolePlated  =
   m_toolHolePunched  =
   m_package = 
   m_checkedFlag = false;
}

BlockStruct::~BlockStruct()
{
   removeBlock();

   delete m_attributes;
}

void BlockStruct::updateName(const CString& newName)
{
   if (m_camCadData != NULL)
   {
      m_name = newName;
   }
}

void BlockStruct::updateFileNumber(int newFileNumber)
{
   if (m_camCadData != NULL)
   {
      m_fileNumber = newFileNumber;
   }
}

void BlockStruct::updateBlockType(BlockTypeTag newBlockType)
{
   if (m_camCadData != NULL)
   {
      m_blockType = newBlockType;
   }
}

double BlockStruct::getRotationDegrees() const
{
   return radiansToDegrees(m_rotationRadians);
}

void BlockStruct::setRotationDegrees(double degrees)
{
   m_rotationRadians = degreesToRadians(degrees);
}

void BlockStruct::removeBlock()
{
   if (m_camCadData != NULL)
   {
      m_camCadData->getBlockDirectory().remove(this);
      m_camCadData->getBlockArray().removeBlock(this);
   }
}

void BlockStruct::setProperties(const BlockStruct& other,bool copyAttributesFlag)
{
   if (&other != this)
   {
      m_libraryBlockNumber = other.m_libraryBlockNumber;
      setName(other.m_name);
      m_originalName       = other.m_originalName;
      setFileNumber(other.m_fileNumber);
      m_marked             = other.m_marked;
      setFlags(other.getFlags());
      setBlockType(other.m_blockType);
      m_comment            = other.m_comment;
      m_package            = other.m_package;
      m_extent             = other.m_extent;
      m_shape              = other.m_shape;
      m_sizeA              = other.m_sizeA;
      m_sizeB              = other.m_sizeB;
      m_xOffset            = other.m_xOffset;
      m_yOffset            = other.m_yOffset;
      m_rotationRadians    = other.m_rotationRadians;
      m_dCode              = other.m_dCode;
      m_sizeC              = other.m_sizeC;
      m_sizeD              = other.m_sizeD;
      m_spokeCount         = other.m_spokeCount;
      m_tCode              = other.m_tCode;
      m_toolSize           = other.m_toolSize;
      m_toolDisplay        = other.m_toolDisplay;
      m_toolBlockNumber    = other.m_toolBlockNumber;
      m_toolType           = other.m_toolType;
      m_toolHolePlated     = other.m_toolHolePlated;
      m_toolHolePunched    = other.m_toolHolePunched;

      if (m_camCadData != NULL)
      {
         m_dataList.setEqual(other.m_dataList,*m_camCadData);
      }

      delete m_attributes;
      m_attributes     = NULL;

      if (other.m_attributes != NULL && copyAttributesFlag)
      {
         m_attributes = new CAttributes();
         CAttributeMapWrap attributeMap(m_attributes);
         attributeMap.copyFrom(other.m_attributes);
      }

      // don't copy m_checkedFlag
   }
}

void BlockStruct::setName(const CString& blockName)
{
   if (m_camCadData != NULL)
   {
      m_camCadData->getBlockDirectory().updateName(*this,blockName);
   }
   else
   {
      m_name = blockName;
   }
}

void BlockStruct::setOriginalName(const CString& blockName)
{
   m_originalName = blockName;
}

DbFlag BlockStruct::getFlags() const
{
   return m_flags; 
}

void BlockStruct::setFlags(DbFlag blockFlag)
{
   m_flags = blockFlag; 
}

void BlockStruct::setFlagBits(DbFlag mask)
{
   setFlags(getFlags() | mask); 
}

void BlockStruct::clearFlagBits(DbFlag mask)
{
   setFlags(getFlags() & ~mask); 
}

short BlockStruct::getFileNumber() const
{
   return m_fileNumber;
}

void BlockStruct::setFileNumber(short fileNumber)
{
   if (m_camCadData != NULL)
   {
      m_camCadData->getBlockDirectory().updateFileNumber(*this,fileNumber);
   }
   else
   {
      m_fileNumber = fileNumber;
   }
}

void BlockStruct::setBlockType(BlockTypeTag blockType)
{
   if (m_camCadData != NULL)
   {
      m_camCadData->getBlockDirectory().updateBlockType(*this,blockType);
   }
   else
   {
      m_blockType = blockType;
   }
}

ApertureShapeTag BlockStruct::getShape() const 
{ 
   return m_shape; 
}

void BlockStruct::setShape(ApertureShapeTag shape)
{ 
   if (shape == apertureUnknown || shape == apertureUndefined)
   {
      m_shape = apertureUndefined;

      setFlags(getFlags() & ~(BL_APERTURE | BL_BLOCK_APERTURE));
   }
   else
   {
      m_shape = shape; 

      if ((getFlags() & (BL_APERTURE | BL_BLOCK_APERTURE)) == 0)
      {
         setFlags(getFlags() | BL_APERTURE);
      }
   }
}

void BlockStruct::setShape(short shape)
{ 
   setShape(intToApertureShape(shape));
}

CAttributes*& BlockStruct::getDefinedAttributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return m_attributes;
}

CAttributes& BlockStruct::attributes() const
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

bool BlockStruct::setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr)
{
   return camCadData.setAttribute(attributes(),keywordIndex,valueType,value,updateMethod,attribPtr);
}

bool BlockStruct::lookUpAttrib(WORD keyword, CAttribute*& attribute)
{
   bool retval = false;

   if (m_attributes != NULL)
   {
      retval = (m_attributes->Lookup(keyword, attribute) != 0);
   }

   return retval;
}

CPoint2d BlockStruct::getPinCentroid() const
{
   return m_dataList.getPinCentroid();
}

CExtent BlockStruct::getPinExtent() const
{
   return m_dataList.getPinExtent();
}

CBasesVector BlockStruct::getPinMajorMinorAxes() const
{
   return m_dataList.getPinMajorMinorAxes();
}

void BlockStruct::swapBlockNumber(BlockStruct& other)
{
   int tempBlockNumber = m_blockNumber;
   m_blockNumber       = other.m_blockNumber;
   other.m_blockNumber = tempBlockNumber;
}

bool BlockStruct::isTechnologyThruHole(CCamCadData& camCadData) const
{
	bool retval = false;

   if (m_attributes != NULL)
   {
		CString technology;

		if (camCadData.getAttributeStringValue(technology,attributes(),standardAttributeTechnology))
		{
			if (technology.CompareNoCase("thru") == 0)
			{
				retval = true;
			}
		}
	}

	return retval;
}

bool BlockStruct::isTechnologySmd(CCamCadData& camCadData) const
{
	bool retval = false;

   if (m_attributes != NULL)
   {
		CString technology;

		if (camCadData.getAttributeStringValue(technology,attributes(),standardAttributeTechnology))
		{
			if (technology.CompareNoCase("smd") == 0)
			{
				retval = true;
			}
		}
	}

	return retval;
}

bool BlockStruct::isAperture() const
{
   bool retval = ((getFlags() & (BL_APERTURE | BL_BLOCK_APERTURE)) != 0);

   return retval;
}

bool BlockStruct::isSimpleAperture() const
{
   bool retval = false;

   if (isAperture())
   {
      switch (m_shape)
      {
      case apertureRound:
      case apertureSquare:
      case apertureRectangle:
      case apertureTarget:
      case apertureThermal:
      case apertureDonut:
      case apertureOctagon:
      case apertureOblong:
      case apertureBlank:
         retval = true;
         break;
      }
   }

   return retval;
}

bool BlockStruct::isComplexAperture() const
{
   bool retval = (isAperture() && (m_shape == apertureComplex));

   return retval;
}

bool BlockStruct::isTool() const
{
   bool retval = ((getFlags() & (BL_TOOL | BL_BLOCK_TOOL)) != 0);

   return retval;
}

bool BlockStruct::isDrillHole() const
{
   return (m_blockType == blockTypeDrillHole);
}

CBasesVector BlockStruct::getApertureBasesVector() const
{
   CBasesVector apertureBasesVector;
   CTMatrix matrix = getApertureTMatrix();
   //matrix.invert();
   apertureBasesVector.transform(matrix);

   return apertureBasesVector;
}

CTMatrix BlockStruct::getApertureTMatrix() const
{
   CTMatrix matrix;
   matrix.rotateRadiansCtm(m_rotationRadians);
   matrix.translateCtm(m_xOffset,m_yOffset);

   return matrix;
}

POSITION BlockStruct::getHeadDataPosition() const
{
   return m_dataList.GetHeadPosition();
}

DataStruct* BlockStruct::getNextData(POSITION& pos) const
{
   return m_dataList.GetNext(pos);
}

POSITION BlockStruct::getHeadDataInsertPosition() const
{
   POSITION pos;

   for (pos = m_dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_dataList.GetAt(pos);

      if (data->getDataType() == dataTypeInsert)
      {
         break;
      }

      m_dataList.GetNext(pos);
   }

   return pos;
}

DataStruct* BlockStruct::getNextDataInsert(POSITION& pos) const
{
   DataStruct* data = m_dataList.GetNext(pos);

   while (pos != NULL)
   {
      DataStruct* nextdata = m_dataList.GetAt(pos);

      if (nextdata->getDataType() == dataTypeInsert)
      {
         break;
      }

      m_dataList.GetNext(pos);
   }

   return data;
}

DataStruct* BlockStruct::getAtData(POSITION pos) const
{
   return m_dataList.GetAt(pos);
}

int BlockStruct::getDataCount() const
{
   return (int) m_dataList.GetCount();
}

CExtent BlockStruct::getExtent(CCamCadData& camCadData)
{
   if (!m_extent.isValid())
   {
      //camCadDoc.CalcBlockExtents(this);
      // knv - todo
      //camCadData.calculateBlockExtents(*this);
      calculateVisibleBlockExtents(camCadData);
   }

   return m_extent;
}

void BlockStruct::resetExtent()
{
   m_extent.reset();
}

void BlockStruct::setExtent(double x0,double y0,double x1,double y1)
{
   m_extent.set(x0,y0,x1,y1);
}

void BlockStruct::setExtent(const CExtent& extent)
{
   m_extent = extent;
}

void BlockStruct::updateExtent(double x,double y)
{
   m_extent.update(x,y);
}

void BlockStruct::calculateVisibleBlockExtents(CCamCadData& camCadData)
{
   calculateBlockExtents(camCadData,true);
}

void BlockStruct::calculateBlockExtents(CCamCadData& camCadData,bool checkOnlyVisibleEntitiesFlag,CMessageFilter* messageFilter)
{
   CWaitCursor wait;
   int insertlayer = -1;

   //Mat2x2 m;
   //RotMat2(&m, 0.0);

   CExtent extent;

	bool debugFlag = false;

	if (debugFlag && messageFilter != NULL)
		messageFilter->formatMessage("BlockName = %s\n", this->getName());

   if (getFlags() & BL_APERTURE)
   {
      if (getShape() == apertureComplex)
      {
         //BlockStruct* subblock = camCadDoc.Find_Block_by_Num((int)(getSizeA()));
         BlockStruct* subblock = camCadData.getBlock(getComplexApertureSubBlockNumber());

         if (subblock != NULL)
         {
				if (debugFlag && messageFilter != NULL)
					messageFilter->formatMessage("SubBlockName = %s\n", subblock->getName());

				//extent = camCadData.blockExtents(&subblock->getDataList(), 0.0, 0.0, 0.0, 0, 1.0, insertlayer,checkOnlyVisibleEntitiesFlag, messageFilter);
            // knv - todo
				extent = camCadData.blockExtents(subblock->getDataList(), 0.0, 0.0, 0.0, 0, 1.0, insertlayer,checkOnlyVisibleEntitiesFlag, messageFilter);

            if (extent.isValid())
            {
               setExtent(extent);
            }
         }
         else
         {
            resetExtent();
         }
      }
      else
      {
         CPolyList* polylist = convertApertureToPoly();

         if (polylist != NULL)
         {
            //extent = PolyExtents(&camCadDoc, polylist, 1.0, 0, 0.0, 0.0, &m, TRUE);
            // knv - todo
            extent = polylist->getExtent(camCadData);

            setExtent(extent);

            delete polylist;
         }
      }
   }
   else
   {
      //extent = camCadDoc.blockExtents(&(getDataList()), 0.0, 0.0, 0.0, 0, 1.0, insertlayer,checkOnlyVisibleEntitiesFlag, messageFilter);
      extent = camCadData.blockExtents(getDataList(), 0.0, 0.0, 0.0, 0, 1.0, insertlayer,checkOnlyVisibleEntitiesFlag, messageFilter);
      setExtent(extent);
   }

	if (debugFlag && messageFilter != NULL)
	{
		messageFilter->formatMessage("BlockName = %s\txMin = %0.3f\txMax = %0.3f\tyMin = %0.3f\tyMax = %0.3f\n",
				this->getName(), extent.getXmin(), extent.getXmax(), extent.getYmin(), extent.getYmax());
		messageFilter->formatMessage("=============================================================================\n");
	}
}

DataStruct* BlockStruct::GetPackage() const
{
   if (m_blockType != blockTypeRealPart)
      return NULL;
   
   for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_dataList.GetNext(pos);

      if (data->getDataType() != dataTypeInsert) continue;
      
      if (data->getInsert()->getInsertType() == insertTypePackage)
         return data;
   }

   return NULL;
}

DataStruct* BlockStruct::GetCentroidData() const
{
   if (m_blockType != blockTypePcbComponent)
      return NULL;
   
   for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_dataList.GetNext(pos);

      if (data->isInsertType(insertTypeCentroid))
         return data;
   }

   return NULL;
}

DataStruct* BlockStruct::getPin(const CString& pinName) const
{
   DataStruct* padStack = NULL;

   if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert)
         { 
            InsertStruct* insert = data->getInsert();

            if (insert->getInsertType() == insertTypePin)
            {
               if (insert->hasRefnameData() && STRCMPI(insert->getRefname(),pinName) == 0)
               {
                  padStack = data;
                  break;
               }
            }
         }
      }
   }

   return padStack;
}

DataStruct* BlockStruct::getPin() const
{
   // Allegedly "get any pin", but in reality get pin with lowest pin number

   DataStruct* padStack = NULL;

   // It does not really matter if this block is type PcbComponent, it
   // only matters that the inserts are type Pin.
   //
   // Get the pin with the lowest pin number that inserts given padstack.

   //if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->isInsertType(insertTypePin))
         { 
            InsertStruct* insert = data->getInsert();

            if (insert != NULL)
            {
               //BlockStruct* insertedBlk = doc.Find_Block_by_Num(insert->getBlockNumber());

               //if (insertedBlk != NULL && insertedBlk->getName().CompareNoCase(padstackGeomName) == 0)
               {
                  // Found one, keep if first one or pin num is lower than current one
                  if (padStack == NULL)
                  {
                     padStack = data;
                  }
                  else
                  {
                     CString curName(padStack->getInsert()->getSortableRefDes());
                     CString newName(data->getInsert()->getSortableRefDes());

                     if (newName < curName)
                        padStack = data;
                  }
               }
            }
         }
      }
   }

   return padStack;
}

DataStruct* BlockStruct::getPinUsingPadstack(CCamCadData &ccdata, const CString& padstackGeomName) const
{
   DataStruct* padStack = NULL;

   // It does not really matter if this block is type PcbComponent, it
   // only matters that the inserts are type Pin.
   //
   // Get the pin with the lowest pin number that inserts given padstack.

   //if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->isInsertType(insertTypePin))
         { 
            InsertStruct* insert = data->getInsert();

            if (insert != NULL)
            {
               BlockStruct* insertedBlk = ccdata.getBlockAt(insert->getBlockNumber());

               if (insertedBlk != NULL && insertedBlk->getName().CompareNoCase(padstackGeomName) == 0)
               {
                  // Found one, keep if first one or pin num is lower than current one
                  if (padStack == NULL)
                  {
                     padStack = data;
                  }
                  else
                  {
                     CString curName(padStack->getInsert()->getSortableRefDes());
                     CString newName(data->getInsert()->getSortableRefDes());

                     if (newName < curName)
                        padStack = data;
                  }
               }
            }
         }
      }
   }

   return padStack;
}

DataStruct* BlockStruct::getPinUsingPadstack(const int n) const
{
   DataStruct* padStack = NULL;

   // It does not really matter if this block is type PcbComponent, it
   // only matters that the inserts are type Pin.
   //
   // Get the pin with the lowest pin number that inserts the nth padstack.

   CMapWordToPtr blockNumMap;
   int selectedPadstackBlockNum = -1;

   //if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->isInsertType(insertTypePin))
         { 
            InsertStruct* insert = data->getInsert();

            if (insert != NULL)
            {
               blockNumMap.SetAt(insert->getBlockNumber(), NULL);
               if (blockNumMap.GetCount() == n)
                  selectedPadstackBlockNum = insert->getBlockNumber();

               if (insert->getBlockNumber() == selectedPadstackBlockNum)
               {
                  // Found one, keep if first one or pin num is lower than current one
                  if (padStack == NULL)
                  {
                     padStack = data;
                  }
                  else
                  {
                     CString curName(padStack->getInsert()->getSortableRefDes());
                     CString newName(data->getInsert()->getSortableRefDes());

                     if (newName < curName)
                        padStack = data;
                  }
               }
            }
         }
      }
   }

   return padStack;
}

DataStruct* BlockStruct::FindInsertData(CString refname, InsertTypeTag insertType)
{
   return this->getDataList().FindInsertData(refname, insertType);
}

int BlockStruct::getPadstackCount() const
{
   // Return number of different kinds of padstacks used by pin inserts.
   // Same padstack means same block, so only need to look at inserted block numbers.
   // Just put all the block nums in a map and then return the map size.

   CMapWordToPtr blockNumMap;

   // block type doesn't really matter, all that matters
   // are pin inserts, regardless of what this block thinks it is
   //if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->isInsertType(insertTypePin))
         { 
            int blockNum = data->getInsert()->getBlockNumber();
            blockNumMap.SetAt(blockNum, NULL);
         }
      }
   }

   return blockNumMap.GetCount();
}

int BlockStruct::getPinCount() const
{
   int pinCount = 0;

   if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert)
         { 
            InsertStruct* insert = data->getInsert();

            if (insert->getInsertType() == insertTypePin)
            {
               pinCount++;
            }
         }
      }
   }

   return pinCount;
}

int BlockStruct::getNoConnectPinCount() const
{
   int noConnectPinCount = 0;

   if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert)
         { 
            InsertStruct* insert = data->getInsert();

            if (insert->getInsertType() == insertTypePin)
            {
               CString pinName = insert->getRefname();

               if (pinName.CompareNoCase(ATT_VALUE_NO_CONNECT) == 0)
               {
                  noConnectPinCount++;
               }
            }
         }
      }
   }

   return noConnectPinCount;
}

bool BlockStruct::hasNamedPin() const
{
   // Named pin means pin refname contains a non-numeric char.
   // Return true if there is at least one such pin present.

   if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert)
         { 
            InsertStruct* insert = data->getInsert();

            if (insert->getInsertType() == insertTypePin)
            {
               CString refname = insert->getRefname();

               for (int i = 0; i < refname.GetLength(); i++)
               {
                  char c = refname.GetAt(i);

                  if (!isdigit(c))
                     return true;
               }
               
            }
         }
      }
   }

   return false;
}

bool BlockStruct::hasNumberedPin() const
{
   // Numbered pin means pin refname contains all numeric chars.
   // Return true if there is at least one such pin present.

   if (m_blockType == blockTypePcbComponent)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert)
         { 
            InsertStruct* insert = data->getInsert();

            if (insert->getInsertType() == insertTypePin)
            {
               CString refname = insert->getRefname();

               bool allNumeric = true;

               for (int i = 0; i < refname.GetLength() && allNumeric; i++)
               {
                  char c = refname.GetAt(i);

                  if (!isdigit(c))
                     allNumeric = false;
               }

               if (allNumeric)
                  return true;
            }
         }
      }
   }

   return false;
}

int BlockStruct::getDiePinCount() const
{
   int diepinCount = 0;

   if (m_blockType == blockTypeDie)
   {      
      for (POSITION pos = m_dataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert)
         { 
            InsertStruct* insert = data->getInsert();

            if (insert->getInsertType() == insertTypeDiePin)
            {
               diepinCount++;
            }
         }
      }
   }

   return diepinCount;
}

// If the block is not an aperture, then NULL is returned.
// If the block is a complex aperture, then the polys in the subblock are combined into
// a single poly list and returned, no flattening of inserts is performed.
// Simple apertures are converted to a polyList;
CPolyList* BlockStruct::getAperturePolys(CCamCadData& camCadData, bool convertToOutline) 
{
   CPolyList* polyList = NULL;

   if (isAperture())
   {
      if (m_shape == apertureComplex)
      {
         int blockNum = getComplexApertureSubBlockNumber();
         BlockStruct* subBlock = camCadData.getBlock(blockNum);
         polyList = new CPolyList();

         for (POSITION dataPos = subBlock->getDataList().GetHeadPosition(); dataPos != NULL; )
         {
            DataStruct* data = subBlock->getDataList().GetNext(dataPos);

            if (data->getDataType() == dataTypePoly)
            {
               for (POSITION polyPos = data->getPolyList()->GetHeadPosition(); polyPos != NULL; )
               {
                  CPoly* poly = data->getPolyList()->GetNext(polyPos);
                  CPoly* newPoly = new CPoly(*poly);

                  // For dts0100479012
                  // The as-is poly is going to give the centerline for polys that have non-zero width.
                  // Sometimes that is exactly what you want (if what you want is to get the original data).
                  // But sometimes what we want is more akin to the final shape, i.e. the outline of the
                  // shape that will result from drawing the poly with width > 0, and this outline should
                  // itself be drawn with zero width. That is what this will make.
                  if (convertToOutline)
                  {
                     double width = camCadData.getWidth( poly->getWidthIndex() );
                     newPoly->convertToOutline(width, 0.0, camCadData.getZeroWidthIndex());
                  }

                  polyList->AddTail(newPoly);
               }
            }
            else if (data->getDataType() == dataTypeInsert)
            {
               // dts0100393141 - One of the issues in the case was that some complex apertures
               // had more than one level on insert, i.e. the complex aperture inserted a block
               // that had no polys but was itself a complex aperture that inserted yet another 
               // block. This will support an unlimited number of levels of such, but only
               // if all levels are complex apertures. No data at present to test a "deep"
               // complex aperture that starts inserting regular blocks.

               int insBlkNum = data->getInsert()->getBlockNumber();
               BlockStruct *insBlk = camCadData.getBlockAt(insBlkNum);
               if (insBlk != NULL)
               {
                  if (insBlk->isAperture())
                  {
                     CPolyList *list2 = insBlk->getAperturePolys(camCadData);
                     CTMatrix mat = data->getInsert()->getTMatrix();
                     list2->transform(mat);
                     polyList->takeData(*list2);
                  }
                  else
                  {
                     // Insert of a "regular" block here is not currently supported. At implementation
                     // time there was no test data for this case, and we were specifically catering
                     // to what the Zuken reader creates, so it didn't seem to matter. If you are
                     // looking here, maybe it matters now.
                  }
               }
            }
         }   

         // compensate for offset pads
         CTMatrix matrix;
         matrix.rotateRadiansCtm(getRotationRadians());
         matrix.translateCtm(getXoffset(),getYoffset());

         polyList->transform(matrix);
      }
      else
      {
         //polyList = ApertureToPoly_Base(&camCadDoc,this, 0., 0., 0., false);
         // knv - todo
         polyList = convertApertureToPoly();
      }
   }

   return polyList;
}

CPolyList* BlockStruct::convertApertureToPoly()
{
   CPolyList* aperturePolyList = NULL;
   BlockStruct& block = *this;

   if (block.isAperture())
   {
      CPoly* poly;

      switch (block.getShape())
      {
      case apertureUndefined:
      case apertureComplex:
      case apertureBlank:
      default:
         break;
      case apertureSquare:
      case apertureRectangle:
         {
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double sizeA = block.getSizeA() / 2.;
            double sizeB = ((block.getShape() == apertureRectangle) ? block.getSizeB() / 2. : sizeA);

            pntList.AddTail(new CPnt( sizeA, sizeB));
            pntList.AddTail(new CPnt( sizeA,-sizeB));
            pntList.AddTail(new CPnt(-sizeA,-sizeB));
            pntList.AddTail(new CPnt(-sizeA, sizeB));
            pntList.AddTail(new CPnt( sizeA, sizeB));
         }

         break;
      case apertureRound:
         {
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double radius = block.getSizeA() / 2.;

            pntList.AddTail(new CPnt( radius,0.,1.));
            pntList.AddTail(new CPnt(-radius,0.,1.));
            pntList.AddTail(new CPnt( radius,0.,1.));
         }

         break;
      case apertureDonut:
         if (CUseVoidsInDonuts::getUseVoidsInDonuts())
         {
            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double radius = block.getSizeA() / 2.;

            pntList.AddTail(new CPnt( radius,0.,1.));
            pntList.AddTail(new CPnt(-radius,0.,1.));
            pntList.AddTail(new CPnt( radius,0.,1.));

            // Donut Hole
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setVoid(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList->AddTail(poly);

            CPntList& donutPntList = poly->getPntList();

            radius = block.getSizeB() / 2.;

            donutPntList.AddTail(new CPnt( radius,0.,1.));
            donutPntList.AddTail(new CPnt(-radius,0.,1.));
            donutPntList.AddTail(new CPnt( radius,0.,1.));
         }
         else // thick circle
         {
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setClosed(true);
            poly->setFilled(false);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double radius = (block.getSizeA() + block.getSizeB()) / 4.;

            pntList.AddTail(new CPnt( radius,0.,1.));
            pntList.AddTail(new CPnt(-radius,0.,1.));
            pntList.AddTail(new CPnt( radius,0.,1.));
         }

         break;
      case apertureOctagon:
         {
            poly = new CPoly;
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double radius   = block.getSizeA()/2.;
            double halfSide = block.getSizeA()/4.83; // size of half of one side of octagon

            pntList.AddTail(new CPnt(-radius  , halfSide));
            pntList.AddTail(new CPnt(-halfSide, radius  ));
            pntList.AddTail(new CPnt( halfSide, radius  ));
            pntList.AddTail(new CPnt( radius  , halfSide));
            pntList.AddTail(new CPnt( radius  ,-halfSide));
            pntList.AddTail(new CPnt( halfSide,-radius  ));
            pntList.AddTail(new CPnt(-halfSide,-radius  ));
            pntList.AddTail(new CPnt(-radius  ,-halfSide));
            pntList.AddTail(new CPnt(-radius  , halfSide));
         }

         break;
      case apertureTarget:
         {
            // circle
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(false);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double radius = block.getSizeA() / 2.;

            pntList.AddTail(new CPnt( radius,0.,1.));
            pntList.AddTail(new CPnt(-radius,0.,1.));
            pntList.AddTail(new CPnt( radius,0.,1.));
         
            // horizontal line
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(false);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList->AddTail(poly);

            radius = block.getSizeA();  // This is actually DIAMETER now, not radius. Just reusing the double var.

            poly->getPntList().AddTail(new CPnt(-radius,0.));
            poly->getPntList().AddTail(new CPnt( radius,0.));
         
            // vertical line
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(false);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList->AddTail(poly);

            poly->getPntList().AddTail(new CPnt(0.,-radius));
            poly->getPntList().AddTail(new CPnt(0., radius));
         }

         break;
      case apertureOblong: // make sure it is counter clock wise.
         {
            poly = new CPoly();
            poly->setWidthIndex(-1);
            poly->setFilled(true);
            poly->setClosed(true);
            poly->setHatchLine(false);
            poly->setHidden(false);
            poly->setVoid(false);
            poly->setThermalLine(false);
            poly->setFloodBoundary(false);

            aperturePolyList = new CPolyList();
            aperturePolyList->AddTail(poly);

            CPntList& pntList = poly->getPntList();

            double halfWidth   = block.getSizeA() / 2.;
            double halfHeight  = block.getSizeB() / 2.;

            if (halfWidth >= halfHeight)
            {
               pntList.AddTail(new CPnt( halfWidth - halfHeight,-halfHeight, 1.));
               pntList.AddTail(new CPnt( halfWidth - halfHeight, halfHeight    ));
               pntList.AddTail(new CPnt(-halfWidth + halfHeight, halfHeight, 1.));
               pntList.AddTail(new CPnt(-halfWidth + halfHeight,-halfHeight    ));
               pntList.AddTail(new CPnt( halfWidth - halfHeight,-halfHeight    ));
            }
            else
            {
               pntList.AddTail(new CPnt( halfWidth, halfHeight - halfWidth, 1.));
               pntList.AddTail(new CPnt(-halfWidth, halfHeight - halfWidth    ));
               pntList.AddTail(new CPnt(-halfWidth,-halfHeight + halfWidth, 1.));
               pntList.AddTail(new CPnt( halfWidth,-halfHeight + halfWidth    ));
               pntList.AddTail(new CPnt( halfWidth, halfHeight - halfWidth    ));
            }
         } 

         break;
      case apertureThermal:
         {
            // 60° arcs, therfore 30° between them
            const double bulge = 0.268; // bulge = tan(da/4) ; da = 60°
            double sinAngle = 0.5; 
            double cosAngle = 0.866;

            double halfSizeA = block.getSizeA() / 2.;
            double halfSizeB = block.getSizeB() / 2.;

            aperturePolyList = new CPolyList();
            CTMatrix matrix;
            if(halfSizeB > halfSizeA)
            {
               //Swap SizeA with SizeB
               halfSizeA = halfSizeB + halfSizeA;
               halfSizeB = halfSizeA - halfSizeB;
               halfSizeA = halfSizeA - halfSizeB;

               //Set the block's sizeA and sizeB
               block.setSizeA(halfSizeA*2);
               block.setSizeB(halfSizeB*2);
            }

            for (int quadrantIndex = 0;quadrantIndex < 4;quadrantIndex++)
            {
               poly = new CPoly();
               poly->setWidthIndex(-1);
               poly->setFilled(true);
               poly->setClosed(true);
               poly->setHatchLine(false);
               poly->setHidden(false);
               poly->setVoid(false);
               poly->setThermalLine(false);
               poly->setFloodBoundary(false);

               aperturePolyList->AddTail(poly);

               CPntList& pntList = poly->getPntList();

               pntList.AddTail(new CPnt( halfSizeB * sinAngle, halfSizeB * cosAngle));
               pntList.AddTail(new CPnt( halfSizeA * sinAngle, halfSizeA * cosAngle, bulge));
               pntList.AddTail(new CPnt(-halfSizeA * sinAngle, halfSizeA * cosAngle));
               pntList.AddTail(new CPnt(-halfSizeB * sinAngle, halfSizeB * cosAngle,-bulge));
               pntList.AddTail(new CPnt( halfSizeB * sinAngle, halfSizeB * cosAngle));

               matrix.rotateDegrees(90);
               poly->transform(matrix);
            }

            break;
         }
      }
   }

   if (aperturePolyList != NULL)
   {
      CTMatrix matrix = block.getApertureTMatrix();
      aperturePolyList->transform(matrix);
   }

   return aperturePolyList;
}

OutlineIndicatorOrientationTag BlockStruct::getComponentOutlineIndicatorOrientation(CCamCadData& camCadData)
{
   OutlineIndicatorOrientationTag retval = outlineIndicatorOrientationUndefined;

   CString stringValue;

   if (camCadData.getAttributeStringValue(stringValue,attributes(),standardAttributeComponentOutlineOrientation))
   {
      retval = stringToOutlineIndicatorOrientationTag(stringValue);
   }

   return retval;
}

void BlockStruct::setComponentOutlineIndicatorOrientation(CCamCadData& camCadData,OutlineIndicatorOrientationTag outlineIndicatorOrientation)
{
   CString stringValue = outlineIndicatorOrientationTagToString(outlineIndicatorOrientation);

   camCadData.setAttribute(attributes(),standardAttributeComponentOutlineOrientation,stringValue);
}

bool BlockStruct::generateDefaultComponentOutline(CCamCadData& camCadData)
{
   // Need to remove previous component outline first before calculating extent
   camCadData.removeDataFromDataListByGraphicClass(*this,graphicClassComponentOutline);

   CExtent componentBodyExtent;

   calculateBlockExtents(camCadData);
   CExtent geometryExtent = getExtent();
   componentBodyExtent.update(geometryExtent);

   int assemblyLayerIndex = camCadData.getLayerIndex(ccLayerAssemblyTop);   

   DataStruct* polyStruct = camCadData.addPolyStruct(*this,assemblyLayerIndex,graphicClassComponentOutline);

   int widthIndex = camCadData.getDefinedWidthIndex(camCadData.convertToPageUnits(pageUnitsInches,.003));

   CPoly* poly = camCadData.addClosedPoly(*polyStruct,widthIndex);

   componentBodyExtent.expand(camCadData.convertToPageUnits(pageUnitsInches,.016));

   // component body
   camCadData.addVertex(*poly,componentBodyExtent.getXmin(),componentBodyExtent.getYmin());
   camCadData.addVertex(*poly,componentBodyExtent.getXmin(),componentBodyExtent.getYmax());
   camCadData.addVertex(*poly,componentBodyExtent.getXmax(),componentBodyExtent.getYmax());
   camCadData.addVertex(*poly,componentBodyExtent.getXmax(),componentBodyExtent.getYmin());
   camCadData.addVertex(*poly,componentBodyExtent.getXmin(),componentBodyExtent.getYmin());

   OutlineIndicatorOrientationTag indicatorOrientation = getComponentOutlineIndicatorOrientation(camCadData);

   // orientation indicator
   if (indicatorOrientation != outlineIndicatorOrientationUndefined)
   {
      double indicatorWidth,indicatorHeight;
      const double cornerIndicatorExtensionFactor = 1.5;

      switch (indicatorOrientation)
      {
      case outlineIndicatorOrientationTop:   
      case outlineIndicatorOrientationBottom:
         indicatorWidth  = componentBodyExtent.getXsize()/4.;
         indicatorHeight = min(componentBodyExtent.getYsize()/4.,indicatorWidth);
         break;
      case outlineIndicatorOrientationRight: 
      case outlineIndicatorOrientationLeft:  
         indicatorHeight = componentBodyExtent.getYsize()/4.;
         indicatorWidth  = min(componentBodyExtent.getXsize()/4.,indicatorHeight);
         break;
      case outlineIndicatorOrientationTopRight: 
      case outlineIndicatorOrientationTopLeft:  
      case outlineIndicatorOrientationBottomRight: 
      case outlineIndicatorOrientationBottomLeft:  
         indicatorWidth  = min(componentBodyExtent.getXsize(),componentBodyExtent.getYsize())/8.;
         indicatorHeight = indicatorWidth;
         break;
      }

      poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

      CPoint2d base1,base2,apex;

      switch (indicatorOrientation)
      {
      case outlineIndicatorOrientationRight: 
         base1.x = componentBodyExtent.getXmax();
         base1.y = componentBodyExtent.getCenter().y - indicatorHeight/2.;
         base2.x = componentBodyExtent.getXmax();
         base2.y = componentBodyExtent.getCenter().y + indicatorHeight/2.;
         apex.x  = componentBodyExtent.getXmax() - indicatorWidth;
         apex.y  = componentBodyExtent.getCenter().y;

         break;
      case outlineIndicatorOrientationLeft:  
         base1.x = componentBodyExtent.getXmin();
         base1.y = componentBodyExtent.getCenter().y + indicatorHeight/2.;
         base2.x = componentBodyExtent.getXmin();
         base2.y = componentBodyExtent.getCenter().y - indicatorHeight/2.;
         apex.x  = componentBodyExtent.getXmin() + indicatorWidth;
         apex.y  = componentBodyExtent.getCenter().y;

         break;
      case outlineIndicatorOrientationTop:   
         base1.x = componentBodyExtent.getCenter().x + indicatorWidth/2.;
         base1.y = componentBodyExtent.getYmax();
         base2.x = componentBodyExtent.getCenter().x - indicatorWidth/2.;
         base2.y = componentBodyExtent.getYmax();
         apex.x  = componentBodyExtent.getCenter().x;
         apex.y  = componentBodyExtent.getYmax() - indicatorHeight;

         break;
      case outlineIndicatorOrientationBottom:
         base1.x = componentBodyExtent.getCenter().x - indicatorWidth/2.;
         base1.y = componentBodyExtent.getYmin();
         base2.x = componentBodyExtent.getCenter().x + indicatorWidth/2.;
         base2.y = componentBodyExtent.getYmin();
         apex.x  = componentBodyExtent.getCenter().x;
         apex.y  = componentBodyExtent.getYmin() + indicatorHeight;

         break;
      case outlineIndicatorOrientationTopLeft:
         base1.x = componentBodyExtent.getXmin();
         base1.y = componentBodyExtent.getYmax() - indicatorHeight;
         base2.x = componentBodyExtent.getXmin() + indicatorWidth;
         base2.y = componentBodyExtent.getYmax();
         //apex.x  = componentBodyExtent.getXmin() + indicatorWidth*cornerIndicatorExtensionFactor;
         //apex.y  = componentBodyExtent.getYmax() - indicatorHeight*cornerIndicatorExtensionFactor;
         apex.x  = (base1.x + base2.x)/2.;
         apex.y  = (base1.y + base2.y)/2.;

         break;
      case outlineIndicatorOrientationBottomLeft:
         base1.x = componentBodyExtent.getXmin();
         base1.y = componentBodyExtent.getYmin() + indicatorHeight;
         base2.x = componentBodyExtent.getXmin() + indicatorWidth;
         base2.y = componentBodyExtent.getYmin();
         //apex.x  = componentBodyExtent.getXmin() + indicatorWidth*cornerIndicatorExtensionFactor;
         //apex.y  = componentBodyExtent.getYmin() + indicatorHeight*cornerIndicatorExtensionFactor;
         apex.x  = (base1.x + base2.x)/2.;
         apex.y  = (base1.y + base2.y)/2.;

         break;
      case outlineIndicatorOrientationBottomRight:
         base1.x = componentBodyExtent.getXmax();
         base1.y = componentBodyExtent.getYmin() + indicatorHeight;
         base2.x = componentBodyExtent.getXmax() - indicatorWidth;
         base2.y = componentBodyExtent.getYmin();
         //apex.x  = componentBodyExtent.getXmax() - indicatorWidth*cornerIndicatorExtensionFactor;
         //apex.y  = componentBodyExtent.getYmin() + indicatorHeight*cornerIndicatorExtensionFactor;
         apex.x  = (base1.x + base2.x)/2.;
         apex.y  = (base1.y + base2.y)/2.;

         break;
      case outlineIndicatorOrientationTopRight:
         base1.x = componentBodyExtent.getXmax();
         base1.y = componentBodyExtent.getYmax() - indicatorHeight;
         base2.x = componentBodyExtent.getXmax() - indicatorWidth;
         base2.y = componentBodyExtent.getYmax();
         //apex.x  = componentBodyExtent.getXmax() - indicatorWidth*cornerIndicatorExtensionFactor;
         //apex.y  = componentBodyExtent.getYmax() - indicatorHeight*cornerIndicatorExtensionFactor;
         apex.x  = (base1.x + base2.x)/2.;
         apex.y  = (base1.y + base2.y)/2.;

         break;
      }

      camCadData.addVertex(*poly,base1.x,base1.y);
      camCadData.addVertex(*poly, apex.x, apex.y );
      camCadData.addVertex(*poly,base2.x,base2.y);
   }

   return true;
}

bool BlockStruct::verifyExtents() const 
{
   bool retval = true;

   if (!(_finite(getXmin()) && _finite(getYmin()) && _finite(getXmax()) && _finite(getYmax())))
   {
      retval = false;
   }

   return retval;
}

//void BlockStruct::RemoveDataFromList(CCEtoODBDoc *doc, DataStruct* data, POSITION dataPos)
//{
//   m_dataList.RemoveDataFromList(doc, data, dataPos);
//}
//
//void BlockStruct::RemoveDataByGraphicClass(CCEtoODBDoc *doc, GraphicClassTag graphicClass)
//{
//   m_dataList.RemoveDataByGraphicClass(doc, graphicClass);
//}
//
//void BlockStruct::RemoveDataByInsertType(CCEtoODBDoc *doc, InsertTypeTag insertType)
//{
//   m_dataList.RemoveDataByInsertType(doc, insertType);
//}

void BlockStruct::transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData, bool doAttributes)
{
   m_dataList.transform(transformationMatrix, camCadData, doAttributes);
}

bool BlockStruct::isValid() const
{
   bool retval = true;

   if (! m_dataList.isValid())
   {
      retval = false;
   }

   return retval;
}

DataStruct* BlockStruct::FindData(const CString& refdes)
{
   for (POSITION pos = this->getHeadDataInsertPosition();pos != NULL;)
   {
      DataStruct* data = this->getNextDataInsert(pos);
      InsertStruct *insert = data->getInsert();

      if (insert->getRefname().Compare(refdes) == 0)
         return data;
   }

   return NULL;      
}

DataStruct *BlockStruct::FindData(const CString& refdes, InsertTypeTag insertType)
{
   for (POSITION pos = this->getHeadDataInsertPosition();pos != NULL;)
   {
      DataStruct* data = this->getNextDataInsert(pos);
      InsertStruct *insert = data->getInsert();

      if ( insert->getInsertType() == insertType && insert->getRefname().Compare(refdes) == 0)
         return data;
   }

   return NULL; 
}


DesignSurfaceValueTag BlockStruct::getDesignSurface(DesignSurfaceValueTag topDesignSurfaceValueTag, DesignSurfaceValueTag botDesignSurfaceValueTag)
{
/* 
   TOP_PADSTACK_DESIGN_SURFACE								   BOT_PADSTACK_DESIGN_SURFACE				      What is it?		
	==================================================================================================================
   //------------------------------------------------------------------------------------------------------------------   
   // These four design surface are special because if one apply to TOP_PADSTACK_DESIGN_SURFACE, 
   // it msut also apply to BOT_PADSTACK_DESIGN_SURFACE.
   //------------------------------------------------------------------------------------------------------------------
   designSurfaceBothSurface                              designSurfaceBothSurface                     Top Build
   designSurfacePlacedSurface                            designSurfacePlacedSurface                   Top Build
   designSurfaceOppositeSurface                          designSurfaceOppositeSurface                 Bottom Build
   designSurfaceBuriedSurface                            designSurfaceBuriedSurface                   Top Build 

   //------------------------------------------------------------------------------------------------------------------
   // The following design surface can be apply individually to TOP_PADSTACK_DESIGN_SURFACE or BOT_PADSTACK_DESIGN_SURFACE;
   // theefore, they can create different combination.
   //------------------------------------------------------------------------------------------------------------------
   designSurfacePlacedTopPlacedSurface                   designSurfacePlacedBottomPlacedSurface       Top Build
   designSurfacePlacedTopPlacedSurface                   designSurfacePlacedBottomOppositeSurface     Top Only Build
   designSurfacePlacedTopPlacedSurface                   designSurfaceNone                            Top Only Build

   designSurfacePlacedTopOppositeSurface                 designSurfacePlacedBottomPlacedSurface       Bottom Only Build
   designSurfacePlacedTopOppositeSurface                 designSurfacePlacedBottomOppositeSurface     Bottom Build
   designSurfacePlacedTopOppositeSurface                 designSurfaceNone                            Bottom Only Build

   designSurfaceNone                                     designSurfacePlacedBottomPlacedSurface       Bottom Only Build
   designSurfaceNone                                     designSurfacePlacedBottomOppositeSurface     Top Only Build
   designSurfaceNone                                     designSurfaceNone                            Undefined
*/

   DesignSurfaceValueTag designSurfaceValueTag = designSurfaceUndefine;

   if (topDesignSurfaceValueTag == designSurfaceBothSurface ||
         topDesignSurfaceValueTag == designSurfacePlacedSurface ||
         topDesignSurfaceValueTag == designSurfaceBuriedSurface ||
      (topDesignSurfaceValueTag == designSurfacePlacedTopPlacedSurface && botDesignSurfaceValueTag == designSurfacePlacedBottomPlacedSurface))
   {
      // Top Build 
      designSurfaceValueTag = designSurfacePlacedSurface;
   }
   else if (topDesignSurfaceValueTag == designSurfaceOppositeSurface ||
            (topDesignSurfaceValueTag == designSurfacePlacedTopOppositeSurface && botDesignSurfaceValueTag == designSurfacePlacedBottomOppositeSurface))
   {
      // Bottom Build
      designSurfaceValueTag = designSurfaceOppositeSurface;
   }
   else if ((topDesignSurfaceValueTag == designSurfacePlacedTopPlacedSurface && botDesignSurfaceValueTag == designSurfacePlacedBottomOppositeSurface) ||
            (topDesignSurfaceValueTag == designSurfacePlacedTopPlacedSurface && botDesignSurfaceValueTag == designSurfaceNone) ||
            (topDesignSurfaceValueTag == designSurfaceNone && botDesignSurfaceValueTag == designSurfacePlacedBottomOppositeSurface))
   {
      // Top Only Build
      designSurfaceValueTag = designSurfaceTopOnlySurface;
   }
   else if ((topDesignSurfaceValueTag == designSurfacePlacedTopOppositeSurface && botDesignSurfaceValueTag == designSurfacePlacedBottomPlacedSurface) ||
            (topDesignSurfaceValueTag == designSurfacePlacedTopOppositeSurface && botDesignSurfaceValueTag == designSurfaceNone) ||
            (topDesignSurfaceValueTag == designSurfaceNone && botDesignSurfaceValueTag == designSurfacePlacedBottomPlacedSurface))
   {
      // Bottom Only Build
      designSurfaceValueTag = designSurfaceBottomOnlySurface;
   }

   return designSurfaceValueTag;
}

DesignSurfaceValueTag BlockStruct::getDesignSurface(CCamCadData& camCadData)
{
   DesignSurfaceValueTag designSurfaceValueTag = designSurfaceUndefine;

   if (this->m_blockType == blockTypePadstack)
   {
      WORD topDesignSurfaceKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
      WORD botDesignSurfaceKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);
      CAttribute *topAttrib = NULL;
      CAttribute *botAttrib = NULL;

      lookUpAttrib(topDesignSurfaceKW, topAttrib);
      lookUpAttrib(botDesignSurfaceKW, botAttrib);

      DesignSurfaceValueTag topDesignSurfaceValueTag = DesignSurfaceValueStringToTag(camCadData.getAttributeStringValue(*topAttrib));
      DesignSurfaceValueTag botDesignSurfaceValueTag = DesignSurfaceValueStringToTag(camCadData.getAttributeStringValue(*botAttrib));

      designSurfaceValueTag = getDesignSurface(topDesignSurfaceValueTag, botDesignSurfaceValueTag);
   }
   else
   {
      WORD topDesignSurfaceKW = camCadData.getAttributeKeywordIndex(standardAttributeDesignedSurface);
      CAttribute *attrib = NULL;
      if (lookUpAttrib(topDesignSurfaceKW, attrib))
      {
         designSurfaceValueTag = DesignSurfaceValueStringToTag(camCadData.getAttributeStringValue(*attrib));
      }      
   }

   return designSurfaceValueTag;
}

bool BlockStruct::IsDesignedOnTop(CCamCadData& camCadData)
{
   // default on top unless specified by the DESIGNED_SURFACE or ATT_TOP_DESIGNED_SURFACE or ATT_BOT_DESIGNED_SURFACE attribute
   DesignSurfaceValueTag designSurfaceValueTag = getDesignSurface(camCadData);
   if (designSurfaceValueTag == designSurfacePlacedSurface || designSurfaceValueTag == designSurfaceUndefine)
      return true;
   else
      return false;
}

bool BlockStruct::IsDesignedOnBottom(CCamCadData& camCadData)
{
   DesignSurfaceValueTag designSurfaceValueTag = getDesignSurface(camCadData);
   if (designSurfaceValueTag == designSurfaceOppositeSurface)
      return true;
   else
      return false;
}

bool BlockStruct::IsDesignedTopOnly(CCamCadData& camCadData)
{
   DesignSurfaceValueTag designSurfaceValueTag = getDesignSurface(camCadData);
   if (designSurfaceValueTag == designSurfaceTopOnlySurface)
      return true;
   else

      return false;
}

bool BlockStruct::IsDesignedBottomOnly(CCamCadData& camCadData)
{
   DesignSurfaceValueTag designSurfaceValueTag = getDesignSurface(camCadData);
   if (designSurfaceValueTag == designSurfaceBottomOnlySurface)
      return true;
   else
      return false;
}

void BlockStruct::GenerateDesignSurfaceAttribute(CCamCadData& camCadData, bool regenerate)
{
   if (m_blockType == blockTypePadstack)
   {
      GeneratePadstackDesignSurfaceAttributes(camCadData, regenerate);
   }
   else
   {
      GenerateGeometryDesignSurfaceAttributes(camCadData, regenerate);
   }
}

void BlockStruct::GenerateGeometryDesignSurfaceAttributes(CCamCadData& camCadData, bool regenerate)
{
   if (m_blockType == blockTypePadstack)
      return;

   WORD geometryDesignSurfaceKW = camCadData.getAttributeKeywordIndex(standardAttributeDesignedSurface);
   CAttribute* attrib = NULL;
   if (lookUpAttrib(geometryDesignSurfaceKW, attrib) && !regenerate)
      return;

   DesignSurfaceValueTag topDesignSurfaceValueTag = designSurfaceNone;
   DesignSurfaceValueTag botDesignSurfaceValueTag = designSurfaceNone;

   for (CDataListIterator dataListIterator(m_dataList, dataTypeInsert); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();

      if (data == NULL)
         continue;

      InsertStruct* insert = data->getInsert();

      if (insert == NULL)
         continue;

      //BlockStruct* block = doc.getBlockAt(insert->getBlockNumber());
      BlockStruct* block = camCadData.getBlock(insert->getBlockNumber());

      if (block == NULL)
         continue;

      if (block->isTool() || block->isAperture())
      {
         //LayerStruct* layer = doc.getLayerAt(data->getLayerIndex());
         LayerStruct* layer = camCadData.getLayer(data->getLayerIndex());

         if (layer == NULL)
            continue;

         getDesignSurfaceFromLayer(*layer, topDesignSurfaceValueTag, botDesignSurfaceValueTag); 
      }
      else if (insert->getInsertType() == insertTypePin ||
         insert->getInsertType() == insertTypeMechanicalPin) // mechanical pin added for case dts0100441046
      {
         block->GenerateDesignSurfaceAttribute(camCadData, regenerate);

         DesignSurfaceValueTag insertTopDesignSurfaceValueTag = designSurfaceNone;
         DesignSurfaceValueTag insertBotDesignSurfaceValueTag = designSurfaceNone;

         if (!insert->getLayerMirrored())
         {
            //WORD topDesKW = doc.RegisterKeyWord(ATT_TOP_DESIGNED_SURFACE, 0, VT_STRING);
            WORD topDesKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
            CAttribute* attrib = NULL;

            if (!block->lookUpAttrib(topDesKW, attrib) || attrib == NULL)
               continue;

            //CString designSurfaceValueString = get_attvalue_string(&doc, attrib);
            CString designSurfaceValueString  = camCadData.getAttributeStringValue(*attrib);
            insertTopDesignSurfaceValueTag = DesignSurfaceValueStringToTag(designSurfaceValueString);


            //WORD botDesKW = doc.RegisterKeyWord(ATT_BOT_DESIGNED_SURFACE, 0, VT_STRING);
            WORD botDesKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);
            attrib = NULL;

            if (!block->lookUpAttrib(botDesKW, attrib) || attrib == NULL)
               continue;

            //designSurfaceValueString = get_attvalue_string(&doc, attrib);
            designSurfaceValueString  = camCadData.getAttributeStringValue(*attrib);
            insertBotDesignSurfaceValueTag = DesignSurfaceValueStringToTag(designSurfaceValueString);
         }
         else
         {
            //WORD topDesKW = doc.RegisterKeyWord(ATT_TOP_DESIGNED_SURFACE, 0, VT_STRING);
            WORD topDesKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
            CAttribute* attrib = NULL;

            if (!block->lookUpAttrib(topDesKW, attrib) || attrib == NULL)
               continue;

            //CString designSurfaceValueString = get_attvalue_string(&doc, attrib);
            CString designSurfaceValueString  = camCadData.getAttributeStringValue(*attrib);
            insertTopDesignSurfaceValueTag = DesignSurfaceValueStringToTag(designSurfaceValueString);

            if (insertBotDesignSurfaceValueTag == designSurfaceOppositeSurface)
               insertBotDesignSurfaceValueTag = designSurfacePlacedSurface;
            else if (insertBotDesignSurfaceValueTag = designSurfacePlacedSurface)
               insertBotDesignSurfaceValueTag = designSurfaceOppositeSurface;


            //WORD botDesKW = doc.RegisterKeyWord(ATT_BOT_DESIGNED_SURFACE, 0, VT_STRING);
            WORD botDesKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);
            attrib = NULL;

            if (!block->lookUpAttrib(botDesKW, attrib) || attrib == NULL)
               continue;

            //designSurfaceValueString = get_attvalue_string(&doc, attrib);
            designSurfaceValueString  = camCadData.getAttributeStringValue(*attrib);
            insertBotDesignSurfaceValueTag = DesignSurfaceValueStringToTag(designSurfaceValueString);

            if (insertBotDesignSurfaceValueTag == designSurfaceOppositeSurface)
            {
               insertBotDesignSurfaceValueTag = designSurfacePlacedSurface;
            }
            else if (insertBotDesignSurfaceValueTag = designSurfacePlacedSurface)
            {
               insertBotDesignSurfaceValueTag = designSurfaceOppositeSurface;
            }
         }

         getDesignSurfaceValueTag(insertTopDesignSurfaceValueTag, insertBotDesignSurfaceValueTag, topDesignSurfaceValueTag, botDesignSurfaceValueTag);
      }
   }

   if (topDesignSurfaceValueTag == designSurfaceBothSurface || botDesignSurfaceValueTag == designSurfaceBothSurface)
   {
      topDesignSurfaceValueTag = designSurfaceBothSurface;
      botDesignSurfaceValueTag = designSurfaceBothSurface;
   }


   // Check for things on the surface layers.
   // Originally this just checked for silkscreen layers.
   // Case dts0100501551 made it clear it should check all surface layers.
   if (topDesignSurfaceValueTag <= designSurfaceNone && botDesignSurfaceValueTag <= designSurfaceNone)
   {
      topDesignSurfaceValueTag = designSurfaceNone;
      botDesignSurfaceValueTag = designSurfaceNone;

      for (CDataListIterator dataListIterator(m_dataList); dataListIterator.hasNext();)
      {
         DataStruct* data = dataListIterator.getNext();

         if (data != NULL)
         {
            LayerStruct *layer = camCadData.getLayer(data->getLayerIndex());

            if (layer != NULL)
            {
               if (layer->isLayerTypeTopSurface())
               {
                  topDesignSurfaceValueTag = designSurfacePlacedSurface;
                  botDesignSurfaceValueTag = designSurfacePlacedSurface;
                  break;
               }
               else if (layer->isLayerTypeBottomSurface())
               {
                  topDesignSurfaceValueTag = designSurfaceOppositeSurface;
                  botDesignSurfaceValueTag = designSurfaceOppositeSurface;
               }
            }
         }
      }
   }

   // Now that we check all things for surface layer above, the specific outline check
   // below is not really needed. But paranoia leads me to leave it in place.


   // check the comp outline
   if (topDesignSurfaceValueTag <= designSurfaceNone && botDesignSurfaceValueTag <= designSurfaceNone)
   {
      topDesignSurfaceValueTag = designSurfaceNone;
      botDesignSurfaceValueTag = designSurfaceNone;

      for (CDataListIterator dataListIterator(m_dataList, dataTypePoly); dataListIterator.hasNext();)
      {
         DataStruct* data = dataListIterator.getNext();

         if (data == NULL || data->getGraphicClass() != graphicClassComponentOutline)
            continue;
         
         //LayerStruct* layer = doc.getLayerAt(data->getLayerIndex());
         LayerStruct* layer = camCadData.getLayer(data->getLayerIndex());

         if (layer == NULL)
            continue;

         if (layer->getType() == layerTypeTop)
         {
            topDesignSurfaceValueTag = designSurfacePlacedSurface;
            botDesignSurfaceValueTag = designSurfacePlacedSurface;

            break;
         }
         else if (layer->getType() == layerTypeBottom)
         {
            topDesignSurfaceValueTag = designSurfaceOppositeSurface;
            botDesignSurfaceValueTag = designSurfaceOppositeSurface;
         }
      }
   }

   DesignSurfaceValueTag geomDesignSurfaceValueTag = getDesignSurface(topDesignSurfaceValueTag, botDesignSurfaceValueTag);

   WORD topDesignSurfaceKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
   WORD botDesignSurfaceKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);

   CString topDesignSurfaceValueString = DesignSurfaceValueTagToString(topDesignSurfaceValueTag);
   CString botDesignSurfaceValueString = DesignSurfaceValueTagToString(botDesignSurfaceValueTag);
   CString geometryDesignSurfaceValueString = DesignSurfaceValueTagToString(geomDesignSurfaceValueTag);

   camCadData.setAttribute(attributes(),topDesignSurfaceKW,topDesignSurfaceValueString);
   camCadData.setAttribute(attributes(),botDesignSurfaceKW,botDesignSurfaceValueString);
   camCadData.setAttribute(attributes(),geometryDesignSurfaceKW,geometryDesignSurfaceValueString);
}

void BlockStruct::GeneratePadstackDesignSurfaceAttributes(CCamCadData& camCadData, bool regenerate)
{
   if (m_blockType != blockTypePadstack)
      return;

   //WORD topDesKW = doc.RegisterKeyWord(ATT_TOP_DESIGNED_SURFACE, 0, VT_STRING);
   WORD topDesKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
   //WORD botDesKW = doc.RegisterKeyWord(ATT_BOT_DESIGNED_SURFACE, 0, VT_STRING);
   WORD botDesKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);

   CAttribute* attrib = NULL;

   if (lookUpAttrib(topDesKW, attrib) && lookUpAttrib(botDesKW, attrib) && !regenerate)
      return;

   DesignSurfaceValueTag topPadstackDesignSurfaceValueTag = designSurfaceNone;
   DesignSurfaceValueTag botPadstackDesignSurfaceValueTag = designSurfaceNone;

   for (CDataListIterator dataListIterator(m_dataList, dataTypeInsert); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();

      if (data == NULL || data->getInsert() == NULL)
         continue;
   
      //BlockStruct* block = doc.getBlockAt(data->getInsert()->getBlockNumber());
      BlockStruct* block = camCadData.getBlock(data->getInsert()->getBlockNumber());

      if (block == NULL || (!block->isTool() && !block->isAperture()))
         continue;

      //LayerStruct* layer = doc.getLayerAt(data->getLayerIndex());
      LayerStruct* layer = camCadData.getLayer(data->getLayerIndex());

      if (layer == NULL)
         continue;

      getDesignSurfaceFromLayer(*layer, topPadstackDesignSurfaceValueTag, botPadstackDesignSurfaceValueTag);   
   }

   CString topPadstackDesignSurfaceValueString = DesignSurfaceValueTagToString(topPadstackDesignSurfaceValueTag);
   CString botPadstackDesignSurfaceValueString = DesignSurfaceValueTagToString(botPadstackDesignSurfaceValueTag);

   //setAttrib(doc, topDesKW, valueTypeString, topPadstackDesignSurfaceValueString.GetBuffer(0), SA_OVERWRITE, NULL);
   camCadData.setAttribute(attributes(),topDesKW,topPadstackDesignSurfaceValueString);
   //setAttrib(doc, botDesKW, valueTypeString, botPadstackDesignSurfaceValueString.GetBuffer(0), SA_OVERWRITE, NULL);
   camCadData.setAttribute(attributes(),botDesKW,botPadstackDesignSurfaceValueString);
}

/******************************************************************************
* getDesignSurface
   
   Return value:
      topDesignSurfaceValueTag  = used by mirror is false
      botDesignSurfaceValueTag  = used by mirror is true

   This table will help to understand what is visible with the corresponding padstack design surface and insert mirror flag

   PADSTACK_DESIGN_SURFACE                               INSERT_WITH_NO_MIRROR                  INSERT_WITH_MIRRORED
   ==================================================================================================================
   designSurfaceUndefine                                 no                                     no
   designSurfaceNone                                     no                                     no
   designSurfaceBuriedSurface                            no                                     no
   designSurfacePlacedTopOppositeSurface                 bottom                                 no       
   designSurfacePlacedTopPlacedSurface                   top                                    no       
   designSurfacePlacedBottomOppositeSurface              no                                     top                                       
   designSurfacePlacedBottomPlacedSurface                no                                     bottom
   designSurfaceOppositeSurface                          bottom                                 top               
   designSurfacePlacedSurface                            top                                    bottom
   designSurfaceBothSurface                              top & bottom                           top & bottom
*/
void getDesignSurfaceValueTag(DesignSurfaceValueTag newTopDesignSurfaceValueTag, DesignSurfaceValueTag newBotDesignSurfaceValueTag,
                              DesignSurfaceValueTag& topDesignSurfaceValueTag, DesignSurfaceValueTag& botDesignSurfaceValueTag)
{
   bool visibleOnTop, visibleOnBottom, newVisibleOnTop,  newVisibleOnBottom;

   // Determin the return value of the topDesignSurfaceValueTag
   visibleOnTop         = (topDesignSurfaceValueTag == designSurfacePlacedTopPlacedSurface || 
                           topDesignSurfaceValueTag == designSurfacePlacedSurface ||
                           topDesignSurfaceValueTag == designSurfaceBothSurface );

   visibleOnBottom      = (topDesignSurfaceValueTag == designSurfacePlacedTopOppositeSurface ||
                           topDesignSurfaceValueTag == designSurfaceOppositeSurface ||
                           topDesignSurfaceValueTag == designSurfaceBothSurface );

   newVisibleOnTop      = (newTopDesignSurfaceValueTag == designSurfacePlacedTopPlacedSurface || 
                           newTopDesignSurfaceValueTag == designSurfacePlacedSurface ||
                           newTopDesignSurfaceValueTag == designSurfaceBothSurface );

   newVisibleOnBottom   = (newTopDesignSurfaceValueTag == designSurfacePlacedTopOppositeSurface ||
                           newTopDesignSurfaceValueTag == designSurfaceOppositeSurface ||
                           newTopDesignSurfaceValueTag == designSurfaceBothSurface );

   if ((visibleOnTop && newVisibleOnBottom) || (visibleOnBottom && newVisibleOnTop))
   {
      topDesignSurfaceValueTag = designSurfaceBothSurface;
   }
   else if (topDesignSurfaceValueTag < newTopDesignSurfaceValueTag)
   {
      topDesignSurfaceValueTag = newTopDesignSurfaceValueTag;
   }


   // Determin the return value of the topDesignSurfaceValueTag
   visibleOnTop         = (botDesignSurfaceValueTag == designSurfacePlacedBottomOppositeSurface || 
                           botDesignSurfaceValueTag == designSurfaceOppositeSurface ||
                           botDesignSurfaceValueTag == designSurfaceBothSurface );

   visibleOnBottom      = (botDesignSurfaceValueTag == designSurfacePlacedBottomPlacedSurface ||
                           botDesignSurfaceValueTag == designSurfacePlacedSurface ||
                           botDesignSurfaceValueTag == designSurfaceBothSurface );

   newVisibleOnTop      = (newBotDesignSurfaceValueTag == designSurfacePlacedBottomOppositeSurface || 
                           newBotDesignSurfaceValueTag == designSurfaceOppositeSurface ||
                           newBotDesignSurfaceValueTag == designSurfaceBothSurface );

   newVisibleOnBottom   = (newBotDesignSurfaceValueTag == designSurfacePlacedBottomPlacedSurface ||
                           newBotDesignSurfaceValueTag == designSurfacePlacedSurface ||
                           newBotDesignSurfaceValueTag == designSurfaceBothSurface );

   if ((visibleOnTop && newVisibleOnBottom) || (visibleOnBottom && newVisibleOnTop))
   {
      botDesignSurfaceValueTag = designSurfaceBothSurface;
   }
   else if (botDesignSurfaceValueTag < newBotDesignSurfaceValueTag)
   {
      botDesignSurfaceValueTag = newBotDesignSurfaceValueTag;
   }
}

bool BlockStruct::IsAccessibleFromTop(CCamCadData& camCadData, bool mirrored)
{
   WORD topDesKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
   WORD botDesKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);

   CAttribute* attrib = NULL;

   if (!mirrored)
   {
      if (lookUpAttrib(topDesKW, attrib) && attrib != NULL)
      {
         //CString desSurfValue = get_attvalue_string(&doc, attrib);
         CString desSurfValue = camCadData.getAttributeStringValue(*attrib);
         DesignSurfaceValueTag designSurfaceValueTag = DesignSurfaceValueStringToTag(desSurfValue);

         if (designSurfaceValueTag == designSurfacePlacedSurface || 
             designSurfaceValueTag == designSurfaceBothSurface   ||
             designSurfaceValueTag == designSurfacePlacedTopPlacedSurface)
         {
             return true;
         }
      }
   }
   else
   {
      if (lookUpAttrib(botDesKW, attrib) && attrib != NULL)
      {
         //CString desSurfValue = get_attvalue_string(&doc, attrib);
         CString desSurfValue = camCadData.getAttributeStringValue(*attrib);
         DesignSurfaceValueTag designSurfaceValueTag = DesignSurfaceValueStringToTag(desSurfValue);

         if (designSurfaceValueTag == designSurfaceOppositeSurface || 
             designSurfaceValueTag == designSurfaceBothSurface   ||
             designSurfaceValueTag == designSurfacePlacedBottomOppositeSurface)
         {
             return true;
         }
      }
   }

   return false;
}

bool BlockStruct::IsAccessibleFromBottom(CCamCadData& camCadData, bool mirrored)
{
   //WORD topDesKW = doc.RegisterKeyWord(ATT_TOP_DESIGNED_SURFACE, 0, VT_STRING);
   WORD topDesKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
   //WORD botDesKW = doc.RegisterKeyWord(ATT_BOT_DESIGNED_SURFACE, 0, VT_STRING);
   WORD botDesKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);

   CAttribute* attrib = NULL;

   if (!mirrored)
   {
      if (lookUpAttrib(topDesKW, attrib) && attrib != NULL)
      {
         //CString desSurfValue = get_attvalue_string(&doc, attrib);
         CString desSurfValue = camCadData.getAttributeStringValue(*attrib);
         DesignSurfaceValueTag designSurfaceValueTag = DesignSurfaceValueStringToTag(desSurfValue);

         if (designSurfaceValueTag == designSurfaceOppositeSurface || 
             designSurfaceValueTag == designSurfaceBothSurface   ||
             designSurfaceValueTag == designSurfacePlacedTopOppositeSurface)
         {
             return true;
         }
      }
   }
   else
   {
      if (lookUpAttrib(botDesKW, attrib) && attrib != NULL)
      {
         //CString desSurfValue = get_attvalue_string(&doc, attrib);
         CString desSurfValue = camCadData.getAttributeStringValue(*attrib);
         DesignSurfaceValueTag designSurfaceValueTag = DesignSurfaceValueStringToTag(desSurfValue);

         if (designSurfaceValueTag == designSurfacePlacedSurface || 
             designSurfaceValueTag == designSurfaceBothSurface   ||
             designSurfaceValueTag == designSurfacePlacedBottomPlacedSurface)
         {
             return true;
         }
      }
   }

   return false;
}

void BlockStruct::Scale(double factor)
{
   m_extent.scale(factor);

   if (isAperture() || isTool() || isDrillHole())
   {
      if (!isComplexAperture())
      {
         m_sizeA *= (DbUnit)factor;
         m_sizeB *= (DbUnit)factor;
         m_sizeC *= (DbUnit)factor;
         m_xOffset *= (DbUnit)factor;
         m_yOffset *= (DbUnit)factor;
         m_toolSize *= (DbUnit)factor;
      }
   }
   else
   {
      for (POSITION dataPos = m_dataList.GetHeadPosition();dataPos != NULL;)
      {
         DataStruct* data = m_dataList.GetNext(dataPos);

         CTMatrix mat;
         mat.scale(factor, factor);
         data->transform(mat, NULL, true);
      }
   }

   if (m_attributes != NULL)
   {
      CTMatrix mat;
      mat.scale(factor, factor);
      m_attributes->transform(mat);
   }
}

/******************************************************************************
* BlockStruct::GetFirstPinInsertData
*/
DataStruct *BlockStruct::GetFirstPinInsertData()
{
   // Find the first insertTypePin in the block
   for(POSITION dataPos = getHeadDataInsertPosition();dataPos;)
   {
      DataStruct *data = getNextDataInsert(dataPos);
      if(data && data->isInsertType(insertTypePin))
         return data;
   }

   return NULL;
}

/******************************************************************************
* BlockStruct::GetFirstPinInsertPadstackGeometry
*/
BlockStruct *BlockStruct::GetFirstPinInsertPadstackGeometry (CCamCadData& camCadData)
{
   DataStruct *data = this->GetFirstPinInsertData();
   if(data)
   {
      BlockStruct *padstackBlock =  camCadData.getBlockAt(data->getInsert()->getBlockNumber());
      return (padstackBlock && padstackBlock->getBlockType() == blockTypePadstack)?padstackBlock:NULL;
   }
   return NULL;
}

void BlockStruct::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   writeFormat.writef(
"BlockStruct\n"
"{\n"
"   num=%d\n"
"   name='%s'\n"
"   filenum=%d\n"
"   flag=0x%04x\n"
"   blocktype=%d (%s)\n"
"   shape=%d (%s)\n"
"   [(xmin,ymin),(xmax,ymax)]=[(%.3f,%.3f),(%.3f,%.3f)]\n"
"   [(m_sizeA,m_sizeB),(m_sizeC,m_sizeD)]=[(%.3f,%.3f),(%.3f,%.3f)]\n"
"   (xoffset,yoffset)=(%.3f,%.3f)\n"
"   rotation=%.3f\n"
"   tcode=%d\n"
"   geomnum=%d\n"
,
getBlockNumber(),
(const char*)m_name,
m_fileNumber,
getFlags(),
m_blockType,(const char*)blockTypeToString(m_blockType),
m_shape,(const char*)apertureShapeToString(m_shape),
getXmin(),getYmin(),getXmax(),getYmax(),
m_sizeA,m_sizeB,m_sizeC,m_sizeD,
m_xOffset,m_yOffset,
m_rotationRadians,
m_tCode,
m_toolBlockNumber
);

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      m_dataList.dump(writeFormat,depth);

      CAttributeMapWrap attributeMap(m_attributes);
      attributeMap.dump(writeFormat,depth);

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n\n");
}

//_____________________________________________________________________________
CBlockArray::CBlockArray(CCamCadData& camCadData,int growBy)
: m_camCadData(camCadData)
, m_blocks(growBy)
{
}

CBlockArray::~CBlockArray()
{
   empty();
}

void CBlockArray::empty()
{
   m_blocks.empty();
}

bool CBlockArray::removeBlock(BlockStruct* block)
{
   bool retval = false;

   if (block != NULL)
   {
      int blockNumber = block->getBlockNumber();

      if (blockNumber < m_blocks.GetSize())
      {
         if (m_blocks.GetAt(blockNumber) == block)
         {
            retval = true;
            m_blocks.SetAt(blockNumber,NULL);
         }
      }
   }

   return retval;
}

int CBlockArray::getSize() const
{
   return (int) m_blocks.GetSize();
}

BlockStruct& CBlockArray::getNewBlock()
{
   BlockStruct& block = getDefinedBlockAt(m_blocks.GetSize());

   return block;
}

BlockStruct& CBlockArray::getNewBlock(const CString& name,int fileNumber,BlockTypeTag blockType)
{
   int blockNumber = m_blocks.GetSize();

   BlockStruct* block = new BlockStruct(m_camCadData,blockNumber,name,fileNumber,blockType);
   m_blocks.SetAtGrow(blockNumber,block);

   return *block;
}

BlockStruct* CBlockArray::getBlockAt(int index) const
{
   BlockStruct* block = NULL;
   
   if (index >= 0 && index < m_blocks.GetSize())
   {
      block = m_blocks.GetAt(index);
   }

   return block;
}

BlockStruct& CBlockArray::getDefinedBlockAt(int index)
{
   BlockStruct* block = NULL;

   if (index < 0)
   {
      index = 0;
   }

   if (index < m_blocks.GetSize())
   {
      block = m_blocks.GetAt(index);
   }

   if (block == NULL)
   {
      block = new BlockStruct(m_camCadData,index);
      m_blocks.SetAtGrow(index,block);
   }

   return *block;
}

BlockStruct& CBlockArray::getDefinedBlockAt(int index,const CString& name,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct* block = NULL;

   if (index < 0)
   {
      index = 0;
   }

   if (index < m_blocks.GetSize())
   {
      block = m_blocks.GetAt(index);
   }

   if (block == NULL)
   {
      block = new BlockStruct(m_camCadData,index,name,fileNumber,blockType);
      m_blocks.SetAtGrow(index,block);
   }

   return *block;
}

bool CBlockArray::swapBlocks(BlockStruct* block0,BlockStruct* block1)
{
   bool retval = false;

   if (block0 != NULL && block1 != NULL)
   {
      if (block0->getBlockNumber() < m_blocks.GetSize() && 
          block1->getBlockNumber() < m_blocks.GetSize()  )
      {
         if (m_blocks.GetAt(block0->getBlockNumber()) == block0 &&
             m_blocks.GetAt(block1->getBlockNumber()) == block1     )
         {
            retval = true;

            block0->swapBlockNumber(*block1);

            m_blocks.SetAt(block0->getBlockNumber(),block0);
            m_blocks.SetAt(block1->getBlockNumber(),block1);
         }
      }
   }

   return retval;
}

bool CBlockArray::isValid() const
{
   bool retval = true;

   for (int index = 0;index < m_blocks.GetSize();index++)
   {
      BlockStruct* block = m_blocks.GetAt(index);

      if (block != NULL)
      {
         if (! block->isValid())
         {
            retval = false;
         }
      }
   }

   return retval;
}

void CBlockArray::GenerateDesignSurfaceInfo(CCamCadData& camCadData, bool regenerate)
{
   if (regenerate)
   {
      // Remove the following attributes from block and have them regenerate
      //WORD desKW = camCadData.RegisterKeyWord(ATT_DESIGNED_SURFACE, 0, VT_STRING);
      WORD desKW = camCadData.getAttributeKeywordIndex(standardAttributeDesignedSurface);
      //WORD topDesKW = camCadData.RegisterKeyWord(ATT_TOP_DESIGNED_SURFACE, 0, VT_STRING);
      WORD topDesKW = camCadData.getAttributeKeywordIndex(standardAttributeTopPadstackDesignSurface);
      //WORD botDesKW = camCadData.RegisterKeyWord(ATT_BOT_DESIGNED_SURFACE, 0, VT_STRING);
      WORD botDesKW = camCadData.getAttributeKeywordIndex(standardAttributeBottomPadstackDesignSurface);

      for (int i=0; i<m_blocks.GetSize(); i++)
      {
         BlockStruct* block = m_blocks.GetAt(i);

         if (block != NULL)
         {
            block->attributes().removeAttribute(desKW);
            block->attributes().removeAttribute(topDesKW);
            block->attributes().removeAttribute(botDesKW);
         }
      }
   }

   for (int i=0; i<m_blocks.GetSize(); i++)
   {
      BlockStruct* block = m_blocks.GetAt(i);

      if (block != NULL)
      {
         block->GenerateDesignSurfaceAttribute(camCadData, regenerate);
      }
   }
}

/******************************************************************************
* getDesignSurface
*/
void getDesignSurfaceFromLayer(LayerStruct& layer, DesignSurfaceValueTag& topDesignSurfaceValueTag, DesignSurfaceValueTag& botDesignSurfaceValueTag)
{
   LayerTypeTag layerType = (LayerTypeTag)layer.getType();

   if (layerType != layerTypeSignalTop       && layerType != layerTypePadTop &&
       layerType != layerTypeSignalInner     && layerType != layerTypePadInner &&
       layerType != layerTypeSignalAll       && layerType != layerTypePadAll &&
       layerType != layerTypeSignalBottom    && layerType != layerTypePadBottom &&
       layerType != layerTypeSignalOuter     && layerType != layerTypePadOuter)
   {
      return;
   }

   DesignSurfaceValueTag tmpTopDesignSurfaceValueTag = designSurfaceNone;        
   DesignSurfaceValueTag tmpBotDesignSurfaceValueTag = designSurfaceNone;

   if (layerType == layerTypeSignalAll || layerType == layerTypePadAll)
   {
      tmpTopDesignSurfaceValueTag = designSurfaceBothSurface;
      tmpBotDesignSurfaceValueTag = designSurfaceBothSurface;
   }
   else if (layerType == layerTypeSignalTop   || layerType == layerTypePadTop ||
            layerType == layerTypeSignalAll   || layerType == layerTypePadAll ||
            layerType == layerTypeSignalOuter || layerType == layerTypePadOuter   )
   {
      if (topDesignSurfaceValueTag == designSurfaceBothSurface)
         return;  // No need to continue to evaluate

      if (layer.getNeverMirror())
      {
         // Mirror never
         tmpTopDesignSurfaceValueTag = designSurfacePlacedTopPlacedSurface;
      }
      else if (layer.getMirrorOnly())
      {
         // Mirror always
         tmpTopDesignSurfaceValueTag = designSurfacePlacedBottomOppositeSurface;
      }
      else
      {
         if (layer.getMirroredLayerIndex() == layer.getLayerIndex() || layer.getLayerIndex() < 0)
         {
            // Mirror normal with no mirroring layer
            tmpTopDesignSurfaceValueTag = designSurfacePlacedTopPlacedSurface;
            tmpBotDesignSurfaceValueTag = designSurfacePlacedBottomOppositeSurface;
         }
         else
         {
            // Mirror normal with mirroring layer
            tmpTopDesignSurfaceValueTag = designSurfacePlacedSurface;
            tmpBotDesignSurfaceValueTag = designSurfacePlacedSurface;
         }
      }
   }
   else if (layerType == layerTypeSignalInner || layerType == layerTypePadInner)
   {
      if (topDesignSurfaceValueTag > designSurfaceBuriedSurface && botDesignSurfaceValueTag > designSurfaceBuriedSurface)
         return;  // No need to continue to evaluate

      tmpTopDesignSurfaceValueTag = designSurfaceBuriedSurface;   
      tmpBotDesignSurfaceValueTag = designSurfaceBuriedSurface;   
   }  
   else if (layerType == layerTypeSignalBottom  || layerType == layerTypePadBottom)
   {
      if (botDesignSurfaceValueTag == designSurfaceBothSurface)
         return;  // No need to continue to evaluate

      if (layer.getNeverMirror())
      {
         // Mirror never
         tmpTopDesignSurfaceValueTag = designSurfacePlacedTopOppositeSurface;
      }
      else if (layer.getMirrorOnly())
      {
         // Mirror always
         tmpBotDesignSurfaceValueTag = designSurfacePlacedBottomPlacedSurface;
      }
      else 
      {
         if (layer.getMirroredLayerIndex() == layer.getLayerIndex() || layer.getLayerIndex() < 0)
         {
            // Mirror normal with no mirroring layer
            tmpTopDesignSurfaceValueTag = designSurfacePlacedTopOppositeSurface;
            tmpBotDesignSurfaceValueTag = designSurfacePlacedBottomPlacedSurface;
         }
         else
         {
            // Mirror normal with mirroring layer
            tmpTopDesignSurfaceValueTag = designSurfaceOppositeSurface;
            tmpBotDesignSurfaceValueTag = designSurfaceOppositeSurface;
         }
      }
   }

   getDesignSurfaceValueTag(tmpTopDesignSurfaceValueTag, tmpBotDesignSurfaceValueTag, topDesignSurfaceValueTag, botDesignSurfaceValueTag);
}

//_________________________________________________________________________________________________
CBlockList::CBlockList(bool isContainer)
: m_blocks(isContainer)
{
}

CBlockList::~CBlockList()
{
}

void CBlockList::empty()
{
   m_blocks.empty();
}

bool CBlockList::isEmpty() const
{
   return (getCount() == 0);
}

int CBlockList::getCount() const
{
   return m_blocks.GetCount();
}

BlockStruct* CBlockList::getHead()
{
   BlockStruct* block = NULL;

   if (m_blocks.GetCount() > 0)
   {
      block = m_blocks.GetHead();
   }

   return block;
}

POSITION CBlockList::getHeadPosition() const
{
   return m_blocks.GetHeadPosition();
}

BlockStruct* CBlockList::getNext(POSITION& pos) const
{
   return m_blocks.GetNext(pos);
}

void CBlockList::addTail(BlockStruct* block)
{
   m_blocks.AddTail(block);
}

bool CBlockList::remove(BlockStruct* block)
{
   POSITION pos = m_blocks.Find(block);

   bool retval = (pos != NULL);

   if (retval)
   {
      m_blocks.RemoveAt(pos);
   }

   return retval;
}

void CBlockList::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CMapOfBlockLists\n"
"{\n");

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = m_blocks.GetHeadPosition();pos != NULL;)
      {
         BlockStruct* block = m_blocks.GetNext(pos);

         writeFormat.writef(
"BlockStruct: blockNumber=%d, blockName='%s', blockType='%s'\n",
            block->getBlockNumber(),block->getName(),blockTypeToDisplayString(block->getBlockType()));
      }

      writeFormat.popHeader();
   }

   writeFormat.writef(
"}\n\n");
}

//_________________________________________________________________________________________________
CMapOfBlockLists::CMapOfBlockLists(int hashSize)
{
   m_mapOfBlockLists.InitHashTable(hashSize,0);
}

//void CMapOfBlockLists::empty()
//{
//   m_mapOfBlockLists.empty();
//}

bool CMapOfBlockLists::contains(const CString& blockName) const
{
   bool containsFlag = (getAt(blockName) != NULL);

   return containsFlag;
}

BlockStruct* CMapOfBlockLists::getAt(const CString& blockName) const
{
   BlockStruct* block = NULL;
   CBlockList* blockList = NULL;

   if (m_mapOfBlockLists.Lookup(blockName,blockList))
   {
      block = blockList->getHead();
   }

   return block;
}

CBlockList* CMapOfBlockLists::getListAt(const CString& blockName) const
{
   CBlockList* blockList = NULL;

   m_mapOfBlockLists.Lookup(blockName,blockList);

   return blockList;
}

void CMapOfBlockLists::add(BlockStruct* block)
{
   if (block != NULL)
   {
      CString blockName = block->getName();

      CBlockList* blockList = NULL;

      m_mapOfBlockLists.Lookup(blockName,blockList);

      if (blockList == NULL)
      {
         blockList = new CBlockList(false);
         m_mapOfBlockLists.setAt(blockName,blockList);
      }

      blockList->addTail(block);
   }
}

bool CMapOfBlockLists::remove(BlockStruct* block)
{
   bool retval = false;

   if (block != NULL)
   {
      CString blockName = block->getName();

      CBlockList* blockList = NULL;

      if (m_mapOfBlockLists.Lookup(blockName,blockList))
      {
         retval = blockList->remove(block);

         if (blockList->isEmpty())
         {
            m_mapOfBlockLists.RemoveKey(blockName);
            delete blockList;
         }

      }
   }

   return retval;
}

void CMapOfBlockLists::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CMapOfBlockLists\n"
"{\n");

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (POSITION pos = m_mapOfBlockLists.GetStartPosition();pos != NULL;)
      {
         CString blockName;
         CBlockList* blockList;

         m_mapOfBlockLists.GetNextAssoc(pos,blockName,blockList);

         writeFormat.writef(
"key='%s', blockList.GetCount=%d\n",
            blockName,blockList->getCount());

         blockList->dump(writeFormat,depth);
      }

      writeFormat.popHeader();
   }

   writeFormat.writef(
"}\n\n");
}

//_____________________________________________________________________________
void CArrayOfMapsOfBlockLists::SetSize(int newSize,int growBy)
{
   m_arrayOfMapsOfBlockLists.SetSize(newSize,growBy);
}

int CArrayOfMapsOfBlockLists::GetSize() const
{
   return m_arrayOfMapsOfBlockLists.GetSize();
}

CMapOfBlockLists* CArrayOfMapsOfBlockLists::GetAt(int index) const
{
   return m_arrayOfMapsOfBlockLists.GetAt(index);
}

void CArrayOfMapsOfBlockLists::SetAtGrow(int index,CMapOfBlockLists* mapOfBlockLists)
{
   m_arrayOfMapsOfBlockLists.SetAtGrow(index,mapOfBlockLists);
}

void CArrayOfMapsOfBlockLists::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CArrayOfMapsOfBlockLists\n"
"{\n");

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (int blockTypeIndex = 0;blockTypeIndex < m_arrayOfMapsOfBlockLists.GetSize();blockTypeIndex++)
      {
         CMapOfBlockLists* mapOfBlockLists = m_arrayOfMapsOfBlockLists.GetAt(blockTypeIndex);

         writeFormat.writef(
"index = %d (%s), mapOfBlockLists = %p\n",blockTypeIndex,blockTypeToDisplayString(intToBlockTypeTag(blockTypeIndex)),mapOfBlockLists);

         if (mapOfBlockLists != NULL)
         {
            mapOfBlockLists->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef(
"}\n\n");
}

//_________________________________________________________________________________________________
CBlockDirectoryForFile::CBlockDirectoryForFile(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_map(nextPrime2n(2000))
{
   m_arrayOfMapsOfBlockLists.SetSize(0,4);
}

//CBlockDirectoryForFile::~CBlockDirectoryForFile()
//{
//}

//void CBlockDirectoryForFile::deleteAll()
//{
//}

void CBlockDirectoryForFile::add(BlockStruct* block)
{
   if (block != NULL)
   {
      m_map.add(block);

      CMapOfBlockLists& mapOfBlockLists = getDefinedMapOfBlockListsForBlockType(block->getBlockType());

      mapOfBlockLists.add(block);
   }
}

bool CBlockDirectoryForFile::remove(BlockStruct* block)
{
   bool retval = false;

   if (block != NULL)
   {
      retval = m_map.remove(block);

      CMapOfBlockLists& mapOfBlockLists = getDefinedMapOfBlockListsForBlockType(block->getBlockType());

      retval = mapOfBlockLists.remove(block) || retval;
   }

   return retval;
}

BlockStruct* CBlockDirectoryForFile::getAt(const CString& name) const
{
   BlockStruct* block = m_map.getAt(name);

   return block;
}

BlockStruct* CBlockDirectoryForFile::getAt(const CString& name,BlockTypeTag blockType) const
{
   BlockStruct* block = NULL;

   CMapOfBlockLists* mapOfBlockListsForBlockType = getMapOfBlockListsForBlockType(blockType);

   if (mapOfBlockListsForBlockType != NULL)
   {
      block = mapOfBlockListsForBlockType->getAt(name);
   }

   return block;
}

CBlockList* CBlockDirectoryForFile::getListAt(const CString& name) const
{
   CBlockList* blockList = m_map.getListAt(name);

   return blockList;
}

CBlockList* CBlockDirectoryForFile::getListAt(const CString& name,BlockTypeTag blockType) const
{
   CBlockList* blockList = NULL;

   CMapOfBlockLists* mapOfBlockListsForBlockType = getMapOfBlockListsForBlockType(blockType);

   if (mapOfBlockListsForBlockType != NULL)
   {
      blockList = mapOfBlockListsForBlockType->getListAt(name);
   }

   return blockList;
}

CMapOfBlockLists* CBlockDirectoryForFile::getMapOfBlockListsForBlockType(BlockTypeTag blockType) const
{
   if (blockType < blockTypeUnknown)
   {
      blockType = blockTypeUnknown;
   }

   CMapOfBlockLists* mapOfBlockLists = NULL;

   if (blockType < m_arrayOfMapsOfBlockLists.GetSize())
   {
      mapOfBlockLists = m_arrayOfMapsOfBlockLists.GetAt(blockType);
   }

   return mapOfBlockLists;
}

CMapOfBlockLists& CBlockDirectoryForFile::getDefinedMapOfBlockListsForBlockType(BlockTypeTag blockType)
{
   if (blockType < blockTypeUnknown)
   {
      blockType = blockTypeUnknown;
   }

   CMapOfBlockLists* mapOfBlockLists = NULL;

   if (blockType < m_arrayOfMapsOfBlockLists.GetSize())
   {
      mapOfBlockLists = m_arrayOfMapsOfBlockLists.GetAt(blockType);
   }

   if (mapOfBlockLists == NULL)
   {
      mapOfBlockLists = new CMapOfBlockLists(nextPrime2n(2000));
      m_arrayOfMapsOfBlockLists.SetAtGrow(blockType,mapOfBlockLists);
   }

   return *mapOfBlockLists;
}

void CBlockDirectoryForFile::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CBlockDirectoryForFile\n"
"{\n");

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      writeFormat.writef(
"m_map - ");

      m_map.dump(writeFormat,depth);

      writeFormat.writef(
"m_arrayOfMapsOfBlockLists - ");

      m_arrayOfMapsOfBlockLists.dump(writeFormat,depth);

      writeFormat.popHeader();
   }

   writeFormat.writef(
"}\n\n");
}

//_____________________________________________________________________________
void CBlockDirectoryForFiles::SetSize(int newSize,int growBy)
{
   m_blockDirectoryForFiles.SetSize(newSize,growBy);
}

int CBlockDirectoryForFiles::GetSize() const
{
   return m_blockDirectoryForFiles.GetSize();
}

CBlockDirectoryForFile* CBlockDirectoryForFiles::GetAt(int index) const
{
   return m_blockDirectoryForFiles.GetAt(index);
}

void CBlockDirectoryForFiles::SetAtGrow(int index,CBlockDirectoryForFile* blockDirectoryForFile)
{
   m_blockDirectoryForFiles.SetAtGrow(index,blockDirectoryForFile);
}

void CBlockDirectoryForFiles::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef("CBlockDirectoryForFiles\n{\n");

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (int fileIndex = 0;fileIndex < m_blockDirectoryForFiles.GetSize();fileIndex++)
      {
         CBlockDirectoryForFile* blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(fileIndex);

         writeFormat.writef("index = %d, blockDirectory = %p\n",fileIndex,blockDirectoryForFile);

         if (blockDirectoryForFile != NULL)
         {
            blockDirectoryForFile->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n\n");
}

//_________________________________________________________________________________________________
CBlockDirectory::CBlockDirectory(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_blockArray(camCadData,4000)
{
   m_blockDirectoryForFiles.SetSize(0,4);
}

CBlockDirectory::~CBlockDirectory()
{
   m_blockArray.empty();
}

BlockStruct& CBlockDirectory::getNewBlock(const CString& name,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct& block = m_blockArray.getNewBlock(name,fileNumber,blockType);

   add(&block);

   return block;
}

BlockStruct& CBlockDirectory::getDefinedBlock(const CString& name,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct* block = getAt(name,fileNumber,blockType);

   if (block == NULL)
   {
      block = &(m_blockArray.getNewBlock(name,fileNumber,blockType));
      add(block);
   }

   return *block;
}

BlockStruct& CBlockDirectory::getDefinedBlock(int blockNumber,const CString& name,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct* block = getAt(blockNumber);

   if (block == NULL)
   {
      block = &(m_blockArray.getDefinedBlockAt(blockNumber,name,fileNumber,blockType));
      add(block);
   }

   return *block;
}

BlockStruct& CBlockDirectory::getDefinedBlock(int blockNumber)
{
   BlockStruct* block = getAt(blockNumber);

   if (block == NULL)
   {
      block = &(m_blockArray.getDefinedBlockAt(blockNumber,"",-1,blockTypeUnknown));
      add(block);
   }

   return *block;
}

BlockStruct* CBlockDirectory::getAt(int blockNumber) const
{
   return m_blockArray.getBlockAt(blockNumber);
}

BlockStruct* CBlockDirectory::getAt(const CString& name) const
{
   return getAt(name,-1);
}

BlockStruct* CBlockDirectory::getAt(const CString& name,int fileNumber) const
{
   BlockStruct* block = NULL;

   int index = fileNumber + 1;

   if (index < 0)
   {
      index = 0;
   }

   if (index < m_blockDirectoryForFiles.GetSize())
   {
      CBlockDirectoryForFile* blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(index);

      if (blockDirectoryForFile != NULL)
      {
         block = blockDirectoryForFile->getAt(name);
      }
   }

   return block;
}

BlockStruct* CBlockDirectory::getAt(const CString& name,int fileNumber,BlockTypeTag blockType) const
{
   BlockStruct* block = NULL;

   int index = fileNumber + 1;

   if (index < 0)
   {
      index = 0;
   }

   if (index < m_blockDirectoryForFiles.GetSize())
   {
      CBlockDirectoryForFile* blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(index);

      if (blockDirectoryForFile != NULL)
      {
         block = blockDirectoryForFile->getAt(name,blockType);
      }
   }

   return block;
}

CBlockList* CBlockDirectory::getListAt(const CString& name) const
{
   return getListAt(name,-1);
}

CBlockList* CBlockDirectory::getListAt(const CString& name,int fileNumber) const
{
   CBlockList* blockList = NULL;

   int index = fileNumber + 1;

   if (index < 0)
   {
      index = 0;
   }

   if (index < m_blockDirectoryForFiles.GetSize())
   {
      CBlockDirectoryForFile* blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(index);

      if (blockDirectoryForFile != NULL)
      {
         blockList = blockDirectoryForFile->getListAt(name);
      }
   }

   return blockList;
}

CBlockList* CBlockDirectory::getListAt(const CString& name,int fileNumber,BlockTypeTag blockType) const
{
   CBlockList* blockList = NULL;

   int index = fileNumber + 1;

   if (index < 0)
   {
      index = 0;
   }

   if (index < m_blockDirectoryForFiles.GetSize())
   {
      CBlockDirectoryForFile* blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(index);

      if (blockDirectoryForFile != NULL)
      {
         blockList = blockDirectoryForFile->getListAt(name,blockType);
      }
   }

   return blockList;
}

BlockStruct& CBlockDirectory::copyBlock(const CString& newName,BlockStruct& srcBlock,bool copyAttributesFlag)
{
   BlockStruct& block = getDefinedBlock(newName,srcBlock.getFileNumber(),srcBlock.getBlockType());

   block.setProperties(srcBlock,copyAttributesFlag);
   block.setName(newName);

   return block;
}

void CBlockDirectory::add(BlockStruct* block)
{
   if (block != NULL)
   {
      int fileNumber = block->getFileNumber();

      if (fileNumber < -1)
      {
         fileNumber = -1;
         block->updateFileNumber(fileNumber);
      }

      int index = fileNumber + 1;
      CBlockDirectoryForFile* blockDirectoryForFile = NULL;

      if (index < m_blockDirectoryForFiles.GetSize())
      {
         blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(index);
      }

      if (blockDirectoryForFile == NULL)
      {
         blockDirectoryForFile = new CBlockDirectoryForFile(m_camCadData);
         m_blockDirectoryForFiles.SetAtGrow(index,blockDirectoryForFile);
      }

      blockDirectoryForFile->add(block);
   }
}

void CBlockDirectory::add(BlockStruct* block,int fileNumber)
{
   if (block != NULL)
   {
      block->updateFileNumber(fileNumber);
   }

   add(block);
}

void CBlockDirectory::remove(BlockStruct* block)
{
   if (block != NULL)
   {
      int index = block->getFileNumber() + 1;

      if (index >= 0 && index < m_blockDirectoryForFiles.GetSize())
      {
         CBlockDirectoryForFile* blockDirectoryForFile = m_blockDirectoryForFiles.GetAt(index);

         if (blockDirectoryForFile != NULL)
         {
            blockDirectoryForFile->remove(block);
         }
      }
   }
}

void CBlockDirectory::removeFile(FileStruct& file)
{
   CBlockList blocksToRemove(false);

   if (&m_camCadData == &(file.getCamCadData()))
   {
      int fileNumber = file.getFileNumber();
      CBlockArray& blockArray = m_camCadData.getBlockArray();

      for (int blockIndex = 0;blockIndex < blockArray.getSize();blockIndex++)
      {
         BlockStruct* block = blockArray.getBlockAt(blockIndex);

         if (block != NULL && block->getFileNumber() == fileNumber)
         {            
            blocksToRemove.addTail(block);
         }
      }

      CWidthTableArray& widthTableArray = m_camCadData.getWidthTable();
      for (int widthIndex = 0;widthIndex < widthTableArray.GetSize();widthIndex++)
      {
         BlockStruct* widthBlk  = widthTableArray.getAt(widthIndex);

         if (widthBlk  != NULL && widthBlk ->getFileNumber() == fileNumber)
         {
            widthTableArray.SetAt(widthIndex,NULL);
         }
      }
   }

   for (POSITION pos = blocksToRemove.getHeadPosition();pos != NULL;)
   {
      BlockStruct* block = blocksToRemove.getNext(pos);

      delete block;
   }
}

void CBlockDirectory::updateName(BlockStruct& block,const CString& newBlockName)
{
   if (newBlockName.Compare(block.getName()) != 0)
   {
      remove(&block);
      block.updateName(newBlockName);
      add(&block);
   }
}

void CBlockDirectory::updateFileNumber(BlockStruct& block,int newFileNumber)
{
   if (newFileNumber < -1)
   {
      newFileNumber = -1;
   }

   if (block.getFileNumber() != newFileNumber)
   {
      remove(&block);
      block.updateFileNumber(newFileNumber);
      add(&block);
   }
}

void CBlockDirectory::updateBlockType(BlockStruct& block,BlockTypeTag newBlockType)
{
   if (newBlockType < blockTypeUnknown)
   {
      newBlockType = blockTypeUnknown;
   }

   if (block.getBlockType() != newBlockType)
   {
      remove(&block);
      block.updateBlockType(newBlockType);
      add(&block);
   }
}

void CBlockDirectory::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef("CBlockDirectory\n{\n");

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      writeFormat.writef("m_blockArray\n" );

      for (int blockIndex = 0;blockIndex < m_blockArray.getSize();blockIndex++)
      {
         BlockStruct* block = m_blockArray.getBlockAt(blockIndex);

         if (block != NULL)
         {
            writeFormat.writef("BlockStruct: blockNumber=%4d, blockName='%s', blockType='%s'\n",
               block->getBlockNumber(),block->getName(),blockTypeToDisplayString(block->getBlockType()));
         }
      }

      writeFormat.writef("\nm_blockDirectoryForFiles - ");

      m_blockDirectoryForFiles.dump(writeFormat,depth);

      writeFormat.popHeader();
   }

   writeFormat.writef( "}\n\n" );
}














