
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "xmlrealpartcontent.h"
#include "graph.h"
#include "xform.h"
#include "EntityNumber.h"
#include "attribute.h"
#include "crypt.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif                           

extern int NextFileNum; // from GRAPH.CPP
//extern CString CCPath; // from DATAFILE.CPP

/******************************************************************************
/******************************************************************************
* CAttributeStack
*/
template<class TYPE, class ARG_TYPE>
CStack<TYPE, ARG_TYPE>::CStack()
{ 
   stack.RemoveAll();
}

/******************************************************************************
* CAttributeStack::Push
*/
template<class TYPE, class ARG_TYPE>
void CStack<TYPE, ARG_TYPE>::Push(ARG_TYPE attribMap)
{
   stack.AddTail(attribMap);
}

/******************************************************************************
* CAttributeStack::Pop
*/
template<class TYPE, class ARG_TYPE>
TYPE CStack<TYPE, ARG_TYPE>::Pop()
{
   if (stack.IsEmpty())
      return NULL;

   return stack.RemoveTail();
}

/******************************************************************************
* CAttributeStack::StackHeight
*/
template<class TYPE, class ARG_TYPE>
int CStack<TYPE, ARG_TYPE>::StackHeight()
{
   return stack.GetCount();
}


/******************************************************************************
/******************************************************************************
* XMLRealPartContent
*/
XMLRealPartContent::XMLRealPartContent(CCEtoODBDoc& camCadDoc)
: CDcaXmlContent(camCadDoc.getCamCadData())
, m_camCadDoc(camCadDoc)
{
   currentVisBoard = NULL;
   libMap = NULL;

	//pkgLic = getApp().getCamcadLicense().isLicensed(camcadProductRealPartPackage);
	//devLic = getApp().getCamcadLicense().isLicensed(camcadProductRealPartDevice);

   libMap = new CMapWordToLibrary(&camCadDoc);

   
   for (POSITION filePos = camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = camCadDoc.getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      currentVisBoard = file;

      break;
   }
}

/******************************************************************************
* SetDoc
*/
//void XMLRealPartContent::SetDoc(CCEtoODBDoc *ccDoc)
//{
//   XMLContent::SetDoc(ccDoc);
//
//   // get current visible board
//   if (!doc)
//      return;
//
//   libMap = new CMapWordToLibrary(doc);
//
//   POSITION filePos = getCamCadDoc().getFileList().GetHeadPosition();
//   while (filePos)
//   {
//      FileStruct *file = getCamCadDoc().getFileList().GetNext(filePos);
//
//      if (!file->isShown() || file->isHidden())
//         continue;
//
//      if (file->getBlockType() != BLOCKTYPE_PCB)
//         continue;
//
//      currentVisBoard = file;
//
//      return;
//   }
//}

/******************************************************************************
* startElement
*/
BOOL XMLRealPartContent::startElement(CString localTagName, CMapStringToString *tAttributes)
{
	if (!pkgLic && !devLic)
		return TRUE;

   CString localName = localTagName;
   CMapStringToString *pAttributes = tAttributes;

   char c = localName[0];

   switch (c)
   {
   case 'A':
      if (localName == RP_ATTRIB_LIST)
		{
			// nothing to do
		}
      if (localName == RP_ATTRIB)
      {
			addAttrib(tAttributes);
      }
      break;
   case 'D':
      if (localName == RP_DEVDEF_LIST)
      {
         // nothing to do
      }
      else if (localName == RP_DEVDEF)
      {
         addDeviceDefinition(tAttributes);
      }
      else if (localName == RP_DEV)
      {
         addDevice(tAttributes);
      }
      break;
   case 'L':
      if (localName == RP_LIB_LIST)
      {
         // nothing to do
      }
      else if (localName == RP_LIB)
      {
         addLibrary(tAttributes);
      }
      break;
   case 'P':
      if (localName == RP_PKGDEF_LIST)
      {
         // nothing to do
      }
      else if (localName == RP_PKGDEF)
      {
         addPackageDefinition(tAttributes);
      }
      else if (localName == RP_PKG)
      {
         addPackage(tAttributes);
      }
      else if (localName == RP_POLY)
      {
         addPoly(tAttributes);
      }
      else if (localName == RP_PIN_LIST)
		{
			// nothing to do
		}
      else if (localName == RP_PIN)
      {
         addPin(tAttributes);
      }
      else if (localName == RP_POLYPOINT)
      {
         addPolyPoint(tAttributes);
      }
      else if (localName == RP_PARTDEF_LIST)
      {
         // nothing to do
      }
      else if (localName == RP_PARTDEF)
      {
         addPartDefinition(tAttributes);
      }
      else if (localName == RP_PINDEF_LIST)
      {
         // nothing to do
      }
      else if (localName == RP_PINDEF)
      {
         addPinDefinition(tAttributes);
      }
      else if (localName == RP_PART_LIST)
      {
         // nothing to do
      }
      else if (localName == RP_PART)
      {
         addPart(tAttributes);
      }
      break;

   case 'R':
      if (localName == RP_RPXML)
      {
         initRealPartData(tAttributes);
      }
      break;
   }

   return TRUE;
}

