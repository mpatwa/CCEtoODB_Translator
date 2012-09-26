
#include "StdAfx.h"
#include "DcaNet.h"
#include "DcaAttributes.h"
#include "DcaEntityNumber.h"
#include "DcaCamCadData.h"
#include "DcaTMatrix.h"

//_____________________________________________________________________________
NetStruct::NetStruct(CNetList& parentNetList,const CString& netName,int entityNumber)
: m_parentNetList(parentNetList)
, m_netName(netName)
, m_entityNumber(entityNumber)
, m_attributes(NULL)
, m_compPinList(*this)
, m_flagBits(0)
{
   getCamCadData().allocateEntityNumber(m_entityNumber);
}

NetStruct::~NetStruct()
{
   m_compPinList.empty();

   delete m_attributes;
}

CCamCadData& NetStruct::getCamCadData() const
{
   return m_parentNetList.getCamCadData();
}

CNetList& NetStruct::getNetList() const
{
   return m_parentNetList;
}

const CString& NetStruct::getNetName() const
{
   return m_netName;
}

void NetStruct::setNetName(const CString& netName)
{
   m_netName = netName;
}

long NetStruct::getEntityNumber() const
{
   return m_entityNumber;
}

//void NetStruct::setEntityNumber(long entityNumber)
//{
//   m_entityNumber = entityNumber;
//}

bool NetStruct::isHighlighted() const
{
   return (m_highlighted != 0);
}

void NetStruct::setHighlighted(int flag)
{
   m_highlighted = flag;
}

void NetStruct::setHighlighted(bool flag)
{
   m_highlighted = flag;
}

DbFlag NetStruct::getFlags() const
{
   return m_flagBits;
}

void NetStruct::setFlags(DbFlag flags)
{
   m_flagBits  =  flags;
}

void NetStruct::setFlagBits(DbFlag mask)
{
   m_flagBits |=  mask;
}

void NetStruct::clearFlagBits(DbFlag mask)
{
   m_flagBits &= ~mask;
}

CAttributes*& NetStruct::getAttributesRef()
{
   return m_attributes;
}

CAttributes*& NetStruct::getDefinedAttributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return m_attributes;
}

CAttributes* NetStruct::getAttributes() const
{
   return m_attributes;
}

CAttributes& NetStruct::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

CompPinStruct* NetStruct::addCompPin(const CString compName, const CString pinName,int entityNumber)
{
   CompPinStruct *cp = addCompPin(compName, pinName, 0.0, 0.0, 0.0, 0, -1, visibleBoth,entityNumber);
	if (cp != NULL)
      cp->setPinCoordinatesComplete(false);  // Location sent in above is not real

   return cp;
}

CompPinStruct* NetStruct::addCompPin(const CString compName, const CString pinName, const double x, const double y,
   const double rotationDegrees, const int mirror, const int padstackBlockNumber, const VisibleTag visible,int entityNumber)
{
   if (compName.IsEmpty())     return NULL;
   if (pinName.IsEmpty())      return NULL;

   CompPinStruct* compPinStruct = m_compPinList.addCompPin(compName,pinName,entityNumber);

   //compPinStruct->setEntityNumber(CEntityNumber::allocate());
   //compPinStruct->setRefDes(compName);
   //compPinStruct->setPinName(pinName);
   //compPinStruct->getAttributesRef() = NULL;
   compPinStruct->setOrigin(x, y);
   compPinStruct->setRotationDegrees(rotationDegrees);  
   compPinStruct->setMirror(mirror); 
   compPinStruct->setPinCoordinatesComplete(true);   
   compPinStruct->setVisible(visible);   
   compPinStruct->setPadstackBlockNumber(padstackBlockNumber);

   //m_compPinList.AddTail(cp);

   return compPinStruct;
}

CompPinStruct* NetStruct::getDefinedCompPin(const CString compName, const CString pinName, bool caseSensitive)
{
   CompPinStruct* compPinStruct = findCompPin(compName, pinName, caseSensitive);

   if (compPinStruct == NULL)
   {
      compPinStruct = m_compPinList.addCompPin(compName,pinName);
   }

   return compPinStruct;
}

CompPinStruct* NetStruct::findCompPin(const CString compName, const CString pinName, bool caseSensitive)
{   
   for (POSITION cpPos =getHeadCompPinPosition();cpPos != NULL;)
   {
      CompPinStruct* compPinStruct = getNextCompPin(cpPos);

		if (!caseSensitive)
		{
			if (!compPinStruct->getRefDes().CompareNoCase(compName) && !compPinStruct->getPinName().CompareNoCase(pinName))
         {
				return compPinStruct;
         }
		}
		else
		{
			if (!compPinStruct->getRefDes().Compare(compName) && !compPinStruct->getPinName().Compare(pinName))
         {
				return compPinStruct;
         }
      }
   }

   return NULL;
}

