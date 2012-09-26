
#include "StdAfx.h"
#include "DcaCamCadFileWriter.h"
#include "DcaXmlContent.h"
#include "DcaCamCadData.h"
#include "DcaDataType.h"
#include "DcaHorizontalPosition.h"
#include "DcaVerticalPosition.h"
#include "DcaAttributes.h"
#include "DcaDftCommon.h"
#include "DcaLayerType.h"
#include "DcaEntityNumber.h"
#include "DcaPoly.h"
#include "DcaFillStyle.h"
#include "DcaPoint.h"
#include "DcaInsert.h"
#include "DcaText.h"
#include "DcaBlob.h"
#include "DcaFileType.h"
#include "DcaVariant.h"
#include "DcaUnits.h"


//#define ImplementEsotericCcSections

#define NO_INTELLIGENCE_MSG \
   "Warning:  CAMCAD Graphic will not read the intelligent portion of the ECAD\
data.  Elements such as the Netlist, BusList, VariantList, Typelist and NETNAME attribues will not be loaded.\
\
Do you want to continue?"

extern int Product;

// todo -- knv 20070316 - The NextFileNum global is no longer maintained by the dca - check repurcussions
//extern int NextFileNum; // from GRAPH.CPP
//extern CString CCPath; // from DATAFILE.CPP

