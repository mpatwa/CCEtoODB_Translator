// $Header: /CAMCAD/5.0/Attrib.cpp 129   6/17/07 8:49p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "attrib.h"
#include "ogp.h"
#include "pcbutil.h"
#include "WriteFormat.h"
#include <math.h>
#include <float.h>
#include <string.h>
#include "CCEtoODB.h"

extern BOOL LoadingDataFile; // CCDOC.CPP
extern CStatusBar *StatusBar; // from MAINFRM.CPP

void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);

char *attribute_group[] = // see attrib.h ATTGROUP_xxx
{
   "User",              //
   "Internal",          //
   "Test",              // generic test
   "Agilent Test",      // specific to 5DX, 3070, AOI
   "OGP",
   "PCB Design",        // generic PCB design attributes
   "Assembly",
   "DFM",
   "RealPart",
   "Huntron",
};


//_____________________________________________________________________________

// Supports european convention of comma as decimal point as well as period
static double strToDouble(const char *c)
{
   CString  cc;
   cc = c;
   cc.Replace(",",".");
   return atof(cc);
}

/*****************************************************************************
* IsKeyWord()
*
*  Description: Gets index to this KeyWord in the KeyWordArray.
*
*  Parameter : str -> KeyWord
*              section -> 0=cc, 1=in, 2=out
*
*  Returns : index to KeyWord in KeyWordArray
*            or -1 if not found.
*/ // Keep
int CCEtoODBDoc::IsKeyWord(const char *STR, int section)
{
   // no left or right blanks
   CString  str;
   str = STR;
   str.TrimLeft();
   str.TrimRight();

   if (strlen(str) == 0)
      return -1;

   return getKeyWordArray().Lookup(STR, (EKeyWordSection)section);
}

/* RegisterKeyWord()
*
*  Description: Gets index to this KeyWord in the KeyWordArray.
*     If this KeyWord was not in the array, it adds it.
*
*  Parameter : str -> KeyWord
*              section -> 0=cc, 1=in, 2=out
*
*  Returns : index to KeyWord in KeyWordArray
*/
int CCEtoODBDoc::RegisterKeyWord(const char* attributeName,ValueTypeTag valueType,bool hiddenFlag)
{
   return RegisterKeyWord(attributeName,0,valueType,hiddenFlag);
}

int CCEtoODBDoc::RegisterKeyWord(const char *keyword, int section, int valueType,bool hiddenFlag)
{
   int   index;

   // no left or right blanks
   CString  str;
   str = keyword;
   str.TrimLeft();
   str.TrimRight();
   ValueTypeTag valueTypeTag = intToValueTypeTag(valueType);

   if (strlen(str) == 0)
   {
      return -1;
   }

   if ((index = IsKeyWord(keyword, section)) > -1)
   {
      if (getKeyWordArray()[index]->getValueType() != valueType)
      {
         if (LoadingDataFile)
         {
            getKeyWordArray().SetValueType(index,valueTypeTag);
         }
         else
         {
            CString  tmp;
            tmp.Format("Attrib Keyword [%s] Value Type Mismatch! \nThis Keyword is  reserved for internal use.  \nYou must rename this keyword in order to continue.", keyword);
            ErrorMessage(tmp);
            // need to make a new keyword, different from the internal one.
         }
      }

      return index;
   }

   // add to array
   return getKeyWordArray().SetAtGrow(str, attribGroupUser, valueTypeTag,true,hiddenFlag);
}

const KeyWordStruct* CCEtoODBDoc::getStandardAttributeKeyword(StandardAttributeTag standardAttributeTag)
{
   const KeyWordStruct* keyword = NULL;

   if (standardAttributeTag < m_standardKeywordArray.GetSize())
   {
      keyword = m_standardKeywordArray.GetAt(standardAttributeTag);
   }

   if (keyword == NULL)
   {
      CString keywordName = StandardAttributeTagToName(standardAttributeTag);
      int keywordIndex = getKeyWordArray().Lookup(keywordName,keyWordSectionCC);

      if (keywordIndex >= 0)
      {
         keyword = getKeyWordArray()[keywordIndex];
      }

      m_standardKeywordArray.SetAtGrow(standardAttributeTag,(KeyWordStruct*)keyword);
   }

   return keyword;
}

int CCEtoODBDoc::getStandardAttributeKeywordIndex(StandardAttributeTag standardAttributeTag)
{
   int keywordIndex = -1;

   const KeyWordStruct* keyword = getStandardAttributeKeyword(standardAttributeTag);

   if (keyword != NULL)
   {
      keywordIndex = keyword->getIndex();
   }

   return keywordIndex;
}

const KeyWordStruct* CCEtoODBDoc::getKeyword(int keywordIndex)
{
   const KeyWordStruct* keyword = NULL;

   if (keywordIndex >= 0 && keywordIndex < getKeyWordArray().GetSize())
   {
      keyword = getKeyWordArray().GetAt(keywordIndex);
   }

   return keyword;
}

/******************************************************************************
* IsValue()
*
*  Description: Gets index to this Value in the ValueArray.
*
*  Parameter : str -> Value
*
*  Returns : index to Value in ValueArray
*/
int CCEtoODBDoc::IsValue(const char *STR)
{
   int retval = getCamCadData().getAttributeValueDictionary().getValueIndex(STR);

   //// no left or right blanks
   //CString  str;
   //str = STR;
   //str.TrimLeft();
   //str.TrimRight();

   //long index;

   //if (str.IsEmpty())
   //   return -1;

   //// check for key word already in array
   //void *voidPtr;
   //if (ValueDictionary.Lookup(str, voidPtr))
   //{
   //   index = (long)voidPtr;
   //   return index;
   //}

   return retval;
}

/******************************************************************************
* RegisterValue()
*
*  Description: Gets index to this Value in the ValueArray.
*     If this Value was not in the array, it adds it.
*
*  Parameter : str -> Value
*
*  Returns : index to Value in ValueArray
*/
int CCEtoODBDoc::RegisterValue(const char *STR)
{
   int index = getCamCadData().getAttributeValueDictionary().add(STR);

   //int index = IsValue(STR);
   //if (index >= 0)
   //   return index;

   //// add to array
   //index = ++maxValueArray;
   //ValueArray.SetAtGrow(index, STR);

   //void *voidPtr = (void*)index;
   //ValueDictionary.SetAt(STR, voidPtr);

   return index;
}

/******************************************************************************
* SetUnknownAttrib
*
*  set an attribute by first testing the iskeyword function
*/
BOOL CCEtoODBDoc::SetUnknownAttrib(CAttributes** map, const char *keyword, const char *value, int method, Attrib **attribPtr)
{
   int      r;
   CString  kw1;
   int      valtype;

   if ((r = IsKeyWord(keyword, 0)) > -1)
   {
      valtype = getKeyWordArray()[r]->getValueType();
   }
   else
   {
      if ((r = RegisterKeyWord(keyword, 0, valueTypeString)) < 0)
         return 0;
      valtype = valueTypeString;
   }

   switch (valtype)
   {
      case valueTypeNone:
         return SetAttrib(map, r, valueTypeNone, NULL, method, attribPtr);
      break;
      case valueTypeString:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
      default:
         return SetAttrib(map, r, valueTypeString, (char *)value, method, attribPtr);
      break;
      case valueTypeInteger:
      {
         int i = atoi(value);
         return SetAttrib(map, r, valueTypeInteger, &i, method, attribPtr);
      }
      break;
      case valueTypeDouble:
      {
         double d = strToDouble(value);
         return SetAttrib(map, r, valueTypeDouble, &d, method, attribPtr);
      }
      break;
      case valueTypeUnitDouble:
      {
         double d = strToDouble(value);
         return SetAttrib(map, r, valueTypeUnitDouble, &d, method, attribPtr);
      }
      break;
   }
}

/******************************************************************************
* SetUnknownVisAttrib
*
*  set an attribute by first testing the iskeyword function
*/
BOOL CCEtoODBDoc::SetUnknownVisAttrib(CAttributes** map, const char *keyword, const char *value,
      double x, double y, double rotation, double height, double width, BOOL proportional, int penWidthIndex,
      BOOL visible, int method, DbFlag flag, short layer, BOOL never_mirror, int textAlignment, int lineAlignment,
      BOOL mirror_flip)
{
   Attrib* attrib = NULL;
   BOOL result = SetUnknownAttrib(map, keyword, value, method, &attrib);

   if (attrib != NULL)
   {
      attrib->setCoordinate(x,y);
      attrib->setRotationRadians(rotation);
      attrib->setHeight(height);
      attrib->setWidth(width);
      attrib->setProportionalSpacing(proportional);
      attrib->setPenWidthIndex(penWidthIndex);
      attrib->setMirrorDisabled(never_mirror);
      attrib->setMirrorFlip(mirror_flip);
      attrib->setVisible(visible);
      attrib->setFlags(flag);
      attrib->setLayerIndex(layer);
      attrib->setInherited(false);
      attrib->setHorizontalPosition(intToHorizontalPositionTag(textAlignment));
      attrib->setVerticalPosition(intToVerticalPositionTag(lineAlignment));
   }

   return 0;
}

