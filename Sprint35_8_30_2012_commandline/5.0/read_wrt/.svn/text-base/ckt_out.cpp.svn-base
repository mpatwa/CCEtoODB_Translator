// $Header: /CAMCAD/5.0/read_wrt/ckt_out.cpp 50    6/17/07 8:55p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "ckt_out.h"
#include "net_util.h"
#include "find.h" 
#include "ck.h"
#include "CCEtoODB.h"
#include "PcbUtil.h"
#include "UserDefinedMessage.h"
#include "port.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CUserDefinedMessage *usrDefMsg = NULL;

static CUserDefinedMessage* GetUserDefMsg()
{
   if (usrDefMsg == NULL)
      usrDefMsg = new CUserDefinedMessage;

   return usrDefMsg;
}

static void ClearUserDefMsg()
{
   if (usrDefMsg != NULL)
      delete usrDefMsg;

   usrDefMsg = NULL;
}


void Teradyne228xCKT_WriteFiles(CString filename, CCEtoODBDoc &doc, FormatStruct *format, int page_units)
{
   // Open log file.
   CString localLogFilename;
   FILE *logFp = getApp().OpenOperationLogFile("ckt.log", localLogFilename);
   if (logFp == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(logFp, "Teradyne 228x CKT");

	CKTWriter cktWriter(doc, format, page_units, logFp);

	cktWriter.WriteFile(filename);

   ClearUserDefMsg();

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(logFp);
   }
}



/******************************************************************************
* CKTInfoNetNameItem
*/
CKTInfoNetNameItem::CKTInfoNetNameItem(CCEtoODBDoc* doc, DataStruct* data, CString info, CString netName)
{
   m_pDoc = doc;
	m_pData = data;
	m_sInfo = info;
	m_sCamcadNetName = netName;

   // This depends on m_sCamcadNetName and m_pDoc already being set
   m_sCktValidNetName = check_name('n', this->GetCKTNetName(), FALSE);

   // This depends on m_pData already being set
   m_bTool = SetTool();
}

CKTInfoNetNameItem::CKTInfoNetNameItem(const CKTInfoNetNameItem &info)
{
	m_pData = info.m_pData;
	m_sInfo = info.m_sInfo;
	m_sCamcadNetName = info.m_sCamcadNetName;
   m_sCktNetName = info.m_sCktNetName;
   m_sCktValidNetName = info.m_sCktValidNetName;
	m_pDoc = info.m_pDoc;
	m_bTool = info.m_bTool;
}

CKTInfoNetNameItem &CKTInfoNetNameItem::operator=(const CKTInfoNetNameItem &other)
{
	if (this != &other)
	{
		m_pData = other.m_pData;
		m_sInfo = other.m_sInfo;
		m_sCamcadNetName = other.m_sCamcadNetName;
      m_sCktNetName = other.m_sCktNetName;
      m_sCktValidNetName = other.m_sCktValidNetName;
		m_pDoc = other.m_pDoc;
		m_bTool = other.m_bTool;
	}

	return *this;
}

CString CKTInfoNetNameItem::GetCKTNetName() const
{
   // Case dts0100407106
   // Other than some one-liner adjustments to some func names spread out in the importer, this
   // function embodies the essence of case dts0100407106 for making adjustment to net names
   // for power and ground nets.

   if (this->m_sCktNetName.IsEmpty())
   {
      m_sCktNetName = this->m_sCamcadNetName;

      if (this->m_pDoc != NULL)
      {
         NetStruct *net = FindNet(m_pDoc->getFileList().GetOnlyShown(blockTypePcb), this->m_sCamcadNetName);
         if (net != NULL)
         {
            Attrib *attrib;
            WORD netTypeKw = m_pDoc->getStandardAttributeKeywordIndex(standardAttributeNetType);
            if (net->lookUpAttrib(netTypeKw, attrib))
            {
               CString netType = attrib->getStringValue();
		         if (netType.CompareNoCase("POWER") == 0)
                  m_sCktNetName = "VCC_" + m_sCamcadNetName;
		         else if (netType.CompareNoCase("GROUND") == 0)
                  m_sCktNetName = "GND_" + m_sCamcadNetName;
            }
         }
      }
   }


   return m_sCktNetName;
}

BOOL CKTInfoNetNameItem::SetTool()
{
	if (m_pData != NULL && m_pData->getInsert()->getInsertType() == insertTypeDrillHole)
		return TRUE;
	
	return FALSE;
}

CString CKTInfoNetNameItem::GetSortKey()
{
   // It item has DataStruct and it is insert type pin then sort key
   // is the pin's sortable refname.
   // Otherwise sort key is info string.

   if (m_pData != NULL && m_pData->isInsertType(insertTypePin))
      return m_pData->getInsert()->getSortableRefDes();

   return GetInfo();
}

/******************************************************************************
* CKTBoardItem
*/
CKTBoardItem::CKTBoardItem(const CKTBoardItem &item)
{
	m_boardItemLogFp = item.m_boardItemLogFp;
	m_sRefName = item.m_sRefName;
	m_sSortKey = item.m_sSortKey;
	m_sCKTDeviceID = item.m_sCKTDeviceID;
	m_sDeviceType = item.m_sDeviceType;
	m_sValue = item.m_sValue;
	m_cvValue = item.m_cvValue;
	m_sPlusTolerance = item.m_sPlusTolerance;
	m_sMinusTolerance = item.m_sMinusTolerance;
	m_eItemType = item.m_eItemType;
	m_sMessage = item.m_sMessage;
	m_sPlacement = item.m_sPlacement;
	m_data       = item.m_data;
	m_bCapOpen   = item.m_bCapOpen;

	m_pinList.RemoveAll();
	POSITION pos = item.m_pinList.GetHeadPosition();
	while (pos)
		m_pinList.AddTail(item.m_pinList.GetNext(pos));
}

