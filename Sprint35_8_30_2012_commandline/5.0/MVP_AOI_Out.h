// $Header: /CAMCAD/4.5/MVP_AOI_Out.h 13    10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

typedef struct {
	int sizeA;  // X dimension, per amendment #041205b
	int sizeB;  // Y dim
	int fill;
} mvpAperture;

bool getVisibleFile(CCEtoODBDoc *doc, FileStruct *&file);
bool openFiles(const char *fullPath, FILE *&topFile, FILE *&botFile);

mvpAperture *getMvpApertureParams(CCEtoODBDoc *doc, BlockStruct *geom, double unitFactor);

CString generateApertureDefinition(CCEtoODBDoc *doc, BlockStruct *geom, int pinNum, double xOffset, double yOffset, double insertRotation, double unitFactor, double thickness);

int getActualLayer(CCEtoODBDoc *doc, DataStruct *data, int insertLayerIndex, bool mirrorLayers);

int convertUnit(double num, double unitFactor);
int convertAngle(double radians);

// TODO: Check for Layer Typing
