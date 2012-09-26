     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2008. All Rights Reserved.
*/           

#include "stdafx.h"


#include "Element.h"
#include "DcaInsert.h"
#include "DcaDataType.h"
#include "ODBC_Lib.h"

CString CSubclassElement::getPinNName(int n)
{
   switch (n)
   {
   case 1:  return getPin1Name();
   case 2:  return getPin2Name();
   case 3:  return getPin3Name();
   case 4:  return getPin4Name();
   }

   return "";
}

CString CSubclassElement::getPinFunction(int pinNum)
{
   // Pin function is fixed per pin type, but depends on device type.
   // E.g. when dev type calls for anode, pin1Name is always anode, pin2Name is always cathode.

   CString pinFunction("---"); // Indicator for "not used" or "not applicable"


   switch (this->m_deviceType)
   {
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      switch (pinNum)
      {
      case 1: pinFunction = "Positive";     break;
      case 2: pinFunction = "Negative";     break;
      }
      break;

   case deviceTypeDiode:
   case deviceTypeDiodeLed:
   case deviceTypeDiodeZener:
      switch (pinNum)
      {
      case 1: pinFunction = "Anode";     break;
      case 2: pinFunction = "Cathode";   break;
      }
      break;

   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      switch (pinNum)
      {
      case 1: pinFunction = "Base";        break;
      case 2: pinFunction = "Emitter";     break;
      case 3: pinFunction = "Collector";   break;
      }
      break;

   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorTriac:
      switch (pinNum)
      {
      case 1: pinFunction = "Drain";    break;
      case 2: pinFunction = "Gate";     break;
      case 3: pinFunction = "Source";   break;
      }
      break;

   case deviceTypeTransistorScr:
      switch (pinNum)
      {
      case 1: pinFunction = "Anode";     break;
      case 2: pinFunction = "Gate";      break;
      case 3: pinFunction = "Cathode";   break;
      }
      break;
   
   case deviceTypeOpto:
      switch (pinNum)
      {
      case 1: pinFunction = "Anode";      break;
      case 2: pinFunction = "Cathode";    break;
      case 3: pinFunction = "Collector";  break;
      case 4: pinFunction = "Emitter";    break;
      }


   default:
#ifdef HANDY_FOR_DEBUGGING
      switch (pinNum)
      {
      case 1: pinFunction = "A";        break;
      case 2: pinFunction = "B";     break;
      case 3: pinFunction = "C";   break;
      case 4: pinFunction = "D";   break;

      }
#endif
      break;
   }

   return pinFunction;
}

