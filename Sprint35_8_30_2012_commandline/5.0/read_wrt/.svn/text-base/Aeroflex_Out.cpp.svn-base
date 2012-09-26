
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "aeroflex_out.h"
#include "find.h" 
#include "DFT.h"
#include "PcbUtil.h"
#include "PolyLib.h"
#include "CompValues.h"
#include "CCEtoODB.h"
#include "fixture_out.h"  // for drill hole size getter
#include "Ck.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int writeSHPoly(CCEtoODBDoc &doc, CFormatStdioFile &file, int indent,const CPolyList* polyList, double unitFactory, bool writeWidth = true, bool writeMirrored = false);
int getDeviceTypeInfo(CString &deviceType, CString subClass, int &pinCount, CString &value, double &plusTol, double &minusTol);
bool getDevicePinMap(CString deviceType, CString pinFunction, CString &pinName);


/******************************************************************************
* Aeroflex_WriteFile
*/
void Aeroflex_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   // Open log file.
   CString localLogFilename;
   FILE *logFp = getApp().OpenOperationLogFile("Aeroflex.log", localLogFilename);
   if (logFp == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(logFp, "Aeroflex CB");

   // Get the writer.
	CAeroflexWrite aeroflexWriter(doc, format->Scale, logFp);

   // Do the writing.
	aeroflexWriter.WriteFile(filename);

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(logFp);

      // We want this to happen only for local log files, not when
      // system log file is in play.
      if (aeroflexWriter.ShowLogFile())
         Logreader(localLogFilename);
   }
}


int writeSHPoly(CCEtoODBDoc &doc, CFormatStdioFile &file, int indent,const CPolyList* polyList, double unitFactor, bool writeWidth, bool writeMirrored)
{
	// Output the list of poly
	POSITION pos = polyList->GetHeadPosition();
	while(pos)
	{
		const CPoly* poly = polyList->GetNext(pos);

		if (writeWidth == true)
		{
			// Output the width
			int width = 0;
			if (poly->getWidthIndex() > -1)
			{
				BlockStruct *block = doc.getWidthTable()[poly->getWidthIndex()];
				width = round(block->getSizeA() * unitFactor);
			}
			file.WriteString("%*s(WIDTH %d)\n", indent, " ", width);
		}

		double cX = 0.0;
		double cY = 0.0;
		double radius = 0.0;
		if (PolyIsCircle(poly, &cX, &cY, &radius))
		{
			cX = cX * unitFactor;
			cY = cY * unitFactor;
			if (writeMirrored == true)
				cX = -cX;

			file.WriteString("%*s(CIRCLE %d %d %d)\n", indent, " ", round(cX), round(cY), round(radius *unitFactor));
		}
		else
		{
			bool firstPnt = true;
			CPntList pntList = poly->getPntList();
			POSITION pntPos = pntList.GetHeadPosition();
			while(pntPos)
			{
				CPnt *pnt = pntList.GetNext(pntPos);

				CPnt curPnt;
				curPnt.x = (DbUnit)(pnt->x * unitFactor);
				curPnt.y = (DbUnit)(pnt->y * unitFactor);
				curPnt.bulge = pnt->bulge;
				if (writeMirrored == true)
				{
					curPnt.x = -curPnt.x;
					if (!fpeq(curPnt.bulge, 0))
						curPnt.bulge = -curPnt.bulge;
				}

				if (firstPnt)
				{
					// Output first point
					firstPnt = false;
					file.WriteString("%*s(M %d %d)\n", indent, " ", round(curPnt.x), round(curPnt.y));
				}
				else
				{
					file.WriteString("%*s(D %d %d)\n", indent, " ", round(curPnt.x), round(curPnt.y));
				}

				if (!fpeq(curPnt.bulge, 0))
				{
					CPnt *tmpPnt = pntList.GetNext(pntPos);

					CPnt nextPnt;
					nextPnt.x = (DbUnit)(tmpPnt->x * unitFactor);
					nextPnt.y = (DbUnit)(tmpPnt->y * unitFactor);
					if (writeMirrored == true)
					{
						nextPnt.x = -nextPnt.x;
					}

					double startAngle, radius, xCenter, yCenter;
					double deltaAngle = atan(curPnt.bulge) * 4;

					ArcPoint2Angle(curPnt.x, curPnt.y, nextPnt.x, nextPnt.y, deltaAngle, &xCenter, &yCenter, &radius, &startAngle);

					file.WriteString("%*s(ARC %d %d %d %d %d)\n", indent, " ", round(xCenter), round(yCenter),
							round(radius), round(RadToDeg(startAngle)), round(RadToDeg(deltaAngle)));

					file.WriteString("%*s(D %d %d)\n", indent, " ", round(nextPnt.x), round(nextPnt.y));
				}
			}
		}
	}

	return 0;
}

bool deviceRequiresPinMap(CString& deviceTypeStr)
{
   DeviceTypeTag deviceType = stringToDeviceTypeTag(deviceTypeStr);

   bool retval;

   switch(deviceType)
   {
	case deviceTypeCapacitorPolarized:  
	case deviceTypeCapacitorTantalum:   
	case deviceTypeDiode:               
	case deviceTypeDiodeLed:            
	case deviceTypeDiodeZener:          
	case deviceTypePotentiometer:       
	case deviceTypeTransistorFetNpn:    
	case deviceTypeTransistorFetPnp:    
	case deviceTypeTransistorMosfetNpn: 
	case deviceTypeTransistorMosfetPnp: 
	case deviceTypeTransistorNpn:       
	case deviceTypeTransistorPnp:       
      retval = true;
      break;
	default:
      retval = false;			
      break;
	}
   return retval;
}

/******************************************************************************
* getDeviceTypeInfo
	This function will do the followings:
	1) Change the following variable base on deviceType
			pinName
			value
			plusTol
			minusTol
	2) If any variable is to be reset then they will have the following value.
			pinName = ""
			value = ""
			plusTol = DBL_MAX
			minusTol = DBL_MAX
	3) If any variable is NOT change then it mean it is good and should stay the way it is
*/
int getDeviceTypeInfo(CString &deviceType, CString subClass, int &pinCount, CString &value, double &plusTol, double &minusTol)
{	
	deviceType.Trim();
	subClass.Trim();
	value.Trim();

	if (!deviceType.CompareNoCase("Capacitor"))
	{
		if (plusTol == minusTol)
		{
			deviceType = "cap";
			minusTol = DBL_MAX;
		}
		else
			deviceType = "dcap";
	}
	else if (!deviceType.CompareNoCase("Capacitor_Polarized"))
	{
		if (plusTol == minusTol)
		{
			deviceType = "spcap";
			minusTol = DBL_MAX;
		}
		else
			deviceType = "dpcap";

		pinCount = 2;
	}
	else if (!deviceType.CompareNoCase("Capacitor_Tantalum"))
	{
		if (plusTol == minusTol)
		{
			deviceType = "spcap";
			minusTol = DBL_MAX;
		}
		else
			deviceType = "dpcap";

		pinCount = 2;
	}
	else if (!deviceType.CompareNoCase("Connector"))
	{
		deviceType.Format("conn%d", pinCount);
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;
	}
	else if (!deviceType.CompareNoCase("Diode"))
	{
		deviceType = "diode";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 2;
	}
	else if (!deviceType.CompareNoCase("Diode_LED"))
	{
		deviceType = "led";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 2;
	}
	else if (!deviceType.CompareNoCase("Diode_Zener"))
	{
		deviceType = "zener";
		minusTol = DBL_MAX;

		pinCount = 2;
	}
	else if (!deviceType.CompareNoCase("Fuse"))
	{
		deviceType = "fuse";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;
	}
	else if (!deviceType.CompareNoCase("Inductor"))
	{
		if (plusTol == minusTol)
		{
			deviceType = "ind";
			minusTol = DBL_MAX;
		}
		else
			deviceType = "dind";
	}
	else if (!deviceType.CompareNoCase("Jumper"))
	{
		deviceType = "link";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;
	}
	else if (!deviceType.CompareNoCase("No_Test"))
	{
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;
	}
	else if (!deviceType.CompareNoCase("Potentiometer"))
	{
		if (plusTol == minusTol)
		{
			deviceType = "vres";
			minusTol = DBL_MAX;
		}
		else
			deviceType = "dvres";

		pinCount = 3;
	}
	else if (!deviceType.CompareNoCase("Resistor"))
	{
		if (plusTol == minusTol)
		{
			deviceType = "res";
			minusTol = DBL_MAX;
		}
		else
			deviceType = "dres";
	}
	else if (!deviceType.CompareNoCase("Resistor_Array"))
	{
		minusTol = DBL_MAX;

      // Case dts0100478999, if subclass is not blank then use it for deviceType
      if (!subClass.IsEmpty())
         deviceType = subClass;
	}
	else if (!deviceType.CompareNoCase("Transistor_FET_NPN"))
	{
		deviceType = "jfetn";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 3;
	}
	else if (!deviceType.CompareNoCase("Transistor_FET_PNP"))
	{
		deviceType = "pfetn";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 3;
	}
	else if (!deviceType.CompareNoCase("Transistor_Mosfet_NPN"))
	{
		deviceType = "mosnd";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 3;
	}
	else if (!deviceType.CompareNoCase("Transistor_Mosfet_PNP"))
	{
		deviceType = "mospd";
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 3;
	}
	else if (!deviceType.CompareNoCase("Transistor_NPN"))
	{
		deviceType = "tran";
		value = "npn";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 3;
	}
	else if (!deviceType.CompareNoCase("Transistor_PNP"))
	{
		deviceType = "tran";
		value = "pnp";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;

		pinCount = 3;
	}
	else if (deviceType.CompareNoCase("Capacitor_Array")) // not Capacitor_Array
	{
		if (!subClass.IsEmpty())
			deviceType = subClass;
		value = "";
		plusTol = DBL_MAX;
		minusTol = DBL_MAX;
	}

	// Format there parameter string
	CString parameter = "";
	if (!value.IsEmpty() && value.CompareNoCase("npn") != 0 && value.CompareNoCase("pnp") != 0)
	{
		double newValue = 0.0;
		MetricPrefixTag newPrefixTag;
		ComponentValues compValue(value);
		
		if (compValue.GetSmallestWholeNumber(newValue, newPrefixTag))
		{
			CString multiplier = metricPrefixTagToAbbreviatedString(newPrefixTag);
			value.Format("%0.3f%s", newValue, multiplier);
		}
	}

	return 0;
}

