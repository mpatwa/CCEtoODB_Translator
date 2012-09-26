// $Header: /CAMCAD/5.0/DataDoctor.cpp 79    6/17/07 8:50p Kurt Van Ness $

#include "StdAfx.h"
#include "CcDoc.h"
#include "DataDoctor.h"
#include "DataDoctorDialog.h"
#include "Response.h"
#include "CCEtoODB.h"
#include "PcbUtil.h"
#include "Centroid.h"



#include "GenerateCentroidAndOutline.h"  // for #define ALG_NAME_UNKNOWN, want to use same as Access Analysis


#define QOriginalValueKeyword "OriginalValue"


//_____________________________________________________________________________
afx_msg void CCEtoODBDoc::OnDataDoctor()
{
   CDataDoctor dataDoctor(*this);

   dataDoctor.doModal();

#if CamCadMajorVersion > 4 || (CamCadMajorVersion == 4 && CamCadMinorVersion > 4)
	FileStruct *pcbFile = dataDoctor.getCamCadDatabase().getSingleVisiblePcb();

	if (pcbFile != NULL)
   {
		//pcbFile->HideRealpartsForUnloadedComponents(*this);
      this->HideRealpartsForUnloadedComponents(*pcbFile);
   }
#endif
}

//_____________________________________________________________________________
CDataDoctorPin::CDataDoctorPin(CompPinStruct& pin,NetStruct& net) :
   m_pin(pin), 
   m_net(net)
{
}

//_____________________________________________________________________________
int CDataDoctorComponent::m_partNumberKeywordIndex            = -1;
int CDataDoctorComponent::m_deviceTypeKeywordIndex            = -1;
int CDataDoctorComponent::m_valueKeywordIndex                 = -1;
int CDataDoctorComponent::m_originalValueKeywordIndex         = -1;
int CDataDoctorComponent::m_pToleranceKeywordIndex            = -1;
int CDataDoctorComponent::m_nToleranceKeywordIndex            = -1;
int CDataDoctorComponent::m_toleranceKeywordIndex             = -1;
int CDataDoctorComponent::m_subclassKeywordIndex              = -1;
int CDataDoctorComponent::m_loadedKeywordIndex                = -1;
int CDataDoctorComponent::m_deviceToPackagePinmapKeywordIndex = -1;
int CDataDoctorComponent::m_mergedStatusKeywordIndex          = -1;
int CDataDoctorComponent::m_mergedValueKeywordIndex           = -1;
int CDataDoctorComponent::m_capacitiveOpensKeywordIndex       = -1;
int CDataDoctorComponent::m_diodeOpensKeywordIndex            = -1;
int CDataDoctorComponent::m_descriptionKeywordIndex			  = -1;
int CDataDoctorComponent::m_packageAliasKeywordIndex          = -1;
int CDataDoctorComponent::m_dpmoKeywordIndex                  = -1;
int CDataDoctorComponent::m_packageSourceKeywordIndex         = -1;
int CDataDoctorComponent::m_outlineMethodKeywordIndex         = -1;
int CDataDoctorComponent::m_compHeightKeywordIndex            = -1;
int CDataDoctorComponent::m_partCommentKeywordIndex           = -1;
int CDataDoctorComponent::m_packageCommentKeywordIndex        = -1;

//**************************************************************************************

//*rcf Move this to a BlockStruct member func, for now just make static here.
// Also, one of those datalistiterator classes might look neater
//DataStruct* BlockStruct::FindData(const CString& refdes, insertTypeTag insertType)
static DataStruct* FindData(BlockStruct *blk, const CString& refdes, InsertTypeTag insertType)
{
   for (POSITION pos = blk->getHeadDataInsertPosition();pos != NULL;)
   {
      DataStruct* data = blk->getNextDataInsert(pos);
      InsertStruct *insert = data->getInsert();

      if (insert->getInsertType() == insertType && insert->getRefname().Compare(refdes) == 0)
         return data;
   }

   return NULL;      
}

//**************************************************************************************

CDBGenericAttributeMap* CDataDoctorPackageInfo::AllocDBAttribMap()
{
   // Returned data becomes property of the caller

   CDBGenericAttributeMap *map = new CDBGenericAttributeMap;

   POSITION pos = this->m_genericAttribs.GetStartPosition();
   while (pos != NULL)
   {
      CString name, value;
      this->m_genericAttribs.GetNextAssoc(pos, name, value);
      map->Add(name, value);
   }

   return map;
}


//**************************************************************************************

CDataDoctorComponent::CDataDoctorComponent(CCamCadDatabase& camCadDatabase,DataStruct& camcadComponentData) :
   m_camcadComponentData(camcadComponentData)
{
   m_sortableRefDes = m_camcadComponentData.getInsert()->getSortableRefDes();
   m_status         = dataDoctorStatusError;

   m_mergedComponents = NULL;

   CString deviceTypeIdentifier = deviceTypeTagToValueString(stringToDeviceTypeTag(getDeviceType(camCadDatabase)));
   m_deviceType    = CDeviceTypes::getDeviceTypes().lookup(deviceTypeIdentifier);
   m_deviceTypeTag = stringToDeviceTypeTag(deviceTypeIdentifier);
}

CDataDoctorComponent::~CDataDoctorComponent()
{
   delete m_mergedComponents;
}

void CDataDoctorComponent::resetKeywordIndices()
{
   m_partNumberKeywordIndex            = -1;
   m_deviceTypeKeywordIndex            = -1;
   m_valueKeywordIndex                 = -1;
   m_originalValueKeywordIndex         = -1;
   m_pToleranceKeywordIndex            = -1;
   m_nToleranceKeywordIndex            = -1;
   m_toleranceKeywordIndex             = -1;
   m_subclassKeywordIndex              = -1;
   m_loadedKeywordIndex                = -1;
   m_deviceToPackagePinmapKeywordIndex = -1;
   m_mergedStatusKeywordIndex          = -1;
   m_mergedValueKeywordIndex           = -1;
   m_capacitiveOpensKeywordIndex       = -1;
   m_diodeOpensKeywordIndex            = -1;
	m_descriptionKeywordIndex           = -1;
   m_packageAliasKeywordIndex          = -1;
   m_dpmoKeywordIndex                  = -1;
   m_packageSourceKeywordIndex         = -1;
   m_outlineMethodKeywordIndex         = -1;
   m_compHeightKeywordIndex            = -1;
}

void CDataDoctorComponent::clearPinNameMaps()
{ 
	m_pinNameToPinRefMap.RemoveAll();
	m_pinRefToPinNameMap.RemoveAll();
}

void CDataDoctorComponent::createPinMaps(CCamCadDatabase& camCadDatabase)
{
	clearPinNameMaps();

	// Reset pinName map to those that are in the device type but set the pinRef to nothing
	if (m_deviceType != NULL)
	{
		for ( POSITION pos = m_deviceType->getPinNames().GetStartPosition(); pos != NULL;)
		{
			CString pinName, pinRef;
			m_deviceType->getPinNames().GetNextAssoc(pos, pinName, pinRef);

			if (!pinName.IsEmpty())
			{
				pinName.MakeLower();
				m_pinNameToPinRefMap.SetAt(pinName, "");
			}
		}
	}


	// Get the pinRef to pinName map from comppin attributes
   if (m_pinRefToPinNameMap.GetCount() == 0)
   {
      CDataDoctorPin* pin;

      for (POSITION pos = m_pins.GetStartPosition();pos != NULL;)
      {
			CString key;
         m_pins.GetNextAssoc(pos,key,pin);
         
			CString pinNameKey = getDeviceToPackagePinmap(camCadDatabase,*pin);
			CString pinRef = pin->getCompPin().getPinName();

			// Set the pairing
			m_pinRefToPinNameMap.SetAt(pinRef, pinNameKey);
			if (!pinNameKey.IsEmpty())
			{
				pinNameKey.MakeLower();
				m_pinNameToPinRefMap.SetAt(pinNameKey, pinRef);
			}
      }
   }
}

void CDataDoctorComponent::resetPinRefToPinNameMapAttrib(CCamCadDatabase& camCadDatabase)
{
	// Remove all pinmap attributes

	for (POSITION pos = m_pins.GetStartPosition(); pos != NULL;)
	{
		CString pinRef;
		CDataDoctorPin* pin = NULL;
		m_pins.GetNextAssoc(pos,pinRef,pin);

		camCadDatabase.removeAttribute(&(pin->getCompPin().getAttributesRef()), getDeviceToPackagePinmapKeywordIndex(camCadDatabase));
	}

	createPinMaps(camCadDatabase);
}

void CDataDoctorComponent::removeMergedComponents()
{ 
   delete m_mergedComponents;
   m_mergedComponents = NULL;
}

void CDataDoctorComponent::addMergedComponent(CDataDoctorComponent& component)
{
   if (m_mergedComponents == NULL)
   {
      m_mergedComponents = new CDataDoctorComponentList();
   }

   m_mergedComponents->AddTail(&component);
}

DeviceTypeTag CDataDoctorComponent::getPassiveDeviceType()
{
   DeviceTypeTag passiveDeviceType = deviceTypeUnknown;

   switch (getDeviceTypeTag())
   {
   case deviceTypeResistor:
      passiveDeviceType = deviceTypeResistor;
      break;
   case deviceTypeInductor:
      passiveDeviceType = deviceTypeInductor;
      break;
   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      passiveDeviceType = deviceTypeCapacitor;
      break;
   }

   return passiveDeviceType;
}

CString CDataDoctorComponent::getPinRefsToPinNamesMapString(const CString delimiter, bool allPins)
{
   // Include only pins that actually have a mapping, skip those with none

	CString pinNamesMapString;

	for (POSITION pos = m_pinRefToPinNameMap.GetStartPosition(); pos != NULL;)
	{
		CString pinNameKey, pinRef;
		m_pinRefToPinNameMap.GetNextAssoc(pos, pinRef, pinNameKey);
	
      if (allPins || (!pinRef.IsEmpty() && !pinNameKey.IsEmpty()))
      {
         if (!pinNamesMapString.IsEmpty())
            pinNamesMapString += delimiter;
         pinNamesMapString.AppendFormat("%s=%s ", pinRef, pinNameKey);
      }
	}
	
	return pinNamesMapString.Trim();
}

CString CDataDoctorComponent::getPinNameOptionsString(const CString delimiter)
{
   CString pinNamesString;

#ifdef B4_CASE_2220
   // Before case 2220, pin name options in Data Doc depended on current pin mapping.
   // This could leave holes in the menu (e.g. if transistor was mapped 1=Base, 2=Base, 3=Emitter then
   // Collector would not appear in offerings. 

	for (POSITION pos = m_pinNameToPinRefMap.GetStartPosition(); pos != NULL;)
	{
		CString pinName, pinRef;
		m_pinNameToPinRefMap.GetNextAssoc(pos, pinName, pinRef);
	
		if (!pinNamesString.IsEmpty())
			pinNamesString += delimiter;
		pinNamesString.AppendFormat("%s", pinName);
	}
#else
   // Pin Name Options should be based on Device definition

   if (m_deviceType != NULL)
   {
      CString pinName,value;

      for (POSITION pos = m_deviceType->getPinNames().GetStartPosition();pos != NULL;)
      {
         m_deviceType->getPinNames().GetNextAssoc(pos,pinName,value);

         if (!pinNamesString.IsEmpty())
            pinNamesString += delimiter;
         pinNamesString.AppendFormat("%s", pinName);
      }
   }

#endif
	
	return pinNamesString.Trim();
}

bool CDataDoctorComponent::hasPinFunctionName(CString pinNameKey)
{
   CString pinRef;
   pinNameKey.MakeLower();

   bool retval = (m_pinNameToPinRefMap.Lookup(pinNameKey,pinRef) != 0 && !pinRef.IsEmpty());

   return retval;
}
int CDataDoctorComponent::countPinsWithFunctionName(CString pinFunctionName)
{
   // Return number of pins with this function name.
   // First use map to check it exists at all, don't want to loop hopelessly on a hundred pin part.

   int count = 0;

   if (hasPinFunctionName(pinFunctionName))
   {
      // Loop on pin ref map, because pin name map can only have one entry for function name (by
      // definition of a map).
      POSITION mappos = m_pinRefToPinNameMap.GetStartPosition();
      while (mappos != NULL)
      {
         CString mappedPinRefname;
         CString mappedPinFunctionName;
         m_pinRefToPinNameMap.GetNextAssoc(mappos, mappedPinRefname, mappedPinFunctionName);

         if (mappedPinFunctionName.Compare(pinFunctionName) == 0)
            count++;
      }
   }

   return count;
}
CString CDataDoctorComponent::getPinMapStatus()
{
   if (m_deviceType != NULL)
   {
      // If dev type does not have pin names then no mapping is required.
      if (m_deviceType->getPinNames().GetCount() == 0)
         return "";

      // Check that all pin names in the device are mapped exactly one component pin
      for (POSITION pos = m_deviceType->getPinNames().GetStartPosition();pos != NULL;)
      {
         CString pinFunctionName, value;
         m_deviceType->getPinNames().GetNextAssoc(pos,pinFunctionName,value);

         if (countPinsWithFunctionName(pinFunctionName) != 1)
         {
            return QPinMapUnmap;
         }
      }

      // Check that every component pin is mapped to something
#ifdef NOT_GOOD_ENOUGH
      // The problem with this is it depends on the map being complete in the first place.
      // If some component pin is entirely missing from the map, it will not be found
      // to not be mapped. Only pins actually in the map with no mapping will be found.
      for (POSITION pos = m_pinRefToPinNameMap.GetStartPosition(); pos != NULL;)
      {
         CString pinNameKey, pinRef;
         m_pinRefToPinNameMap.GetNextAssoc(pos, pinRef, pinNameKey);

         if (pinNameKey.IsEmpty())
            return QPinMapUnmap;
      }
#else
      // Do check based on being able to find every component pin in the map and
      // that the mapping is not blank.
      POSITION pinpos = this->getPinStartPosition();
      for (int index = 0; pinpos != NULL; index++)
      {
         CDataDoctorPin *ddpin;
         CString pinSortablePinref;
         this->getPinNextAssoc(pinpos, pinSortablePinref, ddpin);

         CString pinRefname = ddpin->getCompPin().getPinName();

         CString pinFunctionName;
         if (!m_pinRefToPinNameMap.Lookup(pinRefname, pinFunctionName))
         {
            return QPinMapUnmap;  // Pin not in map
         }
         else
         {
            if (pinFunctionName.IsEmpty())  // Pin is in map but mapping is blank
               return QPinMapUnmap;
         }
      }
#endif

      // All Dev Type pin names are mapped to some component pin,
      // all component pins are mapped to some dev type pin name,
      // looks okay.
      return QPinMapMapped;	
   }

   // No dev type, no pin mapping required
   return "";
}

void CDataDoctorComponent::calculateStatus(CCamCadDatabase& camCadDatabase)
{
   bool valueRequiredFlag      = false;
   bool pToleranceRequiredFlag = false;
   bool nToleranceRequiredFlag = false;
   bool subclassRequiredFlag   = false;
   int pinCount    = -1;
   int minPinCount = -1;
   int maxPinCount = -1;

   if (!getLoaded(camCadDatabase))
   {
      m_status = dataDoctorStatusIgnore;
   }
   else
   {
      switch (m_deviceTypeTag)
      {
      case deviceTypeUnknown: 
         break;
      case deviceTypeBattery:             
         break;
      case deviceTypeCapacitor:  
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeCapacitorArray:      
         subclassRequiredFlag = true;
         minPinCount = 3;
         break;
      case deviceTypeCapacitorPolarized:  
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeCapacitorTantalum:   
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeConnector:           
         break;
      case deviceTypeCrystal:             
         break;
      case deviceTypeDiode:               
         break;
      case deviceTypeDiodeArray:          
         subclassRequiredFlag = true;
         minPinCount = 3;
         break;
      case deviceTypeDiodeLed:            
         break;
      case deviceTypeDiodeZener:          
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeFilter:              
         break;
      case deviceTypeFuse:                
         break;
      case deviceTypeIC:                  
         subclassRequiredFlag = true;
         minPinCount = 4;
         break;
      case deviceTypeICDigital:           
         subclassRequiredFlag = true;
         minPinCount = 4;
         break;
      case deviceTypeICLinear:            
         subclassRequiredFlag = true;
         minPinCount = 4;
         break;
      case deviceTypeInductor:            
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeJumper:              
         break;
      case deviceTypeNoTest:              
         break;
      case deviceTypeOscillator:          
         break;
      case deviceTypePotentiometer:       
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 3;
         break;
      case deviceTypePowerSupply:         
         subclassRequiredFlag = true;
         minPinCount = 2;
         break;
      case deviceTypeRelay:               
         subclassRequiredFlag = true;
         minPinCount = 4;
         break;
      case deviceTypeResistor:            
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeResistorArray:       
         valueRequiredFlag = pToleranceRequiredFlag = nToleranceRequiredFlag = true;
         minPinCount = 3;
         break;
      case deviceTypeSpeaker:             
         subclassRequiredFlag = true;
         pinCount = 2;
         break;
      case deviceTypeSwitch:              
         subclassRequiredFlag = true;
         minPinCount = 2;
         break;
      case deviceTypeTestPoint:           
         break;
      case deviceTypeTransformer:         
         subclassRequiredFlag = true;
         minPinCount = 4;
         break;
      case deviceTypeTransistor:          
         break;
      case deviceTypeTransistorArray:     
         subclassRequiredFlag = true;
         minPinCount = 4;
         break;
      case deviceTypeTransistorFetNpn:    
         break;
      case deviceTypeTransistorFetPnp:    
         break;
      case deviceTypeTransistorMosfetNpn: 
         break;
      case deviceTypeTransistorMosfetPnp: 
         break;
      case deviceTypeTransistorNpn:       
         break;
      case deviceTypeTransistorPnp:       
         break;
      case deviceTypeTransistorScr:       
         break;
      case deviceTypeTransistorTriac:     
         break;
      case deviceTypeVoltageRegulator:    
         break;
      case deviceTypeDiodeLedArray:       
         break;
		case deviceTypeOpto:
			break;
      }

      if (m_deviceType != NULL)
      {
         minPinCount = m_deviceType->getMinPinCount();
         maxPinCount = m_deviceType->getMaxPinCount();
      }

      m_status = dataDoctorStatusOk;
      m_errors.Empty();

      CString missingAttributeList;
      CString illegalAttributeList;
      CString error;
      CString commaDelimiter(", ");
      CString semiDelimiter("; ");
      CString empty("");

      CString rawDevType = this->getDeviceType(camCadDatabase);
      if (!rawDevType.IsEmpty())
      {
         DeviceTypeTag rawDevTypeTag = stringToDeviceTypeTag(rawDevType);
         if (rawDevTypeTag == deviceTypeUndefined)
         {
            // We get deviceTypeUndefined when the dev type does not match any
            // of the defined camcad device type. Sort of a misnomer, a better
            // name for the value might be deviceTypeUnrecognized. We checked
            // for blank dev type string first, so we know that if we get here
            // then device type is non-blank, but it is not recognized in camcad.

            m_status = dataDoctorStatusError;
            m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + "Unrecognized device type: " + rawDevType;
         }
      }

      ValueUnitTag defaultValueUnit = getDefaultValueUnitForDeviceType(getDeviceTypeTag());
      CString valueString = getValue(camCadDatabase);
      ComponentValues componentValue(valueString,defaultValueUnit);

      if (getDeviceType(camCadDatabase).IsEmpty() || m_deviceTypeTag == deviceTypeUnknown)
      {
         missingAttributeList += (! missingAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QDeviceType);
      }

      if (valueRequiredFlag)
      {
         if (valueString.IsEmpty())
         {
            missingAttributeList += (! missingAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QValue);
         }
         else
         {
            if (!componentValue.IsValid())
            {
               illegalAttributeList += (! illegalAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QValue);
            }
         }
      }

      if (pToleranceRequiredFlag)
      {
         CString tolval = getPTolerance(camCadDatabase);
         tolval.Trim();
         if (tolval.IsEmpty())
         {
            missingAttributeList += (! missingAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QPTolerance);
         }
         else if (atof(tolval) == 0.0) // case dts0100408178, 0% no longer acceptable
         {
            illegalAttributeList += (! illegalAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QPTolerance);
         }
      }

      if (nToleranceRequiredFlag)
      {
         CString tolval = getNTolerance(camCadDatabase);
         tolval.Trim();
         if (tolval.IsEmpty())
         {
            missingAttributeList += (! missingAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QNTolerance);
         }
         else if (atof(tolval) == 0.0) // case dts0100408178, 0% no longer acceptable
         {
            illegalAttributeList += (! illegalAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QNTolerance);
         }
      }

      if (subclassRequiredFlag && getSubclass(camCadDatabase).IsEmpty())
      {
         missingAttributeList += (! missingAttributeList.IsEmpty() ? commaDelimiter : empty) + quoteString(QSubclass);
      }

      if (! missingAttributeList.IsEmpty())
      {
         m_status = dataDoctorStatusError;
         m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + "Missing attributes: " + missingAttributeList;
      }

      if (! illegalAttributeList.IsEmpty())
      {
         m_status = dataDoctorStatusError;
         m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + "Illegal attributes: " + illegalAttributeList;
      }

      int componentPinCount          = getPinCount();
      int componentNoConnectPinCount = getNoConnectPinCount(camCadDatabase);
      int componentConnectedPinCount = componentPinCount - componentNoConnectPinCount;

      if (pinCount != -1 && pinCount != componentConnectedPinCount)
      {
         m_status = dataDoctorStatusError;
         error.Format("Pin count of %d not equal to required pin count of %d",componentConnectedPinCount,pinCount);

         m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + error;
      }

      if (minPinCount != -1 && componentConnectedPinCount < minPinCount)
      {
         m_status = dataDoctorStatusError;
         error.Format("Minimum pin count of %d exceeds pin count of %d",minPinCount,componentConnectedPinCount);

         m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + error;
      }

      if (maxPinCount != -1 && componentConnectedPinCount > maxPinCount)
      {
         m_status = dataDoctorStatusError;
         error.Format("Maximum pin count of %d exceeded by pin count of %d",maxPinCount,componentConnectedPinCount);

         m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + error;
      }

      if (m_deviceType != NULL)
      {
         //BlockStruct* componentGeometry = camCadDatabase.getBlock(m_component.getInsert()->getBlockNumber());
         CString pinName,value;

         for (POSITION pos = m_deviceType->getPinNames().GetStartPosition();pos != NULL;)
         {
            m_deviceType->getPinNames().GetNextAssoc(pos, pinName, value);

            int usageCount = this->countPinsWithFunctionName(pinName);
            switch (usageCount)
            {
            case 0:
               m_status = dataDoctorStatusError;
               error.Format("Missing pin name '%s'",pinName);
               m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + error;
               break;
            case 1:
               // This is correct usage, no error
               break;
            default:
               // More than one
               m_status = dataDoctorStatusError;
               error.Format("More than one pin named '%s'",pinName);
               m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + error;
               break;
            }
         }
      }

      if (getDeviceTypeTag() != deviceTypeUndefined &&
         componentValue.IsValid())
      {
         if (! isValueUnitCompatibleWithDeviceType(componentValue.GetUnits(),getDeviceTypeTag()))
         {
            m_status = dataDoctorStatusError;
            error.Format("Unit '%s' is incompatible with device type",valueUnitToString(componentValue.GetUnits()));

            m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + error;
         }
      }
   }
}