/******************************************************************************
* endElement
*/
void XMLRealPartContent::endElement(CString localTagName)
{
   CString localName = localTagName;

   if (localName == RP_PKGDEF || localName == RP_DEVDEF || localName == RP_PARTDEF || localName == RP_PINDEF)
   {
      Graph_Block_Off();

      m_currentDataList = dataListStack.Pop();

      m_currentAttribMap = attribStack.Pop();
   }
   else if (localName == RP_POLY)
   {
		if (!currentPntList)
			return;

      CPnt *first = currentPntList->GetHead();
      CPnt *last = currentPntList->GetTail();

      if (first && last)
      {
         if (fabs(first->x - last->x) > SMALLNUMBER || fabs(first->y - last->y) > SMALLNUMBER)
            Graph_Vertex(first->x, first->y, 0);
      }
      currentPntList = NULL;
   }
}

/******************************************************************************
* buildDefinitionName
*/
CString XMLRealPartContent::buildDefinitionName(CString name, SLibrary *lib)
{
   if (lib)
      return name + "_" + lib->GUID;
   else
      return name;
}

/******************************************************************************
* initRealPartData
*/
void XMLRealPartContent::initRealPartData(CMapStringToString *attributes)
{
   // add top and bottom foot layer
   footLayerTop = Graph_Level(RP_LAYER_PKG_FOOT_TOP, "", FALSE);
   footLayerBot = Graph_Level(RP_LAYER_PKG_FOOT_BOTTOM, "", FALSE);
   Graph_Level_Mirror(RP_LAYER_PKG_FOOT_TOP, RP_LAYER_PKG_FOOT_BOTTOM, "");

   // add top and bottom package body
   pkgBodyTop = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
   pkgBodyBot = Graph_Level(RP_LAYER_PKG_BODY_BOTTOM, "", FALSE);
   Graph_Level_Mirror(RP_LAYER_PKG_BODY_TOP, RP_LAYER_PKG_BODY_BOTTOM, "");

   // add top and bottom package pin
   pkgPinTop = Graph_Level(RP_LAYER_PKG_PIN_TOP, "", FALSE);
   pkgPinBot = Graph_Level(RP_LAYER_PKG_PIN_BOTTOM, "", FALSE);
   Graph_Level_Mirror(RP_LAYER_PKG_PIN_TOP, RP_LAYER_PKG_PIN_BOTTOM, "");

	getCamCadDoc().getLayerArray()[pkgPinTop]->setLayerType(LAYTYPE_PKG_PIN_LEG_TOP);
   getCamCadDoc().getLayerArray()[pkgPinTop]->setColor( RGB(198, 198, 198)); // Light Gray
	getCamCadDoc().getLayerArray()[pkgPinBot]->setLayerType(LAYTYPE_PKG_PIN_LEG_BOT);
   getCamCadDoc().getLayerArray()[pkgPinBot]->setColor( RGB(198, 198, 198)); // Light Gray

   getCamCadDoc().getLayerArray()[footLayerTop]->setLayerType(LAYTYPE_PKG_PIN_FOOT_TOP);
   getCamCadDoc().getLayerArray()[footLayerTop]->setColor( RGB(128, 128, 128)); // Dark Gray
   getCamCadDoc().getLayerArray()[footLayerBot]->setLayerType(LAYTYPE_PKG_PIN_FOOT_BOT);
   getCamCadDoc().getLayerArray()[footLayerBot]->setColor( RGB(128, 128, 128)); // Dark Gray

   getCamCadDoc().getLayerArray()[pkgBodyTop]->setLayerType(LAYTYPE_PKG_BODY_TOP);
   getCamCadDoc().getLayerArray()[pkgBodyTop]->setColor( RGB(255, 255, 0)); // Yellow
   getCamCadDoc().getLayerArray()[pkgBodyBot]->setLayerType(LAYTYPE_PKG_BODY_BOT);
   getCamCadDoc().getLayerArray()[pkgBodyBot]->setColor( RGB(0, 255, 255)); // Cyan

   // Get a map to all datas in the file we're adding to
   POSITION pos = currentVisBoard->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = currentVisBoard->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      lpDataMap.SetAt(data->getInsert()->getRefname(), data);
   }
}

