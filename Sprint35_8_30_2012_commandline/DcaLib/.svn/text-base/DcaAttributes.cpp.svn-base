// $Header: /CAMCAD/DcaLib/DcaAttributes.cpp 11    6/17/07 9:01p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"
#include "DcaBasesVector.h"
#include "DcaWriteFormat.h"
#include "DcaTMatrix.h"
#include "DcaLib.h"


//_____________________________________________________________________________

// Supports european convention of comma as decimal point as well as period
static double strToDouble(const char *c)
{
   CString  cc;
   cc = c;
   cc.Replace(",",".");
   return atof(cc);
}

//_____________________________________________________________________________
//#define AttribDebugMode
#ifdef ImplementAttribId
int CAttribute::m_nextId  =  0;
int CAttribute::m_watchId = -1;
#endif


CAttribute::CAttribute(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_valueType(valueTypeInteger)
, m_keywordIndex(-1)
, m_attributeNodeList(NULL)
{
#ifdef ImplementAttribId
   m_id = m_nextId++;
#endif

   init();
}

CAttribute::CAttribute(CCamCadData& camCadData,ValueTypeTag valueType)
: m_camCadData(camCadData)
, m_valueType(valueType)
, m_keywordIndex(-1)
, m_attributeNodeList(NULL)
{
#ifdef ImplementAttribId
   m_id = m_nextId++;
#endif

   init();
}

void CAttribute::init()
{
   setIntValue(0);
   //m_next    = NULL;

   //setCoordinate(0.,0.);

   //m_rotationRadians    = 0.;
   //m_height             = 0.;
   //m_width              = 0.;

   //m_penWidthIndex      = 0;
   //m_proportional       = 0;
   //m_visible            = 0;
   //m_mirrorDisabledFlag = 0;
   m_inherited          = false;
   //m_layerIndex         = 0;
   //m_flags              = 0;

   //m_horizontalPosition = horizontalPositionLeft;
   //m_verticalPosition   = verticalPositionBaseline;
}

CAttribute::CAttribute(const CAttribute& other)
: m_camCadData(other.getCamCadData())
, m_valueType(valueTypeInteger)
, m_keywordIndex(-1)
, m_attributeNodeList(NULL)
{
#ifdef ImplementAttribId
   m_id = m_nextId++;
#endif

#ifdef ImplementAttribId
   if (other.m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   *this  = other;
}

CAttribute::~CAttribute()
{
#ifdef AttribDebugMode
   assertValid();
#endif

#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   //CAttribute* next = m_next;
   //m_next = NULL;
   //delete next;

   delete m_attributeNodeList;
}

CAttribute& CAttribute::operator=(const CAttribute& other)
{
   // assumes "this" and "other" belong to the same CCamCadDatabase object
   //if (&m_camCadData != &(other.m_camCadData))
   //{
   //   do something special here
   //}

#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

   if (&other != this)
   {
      m_keywordIndex = other.m_keywordIndex;
      m_valueType    = other.m_valueType;
      m_inherited    = other.m_inherited;
      //m_flags        = other.m_flags;

      memcpy(&m_doubleValue,&(other.m_doubleValue),sizeof(double));

      if (other.hasNodes())
      {
         getDefinedAttributeNodeList() = other.getDefinedAttributeNodeList();
      }
      else
      {
         releaseNodes();
      }
   }

   return *this;
}

#ifdef ImplementAttribId
int CAttribute::getNextId()
{
   return m_nextId;
}

int CAttribute::getWatchId()
{
   return m_watchId;
}

void CAttribute::setWatchId(int id)
{
   m_watchId = id;
}

int CAttribute::getId() const
{
   return m_id;
}
#endif

int CAttribute::getNodeCount() const
{
   int nodeCount = 0;

   if (m_attributeNodeList != NULL)
   {
      nodeCount = m_attributeNodeList->getCount();
   }

   return nodeCount;
}

bool CAttribute::hasNodes() const
{
   return (getNodeCount() != 0);
}

POSITION CAttribute::getHeadNodePosition() const
{
   POSITION headPos = NULL;

   if (m_attributeNodeList != NULL)
   {
      headPos = m_attributeNodeList->getHeadPosition();
   }

   return headPos;
}

CAttributeNode* CAttribute::getNextNode(POSITION& pos) const
{
   return m_attributeNodeList->getNext(pos);
}

CAttributeNodeList& CAttribute::getDefinedAttributeNodeList() const
{
   if (m_attributeNodeList == NULL)
   {
      m_attributeNodeList = new CAttributeNodeList();
   }

   return *m_attributeNodeList;
}

CAttributeNode& CAttribute::getDefinedFirstNode() const
{
   CAttributeNode* attributeNode = NULL;

   if (getDefinedAttributeNodeList().getCount() < 1)
   {
      attributeNode = getDefinedAttributeNodeList().addNewNode();
   }
   else
   {
      attributeNode = getDefinedAttributeNodeList().getHead();
   }

   return *attributeNode;
}

void CAttribute::deleteNodes()
{
   if (m_attributeNodeList != NULL)
   {
      m_attributeNodeList->empty();
   }
}

void CAttribute::releaseNodes()
{
   delete m_attributeNodeList;

   m_attributeNodeList = NULL;
}

double CAttribute::getHeight() const
{
   double height = 0.;

   if (hasNodes())
   {
      height = getDefinedFirstNode().getHeight();
   }

   return height;
}

void CAttribute::setHeight(double height)
{
   getDefinedFirstNode().setHeight(height);
   //m_height = height;
}

double CAttribute::getWidth() const
{
   double width = 0.;

   if (hasNodes())
   {
      width = getDefinedFirstNode().getWidth();
   }

   return width;
}

void CAttribute::setWidth(double width)
{
   getDefinedFirstNode().setWidth(width);
   //m_width = width;
}

short CAttribute::getPenWidthIndex() const
{
   int penWidthIndex = 0;

   if (hasNodes())
   {
      penWidthIndex = getDefinedFirstNode().getPenWidthIndex();
   }

   if( penWidthIndex < 0 )
   {
      penWidthIndex = 0;
   }
   return penWidthIndex;
}

void CAttribute::setPenWidthIndex(int index)
{
   getDefinedFirstNode().setPenWidthIndex(index);
}

bool CAttribute::isVisible() const
{
   bool visibleFlag = false;

   if (hasNodes())
   {
      visibleFlag = getDefinedFirstNode().isVisible();
   }

   return visibleFlag;
}

void CAttribute::setVisible(bool visibilityFlag)
{
   getDefinedFirstNode().setVisible(visibilityFlag);
   //m_visible = visibilityFlag;
}

void CAttribute::setVisible(BOOL visibilityFlag)
{
   getDefinedFirstNode().setVisible(visibilityFlag != 0);
   //m_visible = visibilityFlag;
}

bool CAttribute::isProportionallySpaced() const
{
   bool proportionallySpacedFlag = false;

   if (hasNodes())
   {
      proportionallySpacedFlag = getDefinedFirstNode().isProportionallySpaced();
   }

   return proportionallySpacedFlag;
}

void CAttribute::setProportionalSpacing(bool flag)
{
   getDefinedFirstNode().setProportionalSpacing(flag);
   //m_proportional = flag;
}

void CAttribute::setProportionalSpacing(BOOL flag)
{
   getDefinedFirstNode().setProportionalSpacing(flag != 0);
   //m_proportional = (flag != 0);
}


CFontProperties * CAttribute::GetFontProperties() const
{
   CFontProperties *fontProps = NULL;

   if (hasNodes())
   {
      fontProps = getDefinedFirstNode().GetFontProperties();
   }

   return fontProps;
}

CFontProperties * CAttribute::GetDefinedFontProperties()
{
   CFontProperties *fontProps = NULL;

   if (hasNodes())
   {
      fontProps = getDefinedFirstNode().GetDefinedFontProperties();
   }

   return fontProps;
}


bool CAttribute::getMirrorDisabled() const
{
   bool mirrorDisabledFlag = false;

   if (hasNodes())
   {
      mirrorDisabledFlag = getDefinedFirstNode().getMirrorDisabled();
   }

   return mirrorDisabledFlag;
}

void CAttribute::setMirrorDisabled(bool flag)
{
   getDefinedFirstNode().setMirrorDisabled(flag);
   //m_mirrorDisabledFlag = flag;
}

void CAttribute::setMirrorDisabled(BOOL flag)
{
   getDefinedFirstNode().setMirrorDisabled(flag != 0);
   //m_mirrorDisabledFlag = (flag != 0);
}

bool CAttribute::getMirrorFlip() const
{
   bool mirrorFlipFlag = false;

   if (hasNodes())
   {
      mirrorFlipFlag = getDefinedFirstNode().getMirrorFlip();
   }

   return mirrorFlipFlag;
}

void CAttribute::setMirrorFlip(bool flag)
{
   getDefinedFirstNode().setMirrorFlip(flag);
}

void CAttribute::setMirrorFlip(BOOL flag)
{
   getDefinedFirstNode().setMirrorFlip(flag != 0);
}

bool CAttribute::isInherited() const
{
   return (m_inherited != 0);
}

void CAttribute::setInherited(bool flag)
{
   m_inherited = flag;
}

void CAttribute::setInherited(BOOL flag)
{
   m_inherited = (flag != 0);
}

HorizontalPositionTag CAttribute::getHorizontalPosition() const
{
   HorizontalPositionTag horizontalPosition = horizontalPositionLeft;

   if (hasNodes())
   {
      horizontalPosition = getDefinedFirstNode().getHorizontalPosition();
   }

   return horizontalPosition;
}

void CAttribute::setHorizontalPosition(HorizontalPositionTag horizontalPosition )
{
   getDefinedFirstNode().setHorizontalPosition(horizontalPosition);
   //m_horizontalPosition = horizontalPosition;
}

VerticalPositionTag CAttribute::getVerticalPosition() const
{
   VerticalPositionTag verticalPosition = verticalPositionBaseline;

   if (hasNodes())
   {
      verticalPosition = getDefinedFirstNode().getVerticalPosition();
   }

   return verticalPosition;
}

void CAttribute::setVerticalPosition(VerticalPositionTag verticalPosition )
{
   getDefinedFirstNode().setVerticalPosition(verticalPosition);
   //m_verticalPosition = verticalPosition;
}

double CAttribute::getX() const
{
   double x = 0.;

   if (hasNodes())
   {
      x = getDefinedFirstNode().getX();
   }

   return x;
}

void CAttribute::setX(double x)
{
   getDefinedFirstNode().setX(x);
   //m_coordinate.x = x;
   //assertValidCoordinate();
}

double CAttribute::getY() const
{
   double y = 0.;

   if (hasNodes())
   {
      y = getDefinedFirstNode().getY();
   }

   return y;
}

void CAttribute::setY(double y)
{
   getDefinedFirstNode().setY(y);
   //m_coordinate.y = y;
   //assertValidCoordinate();
}

CBasesVector CAttribute::getBasesVector() const
{
   CBasesVector basesVector;

   if (hasNodes())
   {
      basesVector = getDefinedFirstNode().getBasesVector();
   }

   return basesVector;
}

const double CAttribute::getRotationRadians() const
{
   double radians = 0.;

   if (hasNodes())
   {
      radians = getDefinedFirstNode().getRotationRadians();
   }

   return radians;
}

const double CAttribute::getRotationDegrees() const
{
   double degrees = 0.;

   if (hasNodes())
   {
      degrees = getDefinedFirstNode().getRotationDegrees();
   }

   return degrees;
}

void CAttribute::setRotationRadians(double radians)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   getDefinedFirstNode().setRotationRadians(radians);
   //m_rotationRadians  = radians;
}

void CAttribute::setRotationDegrees(double degrees)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   getDefinedFirstNode().setRotationDegrees(degrees);
   //m_rotationRadians  = degreesToRadians(degrees);
}