ValueUnitTag CDataDoctorComponent::getUnits()
{
   ValueUnitTag units = valueUnitUndefined;

   switch (getPassiveDeviceType())
   {
   case deviceTypeCapacitor:  units = valueUnitFarad;  break;
   case deviceTypeResistor:   units = valueUnitOhm;    break;
   case deviceTypeInductor:   units = valueUnitHenry;  break;
   }

   return units;
}

void CDataDoctorComponent::saveOriginalValue(CCamCadDatabase& camCadDatabase)
{
   Attrib* valueAttribute;
   Attrib* originalValueAttribute;
   CString valueString;

   if (m_camcadComponentData.getDefinedAttributeMap()->Lookup(getValueKeywordIndex(camCadDatabase),valueAttribute))
   {
      if (! m_camcadComponentData.getDefinedAttributeMap()->Lookup(getOriginalValueKeywordIndex(camCadDatabase),originalValueAttribute))
      {
         camCadDatabase.getAttributeStringValue(valueString,&(m_camcadComponentData.getAttributesRef()),getValueKeywordIndex(camCadDatabase));
         setAttribute(camCadDatabase,getOriginalValueKeywordIndex(camCadDatabase),valueString);
      }
   }
}

int CDataDoctorComponent::getPartNumberKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_partNumberKeywordIndex < 0)
   {
      m_partNumberKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PARTNUMBER);
   }

   return m_partNumberKeywordIndex;
}

int CDataDoctorComponent::getDeviceTypeKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_deviceTypeKeywordIndex < 0)
   {
      m_deviceTypeKeywordIndex = camCadDatabase.getKeywordIndex(ATT_DEVICETYPE);
   }

   return m_deviceTypeKeywordIndex;
}

int CDataDoctorComponent::getValueKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_valueKeywordIndex < 0)
   {
      m_valueKeywordIndex = camCadDatabase.getKeywordIndex(ATT_VALUE);
   }

   return m_valueKeywordIndex;
}

int CDataDoctorComponent::getOriginalValueKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_originalValueKeywordIndex < 0)
   {
      m_originalValueKeywordIndex = camCadDatabase.getKeywordIndex(QOriginalValueKeyword);
   }

   return m_originalValueKeywordIndex;
}

int CDataDoctorComponent::getPToleranceKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_pToleranceKeywordIndex < 0)
   {
      m_pToleranceKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PLUSTOLERANCE);
   }

   return m_pToleranceKeywordIndex;
}

int CDataDoctorComponent::getNToleranceKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_nToleranceKeywordIndex < 0)
   {
      m_nToleranceKeywordIndex = camCadDatabase.getKeywordIndex(ATT_MINUSTOLERANCE);
   }

   return m_nToleranceKeywordIndex;
}

int CDataDoctorComponent::getToleranceKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_toleranceKeywordIndex < 0)
   {
      m_toleranceKeywordIndex = camCadDatabase.getKeywordIndex(ATT_TOLERANCE);
   }

   return m_toleranceKeywordIndex;
}

int CDataDoctorComponent::getSubclassKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_subclassKeywordIndex < 0)
   {
      m_subclassKeywordIndex = camCadDatabase.getKeywordIndex(ATT_SUBCLASS);

      if (m_subclassKeywordIndex < 0)
      {
         m_subclassKeywordIndex = camCadDatabase.registerKeyword(ATT_SUBCLASS,valueTypeString);
      }
   }

   return m_subclassKeywordIndex;
}

int CDataDoctorComponent::getLoadedKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_loadedKeywordIndex < 0)
   {
      m_loadedKeywordIndex = camCadDatabase.getKeywordIndex(ATT_LOADED);
   }

   return m_loadedKeywordIndex;
}

int CDataDoctorComponent::getDeviceToPackagePinmapKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_deviceToPackagePinmapKeywordIndex < 0)
   {
      m_deviceToPackagePinmapKeywordIndex = camCadDatabase.getKeywordIndex(ATT_DEVICETOPACKAGEPINMAP);
   
		if (m_deviceToPackagePinmapKeywordIndex < 0)
      {
         m_deviceToPackagePinmapKeywordIndex = camCadDatabase.registerKeyword(ATT_DEVICETOPACKAGEPINMAP,valueTypeString);
      }
	}

   return m_deviceToPackagePinmapKeywordIndex;
}

int CDataDoctorComponent::getMergedStatusKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_mergedStatusKeywordIndex < 0)
   {
      m_mergedStatusKeywordIndex = camCadDatabase.registerKeyword("MergedStatus",valueTypeString);
   }

   return m_mergedStatusKeywordIndex;
}

int CDataDoctorComponent::getMergedValueKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_mergedValueKeywordIndex < 0)
   {
      m_mergedValueKeywordIndex = camCadDatabase.registerKeyword("MergedValue",valueTypeString);
   }

   return m_mergedValueKeywordIndex;
}

int CDataDoctorComponent::getCapacitiveOpensKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_capacitiveOpensKeywordIndex < 0)
   {
      m_capacitiveOpensKeywordIndex = camCadDatabase.registerKeyword(ATT_CAP_OPENS,valueTypeString);
   }

   return m_capacitiveOpensKeywordIndex;
}

int CDataDoctorComponent::getDiodeOpensKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_diodeOpensKeywordIndex < 0)
   {
      m_diodeOpensKeywordIndex = camCadDatabase.registerKeyword(ATT_DIODE_OPENS, valueTypeString);
   }

   return m_diodeOpensKeywordIndex;
}

int CDataDoctorComponent::getDescriptionKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_descriptionKeywordIndex < 0)
   {
      m_descriptionKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PART_DESCRIPTION);
   }

   return m_descriptionKeywordIndex;
}

int CDataDoctorComponent::getPackageAliasKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_packageAliasKeywordIndex < 0)
   {
      m_packageAliasKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PACKAGE_ALIAS);

		if (m_packageAliasKeywordIndex < 0)
      {
         m_packageAliasKeywordIndex = camCadDatabase.registerKeyword(ATT_PACKAGE_ALIAS, valueTypeString);
      }
   }

   return m_packageAliasKeywordIndex;
}


int CDataDoctorComponent::getDPMOKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_dpmoKeywordIndex < 0)
   {
      m_dpmoKeywordIndex = camCadDatabase.getKeywordIndex(ATT_DPMO);

		if (m_dpmoKeywordIndex < 0)
      {
         m_dpmoKeywordIndex = camCadDatabase.registerKeyword(ATT_DPMO, valueTypeInteger);
      }
   }

   return m_dpmoKeywordIndex;
}


int CDataDoctorComponent::getGenericAttribKeywordIndex(CCamCadDatabase& camCadDatabase, CString attribName)
{
   // These are not cached, just look it up every time.

   int kwi = -1;

   {
      kwi = camCadDatabase.getKeywordIndex(attribName);

		if (kwi < 0)
      {
         kwi = camCadDatabase.registerKeyword(attribName, valueTypeString);
      }
   }

   return kwi;
}

int CDataDoctorComponent::getPackageSourceKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_packageSourceKeywordIndex < 0)
   {
      m_packageSourceKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PACKAGE_SOURCE);

		if (m_packageSourceKeywordIndex < 0)
      {
         m_packageSourceKeywordIndex = camCadDatabase.registerKeyword(ATT_PACKAGE_SOURCE, valueTypeString);
      }
   }

   return m_packageSourceKeywordIndex;
}

int CDataDoctorComponent::getOutlineMethodKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_outlineMethodKeywordIndex < 0)
   {
      m_outlineMethodKeywordIndex = camCadDatabase.getKeywordIndex(ATT_OUTLINE_METHOD);

		if (m_outlineMethodKeywordIndex < 0)
      {
         m_outlineMethodKeywordIndex = camCadDatabase.registerKeyword(ATT_OUTLINE_METHOD, valueTypeString);
      }
   }

   return m_outlineMethodKeywordIndex;
}

int CDataDoctorComponent::getCompHeightKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_compHeightKeywordIndex < 0)
   {
      m_compHeightKeywordIndex = camCadDatabase.getKeywordIndex(ATT_COMPHEIGHT);

		if (m_compHeightKeywordIndex < 0)
      {
         m_compHeightKeywordIndex = camCadDatabase.registerKeyword(ATT_COMPHEIGHT, valueTypeString);
      }
   }

   return m_compHeightKeywordIndex;
}

int CDataDoctorComponent::getPartCommentKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   m_partCommentKeywordIndex = camCadDatabase.getKeywordIndex("PartComment");

	if (m_partCommentKeywordIndex < 0)
   {
      m_partCommentKeywordIndex = camCadDatabase.registerKeyword("PartComment", valueTypeString);
   }

   return m_partCommentKeywordIndex;
}

int CDataDoctorComponent::getPackageCommentKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   m_packageCommentKeywordIndex = camCadDatabase.getKeywordIndex("PackageComment");

	if (m_packageCommentKeywordIndex < 0)
   {
      m_packageCommentKeywordIndex = camCadDatabase.registerKeyword("PackageComment", valueTypeString);
   }

   return m_packageCommentKeywordIndex;
}

CString CDataDoctorComponent::getCompHeight(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getCompHeightKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getPartNumber(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getPartNumberKeywordIndex(camCadDatabase));

   return retval;
}


CString CDataDoctorComponent::getOutlineMethod(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getOutlineMethodKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getPackageAlias(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getPackageAliasKeywordIndex(camCadDatabase));

   return retval;
}


CString CDataDoctorComponent::getDPMO(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getDPMOKeywordIndex(camCadDatabase));

   return retval;
}


CString CDataDoctorComponent::getGenericAttrib(CCamCadDatabase& camCadDatabase, CString attribName)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getGenericAttribKeywordIndex(camCadDatabase, attribName));

   return retval;
}

CString CDataDoctorComponent::getPackageSource(CCamCadDatabase& camCadDatabase)
{
   CString retval(QGeometry); // If attrib is not set then default is Geometry

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getPackageSourceKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getPartComments(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getPartCommentKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getPackageComments(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getPackageCommentKeywordIndex(camCadDatabase));

   return retval;
}

BlockStruct *CDataDoctorComponent::getGeometryBlock(CCamCadDatabase& camCadDatabase)
{
   BlockStruct *insertedBlock = NULL;

   if (m_camcadComponentData.getInsert() != NULL)
   {
      int insertedBlkNum = m_camcadComponentData.getInsert()->getBlockNumber();
      insertedBlock = camCadDatabase.getCamCadDoc().getBlockAt(insertedBlkNum);
   }

   return insertedBlock;
}

CString CDataDoctorComponent::getGeometryName(CCamCadDatabase& camCadDatabase)
{
   BlockStruct *blk = this->getGeometryBlock(camCadDatabase);
   if (blk != NULL)
      return blk->getName();

   return "";
}

CString CDataDoctorComponent::getDeviceType(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getDeviceTypeKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getValue(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getValueKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getNormalizedValue(CCamCadDatabase& camCadDatabase)
{
   DeviceTypeTag deviceType = getDeviceTypeTag();
   ValueUnitTag defaultUnit = getDefaultValueUnitForDeviceType(deviceType);

   ComponentValues componentValue(defaultUnit);
   componentValue.SetValue(getValue(camCadDatabase));
   CString retval = componentValue.GetPrintableString();

   return retval;
}

CString CDataDoctorComponent::getDisplayValue(CCamCadDatabase& camCadDatabase)
{
   CString retval = getNormalizedValue(camCadDatabase);

   if (retval.IsEmpty())
   {
      retval = getValue(camCadDatabase);
   }

   return retval;
}

CString CDataDoctorComponent::getPTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getPToleranceKeywordIndex(camCadDatabase));

   if (retval.IsEmpty())
   {
      retval = getTolerance(camCadDatabase);

      if (! retval.IsEmpty())
      {
         camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),getPToleranceKeywordIndex(camCadDatabase),retval);
      }
   }

   if (! retval.IsEmpty())
   {
      if (getNTolerance(camCadDatabase).IsEmpty())
      {
         camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),getNToleranceKeywordIndex(camCadDatabase),retval);
      }
   }

   return retval;
}

CString CDataDoctorComponent::getNTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getNToleranceKeywordIndex(camCadDatabase));

   if (retval.IsEmpty())
   {
      retval = getTolerance(camCadDatabase);

      if (! retval.IsEmpty())
      {
         camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),getNToleranceKeywordIndex(camCadDatabase),retval);
      }
   }

   return retval;
}

CString CDataDoctorComponent::getTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getToleranceKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getNormalizedTolerance(const CString& unnormalizedTolerance)
{
   CString retval;

   if (! unnormalizedTolerance.IsEmpty())
   {
      ComponentValues componentValue;
      componentValue.SetValue(unnormalizedTolerance);

      if (componentValue.GetUnits() == valueUnitUndefined || 
          componentValue.GetUnits() == valueUnitOhm         )
      {
         double value = fabs(componentValue.GetValue());

         //if      (value < -100.) value = -100.;
         //else if (value >  100.) value =  100.;

         componentValue.SetValue(value);
         componentValue.SetUnits(valueUnitPercent);
      }

      retval = componentValue.GetPrintableString();
   }

   return retval;
}

CString CDataDoctorComponent::getNormalizedPTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval = getNormalizedTolerance(getPTolerance(camCadDatabase));
   setPTolerance(camCadDatabase,retval);

   return retval;
}

CString CDataDoctorComponent::getNormalizedNTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval = getNormalizedTolerance(getNTolerance(camCadDatabase));
   setNTolerance(camCadDatabase,retval);

   return retval;
}

CString CDataDoctorComponent::getSubclass(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getSubclassKeywordIndex(camCadDatabase));

   return retval;
}

bool CDataDoctorComponent::getLoaded(CCamCadDatabase& camCadDatabase)
{
   CString boolString;

   if (!camCadDatabase.getAttributeStringValue(boolString,&(m_camcadComponentData.getAttributeMap()),
          getLoadedKeywordIndex(camCadDatabase)))
   {
      boolString = "true";
      setLoaded(camCadDatabase,boolString);
   }

   bool retval = (boolString.CompareNoCase("false") != 0);

   return retval;
}

CString CDataDoctorComponent::getDeviceToPackagePinmap(CCamCadDatabase& camCadDatabase,CDataDoctorPin& pin)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(pin.getCompPin().getAttributesRef()),
      getDeviceToPackagePinmapKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getMergedStatus(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getMergedStatusKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getMergedValue(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getMergedValueKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getCapacitiveOpens(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getCapacitiveOpensKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getDiodeOpens(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getDiodeOpensKeywordIndex(camCadDatabase));

   return retval;
}
		
CString CDataDoctorComponent::getDescription(CCamCadDatabase& camCadDatabase)
{
   CString retval;

   camCadDatabase.getAttributeStringValue(retval,&(m_camcadComponentData.getAttributeMap()),
      getDescriptionKeywordIndex(camCadDatabase));

   return retval;
}

CString CDataDoctorComponent::getDefinedCapacitiveOpens(CCamCadDatabase& camCadDatabase)
{
   CString retval = getCapacitiveOpens(camCadDatabase);

   if (retval.IsEmpty())
   {
      retval = "False";
   }

   return retval;
}

CString CDataDoctorComponent::getDefinedDiodeOpens(CCamCadDatabase& camCadDatabase)
{
   CString retval = getDiodeOpens(camCadDatabase);

   if (retval.IsEmpty())
   {
      retval = "False";
   }

   return retval;
}

int CDataDoctorComponent::getPinCount() const
{
   return m_pins.GetCount();
}

int CDataDoctorComponent::getNoConnectPinCount(CCamCadDatabase& camCadDatabase)
{
   int noConnectPinCount = 0;
   CString key,pinName;
   CDataDoctorPin* pin;

   for (POSITION pos = m_pins.GetStartPosition();pos != NULL;)
   {
      m_pins.GetNextAssoc(pos,key,pin);

      if (pin != NULL && this->IsNoConnect(camCadDatabase, *pin))
      {
         noConnectPinCount++;
      }
   }

   return noConnectPinCount;
}

bool CDataDoctorComponent::IsNoConnect(CCamCadDatabase &camCadDatabase, CDataDoctorPin &pin)
{
   CString pinName = getDeviceToPackagePinmap(camCadDatabase, pin);

   if (pinName.CompareNoCase(ATT_VALUE_NO_CONNECT) == 0)
   {
      return true;
   }

   return false;
}

CString CDataDoctorComponent::getPinCountString()
{
   CString retval;

   retval.Format("%d",getPinCount());

   return retval;
}

CString CDataDoctorComponent::getStatusString()
{
   CString retval;

   switch (m_status)
   {
   case dataDoctorStatusOk:         retval = "OK";       break;
   case dataDoctorStatusError:      retval = "Errors";   break;
   case dataDoctorStatusIgnore:     retval = "Ignored";  break;
   case dataDoctorStatusUndefined:  retval = "?";        break;
   }

   return retval;
}

CString CDataDoctorComponent::getErrors()
{
   return m_errors;
}

CPoint2d CDataDoctorComponent::GetPinsCentroid(CCamCadDatabase& camCadDatabase)
{
   CPoint2d pnt(0.,0.);

   BlockStruct *insertedCompBlk = this->getGeometryBlock(camCadDatabase);

   if (insertedCompBlk != NULL)
   {
      double centX, centY;
      if (CalculateCentroid_PinCenters(&camCadDatabase.getCamCadDoc(), insertedCompBlk, &centX, &centY))
      {
         pnt.x = centX;
         pnt.y = centY;
      }
   }

   return pnt;
}

DataStruct *CDataDoctorComponent::FindAssociatedRealPartInsertData(CCamCadDatabase& camCadDatabase)
{
   FileStruct *pcbFile = camCadDatabase.getSingleVisibleFile();

   //*rcf  This would have done it...    DataStruct *oldInsertData = FindData(pcbblk, ddcomp->getRefDes(), insertTypeRealPart);

   if (pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
      POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *d = pcbFile->getBlock()->getNextDataInsert(pos);

         if (d->isInsertType(insertTypeRealPart) &&
            d->getInsert()->getRefname().CompareNoCase(this->getRefDes()) == 0)
         {
            return d;
         }
      }
   }

   return NULL;
}

DataStruct *CDataDoctorComponent::GetOldStyleDFTOutlineData(CCamCadDatabase& camCadDatabase)
{

   BlockStruct *insertedBlk = this->getGeometryBlock(camCadDatabase);
   if (insertedBlk != NULL)
   {
      POSITION pos = insertedBlk->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = insertedBlk->getDataList().GetNext(pos);

         if (data != NULL && data->getDataType() == dataTypePoly &&
            data->getGraphicClass() == graphicClassPackageOutline)
         {
            return data;
         }
      }
   }

   return NULL;
}

CString CDataDoctorComponent::GetExistingOutlineMethod(CCamCadDatabase& camCadDatabase)
{
   // Observe that blank return means there is no outline.
   // "Unknown" return means there is an outline but we don't know the method that created it.

   CString algorithm;

   DataStruct *data = this->GetOldStyleDFTOutlineData(camCadDatabase);  //*rcf No good cuz the outline in geom gets deleted, need outline from RP

   if (data != NULL)
   {
      WORD keyword = camCadDatabase.getCamCadDoc().RegisterKeyWord(ATT_OUTLINE_METHOD, 0, VT_STRING);

      Attrib* attrib = NULL;
      if (data->getAttributes() && data->getAttributes()->Lookup(keyword, attrib))
      {
         algorithm = attrib->getStringValue();
      }
      else
      {
         algorithm = outlineAlgTagToString(algUnknown);
      }
   }

   return algorithm;
}

void CDataDoctorComponent::setAttribute(CCamCadDatabase& camCadDatabase,int keywordIndex,const CString& stringValue)
{
   if (stringValue.IsEmpty())
   {
      camCadDatabase.removeAttribute(&(m_camcadComponentData.getAttributeMap()),keywordIndex);
   }
   else
   {
      camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),keywordIndex, stringValue);
   }
}

