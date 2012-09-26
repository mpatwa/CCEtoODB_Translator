// $Header: /CAMCAD/5.0/RealPart.cpp 57    6/17/07 8:53p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2003. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
//#include "xform.h"
#include "realpart.h"
#include "extents.h"
#include "apertur2.h"
#include "xform.h"
#include "polylib.h"

CString SwapSpecialCharacters(CString string);

/******************************************************************************
* CComponentRealpartLink::CComponentRealpartLink
*/
CComponentRealpartLink::CComponentRealpartLink(CCEtoODBDoc &doc)
	: CObject()
	, m_doc(doc)
	, m_pComponentData(NULL)
	, m_pRealpartData(NULL)
{
	m_loadedKW = doc.RegisterKeyWord(ATT_LOADED, 0, valueTypeString);
}

CComponentRealpartLink::~CComponentRealpartLink()
{
}

bool CComponentRealpartLink::IsComponentLoaded() const
{
	if (m_pComponentData == NULL)
		return false;
	
	Attrib *attr = NULL;
	if (!m_pComponentData->lookUpAttrib(m_loadedKW, attr))
		return false;

	return (!STRICMP(get_attvalue_string(&m_doc, attr), "True"))?true:false;
}

bool CComponentRealpartLink::HasMatch() const
{
	return (m_pComponentData != NULL && m_pRealpartData != NULL);
}

bool CComponentRealpartLink::AddComponent(DataStruct *data)
{
	if (data->getDataType() != dataTypeInsert)
		return false;

	InsertStruct *insert = data->getInsert();
	if (insert->getInsertType() != insertTypePcbComponent)
		return false;
	CString compRefname = insert->getRefname();

	// if there is a realpart, we need to make sure the ref names are the same
	if (m_pRealpartData != NULL)
	{
		CString rpRefname = m_pRealpartData->getInsert()->getRefname();
		if (compRefname.MakeUpper() != rpRefname.MakeUpper())
			return false;
	}

	m_pComponentData = data;
	return true;
}

bool CComponentRealpartLink::AddRealpart(DataStruct *data)
{
	if (data->getDataType() != dataTypeInsert)
		return false;

	InsertStruct *insert = data->getInsert();
	if (insert->getInsertType() != insertTypeRealPart)
		return false;
	CString rpRefname = insert->getRefname();

	// if there is a component, we need to make sure the ref names are the same
	if (m_pComponentData != NULL)
	{
		CString compRefname = m_pComponentData->getInsert()->getRefname();
		if (compRefname.MakeUpper() != rpRefname.MakeUpper())
			return false;
	}

	m_pRealpartData = data;
	return true;
}

bool CComponentRealpartLink::ShowRealpart()
{
	if (m_pRealpartData == NULL)
		return false;

	m_pRealpartData->setHidden(false);
	return true;
}

bool CComponentRealpartLink::HideRealpart()
{
	if (m_pRealpartData == NULL)
		return false;

	m_pRealpartData->setHidden(true);
	return true;
}


/******************************************************************************
* CComponentRealpartLinkMap::CComponentRealpartLinkMap
*/
CComponentRealpartLinkMap::CComponentRealpartLinkMap(CCEtoODBDoc &doc, FileStruct &file)
: m_camCadDoc(doc)
, m_fileStruct(file)
{
	WORD m_loadedKW = doc.RegisterKeyWord(ATT_LOADED, 0, valueTypeString);

	for (POSITION pos=file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = file.getBlock()->getAtData(pos);
		InsertStruct *insert = data->getInsert();
		CString refName = insert->getRefname();

		if (insert->getInsertType() != insertTypePcbComponent && insert->getInsertType() != insertTypeRealPart)
			continue;

		CComponentRealpartLink *crpLink = NULL;
		if (!Lookup(refName, crpLink))
		{
			crpLink = new CComponentRealpartLink(doc);
			SetAt(refName, crpLink);
		}

		if (insert->getInsertType() == insertTypePcbComponent)
			crpLink->AddComponent(data);
		else if (insert->getInsertType() == insertTypeRealPart)
			crpLink->AddRealpart(data);
	}	
}

void CComponentRealpartLinkMap::HideRealpartsForUnloadedComponents()
{
	if (IsEmpty())
		return;
	
	for (POSITION pos = GetStartPosition();pos != NULL;)
	{
		CString refName;
		CComponentRealpartLink* crpLink = NULL;
		GetNextAssoc(pos, refName, crpLink);

		if (!crpLink->HasMatch())
			continue;

		if (crpLink->IsComponentLoaded())
			crpLink->ShowRealpart();
		else
			crpLink->HideRealpart();
	}
}


bool HasRealPart(FileStruct &file)
{
	for (POSITION pos=file.getBlock()->getHeadDataInsertPosition(); pos!=NULL; file.getBlock()->getNextDataInsert(pos))
	{
		DataStruct *data = file.getBlock()->getAtData(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() == insertTypeRealPart)
			return true;
	}

	return false;
}


