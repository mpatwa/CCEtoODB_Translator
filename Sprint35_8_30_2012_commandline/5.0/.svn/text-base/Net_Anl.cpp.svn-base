// $Header: /CAMCAD/5.0/Net_Anl.cpp 16    6/17/07 8:52p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

#include "net_anl.h"
#include "geomlib.h"
#include "attrib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
*  NetLength_Net
*  returns -1 for net not found
*/
double NetLength_Net(CCEtoODBDoc *doc, FileStruct *file, CString netname) 
{
   double length = 0;
   int found = 0;
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, 0);

   // here get netnames
   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();

   while (pos != NULL)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() != T_POLY)  
         continue;

      if (!data->getAttributesRef())
         continue;

      Attrib* attrib;

      if (!data->getAttributesRef()->Lookup(keyword, attrib))
         continue;

      if (netname.Compare(attrib->getStringValue()))
         continue;
         
      found++;
      
      length += NetLength_Poly(doc, data->getPolyList());
   }

   if (!found) return -1;

   return length;
}

/******************************************************************************
*  NetLength_Net_Manhattan
*  returns -1 for net not found
*/
double NetLength_Net_Manhattan(CCEtoODBDoc *doc, FileStruct *file, CString netname) 
{
   return 0;
}

/******************************************************************************
* NetLength_Pins
*/
double NetLength_Pins(CCEtoODBDoc *doc, FileStruct *file, CString comp1, CString pin1, CString comp2, CString pin2) 
{

   return 0.0;
}

/******************************************************************************
* NetLength_Pins_Manhattan
*/
double NetLength_Pins_Manhattan(CCEtoODBDoc *doc, FileStruct *file, CString comp1, CString pin1, CString comp2, CString pin2) 
{

   return 0.0;
}

/******************************************************************************
* NetLength_Poly
*/
double NetLength_Poly(CCEtoODBDoc *doc, CPolyList *polylist) 
{
   double length = 0;

   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = polylist->GetNext(polyPos);
      if (poly->isHidden()) continue;
      if (poly->isClosed()) continue;

      CPnt *pnt, *prev;
      POSITION pntPos = poly->getPntList().GetHeadPosition();
      if (pntPos)
         pnt = poly->getPntList().GetNext(pntPos);
      while (pntPos)
      {
         prev = pnt;
         pnt = poly->getPntList().GetNext(pntPos);
         length += NetLength_Segment(doc, prev->x, prev->y, pnt->x, pnt->y, prev->bulge);
      }
   }  

   return length;
}

/******************************************************************************
* NetLength_Segment
*/
double NetLength_Segment(CCEtoODBDoc *doc, double x1, double y1, double x2, double y2, double bulge) 
{
   double len;

   if (fabs(bulge) < BULGE_THRESHOLD)
   {
      len = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2-y1));
   }
   else
   {
      Point2 center;
      double r, sa, x, y;
      double da = atan(bulge) * 4;
      ArcPoint2Angle(x1, y1, x2, y2, da, &x, &y, &r, &sa);
      center.x = x;
      center.y = y;
      len = LengthOfArc(&center, r, sa, da);
   }

   return len;
}
