// $Header: /CAMCAD/4.5/Api_Net.cpp 13    4/28/06 11:32a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "API.h"
#include "ccview.h"
#include "net_anl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCEtoODBView *apiView; // from API.CPP

/******************************************************************************
* NetLengthNet
*/
short API::NetLengthNet(long filePosition, LPCTSTR NetName, double FAR* length) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   *length = NetLength_Net(doc, file, NetName);
   if (*length < 0)
      return RC_ITEM_NOT_FOUND;
   return RC_SUCCESS;
}

/******************************************************************************
* NetLengthNetManhattan
*/
short API::NetLengthNetManhattan(long filePosition, LPCTSTR NetName, double FAR* length) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   *length = NetLength_Net(doc, file, NetName);
   if (*length < 0)
      return RC_ITEM_NOT_FOUND;
   return RC_SUCCESS;
}

/******************************************************************************
* NetLengthPins
*/
short API::NetLengthPins(long filePosition, LPCTSTR comp1, LPCTSTR pin1, LPCTSTR comp2, LPCTSTR pin2, double FAR* length) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   *length = NetLength_Pins(doc, file, comp1, pin1, comp2, pin2);
   if (*length < 0)
      return RC_ITEM_NOT_FOUND;
   return RC_SUCCESS;
}

/******************************************************************************
* NetLengthPinsManhattan
*/
short API::NetLengthPinsManhattan(long filePosition, LPCTSTR comp1, LPCTSTR pin1, LPCTSTR comp2, LPCTSTR pin2, double FAR* length) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   POSITION pos = (POSITION)filePosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   FileStruct *file = doc->getFileList().GetAt(pos);

   *length = NetLength_Pins_Manhattan(doc, file, comp1, pin1, comp2, pin2);
   if (*length < 0)
      return RC_ITEM_NOT_FOUND;
   return RC_SUCCESS;
}

/******************************************************************************
* NetLengthPoly
*/
short API::NetLengthPoly(short geometryNumber, long dataPosition, double FAR* length) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   if (geometryNumber < 0 || geometryNumber >= doc->getMaxBlockIndex())
      return -1;

   BlockStruct *block = doc->getBlockAt(geometryNumber);

   if (!block)
      return -1;

   // find DATA
   POSITION pos = (POSITION)dataPosition;
   if (!AfxIsValidAddress(pos, 3 * sizeof(void*)))
      return RC_ILLEGAL_POSITION;
   DataStruct *data = block->getDataList().GetAt(pos);

   if (data->getDataType() != T_POLY)
      return RC_WRONG_TYPE;

   *length = NetLength_Poly(doc, data->getPolyList());
   return RC_SUCCESS;
}

/******************************************************************************
* NetLengthSegment
*/
short API::NetLengthSegment(double x1, double y1, double x2, double y2, double bulge, double FAR* length) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   *length = NetLength_Segment(doc, x1, y1, x2, y2, bulge);
   return RC_SUCCESS;
}
