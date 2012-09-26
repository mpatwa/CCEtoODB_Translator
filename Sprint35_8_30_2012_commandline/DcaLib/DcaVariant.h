// $Header: /CAMCAD/DcaLib/DcaVariant.h 3     6/30/07 3:01a Kurt Van Ness $

#if !defined(__DcaVariant_h__)
#define __DcaVariant_h__

#pragma once

#define DEFAULT_VARIANT_NAME     "~Default Variant~"
#define BOARD_DATA_VARIANT_NAME  "Board Data Variant"

#include "DcaEntityWithAttributes.h"
#include "DcaContainer.h"

class FileStruct;

//_____________________________________________________________________________
class CVariantItem : public CEntityWithAttributes
{
public:
   CVariantItem(CCamCadData& camCadData,CString name);
   CVariantItem(const CVariantItem& other);
   ~CVariantItem();
   CVariantItem& operator=(const CVariantItem& other);

   void setLoaded(CCamCadData& camCadData,bool loadedFlag);
   void setPartNumber(CCamCadData& camCadData,const CString& partNumber);
};

//_____________________________________________________________________________
class CVariant : public CTypedMapStringToPtrContainer<CVariantItem*>
{
public:
   CVariant(CCamCadData& camCadData,CString name);
   CVariant(const CVariant &other);
   ~CVariant();
   CVariant& operator=(const CVariant &other);

private:
   CCamCadData& m_camCadData;

   CString m_sName;

public:
   CCamCadData& getCamCadData() const;

   CString GetName() const                   { return m_sName; }

   void AddItem(CVariantItem* item);
   CVariantItem* AddItem(CString refdes);
   CVariantItem* FindItem(CString refdes);

   POSITION GetStartPosition() const;
   CVariantItem* GetNext(POSITION& pos) const;
   int GetCount() const;

   void PopulateVariant(CCamCadData& camCadData, FileStruct& pFile);
   void setLoadedAttributes(CCamCadData& camCadData,bool loadedFlag);
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CVariantList
{
friend FileStruct;

private:
   CTypedPtrListContainer<CVariant*> m_variantList;

public:
   CVariantList(CCamCadData& camCadData);

private:
   ~CVariantList();

private:
   CCamCadData& m_camCadData;

   CVariant* m_pDefaultVariant;  // the varaint that is create for default use

public:
   CCamCadData& getCamCadData() const;

   CVariant* CreateDefaultVariant();
   CVariant* GetDefaultVariant() const;
   CVariant* RemoveDefaultVariant();
   void UpdateDefaultVariant(CVariant& variant);
   void UpdateDefaultVariant(CCamCadData& camCadData, FileStruct& pFile);

   // Functions for accessing variants in list
   CVariant* AddVariant(CString name);
   CVariant* AddVariant(CCamCadData& camCadData, FileStruct& pFile, CString name, bool requireFileShown = true);
   CVariant* FindVariant(CString name);
   CVariant* RemoveVariant(CString name);
   
   POSITION GetHeadPosition() const;
   CVariant* GetNext(POSITION &pos) const;
   CVariant* GetHead() const;
   int GetCount() const;

   void FreeVariantList();
   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};


#endif