int CSubclassElement::getPinNumForFunction(CString pinFunction)
{
   // Pin function is fixed per pin type, but depends on device type.
   // E.g. when dev type calls for anode, pin1Name is always anode, pin2Name is always cathode.

   int pinNum = 0; // 0 means not set, no pin, not a recognized function for device type

   switch (this->m_deviceType)
   {
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      {
         // case 1: pinFunction = "Positive";     break;
         // case 2: pinFunction = "Negative";     break;
         if (pinFunction.CompareNoCase("Positive") == 0)
            pinNum = 1;
         else if (pinFunction.CompareNoCase("Negative") == 0)
            pinNum = 2;
      }
      break;

   case deviceTypeDiode:
   case deviceTypeDiodeLed:
   case deviceTypeDiodeZener:
      {
         // case 1: pinFunction = "Anode";     break;
         // case 2: pinFunction = "Cathode";   break;
         if (pinFunction.CompareNoCase("Anode") == 0)
            pinNum = 1;
         else if (pinFunction.CompareNoCase("Cathode") == 0)
            pinNum = 2;
      }
      break;

   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
      {
         // case 1: pinFunction = "Base";        break;
         // case 2: pinFunction = "Emitter";     break;
         // case 3: pinFunction = "Collector";   break;
         if (pinFunction.CompareNoCase("Base") == 0)
            pinNum = 1;
         else if (pinFunction.CompareNoCase("Emitter") == 0)
            pinNum = 2;
         else if (pinFunction.CompareNoCase("Collector") == 0)
            pinNum = 3;
      }
      break;

   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorTriac:
      {
         // case 1: pinFunction = "Drain";    break;
         // case 2: pinFunction = "Gate";     break;
         // case 3: pinFunction = "Source";   break;
         if (pinFunction.CompareNoCase("Drain") == 0)
            pinNum = 1;
         else if (pinFunction.CompareNoCase("Gate") == 0)
            pinNum = 2;
         else if (pinFunction.CompareNoCase("Source") == 0)
            pinNum = 3;
      }
      break;

   case deviceTypeTransistorScr:
      {
         // case 1: pinFunction = "Anode";     break;
         // case 2: pinFunction = "Gate";      break;
         // case 3: pinFunction = "Cathode";   break;
         if (pinFunction.CompareNoCase("Anode") == 0)
            pinNum = 1;
         else if (pinFunction.CompareNoCase("Gate") == 0)
            pinNum = 2;
         else if (pinFunction.CompareNoCase("Cathode") == 0)
            pinNum = 3;
      }
      break;
   
   case deviceTypeOpto:
      {
         // case 1: pinFunction = "Anode";      break;
         // case 2: pinFunction = "Cathode";    break;
         // case 3: pinFunction = "Collector";  break;
         // case 4: pinFunction = "Emitter";    break;
         if (pinFunction.CompareNoCase("Anode") == 0)
            pinNum = 1;
         else if (pinFunction.CompareNoCase("Cathode") == 0)
            pinNum = 2;
         else if (pinFunction.CompareNoCase("Collector") == 0)
            pinNum = 3;
         else if (pinFunction.CompareNoCase("Emitter") == 0)
            pinNum = 4;
      }


   default:
      break;
   }

   return pinNum;
}

CString CSubclassElement::getPinFunction(CString pinRefname)
{
   if (!pinRefname.IsEmpty())
   {
      if (getPin1Name().Compare(pinRefname) == 0)
         return getPinFunction(1);
      else if (getPin2Name().Compare(pinRefname) == 0)
         return getPinFunction(2);
      else if (getPin3Name().Compare(pinRefname) == 0)
         return getPinFunction(3);
      else if (getPin4Name().Compare(pinRefname) == 0)
         return getPinFunction(4);
   }

   return "";
}

int CSubclassElement::getNumPinsRequired()
{
   // Number of pins required depends on device type.
   // Most require 2, a few require 3. 

   // All we really need here is the transistor case, maybe this ought to be simplified,
   // but as it is, all is explicit.

   int pinsReq = 2;

   switch (this->m_deviceType)
   {
   case deviceTypeTransistor:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
      pinsReq = 3;
      break;

   case deviceTypeOpto:
      pinsReq = 4;
      break;
   }

   return pinsReq;
}

CSubclassElement& CSubclassElement::operator=(const CSubclassElement &other)
{
   if (&other != NULL)
   {
      this->m_name        = other.m_name;
      this->m_deviceType  = other.m_deviceType;
      this->m_minusTol    = other.m_minusTol;
      this->m_pin1Refname = other.m_pin1Refname;
      this->m_pin2Refname = other.m_pin2Refname;
      this->m_pin3Refname = other.m_pin3Refname;
      this->m_pin4Refname = other.m_pin4Refname;
      this->m_plusTol     = other.m_plusTol;
      this->m_value       = other.m_value;
   }

   return *this;
}

CSubclassElementList& CSubclassElementList::operator=(const CSubclassElementList &other)
{
   if (&other != NULL)
   {
      this->empty();

      for (int i = 0; i < other.GetCount(); i++)
      {
         CSubclassElement *sourceEl = other.GetAt(i);
         
         if (sourceEl != NULL)
         {
            CSubclassElement *destEl = new CSubclassElement(*sourceEl);
            this->Add(destEl);
         }
      }
   }

   return *this;
}

