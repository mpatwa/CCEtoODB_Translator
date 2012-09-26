// $Header: /CAMCAD/4.6/read_wrt/FabmasterFATFWriter.cpp 31    6/06/07 6:59p Rick Faltersack $



// FABMaster FATF Writer

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Port.h"
#include "PolyLib.h"
#include "FabmasterFATFWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// For vectoring out arcs
#define MIN_ARC_ANGLE_DEGREES  5.0

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


void FABMasterFATF_WriteFiles     (CString     filename, CCEtoODBDoc &doc, FormatStruct *format)
{

   FileStruct *pcbFile = doc.getFileList().GetOnlyShown(blockTypePcb);

   if (pcbFile == NULL)
   {
      ErrorMessage("A single PCB File must be visible for FATF Export", "");
   }
   else
   {
      CFabmasterFATFWriter FatfWriter(&doc, pcbFile);
      FatfWriter.InitData();
      FatfWriter.WriteFile(filename);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////

CString CFatfPart::GetDeviceName(CCEtoODBDoc *doc)
{
   // Favor PARTNUMBER attribute.
   // If not set then use $$DEVICE$$ attribute.

   CString devname;

   WORD kw = doc->getStandardAttributeKeywordIndex(standardAttributePartNumber);
   Attrib *attrib = NULL;

   if (this->GetCamcadPartData()->lookUpAttrib(kw, attrib))
   {
      #if CamCadMajorMinorVersion > 406  //  > 4.6
		   devname = attrib->getStringValue();
      #else
		   devname = doc->ValueArray[attrib->getStringValueIndex()];
      #endif
   }
   else
   {
      kw = doc->getStandardAttributeKeywordIndex(standardAttributeTypeListLink /* $$DEVICE$$ */);

      if (this->GetCamcadPartData()->lookUpAttrib(kw, attrib))
      {
      #if CamCadMajorMinorVersion > 406  //  > 4.6
		   devname = attrib->getStringValue();
      #else
 		   devname = doc->ValueArray[attrib->getStringValueIndex()];
      #endif
      }
   }

   return devname;
}

CString CFatfPart::GetInsertedGeometryName(CCEtoODBDoc *doc)
{
   CString geomname;

   int insertedBlkNum = this->GetCamcadPartData()->getInsert()->getBlockNumber();
   BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
   if (insertedBlk != NULL)
      geomname = insertedBlk->getName();

   return geomname;
}

CString CFatfPart::GetInsertedBlockNumberAsString()
{
   CString str;
   str.Format("%d", this->GetCamcadPartData()->getInsert()->getBlockNumber());
   return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////
   
CString CFatfLayer::GetFatfLayerSense(int maxElectStackNum)
{
   LayerStruct *camcadLayer = this->GetCamcadLayerData();

   LayerTypeTag camcadLayerType = camcadLayer->getLayerType();
   CString typeStr = layerTypeToString(camcadLayerType);
   typeStr.MakeUpper();
   if (typeStr.Find("TOP") > -1)
      return "TOP";
   else if (typeStr.Find("BOTTOM") > -1)
      return "BOTTOM";

   int electStack = camcadLayer->getElectricalStackNumber();
   if (electStack == 1)
      return "TOP";
   else if (electStack == maxElectStackNum && maxElectStackNum != 0)
      return "BOTTOM";

   return "TRANSPARENT";
}

CString CFatfLayer::GetFatfLayerType()
{
   // Use predetermined type if set
   if (!m_fatfLayerType.IsEmpty())
   {
      return m_fatfLayerType;
   }

   // Determine fatf type from camcad type
   LayerStruct *camcadLayer = this->GetCamcadLayerData();

   if (camcadLayer->getElectricalStackNumber() > 0)
      return "ELECTRICAL";

   LayerTypeTag camcadLayerType = camcadLayer->getLayerType();

   if (camcadLayerType == layerTypeComponentOutline)
      return "ASSEMBLY";

   CString typeStr = layerTypeToString(camcadLayerType);
   typeStr.MakeUpper();

   if (typeStr.Find("SIGNAL") > -1)
      return "ELECTRICAL";

   if (typeStr.Find("POWER") > -1)
      return "ELECTRICAL";

   if (typeStr.Find("MASK") > -1)
      return "MASKING";

   if (typeStr.Find("SILKSCREEN") > -1)
      return "SILKSCREEN";

   return "DOCUMENTATION";

}

void CFatfLayerMap::CollectLayers(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   this->RemoveAll();

   m_doc = doc;
   m_maxElectricalStackNum = 0;
   
   for (int i = 0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *camcadLayer = doc->getLayer(i);
      if (camcadLayer != NULL)
      {
         CFatfLayer *fatfLayer = new CFatfLayer(0, camcadLayer);
         this->SetAt(camcadLayer->getName(), fatfLayer);

         if (camcadLayer->getElectricalStackNumber() > m_maxElectricalStackNum)
            m_maxElectricalStackNum = camcadLayer->getElectricalStackNumber();
      }
   }

   // Is already sorted on key (layer name) so no need to re-sort

   CString *ignoredKey;
   CFatfLayer *fatfLayer = NULL;
   int fatfLayerId = 0;
   for (this->GetFirstSorted(ignoredKey, fatfLayer); fatfLayer != NULL; this->GetNextSorted(ignoredKey, fatfLayer))
   {
      fatfLayer->SetFatfLayerId( ++fatfLayerId );
   }
}

int CFatfLayerMap::GetFatfMirrorLayerId(CFatfLayer *fatfLayer)
{
   LayerStruct *camcadLayer = fatfLayer->GetCamcadLayerData();
   int camcadMirrorLayerIndx = camcadLayer->getMirroredLayerIndex();

   if (camcadLayer->getLayerIndex() != camcadMirrorLayerIndx)
   {
      LayerStruct *camcadMirrorLayer = m_doc->getLayer(camcadMirrorLayerIndx);
      if (camcadMirrorLayer != NULL)
      {
         CFatfLayer *fatfMirrorLayer = NULL;
         if (this->Lookup(camcadMirrorLayer->getName(), fatfMirrorLayer))
            return fatfMirrorLayer->GetFatfLayerId();
      }
   }

   // No mirror layer
   return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CFatfPartMap::CollectParts(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   this->RemoveAll();

   // Collect the parts
   if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypePcbComponent) || datum->isInsertType(insertTypeTestPoint))
         {
            CString refname = datum->getInsert()->getRefname();
            CString uniqueRefname = refname;
            int count = 1;
            LPCTSTR key;
            while (this->LookupKey(uniqueRefname, key))
            {
               uniqueRefname.Format("%s_%d", refname, count++);
            }

            CFatfPart *fatfPart = new CFatfPart(0, datum);
            this->SetAt(uniqueRefname, fatfPart);
         }
      }
   }

   // Sort the parts by ascending refname
   this->setSortFunction(&CFatfPartMap::AscendingRefnameSortFunc);
   this->Sort();

   // Assign the FATF part IDs
   CString *ignoredKey;
   CFatfPart *fatfPart = NULL;
   int partID = 1;
   for (this->GetFirstSorted(ignoredKey, fatfPart); fatfPart != NULL; this->GetNextSorted(ignoredKey, fatfPart)) 
	{
      fatfPart->SetFatfPartId( partID++ );
   }
}

//-----------------------------------------------------------------------------------------

int CFatfPartMap::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CFatfPart* itemA = (CFatfPart*)(((SElement*) a )->pObject->m_object);
   CFatfPart* itemB = (CFatfPart*)(((SElement*) b )->pObject->m_object);

   InsertStruct *insA = itemA->GetCamcadPartData()->getInsert();
   InsertStruct *insB = itemB->GetCamcadPartData()->getInsert();

   return (insA->getSortableRefDes().CompareNoCase(insB->getSortableRefDes()));
}

/////////////////////////////////////////////////////////////////////////////////////////////

CString CFatfVia::GetInsertedBlockNumberAsString()
{
   CString str;
   str.Format("%d", this->GetCamcadViaData()->getInsert()->getBlockNumber());
   return str;
}

void CFatfViaMap::CollectVias(CCEtoODBDoc *doc, FileStruct *pcbFile, CFatfNetMap &fatfNetMap)
{
   // Vias don't have refnames as often as they do, and in any case FATF format does not
   // have a representation for refnames for vias, so it doesn't matter even if they do.
   // SO we ignore the via refnames and just make up an in ID in order for putting vias
   // in the map, but even this ID is never output to FATF.

   this->RemoveAll();

   WORD netnameKw = doc->getStandardAttributeKeywordIndex(standardAttributeNetName);

   int fatfViaId = 1;

   // Collect the vias
   if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypeVia) || datum->isInsertType(insertTypeBondPad))
         {
            CString uniqueRefname;
            uniqueRefname.Format("%d", fatfViaId++);

            int fatfNetId = 0; // Default, valid value for "no net"
            
            Attrib *attrib = NULL;
            if (datum->lookUpAttrib(netnameKw, attrib))
            {
            #if CamCadMajorMinorVersion > 406  //  > 4.6
               CString netname = attrib->getStringValue();
            #else
               CString netname = doc->ValueArray[attrib->getStringValueIndex()];
            #endif
               CFatfNet *fatfNet = NULL;
               if (fatfNetMap.Lookup(netname, fatfNet))
               {
                  fatfNetId = fatfNet->GetFatfNetId();
               }
            }

            CFatfVia *fatfVia = new CFatfVia(fatfViaId, fatfNetId, datum);
            this->SetAt(uniqueRefname, fatfVia);
         }
      }
   }

   // Sort the vias by ascending net ID
   this->setSortFunction(&CFatfViaMap::AscendingNetIdSortFunc);
   this->Sort();
}

