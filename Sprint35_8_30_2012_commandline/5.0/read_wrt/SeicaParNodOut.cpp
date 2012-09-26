// $Header: /CAMCAD/4.5/read_wrt/SeicaParNodOut.cpp 21    2/24/06 3:14p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "StdAfx.h"
#include "DeviceType.h"
#include "CompValues.h"
#include "SeicaParNodOut.h"
#include "StencilGenerator.h"
#include "StandardAperture.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"

/*
Notes (.nod file):
1) POS column
   A) Pins
      i)  Thru hole pins have a value of "T" or "B" depending on the primary access side.
      II) Surface mount pins have a value of "T" or "B" depending on the surface pad.
   B) Vias
      i)   Thru hole vias have a value of "T".
      ii)  Blind vias have a value of "T" or "B" depending on the surface pad.
      iii) Buried vias have a value of "O"
2) Test column
   Placed probe on primary access side?;  value = "F";
   Else if access is on primary access side;  value = "Y"
   Else value = "N"
*/

//_____________________________________________________________________________
void SeicaParNod_WriteFile(const CString& filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   // Open log file.
   CString localLogFilename;
   FILE *logFp = getApp().OpenOperationLogFile("Seica.log", localLogFilename);
   if (logFp == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(logFp, "Seica");

   CCamCadDatabase camCadDatabase(doc);
   CSeicaSettings seicaSettings;

   // par
   CFilePath parFilePath(filename);
   parFilePath.setExtension("par");

   CSeicaParWriter seicaParWriter(camCadDatabase, seicaSettings);
   seicaParWriter.write(parFilePath.getPath());

   // nod
   CFilePath nodFilePath(filename);
   nodFilePath.setExtension("nod");

   CSeicaNodWriter seicaNodWriter(camCadDatabase, seicaSettings);
   seicaNodWriter.write(nodFilePath.getPath());

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(logFp);
   }
}

//_____________________________________________________________________________
CString getSeicaUnits(ValueUnitTag valueUnit)
{
   CString seicaUnits;

   switch (valueUnit)
   {
   case valueUnitOhm:    seicaUnits = "O";  break;
   case valueUnitFarad:  seicaUnits = "F";  break;
   case valueUnitHenry:  seicaUnits = "H";  break;
   case valueUnitVolt:   seicaUnits = "V";  break;
   }

   return seicaUnits;
}

CString getSeicaMacro(DeviceTypeTag deviceType)
{
   CString macro;

   switch (deviceType)
   {
   case deviceTypeBattery:
   case deviceTypeCapacitorArray:
   case deviceTypeCrystal:
   case deviceTypeDiodeArray:
   case deviceTypeDiodeLedArray:
   case deviceTypeFilter:
   case deviceTypeIC:
   case deviceTypeICDigital:
   case deviceTypeICLinear:
   case deviceTypeOscillator:
   case deviceTypePotentiometer:
   case deviceTypePowerSupply:
   case deviceTypeResistorArray:
   case deviceTypeSpeaker:
   case deviceTypeSwitch:
   case deviceTypeTransistorArray:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
   case deviceTypeVoltageRegulator:
      macro = "$ICS";
      break;
   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      macro = "$CAPACITORS";
      break;
   case deviceTypeConnector:
      macro = "$CONNECTORS";
      break;
   case deviceTypeDiode:
   case deviceTypeDiodeLed:
      macro = "$DIODES";
      break;
   case deviceTypeDiodeZener:
      macro = "$ZENERS";
      break;
   case deviceTypeFuse:
   case deviceTypeJumper:
      macro = "$JUMPERS";
      break;
   case deviceTypeInductor:
      macro = "$INDUCTORS";
      break;
   case deviceTypeNoTest:
   case deviceTypeTestPoint:
      macro = "IGNORE";
      break;
   case deviceTypeRelay:
      macro = "$RELAYS";
      break;
   case deviceTypeResistor:
      macro = "$RESISTORS";
      break;
   case deviceTypeTransformer:
      macro = "$TRANSFORMERS";
      break;
   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      macro = "$TRANSISTORS";
      break;
   }

   return macro;
}

CString getSeicaDevice(DeviceTypeTag deviceType,const CString& subclass,const CString& partNumber)
{
   CString subclassOrDollarDevice;
   CString subclassOrPartNumberOrDollarDevice;
   CString partNumberOrDollarDevice;

   if (!partNumber.IsEmpty())
   {
      partNumberOrDollarDevice = partNumber;
   }
   else
   {
      partNumberOrDollarDevice = "$$DEVICE";
   }

   if (!subclass.IsEmpty())
   {
      subclassOrDollarDevice = subclass;
      subclassOrPartNumberOrDollarDevice = subclass;
   }
   else
   {
      subclassOrDollarDevice = "$$DEVICE";
      subclassOrPartNumberOrDollarDevice = partNumberOrDollarDevice;
   }

   CString device;

   switch (deviceType)
   {
   case deviceTypeBattery:
   case deviceTypeCapacitorArray:
   case deviceTypeCrystal:
   case deviceTypeDiodeArray:
   case deviceTypeDiodeLedArray:
   case deviceTypeFilter:
   case deviceTypeIC:
      device = subclassOrDollarDevice;
      break;
   case deviceTypeICDigital:
   case deviceTypeICLinear:
   case deviceTypeOscillator:
   case deviceTypePotentiometer:
   case deviceTypePowerSupply:
   case deviceTypeResistorArray:
   case deviceTypeSpeaker:
   case deviceTypeSwitch:
   case deviceTypeTransistorArray:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
   case deviceTypeVoltageRegulator:
      device = subclassOrPartNumberOrDollarDevice;
      break;
   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
   case deviceTypeConnector:
   case deviceTypeDiode:
   case deviceTypeDiodeLed:
   case deviceTypeDiodeZener:
   case deviceTypeFuse:
   case deviceTypeJumper:
      device.Empty();
      break;
   case deviceTypeInductor:
   case deviceTypeRelay:
   case deviceTypeResistor:
   case deviceTypeTransformer:
   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      device = partNumberOrDollarDevice;
      break;
   case deviceTypeNoTest:
   case deviceTypeTestPoint:
      device = "IGNORE";
      break;
   }

   return device;
}

//-----------------------------------------------------------------------------
// CSeicaSettings
//-----------------------------------------------------------------------------
CSeicaSettings::CSeicaSettings()
{
   reset();
   loadSettingFile();
}

CSeicaSettings::~CSeicaSettings()
{
}

void CSeicaSettings::reset()
{
   m_deviceTypeKeyword.Empty();
}

