// $Header: /CAMCAD/4.5/DeviceType.h 9     10/21/05 4:13p Lynn Phung $

#if ! defined (__DeviceType_h__)
#define __DeviceType_h__

#include "TypedContainer.h"
#include "RwLib.h"

#pragma once

enum ValueUnitTag;

// should be placed in alphabetical order, except for deviceTypeUnknown and deviceTypeUndefined
enum DeviceTypeTag
{
	deviceTypeUnknown,     // always first
	deviceTypeBattery,
	deviceTypeCapacitor,
	deviceTypeCapacitorArray,
	deviceTypeCapacitorPolarized,
	deviceTypeCapacitorTantalum,
	deviceTypeConnector,
	deviceTypeCrystal,
	deviceTypeDiode,
	deviceTypeDiodeArray,
	deviceTypeDiodeLed,
	deviceTypeDiodeLedArray,
	deviceTypeDiodeZener,
	deviceTypeFilter,
	deviceTypeFuse,
	deviceTypeIC,
	deviceTypeICDigital,
	deviceTypeICLinear,
	deviceTypeInductor,
	deviceTypeJumper,
	deviceTypeNoTest,
	deviceTypeOscillator,
	deviceTypePotentiometer,
	deviceTypePowerSupply,
	deviceTypeRelay,
	deviceTypeResistor,
	deviceTypeResistorArray,
	deviceTypeSpeaker,
	deviceTypeSwitch,
	deviceTypeTestPoint,
	deviceTypeTransformer,
	deviceTypeTransistor,
	deviceTypeTransistorArray,
	deviceTypeTransistorFetNpn,
	deviceTypeTransistorFetPnp,
	deviceTypeTransistorMosfetNpn,
	deviceTypeTransistorMosfetPnp,
	deviceTypeTransistorNpn,
	deviceTypeTransistorPnp,
	deviceTypeTransistorScr,
	deviceTypeTransistorTriac,
	deviceTypeVoltageRegulator,
	deviceTypeOpto,
	deviceTypeUndefined          // always last
};

CString deviceTypeTagToFriendlyString(DeviceTypeTag deviceType);
CString deviceTypeTagToValueString(DeviceTypeTag deviceType);
CString getDevceTypeListAsDelimitedFriendlyString(CString delimiter);
CString getDevceTypeListAsDelimitedValueString(CString delimiter);
DeviceTypeTag stringToDeviceTypeTag(const CString& valueString);
DeviceTypeTag getDefaultDeviceTypeForValueUnit(ValueUnitTag valueUnit);
ValueUnitTag getDefaultValueUnitForDeviceType(DeviceTypeTag deviceType);
bool isValueUnitCompatibleWithDeviceType(ValueUnitTag valueUnit,DeviceTypeTag deviceType);

enum PinTypeTag
{
	pinTypeMin			= 0,
	pinTypeUnknown		= 0,
	pinTypeBase			= 1,
	pinTypeEmitter		= 2,
	pinTypeCollector	= 3,
	pinTypeAnode		= 4,
	pinTypeCathode		= 5,
	pinTypePositive	= 6,
	pinTypeNegative	= 7,
	pinTypeDrain		= 8,
	pinTypeGate			= 9,
	pinTypeSource		= 10,
	pinTypeMax			= 11,
};

CString pinTypeTagToValueString(PinTypeTag pinType);
PinTypeTag stringToPinTypeTag(const CString& valueString);

//_____________________________________________________________________________
class CDeviceType
{
private:
   int m_id;
   CSupString m_descriptor;
   CString m_deviceTypeIdentifier;            // IC_Digital
   CString m_friendlyDeviceTypeIdentifier;    // ICDigital
   CString m_camelCaseDeviceTypeIdentifier;   // icDigital
   int m_minPinCount;
   int m_maxPinCount;
   CMapStringToString m_pinNames;
   //CMapStringToString m_requiredAttributeNames;

public:
   CDeviceType(int id,const CString& descriptor);

   int getId()                                const { return m_id; }
   CString getDeviceTypeIdentifier()          const { return m_deviceTypeIdentifier; }
   CString getFriendlyDeviceTypeIdentifier()  const { return m_friendlyDeviceTypeIdentifier; }
   CString getCamelCaseDeviceTypeIdentifier() const { return m_camelCaseDeviceTypeIdentifier; }
   int getMinPinCount()                       const { return m_minPinCount; }
   int getMaxPinCount()                       const { return m_maxPinCount; }
   bool hasPinName(const CString& pinName) const;
   CMapStringToString& getPinNames()                { return m_pinNames; }

private:
   void parseDescriptor();
   void calcFriendlyIdentifiers();
};

//_____________________________________________________________________________
class CDeviceTypes
{
private:
   static CDeviceTypes* s_deviceTypes;
   CTypedMapStringToPtrContainer<CDeviceType*> m_devices;

public:
   CDeviceTypes();

   static CDeviceTypes& getDeviceTypes();

   static void reset(); // re-read devices.stp

   CDeviceType* lookup(CString deviceType);

private:
   bool read();
};

#endif
