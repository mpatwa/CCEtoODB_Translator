// $Header: /CAMCAD/4.3/Net_Anl.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"

double NetLength_Net(CCEtoODBDoc *doc, FileStruct *file, CString netname);
double NetLength_Net_Manhattan(CCEtoODBDoc *doc, FileStruct *file, CString netname);
double NetLength_Pins(CCEtoODBDoc *doc, FileStruct *file, CString comp1, CString pin1, CString comp2, CString pin2);
double NetLength_Pins_Manhattan(CCEtoODBDoc *doc, FileStruct *file, CString comp1, CString pin1, CString comp2, CString pin2);
double NetLength_Poly(CCEtoODBDoc *doc, CPolyList *polylist);
double NetLength_Segment(CCEtoODBDoc *doc, double x1, double y1, double x2, double y2, double bulge);
