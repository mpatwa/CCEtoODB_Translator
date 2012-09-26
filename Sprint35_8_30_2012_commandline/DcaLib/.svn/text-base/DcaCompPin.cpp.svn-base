// $Header: /CAMCAD/DcaLib/DcaCompPin.cpp 8     6/30/07 2:57a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaCompPin.h"
#include "DcaPoint2d.h"
#include "DcaData.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"
#include "DcaPoint.h"
#include "DcaTMatrix.h"

//_____________________________________________________________________________
CompPinStruct::CompPinStruct(CCompPinList& parentCompPinList,const CString& refDes,const CString& pinName,int entityNumber)
: m_parentCompPinList(parentCompPinList)
, m_refDes(refDes)
, m_pinName(pinName)
, m_entityNumber(entityNumber)
, m_attributes(NULL)
, m_x(0.)
, m_y(0.)
, m_rotationRadians(0.)
, m_mirror(0)
, m_pinCoordinatesComplete(0)
, m_visible(visibleNone)
, m_padstackBlockNumber(-1)
{
   getCamCadData().allocateEntityNumber(m_entityNumber);

   addToMap();
}

CompPinStruct::~CompPinStruct()
{
   //removeFromMap();

   delete m_attributes;
}

CCamCadData& CompPinStruct::getCamCadData() const
{
   return m_parentCompPinList.getCamCadData();
}

CNetList& CompPinStruct::getNetList() const
{
   return m_parentCompPinList.getNetList();
}

void CompPinStruct::setRefDes(const CString& refDes)
{
   if (m_refDes != refDes)
   {
      removeFromMap();

      m_refDes = refDes;

      addToMap();
   }
}

void CompPinStruct::setPinName(const CString& pinName)
{
   if (m_pinName != pinName)
   {
      removeFromMap();

      m_pinName = pinName;

      addToMap();
   }
}

double CompPinStruct::getRotationDegrees() const
{
   return radiansToDegrees(m_rotationRadians);
}

void CompPinStruct::setRotationDegrees(DbUnit degrees)
{
   m_rotationRadians =  (DbUnit)degreesToRadians(degrees);
}

void CompPinStruct::setRotationDegrees(double degrees)
{
   m_rotationRadians =  (DbUnit)degreesToRadians(degrees);
}

void CompPinStruct::incRotationDegrees(double degrees)
{
   m_rotationRadians += (DbUnit)degreesToRadians(degrees);
}

CPoint2d CompPinStruct::getOrigin() const
{
   return CPoint2d(m_x,m_y);
}

void CompPinStruct::setOrigin(const CPoint2d& origin)
{
   m_x = (DbUnit)origin.x;
   m_y = (DbUnit)origin.y;
}

void CompPinStruct::setOrigin(const PointStruct& origin)
{
   m_x = origin.x;
   m_y = origin.y;
}

CString CompPinStruct::getSortableReference(const CString& reference)
{
   const char* p = reference;
   CString alpha;
   CString numeric;
   CString suffix;

   while (*p != '\0')
   {
      if (isdigit(*p))
      {
         break;
      }

      alpha += *(p++);
   }

   while (*p != '\0')
   {
      if (!isdigit(*p))
      {
         break;
      }

      numeric += *(p++);
   }

   suffix = p;

   CString leader("00000000");
   CString trailer("        ");

   alpha   += trailer;
   numeric  = leader + numeric;
   suffix  += trailer;

   CString sortableReference = alpha.Left(8) + numeric.Right(8) + suffix.Left(8);

   // Case dts0100408077 - The general construction of sortableRefDes as above causes 
   // collisions for refnames like C3 and C03, they both result in the same value.
   // Some features build component maps based on the sortableRefDes, and they are dropping entities when
   // such name collisions occur. So here we tack the original refname onto the end, thus making them
   // unique again. This keeps intact the basic overall "numerical" sorting portion of the game.
   sortableReference += "|" + reference;

   return sortableReference;
}

CString CompPinStruct::getPinRef(const CString& refDes,const CString& pinName,char delim)
{
   CString pinRef = refDes + delim + pinName;

   return pinRef;
}

CString CompPinStruct::getSortableRefDes() const
{
   return getSortableReference(m_refDes);
}

CString CompPinStruct::getSortablePinName() const
{
   return getSortableReference(m_pinName);
}

CString CompPinStruct::getSortablePinRef() const
{
   CString sortablePinRef = getSortableRefDes() + "." + getSortablePinName() ;

   return sortablePinRef;
}

bool CompPinStruct::lookUpAttrib(WORD keyword, CAttribute*& attribute) const
{
   if (m_attributes == NULL)
      return false;

   return m_attributes->Lookup(keyword, attribute)?true:false;
}

