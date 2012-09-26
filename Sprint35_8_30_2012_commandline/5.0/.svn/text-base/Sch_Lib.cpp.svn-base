// $Header: /CAMCAD/4.6/Sch_Lib.cpp 53    4/12/07 3:34p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "Sch_Lib.h"
#include "graph.h"
#include "pcbutil.h"
#include "net_util.h"


// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* Function Prototypes *********************************************************/

static void PrepareMaps(CCEtoODBDoc *docPtr, FileStruct *filePtr, CMapStringToString *refNameMap, CMapStringToString *designatorMap,
                  CMapStringToString *netnameMap, CMapStringToString *comppinToNetnameMap, CMapStringToString *comppinToRemoveMap);
static void PreparNetToNetMap(CCEtoODBDoc *docPtr, BlockStruct *symbolBlock, CString symbolRefname, CString hierarchySheetName,
                  CMapStringToString *netToNetMap, CMapStringToString *comppinToNetnameMap);
static void PreparOffAndOnPageNetMap(CCEtoODBDoc *docPtr, BlockStruct *hierarchySheet, CMapStringToString *offAndOnPageNetMap);
static void CloneHierarchySheet(CCEtoODBDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, CMapStringToString *refNameMap, 
                  CMapStringToString *designatorMap, CMapStringToString *netnameMap, CMapStringToString *netToNetMap,
                  CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap, BlockStruct *hierarchySheet, BlockStruct **clonedSheet);
static void PropagateNetList(CCEtoODBDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, BlockStruct *hierarchySheet, CMapStringToString *netnameMap,
									  CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap);
static void FixNetList(CCEtoODBDoc *docPtr, FileStruct *filePtr, DataStruct *data, CString prevRefName,  CMapStringToString *netnameMap,
								CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap);

/******************************************************************************
* CCEtoODBDoc::OnSchematicChooserefdes
*/
void CCEtoODBDoc::OnSchematicChooserefdes()
{
	if (this->getFileList().GetCount() == 0)
	{
      MessageBox(NULL, "No file is loaded, cannot run Schematic->Select Attribute", "Select Attribute", MB_OK | MB_ICONHAND);
		return;
	}

   CSchSelAttributes dlg;
   dlg.filePtr = this->getFileList().GetAt(this->getFileList().GetHeadPosition());
   dlg.docPtr = this;

   if (dlg.DoModal() != IDCANCEL)
   {
      RenameCompAndPinDesignator(this, dlg.filePtr->getFileNumber(), dlg.newRefDesAttrib, dlg.newPinnoAttrib,
            dlg.newRefDesAttrib, dlg.newPinnoAttrib);
   }
}

//----------------------------------------------------------------------------------------

static CString GetAttribStringValue(CCEtoODBDoc *doc, Attrib *attrib)
{
   CString attrval;

   if (attrib != NULL)
   {
      attrval = attrib->getStringValue();
   }

   return attrval;
}

//----------------------------------------------------------------

static CString GetAttribStringValue(CCEtoODBDoc *doc, CAttributes *attributes, WORD key)
{
   CString attrval;

   Attrib *attrib;
   if (attributes != NULL && attributes->Lookup(key, attrib))
   {
      attrval = GetAttribStringValue(doc, attrib);
   }

   return attrval;
}

//----------------------------------------------------------------------------------------

static void CopyAttribVisibilitySettings(Attrib *destination, Attrib *source)
{
   if (destination != NULL && source != NULL)
   {
      destination->setCoordinate(          source->getCoordinate());
      destination->setRotationRadians(     source->getRotationRadians());
      destination->setHeight(              source->getHeight());
      destination->setWidth(               source->getWidth());
      destination->setProportionalSpacing( source->isProportionallySpaced());
      destination->setPenWidthIndex(       source->getPenWidthIndex());
      destination->setMirrorDisabled(      source->getMirrorDisabled());
      destination->setVisible(             source->isVisible());
      destination->setFlags(               source->getFlags());
      destination->setLayerIndex(          source->getLayerIndex());
      //destination->setInherited(         designatorAttrib->isInherited());   // ORIGINAL CODE DID NOT COPY THIS ONE, BUG?
      destination->setHorizontalPosition(  source->getHorizontalPosition());
      destination->setVerticalPosition(    source->getVerticalPosition());
   }
}

//----------------------------------------------------------------------------------------

/******************************************************************************
* RenameCompAndPinDesignator
*/
void RenameCompAndPinDesignator(CCEtoODBDoc *doc, int fileNum, CString refdesAttrib, CString pinnoAttrib,
                                CString refdesLocAttrib, CString pinnoLocAttrib)
{
   refdesLocAttrib.Trim();
   pinnoLocAttrib.Trim();

   refdesAttrib.Trim();
   pinnoAttrib.Trim();
   if (refdesAttrib == "" && pinnoAttrib == "")
      return;


   // - Add the attribute SCH_ATT_SECONDDESIGNATOR to the sheet block so in 
   // Schematic List it will know to look for the designator in that attribute
   // - Add the select attribute of the pin number to the sheet block so in 
   // Schematic List it will know to look for the pin number in that attribute
   int refdesMapKey = doc->RegisterKeyWord(SCH_ATT_REFDES_MAP, 0, valueTypeString);
   int pinnoMapKey = doc->RegisterKeyWord(SCH_ATT_PINNO_MAP, 0, valueTypeString);

   //for (int i=0; i<doc->getMaxBlockIndex(); i++)
   //{
   //   BlockStruct *block = doc->getBlockAt(i);

   //   if (block == NULL)
   //      continue;
   //   if (block->getFileNumber() != fileNum)
   //      continue;

   //   if (block->getBlockType() == BLOCKTYPE_SHEET)
   //   {
   //      // To indicate that this is schematic file, not a regular PCB file
   //      if (refdesAttrib != "")
   //         doc->SetAttrib(&block->getAttributesRef(), refdesMapKey, valueTypeString, SCH_ATT_SECONDDESIGNATOR, attributeUpdateOverwrite, NULL);

   //      if (pinnoAttrib != "")
   //         doc->SetAttrib(&block->getAttributesRef(), pinnoMapKey, valueTypeString, pinnoAttrib.GetBuffer(0), attributeUpdateOverwrite, NULL);
   //   }
   //}


   // Here are do the followings:
   // - Add the attribute SCH_ATT_SECONDDESIGNATOR to every component
   // - Re-assign the value of compDesignator and pinDesignator in netlist basic on the value
   // of the selected "refdesAttrib" and "pinnoAttrib"

   int compDesKey    = doc->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR,   0, valueTypeString);
   int pinDesKey     = doc->RegisterKeyWord(SCH_ATT_PINDESIGNATOR,    0, valueTypeString);
   int designatorKey = doc->RegisterKeyWord(SCH_ATT_DESIGNATOR,       0, valueTypeString);
   int secondDesKey  = doc->RegisterKeyWord(SCH_ATT_SECONDDESIGNATOR, 0, valueTypeString);

   WORD libraryNameKey = doc->RegisterKeyWord(SCH_ATT_LIBRARYNAME, 0, valueTypeString);
   WORD cellNameKey    = doc->RegisterKeyWord(SCH_ATT_CELLNAME,    0, valueTypeString);
   WORD clusterNameKey = doc->RegisterKeyWord(SCH_ATT_CLUSTERNAME, 0, valueTypeString);

   WORD parentLibraryNameKey = doc->RegisterKeyWord(SCH_ATT_PARENTLIBRARY, 0, valueTypeString);
   WORD parentCellNameKey    = doc->RegisterKeyWord(SCH_ATT_PARENTCELL,    0, valueTypeString);
   WORD parentClusterNameKey = doc->RegisterKeyWord(SCH_ATT_PARENTCLUSTER, 0, valueTypeString);

   WORD refdesKey    = doc->IsKeyWord(refdesAttrib,    0);
   WORD pinnoKey     = doc->IsKeyWord(pinnoAttrib,     0);
   WORD refdesLocKey = doc->IsKeyWord(refdesLocAttrib, 0);
   WORD pinnoLocKey  = doc->IsKeyWord(pinnoLocAttrib,  0);

   Attrib* attrib;

   /////CMapStringToString designatorMap;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      if (file->getNetList().GetCount() <= 0)
         continue;

      //(1) Find the shematic sheet
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *sheetBlock = doc->getBlockAt(i);
         if (sheetBlock == NULL)
            continue;

         if (sheetBlock->getBlockType() != blockTypeSheet || sheetBlock->getFileNumber() != file->getFileNumber())
            continue;

         //(2) Find the instance insert
         POSITION dataPos = sheetBlock->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = sheetBlock->getDataList().GetNext(dataPos);

            if (data->isInsertType(insertTypeSymbol) && data->getAttributesRef() != NULL)
            {
               //CString dataRefname(data->getInsert()->getRefname());

               CString instanceOwnerLibraryName( GetAttribStringValue(doc, data->getAttributesRef(), parentLibraryNameKey) );
               CString instanceOwnerCellName(    GetAttribStringValue(doc, data->getAttributesRef(), parentCellNameKey) );
               CString instanceOwnerClusterName( GetAttribStringValue(doc, data->getAttributesRef(), parentClusterNameKey) );

               //(3) Get the designator of the instance
               CString newCompDes;
               if (refdesAttrib != "")
               {
                  // If there is a new selected designator attribute provided then look for it
                  if (!data->getAttributesRef()->Lookup(refdesKey, attrib))
                     continue;

                  if (attrib)
                     newCompDes = GetAttribStringValue(doc, attrib);

                  Attrib* designatorAttrib = NULL;
                  if (data->lookUpAttrib(designatorKey, designatorAttrib))
                  {
                     // Get the original DESIGNATOR and copy it to DESIGNATOR_ORIGINAL
                     Attrib* originalDesignatorAttrib = NULL;
                     int originalDesignatorKey = doc->RegisterKeyWord("DESIGNATOR_ORIGINAL", 0, valueTypeString);
                     CString originalDesignatorValue( GetAttribStringValue(doc, designatorAttrib) );
                     ///data->setAttrib(doc, originalDesignatorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                     doc->SetAttrib(&data->getDefinedAttributes(), originalDesignatorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                     originalDesignatorAttrib->setProperties(*designatorAttrib);
                     originalDesignatorAttrib->setVisible(false);

                     // Copy the value of the intended designator attribtue to DESIGNATOR
                     designatorAttrib->setValue(*attrib);
                  }
                  else
                  {
                     // Since there is no original DESIGNATOR so set DESIGNATOR attribute
                     ///data->setAttrib(doc, designatorKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                     doc->SetAttrib(&data->getDefinedAttributes(), designatorKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                  }

                  if (!designatorAttrib->isVisible() && attrib->isVisible())
                  {
                     // If the new DESIGNATOR does not have visibility, but the one it copy from has then use it
                     if (refdesLocAttrib.IsEmpty())
                     {
                        refdesLocAttrib = refdesAttrib;
                        refdesLocKey = doc->IsKeyWord(refdesAttrib, 0);
                     }
                  }

                  // Copy the property of the intended location of designator to DESIGNATOR
                  if (refdesLocAttrib != "" && designatorAttrib != NULL)
                  {
                     // (3.3) If there is a new selected designator location attribute provided then look for it
                     if (data->getAttributesRef()->Lookup(refdesLocKey, attrib))
                     {
                        // Now copy the location from this attribute
                        CopyAttribVisibilitySettings(designatorAttrib, attrib);

                        // Turn off the visibility of the given attribute
                        attrib->setVisible(false);
                     }
                  }


                  //// (3.2) Add the new compDes as a 2nd designator
                  //Attrib *newAttrib = NULL;
                  //doc->SetAttrib(&data->getAttributesRef(), secondDesKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, &newAttrib);          

                  //if (refdesLocAttrib != "" && newAttrib)
                  //{
                  //   // (3.3) If there is a new selected designator location attribute provided then look for it
                  //   if (data->getAttributesRef()->Lookup(refdesLocKey, attrib))
                  //   {
                  //      // Now copy the location from this attribute
                  //      newAttrib->setCoordinate(attrib->getCoordinate());
                  //      newAttrib->setRotationRadians( attrib->getRotationRadians());
                  //      newAttrib->setHeight( attrib->getHeight());
                  //      newAttrib->setWidth( attrib->getWidth());
                  //      newAttrib->setProportionalSpacing( attrib->isProportionallySpaced());
                  //      newAttrib->setPenWidthIndex( attrib->getPenWidthIndex());
                  //      newAttrib->setMirrorDisabled( attrib->getMirrorDisabled());
                  //      newAttrib->setVisible( attrib->isVisible());
                  //      newAttrib->setFlags( attrib->getFlags());
                  //      newAttrib->setLayerIndex( attrib->getLayerIndex());
                  //      newAttrib->setInherited( newAttrib->isInherited());
                  //      newAttrib->setHorizontalPosition(attrib->getHorizontalPosition());
                  //      newAttrib->setVerticalPosition(attrib->getVerticalPosition());

                  //      // Turn off the visibility of the given attribute
                  //      attrib->setVisible(false);
                  //   }
                  //}
               }
               BlockStruct *insertedBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());

               //(5) Find the port insert
               POSITION portDataPos = insertedBlock->getDataList().GetHeadPosition();
               while (portDataPos)
               {
                  DataStruct *portData = insertedBlock->getDataList().GetNext(portDataPos);
                  if (portData->isInsertType(insertTypePortInstance) && portData->getAttributesRef() != NULL)
                  {
                     //(6) Get the designator of the port
                     CString pinDes;
                     if (pinnoAttrib != "")
                     {
                        // If there is a new selected pin number attribute provided then look for it
                        if (!portData->getAttributesRef()->Lookup(pinnoKey, attrib))
                           continue;

                        if (attrib)
                           pinDes = GetAttribStringValue(doc, attrib);

                        Attrib* designatorAttrib = NULL;
                        if (portData->lookUpAttrib(designatorKey, designatorAttrib))
                        {
                           // Get the original DESIGNATOR and copy it to DESIGNATOR_ORIGINAL
                           Attrib* originalDesignatorAttrib = NULL;
                           int originalDesignaotorKey = doc->RegisterKeyWord("DESIGNATOR_ORIGINAL", 0, valueTypeString);
                           CString originalDesignatorValue( GetAttribStringValue(doc, designatorAttrib) );
                           ///portData->setAttrib(doc, originalDesignaotorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                           doc->SetAttrib(&portData->getDefinedAttributes(), originalDesignaotorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                           originalDesignatorAttrib->setProperties(*designatorAttrib);
                           originalDesignatorAttrib->setVisible(false);

                           // Copy the value of the intended designator attribtue to DESIGNATOR
                           designatorAttrib->setValue(*attrib);
                        }
                        else
                        {
                           // Since there is no original DESIGNATOR so set DESIGNATOR attribute
                           ///portData->setAttrib(doc, designatorKey, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                           doc->SetAttrib(&portData->getDefinedAttributes(), designatorKey, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                        }

                        if (!designatorAttrib->isVisible() && attrib->isVisible())
                        {
                           // If the new DESIGNATOR does not have visibility, but the one it copy from has then use it
                           if (pinnoLocAttrib.IsEmpty())
                           {
                              pinnoLocAttrib = pinnoAttrib;
                              pinnoLocKey = doc->IsKeyWord(pinnoAttrib, 0);
                           }
                        }


                        // Copy the property of the intended location of designator to DESIGNATOR
                        if (pinnoLocAttrib != "" && designatorAttrib != NULL)
                        {
                           // If there is a new selected designator location attribute provided then look for it
                           if (portData->getAttributesRef()->Lookup(pinnoLocKey, attrib))
                           {
                              // Now copy the location from this attribute
                              CopyAttribVisibilitySettings(designatorAttrib, attrib);

                              // Turn off the visibility of the given attribute
                              attrib->setVisible(false);
                           }
                        }
                     }

                     //(7) Find the net
                     bool foundComppin = false;
                     POSITION netPos = file->getNetList().GetHeadPosition();
                     while (netPos)
                     {
                        NetStruct *net = file->getNetList().GetNext(netPos);

                           CString netname( net->getNetName() );
                           if (netname.CompareNoCase("UNNAMED_1_CAPACITED_46P_A") == 0)
                           {
                              int jj = 0;
                           }
                           CString dataRefname(data->getInsert()->getRefname());
                           if (dataRefname.CompareNoCase("P1_51P") == 0)
                           {
                              int jj = 0;
                           }
                           if (netname.CompareNoCase("UNNAMED_1_CAPACITED_46P_A") == 0 &&
                              dataRefname.CompareNoCase("P1_51P") == 0)
                           {
                              int jj = 0;
                           }




                        CString netLibraryName( GetAttribStringValue(doc, net->getAttributesRef(), libraryNameKey) );
                        CString netCellName(    GetAttribStringValue(doc, net->getAttributesRef(), cellNameKey) );
                        CString netClusterName( GetAttribStringValue(doc, net->getAttributesRef(), clusterNameKey) );

                        //if (/*instanceOwnerLibraryName.CompareNoCase(netLibraryName) == 0 &&*/
                        //   instanceOwnerCellName.CompareNoCase(netCellName) == 0 // &&
                         //  /*instanceOwnerClusterName.CompareNoCase(netClusterName) == 0*/)
                        {
                           //(8) Find the comppin
                           POSITION comppinPos = net->getHeadCompPinPosition();
                           while (comppinPos)
                           {
                              CompPinStruct *comppin = net->getNextCompPin(comppinPos);

                              if ((comppin->getRefDes().CompareNoCase(data->getInsert()->getRefname()) == 0) &&
                                 (comppin->getPinName().CompareNoCase(portData->getInsert()->getRefname()) == 0))
                              {

                                 CString comppinLibraryName( GetAttribStringValue(doc, comppin->getAttributesRef(), parentLibraryNameKey) );
                                 CString comppinCellName(    GetAttribStringValue(doc, comppin->getAttributesRef(), parentCellNameKey) );
                                 CString comppinClusterName( GetAttribStringValue(doc, comppin->getAttributesRef(), parentClusterNameKey) );

                                 if (/*instanceOwnerLibraryName.CompareNoCase(comppinLibraryName) == 0 &&*/
                                    instanceOwnerCellName.CompareNoCase(comppinCellName) == 0 //&&
                                    /*instanceOwnerClusterName.CompareNoCase(comppinClusterName) == 0*/ )
                                 {
                                    foundComppin = true;
                                    //(9) Re-assign the "compDesignator" attributes
                                    if (refdesAttrib != "")
                                       doc->SetAttrib(&comppin->getAttributesRef(), compDesKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
                                    else
                                       int jj = 0;

                                    //(10) Re-assign the "pinDesignator" attribute if there is a new selected pin number attribute provided
                                    if (pinnoAttrib != "")
                                       doc->SetAttrib(&comppin->getAttributesRef(), pinDesKey, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
                                    else
                                       int jj = 0;
                                 }
                              }
                           }
                        }
                     }
                     //if (!foundComppin)
                     //{
                     //   CString compRef(data->getInsert()->getRefname());
                     //   CString pinRef(portData->getInsert()->getRefname());
                     //   int jj = 0;
                     //}

                  }
               }
            }
         }
      }
   }

}