void CSeicaSettings::loadSettingFile()
{
   CString settingsFile( getApp().getExportSettingsFilePath("Seica.out") );
   {
      CString msg;
      msg.Format("Seica: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }

   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(settingsFile,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      ErrorMessage(tmp, "Seica Settings", MB_OK | MB_ICONHAND);
	}
	else
	{
		while (fgets(line,255,fp))
		{
			if ((lp = strtok(line," \t\n")) == NULL)
				continue;

			if (lp[0] == '.')
			{
            if (STRCMPI(lp,".DEV_TYPE") == 0)
				{
					if ((lp = strtok(NULL," \"\t\n")) != NULL)
						m_deviceTypeKeyword = lp;
				}
			}
		}

		fclose(fp);
	}
}

//_____________________________________________________________________________
CSeicaParWriter::CSeicaParWriter(CCamCadDatabase& camCadDatabase, CSeicaSettings& seicaSettings)
   : m_camCadDatabase(camCadDatabase)
   , m_seicaSettings(seicaSettings)
{
}

bool CSeicaParWriter::write(const CString& outputFilePath)
{
   bool retval = false;

   CStdioFileWriteFormat writeFormat;
   const CString formatString = "%-14s %-16s %-8s %-4s %-4s %-4s %-5s %-20s\n";

   if (writeFormat.open(outputFilePath))
   {
      writeFormat.writef(formatString,
         "*COMPONENT","DEVICE","VALUE","UNIT","TOL+","TOL-","NRPIN","MACRO");

      FileStruct* pcbFile = m_camCadDatabase.getSingleVisiblePcb();

      if (pcbFile != NULL)
      {
         retval = true;

         int valueKeywordIndex      = m_camCadDatabase.getKeywordIndex(ATT_VALUE);
         int deviceTypeKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_DEVICETYPE);
         int pToleranceKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_PLUSTOLERANCE);
         int nToleranceKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_MINUSTOLERANCE);
         int subclassKeywordIndex   = m_camCadDatabase.getKeywordIndex(ATT_SUBCLASS);
         int partNumberKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_PARTNUMBER);

         CString value;

         BlockStruct* pcbBlock = pcbFile->getBlock();

         for (CDataListIterator dataListIterator(*pcbBlock,insertTypePcbComponent);dataListIterator.hasNext();)
         {
            DataStruct* data = dataListIterator.getNext();
            InsertStruct* insert = data->getInsert();

            CString subclass,partNumber;
            CString refDes,device,value,unit,tolPlus,tolMinus,nrpin,macro;

            refDes = insert->getRefname();

            m_camCadDatabase.getAttributeStringValue(tolPlus   ,&(data->getAttributesRef()),pToleranceKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(tolMinus  ,&(data->getAttributesRef()),nToleranceKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(subclass  ,&(data->getAttributesRef()),subclassKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(partNumber,&(data->getAttributesRef()),partNumberKeywordIndex);

            if (m_camCadDatabase.getAttributeStringValue(value,&(data->getAttributesRef()),valueKeywordIndex))
            {
               ComponentValues compValue(value);

					double valueDbl = compValue.getScientificValue();
					int exponent = 0;
					double realNum = scientificNormalization(valueDbl,exponent);

					if (exponent < 0)
					{
						value.Format("%.15f", valueDbl);
						value.TrimRight("0");
						if (value.Right(1) == '.')
							value.Delete(value.GetLength() -1, 1);
					}
					else
					{
						value.Format("%d", round(compValue.getScientificValue()));
					}

               unit = getSeicaUnits(compValue.GetUnits());
            }

            if (m_camCadDatabase.getAttributeStringValue(device,&(data->getAttributesRef()),deviceTypeKeywordIndex))
            {
               DeviceTypeTag deviceType = stringToDeviceTypeTag(device);
               device = getSeicaDevice(deviceType,subclass,partNumber);
               macro  = getSeicaMacro(deviceType);
            }

            BlockStruct* componentGeometry = m_camCadDatabase.getBlock(insert->getBlockNumber());

            int pinCount          = componentGeometry->getPinCount();
            int noConnectPinCount = componentGeometry->getNoConnectPinCount();
            int connectedPinCount = pinCount -noConnectPinCount;

            nrpin.Format("%d",connectedPinCount);

            if (device.IsEmpty())
            {
               device = "0";
               if (!m_seicaSettings.getDeviceTypeKeyword().IsEmpty())
               {
                  int userDefineDeviceTypeKeywordIndex =  m_camCadDatabase.getKeywordIndex(m_seicaSettings.getDeviceTypeKeyword());
                  if (!m_camCadDatabase.getAttributeStringValue(device,&(data->getAttributesRef()),userDefineDeviceTypeKeywordIndex))
                     device = "0";
               }
            }

            if (value.IsEmpty()   ) value    = "0";
            if (unit.IsEmpty()    ) unit     = "0";
            if (tolPlus.IsEmpty() ) tolPlus  = "0";
            if (tolMinus.IsEmpty()) tolMinus = "0";
            if (macro.IsEmpty()   ) macro    = "0";

            writeFormat.writef(formatString,
               "\"" + refDes + "\"",device,value,unit,fpfmt(atof(tolPlus),4),fpfmt(atof(tolMinus),4),nrpin,macro);
         }
      }
   }

   return retval;
}

//_____________________________________________________________________________
CTestEntityMap::CTestEntityMap(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase),
   m_entityToLinkedAccessPointMap(1000,false),
   m_entityToLinkedTestProbeMap(1000,false)
{
   m_initializedFlag = false;
	m_hasProbesOnSideFlag = 0; 
	init();
}

/*
   Test Access Points - insert DataStructs with an insert type of insertTypeTestAccessPoint.
   They are associated with component pins (CompPinStructs), or with vias or test points (DataStructs).
   The association is defined by a data link attribute, ATT_DDLINK, whose value
   is the entity number of a component pin, via, or testpoint.

   Test Probes - insert DataStructs with an insert type of insertTypeTestProbe.
   If they are placed, then they will have a data link attribute, ATT_DDLINK, whose value
   is the entity number of a test access point.
*/

void CTestEntityMap::init()
{
   if (!m_initializedFlag)
   {
      m_initializedFlag = true;

      FileStruct* pcbFile = m_camCadDatabase.getSingleVisiblePcb();

      if (pcbFile != NULL)
      {
         BlockStruct* pcbBlock = pcbFile->getBlock();

         m_entityToLinkedAccessPointMap.InitHashTable(nextPrime2n(5000));
         m_entityToLinkedTestProbeMap.InitHashTable(nextPrime2n(5000));

         int dataLinkKeywordIndex= m_camCadDatabase.getKeywordIndex(ATT_DDLINK);

         // scan for probes and access markers
         for (CDataListIterator testIterator(*pcbBlock,dataTypeInsert);testIterator.hasNext();)
         {
            DataStruct* testData = testIterator.getNext();

            if (testData->getInsert()->getInsertType() == insertTypeTestAccessPoint ||
                testData->getInsert()->getInsertType() == insertTypeTestProbe          )
            {
               Attrib* dataLinkAttribute;

               if (testData->getAttributes()->Lookup(dataLinkKeywordIndex,dataLinkAttribute))
               {
                  int linkedEntityNumber = dataLinkAttribute->getIntValue();

                  if (testData->getInsert()->getInsertType() == insertTypeTestAccessPoint)
                  {
                     m_entityToLinkedAccessPointMap.SetAt(linkedEntityNumber,testData);
                  }
                  else if (testData->getInsert()->getInsertType() == insertTypeTestProbe)
                  {
							if (testData->getInsert()->getGraphicMirrored())
								m_hasProbesOnSideFlag |= 2;
							else
								m_hasProbesOnSideFlag |= 1;
                     m_entityToLinkedTestProbeMap.SetAt(linkedEntityNumber,testData);
                  }
               }
            }
         }
      }
   }
}

bool CTestEntityMap::hasProbesOnTop()
{
	// 1 = top
	// 2 = bottom
	return (m_hasProbesOnSideFlag&1)?true:false;
}

bool CTestEntityMap::hasProbesOnBottom()
{
	// 1 = top
	// 2 = bottom
	return (m_hasProbesOnSideFlag&2)?true:false;
}

DataStruct* CTestEntityMap::getAccessPoint(int entityNumber)
{
   DataStruct* accessPoint = NULL;

   if (!m_entityToLinkedAccessPointMap.Lookup(entityNumber,accessPoint))
   {
      accessPoint = NULL;
   }

   return accessPoint;
}

DataStruct* CTestEntityMap::getTestProbe(int entityNumber)
{
   DataStruct* testProbe = NULL;
   DataStruct* accessPoint = getAccessPoint(entityNumber);

   if (accessPoint != NULL)
   {
      if (!m_entityToLinkedTestProbeMap.Lookup(accessPoint->getEntityNumber(),testProbe))
      {
         testProbe = NULL;
      }
   }

   return testProbe;
}

//_____________________________________________________________________________
CSeicaNode::CSeicaNode(const CString& signal,const CString& refDes,const CString& pin,
	CPoint2d origin, CPoint2d testorigin,
   const CString& pad,const CString& position,const CString& technology,
   const CString& testability,const CString& channel) :
      m_signal(signal),
      m_refDes(refDes),
      m_pin(pin),
      m_origin(origin),
		m_testorigin(testorigin),
      m_pad(pad),
      m_position(position),
      m_technology(technology),
      m_testability(testability),
      m_channel(channel)
{
   m_sortablePinRef = CompPinStruct::getSortableReference(m_refDes) + "." + 
                      CompPinStruct::getSortableReference(m_pin);
}

//_____________________________________________________________________________
CSeicaNodes::CSeicaNodes(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase),
   m_testEntityMap(camCadDatabase)
{
   setSortFunction(sortBySignalAndPinRef);
}

int CSeicaNodes::sortBySignalAndPinRef(const void* element0,const void* element1)
{
   const CSeicaNode* node0 = (CSeicaNode*)(((SElement*)element0)->pObject->m_object);
   const CSeicaNode* node1 = (CSeicaNode*)(((SElement*)element1)->pObject->m_object);

   int retval = node0->getSignal().Compare(node1->getSignal());

   if (retval == 0)
   {
      retval = node0->getSortablePinRef().Compare(node1->getSortablePinRef());
   }

   return retval;
}

void CSeicaNodes::scanData()
{
   FileStruct* pcbFile = m_camCadDatabase.getSingleVisiblePcb();

   if (pcbFile != NULL)
   {
      BlockStruct* pcbBlock = pcbFile->getBlock();
      CString refDesValue,pinValue,netNameValue,padValue,positionValue,technologyValue,testabilityValue,channelValue;
      CPoint2d nodeOrigin;
      int deviceTypeKeywordIndex            = m_camCadDatabase.getKeywordIndex(ATT_DEVICETYPE);
      int netNameKeywordIndex               = m_camCadDatabase.getKeywordIndex(ATT_NETNAME);
      int technologyKeywordIndex            = m_camCadDatabase.getKeywordIndex(ATT_TECHNOLOGY);
      int deviceToPackagePinMapKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_DEVICETOPACKAGEPINMAP);
      int blindKeywordIndex                 = m_camCadDatabase.getKeywordIndex(ATT_BLINDSHAPE);   
      int dataLinkKeywordIndex				  = m_camCadDatabase.getKeywordIndex(ATT_DDLINK);


      for (CDataListIterator probeIterator(*pcbBlock,insertTypeTestProbe);probeIterator.hasNext();)
		{
			DataStruct* probe = probeIterator.getNext();
			if (probe->getAttributes() == NULL)
				continue;

			Attrib* attrib = NULL;
         if (!probe->getAttributes()->Lookup(dataLinkKeywordIndex, attrib))
			{
            CString netName;
            m_camCadDatabase.getAttributeStringValue(netName,&(probe->getAttributesRef()),netNameKeywordIndex);

				m_unplacedProbesChannelNumber.SetAt(netName, probe->getInsert()->getRefname());
			}
		}

      // Determine primary access side by examining which sides have probes
      bool primaryAccessSideTopFlag = m_testEntityMap.hasProbesOnTop();
      if (m_testEntityMap.hasProbesOnTop() && m_testEntityMap.hasProbesOnBottom())
		{
         int status = ErrorMessage("Probes found on both top and bottom sides.  Is the top side the primary access side?", "", MB_ICONQUESTION | MB_YESNO);
         if (status != IDYES) primaryAccessSideTopFlag = false;
		}
      else if (!m_testEntityMap.hasProbesOnTop() && !m_testEntityMap.hasProbesOnBottom())
      {
         int status = ErrorMessage("No probes found on either top or bottom sides.  Is the top side the primary access side?", "", MB_ICONQUESTION | MB_YESNO);
         if (status == IDYES) primaryAccessSideTopFlag = true;
      }

      // scan for component pins and test points
      for (CDataListIterator pcbDataListIterator(*pcbBlock,dataTypeInsert);pcbDataListIterator.hasNext();)
      {
         DataStruct* componentData = pcbDataListIterator.getNext();
         InsertStruct* componentInsert = componentData->getInsert();

         bool componentFlag = (componentInsert->getInsertType() == insertTypePcbComponent);
         bool testPointFlag = (componentInsert->getInsertType() == insertTypeTestPoint   );

         if (componentFlag || testPointFlag)
         {
            CTMatrix matrix(componentInsert->getTMatrix());
            BlockStruct* componentGeometry = m_camCadDatabase.getBlock(componentInsert->getBlockNumber());
            CString device;
            DeviceTypeTag deviceType = deviceTypeUndefined;
            refDesValue = componentInsert->getRefname();

            if (m_camCadDatabase.getAttributeStringValue(device,&(componentData->getAttributesRef()),deviceTypeKeywordIndex))
            {
               deviceType = stringToDeviceTypeTag(device);
            }

            CString componentTechnology;
            m_camCadDatabase.getAttributeStringValue(componentTechnology,&(componentData->getAttributesRef()),technologyKeywordIndex);

            CMapStringToString seicaPinMap;
            getSeicaPinMapping(pcbFile,seicaPinMap,*componentData,deviceType);

            for (CDataListIterator componentGeometryDataListIterator(*componentGeometry,dataTypeInsert);
                  componentGeometryDataListIterator.hasNext();)
            {
               DataStruct* padstackData = componentGeometryDataListIterator.getNext();
               InsertStruct* padstackInsert = padstackData->getInsert();
               BlockStruct* padstackGeometry = m_camCadDatabase.getBlock(padstackInsert->getBlockNumber());

               if (padstackGeometry->getBlockType() == blockTypePadstack)
               {
                  CString pinName = padstackInsert->getRefname();
                  CCamCadPin* camCadPin = m_camCadDatabase.getCamCadPin(pcbFile,refDesValue,pinName);

                  if (camCadPin != NULL)
                  {
                     CString deviceToPackagePinName;
                     pinValue = camCadPin->getCompPin()->getPinName();

							seicaPinMap.Lookup(pinName,pinValue);

                     CompPinStruct* compPin = camCadPin->getCompPin();
                     NetStruct* net = camCadPin->getNet();
                     netNameValue = net->getNetName();

                     CBasesVector padstackBasesVector = padstackInsert->getBasesVector();
                     padstackBasesVector.transform(matrix);
                     nodeOrigin = padstackBasesVector.getOrigin();

                     DataStruct* surfacePad = NULL;
                     bool topPadFlag = true;

                     if (!getCopperPad(m_camCadDatabase,surfacePad,*componentData,*padstackGeometry,topPadFlag, padstackData->getLayerIndex()))
                     {
                        topPadFlag = false;
                        getCopperPad(m_camCadDatabase,surfacePad,*componentData,*padstackGeometry,topPadFlag, padstackData->getLayerIndex());
                     }

                     padValue = getSurfacePadDescriptor(surfacePad,*padstackGeometry);

                     CString padstackTechnology;
                     m_camCadDatabase.getAttributeStringValue(padstackTechnology,&(padstackData->getAttributesRef()),technologyKeywordIndex);

                     if (padstackTechnology.IsEmpty())
                     {
                        padstackTechnology = componentTechnology;
                     }

                     bool smdPad = (padstackTechnology.CompareNoCase("SMD" ) == 0);
                     bool thPad  = (padstackTechnology.CompareNoCase("THRU") == 0);

                     if (thPad)
                     {
                        technologyValue = "T";

                        int entityNumber = compPin->getEntityNumber();
                        DataStruct* probe = m_testEntityMap.getTestProbe(entityNumber);

                        if (probe != NULL)
                        {
                           positionValue = (primaryAccessSideTopFlag ? "T" : "B") ;
                        }
                     }
                     else if (smdPad)
                     {
                        technologyValue = "S";

                        positionValue = (topPadFlag ? "T" : "B") ;
                     }
                     else
                     {
                        technologyValue = "?";
                     }

							CPoint2d testOrigin = nodeOrigin;
                     getTestValues(testabilityValue,channelValue,testOrigin,compPin->getEntityNumber(),primaryAccessSideTopFlag);
                     
							CSeicaNode* pinNode = new CSeicaNode(netNameValue,refDesValue,pinValue,
                                                nodeOrigin, testOrigin, padValue,positionValue,technologyValue,
                                                testabilityValue,channelValue);

                     SetAt(pinNode->getPinRef(),pinNode);
                  }
               }
            }
         }
      }

      // scan for vias
      CTypedMapStringToPtrContainer<DataStruct*> viaMap(10,false);
      viaMap.InitHashTable(nextPrime2n(5000));
      DataStruct* existingVia;
      CDataList invalidVias(false);
      technologyValue = "T";
      pinValue = "1";
      CInsertTypeMask insertMask(insertTypeVia,insertTypeBondPad);

      for (CDataListIterator viaIterator(*pcbBlock,insertMask);viaIterator.hasNext();)
      {
         DataStruct* via = viaIterator.getNext();
         InsertStruct* viaInsert = via->getInsert();
         refDesValue = viaInsert->getRefname();
         refDesValue.Trim();

         if (refDesValue.IsEmpty())
         {
            invalidVias.AddTail(via);
         }
         else if (viaMap.Lookup(refDesValue,existingVia))
         {
            invalidVias.AddTail(via);
         }
         else
         {
            viaMap.SetAt(refDesValue,via);
         }
      }

      // name unnamed vias
      int viaId = 1;

      for (POSITION viaPos = invalidVias.GetHeadPosition();viaPos != NULL;)
      {
         DataStruct* via = invalidVias.GetNext(viaPos);

         while (true)
         {
            refDesValue.Format("via_%d",viaId++);

            if (!viaMap.Lookup(refDesValue,existingVia))
            {
               break;
            }
         } 

         via->getInsert()->setRefname(refDesValue);
         viaMap.SetAt(refDesValue,via);
      }

      DataStruct* via;

      for (POSITION viaPos = viaMap.GetStartPosition();viaPos != NULL;)
      {
         viaMap.GetNextAssoc(viaPos,refDesValue,via);
         InsertStruct* viaInsert = via->getInsert();
         BlockStruct* padstackGeometry = m_camCadDatabase.getBlock(viaInsert->getBlockNumber());
         
         //get padstack geometry in bondpad
         if(viaInsert->getInsertType() == insertTypeBondPad)
            padstackGeometry = GetBondPadPadstackBlock(m_camCadDatabase.getCamCadData(), padstackGeometry);

         if (viaInsert->getRefname().CompareNoCase("via_1024") == 0)
         {
            int iii = 3;
         }

         m_camCadDatabase.getAttributeStringValue(netNameValue,&(via->getAttributesRef()),netNameKeywordIndex);

         Attrib* attrib;
         DataStruct* surfacePad = NULL;
         bool blindFlag  = (via->getAttributesRef()->Lookup(blindKeywordIndex,attrib) != 0);
         bool topFlag    = getCopperPad(m_camCadDatabase,surfacePad,*padstackGeometry,true,true, via->getLayerIndex());
         bool bottomFlag = getCopperPad(m_camCadDatabase,surfacePad,*padstackGeometry,true,false, via->getLayerIndex());
         bool buriedFlag = blindFlag && (!topFlag && !bottomFlag);

         if (buriedFlag)
         {
            positionValue = "O";
         }
         else if (blindFlag)
         {
            positionValue = (topFlag ? "T" : "B");
         }
         else
         {
            positionValue = (primaryAccessSideTopFlag ? "T" : "B") ;
         }

         nodeOrigin = viaInsert->getOrigin2d();
			CPoint2d testOrigin = nodeOrigin;
         getTestValues(testabilityValue,channelValue,testOrigin,via->getEntityNumber(),primaryAccessSideTopFlag);

         padValue = getSurfacePadDescriptor(surfacePad,*padstackGeometry);

         CSeicaNode* viaNode = new CSeicaNode(netNameValue,refDesValue,pinValue,
                                       nodeOrigin, testOrigin, padValue,positionValue,technologyValue,
                                       testabilityValue,channelValue);

         SetAt(viaNode->getPinRef(),viaNode);
      }
   }
}

