
#include "StdAfx.h"
#include "DcaLegacyGraph.h"
#include "DcaCamCadData.h"
#include "DcaDataType.h"
#include "DcaInsert.h"
#include "DcaAttributes.h"
#include "DcaLayerType.h"
#include "DcaPoly.h"
#include "DcaTMatrix.h"
#include "DcaText.h"
#include "DcaWriteFormat.h"

//_____________________________________________________________________________
CDcaLegacyGraph::CDcaLegacyGraph(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_currentFile(NULL)
, m_currentDataList(NULL)
, m_currentPolyList(NULL)
, m_currentPoly(NULL)
, m_traceWriteFormat(NULL)
{
   m_blockArray.SetSize(0,MaxBlockLevel);
}

FileStruct* CDcaLegacyGraph::graphFileStart(const CString& name,FileTypeTag sourceCadFileType)
{
   trace("Entering %-50.50s - ",__FUNCTION__);

   //block_level = 0;
   m_blockArray.RemoveAll();

	//File = doc->Add_File(name, source_cad);
   m_currentFile = getCamCadData().getFileList().addNewFile(name,sourceCadFileType);

   m_currentFile->setBlockType(blockTypePcb);
   m_currentFile->getBlock()->setBlockType(m_currentFile->getBlockType());

   BlockStruct* block = m_currentFile->getBlock();

   m_currentDataList = &(block->getDataList());

   if (m_blockArray.GetSize() < MaxBlockLevel)
   {
      m_blockArray.Add(m_currentFile->getBlock()->getBlockNumber());
   }
   else
   {
      ErrorMessage("Too many Block nestings\n");
      return NULL;
   }

   trace("Exiting  %-50.50s - ",__FUNCTION__);

	return m_currentFile;
}

//BlockStruct *Graph_Block_On(int method, const char *block_name, int filenum, DbFlag flg, BlockTypeTag blockType) // declares and begins defining a block
BlockStruct* CDcaLegacyGraph::graphBlockOn(GraphBlockOnMethodTag method,const CString& blockName,int fileNumber,DbFlag flags,BlockTypeTag blockType)
{
   trace("Entering %-50.50s - ",__FUNCTION__);

   // filenumber -1 for global or 1... for assigned to file
   // 0 is not legal !!!
   if (fileNumber == 0)
   {
#ifdef _DEBUG
      ErrorMessage("DEBUG: Wrong filenumber in CDcaLegacyGraph::graphBlockOn");
#endif
      // Message
      fileNumber = -1;
   }

   //BlockStruct *block = Graph_Block_Exists(doc, blockName, fileNumber, blockType);
   BlockStruct* block = getCamCadData().getBlockDirectory().getAt(blockName,fileNumber);

   if (block != NULL)
   {
      switch (method)
      {
      case graphBlockOnMethodReturn:
         return NULL; 

      case graphBlockOnMethodOverwrite:
         {
            // remove old datas                 
            //for (POSITION pos = block->getDataList().GetHeadPosition();pos != NULL;)
            //{
            //   DataStruct *data = block->getDataList().GetNext(pos);
            //   RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
            //}

            //block->getDataList().RemoveAll();

            getCamCadData().discardData(block->getDataList());
         }

         break;
      case graphBlockOnMethodAppend:
         break;
      }
   }
   else
   {
      //block = doc->Add_Blockname(blockName, fileNumber, flg, FALSE, blockType); 

      if (blockType < blockTypeUnknown)
      {
         blockType = blockTypeUnknown;
      }

      block = &(getCamCadData().getNewBlock(blockName,fileNumber,blockType));
      block->setFlags(flags);
   }

   m_currentDataList = &(block->getDataList());

   if (m_blockArray.GetSize() < MaxBlockLevel)
   {
      m_blockArray.Add(block->getBlockNumber());
   }
   else
   {
      ErrorMessage("Too many Block nestings\n");
      return NULL;
   }

   trace("Exiting  %-50.50s - ",__FUNCTION__);

   return block;
}

void CDcaLegacyGraph::graphBlockOff()
{
   trace("Entering %-50.50s - ",__FUNCTION__);

   if (m_blockArray.GetSize() > 0)
   {
      m_blockArray.RemoveAt(m_blockArray.GetSize() - 1);
   }

   if (m_blockArray.GetSize() > 0)
   {
      BlockStruct* block = getCamCadData().getBlock(m_blockArray.GetAt(m_blockArray.GetSize() - 1));
      m_currentDataList = &(block->getDataList());
   }
   else
   {
      m_currentDataList = NULL; 
   }

   trace("Exiting  %-50.50s - ",__FUNCTION__);
}

BlockStruct* CDcaLegacyGraph::graphBlockExists(const CString& blockName,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct* block = NULL;

   for (int blockIndex = 0;blockIndex < getCamCadData().getMaxBlockIndex();blockIndex++)
   {
      block = getCamCadData().getBlock(blockIndex);

      if (block != NULL)
      {
         // if filenumber is -1, look only for name.
         if (block->getName().Compare(blockName) == 0 && (fileNumber == -1 || block->getFileNumber() == fileNumber) ) // if block is already declared
         {
			   if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
            {
	            break;
            }
         }

         block = NULL;
      }
   }

   return block;
}

////////////////////////////////////////////////////////////////////////////////////
/* Graph_Block_Reference 
*     - Adds an insert to a geometry
*     - global parameter only used if block is not found and declaring a new block

*     PARAMETERS :
*     - global parameter only used if block not found so declaring a new block
*     - filenum :
*        0        : find first block with this name
*        -1       : only find global blocks
*        filenum  : a block in this file OR a global block (whichever it finds first)
*
*    The inser_layer is as follows:
*    if entities are marked as floating layers, take the insert_layer as the layer to plot on.
*
*/

// If a block is placed on "NO LAYER" (layer index  -1 ) = only legal for Block_Ref. 
// Layerindex -1 means leave geom_block entities on their respective layers.

// be placed on a floating layer (preferred Graph_Level("0","",1))
// floating layer is which gets overwritten by a preceeding insert layer.

//DataStruct* CCEtoODBDoc::Graph_Block_Reference(const char *block_name, const char *refname, int filenum, double x, double y,
//      double angle, int mirror, double scale, int layer, int global, BlockTypeTag blockType)

DataStruct* CDcaLegacyGraph::graphBlockReference(const CString& blockName,const char* refName,int fileNumber,double x,double y,
      double angleRadians,bool mirror,double scale,int layerIndex,bool global,BlockTypeTag blockType)
{
   if (blockType < blockTypeUnknown)
   {
      blockType = blockTypeUnknown;
   }

   //BlockStruct& referencedBlock = getCamCadData().getDefinedBlock(blockName,blockType,fileNumber);
   int subBlockNumber = getBlockNum(blockName,fileNumber,global);
   BlockStruct& referencedBlock = *(getCamCadData().getBlock(subBlockNumber));

   traceAppendFormat(" - fileNumber = %d, subBlockName = '%s', subBlockNumber = %d",fileNumber,blockName,referencedBlock.getBlockNumber());
   trace("Entering %-50.50s - ",__FUNCTION__);

   DataStruct* data = getCamCadData().getNewDataStruct(dataTypeInsert);
   m_currentDataList->AddTail(data);

   // If layer is -1 the block elements are placed on the layers which they are
   // defined on. If layer > -1, all entities on floating layers are put on that layer.
   data->setLayerIndex(layerIndex);

   data->getInsert()->getShadingRef().On = FALSE;
   data->getInsert()->setOriginX(x);
   data->getInsert()->setOriginY(y);

   data->getInsert()->setAngle(angleRadians);

   data->getInsert()->setMirrorFlags((mirror ? (MIRROR_FLIP | MIRROR_LAYERS) : 0)); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   data->getInsert()->setPlacedBottom(mirror); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   data->getInsert()->setScale(scale);
   data->getInsert()->setBlockNumber(referencedBlock.getBlockNumber()); 
   data->getInsert()->setInsertType(insertTypeUnknown);

   data->getInsert()->setRefname(refName);

   // inherit block's attributes
   if (referencedBlock.getAttributesRef() != NULL)
   {
      WORD keyword;
      CAttribute* b_ptr;
      CAttribute* i_ptr;
      CAttributeMapWrap dataAttribMap(data->getAttributesRef());

      for (POSITION pos = referencedBlock.attributes().GetStartPosition();pos != NULL;)
      {
         referencedBlock.attributes().GetNextAssoc(pos, keyword,b_ptr);

         i_ptr = b_ptr->allocateCopy();
         i_ptr->setInherited(true); // set the inherited flag

         dataAttribMap.addInstance(keyword,i_ptr);
      }
   }

   return data;
}

void CDcaLegacyGraph::graphBlockCopy(BlockStruct* block,double x,double y,double angle,int mirror,double scale,int layerIndex,int copyAttributes,int hideData)
{
   trace("Entering %-50.50s - ",__FUNCTION__);

   if (block != NULL && m_blockArray.GetSize() > 0)
   {
      if (copyAttributes)
      {
         //BlockStruct *curb = doc->Find_Block_by_Num(blockarray[block_level]);
         BlockStruct* currentBlock = getCamCadData().getBlock(m_blockArray.GetAt(m_blockArray.GetSize() - 1));

         if (currentBlock != NULL && block->getAttributesRef() != NULL)
         {
            //doc->CopyAttribs(&curb->getAttributesRef(), block->getAttributesRef());
            currentBlock->attributes().CopyAll(block->attributes());
         }
      }

      for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct* data = block->getDataList().GetNext(dataPos);
         DataStruct* copy = copyTransposeEntity(data,x,y,angle,mirror,scale,layerIndex,true);

         if (hideData)
         {
            copy->setHidden(true);
         }

         m_currentDataList->AddTail(copy);
      }
   }

   trace("Exiting  %-50.50s - ",__FUNCTION__);
}