bool getDevicePinMap(CString deviceType, CString pinFunction, CString &pinName)
{
	if (!deviceType.CompareNoCase("Potentimeter"))
	{
		if (pinFunction == "wiper")
			pinName = "2";
		else if (pinFunction == "1")
			pinName = "1";
		else if (pinFunction == "2")
			pinName = "3";
	}
	else if (pinFunction.CompareNoCase("positive") == 0)
		pinName = "1";
	else if (pinFunction.CompareNoCase("negative") == 0)
		pinName = "2";
	else if (pinFunction.CompareNoCase("anode") == 0)
		pinName = "1";
	else if (pinFunction.CompareNoCase("cathode") == 0 || pinFunction.CompareNoCase("catode") == 0)
		pinName = "2";
	else if (pinFunction.CompareNoCase("collector") == 0)
		pinName = "1";
	else if (pinFunction.CompareNoCase("base") == 0)
		pinName = "2";
	else if (pinFunction.CompareNoCase("emitter") == 0)
		pinName = "3";
	else if (pinFunction.CompareNoCase("drain") == 0)
		pinName = "1";
	else if (pinFunction.CompareNoCase("gate") == 0)
		pinName = "2";
	else if (pinFunction.CompareNoCase("source") == 0)
		pinName = "3";
	else if (pinFunction.CompareNoCase(ATT_VALUE_NO_CONNECT) == 0)	// return false if pin function is "no connect"
		return false;

	return true;
}


/******************************************************************************
* CAeroflexWrite
*/
CAeroflexWrite::CAeroflexWrite(CCEtoODBDoc &doc, double scale, FILE *logFp)
: m_pDoc(doc)
, m_clusterMap(doc)
, m_logFp(logFp)
{
	m_eDFTSource = DEFAULT_SOURCE;
	m_eUnits = DEFAULT_AEROFLEX_UNITS;

	if (m_eUnits == unitMils)
		m_dUnitFactor = Units_Factor(m_pDoc.getSettings().getPageUnits(), UNIT_MILS) * scale;
	else
		m_dUnitFactor = Units_Factor(m_pDoc.getSettings().getPageUnits(), UNIT_MICRONS) * scale;

	m_dScale = scale;
	m_iStartLayerIndex = 0;
	m_iEndLayerIndex = 0;
	m_bDisplayError = false;
	m_textTypeMap.RemoveAll();
	m_shapeDefinitionMap.RemoveAll();
	m_toolHoleGeomDefinitionMap.RemoveAll();
	m_clusterMap.DeleteAllClusters();
}

CAeroflexWrite::~CAeroflexWrite()
{
}

bool CAeroflexWrite::isThereTestProbe(FileStruct *pPcbFile)
{
	POSITION pos = pPcbFile->getBlock()->getHeadDataInsertPosition();
	while (pos != NULL)
	{
		DataStruct *data = pPcbFile->getBlock()->getNextDataInsert(pos);
		if (data == NULL || data->getInsert() == NULL)
			continue;

		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() == insertTypeTestProbe)
			return true;
	}

	return false;
}

CString CAeroflexWrite::getTextTypeID(int width, int height)
{
	CString typeID = "";
	CString textWidthHeight = "";
	textWidthHeight.Format("%d %d", width, height);
	if (!m_textTypeMap.Lookup(textWidthHeight, typeID))
	{
		typeID.Format("t%d", m_textTypeMap.GetCount());
		m_textTypeMap.SetAt(textWidthHeight, typeID);
	}

	return typeID;
}

CString CAeroflexWrite::getShapeDefineID(BlockStruct *block, bool mirrored)
{
   // No case #, was using block name for map key, this resulted in mulitple blocks
   // with same name colliding. Use block number instead.

   CString shapeID = "";

   if (block != NULL)
   {
	   CString shapeName;
      shapeName.Format("%d", block->getBlockNumber());
	   if (mirrored)
		   shapeName.AppendFormat("_Bot");

	   if (!m_shapeDefinitionMap.Lookup(shapeName, shapeID))
	   {
		   shapeID.Format("PIC%d", m_shapeDefinitionMap.GetSize());
		   m_shapeDefinitionMap.SetAt(shapeName, shapeID);
	   }
   }

	return shapeID;
}

CString CAeroflexWrite::findShapeDefineID(int blockNumber, bool mirrored)
{
	CString shapeName;
   shapeName.Format("%d", blockNumber);
	if (mirrored)
		shapeName.AppendFormat("_Bot");

	CString shapeID = "";
	m_shapeDefinitionMap.Lookup(shapeName, shapeID);

	return shapeID;
}

void CAeroflexWrite::getStackLayer()
{
	m_iStartLayerIndex = m_pDoc.getMaxLayerIndex();
	m_iEndLayerIndex = 0;

	for (int i=0; i<m_pDoc.getMaxLayerIndex(); i++)
	{
		LayerStruct *layer = m_pDoc.getLayerAt(i);
		if (layer ==  NULL)
			continue;

		int stackNum = layer->getElectricalStackNumber();
		if (stackNum > 0)
		{
			if(m_iStartLayerIndex > stackNum)
				m_iStartLayerIndex = stackNum;
			if (m_iEndLayerIndex < stackNum)
				m_iEndLayerIndex = stackNum;
		}
	}

	if (m_iStartLayerIndex == m_pDoc.getMaxLayerIndex())
	{
		// If there is no stack number, then set to Start at 0 and End at 1
		m_iStartLayerIndex = 0;
		m_iEndLayerIndex = 1;
	}
}

