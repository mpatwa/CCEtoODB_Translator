// $Header: /CAMCAD/5.0/XmlLayerTypeContent.h 6     6/30/07 2:41a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#pragma once
#include "xml_read.h"

//_____________________________________________________________________________
//class LayerTypeXMLReader : public XMLReader
class LayerTypeXMLReader : public CCamCadDcaXmlReader
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   LayerTypeXMLReader(CCEtoODBDoc& camCadDoc);
   virtual ~LayerTypeXMLReader();

   virtual FileStatusTag open(const CString& filename);
   //void setDoc(CCEtoODBDoc *doc);
   virtual CDcaXmlContent& getDefinedContent();
};

//_____________________________________________________________________________
//class XMLLayerTypeContent : public XMLContent
class XMLLayerTypeContent : public CDcaXmlContent
{
private:
   CCEtoODBDoc& m_camCadDoc;

public:
   XMLLayerTypeContent(CCEtoODBDoc& camCadDoc);
   virtual BOOL startElement(CString localTagName, CMapStringToString *tAttributes);
   virtual void endElement(CString localTagName);

protected:
	CString currentColorSetName;
	long currentColorSetIndex;

   //virtual void SetDoc(CCEtoODBDoc *ccDoc);

private:
	void onColorSet(CMapStringToString *attributes);
	void onLayerTypeDoc(CMapStringToString *attributes);
	void onLayerType(CMapStringToString *attributes);
};