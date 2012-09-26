// $Header: /CAMCAD/5.0/read_wrt/DigitalTest_o.cpp 23    6/17/07 8:56p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "digitaltest_o.h"
#include "CCEtoODB.h"
#include "net_util.h"
#include "CCEtoODB.h"
#include "graph.h"
#include ".\digitaltest_o.h"
#include "dft.h"
#include "ck.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
* DigitalTest_WriteFile
*/
void DigitalTest_WriteFile(const CString& filename, CCEtoODBDoc &doc, FormatStruct *format)
{
	FileStruct *file = doc.getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
	{
		ErrorMessage("Only not single file shown to export", "Digital Test Writer", MB_OK);
		return;
	}

	try
	{
		CDigitalTestWriter dtWriter(*file, doc);

		CDigitalTestSettingsDlg dlg(dtWriter);
		if (dlg.DoModal() == IDOK)
		{
			// Update settings from dialog
			dtWriter.getSettings()->setAnalogLow(dlg.m_analogLow);
			dtWriter.getSettings()->setAnalogHigh(dlg.m_analogHigh);
			dtWriter.getSettings()->setDigitalHigh(dlg.m_digitalHigh);

			if (dlg.m_analogLow >= dlg.m_analogHigh ||
				dlg.m_digitalHigh <= dlg.m_analogHigh) 
			{
				ErrorMessage("Probe numbering range error. Analog_Low must be less than Analog_High, Digital_High must be greater than Analog_High.", "Digital Test Writer");
				return;
			}

			// output all file info
			dtWriter.WriteFiles(filename);
		}
	}
	catch (CString exception)
	{
		ErrorMessage(exception, "Digital Test Writer", MB_OK);
	}
}

/******************************************************************************
* CPin::CPin
*/
CPin::CPin(DataStruct *data, CPinList &parentPinList, CComponent *parentComponent)
	: CSortKeyItem()
	, m_pComponent(parentComponent)
	, m_pPinList(&parentPinList)
	, m_iNumber(0)
{
	if (data == NULL || data->getDataType() != dataTypeInsert)
		return;
	if (data->getInsert()->getInsertType() != insertTypePin)
		return;

	m_sName = data->getInsert()->getRefname();

	Attrib *attr = NULL;
	CCEtoODBDoc &doc = parentPinList.GetDoc();

	WORD pinNumKW = doc.RegisterKeyWord(ATT_COMPPINNR, 0, valueTypeInteger);
	if (data->lookUpAttrib(pinNumKW, attr))
      m_iNumber = attr->getIntValue();

	if (parentComponent != NULL)
	{
		NetStruct *net = NULL;
      CompPinStruct *cp = NULL;
		if ((cp = FindCompPin(&parentPinList.GetFile(), parentComponent->GetRefName(), m_sName, &net)) != NULL)
      {
			m_sNetName = net->getNetName();

			WORD pinMapKW = doc.RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
			Attrib *attrib = NULL;
			if (cp->getAttributesRef() != NULL && cp->getAttributesRef()->Lookup(pinMapKW, attrib))
			{
				CString pinTypeStr = get_attvalue_string(&doc, attrib);
				m_sPinType = pinTypeStr;
			}
      }
	}
}

CPin::CPin(const CPin &other)
	: CSortKeyItem()
{
	if (&other != this)
	{
		m_pComponent = other.m_pComponent;
		m_pPinList = other.m_pPinList;
		m_sName = other.m_sName;
		m_iNumber = other.m_iNumber;
		m_sPinType = other.m_sPinType;
		m_sNetName = other.m_sNetName;
		m_sSortKey = other.m_sSortKey;
	}
}

void CPin::DumpToFile(CWriteFormat &file)
{
	file.writef("Name : %s\n", m_sName);
	file.writef("Number : %d\n", m_iNumber);
	file.writef("Type : %s\n", m_sPinType);
	file.writef("Net : %s\n", m_sNetName);
	file.writef("Sort Key : %s\n", m_sSortKey);
}



/******************************************************************************
* CPinList::CPinList
*/
CPinList::CPinList(CCEtoODBDoc &doc, FileStruct &file, CComponent *parentComponent, CString(*keyGenFunc)(const CString))
	: CTypedPtrListContainer<CPin*>()
	, m_doc(doc)
	, m_file(file)
	, m_pComponent(parentComponent)
	, m_pKeyGenFunction(keyGenFunc)
	, m_bSorted(false)
{
}

void CPinList::DumpToFile(CWriteFormat &file)
{
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CPin *pin = GetAt(pos);

		file.writef("Pin : %s\n", pin->GetName());

		file.pushHeader("   ");
		pin->DumpToFile(file);
		file.popHeader();
	}
}

void CPinList::Sort(CString(*keyGenFunc)(const CString))
{
	if (m_bSorted)
		return;

	if (keyGenFunc != NULL)
		m_pKeyGenFunction = keyGenFunc;

	if (m_pKeyGenFunction == NULL)
		return;

	// regenerate keys (CPinList)
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CPin *pin = GetAt(pos);
		pin->SetSortKey((*m_pKeyGenFunction)(pin->GetName()));
	}

	// TODO: sort by keys (CPinList)
   //m_sortSize = GetCount();
   //CPin *pins = new CPin[m_sortSize];

   //POSITION pos;
   //CString key;
   //CPObject* pObject;
   //int i;

   //for (pos = CMapStringToOb::GetStartPosition(),i = 0;pos != NULL && i < m_sortSize;i++)
   //{
   //   CMapStringToOb::GetNextAssoc(pos,key,(CObject*&)pObject);
   //   m_hElement[i].pString = new CString(key);
   //   m_hElement[i].pObject = pObject;
   //}

   //qsort((void*)m_hElement,(size_t)m_sortSize,sizeof(SElement), m_sortFunction);

	m_bSorted = true;
}

bool CPinList::AddPin(DataStruct *data)
{
	CPin *pin = new CPin(data, *this, m_pComponent);
	AddTail(pin);

	m_bSorted = false;
	return true;
}




/******************************************************************************
* CDevice::CDevice
*/
CDevice::CDevice(DataStruct *data, CCEtoODBDoc &doc, CComponent *parentComponent)
	: CObject()
	, m_pComponent(parentComponent)
	, m_dPlusTol(0.0)
	, m_dMinusTol(0.0)
{
	if (data == NULL || data->getDataType() != dataTypeInsert)
		return;
	if (data->getInsert()->getInsertType() != insertTypePcbComponent && data->getInsert()->getInsertType() != insertTypeTestPoint)
		return;

	if (data->getInsert()->getInsertType() == insertTypeTestPoint)
	{
		m_sType = "TestPoint";
		return;
	}

	Attrib *attr = NULL;
	WORD devTypeKW = doc.RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
	if (data->lookUpAttrib(devTypeKW, attr))
		m_sType = get_attvalue_string(&doc, attr);

	WORD valueKW = doc.RegisterKeyWord(ATT_VALUE, 0, valueTypeString);
	if (data->lookUpAttrib(valueKW, attr))
		m_cvValue.SetValue(get_attvalue_string(&doc, attr));

	// Make sure units are set for these part types, even if value is zero
	if (m_cvValue.GetValue() == 0.0)
	{
		// Value is already zero, but "not valid". Reset it to make it "valid".
		// We want this so "getPrintableString()" will give us a value and units.
		// Must use the "Cstring arg" call, the "double arg" call does not set "valid".
		m_cvValue.SetValue("0.0");

		// Now set units depending on part type
		CString deviceType = m_sType;
		deviceType.MakeUpper();

		if (deviceType == "RESISTOR")
			m_cvValue.SetUnits(valueUnitOhm);
		else if (deviceType == "CAPACITOR" || deviceType == "CAPACITOR_POLARIZED" || deviceType == "CAPACITOR_TANTALUM")
			m_cvValue.SetUnits(valueUnitFarad);
		else if (deviceType == "INDUCTOR")
			m_cvValue.SetUnits(valueUnitHenry);
	}

	WORD tolKW = doc.RegisterKeyWord(ATT_TOLERANCE, 0, valueTypeDouble);
	if (data->lookUpAttrib(tolKW, attr))
		m_dPlusTol = m_dMinusTol = attr->getDoubleValue();

	WORD pTolKW = doc.RegisterKeyWord(ATT_PLUSTOLERANCE, 0, valueTypeDouble);
	if (data->lookUpAttrib(pTolKW, attr))
		m_dPlusTol = attr->getDoubleValue();

	WORD mTolKW = doc.RegisterKeyWord(ATT_MINUSTOLERANCE, 0, valueTypeDouble);
	if (data->lookUpAttrib(mTolKW, attr))
		m_dMinusTol = attr->getDoubleValue();

}

CDevice::CDevice(const CDevice &other)
	: CObject()
{
	if (&other != this)
	{
		m_pComponent = other.m_pComponent;
		m_sType = other.m_sType;
		m_cvValue = other.m_cvValue;
		m_dPlusTol = other.m_dPlusTol;
		m_dMinusTol = other.m_dMinusTol;
	}
}