void CDataDoctorComponent::setPartNumber(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getPartNumberKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setOutlineMethod(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getOutlineMethodKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setPackageAlias(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getPackageAliasKeywordIndex(camCadDatabase),stringValue);
}


void CDataDoctorComponent::setDPMO(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getDPMOKeywordIndex(camCadDatabase),stringValue);
}


void CDataDoctorComponent::setGenericAttrib(CCamCadDatabase& camCadDatabase, const CString& attribName, const CString& stringValue)
{
   setAttribute(camCadDatabase,getGenericAttribKeywordIndex(camCadDatabase, attribName), stringValue);
}

void CDataDoctorComponent::setPackageSource(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getPackageSourceKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setCompHeight(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getCompHeightKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setPartComments(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getPartCommentKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setPackageComments(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getPackageCommentKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setDeviceType(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   CString deviceTypeIdentifier(stringValue);

   if (!deviceTypeIdentifier.Trim().IsEmpty())
   {
      m_deviceTypeTag      = stringToDeviceTypeTag(deviceTypeIdentifier);
      deviceTypeIdentifier = deviceTypeTagToValueString(m_deviceTypeTag);
      m_deviceType         = CDeviceTypes::getDeviceTypes().lookup(deviceTypeIdentifier);
	}

   setAttribute(camCadDatabase,getDeviceTypeKeywordIndex(camCadDatabase),deviceTypeIdentifier);
}

void CDataDoctorComponent::setValue(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getValueKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setOriginalValue(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getOriginalValueKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setPTolerance(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getPToleranceKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setNTolerance(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getNToleranceKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setPinRefToPinNamesMapString(CCamCadDatabase& camCadDatabase,const CString& value, CString &errMsg)
{
   // This setPinRefToPinNamesMapString func is not used to accumulate individual settings, it makes all
   // settings for all pins all at once. I.e. not to be called individually to set pin 1, then 2, then 3, etc.
   // It is called with map for all pins in comp (that have a mapping) and applied all at once.

	CStringArray records;
   CSupString pinNamesMapString = value;
	pinNamesMapString.ParseQuote(records, "|");

   // If value is not empty but we found no records, something must be wrong.
   errMsg.Empty();
   if (!value.IsEmpty() && records.GetCount() < 1)
      errMsg.Format("Pin mapping value for part number %s is malformed.\n[%s]", this->getPartNumber(camCadDatabase), value);

   // Clear the DD comp specific "convenience" maps.
   clearPinNameMaps();

   // Delete all existing pinmap attribs.
   resetPinRefToPinNameMapAttrib(camCadDatabase);

   // Make new entries -- add back only non-blank pin mappings
	for (int index=0; index<records.GetCount(); index++)
	{
		CStringArray params;
		CSupString record = records.GetAt(index);
		record.ParseQuote(params, "=");

		if (params.GetCount() > 0)
		{
			CString pinRef = params.GetAt(0);
         CString pinName;
         if (params.GetCount() > 1)
			   pinName = params.GetAt(1);

			pinRef.Trim();
			pinName.Trim();

			CString key;
			CDataDoctorPin* pin;
			for (POSITION pos = m_pins.GetStartPosition();pos != NULL;)
			{
				m_pins.GetNextAssoc(pos,key,pin);
				if (pin->getCompPin().getPinName().CompareNoCase(pinRef) == 0)
            {
               // In new scheme we want the attrib absent if not actually mapped, this result
               // is from discussion with Mark 04 Feb 2009. During init above we now delete
               // all attribs first, so now we only want to add back non-blank settings.
               if (!pinName.IsEmpty())
               {
                  camCadDatabase.addAttribute(&(pin->getCompPin().getAttributesRef()), 
                     getDeviceToPackagePinmapKeywordIndex(camCadDatabase), pinName);
               }
            }
			}

         // This part of map gets entry for pinRef even when mapped-to pinName is blank
			m_pinRefToPinNameMap.SetAt(pinRef, pinName);

         // This part of the map gets entry only if mapped-to pinName is non-blank
			if (!pinName.IsEmpty()) // Removed the following "and", it prevents update of "MAPPED" status when adding mappings via Data Doctor or directly in msAccess (and then Load DB) -- && m_pinNameToPinRefMap.Lookup(pinName, key))
			{
				pinName.MakeLower();
				m_pinNameToPinRefMap.SetAt(pinName, pinRef);
			}
		}
	}
}

void CDataDoctorComponent::setSubclass(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getSubclassKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setLoaded(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getLoadedKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setMergedStatus(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),
      getMergedStatusKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setMergedValue(CCamCadDatabase& camCadDatabase,double value)
{
   ComponentValues componentValue(value,getUnits());

   camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),
      getMergedValueKeywordIndex(camCadDatabase),componentValue.GetPrintableString());
}

void CDataDoctorComponent::setDescription(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   setAttribute(camCadDatabase,getDescriptionKeywordIndex(camCadDatabase),stringValue);
}

void CDataDoctorComponent::setCapacitiveOpens(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   CString normalizedValue;

   if (stringValue.CompareNoCase("true") == 0 ||
       stringValue.CompareNoCase("t"   ) == 0 ||
       stringValue == "1")
   {
      normalizedValue = "True";
   }
   else if (stringValue.CompareNoCase("false") == 0 ||
            stringValue.CompareNoCase("f"    ) == 0 ||
            stringValue == "0")
   {
      normalizedValue = "False";
   }

   if (normalizedValue.IsEmpty())
   {
      camCadDatabase.removeAttribute(&(m_camcadComponentData.getAttributeMap()),
         getCapacitiveOpensKeywordIndex(camCadDatabase));
   }
   else
   {
      camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),
         getCapacitiveOpensKeywordIndex(camCadDatabase),normalizedValue);
   }
}

void CDataDoctorComponent::setDiodeOpens(CCamCadDatabase& camCadDatabase,const CString& stringValue)
{
   CString normalizedValue;

   if (stringValue.CompareNoCase("true") == 0 ||
       stringValue.CompareNoCase("t"   ) == 0 ||
       stringValue == "1")
   {
      normalizedValue = "True";
   }
   else if (stringValue.CompareNoCase("false") == 0 ||
            stringValue.CompareNoCase("f"    ) == 0 ||
            stringValue == "0")
   {
      normalizedValue = "False";
   }

   if (normalizedValue.IsEmpty())
   {
      camCadDatabase.removeAttribute(&(m_camcadComponentData.getAttributeMap()),
         getDiodeOpensKeywordIndex(camCadDatabase));
   }
   else
   {
      camCadDatabase.addAttribute(&(m_camcadComponentData.getAttributeMap()),
         getDiodeOpensKeywordIndex(camCadDatabase),normalizedValue);
   }
}

void CDataDoctorComponent::removeMergedAttributes(CCamCadDatabase& camCadDatabase)
{
   camCadDatabase.removeAttribute(&(m_camcadComponentData.getAttributeMap()),
      getMergedStatusKeywordIndex(camCadDatabase));

   camCadDatabase.removeAttribute(&(m_camcadComponentData.getAttributeMap()),
      getMergedValueKeywordIndex(camCadDatabase));
}

//_____________________________________________________________________________
CDataDoctorComponents::CDataDoctorComponents(bool isContainer,int size)
   : CTypedMapSortStringToObContainer<CDataDoctorComponent>(size,isContainer)
{   
}

void CDataDoctorComponents::createPinMaps(CCamCadDatabase& camCadDatabase)
{
	for (POSITION pos = this->GetStartPosition(); pos != NULL;)
	{
		CString refdes;
		CDataDoctorComponent* component = NULL;
		this->GetNextAssoc(pos, refdes, component);
		if (component != NULL)
			component->createPinMaps(camCadDatabase);
	}
}

int CDataDoctorComponents::GetDieCount()
{
   int DieCount = 0;
   for (POSITION pos = this->GetStartPosition(); pos != NULL;)
	{
		CString refdes;
		CDataDoctorComponent* component = NULL;
		this->GetNextAssoc(pos, refdes, component);
      if (component != NULL && component->IsDie())
			DieCount ++;
	}

   return DieCount;
}

//_____________________________________________________________________________
CDataDoctorPart::CDataDoctorPart(const CString& partNumber, bool isDiePart)
   : m_components(false,nextPrime2n(50))
   , m_partNumber(partNumber)
	, m_fromLibrary(dataDoctorLibraryNo)
   , m_realPart("PART", partNumber)
   , m_isDiePart(isDiePart)
{
   // A hack to propagate the "fill" setting from the Packages tab to the construction
   // of parts for Parts tab. Need to promote this setting to a base Data Doc setting so
   // both tabs can interact with it as needed and not be a hack. Too much of a change
   // for 10 days before DC.
   bool outlineFill = CDataDoctorPackagesPage::GetOutlineFillMode();
   this->m_pkgInfo.SetOutlineFilled(outlineFill);
}

CDataDoctorPart::~CDataDoctorPart()
{
}

void CDataDoctorPart::addComponent(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& component, CDataDoctorLibraryAttribMap& libraryAttribMap)
{
	if (component.getLoaded(camCadDatabase))
   {
		m_components.SetAt(component.getSortableRefDes(),&component);

      // If first component, set up package info
      if (m_components.GetCount() == 1)
      {
         if (m_realPart.GetBlock() == NULL)
            m_realPart.GenerateSelf(camCadDatabase, component, this->m_pkgInfo.GetOutlineFilled());

         m_realPart.SetOutlineFill(camCadDatabase, this->m_pkgInfo.GetOutlineFilled());

         m_pkgInfo.SetPackageSource( component.getPackageSource(camCadDatabase) );
         m_pkgInfo.SetOutlineMethod( m_realPart.GetExistingOutlineMethod(camCadDatabase) );
         m_pkgInfo.SetPackageAlias(  component.getPackageAlias(camCadDatabase) );
         m_pkgInfo.SetPackageHeight( component.getCompHeight(camCadDatabase), camCadDatabase.getPageUnits() );
         m_pkgInfo.SetDPMO(          atoi(component.getDPMO(camCadDatabase)) );
         m_pkgInfo.SetPartComments(  component.getPartComments(camCadDatabase) );
         m_pkgInfo.SetPackageComments(  component.getPackageComments(camCadDatabase) );

         //*rcf Move to func, will need to be reset when users changes config
         CDataDoctorLibraryAttrib *libattr = NULL;
         CString* attrname;

         for (libraryAttribMap.GetFirstSorted(attrname, libattr); libattr != NULL; libraryAttribMap.GetNextSorted(attrname, libattr))
         {
            if (libattr->IsActive())
            {
               m_pkgInfo.SetGenericAttrib(libattr->GetName(), component.getGenericAttrib(camCadDatabase, libattr->GetName()));
            }
         }
      }
   }
}

bool CDataDoctorPart::isLoaded(CCamCadDatabase& camCadDatabase)
{
   // Return true if at least one component in collection is loaded, otherwise false
   int index;
   CDataDoctorComponent* component;

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      if (component->getLoaded(camCadDatabase))
         return true;
   }

   return false;
}

CDataDoctorComponent* CDataDoctorPart::getHeadComponent()
{
   CDataDoctorComponent* component = NULL;
   CString* refDes;

   m_components.GetFirstSorted(refDes,component);

   return component;
}

int CDataDoctorPart::getGeometryCount(CCamCadDatabase& camCadDatabase)
{
   // Count of different blocks inserted by this part number.
   // Just build a map based on block numbers then see what the count is.
   // The map takes care of handling dups for us.

   CMapWordToPtr map;

   CDataDoctorComponent* ddcomp = NULL;
   CString* refDes;

   for (m_components.GetFirstSorted(refDes, ddcomp); ddcomp != NULL; m_components.GetNextSorted(refDes, ddcomp))
   {
      BlockStruct *blockptr = ddcomp->getGeometryBlock(camCadDatabase);  //*rcf This could be sped up w/ just using blocknumber from insert, we're not really needing the block itself so why search for it?
      //*rcf On the other hand we possibly need the geometries more than once, maybe should cache the collection
      if (blockptr != NULL)
      {
         map.SetAt(blockptr->getBlockNumber(), blockptr);
      }
   }

   return map.GetCount();
}

DataDoctorStatusTag CDataDoctorPart::GetGeometryStatus(CCamCadDatabase& camCadDatabase, CString &msg)
{
   // Count of different blocks inserted by this part number.
   // Just build a map based on block numbers then see what the count is.
   // The map takes care of handling dups for us.

   msg.Empty();

   CMapWordToPtr map; //*rcf cache this in the DDPart, need to clear whenever grid is (re)filled, eg. if PNs change on Part tab, or somethign like that, want to avoid doing it too often

   CDataDoctorComponent* ddcomp = NULL;
   CString* refDes;

   for (m_components.GetFirstSorted(refDes, ddcomp); ddcomp != NULL; m_components.GetNextSorted(refDes, ddcomp))
   {
      BlockStruct *blockptr = ddcomp->getGeometryBlock(camCadDatabase);
      
      if (blockptr != NULL)
      {
         map.SetAt(blockptr->getBlockNumber(), blockptr);
      }
   }

   if (map.GetCount() > 1)
   {
      CString geomNames;

      POSITION pos = map.GetStartPosition();
      while (pos)
      {
         void  *blockptr;
         WORD blocknum;

         map.GetNextAssoc(pos, blocknum, blockptr);
         if (!geomNames.IsEmpty())
            geomNames += ", ";
         geomNames += ((BlockStruct*)blockptr)->getName();
      }

      msg = "Multiple geometries: " + geomNames;
   }

   return dataDoctorStatusOk; //map.GetCount();
}


DataDoctorStatusTag CDataDoctorPart::GetDPMOStatus(CCamCadDatabase& camCadDatabase, CString &msg)
{
   // DPMO status is okay if all insert instances have the
   // same value. It is okay if the value is blank.

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   // A map to keep dups out of message
   CMapStringToPtr badvaluemap;
   void *dontcare = NULL;

   int index;
   CDataDoctorComponent* ddcomp;

   for (m_components.rewind(index);m_components.next(ddcomp,index);)
   {
      CString dpmoStr = ddcomp->getDPMO( camCadDatabase );

      if (this->m_pkgInfo.GetDPMOStr().CompareNoCase(dpmoStr) != 0)
         badvaluemap.SetAt(dpmoStr, dontcare);  // convenience of map weeds out duplicates for us
   }

   if (badvaluemap.GetCount() > 0)
   {
      retval = dataDoctorStatusError;
      CString dpmoSetting( this->m_pkgInfo.GetDPMOStr() );
      if (dpmoSetting.IsEmpty())
         dpmoSetting = "<Blank>";
      msg = "Inconsistent DPMO values: " + dpmoSetting;
      POSITION pos = badvaluemap.GetStartPosition();
      CString badval;
      while (pos != NULL)
      {
         badvaluemap.GetNextAssoc(pos, badval, dontcare);

         if (!msg.IsEmpty())
            msg += ", ";
         msg += badval.IsEmpty() ? "<Blank>" : badval;
      }
   }

   return retval;
}


DataDoctorStatusTag CDataDoctorPart::GetGenericAttribStatus(CCamCadDatabase& camCadDatabase, CString attribName, CString &msg)
{
   // Attrib status is okay if all insert instances have the
   // same value. It is okay if the value is blank.

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   // A map to keep dups out of message
   CMapStringToPtr badvaluemap;
   void *dontcare = NULL;

   int index;
   CDataDoctorComponent* ddcomp;

   for (m_components.rewind(index);m_components.next(ddcomp,index);)
   {
      CString valueStr = ddcomp->getGenericAttrib(camCadDatabase, attribName);

      if (this->m_pkgInfo.GetGenericAttrib(attribName).CompareNoCase(valueStr) != 0)
         badvaluemap.SetAt(valueStr, dontcare);  // convenience of map weeds out duplicates for us
   }

   if (badvaluemap.GetCount() > 0)
   {
      retval = dataDoctorStatusError;
      CString valueSetting( this->m_pkgInfo.GetGenericAttrib(attribName) );
      if (valueSetting.IsEmpty())
         valueSetting = "<Blank>";
      msg = "Inconsistent " + attribName + " values: " + valueSetting;
      POSITION pos = badvaluemap.GetStartPosition();
      CString badval;
      while (pos != NULL)
      {
         badvaluemap.GetNextAssoc(pos, badval, dontcare);

         if (!msg.IsEmpty())
            msg += ", ";
         msg += badval.IsEmpty() ? "<Blank>" : badval;
      }
   }

   return retval;
}


DataDoctorStatusTag CDataDoctorPart::GetGenericAttribListStatus(CCamCadDatabase& camCadDatabase, CString &msg)
{
   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   POSITION pos = this->m_pkgInfo.GetGenericAttribStartPosition();
   while (pos != NULL)
   {
      //*rcf Maybe add a pkginfo func to return CStringArray of attrib names in one shot, we don't need values here
      CString name, value;
      this->m_pkgInfo.GetNextGenericAttribAssoc(pos, name, value);

      // Note, potentially status is okay but there is still a message.
      // I.e. status and message may be independent.

      CString submsg;
      DataDoctorStatusTag subretval = this->GetGenericAttribStatus(camCadDatabase, name, submsg);

      if (subretval != dataDoctorStatusOk)
         retval = subretval;

      if (!submsg.IsEmpty())
      {
         if (!msg.IsEmpty())
            msg += ", ";

         msg += submsg;
      }
   }

   return retval;
}

DataDoctorStatusTag CDataDoctorPart::GetPackageAliasStatus(CCamCadDatabase& camCadDatabase, CString &msg)
{
   // Package Alias status is okay if all insert instances have the
   // same value. It is okay if the value is blank.

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   // A map to keep dups out of message
   CMapStringToPtr badvaluemap;
   void *dontcare = NULL;

   int index;
   CDataDoctorComponent* ddcomp;

   for (m_components.rewind(index);m_components.next(ddcomp,index);)
   {
      CString pkgalias = ddcomp->getPackageAlias( camCadDatabase );

      if (this->m_pkgInfo.GetPackageAlias().CompareNoCase(pkgalias) != 0)
         badvaluemap.SetAt(pkgalias, dontcare);  // convenience of map weeds out duplicates for us
   }

   if (badvaluemap.GetCount() > 0)
   {
      retval = dataDoctorStatusError;
      CString aliasSetting( this->m_pkgInfo.GetPackageAlias() );
      if (aliasSetting.IsEmpty())
         aliasSetting = "<Blank>";
      msg = "Inconsistent Package Alias values: " + aliasSetting;
      POSITION pos = badvaluemap.GetStartPosition();
      CString badval;
      while (pos != NULL)
      {
         badvaluemap.GetNextAssoc(pos, badval, dontcare);

         if (!msg.IsEmpty())
            msg += ", ";
         msg += badval.IsEmpty() ? "<Blank>" : badval;
      }
   }

   return retval;

}

DataDoctorStatusTag CDataDoctorPart::GetOutlineStatus(CCamCadDatabase& camCadDatabase, CString &msg)
{
   // Outline status is okay if the outline exists and the method name
   // in the DD package and in the inserted geometry are the same.
   // (If they are not the same then the user made a method choice
   // that failed when it was applied.)

   msg.Empty();

   DataStruct *outline = this->GetRealPart().GetOutlineData(camCadDatabase);

   if (outline == NULL)
   {
      msg = "No Outline";
   }
   else
   {
      if (outline->getDataType() == dataTypePoly)
      {
         if (outline->getPolyList()->GetCount() != 1)
            msg.Format("Invlaid outline, should contain 1 polyline, but contains %d.", outline->getPolyList()->GetCount());
         else
         {
            CPoly *poly  = outline->getPolyList()->GetHead();
            if (poly == NULL || poly->getPntList().GetCount() < 3)
               msg = "Invalid outline, contains less than 3 points.";
         }
      }
      else
      {
         msg = "Invalid outline, object is not correct data type.";
      }

      CString existingMethod = this->GetRealPart().GetExistingOutlineMethod(camCadDatabase);
      if (existingMethod.CompareNoCase(this->GetOutlineMethod()) != 0)
      {
         if (!msg.IsEmpty())
            msg += " ";
         msg = "Outline Method failure, existing outline does not match selected method.";
      }
   }

   return msg.IsEmpty() ? dataDoctorStatusOk : dataDoctorStatusError;
}

DataDoctorStatusTag CDataDoctorPart::GetPackageStatus(CCamCadDatabase& camCadDatabase, CString &msg)
{
   // Collect individual status messages into one message.
   // Overall status is "ok" only if all sub-statuses are "ok".
   // Messages may still be non-blank, even if status is "ok".

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   CString msg_1;
   if (GetPackageAliasStatus(camCadDatabase, msg_1) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   CString msg_2;
   if (GetOutlineStatus(camCadDatabase, msg_2) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   CString msg_3;
   GetGeometryStatus(camCadDatabase, msg_3); // Info, not necessarily an error

   CString msg_4;
   if (GetDPMOStatus(camCadDatabase, msg_4) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   CString msg_5;
   if (GetGenericAttribListStatus(camCadDatabase, msg_5) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   // If not using this Part Number entry then set status to Ignore but leave messages
   if (GetPackageSource().CompareNoCase(QPartNumber) != 0)
      retval = dataDoctorStatusIgnore;

   msg = msg_1;
   if (!msg.IsEmpty() && !msg_2.IsEmpty())
      msg += "; ";
   msg += msg_2;
   if (!msg.IsEmpty() && !msg_3.IsEmpty())
      msg += "; ";
   msg += msg_3;
   if (!msg.IsEmpty() && !msg_4.IsEmpty())
      msg += "; ";
   msg += msg_4;
   if (!msg.IsEmpty() && !msg_5.IsEmpty())
      msg += "; ";
   msg += msg_5;

   return retval;
}

void CDataDoctorPart::calculateStatus(CCamCadDatabase& camCadDatabase)
{
   m_status = dataDoctorStatusOk;
   m_errors.Empty();

   // check for inconsistant values
   CMapStringToPtr values,deviceTypes,nTolerances,pTolerances,pinCounts,subClasses;

   int index;
   CDataDoctorComponent* component;

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      values.SetAt(component->getNormalizedValue(camCadDatabase),0);
      deviceTypes.SetAt(component->getDeviceType(camCadDatabase),0);
      nTolerances.SetAt(component->getNormalizedNTolerance(camCadDatabase),0);
      pTolerances.SetAt(component->getNormalizedPTolerance(camCadDatabase),0);
      pinCounts.SetAt(component->getPinCountString(),0);
      subClasses.SetAt(component->getSubclass(camCadDatabase),0);
   }

   CString commaDelimiter(", ");
   CString semiDelimiter("; ");
   CString empty("");
   CString inconsistantAttributeList;
   CString key;
   void* value;

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   for (POSITION pos = values.GetStartPosition();pos != NULL && values.GetCount() > 1;)
   {
      values.GetNextAssoc(pos,key,value);

      if (key.IsEmpty())
      {
         key = "<empty>";
      }

      inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Inconsistant values: " + inconsistantAttributeList;
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   inconsistantAttributeList.Empty();

   for (POSITION pos = deviceTypes.GetStartPosition();pos != NULL && deviceTypes.GetCount() > 1;)
   {
      deviceTypes.GetNextAssoc(pos,key,value);

      if (key.IsEmpty())
      {
         key = "<empty>";
      }

      inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Inconsistant device types: " + inconsistantAttributeList;
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   inconsistantAttributeList.Empty();

   for (POSITION pos = deviceTypes.GetStartPosition();pos != NULL;)
   {
      deviceTypes.GetNextAssoc(pos,key,value);  // "key" is Device Type value

      if (!key.IsEmpty())
      {
         DeviceTypeTag rawDevTypeTag = stringToDeviceTypeTag(key);
         if (rawDevTypeTag == deviceTypeUndefined)
         {
            inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
         }
      } 
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Unrecognized device types: " + inconsistantAttributeList;
   }
   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   inconsistantAttributeList.Empty();

   for (POSITION pos = nTolerances.GetStartPosition();pos != NULL && nTolerances.GetCount() > 1;)
   {
      nTolerances.GetNextAssoc(pos,key,value);

      if (key.IsEmpty())
      {
         key = "<empty>";
      }

      inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Inconsistant NTolerances: " + inconsistantAttributeList;
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   inconsistantAttributeList.Empty();

   for (POSITION pos = pTolerances.GetStartPosition();pos != NULL && pTolerances.GetCount() > 1;)
   {
      pTolerances.GetNextAssoc(pos,key,value);

      if (key.IsEmpty())
      {
         key = "<empty>";
      }

      inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Inconsistant PTolerances: " + inconsistantAttributeList;
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   inconsistantAttributeList.Empty();

   for (POSITION pos = pinCounts.GetStartPosition();pos != NULL && pinCounts.GetCount() > 1;)
   {
      pinCounts.GetNextAssoc(pos,key,value);

      if (key.IsEmpty())
      {
         key = "<empty>";
      }

      inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Inconsistant pin counts: " + inconsistantAttributeList;
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   inconsistantAttributeList.Empty();

   for (POSITION pos = subClasses.GetStartPosition();pos != NULL && subClasses.GetCount() > 1;)
   {
      subClasses.GetNextAssoc(pos,key,value);

      if (key.IsEmpty())
      {
         key = "<empty>";
      }

      inconsistantAttributeList += (inconsistantAttributeList.IsEmpty() ? empty : commaDelimiter) + key;
   }

   if (! inconsistantAttributeList.IsEmpty())
   {
      m_status = dataDoctorStatusError;

      m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + 
         "Inconsistant subclasses: " + inconsistantAttributeList;
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
   CString retval;
	component = getHeadComponent();
   if (component != NULL)
   {
      component->calculateStatus(camCadDatabase);
		if (!component->getErrors().IsEmpty())
		{
			m_status = dataDoctorStatusError;

         // If the only error is about unrecognized device type, then we would have already
         // added a message for that, don't add another. We check this by looking for the
         // message text and a semiDelimiter. If no semiDelimiter then it is the only msg.

         CString compErr = component->getErrors();
         if (compErr.Find("Unrecognized device type") > -1 && compErr.Find(semiDelimiter) < 0)
         {
            // skip
         }
         else
         {
			   m_errors += (m_errors.IsEmpty() ? empty : semiDelimiter) + component->getErrors();
         }
		}
   }


}

CString CDataDoctorPart::getPartNumber()
{
   return m_partNumber;
}

CString CDataDoctorPart::getStatusString()
{
   CString retval;

   switch (m_status)
   {
   case dataDoctorStatusOk:         retval = "OK";       break;
   case dataDoctorStatusError:      retval = "Errors";   break;
   case dataDoctorStatusIgnore:     retval = "Ignored";  break;
   case dataDoctorStatusUndefined:  retval = "?";        break;
   }

   return retval;
}

CString CDataDoctorPart::getDeviceType(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getDeviceType(camCadDatabase);
   }

   return retval;
}

CString CDataDoctorPart::getDisplayValue(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getDisplayValue(camCadDatabase);
   }

   return retval;
}

CString CDataDoctorPart::getNormalizedPTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getNormalizedPTolerance(camCadDatabase);
   }

   return retval;
}

CString CDataDoctorPart::getNormalizedNTolerance(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getNormalizedNTolerance(camCadDatabase);
   }

   return retval;
}

int CDataDoctorPart::getPinCount()
{
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      return component->getPinCount();
   }

   return 0;
}

CString CDataDoctorPart::getPinCountString()
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getPinCountString();
   }

   return retval;
}

CString CDataDoctorPart::getPinRefsToPinNamesMapString(const CString delimiter, bool allPins)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getPinRefsToPinNamesMapString(delimiter,allPins);
   }

   return retval;
}

CString CDataDoctorPart::getPinNameOptionsString(const CString delimiter)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getPinNameOptionsString(delimiter);
   }

   return retval;
}

CString CDataDoctorPart::getSubclass(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getSubclass(camCadDatabase);
   }

   return retval;
}

DeviceTypeTag CDataDoctorPart::getDeviceTypeTag()
{
   DeviceTypeTag retval = deviceTypeUndefined;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getDeviceTypeTag();
   }

   return retval;
}