//-----------------------------------------------------------------------------------------

int CFatfViaMap::AscendingNetIdSortFunc(const void *a, const void *b)
{
   CFatfVia* itemA = (CFatfVia*)(((SElement*) a )->pObject->m_object);
   CFatfVia* itemB = (CFatfVia*)(((SElement*) b )->pObject->m_object);

   return (itemA->GetFatfNetId() - itemB->GetFatfNetId());
}

/////////////////////////////////////////////////////////////////////////////////////////////


CString CFatfPin::GetInsertedBlockNumberAsString()
{
   CString str;
   str.Format("%d", this->GetCamcadPinData()->getInsert()->getBlockNumber());
   return str;
}

CString CFatfPin::GetMountTechnology(CCEtoODBDoc *doc)
{
   CString mounttech;

   WORD kw = doc->getStandardAttributeKeywordIndex(standardAttributeTechnology);
   Attrib *attrib = NULL;

   if (this->GetCamcadPinData()->lookUpAttrib(kw, attrib))
   {
   #if CamCadMajorMinorVersion > 406  //  > 4.6
      mounttech = attrib->getStringValue();
   #else
      mounttech = doc->ValueArray[attrib->getStringValueIndex()];
   #endif
   }

   return mounttech;
}

//-----------------------------------------------------------------------------------------

bool CFatfPin::IsThruHole(CCEtoODBDoc *doc)
{
   CString mountTech = this->GetMountTechnology(doc);
   if (mountTech.CompareNoCase("THRU") == 0)
      return true;

   return false;
}

//-----------------------------------------------------------------------------------------

void CFatfPinMap::CollectPins(CCEtoODBDoc *doc, FileStruct *pcbFile, BlockStruct *compGeomBlk)
{
   this->RemoveAll();

   // Collect the parts
   if (doc != NULL && pcbFile != NULL && compGeomBlk != NULL)
   {
	   POSITION pos = compGeomBlk->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = compGeomBlk->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypePin))
         {
            CString refname = datum->getInsert()->getRefname();
            CString uniqueRefname = refname;
            //int count = 1;
            //char *key;
            //while (this->LookupKey(uniqueRefname, key))
            //{
            //   uniqueRefname.Format("%s_%d", refname, count++);
            //}

            CFatfPin *fatfPin = new CFatfPin(0, datum);
            this->SetAt(uniqueRefname, fatfPin);
         }
      }
   }

   // Sort the parts by ascending refname
   this->setSortFunction(&CFatfPinMap::AscendingNameSortFunc);
   this->Sort();

   // Assign the FATF part IDs
   CString *ignoredKey;
   CFatfPin *fatfPin = NULL;
   int pinID = 1;
   for (this->GetFirstSorted(ignoredKey, fatfPin); fatfPin != NULL; this->GetNextSorted(ignoredKey, fatfPin)) 
	{
      fatfPin->SetFatfPinId( pinID++ );
   }
}

//-----------------------------------------------------------------------------------------

int CFatfPinMap::AscendingNameSortFunc(const void *a, const void *b)
{
   CFatfPin* itemA = (CFatfPin*)(((SElement*) a )->pObject->m_object);
   CFatfPin* itemB = (CFatfPin*)(((SElement*) b )->pObject->m_object);

   InsertStruct *insA = itemA->GetCamcadPinData()->getInsert();
   InsertStruct *insB = itemB->GetCamcadPinData()->getInsert();

   return (insA->getSortableRefDes().CompareNoCase(insB->getSortableRefDes()));
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CFatfPadSymbolMap::CollectPadSymbols(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   this->RemoveAll();

   // Collect the parts
   if (doc != NULL)
   {
      for (int i = 0; i < doc->getNextWidthIndex(); i++)
      {  
         BlockStruct *block = doc->getWidthTable()[i];

         if (block != NULL &&
            (block->isComplexAperture() || (block->isSimpleAperture() && block->getSizeA() > 0.0))
            )
         {
            CString uniqueRefname;
            uniqueRefname.Format("%d", block->getBlockNumber());
            CFatfPadSymbol *fatfPadSym = new CFatfPadSymbol(0, block);
            this->SetAt(uniqueRefname, fatfPadSym);
         }
      }
   }

   // Sort the parts by ascending refname
   this->setSortFunction(&CFatfPadSymbolMap::AscendingNameSortFunc);
   this->Sort();

   // Assign the FATF part IDs
   CString *ignoredKey;
   CFatfPadSymbol *fatfPadSym = NULL;
   int padsymID = 1;
   for (this->GetFirstSorted(ignoredKey, fatfPadSym); fatfPadSym != NULL; this->GetNextSorted(ignoredKey, fatfPadSym)) 
	{
      fatfPadSym->SetFatfPadSymbolId( padsymID++ );
   }
}

//-----------------------------------------------------------------------------------------

int CFatfPadSymbolMap::AscendingNameSortFunc(const void *a, const void *b)
{
   CFatfPadSymbol* itemA = (CFatfPadSymbol*)(((SElement*) a )->pObject->m_object);
   CFatfPadSymbol* itemB = (CFatfPadSymbol*)(((SElement*) b )->pObject->m_object);

   BlockStruct *blkA = itemA->GetCamcadAperture();
   BlockStruct *blkB = itemB->GetCamcadAperture();

   return (blkA->getName().CompareNoCase(blkB->getName()));
}

/////////////////////////////////////////////////////////////////////////////////////////////

double CFatfPadstack::GetDrillSize(CCEtoODBDoc *doc, BlockStruct *block)
{
   double drillsize = 0.0;

   if (block != NULL)
   {
      if (block->isTool())
      {
         drillsize = block->getToolSize();
      }
      else
      {
         
         POSITION pos = block->getHeadDataInsertPosition();
         while (pos != NULL && drillsize == 0.0)
         {
            DataStruct *data = block->getNextDataInsert(pos);
            if (data->isInsert())
            {
               InsertStruct *insert = data->getInsert();
               int insertedBlkNum = insert->getBlockNumber();
               BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
               drillsize = this->GetDrillSize(doc, insertedBlk);
            }
         }
      }
   }

   return drillsize;
}

//-----------------------------------------------------------------------------------------

double CFatfPadstack::GetDrillSize(CCEtoODBDoc *doc)
{
   return this->GetDrillSize(doc, this->GetCamcadGeometryBlock() );
}

//-----------------------------------------------------------------------------------------

void CFatfPadstackMap::CollectPadstacks(CCEtoODBDoc *doc, FileStruct *pcbFile, CFatfViaMap &viaMap, CFatfPackageMap &packageMap)
{
   this->RemoveAll();

   if (doc != NULL && pcbFile != NULL)
   {
      // All vias need a pad_stack
      POSITION pos = viaMap.GetStartPosition();
	   while (pos != NULL)
      {
         CString viaKey;
         CFatfVia *fatfVia = NULL;
         viaMap.GetNextAssoc(pos, viaKey, fatfVia);

         if (fatfVia != NULL)
         {
            DataStruct *camcadViaData = fatfVia->GetCamcadViaData();
            InsertStruct *camcadViaInsert = camcadViaData->getInsert();
            int insertedBlkNum = camcadViaInsert->getBlockNumber();
            BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
            if (insertedBlk != NULL)
            {
               CString uniqueRefname;
               CFatfPadstack *existingPadstackEntry = NULL;
               uniqueRefname.Format("%d", insertedBlkNum);
               if (!this->Lookup(uniqueRefname, existingPadstackEntry))
               {
                  CFatfPadstack *fatfPadstack = new CFatfPadstack(0, insertedBlk, camcadViaData->getLayerIndex());
                  this->SetAt(uniqueRefname, fatfPadstack);
               }
            }
         }
      }

      // THRUHOLE pins need a padstack
      pos = packageMap.GetStartPosition();
      while (pos != NULL)
      {
         CString pkgKey;
         CFatfPackage *fatfPkg = NULL;
         packageMap.GetNextAssoc(pos, pkgKey, fatfPkg);

         if (fatfPkg != NULL)
         {
            POSITION pinpos = fatfPkg->GetPinMap().GetStartPosition();
            while (pinpos != NULL)
            {
               CString pinKey;
               CFatfPin *fatfPin = NULL;
               fatfPkg->GetPinMap().GetNextAssoc(pinpos, pinKey, fatfPin);
               DataStruct *camcadPinData = fatfPin->GetCamcadPinData();
               InsertStruct *camcadPinInsert = camcadPinData->getInsert();
               int insertedBlkNum = camcadPinInsert->getBlockNumber();
               BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
               if (insertedBlk != NULL)
               {
                  CString uniqueRefname;
                  CFatfPadstack *existingPadstackEntry = NULL;
                  uniqueRefname.Format("%d", insertedBlkNum);
                  if (!this->Lookup(uniqueRefname, existingPadstackEntry))
                  {
                     CFatfPadstack *fatfPadstack = new CFatfPadstack(0, insertedBlk, camcadPinData->getLayerIndex());

                     if (fatfPin->IsThruHole(doc) || fatfPadstack->GetDrillSize(doc) > 0.0)
                     {
                        this->SetAt(uniqueRefname, fatfPadstack);
                     }
                     else
                     {
                        delete fatfPadstack;
                     }
                  }
               }
            }
         }
      }
   }

   // Sort the parts by ascending refname
   this->setSortFunction(&CFatfPadstackMap::AscendingNameSortFunc);
   this->Sort();

   // Assign the FATF part IDs
   CString *ignoredKey;
   CFatfPadstack *fatfPadstack = NULL;
   int padstackID = 1;
   for (this->GetFirstSorted(ignoredKey, fatfPadstack); fatfPadstack != NULL; this->GetNextSorted(ignoredKey, fatfPadstack)) 
	{
      fatfPadstack->SetFatfPadstackId( padstackID++ );
   }
}

//-----------------------------------------------------------------------------------------

int CFatfPadstackMap::AscendingNameSortFunc(const void *a, const void *b)
{
   CFatfPadstack* itemA = (CFatfPadstack*)(((SElement*) a )->pObject->m_object);
   CFatfPadstack* itemB = (CFatfPadstack*)(((SElement*) b )->pObject->m_object);

   //InsertStruct *insA = itemA->GetCamcadPinData()->getInsert();
   //InsertStruct *insB = itemB->GetCamcadPinData()->getInsert();

   return (itemA->GetFatfPadstackName().CompareNoCase( itemB->GetFatfPadstackName() ));
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CFatfPackageMap::CollectPackages(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   // Select package blocks based on their being inserted by a pcb component, as opposed
   // to scanning for blocks of type pcb component. The latter is unreliable, component 
   // inserts can insert any kind of block, and there may be extra (unused) blocks of
   // type pcb component that we don't need.

   this->RemoveAll();

   CMapStringToInt blockNames;

   if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypePcbComponent) || datum->isInsertType(insertTypeTestPoint))
         {
            int insertedBlkNum = datum->getInsert()->getBlockNumber();
            
            CString blkNumStr;
            blkNumStr.Format("%d", insertedBlkNum);

            // If block not already in list, add it
            CFatfPackage *existingBlkEntry = NULL;
            if (!this->Lookup(blkNumStr, existingBlkEntry))
            {
               BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
               if (insertedBlk != NULL)
               {
                  // Default pkg name same as block name
                  CString pkgName( insertedBlk->getName() );

                  // Ensure unique package name
                  int blkNum;
                  if (blockNames.Lookup(pkgName, blkNum))
                  {
                     // Same name is okay if is same block num, if different then swizzle name
                     if (blkNum != insertedBlkNum)
                     {
                        pkgName.Format("%s_%d", insertedBlk->getName(), insertedBlkNum);    
                     }
                  }
                  blockNames.SetAt(pkgName, insertedBlkNum);

                  CFatfPackage *fatfPkg = new CFatfPackage(0, insertedBlk);
                  fatfPkg->SetFatfPackageName(pkgName);
                  fatfPkg->CollectPins(doc, pcbFile, insertedBlk);
                  this->SetAt(blkNumStr, fatfPkg);
               }
            }
         }
      }
   }

   blockNames.RemoveAll();

   // Sort the packages by ascending name
   this->setSortFunction(&CFatfPackageMap::AscendingNameSortFunc);
   this->Sort();

   // Assign the FATF package IDs
   CString *ignoredKey;
   CFatfPackage *fatfPkg = NULL;
   int pkgID = 1;
   for (this->GetFirstSorted(ignoredKey, fatfPkg); fatfPkg != NULL; this->GetNextSorted(ignoredKey, fatfPkg)) 
	{
      fatfPkg->SetFatfPkgId( pkgID++ );
   }
}

