// $Header: /CAMCAD/4.5/DeviceType.cpp 12    8/21/06 5:44p Rick Faltersack $

#include "StdAfx.h"
#include "DeviceType.h"
#include "CompValues.h"
#include "CCEtoODB.h"

// DeviceTyper defines this too.
#define QDEV_TYPER_FILENAME_DEVICES_STP       "devices.stp"

CString deviceTypeTagToFriendlyString(DeviceTypeTag deviceType)
{
   const char* retval;

   switch(deviceType)
   {
   case deviceTypeUnknown:             retval = "Unknown";                 break;
   case deviceTypeBattery:             retval = "Battery";                 break;
   case deviceTypeCapacitor:           retval = "Capacitor";               break;
   case deviceTypeCapacitorArray:      retval = "Capacitor Array";         break;
   case deviceTypeCapacitorPolarized:  retval = "Capacitor Polarized";     break;
   case deviceTypeConnector:           retval = "Connector";               break;
   case deviceTypeCrystal:             retval = "Crystal";                 break;
   case deviceTypeDiode:               retval = "Diode";                   break;
   case deviceTypeDiodeArray:          retval = "Diode Array";             break;
   case deviceTypeDiodeLed:            retval = "Diode LED";               break;
   case deviceTypeDiodeZener:          retval = "Diode Zener";             break;
   case deviceTypeFilter:              retval = "Filter";                  break;
   case deviceTypeFuse:                retval = "Fuse";                    break;
   case deviceTypeIC:                  retval = "IC";                      break;
   case deviceTypeICDigital:           retval = "IC Digital";              break;
   case deviceTypeICLinear:            retval = "IC Linear";               break;
   case deviceTypeInductor:            retval = "Inductor";                break;
   case deviceTypeJumper:              retval = "Jumper";                  break;
   case deviceTypeNoTest:              retval = "No Test";                 break;
   case deviceTypeOscillator:          retval = "Oscillator";              break;
   case deviceTypePotentiometer:       retval = "Potentiometer";           break;
   case deviceTypePowerSupply:         retval = "Power Supply";            break;
   case deviceTypeRelay:               retval = "Relay";                   break;
   case deviceTypeResistor:            retval = "Resistor";                break;
   case deviceTypeResistorArray:       retval = "Resistor Array";          break;
   case deviceTypeSpeaker:             retval = "Speaker";                 break;
   case deviceTypeSwitch:              retval = "Switch";                  break;
   case deviceTypeTestPoint:           retval = "Test Point";              break;
   case deviceTypeTransformer:         retval = "Transformer";             break;
   case deviceTypeTransistor:          retval = "Transistor";              break;
   case deviceTypeTransistorArray:     retval = "Transistor Array";        break;
   case deviceTypeTransistorFetNpn:    retval = "Transistor FET NPN";      break;
   case deviceTypeTransistorFetPnp:    retval = "Transistor FET PNP";      break;
   case deviceTypeTransistorMosfetNpn: retval = "Transistor MosFET NPN";   break;
   case deviceTypeTransistorMosfetPnp: retval = "Transistor MosFET PNP";   break;
   case deviceTypeTransistorNpn:       retval = "Transistor NPN";          break;
   case deviceTypeTransistorPnp:       retval = "Transistor PNP";          break;
   case deviceTypeTransistorScr:       retval = "Transistor SCR";          break;
   case deviceTypeTransistorTriac:     retval = "Transistor Triac";        break;
   case deviceTypeVoltageRegulator:    retval = "Voltage Regulator";       break;
   case deviceTypeDiodeLedArray:       retval = "Diode LED Array";         break;
   case deviceTypeCapacitorTantalum:   retval = "Capacitor Tantalum";      break;
	case deviceTypeOpto:						retval = "Opto";							break;
   default:
   case deviceTypeUndefined:           retval = "Undefined";               break;
   }

   return retval;
}