CString CDataDoctorPart::getReferenceDesignators()
{
   CString referenceDesignators;

   int index;
   CDataDoctorComponent* component;

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      if (!referenceDesignators.IsEmpty())
      {
         referenceDesignators += ",";
      }

      referenceDesignators += component->getRefDes();
   }

   return referenceDesignators;
}

CString CDataDoctorPart::getLoadedReferenceDesignators(CCamCadDatabase& camCadDatabase)
{
   CString referenceDesignators;

   int index;
   CDataDoctorComponent* component;

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      if (component->getLoaded(camCadDatabase))
      {
         if (!referenceDesignators.IsEmpty())
         {
            referenceDesignators += ",";
         }

         referenceDesignators += component->getRefDes();
      }
   }

   return referenceDesignators;
}

CString CDataDoctorPart::getErrors()
{
   return m_errors;
}

CString CDataDoctorPart::getDescription(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getDescription(camCadDatabase);
   }

   return retval;
}

CString CDataDoctorPart::getGenericAttrib(CCamCadDatabase& camCadDatabase, CString& attribName)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getGenericAttrib(camCadDatabase, attribName);
   }

   return retval;
}

CString CDataDoctorPart::getPinMapStatus()
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getPinMapStatus();
   }

   return retval;
}

CString CDataDoctorPart::getPartComments(CCamCadDatabase& camCadDatabase)
{
   CString retval;
   CDataDoctorComponent* component = getHeadComponent();

   if (component != NULL)
   {
      retval = component->getPartComments(camCadDatabase);
   }

   return retval;
}

DataDoctorLibraryTag CDataDoctorPart::getFromLibrary()
{
	return m_fromLibrary;
}

CString CDataDoctorPart::getFromLibraryString()
{
	CString fromLibraryString;
	switch (m_fromLibrary)
	{
	case dataDoctorLibraryExist:
		fromLibraryString = QFromLibraryExist;
		break;
	case dataDoctorLibraryLoaded:
		fromLibraryString = QFromLibraryLoaded;
		break;
	default:
		break;
	}

	return fromLibraryString;
}


void CDataDoctorPart::setPartNumber(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_partNumber = value;

   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setPartNumber(camCadDatabase,value);
   }
}

void CDataDoctorPart::setPackageAlias(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetPackageAlias(value);
   
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setPackageAlias(camCadDatabase,value);
   }
}

void CDataDoctorPart::setDPMO(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetDPMO(value);
   
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setDPMO(camCadDatabase,value);
   }
}

void CDataDoctorPart::setPackageSource(CCamCadDatabase& camCadDatabase, EDBNameType value)
{
   setPackageSource(camCadDatabase, value == dbnamePartNumber ? QPartNumber : QGeometry );
}

void CDataDoctorPart::setPackageSource(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetPackageSource(value);
   
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setPackageSource(camCadDatabase,value);
   }
}

void CDataDoctorPart::setOutlineMethod(CCamCadDatabase& camCadDatabase, const CString& value, bool filled, bool apply)
{
   this->m_pkgInfo.SetOutlineMethod(value);
   this->m_pkgInfo.SetOutlineFilled(filled);

   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setOutlineMethod(camCadDatabase,value);
   }

   int dftOutlineResult = 0;  // non-zero values are errors

   if (apply)
   {
      //*rcf Uh Oh -- FLAW! In overall structure, multiple geomes per PN implies we ought to have
      // multiple RPs per PN too, for applicaiton of Algorithm setting!
      // PUNT for now...
      CCEtoODBDoc *doc = &camCadDatabase.getCamCadDoc();
      BlockStruct *compBlk = this->getHeadComponent()->getGeometryBlock(camCadDatabase);
      BlockStruct *rpPkgBlk = this->GetRealPart().GetPkgBlock(camCadDatabase);

      dftOutlineResult = GeneratePackageOutlineGeometry(doc, compBlk, rpPkgBlk, stringToOutlineAlgTag(this->m_pkgInfo.GetOutlineMethod()), false /*m_chkRectangleOutline*/, filled, true /*m_chkOverwriteOutline*/);
   
      // Adjust package insert to pin center
      CDataDoctorComponent *sampleComp = this->getHeadComponent();
      if (sampleComp != NULL && this->GetRealPart().GetBlock() != NULL)
      {
         CPoint2d pinsCenter = sampleComp->GetPinsCentroid( camCadDatabase );
         DataStruct *pkgInsertData = this->GetRealPart().GetBlock()->GetPackage();
         if (pkgInsertData != NULL && pkgInsertData->getDataType() == dataTypeInsert)
            pkgInsertData->getInsert()->setOrigin(pinsCenter);
      }
   }

   //return dftOutlineResult == 0 ? true : false;
}

void CDataDoctorPart::setPackageHeight(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetPackageHeight(value, camCadDatabase.getPageUnits());

   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setCompHeight(camCadDatabase,value); // "package height" stored as "compheight" on individual inserts
   }
}

void CDataDoctorPart::setFamily(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetFamily(value);

   DataStruct *pkgdata = this->m_realPart.GetPkgInsertData(camCadDatabase);
   if(pkgdata != NULL)
      pkgdata->getDefinedAttributes()->setAttribute(camCadDatabase.getCamCadData(), m_realPart.getFamilyKeywordIndex(camCadDatabase), value, attributeUpdateOverwrite);
}

void CDataDoctorPart::addFamilyFromAttribute(CCamCadDatabase& camCadDatabase)
{
   CAttribute *attrib = NULL;
   DataStruct *pkgdata = this->m_realPart.GetPkgInsertData(camCadDatabase);
   if(pkgdata != NULL && pkgdata->lookUpAttrib(m_realPart.getFamilyKeywordIndex(camCadDatabase), attrib))
      this->m_pkgInfo.SetFamily(attrib->getStringValue());
}

void CDataDoctorPart::setPackageComments(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetPackageComments(value);
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);
      component->setPackageComments(camCadDatabase,value); 
   }
}

void CDataDoctorPart::setPartComments(CCamCadDatabase& camCadDatabase,const CString& value)
{
   this->m_pkgInfo.SetPartComments(value);
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);
      component->setPartComments(camCadDatabase,value); 
   }
}

void CDataDoctorPart::setGenericAttrib(CCamCadDatabase& camCadDatabase, const CString& name, const CString& value)
{
   this->m_pkgInfo.SetGenericAttrib(name, value);

   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setGenericAttrib(camCadDatabase, name, value);
   }
}

void CDataDoctorPart::LoadDBAttribs(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& configuredAttribMap, CDBGenericAttributeMap *attrDataMap, bool overwrite)
{
   if (attrDataMap != NULL)
   {
      POSITION pos = attrDataMap->GetStartPosition();
      while (pos != NULL)
      {
         CString ignoredKey;
         CDBNameValuePair *nvp;
         attrDataMap->GetNextAssoc(pos, ignoredKey, nvp);

         CDataDoctorLibraryAttrib *libattr = NULL;
         if (configuredAttribMap.Lookup(nvp->GetName(), libattr))
            if (libattr->IsActive() && (overwrite || this->getGenericAttrib(camCadDatabase, nvp->GetName()).IsEmpty()))
               this->setGenericAttrib(camCadDatabase, nvp->GetName(), nvp->GetValue());
      }
   }
}

void CDataDoctorPart::setValue(CCamCadDatabase& camCadDatabase,const CString& value)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setValue(camCadDatabase,value);
   }
}

void CDataDoctorPart::setPTolerance(CCamCadDatabase& camCadDatabase,const CString& value)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setPTolerance(camCadDatabase,value);
   }
}

void CDataDoctorPart::setNTolerance(CCamCadDatabase& camCadDatabase,const CString& value)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setNTolerance(camCadDatabase,value);
   }
}

void CDataDoctorPart::setPinRefToPinNamesMapString(CCamCadDatabase& camCadDatabase,const CString& value, CString &errMsg)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setPinRefToPinNamesMapString(camCadDatabase,value, errMsg);
   }
}

void CDataDoctorPart::resetPinRefToPinNameMapAttrib(CCamCadDatabase& camCadDatabase)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->resetPinRefToPinNameMapAttrib(camCadDatabase);
   }
}

void CDataDoctorPart::setSubclass(CCamCadDatabase& camCadDatabase,const CString& value)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setSubclass(camCadDatabase,value);
   }
}

void CDataDoctorPart::setDeviceType(CCamCadDatabase& camCadDatabase,const CString& value)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setDeviceType(camCadDatabase,value);
   }
}

void CDataDoctorPart::setDescription(CCamCadDatabase& camCadDatabase,const CString& value)
{
   CDataDoctorComponent* component;
   CString refDes;

   for (POSITION pos = m_components.GetStartPosition();pos != NULL;)
   {
      m_components.GetNextAssoc(pos,refDes,component);

      component->setDescription(camCadDatabase,value);
   }
}

void CDataDoctorPart::resetFromLibrary()
{
	m_fromLibrary = dataDoctorLibraryNo;
}

int CDataDoctorPart::adoptDBPartValues(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, CDBPart *dbpart, bool overwriteFlag)
{
   int retval = 0;

   if (dbpart != NULL)
   {
      if (overwriteFlag || getDeviceType(camCadDatabase).IsEmpty())
      {
         setDeviceType(camCadDatabase, dbpart->getDeviceType());
         retval++;
      }

      if (overwriteFlag || getDisplayValue(camCadDatabase).IsEmpty())
      {
         setValue(camCadDatabase, dbpart->getValue());
         retval++;
      }

      if (overwriteFlag || getNormalizedPTolerance(camCadDatabase).IsEmpty())
      {
         setPTolerance(camCadDatabase, dbpart->getPTol());
         retval++;
      }

      if (overwriteFlag || getNormalizedNTolerance(camCadDatabase).IsEmpty())
      {
         setNTolerance(camCadDatabase, dbpart->getNTol());
         retval++;
      }

      if (overwriteFlag || getSubclass(camCadDatabase).IsEmpty())
      {
         setSubclass(camCadDatabase, dbpart->getSubclass());
         retval++;
      }

      if (overwriteFlag || getPinMapStatus().IsEmpty() || getPinMapStatus().CompareNoCase("UNMAP") == 0)
      {
         CString errMsg;
         setPinRefToPinNamesMapString(camCadDatabase, dbpart->getPinmap(), errMsg);
         retval++;

         if (!errMsg.IsEmpty())
            ErrorMessage(errMsg, "Database Error");
      }

		if (overwriteFlag || getDescription(camCadDatabase).IsEmpty())
      {
         setDescription(camCadDatabase, dbpart->getDescription());
         retval++;
      }

      if (dbpart->GetAttribMap() != NULL)
      {
         this->LoadDBAttribs(camCadDatabase, activeAttribMap, dbpart->GetAttribMap(), overwriteFlag);
      }

		if (overwriteFlag || getPartComments(camCadDatabase).IsEmpty())
      {
         setPartComments(camCadDatabase, dbpart->getComment());
         retval++;
      }

		if (overwriteFlag)
			m_fromLibrary = dataDoctorLibraryLoaded;
		else
			m_fromLibrary = dataDoctorLibraryExist;
	}
	else
	{
		m_fromLibrary = dataDoctorLibraryNo;
	}

   return retval;
}


bool CDataDoctorPart::LoadPackageInfoFromDB(CCamCadDatabase& camCadDatabase, bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap)
{
   if (!getPartNumber().IsEmpty())
   {
      CDBInterface db;
      if (db.Connect())
      {
         CDBPackageInfo *dbpkginfo = db.LookupPackageInfo(getPartNumber(), dbnamePartNumber);
         if (dbpkginfo != NULL)
         {
            this->setPackageSource( camCadDatabase, dbpkginfo->getPackageSource() );
            this->setPackageAlias( camCadDatabase, dbpkginfo->getPackageAlias() );
            this->setPackageHeight( camCadDatabase, dbpkginfo->getPackageHeightStr(camCadDatabase.getPageUnits()) );
            this->setOutlineMethod( camCadDatabase, dbpkginfo->getOutlineMethod(), outlineFillFlag, true);
            this->setDPMO( camCadDatabase, dbpkginfo->getDPMO() );
            this->LoadDBAttribs( camCadDatabase, configuredAttribMap, dbpkginfo->GetAttribMap(), true );
            this->setFamily( camCadDatabase, dbpkginfo->getFamily() );
            delete dbpkginfo;

            if (this->GetOutlineMethodTag() == algCustom)
            {
               CDBOutlineVertexMap *dboutline = db.LookupOutline( getPartNumber(), dbnamePartNumber );
               if (dboutline != NULL)
               {
                  if (dboutline->GetCount() > 2)  // 3 pts required for closed shape
                  {
                     this->GetRealPart().AdoptDBOutline(camCadDatabase, dboutline, outlineFillFlag, this->getHeadComponent());
                  }
                  else
                  {
                     int jj = 0; //*rcf Error
                  }

                  delete dboutline;
               }
            }

            return true;
         }
      }
   }

   return false;
}

bool CDataDoctorRealPart::SaveToDB(CDBInterface &db, CCamCadDatabase &camCadDatabase, CString name, EDBNameType nametype)
{
   if (!name.IsEmpty() && db.IsConnected())
   {
      CDBOutlineVertexMap *outlineMap = this->AllocDBOutline(camCadDatabase, name, nametype);
      DataStruct *outlineData = this->GetOutlineData(camCadDatabase);
      if (outlineData != NULL)
      {
         if (outlineMap->GetCount() > 0)  //*rcf probably should require three?
         {
            db.SaveOutlineVertices(name, nametype, outlineMap);
         }
      }
      delete outlineMap;

      return true;
   }

   return false;
}

void CDataDoctorPart::SavePackageInfoToDB(CCamCadDatabase& camCadDatabase)
{
   if (!getPartNumber().IsEmpty())
   {
      CDBInterface db;
      if (db.Connect())
      {
         CDBPackageInfo dbpkginfo( getPartNumber(), dbnamePartNumber, GetPackageSource(), GetPackageAlias(), GetPackageHeight(pageUnitsMilliMeters), GetOutlineMethod(), GetDPMO(), 
            GetFamily(), "", "", GetPackageComments(), AllocDBAttribMap() );
         db.SavePackageInfo(dbpkginfo);

         if (this->GetOutlineMethodTag() == algCustom)
         {
            CDataDoctorRealPart& ddrp = this->GetRealPart();
            ddrp.SaveToDB(db, camCadDatabase, this->getPartNumber(), dbnamePartNumber);
         }
      }
   }
}

//_____________________________________________________________________________
CDataDoctorParts::CDataDoctorParts(bool isContainer,int size)
   : CTypedMapSortStringToObContainer<CDataDoctorPart>(size,isContainer)
{
}

void CDataDoctorParts::sync(CCamCadDatabase& camCadDatabase,CDataDoctorComponents& components, CDataDoctorLibraryAttribMap& libraryAttribMap)
{
   // We empty the exmaple component list and then refill it.
   // IF we end up with any CDataDoctorPart in list that does not have an example
   // component, then it is a part number that has been deleted altogether since
   // last sync, and we want to drop it else it will make an empty row in
   // the Parts table.
   // We tried just emptying the entire Parts list and rebuilding from scratch,
   // but there is data in the CDataDoctorPart that cannot be recovered from the
   // individual items. IN particuler the "EXISTS"/"FROM LIBRARY" flag is in the
   // CDataDoctorPart, but has no representation in the individual components.
   // We don't want to lose this or anything else. So after refilling the 
   // example components list, we weed out the parts with no example comps.

   // Get rid of current list of components
   resetComponents();

   // Refill components list
   int index;
   CDataDoctorComponent* component;
   for (components.rewind(index);components.next(component,index);)
   {
      addComponent(camCadDatabase,*component, libraryAttribMap);
   }


   // Remove obsolete part number items from list.
   // If a part has no example components, then it represents a part number that no longer exists.
   CDataDoctorPart *part = NULL;
   CString partnumber;
   for (POSITION pos = GetStartPosition();pos != NULL;)
   {    
      GetNextAssoc(pos, partnumber, part);

      if (part->getHeadComponent() == NULL)
      {
         this->RemoveKey(partnumber);
         // start over, removing this item moves next item into this position.
         // if we continue from where we are then we'd skip an item, no?
         pos = this->GetStartPosition();
      }
   }
}