DataStruct* CDcaLegacyGraph::copyTransposeEntity(DataStruct* data,double x,double y,double angle,int mirror,double scale,int layerIndex,BOOL CopyAttribs,int entityNumber)
{
   DataStruct* newData = getCamCadData().getNewDataStruct(*data,CopyAttribs != 0,entityNumber);

   CTMatrix matrix;

   matrix.scale(mirror ? -scale : scale,scale);
   matrix.rotateRadians(angle);
   matrix.translate(x,y);

   newData->transform(matrix,&(getCamCadData()));

   if (getCamCadData().isFloatingLayer(newData->getLayerIndex()) && layerIndex != -1)
   {
      newData->setLayerIndex(layerIndex);
   }

   if (newData->getDataType() == dataTypeInsert)
   {
      if (getCamCadData().isFloatingLayer(data->getLayerIndex()) && layerIndex != -1)
      {
         newData->setLayerIndex(layerIndex);
      }
   }

   return newData;
}

int CDcaLegacyGraph::graphLevel(const char* layerName,const char* prefix,bool floatingFlag)
{
   CString fullLayerName;

   if (prefix != NULL)
   {
      fullLayerName = prefix;
   }

   fullLayerName += layerName;
   LayerStruct& layer = getCamCadData().getDefinedLayer(layerName,floatingFlag,layerTypeUndefined);

   return layer.getLayerIndex();   
}

