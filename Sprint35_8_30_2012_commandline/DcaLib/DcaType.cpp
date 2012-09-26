// $Header: /CAMCAD/DcaLib/DcaType.cpp 4     6/30/07 3:00a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaType.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"

//_____________________________________________________________________________
//TypeStruct::TypeStruct()
//: m_attributes(NULL)
//, m_blockNumber(0)
//, m_entityNumber(0)
//{
//}

TypeStruct::TypeStruct(CDeviceTypeList& parentList,const CString& name,int entityNumber)
: m_parentList(parentList)
, m_attributes(NULL)
, m_blockNumber(0)
, m_entityNumber(entityNumber)
, m_name(name)
, m_typePinList(*this)
{
   if (m_entityNumber < 0)
   {
      m_entityNumber = getCamCadData().allocateEntityNumber();
   }
}

// does not copy the CTypePinList
TypeStruct::TypeStruct(const TypeStruct& other,int entityNumber)
: m_parentList(other.m_parentList)
, m_attributes(NULL)
, m_blockNumber(other.getBlockNumber())
, m_entityNumber(entityNumber)
, m_name(other.getName())
, m_typePinList(*this)
{
   if (m_entityNumber < 0)
   {
      m_entityNumber = getCamCadData().allocateEntityNumber();
   }

   if (other.getAttributes() != NULL)
   {
      attributes() = *(other.getAttributes());
   }
}

TypeStruct::~TypeStruct()
{
   delete m_attributes;
}

CCamCadData& TypeStruct::getCamCadData()
{
   return m_parentList.getCamCadData();
}

CAttributes& TypeStruct::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

//_____________________________________________________________________________
CDeviceTypeList::CDeviceTypeList(CDeviceTypeDirectory& parentDirectory)
: m_parentDirectory(parentDirectory)
{
}

CDeviceTypeList::~CDeviceTypeList()
{
}

CCamCadData& CDeviceTypeList::getCamCadData()
{
   return m_parentDirectory.getCamCadData();
}

TypeStruct* CDeviceTypeList::addType(const CString& name,int entityNumber)
{
   getCamCadData().allocateEntityNumber(entityNumber);

   TypeStruct* typeStruct = new TypeStruct(*this,name,entityNumber);

   m_typeList.AddTail(typeStruct);

   return typeStruct;
}

TypeStruct* CDeviceTypeList::addType(const TypeStruct& otherTypeStruct,int entityNumber)
{
   TypeStruct* typeStruct = new TypeStruct(otherTypeStruct,entityNumber);

   m_typeList.AddTail(typeStruct);

   return typeStruct;
}

int CDeviceTypeList::GetCount() const
{
   return m_typeList.GetCount();
}

POSITION CDeviceTypeList::GetHeadPosition() const
{
   return m_typeList.GetHeadPosition();
}

TypeStruct* CDeviceTypeList::GetNext(POSITION& pos) const
{
   return m_typeList.GetNext(pos);
}

TypeStruct* CDeviceTypeList::GetAt(POSITION pos) const
{
   return m_typeList.GetAt(pos);
}

TypeStruct* CDeviceTypeList::findType(const CString& name) const
{
   TypeStruct* typeStruct = NULL;

   for (POSITION pos = m_typeList.GetHeadPosition();;)
   {
      if (pos == NULL)
      {
         typeStruct = NULL;
         break;
      }

      typeStruct = m_typeList.GetNext(pos);

      if (typeStruct->getName().Compare(name) == 0)
      {
         break;
      }
   }

   return typeStruct;
}

TypeStruct* CDeviceTypeList::removeAt(POSITION pos)
{
   TypeStruct* typeStruct = m_typeList.GetAt(pos);

   m_typeList.RemoveAt(pos);

   return typeStruct;
}

void CDeviceTypeList::deleteAt(POSITION pos)
{
   TypeStruct* typeStruct = removeAt(pos);

   delete typeStruct;
}

bool CDeviceTypeList::IsEmpty() const
{
   return (m_typeList.GetCount() == 0);
}

void CDeviceTypeList::addTail(TypeStruct* typeStruct)
{
   m_typeList.AddTail(typeStruct);
}

//void CDeviceTypeList::takeData(CDeviceTypeList& other)
//{
//   for (POSITION pos = other.GetHeadPosition();pos != NULL;)
//   {
//      POSITION oldPos = pos;
//      TypeStruct* typeStruct = other.GetNext(pos);
//
//      other.m_typeList.RemoveAt(oldPos);
//
//      addType(typeStruct->getName(),entityNumber);
//   }
//}

//_____________________________________________________________________________
CDeviceTypeArrayWithMap::CDeviceTypeArrayWithMap(int size,bool isContainer)
: CTypedPtrArrayWithMap<TypeStruct>(size,isContainer)
{
}

