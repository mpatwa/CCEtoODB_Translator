
/*****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"        
#include "math.h"       
#include "dbutil.h"
#include "CCdoc.h"
#include "Graph.h"
#include "attrib.h"
#include "polylib.h"
#include "pcbutil.h"
#include "Attribute.h"
#include "EntityNumber.h"
#include "TMState.h"

extern BOOL Editing;                   // from SELECT.CPP

void ShowNothingDlg();

static   CCEtoODBDoc* doc=NULL;
static   CDataList      *CurrentDataList;

int      NextFileNum    = 1;

CPolyList      *currentPolyList;
static CPoly   *currentPoly;

// check if in a block
static   int         block_level = 0;
static   FileStruct  *File = NULL;     // first init until a Graph_File_Start is done

static   CWriteFormat* s_graphTraceWriteFormat = NULL;
static   CString s_graphDebugText;

#define MAX_BLOCKNESTING  10
static int blockarray[MAX_BLOCKNESTING+1];

HBITMAP LoadJPEG(LPCTSTR lpszPathName);

CCEtoODBDoc* GraphGetDoc()
{
   return doc;
}

/* StoreDocForImporting */
CCEtoODBDoc* CCEtoODBDoc::StoreDocForImporting()
{
   CCEtoODBDoc* oldDoc = doc;

   if (doc != this)
   {
      block_level = 0;
   }

   doc = this;

   return oldDoc;
}

/* PrepareAddEntity */
void CCEtoODBDoc::PrepareAddEntity(FileStruct *file)
{
   doc = this;
   File = file;
   CurrentDataList = &(file->getBlock()->getDataList());
   block_level = 1;
   blockarray[block_level] = file->getBlock()->getBlockNumber();
   file->getBlock()->resetExtent();
}

// GetCurrentDataList()
CDataList *GetCurrentDataList()
{
   return CurrentDataList;
}

// SetCurrentDataList()
void SetCurrentDataList(CDataList *dataList)
{
   CurrentDataList = dataList;
}

/******************************************************************************
* RemoveOneEntityFromDataList
*/
bool RemoveOneEntityFromDataList(CCEtoODBDoc *doc, CDataList *dataList, DataStruct *data, POSITION dataPos)
{
   bool retval = false;

   if (doc != NULL && dataList != NULL)
   {
      retval = doc->removeDataFromDataList(*dataList,data,dataPos);
   }

   return retval;
}

// if moved between files, need to update    data->filenum
int MoveEntityToAnotherDataList(DataStruct *data, CDataList *oldList, CDataList *newList)
{
   POSITION pos = oldList->Find(data);
   if (!pos)
      return 0;
   oldList->RemoveAt(pos);
   newList->AddTail(data);
   return 1;
}

/* FILE ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////////
FileStruct *Graph_File_Start(const char *name, int source_cad)
{
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

   block_level = 0;

	File = doc->Add_File(name, source_cad);

	{ // set graph block for legacy graph calls (excerpt from Graph_Block_On)
		CurrentDataList = &(File->getBlock()->getDataList());
		block_level++;

		if (block_level < MAX_BLOCKNESTING)
		{
			blockarray[block_level] = File->getBlock()->getBlockNumber();
		}
		else
		{
			ErrorMessage("Too many Block nestings\n");
			return NULL;
		}
	}

// graphTrace("Exiting  %-40.40s - ",__FUNCTION__);

	return File;
}

////////////////////////////////////////////////////////////////////////////////////
void Graph_File_Reference(FileStruct *file, double insert_x, double insert_y,
      double angle, int mirror, double scale)
{
   file->setInsertX((DbUnit)insert_x);
   file->setInsertY((DbUnit)insert_y);
   file->setRotation((DbUnit)angle);
   file->setMirrored(mirror);
   file->setScale((DbUnit)scale);
}

/* BLOCK **************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
BlockStruct* Graph_Block_Exists(CCEtoODBDoc* camCadDocument, const char* block_name, int filenum, BlockTypeTag blockType, bool compareNoCase)
{
   for (int i=0;i < camCadDocument->getMaxBlockIndex();i++)
   {
      BlockStruct* block = camCadDocument->getBlockAt(i);

      if (block == NULL)
      {
         continue;
      }

      // if filenumber is -1, look only for name.
      if ((!block->getName().Compare(block_name) || ( compareNoCase && !block->getName().CompareNoCase(block_name))) && (filenum == -1 || block->getFileNumber() == filenum) ) // if block is already declared
      {
			if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
         {
	         return block;
         }
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
/* Graph_Block_On
*     - method :
*        - GBO_APPEND
*        - GBO_OVERWRITE
*        - GBO_RETURN
*/
BlockStruct *Graph_Block_On(int method, const char *block_name, int filenum, DbFlag flg, BlockTypeTag blockType) // declares and begins defining a block
{
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

   // filenumber -1 for global or 1... for assigned to file
   // 0 is not legal !!!
   if (filenum == 0)
   {
#ifdef _DEBUG
      ErrorMessage("DEBUG: Wrong filenumber in Graph_Block_On");
#endif
      // Message
      filenum = -1;
   }

   BlockStruct *block = Graph_Block_Exists(doc, block_name, filenum, blockType);

   if (block != NULL)
   {
      switch (method)
      {
      case GBO_RETURN:
         return NULL; 

      case GBO_OVERWRITE:
         {
            // remove old datas                 
            for (POSITION pos = block->getDataList().GetHeadPosition();pos != NULL;)
            {
               DataStruct *data = block->getDataList().GetNext(pos);
               RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
            }

            block->getDataList().RemoveAll();
         }

         break;
      case GBO_APPEND:
         break;
      }
   }
   else
   {
      block = doc->Add_Blockname(block_name, filenum, flg, FALSE, blockType); 
   }

   CurrentDataList = &(block->getDataList());
   block_level++;

   if (block_level < MAX_BLOCKNESTING)
   {
      blockarray[block_level] = block->getBlockNumber();
   }
   else
   {
      ErrorMessage("Too many Block nestings\n");
      return NULL;
   }

// graphTrace("Exiting  %-40.40s - ",__FUNCTION__);

   return block;
}

BlockStruct* Graph_Block_On(BlockStruct* block) 
{
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

   if (block != NULL)
   {
      CurrentDataList = &(block->getDataList());
      block_level++;

      if (block_level < MAX_BLOCKNESTING)
      {
         blockarray[block_level] = block->getBlockNumber();
      }
      else
      {
         ErrorMessage("Too many Block nestings\n");
         return NULL;
      }
   }

// graphTrace("Exiting  %-40.40s - ",__FUNCTION__);

   return block;
}

// Declare Block ///////////////////////////////////////////////////////////////////
//    - DOES NOT check to see if a block by the same name exists.
BlockStruct *Declare_Block(const char *block_name, int filenum, DbFlag flg, BOOL Aperture, BlockTypeTag blockType)
{ 
   return doc->Add_Blockname(block_name, filenum, flg, Aperture, blockType); 
}

////////////////////////////////////////////////////////////////////////////////////
void Graph_Block_Off()
{
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

   block_level--;

   if (!block_level)
   {
      CurrentDataList = NULL; 
      return;
   }
   else if (block_level < 0)
   {
      ErrorMessage("Big block_level Problem\n");
      return;
   }

   BlockStruct *block = doc->getBlockAt(blockarray[block_level]);
   CurrentDataList = &(block->getDataList());

// graphTrace("Exiting  %-40.40s - ",__FUNCTION__);

/* for (int i=0; i<doc->maxBlockArray; i++)
   {
      BlockStruct *block = doc->BlockArray[i];
      if (block == NULL)   continue;
      if (block->num == blockarray[block_level])
      {
         CurrentDataList = &(block->getDataList());
         return;
      }
   }
*/
}