/******************************************************************************
* CDcaXmlContent
*/
CDcaXmlContent::CDcaXmlContent(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_currentBlock(NULL)
{
   m_currentAttribMap = lastAttribMap = NULL;
   m_currentFile      = NULL;
   currentData        = NULL;
   m_currentDataList  = NULL;
   currentPntList     = NULL;
   currentBusList     = NULL;
   currentBus         = NULL;
   currentVariantList = NULL;
   currentVariant     = NULL;
   currentNetList     = NULL;
   currentPolyList    = NULL;
   currentTypeList    = NULL;
   currentDRCList     = NULL;
   currentDRC         = NULL;
   currentCompPinList = NULL;
   currentNamedView   = NULL;
   currentTableList   = NULL;
   currentTable       = NULL;
   currentTableRow    = NULL;
   currentPoly3DList    = NULL;
   currentPnt3DList     = NULL;

   askLoadNonGrahpicData = (Product == PRODUCT_GRAPHIC);

   currentMachineName = "";
}

/******************************************************************************
* ~CDcaXmlContent
*/
CDcaXmlContent::~CDcaXmlContent()
{
}

/******************************************************************************
* SetDoc
*/
//void CDcaXmlContent::SetDoc(CCEtoODBDoc* ccDoc)
//{
//   doc = ccDoc;
//   netKeywordIndex = getCamCadData().IsKeyWord(ATT_NETNAME, 0);
//}

/******************************************************************************
* GetDoc
*/
//CCEtoODBDoc* CDcaXmlContent::GetDoc()
//{
//   return doc;
//}

/******************************************************************************
* startElement
*/
// Miten -- This function will fill in the datastructure based on the keyword along with the set of attributes
BOOL CDcaXmlContent::startElement(CString localTagName, CMapStringToString* tAttributes)
{
   CString localName = localTagName;
   CMapStringToString* pAttributes = tAttributes;

   char c = localName[0];

   switch (c)
   {
   case 'C':
      if (localName == "CCDoc")
      {
         CString attValue;
         if (GetAttribute(pAttributes, "application", &attValue) && !attValue.CompareNoCase("CAMCAD.NET"))
         {
            // warning is removed 6/1/2005.. ColorSets and Topics sections are added.  
            /*CString tmp = (CString)"This CC File is generated from visECAD.  Collaboration notes in\n"
            + "the CC file will NOT be imported.\n\n"
            + "Do you want to continue reading this file?";
            if (ErrorMessage(tmp, "Information:", MB_ICONINFORMATION|MB_YESNO) == IDNO)
            {
            UserCancel = TRUE;
            return FALSE;
            }*/
         }
         else if (GetAttribute(pAttributes, "version", &attValue))
         {
            CString supportedDocVersion = DOC_VERSION;
            int supportedMajor = atoi(supportedDocVersion.Mid(0, supportedDocVersion.Find('.')));
            int supportedMinor = atoi(supportedDocVersion.Mid(supportedDocVersion.Find('.')+1));
            int major = atoi(attValue.Mid(0, attValue.Find('.')));
            int minor = atoi(attValue.Mid(attValue.Find('.')+1));

            if (major < 6)
            {
               CString  tmp;
               tmp.Format("CC File Version %s is not supported!", attValue);
               ErrorMessage(tmp, "XML Read Error");
               return FALSE;
            }
            else if (major > supportedMajor || (major == supportedMajor && minor > supportedMinor))
            {
               //		This message is to indicate that the version of the file being read in likely has
               //	something in it that this version of CAMCAD does not support appropriately, or at all.
               // The message below is too specific to be appropriate for the user.  Not all versions of
               //	the CC file with versions not compatible will likely just have problems with collaboration
               //	notes.
               //
               //	"Collaboration notes in the CC file will NOT be imported."
               //	"This CC File is generated from visECAD or a later version of CAMCAD.  Therefore, the collaboration data may be affected."
               //
               //	DO NOT change this message to be more specific.  It is to be generic.
               CString tmp = (CString)"This is a newer CC File!  CAMCAD may not read this file completely.\n"
                  + "Overwriting this file will revert it to an earlier version and may result in lost data.\n\n"
                  + "Do you want to continue reading this file?";
               if (ErrorMessage(tmp, "Information:", MB_ICONINFORMATION|MB_YESNO) == IDNO)
               {
                  UserCancel = TRUE;
                  return FALSE;
               }
            }
         }
      }
      else if (localName == "CompPin")
      {
         if (askLoadNonGrahpicData && Product == PRODUCT_GRAPHIC)
         { // Graphic doesn't read intelligent info
            CString buf = NO_INTELLIGENCE_MSG;
            if (ErrorMessage(buf, "CC File Read Warning", MB_YESNO) == IDNO)
               return FALSE;
            else
               askLoadNonGrahpicData = FALSE;
         }

         AddCompPin(pAttributes);
      }
      else if (localName == "Cloud")
         AddTopicNode(pAttributes, T_Cloud);
      else if (localName == "Cell")
         AddTableRowCell(pAttributes);
      else if (localName == "Col")
         AddTableCol(pAttributes);
      break;

   case 'A':
      if (localName == "Attrib")
      {
         CString attValue;

         if (!GetAttribute(pAttributes, "key", &attValue))
         {
            return TRUE;
         }

         WORD keyword = atoi(attValue);

         if (askLoadNonGrahpicData && netKeywordIndex == keyword && Product == PRODUCT_GRAPHIC)
         { // Graphic doesn't read intelligent info
            CString buf = NO_INTELLIGENCE_MSG;
            if (ErrorMessage(buf, "CC File Read Warning", MB_YESNO) == IDNO)
            {
               return FALSE;
            }
            else
            {
               askLoadNonGrahpicData = FALSE;
            }
         }

         AddAttrib(pAttributes);
      }
      break;

   case 'B':
      if (localName == "Blob")
         AddData(pAttributes,dataTypeBlob);
      else if (localName == "BackgroundBitmapTop")
         AddBackgroundBitmap(pAttributes, 1);
      else if (localName == "BackgroundBitmapBottom")
         AddBackgroundBitmap(pAttributes, 0);
      else if (localName == "Bus")
      {
         if (askLoadNonGrahpicData && Product == PRODUCT_GRAPHIC)
         { // Graphic doesn't read intelligent info
            CString buf = NO_INTELLIGENCE_MSG;
            if (ErrorMessage(buf, "CC File Read Warning", MB_YESNO) == IDNO)
               return FALSE;
            else
               askLoadNonGrahpicData = FALSE;
         }

         AddBus(pAttributes);
      }
      else if (localName  == "BusRef")
         AddBusRef(pAttributes);
      break;

   case 'D':
      if (localName == "DRC")
         AddDRC(pAttributes);
      else if (localName == "DRC_Measure")
         AddDRC_Measure(pAttributes);
      else if (localName == "DRC_Net")
         AddDRC_Net(pAttributes);
      else if (localName == "DRC_Algorithm")
         AddDRC_Algorithm(pAttributes);
      else if(localName == "Dimension")
         AddTopicNode(pAttributes, T_Dimension);
      break;
   case 'E':
      if(localName == "Ellipse")
         AddTopicNode(pAttributes, T_Ellipse);
      break;
   case 'F':
      if (localName == "File")
         AddFile(pAttributes);
      else if(localName == "FreeText")
         AddTopicNode(pAttributes, T_FreeText);
      else if(localName == "FreehandDraw")
         AddTopicNode(pAttributes, T_FreehandDraw);
      break;

   case 'G':
      if (localName == "Geometry")
         AddGeometry(pAttributes);
      break;

   case 'I':
      if (localName == "Insert")
         AddData(pAttributes,dataTypeInsert);
      else if(localName == "Issue")
         AddIssue(pAttributes);
      break;

   case 'K':
      if (localName == "Keyword")
         AddKeyword(pAttributes);
      break;

   case 'L':
      if (localName == "Layer")
         AddLayer(pAttributes);
      else if(localName == "Line")
         AddTopicNode(pAttributes, T_Line);
      else if(localName == "LeaderNote")
         AddTopicNode(pAttributes, T_LeaderNote);
      break;

   case 'N':
      if (localName == "Net")
      {
         if (askLoadNonGrahpicData && Product == PRODUCT_GRAPHIC)
         { // Graphic doesn't read intelligent info
            CString buf = NO_INTELLIGENCE_MSG;
            if (ErrorMessage(buf, "CC File Read Warning", MB_YESNO) == IDNO)
               return FALSE;
            else
               askLoadNonGrahpicData = FALSE;
         }

         AddNet(pAttributes);
      }
      else if (localName == "NamedView")
         AddNamedView(pAttributes);
      else if (localName == "NamedViewLayer")
         AddNamedViewLayer(pAttributes);
      else if (localName  == "NetRef")
         AddNetRef(pAttributes);
      break;

   case 'P':
      if (localName == "Pnt")
         AddPnt(pAttributes);
      else if (localName == "Poly")
         AddPoly(pAttributes);
      else if (localName == "PolyStruct")
         AddData(pAttributes,dataTypePoly);
      else if (localName == "Point")
         AddData(pAttributes,dataTypePoint);
      else if(localName == "Polyline")
         AddTopicNode(pAttributes, T_Polyline);
      else if (localName == "PolyStruct3D")
         AddData(pAttributes,dataTypePoly3D);
      else if (localName == "Poly3D")
         AddPoly3D(pAttributes);
      else if (localName == "Pnt3D")
         AddPnt3D(pAttributes);
      break;
   case 'R':
      if(localName == "Rectangle")
         AddTopicNode(pAttributes, T_Rectangle);
      else if(localName == "Row")
         AddTableRow(pAttributes);
      break;
   case 'S':
      if (localName == "Settings")
         AddSettings(pAttributes);
      else if (localName == "StickyNote")
         AddTopicNode(pAttributes, T_StickNote);
      break;

   case 'T':
      if (localName == "Table")
      {
         AddTable(pAttributes);
      }
      else if (localName == "Text")
      {
         AddData(pAttributes,dataTypeText);
      }
      else if (localName == "Type")
      {
         if (askLoadNonGrahpicData && Product == PRODUCT_GRAPHIC)
         {
            CString buf = NO_INTELLIGENCE_MSG;
            if (ErrorMessage(buf, "CC File Read Warning", MB_YESNO) == IDNO)
               return FALSE;
            else
               askLoadNonGrahpicData = FALSE;
         }

         AddType(pAttributes);
      }
      else if (localName == "Topic")
         AddTopic(pAttributes);
      break;

   case 'V':
      if (localName == "Variant")
      {
         if (askLoadNonGrahpicData && Product == PRODUCT_GRAPHIC)
         { // Graphic doesn't read intelligent info
            CString buf = NO_INTELLIGENCE_MSG;
            if (ErrorMessage(buf, "CC File Read Warning", MB_YESNO) == IDNO)
               return FALSE;
            else
               askLoadNonGrahpicData = FALSE;
         }

         AddVariant(pAttributes);
      }
      else if (localName == "VariantItem")
         AddVariantItem(pAttributes);

   case 'W':
      if (localName == "Width")
         AddWidth(pAttributes);
      break;
   case 'X':
      if (localName == "XOutRectangle")
         AddTopicNode(pAttributes, T_XOutRectangle);
      else if (localName == "XOutEllipse")
         AddTopicNode(pAttributes, T_XOutEllipse);
      break;
   }

   return TRUE;
}

/******************************************************************************
* endElement
*/
void CDcaXmlContent::endElement(CString localTagName)
{
   CString localName = localTagName;

   if (localName == "Geometry")
      m_currentDataList = NULL;
   else if (localName == "PolyStruct")
      currentPolyList = NULL;
   else if (localName == "Poly")
      currentPntList = NULL;
   else if (localName == "Text")
      currentData = NULL;
   else if (localName == "Bus")
      currentBus = NULL;
   else if (localName == "Tables")
      currentTableList = NULL;
   else if (localName == "Table")
      currentTable = NULL;
   else if (localName == "Row")
      currentTableRow = NULL;
   else if (localName == "PolyStruct3D")
      currentPoly3DList = NULL;
   else if (localName == "Poly3D")
      currentPnt3DList = NULL;
   else if (localName == "File")
   {
#ifdef ImplementEsotericCcSections
      if (!currentMachineName.IsEmpty())
         getCamCadData().SetCurrentMachine(*currentFile,getCamCadData().FindMachine(*currentFile,currentMachineName));
#endif

      m_currentFile = NULL;
      currentTableList   = NULL;
      currentMachineName = "";
   }
   else if(localName == "Polyline" || localName == "FreehandDraw")
   {
      currentPntList = NULL;
   }

   if (localName == "CompPin" || localName == "PolyStruct" || localName == "Insert" || localName == "Table")
   {
      m_currentAttribMap = lastAttribMap;
      lastAttribMap = NULL;
   }
}

/******************************************************************************
* SwapCStyleEscapeSequences
*/
void CDcaXmlContent::SwapEscapeSequences(CString &strVal, bool escToChar)
{
   // Handle C-style and Xml escape for \ and newline -> from 4.6
   // escToChar == true == swap escapes sequence for internal char  (INPUT : used for reading file)
   // escToChar == false == swap internal char for escape sequence (OUTPUT : used for writing file)

   if(escToChar)
   {
      // C-Style escape
      for (int i = -1;(i = strVal.Find('\\', i+1)) != -1;)
      {
         switch (strVal[i+1])
         {
         case '\\':
            strVal.Delete(i);
            break;

         case 'n':
            strVal.Delete(i);
            strVal.SetAt(i, '\n');
            break;
         }
      }

      // XML escape 
      strVal.Replace("&#10;", "\n");
   }
   else
   {
      strVal.Replace("\\", "\\\\");
      strVal.Replace("\n", "\\n");
   }
}

/******************************************************************************
* GetAttribute
*/
BOOL CDcaXmlContent::GetAttribute(CMapStringToString* attributes, CString attName, CString* attValue)
{
   return attributes->Lookup(attName,*attValue);
}

/******************************************************************************
* AddAttrib
*/
void CDcaXmlContent::AddAttrib(CMapStringToString* attributes)
{
   if (m_currentBlock != NULL && m_currentBlock->getBlockNumber() == 3)
   {
      int iii = 3;
   }

   if (m_currentAttribMap == NULL)
   {
      return;
   }

   CString attValue;

   if (!GetAttribute(attributes, "key", &attValue))
   {
      return;
   }

   WORD keyword = atoi(attValue);
   WORD origKW;

   if (m_remappingMap.Lookup(keyword, origKW))
      keyword = origKW;

   if (netKeywordIndex == keyword && Product == PRODUCT_GRAPHIC)
   {
      return;  // Graphic doesn't read intelligent info
   }

   CString val;

   if (GetAttribute(attributes, "val", &attValue))
   {
      val = attValue;
   }

   ValueTypeTag valueType = getCamCadData().getKeyWordArray()[keyword]->getValueType();

   void* voidPtr;
   int intValue;
   double doubleValue;

   switch (valueType)
   {
   case valueTypeNone:
      voidPtr = NULL;
      break;
   case valueTypeString:
   case valueTypeEmailAddress:
   case valueTypeWebAddress:
      SwapEscapeSequences(val, true);
      voidPtr = (void*)(LPCTSTR)val;
      break;
   case valueTypeInteger:
      intValue = atoi(val);
      voidPtr = (void*)&intValue;
      break;
   case valueTypeDouble:
   case valueTypeUnitDouble:
      doubleValue = atof(val);
      voidPtr = (void*)&doubleValue;
      break;
   }

   double x = 0.;
   double y = 0.;
   double rotation = 0.;
   double height = 0.;
   double width = 0.;
   int penWidthIndex = 0;
   int layer = -1;
   bool visible = false;
   bool proportional = false;
   bool neverMirror = false;
   bool mirrorFlip = false;
   HorizontalPositionTag horizontalPosition = horizontalPositionLeft;
   VerticalPositionTag verticalPosition     = verticalPositionBaseline;
   DbFlag flag = 0;

   if (GetAttribute(attributes, "flag", &attValue))
      flag = atol(attValue);

   if (GetAttribute(attributes, "visible", &attValue))
      visible = (atoi(attValue) != 0);

   if (GetAttribute(attributes, "x", &attValue))
      x = atof(attValue);

   if (GetAttribute(attributes, "y", &attValue))
      y = atof(attValue);

   if (GetAttribute(attributes, "height", &attValue))
      height = atof(attValue);

   if (GetAttribute(attributes, "width", &attValue))
      width = atof(attValue);

   if (GetAttribute(attributes, "penWidthIndex", &attValue))
      penWidthIndex = atoi(attValue);

   if (GetAttribute(attributes, "proportional", &attValue))
      proportional = (atoi(attValue) != 0);

   if (GetAttribute(attributes, "rotation", &attValue))
      rotation = atof(attValue);

   if (GetAttribute(attributes, "layer", &attValue))
      layer = atoi(attValue);

   if (GetAttribute(attributes, "neverMirror", &attValue))
      neverMirror = (atoi(attValue) != 0);

   if (GetAttribute(attributes, "mirror", &attValue)) // only supports mirror flip, not mirror layer, on purpose
      mirrorFlip = (atoi(attValue) != 0);

   if (GetAttribute(attributes, "textAlignment", &attValue))
      horizontalPosition = intToHorizontalPositionTag(atoi(attValue));

   if (GetAttribute(attributes, "lineAlignment", &attValue))
      verticalPosition = intToVerticalPositionTag(atoi(attValue));


   // New font stuff
   // facename="VeriBest Gerber 0" fonttype="2" weight="400" underlined="0" strikeout="0" orientation="0" charset="0" pitchAndFamily="0">
   CString fntfacename;
   CString fntfonttype;
   CString fntweight;
   CString fntunderlined;
   CString fntstrikeout;
   CString fntorientation;
   CString fntcharset;
   CString fntpitchnfamily;
   if (GetAttribute(attributes, "facename", &attValue))
      fntfacename = attValue; 

   if (GetAttribute(attributes, "fonttype", &attValue))
      fntfonttype = attValue;

   if (GetAttribute(attributes, "weight", &attValue))
      fntweight = attValue;

   if (GetAttribute(attributes, "underlined", &attValue))
      fntunderlined = attValue;

   if (GetAttribute(attributes, "strikeout", &attValue))
      fntstrikeout = attValue;

   if (GetAttribute(attributes, "orientation", &attValue))
      fntorientation = attValue;

   if (GetAttribute(attributes, "charset", &attValue))
      fntcharset = attValue;

   if (GetAttribute(attributes, "pitchAndFamily", &attValue))
      fntpitchnfamily = attValue;


   // Special handling for PROBE_PLACEMENT and TEST_RESOURCE attributes.
   // We added these two attribs earlier on in order to accomplish backward
   // compatibility with probes in old cc file. The attribs added were not
   // from the CC file. So if we find some now in the CC file, we do not
   // want to append them as duplicates, as we would in the general case
   // for duplicate attribs. We just want to alter the previously added
   // value to match what we now find in CC file attribs.
   // Fixes case 1452.

   const KeyWordStruct* keywordStruct = getCamCadData().getKeyWordArray()[keyword];
   CString cckeyword = keywordStruct->getCCKeyword();

   bool overwrite = (cckeyword.CompareNoCase(StandardAttributeTagToName(standardAttributeProbePlacement)) == 0) ||
      (cckeyword.CompareNoCase(StandardAttributeTagToName(standardAttributeTestResource)) == 0);

   CAttribute* existingAttrib;

   if (!overwrite && *m_currentAttribMap != NULL && (*m_currentAttribMap)->Lookup(keyword,existingAttrib))
   {
      CAttribute* duplicateAttribute = getCamCadData().constructAttribute();

      duplicateAttribute->setCoordinate(x,y);
      duplicateAttribute->setRotationRadians(rotation);
      duplicateAttribute->setHeight(height);
      duplicateAttribute->setWidth(width);
      duplicateAttribute->setProportionalSpacing(proportional);
      duplicateAttribute->setPenWidthIndex(penWidthIndex);
      duplicateAttribute->setMirrorDisabled(neverMirror);
      duplicateAttribute->setMirrorFlip(mirrorFlip);
      duplicateAttribute->setVisible(visible);
      duplicateAttribute->setFlags(flag);
      duplicateAttribute->setLayerIndex(layer);
      duplicateAttribute->setInherited(false);
      duplicateAttribute->setHorizontalPosition(horizontalPosition);
      duplicateAttribute->setVerticalPosition(verticalPosition);

      if (!fntfacename.IsEmpty() || 
         !fntfonttype.IsEmpty() ||
         !fntweight.IsEmpty() ||
         !fntunderlined.IsEmpty() ||
         !fntstrikeout.IsEmpty() ||
         !fntorientation.IsEmpty() ||
         !fntcharset.IsEmpty() ||
         !fntpitchnfamily.IsEmpty())
      {
         CFontProperties *fontProps = duplicateAttribute->GetDefinedFontProperties();
         fontProps->SetFaceName(fntfacename);
         fontProps->SetFontType(fntfonttype);
         fontProps->SetWeight(fntweight);
         fontProps->SetStrikeOut(fntstrikeout);
         fontProps->SetOrientation(fntorientation);
         fontProps->SetCharSet(fntcharset);
         fontProps->SetPitchAndFamily(fntpitchnfamily);
      }

      existingAttrib->addInstance(duplicateAttribute);
   }
   else
   {
      if (*m_currentAttribMap == NULL)
      {
         *m_currentAttribMap = new CAttributes();
      }

      CAttribute *attrib;
      getCamCadData().setVisibleAttribute(**m_currentAttribMap, keyword, valueType, voidPtr, 
         x, y, rotation, height, width, proportional, 
         penWidthIndex, visible, overwrite ? attributeUpdateOverwrite : attributeUpdateAppend, 
         flag, layer, neverMirror, mirrorFlip, horizontalPosition, verticalPosition, &attrib);

      if (!fntfacename.IsEmpty() || 
         !fntfonttype.IsEmpty() ||
         !fntweight.IsEmpty() ||
         !fntunderlined.IsEmpty() ||
         !fntstrikeout.IsEmpty() ||
         !fntorientation.IsEmpty() ||
         !fntcharset.IsEmpty() ||
         !fntpitchnfamily.IsEmpty())
      {
         CFontProperties *fontProps = attrib->GetDefinedFontProperties();
         fontProps->SetFaceName(fntfacename);
         fontProps->SetFontType(fntfonttype);
         fontProps->SetWeight(fntweight);
         fontProps->SetStrikeOut(fntstrikeout);
         fontProps->SetOrientation(fntorientation);
         fontProps->SetCharSet(fntcharset);
         fontProps->SetPitchAndFamily(fntpitchnfamily);
      }

      if (m_currentBlock != NULL && m_currentBlock->getBlockNumber() == 3)
      {
         CAttribute* attrib;

         if (keyword == 25 && CAttribute::getWatchId() < 0)
         {
            if ((*m_currentAttribMap)->Lookup(keyword,attrib))
            {
               CAttribute::setWatchId(attrib->getId());
            }
         }

         int iii = 3;
      }
   }
}

/******************************************************************************
* AddKeyword
*
* Steps when duplicate keywords exist
*	1.	Check what type the keyword is (on both instances)
*	2.	rename the second keyword with an index number (ie. <Keyword>_RENAMED_n) and add it to the keyword array
*	3.	if they are the same, then map the second keyword to the first so that, when the attributes are read for 
*		each item in the cc database, we can map the attribute to the first keyword (remapping occurs in AddAttrib)
*/
void CDcaXmlContent::AddKeyword(CMapStringToString* attributes)
{
   CString attValue;

   if (!GetAttribute(attributes, "index", &attValue))
      return;

   int index = atoi(attValue);
   ValueTypeTag valueType = valueTypeNone;
   AttribGroupTag group = attribGroupUser;
   CString in, cc, out;
   bool hidden = false, inherited = false;

   if (GetAttribute(attributes, "valueType", &attValue))
      valueType = intToValueTypeTag(atoi(attValue));

   if (GetAttribute(attributes, "group", &attValue))
      group = intToAttribGroupTag(atoi(attValue));

   if (GetAttribute(attributes, "CCKeyword", &attValue))
   {
      cc = attValue;
      in = attValue;
      out = attValue;
   }

   // 041223 - AC
   //  Exposed Metal Diameter attribute should be a unit double to ensure that if the
   // units are changed, this attribute is modified according to the unit change
   if (cc == DFT_ATT_EXPOSE_METAL_DIAMETER)
      valueType = valueTypeUnitDouble;

   if (GetAttribute(attributes, "inKeyword", &attValue))
      in = attValue;

   if (GetAttribute(attributes, "outKeyword", &attValue))
      out = attValue;

   if (GetAttribute(attributes, "hidden", &attValue))
      hidden = (atoi(attValue) != 0);

   if (GetAttribute(attributes, "inherit", &attValue))
      inherited = (atoi(attValue) != 0);

   // check if this keyword has been added before
   int existingKW = getCamCadData().getAttributeKeywordIndex(cc);

   if (existingKW > 0)
   {
      // get the count we are using
      int count = 0;
      m_indexMap.Lookup(cc, count);

      // rename the current keyword
      CString buf = cc;
      cc.Format("%s_RENAMED_%d", buf, count++);
      m_indexMap.SetAt(buf, count);

      if (in == buf)
         in = cc;
      if (out == buf)
         out = cc;

      // add to the remapping map if they are the same type
      const KeyWordStruct* kw = getCamCadData().getKeyWordArray()[existingKW];
      if (kw->getValueType() == valueType)
         m_remappingMap.SetAt(index, existingKW);
   }
   else
   {
      // start the count for this keyword
      m_indexMap.SetAt(cc, 1);
   }

   int newIndex = getCamCadData().getKeyWordArray().SetAtGrow(cc, group, valueType, inherited, hidden, index);
   getCamCadData().getKeyWordArray().SetInKeyword(newIndex, in);
   getCamCadData().getKeyWordArray().SetOutKeyword(newIndex, out);
}

/******************************************************************************
* AddLayer
*/
void CDcaXmlContent::AddLayer(CMapStringToString* attributes)
{
   CString attValue;

   if (!GetAttribute(attributes, "num", &attValue))
      return;

   LayerStruct& layer = getCamCadData().getDefinedLayer(atoi(attValue));

   // init
   layer.getAttributesRef() = NULL;
   layer.setMirroredLayerIndex( layer.getLayerIndex());
   layer.setColor(255);
   layer.setOriginalColor(255);
   layer.setVisible(true);
   layer.setOriginalVisible(true);
   layer.setEditable(true);
   layer.setOriginalEditable(true);
   layer.setNegative(false);
   layer.setFloating(false);
   layer.setWorldView(true);
   layer.setSubset(false);
   layer.setZHeight(0.0);
   layer.setLayerType(layerTypeUnknown);
   layer.setArtworkStackNumber(0);
   layer.setElectricalStackNumber(0);
   layer.setPhysicalStackNumber(0);
   layer.setLayerFlags(0);
   layer.setComment("");

   m_currentAttribMap = &layer.getAttributesRef();

   if (GetAttribute(attributes, "name", &attValue))
      layer.setName(attValue);

   if (GetAttribute(attributes, "layerType", &attValue))
      layer.setLayerType(atoi(attValue));

   CString layerName = layer.getName();

   if (layerName.CompareNoCase("PROBE_TOP") == 0 || layerName.CompareNoCase("PROBEDRILL_TOP") == 0 || layerName.CompareNoCase("TEST_ACCESS_TOP") == 0)
      layer.setLayerType(layerTypeDftTop);

   else if (layerName.CompareNoCase("PROBE_BOT") == 0 || layerName.CompareNoCase("PROBEDRILL_BOT") == 0 || layerName.CompareNoCase("TEST_ACCESS_BOT") == 0)
      layer.setLayerType(layerTypeDftBottom);

   if (GetAttribute(attributes, "color", &attValue))
   {
      layer.setColor( atol(attValue));
      layer.setOriginalColor( layer.getColor());
   }

   if (GetAttribute(attributes, "originalColor", &attValue))
      layer.setOriginalColor(atol(attValue));

   if (GetAttribute(attributes, "visible", &attValue))
   {
      bool flag = (atoi(attValue) != 0);
      layer.setVisible(flag);
      layer.setOriginalVisible(flag);
      layer.setEditable(flag);
      layer.setOriginalEditable(flag);
   }

   if (GetAttribute(attributes, "editable", &attValue))
   {
      bool flag = (atoi(attValue) != 0);
      layer.setEditable(flag);
      layer.setOriginalEditable(flag);
   }

   if (GetAttribute(attributes, "originalVisible", &attValue))
      layer.setOriginalVisible(atoi(attValue));

   if (GetAttribute(attributes, "originalEditable", &attValue))
      layer.setOriginalEditable(atoi(attValue));

   if (GetAttribute(attributes, "comment", &attValue))
      layer.setComment(attValue);

   if (GetAttribute(attributes, "floating", &attValue))
      layer.setFloating( atoi(attValue));

   if (GetAttribute(attributes, "mirror", &attValue))
      layer.setMirroredLayerIndex( atoi(attValue));

   if (GetAttribute(attributes, "electricStackup", &attValue))
      layer.setElectricalStackNumber(atoi(attValue));

   if (GetAttribute(attributes, "artworkStackup", &attValue))
      layer.setArtworkStackNumber(atoi(attValue));

   if (GetAttribute(attributes, "physicalStackup", &attValue))
      layer.setPhysicalStackNumber(atoi(attValue));

   if (GetAttribute(attributes, "attr", &attValue))
      layer.setLayerFlags(atol(attValue));

   if (GetAttribute(attributes, "zHeight", &attValue))
      layer.setZHeight( atof(attValue));
}

/******************************************************************************
* AddTable
*/
void CDcaXmlContent::AddTable(CMapStringToString* attributes)
{
   CGTabTable *table = new CGTabTable;
   currentTable = table;

   lastAttribMap = m_currentAttribMap;
   m_currentAttribMap = &table->getAttributesRef();


   CString attValue;

   if (GetAttribute(attributes, "name", &attValue))
   {
      table->SetName(attValue);
   }

   // Put in currentTableList, if currentTableList is not set then 
   // add to global table list.
   if (currentTableList != NULL)
      currentTableList->AddTable(table);
   else
      getCamCadData().getTableList().AddTable(table);
}

/******************************************************************************
* AddTableRow
*/
void CDcaXmlContent::AddTableRow(CMapStringToString* attributes)
{
   if (currentTable != NULL)
   {
      currentTableRow = new CGTabRow;

      CString attValue;

      if (GetAttribute(attributes, "name", &attValue))
      {
         currentTableRow->SetName(attValue);
      }

      currentTable->AddRow(currentTableRow);
   }
}

/******************************************************************************
* AddTableRowCell
*/
void CDcaXmlContent::AddTableRowCell(CMapStringToString* attributes)
{
   if (currentTableRow != NULL)
   {
      CString attValue;

      // We're going to add the cell no matter what, so don't need to
      // check success of GetAttribute "value".
      // Will just add with blank value, which is correct behavior.

      GetAttribute(attributes, "value", &attValue);

      currentTableRow->AddCellValue(attValue);
   }
}

/******************************************************************************
* AddTableCol
*/
void CDcaXmlContent::AddTableCol(CMapStringToString* attributes)
{
   if (currentTable != NULL)
   {
      CString attValue;

      // Don't care about this result, okay if there, okay if not there
      GetAttribute(attributes, "name", &attValue);

      // Add even if blank, necessar to keep alignment with possible non-blank followers
      currentTable->AddColumnHeading(attValue);
   }
}

/******************************************************************************
* AddGeometry
*/
void CDcaXmlContent::AddGeometry(CMapStringToString* attributes)
{
   CString attValue;
   int blockNumber;
   CString blockName;
   int fileNumber = -1;
   BlockTypeTag blockType = blockTypeUnknown;

   if (GetAttribute(attributes, "num", &attValue))
   {
      blockNumber = atoi(attValue);
   }
   else
   {
      return;
   }

   GetAttribute(attributes, "name", &blockName);

   if (GetAttribute(attributes, "fileNum", &attValue))
   {
      fileNumber = atoi(attValue);
   }

   if (GetAttribute(attributes, "geomType", &attValue))
   {
      blockType = intToBlockTypeTag(atoi(attValue));
   }

   BlockStruct& block = getCamCadData().getDefinedBlock(blockNumber,blockName,fileNumber,blockType);

   m_currentBlock = &block;
   //BlockStruct* block = new BlockStruct;
   //block->num = atoi(attValue);
   //getCamCadData().getBlockArray().SetAtGrow(block->num, block);
   //if (getCamCadData().getMaxBlockIndex() <= block->num)
   //   getCamCadData().maxBlockArray = block->num+1;

   // init
   block.getAttributesRef() = NULL;
   block.setLibraryNumber(0);
   block.resetExtent();
   block.setMarked(0);
   block.setFlags(0);
   block.setPackage(false);

   block.setShape(apertureUndefined);
   block.setSizeA(0);
   block.setSizeB(0);
   block.setSizeC(0);
   block.setSizeD(0);
   block.setXoffset(0);
   block.setYoffset(0);
   block.setRotation(0);
   block.setDcode(0);
   block.setSpokeCount(0);
   block.setTcode(0);
   block.setToolSize(0);
   block.setToolDisplay(false);
   block.setToolBlockNumber(0);
   block.setToolType(0);
   block.setToolHolePlated(true);

   m_currentDataList  = &block.getDataList();
   m_currentAttribMap = &block.getAttributesRef();

   if (GetAttribute(attributes, "originalName", &attValue))
      block.setOriginalName(attValue);

   if (GetAttribute(attributes, "package", &attValue))
      block.setPackage(atoi(attValue) != 0);

   if (GetAttribute(attributes, "flag", &attValue))
      block.setFlags(atol(attValue));

   if (GetAttribute(attributes, "shape", &attValue))
      block.setShape(atoi(attValue));

   if (GetAttribute(attributes, "dCode", &attValue))
      block.setDcode(atoi(attValue));

   if (GetAttribute(attributes, "sizeA", &attValue))
      block.setSizeA((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "sizeB", &attValue))
      block.setSizeB((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "sizeC", &attValue))
      block.setSizeC((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "sizeD", &attValue))
      block.setSizeD((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "rotation", &attValue))
      block.setRotation((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "xOffset", &attValue))
      block.setXoffset((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "yOffset", &attValue))
      block.setYoffset((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "numSpokes", &attValue))
      block.setSpokeCount(atoi(attValue));

   if (GetAttribute(attributes, "tCode", &attValue))
      block.setTcode(atoi(attValue));

   if (GetAttribute(attributes, "toolSize", &attValue))
      block.setToolSize((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "type", &attValue))
      block.setToolType(atoi(attValue));

   if (GetAttribute(attributes, "display", &attValue))
      block.setToolDisplay(atoi(attValue));

   if (GetAttribute(attributes, "geomNum", &attValue))
      block.setToolBlockNumber(atoi(attValue));

   if (GetAttribute(attributes, "plated", &attValue))
      block.setToolHolePlated(atoi(attValue));

   if (GetAttribute(attributes, "punched", &attValue))
      block.setToolHolePunched(atoi(attValue));
}

/******************************************************************************
* AddPolyStruct
*/
void CDcaXmlContent::AddData(CMapStringToString* attributes,DataTypeTag dataType)
{
   long entityNumber = 0;

   CString attValue;

   if (GetAttribute(attributes, "entityNum", &attValue))
   {
      entityNumber = atol(attValue); 
   }

   DataStruct* data = getCamCadData().getNewDataStruct(dataType,entityNumber);

   // init
   data->setLayerIndex(-1);

   m_currentDataList->AddTail(data);
   lastAttribMap = m_currentAttribMap;
   m_currentAttribMap = &data->getAttributesRef();

   if (GetAttribute(attributes, "layer", &attValue))
      data->setLayerIndex(atoi(attValue));

   if (GetAttribute(attributes, "graphicClass", &attValue))
      data->setGraphicClass(atoi(attValue));

   if (GetAttribute(attributes, "negative", &attValue))
      data->setNegative(atoi(attValue));

   if (GetAttribute(attributes, "flag", &attValue))
      data->setFlags(atol(attValue));

   if (GetAttribute(attributes, "colorOverride", &attValue))
      data->setColorOverride(atoi(attValue));

   if (GetAttribute(attributes, "overrideColor", &attValue))
      data->setOverrideColor(atol(attValue));

   if (GetAttribute(attributes, "hide", &attValue))
      data->setHidden(atoi(attValue));

   if (GetAttribute(attributes, "physLyrStart", &attValue))
      data->SetPhysicalStartLayer(atoi(attValue));

   if (GetAttribute(attributes, "physLyrEnd", &attValue))
      data->SetPhysicalEndLayer(atoi(attValue));

   switch (dataType)
   {
   case dataTypePoly:
      currentPolyList = data->getPolyList();

      if (GetAttribute(attributes, "fillStyle", &attValue) && !attValue.IsEmpty())
         data->getPolyList()->setFillStyle(ValidatedFillStyle(atoi(attValue)));

      break;
   case dataTypePoint:
      data->getPoint()->x = 0;
      data->getPoint()->y = 0;

      if (GetAttribute(attributes, "x", &attValue))
         data->getPoint()->x = (DbUnit)atof(attValue);

      if (GetAttribute(attributes, "y", &attValue))
         data->getPoint()->y = (DbUnit)atof(attValue);

      break;
   case dataTypeInsert:
      {
         InsertStruct& insert = *(data->getInsert());

         insert.setBlockNumber(0);
         insert.setInsertType(insertTypeUnknown);
         insert.setRefname(NULL);
         insert.setOriginX(0.);
         insert.setOriginY(0.);
         insert.setAngle(0.);
         insert.setMirrorFlags(0);
         insert.setPlacedBottom(false);
         insert.setScale(1.);
         insert.getShadingRef().On = FALSE;

         if (GetAttribute(attributes, "geomNum", &attValue))
            insert.setBlockNumber(atoi(attValue));

         if (GetAttribute(attributes, "insertType", &attValue))
            insert.setInsertType(atoi(attValue));

         if (GetAttribute(attributes, "refName", &attValue))
            insert.setRefname(STRDUP(attValue));

         if (GetAttribute(attributes, "x", &attValue))
            insert.setOriginX(atof(attValue));

         if (GetAttribute(attributes, "y", &attValue))
            insert.setOriginY(atof(attValue));

         if (GetAttribute(attributes, "angle", &attValue))
            insert.setAngle(atof(attValue));

         if (GetAttribute(attributes, "mirror", &attValue))
            insert.setMirrorFlags(atoi(attValue));

         if (GetAttribute(attributes, "placeBottom", &attValue))
            insert.setPlacedBottom(atoi(attValue));

         if (GetAttribute(attributes, "scale", &attValue))
            insert.setScale(atof(attValue));

         if (GetAttribute(attributes, "fillStyle", &attValue)  && !attValue.IsEmpty())
            insert.setFillStyleValidated(atoi(attValue));

         if (insert.getInsertType() == insertTypeTestProbe)
         {
            int probePlacementKw = getCamCadData().getAttributeKeywordIndex(standardAttributeProbePlacement);
            int testResourceKw   = getCamCadData().getAttributeKeywordIndex(standardAttributeTestResource);

            CString placed = "Placed";
            CString tested = "Test";

            data->setAttrib(getCamCadData(), probePlacementKw, valueTypeString, placed.GetBuffer(0), attributeUpdateOverwrite, NULL);
            data->setAttrib(getCamCadData(),   testResourceKw, valueTypeString, tested.GetBuffer(0), attributeUpdateOverwrite, NULL);
         }
      }

      break;
   case dataTypeText:
      {
         currentData = data;
         TextStruct& text = *(data->getText());

         text.setPenWidthIndex(0);
         text.setHeight(1);
         text.setWidth(1);
         text.setFontNumber(0);
         text.setPnt(0.,0.);
         text.setMirrored(false);
         text.setMirrorDisabled(false);
         text.setOblique(0);
         text.setRotation(0);
         text.setProportionalSpacing(false);
         text.setHorizontalPosition(horizontalPositionLeft);
         text.setVerticalPosition(verticalPositionBaseline);

         if (GetAttribute(attributes, "text", &attValue))
         {                     
            SwapEscapeSequences(attValue, true);
            text.setText(attValue);
         }

         if (GetAttribute(attributes, "x", &attValue))
            text.setPntX((DbUnit)atof(attValue));  

         if (GetAttribute(attributes, "y", &attValue))
            text.setPntY((DbUnit)atof(attValue));  

         if (GetAttribute(attributes, "height", &attValue))
            text.setHeight(atof(attValue)); 

         if (GetAttribute(attributes, "width", &attValue))
            text.setWidth(atof(attValue));  

         if (GetAttribute(attributes, "mirror", &attValue))
            text.setMirrored(atoi(attValue));   

         if (GetAttribute(attributes, "angle", &attValue))
            text.setRotation(atof(attValue));  

         if (GetAttribute(attributes, "penWidthIndex", &attValue))
            text.setPenWidthIndex(atoi(attValue));  

         if (GetAttribute(attributes, "fontnum", &attValue))
            text.setFontNumber(atoi(attValue));  

         if (GetAttribute(attributes, "proportional", &attValue))
            text.setProportionalSpacing(atoi(attValue));   

         if (GetAttribute(attributes, "oblique", &attValue))
            text.setOblique(atoi(attValue));  

         if (GetAttribute(attributes, "neverMirror", &attValue))
            text.setMirrorDisabled(atoi(attValue));   

         if (GetAttribute(attributes, "textAlignment", &attValue))
            text.setHorizontalPosition(intToHorizontalPositionTag(atoi(attValue)));  

         if (GetAttribute(attributes, "lineAlignment", &attValue))
            text.setVerticalPosition(intToVerticalPositionTag(atoi(attValue)));  

         // New font stuff
         // facename="VeriBest Gerber 0" fonttype="2" weight="400" underlined="0" strikeout="0" orientation="0" charset="0" pitchAndFamily="0">
         if (GetAttribute(attributes, "facename", &attValue))
            text.GetDefinedFontProperties()->SetFaceName(attValue); 

         if (GetAttribute(attributes, "fonttype", &attValue))
            text.GetDefinedFontProperties()->SetFontType(attValue);

         if (GetAttribute(attributes, "weight", &attValue))
            text.GetDefinedFontProperties()->SetWeight(attValue);

         if (GetAttribute(attributes, "underlined", &attValue))
            text.GetDefinedFontProperties()->SetUnderlined(attValue);

         if (GetAttribute(attributes, "strikeout", &attValue))
            text.GetDefinedFontProperties()->SetStrikeOut(attValue);

         if (GetAttribute(attributes, "orientation", &attValue))
            text.GetDefinedFontProperties()->SetOrientation(attValue);

         if (GetAttribute(attributes, "charset", &attValue))
            text.GetDefinedFontProperties()->SetCharSet(attValue);

         if (GetAttribute(attributes, "pitchAndFamily", &attValue))
            text.GetDefinedFontProperties()->SetPitchAndFamily(attValue);
      }

      break;
   case dataTypeBlob:
      {
         currentData = data;
         BlobStruct& blob = *(data->getBlob());

         blob.pnt.x = 0;
         blob.pnt.y = 0;
         blob.width = 1;
         blob.height = 1;
         blob.rotation = 0;
         blob.top = TRUE;
         blob.bottom = TRUE;

         if (GetAttribute(attributes, "filename", &attValue))
            blob.setFileName(attValue);

         if (GetAttribute(attributes, "x", &attValue))
            blob.pnt.x = (DbUnit)atof(attValue);  

         if (GetAttribute(attributes, "y", &attValue))
            blob.pnt.y = (DbUnit)atof(attValue);  

         if (GetAttribute(attributes, "width", &attValue))
            blob.width = (DbUnit)atof(attValue);  

         if (GetAttribute(attributes, "height", &attValue))
            blob.height = (DbUnit)atof(attValue); 

         if (GetAttribute(attributes, "rotation", &attValue))
            blob.rotation = (DbUnit)atof(attValue);  

         if (GetAttribute(attributes, "top", &attValue))
            blob.top = atoi(attValue);   

         if (GetAttribute(attributes, "bottom", &attValue))
            blob.bottom = atoi(attValue);   
      }

      break;
   case dataTypePoly3D:
      currentPoly3DList = data->getPoly3DList();
      break;
   }
}

/******************************************************************************
* AddPoly
*/
void CDcaXmlContent::AddPoly(CMapStringToString* attributes)
{
   CPoly* poly = new CPoly;
   poly->setFloodBoundary(false);
   poly->setBreakOut(false);
   poly->setClosed(false);
   poly->setFilled(false);
   poly->setHatchLine(false);
   poly->setHidden(false);
   poly->setThermalLine(false);
   poly->setVoid(false);
   poly->setWidthIndex(0);

   currentPolyList->AddTail(poly);
   currentPntList = &poly->getPntList();
   CString attValue;

   if (GetAttribute(attributes, "widthIndex", &attValue))
      poly->setWidthIndex(atoi(attValue));

   if (GetAttribute(attributes, "closed", &attValue))
      poly->setClosed(atoi(attValue));

   if (GetAttribute(attributes, "filled", &attValue))
      poly->setFilled(atoi(attValue));

   if (GetAttribute(attributes, "void", &attValue))
      poly->setVoid(atoi(attValue));

   if (GetAttribute(attributes, "thermal", &attValue))
      poly->setThermalLine(atoi(attValue));

   if (GetAttribute(attributes, "boundary", &attValue))
      poly->setFloodBoundary(atoi(attValue));

   if (GetAttribute(attributes, "hidden", &attValue))
      poly->setHidden(atoi(attValue));
}

/******************************************************************************
* AddPnt
*/
void CDcaXmlContent::AddPnt(CMapStringToString* attributes)
{               
   CPnt* pnt = new CPnt;
   pnt->x = 0;
   pnt->y = 0;
   pnt->bulge = 0;

   if (!currentPntList)
      return;

   currentPntList->AddTail(pnt);

   CString attValue;

   if (GetAttribute(attributes, "x", &attValue))
      pnt->x = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "y", &attValue))
      pnt->y = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "bulge", &attValue))
      pnt->bulge = (DbUnit)atof(attValue);
}

