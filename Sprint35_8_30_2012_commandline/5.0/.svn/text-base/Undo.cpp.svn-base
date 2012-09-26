// $Header: /CAMCAD/5.0/Undo.cpp 21    6/30/07 2:39a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

/******************************************************************************
* UNDO
*
*  - Circular Array to store Undos
*  - Redos saved in undo array by back-tracking undo_level
*     - when there are redos and user creates a new undo, all redos are cleared (typical undo algorithm)
*  - undo_level is index to next position to fill with undo action
*     - next undo is undo_level - 1
*     - next redo is undo_level
*  - undo_min is index to oldest undo action
*  - redo_max - 1 is index to oldest redo action
******************************************************************************/

#include "stdafx.h"
#include "ccview.h"
//#include "ccdoc.h"
#include "undo.h"
#include "polylib.h"
#include "attrib.h"
#include "graph.h"
#include "CCEtoODB.h"

extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern BOOL Editing;
extern CView *activeView; // from CCVIEW.CPP

static void FreeUndoLevel(UndoItemList *list);

/******************************************************************************
* OnUndo 
*/
void CCEtoODBDoc::OnUndo()
{
   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      return;
   }

   CWaitCursor wait;
   
   UnselectAll(FALSE);
   ((CCEtoODBView*)activeView)->cursorMode = (CursorMode)0;
   //((CCEtoODBView*)activeView)->Anchored = FALSE; 

   // no more undos
   if (undo_level == undo_min)
   {
      ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)"No more undos.");
      return;
   }

   // move to last undo
   if (undo_level == 0)
      undo_level = MAX_UNDO_LEVEL - 1;
   else
      undo_level--;

   // UNDO
   POSITION itemPos = undo[undo_level]->GetHeadPosition();
   while (itemPos) // loop through items
   {
      UndoItemStruct *item = undo[undo_level]->GetNext(itemPos);

      switch (item->type)
      {
         /* Delete
         *     - when you delete, move data to undo structure, so no need to free memory
         *     - when you undo a delete, allocate memory and memcpy to a new data structure
         */
         case Edit_Delete:
         {
            BOOL Head = TRUE;
            POSITION newPos;
            if (item->num)
            {
               Head = FALSE;
               newPos = item->DataList->GetHeadPosition();
               while (newPos != NULL)
               {
                  if (item->DataList->GetAt(newPos)->getEntityNumber() == item->num)
                     break;
                  item->DataList->GetNext(newPos);
               }
            }

            DataStruct* newData = getCamCadData().getNewDataStruct(*(item->data),true,item->data->getEntityNumber());
            //newData->setEntityNumber(item->data->getEntityNumber());
            if (Head || !newPos)
               item->DataList->AddHead(newData);
            else
               item->DataList->InsertAfter(newPos, newData);
         }
         break; // end Edit_Delete

         /* Add
         *     - always added at tail, so just RemoveTail()
         */
         case Edit_Add:
         {
            //OnClearSelected();
// last command 
            DataStruct *data = item->DataList->GetTail();
            //switch (data->getDataType())
            //{
            //case T_TEXT:
            //   free(data->getText()->getText());
            //   delete data->getText();
            //   data->getText() = NULL;
            //   break;
            //case T_INSERT:
            //   {
            //      if (data->getInsert()->refname)
            //         free(data->getInsert()->refname);
            //      delete data->getInsert();
            //      data->getInsert() = NULL;
            //   }
            //   break;
            //case T_POINT:
            //   delete data->getPoint();
            //   data->getPoint() = NULL;
            //   break;
            //case T_POLY:
            //   FreePolyList(data->getPolyList());
            //   break;
            //}

            delete data;
            item->DataList->RemoveTail();

            if (Editing)
               OnEditEntity();
         }
         break; // end Edit_Add

         /* Modify
         *     - swap data ptrs so we can redo
         */
         case Edit_Modify:
         {
            DataStruct *orig, *mod;
            POSITION dataPos, selectPos;

            dataPos = item->DataList->GetHeadPosition();
            while (dataPos != NULL)
            {
               if (item->DataList->GetAt(dataPos)->getEntityNumber() == item->num)
                  break;
               item->DataList->GetNext(dataPos);
            }

            orig = item->data;
            mod = item->DataList->GetAt(dataPos);

            // update select list with new pointer to data
            BOOL Found = FALSE;
            selectPos = SelectList.GetHeadPosition();
            while (selectPos != NULL)
            {
               SelectStruct *s = SelectList.GetNext(selectPos);
               if (s->getData() == mod)
               {
                  s->setData(orig);
                  orig->setSelected(mod->isSelected());
                  orig->setMarked(mod->isMarked());
                  Found = TRUE;
                  break;
               }
            }

            if (!Found)
            {
               orig->setMarked(false);
               orig->setSelected(false);
            }

            item->data = mod;
            item->DataList->SetAt(dataPos, orig);

            if (Editing)
               OnEditEntity();
         }
         break; // end Edit_Modify
      }
   }
   UpdateAllViews(NULL);
}


