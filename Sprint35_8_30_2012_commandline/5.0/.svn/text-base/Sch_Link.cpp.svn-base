
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "sch_link.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "file.h"
#include "data.h"
#include "attrib.h"
#include "panref.h"
#include "net_hilt.h"
#include "net_util.h"
#include "float.h"
#include "gauge.h"
#include "api.h"
#include "Sch_Lib.h"
#include "PcbUtil.h"
#include "MainFrm.h"
#include "SchematicLink.h" // This is the new Schematic Link
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define _LOGRESULTS_
#endif

#ifdef SCHLINK_DEBUG
#define _LOGRESULTS_
#endif

#define SCHLINK_PCB_REFDES    "XREF_NAME"
#define SCHLINK_PCB_NETNAME   "XREF_NAME"

#define REFDES_KW             "DESIGNATOR"
#define SCHLINK_ERR           (CString)"Schematic linking failed due to the following errors:\n\n"

// indices for the xref arrays
#define SCH_XREF              0
#define PCB_XREF              1

// column numbers for the net xref list
#define SCH_COL               0
//#define SCH_FILTER_COL        1
#define PCB_COL               1
//#define PCB_FILTER_COL        3

// net comparison types
#define NETCOMPTYPE_NAME                  0
#define NETCOMPTYPE_CONTENT               1
#define NETCOMPTYPE_CONTENT_PCBASMASTER   2

// find option
#define FINDOPT_ORIGINALNAME  "ORIGINAL_NAME"
#define FINDOPT_COMPARENAME   "COMPARE_NAME"

// local pointers to the comparison arrays
static CSchCompArray *mPcbCompArray = NULL;
static CSchCompArray *mSchCompArray = NULL;
static CSchNetArray *mPcbNetArray = NULL;
static CSchNetArray *mSchNetArray = NULL;
static CFormatStdioFile genericLogFile;

extern CProgressDlg  *progress;
extern CMultiDocTemplate* pDocTemplate; // from CAMCAD.CPP

static int CompareSchematicNetData(const void* arg1, const void* arg2);

////////////////////////////////////////////////////////////////////////////////////////////
// Other Functions
////////////////////////////////////////////////////////////////////////////////////////////
BOOL GetLicenseString(CLSID clsid, BSTR &licStr)
{
   LPCLASSFACTORY2 pClassFactory;

   // Create an instance of the object and query it for
   //  the IClassFactory2 interface.
   if (SUCCEEDED(CoGetClassObject(clsid, CLSCTX_INPROC_SERVER, NULL,
      IID_IClassFactory2, (LPVOID *)(&pClassFactory))))
   {
      LICINFO licinfo;

      // Check to see if this object has a runtime license key
      if (SUCCEEDED(pClassFactory->GetLicInfo(&licinfo)))
      {
         if (licinfo.fRuntimeKeyAvail)
         {
            HRESULT hr;
            
            // The object has a runtime license key so request it
            hr = pClassFactory->RequestLicKey(0, &licStr);

            if (SUCCEEDED(hr))
               return TRUE;
         }
      }

      // Make sure we release the reference to the class factory
      pClassFactory->Release();
   }

   return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SchCompPinData Class
////////////////////////////////////////////////////////////////////////////////////////////
int SchCompPinData::Compare(SchCompPinData *cpData)
{
   return Compare(cpData->matchedCompName, cpData->PinName);
}

int SchCompPinData::Compare(CString compName, CString pinName)
{
   BOOL usePin = (pinName.Trim() != "?");

   // Did we add the component yet?  If so, we're done.
   if (!matchedCompName.Compare(compName) && (!usePin || !PinName.Compare(pinName)))
   {
      return 0;
   }

   // Does the component belong after this one?
   if (matchedCompName.Compare(compName) > 0)
      return 1;

   // If the components are the same, then does the pin belong after this one?
   if (!matchedCompName.Compare(compName) && (!usePin || PinName.Compare(pinName) > 0))
      return 1;

   return -1;
}

int SchCompPinData::CompareNoCase(SchCompPinData *cpData)
{
   return CompareNoCase(cpData->matchedCompName, cpData->PinName);
}

int SchCompPinData::CompareNoCase(CString compName, CString pinName)
{
   BOOL usePin = (pinName.Trim() != "?");

   // Did we add the component yet?  If so, we're done.
   if (!matchedCompName.CompareNoCase(compName) && (!usePin || !PinName.CompareNoCase(pinName)))
   {
      return 0;
   }

   // Does the component belong after this one?
   if (matchedCompName.CompareNoCase(compName) > 0)
      return 1;

   // If the components are the same, then does the pin belong after this one?
   if (!matchedCompName.CompareNoCase(compName) && (!usePin || PinName.CompareNoCase(pinName) > 0))
      return 1;

   return -1;
}




////////////////////////////////////////////////////////////////////////////////////////////
// SchNetData Class
////////////////////////////////////////////////////////////////////////////////////////////
SchNetData::SchNetData()
{ 
   OrigNetName = CompareName = "";
   data = NULL;
   net = NULL;
   Sheet = -1; 
   indexMatch = -1;
   xrfMatch = FALSE;
   m_iPossibleMatches = -1;
   m_matchedCount = 0;
}

SchNetData::~SchNetData()
{
   ClearCompPins();
}

void SchNetData::ClearCompPins()
{
   for (int i=0; i<cpArray.GetCount(); i++)
   {
      SchCompPinData *cpData = cpArray[i];
      cpArray[i] = NULL;
      delete cpData;
   }
   cpArray.RemoveAll();
   m_iPossibleMatches = -1;
   m_matchedCount = 0;
}

CString SchNetData::getCPCompName(CompPinStruct *cpData, BOOL pcb, CCEtoODBDoc *doc)
{
   if (pcb)
      return cpData->getRefDes();
   else
   {
      // Look up the "compDesignator" attribute
      LPVOID voidPtr = NULL;
      int kwIndex = doc->IsKeyWord(SCH_ATT_COMPDESIGNATOR, 0);
      Attrib* attrib;

      if (cpData->getAttributesRef() && cpData->getAttributesRef()->Lookup(kwIndex, attrib))
      {
         // Make sure the attribute is a string
         if (attrib->getValueType() == VT_STRING)
            return get_attvalue_string(doc, attrib);
      }

      return "";
   }
}

CString SchNetData::getCPPinName(CompPinStruct *cpData, BOOL pcb, CCEtoODBDoc *doc)
{
   if (pcb)
      return cpData->getPinName();
   else
   {
      // Look up the "pinDesignator" attribute
      Attrib* attrib;
      int kwIndex = doc->IsKeyWord(SCH_ATT_PINDESIGNATOR, 0);

      if (cpData->getAttributesRef() && cpData->getAttributesRef()->Lookup(kwIndex, attrib))
      {
         // Make sure the attribute is a string
         if (attrib->getValueType() == VT_STRING)
            return get_attvalue_string(doc, attrib);
      }
      return "";
   }
}

int SchNetData::addCompPin(CString compName, CString pinName, CString matchedName)
{
   if (compName.IsEmpty() || pinName.IsEmpty())
      return -1;

   CString componentName = (matchedName.IsEmpty()?compName:matchedName);

   // See if the component needs to be added and where
   int i;
   for (i=0; i<cpArray.GetCount(); i++)
   {
      SchCompPinData *cpData = cpArray[i];
      int compRes = cpData->Compare(componentName, pinName);

      // Did we add the comp/pin yet?  If so, we're done.
      if (!compRes)
         return i;

      // Do we add after this one?
      if (compRes > 0)
         break;
   }

   SchCompPinData *cpData = new SchCompPinData();
   cpArray.InsertAt(i, cpData);

   cpData->CompName = compName;
   cpData->PinName = pinName;
   cpData->matchedCompName = componentName;

   return i;
}

void SchNetData::SetNet(NetStruct *netData, BOOL pcb, CCEtoODBDoc *doc)
{
   if (!netData)
      return;

   // Add netlist
   POSITION pos = netData->getHeadCompPinPosition();
   while (pos)
   {
      CompPinStruct *cp = netData->getNextCompPin(pos);

      CString compName, pinName;
      compName = getCPCompName(cp, pcb, doc);
      pinName = getCPPinName(cp, pcb, doc);

      // Add the comp/pin if we don't need to look for matched components
      addCompPin(compName, pinName);
   }

   net = netData;
}

void SchNetData::ApplyCompMatches(BOOL pcb, CCEtoODBDoc *doc, SchLink *schLink)
{
   if (!net)
      return;

   ClearCompPins();

   // Add netlist
   POSITION pos = net->getHeadCompPinPosition();
   while (pos)
   {
      CompPinStruct *cp = net->getNextCompPin(pos);

      CString compName, pinName;
      compName = getCPCompName(cp, pcb, doc);
      pinName = getCPPinName(cp, pcb, doc);

      // Add the comp/pin if we don't need to look for matched components
      if (schLink == NULL)
      {
         addCompPin(compName, pinName);
         continue;
      }

      // Find the component in the schematics component list
      SchCompData *compData = schLink->findSchComp(compName, FINDOPT_COMPARENAME);
      if (!compData)
      {
         // just add the comp pin if there was not match
         addCompPin(compName, pinName);
         continue;
      }

      if (compData->indexMatch < 0)
      {
         // Set the matched component name to 'NULL' if there is no match
         // indicating that it not be used in the net comparison by content
         addCompPin(compName, pinName, "NULL");
         continue;
      }

      // Get the matching component in the PCB
      compData = mPcbCompArray->GetAt(compData->indexMatch);

      // Add the comp/pin
      addCompPin(compName, pinName, compData->CompareName);
   }
}

int SchNetData::GetPossibleMatches()
{
   if (m_iPossibleMatches >= 0)
      return m_iPossibleMatches;

   int count = 0;
   for (int i=0; i<cpArray.GetCount(); i++)
   {
      const SchCompPinData *schCPData = cpArray.GetAt(i);

      if (schCPData->matchedCompName != "NULL")
         count++;
   }

   m_iPossibleMatches = count;

   return count;
}

void SchNetData::AddMatchedCount(int matchedCount)
{
   m_matchedCount += matchedCount;;
}

long SchNetData::GetMatchedCount()
{
   return m_matchedCount;
}

long SchNetData::GetUnmatchedCount()
{
   long unmatchedCount = GetPossibleMatches() - GetMatchedCount();
   if (unmatchedCount < 0)
      return 0;
   else 
      return unmatchedCount;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SchLink Class
////////////////////////////////////////////////////////////////////////////////////////////
SchLink::SchLink()
{
   caseSensitive = FALSE;
   keepActiveDoc = TRUE;
   netCompType = NETCOMPTYPE_CONTENT_PCBASMASTER;
   minPercentMatch = 70;
   schRefDesAttrib = REFDES_KW;
   pcbRefDesAttrib = REFDES_KW;
   schNetNameAttrib = ATT_NETNAME;
   pcbNetNameAttrib = ATT_NETNAME;
   netLinkState = FALSE;
   compLinkState = FALSE;
   completelyDestroy = FALSE;
}

SchLink::~SchLink()
{
   completelyDestroy = TRUE;
   ClearAllData();
}

void SchLink::Reset()
{
   ClearAllData();

   pcbDocPtr = schDocPtr = NULL;
   pcbFilePtr = schFilePtr = NULL;
}

void SchLink::ClearAllData()
{
   // Clean up all the memory
   clearAllSheetData();
   clearAllSchCompData();
   clearAllSchNetData();
   clearAllPcbCompData();
   clearAllPcbNetData();

   clearSchXrefData();
   clearPcbXrefData();

   ClearFilterData();
   //filterSettings.FileReloaded = FALSE;

   removeHighlightedNets();
   removeOverriddenComps();

   netLinkState = FALSE;
   compLinkState = FALSE;
}

void SchLink::clearAllSheetData()
{
   sheetList.RemoveAll();
}

void SchLink::clearAllSchCompData()
{
   for (int i=0; i<schCompArray.GetCount(); i++)
   {
      SchCompData *compData = schCompArray[i];
      schCompArray[i] = NULL;
      delete compData;
   }
   schCompArray.RemoveAll();
}

void SchLink::clearAllSchNetData()
{
   for (int i=0; i<schNetArray.GetCount(); i++)
   {
      SchNetData *compData = schNetArray[i];
      schNetArray[i] = NULL;
      delete compData;
   }
   schNetArray.RemoveAll();
}

void SchLink::clearAllPcbCompData()
{
   for (int i=0; i<pcbCompArray.GetCount(); i++)
   {
      SchCompData *compData = pcbCompArray[i];
      pcbCompArray[i] = NULL;
      delete compData;
   }
   pcbCompArray.RemoveAll();
}

void SchLink::clearAllPcbNetData()
{
   for (int i=0; i<pcbNetArray.GetCount(); i++)
   {
      SchNetData *compData = pcbNetArray[i];
      pcbNetArray[i] = NULL;
      delete compData;
   }
   pcbNetArray.RemoveAll();
}

void SchLink::clearSchXrefData()
{
   CompXRefArray[SCH_XREF].RemoveAll();
   NetXRefArray[SCH_XREF].RemoveAll();
}

void SchLink::clearPcbXrefData()
{
   CompXRefArray[PCB_XREF].RemoveAll();
   NetXRefArray[PCB_XREF].RemoveAll();
}

CString SchLink::ApplyFilters(CString src, BOOL Component, BOOL Schematic)
{
   CString buf = src;

   if (Component)
   {
      // apply this filter
      if (killTrailingAlphas)
      {
         while (buf.GetLength() > 0 && isalpha(buf[buf.GetLength()-1]))
            buf = buf.Left(buf.GetLength()-1);

         // If everything is an alpha and was removed, then the name should not change
         if (!buf.GetLength())
            buf = src;
      }

      // apply this filter
      if (killTrailingSuffix)
      {
         int i = 1;
         while (i < buf.GetLength())
         {
            if (buf.Right(i).Find(trailingSuffix) == 0)
               break;
            i++;
         }

         // If there was no such character and was removed, then the name should not change
         if (i < buf.GetLength())
            buf = buf.Left(buf.GetLength() - i);
      }
   }
   else
   {
      buf = src;
      //buf = filterSettings.CheckName(src, Component, Schematic);
   }

   return buf;
}

int SchLink::fillArrays()
{
   ClearAllData();

   // Always create a new one (seems each process is responsible for creating and destroying
   progress = new CProgressDlg("Schematic Link");
   progress->Create(AfxGetMainWnd());

   progress->ShowWindow(SW_SHOW);
   progress->SetStep(1);

   int retVal;
   retVal = fillSheetArray();
   if (retVal == ERR_NOERROR)
   {
      retVal = fillSchArrays();
      if (retVal == ERR_NOERROR)
         retVal = fillPcbArrays();
   }

   progress->ShowWindow(SW_HIDE);
   if (progress->DestroyWindow())
   {
      delete progress;
      progress = NULL;
   }

   return retVal;
}

int SchLink::fillSchArrays()
{
   clearAllSchNetData();
   clearAllSchCompData();

   int retVal;
   retVal = fillSchCompArray();
   if (retVal != ERR_NOERROR)
      return retVal;
   retVal = fillSchNetArray();

   return retVal;
}

int SchLink::fillPcbArrays()
{
   clearAllPcbNetData();
   clearAllPcbCompData();

   int retVal;
   retVal = fillPcbCompArray();
   if (retVal != ERR_NOERROR)
      return retVal;
   retVal = fillPcbNetArray();

   return retVal;
}

int SchLink::fillNetArrays()
{
   clearAllSchNetData();
   clearAllPcbNetData();

   int retVal;
   retVal = fillSchNetArray();
   if (retVal != ERR_NOERROR)
      return retVal;
   retVal = fillPcbNetArray();

   return retVal;
}

int SchLink::fillCompArrays()
{
   clearAllSchCompData();
   clearAllPcbCompData();

   int retVal;
   retVal = fillSchCompArray();
   if (retVal != ERR_NOERROR)
      return retVal;
   retVal = fillPcbCompArray();

   return retVal;
}

int SchLink::fillSheetArray()
{
   // reset status 
   CString  buf;
   long maxRange = 0;

   if (!pcbDocPtr || !schDocPtr) return ERR_GENERAL;
   if (!pcbFilePtr || !schFilePtr) return ERR_GENERAL;

   //////////////////////////////////////////////////////////////////
   // Count the sheets in the schematic

   // Set up the progress bar
   buf.Format("Gathering sheets...");
   progress->SetStatus(buf);
   progress->SetRange32(0, schDocPtr->getMaxBlockIndex());
   progress->SetPos(0);
   for (int i=0; i<schDocPtr->getMaxBlockIndex(); i++)
   {
      progress->StepIt();

      BlockStruct *block = schDocPtr->getBlockAt(i);

      if (!block) 
         continue;

      if (block->getFileNumber() == schFilePtr->getFileNumber() && block->getBlockType() == BLOCKTYPE_SHEET)
         sheetList.AddTail(i);
   }

   return ERR_NOERROR;
}

int SchLink::fillSchCompArray()
{
   // reset status 
   CString  buf;
   long maxRange = 0;

   if (!pcbDocPtr || !schDocPtr) return ERR_GENERAL;
   if (!pcbFilePtr || !schFilePtr) return ERR_GENERAL;

   //////////////////////////////////////////////////////////////////
   // Fill the schematic component array
   // Count the interations for the progress bar
   maxRange = 0;
   POSITION sheetPos = sheetList.GetHeadPosition();
   while (sheetPos)
   {
      int sheetGeomNum = sheetList.GetNext(sheetPos);
      BlockStruct *schFileBlock = schDocPtr->getBlockAt(sheetGeomNum);
      POSITION schDataPos = schFileBlock->getDataList().GetHeadPosition();
      while (schDataPos)
      {
         schFileBlock->getDataList().GetNext(schDataPos);
         maxRange++;
      }
   } // END while (sheetPos)

   // Set up the progress bar
   buf.Format("Gathering schematic components...");
   progress->SetStatus(buf);
   progress->SetRange32(0, maxRange);
   progress->SetPos(0);

   maxRange = 0;
   sheetPos = sheetList.GetHeadPosition();
   while (sheetPos)
   {
      int sheetGeomNum = sheetList.GetNext(sheetPos);
      BlockStruct *schFileBlock = schDocPtr->getBlockAt(sheetGeomNum);
   
      POSITION schDataPos = schFileBlock->getDataList().GetHeadPosition();
      while (schDataPos)
      {
         progress->SetPos(maxRange++);

         DataStruct *schData = schFileBlock->getDataList().GetNext(schDataPos);
      
         // Add the data to the array
         //  addCompToArray will check for the data type and insert type
         addCompToArray(&schCompArray, schData, sheetGeomNum);
      } // END while (schDataPos)
   } // END while (sheetPos)

   // create map to schematic components
   m_schCompMapCompareName.RemoveAll();
   m_schCompMapRefName.RemoveAll();
   for (int index=0; index<schCompArray.GetCount(); index++)
   {
      SchCompData *schCompData = schCompArray[index];

      WORD foundIndex = -1;
      if (!m_schCompMapCompareName.Lookup(schCompData->CompareName, foundIndex))
         m_schCompMapCompareName.SetAt(schCompData->CompareName, index);

      if (!m_schCompMapRefName.Lookup(schCompData->OrigRefName, foundIndex))
         m_schCompMapRefName.SetAt(schCompData->OrigRefName, index);
   }

   return ERR_NOERROR;
}

int SchLink::fillSchNetArray()
{
   // reset status 
   CString  buf;
   long maxRange = 0;

   if (!pcbDocPtr || !schDocPtr) return ERR_GENERAL;
   if (!pcbFilePtr || !schFilePtr) return ERR_GENERAL;

   //////////////////////////////////////////////////////////////////
   // Fill the schematic net array
   // Count the interations for the progress bar
   maxRange = schFilePtr->getNetList().GetCount();
   POSITION sheetPos = sheetList.GetHeadPosition();
   while (sheetPos)
   {
      int sheetGeomNum = sheetList.GetNext(sheetPos);
      BlockStruct *schFileBlock = schDocPtr->getBlockAt(sheetGeomNum);
      POSITION schDataPos = schFileBlock->getDataList().GetHeadPosition();
      while (schDataPos)
      {
         schFileBlock->getDataList().GetNext(schDataPos);
         maxRange++;
      }
   } // END while (sheetPos)

   // Set up the progress bar
   buf.Format("Gathering schematic nets...");
   progress->SetStatus(buf);
   progress->SetRange32(0, maxRange);
   progress->SetPos(0);

//genericLogFile.Open("C:\\Development\\fillSchNetArray.txt", CFile::modeCreate|CFile::modeWrite);

   // Fill the schematic net array
   maxRange = 0;
   POSITION schNetPos = schFilePtr->getNetList().GetHeadPosition();
   while (schNetPos)
   {
      progress->SetPos(maxRange++);

      NetStruct *schNet = schFilePtr->getNetList().GetNext(schNetPos);

      if (schNet->getNetName() == NET_UNUSED_PINS)
         continue;
   
      // Add the data to the array
      //  addNetToArray will check for the data type, graphic class and net name
      addNetToArray(&schNetArray, schNet, schFilePtr->getBlock()->getBlockNumber());
   } // END while (schNetPos) ...

   sheetPos = sheetList.GetHeadPosition();
   while (sheetPos)
   {
      int sheetGeomNum = sheetList.GetNext(sheetPos);
      BlockStruct *schFileBlock = schDocPtr->getBlockAt(sheetGeomNum);
   
      POSITION schDataPos = schFileBlock->getDataList().GetHeadPosition();
      while (schDataPos)
      {
         progress->SetPos(maxRange++);

         DataStruct *schData = schFileBlock->getDataList().GetNext(schDataPos);
      
         // Add the data to the array
         //  addNetToArray will check for the data type, graphic class and net name
         addNetToArray(&schNetArray, schData, sheetGeomNum);
      } // END while (schDataPos)
   } // END while (sheetPos)

genericLogFile.Close();

   return ERR_NOERROR;
}

int SchLink::fillPcbCompArray()
{
   // reset status 
   CString  buf;
   long maxRange = 0;

   if (!pcbDocPtr || !schDocPtr) return ERR_GENERAL;
   if (!pcbFilePtr || !schFilePtr) return ERR_GENERAL;

   BlockStruct *pcbFileBlock = pcbFilePtr->getBlock();

   //////////////////////////////////////////////////////////////////
   // Fill the pcb component array
   // Count the interations for the progress bar
   maxRange = 0;
   POSITION pcbDataPos = pcbFileBlock->getDataList().GetHeadPosition();
   while (pcbDataPos)
   {
      pcbFileBlock->getDataList().GetNext(pcbDataPos);
      maxRange++;
   }

   // Set up the progress bar
   buf.Format("Gathering PCB components...");
   progress->SetStatus(buf);
   progress->SetRange32(0, maxRange);
   progress->SetPos(0);

   maxRange = 0;
   pcbDataPos = pcbFileBlock->getDataList().GetHeadPosition();
   while (pcbDataPos)
   {
      progress->SetPos(maxRange++);

      DataStruct *pcbData = pcbFileBlock->getDataList().GetNext(pcbDataPos);
      
      // Add the data to the array
      //  addCompToArray will check for the data type and insert type
      addCompToArray(&pcbCompArray, pcbData, -1);
   } // END while (pcbDataPos)

   return ERR_NOERROR;
}

int SchLink::fillPcbNetArray()
{
   // reset status 
   CString  buf;
   long maxRange = 0;

   if (!pcbDocPtr || !schDocPtr) return ERR_GENERAL;
   if (!pcbFilePtr || !schFilePtr) return ERR_GENERAL;

   BlockStruct *pcbFileBlock = pcbFilePtr->getBlock();

   //////////////////////////////////////////////////////////////////
   // Fill the PCB net array
   // Count the interations for the progress bar
   maxRange = pcbFilePtr->getNetList().GetCount();
   POSITION pcbDataPos = pcbFileBlock->getDataList().GetHeadPosition();
   while (pcbDataPos)
   {
      pcbFileBlock->getDataList().GetNext(pcbDataPos);
      maxRange++;
   }

   // Set up the progress bar
   buf.Format("Gathering PCB nets...");
   progress->SetStatus(buf);
   progress->SetRange32(0, maxRange);
   progress->SetPos(0);

   maxRange = 0;
   POSITION pcbNetPos = pcbFilePtr->getNetList().GetHeadPosition();
   while (pcbNetPos)
   {
      progress->SetPos(maxRange++);

      NetStruct *pcbNet = pcbFilePtr->getNetList().GetNext(pcbNetPos);
   
      if (pcbNet->getNetName() == NET_UNUSED_PINS)
         continue;

      // Add the data to the array
      //  addNetToArray will check for the data type, graphic class and net name
      addNetToArray(&pcbNetArray, pcbNet, -1);
   } // END while (schNetPos) ...

   pcbDataPos = pcbFileBlock->getDataList().GetHeadPosition();
   while (pcbDataPos)
   {
      progress->SetPos(maxRange++);

      DataStruct *pcbData = pcbFileBlock->getDataList().GetNext(pcbDataPos);
      
      // Add the data to the array
      //  addNetToArray will check for the data type, graphic class and net name
      addNetToArray(&pcbNetArray, pcbData, -1);
   } // END while (pcbDataPos)

   return ERR_NOERROR;
}

int SchLink::addCompToArray(CSchCompArray *compArray, DataStruct *data, int sheetGeomNum)
{
   CCEtoODBDoc *docPtr = (sheetGeomNum>=0?schDocPtr:pcbDocPtr);
   CString refDesAttrib = (sheetGeomNum>=0)?schRefDesAttrib:pcbRefDesAttrib;

   // if sheetGeomNum is -1, then we are adding from a pcb file
   // if it's 0 or greater, then we are adding a from a schematic file
   if (sheetGeomNum >= 0)
   {
      // Make sure we have a component in the schematic
      if (data->getDataType() != T_INSERT) return ERR_BADTYPE;
      if (data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL) return ERR_BADTYPE;
   }
   else
   {
      // Make sure we have a component in the PCB
      if (data->getDataType() != T_INSERT)
         return ERR_BADTYPE;
      if (data->getInsert()->getInsertType() != insertTypePcbComponent &&
          data->getInsert()->getInsertType() != insertTypeTestPoint)
         return ERR_BADTYPE;
   }

   if (strlen(data->getInsert()->getRefname()) <= 0)
      return ERR_NODATA;

   // look up the "Designator" attribute
   Attrib* attrib;
   CString compareName;
   int kwIndex = docPtr->IsKeyWord(refDesAttrib , 0);

   if (data->getAttributesRef() && data->getAttributesRef()->Lookup(kwIndex, attrib))
   {
      // Make sure the attribute is a string
      if (attrib->getValueType() != VT_STRING)
         compareName = data->getInsert()->getRefname();
      else
         compareName = get_attvalue_string(docPtr, attrib);
   }
   else
      compareName = data->getInsert()->getRefname();

   if (compareName.IsEmpty())
      return ERR_NODATA;

   CString sortKey = createSortKey(compareName);

   // if we need to add the component, figure out where
   int i;
   for (i=0; i<compArray->GetCount(); i++)
   {
      SchCompData *compData = compArray->GetAt(i);

      // Do we add after this one?
      if (compData->SortKey.Compare(sortKey) > 0)
         break;
   }

   // Add the new component
   SchCompData *compData = new SchCompData();
   compData->OrigRefName = data->getInsert()->getRefname();
   compData->CompareName = compareName;
   compData->SortKey = sortKey;
   compData->data = data;
   compData->Sheet = sheetGeomNum;
   compData->surface = data->getInsert()->getMirrorFlags();

   compArray->InsertAt(i, compData);

   return i;
}

int SchLink::addNetToArray(CSchNetArray *netArray, NetStruct *net, int sheetGeomNum)
{
   if (net == NULL) return ERR_NOTANET;

   CCEtoODBDoc *docPtr = (sheetGeomNum>=0?schDocPtr:pcbDocPtr);
   FileStruct *filePtr = (sheetGeomNum>=0?schFilePtr:pcbFilePtr);
   CString netNameAttrib = (sheetGeomNum>=0?schNetNameAttrib:pcbNetNameAttrib);

   if (!(net && net->getCompPinCount()))
      return ERR_NODATA;

   // look up the "Netname" attribute
   LPVOID voidPtr = NULL;
   CString netName = net->getNetName(), compareName;
   if (netName.IsEmpty())
      return ERR_NODATA;
   
   compareName = ApplyFilters(netName, FALSE, (sheetGeomNum>=0));

   CString sortKey = createSortKey(compareName);

genericLogFile.WriteString("Adding Net: %s (%d) : [%s]\n", netName, sheetGeomNum, sortKey);

   // See if the net was already added
	int i=0;
   for (i=0; i<netArray->GetCount(); i++)
   {
      SchNetData *netData = netArray->GetAt(i);

      // Did we add the net yet?  If so, we're done.
      if (netData->CompareName == compareName)
         return i;
   }

   // If we need to add the net, figure out where
   for (i=0; i<netArray->GetCount(); i++)
   {
      SchNetData *netData = netArray->GetAt(i);

      // Do we add after this one?
      if (netData->SortKey.Compare(sortKey) > 0)
         break;
   }

   // Add the new net
   SchNetData *netData = new SchNetData();
   netData->OrigNetName = netName;
   netData->CompareName = compareName;
   netData->SortKey = sortKey;
   netData->SetNet(net, (sheetGeomNum<0), docPtr);
   netData->Sheet = -1; // We need to find etches that exist on sheets in the addNetToArray() that take DataStruct

   netArray->InsertAt(i, netData);

   return i;
}

int SchLink::addNetToArray(CSchNetArray *netArray, DataStruct *data, int sheetGeomNum)
{
   // Make sure we have a component in the schematic
   if (data->getDataType() != T_POLY) return ERR_BADTYPE;
   if (data->getGraphicClass() != graphicClassSignal) return ERR_BADTYPE;

   CCEtoODBDoc *docPtr = (sheetGeomNum>=0?schDocPtr:pcbDocPtr);
   FileStruct *filePtr = (sheetGeomNum>=0?schFilePtr:pcbFilePtr);
   CString netNameAttrib = (sheetGeomNum>=0?schNetNameAttrib:pcbNetNameAttrib);

   // look up the "Netname" attribute
   Attrib* attrib;
   CString netName, compareName;
   int kwIndex = docPtr->IsKeyWord(netNameAttrib , 0);

   if (data->getAttributesRef() && data->getAttributesRef()->Lookup(kwIndex, attrib))
   {
      // Make sure the attribute is a string
      if (attrib->getValueType() == VT_STRING)
         netName = get_attvalue_string(docPtr, attrib);
      else
         return ERR_NOTANET;
   }
   else
      return ERR_NOTANET;

   if (netName.IsEmpty())
      return ERR_NODATA;
   
   compareName = ApplyFilters(netName, FALSE, (sheetGeomNum>=0));

   CString sortKey = createSortKey(compareName);

genericLogFile.WriteString("Adding Etch: %s (%d) : [%s]\n", netName, sheetGeomNum, sortKey);

   // See if the net was already added
	int i=0;
   for (i=0; i<netArray->GetCount(); i++)
   {
      SchNetData *netData = netArray->GetAt(i);

      // Find the net we're trying to add
      if (netData->CompareName == compareName)
      {
         // if we are dealing with etches from a schematic
         if (sheetGeomNum>=0)
         {
            if (sheetGeomNum == netData->Sheet && netData->data == NULL)
            {
               // If the etch hasn't been set and the  data is on the same sheet and there is no data yet
               netData->data = data;
               return i;
            }
            else if (netData->Sheet < 0)
            {
               // If the etch data hasn't been set, set it and we're done.
               netData->data = data;
               netData->Sheet = sheetGeomNum;
               return i;
            }
         }
         else
         {
            if (netData->data == NULL)
               netData->data = data;
            return i;
         }
      }
   }

   // If we need to add the net, figure out where
   for (i=0; i<netArray->GetCount(); i++)
   {
      SchNetData *netData = netArray->GetAt(i);

     // Do we insert here?
      if (netData->SortKey.Compare(sortKey) > 0)
         break;
   }

   NetStruct *net = FindNet(filePtr, netName);
   if (net == NULL || net->getCompPinCount() == 0)
      return ERR_NODATA;

   // Add the new net
   SchNetData *netData = new SchNetData();
   netData->OrigNetName = netName;
   netData->CompareName = compareName;
   netData->SortKey = sortKey;
   netData->data = data;
   netData->SetNet(net, (sheetGeomNum<0), docPtr);
   netData->Sheet = sheetGeomNum;

   netArray->InsertAt(i, netData);

   return i;
}

CString SchLink::createSortKey(CString name)
{
   // create sort name
   CString temp, buf;
   bool alpha = true, lastAlpha = true, newSection = true;

   name.MakeUpper();
   for (int i=0; i<name.GetLength(); i++)
   {
      if (isalpha(name[i]))
         alpha = true;
      else if (isdigit(name[i]))
         alpha = false;
      else
      {
         if (lastAlpha)
         {
            // add string
            temp = (CString)"AAAAAAAAAAAA" + temp;
            temp.Delete(0, temp.GetLength() - 12);
            buf += temp;
         }
         else
         {
            // add number
            temp = (CString)"000000000000" + temp;
            temp.Delete(0, temp.GetLength() - 12);
            buf += temp;
         }
         temp.Empty();

         buf += name[i];
         lastAlpha = alpha;

         newSection = true;
         continue;
      }

      if (newSection)
      {
         lastAlpha = alpha;
         temp = name[i];

         newSection = false;
         continue;
      }

      if (lastAlpha != alpha)
      {
         if (lastAlpha)
         {
            // add string
            temp = (CString)"AAAAAAAAAAAA" + temp;
            temp.Delete(0, temp.GetLength() - 12);
            buf += temp;
         }
         else
         {
            // add number
            temp = (CString)"000000000000" + temp;
            temp.Delete(0, temp.GetLength() - 12);
            buf += temp;
         }

         temp.Empty();
      }
      temp += name[i];
      lastAlpha = alpha;
   }

   if (lastAlpha)
   {
      // add string
      temp = (CString)"AAAAAAAAAAAA" + temp;
      temp.Delete(0, temp.GetLength() - 12);
      buf += temp;
   }
   else
   {
      // add number
      temp = (CString)"000000000000" + temp;
      temp.Delete(0, temp.GetLength() - 12);
      buf += temp;
   }

   return buf;
}

int SchLink::SetDocuments(CCEtoODBDoc *pcbDoc, CCEtoODBDoc *schDoc)
{
   errMsg.Empty();

   pcbDoc->schLinkPCBDoc = TRUE;
   schDoc->schLinkPCBDoc = FALSE;

   FileStruct *visPcbFile = NULL, *visSchFile = NULL;

   { // Make sure we have only the correct, single file is showing ///////
      int fileVisibleCnt = 0;

      // Make sure the schematic document has only one file showing
      POSITION filePos = schDoc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *file = schDoc->getFileList().GetNext(filePos);
         if (file->isShown() && file->getBlockType() == BLOCKTYPE_SHEET)
         {
            visSchFile = file;
            fileVisibleCnt++;
         }
      }

      if (fileVisibleCnt != 1)
      {
         // there are no comparison results
         errMsg = SCHLINK_ERR + 
                  "More than one or no View is currently visible.";
         return ERR_GENERAL;
      }

      // Make sure the PCB document has only one file showing
      fileVisibleCnt = 0;
      filePos = pcbDoc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *file = pcbDoc->getFileList().GetNext(filePos);
         if (file->isShown() && file->getBlockType() == BLOCKTYPE_PCB)
         {
            visPcbFile = file;
            fileVisibleCnt++;
         }
      }

      if (fileVisibleCnt != 1)
      {
         // there are not comparison results
         errMsg = SCHLINK_ERR + 
                  "More than one or no board is currently visible.";
         return ERR_GENERAL;
      }
   } /////////////////////////////////////////////////////////////////////////

   { // Make sure we have components and/or nets to compare with /////////////
      BOOL havePcbComps = FALSE, haveSchComps = FALSE;
      BOOL havePcbNets = FALSE, haveSchNets = FALSE;

      // Check for components in the PCB
      for (int i=0; (i<pcbDoc->getMaxBlockIndex() && !havePcbComps); i++)
      {
         BlockStruct *block = pcbDoc->getBlockAt(i);

         if (!block) 
            continue;

         if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
            havePcbComps = TRUE;
      }

      // Check for components in the Schematic
      for (int i=0; (i<schDoc->getMaxBlockIndex() && !haveSchComps); i++)
      {
         BlockStruct *block = schDoc->getBlockAt(i);

         if (!block) 
            continue;

         if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
            haveSchComps = TRUE;
      }

      // Check for nets in the PCB
      havePcbNets = (visPcbFile->getNetList().GetCount() > 0);

      // Check for nets in the Schematic
      haveSchNets = (visSchFile->getNetList().GetCount() > 0);

      // there is nothing to compare
      if (!(havePcbComps && haveSchComps) && !(havePcbNets && haveSchNets))
      {
         errMsg = SCHLINK_ERR + 
                  "There are no components and nets to compare.";
         return ERR_GENERAL;
      }
   } /////////////////////////////////////////////////////////////////////////

   // set the document and file pointers ///////////////////////////////////
   pcbDocPtr = pcbDoc;
   pcbFilePtr = visPcbFile;

   schDocPtr = schDoc;
   schFilePtr = visSchFile;

   // Get the schematic refdes attribute that has been set by the user
   Attrib* attrib;
   int refdesKW = schDoc->IsKeyWord(SCH_ATT_REFDES_MAP, 0);

   if (visSchFile->getBlock()->getAttributesRef() && visSchFile->getBlock()->getAttributesRef()->Lookup(refdesKW, attrib))
   {
      schRefDesAttrib = get_attvalue_string(schDoc, attrib);
   }

   if (fillArrays() != ERR_NOERROR)
   {
      ClearAllData();
      return ERR_NODATA;
   }

   mPcbCompArray = &pcbCompArray;
   mSchCompArray = &schCompArray;
   mPcbNetArray = &pcbNetArray;
   mSchNetArray = &schNetArray;

   return ERR_NOERROR;
}