//--------------------------------------------------------------------------------------

/******************************************************************************
* RenameCompAndPinDesignator
*/
void RenameCompAndPinDesignator(CCEtoODBDoc *doc, int fileNum, CStringArray& symbolDesignatorBackAnnotateArray, CString pinnoAttrib,
                                CString refdesLocAttrib, CString pinnoLocAttrib)
{
   refdesLocAttrib.Trim();
   pinnoLocAttrib.Trim();

   ////refdesAttrib.Trim();
   ////pinnoAttrib.Trim();
   ////if (refdesAttrib == "" && pinnoAttrib == "")
   ////   return;
   bool hasRefdesAttrib = false;
   for (int i = 0; i < symbolDesignatorBackAnnotateArray.GetCount() && !hasRefdesAttrib; i++)
   {
      if (!symbolDesignatorBackAnnotateArray.IsEmpty())
         hasRefdesAttrib = true;
   }
   if (!hasRefdesAttrib && pinnoAttrib == "")
      return;


   // - Add the attribute SCH_ATT_SECONDDESIGNATOR to the sheet block so in 
   // Schematic List it will know to look for the designator in that attribute
   // - Add the select attribute of the pin number to the sheet block so in 
   // Schematic List it will know to look for the pin number in that attribute
   int refdesMapKey = doc->RegisterKeyWord(SCH_ATT_REFDES_MAP, 0, valueTypeString);
   int pinnoMapKey = doc->RegisterKeyWord(SCH_ATT_PINNO_MAP, 0, valueTypeString);

   //for (int i=0; i<doc->getMaxBlockIndex(); i++)
   //{
   //   BlockStruct *block = doc->getBlockAt(i);

   //   if (block == NULL)
   //      continue;
   //   if (block->getFileNumber() != fileNum)
   //      continue;

   //   if (block->getBlockType() == BLOCKTYPE_SHEET)
   //   {
   //      // To indicate that this is schematic file, not a regular PCB file
   //      if (refdesAttrib != "")
   //         doc->SetAttrib(&block->getAttributesRef(), refdesMapKey, valueTypeString, SCH_ATT_SECONDDESIGNATOR, attributeUpdateOverwrite, NULL);

   //      if (pinnoAttrib != "")
   //         doc->SetAttrib(&block->getAttributesRef(), pinnoMapKey, valueTypeString, pinnoAttrib.GetBuffer(0), attributeUpdateOverwrite, NULL);
   //   }
   //}


   // Here are do the followings:
   // - Add the attribute SCH_ATT_SECONDDESIGNATOR to every component
   // - Re-assign the value of compDesignator and pinDesignator in netlist basic on the value
   // of the selected "refdesAttrib" and "pinnoAttrib"

   int compDesKey    = doc->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR,   0, valueTypeString);
   int pinDesKey     = doc->RegisterKeyWord(SCH_ATT_PINDESIGNATOR,    0, valueTypeString);
   int designatorKey = doc->RegisterKeyWord(SCH_ATT_DESIGNATOR,       0, valueTypeString);
   int secondDesKey  = doc->RegisterKeyWord(SCH_ATT_SECONDDESIGNATOR, 0, valueTypeString);

   WORD libraryNameKey = doc->RegisterKeyWord(SCH_ATT_LIBRARYNAME, 0, valueTypeString);
   WORD cellNameKey    = doc->RegisterKeyWord(SCH_ATT_CELLNAME,    0, valueTypeString);
   WORD clusterNameKey = doc->RegisterKeyWord(SCH_ATT_CLUSTERNAME, 0, valueTypeString);

   WORD parentLibraryNameKey = doc->RegisterKeyWord(SCH_ATT_PARENTLIBRARY, 0, valueTypeString);
   WORD parentCellNameKey    = doc->RegisterKeyWord(SCH_ATT_PARENTCELL,    0, valueTypeString);
   WORD parentClusterNameKey = doc->RegisterKeyWord(SCH_ATT_PARENTCLUSTER, 0, valueTypeString);

   WORD pinnoKey     = doc->IsKeyWord(pinnoAttrib,     0);
   WORD refdesLocKey = doc->IsKeyWord(refdesLocAttrib, 0);
   WORD pinnoLocKey  = doc->IsKeyWord(pinnoLocAttrib,  0);

   Attrib* attrib;

   /////CMapStringToString designatorMap;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      if (file->getNetList().GetCount() <= 0)
         continue;

      // Map camcad refname to edif refname.
      // This is used to keep from making more than one camcad comppin with same refname.
      CMapStringToString refnameMap; 

      //(1) Find the shematic sheet
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *sheetBlock = doc->getBlockAt(i);
         if (sheetBlock == NULL)
            continue;

         if (sheetBlock->getBlockType() != blockTypeSheet || sheetBlock->getFileNumber() != file->getFileNumber())
            continue;

         //(2) Find the instance insert
         POSITION dataPos = sheetBlock->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = sheetBlock->getDataList().GetNext(dataPos);

            if (data->isInsertType(insertTypeSymbol) && data->getAttributesRef() != NULL)
            {
               CString dataRefname(data->getInsert()->getRefname());

               CString instanceOwnerLibraryName( GetAttribStringValue(doc, data->getAttributesRef(), parentLibraryNameKey) );
               CString instanceOwnerCellName(    GetAttribStringValue(doc, data->getAttributesRef(), parentCellNameKey) );
               CString instanceOwnerClusterName( GetAttribStringValue(doc, data->getAttributesRef(), parentClusterNameKey) );

               CString refdesAttribName;
               WORD refdesKey = 0;
               for (int i = 0; i < symbolDesignatorBackAnnotateArray.GetCount() && refdesAttribName.IsEmpty(); i++)
               {
                  if (!symbolDesignatorBackAnnotateArray.IsEmpty())
                  {
                     refdesKey = doc->RegisterKeyWord(symbolDesignatorBackAnnotateArray.GetAt(i), 0, valueTypeString);
                     if (data->getAttributesRef()->Lookup(refdesKey, attrib))
                     {
                        refdesAttribName = symbolDesignatorBackAnnotateArray.GetAt(i);
                     }
                  }
               }

               //(3) Get the designator of the instance
               CString newCompDes;
               if (refdesAttribName != "")
               {
                  // If there is a new selected designator attribute provided then look for it
                  if (!data->getAttributesRef()->Lookup(refdesKey, attrib))
                     continue;

                  if (attrib)
                     newCompDes = GetAttribStringValue(doc, attrib);

#define MAKE_UNIQUE_REFNAMES
#ifdef  MAKE_UNIQUE_REFNAMES
                  CString mappedEdifRefname;
                  if (refnameMap.Lookup(newCompDes, mappedEdifRefname))
                  {
                     // newCompDes is already used. 
                     // Just use "standard" integer suffixing. Start at "2", the "plain" refname is the implied "1".
                     int suffix = 2;
                     CString candidateRefname;
                     candidateRefname.Format("%s_%d", newCompDes, suffix++);        
                     while (refnameMap.Lookup(candidateRefname, mappedEdifRefname))
                     {
                        candidateRefname.Format("%s_%d", newCompDes, suffix++);
                     }

                     // The candidate should be unique now
                     newCompDes = candidateRefname;

                     // Save this back to where the name came from, it will get used later in crosslinking
                     ///data->setAttrib(doc, refdesKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
                     doc->SetAttrib(&data->getDefinedAttributes(), refdesKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
                     newCompDes.ReleaseBuffer();
                  }
                  refnameMap.SetAt(newCompDes, dataRefname);
#endif



                  Attrib* designatorAttrib = NULL;
                  if (data->lookUpAttrib(designatorKey, designatorAttrib))
                  {
                     // Get the original DESIGNATOR and copy it to DESIGNATOR_ORIGINAL
                     Attrib* originalDesignatorAttrib = NULL;
                     int originalDesignatorKey = doc->RegisterKeyWord("DESIGNATOR_ORIGINAL", 0, valueTypeString);
                     CString originalDesignatorValue( GetAttribStringValue(doc, designatorAttrib) );
                     //data->setAttrib(doc, originalDesignaotorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                     doc->SetAttrib(&data->getDefinedAttributes(), originalDesignatorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                     originalDesignatorAttrib->setProperties(*designatorAttrib);
                     originalDesignatorAttrib->setVisible(false);

                     // Copy the value of the intended designator attribtue to DESIGNATOR
                     designatorAttrib->setValue(*attrib);
                  }
                  else
                  {
                     // Since there is no original DESIGNATOR so set DESIGNATOR attribute
                     //data->setAttrib(doc, designatorKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                     doc->SetAttrib(&data->getDefinedAttributes(), designatorKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                  }

                  if (!designatorAttrib->isVisible() && attrib->isVisible())
                  {
                     // If the new DESIGNATOR does not have visibility, but the one it copy from has then use it
                     if (refdesLocAttrib.IsEmpty())
                     {
                        refdesLocAttrib = refdesAttribName;
                        refdesLocKey = doc->IsKeyWord(refdesAttribName, 0);
                     }
                  }

                  // Copy the property of the intended location of designator to DESIGNATOR
                  if (refdesLocAttrib != "" && designatorAttrib != NULL)
                  {
                     // (3.3) If there is a new selected designator location attribute provided then look for it
                     if (data->getAttributesRef()->Lookup(refdesLocKey, attrib))
                     {
                        // Now copy the location from this attribute
                        CopyAttribVisibilitySettings(designatorAttrib, attrib);

                        // Turn off the visibility of the given attribute
                        attrib->setVisible(false);
                     }
                  }


                  //// (3.2) Add the new compDes as a 2nd designator
                  //Attrib *newAttrib = NULL;
                  //doc->SetAttrib(&data->getAttributesRef(), secondDesKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, &newAttrib);          

                  //if (refdesLocAttrib != "" && newAttrib)
                  //{
                  //   // (3.3) If there is a new selected designator location attribute provided then look for it
                  //   if (data->getAttributesRef()->Lookup(refdesLocKey, attrib))
                  //   {
                  //      // Now copy the location from this attribute
                  //      newAttrib->setCoordinate(attrib->getCoordinate());
                  //      newAttrib->setRotationRadians( attrib->getRotationRadians());
                  //      newAttrib->setHeight( attrib->getHeight());
                  //      newAttrib->setWidth( attrib->getWidth());
                  //      newAttrib->setProportionalSpacing( attrib->isProportionallySpaced());
                  //      newAttrib->setPenWidthIndex( attrib->getPenWidthIndex());
                  //      newAttrib->setMirrorDisabled( attrib->getMirrorDisabled());
                  //      newAttrib->setVisible( attrib->isVisible());
                  //      newAttrib->setFlags( attrib->getFlags());
                  //      newAttrib->setLayerIndex( attrib->getLayerIndex());
                  //      newAttrib->setInherited( newAttrib->isInherited());
                  //      newAttrib->setHorizontalPosition(attrib->getHorizontalPosition());
                  //      newAttrib->setVerticalPosition(attrib->getVerticalPosition());

                  //      // Turn off the visibility of the given attribute
                  //      attrib->setVisible(false);
                  //   }
                  //}
               }
               BlockStruct *insertedBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());

               //(5) Find the port insert
               POSITION portDataPos = insertedBlock->getDataList().GetHeadPosition();
               while (portDataPos)
               {
                  DataStruct *portData = insertedBlock->getDataList().GetNext(portDataPos);
                  if (portData->isInsertType(insertTypePortInstance) && portData->getAttributesRef() != NULL)
                  {
                     //(6) Get the designator of the port
                     CString pinDes;
                     if (pinnoAttrib != "")
                     {
                        // If there is a new selected pin number attribute provided then look for it
                        if (!portData->getAttributesRef()->Lookup(pinnoKey, attrib))
                           continue;

                        if (attrib)
                           pinDes = GetAttribStringValue(doc, attrib);

                        Attrib* designatorAttrib = NULL;
                        if (portData->lookUpAttrib(designatorKey, designatorAttrib))
                        {
                           // Get the original DESIGNATOR and copy it to DESIGNATOR_ORIGINAL
                           Attrib* originalDesignatorAttrib = NULL;
                           int originalDesignaotorKey = doc->RegisterKeyWord("DESIGNATOR_ORIGINAL", 0, valueTypeString);
                           CString originalDesignatorValue( GetAttribStringValue(doc, designatorAttrib) );
                           ///portData->setAttrib(doc, originalDesignaotorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                           doc->SetAttrib(&portData->getDefinedAttributes(), originalDesignaotorKey, valueTypeString, originalDesignatorValue.GetBuffer(0), attributeUpdateOverwrite, &originalDesignatorAttrib);
                           originalDesignatorAttrib->setProperties(*designatorAttrib);
                           originalDesignatorAttrib->setVisible(false);

                           // Copy the value of the intended designator attribtue to DESIGNATOR
                           designatorAttrib->setValue(*attrib);
                        }
                        else
                        {
                           // Since there is no original DESIGNATOR so set DESIGNATOR attribute
                           ///portData->setAttrib(doc, designatorKey, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                           doc->SetAttrib(&portData->getDefinedAttributes(), designatorKey, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, &designatorAttrib);
                        }

                        if (!designatorAttrib->isVisible() && attrib->isVisible())
                        {
                           // If the new DESIGNATOR does not have visibility, but the one it copy from has then use it
                           if (pinnoLocAttrib.IsEmpty())
                           {
                              pinnoLocAttrib = pinnoAttrib;
                              pinnoLocKey = doc->IsKeyWord(pinnoAttrib, 0);
                           }
                        }


                        // Copy the property of the intended location of designator to DESIGNATOR
                        if (pinnoLocAttrib != "" && designatorAttrib != NULL)
                        {
                           // If there is a new selected designator location attribute provided then look for it
                           if (portData->getAttributesRef()->Lookup(pinnoLocKey, attrib))
                           {
                              // Now copy the location from this attribute
                              CopyAttribVisibilitySettings(designatorAttrib, attrib);

                              // Turn off the visibility of the given attribute
                              attrib->setVisible(false);
                           }
                        }
                     }

                     //(7) Find the net
                     bool foundComppin = false;
                     POSITION netPos = file->getNetList().GetHeadPosition();
                     while (netPos)
                     {
                        NetStruct *net = file->getNetList().GetNext(netPos);

                        CString netLibraryName( GetAttribStringValue(doc, net->getAttributesRef(), libraryNameKey) );
                        CString netCellName(    GetAttribStringValue(doc, net->getAttributesRef(), cellNameKey) );
                        CString netClusterName( GetAttribStringValue(doc, net->getAttributesRef(), clusterNameKey) );

                        //if (/*instanceOwnerLibraryName.CompareNoCase(netLibraryName) == 0 &&*/
                        //   instanceOwnerCellName.CompareNoCase(netCellName) == 0 // &&
                         //  /*instanceOwnerClusterName.CompareNoCase(netClusterName) == 0*/)
                        {
                           //(8) Find the comppin
                           POSITION comppinPos = net->getHeadCompPinPosition();
                           while (comppinPos)
                           {
                              CompPinStruct *comppin = net->getNextCompPin(comppinPos);

                              if ((comppin->getRefDes().CompareNoCase(data->getInsert()->getRefname()) == 0) &&
                                 (comppin->getPinName().CompareNoCase(portData->getInsert()->getRefname()) == 0))
                              {

                                 CString comppinLibraryName( GetAttribStringValue(doc, comppin->getAttributesRef(), parentLibraryNameKey) );
                                 CString comppinCellName(    GetAttribStringValue(doc, comppin->getAttributesRef(), parentCellNameKey) );
                                 CString comppinClusterName( GetAttribStringValue(doc, comppin->getAttributesRef(), parentClusterNameKey) );

                                 if (/*instanceOwnerLibraryName.CompareNoCase(comppinLibraryName) == 0 &&*/
                                    instanceOwnerCellName.CompareNoCase(comppinCellName) == 0 //&&
                                    /*instanceOwnerClusterName.CompareNoCase(comppinClusterName) == 0*/ )
                                 {
                                    foundComppin = true;
                                    //(9) Re-assign the "compDesignator" attributes
                                    if (refdesAttribName != "")
                                       doc->SetAttrib(&comppin->getAttributesRef(), compDesKey, valueTypeString, newCompDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
                                    else
                                       int jj = 0;

                                    //(10) Re-assign the "pinDesignator" attribute if there is a new selected pin number attribute provided
                                    if (pinnoAttrib != "")
                                       doc->SetAttrib(&comppin->getAttributesRef(), pinDesKey, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
                                    else
                                       int jj = 0;
                                 }
                              }
                           }
                        }
                     }
                     //if (!foundComppin)
                     //{
                     //   CString compRef(data->getInsert()->getRefname());
                     //   CString pinRef(portData->getInsert()->getRefname());
                     //   int jj = 0;
                     //}

                  }
               }
            }
         }
      }
   }

}