int CAeroflexWrite::gatherCluster(FileStruct *pPcbFile, bool useTestAttrib)
{
	WORD testKey = (WORD)m_pDoc.RegisterKeyWord(ATT_TEST, 0, VT_STRING);
	WORD netnameKey = (WORD)m_pDoc.RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
	WORD deviceTypeKey = (WORD)m_pDoc.RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
	WORD pinMapKey = (WORD)m_pDoc.RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
	WORD technologyKey = (WORD)m_pDoc.RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);
	WORD loadedKey = (WORD)m_pDoc.RegisterKeyWord(ATT_LOADED, 0, VT_STRING);

	// Add all comppin to m_cluserMap
	POSITION netPos = pPcbFile->getNetList().GetHeadPosition();
	while (netPos)
	{
		NetStruct *net = pPcbFile->getNetList().GetNext(netPos);
		if (net == NULL)
			continue;
		
		CCluster *cluster = m_clusterMap.AddCluster(net->getNetName(), m_iStartLayerIndex, m_iEndLayerIndex);

		POSITION comppinPos = net->getCompPinList().getHeadPosition();
		while (comppinPos)
		{
			CompPinStruct *comppin = net->getCompPinList().getNext(comppinPos);
			if (comppin == NULL)
				continue;

			CString pinName = comppin->getPinName();
			CString refName = comppin->getRefDes();

			// Get the data for the component so we can get the device type of the component
			DataStruct *data = pPcbFile->getBlock()->FindData(refName);
			if (data == NULL)
				continue;

			// Check if component is LOADED
			bool isLoaded = true;
			if (m_bExportUnloaded == false)
			{
				Attrib *attrib = NULL;
				if (data->getAttributes() && data->getAttributes()->Lookup(loadedKey, attrib) && attrib != NULL)
				{
					CString loaded = attrib->getStringValue();
					if (loaded.Trim().CompareNoCase("FALSE") == 0)
						isLoaded = false;
				}
			}


			// Get the pin function 
			// If there is pin function, the get the pin name base on device type and pin function mapping
			Attrib *attrib = NULL;
			if (comppin->getAttributes() && comppin->getAttributes()->Lookup(pinMapKey, attrib))
			{
				CString pinFunction = attrib->getStringValue();

				if (data->getAttributes() && data->getAttributes()->Lookup(deviceTypeKey, attrib) && attrib != NULL)
				{
					CString deviceType = attrib->getStringValue();

               if(deviceRequiresPinMap(deviceType))
               {
					   if (getDevicePinMap(deviceType, pinFunction, pinName) == false)
						   continue;
               }
				}
			}


			// Check TECHNOLOGY attribute
			// Check if use "TEST" attributes and get accessibility
			Attrib *testAttrib = NULL;
			Attrib *technologyAttrib = NULL;
			if (comppin->getAttributes())				
			{
				comppin->getAttributes()->Lookup(technologyKey, technologyAttrib);

				if (useTestAttrib)
				{
					comppin->getAttributes()->Lookup(testKey, testAttrib);
					if (testAttrib == NULL && data->getAttributes())
					{
						// No TEST attribute on comppin so look for at the component
						data->getAttributes()->Lookup(testKey, testAttrib);
					}
				}
			}

			EAllocate allocTop = allocUnknown;
			EAllocate allocBottom = allocUnknown;

			// Add item to cluster
			int layerIndex = cluster->GetBottomAeroflexLayer();

			if (!(comppin->getMirror() > 0))
				layerIndex = cluster->GetTopAeroflexLayer();

			if (data->getInsert() && data->getInsert()->getInsertType() == insertTypePcbComponent)
			{
				getAllocates(data->getInsert()->getInsertType(), comppin->getMirror()==0?false:true, testAttrib, technologyAttrib, allocTop, allocBottom);
				cluster->AddComppinItem(comppin->getEntityNumber(), refName, pinName, comppin->getOriginX(), comppin->getOriginY(),
						comppin->getRotationDegrees(), layerIndex, allocTop, allocBottom, isLoaded);
			}
			else if (data->getInsert() && data->getInsert()->getInsertType() == insertTypeTestPoint)
			{
				getAllocates(data->getInsert()->getInsertType(), comppin->getMirror()==0?false:true, testAttrib, technologyAttrib, allocTop, allocBottom);
				cluster->AddTestpadItem(comppin->getEntityNumber(), refName, comppin->getOriginX(), comppin->getOriginY(),
						comppin->getRotationDegrees(), layerIndex, allocTop, allocBottom, true);
			}
			// else ignore it, tooling holes, fids, and such
			
		}
	}


	// Add all via and trace(track) to m_clusterMap
	BlockStruct *block = pPcbFile->getBlock();
	POSITION pos = block->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = block->getDataList().GetNext(pos);
		if (data == NULL || data->getAttributes() == NULL)
			continue;

		Attrib *attrib = NULL;
		data->getAttributes()->Lookup(netnameKey, attrib);
		if (attrib == NULL)
			continue;

		CString netName = attrib->getStringValue();
		CCluster *cluster = m_clusterMap.FindCluster(netName);
		if (cluster == NULL)
			cluster = m_clusterMap.AddCluster(netName, m_iStartLayerIndex, m_iEndLayerIndex);

		if (data->getDataType() == dataTypeInsert)
		{
			InsertStruct *insert = data->getInsert();
			if (insert == NULL)
				continue;


			// Check TECHNOLOGY attribute
			// Check if use "TEST" attributes and get accessibility
			Attrib *testAttrib = NULL;
			Attrib *technologyAttrib = NULL;
			if (data->getAttributes())				
			{
				data->getAttributes()->Lookup(technologyKey, technologyAttrib);
				if (useTestAttrib)
					data->getAttributes()->Lookup(testKey, testAttrib);
			}


			// Add item to cluster
			if (insert->getInsertType() == insertTypeVia || insert->getInsertType() == insertTypeBondPad)
			{
				EAllocate allocTop = allocUnknown;
				EAllocate allocBottom = allocUnknown;
				getAllocates(insert->getInsertType(), insert->getGraphicMirrored(), testAttrib, technologyAttrib, allocTop, allocBottom);
				
				cluster->AddViaItem(data->getEntityNumber(), insert->getRefname(), insert->getOriginX(), insert->getOriginY(),
					insert->getAngleDegrees(), GetBottomAeroflexLayer(), GetTopAeroflexLayer(), allocTop, allocBottom, true);
			}
		}
		else if (data->getDataType() == dataTypePoly && data->getGraphicClass() == graphicClassEtch)
		{
			cluster->AddTrack(data);
		}
	}

	return 0;
}

void CAeroflexWrite::getAllocates(InsertTypeTag insertType, bool isBottom, Attrib *testAttrib, Attrib *technologyAttrib, EAllocate &allocTop, EAllocate & allocBottom)
{
	bool isTestPad = (insertType == insertTypeTestPad);
	bool isVia = (insertType == insertTypeVia || insertType == insertTypeBondPad);

	// Check if comppin is THRU or not
	bool isTHRU = false;
	if (technologyAttrib != NULL)
	{
		CString technology = technologyAttrib->getStringValue();
		isTHRU =  (technology.CompareNoCase("THRU") == 0)?true:false;
	}


	// Initial allocTop and allocBottom
	if (isTHRU || isVia)  // treat "SMD" vias as THRU too, case 1728
	{
		allocTop = allocMasked;
		allocBottom = allocMasked;
	}
	else if (isBottom)
	{
		allocTop = allocUnknown;
		allocBottom = allocMasked;
	}
	else 
	{
		allocTop = allocMasked;
		allocBottom = allocUnknown;
	}


	// When use "TEST" attribute to determine accessibility, then the prefered side is ALWAYS bottom
	if (testAttrib != NULL)
	{
		// Value of TEST" attribute specified in DbUtil.h
      // no value is ALL
		// BOTH is ALL
      // TOP is TOP ACCESS
      // BOTTOM is BOTTOM ACCESS
      // NONE is no test

		CString test = testAttrib->getStringValue();
		test = test.Trim();

		if (allocTop != allocUnknown && (test.IsEmpty() || test.CompareNoCase("BOTH") == 0 || test.CompareNoCase("Top") == 0))
		{
			if (isTestPad)
				allocTop = allocPrime;
			else
				allocTop = allocGetat;
		}
		
		if (allocBottom != allocUnknown && (test.IsEmpty() || test.CompareNoCase("BOTH") == 0 || test.CompareNoCase("BOTTOM") == 0))
		{
			if (isTestPad)
				allocBottom = allocPrime;
			else 
				allocBottom = allocGetat;
		}
	}
}

int CAeroflexWrite::checkDFTResult(FileStruct *pPcbFile)
{
	WORD dataLinkKey = (WORD)m_pDoc.RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
	WORD netnameKey = (WORD)m_pDoc.RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);

	// This map keeps all the testpoints that are already checked
	CMapStringToString testPointMap;

	// 1st check access point
	BlockStruct *block = pPcbFile->getBlock();
	POSITION pos = block->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = block->getNextDataInsert(pos);
		if (data == NULL)
			continue;

		InsertStruct *insert =  data->getInsert();
		if (insert == NULL)
			continue;

		DataStruct *testProbeData = NULL;
		DataStruct *accesPointData = NULL;
		InsertStruct *testProbeInsert = NULL;
		InsertStruct *accessPointInsert = NULL;

		if (insert->getInsertType() == insertTypeTestProbe)
		{
			testProbeData = data;
			testProbeInsert = insert;
		}
		else if (insert->getInsertType() == insertTypeTestAccessPoint)
		{
			accesPointData = data;
			accessPointInsert = insert;
		}
		else
		{
			continue;
		}

		// Check if there is test probe to get test point
		if (testProbeData != NULL)
		{
			// If there no DDLink in test probe to point to a access point then skip
			Attrib *attrib = NULL;
			if (testProbeData->getAttributes() == NULL || !testProbeData->getAttributes()->Lookup(dataLinkKey, attrib) || attrib == NULL)
				continue;

			// If the entity is not a DATA then skip
			//EEntityType entityType;
			//void *voidPtr = FindEntity(&m_pDoc, attrib->getIntValue(), entityType);
         CEntity accessEntity = CEntity::findEntity(m_pDoc.getCamCadData(), attrib->getIntValue());

         if (accessEntity.getEntityType() != entityTypeData)
				continue;

			// Get the access point from DATALINK
         accesPointData = accessEntity.getData();
			accessPointInsert = data->getInsert();  // 20051015 knv - should this be { accessPointInsert = accesPointData->getInsert(); }
         //accessPointInsert = accesPointData->getInsert();
		}

		// If there is access point then get the entity number of via, testpad, or comppin from DATALINK
		if (accesPointData != NULL &&  accessPointInsert != NULL)
		{
			// If test point is already checked then skip
			CString accessPointName = accessPointInsert->getRefname();
			if (testPointMap.Lookup(accessPointName, accessPointName))
				continue;
			testPointMap.SetAt(accessPointName, accessPointName);


			// If there no DDLink in access point to via, testpad, or comppin then skip
			// And if the entity is not DATA and not COMPPIN then skip
			Attrib *attrib = NULL;
			if (accesPointData->getAttributes() == NULL || !accesPointData->getAttributes()->Lookup(dataLinkKey, attrib) || attrib == NULL)
				continue;

			int entityNum = attrib->getIntValue();
         CEntity accessEntity = CEntity::findEntity(m_pDoc.getCamCadData(), entityNum);

         if (accessEntity.getEntityType() != entityTypeData && 
             accessEntity.getEntityType() != entityTypeCompPin)
				continue;

			// If there is not netname then skip
			attrib = NULL;

			if (!data->getAttributes()->Lookup(netnameKey, attrib) || attrib == NULL)
				continue;

			CString netName = attrib->getStringValue();

			// Get the item type
			EItemType itemType = typeUnknown;

			if (accessEntity.getEntityType() == entityTypeData)
			{
            DataStruct* tmpData = accessEntity.getData();

				if (tmpData->getInsert()->getInsertType() == insertTypeVia || 
               tmpData->getInsert()->getInsertType() == insertTypeBondPad)
					itemType = typeVia;
				else
					itemType = typeTestpad;
			}
			else if (accessEntity.getEntityType() == entityTypeCompPin)
			{
            CompPinStruct *cp = accessEntity.getCompPin();
            DataStruct *tmpData = pPcbFile->getBlock()->FindData(cp->getRefDes());

            if (tmpData != NULL && tmpData->isInsertType(insertTypeTestPoint))
               itemType = typeTestpad;
            else
               itemType = typeComppin;
			}

			// Get the cluster
			CCluster *cluster = m_clusterMap.FindCluster(netName);

			if (cluster == NULL)
				continue;

			// Set the allocate
			if (testProbeInsert != NULL)
			{
				cluster->SetItemAllocate(entityNum, allocPrime, itemType, testProbeInsert->getGraphicMirrored());
			}
			else
			{
				cluster->SetItemAllocate(entityNum, allocGetat, itemType, accessPointInsert->getGraphicMirrored());
			}
		}
	}

	return 0;
}