CSubclass *CSubclassList::findSubclass(CString name)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      CSubclass *sc = this->GetAt(i);

      if (sc != NULL && sc->getName().Compare(name) == 0)
      {
         return sc;
      }
   }

   return NULL;
}

void CSubclassList::deleteSubclass(CSubclass *victim)
{
   // DANGER !!! victim is bad ptr after this call

   for (int i = 0; i < this->GetCount(); i++)
   {
      CSubclass *sc = this->GetAt(i);

      if (sc == victim)
      {
         CDBInterface db;
         if (db.Connect())
            db.DeleteSubclass(victim->getName());

         this->RemoveAt(i);
         delete sc;

         return;
      }
   }
}

/******************************************************************************
*/
/*
<SubclassLib>
  <Subclass Name="subclass1">
    <Element Name="el1" DeviceType="Capacitor" Value="10" PlusTolerance="9" MinusTolerance="11" />
    <Element Name="el2" DeviceType="Transistor" Value="12" PlusTolerance="14" MinusTolerance="13">
      <Pin Name="3" Function="Base" />
      <Pin Name="4" Function="Emitter" />
      <Pin Name="5" Function="Collector" />
    </Element>
  </Subclass>
</SubclassLib>
*/

void CSubclassList::LoadFromTables(CGTabTableList &tableList)
{
   // Clean out the current subclass list.
   // The result of LoadFromTables() is ONLY what comes from the tables. No leftovers.
   this->empty();  // Does more than RemoveAll(). RemoveAll() call would make memory leak.

   CGTabTable *table = tableList.GetTable("SubclassDB");
   if (table != NULL)
   {
      for (int rowI = 0; rowI < table->GetRowCount(); rowI++)
      {
         CGTabRow *row = table->GetRowAt(rowI);

         if (row != NULL)
         {
            CString subclassName, elementName, deviceType, pin1Name, pin2Name, pin3Name, pin4Name, value, ptol, ntol;

            // Don't need to check "get" here, it works and we have a value or it failed and value is empty.
            row->GetCellValueAt(0, subclassName);
            row->GetCellValueAt(1, elementName);
            row->GetCellValueAt(2, deviceType);
            row->GetCellValueAt(3, pin1Name);
            row->GetCellValueAt(4, pin2Name);
            row->GetCellValueAt(5, pin3Name);
            row->GetCellValueAt(6, pin4Name);
            row->GetCellValueAt(7, value);
            row->GetCellValueAt(8, ptol);
            row->GetCellValueAt(9, ntol);

            if (!subclassName.IsEmpty())
            {
               CSubclass *subclass = this->findSubclass(subclassName);
               if (subclass == NULL)
               {
                  subclass = new CSubclass(subclassName);
                  this->Add(subclass);
               }

               DeviceTypeTag deviceTypeTag = stringToDeviceTypeTag(deviceType);
               CSubclassElement *el = new CSubclassElement(elementName, deviceTypeTag, pin1Name, pin2Name, pin3Name, pin4Name, value, ptol, ntol);
               subclass->addElement(el);
               subclass->setNumElements( subclass->getNumElements() + 1 );
            }
         }
      }
   }
}