/******************************************************************************
* AddEntity()
*/
DataStruct *AddEntity(DataStruct *data)
{
   CurrentDataList->AddTail(data);
   //data->setEntityNumber(CEntityNumber::allocate());

   return data;
}

/******************************************************************************
* CopyEntity()
*/
//DataStruct *CopyEntity(DataStruct *data, BOOL CopyAttribs)
//{
//   DataStruct* newData = new DataStruct;
//
//   memcpy(newData, data, sizeof(DataStruct));
//   newData->getAttributesRef() = NULL;
//
//   if (CopyAttribs)
//      doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributesRef());
//
//   newData->setEntityNumber(CEntityNumber::allocate());
//
//   switch (newData->type)
//   {
//      case T_POLY:
//      {
//         newData->getPolyList() = new CPolyList;
//
//         POSITION polyPos = data->getPolyList()->GetHeadPosition();
//         while (polyPos)
//         {
//            CPoly *poly = data->getPolyList()->GetNext(polyPos);
//
//            CPoly *newPoly = new CPoly;
//            memcpy(newPoly, poly, sizeof(CPoly) - sizeof(CPntList));
//
//            newData->getPolyList()->AddTail(newPoly);
//
//            POSITION pntPos = poly->getPntList().GetHeadPosition();
//            while (pntPos != NULL)
//            {
//               CPnt *pnt = poly->getPntList().GetNext(pntPos);
//
//               CPnt *newPnt = new CPnt;
//               memcpy(newPnt, pnt, sizeof(CPnt));
//
//               newPoly->list.AddTail(newPnt);
//            }
//         }
//      }
//      break;
//
//      case T_TEXT:
//      {
//         newData->getText() = new TextStruct;
//         memcpy(newData->getText(), data->getText(), sizeof(TextStruct));
//         newData->getText()->setText(STRDUP(data->getText()->getText()));
//      }
//      break;
//
//      case T_INSERT:
//      {
//         newData->getInsert() = new InsertStruct;
//         memcpy(newData->getInsert(), data->getInsert(), sizeof(InsertStruct));
//         if (data->getInsert()->getRefname())
//            newData->getInsert()->setRefname(STRDUP(data->getInsert()->getRefname()));
//      }
//      break;
//   }
//
//   return newData;
//}

/******************************************************************************
* CopyTransposeEntity()
*/
DataStruct *CopyTransposeEntity(DataStruct *data, double x, double y, double angle, int mirror, double scale, int layer, BOOL CopyAttribs,int entityNumber)
{
   DataStruct* newData = doc->getCamCadData().getNewDataStruct(*data,CopyAttribs != 0,entityNumber);

   CTMatrix matrix;

   matrix.scale(mirror ? -scale : scale,scale);
   matrix.rotateRadians(angle);
   matrix.translate(x,y);

   newData->transform(matrix, &(doc->getCamCadData()));

   if (doc->IsFloatingLayer(newData->getLayerIndex()) && layer != -1)
      newData->setLayerIndex(layer);

   if (newData->getDataType() == dataTypeInsert)
   {
      if (doc->IsFloatingLayer(data->getLayerIndex()) && layer != -1)
         newData->setLayerIndex(layer);
   }

   return newData;

   //DataStruct *newData = new DataStruct;

   //memcpy(newData, data, sizeof(DataStruct));
   //newData->getAttributesRef() = NULL;

   //if (CopyAttribs)
   //   doc->CopyAttribs(&newData->getAttributesRef(), data->getAttributesRef());

   //newData->setEntityNumber(CEntityNumber::allocate());

   //if (doc->IsFloatingLayer(newData->layer) && layer != -1)
   //   newData->layer = layer;
   //
   //Mat2x2 m;
   //RotMat2(&m, angle);

   //// transpose data by insert data
   //switch (newData->type)
   //{
   //   case T_POLY:
   //   {
   //      POSITION polyPos, pntPos;
   //      CPoly *poly, *newPoly;
   //      CPnt *pnt, *newPnt;
   //      Point2 v;
   //      newData->getPolyList() = new CPolyList;

   //      polyPos = data->getPolyList()->GetHeadPosition();
   //      while (polyPos != NULL)
   //      {
   //         poly = data->getPolyList()->GetNext(polyPos);
   //         newPoly = new CPoly;
   //         memcpy(newPoly, poly, sizeof(CPoly) - sizeof(CPntList));
   //         newData->getPolyList()->AddTail(newPoly);
   //         if (fabs(scale - 1.0) > SMALLNUMBER)
   //         {
   //            BlockStruct *widthBlock = doc->widthTable[poly->getWidthIndex()];
   //            newPoly->widthIndex = Graph_Aperture("", widthBlock->shape, widthBlock->getSizeA() * scale, widthBlock->getSizeB() * scale,
   //                  widthBlock->xoffset * scale, widthBlock->yoffset * scale, widthBlock->rotation, 0, 
   //                  widthBlock->flag, FALSE, NULL);
   //         }

   //         pntPos = poly->getPntList().GetHeadPosition();
   //         while (pntPos != NULL)
   //         {
   //            pnt = poly->getPntList().GetNext(pntPos);
   //            v.x = pnt->x * scale;
   //            if (mirror) v.x = -v.x;
   //            v.y = pnt->y * scale;
   //            TransPoint2(&v, 1, &m, x, y);

   //            newPnt = new CPnt;
   //            newPnt->x = (DbUnit)v.x;
   //            newPnt->y = (DbUnit)v.y;
   //            if (mirror)
   //               newPnt->bulge = (DbUnit)(-pnt->bulge);
   //            else
   //               newPnt->bulge = (DbUnit)(pnt->bulge);

   //            newPoly->list.AddTail(newPnt);
   //         }
   //      }
   //   }
   //   break;
   //   case T_TEXT:
   //   {
   //      newData->getText() = new TextStruct;
   //      memcpy(newData->getText(), data->getText(), sizeof(TextStruct));
   //      newData->getText()->setText(STRDUP(data->getText()->getText()));
   //
   //      Point2 pnt;
   //      pnt.x = data->getText()->pnt.x * scale;
   //      if (mirror) pnt.x = -pnt.x;
   //      pnt.y = data->getText()->pnt.y * scale;
   //      TransPoint2(&pnt, 1, &m, x, y);
   //   
   //      newData->getText()->pnt.x = (DbUnit)pnt.x;
   //      newData->getText()->pnt.y = (DbUnit)pnt.y;
   //   
   //      newData->getText()->height *= (DbUnit)scale;
   //      newData->getText()->width *= (DbUnit)scale;
   //      newData->getText()->mirror = data->getText()->mirror ^ mirror;
   //      newData->getText()->never_mirror = data->getText()->never_mirror;
   //      if (mirror)
   //         newData->getText()->angle = (DbUnit)(angle - data->getText()->angle);
   //      else
   //         newData->getText()->angle = (DbUnit)(angle + data->getText()->angle);
   //   }
   //   break;
   //   case T_INSERT:
   //   {
   //      newData->getInsert() = new InsertStruct;
   //      memcpy(newData->getInsert(), data->getInsert(), sizeof(InsertStruct));
   //      if (data->getInsert()->getRefname())
   //         newData->getInsert()->setRefname(STRDUP(data->getInsert()->getRefname()));

   //      Point2 pnt;
   //      pnt.x = data->getInsert()->getOriginX() * scale;
   //      if (mirror) pnt.x = -pnt.x;
   //      pnt.y = data->getInsert()->getOriginY() * scale;
   //      TransPoint2(&pnt, 1, &m, x, y);
   //   
   //      newData->getInsert()->getOriginX() = (DbUnit)pnt.x;
   //      newData->getInsert()->getOriginY() = (DbUnit)pnt.y;
   //   
   //      newData->getInsert()->getScale() *= (DbUnit)scale;
   //      if (mirror)
   //         newData->getInsert()->getAngle() = (DbUnit)(angle - data->getInsert()->getAngle());
   //      else
   //         newData->getInsert()->getAngle() = (DbUnit)(angle + data->getInsert()->getAngle());
   //      newData->getInsert()->getMirrorFlags() = data->getInsert()->getMirrorFlags() ^ mirror;
   //   
   //      if (!(!doc->IsFloatingLayer(data->getLayerIndex()) || layer == -1))
   //         newData->layer = layer;
   //
   //   }
   //   break;
   //}
   //return newData;
}