/******************************************************************************
* addLibrary
*/
void XMLRealPartContent::addLibrary(CMapStringToString *attributes)
{
   CString GUID, name, attValue;
   WORD number;

   if (GetAttribute(attributes, "number", &attValue))
      number = atoi(attValue);

   if (GetAttribute(attributes, "id", &attValue))
      GUID = attValue;

   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;

   // find the library file associated current GUID
   FileStruct *file = NULL;
   int guidKw = getCamCadDoc().IsKeyWord(REALPART_GUID, 0);

   if (guidKw >= 0)
   {
      POSITION filePos = getCamCadDoc().getFileList().GetHeadPosition();
      while (filePos)
      {
         file = getCamCadDoc().getFileList().GetNext(filePos);

         if (file->getBlockType() == blockTypeLibrary)
         {        
            BlockStruct *block = file->getBlock();
            if (block->getAttributesRef())
            {
               Attrib* attrib = NULL;

               if (block->getAttributesRef()->Lookup(guidKw, attrib))
               {
                  if (GUID == get_attvalue_string(&(getCamCadDoc()), attrib))
                     break;
               }
            }
         }

         file = NULL;
      }
   }

   // if there was an library, create one
   if (!file)
   {
      // create the new file
      file = Graph_File_Start(name, 0);
      file->setBlockType(blockTypeLibrary);
      file->getBlock()->setBlockType(blockTypeLibrary);
      Graph_Block_Off();
   }

   // update the library's name
   file->setName(name);

   // add database GUID attribute to the files block
   getCamCadDoc().SetUnknownAttrib(&(file->getBlock()->getAttributesRef()), REALPART_GUID, GUID, SA_OVERWRITE, NULL);

   // create a map to this library
   SLibrary *lib = new SLibrary();
   lib->GUID = GUID;
   lib->number = number;
   lib->file = file;

   // add the library map
   libMap->SetAt(lib->number, lib);
}

/******************************************************************************
* addPackageDefinition
*/
void XMLRealPartContent::addPackageDefinition(CMapStringToString *attributes)
{
   if (!pkgLic) 
   {
		// set current entity list
		dataListStack.Push(m_currentDataList);
		m_currentDataList = NULL;

		// set current attribMap
		attribStack.Push(m_currentAttribMap);
		m_currentAttribMap = NULL;

      return;
   }

	CString name, attValue;
   WORD libNumber = -1;

   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;
   if (GetAttribute(attributes, "library", &attValue))
      libNumber = atoi(attValue);

   // look up the library
   SLibrary *lib = libMap->Lookup(libNumber);

   // create package geometery
   BlockStruct *block = Graph_Block_On(GBO_OVERWRITE, name, lib?lib->file->getFileNumber():currentVisBoard->getFileNumber(), 0);
   if (!block)
      return;
   block->setBlockType(BLOCKTYPE_PACKAGE);

   packageMap.SetAt(buildDefinitionName(name, lib), block);

   // set current entity list
   dataListStack.Push(m_currentDataList);
   m_currentDataList = &block->getDataList();

   // set current attribMap
   attribStack.Push(m_currentAttribMap);
   m_currentAttribMap = &block->getAttributesRef();
}

