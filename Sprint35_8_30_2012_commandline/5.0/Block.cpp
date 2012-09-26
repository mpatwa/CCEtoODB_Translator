// $Header: /CAMCAD/5.0/Block.cpp 129   6/14/07 1:18p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccview.H"
#include "graph.h"
#include "crypt.h"
#include "Attribute.h"
#include "WriteFormat.h"
#include "Apertur2.h"
#include "CamCadDatabase.h"
#include "Extents.h"
#include <float.h>

BOOL InfiniteRecursion(CCEtoODBDoc *doc);

typedef CArray<int, int> CBlockStackArray;

static CBlockStackArray blocknumarray;

/* Add_Blockname *****************************************************************
*     - Adds a block to the blocklist
*     - Does not check for any conflicting blocks
*     - if (Aperture), tries to extract a dcode from the blockname
*     - returns block ptr
*/                                 
BlockStruct* CCEtoODBDoc::Add_Blockname(const char *name, int filenum, DbFlag flg, BOOL Aperture, BlockTypeTag blockType)
{
   if (filenum == 0)
   {
      filenum = -1;
   }

   if (blockType < blockTypeUnknown)
   {
      blockType = blockTypeUnknown;
   }

   BlockStruct* block = &(getCamCadData().getNewBlock(name,filenum,blockType));
   block->getAttributesRef() = NULL;

   // init variables
   block->setLibraryNumber(0);
   //block->setFileNumber(0);
   block->setMarked(0);

   //if (blockType == blockTypeUndefined)
   //   block->setBlockType(0);
   //else
   //   block->setBlockType(blockType);

   block->setFlags(0L);
   block->resetExtent();
   block->setShape(0);
   block->setDcode(0);
   block->setSizeA(0.);
   block->setSizeB(0.);
   block->setSizeC(0.);
   block->setSizeD(0.);
   block->setXoffset(0.);
   block->setYoffset(0.);
   block->setRotation(0.);
   block->setSpokeCount(4);

   block->setTcode(0);
   block->setToolDisplay(false);
   block->setToolBlockNumber(0);
   block->setToolType(0);
   block->setToolSize(0.0);
   
 //block->setName(name);
   block->setOriginalName(""); // initialized WS 30-dec-98
 //block->setFileNumber(filenum);
   block->setFlags(flg);
   block->setPackage(false);

   // if aperture, set block->ApNum accordingly
   if (Aperture)
   {
      if (strlen(name))
      {
         int i = strlen(name)-1;
         int tmpcnt = 0;
         char  tmp[80];

         while (i && tmpcnt < 80 && isdigit(name[i]))  tmp[tmpcnt++] = name[i--];

         tmp[tmpcnt] = '\0';
         STRREV(tmp);
         block->setDcode(atoi(tmp));
      }
   }

   return block;
}

