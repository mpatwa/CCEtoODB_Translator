// $Header: /CAMCAD/DcaLib/DcaAttributes.h 9     6/15/07 7:39p Kurt Van Ness $

#if !defined(__DcaAttributes_h__)
#define __DcaAttributes_h__

#pragma once

#include "DcaPoint2d.h"
#include "Dca.h"
#include "DcaKeyword.h"
#include "DcaHorizontalPosition.h"
#include "DcaVerticalPosition.h"
#include "DcaFont.h"

//_____________________________________________________________________________
#define ImplementAttribId

class CCamCadData;
class AttribIterator;
class CBasesVector;
class CAttributeNode;
class CAttributeNodeList;
//class CNamedAttributeInstance;

//_____________________________________________________________________________
enum AttributeUpdateMethodTag
{
   attributeUpdateAppend    = 1,  // SA_APPEND       1        // attributes gets CAT on the end of the string
   attributeUpdateOverwrite = 2,  // SA_OVERWRITE    2        // old att gets deleted, new gets added 
   attributeUpdateFenced    = 3,  // SA_RETURN       3        // if old exist, new value is not accepted.
   attributeUpdateUndefined = 0
};

//_____________________________________________________________________________
enum EAttribCompare
{
	// - These are enums are treated as flags
	// - "attribCompareByAll" MUST be the last one and is he sum of all the other enums
	// - When adding additional enum, MUST change the integer of "attribCompareByAll"

	attribCompareByKeyword		= 0,
	attribCompareByValue			= 1,
	attribCompareByProperty		= 2,

	attribCompareByAll			= 3,	// Last one and is he sum of all the other enums
};

//_____________________________________________________________________________
enum AttributeMergeMethodTag
{
   attributeMergeMethodRemoveSourceKeepDestination,  // moves source to missing destination or discards source
   attributeMergeMethodMoveSourceToDestination,      // moves source to destination, discards original destination if present
   attributeMergeMethodCopyNewSourceToDestination,   // copies new source to destination, discards original source
};

//_____________________________________________________________________________
class CAttribute
{
   friend AttribIterator;

private:
   CCamCadData& m_camCadData;
   int m_keywordIndex;

#ifdef ImplementAttribId
   static int m_nextId;
   static int m_watchId;
   int m_id;
#endif

   ValueTypeTag m_valueType;

   union 
   {
      int      m_stringValueIndex;
      int      m_intValue;
      double   m_doubleValue;
   };

   bool           m_inherited;

   mutable CAttributeNodeList* m_attributeNodeList;

public:
   CAttribute(const CAttribute& other);
   CAttribute(CCamCadData& camCadData);
   CAttribute(CCamCadData& camCadData,ValueTypeTag valueType);

public:
   ~CAttribute();

private:
   void init();
   void transformAllInstances(const CTMatrix& transformationMatrix);

public:
   CAttribute& operator=(const CAttribute& other);
   CAttribute* allocateCopy();

   // accessors
   CCamCadData& getCamCadData() const;

   ValueTypeTag getValueType() const;
   void setValueType(ValueTypeTag type);
   void setValueType(int type);

   int getKeywordIndex() const;
   void setKeywordIndex(int keywordIndex);

   CString getName() const;
   CString getNameValue() const;

   const int getStringValueIndex() const;
   void setStringValueIndex(int index);

   const int getIntValue() const;
   void setIntValue(int value);

   const double getDoubleValue() const;
   void setDoubleValue(double value);

   CPoint2d getCoordinate() const;
   void setCoordinate(const CPoint2d& coordinate);
   void setCoordinate(double x,double y);
   void offsetCoordinate(double offsetX,double offsetY);

   double getX() const;
   void setX(double x);
   double getY() const;
   void setY(double y);
   CBasesVector getBasesVector() const;

   const double getRotationRadians() const;
   const double getRotationDegrees() const;
   void setRotationRadians(double radians);
   void setRotationDegrees(double degrees);
   void incRotationRadians(double radians);
   void incRotationDegrees(double degrees);

   double getHeight() const;
   void setHeight(double height);

   double getWidth() const;
   void setWidth(double width);

   short getPenWidthIndex() const;
   void setPenWidthIndex(int index);

   short getLayerIndex() const;
   void setLayerIndex(int index);