int CAeroflexWrite::loadSettings(CString fileName)
{
   FILE *fp = fopen(fileName, "rt");
   if ((fp ) == NULL)
   {
      CString tmp;
      tmp.Format("File [%s] not found.", fileName);
      ErrorMessage(tmp, "Aeroflex CB Write Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

	m_bExportUnloaded = true;
	m_bWriteGraphics = true;
	m_bWriteTraces = true;
	m_eDFTSource = DEFAULT_SOURCE;
	m_eUnits = DEFAULT_AEROFLEX_UNITS;
              
   char line[255];
   while (fgets(line, 255, fp))
   {
      CString w = line;
      char *lp = get_string(line, " \t\n");
      if (lp == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".EXPORT_UNLOADED"))
         {
            lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            if (toupper(lp[0]) == 'N')
					m_bExportUnloaded = false;
         }
         else if (!STRICMP(lp, ".WRITE_GRAPHICS"))
         {
            lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            if (toupper(lp[0]) == 'N')
					m_bWriteGraphics = false;
         }
         else if (!STRICMP(lp, ".WRITE_TRACES"))
         {
            lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            if (toupper(lp[0]) == 'N')
					m_bWriteTraces = false;
         }
         else if (!STRICMP(lp, ".DFT_SOURCE"))
         {
            lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            if (!STRICMP(lp, "TESTATTR"))
					m_eDFTSource = sourceTestAttr;
				else if (!STRICMP(lp, "ACCESS"))
					m_eDFTSource = sourceAccess;
         }
         else if (!STRICMP(lp, ".OUTPUT_UNITS"))
         {
            lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

				if (!STRICMP(lp, "UM"))
					m_eUnits = unitMicrons;
				else if (!STRICMP(lp, "THOU"))
					m_eUnits = unitMils;
         }
		}
	}

	if (m_eUnits == unitMils)
		m_dUnitFactor = Units_Factor(m_pDoc.getSettings().getPageUnits(), UNIT_MILS) * m_dScale;
	else
		m_dUnitFactor = Units_Factor(m_pDoc.getSettings().getPageUnits(), UNIT_MICRONS) * m_dScale;

   fclose(fp);
   return 1;
}

