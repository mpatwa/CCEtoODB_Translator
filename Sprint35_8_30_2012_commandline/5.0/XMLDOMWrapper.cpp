// $Header: /CAMCAD/4.5/XMLDOMWrapper.cpp 4     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "xmldomwrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
* CXMLNode::CXMLNode
*/
CXMLNode::CXMLNode(IXMLDOMNode *node)
{
	if (node == NULL)
	{
		CoInitialize(NULL);
		if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMNode, (void**)&m_pXMLNode) != S_OK)
		{
			m_pXMLNode = NULL;
			return;
		}
	}
	else
		m_pXMLNode = node;

	m_pXMLNodeList = getChildNodes();
}

CXMLNode::~CXMLNode()
{
	m_pXMLNode->Release();
	delete m_pXMLNodeList;
}

CXMLNodeList *CXMLNode::getChildNodes()
{
	if (m_pXMLNode == NULL)
		return NULL;

	IXMLDOMNodeList *nodeList = NULL;
	if (m_pXMLNode->get_childNodes(&nodeList) != S_OK)
		return NULL;

	CXMLNodeList *nList = new CXMLNodeList(nodeList);
	nList->Reset();

	return nList;
}

void CXMLNode::SetXMLNode(IXMLDOMNode *node)
{
	if (node == NULL)
		return;

	if (m_pXMLNode != NULL)
	{
		m_pXMLNode->Release();
		delete m_pXMLNodeList;
	}

	m_pXMLNode = node;
	m_pXMLNodeList = getChildNodes();
}

bool CXMLNode::GetName(CString &nodeName)
{
	if (m_pXMLNode == NULL)
		return false;

	BSTR bsName;
	if (m_pXMLNode->get_nodeName(&bsName) != S_OK)
		return false;

	nodeName = bsName;
	return true;
}

bool CXMLNode::GetAttrValue(CString name, CString &value)
{
	if (m_pXMLNode == NULL)
		return false;

	IXMLDOMNamedNodeMap *attrMap = NULL;
	if (m_pXMLNode->get_attributes(&attrMap) != S_OK)
		return false;

	IXMLDOMNode *attrNode = NULL;
	BSTR bsName = name.AllocSysString();
	HRESULT hRes = attrMap->getNamedItem(bsName, &attrNode);
	SysFreeString(bsName);
	attrMap->Release();

	if (hRes != S_OK)
		return false;

	VARIANT varValue;
	attrNode->get_nodeValue(&varValue);
	attrNode->Release();

	value = varValue;
	return true;
}

CXMLNode *CXMLNode::GetFirstChild()
{
	if (m_pXMLNode == NULL)
		return NULL;

	IXMLDOMNode *node;
	if (m_pXMLNode->get_nextSibling(&node) != S_OK)
		return NULL;

	return new CXMLNode(node);
}

void CXMLNode::ResetChildList()
{
	if (m_pXMLNodeList == NULL)
		return;

	m_pXMLNodeList->Reset();
}

CXMLNode *CXMLNode::NextChild()
{
	if (m_pXMLNodeList == NULL)
		return NULL;

	return m_pXMLNodeList->NextNode();
}

int CXMLNode::GetChildNodeCount()
{
   int childNodeCount = 0;

	if (m_pXMLNodeList != NULL)
   {
      childNodeCount = m_pXMLNodeList->GetChildNodeCount();
   }

	return childNodeCount;
}

int CXMLNode::GetDescendantNodeCount()
{
   int descendantNodeCount = 0;

	if (m_pXMLNodeList != NULL)
   {
      descendantNodeCount = m_pXMLNodeList->GetDescendantNodeCount();
   }

	return descendantNodeCount;
}

/******************************************************************************
* CXMLNodeList::CXMLNodeList
*/
CXMLNodeList::CXMLNodeList(IXMLDOMNodeList *nodeList)
{
	if (nodeList != NULL)
	{
		m_pXMLNodeList = nodeList;
		return;
	}

	CoInitialize(NULL);
	if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMNodeList, (void**)&m_pXMLNodeList) != S_OK)
	{
		m_pXMLNodeList = NULL;
		return;
	}
}

