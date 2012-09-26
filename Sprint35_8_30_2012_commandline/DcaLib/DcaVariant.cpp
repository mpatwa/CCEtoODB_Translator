// $Header: /CAMCAD/DcaLib/DcaVariant.cpp 3     6/30/07 3:01a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaVariant.h"
#include "DcaCamCadData.h"
#include "DcaInsert.h"
#include "DcaAttributes.h"

//_____________________________________________________________________________
CVariantItem::CVariantItem(CCamCadData& camCadData,CString name)
: CEntityWithAttributes(camCadData,false, -1)
{
	this->m_className = "CVarianItem";
	this->SetName(name);
}

CVariantItem::CVariantItem(const CVariantItem& other)
: CEntityWithAttributes(other, true)
{
}

CVariantItem::~CVariantItem() 
{
}

void CVariantItem::setLoaded(CCamCadData& camCadData,bool loadedFlag)
{
   int loadedKeywordIndex = camCadData.getAttributeKeywordIndex(standardAttributeLoaded);
   CString loadedValue(loadedFlag ? "TRUE" : "FALSE");

   camCadData.setAttribute(*(this->getAttributesRef()),loadedKeywordIndex,loadedValue);
}

void CVariantItem::setPartNumber(CCamCadData& camCadData,const CString& partNumber)
{
   int partNumberKeywordIndex = camCadData.getAttributeKeywordIndex(standardAttributePartNumber);

   camCadData.setAttribute(*(this->getAttributesRef()),partNumberKeywordIndex,partNumber);
}

//_____________________________________________________________________________
CVariant::CVariant(CCamCadData& camCadData,CString name)
: CTypedMapStringToPtrContainer<CVariantItem*>()
, m_camCadData(camCadData)
{
	this->m_sName = name;
	this->empty();
}

CVariant::CVariant(const CVariant &other)
: m_camCadData(other.getCamCadData())
{
	*this = other;
}

CVariant::~CVariant()
{
	this->empty();
}

CVariant& CVariant::operator=(const CVariant &variant)
{
	if (&variant != this)
	{
		this->empty();

		POSITION pos = variant.GetStartPosition();
		while (pos != NULL)
		{
			CVariantItem* newItem = new CVariantItem(*variant.GetNext(pos));
			this->AddItem(newItem);
		}
	}

	return *this;
}

CCamCadData& CVariant::getCamCadData() const
{
   return m_camCadData;
}

void CVariant::AddItem(CVariantItem* item)
{
	if (item->GetName().IsEmpty())
		return;

	if (FindItem(item->GetName()) != NULL) 
		return;

	this->SetAt(item->GetName(), item);
}

CVariantItem* CVariant::AddItem(CString refdes)
{
	if (refdes.IsEmpty())
		return NULL;

	// Check to see if item exists
	CVariantItem* item = FindItem(refdes);
	if (item == NULL) 
	{
		item = new CVariantItem(getCamCadData(),refdes);
		this->SetAt(refdes, item);
	}
	
	return item;
}

CVariantItem* CVariant::FindItem(CString refdes)
{
	CVariantItem* item = NULL;
	this->Lookup(refdes, item);
	return item;
}

POSITION CVariant::GetStartPosition() const
{
	return CTypedPtrMap<CMapStringToPtr, CString, CVariantItem*>::GetStartPosition();	
}

CVariantItem* CVariant::GetNext(POSITION& pos) const
{
	CVariantItem* item = NULL;
	CString key = "";
	this->GetNextAssoc(pos, key, item);
	return item;
}

int CVariant::GetCount() const
{
	return CTypedPtrMap<CMapStringToPtr, CString, CVariantItem*>::GetCount();
}

void CVariant::PopulateVariant(CCamCadData& camCadData, FileStruct& pFile)
{
	this->empty();

	POSITION pos = pFile.getBlock()->getHeadDataInsertPosition();
	while (pos != NULL)
	{
		DataStruct* data = pFile.getBlock()->getNextDataInsert(pos);
		InsertStruct* insert = data->getInsert();
		if (insert == NULL || insert->getInsertType() != insertTypePcbComponent)
			continue;

		if (insert->getRefname() == "")   //added for case 1551  .MAN
			continue;

		CVariantItem* item = AddItem(insert->getRefname());
		if (item->getAttributes() == NULL)
			item->getAttributesRef() = new CAttributes(*(data->getAttributes()));
		else
			*item->getAttributesRef() = *data->getAttributes();
	}
}