bool SchLink::ApplyXrefNameMatches()
{
   // Always create a new one (seems each process is responsible for creating and destroying
   progress = new CProgressDlg("Schematic Link");
   progress->Create(AfxGetMainWnd());

   progress->ShowWindow(SW_SHOW);
   progress->SetStep(1);

   long compMatches = applyCompXrefNameMatches();
   long netMatches = applyNetXrefNameMatches();

   progress->ShowWindow(SW_HIDE);
   if (progress->DestroyWindow())
   {
      delete progress;
      progress = NULL;
   }

   if (compMatches > 0 && netMatches > 0)
      return true;
   else
      return false;
}

int SchLink::processNetXRef()
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
   {
      errMsg = "Major error during initialization.";
      return ERR_GENERAL;
   }

   if (pcbNetArray.IsEmpty() || schNetArray.IsEmpty()) 
   {
      errMsg = "No nets to compare.";
      return ERR_CANTCOMPARE;
   }

   progress->SetStatus("Applying Cross Reference...");
   progress->SetRange32(0, NetXRefArray[SCH_XREF].GetCount());
   progress->SetPos(0);

   for (int xrefCnt=0; xrefCnt<NetXRefArray[SCH_XREF].GetCount(); xrefCnt++)
   {
      progress->StepIt();

      CString schName = NetXRefArray[SCH_XREF].GetAt(xrefCnt),
              pcbName = NetXRefArray[PCB_XREF].GetAt(xrefCnt);

      // Nothing to look for if there isn't a valid cross reference
      if (schName.IsEmpty() || pcbName.IsEmpty())
         continue;

      // Find the PCB that will match with all instances of a schematic item
      SchNetData *pcbNetData;
      long pcbIndex = -1;
      for (long pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
      {
         pcbNetData = pcbNetArray[pcbCnt];

         // Compare with OrigRefName because that is the rule using cross reference files
         if (caseSensitive)
         {
            if (!pcbNetData->OrigNetName.Compare(pcbName))
            {
               pcbIndex = pcbCnt;
               break;
            }
         }
         else
         {
            if (!pcbNetData->OrigNetName.CompareNoCase(pcbName))
            {
               pcbIndex = pcbCnt;
               break;
            }
         }
      }

      // Make sure we found a pcb item
      if (pcbIndex < 0)
         continue;

      // Find the all schematic instances and set to match with the pcb item
      BOOL firstAssign = TRUE;
      for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
      {
         SchNetData *schNetData = schNetArray[schCnt];

         // Compare with OrigRefName because that is the rule using cross reference files
         if (caseSensitive)
         {
            if (!schNetData->OrigNetName.Compare(schName))
            {
               // If this is the first match found, make sure the PCB matches with it
               if (firstAssign)
               {
                  pcbNetData->indexMatch = schCnt;
                  firstAssign = FALSE;
               }
               schNetData->indexMatch = pcbIndex;
               schNetData->xrfMatch = TRUE;
            }
         }
         else
         {
            if (!schNetData->OrigNetName.CompareNoCase(schName))
            {
               // If this is the first match found, make sure the PCB matches with it
               if (firstAssign)
               {
                  pcbNetData->indexMatch = schCnt;
                  firstAssign = FALSE;
               }
               schNetData->indexMatch = pcbIndex;
               schNetData->xrfMatch = TRUE;
            }
         }
      }
   }

   return ERR_NOERROR;
}

int SchLink::processCompXRef()
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
   {
      errMsg = "Major error during initialization.";
      return ERR_GENERAL;
   }

   if (pcbCompArray.IsEmpty() || schCompArray.IsEmpty()) 
   {
      errMsg = "No components to compare.";
      return ERR_CANTCOMPARE;
   }

   progress->SetStatus("Applying Cross Reference...");
   progress->SetRange32(0, CompXRefArray[SCH_XREF].GetCount());
   progress->SetPos(0);

   for (int xrefCnt=0; xrefCnt<CompXRefArray[SCH_XREF].GetCount(); xrefCnt++)
   {
      progress->StepIt();

      CString schName = CompXRefArray[SCH_XREF].GetAt(xrefCnt),
              pcbName = CompXRefArray[PCB_XREF].GetAt(xrefCnt);

      // Nothing to look for if there isn't a valid cross reference
      if (schName.IsEmpty() || pcbName.IsEmpty())
         continue;

      // Find the PCB that will match with all instances of a schematic item
      SchCompData *pcbCompData;
      long pcbIndex = -1;
      for (int pcbCnt=0; pcbCnt<pcbCompArray.GetCount(); pcbCnt++)
      {
         pcbCompData = pcbCompArray[pcbCnt];

         // Compare with OrigRefName because that is the rule using cross reference files
         if (caseSensitive)
         {
            if (!pcbCompData->OrigRefName.Compare(pcbName))
            {
               pcbIndex = pcbCnt;
               break;
            }
         }
         else
         {
            if (!pcbCompData->OrigRefName.CompareNoCase(pcbName))
            {
               pcbIndex = pcbCnt;
               break;
            }
         }
      }

      // Make sure we found a pcb item
      if (pcbIndex < 0)
         continue;

      // Find the all schematic instances and set to match with the pcb item
      BOOL firstAssign = TRUE;
      for (int schCnt=0; schCnt<schCompArray.GetCount(); schCnt++)
      {
         SchCompData *schCompData = schCompArray[schCnt];

         // Compare with OrigRefName because that is the rule using cross reference files
         if (caseSensitive)
         {
            if (!schCompData->CompareName.Compare(schName))
            {
               // If this is the first match found, make sure the PCB matches with it
               if (firstAssign)
               {
                  pcbCompData->indexMatch = schCnt;
                  firstAssign = FALSE;
               }
               schCompData->indexMatch = pcbIndex;
               schCompData->xrfMatch = TRUE;
            }
         }
         else
         {
            if (!schCompData->CompareName.CompareNoCase(schName))
            {
               // If this is the first match found, make sure the PCB matches with it
               if (firstAssign)
               {
                  pcbCompData->indexMatch = schCnt;
                  firstAssign = FALSE;
               }
               schCompData->indexMatch = pcbIndex;
               schCompData->xrfMatch = TRUE;
            }
         }
      }
   }

   return ERR_NOERROR;
}

