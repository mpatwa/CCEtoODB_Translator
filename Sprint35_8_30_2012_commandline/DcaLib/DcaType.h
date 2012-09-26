// $Header: /CAMCAD/DcaLib/DcaType.h 5     6/30/07 3:00a Kurt Van Ness $

#if !defined(__DcaType_h__)
#define __DcaType_h__

#pragma once

#include "DcaTypePin.h"

class CCamCadData;
class CDeviceTypeDirectory;
class CDeviceTypeList;

//_____________________________________________________________________________
class TypeStruct
{
   //friend CTypedPtrListContainer<TypeStruct*>;
   friend CTypedListContainer<CPtrList,TypeStruct*>;
   //friend CTypedArrayWithMap<CPtrList,TypeStruct*>;
   friend CTypedArrayWithMap<CPtrArray,CMapStringToPtr,TypeStruct>;
   friend CDeviceTypeList;

   // TypeList - this list deals with part number component description.
   //            all info concerning a part desciption should be in this list.
   //            the component list may locally overwrite this values      
   // the Typelist has 2 references
   //                device name - 7400
   //                partnunmber
   //                logical / physical pin mapping
   //                the assiciated Graph_Block number
   //                values
   //                tolerances
   //                
private:
   long           m_entityNumber;///entity;
   CString        m_name;///Name;
   int            m_blockNumber;///blocknum;   // shape geometry which is linked to the type i.e 7400
   CTypePinList   m_typePinList;///TypePinList;
   CAttributes*   m_attributes;

   CDeviceTypeList& m_parentList;

public:
   //TypeStruct();
   TypeStruct(CDeviceTypeList& parentList,const CString& name,int entityNumber = -1);
   TypeStruct(const TypeStruct& other,int entityNumber = -1); // todo knv 20070624 - does not copy m_typePinList, should it?

private:
   ~TypeStruct();

public:
   CCamCadData& getCamCadData();

   CDeviceTypeList& getParentList();

   // accessors
   long getEntityNumber() const            { return m_entityNumber; }
   //void setEntityNumber(long entityNumber) { m_entityNumber = entityNumber; }

   const CString& getName() const          { return m_name; }
   //void setName(const CString& name)       { m_name = name; }

   int getBlockNumber() const              { return m_blockNumber; }
   void setBlockNumber(int blockNumber)    { m_blockNumber = blockNumber; }

   CAttributes& attributes();
   CAttributes*  getAttributes() const     { return m_attributes; }
   CAttributes*& getAttributesRef()        { return m_attributes; }
   CTypePinList& getTypePinList()          { return m_typePinList; }

   // operations
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CDeviceTypeList
{
friend CDeviceTypeDirectory;
//friend void CDeviceTypeDirectory::takeData(CDeviceTypeDirectory& other);

private:
   CTypedPtrListContainer<TypeStruct*> m_typeList;
   CDeviceTypeDirectory& m_parentDirectory;

public:
   CDeviceTypeList(CDeviceTypeDirectory& parentDirectory);
   ~CDeviceTypeList();

   void empty();

   CCamCadData& getCamCadData();

   CDeviceTypeDirectory& getParentDirectory();

   TypeStruct* addType(const CString& name,int entityNumber= -1);
   TypeStruct* addType(const TypeStruct& otherTypeStruct,int entityNumber= -1);

   int GetCount() const;
   POSITION GetHeadPosition() const;
   TypeStruct* GetNext(POSITION& pos) const;
   TypeStruct* GetAt(POSITION pos) const;
   TypeStruct* findType(const CString& name) const;
   TypeStruct* removeAt(POSITION pos);
   void deleteAt(POSITION pos);
   bool IsEmpty() const;
   //void takeData(CDeviceTypeList& other);

private:
   void addTail(TypeStruct* typeStruct);
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CDeviceTypeArrayWithMap : public CTypedPtrArrayWithMap<TypeStruct>
{
public:
   CDeviceTypeArrayWithMap(int size,bool isContainer);
};


//_____________________________________________________________________________
class CDeviceTypeDirectory
{
private:
   CCamCadData& m_camCadData;
   CDeviceTypeList m_deviceTypeList;
   mutable CDeviceTypeArrayWithMap* m_deviceTypeMap;

public:
   CDeviceTypeDirectory(CCamCadData& camCadData);
   virtual ~CDeviceTypeDirectory();

public: 
   TypeStruct* getType(const CString& deviceName) const;
   TypeStruct& getDefinedType(const CString& deviceName);

   CCamCadData& getCamCadData();

   TypeStruct* addType(const CString& name,int entityNumber= -1);
   TypeStruct* addType(const TypeStruct& otherTypeStruct,int entityNumber= -1);
   int getCount() const;
   POSITION getHeadPosition() const;
   TypeStruct* getNext(POSITION& pos) const;
   TypeStruct* getAt(POSITION pos) const;
   //TypeStruct* findType(const CString& name) const;
   void deleteAt(POSITION pos);
   bool isEmpty() const;
   void takeData(CDeviceTypeDirectory& other);
   void removeUnusedDeviceTypes();

   // deprecated
   int GetCount() const                 { return getCount(); }
   POSITION GetHeadPosition() const     { return getHeadPosition(); }
   TypeStruct* GetNext(POSITION& pos) const { return getNext(pos); }
   TypeStruct* GetAt(POSITION pos) const    { return getAt(pos); }
   TypeStruct* findType(const CString& name) const { return getType(name); }
   //void deleteAt(POSITION pos);
   bool IsEmpty() const                   { return isEmpty(); }

private:
   void reallocateMap() const;
   void deallocateMap() const;
   //bool sync();
   CDeviceTypeArrayWithMap& getDeviceTypeMap() const;
   TypeStruct* removeAt(POSITION pos);
};

typedef CDeviceTypeDirectory CTypeList;

#endif
