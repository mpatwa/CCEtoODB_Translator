// $Header: /CAMCAD/5.0/Report.cpp 68    6/17/07 8:53p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Report.h"
#include "ccdoc.h"
#include "DirDlg.h"
#include "pcbutil.h"
#include "net_anl.h"
#include <float.h>
#include <math.h>
#include "attrib.h"
#include "extents.h"
#include "pcb_net.h"
#include "centroid.h"
#include "xform.h"
#include "find.h"
#include "net_util.h"
#include "RwUiLib.h"
#include "InFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  REPORTERR         "reports.log"

//extern   LayerTypeInfoStruct	layertypes[];

static char *report[] =
{
   "Manufacturing",
   "Components",
   "Geometries",
   "Layers",
   "Netlist",
   "Aperture",
   "Toollist",
   "Vialist",
   "LineLength",
   "TestAttributes",
   "THT Pin",
   "PinToPinLength",
   "TestProbe",
   "AdvancedPackaging",

//*** These reports are currently not implemented yet ***
   //"Files",
   //"Tracelist",
   //"Devicelist",
   //"Statistics",
};

typedef CTypedPtrArray<CPtrArray, CompPinStruct*> PinArray;

typedef struct
{
   CString netname;
   POSITION dataPos;
} RPTNetnameData;
typedef CTypedPtrArray<CPtrArray, RPTNetnameData*> NetnameDataArray;

enum EGeomUseAs
{
	useAsUnknown,
	useAsPCBComponent,
	useAsVia,
};

typedef struct
{
   CString  	name;
   int      	geomnr;
   bool      	smd;
   int      	totalused;           // loaded
   int      	total_not_used;      // unloaded
   int      	topused;             // loaded 
   int      	bottomused;
   int      	top_not_used;        // unloaded
   int      	bottom_not_used;
   int      	number_of_pins;
   int      	smd_pins;
   int      	thru_pins;
	EGeomUseAs	geomUseAs;
} RPTGeomData;
typedef CTypedPtrArray<CPtrArray, RPTGeomData*> GeomArray;

extern CSegmentArray *segmentArray; // from PCB_NET.CPP

static RPTGeomData* FindRPTGeomData(GeomArray& geomarray, int blockNum);

static int display_error = 0;
static CComponentOutSetting   commendSetting;
//******************************************************************************

CString GetLocaleListSeparator()
{
	CString localeData;
	{
		int length = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, NULL, 0);
		LPTSTR p = localeData.GetBuffer(length);
		VERIFY(::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, p, length));
	}
	localeData.ReleaseBuffer();

	return localeData;
}

CString GetListSeparator(bool reset = false)
{
   static CString sep;

   if (reset)
      sep.Empty();

   if (sep.IsEmpty())
      sep = GetLocaleListSeparator();

   return sep;
}

CString GetLocaleDecimalSeparator()
{
	CString localeData;
	{
		int length = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, NULL, 0);
		LPTSTR p = localeData.GetBuffer(length);
		VERIFY(::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, p, length));
	}
	localeData.ReleaseBuffer();

	return localeData;
}

CString GetDecimalSeparator(bool reset = false)
{
   static CString sep;

   if (reset)
      sep.Empty();

   if (sep.IsEmpty())
      sep = GetLocaleDecimalSeparator();

   return sep;
}

CString FormatDecimal(double value, int decimal_places)
{
   CString valstr;

   valstr.Format("%0.*f", decimal_places, value);

   valstr.Replace(".", GetDecimalSeparator());

   return valstr;
}

int Get_SMD_Pin_Count(CCEtoODBDoc *doc, CDataList *dataList)
{
   int smdPin = 0;

   POSITION pos = dataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *pinData = dataList->GetNext(pos);
      
      if (pinData->getDataType() != T_INSERT)
         continue;

      if (pinData->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      Attrib *pinAttrib = is_attvalue(doc, pinData->getAttributesRef(), ATT_SMDSHAPE, 1);
      if (pinAttrib)
         smdPin++;
   }

   return smdPin;
}

//******************************************************************************

static RPTGeomData* FindRPTGeomData(GeomArray& geomarray, int blockNum)
{
   for (int i=0; i<geomarray.GetCount(); i++)
   {
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL)
			continue;

		if (rptGeom->geomnr == blockNum)
			return rptGeom;
	}

	return NULL;
}


/*****************************************************************************
*

Number of Components LOADED,84,,,,
Number of Components NOT LOADED,12,( no p/n assigned ),,,
,,,,,
,,,,,

,Total TOP/BOT,Total TOP,Total BOTTOM,,
SMT Solder Joints,1400,1130,270,,
THRU Solder Joints,170,170,0,,
Total Solder Joints,1570,1300,270,,
,,,,,


,Total TOP/BOT,Total TOP,Total BOTTOM,
Number of SMT Components,75,60,15,
Number of THRU Components,9,9,0,
Total Number of Components,84,69,15,



,Total TOP/BOT,Total TOP,Total BOTTOM,
Number of SMT Components P/N,32,27,5,
Number of THRU Components P/N,1,1,0,
Total Number of Components P/N,33,28,5,



 List of Components Package,Total TOP/BOT,Total TOP,Total BOTTOM,Number of pins
0805,146,100,46,2
1206,184,184,0,2
QFP128,5,5,0,128
CONN54,9,9,0,54
SO8,15,12,3,8
BGA,3,3,0,452

*/


//******************************************************************************

static void GatherAllGeometries(CCEtoODBDoc* doc, GeomArray &geomarray)
{

   // Create geometries array by gathering all geometries
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block != NULL && 
			(block->getBlockType() == blockTypePcbComponent || block->getBlockType() == blockTypePadstack))
		{
			bool SMD = false;
			Attrib *attrib;
			if (attrib = is_attvalue(doc, block->getAttributesRef(), ATT_TECHNOLOGY, 1))
			{
				CString technology = attrib->getStringValue();
				if (technology.CompareNoCase("SMD") == 0)
					SMD = true;
			}
			else if (attrib = is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
			{
				SMD = true;
			}

			RPTGeomData *rptGeom = new RPTGeomData;
			geomarray.SetAtGrow(geomarray.GetCount(), rptGeom);
			rptGeom->name = block->getName();
			rptGeom->geomnr = block->getBlockNumber();
			rptGeom->totalused = 0;
			rptGeom->top_not_used = 0;
			rptGeom->topused = 0;
			rptGeom->bottomused = 0;
			rptGeom->bottom_not_used = 0;
			rptGeom->smd = SMD;
			rptGeom->number_of_pins = get_pincnt(&block->getDataList());
			rptGeom->smd_pins = Get_SMD_Pin_Count(doc, &block->getDataList());
			rptGeom->thru_pins = rptGeom->number_of_pins - rptGeom->smd_pins;
			rptGeom->geomUseAs = useAsUnknown;
		}
   }

}

//******************************************************************************

static void GatherPcbFileGeometriesInfo(CCEtoODBDoc *doc, FileStruct* file, GeomArray &geomarray, int *loaded, int *notloaded)
{

	*loaded = *notloaded = 0;

	if (file == NULL || file->getBlockType() != blockTypePcb)
		return;

	// Gather geometries information
	BlockStruct *fileblock = file->getBlock();
	if (fileblock != NULL)
	{
		POSITION dataPos = fileblock->getHeadDataInsertPosition();
		while (dataPos != NULL)
		{
			DataStruct* data = fileblock->getNextDataInsert(dataPos);
			if (data == NULL || data->getInsert() == NULL)
				continue;

			InsertStruct* insert = data->getInsert();
			if (insert->getInsertType() != insertTypePcbComponent && insert->getInsertType() != insertTypeVia)
				continue;

			RPTGeomData* rptGeom = FindRPTGeomData(geomarray, insert->getBlockNumber());
			if (rptGeom == NULL)
				continue;

			if (insert->getInsertType() == insertTypePcbComponent)
			{
				// Geometry used as PCB Components
				rptGeom->geomUseAs = useAsPCBComponent;

				BOOL CompLoaded = TRUE;  // Since CompLoaded is initialized to TRUE, if the cc file did not load any info from BOM then 
				// all components are treated as if they are all loaded
				Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_LOADED, 1);
				if (attrib)
				{
					CString loadedValue = attrib->getStringValue();

					if (!loadedValue.CompareNoCase("FALSE"))
						CompLoaded = FALSE;  
				}  

				if (CompLoaded)
				{
					(*loaded)++;
					rptGeom->totalused++;

					if (!insert->getPlacedBottom())
						rptGeom->topused++;
					else
						rptGeom->bottomused++;
				}
				else
				{
					(*notloaded)++;
					rptGeom->total_not_used++;

					if (!insert->getPlacedBottom())
						rptGeom->top_not_used++;
					else
						rptGeom->bottom_not_used++;
				}
			}
			else if (insert->getInsertType() == insertTypeVia && rptGeom->geomUseAs != useAsPCBComponent)
			{
				// Geometry used as vias
				rptGeom->geomUseAs = useAsVia;
				rptGeom->totalused++;
			}
		}
	}

}

//******************************************************************************

static int GetPinCount(BlockStruct *block)
{
   if (!block)
      return 0;

   int pinCount = 0;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
         pinCount++;
   }

   return pinCount;
}

//******************************************************************************

void getDrillInfo(CCEtoODBDoc *doc, DataStruct *data, double &size)
{
	size = 0.0;

	if (data->getDataType() != T_INSERT)
		return;

	if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
		return;

	BlockStruct *pinBlock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
	// find the drill size

	POSITION drillPos = pinBlock->getDataList().GetHeadPosition();
	while (drillPos)
	{
		DataStruct *drill = pinBlock->getDataList().GetNext(drillPos);

		if (drill->getDataType() != dataTypeInsert)
			continue;

		BlockStruct *drillBlock = doc->Find_Block_by_Num(drill->getInsert()->getBlockNumber());

		if (drill->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE || drillBlock->getBlockType() == BLOCKTYPE_DRILLHOLE)
		{
			size = drillBlock->getToolSize();
			return;
		}
	}
}


//******************************************************************************

static UINT64 GetPadstackLayerMap(CCEtoODBDoc *doc, int blockNum, int mirror, int maxStackup)
{
   UINT64 layerMap = 0;

   BlockStruct *padstackGeom = doc->getBlockAt(blockNum);

   POSITION dataPos = padstackGeom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = padstackGeom->getDataList().GetNext(dataPos);

      if (data->getLayerIndex() < 0)
         continue;

      int layerNum = data->getLayerIndex();
      if (mirror & MIRROR_LAYERS)
         layerNum = doc->getLayerArray()[layerNum]->getMirroredLayerIndex();

      LayerStruct *layer = doc->getLayerArray()[layerNum];
      UINT64 baseVal = 1L;

      if (layer->getElectricalStackNumber())
         layerMap |= baseVal << (layer->getElectricalStackNumber()-1);
      else
      {
         int i;
         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_ALL:
            for (i=0; i<=maxStackup; i++)
               layerMap |= baseVal << (i-1);
            break;
         case LAYTYPE_PAD_INNER:
            for (i=1; i<maxStackup; i++)
               layerMap |= baseVal << (i-1);
            break;
         case LAYTYPE_PAD_OUTER:
            layerMap |= baseVal << 0;
            layerMap |= baseVal << (maxStackup-1);
            break;
         case LAYTYPE_PAD_TOP:
            layerMap |= baseVal << 0;
            break;
         case LAYTYPE_PAD_BOTTOM:
            layerMap |= baseVal << (maxStackup-1);
            break;
         }
      }
   }

   return layerMap;
}

//******************************************************************************

void Report_StatNetListData(FILE *fReport, CCEtoODBDoc *doc, FileStruct *file)
{
   NetStruct   *net;
   POSITION    netPos;
   int         netcnt = 0;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
      {
      }
      else
      {
         netcnt++;
      }
   }
   fprintf(fReport,"Number of Nets, %d\n", netcnt);
}

//******************************************************************************

int GetBoardSize(CCEtoODBDoc *Doc, 
                        CDataList *DataList, double *xmin, double *ymin, double *xmax, double *ymax)
{
   DataStruct *np;

   *xmin = *ymin = FLT_MAX;
   *xmax = *ymax = -FLT_MAX;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      // if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_POLY:
            // found Panel Outline
            if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
            {
               ExtentRect extents;
               Mat2x2 m;
               RotMat2(&m, 0.0);
               
               // same as autoboard origin
               PolyExtents(Doc, np->getPolyList(), &extents, 1.0, 0, 0.0, 0.0, &m, FALSE);
               if (extents.left < *xmin) *xmin = extents.left;
               if (extents.right > *xmax) *xmax = extents.right;
               if (extents.bottom < *ymin) *ymin = extents.bottom;
               if (extents.top > *ymax) *ymax = extents.top;
            }
         break;
         case T_INSERT:
         {
         } // case INSERT
         break;
      } // end switch
   } // end check_comps */

   if (*xmin > *xmax)
      return 0;
   return 1;
}