void CDataDoctorParts::resetComponents()
{
   CDataDoctorPart *part = NULL;
   CString thispartnumber;

   for (POSITION pos = GetStartPosition();pos != NULL;)
   {    
      GetNextAssoc(pos, thispartnumber, part);

      if (part != NULL)
         part->resetComponents();
   }
}

void CDataDoctorParts::addComponent(CCamCadDatabase& camCadDatabase,CDataDoctorComponent& component, CDataDoctorLibraryAttribMap& libraryAttribMap)
{
   CString partNumber = component.getPartNumber(camCadDatabase);
   bool isDiePart = component.IsDie();
   CDataDoctorPart* part = NULL;
   Lookup(partNumber,part);

   if (part == NULL)
   {
      part = new CDataDoctorPart(partNumber, isDiePart);
      SetAt(partNumber,part);
   }

   part->addComponent(camCadDatabase, component, libraryAttribMap);
}

CDataDoctorPart *CDataDoctorParts::findPart(CString partnumber)
{
   CDataDoctorPart *part = NULL;
   CString thispartnumber;

   for (POSITION pos = GetStartPosition();pos != NULL;)
   {    
      GetNextAssoc(pos, thispartnumber, part);

      if (thispartnumber.CompareNoCase(partnumber) == 0)
      {
         return part;
      }
   }

   return NULL;
}

CDataDoctorPart *CDataDoctorParts::FindPartUsingGeomBlk(int compGeomBlockNum)
{
   CDataDoctorPart *part = NULL;
   CString thispartnumber;

   for (POSITION pos = GetStartPosition();pos != NULL;)
   {    
      GetNextAssoc(pos, thispartnumber, part);

      if (part->getHeadComponent()->getGeometryBlockNum() == compGeomBlockNum)
      {
         return part;
      }
   }

   return NULL;
}

bool CDataDoctorParts::storeOnePartToLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorPart *part)
{
   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      return storeOnePartToLibrary(camCadDatabase, part, db);
   }

   return false;
}

bool CDataDoctorParts::storeOnePartToLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorPart *part, CDBInterface& db)
{
   bool saved = false;

   if (!part->getPartNumber().IsEmpty() && db.IsConnected())
   {
      
#if defined(EnablePartLibrary)
      CString partnumber( part->getPartNumber() );
      CString devicetype( part->getDeviceType(camCadDatabase) );
      CString value( part->getDisplayValue(camCadDatabase) );
      CString ptol( part->getNormalizedPTolerance(camCadDatabase) );
      CString ntol( part->getNormalizedNTolerance(camCadDatabase) );
      CString subclass( part->getSubclass(camCadDatabase) );
		CString pinmap( part->getPinRefsToPinNamesMapString("|",false) );
		CString desc( part->getDescription(camCadDatabase) );
      CString comment(part->getPartComments(camCadDatabase));

      CDBPart dbpart(0, partnumber, devicetype, value, ptol, ntol, subclass, pinmap, desc, "", "", comment, part->AllocDBAttribMap());
      if (db.SavePart(dbpart))
            saved = true;
#endif
      if (!saved)
         ErrorMessage("Save part to library failed.");
   }

   return saved;
}

void CDataDoctorParts::storePartDataToLibrary(CCamCadDatabase& camCadDatabase, bool storeAllParts)
{
#if defined(EnablePartLibrary)
   CDataDoctorPart* part;
   CString partNumber;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      int attempts = 0;
      int successes = 0;
      for (POSITION pos = GetStartPosition();pos != NULL;)
      {
         GetNextAssoc(pos,partNumber, part);

         if (storeAllParts)
         {
            attempts++;
            if( storeOnePartToLibrary(camCadDatabase, part, db) )
               successes++;
         }
         else
         {
            CDBPart *existingPart;
            if ((existingPart = db.LookupPart(part->getPartNumber())) != NULL)
            {
               // Already exists, do not save
               delete existingPart;
            }
            else
            {
               // Part did not exist in lib, continue with save
               attempts++;
               if (storeOnePartToLibrary(camCadDatabase, part, db))
                  successes++;
            }
         }
      }

      db.ReportDBSaveResult(attempts, successes);
   }

#endif
}

bool CDataDoctorParts::loadOnePartFromLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, CDataDoctorPart *part, bool overwriteFlag, bool displayMessage)
{
   if (part != NULL)
   {
      CDBInterface db;
      CString dbname( db.GetLibraryDatabaseName() );

      if (db.Connect(dbname, "", ""))
      {
         return loadOnePartFromLibrary(camCadDatabase, activeAttribMap, part, overwriteFlag, displayMessage, db);
      }
   }

	return false;
}

bool CDataDoctorParts::loadOnePartFromLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, CDataDoctorPart *part, bool overwriteFlag, bool displayMessage, CDBInterface& db)
{
   if (part != NULL)
   {
      part->resetFromLibrary();

      CDBPart *dbpart = db.LookupPart(part->getPartNumber());

      if (dbpart != NULL)
      {
         part->adoptDBPartValues(camCadDatabase, activeAttribMap, dbpart, overwriteFlag);

         delete dbpart;

         return true;
      }
   }

	return false;
}

bool CDataDoctorParts::loadPartDataFromLibrary(CCamCadDatabase& camCadDatabase, CDataDoctorLibraryAttribMap& activeAttribMap, bool overwriteFlag, bool displayMessage)
{
   int count = 0;

   CDBInterface db;
   CString dbname( db.GetLibraryDatabaseName() );

   if (db.Connect(dbname, "", ""))
   {
      for (POSITION pos = GetStartPosition();pos != NULL;)
      {
         CString partNumber;    
         CDataDoctorPart* part;
         GetNextAssoc(pos, partNumber, part);

         if (loadOnePartFromLibrary(camCadDatabase, activeAttribMap, part, overwriteFlag, displayMessage, db))
            count++;
      }
   }

   if (displayMessage)
   {
      CString msg;
      msg.Format("Load All Parts from Library found %d part%s.", count, count == 1 ? "" : "s");
      ErrorMessage(msg,"", MB_ICONINFORMATION | MB_OK);
   }

   return true;
}


void CDataDoctorParts::storePackageDataToLibrary(CCamCadDatabase& camCadDatabase, bool storeAll)
{
   CDBInterface db;
   if (db.Connect())
   {
      int attemptCount = 0;
      int successCount = 0;

      POSITION pos = this->GetStartPosition();
      while (pos != NULL)
      {
         CString partnumber;
         CDataDoctorPart *ddpart;
         this->GetNextAssoc(pos, partnumber, ddpart);
         if (ddpart != NULL) //*rcf BUG check package source
         {
            bool saveThisOne = true;
            if (!storeAll)  // !storeAll == store only new ones, those not already in DB
            {
               // Don't use backward compatible lookup here.
               // We want to know specifically if it is in newest style Package Info table.
               CDBPackageInfo *dbpkginfo = db.LookupPackageInfo(ddpart->getPartNumber(), dbnamePartNumber, false);
               if (dbpkginfo != NULL)
               {
                  saveThisOne = false;  // overwrite is false and this one already exists
                  delete dbpkginfo;
               }
            }
            if (saveThisOne)
            {
               attemptCount++;
               CDBPackageInfo pkg( ddpart->getPartNumber(), dbnamePartNumber, ddpart->GetPackageSource(), ddpart->GetPackageAlias(), ddpart->GetPackageHeight(pageUnitsMilliMeters), ddpart->GetOutlineMethod(), ddpart->GetDPMO(), 
                  ddpart->GetFamily(), "", "", ddpart->GetPackageComments(), ddpart->AllocDBAttribMap() );
               if (db.SavePackageInfo(pkg))
                  successCount++;

               if (ddpart->GetOutlineMethodTag() == algCustom)
               {
                  CDataDoctorRealPart& ddrp = ddpart->GetRealPart();
                  ddrp.SaveToDB(db, camCadDatabase, ddpart->getPartNumber(), dbnamePartNumber);
               }
            }
         }
      }

      db.ReportDBSaveResult(attemptCount, successCount);
   }
}

bool CDataDoctorParts::loadPackageDataFromLibrary(CCamCadDatabase& camCadDatabase, bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap, bool apply, bool displayMessage)
{
   CDBInterface db;
   if (db.Connect())
   {
      POSITION pos = this->GetStartPosition();
      while (pos != NULL)
      {
         CString partnumber;
         CDataDoctorPart *ddpart;
         this->GetNextAssoc(pos, partnumber, ddpart);
         if (ddpart != NULL) //*rcf BUG check package source
         {
            CDBPackageInfo *dbpkginfo = db.LookupPackageInfo( ddpart->getPartNumber(), dbnamePartNumber );
            if (dbpkginfo != NULL)
            {
               ddpart->setPackageSource(camCadDatabase, dbpkginfo->getPackageSource());
               ddpart->setPackageAlias(camCadDatabase, dbpkginfo->getPackageAlias());
               ddpart->setPackageHeight(camCadDatabase, dbpkginfo->getPackageHeightStr(camCadDatabase.getPageUnits()));
               ddpart->setOutlineMethod(camCadDatabase,  dbpkginfo->getOutlineMethod(),  outlineFillFlag, apply );
               ddpart->setDPMO(camCadDatabase, dbpkginfo->getDPMO() );
               ddpart->LoadDBAttribs(camCadDatabase, configuredAttribMap, dbpkginfo->GetAttribMap(), apply );
               ddpart->setPackageComments(camCadDatabase, dbpkginfo->getComment());
               ddpart->setFamily( camCadDatabase, dbpkginfo->getFamily() );

               delete dbpkginfo;

               if (ddpart->GetOutlineMethodTag() == algCustom)
               {
                  CDBOutlineVertexMap *dboutline = db.LookupOutline( ddpart->getPartNumber(), dbnamePartNumber );
                  if (dboutline != NULL)
                  {
                     if (dboutline->GetCount() > 2)  // 3 pts required for closed shape
                     {
                        ddpart->GetRealPart().AdoptDBOutline(camCadDatabase, dboutline, outlineFillFlag, ddpart->getHeadComponent());
                     }
                     else
                     {
                        int jj = 0; //*rcf Error
                     }

                     delete dboutline;
                  }
               }
            }
         }
      }
      return true;
   }

	return false;
}

int CDataDoctorParts::GetDiePartCount()
{
   int DiePartCount = 0;
   for(POSITION partPos = GetStartPosition(); partPos;)
   {
      CString partnumber;
      CDataDoctorPart *ddpart;
      GetNextAssoc(partPos, partnumber, ddpart);
      if(ddpart && ddpart->IsDiePart())
         DiePartCount ++;
   }

   return DiePartCount;
}

//_____________________________________________________________________________

CDataDoctorPackage::CDataDoctorPackage(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& ddcomponent, CDataDoctorLibraryAttribMap& libraryAttribMap, bool filled)
: m_realPart("GEOM")
{
   m_exampleDDComponent = &ddcomponent;
   m_ccdb = &camCadDatabase;

   m_insertedGeomBlk = ddcomponent.getGeometryBlock(camCadDatabase);

   // Set the geometry default package values.
   // If not present on the geometry then upgrade the geometry with attributes set from the component inserts.
   if (m_insertedGeomBlk != NULL)
   {
      this->m_geometryName = m_insertedGeomBlk->getName();
      this->m_realPart.SetName(m_geometryName);
      this->m_realPart.GenerateSelf(camCadDatabase, ddcomponent, filled);

      Attrib *attrib;

      if (m_insertedGeomBlk->lookUpAttrib(m_exampleDDComponent->getDPMOKeywordIndex(*m_ccdb), attrib))
         this->m_pkgInfo.SetDPMO( attrib->getIntValue() );
      else
      {
         this->m_pkgInfo.SetDPMO( m_exampleDDComponent->getDPMO(*m_ccdb) );
         m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getDPMOKeywordIndex(*m_ccdb), m_pkgInfo.GetDPMO(), attributeUpdateOverwrite);
      }

      if (m_insertedGeomBlk->lookUpAttrib(m_exampleDDComponent->getPackageAliasKeywordIndex(*m_ccdb), attrib))
         this->m_pkgInfo.SetPackageAlias( attrib->getStringValue() );
      else
      {
         this->m_pkgInfo.SetPackageAlias( m_exampleDDComponent->getPackageAlias(*m_ccdb) );
         m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getPackageAliasKeywordIndex(*m_ccdb), m_pkgInfo.GetPackageAlias(), attributeUpdateOverwrite);
      }

      if (m_insertedGeomBlk->lookUpAttrib(m_exampleDDComponent->getOutlineMethodKeywordIndex(*m_ccdb), attrib))
         this->m_pkgInfo.SetOutlineMethod( attrib->getStringValue() );
      else
      {
         this->m_pkgInfo.SetOutlineMethod( this->GetExistingOutlineMethod() );
         m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getOutlineMethodKeywordIndex(*m_ccdb), m_pkgInfo.GetOutlineMethod(), attributeUpdateOverwrite);
      }

      //*rcf Get fill mode
      
      if (m_insertedGeomBlk->lookUpAttrib(m_exampleDDComponent->getCompHeightKeywordIndex(*m_ccdb), attrib))
         this->m_pkgInfo.SetPackageHeight( attrib->getStringValue(), m_ccdb->getPageUnits() );
      else
      {
         this->m_pkgInfo.SetPackageHeight( m_exampleDDComponent->getCompHeight(*m_ccdb), m_ccdb->getPageUnits() );
         m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getCompHeightKeywordIndex(*m_ccdb), m_pkgInfo.GetPackageHeight(m_ccdb->getPageUnits()), attributeUpdateOverwrite);
      }

      // Set Package Comments
      if (m_exampleDDComponent->getComponent()->lookUpAttrib(m_exampleDDComponent->getPackageCommentKeywordIndex(*m_ccdb), attrib))
         this->m_pkgInfo.SetPackageComments( attrib->getStringValue());
      else
         this->m_pkgInfo.SetPackageComments( m_exampleDDComponent->getPackageComments(*m_ccdb) );

      // User configured attribs
      CDataDoctorLibraryAttrib *libattr = NULL;
      CString* attrname;

      for (libraryAttribMap.GetFirstSorted(attrname, libattr); libattr != NULL; libraryAttribMap.GetNextSorted(attrname, libattr))
      {
         if (libattr->IsActive())
         {
            if (m_insertedGeomBlk->lookUpAttrib(m_exampleDDComponent->getGenericAttribKeywordIndex(*m_ccdb, libattr->GetName()), attrib))
               this->m_pkgInfo.SetGenericAttrib(attrib->getName(), attrib->getStringValue());
            else
            {
               this->m_pkgInfo.SetGenericAttrib(libattr->GetName(), m_exampleDDComponent->getGenericAttrib(*m_ccdb, libattr->GetName()));
               m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getGenericAttribKeywordIndex(*m_ccdb, libattr->GetName()), m_pkgInfo.GetGenericAttrib(libattr->GetName()), attributeUpdateOverwrite);
            }
         }
      }

      //Set Family
      AddFamilyFromAttribute();

      this->AddInsert(&ddcomponent);
   }
}
/*
BlockStruct *CDataDoctorPackage::GetInsertedGeomBlock()
{
   if (m_insertedGeomBlk == NULL && m_ccdb != NULL && m_exampleDDComponent != NULL)
   {
      m_insertedGeomBlk = m_exampleDDComponent->getGeometryBlock(*m_ccdb);
   }

   return m_insertedGeomBlk;
}
*/

CString CDataDoctorPackage::GetRefNameList()
{
   // Return comma separate list of refnames for all inserts of this geometry, 
   // regardless of setting for Package Source

   CString refnames;

   for (int j = 0; j < this->m_allInsertsOfThisPkg.GetCount(); j++)
   {
      CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.GetAt(j);
      CString refname = ddcomp->getRefDes();
      if (j > 0)
         refnames += ", ";
      refnames += refname;
   }

   return refnames;
}

void CDataDoctorPackage::SetPackageAlias(CString alias, bool apply)
{
   // apply == true, set value in cc data as well as this ddpackage.
   //       == false, set only in this ddpackage

   this->m_pkgInfo.SetPackageAlias( alias );

   if (m_insertedGeomBlk != NULL)
      m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getPackageAliasKeywordIndex(*m_ccdb), alias, attributeUpdateOverwrite);

   // Note that the "example part" is also included in the list, so no
   // need to handle it separately.

   if (apply && m_ccdb != NULL)
   {
      for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
      {
         CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
         //*rcf possible performance improvement to cache pkgInfo in DDComponent, to avoid attrib lookup.
         //*rcf Or maybe lookup PN in other grid, not sure if that is better than looking up an attrib.
         //*rcf Possibly maintain a MAP of PNs and maybe of Geom names too, for quicker cross ref between the grids.
         //*rcf But here we are in teh package, and we're going heavily into dependance on the grid and grid page if we shy from the attribs.
         if (ddcomp->getPackageSource(*m_ccdb).CompareNoCase(QPartNumber) != 0) //*rcf do better than string compare, make enum of bool func in pkg info
            ddcomp->setPackageAlias(*m_ccdb, alias);
      }
   }
}

void CDataDoctorPackage::SetDPMO(CString dpmo, bool apply)
{
   // apply == true, set value in cc data as well as this ddpackage.
   //       == false, set only in this ddpackage

   this->m_pkgInfo.SetDPMO( dpmo );

   if (m_insertedGeomBlk != NULL)
      m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getDPMOKeywordIndex(*m_ccdb), atoi(dpmo), attributeUpdateOverwrite);

   // Note that the "example part" is also included in the list, so no
   // need to handle it separately.

   if (apply && m_ccdb != NULL)
   {
      for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
      {
         CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);

         if (ddcomp->getPackageSource(*m_ccdb).CompareNoCase(QPartNumber) != 0) //*rcf do better than string compare, make enum of bool func in pkg info
            ddcomp->setDPMO(*m_ccdb, dpmo);
      }
   }
}

void CDataDoctorPackage::SetPackageHeight(CString height, PageUnitsTag fromUnits, bool apply)
{
   double h = atof(height); 
   CString hpageunits; 
   if (h > 0.0)  // leave string empty if h is zero or less
   {
      h *= getUnitsFactor(fromUnits, m_ccdb->getPageUnits());
      hpageunits.Format("%.*f", GetDecimals(m_ccdb->getPageUnits()), h);
   }
   
   SetPackageHeight(hpageunits, apply);
}

void CDataDoctorPackage::SetPackageHeight(CString height, bool apply)
{
   // apply == true, set value in cc data as well as this ddpackage.
   //       == false, set only in this ddpackage

   this->m_pkgInfo.SetPackageHeight(height, this->m_ccdb->getPageUnits());

   if (m_insertedGeomBlk != NULL)
      m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getCompHeightKeywordIndex(*m_ccdb), height, attributeUpdateOverwrite);


   // Note that the "example part" is also included in the list, so no
   // need to handle it separately.

   if (apply && m_ccdb != NULL)
   {
      for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
      {
         CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
         if (ddcomp->getPackageSource(*m_ccdb).CompareNoCase(QPartNumber) != 0)
            ddcomp->setCompHeight(*m_ccdb, height);
      }
   }
}

void CDataDoctorPackage::AddFamilyFromAttribute()
{
   Attrib *attrib = NULL;
   DataStruct *pkgdata = this->m_realPart.GetPkgInsertData(*m_ccdb);
   if(pkgdata != NULL && pkgdata->lookUpAttrib(m_realPart.getFamilyKeywordIndex(*m_ccdb), attrib))
      m_pkgInfo.SetFamily( attrib->getStringValue());
}

void CDataDoctorPackage::SetFamily(CString familyStr)
{
   this->m_pkgInfo.SetFamily( familyStr );
   DataStruct *pkgdata = this->m_realPart.GetPkgInsertData(*m_ccdb);
   if(pkgdata != NULL)
      pkgdata->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_realPart.getFamilyKeywordIndex(*m_ccdb), familyStr, attributeUpdateOverwrite);
}

void CDataDoctorPackage::SetPackageComments(CString commentStr)
{
   this->m_pkgInfo.SetPackageComments( commentStr );

   if (m_ccdb != NULL)
   {
      for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
      {
         CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
         if (ddcomp->getPackageSource(*m_ccdb).CompareNoCase(QPartNumber) != 0)
            ddcomp->setPackageComments(*m_ccdb, commentStr);
      }
   }
}

void CDataDoctorPackage::SetGenericAttrib(CString name, CString value, bool apply)
{
   // apply == true, set value in cc data as well as this ddpackage.
   //       == false, set only in this ddpackage

   this->m_pkgInfo.SetGenericAttrib(name, value);

   if (m_insertedGeomBlk != NULL)
      m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getGenericAttribKeywordIndex(*m_ccdb, name), value, attributeUpdateOverwrite);


   // Note that the "example part" is also included in the list, so no
   // need to handle it separately.

   if (apply && m_ccdb != NULL)
   {
      for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
      {
         CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
         if (ddcomp->getPackageSource(*m_ccdb).CompareNoCase(QPartNumber) != 0)
            ddcomp->setGenericAttrib(*m_ccdb, name, value);
      }
   }
}