void CAttribute::incRotationRadians(double radians)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   getDefinedFirstNode().incRotationRadians(radians);
   //m_rotationRadians += radians;
}

void CAttribute::incRotationDegrees(double degrees)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   getDefinedFirstNode().incRotationDegrees(degrees);
   //m_rotationRadians += degreesToRadians(degrees);
}

void CAttribute::setVisibleAllInstances(bool visibilityFlag)
{
   if (hasNodes())
   {
      this->getDefinedAttributeNodeList().setVisibleAllInstances(visibilityFlag);
   }

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   attrib->m_visible = visibilityFlag;
   //}
}

short CAttribute::getLayerIndex() const
{
   int layerIndex = 0;

   if (hasNodes())
   {
      layerIndex = getDefinedFirstNode().getLayerIndex();
   }

   return layerIndex;
}

void CAttribute::setLayerIndex(int index)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   getDefinedFirstNode().setLayerIndex(index);
   //m_layerIndex = index;
}

CPoint2d CAttribute::getCoordinate() const
{
   CPoint2d coordinate;

   if (hasNodes())
   {
      coordinate = getDefinedFirstNode().getOrigin();
   }

   return coordinate;
}

void CAttribute::setCoordinate(double x,double y)
{
   getDefinedFirstNode().setOrigin(x,y);
   //m_coordinate.x = x;
   //m_coordinate.y = y;
   //assertValidCoordinate();
}

void CAttribute::setCoordinate(const CPoint2d& coordinate)
{
   getDefinedFirstNode().setOrigin(coordinate);
   //m_coordinate = coordinate;
   //assertValidCoordinate();
}

void CAttribute::offsetCoordinate(double offsetX,double offsetY)
{
   getDefinedFirstNode().offsetOrigin(offsetX,offsetY);
   //m_coordinate.x += offsetX;
   //m_coordinate.y += offsetY;
   //assertValidCoordinate();
}

bool CAttribute::assertValidCoordinate()
{
   bool retval = true;

   if (hasNodes())
   {
      retval = getDefinedFirstNode().assertValidCoordinate();
   }

   return retval;
}

DbFlag CAttribute::getFlags() const
{
   DbFlag retval = 0;

   if (hasNodes())
   {
      retval = getDefinedFirstNode().getFlags();
   }

   return retval;
}

void CAttribute::setFlags(DbFlag attribFlag)
{
   getDefinedFirstNode().setFlags(attribFlag);
}

void CAttribute::setFlagBits(DbFlag mask)
{
   getDefinedFirstNode().setFlagBits(mask);
}

void CAttribute::clearFlagBits(DbFlag mask)
{
   getDefinedFirstNode().clearFlagBits(mask);
}

CAttribute* CAttribute::allocateCopy()
{
#ifdef AttribDebugMode
   assertValid();
#endif

   CAttribute* copy = new CAttribute(*this);
   //CAttribute* instanceCopy = copy;

   //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
   //{
   //   instanceCopy->m_next = new CAttribute(*attrib);
   //   instanceCopy = instanceCopy->m_next;
   //}

   return copy;
}

CCamCadData& CAttribute::getCamCadData() const
{
   return m_camCadData;
}

ValueTypeTag CAttribute::getValueType() const
{
   return m_valueType;
}

void CAttribute::setValueType(ValueTypeTag valueType)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   m_valueType = valueType;
}

void CAttribute::setValueType(int valueType)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   m_valueType = ((valueType >= ValueTypeTagMin && valueType <= ValueTypeTagMax) ? (ValueTypeTag) valueType : valueTypeUndefined);
}

int CAttribute::getKeywordIndex() const
{
   return m_keywordIndex;
}

void CAttribute::setKeywordIndex(int keywordIndex)
{
   if (keywordIndex != m_keywordIndex)
   {
      KeyWordStruct* keyword = getCamCadData().getKeyWordArray().getAt(keywordIndex);

      if (keyword != NULL)
      {
         ValueTypeTag valueType = keyword->getValueType();

         if (m_keywordIndex >= 0)
         {
            if (valueType != m_valueType)
            {
               CString stringValue = getStringValue();

               setValueType(valueType);

               setValueFromString(stringValue);
            }
         }
         else
         {
            setValueType(valueType);
         }

         m_keywordIndex = keywordIndex;
      }
   }
}

CString CAttribute::getName() const
{
   CString attributeName;

   KeyWordStruct* keyword = getCamCadData().getKeyWordArray().getAt(m_keywordIndex);

   if (keyword != NULL)
   {
      attributeName = keyword->getCCKeyword();
   }

   return attributeName;
}

CString CAttribute::getNameValue() const
{
   return getName() + ":" + getStringValue();
}

const int CAttribute::getStringValueIndex() const
{
   return m_stringValueIndex;
}

void CAttribute::setStringValueIndex(int index)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   m_stringValueIndex = index;
}

CString CAttribute::getStringValue() const
{
   CString stringValue;

   switch (getValueType())
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         int stringIndex = getStringValueIndex();

         stringValue = getCamCadData().getAttributeValueDictionaryStringValue(stringIndex);
      }

      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      stringValue.Format("%f",getDoubleValue());
      break;
   case valueTypeInteger:
      stringValue.Format("%d",getIntValue());
      break;
   }

   return stringValue;
}

const int CAttribute::getIntValue() const
{
   return m_intValue;
}

void CAttribute::setIntValue(int value)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   m_intValue = value;
}

const double CAttribute::getDoubleValue() const
{
   return m_doubleValue;
}

void CAttribute::setDoubleValue(double value)
{
#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   m_doubleValue = value;
}

int CAttribute::getCount() const
{
#ifdef AttribDebugMode
   assertValid();
#endif

   int count = 0;

   if (hasNodes())
   {
      count = getDefinedAttributeNodeList().getCount();
   }

   //for (CAttribute const* attrib = this;attrib != NULL;attrib = attrib->getNext())
   //{
   //   count++;
   //}

   return count;
}

//CAttribute* CAttribute::getNext() const
//{
//   return m_next;
//}

//CAttribute* CAttribute::getTail()
//{
//#ifdef AttribDebugMode
//   assertValid();
//#endif
//
//   CAttribute* tailAttribute = this;
//
//   while (tailAttribute->getNext() != NULL)
//   {
//      tailAttribute = tailAttribute->getNext();
//   }
//
//   return tailAttribute;
//}

void CAttribute::setValue(const CAttribute& other)
{
#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

   m_valueType = other.m_valueType;

   switch (other.m_valueType)
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      setStringValueIndex(other.getStringValueIndex());
      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      setDoubleValue(other.getDoubleValue());
      break;
   case valueTypeInteger:
      setIntValue( other.getIntValue());
      break;
   }
}

void CAttribute::setValueFromString(CCamCadData& camCadData,WORD keywordIndex,const CString& value)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   const KeyWordStruct* keywordStruct = camCadData.getAttributeKeyword(keywordIndex);

   if (keywordStruct != NULL)
   {
      switch (keywordStruct->getValueType())
      {
      case valueTypeString:
      case valueTypeEmailAddress:
      case valueTypeWebAddress:
         {
            setStringValueIndex(camCadData.getDefinedAttributeStringValueIndex(value));

            //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
            //{
            //   attrib->setStringValueIndex(getStringValueIndex());
            //}
         }

         break;
      case valueTypeDouble:
      case valueTypeUnitDouble:
         {
            setDoubleValue( strToDouble(value));

            //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
            //{
            //   attrib->setDoubleValue(getDoubleValue());
            //}
         }

         break;
      case valueTypeInteger:
         {
            setIntValue( atoi(value));

            //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
            //{
            //   attrib->setIntValue(getIntValue());
            //}
         }

         break;
      }
   }
}

void CAttribute::setValueFromString(const CString& value)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   switch (getValueType())
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         setStringValueIndex(getCamCadData().getDefinedAttributeStringValueIndex(value));

         //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
         //{
         //   attrib->setStringValueIndex(getStringValueIndex());
         //}
      }

      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      {
         setDoubleValue(strToDouble(value));

         //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
         //{
         //   attrib->setDoubleValue(getDoubleValue());
         //}
      }

      break;
   case valueTypeInteger:
      {
         setIntValue(atoi(value));

         //for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->m_next)
         //{
         //   attrib->setIntValue(getIntValue());
         //}
      }

      break;
   }
}

