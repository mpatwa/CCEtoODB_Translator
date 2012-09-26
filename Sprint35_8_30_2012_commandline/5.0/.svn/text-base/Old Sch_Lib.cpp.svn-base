// $Header: /CAMCAD/4.6/Sch_Lib.cpp 46    4/30/06 1:02p Kurt Van Ness $

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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* Function Prototypes *********************************************************/

static void PrepareMaps(CCAMCADDoc *docPtr, FileStruct *filePtr, CMapStringToString *refNameMap, CMapStringToString *designatorMap,
                  CMapStringToString *netnameMap, CMapStringToString *comppinToNetnameMap, CMapStringToString *comppinToRemoveMap);
static void PreparNetToNetMap(CCAMCADDoc *docPtr, BlockStruct *symbolBlock, CString symbolRefname, CString hierarchySheetName,
                  CMapStringToString *netToNetMap, CMapStringToString *comppinToNetnameMap);
static void PreparOffAndOnPageNetMap(CCAMCADDoc *docPtr, BlockStruct *hierarchySheet, CMapStringToString *offAndOnPageNetMap);
static void CloneHierarchySheet(CCAMCADDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, CMapStringToString *refNameMap, 
                  CMapStringToString *designatorMap, CMapStringToString *netnameMap, CMapStringToString *netToNetMap,
                  CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap, BlockStruct *hierarchySheet, BlockStruct **clonedSheet);
static void PropagateNetList(CCAMCADDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, BlockStruct *hierarchySheet, CMapStringToString *netnameMap,
									  CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap);
static void FixNetList(CCAMCADDoc *docPtr, FileStruct *filePtr, DataStruct *data, CString prevRefName,  CMapStringToString *netnameMap,
								CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap);

/******************************************************************************
* CCAMCADDoc::OnSchematicChooserefdes
*/
void CCAMCADDoc::OnSchematicChooserefdes()
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

