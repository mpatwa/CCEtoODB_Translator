
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "xmllayertypecontent.h"
#include "lyrtype.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif                           

//_____________________________________________________________________________
/******************************************************************************
* LayerTypeXMLReader
*/
LayerTypeXMLReader::LayerTypeXMLReader(CCEtoODBDoc& camCadDoc)
: CCamCadDcaXmlReader(camCadDoc)
, m_camCadDoc(camCadDoc)
{
}

/******************************************************************************
* ~LayerTypeXMLReader
*/
LayerTypeXMLReader::~LayerTypeXMLReader()
{
}

CDcaXmlContent& LayerTypeXMLReader::getDefinedContent()
{
   if (m_content == NULL)
   {
      m_content = new XMLLayerTypeContent(m_camCadDoc);
   }

   return *m_content;
}

/******************************************************************************
* setDoc
*/
//void LayerTypeXMLReader::setDoc(CCEtoODBDoc *doc)
//{
//   //content = new XMLLayerTypeContent();
//   //content->SetDoc(doc);
//}

/******************************************************************************
* open
*/
FileStatusTag LayerTypeXMLReader::open(const CString& filename)
{
   FileStatusTag retval = statusUndefined;

   ERRMsg.Empty();

   if (m_file == NULL)
   {
      m_file = new CFlexFile();
   }

   if (!m_file->Open(filename, CFile::modeRead))
   {
      retval = statusFileOpenFailure;
   }
   else
   {
      m_lastFilePos = 0;
      retval = statusInvalidHeader;

      CString header("<LAYERTYPEDOC ");
      CString headerBuf;
      int strLen = header.GetLength();

      if (m_file->Read(headerBuf.GetBufferSetLength(strLen), strLen))
      {
         headerBuf.GetBufferSetLength(strLen);

         if (headerBuf.CompareNoCase(header) == 0)
         {
            m_file->SeekToBegin();

            fileName = filename;
            retval = statusSucceeded;
         }
         else
         {
            m_file->Close();
         }
      }
   }

   return retval;
}

/******************************************************************************
/******************************************************************************
* XMLRealPartContent
*/
XMLLayerTypeContent::XMLLayerTypeContent(CCEtoODBDoc& camCadDoc)
: CDcaXmlContent(camCadDoc.getCamCadData())
, m_camCadDoc(camCadDoc)
{
	currentColorSetIndex = -1;
}

/******************************************************************************
* SetDoc
*/
//void XMLLayerTypeContent::SetDoc(CCEtoODBDoc *ccDoc)
//{
//   //XMLContent::SetDoc(ccDoc);
//}

/******************************************************************************
* startElement
*/
BOOL XMLLayerTypeContent::startElement(CString localTagName, CMapStringToString *tAttributes)
{
   CString localName = localTagName;
   CMapStringToString *pAttributes = tAttributes;

   char c = localName[0];

   switch (c)
   {
   case 'C':
      if (localName == LT_COLORSET)
		{
			onColorSet(tAttributes);
		}
      break;
   case 'L':
      if (localName == LT_LAYERTYPEDOC)
      {
         onLayerTypeDoc(tAttributes);
      }
      else if (localName == LT_LAYERTYPE)
      {
         onLayerType(tAttributes);
      }
   }

   return TRUE;
}

/******************************************************************************
* endElement
*/
void XMLLayerTypeContent::endElement(CString localTagName)
{
   CString localName = localTagName;

   if (localName == LT_LAYERTYPE)
   {
   }
   else if (localName == LT_COLORSET)
   {
		currentColorSetIndex = -1;
   }
}

void XMLLayerTypeContent::onColorSet(CMapStringToString *attributes)
{
	CString value;

	if (attributes->Lookup("name", value))
		currentColorSetName = value;

	if (attributes->Lookup("index", value))
		currentColorSetIndex = atoi(value);

	if (currentColorSetIndex >= 3)
   {
		m_camCadDoc.CustomLayersetNames[currentColorSetIndex - MAX_STANDARD_COLORSETS] = currentColorSetName;
   }
}

void XMLLayerTypeContent::onLayerTypeDoc(CMapStringToString *attributes)
{
}

void XMLLayerTypeContent::onLayerType(CMapStringToString *attributes)
{
	if (currentColorSetIndex < 0)
		return;

	CString value;
	int type;
	COLORREF color;
	BOOL show;

	if (attributes->Lookup("type", value))
		type = atoi(value);

	if (attributes->Lookup("color", value))
		color = atoi(value);

	if (attributes->Lookup("show", value))
		show = (value == "Yes")?TRUE:FALSE;

	if (currentColorSetIndex >= MAX_LAYERSETS)
		return;

	if (type >= MAX_LAYTYPE)
		return;

	LayerTypeStruct *lt = &(m_camCadDoc.LayerTypeArray[currentColorSetIndex][type]);

	lt->color = color;
	lt->show = show;
}