////////////////////////////////////////////////////////////////////////////////////
void Graph_Block_Copy(BlockStruct *block, double x, double y,
      double angle, int mirror, double scale, int layer, int copy_attributes, int hideData)
{
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

   if (block == NULL)
      return;

   if (copy_attributes)
   {
      BlockStruct *curb = doc->Find_Block_by_Num(blockarray[block_level]);
      if (block)
         doc->CopyAttribs(&curb->getAttributesRef(), block->getAttributesRef());
   }

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      DataStruct *copy = CopyTransposeEntity(data, x, y, angle, mirror, scale, layer, TRUE);
      if (hideData)
         copy->setHidden(true);
      CurrentDataList->AddTail(copy);
   }

// graphTrace("Exiting  %-40.40s - ",__FUNCTION__);
}

   
/* APERTURE / WIDTH ***************************************************************/
////////////////////////////////////////////////////////////////////////////////////

int CCEtoODBDoc::Graph_Complex(int filenum, const char *name, int dcode, const char *subBlockName,
                              double xoffset, double yoffset, double rotation, BlockTypeTag blockType)
{
   int i;
   BlockStruct *block;

   DbFlag flag = BL_APERTURE;
   if (filenum == -1)
      flag |= BL_GLOBAL;

   for (i = 0; i < this->getNextWidthIndex(); i++)
   {
      block = this->getWidthTable()[i];
      if (block != NULL)
      {
         if (block->getName().Compare(name) == 0 && block->getFileNumber() == filenum)
         {
            if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
            {
               block->setShape(T_COMPLEX);
               block->setSizeA((DbUnit)this->Get_Block_Num(subBlockName, filenum, (flag & BL_GLOBAL)));
               block->setDcode(dcode);
               block->setFlags(flag);
               return i;
            }
         } // end if same name
      }
   } // end loop through widthtable

   // a match does not exist
   // create an aperture and return its index
   block = this->Add_Blockname(name, filenum, flag, TRUE, blockType);
   block->setShape(T_COMPLEX);
   block->setSizeA((DbUnit)this->Get_Block_Num(subBlockName, filenum, (flag & BL_GLOBAL)));
   block->setDcode(dcode);
   block->setFlags(flag);

   block->setSizeB(0);
   block->setXoffset((DbUnit)xoffset);
   block->setYoffset((DbUnit)yoffset);
   block->setRotation((DbUnit)rotation);
   block->setSizeC(0);
   block->setSizeD(0);
   block->setSpokeCount(0);

   //i = this->getNextWidthIndex()++;
   //this->getWidthTable().SetAtGrow(i, block);

   int widthIndex = this->getWidthTable().Add(block);

   return widthIndex;
}

//-----------------------------------------------------------------------

int CCEtoODBDoc::Graph_Complex(const char *name, int dcode, BlockStruct *subBlock, 
                              double xoffset, double yoffset, double rotation)
{
   // Only for use when subBlock already exists and we have it in hand

   if (subBlock != NULL)
   {
      int filenum = subBlock->getFileNumber();

      int i;
      BlockStruct *block;

      DbFlag flag = BL_APERTURE;
      if (filenum == -1)
         flag |= BL_GLOBAL;

      for (i = 0; i < this->getNextWidthIndex(); i++)
      {
         block = this->getWidthTable()[i];
         if (block != NULL)
         {
            if (block->getName().Compare(name) == 0 && block->getFileNumber() == filenum)
            {
               //if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
               {
                  block->setShape(T_COMPLEX);
                  block->setSizeA((DbUnit) subBlock->getBlockNumber() );
                  block->setDcode(dcode);
                  block->setFlags(flag);
                  return i;
               }
            } // end if same name
         }
      } // end loop through widthtable

      // a match does not exist
      // create an aperture and return its index
      block = this->Add_Blockname(name, filenum, flag, TRUE, blockTypeUnknown);
      block->setShape(T_COMPLEX);
      block->setSizeA((DbUnit) subBlock->getBlockNumber() );
      block->setDcode(dcode);
      block->setFlags(flag);

      block->setSizeB(0);
      block->setXoffset((DbUnit)xoffset);
      block->setYoffset((DbUnit)yoffset);
      block->setRotation((DbUnit)rotation);
      block->setSizeC(0);
      block->setSizeD(0);
      block->setSpokeCount(0);

      //i = this->getNextWidthIndex()++;
      //this->getWidthTable().SetAtGrow(i, block);

      int widthIndex = this->getWidthTable().Add(block);

      return widthIndex;
   }

   return 0;
}

//------------------------------------------------------------------------------

int CCEtoODBDoc::Graph_Complex(const char *name, int dcode, const char *subBlockName,
                              double xoffset, double yoffset, double rotation, BlockTypeTag blockType)
{
   // This function exists for backward compatibility with historical camcad that assumed and
   // operates as if all apertures are global.

   return Graph_Complex(-1 /*global filenum*/,  name, dcode, subBlockName, xoffset, yoffset, rotation, blockType);
}

//------------------------------------------------------------------------------

int Graph_Complex(const char *name, int dcode, const char *subBlockName,
                  double xoffset, double yoffset, double rotation, BlockTypeTag blockType)
{
   return doc->Graph_Complex(name,dcode,subBlockName,xoffset,yoffset,rotation,blockType);
}

//------------------------------------------------------------------------------

int Graph_Complex(int filenum, const char *name, int dcode, const char *subBlockName,
                  double xoffset, double yoffset, double rotation, BlockTypeTag blockType)
{
   return doc->Graph_Complex(filenum, name,dcode,subBlockName,xoffset,yoffset,rotation,blockType);
}

//------------------------------------------------------------------------------

int Graph_Complex(const char *name, int dcode, BlockStruct *subBlock, 
                              double xoffset, double yoffset, double rotation)
{
   return doc->Graph_Complex(name, dcode, subBlock, xoffset, yoffset, rotation);
}

/******************************************************************************
* Graph_Aperture
*/
int CCEtoODBDoc::Graph_Aperture(const char *name, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation,
      int dcode, DbFlag flag, int overWriteValues, int *overWriteError)
{
   return Graph_Aperture(-1, name, shape, sizeA, sizeB,
      xoffset, yoffset, rotation,
      dcode, flag, overWriteValues, overWriteError);
}

