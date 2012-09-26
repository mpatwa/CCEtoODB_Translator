// $Header: /CAMCAD/4.3/Outline.h 9     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

//----------------------------------------------------------------
//
// Outline Utility, collects pnts and determines an outline
//
void Outline_Start(CCEtoODBDoc *Doc, bool allowUnion = false, bool useSinglePolyOutlineAsIs = false);

int Outline_Add(DataStruct *data, double scale, double insert_x, double insert_y, double rotation, BOOL mirror);
int Outline_Add_With_Width(DataStruct *data, double scale, double insert_x, double insert_y, double rotation, BOOL mirror);
int Outline_Add_Poly(CPoly *poly, double scale, double insert_x, double insert_y, double rotation, BOOL mirror);
int Outline_Add_Aperture(DataStruct *data, double scale, double insert_x, double insert_y, double rotation, BOOL mirror);

CPntList *Outline_GetOutline(int *returnCode, double accuracy);

void Outline_FreeResults();

CPoly *GetSegmentOutline(double ax, double ay, double bx, double by, double bulge, double widthRadius, int widthIndex);

//
//-----------------------------------------------------------------

// A function to get panel or board outline extent, presumes activeFile is a panel or pcb design file.
// Returns true if outline found, false otherwise in which case extent is block extent.
bool GetOutlineExtent(CCEtoODBDoc &doc, FileStruct *activeFile, CExtent &extent); 

// end OUTLINE.H