/******************************************************************************
* OnRedo 
*/
void CCEtoODBDoc::OnRedo()
{
   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      return;
   }

   CWaitCursor wait;

   UnselectAll(FALSE);

   // no more redos
   if (undo_level == redo_max)
   {
      ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)"No more redos.");
      return;
   }

   // REDO
   POSITION itemPos = undo[undo_level]->GetTailPosition();
   while (itemPos) // loop through items
   {
      UndoItemStruct *item = undo[undo_level]->GetPrev(itemPos);

      switch (item->type)
      {
         case Edit_Delete:
         {
            DataStruct *data;
            POSITION pos = item->DataList->GetHeadPosition();
            while (pos)
            {
               data = item->DataList->GetAt(pos);
               if (data->getEntityNumber() == item->data->getEntityNumber())
                  break;
               item->DataList->GetNext(pos);
            }
            item->DataList->RemoveAt(pos);

            //switch (data->getDataType())
            //{
            //case T_TEXT:
            //   free(data->getText()->getText());
            //   delete data->getText();
            //   data->getText() = NULL;
            //   break;
            //case T_INSERT:
            //   if (data->getInsert()->refname)
            //      free(data->getInsert()->refname);
            //   delete data->getInsert();
            //   data->getInsert() = NULL;
            //   break;
            //case T_POINT:
            //   delete data->getPoint();
            //   data->getPoint() = NULL;
            //   break;
            //case T_POLY:
            //   FreePolyList(data->getPolyList());
            //   break;
            //}
            delete data;
         }
         break; // end Edit_Delete

         case Edit_Add:
            {
               DataStruct* newData = getCamCadData().getNewDataStruct(*(item->data),true,item->data->getEntityNumber());
               //DataStruct *newData = new DataStruct(*(item->data));
               //newData->setEntityNumber(item->data->getEntityNumber());
               item->DataList->AddTail(newData);
            }
            break; // end Edit_Add

         case Edit_Modify:
         {
            POSITION dataPos = item->DataList->GetHeadPosition();
            while (dataPos != NULL)
            {
               if (item->DataList->GetAt(dataPos)->getEntityNumber() == item->num)
                  break;
               item->DataList->GetNext(dataPos);
            }

            DataStruct *orig, *mod;
            mod = item->data;
            orig = item->DataList->GetAt(dataPos);

            // update select list with pointer remodified data
            BOOL Found = FALSE;
            POSITION selectPos = SelectList.GetHeadPosition();
            while (selectPos != NULL)
            {
               SelectStruct *s = SelectList.GetNext(selectPos);
               if (s->getData() == orig)
               {
                  s->setData(mod);
                  mod->setSelected(orig->isSelected());
                  mod->setMarked(orig->isMarked());
                  Found = TRUE;
                  break;
               }
            }

            if (!Found)
            {
               mod->setMarked(false);
               mod->setSelected(false);
            }

            item->data = orig;
            item->DataList->SetAt(dataPos, mod);
         if (Editing)
            OnEditEntity();
         }
         break; // end Edit_Modify
      }
   }
   UpdateAllViews(NULL);

   undo_level = (undo_level + 1) % MAX_UNDO_LEVEL; // advance undo_level
}

/******************************************************************************
* InsertAddUndo 
*/
UndoItemStruct *InsertAddUndo(SelectStruct *s, CCEtoODBDoc *doc)
{
   doc->PrepareThisUndoLevel();
   doc->undo[doc->undo_level] = new UndoItemList;

   // update undo array
   UndoItemStruct *item = new UndoItemStruct;
   item->type = Edit_Add;
   item->DataList = s->getParentDataList();
   item->num = s->getData()->getEntityNumber();
   item->data = doc->getCamCadData().getNewDataStruct(*(s->getData()),true,item->num);
   //item->data = new DataStruct(*(s->getData()));
   //item->data->setEntityNumber(item->num);

   doc->undo[doc->undo_level]->AddHead(item);
   doc->undo_level = doc->redo_max = (doc->undo_level + 1) % MAX_UNDO_LEVEL;
   doc->ClearNextUndoLevel();

   return item;
}

