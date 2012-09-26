// $Header: /CAMCAD/5.0/read_wrt/MVP_AOI_Paste_Out.cpp 28    6/17/07 8:59p Kurt Van Ness $

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


static void collectInformation();
static void doGeometry(BlockStruct *geom, 
      double insert_x, double insert_y, double rotation, int mirror,
      int insertLayerIndex);
static void doInserts();
static void doDefinitions();
static CString cleanGeomName(BlockStruct *geom);//, double radianRotation);
static bool checkLayerTyping(CCEtoODBDoc *doc);


static FILE *topFile, *botFile;
static CCEtoODBDoc *doc;
static FileStruct *file = NULL;
static double unitFactor;
static CPtrList inserts;


struct InsertedGeomStruct
{
	BlockStruct *geom;
	CString geomName;
	CString mvpGeomName;  // as per ammendment #041205b
	double insert_x;
	double insert_y;
	double rotation;
	double stencilThickness;
	bool mirrorFlip;
	int insertLayerIndex;
	bool top;
	bool bot;
	CString ecad_att_board;
	CString ecad_att_refdes;
	CString ecad_att_pin;
};


/******************************************************************************
* MVP_AOI_Paste_WriteFiles
*/
void MVP_AOI_Paste_WriteFiles(const char *filename, CCEtoODBDoc *Doc)
{
	// Init Variables
   doc = Doc;
	unitFactor = Units_Factor(doc->getSettings().getPageUnits(), UNIT_MILS);
	file = NULL;
	inserts.RemoveAll();


	// 1 Visible File Only
	if (!getVisibleFile(doc, file))
		return;


	// Open Files
	if (!openFiles(filename, topFile, botFile))
		return;


	// Check for Layer Typing
	if (!checkLayerTyping(doc))
		return;


	// Collect Information
	collectInformation();


	// Header
	fprintf(topFile, "[MVPCAD]\n");
	fprintf(botFile, "[MVPCAD]\n");


	// Inserts
	doInserts();


	// Geometry Definitions
	fprintf(topFile, "\n[USER]\n");
	fprintf(botFile, "\n[USER]\n");
	doDefinitions();


	// Close Files
   fclose(topFile);
   fclose(botFile);
}


// collectInformation
void collectInformation()
{
	doGeometry(file->getBlock(), file->getInsertX(), file->getInsertY(), file->getRotation(), file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0, -1);
}


// doGeometry
void doGeometry(BlockStruct *geom, 
      double insert_x, double insert_y, double rotation, int mirror,
      int insertLayerIndex)
{

	double stencilBaseThickness = 0.0;
	Attrib *attrib = NULL;
	if (attrib = is_attvalue(doc, geom->getAttributesRef(), "StencilBaseThickness", 0))
	{
		stencilBaseThickness = attrib->getDoubleValue();
	}

   POSITION dataPos = geom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = geom->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

		BlockStruct *insertedGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

      Point2 point2;
      point2.x = data->getInsert()->getOriginX();
      point2.y = data->getInsert()->getOriginY();
      if (mirror & MIRROR_FLIP)
			point2.x = -point2.x;
		Mat2x2 m;
		RotMat2(&m, rotation);
		TransPoint2(&point2, 1, &m, insert_x, insert_y);

		double insertRotation;
		if (mirror & MIRROR_FLIP)
         insertRotation = rotation - data->getInsert()->getAngle();
      else
         insertRotation = rotation + data->getInsert()->getAngle();

		//------amendment #041205b------
		double stencilThickness = stencilBaseThickness;
		if (attrib = is_attvalue(doc, data->getAttributesRef(), "StencilThickness", 0))
		{
			stencilThickness = attrib->getDoubleValue();
		}

		CString attBoardname = "";
		if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_ECAD_BOARD_ON_PANEL, 0))
		{
			attBoardname = attrib->getStringValue();
		}

		CString attRefdes = "";
		if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_ECAD_REFDES, 0))
		{
			attRefdes = attrib->getStringValue();
		}

		CString attPin = "";
		if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_ECAD_PIN, 0))
		{
			attPin = attrib->getStringValue();
		}

		int layerIndex = getActualLayer(doc, data, insertLayerIndex, mirror & MIRROR_LAYERS ? true : false);

		if (insertedGeom->getFlags() & BL_APERTURE || insertedGeom->getFlags() & BL_BLOCK_APERTURE) // Aperture
		{
			if (geom->getShape() == T_COMPLEX)
				insertedGeom = doc->Find_Block_by_Num((int)(geom->getSizeA()));
			else
			{
				if (layerIndex < 0)
					continue;

				LayerStruct *layer = doc->getLayerArray()[layerIndex];

				bool topHasPaste = false, botHasPaste = false;
				if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
					topHasPaste = TRUE;
				if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
					botHasPaste = TRUE;

				if (!topHasPaste && !botHasPaste)
					continue;

				mvpAperture *mvpap = getMvpApertureParams(doc, insertedGeom, unitFactor);
				CString mvpGeomName;
				if (mvpap) {
					// Yes, sizeB is first on purpose, per amendment #041205b
					mvpGeomName.Format("PAD_%04d_%04d_%04d_%04d", mvpap->sizeB, mvpap->sizeA, mvpap->fill, convertUnit(stencilThickness, unitFactor));
					delete mvpap;
				} else {
					mvpGeomName = cleanGeomName(insertedGeom);
				}

				InsertedGeomStruct *insert = new InsertedGeomStruct();
				insert->geom = insertedGeom;
				insert->geomName = cleanGeomName(insertedGeom);// Probably not needed anymore
				insert->mvpGeomName = mvpGeomName;
				insert->insert_x = point2.x;
				insert->insert_y = point2.y;
				insert->rotation = insertRotation;
				insert->stencilThickness = stencilThickness;
				insert->mirrorFlip = (mirror ^ data->getInsert()->getMirrorFlags()) & MIRROR_FLIP;
				insert->insertLayerIndex = layerIndex;
				insert->top = topHasPaste;
				insert->bot = botHasPaste;
				insert->ecad_att_board = attBoardname;
				insert->ecad_att_refdes = attRefdes;
				insert->ecad_att_pin = attPin;

				inserts.AddTail(insert);

				continue;
			}
		}

		doGeometry(insertedGeom, point2.x, point2.y, insertRotation, mirror ^ data->getInsert()->getMirrorFlags(), layerIndex);
	}
}