//******************************************************************************

static void Report_WriteManufacturingReportData(FILE* wfp, CCEtoODBDoc* doc, FileStruct* file,
     double insert_xx, double insert_yx, double rotationx, int mirror, double scale)
{
	if (file == NULL || file->getBlockType() != blockTypePcb)
		return;

   int loaded = 0;
	int notloaded = 0; 
   GeomArray geomarray;
	geomarray.SetSize(0, 100);
	int i;

	GatherAllGeometries(doc, geomarray);

	GatherPcbFileGeometriesInfo(doc, file, geomarray, &loaded, &notloaded);

   CString sep = GetListSeparator();
   fprintf(wfp, "Number of Components LOADED%s%d%s%s%s%s\n", sep,loaded, sep, sep, sep, sep);
   fprintf(wfp, "Number of Components NOT LOADED%s%d%s%s%s%s\n", sep,notloaded, sep, sep, sep, sep);


	// Nets
	int nets = 0, multiPinNets = 0, singlePinNets = 0;
	POSITION netPos = file->getNetList().GetHeadPosition();
	while (netPos != NULL)
	{
		NetStruct *net = file->getNetList().GetNext(netPos);

		nets++;

		int pins = net->getCompPinCount();

		if (pins == 1)
			singlePinNets++;
		else if (pins > 1)
			multiPinNets++;
	}

   fprintf(wfp, "Number of Nets%s%d%s%s%s%s\n", sep, nets, sep, sep, sep, sep);
   fprintf(wfp, "Number of Single Pin Nets%s%d%s%s%s%s\n", sep, singlePinNets, sep, sep, sep, sep);
   fprintf(wfp, "Number of Multi Pin Nets%s%d%s%s%s%s\n", sep, multiPinNets, sep, sep, sep, sep);


	int electricalLayers = 0;
	double smallestTraceWidth = DBL_MAX;
	double smallestViaDiameter = DBL_MAX;
	double smallestPinPitch = DBL_MAX;

	// Count number of electrial layers
	for (i=0; i<doc->getMaxLayerIndex(); i++)
	{
		LayerStruct *layer = doc->getLayerAt(i);
		if (layer == NULL)
			continue;

		if (layer->getElectricalStackNumber() > 0)
			electricalLayers++;
	}

	// Get smallest width
   POSITION pos = file->getBlock()->getHeadDataPosition();
   while (pos != NULL)
   {
      DataStruct *data = file->getBlock()->getNextData(pos);
      if (data != NULL && data->getDataType() == dataTypePoly && data->getGraphicClass() != graphicClassEtch)
      {
         CPolyList *polyList = data->getPolyList();
         if (polyList != NULL)
         {
            POSITION polyPos = polyList->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = polyList->GetNext(polyPos);
               if (poly != NULL)
               {
                  BlockStruct *width = doc->getWidthBlock(poly->getWidthIndex());
                  if (width->getSizeA() < smallestTraceWidth) 
                     smallestTraceWidth = width->getSizeA();
               }
            }
         }
      }
   }
	if (smallestTraceWidth == DBL_MAX)
		smallestTraceWidth = 0.0;

	// Get smallest via
	pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos != NULL)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);
		if (data == NULL || data->getInsert() == NULL)
			continue;

		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypeVia)
			continue;

		BlockStruct *insertBlock = doc->getBlockAt(insert->getBlockNumber());
		if (insertBlock == NULL)
			continue;

		POSITION subPos = insertBlock->getHeadDataInsertPosition();
		while (subPos != NULL)
		{
			DataStruct *subData = insertBlock->getNextDataInsert(subPos);
			if (subData == NULL || subData->getInsert() == NULL)
				continue;

			BlockStruct *aperture = doc->getBlockAt(subData->getInsert()->getBlockNumber());
			if (aperture == NULL || aperture->isAperture() == false)
				continue;

			if (aperture->getSizeA() < smallestViaDiameter)
				smallestViaDiameter = aperture->getSizeA();
		}
	}
	if (smallestViaDiameter == DBL_MAX)
		smallestViaDiameter = 0.0;

	// Get smallest pin pitch
	WORD pinPitchKey = (WORD)doc->RegisterKeyWord(ATT_PIN_PITCH, 0, VT_UNIT_DOUBLE);
	for (i=0; i<doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);
		if (block == NULL || block->getAttributes() == NULL)
			continue;

		Attrib *attrib = NULL;
		if (block->getAttributes()->Lookup(pinPitchKey, attrib) && attrib != NULL)
		{
			if (attrib->getDoubleValue() < smallestPinPitch)
				smallestPinPitch = attrib->getDoubleValue();
		}
	}
	if (smallestPinPitch == DBL_MAX)
		smallestPinPitch = 0.0;

   fprintf(wfp, "Number of Electrical Layers%s%d%s%s%s%s\n", sep,electricalLayers, sep, sep, sep, sep);
   fprintf(wfp, "Smallest Trace Width%s%s%s%s%s%s\n", sep,FormatDecimal(smallestTraceWidth), sep, sep, sep, sep);
   fprintf(wfp, "Smallest Via Diameter%s%s%s%s%s%s\n", sep,FormatDecimal(smallestViaDiameter), sep, sep, sep, sep);
   fprintf(wfp, "Smallest Pin Pitch%s%s%s%s%s%s\n", sep, FormatDecimal(smallestPinPitch), sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);


   // solderjoints
   int smdtop, smdbot, thrutop, thrubot;
   smdtop = smdbot = thrutop = thrubot = 0;

   for (i=0; i<geomarray.GetCount(); i++)
   {
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL || rptGeom->geomUseAs != useAsPCBComponent)
			continue;
      if (rptGeom->totalused == 0)
         continue;

      smdtop += rptGeom->topused * rptGeom->smd_pins;
      smdbot += rptGeom->bottomused * rptGeom->smd_pins;
      thrutop += rptGeom->topused * rptGeom->thru_pins;
      thrubot += rptGeom->bottomused * rptGeom->thru_pins;
   }

   fprintf(wfp, sep + "Total TOP/BOT" + sep + "Total TOP" + sep + "Total BOTTOM" + sep + "\n");
   fprintf(wfp, "SMT Solder Joints,%d,%d,%d%s%s\n", smdtop + smdbot, smdtop, smdbot, sep, sep);
   fprintf(wfp, "THRU Solder Joints,%d,%d,%d%s%s\n", thrutop + thrubot, thrutop, thrubot, sep, sep);
   fprintf(wfp, "Total Solder Joints,%d,%d,%d%s%s\n", smdtop + smdbot + thrutop + thrubot, smdtop + thrutop, smdbot + thrubot, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);


   // Number of components not loaded
   smdtop = smdbot = thrutop = thrubot = 0;
   smdtop = smdbot = thrutop = thrubot = 0;
   for (i=0; i<geomarray.GetCount(); i++)
   {
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL || rptGeom->geomUseAs != useAsPCBComponent)
			continue;

		if (rptGeom->smd)
      {
         smdtop += rptGeom->top_not_used;
         smdbot += rptGeom->bottom_not_used;
      }
      else
      {
         thrutop += rptGeom->top_not_used;
         thrubot += rptGeom->bottom_not_used;
      }
   }

   fprintf(wfp, sep, "Total TOP/BOT" + sep + "Total TOP" + sep + "Total BOTTOM" + sep + "\n");
   fprintf(wfp, "Number of SMT not loaded Components%s%d%s%d%s%d%s\n",sep, smdtop+smdbot, sep, smdtop, sep, smdbot, sep);
   fprintf(wfp, "Number of THRU not loaded Components%s%d%s%d%s%d%s\n", sep, thrutop+thrubot, sep, thrutop, sep, thrubot, sep);
   fprintf(wfp, "Total Number of not loaded Components%s%d%s%d%s%d%s\n", sep, smdtop+smdbot+thrutop+thrubot, sep,  smdtop+thrutop, sep,  smdbot+thrubot, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);



   // Nubmer of components loaded
   smdtop = smdbot = thrutop = thrubot = 0;
   smdtop = smdbot = thrutop = thrubot = 0;
   for (i=0; i<geomarray.GetCount(); i++)
   {
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL || rptGeom->geomUseAs != useAsPCBComponent)
			continue;

      if (rptGeom->smd)
      {
         smdtop += rptGeom->topused;
         smdbot += rptGeom->bottomused;
      }
      else
      {
         thrutop += rptGeom->topused;
         thrubot += rptGeom->bottomused;
      }
   }

   fprintf(wfp, sep + "Total TOP/BOT" + sep + "Total TOP" + sep + "Total BOTTOM" + sep + "\n");
   fprintf(wfp, "Number of SMT loaded Components%s%d%s%d%s%d%s\n", sep,  smdtop+smdbot,sep,  smdtop, sep,  smdbot, sep);
   fprintf(wfp, "Number of THRU loaded Components%s%d%s%d%s%d%s\n", sep, thrutop+thrubot, sep,  thrutop, sep, thrubot, sep);
   fprintf(wfp, "Total Number of loaded Components%s%d%s%d%s%d%s\n", sep, smdtop+smdbot+thrutop+thrubot,sep, smdtop+thrutop, sep, smdbot+thrubot, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);


   // Number of component packages
   smdtop = smdbot = thrutop = thrubot = 0;
   for (i=0; i<geomarray.GetCount(); i++)
   {
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL || rptGeom->geomUseAs != useAsPCBComponent)
			continue;
      if (rptGeom->totalused == 0)
         continue;

      if (rptGeom->smd)
      {
         if (rptGeom->topused)
            smdtop++;
         if (rptGeom->bottomused)
            smdbot++;
      }
      else
      {
         if (rptGeom->topused)
            thrutop++;
         if (rptGeom->bottomused)
            thrubot++;
      }
   }

   fprintf(wfp, sep + "Total TOP/BOT" + sep + "Total TOP" + sep + "Total BOTTOM" + sep + "\n");
   fprintf(wfp, "Number of SMT Components Package%s%d%s%d%s%d%s\n", sep, smdtop+smdbot, sep, smdtop, sep, smdbot, sep);
   fprintf(wfp, "Number of THRU Components Package%s%d%s%d%s%d%s\n", sep, thrutop+thrubot, sep, thrutop, sep, thrubot, sep);
   fprintf(wfp, "Total Number of Components Package%s%d%s%d%s%d%s\n", sep, smdtop+smdbot+thrutop+thrubot, sep,  smdtop+thrutop, sep, smdbot+thrubot, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);



   // List of component packages
   fprintf(wfp, "List of Components Package" + sep + "Total TOP/BOT" + sep + "Total TOP" + sep + "Total BOTTOM" + sep + "Number of pins"  + "\n");
   for (i=0; i<geomarray.GetCount(); i++)
   {
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL || rptGeom->geomUseAs != useAsPCBComponent)
			continue;
      if (rptGeom->totalused == 0)
         continue;

      fprintf(wfp, "%s%s%d%s%d%s%d%s%d\n", 
            csv_string(rptGeom->name),sep, rptGeom->totalused, sep, rptGeom->topused, sep, rptGeom->bottomused, sep, rptGeom->number_of_pins);
   }
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);



	// List of via geometries
   fprintf(wfp, "List of via Geom" + sep + "Number of vias" + sep + "Drill Size" + sep + "Pad Top Diameter" + sep + "Pad Bottom Diameter" + sep + "Mask Top Diameter" + sep + "Mask Bottom Diameter\n");
	for (i=0; i<geomarray.GetCount(); i++)
	{
		RPTGeomData* rptGeom = geomarray[i];
		if (rptGeom == NULL || rptGeom->geomUseAs != useAsVia)
			continue;
      if (rptGeom->totalused == 0)
         continue;

		BlockStruct* padstack = doc->getBlockAt(rptGeom->geomnr);
		if (padstack == NULL)
			continue;

		double drillSize = -1;
		double padTopDiameter = -1;
		double padBottomDiameter = -1;
		double maskTopDiameter = -1;
		double maskBottomDiameter = -1;

		POSITION pos = padstack->getHeadDataInsertPosition();
		while (pos != NULL)
		{
			DataStruct* padData = padstack->getNextDataInsert(pos);
			if (padData == NULL || padData->getInsert() == NULL)
				continue;

			InsertStruct* insert = padData->getInsert();
			BlockStruct* padBlock = doc->getBlockAt(insert->getBlockNumber());
			if (padBlock == NULL)
				continue;
			
			if (padBlock->isDrillHole())
			{
				drillSize = padBlock->getToolSize();
				continue;
			}

			if (!padBlock->isAperture())
				continue;

			LayerTypeTag layerType = doc->getLayerAt(padData->getLayerIndex())->getLayerType();
			double diameter = padBlock->getSizeA();

			if (layerType == layerTypePadAll || layerType == layerTypeSignalAll)
			{
				if (padTopDiameter < diameter)
					padTopDiameter = diameter;
				if (padBottomDiameter < diameter)
					padBottomDiameter = diameter;
			}
			else if (layerType == layerTypePadTop || layerType == layerTypeSignalTop)
			{
				if (padTopDiameter < diameter)
					padTopDiameter = diameter;
			}
			else if (layerType == layerTypePadBottom || layerType == layerTypeSignalBottom)
			{
				if (padBottomDiameter < diameter)
					padBottomDiameter = diameter;
			}
			else if (layerType == layerTypeMaskAll)
			{
				if (maskTopDiameter < diameter)
					maskTopDiameter = diameter;
				if (maskBottomDiameter < diameter)
					maskBottomDiameter = diameter;
			}
			else if (layerType == layerTypeMaskTop)
			{
				if (maskTopDiameter < diameter)
					maskTopDiameter = diameter;
			}
			else if (layerType == layerTypeMaskBottom)
			{
				if (maskBottomDiameter < diameter)
					maskBottomDiameter = diameter;
			}
		}

		CString drillSizeStr, padTopDiameterStr, padBottomDiameterStr, maskTopDiameterStr, maskBottomDiameterStr;
		if (drillSize > -1)
			drillSizeStr = FormatDecimal(drillSize);

		if (padTopDiameter > -1)
			padTopDiameterStr = FormatDecimal(padTopDiameter);

		if (padBottomDiameter > -1)
			padBottomDiameterStr = FormatDecimal(padBottomDiameter);

		if (maskTopDiameter > -1)
			maskTopDiameterStr = FormatDecimal(maskTopDiameter);

		if (maskBottomDiameter > -1)
			maskBottomDiameterStr = FormatDecimal(maskBottomDiameter);

      fprintf(wfp, "%s%s%d%s%s%s%s%s%s%s%s%s%s\n",	csv_string(rptGeom->name), sep,  rptGeom->totalused,sep, 
				drillSizeStr,sep,  padTopDiameterStr, sep, padBottomDiameterStr, sep, maskTopDiameterStr, sep, maskBottomDiameterStr);
	}			
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);
   fprintf(wfp, "%s%s%s%s\n", sep, sep, sep, sep);



   for (i=0; i<geomarray.GetCount(); i++)
      delete geomarray[i];
   geomarray.RemoveAll();
} 