   bool isVisible() const;
   void setVisible(bool visibilityFlag);
   void setVisible(BOOL visibilityFlag);
   void setVisibleAllInstances(bool visibilityFlag);

   bool isProportionallySpaced() const;
   void setProportionalSpacing(bool flag);
   void setProportionalSpacing(BOOL flag);

   bool getMirrorDisabled() const;
   void setMirrorDisabled(bool flag);
   void setMirrorDisabled(BOOL flag);

   CFontProperties *GetFontProperties() const;
   CFontProperties *GetDefinedFontProperties();

   bool getMirrorFlip() const;
   void setMirrorFlip(bool flag);
   void setMirrorFlip(BOOL flag);

   bool isInherited() const;
   void setInherited(bool flag);
   void setInherited(BOOL flag);

   HorizontalPositionTag getHorizontalPosition() const;
   void setHorizontalPosition(HorizontalPositionTag horizontalPosition );

   VerticalPositionTag getVerticalPosition()     const;
   void setVerticalPosition(VerticalPositionTag verticalPosition );

   DbFlag getFlags() const;
   void setFlags(DbFlag attribFlag);
   void setFlagBits(DbFlag mask);
   void clearFlagBits(DbFlag mask);

   // operations

	int getCount() const;

   void setValue(const CAttribute& other);
   void setValueFromString(CCamCadData& camCadData,WORD keyword,const CString& value); // deprecated
   void setValueFromString(const CString& value);
   void setProperties(const CAttribute& other);
   void setProperties(double x,double y,double rotation,double height,double width,
      short penWidthIndex,BOOL proportional,BOOL visible,BOOL never_mirror,BOOL mirror_flip,BOOL inherited,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition,
      short layer,DbFlag flg);
   bool propertiesEqual(const CAttribute& other) const;
   bool valueEqual(const CAttribute& other) const;
   bool propertiesAndValueEqual(const CAttribute& other) const;

   CString getStringValue() const;

   void addInstance(CAttribute* attribute);
   void duplicateInstance();
   //CAttribute* removeInstance(CAttribute* attribute);
   bool findInstanceLike(const CAttribute& attribute);
   bool deleteInstanceLike(const CAttribute& attribute);
   bool duplicateInstanceLike(const CAttribute& attribute);
   
	bool updateInstanceLike(const CAttribute& attribute,const CAttribute& updatedAttribute);
   bool updatePropertiesOfInstanceLike(const CAttribute& attribute,const CAttribute& updatedAttribute);
	bool updateLayerOfInstanceLike(const CAttribute& attribute, const int layerIndex);
   bool updateValueOfInstanceLike(const CAttribute& attribute,const CAttribute& updatedAttribute);

   //bool getInstanceAt(int index,CAttribute& attribute);
   //bool deleteInstance(CAttribute* attribute);
   //bool deleteInstance(CAttributes& map);

   void ConvertType(CCamCadData& camCadData, WORD toKeyword); // convert attrib to match type of toKeyword

   void transform(const CTMatrix& transformationMatrix);

// utility
   bool isValid() const;
   void assertValid() const;
   bool assertValidCoordinate();

// nodes
   int getNodeCount() const;
   bool hasNodes() const;
   POSITION getHeadNodePosition() const;
   CAttributeNode* getNextNode(POSITION& pos) const;
   CAttributeNodeList& getDefinedAttributeNodeList() const;
   void deleteNodes();

#ifdef ImplementAttribId
   static int getNextId();

   static int getWatchId();
   static void setWatchId(int id);

   int getId() const;
#endif

   void dump(CWriteFormat& writeFormat,int depth = -1,int keywordIndex = -1,CCamCadData* camCadData = NULL);

private:
   //CAttribute* getNext() const;
   //CAttribute* getTail();

   CAttributeNode& getDefinedFirstNode() const;
   void releaseNodes();
};

typedef CAttribute Attrib;

//_____________________________________________________________________________
class CAttributeNode
{
private:
   // the attribute is an offset to the mirror and rotation of its parent.
   CPoint2d       m_origin;

   double         m_rotationRadians;
   double         m_height;
   double         m_width;

   int            m_penWidthIndex;

   CFontProperties *m_fontProps;

   unsigned char  m_proportional:1;
   unsigned char  m_visible:1;
   unsigned char  m_mirrorDisabledFlag:1;   // this attribute is always unmirrored, regardsless of inserts etc...
   unsigned char  m_mirrorFlipFlag:1;  // Mirror graphics flag, no Mirror layer support since layer is directly assigned

