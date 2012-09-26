// $Header: /CAMCAD/5.0/Apertur2.h 15    12/10/06 3:52p Kurt Van Ness $

/*****************************************************************************/
/*  
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-98. All Rights Reserved.
*/           

#include "block.h"
#include "geomlib.h"
#include "DcaModeStack.h"

CPolyList* ApertureToPoly(BlockStruct* block, InsertStruct* insert, int mirror);
CPolyList* ApertureToPoly(const BlockStruct* block, double x=0., double y=0., double rot=0., int mirror=0);
CPolyList* ApertureToPoly(CCEtoODBDoc* camCadDoc,const BlockStruct* block, double x=0., double y=0., double rot=0., int mirror=0);
CPolyList* ApertureToPoly_Base(CCEtoODBDoc* camCadDoc,const BlockStruct* block, double x, double y, double rot, int mirror);
CPolyList* ApertureToPoly_Base(const BlockStruct* block, double x, double y, double rot, int mirror);

CPolyList* ApertureToPolyTranspose(BlockStruct* block, InsertStruct *insert, 
      double insert_x, double insert_y, double angle, double scale, int mirror);

CPolyList* ApertureToPolyTranspose_Base(BlockStruct* block, double x, double y, double rot, 
      double insert_x, double insert_y, double angle, double scale, int mirror);

// end APERTURE2.H