/******************************************************************************
* AddFile
*/
void CDcaXmlContent::AddFile(CMapStringToString* attributes)
{
   CString attValue;
   int blockNumber = -1;
   int fileNumber  = -1;
   CString fileName;
   FileTypeTag sourceFileType = fileTypeUnknown;

   if (GetAttribute(attributes, "geomNum", &attValue))
   {
      blockNumber = atoi(attValue);
   }

   if (GetAttribute(attributes, "num", &attValue))
   {
      fileNumber = atoi(attValue);
   }

   if (blockNumber < 0 || fileNumber < 0)
   {
      return;
   }

   FileStruct* file = getCamCadData().getFileList().getFileAt(fileNumber);

   if (file != NULL)
   {
      return;
   }

   if (GetAttribute(attributes, "name", &attValue))
   {
      fileName = attValue;
   }

   if (GetAttribute(attributes, "sourceCAD", &attValue))
   {
      sourceFileType = intToFileTypeTag(atoi(attValue));
   }

   BlockStruct* fileBlock = getCamCadData().getBlockAt(blockNumber);

   file = &(getCamCadData().getFileList().getDefinedFileAt(fileNumber,fileName,sourceFileType,fileBlock));

   m_currentFile      = file;
   currentNetList     = &file->getNetList();
   currentBusList     = &file->getBusList();
   currentVariantList = &file->getVariantList();
   currentTypeList    = &file->getTypeList();
   currentDRCList     = &file->getDRCList();
   currentTableList   = &file->getTableList();

   // init
   file->setNotPlacedYet(false);
   file->setShow(true);

   if (GetAttribute(attributes, "origGeomNum", &attValue))
      file->setOriginalBlock(getCamCadData().getBlockAt(atoi(attValue)));

   if (GetAttribute(attributes, "refName", &attValue))
      file->setRefName(attValue);

   if (GetAttribute(attributes, "geomType", &attValue))
      file->setBlockType(intToBlockTypeTag((atoi)(attValue)));

   if (GetAttribute(attributes, "x", &attValue))
      file->setInsertX((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "y", &attValue))
      file->setInsertY((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "scale", &attValue))
      file->setScale((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "rotation", &attValue))
      file->setRotation((DbUnit)atof(attValue));

   if (GetAttribute(attributes, "mirror", &attValue))
      file->setMirrored(atoi(attValue));

   if (GetAttribute(attributes, "show", &attValue))
      file->setShow(atoi(attValue));

   if (GetAttribute(attributes, "curDFTSolution", &attValue))
      file->setCurrentDftSolutionName(attValue);

   if (GetAttribute(attributes, "curMachineSolution", &attValue))
      currentMachineName = attValue;
}