void CDataDoctorPackage::LoadDBAttribs(CDataDoctorLibraryAttribMap& configuredAttribMap, CDBGenericAttributeMap *attrMap, bool apply)
{
   if (attrMap != NULL)
   {
      POSITION pos = attrMap->GetStartPosition();
      while (pos != NULL)
      {
         CString ignoredKey;
         CDBNameValuePair *nvp;
         attrMap->GetNextAssoc(pos, ignoredKey, nvp);

         CDataDoctorLibraryAttrib *libattr = NULL;
         if (configuredAttribMap.Lookup(nvp->GetName(), libattr))
            if (libattr->IsActive())
               this->SetGenericAttrib(nvp->GetName(), nvp->GetValue(), apply);
      }
   }
}


bool CDataDoctorPackage::SetOutlineMethod(CString val, bool filled, bool apply)
{
   // apply == true, set value in cc data as well as this ddpackage.
   //       == false, set only in this ddpackage

   // Creating an outline by a particular method may fail.
   // If apply is true, then return true if outline creation succeeds, otherwise false.
   // If apply is false, always return true;

   this->m_pkgInfo.SetOutlineMethod( val );
   this->m_pkgInfo.SetOutlineFilled( filled ); //*rcf Add this as attribute set/get too, also in DDPart

   if (m_insertedGeomBlk != NULL)
      m_insertedGeomBlk->getDefinedAttributes()->setAttribute(m_ccdb->getCamCadData(), m_exampleDDComponent->getOutlineMethodKeywordIndex(*m_ccdb), val, attributeUpdateOverwrite);


   int dftOutlineResult = 0;  // non-zero values are errors

   if (apply)
   {
      CCEtoODBDoc *doc = &this->m_ccdb->getCamCadDoc();
      BlockStruct *compBlk = this->m_exampleDDComponent->getGeometryBlock(*this->m_ccdb);
      BlockStruct *rpPkgBlk = this->GetRealPart().GetPkgBlock(*this->m_ccdb);

      dftOutlineResult = GeneratePackageOutlineGeometry(doc, compBlk, rpPkgBlk, stringToOutlineAlgTag(this->m_pkgInfo.GetOutlineMethod()), false /*m_chkRectangleOutline*/, filled, true /*m_chkOverwriteOutline*/);
   
      // Adjust package insert to pin center
      CDataDoctorComponent *sampleComp = this->m_exampleDDComponent;
      CPoint2d pinsCenter = sampleComp->GetPinsCentroid( *this->m_ccdb );
      DataStruct *pkgInsertData = this->GetRealPart().GetBlock()->GetPackage();
      if (pkgInsertData != NULL && pkgInsertData->getDataType() == dataTypeInsert)
            pkgInsertData->getInsert()->setOrigin(pinsCenter); 
   }

   return dftOutlineResult == 0 ? true : false;
}

DataStruct *CDataDoctorPackage::GetOldStyleDFTOutlineData()
{
   if (m_ccdb != NULL && m_exampleDDComponent != NULL)
   {
      return m_exampleDDComponent->GetOldStyleDFTOutlineData(*m_ccdb);
   }

   return NULL;
}

CString CDataDoctorPackage::GetExistingOutlineMethod()
{
   // Observe that blank return means there is no outline.
   // "Unknown" return means there is an outline but we don't know the method that created it.

   if (this->GetRealPart().HasOutlineData(*m_ccdb))  // We want blank instead of "Unknown" if there is no outline
   {
      return this->GetRealPart().GetExistingOutlineMethod(*m_ccdb);
   }

   return "";
}

DataDoctorStatusTag CDataDoctorPackage::GetPackageAliasStatus(CString &msg)
{
   // Package Alias status is okay if all insert instances have the
   // same value. It is okay if the value is blank.

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   // A map to keep dups out of message
   CMapStringToPtr badvaluemap;
   void *dontcare = NULL;

   for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
   {
      CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
      CString pkgalias = ddcomp->getPackageAlias( *m_ccdb );

      if (this->m_pkgInfo.GetPackageAlias().CompareNoCase(pkgalias) != 0)
         badvaluemap.SetAt(pkgalias, dontcare);  // convenience of map weeds out duplicates for us
   }

   if (badvaluemap.GetCount() > 0)
   {
      retval = dataDoctorStatusError;
      CString aliasSetting( this->m_pkgInfo.GetPackageAlias() );
      if (aliasSetting.IsEmpty())
         aliasSetting = "<Blank>";
      msg = "Inconsistent Package Alias values: " + aliasSetting;
      POSITION pos = badvaluemap.GetStartPosition();
      CString badval;
      while (pos != NULL)
      {
         badvaluemap.GetNextAssoc(pos, badval, dontcare);

         if (!msg.IsEmpty())
            msg += ", ";
         msg += badval.IsEmpty() ? "<Blank>" : badval;
      }
   }

   return retval;

}

DataDoctorStatusTag CDataDoctorPackage::GetDPMOStatus(CString &msg)
{
   // DPMO status is okay if all insert instances have the
   // same value. It is okay if the value is blank.

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   // A map to keep dups out of message
   CMapStringToPtr badvaluemap;
   void *dontcare = NULL;

   for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
   {
      CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
      CString dpmo = ddcomp->getDPMO( *m_ccdb );

      if (this->m_pkgInfo.GetDPMOStr().CompareNoCase(dpmo) != 0)
         badvaluemap.SetAt(dpmo, dontcare);  // convenience of map weeds out duplicates for us
   }

   if (badvaluemap.GetCount() > 0)
   {
      retval = dataDoctorStatusError;
      CString dpmoSetting( this->m_pkgInfo.GetDPMOStr() );
      if (dpmoSetting.IsEmpty())
         dpmoSetting = "<Blank>";
      msg = "Inconsistent DPMO values: " + dpmoSetting;
      POSITION pos = badvaluemap.GetStartPosition();
      CString badval;
      while (pos != NULL)
      {
         badvaluemap.GetNextAssoc(pos, badval, dontcare);

         if (!msg.IsEmpty())
            msg += ", ";
         msg += badval.IsEmpty() ? "<Blank>" : badval;
      }
   }

   return retval;

}

DataDoctorStatusTag CDataDoctorPackage::GetGenericAttribStatus(CString attribName, CString &msg)
{
   // Attrib status is okay if all insert instances have the
   // same value. It is okay if the value is blank.

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   // A map to keep dups out of message
   CMapStringToPtr badvaluemap;
   void *dontcare = NULL;

   for (int i = 0; i < this->m_allInsertsOfThisPkg.GetCount(); i++)
   {
      CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.ElementAt(i);
      CString value = ddcomp->getGenericAttrib( *m_ccdb, attribName );

      if (this->m_pkgInfo.GetGenericAttrib(attribName).CompareNoCase(value) != 0)
         badvaluemap.SetAt(value, dontcare);  // convenience of map weeds out duplicates for us
   }

   if (badvaluemap.GetCount() > 0)
   {
      retval = dataDoctorStatusError;
      CString valueSetting( this->m_pkgInfo.GetGenericAttrib(attribName) );
      if (valueSetting.IsEmpty())
         valueSetting = "<Blank>";
      msg = "Inconsistent " + attribName + " values: " + valueSetting;
      POSITION pos = badvaluemap.GetStartPosition();
      CString badval;
      while (pos != NULL)
      {
         badvaluemap.GetNextAssoc(pos, badval, dontcare);

         if (!msg.IsEmpty())
            msg += ", ";
         msg += badval.IsEmpty() ? "<Blank>" : badval;
      }
   }

   return retval;

}

DataDoctorStatusTag CDataDoctorPackage::GetGenericAttribListStatus(CString &msg)
{
   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   POSITION pos = this->m_pkgInfo.GetGenericAttribStartPosition();
   while (pos != NULL)
   {
      //*rcf Maybe add a pkginfo func to return CStringArray of attrib names in one shot, we don't need values here
      CString name, value;
      this->m_pkgInfo.GetNextGenericAttribAssoc(pos, name, value);

      // Note, potentially status is okay but there is still a message.
      // I.e. status and message are independent.

      CString submsg;
      DataDoctorStatusTag subretval = this->GetGenericAttribStatus(name, submsg);

      if (subretval != dataDoctorStatusOk)
         retval = subretval;

      if (!submsg.IsEmpty())
      {
         if (!msg.IsEmpty())
            msg += ", ";

         msg += submsg;
      }
   }

   return retval;
}

DataDoctorStatusTag CDataDoctorPackage::GetOutlineStatus(CString &msg)
{
   // Outline status is okay if the outline exists and the method name
   // in the DD package and in the inserted geometry are the same.
   // (If they are not the same then the user made a method choice
   // that failed when it was applied.)

   msg.Empty();

   DataStruct *outline = this->GetRealPart().GetOutlineData(*m_ccdb);

   if (outline == NULL)
   {
      msg = "No Outline";
   }
   else
   {
      if (outline->getDataType() == dataTypePoly)
      {
         if (outline->getPolyList()->GetCount() != 1)
            msg.Format("Invlaid outline, should contain 1 polyline, but contains %d.", outline->getPolyList()->GetCount());
         else
         {
            CPoly *poly  = outline->getPolyList()->GetHead();
            if (poly == NULL || poly->getPntList().GetCount() < 3)
               msg = "Invalid outline, contains less than 3 points.";
         }
      }
      else
      {
         msg = "Invalid outline, object is not correct data type.";
      }

      CString existingMethod = this->GetExistingOutlineMethod();
      if (existingMethod.CompareNoCase(this->GetOutlineMethod()) != 0)
      {
         if (!msg.IsEmpty())
            msg += " ";
         msg = "Outline Method failure, existing outline does not match selected method.";
      }
   }

   return msg.IsEmpty() ? dataDoctorStatusOk : dataDoctorStatusError;
}