void CAttribute::setProperties(const CAttribute& other)
{
#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }

   if (other.m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   if (other.hasNodes())
   {
      getDefinedAttributeNodeList() = other.getDefinedAttributeNodeList();
   }
   else
   {
      releaseNodes();
   }

   m_inherited          = other.m_inherited;

}

void CAttribute::setProperties(double x,double y,double rotationRadians,double height,double width,
      short penWidthIndex,BOOL proportional,BOOL visible,BOOL never_mirror,BOOL mirror_flip,BOOL inherited,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition,
      short layer,DbFlag flg)
{
#ifdef AttribDebugMode
   assertValid();
#endif

#ifdef ImplementAttribId
   if (m_id == m_watchId)
   {
      int iii = 3;
   }
#endif

   setCoordinate(x,y);

   setRotationRadians(rotationRadians);
   setHeight(height);
   setWidth(width);

   setPenWidthIndex(penWidthIndex);
   setProportionalSpacing(proportional);
   setVisible(visible);
   setMirrorDisabled(never_mirror);
   setMirrorFlip(mirror_flip);
   setInherited(inherited);
   setLayerIndex(layer);
   setFlags(flg);
   setHorizontalPosition(horizontalPosition);
   setVerticalPosition(verticalPosition);
}

void CAttribute::duplicateInstance()
{
#ifdef AttribDebugMode
   assertValid();
#endif

   getDefinedAttributeNodeList().addNewNode(getDefinedFirstNode());

   //CAttribute* duplicate = new CAttribute(getCamCadData());
   //duplicate->setValue(*this);
   //duplicate->setProperties(*this);

   //duplicate->m_next = m_next;
   //m_next            = duplicate;

#ifdef AttribDebugMode
   assertValid();
#endif
}

void CAttribute::addInstance(CAttribute* attribute)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   attribute->setValue(*this);

   if (attribute->hasNodes())
   {
      getDefinedAttributeNodeList().addNewNode(attribute->getDefinedFirstNode());
   }
   else
   {
      getDefinedAttributeNodeList().addNewNode();
   }

   //bool memberFlag = false;

   //for (CAttribute* attrib = attribute;attrib != NULL;attrib = attrib->getNext())
   //{
   //   attrib->setValue(*this);

   //   if (attrib == this)
   //   {
   //      memberFlag = true;
   //   }
   //}

   //if (!memberFlag)
   //{
   //   CAttribute* tailAttribute = getTail();
   //   tailAttribute->m_next = attribute;
   //}

#ifdef AttribDebugMode
   assertValid();
#endif
}

//CAttribute* CAttribute::removeInstance(CAttribute* attribute)
//{
//   CAttribute* retval = NULL;
//
//   if (m_next != NULL)
//   {
//      if (attribute == this)
//      {
//         CAttribute tempAttrib;
//         tempAttrib.setProperties(*this);
//
//         setProperties(*m_next);
//         CAttribute* next = m_next;
//         m_next = next->m_next;
//         next->m_next = NULL;
//         next->setProperties(tempAttrib);
//
//         retval = next;
//      }
//      else
//      {
//         CAttribute* prevAttribute = this;
//
//         for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->getNext())
//         {
//            if (attrib == attribute)
//            {
//               prevAttribute->m_next = attribute->m_next;
//               attribute->m_next = NULL;
//
//               retval = attribute;
//               break;
//            }
//
//            prevAttribute = attrib;
//         }
//      }
//   }
//
//   return retval;
//}
//
//bool CAttribute::deleteInstance(CAttribute* attribute)
//{
//   CAttribute* attrib = removeInstance(attribute);
//
//   bool retval = (attrib != NULL);
//
//   if (retval)
//   {
//      delete attrib;
//   }
//
//   return retval;
//}
//
//bool CAttribute::deleteInstance(CAttributes& map)
//{
//   bool retval = false;
//   CAttribute* parentAttrib;
//   WORD key;
//
//   for (POSITION pos = map.GetStartPosition();pos != NULL && !retval;)
//   {
//      map.GetNextAssoc(pos,key,(void*&)parentAttrib);
//
//      retval = deleteInstance(parentAttrib);
//   }
//
//   return retval;
//}

bool CAttribute::deleteInstanceLike(const CAttribute& attribute)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   bool retval = false;
   //CAttribute* retval = NULL;

   if (hasNodes() && attribute.hasNodes())
   {
      retval = getDefinedAttributeNodeList().deleteInstanceLike(attribute.getDefinedFirstNode());
   }

   //if (m_next != NULL)
   //{
   //   if (propertiesAndValueEqual(attribute))
   //   {
   //      setProperties(*m_next);
   //      setValue(*m_next);
   //      CAttribute* next = m_next;
   //      m_next = next->m_next;
   //      next->m_next = NULL;

   //      delete next;

   //      retval = true;
   //   }
   //   else
   //   {
   //      CAttribute* prevAttribute = this;

   //      for (CAttribute* attrib = m_next;attrib != NULL;attrib = attrib->getNext())
   //      {
   //         if (attrib->propertiesAndValueEqual(attribute))
   //         {
   //            prevAttribute->m_next = attrib->m_next;
   //            attrib->m_next = NULL;

   //            delete attrib;

   //            retval = true;
   //            break;
   //         }

   //         prevAttribute = attrib;
   //      }
   //   }
   //}

#ifdef AttribDebugMode
   assertValid();
#endif

   return retval;
}

bool CAttribute::duplicateInstanceLike(const CAttribute& attribute)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   bool retval = false;

   if (hasNodes() && attribute.hasNodes())
   {
      retval = getDefinedAttributeNodeList().duplicateInstanceLike(attribute.getDefinedFirstNode());
   }

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   if (attrib->propertiesAndValueEqual(attribute))
   //   {
   //      attrib->duplicateInstance();
   //      retval = true;

   //      break;
   //   }
   //}

   return retval;
}

bool CAttribute::updateInstanceLike(const CAttribute& attribute,const CAttribute& updatedAttribute)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   bool retval = false;

   if (hasNodes() && attribute.hasNodes() && updatedAttribute.hasNodes())
   {
      retval = getDefinedAttributeNodeList().updateInstanceLike(attribute.getDefinedFirstNode(),updatedAttribute.getDefinedFirstNode());
   }

   setValueFromString(updatedAttribute.getStringValue());

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   if (attrib->propertiesAndValueEqual(attribute))
   //   {
   //      attrib->setProperties(updatedAttribute);
   //      attrib->setValue(updatedAttribute);
   //      retval = true;
   //   }
   //   else
   //   {
   //      attrib->setValue(updatedAttribute);
   //      retval = true;
   //   }
   //}

   return retval;
}

bool CAttribute::updatePropertiesOfInstanceLike(const CAttribute& attribute,const CAttribute& updatedAttribute)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   bool retval = false;

   if (hasNodes() && attribute.hasNodes() && updatedAttribute.hasNodes())
   {
      retval = getDefinedAttributeNodeList().updateInstanceLike(attribute.getDefinedFirstNode(),updatedAttribute.getDefinedFirstNode());
   }

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   if (attrib->propertiesAndValueEqual(attribute))
   //   {
   //      attrib->setProperties(updatedAttribute);
   //      retval = true;

   //      break;
   //   }
   //}

   return retval;
}

bool CAttribute::updateLayerOfInstanceLike(const CAttribute& attribute, const int layerIndex)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   bool retval = false;

   if (hasNodes() && attribute.hasNodes())
   {
      retval = getDefinedAttributeNodeList().updateLayerOfInstanceLike(attribute.getDefinedFirstNode(),layerIndex);
   }

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   if (attrib->propertiesAndValueEqual(attribute))
   //   {
   //      attrib->setLayerIndex(layerIndex);
   //      retval = true;

   //      break;
   //   }
   //}

   return retval;
}

bool CAttribute::updateValueOfInstanceLike(const CAttribute& attribute,const CAttribute& updatedAttribute)
{
#ifdef AttribDebugMode
   assertValid();
#endif

   bool retval = false;

   if (hasNodes() && attribute.hasNodes() && updatedAttribute.hasNodes())
   {
      if (propertiesAndValueEqual(attribute))
      {
         setValueFromString(updatedAttribute.getStringValue());

         retval = true;
      }
   }

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   if (attrib->propertiesAndValueEqual(attribute))
   //   {
   //      attrib->setValue(updatedAttribute);
   //      retval = true;

   //      break;
   //   }
   //}

   return retval;
}

bool CAttribute::propertiesEqual(const CAttribute& other) const
{
#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

   bool retval = (m_inherited == other.m_inherited && hasNodes() == other.hasNodes());

   if (retval && hasNodes())
   {
      retval = this->getDefinedFirstNode().propertiesEqual(other.getDefinedFirstNode());
   }

   return retval;
}

bool CAttribute::valueEqual(const CAttribute& other) const
{
#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

   bool retval = false;

   if (m_valueType == other.m_valueType)
   {
      switch (m_valueType)
      {
      case valueTypeString:
      case valueTypeEmailAddress:
      case valueTypeWebAddress:
         retval = (getStringValueIndex() == other.getStringValueIndex());
         break;
      case valueTypeDouble:
      case valueTypeUnitDouble:
         retval = fpeq(getDoubleValue(),other.getDoubleValue());
         break;
      case valueTypeInteger:
         retval = (getIntValue() == other.getIntValue());
         break;
      case valueTypeNone:
         retval = true;
         break;
      }
   }

   return retval;
}

bool CAttribute::propertiesAndValueEqual(const CAttribute& other) const
{
#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

   bool retval = valueEqual(other) && propertiesEqual(other);

   return retval;
}

bool CAttribute::findInstanceLike(const CAttribute& attribute)
{
#ifdef AttribDebugMode
   assertValid();
   attribute.assertValid();
#endif

   bool retval = false;

   if (hasNodes() && attribute.hasNodes())
   {
      retval = getDefinedAttributeNodeList().findInstanceLike(attribute.getDefinedFirstNode());
   }

   //for (CAttribute* attrib = this;!retval && attrib != NULL;attrib = attrib->getNext())
   //{
   //   retval = attribute.propertiesEqual(*attrib);
   //}

   return retval;
}