/******************************************************************************
* AddNet
*/
void CDcaXmlContent::AddNet(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString attValue;
   CString netName;
   int entityNumber = 0;
   DbFlag flags = 0;

   if (GetAttribute(attributes, "name", &attValue))
   {
      netName = attValue;
   }

   if (GetAttribute(attributes, "entity", &attValue))
   {
      entityNumber = atol(attValue); 
   }

   if (GetAttribute(attributes, "flag", &attValue))
   {
      flags = atol(attValue);
   }

   netName.Trim();
   NetStruct* net = currentNetList->addNet(netName, entityNumber);

   net->setFlagBits(flags);

   currentCompPinList = &(net->getCompPinList());
   lastAttribMap = NULL;
   m_currentAttribMap = &net->getAttributesRef();
}

/******************************************************************************
* CDcaXmlContent::AddBus
*/
void CDcaXmlContent::AddBus(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString attValue = "";

   CString busName = "";
   if (GetAttribute(attributes, "name", &attValue))
      busName = attValue;

   int entityNum = -1;
   if (GetAttribute(attributes, "entity", &attValue))
      entityNum = atol(attValue);

   CBusStruct* bus = currentBusList->AddBus(busName, entityNum);	
   currentBus = bus;
   lastAttribMap = NULL;
   m_currentAttribMap = &currentBus->getAttributesRef();
}