//-----------------------------------------------------------------------------------------

int CFatfPackageMap::AscendingNameSortFunc(const void *a, const void *b)
{
   CFatfPackage* itemA = (CFatfPackage*)(((SElement*) a )->pObject->m_object);
   CFatfPackage* itemB = (CFatfPackage*)(((SElement*) b )->pObject->m_object);

   BlockStruct *AAA = itemA->GetCamcadGeometryBlock();
   BlockStruct *BBB = itemB->GetCamcadGeometryBlock();

   return (AAA->getName().CompareNoCase(BBB->getName()));
}

/////////////////////////////////////////////////////////////////////////////////////////////
CString CFatfNet::GetFatfNetType(CCEtoODBDoc *doc)
{
   CString fatfnettype("S");  // "S"ignal by default

   WORD netTypeKw = doc->getStandardAttributeKeywordIndex(standardAttributeNetType);
   Attrib *attrib = NULL;

   if (this->GetCamcadNet()->lookUpAttrib(netTypeKw, attrib))
   {
   #if CamCadMajorMinorVersion > 406  //  > 4.6
      CString camcadNetType = attrib->getStringValue();
   #else
      CString camcadNetType = doc->ValueArray[attrib->getStringValueIndex()];
   #endif

      if (camcadNetType.CompareNoCase("POWER") == 0 || camcadNetType.CompareNoCase("GROUND") == 0)
      {
         fatfnettype = "P";  // in fatf both power and ground classified as "P"ower.
      }
   }

   return fatfnettype;
}

//-----------------------------------------------------------------------------------------

void CFatfNetMap::CollectNets(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   this->RemoveAll();

   // Collect the nets
   if (pcbFile != NULL)
   {
      POSITION netPos = pcbFile->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = pcbFile->getNetList().GetNext(netPos);

         // Marks says use all nets, even unused nets
         // Was limited to used nets, as: if (!(net->getFlags() & NETFLAG_UNUSEDNET))
         {
            CString name = net->getNetName();
            CString uniqueName = name;
            int count = 1;
            LPCTSTR key;
            while (this->LookupKey(uniqueName, key))
            {
               uniqueName.Format("%s_%d", name, count++);
            }

            CFatfNet *fatfNet = new CFatfNet(0, net);
            this->SetAt(uniqueName, fatfNet);
         }
      }
   }

   // Sort the nets by ascending refname
   this->setSortFunction(&CFatfNetMap::AscendingNetnameSortFunc);
   this->Sort();

#ifdef CANT_DO_THIS_HERE
   // Assign the FATF net IDs
   CString *ignoredKey;
   CFatfNet *fatfNet = NULL;
   int netID = 1;
   for (this->GetFirstSorted(ignoredKey, fatfNet); fatfNet != NULL; this->GetNextSorted(ignoredKey, fatfNet)) 
	{
      fatfNet->SetFatfNetId( netID++ );
   }
#endif
}

//-----------------------------------------------------------------------------------------