/******************************************************************************
* addPackage
*/
void XMLRealPartContent::addPackage(CMapStringToString *attributes)
{
   if (!pkgLic) 
      return;

	if (!m_currentDataList)
      return;

   CString name, attValue;
   WORD libNumber = -1;

   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;
   if (GetAttribute(attributes, "library", &attValue))
      libNumber = atoi(attValue);

   SLibrary *lib = libMap->Lookup(libNumber);

   /////////////////////////////////////////////////////////////////////////////////
   // add the package block if necessary
   BlockStruct *pkgBlock = packageMap.Lookup(buildDefinitionName(name, lib));
   if (pkgBlock)
   {
      // check to see if there was an existing package
      DataStruct *pkgData = NULL;
      POSITION pos = m_currentDataList->GetHeadPosition();
      while (pos)
      {
         DataStruct *data = m_currentDataList->GetNext(pos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (data->getInsert()->getInsertType() != INSERTTYPE_PACKAGE)
            continue;

         pkgData = data;
      }

      // if there was no package, create an insert and add it to the list
      if (!pkgData)
      {
         pkgData = getCamCadData().getNewDataStruct(dataTypeInsert);
         //pkgData->getInsert() = new InsertStruct();
         m_currentDataList->AddTail(pkgData);

         //pkgData->setDataType(dataTypeInsert);
         //pkgData->setEntityNumber(CEntityNumber::allocate());
         pkgData->setLayerIndex(-1);

         pkgData->getInsert()->setInsertType(insertTypePackage);
         //pkgData->getInsert()->setScale(1.0);
      }

      // assign what it inserts and set all the properties
      pkgData->getInsert()->setRefname(STRDUP(name));
      pkgData->getInsert()->setBlockNumber(pkgBlock->getBlockNumber());
      pkgData->getInsert()->setOriginX(0.0);
      pkgData->getInsert()->setOriginY(0.0);
      pkgData->getInsert()->setAngle(0.0);

		inheritAttributes(pkgData, pkgBlock);
	}
}

/******************************************************************************
* addPoly
*/
void XMLRealPartContent::addPoly(CMapStringToString *attributes)
{
   // make sure there is a datalist to add to
   if (!m_currentDataList)
      return;

   CString attValue;
   int type = 0, layer = 0, graphic_class = 0;

   if (GetAttribute(attributes, "type", &attValue))
      type = atoi(attValue);

   // get the layer we need to place this on based on the type
   switch (type)
   {
   case RP_POLY_BODY:
      layer = pkgBodyTop;
      graphic_class = GR_CLASS_PACKAGEBODY;
      break;
   case RP_POLY_PIN:
      layer = pkgPinTop;
      graphic_class = GR_CLASS_PACKAGEPIN;
      break;
   case RP_POLY_PKG_FOOT:
      layer = footLayerTop;
      graphic_class = GR_CLASS_PACKAGEPIN;
      break;
   case RP_POLY_PIN_1_DES:
      layer = pkgPinTop;
      graphic_class = 0;
      break;
   }

   // add poly to current entity list
   DataStruct *polyData = Graph_PolyStruct(layer, 0, FALSE);
   if (!polyData)
      return;
   polyData->setGraphicClass(graphic_class);

   CPoly *poly = Graph_Poly(polyData, 0, TRUE, FALSE, TRUE);
   if (!poly)
      return;

   // set current poly
   currentPntList = &poly->getPntList();
}

/******************************************************************************
* addPolyPoint
*/
void XMLRealPartContent::addPolyPoint(CMapStringToString *attributes)
{
   if (!currentPntList)
      return;

   CString attValue;
   DbUnit x = 0.0, y = 0.0, bulge = 0.0;

   if (GetAttribute(attributes, "x", &attValue))
      x = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().getSettings().getPageUnits()) / 1000.0);
   if (GetAttribute(attributes, "y", &attValue))
      y = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().getSettings().getPageUnits()) / 1000.0);
   if (GetAttribute(attributes, "bulge", &attValue))
      bulge = (DbUnit)atof(attValue);

   // add poly point to current poly
   Graph_Vertex(x, y, bulge);
}