/******************************************************************************
* CDcaXmlContent::AddNetRef
*/
void CDcaXmlContent::AddNetRef(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString attValue = "";

   CString netName = "";
   if (GetAttribute(attributes, "name", &attValue))
      netName = attValue;

   if (currentBus)
      currentBus->AddNetName(netName);
}

/******************************************************************************
* CDcaXmlContent::AddBusRef
*/
void CDcaXmlContent::AddBusRef(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString attValue = "";

   CString busName = "";
   if (GetAttribute(attributes, "name", &attValue))
      busName = attValue;

   if (currentBus)
      currentBus->AddBusName(busName);
}

/******************************************************************************
* CDcaXmlContent::AddVariant
*/
void CDcaXmlContent::AddVariant(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString variantName = "";
   GetAttribute(attributes, "name", &variantName);

   CVariant* variant = NULL;

   if (variantName.CompareNoCase(DEFAULT_VARIANT_NAME) == 0)	
      currentVariant = currentVariantList->CreateDefaultVariant();
   else
      currentVariant = currentVariantList->AddVariant(variantName);
}

/******************************************************************************
* CDcaXmlContent::AddVariantItem
*/
void CDcaXmlContent::AddVariantItem(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString itemName = "";
   GetAttribute(attributes, "name", &itemName);

   if (currentVariant != NULL)
   {
      CVariantItem* item = currentVariant->AddItem(itemName);
      lastAttribMap = NULL;
      m_currentAttribMap = (item != NULL) ? &item->getAttributesRef() : NULL;
   }
}