SchCompData *SchLink::findSchComp(CString findString, CString option)
{
   int minIndex, maxIndex, centerIndex;

   minIndex = 0;
   maxIndex = schCompArray.GetCount();
   centerIndex = (minIndex + maxIndex) / 2;

   // TODO: Edit for possible speed enhancements
   BOOL foundItem = FALSE;
   WORD foundIndex = -1;
   if (option == FINDOPT_ORIGINALNAME)
      foundItem = m_schCompMapRefName.Lookup(findString, foundIndex);
   else if (option == FINDOPT_COMPARENAME)
      foundItem = m_schCompMapCompareName.Lookup(findString, foundIndex);

   if (!foundItem)
      return NULL;
   
   SchCompData *schData = schCompArray[foundIndex];
   return schData;
}

SchCompData *SchLink::findPcbComp(CString findString, CString option)
{
   int minIndex, maxIndex, centerIndex;

   minIndex = 0;
   maxIndex = schCompArray.GetCount();
   centerIndex = (minIndex + maxIndex) / 2;

   // TODO: Edit for possible speed enhancements

   for (int schCnt=0; schCnt<pcbCompArray.GetCount(); schCnt++)
   {
      SchCompData *schData = schCompArray[schCnt];
      BOOL foundItem = FALSE;

      if (option == FINDOPT_ORIGINALNAME)
         foundItem = (schData->OrigRefName == findString);
      else if (option == FINDOPT_COMPARENAME)
         foundItem = (schData->CompareName == findString);
      
      if (foundItem)
      {
         return schData;
         //// See if there is a match
         //if (schData->indexMatch >= 0)
         //   // Get the name for the PCB component that matches the schematic component
         //   return pcbCompArray[schData->indexMatch];
      }
   }
   return NULL;
}

void SchLink::applyCompMatches()
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
      return;

   if (pcbCompArray.IsEmpty() || schCompArray.IsEmpty() || pcbNetArray.IsEmpty() || schNetArray.IsEmpty()) 
      return;

   // Reset the component matches from the comp/pins
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData *netData = schNetArray[schCnt];
      netData->ClearCompPins();
   }

   progress->SetStatus("Applying Component Matches to Netlist...");
   progress->SetRange32(0, schNetArray.GetCount());
   progress->SetPos(0);

   // Loop through all the nets and apply the component matches to each comp/pin
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      progress->StepIt();

      SchNetData *netData = schNetArray[schCnt];

      netData->ApplyCompMatches(FALSE, schDocPtr, this);
   }
}

long SchLink::applyCompXrefNameMatches()
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
      return 0;

   if (pcbCompArray.IsEmpty() || schCompArray.IsEmpty()) 
      return 0;

   int xrefKW = schDocPtr->IsKeyWord(SCHLINK_PCB_REFDES, 0);
   if (xrefKW < 0)
      return 0;

   long matchCnt = 0;

   progress->SetStatus("Applying saved Component XREF_NAME attribute data...");
   progress->SetRange32(0, schCompArray.GetCount());
   progress->SetPos(0);

   // Loop through all the components and apply the component matches based on SCHLINK_PCB_REFDES
   for (int schCnt=0; schCnt<schCompArray.GetCount(); schCnt++)
   {
      progress->StepIt();

      SchCompData *schData = schCompArray[schCnt];

      Attrib* attrib;
      CString xrefName;

      if (schData->data->getAttributesRef()->Lookup(xrefKW, attrib))
      {
         if (attrib->getValueType() == VT_STRING)
            xrefName = get_attvalue_string(schDocPtr, attrib);
      }

      if (xrefName.IsEmpty())
         continue;

      for (int pcbCnt=0; pcbCnt<pcbCompArray.GetCount(); pcbCnt++)
      {
         SchCompData *pcbData = pcbCompArray[pcbCnt];

         if (pcbData->CompareName == xrefName)
         {
            // Make sure we don't override a match
            if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
            if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

            matchCnt++;

            break;
         }
      }
   }

   return matchCnt;
}

long SchLink::applyNetXrefNameMatches()
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
      return 0;

   if (pcbNetArray.IsEmpty() || schNetArray.IsEmpty()) 
      return 0;

   int xrefKW = schDocPtr->IsKeyWord(SCHLINK_PCB_NETNAME, 0);
   if (xrefKW < 0)
      return 0;

   long matchCnt = 0;

   progress->SetStatus("Applying saved Net XREF_NAME attribute data...");
   progress->SetRange32(0, schCompArray.GetCount());
   progress->SetPos(0);

   defaultFirstNetCompare = NETCOMPTYPE_CONTENT_PCBASMASTER;

   // Loop through all the nets and apply the component matches based on SCHLINK_PCB_NETNAME
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      progress->StepIt();

      SchNetData *schData = schNetArray[schCnt];

      Attrib* attrib;
      CString xrefName;

      //if (schData-data->getAttributesRef() != NULL && schData-data->getAttributesRef()->Lookup(xrefKW, voidPtr))
      if (schData->net->getAttributesRef() != NULL && schData->net->getAttributesRef()->Lookup(xrefKW, attrib))
      {
         if (attrib->getValueType() == VT_STRING)
            xrefName = get_attvalue_string(schDocPtr, attrib);
      }

      if (xrefName.IsEmpty())
         continue;

      for (int pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
      {
         SchNetData *pcbData = pcbNetArray[pcbCnt];

         if (pcbData->CompareName == xrefName)
         {
            // Make sure we don't override a match
            if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
            if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

            defaultFirstNetCompare = NETCOMPTYPE_NAME;

            matchCnt++;

            break;
         }
      }
   }

   return matchCnt;
}

BOOL SchLink::compareNet(SchNetData *schData, SchNetData *pcbData)
{
   NetStruct *schNet = schData->net;
   NetStruct *pcbNet = pcbData->net;
   int lNetCompType = netCompType;

   if (!schNet || !pcbNet)
      lNetCompType = NETCOMPTYPE_NAME;

   switch (lNetCompType)
   {
   case NETCOMPTYPE_NAME:
      {
         // Apply all filters before the comparison
         CString schCompareName = ApplyFilters(schData->CompareName, FALSE, TRUE);
         CString pcbCompareName = ApplyFilters(pcbData->CompareName, FALSE, FALSE);

         if (caseSensitive)
            return (!pcbCompareName.Compare(schCompareName));
         else
            return (!pcbCompareName.CompareNoCase(schCompareName));
      }
      break;
   case NETCOMPTYPE_CONTENT:
      {
         int totPossibleMatches = max((schData->GetPossibleMatches()), pcbData->GetPossibleMatches());
         int matchCnt = 0;
         int matchesNeeded = (int)ceil(((float)minPercentMatch * (float)totPossibleMatches)/100.0);

         if (totPossibleMatches <= 0)
            return FALSE;

         // if we need more matches than either one of the nets Comp/Pin count, then they'll never match
         //  Not enough pins to be a sufficient match
         if (matchesNeeded > schData->cpArray.GetCount() || matchesNeeded > pcbData->cpArray.GetCount())
            return FALSE;

         int schCnt = 0, pcbCnt = 0;
         while (schCnt < schData->cpArray.GetCount() && pcbCnt < pcbData->cpArray.GetCount())
         {
            // if the number still needed is greater than what's left in either of the nets,
            // there won't be enough left to constitute a match
            int stillNeeded = matchesNeeded - matchCnt;
            int remainingSchPins = schData->cpArray.GetCount() - schCnt;
            int remainingPcbPins = pcbData->cpArray.GetCount() - pcbCnt;
            if (stillNeeded > remainingSchPins || stillNeeded > remainingPcbPins)
               return FALSE;

            SchCompPinData *schCPData = schData->cpArray[schCnt];
            SchCompPinData *pcbCPData = pcbData->cpArray[pcbCnt];

            // Get the component and pin names
            CString schCompName = schCPData->matchedCompName,
                    schPinName = schCPData->PinName,
                    pcbCompName = pcbCPData->matchedCompName,
                    pcbPinName = pcbCPData->PinName;

            // If there isn't anything to compare, then move on
            if (schCompName == "NULL" || pcbCompName == "NULL")
            {
               schCnt += ((schCompName == "NULL")?1:0);
               pcbCnt += ((pcbCompName == "NULL")?1:0);
               continue;
            }

            int compRet = 0;
            if (caseSensitive)
            {
               compRet = pcbCPData->Compare(schCompName, schPinName);
               if (!compRet)
               {
                  if (++matchCnt >= matchesNeeded)
                     return TRUE;
               }
            }
            else
            {
               compRet = pcbCPData->CompareNoCase(schCompName, schPinName);
               if (!compRet)
               {
                  if (++matchCnt >= matchesNeeded)
                     return TRUE;
               }
            }

            // Move onto the next item(s)
            if (compRet > 0)
            {
               schCnt++;
            }
            else if (compRet < 0)
            {
               pcbCnt++;
            }
            else // (!compRet)
            {
               pcbCnt++;
               schCnt++;
            }
         }
      }
      break;
   case NETCOMPTYPE_CONTENT_PCBASMASTER:
      {
         int schUnmatches = schData->GetUnmatchedCount();
         int matchCnt = 0;
         int schMatchCnt = 0;
         int pcbMatchCnt = 0;
         int schCnt = 0, pcbCnt = 0;
         while (schCnt < schData->cpArray.GetCount() && pcbCnt < pcbData->cpArray.GetCount())
         {
            SchCompPinData *schCPData = schData->cpArray[schCnt];
            SchCompPinData *pcbCPData = pcbData->cpArray[pcbCnt];

            // Get the component and pin names
            CString schCompName = schCPData->matchedCompName,
                    schPinName = schCPData->PinName,
                    pcbCompName = pcbCPData->matchedCompName,
                    pcbPinName = pcbCPData->PinName;

            // If there isn't anything to compare, then move on
            if (schCompName == "NULL" || pcbCompName == "NULL")
            {
               schCnt += ((schCompName == "NULL")?1:0);
               pcbCnt += ((pcbCompName == "NULL")?1:0);
               continue;
            }

            int compRet = 0;
            if (caseSensitive)
            {
               compRet = pcbCPData->Compare(schCompName, schPinName);
            }
            else
            {
               compRet = pcbCPData->CompareNoCase(schCompName, schPinName);
            }

            if (!compRet)
            {
               if (!pcbCPData->isMatched())
               {
                  pcbCPData->setMatched();
                  pcbMatchCnt++;
               }

               if (!schCPData->isMatched())
               {
                  schCPData->setMatched();
                  schMatchCnt++;
               }

               matchCnt++;
            }

            // Move onto the next item(s)
            if (compRet > 0)
            {
               schCnt++;
            }
            else if (compRet < 0)
            {
               pcbCnt++;
            }
            else // (!compRet)
            {
               pcbCnt++;
               schCnt++;
            }
         }

         if (matchCnt == schUnmatches)
         {
            // Only consider match if all comppins in Schematic net are matched
            schData->AddMatchedCount(schMatchCnt);
            pcbData->AddMatchedCount(pcbMatchCnt);
            return TRUE;
         }
      }
      break;
   }

   return FALSE;
}

int SchLink::compareNetsByName()
{
   int schCnt = 0, pcbCnt = 0;
   progress->SetStatus("Comparing Nets by Name...");
   progress->SetRange32(0, schNetArray.GetCount() + pcbNetArray.GetCount());
   while (schCnt < schNetArray.GetCount() && pcbCnt < pcbNetArray.GetCount())
   {
      // Update the progress bar
      progress->StepIt();

      SchNetData *schData = schNetArray[schCnt];
      SchNetData *pcbData = pcbNetArray[pcbCnt];

      if (schData->indexMatch >= 0)
      {
         // This one has a match already, so move onto the next schematic item
         schCnt++;
         continue;
      }

      if (compareNet(schData, pcbData))
      {
         // Make sure we don't override a match
         if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
         if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

         schCnt++;
         continue;
      }

      // Move onto the next item(s)
      int compRet = schData->SortKey.Compare(pcbData->SortKey);
      if (compRet > 0)
         pcbCnt++;
      else // (compRet <= 0)
         schCnt++;
   }

   return 0;
}

int SchLink::compareNetsByContent()
{
   //int schCnt = 0, pcbCnt = 0;
   //progress->SetStatus("Comparing Nets by Content...");
   //progress->SetRange32(0, schNetArray.GetCount() * pcbNetArray.GetCount());

   //for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   //{
   //   SchNetData *schData = schNetArray[schCnt];

   //   if (schData->indexMatch >= 0)
   //   {
   //      // Update the progress bar for the portion that we will skip
   //      progress->OffsetPos(pcbNetArray.GetCount());
   //      continue;
   //   }

   //   // See if there is a matching component in the PCB
   //   for (int pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
   //   {
   //      progress->StepIt();
   //      SchNetData *pcbData = pcbNetArray[pcbCnt];

   //      //if (pcbData->indexMatch >= 0)
   //      //   continue;

   //      if (compareNet(schData, pcbData))
   //      {
   //         // Make sure we don't override a match
   //         if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
   //         if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

   //         // Update the progress bar for the portion that we will skip
   //         progress->OffsetPos(pcbNetArray.GetCount() - pcbCnt - 1);

   //         // if match is found, move on to the next item in the schematic because
   //         // we know that there can only be one PCB item to associate with any number
   //         // of schematic items (but never more than one)
   //         break;
   //      }
   //   }
   //}


   progress->SetRange32(0, schNetArray.GetCount()+pcbNetArray.GetCount()+(schNetArray.GetCount()*pcbNetArray.GetCount()));

   int nameMatches = 0, contentMatches = 0;

   // pass 1:
   //  Name content matching to match nets that share the same content only if they
   // have the same name.
   int schCnt = 0, pcbCnt = 0;
   progress->SetStatus("Comparing Nets by Content (Names)...");
   while (schCnt < schNetArray.GetCount() && pcbCnt < pcbNetArray.GetCount())
   {
      // Update the progress bar
      progress->StepIt();

      SchNetData *schData = schNetArray[schCnt];
      SchNetData *pcbData = pcbNetArray[pcbCnt];

      if (schData->indexMatch >= 0)
      {
         // This one has a match already, so move onto the next schematic item
         schCnt++;
         continue;
      }

      netCompType = NETCOMPTYPE_NAME;
      if (compareNet(schData, pcbData))
      {
         nameMatches++;

         netCompType = NETCOMPTYPE_CONTENT;
         if (compareNet(schData, pcbData))
         {
            contentMatches++;

            // Make sure we don't override a match
            if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
            if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

            schCnt++;
            continue;
         }
      }

      // Move onto the next item(s)
      int compRet = schData->SortKey.Compare(pcbData->SortKey);
      if (compRet > 0)
         pcbCnt++;
      else // (compRet <= 0)
         schCnt++;
   }

   // pass 2:
   //  Strictly content matching on remaining unmatched nets.
   netCompType = NETCOMPTYPE_CONTENT;
   progress->SetStatus("Comparing Nets by Content (Contents)...");
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData *schData = schNetArray[schCnt];

      if (schData->indexMatch >= 0)
      {
         // Update the progress bar for the portion that we will skip
         progress->OffsetPos(pcbNetArray.GetCount());
         continue;
      }

      // See if there is a matching component in the PCB
      for (int pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
      {
         progress->StepIt();
         SchNetData *pcbData = pcbNetArray[pcbCnt];

         //if (pcbData->indexMatch >= 0)
         //   continue;

         if (compareNet(schData, pcbData))
         {
            // Make sure we don't override a match
            if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
            if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

            // Update the progress bar for the portion that we will skip
            progress->OffsetPos(pcbNetArray.GetCount() - pcbCnt - 1);

            // if match is found, move on to the next item in the schematic because
            // we know that there can only be one PCB item to associate with any number
            // of schematic items (but never more than one)
            break;
         }
      }
   }

   return 0;
}

int SchLink::compareNetsByContentUsingPcbAsMaster()
{
   progress->SetRange32(0, schNetArray.GetCount()+pcbNetArray.GetCount()+(schNetArray.GetCount()*pcbNetArray.GetCount()));

   int matchedCount = 0;
   int noMatchedCount = 0;

   netCompType = NETCOMPTYPE_CONTENT_PCBASMASTER;
   progress->SetStatus("Comparing Nets by Content (Using PCB as Master)...");
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData *schNetData = schNetArray[schCnt];
      if (schNetData->indexMatch >= 0)
      {
         // Update the progress bar for the portion that we will skip
         progress->OffsetPos(pcbNetArray.GetCount());
         continue;
      }

      // See if there is a matching component in the PCB
      bool matchedFound = false;
      for (int pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
      {
         progress->StepIt();
         SchNetData *pcbNetData = pcbNetArray[pcbCnt];
         
         if (compareNet(schNetData, pcbNetData))
         {
            // Make sure we don't override a match
            if (schNetData->indexMatch < 0) schNetData->indexMatch = pcbCnt;
            if (pcbNetData->indexMatch < 0) pcbNetData->indexMatch = schCnt;

            // Update the progress bar for the portion that we will skip
            progress->OffsetPos(pcbNetArray.GetCount() - pcbCnt - 1);
      
            matchedFound = true;

            // if match is found, move on to the next item in the schematic because
            // we know that there can only be one PCB item to associate with any number
            // of schematic items (but never more than one)
            break;
         }
      }

      if (matchedFound)
         matchedCount++;
      else
         noMatchedCount++;
   }

   return 0;
}

int SchLink::CompareNets(BOOL firstTime)
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
   {
      errMsg = "Major error during initialization.";
      return ERR_GENERAL;
   }

   if (pcbNetArray.IsEmpty() || schNetArray.IsEmpty()) 
   {
      errMsg.Empty();
      if (pcbNetArray.IsEmpty())
         errMsg = "No nets in the PCB to compare.";
      if (schNetArray.IsEmpty())
         errMsg = "No nets in the schematic to compare.";
      return ERR_CANTCOMPARE;
   }

   progress = new CProgressDlg("Schematic Link");
   progress->Create(AfxGetMainWnd());
   progress->SetStep(1);

   bool needToCompare = true;
   if (firstTime)
      needToCompare = (applyNetXrefNameMatches() <= 0);

   processNetXRef();

   int retVal = ERR_NOERROR;
   int oldNetCompType = netCompType;
   if (firstTime) netCompType =  defaultFirstNetCompare;
   if (needToCompare)
   {
      switch (netCompType)
      {
      case NETCOMPTYPE_NAME:
         // See if we need to reload everything first
         //if (filterSettings.FileReloaded)
         //   fillNetArrays();
         retVal = compareNetsByName();
         //filterSettings.FileReloaded = FALSE;
         break;
      case NETCOMPTYPE_CONTENT:
         applyCompMatches();
         retVal = compareNetsByContent();
         break;
      case NETCOMPTYPE_CONTENT_PCBASMASTER:
         applyCompMatches();
         retVal = compareNetsByContentUsingPcbAsMaster();
         break;
      }
   }
   netCompType = oldNetCompType;

   if (progress->DestroyWindow())
   {
      delete progress;
      progress = NULL;
   }
#ifdef _LOGRESULTS_
CString buf;
CStdioFile file;
if (file.Open("C:\\Development\\CAMCAD\\schNetResults.txt", CFile::modeCreate|CFile::modeWrite))
{
   POSITION sheetPos = sheetList.GetHeadPosition();
   while (sheetPos)
   {
      int sheetGeomNum = sheetList.GetNext(sheetPos);
      BlockStruct *schFileBlock = schDocPtr->getBlockAt(sheetGeomNum);

      buf.Format("%d - %s\n", sheetGeomNum, schFileBlock->getName());
      file.WriteString(buf);
   }
   file.WriteString("\n");
   file.WriteString("\n");

   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData *schData = schNetArray[schCnt];
      SchNetData *pcbData = NULL;

      CString sheetNumber;
      sheetNumber.Format("%d", schData->Sheet);
      buf = schData->OrigNetName + "/" + schData->CompareName + " (" + sheetNumber + "),";
      if (schData->indexMatch >= 0)
      {
         pcbData = pcbNetArray[schData->indexMatch];
         sheetNumber.Format("%d", pcbData->Sheet);
         buf += pcbData->OrigNetName + "/" + pcbData->CompareName + " (" + sheetNumber + ")\n";
      }
      else
         buf += "Nothing\n";

      int cnt = 0;
      while (cnt < schData->cpArray.GetCount() &&
         (pcbData?(cnt < pcbData->cpArray.GetCount()):TRUE))
      {
         SchCompPinData *schCPData = schData->cpArray[cnt];
         SchCompPinData *pcbCPData = (pcbData?pcbData->cpArray[cnt]:NULL);
         buf += (CString)"   " + schCPData->CompName + "[" + schCPData->matchedCompName + "]/" + schCPData->PinName +
            (pcbCPData?"   " + pcbCPData->CompName + "[" + pcbCPData->matchedCompName + "]/" + pcbCPData->PinName:"") + "\n";

         cnt++;
      }

      file.WriteString(buf);
   }
   file.Close();
}
if (file.Open("C:\\Development\\CAMCAD\\pcbNetResults.txt", CFile::modeCreate|CFile::modeWrite))
{
   for (int pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
   {
      SchNetData *pcbData = pcbNetArray[pcbCnt];
      SchNetData *schData = NULL;

      buf = pcbData->OrigNetName + "/" + pcbData->CompareName + ",";
      if (pcbData->indexMatch >= 0)
      {
         schData = schNetArray[pcbData->indexMatch];
         buf += schData->OrigNetName + "/" + schData->CompareName + "\n";
      }
      else
         buf += "Nothing\n";

      int cnt = 0;
      while (cnt < pcbData->cpArray.GetCount() &&
         (schData?(cnt < schData->cpArray.GetCount()):TRUE))
      {
         SchCompPinData *pcbCPData = pcbData->cpArray[cnt];
         SchCompPinData *schCPData = (schData?schData->cpArray[cnt]:NULL);
         buf += (CString)"   " + pcbCPData->CompName + "[" + pcbCPData->matchedCompName + "]/" + pcbCPData->PinName +
            (schCPData?"   " + schCPData->CompName + "[" + schCPData->matchedCompName + "]/" + schCPData->PinName:"") + "\n";

         cnt++;
      }

      file.WriteString(buf);
   }
   file.Close();
}
#endif
   return retVal;
}