/******************************************************************************
* addDeviceDefinition
*/
void XMLRealPartContent::addDeviceDefinition(CMapStringToString *attributes)
{
   if (!devLic) 
   {
		// set current entity list
		dataListStack.Push(m_currentDataList);
		m_currentDataList = NULL;

		// set current attribMap
		attribStack.Push(m_currentAttribMap);
		m_currentAttribMap = NULL;

      return;
   }

   //CString name, attValue;
   //WORD libNumber = -1;

   //if (GetAttribute(attributes, "name", &attValue))
   // name = attValue;
   //if (GetAttribute(attributes, "package", &attValue))
   // libNumber = atoi(attValue);

   //SLibrary *lib = libMap->Lookup(libNumber);

   //// create device geometery
   //BlockStruct *block = Graph_Block_On(GBO_OVERWRITE, name, currentVisBoard->num, 0);
   //if (!block)
   // return;
   //block->setBlockType(BLOCKTYPE_DEVICE);

   //deviceMap.SetAt(buildDefinitionName(name, lib), block);

   //// set current entity list
   //dataListStack.Push(m_currentDataList);
   //m_currentDataList = &block->getDataList();

   //// set current attribMap
   //attribStack.Push(m_currentAttribMap);
   //m_currentAttribMap = &block->getAttributesRef();
}

/******************************************************************************
* addDevice
*/
void XMLRealPartContent::addDevice(CMapStringToString *attributes)
{
   if (!devLic) 
      return;

	if (!m_currentDataList)
      return;

   //CString name, attValue;
   //DbUnit rotation = 0.0, x = 0.0, y = 0.0;
   //WORD libNumber = -1;

   //if (GetAttribute(attributes, "name", &attValue))
   // name = attValue;
   //if (GetAttribute(attributes, "rotation", &attValue))
   // rotation = (DbUnit)DegToRad(atof(attValue));
   //if (GetAttribute(attributes, "x", &attValue))
   // x = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().Settings.PageUnits) / 1000.0);
   //if (GetAttribute(attributes, "y", &attValue))
   // y = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().Settings.PageUnits) / 1000.0);
   //if (GetAttribute(attributes, "library", &attValue))
   // libNumber = atoi(attValue);

   //SLibrary *lib = libMap->Lookup(libNumber);

   //BlockStruct *devBlock = deviceMap.Lookup(buildDefinitionName(name, lib));
   //if (devBlock)
   //{
   // // check to see if there was an existing package
   // DataStruct *devData = NULL;
   // POSITION pos = m_currentDataList->GetHeadPosition();
   // while (pos)
   // {
   //    DataStruct *data = m_currentDataList->GetNext(pos);

   //    if (data->getDataType() != T_INSERT)
   //       continue;

   //    if (data->getInsert()->getInsertType() != INSERTTYPE_DEVICE)
   //       continue;

   //    devData = data;
   // }

   // // if there was no package, create an insert and add it to the list
   // if (!devData)
   // {
   //    devData = new DataStruct();
   //    devData->getInsert() = new InsertStruct();
   //    m_currentDataList->AddTail(devData);

   //    devData->setDataType(dataTypeInsert);
   //    devData->getEntityNumber() = EntityNum++;
   //    devData->layer = -1;

   //    devData->getInsert()->setInsertType(insertTypePackage);
   //    devData->getInsert()->getScale() = 1.0;
   // }
   // else
   //    delete devData->getInsert()->refname;

   // // assign what it inserts and set all the properties
   // devData->getInsert()->refname = STRDUP(refDes);
   // devData->getInsert()->getBlockNumber() = devBlock->num;
   // devData->getInsert()->getOriginX() = 0.0;
   // devData->getInsert()->getOriginY() = 0.0;
   // devData->getInsert()->getAngle() = 0.0;

	//	inheritAttributes(devData, devBlock);
   //}
}