//bool CAttribute::getInstanceAt(int index,CAttribute& attribute)
//{
//   bool retval = false;
//
//   if (index >= 0)
//   {
//      CAttribute* attrib = this;
//
//      for (int attributeIndex = 0;;attributeIndex++)
//      {
//         if (attributeIndex == index)
//         {
//            attribute = *attrib;
//            retval = true;
//            break;
//         }
//
//         attrib = attrib->getNext();
//
//         if (attrib == NULL)
//         {
//            break;
//         }
//      }
//   }
//
//   return retval;
//}

void CAttribute::transform(const CTMatrix& transformationMatrix)
{
   // This seems pretty sketch to me, just up and scaling a double value.
   // It's fine if the double happened to be a linear value in same units as drawing.
   // But what if it is some random floating point attribute?
   // This is what 4.6 did, and there is no case against it as far as I know,
   // so we'll keep doing the same.
   if (m_valueType == valueTypeUnitDouble)
   {
      CBasesVector basesVector;
      basesVector.transform(transformationMatrix);
      double factor = basesVector.getScale();

      m_doubleValue *= factor;
   }


   // This part is normal enough, transform all the instances of the attrib
   transformAllInstances(transformationMatrix);
}

void CAttribute::transformAllInstances(const CTMatrix& transformationMatrix)
{
   if (hasNodes())
   {
      getDefinedAttributeNodeList().transform(transformationMatrix);
   }

   //for (CAttribute* attrib = this;attrib != NULL;attrib = attrib->m_next)
   //{
   //   attrib->transform(transformationMatrix);
   //}
}

void CAttribute::ConvertType(CCamCadData& camCadData, WORD toKeyword)
{
   ValueTypeTag fromType = this->getValueType();
   ValueTypeTag toType = (ValueTypeTag)camCadData.getKeyWordArray()[toKeyword]->getValueType();

   if (fromType != toType)
   {
      CString originalValStr = camCadData.getAttributeStringValue(*this);

      this->setValueType(toType);
      //for (CAttribute* attrib = this->m_next; attrib != NULL; attrib = attrib->m_next)
      //{
      //   attrib->setValueType(toType);
      //}

      this->setValueFromString(camCadData, toKeyword, originalValStr);
   }
}

bool CAttribute::isValid() const
{
   bool validFlag = true;

   switch (m_valueType)
   {
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      validFlag = (getStringValueIndex() >= 0);
      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      validFlag = (getDoubleValue() < 1.e50 && getDoubleValue() > -1.e50);
      break;
   case valueTypeInteger:
   case valueTypeNone:
      validFlag = true;
      break;
   default:
      validFlag = false;
      break;
   }

   //if (m_next != NULL)
   //{
   //   if (AfxIsValidAddress(m_next,sizeof(CAttribute)))
   //   {
   //      validFlag = m_next->isValid();
   //   }
   //   else
   //   {
   //      validFlag = false;
   //   }
   //}

   return validFlag;
}

void CAttribute::assertValid() const
{
   if (! isValid())
   {
      AfxDebugBreak();
      isValid();
   }
}

void CAttribute::dump(CWriteFormat& writeFormat,int depth,int keywordIndex,CCamCadData* camCadData)
{
   depth--;

writeFormat.writef(
"CAttribute\n"
"{\n");

   if (keywordIndex >= 0 && camCadData != NULL)
   {
      const KeyWordStruct* keyword = camCadData->getAttributeKeyword(keywordIndex);

      if (keyword != NULL)
      {
writeFormat.writef(
"   name      = '%s'\n",
keyword->getCCKeyword());
      }
   }

writeFormat.writef(
"   valueType = %s\n",
(const char*)valueTypeTagToString(m_valueType));

   if (hasNodes())
   {
      getDefinedAttributeNodeList().dump(writeFormat,depth);
   }

//   if (m_visible)
//   {
//writeFormat.writef(
//"   (x,y)     = (%.3f,%.3f)\n"
//"   rotation  = %.3f\n"
//"   m_height    = %.3f\n"
//"   m_width     = %.3f\n",
//getX(),getY(),
//m_rotationRadians,
//m_height,
//m_width);
//   }

   switch (m_valueType)
   {
   case valueTypeNone:
      break;
   case valueTypeInteger:
      writeFormat.writef("   value     =% d\n",getIntValue());
      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      writeFormat.writef("   value     = %.3f\n",getDoubleValue());
      break;
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      {
         CString value("n/a");
         
         if (camCadData != NULL)
         {
            value = camCadData->getAttributeStringValue(*this);
         }

         writeFormat.writef("   value     = \"%s\"\n",value);
      }

      break;
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
/*
CAttributeNode::CAttributeNode(
   double x,double y,double rotationRadians,double height,double width,int penWidthIndex,int layerIndex,
   bool visible,bool proportionalSpacing,bool mirrorDisabled,
   DbFlag flags,
   HorizontalPositionTag horizontalPosition,
   VerticalPositionTag verticalPosition)
: m_origin(x,y)
, m_rotationRadians(rotationRadians)
, m_height(height)
, m_width(width)
, m_penWidthIndex(penWidthIndex)
, m_layerIndex(layerIndex)
, m_visible(visible)
, m_proportional(proportionalSpacing)
, m_mirrorDisabledFlag(mirrorDisabled)
, m_mirrorFlipFlag(false)
, m_flags(flags)
, m_horizontalPosition(horizontalPosition)
, m_verticalPosition(verticalPosition)
{
}
*/

CAttributeNode::CAttributeNode(
   double x,double y,double rotationRadians,double height,double width,int penWidthIndex,int layerIndex,
   bool visible,bool proportionalSpacing,bool mirrorDisabled, bool mirrorFlip,
   DbFlag flags,
   HorizontalPositionTag horizontalPosition,
   VerticalPositionTag verticalPosition)
: m_origin(x,y)
, m_rotationRadians(rotationRadians)
, m_height(height)
, m_width(width)
, m_penWidthIndex(penWidthIndex)
, m_layerIndex(layerIndex)
, m_visible(visible)
, m_proportional(proportionalSpacing)
, m_mirrorDisabledFlag(mirrorDisabled)
, m_mirrorFlipFlag(mirrorFlip)
, m_flags(flags)
, m_horizontalPosition(horizontalPosition)
, m_verticalPosition(verticalPosition)
, m_fontProps(NULL)
{
}

CAttributeNode::CAttributeNode(const CAttributeNode& other)
: m_origin(other.m_origin)
, m_rotationRadians(other.m_rotationRadians)
, m_height(other.m_height)
, m_width(other.m_width)
, m_penWidthIndex(other.m_penWidthIndex)
, m_layerIndex(other.m_layerIndex)
, m_visible(other.m_visible)
, m_proportional(other.m_proportional)
, m_mirrorDisabledFlag(other.m_mirrorDisabledFlag)
, m_mirrorFlipFlag(other.m_mirrorFlipFlag)
, m_flags(other.m_flags)
, m_horizontalPosition(other.m_horizontalPosition)
, m_verticalPosition(other.m_verticalPosition)
, m_fontProps(NULL)
{
   if (other.m_fontProps != NULL)
   {
      this->GetDefinedFontProperties();  // makes sure this->fontProps is defined
      *this->m_fontProps = other.m_fontProps;
   }
}

CAttributeNode& CAttributeNode::operator=(const CAttributeNode& other)
{
   if (&other != NULL)
   {
      m_origin              = other.m_origin;
      m_rotationRadians     = other.m_rotationRadians;
      m_height              = other.m_height;
      m_width               = other.m_width;
      m_penWidthIndex       = other.m_penWidthIndex;
      m_layerIndex          = other.m_layerIndex;
      m_visible             = other.m_visible;
      m_proportional        = other.m_proportional;
      m_mirrorDisabledFlag  = other.m_mirrorDisabledFlag;
      m_mirrorFlipFlag      = other.m_mirrorFlipFlag;
      m_flags               = other.m_flags;
      m_horizontalPosition  = other.m_horizontalPosition;
      m_verticalPosition    = other.m_verticalPosition;

      if (other.m_fontProps != NULL)
      {
         this->GetDefinedFontProperties();  // makes sure this->fontProps is defined
         *this->m_fontProps = other.m_fontProps;
      }
      else
      {
         delete this->m_fontProps;
         this->m_fontProps = NULL;
      }
   }

   return *this;
}

CPoint2d CAttributeNode::getOrigin() const
{
   return m_origin;
}

void CAttributeNode::setOrigin(double x,double y)
{
   m_origin.x = x;
   m_origin.y = y;
   assertValidCoordinate();
}

void CAttributeNode::setOrigin(const CPoint2d& origin)
{
   m_origin = origin;
   assertValidCoordinate();
}

void CAttributeNode::offsetOrigin(double offsetX,double offsetY)
{
   m_origin.x += offsetX;
   m_origin.y += offsetY;
   assertValidCoordinate();
}

double CAttributeNode::getX() const
{
   return m_origin.x;
}

void CAttributeNode::setX(double x)
{
   m_origin.x = x;
   assertValidCoordinate();
}

double CAttributeNode::getY() const
{
   return m_origin.y;
}

void CAttributeNode::setY(double y)
{
   m_origin.y = y;
   assertValidCoordinate();
}

CBasesVector CAttributeNode::getBasesVector() const
{
   return CBasesVector(m_origin.x,m_origin.y,radiansToDegrees(m_rotationRadians));
}

const double CAttributeNode::getRotationRadians() const
{
   return m_rotationRadians;
}

const double CAttributeNode::getRotationDegrees() const
{
   return radiansToDegrees(m_rotationRadians);
}

void CAttributeNode::setRotationRadians(double radians)
{
   m_rotationRadians  = radians;
}

void CAttributeNode::setRotationDegrees(double degrees)
{
   m_rotationRadians  = degreesToRadians(degrees);
}

void CAttributeNode::incRotationRadians(double radians)
{
   m_rotationRadians += radians;
}

void CAttributeNode::incRotationDegrees(double degrees)
{
   m_rotationRadians += degreesToRadians(degrees);
}

double CAttributeNode::getHeight() const
{
   return m_height;
}

void CAttributeNode::setHeight(double height)
{
   m_height = height;
}

double CAttributeNode::getWidth() const
{
   return m_width;
}

void CAttributeNode::setWidth(double width)
{
   m_width = width;
}

int CAttributeNode::getPenWidthIndex() const
{
   return m_penWidthIndex;
}

void CAttributeNode::setPenWidthIndex(int index)
{
   m_penWidthIndex = index;
}

short CAttributeNode::getLayerIndex() const
{
   return m_layerIndex;
}

void CAttributeNode::setLayerIndex(int index)
{
   m_layerIndex = index;
}

bool CAttributeNode::isVisible() const
{
   return (m_visible != 0);
}

void CAttributeNode::setVisible(bool visibilityFlag)
{
   m_visible = visibilityFlag;
}

bool CAttributeNode::isProportionallySpaced() const
{
   return (m_proportional != 0);
}

void CAttributeNode::setProportionalSpacing(bool flag)
{
   m_proportional = flag;
}

bool CAttributeNode::getMirrorDisabled() const
{
   return (m_mirrorDisabledFlag != 0);
}

void CAttributeNode::setMirrorDisabled(bool flag)
{
   m_mirrorDisabledFlag = flag;
}


bool CAttributeNode::getMirrorFlip() const
{
   return (m_mirrorFlipFlag != 0);
}

void CAttributeNode::setMirrorFlip(bool flag)
{
   m_mirrorFlipFlag = flag;
}

HorizontalPositionTag CAttributeNode::getHorizontalPosition() const
{
   return (HorizontalPositionTag)m_horizontalPosition;
}

void CAttributeNode::setHorizontalPosition(HorizontalPositionTag horizontalPosition )
{
   m_horizontalPosition = horizontalPosition;
}

VerticalPositionTag CAttributeNode::getVerticalPosition() const
{
   return (VerticalPositionTag)m_verticalPosition;
}

void CAttributeNode::setVerticalPosition(VerticalPositionTag verticalPosition )
{
   m_verticalPosition = verticalPosition;
}

DbFlag CAttributeNode::getFlags() const
{
   return m_flags;
}

void CAttributeNode::setFlags(DbFlag attribFlag)
{
   m_flags = attribFlag;
}

void CAttributeNode::setFlagBits(DbFlag mask)
{
   m_flags |= mask;
}

void CAttributeNode::clearFlagBits(DbFlag mask)
{
   m_flags &= ~mask;
}

bool CAttributeNode::propertiesEqual(const CAttributeNode& other) const
{
#ifdef AttribDebugMode
   assertValid();
   other.assertValid();
#endif

   bool retval = (
       m_penWidthIndex       == other.m_penWidthIndex       &&
       m_proportional        == other.m_proportional        &&
       m_visible             == other.m_visible             &&
       m_mirrorDisabledFlag  == other.m_mirrorDisabledFlag  &&
       m_horizontalPosition  == other.m_horizontalPosition  &&
       m_verticalPosition    == other.m_verticalPosition    &&
       m_layerIndex          == other.m_layerIndex          &&
       m_flags               == other.m_flags               &&
       fpeq(m_origin.x   ,other.m_origin.x)                 &&
       fpeq(m_origin.y   ,other.m_origin.y)                 &&
       fpeq(m_rotationRadians,other.m_rotationRadians)      &&
       fpeq(m_height         ,other.m_height)               &&
       fpeq(m_width          ,other.m_width)            );

   return retval;
}

void CAttributeNode::transform(const CTMatrix& transformationMatrix)
{
   CBasesVector basesVector = getBasesVector();
   basesVector.transform(transformationMatrix);

   m_origin          = basesVector.getOrigin();
   m_rotationRadians = basesVector.getRotationRadians();


   double factor = basesVector.getScale();

   if (isVisible())
   {
      m_height *= factor;
      m_width *= factor;
   }
}

bool CAttributeNode::assertValidCoordinate()
{
   bool retval = true;

   if (! _finite(m_origin.x))
   {
      retval = false;
   }

   if (! _finite(m_origin.y))
   {
      retval = false;
   }

   return retval;
}

void CAttributeNode::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

writeFormat.writef(
"CAttributeNode\n"
"{\n"
"   visible   = %d"
"   (x,y)     = (%.3f,%.3f)\n"
"   rotation  = %.3f\n"
"   m_height    = %.3f\n"
"   m_width     = %.3f\n"
")\n",
m_visible,
getX(),getY(),
m_rotationRadians,
m_height,
m_width);
}

//_____________________________________________________________________________
CAttributeNodeList::CAttributeNodeList()
{
}

CAttributeNodeList::~CAttributeNodeList()
{
}

CAttributeNodeList& CAttributeNodeList::operator=(const CAttributeNodeList& other)
{
   if (&other != this)
   {
      empty();

      for (POSITION pos = other.m_attributeNodeList.GetHeadPosition();pos != NULL;)
      {
         CAttributeNode* attributeNode = other.m_attributeNodeList.GetNext(pos);

         addNewNode(*attributeNode);
      }
   }

   return *this;
}

void CAttributeNodeList::empty()
{
   m_attributeNodeList.empty();
}

int CAttributeNodeList::getCount() const
{
   int count = m_attributeNodeList.GetCount();

   return count;
}

CAttributeNode* CAttributeNodeList::getHead() const
{
   CAttributeNode* attributeNode = NULL;
   
   if (m_attributeNodeList.GetCount() > 0)
   {
      attributeNode = m_attributeNodeList.GetHead();
   }

   return attributeNode;
}

POSITION CAttributeNodeList::getHeadPosition() const
{
   return m_attributeNodeList.GetHeadPosition();
}

CAttributeNode* CAttributeNodeList::getNext(POSITION& pos)
{
   return m_attributeNodeList.GetNext(pos);
}

CAttributeNode* CAttributeNodeList::addNewNode(
   double x,double y,double rotationRadians,double height,double width,int penWidthIndex,int layerIndex,
   bool visible,bool proportionalSpacing,bool mirrorDisabled,bool mirrorFlip,
   DbFlag flags,
   HorizontalPositionTag horizontalPosition,
   VerticalPositionTag verticalPosition)
{
   CAttributeNode* attributeNode = new CAttributeNode(x,y,rotationRadians,height,width,penWidthIndex,
      layerIndex,visible,proportionalSpacing,mirrorDisabled, mirrorFlip,                        
      flags,horizontalPosition,verticalPosition);

   m_attributeNodeList.AddTail(attributeNode);

   return attributeNode;
}

CAttributeNode* CAttributeNodeList::addNewNode(const CAttributeNode& attributeNodeToCopy)
{
   CAttributeNode* attributeNode = new CAttributeNode(attributeNodeToCopy);

   m_attributeNodeList.AddTail(attributeNode);

   return attributeNode;
}

void CAttributeNodeList::setVisibleAllInstances(bool visibilityFlag)
{
   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      listNode->setVisible(visibilityFlag);
   }
}