BOOL SchLink::compareComp(SchCompData *schData, SchCompData *pcbData)
{
   // Apply all filters before the comparison
   CString schCompareName = ApplyFilters(schData->CompareName, TRUE, TRUE);
   CString pcbCompareName = ApplyFilters(pcbData->CompareName, TRUE, FALSE);
   if (caseSensitive)
      return (!pcbCompareName.Compare(schCompareName));
   else
      return (!pcbCompareName.CompareNoCase(schCompareName));
}

int SchLink::CompareComps(BOOL firstTime)
{
   if (!pcbDocPtr || !schDocPtr || !pcbFilePtr || !schFilePtr)
   {
      errMsg = "Major error during initialization.";
      return ERR_GENERAL;
   }

   if (pcbCompArray.IsEmpty() || schCompArray.IsEmpty()) 
   {
      errMsg.Empty();
      if (pcbCompArray.IsEmpty())
         errMsg = "No components in the PCB to compare.";
      if (schCompArray.IsEmpty())
         errMsg = "No components in the schematic to compare.";
      return ERR_CANTCOMPARE;
   }

   progress = new CProgressDlg("Schematic Link");
   progress->Create(AfxGetMainWnd());
   progress->SetStep(1);

   if (firstTime) applyCompXrefNameMatches();

   processCompXRef();

   int schCnt = 0, pcbCnt = 0;
   progress->SetStatus("Comparing Components...");
   progress->SetRange32(0, schCompArray.GetCount() + pcbCompArray.GetCount());
   progress->SetPos(0);

   while (schCnt < schCompArray.GetCount() && pcbCnt < pcbCompArray.GetCount())
   {
      // Update the progress bar
      progress->StepIt();

      SchCompData *schData = schCompArray[schCnt];
      SchCompData *pcbData = pcbCompArray[pcbCnt];

      if (schData->indexMatch >= 0)
      {
         // This one has a match already, so move onto the next schematic item
         schCnt++;
         continue;
      }

      if (compareComp(schData, pcbData))
      {
         // Make sure we don't override a match
         if (schData->indexMatch < 0) schData->indexMatch = pcbCnt;
         if (pcbData->indexMatch < 0) pcbData->indexMatch = schCnt;

         schCnt++;
         continue;
      }

      // Move onto the next item(s)
      int compRet = schData->SortKey.Compare(pcbData->SortKey);
      if (compRet > 0)
         pcbCnt++;
      else // (compRet <= 0)
         schCnt++;
   }

   if (progress->DestroyWindow())
   {
      delete progress;
      progress = NULL;
   }
#ifdef _LOGRESULTS_
CString buf;
CStdioFile file;
if (file.Open("C:\\Development\\CAMCAD\\schCompResults.txt", CFile::modeCreate|CFile::modeWrite))
{
   for (int schCnt=0; schCnt<schCompArray.GetCount(); schCnt++)
   {
      SchCompData *schData = schCompArray[schCnt];
      //DataStruct *data = schData->data;

      buf = schData->OrigRefName + "/" + schData->CompareName + ",";
      if (schData->indexMatch >= 0)
      {
         schData = pcbCompArray[schData->indexMatch];
         buf += schData->OrigRefName + "/" + schData->CompareName + "\n";
      }
      else
         buf += "Nothing\n";

      file.WriteString(buf);
   }
   file.Close();
}
if (file.Open("C:\\Development\\CAMCAD\\pcbCompResults.txt", CFile::modeCreate|CFile::modeWrite))
{
   for (int pcbCnt=0; pcbCnt<pcbCompArray.GetCount(); pcbCnt++)
   {
      SchCompData *schData = pcbCompArray[pcbCnt];
      //DataStruct *data = schData->data;

      buf = schData->OrigRefName + "/" + schData->CompareName + ",";
      if (schData->indexMatch >= 0)
      {
         schData = schCompArray[schData->indexMatch];
         buf += schData->OrigRefName + "/" + schData->CompareName + "\n";
      }
      else
         buf += "Nothing\n";

      file.WriteString(buf);
   }
   file.Close();
}
#endif
   return 0;
}

void SchLink::ResetComps()
{
   SchCompData *compData = NULL;

   // Reset the schematic component array
   for (int schCnt=0; schCnt<schCompArray.GetCount(); schCnt++)
   {
      compData = schCompArray[schCnt];
      compData->indexMatch = -1;
      compData->xrfMatch = FALSE;
   }

   // Reset the pcb component array
   for (int pcbCnt=0; pcbCnt<pcbCompArray.GetCount(); pcbCnt++)
   {
      compData = pcbCompArray[pcbCnt];
      compData->indexMatch = -1;
      compData->xrfMatch = FALSE;
   }
}

void SchLink::ResetNets()
{
   SchNetData *netData = NULL;

   // Reset the schematic component array
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      netData = schNetArray[schCnt];
      netData->indexMatch = -1;
      netData->ResetMatchedCount();
      netData->xrfMatch = FALSE;
   }

   // Reset the pcb component array
   for (int pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
   {
      netData = pcbNetArray[pcbCnt];
      netData->indexMatch = -1;
      netData->ResetMatchedCount();
      netData->xrfMatch = FALSE;
   }
}

int SchLink::LoadXRefFile(CString xRefPath, BOOL component, CStringArray *schArray, CStringArray *pcbArray)
{
   // If we have arrays to fill, leave all original xRef data
   if (!schArray || !pcbArray)
   {
      if (component)
      {
         CompXRefArray[SCH_XREF].RemoveAll();
         CompXRefArray[PCB_XREF].RemoveAll();
      }
      else
      {
         NetXRefArray[SCH_XREF].RemoveAll();
         NetXRefArray[PCB_XREF].RemoveAll();
      }
   }

   CStdioFile file;
   CFileException e;
   if (!file.Open(xRefPath, CFile::modeRead|CFile::shareDenyWrite, &e))
   {
      errMsg = (CString)"Error opening file.";  // + e.GetErrorMessage(
      return ERR_NOTFOUND;
   }

   CString buf, schName, pcbName;
   while (file.ReadString(buf))
   {
      if (buf[0] == ';') 
         continue;

      buf = buf.Trim();
      schName = buf.Mid(0, buf.FindOneOf(" \t")).Trim();
      pcbName = buf.Mid(buf.FindOneOf(" ")+1).Trim();

      if (schArray && pcbArray)
      {
         schArray->Add(schName);
         pcbArray->Add(pcbName);
      }
      else
      {
         if (component)
         {
            CompXRefArray[SCH_XREF].Add(schName);
            CompXRefArray[PCB_XREF].Add(pcbName);
         }
         else
         {
            NetXRefArray[SCH_XREF].Add(schName);
            NetXRefArray[PCB_XREF].Add(pcbName);
         }
      }
   }
   file.Close();

   return ERR_NOERROR;
}

int SchLink::LoadFilterFile(CString filterPath)
{
   //filterSettings.ClearAllData();
   //return filterSettings.ReadFile(filterPath);
   return FALSE;
}

void SchLink::ClearFilterData()
{
   //filterSettings.ClearAllData();
}

CString SchLink::GetSheetName(int index)
{
   return "";
}

void SchLink::ClearCrossProbedColors()
{
   removeHighlightedNets();
   removeOverriddenComps();
}

//void HighlightPins(CDC *pDC, CCEtoODBView *view);
void SchLink::removeHighlightedNets()
{
   if (!pcbDocPtr || !schDocPtr)
      return;

   // See if we're closing CAMCAD
   //  The docs would have been removed already
   if (completelyDestroy)
      return;

   if (!netLinkState)
      return;

   //pcbDocPtr->HighlightedNetsMap.RemoveAll();
   //pcbDocPtr->UpdateAllViews(NULL);
   //schDocPtr->HighlightedNetsMap.RemoveAll();
   //schDocPtr->UpdateAllViews(NULL);
   //return;


   if (!pcbDocPtr->HighlightedNetsMap.IsEmpty())
   {
      if (pcbDocPtr->HighlightedNetsMap.GetCount() > 1)
      {
         pcbDocPtr->HighlightedNetsMap.RemoveAll();
         pcbDocPtr->UpdateAllViews(NULL);
      }
      else
      {
         POSITION pos = pcbDocPtr->HighlightedNetsMap.GetStartPosition();
         if (pos)
         {
            int value;
            COLORREF key;
            pcbDocPtr->HighlightedNetsMap.GetNextAssoc(pos, value, key);
            pcbDocPtr->UnhighlightByAttrib(FALSE, pcbDocPtr->IsKeyWord(pcbNetNameAttrib, TRUE), VT_STRING, value);
         }
      }
   }

   if (!schDocPtr->HighlightedNetsMap.IsEmpty())
   {
      if (schDocPtr->HighlightedNetsMap.GetCount() > 1)
      {
         schDocPtr->HighlightedNetsMap.RemoveAll();
         schDocPtr->UpdateAllViews(NULL);
      }
      else
      {
         POSITION pos = schDocPtr->HighlightedNetsMap.GetStartPosition();
         if (pos)
         {
            int value;
            COLORREF key;
            schDocPtr->HighlightedNetsMap.GetNextAssoc(pos, value, key);
            schDocPtr->UnhighlightByAttrib(FALSE, schDocPtr->IsKeyWord(schNetNameAttrib, TRUE), VT_STRING, value);
         }
      }
   }
}

void SchLink::removeOverriddenComps()
{
   if (!pcbFilePtr || !schFilePtr)
      return;

   // See if we're closing CAMCAD
   //  The docs would have been removed already
   if (completelyDestroy)
      return;

   if (!compLinkState)
      return;

   BlockStruct *block = pcbFilePtr->getBlock();
   if (block)
   {
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         BOOL needRedraw = data->getColorOverride();
         data->setColorOverride(false);

         if (needRedraw)
         {
            SelectStruct s;

            s.setData(data);
            s.setParentDataList(&block->getDataList());
            s.scale = (DbUnit)pcbFilePtr->getScale();
            s.insert_x = (DbUnit)pcbFilePtr->getInsertX();
            s.insert_y = (DbUnit)pcbFilePtr->getInsertY();
            s.rotation = (DbUnit)pcbFilePtr->getRotation();
            s.mirror = pcbFilePtr->isMirrored();
            pcbDocPtr->DrawEntity(&s, 0, TRUE);
         }
      }
   }

   block = schFilePtr->getBlock();
   if (block)
   {
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         BOOL needRedraw = data->getColorOverride();
         data->setColorOverride(false);

         if (needRedraw)
         {
            SelectStruct s;

            s.setData(data);
            s.setParentDataList(&block->getDataList());
            s.scale = (DbUnit)pcbFilePtr->getScale();
            s.insert_x = (DbUnit)pcbFilePtr->getInsertX();
            s.insert_y = (DbUnit)pcbFilePtr->getInsertY();
            s.rotation = (DbUnit)pcbFilePtr->getRotation();
            s.mirror = pcbFilePtr->isMirrored();
            schDocPtr->DrawEntity(&s, 0, TRUE);
         }
      }
   }
}

int SchLink::GetMatchForSch(CString schItemName, CString &matchedName, BOOL component, BOOL zoom)
{
   // Check if we should be linking
   if (component && !compLinkState)
      return ERR_SETOFF;
   if (!component && !netLinkState)
      return ERR_SETOFF;

   if (component)
   {
      // Search the schematic's component array for the item
      for (int schCnt=0; schCnt<schCompArray.GetCount(); schCnt++)
      {
         SchCompData *compData = schCompArray[schCnt];
//         if (!schItemName.CompareNoCase(compData->OrigRefName))
         if (!schItemName.CompareNoCase(compData->CompareName))
         {
            // Make sure there's a match in the PCB
            if (compData->indexMatch < 0) return ERR_NODATA;

            compData = pcbCompArray[compData->indexMatch];
            matchedName = compData->OrigRefName;

            if (zoom)
               ZoomToComp(pcbDocPtr, compData);

            return ERR_NOERROR;
         }
      }
   }
   else
   {
      // Search the schematic's net array for the item
      for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
      {
         SchNetData *netData = schNetArray[schCnt];

         if (!schItemName.CompareNoCase(netData->OrigNetName))
         {
            // Make sure there's a match in the PCB
            if (netData->indexMatch < 0) return ERR_NODATA;

            netData = pcbNetArray[netData->indexMatch];
            matchedName = netData->OrigNetName;

            if (zoom)
               ZoomToNet(pcbDocPtr, netData);

            return ERR_NOERROR;
         }
      }
   }

   return ERR_NOTFOUND;
}

int SchLink::GetMatchForSch(DataStruct *schItem, CString &matchedName, BOOL zoom)
{
   BOOL component;
   CString schItemName;

   if (schItem->getDataType() == dataTypeInsert && schItem->getInsert()->getInsertType() == INSERTTYPE_SYMBOL)
   {
      // Check if we should be linking
      if (!compLinkState)
         return ERR_SETOFF;

      Attrib* attrib = NULL;
      CString compareName;
      int kwIndex = schDocPtr->IsKeyWord(schRefDesAttrib, 0);
      if (schItem->getAttributesRef() && schItem->getAttributesRef()->Lookup(kwIndex, attrib))
      {
         // Make sure the attribute is a string
         if (attrib->getValueType() == VT_STRING)
            compareName = get_attvalue_string(schDocPtr, attrib);
      }
      schItemName = compareName;

//      schItemName = schItem->getInsert()->getRefname();
      component = TRUE;
   }
   else if (schItem->getDataType() == dataTypePoly && schItem->getGraphicClass() == graphicClassSignal)
   {
      // Check if we should be linking
      if (!netLinkState)
         return ERR_SETOFF;

      // look up the "Netname" attribute
      Attrib* attrib = NULL;
      CString netName;
      int kwIndex = schDocPtr->IsKeyWord(schNetNameAttrib, 0);
      if (schItem->getAttributesRef() && schItem->getAttributesRef()->Lookup(kwIndex, attrib))
      {
         // Make sure the attribute is a string
         if (attrib->getValueType() == VT_STRING)
            netName = get_attvalue_string(schDocPtr, attrib);
      }
      schItemName = netName;
      component = FALSE;
   }

   removeHighlightedNets();
   removeOverriddenComps();

   return GetMatchForSch(schItemName, matchedName, component, zoom);
}

int SchLink::GetMatchForPcb(CString pcbItemName, CString &matchedName, BOOL component, BOOL zoom)
{
   // Check if we should be linking
   if (component && !compLinkState)
      return ERR_SETOFF;
   if (!component && !netLinkState)
      return ERR_SETOFF;

   int itemCnt = 0;
   int foundIndex = -1;

   CSelectItem dlg(AfxGetMainWnd());
   dlg.SchLinkPtr = this;
   dlg.ForComps = component;
   dlg.DocPtr = schDocPtr;
   dlg.FilePtr = schFilePtr;
   dlg.NetAttrib = schNetNameAttrib;

   if (component)
   {
      int pcbCnt = 0;
      SchCompData *compData = NULL;

      // Search the pcb's component array for the item
      for (pcbCnt=0; pcbCnt<pcbCompArray.GetCount(); pcbCnt++)
      {
         compData = pcbCompArray[pcbCnt];
         if (!pcbItemName.CompareNoCase(compData->OrigRefName))
         {
            // Make sure there's a match in the PCB
            if (compData->indexMatch < 0) return ERR_NODATA;

            break;            
         }
      }
      
      // Search through the schematic's component array for other possible matches
      // starting from the match index from the pcb
      for (int schCnt=compData->indexMatch; schCnt<schCompArray.GetCount(); schCnt++)
      {
         compData = schCompArray[schCnt];

         // Make sure the index of the schematic component is matched with the item
         if (compData->indexMatch != pcbCnt)
            continue;

         itemCnt++;
         foundIndex = pcbCnt;

         if (zoom)
            dlg.AddItem(compData);
      }

      if (foundIndex > -1 && itemCnt == 1 && zoom)
      {
         SchCompData *compData = pcbCompArray[foundIndex];
         compData = schCompArray[compData->indexMatch];
         matchedName = compData->OrigRefName;

         if (compData->Sheet < 0)
            return ERR_NOTFOUND;

         // set sheet and zoom to the component
         BlockStruct *lastBlock = schFilePtr->getBlock();
         schFilePtr->setBlock(schDocPtr->getBlockAt(compData->Sheet));
         if (lastBlock != schFilePtr->getBlock())
            schDocPtr->OnFitPageToImage();
         ZoomToComp(schDocPtr, compData);

         return ERR_NOERROR;
      }
   }
   else
   {
      int pcbCnt = 0;
      SchNetData *netData = NULL;

      // Search the pcb's net array for the item
      for (pcbCnt=0; pcbCnt<pcbNetArray.GetCount(); pcbCnt++)
      {
         netData = pcbNetArray[pcbCnt];

         if (!pcbItemName.CompareNoCase(netData->OrigNetName))
         {
            // Make sure there's a match in the PCB
            if (netData->indexMatch < 0) return ERR_NODATA;

            break;
         }
      }

      // Search through the schematic's net array for other possible matches
      // starting from the match index from the pcb
//      for (int schCnt=netData->indexMatch; schCnt<schNetArray.GetCount(); schCnt++)
      for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
      {
         netData = schNetArray[schCnt];

         // Make sure the index of the schematic net is matched with the item
         if (netData->indexMatch != pcbCnt)
            continue;

         itemCnt++;
         foundIndex = pcbCnt;

         if (zoom)
            dlg.AddItem(netData);
      }
      
      if (foundIndex >= 0 && itemCnt == 1 && zoom)
      {
         SchNetData *netData = pcbNetArray[foundIndex];
         netData = schNetArray[netData->indexMatch];
         matchedName = netData->OrigNetName;

         if (netData->Sheet < 0)
            return ERR_NOTFOUND;

         // set sheet, highlight and zoom to the net
         BlockStruct *lastBlock = schFilePtr->getBlock();
         schFilePtr->setBlock(schDocPtr->getBlockAt(netData->Sheet));
         if (lastBlock != schFilePtr->getBlock())
            schDocPtr->OnFitPageToImage();

         ZoomToNet(schDocPtr, netData);

         return ERR_NOERROR;
      }
   }

   if (foundIndex >= 0 && zoom)
      dlg.DoModal();

   return ERR_NOTFOUND;
}

int SchLink::GetMatchForPcb(DataStruct *pcbItem, CString &matchedName, BOOL zoom)
{
   BOOL component;
   CString pcbItemName;

   if (pcbItem->getDataType() == dataTypeInsert && 
       (pcbItem->getInsert()->getInsertType() == insertTypePcbComponent || pcbItem->getInsert()->getInsertType() == insertTypeTestPoint))
   {
      // Check if we should be linking
      if (!compLinkState)
         return ERR_SETOFF;

      pcbItemName = pcbItem->getInsert()->getRefname();
      component = TRUE;
   }
   else if (pcbItem->getDataType() == dataTypePoly && pcbItem->getGraphicClass() == graphicClassEtch)
   {
      // Check if we should be linking
      if (!netLinkState)
         return ERR_SETOFF;

      // look up the "Netname" attribute
      Attrib* attrib;
      CString netName;
      int kwIndex = pcbDocPtr->IsKeyWord(pcbNetNameAttrib , 0);

      if (pcbItem->getAttributesRef() && pcbItem->getAttributesRef()->Lookup(kwIndex, attrib))
      {
         // Make sure the attribute is a string
         if (attrib->getValueType() == VT_STRING)
            netName = get_attvalue_string(pcbDocPtr, attrib);
      }

      pcbItemName = netName;
      component = FALSE;
   }
   else
      return ERR_BADTYPE;

   removeHighlightedNets();
   removeOverriddenComps();

   return GetMatchForPcb(pcbItemName, matchedName, component, zoom);
}

void SchLink::ZoomToComp(CCEtoODBDoc *doc, SchCompData *compData)
{
   CString comp = compData->OrigRefName;

   POSITION viewPos = doc->GetFirstViewPosition();
   if (viewPos == NULL)
   {
      return;
   }

   CCEtoODBView *view = (CCEtoODBView*)doc->GetNextView(viewPos);
   if (!keepActiveDoc)
   {
      view->GetParentFrame()->ActivateFrame(SW_SHOW);
      view->SetFocus();
   }

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;
   int Select_component = FALSE;

   doc->UnselectAll(FALSE);
   if (!ComponentsExtents(doc, comp, &xmin, &xmax, &ymin, &ymax, Select_component))
      return;

   double pageWidth = doc->getSettings().getXmax() - doc->getSettings().getXmin();
   double pageHeight = doc->getSettings().getYmax() - doc->getSettings().getYmin();
   double pageSize = max(pageWidth, pageWidth);

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   double marginSize = pageSize * (doc->schLinkPCBDoc?pApp->schSettings.PcbMargin:pApp->schSettings.SchMargin) / 100;

   xmin -= marginSize;
   xmax += marginSize;
   
   ymin -= marginSize;
   ymax += marginSize;

   compData->data->setOverrideColor((doc->schLinkPCBDoc?pApp->schSettings.PcbHightlightColor:pApp->schSettings.SchHightlightColor));
   compData->data->setColorOverride(true);
   view->ZoomBox(min(xmin, xmax), max(xmin, xmax), min(ymin, ymax), max(ymin, ymax));
}

