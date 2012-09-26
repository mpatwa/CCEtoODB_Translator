// $Header: /CAMCAD/DcaLib/DcaBus.h 6     6/30/07 2:55a Kurt Van Ness $

#if !defined(__DcaBus_h__)
#define __DcaBus_h__

#pragma once

#include "DcaEntityWithAttributes.h"
#include "DcaContainer.h"

class FileStruct;

//_____________________________________________________________________________
class CBusStruct : public CEntityWithAttributes
{
public:
   CBusStruct(CCamCadData& camCadData,CString name, long entityNumber = -1);
   ~CBusStruct();

private:
   CStringList m_netNameList;
   CStringList m_busNameList;

public:
   // Function for m_netNameList
   void AddNetName(CString netName);
   void ReplaceNetName(CString oldName, CString newName);

   POSITION GetHeadPositionNetName()     const { return m_netNameList.GetHeadPosition();    }
   CString GetNextNetName(POSITION &pos) const { return m_netNameList.GetNext(pos);         }

   // Function for m_busNameList
   void AddBusName(CString busName);
   void ReplaceBusName(CString oldName, CString newName);

   POSITION GetHeadPositionBusName()     const { return m_busNameList.GetHeadPosition();    }
   CString GetNextBusName(POSITION &pos) const { return m_busNameList.GetNext(pos);         }

   void CopyBusAndNetNameList(CBusStruct* fromBus);
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CBusList
{
friend FileStruct;

private:
   CTypedPtrListContainer<CBusStruct*> m_busList;

   CCamCadData& m_camCadData;

public:
   CBusList(CCamCadData& camCadData);

private:
   ~CBusList();

public:
   CCamCadData& getCamCadData() const;

   void AddBus(CBusStruct* bus);
   CBusStruct* AddBus(CString busName, long entityNumber = -1);
   CBusStruct* Find(CString busName);
   bool RemoveBus(CBusStruct* bus);

   POSITION GetHeadPosition() const;
   CBusStruct* GetNext(POSITION &pos) const;
   int GetCount() const;

   void FreeBusList();
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
   void Scale(double factor);
};

#endif
