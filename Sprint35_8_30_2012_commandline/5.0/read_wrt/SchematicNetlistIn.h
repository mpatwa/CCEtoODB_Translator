// $Header: /CAMCAD/4.5/read_wrt/SchematicNetlistIn.h 2     9/29/06 4:18p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/

#if !defined(__Schematic_Netlist_In_h__)
#define __Schematic_Netlist_In_h__

#pragma once

#include "DcaContainer.h"


class CSchematicNetlistFileReader
{
private:
   CCamCadData m_tempCamCadData;
   CNetList *m_netList;

public:
   CSchematicNetlistFileReader();
   ~CSchematicNetlistFileReader();

   void ReadCamcadNetlistFile(CString filename);
   void ReadVBKeyinNetlistFile(CString filename);
   void ReadBoardstationNetlistFile(CString filename);
   void ReadViewlogicNetlistFile(CString filename);
   void ReadPadsPowerPcbNetlist(CString filename);

   bool GetNextRecord(CFormatStdioFile &infile, CString &recordRetval);


   // When reading is done, this netlist has the result
   CNetList *GetNetList()     { return m_netList; }
};


/******************************************************************************
* CMentorBoardstationNetlistReader
* Code for this is in MentorIn.cpp
*/
class CMentorBoardstationNetlistReader
{
   // This is for usage "outside" of this MentorIn importer.
   // It allows access to the NetList reader portion of the regular importer code.

public:
   CMentorBoardstationNetlistReader(CString filename);
   ~CMentorBoardstationNetlistReader();

   void ReadNetlist(CNetList &netlistToPopulate);
};

/******************************************************************************
*/

class CViewlogicDataPair
{
private:
   CString m_value_1;
   CString m_value_2;

public:
   CViewlogicDataPair(CString value1, CString value2)       { m_value_1 = value1;   m_value_2 = value2; }

   CString GetValue1()    { return m_value_1; }
   CString GetValue2()    { return m_value_2; }
};

class CViewlogicDataPairList : public CTypedPtrArrayContainer<CViewlogicDataPair*>
{
};


class CViewlogicSignal : public CViewlogicDataPair
{
public:
   CViewlogicSignal(CString signame, CString pinname);

   CString GetSignalName()    { return GetValue1(); }
   CString GetPinName()       { return GetValue2(); }
};

class CViewlogicSignalList : public CTypedPtrArrayContainer<CViewlogicSignal*>
{
};

//-------------------------------------------------------------------------------

class CViewlogicPackage
{
private:
   CString m_pkgname;
   CStringArray m_pinPortNames;    // in order of logical pin num
   CStringArray m_pinPhysNums;     // in order of logical pin num, this is the "traditional" pin refname

   CViewlogicSignalList m_signalList;

public:
   CViewlogicPackage(CString pkgname)          { m_pkgname = pkgname; }
   ~CViewlogicPackage()                        {}

   void AddPin(CString pinname)                { m_pinPortNames.Add(pinname); }
   void AddPinNum(int logNum, CString physNum) { m_pinPhysNums.SetAtGrow(logNum, physNum); } //int i = m_pinPhysNums.GetCount(); while (i < logNum) m_pinPhysNums.SetAtGrow(i++, 0); m_pinPhysNums.SetAtGrow(logNum, physNum); }
   int GetPinCount()                           { return m_pinPortNames.GetCount(); }
   CString GetPinPortNameAt(int indx)          { return (indx >= 0 && indx < m_pinPortNames.GetCount()) ? m_pinPortNames.GetAt(indx) : (CString)""; }
   CString GetPinPhysNumAt(int indx)           { return (indx >= 0 && indx < m_pinPhysNums.GetCount())  ? m_pinPhysNums.GetAt(indx) : (CString)""; }

   void AddSignal(CString signame, CString pinname)      { m_signalList.Add(new CViewlogicSignal(signame, pinname)); }
   int GetSignalCount()                                  { return m_signalList.GetCount(); }
   CViewlogicSignal *GetSignalAt(int i)                  { return (i >= 0 && i < m_signalList.GetCount()) ? m_signalList.GetAt(i) : NULL; }
};

class CViewlogicPackageMap : public CTypedMapStringToPtrContainer<CViewlogicPackage*>
{
};

//-------------------------------------------------------------------------------

class CViewlogicSymInstance
{
private:
   CString m_instanceName;
   CString m_compRefname;

   CStringArray m_nets;

public:
   CViewlogicSymInstance(CString instName, CString compRefname)      { m_instanceName = instName; m_compRefname = compRefname; }

   void AddNet(CString netname)        { m_nets.Add(netname);      }
   int GetNetCount()                   { return m_nets.GetCount(); }
   CString GetNetAt(int i)             { return m_nets.GetAt(i);   }

   CString GetRefname()                { return m_compRefname; }
};

class CViewlogicSymInstanceMap : public CTypedMapStringToPtrContainer<CViewlogicSymInstance*>
{
};

//-------------------------------------------------------------------------------

class CViewlogicNetlistReader
{
private:
   CString m_inputFilename;

   bool ParseNameValuePair(CString inputStr, CString &outputName, CString &outputValue);

public:
   CViewlogicNetlistReader(CString filename);
   ~CViewlogicNetlistReader();

   void ReadNetlist(CNetList &netlistToPopulate);


};

/******************************************************************************
*/

#endif