/******************************************************************************
* addPinDefinition
*/
void XMLRealPartContent::addPinDefinition(CMapStringToString *attributes)
{
   CString name, attValue;
   WORD libNumber = -1;

   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;
   if (GetAttribute(attributes, "library", &attValue))
      libNumber = atoi(attValue);

   SLibrary *lib = libMap->Lookup(libNumber);

   // create part geometery
   BlockStruct *block = Graph_Block_On(GBO_OVERWRITE, name, lib?lib->file->getFileNumber():currentVisBoard->getFileNumber(), 0);
   if (!block)
      return;
   block->setBlockType(BLOCKTYPE_PACKAGEPIN);
   
   pinMap.SetAt(buildDefinitionName(name, lib), block);

   // set current entity list
   dataListStack.Push(m_currentDataList);
   m_currentDataList = &block->getDataList();

   // set current attribMap
   attribStack.Push(m_currentAttribMap);
   m_currentAttribMap = &block->getAttributesRef();
}

/******************************************************************************
* addPin
*/
void XMLRealPartContent::addPin(CMapStringToString *attributes)
{
   // make sure there is a datalist to add to
   if (!m_currentDataList)
      return;

   CString attValue;

   CString refDes, name;
   DbUnit rotation = 0.0, x = 0.0, y = 0.0;
   WORD libNumber = -1;

   if (GetAttribute(attributes, "refDes", &attValue))
      refDes = attValue;
   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;
   if (GetAttribute(attributes, "rotation", &attValue))
      rotation = (DbUnit)DegToRad(atof(attValue));
   if (GetAttribute(attributes, "x", &attValue))
      x = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().getSettings().getPageUnits()) / 1000.0);
   if (GetAttribute(attributes, "y", &attValue))
      y = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().getSettings().getPageUnits()) / 1000.0);
   if (GetAttribute(attributes, "library", &attValue))
      libNumber = atoi(attValue);

   SLibrary *lib = libMap->Lookup(libNumber);

   // get the block to add
   BlockStruct *pinBlock = pinMap.Lookup(buildDefinitionName(name, lib));
   if (!pinBlock)
      return;

   DataStruct *pinData = getCamCadData().getNewDataStruct(dataTypeInsert);
   //pinData->getInsert() = new InsertStruct();
   m_currentDataList->AddTail(pinData);

   //pinData->setDataType(dataTypeInsert);
   //pinData->setEntityNumber(CEntityNumber::allocate());
   pinData->setLayerIndex(-1);

   pinData->getInsert()->setInsertType(insertTypePackagePin);
   //pinData->getInsert()->setScale(1.0);

   pinData->getInsert()->setRefname(STRDUP(refDes));
   pinData->getInsert()->setBlockNumber(pinBlock->getBlockNumber());
   pinData->getInsert()->setOriginX(x);
   pinData->getInsert()->setOriginY(y);
   pinData->getInsert()->setAngle(rotation);
   pinData->getInsert()->setPlacedBottom(false);
   pinData->getInsert()->setMirrorFlags(0);

	inheritAttributes(pinData, pinBlock);
}

/******************************************************************************
* addPartDefinition
*/
void XMLRealPartContent::addPartDefinition(CMapStringToString *attributes)
{
   CString name, attValue;
   WORD libNumber = -1;

   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;
   if (GetAttribute(attributes, "library", &attValue))
      libNumber = atoi(attValue);

   SLibrary *lib = libMap->Lookup(libNumber);

   // create part geometery
   BlockStruct *block = Graph_Block_On(GBO_OVERWRITE, name, lib?lib->file->getFileNumber():currentVisBoard->getFileNumber(), 0);
   if (!block)
      return;
   block->setBlockType(BLOCKTYPE_REALPART);
   
   partMap.SetAt(buildDefinitionName(name, lib), block);

   // set current entity list
   dataListStack.Push(m_currentDataList);
   m_currentDataList = &block->getDataList();

   // set current attribMap
   attribStack.Push(m_currentAttribMap);
   m_currentAttribMap = &block->getAttributesRef();
}