/******************************************************************************
* InsertCopyUndo 
*/
void InsertCopyUndo(CCEtoODBDoc *doc)
{
   doc->PrepareThisUndoLevel();
   doc->undo[doc->undo_level] = new UndoItemList;

   // update undo array
   POSITION pos = doc->SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = doc->SelectList.GetNext(pos);
      UndoItemStruct *item = new UndoItemStruct;
      item->type = Edit_Add;
      item->DataList = s->getParentDataList();
      item->num = s->getData()->getEntityNumber();
      item->data = doc->getCamCadData().getNewDataStruct(*(s->getData()),true,item->num);
      //item->data = new DataStruct(*(s->getData()));
      //item->data->setEntityNumber(item->num);
   
      doc->undo[doc->undo_level]->AddTail(item);
   }

   doc->undo_level = doc->redo_max = (doc->undo_level + 1) % MAX_UNDO_LEVEL;
   doc->ClearNextUndoLevel();
}

/******************************************************************************
* InsertModifyUndo()
*
* PARAMETER    : BOOL Head => Only save head item
*/
void InsertModifyUndo(CCEtoODBDoc *doc, BOOL Head)
{
   doc->PrepareThisUndoLevel();
   doc->undo[doc->undo_level] = new UndoItemList;

   POSITION selectPos = doc->SelectList.GetHeadPosition();
   while (selectPos != NULL)
   {
      SelectStruct *s = doc->SelectList.GetNext(selectPos);
      // update undo array
      UndoItemStruct *item = new UndoItemStruct;
      item->type = Edit_Modify;
      item->DataList = s->getParentDataList();
      item->num = s->getData()->getEntityNumber();

      // insert a new copy of data in undo list
      item->data = doc->getCamCadData().getNewDataStruct(*(s->getData()),true,item->num);
      //item->data = new DataStruct(*(s->getData()));
      //item->data->setEntityNumber(item->num);

      doc->undo[doc->undo_level]->AddTail(item);
   }

   doc->undo_level = doc->redo_max = (doc->undo_level + 1) % MAX_UNDO_LEVEL;
   doc->ClearNextUndoLevel();
}

/******************************************************************************
* ClearNextUndoLevel 
*  - prepares next undo level to be overwritten by freeing memory and adjusting indexes 
*     - call this after advancing undo level to take care of overwriting oldest undo
*/
void CCEtoODBDoc::ClearNextUndoLevel()
{
   if (undo_level == undo_min)
   {
      FreeUndoLevel(undo[undo_level]);
   
      // advance last undo because we are using its spot next
      undo_min = (undo_level + 1) % MAX_UNDO_LEVEL;
   }
}


/******************************************************************************
* PrepareThisUndoLevel 
*  - prepares this undo level to be overwritten by freeing memory and adjusting indexes 
*     - call this before adding an undo to clear all redos
*/
void CCEtoODBDoc::PrepareThisUndoLevel()
{
   for (int i = undo_level; i != redo_max; i++)
   {
      if (i == MAX_UNDO_LEVEL) // circular array; move to front of array
      {
         if (redo_max == 0) break;
         i = 0;
      }
      FreeUndoLevel(undo[i]);
   }

   redo_max = undo_level;
}


/* FreeUndoLevel 
*     - Frees memory allocated for 1 level 
*/
void FreeUndoLevel(UndoItemList *list)
{
	if (list == NULL)
		return;

   // loop through items affected by this undo
   POSITION pos = list->GetHeadPosition();
   while (pos)
   {
      UndoItemStruct *item = list->GetNext(pos);

      //switch (item->data->getDataType())
      //{
      //case T_POINT:
      //   delete item->data->getPoint();
      //   item->data->getPoint() = NULL;
      //   break;
      //case T_TEXT:
      //   free(item->data->getText()->getText());
      //   delete item->data->getText();
      //   item->data->getText() = NULL;
      //   break;
      //case T_INSERT:
      //   if (item->data->getInsert()->refname)
      //      free(item->data->getInsert()->refname);
      //   delete item->data->getInsert();
      //   item->data->getInsert() = NULL;
      //   break;
      //case T_POLY:
      //   FreePolyList(item->data->getPolyList());
      //   break;
      //}
      //if (item->data->getAttributesRef())
      //{
      //   FreeAttribs(item->data->getAttributesRef());
      //   delete item->data->getAttributesRef();
      //}
      delete item->data;

      delete item;
   }

   delete list;
}