DataStruct* CDcaLegacyGraph::graphPolyStruct(int layerIndex,DbFlag flags,bool negative,int entityNumber)
{
   trace("Entering %-50.50s - ",__FUNCTION__);

   if (layerIndex < 0)
   {
      layerIndex = getCamCadData().getDefinedLayerIndex("NegativeNumericLayer");
   }

   DataStruct* data = getCamCadData().getNewDataStruct(dataTypePoly,entityNumber);

   m_currentDataList->AddTail(data);

   data->setLayerIndex(layerIndex);
   data->setFlags(flags);
   data->setNegative(negative);

   m_currentPolyList = data->getPolyList();

   return data;
}

CPoly* CDcaLegacyGraph::graphPoly(DataStruct* polyStruct,int widthIndex,BOOL filledFlag,BOOL voidFlag,BOOL closedFlag)
{
   if (widthIndex < 0)
   {
      widthIndex = getCamCadData().getZeroWidthIndex();
   }

   if (polyStruct != NULL)
   {
      m_currentPolyList = polyStruct->getPolyList();
   }

   CPoly* poly = new CPoly();

   poly->setWidthIndex(widthIndex);
   poly->setFilled(filledFlag);
   poly->setVoid(voidFlag);
   poly->setClosed(closedFlag || filledFlag);

   m_currentPolyList->AddTail(poly);

   m_currentPoly = poly;

   return poly;
}

CPnt* CDcaLegacyGraph::graphVertex(double x, double y, double bulge)
{
   // 572 is a bulge for 89.9 * 4 = 359.6
   // after this number, the bulge does into infinity.
   // infinity 1*e16 is for 90 degree * 4 = a full circle
   if (fabs(bulge) > 572.)
   {
      bulge = 0.;  
   }

   CPnt* pnt = new CPnt(x,y,bulge);

   m_currentPoly->getPntList().AddTail(pnt);

   return pnt;
}