   //unsigned char  textAlignment:2; // 0=left, 1=center, 2=right
   //unsigned char  lineAlignment:2; // 0=bottom, 1=center, 2=top
   unsigned char m_horizontalPosition:2; // horizontal position relative to insert point
   unsigned char m_verticalPosition:2;   // vertical position relative to insert point

   short          m_layerIndex;
   DbFlag         m_flags;        // see dbutil.h ATTRIB_xxx

public:
   /*
   CAttributeNode(double x=0.,double y=0.,double rotationRadians=0.,double height=0.,double width=0.,int penWidthIndex= -1,int layerIndex= -1,
                  bool visible=true,bool proportionalSpacing=false,bool mirrorDisabled=false, /*mirrorFlip=false*/
            /*      DbFlag flags=0,
                  HorizontalPositionTag horizontalPosition = horizontalPositionLeft,
                  VerticalPositionTag verticalPosition = verticalPositionBaseline);
                  */

   CAttributeNode(double x=0.,double y=0.,double rotationRadians=0.,double height=0.,double width=0.,int penWidthIndex= -1,int layerIndex= -1,
                  bool visible=true,bool proportionalSpacing=false,bool mirrorDisabled=false, bool mirrorFlip=false,
                  DbFlag flags=0,
                  HorizontalPositionTag horizontalPosition = horizontalPositionLeft,
                  VerticalPositionTag verticalPosition = verticalPositionBaseline);

   CAttributeNode(const CAttributeNode& other);

   CAttributeNode& operator=(const CAttributeNode& other);

   CPoint2d getOrigin() const;
   void setOrigin(const CPoint2d& origin);
   void setOrigin(double x,double y);
   void offsetOrigin(double offsetX,double offsetY);

   CFontProperties *GetFontProperties() const    { return m_fontProps; }
   CFontProperties *GetDefinedFontProperties()   { if (m_fontProps == NULL) m_fontProps = new CFontProperties; return m_fontProps; }

   double getX() const;
   void setX(double x);
   double getY() const;
   void setY(double y);
   CBasesVector getBasesVector() const;

   const double getRotationRadians() const;
   const double getRotationDegrees() const;
   void setRotationRadians(double radians);
   void setRotationDegrees(double degrees);
   void incRotationRadians(double radians);
   void incRotationDegrees(double degrees);

   double getHeight() const;
   void setHeight(double height);

   double getWidth() const;
   void setWidth(double width);

   int getPenWidthIndex() const;
   void setPenWidthIndex(int index);

   short getLayerIndex() const;
   void setLayerIndex(int index);

   bool isVisible() const;
   void setVisible(bool visibilityFlag);

   bool isProportionallySpaced() const;
   void setProportionalSpacing(bool flag);

   bool getMirrorDisabled() const;
   void setMirrorDisabled(bool flag);

   bool getMirrorFlip() const;
   void setMirrorFlip(bool flag);

   HorizontalPositionTag getHorizontalPosition() const;
   void setHorizontalPosition(HorizontalPositionTag horizontalPosition );

   VerticalPositionTag getVerticalPosition()     const;
   void setVerticalPosition(VerticalPositionTag verticalPosition );

   DbFlag getFlags() const;
   void setFlags(DbFlag attribFlag);
   void setFlagBits(DbFlag mask);
   void clearFlagBits(DbFlag mask);

   bool propertiesEqual(const CAttributeNode& other) const;

	void transform(const CTMatrix& transformationMatrix);

