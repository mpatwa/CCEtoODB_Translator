// $Header: /CAMCAD/DcaLib/DcaTypePin.cpp 4     6/30/07 3:01a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaTypePin.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"

//_____________________________________________________________________________
TypePinStruct::TypePinStruct(CTypePinList& parentTypePinList,int entityNumber)
: m_parentTypePinList(parentTypePinList)
, m_entityNumber(entityNumber)
, m_attributes(NULL)
{
   if (m_entityNumber < 0)
   {
      m_entityNumber = getCamCadData().allocateEntityNumber();
   }
}

TypePinStruct::~TypePinStruct()
{
   delete m_attributes;
}

TypePinStruct& TypePinStruct::operator=(const TypePinStruct& other)
{
   if (&other != this)
   {
      m_physicalPinName = other.m_physicalPinName;
      m_logicalPinName  = other.m_logicalPinName;
      m_gateNumber      = other.m_gateNumber;
      m_swap            = other.m_swap;
      m_pinType         = other.m_pinType;
      m_flagBits        = other.m_flagBits;

      if (other.m_attributes == NULL)
      {
         delete m_attributes;
         m_attributes = NULL;
      }
      else
      {
         attributes() = *(other.m_attributes);
      }
   }

   return *this;
}

CCamCadData& TypePinStruct::getCamCadData()
{
   return m_parentTypePinList.getCamCadData();
}

long TypePinStruct::getEntityNumber() const
{
   return m_entityNumber;
}

//void TypePinStruct::setEntityNumber(long entityNumber)
//{
//   m_entityNumber = entityNumber;
//}

const CString& TypePinStruct::getPhysicalPinName() const
{
   return m_physicalPinName;
}

void TypePinStruct::setPhysicalPinName(const CString& pinName)
{
   m_physicalPinName = pinName;
}

const CString& TypePinStruct::getLogicalPinName() const
{
   return m_logicalPinName;
}

void TypePinStruct::setLogicalPinName(const CString& pinName)
{
   m_logicalPinName = pinName;
}

int TypePinStruct::getGateNumber() const
{
   return m_gateNumber;
}

void TypePinStruct::setGateNumber(int gateNumber)
{
   m_gateNumber = gateNumber;
}

int TypePinStruct::getSwap() const
{
   return m_swap;
}

void TypePinStruct::setSwap(int swapCode)
{
   m_swap = swapCode;
}

int TypePinStruct::getPinType() const
{
   return m_pinType;
}

void TypePinStruct::setPinType(int pinType)
{
   m_pinType = pinType;
}

DbFlag TypePinStruct::getFlags() const
{
   return m_flagBits;
}

void TypePinStruct::setFlags(DbFlag attribFlag)
{
   m_flagBits = attribFlag;
}

void TypePinStruct::setFlagBits(DbFlag mask)
{
   m_flagBits |= mask;
}

void TypePinStruct::clearFlagBits(DbFlag mask)
{
   m_flagBits &= ~mask;
}

CAttributes*& TypePinStruct::getAttributesRef()
{
   return m_attributes;
}

CAttributes& TypePinStruct::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

//_____________________________________________________________________________
CTypePinList::CTypePinList(TypeStruct& parentTypeStruct)
: m_parentTypeStruct(parentTypeStruct)
{
}

CTypePinList::~CTypePinList()
{
}

void CTypePinList::empty()
{
   m_list.empty();
}

CCamCadData& CTypePinList::getCamCadData()
{
   return m_parentTypeStruct.getCamCadData();
}

TypeStruct& CTypePinList::getParentTypeStruct()
{
   return m_parentTypeStruct;
}

POSITION CTypePinList::GetHeadPosition() const
{
   return m_list.GetHeadPosition();
}

TypePinStruct* CTypePinList::GetNext(POSITION& pos) const
{
   return m_list.GetNext(pos);
}

TypePinStruct* CTypePinList::addTypePin(int entityNumber)
{
   TypePinStruct* typePinStruct = new TypePinStruct(*this,entityNumber);

   m_list.AddTail(typePinStruct);

   return typePinStruct;
}

//TypePinStruct* CTypePinList::addTypePin(const TypePinStruct& otherTypePinStruct,int entityNumber)
//{
//   TypePinStruct* typePinStruct = new TypePinStruct(*this,entityNumber);
//
//   m_list.AddTail(typePinStruct);
//
//   return typePinStruct;
//}

void CTypePinList::removeAt(POSITION& pos)
{
   m_list.RemoveAt(pos);
}

void CTypePinList::takeData(CTypePinList& otherTypePinList)
{
   for (POSITION pos = otherTypePinList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;

      TypePinStruct* otherTypePinStruct = GetNext(pos);

      removeAt(oldPos);

      TypePinStruct* newTypePinStruct = new TypePinStruct(*this,otherTypePinStruct->getEntityNumber());

      *newTypePinStruct = *otherTypePinStruct;

      m_list.AddTail(newTypePinStruct);

      delete otherTypePinStruct;
   }
}



