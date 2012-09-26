// $Header: /CAMCAD/4.3/Aperture.h 7     8/12/03 9:05p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#pragma once

#include "block.h"

struct AperStruct
{
   char Name[MAXAPNAME+1];
   int DCode;
   short Type;
   double Size_a;
   double Size_b;
   double Offset_x;
   double Offset_y;
   int Rotation;        // in degree
   AperStruct *next;
};

AperStruct *UniversalApertureRead(char *macroFile, const char *apertureFile, char *logFile,
                                  int *apUnits, char *macName);

void SetFields(struct AperStruct  *node, struct Data  *data);

/**************************************************************************/
AperStruct *BuildList(BlockStruct *block, BlockStruct **widthTable, int lastIndex);
void ap_list(char *prefix, AperStruct *Head, int pageUnits, int apUnits);
int show_aplist(AperStruct **Head, char* prefix);
void free_list(AperStruct *Head);

// end aperatur.h