/******************************************************************************
* addPart
*/
void XMLRealPartContent::addPart(CMapStringToString *attributes)
{
   CString attValue;

   CString refDes, name;
   DbUnit rotation = 0.0, x = 0.0, y = 0.0;
   WORD libNumber = -1;

   if (GetAttribute(attributes, "refDes", &attValue))
      refDes = attValue;
	// if the name attribute is empty, then that means we are unassigning a part to "refDes"
   if (GetAttribute(attributes, "name", &attValue))
      name = attValue;
   if (GetAttribute(attributes, "rotation", &attValue))
      rotation = (DbUnit)DegToRad(atof(attValue));
   if (GetAttribute(attributes, "x", &attValue))
      x = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().getSettings().getPageUnits()) / 1000.0);
   if (GetAttribute(attributes, "y", &attValue))
      y = (DbUnit)(atof(attValue) * Units_Factor(UNIT_MM, getCamCadDoc().getSettings().getPageUnits()) / 1000.0);
   if (GetAttribute(attributes, "library", &attValue))
      libNumber = atoi(attValue);

   SLibrary *lib = libMap->Lookup(libNumber);

   // get the block to add
   BlockStruct *prtBlock = partMap.Lookup(buildDefinitionName(name, lib));
   if (!name.IsEmpty() && !prtBlock)
      return;

	// we only need to look up the component when we are adding new realparts
	char placeBottom = 0;
	char mirror = 0;
   if (!name.IsEmpty())
	{
		// find the component so I know what surface this part is on
		DataStruct *compData = lpDataMap.Lookup(refDes);
		DTransform xForm(compData->getInsert()->getOriginX(), compData->getInsert()->getOriginY(), compData->getInsert()->getScale(), compData->getInsert()->getAngle(), compData->getInsert()->getMirrorFlags());
		Point2 pnt(x, y);
		xForm.TransformPoint(&pnt);
		x = (DbUnit)pnt.x;
		y = (DbUnit)pnt.y;
		placeBottom = compData->getInsert()->getPlacedBottom();
		mirror = compData->getInsert()->getMirrorFlags();

		if (mirror & MIRROR_FLIP)
			rotation = compData->getInsert()->getAngle() - rotation;
		else
			rotation += compData->getInsert()->getAngle();

		while (rotation < 0)
			rotation += (DbUnit)PI2;
		while (rotation >= PI2)
			rotation -= (DbUnit)PI2;
	}

   // find the RealPart to add the part to
   DataStruct *realPartData = NULL;
   POSITION rpPos = currentVisBoard->getBlock()->getDataList().GetHeadPosition();
   while (rpPos)
   {
      realPartData = currentVisBoard->getBlock()->getDataList().GetNext(rpPos);

      if (realPartData->getDataType() == T_INSERT && realPartData->getInsert()->getInsertType() == INSERTTYPE_REALPART &&
         !STRICMP(realPartData->getInsert()->getRefname(), refDes))
         break;

      realPartData = NULL;
   }

	// unassign the real part
	if (name.IsEmpty())
	{
		// no real part to unassign
		if (!realPartData)
			return;

		RemoveOneEntityFromDataList(&(getCamCadDoc()), &currentVisBoard->getBlock()->getDataList(), realPartData, NULL);
		return;
	}

   // if we don't have an existing real part, create one
   if (!realPartData)
   {
      realPartData = getCamCadData().getNewDataStruct(dataTypeInsert);
      //realPartData->getInsert() = new InsertStruct();
      currentVisBoard->getBlock()->getDataList().AddTail(realPartData);

      //realPartData->setDataType(dataTypeInsert);
      //realPartData->setEntityNumber(CEntityNumber::allocate());
      realPartData->setLayerIndex(-1);

      realPartData->getInsert()->setInsertType(insertTypeRealPart);
      //realPartData->getInsert()->setScale(1.0);
   }

   // assign what it inserts and set all the properties
   realPartData->getInsert()->setRefname(STRDUP(refDes));
   realPartData->getInsert()->setBlockNumber(prtBlock->getBlockNumber());
   realPartData->getInsert()->setOriginX(x);
   realPartData->getInsert()->setOriginY(y);
   realPartData->getInsert()->setAngle(rotation);
   realPartData->getInsert()->setPlacedBottom(placeBottom);
   realPartData->getInsert()->setMirrorFlags(mirror);

	inheritAttributes(realPartData, prtBlock);
}