void SchLink::ZoomToNet(CCEtoODBDoc *doc, SchNetData *netData)
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   CString net = netData->OrigNetName;

   POSITION viewPos = doc->GetFirstViewPosition();
   if (viewPos == NULL)
      return;

   CCEtoODBView *view = (CCEtoODBView*)doc->GetNextView(viewPos);
   if (!keepActiveDoc)
   {
      view->GetParentFrame()->ActivateFrame(SW_SHOW);
      view->SetFocus();
   }

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   doc->UnselectAll(FALSE);
   if (!NetExtents(doc, net, &xmin, &xmax, &ymin, &ymax))
      return;

   double pageWidth = doc->getSettings().getXmax() - doc->getSettings().getXmin();
   double pageHeight = doc->getSettings().getYmax() - doc->getSettings().getYmin();
   double pageSize = max(pageWidth, pageWidth);

   double marginSize = pageSize * (doc->schLinkPCBDoc?pApp->schSettings.PcbMargin:pApp->schSettings.SchMargin) / 100;

   xmin -= marginSize;
   xmax += marginSize;
   
   ymin -= marginSize;
   ymax += marginSize;

   view->ZoomBox(min(xmin, xmax), max(xmin, xmax), min(ymin, ymax), max(ymin, ymax));
   FlushQueue();

   if (netData->net)
   {
      netData->net->setHighlighted(true);
      int value = doc->RegisterValue(netData->net->getNetName());
      doc->HighlightedNetsMap.SetAt(value, (doc->schLinkPCBDoc?pApp->schSettings.PcbHightlightColor:pApp->schSettings.SchHightlightColor));
      doc->HighlightByAttrib(FALSE, doc->IsKeyWord((doc->schLinkPCBDoc?pcbNetNameAttrib:schNetNameAttrib), TRUE), VT_STRING, value);
   }
}

CString SchLink::GetLastError()
{
   return errMsg;
}

BOOL SchLink::SetCaseSensitive(BOOL CaseSensitive)
{
   BOOL lastVal = caseSensitive;
   caseSensitive = CaseSensitive;

   return lastVal;
}

BOOL SchLink::SetKillTrailingAlphas(BOOL KillTrailingAlphas)
{
   BOOL lastVal = killTrailingAlphas;
   killTrailingAlphas = KillTrailingAlphas;

   return lastVal;
}

BOOL SchLink::SetKillTrailingSuffix(CString TrailingSuffix, BOOL KillTrailingSuffix)
{
   BOOL lastVal = killTrailingSuffix;
   killTrailingSuffix = KillTrailingSuffix;
   trailingSuffix = TrailingSuffix;

   return lastVal;
}

BOOL SchLink::SetToKeepActiveDoc(BOOL KeepActiveDoc)
{
   BOOL lastVal = keepActiveDoc;
   keepActiveDoc = KeepActiveDoc;

   return lastVal;
}

int SchLink::SetNetCompType(int NetCompType)
{
   BOOL lastVal = netCompType;
   netCompType = NetCompType;

   return lastVal;
}

CString SchLink::SetSchRefDesAttrib(CString RefDesAttrib)
{
   if (RefDesAttrib.IsEmpty())
      return schRefDesAttrib;

   CString lastVal = schRefDesAttrib;
   schRefDesAttrib = RefDesAttrib;

   // reload the data if the attrib has changed
   if (lastVal != schRefDesAttrib)
   {
      // Always create a new one (seems each process is responsible for creating and destroying
      progress = new CProgressDlg("Schematic Link");
      progress->Create(AfxGetMainWnd());

      progress->ShowWindow(SW_SHOW);
      progress->SetStep(1);

      fillSchArrays();

      progress->ShowWindow(SW_HIDE);
      if (progress->DestroyWindow())
      {
         delete progress;
         progress = NULL;
      }
   }

   return lastVal;
}

CString SchLink::SetPcbRefDesAttrib(CString RefDesAttrib)
{
   if (RefDesAttrib.IsEmpty())
      return pcbRefDesAttrib;

   CString lastVal = pcbRefDesAttrib;
   pcbRefDesAttrib = RefDesAttrib;

   // reload the data if the attrib has changed
   if (lastVal != pcbRefDesAttrib)
   {
      // Always create a new one (seems each process is responsible for creating and destroying
      progress = new CProgressDlg("Schematic Link");
      progress->Create(AfxGetMainWnd());

      progress->ShowWindow(SW_SHOW);
      progress->SetStep(1);

      fillPcbArrays();

      progress->ShowWindow(SW_HIDE);
      if (progress->DestroyWindow())
      {
         delete progress;
         progress = NULL;
      }
   }

   return lastVal;
}

CString SchLink::SetSchNetNameAttrib(CString NetNameAttrib)
{
   CString lastVal = schNetNameAttrib;

   schNetNameAttrib = (NetNameAttrib.IsEmpty()?ATT_NETNAME:NetNameAttrib);

   return lastVal;
}

CString SchLink::SetPcbNetNameAttrib(CString NetNameAttrib)
{
   CString lastVal = pcbNetNameAttrib;

   pcbNetNameAttrib = (NetNameAttrib.IsEmpty()?ATT_NETNAME:NetNameAttrib);

   return lastVal;
}

void SchLink::AnnotateNetnameUsingSchematicResult()
{
   if (schDocPtr == NULL || schFilePtr == NULL)
      return;

   int netNameKw = schDocPtr->getStandardAttributeKeywordIndex(standardAttributeNetName);
   CMapStringToString uniqueMergeNameMap;
   CTypedMapStringToPtrContainer<NetStruct*> netStructToBeRemoveMap(nextPrime2n(20),false);

   // Create unique netname for Schematic nets that merge to match PCB nets
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData* schNetData = schNetArray.GetAt(schCnt);      
      if (schNetData == NULL || schNetData->indexMatch < 0)
         continue;

      SchNetData *pcbCPData = pcbNetArray[schNetData->indexMatch];      

      CString uniqueMergeName;
      CString value;
      CString netName;
      if (uniqueMergeNameMap.Lookup(pcbCPData->OrigNetName, value))
      {
         uniqueMergeName.Format(" %s,", value); 
         uniqueMergeName.MakeLower();

         netName.Format(" %s,", schNetData->OrigNetName);
         netName.MakeLower();

         if (uniqueMergeName.Find(netName) < 0)
         {
            // Only add netname that is not already in the string
            uniqueMergeName.Format("%s, %s", value, schNetData->OrigNetName);
            uniqueMergeNameMap.SetAt(pcbCPData->OrigNetName, uniqueMergeName);
         }
      }
      else
      {
         uniqueMergeName = schNetData->OrigNetName;
         uniqueMergeNameMap.SetAt(pcbCPData->OrigNetName, uniqueMergeName);
      }
   }

   // Update merged nets with new netnames
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData* schNetData = schNetArray.GetAt(schCnt);      
      if (schNetData == NULL || schNetData->indexMatch < 0)
         continue;

      SchNetData* pcbNetData = pcbNetArray[schNetData->indexMatch];
      CString uniqueMergeName;
      if (!uniqueMergeNameMap.Lookup(pcbNetData->OrigNetName, uniqueMergeName))
         continue;

      // Skip over non-merge net
      if (uniqueMergeName.CompareNoCase(schNetData->OrigNetName) == 0)
         continue;

      // 1) Derive new schematic netname, get compareName and sortKey
      CString newNetName;
      newNetName.Format("%s_(%s)", pcbNetData->OrigNetName, uniqueMergeName);

      CString compareName = ApplyFilters(newNetName, FALSE, TRUE);
      CString sortKey = createSortKey(compareName);

      //2) Assign them to the schNetData
      schNetData->OrigNetName = newNetName;
      schNetData->CompareName = compareName;
      schNetData->SortKey = sortKey;

      //3)) Add new netStruct to netlist and assign to schNetData
      NetStruct* oldNet = schNetData->net;
      NetStruct* newNet = add_net(schFilePtr, newNetName);
      schNetData->net = newNet;

      //4) Update NET_NAME attribute on dataStruct to newNetName
      if (schNetData->net != NULL)
         schNetData->net->setAttrib(schDocPtr->getCamCadData(), netNameKw, valueTypeString, newNetName.GetBuffer(0), attributeUpdateOverwrite, NULL);

      if (schNetData->data != NULL)
         schNetData->data->setAttrib(schDocPtr->getCamCadData(), netNameKw, valueTypeString, newNetName.GetBuffer(0), attributeUpdateOverwrite, NULL);

      // Only do this if the old net is not already in the map
      NetStruct* removeNet = NULL;
      if (!netStructToBeRemoveMap.Lookup(oldNet->getNetName(), removeNet))
      {
         netStructToBeRemoveMap.SetAt(oldNet->getNetName(), oldNet);

         schDocPtr->CopyAttribs(&newNet->getAttributesRef(), oldNet->getAttributesRef());

         //5) Move comppin from old netStruct to new netStruct
         newNet->takeCompPins(*oldNet);

         //for (POSITION comPinPos=oldNet->getHeadCompPinPosition(); comPinPos != NULL;)
         //{
         //   POSITION curPos = comPinPos;
         //   CompPinStruct* compPin = oldNet->getNextCompPin(comPinPos);
         //   if (compPin == NULL)
         //      continue;

         //   oldNet->getCompPinList().RemoveAt(curPos);
         //   
         //   if (!newNet->findCompPin(compPin->getRefDes(), compPin->getPinName(), true))
         //   {
         //      newNet->getCompPinList().AddTail(compPin);
         //   }
         //   else
         //   {
         //      delete compPin;
         //      compPin = NULL;
         //   }
         //}

         //6) Replace old netname with new netname in bus
         for (POSITION busPos=schFilePtr->getBusList().GetHeadPosition(); busPos != NULL;)
         {
            CBusStruct* bus = schFilePtr->getBusList().GetNext(busPos);
            if (bus == NULL)
               continue;

            // This function will only replace the netname if it is in the bus
            bus->ReplaceNetName(oldNet->getNetName(), newNet->getNetName());
         }
      }
   }

   //7) Remove old nets from netlist
   for (POSITION pos=netStructToBeRemoveMap.GetStartPosition(); pos!= NULL;)
   {
      CString key;
      NetStruct* oldNet;
      netStructToBeRemoveMap.GetNextAssoc(pos, key, oldNet);
      RemoveNet(schFilePtr, oldNet);
   }

   //8) Sort the array of netData in Schematic 
   qsort(schNetArray.GetData(), schNetArray.GetCount(), sizeof(SchNetData*), CompareSchematicNetData);

   //9) Update the matched index of netData in PCB
   int prevPcbIndexMatch = -1;
   for (int schCnt=0; schCnt<schNetArray.GetCount(); schCnt++)
   {
      SchNetData* schNetData = schNetArray.GetAt(schCnt);      
      if (schNetData == NULL || schNetData->indexMatch < 0)
         continue;

      // Since schNetArray is sorted alphabetically, therefore, all schNetData that matched to the same pcbNetData
      // are group together so if the current schNetData is match to the same pcbNetData as the last one then skip
      if (schNetData->indexMatch == prevPcbIndexMatch)
         continue;

      SchNetData* pcbNetData = pcbNetArray[schNetData->indexMatch];
      pcbNetData->indexMatch = schCnt;
      
      // Save the current pcb index match
      prevPcbIndexMatch = schNetData->indexMatch;
   }
}

int CompareSchematicNetData(const void* arg1, const void* arg2)
{
   SchNetData** netData1 = (SchNetData**)arg1;
   SchNetData** netData2 = (SchNetData**)arg2;

   return (*netData1)->SortKey.Compare((*netData2)->SortKey);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CCompareCompPP dialog
////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCompareCompPP, CPropertyPage)
CCompareCompPP::CCompareCompPP()
   : CPropertyPage(CCompareCompPP::IDD)
   , m_killAlphas(TRUE)
   , m_caseSensitive(FALSE)
   , m_schRefDesAttrib(_T(""))
   , m_pcbRefDesAttrib(_T(""))
   , m_killSuffix(FALSE)
   , m_suffix(_T(""))
{
}

CCompareCompPP::~CCompareCompPP()
{
}

void CCompareCompPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SCHLIST, m_SchListBox);
   DDX_Control(pDX, IDC_PCBLIST, m_PcbListBox);
   DDX_Control(pDX, IDC_UNMATCHEDLIST_SCH, m_UnmatchedSchListBox);
   DDX_Control(pDX, IDC_UNMATCHEDLIST_PCB, m_UnmatchedPcbListBox);
   DDX_Check(pDX, IDC_KILLTRAILINGALPHAS, m_killAlphas);
   DDX_Check(pDX, IDC_CASESENSITIVE, m_caseSensitive);
   DDX_Text(pDX, IDC_REFDESATTRIB_SCH, m_schRefDesAttrib);
   DDX_Text(pDX, IDC_REFDESATTRIB_PCB, m_pcbRefDesAttrib);
   DDX_Check(pDX, IDC_KILLTRAILINGSUFFIX, m_killSuffix);
   DDX_Text(pDX, IDC_SUFFIX, m_suffix);
}


BEGIN_MESSAGE_MAP(CCompareCompPP, CPropertyPage)
   ON_BN_CLICKED(IDC_RECOMPARE, OnBnClickedRecompare)
   ON_BN_CLICKED(IDC_LOADXRF, OnBnClickedLoadxrf)
   ON_BN_CLICKED(IDC_EDITXRF, OnBnClickedEditxrf)
   ON_BN_CLICKED(IDC_CLEARXRF, OnBnClickedClearxrf)
   ON_EN_CHANGE(IDC_XRFPATH, OnEnChangeXrfpath)
   ON_BN_CLICKED(IDC_NEWREFDES_SCH, OnBnClickedNewrefdesSch)
   ON_BN_CLICKED(IDC_NEWREFDES_PCB, OnBnClickedNewrefdesPcb)
   ON_BN_CLICKED(IDC_KILLTRAILINGSUFFIX, OnBnClickedKilltrailingsuffix)
END_MESSAGE_MAP()


// CCompareCompPP message handlers
//
BOOL CCompareCompPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   if (!compareItems(TRUE)) return FALSE;

   GetDlgItem(IDC_SUFFIX)->EnableWindow(FALSE);

   fillListBox();
   updateXrefButtons();

   return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCompareCompPP::compareItems(BOOL firstTime)
{
   UpdateData();

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   if (firstTime)
   {
      m_schRefDesAttrib = pApp->schLink.SetSchRefDesAttrib("");
      m_pcbRefDesAttrib = pApp->schLink.SetPcbRefDesAttrib("");
      UpdateData(FALSE);
   }

   pApp->schLink.ResetComps();
   pApp->schLink.SetCaseSensitive(m_caseSensitive);
   pApp->schLink.SetKillTrailingAlphas(m_killAlphas);
   pApp->schLink.SetKillTrailingSuffix(m_suffix, m_killSuffix);
   pApp->schLink.SetSchRefDesAttrib(m_schRefDesAttrib);
   pApp->schLink.SetPcbRefDesAttrib(m_pcbRefDesAttrib);
   if (pApp->schLink.CompareComps(firstTime) != ERR_NOERROR)
   {
      ErrorMessage(SCHLINK_ERR + pApp->schLink.GetLastError(), "Schematic Linking Error", MB_OK);
   
      CWnd *pWnd = GetDlgItem(IDC_SCHLIST);
      while (pWnd->IsWindowEnabled())
      {
         pWnd->EnableWindow(FALSE);
         pWnd = GetNextDlgTabItem(pWnd);
      }

      return FALSE;
   }

   return TRUE;
}

void CCompareCompPP::fillListBox()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   CString buf;
   long matchCnt = 0;

   // used to make sure we don't add duplicates
   CMapStringToString listStrings(100), unmatchedStrings(100);

   m_SchListBox.ResetContent();
   m_PcbListBox.ResetContent();
   m_UnmatchedSchListBox.ResetContent();
   m_UnmatchedPcbListBox.ResetContent();

   // Fill the schematic list box
   listStrings.RemoveAll();
   unmatchedStrings.RemoveAll();
   for (int i=0; i<mSchCompArray->GetCount(); i++)
   {
      SchCompData *compData = mSchCompArray->GetAt(i);
      CString compname = pApp->schLink.ApplyFilters(compData->CompareName, TRUE, TRUE), dummy;

      if (!listStrings.Lookup(compname, dummy))
      {
#ifdef SCHLINK_DEBUG
         m_SchListBox.AddString(compData->OrigRefName + " " + compname);
#else
         m_SchListBox.AddString(compname);
#endif
         listStrings.SetAt(compname, compname);
      }

      if (compData->indexMatch < 0 && !unmatchedStrings.Lookup(compname, dummy))
      {
#ifdef SCHLINK_DEBUG
         m_UnmatchedSchListBox.AddString(compData->OrigRefName + " " + compname);
#else
         m_UnmatchedSchListBox.AddString(compname);
#endif
         unmatchedStrings.SetAt(compname, compname);
      }
   }
   buf.Format("%i Items", m_SchListBox.GetCount());
   GetDlgItem(IDC_SCHCOUNT_TITLE)->SetWindowText(buf);
   buf.Format("%i Items with no match", m_UnmatchedSchListBox.GetCount());
   GetDlgItem(IDC_UNMATCHEDLIST_SCH_TITLE)->SetWindowText(buf);

   // Fill the pcb list box
   listStrings.RemoveAll();
   unmatchedStrings.RemoveAll();
   for (int i=0; i<mPcbCompArray->GetCount(); i++)
   {
      SchCompData *compData = mPcbCompArray->GetAt(i);
      CString compname = pApp->schLink.ApplyFilters(compData->CompareName, TRUE, FALSE), dummy;

      if (!listStrings.Lookup(compname, dummy))
      {
#ifdef SCHLINK_DEBUG
      m_PcbListBox.AddString(compData->OrigRefName + " " + compname);
#else
      m_PcbListBox.AddString(compname);
#endif
      }

      if (compData->indexMatch < 0)
      {
         if (!listStrings.Lookup(compname, dummy))
         {
#ifdef SCHLINK_DEBUG
         m_UnmatchedPcbListBox.AddString(compData->OrigRefName + " " + compname);
#else
         m_UnmatchedPcbListBox.AddString(compname);
#endif
            unmatchedStrings.SetAt(compname, compname);
         }
      }
      else if (!listStrings.Lookup(compname, dummy))
         matchCnt++;    // don't count duplicates

      listStrings.SetAt(compname, compname);
   }
   buf.Format("%i Items", m_PcbListBox.GetCount());
   GetDlgItem(IDC_PCBCOUNT_TITLE)->SetWindowText(buf);
   buf.Format("%i Items with no match", m_UnmatchedPcbListBox.GetCount());
   GetDlgItem(IDC_UNMATCHEDLIST_PCB_TITLE)->SetWindowText(buf);

   buf.Format("Total Matched : %i", matchCnt);
   GetDlgItem(IDC_MATCHEDCOUNT_TITLE)->SetWindowText(buf);

   //m_SchListBox.ShowScrollBar(SB_BOTH, TRUE);
   //m_PcbListBox.ShowScrollBar(SB_BOTH, TRUE);
   //m_UnmatchedSchListBox.ShowScrollBar(SB_BOTH, TRUE);
   //m_UnmatchedPcbListBox.ShowScrollBar(SB_BOTH, TRUE);
}

void CCompareCompPP::updateXrefButtons()
{
   CString xrfPath;
   GetDlgItem(IDC_XRFPATH)->GetWindowText(xrfPath);
   if (xrfPath.IsEmpty())
      GetDlgItem(IDC_CLEARXRF)->EnableWindow(FALSE);
   else
      GetDlgItem(IDC_CLEARXRF)->EnableWindow(TRUE);
}

void CCompareCompPP::loadXrfFile(CString xrfPath, CStringArray *schArray, CStringArray *pcbArray)
{
   CWaitCursor wait;
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   
   if (pApp->schLink.LoadXRefFile(xrfPath, TRUE, schArray, pcbArray) != ERR_NOERROR)
      ErrorMessage(pApp->schLink.GetLastError());
}

int CCompareCompPP::editXrfFile(BOOL fromLoad, CStringArray *schArray, CStringArray *pcbArray)
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   CXrefEditorDlg editDlg;
   editDlg.SetData(schArray, pcbArray);
   editDlg.forCompData = TRUE;
   if (editDlg.DoModal() == IDCANCEL)
      return IDCANCEL;

   // Clear out the old data
   pApp->schLink.CompXRefArray[SCH_XREF].RemoveAll();
   pApp->schLink.CompXRefArray[PCB_XREF].RemoveAll();

   // Populate with the new cross reference data
   CString schName, pcbName;
   for (long cnt=0; editDlg.GetXRefData(cnt, schName, pcbName); cnt++)
   {
      pApp->schLink.CompXRefArray[SCH_XREF].SetAtGrow(cnt, schName);
      pApp->schLink.CompXRefArray[PCB_XREF].SetAtGrow(cnt, pcbName);
   }

   if (!fromLoad)
      GetDlgItem(IDC_XRFPATH)->SetWindowText(editDlg.SavedName);

   OnBnClickedRecompare();

   return IDOK;
}

BOOL CCompareCompPP::OnSetActive()
{
   CPropertySheet* psheet = (CPropertySheet*)GetParent();   
   psheet->SetWizardButtons(PSWIZB_NEXT);

   return TRUE;
}

void CCompareCompPP::OnBnClickedRecompare()
{
   UpdateData();
   compareItems();
   fillListBox();
}