//_____________________________________________________________________________
CDeviceTypeDirectory::CDeviceTypeDirectory(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_deviceTypeList(*this)
, m_deviceTypeMap(NULL)
{
}

CDeviceTypeDirectory::~CDeviceTypeDirectory()
{
   delete m_deviceTypeMap;
}

CCamCadData& CDeviceTypeDirectory::getCamCadData()
{
   return m_camCadData;
}

void CDeviceTypeDirectory::reallocateMap() const
{
   delete m_deviceTypeMap;

   int size = max(100,(int)(1.3 * m_deviceTypeList.GetCount()));

   m_deviceTypeMap = new CDeviceTypeArrayWithMap(size,false);

   for (POSITION pos = m_deviceTypeList.GetHeadPosition();pos != NULL;)
   {
      TypeStruct* typeStruct = m_deviceTypeList.GetNext(pos);
      m_deviceTypeMap->add(typeStruct->getName(),typeStruct);
   }
}

void CDeviceTypeDirectory::deallocateMap() const
{
   delete m_deviceTypeMap;
   m_deviceTypeMap = NULL;
}

CDeviceTypeArrayWithMap& CDeviceTypeDirectory::getDeviceTypeMap() const
{
   if (m_deviceTypeMap == NULL)
   {
      reallocateMap();
   }

   return *m_deviceTypeMap;
}

// returns true if the arrays are in sync
//bool CDeviceTypeDirectory::sync()
//{
//   bool retval = (m_deviceTypeMap != NULL && 
//                  m_deviceTypeList.GetCount() == m_deviceTypeMap->getSize());
//
//   if (! retval)
//   {
//      resync();
//   }
//
//   return retval;
//}

TypeStruct* CDeviceTypeDirectory::getType(const CString& deviceName) const
{
   TypeStruct* typeStruct = NULL;

   if (! getDeviceTypeMap().lookup(deviceName,typeStruct))
   {
      typeStruct = NULL;
   }

   return typeStruct;
}

TypeStruct& CDeviceTypeDirectory::getDefinedType(const CString& deviceName)
{
   TypeStruct* typeStruct = getType(deviceName);

   if (typeStruct == NULL)
   {
      typeStruct = m_deviceTypeList.addType(deviceName,m_camCadData.allocateEntityNumber());

      typeStruct->setBlockNumber( -1);   // no name assigned

      getDeviceTypeMap().add(deviceName,typeStruct);
   }

   return *typeStruct;
}

TypeStruct* CDeviceTypeDirectory::addType(const CString& deviceName,int entityNumber)
{
   getCamCadData().allocateEntityNumber(entityNumber);

   TypeStruct* typeStruct = m_deviceTypeList.addType(deviceName,entityNumber);

   // if more than one type has the same name, the last added type is in the map
   getDeviceTypeMap().add(deviceName,typeStruct);

   return typeStruct;
}

TypeStruct* CDeviceTypeDirectory::addType(const TypeStruct& otherTypeStruct,int entityNumber)
{
   TypeStruct* typeStruct = m_deviceTypeList.addType(otherTypeStruct,entityNumber);

   // if more than one type has the same name, the last added type is in the map
   getDeviceTypeMap().add(typeStruct->getName(),typeStruct);

   return typeStruct;
}

int CDeviceTypeDirectory::getCount() const
{
   int count = m_deviceTypeList.GetCount();

   return count;
}

POSITION CDeviceTypeDirectory::getHeadPosition() const
{
   POSITION pos = m_deviceTypeList.GetHeadPosition();

   return pos;
}

TypeStruct* CDeviceTypeDirectory::getNext(POSITION& pos) const
{
   TypeStruct* typeStruct = m_deviceTypeList.GetNext(pos);

   return typeStruct;
}

TypeStruct* CDeviceTypeDirectory::getAt(POSITION pos) const
{
   TypeStruct* typeStruct = m_deviceTypeList.GetAt(pos);

   return typeStruct;
}

void CDeviceTypeDirectory::deleteAt(POSITION pos)
{
   TypeStruct* listTypeStruct = getAt(pos);

   if (m_deviceTypeMap != NULL)
   {
      TypeStruct* mapTypeStruct = getDeviceTypeMap().getAt(listTypeStruct->getName());

      // If the map contains a different TypeStruct, no need to deallocate map.
      // If the map contains this TypeStruct, deallocate it so that when it is rebuilt, another TypeStruct with the same name
      // will be put into the map.
      if (mapTypeStruct == listTypeStruct)
      {
         deallocateMap();
      }
   }

   m_deviceTypeList.deleteAt(pos);
}

TypeStruct* CDeviceTypeDirectory::removeAt(POSITION pos)
{
   TypeStruct* listTypeStruct = getAt(pos);

   if (m_deviceTypeMap != NULL)
   {
      TypeStruct* mapTypeStruct = getDeviceTypeMap().getAt(listTypeStruct->getName());

      // If the map contains a different TypeStruct, no need to deallocate map.
      // If the map contains this TypeStruct, deallocate it so that when it is rebuilt, another TypeStruct with the same name
      // will be put into the map.
      if (mapTypeStruct == listTypeStruct)
      {
         deallocateMap();
      }
   }

   m_deviceTypeList.removeAt(pos);

   return listTypeStruct;
}

bool CDeviceTypeDirectory::isEmpty() const
{
   return m_deviceTypeList.IsEmpty();
}

void CDeviceTypeDirectory::takeData(CDeviceTypeDirectory& other)
{
   for (POSITION pos = other.getHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      TypeStruct* typeStruct = other.getNext(pos);
      other.removeAt(oldPos);
      TypeStruct* newTypeStruct = this->addType(*typeStruct,typeStruct->getEntityNumber());

      newTypeStruct->getTypePinList().takeData(typeStruct->getTypePinList());
   }
}

void CDeviceTypeDirectory::removeUnusedDeviceTypes()
{
   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      TypeStruct* typeStruct = getNext(pos);

      if (typeStruct->getBlockNumber() < 0)
      {
         deleteAt(oldPos);
      }
   }
}