#include "CCEtoODB.h"

/******************************************************************************
* addAttrib
*/
void XMLRealPartContent::addAttrib(CMapStringToString *attributes)
{
   if (m_currentAttribMap == NULL)
      return;

   CString attValue, name, type, val;
   if (!GetAttribute(attributes, "name", &attValue))
      return;
	name = attValue;
   if (GetAttribute(attributes, "type", &attValue))
      type = attValue;
   if (GetAttribute(attributes, "value", &attValue))
      val = attValue;

	int valueType = VT_STRING;
	if (type == "STRING")
		valueType = VT_STRING;
	else if (type == "DIMENSIONAL")
		valueType = VT_DOUBLE;
   WORD keyword = getCamCadDoc().RegisterKeyWord(name, 0, valueType);

	// do not assign anything unless they are the same type
	if (valueType != getCamCadDoc().getKeyWordArray()[keyword]->getValueType())
		return;

   void *voidPtr;
   int intValue;
   double doubleValue;

   switch (valueType)
   {
   case VT_NONE:
      voidPtr = NULL;
      break;
   case VT_STRING:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      voidPtr = (void*)(LPCTSTR)val;
      break;
   case VT_INTEGER:
      intValue = atoi(val);
      voidPtr = (void*)&intValue;
      break;
   case VT_DOUBLE:
   case VT_UNIT_DOUBLE:
      doubleValue = atof(val);
      voidPtr = (void*)&doubleValue;
      break;
   }

   double x = 0, y = 0, rotation = 0, height = 0, width = 0;
   int penWidthIndex = 0, layer = -1;
   BOOL visible = FALSE, proportional = FALSE, neverMirror = FALSE;
   HorizontalPositionTag horizontalPosition = horizontalPositionLeft;
   VerticalPositionTag verticalPosition     = verticalPositionBaseline;
   DbFlag flag = 0;

   /*void* voidAttrib;
   if (*m_currentAttribMap != NULL && (*m_currentAttribMap)->Lookup(keyword,voidAttrib))
   {
      Attrib* existingAttrib = (Attrib*)voidAttrib;
      Attrib* duplicateAttribute = new Attrib();

      duplicateAttribute->setCoordinate(x,y);
      duplicateAttribute->setRotationRadians( rotation);
      duplicateAttribute->setHeight(height);
      duplicateAttribute->setWidth(width);
      duplicateAttribute->setProportionalSpacing(proportional);
      duplicateAttribute->setPenWidthIndex(penWidthIndex);
      duplicateAttribute->setMirrorDisabled(neverMirror);
      duplicateAttribute->setVisible(visible);
      duplicateAttribute->setFlags(flag);
      duplicateAttribute->setLayerIndex(layer);
      duplicateAttribute->setInherited(false);
      duplicateAttribute->setHorizontalPosition(horizontalPosition);
      duplicateAttribute->setVerticalPosition(verticalPosition);

      existingAttrib->addInstance(duplicateAttribute);
   }
   else*/
   {
      getCamCadDoc().SetVisAttrib(m_currentAttribMap, keyword, valueType, voidPtr, x, y, rotation, height, width, proportional, 
         penWidthIndex, visible, SA_OVERWRITE, flag, layer, neverMirror, horizontalPosition, verticalPosition);
   }

}

/******************************************************************************
* inheritAttributes
*/
void XMLRealPartContent::inheritAttributes(DataStruct *data, BlockStruct *block)
{
	// inherit block's attributes
	if (block->getAttributesRef() != NULL)
	{
		WORD keyword;
		Attrib* b_ptr;
		Attrib* i_ptr;
		CAttributeMapWrap dataAttribMap(data->getAttributesRef());

		for (POSITION pos = block->getAttributesRef()->GetStartPosition();pos != NULL;)
		{
			block->getAttributesRef()->GetNextAssoc(pos, keyword,b_ptr);
			i_ptr = b_ptr->allocateCopy();
			i_ptr->setInherited(true); // set the inherited flag

			dataAttribMap.addInstance(keyword,i_ptr);
		}
	}
}