CString CDevice::GetValue(bool allowSpaceInValue)
{
	CString val = m_cvValue.GetPrintableString(allowSpaceInValue);

	if (val.IsEmpty()) val = "0 "; // the trailing space is important

	return val;

}
	
void CDevice::DumpToFile(CWriteFormat &file)
{
	file.writef("Device Type : %s\n", m_sType);
	file.writef("Value : %s\n", m_cvValue.GetPrintableString());
	file.writef("Plus Tolerance : %0.3f\n", m_dPlusTol);
	file.writef("Minus Tolerance : %0.3f\n", m_dMinusTol);
}



/******************************************************************************
* CComponent::CComponent
*/
CComponent::CComponent(DataStruct *data, CCEtoODBDoc &doc, FileStruct &file, CComponentList *parentComponentList)
	: CSortKeyItem()
	, m_pComponentData(data)
	, m_pComponentList(parentComponentList)
	, m_cDevice(data, doc, this)
	, m_lPins(doc, file, this)
{
	if (data == NULL || data->getDataType() != dataTypeInsert)
		return;
	if (data->getInsert()->getInsertType() != insertTypePcbComponent && data->getInsert()->getInsertType() != insertTypeTestPoint)
		return;

	m_sRefName = data->getInsert()->getRefname();

	Attrib *attr = NULL;
	WORD partNumberKW = doc.RegisterKeyWord(ATT_PARTNUMBER, 0, valueTypeString);
	if (data->lookUpAttrib(partNumberKW, attr))
		m_sPartNumber = get_attvalue_string(&doc, attr);

	BlockStruct *compBlock = doc.getBlockAt(data->getInsert()->getBlockNumber());
	if (compBlock != NULL)
	{
		for (POSITION pos=compBlock->getHeadDataInsertPosition(); pos!=NULL; compBlock->getNextDataInsert(pos))
		{
			DataStruct *pinData = compBlock->getAtData(pos);
			if (pinData->getDataType() != dataTypeInsert)
				continue;

			InsertStruct *pinInsert = pinData->getInsert();
			if (pinInsert->getInsertType() != insertTypePin)
				continue;

			m_lPins.AddPin(pinData);
		}
	}
}

CComponent::CComponent(const CComponent &other)
	: CSortKeyItem()
	, m_cDevice(other.m_cDevice)
	, m_lPins(other.m_lPins.GetDoc(), other.m_lPins.GetFile())
{
	if (&other != this)
	{
		m_pComponentData = other.m_pComponentData;
		m_pComponentList = other.m_pComponentList;
		m_sRefName = other.m_sRefName;
		m_sPartNumber = other.m_sPartNumber;
		m_sSortKey = other.m_sSortKey;

		// TODO: copy pins, parentComponent, and key gen function
		//m_lPins = other.m_lPins;
	}
}

void CComponent::DumpToFile(CWriteFormat &file)
{
	file.writef("Reference Name : %s\n", m_sRefName);
	file.writef("Part Number : %s\n", m_sPartNumber);
	file.writef("Sort Key : %s\n", m_sSortKey);
	m_cDevice.DumpToFile(file);

	file.pushHeader("   ");
	m_lPins.DumpToFile(file);
	file.popHeader();
}

void CComponent::SortPins(CString(*keyGenFunc)(const CString))
{
	m_lPins.Sort(keyGenFunc);
}





/******************************************************************************
* CComponentList::CComponentList
*/
CComponentList::CComponentList(CCEtoODBDoc &doc, FileStruct &file, CString(*keyGenFunc)(const CString))
	: CTypedPtrListContainer<CComponent*>()
	, m_doc(doc)
	, m_file(file)
	, m_pKeyGenFunction(keyGenFunc)
	, m_bSorted(false)
{
}

void CComponentList::DumpToFile(CWriteFormat &file)
{
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CComponent *comp = GetAt(pos);

		file.writef("Component : %s\n", comp->GetRefName());

		file.pushHeader("   ");
		comp->DumpToFile(file);
		file.popHeader();
	}
}

void CComponentList::Sort(CString(*keyGenFunc)(const CString))
{
	if (m_bSorted)
		return;

	if (keyGenFunc != NULL)
		m_pKeyGenFunction = keyGenFunc;

	if (m_pKeyGenFunction == NULL)
		return;

	// regenerate keys (CComponentList)
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CComponent *comp = GetAt(pos);
		comp->SetSortKey((*m_pKeyGenFunction)(comp->GetRefName()));
		comp->SortPins(m_pKeyGenFunction);
	}

	// TODO: sort by keys (CComponentList)

	m_bSorted = true;
}

bool CComponentList::AddComponent(DataStruct *data)
{
	CComponent *component = new CComponent(data, m_doc, m_file, this);
	AddTail(component);

	m_bSorted = false;
	return true;
}



/******************************************************************************
* CDTComponent
*/
CDTComponent::CDTComponent(DataStruct *data, CCEtoODBDoc &doc, FileStruct &file, CComponentList *parentComponentList)
	: CComponent(data, doc, file, parentComponentList)
{
	if (data == NULL || data->getDataType() != dataTypeInsert)
		return;
	if (data->getInsert()->getInsertType() != insertTypePcbComponent && data->getInsert()->getInsertType() != insertTypeTestPoint)
		return;

	Attrib *attr = NULL;
	WORD subClassKW = doc.RegisterKeyWord(ATT_SUBCLASS, 0, valueTypeString);
	if (data->lookUpAttrib(subClassKW, attr))
		m_sSubClass = get_attvalue_string(&doc, attr);
}