bool CAttributeNodeList::findInstanceLike(const CAttributeNode& attributeNode)
{
   bool retval = false;

   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      retval = attributeNode.propertiesEqual(*listNode);
   }

   return retval;
}

bool CAttributeNodeList::deleteInstanceLike(const CAttributeNode& attributeNode)
{
   bool retval = false;

   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      if (listNode->propertiesEqual(attributeNode))
      {
         m_attributeNodeList.RemoveAt(oldPos);

         delete listNode;

         retval = true;

         break;
      }
   }

   return retval;
}

bool CAttributeNodeList::duplicateInstanceLike(const CAttributeNode& attributeNode)
{
   bool retval = false;

   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      if (listNode->propertiesEqual(attributeNode))
      {
         addNewNode(*listNode);

         retval = true;

         break;
      }
   }

   return retval;
}

bool CAttributeNodeList::updateInstanceLike(const CAttributeNode& attributeNode,const CAttributeNode& updatedAttributeNode)
{
   bool retval = false;

   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      if (listNode->propertiesEqual(attributeNode))
      {
         *listNode = updatedAttributeNode;

         retval = true;
      }
   }

   return retval;
}

bool CAttributeNodeList::updateLayerOfInstanceLike(const CAttributeNode& attributeNode, const int layerIndex)
{
   bool retval = false;

   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      if (listNode->propertiesEqual(attributeNode))
      {
         listNode->setLayerIndex(layerIndex);

         retval = true;
      }
   }

   return retval;
}

void CAttributeNodeList::transform(const CTMatrix& transformationMatrix)
{
   for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
   {
      CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

      listNode->transform(transformationMatrix);
   }
}

void CAttributeNodeList::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   writeFormat.writef(
"CAttributeNodeList\n"
"{\n"
"   Count=%d\n",
getCount());

   if (depth != 0)
   {
      writeFormat.pushPrefix("   ");

      for (POSITION pos = m_attributeNodeList.GetHeadPosition();pos != NULL;)
      {
         CAttributeNode* listNode = m_attributeNodeList.GetNext(pos);

         listNode->dump(writeFormat,depth);
      }

      writeFormat.popPrefix();
   }

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
AttribIterator::AttribIterator(const CAttribute* attrib)
{
   m_attrib = attrib;
   //m_next   = NULL;
   m_attrib->getDefinedFirstNode();
   m_pos = m_attrib->getHeadNodePosition();
}

bool AttribIterator::getNext(CAttribute& attrib)
{
   bool retval = false;

   if (m_pos != NULL)
   {
      CAttributeNode* attributeNode = m_attrib->getNextNode(m_pos);

      attrib.deleteNodes();
      attrib.getDefinedAttributeNodeList().addNewNode(*attributeNode);

      attrib.setValue(*m_attrib);      
      attrib.setProperties(attributeNode->getX(), attributeNode->getY(), attributeNode->getRotationRadians(), attributeNode->getHeight(), 
         attributeNode->getWidth(), attributeNode->getPenWidthIndex(), attributeNode->isProportionallySpaced(), attributeNode->isVisible(), attributeNode->getMirrorDisabled(), 
         attributeNode->getMirrorFlip(), m_attrib->isInherited(), attributeNode->getHorizontalPosition(), attributeNode->getVerticalPosition(), attributeNode->getLayerIndex(), attributeNode->getFlags());

      retval = true;
   }

   //if (m_next == NULL)
   //{
   //   m_next = m_attrib;
   //}
   //else
   //{
   //   m_next = m_next->m_next;
   //}

   //if (m_next != NULL)
   //{
   //   attrib.setProperties(*m_next);
   //   attrib.setValue(*m_next);
   //   retval = true;
   //}

   return retval;
}

