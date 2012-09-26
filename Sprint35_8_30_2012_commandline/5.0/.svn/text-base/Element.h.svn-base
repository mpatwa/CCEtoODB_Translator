
/*****************************************************************************/
/*  
    Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2008. All Rights Reserved.
*/ 

#if !defined(__Element_h__)
#define __Element_h__

#pragma once

#include "DeviceType.h"
#include "DcaContainer.h"
#include "DcaCamCadData.h"
#include "DcaBlock.h"
#include "DcaInsert.h"


//----------------------------------------------------------------------------


// There are multiple classes of "elements" and "element lists", each has its own niche.
// The overall purpose of the Element is to provide a means to define how testable component elements
// are bundled into component packages. For example, which pins go together to form a single resistor
// in a resistor array component.
//
// The CDBSubclass and CDBSubclassElement are strictly for database interaction. They are defined
// over in ODB_Lib.h/cpp. They are arguably unnecessary, and the CSubclassElement could have been
// used directly there. However the CDBxxxx class pattern is already a set precedent, and they
// are kept simple and specific to the task of database I/O.
//
// The CSubclass and CSubclassElement were first invented for handling Data Doctor operations, and
// have capabilities specific to that task. It turned out they are also useful directly in the 
// exporters that need to access this data. These represent the Test Element Library, i.e. they
// are a generic view of the elements.
//
// The CTestSubclass and CTestElement are "convenience" classes whose purpose is to provide test elements 
// that are bound to a specific component on the PCB. It is at this level that a test element can inherit
// data from a specific component, such as the Refname or the Value (if Value is not set in the DB).
//

//// This was mostly initially implemented in DataDoctorPageSubclass.cpp/h, and there is still
//// a little bit of stuff lingering there that ought to be extracted and moved here.
//// Someday when it matters, maybe it will happen.

//----------------------------------------------------------------------------

class CSubclassElement : public CObject
{
private:
   CString m_name; // this element's name
   DeviceTypeTag m_deviceType;  // Single element's dev type, e.g is Transistor if parent is Transistor_Array
   CString m_pin1Refname;
   CString m_pin2Refname;
   CString m_pin3Refname;
   CString m_pin4Refname;
   CString m_value;
   CString m_plusTol;
   CString m_minusTol;

public:
   CSubclassElement() { m_deviceType = deviceTypeUnknown; }
   CSubclassElement(CString name, DeviceTypeTag devType, CString pin1name, CString pin2name, CString val, CString ptol, CString mtol)
   {
      setName(name);
      setDeviceType(devType);
      setPin1Name(pin1name);
      setPin2Name(pin2name);
      setValue(val);
      setPlusTol(ptol);
      setMinusTol(mtol);
   }
   CSubclassElement(CString name, DeviceTypeTag devType, CString pin1name, CString pin2name, CString pin3name, CString val, CString ptol, CString mtol)
   {
      setName(name);
      setDeviceType(devType);
      setPin1Name(pin1name);
      setPin2Name(pin2name);
      setPin3Name(pin3name);
      setValue(val);
      setPlusTol(ptol);
      setMinusTol(mtol);
   }
   CSubclassElement(CString name, DeviceTypeTag devType, CString pin1name, CString pin2name, CString pin3name, CString pin4name, CString val, CString ptol, CString mtol)
   {
      setName(name);
      setDeviceType(devType);
      setPin1Name(pin1name);
      setPin2Name(pin2name);
      setPin3Name(pin3name);
      setPin4Name(pin4name);
      setValue(val);
      setPlusTol(ptol);
      setMinusTol(mtol);
   }
   CSubclassElement(const CSubclassElement &other)    { *this = other; }

   CSubclassElement& operator=(const CSubclassElement &other);

   CString getName()                      { return m_name;        }
   DeviceTypeTag getDeviceType()          { return m_deviceType;  }
   CString getPin1Name()                  { return m_pin1Refname; }
   CString getPin2Name()                  { return m_pin2Refname; }
   CString getPin3Name()                  { return m_pin3Refname; }
   CString getPin4Name()                  { return m_pin4Refname; }
   CString getPinNName(int n);
   CString getValue()                     { return m_value;       }
   CString getPlusTol()                   { return m_plusTol;     }
   CString getMinusTol()                  { return m_minusTol;    }