//******************************************************************************

static void Report_WriteManufacturingReportPcb(FILE *fReport, CCEtoODBDoc *doc, FileStruct *pcbfile)
{
	if (fReport != NULL && doc != NULL && pcbfile != NULL)
	{
		CTime t;
		t = t.GetCurrentTime();
		CString sep = GetListSeparator();   // get the separator list from the locale settings
		fprintf(fReport,"BOARD ASSEMBLY QUOTATION REPORT" + sep + sep + sep + sep + "Date %s,\n",  t.Format("%d-%B-%Y"));
		//fprintf(fReport,",,,,,\n");
		fprintf(fReport, "%s%s%s%s%s\n", sep, sep, sep, sep, sep);
		fprintf(fReport, "%s%s%s%s%s\n", sep, sep, sep, sep, sep);
		fprintf(fReport,"PROJECT,%s%s%s%s%s\n", doc->GetProjectPath(), sep, sep, sep, sep);
		fprintf(fReport,"CAD FILE,%s%s%s%s%s\n", pcbfile->getName(), sep, sep, sep, sep);

		double width = 0.0, height = 0.0;
		int decimals = GetDecimals(doc->getSettings().getPageUnits());

		double   xmin, ymin, xmax, ymax;

		if (GetBoardSize(doc, &(pcbfile->getBlock()->getDataList()),&xmin, &ymin, &xmax, &ymax))
		{
			width  = xmax - xmin;
			height = ymax - ymin;
		}
		else
		{
			doc->CalcBlockExtents(pcbfile->getBlock());
			width  = pcbfile->getBlock()->getXmax() - pcbfile->getBlock()->getXmin();
			height = pcbfile->getBlock()->getYmax() - pcbfile->getBlock()->getYmin();
		}

		fprintf(fReport,"PCB DIMENSION%s%sx%s,(%s)%s%s%s\n", sep,
         FormatDecimal(width, decimals), FormatDecimal(height, decimals), 
			GetUnitName(doc->getSettings().getPageUnits()), sep, sep, sep);

		Report_WriteManufacturingReportData(fReport, doc, pcbfile, /*file->getBlock(),*//* &(file->getBlock()->getDataList()), */
			pcbfile->getInsertX(), pcbfile->getInsertY(), pcbfile->getRotation(),  pcbfile->isMirrored(), pcbfile->getScale());

		fprintf(fReport,"\n");
	}
}

//******************************************************************************

static CString Report_WriteManufacturingReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "manufacturing.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
			POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
				file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)
					{
						Report_WriteManufacturingReportPcb(fReport, doc, file);
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								Report_WriteManufacturingReportPcb(fReport, doc, pcbFile);
							}
						}
					}

				}
         }

         fclose(fReport);
			return reportFile;
		}
	}
	return "";
}

//******************************************************************************

void Report_WriteComponentData(FILE *wfp, CCEtoODBDoc& doc, CDataList& dataList, const CTMatrix transformMatrix, const CString refdesPrefx, const bool suppressHeader)
{
   //Mat2x2 m;
   //RotMat2(&m, rotation);
   CString sep = GetListSeparator();   // get the separator list from the locale settings

   if (!suppressHeader)
   {
      fprintf(wfp,"Refdes" + sep + "Device/Type" + sep + "Partnumber" + sep + "Geometry" + sep +  "GridLocation"  + sep + "X"  + sep + "Y"  + sep + "Mirror" + sep + "Rotation" + sep + "SMD/THRU" + sep + "PinCount" + sep + "CentroidX" + sep + "CentroidY" + sep + "CentroidRotation" + sep + "LOADED");
      for(int i = 0, columnSize =  commendSetting.getcompsColumnHeaders().GetCount(); i < columnSize; i++)
      {
         CString compsColumnHeadersItr = commendSetting.getcompsColumnHeaders().GetAt(i);
         fprintf(wfp,sep + compsColumnHeadersItr);
      }
      fprintf(wfp,"\n");
   }

   bool option_allquote = commendSetting.getOptionQuoteAllFields();
	for (CDataListIterator componentIterator(dataList, insertTypePcbComponent); componentIterator.hasNext();)
	{
		DataStruct *data = componentIterator.getNext();
      BlockStruct* block = doc.getBlockAt(data->getInsert()->getBlockNumber());

      CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
      insertBasesVector.transform(transformMatrix);
      
      Attrib *attrib;                                                        
      CString device = "";
      CString part_number = "";
      CString grid_location = "";
      CString geometry = csv_string(block->getName(),option_allquote);
      CString compname = csv_string(refdesPrefx + data->getInsert()->getRefname(),option_allquote);
      CString technology = "";
		CString loaded = "TRUE";

      if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1))
         device = csv_string(doc.getAttributeStringValue(attrib),option_allquote);

      if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_PARTNUMBER, 1))
         part_number = csv_string(doc.getAttributeStringValue(attrib),option_allquote);

      if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_GRID_LOCATION, 1))
         grid_location = csv_string(doc.getAttributeStringValue(attrib),option_allquote);

      if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_TECHNOLOGY, 1))
      {
         technology = csv_string(doc.getAttributeStringValue(attrib));         
      }

      if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_LOADED, 1))
		{
			CString value = csv_string(doc.getAttributeStringValue(attrib),option_allquote);
         if (value.CompareNoCase("false") == 0)
				loaded = "FALSE";
		}

      int pinCount = block->getPinCount();

      fprintf(wfp, "%s%s%s%s%s%s%s%s%s%s", compname, sep, device, sep, part_number, sep, geometry, sep, grid_location, sep);
      fprintf(wfp, "%s%s%s%s", FormatDecimal(insertBasesVector.getX(), 5), sep, FormatDecimal(insertBasesVector.getY(), 5), sep);
      fprintf(wfp, "%s%s", data->getInsert()->getPlacedBottom()?"BOTTOM":"TOP", sep); 
      fprintf(wfp, "%d%s", normalizeDegrees(round(insertBasesVector.getRotationDegrees())), sep);
      fprintf(wfp, "%s", technology);
      fprintf(wfp, "%s%d%s", sep, pinCount, sep);

      // if centroid
      DataStruct *centroid = block->GetCentroidData();
      if (centroid)
      {
         CBasesVector centroidBasesVector = centroid->getInsert()->getBasesVector();
         centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());

         // Round first, then normalize. If you normalize first then round, then (for example) 
         // 359.9998 will pass the 0 <= angle < 360 test, but upon rounding will become 360
         // so suddenly it is bumped out of desired range.
         int normCentAngle = normalizeDegrees(round(centroidBasesVector.getRotationDegrees()));

         fprintf(wfp,"%s%s%s%s%d", FormatDecimal(centroidBasesVector.getX()), sep,  FormatDecimal(centroidBasesVector.getY(), 5), sep, normCentAngle);
      }  
      else
      {
         fprintf(wfp, "%s%s", sep, sep);
      }

      
      fprintf(wfp, "%s%s", sep, loaded);

      for(int i = 0, columnSize =  commendSetting.getcompsColumnHeaders().GetCount() ; i < columnSize; i++)
      {
         CString compsColumnHeadersItr = commendSetting.getcompsColumnHeaders().GetAt(i);
         fprintf(wfp,"%s",sep);
         if (attrib = is_attvalue(&doc, data->getAttributesRef(), compsColumnHeadersItr, 1))
         {
            CString value = csv_string(doc.getAttributeStringValue(attrib));
            fprintf(wfp,"%s",value);
         }
      }
      fprintf(wfp,"\n");
   }
}

//******************************************************************************

static CString Report_WriteComponentReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "comps.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         commendSetting.LoadCompsReportSettings(getApp().getUserPath() + "Components.out");
			POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						// vias and generic and mechanical components
						Report_WriteComponentData(fReport, *doc, file->getBlock()->getDataList(), file->getTMatrix(), "", false);
					}
					else if (file->getBlockType() == blockTypePanel)
					{
                  bool suppressHeader = false;
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
                        CTMatrix pcbMatrix = pcbInsert->getTMatrix() * pcbFile->getTMatrix() * file->getTMatrix();                        
								Report_WriteComponentData(fReport, *doc, pcbFile->getBlock()->getDataList(), pcbMatrix, pcbInsert->getRefname() + "_", suppressHeader);
                        suppressHeader = true;
							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static CString Report_WriteGeometriesReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString sep = GetListSeparator();   // get the separator list from the locale settings

	  CString reportFile = directory;
      reportFile += "geoms.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         fprintf(fReport, "Geometry Name" + sep + "Number" + sep + "Type\n");

         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);
            if (block != NULL)
				{
					fprintf(fReport, "%s", csv_string(block->getName()));
					fprintf(fReport, "%s%d", sep, j);
					fprintf(fReport, "%s%s", sep,  blockTypeToDisplayString(block->getBlockType()));
					fprintf(fReport, "\n");
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static CString Report_WriteLayersReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
	  CString sep = GetListSeparator();   // get the separator list from the locale settings
      CString reportFile = directory;
      reportFile += "layers.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]",reportFile);
         MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         fprintf(fReport,"Layer Name" + sep + "Comment" + sep + "Z Height" + sep + "Layer Type" + sep + "Artwork #" + sep + "Electrical #" + sep + "Physical #\n");

         for (int j=0; j< doc->getMaxLayerIndex(); j++)   // getMaxLayerIndex() is the exact number of layers, not
                                                   // the next index.
         {
            LayerStruct *layer;
            layer = doc->getLayerArray()[j];
            if (layer == NULL)   
				continue;
            fprintf(fReport,"%s",   csv_string(layer->getName()));
            fprintf(fReport,"%s%s", sep, csv_string(layer->getComment()));
            fprintf(fReport,"%s%s", sep, FormatDecimal(layer->getZHeight(), 5));
            fprintf(fReport,"%s%s", sep, layerTypeToString(layer->getLayerType()));
            fprintf(fReport,"%s%d", sep, layer->getArtworkStackNumber());
            fprintf(fReport,"%s%d", sep, layer->getElectricalStackNumber());
            fprintf(fReport,"%s%d", sep, layer->getPhysicalStackNumber());
            
            fprintf(fReport,"\n");
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WriteNetListData(FILE *fReport, CCEtoODBDoc *doc, FileStruct *file)
{
   NetStruct   *net;
   POSITION    netPos;
   CString     netname;
   CString sep = GetListSeparator();   // get the separator list from the locale settings

   fprintf(fReport,"NetName" + sep + "Component"  + sep + "PinName\n");

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if ((net->getFlags() & NETFLAG_UNUSEDNET) && !(net->getFlags() & NETFLAG_SINGLEPINNET))
         netname = "UNUSED";
      else
         netname = net->getNetName();
   
      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         
         CString  n, c, p;
         n = csv_string(netname);
         c = csv_string(compPin->getRefDes());
         p = csv_string(compPin->getPinName());
         fprintf(fReport,"%s%s%s%s%s\n", n, sep, c, sep, p);
      }
   }
	fprintf(fReport,"\n");
}