CString CCEtoODBDoc::getAttributeStringValue(Attrib* attribute)
{
   CString stringValue = attribute->getStringValue();

   //switch (attribute->getValueType())
   //{
   //case valueTypeString:
   //case valueTypeEmailAddress:
   //case valueTypeWebAddress:
   //   {
   //      int stringIndex = attribute->getStringValueIndex();

   //      if (stringIndex >= 0 && stringIndex < ValueArray.GetSize())
   //      {
   //         stringValue = ValueArray.GetAt(stringIndex);
   //      }
   //   }

   //   break;
   //case valueTypeDouble:
   //case valueTypeUnitDouble:
   //   stringValue.Format("%f",attribute->getDoubleValue());
   //   break;
   //case valueTypeInteger:
   //   stringValue.Format("%d",attribute->getIntValue());
   //   break;
   //}

   return stringValue;
}

bool CCEtoODBDoc::getAttributeStringValue(CString& stringValue,CAttributes** attributes,int keywordIndex)
{
   bool retval = false;

   if (attributes != NULL && *attributes != NULL)
   {
      retval = getAttributeStringValue(stringValue,**attributes,keywordIndex);
   }

   return retval;
}

bool CCEtoODBDoc::getAttributeStringValue(CString& stringValue,CAttributes& attributes,int keywordIndex)
{
   bool retval = false;

   Attrib* attribute;

   if (attributes.Lookup(keywordIndex,attribute))
   {
      stringValue = getAttributeStringValue(attribute);
      retval = true;
   }

   return retval;
}

/******************************************************************************
* SetAttrib()
*
*  Description: Sets values for keyword for a data (allocating AttribMap if necessary)
*
*  Parameter : data
*              keyword
*              void *value    --> int*, double*, char* (or NULL for valueTypeNone)
*              method --> SA_OVERWRITE, SA_APPEND, or SA_RETURN
*
*  Returns :   TRUE if keyword existed
*              FALSE if adding new keyword to map
*/
BOOL CCEtoODBDoc::SetAttrib(CAttributes** map, int keyword, int valueType, void *value, int method, Attrib **attribPtr)
{
   // NOTE: in "old" CC files, Keyword Array is filled at the end, so can't index into KeyWordArray yet
   if (keyword == -1)
   {
      ErrorMessage("Wrong Attribute Keyword Register");
      return FALSE;
   }

   if (!LoadingDataFile && valueType != getKeyWordArray()[keyword]->getValueType())
   {
      CString tmp, atttype, keytype;
      atttype = "UNKNOWN";
      keytype = "UNKNOWN";

      switch (valueType)
      {
      case valueTypeString:       atttype = "STRING";       break;
      case valueTypeDouble:       atttype = "DOUBLE";       break;
      case valueTypeUnitDouble:   atttype = "UNIT DOUBLE";  break;
      case valueTypeInteger:      atttype = "INTEGER";      break;
      case valueTypeNone:         atttype = "NONE";         break;
      }

      switch (getKeyWordArray()[keyword]->getValueType())
      {
      case valueTypeString:       keytype = "STRING";       break;
      case valueTypeDouble:       keytype = "DOUBLE";       break;
      case valueTypeUnitDouble:   keytype = "UNIT DOUBLE";  break;
      case valueTypeInteger:      keytype = "INTEGER";      break;
      case valueTypeNone:         keytype = "NONE";         break;
      }

#ifdef _DEBUG
      tmp.Format("Attribute Value Type [%s] - Keyword Value Type [%s]", atttype, keytype);
      ErrorMessage(tmp, "Wrong Attribute Value Type");
#endif
      return FALSE;
   }

   if (*map == NULL)
      *map = new CAttributes();

   BOOL result;
   Attrib* attrib = NULL;

   // is keyword in map
   if ((*map)->Lookup(keyword, attrib))
   {
      if (attribPtr) // this can be NULL
         *attribPtr = attrib;

      result = TRUE;

      if (method == SA_RETURN)
         return TRUE;
   }
   else // doesn't exist, add keyword to map
   {
      attrib = getCamCadData().constructAttribute();

      if (attribPtr) // can be NULL
         *attribPtr = attrib;

      (*map)->SetAt(keyword, attrib);

      result = FALSE;
   }

   switch (valueType)
   {
   case valueTypeInteger:
      attrib->setIntValue(*((int*)value));
      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      attrib->setDoubleValue(*((double*)value));
      break;
   case valueTypeNone:
      attrib->setDoubleValue(0.0);
      break;
   case valueTypeString:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      // already had a string value for this keyword
      if (result &&
         method == SA_APPEND &&
         attrib->getStringValueIndex() != -1)
      {
         CString temp = attrib->getStringValue();

         // same attribute with same keyword with same value already exist - do not double up.
         // result == TRUE means the keyword already exist.
         if (value && temp.CompareNoCase((char *)value) != 0)
         {
            temp += "\n";
            temp += (char*)value;
            attrib->setStringValueIndex(RegisterValue(temp));
         }
         else
         {
            int i=0; // here double
         }
      }
      else // adding a new keyword and value
      {
         if ((char*)value != NULL)
         {
            attrib->setStringValueIndex(RegisterValue((char*)value));
         }
         else
         {
            attrib->setStringValueIndex(RegisterValue((char*)""));
         }
      }
      break;
   }

   if (attrib == NULL)
   {
      return 0;
   }

   attrib->setValueType(valueType);
   attrib->setCoordinate(0.,0.);
   attrib->setRotationRadians(0);
   attrib->setHeight(0);
   attrib->setWidth(0);
   attrib->setProportionalSpacing(false);
   attrib->setPenWidthIndex(0);
   attrib->setMirrorDisabled(false);
   attrib->setVisible(false);
   attrib->setFlags(0);
   attrib->setLayerIndex(0);
   attrib->setInherited(false);
   attrib->setHorizontalPosition(horizontalPositionLeft);
   attrib->setVerticalPosition(verticalPositionBaseline);

   return result;
}

bool CCEtoODBDoc::SetAttrib(CAttributes& attributes, int keyword, int valueType, void *value, int method, Attrib **attribPtr)
{
   CAttributes* pAttributes = &attributes;

   bool retval = (SetAttrib(&pAttributes,keyword,valueType,value,method,attribPtr) != 0);

   return retval;
}

BOOL CCEtoODBDoc::SetAttrib(CAttributes** map, int keyword,int value, int method, Attrib **attribPtr)
{
   return SetAttrib(map,keyword,valueTypeInteger,&value,method,attribPtr);
}

BOOL CCEtoODBDoc::SetAttrib(CAttributes** map, int keyword,float value, int method, Attrib **attribPtr)
{
   double doubleValue = value;
   return SetAttrib(map,keyword,valueTypeUnitDouble,&doubleValue,method,attribPtr);
}

BOOL CCEtoODBDoc::SetAttrib(CAttributes** map, int keyword,double value, int method, Attrib **attribPtr)
{
   return SetAttrib(map,keyword,valueTypeDouble,&value,method,attribPtr);
}

BOOL CCEtoODBDoc::SetAttrib(CAttributes** map, int keyword,const char* value, int method, Attrib **attribPtr)
{
   return SetAttrib(map,keyword,valueTypeString,(void*)value,method,attribPtr);
}

/******************************************************************************
* SetVisAttrib()
*
*  Description: Sets values for keyword for a data (allocating AttribMap if necessary)
*
*  Parameter : data
*              keyword
*              void *value    --> int*, double*, char* (or NULL for valueTypeNone)
*              method --> SA_OVERWRITE, SA_APPEND, or SA_RETURN
*
*  Returns :   TRUE if keyword existed
*              FALSE if adding new keyword to map
*/
BOOL CCEtoODBDoc::SetVisAttrib(CAttributes** map, int keyword,
      int valueType, void *value,
      double x, double y, double rotation, double height, double width, BOOL proportional, int penWidthIndex,
      BOOL visible, int method, DbFlag flag, short layer, BOOL never_mirror, int textAlignment, int lineAlignment,
      BOOL mirror_flip)
{
   Attrib *attrib;
   BOOL result = SetAttrib(map, keyword, valueType, value, method, &attrib);

   attrib->setCoordinate(x,y);
   attrib->setRotationRadians(rotation);
   attrib->setHeight(height);
   attrib->setWidth(width);
   attrib->setProportionalSpacing(proportional);
   attrib->setPenWidthIndex(penWidthIndex);
   attrib->setMirrorDisabled(never_mirror);
   attrib->setMirrorFlip(mirror_flip);
   attrib->setVisible(visible);
   attrib->setFlags(flag);
   attrib->setLayerIndex(layer);
   attrib->setInherited(false);
   attrib->setHorizontalPosition(intToHorizontalPositionTag(textAlignment));
   attrib->setVerticalPosition(intToVerticalPositionTag(lineAlignment));

   return result;
}

/*****************************************************************************/
/*
   Merges attribute with Methode.

   doc->SetVisAttrib(currentAttribMap, keyword, valueType, voidPtr, x, y, rotation, height, width, proportional, penWidthIndex, visible, SA_APPEND, flag, layer, neverMirror);


*/
void CCEtoODBDoc::MergeAttribs(CAttributes** to, CAttributes* from, int Method)
{
   if (from != NULL)
   {
      WORD keyword;
      Attrib* attribute;
      
      for (POSITION pos = from->GetStartPosition();pos != NULL;)
      {
         from->GetNextAssoc(pos, keyword, attribute);
         double doubleValue = attribute->getDoubleValue();

         SetVisAttrib(to, keyword, attribute->getValueType(), (void *)&doubleValue,
            attribute->getX(), attribute->getY(), attribute->getRotationRadians(),
            attribute->getHeight(), attribute->getWidth(), attribute->isProportionallySpaced(),
            attribute->getPenWidthIndex(), attribute->isVisible(), Method, 
            attribute->getFlags(), attribute->getLayerIndex(), attribute->getMirrorDisabled(), 0, 0);

         attribute->setDoubleValue(doubleValue);
      }
   }
}