int CAeroflexWrite::WriteFile(CString fileName)
{
   // open file for writing
	CFormatStdioFile outFile;
	if (!outFile.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fileName);
      ErrorMessage(tmp);
      getApp().UpdateExitCode(ExitCodeExportFailed);
      return -1;
   }


	// Prepare for output
   CString settingsFile( getApp().getExportSettingsFilePath("aeroflex.out") );
   {
      CString msg;
      msg.Format("Aeroflex: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
	loadSettings(settingsFile);
	getStackLayer();

	FileStruct *pPcbFile = m_pDoc.getFileList().GetFirstShown(blockTypePcb);

	if (pPcbFile == NULL)
   {
      ErrorMessage("No currently visible PCB File.", "Aeroflex export aborted.");
      getApp().UpdateExitCode(ExitCodeExportFailed);
      return -1;
   }

   CString nameCheckFile( getApp().getSystemSettingsFilePath("Aeroflex.chk") );
   check_init(nameCheckFile, /*do not raise case*/ false);

	if (m_eDFTSource == sourceAccess)
	{
		if (isThereTestProbe(pPcbFile) == true)
		{
			m_eTestSide = testSurfaceBottom;

			/*if (m_pDoc.GetCurrentDFTSolution(*pPcbFile) != NULL)
			{
				if (m_pDoc.GetCurrentDFTSolution(*pPcbFile)->GetTestPlan()->GetProbeSide() == testSurfaceBoth)
					m_eTestSide = m_pDoc.GetCurrentDFTSolution(*pPcbFile)->GetTestPlan()->GetProbeSidePreference();
				else
					m_eTestSide = m_pDoc.GetCurrentDFTSolution(*pPcbFile)->GetTestPlan()->GetProbeSide();
			}*/

			gatherCluster(pPcbFile, false);
			checkDFTResult(pPcbFile);
		}
		else
		{
			ErrorMessage("DFT_SOURCE is ACCESS, but there is no Probe Placement result.  Run Probe Placement first and try again.", "Error");
			check_deinit();
         getApp().UpdateExitCode(ExitCodeExportFailed);  // To let vPlan know export failed.
			return -1;
		}
	}
	else if (m_eDFTSource == sourceTestAttr)
	{
		m_eTestSide = testSurfaceBottom;
		gatherCluster(pPcbFile, true);
	}
	
	// Output the file
	writeCB_Description(outFile, pPcbFile);
	writeCB_NetAttributes(pPcbFile->getNetList());

	// Close output file and log file
	outFile.Close();

   CTime t;
   t = t.GetCurrentTime();
   fprintf(m_logFp, "%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));
	fprintf(m_logFp, "\nNames Changed In Output\n");
	if (check_report(m_logFp) < 1) fprintf(m_logFp, "... None.\n");
	fprintf(m_logFp, "\n");
   fprintf(m_logFp, "Aeroflex log file end.\n");

	check_deinit();

   return 0;
}

int CAeroflexWrite::writeCB_Description(CFormatStdioFile &file, FileStruct *pPcbFile)
{
	int indent = 0;
	file.WriteString("(CB_DESCRIPTION\n");
	
	indent += 3;
	writeHeader(file, indent, pPcbFile);
	writeEnvironment(file, indent, pPcbFile);
	writeComponents(file, indent, pPcbFile);
	writeConnections(file, indent, pPcbFile);
	indent -= 3;

	file.WriteString(")\n");

	return 0;
}
int CAeroflexWrite::writeCB_NetAttributes(CNetList &netList)
{
   POSITION netPos = netList.GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = netList.GetNext(netPos);
      if(net)
      {
         char *netName = check_name('n', net->getNetName());
         net->setAttrib(m_pDoc.getCamCadData(), m_pDoc.getStandardAttributeKeywordIndex(standardAttributeCBNetname), valueTypeString, (void*)netName, attributeUpdateOverwrite, NULL);
      }
   }
   return 0;
}

int CAeroflexWrite::writeHeader(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	file.WriteString("%*s(NAME \"%s\")\n", indent, " ", pPcbFile->getName());

   CTime t = t.GetCurrentTime();
	file.WriteString("%*s(ISSUE \"%s\")\n", indent, " ", t.Format("%A, %B %d, %Y at %H:%M:%S"));


	// Write this information as comments
	/* 
	(* ======================================================================== *)
	(* Pin Count on Bottom Surface  : 41                                        *)
	(* Pin Count on Top Surface     : 267                                       *)
	(* ======================================================================== *)
	(* Test Surface Name            : [Top (Component) Surface], TestSide : 2   *)
	(* Total Probe Count            : 108                                       *)
	(* Probe Count on Bottom Surface: 0                                         *)
	(* Probe Count on Top Surface   : 108                                       *)
	(* Placed Probe Count           : 108                                       *)
	(* Unplaced Probe Count         : 0                                         *)
	(* ======================================================================== *)
	*/

	int pinCountBottom = 0;
	int pinCountTop = 0;
	int testSide = 0;
	int totalProbeCount = 0;
	int probeCountBottom = 0;
	int probeCountTop = 0;
	int placedProbeCount = 0;
	int unplacedProbeCount = 0;
	// this section is commented out for case #1153
	//file.WriteString("%*s(* ======================================================================== *)\n", indent, " ");
	//file.WriteString("%*s(* Pin Count on Bottom Surface  : %d *)\n", indent, " ", pinCountBottom);
	//file.WriteString("%*s(* Pin Count on Top Surface     : %d *)\n", indent, " ", pinCountTop);
	//file.WriteString("%*s(* ======================================================================== *)\n", indent, " ");
	//file.WriteString("%*s(* Test Surface Name            : [Top (Component) Surface], TestSide : %d *)\n", indent, " ", testSide);
	//file.WriteString("%*s(* Total Probe Count            : %d *)\n", indent, " ", totalProbeCount);
	//file.WriteString("%*s(* Probe Count on Bottom Surface: %d *)\n", indent, " ", probeCountBottom); 
	//file.WriteString("%*s(* Probe Count on Top Surface   : %d *)\n", indent, " ", probeCountTop); 
	//file.WriteString("%*s(* Placed Probe Count           : %d *)\n", indent, " ", placedProbeCount);
	//file.WriteString("%*s(* Unplaced Probe Count         : %d *)\n", indent, " ", unplacedProbeCount);
	//file.WriteString("%*s(* ======================================================================== *)\n", indent, " ");	

	return 0;
}

/******************************************************************************
* CAeroflexWrite::writeEnvironment
*/
int CAeroflexWrite::writeEnvironment(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	file.WriteString("%*s(ENVIRONMENT\n", indent, " ");	
	indent += 3;


	// Output units
	if (m_eUnits == unitMicrons)
		file.WriteString("%*s(UNITS UM (SCALE %d))\n", indent, " ", round(m_dScale));
	else 
		file.WriteString("%*s(UNITS THOU (SCALE %d))\n", indent, " ", round(m_dScale));

	// Output extents
	CExtent extent = m_pDoc.calcFileOutlineExtents(pPcbFile);
	if (!extent.isValid())
	{
		m_pDoc.CalcBlockExtents(pPcbFile->getBlock());
		extent = pPcbFile->getBlock()->getExtent();
	}

	int minX = round(extent.getMin().x * m_dUnitFactor);
	int minY = round(extent.getMin().y * m_dUnitFactor);
	int maxX = round(extent.getMax().x * m_dUnitFactor);
	int maxY = round(extent.getMax().y * m_dUnitFactor);
	file.WriteString("%*s(EXTENT %d %d %d %d)\n", indent, " ", minX, minY, maxX, maxY);


	// Output layer definition
	int testSide = GetBottomAeroflexLayer();
	if (m_eTestSide == testSurfaceTop)
		testSide = GetTopAeroflexLayer();

	file.WriteString("%*s(LAYERDEF %d %d (TESTSIDE %d))\n",
		indent, " ", GetBottomAeroflexLayer(), GetTopAeroflexLayer(), testSide);


	// Output all polys
	writeBoardShape(file, indent, pPcbFile);
	if (m_bWriteGraphics)
	{
		writeShapeDefine(file, indent);
	}
//	writeTextDefine(file, indent);

	writeToolHoleGeometries(file, indent, pPcbFile);
	writeToolHoleInstances(file, indent, pPcbFile);


	indent -= 3;
	file.WriteString("%*s)\n", indent, " ");

	return 0;
}

int CAeroflexWrite::writeBoardShape(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	file.WriteString("%*s(SH\n", indent, " ");
	indent += 3;

	CDataList& dataList = pPcbFile->getBlock()->getDataList();
	POSITION pos = dataList.GetHeadPosition();
	while(pos)
	{
		const DataStruct* data = dataList.GetNext(pos);
		if (data == NULL || data->getDataType() == dataTypeInsert)
			continue;

		if (data->getDataType() == dataTypePoly && (data->getGraphicClass() == graphicClassBoardOutline || data->getGraphicClass() == graphicClassPanelOutline))
			writeSHPoly(m_pDoc, file, indent, data->getPolyList(), m_dUnitFactor, false, false);
		else if (data->getDataType() == dataTypeText)
			writeSHText(file, indent, data->getText());
	}

	indent -= 3;
	file.WriteString("%*s)\n", indent, " ");

	return 0;
}

int CAeroflexWrite::writeSHText(CFormatStdioFile &file, int indent,const TextStruct* text)
{
	// *** DO NOT write text for now, if write text then need to uncomment codes int writeTextDefine()

//	file.WriteString("%*s(M %d %d)\n", indent, " ", round(text->getOrigin().x * m_dUnitFactor), round(text->getOrigin().y * m_dUnitFactor));

//	CString textTypeIndex = getTextTypeID(round(text->getWidth() * m_dUnitFactor), round(text->getHeight() *m_dUnitFactor)) ;	
//	file.WriteString("%*s(TEXT \"%s\" %d %s)\n", indent, " ", text->getText(), round(text->getRotation()), textTypeIndex);

	return 0;
}

int CAeroflexWrite::writeShapeDefine(CFormatStdioFile &file, int indent)
{
	for (int i=0; i<m_pDoc.getMaxBlockIndex(); i++)
	{
		BlockStruct *block = m_pDoc.getBlockAt(i);
		if (block == NULL || block->getBlockType() != blockTypePcbComponent)
			continue;


		bool hasCompOutline = false;
		POSITION pos = block->getHeadDataPosition();
		while (pos)
		{
			DataStruct *data = block->getNextData(pos);
			if (data == NULL || data->getDataType() != dataTypePoly || data->getGraphicClass() != graphicClassComponentOutline)
				continue;

			hasCompOutline = true;
			break;
		}


		if (hasCompOutline == true)
		{
			// Write top shape definition
			CString shapeID = getShapeDefineID(block, false);

			file.WriteString("%*s(* %s *)\n", indent, " ", block->getName());
			file.WriteString("%*s(DEFINE %s\n", indent, " ", shapeID);
			indent += 3;
			file.WriteString("%*s(SH\n", indent, " ");
			indent += 3;

			pos = block->getHeadDataPosition();
			while (pos != NULL)
			{
				DataStruct *data = block->getNextData(pos);
				if (data == NULL || data->getDataType() != dataTypePoly || data->getGraphicClass() != graphicClassComponentOutline)
					continue;

				writeSHPoly(m_pDoc, file, indent, data->getPolyList(), m_dUnitFactor, false, false);
			}

			indent -= 3;
			file.WriteString("%*s)\n", indent, " ");
			indent -= 3;
			file.WriteString("%*s)\n", indent, " ");


			// Write bottom shape definition
			shapeID = getShapeDefineID(block, true);

			file.WriteString("%*s(* %s mirrored *)\n", indent, " ", block->getName());
			file.WriteString("%*s(DEFINE %s\n", indent, " ", shapeID);
			indent += 3;
			file.WriteString("%*s(SH\n", indent, " ");
			indent += 3;

			pos = block->getHeadDataPosition();
			while (pos != NULL)
			{
				DataStruct *data = block->getNextData(pos);
				if (data == NULL || data->getDataType() != dataTypePoly || data->getGraphicClass() != graphicClassComponentOutline)
					continue;

				writeSHPoly(m_pDoc, file, indent, data->getPolyList(), m_dUnitFactor, false, true);
			}

			indent -= 3;
			file.WriteString("%*s)\n", indent, " ");
			indent -= 3;
			file.WriteString("%*s)\n", indent, " ");
		}
	}

	return 0;
}

int CAeroflexWrite::writeTextDefine(CFormatStdioFile &file, int indent)
{
	// *** DO NOT write TEXT DEFINE since we are not writing TEXT

	//POSITION pos = m_textTypeMap.GetStartPosition();
	//while (pos)
	//{
	//	int curPos = 0;
	//	CString textWidthHeight = "";
	//	CString typeID = "";
	//	m_textTypeMap.GetNextAssoc(pos, textWidthHeight, typeID);

	//	CString width = textWidthHeight.Tokenize(" ", curPos).Trim();
	//	if (width.IsEmpty())
	//		width = "0";

	//	CString height = textWidthHeight.Tokenize(" ", curPos).Trim();
	//	if (height.IsEmpty())
	//		height = "0";

	//	file.WriteString("%*s(TEXT_DEFINE %s %s %s)\n", indent, " ", typeID, width, height);
	//}

	return 0;
}

DbUnit CAeroflexWrite::getToolHoleDiameter(BlockStruct *block)
{
	// block is  ptr to inserted geometry, ie geom block inserted by tool hole instance

	// Use diameter of first pin encountered
	POSITION pos = block->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *pinData = block->getNextDataInsert(pos);
		if (pinData && pinData->getInsert() &&
			pinData->getInsert()->getInsertType() == insertTypePin)
		{
			int insertedPinGeomBlockNum = pinData->getInsert()->getBlockNumber();
			BlockStruct *insertedPinGeomBlock = m_pDoc.getBlockAt(insertedPinGeomBlockNum);
			if (insertedPinGeomBlock != NULL)
			{
				POSITION pos2 = insertedPinGeomBlock->getHeadDataInsertPosition();
				while (pos2)
				{
					DataStruct *insertedHoleData = insertedPinGeomBlock->getNextDataInsert(pos2);
					int jj = 0;
					BlockStruct *insertedHoleBlk = m_pDoc.getBlockAt(insertedHoleData->getInsert()->getBlockNumber());
					if (insertedHoleBlk && insertedHoleBlk->getBlockType() == blockTypeDrillHole)
					{
						DbUnit holeSize = (DbUnit)insertedHoleBlk->getToolSize();
						return holeSize;	
					}

				}

			}
		}
	}

	return 0.0;
}