//******************************************************************************

static CString Report_WriteNetlistReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "nets.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         POSITION filePos = doc->getFileList().GetHeadPosition();
         while (filePos)
         {
            FileStruct *file = doc->getFileList().GetNext(filePos);

            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)
					{
						Report_WriteNetListData(fReport, doc, file);
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								Report_WriteNetListData(fReport, doc, pcbFile);
							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static CString Report_WriteApertureReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "aperture.csv";
	  CString sep = GetListSeparator();   // get the separator list from the locale settings

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         fprintf(fReport,"Aperture Name"  + sep + "Dcode" + sep + "Form" + sep + "Height" + sep + "Width" + sep + "X Offset" + sep + "Y Offset" + sep + "Rotation\n");

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);
            if (block != NULL)
				{
					// create necessary aperture blocks
					if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
					{
						if (block->getShape() == T_COMPLEX)
						{
							BlockStruct *subblock = doc->Find_Block_by_Num(block->getComplexApertureSubBlockNumber());
							CString  bname = csv_string(block->getName());
							CString  sname = csv_string(subblock->getName());
							fprintf(fReport,"%s%s%d%s%s%s%s%s%s%s%s %s%d\n",
								bname,sep, block->getDcode(),sep, "COMPLEX", sep, sname, sep,  
								FormatDecimal(block->getXoffset(), 5), sep, 
                        FormatDecimal(block->getYoffset(), 5), sep, 
                        round(RadToDeg(block->getRotation())));
						}
						else
						{
							char  *form;
							switch (block->getShape())
							{
							case T_ROUND:
								form = "ROUND";
								break;
							case T_SQUARE:
								form = "SQUARE";
								break;
							case T_RECTANGLE:
								form = "RECTANGLE";
								break;
							case T_TARGET:
								form = "TARGET";
								break;
							case T_THERMAL:
								form = "THERMAL";
								break;
							case T_DONUT:
								form = "DONUT";
								break;
							case T_OCTAGON:
								form = "OCTAGON";
								break;
							case T_OBLONG:
								form = "OBLONG";
								break;
							case T_BLANK:
								form = "BLANK";
								break;
							default:
								form = "UNDEFINED";
								break;
							}
							fprintf(fReport, "%s%s%d%s%s%s%s%s%s%s%s%s%s%s%d\n",
								csv_string(block->getName()), sep, block->getDcode(), sep, form, sep, 
                        FormatDecimal(block->getSizeA(), 5), sep, FormatDecimal(block->getSizeB(), 5), sep,
								FormatDecimal(block->getXoffset(), 5), sep, FormatDecimal(block->getYoffset(), 5), sep,
                        round(RadToDeg(block->getRotation())));
						}
					}
				}
			}

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static CString Report_WriteToolReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString sep = GetListSeparator();   // get the separator list from the locale settings
	  CString reportFile = directory;
      reportFile += "tools.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         fprintf(fReport,"Tool Name" + sep + "Tcode" + sep + "Size\n");

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);
            if (block != NULL)
				{
					if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
					{
						fprintf(fReport,"%s%s%d%s%s\n",
                     block->getName(), sep, block->getTcode(), sep, FormatDecimal(block->getToolSize(), 5));
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WriteViaListData(FILE *wfp, CCEtoODBDoc *doc, BlockStruct *fileblock, CDataList *DataList, 
                              double insert_x, double insert_y,
                              double rotation, int mirror, double scale)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   CString sep = GetListSeparator();   // get the separator list from the locale settings
	  
   RotMat2(&m, rotation);

   fprintf(wfp,"ViaName" + sep + "NetName" + sep + "Geometry" + sep + "X" + sep + "Y" + sep + "Access\n");
   int   vianamecnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_VIA)   continue;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // block->getRotation() is the aperture rotation 
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.
            

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               CString  vianame = np->getInsert()->getRefname();
               CString  geometry = block->getName();
               CString  netname;

               netname = "";
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
               if (a)
                  netname = get_attvalue_string(doc, a);

               if (strlen(vianame) == 0)
               {
                  DataStruct *d = NULL;
                  // make a vianame via%d, but make sure it does not exist yet.
                  do
                  {
                     vianame.Format("via%d", ++vianamecnt);
                  } while (d = datastruct_from_refdes(doc, fileblock, vianame));
                  np->getInsert()->setRefname(STRDUP(vianame));
               }

               vianame = csv_string(vianame);
               netname = csv_string(netname);
               geometry = csv_string(geometry);

               fprintf(wfp,"%s%s%s%s%s%s",vianame, sep, netname,sep, geometry, sep);

               // x y 
               fprintf(wfp,"%s%s%s%s", FormatDecimal(point2.x, 5), sep, FormatDecimal(point2.y, 5), sep);

               // manufacturing layer a padstack exist on, top, bottom, all ,none
               if ((block->getFlags() & BL_ACCESS_OUTER) == BL_ACCESS_OUTER)
                  fprintf(wfp,"BOTH");
               else
               if (block->getFlags() & BL_ACCESS_BOTTOM)
                  fprintf(wfp,"BOTTOM");
               else
               if (block->getFlags() & BL_ACCESS_TOP)
                  fprintf(wfp," TOP ");
               else
               if (block->getFlags() & BL_ACCESS_NONE)
                  fprintf(wfp,"NONE");
               else
                  fprintf(wfp,"UNKNOWN");

               fprintf(wfp,"\n");
            }
         } // case INSERT
         break;
      } // end switch
   } // end for
	fprintf(wfp,"\n");
}

//******************************************************************************

static CString Report_WriteViaListReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "vialist.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]",reportFile);
         MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						generate_PADSTACKACCESSFLAG(doc, 0);
						Report_WriteViaListData(fReport, doc, file->getBlock(), &(file->getBlock()->getDataList()), 
							file->getInsertX(), file->getInsertY(), file->getRotation(), file->isMirrored(), file->getScale());
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								generate_PADSTACKACCESSFLAG(doc, 0);
								Report_WriteViaListData(fReport, doc, pcbFile->getBlock(), &(pcbFile->getBlock()->getDataList()), 
									pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale());
					
							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WriteLineLengthData(FILE *wfp, CCEtoODBDoc *doc, FileStruct *file, BlockStruct *fileblock, CDataList *DataList, 
      double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   CString sep = GetListSeparator();   // get the separator list from the locale settings
   fprintf(wfp, "Net Name" + sep + "Manhattan Distance" + sep + "Actual Length" + sep + "Difference" + sep + "%% Difference" + sep + "Qty Pins" + sep + "Qty Vias" + sep + "Min. Width" + sep + "Max. Width" + sep + "Units\n");

   // store datas with netnames
   NetnameDataArray netnameDataArray;
   int netnameDataCnt = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      POSITION tempPos = dataPos;

      DataStruct *data = DataList->GetNext(dataPos);

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
      if (attrib)
      {
         CString netname = get_attvalue_string(doc, attrib);

         RPTNetnameData *n = new RPTNetnameData;
         netnameDataArray.SetAtGrow(netnameDataCnt, n);
         n->netname = netname;
         n->dataPos = tempPos;
         netnameDataCnt++;
      }
   }


   // run through the netlist and collect all netnames.
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)   
         continue;
   
      BOOL powernet = FALSE;

      if (is_attvalue(doc, net->getAttributesRef(), ATT_POWERNET, 1))
         powernet = TRUE;
      
      PinArray pinArray;
      pinArray.SetSize(20, 20);
      int pinCnt = 0;

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (compPin->getPinCoordinatesComplete())
         {
            pinArray.SetAtGrow(pinCnt, compPin);
            pinCnt++;
         }
      }

      if (!powernet)
      {
         // now find all traces and vias, width and layers, 
         double manhattanLength = 0;
         double actualLength = 0;
         int viaCnt = 0;
         double minwidth = -1, maxwidth = -1;
         
         // here do traces and vias
			int i=0;
         for (i=0; i<netnameDataCnt; i++)
         {
            if (strcmp(netnameDataArray[i]->netname, net->getNetName()))  
               continue;

            DataStruct *data = DataList->GetNext(netnameDataArray[i]->dataPos);

            switch (data->getDataType())
            {
               case T_POLY:
               {
                  POSITION polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos)
                  {
                     CPoly *poly = data->getPolyList()->GetNext(polyPos);

                     if (poly->isHidden() || poly->isClosed())   
                        continue;
   
                     double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale;

                     if (minwidth < 0) 
                        minwidth = lineWidth;
                     else if (minwidth > lineWidth)
                        minwidth = lineWidth;

                     if (maxwidth < 0) 
                        maxwidth = lineWidth;
                     else if (maxwidth < lineWidth)
                        maxwidth = lineWidth;

                     BOOL first = TRUE;
                     double x1, y1, b1, x2, y2, b2;
                     POSITION pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        CPnt *pnt = poly->getPntList().GetNext(pntPos);
                        x2 = pnt->x * scale;
                        y2 = pnt->y * scale;
                        b2 = pnt->bulge;
   
                        if (!first)
                           actualLength += NetLength_Segment(doc, x1, y1, x2, y2, b1);
                        first = FALSE;

                        x1 = x2;
                        y1 = y2;
                        b1 = b2;
                     }
                  }
               }
               break;

               case T_INSERT:
               {
                  if (data->getInsert()->getInsertType() == INSERTTYPE_VIA) 
                     viaCnt++;
               }
               break;
            } // end switch
         } // while

         // here calc manhattanLength
         for (i=0; i<pinCnt; i++)
         {
            double minlength = FLT_MAX;

            for (int ii=i+1; ii<pinCnt; ii++)
            {
               double len = fabs(pinArray[i]->getOriginX() - pinArray[ii]->getOriginX()) + 
                            fabs(pinArray[i]->getOriginY() - pinArray[ii]->getOriginY());
               if (len < minlength)
                  minlength = len;
            }
            if (minlength < FLT_MAX)
               manhattanLength += minlength;
         }

         int percent;
         if (fabs(actualLength) > 0 && fabs(manhattanLength) > 0)
            percent = round((actualLength / manhattanLength)*100);
         else
            percent = 0;

         fprintf(wfp, "%s%s", csv_string(net->getNetName()), sep);
         fprintf(wfp, "%s%s", FormatDecimal(manhattanLength, decimals), sep);
         fprintf(wfp, "%s%s", FormatDecimal(actualLength, decimals), sep);
         fprintf(wfp, "%s%s", FormatDecimal(actualLength - manhattanLength, decimals), sep);
         fprintf(wfp, "%d%s%d%s%d,", percent, sep, pinCnt, sep, viaCnt);
         fprintf(wfp, "%s%s", FormatDecimal(minwidth, decimals), sep);
         fprintf(wfp, "%s%s", FormatDecimal(maxwidth, decimals), sep);
         fprintf(wfp, "%s\n", GetUnitName(doc->getSettings().getPageUnits()));
      }
      else
      {
         // powernet
         fprintf(wfp, "%s%sPOWERNET%s%s%s%s%d%s%s%s%s\n", csv_string(net->getNetName()), sep, sep, sep, sep, sep, pinCnt, sep, sep, sep, sep);
      }

      pinArray.RemoveAll();   
   }

   fprintf(wfp, "\n");

   for (int i=0; i<netnameDataCnt; i++)
      delete netnameDataArray[i];
}

//******************************************************************************