/* CopyAttribs(CAttributes* to, CAttributes* from)
*
*  Description: copys all attributes
*  Attributes with multiple instances are copied as single instances
*/
void CCEtoODBDoc::CopyAttribs(CAttributes** to, CAttributes* from)
{
   if (from == NULL)
   {
      // do not NULL, because to_attribute list already can have contens
      //*to = NULL;
      return;
   }

   if (*to == NULL)  // to_attribute list can already have contens
   {
      *to = new CAttributes();
   }

   (*to)->CopyAll(*from);

   //WORD keyword;
   //Attrib *toAttrib,*fromAttrib,*oldAttrib;

   //for (POSITION pos = from->GetStartPosition();pos != NULL;)
   //{
   //   from->GetNextAssoc(pos,keyword,(void*&)fromAttrib);

   //   if ((*to)->Lookup(keyword,(void*&)oldAttrib))
   //   {
   //      delete oldAttrib;
   //   }

   //   toAttrib = fromAttrib->allocateCopy();
   //   (*to)->SetAt(keyword,toAttrib);
   //}
}

//--------------------------------------------------------------
// this attributes have to be registered, that they appear in the coice box.
// Keep
void CCEtoODBDoc::RegisterInternalKeywords()
{
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_NAME                         ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_NETNAME                      ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_REFNAME                      ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PATH                         ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_GATEINSTANCENAME             ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_COMPPINNR                    ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_COMPHEIGHT                   ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIALAYER                     ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPANS_GENERAL_CLEARANCE  ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPANS_SAME_NET_CLEARANCE ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPANS_USE_MNT_OPP_PADS   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_FROM_LAYER          ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_TO_LAYER            ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_CAPACITANCE         ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_INDUCTANCE          ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_DELAY               ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_GRID                ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VIA_SPAN_ATTRIBUTES          ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TYPELISTLINK                 ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VALUE                        ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TOLERANCE                    ,keyWordSectionCC, valueTypeDouble     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PLUSTOLERANCE                ,keyWordSectionCC, valueTypeDouble     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_MINUSTOLERANCE               ,keyWordSectionCC, valueTypeDouble     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_VOLTAGE                      ,keyWordSectionCC, valueTypeDouble     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_CURRENT                      ,keyWordSectionCC, valueTypeDouble     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PARTNUMBER                   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DEVICETYPE                   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SUBCLASS                     ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SPEA4040_SUBCLASS            ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TERADYNE1800_SUBCLASS        ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_AEROFLEX_SUBCLASS            ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PINFUNCTION                  ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PINLOGICNAME                 ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_UNPLACED                     ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PACKAGE_FAMILY               ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TECHNOLOGY                   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFT_TARGETTYPE               ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_STRATEGY                ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PART_DESCRIPTION             ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_GEOM_REVIEW_STATUS           ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP        ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_ETCH_TYPE                    ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_BEAD_PROBE                   ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_CB_NETNAME                   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);

   // datalink attribute which refers to a CC entity. Used to attach i.e a ACCESS POINT to
   // a CompPin or Via.
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DDLINK                       ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);

   // needed in GENERATE_PADSTACKS - see description in dbutil.h
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PAD_USAGE                    ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);

   // need in global Draw->Flahs not to convert a Polyline
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_KEEPDRAW                     ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_POWERNET                     ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SINGLEPINNET                 ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(NETATT_SIGNALSTACK               ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);

   // fiducial on both layers
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_FIDUCIAL_PLACE_BOTH          ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);

   // these are padstacks attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(DRILL_NONPLATED                  ,keyWordSectionCC, valueTypeNone       ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(DRILL_STARTLAYER                 ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(DRILL_ENDLAYER                   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);

   // Siemens attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SIEMENS_FIDSYM               ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SIEMENS_THICKNESS_PANEL      ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SIEMENS_THICKNESS_BOARD      ,keyWordSectionCC, valueTypeInteger    ), attribGroupInternal);

   // Gerber Educator attributres
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_GerberEducatorSurface        ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_MergeToolSurface             ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DataType                     ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DataSource                   ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);

   // these are PCB block attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(BOARD_ORIGINAL_SOURCE            ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(BOARD_THICKNESS                  ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(BOARD_NUMBER_OF_LAYERS           ,keyWordSectionCC, valueTypeInteger    ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(BOARD_MATERIAL                   ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_CENTROID_X                   ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_CENTROID_Y                   ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SOLDERMASK                   ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SMDSHAPE                     ,keyWordSectionCC, valueTypeNone       ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_BLINDSHAPE                   ,keyWordSectionCC, valueTypeNone       ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_COMPONENT_PINS_MODIFIED      ,keyWordSectionCC, valueTypeNone       ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_COPPER_THICKNESS          ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_THICKNESS                 ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_MATERIAL                  ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_DIELECTRIC_CONSTANT       ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_RESISTIVITY               ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_ELECTRICAL_CONDUCTIVITY   ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_SHIELD_LAYER              ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(LAYATT_THERMAL_CONDUCTIVITY      ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_LOADED                       ,keyWordSectionCC, valueTypeString     ), attribGroupPcb);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HEIGHT                       ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupPcb);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_KEEPOUT_TYPE                 ,keyWordSectionCC, valueTypeString ),     attribGroupPcb);

   // REALPART
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PACKAGELINK                  ,keyWordSectionCC, valueTypeString     ), attribGroupRealPart);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PIN_PITCH                    ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupRealPart);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PIN_COUNT                    ,keyWordSectionCC, valueTypeInteger    ), attribGroupRealPart);

   // test attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTest )                            ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTestAccess )                      ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTestAccessFail )                  ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTestAccessFailTop )               ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTestAccessFailBottom )            ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTestResource )                    ,keyWordSectionCC, valueTypeString     ), attribGroupTest);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TESTPREFERENCE               ,keyWordSectionCC, valueTypeInteger    ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PINACCESS                    ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_SHORT_TEST                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_IGNORE_OUTLINE          ,keyWordSectionCC, valueTypeNone       ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_CONNECTOR               ,keyWordSectionCC, valueTypeNone       ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_OFFSET_TOP_X            ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_OFFSET_TOP_Y            ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_OFFSET_BOT_X            ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_OFFSET_BOT_Y            ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_NET_STATUS              ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TEST_NET_PROBES              ,keyWordSectionCC, valueTypeInteger    ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_ACCESS_DISTANCE              ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeProbeSize )                      ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeProbePlacement )                 ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeProbeStyle )                     ,keyWordSectionCC, valueTypeString     ), attribGroupTest);

   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeNetType )                        ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributeTrRequired )                     ,keyWordSectionCC, valueTypeInteger    ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName( standardAttributePirRequired )                    ,keyWordSectionCC, valueTypeInteger    ), attribGroupTest);


   // Takaya stuff
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TAKAYA_DEVICECLASS           ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TAKAYA_REFDES                ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TAKAYA_DEVICECLASS_SUBCLASS  ,keyWordSectionCC, valueTypeString     ), attribGroupTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeTakayaAccess)       , keyWordSectionCC, valueTypeString    ), attribGroupTest);

   // Teradyne stuff
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TERADYNE_AOI_DEVICECLASS     ,keyWordSectionCC, valueTypeString     ), attribGroupTest);

   // TRI-MDA/INGUN stuff
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_INGUN_DEVICECLASS            ,keyWordSectionCC, valueTypeString     ), attribGroupTest);

   // insertion maschine process
   getKeyWordArray().SetGroup(RegisterKeyWord(INSERTION_PROCESS                ,keyWordSectionCC, valueTypeString     ), attribGroupAssy);

   // hp5dx attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_TEST                     ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_PACKAGEID                ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_PINFAMILY                ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_ALIGNMENT_1              ,keyWordSectionCC, valueTypeNone       ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_ALIGNMENT_2              ,keyWordSectionCC, valueTypeNone       ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_ALIGNMENT_3              ,keyWordSectionCC, valueTypeNone       ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_VIATEST                  ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_PITCH                    ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_5DX_VIATEST                  ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);

   // hp3070 attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_COMMENT                 ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_COMMON_PIN              ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_CONTACT_PIN             ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_DEVICECLASS             ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_FAIL_MESSAGE            ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_HI_VALUE                ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_LOW_VALUE               ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_NTOL                    ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_PTOL                    ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_TYPE                    ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_VALUE                   ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_PROBESIDE               ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_PROBEACCESS             ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_3070_NETNAME                 ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);

   // AOI attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_AGILENT_AOI_PLACEMENT_MACHINE,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_AGILENT_AOI_PLACEMENT_FEEDER ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_AGILENT_AOI_TEST             ,keyWordSectionCC, valueTypeString     ), attribGroupHpTest);

   // Teradyne 7200
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TERADYNE_7200_TEST           ,keyWordSectionCC, valueTypeString     ), attribGroupTest);

   // Teradyne 7300
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_TERADYNE_AOI_PINTEST         ,keyWordSectionCC, valueTypeString     ), attribGroupTest);

   // HAF
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_REFNAME                  ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_DEVICECLASS              ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_PARTNUMBER               ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_PACKAGE                  ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_VALUE                    ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_PTOL                     ,keyWordSectionCC, valueTypeDouble     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_NTOL                     ,keyWordSectionCC, valueTypeDouble     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_COMPHEIGHT               ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_TEST                     ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_BOUNDARYSCAN             ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_NETPOTENTIAL             ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_PINDIRECTION             ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_PINFUNCTION              ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_HAF_COMMENT                  ,keyWordSectionCC, valueTypeString     ), attribGroupHaf);

  // OGP attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_ReportText               ,keyWordSectionCC, valueTypeString     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_FeatureText              ,keyWordSectionCC, valueTypeString     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_fUpper                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_fLower                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_fPrint                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_fStats                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_xUpper                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_xLower                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_xPrint                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_xStats                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_yUpper                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_yLower                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_yPrint                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_yStats                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_zUpper                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_zLower                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_zPrint                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_zStats                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Tol_zLoc                     ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_Bounds                  ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_Extents                 ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_NearestNominal          ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_First                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_Last                    ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_FirstOf2                ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_LastOf2                 ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_Contrast                ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_WeakStrong              ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_RoughSmooth             ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_PercentFeature          ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Edge_PercentCoverage         ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Light_Profile                ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Light_Surface                ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Light_Ring                   ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_FOV_Inches                   ,keyWordSectionCC, valueTypeDouble     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_FOV_ZoomCounts               ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_PointType                    ,keyWordSectionCC, valueTypeString     ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_Threshold           ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_MultipleValue       ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_Filter              ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_MultipleOn          ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_Fill                ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_TouchBoundary       ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_Centroid_Illumination        ,keyWordSectionCC, valueTypeInteger    ), attribGroupOgp);

   // dfm attributes
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_TESTNAME                 ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_FAILURE_RANGE            ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_CHECK_VALUE              ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_ACTUAL_VALUE             ,keyWordSectionCC, valueTypeUnitDouble ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_LAYERTYPE                ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_LAYERSTACKNUMBER         ,keyWordSectionCC, valueTypeInteger    ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_REF1                     ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_REF2                     ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DIRECTION_OF_TRAVEL          ,keyWordSectionCC, valueTypeString     ), attribGroupInternal);

   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_KEYWORD                  ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_VALUE1                   ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);
   getKeyWordArray().SetGroup(RegisterKeyWord(ATT_DFM_VALUE2                   ,keyWordSectionCC, valueTypeString     ), attribGroupDfm);

   // new keywords
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeComponentOutlineOrientation)    ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeTakayaReferencePointSetting)    ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeSonyAoiSettings)                ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeDebugId)                        ,keyWordSectionCC,valueTypeInteger     ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeStencilGeneratorTempComponent)  ,keyWordSectionCC,valueTypeNone        ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeMiscellaneous)                  ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeEcadBoardOnPanel)               ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);    
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standardAttributeCurrentVariant)                 ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);    
   getKeyWordArray().SetGroup(RegisterKeyWord(StandardAttributeTagToName(standartAttributeSiemensDialogSetting)           ,keyWordSectionCC,valueTypeString      ), attribGroupInternal);    
}