/******************************************************************************
* RenameCompAndPinDesignator
*/
void RenameCompAndPinDesignator(CCAMCADDoc *doc, int fileNum, CString refdesAttrib, CString pinnoAttrib,
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
   int refdesMapKey = doc->RegisterKeyWord(SCH_ATT_REFDES_MAP, 0, VT_STRING);
   int pinnoMapKey = doc->RegisterKeyWord(SCH_ATT_PINNO_MAP, 0, VT_STRING);

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;
      if (block->getFileNumber() != fileNum)
         continue;

      if (block->getBlockType() == BLOCKTYPE_SHEET)
      {
         // To indicate that this is schematic file, not a regular PCB file
         if (refdesAttrib != "")
            doc->SetAttrib(&block->getAttributesRef(), refdesMapKey, VT_STRING, SCH_ATT_SECONDDESIGNATOR, SA_OVERWRITE, NULL);

         if (pinnoAttrib != "")
            doc->SetAttrib(&block->getAttributesRef(), pinnoMapKey, VT_STRING, pinnoAttrib.GetBuffer(0), SA_OVERWRITE, NULL);
      }
   }


   // Here are do the followings:
   // - Add the attribute SCH_ATT_SECONDDESIGNATOR to every component
   // - Re-assign the value of compDesignator and pinDesignator in netlist basic on the value
   // of the selected "refdesAttrib" and "pinnoAttrib"

   int compDesKey = doc->RegisterKeyWord("compDesignator", 0, VT_STRING);
   int pinDesKey = doc->RegisterKeyWord("pinDesignator", 0, VT_STRING);
   int secondDesKey = doc->RegisterKeyWord(SCH_ATT_SECONDDESIGNATOR, 0, VT_STRING);
   WORD refdesKey = doc->IsKeyWord(refdesAttrib, 0);
   WORD pinnoKey = doc->IsKeyWord(pinnoAttrib, 0);
   WORD refdesLocKey = doc->IsKeyWord(refdesLocAttrib, 0);
   WORD pinnoLocKey = doc->IsKeyWord(pinnoLocAttrib, 0);

   int count = 0;
   void *voidPtr;
   Attrib *attrib;

   CMapStringToString designatorMap;

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

         if (sheetBlock->getBlockType() != BLOCKTYPE_SHEET || sheetBlock->getFileNumber() != file->getFileNumber())
            continue;

         //(2) Find the instance insert
         POSITION dataPos = sheetBlock->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = sheetBlock->getDataList().GetNext(dataPos);
            if (data->getDataType() != T_INSERT)
               continue;
            if (data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL)
               continue;
            if (data->getAttributesRef() == NULL)
               continue;

            //(3) Get the designator of the instance
            CString newCompDes = "";
            if (refdesAttrib != "")
            {
               // If there is a new selected designator attribute provided then look for it

               if (!data->getAttributesRef()->Lookup(refdesKey, voidPtr))
                  continue;
               attrib = (Attrib*)voidPtr;
               CString compDes = "";
               if (attrib)
                  compDes = doc->ValueArray[attrib->getStringValueIndex()];

               // (3.1) Make sure the compDes is unique
               count = 0;
               newCompDes = compDes;
               //while (designatorMap.Lookup(newCompDes, newCompDes))
               // newCompDes.Format("%s_%d", compDes, ++count);
               //designatorMap.SetAt(newCompDes, newCompDes);

               // (3.2) Add the new compDes as a 2nd designator
               Attrib *newAttrib = NULL;
               doc->SetAttrib(&data->getAttributesRef(), secondDesKey, VT_STRING, newCompDes.GetBuffer(0), SA_OVERWRITE, &newAttrib);

               if (refdesLocAttrib != "" && newAttrib)
               {
                  // (3.3) If there is a new selected designator location attribute provided then look for it
                  if (data->getAttributesRef()->Lookup(refdesLocKey, voidPtr))
                  {
                     // Now copy the location from this attribute
                     attrib = (Attrib*)voidPtr;
                     
                     newAttrib->setCoordinate(attrib->getCoordinate());
                     newAttrib->setRotationRadians( attrib->getRotationRadians());
                     newAttrib->setHeight( attrib->getHeight());
                     newAttrib->setWidth( attrib->getWidth());
                     newAttrib->setProportionalSpacing( attrib->isProportionallySpaced());
                     newAttrib->setPenWidthIndex( attrib->getPenWidthIndex());
                     newAttrib->setMirrorDisabled( attrib->getMirrorDisabled());
                     newAttrib->setVisible( attrib->isVisible());
                     newAttrib->setFlags( attrib->getFlags());
                     newAttrib->setLayerIndex( attrib->getLayerIndex());
                     newAttrib->setInherited( newAttrib->isInherited());
                     newAttrib->setHorizontalPosition(attrib->getHorizontalPosition());
                     newAttrib->setVerticalPosition(attrib->getVerticalPosition());

                     // Turn off the visibility of the given attribute
                     attrib->setVisible(false);
                  }
               }
            }
            BlockStruct *insertedBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());

            //(5) Find the port insert
            POSITION portDataPos = insertedBlock->getDataList().GetHeadPosition();
            while (portDataPos)
            {
               DataStruct *portData = insertedBlock->getDataList().GetNext(portDataPos);
               if (portData->getDataType() != T_INSERT)
                  continue;
               if (portData->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
                  continue;
               if (!portData->getAttributesRef())
                  continue;

               //(6) Get the designator of the port
               CString pinDes = "";
               if (pinnoAttrib != "")
               {
                  // If there is a new selected pin number attribute provided then look for it
                  if (!portData->getAttributesRef()->Lookup(pinnoKey, voidPtr))
                     continue;

                  attrib = (Attrib*)voidPtr;
                  if (attrib)
                     pinDes = doc->ValueArray[attrib->getStringValueIndex()];

                  if (!portData->getAttributesRef()->Lookup(secondDesKey, voidPtr))
                  {
                     // (6.1) Add the new pinDes as a 2nd designator if it is no already there
                     Attrib *newAttrib = NULL;
                     doc->SetAttrib(&portData->getAttributesRef(), secondDesKey, VT_STRING, pinDes.GetBuffer(0), SA_OVERWRITE, &newAttrib);

                     if (pinnoLocAttrib != "" && newAttrib)
                     {
                        // (3.3) If there is a new selected designator location attribute provided then look for it
                        if (portData->getAttributesRef()->Lookup(pinnoLocKey, voidPtr))
                        {
                           // Now copy the location from this attribute
                           attrib = (Attrib*)voidPtr;
                           
                           newAttrib->setCoordinate(attrib->getCoordinate());
                           newAttrib->setRotationRadians( attrib->getRotationRadians());
                           newAttrib->setHeight( attrib->getHeight());
                           newAttrib->setWidth( attrib->getWidth());
                           newAttrib->setProportionalSpacing( attrib->isProportionallySpaced());
                           newAttrib->setPenWidthIndex( attrib->getPenWidthIndex());
                           newAttrib->setMirrorDisabled( attrib->getMirrorDisabled());
                           newAttrib->setVisible( attrib->isVisible());
                           newAttrib->setFlags( attrib->getFlags());
                           newAttrib->setLayerIndex( attrib->getLayerIndex());
                           newAttrib->setInherited( newAttrib->isInherited());
                           newAttrib->setHorizontalPosition(attrib->getHorizontalPosition());
                           newAttrib->setVerticalPosition(attrib->getVerticalPosition());

                           // Turn off the visibility of the given attribute
                           attrib->setVisible(false);
                        }
                     }
                  }
               }

               //(7) Find the net
               POSITION netPos = file->getNetList().GetHeadPosition();
               while (netPos)
               {
                  NetStruct *net = file->getNetList().GetNext(netPos);
                  
                  //(8) Find the comppin
                  POSITION comppinPos = net->getHeadCompPinPosition();
                  while (comppinPos)
                  {
                     CompPinStruct *comppin = net->getNextCompPin(comppinPos);
                     if (comppin->getRefDes().CompareNoCase(data->getInsert()->getRefname()))
                        continue;
                     if (comppin->getPinName().CompareNoCase(portData->getInsert()->getRefname()))
                        continue;

                     //(9) Re-assign the "compDesignator" attributes
                     if (refdesAttrib != "")
                        doc->SetAttrib(&comppin->getAttributesRef(), compDesKey, VT_STRING, newCompDes.GetBuffer(0), SA_OVERWRITE, NULL);

                     //(10) Re-assign the "pinDesignator" attribute if there is a new selected pin number attribute provided
                     if (pinnoAttrib != "")
                        doc->SetAttrib(&comppin->getAttributesRef(), pinDesKey, VT_STRING, pinDes.GetBuffer(0), SA_OVERWRITE, NULL);
                  }
               }
            }
         }
      }
   }

}