void CCompareCompPP::OnBnClickedLoadxrf()
{
   CString xrfFileName;
   GetDlgItem(IDC_XRFPATH)->GetWindowText(xrfFileName);

   if (!xrfFileName.IsEmpty())
   {
      CString buf = "All previous cross reference data will be discarded.\n\nDo you want to continue?";
      if (AfxMessageBox(buf, MB_YESNO) == IDNO)
         return;
   }

   CFileDialog fileDlg(TRUE, ".xrf", NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, 
                  "Cross Reference Files (*.xrf)|*.xrf|All Files (*.*)|*.*||", this);   
   if (fileDlg.DoModal() == IDCANCEL) return;

   xrfFileName = fileDlg.GetPathName();
   CStringArray schArray, pcbArray;
   loadXrfFile(xrfFileName, &schArray, &pcbArray);
   if (editXrfFile(TRUE, &schArray, &pcbArray) == IDOK)
      GetDlgItem(IDC_XRFPATH)->SetWindowText(xrfFileName);
}

void CCompareCompPP::OnBnClickedEditxrf()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   editXrfFile(FALSE, &(pApp->schLink.CompXRefArray[SCH_XREF]), &(pApp->schLink.CompXRefArray[PCB_XREF]));
}

void CCompareCompPP::OnBnClickedClearxrf()
{
   GetDlgItem(IDC_XRFPATH)->SetWindowText("");

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->schLink.CompXRefArray[SCH_XREF].RemoveAll();
   pApp->schLink.CompXRefArray[PCB_XREF].RemoveAll();
}

void CCompareCompPP::OnEnChangeXrfpath()
{
   updateXrefButtons();
}

void CCompareCompPP::OnBnClickedNewrefdesSch()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   //get new ref des
   CSchSelRefDes dlg;
   dlg.forPcb = FALSE;
   dlg.filePtr = pApp->schLink.schFilePtr;
   dlg.docPtr = pApp->schLink.schDocPtr;
   if (dlg.DoModal() == IDCANCEL)
      return;

   m_schRefDesAttrib = dlg.newRefDesValue;
   UpdateData(FALSE);
}

void CCompareCompPP::OnBnClickedNewrefdesPcb()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   //get new ref des
   CSchSelRefDes dlg;
   dlg.forPcb = TRUE;
   dlg.filePtr = pApp->schLink.pcbFilePtr;
   dlg.docPtr = pApp->schLink.pcbDocPtr;
   if (dlg.DoModal() == IDCANCEL)
      return;

   m_pcbRefDesAttrib = dlg.newRefDesValue;
   UpdateData(FALSE);
}

void CCompareCompPP::OnBnClickedKilltrailingsuffix()
{
   UpdateData();

   GetDlgItem(IDC_SUFFIX)->EnableWindow(m_killSuffix);
}

LRESULT CCompareCompPP::OnWizardNext()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   // Save all comparison results into the attributes
   for (int i=0; i<mSchCompArray->GetCount(); i++)
   {
      SchCompData *compData = mSchCompArray->GetAt(i);
      CString value;

      // get the matching PCB data
      SchCompData *pcbCompData = NULL;
      if (compData->indexMatch >= 0)
      {
         pcbCompData = mPcbCompArray->GetAt(compData->indexMatch);
         value = pcbCompData->OrigRefName;
      }
      else
         value = "NULL";

      Attrib *attrib = NULL;
      pApp->schLink.schDocPtr->SetUnknownAttrib(&(compData->data->getAttributesRef()), SCHLINK_PCB_REFDES, value, SA_OVERWRITE, &(attrib));
   }

   return CPropertyPage::OnWizardNext();
}



////////////////////////////////////////////////////////////////////////////////////////////
// CCompareNetPP dialog
////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCompareNetPP, CPropertyPage)
CCompareNetPP::CCompareNetPP()
   : CPropertyPage(CCompareNetPP::IDD)
   , m_caseSensitive(FALSE)
   , m_CompareBy(NETCOMPTYPE_CONTENT_PCBASMASTER)
{
}

CCompareNetPP::~CCompareNetPP()
{
}

void CCompareNetPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SCHLIST, m_SchListBox);
   DDX_Control(pDX, IDC_PCBLIST, m_PcbListBox);
   DDX_Control(pDX, IDC_UNMATCHEDLIST_SCH, m_UnmatchedSchListBox);
   DDX_Control(pDX, IDC_UNMATCHEDLIST_PCB, m_UnmatchedPcbListBox);
   DDX_Check(pDX, IDC_CASESENSITIVE, m_caseSensitive);
   DDX_Radio(pDX, IDC_COMPAREBY, m_CompareBy);
}


BEGIN_MESSAGE_MAP(CCompareNetPP, CPropertyPage)
   ON_BN_CLICKED(IDC_RECOMPARE, OnBnClickedRecompare)
   ON_BN_CLICKED(IDC_LOADXRF, OnBnClickedLoadxrf)
   ON_BN_CLICKED(IDC_EDITXRF, OnBnClickedEditxrf)
   ON_BN_CLICKED(IDC_CLEARXRF, OnBnClickedClearxrf)
   ON_EN_CHANGE(IDC_XRFPATH, OnEnChangeXrfpath)
   ON_BN_CLICKED(IDC_LOADFILTER, OnBnClickedLoadfilter)
   ON_BN_CLICKED(IDC_EDITFILTER, OnBnClickedEditfilter)
   ON_BN_CLICKED(IDC_CLEARFILTER, OnBnClickedClearfilter)
END_MESSAGE_MAP()


// CCompareNetPP message handlers
BOOL CCompareNetPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   if (!compareItems(TRUE)) return FALSE;
   
   GetDlgItem(IDC_EDITFILTER)->ShowWindow(SW_HIDE);

   GetDlgItem(IDC_FILTERPATH)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_LOADFILTER)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_EDITFILTER)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_CLEARFILTER)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_XREF_BOX2)->ShowWindow(SW_HIDE);

   fillListBox();
   updateXrefButtons();

   return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCompareNetPP::compareItems(BOOL firstTime)
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->schLink.ResetNets();
   pApp->schLink.SetCaseSensitive(m_caseSensitive);
   pApp->schLink.SetKillTrailingAlphas(FALSE);
   pApp->schLink.SetNetCompType(m_CompareBy);
   if (pApp->schLink.CompareNets(firstTime) != ERR_NOERROR)
   {
      ErrorMessage(SCHLINK_ERR + pApp->schLink.GetLastError(), "Schematic Linking Error", MB_OK);
   
      CWnd *pWnd = GetDlgItem(IDC_SCHLIST);
      while (pWnd->IsWindowEnabled())
      {
         pWnd->EnableWindow(FALSE);
         pWnd = GetNextDlgTabItem(pWnd);
      }
      return FALSE;
   }

   return TRUE;
}

void CCompareNetPP::fillListBox()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   CString buf;
   long matchCnt = 0;

   // used to make sure we don't add duplicates
   CMapStringToString listStrings(100), unmatchedStrings(100);

   m_SchListBox.ResetContent();
   m_PcbListBox.ResetContent();
   m_UnmatchedSchListBox.ResetContent();
   m_UnmatchedPcbListBox.ResetContent();

   // Fill the schematic list box
   listStrings.RemoveAll();
   unmatchedStrings.RemoveAll();
   for (int i=0; i<mSchNetArray->GetCount(); i++)
   {
      SchNetData *netData = mSchNetArray->GetAt(i);
      CString netname = pApp->schLink.ApplyFilters(netData->CompareName, FALSE, TRUE), dummy;

      if (!listStrings.Lookup(netname, dummy))
      {
#ifdef SCHLINK_DEBUG
         m_SchListBox.AddString(netData->OrigNetName + " " + netname);
#else
         m_SchListBox.AddString(netname);
#endif
         listStrings.SetAt(netname, netname);
      }


      if (netData->indexMatch < 0 && !unmatchedStrings.Lookup(netname, dummy))
      {
#ifdef SCHLINK_DEBUG
         m_UnmatchedSchListBox.AddString(netData->OrigNetName + " " + netname);
#else
         m_UnmatchedSchListBox.AddString(netname);
#endif
         unmatchedStrings.SetAt(netname, netname);
      }
   }
   buf.Format("%i Items", m_SchListBox.GetCount());
   GetDlgItem(IDC_SCHCOUNT_TITLE)->SetWindowText(buf);
   buf.Format("%i Items with no match", m_UnmatchedSchListBox.GetCount());
   GetDlgItem(IDC_UNMATCHEDLIST_SCH_TITLE)->SetWindowText(buf);

   // Fill the pcb list box
   listStrings.RemoveAll();
   unmatchedStrings.RemoveAll();
   for (int i=0; i<mPcbNetArray->GetCount(); i++)
   {
      SchNetData *netData = mPcbNetArray->GetAt(i);
      CString netname = pApp->schLink.ApplyFilters(netData->CompareName, FALSE, FALSE), dummy;

      if (!listStrings.Lookup(netname, dummy))
      {
#ifdef SCHLINK_DEBUG
         m_PcbListBox.AddString(netData->OrigNetName + " " + netname);
#else
         m_PcbListBox.AddString(netname);
#endif
      }

      if (netData->indexMatch < 0 && !unmatchedStrings.Lookup(netname, dummy))
      {
#ifdef SCHLINK_DEBUG
         m_UnmatchedPcbListBox.AddString(netData->OrigNetName + " " + netname);
#else
         m_UnmatchedPcbListBox.AddString(netname);
#endif
         unmatchedStrings.SetAt(netname, netname);
      }
      else if (!listStrings.Lookup(netname, dummy))
         matchCnt++;    // don't count duplicates

      listStrings.SetAt(netname, netname);
   }
   buf.Format("%i Items", m_PcbListBox.GetCount());
   GetDlgItem(IDC_PCBCOUNT_TITLE)->SetWindowText(buf);
   buf.Format("%i Items with no match", m_UnmatchedPcbListBox.GetCount());
   GetDlgItem(IDC_UNMATCHEDLIST_PCB_TITLE)->SetWindowText(buf);

   buf.Format("Total Matched : %i", matchCnt);
   GetDlgItem(IDC_MATCHEDCOUNT_TITLE)->SetWindowText(buf);

   //m_SchListBox.ShowScrollBar(SB_BOTH, TRUE);
   //m_PcbListBox.ShowScrollBar(SB_BOTH, TRUE);
   //m_UnmatchedSchListBox.ShowScrollBar(SB_BOTH, TRUE);
   //m_UnmatchedPcbListBox.ShowScrollBar(SB_BOTH, TRUE);
}

void CCompareNetPP::updateXrefButtons()
{
   CString xrfPath;
   GetDlgItem(IDC_XRFPATH)->GetWindowText(xrfPath);
   if (xrfPath.IsEmpty())
      GetDlgItem(IDC_CLEARXRF)->EnableWindow(FALSE);
   else
      GetDlgItem(IDC_CLEARXRF)->EnableWindow(TRUE);
}

void CCompareNetPP::loadXrfFile(CString xrfPath, CStringArray *schArray, CStringArray *pcbArray)
{
   CWaitCursor wait;
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   
   if (pApp->schLink.LoadXRefFile(xrfPath, FALSE, schArray, pcbArray) != ERR_NOERROR)
      ErrorMessage(pApp->schLink.GetLastError());
}

int CCompareNetPP::editXrfFile(BOOL fromLoad, CStringArray *schArray, CStringArray *pcbArray)
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   
   // Get the current filename
   CString fileName;
   GetDlgItem(IDC_XRFPATH)->GetWindowText(fileName);

   CXrefEditorDlg editDlg;
   editDlg.SetData(schArray, pcbArray);
   editDlg.forCompData = FALSE;
   if (!fromLoad && !fileName.IsEmpty()) editDlg.SavedName = fileName;
   if (editDlg.DoModal() == IDCANCEL)
      return IDCANCEL;

   // Clear out the old data
   pApp->schLink.NetXRefArray[SCH_XREF].RemoveAll();
   pApp->schLink.NetXRefArray[PCB_XREF].RemoveAll();

   // Populate with the new cross reference data
   CString schName, pcbName;
   for (long cnt=0; editDlg.GetXRefData(cnt, schName, pcbName); cnt++)
   {
      pApp->schLink.NetXRefArray[SCH_XREF].SetAtGrow(cnt, schName);
      pApp->schLink.NetXRefArray[PCB_XREF].SetAtGrow(cnt, pcbName);
   }

   if (!fromLoad)
      GetDlgItem(IDC_XRFPATH)->SetWindowText(editDlg.SavedName);

   OnBnClickedRecompare();

   return IDOK;
}

void CCompareNetPP::loadFilterFile(CString filterPath)
{
   CWaitCursor wait;
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   
   if (pApp->schLink.LoadFilterFile(filterPath) == ERR_NOERROR)
      GetDlgItem(IDC_FILTERPATH)->SetWindowText(filterPath);
   else
      ErrorMessage(pApp->schLink.GetLastError());
}

BOOL CCompareNetPP::OnSetActive()
{
   CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

   return TRUE;
}

void CCompareNetPP::OnBnClickedRecompare()
{
   UpdateData();
   compareItems();   
   fillListBox();
}

void CCompareNetPP::OnBnClickedLoadxrf()
{
   CString xrfFileName;
   GetDlgItem(IDC_XRFPATH)->GetWindowText(xrfFileName);

   if (!xrfFileName.IsEmpty())
   {
      CString buf = "All previous cross reference data will be discarded.\n\nDo you want to continue?";
      if (AfxMessageBox(buf, MB_YESNO) == IDNO)
         return;
   }

   CFileDialog fileDlg(TRUE, ".xrf", NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, 
                  "Cross Reference Files (*.xrf)|*.xrf|All Files (*.*)|*.*||", this);
   if (fileDlg.DoModal() == IDCANCEL)
      return;

   xrfFileName = fileDlg.GetPathName();
   CStringArray schArray, pcbArray;
   loadXrfFile(xrfFileName, &schArray, &pcbArray);
   if (editXrfFile(TRUE, &schArray, &pcbArray) == IDOK)
      GetDlgItem(IDC_XRFPATH)->SetWindowText(xrfFileName);
}

void CCompareNetPP::OnBnClickedEditxrf()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   editXrfFile(FALSE, &(pApp->schLink.NetXRefArray[SCH_XREF]), &(pApp->schLink.NetXRefArray[PCB_XREF]));
}

void CCompareNetPP::OnBnClickedClearxrf()
{
   GetDlgItem(IDC_XRFPATH)->SetWindowText("");

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->schLink.NetXRefArray[SCH_XREF].RemoveAll();
   pApp->schLink.NetXRefArray[PCB_XREF].RemoveAll();
}

void CCompareNetPP::OnEnChangeXrfpath()
{
   updateXrefButtons();
}

void CCompareNetPP::OnBnClickedLoadfilter()
{
   CString buf = "All previous filter data will be discarded.\n\nDo you want to continue?";
   if (AfxMessageBox(buf, MB_YESNO) == IDNO)
      return;

   CFileDialog fileDlg(TRUE, ".txt", NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, 
                  "Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this);
   if (fileDlg.DoModal() == IDCANCEL)
      return;

   CString filterFileName = fileDlg.GetPathName();

   // Load filter file
   loadFilterFile(filterFileName);
}

void CCompareNetPP::OnBnClickedEditfilter()
{
   // Edit the filter data
}

void CCompareNetPP::OnBnClickedClearfilter()
{
   GetDlgItem(IDC_FILTERPATH)->SetWindowText("");

   // Clear the filter data
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->schLink.ClearFilterData();
}

BOOL CCompareNetPP::OnWizardFinish()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   if (m_CompareBy == NETCOMPTYPE_CONTENT_PCBASMASTER)
   {
      pApp->schLink.AnnotateNetnameUsingSchematicResult();
   }

   // Save all comparison results into the attributes
   for (int i=0; i<mSchNetArray->GetCount(); i++)
   {
      SchNetData *netData = mSchNetArray->GetAt(i);
      CString value;

      // get the matching PCB data
      SchNetData *pcbNetData = NULL;
      if (netData->indexMatch >= 0)
      {
         pcbNetData = mPcbNetArray->GetAt(netData->indexMatch);
         value = pcbNetData->OrigNetName;
      }
      else
         value = "NULL";

      Attrib *attrib = NULL;
      //pApp->schLink.schDocPtr->SetUnknownAttrib(&(netData->data->getAttributesRef()), SCHLINK_PCB_NETNAME, value, SA_OVERWRITE, &(attrib));
      pApp->schLink.schDocPtr->SetUnknownAttrib(&(netData->net->getAttributesRef()), SCHLINK_PCB_NETNAME, value, SA_OVERWRITE, &(attrib));
   }

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CToolBarCtrl *c = &(frame->m_SchematicToolBar.GetToolBarCtrl());
   c->PressButton(ID_LINK_COMPS, 1);
   c->PressButton(ID_LINK_NETS, 1);

   pApp->schLink.netLinkState = TRUE;
   pApp->schLink.compLinkState = TRUE;

   return CPropertyPage::OnWizardFinish();
}



////////////////////////////////////////////////////////////////////////////////////////////
// CXrefEditorDlg dialog
////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXrefEditorDlg, CDialog)
CXrefEditorDlg::CXrefEditorDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CXrefEditorDlg::IDD, pParent)
   , m_DropDownOption(FALSE)
{
   SavedName = "User Defined";
   lastDropDownOption = TRUE;
   filterSettings.FileReloaded = FALSE;
}

CXrefEditorDlg::~CXrefEditorDlg()
{
}

void CXrefEditorDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
//   DDX_Control(pDX, IDC_VSFLEX, m_flexGrid);
   DDX_Radio(pDX, IDC_DROPDOWN_SETTING, m_DropDownOption);
}


BEGIN_MESSAGE_MAP(CXrefEditorDlg, CDialog)
   ON_BN_CLICKED(IDC_SHOWMATCHES, OnBnClickedShowmatches)
   ON_BN_CLICKED(IDC_CLEARMATCHES, OnBnClickedClearmatches)
   ON_BN_CLICKED(IDC_CLEARALL, OnBnClickedClearall)
   ON_BN_CLICKED(IDC_SAVEXRF, OnBnClickedSavexrf)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
   ON_BN_CLICKED(IDC_SETUNMATCHED_BYNAME, OnBnClickedSetunmatchedByname)
END_MESSAGE_MAP()


// CXrefEditorDlg message handlers

BOOL CXrefEditorDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   CRect rc;
   BSTR licenseKey = NULL;

   // TODO: License the flex grid
   if (!GetLicenseString(m_flexGrid.GetClsid(), licenseKey))
      licenseKey = NULL;

   GetDlgItem(IDC_OCXBOX)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_OCXBOX)->GetWindowRect(&rc);
   ScreenToClient(&rc);
   m_flexGrid.Create("FlexGrid", WS_VISIBLE, rc, this, IDC_VSFLEX,
                      NULL, FALSE, licenseKey);

   m_flexGrid.put_Editable(CFlexGrid::flexEDNone);
   m_flexGrid.put_ExplorerBar(CFlexGrid::flexExSortShow);
   m_flexGrid.put_AllowSelection(TRUE);
   m_flexGrid.put_SelectionMode(CFlexGrid::flexSelectionByRow);
   m_flexGrid.put_AllowBigSelection(FALSE);
   m_flexGrid.put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   m_flexGrid.put_GridLines(CFlexGrid::flexGridFlat);
   m_flexGrid.put_ExtendLastCol(TRUE);
   m_flexGrid.put_WordWrap(FALSE);
   m_flexGrid.put_AutoSearch(CFlexGrid::flexSearchFromCursor);
   m_flexGrid.put_AutoResize(TRUE);
   m_flexGrid.put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);

   m_flexGrid.put_Cols(2);
   m_flexGrid.put_Rows(1);
   m_flexGrid.put_FixedCols(0);
   m_flexGrid.put_FixedRows(1);
   COleVariant row1, col1, newValue;
   row1 = (GRID_CNT_TYPE)0;
   col1 = (GRID_CNT_TYPE)SCH_COL;
   newValue = "Schematic Name";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);

   row1 = (GRID_CNT_TYPE)0;
   col1 = (GRID_CNT_TYPE)PCB_COL;
   newValue = "Cross Reference Name";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);

   //if (forCompData)
   //{
   //}
   //else
   //{
   //   COleVariant row1, col1, newValue;
   //   row1 = (GRID_CNT_TYPE)0;
   //   col1 = (GRID_CNT_TYPE)SCH_NET_COL;
   //   newValue = "Schematic Name";
   //   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);

   //   row1 = (GRID_CNT_TYPE)0;
   //   col1 = (GRID_CNT_TYPE)SCH_FILTER_COL;
   //   newValue = "Schematic Filter Name";
   //   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);

   //   row1 = (GRID_CNT_TYPE)0;
   //   col1 = (GRID_CNT_TYPE)PCB_COL;
   //   newValue = "Cross Reference Name";
   //   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);

   //   row1 = (GRID_CNT_TYPE)0;
   //   col1 = (GRID_CNT_TYPE)PCB_FILTER_COL;
   //   newValue = "Cross Reference Filter Name";
   //   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);
   //}

   //filterSettings.ReadFile("C:\\Development\\CAMCAD\\Samples\\SchematicLink\\filterTest.txt");
   fillGrid();
   getComboBoxString();

   COleVariant equal((GRID_CNT_TYPE)AFX_OLE_FALSE), extra((GRID_CNT_TYPE)0);
   col1 = (GRID_CNT_TYPE)SCH_COL;
   m_flexGrid.AutoSize(SCH_COL, col1, equal, extra);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CXrefEditorDlg::setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue)
{
   COleVariant row1(row), col1(col), newValue(itemValue);

   if (row >= m_flexGrid.get_Rows())
      m_flexGrid.put_Rows(row+1);

   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);
}

