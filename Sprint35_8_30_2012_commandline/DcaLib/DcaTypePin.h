// $Header: /CAMCAD/DcaLib/DcaTypePin.h 4     6/30/07 3:01a Kurt Van Ness $

#if !defined(__DcaTypePin_h__)
#define __DcaTypePin_h__

#pragma once

#include "Dca.h"
#include "DcaContainer.h"

class CAttributes;
class CTypePinList;
class TypeStruct;
class CCamCadData;

//_____________________________________________________________________________
class TypePinStruct
{
private:
   int      m_entityNumber;
   CString  m_physicalPinName;       // pinname of geom
   CString  m_logicalPinName;       // pinname of gate/logical
   char     m_gateNumber;        
   char     m_swap;
   char     m_pinType;    
   DbFlag   m_flagBits;
   CAttributes* m_attributes;

   CTypePinList& m_parentTypePinList;

public:
   TypePinStruct(CTypePinList& parentTypePinList,int entityNumber = -1);
   ~TypePinStruct();

   TypePinStruct& operator=(const TypePinStruct& other);

   CCamCadData& getCamCadData();

   // accessors
   long getEntityNumber() const;
   //void setEntityNumber(long entityNumber);

   const CString& getPhysicalPinName() const;
   void setPhysicalPinName(const CString& pinName);

   const CString& getLogicalPinName() const;
   void setLogicalPinName(const CString& pinName);

   int getGateNumber() const;
   void setGateNumber(int gateNumber);

   int getSwap() const;
   void setSwap(int swapCode);

   int getPinType() const;
   void setPinType(int pinType);

   DbFlag getFlags() const;
   void setFlags(DbFlag attribFlag);
   void setFlagBits(DbFlag mask);
   void clearFlagBits(DbFlag mask);

   CAttributes*& getAttributesRef();
   CAttributes& attributes();

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CTypePinList // : public CTypedPtrList<CPtrList, TypePinStruct*>
{
private:
   CTypedPtrListContainer<TypePinStruct*> m_list;
   TypeStruct& m_parentTypeStruct;

public:
   CTypePinList(TypeStruct& parentTypeStruct);
   ~CTypePinList();

   void empty();

   CCamCadData& getCamCadData();

   TypeStruct& getParentTypeStruct();

   POSITION GetHeadPosition() const;
   TypePinStruct* GetNext(POSITION& pos) const;
   void removeAt(POSITION& pos);

   TypePinStruct* addTypePin(int entityNumber = -1);
   //TypePinStruct* addTypePin(const TypePinStruct& otherTypePinStruct,int entityNumber = -1);

   void takeData(CTypePinList& otherTypePinList);

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

#endif