CString CSeicaNodes::getUnplacedProbe(CString netname)
{
	CString channelNumber;
	if (m_unplacedProbesChannelNumber.Lookup(netname, channelNumber))
		return channelNumber;
	else
		return "0";
}

void CSeicaNodes::getTestValues(CString& testabilityValue,CString& channelValue,CPoint2d& testOrigin,int entityNumber,bool primaryAccesSideTopFlag)
{
   DataStruct* probe = m_testEntityMap.getTestProbe(entityNumber);
   bool probePlacedOnPrimaryAccessSideFlag = false;
   channelValue = "0";

   if (probe != NULL)
   {
      channelValue = probe->getInsert()->getRefname();
		testOrigin.x = probe->getInsert()->getOrigin2d().x;
		testOrigin.y = probe->getInsert()->getOrigin2d().y;

      probePlacedOnPrimaryAccessSideFlag = (probe->getInsert()->getGraphicMirrored() != primaryAccesSideTopFlag);
   }

   if (probePlacedOnPrimaryAccessSideFlag)
   {
      testabilityValue = "F";
   }
   else
   {
      DataStruct* accessPoint = m_testEntityMap.getAccessPoint(entityNumber);

      testabilityValue = ((accessPoint == NULL) ? "N" : "Y");

		// Want access pt origin only if there was not probe
		if (probe == NULL && accessPoint != NULL)
		{
			testOrigin.x = accessPoint->getInsert()->getOrigin2d().x;
			testOrigin.y = accessPoint->getInsert()->getOrigin2d().y;
		}

   }
}