/******************************************************************************
/******************************************************************************
* CMapStringToBlock::Lookup
*/
BlockStruct *CMapStringToBlock::Lookup(LPCTSTR key)
{
   if (!strlen(key))
      return NULL;

   BlockStruct *block;

   if (CTypedPtrMap<CMapStringToPtr, CString, BlockStruct*>::Lookup(key, block))
      return block;

   return NULL;
}

///******************************************************************************
//* CMapStringToBlock::Lookup
//*/
//BOOL CMapStringToBlock::Lookup(LPCTSTR key, BlockStruct *&block) const
//{
// return CMapStringToPtr::Lookup(key, (VOID*&)block);
//}
//
///******************************************************************************
//* CMapStringToBlock::SetAt
//*/
//void CMapStringToBlock::SetAt(LPCTSTR key, BlockStruct *block)
//{
// CMapStringToPtr::SetAt(key, (VOID*)block);
//}
//
///******************************************************************************
//* CMapStringToBlock::GetNextAssoc
//*/
//void CMapStringToBlock::GetNextAssoc(POSITION &rNextPosition, CString &rKey, BlockStruct *&block) const
//{
// CMapStringToPtr::GetNextAssoc(rNextPosition, rKey, (VOID*&)block);
//}


/******************************************************************************
/******************************************************************************
* CMapStringToData::Lookup
*/
DataStruct *CMapStringToData::Lookup(LPCTSTR key)
{
   if (!strlen(key))
      return NULL;

   DataStruct *data;

   if (CTypedPtrMap<CMapStringToPtr, CString, DataStruct*>::Lookup(key, data))
      return data;

   return NULL;
}

///******************************************************************************
//* CMapStringToData::Lookup
//*/
//BOOL CMapStringToData::Lookup(LPCTSTR key, DataStruct *&data) const
//{
// return CMapStringToPtr::Lookup(key, (VOID*&)data);
//}
//
///******************************************************************************
//* CMapStringToData::SetAt
//*/
//void CMapStringToData::SetAt(LPCTSTR key, DataStruct *data)
//{
// CMapStringToPtr::SetAt(key, (VOID*)data);
//}
//
///******************************************************************************
//* CMapStringToData::GetNextAssoc
//*/
//void CMapStringToData::GetNextAssoc(POSITION &rNextPosition, CString &rKey, DataStruct *&data) const
//{
// CMapStringToPtr::GetNextAssoc(rNextPosition, rKey, (VOID*&)data);
//}


/******************************************************************************
/******************************************************************************
* CMapStringToLibrary::CMapStringToLibrary
*/
CMapStringToLibrary::CMapStringToLibrary(CCEtoODBDoc *doc) 
   :CTypedPtrMap<CMapStringToPtr, CString, SLibrary*>()
{
   int libNum = 0;
   int rpKw = doc->IsKeyWord(REALPART_GUID, 0);

   if (!rpKw)
      return;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() != BLOCKTYPE_LIBRARY)
         continue;

      BlockStruct *block = file->getBlock();

      Attrib *attrib = NULL;
      block->getAttributesRef()->Lookup(rpKw, attrib);

      SLibrary *lib = new SLibrary();
      lib->file = file;
      lib->number = libNum++;
      lib->GUID = get_attvalue_string(doc, attrib);

      this->SetAt(lib->GUID, lib);
   }
}

/******************************************************************************
* CMapStringToLibrary::~CMapStringToLibrary
*/
CMapStringToLibrary::~CMapStringToLibrary()
{
   POSITION pos = this->GetStartPosition();
   while (pos)
   {
      CString key;
      SLibrary *lib = NULL;
      this->GetNextAssoc(pos, key, lib);

      delete lib;
   }
   this->RemoveAll();
}

/******************************************************************************
* CMapStringToLibrary::Lookup
*/
SLibrary *CMapStringToLibrary::Lookup(LPCTSTR key)
{
   if (!strlen(key))
      return NULL;

   SLibrary *lib;

   if (CTypedPtrMap<CMapStringToPtr, CString, SLibrary*>::Lookup(key, lib))
      return lib;

   return NULL;
}

///******************************************************************************
//* CMapStringToLibrary::Lookup
//*/
//BOOL CMapStringToLibrary::Lookup(LPCTSTR key, SLibrary *&library) const
//{
// return CMapStringToPtr::Lookup(key, (VOID*&)library);
//}
//
///******************************************************************************
//* CMapStringToLibrary::SetAt
//*/
//void CMapStringToLibrary::SetAt(LPCTSTR key, SLibrary *library)
//{
// CMapStringToPtr::SetAt(key, (VOID*)library);
//}
//
///******************************************************************************
//* CMapStringToLibrary::GetNextAssoc
//*/
//void CMapStringToLibrary::GetNextAssoc(POSITION &rNextPosition, CString &rKey, SLibrary *&library) const
//{
// CMapStringToPtr::GetNextAssoc(rNextPosition, rKey, (VOID*&)library);
//}