void CSubclassList::SaveToTables(CGTabTableList &tableList)
{
   // One Table, to be completely replaced by current CSubclassList content. Even if the table
   // is not empty and the current CSubclassList is empty.

   tableList.DeleteTable("SubclassDB");

   CGTabTable *table = tableList.GetTable("SubclassDB");
   if (table == NULL)
   {
      table = new CGTabTable("SubclassDB");
      tableList.AddTable(table);
   }

   table->AddColumnHeading("SubclassName");
   table->AddColumnHeading("ElementName");
   table->AddColumnHeading("DeviceType");
   table->AddColumnHeading("Pin1Refname");
   table->AddColumnHeading("Pin2Refname");
   table->AddColumnHeading("Pin3Refname");
   table->AddColumnHeading("Pin4Refname");
   table->AddColumnHeading("Value");
   table->AddColumnHeading("PlusTol");
   table->AddColumnHeading("MinusTol");

   for (int subclassI = 0; subclassI < this->GetCount(); subclassI++)
   {
      CSubclass *sc = this->GetAt(subclassI);
      int numElements = sc->getNumElements();

      for (int elemI = 0; elemI < sc->getNumElements(); elemI++)
      {
         CSubclassElement *el = sc->getElementAt(elemI);
   
         // The row is a match in tables to what is (at time of writing) found in DB.
         // query.Format("SELECT SubclassName, ElementName, ElementDevType, Pin1Refname, Pin2Refname, Pin3Refname, Pin4Refname, ElementValue, PlusTol, MinusTol FROM %s" , tabledef.GetTableName());

         CGTabRow *row = new CGTabRow();         // COLUMNS
         row->AddCellValue(sc->getName());       // Subclass Name
         row->AddCellValue(el->getName());       // Element Name
         row->AddCellValue(deviceTypeTagToValueString(el->getDeviceType()));  // Device Type
         row->AddCellValue(el->getPin1Name());   // Pin 1 Name
         row->AddCellValue(el->getPin2Name());   // Pin 2 Name
         row->AddCellValue(el->getPin3Name());   // Pin 3 Name
         row->AddCellValue(el->getPin4Name());   // Pin 4 Name
         row->AddCellValue(el->getValue());      // Value
         row->AddCellValue(el->getPlusTol());    // Plus Tolerance
         row->AddCellValue(el->getMinusTol());   // Minus Tolerance

         table->AddRow(row);
      }
   }

}