DataStruct* CDcaLegacyGraph::graphCircle(int layerIndex,double xCenter,double yCenter,double radius,DbFlag flags,int widthIndex,BOOL negative,BOOL filled)
{
   if (widthIndex < 0)
   {
      widthIndex = getCamCadData().getZeroWidthIndex();
   }

   DataStruct* data = getCamCadData().getNewDataStruct(dataTypePoly);
   m_currentDataList->AddTail(data);

   data->setLayerIndex(layerIndex);
   data->setFlags(flags);
   data->setNegative(negative);

   CPoly* poly = new CPoly;

   poly->setWidthIndex(widthIndex);
   poly->setClosed(true);
   poly->setFilled(filled);

   data->getPolyList()->AddTail(poly);

   CPnt* pnt = new CPnt;
   pnt->x = (DbUnit)xCenter;
   pnt->y = (DbUnit)(yCenter + radius);
   pnt->bulge = (DbUnit)1.0;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)xCenter;
   pnt->y = (DbUnit)(yCenter - radius);
   pnt->bulge = (DbUnit)1.0;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)xCenter;
   pnt->y = (DbUnit)(yCenter + radius);
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);

   return data;
}

DataStruct* CDcaLegacyGraph::graphArc(int layerIndex,double xCenter,double yCenter,double radius,
   double startAngleRadians,double deltaAngleRadians,DbFlag flags,int widthIndex,BOOL negative)
{
   if (widthIndex < 0)
   {
      widthIndex = getCamCadData().getZeroWidthIndex();
   }

   double bulge = tan(deltaAngleRadians/4);
   double    x1 = xCenter + radius * cos(startAngleRadians);
   double    y1 = yCenter + radius * sin(startAngleRadians);
   double    x2 = xCenter + radius * cos(startAngleRadians + deltaAngleRadians);
   double    y2 = yCenter + radius * sin(startAngleRadians + deltaAngleRadians);

   DataStruct* data = getCamCadData().getNewDataStruct(dataTypePoly);
   m_currentDataList->AddTail(data);

   data->setLayerIndex(layerIndex);
   data->setFlags(flags);
   data->setNegative(negative);

   CPoly *poly = new CPoly;

   poly->setWidthIndex(widthIndex);

   data->getPolyList()->AddTail(poly);

   CPnt* pnt = new CPnt;
   pnt->x = (DbUnit)x1;
   pnt->y = (DbUnit)y1;
   pnt->bulge = (DbUnit)bulge;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)x2;
   pnt->y = (DbUnit)y2;
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);

   return data;
}

////////////////////////////////////////////////////////////////////////////////////
/*
   Specialchar is
   1. if a textstring contains a %, a % is added to make UNDERLINE and OVERLINE
   work.
   2. if specialchar is TRUE, do not do it because it is done in the input


*/
DataStruct* CDcaLegacyGraph::graphText(int layerIndex, const char* text, double x, double y, 
   double height, double charwidth, double angleRadians, DbFlag flags,char proportional, char mirror, int oblique, BOOL negative, 
   int penWidthIndex, int specialChar, int textAlignment, int lineAlignment)
{
   CString  t;
   const char* lp = text;

   if (layerIndex < 0)
   {
      layerIndex = getCamCadData().getDefinedLayerIndex("NegativeNumericLayer");
   }

   // no text
   if (strlen(text) == 0)  return NULL;

   if (!specialChar)
   {
      unsigned int   i;

      for (i=0;i < strlen(text);i++)
      {
         if (text[i] == '%')
         {
            t += '%';
         }

         t += text[i];
      }

      lp = t.GetBuffer(0);
   }

   DataStruct* data = getCamCadData().getNewDataStruct(dataTypeText);

   m_currentDataList->AddTail(data);

   data->setLayerIndex(layerIndex);
   data->setFlags(flags);
   data->setNegative(negative);

   // if no widhtindex supplied (-1) take 1/10 of the height as the penwidth
   if (penWidthIndex < 0)
   {
      penWidthIndex = getCamCadData().getDefinedWidthIndex(.1 * height);
   }

   data->getText()->setPenWidthIndex(penWidthIndex);

   data->getText()->setText(lp);
   data->getText()->setPnt(x,y);
   data->getText()->setHeight(height);
   data->getText()->setWidth(charwidth);    // character width
   data->getText()->setProportionalSpacing(proportional);
   data->getText()->setRotation(angleRadians);        // angle in Radians
   data->getText()->setMirrored(mirror);
   data->getText()->setOblique(oblique);
   data->getText()->setFontNumber(0);
   data->getText()->setMirrorDisabled(false);
   data->getText()->setVerticalPosition(intToVerticalPositionTag(lineAlignment));
   data->getText()->setHorizontalPosition(intToHorizontalPositionTag(textAlignment));

   return data;
}