CKTBoardItem::CKTBoardItem(DataStruct *data, CTMatrix mat, CCEtoODBDoc &doc, FILE *logFp, CString msgAttrName, CString refName)
{
	m_boardItemLogFp = logFp;

	m_eItemType = cktBoardItemTypeUnknown;
	m_pinList.RemoveAll();

	if (data->getInsert() == NULL)
		return;
	InsertStruct *insert = data->getInsert();
	BlockStruct *subBlock = doc.getBlockAt(insert->getBlockNumber());
	Attrib *attrib = NULL;

   m_sRefName = refName;
	if (m_sRefName.IsEmpty())
		m_sRefName = data->getInsert()->getRefname();
	m_sSortKey = generateSortKey(m_sRefName);

	if (insert->getInsertType() != insertTypePcbComponent && insert->getInsertType() != insertTypeVia
      && insert->getInsertType() != insertTypeBondPad)
		return;

	WORD valueKw = doc.RegisterKeyWord(ATT_VALUE, 0, VT_STRING);
	if (data->lookUpAttrib(valueKw, attrib))
	{
		m_sValue = get_attvalue_string(&doc, attrib);
		m_cvValue.SetValue(get_attvalue_string(&doc, attrib));
	}
	// only take the numeric value
	int value = atoi(m_sValue);
	m_sValue.Format("%i", value);

	WORD plusTolKw = doc.RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);
	WORD minusTolKw = doc.RegisterKeyWord(ATT_MINUSTOLERANCE, 0, VT_DOUBLE);
	WORD tolKw = doc.RegisterKeyWord(ATT_TOLERANCE, 0, VT_DOUBLE);
	WORD capOpenKW = doc.RegisterKeyWord(ATT_CAP_OPENS, 0, valueTypeString);
	if (data->lookUpAttrib(plusTolKw, attrib))
		m_sPlusTolerance = get_attvalue_string(&doc, attrib);
	if (data->lookUpAttrib(minusTolKw, attrib))
		m_sMinusTolerance = get_attvalue_string(&doc, attrib);
	if (m_sPlusTolerance.IsEmpty() && data->lookUpAttrib(tolKw, attrib))
		m_sPlusTolerance = get_attvalue_string(&doc, attrib);
	m_bCapOpen = FALSE;
	if (data->lookUpAttrib(capOpenKW, attrib))
	{
		if (!((CString)get_attvalue_string(&doc, attrib)).CompareNoCase("False"))
			m_bCapOpen = FALSE;
		else
			m_bCapOpen = TRUE;
	}
	if (insert->getPlacedBottom())
	{
		m_sPlacement = "bottom";
	}
	else
	{
		m_sPlacement = "top";
	}

	// Get the device ID, even if item ends up being a via
	WORD deviceKw = doc.RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
	WORD subClassKw = doc.RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);
	CString subClass;
	if (data->lookUpAttrib(deviceKw, attrib))
		m_sDeviceType = get_attvalue_string(&doc, attrib);
	if (data->lookUpAttrib(subClassKw, attrib))
		subClass = get_attvalue_string(&doc, attrib);


	m_data = data;
		
	m_sMessage = generateMessage(&doc, data, mat, msgAttrName, GetUserDefMsg());
	
	WORD cktNetnameKw = doc.RegisterKeyWord(ATT_CKT_NETNAME, 0, VT_STRING);
	if (insert->getInsertType() == insertTypePcbComponent)
	{
		m_eItemType = cktBoardItemTypeComponent;
		POSITION pos = subBlock->getHeadDataInsertPosition();

		while (pos)
		{
			DataStruct *subData = subBlock->getNextDataInsert(pos);

         if (!subData->isInsertType(insertTypePin))
				continue;

			NetStruct *net = NULL;
			CompPinStruct *cp = FindCompPin(doc.getFileList().GetOnlyShown(blockTypePcb), insert->getRefname(), subData->getInsert()->getRefname(), &net);

			if (cp == NULL)
				continue;

         CString infoStr;

         // Assign infoStr to mapped CKT special pin name if pin mapping is active for this part
         if (this->IsPinMappingSupportedByDevType(doc))
         {
            infoStr = GetCKTMappedPinName(doc, cp);

            if (infoStr.CompareNoCase(ATT_VALUE_NO_CONNECT) == 0)
            {
               // Skip no-connect pins
               continue;
            }
         }

         // If pin mapping did not set infoStr then default to pin's refname
         if (infoStr.IsEmpty())
            infoStr = subData->getInsert()->getRefname();

			// add the net name item (pin)
			CKTInfoNetNameItem netNameItem(&doc, subData, infoStr, net->getNetName());
         InsertPinItem(netNameItem);

			// add the CKT_NETNAME attribute on the net
			CString validName = netNameItem.GetCKTValidNetName();
			if (net != NULL)
            net->setAttrib(doc.getCamCadData(), cktNetnameKw, valueTypeString, validName.GetBuffer(0), attributeUpdateOverwrite, NULL);
		}
	}
	else if (insert->getInsertType() == insertTypeVia
      || insert->getInsertType() == insertTypeBondPad)
	{
		m_eItemType = cktBoardItemTypeVia;
		CString netName;

		WORD netNameKw = doc.RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);

		if (data->lookUpAttrib(netNameKw, attrib))
		{
			netName = get_attvalue_string(&doc, attrib);

			// add the net name item (pin)
			CKTInfoNetNameItem netNameItem(&doc, NULL, "1", netName);
         InsertPinItem(netNameItem);
			m_sCKTDeviceID = "UK1";

			// add the CKT_NETNAME attribute on the net
			NetStruct *net = FindNet(doc.getFileList().GetOnlyShown(blockTypePcb), netName);
			CString validName = netNameItem.GetCKTValidNetName();
			if (net)
            net->setAttrib(doc.getCamCadData(), cktNetnameKw, valueTypeString, validName.GetBuffer(0), attributeUpdateOverwrite, NULL);
			else if (m_boardItemLogFp != NULL)
				fprintf(m_boardItemLogFp, "Via(%s) references non-existent net (%s) (%s)\n", insert->getRefname(), netName, validName);
			
			return;
		}
	}

   // Get CKT device ID. 
   // This has to be at end because it may depend on pin count (so have to gather pins first).
   m_sCKTDeviceID = getCKTDeviceID(m_sDeviceType, subClass, m_pinList.GetCount());
}

void CKTBoardItem::InsertPinItem(CKTInfoNetNameItem &item)
{
   // If first item just add it
   if (m_pinList.GetCount() == 0)
	{
		m_pinList.AddHead(item);
		return;
	}

   // Inser item if it belongs in front of any existing entry
	POSITION pos = m_pinList.GetHeadPosition();
	while (pos != NULL)
	{
		POSITION curPos = pos;
		CKTInfoNetNameItem curItem = m_pinList.GetNext(pos);

      CString itmSortKey( item.GetSortKey() );
      CString curSortKey( curItem.GetSortKey() );

		if (itmSortKey.Compare(curSortKey) < 0)
		{
			m_pinList.InsertBefore(curPos, item);
			return;
		}
	}

	// Not first item and not inserted before existing, add to end
	m_pinList.AddTail(item);
}