/******************************************************************************
* AddCompPin
*/
void CDcaXmlContent::AddCompPin(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
   {
      lastAttribMap = m_currentAttribMap;
      m_currentAttribMap = NULL;
      return;  // Graphic doesn't read intelligent info
   }

   CString attValue,refDes,pinName;
   int entityNumber;

   //CompPinStruct* cp = new CompPinStruct;
   //currentCompPinList->AddTail(cp);
   //lastAttribMap = m_currentAttribMap;
   //m_currentAttribMap = &cp->getAttributesRef();

   //// init
   //cp->getAttributesRef() = NULL;
   //cp->setRefDes("");
   //cp->setPinName("");
   //cp->setEntityNumber(0);
   //cp->setVisible(0);
   //cp->setOrigin(0.,0.);
   //cp->setMirror(0);
   //cp->setRotationRadians(0.);
   //cp->setPinCoordinatesComplete(false);
   //cp->setPadstackBlockNumber(-1);   // this is init for no padstack

   if (GetAttribute(attributes, "comp", &attValue))
   {
      refDes = attValue;
   }

   if (GetAttribute(attributes, "pin", &attValue))
   {
      pinName = attValue;
   }

   if (GetAttribute(attributes, "entity", &attValue))
   {
      entityNumber = atol(attValue); 
   }

   CompPinStruct* compPinStruct = currentCompPinList->addCompPin(refDes,pinName,entityNumber);

   lastAttribMap = m_currentAttribMap;
   m_currentAttribMap = &compPinStruct->getAttributesRef();

   if (GetAttribute(attributes, "pinCoords", &attValue))
      compPinStruct->setPinCoordinatesComplete( atoi(attValue));

   if (GetAttribute(attributes, "x", &attValue))
      compPinStruct->setOriginX( atof(attValue));

   if (GetAttribute(attributes, "y", &attValue))
      compPinStruct->setOriginY( atof(attValue));

   if (GetAttribute(attributes, "mirror", &attValue))
      compPinStruct->setMirror( atoi(attValue));

   if (GetAttribute(attributes, "rotation", &attValue))
      compPinStruct->setRotationRadians( atof(attValue));

   if (GetAttribute(attributes, "padstackGeomNum", &attValue))
      compPinStruct->setPadstackBlockNumber( atoi(attValue));

   if (GetAttribute(attributes, "visible", &attValue))
      compPinStruct->setVisible( atoi(attValue));
}