int CDcaLegacyGraph::graphComplex(int fileNumber, const CString& name, int dcode, const CString& subBlockName,
   double xoffset, double yoffset, double rotation, BlockTypeTag blockType)
{
   BlockStruct* block;

   DbFlag flag = BL_APERTURE;

   if (fileNumber == -1)
   {
      flag |= BL_GLOBAL;
   }

   for (int loopWidthIndex = 0;loopWidthIndex < getCamCadData().getWidthTable().GetSize();loopWidthIndex++)
   {
      block = getCamCadData().getWidthTable().getAt(loopWidthIndex);

      if (block != NULL)
      {
         if (block->getName().Compare(name) == 0 && block->getFileNumber() == fileNumber)
         {
            if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
            {
               int subBlockNumber = getBlockNum(subBlockName,fileNumber,(flag & BL_GLOBAL));

               block->setShape(apertureComplex);
               block->setComplexApertureSubBlockNumber(subBlockNumber);
               block->setDcode(dcode);
               block->setFlags(flag);

               return loopWidthIndex;
            }
         } // end if same name
      }
   } // end loop through widthtable

   // a match does not exist
   // create an aperture and return its index
   int subBlockNumber = getBlockNum(subBlockName,fileNumber,(flag & BL_GLOBAL));

   if (blockType < blockTypeUnknown)
   {
      blockType = blockTypeUnknown;
   }

   block = &(getCamCadData().getBlockDirectory().getNewBlock(name,fileNumber,blockType));

   block->setShape(apertureComplex);
   block->setComplexApertureSubBlockNumber(subBlockNumber);
   block->setDcode(dcode);
   block->setFlags(flag);
   block->setXoffset(xoffset);
   block->setYoffset(yoffset);
   block->setRotation(rotation);

   int widthIndex = getCamCadData().getWidthTable().Add(block);

   return widthIndex;
}

/*******************************************************************************
* Get_Block_Num
*     DECSRIPTION :
*     - Finds a block using Block Name and by matching block's filenum if block is not global
*     - If not found, declares Block and assigns it a unique Block Number
*
*     RETURNS :
*     - the Unique Block Number of the block.
*
*     PARAMETERS :
*     - global parameter only used if block not found so declaring a new block
*     - filenum :
*        0        : find first block with this name
*        -1       : only find global blocks
*        filenum  : a block in this file OR a global block (whichever it finds first)
*/
int CDcaLegacyGraph::getBlockNum(const CString& blockName,int fileNumber,int global,BlockTypeTag blockType)
{
   BlockStruct* block;

   // BL_Global are blocks which are not unique to a file, but to the Document
   for (int blockIndex=0;blockIndex < getCamCadData().getMaxBlockIndex();blockIndex++)
   {
      block = getCamCadData().getBlock(blockIndex);

      if (block == NULL)
         continue;

      // if name is the same and (either filenum=0
      if (block->getName().Compare(blockName) == 0)
      {
         if (fileNumber == 0 || block->getFileNumber() == fileNumber || (block->getFlags() & BL_GLOBAL))
         {
            if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
            {
               return block->getBlockNumber();
            }
         }
      }
   }

   // if not in list of declared blocks, declare now and define it later
   DbFlag flg = 0;

   if (global) flg |= BL_GLOBAL;

   if (blockType < blockTypeUnknown)
   {
      blockType = blockTypeUnknown;
   }

   block = &(getCamCadData().getDefinedBlock(blockName,blockType,fileNumber));

   block->setFlags(flg);

   return block->getBlockNumber();
}

void CDcaLegacyGraph::setTraceWriteFormat(CWriteFormat* traceWriteFormat)
{
   m_traceWriteFormat = traceWriteFormat;
}

void CDcaLegacyGraph::trace(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString buf;

   buf.FormatV(format,args);

   for (int index = 0;index < m_blockArray.GetSize();index++)
   {
      int blockNumber = m_blockArray.GetAt(index);
      BlockStruct* block = getCamCadData().getBlock(blockNumber);

      buf.AppendFormat("|[%d] %s:%d",index,block->getName(),block->getBlockNumber());
   }

   buf.AppendFormat("|%s\n",m_graphDebugText);

   if (m_traceWriteFormat != NULL)
   {
      m_traceWriteFormat->write(buf);
   }

   m_graphDebugText.Empty();
}

void CDcaLegacyGraph::traceAppendFormat(const char* format,...)
{
   va_list args;
   va_start(args,format);

   m_graphDebugText.AppendFormatV(format,args);
}


