void CXrefEditorDlg::fillGrid()
{
   CMapStringToString strMap;
   CString temp;

   // If there was a cross reference loaded, make sure the matches show up
   m_flexGrid.put_Rows(xRefArray[SCH_XREF].GetCount()+1);
   for (int cnt=0; cnt<xRefArray[SCH_XREF].GetCount(); cnt++)
   {
      CString itemName = xRefArray[SCH_XREF].GetAt(cnt);

      // Don't allow duplicates
      if (strMap.Lookup(itemName, temp))
         continue;
      strMap.SetAt(itemName, itemName);

      // add
      long row = strMap.GetCount();
      setItemToGrid(row, SCH_COL, itemName);
      //setItemToGrid(row, SCH_FILTER_COL, filterSettings.CheckName(itemName, forCompData, TRUE));

      // See if there is a match
      itemName = xRefArray[PCB_XREF].GetAt(cnt);
      if (!itemName.IsEmpty())
      {
         // Populate if there is a match
         setItemToGrid(row, PCB_COL, itemName);
         //setItemToGrid(row, PCB_FILTER_COL, filterSettings.CheckName(itemName, forCompData, FALSE));
      }
   }

   if (forCompData)
   {
      // fill the cross reference list with the current shematic data
      for (int i=0; i<mSchCompArray->GetCount(); i++)
      {
         SchCompData *compData = mSchCompArray->GetAt(i);

         if (strMap.Lookup(compData->CompareName, temp))
            continue;
         strMap.SetAt(compData->CompareName, compData->CompareName);

         long row = strMap.GetCount();
         setItemToGrid(row, SCH_COL, compData->CompareName);
         //setItemToGrid(row, SCH_FILTER_COL, filterSettings.CheckName(compData->CompareName, forCompData, TRUE));
      }
   }
   else
   {
      // fill the cross reference list with the current shematic data
      for (int i=0; i<mSchNetArray->GetCount(); i++)
      {
         SchNetData *netData = mSchNetArray->GetAt(i);

         if (strMap.Lookup(netData->CompareName, temp))
            continue;
         strMap.SetAt(netData->CompareName, netData->CompareName);

         long row = strMap.GetCount();
         setItemToGrid(row, SCH_COL, netData->CompareName);
         //setItemToGrid(row, SCH_FILTER_COL, filterSettings.CheckName(netData->CompareName, forCompData, TRUE));
      }
   }
   strMap.RemoveAll();
}

int CompareStrings(const void *param1, const void *param2)
{
   return strcmp(*((char**)param1), *((char**)param2));
}

CString CXrefEditorDlg::getComboBoxString()
{
   UpdateData();
   
   if (lastDropDownOption == m_DropDownOption)
      return comboBoxString;

   char **temp = NULL;
   int itemCnt = 0;
   if (forCompData)
   {
      for (int pcbCnt=0;pcbCnt<mPcbCompArray->GetCount(); pcbCnt++)
      {
         SchCompData *compData = mPcbCompArray->GetAt(pcbCnt);

         // If we are set to only populate unused items, make sure we only
         // add those without matches
         if (m_DropDownOption == 1 && compData->indexMatch >= 0) 
            continue;

         itemCnt++;
      }

      temp = (char**)calloc(itemCnt, sizeof(char*));
      itemCnt = 0;
      for (int pcbCnt=0;pcbCnt<mPcbCompArray->GetCount(); pcbCnt++)
      {
         SchCompData *compData = mPcbCompArray->GetAt(pcbCnt);

         // If we are set to only populate unused items, make sure we only
         // add those without matches
         if (m_DropDownOption == 1 && compData->indexMatch >= 0) 
            continue;

         temp[itemCnt++] = STRDUP(compData->CompareName);
         //temp.Add(compData->CompareName);
      }
   }
   else
   {
      for (int pcbCnt=0;pcbCnt<mPcbNetArray->GetCount(); pcbCnt++)
      {
         SchNetData *netData = mPcbNetArray->GetAt(pcbCnt);

         // If we are set to only populate unused items, make sure we only
         // add those without matches
         if (m_DropDownOption == 1 && netData->indexMatch >= 0) 
            continue;

         itemCnt++;
      }

      temp = (char**)calloc(itemCnt, sizeof(char*));
      itemCnt = 0;
      for (int pcbCnt=0;pcbCnt<mPcbNetArray->GetCount(); pcbCnt++)
      {
         SchNetData *netData = mPcbNetArray->GetAt(pcbCnt);

         // If we are set to only populate unused items, make sure we only
         // add those without matches
         if (m_DropDownOption == 1 && netData->indexMatch >= 0) 
            continue;

         temp[itemCnt++] = STRDUP(netData->CompareName);
         //temp.Add(netData->CompareName);
      }
   }

   if (!temp)
      return "";

   // Sort all the names for the combo box
   qsort((void*)temp, (size_t)itemCnt, sizeof(char*), CompareStrings);

   comboBoxString.Empty();
   for (int cnt=0; cnt<itemCnt; cnt++)
      comboBoxString += (CString)temp[cnt] + "|";
   comboBoxString.TrimRight("|");
   lastDropDownOption = m_DropDownOption;

   for (int cnt=0; cnt<itemCnt; cnt++)
      delete temp[cnt];
   free(temp);

   return comboBoxString;
}

void CXrefEditorDlg::SetData(CStringArray *schArray, CStringArray *pcbArray)
{
   if (!schArray || !pcbArray) return;

   xRefArray[SCH_XREF].RemoveAll();
   xRefArray[PCB_XREF].RemoveAll();

   xRefArray[SCH_XREF].Append(*schArray);
   xRefArray[PCB_XREF].Append(*pcbArray);
}

BOOL CXrefEditorDlg::GetXRefData(long index, CString &schName, CString &pcbName)
{
   if (index < 0 || index >= xRefArray[SCH_XREF].GetCount())
      return FALSE;

   schName = xRefArray[SCH_XREF].GetAt(index);
   pcbName = xRefArray[PCB_XREF].GetAt(index);

   return TRUE;
}

void CXrefEditorDlg::OnBnClickedShowmatches()
{
   COleVariant item, row((GRID_CNT_TYPE)m_flexGrid.get_FixedRows()), col((GRID_CNT_TYPE)SCH_COL),
      caseSense((GRID_CNT_TYPE)TRUE), fullMatch((GRID_CNT_TYPE)TRUE);
   long foundItem = 0;

   if (forCompData)
   {
      for (int schCnt=0;schCnt<mSchCompArray->GetCount(); schCnt++)
      {
         SchCompData *compData = mSchCompArray->GetAt(schCnt);
         if (compData->indexMatch < 0) 
            continue; // There was no match for this item

         item = compData->CompareName;
         foundItem = m_flexGrid.get_FindRow(item, row, col, caseSense, fullMatch);
         if (foundItem < 0) 
            continue;  // Didn't find it

         compData = mPcbCompArray->GetAt(compData->indexMatch);
      
         COleVariant row1((GRID_CNT_TYPE)foundItem), col1((GRID_CNT_TYPE)PCB_COL), newValue;
         
         // See if there is a value currently assigned
         CString itemValue = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1);

         // Only if there isn't already a value do I take the defaulted values from the comparison
         if (itemValue.IsEmpty())
         {
            newValue = compData->CompareName;
            setItemToGrid(foundItem, PCB_COL, compData->CompareName);
            //setItemToGrid(foundItem, PCB_FILTER_COL, filterSettings.CheckName(compData->CompareName, forCompData, FALSE));
         }
      }
   }
   else
   {
      for (int schCnt=0;schCnt<mSchNetArray->GetCount(); schCnt++)
      {
         SchNetData *netData = mSchNetArray->GetAt(schCnt);
         if (netData->indexMatch < 0) 
            continue; // There was not match for this item

         item = netData->CompareName;
         foundItem = m_flexGrid.get_FindRow(item, row, col, caseSense, fullMatch);
         if (foundItem < 0) 
            continue;  // Didn't find it

         netData = mPcbNetArray->GetAt(netData->indexMatch);
      
         COleVariant row1((GRID_CNT_TYPE)foundItem), col1((GRID_CNT_TYPE)PCB_COL), newValue;

         // See if there is a value currently assigned
         CString itemValue = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1);

         // Only if there isn't already a value do I take the defaulted values from the comparison
         if (itemValue.IsEmpty())
         {
            newValue = netData->CompareName;
            setItemToGrid(foundItem, PCB_COL, netData->CompareName);
            //setItemToGrid(foundItem, PCB_FILTER_COL, filterSettings.CheckName(netData->CompareName, forCompData, FALSE));
         }
      }
   }
}


void CXrefEditorDlg::OnBnClickedSetunmatchedByname()
{
   COleVariant item, row((GRID_CNT_TYPE)m_flexGrid.get_FixedRows()), col((GRID_CNT_TYPE)SCH_COL),
      caseSense((GRID_CNT_TYPE)TRUE), fullMatch((GRID_CNT_TYPE)TRUE);
   long foundItem = 0;

   // we want to get the available ref names to check if there are any matches
   int option = m_DropDownOption;
   m_DropDownOption = 1;
   CString comboString= (CString)"|" + getComboBoxString() + "|";
   comboString.MakeUpper();
   m_DropDownOption = option;

   if (forCompData)
   {
      COleVariant row, col;
      for (long cnt=1; cnt<m_flexGrid.get_Rows(); cnt++)
      {
         CString schName, xRefName;

         row = (GRID_CNT_TYPE)cnt;
         col = (GRID_CNT_TYPE)SCH_COL;
         COleVariant schText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

         col = (GRID_CNT_TYPE)PCB_COL;
         COleVariant xRefText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

         // if it's empty, we want to see if there are any available matches
         if (((CString)xRefText).IsEmpty())
         {
            CString searchString = (CString)"|" + (CString)schText + "|";
            searchString.MakeUpper();
            if (comboString.Find(searchString) >= 0)
               setItemToGrid(cnt, PCB_COL, (CString)schText);
         }
      }
   }
   else
   {
      COleVariant row, col;
      for (long cnt=1; cnt<m_flexGrid.get_Rows(); cnt++)
      {
         CString schName, xRefName;

         row = (GRID_CNT_TYPE)cnt;
         col = (GRID_CNT_TYPE)SCH_COL;
         COleVariant schText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

         col = (GRID_CNT_TYPE)PCB_COL;
         COleVariant xRefText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

         // if it's empty, we want to see if there are any available matches
         if (((CString)xRefText).IsEmpty())
         {
            CString searchString = (CString)"|" + (CString)schText + "|";
            searchString.MakeUpper();
            if (comboString.Find(searchString) >= 0)
               setItemToGrid(cnt, PCB_COL, (CString)schText);
         }
      }
   }
}

void CXrefEditorDlg::OnBnClickedClearmatches()
{
   COleVariant item, row((GRID_CNT_TYPE)m_flexGrid.get_FixedRows()), col((GRID_CNT_TYPE)SCH_COL), 
      caseSense((short)TRUE), fullMatch((short)TRUE);
   long foundItem = 0;

   if (forCompData)
   {
      for (int schCnt=0;schCnt<mSchCompArray->GetCount(); schCnt++)
      {
         SchCompData *compData = mSchCompArray->GetAt(schCnt);
         if (compData->indexMatch < 0 || compData->xrfMatch) 
            continue; // There was not match for this item

         item = compData->CompareName;
         foundItem = m_flexGrid.get_FindRow(item, row, col, caseSense, fullMatch);
         if (foundItem < 0) 
            continue;  // Didn't find it

         COleVariant row1((GRID_CNT_TYPE)foundItem), col1((GRID_CNT_TYPE)PCB_COL), oldValue;
         
         // See if there is a value currently assigned
         oldValue = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1);
         compData = mPcbCompArray->GetAt(compData->indexMatch);
         
         // Clear only if the matched item is the same as the one defined in the grid, 
         // otherwise it has been redefined
         if (!compData->CompareName.Compare((CString)oldValue))
         {
            setItemToGrid(foundItem, PCB_COL, "");
            //setItemToGrid(foundItem, PCB_FILTER_COL, "");
         }
      }
   }
   else
   {
      for (int schCnt=0;schCnt<mSchNetArray->GetCount(); schCnt++)
      {
         SchNetData *netData = mSchNetArray->GetAt(schCnt);
         if (netData->indexMatch < 0 || netData->xrfMatch) 
            continue; // There was not match for this item

         item = netData->CompareName;
         foundItem = m_flexGrid.get_FindRow(item, row, col, caseSense, fullMatch);
         if (foundItem < 0) 
            continue;  // Didn't find it

         COleVariant row1((GRID_CNT_TYPE)foundItem), col1((GRID_CNT_TYPE)PCB_COL), oldValue;

         // See if there is a value currently assigned
         oldValue = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1);
         netData = mPcbNetArray->GetAt(netData->indexMatch);

         // Clear only if the matched item is the same as the one defined in the grid, 
         // otherwise it has been redefined
         if (!netData->CompareName.Compare((CString)oldValue))
         {
            setItemToGrid(foundItem, PCB_COL, "");
            //setItemToGrid(foundItem, PCB_FILTER_COL, "");
         }
      }
   }
}

void CXrefEditorDlg::OnBnClickedClearall()
{
   for (long cnt=1; cnt<m_flexGrid.get_Rows(); cnt++)
   {
      setItemToGrid(cnt, PCB_COL, "");
      //setItemToGrid(cnt, PCB_FILTER_COL, "");
   }
}

void CXrefEditorDlg::OnBnClickedSavexrf()
{
   CFileDialog fileDlg(FALSE, ".xrf", NULL, NULL,
                     "Cross Reference Files (*.xrf)|*.xrf|All Files (*.*)|*.*||", this);
   if (fileDlg.DoModal() == IDCANCEL)
      return;

   CStdioFile xrfFile;
   CString filename = fileDlg.GetPathName();
   if (!xrfFile.Open(filename, CFile::modeCreate|CFile::modeWrite))
   {
      AfxMessageBox("Failed to open file for writing.");
      return;
   }

   CTime now = CTime::GetCurrentTime();
   xrfFile.WriteString(now.Format("; %b %d, %Y %I:%M:%S %p\n"));
   xrfFile.WriteString("; Schematic Link Component Cross Reference File\n");
   xrfFile.WriteString(";\n");
   xrfFile.WriteString("; Schematic RefDes              PCB RefDes\n");

   CString buf;
   COleVariant row, col;
   for (long cnt=1; cnt<m_flexGrid.get_Rows(); cnt++)
   {
      CString schName, xRefName;

      row = (GRID_CNT_TYPE)cnt;
      col = (GRID_CNT_TYPE)SCH_XREF;
      COleVariant schText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

      col = (GRID_CNT_TYPE)PCB_XREF;
      COleVariant xRefText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

      // Only write the cross reference if there is one
      if (!((CString)xRefText).IsEmpty())
      {
         buf.Format("  %-30s %s\n", (CString)schText, (CString)xRefText);
         xrfFile.WriteString(buf);
      }
   }
   xrfFile.Close();

   SavedName = filename;
}

void CXrefEditorDlg::OnBnClickedOk()
{
   xRefArray[SCH_XREF].RemoveAll();
   xRefArray[PCB_XREF].RemoveAll();

   CString buf;
   COleVariant row, col;
   for (long cnt=1; cnt<m_flexGrid.get_Rows(); cnt++)
   {
      CString schName, xRefName;

      row = (GRID_CNT_TYPE)cnt;
      col = (GRID_CNT_TYPE)SCH_COL;
      COleVariant schText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

      col = (GRID_CNT_TYPE)PCB_COL;
      COleVariant xRefText = m_flexGrid.get_Cell(CFlexGrid::flexcpText, row, col, row, col);

      // Only update the cross reference if there is one
      if (!((CString)xRefText).IsEmpty())
      {
         xRefArray[SCH_XREF].Add((CString)schText);
         xRefArray[PCB_XREF].Add((CString)xRefText);
      }
   }

   OnOK();
}


void CXrefEditorDlg::OnBnClickedCancel()
{
   xRefArray[PCB_XREF].RemoveAll();

   OnCancel();
}

BEGIN_EVENTSINK_MAP(CXrefEditorDlg, CDialog)
   ON_EVENT(CXrefEditorDlg, IDC_VSFLEX, DISPID_KEYPRESS, KeyPressVsflex, VTS_PI2)
   ON_EVENT(CXrefEditorDlg, IDC_VSFLEX, DISPID_KEYUP, KeyUpVsflex, VTS_PI2 VTS_I2)
   ON_EVENT(CXrefEditorDlg, IDC_VSFLEX, DISPID_MOUSEUP, MouseUpVsflex, VTS_I2 VTS_I2 VTS_R4 VTS_R4)
   ON_EVENT(CXrefEditorDlg, IDC_VSFLEX, 28, ChangeEditVsflex, VTS_NONE)
END_EVENTSINK_MAP()

void CXrefEditorDlg::KeyPressVsflex(short* KeyAscii)
{
CString buf;
buf.Format("Key Press = %i", *KeyAscii);
AfxMessageBox(buf);
   if (CFlexGrid::flexEDKbdMouse == m_flexGrid.get_Editable() &&
      *KeyAscii == 127)
   {
      COleVariant row, col;
      row = (GRID_CNT_TYPE)m_flexGrid.get_Row();
      col = (GRID_CNT_TYPE)PCB_COL;
      m_flexGrid.put_Cell(CFlexGrid::flexcpText, row, col, row, col, COleVariant(""));
   }
}

void CXrefEditorDlg::KeyUpVsflex(short* KeyCode, short Shift)
{
   if (*KeyCode == 32 /*space*/)
   {
      if (m_flexGrid.get_Row() > 0)
      {
         m_flexGrid.put_Col((GRID_CNT_TYPE)PCB_COL);
         m_flexGrid.put_ColComboList(PCB_COL, getComboBoxString());
         m_flexGrid.EditCell();
      }
   }
}

void CXrefEditorDlg::MouseUpVsflex(short Button, short Shift, float X, float Y)
{
   if (m_flexGrid.get_Col() == PCB_COL && m_flexGrid.get_Row() > 0)
   {
      m_flexGrid.put_ColComboList(PCB_COL, getComboBoxString());
      m_flexGrid.EditCell();
   }
}

void CXrefEditorDlg::ChangeEditVsflex()
{
   CString itemName = m_flexGrid.get_EditText();

   if (itemName.IsEmpty())
      return;

   // Update the filtered name
   //  we always know that they use can only change the PCB cross reference name, so we only
   // need to update the PCB filter column
   //setItemToGrid(m_flexGrid.get_Row(), PCB_FILTER_COL, filterSettings.CheckName(itemName, forCompData, FALSE));
}



////////////////////////////////////////////////////////////////////////////////////////////
// CSelectItem dialog
////////////////////////////////////////////////////////////////////////////////////////////
CSelectItem::CSelectItem(CWnd* pParent /*=NULL*/)
   : CResizingDialog(CSelectItem::IDD, pParent)
{
   SchLinkPtr = NULL;
   DocPtr = NULL;
   FilePtr = NULL;
   ForComps = FALSE;
   lastSelCompData = NULL;

   addFieldControl(IDCANCEL, anchorBottom);
   CResizingDialogField& okField = addFieldControl(IDOK, anchorBottomRight);
   CResizingDialogField& staticField = addFieldControl(IDC_OCXBOX, anchorLeft);
   staticField.getOperations().addOperation(glueBottomEdge, toTopEdge, &okField);
   staticField.getOperations().addOperation(glueRightEdge, toRightEdge, &okField);
}

CSelectItem::~CSelectItem()
{
}

void CSelectItem::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_OCXBOX, m_ocxStaGrid);
}