//_____________________________________________________________________________
CAttributes::CAttributes()
{
}

CAttributes::CAttributes(const CAttributes& other)
{
   *this = other;
}

CAttributes::~CAttributes()
{
   empty();
}

void CAttributes::empty()
{
   m_map.empty();
}

CAttributes& CAttributes::operator=(const CAttributes& other)
{
   if (&other != this)
   {
      empty();

      this->CopyAll(other);
   }

   return *this;
}

POSITION CAttributes::GetStartPosition() const
{
   return m_map.GetStartPosition();
}

void CAttributes::GetNextAssoc(POSITION& nextPosition,WORD& key,CAttribute*& value) const
{
   return m_map.GetNextAssoc(nextPosition,key,value);
}

CAttribute* CAttributes::getFirst() const
{
   CAttribute* firstAttribute = NULL;

   POSITION pos = m_map.GetStartPosition();

   if (pos != NULL)
   {
      WORD key;
      m_map.GetNextAssoc(pos,key,firstAttribute);
   }

   return firstAttribute;
}

void CAttributes::SetAt(WORD key,CAttribute* newValue)
{
   CAttribute* existingAttribute;

   if (m_map.Lookup(key,existingAttribute))
   {
      delete existingAttribute;
   }

   newValue->setKeywordIndex(key);

   m_map.SetAt(key,newValue);
}

bool CAttributes::Lookup(WORD key,CAttribute*& value) const 
{
   return (m_map.Lookup(key,value) != 0);
}

CAttribute* CAttributes::lookupAttribute(int keywordIndex) const
{
   CAttribute* attribute;

   if (! Lookup(keywordIndex,attribute))
   {
      attribute = NULL;
   }

   return attribute;
}

CAttribute* CAttributes::lookupAttribute(StandardAttributeTag attributeTag) const
{
   CAttribute* attribute      = NULL;
   CAttribute* firstAttribute = getFirst();

   if (firstAttribute != NULL)
   {
      int keywordIndex = firstAttribute->getCamCadData().getAttributeKeywordIndex(attributeTag);

      if (keywordIndex >= 0)
      {
         if (! Lookup(keywordIndex,attribute))
         {
            attribute = NULL;
         }
      }
   }

   return attribute;
}

bool CAttributes::getAttribute(int keywordIndex) const
{
   CAttribute* attribute;
   bool retval = Lookup(keywordIndex,attribute);

   return retval;
}

bool CAttributes::getAttribute(int keywordIndex,int& value) const
{
   bool retval = false;
   CAttribute* attribute;

   if (Lookup(keywordIndex,attribute))
   {
      value = attribute->getIntValue();
      retval = true;
   }

   return retval;
}

bool CAttributes::getAttribute(int keywordIndex,double& value) const
{
   bool retval = false;
   CAttribute* attribute;

   if (Lookup(keywordIndex,attribute))
   {
      value = attribute->getDoubleValue();
      retval = true;
   }

   return retval;
}

bool CAttributes::getAttribute(int keywordIndex,CString& value) const
{
   bool retval = false;
   CAttribute* attribute;

   if (Lookup(keywordIndex,attribute))
   {
      value = attribute->getStringValue();
      retval = true;
   }

   return retval;
}

bool CAttributes::getAttribute(StandardAttributeTag attributeTag) const
{
   bool retval = false;

   CAttribute* firstAttribute = getFirst();

   if (firstAttribute != NULL)
   {
      int keywordIndex = firstAttribute->getCamCadData().getAttributeKeywordIndex(attributeTag);

      if (keywordIndex >= 0)
      {
         retval = getAttribute(keywordIndex);
      }
   }

   return retval;
}

bool CAttributes::getAttribute(StandardAttributeTag attributeTag,int& value) const
{
   bool retval = false;

   CAttribute* firstAttribute = getFirst();

   if (firstAttribute != NULL)
   {
      int keywordIndex = firstAttribute->getCamCadData().getAttributeKeywordIndex(attributeTag);

      if (keywordIndex >= 0)
      {
         retval = getAttribute(keywordIndex,value);
      }
   }

   return retval;
}

bool CAttributes::getAttribute(StandardAttributeTag attributeTag,double& value) const
{
   bool retval = false;

   CAttribute* firstAttribute = getFirst();

   if (firstAttribute != NULL)
   {
      int keywordIndex = firstAttribute->getCamCadData().getAttributeKeywordIndex(attributeTag);

      if (keywordIndex >= 0)
      {
         retval = getAttribute(keywordIndex,value);
      }
   }

   return retval;
}

bool CAttributes::getAttribute(StandardAttributeTag attributeTag,CString& value) const
{
   bool retval = false;

   CAttribute* firstAttribute = getFirst();

   if (firstAttribute != NULL)
   {
      int keywordIndex = firstAttribute->getCamCadData().getAttributeKeywordIndex(attributeTag);

      if (keywordIndex >= 0)
      {
         retval = getAttribute(keywordIndex,value);
      }
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,int keywordIndex) const  // deprecated
{
   CAttribute* attribute;
   bool retval = Lookup(keywordIndex,attribute);

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,int keywordIndex,int& value) const  // deprecated
{
   bool retval = false;
   CAttribute* attribute;

   if (Lookup(keywordIndex,attribute))
   {
      value = camCadData.getAttributeIntegerValue(*attribute);
      retval = true;
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,int keywordIndex,double& value) const  // deprecated
{
   bool retval = false;
   CAttribute* attribute;

   if (Lookup(keywordIndex,attribute))
   {
      value = camCadData.getAttributeDoubleValue(*attribute);
      retval = true;
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,int keywordIndex,CString& value) const  // deprecated
{
   bool retval = false;
   CAttribute* attribute;

   if (Lookup(keywordIndex,attribute))
   {
      value = camCadData.getAttributeStringValue(*attribute);
      retval = true;
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag) const  // deprecated
{
   bool retval = false;

   int keywordIndex = camCadData.getAttributeKeywordIndex(StandardAttributeTagToName(attributeTag));

   if (keywordIndex >= 0)
   {
      retval = getAttribute(camCadData,keywordIndex);
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag,int& value) const  // deprecated
{
   bool retval = false;

   int keywordIndex = camCadData.getAttributeKeywordIndex(StandardAttributeTagToName(attributeTag));

   if (keywordIndex >= 0)
   {
      retval = getAttribute(camCadData,keywordIndex,value);
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag,double& value) const  // deprecated
{
   bool retval = false;

   int keywordIndex = camCadData.getAttributeKeywordIndex(StandardAttributeTagToName(attributeTag));

   if (keywordIndex >= 0)
   {
      retval = getAttribute(camCadData,keywordIndex,value);
   }

   return retval;
}

bool CAttributes::getAttribute(const CCamCadData& camCadData,StandardAttributeTag attributeTag,CString& value) const  // deprecated
{
   bool retval = false;

   int keywordIndex = camCadData.getAttributeKeywordIndex(StandardAttributeTagToName(attributeTag));

   if (keywordIndex >= 0)
   {
      retval = getAttribute(camCadData,keywordIndex,value);
   }

   return retval;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,int keywordIndex)
{
   return setAttribute(camCadData,keywordIndex,"");
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,int keywordIndex,int value,AttributeUpdateMethodTag attributeUpdateMethod)
{
   CAttribute* attribute = NULL;

   const KeyWordStruct* keyword = camCadData.getAttributeKeyword(keywordIndex);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keyword != NULL)
   {
      Lookup(keywordIndex,attribute);

      if (attribute == NULL)
      {
         attribute = camCadData.constructAttribute(keyword->getValueType());
         attribute->setKeywordIndex(keywordIndex);

         m_map.setAt(keywordIndex,attribute);

         attributeUpdateMethod = attributeUpdateOverwrite;
      }

      if (attributeUpdateMethod != attributeUpdateFenced)
      {
         switch (keyword->getValueType())
         {
         case valueTypeInteger:
            attribute->setIntValue(value);
            break;
         case valueTypeDouble:
         case valueTypeUnitDouble:
            attribute->setDoubleValue(value);
            break;
         case valueTypeNone:
            attribute->setDoubleValue(0.0);
            break;
         case valueTypeString:
         case valueTypeEmailAddress:
         case valueTypeWebAddress:
            {
               CString stringValue;
               stringValue.Format("%d",value);

               if (attributeUpdateMethod == attributeUpdateAppend)
               {
                  CString oldStringValue = camCadData.getAttributeStringValue(*attribute);

                  if (oldStringValue != stringValue && !oldStringValue.IsEmpty())
                  {
                     stringValue = oldStringValue + "\n" + stringValue;
                  }
               }

               int stringIndex = camCadData.getDefinedAttributeStringValueIndex(stringValue);
               attribute->setStringValueIndex(stringIndex);
            }

            break;
         }
      }
   }

   return attribute;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,int keywordIndex,double value,AttributeUpdateMethodTag attributeUpdateMethod)
{
   CAttribute* attribute = NULL;

   const KeyWordStruct* keyword = camCadData.getAttributeKeyword(keywordIndex);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keyword != NULL)
   {
      Lookup(keywordIndex,attribute);

      if (attribute == NULL)
      {
         attribute = camCadData.constructAttribute(keyword->getValueType());
         attribute->setKeywordIndex(keywordIndex);

         m_map.setAt(keywordIndex,attribute);

         attributeUpdateMethod = attributeUpdateOverwrite;
      }

      if (attributeUpdateMethod != attributeUpdateFenced)
      {
         switch (keyword->getValueType())
         {
         case valueTypeInteger:
            attribute->setIntValue(DcaRound(value));
            break;
         case valueTypeDouble:
         case valueTypeUnitDouble:
            attribute->setDoubleValue(value);
            break;
         case valueTypeNone:
            attribute->setDoubleValue(0.0);
            break;
         case valueTypeString:
         case valueTypeEmailAddress:
         case valueTypeWebAddress:
            {
               CString stringValue;
               stringValue.Format("%f",value);

               if (attributeUpdateMethod == attributeUpdateAppend)
               {
                  CString oldStringValue = camCadData.getAttributeStringValue(*attribute);

                  if (oldStringValue != stringValue && !oldStringValue.IsEmpty())
                  {
                     stringValue = oldStringValue + "\n" + stringValue;
                  }
               }

               int stringIndex = camCadData.getDefinedAttributeStringValueIndex(stringValue);
               attribute->setStringValueIndex(stringIndex);
            }

            break;
         }
      }
   }

   return attribute;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,int keywordIndex,const CString& value,AttributeUpdateMethodTag attributeUpdateMethod)
{
   CAttribute* attribute = NULL;

   const KeyWordStruct* keyword = camCadData.getAttributeKeyword(keywordIndex);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keyword != NULL)
   {
      Lookup(keywordIndex,attribute);

      if (attribute == NULL)
      {
         attribute = camCadData.constructAttribute(keyword->getValueType());
         attribute->setKeywordIndex(keywordIndex);

         m_map.setAt(keywordIndex,attribute);

         attributeUpdateMethod = attributeUpdateOverwrite;
      }

      if (attributeUpdateMethod != attributeUpdateFenced)
      {
         switch (keyword->getValueType())
         {
         case valueTypeInteger:
            attribute->setIntValue(atoi(value));
            break;
         case valueTypeDouble:
         case valueTypeUnitDouble:
            attribute->setDoubleValue(atof(value));
            break;
         case valueTypeNone:
            attribute->setDoubleValue(0.0);
            break;
         case valueTypeString:
         case valueTypeEmailAddress:
         case valueTypeWebAddress:
            {
               CString stringValue(value);

               if (attributeUpdateMethod == attributeUpdateAppend)
               {
                  CString oldStringValue = camCadData.getAttributeStringValue(*attribute);

                  if (oldStringValue != stringValue && !oldStringValue.IsEmpty())
                  {
                     stringValue = oldStringValue + "\n" + stringValue;
                  }
               }

               int stringIndex = camCadData.getDefinedAttributeStringValueIndex(stringValue);
               attribute->setStringValueIndex(stringIndex);
            }

            break;
         }
      }
   }

   return attribute;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag)
{
   CAttribute* attribute = NULL;

   int keywordIndex = camCadData.getAttributeKeywordIndex(attributeTag);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keywordIndex >= 0)
   {
      attribute = setAttribute(camCadData,keywordIndex);
   }

   return attribute;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag,int value,AttributeUpdateMethodTag attributeUpdateMethod)
{
   CAttribute* attribute = NULL;

   int keywordIndex = camCadData.getAttributeKeywordIndex(attributeTag);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keywordIndex >= 0)
   {
      attribute = setAttribute(camCadData,keywordIndex,value);
   }

   return attribute;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag,double value,AttributeUpdateMethodTag attributeUpdateMethod)
{
   CAttribute* attribute = NULL;

   int keywordIndex = camCadData.getAttributeKeywordIndex(attributeTag);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keywordIndex >= 0)
   {
      attribute = setAttribute(camCadData,keywordIndex,value);
   }

   return attribute;
}