/******************************************************************************
/******************************************************************************
* CMapWordToLibrary::CMapWordToLibrary
*/
CMapWordToLibrary::CMapWordToLibrary(CCEtoODBDoc *doc) 
   :CMapWordToPtr()
{
   int libNum = 0;
   int rpKw = doc->IsKeyWord("RealPart_GUID", 0);

   if (!rpKw)
      return;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() != BLOCKTYPE_LIBRARY)
         continue;

      BlockStruct *block = file->getBlock();

      Attrib *attrib = NULL;
      CAttributes *attributes = block->getAttributesRef();

      if (attributes != NULL)
      {
         if (attributes->Lookup(rpKw, attrib))
			{
				SLibrary *lib = new SLibrary();
				lib->file = file;
				lib->number = libNum++;
				lib->GUID = get_attvalue_string(doc, attrib);

				this->SetAt(file->getFileNumber(), lib);
			}
      }
   }
}

/******************************************************************************
* CMapWordToLibrary::~CMapWordToLibrary
*/
CMapWordToLibrary::~CMapWordToLibrary()
{
   POSITION pos = this->GetStartPosition();
   while (pos)
   {
      WORD key;
      SLibrary *lib = NULL;
      this->GetNextAssoc(pos, key, lib);

      delete lib;
   }
   this->RemoveAll();
}

/******************************************************************************
* CMapStringToLibrary::Lookup
*/
SLibrary *CMapWordToLibrary::Lookup(WORD key)
{
   if (key < 0)
      return NULL;

   SLibrary *lib;

   if (Lookup(key, lib))
      return lib;

   return NULL;
}

/******************************************************************************
* CMapWordToLibrary::Lookup
*/
BOOL CMapWordToLibrary::Lookup(WORD key, SLibrary *&library) const
{
   return CMapWordToPtr::Lookup(key, (VOID*&)library);
}

/******************************************************************************
* CMapWordToLibrary::SetAt
*/
void CMapWordToLibrary::SetAt(WORD key, SLibrary *library)
{
   CMapWordToPtr::SetAt(key, (VOID*)library);
}

/******************************************************************************
* CMapWordToLibrary::GetNextAssoc
*/
void CMapWordToLibrary::GetNextAssoc(POSITION &rNextPosition, WORD &rKey, SLibrary *&library) const
{
   CMapWordToPtr::GetNextAssoc(rNextPosition, rKey, (VOID*&)library);
}


/******************************************************************************
/******************************************************************************
* CRealPartWrite::CRealPartWrite
*/
CRealPartWrite::CRealPartWrite(CCEtoODBDoc *ccDoc)
{
   indent = 0;
   doc = ccDoc;

   mFile = new CMemFile(MEMBUF);
   libMap = new CMapWordToLibrary(doc);

   doc->OnRegen();
}

/******************************************************************************
* CRealPartWrite::~CRealPartWrite
*/
CRealPartWrite::~CRealPartWrite()
{
   delete libMap;
   delete mFile;
}

/******************************************************************************
* CRealPartWrite::GatherRPData
*/
int CRealPartWrite::GatherRPData()
{
   CString buf;

   buf.Format("<%s version=\"1.0.0.0\">\n", RP_RPXML); 
   mFile->Write(buf, buf.GetLength());

   increaseIndent();

   writeLibraryList();

   // Format XML string
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      POSITION curFilePos = filePos;
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden()) continue;

      if (file->getBlockType() != BLOCKTYPE_PCB) continue;

      // find all the packages and landpatterns
      getAllData(file->getBlock());

      // write the data out to mFile
      writeFile(file, curFilePos);
   } // END while (filePos) ...

   decreaseIndent();

   buf.Format("</%s>\n", RP_RPXML);
   mFile->Write(buf, buf.GetLength());

   return 0;
}

/******************************************************************************
* CRealPartWrite::GetLength
*/
ULONGLONG CRealPartWrite::GetLength()
{
   return mFile->GetLength();
}

/******************************************************************************
* CRealPartWrite::Detach
*/
BYTE *CRealPartWrite::Detach()
{
   return mFile->Detach();
}

/******************************************************************************
* CRealPartWrite::increaseIndent
*/
int CRealPartWrite::increaseIndent()
{
   // no limit as to how much to indent
   indent += INDENT;

   return indent;
}

/******************************************************************************
* CRealPartWrite::decreaseIndent
*/
int CRealPartWrite::decreaseIndent()
{
   if (indent < INDENT)
      indent = 0;
   else
      indent -= INDENT;

   return indent;
}

/******************************************************************************
* CRealPartWrite::getBottomStackNum
*/
int CRealPartWrite::getBottomStackNum()
{
   int botStackNum = 0;
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (!layer) continue;

      if (layer->getElectricalStackNumber() > botStackNum)
         botStackNum = layer->getElectricalStackNumber();
   }

   return botStackNum;
}