//--------------------------------------------------------------------------------------

int CheckMissingOrDuplicateDesignator(CCEtoODBDoc *docPtr, FileStruct *filePtr, FILE *errorFile, long &errorCount)
{
   WORD designatorKeywordIndex = docPtr->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
	CMapStringToString designatorMap;
	designatorMap.RemoveAll();
	int designatorErr = SCH_ERR_DESIGNATOR_OK;

   for (int i=0; i<docPtr->getMaxBlockIndex(); i++)
   {
      BlockStruct *sheet = docPtr->getBlockAt(i);

      if (sheet == NULL)
         continue;
      if (sheet->getFileNumber() != filePtr->getFileNumber())
         continue;

      if (sheet->getBlockType() == BLOCKTYPE_SHEET)
      {
         POSITION posSheet = sheet->getDataList().GetHeadPosition();
         while (posSheet)
         {
            DataStruct *data = sheet->getDataList().GetNext(posSheet);

				// Make sure we have a hierarchy symbol insert
            if (data->getDataType() != dataTypeInsert || data->getInsert()->getInsertType() != insertTypeSymbol)
               continue;

            Attrib* attrib = NULL;
            if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(designatorKeywordIndex, attrib) || attrib == NULL)
               continue;

				CString tmpString;
				CString designator( GetAttribStringValue(docPtr, attrib) );
				if (designator.IsEmpty())
				{
					// Designator is blank
					fprintf(errorFile, "Note : Instance [%s] has empty designator\n", data->getInsert()->getRefname());
					errorCount++;
					designatorErr = SCH_ERR_DESIGNATOR_BLANK;
				}
				else if (designatorMap.Lookup(designator, tmpString))
				{
					// Duplicate designator
					fprintf(errorFile, "Note : Instance [%s] has duplicate designator [%s]\n", data->getInsert()->getRefname(), designator);
					errorCount++;
					designatorErr = SCH_ERR_DESIGNATOR_DUPLICATE;
				}
				else
				{
					// Add to map
					designatorMap.SetAt(designator, designator);
				}
			}
		}
	}

	designatorMap.RemoveAll();

	return designatorErr;
}