CString deviceTypeTagToValueString(DeviceTypeTag deviceType)
{
   const char* retval;

   switch(deviceType)
   {
   case deviceTypeUnknown:             retval = "Unknown";                 break;
   case deviceTypeBattery:             retval = "Battery";                 break;
   case deviceTypeCapacitor:           retval = "Capacitor";               break;
   case deviceTypeCapacitorArray:      retval = "Capacitor_Array";         break;
   case deviceTypeCapacitorPolarized:  retval = "Capacitor_Polarized";     break;
   case deviceTypeCapacitorTantalum:   retval = "Capacitor_Tantalum";      break;
   case deviceTypeConnector:           retval = "Connector";               break;
   case deviceTypeCrystal:             retval = "Crystal";                 break;
   case deviceTypeDiode:               retval = "Diode";                   break;
   case deviceTypeDiodeArray:          retval = "Diode_Array";             break;
   case deviceTypeDiodeLed:            retval = "Diode_LED";               break;
   case deviceTypeDiodeLedArray:       retval = "Diode_LED_Array";         break;
   case deviceTypeDiodeZener:          retval = "Diode_Zener";             break;
   case deviceTypeFilter:              retval = "Filter";                  break;
   case deviceTypeFuse:                retval = "Fuse";                    break;
   case deviceTypeIC:                  retval = "IC";                      break;
   case deviceTypeICDigital:           retval = "IC_Digital";              break;
   case deviceTypeICLinear:            retval = "IC_Linear";               break;
   case deviceTypeInductor:            retval = "Inductor";                break;
   case deviceTypeJumper:              retval = "Jumper";                  break;
   case deviceTypeNoTest:              retval = "No_Test";                 break;
   case deviceTypeOscillator:          retval = "Oscillator";              break;
   case deviceTypePotentiometer:       retval = "Potentiometer";           break;
   case deviceTypePowerSupply:         retval = "Power_Supply";            break;
   case deviceTypeRelay:               retval = "Relay";                   break;
   case deviceTypeResistor:            retval = "Resistor";                break;
   case deviceTypeResistorArray:       retval = "Resistor_Array";          break;
   case deviceTypeSpeaker:             retval = "Speaker";                 break;
   case deviceTypeSwitch:              retval = "Switch";                  break;
   case deviceTypeTestPoint:           retval = "Test_Point";              break;
   case deviceTypeTransformer:         retval = "Transformer";             break;
   case deviceTypeTransistor:          retval = "Transistor";              break;
   case deviceTypeTransistorArray:     retval = "Transistor_Array";        break;
   case deviceTypeTransistorFetNpn:    retval = "Transistor_FET_NPN";      break;
   case deviceTypeTransistorFetPnp:    retval = "Transistor_FET_PNP";       break;
   case deviceTypeTransistorMosfetNpn: retval = "Transistor_MosFET_NPN";   break;
   case deviceTypeTransistorMosfetPnp: retval = "Transistor_MosFET_PNP";   break;
   case deviceTypeTransistorNpn:       retval = "Transistor_NPN";          break;
   case deviceTypeTransistorPnp:       retval = "Transistor_PNP";          break;
   case deviceTypeTransistorScr:       retval = "Transistor_SCR";          break;
   case deviceTypeTransistorTriac:     retval = "Transistor_Triac";        break;
   case deviceTypeVoltageRegulator:    retval = "Voltage_Regulator";       break;
	case deviceTypeOpto:						retval = "Opto";							break;
   default:
   case deviceTypeUndefined:           retval = "Undefined";               break;
   }

   return retval;
}

CString getDevceTypeListAsDelimitedFriendlyString(CString delimiter)
{
   // Original version did not handle multi-char delimiter, now it does.

	CString deviceTypeList;
   bool addDelimiter = false;

   for (DeviceTypeTag deviceType = (DeviceTypeTag)0; deviceType < deviceTypeUndefined; deviceType = (DeviceTypeTag)(deviceType + 1))
   {
      if (addDelimiter)
         deviceTypeList.AppendFormat("%s", delimiter);

      CString deviceTypeString = deviceTypeTagToFriendlyString(deviceType);
		deviceTypeList.AppendFormat("%s", deviceTypeString);
      addDelimiter = true;
   }
		
   return deviceTypeList;
}