BEGIN_MESSAGE_MAP(CSelectItem, CResizingDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CSelectItem, CResizingDialog)
   ON_EVENT(CSelectItem, IDC_GRID, 1, SelChangeVsflex, VTS_NONE)
   ON_EVENT(CSelectItem, IDC_GRID, 7, AfterRowColChangeVsflex, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   ON_EVENT(CSelectItem, IDC_GRID, DISPID_MOUSEDOWN, MouseDownBrdDataGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4)
END_EVENTSINK_MAP()

void CSelectItem::AfterRowColChangeVsflex(long oldRow, long oldCol, long newRow, long newCol)
{
   m_ocxStaGrid.restoreWindowPlacement();
}

void CSelectItem::MouseDownBrdDataGrid()
{
   m_ocxStaGrid.restoreWindowPlacement();
}

// CSelectItem message handlers
BOOL CSelectItem::OnInitDialog()
{
   CResizingDialog::OnInitDialog();


   // Create the grids
   m_ocxStaGrid.substituteForDlgItem(IDC_GRID, IDC_OCXBOX, "FlexGrid1", *this);

   CExtendedFlexGrid* flexGrid = m_ocxStaGrid.getGrid();
   flexGrid->put_AllowBigSelection(FALSE);
   flexGrid->put_AllowSelection(TRUE);
   flexGrid->put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   flexGrid->put_Appearance(CFlexGrid::flex3D);
   flexGrid->put_BackColor(GRID_COLOR_WHITE);         
   flexGrid->put_BackColorAlternate(GRID_COLOR_IVORY);
   flexGrid->put_BackColorBkg(GRID_COLOR_GRAY);    
   flexGrid->put_Editable(CFlexGrid::flexEDNone);
   flexGrid->put_ExtendLastCol(TRUE);
   flexGrid->put_FocusRect(CFlexGrid::flexFocusInset);
   flexGrid->put_HighLight(CFlexGrid::flexHighlightAlways);
   flexGrid->put_SelectionMode(CFlexGrid::flexSelectionListBox);
   flexGrid->put_Rows(1);


   flexGrid->put_Cols(1);
   flexGrid->put_Rows(1);
   flexGrid->put_FixedCols(0);
   flexGrid->put_FixedRows(0);

   GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

   fillGrid();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectItem::fillGrid()
{
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   CExtendedFlexGrid* flexGrid = m_ocxStaGrid.getGrid();

   if (ForComps)
   {
      flexGrid->put_Rows(compArray.GetCount());
      for (long cnt=0; cnt<compArray.GetCount(); cnt++)
      {
         SchCompData *compData = compArray[cnt];
         BlockStruct *block = DocPtr->getBlockAt(compData->Sheet);

         COleVariant row((GRID_CNT_TYPE)cnt), col((GRID_CNT_TYPE)0), newValue;
         newValue = pApp->schLink.ApplyFilters(compData->CompareName, TRUE, !DocPtr->schLinkPCBDoc) + " in " + block->getName();

         flexGrid->put_Cell(CFlexGrid::flexcpText, row, col, row, col, newValue);
      }
   }
   else
   {
      flexGrid->put_Rows(netArray.GetCount());
      for (long cnt=0; cnt<netArray.GetCount(); cnt++)
      {
         SchNetData *netData = netArray[cnt];
         BlockStruct *block = DocPtr->getBlockAt(netData->Sheet);

         COleVariant row((GRID_CNT_TYPE)cnt), col((GRID_CNT_TYPE)0), newValue;
         newValue = pApp->schLink.ApplyFilters(netData->CompareName, FALSE, !DocPtr->schLinkPCBDoc) + " in " + block->getName();

         flexGrid->put_Cell(CFlexGrid::flexcpText, row, col, row, col, newValue);
      }
   }
}

int CSelectItem::AddItem(VOID *itemPtr)
{
   if (ForComps)
   {
      SchCompData *compData = (SchCompData*)itemPtr;

      // Make sure this is a schematic component.  Schematic items must have
      // a sheet that it belongs on.
      if (compData->Sheet < 0) return -1;

      return compArray.Add(compData);
   }
   else
   {
      SchNetData *netData = (SchNetData*)itemPtr;

      // Make sure this is a schematic component.  Schematic items must have
      // a sheet that it belongs on.
      if (netData->Sheet < 0) return -1;

      for (int i=0; i<netArray.GetCount(); i++)
      {
         SchNetData *tmpNetData = (SchNetData*)netArray.GetAt(i);
         if (tmpNetData->Sheet == netData->Sheet)  
            return i;
      }

      return netArray.Add(netData);
   }
}

void CSelectItem::SelChangeVsflex()
{
   BOOL fitPage = FALSE;
   CExtendedFlexGrid* flexGrid = m_ocxStaGrid.getGrid();

   if (ForComps)
   {
      SchCompData *compData = compArray[flexGrid->get_Row()];

      
      if (FilePtr->getBlock()->getBlockNumber() != compData->Sheet)
      {
         // set sheet and zoom to the component
         FilePtr->setBlock(DocPtr->getBlockAt(compData->Sheet));
         DocPtr->OnFitPageToImage();
      }

      if (lastSelCompData)
         lastSelCompData->data->setColorOverride(false);

      SchLinkPtr->ZoomToComp(DocPtr, compData);

      lastSelCompData = compData;
   }
   else
   {
      SchNetData *netData = netArray[flexGrid->get_Row()];

      if (FilePtr->getBlock()->getBlockNumber() != netData->Sheet)
      {
         // set sheet, highlight and zoom to the net
         FilePtr->setBlock(DocPtr->getBlockAt(netData->Sheet));
         DocPtr->OnFitPageToImage();
      }

      DocPtr->HighlightedNetsMap.RemoveAll();
      SchLinkPtr->ZoomToNet(DocPtr, netData);
   }
}



////////////////////////////////////////////////////////////////////////////////////////////
// CFilter class
////////////////////////////////////////////////////////////////////////////////////////////
CFilter::CFilter()
{
   logOpen = FALSE;
   FileReloaded = FALSE;
   KillTrailingAlphas = FALSE;
   ClearAllData();
}

CFilter::~CFilter()
{
   ClearAllData();
}

int CFilter::GetCmdCount()
{
   return sCmd_Lst.GetCount();
}

void CFilter::ClearAllData()
{
   // Make sure there is data to clear
   if (sCmd_Lst.GetCount() <= 0)
      return;

   CString logFilename = GetLogfilePath("clearFilter.log");
   logOpen = logFile.Open(logFilename, CFile::modeCreate|CFile::modeWrite);
   CTime now = CTime::GetCurrentTime();
   writeLog("%s\n", now.Format(" %b %d, %Y %I:%M:%S %p"));
   writeLog("Schematic Link Filter Clear\n");
   writeLog("\n");
   writeLog("\n");

   CString buf;
   for (int i=0; i<sCmd_Lst.GetCount(); i++)
   {
      sCmd_Struct *sCmd = sCmd_Lst[i];

      writeLog("Removing Command...\n");
      writeLog("   File Type = %c\n", sCmd->fileType);
      writeLog("   Data Type = %c\n", sCmd->dataType);
      writeLog("   Find Arg = %s\n", sCmd->findArg);
      writeLog("   Replace Arg = %s\n", sCmd->replaceArg);

      sCmd_Lst[i] = NULL;
      delete sCmd;
   }
   sCmd_Lst.RemoveAll();
   if (logOpen) logFile.Close();

   KillTrailingAlphas = FALSE;
   FileReloaded = TRUE;
}

int CFilter::sCmd_Add(CString cmd)
{
//.s:s:n  old_sch_net_name new_sch_net_name
//.s:p:n  old_pcb_net_name new_pcb_net_name
//.s:s:c  old_sch_comp_name new_sch_comp_name
//.s:p:c  old_pcb_comp_name new_pcb_comp_name

//.x:s:n  sch_net_find_string sch_net_replace_string
//.x:p:n  pcb_net_find_string pcb_net_replace_string
//.x:s:n  sch_comp_find_string sch_comp_replace_string
//.x:p:n  pcb_comp_find_string pcb_comp_replace_string

   CString tCmd, curField;
   CString command, fileType, dataType, findArg, replaceArg;
   int loc = 0;

   CString buf;
   if (cmd[0] != '.')
   {
      writeLog("Not a command [%s]\n", cmd);
      return -1;
   }

   // Remove all additional spaces
   tCmd = cmd.Mid(1);

   // Get the command type
   curField = tCmd.Tokenize(" \t.:", loc);
   command = curField;
   if (command.GetLength() != 1)
   {
      writeLog("No command [%s]\n", cmd);
      return -1;
   }
   if (command.CompareNoCase("s") &&      // full substitute
       command.CompareNoCase("x"))        // partial substitute
   {
      // Unsupported command
      writeLog("Unsupported command [%s]\n", command);
      return -1;
   }

   // Get the file type
   curField = tCmd.Tokenize(" \t.:", loc);
   fileType = curField;
   if (fileType.GetLength() != 1)
   {
      writeLog("No find type [%s]\n", cmd);
      return -1;
   }
   if (fileType.CompareNoCase("p") &&     // pcb
       fileType.CompareNoCase("s"))       // schematic
   {
      // Unsupported file type
      writeLog("Unsupported file type [%s]\n", fileType);
      return -1;
   }

   // Get the data type
   curField = tCmd.Tokenize(" \t.:", loc);
   dataType = curField;
   if (dataType.GetLength() != 1)
   {
      writeLog("No find type [%s]\n", cmd);
      return -1;
   }
   if (dataType.CompareNoCase("n"))       // nets
   {
      // Unsupported data type
      writeLog("Unsupported data type [%s]\n", dataType);
      return -1;
   }

   // Get the find argument
   curField = tCmd.Tokenize(" \t.:", loc);
   findArg = curField;
   if (findArg.GetLength() < 1)
   {
      writeLog("No find type [%s]\n", cmd);
      return -1;
   }

   // Get the replace argument
   curField = tCmd.Tokenize(" \t.:", loc);
   replaceArg = curField;
   if (replaceArg.GetLength() < 1)
   {
      writeLog("No find type [%s]\n", cmd);
      return -1;
   }

   sCmd_Struct *sCmd = new sCmd_Struct;
   sCmd->cmd = command[0];
   sCmd->fileType = fileType[0];
   sCmd->dataType = dataType[0];
   sCmd->findArg= findArg;
   sCmd->replaceArg = (replaceArg=="\"\"")?"":replaceArg;

   writeLog("Adding %s\n", cmd);
   writeLog("   Command = %c\n", sCmd->cmd);
   writeLog("   File Type = %c\n", sCmd->fileType);
   writeLog("   Data Type = %c\n", sCmd->dataType);
   writeLog("   Find Arg = %s\n", sCmd->findArg);
   writeLog("   Replace Arg = %s\n", sCmd->replaceArg);

   return sCmd_Lst.Add(sCmd);
}

int CFilter::ReadFile(CString filterPath)
{
   BOOL haveUnknownCmds = FALSE;
   CStdioFile file;
   CFileException e;
   CString logFilename = GetLogfilePath("filterRead.log");

   logOpen = logFile.Open(logFilename, CFile::modeCreate|CFile::modeWrite);
   CTime now = CTime::GetCurrentTime();
   writeLog("%s\n", now.Format(" %b %d, %Y %I:%M:%S %p"));
   writeLog("Schematic Link Filter Command File\n");
   writeLog("\n");
   writeLog("\n");

   if (!file.Open(filterPath, CFile::modeRead|CFile::shareDenyWrite, &e))
   {
      CString errMsg = (CString)"Error opening file.";  // + e.GetErrorMessage(
      writeLog("Error opening file.");
      return ERR_NOTFOUND;
   }
   FileReloaded = TRUE;

   CString buf, line, schName, pcbName;
   while (file.ReadString(line))
   {
      if (line[0] != '.')
      {
         writeLog("Reading [%s]\n", line);
         continue;
      }

      if (sCmd_Add(line) == -1)
      {
         writeLog("   Command not added [%s]\n", line);
         haveUnknownCmds = TRUE;
      }
      else
         writeLog("   Added Command [%s]\n", line);
   }
   file.Close();
   if (logOpen) logFile.Close();

   if (haveUnknownCmds)
      Logreader(logFilename);

   return ERR_NOERROR;
}

CString CFilter::CheckName(CString src, BOOL Component, BOOL Schematic)
{
   CString name, dataType, fileType;

   // if we are dealing with components, then we don't need to use filter commands
   name = src;
   if (Component && KillTrailingAlphas)
   {
      while (name.GetLength() && isalpha(name[name.GetLength()]))
         name = name.Left(name.GetLength()-1);

      // If everything is an alpha and was removed, then the name should not change
      if (!name.GetLength())
         name = src;

      return name;
   }

   dataType = (Component?"c":"n");
   fileType = (Schematic?"s":"p");
   if (!sCmd_Lst.IsEmpty())
   {
      // Process full substutute commands
      for (int i=0; i<sCmd_Lst.GetCount(); i++)
      {
         sCmd_Struct *sCmd = sCmd_Lst[i];
         if (tolower(sCmd->dataType) == dataType[0] && tolower(sCmd->fileType) == fileType[0])
         {
            CString findName = sCmd->findArg;
            CString replName = sCmd->replaceArg;
            
            if (tolower(sCmd->cmd) == STP_SUB_FULL)
            {
               if (findName == name)
                  return replName;
            }
         }
      }
      
      // Process partial substutute commands
      for (int i=0; i<sCmd_Lst.GetCount(); i++)
      {
         sCmd_Struct *sCmd = sCmd_Lst[i];

         if (tolower(sCmd->dataType) == dataType[0] && tolower(sCmd->fileType) == fileType[0])
         {
            CString findName = sCmd->findArg;
            CString replName = sCmd->replaceArg;
            
            if (tolower(sCmd->cmd) == STP_SUB_PARTIAL)
            {
               while (name.Find(findName) >= 0)
                  name.Replace(findName, replName);
            }
         }
      }
   }

   return name;
}

void CFilter::writeLog(LPCSTR format, ...)
{
   if (logOpen)
   {
      CString buf;

      va_list arg_ptr;
      va_start(arg_ptr, format);

      buf.FormatV(format, arg_ptr);
      va_end(arg_ptr);

      logFile.WriteString(buf);
   }
}




////////////////////////////////////////////////////////////////////////////////////////////
// CSchematicSettings class
////////////////////////////////////////////////////////////////////////////////////////////
CSchematicSettings::CSchematicSettings()
{
   PcbMargin = 5;
   SchMargin = 5;
   SchHightlightColor = RGB(255, 255, 255);
   SchBusHightlightColor = RGB(255, 255, 255);
   PcbHightlightColor = RGB(255, 255, 255);
}

CSchematicSettings::~CSchematicSettings()
{
}

void CSchematicSettings::LoadSchSettings()
{
   CStdioFile file;
   CFileException e;
   CString filePath = getApp().getCamcadExeFolderPath() + "default.sch";

   if (!file.Open(filePath, CFile::modeRead|CFile::shareDenyWrite, &e))
      return;

   CString buf, line;
   while (file.ReadString(line))
   {
      int pos = 0;
      if (line[0] != '.')
         continue;

      line = line.Mid(1).Trim();

      buf = line.Tokenize(" \t", pos);
      
      if (!buf.CompareNoCase("PcbMargin"))
      {
         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         PcbMargin = atoi(buf);
      }
      else if (!buf.CompareNoCase("SchMargin"))
      {
         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         SchMargin = atoi(buf);
      }
      else if (!buf.CompareNoCase("SchematicHighLightColor"))
      {
         int red, green, blue;

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         red = atoi(buf);

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         green = atoi(buf);

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         blue = atoi(buf);

         SchHightlightColor = RGB(red, green, blue);
      }
      else if (!buf.CompareNoCase("SchematicBusHighLightColor"))
      {
         int red, green, blue;

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         red = atoi(buf);

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         green = atoi(buf);

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         blue = atoi(buf);

         SchBusHightlightColor = RGB(red, green, blue);
      }
      else if (!buf.CompareNoCase("PcbHighLightColor"))
      {
         int red, green, blue;

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         red = atoi(buf);

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         green = atoi(buf);

         if (pos < 0) continue;
         buf = line.Tokenize(" \t", pos);
         if (buf.IsEmpty()) continue;
         blue = atoi(buf);

         PcbHightlightColor = RGB(red, green, blue);
      }
   }
   file.Close();
}

void CSchematicSettings::SaveSchSettings()
{
   CStdioFile file;
   CFileException e;
   CString filePath = getApp().getCamcadExeFolderPath() + "default.sch";

   if (!file.Open(filePath, CFile::modeCreate|CFile::modeWrite, &e))
      return;

   file.WriteString("; Default.sch\n");
   file.WriteString("; \n");
   file.WriteString(";   This file specifies default value for schematic navigation.  If no values are\n");
   file.WriteString("; specified, then the default below will be used.\n");
   file.WriteString(";      PcbMargin = 5\n");
   file.WriteString(";      SchMargin = 5\n");
   file.WriteString(";      SchematicHighLightColor = RGB(255, 255, 255) ---> White\n");
   file.WriteString(";      SchematicBusHighLightColor = RGB(255, 255, 255) ---> White\n");
   file.WriteString(";      PcbHighLightColor = RGB(255, 255, 255) ---> White\n");
   file.WriteString("; \n");
   file.WriteString("; \n");
   file.WriteString("; \n");

   CString buf;

   file.WriteString("; PcbMargin - indicated the percentage margin used in the pcb file\n");
   buf.Format(".PcbMargin %i\n", PcbMargin);
   file.WriteString(buf);
   file.WriteString("; \n");
   file.WriteString("; \n");

   file.WriteString("; SchMargin - indicated the percentage margin used in the schematic file\n");
   buf.Format(".SchMargin %i\n", SchMargin);
   file.WriteString(buf);
   file.WriteString("; \n");
   file.WriteString("; \n");

   file.WriteString("; SchematicHighLightColor - indicated the color used when navigating with the schematic tools\n");
   buf.Format(".SchematicHighLightColor %i %i %i\n", GetRValue(SchHightlightColor), GetGValue(SchHightlightColor), GetBValue(SchHightlightColor));
   file.WriteString(buf);
   file.WriteString("; \n");
   file.WriteString("; \n");

   file.WriteString("; SchematicBusHighLightColor - indicated the color used when navigating with the schematic tools\n");
   buf.Format(".SchematicBusHighLightColor %i %i %i\n", GetRValue(SchBusHightlightColor), GetGValue(SchBusHightlightColor), GetBValue(SchBusHightlightColor));
   file.WriteString(buf);
   file.WriteString("; \n");
   file.WriteString("; \n");

   file.WriteString("; PcbHighLightColor - indicated the color used when navigating with the PCB tools\n");
   buf.Format(".PcbHighLightColor %i %i %i\n", GetRValue(PcbHightlightColor), GetGValue(PcbHightlightColor), GetBValue(PcbHightlightColor));
   file.WriteString(buf);
   file.WriteString("; \n");
   file.WriteString("; \n");

   file.Close();
}



////////////////////////////////////////////////////////////////////////////////////////////
// CSchLinkSettings dialog
////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSchLinkSettings, CDialog)
CSchLinkSettings::CSchLinkSettings(CWnd* pParent /*=NULL*/)
   : CDialog(CSchLinkSettings::IDD, pParent)
   , m_schMargin(_T(""))
   , m_pcbMargin(_T(""))
{
}

CSchLinkSettings::~CSchLinkSettings()
{
}

void CSchLinkSettings::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_SCH_MARGIN, m_schMargin);
   DDX_Text(pDX, IDC_PCB_MARGIN, m_pcbMargin);
   DDX_Control(pDX, IDC_SCH_HIGHLIGHT_COLOR, m_schHighlightColorBtn);
   DDX_Control(pDX, IDC_SCH_BUS_HIGHLIGHT_COLOR, m_schBusHighlightColorBtn);
   DDX_Control(pDX, IDC_PCB_HIGHLIGHT_COLOR, m_pcbHighlightColorBtn);
}


BEGIN_MESSAGE_MAP(CSchLinkSettings, CDialog)
END_MESSAGE_MAP()


// CSchLinkSettings message handlers
BOOL CSchLinkSettings::OnInitDialog()
{
   CDialog::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}







/////////////////////////////////////////////////////////////////////////////
// Schematic Link APIs
/////////////////////////////////////////////////////////////////////////////
extern CCEtoODBView *apiView; // from API.CPP

/******************************************************************************
* LoadSchematicNetCrossReference
*/
short API::LoadSchematicNetCrossReference(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBApp *app = (CCEtoODBApp*)AfxGetApp();

   int retVal = RC_NOT_ALLOWED;
   int versionNumber =  CamCadMajorVersion * 100 + CamCadMinorVersion;
   if (versionNumber >= 406)
   {
      retVal = RC_NOT_ALLOWED;
   }
   else
   {
      if (!app->schLinkState) 
      {
         CString errMsg;
         /*if (app->CheckAndSetSchematiclink(errMsg) < 0)
            return RC_NOT_ALLOWED;*/
      }

      retVal = app->schLink.LoadXRefFile(filename, FALSE, NULL, NULL);
      if (retVal < 0)
         return retVal;

      app->schLinkState = TRUE;

      // Always create a new one (seems each process is responsible for creating and destroying
      progress = new CProgressDlg("Schematic Link");
      progress->Create(AfxGetMainWnd());

      progress->ShowWindow(SW_SHOW);
      progress->SetStep(1);

      retVal = app->schLink.processNetXRef();

      progress->ShowWindow(SW_HIDE);
      if (progress->DestroyWindow())
      {
         delete progress;
         progress = NULL;
      }
   }

   return retVal;
}

/******************************************************************************
* LoadSchematicComponentCrossReference
*/
short API::LoadSchematicComponentCrossReference(LPCTSTR filename) 
{
#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBApp *app = (CCEtoODBApp*)AfxGetApp();

   int retVal = RC_NOT_ALLOWED;
   int versionNumber =  CamCadMajorVersion * 100 + CamCadMinorVersion;
   if (versionNumber >= 406)
   {
      retVal = RC_NOT_ALLOWED;
   }
   else
   {
      if (!app->schLinkState) 
      {
         CString errMsg;
         /*if (app->CheckAndSetSchematiclink(errMsg) < 0)
            return RC_NOT_ALLOWED;*/
      }

      retVal = app->schLink.LoadXRefFile(filename, TRUE, NULL, NULL);
      if (retVal < 0)
         return retVal;

      app->schLinkState = TRUE;

      // Always create a new one (seems each process is responsible for creating and destroying
      progress = new CProgressDlg("Schematic Link");
      progress->Create(AfxGetMainWnd());

      progress->ShowWindow(SW_SHOW);
      progress->SetStep(1);

      retVal = app->schLink.processCompXRef();

      progress->ShowWindow(SW_HIDE);
      if (progress->DestroyWindow())
      {
         delete progress;
         progress = NULL;
      }
   }

   return retVal;
}

/******************************************************************************
* CrossProbeSchematicComponent
*/
short API::CrossProbeSchematicComponent(LPCTSTR compName)
{
   CCEtoODBApp *app = (CCEtoODBApp*)AfxGetApp();
   CString matchedName;

   if (!app->schLink.compLinkState)
      return RC_GENERAL_ERROR;

   app->schLink.removeHighlightedNets();
   app->schLink.removeOverriddenComps();

   int retVal = app->schLink.GetMatchForPcb(compName, matchedName, TRUE);
   if (retVal <= 0)
      retVal = app->schLink.GetMatchForSch(matchedName, matchedName, TRUE);
   else
      return retVal;

   return retVal;
}

/******************************************************************************
* CrossProbeSchematicNet
*/
short API::CrossProbeSchematicNet(LPCTSTR netName)
{
   CCEtoODBApp *app = (CCEtoODBApp*)AfxGetApp();
   CString matchedName;

   if (!app->schLink.netLinkState)
      return RC_GENERAL_ERROR;

   app->schLink.removeHighlightedNets();
   app->schLink.removeOverriddenComps();

   int retVal = app->schLink.GetMatchForPcb(netName, matchedName, FALSE);
   if (retVal <= 0)
      retVal = app->schLink.GetMatchForSch(matchedName, matchedName, FALSE);
   else
      return retVal;

   return retVal;
}