CString CSeicaNodes::getSurfacePadDescriptor(DataStruct* surfacePad,BlockStruct& padstackGeometry)
{
   CString padDescriptor;
   CUnits units(m_camCadDatabase.getPageUnits());

   if (surfacePad != NULL)
   {
      BlockStruct* apertureBlock = m_camCadDatabase.getBlock(surfacePad->getInsert()->getBlockNumber());
      CStandardAperture standardAperture(apertureBlock,m_camCadDatabase.getPageUnits());
      BlockStruct* drillGeometry = GetDrillInGeometry(&(m_camCadDatabase.getCamCadDoc()),&padstackGeometry);

      double toolSize = ((drillGeometry != NULL) ? drillGeometry->getToolSize() : 0.);

      if (standardAperture.isStandardAperture())
      {
         switch (standardAperture.getType())
         {
         case standardApertureCircle:     
         case standardApertureRoundDonut: 
            padDescriptor.Format("R%dH%d",
               round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))),
               round(units.convertTo(pageUnitsMils,toolSize)));
            break;
         case standardApertureSquare:     
            padDescriptor.Format("X%dY%d",round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))),
													round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))));
            break;
         case standardApertureRectangle:  
         case standardApertureOval:       
            padDescriptor.Format("X%dY%d",round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))),
                                       round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(1))) );
            break;
         case standardApertureHole:       
         case standardApertureNull:
            break;
         }                              
      }

      if (padDescriptor.IsEmpty())
      {
         CExtent padExtent = standardAperture.getExtent();
         padDescriptor.Format("X%dY%d",round(units.convertTo(pageUnitsMils,padExtent.getXsize())),
                                    round(units.convertTo(pageUnitsMils,padExtent.getYsize())) );
      }

      delete surfacePad;
   }

   return padDescriptor;
}