/******************************************************************************
* Graph_Aperture
*/
int CCEtoODBDoc::Graph_Aperture(int filenum, const char *name, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation,
      int dcode, DbFlag flag, int overWriteValues, int *overWriteError)
{
   int i;
   BlockStruct *block;

#ifdef _DEBUG
   if (sizeA < 0) 
   {
      ErrorMessage("Wrong sizeA in Graph_Aperture", "DEBUG ERROR");
   }

   if (sizeB < 0)
   {
      ErrorMessage("Wrong sizeB in Graph_Aperture", "DEBUG ERROR");
   }
#endif

   if (sizeA < 0) 
   {
      sizeA = 0;
   }

   if (sizeB < 0) 
   {
      sizeB = 0;
   }

   if (overWriteError)
   {
      *overWriteError = FALSE;  // the overwrite error is flags, if the flag
                               // is set not to overwrite, but the values are
                               // different. It still works like the old one, returning
                               // the index to the array.
   }

   if (shape == T_ROUND || shape == T_SQUARE)
   {
      sizeB = (DbUnit)0.0;
   }

   //case 2158, changed this
   if (filenum < 0)
      flag |= BL_APERTURE | BL_GLOBAL | BL_WIDTH; // apertures and widths are global
   else
      flag |= BL_APERTURE | BL_WIDTH; // apertures and widths are not global

   // an aperture only loops through the widthindex, this means it is possible to create
   // a block and a aperture with the same name !!! And this is really bad !
   for (i = 0; i < this->getNextWidthIndex(); i++)
   {
      block = this->getWidthTable()[i];

      if (block == NULL)   continue;
      
      if (strlen(name) == 0) // widths pass "" for name
      {
         // do not care what dcode this item will be exported as
         if ((block->getFlags() & BL_WIDTH) && // item must not be an aperture
               block->getShape() == shape &&
               fabs(block->getSizeA()    - sizeA   ) < SMALLNUMBER &&
               fabs(block->getSizeB()    - sizeB   ) < SMALLNUMBER &&
               fabs(block->getXoffset()  - xoffset ) < SMALLNUMBER &&
               fabs(block->getYoffset()  - yoffset ) < SMALLNUMBER &&
               fabs(block->getRotation() - rotation) < SMALLANGLE)
         {
            return i;
         }
      } // end if name == ""
      else // search for this name
      {
         if (block->getName().Compare(name) == 0 &&
            (block->getFileNumber() == filenum) )
         {     
            // here check if the values are the same.
            if (block->getShape() != shape ||
                fabs(block->getSizeA()    - sizeA   ) > SMALLNUMBER ||
                fabs(block->getSizeB()    - sizeB   ) > SMALLNUMBER ||
                fabs(block->getXoffset()  - xoffset ) > SMALLNUMBER ||
                fabs(block->getYoffset()  - yoffset ) > SMALLNUMBER ||
                fabs(block->getRotation() - rotation) > SMALLANGLE)
            {  //  same aperture, but different values.
               if (overWriteError)
               {
                  *overWriteError = TRUE;
               }
            }

            if (overWriteValues)
            {

               block->setShape(shape);
               block->setSizeA((DbUnit)sizeA);
               block->setSizeB((DbUnit)sizeB);
               block->setXoffset((DbUnit)xoffset);
               block->setYoffset((DbUnit)yoffset);
               block->setRotation((DbUnit)rotation);
               block->setSizeC((DbUnit)0);
               block->setSizeD((DbUnit)0);
               block->setSpokeCount(0);
               block->setFlags(flag);
            } // end if overwrite

            return i;
         } // end if same name
      } // end else compare name
   } // end loop through widthtable

   // a match does not exist
   // create an aperture and return its index
   block = this->Add_Blockname(name, filenum, flag, TRUE);

   block->setShape(shape);
   block->setSizeA((DbUnit)sizeA);
   block->setSizeB((DbUnit)sizeB);
   block->setXoffset((DbUnit)xoffset);
   block->setYoffset((DbUnit)yoffset);
   block->setRotation((DbUnit)rotation);
   block->setSizeC((DbUnit)0);
   block->setSizeD((DbUnit)0);
   block->setSpokeCount(0);
   block->setDcode(dcode);
   block->setFlags(flag);

   //i = this->getNextWidthIndex()++;
   //this->getWidthTable().SetAtGrow(i, block);

   int widthIndex = this->getWidthTable().Add(block);

   return widthIndex;
}

int Graph_Aperture(const char *name, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation,
      int dcode, DbFlag flag, int overWriteValues, int *overWriteError)
{
   return doc->Graph_Aperture(name,shape,sizeA,sizeB,xoffset,yoffset,rotation,dcode,flag,overWriteValues,overWriteError);
}

// Case 2158
int Graph_Aperture(int filenum, const char *name, int shape, double sizeA, double sizeB,
      double xoffset, double yoffset, double rotation,
      int dcode, DbFlag flag, int overWriteValues, int *overWriteError)
{
   return doc->Graph_Aperture(filenum, name,shape,sizeA,sizeB,xoffset,yoffset,rotation,dcode,flag,overWriteValues,overWriteError);
}

/******************************************************************************
* Graph_FindAperture
   Find an aperture with same shape and size without care about name
   If found return pointer or create one.

   named = must find a named aperture
   width_allowed = allows the return of a width.
*/
BlockStruct *Graph_FindAperture(int shape, double sizeA, double sizeB, double xoffset, double yoffset, double rotation,
      DbFlag flag, int named, int width_allowed)
{
   if (shape == T_ROUND || shape == T_SQUARE)
      sizeB = (DbUnit)0.0;

   flag |= BL_APERTURE | BL_GLOBAL; // apertures and widths are global

   for (int i=0; i<doc->getNextWidthIndex(); i++)
   {
      BlockStruct *block = doc->getWidthTable()[i];

      if (block == NULL)
         continue;

      if (named && !strlen(block->getName()))
         continue;

      if (!width_allowed && (block->getFlags() & BL_WIDTH))
         continue;

      // do not care what dcode this item will be exported as
      if ( block->getShape() == shape &&
           fabs(block->getSizeA() - sizeA) < SMALLNUMBER &&
           fabs(block->getSizeB() - sizeB) < SMALLNUMBER &&
           fabs(block->getXoffset() - xoffset) < SMALLNUMBER &&
           fabs(block->getYoffset() - yoffset) < SMALLNUMBER &&
           fabs(normalizeRadians(block->getRotation() - rotation)) < 0.001)
         return block;
   } // end loop through widthtable

   // a match does not exist
   // create an aperture and return its index
   CString name;
   name.Format("APERTURE %d", doc->getNextWidthIndex()+1);
   BlockStruct *block = doc->Add_Blockname(name, -1, flag, TRUE);
   block->setShape(shape);
   block->setSizeA((DbUnit)sizeA);
   block->setSizeB((DbUnit)sizeB);
   block->setXoffset((DbUnit)xoffset);
   block->setYoffset((DbUnit)yoffset);
   block->setRotation((DbUnit)rotation);
   block->setSizeC((DbUnit)0);
   block->setSizeD((DbUnit)0);
   block->setSpokeCount(0);

   block->setDcode(0);
   block->setFlags(flag);
   //i = doc->getNextWidthIndex()++;
   //doc->getWidthTable().SetAtGrow(i, block);

   int widthIndex = doc->getWidthTable().Add(block);

   return block;
}

//-----------------------------------------------------------------------------------
// Similar to original above, but different.
// This version applies file association.

BlockStruct *Graph_FindAperture(int filenum, int shape, double sizeA, double sizeB, double xoffset, double yoffset, double rotation,
      bool mustBeNamed, bool widthAllowed)
{
   if (shape == T_ROUND || shape == T_SQUARE)
      sizeB = (DbUnit)0.0;

   for (int i=0; i<doc->getNextWidthIndex(); i++)
   {
      BlockStruct *block = doc->getWidthTable()[i];

      if (block != NULL)
      {
         if (mustBeNamed && block->getName().IsEmpty())
            continue;

         if (!widthAllowed && (block->getFlags() & BL_WIDTH))
            continue;

         // Is candidate if file numbers match, or is global and unnamed
         if (block->getFileNumber() == filenum || (block->getFileNumber() == -1 && block->getName().IsEmpty()))
         {
            // do not care what dcode this item will be exported as
            if ( block->getShape() == shape &&
               fabs(block->getSizeA() - sizeA) < SMALLNUMBER &&
               fabs(block->getSizeB() - sizeB) < SMALLNUMBER &&
               fabs(block->getXoffset() - xoffset) < SMALLNUMBER &&
               fabs(block->getYoffset() - yoffset) < SMALLNUMBER &&
               fabs(block->getRotation() - rotation) < SMALLNUMBER)
               return block;
         }
      }
   } // end loop through widthtable


   // A match does not exist.
   // Create a named aperture and return its index

   DbFlag flag = BL_APERTURE;
   if (filenum == -1)
      flag |= BL_GLOBAL;

   CString name;
   name.Format("APERTURE %d", doc->getNextWidthIndex()+1);

   BlockStruct *block = doc->Add_Blockname(name, filenum, flag, TRUE);
   block->setShape(shape);
   block->setSizeA((DbUnit)sizeA);
   block->setSizeB((DbUnit)sizeB);
   block->setXoffset((DbUnit)xoffset);
   block->setYoffset((DbUnit)yoffset);
   block->setRotation((DbUnit)rotation);
   block->setSizeC((DbUnit)0);
   block->setSizeD((DbUnit)0);
   block->setSpokeCount(0);

   block->setDcode(0);
   block->setFlags(flag);
   //i = doc->getNextWidthIndex()++;
   //doc->getWidthTable().SetAtGrow(i, block);

   int widthIndex = doc->getWidthTable().Add(block);

   return block;
}