DataDoctorStatusTag CDataDoctorPackage::GetStatus(CString &msg)
{
   // Collect individual status messages into one message.
   // Overall status is "ok" only if all sub-statuses are "ok".
   // Messages may still be non-blank, even if status is "ok".

   msg.Empty();
   DataDoctorStatusTag retval = dataDoctorStatusOk;

   CString msg_1;
   if (GetPackageAliasStatus(msg_1) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   CString msg_2;
   if (GetOutlineStatus(msg_2) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   CString msg_3;
   if (GetDPMOStatus(msg_3) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   CString msg_4;
   if (GetGenericAttribListStatus(msg_4) == dataDoctorStatusError)
      retval = dataDoctorStatusError;

   msg = msg_1;
   if (!msg.IsEmpty() && !msg_2.IsEmpty())
      msg += "; ";
   msg += msg_2;
   if (!msg.IsEmpty() && !msg_3.IsEmpty())
      msg += "; ";
   msg += msg_3;
   if (!msg.IsEmpty() && !msg_4.IsEmpty())
      msg += "; ";
   msg += msg_4;

   return retval;
}

bool CDataDoctorPackage::LoadFromDB(bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap)
{
   if (!GetGeometryName().IsEmpty())
   {
      CDBInterface db;
      if (db.Connect())
      {
         CDBPackageInfo *dbpkginfo = db.LookupPackageInfo(GetGeometryName(), dbnameGeometry);
         if (dbpkginfo != NULL)
         {
            this->SetPackageAlias( dbpkginfo->getPackageAlias(), true);
            this->SetPackageHeight( dbpkginfo->getPackageHeightStr(m_ccdb->getPageUnits()), true );
            this->SetOutlineMethod( dbpkginfo->getOutlineMethod(), outlineFillFlag, true);
            this->LoadDBAttribs( configuredAttribMap, dbpkginfo->GetAttribMap(), true );
            delete dbpkginfo;

            if (this->GetOutlineMethodTag() == algCustom && m_ccdb != NULL)
            {
               CDBOutlineVertexMap *dboutline = db.LookupOutline( GetGeometryName(), dbnameGeometry );
               if (dboutline != NULL)
               {
                  if (dboutline->GetCount() > 2)  // 3 pts required for closed shape
                  {
                     this->GetRealPart().AdoptDBOutline(*m_ccdb, dboutline, outlineFillFlag, this->GetExampleComponent());
                  }
                  else
                  {
                     int jj = 0; //*rcf Error
                  }

                  delete dboutline;
               }
            }
            return true;
         }
      }
   }

   return false;
}

void CDataDoctorPackage::SaveToDB()
{
   if (!GetGeometryName().IsEmpty())
   {
      CDBInterface db;
      if (db.Connect())
      {
         CDBPackageInfo dbpkginfo( GetGeometryName(), dbnameGeometry, dbnameGeometry, GetPackageAlias(), GetPackageHeight(pageUnitsMilliMeters), GetOutlineMethod(), GetDPMO(), 
            GetFamily(), "", "", GetPackageComments(), AllocDBAttribMap() );
         db.SavePackageInfo(dbpkginfo);

         if (this->GetOutlineMethodTag() == algCustom && m_ccdb != NULL)
         {
            CDataDoctorRealPart& ddrp = GetRealPart();
            ddrp.SaveToDB(db, *m_ccdb, GetGeometryName(), dbnameGeometry);
         }
      }
   }
}

//----------------------------------------------------------------------------------------

CDataDoctorPackages::CDataDoctorPackages(bool isContainer,int size)
   : CTypedMapSortStringToObContainer<CDataDoctorPackage>(size,isContainer)
{
}

void CDataDoctorPackages::AddPkg(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& ddcomponent, CDataDoctorLibraryAttribMap& libraryAttribMap, bool filled)
{

   BlockStruct *insertedBlk = ddcomponent.getGeometryBlock(camCadDatabase); 
   if (insertedBlk != NULL)
   {
      CString geomAkaPkgName = insertedBlk->getName();

      CDataDoctorPackage *pkg = this->FindPkg( geomAkaPkgName );
      if (pkg != NULL)
      {
         pkg->AddInsert(&ddcomponent);
      }
      else
      {
         pkg = new CDataDoctorPackage(camCadDatabase, ddcomponent, libraryAttribMap, filled);
         this->SetAt(geomAkaPkgName, pkg);
      }
   }
}

CDataDoctorPackage *CDataDoctorPackages::FindPkg(CString pkgname)
{
   // pkgname aka geometry name

   if (!pkgname.IsEmpty())
   {
      CDataDoctorPackage *pkg;
      if (Lookup(pkgname, pkg))
         return pkg;
   }

   return NULL;
}

void CDataDoctorPackages::GatherPackages(CCamCadDatabase& camCadDatabase, CDataDoctorComponents& ddcomponents, CDataDoctorLibraryAttribMap& libraryAttribMap, bool filled)
{
   this->RemoveAll();

   int index;
   CDataDoctorComponent* component;

   for (ddcomponents.rewind(index); ddcomponents.next(component,index); )
   {
      AddPkg(camCadDatabase,*component, libraryAttribMap, filled);
   }
}

void CDataDoctorPackage::PurgeOldStyleDFTPackageOutline()
{
   DataStruct *obsoleteInsertData = this->GetOldStyleDFTOutlineData();

   if (obsoleteInsertData != NULL)
   {
      CDataDoctorComponent *ddcomp = this->GetExampleComponent();
      m_ccdb->getCamCadDoc().removeDataFromDataList(*ddcomp->getGeometryBlock(*m_ccdb), obsoleteInsertData);
   }
}

void CDataDoctorPackages::PurgeOldStyleDFTPackageOutlines()
{
   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CString name;
      CDataDoctorPackage *ddpkg;
      this->GetNextAssoc(pos, name, ddpkg);

      ddpkg->PurgeOldStyleDFTPackageOutline();
   }
}

CDataDoctorPackage *CDataDoctorPackages::FindPkgUsingGeomBlk(int compGeomBlockNum)
{
   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CString name;
      CDataDoctorPackage *ddpkg;
      this->GetNextAssoc(pos, name, ddpkg);

      if (ddpkg->GetGeomBlockNum() == compGeomBlockNum)
      {
         return ddpkg;
      }
   }

   return NULL;
}

void CDataDoctorPackages::storePackageDataToLibrary(CCamCadDatabase& camCadDatabase, bool storeAll)
{
   CDBInterface db;
   if (db.Connect())
   {
      int attemptCount = 0;
      int successCount = 0;

      POSITION pos = this->GetStartPosition();
      while (pos != NULL)
      {
         CString name;
         CDataDoctorPackage *ddpkg;
         this->GetNextAssoc(pos, name, ddpkg);
         if (ddpkg != NULL)
         {
            bool saveThisOne = true;
            if (!storeAll)  // !storeAll == store only new ones, those not already in DB
            {
               // Don't use backward compatible lookup here.
               // We want to know specifically if it is in newest style Package Info table.
               CDBPackageInfo *dbpkginfo = db.LookupPackageInfo(ddpkg->GetGeometryName(), dbnameGeometry, false);
               if (dbpkginfo != NULL)
               {
                  saveThisOne = false;  // overwrite is false and this one already exists
                  delete dbpkginfo;
               }
            }
            if (saveThisOne)
            {
               attemptCount++;
               CDBPackageInfo pkg( ddpkg->GetGeometryName(), dbnameGeometry, dbnameGeometry, ddpkg->GetPackageAlias(), ddpkg->GetPackageHeight(pageUnitsMilliMeters), ddpkg->GetOutlineMethod(), ddpkg->GetDPMO(), 
                  ddpkg->GetFamily(), "", "", ddpkg->GetPackageComments(), ddpkg->AllocDBAttribMap() );
               if (db.SavePackageInfo(pkg))
                  successCount++;

               if (ddpkg->GetOutlineMethodTag() == algCustom)
               {
                  CDataDoctorRealPart& ddrp = ddpkg->GetRealPart();
                  ddrp.SaveToDB(db, camCadDatabase, ddpkg->GetGeometryName(), dbnameGeometry);
               }
            }
         }
      }

      db.ReportDBSaveResult(attemptCount, successCount);
   }
}

//*rcf this probably belongs closer to other realpart funcs
CDBOutlineVertexMap *CDataDoctorRealPart::AllocDBOutline(CCamCadDatabase& camCadDatabase, CString name, EDBNameType nametype)
{
   CDBOutlineVertexMap *outlineMap = new CDBOutlineVertexMap;

   //CDataDoctorRealPart& ddrp = ddpkg->GetRealPart();
   DataStruct *outlineData = this->GetOutlineData(camCadDatabase);
   if (outlineData != NULL)
   {
      int overallPointNum = 0;
      int polynum = 0;
      DataTypeTag outlineDataType = outlineData->getDataType();

      
      POSITION polylistPos = outlineData->getPolyList()->GetHeadPosition();
      while (polylistPos != NULL)
      {
         ++polynum;
         CPoly *poly = outlineData->getPolyList()->GetNext(polylistPos);
         if (poly != NULL)
         {
            int pointnum = 0;
            POSITION vertexPos = poly->getHeadVertexPosition();
            while (vertexPos != NULL)
            {
               CPnt *point = poly->getNextVertex(vertexPos);
               if (point != NULL)
               {
                  ++pointnum; 
                  ++overallPointNum;

                  double xxx = point->x;
                  double yyy = point->y;
                  double bulge = point->bulge;

                  CDBOutlineVertex *dbvertex = new CDBOutlineVertex(name, nametype,
                     polynum, pointnum, xxx, yyy, bulge, camCadDatabase.getPageUnits());

                  CString key;
                  key.Format("%d", overallPointNum);
                  outlineMap->SetAt(key, dbvertex);
               }
            }
         }
      }
   }

   return outlineMap;
}

void CDataDoctorRealPart::AdoptDBOutline(CCamCadDatabase& camCadDatabase, CDBOutlineVertexMap *vertexMap, bool outlineFillFlag, CDataDoctorComponent *sampleComp)
{
   if (vertexMap != NULL)
   {
      // Out with the old
      this->RemoveOutlineData(camCadDatabase);

      // In with the new

      PageUnitsTag toPageUnits = camCadDatabase.getPageUnits();

      //BlockStruct *pkgBlk = this->GetBlock();
      BlockStruct *pkgGeomBlk = this->GetPkgBlock(camCadDatabase);

      // Align
      if (sampleComp != NULL) 
      {
         CPoint2d pinsCenter = sampleComp->GetPinsCentroid( camCadDatabase );
         DataStruct *pkgInsertData = this->GetBlock()->GetPackage();
         if (pkgInsertData != NULL && pkgInsertData->getDataType() == dataTypeInsert)
         {         
#define ALIGN_BY_PIN_CENTER
#ifdef  ALIGN_BY_PIN_CENTER
            pkgInsertData->getInsert()->setOrigin(pinsCenter);
#else
            // Simply align same as comp
            pkgInsertData->getInsert()->setOrigin(0.,0.);// sampleComp->getOrigin() );
#endif
         }
      }

      Graph_Block_On(pkgGeomBlk);

      int packageOutlineTopIndex = camCadDatabase.getDefinedLayerIndex(RP_LAYER_PKG_BODY_TOP,    false);

      DataStruct *outline = Graph_PolyStruct(packageOutlineTopIndex, 0, FALSE);

      outline->setLayerIndex(packageOutlineTopIndex);
      outline->setGraphicClass(graphicClassPackageBody);

      // All outlines from DB are considered custom, set the attribute.
      //*rcf Alternatively we could set this attrib after doing the Adopt..()
      int outlineMethodKW = camCadDatabase.getCamCadDoc().RegisterKeyWord(ATT_OUTLINE_METHOD, 0, valueTypeString);
      CString algCustomStr = outlineAlgTagToString(algCustom);
      camCadDatabase.getCamCadDoc().SetAttrib(&outline->getDefinedAttributes(), outlineMethodKW, valueTypeString, algCustomStr.GetBuffer(0), attributeUpdateOverwrite, NULL);

      vertexMap->setSortFunction(CDBOutlineVertexMap::VertexOrderSortFunc);
      vertexMap->Sort();

      CDBOutlineVertex *vertex = NULL;
      CString* key;

      int curPolyNum = 0;

      for (vertexMap->GetFirstSorted(key, vertex); vertex != NULL; vertexMap->GetNextSorted(key, vertex))
      {
         CString name = vertex->getName();
         EDBNameType ntype = vertex->getNameType();
         int polyNum = vertex->getPolyNum();
         int pointNum = vertex->getPointNum();
         double x = vertex->getX(toPageUnits);
         double y = vertex->getY(toPageUnits);
         double bulge = vertex->getBulge();

         if (polyNum != curPolyNum)
         {
            Graph_Poly(NULL, 0, outlineFillFlag, FALSE, TRUE);
            curPolyNum = polyNum;
         }

         Graph_Vertex(x, y, bulge);
      }

      //?pkgBlk->getDataList().empty();
      //?pkgBlk->getDataList().AddHead(outline);

      Graph_Block_Off();

   }
}


bool CDataDoctorPackages::loadPackageDataFromLibrary(CCamCadDatabase& camCadDatabase, bool outlineFillFlag, CDataDoctorLibraryAttribMap& configuredAttribMap, bool apply, bool displayMessage)
{
   CDBInterface db;
   if (db.Connect())
   {
      POSITION pos = this->GetStartPosition();
      while (pos != NULL)
      {
         CString name;
         CDataDoctorPackage *ddpkg;
         this->GetNextAssoc(pos, name, ddpkg);
         if (ddpkg != NULL)
         {
            CDBPackageInfo *dbpkginfo = db.LookupPackageInfo( ddpkg->GetGeometryName(), dbnameGeometry );
            if (dbpkginfo != NULL)
            {
               ddpkg->SetPackageAlias(   dbpkginfo->getPackageAlias(),   apply );
               ddpkg->SetPackageHeight(  dbpkginfo->getPackageHeight(),  pageUnitsMilliMeters, apply );
               ddpkg->SetOutlineMethod(  dbpkginfo->getOutlineMethod(),  outlineFillFlag, apply );
               ddpkg->SetDPMO(           dbpkginfo->getDPMO(),           apply);
               ddpkg->LoadDBAttribs(     configuredAttribMap, dbpkginfo->GetAttribMap(),      apply );
               ddpkg->SetFamily(dbpkginfo->getFamily());
               ddpkg->SetPackageComments(dbpkginfo->getComment());
               delete dbpkginfo;
            }

            if (ddpkg->GetOutlineMethodTag() == algCustom)
            {
               CDBOutlineVertexMap *dboutline = db.LookupOutline( ddpkg->GetGeometryName(), dbnameGeometry );
               if (dboutline != NULL)
               {
                  if (dboutline->GetCount() > 2)  // 3 pts required for closed shape
                  {
                     ddpkg->GetRealPart().AdoptDBOutline(camCadDatabase, dboutline, outlineFillFlag, ddpkg->GetExampleComponent());
                     //*rcf currently setting the Custom method attrib in Adopt..(), not sure I like that.
                     // maybe want 2nd call to SetOutlineMethod() here, or maybe add as arg in Adopt call (hey!)
                  }
                  else
                  {
                     int jj = 0; //*rcf Error
                  }

                  delete dboutline;
               }
            }


         }
      }
      return true;
   }

	return false;
}

//_____________________________________________________________________________
CDataDoctor::CDataDoctor(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_components(true,nextPrime2n(2000))
, m_partLibraryAttribMap("Part")
, m_pkgLibraryAttribMap("Package")
{
   m_pcbBlock = NULL;
   CDataDoctorComponent::resetKeywordIndices();

   //camCadDoc.clearSelected(false, false);
}

bool CDataDoctor::initializeData()
{
   bool retval = false;
   CNullWriteFormat errorLog;
   getCamCadDatabase().registerHiddenKeyWord(QOriginalValueKeyword,0,valueTypeString,errorLog);

   CDeviceTypes::reset();

   GetPartLibraryAttribMap().LoadSettings();
   GetPartLibraryAttribMap().LoadAttribNamesFromDB();

   GetPackageLibraryAttribMap().LoadSettings();
   GetPackageLibraryAttribMap().LoadAttribNamesFromDB();

   while (true)
   {
      FileStruct* pcbFile = getCamCadDatabase().getSingleVisiblePcb();

      if (pcbFile == NULL)
      {
         formatMessageBox(MB_ICONERROR,"One and only one board must be visible; Data Doctor session terminated");
         break;
      }

      m_components.empty();
      m_pcbBlock = pcbFile->getBlock();

      m_camCadDoc.PrepareAddEntity(pcbFile);

      CInsertTypeMask insertTypeMask(insertTypePcbComponent, insertTypeDie);
      for (CDataListIterator pcbComponentIterator(*m_pcbBlock,insertTypeMask);pcbComponentIterator.hasNext();)
      {
         DataStruct* pcbComponent = pcbComponentIterator.getNext();

         CDataDoctorComponent* component = new CDataDoctorComponent(getCamCadDatabase(),*pcbComponent);
         m_components.SetAt(component->getSortableRefDes(),component);

         // silently normalize all component VALUE attributes
         {
            component->saveOriginalValue(getCamCadDatabase());
            CString normalizedValue = component->getNormalizedValue(getCamCadDatabase());

            if (! normalizedValue.IsEmpty())
            {
               component->setValue(getCamCadDatabase(),normalizedValue);
            }
         }

         // put all attribute names in attrib map
         if (pcbComponent->getAttributesRef() != NULL)
         {
            POSITION attpos = pcbComponent->getAttributesRef()->GetStartPosition();
            while (attpos != NULL)
            {
               WORD key;
               CAttribute *value;
               pcbComponent->getAttributesRef()->GetNextAssoc(attpos, key, value);
               CString name = value->getName();
               CString nameval = value->getNameValue();
               LPCSTR attribnamekey = NULL;

               // For Parts tab
               if (!GetPartLibraryAttribMap().LookupKey(name, attribnamekey) && 
                  !GetPartLibraryAttribMap().IsProhibited(name))
               {
                  GetPartLibraryAttribMap().SetAt(name, new CDataDoctorLibraryAttrib(name));
               }

               // For Packages tab
               if (!GetPackageLibraryAttribMap().LookupKey(name, attribnamekey) && 
                  !GetPackageLibraryAttribMap().IsProhibited(name))
               {
                  GetPackageLibraryAttribMap().SetAt(name, new CDataDoctorLibraryAttrib(name));
               }
            }
         }
      }

      for (POSITION netPos = pcbFile->getHeadNetPosition();netPos != NULL;)
      {
         NetStruct* net = pcbFile->getNextNet(netPos);

         for (POSITION pinPos = net->getHeadCompPinPosition();pinPos != NULL;)
         {
            CompPinStruct* compPin = net->getNextCompPin(pinPos);
            CDataDoctorComponent* component;

            if (m_components.Lookup(InsertStruct::getSortableRefDes(compPin->getRefDes()),component))
            {
               CDataDoctorPin* pin = new CDataDoctorPin(*compPin,*net);
               component->addPin(*pin);
            }
         }
      }

      retval = true;

      break;
   }

	m_components.createPinMaps(getCamCadDatabase());

   getSubclassList().LoadFromTables(m_camCadDoc.getCamCadData().getTableList());

   return retval;
}

bool CDataDoctor::doModal(DdPageTag initialPage)
{
   bool retval = initializeData();

   if (retval)
   {
      CDataDoctorPropertySheet dataDoctorPropertySheet(*this);

      if (initialPage != DdUnknownPage)
         dataDoctorPropertySheet.SetInitialPage(initialPage);

      dataDoctorPropertySheet.DoModal();
   }

   return retval;
}

CString CDataDoctor::getNextNoConnectNetName()
{
   static int noConnectNetNameCount = 0;

   CString netName;
   netName.Format("_NC%05d_",noConnectNetNameCount++);

   return netName;
}

void CDataDoctor::identifyMergedNets(CMapStringToString &mergedNetMap, bool ignoreUnloadedComponents)
{
   // Nets that have jumpers across them are considered merged.
   // Note that jumpers may string along, i.e. a jumped to b, b jumped to c.
   // Collect the whole thing as one merged net.
   // Built a map that maps original net names to merged net names.

   int index;
   CDataDoctorComponent* component;
 
   mergedNetMap.RemoveAll();

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      DeviceTypeTag componentDeviceType = component->getDeviceTypeTag();
      bool loadedFlag  = (component->getLoaded(m_camCadDatabase) || !ignoreUnloadedComponents);

      if (componentDeviceType == deviceTypeJumper && loadedFlag)
      {
         CDataDoctorPin *pin0,*pin1;
         CString pinRef0,pinRef1;
         POSITION pos = component->getPinStartPosition();

         component->getPinNextAssoc(pos,pinRef0,pin0);
         component->getPinNextAssoc(pos,pinRef1,pin1);

         CString net0 = pin0->getNet().getNetName();
         CString net1 = pin1->getNet().getNetName();

         CStringArray netNamesToMerge;
         netNamesToMerge.Add(net0);
         netNamesToMerge.Add(net1);

         // Maps each net name to the merged net name.
         // If one or the other is already mapped, coalesce the whole thing.
         CString mergedNet0, mergedNet1;
         BOOL net0alreadyMapped = mergedNetMap.Lookup(net0, mergedNet0);
         BOOL net1alreadyMapped = mergedNetMap.Lookup(net1, mergedNet1);

         // If nets in hand are already mapped then get merged net names and
         // take them apart and collect the original pieces.

         if (net0alreadyMapped)
         {
            CSupString combinedNetNames(mergedNet0);
            CStringArray separatedNetNames;
            combinedNetNames.ParseQuote(separatedNetNames, ",", true);
            for (int i = 0; i < separatedNetNames.GetCount(); i++)
            {
               CString name = separatedNetNames.GetAt(i);
               netNamesToMerge.Add(name);
            }
         }

         if (net1alreadyMapped)
         {
            CSupString combinedNetNames(mergedNet1);
            CStringArray separatedNetNames;
            combinedNetNames.ParseQuote(separatedNetNames, ",", true);
            for (int i = 0; i < separatedNetNames.GetCount(); i++)
            {
               CString name = separatedNetNames.GetAt(i);
               netNamesToMerge.Add(name);
            }
         }

         // Combine the names back together, remove duplicates from list
         // to prevent redundant map update later.

         CString mergedNet;
         for (int i = 0; i < netNamesToMerge.GetCount(); i++)
         {
            CString name = netNamesToMerge.GetAt(i);
            if (mergedNet.Find(name) == -1)
            {
               if (!mergedNet.IsEmpty())
                  mergedNet += ",";

               mergedNet += '"' + name + '"';
            }
            else
            {
               netNamesToMerge.SetAt(i, ""); // duplicate, removed from list
            }
         }

         // Update the map
         for (int i = 0; i < netNamesToMerge.GetCount(); i++)
         {
            CString originalNetName = netNamesToMerge.GetAt(i);
            if (!originalNetName.IsEmpty())
            {
               mergedNetMap.SetAt(originalNetName, mergedNet);
            }
         }
      }
   }
}

bool CDataDoctor::performParallelAnalysis(bool analyzeResistors, bool analyzeCapacitors, bool analyzeInductors,
   bool ignoreUnloadedComponents, bool mergeValues)
{
   // The GUI allows one to select a set of device types to analyze, but the (original) implementation
   // fails to analyze net paris when there is a mix of dev types on same net pairs. So the analysis
   // is broken into separate analyze steps for each device type. The results coalesce.
   // Note also that the mergeValues parameter was not used before, still isn't.

   removeParallelAnalysisResults();

   identifyMergedNets(m_mergedNetMap, ignoreUnloadedComponents);

   if (analyzeResistors)
      performParallelAnalysis(deviceTypeResistor, ignoreUnloadedComponents);

   if (analyzeCapacitors)
      performParallelAnalysis(deviceTypeCapacitor, ignoreUnloadedComponents);

   if (analyzeInductors)
      performParallelAnalysis(deviceTypeInductor, ignoreUnloadedComponents);

   // Although the above "perform" calls allegedly return a value, casual inspection reveals it
   // is always true, so we are not bothering to try to capture their returns.
   return true;

}

bool CDataDoctor::performParallelAnalysis(DeviceTypeTag targetDeviceType, bool ignoreUnloadedComponents)
{
   bool retval = true;

   int index;
   CDataDoctorComponent* component;
   CTypedMapStringToPtrContainer<CDataDoctorComponentList*> parallelDeviceMap;

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      DeviceTypeTag componentDeviceType = component->getPassiveDeviceType();

      bool loadedFlag  = (component->getLoaded(m_camCadDatabase) || !ignoreUnloadedComponents);
      bool pinPairFlag = ((component->getPinCount() - component->getNoConnectPinCount(m_camCadDatabase)) == 2); // only care about "real" pins
      bool analyzeFlag = (componentDeviceType == targetDeviceType) && pinPairFlag && loadedFlag;

      if (analyzeFlag)
      {
         CDataDoctorPin *pin0 = NULL,*pin1 = NULL;
         CString pinRef0,pinRef1;
         CString netPair;
         POSITION pos = component->getPinStartPosition();

         // Get the pair of connected pins, there should be exactly two.
         // Skip over any no-connect pins.
         while (pos != NULL && (pin0 == NULL || pin1 == NULL))
         {
            CDataDoctorPin *tempPin;
            CString tempPinRef;
            component->getPinNextAssoc(pos, tempPinRef, tempPin);
            if (tempPin != NULL && !component->IsNoConnect(m_camCadDatabase, *tempPin))
            {
               if (pin0 == NULL)
                  pin0 = tempPin;
               else if (pin1 == NULL)
                  pin1 = tempPin;
            }
         }

         // Getting pins should not have failed, but just in case ...
         if (pin0 != NULL && pin1 != NULL)
         {
            CString net0 = pin0->getNet().getNetName();
            CString net1 = pin1->getNet().getNetName();

            // If nets are jumpered then they are merged for purposes here.
            // Override the net names with merged net names if they exist.
            CString mergedNet;
            if (m_mergedNetMap.Lookup(net0, mergedNet))
               net0 = mergedNet;
            if (m_mergedNetMap.Lookup(net1, mergedNet))
               net1 = mergedNet;

            if (net0.CompareNoCase(NET_UNUSED_PINS) == 0 ||
               net0.CompareNoCase(NET_NO_NET_PINS) == 0)
            {
               net0 = getNextNoConnectNetName();
            }

            if (net1.CompareNoCase(NET_UNUSED_PINS) == 0 ||
               net1.CompareNoCase(NET_NO_NET_PINS) == 0)
            {
               net1 = getNextNoConnectNetName();
            }

            if (net0.CompareNoCase(net1) <= 0)
            {
               netPair = net0 + '|' + net1;
            }
            else
            {
               netPair = net1 + '|' + net0;
            }

            CDataDoctorComponentList* netPairComponentList = NULL;

            parallelDeviceMap.Lookup(netPair,netPairComponentList);

            if (netPairComponentList == NULL)
            {
               netPairComponentList = new CDataDoctorComponentList();
               parallelDeviceMap.SetAt(netPair,netPairComponentList);
            }

            netPairComponentList->AddTail(component);
         }
      }
   }



   CString netPair;
   CDataDoctorComponentList* netPairComponentList = NULL;

   for (POSITION pairPos = parallelDeviceMap.GetStartPosition();pairPos != NULL;)
   {
      parallelDeviceMap.GetNextAssoc(pairPos,netPair,netPairComponentList);
      int componentCount = netPairComponentList->GetCount();
      bool mixedFlag = false;
      double totalValue = 0.,maxValue = 0.;
      CDataDoctorComponent* primaryComponent = NULL;
      DeviceTypeTag componentDeviceType;

      for (POSITION pinPos = netPairComponentList->GetHeadPosition();pinPos != NULL;)
      {
         CDataDoctorComponent* component = netPairComponentList->GetNext(pinPos);

         if (primaryComponent == NULL)
         {
            componentDeviceType = component->getPassiveDeviceType();
            primaryComponent = component;
         }
         else if (componentDeviceType != component->getPassiveDeviceType())
         {
            mixedFlag = true;
            break;
         }

         ComponentValues componentValue;
         componentValue.SetValue(component->getValue(m_camCadDatabase));
         double value = componentValue.GetValue();
         CString debugStringValue = componentValue.GetPrintableString();

         if (value > maxValue)
         {
            maxValue = value;
            primaryComponent = component;
         }

         if (componentDeviceType == deviceTypeCapacitor)
         {
            totalValue += value;
         }
         else
         {
            if (value != 0.)
            {
               totalValue += 1./value;
            }
         }
      }

      if (primaryComponent != NULL && !mixedFlag)
      {
         double mergedValue = totalValue;
         CString mergedStatus;

         if (componentDeviceType != deviceTypeCapacitor)
         {
            if (mergedValue != 0.)
            {
               mergedValue = 1./mergedValue;
            }
         }

         for (POSITION pinPos = netPairComponentList->GetHeadPosition();pinPos != NULL;)
         {
            CDataDoctorComponent* component = netPairComponentList->GetNext(pinPos);

            if (component == primaryComponent)
            {
               mergedStatus = ((componentCount > 1) ? "Primary" : "Solitary");
            }
            else
            {
               primaryComponent->addMergedComponent(*component);
               mergedStatus = "Ignored";
            }

            component->setMergedStatus(m_camCadDatabase,mergedStatus);
            component->setMergedValue(m_camCadDatabase,mergedValue);
         }
      }
   }

   return retval;
}

void CDataDoctor::removeParallelAnalysisResults()
{
   int index;
   CDataDoctorComponent* component;

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      component->removeMergedAttributes(m_camCadDatabase);
      component->removeMergedComponents();
   }
}

CString CDataDoctor::getReportFilePath()
{
   CFilePath filePath(m_camCadDoc.GetProjectPath(getApp().getUserPath()));
   filePath.pushLeaf("ParallelAnalysisReport.htm");

   return filePath.getPath();
}

bool CDataDoctor::generateParallelAnalysisReport(CWriteFormat& report)
{
   bool retval = false;

   int index;
   CDataDoctorComponent* component;

   report.writef("%-15.15s %-15.15s %-15.15s %-15.15s %-25s %-25s\n",
      "RefDes",
      "MergedStatus",
      "Value",
      "MergedValue",
      "NetName1",
      "NetName2");

   for (m_components.rewind(index);m_components.next(component,index);)
   {
      CString mergedStatus = component->getMergedStatus(m_camCadDatabase);

      if (! mergedStatus.IsEmpty() && mergedStatus.CompareNoCase("Ignored") != 0)
      {
         CDataDoctorPin *pin0 = NULL, *pin1 = NULL;
         CString netPair;
         POSITION pos = component->getPinStartPosition();

         // Get the pair of connected pins, there should be exactly two.
         // Skip over any no-connect pins.
         while (pos != NULL && (pin0 == NULL || pin1 == NULL))
         {
            CDataDoctorPin *tempPin;
            CString tempPinRef;
            component->getPinNextAssoc(pos, tempPinRef, tempPin);
            if (tempPin != NULL && !component->IsNoConnect(m_camCadDatabase, *tempPin))
            {
               if (pin0 == NULL)
                  pin0 = tempPin;
               else if (pin1 == NULL)
                  pin1 = tempPin;
            }
         }

         if (pin0 != NULL && pin1 != NULL) // jic, should not happen
         {
            CString originalnet0 = pin0->getNet().getNetName();
            CString originalnet1 = pin1->getNet().getNetName();

            CString net0(originalnet0);
            CString net1(originalnet1);

            CString mergedNet;
            if (m_mergedNetMap.Lookup(net0, mergedNet))
               net0 = mergedNet;
            if (m_mergedNetMap.Lookup(net1, mergedNet))
               net1 = mergedNet;

            CSupString netnames;
            CStringArray net0names, net1names;
            netnames = net0;
            netnames.ParseQuote(net0names, ",", true);
            netnames = net1;
            netnames.ParseQuote(net1names, ",", true);

            if (net0names.GetCount() > 0)
               net0 = net0names.GetAt(0);
            if (net1names.GetCount() > 0)
               net1 = net1names.GetAt(0);

            // Use original net names here, i.e. nets that part pins directly touch
            report.writef("%-15.15s %-15.15s %-15.15s %-15.15s %-25s %-25s\n",
               (const char*)component->getRefDes(),
               (const char*)mergedStatus,
               (const char*)component->getValue(m_camCadDatabase),
               (const char*)component->getMergedValue(m_camCadDatabase),
               (const char*)originalnet0,
               (const char*)originalnet1);

            // Break up merged net names to one per line.
            // (It didn't work out to print them all on same line, got ugly when names got long).
            if (net0names.GetCount() > 1 || net1names.GetCount() > 1)
            {
               int i0 = 0;
               int i1 = 0;

               while (i0 < net0names.GetCount() || i1 < net1names.GetCount())
               {
                  // Skip over originalnet names here, they are already printed
                  do
                  {
                     net0 = "";
                     if (i0 < net0names.GetCount())
                        net0 = net0names.GetAt(i0++);
                  }
                  while (net0.Compare(originalnet0) == 0);

                  do
                  {
                     net1 = "";
                     if (i1 < net1names.GetCount())
                        net1 = net1names.GetAt(i1++);
                  }
                  while (net1.Compare(originalnet1) == 0);

                  if (!net0.IsEmpty() || !net1.IsEmpty())
                  {
                     // cheapo way to get format to align
                     report.writef("%-15.15s %-15.15s %-15.15s %-15.15s %-25s %-25s\n",
                        (const char*)"", //component->getRefDes(),
                        (const char*)"", //mergedStatus,
                        (const char*)"", //component->getValue(m_camCadDatabase),
                        (const char*)"", //component->getMergedValue(m_camCadDatabase),
                        (const char*)net0,
                        (const char*)net1);
                  }
               }
            }

            for (POSITION pos = component->getHeadMergedComponentPosition();pos != NULL;)
            {
               CDataDoctorComponent* mergedComponent = component->getMextMergedComponent(pos);

               report.writef("   %-12.12s %-15.15s %-15.15s\n",
                  (const char*)mergedComponent->getRefDes(),
                  (const char*)mergedComponent->getMergedStatus(m_camCadDatabase),
                  (const char*)mergedComponent->getValue(m_camCadDatabase));
            }
         }
      }
   }

   report.flush();

   return retval;
}