bool CSeicaNodes::getCopperPad(CCamCadDatabase& camCadDatabase,
   DataStruct*& copperPad,BlockStruct& padstackGeometry,bool placedTopFlag,bool topFlag, int insertlayer) const
{
   //  topFlag   placedTopFlag   surfaceType
   //   f                f          top
   //   f                t          bottom
   //   t                f          bottom
   //   t                t          top

   bool topSurfaceTypeFlag = (topFlag == placedTopFlag);

   CCompositePad surfacePad(camCadDatabase);

   for (CDataListIterator dataListIterator(padstackGeometry,dataTypeInsert);dataListIterator.hasNext();)
   {
      DataStruct* pad = dataListIterator.getNext();
      InsertStruct* padInsert = pad->getInsert();
      BlockStruct* padGeometry = camCadDatabase.getBlock(padInsert->getBlockNumber());

      if (! padGeometry->isAperture())
      {
         continue;
      }

      LayerStruct* layer = camCadDatabase.getLayerAt(pad->getLayerIndex());
      if(layer && layer->isFloating() && insertlayer != -1)
      {
         layer = camCadDatabase.getLayerAt(insertlayer);
      }

      if (! placedTopFlag)
      {
         layer = camCadDatabase.getLayerAt(layer->getMirroredLayerIndex());
      }

      bool notInstantiatedFlag = ( 
         ((layer->getLayerFlags() & LY_TOPONLY    ) != 0 && !placedTopFlag) ||
         ((layer->getLayerFlags() & LY_BOTTOMONLY ) != 0 &&  placedTopFlag) ||
         (layer->getNeverMirror() && !placedTopFlag) ||
         (layer->getMirrorOnly()  &&  placedTopFlag)     );

      if (notInstantiatedFlag)
      {
         continue;
      }

      bool instantiatedOnSurfaceLayerFlag = false;

      switch (layer->getLayerType())
      {
      case LAYTYPE_SIGNAL_TOP:
      case LAYTYPE_PAD_TOP:
      case LAYTYPE_TOP:
         instantiatedOnSurfaceLayerFlag =  topFlag;
         break;

      case LAYTYPE_SIGNAL_BOT:
      case LAYTYPE_PAD_BOTTOM:
      case LAYTYPE_BOTTOM:
         instantiatedOnSurfaceLayerFlag = !topFlag;
         break;

      case LAYTYPE_SIGNAL_ALL:
      case LAYTYPE_PAD_ALL:
      case LAYTYPE_ALL:
      case LAYTYPE_PAD_OUTER:
      case LAYTYPE_SIGNAL_OUTER:
         instantiatedOnSurfaceLayerFlag = true;
         break;
      }

      if (!instantiatedOnSurfaceLayerFlag)
      {
         continue;
      }

      surfacePad.addPad(camCadDatabase.getCamCadData().getNewDataStruct(*pad));
   }

   if (surfacePad.getCount() > 0)
   {
      copperPad = camCadDatabase.getCamCadData().getNewDataStruct(*(surfacePad.getPad()));
   }

   return (copperPad != NULL);
}

bool CSeicaNodes::getCopperPad(CCamCadDatabase& camCadDatabase,
   DataStruct*& copperPad,DataStruct& component,BlockStruct& padstackGeometry,bool topFlag, int insertlayer) const
{
   bool placedTopFlag = component.getInsert()->getPlacedTop();

   return getCopperPad(camCadDatabase,copperPad,padstackGeometry,placedTopFlag,topFlag, insertlayer);
}

void CSeicaNodes::getSeicaPinMapping(FileStruct* pcbFile,CMapStringToString& seicaPinMap,DataStruct& component,DeviceTypeTag deviceType)
{
   int deviceToPackagePinMapKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_DEVICETOPACKAGEPINMAP);

   BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component.getInsert()->getBlockNumber());
	CString refDesValue = component.getInsert()->getRefname();
   int pinIndex = 1;

   for (CDataListIterator dataListIterator(*componentGeometry,insertTypePin);dataListIterator.hasNext();pinIndex++)
   {
      DataStruct* pinData = dataListIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();

		CCamCadPin* camCadPin = m_camCadDatabase.getCamCadPin(pcbFile,refDesValue,pinName);
		if (camCadPin == NULL)
			continue;

		CompPinStruct* compPin = camCadPin->getCompPin();
      CString deviceToPackagePinName;
		m_camCadDatabase.getAttributeStringValue(deviceToPackagePinName, &compPin->getAttributesRef(),deviceToPackagePinMapKeywordIndex);

		int pinNumber = -1;
		switch(deviceType)
      {
      case deviceTypeCapacitorTantalum:
      case deviceTypeCapacitorPolarized:
         if      (deviceToPackagePinName.CompareNoCase("Positive") == 0) pinNumber = 1;
         else if (deviceToPackagePinName.CompareNoCase("Negative") == 0) pinNumber = 2;
         break;

      case deviceTypeDiode:
      case deviceTypeDiodeLed:
      case deviceTypeDiodeZener:
         if      (deviceToPackagePinName.CompareNoCase("Anode"  ) == 0) pinNumber = 1;
         else if (deviceToPackagePinName.CompareNoCase("Cathode") == 0) pinNumber = 2;
         break;

      case deviceTypeTransistor:
      case deviceTypeTransistorNpn:
      case deviceTypeTransistorPnp:
         if      (deviceToPackagePinName.CompareNoCase("Emitter"  ) == 0) pinNumber = 1;
         else if (deviceToPackagePinName.CompareNoCase("Base"     ) == 0) pinNumber = 2;
         else if (deviceToPackagePinName.CompareNoCase("Collector") == 0) pinNumber = 3;
         break;

		default:
         break;
      }

		if (pinNumber == -1)
      {
         char* endChar;
         pinNumber = strtol(pinName,&endChar,10);

         if (*endChar != '\0')
         {
            pinNumber = pinIndex + 1000;
         }
      }

		CString pinValue;
		pinValue.Format("%d", pinNumber);

		seicaPinMap.SetAt(pinName, pinValue);
	}
}

void CSeicaNodes::getSeicaPinMapping(CMapStringToString& seicaPinMap,DataStruct& component,DeviceTypeTag deviceType)
{
   CStringArray seicaPinMapping;
   seicaPinMapping.SetSize(0,200);

   BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component.getInsert()->getBlockNumber());
   int pinIndex = 1;

   for (CDataListIterator dataListIterator(*componentGeometry,insertTypePin);dataListIterator.hasNext();pinIndex++)
   {
      DataStruct* pinData = dataListIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();
      CString seicaPinNumber;
      int pinNumber = 0;

      if (deviceType == deviceTypeUndefined)
      {
         seicaPinMapping.SetAtGrow(pinIndex,pinName);
      }
      else
      {
         switch(deviceType)
         {
         case deviceTypeCapacitorTantalum:
         case deviceTypeCapacitorPolarized:
            if      (pinName.CompareNoCase("Positive") == 0) pinNumber = 1;
            else if (pinName.CompareNoCase("Negative") == 0) pinNumber = 2;

            break;
         case deviceTypeDiode:
         case deviceTypeDiodeLed:
         case deviceTypeDiodeZener:
            if      (pinName.CompareNoCase("Anode"  ) == 0) pinNumber = 1;
            else if (pinName.CompareNoCase("Cathode") == 0) pinNumber = 2;

            break;
         case deviceTypeTransistor:
         case deviceTypeTransistorNpn:
         case deviceTypeTransistorPnp:
            if      (pinName.CompareNoCase("Emitter"  ) == 0) pinNumber = 1;
            else if (pinName.CompareNoCase("Base"     ) == 0) pinNumber = 2;
            else if (pinName.CompareNoCase("Collector") == 0) pinNumber = 3;

            break;
         case deviceTypeUnknown:
         case deviceTypeBattery:
         case deviceTypeCapacitor:
         case deviceTypeCapacitorArray:
         case deviceTypeConnector:
         case deviceTypeCrystal:
         case deviceTypeDiodeArray:
         case deviceTypeFilter:
         case deviceTypeFuse:
         case deviceTypeIC:
         case deviceTypeICDigital:
         case deviceTypeICLinear:
         case deviceTypeInductor:
         case deviceTypeJumper:
         case deviceTypeNoTest:
         case deviceTypeOscillator:
         case deviceTypePotentiometer:
         case deviceTypePowerSupply:
         case deviceTypeRelay:
         case deviceTypeResistor:
         case deviceTypeResistorArray:
         case deviceTypeSpeaker:
         case deviceTypeSwitch:
         case deviceTypeTestPoint:
         case deviceTypeTransformer:
         case deviceTypeTransistorArray:
         case deviceTypeTransistorFetNpn:
         case deviceTypeTransistorFetPnp:
         case deviceTypeTransistorMosfetNpn:
         case deviceTypeTransistorMosfetPnp:
         case deviceTypeTransistorScr:
         case deviceTypeTransistorTriac:
         case deviceTypeVoltageRegulator:
         case deviceTypeDiodeLedArray:
            {
               char* endChar;
               pinNumber = strtol(pinName,&endChar,10);

               if (*endChar != '\0')
               {
                  pinNumber = pinIndex + 1000;
               }
            }

            break;
         default:
         case deviceTypeUndefined:
            break;
         }

         if (pinNumber > 0)
         {
            seicaPinMapping.SetAtGrow(pinNumber,pinName);
         }
      }
   }

   for (int pinIndex = 1;pinIndex < seicaPinMapping.GetSize();pinIndex++)
   {
      CString pinName = seicaPinMapping.GetAt(pinIndex);

      if (!pinName.IsEmpty())
      {
         CString pinNumber;
         pinNumber.Format("%d",pinIndex);

         seicaPinMap.SetAt(pinName,pinNumber);
      }
   }
}