/*****************************************************************************/
/*
   Find an tool with same shape and size without care about name
   If found return pointer or create one.

   named = must find a named aperture
*/
BlockStruct *Graph_FindTool(double size, DbFlag flag, int named,bool platedFlag)
{
   int i;
   BlockStruct *block;

   flag |= BL_TOOL | BL_GLOBAL; // tools are global

   for (i = 0; i < doc->getNextWidthIndex(); i++)
   {
      block = doc->getWidthTable()[i];
      if (block == NULL)                  continue;
      if (block->getBlockType() != BLOCKTYPE_DRILLHOLE) continue;
      if (block->getToolHolePlated() != platedFlag) continue;

      if (named && !strlen(block->getName()))  continue;

      // do not care what dcode this item will be exported as
      if (fabs(block->getToolSize() - size) < SMALLNUMBER)
      {
         return block;
      }
   } // end loop through widthtable

   // a match does not exist
   // create an aperture and return its index
   CString name;
   name.Format("DRILL %d",doc->getNextWidthIndex()+1);
   block = doc->Add_Blockname(name, -1, flag, TRUE);
   block->setBlockType(BLOCKTYPE_DRILLHOLE);
   block->setTcode(0);
   block->setToolSize((DbUnit)size);
   block->setToolType(0);
   block->setToolBlockNumber(0);
   block->setToolDisplay(false);
   block->setDcode(0);
   block->setFlags(flag);
   block->setToolHolePlated(platedFlag);

   //i = doc->getNextWidthIndex()++;
   //doc->getWidthTable().SetAtGrow(i, block);

   int widthIndex = doc->getWidthTable().Add(block);

   return block;
}

/* TOOL ***************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
//PARAMETERS:
//    name     -
//    toolnum  -
//    size     -
//    type     - 
//    geomnum  - block num of drill symbol graphic
//    display  - display a graphic instead of generic tool drawing
//    flag
//
//  - if a tool with this name exists, overwrites size if size > 0, geomnum if > 0
int Graph_Tool(const char *name, int tcode, double size, int type, int geomnum, 
                        BOOL display, DbFlag flag, BOOL plated,BOOL punched)
{
   int i;
   BlockStruct *block;

   flag |= BL_TOOL | BL_GLOBAL; // tools are global

   for (i = 0; i < doc->getNextWidthIndex(); i++)
   {
      if (!(block = doc->getWidthTable()[i])) continue;

      if (!(block->getFlags() & BL_TOOL)) continue;

      if (!block->getName().Compare(name))
      {     
         if (size)
            block->setToolSize((DbUnit)size);
         if (geomnum)
            block->setToolBlockNumber(geomnum);
         block->setToolDisplay(display);
         block->setFlags(flag);
         return i;
      } // end if same name
   } // end loop through widthtable

   // a match does not exist
   // create a tool and return its index
   block = doc->Add_Blockname(name, -1, flag, TRUE);
   block->setBlockType(BLOCKTYPE_DRILLHOLE);
   block->setTcode(tcode);
   block->setToolSize((DbUnit)size);
   block->setToolType(type);
   block->setToolBlockNumber(geomnum);
   block->setToolDisplay(display);
   block->setFlags(flag);
   block->setToolHolePlated(plated);
   block->setToolHolePunched(punched);
   // we need to make sure that we also can NC-ROUTE (Draw) with a tool
   // must be in the width index. The Tool List works of this 
   //i = doc->getNextWidthIndex()++;
   //doc->getWidthTable().SetAtGrow(i, block);

   int widthIndex = doc->getWidthTable().Add(block);

   return widthIndex;
}

/* COMPLEX DRILL TOOL ***************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
//PARAMETERS:
//    name     -
//    toolnum  -
//    size     -
//    type     - 
//    geomnum  - block num of drill symbol graphic
//    display  - display a graphic instead of generic tool drawing
//    flag
//
//  - if a Complex Drill tool with this name exists, Then don't add 
int Graph_ComplexTool(const char *name, int tcode, int type, int geomnum, 
                        BOOL display, DbFlag flag, BOOL plated, BOOL punched,
                        int Shape, double width , double height)
{
   int i;
   BlockStruct *block;

   flag |= BL_COMPLEX_TOOL;

   for (i = 0; i < doc->getNextWidthIndex(); i++)
   {
      if (!(block = doc->getWidthTable()[i])) continue;

      if (!(block->getFlags() & BL_COMPLEX_TOOL)) continue;

      if (!block->getName().Compare(name))
      {  
         return i;
      } // end if same name
   } // end loop through widthtable

   // a match does not exist
   // create a tool and return its index
   block = doc->Add_Blockname(name, -1, flag, TRUE);
   block->setBlockType(BLOCKTYPE_COMPLEXDRILLHOLE);
   block->setTcode(tcode);
   block->setToolType(type);
   block->setToolBlockNumber(geomnum);
   block->setToolDisplay(display);
   block->setFlags(flag);
   block->setToolHolePlated(plated);
   block->setToolHolePunched(punched);

   Graph_Block_On(block);

   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);

   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   if(Shape == T_SQUARE)
      height = width;
   Graph_Vertex(0,0,0.);
   Graph_Vertex(0,height,0.);
   Graph_Vertex(width,height,0.);
   Graph_Vertex(width,0,0.);
   Graph_Vertex(0,0,0.); 

   Graph_Block_Off();

   int widthIndex = doc->getWidthTable().Add(block);
   return widthIndex;
}

/* LAYER **************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
int CCEtoODBDoc::Graph_Level(const char *layerName, const char *prefix, BOOL floating)
{
   CString fullLayerName = "";

   if (prefix)
   {
      fullLayerName = prefix;
   }

   fullLayerName += layerName;
   LayerStruct *layer = this->Add_Layer(fullLayerName);

   if (floating) 
   {
      layer->setFloating(true);
   }

   return layer->getLayerIndex();   
}

int Graph_Level(const char *layerName, const char *prefix, BOOL floating)
{
   return doc->Graph_Level(layerName,prefix,floating);
}

////////////////////////////////////////////////////////////////////////////////////
void CCEtoODBDoc::Graph_Level_Mirror(const char* layerName1,const char* layerName2,const char* prefix)
{
   CString prefixString;

   if (prefix != NULL)
   {
      prefixString = prefix;
   }

   CString fullLayerName1(prefixString + layerName1);
   CString fullLayerName2(prefixString + layerName2);

   LayerStruct *layer1, *layer2;

   // guarantee layers are in database
   layer1 = this->Add_Layer(fullLayerName1);  
   layer2 = this->Add_Layer(fullLayerName2); 

   if (layer1->getMirroredLayerIndex() != layer2->getLayerIndex() ||
       layer2->getMirroredLayerIndex() != layer1->getLayerIndex()    )
   {
      // protect against illegal combinations
      this->getLayerArray().unmirrorLayer(*layer1);
      this->getLayerArray().unmirrorLayer(*layer2);

      //// if a layer to mirror had a different mirror layer, fix the different mirror layer to itself
      //int layerIndex;

      //if (layer1->getMirroredLayerIndex() != layer1->getLayerIndex())
      //{
      //   layerIndex = layer1->getMirroredLayerIndex();
      //   this->LayerArray[layerIndex]->setMirroredLayerIndex(layerIndex);
      //}

      //if (layer2->getMirroredLayerIndex() != layer2->getLayerIndex())
      //{
      //   layerIndex = layer2->getMirroredLayerIndex();
      //   this->LayerArray[layerIndex]->setMirroredLayerIndex(layerIndex);
      //}
      
      // set layer mirrors
      layer1->setMirroredLayerIndex(layer2->getLayerIndex());
      layer2->setMirroredLayerIndex(layer1->getLayerIndex());
   }
}

void Graph_Level_Mirror(const char* layerName1,const char* layerName2,const char* prefix)
{
   doc->Graph_Level_Mirror(layerName1,layerName2,prefix);
}

////////////////////////////////////////////////////////////////////////////////////
LayerGroupStruct *Graph_LayerGroup(const char *name)
{
   LayerGroupStruct *group;
   group = doc->FindLayerGroup(name);
   if (!group)
      group = doc->AddLayerGroup(name);
   return group;
}

////////////////////////////////////////////////////////////////////////////////////
void Graph_LayerInLayerGroup(LayerGroupStruct *group, LayerStruct *layer)
{
   // check that it's not already in there
   POSITION layerPos = group->LayerList.GetHeadPosition();
   while (layerPos != NULL)
   {
      LayerStruct *temp = group->LayerList.GetNext(layerPos);
      if (temp == layer)   
         return; 
   }

   // add in Database
   group->LayerList.AddTail(layer);
}

////////////////////////////////////////////////////////////////////////////////////
void Graph_GroupInLayerGroup(LayerGroupStruct *group, LayerGroupStruct *subgroup)
{
   // check that it's not already in there
   POSITION groupPos = group->GroupList.GetHeadPosition();
   while (groupPos != NULL)
   {
      LayerGroupStruct *temp = group->GroupList.GetNext(groupPos);
      if (temp == subgroup)   
         return; 
   }

   // add in Database
   group->GroupList.AddTail(subgroup);
}


/* DATA ***************************************************************************/
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