int CFatfNetMap::AscendingNetnameSortFunc(const void *a, const void *b)
{
   CFatfNet* itemA = (CFatfNet*)(((SElement*) a )->pObject->m_object);
   CFatfNet* itemB = (CFatfNet*)(((SElement*) b )->pObject->m_object);

   return (itemA->GetCamcadNet()->getNetName().CompareNoCase(itemB->GetCamcadNet()->getNetName()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

CFabmasterFATFWriter::CFabmasterFATFWriter(CCEtoODBDoc *doc, FileStruct *pcbFile)
: m_doc(doc)
, m_pcbFile(pcbFile)
{
   m_linearScaleFactor = getUnitsFactor(m_doc->getSettings().getPageUnits(), pageUnitsMils) * 10.0;
}

void CFabmasterFATFWriter::InitData()
{
   this->m_fatfLayerMap.CollectLayers(m_doc, m_pcbFile);
   this->m_fatfPartMap.CollectParts(m_doc, m_pcbFile);
   this->m_fatfPackageMap.CollectPackages(m_doc, m_pcbFile);
   this->m_fatfNetMap.CollectNets(m_doc, m_pcbFile);
   this->m_fatfViaMap.CollectVias(m_doc, m_pcbFile, m_fatfNetMap);
   this->m_fatfPadstackMap.CollectPadstacks(m_doc, m_pcbFile, m_fatfViaMap, m_fatfPackageMap);
   this->m_fatfPadSymbolMap.CollectPadSymbols(m_doc, m_pcbFile);

   AssignNetIDs();
   MarkSpecialLayers();
}

void CFabmasterFATFWriter::MarkCompOutlineLayers(BlockStruct *camcadGeom, bool isOutline)
{
   if (camcadGeom != NULL)
   {
      POSITION dataPos = camcadGeom->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = camcadGeom->getDataList().GetNext(dataPos);
         if (data != NULL)
         {
            // Is outline if parent was an outline or this data is an outline
            bool dataIsOutline = isOutline || data->getGraphicClass() == graphicClassComponentOutline;

            switch (data->getDataType())
            {
            case dataTypePoly:
               if (dataIsOutline)
               {
                  int layerIndx = data->getLayerIndex();
                  LayerStruct *camcadLayer = m_doc->FindLayer(layerIndx);
                  CFatfLayer *fatfLayer = NULL;
                  if (camcadLayer != NULL && this->m_fatfLayerMap.Lookup( camcadLayer->getName(), fatfLayer ))
                  {
                     fatfLayer->SetFatfLayerType("ASSEMBLY");
                  }
               }
               break;

            case dataTypeInsert:
               {
                  BlockStruct *insertedBlk = this->m_doc->getBlockAt( data->getInsert()->getBlockNumber() );
                  MarkCompOutlineLayers(insertedBlk, dataIsOutline);
               }
               break;

            case dataTypeDraw:
            case dataTypeText:
            case dataTypeTool:
            case dataTypePoint:
            case dataTypeBlob:
            default:
               /* no op*/
               break;
            }
         }
      }
   }
}

void CFabmasterFATFWriter::MarkSpecialLayers()
{
   // Layer mapping can not be done purely on Layer data, some of the inserts
   // have to be examined and map Layers to fatf layers depending on how the
   // inserts use the layers.

   // Layer used by parts as component outline need to use a layer of fatf type ASSEMBLY.
   // Scan packages for entities of graphic type Primary Component Outline and mark the
   // layer they use as ASSEMBLY.

   CString *ignoredKey;
   CFatfPackage *fatfPkg = NULL;

   for (this->m_fatfPackageMap.GetFirstSorted(ignoredKey, fatfPkg); fatfPkg != NULL; this->m_fatfPackageMap.GetNextSorted(ignoredKey, fatfPkg)) 
	{
      BlockStruct *camcadGeom = fatfPkg->GetCamcadGeometryBlock();
      MarkCompOutlineLayers(camcadGeom, false);
   }
}

void CFabmasterFATFWriter::WriteFile(CString filename)
{
	CFormatStdioFile outFile;
	CFileException e;
	
	if (!outFile.Open(filename, CFile::modeCreate|CFile::modeWrite, &e))
   {
      CString msg;
      msg.Format("Can not open \"%s\" for writing.", filename);
      ErrorMessage(msg, "");
   }
   else
   {
      WriteHeader(outFile);
      WriteBoardData(outFile);
      WriteLayerNames(outFile);
      WritePackages(outFile);
      WriteParts(outFile);
      WriteNets(outFile);
      WriteNetTraces(outFile);
      WritePadSymbols(outFile);
      WriteLayerSets(outFile);
      WritePadStacks(outFile);
      WritePads(outFile); // aka vias
      WriteFonts(outFile);

      outFile.WriteString("\n:EOF\n");

      outFile.Close();
   }
}

int CFabmasterFATFWriter::ScaleToFatf(double camcadValue)
{
   return round( camcadValue * m_linearScaleFactor );
}

void CFabmasterFATFWriter::WriteHeader(CFormatStdioFile &outFile)
{
   outFile.WriteString("; Produced by CamCad Professional %s\n", getApp().getVersionString());
   outFile.WriteString(":FABMASTER FATF REV 11.1;\n");
   outFile.WriteString(":UNITS = 1/10000 INCH;\n");
   outFile.WriteString("\n");
}

double CFabmasterFATFWriter::GetCamcadBoardThickness()
{
   double thickness = 0.0;

   if (m_doc != NULL && m_pcbFile != NULL && m_pcbFile->getBlock())
   {
      Attrib *a;
      if (a = is_attvalue(m_doc, m_pcbFile->getBlock()->getAttributesRef(), BOARD_THICKNESS, 2))   
      {
         CString l = get_attvalue_string(m_doc, a);
         thickness = atof(l);
      }
   }

   return thickness;
}

void CFabmasterFATFWriter::WriteBoardData(CFormatStdioFile &outFile)
{
   int recId = 1;

   CFilePath filepath( outFile.GetFileName() );
   CTime currentTime = CTime::GetCurrentTime();

   outFile.WriteString(":BOARD_DATA\n");

   outFile.WriteString("%d", recId++);
   outFile.WriteString(",JOB(\"%.64s\"", filepath.getBaseFileName());
   outFile.WriteString(","); // Optional VERSION left empty
   outFile.WriteString(",%s", currentTime.Format("%d-%m-%Y")); // Created date (optional)
   outFile.WriteString(",%s", currentTime.Format("%d-%m-%Y")); // Nodified date (required)
   outFile.WriteString(");\n");

   WriteBoardOutline(outFile, recId);

   WriteFiducials(outFile, recId);

   double boardThickness = GetCamcadBoardThickness();
   if (boardThickness > 0.0)
      outFile.WriteString("%d,THICKNESS,%d;\n", recId++, ScaleToFatf(boardThickness));

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WriteFiducials(CFormatStdioFile &outFile, int &recId)
{
   if (m_pcbFile != NULL && m_pcbFile->getBlock() != NULL)
   {
      int count = 0;
      bool mirror = false;
      BlockStruct *pcbBlk = m_pcbFile->getBlock();
      POSITION pcbDataPos = pcbBlk->getHeadDataInsertPosition();
      while (pcbDataPos != NULL)
      {
         DataStruct *data = pcbBlk->getNextDataInsert(pcbDataPos);

         if (data != NULL && data->isInsertType(insertTypeFiducial))
         {
            if (count == 0)
               outFile.WriteString("%d,FIDUCIALS (", recId++);
            else if ((count % 5) == 0)
               outFile.WriteString("\n     ");

            outFile.WriteString("%s(%d,%d)",
               (count > 0) ? "," : "",
               ScaleToFatf(data->getInsert()->getOriginX()),
               ScaleToFatf(data->getInsert()->getOriginY()));

            count++;
         }
      }
      if (count > 0)
         outFile.WriteString(");\n");
   }
}

void CFabmasterFATFWriter::WriteBoardOutline(CFormatStdioFile &outFile, int &recId)
{
   // FATF format supports more than one COUNTOUR for the board outline, so we output
   // a COUNTOUR record for each poly of appropriate graphic class.'

   // This code lifted from elsewhere, the guts are kinda messy, lifted from fatf_out.cpp, maybe make this nicer?

   if (m_pcbFile != NULL && m_pcbFile->getBlock() != NULL)
   {
      bool mirror = false;
      BlockStruct *pcbBlk = m_pcbFile->getBlock();
      POSITION pcbDataPos = pcbBlk->getDataList().GetHeadPosition();
      while (pcbDataPos != NULL)
      {
         DataStruct *data = pcbBlk->getDataList().GetNext(pcbDataPos);

         if (data != NULL && data->getDataType() == dataTypePoly &&
            data->getGraphicClass() == graphicClassBoardOutline)
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               Point2   p;

               int first = TRUE; 
               int cnt = 0;
               outFile.WriteString("%d,CONTOUR (", recId++);

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
                  p.x = pnt->x;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y;
                  p.bulge = pnt->bulge; // NOT handling curves !!!!!!!!!!!!!!!!!
                  //TransPoint2(&p, 1, &m, insert_x, insert_y);
                  if (cnt > 1)
                     outFile.WriteString(",");
                  if ((cnt % 5) == 0)
                     outFile.WriteString("\n");

                  outFile.WriteString("(%d,%d,0)", ScaleToFatf(p.x), ScaleToFatf(p.y));
                  first = FALSE;
               }
               outFile.WriteString(");\n");
            }
         }
      }
   }
}
/*
DataStruct *CAcculogicBoard::GetBoardOutline()
{
   // Patterned after CBoardInstance in panelization.cpp

   if (m_pcbBlock != NULL)
   {
      POSITION pos = m_pcbBlock->getDataList().GetHeadPosition();
      while (pos != NULL)
      {
         DataStruct *data = m_pcbBlock->getDataList().GetNext(pos);

         if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
         {
            return data;
         }
      }
   }

   return NULL;
}
*/

CFatfFont *CFatfFontMap::GetFatfFont(int fatfFontHeight, int fatfFontWidth, int fatfPenWidth)
{
   CString key;
   key.Format("%d_%d_%d", fatfFontHeight, fatfFontWidth, fatfPenWidth);

   CFatfFont *fatfFont = NULL;
   this->Lookup(key, fatfFont);

   if (fatfFont == NULL)
   {
      fatfFont = new CFatfFont((this->GetCount() + 1), fatfFontHeight, fatfFontWidth, fatfPenWidth);
      this->SetAt(key, fatfFont);
   }

   return fatfFont;
}

int CFatfFontMap::AscendingIdSortFunc(const void *a, const void *b)
{
   CFatfFont* itemA = (CFatfFont*)(((SElement*) a )->pObject->m_object);
   CFatfFont* itemB = (CFatfFont*)(((SElement*) b )->pObject->m_object);

   return (itemA->GetFatfFontId() - itemB->GetFatfFontId());
}

