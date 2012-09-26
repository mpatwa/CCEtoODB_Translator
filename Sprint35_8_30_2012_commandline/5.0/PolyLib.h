// $Header: /CAMCAD/5.0/PolyLib.h 28    6/21/07 8:26p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/  

#if !defined(__PolyLib_h__)
#define __PolyLib_h__

#pragma once

#include "ccdoc.h"
#include "math.h"
#include "geomlib.h"
#include "DcaBuildingBlockLib.h"
#include "DcaPolyLib.h"

void FreePolyList(CPolyList*& polylist);
//void FreePoly(CPoly *poly);
void FreePntList(CPntList *pntList);

void TransformPolyList(CPolyList *polylist, double insert_x, double insert_y, double rotation, int mirror, double scale);
void TransformPoly(CPoly *poly, double insert_x, double insert_y, double rotation, int mirror, double scale);

void CleanPoly(CPoly *poly, double smalldist);

void LinkPolyDatas(CCEtoODBDoc *doc, CDataList *datalist, DataStruct *parentData, DataStruct *childData);

void VectorizePoly(CPoly *poly,PageUnitsTag pageUnits);
void VectorizePntList(CPntList* pntList,PageUnitsTag pageUnits);

bool PolyIsCircle(const CPoly *poly, double *cx, double *cy, double *radius);
bool PntListIsCircle(const CPntList* pntList, double *cx, double *cy, double *radius);
bool PolyIsRectangle(CPoly *poly, double *llx, double *llr, double *uux, double *uur);
bool PolyIsRectangle(CPoly *poly, double lineWidth, double *llx, double *llr, double *uux, double *uur);

// accuracy is needed to the angle transformation
BOOL PolyIsRectangleAngle(CPoly *poly, double linewidth, double accuracy, 
                          double *llx, double *llr, double *uux, double *uur, double *angle);

BOOL PolyIsOblong(CPoly *poly,  double *sizeA, double *sizeB, double *cx, double *cy);
BOOL PolySelfIntersects(CPntList *pntList);

//CPntList *ShrinkPoly(CPntList *pntList, double dist,bool suppressErrorMessages=false);
void CopyPoly(DataStruct *dest, DataStruct *src);
void ClosePoly(CPoly *poly); //closing the open poly
void ReversePoly(CPntList *pntList); //change poly to backward

int FixFreeVoids(CCEtoODBDoc *doc, CDataList *datalist);

int Is_Pnt_Inside_Poly(CPoly *poly, CPnt *p, double tolerance = SMALLNUMBER); //check if pnt inside of poly.
int Is_Pnt_Inside_PntList(const CPntList *list, CPnt *p,PageUnitsTag pageUnits, double tolerance = SMALLNUMBER); //check if pnt inside of poly.
int Is_Pnt_Inside_PntList(const CPntList *list, double xPoint, double yPoint,PageUnitsTag pageUnits, double tolerance = SMALLNUMBER);                           
int Is_Point_Inside_Poly(CPoly *Poly, Point2 *p);                            

int CalcTransform(CPnt *referencePnt, 
      CPnt *origA, CPnt *origB, CPnt *origC,
      CPnt *newA, CPnt *newB, CPnt *newC,
      double *offsetX, double *offsetY, double *scale, double *rotation, BOOL *mirror);

// end POLYLIB.H

#endif