DataStruct* CCEtoODBDoc::Graph_Block_Reference(const char *block_name, const char *refname, int filenum, double x, double y,
      double angle, int mirror, double scale, int layer, int global, BlockTypeTag blockType)
{
   int subBlockNumber = this->Get_Block_Num(block_name, filenum, global, blockType);
   return Graph_Block_Reference(subBlockNumber, refname, x, y, angle, mirror, scale, layer);
}

DataStruct* CCEtoODBDoc::Graph_Block_Reference(int subBlockNumber, const char *refname, double x, double y,
      double angle, int mirror, double scale, int layer)
{
// graphAppendFormat(" - fileNumber = %d, subBlockName = '%s', subBlockNumber = %d",filenum,block_name,subBlockNumber);
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

   DataStruct *data = getCamCadData().getNewDataStruct(dataTypeInsert);
   CurrentDataList->AddTail(data);

   // If layer is -1 the block elements are placed on the layers which they are
   // defined on. If layer > -1, all entities on floating layers are put on that layer.
   data->setLayerIndex(layer);

   //data->getInsert() = new InsertStruct;
   data->getInsert()->getShadingRef().On = FALSE;
   data->getInsert()->setOriginX(x);
   data->getInsert()->setOriginY(y);

   data->getInsert()->setAngle(angle);

   data->getInsert()->setMirrorFlags((mirror ? (MIRROR_FLIP | MIRROR_LAYERS) : 0)); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   data->getInsert()->setPlacedBottom(mirror); // normal mirroring does both. If you want different for an insert, change ->mirror after call to Graph_Block_Ref().  see DBUTIL.H
   data->getInsert()->setScale(scale);
   data->getInsert()->setBlockNumber(subBlockNumber); 
   data->getInsert()->setInsertType(insertTypeUnknown);

   if (refname)
      data->getInsert()->setRefname(STRDUP(refname));
   else 
      data->getInsert()->setRefname(NULL);

   // inherit block's attributes
   BlockStruct *b = this->Find_Block_by_Num(data->getInsert()->getBlockNumber());

   if (b->getAttributesRef() != NULL)
   {
      WORD keyword;
      Attrib* b_ptr;
      Attrib* i_ptr;
      CAttributeMapWrap dataAttribMap(data->getAttributesRef());

      for (POSITION pos = b->getAttributesRef()->GetStartPosition();pos != NULL;)
      {
         b->getAttributesRef()->GetNextAssoc(pos, keyword,b_ptr);

         i_ptr = b_ptr->allocateCopy();
         i_ptr->setInherited(true); // set the inherited flag

         dataAttribMap.addInstance(keyword,i_ptr);
      }
   }

   return data;
}

//------------------------------------------

DataStruct *Graph_Block_Reference(const char *block_name, const char *refname, int filenum, double x, double y,
      double angle, int mirror, double scale, int layer, int global, BlockTypeTag blockType)
{
   return doc->Graph_Block_Reference(block_name,refname,filenum,x,y,angle,mirror,scale,layer,global,blockType);
}

//------------------------------------------

DataStruct *Graph_Block_Reference(int blocknumber, const char *refname, double x, double y,
      double angle, int mirror, double scale, int layer)
{
   return doc->Graph_Block_Reference(blocknumber,refname,x,y,angle,mirror,scale,layer);
}

//------------------------------------------

/******************************************************************************
* RefreshTypetoCompAttributes
*/
void RefreshTypetoCompAttributes(CCEtoODBDoc *doc, int mode)
{
   POSITION    attribPos;
   DataStruct  *data;
   WORD        keyword;
   Attrib      *b_ptr;

   for (POSITION pos = doc->getFileList().GetHeadPosition();pos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);

      if (!file->isShown())
         continue;

      if (file->getBlockType() != BLOCKTYPE_PCB)  
         continue;

      // loop through datalist to find inserts
      for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (!data->getAttributesRef())
            continue;

         Attrib *a =  is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1);

         if (a == NULL)
            continue;

         CString t = get_attvalue_string(doc, a);

         if (strlen(t) == 0)
            continue;

         TypeStruct *type = AddType(file, t);
   
         if (type->getAttributesRef())
         {
            // loop through block's attributes
            for (attribPos = type->getAttributesRef()->GetStartPosition();attribPos != NULL;)
            {
               type->getAttributesRef()->GetNextAssoc(attribPos, keyword, b_ptr);

               CString val = get_attvalue_string(doc, b_ptr);
               doc->SetUnknownAttrib(&data->getAttributesRef(), doc->getKeyWordArray()[keyword]->cc, val, mode, NULL);
            }
         }
      }
   }
   return;
}