int CSubclassList::LoadFromXML(CString xmlFilename)
{
   this->empty();
   int totalLoadedElementCount = 0;

   if (fileExists(xmlFilename))
   {

      CSubclassXmlFileReader *fp = new CSubclassXmlFileReader();

      if(!fp->Open(xmlFilename, CFile::modeRead | CFile::shareDenyNone))
      {
         CString errmsg;
         errmsg.Format("Failed to open file [%s] for reading.", xmlFilename);
         ErrorMessage(errmsg, "Error Reading Subclass XML File");
      }
      else
      {
         CSubclass *curSubclass = NULL;
         CSubclassElement *curElement = NULL;

         CString buf;
         while (fp->ReadString(buf))
         {
            buf.Trim();
            CSupString line(buf);
            CStringArray fields;
            line.ParseQuote(fields, " ");

            if (fields.GetCount() > 0)
            {
               bool endTag = false;
               CString field1 = fields.GetAt(0);

               if (field1.Left(1) == "<")
               {
                  field1.TrimLeft("<");
                  field1.TrimRight(">");

                  if (field1.Left(1) == "/")
                  {
                     int jjjj = 0; // end section
                     endTag = true;
                     field1.TrimLeft("/");
                  }

                  int jj = 0;
                  if (field1.CompareNoCase("SubclassLib") == 0)
                  {
                     int jjj = 0; // start lib or end lib, so far no real need to do anything here.
                  }
                  else if (field1.CompareNoCase("Subclass") == 0)
                  {
                     if (endTag)
                     {
                        // terminate/clear
                        curSubclass = NULL;
                     }
                     else
                     {
                        CString subclassName;

                        // start new subclass entry
                        for (int i = 1; i < fields.GetCount(); i++)
                        {
                           CSupString paramStr(fields.GetAt(i));
                           paramStr.TrimRight(">");
                           CStringArray params;
                           paramStr.ParseQuote(params, "=");
                           if (params.GetCount() > 1)
                           {
                              CString paramName(params.GetAt(0));
                              CString paramVal(params.GetAt(1));

                              if (paramName.CompareNoCase("Name") == 0)
                                 subclassName = paramVal;
                           }
                        }

                        if (!subclassName.IsEmpty())
                        {
                           if (!this->findSubclass(subclassName))
                           {
                              // add new
                              curSubclass = new CSubclass(subclassName);
                              this->Add(curSubclass);
                           }
                           else
                           {
                              // ignore duplicate
                           }
                        }
                     }
                  }
                  else if (field1.CompareNoCase("Element") == 0)
                  {
                     if (endTag)
                     {
                        // terminate/clear
                        curElement = NULL;
                     }
                     else
                     {
                        CString elementName;
                        CString devType;
                        CString value;
                        CString plusTol;
                        CString minusTol;

                        // start new subclass element entry
                        for (int i = 1; i < fields.GetCount(); i++)
                        {
                           CSupString paramStr(fields.GetAt(i));
                           paramStr.TrimRight(">");
                           CStringArray params;
                           paramStr.ParseQuote(params, "=");
                           if (params.GetCount() > 1)
                           {
                              CString paramName(params.GetAt(0));
                              CString paramVal(params.GetAt(1));

                              if (paramName.CompareNoCase("Name") == 0)
                                 elementName = paramVal;
                              else if (paramName.Compare("DeviceType") == 0)
                                 devType = paramVal;
                              else if (paramName.Compare("Value") == 0)
                                 value = paramVal;
                              else if (paramName.Compare("PlusTolerance") == 0)
                                 plusTol = paramVal;
                              else if (paramName.Compare("MinusTolerance") == 0)
                                 minusTol = paramVal;
                           }
                        }

                        if (curSubclass != NULL)
                        {
                           DeviceTypeTag devTypeTag = stringToDeviceTypeTag(devType);
                           if (devTypeTag != deviceTypeUnknown && devTypeTag != deviceTypeUndefined)
                           {
                              // add new
                              curElement = new CSubclassElement(elementName, devTypeTag, "", "", value, plusTol, minusTol);
                              curSubclass->addElement(curElement);
                              curSubclass->setNumElements( curSubclass->getNumElements() + 1 );
                              totalLoadedElementCount++;
                           }
                        }
                     }
                  }
                  else if (field1.CompareNoCase("Pin") == 0)
                  {
                     if (endTag)
                     {
                        // terminate/clear
                     }
                     else
                     {
                        CString pinName;
                        CString pinFunction;

                        // start new subclass element entry
                        for (int i = 1; i < fields.GetCount(); i++)
                        {
                           CSupString paramStr(fields.GetAt(i));
                           paramStr.TrimRight(">");
                           CStringArray params;
                           paramStr.ParseQuote(params, "=");
                           if (params.GetCount() > 1)
                           {
                              CString paramName(params.GetAt(0));
                              CString paramVal(params.GetAt(1));

                              if (paramName.CompareNoCase("Name") == 0)
                                 pinName = paramVal;
                              else if (paramName.Compare("Function") == 0)
                                 pinFunction = paramVal;
                           }
                        }

                        if (!pinName.IsEmpty() && !pinFunction.IsEmpty() && curElement != NULL)
                        {
                           // Order of pins (i.e. pin number) in this structure is fixed. This pin num determines
                           // in which pin setting the pin name shall be saved. I.e. this it not necessarily the
                           // pin num and in pin refname, it is the pin num within the element.
                           // This is 1-based, not 0-based. 0 = not a valid pin function for curElement devType.
                           int pinNum = curElement->getPinNumForFunction(pinFunction);

                           switch (pinNum)
                           {
                           case 1:
                              curElement->setPin1Name(pinName);
                              break;
                           case 2:
                              curElement->setPin2Name(pinName);
                              break;
                           case 3:
                              curElement->setPin3Name(pinName);
                              break;
                           case 4:
                              curElement->setPin4Name(pinName);
                              break;
                           default:
                              // Ignore all others -- out of supported range.
                              break;
                           }

                        }
                     }
                  }
               }
               else
               {
                  // Not expecting any lines that don't start with "<", ignore them.
               }
            }
         }

         // aka close(fp)
         delete fp;
      }
   }

   return totalLoadedElementCount; // count of elements loaded, not subclasses loaded (spans subclasses)
}