/******************************************************************************
* CRealPartWrite::getRPPad
*/
CPolyList *CRealPartWrite::getRPPad(DataStruct *padstackInsert)
{
   if (padstackInsert->getDataType() != dataTypeInsert)
		return NULL;


	BlockStruct *padStackBlock = NULL;
	if (padstackInsert->getInsert()->getBlockNumber() >= 0 && padstackInsert->getInsert()->getBlockNumber() < doc->getMaxBlockIndex())
		padStackBlock = doc->getBlockAt(padstackInsert->getInsert()->getBlockNumber());

	if (padStackBlock == NULL)
		return NULL;


	int botStackNum = getBottomStackNum();
	DataStruct *topPadInsert = NULL, *botPadInsert = NULL;

	POSITION dataPos = padStackBlock->getDataList().GetHeadPosition();
	while (dataPos)
	{
		DataStruct *data = padStackBlock->getDataList().GetNext(dataPos);

		if (data->getDataType() != T_INSERT)
			continue;

		if (data->getLayerIndex() < 0)
			continue;

		LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
		if (!layer)
			continue;

		if (layer->getLayerType() == LAYTYPE_PAD_TOP || layer->getElectricalStackNumber() == 1)
			topPadInsert = data;
		if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM || layer->getElectricalStackNumber() == botStackNum)
			botPadInsert = data;
	}

	DataStruct *padInsert = NULL;
	if (topPadInsert)
		padInsert = topPadInsert;
	else if (botPadInsert)
		padInsert = botPadInsert;

	// if found a pad, return poly of pad
	if (padInsert)
	{
		BlockStruct *padGeom = doc->getBlockAt(padInsert->getInsert()->getBlockNumber());
		CPolyList *polylist = padGeom->getAperturePolys(doc->getCamCadData());

		TransformPolyList(polylist, padInsert->getInsert()->getOriginX(), padInsert->getInsert()->getOriginY(), 
				padInsert->getInsert()->getAngle(), padInsert->getInsert()->getMirrorFlags(), padInsert->getInsert()->getScale());

		TransformPolyList(polylist, padstackInsert->getInsert()->getOriginX(), padstackInsert->getInsert()->getOriginY(), 
				padstackInsert->getInsert()->getAngle(), padstackInsert->getInsert()->getMirrorFlags(), padstackInsert->getInsert()->getScale());

		return polylist;
	}


	// otherwise, return Transformed Extents poly
	double xmin, xmax, ymin, ymax;

   if (padStackBlock->extentIsValid())
   {
      xmin = padStackBlock->getXmin();
      xmax = padStackBlock->getXmax();
      ymin = padStackBlock->getYmin();
      ymax = padStackBlock->getYmax();
      
      DTransform xForm(
			padstackInsert->getInsert()->getOriginX(), 
			padstackInsert->getInsert()->getOriginY(), 
			padstackInsert->getInsert()->getScale(), 
			padstackInsert->getInsert()->getAngle(), 
			padstackInsert->getInsert()->getMirrorFlags());
      Point2 pnt;

      pnt.x = xmin;
      pnt.y = ymin;
      xForm.TransformPoint(&pnt);
      xmin = pnt.x;
      ymin = pnt.y;

      pnt.x = xmax;
      pnt.y = ymax;
      xForm.TransformPoint(&pnt);
      xmax = pnt.x;
      ymax = pnt.y;
   }
   else
   {
      block_extents(
			doc, &xmin, &xmax, &ymin, &ymax, 
			&padStackBlock->getDataList(),
         padstackInsert->getInsert()->getOriginX(), 
			padstackInsert->getInsert()->getOriginY(),
         padstackInsert->getInsert()->getAngle(), 
			padstackInsert->getInsert()->getMirrorFlags(), 
			padstackInsert->getInsert()->getScale(),
         padstackInsert->getLayerIndex(), 
			FALSE);
   }

   // Create the extents poly
   CPolyList *polylist = new CPolyList();
   CPoly *poly = new CPoly();

   poly->getPntList().AddTail(new CPnt((DbUnit)xmin, (DbUnit)ymin));
   poly->getPntList().AddTail(new CPnt((DbUnit)xmin, (DbUnit)ymax));
   poly->getPntList().AddTail(new CPnt((DbUnit)xmax, (DbUnit)ymax));
   poly->getPntList().AddTail(new CPnt((DbUnit)xmax, (DbUnit)ymin));
   poly->getPntList().AddTail(new CPnt((DbUnit)xmin, (DbUnit)ymin));
   polylist->AddTail(poly);

   return polylist;
}

/******************************************************************************
* CRealPartWrite::getAllData
*/
void CRealPartWrite::getAllData(BlockStruct *fileBlock)
{
   POSITION compPos = fileBlock->getDataList().GetHeadPosition();
   while (compPos)
   {
      DataStruct *compData = fileBlock->getDataList().GetNext(compPos);

      if (compData->getDataType() != T_INSERT) continue;

      switch (compData->getInsert()->getInsertType())
      {
      case INSERTTYPE_PCBCOMPONENT:
         addLP(compData->getInsert()->getRefname(), compData);
         break;
      case INSERTTYPE_REALPART:
         addPRT(compData->getInsert()->getRefname(), compData);
         break;
      }
   }
}