CString CKTBoardItem::GetCKTMappedPinName(CCEtoODBDoc &doc, CompPinStruct *cp)
{
   CString cktPinName;

   if (cp != NULL)
   {
      WORD deviceToPackagePinMap = doc.RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
      Attrib *attrib = get_attvalue(cp->getAttributes(), deviceToPackagePinMap);
      if (attrib != NULL)
      {
         CString pinMapName = attrib->getStringValue();
         pinMapName.Trim();

         if (!pinMapName.CompareNoCase("Anode"))
            cktPinName = "A";
         else if (!pinMapName.Trim().CompareNoCase("Cathode"))
            cktPinName = "C";
         else if (!pinMapName.Trim().CompareNoCase("Emitter"))
            cktPinName = "E";
         else if (!pinMapName.Trim().CompareNoCase("Base"))
            cktPinName = "B";
         else if (!pinMapName.Trim().CompareNoCase("Collector"))
            cktPinName = "C";
         else if (!pinMapName.Trim().CompareNoCase("Gate"))
            cktPinName = "G";
         else if (!pinMapName.Trim().CompareNoCase("Source"))
            cktPinName = "S";
         else if (!pinMapName.Trim().CompareNoCase("Drain"))
            cktPinName = "D";
         else if (!pinMapName.Trim().CompareNoCase("Input"))
            cktPinName = "I";
         else if (!pinMapName.Trim().CompareNoCase("Output"))
            cktPinName = "O";
         //else if (!pinMapName.Trim().CompareNoCase("Common"))  NO MAPPING FOR THIS ONE
         //   cktPinName = subData->getInsert()->getRefname();//infoStr = "Common";
         else if (!pinMapName.Trim().CompareNoCase("Positive"))
            cktPinName = "P";
         else if (!pinMapName.Trim().CompareNoCase("Negative"))
            cktPinName = "N";
         else if (!pinMapName.Trim().CompareNoCase(ATT_VALUE_NO_CONNECT))
            cktPinName = ATT_VALUE_NO_CONNECT; // not a real CKT value
      }
   }

   return cktPinName;
}

bool CKTBoardItem::IsPinMappingSupportedByDevType(CCEtoODBDoc &doc)
{
   // Make sure attrib has been looked up
   if (m_data != NULL && m_sDeviceType.IsEmpty())
   {
      WORD deviceKw = doc.RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
      Attrib *attrib = NULL;
      if(m_data->lookUpAttrib(deviceKw, attrib))
         m_sDeviceType = get_attvalue_string(&doc, attrib);
   }

   if (!m_sDeviceType.IsEmpty())   // Can't qualify if empty so skip all those string compares
   {
      if ( m_sDeviceType.CompareNoCase("Capacitor_Polarized")   == 0 || 
         m_sDeviceType.CompareNoCase("Capacitor_Tantalum")    == 0 || 
         m_sDeviceType.CompareNoCase("Diode")                 == 0 || 
         m_sDeviceType.CompareNoCase("Diode_LED")             == 0 || 
         m_sDeviceType.CompareNoCase("Diode_Zener")           == 0 ||
         m_sDeviceType.CompareNoCase("Transistor")            == 0 ||
         m_sDeviceType.CompareNoCase("Transistor_FET_NPN")    == 0 ||
         m_sDeviceType.CompareNoCase("Transistor_FET_PNP")    == 0 ||
         m_sDeviceType.CompareNoCase("Transistor_Mosfet_NPN") == 0 ||
         m_sDeviceType.CompareNoCase("Transistor_Mosfet_PNP") == 0 ||
         m_sDeviceType.CompareNoCase("Transistor_NPN")        == 0 ||
         m_sDeviceType.CompareNoCase("Transistor_PNP")        == 0)
      {
         return true;
      }
   }

   // If not explicitly supported then it is not supported
   return false;
}

CKTBoardItem &CKTBoardItem::operator=(const CKTBoardItem &other)
{
	if (this != &other)
	{
		m_sRefName = other.m_sRefName;
		m_sSortKey = other.m_sSortKey;
		m_sCKTDeviceID = other.m_sCKTDeviceID;
		m_sDeviceType = other.m_sDeviceType;
		m_sValue = other.m_sValue;
		m_cvValue = other.m_cvValue;
		m_sPlusTolerance = other.m_sPlusTolerance;
		m_sMinusTolerance = other.m_sMinusTolerance;
		m_eItemType = other.m_eItemType;
		m_sMessage = other.m_sMessage;
		m_bCapOpen   = other.m_bCapOpen;
		m_sPlacement = other.m_sPlacement;
		m_data       = other.m_data;

		m_pinList.RemoveAll();
		POSITION pos = other.m_pinList.GetHeadPosition();
		while (pos)
			m_pinList.AddTail(other.m_pinList.GetNext(pos));
	}

	return *this;
}

CString CKTBoardItem::generateSortKey(CString name) const
{
	// create sort name
	CString temp, buf;
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
				temp = (CString)"AAAAAAAAAAAA" + temp;
				temp.Delete(0, temp.GetLength() - 12);
				buf += temp;
			}
			else
			{
				// add number
				temp = (CString)"000000000000" + temp;
				temp.Delete(0, temp.GetLength() - 12);
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
		temp = (CString)"AAAAAAAAAAAA" + temp;
		temp.Delete(0, temp.GetLength() - 12);
		buf += temp;
	}
	else
	{
		// add number
		temp = (CString)"000000000000" + temp;
		temp.Delete(0, temp.GetLength() - 12);
		buf += temp;
	}

	return buf;
}