static CString Report_WriteLineLengthReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "linelength.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						generate_PINLOC(doc, file, 0);  // this function generates the PINLOC argument for all pins.
						Report_WriteLineLengthData(fReport, doc, file, file->getBlock(), &(file->getBlock()->getDataList()), 
							file->getInsertX(), file->getInsertY(),file->getRotation(),  file->isMirrored(), file->getScale());
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								generate_PINLOC(doc, pcbFile, 0);  // this function generates the PINLOC argument for all pins.
								Report_WriteLineLengthData(fReport, doc, pcbFile, pcbFile->getBlock(), &(pcbFile->getBlock()->getDataList()), 
									pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale());
					

							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WriteTestAttributeReportData(FILE *wfp, CCEtoODBDoc *doc, FileStruct *file, BlockStruct *fileblock, 
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   WORD keyword = doc->IsKeyWord(ATT_TEST, 0);
   CString sep = GetListSeparator();   // get the separator list from the locale settings
   // inherit TEST attribute placed on Component Insert for Single Pin Components
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      if (!data->getInsert()->getRefname() || !strlen(data->getInsert()->getRefname()))
         continue;

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0);
      if (!attrib)
         continue;

      // single-pin components only
      if (GetPinCount(doc->getBlockAt(data->getInsert()->getBlockNumber())) > 1)
         continue;

      // set attrib on comppins
      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (net->getFlags() & NETFLAG_UNUSEDNET)   
            continue;

         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            if (compPin->getRefDes().Compare(data->getInsert()->getRefname()))
               continue;

            doc->SetAttrib(&compPin->getAttributesRef(), keyword, VT_STRING, NULL, SA_OVERWRITE, NULL);
         }
      }
   }


   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   NetnameDataArray netnameDataArray;
   int netnameDataCnt = 0;


   fprintf(wfp, "Feature" + sep + "Pin" + sep + "Netname" + sep + "X" + sep + "Y" + sep + "TestAttribute\n");


   // store dataPosition with netname
   dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      POSITION tempPos = dataPos;

      DataStruct *data = DataList->GetNext(dataPos);

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0);
      if (attrib)
      {
         CString netname = get_attvalue_string(doc, attrib);

         RPTNetnameData *netnameData = new RPTNetnameData;
         netnameDataArray.SetAtGrow(netnameDataCnt, netnameData);
         netnameData->netname = netname;
         netnameData->dataPos = tempPos;
         netnameDataCnt++;
      }
   }


   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
   
      if (net->getFlags() & NETFLAG_UNUSEDNET)   
         continue;
   
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         Attrib *attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST, 0);
         if (attrib && compPin->getPinCoordinatesComplete())
         {
            CString testAtrtib = get_attvalue_string(doc, attrib);

            if (!testAtrtib.GetLength())
               testAtrtib = "TEST";

            fprintf(wfp, "%s%s %s%s %s%s %s%s %s%s %s\n",
                  compPin->getRefDes(), sep,  
                  compPin->getPinName(), sep, 
                  csv_string(net->getNetName()), sep, 
                  FormatDecimal(compPin->getOriginX(), decimals), sep, 
                  FormatDecimal(compPin->getOriginY(), decimals), sep, 
                  testAtrtib);
         }
      }


      // Do traces and vias for this net
      for (int i=0; i<netnameDataCnt; i++)
      {
         RPTNetnameData *netnameData = netnameDataArray[i];

         if (netnameData->netname.Compare(net->getNetName())) 
            continue; 

         DataStruct *data = DataList->GetAt(netnameData->dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0);
         if (attrib)
         {
            CString  feature;
            feature = data->getInsert()->getRefname();

            if (strlen(feature) == 0)
            {
               BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
               feature = block->getName();
            }

            CString testAtrtib = get_attvalue_string(doc, attrib);

            if (!testAtrtib.GetLength())
               testAtrtib = "TEST";

            fprintf(wfp, "%s%s %s %s%s %s%s %s%s %s\n",
                  feature, sep, sep, 
                  csv_string(net->getNetName()), sep, 
                  FormatDecimal(data->getInsert()->getOriginX(), decimals), sep, 
                  FormatDecimal(data->getInsert()->getOriginY(), decimals), sep,
                  testAtrtib);
         }
      }
   }

	fprintf(wfp, "\n");

   for (int i=0; i<netnameDataCnt; i++)
      delete netnameDataArray[i];
   netnameDataArray.RemoveAll();
}

//******************************************************************************

static CString Report_WriteTestAttributeReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "testattribute.csv";

      FILE *fReport = fopen(reportFile, "wt");

      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						generate_PINLOC(doc, file, 0);  // this function generates the PINLOC argument for all pins.
						Report_WriteTestAttributeReportData(fReport, doc, file, file->getBlock(), &(file->getBlock()->getDataList()), 
							file->getInsertX(), file->getInsertY(),file->getRotation(),  file->isMirrored(), file->getScale());
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								generate_PINLOC(doc, pcbFile, 0);  // this function generates the PINLOC argument for all pins.
								Report_WriteTestAttributeReportData(fReport, doc, pcbFile, pcbFile->getBlock(), &(pcbFile->getBlock()->getDataList()), 
									pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale());

							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WriteTHTPinsData(FILE *wfp, CCEtoODBDoc *doc, CDataList *DataList, DTransform xForm)
{
	int SMD = doc->IsKeyWord("SMD", 0);

//   fprintf(wfp,"Refdes, Device/Type, Partnumber, Geometry, GridLocation, X, Y, Mirror, Rotation, SMD/THRU, PinCount, CentroidX, CentroidY, CentroidRotation\n");
    CString sep = GetListSeparator();   // get the separator list from the locale settings
	fprintf(wfp,"Refdes" + sep + "Pin" + sep + "X" + sep + "Y" + sep + "DrillSize" + sep + "Surface\n");

   POSITION compPos = DataList->GetHeadPosition();
   while (compPos)
   {
      DataStruct *comp = DataList->GetNext(compPos);

      if (comp->getDataType() != dataTypeInsert)
         continue;

      if (comp->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      Point2 point2(comp->getInsert()->getOriginX(), comp->getInsert()->getOriginY());
		xForm.TransformPoint(&point2);

      BlockStruct *compBlock = doc->Find_Block_by_Num(comp->getInsert()->getBlockNumber());

		POSITION pinPos = compBlock->getDataList().GetHeadPosition();
		while (pinPos)
		{
			DataStruct *pin = compBlock->getDataList().GetNext(pinPos);

			if (pin->getDataType() != T_INSERT)
				continue;

			if (pin->getInsert()->getInsertType() != INSERTTYPE_PIN)
				continue;

			// don't write SMD pins
			Attrib* attrib = NULL;

			if (pin->getAttributesRef() && pin->getAttributesRef()->Lookup(SMD, attrib))
				continue;

			// calculate the pin's location
			DTransform pinXForm(point2.x, point2.y,
				xForm.scale * comp->getInsert()->getScale(),
				xForm.rotation + ((xForm.mirror & MIRROR_FLIP)?-comp->getInsert()->getAngle():comp->getInsert()->getAngle()),
				xForm.mirror ^ comp->getInsert()->getMirrorFlags());
			Point2 pinPoint(pin->getInsert()->getOriginX(), pin->getInsert()->getOriginY());
			pinXForm.TransformPoint(&pinPoint);

			double drillSize = 0.0;
			getDrillInfo(doc, pin, drillSize);

			fprintf(wfp, "%s%s%s%s%s%s%s%s%s%s%s\n", 
            comp->getInsert()->getRefname(), sep,  pin->getInsert()->getRefname(), sep, 
				FormatDecimal(pinPoint.x, 5), sep,  FormatDecimal(pinPoint.y, 5), sep, 
            FormatDecimal(drillSize, 5), sep, 
            (comp->getInsert()->getMirrorFlags()?"BOTTOM":"TOP"));
		} // END while (pinPos) ...
	} // END while (compPos) ...

	fprintf(wfp, "\n");
}

//******************************************************************************

static CString Report_WriteTHTPinsReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "thtpins.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						// vias and generic and mechanical components
						DTransform xForm(file->getInsertX(), file->getInsertX(), file->getScale(), file->getRotation(), file->isMirrored());
						Report_WriteTHTPinsData(fReport, doc, &(file->getBlock()->getDataList()), xForm);
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								DTransform xForm(pcbFile->getInsertX(), pcbFile->getInsertX(), pcbFile->getScale(), pcbFile->getRotation(), pcbFile->isMirrored());
								Report_WriteTHTPinsData(fReport, doc, &(pcbFile->getBlock()->getDataList()), xForm);
							
							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WritePinToPinLengthData(FILE *fReport, CCEtoODBDoc *doc, FileStruct *file, BlockStruct *fileblock, CDataList *DataList, 
      double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   CString sep = GetListSeparator();   // get the separator list from the locale settings
   int maxStackup = 0;
	int i=0;
   for (i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (!layer)
         continue;

      int stackupNum = layer->getElectricalStackNumber();
      if (stackupNum > maxStackup)
         maxStackup = stackupNum;
   }


   EliminateDuplicateVias(doc, fileblock);
   Crack(doc, fileblock, TRUE);         
   EliminateSinglePointPolys(doc);                
   BreakSpikePolys(fileblock); 
   double accuracy = get_accuracy(doc);
   EliminateOverlappingTraces(doc, fileblock, TRUE, accuracy);               


   fprintf(fReport, "Net Name" + sep + "Pin1" + sep + "Pin2" + sep + "Manhattan Distance" + sep + "Actual Length" + sep + "Difference" + sep + "%% Difference" + sep + "Qty Pins" + sep + "Qty Vias" + sep + "Min. Width" + sep + "Max. Width" + sep + "Units\n");


   // store datas with netnames
   NetnameDataArray netnameDataArray;
   netnameDataArray.SetSize(20, 20);
   int netnameDataCnt = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      POSITION tempPos = dataPos;

      DataStruct *data = DataList->GetNext(dataPos);

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
      if (attrib)
      {
         CString netname = get_attvalue_string(doc, attrib);

         RPTNetnameData *n = new RPTNetnameData;
         netnameDataArray.SetAtGrow(netnameDataCnt, n);
         n->netname = netname;
         n->dataPos = tempPos;
         netnameDataCnt++;
      }
   }


   // run through the netlist and collect all netnames.
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)   
         continue;

      fputs("\n", fReport);

      ClearTrace();
   
      BOOL powernet = FALSE;
      if (is_attvalue(doc, net->getAttributesRef(), ATT_POWERNET, 1))
         powernet = TRUE;
      
      PinArray pinArray;
      pinArray.SetSize(20, 20);
      int pinCnt = 0;

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (compPin->getPinCoordinatesComplete())
         {
            pinArray.SetAtGrow(pinCnt, compPin);

            LoadEndpoint(pinCnt, compPin->getOriginX(), compPin->getOriginY(), 
                  GetPadstackLayerMap(doc, compPin->getPadstackBlockNumber(), compPin->getMirror(), maxStackup));

            pinCnt++;
         }
      }

      if (!powernet)
      {
         // now find all traces and vias, width and layers, 
         double manhattanLength = 0;
         double actualLength = 0;
         int viaCnt = 0;
         double minwidth = -1, maxwidth = -1;
         
         // here do traces and vias
			int i=0;
         for (i=0; i<netnameDataCnt; i++)
         {
            if (strcmp(netnameDataArray[i]->netname, net->getNetName()))  
               continue;

            DataStruct *data = DataList->GetAt(netnameDataArray[i]->dataPos);

            switch (data->getDataType())
            {
               case T_POLY:
               {
                  POSITION polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos)
                  {
                     CPoly *poly = data->getPolyList()->GetNext(polyPos);

                     if (poly->isHidden() || poly->isClosed())   
                        continue;

                     LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
                     int stackupNum = -1;
                     if (layer->getElectricalStackNumber())
                        stackupNum = layer->getElectricalStackNumber();
                     else
                     {
                        switch (layer->getLayerType())
                        {
                        case LAYTYPE_SIGNAL_TOP:
                           stackupNum = 0;
                           break;
                        case LAYTYPE_SIGNAL_BOT:
                           stackupNum = maxStackup;
                           break;
                        }
                     }

                     if (stackupNum < 0)
                        continue;

                     LoadPoly(poly, stackupNum, poly->getWidthIndex());
                     
                     double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale;

                     if (minwidth < 0) 
                        minwidth = lineWidth;
                     else if (minwidth > lineWidth)
                        minwidth = lineWidth;

                     if (maxwidth < 0) 
                        maxwidth = lineWidth;
                     else if (maxwidth < lineWidth)
                        maxwidth = lineWidth;

                     BOOL first = TRUE;
                     double x1, y1, b1, x2, y2, b2;
                     POSITION pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        CPnt *pnt = poly->getPntList().GetNext(pntPos);
                        x2 = pnt->x * scale;
                        y2 = pnt->y * scale;
                        b2 = pnt->bulge;
   
                        if (!first)
                           actualLength += NetLength_Segment(doc, x1, y1, x2, y2, b1);
                        first = FALSE;

                        x1 = x2;
                        y1 = y2;
                        b1 = b2;
                     }
                  }
               }
               break;

               case T_INSERT:
               {
                  if (data->getInsert()->getInsertType() == INSERTTYPE_VIA)
                  {
                     viaCnt++;

                     LoadVia(data->getEntityNumber(), data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 
                           GetPadstackLayerMap(doc, data->getInsert()->getBlockNumber(), data->getInsert()->getMirrorFlags(), maxStackup));
                  }
               }
               break;
            } // end switch
         } // while

         // here calc manhattanLength
         for (i=0; i<pinCnt; i++)
         {
            double minlength = FLT_MAX;

            for (int ii=i+1; ii<pinCnt; ii++)
            {
               double len = fabs(pinArray[i]->getOriginX() - pinArray[ii]->getOriginX()) + 
                            fabs(pinArray[i]->getOriginY() - pinArray[ii]->getOriginY());
               if (len < minlength)
                  minlength = len;
            }
            if (minlength < FLT_MAX)
               manhattanLength += minlength;
         }

         int percent;
         if (fabs(actualLength) > 0 && fabs(manhattanLength) > 0)
            percent = round((actualLength / manhattanLength)*100);
         else
            percent = 0;

         fprintf(fReport, "%s%s%s%s", csv_string(net->getNetName()), sep, sep, sep);
         fprintf(fReport, "%s%s", FormatDecimal(manhattanLength, decimals), sep);
         fprintf(fReport, "%s%s", FormatDecimal(actualLength, decimals), sep);
         fprintf(fReport, "%s%s", FormatDecimal(actualLength - manhattanLength, decimals), sep);
         fprintf(fReport, "%d%s%d%s%d%s", percent, sep, pinCnt, sep, viaCnt, sep);
         fprintf(fReport, "%s%s", FormatDecimal(minwidth, decimals), sep);
         fprintf(fReport, "%s%s", FormatDecimal(maxwidth, decimals), sep);
         fprintf(fReport, "%s\n", GetUnitName(doc->getSettings().getPageUnits()));


         CResultList *resultList = FindShortestNetList();

         if (resultList)
         {
            POSITION resultPos = resultList->GetHeadPosition();
            while (resultPos)
            {
               CPathList *path = resultList->GetNext(resultPos)->path;

               Net_Path *firstNode = path->GetHead();
               Net_Path *lastNode = path->GetTail();
               if (firstNode->type != ENDPOINT_NODE && lastNode->type != ENDPOINT_NODE)
                  continue;

               fprintf(fReport, "%s%s", csv_string(net->getNetName()), sep);

               CompPinStruct *pin1 = NULL, *pin2 = NULL;
               if (firstNode->type == ENDPOINT_NODE)
               {
                  pin1 = pinArray[firstNode->index_id];
                  fprintf(fReport, "%s-%s", pin1->getRefDes(), pin1->getPinName());
               }
               else
               {
                  fprintf(fReport, "STUB");
               }
               fputs(sep, fReport);
                  
               if (lastNode->type == ENDPOINT_NODE)
               {
                  pin2 = pinArray[lastNode->index_id];
                  fprintf(fReport, "%s-%s", pin2->getRefDes(), pin2->getPinName());
               }
               else
               {
                  fprintf(fReport, "STUB");
               }
               fputs(sep, fReport);

               double manhattanLength = 0;
               if (pin1 && pin2)
                  manhattanLength = fabs(pin1->getOriginX() - pin2->getOriginX()) + 
                                    fabs(pin1->getOriginY() - pin2->getOriginY());

               double actualLength = 0;
               int viaCnt = 0;
               int pinCnt = 0;
               double minWidth = FLT_MAX, maxWidth = 0;
               POSITION pathPos = path->GetHeadPosition();
               while (pathPos)
               {
                  Net_Path *node = path->GetNext(pathPos);

                  switch (node->type)
                  {
                  case ENDPOINT_NODE:
                     pinCnt++;
                     break;
                  case VIA_NODE:
                     viaCnt++;
                     break;
                  case SEGMENT_NODE:
                     {
                        Net_Segment *segment = segmentArray->GetAt(node->index_id);
                        actualLength += NetLength_Segment(doc, segment->x1, segment->y1, segment->x2, segment->y2, 0);

                        if (doc->getWidthTable()[segment->widthIndex]->getSizeA() < minWidth)
                           minWidth = doc->getWidthTable()[segment->widthIndex]->getSizeA();

                        if (doc->getWidthTable()[segment->widthIndex]->getSizeA() > maxWidth)
                           maxWidth = doc->getWidthTable()[segment->widthIndex]->getSizeA();
                     }
                     break;
                  }
               }

               if (fabs(actualLength) > 0 && fabs(manhattanLength) > 0)
                  percent = round((actualLength / manhattanLength)*100);
               else
                  percent = 0;

               fprintf(fReport, "%s%s", FormatDecimal(manhattanLength, decimals), sep);
               fprintf(fReport, "%s%s", FormatDecimal(actualLength, decimals), sep);
               fprintf(fReport, "%s%s", FormatDecimal(actualLength - manhattanLength, decimals), sep);
               fprintf(fReport, "%d%s%d%s%d%s", percent, sep, pinCnt, sep, viaCnt, sep);
               fprintf(fReport, "%s%s", FormatDecimal(minWidth, decimals), sep);
               fprintf(fReport, "%s%s", FormatDecimal(maxWidth, decimals), sep);
               fprintf(fReport, "%s\n", GetUnitName(doc->getSettings().getPageUnits()));
            }
         }
      }
      else
      {
         // powernet
         fprintf(fReport, "%s%s%s%sPOWERNET%s%s%s%s%d%s%s%s%s\n", csv_string(net->getNetName()),sep, sep, sep, sep, sep, sep, sep, pinCnt,sep, sep, sep, sep);
      }

      pinArray.RemoveAll();   
   }

	fprintf(fReport, "\n\n");

   for (i=0; i<netnameDataCnt; i++)
      delete netnameDataArray[i];
}