CompPinStruct* NetStruct::findCompPin(CPoint2d location, double tolerance)
{   
   for (POSITION cpPos = getHeadCompPinPosition();cpPos != NULL;)
   {
      CompPinStruct* compPinStruct = getNextCompPin(cpPos);
      CPoint2d cpLocation = compPinStruct->getOrigin();

      if (location.distance(cpLocation) <= tolerance)
      {
         return compPinStruct;
      }
   }

   return NULL;
}

CompPinStruct* NetStruct::getHeadCompPin() const           
{ 
   CompPinStruct* headCompPin = NULL;

	if (m_compPinList.getCount() > 0)
   {
		headCompPin = m_compPinList.getHead(); 
   }

	return headCompPin;
}

POSITION NetStruct::getHeadCompPinPosition()
{
   return m_compPinList.getHeadPosition();
}

CompPinStruct* NetStruct::getNextCompPin(POSITION& pos)
{
   return m_compPinList.getNext(pos);
}

int NetStruct::getCompPinCount()
{
   return m_compPinList.getCount();
}

CCompPinList& NetStruct::getCompPinList()
{
   return m_compPinList;
}

const CCompPinList& NetStruct::getCompPinList() const
{
   return m_compPinList;
}

void NetStruct::takeCompPins(NetStruct& otherNet)
{
   if (&otherNet != this)
   {
      for (POSITION pos = otherNet.getCompPinList().getHeadPosition();pos != NULL;)
      {
         POSITION oldPos = pos;

         CompPinStruct* otherCompPinStruct = otherNet.getCompPinList().getNext(pos);
         otherNet.getCompPinList().removeAt(oldPos);

         CompPinStruct* compPinStruct = getCompPinList().addCompPin(otherCompPinStruct->getRefDes(),otherCompPinStruct->getPinName(),otherCompPinStruct->getEntityNumber());

         delete otherCompPinStruct;
      }
   }
}

bool NetStruct::setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr)
{
   return (camCadData.setAttribute(attributes(), keywordIndex, valueType, value, updateMethod, attribPtr) ?  true : false); 
}

bool NetStruct::lookUpAttrib(WORD keyword, CAttribute *&attribute) const
{
   if (m_attributes == NULL)
      return false;

   return (m_attributes->Lookup(keyword, attribute) ? true : false);
}

bool NetStruct::IsDiePin(CString compName, CString pinName)
{
   CompPinStruct* comppin = findCompPin(compName, pinName, true);      
   return (comppin)?comppin->IsDiePin(getCamCadData()):false;
}

bool NetStruct::IsSingleDiePinNet()
{
   if((this->getFlags() & (NETFLAG_UNUSEDNET | NETFLAG_SINGLEPINNET))
      && this->getCompPinList().getCount())
   {
      CompPinStruct *comppin = this->getCompPinList().getHead();
      if(comppin)
         return comppin->IsDiePin(getCamCadData());
   }

   return false;
}

//_____________________________________________________________________________
CCompPinMap::CCompPinMap()
{
   m_pinMap.InitHashTable(nextPrime2n(10000));
}

void CCompPinMap::addPin(CompPinStruct& compPinStruct)
{
   m_pinMap.SetAt(compPinStruct.getPinRef(),&compPinStruct);
}

void CCompPinMap::addPins(const CCompPinList& compPinList)
{
   for (POSITION pos = compPinList.getHeadPosition();pos != NULL;)
   {
      CompPinStruct* compPinStruct = compPinList.getNext(pos);

      addPin(*compPinStruct);
   }
}

void CCompPinMap::removePin(const CompPinStruct& compPinStruct)
{
   CompPinStruct* mappedPin;
   CString pinRef = compPinStruct.getPinRef();

   if (m_pinMap.Lookup(pinRef,mappedPin))
   {
      if (&compPinStruct == mappedPin)
      {
         m_pinMap.RemoveKey(pinRef);
      }
   }
}

void CCompPinMap::removePins(const CCompPinList& compPinList)
{
   for (POSITION pos = compPinList.getHeadPosition();pos != NULL;)
   {
      CompPinStruct* compPinStruct = compPinList.getNext(pos);

      removePin(*compPinStruct);
   }
}

CompPinStruct* CCompPinMap::getCompPin(const CString& pinRef)
{
   CompPinStruct* compPinStruct;

   if (! m_pinMap.Lookup(pinRef,compPinStruct))
   {
      compPinStruct = NULL;
   }

   return compPinStruct;
}