int CheckMissingOrDuplicateDesignator(CCAMCADDoc *docPtr, FileStruct *filePtr, FILE *errorFile, long &errorCount)
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

            Attrib *attrib = NULL;
            if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(designatorKeywordIndex, attrib) || attrib == NULL)
               continue;

				CString tmpString = "";
				CString designator = docPtr->ValueArray.GetAt(attrib->getStringValueIndex());
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
void ConvertLogicToPhysicalSchematic(CCAMCADDoc *docPtr, FileStruct *filePtr)
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

            Attrib *attrib = NULL;
            if (!hierarchySymbol->getAttributesRef() || !hierarchySymbol->getAttributesRef()->Lookup(geomNumKeywordIndex, attrib) || attrib == NULL)
               continue;

            // Look for the hierarchy sheets of this hierarchy symbol
            CString hierarchyGeomNumbers = docPtr->ValueArray[attrib->getStringValueIndex()];
            while (hierarchyGeomNumbers != "")
            {
               int start = 0;
               CString geomNum = hierarchyGeomNumbers.Tokenize(" ", start);
               hierarchyGeomNumbers.Delete(0, strlen(geomNum));
               hierarchyGeomNumbers.Trim();

					CString value = "";
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
            void *voidPtr;
            if (hierarchySymbol->getDataType() != dataTypeInsert || (hierarchySymbol->getInsert()->getInsertType() != INSERTTYPE_SYMBOL &&
                hierarchySymbol->getInsert()->getInsertType() != INSERTTYPE_HIERARCHYSYMBOL))
               continue;
            if (!hierarchySymbol->getAttributesRef() || !hierarchySymbol->getAttributesRef()->Lookup(geomNumKeywordIndex, voidPtr))
               continue;
            Attrib *attrib = (Attrib*)voidPtr;


				// Look for the hierarchy sheets of this hierarchy symbol
            CString hierarchyGeomNum = docPtr->ValueArray[attrib->getStringValueIndex()];
            CString newHierarchyGeomNum = "";
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
					CString value = "";
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
static void PrepareMaps(CCAMCADDoc *docPtr, FileStruct *filePtr, CMapStringToString *refNameMap, CMapStringToString *designatorMap,
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
               void *voidPtr;
               if (!data->getAttributesRef()->Lookup(designatorKeywordIndex, voidPtr))
                  continue;

               Attrib *attrib = (Attrib*)voidPtr;
               if (!attrib)
                  continue;

               // Add the designator to the map
               CString designator = docPtr->ValueArray[attrib->getStringValueIndex()];
               designatorMap->SetAt(designator, designator);

               // If it is an hierarchy symbol, the comppin to comppinToRemoveMap to be remove later
               if (data->getAttributesRef()->Lookup(geomNumKeywordIndex, voidPtr))
               {
                  BlockStruct *block = docPtr->getBlockAt(data->getInsert()->getBlockNumber());

                  POSITION blockPos = block->getDataList().GetHeadPosition();
                  while (blockPos)
                  {
                     DataStruct *port = block->getDataList().GetNext(blockPos);
                     if (port->getDataType() != T_INSERT || port->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
                        continue;

                     // The key is the symbol refname + port refname
                     CString key = "";
                     key.Format("%s%c%s", data->getInsert()->getRefname(), SCH_DELIMITER, port->getInsert()->getRefname());

                     // Add the comppin to comppinToRemoveMap to be remove later
                     comppinToRemoveMap->SetAt(key, key);
                  }
               }
            }
            else if (insert->getInsertType() == insertTypePortInstance)
            {
               void *voidPtr = NULL;
               Attrib *attrib = NULL;

               // Get the port name
               if (!data->getAttributesRef()->Lookup(portnameKeywordIndex, voidPtr))
                  continue;
               if (!(attrib = (Attrib*)voidPtr))
                  continue;
               CString portname = docPtr->ValueArray[attrib->getStringValueIndex()];
         
               // Get the unique port name by adding the library, cell, cluster, and view name,
               // just like in DoJoined() in Edif200_in.cpp and portInstanceRef() in Edif300_in.cpp
               int index = sheet->getName().ReverseFind(SCH_DELIMITER);
               CString tmp = sheet->getName().Left(index);
               CString key = "";
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
static void PreparNetToNetMap(CCAMCADDoc *docPtr, BlockStruct *symbolBlock, CString symbolRefname, CString hierarchySheetName, 
                              CMapStringToString *netToNetMap, CMapStringToString *comppinToNetnameMap)
{
   void *voidPtr = NULL;
   Attrib *attrib = NULL;
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
      if (!port->getAttributesRef()->Lookup(portnameKeywordIndex, voidPtr))
         continue;
      if (!(attrib = (Attrib*)voidPtr))
         continue;
      CString portname = docPtr->ValueArray[attrib->getStringValueIndex()];


      // The key is the symbol refname + port refname
      CString key = "";
      key.Format("%s%c%s%", symbolRefname, SCH_DELIMITER, port->getInsert()->getRefname());

      // Get the netname on the port of hierarchy symbol
      CString externalNetname = "";
      if (!comppinToNetnameMap->Lookup(key, externalNetname))
         continue;

      // Get the unique port name by adding the library, cell, cluster, and view name,
      // just like in DoJoined() in Edif200_in.cpp and portInstanceRef() in Edif300_in.cpp
      int index = hierarchySheetName.ReverseFind(SCH_DELIMITER);
      CString tmp = hierarchySheetName.Left(index);
      key.Format("%s[%s%]%c%s", portname, tmp.Trim(), SCH_DELIMITER, SCH_PORT_COMPPIN);

      // Find the netname on the port of the hierarchy sheet
      CString internalNetname = "";
      if (!comppinToNetnameMap->Lookup(key, internalNetname))
         continue;

      // Map the netname linked to the port of hierarchy symbol to the netname linked to the port of hierarchy sheet 
      netToNetMap->SetAt(internalNetname, externalNetname);
   }
}

/******************************************************************************
* PreparOffAndOnPageNetMap
*/
static void PreparOffAndOnPageNetMap(CCAMCADDoc *docPtr, BlockStruct *hierarchySheet, CMapStringToString *offAndOnPageNetMap)
{
	WORD netnameKey = (WORD)docPtr->IsKeyWord(ATT_NETNAME, 0);
	POSITION pos = hierarchySheet->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = hierarchySheet->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypeSheetConnector)
			continue;

		Attrib *attrib = NULL;
		if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(netnameKey, attrib))
			continue;
		if (attrib == NULL)
			continue;

		CString netname = docPtr->ValueArray[attrib->getStringValueIndex()];
		offAndOnPageNetMap->SetAt(netname, netname);
	}
}