CString CKTBoardItem::getCKTDeviceID(CString deviceName, CString subClass, int pinCount) const
{
	if (deviceName == "Battery" || deviceName == "Capacitor_Array" || deviceName == "Crystal" || 
		deviceName == "Filter" || deviceName == "IC" || deviceName == "IC_Digital" || 
		deviceName == "IC_Linear" || deviceName == "Oscillator" || deviceName == "Power_Supply" || 
		deviceName == "Relay" || deviceName == "Resistor_Array" || deviceName == "Speaker" || deviceName == "Switch" || 
		deviceName == "Transformer" || deviceName == "Transistor_Array" || deviceName == "Transistor_SCR" ||
		deviceName == "Transistor_Triac") 
	{
		if (subClass.IsEmpty())
			return deviceName;
		return subClass;
	}
	else if (deviceName == "Connector" || deviceName == "No_Test" || deviceName == "Test_Point" || 
		deviceName == "VIA" || deviceName == "Unknown")
	{
		CString buf;
		buf.Format("UK%d", pinCount);
		return buf;
	}
	else if (deviceName == "Diode_Array")
		return subClass;
	else if (deviceName == "Capacitor")
		return "C";
	else if (deviceName == "Capacitor_Polarized")
		return "CP";
	else if (deviceName == "Capacitor_Tantalum")
		return "CP1";
	else if (deviceName == "Diode" || deviceName == "Diode_LED")
		return "CR";
	else if (deviceName == "Diode_Zener")
		return "VZ";
	else if (deviceName == "Fuse")
		return "F";
	else if (deviceName == "Inductor")
		return "L";
	else if (deviceName == "Jumper")
		return "NC";
	else if (deviceName == "Potentiometer")
		return "RV";
	else if (deviceName == "Resistor")
		return "R";
	else if (deviceName == "Transistor")
		return "QN";
	else if (deviceName == "Transistor_FET_NPN")
		return "NJFET";
	else if (deviceName == "Transistor_FET_PNP")
		return "PJFET";
	else if (deviceName == "Transistor_Mosfet_NPN")
		return "NJFET";
	else if (deviceName == "Transistor_Mosfet_PNP")
		return "PJFET";
	else if (deviceName == "Transistor_NPN")
		return "QN";
	else if (deviceName == "Transistor_PNP")
		return "QP";
	else if (deviceName == "Voltage_Regulator")
		return "VR";

	CString buf;
	buf.Format("UK%d", pinCount);
	return buf;
}

CString CKTBoardItem::generateMessage(CCEtoODBDoc *doc, DataStruct *data, CTMatrix mat, CString msgAttribName, CUserDefinedMessage *usrDefinedMsg) const
{
   // Case 2079
   // If msgAttribName is not blank and its value is not blank, then use value of 
   // named attribute for message.
   // If non-blank message is not generated by attrib, then generate message as was 
   // done before case 2079.
   // And then for case 2106 we added user configurable messages.

   CString buf;

   if (usrDefinedMsg != NULL && !usrDefinedMsg->isEmpty())
   {
      buf = usrDefinedMsg->format(doc, data);
   }

   // This is the case 2079 stuff. Probably should just nuke it. 
   // Newer stuff in "user defined message" can do what this does, and more.
   if (buf.IsEmpty() && !msgAttribName.IsEmpty())
   {
      // Use attrib value for message
      WORD kw = doc->IsKeyWord(msgAttribName, 0);
      Attrib *attrib = NULL;
      if (data->lookUpAttrib(kw, attrib))
      {
         switch (attrib->getValueType())
         {
         case valueTypeString:
            buf.Format("%s", attrib->getStringValue());
            break;
         case valueTypeDouble:
            buf.Format("%f", attrib->getDoubleValue());
            break;
         case valueTypeInteger:
            buf.Format("%d", attrib->getIntValue());
            break;
         }
      }
   }


   // If message buf was not set by attrib, then apply default message construction
   if (buf.IsEmpty())
   {
      WORD partNumberKw = doc->IsKeyWord(ATT_PARTNUMBER, 0);
      CString partNumber;
      Attrib *attrib = NULL;
      if (data->lookUpAttrib(partNumberKw, attrib))
         partNumber = get_attvalue_string(doc, attrib);

      CPoint2d point = data->getInsert()->getOrigin2d();
      mat.transform(point);

      buf.Format("%s %.3f %.3f %s", partNumber, point.x, point.y, data->getInsert()->getGraphicMirrored()?"B":"T");
   }

	return buf;
}

CString CKTBoardItem::GetCircuitString() const
{
	CString retStr;
	retStr.Format("%-9s %-12s ", check_name('c', m_sRefName, FALSE), m_sCKTDeviceID); // case 1823, add space between fields, the final space is important too

	int indent = 0;
	POSITION pos = m_pinList.GetHeadPosition();
	while (pos)
	{
		CString pinInfo;
		CKTInfoNetNameItem item = m_pinList.GetNext(pos);

		pinInfo.Format("%*s = %s%s\n", indent, check_name('p', item.GetInfo(), FALSE), check_name('n', item.GetCKTNetName(), FALSE), pos?",":";");
		indent = 24;

		retStr += pinInfo;
	}

	return retStr;
}

CString CKTBoardItem::GetValueString(CCEtoODBDoc* pDoc) const
{
	if (m_eItemType == cktBoardItemTypeVia)
		return "";


	CString buf = "";
	CString noTest = "";
	CString refName = check_name('c', m_sRefName, FALSE);
   /// Related to case 2106, but not actually case 2106 itself:
   /// 7/20/06 email from Mark says stop doing name check on message.
	///CString message = (CString)"MSG='" + check_name('m', m_sMessage, FALSE) + "'";
   CString message = (CString)"MSG='" + m_sMessage + "'";

	CString tolerances = m_sPlusTolerance + "%";
	if (!m_sMinusTolerance.IsEmpty() && m_sPlusTolerance.CompareNoCase(m_sMinusTolerance) != 0)
		tolerances.AppendFormat(", %s%%", m_sMinusTolerance);


	WORD testStrategyKey = pDoc->RegisterKeyWord(ATT_TEST_STRATEGY, 0, VT_STRING);
	Attrib* attrib = NULL;
	if ((attrib = is_attvalue(pDoc, m_data->getAttributesRef(), ATT_TEST_STRATEGY, keyWordSectionCC)) != NULL)
	{
		if (attrib->getStringValue().Find(ATT_VALUE_TEST_ICT) < 0)
			noTest = ", NOTEST";
	}

	if (m_sDeviceType == "Capacitor"				|| m_sDeviceType == "Capacitor_Array"	|| m_sDeviceType == "Capacitor_Polarized" || 
		 m_sDeviceType == "Capacitor_Tantalum" || m_sDeviceType == "Diode_Zener"		|| m_sDeviceType == "Inductor" ||
		 m_sDeviceType == "Potentiometer"		|| m_sDeviceType == "Resistor")
	{
		MetricPrefixTag metricPrefix;
		double dValue = m_cvValue.GetValue();
		ValueUnitTag valUnit = m_cvValue.GetUnits();
	   double value = scientificNormalization(dValue, metricPrefix);

	   CString prefix = metricPrefixTagToAbbreviatedString(metricPrefix);
		if (valUnit == valueUnitOhm && metricPrefix == metricPrefixMega)
			prefix = "MEG";

		buf.Format("%s = %lg%s, %s, %s%s;", refName, value, prefix.MakeUpper(), tolerances, message, noTest);
	}
	else if (m_sDeviceType == "No_Test" || m_sDeviceType == "Unknown" || m_sDeviceType.IsEmpty())
	{
		// case 1823, no tolerance in NOTEST record
		//buf.Format("%s = %s, %s, %s%s;", refName, "NOTEST", tolerances, message, noTest);
		buf.Format("%s = %s, %s%s;", refName, "NOTEST", message, noTest);
	}
	else
		buf.Format("%s = %s%s;", refName, message, noTest);

	return buf;
}