//******************************************************************************

static CString Report_WritePinToPinLengthReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "pin_pin.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						generate_PINLOC(doc, file, 0);  // this function generates the PINLOC argument for all pins.
						Report_WritePinToPinLengthData(fReport, doc, file, file->getBlock(), &(file->getBlock()->getDataList()), 
								file->getInsertX(), file->getInsertY(),file->getRotation(),  file->isMirrored(), file->getScale());
					}
					else if (file->getBlockType() == blockTypePanel)
					{
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
								generate_PINLOC(doc, pcbFile, 0);  // this function generates the PINLOC argument for all pins.
								Report_WritePinToPinLengthData(fReport, doc, pcbFile, pcbFile->getBlock(), &(pcbFile->getBlock()->getDataList()), 
									pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale());

							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static void Report_WriteTestProbeReportData(CFormatStdioFile &fReport, FILE *fLog, CCEtoODBDoc *doc, FileStruct *file, const CTMatrix transformMatrix, const CString refdesPrefx, bool suppressHeader)
{

	CString sep = GetListSeparator();   // get the separator list from the locale settings

   if (!suppressHeader)
	   fReport.WriteString("ProbeName" + sep + "Netname" + sep + "XLocation" + sep + "YLocation" + sep + "Surface" + sep + "Dev-Pin" + sep + "CopperArea" + sep + "Pin_or_Via" + sep + "TargetName\n");
	

	BlockStruct *fileBlock = file->getBlock();

	POSITION pos = fileBlock->getHeadDataInsertPosition();
	while (pos != NULL)
	{
		DataStruct *data = fileBlock->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

      if (insert->getInsertType() == insertTypeTestProbe)
      {

         Attrib *a = NULL;
         WORD netnameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
         CString netname;

         if (data->lookUpAttrib(netnameKW, a))
            netname = get_attvalue_string(doc, a);

         NetStruct *net = FindNet(file, netname);

         if (net != NULL)
         {
            CompPinStruct *cp = net->getHeadCompPin();

            WORD placedProbeKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
            CString placement;

            if (data->lookUpAttrib(placedProbeKW, a))
               placement = get_attvalue_string(doc, a);

            if (placement != "Unplaced")
            {
               CString featureType, targetName;
               WORD dlinkKW = doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);

               if (data->lookUpAttrib(dlinkKW, a))
               {
                  CEntity probedEntity = CEntity::findEntity(doc->getCamCadData(), a->getIntValue());

                  if (probedEntity.getEntityType() == entityTypeData)
                  {
                     DataStruct* ta = probedEntity.getData();

                     if (ta->lookUpAttrib(dlinkKW, a))
                     {
                        CEntity accessEntity = CEntity::findEntity(doc->getCamCadData(), a->getIntValue());

                        if (accessEntity.getEntityType() == entityTypeData)
                        {
                           DataStruct* feature = accessEntity.getData();
                           if(feature->isInsertType(insertTypeVia) || feature->isInsertType(insertTypeBondPad))
                           {
                              featureType = insertTypeToDisplayString(feature->getInsert()->getInsertType());
                              targetName = feature->getInsert()->getRefname();
                           }
                        }
                        else if (accessEntity.getEntityType() == entityTypeCompPin)
                        {
                           featureType = "PIN";
                           targetName = accessEntity.getCompPin()->getPinRef('-');
                        }
                     }
                  }
               }

               CString probeRefname( refdesPrefx + insert->getRefnameRef().MakeUpper() );

               CBasesVector insertBasesVector = insert->getBasesVector();
               insertBasesVector.transform(transformMatrix);

               fReport.WriteString("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", 
                  probeRefname, sep,
                  netname.MakeUpper(), sep, 
                  FormatDecimal(insertBasesVector.getX(), 3), sep,  
                  FormatDecimal(insertBasesVector.getY(), 3), sep, 
                  insert->getLayerMirrored() ? "BOT" : "TOP", sep,
                  cp->getPinRef('-').MakeUpper(), sep, 
                  FormatDecimal(0.0, 1), sep, 
                  featureType.MakeUpper(), sep, 
                  targetName.MakeUpper());
            }
         }
      }
   }
}

//******************************************************************************

static CString Report_WriteTestProbeReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "testprobe.csv";

      CFormatStdioFile fReport;
		CFileException err;
		if (!fReport.Open(reportFile, CFile::modeCreate|CFile::modeWrite, &err))
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
		else if (false && doc->getFileList().GetOnlyShown(blockTypePcb) == NULL)
		{
         CString tmp;
         tmp.Format("Need one and only one PCB file showing");
         MessageBox(NULL, tmp, "Report Error!", MB_OK | MB_ICONHAND);
		}
      else
      {
         FileStruct *file;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb) 
					{
						Report_WriteTestProbeReportData(fReport, fErr, doc, file, file->getTMatrix(), "", false);
					}
					else if (file->getBlockType() == blockTypePanel)
					{
                  int pcbCount = 0;
                  bool suppressHeader = false;
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
                     pcbCount++;
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
                        CString pcbRefname(pcbInsert->getRefname());
                        if (pcbRefname.IsEmpty())
                           pcbRefname.Format("Board%d", pcbCount);

								CTMatrix pcbMatrix = pcbInsert->getTMatrix() * pcbFile->getTMatrix() * file->getTMatrix();
                        
                        Report_WriteTestProbeReportData(fReport, fErr, doc, pcbFile, pcbMatrix, pcbRefname + "_", suppressHeader);

								//Report_WriteComponentData(fReport, *doc, pcbFile->getBlock()->getDataList(), pcbMatrix, pcbInsert->getRefname() + "_", suppressHeader);

                        suppressHeader = true;
							}
						}
					}
				}
			}

         fReport.Close();
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