void RemoveAttrib(WORD keyword, CAttributes** attribMap)
{
   if (*attribMap == NULL) return;

   (*attribMap)->deleteAttribute(keyword);

   if ((*attribMap)->GetCount() == 0)
   {
      delete (*attribMap);
      (*attribMap) = NULL;
   }
}

/******************************************************************************
* DeleteOccurancesOfKeyword
*/
void CCEtoODBDoc::DeleteOccurancesOfKeyword(WORD keyword)
{
   // Blocks
   //    Datas
	int i=0;
   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getAttributesRef())
         RemoveAttrib(keyword, &block->getAttributesRef());

      // Datas
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getAttributesRef())
            RemoveAttrib(keyword, &data->getAttributesRef());
      }
   }

   // Files
   //   Types
   //   Nets
   //      CompPins
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      // Types
      POSITION typePos = file->getTypeList().GetHeadPosition();
      while (typePos != NULL)
      {
         TypeStruct *type = file->getTypeList().GetNext(typePos);

         if (type->getAttributesRef())
            RemoveAttrib(keyword, &type->getAttributesRef());
      }

      // Nets
      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (net->getAttributesRef())
            RemoveAttrib(keyword, &net->getAttributesRef());

         // CompPins
         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            if (compPin->getAttributesRef())
               RemoveAttrib(keyword, &compPin->getAttributesRef());
         }
      }

      // Buses
      POSITION busPos = file->getBusList().GetHeadPosition();
      while (busPos != NULL)
      {
         CBusStruct *bus = file->getBusList().GetNext(busPos);
         if (bus != NULL)
            bus->RemoveAttrib(keyword);
      }
   }

   // Layers
   for (i=0; i< getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = getLayerArray()[i];
      if (layer == NULL)   continue;
      if (layer->getAttributesRef())
         RemoveAttrib(keyword, &layer->getAttributesRef());
   }
}

/******************************************************************************
* KeywordMerge
*/
void KeywordMerge(CCEtoODBDoc *doc, CAttributes* map, WORD fromKey, WORD toKey, int method)
{
   // no attribs
   if (map == NULL)
      return;

   // Nothing to do if no "from" in this map
   Attrib* fromAttrib;
   if (!map->Lookup(fromKey, fromAttrib))
      return;

   // If types do not match then apply conversion.
   if (doc->getKeyWordArray()[fromKey]->getValueType() != doc->getKeyWordArray()[toKey]->getValueType())
   {
      fromAttrib->ConvertType(doc->getCamCadData(), toKey);

      // fail safe, this should not happen, but if we got an Append method passed in
      // and destination is not a string type, reset to use Replace method.
      switch (doc->getKeyWordArray()[toKey]->getValueType())
      {
         case valueTypeString:
         case valueTypeEmailAddress:
         case valueTypeWebAddress:
            // Ok
            break;
         default:
            // None of the rest can append, change to replace
            if (method == 2)
               method = 1;
            break;
      }
   }

   fromAttrib = map->removeAttribute(fromKey);
   Attrib* toAttrib = map->removeAttribute(toKey);

   // If no "to" in this map, just save the "from"
   if (toAttrib == NULL)
   {
      map->SetAt(toKey, fromAttrib);
      return;
   }

   // both in this map
   switch (method)
   {
   case 0:  // keep "to" attribute, remove "from" attribute
      map->SetAt(toKey,toAttrib);
      delete fromAttrib;

      break;

   case 1:  // remove "to" attribute, move "from" to "to"
      map->SetAt(toKey,fromAttrib);
      delete toAttrib;

      break;

   case 2:  // combine values, but don't double up if values are already the same
      {
         // Can not use SetAttrib with APPEND arg, because originals are already gone from
         // the attrib map. Append the values here and set the attrib. The from value
         // gets appended after the to value, separator is newline char, this matches
         // what SetAttrib() does for append.
         CString firstVal  = toAttrib->getStringValue();
         CString secondVal = fromAttrib->getStringValue();
         CString combinedVal = firstVal;
         if (firstVal.Compare(secondVal) != 0)
            combinedVal += (CString)"\n" + secondVal;

         toAttrib->setStringValueIndex(doc->RegisterValue(combinedVal));
         map->SetAt(toKey,toAttrib);

         delete fromAttrib;
      }
      break;
   }
}

/******************************************************************************
* MergeOccurancesOfKeyword
*/
void CCEtoODBDoc::MergeOccurancesOfKeyword(WORD from, WORD to, int method)
{
   // Blocks
   //    Datas
	int i=0;
   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);
      if (block == NULL)   continue;

      KeywordMerge(this, block->getAttributesRef(), from, to, method);

      // Datas
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         KeywordMerge(this, data->getAttributesRef(), from, to, method);
      }
   }

   // Files
   //   Types
   //   Nets
   //      CompPins
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      // Types
      POSITION typePos = file->getTypeList().GetHeadPosition();
      while (typePos != NULL)
      {
         TypeStruct *type = file->getTypeList().GetNext(typePos);
         KeywordMerge(this, type->getAttributesRef(), from, to, method);
      }

      // Nets
      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         KeywordMerge(this, net->getAttributesRef(), from, to, method);

         // CompPins
         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);
            KeywordMerge(this, compPin->getAttributesRef(), from, to, method);
         }
      }

      // Buses
      POSITION busPos = file->getBusList().GetHeadPosition();
      while (busPos != NULL)
      {
         CBusStruct *bus = file->getBusList().GetNext(busPos);
         if (bus != NULL)
            KeywordMerge(this, bus->getAttributesRef(), from, to, method);
      }
   }

   // Layers
   for (i=0; i< getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = getLayerArray()[i];
      if (layer == NULL)   continue;
      KeywordMerge(this, layer->getAttributesRef(), from, to, method);
   }
}

/******************************************************************************
* AreThereOccurancesOfKeyword
*/
BOOL CCEtoODBDoc::AreThereOccurancesOfKeyword(WORD keyword)
{
   Attrib* attrib;

   // Blocks
   //    Datas
	int i=0;
   for (i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (block == NULL)   continue;

      if (block->getAttributesRef() && block->getAttributesRef()->Lookup(keyword, attrib))
         return TRUE;

      // Datas
      POSITION dataPos = block->getDataList().GetHeadPosition();

      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
            return TRUE;
      }
   }

   // Files
   //   Types
   //   Nets
   //      CompPins
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      // Types
      POSITION typePos = file->getTypeList().GetHeadPosition();
      while (typePos != NULL)
      {
         TypeStruct *type = file->getTypeList().GetNext(typePos);

         if (type->getAttributesRef() && type->getAttributesRef()->Lookup(keyword, attrib))
            return TRUE;
      }

      // Nets
      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);

         if (net->getAttributesRef() && net->getAttributesRef()->Lookup(keyword, attrib))
            return TRUE;

         // CompPins
         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            if (compPin->getAttributesRef() && compPin->getAttributesRef()->Lookup(keyword, attrib))
               return TRUE;
         }
      }

      // Buses
      POSITION busPos = file->getBusList().GetHeadPosition();
      while (busPos != NULL)
      {
         CBusStruct *bus = file->getBusList().GetNext(busPos);
         attrib = NULL;

         if (bus != NULL && bus->LookUpAttrib(keyword, attrib))
            return TRUE;
      }
   }

   // Layers
   for (i=0; i< getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = getLayerArray()[i];

      if (layer == NULL)   continue;

      if (layer->getAttributesRef() && layer->getAttributesRef()->Lookup(keyword, attrib))
         return TRUE;
   }

   return FALSE;
}