/******************************************************************************
* CKTBoardItemList 
*/
void CKTBoardItemList::RemoveAll()
{
	m_boardList.RemoveAll();
}

POSITION CKTBoardItemList::GetHeadPosition() const
{
	return m_boardList.GetHeadPosition();
}

POSITION CKTBoardItemList::GetTailPosition() const
{
	return m_boardList.GetTailPosition();
}

CKTBoardItem& CKTBoardItemList::GetNext(POSITION& rPosition)
{
	return m_boardList.GetNext(rPosition);
}

const CKTBoardItem& CKTBoardItemList::GetNext(POSITION& rPosition) const
{
	return m_boardList.GetNext(rPosition);
}

CKTBoardItem& CKTBoardItemList::GetPrev(POSITION& rPosition)
{
	return m_boardList.GetPrev(rPosition);
}

CKTBoardItem& CKTBoardItemList::GetAt(POSITION position)
{
	return m_boardList.GetAt(position);
}

void CKTBoardItemList::RemoveAt(POSITION position)
{
	m_boardList.RemoveAt(position);
}

void CKTBoardItemList::Insert(CKTBoardItem item)
{
	if (m_boardList.GetCount() == 0)
	{
		m_boardList.AddHead(item);
		return;
	}

	POSITION pos = m_boardList.GetHeadPosition();
	while (pos)
	{
		POSITION curPos = pos;
		CKTBoardItem curItem = m_boardList.GetNext(pos);

		CString sortKey = item.GetSortKey(), curSortKey = curItem.GetSortKey();
		if (sortKey.Compare(curSortKey) < 0)
		{
			m_boardList.InsertBefore(curPos, item);
			break;
		}
	}

	if (pos == NULL)
		m_boardList.AddTail(item);
}



/******************************************************************************
* CKTProbe
*/
CKTProbe::CKTProbe(CCEtoODBDoc *doc, DataStruct* data, CString info, CString netName) : CKTInfoNetNameItem(doc, data, info, netName)
{
	// check if the probe is placed
	WORD placedKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
	CString placedValue = "Unplaced";

	Attrib *attrib = NULL;
	if (data->lookUpAttrib(placedKW, attrib))
		placedValue = get_attvalue_string(doc, attrib);
	m_bPlaced = (placedValue == "Placed");

	// get resource type
	WORD testResKW = doc->RegisterKeyWord(ATT_TESTRESOURCE, 0, VT_STRING);
	CString resourceType;
	if (data->lookUpAttrib(testResKW, attrib))
		resourceType = get_attvalue_string(doc, attrib);

	m_eResourceType = testResourceTypeTest;
	if (resourceType == "Power Injection")
		m_eResourceType = testResourceTypePowerInjection;

   SetProbedDiePin(doc->getCamCadData(), data);
}

CString CKTProbe::GetProbeString() const
{
	CString buf;
   buf.Format("F%s = %s;\n", this->GetInfo(), check_name('n', this->GetCKTNetName(), FALSE));
	return buf;
}

/******************************************************************************
* CKTProbe::SetProbedDiePin
*/
void CKTProbe::SetProbedDiePin(CCamCadData &cancadData, DataStruct* data)
{
   m_IsProbedDiePin = ::IsProbedDiePin(cancadData,data);   
}

/******************************************************************************
* CKTWriter
*/
CKTWriter::CKTWriter(CCEtoODBDoc &doc, FormatStruct *format, int page_units, FILE *logFp)
: m_pDoc(doc)
, m_cktLogFp(logFp)
{
	m_format = format;
	m_pageUnits = page_units;

	m_lViaIndex = 1;
   m_lBondPadIndex = 1;

}

CKTWriter::~CKTWriter()
{
}

int CKTWriter::getAllData()
{
	FileStruct *file = m_pDoc.getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
		return -1;

	// get all components, vias, and probes
	POSITION dataPos = file->getBlock()->getHeadDataInsertPosition();
	while (dataPos)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(dataPos);
		InsertStruct *insert = data->getInsert();
		
		CPoint2d insertLocation = insert->getOrigin2d();
		file->getTMatrix().transform(insertLocation);

		if (insert->getInsertType() == insertTypeVia)
		{
			CString refname;
			if (insert->getRefname().IsEmpty())
				refname.Format("Via%d", m_lViaIndex++);
				
         m_boardItems.Insert(CKTBoardItem(data, file->getTMatrix(), m_pDoc, m_cktLogFp, m_messageAttribName, refname));
		}
      if (insert->getInsertType() == insertTypeBondPad)
		{
			CString refname;
			if (insert->getRefname().IsEmpty())
				refname.Format("BondPad%d", m_lBondPadIndex++);
				
         m_boardItems.Insert(CKTBoardItem(data, file->getTMatrix(), m_pDoc, m_cktLogFp, m_messageAttribName, refname));
		}
		else if (insert->getInsertType() == insertTypePcbComponent)
		{
			m_boardItems.Insert(CKTBoardItem(data, file->getTMatrix(), m_pDoc, m_cktLogFp, m_messageAttribName));
		}
		else if (insert->getInsertType() == insertTypeTestProbe)
		{
			WORD netNameKW = m_pDoc.RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);

			CString netName;
			Attrib *attrib = NULL;
			if (data->lookUpAttrib(netNameKW, attrib))
				netName = get_attvalue_string(&m_pDoc, attrib);

			m_probes.AddTail(CKTProbe(&m_pDoc, data, insert->getRefname(), netName));
		}
	}

	return 0;
}

int CKTWriter::writeCircuitSection(CFormatStdioFile &file)
{
	file.WriteString("%%CIRCUIT;\n");
	
	CString buf;
	POSITION pos = m_boardItems.GetHeadPosition();
	while (pos)
	{
		CKTBoardItem item = m_boardItems.GetNext(pos);
	
		if (item.m_pinList.GetCount() <= 0)
			continue;

		CString str = item.GetCircuitString();
		file.WriteString("%s", str.MakeUpper());
	}

	return 0;
}