/******************************************************************************
* CloneHierarchySheet
*/
static void CloneHierarchySheet(CCAMCADDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, CMapStringToString *refNameMap, 
                         CMapStringToString *designatorMap, CMapStringToString *netnameMap, CMapStringToString *netToNetMap,
                         CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap, BlockStruct *hierarchySheet, BlockStruct **clonedSheet)
{
   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);

   // Get a non-existing block name
   int count = 1;
   CString newBlockName = "";
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
      DataStruct *copy = new DataStruct(*data);
      (*clonedSheet)->getDataList().AddTail(copy);

      if (copy->getDataType() == T_INSERT && (data->getInsert()->getInsertType() == INSERTTYPE_SYMBOL || data->getInsert()->getInsertType() == INSERTTYPE_HIERARCHYSYMBOL))
      {
			int count = 1;
			CString newRefName = "";
			CString oldRefName = copy->getInsert()->getRefname();

			newRefName.Format("%s_%d", copy->getInsert()->getRefname(), count);
			while (refNameMap->Lookup(newRefName, newRefName))
				newRefName.Format("%s_%d", copy->getInsert()->getRefname(), ++count);
			refNameMap->SetAt(newRefName, newRefName);

			copy->getInsert()->setRefname(strdup(newRefName));

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

			CString netname = docPtr->ValueArray[netnameAttrib->getStringValueIndex()];  
         CString newNetname = "";

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
static void PropagateNetList(CCAMCADDoc *docPtr, FileStruct *filePtr, DataStruct *hierarchySymbol, BlockStruct *hierarchySheet, CMapStringToString *netnameMap,
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

			CString netname = docPtr->ValueArray[netnameAttrib->getStringValueIndex()];  
         CString newNetname = "";

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
static void FixNetList(CCAMCADDoc *docPtr, FileStruct *filePtr, DataStruct *data, CString prevRefName,  CMapStringToString *netnameMap,
								CMapStringToString *netToNetMap, CMapStringToString *offAndOnPageNetMap, CMapStringToString *comppinToNetnameMap)
{
   WORD designatorKeywordIndex = docPtr->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
   WORD netnameKeywordIndex = docPtr->IsKeyWord(SCH_ATT_NETNAME, 0);
   WORD compDesKeywordIndex = docPtr->RegisterKeyWord("compDesignator", 0, VT_STRING);
   WORD pinDesKeywordIndex = docPtr->RegisterKeyWord("pinDesignator", 0, VT_STRING);


   // ==================================================
   // RENAME NETNAME ON THE PORT(PIN) AND FIX NETLIST
   // ==================================================
	if (prevRefName.IsEmpty())
		prevRefName = data->getInsert()->getRefname();

	CString compDes = "";
   Attrib *designatorAttrib = NULL;
   if (data->getAttributesRef() && data->getAttributesRef()->Lookup(designatorKeywordIndex, designatorAttrib))
   {
		// Use designator as compDes
		compDes = docPtr->ValueArray[designatorAttrib->getStringValueIndex()];
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
      CString netname = "";
      CString key = (CString)prevRefName + (CString)SCH_DELIMITER + port->getInsert()->getRefname();
      if (!comppinToNetnameMap->Lookup(key, netname))
         continue;


      // (3) Get the designator of the port
		Attrib *attrib = NULL;
      if (!port->getAttributesRef()->Lookup(designatorKeywordIndex, attrib) || attrib == NULL)
         continue;
      CString pinDes = docPtr->ValueArray[attrib->getStringValueIndex()];

      // (4) Get the new netname
      CString newNetname = "";
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
      docPtr->SetAttrib(&comppin->getAttributesRef(), compDesKeywordIndex, VT_STRING, compDes.GetBuffer(0), SA_OVERWRITE, NULL);
      docPtr->SetAttrib(&comppin->getAttributesRef(), pinDesKeywordIndex, VT_STRING, pinDes.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

/******************************************************************************
* RemoveHierarchSheetAndComppin
*/
void RemoveHierarchSheetAndComppin(CCAMCADDoc *docPtr, FileStruct *filePtr, CMapStringToPtr *hierarchySheetToRemove, CMapStringToString *comppinToRemoveMap)
{
   // Remove the original hierarchy sheet
   POSITION pos = hierarchySheetToRemove->GetStartPosition();
   while (pos)
   {
      CString key = "";
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
      CString key = "";
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
   fillAttribs();
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSchSelRefDes::fillAttribs()
{
   // Clear both the lists
   m_attribList.ResetContent();
   m_valueList.ResetContent();

   for (int i=0; i<docPtr->getKeyWordArray().GetCount(); i++)
   {
      const KeyWordStruct *kw = docPtr->getKeyWordArray()[i];

      // Make sure the type is a string
      if (kw->getValueType() != VT_STRING)
         continue;

      if (m_significantAttrib)
      {
         CMapStringToString map;

         BlockStruct *block = filePtr->getBlock();
         POSITION pos = block->getDataList().GetHeadPosition();
         while (pos)
         {
            DataStruct *data = block->getDataList().GetNext(pos);

            if (forPcb)
            {
               // Make sure we have a component in the PCB
               if (data->getDataType() != T_INSERT)
                  continue;
               if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
                  continue;
            }
            else
            {
               // Make sure we have a component in the schematic
               if (data->getDataType() != T_INSERT)
                  continue;
               if (data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL)
                  continue;
            }

            LPVOID temp;
            if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(i, temp))
               continue;

            Attrib *attrib = (Attrib*)temp;
            if (attrib->getValueType() != VT_STRING)
               continue;

            CString value = docPtr->ValueArray[attrib->getStringValueIndex()];
            map.SetAt(value, value);
         } // END while (pos)

         if (map.GetCount() >= 10)
            m_attribList.SetItemData(m_attribList.AddString(kw->cc), i);
      } // END if (m_significantAttrib)
      else
      {
         BlockStruct *block = filePtr->getBlock();
         POSITION pos = block->getDataList().GetHeadPosition();
         while (pos)
         {
            DataStruct *data = block->getDataList().GetNext(pos);

            if (forPcb)
            {
               // Make sure we have a component in the PCB
               if (data->getDataType() != T_INSERT)
                  continue;
               if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
                  continue;
            }
            else
            {
               // Make sure we have a component in the schematic
               if (data->getDataType() != T_INSERT)
                  continue;
               if (data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL)
                  continue;
            }

            LPVOID temp;
            if (data->getAttributesRef() && data->getAttributesRef()->Lookup(i, temp))
               break;
         }

         // Only add keywords that are used for gates or components
         if (pos)
            m_attribList.SetItemData(m_attribList.AddString(kw->cc), i);
      }
   } // END for(int i=0; i<docPtr->getKeyWordArray().GetCount(); i++)

   m_attribList.SetCurSel(-1);
}

void CSchSelRefDes::fillValues()
{
   m_valueList.ResetContent();

   // map used to make sure duplicates don't show
   CMapStringToString map;

   int kwIndex = (int)m_attribList.GetItemData(m_attribList.GetCurSel());

   BlockStruct *block = filePtr->getBlock();
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      LPVOID temp;
      if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(kwIndex, temp))
         continue;

      Attrib *attrib = (Attrib*)temp;
      if (attrib->getValueType() != VT_STRING)
         continue;

      CString value = docPtr->ValueArray[attrib->getStringValueIndex()], tStr;

      // check if we've already added this before (don't add duplicates)
      if (!map.Lookup(value, tStr))
      {
         m_valueList.AddString(value);
         map.SetAt(value, value);
      }
   } // END while (pos)
}

void CSchSelRefDes::OnLbnSelchangeAttribList()
{
   fillValues();
   m_attribList.GetText(m_attribList.GetCurSel(), newRefDesValue);
}

void CSchSelRefDes::OnBnClickedSignificantattrib()
{
   UpdateData();
   fillAttribs();
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

            POSITION attribPos = data->getAttributesRef()->GetStartPosition();
            while (attribPos)
            {
               WORD keywordIndex;
               void *attribPtr;
               data->getAttributesRef()->GetNextAssoc(attribPos, keywordIndex, attribPtr);
               
               Attrib *attrib = (Attrib*)attribPtr;
               if (attrib->getValueType() != VT_STRING)
                  continue;
               CString value = docPtr->ValueArray[attrib->getStringValueIndex()];

               void *valueMapPtr;
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
            POSITION attribPos = data->getAttributesRef()->GetStartPosition();
            while (attribPos)
            {
               WORD keywordIndex;
               void *attribPtr;
               data->getAttributesRef()->GetNextAssoc(attribPos, keywordIndex, attribPtr);
               
               Attrib *attrib = (Attrib*)attribPtr;
               if (attrib->getValueType() != VT_STRING)
                  continue;
               CString value = docPtr->ValueArray[attrib->getStringValueIndex()];

               void *valueMapPtr;
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