//_____________________________________________________________________________
CSeicaNodWriter::CSeicaNodWriter(CCamCadDatabase& camCadDatabase, CSeicaSettings& seicaSettings)
   : m_camCadDatabase(camCadDatabase)
   , m_seicaSettings(seicaSettings)
{
}

CSeicaNodWriter::~CSeicaNodWriter()
{
}

bool CSeicaNodWriter::write(const CString& outputFilePath)
{
   bool retval = true;

   CSeicaNodes seicaNodes(m_camCadDatabase);
   seicaNodes.scanData();
   CUnits units(m_camCadDatabase.getPageUnits());

   CStdioFileWriteFormat writeFormat;
   const CString formatString = "%-18s %-16s %-6s %-10s %-10s %-16s %-3s %-4s %-4s %-12s\n";

   if (writeFormat.open(outputFilePath))
   {
      writeFormat.writef(formatString,
         "*SIGNAL","COMP.","PIN","X","Y","PAD","POS","TECN","TEST","CHANNEL USER");

      CString oldSignal;
      CSeicaNode* seicaNode;
      CSeicaNode* maxChannelSeicaNode;
      CTypedPtrMap<CMapStringToPtr,CString,CSeicaNode*> m_channelMap;
      m_channelMap.InitHashTable(nextPrime2n(seicaNodes.GetCount()));

      for (seicaNodes.rewind();seicaNodes.next(seicaNode);)
      {
         if (m_channelMap.Lookup(seicaNode->getSignal(),maxChannelSeicaNode))
         {
            if (seicaNode->getChannelNumber() > maxChannelSeicaNode->getChannelNumber())
            {
               m_channelMap.SetAt(seicaNode->getSignal(),seicaNode);
            }
         }
         else
         {
            m_channelMap.SetAt(seicaNode->getSignal(),seicaNode);
         }
      }

      for (seicaNodes.rewind();seicaNodes.next(seicaNode);)
      {
         if (seicaNode->getSignal() != oldSignal)
         {
            oldSignal = seicaNode->getSignal();

            if (! m_channelMap.Lookup(seicaNode->getSignal(),maxChannelSeicaNode))
            {
               maxChannelSeicaNode = seicaNode;
            }

            writeFormat.write("\n");
         }

			CString channel;
			if (maxChannelSeicaNode->getChannel() == "0")
				channel = seicaNodes.getUnplacedProbe(seicaNode->getSignal());
			else
				channel = maxChannelSeicaNode->getChannel();

			// Check for test point shifted (offset) from pin
			double partX = seicaNode->getOrigin().x;
			double partY = seicaNode->getOrigin().y;
			double testX = seicaNode->getTestOrigin().x;
			double testY = seicaNode->getTestOrigin().y;

			bool shifted = !(fpnear(partX, testX, 0.005) && fpnear(partY, testY, 0.005));

			// Write basic part
         writeFormat.writef(formatString,
            "\"" + seicaNode->getSignal() + "\"",
            "\"" + seicaNode->getRefDes() + "\"",seicaNode->getPin(),
            fpfmt(units.convertTo(pageUnitsMilliMeters,partX),3),
            fpfmt(units.convertTo(pageUnitsMilliMeters,partY),3),
            seicaNode->getPad(),seicaNode->getPosition(),seicaNode->getTechnology(),
				!shifted ? seicaNode->getTestability() : "N", channel);

			// If test point is offset from part pin, write second record with shift applied
			if (shifted) 
			{
				writeFormat.writef(formatString,
					"\"" + seicaNode->getSignal() + "\"",
					"\"" + seicaNode->getRefDes() + "_SHIFT\"",seicaNode->getPin(),
					fpfmt(units.convertTo(pageUnitsMilliMeters, testX), 3),
					fpfmt(units.convertTo(pageUnitsMilliMeters, testY), 3),
					seicaNode->getPad(),seicaNode->getPosition(),seicaNode->getTechnology(),
					seicaNode->getTestability(),channel);
			}

      }
   }

   return retval;
}

bool CSeicaNodWriter::write2(const CString& outputFilePath)
{
   bool retval = false;

   CSeicaNodes seicaNodes(m_camCadDatabase);
   seicaNodes.scanData();

   CStdioFileWriteFormat writeFormat;
   const CString formatString = "%-14s %-16s %-4s %-10s %-10s %-16s %-3s %-4s %-4s %-12s\n";
   CUnits units(m_camCadDatabase.getPageUnits());

   if (writeFormat.open(outputFilePath))
   {
      writeFormat.writef(formatString,
         "*SIGNAL","COMP.","PIN","X","Y","PAD","POS","TECN","TEST","CHANNEL USER");

      FileStruct* pcbFile = m_camCadDatabase.getSingleVisiblePcb();

      if (pcbFile != NULL)
      {
         retval = true;

         int technologyKeywordIndex         = m_camCadDatabase.getKeywordIndex(ATT_TECHNOLOGY);
         int probePlacementTypeKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_PROBEPLACEMENT);
         int deviceTypeKeywordIndex         = m_camCadDatabase.getKeywordIndex(ATT_DEVICETYPE);

         initMaps();

         for (POSITION netPos = pcbFile->getHeadNetPosition();netPos != NULL;)
         {
            NetStruct* net = pcbFile->getNextNet(netPos);

            for (POSITION pinPos = net->getHeadCompPinPosition();pinPos != NULL;)
            {
               CompPinStruct* pin = net->getNextCompPin(pinPos);
               CString refDes = pin->getRefDes();
               CString pinRef = pin->getPinRef();
               DataStruct* component = getComponent(refDes);

               if (component == NULL)
               {
                  continue;
               }

               bool topFlag = component->getInsert()->getPlacedTop();

               CString pinColumn;

               if (m_pinNumberMap.Lookup(pinRef,pinColumn) && !pinColumn.IsEmpty())
               {
                  CString signalColumn(net->getNetName()),posColumn("?");
                  CString tecnColumn,technology,padColumn;

                  BlockStruct* padstackGeometry = NULL;
                  int padstackBlockNumber = pin->getPadstackBlockNumber();

                  if (padstackBlockNumber >= 0)
                  {
                     padstackGeometry = m_camCadDatabase.getBlock(padstackBlockNumber);

                     m_camCadDatabase.getAttributeStringValue(technology,&(pin->getAttributesRef()),technologyKeywordIndex);

                     DataStruct* surfacePad = NULL;

                     if (!getCopperPad(m_camCadDatabase,surfacePad,*component,*padstackGeometry,true))
                     {
                        getCopperPad(m_camCadDatabase,surfacePad,*component,*padstackGeometry,false);
                     }

                     if (surfacePad != NULL)
                     {
                        BlockStruct* apertureBlock = m_camCadDatabase.getBlock(surfacePad->getInsert()->getBlockNumber());
                        CStandardAperture standardAperture(apertureBlock,m_camCadDatabase.getPageUnits());

                        if (standardAperture.isStandardAperture())
                        {
                           switch (standardAperture.getApertureShape())
                           {
                           case standardApertureCircle:     
                           case standardApertureRoundDonut: 
                              padColumn.Format("R%d",round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))));
                              break;
                           case standardApertureSquare:     
                              padColumn.Format("X%d",round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))));
                              break;
                           case standardApertureRectangle:  
                           case standardApertureOval:       
                              padColumn.Format("X%dY%d",round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(0))),
                                                        round(units.convertTo(pageUnitsMils,standardAperture.getDimensionInPageUnits(1))) );
                              break;
                           case standardApertureHole:       
                           case standardApertureNull:
                              break;
                           }                              
                        }

                        if (padColumn.IsEmpty())
                        {
                           CExtent padExtent = standardAperture.getExtent();
                           padColumn.Format("X%dY%d",round(units.convertTo(pageUnitsMils,padExtent.getXsize())),
                                                     round(units.convertTo(pageUnitsMils,padExtent.getYsize())) );
                        }

                        delete surfacePad;
                     }

                     for (CDataListIterator toolIterator(*padstackGeometry,dataTypeInsert);toolIterator.hasNext();)
                     {
                        DataStruct* tool = toolIterator.getNext();
                        BlockStruct* toolBlock = m_camCadDatabase.getBlock(tool->getInsert()->getBlockNumber());

                        if (toolBlock->getBlockType() == blockTypeDrillHole)
                        {
                           CString drillDescriptor;
                           drillDescriptor.Format("H%d",round(units.convertTo(pageUnitsMils,toolBlock->getToolSize())));
                           padColumn += drillDescriptor;
                           break;
                        }
                     }
                  }

                  if (technology.IsEmpty())
                  {
                     m_camCadDatabase.getAttributeStringValue(technology,&(component->getAttributesRef()),technologyKeywordIndex);
                  }

                  if      (technology.CompareNoCase("SMD" ) == 0) tecnColumn = "S";
                  else if (technology.CompareNoCase("THRU") == 0) tecnColumn = "T";
                  else tecnColumn = "?";

                  if (padColumn.IsEmpty()) padColumn = "?";

                  writeFormat.writef(formatString,
                     signalColumn,refDes,pinColumn,
                     fpfmt(units.convertTo(pageUnitsMilliMeters,pin->getOrigin().x),3),
                     fpfmt(units.convertTo(pageUnitsMilliMeters,pin->getOrigin().y),3),
                     padColumn,(topFlag ? "T" : "B"),tecnColumn,"?","?");
               }
            }

            writeFormat.write("\n");
         }
      }
   }

   return retval;
}