int CKTWriter::writeValueSection(CFormatStdioFile &file)
{
	file.WriteString("%%VALUE;\n");

	CString buf;
	POSITION pos = m_boardItems.GetHeadPosition();
	while (pos)
	{
		CKTBoardItem item = m_boardItems.GetNext(pos);
		
		if (item.GetItemType() == cktBoardItemTypeVia)
			continue;
		
		CString str = item.GetValueString(&m_pDoc);
		file.WriteString("%s\n", str.MakeUpper());
	}

	return 0;
}
int CKTWriter::writeAdaptorSection(CFormatStdioFile &file)
{
   file.WriteString("%%ADAPTOR:%s TEMP;\n", (!m_usersAdaptorName.IsEmpty()) ? m_usersAdaptorName : "2280");
	
	CString buf;
	POSITION pos = m_probes.GetHeadPosition();
	while (pos)
	{
		CKTProbe probe = m_probes.GetNext(pos);
	
		if (probe.GetCKTNetName().IsEmpty() || probe.GetResourceType() != testResourceTypeTest)
			continue;
      
      if(probe.IsProbedDiePin())
      {
         fprintf(m_cktLogFp, "%s - Net: %s Probe F%s - Skipped Probe for Die pin.\n", file.GetFileName(),
            probe.GetCKTNetName(), probe.GetInfo());
      }
		else if (probe.IsPlaced())
		{
			CString str = probe.GetProbeString();		
			file.WriteString("        %s", str);
		}
	}

	return 0;
}

int CKTWriter::writeDPIFile(CFormatStdioFile &file)
{
	POSITION pos = m_boardItems.GetHeadPosition();
	while (pos)
	{
		CKTBoardItem item = m_boardItems.GetNext(pos);

		//check to see if the cap open attrib is true
		if (item.GetCapOpen())
		{
			CString str = item.GetRefName();
         file.WriteString("%s;\n", check_name('r', str.MakeUpper(), FALSE));
		}
	}

	return 0;
}
int CKTWriter::writeFDSAP03File(CFormatStdioFile &file)
{
	file.WriteString("!AP03       UUT      unit\n");
	CString unitStr = "";
	short unit = m_pDoc.getSettings().getPageUnits();
	switch(unit)
	{
	case UNIT_INCHES:
		unitStr = "in";
		break;
	case UNIT_MILS:
		unitStr = "mils";
		break;
	case UNIT_MM:
		unitStr = "mm";
		break;
	}
	file.WriteString("AP03        UUT        %s\n", unitStr.MakeUpper());
	
	return 0;
}

int CKTWriter::writeFDSAP04File(CFormatStdioFile &file)
{
	file.WriteString("!...............................................\n");
	file.WriteString("!AP04       UUT   locate    x        y\n");
	file.WriteString(writeFDSBoardOutline(file));
	return 0;
}

int CKTWriter::writeFDSAP05File(CFormatStdioFile &file)
{
	file.WriteString("!...............................................\n");
	POSITION pos = m_boardItems.GetHeadPosition();
	while (pos)
	{
		CKTBoardItem item = m_boardItems.GetNext(pos);
		POSITION pinPos = item.m_pinList.GetHeadPosition();
		while (pinPos)
		{
         CKTInfoNetNameItem pin = item.m_pinList.GetNext(pinPos);

			if (pin.GetTool())
			{
				file.WriteString("AP05   UUT     %0.4lf   %0.4lf\n",
					item.GetData()->getInsert()->getOriginX(), item.GetData()->getInsert()->getOriginY());
			}
		}			
	}
	return 0;
}

int CKTWriter::writeFDSAS03File(CFormatStdioFile &file)
{
	file.WriteString("!...............................................\n");
	file.WriteString("!AS03   UUT   device      pin          x-coordinate   y-coordinate\n");
	POSITION pos = m_boardItems.GetHeadPosition();
	while (pos)
	{
		CKTBoardItem item = m_boardItems.GetNext(pos);
		POSITION pinPos = item.m_pinList.GetHeadPosition();
		while (pinPos)
		{
         CKTInfoNetNameItem pin = item.m_pinList.GetNext(pinPos);
			Point2 pinLocation;
			DataStruct* compData = item.GetData();
			Mat2x2 compRotMat;
         RotMat2(&compRotMat, compData->getInsert()->getAngle());

			if (pin.GetData())
			{
            InsertStruct *pinInsert = pin.GetData()->getInsert();
				pinLocation.x = pinInsert->getOriginX() * compData->getInsert()->getScale();
				pinLocation.y = pinInsert->getOriginY() * compData->getInsert()->getScale();
				int mirror = pin.GetData()->getInsert()->getMirrorFlags();  // I bet this isn't right. Should be parent comp's mirror flag. But no complaints on file so reluctant to change it.
				if (mirror & MIRROR_FLIP)  pinLocation.x = -pinLocation.x;
				TransPoint2(&pinLocation, 1, &compRotMat, compData->getInsert()->getOriginX(), compData->getInsert()->getOriginY());
			}

			file.WriteString("AS03    UUT      %s         %s             %.4lf              %.4lf\n",
				check_name('r', item.GetRefName(), FALSE), check_name('p', pin.GetInfo(), FALSE), 
            pin.GetData() ? pinLocation.x : compData->getInsert()->getOriginX(),
				pin.GetData() ? pinLocation.y : compData->getInsert()->getOriginY());
		}
	}
	return 0;
}

int CKTWriter::writeFDSAS10File(CFormatStdioFile &file)
{
	CString buf = "";
	file.WriteString("!...............................................\n");
	file.WriteString("!AS10   UUT   device         side\n");

	POSITION pos = m_boardItems.GetHeadPosition();
	while (pos)
	{
		CKTBoardItem item = m_boardItems.GetNext(pos);
		buf.Format("AS10    UUT      %s          %s  \n",item.GetRefName(), item.GetPlacement());
		file.WriteString(buf); 
	}
	return 0;
}