void CVariant::setLoadedAttributes(CCamCadData& camCadData,bool loadedFlag)
{
   int loadedKeywordIndex = camCadData.getAttributeKeywordIndex(standardAttributeLoaded);
   CString loadedValue(loadedFlag ? "TRUE" : "FALSE");

   for (POSITION pos = GetStartPosition();pos != NULL;)
   {
      CVariantItem* variantItem = GetNext(pos);

      camCadData.setAttribute(*(variantItem->getAttributesRef()),loadedKeywordIndex,loadedValue);
   }
}

//_____________________________________________________________________________
CVariantList::CVariantList(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_pDefaultVariant(NULL)
{
}

CVariantList::~CVariantList()
{
	delete m_pDefaultVariant;
}

CCamCadData& CVariantList::getCamCadData() const
{
   return m_camCadData;
}
   
POSITION CVariantList::GetHeadPosition() const
{
   return m_variantList.GetHeadPosition();
}

CVariant* CVariantList::GetNext(POSITION &pos) const
{
   return m_variantList.GetNext(pos);
}

CVariant* CVariantList::GetHead() const
{
   return m_variantList.GetHead();
}

int CVariantList::GetCount() const
{
   return m_variantList.GetCount();
}

CVariant* CVariantList::AddVariant(CString name)
{
	if (name.IsEmpty())
		return NULL;

	CVariant* variant = FindVariant(name);
	if (variant == NULL)
	{
		variant = new CVariant(getCamCadData(),name);
		m_variantList.AddTail(variant);
	}

	return variant;
}

CVariant* CVariantList::AddVariant(CCamCadData& camCadData, FileStruct& pFile, CString name, bool requireFileShown)
{
	if ((requireFileShown == true) && (pFile.isShown() == false))
		return NULL;

	if (name.IsEmpty())
		return NULL;

	CVariant* variant = FindVariant(name);
	if (variant == NULL)
	{
		variant = new CVariant(getCamCadData(),name);
		variant->PopulateVariant(camCadData, pFile);
		m_variantList.AddTail(variant);
	}

	return variant;
}

CVariant* CVariantList::FindVariant(CString name)
{
	POSITION pos = m_variantList.GetHeadPosition();
	while (pos != NULL)
	{
		CVariant* variant = m_variantList.GetNext(pos);
		if (variant->GetName().CompareNoCase(name) == 0)
			return variant;
	}
	return NULL;
}

CVariant* CVariantList::RemoveVariant(CString name)
{
	CVariant* variant = FindVariant(name);
	m_variantList.RemoveAt(m_variantList.Find(variant));
	
	return variant;
}

void CVariantList::FreeVariantList()
{
   m_variantList.empty();
}

CVariant* CVariantList::CreateDefaultVariant()
{
	delete m_pDefaultVariant;
	m_pDefaultVariant = new CVariant(getCamCadData(),DEFAULT_VARIANT_NAME);

	return m_pDefaultVariant;
}

CVariant* CVariantList::GetDefaultVariant() const
{
	return m_pDefaultVariant;
}

CVariant* CVariantList::RemoveDefaultVariant()
{
	CVariant* defaultVariant = m_pDefaultVariant;
	m_pDefaultVariant = NULL;
	return defaultVariant;
}

void CVariantList::UpdateDefaultVariant(CVariant& variant)
{
	if (m_pDefaultVariant == NULL)
		m_pDefaultVariant = new CVariant(getCamCadData(),DEFAULT_VARIANT_NAME);

	*m_pDefaultVariant = variant;
}

void CVariantList::UpdateDefaultVariant(CCamCadData& camCadData, FileStruct& pFile)
{
	if (m_pDefaultVariant == NULL)
		m_pDefaultVariant = new CVariant(getCamCadData(),DEFAULT_VARIANT_NAME);

	m_pDefaultVariant->PopulateVariant(camCadData, pFile);
}