CAttribute* CAttributes::setAttribute(CCamCadData& camCadData,StandardAttributeTag attributeTag,const CString& value,AttributeUpdateMethodTag attributeUpdateMethod)
{
   CAttribute* attribute = NULL;

   int keywordIndex = camCadData.getAttributeKeywordIndex(attributeTag);

   if (keywordIndex == 215)
   {
      int iii = 3;
   }

   if (keywordIndex >= 0)
   {
      attribute = setAttribute(camCadData,keywordIndex,value);
   }

   return attribute;
}

INT_PTR CAttributes::GetSize() const
{
   return m_map.GetSize();
}

INT_PTR CAttributes::GetCount() const
{
   return m_map.GetCount();
}

bool CAttributes::IsEmpty() const
{
   return (m_map.IsEmpty() != 0);
}

UINT CAttributes::GetHashTableSize() const
{
   return m_map.GetHashTableSize();
}

void CAttributes::InitHashTable(UINT hashSize,bool bAllocNow)
{
   m_map.InitHashTable(hashSize,bAllocNow);
}

bool CAttributes::deleteAttribute(int key)
{
   CAttribute* attribute = removeAttribute(key);
   
   bool retval = (attribute != NULL);

   delete attribute;

   return retval;
}

bool CAttributes::changeAttributeKey(int oldKeywordIndex,int newKeywordIndex)
{
   CAttribute* attribute = removeAttribute(oldKeywordIndex);
   
   bool retval = (attribute != NULL);

   if (retval)
   {
      SetAt(newKeywordIndex,attribute);
   }

   return retval;
}

CAttribute* CAttributes::removeAttribute(int key)
{
   CAttribute* attribute = NULL;

   if (m_map.Lookup(key,attribute))
   {
      m_map.RemoveKey(key);
   }

   return attribute;
}

CAttributes& CAttributes::CopyAll(const CAttributes& other)
{
   if (&other != this)
   {
      WORD key;
      CAttribute* attrib;

      for (POSITION attribPos = other.GetStartPosition(); attribPos != NULL;)
      {
         other.GetNextAssoc(attribPos,key,attrib);

         CAttribute *newAttrib = attrib->allocateCopy();
         SetAt(key, newAttrib);
      }
   }

   return *this;
}

CAttributes& CAttributes::CopyNew(const CAttributes& other)
{
   // Copy only attributes that do not already have instance in target

   if (&other != this)
   {
      WORD key;
      CAttribute* otherAttribute;
      CAttribute* existingAttribute;

      for (POSITION attribPos = other.GetStartPosition(); attribPos != NULL;)
      {
         other.GetNextAssoc(attribPos,key,otherAttribute);
         
         if (!Lookup(key, existingAttribute))
         {
            CAttribute* newAttribute = otherAttribute->allocateCopy();
            m_map.SetAt(key, newAttribute);
         }
      }
   }

   return *this;
}

void CAttributes::merge(CCamCadData& camCadData,int sourceKeyIndex,int destinationKeyIndex,AttributeMergeMethodTag mergeMethod)
{
   CAttribute* sourceAttribute;

   if (!Lookup(sourceKeyIndex, sourceAttribute))
   {
      // no source attribute, so just keep the destination attribute, if it exists

      return;
   }

   sourceAttribute = removeAttribute(sourceKeyIndex);
   CAttribute* destinationAttribute = removeAttribute(destinationKeyIndex);

   if (destinationAttribute == NULL)
   {
      // no destination attribute, so just move the source to the destination
      SetAt(destinationKeyIndex,sourceAttribute);

      return;
   }

   // both source and destination were present
   switch (mergeMethod)
   {
   case attributeMergeMethodRemoveSourceKeepDestination:  // keep "to" attribute, remove "from" attribute
      SetAt(destinationKeyIndex,destinationAttribute);

      delete sourceAttribute;

      break;

   case attributeMergeMethodMoveSourceToDestination:  // remove "to" attribute, move "from" to "to"
      SetAt(destinationKeyIndex,sourceAttribute);

      delete destinationAttribute;

      break;

   case attributeMergeMethodCopyNewSourceToDestination:  // add reinitialized "from" attribute to "to", remove original "to" and "from" attributes
      {
         //doc->SetAttrib(&map, toKey, valueTypeString, // toAttrib->getValueType(),
         //   fromAttrib->getStringValue().GetBuffer(0), SA_APPEND, NULL);
         CString stringValue = camCadData.getAttributeStringValue(*sourceAttribute);

         camCadData.setAttribute(*this,destinationKeyIndex,stringValue);

         delete sourceAttribute;
         delete destinationAttribute;
      }

      break;
   default:
      delete sourceAttribute;
      delete destinationAttribute;

      break;
   }
}

void CAttributes::transform(const CTMatrix& transformationMatrix)
{
   WORD key;
   CAttribute* attrib;

   for (POSITION attribPos = GetStartPosition();attribPos != NULL;)
   {
      GetNextAssoc(attribPos,key,attrib);

      // Must transform all instance of an attribute
      attrib->transform(transformationMatrix);
   }
}

CString CAttributes::getNameValues() const
{
   CString nameValues;

   WORD key;
   CAttribute* attrib;

   for (POSITION attribPos = GetStartPosition();attribPos != NULL;)
   {
      GetNextAssoc(attribPos,key,attrib);

      nameValues += attrib->getNameValue() + "\n";
   }

   return nameValues;
}

bool CAttributes::IsEqual(const CAttributes& other, EAttribCompare attribCompareBy)
{
   bool retval = true;

   if (this->GetCount() != other.GetCount())
   {
      retval = false;
   }
   else
   {      
      for (POSITION pos = GetStartPosition();pos != NULL;)
      {
         WORD key;
         CAttribute* attrib = NULL;
         GetNextAssoc(pos, key, attrib);

         CAttribute* attribOther = NULL;

         if (!other.Lookup(key, attribOther))
         {
            retval = false;
            break;
         }

         if ((attribCompareBy & attribCompareByValue) != 0)
         {
            if (!attrib->valueEqual(*attribOther))
            {
               retval = false;
               break;
            }
         }

         if ((attribCompareBy & attribCompareByProperty) != 0)
         {
            if (!attrib->propertiesEqual(*attribOther))
            {
               retval = false;
               break;
            }
         }
      }
   }

   return retval;
}

//_____________________________________________________________________________
CNamedAttributeInstance::CNamedAttributeInstance(WORD key,const CString& name,const CAttribute& attrib,int instanceNumber)
: m_attribute(attrib)
{
   m_key  = key;
   m_name = name;
   m_instanceNumber = instanceNumber;
}

WORD CNamedAttributeInstance::getKey() const
{
   return m_key;
}