CString getDevceTypeListAsDelimitedValueString(CString delimiter)
{
   // Original version did not handle multi-char delimiter, now it does.

	CString deviceTypeList;
   bool addDelimiter = false;

   for (DeviceTypeTag deviceType = (DeviceTypeTag)0; deviceType < deviceTypeUndefined; deviceType = (DeviceTypeTag)(deviceType + 1))
   {
      if (addDelimiter)
         deviceTypeList.AppendFormat("%s", delimiter);

      CString deviceTypeString = deviceTypeTagToValueString(deviceType);
		deviceTypeList.AppendFormat("%s", deviceTypeString);
      addDelimiter = true;
   }
		
   return deviceTypeList;
}

DeviceTypeTag stringToDeviceTypeTag(const CString& valueString)
{
   CString deviceTypeString;
   DeviceTypeTag deviceType;

   for (deviceType = (DeviceTypeTag)0; deviceType < deviceTypeUndefined; deviceType = (DeviceTypeTag)(deviceType + 1))
   {
      deviceTypeString = deviceTypeTagToValueString(deviceType);
      if (deviceTypeString.CompareNoCase(valueString) == 0)
         break;

      deviceTypeString = deviceTypeTagToFriendlyString(deviceType);
      if (deviceTypeString.CompareNoCase(valueString) == 0)
         break;
   }

   return deviceType;
}

DeviceTypeTag getDefaultDeviceTypeForValueUnit(ValueUnitTag valueUnit)
{
   DeviceTypeTag deviceType = deviceTypeUndefined;

   switch (valueUnit)
   {
   case valueUnitOhm:
      deviceType = deviceTypeResistor;
      break;
   case valueUnitFarad:
      deviceType = deviceTypeCapacitor;
      break;
   case valueUnitHenry:
      deviceType = deviceTypeInductor;
      break;
   case valueUnitVolt:
      deviceType = deviceTypeResistor;
      break;
   case valueUnitAmphere:
      deviceType = deviceTypeFuse;
      break;
   case valueUnitWatt:
      deviceType = deviceTypePowerSupply;
      break;
   case valueUnitHertz:
      deviceType = deviceTypeOscillator;
      break;
   case valueUnitJoule:
      deviceType = deviceTypeUnknown;
      break;
   case valueUnitLumen:
      deviceType = deviceTypeDiodeLed;
      break;
   }

   return deviceType;
}

ValueUnitTag getDefaultValueUnitForDeviceType(DeviceTypeTag deviceType)
{
   ValueUnitTag valueUnit = valueUnitUndefined;

   switch (deviceType)
   {
   case deviceTypeUnknown:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeBattery:
      valueUnit = valueUnitVolt;
      break;
   case deviceTypeCapacitor:
   case deviceTypeCapacitorArray:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      valueUnit = valueUnitFarad;
      break;
   case deviceTypeConnector:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeCrystal:
      valueUnit = valueUnitHertz;
      break;
   case deviceTypeDiode:
   case deviceTypeDiodeArray:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeDiodeLed:
   case deviceTypeDiodeLedArray:
      valueUnit = valueUnitLumen;
      break;
   case deviceTypeDiodeZener:
      valueUnit = valueUnitVolt;
      break;
   case deviceTypeFilter:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeFuse:
      valueUnit = valueUnitAmphere;
      break;
   case deviceTypeIC:
   case deviceTypeICDigital:
   case deviceTypeICLinear:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeInductor:
      valueUnit = valueUnitHenry;
      break;
   case deviceTypeJumper:
   case deviceTypeNoTest:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeOscillator:
      valueUnit = valueUnitHertz;
      break;
   case deviceTypePotentiometer:
      valueUnit = valueUnitOhm;
      break;
   case deviceTypePowerSupply:
      valueUnit = valueUnitWatt;
      break;
   case deviceTypeRelay:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeResistor:
   case deviceTypeResistorArray:
      valueUnit = valueUnitOhm;
      break;
   case deviceTypeSpeaker:
   case deviceTypeSwitch:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeTestPoint:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeTransformer:
   case deviceTypeTransistor:
   case deviceTypeTransistorArray:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
	case deviceTypeOpto:
      valueUnit = valueUnitUndefined;
      break;
   case deviceTypeVoltageRegulator:
      valueUnit = valueUnitVolt;
      break;
   case deviceTypeUndefined:
      valueUnit = valueUnitUndefined;
      break;
   }

   return valueUnit;
}