//--------------------------------------------------------------
Attrib* get_attvalue(CAttributes* map, WORD keytok)
{
   if (map == NULL)
      return NULL;

   Attrib* attrib;

   if (!map->Lookup(keytok, attrib))
      return NULL;

   return attrib;
}

//--------------------------------------------------------------
Attrib* is_attvalue(CCEtoODBDoc *doc, CAttributes* map, const char *keywordName, int keywordSection)
{
   if (map == NULL)
      return NULL;

   WORD keyword;

   if ((keyword = doc->IsKeyWord(keywordName, keywordSection)) < 0)  // first keyword can be index 0, no keyword is -1
      return NULL;

   Attrib* attrib;

   if (!map->Lookup(keyword, attrib))
      return NULL;

   return attrib;
}

void CCEtoODBDoc::OnKeywordMap()
{
   KeywordMap dlg;

   dlg.doc = this;

   dlg.DoModal();
}

static int SortBy;
static BOOL Reverse;
int CALLBACK CompareKeywords(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
/////////////////////////////////////////////////////////////////////////////
// KeywordMap dialog
KeywordMap::KeywordMap(CWnd* pParent /*=NULL*/)
   : CResizingDialog(KeywordMap::IDD, pParent)
{
   //{{AFX_DATA_INIT(KeywordMap)
   //}}AFX_DATA_INIT

   addFieldControl(IDC_EDIT        ,anchorBottom);
   addFieldControl(IDC_ADD         ,anchorBottom);
   addFieldControl(IDC_MERGE       ,anchorBottom);
   addFieldControl(IDC_REMOVE      ,anchorBottom);
   addFieldControl(IDC_LOAD        ,anchorBottom);
   addFieldControl(IDC_SAVE        ,anchorBottom);
   addFieldControl(IDOK            ,anchorBottom);
   addFieldControl(IDC_LIST_CONTROL,anchorLeft  ,growBoth);
   addFieldControl(IDC_GROUPS_LB   ,anchorLeft  ,growVertical);
}

KeywordMap::~KeywordMap()
{
/* if (m_imageList)
      delete m_imageList; */
}

void KeywordMap::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(KeywordMap)
   DDX_Control(pDX, IDC_GROUPS_LB, m_groupsLB);
   DDX_Control(pDX, IDC_LIST_CONTROL, m_listCtrl);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(KeywordMap, CResizingDialog)
   //{{AFX_MSG_MAP(KeywordMap)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_LOAD, OnLoad)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CONTROL, OnColumnclickListControl)
   ON_BN_CLICKED(IDC_REMOVE, OnRemove)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST_CONTROL, OnDblclkListControl)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_MERGE, OnMerge)
   ON_LBN_DBLCLK(IDC_LIST1, OnEdit)
   ON_LBN_SELCHANGE(IDC_GROUPS_LB, OnSelchangeGroupsLb)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KeywordMap message handlers
BOOL KeywordMap::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   for (int i=0; i<=AttribGroupTagMax; i++)
   {
      m_groupsLB.SetItemData(m_groupsLB.AddString(attribute_group[i]), i);
      m_groupsLB.SetSel(i);
      groups[i] = TRUE;
   }

   SortBy = -1;
   Reverse = FALSE;

/* // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 2, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_NOTHING));
   m_imageList->Add(app->LoadIcon(IDI_INTERNAL));

   m_listCtrl.SetImageList(m_imageList, LVSIL_SMALL);*/

   // columns
   LV_COLUMN column;
   CRect rect;
   m_listCtrl.GetWindowRect(&rect);
   int width = rect.Width() / 7;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = 2 * width;

   column.pszText = "CAMCAD Keyword";
   column.iSubItem = 0;
   m_listCtrl.InsertColumn(0, &column);

   column.pszText = "Import Keyword";
   column.iSubItem = 1;
   m_listCtrl.InsertColumn(1, &column);

   column.pszText = "Export Keyword";
   column.iSubItem = 2;
   m_listCtrl.InsertColumn(2, &column);

   column.pszText = "Group";
   column.iSubItem = 3;
   column.cx = width;
   m_listCtrl.InsertColumn(3, &column);

   FillListCtrl();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void KeywordMap::OnSelchangeGroupsLb()
{
   int count = m_groupsLB.GetSelCount();
   int *array = (int*)calloc(count, sizeof(int));
   m_groupsLB.GetSelItems(count, array);

	int i=0;
   for (i=0; i<=AttribGroupTagMax; i++)
      groups[i] = FALSE;

   for (i=0; i<count; i++)
      groups[array[i]] = TRUE;

   FillListCtrl();
}

void KeywordMap::FillListCtrl()
{
   int actualItem;

   m_listCtrl.DeleteAllItems();

   for (int i=0; i<doc->getKeyWordArray().GetCount(); i++)
   {
      const KeyWordStruct *kw = doc->getKeyWordArray()[i];
      if (!groups[kw->getGroup()])
         continue;

      actualItem = m_listCtrl.InsertItem(i, kw->getCCKeyword());
      m_listCtrl.SetItemData(actualItem, (LPARAM)i);
      m_listCtrl.SetItemText(actualItem, 1, kw->getInKeyword());
      m_listCtrl.SetItemText(actualItem, 2, kw->getOutKeyword());
      m_listCtrl.SetItemText(actualItem, 3, attribute_group[kw->getGroup()]);
   }

   m_listCtrl.SortItems(CompareKeywords, (DWORD_PTR)doc);
}

void KeywordMap::OnColumnclickListControl(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   if (SortBy == pNMListView->iSubItem)
      Reverse = !Reverse;
   else
      Reverse = FALSE;
   SortBy = pNMListView->iSubItem;
   m_listCtrl.SortItems(CompareKeywords, (DWORD_PTR)doc);

   *pResult = 0;
}

int CALLBACK CompareKeywords(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   int swap = (Reverse?-1:1);

   CCEtoODBDoc *doc  = (CCEtoODBDoc*)lParamSort;
   const KeyWordStruct *kw1 = doc->getKeyWordArray()[lParam1];
   const KeyWordStruct *kw2 = doc->getKeyWordArray()[lParam2];

   switch (SortBy)
   {
   case 1: // in
      return swap * (kw1->getInKeyword().CompareNoCase(kw2->getInKeyword()));
   case 2: // out
      return swap * (kw1->getOutKeyword().CompareNoCase(kw2->getOutKeyword()));
   default:
   case 0: // cc
      return swap * (kw1->getCCKeyword().CompareNoCase(kw2->getCCKeyword()));
   }
}

/*****************************************************************************/
/*
   return 1 if merged
   return 0 if not
*/
static int LoadKeyWordMap(CKeyWordArray& ar, int keyWordCount, CString in, CString cc, CString out,
                    AttribGroupTag group, ValueTypeTag valueType, int inherit)
{
   if (!in.IsEmpty())
   {
      for (int i=0; i<keyWordCount; i++)
      {
         const KeyWordStruct *kwd= ar[i];

         if (!in.Compare(kwd->getInKeyword()))
         {
            ar.SetCCKeyword(i, cc);
            if (!out.IsEmpty())
               ar.SetOutKeyword(i, out);

            // this is doe so that we can still read old keyword files.
            if (group != attribGroupUndefined)
               ar.SetGroup(i, group);

            if (inherit > -1)
               ar.SetInherited(i, (inherit != 0));

            if (valueType != valueTypeUndefined)
               ar.SetValueType(i, valueType);

            return 1;
         }
      }
   }
   else
   {
      for (int i=0; i<keyWordCount; i++)
      {
         if (!cc.Compare(ar[i]->getCCKeyword()))
         {
            ar.SetOutKeyword(i, out);

            // this is doe so that we can still read old keyword files.
            if (group != attribGroupUndefined)
               ar.SetGroup(i, group);

            if (inherit > -1)
               ar.SetInherited(i, (inherit != 0));

            if (valueType != valueTypeUndefined)
               ar.SetValueType(i, valueType);

            return 1;
         }
      }
   }

   // here add it.

   return 0;
}