void CompPinStruct::setVisible(VisibleTag visible)
{
   m_visible = visible;
}

void CompPinStruct::setVisible(unsigned int visible)
{
   m_visible = (VisibleTag)(visible & visibleBoth);
}

void CompPinStruct::setVisibleBits(unsigned int mask)
{
   m_visible = (VisibleTag)((m_visible | mask) & visibleBoth);
}

void CompPinStruct::clearVisibleBits(unsigned int mask)
{
   m_visible = (VisibleTag)((m_visible & ~mask) & visibleBoth);
}

bool CompPinStruct::setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr)
{
   return (camCadData.setAttribute(attributes(), keywordIndex, valueType, value, updateMethod, attribPtr) ?  true : false); 
}

CAttributes*& CompPinStruct::getDefinedAttributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return m_attributes;
}

CAttributes& CompPinStruct::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

void CompPinStruct::addToMap()
{
   getNetList().getCompPinMap().addPin(*this);
}

void CompPinStruct::removeFromMap() const
{
   getNetList().getCompPinMap().removePin(*this);
}

bool CompPinStruct::IsDiePin(CCamCadData &camCadData)
{
   BlockStruct *block = camCadData.getBlockAt( this->getPadstackBlockNumber() );
   if(block && block->getBlockType() == blockTypeDiePin)
        return true;

    return false;
}

//_____________________________________________________________________________
CCompPinList::CCompPinList(NetStruct& parentNet)
: m_parentNet(parentNet)
{
}

CCompPinList::~CCompPinList()
{
   m_compPinList.empty();
}

void CCompPinList::empty()
{
   m_compPinList.empty();
}

CCamCadData& CCompPinList::getCamCadData() const
{
   return m_parentNet.getCamCadData();
}

CNetList& CCompPinList::getNetList() const
{
   return m_parentNet.getNetList();
}

// duplicate pins allowed
CompPinStruct* CCompPinList::addCompPin(const CString& refDes,const CString& pinName,int entityNumber)
{
   CompPinStruct* compPinStruct = new CompPinStruct(*this,refDes,pinName,entityNumber);

   m_compPinList.AddTail(compPinStruct);

   return compPinStruct;
}

void CCompPinList::addHead(CompPinStruct*& compPin)
{
   m_compPinList.AddHead(compPin);
}

void CCompPinList::addTail(CompPinStruct*& compPin)
{
   m_compPinList.AddTail(compPin);
}

POSITION CCompPinList::getHeadPosition() const
{
   return m_compPinList.GetHeadPosition();
}

POSITION CCompPinList::find(CompPinStruct*& compPin) const
{
   return m_compPinList.Find(compPin);
}

CompPinStruct* CCompPinList::getHead() const
{
   return m_compPinList.GetHead();
}

CompPinStruct* CCompPinList::getTail() const
{
   return m_compPinList.GetTail();
}

CompPinStruct* CCompPinList::getNext(POSITION& pos) const
{
   return m_compPinList.GetNext(pos);
}

CompPinStruct* CCompPinList::getAt(POSITION pos) const
{
   return m_compPinList.GetAt(pos);
}

int CCompPinList::getCount() const
{
   return m_compPinList.GetCount();
}

bool CCompPinList::isEmpty() const
{
   return (m_compPinList.GetCount() == 0);
}

void CCompPinList::removeAt(POSITION pos)
{
   m_compPinList.RemoveAt(pos);
}

void CCompPinList::deleteCompPin(CompPinStruct*& compPin)
{
   POSITION pos = m_compPinList.Find(compPin);

   if (pos != NULL)
   {
      compPin = NULL;
   }
}

void CCompPinList::deleteAt(POSITION pos)
{
   if (pos != NULL)
   {
      CompPinStruct* compPinStruct = m_compPinList.GetAt(pos);
      m_compPinList.RemoveAt(pos);

      delete compPinStruct;
   }
}

void CCompPinList::Scale(double factor)
{
	CTMatrix mat;
	mat.scale(factor, factor);
	
	for (POSITION cpPos = m_compPinList.GetHeadPosition(); cpPos != NULL;)
	{
		CompPinStruct* compPinStruct = m_compPinList.GetNext(cpPos);

		if (compPinStruct == NULL)
			continue;

		if (compPinStruct->getAttributes() != NULL)
      {
			compPinStruct->getAttributes()->transform(mat);
      }

		if (compPinStruct->getPinCoordinatesComplete())
		{
			compPinStruct->setOriginX(compPinStruct->getOriginX() * factor);
			compPinStruct->setOriginY(compPinStruct->getOriginY() * factor);
		}
	}
}
