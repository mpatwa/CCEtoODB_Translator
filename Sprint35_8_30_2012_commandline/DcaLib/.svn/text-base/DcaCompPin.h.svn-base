// $Header: /CAMCAD/DcaLib/DcaCompPin.h 7     6/30/07 2:57a Kurt Van Ness $

#if !defined(__DcaCompPin_h__)
#define __DcaCompPin_h__

#pragma once

#include "Dca.h"
#include "DcaContainer.h"

class CAttributes;
class CPoint2d;
class PointStruct;
class CAttribute;
class CCamCadData;
class CCompPinList;
class NetStruct;
class CNetList;

enum AttributeUpdateMethodTag;
enum ValueTypeTag;

//_____________________________________________________________________________
enum VisibleTag 
{
   visibleNone   = 0,
   visibleTop    = 1,
   visibleBottom = 2,
   visibleBoth   = 3
};

//_____________________________________________________________________________
class CompPinStruct
{
friend CTypedListContainer<CPtrList,CompPinStruct*>;
friend CCompPinList;
friend NetStruct;

private:
   int      m_entityNumber;

   CString  m_refDes;
   CString  m_pinName;

   // this eliminates PINLOC
   double   m_x;
   double   m_y;
   double   m_rotationRadians;

   unsigned char  m_mirror:2;                     // init to 0, same like DataStruct.
   unsigned char  m_pinCoordinatesComplete:1;     // are pinCoordinats, mirror and rotation calculated.
   unsigned char  m_visible:2;                    //

   int      m_padstackBlockNumber;   // init to -1

   CAttributes* m_attributes;

   CCompPinList& m_parentCompPinList;

private:
   CompPinStruct(CCompPinList& parentCompPinList,const CString& refDes,const CString& pinName,int entityNumber = -1);
   ~CompPinStruct();

public:
   CCamCadData& getCamCadData() const;
   CNetList& getNetList() const;

   // static methods
   static CString getSortableReference(const CString& reference);
   static CString getPinRef(const CString& refDes,const CString& pinName,char delim = '.');

   // accessors
   long getEntityNumber() const                    { return m_entityNumber; }
   //void setEntityNumber(long entityNumber)         { m_entityNumber = entityNumber; }

   const CString& getRefDes() const                { return m_refDes; }
   void setRefDes(const CString& refDes);

   const CString& getPinName() const               { return m_pinName; }
   void setPinName(const CString& pinName);

   CString getPinRef(char delim = '.') const       { return m_refDes + delim + m_pinName; }
   CString getSortableRefDes() const;
   CString getSortablePinName() const;
   CString getSortablePinRef() const;

   CPoint2d getOrigin() const;
   //PointStruct getOrigin() const                   { return pnt; }
   //CPoint2d getOrigin2d() const                    { return CPoint2d(pnt.x,pnt.y); }
   double getOriginX() const                       { return m_x; }
   double getOriginY() const                       { return m_y; }
   void setOrigin(const CPoint2d& origin);
   void setOrigin(const PointStruct& origin);
   void setOrigin(double x,double y)               { m_x = x;  m_y = y; }
   void setOrigin(DbUnit x,DbUnit y)               { m_x = x;  m_y = y; }
   void setOriginX(double x)                       { m_x = x;  }
   void setOriginY(double y)                       { m_y = y;  }
   void setOriginX(DbUnit x)                       { m_x = x;  }
   void setOriginY(DbUnit y)                       { m_y = y;  }
   void incOriginX(double x)                       { m_x += x;  }
   void incOriginY(double y)                       { m_y += y;  }

   double getRotationRadians() const               { return m_rotationRadians; }
   double getRotationDegrees() const;
   void setRotationRadians(DbUnit radians)         { m_rotationRadians =  radians; }
   void setRotationRadians(double radians)         { m_rotationRadians =  radians; }
   void incRotationRadians(double radians)         { m_rotationRadians += radians; }
   void setRotationDegrees(DbUnit degrees);
   void setRotationDegrees(double degrees);
   void incRotationDegrees(double degrees);

   int getPadstackBlockNumber() const              { return m_padstackBlockNumber; }
   void setPadstackBlockNumber(int blockNumber)    { m_padstackBlockNumber = blockNumber; }

   VisibleTag getVisible() const                   { return (VisibleTag)m_visible; }
   bool isVisible(bool topFlag) const              { return (( topFlag && ((m_visible & visibleTop   ) != 0) ) || 
                                                             (!topFlag && ((m_visible & visibleBottom) != 0) )    ); }
   bool isVisibleTop() const                       { return ((m_visible & visibleTop   ) != 0); }
   bool isVisibleBottom() const                    { return ((m_visible & visibleBottom) != 0); }
   bool isVisibleBoth() const                      { return  (m_visible == visibleBoth); }
   void setVisible(VisibleTag visible);
   void setVisible(unsigned int visible);
   void setVisibleBits(unsigned int mask);
   void clearVisibleBits(unsigned int mask);

   int getMirror() const                           { return m_mirror; }
   void setMirror(int mirror)                      { m_mirror = mirror; }

   bool getPinCoordinatesComplete() const          { return (m_pinCoordinatesComplete != 0); }
   void setPinCoordinatesComplete(int flag)        { m_pinCoordinatesComplete = flag; }
   void setPinCoordinatesComplete(bool flag)       { m_pinCoordinatesComplete = flag; }

   CAttributes* getAttributes() const              { return m_attributes; }
   CAttributes*& getAttributesRef()                { return m_attributes; }
   CAttributes*& getDefinedAttributes();
   CAttributes& attributes();
   bool lookUpAttrib(WORD keyword, CAttribute *&attribute) const;
   bool setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void* value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr);
   bool IsDiePin(CCamCadData &camCadData);

   // operations
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);

private:
   void addToMap();
   void removeFromMap() const;
};

//_____________________________________________________________________________
class CCompPinStructListContainer : public CTypedPtrListContainer<CompPinStruct*>
{
};

//_____________________________________________________________________________
class CCompPinList // : public CTypedPtrList<CPtrList, CompPinStruct*>
{
//friend void NetStruct::takeCompPins(NetStruct& otherNet);

private:
   CCompPinStructListContainer m_compPinList;

   NetStruct& m_parentNet;

public:
   CCompPinList(NetStruct& parentNet);
   ~CCompPinList();

   void empty();

   CCamCadData& getCamCadData() const;
   CNetList& getNetList() const;

   CompPinStruct* addCompPin(const CString& refDes,const CString& pinName,int entityNumber = -1);
   void addHead(CompPinStruct*& compPin);
   void addTail(CompPinStruct*& compPin);
   POSITION getHeadPosition() const;
   POSITION find(CompPinStruct*& compPin) const;
   CompPinStruct* getHead() const;
   CompPinStruct* getTail() const;
   CompPinStruct* getNext(POSITION& pos) const;
   CompPinStruct* getAt(POSITION pos) const;
   int getCount() const;
   bool isEmpty() const;
   void removeAt(POSITION pos);
   void deleteCompPin(CompPinStruct*& compPin);
   void deleteAt(POSITION pos);

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
	void Scale(double factor);

private:
   //void addCompPin(CompPinStruct* compPinStruct);
};

#endif