void KeywordMap::OnLoad()
{
   CStdioFile File;
   CString FileName, buf;

   CFileDialog FileDialog(TRUE, "kwd", "*.kwd",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
         "CAMCAD Keyword Map File (*.kwd)|*.kwd|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   FileName = FileDialog.GetPathName();

   // Open File
   if (!File.Open(FileName, File.modeRead | File.typeText))
   {
      ErrorMessage(FileName, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString  in, cc, out;
   char     *lp, line[1000];  // maxline
   int      found = 0, added = 0;
   int      version = 2;

   // Parse Keyword Map
   while (File.ReadString(buf))
   {
      strcpy(line, buf);
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '!')                            continue;

      if (!STRCMPI(line,"* CAMCAD Keyword File"))
      {
         // old version
         version = 1;
         continue;
      }

      if (version == 1)
      {
         in =lp;
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         cc = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         out = lp;

         if (!LoadKeyWordMap(doc->getKeyWordArray(), doc->getKeyWordArray().GetCount(), in, cc, out,
            attribGroupUndefined,valueTypeUndefined, -1))
         {
            // add a keyword.
            int reg = doc->RegisterKeyWord(cc, 0, 0);

            doc->getKeyWordArray().SetInKeyword(reg, in);
            doc->getKeyWordArray().SetOutKeyword(reg, out);
            doc->getKeyWordArray().SetGroup(reg, attribGroupUser);
            doc->getKeyWordArray().SetInherited(reg, true);
            added++;
         }
         found++;

         continue;
      }


      if (!STRCMPI(lp,".KWD"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         in =lp;
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         cc = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         out = lp;

         int inherit;
         AttribGroupTag group;
         ValueTypeTag   valueType;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         group = intToAttribGroupTag(atoi(lp));

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         valueType = intToValueTypeTag(atoi(lp));

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         inherit = atoi(lp);

         if (!LoadKeyWordMap(doc->getKeyWordArray(), doc->getKeyWordArray().GetCount(), in, cc, out,
            group, valueType, inherit))
         {
            // add a keyword.
            int reg = doc->RegisterKeyWord(cc, 0, valueType);

            doc->getKeyWordArray().SetInKeyword(reg, in);
            doc->getKeyWordArray().SetOutKeyword(reg, out);
            doc->getKeyWordArray().SetGroup(reg, group);
            doc->getKeyWordArray().SetInherited(reg, (inherit != 0));
            added++;
         }
         found++;
      }
   }

   if (version == 1)
   {
      ErrorMessage("This is file is an old format!\nPlease check the values and save it.","Keyword Map Load");
   }

   if (!found)
   {
      ErrorMessage("No Keyword loaded!\nThis may indicate a possible damaged file.Please check the .KWD file format.","Keyword Map Load");
   }

   FillListCtrl();
}

void KeywordMap::OnSave()
{
   FILE *stream;

   CString FileName;
   CFileDialog FileDialog(FALSE, "kwd", "*.kwd",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
         "CAMCAD Keyword Map File (*.kwd)|*.kwd|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   FileName = FileDialog.GetPathName();

   stream = fopen(FileName, "w+t");
   if (stream == NULL)
   {
      ErrorMessage(FileName, "Unable To Open File", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fputs("* CAMCAD Keyword File V2\n\n! Import Keyword, CAMCAD Keyword, Export Keyword\n\n", stream);

   for (int i=0; i<doc->getKeyWordArray().GetCount(); i++)
   {
      const KeyWordStruct *kwd= doc->getKeyWordArray()[i];
      fprintf(stream, ".KWD \"%s\"\t\"%s\"\t\"%s\" %d %d %d\n", kwd->getInKeyword(), kwd->getCCKeyword(), kwd->getOutKeyword(),
         kwd->getGroup(), kwd->getValueType(), (int)(kwd->getInherited()));
   }

   fclose(stream);
}

void KeywordMap::OnMerge()
{
   if (!m_listCtrl.GetSelectedCount())
   {
      ErrorMessage("You must select a keyword", "No keyword selected");
      return;
   }

   int count = m_listCtrl.GetItemCount();
	int selItem=0; 
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_listCtrl.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   WORD from = (WORD)m_listCtrl.GetItemData(selItem);
   const KeyWordStruct *kw = doc->getKeyWordArray()[from];

   MergeKeywordsDlg dlg;
   dlg.doc = doc;
   dlg.from = from;
   dlg.fromString = kw->getCCKeyword();
   dlg.DisableAppend = (kw->getValueType() != valueTypeString);
   if (dlg.DoModal() != IDOK)
      return;

   doc->MergeOccurancesOfKeyword(from, dlg.to, dlg.m_choice);
}

void KeywordMap::OnRemove()
{
   if (!m_listCtrl.GetSelectedCount())
   {
      ErrorMessage("You must select a keyword", "No keyword selected");
      return;
   }

   if (ErrorMessage("Warning - Remove can't be undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone. Do you wish to continue?", "Remove Keyword Occurances", MB_YESNO | MB_DEFBUTTON2)!=IDYES)
      return;

   int count = m_listCtrl.GetItemCount();
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_listCtrl.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   int kwIndex = m_listCtrl.GetItemData(selItem);
   doc->DeleteOccurancesOfKeyword(kwIndex);
}

void KeywordMap::OnEdit()
{
   if (!m_listCtrl.GetSelectedCount())
      return;

   int count = m_listCtrl.GetItemCount();
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_listCtrl.GetItemState(selItem, LVIS_SELECTED))
         break;
   }

   int kwIndex = m_listCtrl.GetItemData(selItem);
   const KeyWordStruct *kw = (const KeyWordStruct*)doc->getKeyWordArray()[kwIndex];
   EditKeywordMap dlg;
   dlg.doc       = doc;
   dlg.m_in      = kw->getInKeyword();
   dlg.m_cc      = kw->getCCKeyword();
   dlg.m_out     = kw->getOutKeyword();
   dlg.group     = kw->getGroup();
   dlg.adding    = FALSE;
   dlg.index     = kwIndex;
   dlg.valueType = kw->getValueType();
   dlg.m_hidden  = kw->getHidden();

   if (dlg.DoModal() == IDOK)
   {
      doc->getKeyWordArray().SetInKeyword(kwIndex, dlg.m_in);
      doc->getKeyWordArray().SetCCKeyword(kwIndex, dlg.m_cc);
      doc->getKeyWordArray().SetOutKeyword(kwIndex, dlg.m_out);
      doc->getKeyWordArray().SetHidden(kwIndex, (dlg.m_hidden != 0));

      if (!kw->getGroup() && kw->getValueType() != dlg.valueType)
      {
         if (doc->AreThereOccurancesOfKeyword(kwIndex))
         {
            ErrorMessage("You Can Not Change the ValueType.\nThere are Occurences of this KeyWord.", "ValueType NOT Changed", MB_ICONEXCLAMATION);
         }
         else
            doc->getKeyWordArray().SetValueType(kwIndex, intToValueTypeTag(dlg.valueType));
      }

      m_listCtrl.SetItemText(selItem, 0, kw->getCCKeyword());
      m_listCtrl.SetItemText(selItem, 1, kw->getInKeyword());
      m_listCtrl.SetItemText(selItem, 2, kw->getOutKeyword());
      m_listCtrl.SetItemText(selItem, 3, attribute_group[kw->getGroup()]);
   }
}

void KeywordMap::OnDblclkListControl(NMHDR* pNMHDR, LRESULT* pResult)
{
   OnEdit();
   *pResult = 0;
}

void KeywordMap::OnAdd()
{
   EditKeywordMap dlg;
   dlg.doc = doc;
   dlg.group = attribGroupUser;
   dlg.adding = TRUE;
   dlg.valueType = valueTypeNone;
   dlg.index = -1;

   if (dlg.DoModal() == IDOK)
   {
      if (dlg.m_cc.IsEmpty())
         return;

      int index = doc->RegisterKeyWord(dlg.m_cc, 0, dlg.valueType);
      doc->getKeyWordArray().SetHidden(index, (dlg.m_hidden != 0));
      const KeyWordStruct* kw = doc->getKeyWordArray()[index];

      int actualItem = m_listCtrl.InsertItem(0, dlg.m_cc);
      m_listCtrl.SetItemData(actualItem, (LPARAM)0);
      m_listCtrl.SetItemText(actualItem, 1, dlg.m_cc);
      m_listCtrl.SetItemText(actualItem, 2, dlg.m_cc);
      m_listCtrl.SetItemText(actualItem, 3, attribute_group[kw->group]);

      m_listCtrl.EnsureVisible(actualItem, FALSE);
      m_listCtrl.SetItemState(actualItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
   }
}

/////////////////////////////////////////////////////////////////////////////
// EditKeywordMap dialog
EditKeywordMap::EditKeywordMap(CWnd* pParent /*=NULL*/)
   : CDialog(EditKeywordMap::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditKeywordMap)
   m_cc = _T("");
   m_in = _T("");
   m_out = _T("");
   m_valueType = -1;
   m_hidden    = 0;
   //}}AFX_DATA_INIT
}

void EditKeywordMap::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditKeywordMap)
   DDX_Text(pDX, IDC_CC, m_cc);
   DDX_Text(pDX, IDC_IN, m_in);
   DDX_Text(pDX, IDC_OUT, m_out);
   DDX_Radio(pDX, IDC_VT_NONE, m_valueType);
   DDX_Check(pDX, IDC_HIDDEN, m_hidden);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditKeywordMap, CDialog)
   //{{AFX_MSG_MAP(EditKeywordMap)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditKeywordMap::OnInitDialog()
{
   CDialog::OnInitDialog();

   switch (valueType)
   {
      case valueTypeNone:
      default:
         m_valueType = 0;
         break;
      case valueTypeString:
         m_valueType = 1;
         break;
      case valueTypeInteger:
         m_valueType = 2;
         break;
      case valueTypeDouble:
         m_valueType = 3;
         break;
      case valueTypeUnitDouble:
         m_valueType = 4;
         break;
      case VT_EMAIL_ADDRESS:
         m_valueType = 5;
         break;
      case VT_WEB_ADDRESS:
         m_valueType = 6;
         break;
   }

   if (group)
   {
      GetDlgItem(IDC_VT_NONE)->EnableWindow(FALSE);
      GetDlgItem(IDC_VT_STRING)->EnableWindow(FALSE);
      GetDlgItem(IDC_VT_INTEGER)->EnableWindow(FALSE);
      GetDlgItem(IDC_VT_DOUBLE)->EnableWindow(FALSE);
      GetDlgItem(IDC_VT_UNIT_DOUBLE)->EnableWindow(FALSE);
      GetDlgItem(IDC_VT_EMAIL)->EnableWindow(FALSE);
      GetDlgItem(IDC_VT_WEB)->EnableWindow(FALSE);
   }

   if (adding)
   {
      GetDlgItem(IDC_IN)->EnableWindow(FALSE);
      GetDlgItem(IDC_OUT)->EnableWindow(FALSE);
   }

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditKeywordMap::OnOK()
{
   UpdateData();

   if (m_cc.IsEmpty())
   {
      ErrorMessage("CAMCAD KeyWord name required");
      return;
   }

   if (!adding && (m_in.IsEmpty() || m_out.IsEmpty()))
   {
      ErrorMessage("KeyWord name required");
      return;
   }

   for (int i=0; i<doc->getKeyWordArray().GetCount(); i++)
   {
      if (i == index) // skip the keyword we're editing (index is -1 if adding a new keyword)
         continue;

      const KeyWordStruct *kw = doc->getKeyWordArray()[i];

      if (!kw->getCCKeyword().CompareNoCase(m_cc))
      {
         ErrorMessage("CAMCAD KeyWord name already used");
         return;
      }

      else if (!kw->getOutKeyword().CompareNoCase(m_out))
      {
         ErrorMessage("Export KeyWord name already used");
         return;
      }

      else if (!kw->getInKeyword().CompareNoCase(m_in))
      {
         ErrorMessage("Import KeyWord name already used");
         return;
      }
   }

   switch (m_valueType)
   {
   case 0:
   default:
      valueType = valueTypeNone;
      break;
   case 1:
      valueType = valueTypeString;
      break;
   case 2:
      valueType = valueTypeInteger;
      break;
   case 3:
      valueType = valueTypeDouble;
      break;
   case 4:
      valueType = valueTypeUnitDouble;
      break;
   case 5:
      valueType = VT_EMAIL_ADDRESS;
      break;
   case 6:
      valueType = VT_WEB_ADDRESS;
      break;
   }

   CDialog::OnOK();
}


/***************************************************************************
*
*/
void InitPens(CDC *pDC, int fillStyle);
void FreePens(CDC *pDC);
void HighlightPins(CDC *pDC, CCEtoODBView *view);
void Block_HighlightByAttrib(CCEtoODBDoc *doc, BlockStruct *bp,
      double insert_x, double insert_y, double rotation, BOOL mirror, double scale);
static WORD keyword;
static int valueType;
static double val;
static BOOL allValues;
/**************************************************************************
* HighlightByAttrib
*/
void CCEtoODBDoc::HighlightByAttrib(BOOL AllValues, WORD Keyword, int ValueType, double Val)
{
   allValues = AllValues;
   keyword = Keyword;
   valueType = ValueType;
   val = Val;

   FileStruct *f;

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      f = getFileList().GetNext(pos);

      if (!f->isShown() || f->isHidden())
         continue;

      if (f->getBlock() == NULL)
         continue;

      double file_rot = f->getRotation();
      if (f->isMirrored()) file_rot = -file_rot;

      Block_HighlightByAttrib(this, f->getBlock(), f->getInsertX(), f->getInsertY(),
                file_rot, f->isMirrored(), f->getScale());

      POSITION viewPos = GetFirstViewPosition();
      if (viewPos)
      {
         CCEtoODBView *view = (CCEtoODBView *)GetNextView(viewPos);

         CClientDC dc(view);
         //view->OnPrepareDC(&dc);

         dc.SetTextAlign(TA_CENTER);
         dc.SetBkColor(RGB(255, 255, 255));
         dc.SetBkMode(TRANSPARENT);

         InitPens(&dc,getSettings().FillStyle);
         HighlightPins(&dc, view);

         FreePens(&dc);
      }
   }
}

void CCEtoODBDoc::UnhighlightByAttrib(BOOL AllValues, WORD Keyword, int ValueType, double Val)
{
   COLORREF color;
   if (!HighlightedNetsMap.Lookup((int)Val, color))
      return;
   HighlightedNetsMap.SetAt((int)Val, -1);

   allValues = AllValues;
   keyword = Keyword;
   valueType = ValueType;
   val = Val;

   FileStruct *f;

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      f = getFileList().GetNext(pos);

      if (!f->isShown() || f->isHidden())
         continue;

      if (f->getBlock() == NULL)
         continue;

      double file_rot = f->getRotation();
      if (f->isMirrored()) file_rot = -file_rot;

      Block_HighlightByAttrib(this, f->getBlock(), f->getInsertX(), f->getInsertY(),
                file_rot, f->isMirrored(), f->getScale());

      POSITION viewPos = GetFirstViewPosition();
      if (viewPos)
      {
         CCEtoODBView *view = (CCEtoODBView *)GetNextView(viewPos);

         CClientDC dc(view);
         //view->OnPrepareDC(&dc);

         dc.SetTextAlign(TA_CENTER);
         dc.SetBkColor(RGB(255, 255, 255));
         dc.SetBkMode(TRANSPARENT);

         InitPens(&dc,getSettings().FillStyle);
         HighlightPins(&dc, view);

         FreePens(&dc);
      }
   }

   HighlightedNetsMap.RemoveKey((int)Val);
}

void Block_HighlightByAttrib(CCEtoODBDoc *doc, BlockStruct *bp,
      double insert_x, double insert_y, double rotation, BOOL mirror, double scale)
{
   Mat2x2 m;
   DataStruct *np;
   Attrib* attrib;
   SelectStruct s;

   RotMat2(&m, rotation);

   POSITION pos;
   pos = bp->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = bp->getDataList().GetNext(pos);

      if (np->getAttributesRef() && np->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getValueType() != valueType)
            continue;

         if (!allValues) // if !allValues, skip this entity if values don't match
         {
            switch (valueType)
            {
            case valueTypeNone:
               break;

            case valueTypeDouble:
               if (val != attrib->getDoubleValue())
                  continue;
               break;

            case valueTypeInteger:
               if ((int)val != attrib->getIntValue())
                  continue;
               break;

            case valueTypeString:
            case VT_EMAIL_ADDRESS:
            case VT_WEB_ADDRESS:
               if ((int)val != attrib->getStringValueIndex())
                  continue;
               break;
            }
         }

         s.setData(np);
         s.setParentDataList(&bp->getDataList());
         s.scale = (DbUnit)scale;
         s.insert_x = (DbUnit)insert_x;
         s.insert_y = (DbUnit)insert_y;
         s.rotation = (DbUnit)rotation;
         s.mirror = mirror;

         if (np->getDataType() == T_INSERT || doc->get_layer_visible(np->getLayerIndex(), mirror))
            doc->DrawEntity(&s, 0, TRUE);
      }
   }
}