/******************************************************************************
* ConvertLogicToPhysicalSchematic
*/
void ConvertLogicToPhysicalSchematic(CCEtoODBDoc *docPtr, FileStruct *filePtr)
{
   // These three maps are use to check for unique names
   CMapStringToString refNameMap;
   CMapStringToString designatorMap;
   CMapStringToString netnameMap;
	CMapStringToString cloneSheetMap;

   CMapStringToString comppinToNetnameMap;
   CMapStringToString comppinToRemoveMap;
   CMapStringToPtr hierarchySheetToRemove;

   WORD geomNumKeywordIndex = (WORD)docPtr->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);

   PrepareMaps(docPtr, filePtr, &refNameMap, &designatorMap, &netnameMap, &comppinToNetnameMap, &comppinToRemoveMap);


	// Check to see if a hierachy sheet is used more than one.
	// If it is then add it to the cloneSheetMap to clone it,
	// else just use the original hierarchy sheet and propagate all the netname from port and sheet connector
   for (int i=0; i<docPtr->getMaxBlockIndex(); i++)
   {
      BlockStruct *sheet = docPtr->getBlockAt(i);

      if (sheet == NULL)
         continue;
      if (sheet->getFileNumber() != filePtr->getFileNumber())
         continue;

      if (sheet->getBlockType() == BLOCKTYPE_SHEET)
      {
         POSITION posSheet = sheet->getDataList().GetHeadPosition();
         while (posSheet)
         {
            DataStruct *hierarchySymbol = sheet->getDataList().GetNext(posSheet);

            // Make sure we have a hierarchy symbol insert
            if (hierarchySymbol->getDataType() != dataTypeInsert || (hierarchySymbol->getInsert()->getInsertType() != INSERTTYPE_SYMBOL &&
                hierarchySymbol->getInsert()->getInsertType() != INSERTTYPE_HIERARCHYSYMBOL))
               continue;

            Attrib* attrib = NULL;
            if (!hierarchySymbol->getAttributesRef() || !hierarchySymbol->getAttributesRef()->Lookup(geomNumKeywordIndex, attrib) || attrib == NULL)
               continue;

            // Look for the hierarchy sheets of this hierarchy symbol
            CString hierarchyGeomNumbers( GetAttribStringValue(docPtr, attrib) );
            while (hierarchyGeomNumbers != "")
            {
               int start = 0;
               CString geomNum = hierarchyGeomNumbers.Tokenize(" ", start);
               hierarchyGeomNumbers.Delete(0, strlen(geomNum));
               hierarchyGeomNumbers.Trim();

					CString value;
					if (cloneSheetMap.Lookup(geomNum, value))
					{
						// More than one encounter of the hierarch sheet so add it to the map with the geomNum as its value
						int count = atoi(value);
						value.Format("%d", ++count);
						cloneSheetMap.SetAt(geomNum, value);
					}
					else
					{
						// First first encounter of the hierarch sheet so add it to the map with blank as its value
						cloneSheetMap.SetAt(geomNum, "1");
					}
				}
			}
		}
	}


   for (int i=0; i<docPtr->getMaxBlockIndex(); i++)
   {
      BlockStruct *sheet = docPtr->getBlockAt(i);

      if (sheet == NULL)
         continue;

      if (sheet->getFileNumber() != filePtr->getFileNumber())
         continue;

      if (sheet->getBlockType() == BLOCKTYPE_SHEET)
      {
         POSITION posSheet = sheet->getDataList().GetHeadPosition();
         while (posSheet)
         {
            DataStruct *hierarchySymbol = sheet->getDataList().GetNext(posSheet);

            // Make sure we have a hierarchy symbol insert
            Attrib* attrib;

            if (hierarchySymbol->getDataType() != dataTypeInsert || (hierarchySymbol->getInsert()->getInsertType() != INSERTTYPE_SYMBOL &&
                hierarchySymbol->getInsert()->getInsertType() != INSERTTYPE_HIERARCHYSYMBOL))
               continue;

            if (!hierarchySymbol->getAttributesRef() || !hierarchySymbol->getAttributesRef()->Lookup(geomNumKeywordIndex, attrib))
               continue;

				// Look for the hierarchy sheets of this hierarchy symbol
            CString hierarchyGeomNum( GetAttribStringValue(docPtr, attrib) );
            CString newHierarchyGeomNum;

            while (hierarchyGeomNum != "")
            {
               int start = 0;
               CString geomNumString = hierarchyGeomNum.Tokenize(" \t\n", start);

               int geomNum = atoi(geomNumString);
               hierarchyGeomNum.Delete(0, strlen(geomNumString));
               hierarchyGeomNum.Trim();

               BlockStruct *hierarchySheet = docPtr->getBlockAt(geomNum);
               BlockStruct *symbolBlock = docPtr->getBlockAt(hierarchySymbol->getInsert()->getBlockNumber());

               CMapStringToString internalToExternalNetMap;
					CMapStringToString offAndOnPageNetMap;

               PreparNetToNetMap(docPtr, symbolBlock, hierarchySymbol->getInsert()->getRefname(), hierarchySheet->getName(), &internalToExternalNetMap, &comppinToNetnameMap);
					PreparOffAndOnPageNetMap(docPtr, hierarchySheet, &offAndOnPageNetMap);

               BlockStruct *cloneSheet = NULL;
					
					int useCount = 1;
					CString value;
					if (cloneSheetMap.Lookup(geomNumString, value))
						useCount = atoi(value);

					if (useCount > 1)
					{
						// The hierarchy sheet is used more than once so clone it
						CloneHierarchySheet(docPtr, filePtr, hierarchySymbol, &refNameMap, &designatorMap, &netnameMap,
								&internalToExternalNetMap, &offAndOnPageNetMap, &comppinToNetnameMap, hierarchySheet, &cloneSheet);

					}
					else
					{
						// The hierarchy sheet is only used once so use the original hierarchy sheet
						// and propagate all the netname from port and sheet connector
						PropagateNetList(docPtr, filePtr, hierarchySymbol, hierarchySheet, &netnameMap, &internalToExternalNetMap,
								&offAndOnPageNetMap, &comppinToNetnameMap);
					}

					if (cloneSheet)
					{
						newHierarchyGeomNum.AppendFormat(" %d", cloneSheet->getBlockNumber());

						//Add the hierarchy sheet to the map to be remove later
						CString key;
						key.Format("%d", geomNum);
						hierarchySheetToRemove.SetAt(key, hierarchySheet);
					}
					else
					{
						newHierarchyGeomNum.AppendFormat(" %d", hierarchySheet->getBlockNumber());
					}

               internalToExternalNetMap.RemoveAll();
					offAndOnPageNetMap.RemoveAll();
            }

            newHierarchyGeomNum.Trim();
            attrib->setStringValueIndex(docPtr->RegisterValue(newHierarchyGeomNum));
         }
      }
   }

   // Remove hierarchy sheet block & some comppin from netlist
   RemoveHierarchSheetAndComppin(docPtr, filePtr, &hierarchySheetToRemove, &comppinToRemoveMap);

   // Free all the maps
   refNameMap.RemoveAll();
   designatorMap.RemoveAll();
   netnameMap.RemoveAll();
	cloneSheetMap.RemoveAll();
   hierarchySheetToRemove.RemoveAll();
   comppinToRemoveMap.RemoveAll();
}

/******************************************************************************
* PrepareMaps
*/
static void PrepareMaps(CCEtoODBDoc *docPtr, FileStruct *filePtr, CMapStringToString *refNameMap, CMapStringToString *designatorMap,
                        CMapStringToString *netnameMap, CMapStringToString *comppinToNetnameMap, CMapStringToString *comppinToRemoveMap)
{
   WORD designatorKeywordIndex = docPtr->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
   WORD portnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_PORTNAME, 0);
   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);
   WORD geomNumKeywordIndex = (WORD)docPtr->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);

   // Get all the designator and store them in the map
   for (int i=0; i<docPtr->getMaxBlockIndex(); i++)
   {
      BlockStruct *sheet = docPtr->getBlockAt(i);

      if (sheet == NULL)
         continue;
      if (sheet->getFileNumber() != filePtr->getFileNumber())
         continue;

      if (sheet->getBlockType() == BLOCKTYPE_SHEET)
      {
         POSITION pos = sheet->getDataList().GetHeadPosition();
         while (pos)
         {
            DataStruct *data = sheet->getDataList().GetNext(pos);
            if (!data->getAttributesRef() || data->getInsert() == NULL)
               continue;

            // Make sure we have a component insert
				InsertStruct *insert = data->getInsert();
				if (insert->getInsertType() != insertTypeSymbol && insert->getInsertType() != insertTypeHierarchicalSymbol &&
					 insert->getInsertType() != insertTypePortInstance)
					 continue;

            // Add the refname to the map
            refNameMap->SetAt(insert->getRefname(), insert->getRefname());

            if (insert->getInsertType() == insertTypeSymbol || insert->getInsertType() == insertTypeHierarchicalSymbol)
            {
               Attrib* attrib;

               if (!data->getAttributesRef()->Lookup(designatorKeywordIndex, attrib))
                  continue;

               if (!attrib)
                  continue;

               // Add the designator to the map
               CString designator( GetAttribStringValue(docPtr, attrib) );
               designatorMap->SetAt(designator, designator);

               // If it is an hierarchy symbol, the comppin to comppinToRemoveMap to be remove later
               if (data->getAttributesRef()->Lookup(geomNumKeywordIndex, attrib))
               {
                  BlockStruct *block = docPtr->getBlockAt(data->getInsert()->getBlockNumber());

                  POSITION blockPos = block->getDataList().GetHeadPosition();
                  while (blockPos)
                  {
                     DataStruct *port = block->getDataList().GetNext(blockPos);
                     if (port->getDataType() != T_INSERT || port->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
                        continue;

                     // The key is the symbol refname + port refname
                     CString key;
                     key.Format("%s%c%s", data->getInsert()->getRefname(), SCH_DELIMITER, port->getInsert()->getRefname());

                     // Add the comppin to comppinToRemoveMap to be remove later
                     comppinToRemoveMap->SetAt(key, key);
                  }
               }
            }
            else if (insert->getInsertType() == insertTypePortInstance)
            {
               Attrib* attrib = NULL;

               // Get the port name
               if (!data->getAttributesRef()->Lookup(portnameKeywordIndex, attrib))
                  continue;

               if (attrib == NULL)
                  continue;

               CString portname( GetAttribStringValue(docPtr, attrib) );
         
               // Get the unique port name by adding the library, cell, cluster, and view name,
               // just like in DoJoined() in Edif200_in.cpp and portInstanceRef() in Edif300_in.cpp
               int index = sheet->getName().ReverseFind(SCH_DELIMITER);
               CString tmp = sheet->getName().Left(index);
               CString key;
               key.Format("%s[%s%]%c%s", portname, tmp.Trim(), SCH_DELIMITER, SCH_PORT_COMPPIN);
            
               // Add the comppin to comppinToRemoveMap to be remove later
               comppinToRemoveMap->SetAt(key, key);
            }
         }
      }
   }

   // Get all the netnames and store them in the Map
   POSITION netPos = filePtr->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = filePtr->getNetList().GetNext(netPos);
      netnameMap->SetAt(net->getNetName(), net->getNetName());

      POSITION comppinPos = net->getHeadCompPinPosition();
      while (comppinPos)
      {
         CompPinStruct *comppin = net->getNextCompPin(comppinPos);

         CString key = (CString)comppin->getRefDes() + (CString)SCH_DELIMITER + comppin->getPinName();
         comppinToNetnameMap->SetAt(key, net->getNetName());
      }
   }
}