void CFabmasterFATFWriter::WriteFonts(CFormatStdioFile &outFile)
{
   // This write func should be called last, or at least after all that may write text graphics.
   // Fonts are collected during output of other sections, then output at end here.

   this->m_fatfFontMap.setSortFunction(&CFatfFontMap::AscendingIdSortFunc);
   this->m_fatfFontMap.Sort();

   outFile.WriteString(":FONTS\n");

   double textSpaceRatio = m_doc->getSettings().getTextSpaceRatio();

   CString *ignoredKey;
   CFatfFont *fatfFont = NULL;

   for (this->m_fatfFontMap.GetFirstSorted(ignoredKey, fatfFont); fatfFont != NULL; this->m_fatfFontMap.GetNextSorted(ignoredKey, fatfFont)) 
	{
      outFile.WriteString("%d", fatfFont->GetFatfFontId());
      outFile.WriteString(",\"FONT%d\"", fatfFont->GetFatfFontId());
      outFile.WriteString(",%d", fatfFont->GetFatfCharHeight());
      outFile.WriteString(",%d", fatfFont->GetFatfCharWidth());
      outFile.WriteString(",%d", round(fatfFont->GetFatfCharWidth() * (1.0 + textSpaceRatio)) );
      outFile.WriteString(",%d", fatfFont->GetFatfPenWidth());
      outFile.WriteString(";\n");
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WriteParts(CFormatStdioFile &outFile)
{
   outFile.WriteString(":PARTS\n");

   CString *ignoredKey;
   CFatfPart *fatfPart = NULL;

   for (this->m_fatfPartMap.GetFirstSorted(ignoredKey, fatfPart); fatfPart != NULL; this->m_fatfPartMap.GetNextSorted(ignoredKey, fatfPart)) 
	{
      

      CFatfPackage *fatfPkg = NULL;
      m_fatfPackageMap.Lookup(fatfPart->GetInsertedBlockNumberAsString(), fatfPkg);

      double angle = fatfPart->GetCamcadPartData()->getInsert()->getAngleDegrees();
      if (fatfPart->GetCamcadPartData()->getInsert()->getGraphicMirrored())
         angle = 360.0 - angle;  // reverse direction for mirrord insert

      // fatf handles bottom differently, nothing explicit in fatf format spec, adjustment
      // by 180 is as per found in our fatf reader
      if (fatfPart->GetCamcadPartData()->getInsert()->getGraphicMirrored())
         angle = 180.0 - angle;

      angle = normalizeDegrees(angle);


      outFile.WriteString("%d", fatfPart->GetFatfPartId());
      outFile.WriteString(",\"%s\"", fatfPart->GetCamcadPartData()->getInsert()->getRefname());
      outFile.WriteString(",\"%s\"", fatfPart->GetDeviceName(this->m_doc));
      outFile.WriteString(",\"%s\"", fatfPkg != NULL ? fatfPkg->GetFatfPackageName() : "MissingGeometryBlock");
      outFile.WriteString(",%d", ScaleToFatf( fatfPart->GetCamcadPartData()->getInsert()->getOriginX() ));
      outFile.WriteString(",%d", ScaleToFatf( fatfPart->GetCamcadPartData()->getInsert()->getOriginY() ));
      // note that fatf positive rotation direction is clockwise, opposite of camcad
      outFile.WriteString(",%d", round( (360.0 - angle) * 10.0) ); 
      outFile.WriteString(",%s", fatfPart->GetCamcadPartData()->getInsert()->getPlacedTop() ? "T" : "B"); // Add pcb file mirror support... someday.
      outFile.WriteString(";\n");
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WritePadSymbols(CFormatStdioFile &outFile)
{
   outFile.WriteString(":PAD_SYMBOLS\n");
   
   CString *ignoredKey;
   CFatfPadSymbol *fatfPadSym = NULL;

   for (this->m_fatfPadSymbolMap.GetFirstSorted(ignoredKey, fatfPadSym); fatfPadSym != NULL; this->m_fatfPadSymbolMap.GetNextSorted(ignoredKey, fatfPadSym)) 
	{
      CString shapeDescriptor("Undefined");

      BlockStruct *camcadAperture = fatfPadSym->GetCamcadAperture();

      switch (camcadAperture->getShape())
      {
      case apertureRound:
         {
            shapeDescriptor.Format("P_ROUND(%d)", ScaleToFatf(camcadAperture->getSizeA()));
         }
         break;
      case apertureRectangle:
         {
            // Wait for square supporting rectangle 
            // Draw the squre by short side of the rectangle in order to avoid overlap
            double minSize = min(camcadAperture->getSizeA(), camcadAperture->getSizeB()); // punt
            int halfsize = ScaleToFatf(minSize / 2.0);
            shapeDescriptor.Format("P_BLOCK(%d,%d,%d,%d)", -halfsize, -halfsize, halfsize, halfsize);
         }
         break;
      case apertureSquare:
         {
            int halfsize = ScaleToFatf(camcadAperture->getSizeA() / 2.0);
            shapeDescriptor.Format("P_BLOCK(%d,%d,%d,%d)", -halfsize, -halfsize, halfsize, halfsize);
         }
         break;
      case apertureOblong:
         {
            Point2 point2;
            double sizeA = camcadAperture->getSizeA();
            double sizeB = camcadAperture->getSizeB();
            double len = ( (sizeA - sizeB) );/// 2.0 );
            double rot = camcadAperture->getRotationDegrees();
            double x1 = point2.x - (len/2.0);
            double y1 = point2.y;
            double x2 = point2.x + (len/2.0);
            double y2 = point2.y;
            if (fpeq(rot, 90.0, 1.0) || fpeq(rot, 270.0, 1.0)) // 90 or 270 within one degree
            {
               x1 = point2.x;
               y1 = point2.y - (len/2.0);
               x2 = point2.x;
               y2 = point2.y + (len/2.0);
            }
            shapeDescriptor.Format("TRACK( %d (%d,%d)(%d,%d))",
               ScaleToFatf(sizeB), // width
               ScaleToFatf( x1 ), ScaleToFatf( y1 ),
               ScaleToFatf( x2 ), ScaleToFatf( y2 ));
         }
         break;
      case apertureComplex:
         {
            double diameter = 0.0;
            BlockStruct *complexBlock = m_doc->getBlockAt( camcadAperture->getComplexApertureSubBlockNumber() );
            if (complexBlock != NULL)
            {
            #if CamCadMajorMinorVersion > 406  //  > 4.6
               CExtent ext = complexBlock->getExtent(m_doc->getCamCadData());
            #else
               CExtent ext = complexBlock->getExtent(*m_doc);
            #endif   
               if (ext.isValid())
                  diameter = (ext.getXsize() + ext.getYsize()) / 2.0; // average of extent
            }
            if (diameter == 0.0)
               diameter = m_doc->convertToPageUnits(pageUnitsInches, 0.010);

            shapeDescriptor.Format("P_ROUND(%d)", ScaleToFatf( diameter ));
         }
         break;

      case apertureTarget:
      case apertureThermal: 
      case apertureDonut:
      case apertureOctagon:
         {
            double diameter = camcadAperture->getSizeA(); // punt
            shapeDescriptor.Format("P_ROUND(%d)", ScaleToFatf( diameter ));
         }
         break;

      case apertureBlank:
      case apertureUndefined:
      case apertureUnknown:
      default:
         {
            double diameter = m_doc->convertToPageUnits(pageUnitsInches, 0.010);
            shapeDescriptor.Format("P_ROUND(%d)", ScaleToFatf( diameter ));
         }
         break;
      }


      outFile.WriteString("%d,%s;\n", 
         fatfPadSym->GetFatfPadSymbolId(),
         shapeDescriptor);
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WriteLayerSets(CFormatStdioFile &outFile)
{
   // punt - would have to analyze how padstacks use layers to come up with
   // meaningful layer sets. Instead, make each layer its own set and in padstacks
   // place each aperture on its own layer. We'll see if this works out.

   outFile.WriteString(":LAYER_SETS\n");

   CString *ignoredKey;
   CFatfLayer *fatfLayer = NULL;

   for (this->m_fatfLayerMap.GetFirstSorted(ignoredKey, fatfLayer); fatfLayer != NULL; this->m_fatfLayerMap.GetNextSorted(ignoredKey, fatfLayer))
   {
      outFile.WriteString("%d", fatfLayer->GetFatfLayerId());
      outFile.WriteString(",\"%s\"", fatfLayer->GetCamcadLayerData()->getName());
      outFile.WriteString(",(%d)", fatfLayer->GetFatfLayerId());
      outFile.WriteString(";\n");
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WritePadStackPads(CFormatStdioFile &outFile, BlockStruct *padstackBlock, LayerStruct *layer, int &count)
{
   if (padstackBlock != NULL)
   {
      if (padstackBlock->isAperture())
      {
         int fatfLayerId = 0;
         CFatfLayer *fatfLayer = NULL;
         if (layer != NULL && this->m_fatfLayerMap.Lookup( layer->getName(), fatfLayer ))
            fatfLayerId = fatfLayer->GetFatfLayerId();

         int fatfPadSymId = 0;
         CFatfPadSymbol *fatfPadSym = NULL;
         CString padSymKey;
         padSymKey.Format("%d", padstackBlock->getBlockNumber());
         if (this->m_fatfPadSymbolMap.Lookup(padSymKey, fatfPadSym))
            fatfPadSymId = fatfPadSym->GetFatfPadSymbolId();

         // Note that incrementing before mod operation for newline puts
         // out one less field in 1st line than in subsequent lines.
         // We want this side effect, as first line has initial text
         // that is not on rest of lines, so this compensates a little.

         count++;
         if (count > 1)
            outFile.WriteString(",");
         if ((count % 8) == 0)
            outFile.WriteString("\n   ");

         outFile.WriteString("(%d,%d)", fatfLayerId, fatfPadSymId);
      }
      else
      {
         POSITION pos = padstackBlock->getHeadDataInsertPosition();
         while (pos != NULL)
         {
            DataStruct *data = padstackBlock->getNextDataInsert(pos);
            if (data->isInsert())
            {
               LayerStruct *insertLayer= NULL;
               // Use insert layer of the padstack if the pad layer is floating layer
               if(m_doc->IsFloatingLayer(data->getLayerIndex()) && layer && layer->getLayerIndex() != -1)
                  insertLayer = m_doc->getLayer( layer->getLayerIndex() );
               else
                  insertLayer = m_doc->getLayer( data->getLayerIndex() );

               BlockStruct *insertedBlk = m_doc->getBlockAt( data->getInsert()->getBlockNumber() );
               WritePadStackPads(outFile, insertedBlk, insertLayer, count);
            }
         }
      }
   }
}

void CFabmasterFATFWriter::WritePadStacks(CFormatStdioFile &outFile)
{
   outFile.WriteString(":PAD_STACKS\n");
   
   CString *ignoredKey;
   CFatfPadstack *fatfPadstack = NULL;

   for (this->m_fatfPadstackMap.GetFirstSorted(ignoredKey, fatfPadstack); fatfPadstack != NULL; this->m_fatfPadstackMap.GetNextSorted(ignoredKey, fatfPadstack)) 
	{
      outFile.WriteString("%d,\"%s\",%d,%s,", 
         fatfPadstack->GetFatfPadstackId(),
         fatfPadstack->GetCamcadGeometryBlock()->getName(),
         ScaleToFatf(fatfPadstack->GetDrillSize(m_doc)),
         "P" /*plated*/
         );

      outFile.WriteString("(");
      int padcount = 0;
      WritePadStackPads(outFile, fatfPadstack->GetCamcadGeometryBlock(), m_doc->getLayer(fatfPadstack->GetInsertlayerId()), padcount);
      outFile.WriteString(")");

      outFile.WriteString(";\n");
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WritePads(CFormatStdioFile &outFile)
{
   outFile.WriteString(":PADS\n");

   CString *ignoredKey;
   CFatfVia *fatfVia = NULL;

   for (this->m_fatfViaMap.GetFirstSorted(ignoredKey, fatfVia); fatfVia != NULL; this->m_fatfViaMap.GetNextSorted(ignoredKey, fatfVia)) 
	{
      //double angle = fatfPart->GetCamcadPartData()->getInsert()->getAngleDegrees();
      //if (fatfPart->GetCamcadPartData()->getInsert()->getGraphicMirrored())
      //   angle = 360.0 - angle;  // reverse direction for mirrord insert

      // fatf handles bottom differently, nothing explicit in fatf format spec, adjustment
      // by 180 is as per found in our fatf reader
      //if (fatfPart->GetCamcadPartData()->getInsert()->getGraphicMirrored())
      //   angle = 180.0 - angle;

      //angle = normalizeDegrees(angle);

      CFatfPadstack *fatfPadstack = NULL;
      int fatfPadstackId = 0;
      if (m_fatfPadstackMap.Lookup( fatfVia->GetInsertedBlockNumberAsString(), fatfPadstack ))
         fatfPadstackId = fatfPadstack->GetFatfPadstackId();

      outFile.WriteString("%d", fatfVia->GetFatfNetId());
      outFile.WriteString(",%d", fatfPadstackId);
      outFile.WriteString(",("); // Begin list of pad locations
      outFile.WriteString("(%d",  ScaleToFatf( fatfVia->GetCamcadViaData()->getInsert()->getOriginX() ));
      outFile.WriteString(",%d)", ScaleToFatf( fatfVia->GetCamcadViaData()->getInsert()->getOriginY() ));
      outFile.WriteString(")"); // End list of pad locations
      outFile.WriteString(";\n");
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WritePackages(CFormatStdioFile &outFile)
{
   outFile.WriteString(":PACKAGES\n");

   CString *ignoredKey;
   CFatfPackage *fatfPkg = NULL;

   for (this->m_fatfPackageMap.GetFirstSorted(ignoredKey, fatfPkg); fatfPkg != NULL; this->m_fatfPackageMap.GetNextSorted(ignoredKey, fatfPkg)) 
	{
      outFile.WriteString("; %s\n", fatfPkg->GetFatfPackageName()); // A comment line, common in several sample fatf files, not in format spec

      outFile.WriteString("%d", fatfPkg->GetFatfPkgId());
      outFile.WriteString(",\"%s\"", fatfPkg->GetFatfPackageName());
      outFile.WriteString(",,,,"); // Xmin,Xmax,Ymin,Ymax - format spec says these are unused
      outFile.WriteString("\n");

      CString indent("  ");
      if (fatfPkg->GetPinMap().GetCount() > 0)
      {
         outFile.WriteString("%s(PINS ", indent);

         CString *ignoredPinKey;
         CFatfPin *fatfPin = NULL;
         int count = 0;
         int totalCount = fatfPkg->GetPinMap().GetCount();
         for (fatfPkg->GetPinMap().GetFirstSorted(ignoredPinKey, fatfPin); fatfPin != NULL; fatfPkg->GetPinMap().GetNextSorted(ignoredPinKey, fatfPin))
         {
            count++;

            outFile.WriteString("(%d", fatfPin->GetFatfPinId());
            outFile.WriteString(",\"%s\"", fatfPin->GetCamcadPinData()->getInsert()->getRefname());
            outFile.WriteString(",%d", ScaleToFatf( fatfPin->GetCamcadPinData()->getInsert()->getOriginX() ));
            outFile.WriteString(",%d", ScaleToFatf( fatfPin->GetCamcadPinData()->getInsert()->getOriginY() ));
            outFile.WriteString(",%s)", fatfPin->IsThruHole(m_doc) ? "D" : fatfPin->GetCamcadPinData()->getInsert()->getPlacedBottom() ? "B" : "T" );

            if (count < totalCount)
               outFile.WriteString(",%s", (count % 4) == 0 ? ("\n" + indent) : "");
            else
               outFile.WriteString(")\n");
         }

         outFile.WriteString("  (");
         indent.Empty(); // no indent for first entry, it's handled with paren above

         // Write padstack references for TH pins
         for (fatfPkg->GetPinMap().GetFirstSorted(ignoredPinKey, fatfPin); fatfPin != NULL; fatfPkg->GetPinMap().GetNextSorted(ignoredPinKey, fatfPin))
         {
            if (fatfPin->IsThruHole(m_doc))
            {
               int padstackId = 0;
               CFatfPadstack *fatfPadstack = NULL;
               if (this->m_fatfPadstackMap.Lookup( fatfPin->GetInsertedBlockNumberAsString(), fatfPadstack) )
                  padstackId = fatfPadstack->GetFatfPadstackId();

               outFile.WriteString("%s%d, PAD(%d((%d,%d)))\n",
                  indent,
                  fatfPin->GetFatfPinId(),
                  padstackId,
                  ScaleToFatf( fatfPin->GetCamcadPinData()->getInsert()->getOriginX() ),
                  ScaleToFatf( fatfPin->GetCamcadPinData()->getInsert()->getOriginY() ));

               indent = "  "; // indent for rest of section
            }
         }
      }
      else
      {
         // If there were no pins then need to provide open paren for graphical section.
         // If there were pins then the TH padstack reference section did this already.
         outFile.WriteString("  (");
      }

      // Write SMD pads and whatever else is left
      WriteGraphicBlock(outFile, fatfPkg, fatfPkg->GetCamcadGeometryBlock(),
         0.0, 0.0, 0.0, FALSE, 1.0, -1, 0, 0);

      outFile.WriteString(");\n");
   }

   outFile.WriteString(":EOD\n\n");
}



void CFabmasterFATFWriter::AssignNetIDs()
{
   CString *ignoredKey;
   CFatfNet *fatfNet = NULL;

   int goodFatfNetID = 1;

   for (this->m_fatfNetMap.GetFirstSorted(ignoredKey, fatfNet); fatfNet != NULL; this->m_fatfNetMap.GetNextSorted(ignoredKey, fatfNet)) 
   {
      // Net can be used only if one or more comp pins are present.
      // Just has to have at least one to qualify.
      
      fatfNet->SetFatfNetId( -10203 ); // Mark as bad net, -10203 means nothing special, just easily stands out for inspection

      if (fatfNet->GetCamcadNet()->getCompPinCount() > 0)
      {
         POSITION compPinPos = fatfNet->GetCamcadNet()->getHeadCompPinPosition();
         int count = 0;
         while (compPinPos != NULL && fatfNet->GetFatfNetId() < 1)
         {
            CompPinStruct *compPin = fatfNet->GetCamcadNet()->getNextCompPin(compPinPos);
            CFatfPart *fatfPart = NULL;
            if (this->m_fatfPartMap.Lookup(compPin->getRefDes(), fatfPart))
            {
               CFatfPin *fatfPin = NULL;
               CFatfPackage *fatfPkg = NULL;
               if (GetFatfPackageAndPin(fatfPart, compPin->getPinName(), fatfPkg, fatfPin))
               {
                  fatfNet->SetFatfNetId( goodFatfNetID++ );
               }
            }
         }
      }
   }
}

void CFabmasterFATFWriter::WriteNets(CFormatStdioFile &outFile)
{
   outFile.WriteString(":NETS\n");

   CString *ignoredKey;
   CFatfNet *fatfNet = NULL;

   for (this->m_fatfNetMap.GetFirstSorted(ignoredKey, fatfNet); fatfNet != NULL; this->m_fatfNetMap.GetNextSorted(ignoredKey, fatfNet)) 
   {
      // Write net only if one or more comp pins are present.
      // Note that there is another "at least one valid comppin" check later, i.e. not record is output
      // unless the comppin data exists in the other lists. This came about due to an imported Allegro
      // cad sample in which there was a net with comppins, but the refname refered to an insert that
      // was type Tooling, and so in combination formed invalid camcad data. (A comppin can only refer
      // to a PCB COmponent or a Test Point). So in short, we had a comp pin with no PCB component to 
      // reference, this resulted in a FATF net definition with no entries, which may itself be
      // valid or invalid (it is inconclusive from the format spec) but in any case is useless.

      if (fatfNet->GetCamcadNet()->getCompPinCount() > 0 && fatfNet->GetFatfNetId() > 0)
      {
         POSITION compPinPos = fatfNet->GetCamcadNet()->getHeadCompPinPosition();
         int count = 0;
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = fatfNet->GetCamcadNet()->getNextCompPin(compPinPos);
            CFatfPart *fatfPart = NULL;
            if (this->m_fatfPartMap.Lookup(compPin->getRefDes(), fatfPart))
            {
               CFatfPin *fatfPin = NULL;
               CFatfPackage *fatfPkg = NULL;
               if (GetFatfPackageAndPin(fatfPart, compPin->getPinName(), fatfPkg, fatfPin))
               {
                  if (count == 0)
                  {
                     // write header, but only now that we've determined there is at least one
                     // valid comppin to output
                     outFile.WriteString("%d", fatfNet->GetFatfNetId());
                     outFile.WriteString(",\"%s\"", fatfNet->GetCamcadNet()->getNetName());
                     outFile.WriteString(",%s", fatfNet->GetFatfNetType(m_doc));
                     outFile.WriteString(",\n   %s", "(" );
                  }

                  outFile.WriteString("%s(%d,%d)", 
                     (count > 0) ? ((count % 6) == 0 ? ",\n   " : ",") : "",
                     fatfPart->GetFatfPartId(), fatfPin != NULL ? fatfPin->GetFatfPinId() : 0);
                  count++;
               }
            }
         }

         if (count > 0)
         {
            // there was output, so terminate record
            outFile.WriteString("%s", ")");
            outFile.WriteString(";\n");
         }
      }
   }

   outFile.WriteString(":EOD\n\n");
}

void CFabmasterFATFWriter::WriteLayerNames(CFormatStdioFile &outFile)
{
   outFile.WriteString(":LAYER_NAMES\n");

   CString *ignoredKey;
   CFatfLayer *fatfLayer = NULL;

   for (this->m_fatfLayerMap.GetFirstSorted(ignoredKey, fatfLayer); fatfLayer != NULL; this->m_fatfLayerMap.GetNextSorted(ignoredKey, fatfLayer))
   {
      outFile.WriteString("%d", fatfLayer->GetFatfLayerId());
      outFile.WriteString(",\"%s\"", fatfLayer->GetCamcadLayerData()->getName());
      outFile.WriteString(",%s", fatfLayer->GetFatfLayerSense( this->m_fatfLayerMap.GetMaxElectricalStackNum() ));
      outFile.WriteString(",%d", this->m_fatfLayerMap.GetFatfMirrorLayerId(fatfLayer));
      outFile.WriteString(",%s", fatfLayer->GetFatfLayerType());
      outFile.WriteString(";\n");
   }


   outFile.WriteString(":EOD\n\n");
}

bool CFabmasterFATFWriter::GetFatfPackageAndPin(CFatfPart *fatfPart, CString camcadPinRefname, CFatfPackage *&fatfPkg, CFatfPin *&fatfPin)
{
   if (fatfPart != NULL)
      if (m_fatfPackageMap.Lookup(fatfPart->GetInsertedBlockNumberAsString(), fatfPkg))
         if (fatfPkg->GetPinMap().Lookup(camcadPinRefname, fatfPin))
            return true;

   return false;
}

double CFabmasterFATFWriter::ArcAngleRadians(double radius)
{
   // Determine angle step for vectoring out arcs

   double angle = MIN_ARC_ANGLE_DEGREES;
   double radiusMils = m_doc->convertPageUnitsTo(pageUnitsMils, radius);

   if (radiusMils > 750.0)
      angle = 2.5;
   if (radiusMils > 1000.0)
      angle = 1.0;

   return DegToRad(angle);
}

void CFabmasterFATFWriter::WriteGraphicBlock(CFormatStdioFile &outFile, CFatfPackage *fatfPkg, BlockStruct *block,
                                             double insertX, double insertY, double insertRotation, 
                                             int insertMirror, double insertScale, int insertLayer, int fatfPinId, int embeddedLevel)
{
   if (block == NULL)
      return;

   CDataList *DataList = &block->getDataList();
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         fatfLayerId = 0;
   int         layer;

   RotMat2(&m, insertRotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      //if (ignore_non_manufacturing_info(np))
      // continue;

      if (np->getDataType() != T_INSERT)
      {
         // write if either GR_CLASS_BOARDOUTLINE
         // or on LAYTYPE_BOARD_OUTLINE
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE )  
            continue;

         if (m_doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         //if (!m_doc->get_layer_visible(layer, mirror))
         //   continue;

         LayerStruct *l = m_doc->FindLayer(layer);

         if (l != NULL)
         {
            if (l->getLayerType() == LAYTYPE_BOARD_OUTLINE)        
               continue;

            //lindex = layernamesarray[get_layernameindex(l->getName())]->index;
            CFatfLayer *fatfLayer = NULL;
            this->m_fatfLayerMap.Lookup(l->getName(), fatfLayer);
            if (fatfLayer != NULL)
              fatfLayerId = fatfLayer->GetFatfLayerId();
         }
      }

      switch(np->getDataType())
      {
         case dataTypePoly:
         {
            outFile.WriteString("  %d,LAYER (%d(", fatfPinId, fatfLayerId);
            WritePolylist(outFile, np->getPolyList(), false, insertX, insertY, insertRotation, insertMirror, insertScale);
            outFile.WriteString("))\n");
         }
         break;

         case dataTypeText:
         {

            if (!m_doc->get_layer_visible(np->getLayerIndex(), insertMirror))
               continue;

            point2.x = (np->getText()->getPnt().x) * insertScale;
            if (insertMirror) point2.x = -point2.x;
            point2.y = (np->getText()->getPnt().y) * insertScale;
            TransPoint2(&point2, 1, &m,insertX,insertY);

            double text_rot = insertRotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            CFatfFont *fatfFont = m_fatfFontMap.GetFatfFont(
               ScaleToFatf(np->getText()->getHeight()), 
               ScaleToFatf(np->getText()->getWidth()), 
               ScaleToFatf( m_doc->getWidth(np->getText()->getPenWidthIndex() )));

            char  mirror = ' ';
            if (text_mirror)  mirror = 'M';
            int rot = normalizeDegrees( round(360.0 - RadToDeg(text_rot)) ); // fatf positive rotation is clockwise

            outFile.WriteString("  %d,LAYER (%d,", 0, fatfLayerId);
            outFile.WriteString("(TEXT (%d,%ld,%ld,BOTTOM_LEFT,%d,%c,\"%s\")))\n", fatfFont->GetFatfFontId(), 
                  ScaleToFatf(point2.x), ScaleToFatf(point2.y), rot, mirror, np->getText()->getText());

         }
         break;
         
         case dataTypeInsert:
         {
            point2.x = np->getInsert()->getOriginX() * insertScale;
            if (insertMirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * insertScale;
            TransPoint2(&point2, 1, &m, insertX,insertY);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = insertRotation + np->getInsert()->getAngle();

            BlockStruct *insertedblock = m_doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

           // if (block->getBlockType() == BLOCKTYPE_PADSTACK)  break;

            int block_layer;
            if (!m_doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
               block_layer = np->getLayerIndex();
            else 
               block_layer = insertLayer;

            int blockFatfPinId = fatfPinId;
            if (np->isInsertType(insertTypePin))
            {
               CString pinRefname = np->getInsert()->getRefname();
               CFatfPin *fatfPin = NULL;
               if (fatfPkg->GetPinMap().Lookup(pinRefname, fatfPin))
                  blockFatfPinId = fatfPin->GetFatfPinId();
            }

            // Suggestion: REARRANGE code so aperture block is handled at top as incoming block type, not 
            // usurped down here, then just recurs call to handle the apertures.

            if ( insertedblock->isAperture() )
            {
               CFatfLayer *fatfLayer = NULL;
               LayerStruct *lp = m_doc->getLayer(block_layer);
               if (lp != NULL)
                  m_fatfLayerMap.Lookup(lp->getName(), fatfLayer);
               if (fatfLayer != NULL)
                  fatfLayerId = fatfLayer->GetFatfLayerId();

               if (insertedblock->isComplexAperture())
               {
                  {
                     BlockStruct *complexApBlk = m_doc->getBlockAt(insertedblock->getComplexApertureSubBlockNumber());
                     if (complexApBlk != NULL)
                     {
                        WriteGraphicBlock(outFile, fatfPkg, complexApBlk,
                           point2.x, point2.y, block_rot, 
                           block_mirror, insertScale * np->getInsert()->getScale(),
                           block_layer, blockFatfPinId, embeddedLevel+1);
                     }
                  }
               }
               else
               {
                  // Simple apertures

                  double rotDeg = normalizeDegrees(RadToDeg(block_rot) + insertedblock->getRotationDegrees());
                  bool rot90 = fpeq(rotDeg, 90.0, 1.0) || fpeq(rotDeg, 270.0, 1.0);

                  outFile.WriteString("  %d,LAYER (%d,", blockFatfPinId, fatfLayerId); //fatfPinId, fatfLayerId);
                  switch (insertedblock->getShape())
                  {
                  case apertureRound:
                     outFile.WriteString("(ROUND(%d,%d,%d))", ScaleToFatf(insertedblock->getSizeA()), ScaleToFatf(point2.x), ScaleToFatf(point2.y));
                     break;
                  case apertureSquare:
                     {
                        double halfsize = insertedblock->getSizeA() / 2.0;
                        outFile.WriteString("(BLOCK(%d,%d,%d,%d))", 
                           ScaleToFatf(point2.x - halfsize), ScaleToFatf(point2.y - halfsize),
                           ScaleToFatf(point2.x + halfsize), ScaleToFatf(point2.y + halfsize));
                     }
                     break;
                  case apertureRectangle:
                     {
                        double halfsizeX = insertedblock->getSizeA() / 2.0;
                        double halfsizeY = insertedblock->getSizeB() / 2.0;
                        if (rot90)
                        {
                           halfsizeX = insertedblock->getSizeB() / 2.0;
                           halfsizeY = insertedblock->getSizeA() / 2.0;
                        }
                        outFile.WriteString("(BLOCK(%d,%d,%d,%d))", 
                           ScaleToFatf(point2.x - halfsizeX), ScaleToFatf(point2.y - halfsizeY),
                           ScaleToFatf(point2.x + halfsizeX), ScaleToFatf(point2.y + halfsizeY));
                     }
                     break;
                  case apertureOblong:
                     {
                        double sizeA = insertedblock->getSizeA();
                        double sizeB = insertedblock->getSizeB();
                        double len = ( (sizeA - sizeB) );/// 2.0 );
                        double rot = insertedblock->getRotationDegrees();
                        double x1 = point2.x - (len/2.0);
                        double y1 = point2.y;
                        double x2 = point2.x + (len/2.0);
                        double y2 = point2.y;
                        // BUG need to do trig to handle any angle, punt for now
                        if (fpeq(rot, 90.0, 1.0) || fpeq(rot, 270.0, 1.0)) // 90 or 279 within one degree
                        {
                           x1 = point2.x;
                           y1 = point2.y - (len/2.0);
                           x2 = point2.x;
                           y2 = point2.y + (len/2.0);
                        }
                        outFile.WriteString("(TRACK( %d (%d,%d)(%d,%d)))",
                           ScaleToFatf(sizeB), // width
                           ScaleToFatf( x1 ), ScaleToFatf( y1 ),
                           ScaleToFatf( x2 ), ScaleToFatf( y2 ));
                     }
                     break;
                  case apertureComplex: // complex apertures should't get here
                  case apertureTarget:
                  case apertureThermal:  
                  case apertureDonut:
                  case apertureOctagon:
                  case apertureBlank:
                  case apertureUndefined:
                  case apertureUnknown:
                  default:
                     outFile.WriteString("(ROUND(%d,%d,%d))", 
                        ScaleToFatf(m_doc->convertToPageUnits(pageUnitsMils, 15.0)), 
                        ScaleToFatf(point2.x), ScaleToFatf(point2.y));
                     break;
                  }
                  outFile.WriteString(")\n");
               }
            }
            else
            {
               WriteGraphicBlock(outFile, fatfPkg, insertedblock,
                     point2.x, point2.y, block_rot, 
                     block_mirror, insertScale * np->getInsert()->getScale(),
                     block_layer, blockFatfPinId, embeddedLevel+1);
            }
         }
         break;

      }
   }

}

void CFabmasterFATFWriter::WritePolylist(CFormatStdioFile &outFile, CPolyList *polyList, bool isNetTrace,
      double insertX, double insertY, double insertRotation, 
      int insertMirror, double insertScale)
{
	// Output the list of polylines

   bool first = true;
   CString newlineIndent = "\n  ";
   int ptsPerLine = 5;

   Mat2x2      m;
   RotMat2(&m, insertRotation);

	POSITION pos = polyList->GetHeadPosition();
	while(pos)
	{
		CPoly *poly = polyList->GetNext(pos);
		double width = 0.0;
      int ptsOutCnt = 0; // number of points output, for line formatting

      // From borrowed code, but I don't see why isNetTrace matters for width, so
      // just get width always for now at least, til we see why it should matter
		/////if (isNetTrace)
		{
			// Get width for net trace
			if (poly->getWidthIndex() > -1)
				width = m_doc->getWidth(poly->getWidthIndex()) * insertScale;
		}

      if (poly->isClosed())
         outFile.WriteString("%sPOLYGON(", !first ? newlineIndent : "");
      else
         outFile.WriteString("%sTRACK(%d", !first ? newlineIndent : "", ScaleToFatf(width));
      first = false;
      

		double cX = 0.0;
		double cY = 0.0;
		double radius = 0.0;

		if (PolyIsCircle(poly, &cX, &cY, &radius))
		{
         Point2 cntrPnt;
			cntrPnt.x = (DbUnit)(cX * insertScale);
			cntrPnt.y = (DbUnit)(cY * insertScale);
         radius *= insertScale;

			if (insertMirror)
				cntrPnt.x = -cntrPnt.x;

         TransPoint2(&cntrPnt, 1, &m, insertX, insertY);

			int cnt = 255;
			Point2 *arcpolyline = (Point2 *)calloc(255,sizeof(Point2));

         if (arcpolyline == NULL)
         {
            ErrorMessage("Memory allocation failed for creation of arc-polyline.", "Out Of Memory");
         }
         else
         {
			   arc2poly(DegToRad(0.0), DegToRad(360.0), ArcAngleRadians(radius), cntrPnt.x, cntrPnt.y, radius, arcpolyline, &cnt);

			   if (cnt > 0)
			   {
				   for (int i = 0; i < cnt; i++)
				   {
					   outFile.WriteString("%s(%d,%d)", 
                     (++ptsOutCnt % ptsPerLine == 0 ? newlineIndent : ""),
						   ScaleToFatf(arcpolyline[i].x), ScaleToFatf(arcpolyline[i].y));
 				   }
			   }
            free(arcpolyline);
         }
		}
		else
		{
			bool firstPnt = true;
			CPntList pntList = poly->getPntList();

			POSITION pntPos = pntList.GetHeadPosition();
			while(pntPos)
			{
				CPnt *pnt = pntList.GetNext(pntPos);

				Point2 curPnt;
				curPnt.x = (DbUnit)(pnt->x * insertScale);
				curPnt.y = (DbUnit)(pnt->y * insertScale);
				curPnt.bulge = pnt->bulge;

				if (insertMirror)
				{
					curPnt.x = -curPnt.x;
					curPnt.bulge = -curPnt.bulge;  // Swaps CW and CCW arc direction.
				}

            TransPoint2(&curPnt, 1, &m, insertX, insertY);

            outFile.WriteString("%s(%d,%d)", 
               (++ptsOutCnt % ptsPerLine == 0 ? newlineIndent : ""), 
               ScaleToFatf(curPnt.x), ScaleToFatf(curPnt.y));

				firstPnt = false;

				while (!fpeq(curPnt.bulge, 0.0) && pntPos != NULL)
				{
					CPnt *tmpPnt = pntList.GetNext(pntPos);

					Point2 nextPnt;
					nextPnt.x = (DbUnit)(tmpPnt->x * insertScale);
					nextPnt.y = (DbUnit)(tmpPnt->y * insertScale);
               nextPnt.bulge = tmpPnt->bulge;

					if (insertMirror)
					{
						nextPnt.x = -nextPnt.x;
                  nextPnt.bulge = -nextPnt.bulge; // Swaps CW and CCW arc direction.
					}

               TransPoint2(&nextPnt, 1, &m, insertX, insertY);

					double startAngle, radius, xCenter, yCenter;
					double deltaAngle = atan(curPnt.bulge) * 4;

					ArcPoint2Angle(curPnt.x, curPnt.y, nextPnt.x, nextPnt.y, deltaAngle, &xCenter, &yCenter, &radius, &startAngle);

               int cnt = 255;
               Point2 *arcpolyline = (Point2 *)calloc(255,sizeof(Point2));
         
               if (arcpolyline == NULL)
               {
                  ErrorMessage("Memory allocation failed for creation of arc-polyline.", "Out Of Memory");
               }
               else
               {
                  arc2poly(startAngle, deltaAngle, ArcAngleRadians(radius), xCenter, yCenter, radius, arcpolyline, &cnt);

   				   if (cnt > 0)
					   {
						   for (int i = 0; i < cnt; i++)
						   {
							   outFile.WriteString("%s(%d,%d)", 
                           (++ptsOutCnt % ptsPerLine == 0 ? newlineIndent : ""),
                           ScaleToFatf(arcpolyline[i].x), ScaleToFatf(arcpolyline[i].y));
						   }
					   }
                  free(arcpolyline);
               }

               // handle back-to-back arcs
               curPnt = nextPnt;
				}
			}
		}
      outFile.WriteString(")");
	}
}

void CFabmasterFATFWriter::WriteNetTraces(CFormatStdioFile &outFile)
{
   BlockStruct *fileBlock = m_pcbFile->getBlock();

	if (fileBlock == NULL)
		return;

   WORD netnameKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeNetName);

   outFile.WriteString(":LAYERS\n");
	
   CString *ignoredKey;
   CFatfNet *fatfNet = NULL;

   for (this->m_fatfNetMap.GetFirstSorted(ignoredKey, fatfNet); fatfNet != NULL; this->m_fatfNetMap.GetNextSorted(ignoredKey, fatfNet)) 
	{
      int fatfNetId = fatfNet->GetFatfNetId();
		NetStruct *net = fatfNet->GetCamcadNet();

		if (net != NULL)
		{
			POSITION blockPos = fileBlock->getDataList().GetHeadPosition();
			while (blockPos)
			{
				DataStruct *data = fileBlock->getDataList().GetNext(blockPos);

				if (data != NULL && data->getAttributes() != NULL)
				{
					Attrib *attrib = NULL;
					if (data->getAttributes()->Lookup(netnameKw, attrib))
					{
               #if CamCadMajorMinorVersion > 406  //  > 4.6
		            CString netName = attrib->getStringValue();
               #else
		            CString netName = m_doc->ValueArray[attrib->getStringValueIndex()];
               #endif

                  if (netName.CompareNoCase(net->getNetName()) == 0)
						{
							if (data->getDataType() == dataTypePoly && data->getGraphicClass() == graphicClassEtch)
							{
								LayerStruct *layer = m_doc->FindLayer(data->getLayerIndex());
                        CFatfLayer *fatfLayer = NULL;
                        this->m_fatfLayerMap.Lookup(layer->getName(), fatfLayer);
                        int fatfLayerId = fatfLayer != NULL ? fatfLayer->GetFatfLayerId() : 0;

                        outFile.WriteString("%d,LAYER(%d(", fatfNetId, fatfLayerId);

                        WritePolylist(outFile, data->getPolyList(), true, 0.0, 0.0, 0.0, 0/*mirror*/, 1.0);
								
                        outFile.WriteString("));\n");
							}
						}
					}
				}
			}
		}
	}

	outFile.WriteString(":EOD\n\n");
}