int CAeroflexWrite::writeToolHoleGeometries(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	int pcblocNum = 1;
	CDataList& dataList = pPcbFile->getBlock()->getDataList();
	POSITION pos = dataList.GetHeadPosition();
	while(pos)
	{
		const DataStruct* data = dataList.GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		const InsertStruct* insert = data->getInsert();
		if (insert == NULL || (insert->getInsertType() != insertTypeDrillTool && insert->getInsertType() != insertTypeDrillHole))
			continue;


		BlockStruct *block = m_pDoc.getBlockAt(insert->getBlockNumber());
		if (block != NULL)
		{
			CString blockName = block->getName();
			CString mappedName;
			if (!m_toolHoleGeomDefinitionMap.Lookup(blockName, mappedName))
			{
				mappedName.Format("PCBLOC%d", pcblocNum++);
				m_toolHoleGeomDefinitionMap.SetAt(blockName, mappedName);
				
				// Output looks like:
				//(DEFINE PCBLOCn
				// (SH
				//  (CIRCLE 0 0 <diameter>)))

				DbUnit diameter = getToolHoleDiameter(block);

				file.WriteString("%*s(DEFINE %s\n", indent, " ", mappedName);
				file.WriteString("%*s (SH\n", indent, " ");
				file.WriteString("%*s  (CIRCLE 0 0 %d)))\n", indent, " ", round(diameter * m_dUnitFactor));
			}
		}
	
	}

	return 0;
}

int CAeroflexWrite::writeToolHoleInstances(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	CDataList& dataList = pPcbFile->getBlock()->getDataList();
	POSITION pos = dataList.GetHeadPosition();
	while(pos)
	{
		const DataStruct* data = dataList.GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		const InsertStruct* insert = data->getInsert();
		if (insert == NULL || (insert->getInsertType() != insertTypeDrillTool && insert->getInsertType() != insertTypeDrillHole))
			continue;


		BlockStruct *block = m_pDoc.getBlockAt(insert->getBlockNumber());
		if (block != NULL) {
			CString blockName = block->getName();
			CString mappedBlockName;
			if (!m_toolHoleGeomDefinitionMap.Lookup(blockName, mappedBlockName))
				mappedBlockName = "MissingMappedBlockName";

			//Use original block  name
			// note - if you change to this style, you also have to change
			// writeToolHoleGeometries() to use the block name instead of mappedName.
			//file.WriteString("%*s(LOCATE (AT %d %d) (GTYPE %s) (COMMENT %s))\n", indent, " ",
			//		round(insert->getOriginX() * m_dUnitFactor), round(insert->getOriginY() * m_dUnitFactor), 
			//		block->getName(), insert->getRefname());
			
			// Use mapped block name, include refdes in comment
			//file.WriteString("%*s(LOCATE (AT %d %d) (GTYPE %s) (COMMENT %s))\n", indent, " ",
			//		round(insert->getOriginX() * m_dUnitFactor), round(insert->getOriginY() * m_dUnitFactor), 
			//		mappedBlockName, insert->getRefname());

			// Use mapped block name, no refdes comment
			file.WriteString("%*s(LOCATE (AT %d %d) (GTYPE %s))\n", indent, " ",
					round(insert->getOriginX() * m_dUnitFactor), round(insert->getOriginY() * m_dUnitFactor), 
					mappedBlockName);
		}
	}

	return 0;
}

/******************************************************************************
* CAeroflexWrite::writeComponents
*/
int CAeroflexWrite::writeComponents(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	file.WriteString("%*s(COMPONENTS\n", indent, " ");	
	indent += 3;

	writeDevice(file, indent, pPcbFile);

	indent -= 3;
	file.WriteString("%*s)\n", indent, " ");

	return 0;
}

int CAeroflexWrite::writeDevice(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	WORD partNumKey = (WORD)m_pDoc.RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);
	WORD loadedKey = (WORD)m_pDoc.RegisterKeyWord(ATT_LOADED, 0, VT_STRING);

	BlockStruct *block = pPcbFile->getBlock();
	POSITION pos = block->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = block->getNextDataInsert(pos);
		if (data == NULL)
			continue;

		InsertStruct *insert = data->getInsert();
		if (insert == NULL || insert->getInsertType() != insertTypePcbComponent)
			continue;

		if (m_bExportUnloaded == false)
		{
			Attrib *attrib = NULL;
			if (data->getAttributes() && data->getAttributes()->Lookup(loadedKey, attrib) && attrib != NULL)
			{
				CString loaded = attrib->getStringValue();
				if (loaded.Trim().CompareNoCase("FALSE") == 0)
					continue;
			}
		}

		int layerIndex = GetBottomAeroflexLayer();
		if (insert->getGraphicMirrored() == false)
			layerIndex = GetTopAeroflexLayer();

		CString shapeID = findShapeDefineID(insert->getBlockNumber(), insert->getGraphicMirrored());
      bool written = false;
		if (m_bWriteGraphics)
		{
			if (!shapeID.Trim().IsEmpty())
			{
				file.WriteString("%*s(DEV %s (AT %d %d) (Z %d) (ROTATE %d) (GTYPE %s)\n", indent, " ",
						check_name('c', insert->getRefname()), 
						round(insert->getOriginX() * m_dUnitFactor), round(insert->getOriginY() * m_dUnitFactor),
						layerIndex, round(insert->getAngleDegrees()), shapeID);
            written = true;
			}
			else
			{
            CString blockName;
            BlockStruct *insertedBlk = m_pDoc.getBlockAt(insert->getBlockNumber());
            if (insertedBlk != NULL)
               blockName = insertedBlk->getName();
				fprintf(m_logFp, "Warning: DEV (%s) has no Primary Component Outline in geometry block (%s)\n", insert->getRefname(), blockName);
				m_bDisplayError = true;
				// Case 2127, will get same output as if not writing graphics
			}
		}

      // Case 2127, Output for WRITE_GRAPHICS N (always) and for WRITE_GRAPHICS Y but comp had
      // no outline.
      if (!written)
		{
			file.WriteString("%*s(DEV %s (AT %d %d) (Z %d)\n", indent, " ", 
				check_name('c', insert->getRefname()), 
				round(insert->getOriginX() * m_dUnitFactor), round(insert->getOriginY() * m_dUnitFactor),
				layerIndex);	
		}
		indent += 3;

		if (data->getAttributes())
		{
			Attrib *attrib = NULL;
			if (data->getAttributes()->Lookup(partNumKey, attrib) && attrib!= NULL)
			{
				CString stockCode = attrib->getStringValue();
				file.WriteString("%*s(STOCK \"%s\")\n", indent, " ", check_name('b', stockCode));
			}
		}
      
      int pinCount = 0;
      if (m_pDoc.getBlockAt(insert->getBlockNumber())!= NULL)
      {
		   pinCount = m_pDoc.getBlockAt(insert->getBlockNumber())->getPinCount();
         //int pinCount = m_pDoc.getBlockAt(insert->getBlockNumber())->getPinCount();
      }
		writeType(data, pinCount, file, indent);

		indent -= 3;
		file.WriteString("%*s)\n", indent, " ");
	}

	return 0;
}

int CAeroflexWrite::writeType(DataStruct *data, int pinCount, CFormatStdioFile &file, int indent)
{
	if (data->getAttributes() == NULL)
		return 0;

	WORD AeroflexSubClassKey = (WORD)m_pDoc.RegisterKeyWord(ATT_AEROFLEX_SUBCLASS, 0, VT_STRING);
	WORD subClassKey = (WORD)m_pDoc.RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);
	WORD deviceTypeKey = (WORD)m_pDoc.RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
	WORD valueKey = (WORD)m_pDoc.RegisterKeyWord(ATT_VALUE, 0, VT_STRING);
	WORD plusTolKey = (WORD)m_pDoc.RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);
	WORD minusTolKey = (WORD)m_pDoc.RegisterKeyWord(ATT_MINUSTOLERANCE, 0, VT_DOUBLE);

	Attrib *attrib = NULL;
	CString deviceType = "";
	CString subClass = "";
	CString value = "";
	double plusTol = DBL_MAX;
	double minusTol = DBL_MAX;

	// Get all the required attributes
	if (data->getAttributes()->Lookup(deviceTypeKey, attrib) && attrib != NULL)
		deviceType = attrib->getStringValue().Trim();

	if (data->getAttributes()->Lookup(AeroflexSubClassKey, attrib) && attrib != NULL)
		subClass = attrib->getStringValue().Trim();
	else if (data->getAttributes()->Lookup(subClassKey, attrib) && attrib != NULL)
		subClass = attrib->getStringValue().Trim();

	if (data->getAttributes()->Lookup(valueKey, attrib) && attrib != NULL)
		value = attrib->getStringValue().Trim();

	if (data->getAttributes()->Lookup(plusTolKey, attrib) && attrib != NULL)
		plusTol = attrib->getDoubleValue();

	if (data->getAttributes()->Lookup(minusTolKey, attrib) && attrib != NULL)
		minusTol = attrib->getDoubleValue();

	getDeviceTypeInfo(deviceType, subClass, pinCount, value, plusTol, minusTol);


	// Format the parameter string
	CString parameter = "";
	if (!value.IsEmpty())
		parameter = value;

	if (plusTol != DBL_MAX)
		parameter.AppendFormat(",%0.3f", plusTol);

	if (minusTol != DBL_MAX)
		parameter.AppendFormat(",%0.3f", minusTol);


	// Output TYPE and PAR
	if (parameter.IsEmpty())
		file.WriteString("%*s(TYPE \"%s\") (LEGS %d)\n", indent, " ", check_name('t', deviceType), pinCount);
	else
		file.WriteString("%*s(TYPE \"%s\" (PAR \"%s\")) (LEGS %d)\n", indent, " ", check_name('t', deviceType), parameter, pinCount);

	return 0;
}