//--------------------------------------------------------------
const char *get_attvalue_string(CCEtoODBDoc *doc, Attrib *a)
{
   if (a == NULL)
      return NULL;

   static CString val; // must be initialized here

   val = "";   // blank here, not in the initialize line, because the compiler
               // skips the static init line after the first time.

   switch (a->getValueType())
   {
      case valueTypeInteger:
         val.Format("%d", a->getIntValue());
      break;
      case valueTypeDouble:
         val.Format("%lg", a->getDoubleValue());
      break;
      case valueTypeUnitDouble:
         val.Format("%lg", a->getDoubleValue());
      break;
      case valueTypeString:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         val = a->getStringValue();
      break;
   }

   return val.GetBuffer(0);
}


/////////////////////////////////////////////////////////////////////////////
// OverwriteAttrib dialog
OverwriteAttrib::OverwriteAttrib(CWnd* pParent /*=NULL*/)
   : CDialog(OverwriteAttrib::IDD, pParent)
{
   //{{AFX_DATA_INIT(OverwriteAttrib)
   m_keyword = _T("");
   m_value = _T("");
   //}}AFX_DATA_INIT
}

void OverwriteAttrib::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OverwriteAttrib)
   DDX_Control(pDX, IDC_APPEND, m_appendBtn);
   DDX_Text(pDX, IDC_Keyword, m_keyword);
   DDX_Text(pDX, IDC_VALUE, m_value);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OverwriteAttrib, CDialog)
   //{{AFX_MSG_MAP(OverwriteAttrib)
   ON_BN_CLICKED(IDC_OVERWRITE, OnOverwrite)
   ON_BN_CLICKED(IDC_SKIP, OnSkip)
   ON_BN_CLICKED(IDC_APPEND, OnAppend)
   ON_BN_CLICKED(IDC_APPEND_ALL, OnAppendAll)
   ON_BN_CLICKED(IDC_OVERWRITE_ALL, OnOverwriteAll)
   ON_BN_CLICKED(IDC_SKIP_ALL, OnSkipAll)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OverwriteAttrib message handlers