CString CKTWriter::writeFDSBoardOutline(CFormatStdioFile &file)
{
	CString buf = "";
	CString tempBuf = "";
	for (int i=0; i<m_pDoc.getMaxBlockIndex(); i++)
	{
		BlockStruct* block = m_pDoc.getBlockAt(i);
		if (!block)
			continue;
		
		POSITION dataPos = block->getHeadDataPosition();
		while (dataPos)
		{
			DataStruct* data = block->getNextData(dataPos);
			if (!data || !data->getPolyList() || data->getGraphicClass() != graphicClassBoardOutline)
				continue;
			POSITION polyPos = data->getPolyList()->GetHeadPosition();
			int cnt = 1;
			while (polyPos)
			{
				double x,y;
				CPoly* outlinePoly = data->getPolyList()->GetNext(polyPos);
				if (!outlinePoly)
					continue;
				POSITION pntPos = outlinePoly->getPntList().GetHeadPosition();
				while (pntPos)
				{
					CPnt* pnt = outlinePoly->getPntList().GetNext(pntPos);
					if (!pnt)
						continue;
					x = pnt->x;
					y = pnt->y;
					tempBuf.Format("AP04        UUT     %d      %.4lf      %.4lf\n", cnt, x, y);
					buf += tempBuf;
					cnt++;
				}
			}
		}
	}

	return buf;
}

int CKTWriter::writeNDBFile(CFormatStdioFile &file)
{
	FileStruct *pFile = m_pDoc.getFileList().GetOnlyShown(blockTypePcb);

	if (pFile == NULL)
		return -1;

	int viaCount = 1;
	file.WriteString("%%NAILLOC;\n");
	CompPinStruct* cpAccess = NULL;
	DataStruct* viaAccess = NULL;
	POSITION pos = m_probes.GetHeadPosition();
	while (pos)
	{
		CKTProbe probe = m_probes.GetNext(pos);
	
		if (probe.GetCKTNetName().IsEmpty())
			continue;

		cpAccess = NULL;
		viaAccess = NULL;

		Attrib* attrib = is_attvalue(&m_pDoc, probe.GetData()->getAttributesRef(), ATT_DDLINK, 1);
		if (attrib)
		{
			long entityNum = (long)attrib->getIntValue();

			//EEntityType type;
			//void* voidPtr = FindEntity(&m_pDoc, entityNum, type);
         CEntity accessEntity = CEntity::findEntity(m_pDoc.getCamCadData(),entityNum);

         if (accessEntity.getEntityType() != entityTypeUndefined)
			{
				attrib = NULL;
				//attrib = is_attvalue(&m_pDoc, ((DataStruct*)voidPtr)->getAttributesRef(), ATT_DDLINK, 1);
				attrib = is_attvalue(&m_pDoc,*(accessEntity.getAttributesRef()), ATT_DDLINK, 1);

				if (attrib)
				{							
					entityNum = (long)attrib->getIntValue();

					//voidPtr = NULL;
					//voidPtr = FindEntity(&m_pDoc, entityNum, type);
               CEntity entity = CEntity::findEntity(m_pDoc.getCamCadData(),entityNum);

               if (entity.getEntityType() == entityTypeData)
					{
                  viaAccess = entity.getData();
					}
					else if (entity.getEntityType() == entityTypeCompPin)
					{
                  cpAccess = entity.getCompPin();
					}
				}
			}
		}

		if (viaAccess != NULL || cpAccess != NULL)
		{
			CString refName, pinName;
			if (viaAccess)
			{
				refName = viaAccess->getInsert()->getRefname();
				pinName = "1";

				if (refName.IsEmpty())
					refName.Format("VIA%04d.1", viaCount++);
			}
			else if (cpAccess)
			{
				refName = cpAccess->getRefDes();
				pinName = cpAccess->getPinName();
			}

         if(!probe.IsProbedDiePin())
			   file.WriteString("%s.%s;                 /*%s*/\n", check_name('r', refName.MakeUpper(), FALSE), check_name('p', pinName.MakeUpper(), FALSE),
				   check_name('n', probe.GetCKTNetName().MakeUpper(), FALSE));
         else
            fprintf(m_cktLogFp, "%s - Net: %s CompPin %s.%s - Skipped CompPin for Die component.\n", file.GetFileName(),
                  probe.GetCKTNetName(), refName, pinName);
		}
	}


	// gather all the nets
	CMapStringToInt netMap;
	WORD deviceToPackagePinMap = m_pDoc.RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
	POSITION netPos = pFile->getHeadNetPosition();
	while (netPos)
	{
		NetStruct *net = pFile->getNextNet(netPos);

		bool noConnect = true;
		POSITION compPinPos = net->getHeadCompPinPosition();
		while (compPinPos != NULL)
		{
			CString pinMapName = "";
			CompPinStruct* compPin = net->getNextCompPin(compPinPos);
			Attrib *attrib = get_attvalue(compPin->getAttributes(), deviceToPackagePinMap);
			if (attrib != NULL)
				pinMapName = attrib->getStringValue();

			if (pinMapName.Trim().CompareNoCase(ATT_VALUE_NO_CONNECT) != 0)
			{
				noConnect = false;
				break;
			}
		}

		if (!noConnect)
			netMap.SetAt(net->getNetName(), FALSE);
	}


	POSITION dataPos = pFile->getBlock()->getHeadDataInsertPosition();
	while (dataPos)
	{
		BOOL accessProbed = FALSE;
		DataStruct* accessData = pFile->getBlock()->getNextDataInsert(dataPos);
		
		if (!accessData || accessData->getInsert()->getInsertType() != insertTypeTestAccessPoint)
			continue;
		
		POSITION pos = m_probes.GetHeadPosition();
		while (pos)
		{
			CKTProbe probe = m_probes.GetNext(pos);
		
			if (probe.GetCKTNetName().IsEmpty())
				continue;
		   
			DataStruct* probeData = probe.GetData();
			Attrib* attrib = is_attvalue(&m_pDoc, probeData->getAttributesRef(), ATT_DDLINK, 1);

			if (attrib)
			{
				long entityNum = (long)attrib->getIntValue();
				if (entityNum == accessData->getEntityNumber())
					accessProbed = TRUE;
			}
		}
		if (!accessProbed)
		{
			Attrib* attrib = is_attvalue(&m_pDoc, accessData->getAttributesRef(), ATT_DDLINK, 1);
			if (attrib)
			{
				CompPinStruct* cp = NULL;
				long entityNum = (long)attrib->getIntValue();

				//void* voidPtr = NULL;
				//EEntityType type;
				//voidPtr = FindEntity(&m_pDoc, entityNum, type);
            CEntity accessEntity = CEntity::findEntity(m_pDoc.getCamCadData(), entityNum);

            if (accessEntity.getEntityType() == entityTypeCompPin)
				{
               cp = accessEntity.getCompPin();
				}

				if (cp)
				{
               if(!cp->IsDiePin(m_pDoc.getCamCadData()))
               {
					   CString str = (CString)check_name('r', cp->getRefDes(), FALSE) + "." + check_name('p', cp->getPinName(), FALSE);
					   file.WriteString("^%s;\n", str.MakeUpper());
               }
               else
               {
                  fprintf(m_cktLogFp, "%s - Net: %s CompPin %s - Skipped CompPin for Die component.\n", file.GetFileName(),
                     (accessEntity.getNet())?accessEntity.getNet()->getNetName():"", cp->getPinRef());
               }
				}
			}
		}

		Attrib* attrib = is_attvalue(&m_pDoc, accessData->getAttributesRef(), ATT_NETNAME, 1);
		if (attrib != NULL)
		{
			CString netName = get_attvalue_string(&m_pDoc, attrib);
			netMap.SetAt(netName, TRUE);
		}
	}
   
	file.WriteString("\n");
	file.WriteString("%%NONAIL;\n");
	netPos = netMap.GetStartPosition();
	while (netPos)
	{
		CString netName;
		int value;
		netMap.GetNextAssoc(netPos, netName, value);
      
      NetStruct *net = pFile->getNetList().getNet(netName);
      bool IsDieNet = (net)?net->IsSingleDiePinNet():false;
      //Skip Die Pins and leave log message
      if(IsDieNet)
         fprintf(m_cktLogFp, "%s - Net: %s - Skipped Single Die Pin Net.\n", file.GetFileName(), netName);
		else if (!value)
			file.WriteString("%s;\n", check_name('n', netName.MakeUpper(), FALSE));
	}


	return 0;
}