/******************************************************************************
* CAeroflexWrite::writeConnections
*/
int CAeroflexWrite::writeConnections(CFormatStdioFile &file, int indent, FileStruct *pPcbFile)
{
	file.WriteString("%*s(CONNECTIONS\n", indent, " ");	
	indent += 3;

	if (m_bWriteTraces == true)
		m_clusterMap.WriteClusters(file, indent, m_dUnitFactor, true, m_eTestSide, m_bExportUnloaded);
	else
		m_clusterMap.WriteClusters(file, indent, m_dUnitFactor, false, m_eTestSide, m_bExportUnloaded);

	indent -= 3;
	file.WriteString("%*s)\n", indent, " ");

	return 0;
}


/******************************************************************************
* CClusterItem
*/
CClusterItem::CClusterItem(int entityNum, CString refName, CString pinName, double x, double y, double rotation,
									int layer1, int layer2, EItemType type, EAllocate allocTop, EAllocate allocBottom, 
									bool isLoaded, CCluster *cluster)
{
	m_iEntityNum = entityNum;
	m_sRefname = refName;
	m_sPinname = pinName;
	m_eAllocTop = allocTop;
	m_eAllocBottom = allocBottom;
	m_iX = x;
	m_iY = y;
	m_iRotation = rotation;
	m_iLayer1 = layer1;
	m_iLayer2 = layer2;
	m_eType = type;
	m_bIsLoaded = isLoaded;
	m_pCluster = cluster;
}

CClusterItem::~CClusterItem()
{
}

void CClusterItem::SetProperty(double x, double y, double rotation, int layer1, int layer2)
{
	m_iX = x;
	m_iY = y;
	m_iRotation = rotation;
	m_iLayer1 = layer1;
	m_iLayer2 = layer2;
}

void CClusterItem::SetAllocTop(EAllocate allocate)
{
	m_eAllocTop = allocate;
}

void CClusterItem::SetAllocBottom(EAllocate allocate)
{
	m_eAllocBottom = allocate;
}

int CClusterItem::WriteItem(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface, bool exportUnloaded)
{
	if (exportUnloaded == false && m_bIsLoaded == false)
		return 0;

	switch (m_eType)
	{
	case typeVia:
		writeVia(file, indent, unitFactor, preferSurface);
		break;

	case typeTestpad:
		writeTestPad(file, indent, unitFactor, preferSurface);
		break;

	case typeComppin:
		writeComppin(file, indent, unitFactor, preferSurface);
		break;

	default:
		break;
	}

	return 0;
}

CString CClusterItem::getLayerString()
{
	// The layers output depend on the ALLOC string.
	// But we can just check the ALLOC flags, as they determine the ALLOC string.
	// The two layers are being output, it is always bottom first, then top.
	// Otherwise, if it is only one, it's the one that it is.
	// A third situation may arise (though I'm not sure how) where neither
	// allocTop or allocBot are "known". In this case, default to the
	// insert level.

	CString layerStr;


	if (m_eAllocBottom != allocUnknown)
	{
		layerStr.Format("%d", GetCluster()->GetBottomAeroflexLayer());
	}

	if (m_eAllocTop != allocUnknown)
	{
		if (!layerStr.IsEmpty())
			layerStr += " ";

		layerStr.AppendFormat("%d", GetCluster()->GetTopAeroflexLayer());
	}

	// If layerStr is still empty, default to insert layer
	if (layerStr.IsEmpty())
		layerStr.Format("%d", m_iLayer1);

	return layerStr;
}

CString CClusterItem::getAllocString(ETestSurface preferSurface)
{
#ifdef ORIGINAL_ALLOC_STRING
	// 9/23/05, no case #, Mark relayed that his contact at Aeroflex reports that the 
	// Aeroflex spec is wrong. The order of alloc strings should not change depending
	// on the test surface, preferred side, or anything else.
	// The order should always be bottom first and top second, if it is a two-value
	// alloc string. If there is only one value in the alloc string, then it is
	// of course the side the probe is on.
	// If the report is correct, then all this double side code is wrong, and
	// the actual need is much simpler, as it can be done in the same order all the time.

	CString allocates = ALLOCATE_MASKED;

	if (m_eAllocTop != allocUnknown && m_eAllocBottom != allocUnknown)
	{
		// This is THRU because it has allocate for both top and bottom;
		// therefore we care about the order of allocate, prefer surface is first then non prefer surface

		if (preferSurface == testSurfaceTop)
		{
			// Prefer surface is TOP
			if (m_eAllocTop == allocGetat)
				allocates = ALLOCATE_GETAT;
			else if (m_eAllocTop == allocPrime)
				allocates = ALLOCATE_PRIME;
			else
				allocates = ALLOCATE_MASKED;

			// Non prefer surface is BOTTOM
			if (m_eAllocBottom == allocMasked)
				allocates.AppendFormat(" %s",  ALLOCATE_MASKED);
			else if (m_eAllocBottom == allocGetat)
				allocates.AppendFormat(" %s",  ALLOCATE_GETAT);
			else if (m_eAllocBottom == allocPrime)
				allocates.AppendFormat(" %s",  ALLOCATE_PRIME);
		}
		else
		{
			// Prefer surface is BOTTOM
			if (m_eAllocBottom == allocGetat)
				allocates = ALLOCATE_GETAT;
			else if (m_eAllocBottom == allocPrime)
				allocates = ALLOCATE_PRIME;
			else
				allocates = ALLOCATE_MASKED;

			// Non prefer surface is TOP
			if (m_eAllocTop == allocMasked)
				allocates.AppendFormat(" %s",  ALLOCATE_MASKED);
			else if (m_eAllocTop == allocGetat)
				allocates.AppendFormat(" %s",  ALLOCATE_GETAT);
			else if (m_eAllocTop == allocPrime)
				allocates.AppendFormat(" %s",  ALLOCATE_PRIME);
		}
	}
	else
	{
		// This is SMD; therefore we don't care about prefer surface, we only output one allocate
		if (m_eAllocTop != allocUnknown)
		{
			if (m_eAllocTop == allocGetat)
				allocates = ALLOCATE_GETAT;
			else if (m_eAllocTop == allocPrime)
				allocates = ALLOCATE_PRIME;
			else
				allocates = ALLOCATE_MASKED;
		}
		else if (m_eAllocBottom != allocUnknown)
		{
			if (m_eAllocBottom == allocGetat)
				allocates = ALLOCATE_GETAT;
			else if (m_eAllocBottom == allocPrime)
				allocates = ALLOCATE_PRIME;
			else
				allocates = ALLOCATE_MASKED;
		}
	}

	return allocates;
#else
	// Simpler version of creating alloc string, since order is constant.
	// Always bottom first.

	CString allocates;

	if (m_eAllocBottom != allocUnknown)
	{
		if (m_eAllocBottom == allocGetat)
			allocates = ALLOCATE_GETAT;
		else if (m_eAllocBottom == allocPrime)
			allocates = ALLOCATE_PRIME;
		else
			allocates = ALLOCATE_MASKED;
	}
	
	if (m_eAllocTop != allocUnknown)
	{
		if (!allocates.IsEmpty())
			allocates.Append(" ");

		if (m_eAllocTop == allocGetat)
			allocates.Append(ALLOCATE_GETAT);
		else if (m_eAllocTop == allocPrime)
			allocates.Append(ALLOCATE_PRIME);
		else
			allocates.Append(ALLOCATE_MASKED);
	}

	if (allocates.IsEmpty())
		allocates = ALLOCATE_MASKED;  // default, to match what original code did, presumed correct

	return allocates;

#endif
}

int CClusterItem::writeVia(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface)
{
	CString allocates = getAllocString(preferSurface);

	if (m_sRefname.Trim().IsEmpty())
	{
		file.WriteString("%*s(V %d %d %d %d (ALLOC %s))\n", indent, " ",
				round(m_iX * unitFactor), round(m_iY * unitFactor), m_iLayer1, m_iLayer2, allocates);
	}
	else
	{
		file.WriteString("%*s(V %d %d %d %d (ALLOC %s)) (* %s *)\n", indent, " ",
				round(m_iX * unitFactor), round(m_iY * unitFactor), 
				m_iLayer1, m_iLayer2, allocates, m_sRefname);
	}

	return 0;
}

int CClusterItem::writeTestPad(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface)
{
	CString allocates = getAllocString(preferSurface);

   CString refnameComment;
   if (!m_sRefname.IsEmpty())
      refnameComment.Format(" (* %s *)", m_sRefname);

	file.WriteString("%*s(P %d %d %d (ALLOC %s))%s\n", indent, " ",
			round(m_iX * unitFactor), round(m_iY * unitFactor), m_iLayer1, allocates, refnameComment);

	return 0;
}