/******************************************************************************
* CRealPartWrite::addLP
*/
void CRealPartWrite::addLP(CString key, DataStruct *data)
{
   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   addLP(block->getName(), block);

   lpDatas.SetAt(key, data);
}

/******************************************************************************
* CRealPartWrite::addPRT
*/
void CRealPartWrite::addPRT(CString key, DataStruct *data)
{
   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   addPRT(block->getName(), block);

   prtDatas.SetAt(key, data);
}

/******************************************************************************
* CRealPartWrite::addPKG
*/
void CRealPartWrite::addPKG(CString key, DataStruct *data)
{
   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   addPKG(block->getName(), block);

// pkgDatas.SetAt(key, data);
}

/******************************************************************************
* CRealPartWrite::addLP
*/
void CRealPartWrite::addLP(CString key, BlockStruct *block)
{
   lpGeoms.SetAt(key, block);
}

/******************************************************************************
* CRealPartWrite::addPRT
*/
void CRealPartWrite::addPRT(CString key, BlockStruct *block)
{
   // add the part to the map
   prtGeoms.SetAt(key, block);

   // get the package and add it
   POSITION pkgPos = block->getDataList().GetHeadPosition();
   while (pkgPos)
   {
      DataStruct *pkg = block->getDataList().GetNext(pkgPos);

      if (pkg->getDataType() != dataTypeInsert)
         continue;

      if (pkg->getInsert()->getInsertType() != INSERTTYPE_PACKAGE)
         continue;

      addPKG(pkg->getInsert()->getRefname(), pkg);
   }
}

/******************************************************************************
* CRealPartWrite::addPKG
*/
void CRealPartWrite::addPKG(CString key, BlockStruct *block)
{
   pkgGeoms.SetAt(key, block);
}

/******************************************************************************
* CRealPartWrite::writeLibraryList
*/
void CRealPartWrite::writeLibraryList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_LIB_LIST);
   mFile->Write(buf, buf.GetLength());

   increaseIndent();
   POSITION pos = libMap->GetStartPosition();
   while (pos)
   {
      WORD key;
      SLibrary *lib = NULL;
      libMap->GetNextAssoc(pos, key, lib);

      writeLibrary(lib);
   }
   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_LIB_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeLibrary
*/
void CRealPartWrite::writeLibrary(SLibrary *library)
{
   CString buf;

   buf.Format("%*s<%s number=\"%d\" id=\"%s\" name=\"%s\"/>\n", indent, " ", RP_LIB,
      library->number, SwapSpecialCharacters(library->GUID), SwapSpecialCharacters(library->file->getName()));
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeFile
*/
void CRealPartWrite::writeFile(FileStruct *file, POSITION filePos)
{
   CString buf;

   buf.Format("%*s<%s number=\"%d\" position=\"%d\">\n", indent, " ", RP_FILE, file->getFileNumber(), filePos); 
   mFile->Write(buf, buf.GetLength());

   increaseIndent();
   
   // write the part section
   writePartData();
   // write the land pattern section
   writeLandPatternData();

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_FILE);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePartData
*/
void CRealPartWrite::writePartData()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_PART_DATA);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   writePackageDefinitionList();
   writeDeviceDefinitionList();
   writePartDefinitionList();
   writePartList();

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_PART_DATA);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePackageDefinitionList
*/
void CRealPartWrite::writePackageDefinitionList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_PKGDEF_LIST);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   POSITION pos = pkgGeoms.GetStartPosition();
   while (pos)
   {
      CString key;
      BlockStruct *block;
      
      pkgGeoms.GetNextAssoc(pos, key, block);
      writePackageDefinition(block);
   }

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_PKGDEF_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePackageDefinition
*/
void CRealPartWrite::writePackageDefinition(BlockStruct *block)
{
   CString buf, formatString;

   formatString = "%*s<%s name=\"%s\"";

   SLibrary *lib = libMap->Lookup(block->getFileNumber());
   if (lib)
   {
      formatString += " library=\"%d\">\n";
      buf.Format(formatString, indent, " ", RP_PKGDEF, SwapSpecialCharacters(block->getName()), lib->number);
   }
   else
   {
      formatString += ">\n";
      buf.Format(formatString, indent, " ", RP_PKGDEF, block->getName());
   }  
   mFile->Write(buf, buf.GetLength());

   increaseIndent();

   writeAttributes(block->getAttributesRef());

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_PKGDEF);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePackage
*/
void CRealPartWrite::writePackage(DataStruct *data)
{
   CString buf;

   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   SLibrary *lib = libMap->Lookup(block->getFileNumber());

   buf.Format("%*s<%s name=\"%s\" library=\"%d\" x=\"%d\" y=\"%d\" rotation=\"%.*f\"/>\n", indent, " ", RP_PKG,
      SwapSpecialCharacters(data->getInsert()->getRefname()), lib->number, 
      (int)(data->getInsert()->getOriginX() * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
      (int)(data->getInsert()->getOriginY() * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
      PRECISION, RadToDeg((double)data->getInsert()->getAngle()));
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeDeviceDefinitionList
*/
void CRealPartWrite::writeDeviceDefinitionList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_DEVDEF_LIST);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   POSITION pos = devGeoms.GetStartPosition();
   while (pos)
   {
      CString key;
      BlockStruct *block;
      
      devGeoms.GetNextAssoc(pos, key, block);
      
      writeDeviceDefinition(block);
   }

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_DEVDEF_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeDeviceDefinition
*/
void CRealPartWrite::writeDeviceDefinition(BlockStruct *block)
{
   CString buf;

   //buf.Format("%*s<%s name=\"%s\">\n", indent, " ", SwapSpecialCharacters(block->name), RP_DEVDEF);
   //mFile->Write(buf, buf.GetLength());

   //increaseIndent();

   //writeAttributes(block->getAttributesRef());

   //decreaseIndent();

   //buf.Format("%*s</%s>\n", indent, " ", block->name, RP_DEVDEF);
   //mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeDevice
*/
void CRealPartWrite::writeDevice(DataStruct *data)
{
   CString buf;

   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   SLibrary *lib = libMap->Lookup(block->getFileNumber());

   //buf.Format("%*s<%s name=\"%s\" library\"%d\" x=\"%d\" y=\"%d\" rotation=\"%.*f\"/>\n", indent, " ", RP_DEV,
   // SwapSpecialCharacters(data->getInsert()->refname), lib->number, 
   // (int)(data->getInsert()->getOriginX() * Units_Factor(doc->Settings.PageUnits, UNIT_MM) * 1000),
   // (int)(data->getInsert()->getOriginY() * Units_Factor(doc->Settings.PageUnits, UNIT_MM) * 1000),
   // PRECISION, RadToDeg((double)data->getInsert()->getAngle()));
   //mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePartDefinitionList
*/
void CRealPartWrite::writePartDefinitionList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_PARTDEF_LIST);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   POSITION pos = prtGeoms.GetStartPosition();
   while (pos)
   {
      CString key;
      BlockStruct *block;
      
      prtGeoms.GetNextAssoc(pos, key, block);
   
      writePartDefinition(block);
   }

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_PARTDEF_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePartDefinition
*/
void CRealPartWrite::writePartDefinition(BlockStruct *block)
{
   CString buf, formatString;

   formatString = "%*s<%s name=\"%s\"";

   SLibrary *lib = libMap->Lookup(block->getFileNumber());
   if (lib)
   {
      formatString += " library=\"%d\">\n";
      buf.Format(formatString, indent, " ", RP_PARTDEF, SwapSpecialCharacters(block->getName()), lib->number);
   }
   else
   {
      formatString += ">\n";
      buf.Format(formatString, indent, " ", RP_PARTDEF, block->getName());
   }  
   mFile->Write(buf, buf.GetLength());

   increaseIndent();

   // Get the package and device name
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case INSERTTYPE_PACKAGE:
         writePackage(data);
         break;
      //case INSERTTYPE_DEVICE:
      // writeDevice(data);
      // break;
      } // END switch (data->getInsert()->getInsertType()) ...
   } // END while (dataPos) ...

   writeAttributes(block->getAttributesRef());

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_PARTDEF);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePartList
*/
void CRealPartWrite::writePartList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_PART_LIST);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   POSITION pos = prtDatas.GetStartPosition();
   while (pos)
   {
      CString key;
      DataStruct *data;
      
      prtDatas.GetNextAssoc(pos, key, data);

      writePart(data);
   }

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_PART_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writePart
*/
void CRealPartWrite::writePart(DataStruct *partInsert)
{
	DataStruct *landPatternInsert = lpDatas.Lookup(partInsert->getInsert()->getRefname());
   DTransform xForm(landPatternInsert->getInsert()->getOriginX(), landPatternInsert->getInsert()->getOriginY(), landPatternInsert->getInsert()->getScale(), landPatternInsert->getInsert()->getAngle(), landPatternInsert->getInsert()->getMirrorFlags());
   Point2 pnt(partInsert->getInsert()->getOriginX(), partInsert->getInsert()->getOriginY());
   xForm.UntransformPoint(&pnt);
   DbUnit x = (DbUnit)pnt.x;
   DbUnit y = (DbUnit)pnt.y;
	DbUnit rotation = partInsert->getInsert()->getAngle() - landPatternInsert->getInsert()->getAngle();
	if (landPatternInsert->getInsert()->getMirrorFlags())
		rotation = -rotation;

	while (rotation < 0)
		rotation += (DbUnit)PI2;
	while (rotation >= PI2)
		rotation -= (DbUnit)PI2;

   BlockStruct *block = doc->getBlockAt(partInsert->getInsert()->getBlockNumber());
   SLibrary *lib = libMap->Lookup(block->getFileNumber());

   CString buf;
   buf.Format("%*s<%s name=\"%s\" library=\"%d\" x=\"%d\" y=\"%d\" rotation=\"%.*f\" refDes=\"%s\"/>\n", indent, " ", RP_PART,
      SwapSpecialCharacters(block->getName()), lib->number, 
      (int)(x * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
      (int)(y * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
      PRECISION, RadToDeg((double)rotation),
      partInsert->getInsert()->getRefname());
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeLandPatternData
*/
void CRealPartWrite::writeLandPatternData()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_LP_DATA);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   writeLandPatternDefinitionList();
   writeLandPatternList();

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_LP_DATA);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeLandPatternDefinitionList
*/
void CRealPartWrite::writeLandPatternDefinitionList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_LPDEF_LIST);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   POSITION pos = lpGeoms.GetStartPosition();
   while (pos)
   {
      CString key;
      BlockStruct *block;
      
      lpGeoms.GetNextAssoc(pos, key, block);
   
      writeLandPatternDefinition(block);
   }

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_LPDEF_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeLandPatternDefinition
*/
void CRealPartWrite::writeLandPatternDefinition(BlockStruct *block)
{
   CString buf;

   // Find the centroid
   DataStruct *centData = block->GetCentroidData();

   buf.Format("%*s<%s name=\"%s\" number=\"%d\" centroidX=\"%d\" centroidY=\"%d\">\n", indent, " ", RP_LPDEF,
      SwapSpecialCharacters(block->getName()), block->getBlockNumber(),
      centData?(int)(centData->getInsert()->getOriginX() * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000):0,
      centData?(int)(centData->getInsert()->getOriginY() * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000):0);
   mFile->Write(buf, buf.GetLength());

   increaseIndent();

   ////////////////////////////////////////////////////////
   // Loop through datas in the land pattern to find pads and a silkscreen
   POSITION padPos = block->getDataList().GetHeadPosition();
   while (padPos)
   {
      DataStruct *padData = block->getDataList().GetNext(padPos);


		// Figure Poly Type
      int polyType;
      if (padData->getDataType() == T_INSERT && padData->getInsert()->getInsertType() == INSERTTYPE_PIN)
         polyType = RP_POLY_PAD;
      else if (padData->getDataType() == T_POLY && padData->getGraphicClass() == GR_CLASS_COMPOUTLINE)
         polyType = RP_POLY_SILKSCREEN;
      else
         continue;


      switch (polyType)
      {
      case RP_POLY_SILKSCREEN:
         {
            // Write the silkscreen poly
            POSITION polyPos = padData->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = padData->getPolyList()->GetNext(polyPos);

					// Begin Poly
               buf.Format("%*s<%s type=\"%d\">\n", indent, " ", RP_POLY, polyType);
               mFile->Write(buf, buf.GetLength());

					// Write Pnts
               increaseIndent();
               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

                  buf.Format("%*s<%s x=\"%d\" y=\"%d\" bulge=\"%.*f\"/>\n", indent, " ", RP_POLYPOINT,
                     (int)(pnt->x * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
                     (int)(pnt->y * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
                     PRECISION, pnt->bulge);
                  mFile->Write(buf, buf.GetLength());
               }
               decreaseIndent();

					// End POLY
			      buf.Format("%*s</%s>\n", indent, " ", RP_POLY);
					mFile->Write(buf, buf.GetLength());
            }
            break;
         }

      case RP_POLY_PAD:
         {
            // Get the pin number and pin name
            CString pinNum, pinName = padData->getInsert()->getRefname();
            int pinNumberKW = doc->IsKeyWord(ATT_COMPPINNR, keyWordSectionCC);
            Attrib *attrib = NULL;

            if (pinNumberKW >= 0 && padData->getAttributesRef() && padData->getAttributesRef()->Lookup(pinNumberKW, attrib))
            {
               switch (attrib->getValueType())
               {
                  case VT_NONE:
                     break;
                  case VT_INTEGER:
                     pinNum.Format("%d", attrib->getIntValue());
                     break;
                  case VT_DOUBLE:
                  case VT_UNIT_DOUBLE:
                     pinNum.Format("%lf", attrib->getDoubleValue());
                     break;
                  case VT_STRING:
                  case VT_EMAIL_ADDRESS:
                  case VT_WEB_ADDRESS:
                     {
                        if (!attrib->getStringValue().IsEmpty())
                           pinNum = attrib->getStringValue();
                     }
                     break;
               }
            }

            // Get the actual pad shape (top, bottom, then extents)
            CPolyList *polylist = getRPPad(padData);
				if (!polylist)
					break;

            increaseIndent();
            POSITION polyPos = polylist->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = polylist->GetNext(polyPos);

					// Begin Poly
					buf.Format("%*s<%s type=\"%d\" number=\"%s\" name=\"%s\">\n",
						indent, " ", RP_POLY, polyType, SwapSpecialCharacters(pinNum), SwapSpecialCharacters(pinName));
					mFile->Write(buf, buf.GetLength());

					// Write Pnts
               increaseIndent();
					POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  buf.Format("%*s<%s x=\"%d\" y=\"%d\" bulge=\"%.*f\"/>\n",
                     indent, " ", RP_POLYPOINT,
                     (int)(pnt->x * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
                     (int)(pnt->y * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
                     PRECISION, pnt->bulge);

                  mFile->Write(buf, buf.GetLength());
               }
					decreaseIndent();

					// End POLY
					buf.Format("%*s</%s>\n", indent, " ", RP_POLY);
					mFile->Write(buf, buf.GetLength());
            } // END while (polyPos)
				decreaseIndent();

            // Free the memory created by getRPPad
            delete polylist;

				break;
         }
      } // END switch (polyType) ...
   } // END while (padPos) ...

   writeAttributes(block->getAttributesRef());

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_LPDEF);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeLandPatternList
*/
void CRealPartWrite::writeLandPatternList()
{
   CString buf;

   buf.Format("%*s<%s>\n", indent, " ", RP_LP_LIST);
   mFile->Write(buf, buf.GetLength());
   
   increaseIndent();

   POSITION pos = lpDatas.GetStartPosition();
   while (pos)
   {
      CString key;
      DataStruct *data;
      
      lpDatas.GetNextAssoc(pos, key, data);
   
      writeLandPattern(data);
   }

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_LP_LIST);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeLandPattern
*/
void CRealPartWrite::writeLandPattern(DataStruct *data)
{
   CString buf;

   buf.Format("%*s<%s refDes=\"%s\" x=\"%d\" y=\"%d\" rotation=\"%.*f\" geometry=\"%d\">\n", indent, " ", RP_LP,
      SwapSpecialCharacters(data->getInsert()->getRefname()),
      (int)(data->getInsert()->getOriginX() * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
      (int)(data->getInsert()->getOriginY() * Units_Factor(doc->getSettings().getPageUnits(), UNIT_MM) * 1000),
      PRECISION, RadToDeg((double)data->getInsert()->getAngle()),
      data->getInsert()->getBlockNumber());
   mFile->Write(buf, buf.GetLength());

   increaseIndent();

   writeAttributes(data->getAttributesRef());

   decreaseIndent();

   buf.Format("%*s</%s>\n", indent, " ", RP_LP);
   mFile->Write(buf, buf.GetLength());
}

/******************************************************************************
* CRealPartWrite::writeAttributes
*/
void CRealPartWrite::writeAttributes(CAttributes* attribMap)
{
   CString buf;

   if (attribMap)
   {
      for (POSITION attribPos = attribMap->GetStartPosition();attribPos != NULL;)
      {
         Attrib* pAttrib;
         WORD key;
         attribMap->GetNextAssoc(attribPos, key,pAttrib);
         AttribIterator attribIterator(pAttrib);
         Attrib attrib(pAttrib->getCamCadData());
         CString buf2;

         CString keyword = doc->getKeyWordArray()[key]->cc;

         while (attribIterator.getNext(attrib))
         {
            switch (attrib.getValueType())
            {
            case VT_NONE:
               buf.Format("%*s<%s name=\"%s\"/>\n", indent, " ", RP_ATTRIB, keyword);
               break;

            case VT_STRING:
            case VT_EMAIL_ADDRESS:
            case VT_WEB_ADDRESS:
               {
                  CString buf2 = attrib.getStringValue();
                  int i = buf2.Find('\n');
                  while (i != -1)
                  {
                     buf.Format("%*s<%s name=\"%s\" value=\"%s\"/>\n", indent, " ", RP_ATTRIB, SwapSpecialCharacters(keyword), SwapSpecialCharacters(buf2.Left(i)));
                     mFile->Write(buf, buf.GetLength());

                     buf2 = buf2.Right(buf2.GetLength()-i-1);
                     i = buf2.Find('\n');
                  }

                  buf.Format("%*s<%s name=\"%s\" value=\"%s\"/>\n", indent, " ", RP_ATTRIB, SwapSpecialCharacters(keyword), SwapSpecialCharacters(buf2));
               }
               break;

            case VT_INTEGER:
               buf.Format("%*s<%s name=\"%s\" value=\"%d\"/>\n", indent, " ", RP_ATTRIB, SwapSpecialCharacters(keyword), attrib.getIntValue());
               break;

            case VT_DOUBLE:
            case VT_UNIT_DOUBLE:
               buf.Format("%*s<%s name=\"%s\" value=\"%1.*g\"/>\n", indent, " ", RP_ATTRIB, SwapSpecialCharacters(keyword), 4 , attrib.getDoubleValue());
               break;
            }

            mFile->Write(buf, buf.GetLength());
         } // END while (attribIterator.getNext(attrib)) ...
      }
   }
}