   void setName(CString name)             { m_name = name;        }
   void setDeviceType(DeviceTypeTag tag)  { m_deviceType = tag;   }
   void setPin1Name(CString name)         { m_pin1Refname = name; }
   void setPin2Name(CString name)         { m_pin2Refname = name; }
   void setPin3Name(CString name)         { m_pin3Refname = name; }
   void setPin4Name(CString name)         { m_pin4Refname = name; }
   void setValue(CString val)             { m_value = val;        }
   void setPlusTol(CString val)           { m_plusTol = val;      }
   void setMinusTol(CString val)          { m_minusTol = val;     }

   // Pin function is fixed per pin type, but depends on device type.
   // E.g. when dev type calls for anode, pin1Name is always anode, pin2Name is always cathode.
   CString getPinFunction(int pinNum);
   CString getPinFunction(CString pinRefname);
   int getNumPinsRequired(); // based on dev type
   int getPinNumForFunction(CString function);

   bool containsPin(CString pinRefname)   { for (int i = 1; i <= 4; i++) if (getPinNName(i).Compare(pinRefname) == 0) return true; return false; }
};


//----------------------------------------------------------------------------

class CSubclassElementList : public CTypedObArrayContainer<CSubclassElement*>
{
public:
   CSubclassElementList& operator=(const CSubclassElementList &other);
};

//----------------------------------------------------------------------------

class CSubclass : public CObject
{
private:
   CString m_subclassName;
   DeviceTypeTag m_deviceType; // Overall device's type, e.g. Transistor_Array
   int m_numElements;

   CSubclassElementList m_elements;

public:
   CSubclass()                   {                        m_deviceType = deviceTypeUnknown; m_numElements = 0;}
   CSubclass(CString name)       { m_subclassName = name; m_deviceType = deviceTypeUnknown; m_numElements = 0;}
   CSubclass(CSubclass &other)   { *this = other; }

   CSubclass& operator=(const CSubclass &other);

   CString getName()                      { return m_subclassName; }
   void setName(CString name)             { m_subclassName = name; }

   DeviceTypeTag getDeviceType()          { return m_deviceType;  }
   void setDeviceType(DeviceTypeTag tag)  { m_deviceType = tag;   }

   // The GUI can set the number of elements, and it can be more than what is actually in the
   // list. This makes getting count problematic. When count is higher than list then those "beyond" are
   // simply undefined so far, but still considered to exist.
   void setNumElements(int num)           { m_numElements = num;  }  
   int getNumElements()                   { return m_numElements; }

   DeviceTypeTag getElementDeviceType();  // Map's parent device type to element's device type

   void addElement(CSubclassElement *el)  { m_elements.Add(el); }
   void setElementAt(int i, CSubclassElement *el)  { if (i >= 0 && i < m_numElements) m_elements.setAtGrow(i, el); }
   CSubclassElement *getElementAt(int i)  { if (i >= 0 && i < m_elements.GetCount()) return m_elements.GetAt(i); else return NULL; }

   bool LoadFromDB();

   CSubclassElement *findNthElementUsingPin(int nth, CString pinRefname);

   bool isValidMatch(CCamCadData &camCadData, DataStruct *pcbCompData, CString &errMsg);
   static bool hasPin(BlockStruct *pcbCompBlk, CString pinRefname);

};

//----------------------------------------------------------------------------

class CSubclassList : public CTypedObArrayContainer<CSubclass*>
{
public:
   CSubclass *findSubclass(CString name);
   void deleteSubclass(CSubclass *victim);

   // Apps (like exporters) should use this one.
   // It will load from DB if it can, otherwise load from tables.
   // This is what ultimately connects (say) an Exporter to the Subclass.xml file.
   int Load(CGTabTableList *tableList = NULL);

   // These are to be used for specific operation, when it is known that this
   // is the specific operation desired.
   int LoadFromDB();
   int LoadFromXML(CString xmlFilename);
   void SaveToTables(CGTabTableList &tableList);
   void LoadFromTables(CGTabTableList &tableList);
};

//----------------------------------------------------------------------------

class CSubclassXmlFileReader : public CStdioFile
{
public:
  // SubclassCommandTag GetCommandCode(CString xmlString)
};

//----------------------------------------------------------------------------





#endif
