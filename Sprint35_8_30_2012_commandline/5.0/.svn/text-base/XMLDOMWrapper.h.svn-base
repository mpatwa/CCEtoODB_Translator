// $Header: /CAMCAD/4.5/XMLDOMWrapper.h 4     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#pragma once

#ifndef __XMLDOMWRAPPER__
#define __XMLDOMWRAPPER__

#include "msxml.h"

class CXMLNodeList;

/******************************************************************************
* CXMLNode
*/
class CXMLNode
{
public:
	CXMLNode(IXMLDOMNode *node = NULL);
	~CXMLNode();

private:
	IXMLDOMNode *m_pXMLNode;
	CXMLNodeList *m_pXMLNodeList;

	CXMLNodeList *getChildNodes();

public:
	void SetXMLNode(IXMLDOMNode *node);

	bool GetName(CString &nodeName);
	bool GetAttrValue(CString name, CString &value);

	CXMLNode *GetFirstChild();

	void ResetChildList();
	CXMLNode *NextChild();
   int GetChildNodeCount();
   int GetDescendantNodeCount();
};

/******************************************************************************
* CXMLNodeList
*/
class CXMLNodeList
{
public:
	CXMLNodeList(IXMLDOMNodeList *nodeList = NULL);
	~CXMLNodeList();

private:
	IXMLDOMNodeList *m_pXMLNodeList;

public:
	void Reset();
	CXMLNode *NextNode();
   int GetChildNodeCount();
   int GetDescendantNodeCount();
};

/******************************************************************************
* CXMLDocument
*/
class CXMLDocument : public CXMLNode
{
public:
	CXMLDocument(IXMLDOMDocument *doc = NULL);
	~CXMLDocument();

private:
	IXMLDOMDocument* m_pXMLDoc;

public:
	bool LoadXML(CString xmlString);
	bool LoadXMLFile(CString filename);

	CXMLNode *GetNodeFromID(CString name);
	CXMLNodeList *GetElementsByTagName(CString tag);
};


#endif // __XMLDOMWRAPPER__