bool isValueUnitCompatibleWithDeviceType(ValueUnitTag valueUnit,DeviceTypeTag deviceType)
{
   bool retval = false;

   switch (deviceType)
   {
   case deviceTypeUnknown:
      retval = true;
      break;
   case deviceTypeBattery:
      retval = true;
      break;
   case deviceTypeCapacitor:
   case deviceTypeCapacitorArray:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      retval = (valueUnit == valueUnitFarad);
      break;
   case deviceTypeConnector:
      retval = true;
      break;
   case deviceTypeCrystal:
      retval = true;
      break;
   case deviceTypeDiode:
   case deviceTypeDiodeArray:
      retval = true;
      break;
   case deviceTypeDiodeLed:
   case deviceTypeDiodeLedArray:
      retval = true;
      break;
   case deviceTypeDiodeZener:
      retval = (valueUnit == valueUnitVolt);
      break;
   case deviceTypeFilter:
      retval = true;
      break;
   case deviceTypeFuse:
      retval = true;
      break;
   case deviceTypeIC:
   case deviceTypeICDigital:
   case deviceTypeICLinear:
      retval = true;
      break;
   case deviceTypeInductor:
      retval = (valueUnit == valueUnitHenry);
      break;
   case deviceTypeJumper:
   case deviceTypeNoTest:
      retval = true;
      break;
   case deviceTypeOscillator:
      retval = true;
      break;
   case deviceTypePotentiometer:
      retval = (valueUnit == valueUnitOhm);
      break;
   case deviceTypePowerSupply:
      retval = true;
      break;
   case deviceTypeRelay:
      retval = true;
      break;
   case deviceTypeResistor:
   case deviceTypeResistorArray:
      retval = (valueUnit == valueUnitOhm    );
      break;
   case deviceTypeSpeaker:
   case deviceTypeSwitch:
      retval = true;
      break;
   case deviceTypeTestPoint:
      retval = true;
      break;
   case deviceTypeTransformer:
   case deviceTypeTransistor:
   case deviceTypeTransistorArray:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
	case deviceTypeOpto:
      retval = true;
      break;
   case deviceTypeVoltageRegulator:
      retval = true;
      break;
   case deviceTypeUndefined:
      retval = true;
      break;
   }

   return retval;
}

CString pinTypeTagToValueString(PinTypeTag pinType)
{
	CString retval = "";

   switch(pinType)
   {
   case pinTypeUnknown:             retval = "Unknown";               break;
   case pinTypeBase:						retval = "Base";				       break;
   case pinTypeEmitter:					retval = "Emitter";               break;
   case pinTypeCollector:           retval = "Collector";             break;
   case pinTypeAnode:					retval = "Anode";                 break;
   case pinTypeCathode:					retval = "Cathode";               break;
   case pinTypePositive:				retval = "Positive";              break;
   case pinTypeNegative:				retval = "Negative";              break;
   case pinTypeDrain:					retval = "Drain";		             break;
   case pinTypeGate:						retval = "Gate";	                break;
   case pinTypeSource:					retval = "Source";                break;
	}

	return retval;
}

PinTypeTag stringToPinTypeTag(const CString& valueString)
{
   CString pinTypeString = "";
   PinTypeTag pinType = pinTypeUnknown;

   for (pinType = (PinTypeTag)0; pinType < pinTypeMax; pinType = (PinTypeTag)(pinType + 1))
   {
      pinTypeString = pinTypeTagToValueString(pinType);

      if (pinTypeString.CompareNoCase(valueString) == 0)
         break;
   }

   return pinType;
}