int CKTWriter::writeFDSFile(CFormatStdioFile &fdsFile)
{
   writeFDSAP03File(fdsFile);
	fdsFile.WriteString("\n");
	writeFDSAP04File(fdsFile);
	fdsFile.WriteString("\n");
	writeFDSAP05File(fdsFile);
	fdsFile.WriteString("\n");
	writeFDSAS03File(fdsFile);
	fdsFile.WriteString("\n");
	writeFDSAS10File(fdsFile);
	fdsFile.WriteString("\n");

	return 0;
}

int CKTWriter::writeCKTFile(CFormatStdioFile &outFile)
{
	writeCircuitSection(outFile);
	outFile.WriteString("\n");
	writeValueSection(outFile);
	outFile.WriteString("\n");
	writeAdaptorSection(outFile);

	return 0;
}


int CKTWriter::WriteFile(CString fileName)
{
	// load settings
   CString settingsFile( getApp().getExportSettingsFilePath("ckt.out") );
   {
      CString msg;
      msg.Format("Teradyne 228x CKT: Settings file [%s].\n\n", settingsFile);
      getApp().LogMessage(msg);
   }
	if (LoadSettings(settingsFile) < 0)
		return -1;



	// write data
   CString checkFile( getApp().getSystemSettingsFilePath("ckt.chk") );
	check_init(checkFile);

	getAllData();

	OutputCKTFormat(fileName);
	
	check_deinit();

	// closing log file entry
   CTime t;
   t = t.GetCurrentTime();
   fprintf(m_cktLogFp, "%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));
   fprintf(m_cktLogFp, "Logfile closed\n");

   return 0;
}

int CKTWriter::LoadSettings(CString fileName)
{
	FILE *fp = fopen(fileName,  "rt");
   if (fp == NULL)
   {
      return 0;
   }
     
   char line[255];
   while (fgets(line, 255, fp))
   {
      CString w = line;
      char *lp = get_string(line, " \t\n");
      if (lp == NULL)
         continue;

      if (lp[0] == '.')
      {
     		if (STRICMP(lp, ".OUTPUT_FORMAT") == 0)
			{
            // This keyword was used in 4.5 but no longer in 4.6, see case dts0100407106
			}
         else if (STRICMP(lp, ".MESSAGE_ATTRIBUTE") == 0)
         {
				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            m_messageAttribName = lp;
         }
         else if (STRICMP(lp, ".ADAPTOR_NAME") == 0)
         {
				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            m_usersAdaptorName = lp;
         }
         else if (GetUserDefMsg()->isKeyword(lp))
         {
            CString kw(lp);

				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            GetUserDefMsg()->append(kw, lp);
         }

		}
   }

	return 0;
}

int CKTWriter::OutputCKTFormat(CString fileName)
{
	fprintf(m_cktLogFp, "Output Format: CKT\n");

	// write CKT file
	// initial filename already has .ckt extension
   fprintf(m_cktLogFp, "Filename: %s\n", fileName);
	CFormatStdioFile outFile;
	if (!outFile.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fileName);
		fprintf(m_cktLogFp, "Can not open file [%s]", fileName);
      ErrorMessage(tmp);
      return -1;
   }

	writeCKTFile(outFile);
	outFile.Close();



	//write DPI File
	CFormatStdioFile dpiFile;
	int nIndex = fileName.ReverseFind('.');
	if (nIndex >0)
		fileName = fileName.Left(nIndex);
	fileName += ".dpi";
	fprintf(m_cktLogFp, "Filename: %s\n", fileName);
	if (!dpiFile.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fileName);
		fprintf(m_cktLogFp, "Can not open file [%s]", fileName);
      ErrorMessage(tmp);
      return -1;
   }

	writeDPIFile(dpiFile);
	dpiFile.Close();

	//write FDS File
	CFormatStdioFile fdsFile;
	nIndex = -1;
	nIndex = fileName.ReverseFind('.');
	if (nIndex >0)
		fileName = fileName.Left(nIndex);
	fileName += ".fds";
	fprintf(m_cktLogFp, "Filename: %s\n", fileName);
	if (!fdsFile.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fileName);
		fprintf(m_cktLogFp, "Can not open file [%s]", fileName);
      ErrorMessage(tmp);
      return -1;
   }

	writeFDSFile(fdsFile);
	fdsFile.Close();

	//write NDB File
	CFormatStdioFile ndbFile;
	nIndex = -1;
	nIndex = fileName.ReverseFind('.');
	if (nIndex >0)
		fileName = fileName.Left(nIndex);
	fileName += ".ndb";
	fprintf(m_cktLogFp, "Filename: %s\n", fileName);
	if (!ndbFile.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fileName);
		fprintf(m_cktLogFp, "Can not open file [%s]", fileName);
      ErrorMessage(tmp);
      return -1;
   }

	writeNDBFile(ndbFile);
	ndbFile.WriteString("\n");

	ndbFile.Close();

	return 0;
}


