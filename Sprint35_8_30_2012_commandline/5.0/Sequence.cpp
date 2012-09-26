// $Header: /CAMCAD/4.6/Sequence.cpp 18    6/16/06 8:23p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "mainfrm.h"

extern CView *activeView; // from CCVIEW.CPP

void CCEtoODBDoc::OnShowSeqNums() 
{
   showSeqNums = !showSeqNums;
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnShowSeqArrows() 
{
   showSeqArrows = !showSeqArrows;
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnSeqHead() 
{
   if (SelectList.IsEmpty())
      return;

   ((CCEtoODBView*)activeView)->lastCommand = LC_SeqFirst;

   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);

      s->getParentDataList()->RemoveAt(s->getParentDataList()->Find(s->getData()));

      s->getParentDataList()->AddHead(s->getData());

      // reset sequence flags
      s->getData()->clearFlagBits(DATA_SEQ_HEAD | DATA_SEQ_CONT);
   }

   ClearUndo();

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnSeqTail() 
{
   if (SelectList.IsEmpty())
      return;

   ((CCEtoODBView*)activeView)->lastCommand = LC_SeqLast;

   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);

      s->getParentDataList()->RemoveAt(s->getParentDataList()->Find(s->getData()));

      s->getParentDataList()->AddTail(s->getData());

      // reset sequence flags
      s->getData()->clearFlagBits(DATA_SEQ_HEAD | DATA_SEQ_CONT);
   }

   ClearUndo();

   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnSeqAssignHead() 
{
   if (nothingIsSelected())
      return;

   SelectStruct *s = getSelectStack().getAt(0);

   seqInsertBehind = s->getData();
   seqDataList = s->getParentDataList();

   s->getData()->clearFlagBits(DATA_SEQ_CONT);
   s->getData()->setFlagBits(DATA_SEQ_HEAD);
}

void CCEtoODBDoc::OnSeqBehindHead() 
{
   if (nothingIsSelected())
      return;

   ((CCEtoODBView*)activeView)->lastCommand = LC_SeqBehindHead;

   SelectStruct *s = getSelectStack().getAt(0);

   if (!seqInsertBehind)
   {
      ErrorMessage("First pick Head to insert behind");
      return;
   }

   if (seqDataList != s->getParentDataList())
   {
      ErrorMessage("Entity is not in same Block as Head");
      return;
   }

   if (seqInsertBehind == s->getData())
   {
      ErrorMessage("You selected the current Head");
      return;
   }

   POSITION insertPos = seqDataList->Find(seqInsertBehind);
   if (!insertPos)
   {
      ErrorMessage("Could not find Head");
      return;
   }

   s->getParentDataList()->RemoveAt(s->getParentDataList()->Find(s->getData()));
   s->getParentDataList()->InsertAfter(insertPos, s->getData());

   seqInsertBehind = s->getData();
   s->getData()->clearFlagBits(DATA_SEQ_HEAD);
   s->getData()->setFlagBits(DATA_SEQ_CONT);

   ClearUndo();

   UpdateAllViews(NULL);
}

// end SEQUENCE.CPP