// doInserts
void doInserts()
{
	// Fiducials
	int fiducialNum = 1;

	CDataList& dataList = file->getBlock()->getDataList();
   POSITION dataPos = dataList.GetHeadPosition();
   while (dataPos)
   {
      const DataStruct* data = dataList.GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

		if (data->getInsert()->getInsertType() != INSERTTYPE_FIDUCIAL)
			continue;

		// Ref Des
		CString refDes;
		if (data->getInsert()->getRefname().GetLength() == 0)
			refDes.Format("Fid%d", fiducialNum++);
		else
			refDes = data->getInsert()->getRefname();
		
		// Geom
		BlockStruct *insertedGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());
		CString geomName = cleanGeomName(insertedGeom);

		// Output
		CString output;
		output.Format("%s\t%s\t%d\t%d\t%d\t%s\n", 
				refDes, 
				geomName,
				convertUnit(data->getInsert()->getOriginX(), unitFactor), 
				convertUnit(data->getInsert()->getOriginY(), unitFactor), 
				convertAngle(data->getInsert()->getAngle()),
				geomName);

		if (data->getInsert()->getPlacedTop())
			fprintf(topFile, output);
		else
			fprintf(botFile, output);
	}


	// Paste
	int pasteDepositNumber = 1;

   POSITION insertPos = inserts.GetHeadPosition();
   while (insertPos)
   {
      InsertedGeomStruct *insert = (InsertedGeomStruct*)inserts.GetNext(insertPos);

		// Default reference is pasteDepositNumber
		// Let the pasteDepositNumber keep on counting across each record, even
		// if a string ref is built from board/refdes/pin.
		CString ref;
		ref.Format("%d", pasteDepositNumber++);

		// Alternate reference is built from ECAD attributes.
		// Must have refdes and pin attribs to make alternative reference style, board can be blank
		if (!insert->ecad_att_refdes.IsEmpty() && !insert->ecad_att_pin.IsEmpty())
		{
			if (!insert->ecad_att_board.IsEmpty())
			{
				ref.Format(".BOARD%s.NAME%s.PIN%s",
					insert->ecad_att_board, insert->ecad_att_refdes, insert->ecad_att_pin);
			} else {
				ref.Format(".NAME%s.PIN%s",
					insert->ecad_att_refdes, insert->ecad_att_pin);
			}
		}

		CString output;
		output.Format("%s\t%s\t%d\t%d\t%d\t%s\n", 
				ref, 
				insert->mvpGeomName, 
				convertUnit(insert->insert_x, unitFactor), 
				convertUnit(insert->insert_y, unitFactor), 
				convertAngle(insert->rotation), 
				insert->mvpGeomName);

		if (insert->top)
			fprintf(topFile, output);
		if (insert->bot)
			fprintf(botFile, output);
	}
}


// doDefinitions
void doDefinitions()
{
	void *voidPtr;

	// Collect Unique Geometries
	CMapStringToPtr map;
   POSITION insertPos = inserts.GetHeadPosition();
   while (insertPos)
   {
      InsertedGeomStruct *insert = (InsertedGeomStruct*)inserts.GetNext(insertPos);
		if (!map.Lookup(insert->mvpGeomName, voidPtr))
			map.SetAt(insert->mvpGeomName, insert);
	}


	// Write Each Geometry Definition
	POSITION mapPos = map.GetStartPosition();
	while (mapPos)
	{
		CString mvpGeomName = "";
      void *voidPtr;
		map.GetNextAssoc(mapPos, mvpGeomName, voidPtr);
		InsertedGeomStruct *insert = (InsertedGeomStruct*)voidPtr;

		// Output
		CString output = generateApertureDefinition(doc, insert->geom, 1, insert->geom->getXoffset(), insert->geom->getYoffset(), insert->rotation, unitFactor, insert->stencilThickness);

		if (insert->top)
		{
			fprintf(topFile, ".%s\n", mvpGeomName);
			fprintf(topFile, output);
		}
		if (insert->bot)
		{
			fprintf(botFile, ".%s\n", mvpGeomName);
			fprintf(botFile, output);
		}
	}
}


// cleanGeomName
CString cleanGeomName(BlockStruct *geom)//, double radianRotation);
{
/*	int degrees = convertAngle(radianRotation);

	CString suffix = "";
	if (degrees)
		suffix.Format("_%d", degrees);*/

	// Generate Unique Name less than 31 Chars
	CString geomName = geom->getName().Left(31);// - suffix.GetLength()) + suffix;


	// Clean Illegal Chars
	for (int i=0; i<geomName.GetLength(); i++)
	{
		if (!__iscsym(geomName[i]))
			geomName.SetAt(i, '_');
	}

	return geomName;
}


// Layer Typing
bool checkLayerTyping(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

		if (layer == NULL)
			continue;

		if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
			return true;
		if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
			return true;
	}

	ErrorMessage("No layers with Layer Type Paste.", "", MB_OK);

	return false;
}


