// $Header: /CAMCAD/5.0/read_wrt/UserDefinedMessage.cpp 7     6/17/07 9:00p Kurt Van Ness $

#include "StdAfx.h"
#include "UserDefinedMessage.h"
#include "CcDoc.h"

//------------------------------------------------------------------------
   
CUsrDefMsgNode::CUsrDefMsgNode(CString keyword, CString value)
{
   m_type = getTypeFromString(keyword);
   m_value = value;
}

EUsrDefMsgNodeType CUsrDefMsgNode::getTypeFromString(CString str)
{
   if (str.CompareNoCase(".MSG_TEXT") == 0)
   {
      return EMsgNodeTypeText;
   }
   else if (str.CompareNoCase(".MSG_ATTR") == 0)
   {
      return EMsgNodeTypeAttr;
   }
   else if (str.CompareNoCase(".MSG_PARAM") == 0)
   {
      return EMsgNodeTypeParam;
   }

   return EMsgNodeTypeUnknown;
}

CString CUsrDefMsgNode::format(CCEtoODBDoc *doc, const DataStruct *data)
{
   CString msg;

   if (data != NULL)
   {
      if (data->getDataType() == dataTypeInsert)
         msg = format(*doc, data->getInsert(), data->getAttributes());
      else
         msg = format(*doc, NULL, data->getAttributes());
   }
   
   return msg;

   //CString msg;

   //if (data != NULL)
   //{
   //   switch (this->m_type)
   //   {
   //   case EMsgNodeTypeText:
   //      msg = getValue();
   //      break;

   //   case EMsgNodeTypeAttr:
   //      if (!this->m_value.IsEmpty())
   //      {
   //         // Use attrib value for message
   //         WORD kw = doc->IsKeyWord(m_value, 0);
   //         Attrib *attrib = NULL;
   //         if (data->lookUpAttrib(kw, attrib))
   //         {
   //            switch (attrib->getValueType())
   //            {
   //            case valueTypeString:
   //               msg.Format("%s", attrib->getStringValue());
   //               break;
   //            case valueTypeDouble:
   //               msg.Format("%f", attrib->getDoubleValue());
   //               break;
   //            case valueTypeInteger:
   //               msg.Format("%d", attrib->getIntValue());
   //               break;
   //            }
   //         }
   //      }
   //      break;

   //   case EMsgNodeTypeParam:
   //      if (data->getDataType() == dataTypeInsert && data->getInsert() != NULL)
   //      {
   //         int decimals = GetDecimals(doc->getSettings().getPageUnits());

   //         if (getValue().CompareNoCase("X") == 0)
   //         {
   //            msg.Format("%.*lf", decimals, data->getInsert()->getOriginX());
   //         }
   //         else if (getValue().CompareNoCase("Y") == 0)
   //         {
   //            msg.Format("%.*lf", decimals, data->getInsert()->getOriginY());
   //         }
   //         else if (getValue().CompareNoCase("ROT") == 0)
   //         {
   //            msg.Format("%d", round(normalizeDegrees(data->getInsert()->getAngleDegrees())));
   //         }
   //         
   //         else if (getValue().CompareNoCase("CX") == 0)
   //         {
   //            CPoint2d pt;
   //            if (data->getInsert()->getCentroidLocation(doc, pt))
   //               msg.Format("%.*lf", decimals, pt.x);
   //         }
   //         else if (getValue().CompareNoCase("CY") == 0)
   //         {
   //            CPoint2d pt;
   //            if (data->getInsert()->getCentroidLocation(doc, pt))
   //               msg.Format("%.*lf", decimals, pt.y);
   //         }
   //         else if (getValue().CompareNoCase("CROT") == 0)
   //         {
   //            double centRotDegrees = 0.0;
   //            if (data->getInsert()->getCentroidRotation(doc, centRotDegrees))
   //            {
   //               msg.Format("%d", round(normalizeDegrees(centRotDegrees)));
   //            }
   //         }

   //         else if (getValue().CompareNoCase("SIDE") == 0)
   //         {
   //            if (data->getInsert()->getPlacedBottom())
   //               msg = "Bottom";
   //            else
   //               msg = "Top";
   //         }
   //         else if (getValue().CompareNoCase("GEOM") == 0)
   //         {
   //            BlockStruct *blk = doc->getBlockAt(data->getInsert()->getBlockNumber());
   //            if (blk != NULL)
   //               msg = blk->getName();
   //         }

   //      }
   //      break;

   //   default:
   //      break;
   //   }
   //}

   //return msg;
}