CString CDTComponent::GetPartNumberString()
{
	CString buf, deviceName = GetDeviceName();

	// we need to format the string based on the device type.  If there is no device type, then return an empty string
	if (deviceName.IsEmpty())
		return "";

	CString dtModel;

   CString dtDeviceName = CDigitalTestWriter::GetDigitalTestType(GetDeviceName());

	deviceName.MakeUpper();

	if (deviceName == "BATTERY"					|| deviceName == "CAPACITOR_ARRAY"			|| deviceName == "CRYSTAL" ||
		 deviceName == "DIODE_ARRAY"				|| deviceName == "FILTER"						|| deviceName == "DIODE_ZENER" ||
		 deviceName == "IC"							|| deviceName == "IC_DIGITAL"					|| deviceName == "IC_LINEAR" ||
		 deviceName == "NO_TEST"					|| deviceName == "OSCILLATOR"					|| deviceName == "POTENTIOMETER" ||
		 deviceName == "POWER_SUPPLY"				|| deviceName == "RELAY"						|| deviceName == "RESISTOR_ARRAY" ||
		 deviceName == "SPEAKER"					|| deviceName == "SWITCH"						|| deviceName == "TEST_POINT" ||
		 deviceName == "TRANSFORMER"				|| deviceName == "TRANSISTOR_ARRAY"			|| deviceName == "TRANSISTOR_FET_NPN" ||
		 deviceName == "TRANSISTOR_FET_PNP"		|| deviceName == "TRANSISTOR_MOSFET_NPN"	|| deviceName == "TRANSISTOR_MOSFET_PNP" ||
		 deviceName == "TRANSISTOR_SCR"			|| deviceName == "TRANSISTOR_TRIAC"			|| deviceName == "VOLTAGE_REGULATOR")
	{
		dtModel = m_sSubClass;
		if (dtModel.IsEmpty())
			dtModel.Format("%s_%s", dtDeviceName, GetPartNumber());
	}
	else if (deviceName == "RESISTOR")
	{
		// Case 1631, no decimal places in tolerance wanted in name
		dtModel.Format("%s_%s_%0.0f_%0.0f", dtDeviceName, m_cDevice.GetValue(false), m_cDevice.GetPlusTolerance(), m_cDevice.GetMinusTolerance());
	}
	else if (deviceName == "CAPACITOR"	|| deviceName == "CAPACITOR_POLARIZED" || deviceName == "CAPACITOR_TANTALUM" ||
				deviceName == "INDUCTOR")
	{
		// Case 1631, no decimal places in tolerance wanted in name
		dtModel.Format("%s_%s_%0.0f_%0.0f", dtDeviceName, m_cDevice.GetValue(false), m_cDevice.GetPlusTolerance(), m_cDevice.GetMinusTolerance());
	}
	else
	{
		CString modelPrefix;
		if (deviceName == "DIODE"				|| deviceName == "FUSE"					|| deviceName == "JUMPER" ||
			 deviceName == "TRANSISTOR"		|| deviceName == "TRANSISTOR_NPN"	|| deviceName == "TRANSISTOR_PNP")
		{
			modelPrefix = deviceName;
		}
		else if (deviceName == "CONNECTOR")
		{
			modelPrefix = "CONN";
		}
		else if (deviceName == "DIODE_LED")
		{
			modelPrefix = "LED";
		}
		
		dtModel.Format("%s_%d", modelPrefix, m_lPins.GetCount());
	}

	// logic to determine the remaining fields
	CString parameters;
	if (deviceName == "RESISTOR")
	{
		CString pTol, mTol;
		pTol.Format("%0.3f %%", m_cDevice.GetPlusTolerance());
		mTol.Format("%0.3f %%", m_cDevice.GetMinusTolerance());
		// Value ; HTol ; LTol
		parameters.Format("%-11s; %-10s; %s", m_cDevice.GetValue()+"Ohm", pTol, mTol);
	}
	else if (deviceName == "CAPACITOR" || deviceName == "CAPACITOR_POLARIZED" || deviceName == "CAPACITOR_TANTALUM" || deviceName == "INDUCTOR")
	{
		CString pTol, mTol;
		pTol.Format("%0.3f %%", m_cDevice.GetPlusTolerance());
		mTol.Format("%0.3f %%", m_cDevice.GetMinusTolerance());
		// Value ; HTol ; LTol
		parameters.Format("%-11s; %-10s; %s", m_cDevice.GetValue(), pTol, mTol);
	}
	else if (deviceName == "CAPACITOR_ARRAY" || deviceName == "DIODE_ARRAY" || deviceName == "RESISTOR_ARRAY")
	{
		// Lib.Name ; NoPins ; Parameters
		parameters.Format("%-11s; %-8d; %d", "", m_lPins.GetCount(), m_lPins.GetCount());
	}
	else if (deviceName == "DIODE")
	{
		CString fBias, rBias, pTol, mTol;
		fBias.Format("%0.3f V", 0.0);
		rBias.Format("%0.3f V", 0.0);
		pTol.Format("%0.3f %%", m_cDevice.GetPlusTolerance());
		mTol.Format("%0.3f %%", m_cDevice.GetMinusTolerance());
		// For.Bias ; Rev.Bias ; HTol ; LTol
		parameters.Format("%-11s; %-11s; %-10s; %s", fBias, rBias, pTol, mTol);
	}
	else if (deviceName == "DIODE_LED")
	{
		CString fBias, rBias, pTol, mTol;
		fBias.Format("%0.3f V", 0.0);
		rBias.Format("%0.1f V", 1.5);
		pTol.Format("%0.3f %%", m_cDevice.GetPlusTolerance());
		mTol.Format("%0.3f %%", m_cDevice.GetMinusTolerance());
		// For.Bias ; Rev.Bias ; HTol ; LTol
		parameters.Format("%-11s; %-11s; %-10s; %s", fBias, rBias, pTol, mTol);
	}
	else if (deviceName == "DIODE_ZENER")
	{
		CString fBias, rBias, pTol, mTol;
		fBias.Format("%0.3f V", 0.0);
		//rBias.Format(m_cDevice.GetValue());
		pTol.Format("%0.3f %%", m_cDevice.GetPlusTolerance());
		mTol.Format("%0.3f %%", m_cDevice.GetMinusTolerance());
		// For.Bias ; Rev.Bias ; HTol ; LTol
		parameters.Format("%-11s; %-11s; %-10s; %s", fBias, m_cDevice.GetValue(), pTol, mTol);
	}
	else if (deviceName == "IC" || deviceName == "IC_DIGITAL" || deviceName == "IC_LINEAR")
	{
		// Lib.Name ; No. Pins
		parameters.Format("%11s; %d", "", m_lPins.GetCount());
	}
	else if (deviceName == "TRANSISTOR" || deviceName == "TRANSISTOR_NPN" || deviceName == "TRANSISTOR_PNP")
	{
		CString be, bc, ampl, pTol, mTol;
		be.Format("%0.3f mV", 0.0);
		bc.Format("%0.3f mV", 0.0);
		ampl.Format("%d", 0);
		pTol.Format("%0.3f %%", m_cDevice.GetPlusTolerance());
		mTol.Format("%0.3f %%", m_cDevice.GetMinusTolerance());
		// BE ; BC ; Ampl. ; HTol ; LTol
		parameters.Format("%-10s; %-10s; %-8s; %-10s; %s", be, bc, ampl, pTol, mTol);
	}
	else if (deviceName == "TRANSISTOR_FET_NPN" || deviceName == "TRANSISTOR_FET_PNP" || deviceName == "TRANSISTOR_MOSFET_NPN" || deviceName == "TRANSISTOR_MOSFET_PNP")
	{
		CString be, bc, ampl, pTol, mTol;
		be.Format("%0.3f V", 0.750);
		bc.Format("%0.3f V", 0.750);
		ampl.Format("%d", 0);
		pTol.Format("%0.3f %%", 10.0);
		mTol.Format("%0.3f %%", 10.0);
		// BE ; BC ; Ampl. ; HTol ; LTol
		parameters.Format("%-10s; %-10s; %-8s; %-10s; %s", be, bc, ampl, pTol, mTol);
	}
	else if (deviceName == "BATTERY"    	   || deviceName == "CONNECTOR"					|| deviceName == "CRYSTAL" ||
		 deviceName == "FILTER"						|| deviceName == "IC_LINEAR"					|| deviceName == "NO_TEST"	||
		 deviceName == "OSCILLATOR"				|| deviceName == "POTENTIOMETER"				|| deviceName == "POWER_SUPPLY" ||
		 deviceName == "RELAY"						|| deviceName == "SPEAKER"						||
		 deviceName == "SWITCH"						|| deviceName == "TEST_POINT"					|| deviceName == "TRANSFORMER" ||
		 deviceName == "TRANSISTOR_ARRAY"		|| deviceName == "TRANSISTOR_SCR"			||	deviceName == "TRANSISTOR_TRIAC"	 ||		
		 deviceName == "VOLTAGE_REGULATOR")
	{
		parameters.Format("%11s; %d", "", m_lPins.GetCount());
	}
	else if (deviceName == "FUSE" || deviceName == "JUMPER")
   {
      parameters.Format("10.000 Ohm");
   }

   
   // Case 2012, item 5, use SubClass for first field if set, otherwise
   // use "calculated" dtModel value (like before)
   buf.Format("%-25s; %-11s; %-5s; %s", m_sSubClass.IsEmpty() ? dtModel : m_sSubClass, GetPartNumber(), dtDeviceName, parameters);

	return buf;
}