/******************************************************************************
* ClearUndo
*  - Frees all memory allocated to Undo Structure
*/
void CCEtoODBDoc::ClearUndo()
{
   for (int i = undo_min; i != redo_max; i++)
   {
      if (i == MAX_UNDO_LEVEL) // circular array; move to front of array
      {
         if (redo_max == 0) break;
         i = 0;
      }
      FreeUndoLevel(undo[i]);
   }

   undo_level = 0;
   undo_min = 0;
   redo_max = 0;
}


/******************************************************************************/
/******************************************************************************/
static void DumpEntity(FILE *stream, DataStruct *np)
{
   fprintf(stream, "%p\t\t%3ld %2d ", np, np->getEntityNumber(), np->getLayerIndex());

   switch (np->getDataType())
   {
      case T_POLY:
         {
            CPoly *poly;
            CPnt *pnt;
            POSITION polyPos, pntPos;

            fprintf(stream,"POLYSTRUCT %d %ld\n",
                  np->isNegative(), np->getFlags());

            // loop polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               fprintf(stream,"  %p POLY %d %d %d %d\n",
                     poly, poly->getWidthIndex(), poly->isFilled(), poly->isVoid(), poly->isClosed());

               // loop points of poly
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  fprintf(stream,"    %p VERT %+lg %+lg %+lg\n",
                        pnt, pnt->x, pnt->y, pnt->bulge);
               }
            }
            fprintf(stream, " * END POLYSTRUCT\n");
         }
         break;

      case T_TEXT:
         fprintf(stream,"TEXT \"%s\" %+lg %+lg %lg %lg %+lg %d %d %d %d %d\n",
               np->getText()->getText(), np->getText()->getPnt().x, np->getText()->getPnt().y,
               np->getText()->getHeight(), np->getText()->getWidth(), np->getText()->getRotation(), np->getText()->getOblique(),
               np->getText()->isMirrored(), np->getText()->isProportionallySpaced(), np->getText()->getPenWidthIndex(),
               np->isNegative());
         break;
      
      case T_INSERT:
         fprintf(stream,"INSR %d %+lg %+lg %+lg %d %lg\n",
               np->getInsert()->getBlockNumber(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(),
               np->getInsert()->getAngle(), np->getInsert()->getMirrorFlags(), np->getInsert()->getScale());
         break;

      case T_POINT:
         fprintf(stream,"PNT %+lg %+lg\n",
               np->getPoint()->x, np->getPoint()->y);
         break;
   } // end swithch
}

void CCEtoODBDoc::DumpUndo()
{
   FILE *stream;
   CString type;

   static int n = 0;
   CString FileName;
   FileName.Format("C:\\%d.txt", n++);

   stream = fopen(FileName, "w+t");
   if (stream == NULL)
   {
      MessageBox(NULL, FileName, "Unable To Open File", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fputs("* Undo Dump\n", stream);

   fprintf(stream, "\nundo_level = %d, undo_min = %d, redo_max = %d\n", undo_level, undo_min, redo_max);

   fputs("\n* Array Index, Item, Datas\n", stream);

   for (int i = undo_min; i != redo_max; i++)
   {
      if (i == MAX_UNDO_LEVEL) // circular array; move to front of array
      {
         if (redo_max == 0) break;
         i = 0;
      }

      POSITION pos = undo[i]->GetHeadPosition();
      while (pos)
      {
         UndoItemStruct *item = undo[i]->GetNext(pos);
         
         switch (item->type)
         {
            case Edit_Delete:type="Delete";break;
            case Edit_Add:type="Add";break;
            case Edit_Modify:type="Modify";break;
         }

         fputs("\n-----------------------------------------------------------\n", stream);
         fprintf(stream, "%c %d - %s\n", (i == undo_level?'U':' '), i, type);

         if (item->type == Edit_Delete)
            fprintf(stream, "\tInsert After %d\n", item->num);
         fputs("\tData\n", stream);
         DumpEntity(stream, item->data);
      }
   }

   fputs("\n\nSelectList\n\n", stream);
   POSITION selPos = SelectList.GetHeadPosition();
   while (selPos != NULL)
      DumpEntity(stream, SelectList.GetNext(selPos)->getData());

   fclose(stream);
}

// end UNDO.CPP