CString CUsrDefMsgNode::format(CCEtoODBDoc& doc, const InsertStruct* insert, CAttributes* attributeMap)
{
   CString msg;

   switch (this->m_type)
   {
   case EMsgNodeTypeText:
      msg = getValue();
      break;

   case EMsgNodeTypeAttr:
      if (!this->m_value.IsEmpty() && attributeMap != NULL)
      {
         // Use attrib value for message
         WORD kw = doc.IsKeyWord(m_value, 0);
         Attrib *attrib = NULL;
         if (attributeMap->Lookup(kw, attrib))
         {
            switch (attrib->getValueType())
            {
            case valueTypeString:
               msg.Format("%s", attrib->getStringValue());
               break;
            case valueTypeDouble:
               msg.Format("%f", attrib->getDoubleValue());
               break;
            case valueTypeInteger:
               msg.Format("%d", attrib->getIntValue());
               break;
            }
         }
      }
      break;

   case EMsgNodeTypeParam:
      if (insert != NULL)
      {
         int decimals = GetDecimals(doc.getSettings().getPageUnits());

         if (getValue().CompareNoCase("X") == 0)
         {
            msg.Format("%.*lf", decimals, insert->getOriginX());
         }
         else if (getValue().CompareNoCase("Y") == 0)
         {
            msg.Format("%.*lf", decimals, insert->getOriginY());
         }
         else if (getValue().CompareNoCase("ROT") == 0)
         {
            msg.Format("%d", round(normalizeDegrees(insert->getAngleDegrees())));
         }
         else if (getValue().CompareNoCase("CX") == 0)
         {
            CPoint2d pt;
            if (insert->getCentroidLocation(doc.getCamCadData(), pt))
               msg.Format("%.*lf", decimals, pt.x);
         }
         else if (getValue().CompareNoCase("CY") == 0)
         {
            CPoint2d pt;
            if (insert->getCentroidLocation(doc.getCamCadData(), pt))
               msg.Format("%.*lf", decimals, pt.y);
         }
         else if (getValue().CompareNoCase("CROT") == 0)
         {
            double centRotDegrees = 0.0;
            if (insert->getCentroidRotation(doc.getCamCadData(), centRotDegrees))
            {
               msg.Format("%d", round(normalizeDegrees(centRotDegrees)));
            }
         }
         else if (getValue().CompareNoCase("SIDE") == 0)
         {
            if (insert->getPlacedBottom())
               msg = "Bottom";
            else
               msg = "Top";
         }
         else if (getValue().CompareNoCase("GEOM") == 0)
         {
            BlockStruct *blk = doc.getBlockAt(insert->getBlockNumber());
            if (blk != NULL)
               msg = blk->getName();
         }

      }
      break;

   default:
      break;
   }

   return msg;
}

//------------------------------------------------------------------------

CUserDefinedMessage::CUserDefinedMessage()
{
}

CUserDefinedMessage::~CUserDefinedMessage()
{
   m_msgNodes.empty();
}

bool CUserDefinedMessage::isKeyword(CString inputstr)
{
   // Check if inputstr is a keyword recognized by this utility

   if (CUsrDefMsgNode::getTypeFromString(inputstr) != EMsgNodeTypeUnknown)
      return true;

   return false;
}


bool CUserDefinedMessage::append(CString keyword, CString value)
{
   if (isKeyword(keyword))
   {
      CUsrDefMsgNode *msgnode = new CUsrDefMsgNode(keyword, value);
      m_msgNodes.AddTail(msgnode);
      return true;
   }

   return false;
}

bool CUserDefinedMessage::isEmpty()
{
   return m_msgNodes.IsEmpty()?true:false;
}

CString CUserDefinedMessage::format(CCEtoODBDoc *doc, const DataStruct *data)
{
   CString msg;

   POSITION pos = m_msgNodes.GetHeadPosition();
   while (pos)
   {
      CUsrDefMsgNode *msgNode = m_msgNodes.GetNext(pos);
      CString thisMsg = msgNode->format(doc, data);
      msg += thisMsg;
   }

   return msg;
}

CString CUserDefinedMessage::format(CCEtoODBDoc& doc, const InsertStruct* insert, CAttributes* attributeMap)
{
   CString msg;

   POSITION pos = m_msgNodes.GetHeadPosition();
   while (pos)
   {
      CUsrDefMsgNode *msgNode = m_msgNodes.GetNext(pos);
      CString thisMsg = msgNode->format(doc, insert, attributeMap);
      msg += thisMsg;
   }

   return msg;
}

void CUserDefinedMessage::dump()
{
   CString msg;

   POSITION pos = m_msgNodes.GetHeadPosition();
   while (pos)
   {
      CUsrDefMsgNode *msgNode = m_msgNodes.GetNext(pos);
      CString thisMsg;
      thisMsg.Format("Type(%d) Value(%s)\n", msgNode->getType(), msgNode->getValue());
      msg += thisMsg;
   }

   AfxMessageBox(msg);
}