void OverwriteAttrib::OnCancel()
{
   method = SA_RETURN;
   All = FALSE;
   CDialog::OnOK();
}

void OverwriteAttrib::OnOverwrite()
{
   method = SA_OVERWRITE;
   All = FALSE;
   CDialog::OnOK();
}

void OverwriteAttrib::OnOverwriteAll()
{
   method = SA_OVERWRITE;
   All = TRUE;
   CDialog::OnOK();
}

void OverwriteAttrib::OnSkip()
{
   method = SA_RETURN;
   All = FALSE;
   CDialog::OnOK();
}

void OverwriteAttrib::OnSkipAll()
{
   method = SA_RETURN;
   All = TRUE;
   CDialog::OnOK();
}

void OverwriteAttrib::OnAppend()
{
   method = SA_APPEND;
   All = FALSE;
   CDialog::OnOK();
}

void OverwriteAttrib::OnAppendAll()
{
   method = SA_APPEND;
   All = TRUE;
   CDialog::OnOK();
}

BOOL OverwriteAttrib::OnInitDialog()
{
   CDialog::OnInitDialog();

   if (valueType != valueTypeString)
      m_appendBtn.EnableWindow(FALSE);

   switch (valueType)
   {
   case valueTypeString:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      {
         m_value = "Value : ";
         if (attrib->getStringValueIndex() < 0)
         {
            m_value += "\"\"";
         }
         else
         {
            char *tok, *temp = STRDUP(attrib->getStringValue());
            if ((tok = strtok(temp, "\n")) == NULL)
            {
               m_value += "\"\"";
            }
            else
            {
               m_value += "\"";
               m_value += tok;
               m_value += "\"";
               tok = strtok(NULL, "\n");
               while (tok != NULL)
               {
                  m_value += ",\"";
                  m_value += tok;
                  m_value += "\"";
                  tok = strtok(NULL, "\n");
               }
            }
            free(temp);
         }
      }
      break;
   case valueTypeInteger:
      m_value.Format("Value : %d", attrib->getIntValue());
      break;
   case valueTypeUnitDouble:
      m_value.Format("Value : %.*lf", GetDecimals(doc->getSettings().getPageUnits()), attrib->getDoubleValue());
      break;
   case valueTypeDouble:
      m_value.Format("Value : %lg", attrib->getDoubleValue());
      break;
   }

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// MergeKeywordsDlg dialog
MergeKeywordsDlg::MergeKeywordsDlg(CWnd* pParent /*=NULL*/)
   : CDialog(MergeKeywordsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(MergeKeywordsDlg)
   m_choice = 0;
   //}}AFX_DATA_INIT
}

void MergeKeywordsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MergeKeywordsDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Radio(pDX, IDC_RADIO1, m_choice);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MergeKeywordsDlg, CDialog)
   //{{AFX_MSG_MAP(MergeKeywordsDlg)
   ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeKWList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void MergeKeywordsDlg::OnSelchangeKWList() 
{
   to = (WORD)m_list.GetItemData(m_list.GetCurSel());
   CString toKwName = doc->getKeyWordArray()[to]->cc;

   CString buf;
   buf.Format("Keep value from \"%s\"", toKwName);
   GetDlgItem(IDC_RADIO1)->SetWindowText(buf);

   int valType = doc->getKeyWordArray()[to]->getValueType();
   if (!DisableAppend && (valType == valueTypeString || valType == VT_EMAIL_ADDRESS || valType == VT_WEB_ADDRESS))
   {
      GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
   }
   else
   {
      UpdateData(TRUE);
      if (m_choice == 2)
      {
         m_choice = 1;  // change setting from choice being disabled
         UpdateData(FALSE);
      }
      GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
   }
}

/////////////////////////////////////////////////////////////////////////////
// MergeKeywordsDlg message handlers
BOOL MergeKeywordsDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   for (WORD i=0; i<doc->getKeyWordArray().GetCount(); i++)
   {
      if (i == from)
         continue;

      m_list.SetItemData(m_list.AddString(doc->getKeyWordArray()[i]->cc), i);
   }

   CString buf;
   buf.Format("Overwrite with value from \"%s\"", fromString);
   GetDlgItem(IDC_RADIO2)->SetWindowText(buf);

   // The append button is always initially disabled.
   // It will become enabled when a destination of type string is selected, if incoming was a string.
   GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void MergeKeywordsDlg::OnOK()
{
   if (!m_list.GetSelCount())
      return;

   to = (WORD)m_list.GetItemData(m_list.GetCurSel());

   CDialog::OnOK();
}


static Attrib *moveAttrib;
/******************************************************************************
* OnMoveAttrib
*/
void CCEtoODBView::OnMoveAttrib()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   CCEtoODBDoc *doc = GetDocument();
   if (cursorMode == MoveAttrib)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   lastCommand = LC_MoveVisAttrib;

   if (doc->nothingIsSelected())
   {
      ErrorMessage("Nothing is Selected.");
      return;
   }

   SelectStruct *s = doc->getSelectStack().getAtLevel();

   if (s->getData()->getDataType() != T_INSERT)
   {
      ErrorMessage("Select an Insert");
      return;
   }

   if (!s->getData()->getAttributesRef())
   {
      ErrorMessage("No Attributes");
      return;
   }

   Attrib* attribute;
   WORD key;
   int count = 0;
   
   for (POSITION pos = s->getData()->getAttributesRef()->GetStartPosition();pos != NULL;)
   {
      s->getData()->getAttributesRef()->GetNextAssoc(pos, key, attribute);

      if (attribute->isVisible() && attribute->getValueType() == valueTypeString)
      {
         moveAttrib = attribute;
         count++;
      }
   }

   if (!count)
   {
      ErrorMessage("No Visible Attributes");
      return;
   }

   if (count > 1)
   {
      MoveAttribDlg dlg;
      dlg.doc = doc;
      dlg.map = s->getData()->getAttributesRef();

      if (dlg.DoModal() != IDOK)
         return;
   }

   InsertModifyUndo(doc, FALSE);

   if (StatusBar)
      StatusBar->SetWindowText("Pick Reference Point to Move Attribute on Selected Entitiy");

   cursorAnchored = FALSE;
   cursorFirstDrag = TRUE;
   //HideSearchCursor();
   cursorMode = MoveAttrib;
   //ShowSearchCursor();
}

void CCEtoODBView::OnMouseMove_MoveAttrib(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->getSelectStack().getAtLevel();

   doc->DrawEntity(s, -2, FALSE);

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
   Point2 tempPnt;
   tempPnt.x = factor * (pnt->x - cursorPrevPnt.x);
   tempPnt.y = factor * (pnt->y - cursorPrevPnt.y);

   tempPnt.x += s->insert_x;
   tempPnt.y += s->insert_y;

   SelectStruct tempS(*(doc->getSelectStack().getAtLevel()));
   //memcpy(&tempS, doc->SubSelectArray[doc->subSelectLevel], sizeof(SelectStruct));

   tempS.scale *= s->getData()->getInsert()->getScale();

   if (tempS.mirror & MIRROR_FLIP)
      tempS.rotation = tempS.rotation - s->getData()->getInsert()->getAngle();
   else
      tempS.rotation = tempS.rotation + s->getData()->getInsert()->getAngle();

   Untransform(doc, &tempPnt, &tempS);

   tempS.mirror ^= doc->getBottomView() & MIRROR_FLIP;

   moveAttrib->offsetCoordinate(((tempS.mirror ^ s->getData()->getInsert()->getMirrorFlags()) & MIRROR_FLIP) ? -tempPnt.x : tempPnt.x,
      tempPnt.y);

   doc->DrawEntity(s, -2, FALSE);

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;
}

void CCEtoODBView::OnLButtonDown_MoveAttrib(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->getSelectStack().getAtLevel();

   if (!cursorAnchored) // if picking anchor point
   {
      cursorAnchored = TRUE;
      cursorPrevPnt.x = pnt->x;
      cursorPrevPnt.y = pnt->y;

      doc->DrawEntity(s, -1, FALSE);
      doc->DrawEntity(s, -2, FALSE);
   }
   else
   {
      doc->DrawEntity(s, 3, FALSE);

      FileStruct *file = doc->Find_File(s->filenum);
      if (file)
         file->getBlock()->resetExtent();

      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
   }
}


/////////////////////////////////////////////////////////////////////////////
// MoveAttribDlg dialog
MoveAttribDlg::MoveAttribDlg(CWnd* pParent /*=NULL*/)
   : CDialog(MoveAttribDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(MoveAttribDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void MoveAttribDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MoveAttribDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MoveAttribDlg, CDialog)
   //{{AFX_MSG_MAP(MoveAttribDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MoveAttribDlg message handlers
BOOL MoveAttribDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   Attrib* attribute;
   WORD key;
   int count = 0;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, key, attribute);

      if (attribute->isVisible() && attribute->getValueType() == valueTypeString)
      {
         CString buf;
         buf.Format("%s => %s", doc->getKeyWordArray()[key]->cc, attribute->getStringValue());
         m_list.SetItemDataPtr(m_list.AddString(buf), attribute);
      }
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void MoveAttribDlg::OnOK()
{
   int sel = m_list.GetCurSel();
   if (sel == LB_ERR)
      return;

   moveAttrib = (Attrib*)m_list.GetItemDataPtr(sel);

   CDialog::OnOK();
}