   // utility
   //bool isValid() const;
   //void assertValid() const;
   bool assertValidCoordinate();

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class CAttributeNodeList
{
private:
   // NULL values are not allowed in m_attributeNodeList
   CTypedPtrListContainer<CAttributeNode*> m_attributeNodeList;

public:
   CAttributeNodeList();
   ~CAttributeNodeList();

   CAttributeNodeList& operator=(const CAttributeNodeList& other);

   void empty();

   int getCount() const;
   CAttributeNode* getHead() const;
   POSITION getHeadPosition() const;
   CAttributeNode* getNext(POSITION& pos);

   CAttributeNode* addNewNode(double x=0.,double y=0.,double rotationRadians=0.,double height=0.,double width=0.,int penWidthIndex= -1,int layerIndex= -1,
                              bool visible=false,bool proportionalSpacing=false,bool mirrorDisabled=false,bool mirrorFlip=false,
                              DbFlag flags=0,
                              HorizontalPositionTag horizontalPosition = horizontalPositionLeft,
                              VerticalPositionTag verticalPosition = verticalPositionBaseline);
   CAttributeNode* addNewNode(const CAttributeNode& attributeNodeToCopy);
   void setVisibleAllInstances(bool visibilityFlag);

// operations
   bool findInstanceLike(const CAttributeNode& attributeNode);
   bool deleteInstanceLike(const CAttributeNode& attributeNode);
   bool duplicateInstanceLike(const CAttributeNode& attributeNode);
   
	bool updateInstanceLike(const CAttributeNode& attributeNode,const CAttributeNode& updatedAttributeNode);
	bool updateLayerOfInstanceLike(const CAttributeNode& attributeNode, const int layerIndex);

	void transform(const CTMatrix& transformationMatrix);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class AttribIterator
{
private:
   const CAttribute* m_attrib ;
   const CAttribute* m_next;
   POSITION m_pos;

public:
   AttribIterator(const CAttribute* attrib);

   bool getNext(CAttribute& attrib);
};

//_____________________________________________________________________________
class CAttributes
{
private:
   CTypedMapWordToPtrContainer<CAttribute*> m_map;

public:
   CAttributes();
   CAttributes(const CAttributes& other);
   ~CAttributes();

public:
	CAttributes& operator=(const CAttributes& other);

   void empty();

   POSITION GetStartPosition() const;
   void GetNextAssoc(POSITION& nextPosition,WORD& key,CAttribute*& value) const;
   bool Lookup(WORD key,CAttribute*& value) const;
   CAttribute* lookupAttribute(int keywordIndex) const;
   CAttribute* lookupAttribute(StandardAttributeTag attributeTag) const;
   void SetAt(WORD key,CAttribute* newValue);
   CAttribute* getFirst() const;

   bool deleteAttribute(int keywordIndex);
   bool deleteAttribute(StandardAttributeTag attributeTag);
   bool changeAttributeKey(int oldKeywordIndex,int newKeywordIndex);
   CAttribute* removeAttribute(int key);

	CAttributes& CopyAll(const CAttributes& other);
	CAttributes& CopyNew(const CAttributes& other);

   INT_PTR GetSize() const;
   INT_PTR GetCount() const;
   bool IsEmpty() const;

   UINT GetHashTableSize() const;
   void InitHashTable(UINT hashSize,bool bAllocNow = true);

   bool getAttribute(int keywordIndex) const;
   bool getAttribute(int keywordIndex,int& value) const;
   bool getAttribute(int keywordIndex,double& value) const;
   bool getAttribute(int keywordIndex,CString& value) const;

   bool getAttribute(StandardAttributeTag attributeTag) const;
   bool getAttribute(StandardAttributeTag attributeTag,int& value) const;
   bool getAttribute(StandardAttributeTag attributeTag,double& value) const;
   bool getAttribute(StandardAttributeTag attributeTag,CString& value) const;

   bool getAttribute(const CCamCadData& camCadData,int keywordIndex) const;  // deprecated
   bool getAttribute(const CCamCadData& camCadData,int keywordIndex,int& value) const;  // deprecated
   bool getAttribute(const CCamCadData& camCadData,int keywordIndex,double& value) const;  // deprecated
   bool getAttribute(const CCamCadData& camCadData,int keywordIndex,CString& value) const;  // deprecated

   bool getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag) const;  // deprecated
   bool getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag,int& value) const;  // deprecated
   bool getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag,double& value) const;  // deprecated
   bool getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag,CString& value) const;  // deprecated

   CAttribute* setAttribute(CCamCadData& camCadData,int keywordIndex);
   CAttribute* setAttribute(CCamCadData& camCadData,int keywordIndex,int value           ,AttributeUpdateMethodTag attributeUpdateMethod=attributeUpdateOverwrite);
   CAttribute* setAttribute(CCamCadData& camCadData,int keywordIndex,double value        ,AttributeUpdateMethodTag attributeUpdateMethod=attributeUpdateOverwrite);
   CAttribute* setAttribute(CCamCadData& camCadData,int keywordIndex,const CString& value,AttributeUpdateMethodTag attributeUpdateMethod=attributeUpdateOverwrite);

   CAttribute* setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag);
   CAttribute* setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag,int value           ,AttributeUpdateMethodTag attributeUpdateMethod=attributeUpdateOverwrite);
   CAttribute* setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag,double value        ,AttributeUpdateMethodTag attributeUpdateMethod=attributeUpdateOverwrite);
   CAttribute* setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag,const CString& value,AttributeUpdateMethodTag attributeUpdateMethod=attributeUpdateOverwrite);

   void merge(CCamCadData& camCadData,int sourceKeyIndex,int destinationKeyIndex,AttributeMergeMethodTag mergeMethod);
	void transform(const CTMatrix& transformationMatrix);
	bool IsEqual(const CAttributes& other, EAttribCompare attribCompareBy); 
	void WriteXML(CWriteFormat& writeFormat,CCamCadData& camCadData);

   CString getNameValues() const;
};