CString CDTComponent::GetComponentString()
{
   CString deviceName = GetDeviceName();

	// we need to format the string based on the device type.  If there is no device type, then return and empty string
	if (deviceName.IsEmpty())
		return "";

	deviceName.MakeUpper();

	// logic to determine the remaining fields
	CString parameters;
	if (deviceName == "CAPACITOR" || deviceName == "INDUCTOR" || deviceName == "RESISTOR")
	{
		CPin *pin1 = m_lPins.GetHead(), *pin2 = m_lPins.GetTail();
		parameters.Format("%-13s; %s", check_name('n', pin1->GetNetName()), check_name('n', pin2->GetNetName()));
	}
	else if (deviceName == "DIODE" || deviceName == "DIODE_LED" || deviceName == "DIODE_ZENER")
	{
		CPin *pin1 = m_lPins.GetHead(), *pin2 = m_lPins.GetTail();
		CString anodeNetname, cathodeNetname;

		anodeNetname = pin1->GetNetName();
		cathodeNetname = pin2->GetNetName();

		if (pin1->GetType().MakeLower() == "cathode")
			cathodeNetname = pin1->GetNetName();
		if (pin2->GetType().MakeLower() == "anode")
			anodeNetname = pin2->GetNetName();

		parameters.Format("%-13s; %s", check_name('n', anodeNetname), check_name('n', cathodeNetname));
	}
	else if (deviceName == "IC" || deviceName == "IC_DIGITAL" || deviceName == "IC_LINEAR" ||
            deviceName == "NO_TEST" || deviceName == "CONNECTOR" || deviceName == "TRANSFORMER" || deviceName == "RELAY") // case 2012, treat these four like IC_LINEAR
		
	{
		parameters.Format("%-13d; %-7d", 0, m_lPins.GetCount());
		for (POSITION pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
			parameters += (CString)(" ; ") + (CString)check_name('n', pin->GetNetName());
		}
	}
	else if (deviceName == "CAPACITOR_POLARIZED" || deviceName == "CAPACITOR_TANTALUM")
	{
		CPin *pin1 = m_lPins.GetHead(), *pin2 = m_lPins.GetTail();
		parameters.Format("%-13s; %s", check_name('n', pin1->GetNetName()), check_name('n', pin2->GetNetName()));
	}
	else if (deviceName == "TRANSISTOR"	 || deviceName == "TRANSISTOR_NPN"	|| deviceName == "TRANSISTOR_PNP")
	{
		CString collectorNetName, baseNetName, emitterNetName;

      // Init to first three pins
      int pinnum;
      POSITION pos;
		for (pinnum = 1, pos=m_lPins.GetHeadPosition(); pinnum < 4 && pos!=NULL; pinnum++, m_lPins.GetNext(pos))
		{
         CPin *pin = m_lPins.GetAt(pos);
         if (pinnum == 1)
            collectorNetName = pin->GetNetName();
         if (pinnum == 2)
            baseNetName = pin->GetNetName();
         if (pinnum == 3)
            emitterNetName = pin->GetNetName();
      }

      // Now try to override by mapped pin type
		for (pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
			if (pin->GetType().MakeUpper() == "COLLECTOR")
				collectorNetName = pin->GetNetName();
			else if (pin->GetType().MakeUpper() == "BASE")
				baseNetName = pin->GetNetName();
			else if (pin->GetType().MakeUpper() == "EMITTER")
				emitterNetName = pin->GetNetName();
		}

		parameters.Format("%-13s; %-13s; %s", check_name('n', collectorNetName), check_name('n', baseNetName), check_name('n', emitterNetName));
	}
	else if (deviceName == "CAPACITOR_ARRAY" || deviceName == "DIODE_ARRAY" || deviceName == "RESISTOR_ARRAY")
	{
		parameters.Format("%-7d", m_lPins.GetCount());
		for (POSITION pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
			parameters += (CString)" ; " + (CString)check_name('n', pin->GetNetName());
		}
	}
   else if (
      deviceName == "BATTERY"						|| 
      deviceName == "FILTER"						|| deviceName == "OSCILLATOR"          ||
      deviceName == "POTENTIOMETER"				|| deviceName == "POWER_SUPPLY"			|| 
      deviceName == "SPEAKER"						|| deviceName == "SWITCH"					|| 
      deviceName == "TEST_POINT"             || deviceName == "TRANSISTOR_ARRAY"    ||
      deviceName == "TRANSISTOR_SCR"		   || deviceName == "TRANSISTOR_TRIAC"    ||
      deviceName == "VOLTAGE_REGULATOR"		|| deviceName == "DIODE"
      )
	{
		parameters.Format("%-7d", m_lPins.GetCount());
		for (POSITION pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
			parameters += " ; " + (CString)check_name('n', pin->GetNetName());
		}
	}
   
   else if (
      deviceName == "TRANSISTOR_FET_NPN"		|| deviceName == "TRANSISTOR_FET_PNP"	   || 
      deviceName == "TRANSISTOR_MOSFET_NPN"  || deviceName == "TRANSISTOR_MOSFET_PNP"  ||
      deviceName == "CRYSTAL"
      )
	{
		// Case 2210 - No pin count for -FET- components
		for (POSITION pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
         if (!parameters.IsEmpty())
            parameters += " ; ";
			parameters += (CString)check_name('n', pin->GetNetName());
		}
	}

	else if (deviceName == "FUSE" || deviceName == "JUMPER")
	{
		
		for (POSITION pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
         if (!parameters.IsEmpty())
            parameters += " , ";  // Case 2210 says this should be comma, not semicolon
			parameters += (CString)check_name('n', pin->GetNetName());
		}
	}

	else if (deviceName == "DIODE_ZENER")
	{
		
		for (POSITION pos=m_lPins.GetHeadPosition(); pos!=NULL; m_lPins.GetNext(pos))
		{
			CPin *pin = m_lPins.GetAt(pos);
         if (!parameters.IsEmpty())
            parameters += " ; ";
			parameters += (CString)check_name('n', pin->GetNetName());
		}
	}

   CString buf;
	buf.Format("%-16s; %-11s; %s", GetRefName(), GetPartNumber(), parameters);


	// format buf so that no line exceeds 120 characters
	int loopMax = (int)((double)buf.GetLength() / 120.);
	for (int count=0; count<loopMax; count++)
	{
		for (int revCount=(count+1)*120; true; revCount--)
		{
			if (buf[revCount] == ';')
			{
				buf.Insert(revCount, "\n");
				break;
			}
		}
	}

	return buf;
}




/******************************************************************************
* CDTComponentList
*/
CDTComponentList::CDTComponentList(CCEtoODBDoc &doc, FileStruct &file, CString(*keyGenFunc)(const CString))
	: CComponentList(doc, file, keyGenFunc)
{
}

bool CDTComponentList::AddComponent(DataStruct *data)
{
	CDTComponent *component = new CDTComponent(data, GetDoc(), GetFile(), this);
	AddTail(component);

	SetUnsorted();
	return true;
}



/******************************************************************************
* CMapDeviceToComponent::CMapDeviceToComponent
*/
CMapDeviceToComponent::CMapDeviceToComponent(CCEtoODBDoc &doc, FileStruct &file)
	: CTypedMapStringToPtrContainer<CDTComponentList*>()
	, m_doc(doc)
	, m_file(file)
{
}

void CMapDeviceToComponent::AddComponent(DataStruct *data)
{
	if (data->getDataType() != dataTypeInsert)
		return;

	InsertStruct *insert = data->getInsert();
	if (insert->getInsertType() != insertTypePcbComponent)
		return;

	Attrib *attr = NULL;
	CString refName = insert->getRefname(), deviceName;
	WORD devKW = m_doc.RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
	if (!data->lookUpAttrib(devKW, attr))
	{
		return;
		deviceName = "UNKNOWN";
	}
	else
		deviceName = get_attvalue_string(&m_doc, attr);

	CDTComponentList *compList = NULL;
	if (!Lookup(deviceName, compList))
	{
		compList = new CDTComponentList(m_doc, m_file, CDigitalTestWriter::SortKeyGenerator);
		compList->AddComponent(data);
		SetAt(deviceName, compList);
	}
	else
		compList->AddComponent(data);
}

void CMapDeviceToComponent::DumpToFile(CWriteFormat &file)
{
	POSITION pos = GetStartPosition();
	while (pos)
	{
		CString device;
		CDTComponentList *compList = NULL;
		GetNextAssoc(pos, device, compList);

		file.writef("Device : %s\n", device);

		file.pushHeader("   ");
		compList->DumpToFile(file);
		file.popHeader();
	}
}



/******************************************************************************
* CDTProbe::CDTProbe
*/
CDTProbe::CDTProbe(DataStruct *probe, CCEtoODBDoc &doc, FileStruct &file)
	: CObject()
	, m_doc(doc)
	, m_file(file)
	, m_pProbe(probe)
	, m_bPlaced(false)
{
	if (probe == NULL)
		return;
	
	if (probe == NULL || probe->getDataType() != dataTypeInsert)
		return;

	// make sure we have a probe
	InsertStruct *probeInsert = probe->getInsert();
	if (probeInsert->getInsertType() != insertTypeTestProbe)
		return;

	m_sRefName = probeInsert->getRefname();
	m_iProbeNumber = atoi(m_sRefName);

	// check if the probe is placed
	Attrib *attr = NULL;
	WORD probePlacementKW = m_doc.RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
	if (!probe->lookUpAttrib(probePlacementKW, attr))
		return;

	CString placement = get_attvalue_string(&m_doc, attr);
	if (placement == "Placed")
		m_bPlaced = true;
}

void CDTProbe::updateInsertRefname(int probeNum)
{
	DataStruct *probeData = this->getProbeData();
	if (probeData != NULL)
	{
		CString probeNumStr;
		probeNumStr.Format("%d", probeNum);
		InsertStruct *probeInsert = probeData->getInsert();
		if (probeInsert != NULL) 
			probeInsert->setRefname(probeNumStr);
		
		// Regular attrib
		//m_doc.SetUnknownAttrib(&probeData->getAttributesRef(), ATT_REFNAME, probeNumStr, SA_OVERWRITE, NULL);

		// Visible attrib
		// Add probe number as REFNAME attribute
		WORD refnameKW = m_doc.RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);
		double diameter = getProbeDiameter();
		if (diameter == 0.0) diameter = m_doc.convertToPageUnits(pageUnitsInches, 0.080); // a nominal drill size
		CreateTestProbeRefnameAttr(&m_doc, probeData, probeNumStr, diameter);

	}
}

double CDTProbe::getProbeDiameter()
{
	if (!this->getProbeData() || !this->getProbeData()->getInsert())
		return 0.0;

	int insertedProbeBlockNum = this->getProbeData()->getInsert()->getBlockNumber();
	BlockStruct *block = m_doc.getBlockAt(insertedProbeBlockNum);
	if (!block)
		return 0.0;
	
	POSITION pos = block->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *geomData = block->getNextDataInsert(pos);
		if (geomData && geomData->getInsert())
		{
			int insertedGeomBlockNum = geomData->getInsert()->getBlockNumber();
			BlockStruct *insertedGeomBlock = m_doc.getBlockAt(insertedGeomBlockNum);
			if (insertedGeomBlock != NULL &&
				insertedGeomBlock->getBlockType() == blockTypeDrillHole)
			{
				double holeSize = insertedGeomBlock->getToolSize();
				return holeSize;	
			}
		}
	}

	return 0.0;
}