DataStruct* CSeicaNodWriter::getComponent(const CString& refDes)
{
   DataStruct* component = NULL;

   if (!m_componentMap.Lookup(refDes,component))
   {
      component = NULL;
   }

   return component;
}

bool CSeicaNodWriter::getCopperPad(CCamCadDatabase& camCadDatabase,
   DataStruct*& copperPad,DataStruct& component,BlockStruct& padstackGeometry,bool topFlag) const
{
   bool componentTopFlag = component.getInsert()->getPlacedTop();

   //  topFlag   componentTopFlag   surfaceType
   //   f                f          top
   //   f                t          bottom
   //   t                f          bottom
   //   t                t          top

   bool topSurfaceTypeFlag = (topFlag == componentTopFlag);

   CCompositePad surfacePad(camCadDatabase);

   for (CDataListIterator dataListIterator(padstackGeometry,dataTypeInsert);dataListIterator.hasNext();)
   {
      DataStruct* pad = dataListIterator.getNext();
      InsertStruct* padInsert = pad->getInsert();
      BlockStruct* padGeometry = camCadDatabase.getBlock(padInsert->getBlockNumber());

      if (! padGeometry->isAperture())
      {
         continue;
      }

      LayerStruct* layer = camCadDatabase.getLayerAt(pad->getLayerIndex());

      if (! componentTopFlag)
      {
         layer = camCadDatabase.getLayerAt(layer->getMirroredLayerIndex());
      }

      bool notInstantiatedFlag = ( 
         ((layer->getLayerFlags() & LY_TOPONLY    ) != 0 && !componentTopFlag) ||
         ((layer->getLayerFlags() & LY_BOTTOMONLY ) != 0 &&  componentTopFlag) ||
         (layer->getNeverMirror() && !componentTopFlag) ||
         (layer->getMirrorOnly()  &&  componentTopFlag)     );

      if (notInstantiatedFlag)
      {
         continue;
      }

      bool instantiatedOnSurfaceLayerFlag = false;

      switch (layer->getLayerType())
      {
      case LAYTYPE_SIGNAL_TOP:
      case LAYTYPE_PAD_TOP:
      case LAYTYPE_TOP:
         instantiatedOnSurfaceLayerFlag =  topFlag;
         break;

      case LAYTYPE_SIGNAL_BOT:
      case LAYTYPE_PAD_BOTTOM:
      case LAYTYPE_BOTTOM:
         instantiatedOnSurfaceLayerFlag = !topFlag;
         break;

      case LAYTYPE_SIGNAL_ALL:
      case LAYTYPE_PAD_ALL:
      case LAYTYPE_ALL:
      case LAYTYPE_PAD_OUTER:
      case LAYTYPE_SIGNAL_OUTER:
         instantiatedOnSurfaceLayerFlag = true;
         break;
      }

      if (!instantiatedOnSurfaceLayerFlag)
      {
         continue;
      }

      surfacePad.addPad(camCadDatabase.getCamCadData().getNewDataStruct(*pad));
   }

   if (surfacePad.getCount() > 0)
   {
      copperPad = camCadDatabase.getCamCadData().getNewDataStruct(*(surfacePad.getPad()));
   }

   return (copperPad != NULL);
}

void CSeicaNodWriter::initMaps()
{
   m_componentMap.InitHashTable(nextPrime2n(1000));
   m_pinNumberMap.InitHashTable(nextPrime2n(10000));

   FileStruct* pcbFile = m_camCadDatabase.getSingleVisiblePcb();

   if (pcbFile != NULL)
   {
      BlockStruct* pcbBlock = pcbFile->getBlock();

      int deviceTypeKeywordIndex         = m_camCadDatabase.getKeywordIndex(ATT_DEVICETYPE);

      for (CDataListIterator pcbDataListIterator(*pcbBlock,dataTypeInsert);pcbDataListIterator.hasNext();)
      {
         DataStruct* componentData = pcbDataListIterator.getNext();
         InsertStruct* componentInsert = componentData->getInsert();

         if (componentInsert->getInsertType() == insertTypePcbComponent ||
             componentInsert->getInsertType() == insertTypeTestPoint       )
         {
            CString refDes = componentInsert->getRefname();
            CString device;
            DeviceTypeTag deviceType = deviceTypeUndefined;

            m_componentMap.SetAt(refDes,componentData);

            if (m_camCadDatabase.getAttributeStringValue(device,&(componentData->getAttributesRef()),deviceTypeKeywordIndex))
            {
               deviceType = stringToDeviceTypeTag(device);
            }

            CStringArray seicaPinMap;
            getSeicaPinMapping(seicaPinMap,*componentData,deviceType);

            for (int pinIndex = 1;pinIndex < seicaPinMap.GetSize();pinIndex++)
            {
               CString pinName = seicaPinMap.GetAt(pinIndex);

               if (!pinName.IsEmpty())
               {
                  CString pinRef = refDes + "." + pinName;
                  CString pinNumber;
                  pinNumber.Format("%d",pinIndex);

                  m_pinNumberMap.SetAt(pinRef,pinNumber);
               }
            }
         }
      }
   }
}