int CClusterItem::writeComppin(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface)
{
	CString allocates = getAllocString(preferSurface);
	CString layers = getLayerString();

	file.WriteString("%*s(L %s %s (AT %d %d) (Z %s) (ALLOC %s))\n", indent, " ",
			check_name('c', m_sRefname), m_sPinname, round(m_iX  * unitFactor), round(m_iY * unitFactor),
			layers, allocates);

	return 0;
}


/******************************************************************************
* CClusterItemMap
*/
CClusterItemMap::CClusterItemMap()
{
	DeleteAllItems();
}

CClusterItemMap::~CClusterItemMap()
{
	DeleteAllItems();
}

void CClusterItemMap::DeleteAllItems()
{
	POSITION pos = this->GetStartPosition();
	while (pos)
	{
		CString key = "";
		CClusterItem *clusterItem = NULL;
		this->GetNextAssoc(pos, key, clusterItem);
		delete clusterItem;
		clusterItem = NULL;
	}
	this->RemoveAll();
}

CClusterItem*  CClusterItemMap::AddItem(int entityNum, CString refName, CString pinName, double x, double y, double rotation,
													 int layer1, int layer2, EItemType type, EAllocate allocTop, EAllocate allocBottom,
													 bool isLoaded, CCluster *cluster)
{
	CString key = "";
	key.Format("%d", entityNum);

	CClusterItem *clusterItem = NULL;
	if (this->Lookup(key, clusterItem))
	{
		clusterItem->SetProperty(x, y, rotation, layer1, layer2);
		clusterItem->SetAllocTop(allocTop);
		clusterItem->SetAllocBottom(allocBottom);
	}
	else
	{
		clusterItem = new CClusterItem(entityNum, refName, pinName, x, y, rotation, layer1, layer2, type, allocTop, allocBottom, isLoaded, cluster);
		this->SetAt(key, clusterItem);
	}

	return clusterItem;
}

CClusterItem* CClusterItemMap::FindItem(int entityNum)
{
	CString key = "";
	key.Format("%d", entityNum);
	CClusterItem *clusterItem = NULL;
	this->Lookup(key, clusterItem);

	return clusterItem;
}

int CClusterItemMap::WriteItems(CFormatStdioFile &file, int indent, double unitFactor, ETestSurface preferSurface, bool exportUnloaded)
{
	POSITION pos = this->GetStartPosition();
	while (pos)
	{
		CString key = "";
		CClusterItem *clusterItem = NULL;
		this->GetNextAssoc(pos, key, clusterItem);
		clusterItem->WriteItem(file, indent, unitFactor, preferSurface, exportUnloaded);
	}

	return 0;
}


/******************************************************************************
* CCluster
*/
CCluster::CCluster(CCEtoODBDoc &doc, CString netName, int startLayerIndex, int endLayerIndex) :
	m_pDoc(doc)
{
	m_iStartLayerIndex = startLayerIndex;
	m_iEndLayerIndex = endLayerIndex;
	m_sNetName = netName;
	m_viaMap.DeleteAllItems();
	m_testpadMap.DeleteAllItems();
	m_comppinMap.DeleteAllItems();
	m_trackList.RemoveAll();
}

CCluster::~CCluster()
{
	m_trackList.RemoveAll();
}

void CCluster::AddViaItem(int entityNum, CString viaName, double x, double y, double rotation,
								  int layer1, int layer2, EAllocate allocTop, EAllocate allocBottom, bool isLoaded)
{
	m_viaMap.AddItem(entityNum, viaName, "", x, y, rotation, layer1, layer2, typeVia, allocTop, allocBottom, isLoaded, this);
}

void CCluster::AddTestpadItem(int entityNum, CString testPadName, double x, double y, double rotation,
										int layer, EAllocate allocTop, EAllocate allocBottom, bool isLoaded)
{
	m_testpadMap.AddItem(entityNum, testPadName, "", x, y, rotation, layer, 0, typeTestpad, allocTop, allocBottom, isLoaded, this);
}

void CCluster::AddComppinItem(int entityNum, CString compName, CString pinName, double x, double y, double rotation,
										int layer, EAllocate allocTop, EAllocate allocBottom, bool isLoaded)
{
	m_comppinMap.AddItem(entityNum, compName, pinName, x, y, rotation, layer, 0, typeComppin, allocTop, allocBottom, isLoaded, this);
}

void CCluster::AddTrack(DataStruct *polyData)
{
	if (m_trackList.GetCount() == 0)
		m_trackList.AddHead(polyData);
	else
		m_trackList.AddTail(polyData);
}

void CCluster::SetItemAllocate(int entityNum, EAllocate allocate, EItemType type, bool isBottom)
{
	CClusterItem *item = NULL;

	switch (type)
	{
	case typeVia:
		item = m_viaMap.FindItem(entityNum);
		break;

	case typeTestpad:
		item = m_testpadMap.FindItem(entityNum);
		break;

	case typeComppin:
		item = m_comppinMap.FindItem(entityNum);
		break;

	default:
		break;
	}

	if (item != NULL)
	{
		if (isBottom)
			item->SetAllocBottom(allocate);
		else
			item->SetAllocTop(allocate);
	}
}

int CCluster::WriteCluster(CFormatStdioFile &file, int indent, double unitFactor, bool writeTrack, ETestSurface preferSurface, bool exportUnloaded)
{
	// Do not write empty CLU records

	if (!IsEmpty())
	{
		file.WriteString("%*s(CLU \"%s\"\n", indent, " ", check_name('n', m_sNetName));	
		indent += 3;

		// Case 1860
		// Order of output (by record type) should be T, P, L, V.
		// As found, order was T, L, P, V.
		// Nothing is stated about this in the Aeroflex spec (capgref.pdf) but
		// experience of the customer is the machine interface fails if this data 
		// is in any other order.
		
		if (writeTrack == true)
			writeTracks(file, indent, unitFactor);	// T records

		m_testpadMap.WriteItems(file, indent, unitFactor, preferSurface, exportUnloaded);	// P records
		m_comppinMap.WriteItems(file, indent, unitFactor, preferSurface, exportUnloaded);	// L records		
		m_viaMap.WriteItems(file, indent, unitFactor, preferSurface, exportUnloaded);			// V records
		


		indent -= 3;
		file.WriteString("%*s)\n", indent, " ");
	}

	return 0;
}

int CCluster::writeTracks(CFormatStdioFile &file, int indent, double unitFactor)
{
	POSITION pos = m_trackList.GetHeadPosition();
	while (pos)
	{
		DataStruct *polyData = (DataStruct*)m_trackList.GetNext(pos);
		if (polyData == NULL || polyData->getDataType() != dataTypePoly)
			continue;

		int layerIndex = 0;
		LayerStruct *layer = m_pDoc.getLayerAt(polyData->getLayerIndex());

		if (layer != NULL && layer->getElectricalStackNumber() > 0)
			layerIndex = MapToAeroflexLayer(layer->getElectricalStackNumber());


		file.WriteString("%*s(T %d\n", indent, " ", layerIndex);
		indent += 3;

		file.WriteString("%*s(SH\n", indent, " ");
		indent += 3;

		writeSHPoly(m_pDoc, file, indent, polyData->getPolyList(), unitFactor, true, false);

		indent -= 3;
		file.WriteString("%*s)\n", indent, " ");

		indent -= 3;
		file.WriteString("%*s)\n", indent, " ");
	}

	return 0;
}


/******************************************************************************
* CClusterMap
*/
CClusterMap::CClusterMap(CCEtoODBDoc &doc) :
	m_pDoc(doc)
{
	DeleteAllClusters();
}

CClusterMap::~CClusterMap()
{
	DeleteAllClusters();
}

void CClusterMap::DeleteAllClusters()
{
	POSITION pos = this->GetStartPosition();
	while (pos)
	{
		CString key = "";
		CCluster *cluster = NULL;
		this->GetNextAssoc(pos, key, cluster);
		delete cluster;
		cluster = NULL;
	}
	this->RemoveAll();
}

CCluster* CClusterMap::AddCluster(CString netName, int startLayerIndex, int endLayerIndex)
{
	CCluster *cluster = NULL;
	if (!this->Lookup(netName, cluster))
	{
		cluster = new CCluster(m_pDoc, netName, startLayerIndex, endLayerIndex);
		this->SetAt(netName, cluster);
	}

	return cluster;
}

CCluster* CClusterMap::FindCluster(CString netName)
{
	CCluster *cluster = NULL;
	this->Lookup(netName, cluster);
	return cluster;
}

int CClusterMap::WriteClusters(CFormatStdioFile &file, int indent, double unitFactor, bool writeTrack, 
										 ETestSurface preferSurface, bool exportUnloaded)
{
	POSITION pos = this->GetStartPosition();
	while (pos)
	{
		CString key = "";
		CCluster *cluster = NULL;
		this->GetNextAssoc(pos, key, cluster);
		cluster->WriteCluster(file, indent, unitFactor, writeTrack, preferSurface, exportUnloaded);
	}

	return 0;
}