/******************************************************************************
* PrepareNetToNetMap
*/
static void PreparNetToNetMap(CCEtoODBDoc *docPtr, BlockStruct *symbolBlock, CString symbolRefname, CString hierarchySheetName, 
                              CMapStringToString *netToNetMap, CMapStringToString *comppinToNetnameMap)
{
   Attrib* attrib = NULL;
   WORD portnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_PORTNAME, 0);
   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);

   POSITION posBlock = symbolBlock->getDataList().GetHeadPosition();
   while (posBlock)
   {
      // Make sure it is a port and has attribute map
      DataStruct *port = symbolBlock->getDataList().GetNext(posBlock);

      if (!port->getAttributesRef() || port->getDataType() != dataTypeInsert || port->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
         continue;

      // Get the portname of hierarchy symbol
      if (!port->getAttributesRef()->Lookup(portnameKeywordIndex, attrib))
         continue;

      if (attrib == NULL)
         continue;

      CString portname( GetAttribStringValue(docPtr, attrib) );

      // The key is the symbol refname + port refname
      CString key;
      key.Format("%s%c%s%", symbolRefname, SCH_DELIMITER, port->getInsert()->getRefname());

      // Get the netname on the port of hierarchy symbol
      CString externalNetname;
      if (!comppinToNetnameMap->Lookup(key, externalNetname))
         continue;

      // Get the unique port name by adding the library, cell, cluster, and view name,
      // just like in DoJoined() in Edif200_in.cpp and portInstanceRef() in Edif300_in.cpp
      int index = hierarchySheetName.ReverseFind(SCH_DELIMITER);
      CString tmp = hierarchySheetName.Left(index);
      key.Format("%s[%s%]%c%s", portname, tmp.Trim(), SCH_DELIMITER, SCH_PORT_COMPPIN);

      // Find the netname on the port of the hierarchy sheet
      CString internalNetname;
      if (!comppinToNetnameMap->Lookup(key, internalNetname))
         continue;

      // Map the netname linked to the port of hierarchy symbol to the netname linked to the port of hierarchy sheet 
      netToNetMap->SetAt(internalNetname, externalNetname);
   }
}

/******************************************************************************
* PreparOffAndOnPageNetMap
*/
static void PreparOffAndOnPageNetMap(CCEtoODBDoc *docPtr, BlockStruct *hierarchySheet, CMapStringToString *offAndOnPageNetMap)
{
	WORD netnameKey = (WORD)docPtr->IsKeyWord(ATT_NETNAME, 0);
	POSITION pos = hierarchySheet->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = hierarchySheet->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypeSheetConnector)
			continue;

		Attrib* attrib = NULL;
		if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(netnameKey, attrib))
			continue;
		if (attrib == NULL)
			continue;

		CString netname( GetAttribStringValue(docPtr, attrib) );
		offAndOnPageNetMap->SetAt(netname, netname);
	}
}

/******************************************************************************
* CloneHierarchySheet
*/
static void CloneHierarchySheet(CCEtoODBDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, CMapStringToString *refNameMap, 
                         CMapStringToString *designatorMap, CMapStringToString *netnameMap, CMapStringToString *netToNetMap,
                         CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap, BlockStruct *hierarchySheet, BlockStruct **clonedSheet)
{
   CCamCadData& camCadData = docPtr->getCamCadData();

   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);

   // Get a non-existing block name
   int count = 1;
   CString newBlockName;
   newBlockName.Format("%s_%d", hierarchySheet->getName(), count);
   newBlockName.MakeUpper();
   while (Graph_Block_Exists(docPtr, newBlockName, filePtr->getFileNumber()))
   {
      newBlockName.Format("%s_%d", hierarchySheet->getName(), ++count);
      newBlockName.MakeUpper();
   }

   // Set clonedSheet to new block
   (*clonedSheet) = Graph_Block_On(GBO_APPEND, newBlockName, filePtr->getFileNumber(), 0L);
   (*clonedSheet)->setBlockType(hierarchySheet->getBlockType());
   Graph_Block_Off();
   docPtr->CopyAttribs(&(*clonedSheet)->getAttributesRef(), hierarchySheet->getAttributesRef());

   // Rename the insert refname and designator of the cloned sheet
   POSITION pos = hierarchySheet->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = hierarchySheet->getDataList().GetNext(pos);
      DataStruct *copy = camCadData.getNewDataStruct(*data);
      (*clonedSheet)->getDataList().AddTail(copy);

      if (copy->getDataType() == T_INSERT && (data->getInsert()->getInsertType() == INSERTTYPE_SYMBOL || data->getInsert()->getInsertType() == INSERTTYPE_HIERARCHYSYMBOL))
      {
			int count = 1;
			CString newRefName;
			CString oldRefName = copy->getInsert()->getRefname();

			newRefName.Format("%s_%d", copy->getInsert()->getRefname(), count);
			while (refNameMap->Lookup(newRefName, newRefName))
				newRefName.Format("%s_%d", copy->getInsert()->getRefname(), ++count);
			refNameMap->SetAt(newRefName, newRefName);

			copy->getInsert()->setRefname(STRDUP(newRefName));

			FixNetList(docPtr, filePtr, copy, oldRefName, netnameMap, netToNetMap, offAndOnPageNetMap, comppinToNetnameMap);
      }
      else if (copy->getDataType() == T_POLY && copy->getGraphicClass() == graphicClassSignal)
      { 
         // If it is a net then rename net
			Attrib *netnameAttrib = NULL;
         if (!copy->getAttributesRef() || !copy->getAttributesRef()->Lookup(netnameKeywordIndex, netnameAttrib))
            continue;
         if (!netnameAttrib)
            continue;

			CString netname( GetAttribStringValue(docPtr, netnameAttrib) );  
         CString newNetname;

			if (!offAndOnPageNetMap->Lookup(netname, newNetname))
			{
				// First Check to see if netname is net connected to an off/on page connector

				if (!netToNetMap->Lookup(netname, newNetname))
				{
					// The net is not part of a hierarchy net so create a new and unique netname
					newNetname = netname;
				}

				// Rename the netname
				netnameAttrib->setStringValueIndex(docPtr->RegisterValue(newNetname));
			}
      }
   }

}

/******************************************************************************
* PropagateNetList
*/
static void PropagateNetList(CCEtoODBDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, BlockStruct *hierarchySheet, CMapStringToString *netnameMap,
									  CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap)
{
   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);

	// Rename the insert refname and designator of the cloned sheet
   POSITION pos = hierarchySheet->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = hierarchySheet->getDataList().GetNext(pos);

		if (data->getDataType() == T_INSERT && (data->getInsert()->getInsertType() == INSERTTYPE_SYMBOL || data->getInsert()->getInsertType() == INSERTTYPE_HIERARCHYSYMBOL))
      {
			FixNetList(docPtr, filePtr, data, data->getInsert()->getRefname(), netnameMap, netToNetMap, offAndOnPageNetMap, comppinToNetnameMap);
      }
      else if (data->getDataType() == T_POLY && data->getGraphicClass() == graphicClassSignal)
      { 
         // If it is a net then rename net
			Attrib *netnameAttrib = NULL;
         if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(netnameKeywordIndex, netnameAttrib))
            continue;
         if (!netnameAttrib)
            continue;

			CString netname( GetAttribStringValue(docPtr, netnameAttrib) );  
         CString newNetname;

			if (!offAndOnPageNetMap->Lookup(netname, newNetname))
			{
				// First Check to see if netname is net connected to an off/on page connector

				if (!netToNetMap->Lookup(netname, newNetname))
				{
					// The net is not part of a hierarchy net so leave it as it is
					newNetname = netname;
				}

				// Rename the netname
				netnameAttrib->setStringValueIndex(docPtr->RegisterValue(newNetname));
			}
      }
   }
}

/******************************************************************************
* FixNetList
*/
static void FixNetList(CCEtoODBDoc *docPtr, FileStruct *filePtr, DataStruct *data, CString prevRefName,  CMapStringToString *netnameMap,
								CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap)
{
   WORD designatorKeywordIndex = docPtr->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);
   WORD compDesKeywordIndex = docPtr->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, 0, valueTypeString);
   WORD pinDesKeywordIndex = docPtr->RegisterKeyWord(SCH_ATT_PINDESIGNATOR, 0, valueTypeString);


   // ==================================================
   // RENAME NETNAME ON THE PORT(PIN) AND FIX NETLIST
   // ==================================================
	if (prevRefName.IsEmpty())
		prevRefName = data->getInsert()->getRefname();

	CString compDes;
   Attrib *designatorAttrib = NULL;
   if (data->getAttributesRef() && data->getAttributesRef()->Lookup(designatorKeywordIndex, designatorAttrib))
   {
		// Use designator as compDes
		compDes = GetAttribStringValue(docPtr, designatorAttrib);
   }
   else
   {
      // Use the insert refname as compDes
      compDes = data->getInsert()->getRefname();
   }


   BlockStruct *block = docPtr->getBlockAt(data->getInsert()->getBlockNumber());
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      // (1) Get the port
      DataStruct *port = block->getDataList().GetNext(pos);
      if (!port->getAttributesRef() || !port->getDataType() == dataTypeInsert || port->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
         continue;

      // (2) Get the netname
      CString netname;
      CString key = (CString)prevRefName + (CString)SCH_DELIMITER + port->getInsert()->getRefname();
      if (!comppinToNetnameMap->Lookup(key, netname))
         continue;


      // (3) Get the designator of the port
		Attrib* attrib = NULL;
      if (!port->getAttributesRef()->Lookup(designatorKeywordIndex, attrib) || attrib == NULL)
         continue;
      CString pinDes( GetAttribStringValue(docPtr, attrib) );

      // (4) Get the new netname
      CString newNetname;
		if (!offAndOnPageNetMap->Lookup(netname, newNetname))
		{
			// First Check to see if netname is net connected to an off/on page connector

			if (!netToNetMap->Lookup(netname, newNetname))
			{
				// The net is not part of a hierarchy net leave as it is
				newNetname = netname;
			}
		}

      // (5) Add comppin to the renamed net
		NetStruct *net = NULL;										// original net that the port is connected to
      NetStruct *newNet = add_net(filePtr, newNetname);	// new net that the port is going to move to
		CompPinStruct *comppin = FindCompPin(filePtr, data->getInsert()->getRefname(), port->getInsert()->getRefname(),  &net);
		if (comppin == NULL)
		{
			comppin = add_comppin(filePtr, newNet, data->getInsert()->getRefname(), port->getInsert()->getRefname());
		}
		else if (net != newNet)
		{
			MoveCompPin(net, newNet, comppin);
		}

      // (6) Add the "compDes" and "pinDes" attributes
      docPtr->SetAttrib(&comppin->getAttributesRef(), compDesKeywordIndex, valueTypeString, compDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
      docPtr->SetAttrib(&comppin->getAttributesRef(), pinDesKeywordIndex, valueTypeString, pinDes.GetBuffer(0), attributeUpdateOverwrite, NULL);
   }
}