void CSeicaNodWriter::getSeicaPinMapping(CStringArray& seicaPinMapping,DataStruct& component,DeviceTypeTag deviceType)
{
   seicaPinMapping.RemoveAll();
   seicaPinMapping.SetSize(0,200);

   CString refDes = component.getInsert()->getRefname();
   BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component.getInsert()->getBlockNumber());
   int pinIndex = 1;

   for (CDataListIterator dataListIterator(*componentGeometry,insertTypePin);dataListIterator.hasNext();pinIndex++)
   {
      DataStruct* pinData = dataListIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();
      CString pinRef  = refDes + "." + pinName;
      CString seicaPinNumber;
      int pinNumber = 0;

      if (deviceType == deviceTypeUndefined)
      {
         seicaPinMapping.SetAtGrow(pinIndex,pinName);
      }
      else
      {
         switch(deviceType)
         {
         case deviceTypeCapacitorTantalum:
         case deviceTypeCapacitorPolarized:
            if      (pinName.CompareNoCase("Positive") == 0) pinNumber = 1;
            else if (pinName.CompareNoCase("Negative") == 0) pinNumber = 2;

            break;
         case deviceTypeDiode:
         case deviceTypeDiodeLed:
         case deviceTypeDiodeZener:
            if      (pinName.CompareNoCase("Anode"  ) == 0) pinNumber = 1;
            else if (pinName.CompareNoCase("Cathode") == 0) pinNumber = 2;

            break;
         case deviceTypeTransistor:
         case deviceTypeTransistorNpn:
         case deviceTypeTransistorPnp:
            if      (pinName.CompareNoCase("Emitter"  ) == 0) pinNumber = 1;
            else if (pinName.CompareNoCase("Base"     ) == 0) pinNumber = 2;
            else if (pinName.CompareNoCase("Collector") == 0) pinNumber = 3;

            break;
         case deviceTypeUnknown:
         case deviceTypeBattery:
         case deviceTypeCapacitor:
         case deviceTypeCapacitorArray:
         case deviceTypeConnector:
         case deviceTypeCrystal:
         case deviceTypeDiodeArray:
         case deviceTypeFilter:
         case deviceTypeFuse:
         case deviceTypeIC:
         case deviceTypeICDigital:
         case deviceTypeICLinear:
         case deviceTypeInductor:
         case deviceTypeJumper:
         case deviceTypeNoTest:
         case deviceTypeOscillator:
         case deviceTypePotentiometer:
         case deviceTypePowerSupply:
         case deviceTypeRelay:
         case deviceTypeResistor:
         case deviceTypeResistorArray:
         case deviceTypeSpeaker:
         case deviceTypeSwitch:
         case deviceTypeTestPoint:
         case deviceTypeTransformer:
         case deviceTypeTransistorArray:
         case deviceTypeTransistorFetNpn:
         case deviceTypeTransistorFetPnp:
         case deviceTypeTransistorMosfetNpn:
         case deviceTypeTransistorMosfetPnp:
         case deviceTypeTransistorScr:
         case deviceTypeTransistorTriac:
         case deviceTypeVoltageRegulator:
         case deviceTypeDiodeLedArray:
            {
               char* endChar;
               pinNumber = strtol(pinName,&endChar,10);

               if (*endChar != '\0')
               {
                  pinNumber = pinIndex + 1000;
               }
            }

            break;
         default:
         case deviceTypeUndefined:
            break;
         }

         if (pinNumber > 0)
         {
            seicaPinMapping.SetAtGrow(pinNumber,pinName);
         }
      }
   }
}

#ifdef oldCSeicaNodWriter
bool CSeicaNodWriter::write(const CString& outputFilePath)
{
   bool retval = false;

   CStdioFileWriteFormat writeFormat;
   const CString formatString = "%-14s %-16s %-4s %-10s %-10s %-10s %-3s %-4s %-4s %-12s\n";

   if (writeFormat.open(outputFilePath))
   {
      writeFormat.writef(formatString,
         "*SIGNAL","COMP.","PIN","X","Y","PAD","POS","TECN","TEST","CHANNEL USER");

      FileStruct* pcbFile = m_camCadDatabase.getSingleVisiblePcb();

      if (pcbFile != NULL)
      {
         retval = true;

         int technologyKeywordIndex         = m_camCadDatabase.getKeywordIndex(ATT_TECHNOLOGY);
         int probePlacementTypeKeywordIndex = m_camCadDatabase.getKeywordIndex(PROBE_PLACEMENT);

         CString value;

         BlockStruct* pcbBlock = pcbFile->getBlock();

         for (CDataListIterator pcbDataListIterator(*pcbBlock,dataTypeInsert);pcbDataListIterator.hasNext();)
         {
            DataStruct* componentData = pcbDataListIterator.getNext();
            InsertStruct* componentInsert = componentData->getInsert();
            CTMatrix matrix(componentInsert->getTMatrix());

            if (componentInsert->getInsertType() == insertTypePcbComponent ||
                componentInsert->getInsertType() == insertTypeTestPoint    ||
                componentInsert->getInsertType() == insertTypeVia              )
            {
               BlockStruct* componentGeometry = m_camCadDatabase.getBlock(insert->getBlockNumber());

               for (CDataListIterator componentGeometryDataListIterator(*componentGeometry,dataTypeInsert);
                    componentGeometryDataListIterator.hasNext();)
               {
                  DataStruct* padstackData = componentGeometryDataListIterator.getNext();
                  InsertStruct* padstackInsert = padstackData->getInsert();
                  BlockStruct* padstackGeometry = m_camCadDatabase.getBlock(padstackInsert->getBlockNumber());

                  if (padstackGeometry->getBlockType() == blockTypePadstack)
                  {
                     CBasesVector padstackBasesVector = padstackInsert->getBasesVector();
                     padstackBasesVector.transform(matrix);

                     CString subclass,partNumber;
                     CString refDes,device,value,unit,tolPlus,tolMinus,nrpin,macro;

                     refDes = insert->getRefname();

                     m_camCadDatabase.getAttributeStringValue(tolPlus   ,&(data->getAttributesRef()),pToleranceKeywordIndex);
                     m_camCadDatabase.getAttributeStringValue(tolMinus  ,&(data->getAttributesRef()),nToleranceKeywordIndex);
                     m_camCadDatabase.getAttributeStringValue(subclass  ,&(data->getAttributesRef()),subclassKeywordIndex);
                     m_camCadDatabase.getAttributeStringValue(partNumber,&(data->getAttributesRef()),partNumberKeywordIndex);

                     if (m_camCadDatabase.getAttributeStringValue(value,&(data->getAttributesRef()),valueKeywordIndex))
                     {
                        ComponentValues compValue(value);
                        value.Format("%d",round(compValue.getScientificValue()));
                        unit = metricPrefixTagToAbbreviatedString(compValue.getMetricPrefix()) + getSeicaUnits(compValue.GetUnits());
                     }

                     if (m_camCadDatabase.getAttributeStringValue(device,&(data->getAttributesRef()),deviceTypeKeywordIndex))
                     {
                        DeviceTypeTag deviceType = stringToDeviceTypeTag(device);
                        device = getSeicaDevice(deviceType,subclass,partNumber);
                        macro  = getSeicaMacro(deviceType);
                     }

                     BlockStruct* componentGeometry = m_camCadDatabase.getBlock(insert->getBlockNumber());

                     int pinCount          = componentGeometry->getPinCount();
                     int noConnectPinCount = componentGeometry->getNoConnectPinCount();
                     int connectedPinCount = pinCount -noConnectPinCount;

                     nrpin.Format("%d",connectedPinCount);

                     if (value.IsEmpty()   ) value    = "0";
                     if (unit.IsEmpty()    ) unit     = "0";
                     if (tolPlus.IsEmpty() ) tolPlus  = "0";
                     if (tolMinus.IsEmpty()) tolMinus = "0";
                     if (macro.IsEmpty()   ) macro    = "0";

                     writeFormat.writef(formatString,
                        "\"" + refDes + "\"",device,value,unit,tolPlus,tolMinus,nrpin,macro);
                  }
               }
            }
         }
      }
   }

   return retval;
}
#endif