/******************************************************************************
* RefreshInheritedAttributes
*/
void RefreshInheritedAttributes(CCEtoODBDoc *doc, int mode, BOOL overwriteNotInherited)
{
   POSITION dataPos, attribPos;
   DataStruct *data;
   WORD keyword;
   Attrib* attrib;
   Attrib *b_ptr, *i_ptr;

   // loop through blocklist
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      // loop through datalist to find inserts
      dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         data = block->getDataList().GetNext(dataPos);
         if (data->getDataType() != T_INSERT)
            continue;

         BlockStruct *insertBlock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber()); // get inserted block
         if (insertBlock == NULL)
            continue;

         if (insertBlock->getAttributesRef())
         {
            // loop through block's attributes
            attribPos = insertBlock->getAttributesRef()->GetStartPosition();
            while (attribPos != NULL)
            {
               insertBlock->getAttributesRef()->GetNextAssoc(attribPos, keyword, b_ptr);

               if (!data->getAttributesRef())
                  data->getAttributesRef() = new CAttributes();

               // if attribute does not exist on the insert or the mode is to overwrite
               attrib = NULL;

               if (!data->getAttributesRef()->Lookup(keyword, attrib) || mode == SA_OVERWRITE) 
               {
                  i_ptr = b_ptr->allocateCopy();
                  i_ptr->setInherited(true);      // set the inherited flag

                  // If there is an attribute, then remove it
                  if (attrib)
                  {
                     if (!attrib->isInherited() && !overwriteNotInherited)
                        continue; // Skip if the attrib is not inherited from the geometry & overwriteNotInherited is FALSE
                     else
                        RemoveAttrib(keyword, &data->getAttributesRef());
                  }

                  // If the attribute map is empty after deleting the last one, then create it again
                  if (data->getAttributesRef() == NULL)
                     data->getAttributesRef() = new CAttributes();

                  // Insert the new attribute
                  data->getAttributesRef()->SetAt(keyword, i_ptr);
               }
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////////
DataStruct *Graph_Line(int layer, double x1, double y1, double x2, double y2, 
      DbFlag flg, int widthIndex, BOOL negative)
{

#ifdef _DEBUG
   if (layer < 0) 
      ErrorMessage("Wrong layernumber in Graph_Line");

   if (widthIndex < -1)
      ErrorMessage("Wrong widthIndex in Graph_PolyStruct");
#endif

   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypePoly);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;

   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypePoly);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);

   //data->getPolyList() = new CPolyList;

   CPoly *poly = new CPoly;
   poly->setWidthIndex(widthIndex);
   poly->setHatchLine(false);
   poly->setFilled(false);
   poly->setVoid(false);
   poly->setClosed(false);
   poly->setHidden(false); 
   poly->setFloodBoundary(false);
   poly->setBreakOut(false);
   poly->setThermalLine(false);
   data->getPolyList()->AddTail(poly);

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)x1;
   pnt->y = (DbUnit)y1;
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)x2;
   pnt->y = (DbUnit)y2;
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);

   return data;
}

////////////////////////////////////////////////////////////////////////////////////
DataStruct *Graph_PolyStruct(int layer, DbFlag flg, BOOL negative,int entityNumber)
{
// graphTrace("Entering %-40.40s - ",__FUNCTION__);

#ifdef _DEBUG
      if (layer < 0)
         ErrorMessage("Wrong layernumber in Graph_PolyStruct");
#endif

   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypePoly,entityNumber);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;
   //data->setEntityNumber(CEntityNumber::allocate());

   //data->setDataType(dataTypePoly);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);

   currentPolyList = data->getPolyList();
   return data;
}

CPoly *Graph_Poly(DataStruct *polyStruct, int widthIndex, BOOL Filled, BOOL VoidPoly, BOOL Closed)
{
#ifdef _DEBUG
   if (widthIndex < 0)
   {
      ErrorMessage("Wrong widthIndex in Graph_PolyStruct");
      widthIndex = 0;
   }
#endif

   if (polyStruct != NULL)
      currentPolyList = polyStruct->getPolyList();
   CPoly *poly = new CPoly;
   poly->setWidthIndex(widthIndex);
   poly->setFilled(Filled);
   poly->setVoid(VoidPoly);
   poly->setClosed(Closed || Filled);
   poly->setThermalLine(false);
   poly->setFloodBoundary(false);
   poly->setHidden(0);
   poly->setHatchLine(false);
   currentPolyList->AddTail(poly);
   currentPoly = poly;

   return poly;
}

CPoly *Graph_Poly_Arc(double center_x, double center_y, 
      double radius, double startangle, double deltaangle)
{
   double   x1,y1,x2,y2,bulge;
   bulge = tan(deltaangle/4);
   x1 = center_x + radius * cos(startangle);
   y1 = center_y + radius * sin(startangle);
   x2 = center_x + radius * cos(startangle+deltaangle);
   y2 = center_y + radius * sin(startangle+deltaangle);

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)x1;
   pnt->y = (DbUnit)y1;
   pnt->bulge = (DbUnit)bulge;
   currentPoly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)x2;
   pnt->y = (DbUnit)y2;
   pnt->bulge = (DbUnit)0.0;
   currentPoly->getPntList().AddTail(pnt);

   return currentPoly;
}

CPoly *Graph_Poly_Circle(double center_x, double center_y, double radius)
{
   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)center_x;
   pnt->y = (DbUnit)(center_y + radius);
   pnt->bulge = (DbUnit)1.0;
   currentPoly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)center_x;
   pnt->y = (DbUnit)(center_y - radius);
   pnt->bulge = (DbUnit)1.0;
   currentPoly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)center_x;
   pnt->y = (DbUnit)(center_y + radius);
   pnt->bulge = (DbUnit)0.0;
   currentPoly->getPntList().AddTail(pnt);

   return currentPoly;
}

CPnt *Graph_Vertex(double x, double y, double bulge)
{
   // 572 is a bulge for 89.9 * 4 = 359.6
   // after this number, the bulge does into infinity.
   // infinity 1*e16 is for 90 degree * 4 = a full circle
   if (fabs(bulge) > 572)
   {
      bulge = 0;  
#ifdef _DEBUG
      ErrorMessage("Bulge Error in Graph_Vertex");
#endif
   }

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)x;
   pnt->y = (DbUnit)y;
   pnt->bulge = (DbUnit)bulge;
   currentPoly->getPntList().AddTail(pnt);
   return pnt;
}

////////////////////////////////////////////////////////////////////////////////////
DataStruct *Graph_Circle(int layer, double center_x, double center_y, 
      double radius, DbFlag flg, int widthIndex, BOOL negative, BOOL filled)
{

#ifdef _DEBUG
      if (layer < 0) 
         ErrorMessage("Wrong layernumber in Graph_Circle");
      if (widthIndex < 0)
      {
         ErrorMessage("Wrong widthIndex in Graph_PolyStruct");
         widthIndex = 0;
      }
#endif
      

   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypePoly);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;

   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypePoly);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);

   //data->getPolyList() = new CPolyList;

   CPoly *poly = new CPoly;
   poly->setWidthIndex(widthIndex);
   poly->setHatchLine(false);
   poly->setHidden(false); 
   poly->setThermalLine(false);
   poly->setFilled(false);
   poly->setVoid(false);
   poly->setFloodBoundary(false);
   poly->setClosed(true);
   poly->setFilled(filled);

   data->getPolyList()->AddTail(poly);

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)center_x;
   pnt->y = (DbUnit)(center_y + radius);
   pnt->bulge = (DbUnit)1.0;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)center_x;
   pnt->y = (DbUnit)(center_y - radius);
   pnt->bulge = (DbUnit)1.0;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)center_x;
   pnt->y = (DbUnit)(center_y + radius);
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);

   return data;
}

////////////////////////////////////////////////////////////////////////////////////
DataStruct *Graph_Arc(int layer, double center_x, double center_y, 
      double radius, double startangle, double deltaangle,
      DbFlag flg, int widthIndex, BOOL negative)
{

#ifdef _DEBUG
      if (layer < 0) 
         ErrorMessage("Wrong layernumber in Graph_Arc");
      if (widthIndex < 0)
      {
         ErrorMessage("Wrong widthIndex in Graph_PolyStruct");
         widthIndex = 0;
      }
#endif

   double   x1,y1,x2,y2,bulge;
   bulge = tan(deltaangle/4);
   x1 = center_x + radius * cos(startangle);
   y1 = center_y + radius * sin(startangle);
   x2 = center_x + radius * cos(startangle+deltaangle);
   y2 = center_y + radius * sin(startangle+deltaangle);

   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypePoly);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;

   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypePoly);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);
   //data->getPolyList() = new CPolyList;

   CPoly *poly = new CPoly;
   poly->setWidthIndex(widthIndex);
   poly->setHatchLine(false);
   poly->setHidden(false); 
   poly->setThermalLine(false);
   poly->setFilled(false);
   poly->setVoid(false);
   poly->setClosed(false);
   poly->setFloodBoundary(false);

   data->getPolyList()->AddTail(poly);

   CPnt *pnt = new CPnt;
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
DataStruct *Graph_Point(double x, double y, int layer, DbFlag flg, BOOL negative)
{

#ifdef _DEBUG
      if (layer < 0) ErrorMessage("Wrong layernumber in Graph_Point");
#endif

   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypePoint);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;
   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypePoint);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);

   //data->getPoint() = new PointStruct;
   data->getPoint()->x = (DbUnit)x;
   data->getPoint()->y = (DbUnit)y;

   return data;
}

