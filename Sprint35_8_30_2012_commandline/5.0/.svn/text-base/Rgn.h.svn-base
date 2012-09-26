// $Header: /CAMCAD/5.0/Rgn.h 17    6/21/07 8:27p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

class CCEtoODBDoc;

CRgn* CreateRgn(CCEtoODBDoc* doc,CPolyList* polyList,float scaleFactor);
CRgn* CreateRgn(CCEtoODBDoc& doc,CPolyList& polyList,double scaleFactor);

void GetRgnExtents(CRgn *rgn, ExtentRect *extents, float scaleFactor);

CPolyList *GetPolyFromRgn(CRgn *rgn, float scaleFactor);

CRgn* GetPolyRgn(CPoly* poly,float scaleFactor,PageUnitsTag pageUnits);
CRgn* GetPolyRgn(CPoly& poly,double scaleFactor,PageUnitsTag pageUnits);