/******************************************************************************
* AddType
*/
void CDcaXmlContent::AddType(CMapStringToString* attributes)
{
   if (Product == PRODUCT_GRAPHIC)
      return;  // Graphic doesn't read intelligent info

   CString attValue;

   CString typeName;

   if (GetAttribute(attributes, "name", &attValue))
   {
      typeName = attValue;
   }

   int entityNumber = -1;

   if (GetAttribute(attributes, "entity", &attValue))
   {
      entityNumber = atol(attValue); 
      //type->setEntityNumber(entityNum);
      //CEntityNumber::update(entityNum);
      //if (entityNum > CEntityNumber::getCurrent())
      //   setEntityNumber(entityNum);
   }

   TypeStruct* type = currentTypeList->addType(typeName,entityNumber);

   //TypeStruct* type = new TypeStruct;
   //currentTypeList->AddTail(type);
   lastAttribMap = NULL;
   m_currentAttribMap = &(type->getAttributesRef());

   // init
   //type->getAttributesRef() = NULL;
   //type->setName("");
   //type->setEntityNumber(0);
   //type->setBlockNumber(0);

   if (GetAttribute(attributes, "geomNum", &attValue))
   {
      type->setBlockNumber( atoi(attValue));
   }
}

/******************************************************************************
* AddDRC
*/
void CDcaXmlContent::AddDRC(CMapStringToString* attributes)
{
   CString attValue;

   //DRCStruct* drc = new DRCStruct;
   //currentDRCList->AddTail(drc);
   //lastAttribMap = NULL;
   //m_currentAttribMap = &drc->getAttributesRef();
   //currentDRC = drc;

   // init
   //drc->getAttributesRef() = NULL;
   //drc->string = "";
   //drc->comment = "";
   //drc->setEntityNumber(0);
   //drc->x = 0;
   //drc->y = 0;
   //drc->voidPtr = NULL;
   //drc->reviewed = 0;
   //drc->priority = 0;
   //drc->failureRange = 0;
   //drc->algIndex = 0;
   //drc->drcClass = 0;
   //drc->insertEntity = 0;

   CString drcString,comment;
   int entityNumber = 0;

   if (GetAttribute(attributes, "string", &attValue))
      drcString = attValue;

   if (GetAttribute(attributes, "comment", &attValue))
      comment = attValue;

   if (GetAttribute(attributes, "entity", &attValue))
   {
      entityNumber = atol(attValue); 
      //drc->setEntityNumber(entityNum);
      //CEntityNumber::update(entityNum);
      //if (entityNum > CEntityNumber::getCurrent())
      //   setEntityNumber(entityNum);
   }

   DRCStruct* drc = currentDRCList->addDrc(drcString,entityNumber);

   lastAttribMap = NULL;
   m_currentAttribMap = &drc->getAttributesRef();
   currentDRC = drc;

   drc->setComment(comment);

   if (GetAttribute(attributes, "x", &attValue))
      drc->setOriginX(atof(attValue));

   if (GetAttribute(attributes, "y", &attValue))
      drc->setOriginY(atof(attValue));

   if (GetAttribute(attributes, "priority", &attValue))
   {
      switch (attValue[0])
      {
      case 'H':
      case 'h':
         drc->setPriority(2);
         break;
      case 'M':
      case 'm':
         drc->setPriority(1);
         break;
      default:
         drc->setPriority(0);
         break;
      }
   }

   if (GetAttribute(attributes, "reviewed", &attValue))
   {
      if (attValue[0] == 't' || attValue[0] == 'T')
         drc->setReviewed(1);
      else
         drc->setReviewed(0);
   }

   if (GetAttribute(attributes, "failureRange", &attValue))
      drc->setFailureRange(atoi(attValue));

   if (GetAttribute(attributes, "algorithmName", &attValue))
      drc->setAlgorithmIndex(getCamCadData().getDefinedDfmAlgorithmNameIndex(attValue));

   if (GetAttribute(attributes, "algorithmIndex", &attValue))
      drc->setAlgorithmIndex(atoi(attValue));

   if (GetAttribute(attributes, "algorithmType", &attValue))
      drc->setAlgorithmType(atoi(attValue));

   if (GetAttribute(attributes, "drcClass", &attValue))
      drc->setDrcClass(atoi(attValue));

   if (GetAttribute(attributes, "insertEntity", &attValue))
      drc->setInsertEntityNumber(atol(attValue));
}

/******************************************************************************
* AddDRC_Measure
*/
void CDcaXmlContent::AddDRC_Measure(CMapStringToString* attributes)
{
   if (!currentDRC)
      return;

   CString attValue;

   DRC_MeasureStruct* meas = new DRC_MeasureStruct;
   currentDRC->setVoidPtr(meas);

   // init
   meas->entity1 = 0;
   meas->entity2 = 0;
   meas->x1 = 0;
   meas->y1 = 0;
   meas->x2 = 0;
   meas->y2 = 0;

   if (GetAttribute(attributes, "entity1", &attValue))
      meas->entity1 = atol(attValue);

   if (GetAttribute(attributes, "entity2", &attValue))
      meas->entity2 = atol(attValue);

   if (GetAttribute(attributes, "x1", &attValue))
      meas->x1 = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "y1", &attValue))
      meas->y1 = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "x2", &attValue))
      meas->x2 = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "y2", &attValue))
      meas->y2 = (DbUnit)atof(attValue);
}

/******************************************************************************
* AddDRC_Net
*/
void CDcaXmlContent::AddDRC_Net(CMapStringToString* attributes)
{
   if (!currentDRC)
      return;

   CString attValue;

   if (GetAttribute(attributes, "netname", &attValue))
   {
      if (!currentDRC->getVoidPtr())
         currentDRC->setVoidPtr(new CStringList);
      CStringList* list = (CStringList*)currentDRC->getVoidPtr();
      list->AddTail(attValue);
   }
}

/******************************************************************************
* AddDRC_Algorithm
*/
void CDcaXmlContent::AddDRC_Algorithm(CMapStringToString* attributes)
{
   if (!currentDRC)
      return;

   CString attValue;

   if (!GetAttribute(attributes, "index", &attValue))
      return;

   int i = atoi(attValue);

   if (!GetAttribute(attributes, "name", &attValue))
      return;

   getCamCadData().getDfmAlgorithmNamesArray().SetAtGrow(i, attValue);

   //if (getCamCadData().getDfmAlgorithmArraySize() <= i)
   //   getCamCadData().getDfmAlgorithmArraySize() = i+1;
}

/******************************************************************************
* AddWidth
*/
void CDcaXmlContent::AddWidth(CMapStringToString* attributes)
{
   CString attValue;

   if (!GetAttribute(attributes, "index", &attValue))
      return;

   int i = atoi(attValue);

   if (!GetAttribute(attributes, "geomNum", &attValue))
      return;

   int blockNum = atoi(attValue);
   BlockStruct* block = getCamCadData().getBlockAt(blockNum);

   getCamCadData().getWidthTable().SetAtGrow(i, block);

   if(block) // Look for ZeroWidthIndex and SmallWidthIndex and set them
   {
      if(block->getName().Compare(QZeroWidth) == 0)
         getCamCadData().getWidthTable().setZeroWidthIndex(i);
      if(block->getName().Compare(QSmallWidth) == 0)
         getCamCadData().getWidthTable().setSmallWidthIndex(i);
   }
   //if (getCamCadData().getNextWidthIndex() <= i)
   //   getCamCadData().getNextWidthIndex() = i+1;
}     

/******************************************************************************
* AddNamedView
*/
void CDcaXmlContent::AddNamedView(CMapStringToString* attributes)
{
   CString attValue;

   CNamedView* view = new CNamedView;
   getCamCadData().getNamedViewList().AddTail(view);
   //currentNamedViewArray = NULL;
   currentNamedView = NULL;
   lastAttribMap = NULL;
   m_currentAttribMap = NULL;

   // init
   //view->scaleNum = 1;
   //view->scaleDenom = 1;
   //view->scrollX = 0;
   //view->scrollY = 0;
   //view->layercnt = 0;
   //view->layerdata = NULL;

   if (GetAttribute(attributes, "name", &attValue))
      view->setName(attValue);

   if (GetAttribute(attributes, "scaleNum", &attValue))
      view->setScaleNum(atoi(attValue));

   if (GetAttribute(attributes, "scaleDenom", &attValue))
      view->setScaleDenom(atof(attValue));

   if (GetAttribute(attributes, "scrollX", &attValue))
      view->setScrollX(atoi(attValue));

   if (GetAttribute(attributes, "scrollY", &attValue))
      view->setScrollY(atoi(attValue));

   if (GetAttribute(attributes, "layerCount", &attValue))
   {
      int layerCount = atoi(attValue);
      //view->layercnt = atoi(attValue);
      //if (view->layerdata)
      //   free(view->layerdata);

      //view->layerdata = (CViewLayerData* )calloc(view->layercnt, sizeof(CViewLayerData));

      for (int i=0;i < layerCount;i++)
      {
         view->setAt(i,RGB(255, 255, 255),true);
         //view->layerdata[i].color = RGB(255, 255, 255);
         //view->layerdata[i].show = 1;
      }

      //currentNamedViewArray = view->layerdata;
      currentNamedView = view;
   }
}

/******************************************************************************
* AddNamedViewLayer
*/
void CDcaXmlContent::AddNamedViewLayer(CMapStringToString* attributes)
{
   CString attValue;

   if (!currentNamedView)
      return;

   if (!GetAttribute(attributes, "num", &attValue))
      return;

   int layerIndex = atoi(attValue);

   COLORREF color = RGB(255,0,0);
   bool show      = true;

   if (GetAttribute(attributes, "color", &attValue))
      color = atol(attValue);

   if (GetAttribute(attributes, "show", &attValue))
      show = (atoi(attValue) != 0);

   currentNamedView->setAt(layerIndex,color,show);
}