void Report_WriteAdvancedPackagingData(FILE *wfp, CCEtoODBDoc& doc, DataStruct* data, const CTMatrix transformMatrix, 
                                       const CString refdesPrefx, const bool suppressHeader, CString sep, bool option_allquote)
{
   BlockStruct* block = doc.getBlockAt(data->getInsert()->getBlockNumber());
   CString insertTypeStr = insertTypeToDisplayString(data->getInsert()->getInsertType());

   CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
   insertBasesVector.transform(transformMatrix);
   
   Attrib *attrib;                                                        
   CString device = "";
   CString part_number = "";
   CString grid_location = "";
   CString geometry = csv_string(block->getName(),option_allquote);
   CString compname = csv_string(refdesPrefx + data->getInsert()->getRefname(),option_allquote);
   CString technology = "";
	CString loaded = "TRUE";

   if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1))
      device = csv_string(doc.getAttributeStringValue(attrib),option_allquote);

   if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_PARTNUMBER, 1))
      part_number = csv_string(doc.getAttributeStringValue(attrib),option_allquote);

   if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_GRID_LOCATION, 1))
      grid_location = csv_string(doc.getAttributeStringValue(attrib),option_allquote);

   if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_TECHNOLOGY, 1))
   {
      technology = csv_string(doc.getAttributeStringValue(attrib));         
   }

   // Get Technology from the firstpin data of the bondpad
   if(technology.IsEmpty() && data->isInsertType(insertTypeBondPad))
   {
      DataStruct *insertPinData = block->GetFirstPinInsertData();
      if (insertPinData && (attrib = is_attvalue(&doc, insertPinData->getAttributesRef(), ATT_TECHNOLOGY, 1)))
      {
         technology = csv_string(doc.getAttributeStringValue(attrib));         
      }
   }

   if (attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_LOADED, 1))
	{
		CString value = csv_string(doc.getAttributeStringValue(attrib),option_allquote);
      if (value.CompareNoCase("false") == 0)
			loaded = "FALSE";
	}

   //calculate Die pin Count
   int pinCount = block->getDiePinCount();
   CString pinCntStr = "";
   if(pinCount)
      pinCntStr.Format("%d",pinCount);

   fprintf(wfp, "%s%s%s%s%s%s",insertTypeStr , sep, compname, sep, device, sep); 
   fprintf(wfp, "%s%s%s%s%s%s",part_number, sep, geometry, sep, grid_location, sep);
   fprintf(wfp, "%s%s%s%s", FormatDecimal(insertBasesVector.getX(), 5), sep, FormatDecimal(insertBasesVector.getY(), 5), sep);
   fprintf(wfp, "%s%s", data->getInsert()->getPlacedBottom()?"BOTTOM":"TOP", sep); 
   fprintf(wfp, "%d%s", normalizeDegrees(round(insertBasesVector.getRotationDegrees())), sep);
   fprintf(wfp, "%s", technology);
   fprintf(wfp, "%s%s%s", sep, pinCntStr, sep);

   // if centroid
   DataStruct *centroid = block->GetCentroidData();
   if (centroid)
   {
      CBasesVector centroidBasesVector = centroid->getInsert()->getBasesVector();
      centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());
      int normCentAngle = normalizeDegrees(round(centroidBasesVector.getRotationDegrees()));

      fprintf(wfp,"%s%s%s%s%d", FormatDecimal(centroidBasesVector.getX()), sep,  FormatDecimal(centroidBasesVector.getY(), 5), sep, normCentAngle);
   }  
   else
   {
      fprintf(wfp, "%s%s", sep, sep);
   }

   
   fprintf(wfp, "%s%s", sep, loaded);

   for(int i = 0, columnSize =  commendSetting.getcompsColumnHeaders().GetCount() ; i < columnSize; i++)
   {
      CString compsColumnHeadersItr = commendSetting.getcompsColumnHeaders().GetAt(i);
      fprintf(wfp,"%s",sep);
      if (attrib = is_attvalue(&doc, data->getAttributesRef(), compsColumnHeadersItr, 1))
      {
         CString value = csv_string(doc.getAttributeStringValue(attrib));
         fprintf(wfp,"%s",value);
      }
   }
   fprintf(wfp,"\n");
}

//******************************************************************************

void Report_WriteAdvancedPackagingDataList(FILE *wfp, CCEtoODBDoc& doc, CDataList& dataList, const CTMatrix transformMatrix, const CString refdesPrefx, const bool suppressHeader)
{
   //Mat2x2 m;
   //RotMat2(&m, rotation);
   CString sep = GetListSeparator();   // get the separator list from the locale settings

   if (!suppressHeader)
   {
      fprintf(wfp,"Data Type" + sep + "Refdes" + sep + "Device/Type" + sep + "Partnumber" + sep + "Geometry" + sep +  "GridLocation"  + sep + "X"  + sep + "Y"  + sep + "Mirror" + sep + "Rotation" + sep + "SMD/THRU" + sep + "DiePinCount" + sep + "CentroidX" + sep + "CentroidY" + sep + "CentroidRotation" + sep + "LOADED");
      for(int i = 0, columnSize =  commendSetting.getcompsColumnHeaders().GetCount(); i < columnSize; i++)
      {
         CString compsColumnHeadersItr = commendSetting.getcompsColumnHeaders().GetAt(i);
         fprintf(wfp,sep + compsColumnHeadersItr);
      }
      fprintf(wfp,"\n");
   }

   bool option_allquote = commendSetting.getOptionQuoteAllFields();
   CInsertTypeMask insertTypeMask(insertTypeDie, insertTypeBondPad);

	for (CDataListIterator componentIterator(dataList, insertTypeMask); componentIterator.hasNext();)
	{
		DataStruct *data = componentIterator.getNext();
      if(!data || !data->getInsert()) continue;

      Report_WriteAdvancedPackagingData(wfp, doc, data, transformMatrix, refdesPrefx, suppressHeader, sep, option_allquote);      
      BlockStruct *Dieblock = doc.getBlockAt(data->getInsert()->getBlockNumber());

      if(Dieblock)
      {
         for(CDataListIterator DiePinIterator(Dieblock->getDataList(), insertTypeDiePin); DiePinIterator.hasNext();)
         {
            DataStruct *Diepindata = DiePinIterator.getNext();
            CTMatrix diepinMatrix = data->getInsert()->getTMatrix() * transformMatrix;  
            Report_WriteAdvancedPackagingData(wfp, doc, Diepindata, diepinMatrix, refdesPrefx, suppressHeader, sep, option_allquote);
         }
      }
   }
}

//******************************************************************************

static CString Report_WriteAdvancedPackagingReport(FILE *fErr, CString directory, CCEtoODBDoc *doc)
{
	if (doc != NULL)
	{
      CString reportFile = directory;
      reportFile += "advancedPackaging.csv";

      FILE *fReport = fopen(reportFile, "wt");
      if (!fReport)
      {
         CString tmp;
         tmp.Format("Can not open File [%s]", reportFile);
         MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      }
      else
      {
         FileStruct *file;
         commendSetting.LoadCompsReportSettings(getApp().getUserPath() + "AdvPkg.out");
			POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (file->isShown())
				{
					if (file->getBlockType() == blockTypePcb)  
					{
						// vias and generic and mechanical components
						Report_WriteAdvancedPackagingDataList(fReport, *doc, file->getBlock()->getDataList(), file->getTMatrix(), "", false);
					}
					else if (file->getBlockType() == blockTypePanel)
					{
                  bool suppressHeader = false;
						for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
						{
							DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
							InsertStruct *pcbInsert = pcbData->getInsert();

							FileStruct *pcbFile = doc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
							if (pcbFile != NULL)
							{
                        CTMatrix pcbMatrix = pcbInsert->getTMatrix() * pcbFile->getTMatrix() * file->getTMatrix();                        
								Report_WriteAdvancedPackagingDataList(fReport, *doc, pcbFile->getBlock()->getDataList(), pcbMatrix, pcbInsert->getRefname() + "_", suppressHeader);
                        suppressHeader = true;
							}
						}
					}
				}
         }

         fclose(fReport);
			return reportFile;
		}
	}

	return "";
}

//******************************************************************************

static bool AbortOnPanelView(CCEtoODBDoc *doc)
{
	// Case 1447, originally was asked to make Reports work with Panel view mode.
	// And it was done, though result is not really "panel" reports, it is PCB
	// reports grouped together for all PCBs in Panel. Mark and Chris changed
	// minds on this, and now want to issue a message and abort the report when
	// Panel view is encountered. Such is handled here, to get the Panel reports
	// back just make this function return false.

#define PANEL_VIEW_NOT_ALLOWED
#ifdef  PANEL_VIEW_NOT_ALLOWED
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      if (file != NULL && file->isShown())
		{
			if (file->getBlockType() == blockTypePcb) 
			{
				// Okay
			}
			else if (file->getBlockType() == blockTypePanel)
			{
				// Found visible Panel, issue message and abort
            // Whether to abort creating report or not is determin outside of this function.
				//ErrorMessage("Panel files are not supported", "Spreadsheet Reports");
				return true; // found panel, abort Report
			}
		}
	}
#endif

	return false;  // false means do not abort
}

//******************************************************************************

void CCEtoODBDoc::OnReportsSpreadsheet() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   ReportSpreadsheet dlg;   
	dlg.m_directory = this->GetProjectPath(getApp().getUserPath());
   if (dlg.DoModal() != IDOK)
      return;

   // Reset separators in case locale changed while camcad was running
   GetListSeparator(true);
   GetDecimalSeparator(true);

   CString directory = dlg.m_directory;
   if (directory.Right(1) != '\\')
      directory += '\\';

   CWaitCursor wait;

   CString reportLogFile = GetLogfilePath(REPORTERR);

   FILE *fErr = fopen(reportLogFile, "wt");
   if (!fErr)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]", reportLogFile);
      ErrorMessage(tmp, "File Create Error!");
      return;
   }
   display_error = 0;

   // Not all reports are supported in Panel view mode.
   // Set flag to indicate if any non-panel-supported reports are selected.
   bool generateOtherReports = false;
   for (int i=0; i<reportsMax; i++)
   {
      if (dlg.rs[i] && i != reportsComponents && i != reportsTestProbe)
      {
         generateOtherReports = true;
         break;
      }
   }

	if (generateOtherReports && AbortOnPanelView(this) )
   {
	   ErrorMessage("Panel files are not supported, except for Component Report", "Spreadsheet Reports");

      // Generate panel-enabled reports if it is selected
      if (dlg.rs[reportsComponents])
      {
		   CString reportFile = Report_WriteComponentReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "Component Report File [%s] created\n", reportFile);
            display_error++;
         }
      }

      if (dlg.rs[reportsTestProbe])
      {
		   CString reportFile = Report_WriteTestProbeReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "Test Probe Report File [%s] created\n", reportFile);
            display_error++;
         }
      }
   }
   else
   {
      if (dlg.rs[reportsComponents])
      {
		   CString reportFile = Report_WriteComponentReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "Component Report File [%s] created\n", reportFile);
            display_error++;
         }
      }


      if (dlg.rs[reportsGeometries])
      {
		   CString reportFile = Report_WriteGeometriesReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "GeometryList Report File [%s] created\n", reportFile);
            display_error++;
         }
      }


      if (dlg.rs[reportsLayers])
      {
		   CString reportFile = Report_WriteLayersReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr,"LayerList Report File [%s] created\n", reportFile);
            display_error++;
         }
      }


      if (dlg.rs[reportsApertures])
      {
		   CString reportFile = Report_WriteApertureReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "ApertureList Report File [%s] created\n", reportFile);
            display_error++;
         }
      }

      if (dlg.rs[reportsToolList])
      {
		   CString reportFile = Report_WriteToolReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr,"ToolList Report File [%s] created\n", reportFile);
            display_error++;
         }
      }

      if (dlg.rs[reportsNetList])
      {
		   CString reportFile = Report_WriteNetlistReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "NetList Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }

   //*** These reports are currently not implemented yet ***
   //    So the codes are comments out until they are implement

   //### and they also were not updated to new  style that supports Panel view ###

   /*
      if (dlg.rs[RS_TYPELIST])
      {
         ErrorMessage("Typelist Report not implemented");
      }

      if (dlg.rs[RS_STATISTICS])
      {
         CString reportFile = directory;
         reportFile += "stats.csv";

         FILE *fReport = fopen(reportFile, "wt");
         if (!fReport)
         {
            CString tmp;
            tmp.Format("Can not open File [%s]",reportFile);
            MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
         }
         else
         {
            FileStruct *file;
            POSITION pos = FileList.GetHeadPosition();
            while (pos != NULL)
            {
               file = FileList.GetNext(pos);
               if (!file->isShown())  continue;

               fprintf(fReport,"Statictics\n");
               if (file->getBlockType() == blockTypePcb)  
               {
                  Report_StatNetListData(fReport, this, file);
               }
            }

            fclose(fReport);
            fprintf(fErr,"Statistic Report File [%s] created\n", reportFile);
            display_error++;
         }     
      
      }

      if (dlg.rs[RS_TRACELIST])
      {
         CString reportFile = directory;
         reportFile += "tracelist.csv";

         FILE *fReport = fopen(reportFile, "wt");
         if (!fReport)
         {
            CString tmp;
            tmp.Format("Can not open File [%s]", reportFile);
            MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
         }
         else
         {
            FileStruct *file;
            POSITION pos = FileList.GetHeadPosition();
            while (pos != NULL)
            {
               file = FileList.GetNext(pos);
               if (!file->isShown())  continue;

               if (file->getBlockType() == blockTypePcb)  
               {
                  generate_PADSTACKACCESSFLAG(this, 0);
                  ErrorMessage("Trace List not implemented");

                  //Report_WriteTraceListData(fReport, this, file->getBlock(), &(file->getBlock()->getDataList()), 
                  // file->getInsertX(), file->getInsertY(),file->getRotation(),  file->isMirrored(), file->getScale());
                  //Report_WriteTracePinListData(fReport, this, file->getBlock(), &(file->getBlock()->getDataList()), 
                  // file->getInsertX(), file->getInsertY(),file->getRotation(),  file->isMirrored(), file->getScale());
               }
            }

            fclose(fReport);

            fprintf(fErr, "Tracelist Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }
   */

      if (dlg.rs[reportsViaList])
      {
		   CString reportFile = Report_WriteViaListReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "Vialist Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }

      if (dlg.rs[reportsLineLength])
      {
		   CString reportFile = Report_WriteLineLengthReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr,"Linelength Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }

      if (dlg.rs[reportsThroughHolePins])
      {
		   CString reportFile = Report_WriteTHTPinsReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "Component Report File [%s] created\n", reportFile);
            display_error++;
         }
      }

	   if (dlg.rs[reportsPinToPinLength])
      {
		   CString reportFile = Report_WritePinToPinLengthReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr, "Pin to Pin Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }


      if (dlg.rs[reportsManufacturingReport])
      {
		   CString reportFile = Report_WriteManufacturingReport(fErr, directory, this);
         if (!reportFile.IsEmpty())
		   {
			   fprintf(fErr,"Manufacturing Spreadsheet Report File [%s] created\n", reportFile);
			   display_error++;
		   }
      }


      if (dlg.rs[reportsTestAttribute])
      {
		   CString reportFile = Report_WriteTestAttributeReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr,"TestAttribute Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }

      if (dlg.rs[reportsTestProbe])
      {
		   CString reportFile = Report_WriteTestProbeReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr,"TestProbe Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }

      if (dlg.rs[reportsAdvancedPackaging])
      {
		   CString reportFile = Report_WriteAdvancedPackagingReport(fErr, directory, this);
		   if (!reportFile.IsEmpty())
		   {
            fprintf(fErr,"Advanced Packaging Report File [%s] created\n", reportFile);
            display_error++;
         }     
      }
}

 
   fclose(fErr);

   if (display_error)
      Notepad(reportLogFile);

   return;
}

/////////////////////////////////////////////////////////////////////////////
// ReportSpreadsheet dialog
ReportSpreadsheet::ReportSpreadsheet(CWnd* pParent /*=NULL*/)
   : CDialog(ReportSpreadsheet::IDD, pParent)
{
   //{{AFX_DATA_INIT(ReportSpreadsheet)
   m_directory = _T("");
   //}}AFX_DATA_INIT
}

void ReportSpreadsheet::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ReportSpreadsheet)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Text(pDX, IDC_DIR, m_directory);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ReportSpreadsheet, CDialog)
   //{{AFX_MSG_MAP(ReportSpreadsheet)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ReportSpreadsheet message handlers