//_____________________________________________________________________________
CDeviceType::CDeviceType(int id,const CString& descriptor)
{
   m_id          = id;
   m_descriptor  = descriptor;
   m_minPinCount = -1;
   m_maxPinCount = -1;

   parseDescriptor();
}

void CDeviceType::parseDescriptor()
{
   CStringArray params;
   int numParams = m_descriptor.Parse(params,",");

   if (numParams > 0)
   {
      m_deviceTypeIdentifier = params[0];

      if (m_deviceTypeIdentifier.Left(2) == "//")
      {
         m_deviceTypeIdentifier.Empty();
      }
      else if (numParams > 1)
      {
         calcFriendlyIdentifiers();

         m_minPinCount = atoi(params[1]);

         if (numParams > 2)
         {
            if (params[2] != "*")
            {
               m_maxPinCount = atoi(params[1]);
            }

            if (numParams > 3)
            {
               CStringArray pinNames;
               CSupString pinNamesString(params[3]);

               pinNamesString.ParseQuote(pinNames," ");

               for (int ind = 0;ind < pinNames.GetCount();ind++)
               {
                  m_pinNames.SetAt(pinNames[ind],"");
               }
            }
         }
      }
   }
}

void CDeviceType::calcFriendlyIdentifiers()
{
   CSupString deviceType(m_deviceTypeIdentifier);
   CStringArray words;
   deviceType.Parse(words,"_");

   m_friendlyDeviceTypeIdentifier.Empty();
   m_camelCaseDeviceTypeIdentifier.Empty();

   for (int ind = 0;ind < words.GetCount();ind++)
   {
      CString word = words[ind];

      m_friendlyDeviceTypeIdentifier += word;

      word.MakeLower();

      if (ind > 0 && word.GetLength() > 0)
      {
         word.SetAt(0,toupper(word[0]));
      }

      m_camelCaseDeviceTypeIdentifier += word;
   }
}

//_____________________________________________________________________________
CDeviceTypes* CDeviceTypes::s_deviceTypes = NULL;

CDeviceTypes::CDeviceTypes()
{
}

CDeviceTypes& CDeviceTypes::getDeviceTypes()
{
   if (s_deviceTypes == NULL)
   {
      s_deviceTypes = new CDeviceTypes();
      s_deviceTypes->read();
   }

   return *s_deviceTypes;
}

void CDeviceTypes::reset()
{
   if (s_deviceTypes == NULL)
      getDeviceTypes();
   else
      s_deviceTypes->read();
}

CDeviceType* CDeviceTypes::lookup(CString deviceTypeIdentifier)
{
   CDeviceType* deviceType = NULL;
   deviceTypeIdentifier.MakeLower();

   if (!m_devices.Lookup(deviceTypeIdentifier,deviceType))
   {
      deviceType = NULL;
   }

   return deviceType;
}

bool CDeviceTypes::read()
{
   bool retval = false;

   CString devicesStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_DEVICES_STP) );

   m_devices.empty();
   CStdioFile file;

   if (file.Open(devicesStpFilename,CFile::modeRead | CFile::shareDenyWrite))
   {
      retval = true;
      CString descriptor;

      for (int id = 0;file.ReadString(descriptor);)
      {
         CDeviceType* deviceType = new CDeviceType(id,descriptor);
         CString deviceTypeIdentifier = deviceType->getDeviceTypeIdentifier();

         if (deviceTypeIdentifier.IsEmpty() ||
             (lookup(deviceTypeIdentifier) != NULL))
         {
            delete deviceType;
         }
         else
         {
            deviceTypeIdentifier.MakeLower();
            m_devices.SetAt(deviceTypeIdentifier,deviceType);
            id++;
         }
      }
   }
   else
   {
      CString tmp;
      tmp.Format("Cannot open [%s] for reading.", devicesStpFilename);
      ErrorMessage(tmp,"", MB_OK | MB_ICONHAND);
   }

   return retval;
}
