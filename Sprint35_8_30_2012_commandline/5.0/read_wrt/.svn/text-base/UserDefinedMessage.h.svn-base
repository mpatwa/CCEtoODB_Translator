// $Header: /CAMCAD/4.6/read_wrt/UserDefinedMessage.h 3     3/14/07 3:37p Lynn Phung $

#if !defined(__UserDefinedMessage_h__)
#define __UserDefinedMessage_h__

#pragma once

#include "TypedContainer.h"

class CCEtoODBDoc;
class DataStruct;
class InsertStruct;
class CAttributes;

//---------------------------------------------------------------------------

enum EUsrDefMsgNodeType
{
   EMsgNodeTypeUnknown,
   EMsgNodeTypeText,
   EMsgNodeTypeAttr,
   EMsgNodeTypeParam
};

class CUsrDefMsgNode
{
private:
   EUsrDefMsgNodeType m_type;
   CString m_value;

public:
   CUsrDefMsgNode(CString keyword, CString value);
   static EUsrDefMsgNodeType getTypeFromString(CString str);
   EUsrDefMsgNodeType getType()  { return m_type; }
   CString getValue()            { return m_value; }
   CString format(CCEtoODBDoc *doc, const DataStruct *data);
   CString format(CCEtoODBDoc& doc, const InsertStruct* insert, CAttributes* attributeMap);
};

class CUserDefinedMessage
{
   // User defines message by making .out file settings.
   // This class reads and stores the definitions, and produces the
   // message text which (presumably) will be output to some program file.

private:
   CTypedPtrListContainer<CUsrDefMsgNode*> m_msgNodes;

public:
   CUserDefinedMessage();
   ~CUserDefinedMessage();
   bool isKeyword(CString potentialKw);
   bool append(CString keyword, CString value);
   bool isEmpty();
   CString format(CCEtoODBDoc *doc, const DataStruct *data);
   CString format(CCEtoODBDoc& doc, const InsertStruct* insert, CAttributes* attributeMap);
   void dump();
};

#endif