/******************************************************************************
*/

int CSubclassList::Load(CGTabTableList *tableList)
{
   // This should be used internally by apps like Importers or Exporters.
   // It will attempt load from DB first.
   // If that results in non-zero count of subclasses then that is what shall be kept.
   // If load from DB results in zero then it will attempt load from tables.
   
   LoadFromDB();
   if (this->GetCount() < 1 && tableList != NULL)
   {
      LoadFromTables(*tableList);
   }

   return this->GetCount();
}
   
/******************************************************************************
*/

int CSubclassList::LoadFromDB()
{
   this->empty();

   int loadedCount = 0;

   CDBInterface db;
   if (db.Connect())
   {
      CDBSubclassElementList *existingSubclassList = db.LookupSubclassElements(""); // Get all
      
      if (existingSubclassList != NULL)
      {
         for (int i = 0; i < existingSubclassList->GetCount(); i++)
         {
            CDBSubclassElement *dbel = (CDBSubclassElement*)existingSubclassList->GetAt(i);

            if (dbel != NULL)
            {
               CSubclass *sc = this->findSubclass(dbel->getSubclassName());
               if (sc == NULL)
               {
                  // Add new subclass
                  sc = new CSubclass(dbel->getSubclassName());
                  this->Add(sc);
               }

               CSubclassElement *el = new CSubclassElement(dbel->getElementName(), stringToDeviceTypeTag(dbel->getDeviceType()),
                  dbel->getPin1Refname(), dbel->getPin2Refname(), dbel->getPin3Refname(), dbel->getPin4Refname(),
                  dbel->getValue(), dbel->getPlusTol(), dbel->getMinusTol());

               sc->addElement(el);
               sc->setNumElements( sc->getNumElements() + 1 );
               loadedCount++;
            }
         }
      }
   }

   return loadedCount; // coult of elements loaded, not subclasses loaded
}

/******************************************************************************
*/
bool CSubclass::LoadFromDB()
{
   CDBInterface db;
   if (db.Connect())
   {
      CDBSubclassElementList *dbelementList = db.LookupSubclassElements(this->getName());
      
      if (dbelementList != NULL)
      {
         this->m_elements.empty();
         int elementCnt = 0;

         for (int i = 0; i < dbelementList->GetCount(); i++)
         {
            CDBSubclassElement *dbel = (CDBSubclassElement*)dbelementList->GetAt(i);

            if (dbel != NULL)
            {
               CSubclassElement *el = new CSubclassElement(dbel->getElementName(), stringToDeviceTypeTag(dbel->getDeviceType()),
                  dbel->getPin1Refname(), dbel->getPin2Refname(), dbel->getPin3Refname(), dbel->getPin4Refname(),
                  dbel->getValue(), dbel->getPlusTol(), dbel->getMinusTol());

               this->addElement(el);
               this->setNumElements( ++elementCnt );
            }
         }

         delete dbelementList;

         return (elementCnt > 0);
      }
   }

   return false;
}

CSubclass& CSubclass::operator=(const CSubclass &other)
{
   if (&other != NULL)
   {
      this->m_deviceType   = other.m_deviceType;
      this->m_elements     = other.m_elements;
      this->m_numElements  = other.m_numElements;
      this->m_subclassName = other.m_subclassName;
   }

   return *this;
}