void CDTProbe::DumpToFile(CWriteFormat &file)
{
	file.writef("RefName : %s\n", m_sRefName);
	file.writef("NetType : %d\n", m_iProbeNumber);
	file.writef("Placement : %s\n", m_bPlaced?"Placed":"Unplaced");
}




/******************************************************************************
* CDTProbeList::DumpToFile
*/
void CDTProbeList::DumpToFile(CWriteFormat &file)
{
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{		
		CDTProbe *probe = GetAt(pos);
		file.writef("Probe : %s\n", probe->GetRefName());

		file.pushHeader("   ");
		probe->DumpToFile(file);
		file.popHeader();
	}
}




/******************************************************************************
* CDTNet::CDTNet
*/
CDTNet::CDTNet(NetStruct *net, CCEtoODBDoc &doc, FileStruct &file, CDigitalTestSettings &dtSettings)
	: CObject()
	, m_doc(doc)
	, m_file(file)
	, m_pNet(net)
	, m_digitalNet(false)

{
	if (net == NULL)
		return;

	m_sName = net->getNetName();
   m_eType = probeableNetTypeSignal; // Default, in case no attrib is set

	Attrib *attr = NULL;
	WORD powerNetKW = doc.RegisterKeyWord(ATT_POWERNET, 0, valueTypeNone);
	if (net->lookUpAttrib(powerNetKW, attr))
		m_eType = probeableNetTypePower;

	WORD netTypeKW = doc.RegisterKeyWord(ATT_NET_TYPE, 0, valueTypeString);
	if (net->lookUpAttrib(netTypeKW, attr))
	{
		CString netType = get_attvalue_string(&doc, attr);
		if (netType == "Signal")
			m_eType = probeableNetTypeSignal;
		else if (netType == "Power")
			m_eType = probeableNetTypePower;
		else if (netType == "Ground")
			m_eType = probeableNetTypeGround;
	}

	CString digitalAttrName = dtSettings.getDigitalAttrName();
	if (!digitalAttrName.IsEmpty())
	{
		POSITION cpPos = net->getHeadCompPinPosition();
		while (cpPos)
		{
			CompPinStruct *compPin = net->getNextCompPin(cpPos);
			CString refdes = compPin->getRefDes();
			DataStruct *probedComponent = findComponentData(refdes);
			Attrib *attrib;
			if (attrib = is_attvalue(&m_doc, probedComponent->getAttributesRef(), digitalAttrName, 0))
			{
				CString attrVal = attrib->getStringValue();
				if (dtSettings.isDigital(attrVal))
				{
					m_digitalNet = true;
					break;
				}
			}

		}
	}


	m_lProbes.RemoveAll();
}

DataStruct *CDTNet::findComponentData(CString refdes)
{
	POSITION pos = m_file.getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = m_file.getBlock()->getDataList().GetNext(pos);

		if (data->getDataType() == T_INSERT)
		{
			InsertStruct *insert = data->getInsert();
			CString insertname = insert->getRefname();
			if (data->getInsert()->getRefname().Compare(refdes) == 0)
			{
				return data;
			}
		}
	}

	return NULL;
}

void CDTNet::DumpToFile(CWriteFormat &file)
{
   file.writef("NetName : %s  (checked: %s)\n", m_sName, check_name('n', m_sName));

	CString netType;
	switch (m_eType)
	{
	case probeableNetTypeSignal:
			netType = "Signal";
			break;
	case probeableNetTypePower:
			netType = "Power";
			break;
	case probeableNetTypeGround:
			netType = "Ground";
			break;
	}
	file.writef("NetType : %s\n", netType);

	m_lProbes.DumpToFile(file);
}

bool CDTNet::AddProbe(DataStruct *probe)
{
	if (probe == NULL || probe->getDataType() != dataTypeInsert)
		return false;

	// make sure we have a probe
	InsertStruct *probeInsert = probe->getInsert();
	if (probeInsert->getInsertType() != insertTypeTestProbe)
		return false;

	// make sure the probe is on this net
	Attrib *attr = NULL;
	WORD netNameKW = m_doc.RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
	if (!probe->lookUpAttrib(netNameKW, attr))
		return false;
	CString netName = get_attvalue_string(&m_doc, attr);
	if (netName != m_sName)
		return false;

	// get the probe number and add it to the list
	CDTProbe *dtProbe = new CDTProbe(probe, m_doc, m_file);
	return (m_lProbes.AddTail(dtProbe) != NULL);
}



/******************************************************************************
* CDTNetList::DumpToFile
*/
void CDTNetList::DumpToFile(CWriteFormat &file)
{
	POSITION pos = GetStartPosition();
	while (pos)
	{
		CString netName;
		CDTNet *dtNet = NULL;
		GetNextAssoc(pos, netName, dtNet);

      file.writef("Net : %s  (checked: %s)\n", netName, check_name('n', netName));

		file.pushHeader("   ");
		dtNet->DumpToFile(file);
		file.popHeader();
	}
}

//--------------------------------------------------------------------------

