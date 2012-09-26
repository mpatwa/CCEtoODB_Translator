// $Header: /CAMCAD/4.3/Hilite.cpp 7     8/12/03 4:20p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "hilite.h"
#include "ccdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* HighlightAPin
*/
void HighlightAPin(CCEtoODBDoc *doc, CString comp, CString pin, COLORREF color)
{
   // check if this comp pin already exists
   BOOL Found = FALSE;
   POSITION hpPos = doc->HighlightedPinsList.GetHeadPosition();
   while (hpPos)
   {
      HighlightedPinStruct *hp = doc->HighlightedPinsList.GetNext(hpPos);

      if (!hp->comp.CompareNoCase(comp) && !hp->pin.CompareNoCase(pin))
      {
         hp->color = color;
         Found = TRUE;
         break;
      }
   }

   if (!Found)
   {
      HighlightedPinStruct *p = new HighlightedPinStruct;
      p->comp = comp;
      p->pin = pin;
      p->color = color;
      doc->HighlightedPinsList.AddTail(p);
   }
}

/******************************************************************************
* HighlightAPinClear
*/
BOOL HighlightAPinClear(CCEtoODBDoc *doc, CString comp, CString pin)
{
   POSITION pos = doc->HighlightedPinsList.GetHeadPosition();
   while (pos != NULL)
   {
      HighlightedPinStruct *hp = doc->HighlightedPinsList.GetAt(pos);

      if (!hp->comp.CompareNoCase(comp) && !hp->pin.CompareNoCase(pin))
      {
         delete hp;
         doc->HighlightedPinsList.RemoveAt(pos);
         return TRUE;
      }
      doc->HighlightedPinsList.GetNext(pos);
   }

   return FALSE;
}

/******************************************************************************
* HighlightAPinClearAll
*/
void HighlightAPinClearAll(CCEtoODBDoc *doc)
{
   POSITION pos = doc->HighlightedPinsList.GetHeadPosition();
   while (pos != NULL)
      delete(doc->HighlightedPinsList.GetNext(pos));

   doc->HighlightedPinsList.RemoveAll();
}
