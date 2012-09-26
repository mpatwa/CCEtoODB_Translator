// $Header: /CAMCAD/4.6/read_wrt/MVP_AOI_Out.cpp 36    2/14/07 4:08p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "stdafx.h"
#include "float.h"
#include "ccdoc.h"
#include "extents.h"
#include "MVP_AOI_Out.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// getVisibleFile
bool getVisibleFile(CCEtoODBDoc *doc, FileStruct *&file)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *tempFile = doc->getFileList().GetNext(filePos);

		if (!tempFile->isShown())
			continue;

		if (file == NULL)
			file = tempFile;
		else
		{
	      ErrorMessage("Multiple visible files found. Only 1 visible file allowed", "Too many files");
		   return false;
		}
   }
   
	if (file == NULL)
   {
      ErrorMessage("No visible file found.", "No Visible File");
      return false;
   }

	return true;
}


// openFiles
bool openFiles(const char *fullPath, FILE *&topFile, FILE *&botFile)
{
	char drive[_MAX_DRIVE], path[_MAX_DIR], filename[_MAX_FNAME], ext[_MAX_EXT], tempPath[_MAX_PATH];
	_splitpath(fullPath, drive, path, filename, ext);
	_makepath(tempPath, drive, path, filename, NULL);
	CString topFilename, botFilename;
	topFilename = tempPath;
	topFilename += "_TOP.MVP";
	botFilename = tempPath;
	botFilename += "_BOT.MVP";

   topFile = fopen(topFilename, "wt");
   botFile = fopen(botFilename, "wt");
   if (!topFile)
   {
      CString buf;
      buf.Format("Can not open [%s]", topFilename);
      ErrorMessage( buf, "Error File Open", MB_OK | MB_ICONHAND);
      return false;
   }
   if (!botFile)
   {
      CString buf;
      buf.Format("Can not open [%s]", botFilename);
      ErrorMessage(buf, "Error File Open", MB_OK | MB_ICONHAND);
      return false;
   }

	return true;
}

mvpAperture *getMvpApertureParams(CCEtoODBDoc *doc, BlockStruct *geom, double unitFactor)
{
	// Sizes
	int sizeA, sizeB;
	if (geom->getFlags() & BL_APERTURE || geom->getFlags() & BL_BLOCK_APERTURE) 
	{
		if (geom->getShape() == T_COMPLEX)
		{
			BlockStruct *subGeom = doc->Find_Block_by_Num((int)(geom->getSizeA()));
			return getMvpApertureParams(doc, subGeom, unitFactor);
		}

		sizeA = convertUnit(geom->getSizeA(), unitFactor);
		sizeB = convertUnit(geom->getSizeB(), unitFactor);
		if (sizeB == 0)
			sizeB = sizeA;
	}
	else 
	{		
		CExtent extents = geom->getExtent();
		sizeA = convertUnit(extents.getXsize(), unitFactor);
		sizeB = convertUnit(extents.getYsize(), unitFactor);
	}


	// % Fill
	int percentFill = 100;
	switch (geom->getShape())
	{
	case T_ROUND:
	case T_DONUT:
		percentFill = round(PI / 4 * 100);
		break;
	case T_OBLONG:
		{
         double a = max(sizeA, sizeB);
         double b = min(sizeA, sizeB);
			percentFill = round((a - b + PI * b / 4) / a * 100);
		}
		break;
	case T_OCTAGON:
			percentFill = round(sizeA * sizeA - 2 * pow(sizeA / (2 + sqrt(2.0)), 2) * 100);
			break;
	}

	mvpAperture *ap = new mvpAperture;
	ap->sizeA = sizeA;
	ap->sizeB = sizeB;
	ap->fill = percentFill;

	return ap;
}

//	generateApertureDefinition
CString generateApertureDefinition(CCEtoODBDoc *doc, BlockStruct *geom, int pinNum, double xOffset, double yOffset, double insertRotation, double unitFactor, double thickness)
{
	int sizeA = 0;
	int sizeB = 0;
	int percentFill = 0;

	mvpAperture *mvpAp = getMvpApertureParams(doc, geom, unitFactor);
	if (mvpAp)
	{
		sizeA = mvpAp->sizeA;
		sizeB = mvpAp->sizeB;
		percentFill = mvpAp->fill;
	}

	// Orientation
	int degrees = convertAngle(geom->getRotation());
	char orientation;
	if (degrees <= 45)
		orientation = 'N';
	else if (degrees < 135)
		orientation = 'W';
	else if (degrees < 225)
		orientation = 'S';
	else if (degrees < 315)
		orientation = 'E';
	else 
		orientation = 'E';


	// Output
	CString output;
	output.Format("%d\t%d\t%d\t%d\t%d\t%c\t%d\t%d\n", 
			convertUnit(xOffset, unitFactor), 
			convertUnit(yOffset, unitFactor), 
			sizeB,  // Amendment #041205b, swap order of "width of pad" and
      	sizeA,  // "length of pad", which puts sizeB before sizeA.
			pinNum,
			orientation,
			percentFill, 
			convertUnit(thickness, unitFactor));

	delete mvpAp;

	return output;
}

// getActualLayer
int getActualLayer(CCEtoODBDoc *doc, DataStruct *data, int insertLayerIndex, bool mirrorLayers)
{
	int layerIndex = data->getLayerIndex();
	if (doc->IsFloatingLayer(layerIndex) && insertLayerIndex != -1)
		layerIndex = insertLayerIndex;

	if (mirrorLayers && layerIndex != -1)
		layerIndex = doc->getLayerArray()[layerIndex]->getMirroredLayerIndex();

	return layerIndex;
}


// convertUnit
int convertUnit(double num, double unitFactor)
{
	return (int)floor(num * unitFactor + 0.5);
}


// convertAngle
int convertAngle(double radians)
{
	int degrees = (int)floor(radiansToDegrees(radians) + 0.5);

	while (degrees < 0)
		degrees += 360;
	while (degrees >= 360)
		degrees -= 360;

	return degrees;
}