CDigitalTestSettings::CDigitalTestSettings()
{
	m_digAttrName = "DEVICETYPE";
	m_digAttrValues.RemoveAll();
	m_analogLow = 0;
	m_analogHigh = 0;
	m_digitalHigh = 0;
   m_exportNoTest = true; // case 2175, true is backward compatible setting

	void* voidPtr = NULL;

   CString settingsFile( getApp().getExportSettingsFilePath("DigitalTest.out") );

   FILE  *fp;

   if ((fp = fopen(settingsFile,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      MessageBox(NULL, tmp, "Digital Test Writer Settings", MB_OK | MB_ICONHAND);
	}
	else
	{
      char  line[255];

		while (fgets(line,255,fp))
		{
         char *lp = get_string(line, " \t\n");
         if (lp == NULL)
            continue;

			if (lp[0] == '.')
			{
				if (STRCMPI(lp,".DIG_ATTR") == 0)
				{
					if ((lp = get_string(NULL," \t\n")) != NULL)
						m_digAttrName = lp;
				}
				else if (STRCMPI(lp,".DIG_VAL") == 0)
				{
					if ((lp = get_string(NULL," \t\n")) != NULL)
					{
						CString key = lp;
						m_digAttrValues.SetAt(key.MakeUpper(), voidPtr);
					}
				}
				else if (STRCMPI(lp,".ANALOG_LOW") == 0)
				{
					if ((lp = get_string(NULL," \t\n")) != NULL)
						m_analogLow = atoi(lp);
				}
				else if (STRCMPI(lp,".ANALOG_HIGH") == 0)
				{
					if ((lp = get_string(NULL," \t\n")) != NULL)
						m_analogHigh = atoi(lp);
				}
				else if (STRCMPI(lp,".DIGITAL_HIGH") == 0)
				{
					if ((lp = get_string(NULL," \t\n")) != NULL)
						m_digitalHigh = atoi(lp);
				}
				else if (STRCMPI(lp,".EXPORT_NO_TEST") == 0)
				{
					if ((lp = get_string(NULL," \t\n")) != NULL)
						m_exportNoTest = (toupper(lp[0]) == 'Y');
				}
            else if (getUserDefMsg().isKeyword(lp))
            {
               CString kw(lp);

				   lp = get_string(NULL, " \t\n");
				   if (lp == NULL)
					   continue;

               getUserDefMsg().append(kw, lp);
            }

			}
		}

		fclose(fp);
	}
}

CDigitalTestSettings::~CDigitalTestSettings()
{
	m_digAttrValues.RemoveAll();
}

bool CDigitalTestSettings::isDigital(CString str)
{
	void *voidPtr = NULL;
	
	return m_digAttrValues.Lookup(str.MakeUpper(), voidPtr) ? true : false;
}

/******************************************************************************
* CDigitalTestWriter::CDigitalTestWriter
*/
CDigitalTestWriter::CDigitalTestWriter(FileStruct &file, CCEtoODBDoc &doc) : CObject()
	, m_doc(doc)
	, m_file(file)
	, m_mapDev2Comp(doc, file)
{
   CString checkFile( getApp().getSystemSettingsFilePath("DigitalTest.chk") );
   check_init(checkFile);
}

CDigitalTestWriter::~CDigitalTestWriter()
{
   // if (check_report(somefile)) display_error++;
   check_deinit();
}



int CDigitalTestWriter::gatherComponents()
{
	POSITION pos;
	for (pos = m_file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; m_file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = m_file.getBlock()->getAtData(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypePcbComponent)
			continue;

		m_mapDev2Comp.AddComponent(data);
	}


	pos = m_mapDev2Comp.GetStartPosition();
	while (pos)
	{
		CString deviceName;
		CDTComponentList *compList = NULL;
		m_mapDev2Comp.GetNextAssoc(pos, deviceName, compList);

		compList->Sort(CDigitalTestWriter::SortKeyGenerator);
	}

	return TRUE;
}

int CDigitalTestWriter::gatherNets()
{
	POSITION netPos = m_file.getHeadNetPosition();
	while (netPos)
	{
		NetStruct *net = m_file.getNextNet(netPos);

		CDTNet *dtNet = new CDTNet(net, m_doc, m_file, m_dtSettings);
		m_mapNetlist.SetAt(net->getNetName(), dtNet);
	}

	gatherProbes();
	
	return TRUE;
}

int CDigitalTestWriter::gatherProbes()
{
	WORD netNameKW = m_doc.RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);

	BlockStruct *fileBlock = m_file.getBlock();
	for (POSITION pos=fileBlock->getHeadDataInsertPosition(); pos!=NULL; fileBlock->getNextDataInsert(pos))
	{
		DataStruct *probeData = fileBlock->getAtData(pos);
		InsertStruct *probeInsert = probeData->getInsert();

		if (probeInsert->getInsertType() != insertTypeTestProbe)
			continue;

		Attrib *attr = NULL;
		if (!probeData->lookUpAttrib(netNameKW, attr))
			continue;

		CDTNet *dtNet = NULL;
		CString netName = get_attvalue_string(&m_doc, attr);
		if (!m_mapNetlist.Lookup(netName, dtNet))
			continue;

		dtNet->AddProbe(probeData);
	}
	return TRUE;
}

bool CDigitalTestWriter::renumberProbes()
{
	// Reset probe number starting values
	int nextAnalogProbeNum = m_dtSettings.getAnalogLow();
	int nextDigitalProbeNum = m_dtSettings.getDigitalHigh();

	// If unplaced probes overflow the range limits, it isn't fatal.
	// Number them above the max digital probe, but only if there is overflow.
	int nextUnplacedProbeNum = m_dtSettings.getDigitalHigh() + 1;

	// We do two passes here.
	// First pass, renumber the placed probes.
	// Second pass, renumber the unplaced probes
	for (int pass = 1; pass < 3; pass++)
	{
		POSITION netlistPos = m_mapNetlist.GetStartPosition();
		while (netlistPos)
		{
			CString netName;
			CDTNet *dtNet = NULL;
			m_mapNetlist.GetNextAssoc(netlistPos, netName, dtNet);

			if (dtNet->GetCount_ProbeNumbers() > 0)
			{
				// Renumber all probes on net, regardless of if they will be visible in output
				POSITION probePos = dtNet->GetHeadPosition_ProbeNumbers();
				while (probePos != NULL)
				{
					CDTProbe *probe = dtNet->GetNext_ProbeNumbers(probePos);
					if ( (pass == 1 && probe->IsPlaced()) ||
						(pass == 2 && !probe->IsPlaced()))
					{
						int probeNum;
						if (dtNet->isDigital())
							probeNum = nextDigitalProbeNum--;
						else
							probeNum = nextAnalogProbeNum++;

						if (!probeIsInRange(probeNum, dtNet->isDigital()))
							if (pass == 1)
							{
								CString msg;
								msg.Format("Probe number range error, too many %s probes.", dtNet->isDigital() ? "digital" : "analog");
								ErrorMessage(msg, "Digital Test Writer");
								return false;	// fatal error for placed probes
							}
							else 
							{
								// Issue this message only once, regardless of how many overflows occur
								if (nextUnplacedProbeNum == (m_dtSettings.getDigitalHigh() + 1)) // still at initial value
								{
									CString msg = "Probe number range error caused by unplaced probes.\n";
									msg += "These will be numbered above the digital probes.\n";
									msg += "Processing shall continue.";
									ErrorMessage(msg, "Digital Test Writer");
								}
								probeNum = nextUnplacedProbeNum++; // overflow numbering, unplaced  probes only
							}

						// Still here? Probe range is okay.
						probe->setProbeNumber(probeNum);

					}
				}
			}
		}
	}

	// Still here? Then all generated probe numbers were in range.
	// Case 1631: Update Refdes on probe insert and in attribute.
	POSITION netlistPos = m_mapNetlist.GetStartPosition();
	while (netlistPos)
	{
		CString netName;
		CDTNet *dtNet = NULL;
		m_mapNetlist.GetNextAssoc(netlistPos, netName, dtNet);

		if (dtNet->GetCount_ProbeNumbers() > 0)
		{
			POSITION probePos = dtNet->GetHeadPosition_ProbeNumbers();
			while (probePos != NULL)
			{
				CDTProbe *probe = dtNet->GetNext_ProbeNumbers(probePos);
				probe->updateInsertRefname(probe->GetProbeNumber());
			}
		}
	}

	return true;
}

bool CDigitalTestWriter::probeIsInRange(int probeNum, bool digital)
{
	if (digital)
	{
		if (probeNum > m_dtSettings.getDigitalHigh() ||
			probeNum <= m_dtSettings.getAnalogHigh())
		{
			return false;
		}
	}
	else
	{
		if (probeNum < m_dtSettings.getAnalogLow() ||
			probeNum > m_dtSettings.getAnalogHigh())
		{
			return false;
		}
	}

	return true;
}

int CDigitalTestWriter::writeBoard(CWriteFormat &file)
{
	file.writef("$BOARD\n");
	file.writef("\n");
	file.writef("\n");

	return TRUE;
}

int CDigitalTestWriter::writePartNumbers(CWriteFormat &file)
{
   CMapStringToPtr map;
   map.RemoveAll();
   void* voidPtr = NULL;

	file.writef("$PART NUMBER\n");
	file.writef("\n");

	POSITION pos = m_mapDev2Comp.GetStartPosition();
	while (pos != NULL)
	{
		CString deviceName;
		CDTComponentList *compList = NULL;
		m_mapDev2Comp.GetNextAssoc(pos, deviceName, compList);

      // Case 2175, honor switch to control export of NO_TEST section.
      // if (export no test is yes  OR  this is not no test section) then...
      if (getSettings()->getExportNoTest() || deviceName.CompareNoCase("NO_TEST") != 0)
      {
         file.writef("// %s\n", deviceName.MakeUpper());
         file.writef("// %s\n", CDigitalTestWriter::GetPartnumberHeader(deviceName));
         for (POSITION compPos=compList->GetHeadPosition(); compPos!=NULL; compList->GetNext(compPos))
         {
            CDTComponent *comp = (CDTComponent*)compList->GetAt(compPos);
            CString partNumber = comp->GetPartNumber();

            if (!map.Lookup(partNumber, voidPtr))
            {
               CString partNumberString = comp->GetPartNumberString();
               file.writef("%s\n", partNumberString);

               map.SetAt(partNumber, (void*)NULL);
            }
         }

         file.writef("\n");
      }
	}

	map.RemoveAll();

	return TRUE;
}

int CDigitalTestWriter::writeComponents(CWriteFormat &file)
{
	file.writef("$COMPONENT\n");
	file.writef("\n");

	POSITION pos = m_mapDev2Comp.GetStartPosition();
	while (pos!=NULL)
	{
		CString deviceName;
		CDTComponentList *compList = NULL;
		m_mapDev2Comp.GetNextAssoc(pos, deviceName, compList);

      // Case 2175, honor switch to control export of NO_TEST section.
      // if (export no test is yes  OR  this is not no test section) then...
      if (getSettings()->getExportNoTest() || deviceName.CompareNoCase("NO_TEST") != 0)
      {
         file.writef("// %s\n", deviceName.MakeUpper());
         file.writef("// %s\n", CDigitalTestWriter::GetComponentHeader(deviceName));

         for (POSITION compPos=compList->GetHeadPosition(); compPos!=NULL; compList->GetNext(compPos))
         {
            CDTComponent *comp = (CDTComponent*)compList->GetAt(compPos);
            CString compString = comp->GetComponentString();
            file.writef("%s\n", compString);
         }

         file.writef("\n");
      }
	}

	return TRUE;
}

int CDigitalTestWriter::writePower(CWriteFormat &file)
{
	file.writef("$POWER\n");
	file.writef("\n");

	POSITION pos = m_mapNetlist.GetStartPosition();
	while (pos)
	{
		CString netName;
		CDTNet *dtNet = NULL;
		m_mapNetlist.GetNextAssoc(pos, netName, dtNet);

		if (dtNet->GetType() != probeableNetTypePower && dtNet->GetType() != probeableNetTypeGround)
			continue;

		if (dtNet->GetCount_ProbeNumbers() > 0)
		{
			CDTProbe *probe = dtNet->GetHead_ProbeNumbers();
			file.writef("%s : %d\n", check_name('n', netName), probe->IsPlaced()?probe->GetProbeNumber():0);
		}
		else
			file.writef("%s ;\n", check_name('n', netName));

	}

	file.writef("\n");

	return TRUE;
}


int CDigitalTestWriter::writeMessage(CWriteFormat &file)
{
	file.writef("$MESSAGE\n");
	file.writef("\n");

	file.writef("// %-15s%s\n", "Comp Name", "Message");

   // Case 2012 says don't write anything in this section except the comment line above.
   // Case 2175 says support the user defined message and implemented for CKT and 3070 writers.
   // Only output a message if the message format is defined in .out file, i.e. there is
   // no default message. This should appease both cases 2012 and 2175.

   if (!getSettings()->getUserDefMsg().isEmpty())
   {
      POSITION pos = m_mapDev2Comp.GetStartPosition();
      while (pos!=NULL)
      {
         CString deviceName;
         CDTComponentList *compList = NULL;
         m_mapDev2Comp.GetNextAssoc(pos, deviceName, compList);

         for (POSITION compPos=compList->GetHeadPosition(); compPos!=NULL; compList->GetNext(compPos))
         {
            CDTComponent *comp = (CDTComponent*)compList->GetAt(compPos);

            // Old message, when case 2012 came along and said shot off messages
            //file.writef("   %-13s; %s=%s\n", comp->GetRefName(), comp->GetRefName(), comp->GetPartNumber());
            
            // Case 2175 style, user defined messages
            const DataStruct& data = comp->GetParentData();
            CString msg = getSettings()->getUserDefMsg().format(&m_doc, &data);
            file.writef("   %-13s; %s=%s\n", comp->GetRefName(), comp->GetRefName(), msg);
         }
      }
   }

	file.writef("\n");

	return TRUE;
}

int CDigitalTestWriter::writeNetwork(CWriteFormat &file)
{
	file.writef("$NETWORK\n");
	file.writef("\n");

	file.writef("// %-15s%s\n", "Net Name", "Channel#");
	POSITION pos = m_mapNetlist.GetStartPosition();
	while (pos)
	{
		CString netName;
		CDTNet *dtNet = NULL;
		m_mapNetlist.GetNextAssoc(pos, netName, dtNet);

      // Case 2175, output all nets, even if not probed. Write 0 for probe on non-probed net.
      int probenumber = 0; // default, will be used for non-probed nets and nets with unplaced probes

      if (dtNet->GetCount_ProbeNumbers() > 0)
      {
         CDTProbe *probe = dtNet->GetHead_ProbeNumbers();
         if (probe != NULL && probe->IsPlaced())
            probenumber = probe->GetProbeNumber();
      }
      //Skip Single Die Pin Net
      if(!dtNet->GetNet()->IsSingleDiePinNet())
         file.writef("   %-13s; %d\n", check_name('n', netName), probenumber);
      else
         m_logFile.WriteString("Net: %s - Skipped SinglePin Net for Die component.\n", check_name('n', netName));
	}

	file.writef("\n");

	return TRUE;
}

int CDigitalTestWriter::writeFile(CString filename)
{
	CStdioFileWriteFormat outFile(2048);
	CExtFileException err;
	if (!outFile.open(filename, &err))
	{
		CString errMsg;
		char exceptionMsg[255];
		if (err.m_cause != err.none && err.GetErrorMessage(exceptionMsg, 255))
		{
			errMsg.Format("Error writing file!!\n\nFile could not be open due to the following reason:\n   %s", exceptionMsg);
			m_logFile.WriteString("Error: Could not open file for write {%s}\n", exceptionMsg);
		}
		else
			errMsg = "Error writing file!!\n\nFile could not be opened.";
		ErrorMessage(errMsg, "Digital Test Writer");

		return FALSE;
	}

	writeBoard(outFile);
	writePartNumbers(outFile);
	writeComponents(outFile);
	writePower(outFile);
	writeMessage(outFile);
	writeNetwork(outFile);

	outFile.close();

	return TRUE;
}

int CDigitalTestWriter::dumpToFile(CString filename)
{
   CString debugFilename= filename.IsEmpty()?getApp().getSystemSettingsFilePath("digitaltest.dbg"):filename;

	CStdioFileWriteFormat outFile;
	CExtFileException err;
	if (!outFile.open(debugFilename, &err))
	{
		char exceptionMsg[255];
		if (err.m_cause != err.none && err.GetErrorMessage(exceptionMsg, 255))
			m_logFile.WriteString("Error: Could not open file for write {%s}\n", exceptionMsg);
		else
			m_logFile.WriteString("Error writing debug file!!\n\nFile could not be opened.\n");

		return FALSE;
	}

	outFile.writef("Digital Test Debug Output\n");
	outFile.writef("Generated by CAMCAD v.%s\n\n", getApp().getVersionString());
	
	outFile.writef("Map of Components by Device Type\n");
	outFile.pushHeader("   ");
	m_mapDev2Comp.DumpToFile(outFile);
	outFile.popHeader();
	outFile.writef("\n");

	outFile.writef("List of Nets\n");
	outFile.pushHeader("   ");
	m_mapNetlist.DumpToFile(outFile);
	outFile.popHeader();

	outFile.close();

	return TRUE;
}

int CDigitalTestWriter::WriteFiles(CString filename)
{
	CTime t;
	bool display_error = false;

	CString logFilename = GetLogfilePath("fixture.log");
	if (m_logFile.Open(logFilename, CFile::modeCreate|CFile::modeWrite))   // rewrite file
   {
		m_logFile.WriteString("Filename: %s\n", logFilename);
		CTime t;
		t = t.GetCurrentTime();
		m_logFile.WriteString("%s\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));
   }

	gatherComponents();
	gatherNets();
	if (renumberProbes())  // case 1631: if renumber fails, abort output
		writeFile(filename);

#ifdef _DEBUG
	dumpToFile();
#endif

	t = t.GetCurrentTime();
   m_logFile.WriteString("%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));
   m_logFile.WriteString("Logfile closed\n");
   m_logFile.Close();

	if (display_error)
		Logreader(logFilename);

	return TRUE;
}

CString CDigitalTestWriter::SortKeyGenerator(const CString inStr)
{
	// create sort name
	CString temp, buf, name = inStr;
	bool alpha = true, lastAlpha = true;

	name.MakeUpper();
	for (int i=0; i<name.GetLength(); i++)
	{
		if (isalpha(name[i]))
			alpha = true;
		else if (isdigit(name[i]))
			alpha = false;
		else
			continue;

		if (i == 0)
		{
			lastAlpha = alpha;
			temp = name[i];
			continue;
		}

		if (lastAlpha != alpha)
		{
			if (lastAlpha)
			{
				// add string
				int len = temp.GetLength();
				temp = temp + (CString)"AAAAAAAAAAAAAAA";
				temp.Delete(15, len);
				buf += temp;
			}
			else
			{
				// add number
				temp = (CString)"000000000000000" + temp;
				temp.Delete(0, temp.GetLength() - 15);
				buf += temp;
			}

			temp.Empty();
		}
		temp += name[i];
		lastAlpha = alpha;
	}

	if (lastAlpha)
	{
		// add string
		int len = temp.GetLength();
		temp = temp + (CString)"AAAAAAAAAAAAAAA";
		temp.Delete(15, len);
		buf += temp;
	}
	else
	{
		// add number
		temp = (CString)"000000000000000" + temp;
		temp.Delete(0, temp.GetLength() - 15);
		buf += temp;
	}

	return buf;
}

CString CDigitalTestWriter::GetDigitalTestType(const CString deviceType)
{
	CString devType = deviceType;
	devType.MakeUpper();
	if (devType == "BATTERY"					|| devType == "CONNECTOR"					|| devType == "CRYSTAL" ||
		 devType == "FILTER"						|| devType == "IC_LINEAR" ||
		 devType == "NO_TEST"					|| devType == "OSCILLATOR"					|| devType == "POTENTIOMETER" ||
		 devType == "POWER_SUPPLY"				|| devType == "RELAY"						|| devType == "SPEAKER" ||
		 devType == "SWITCH"						|| devType == "TEST_POINT"					|| devType == "TRANSFORMER" ||
		 devType == "TRANSISTOR_ARRAY"		||
		 devType == "TRANSISTOR_SCR"			||	devType == "TRANSISTOR_TRIAC"			|| devType == "VOLTAGE_REGULATOR")
		return "ICA";
	else if (devType == "CAPACITOR")
		return "C";
	else if (devType == "DIODE_ZENER")
		return "Z";
	else if (devType == "CAPACITOR_ARRAY" || devType == "DIODE_ARRAY" || devType == "RESISTOR_ARRAY")
		return "CPL";
	else if (devType == "CAPACITOR_POLARIZED" || devType == "CAPACITOR_TANTALUM")
		return "CP";
	else if (devType == "DIODE")
		return "D";
	else if (devType == "DIODE_LED")
		return "LED";
	else if (devType == "FUSE" || devType == "JUMPER")
		return "CNT";
	else if (devType == "IC" || devType == "IC_DIGITAL")
		return "ICD";
	else if (devType == "INDUCTOR")
		return "L";
	else if (devType == "RESISTOR")
		return "R";
	else if (devType == "TRANSISTOR_NPN" || devType == "TRANSISTOR")
		return "TN";
	else if (devType == "TRANSISTOR_FET_NPN" || devType == "TRANSISTOR_MOSFET_NPN")
		return "NF";
	else if (devType == "TRANSISTOR_FET_PNP" || devType == "TRANSISTOR_MOSFET_PNP")
		return "PF";
	else if (devType == "TRANSISTOR_PNP")
		return "TP";

	return "";
}

CString CDigitalTestWriter::GetPartnumberHeader(const CString deviceType)
{
	CString devType = deviceType, buf;
	devType.MakeUpper();
	if (devType == "CAPACITOR"	|| devType == "CAPACITOR_POLARIZED" || devType == "CAPACITOR_TANTALUM" || devType == "INDUCTOR"	|| devType == "RESISTOR")
		buf.Format("%-24s%-13s%-7s%-13s%-12s%s", "Electrical Model", "Part Number", "Type", "Value", "HTol", "LTol");
	
   else if (devType == "CAPACITOR_ARRAY" || devType == "DIODE_ARRAY" || devType == "RESISTOR_ARRAY")
		buf.Format("%-24s%-13s%-7s%-13s%-10s%s", "Electrical Model", "Part Number", "Type", "Lib.Name", "NoPins", "Parameters");
	
   else if (devType == "DIODE" || devType == "DIODE_LED")
		buf.Format("%-24s%-13s%-7s%-13s%-13s%-12s%s", "Electrical Model", "Part Number", "Type", "For.Bias", "Rev.Bias", "HTol", "LTol");
	
   else if (devType == "DIODE_ZENER")
		buf.Format("%-24s%-13s%-7s%-13s%-13s%-12s%s", "Electrical Model", "Part Number", "Type", "For.Bias", "Zener Voltage ", "HTol", "LTol");
	
   else if (devType == "IC" || devType == "IC_DIGITAL" || devType == "IC_LINEAR")
		buf.Format("%-24s%-13s%-7s%-13s%s", "Electrical Model", "Part Number", "Type", "Lib.Name", "NoPins");
	
   else if (devType == "TRANSISTOR" || devType == "TRANSISTOR_NPN" || devType == "TRANSISTOR_PNP")
		buf.Format("%-24s%-13s%-7s%-12s%-12s%-10s%-12s%s", "Electrical Model", "Part Number", "Type", "BE", "BC", "Ampl", "HTol", "LTol");
	
   else if (devType == "TRANSISTOR_FET_NPN" || devType == "TRANSISTOR_FET_PNP" || devType == "Transistor_MosFET_NPN" || devType == "Transistor_MosFET_PNP")
		buf.Format("%-24s%-13s%-7s%-12s%-12s%-10s%-12s%s", "Electrical Model", "Part Number", "Type", "BE", "BC", "Ampl", "HTol", "LTol");
	
   else if (devType == "FUSE" || devType == "JUMPER")
      buf.Format("%-24s%-13s%-7s%-13s", "Electrical Model", "Part Number", "Type", "Value"); 

   //else if (devType == "BATTERY"					|| devType == "CONNECTOR"					|| devType == "CRYSTAL" ||
	//	 devType == "DIODE_ZENER"				|| devType == "FILTER"						||
	//	 devType == "NO_TEST"					|| devType == "OSCILLATOR"					|| devType == "POTENTIOMETER" ||
	//	 devType == "POWER_SUPPLY"				|| devType == "RELAY"						|| devType == "SPEAKER" ||
	//	 devType == "SWITCH"						|| devType == "TEST_POINT"					|| devType == "TRANSFORMER" ||
	//	 devType == "TRANSISTOR_ARRAY"		|| devType == "TRANSISTOR_FET_NPN"		||
	//	 devType == "TRANSISTOR_FET_PNP"		|| devType == "TRANSISTOR_MOSFET_NPN"	|| devType == "TRANSISTOR_MOSFET_PNP" ||
	//	 devType == "TRANSISTOR_SCR"			||	devType == "TRANSISTOR_TRIAC"			|| devType == "VOLTAGE_REGULATOR" ||
	//	 devType == "CAPACITOR_TANTALUM")
	//	return "ICA";


	return buf;
}

CString CDigitalTestWriter::GetComponentHeader(const CString deviceType)
{
	CString devType = deviceType, buf;
	devType.MakeUpper();
	if (devType == "CAPACITOR" || devType == "INDUCTOR" || devType == "RESISTOR")
		buf.Format("%-15s%-13s%-15s%s", "Comp Name", "Part Number", "A", "B");
	
   else if (devType == "CAPACITOR_ARRAY" || devType == "DIODE_ARRAY" || devType == "RESISTOR_ARRAY")
		buf.Format("%-15s%-13s%s", "Comp Name", "Part Number", "NoPins");
	
   else if (devType == "CAPACITOR_POLARIZED" || devType == "CAPACITOR_TANTALUM")
		buf.Format("%-15s%-13s%-15s%s", "Comp Name", "Part Number", "+", "-");
	
   else if (devType == "DIODE" || devType == "DIODE_LED")
		buf.Format("%-15s%-13s%-15s%s", "Comp Name", "Part Number", "Anode", "Cathode");
	
   else if (devType == "IC" || devType == "IC_DIGITAL" || devType == "IC_LINEAR")
		buf.Format("%-15s%-13s%-15s%s", "Comp Name", "Part Number", "Open Check", "NoPins");

   // Case 2012, item 3, No_Test, Connector, Transformer, Relay should be formatted  like
   // IC_LINEAR. When validated and confirmed, you can merge this into the IC_LINEAR block
   // above. Until then, these are separate for easy ID and their old locations in this 
   // code are just commented out for easy reference.
   else if (devType == "NO_TEST" || devType == "CONNECTOR" || devType == "TRANSFORMER" || devType == "RELAY")
		buf.Format("%-15s%-13s%-15s%s", "Comp Name", "Part Number", "Open Check", "NoPins");
		
   else if (devType == "TRANSISTOR_NPN" || devType == "TRANSISTOR_PNP")
		buf.Format("%-15s%-13s%-15s%-15s%s", "Comp Name", "Part Number", "Collector", "Base", "Emitter");
	
   else if (devType == "FUSE" || devType == "JUMPER" || devType == "DIODE_ZENER"       ||	
            devType == "TRANSISTOR_FET_NPN"		|| devType == "TRANSISTOR_FET_PNP"     ||
				devType == "TRANSISTOR_MOSFET_NPN"	|| devType == "TRANSISTOR_MOSFET_PNP"  ||
            devType == "CRYSTAL"
            )
		buf.Format("%-15s%-13s", "Comp Name", "Part Number");
	
   else if (devType == "BATTERY"						|| 
				devType == "FILTER"						|| devType == "OSCILLATOR"					|| 
            devType == "POTENTIOMETER"				|| devType == "POWER_SUPPLY" ||
				devType == "SPEAKER"						|| devType == "SWITCH" ||
				devType == "TEST_POINT"					|| devType == "TRANSISTOR" ||
				devType == "TRANSISTOR_ARRAY"			|| 
            devType == "TRANSISTOR_SCR"||
				devType == "TRANSISTOR_TRIAC"			|| devType == "VOLTAGE_REGULATOR"    
            )
		buf.Format("%-15s%-13s%s", "Comp Name", "Part Number", "NoPins");

	return buf;
}
// CDigitalTestSettingsDlg dialog

IMPLEMENT_DYNAMIC(CDigitalTestSettingsDlg, CDialog)
CDigitalTestSettingsDlg::CDigitalTestSettingsDlg(CDigitalTestWriter &dtWriter, CWnd* pParent /*=NULL*/)
	: CDialog(CDigitalTestSettingsDlg::IDD, pParent)
	, m_analogLow(0)
	, m_analogHigh(0)
	, m_digitalHigh(0)
{
	CDigitalTestSettings *dtSettings = dtWriter.getSettings();
	m_analogLow = dtSettings->getAnalogLow();
	m_analogHigh = dtSettings->getAnalogHigh();
	m_digitalHigh = dtSettings->getDigitalHigh();
}

CDigitalTestSettingsDlg::~CDigitalTestSettingsDlg()
{
}

void CDigitalTestSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ANALOG_LOW, m_analogLow);
	DDV_MinMaxInt(pDX, m_analogLow, 0, 1000000);

	DDX_Text(pDX, IDC_ANALOG_HIGH, m_analogHigh);
	DDV_MinMaxInt(pDX, m_analogHigh, 0, 1000000);

	DDX_Text(pDX, IDC_DIGITAL_HIGH, m_digitalHigh);
	DDV_MinMaxInt(pDX, m_digitalHigh, 0, 1000000);
}


BEGIN_MESSAGE_MAP(CDigitalTestSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDigitalTestSettingsDlg message handlers


void CDigitalTestSettingsDlg::OnBnClickedOk()
{
	UpdateData();

	if (m_analogLow >= m_analogHigh ||
		m_digitalHigh <= m_analogHigh)
	{
		ErrorMessage("Probe numbering range error.\nAnalog Low must be less than Analog High, Digital High must be greater than Analog High.", "Digital Test Writer");
	}
	else
	{
		OnOK();
	}
}