////////////////////////////////////////////////////////////////////////////////////
int Graph_Attach_Blob(DataStruct *data, const char *filename)
{
   HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   if (!hBitmap)
      hBitmap = LoadJPEG(filename);
   if (!hBitmap)
   {
      ErrorMessage(filename, "Bitmap Load Failed");
      return FALSE;
   }

   data->getBlob()->setBitmap(new CBitmap);
   data->getBlob()->getBitmap()->Attach(hBitmap);

   BITMAP bmpInfo;
   data->getBlob()->getBitmap()->GetBitmap(&bmpInfo);

   data->getBlob()->filename = filename;

   return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
DataStruct *Graph_Blob(double left, double bottom, double width, double height, double rotation, 
      BOOL TopView, BOOL BottomView, CString filename)
{
   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypeBlob);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;
   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypeBlob);
   data->setLayerIndex(-1);
   //data->setFlags(0);
   //data->setNegative(false);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);

   //data->getBlob() = new BlobStruct;

   data->getBlob()->top      = TopView;
   data->getBlob()->bottom   = BottomView;
   data->getBlob()->pnt.x    = (DbUnit)left;
   data->getBlob()->pnt.y    = (DbUnit)bottom;
   data->getBlob()->width    = (DbUnit)width;
   data->getBlob()->height   = (DbUnit)height;
   data->getBlob()->rotation = (DbUnit)rotation;
   data->getBlob()->filename = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

   if (!Graph_Attach_Blob(data, filename))
   {
      ErrorMessage("Could not attach Blob");
   }
   return data;
}

////////////////////////////////////////////////////////////////////////////////////
// if there exists a printable character in the string other than SPACE, return TRUE
static BOOL GoodText(const char *text)
{
   int i = 0;
   if (text == NULL) return FALSE;

   while (text[i] != '\0')
   {
      if (isprint(text[i]) && text[i] != ' ') 
         return TRUE;

      i++;
   }

   return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
/*
   Specialchar is
   1. if a textstring contains a %, a % is added to make UNDERLINE and OVERLINE
   work.
   2. if specialchar is TRUE, do not do it because it is done in the input


*/
DataStruct *Graph_Text(int layer, const char *text, double x, double y, 
      double height, double charwidth, double angle, DbFlag flg,
      char proportional, char mirror, int oblique, BOOL negative, 
      int penWidthIndex, int specialchar, int textAlignment, int lineAlignment)
{
   CString  t;
   const char  *lp = text;

#ifdef _DEBUG
      if (layer < 0) ErrorMessage("Wrong layernumber in Graph_Text");
#endif

   // no text
   if (strlen(text) == 0)  return NULL;

   if (!specialchar)
   {
      unsigned int   i;

      for (i=0;i<strlen(text);i++)
      {
         if (text[i] == '%')
            t += '%';
         t += text[i];
      }
      lp = t.GetBuffer(0);
   }

// if (!GoodText(text)) return NULL;
   int err;

   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypeText);
   CurrentDataList->AddTail(data);
   //data->getAttributesRef() = NULL;
   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypeText);
   data->setLayerIndex(layer);
   data->setFlags(flg);
   data->setNegative(negative);
   //data->setGraphicClass(graphicClassNormal);
   //data->setSelected(false);
   //data->setMarked(false);
   //data->setColorOverride(false);
   //data->setOverrideColor(0);
   //data->setHidden(false);
   //data->getText() = new TextStruct;

   // if no widhtindex supplied (-1) take 1/10 of the height as the penwidth
   if (penWidthIndex < 0)
      data->getText()->setPenWidthIndex(Graph_Aperture("", T_ROUND, 0.1 * height, 0.0, 
            0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE,&err));
   else
      data->getText()->setPenWidthIndex(penWidthIndex);

   data->getText()->setText(STRDUP(lp));
   data->getText()->setPnt((DbUnit)x,(DbUnit)y);
   data->getText()->setHeight(height);
   data->getText()->setWidth(charwidth);    // character width
   data->getText()->setProportionalSpacing(proportional);
   data->getText()->setRotation(angle);        // angle in Radians
   data->getText()->setMirrored(mirror);
   data->getText()->setOblique(oblique);
   data->getText()->setFontNumber(0);
   data->getText()->setMirrorDisabled(false);
   data->getText()->setVerticalPosition(intToVerticalPositionTag(lineAlignment));
   data->getText()->setHorizontalPosition(intToHorizontalPositionTag(textAlignment));

   return data;
}

/* FONT LINE **********************************************************************/
////////////////////////////////////////////////////////////////////////////////////
void Graph_Font_Line(CPolyList *PolyList, double x1, double y1,
      double x2, double y2, DbFlag flg)
{
   CPoly *poly = new CPoly;
   poly->setWidthIndex(0);
   poly->setHatchLine(false);
   poly->setHidden(false);
   poly->setFilled(false);
   poly->setVoid(false);
   poly->setClosed(false);
   poly->setThermalLine(false); 
   poly->setFloodBoundary(false);
   PolyList->AddTail(poly);

   CPnt *pnt = new CPnt;
   pnt->x = (DbUnit)x1;
   pnt->y = (DbUnit)y1;
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);

   pnt = new CPnt;
   pnt->x = (DbUnit)x2;
   pnt->y = (DbUnit)y2;
   pnt->bulge = (DbUnit)0.0;
   poly->getPntList().AddTail(pnt);
}

/******************************************************************************
* Get_ApertureLayer
*  - this gets the layer of an aperture or a block labeled as an aperture.
*/
int Get_ApertureLayer(CCEtoODBDoc *doc, DataStruct *data, BlockStruct *block, int insertLayer)
{
   int layerNum;

   if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
      layerNum = insertLayer;
   else
      layerNum = data->getLayerIndex();

   if (layerNum == -1)
   {
      if (!block->getDataList().IsEmpty()) // if loaded aperture (no data elements in block)
         layerNum = block->getDataList().GetHead()->getLayerIndex(); // assign to layer of first entity
   }

   return layerNum;
}

void setGraphTraceWriteFormat(CWriteFormat* traceWriteFormat)
{
   s_graphTraceWriteFormat = traceWriteFormat;
}

void graphTrace(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString buf;

   buf.FormatV(format,args);

   for (int index = 0;index <= block_level;index++)
   {
      int blockNumber = blockarray[index];
      BlockStruct* block = GraphGetDoc()->getBlockAt(blockNumber);

      buf.AppendFormat("|[%d] %s:%d",index,block->getName(),block->getBlockNumber());
   }

   buf.AppendFormat("|%s\n",s_graphDebugText);

   if (s_graphTraceWriteFormat != NULL)
   {
      s_graphTraceWriteFormat->write(buf);
   }

   s_graphDebugText.Empty();
}

void graphAppendFormat(const char* format,...)
{
   va_list args;
   va_start(args,format);

   s_graphDebugText.AppendFormatV(format,args);

}

// end GRAPH.CPP