CString CNamedAttributeInstance::getName() const
{
   return m_name;
}

const CAttribute& CNamedAttributeInstance::getAttribute() const
{
   return m_attribute;
}

int CNamedAttributeInstance::getInstanceNumber() const
{
   return m_instanceNumber;
}

//_____________________________________________________________________________
CAttributeMapWrap::CAttributeMapWrap(CAttributes*& map)
: m_map(map)
{
}

CAttributeMapWrap::~CAttributeMapWrap()
{
}

void CAttributeMapWrap::empty()
{
   if (m_map != NULL)
   {
      m_map->empty();

      //CAttribute* attrib;
      //WORD keyword;

      //for (POSITION pos = m_map->GetStartPosition();pos != NULL;)
      //{
      //   m_map->GetNextAssoc(pos,keyword,(void*&)attrib);
      //   delete attrib;
      //}

      //m_map->RemoveAll();

      delete m_map;
      m_map = NULL;
   }
}

CAttributes& CAttributeMapWrap::getMap()
{
   if (m_map == NULL)
   {
      m_map = new CAttributes();
   }

   return *m_map;
}

INT_PTR CAttributeMapWrap::GetCount() const
{ 
   return (m_map == NULL ? 0 : m_map->GetCount()); 
}

bool CAttributeMapWrap::IsEmpty() const
{ 
   return (m_map == NULL || m_map->IsEmpty()); 
}

bool CAttributeMapWrap::Lookup(WORD key,CAttribute*& attribute) const
{ 
   bool retval = ((m_map != NULL) && m_map->Lookup(key,attribute));

   return retval;
}

void CAttributeMapWrap::SetAt(WORD key,CAttribute* attribute)
{ 
   RemoveKey(key);

   if (attribute != NULL)
   {
      getMap().SetAt(key,attribute);
   }
}

CAttribute* CAttributeMapWrap::RemoveKey(WORD key)
{ 
   CAttribute* retval = NULL;

   if (Lookup(key,retval))
   {
      m_map->deleteAttribute(key);

      if (m_map->GetCount() == 0)
      {
         delete m_map;
         m_map = NULL;
      }
   }

   return retval;
}

bool CAttributeMapWrap::deleteKey(WORD key)
{ 
   bool retval = false;

   CAttribute* oldAttribute;

   if (Lookup(key,oldAttribute))
   {
      m_map->deleteAttribute(key);
      //delete oldAttribute;

      retval = true;

      if (m_map->GetCount() == 0)
      {
         delete m_map;
         m_map = NULL;
      }
   }

   return retval;
}

void CAttributeMapWrap::RemoveAll()
{ 
   empty(); 
}

POSITION CAttributeMapWrap::GetStartPosition() const
{ 
   POSITION retval = ((m_map == NULL) ? NULL : m_map->GetStartPosition()); 

   return retval;
}

void CAttributeMapWrap::GetNextAssoc(POSITION& rNextPosition,WORD& rKey,CAttribute*& attribute) const
{ 
   m_map->GetNextAssoc(rNextPosition,rKey,attribute);
}

bool CAttributeMapWrap::deleteInstanceLike(const CNamedAttributeInstance& attributeInstance)
{
   bool retval = false;

   if (m_map != NULL)
   {
      deleteKey(attributeInstance.getKey());
      return true;
   }
   return retval;
}

bool CAttributeMapWrap::duplicateInstanceLike(const CNamedAttributeInstance& attributeInstance)
{
   bool retval = false;

   if (m_map != NULL)
   {
      CAttribute* parentAttribute;

      if (m_map->Lookup(attributeInstance.getKey(),parentAttribute))
      {
         if (parentAttribute->duplicateInstanceLike(attributeInstance.getAttribute()))
         {
            retval = true;
         }
      }
   }

   return retval;
}

bool CAttributeMapWrap::updateInstanceLike(const CNamedAttributeInstance& attributeInstance,
   const CAttribute& updatedAttribute)
{
   bool retval = false;

   if (m_map != NULL)
   {
      CAttribute* parentAttribute;

      if (m_map->Lookup(attributeInstance.getKey(),parentAttribute))
      {
         if (parentAttribute->updateInstanceLike(attributeInstance.getAttribute(),updatedAttribute))
         {
            retval = true;
         }
      }
   }

   return retval;
}

void CAttributeMapWrap::copyFrom(CAttributes* map)
{
   CAttribute* attrib;
   WORD word;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos,word,attrib);

      //CString nameValue = attrib->getNameValue();

      CAttribute* attribCopy = new CAttribute(*attrib);
      addInstance(word,attribCopy);
   }
}

//CAttribute* CAttributeMapWrap::removeInstance(CAttribute* attribute)
//{
//   CAttribute* retval = NULL;
//
//   if (m_map != NULL)
//   {
//      CAttribute* parentAttribute;
//      WORD keyword;
//
//      for (POSITION pos = m_map->GetStartPosition();pos != NULL && retval == NULL;)
//      {
//         m_map->GetNextAssoc(pos,keyword,(void*&)parentAttribute);
//
//         retval = parentAttribute->removeInstance(attribute);
//
//         if (retval == NULL && parentAttribute == attribute)
//         {
//            retval = RemoveKey(keyword);
//         }
//      }
//   }
//
//   return retval;
//}

void CAttributeMapWrap::addInstance(WORD key,CAttribute* attribute)
{
   CAttribute* parentAttribute;

   if (Lookup(key,parentAttribute))
   {
      parentAttribute->addInstance(attribute);
   }
   else
   {
      SetAt(key,attribute);
   }
}

bool CAttributeMapWrap::addInstance(CCamCadData& camCadData,int keyword,
   ValueTypeTag valueType,void *value,
   double x,double y,double rotation,double height,double width,bool proportional,int penWidthIndex,
   bool visible,DbFlag flag,short layer,bool neverMirror,bool mirrorFlip,
   HorizontalPositionTag horizontalPosition, VerticalPositionTag verticalPosition)
{  
   CAttributes* map = new CAttributes();

   bool retval = camCadData.setVisibleAttribute(*map,keyword,valueType,value,x,y,rotation,height,width,
         proportional,penWidthIndex,visible,attributeUpdateAppend,flag,layer,neverMirror,mirrorFlip,horizontalPosition,verticalPosition) != 0;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      CAttribute* attrib;
      WORD keyword;

      map->GetNextAssoc(pos,keyword,attrib);
      // Case 2157
      // addInstance(keyword,copyAttrib);  original code, bad!
      // Can't add the attrib directly from the local map, since the map is a 
      // container, it and everything in it will be destroyed when map is destroyed.
      // Having this map's attrib placed directly into another list means that item in the
      // other list becomes damaged when this map is deleted.
      // Need to put a copy in that other list.
      CAttribute *attribCopy = new CAttribute(*attrib);
      addInstance(keyword, attribCopy);
   }

   delete map;

   return retval;
}

void CAttributeMapWrap::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   if (m_map != NULL)
   {
      writeFormat.writef(
   "AttributeMap\n"
   "{\n"
   "   Count=%d\n",
   m_map->GetCount());

      if (depth != 0)
      {
         writeFormat.pushPrefix("   ");

         CAttribute* attrib;
         WORD word;

         for (POSITION pos = m_map->GetStartPosition();pos != NULL;)
         {
            m_map->GetNextAssoc(pos,word,attrib);

            if (attrib != NULL)
            {
               attrib->dump(writeFormat,depth,word);
            }
         }

         writeFormat.popPrefix();
      }

      writeFormat.writef("}\n");
   }
}

//_____________________________________________________________________________
CSortedAttributeListEntry::CSortedAttributeListEntry(int keywordIndex,bool isContainer)
: m_keywordIndex(keywordIndex)
, m_attributeList(isContainer)
{
}

int CSortedAttributeListEntry::getKeywordIndex() const
{
   return m_keywordIndex;
}

POSITION CSortedAttributeListEntry::getHeadPosition() const
{
   return m_attributeList.GetHeadPosition();
}

CAttribute* CSortedAttributeListEntry::getNext(POSITION& pos) const
{
   return m_attributeList.GetNext(pos);
}

int CSortedAttributeListEntry::getCount() const
{
   return m_attributeList.GetCount();
}

void CSortedAttributeListEntry::add(CAttribute& attribute)
{
   m_attributeList.AddTail(&attribute);
}

//_____________________________________________________________________________
CSortedAttributeList::CSortedAttributeList(const CAttributes& attributes)
: m_attributeList(false)
{
   WORD keywordIndex;
   CAttribute* attribute;

   for (POSITION pos = attributes.GetStartPosition();pos != NULL;)
   {
      attributes.GetNextAssoc(pos,keywordIndex,attribute);
      add(keywordIndex,*attribute);
   }
}

POSITION CSortedAttributeList::getHeadPosition() const
{
   return m_attributeList.GetHeadPosition();
}

CSortedAttributeListEntry* CSortedAttributeList::getNext(POSITION& pos) const
{
   return m_attributeList.GetNext(pos);
}

int CSortedAttributeList::getCount() const
{
   return m_attributeList.GetCount();
}

void CSortedAttributeList::add(int keywordIndex,CAttribute& attribute)
{
   CSortedAttributeListEntry& entry = getDefinedAttributeListEntry(keywordIndex);

   entry.add(attribute);
}

CSortedAttributeListEntry& CSortedAttributeList::getDefinedAttributeListEntry(int keywordIndex)
{
   CSortedAttributeListEntry* entry = NULL;

   for (POSITION pos = m_attributeList.GetHeadPosition();;)
   {
      if (pos == NULL)
      {
         entry = new CSortedAttributeListEntry(keywordIndex,false);
         m_attributeList.AddTail(entry);

         break;
      }

      POSITION oldPos = pos;
      entry = m_attributeList.GetNext(pos);

      if (entry->getKeywordIndex() == keywordIndex)
      {
         break;
      }
      else if (entry->getKeywordIndex() > keywordIndex)
      {
         entry = new CSortedAttributeListEntry(keywordIndex,false);
         m_attributeList.InsertBefore(oldPos,entry);

         break;
      }
   }

   return *entry;
}