CXMLNodeList::~CXMLNodeList()
{
	m_pXMLNodeList->Release();
}

void CXMLNodeList::Reset()
{
	if (m_pXMLNodeList == NULL)
		return;

	m_pXMLNodeList->reset();
}

CXMLNode *CXMLNodeList::NextNode()
{
	if (m_pXMLNodeList == NULL)
		return NULL;

	IXMLDOMNode *node;
	HRESULT hRes = m_pXMLNodeList->nextNode(&node);
	if (hRes == S_FALSE)
		return NULL;

	return (new CXMLNode(node));
}

int CXMLNodeList::GetChildNodeCount()
{
	long listLength = 0;

	if (m_pXMLNodeList != NULL)
   {
	   HRESULT hRes = m_pXMLNodeList->get_length(&listLength);
      
	   if (hRes == S_FALSE)
      {
         listLength = 0;
      }
   }

	return (int)listLength;
}

int CXMLNodeList::GetDescendantNodeCount()
{
	Reset();
   int descendantNodeCount = 0;

   for (CXMLNode* subNode = NextNode();subNode != NULL;subNode = NextNode())
   {
      descendantNodeCount++;
      descendantNodeCount += subNode->GetDescendantNodeCount();
      delete subNode;
   }

	return descendantNodeCount;
}

/******************************************************************************
* CXMLDocument::CXMLDocument
*/
CXMLDocument::CXMLDocument(IXMLDOMDocument *doc) : CXMLNode(doc)
{
	if (doc != NULL)
	{
		m_pXMLDoc = doc;

		IXMLDOMNode *pXMLDoc = NULL;
		if (m_pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pXMLDoc) == S_OK)
			SetXMLNode(pXMLDoc);
		return;
	}

	CoInitialize(NULL);
	if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&m_pXMLDoc) != S_OK)
	{
		m_pXMLDoc = NULL;
		return;
	}
}

CXMLDocument::~CXMLDocument()
{
	m_pXMLDoc->Release();
}

bool CXMLDocument::LoadXML(CString xmlString)
{
	if (m_pXMLDoc == NULL)
		return false;

	VARIANT_BOOL success;
	m_pXMLDoc->loadXML(xmlString.AllocSysString(), &success);

	return (success == VARIANT_TRUE);
}

bool CXMLDocument::LoadXMLFile(CString filename)
{
	if (m_pXMLDoc == NULL)
		return false;

	CComVariant fileLoc(filename.AllocSysString());
	VARIANT_BOOL success;
	m_pXMLDoc->load(fileLoc, &success);
	fileLoc.Clear();

	return (success == VARIANT_TRUE);
}

CXMLNode *CXMLDocument::GetNodeFromID(CString name)
{
	if (m_pXMLDoc == NULL)
		return NULL;

	if (name.IsEmpty())
		return NULL;

	BSTR bsName = name.AllocSysString();
	IXMLDOMNode *node = NULL;
	HRESULT hRes = m_pXMLDoc->nodeFromID(bsName, &node);
	SysFreeString(bsName);

	if (hRes != S_OK)
		return NULL;

	return (new CXMLNode(node));
}

CXMLNodeList *CXMLDocument::GetElementsByTagName(CString tag)
{
	if (m_pXMLDoc == NULL)
		return NULL;

	if (tag.IsEmpty())
		return NULL;

	BSTR bsTag = tag.AllocSysString();
	IXMLDOMNodeList *nodeList = NULL;
	HRESULT hRes = m_pXMLDoc->getElementsByTagName(bsTag, &nodeList);
	SysFreeString(bsTag);

	if (hRes != S_OK)
		return NULL;

	return (new CXMLNodeList(nodeList));
}
