// $Header: /CAMCAD/5.0/Attribute.h 17    6/17/07 8:50p Kurt Van Ness $

#if !defined(__Attribute_H__)
#define __Attribute_H__

#pragma once

#include <afxtempl.h> 
#include "DbUtil.h" 
#include "TypedContainer.h" 
#include "Attrib.h" 
#include "DcaAttributes.h" 

#ifdef UseOldCAttribute
//_____________________________________________________________________________
class COldAttribute //class CAttribute
{
public:
   int valueType;
   union 
   {
      int      val_str;
      int      val_int;
      double   val_dbl;
   };

   double x, y, rotation, height, width;  // the attribute is an offset to the mirror and rotation of
                                          // its parent.
   short          penWidthIndex;
   unsigned char  proportional:1;
   unsigned char  visible:1;
   unsigned char  never_mirror:1;  // this attribute is always unmirrored, regardsless of inserts etc...
   unsigned char  inherited:1;
   unsigned char  textAlignment:2; // 0=left, 1=center, 2=right
   unsigned char  lineAlignment:2; // 0=bottom, 1=center, 2=top
   short          layer;
   DbFlag         flg;             // see dbutil.h ATTRIB_xxx

public:
   ~CAttribute();
};

//_____________________________________________________________________________
class CNamedAttribute
{
private:
   CString m_name;
   Attrib* m_attribute;

public:
   CNamedAttribute(const CString& name,Attrib* attribute)
   {
      m_name = name;
      m_attribute = attribute;
   }

   CString getName() { return m_name; }
   Attrib* getAttribute() { return m_attribute; }
};

//_____________________________________________________________________________
class CAttributeList : public CTypedListContainer<CPtrList,CAttribute*>
{
private:
   static int m_refCount;

public:
   CAttributeList();
   ~CAttributeList();
};

//_____________________________________________________________________________
class CAttributeListMap : public CTypedMapWordToObContainer<CAttributeList*>
{
public:
   CAttributeListMap(int blockSize=10);
};

//_____________________________________________________________________________
class CAttributeMap
{
private:
   CAttributeListMap m_map;

public:
   CAttributeMap(int blockSize=10);
   ~CAttributeMap();

   // Attributes
   INT_PTR GetCount() const;
   bool IsEmpty() const;

   // Operations
   bool Lookup(WORD key,CAttribute*& attribute) const;
   void SetAt(WORD key,CAttribute* newValue);

   bool RemoveKey(WORD key);
   void RemoveAll();

   POSITION GetStartPosition() const;
   void GetNextAssoc(POSITION& rNextPosition,WORD& rKey,CAttribute*& attribute) const;
   void GetNextAssoc(POSITION& rNextPosition,WORD& rKey,void*& rValue) const
     { GetNextAssoc(rNextPosition,rKey,(CAttribute*&)rValue); }

   // Attribute list support
   bool Lookup(WORD key,CAttributeList*& attributeList) const;
   bool Lookup(WORD key,void*& rValue) const
     { return Lookup(key,(CAttribute*&)rValue); }
   void GetNextAssoc(POSITION& rNextPosition,WORD& rKey,CAttributeList*& attributeList) const;
};
#endif // #ifdef UseOldCAttribute

#endif
