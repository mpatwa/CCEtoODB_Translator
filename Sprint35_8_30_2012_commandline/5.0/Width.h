// $Header: /CAMCAD/4.3/Width.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include <math.h>
#include "ccdoc.h"
static CPoly *widthpoly;

void widthpoly_StartNew(CCEtoODBDoc *Doc); 
void widthpoly_Free();
void Width(CCEtoODBDoc *doc, DataStruct *data,  
           double penwidth, CPoly *widthpoly);
// end WIDTH.H