bool CDataDoctor::displayParallelAnalysisReport()
{
   bool retval = false;

   CHtmlFileWriteFormat* reportFile = NULL;

   CString reportFilePath = getReportFilePath();

   reportFile = new CHtmlFileWriteFormat(2048);

   if (reportFile->open(reportFilePath))
   {
      retval = generateParallelAnalysisReport(*reportFile);
      ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",reportFilePath,NULL,NULL,SW_SHOW);
   }

   delete reportFile;

   return retval;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// The fence is down, but still in place for easy raising.
//#define FENCEDFEATURE

bool CDataDoctor::IsAnointedUser()
{
   // Ultimately we don't need this function, it is temporary.
   // Mark request this feature unleashed 6/12/06. The fence code will remain
   // in place a short while longer, in case need arises to block access again.
   // After this feature is fully released, this code and calls to it can be
   // eliminated.

#ifdef FENCEDFEATURE
   char acUserName[100];
   DWORD nUserName = sizeof(acUserName);
   if (GetUserName(acUserName, &nUserName))
   {
      CString userName(acUserName);
      userName.MakeUpper();
      if (

         (userName.Find("FALTERS") > -1)
         || (userName.Find("LAING") > -1)

         )
      {
         return true;
      }
   }
   return false;
#else
   return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
//
int CDataDoctorRealPart::m_createdByKeywordIndex            = -1;
int CDataDoctorRealPart::m_familyKeywordIndex               = -1;

CDataDoctorRealPart::CDataDoctorRealPart(CString prefix)
: m_ccRealPartBlk(NULL)
, m_prefix(prefix)
{
}

CDataDoctorRealPart::CDataDoctorRealPart(CString prefix, CString name)
: m_ccRealPartBlk(NULL)
, m_prefix(prefix)
, m_name(name)
{
}

//------------------------------------------------------------------------------------

bool CDataDoctorRealPart::GenerateSelf(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& component, bool filled, bool overwrite)
{
   // Returns true if creation seemed to work, false if failed.
   // If overwrite is false then try to find existing block.
   // If none existing or overwrite is true then create a new one.

   BlockStruct *insertedCompBlk = component.getGeometryBlock(camCadDatabase);

   if (insertedCompBlk != NULL && !m_name.IsEmpty())
   {
      int pcbFileNum = insertedCompBlk->getFileNumber();

      m_ccRealPartBlk = Graph_Block_Exists(&camCadDatabase.getCamCadDoc(), GetRPGeomName(), pcbFileNum, blockTypeRealPart);

      if (overwrite || m_ccRealPartBlk == NULL)
      {
         BlockStruct *pkgBlk = Graph_Block_On(GBO_OVERWRITE, GetRPPkgGeomName(), pcbFileNum, 0, blockTypePackage);
         //pkgBlk->getDefinedAttributes()->setAttribute(camCadDatabase.getCamCadData(), getCreatedByKeywordIndex(camCadDatabase), "DataDoctor", attributeUpdateOverwrite);
         Graph_Block_Off();

         CPoint2d pinsCenter = component.GetPinsCentroid(camCadDatabase);
         m_ccRealPartBlk = Graph_Block_On(GBO_OVERWRITE, GetRPGeomName(), pcbFileNum, 0, blockTypeRealPart);
         DataStruct *rpPkgInsertData = Graph_Block_Reference(pkgBlk->getBlockNumber(), m_name, pinsCenter.x, pinsCenter.y, 0., 0, 1., -1);
         rpPkgInsertData->getInsert()->setInsertType(insertTypePackage);
         m_ccRealPartBlk->getDefinedAttributes()->setAttribute(camCadDatabase.getCamCadData(), getCreatedByKeywordIndex(camCadDatabase), "DataDoctor", attributeUpdateOverwrite);
         Graph_Block_Off();

         if (component.GetOldStyleDFTOutlineData(camCadDatabase) != NULL)
         {
            // Component geometry still has a package outline, so must not have undergone conversion yet.
            // Copy this outline.
            this->CloneOutline(camCadDatabase, component, filled);
         }
         else
         {
            // The Real Part item did not exist until we just now made it (above).
            // The component geometry does not contain an outline.
            // Possibly the part was converted previously, but part number has changed.
            // Look for Real Part insert by refname, if found then clone outline from it.
            DataStruct *oldRpInsertData = component.FindAssociatedRealPartInsertData(camCadDatabase);
            if (oldRpInsertData != NULL)
            {
               CDataDoctorRealPart tempRP("temp", "temp");
               tempRP.SetBlock( camCadDatabase.getBlock( oldRpInsertData->getInsert()->getBlockNumber() ));
               this->CloneOutline(camCadDatabase, tempRP, pcbFileNum, filled);

               // We have cloned the outline as-is.
               // Further above when generating the new RP we inserted the new empty package at 0,0,0.
               // We now need to update the insert of the package to match that of the source, in order to
               // make the new RP actually match package insert location and rotation.
               // We can work with tempRP to get that done.
               DataStruct *oldRpPkgInsertData = tempRP.GetPkgInsertData(camCadDatabase);
               if (oldRpPkgInsertData != NULL) // Just paranoid check
               {
                  InsertStruct *oldRpPkgInsert = oldRpPkgInsertData->getInsert();
                  rpPkgInsertData->getInsert()->setOriginX( oldRpPkgInsert->getOriginX() );
                  rpPkgInsertData->getInsert()->setOriginY( oldRpPkgInsert->getOriginY() );
                  rpPkgInsertData->getInsert()->setAngleRadians( oldRpPkgInsert->getAngleRadians() );
               }

               // The current Real Part insert is inserting the old block. Change it to the current one.
               //*rcf bug, this ends up making the insert always a per-PN insert.
               //*rcf need to fix for the source=geometry situation
               ////DataStruct *oldInsertData = FindData(camCadDatabase.getSingleVisibleFile()->getBlock(), component.getRefDes(), insertTypeRealPart);
               ////if (oldInsertData != NULL)
               ////   oldInsertData->getInsert()->setBlockNumber(m_ccRealPartBlk->getBlockNumber() );

            }

         }
      }

      return true;
   }

   return false;
}

//------------------------------------------------------------------------------------

void CDataDoctorRealPart::DefineLayers(CCamCadDatabase& camCadDatabase)
{
   int packageOutlineTopIndex = camCadDatabase.getDefinedLayerIndex(RP_LAYER_PKG_BODY_TOP,    false);
   int packageOutlineBotIndex = camCadDatabase.getDefinedLayerIndex(RP_LAYER_PKG_BODY_BOTTOM, false);
   camCadDatabase.mirrorLayers(packageOutlineTopIndex, packageOutlineBotIndex);

   LayerStruct *lp = camCadDatabase.getLayerAt(packageOutlineTopIndex);
   if (lp != NULL)
      lp->setLayerType(layerTypePackageBodyTop);

   lp = camCadDatabase.getLayerAt(packageOutlineBotIndex);
   if (lp != NULL)
      lp->setLayerType(layerTypePackageBodyBottom);
}

//------------------------------------------------------------------------------------

bool CDataDoctorRealPart::CloneOutline(CCamCadDatabase& camCadDatabase, CDataDoctorComponent& component, bool fillFlag)
{
   BlockStruct *insertedCompBlk = component.getGeometryBlock(camCadDatabase);

   if (insertedCompBlk != NULL)
   {
      int pcbFileNum = insertedCompBlk->getFileNumber();

      DataStruct *compOutlineData = component.GetOldStyleDFTOutlineData(camCadDatabase);

      if (compOutlineData != NULL)
      {
         // Old form has outline relative to pcb component geometry, want clone to be relative to pins center
         CPoint2d pinsCenter = component.GetPinsCentroid(camCadDatabase);

         return this->CloneOutline(camCadDatabase, compOutlineData, pinsCenter, pcbFileNum, fillFlag);
      }
   }

   return false;
}

//------------------------------------------------------------------------------------

bool CDataDoctorRealPart::CloneOutline(CCamCadDatabase& camCadDatabase, CDataDoctorRealPart &otherRP, int fileNumber, bool fillFlag)
{
   DataStruct *outlineData = otherRP.GetOutlineData(camCadDatabase);
   CPoint2d offset; // 0,0
   if (outlineData != NULL)
      return this->CloneOutline(camCadDatabase, outlineData, offset, fileNumber, fillFlag);

   return false;
}

//------------------------------------------------------------------------------------

bool CDataDoctorRealPart::CloneOutline(CCamCadDatabase& camCadDatabase, DataStruct *outlinePolylistInsertData, CPoint2d newOrigin, int fileNumber, bool fillFlag)
{
   if (m_ccRealPartBlk != NULL && outlinePolylistInsertData != NULL)
   {
      BlockStruct *pkgBlk = Graph_Block_On(GBO_OVERWRITE, GetRPPkgGeomName(), fileNumber, 0, blockTypePackage);

      int packageOutlineTopIndex = camCadDatabase.getDefinedLayerIndex(RP_LAYER_PKG_BODY_TOP,    false);

      DataStruct *outlineCopy = camCadDatabase.getCamCadData().getNewDataStruct(*outlinePolylistInsertData, true);
      outlineCopy->getPolyList()->GetHead()->setFilled(fillFlag);
      outlineCopy->setLayerIndex(packageOutlineTopIndex);
      outlineCopy->setGraphicClass(graphicClassPackageBody);

      if (newOrigin.x != 0. || newOrigin.y != 0.)
      {
         CTMatrix m;
         m.translate(-newOrigin.x, -newOrigin.y);
         outlineCopy->transform(m);
      }

      pkgBlk->getDataList().empty();
      pkgBlk->getDataList().AddHead(outlineCopy);

      Graph_Block_Off();

      return true;
   }

   return false;
}

//------------------------------------------------------------------------------------

void CDataDoctorRealPart::SetOutlineFill(CCamCadDatabase& camCadDatabase, bool flag)
{
   DataStruct *outlineData = this->GetOutlineData(camCadDatabase);

   if (outlineData != NULL)
   {
      CPolyList *polylist = outlineData->getPolyList();
      
      if (polylist != NULL)
      {
         CPoly *poly = polylist->GetHead();
         if (poly != NULL)
         {
            poly->setFilled(flag);
         }
      }
   }
}


//------------------------------------------------------------------------------------

DataStruct *CDataDoctorRealPart::GetPkgInsertData(CCamCadDatabase& camCadDatabase)
{
   // Return block inserted as Package within Real Part.
   // There is supposed to be only one.

   if (m_ccRealPartBlk != NULL)
   {
      return m_ccRealPartBlk->GetPackage();
   }

   return NULL;
}

//------------------------------------------------------------------------------------

BlockStruct *CDataDoctorRealPart::GetPkgBlock(CCamCadDatabase& camCadDatabase)
{
   // Return block inserted as Package within Real Part.
   // There is supposed to be only one.

   DataStruct *data = GetPkgInsertData(camCadDatabase);
   if (data != NULL)
      return camCadDatabase.getBlock(data->getInsert()->getBlockNumber());

   return NULL;
}

//------------------------------------------------------------------------------------

DataStruct *CDataDoctorRealPart::GetOutlineData(CCamCadDatabase& camCadDatabase)
{
   BlockStruct *pkg = GetPkgBlock(camCadDatabase);

   if (pkg != NULL)
   {
      POSITION pos = pkg->getHeadDataPosition();
      while (pos)
      {
         DataStruct *d = pkg->getNextData(pos);

         if (d->getDataType() == dataTypePoly && d->getGraphicClass() == graphicClassPackageBody)
         {
            return d;
         }
      }
   }

   return NULL;
}

//------------------------------------------------------------------------------------

void CDataDoctorRealPart::RemoveOutlineData(CCamCadDatabase& camCadDatabase)
{

   DataStruct *outlineData = this->GetOutlineData(camCadDatabase);

   if (outlineData != NULL)
   {
      BlockStruct *pkg = GetPkgBlock(camCadDatabase);
      camCadDatabase.getCamCadDoc().removeDataFromDataList(*pkg, outlineData);
   }
}

//------------------------------------------------------------------------------------

CString CDataDoctorRealPart::GetExistingOutlineMethod(CCamCadDatabase& camCadDatabase)
{
   // Observe that blank return means there is no outline.
   // "Unknown" return means there is an outline but we don't know the method that created it.

   CString algorithm;

   DataStruct *data = this->GetOutlineData(camCadDatabase);

   if (data != NULL)
   {
      WORD keyword = camCadDatabase.getCamCadDoc().RegisterKeyWord(ATT_OUTLINE_METHOD, 0, VT_STRING);

      Attrib* attrib = NULL;
      if (data->getAttributes() && data->getAttributes()->Lookup(keyword, attrib))
      {
         algorithm = attrib->getStringValue();
      }
      else
      {
         algorithm = outlineAlgTagToString(algUnknown);
      }
   }

   return algorithm;
}

//------------------------------------------------------------------------------------

int CDataDoctorRealPart::getCreatedByKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_createdByKeywordIndex < 0)
   {
      m_createdByKeywordIndex = camCadDatabase.getKeywordIndex(ATT_CREATED_BY);

		if (m_createdByKeywordIndex < 0)
      {
         m_createdByKeywordIndex = camCadDatabase.registerKeyword(ATT_CREATED_BY, valueTypeString);
      }
   }

   return m_createdByKeywordIndex;
}

int CDataDoctorRealPart::getFamilyKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   m_familyKeywordIndex = camCadDatabase.getKeywordIndex("RealPart_PackageFamily");

	if (m_familyKeywordIndex < 0)
   {
      m_familyKeywordIndex = camCadDatabase.registerKeyword("RealPart_PackageFamily", valueTypeString);
   }

   return m_familyKeywordIndex;
}

void CDataDoctorPart::CreateOutlineInserts(CCamCadDatabase& camCadDatabase, BlockStruct *pcbblk)
{
   if (pcbblk != NULL && this->GetPackageSource().CompareNoCase(QPartNumber) == 0)
   {
      BlockStruct *rpblk = this->GetRealPart().GetBlock();

      if (rpblk != NULL)
      {
         CDataDoctorComponent* ddcomp;
         CString refdes;

         Graph_Block_On(pcbblk);

         for (POSITION pos = m_components.GetStartPosition(); pos != NULL; )
         {
            m_components.GetNextAssoc(pos, refdes, ddcomp);

            //CString pkgsrc = ddcomp->getPackageSource(camCadDatabase);

            //if (pkgsrc.CompareNoCase(QPartNumber) == 0)
            { 
               DataStruct *oldInsertData = FindData(pcbblk, ddcomp->getRefDes(), insertTypeRealPart);
               if (oldInsertData != NULL)
                  camCadDatabase.getCamCadDoc().removeDataFromDataList(*pcbblk, oldInsertData);

               DataStruct *rpInsertData = Graph_Block_Reference(rpblk->getBlockNumber(), ddcomp->getRefDes(), 
                  ddcomp->getOrigin().x, ddcomp->getOrigin().y, ddcomp->getAngleRadians(), ddcomp->isPlacedTop()?0:1, 1.0, -1);
               rpInsertData->getInsert()->setInsertType(insertTypeRealPart);
            }
         }

         Graph_Block_Off();
      }
   }
}


void CDataDoctorPackage::CreateOutlineInserts(CCamCadDatabase& camCadDatabase, BlockStruct *pcbblk)
{
   if (pcbblk != NULL)
   {
      BlockStruct *rpblk = this->GetRealPart().GetBlock();

      if (rpblk != NULL)
      {
         Graph_Block_On(pcbblk);

         for (int j = 0; j < this->m_allInsertsOfThisPkg.GetCount(); j++)
         {
            CDataDoctorComponent *ddcomp = this->m_allInsertsOfThisPkg.GetAt(j);

            CString pkgsrc = ddcomp->getPackageSource(camCadDatabase);

            if (pkgsrc.CompareNoCase(QGeometry) == 0)
            {
               DataStruct *oldInsertData = FindData(pcbblk, ddcomp->getRefDes(), insertTypeRealPart);
               if (oldInsertData != NULL)
                  camCadDatabase.getCamCadDoc().removeDataFromDataList(*pcbblk, oldInsertData);

               DataStruct *rpInsertData = Graph_Block_Reference(rpblk->getBlockNumber(), ddcomp->getRefDes(), 
                  ddcomp->getOrigin().x, ddcomp->getOrigin().y, ddcomp->getAngleRadians(), ddcomp->isPlacedTop()?0:1, 1.0, -1);
               rpInsertData->getInsert()->setInsertType(insertTypeRealPart);
            }
         }

         Graph_Block_Off();
      }
   }
}

//-------------------------------------------------------------------

CDataDoctorLibraryAttribMap::CDataDoctorLibraryAttribMap(CString settingsID)
: m_settingsID(settingsID)
{
   // These names are handled as fixed fields in Data Doc, so we don't want
   // to offer them as customer configurable attrib names.
   // Or the name is otherwise undesirable.

   m_prohibitedNames.Add(ATT_COMPHEIGHT);
   m_prohibitedNames.Add(ATT_DEVICETYPE);
   m_prohibitedNames.Add(ATT_DPMO);
   m_prohibitedNames.Add(ATT_LOADED);
   
   m_prohibitedNames.Add(ATT_OUTLINE_METHOD);
   m_prohibitedNames.Add(ATT_PACKAGE_ALIAS);
   m_prohibitedNames.Add(ATT_PACKAGE_SOURCE);
   m_prohibitedNames.Add(ATT_PART_DESCRIPTION);
   m_prohibitedNames.Add(ATT_PARTNUMBER);
   m_prohibitedNames.Add("Quantity"); // Mark wants this one prohibited, even though it does not seem to appear in lookups, it is a BOM Importer related thing 
   m_prohibitedNames.Add(ATT_REFNAME);
   m_prohibitedNames.Add(ATT_SMDSHAPE);
   m_prohibitedNames.Add(ATT_SUBCLASS);
   m_prohibitedNames.Add(ATT_TECHNOLOGY);
   m_prohibitedNames.Add(ATT_TYPELISTLINK);

   m_prohibitedNames.Add(ATT_VALUE);
   m_prohibitedNames.Add(ATT_TOLERANCE);
   m_prohibitedNames.Add(ATT_PLUSTOLERANCE);
   m_prohibitedNames.Add(ATT_MINUSTOLERANCE);
   
   m_prohibitedNames.Add(ATT_DESIGNED_SURFACE);
   m_prohibitedNames.Add(ATT_TOP_DESIGNED_SURFACE);
   m_prohibitedNames.Add(ATT_BOT_DESIGNED_SURFACE);

}

CString CDataDoctorLibraryAttribMap::GetActiveAttribListStr()
{
   // Comma separated, double quotes around each name

   CString list;

   CDataDoctorLibraryAttrib *libattr = NULL;
   CString* attrname;

   for (this->GetFirstSorted(attrname, libattr); libattr != NULL; this->GetNextSorted(attrname, libattr))
   {
      if (libattr->IsActive())
      {
         if (!list.IsEmpty())
            list += ", ";

         list += "\"" + libattr->GetName() + "\"";
      }
   }

   return list;
}

void CDataDoctorLibraryAttribMap::SaveSettings()
{
   CString attriblist = this->GetActiveAttribListStr();
   CString fullSettingsID("Custom " + m_settingsID + " Attributes");
   CAppRegistrySetting("Data Doctor", fullSettingsID).Set(attriblist);
}

void CDataDoctorLibraryAttribMap::LoadSettings(bool reset)
{
   if (reset)
      this->RemoveAll();

   CString fullSettingsID("Custom " + m_settingsID + " Attributes");
   CString attriblist = CAppRegistrySetting("Data Doctor", fullSettingsID).Get();

   if (!attriblist.IsEmpty())
   {
      CSupString suplist(attriblist);
      CStringArray attribs;
      suplist.ParseQuote(attribs, ",");

      for (int i = 0; i < attribs.GetCount(); i++)
      {
         CString name = attribs.GetAt(i);
         LPCSTR existingkey = NULL;
         if (!name.IsEmpty() && !this->LookupKey(name, existingkey))
         {
            this->SetAt(name, new CDataDoctorLibraryAttrib(name, -1, true));
         }
      }
   }
}

void CDataDoctorLibraryAttribMap::LoadAttribNamesFromDB(bool reset)
{
   if (reset)
      this->RemoveAll();

   CDBInterface db;
   if (db.Connect())
   {
      CStringArray existingNames;
      db.GetExistingGenericAttribNames(existingNames);

      for (int i = 0; i < existingNames.GetCount(); i++)
      {
         CString name = existingNames.GetAt(i);

         LPCSTR existingkey = NULL;
         if (!name.IsEmpty() && !this->IsProhibited(name) && !this->LookupKey(name, existingkey))
         {
            this->SetAt(name, new CDataDoctorLibraryAttrib(name, -1, false));
         }
      }
   }
}

bool CDataDoctorLibraryAttribMap::IsProhibited(const CString &name)
{
   for (int i = 0; i < m_prohibitedNames.GetCount(); i++)
   {
      if (m_prohibitedNames.GetAt(i).CompareNoCase(name) == 0)
      {
         return true;
      }
   }

   return false;
}