/******************************************************************************
* AddSettings
*/
void CDcaXmlContent::AddSettings(CMapStringToString* attributes)
{
   CString attValue;

   if (GetAttribute(attributes, "pageUnits", &attValue))
   {
      getCamCadData().setPageUnits(intToPageUnitsTag(atoi(attValue)));
      getCamCadData().getTopicsList().setPageUnits(getCamCadData().getPageUnits());
   }

   if (GetAttribute(attributes, "left", &attValue))
      getCamCadData().getCamCadDataSettings().setXmin(atof(attValue));

   if (GetAttribute(attributes, "right", &attValue))
      getCamCadData().getCamCadDataSettings().setXmax(atof(attValue));

   if (GetAttribute(attributes, "top", &attValue))
      getCamCadData().getCamCadDataSettings().setYmax(atof(attValue));

   if (GetAttribute(attributes, "bottom", &attValue))
      getCamCadData().getCamCadDataSettings().setYmin(atof(attValue));

   if (GetAttribute(attributes, "bottomView", &attValue))
      getCamCadData().getCamCadDataSettings().setBottomView(atoi(attValue) != 0);

   getCamCadData().setMaxCoords();
}

/******************************************************************************
* AddBackgroundBitmap
*/
void CDcaXmlContent::AddBackgroundBitmap(CMapStringToString* attributes, BOOL Top)
{
   CString attValue;

   BackgroundBitmapStruct& backgroundBitMap = (Top ? getCamCadData().getTopBackgroundBitMap() : getCamCadData().getBottomBackgroundBitMap());

   if (!GetAttribute(attributes, "filename", &attValue))
      return;

   backgroundBitMap.filename = getCamCadData().getCcPath() + attValue;

   getCamCadData().loadBackgroundBitmap(backgroundBitMap);

   if (GetAttribute(attributes, "show", &attValue))
      backgroundBitMap.show = atoi(attValue);

   if (GetAttribute(attributes, "left", &attValue))
      backgroundBitMap.placementRect.xMin = atof(attValue);

   if (GetAttribute(attributes, "right", &attValue))
      backgroundBitMap.placementRect.xMax = atof(attValue);

   if (GetAttribute(attributes, "top", &attValue))
      backgroundBitMap.placementRect.yMax = atof(attValue);

   if (GetAttribute(attributes, "bottom", &attValue))
      backgroundBitMap.placementRect.yMin = atof(attValue);
}

/******************************************************************************
* AddTopic
*/
void CDcaXmlContent::AddTopic(CMapStringToString *attributes)
{
   CString attValue;

   CTopic *topicnode = new CTopic;
   getCamCadData().getTopicsList().Add(topicnode);

   if (GetAttribute(attributes, "title", &attValue))
      topicnode->setTitle(attValue);

   if (GetAttribute(attributes, "user", &attValue))
      topicnode->setUser(attValue);

}

/******************************************************************************
* AddIssue
*/
void CDcaXmlContent::AddIssue(CMapStringToString *attributes)
{
   CString attValue;

   int lastpos = getCamCadData().getTopicsList().GetCount() - 1;
   CTopic *current_topic = getCamCadData().getTopicsList().GetAt(lastpos);
   if(!current_topic) return;

   CIssue *issuenode = new CIssue;
   current_topic->Add(issuenode);

   if (GetAttribute(attributes, "title", &attValue))
      issuenode->setTitle(attValue);

   if (GetAttribute(attributes, "user", &attValue))
      issuenode->setUser(attValue);

   if (GetAttribute(attributes, "isGeneralIssue", &attValue))
      issuenode->setisGeneralIssue(atoi(attValue));
   else
      issuenode->setisGeneralIssue(0);

}

/******************************************************************************
* AddTopicNode
*/
void CDcaXmlContent::AddTopicNode(CMapStringToString *attributes, TopicNodeTypeTag nodetype)
{
   CString attValue;
   CTopicNodeTemplate *topicnode = NULL;

   int lastpos = getCamCadData().getTopicsList().GetCount() - 1;
   CTopic *current_topic = getCamCadData().getTopicsList().GetAt(lastpos);  
   if(!current_topic) return;

   lastpos = current_topic->GetCount() - 1;
   CIssue *current_issue = current_topic->GetAt(lastpos);
   if(!current_issue) return;

   topicnode = new CTopicNodeTemplate;
   switch(nodetype)
   {
   case T_Polyline:
   case T_FreehandDraw:
      currentPntList = &topicnode->getPointList();
      break;
   }

   topicnode->setNodeType(nodetype);
   current_issue->Add(topicnode);   

   if (GetAttribute(attributes, "x1", &attValue))
      topicnode->setx1(atof(attValue));

   if (GetAttribute(attributes, "y1", &attValue))
      topicnode->sety1(atof(attValue));

   if (GetAttribute(attributes, "x2", &attValue))
      topicnode->setx2(atof(attValue));

   if (GetAttribute(attributes, "y2", &attValue))
      topicnode->sety2(atof(attValue));

   if (GetAttribute(attributes, "left", &attValue))
      topicnode->setNoteLeft(atof(attValue));

   if (GetAttribute(attributes, "right", &attValue))
      topicnode->setNoteRight(atof(attValue));

   if (GetAttribute(attributes, "top", &attValue))
      topicnode->setNoteTop(atof(attValue));

   if (GetAttribute(attributes, "bottom", &attValue))
      topicnode->setNoteBottom(atof(attValue));

   if (GetAttribute(attributes, "text", &attValue))
      topicnode->setText(attValue);

   if (GetAttribute(attributes, "OverideStickyColor", &attValue))
      topicnode->setOverideStickyColor(atoi(attValue));

   if (GetAttribute(attributes, "StickyBackColor", &attValue))
      topicnode->setStickyBackColor(atoi(attValue));

   if (GetAttribute(attributes, "EndArrow", &attValue))
      topicnode->setEndArrow(atoi(attValue));

   if (GetAttribute(attributes, "StartArrow", &attValue))
      topicnode->setStartArrow(atoi(attValue));

   if (GetAttribute(attributes, "OverideArrow", &attValue))
      topicnode->setOverideArrow(atoi(attValue));

   if (GetAttribute(attributes, "OverideColor", &attValue))
      topicnode->setOverideColor(atoi(attValue));

   if (GetAttribute(attributes, "OverideThickness", &attValue))
      topicnode->setOverideThickness(atoi(attValue));

   if (GetAttribute(attributes, "OverideThicknessSize", &attValue))
      topicnode->setOverideThicknessSize(atof(attValue));

   if (GetAttribute(attributes, "ItemColor", &attValue))
      topicnode->setItemColor(atoi(attValue));

   if (GetAttribute(attributes, "title", &attValue))
      topicnode->setTitle(attValue);

   if (GetAttribute(attributes, "user", &attValue))
      topicnode->setUser(attValue);

   if (GetAttribute(attributes, "color", &attValue))
      topicnode->setColor(atoi(attValue));

   if (GetAttribute(attributes, "visible", &attValue))
      topicnode->setVisible(atoi(attValue));

   if (GetAttribute(attributes, "geometryName", &attValue))
      topicnode->setGeometryName(attValue);
}

/******************************************************************************
* AddPoly3D
*/
void CDcaXmlContent::AddPoly3D(CMapStringToString* attributes)
{
   CPoly3D* poly = new CPoly3D;
   poly->setFloodBoundary(false);
   poly->setBreakOut(false);
   poly->setClosed(false);
   poly->setFilled(false);
   poly->setHatchLine(false);
   poly->setHidden(false);
   poly->setThermalLine(false);
   poly->setVoid(false);
   poly->setWidthIndex(0);

   currentPoly3DList->AddTail(poly);
   currentPnt3DList = &poly->getPntList();
   CString attValue;

   if (GetAttribute(attributes, "widthIndex", &attValue))
      poly->setWidthIndex(atoi(attValue));

   if (GetAttribute(attributes, "closed", &attValue))
      poly->setClosed(atoi(attValue));

   if (GetAttribute(attributes, "filled", &attValue))
      poly->setFilled(atoi(attValue));

   if (GetAttribute(attributes, "void", &attValue))
      poly->setVoid(atoi(attValue));

   if (GetAttribute(attributes, "thermal", &attValue))
      poly->setThermalLine(atoi(attValue));

   if (GetAttribute(attributes, "boundary", &attValue))
      poly->setFloodBoundary(atoi(attValue));

   if (GetAttribute(attributes, "hidden", &attValue))
      poly->setHidden(atoi(attValue));
}

/******************************************************************************
* AddPnt3D
*/
void CDcaXmlContent::AddPnt3D(CMapStringToString* attributes)
{               
   CPnt3D* pnt = new CPnt3D;
   pnt->x = 0;
   pnt->y = 0;
   pnt->z = 0;

   if (!currentPnt3DList)
      return;

   currentPnt3DList->AddTail(pnt);

   CString attValue;

   if (GetAttribute(attributes, "x", &attValue))
      pnt->x = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "y", &attValue))
      pnt->y = (DbUnit)atof(attValue);

   if (GetAttribute(attributes, "z", &attValue))
      pnt->z = (DbUnit)atof(attValue);
}