//_____________________________________________________________________________
class CNamedAttributeInstance
{
private:
   WORD m_key;
   CString m_name;
   CAttribute m_attribute;
   int m_instanceNumber;

public:
   CNamedAttributeInstance(WORD key,const CString& name,const CAttribute& attrib,int instanceNumber);

   WORD getKey() const;
   CString getName() const;
   const CAttribute& getAttribute() const;
   int getInstanceNumber() const;
};

//_____________________________________________________________________________
class CAttributeMapWrap
{
private:
   CAttributes*& m_map;

public:
   CAttributeMapWrap(CAttributes*& map);
   ~CAttributeMapWrap();
   void empty();

   // Attributes
   INT_PTR GetCount() const;
   bool IsEmpty() const;
   CAttributes& getMap();
   CAttributes*& getMapPtr() { return m_map; }

   // Operations
   bool Lookup(WORD key,CAttribute*& attribute) const;
   void SetAt(WORD key,CAttribute* newValue);
   void copyFrom(CAttributes* map);

   CAttribute* RemoveKey(WORD key);
   bool deleteKey(WORD key);
   void RemoveAll();

   POSITION GetStartPosition() const;
   void GetNextAssoc(POSITION& rNextPosition,WORD& rKey,CAttribute*& attribute) const;
   //void GetNextAssoc(POSITION& rNextPosition,WORD& rKey,void*& rValue) const
   //  { GetNextAssoc(rNextPosition,rKey,(CAttribute*&)rValue); }

   // extended operations
   CAttribute* removeInstance(CAttribute* attribute);
   bool deleteInstanceLike(const CNamedAttributeInstance& attributeInstance);
   bool duplicateInstanceLike(const CNamedAttributeInstance& attributeInstance);
   bool updateInstanceLike(const CNamedAttributeInstance& attributeInstance,const CAttribute& updatedAttribute);
   void addInstance(WORD key,CAttribute* attribute);
   bool addInstance(CCamCadData& camCadData,int keyword,
         ValueTypeTag valueType,void *value,
         double x,double y,double rotation,double height,double width,bool proportional,int penWidthIndex,
         bool visible,DbFlag flag,short layer,bool neverMirror,bool mirrorFlip,
         HorizontalPositionTag horizontalPosition, VerticalPositionTag verticalPosition);

   void dump(CWriteFormat& writeFormat,int depth = -1);
};

//_____________________________________________________________________________
class CSortedAttributeListEntry
{
private:
   int m_keywordIndex;
   CTypedPtrListContainer<CAttribute*> m_attributeList;

public:
   CSortedAttributeListEntry(int keywordIndex,bool isContainer);
   void add(CAttribute& attribute);

   int getKeywordIndex() const;

   POSITION getHeadPosition() const;
   CAttribute* getNext(POSITION& pos) const;
   int getCount() const;
};

//_____________________________________________________________________________
class CSortedAttributeList
{
private:
   CTypedPtrListContainer<CSortedAttributeListEntry*> m_attributeList;

public:
   CSortedAttributeList(const CAttributes& attributes);

   POSITION getHeadPosition() const;
   CSortedAttributeListEntry* getNext(POSITION& pos) const;
   int getCount() const;

   void add(int keywordIndex,CAttribute& attribute);

private:
   CSortedAttributeListEntry& getDefinedAttributeListEntry(int keywordIndex);

};

#endif