void ReportSpreadsheet::OnBrowse() 
{
   UpdateData();

   CBrowse dlg;
   dlg.m_strSelDir = m_directory;
   dlg.m_strTitle = "Select path for spreadsheets";
   if (dlg.DoBrowse())
   {
      m_directory = dlg.m_strPath;
      UpdateData(FALSE);
   }
}

BOOL ReportSpreadsheet::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (int i=0; i<reportsMax; i++)
      m_list.AddString(report[i]);
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ReportSpreadsheet::OnOK() 
{
   for (int i=0; i<reportsMax; i++)
      rs[i] = m_list.GetSel(i);
   
   CDialog::OnOK();
}


//***********************************************************************
//***********************************************************************
// Entity Number Reporting
// This is based on the search code for finding entities.
// You will see a similarity of functions here with "Count" as the leading
// part of the name to functions in entity.cpp with the remainder of that name.



//-----------------------------------------------------

void CEntityNumberReporter::TallyEntity(int entNum, CString desc, CMapInt &entityMap)
{
	int count = 1;

	if (entityMap.Lookup(entNum, count))
	{
		count++;
	}

	entityMap.SetAt(entNum, count);

   CString entNumStr;
   entNumStr.Format("%d", entNum);
   CEntityReportRec *rec;
   if (m_entityMap2.lookup(entNumStr, rec))
   {
      rec->count = count;
      rec->desc += ", [" + desc + "]";
   }
   else
   {
      CEntityReportRec *newRec = new CEntityReportRec;
      newRec->count = 1;
      newRec->desc = "[" + desc + "]";
      m_entityMap2.add(entNumStr, newRec);
   }
}

//-----------------------------------------------------

void CEntityNumberReporter::CountFindDataEntity(CCEtoODBDoc *doc, CMapInt &entityMap)
{
	for (int i=0; i < doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);
		if (block == NULL)   continue;      // holes in blocklist !

		POSITION dataPos = block->getDataList().GetHeadPosition();
		while (dataPos != NULL)
		{
			DataStruct *data = block->getDataList().GetNext(dataPos);
			if (data != NULL)
			{
				int entNum = data->getEntityNumber();

            CString desc;

            DataTypeTag datatype = data->getDataType();
            CString datatypeStr = dataTypeTagToString(datatype);
            CString refname;
            CString inserttype;
            if (datatype == dataTypeInsert && data->getInsert() != NULL)
            {
               refname = data->getInsert()->getRefname();
               inserttype = insertTypeToString(data->getInsert()->getInsertType());
               desc = datatypeStr + " " + inserttype + " " + refname;
            }
            else
            {
               desc = datatypeStr + " in block " + block->getName();
            }
             

				TallyEntity(entNum, desc, entityMap);
			}
		}
	}
}

//-----------------------------------------------

void CEntityNumberReporter::CountFindCompPinEntity(CCEtoODBDoc *doc, CMapInt &entityMap)
{
	POSITION filePos = doc->getFileList().GetHeadPosition();
	while (filePos != NULL)
	{
		FileStruct *file = doc->getFileList().GetNext(filePos);

		POSITION netPos = file->getNetList().GetHeadPosition();
		while (netPos != NULL)
		{
			NetStruct *net = file->getNetList().GetNext(netPos);

			POSITION cpPos = net->getHeadCompPinPosition();
			while (cpPos != NULL)
			{
				CompPinStruct *cp = net->getNextCompPin(cpPos);

            CString desc = "CompPin " + cp->getPinRef('.');
				int entNum = cp->getEntityNumber();
				TallyEntity(entNum, desc, entityMap);
			}
		}
	}
}

//-----------------------------------------------------

void CEntityNumberReporter::CountFindNetEntity(CCEtoODBDoc *doc, CMapInt &entityMap)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         CString desc = "Net " + net->getNetName();

         int entNum = net->getEntityNumber();
         TallyEntity(entNum, desc, entityMap);
      }
   }
}

//-----------------------------------------------------

void CEntityNumberReporter::CountFindDrcEntity(CCEtoODBDoc *doc, CMapInt &entityMap)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION drcPos = file->getDRCList().GetHeadPosition();
      while (drcPos)
      {
         DRCStruct *drc = file->getDRCList().GetNext(drcPos);

         CString desc = "DRC ";

         int entNum = drc->getEntityNumber();
         TallyEntity(entNum, desc, entityMap);
      }
   }
}

//-----------------------------------------------------

CEntityNumberReporter::CEntityNumberReporter(CCEtoODBDoc *doc)
: m_doc(doc)
{

   if (m_doc != NULL)
   {
      CountFindDataEntity(m_doc, m_entityMap);

      CountFindCompPinEntity(m_doc, m_entityMap);

      CountFindNetEntity(m_doc, m_entityMap);

      CountFindDrcEntity(m_doc, m_entityMap);
   }
}

//-----------------------------------------------------

void CEntityNumberReporter::WriteFile(CString filename)
{
   // All this entity number report stuff grew out of debug code for some cases where
   // teh cause of the problem was duplicate entity numbers. It originally just wrote 
   // to a log file, in a crude format. Now it presents a pop-up, and this portion
   // has not been polished up. It is not being used as of this writing.

   // Write all entity number stats to file

	FILE *logfp = fopen(filename, "w");

	int highestEnt = 0;


	POSITION pos = m_entityMap.GetStartPosition();
	while (pos != NULL)
	{
		int key, value;
		m_entityMap.GetNextAssoc(pos, key, value);
		if (value != 1)
			fprintf(logfp, "(%d) (%d) %s\n", key, value, value > 1 ? "****************" : "");

		if (key > highestEnt)
			highestEnt = key;
	}
	
	fprintf(logfp, "-----------------------------------------\n");
	fprintf(logfp, "Highest Entity Number  %d\n", highestEnt);
	fprintf(logfp, "-----------------------------------------\n");


	pos = m_entityMap.GetStartPosition();
	while (pos != NULL)
	{
		int key, value;
		m_entityMap.GetNextAssoc(pos, key, value);
		fprintf(logfp, "(%d) (%d) %s\n", key, value, value > 1 ? "****************" : "");
	}

	fclose(logfp);
}

//-----------------------------------------------------

void CEntityNumberReporter::NotifyUser()
{
   // Notify user of any duplicate entity numbers only, do not write all stats.
   // If no duplicates, notify that all is well.

   CString msg;
   CString buf;

   bool init = true;
   int highestEnt = 0;
   int lowestEnt = 0;

	POSITION pos = m_entityMap.GetStartPosition();
	while (pos != NULL)
	{
		int key, value;
		m_entityMap.GetNextAssoc(pos, key, value);

      if (init)
      {
         lowestEnt = highestEnt = key;
         init = false;
      }

      if (key < lowestEnt)
         lowestEnt = key;

		if (key > highestEnt)
			highestEnt = key;
	}

   
   buf.Format("Smallest Entity Number:  %d\nLargest Entity Number:   %d\n", lowestEnt, highestEnt);
   msg += buf;

	pos = m_entityMap.GetStartPosition();
	while (pos != NULL)
	{
		int key, value;
		m_entityMap.GetNextAssoc(pos, key, value);

      if (value != 1)
      {
         buf.Format("\n%d is duplicated, used %d times.\n", key, value);
         msg += buf;

         CString entNumStr;
         entNumStr.Format("%d", key);
         CEntityReportRec *rec;
         if (m_entityMap2.lookup(entNumStr, rec))
         {
            msg += rec->desc;
         }
      }
	}
	
   // Not really an error, but adheres to camcad messaging conventions and settings
   ErrorMessage(msg, "Entity Number Report\n", MB_ICONINFORMATION | MB_OK);
}

//-----------------------------------------------------

bool CEntityNumberReporter::HasDuplicates()
{
	POSITION pos = m_entityMap.GetStartPosition();
	while (pos != NULL)
	{
		int key, value;
		m_entityMap.GetNextAssoc(pos, key, value);

      if (value > 1)
         return true;
   }

   return false;
}

//-----------------------------------------------------

void CCEtoODBDoc::OnReportsEntityNumbers()
{
	// Entity Number Counter

	//CString logFile = getApp().getUserPath() + "EntityNumber.log";
	//CString logFile = filename + ".log";
   //CString logFile = ".\\entity_number.log";

   CEntityNumberReporter reporter(this);
   reporter.NotifyUser();
   //reporter.WriteFile(logFile);

}

/////////////////////////////////////////////////////////////////////////////
// CComponentOutSetting
CComponentOutSetting::CComponentOutSetting()
{
   LoadDefaultSettings();
}

void CComponentOutSetting::LoadDefaultSettings()
{
   m_compsColumnHeaders.RemoveAll();
   m_optionQuoteAllFields = false;
}

void CComponentOutSetting::LoadCompsReportSettings(CString FileName)
{
   m_SettingFileName = FileName;

   CInFile inFile;
   if(!inFile.open(m_SettingFileName))
      return;

   LoadDefaultSettings();

   while (inFile.getNextCommandLine())
   {
      if(inFile.isCommand(COMPOUTCMD_ATTRIBUTE    ,2))   addComponentHeader(inFile.getParam(1));
      if(inFile.isCommand(COMPOUTCMD_QUOTEALLFIELDS, 2)) inFile.parseYesNoParam(1,m_optionQuoteAllFields,false);
   }

   inFile.close();
}

void CComponentOutSetting::addComponentHeader(CString column)
{
   if(!column.IsEmpty())
   {
      column.TrimLeft();
      column.TrimRight();
      m_compsColumnHeaders.Add(column);
   }
}