//_____________________________________________________________________________
CNetList::CNetList(CCamCadData& parentCamCadData)
: m_parentCamCadData(parentCamCadData)
{
   m_netMap.InitHashTable(nextPrime2n(1000));
}

CNetList::~CNetList()
{
   m_netList.empty();
}

CCamCadData& CNetList::getCamCadData() const
{
   return m_parentCamCadData;
}

CCompPinMap& CNetList::getCompPinMap()
{
   return m_compPinMap;
}

NetStruct* CNetList::addNet(const CString& netName,int entityNumber)
{
   NetStruct* net = getNet(netName);

   if (net == NULL)
   {
      net = new NetStruct(*this,netName,entityNumber);

      m_netList.AddTail(net);
      m_netMap.SetAt(net->getNetName(),net);
   }

   return net;
}

NetStruct* CNetList::getNet(const CString& netName)
{
   NetStruct* net = NULL;

   if (! m_netMap.Lookup(netName,net))
   {
      net = NULL;
   }

   return net;
}

NetStruct& CNetList::getUnusedPinsNet()
{
   NetStruct *n = addNet(getUnusedPinsNetName());
   n->setFlagBits(NETFLAG_UNUSEDNET);
   return *n;
}

NetStruct& CNetList::getNoNetPinsNet()
{
   return *(addNet(getNoNetPinsNetName()));
}

POSITION CNetList::getHeadPosition() const
{
   return m_netList.GetHeadPosition();
}

NetStruct* CNetList::getNext(POSITION& pos) const
{
   return m_netList.GetNext(pos);
}

NetStruct* CNetList::getAt(POSITION pos) const
{
   return m_netList.GetAt(pos);
}

int CNetList::getCount() const
{
   return m_netList.GetCount();
}

bool CNetList::isEmpty() const
{
   return (getCount() == 0);
}

void CNetList::deleteAt(POSITION pos)
{
   NetStruct* net = m_netList.GetAt(pos);
   m_netList.RemoveAt(pos);

   m_netMap.RemoveKey(net->getNetName());
   
   m_compPinMap.removePins(net->getCompPinList());

   delete net;
}

void CNetList::deleteNet(const CString& netName)
{
   NetStruct* net = getNet(netName);

   if (net != NULL)
   {
      POSITION pos = m_netList.Find(net);

      if (pos != NULL)
      {
         deleteAt(pos);
      }
   }
}

void CNetList::empty()
{
   m_netList.empty();
   m_netMap.RemoveAll();
}
void CNetList::updateNetName(NetStruct *net, CString newName)
{
   // When net is added to NetList it gets put into a map.
   // NetStruct::setNetName() will update name in the list, but not in the map.
   // This is here to update the map.
   // For convenience it updates the name in NetStruct too (also to ensure consistency).
   // So anyplace that is calling NetStruct::setNetName() after doing some AddNet() that puts
   // it in the NetList ought to call this directly instead.
   // It is REQUIRED that it at least call it in addition.

   if (net != NULL)  // and we're doing it anyway if newName is blank.
   {
      m_netMap.RemoveKey(net->getNetName()); // remove existing name from map
      net->setNetName(newName);  // update name directly in NetStruct
      m_netMap.SetAt(newName, net);
   }
}

CompPinStruct* CNetList::getCompPin(const CString& pinRef)
{
   CompPinStruct* compPin = m_compPinMap.getCompPin(pinRef);

   return compPin;
}

CompPinStruct* CNetList::getCompPin(const CString& refDes,const CString& pinName)
{
   CString pinRef = CompPinStruct::getPinRef(refDes,pinName);

   return getCompPin(pinRef);
}

CompPinStruct& CNetList::getDefinedCompPin(const CString& refDes,const CString& pinName)
{
   CString pinRef = CompPinStruct::getPinRef(refDes,pinName);

   CompPinStruct* compPin = m_compPinMap.getCompPin(pinRef);

   if (compPin == NULL)
   {
      compPin = getUnusedPinsNet().addCompPin(refDes,pinName);
   }

   return *compPin;
}

void CNetList::Scale(double factor)
{
	CTMatrix mat;
	mat.scale(factor, factor);
	
	for (POSITION netPos = m_netList.GetHeadPosition();netPos != NULL;)
	{
		NetStruct* net = m_netList.GetNext(netPos);

		if (net == NULL)
			continue;

		if (net->getAttributes() != NULL)
      {
			net->getAttributes()->transform(mat);
      }

		net->getCompPinList().Scale(factor);
	}
}

CString CNetList::getUnusedPinsNetName()
{
   return CString("~Unused_Pins~");
}

CString CNetList::getNoNetPinsNetName()
{
   return CString("~No_Net_Pins~");
}