/******************************************************************************
* RemoveHierarchSheetAndComppin
*/
void RemoveHierarchSheetAndComppin(CCEtoODBDoc *docPtr, FileStruct *filePtr, CMapStringToPtr *hierarchySheetToRemove, CMapStringToString *comppinToRemoveMap)
{
   // Remove the original hierarchy sheet
   POSITION pos = hierarchySheetToRemove->GetStartPosition();
   while (pos)
   {
      CString key;
      void *voidPtr;
      hierarchySheetToRemove->GetNextAssoc(pos, key, voidPtr);

      BlockStruct *sheet = (BlockStruct*)voidPtr;
      if (!sheet)
         continue;

      // Remove the shet from the doc
      int geomNum = sheet->getBlockNumber();
      docPtr->FreeBlock(sheet);
      //docPtr->getBlockAt(geomNum) = NULL;
   }

   // Remove the comppin comppinToRemoveMap
   pos = comppinToRemoveMap->GetStartPosition();
   while (pos)
   {
      CString key;
      comppinToRemoveMap->GetNextAssoc(pos, key, key);
      
      if (key == "")
         continue;

      int index = key.ReverseFind(SCH_DELIMITER);
      CString comp = key.Left(index);
      CString pin = key.Right(key.GetLength() - index - 1);

      RemoveCompPin(filePtr, comp, pin);
   }
}


////////////////////////////////////////////////////////////////////////////////////////////
// CSchSelRefDes dialog
////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSchSelRefDes, CDialog)
CSchSelRefDes::CSchSelRefDes(CWnd* pParent /*=NULL*/)
   : CDialog(CSchSelRefDes::IDD, pParent)
   , m_significantAttrib(FALSE)
   , m_messageLabel(_T(""))
   , m_titleLabel(_T(""))
{
   forPcb = FALSE;
   docPtr = NULL;
   filePtr = NULL;
   m_titleLabel = "Select the attrubute that is used for the reference designator.";
   m_messageLabel = "Note:\n  If the attribute choosen does not exist, the name of the instance will be used.\n  Schematic:  If there are no reference designators, select the attribute that is used for the instance number.";
}

CSchSelRefDes::~CSchSelRefDes()
{
}

void CSchSelRefDes::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_SIGNIFICANTATTRIB, m_significantAttrib);
   DDX_Control(pDX, IDC_ATTRIB_LIST, m_attribList);
   DDX_Control(pDX, IDC_VALUE_LIST, m_valueList);
   DDX_Text(pDX, IDC_MESSAGE_LABEL, m_messageLabel);
   DDX_Text(pDX, IDC_TITLE_LABEL, m_titleLabel);
}


BEGIN_MESSAGE_MAP(CSchSelRefDes, CDialog)
   ON_LBN_SELCHANGE(IDC_ATTRIB_LIST, OnLbnSelchangeAttribList)
   ON_BN_CLICKED(IDC_SIGNIFICANTATTRIB, OnBnClickedSignificantattrib)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSchSelRefDes message handlers

// CSelectItem message handlers
BOOL CSchSelRefDes::OnInitDialog()
{
   CDialog::OnInitDialog();

   SetWindowText(dialogCaption);
   fillAttributes();
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSchSelRefDes::fillAttributes()
{
   // Clear both the lists
   m_attribList.ResetContent();
   m_valueList.ResetContent();

   // map used to make sure duplicates don't show
   int expectedCount = m_significantAttrib?10:1;
   for (int i=0; i<docPtr->getKeyWordArray().GetCount(); i++)
   {
      const KeyWordStruct* kw = docPtr->getKeyWordArray()[i];
      if (kw->getValueType() != valueTypeString)
         continue;
      
      CMapStringToString map;
      if (forPcb)
      {
         for (CDataListIterator dataList(*filePtr->getBlock(), insertTypePcbComponent); dataList.hasNext();)
         {
            DataStruct* data = dataList.getNext();
            Attrib* attrib = NULL;
            if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(i, attrib))
               continue;
               
            if (!m_significantAttrib || attrib->getValueType() == valueTypeString)
            {
               CString tmp;
               CString value( GetAttribStringValue(docPtr, attrib) );
               if (!map.Lookup(value, tmp))
                  map.SetAt(value, value);

               if (map.GetCount() >= expectedCount)
               {
                  m_attribList.SetItemData(m_attribList.AddString(kw->cc), i);
                  break;
               }
            }
         }
      }
      else
      {
         for (int k=0; k<docPtr->getMaxBlockIndex(); k++)
         {
            BlockStruct* block = docPtr->getBlockAt(k);
            if (block != NULL && block->getBlockType() == blockTypeSheet)
            {
               for (CDataListIterator dataList(*block, insertTypeSymbol); dataList.hasNext();)
               {
                  DataStruct* data = dataList.getNext();
                  Attrib* attrib = NULL;
                  if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(i, attrib))
                     continue;
                     
                  if (!m_significantAttrib || attrib->getValueType() == valueTypeString)
                  {
                     CString tmp;
                     CString value( GetAttribStringValue(docPtr, attrib) );
                     if (!map.Lookup(value, tmp))
                        map.SetAt(value, value);

                     if (map.GetCount() >= expectedCount)
                     {
                        m_attribList.SetItemData(m_attribList.AddString(kw->cc), i);
                        break;
                     }
                  }
               }

               if (map.GetCount() >= expectedCount)
                  break;
            }
         }
      }
   }

   m_attribList.SetCurSel(-1);
}

void CSchSelRefDes::fillValues()
{
   m_valueList.ResetContent();

   // map used to make sure duplicates don't show
   CMapStringToString map;
   int kwIndex = (int)m_attribList.GetItemData(m_attribList.GetCurSel());

   if (forPcb)
   {
      for (CDataListIterator dataList(*filePtr->getBlock(), insertTypePcbComponent); dataList.hasNext();)
      {
         DataStruct* data = dataList.getNext();
         Attrib* attrib = NULL;
         if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(kwIndex, attrib))
            continue;
            
         if (attrib->getValueType() == valueTypeString)
         {
            CString tmp;
            CString value( GetAttribStringValue(docPtr, attrib) );
            if (!map.Lookup(value, tmp))
            {
               m_valueList.AddString(value);
               map.SetAt(value, value);
            }
         }
      }
   }
   else
   {
      int expectedCount = m_significantAttrib?10:1;
      for (int k=0; k<docPtr->getMaxBlockIndex(); k++)
      {
         BlockStruct* block = docPtr->getBlockAt(k);
         if (block != NULL && block->getBlockType() == blockTypeSheet)
         {
            for (CDataListIterator dataList(*block, insertTypeSymbol); dataList.hasNext();)
            {
               DataStruct* data = dataList.getNext();
               Attrib* attrib = NULL;
               if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(kwIndex, attrib))
                  continue;
                  
               if (attrib->getValueType() == valueTypeString)
               {
                  CString tmp;
                  CString value( GetAttribStringValue(docPtr, attrib) );
                  if (!map.Lookup(value, tmp))
                  {
                     m_valueList.AddString(value);
                     map.SetAt(value, value);
                  }
               }
            }

            if (map.GetCount() >= expectedCount)
               break;
         }
      }
   }



}

void CSchSelRefDes::OnLbnSelchangeAttribList()
{
   fillValues();
   m_attribList.GetText(m_attribList.GetCurSel(), newRefDesValue);
}

void CSchSelRefDes::OnBnClickedSignificantattrib()
{
   UpdateData();
   fillAttributes();
}

void CSchSelRefDes::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   OnOK();
}




///////////////////////////////////////////////////////////////
// CSchSelAttributes dialog                                  //
///////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSchSelAttributes, CDialog)
CSchSelAttributes::CSchSelAttributes(CWnd* pParent /*=NULL*/)
   : CDialog(CSchSelAttributes::IDD, pParent)
   , m_uniqueAttrib(FALSE)
{
}

CSchSelAttributes::~CSchSelAttributes()
{
}

void CSchSelAttributes::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_UNIQUEATTRIB, m_uniqueAttrib);
   DDX_Control(pDX, IDC_REFDES_ATTRIB_LIST, m_refdesAttribList);
   DDX_Control(pDX, IDC_REFDES_VALUE_LIST, m_refdesValueList);
   DDX_Control(pDX, IDC_PINNO_ATTRIB_LIST, m_pinnoAttribList);
   DDX_Control(pDX, IDC_PINNO_VALUE_LIST, m_pinnoValueList);
}

