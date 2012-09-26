
#include "StdAfx.h"
#include "DcaEntityWithAttributes.h"
#include "DcaEntityNumber.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"

//_____________________________________________________________________________
CEntityWithAttributes::CEntityWithAttributes(CCamCadData& camCadData,bool hasEntityNumber, long entityNumber)
: m_camCadData(camCadData)
{
	m_className = "";
	m_name = "";
	m_attributes = NULL;

	if (hasEntityNumber == true)						//It is going to have an entity number
	{				
		if (entityNumber < 0)									// assign a new one	
			m_entityNumber = getCamCadData().allocateEntityNumber();
		else if (entityNumber > -1)							// assign the existing one
			m_entityNumber = entityNumber;
	}
	else if (hasEntityNumber == false)				// It is not going to have an entity number
	{
		m_entityNumber = -1;
	}
}

CEntityWithAttributes::CEntityWithAttributes(const CEntityWithAttributes &other, bool copyAttributes)
: m_camCadData(other.getCamCadData())
{
	m_className = "";
	m_name = "";
	m_attributes = NULL;

	if (!copyAttributes)
	{
		m_className = other.m_className;
		m_name = other.m_name;

		if (other.m_entityNumber > -1) 
			m_entityNumber = getCamCadData().allocateEntityNumber();
		else
			m_entityNumber = -1;
	}
	else
	{
		*this = other;
	}
}

CEntityWithAttributes::~CEntityWithAttributes()
{
   if (m_attributes != NULL)
   {
		m_attributes->empty();
      delete m_attributes;
      m_attributes = NULL;
   }
}

CEntityWithAttributes& CEntityWithAttributes::operator=(const CEntityWithAttributes &other)
{
	if (&other != this)
	{
		m_className = other.m_className;
		m_name = other.m_name;

		if (m_attributes != NULL)
		{
			m_attributes->empty();
			delete m_attributes;
			m_attributes = NULL;
		}

		if (other.m_attributes != NULL)
		{
			m_attributes = new CAttributes(*other.m_attributes);
		}

		if (other.m_entityNumber > -1) 
			m_entityNumber = getCamCadData().allocateEntityNumber();
		else
			m_entityNumber = -1;
	}

	return *this;
}

CCamCadData& CEntityWithAttributes::getCamCadData() const
{
   return m_camCadData;
}

CAttributes& CEntityWithAttributes::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

POSITION CEntityWithAttributes::GetStartPositionAttrib() const
{
	if (m_attributes != NULL)
		return m_attributes->GetStartPosition(); 
	else
		return NULL;
}

bool CEntityWithAttributes::GetNextAssocAttrib(POSITION &nextPosition, WORD &keyword, CAttribute *&attribute) const
{ 
	if (m_attributes != NULL)
	{
		m_attributes->GetNextAssoc(nextPosition, keyword,attribute); 

		return true;
	}
	else
	{
		keyword = -1;
		attribute = NULL;	

		return false;
	}
}

bool CEntityWithAttributes::setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr)
{
   return (camCadData.setAttribute(attributes(), keywordIndex, valueType, value, updateMethod, attribPtr) ?  true : false); 
}

bool CEntityWithAttributes::LookUpAttrib(WORD keyword, CAttribute *&attribute)
{
	if (m_attributes == NULL)
		return false;

	return m_attributes->Lookup(keyword, attribute)?true:false;
}

void CEntityWithAttributes::RemoveAttrib(WORD keywordIndex)
{
   if (m_attributes != NULL)
   {
      m_attributes->deleteAttribute(keywordIndex);
   }
}

//void CEntityWithAttributes::KeywordMerge(CCamCadData& camCadData, WORD from, WORD to, int method)
//{
//	::KeywordMerge(doc, m_attributes, from, to, method);
//}

//void CEntityWithAttributes::WriteXML(CWriteFormat& writeFormat, CCEtoODBDoc *doc)
//{
//	if (m_attributes != NULL)
//		m_attributes->WriteXML(writeFormat, doc);	
//};