DeviceTypeTag CSubclass::getElementDeviceType()
{
   // Map's parent (this) device type to element's device type

   switch (m_deviceType)
   {
   case deviceTypeCapacitorArray:
      return deviceTypeCapacitor;
   case deviceTypeDiodeArray:
      return deviceTypeDiode;
   case deviceTypeFilter:
      return deviceTypeFilter;
   case deviceTypeIC:
      return deviceTypeIC;
   case deviceTypeICDigital:
      return deviceTypeIC;
   case deviceTypeICLinear:
      return deviceTypeIC;
   case deviceTypeResistorArray:
      return deviceTypeResistor;
   case deviceTypeSwitch:
      return deviceTypeSwitch;
   case deviceTypeTransformer:
      return deviceTypeTransformer;
   case deviceTypeTransistorArray:
      return deviceTypeTransistor;
   }

   return deviceTypeUnknown;
}

CSubclassElement *CSubclass::findNthElementUsingPin(int nth, CString pinRefname)
{
   // Disallow empty pinRefname in search because many elements have
   // only two pins, hence pin3 and pin3 will be blank, and we don't
   // want to get matches on that since it would be a random result.

   // Looking for nth element using pin, there might be more than one
   // element using same pin. Counting for nth starts at 1.

   if (!pinRefname.IsEmpty())
   {
      int n = 0;

      for (int i = 0; i < m_elements.GetCount(); i++)
      {
         CSubclassElement *el = m_elements.GetAt(i);

         if (
            el->getPin1Name().Compare(pinRefname) == 0 ||
            el->getPin2Name().Compare(pinRefname) == 0 ||
            el->getPin3Name().Compare(pinRefname) == 0 ||
            el->getPin4Name().Compare(pinRefname) == 0
            )
         {
            n++; // Found one

            if (n == nth)
               return el;
         }

      }
   }

   return NULL;
}

bool CSubclass::isValidMatch(CCamCadData &camCadData, DataStruct *pcbCompData, CString &errMsg)
{
   // The rule handed down from Mr. Laing is that a match is valid if
   // all pins referenced in the subclass elements are present in the pcb component.
   // The component may have more pins that are used in elements, that is okay,
   // and allows for the "unuused pins" scenario.
   // The component does not have to have the subclass attrib itself set.
   // E.g. this could be used in a utility that determines what subclasses
   // might be validaly assignable to a given comp, the comp does not neet to
   // have any subclass set yet.

   if (pcbCompData != NULL && pcbCompData->getDataType() == dataTypeInsert)
   {
      BlockStruct *compGeomBlk = camCadData.getBlockAt( pcbCompData->getInsert()->getBlockNumber() );

      if (compGeomBlk != NULL)
      {
         for (int i = 0; i < this->getNumElements() && i < this->m_elements.GetCount(); i++)
         {
            CSubclassElement *el = this->getElementAt(i);

            int pinsReqd = el->getNumPinsRequired();

            for (int pinI = 1; pinI <= pinsReqd; pinI++)
            {
               CString pinIName( el->getPinNName( pinI ) );

               if (pinIName.IsEmpty())
               {
                  // Subclass is missing a required pin, we'll call that invalid, even though that is
                  // not really part of the rule from Mark.
                  errMsg.Format("Invalid Subclass - Element is missing a required pin refname");
                  return false;
               }
               else
               {
                  if (!hasPin(compGeomBlk, pinIName))
                  {
                     errMsg.Format("Invalid subclass match to component. Component geometry %s is missing element pin %s.", compGeomBlk->getName(), pinIName);
                     return false;
                  }
               }
            }
         }
      }

      // Failure returns above and below. If we get here then all is well
      return true;
   }

   // No comp data or comp data is not an insert
   return false;
}

bool CSubclass::hasPin(BlockStruct *pcbCompBlk, CString pinRefname)
{
   if (pcbCompBlk != NULL)
   {
      POSITION pos = pcbCompBlk->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = pcbCompBlk->getNextDataInsert(pos);

         if (data->isInsertType(insertTypePin) &&
            data->getInsert()->getRefname().Compare( pinRefname ) == 0)
         {
            return true;
         }
      }
   }

   return false;
}