/*******************************************************************************
* RemoveBlock
*     DECSRIPTION :
*     - Removes Block and Datas from BlockList and WidthTable
*     - Does not check for inserts of this block
*/
void CCEtoODBDoc::RemoveBlock(BlockStruct* block)
{
   // NULL in widthTable
   for (int i = 0; i < getNextWidthIndex(); i++)
   {
      if (getWidthTable()[i] == block)
      {
         getWidthTable().SetAt(i,NULL);

         break;
      }
   }

   FreeBlock(block);

   //for (i=0; i<getMaxBlockIndex(); i++)
   //{
   //   BlockStruct* b = getBlockAt(i);
   //   if (block == b)
   //   {
   //      // call FreeBlock to clean all memory
   //      FreeBlock(b);
   //      //getBlockAt(i) = NULL;
   //      break;
   //   }
   //}

   // check for inserts of this block
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
int CCEtoODBDoc::Get_Block_Num(const char *blockname, int filenum, int global, BlockTypeTag blockType)
{
   BlockStruct* block;

   // BL_Global are blocks which are not unique to a file, but to the Document
   for (int i=0; i< getMaxBlockIndex(); i++)
   {
      block = getBlockAt(i);

      if (block == NULL)
         continue;

      // if name is the same and (either filenum=0
      if (block->getName().Compare(blockname) == 0)
      {
         if (filenum == 0 || block->getFileNumber() == filenum || (block->getFlags() & BL_GLOBAL))
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

   block = Declare_Block(blockname, filenum, flg, FALSE, blockType);

   return block->getBlockNumber();
}

/*******************************************************************************
* Find_Block
*     - Finds a block using Block Name and block's File Number
*     - Returns Pointer to Block or
*        NULL if not found.
*/
BlockStruct* CCEtoODBDoc::Find_Block_by_Name(CString blockname, int filenum, BlockTypeTag blockType)
{
   for (int i=0; i< getMaxBlockIndex(); i++)
   {
      BlockStruct* block = getBlockAt(i);

      if (block == NULL)   continue;

      if (!block->getName().Compare(blockname) && (filenum == -1 || block->getFileNumber() == filenum || block->getFlags() & BL_GLOBAL))
      {
         if (blockType == blockTypeUndefined || block->getBlockType() == blockType)
         {
            return block;
         }
      }
   }

   return NULL;
}

/*******************************************************************************
* Find_Block
*     - Finds a block using Block Number
*     - Returns Pointer to BLock or
*        NULL if not found.
*/
BlockStruct* CCEtoODBDoc::Find_Block_by_Num(int block_num)
{
   return getBlockAt(block_num);
}

/***************************************************************
BOOL Recycle(int blocklevel)

   if in blocknumarray has same element as blocknumarray[blocklevel]
   return TRUE
   else, return FALSE
*****************************************************************/
BOOL Recycle(int blocklevel)
{
   for (int i = 0; i < blocklevel; i++)
   {
      if (blocknumarray[i] == blocknumarray[blocklevel])
      {
         return TRUE;
      }
   }

   return FALSE;
}


/**************************************************
BOOL TestBlock(CCEtoODBDoc *doc,BlockStruct* block,int blocklevel)

   loop block, if has subblock, see if the subblock will has the 
   same block as before  
   if has, return TRUE, else, continue to TestBlock until return TRUE
   or finish the block loop.
***************************************************/
BOOL TestBlock(CCEtoODBDoc *doc,BlockStruct* block,int blocklevel)
{
   BlockStruct* subblock;
   DataStruct* np;
   
   for (POSITION pos = block->getDataList().GetHeadPosition();pos != NULL;)
   {
      np = block->getDataList().GetNext(pos);

      if (np->getDataType() != T_INSERT)
         continue;
      
      subblock = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      if (!subblock)
         continue;

      //if has subblock
      blocknumarray.SetAtGrow(blocklevel,subblock->getBlockNumber());

      if (Recycle(blocklevel))
         return TRUE;

      if (TestBlock(doc,subblock,blocklevel+1))
         return TRUE;
   }

   return FALSE;
}

/***************************************************
BOOL InfiniteRecursion(CCEtoODBDoc *doc)

  loop files, if has block test block, see if the block will recycle or not
  if files has no block, simplely return FALSE
***************************************************/
BOOL InfiniteRecursion(CCEtoODBDoc *doc)
{
   CFileList *FileList = &(doc->getFileList());
   FileStruct *npf;   

   for (POSITION pos = FileList->GetHeadPosition();pos != NULL;) 
   {
      npf = FileList->GetNext(pos);

      if (!npf->isShown() || npf->isHidden() || npf->notPlacedYet())
         continue;

      if (npf->getBlock() == NULL)
         continue;

      //if it is a block
      blocknumarray.SetSize(10,10);
      blocknumarray.SetAtGrow(0,npf->getBlock()->getBlockNumber()); 

      if (TestBlock(doc,npf->getBlock(),1))
      {
         return TRUE;
      }
   }

   return FALSE;
}

// end BLOCK.CPP