BEGIN_MESSAGE_MAP(CSchSelAttributes, CDialog)
   ON_LBN_SELCHANGE(IDC_REFDES_ATTRIB_LIST, OnLbnSelchangeRefdesAttribList)
   ON_LBN_SELCHANGE(IDC_PINNO_ATTRIB_LIST, OnLbnSelchangePinnoAttribList)
   ON_BN_CLICKED(IDC_UNIQUEATTRIB, OnBnClickedUniqueattrib)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSchSelAttributes::OnInitDialog()
{
   CDialog::OnInitDialog();

   fillAttribValueMaps();
   fillAttribListboxes();
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CSchSelAttributes::OnLbnSelchangeRefdesAttribList()
{
   fillValueListboxes(TRUE);
   m_refdesAttribList.GetText(m_refdesAttribList.GetCurSel(), newRefDesAttrib);
}

void CSchSelAttributes::OnLbnSelchangePinnoAttribList()
{
   fillValueListboxes(FALSE);
   m_pinnoAttribList.GetText(m_pinnoAttribList.GetCurSel(), newPinnoAttrib);
}

void CSchSelAttributes::OnBnClickedUniqueattrib()
{
   UpdateData();
   fillAttribListboxes();
}

void CSchSelAttributes::OnBnClickedOk()
{
   if (newRefDesAttrib.IsEmpty() || newPinnoAttrib.IsEmpty())
   {
      ErrorMessage("Need to select both the REFERENCE DESIGNATOR attribute and PIN NUMBER attribute.", "Cannot Continue");
      return;
   }

   OnOK();

   // Need to free  
   // refdesAttribValueMap
   // pinnoAttribValueMap
}

/******************************************************************************
* CSchSelAttributes::fillAttribValueMaps
*/
void CSchSelAttributes::fillAttribValueMaps()
{

   for (int i=0; i<docPtr->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = docPtr->getBlockAt(i);

      if (block == NULL)
         continue;
      if (block->getFileNumber() != filePtr->getFileNumber())
         continue;

      if (block->getBlockType() == BLOCKTYPE_SHEET)
      {
         POSITION pos = block->getDataList().GetHeadPosition();
         while (pos)
         {
            DataStruct *data = block->getDataList().GetNext(pos);

            // Make sure we have a component insert
            if (data->getDataType() != T_INSERT && data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL)
               continue;

            if (!data->getAttributesRef())
               continue;

            for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
            {
               WORD keywordIndex;
               Attrib* attrib;
               data->getAttributesRef()->GetNextAssoc(attribPos, keywordIndex, attrib);
               
               if (attrib->getValueType() != valueTypeString)
                  continue;

               CString value( GetAttribStringValue(docPtr, attrib) );

               void* valueMapPtr;

               if (refdesAttribValueMap.Lookup(keywordIndex, valueMapPtr))
               {
                  CMapStringToString *valueMap = (CMapStringToString*)valueMapPtr;
                  valueMap->SetAt(value, value);
               }
               else
               {
                  CMapStringToString *valueMap = new CMapStringToString;
                  valueMap->SetAt(value, value);
                  refdesAttribValueMap.SetAt(keywordIndex, valueMap);
               }
            } //End of WHILE data->getAttributesRef()
         } //End of WHILE block->getDataList() 
      } //End of IF block->getBlockType()
      else if (block->getBlockType() == BLOCKTYPE_SYMBOL)
      {
         POSITION pos = block->getDataList().GetHeadPosition();
         while (pos)
         {
            DataStruct *data = block->getDataList().GetNext(pos);

            // Make sure we have either a component or pin
            if (data->getDataType() != T_INSERT && data->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
               continue;

            if (!data->getAttributesRef())
               continue;
            
            for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
            {
               WORD keywordIndex;
               Attrib* attrib;
               data->getAttributesRef()->GetNextAssoc(attribPos, keywordIndex, attrib);
               
               if (attrib->getValueType() != valueTypeString)
                  continue;

               CString value( GetAttribStringValue(docPtr, attrib) );

               void* valueMapPtr;

               if (pinnoAttribValueMap.Lookup(keywordIndex, valueMapPtr))
               {
                  CMapStringToString *valueMap = (CMapStringToString*)valueMapPtr;
                  valueMap->SetAt(value, value);
               }
               else
               {
                  CMapStringToString *valueMap = new CMapStringToString;
                  valueMap->SetAt(value, value);
                  pinnoAttribValueMap.SetAt(keywordIndex, valueMap);
               }
            } //End of WHILE data->getAttributesRef()
         } //End of WHILE block->getDataList()
      } //End of IF block->getBlockType()
   } //End of FOR docPtr->maxBlockArray

}

/******************************************************************************
* CSchSelAttributes::fillAttribListboxes
*/
void CSchSelAttributes::fillAttribListboxes()
{

   // Clear the lists
   m_refdesAttribList.ResetContent();
   m_refdesValueList.ResetContent();
   m_pinnoAttribList.ResetContent();
   m_pinnoValueList.ResetContent();

   // Fill the reference designator attribute listbox
   POSITION attribPos = refdesAttribValueMap.GetStartPosition();
   while (attribPos)
   {
      WORD keywordIndex;
      void *valueMapPtr;
      refdesAttribValueMap.GetNextAssoc(attribPos, keywordIndex, valueMapPtr);

      CMapStringToString *valueMap = (CMapStringToString*)valueMapPtr;

      if (m_uniqueAttrib && valueMap->GetCount() < 10)
         continue;

      const KeyWordStruct *kw = docPtr->getKeyWordArray()[keywordIndex];
      m_refdesAttribList.SetItemData(m_refdesAttribList.AddString(kw->cc), keywordIndex);
   }

   // Fill the pin number attribute listbox
   attribPos = pinnoAttribValueMap.GetStartPosition();
   while (attribPos)
   {
      WORD keywordIndex;
      void *valueMapPtr;
      pinnoAttribValueMap.GetNextAssoc(attribPos, keywordIndex, valueMapPtr);

      CMapStringToString *valueMap = (CMapStringToString*)valueMapPtr;

      if (m_uniqueAttrib && valueMap->GetCount() < 10)
         continue;

      const KeyWordStruct *kw = docPtr->getKeyWordArray()[keywordIndex];
      m_pinnoAttribList.SetItemData(m_pinnoAttribList.AddString(kw->cc), keywordIndex);
   }

   m_refdesAttribList.SetCurSel(-1);
   m_pinnoAttribList.SetCurSel(-1);
}

/******************************************************************************
* CSchSelAttributes::fillValueListboxes
*/
void CSchSelAttributes::fillValueListboxes(BOOL fillRefdes)
{
   if (fillRefdes)
   {  
      m_refdesValueList.ResetContent();

      WORD kwIndex = (int)m_refdesAttribList.GetItemData(m_refdesAttribList.GetCurSel());
      
      void *valueMapPtr;
      if (refdesAttribValueMap.Lookup(kwIndex, valueMapPtr))
      {
         CMapStringToString *valueMap = (CMapStringToString*)valueMapPtr;

         POSITION pos = valueMap->GetStartPosition();
         while (pos)
         {
            CString valueKey;
            CString value;
            valueMap->GetNextAssoc(pos, valueKey, value);
            m_refdesValueList.AddString(value);
         }
      }
   }
   else
   {
      m_pinnoValueList.ResetContent();

      WORD kwIndex = (int)m_pinnoAttribList.GetItemData(m_pinnoAttribList.GetCurSel());

      void *valueMapPtr;
      if (pinnoAttribValueMap.Lookup(kwIndex, valueMapPtr))
      {
         CMapStringToString *valueMap = (CMapStringToString*)valueMapPtr;

         POSITION pos = valueMap->GetStartPosition();
         while (pos)
         {
            CString valueKey;
            CString value;
            valueMap->GetNextAssoc(pos, valueKey, value);
            m_pinnoValueList.AddString(value);
         }
      }
   }
}

/******************************************************************************
* CSchSelAttributes::freeAttribValueMap
*/
void CSchSelAttributes::freeAttribValueMap()
{
   POSITION pos = refdesAttribValueMap.GetStartPosition();
   while (pos)
   {
      WORD key;
      void *valueMapPtr;
      refdesAttribValueMap.GetNextAssoc(pos, key, valueMapPtr);

      CMapWordToPtr *valueMap = (CMapWordToPtr*)valueMapPtr;
      valueMap->RemoveAll();
      delete valueMap;
   }
   refdesAttribValueMap.RemoveAll();

   pos = pinnoAttribValueMap.GetStartPosition();
   while (pos)
   {
      WORD key;
      void *valueMapPtr;
      pinnoAttribValueMap.GetNextAssoc(pos, key, valueMapPtr);

      CMapWordToPtr *valueMap = (CMapWordToPtr*)valueMapPtr;
      valueMap->RemoveAll();
      delete valueMap;
   }
   pinnoAttribValueMap.RemoveAll();
}

//---------------------------------------------------------------------------------------
// AutoDetectSchematicDesignator
//---------------------------------------------------------------------------------------
void AutoDetectSchematicDesignator(CCEtoODBDoc& camcadDoc, FileStruct& schFile, CString& symbolDesignatorKeyword, CString& portDesignatorKeyword, CStringArray& symbolDesignatorBackAnnotateArray, CStringArray& portDesignatorBackannotateArray)
{
   /* Here is the rules for proper reference desigantor attribute for logical symbol
      1. Has at least 90% Unique value
      2. Cannot have space
      3. Of the unique value, at least 90% MUST be AlphaNumeric (e.g. U1), AlphaNumericAlpha (e.g. U1A), AlphaNumericSeparatorMoreCharacters (e.g. U1_A) format
      2. If all alpha prefix start with "I", then attribute is not designator

      Note: Typical alpha prefix will be "C, CR, D, F, J, L, P, Q, R, S, SW, T, U, X, Z"
            Typical alpha suffix will be "A, B, C, D, E, F"
            Typical separator will be "_, -, |" but could be any non alpha or numeric charactor
   */

   /* Here is the rules for proper reference designator attribue for pins within a symbol
      1. Has 100% unique value
      2. Cannot have space
      3. Can any of the following:
         a. Alpha (e.g. A")
         b. AlphaNumeric (e.g. A1)
         c. Numberic (e.g. 1)
         d. NumericAlpha (e.g. 1A)
         e. Symbol (e.g. "+" or "-") - Right now we assume only these are allowed
         f. Keyword (e.g. "POS", "PLUS", "NEG") - Right now we assume only these are allowed
   */

   symbolDesignatorKeyword.Empty();
   portDesignatorKeyword.Empty();

   if (schFile.getBlockType() != blockTypeSheet)
      return;

   int logicalSymbolCount = 0;
   int keyword = camcadDoc.RegisterKeyWord("ProperSchematicDesignator", 0, valueTypeNone);
   Attrib* attrib = NULL;
   if (schFile.getBlock() != NULL && !schFile.getBlock()->lookUpAttrib(keyword, attrib))
   {
      // Schematic file has not go throught AutoDetectSchematicDesignator routine so process file

      UniqueDesignatorKeywordMap uniqueSymbolKeywordMap;
      CTypedMapIntToPtrContainer<BlockStruct*> symbolBlockMap(nextPrime2n(20), false);
      CMapStringToString commonUniquePortKeywordMap;
      for (int i=0; i<camcadDoc.getMaxBlockIndex(); i++)
      {
         BlockStruct* block = camcadDoc.getBlockAt(i);
         if (block != NULL && block->getBlockType() == blockTypeSheet)
         {
            // Get uninque designator for logical symbols on sheets
            for (CDataListIterator dataList(*block, insertTypeSymbol); dataList.hasNext();)
            {
               DataStruct* symbolData = dataList.getNext();
               if (symbolData->getAttributesRef() != NULL)
               {
                  logicalSymbolCount++;
                  uniqueSymbolKeywordMap.addKeywords(camcadDoc, *symbolData->getAttributesRef());

                  BlockStruct* symbolBlock = NULL;
                  if (symbolData->getInsert() != NULL && !symbolBlockMap.Lookup(symbolData->getInsert()->getBlockNumber(), symbolBlock))
                  {
                     symbolBlock = camcadDoc.getBlockAt(symbolData->getInsert()->getBlockNumber());
                     symbolBlockMap.SetAt(symbolData->getInsert()->getBlockNumber(), symbolBlock);
                  }
               }
            }
         }
      }

      // Get Designator for symbol
      if (symbolDesignatorBackAnnotateArray.GetCount() > 0)
      {
         for (int i=0; i<symbolDesignatorBackAnnotateArray.GetCount(); i++)
         {
            CString keyword = symbolDesignatorBackAnnotateArray.GetAt(i);
            int keywordIndex = camcadDoc.IsKeyWord(keyword, 0);
            if (uniqueSymbolKeywordMap.lookup(keywordIndex) != NULL)
            {
               // Backannotate attribute existed on symbol so use the first one found
               symbolDesignatorKeyword = keyword;
               break;
            }
         }
      }

      if (symbolDesignatorKeyword.IsEmpty())
      {
         // No designator is found using symbolDesignatorBackAnnotateArray so find unique designator using automation
         symbolDesignatorKeyword= uniqueSymbolKeywordMap.getLogicalSymbolDesignatorKeyword(camcadDoc, logicalSymbolCount);
      }

      for (POSITION blockPos=symbolBlockMap.GetStartPosition(); blockPos!=NULL;)
      {
         int blockNumber;
         BlockStruct* symbolBlock = NULL;
         symbolBlockMap.GetNextAssoc(blockPos, blockNumber, symbolBlock);
         if (symbolBlock != NULL && symbolBlock->getBlockType() == blockTypeSymbol)
         {
            // Collect all attributes for pins on symbol
            UniqueDesignatorKeywordMap uniquePortKeywordMap;
            for (CDataListIterator dataList(*symbolBlock, insertTypePortInstance); dataList.hasNext();)
            {
               DataStruct* portData = dataList.getNext();
               if (portData->getAttributesRef() != NULL)
               {
                  uniquePortKeywordMap.addKeywords(camcadDoc, *portData->getAttributesRef());
               }
            }
         
            // Get map of unique designator for port instances on symbol block
            CMapStringToString keywordMap;
            uniquePortKeywordMap.getPortInstanceDesignatorKeyword(camcadDoc, keywordMap);

            if (commonUniquePortKeywordMap.GetCount() == 0)
            {
               // Copy the first set of possible keywords found on symbol block
               for (POSITION pos=keywordMap.GetStartPosition(); pos!=NULL;)
               {
                  CString key, keyword;
                  keywordMap.GetNextAssoc(pos, key, keyword);

                  if (!keyword.IsEmpty())
                  {
                     key.MakeLower();
                     commonUniquePortKeywordMap.SetAt(key, keyword);
                  }
               }                    
            }
            else
            {               
               // Remove any keyword that is not common among other symbol block
               for (POSITION pos=commonUniquePortKeywordMap.GetStartPosition(); pos!=NULL;)
               {
                  CString key, keyword, value;
                  commonUniquePortKeywordMap.GetNextAssoc(pos, key, keyword);

                  if (!keywordMap.Lookup(keyword, value))
                  {     
                     key.MakeLower();
                     commonUniquePortKeywordMap.RemoveKey(key);
                  }
               }
            }
         }
      }

      // Get Designator for port
      if (portDesignatorBackannotateArray.GetCount() > 0)
      {
         for (int i=0; i<portDesignatorBackannotateArray.GetCount(); i++)
         {
            CString keyword;
            CString key = portDesignatorBackannotateArray.GetAt(i);
            key.MakeLower();
            if (commonUniquePortKeywordMap.Lookup(key, keyword))
            {
               portDesignatorKeyword = keyword;
               break;
            }
         }
      }

      if (portDesignatorKeyword.IsEmpty())
      {
         // Get Designator for port.  Use the first common unique keyword from map.
         POSITION pos = commonUniquePortKeywordMap.GetStartPosition();
         if (pos != NULL)
         {
            CString key;
            commonUniquePortKeywordMap.GetNextAssoc(pos, key, portDesignatorKeyword);
         }
      }
   }
}

//---------------------------------------------------------------------------------------
// KeywordValues
//---------------------------------------------------------------------------------------
KeywordValues::KeywordValues(const int keywordIndex,  const CString name)
   : m_keywordIndex(keywordIndex)
   , m_name(name)
{
   m_usageCount = 0;
}

KeywordValues::~KeywordValues()
{
   m_originalValueMap.RemoveAll();
   m_valueCountMap.RemoveAll();
}

void KeywordValues::addValue(const CString value)
{
   CString keywordValue = value;
   keywordValue.Trim();
   keywordValue.MakeLower();
   if (!keywordValue.IsEmpty())
   {
      int count;
      if (m_valueCountMap.Lookup(keywordValue, count))
         count++;
      else
         count = 1;

      m_valueCountMap.SetAt(keywordValue, count);
      m_originalValueMap.SetAt(keywordValue, value);
      m_usageCount++;
   }
}  

int KeywordValues::getUniquePercentage()
{
   int retval = 0;
   int uniqueCount = 0;
   for (POSITION pos=m_valueCountMap.GetStartPosition(); pos!=NULL;)
   {
      CString value;
      int count;
      m_valueCountMap.GetNextAssoc(pos, value, count);
      if (count == 1)
      {
         uniqueCount++;
      }
      else
         int a = 0;
   }

   if (m_usageCount > 0)
   {
      retval = round(((double)uniqueCount/(double)m_usageCount) * 100);
   }

   return retval;
}

bool KeywordValues::checkValidLogicalSymbolDesignatorValue()
{
   /* Here is the rules for proper reference desigantor attribute
      1. Has at least 90% Unique value
      2. Cannot have space
      3. Of the unique value, at least 90% MUST be AlphaNumeric (e.g. U1), AlphaNumericAlpha (e.g. U1A), AlphaNumericSeparatorMoreCharacters (e.g. U1_A) format
      4. If all alpha prefix start with "I", then attribute is not designator

      Note: Typical alpha prefix will be "C, CR, D, F, J, L, P, Q, R, S, SW, T, U, X, Z"
            Typical alpha suffix will be "A, B, C, D, E, F"
            Typical separator will be "_, -, |" but could be any non alpha or numeric charactor
   */

   bool retval = false;

   if (getUniquePercentage() >= 90)
   {
      // Rule #1 - 90% unique value

      retval = true;   
      int prefixICount = 0;
      int validDesignatorCount = 0;

      for (POSITION pos=m_valueCountMap.GetStartPosition(); pos!=NULL;)
      {
         CString value;
         int count;
         m_valueCountMap.GetNextAssoc(pos, value, count);
         
         if (value.Find(" ") == -1 && isalpha(value.GetAt(0)) != 0)
         {
            // Rule #2 - No space
            // Rule #3 - Must start with alpha
            
            // Continue checking of rule #3
            bool validDesignator = true;
            bool hasNumeric = false;
            for (int i=0; i<value.GetLength(); i++)
            {
               bool numeric = is_number(value.Mid(i, 1)) > 0;
               bool alpha = isalpha(value.GetAt(i)) != 0;

               if (!numeric && !alpha && !hasNumeric)
               {
                  // Character is not alpha or numeric and is not prefix with AlphaNumberic so it is not valid
                  validDesignator = false;
                  break;
               }
               else if (numeric)
               {
                  hasNumeric = true;
               }
            }

            // Preparing for violation of rule #3
            if (validDesignator)
               validDesignatorCount++;

            // Preparing for violation of rule #4
            if (value.GetAt(0) == 'i')
               prefixICount++;
         }
      }

      // Rule #3 - Must have 90% of valid designator amoung the values
      if (validDesignatorCount < m_valueCountMap.GetCount() * 0.9)
         retval = false;

      // Rule #4 - Must NOT all has prefix of "I"
      if (prefixICount == m_valueCountMap.GetCount())
         retval = false;

   }

   return retval;
}

bool KeywordValues::checkValidPortInstanceDesignatorValue()
{
   /* Here is the rules for proper reference designator attribue for pins within a symbol
      1. Has 100% unique value
      2. Cannot have space
      3. Can any of the following:
         a. Alpha (e.g. A")
         b. AlphaNumeric (e.g. A1)
         c. Numberic (e.g. 1)
         d. NumericAlpha (e.g. 1A)
         e. Symbol (e.g. "+" or "-") - Right now we assume only these are allowed
         f. Keyword (e.g. "POS", "PLUS", "NEG") - Right now we assume only these are allowed
   */

   bool retval = false;

   if (getUniquePercentage() == 100)
   {
      // Rule #1 - 100% unique value

      retval = true;   
      int validDesignatorCount = 0;

      for (POSITION pos=m_valueCountMap.GetStartPosition(); pos!=NULL;)
      {
         CString value;
         int count;
         m_valueCountMap.GetNextAssoc(pos, value, count);
         
         if (value.Find(" ") > -1)
         {
            // Rule #2 - No space
            retval = false;
            break;
         }

         KeywordType keywordType = keywordTypeUnknown;
         if (value == "+" || value == "-" )
         {
            keywordType = keywordTypeSymbol;
         }
         else if (value == "pos" || value == "plus" || value == "neg")
         {
            keywordType = keywordTypeKeyword;
         }
         else
         {
            // Value is not one of the possible symbol or keyword so check for Alpa, AlphaNumeric, Numeric, or NumericAlpha.

            //*rcf This strikes me as extremely overzealous checking of value. It imposes a requirement that customers
            //*rcf follow certain conventions in naming their pins. Who are we to dictate how a pin should be named?
            //*rcf In any case, this code is here already, maybe somebody had a reason for it that is no longer apparent, and
            //*rcf so deemed too dangerous to just remove this. I suspect this will be an ongoing point of repair and 
            //*rcf adjustment.

            bool alphaNumeric = false;
            for (int i=0; i<value.GetLength(); i++)
            {
               bool numeric = is_number(value.Mid(i, 1)) > 0;
               bool alpha = isalpha(value.GetAt(i)) != 0;

               if (numeric)
               {
                  if (i == 0 || keywordType == keywordTypeNumeric)
                     keywordType = keywordTypeNumeric;
                  else if (keywordType == keywordTypeAlpha || keywordType == keywordTypeAlphaNumeric)
                     keywordType = keywordTypeAlphaNumeric;
                  else
                     keywordType = keywordTypeUnknown;
               }
               else if (alpha)
               {
                  if (i == 0 || keywordType == keywordTypeAlpha)
                     keywordType = keywordTypeAlpha;
                  else if (keywordType == keywordTypeNumeric || keywordType == keywordTypeNumericAlpha)
                     keywordType = keywordTypeNumericAlpha;
                  else
                     keywordType = keywordTypeUnknown;
               }

               if (keywordType == keywordTypeUnknown)
               {
                  retval = false;
                  break;
               }
            }

            if (!retval)
               break;
         }
      }
   }

   return retval;
}

//---------------------------------------------------------------------------------------
// UniqueKeywordMap
//---------------------------------------------------------------------------------------
UniqueDesignatorKeywordMap::UniqueDesignatorKeywordMap()
   : m_uniqueKeywordMap(nextPrime2n(20), true)
{
}

UniqueDesignatorKeywordMap::~UniqueDesignatorKeywordMap()
{
   m_uniqueKeywordMap.empty();
}

void UniqueDesignatorKeywordMap::addKeywords(CCEtoODBDoc& camcadDoc, CAttributes& attributes)
{
   for (POSITION pos=attributes.GetStartPosition(); pos!=NULL;)
   {
      WORD key;
      Attrib* attrib = NULL;
      attributes.GetNextAssoc(pos, key, attrib);
      if (attrib != NULL && attrib->getValueType() == valueTypeString)
      {
         CString value( GetAttribStringValue(&camcadDoc, attrib) );
         CString keyString = camcadDoc.getKeyword(key)->cc;
         KeywordValues* keywordValues = NULL;
         if (!m_uniqueKeywordMap.Lookup((int)key, keywordValues))
         {
            keywordValues = new KeywordValues((int)key, keyString);
            m_uniqueKeywordMap.SetAt((int)key, keywordValues);
         }
         keywordValues->addValue(value);
      }
   }
}

KeywordValues* UniqueDesignatorKeywordMap::lookup(int keywordIndex)
{
   KeywordValues* keywordValues = NULL;
   m_uniqueKeywordMap.Lookup(keywordIndex, keywordValues);

   return keywordValues;
}

CString UniqueDesignatorKeywordMap::getLogicalSymbolDesignatorKeyword(CCEtoODBDoc& camcadDoc, const int logicalSymbolCount)
{
   CString retval;

   // First, check the keyword DESIGNATOR to see if it is a valid designator
   KeywordValues* keywordValues = NULL;
   if (m_uniqueKeywordMap.Lookup(camcadDoc.RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString), keywordValues))
   {
      if (keywordValues->checkValidLogicalSymbolDesignatorValue() && keywordValues->getUsageCount() >= logicalSymbolCount*0.9)
      {
         return camcadDoc.getKeyword(keywordValues->getKeywordIndex())->cc;
      }
   }

   // If DESIGNATOR is not a valid designator, then inspect the values of each keyword against the rules for a proper designator
   int maxKeywordUsageCount = 0;
   for (POSITION pos=m_uniqueKeywordMap.GetStartPosition(); pos!=NULL;)
   {
      int keywordIndex;
      m_uniqueKeywordMap.GetNextAssoc(pos, keywordIndex, keywordValues);
      if (keywordValues != NULL && keywordValues->checkValidLogicalSymbolDesignatorValue())
      {
         if (keywordValues->getUsageCount() > maxKeywordUsageCount)
         {
            maxKeywordUsageCount = keywordValues->getUsageCount();
            retval = camcadDoc.getKeyword(keywordValues->getKeywordIndex())->cc;
         }
      }
   }

   return retval;
}

void UniqueDesignatorKeywordMap::getPortInstanceDesignatorKeyword(CCEtoODBDoc& camcadDoc, CMapStringToString& uniqueDesignatorMap)
{
   uniqueDesignatorMap.RemoveAll();

   // First, check the keyword DESIGNATOR to see if it is a valid designator
   KeywordValues* keywordValues = NULL;
   if (m_uniqueKeywordMap.Lookup(camcadDoc.RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString), keywordValues))
   {
      if (keywordValues->checkValidPortInstanceDesignatorValue())
      {
         uniqueDesignatorMap.SetAt(camcadDoc.getKeyword(keywordValues->getKeywordIndex())->cc, camcadDoc.getKeyword(keywordValues->getKeywordIndex())->cc);
         return;
      }
   }

   // If DESIGNATOR is not a valid designator, then inspect the values of each keyword against the rules for a proper designator
   for (POSITION pos=m_uniqueKeywordMap.GetStartPosition(); pos!=NULL;)
   {
      int keywordIndex;
      m_uniqueKeywordMap.GetNextAssoc(pos, keywordIndex, keywordValues);
      if (keywordValues != NULL && keywordValues->checkValidPortInstanceDesignatorValue())
      {
         uniqueDesignatorMap.SetAt(camcadDoc.getKeyword(keywordValues->getKeywordIndex())->cc, camcadDoc.getKeyword(keywordValues->getKeywordIndex())->cc);
      }
   }
}