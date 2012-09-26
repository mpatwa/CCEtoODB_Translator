// $Header: /CAMCAD/4.6/read_wrt/ZukenCr5000Reader.cpp 103   6/15/07 4:25p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/

#include "StdAfx.h"
#include "RwLib.h"
#include "ZukenCr5000Reader.h"
#include "Vector2d.h"
#include "RwLib.h"
#include "InFile.h"
#include "CCEtoODB.h"
#include "StandardAperture.h"
#include "CcRepair.h"
#include "EnumIterator.h"
#include "CrcCalculator.h"
#include "Debug.h"
#include "Variant.h"
#include "PcbUtil.h"
#include "RwUiLib.h"
#include "direct.h"
#include <map>
using std::map;


#include "RwUiLib.h"
#define getCamCadData_Doc getCamCadData
#define getCamCadData_Database getCamCadData
#define new_DataStruct getCamCadData().getNewDataStruct

#define MaxPadstackDescriptorLength 200
#define AxisSize 5.
static CCEtoODBDoc* doc;
map<int, BlockStruct*> subBoardsMap;
#ifdef EnableNewZukenReader

void ReadCR5000 (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);

/* bnf specification changes
   - added padstackHoleType as parameter to padstack
*/

static bool s_debugRefDesFlag = false;

static void setDebugRefDesFlag(bool flag)
{
   s_debugRefDesFlag = flag;
}

static bool getDebugRefDesFlag()
{
   return s_debugRefDesFlag;
}

static void dumpDataList(CWriteFormat& writeFormat,CCamCadDatabase& camCadDatabase,const CDataList& dataList)
{
   for (CDataListIterator insertIterator(dataList,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();
      BlockStruct* geometry = camCadDatabase.getBlock(data->getInsert()->getBlockNumber());
      CBasesVector basesVector = data->getInsert()->getBasesVector();

      CString layerDescriptor;
      layerDescriptor.Format("%d",data->getLayerIndex());

      if (data->getLayerIndex() >= 0)
      {
         LayerStruct* layer = camCadDatabase.getLayerAt(data->getLayerIndex());

         if (layer == NULL)
         {
            layerDescriptor += " (NULL)";
         }
         else
         {
            layerDescriptor.AppendFormat(" (%s)",layer->getName());
         }
      }

      writeFormat.writef("entity=%d basesVector='%s' geometryName='%s', layer=%s\n",data->getEntityNumber(),basesVector.getDescriptor(),geometry->getName(),layerDescriptor); 
   }

   dataList.dump(writeFormat);

   writeFormat.writef("\n");
}

void ReadCR5000new(const char* path_buffer,CCEtoODBDoc* camCadDoc,FormatStruct* Format,int pageunits)
{   
   char  curdir[_MAX_PATH];
   bool currentDirectoryChanged = false;
   CFilePath ftfFilePath(path_buffer),pcfFilePath(path_buffer),pnfFilePath(path_buffer),dstFilePath(path_buffer);
   ftfFilePath.setExtension("ftf");
   pcfFilePath.setExtension("pcf");
   pnfFilePath.setExtension("pnf");
   dstFilePath.setExtension("dst");

   CFilePath panelFtfFilePath1(ftfFilePath);
   panelFtfFilePath1.setBaseFileName(panelFtfFilePath1.getBaseFileName() + "-panel");

   CFilePath panelFtfFilePath2(ftfFilePath);
   panelFtfFilePath2.setBaseFileName(panelFtfFilePath2.getBaseFileName() + "_panel");

   doc = camCadDoc;
   CCamCadDatabase camCadDatabase(*camCadDoc);
   CZukenCr5000Reader zukenReader(camCadDatabase,pcfFilePath.getBaseFileName());

   CString settingsFile( getApp().getImportSettingsFilePath("Cr5k.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nZuken CR5000 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   CFilePath settingsFilePath(settingsFile);

   CFilePath logFileDirectoryPath(path_buffer);
   CString logFileDirectory = logFileDirectoryPath.getDirectoryPath();
   zukenReader.setLogFileDirectoryPath(logFileDirectory);
   if(NULL == _getcwd( curdir, _MAX_PATH ))   
   {
      zukenReader.getLog().writef(PrefixWarning,"Problem getting current directory\n");
   }
   else
   {
      _chdir(ftfFilePath.getDirectoryPath());
      currentDirectoryChanged = true;
   }


   if (fileExists(settingsFilePath.getPath()))
   {
      zukenReader.loadSettings(settingsFilePath.getPath());
   }
   else
   {
      zukenReader.getLog().writef(PrefixWarning,"Could not find .in file for reading, '%s'\n",settingsFilePath.getPath());

      settingsFilePath.setBaseFileName("Cr5000");

      if (fileExists(settingsFilePath.getPath()))
      {
         zukenReader.loadSettings(settingsFilePath.getPath());
      }
      else
      {
         zukenReader.getLog().writef(PrefixWarning,"Could not find .in file for reading, '%s'\n",settingsFilePath.getPath());
      }
   }

   if (zukenReader.getOptionUseOldReader())
   {
      ReadCR5000(path_buffer,camCadDoc,Format,pageunits);
      return;
   }

   bool testFtfReadFlag = zukenReader.getOptionTestFtfRead();
   bool testPcfReadFlag = zukenReader.getOptionTestPcfRead();
   bool testPnfReadFlag = zukenReader.getOptionTestPnfRead();
   bool testDstReadFlag = false;

   if (testFtfReadFlag || testPcfReadFlag || testPnfReadFlag || testDstReadFlag)
   {
      if (testFtfReadFlag && fileExists(ftfFilePath.getPath()))
      {
         zukenReader.read(ftfFilePath.getPath(),ptokFtf);
      }

      if (testPcfReadFlag && fileExists(pcfFilePath.getPath()))
      {
         zukenReader.read(pcfFilePath.getPath(),ptokPcf);
      }

      if (testPnfReadFlag && fileExists(pnfFilePath.getPath()))
      {
         zukenReader.read(pnfFilePath.getPath(),ptokPnf);
      }

      if (testDstReadFlag && fileExists(dstFilePath.getPath()))
      {
         zukenReader.readDstFile(dstFilePath.getPath());
      }
   }
   else
   {
      if (fileExists(ftfFilePath.getPath()) &&
          fileExists(pcfFilePath.getPath())    )
      {
         if (zukenReader.read(ftfFilePath.getPath(),ptokFtf))
         {
            if (zukenReader.read(pcfFilePath.getPath(),ptokPcf))
            {
               bool hasPanelFlag = false;

               // Need to instantiate boards here because reading PNF may encounter "subboard" data,
               // which will check for existance of boards on the CCZ side, so need to put the PCBs
               // in place on the CCZ side (instantiate them) before reading PNF.
               zukenReader.initializeInstantiator();
               zukenReader.instantiateBoardData();               

               if (fileExists(pnfFilePath.getPath()))
               {
                  if (zukenReader.read(pnfFilePath.getPath(),ptokPnf))
                  {
                     hasPanelFlag = true;

                     if (zukenReader.getOptionEnablePanelFtfFile())
                     {
                        if (fileExists(panelFtfFilePath1.getPath()))
                        {
                           if (fileExists(panelFtfFilePath2.getPath()))
                           {
                              // both panel ftf files exist.  Use the first and log the fact
                              zukenReader.getLog().writef("Two panel ftf files exist.  Using '%s', ignoring '%s'\n",panelFtfFilePath1.getPath(),panelFtfFilePath2.getPath());
                           }

                           zukenReader.read(panelFtfFilePath1.getPath(),ptokPanelFtf);
                        }
                        else if (fileExists(panelFtfFilePath2.getPath()))
                        {
                           zukenReader.read(panelFtfFilePath2.getPath(),ptokPanelFtf);
                        }
                        else
                        {
                           zukenReader.getLog().writef("Neither panel ftf files exist ('%s' or '%s')\n",panelFtfFilePath1.getPath(),panelFtfFilePath2.getPath());
                        }
                     }
                  }
               }

               zukenReader.getCamCadDoc().generatePinLocations();

               if (hasPanelFlag)
               {
                  // dts0100628654 -- Need to re-init instantiator to get info from reading PNF.
                  // Need to re-init instantiator so it is able to instanciate panel items.
                  // Failure to do so causes missing items (such as tooling holes that are represented as "components").
                  zukenReader.initializeInstantiator();
                  zukenReader.instantiatePanelData();
               }

               if (fileExists(dstFilePath.getPath()))
               {
                  zukenReader.readDstFile(dstFilePath.getPath());
               }
            }

            if (zukenReader.getOptionEnableLayerMirroring())
            {
               zukenReader.performLayerMirroring();
            }

            //CCamCadDocRepairTool camCadDocRepairTool(*camCadDoc);
            //camCadDocRepairTool.setVerificationLogFilePath("c:\\camCadVerificationLog.htm");
            //camCadDocRepairTool.verifyAndRepairData();

            //camCadDoc->SaveDataFileAs("c:\\Debug.cc");

            if (zukenReader.getOptionPurgeUnusedGeometries())
            {
               camCadDoc->purgeUnusedWidthsAndBlocks(zukenReader.getOptionShowPurgeReport());
            }
         }
      }
      else if(fileExists(pnfFilePath.getPath()))
      {
         if (zukenReader.read(pnfFilePath.getPath(),ptokPnf))
         {         
            if(fileExists(ftfFilePath.getPath()))
               zukenReader.read(ftfFilePath.getPath(),ptokPanelFtf);
            else if (zukenReader.getOptionEnablePanelFtfFile())
            {
               if (fileExists(panelFtfFilePath1.getPath()))
               {
                  if (fileExists(panelFtfFilePath2.getPath()))
                  {
                     // both panel ftf files exist.  Use the first and log the fact
                     zukenReader.getLog().writef("Two panel ftf files exist.  Using '%s', ignoring '%s'\n",panelFtfFilePath1.getPath(),panelFtfFilePath2.getPath());
                  }

                  zukenReader.read(panelFtfFilePath1.getPath(),ptokPanelFtf);
               }
               else if (fileExists(panelFtfFilePath2.getPath()))
               {
                  zukenReader.read(panelFtfFilePath2.getPath(),ptokPanelFtf);
               }
               else
               {
                  zukenReader.getLog().writef("Neither panel ftf files exist ('%s' or '%s')\n",panelFtfFilePath1.getPath(),panelFtfFilePath2.getPath());
               }
            }
            zukenReader.initializeInstantiator();

            zukenReader.instantiatePanelData();
            
            if (fileExists(dstFilePath.getPath()))
            {
               zukenReader.readDstFile(dstFilePath.getPath());
            }

            if (zukenReader.getOptionEnableLayerMirroring())
            {
               zukenReader.performLayerMirroring();
            }

            if (zukenReader.getOptionPurgeUnusedGeometries())
            {
               camCadDoc->purgeUnusedWidthsAndBlocks(zukenReader.getOptionShowPurgeReport());
            }
         }         
      }
   }

   CCr5kTokenReader::releaseStaticData();
   if(currentDirectoryChanged)
      _chdir( curdir );
   CCamCadDocRepairTool camCadDocRepairTool(*camCadDoc);
   camCadDocRepairTool.setVerificationLogFilePath(GetLogfilePath("ZukenReaderCcVerificationLog.txt"));
   camCadDocRepairTool.verifyAndRepairData();

   zukenReader.dts0100713526_CleanUpGeomNames();

   if (zukenReader.getOptionConvertComplexApertures())
   {
      camCadDoc->OnConvertComplexApertureNonZeroWidthPolys();
   }

   if (zukenReader.GetConsolidatorControl().GetIsEnabled())
   {
      CGeometryConsolidator squisher(camCadDoc, "" /*no report*/); 
      squisher.Go( &zukenReader.GetConsolidatorControl() );
   }
}

//_____________________________________________________________________________
//void removeQuotes(CString& string)
//{
//   string.Trim();
//
//   if (string.GetLength() > 1)
//   {
//      if (string.Left(1) == "\"" && string.Right(1) == "\"")
//      {
//         string = string.Mid(1,string.GetLength() - 2);
//      }
//   }
//}

//_____________________________________________________________________________
CString cr5kSourceFileTypeTagToString(Cr5kSourceFileTypeTag tagValue)
{
   const char* pString = "";

   switch (tagValue)
   {
   case sourceFileTypeFtf:        pString = "Ftf";  break;
   case sourceFileTypePcf:        pString = "Pcf";  break;
   case sourceFileTypePnf:        pString = "Pnf";  break;
   default:
   case sourceFileTypeUndefined:  pString = "???";  break;
   }

   return CString(pString);
}

//_____________________________________________________________________________
CCr5kReadElement::CCr5kReadElement(CCr5kElement& element,Cr5kTokenTag token,TokenMatchCountTypeTag matchCountType)
: m_element(element)
, m_token(token)
, m_matchCountType(matchCountType)
{
}

//_____________________________________________________________________________
#define distance(i,j) (distanceArray[(i) + (j)*(n1 + 1)])

int editDistance(const CString& string1,const CString& string2)
{
   const char* s1 = (const char*)string1;
   const char* s2 = (const char*)string2;
   int n1 = string1.GetLength();
   int n2 = string2.GetLength();

   const int insertUnitCost = 1;
   const int deleteUnitCost = 1;
   const int changeUnitCost = 1;

   CString distanceArrayString;
   char* distanceArray = distanceArrayString.GetBufferSetLength((n1+1)*(n2+1));

   for (int i = 0;i <= n1;i++)
   {
      distance(i,0) = i*deleteUnitCost;
   }

   for (int j = 0;j <= n2;j++)
   {
      distance(0,j) = j*insertUnitCost;
   }

   int cost,insertCost,deleteCost,changeCost;

   for (int i = 0;i < n1;i++)
   {
      for (int j = 0;j < n2;j++)
      {
         insertCost = distance(i    ,j + 1) + insertUnitCost;
         deleteCost = distance(i + 1,j    ) + deleteUnitCost;
         changeCost = distance(i    ,j    ) + ((s1[i] != s2[j]) ? changeUnitCost : 0);

         cost = min(min(insertCost,deleteCost),changeCost);

         distance(i + 1,j + 1) = cost;
      }
   }

   int retval = distance(n1,n2);

   return retval;
}

#undef distance

//_____________________________________________________________________________
CString getPadstackGeometryName(const CString& padstackDescriptor)
{
   CString padstackGeometryName;

   if (padstackDescriptor.GetLength() > MaxPadstackDescriptorLength)
   {
      CCrcCalculator crcCalculator;
      crcCalculator.input(padstackDescriptor);
      unsigned int crc = crcCalculator.getCrc();

      padstackGeometryName.Format("PS_%08x",crc);
   }
   else
   {
      padstackGeometryName = "PS_" + padstackDescriptor;
   }

   return padstackGeometryName;
}

bool IsPCBPresent(CString pcbName,CCEtoODBDoc* doc, FileStruct** pcbFile)
{
   if(NULL == doc)
      return false;

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (NULL != pos)
   {      
      FileStruct *file = doc->getFileList().GetNext(pos);
      if(NULL == file)
         continue;
      
      if((file->getBlockType() == BLOCKTYPE_PCB)  && (!file->getName().CompareNoCase(pcbName)))
      {
         *pcbFile = file;
         return true;
      }      
   }
   return false;
}
//_____________________________________________________________________________
CBoolStack::CBoolStack(int growBy)
{
   m_stack.SetSize(0,growBy);
}

void CBoolStack::empty()
{
   m_stack.RemoveAll();
}

void CBoolStack::push(bool flag)
{
   m_stack.Add(flag);
}

bool CBoolStack::pop()
{
   bool retval = false;

   if (!isEmpty())
   {
      int topIndex = m_stack.GetSize() - 1;

      retval = m_stack.GetAt(topIndex);

      m_stack.RemoveAt(topIndex);
   }

   return retval;
}

bool CBoolStack::getTop() const
{
   bool retval = false;

   if (!isEmpty())
   {
      retval = m_stack.GetAt(m_stack.GetSize() - 1);
   }

   return retval;
}

bool CBoolStack::getAt(int index) const
{
   bool retval = false;

   if (index >= 0 && index < getSize())
   {
      retval = m_stack.GetAt(index);
   }

   return retval;
}

int CBoolStack::getSize() const
{
   return m_stack.GetSize();
}

bool CBoolStack::isEmpty() const
{
   return (getSize() == 0);
}

//_____________________________________________________________________________
CCr5kReadElementList::CCr5kReadElementList()
{
}

void CCr5kReadElementList::takeElement(CCr5kReadElementList& otherList,POSITION otherListPos)
{
   CCr5kReadElement* readElement = otherList.m_list.GetAt(otherListPos);
   otherList.m_list.RemoveAt(otherListPos);

   m_list.AddTail(readElement);
}

void CCr5kReadElementList::add(CCr5kElement& element,Cr5kTokenTag token,TokenMatchCountTypeTag matchCountType)
{
   CCr5kReadElement* readElement = new CCr5kReadElement(element,token,matchCountType);

   m_list.AddTail(readElement);
}

void CCr5kReadElementList::addOne(CCr5kElement& element,Cr5kTokenTag token)
{
   add(element,token,matchCountOne);
}

void CCr5kReadElementList::addZeroOrOne(CCr5kElement& element,Cr5kTokenTag token)
{
   add(element,token,matchCountZeroOrOne);
}

//void CCr5kReadElementList::addZeroOrMore(CCr5kElement& element,Cr5kTokenTag token)
//{
//   add(element,token,matchCountZeroOrMore);
//}

//_____________________________________________________________________________
CCr5kTokenReaderToken::CCr5kTokenReaderToken()
: m_linePosition(0)
, m_lineNumber(0)
, m_filePosition(0)
{
}

CCr5kTokenReaderToken::CCr5kTokenReaderToken(const CString& token,const CString& line,int linePosition,int lineNumber,ULONGLONG filePosition)
: m_token(token)
, m_line(line)
, m_linePosition(linePosition)
, m_lineNumber(lineNumber)
, m_filePosition(filePosition)
{
}

CCr5kTokenReaderToken& CCr5kTokenReaderToken::operator=(const CCr5kTokenReaderToken& other)
{
   if (&other != this)
   {
      m_token        = other.m_token;
      m_line         = other.m_line;
      m_linePosition = other.m_linePosition;
      m_lineNumber   = other.m_lineNumber;
      m_filePosition = other.m_filePosition;
   }

   return *this;
}

void CCr5kTokenReaderToken::set(const CString& token,const CString& line,int linePosition,int lineNumber,ULONGLONG filePosition)
{
   m_token        = token;
   m_line         = line;
   m_linePosition = linePosition;
   m_lineNumber   = lineNumber;
   m_filePosition = filePosition;
}

//_____________________________________________________________________________
CCr5kTokenReaderTokenStack::CCr5kTokenReaderTokenStack()
{
   push(CCr5kTokenReaderToken());

   m_lastOperation = opUndefined;
}

CCr5kTokenReaderToken* CCr5kTokenReaderTokenStack::getTop()
{
   CCr5kTokenReaderToken* topReaderToken = NULL;

   if (m_tokenStack.GetCount() > 0)
   {
      topReaderToken = m_tokenStack.GetTail();
   }

   return topReaderToken;
}

void CCr5kTokenReaderTokenStack::push(const CCr5kTokenReaderToken& currentReaderToken)
{
   CCr5kTokenReaderToken* readerToken = new CCr5kTokenReaderToken(currentReaderToken);
   m_tokenStack.AddTail(readerToken);

   m_lastOperation = opPush;
}

void CCr5kTokenReaderTokenStack::replaceTop(const CCr5kTokenReaderToken& currentReaderToken)
{
   if (m_tokenStack.GetCount() > 0)
   {
      CCr5kTokenReaderToken* topReaderToken = m_tokenStack.GetTail();

      *topReaderToken = currentReaderToken;

      m_lastOperation = opReplaceTop;
   }
}

void CCr5kTokenReaderTokenStack::pop()
{
   if (m_tokenStack.GetCount() > 1)
   {
      CCr5kTokenReaderToken* readerToken = m_tokenStack.RemoveTail();

      delete readerToken;

      m_lastOperation = opPop;
   }
   else
   {
      formatMessageBoxApp("Logic error in '%s'",__FUNCTION__);
   }
}

void CCr5kTokenReaderTokenStack::restoreLast(const CCr5kTokenReaderToken& previousReaderToken)
{
   switch (m_lastOperation)
   {
   case opPush:
      pop();
      break;
   case opPop:
      push(previousReaderToken);
      break;
   case opReplaceTop:
      replaceTop(previousReaderToken);
      break;
   default:
      formatMessageBoxApp("Logic error in '%s'",__FUNCTION__);
      break;
   }

   m_lastOperation = opUndefined;

   CCr5kTokenReaderToken* topOfStack = m_tokenStack.GetTail();
}

void CCr5kTokenReaderTokenStack::setFunctionName(const CString& functionName)
{
   if (m_tokenStack.GetCount() > 0)
   {
      CCr5kTokenReaderToken* topReaderToken = m_tokenStack.GetTail();

      topReaderToken->setFunctionName(functionName);
   }
}

void addSpaces(CString& string,int spaceCount)
{
   CString space(" ");

   while (--spaceCount >= 0)
   {
      string += space;
   }
}

#define Cr5000TabSize 8

int untabify(CString& string)
{
   CString spaces;
   addSpaces(spaces,Cr5000TabSize);

   int replaceCount = string.Replace("\t",spaces);

   return replaceCount;
}

void CCr5kTokenReaderTokenStack::write(CWriteFormat& writeFormat)
{
   int previousLineNumber = 0;

   if (m_tokenStack.GetSize() > 1)
   {
      POSITION pos = m_tokenStack.GetHeadPosition();
      m_tokenStack.GetNext(pos);

      for (int index = 1;pos != NULL;index++)
      {
         CCr5kTokenReaderToken* readerToken = m_tokenStack.GetNext(pos);
         int indent = 10 + readerToken->getLinePosition();

         if (readerToken->getLineNumber() != previousLineNumber)
         {
            CString line(readerToken->getLine());
            int tabCount = untabify(line);

            indent += tabCount * (Cr5000TabSize - 1);

            writeFormat.writef("%8d: '%s'\n",readerToken->getLineNumber(),line);

            previousLineNumber = readerToken->getLineNumber();
         }

         CString message;
         addSpaces(message,indent);
         message.AppendFormat("'%s'",readerToken->getToken());
         addSpaces(message,max(3,70 - message.GetLength()));
         message.Append(readerToken->getFunctionName());

         writeFormat.writef("%s\n",message);
      }
   }
}

//_____________________________________________________________________________
CCr5kTokenMap::CCr5kTokenMap()
{
   init();
}

void CCr5kTokenMap::init()
{
   m_map.InitHashTable(nextPrime2n(max(300,(int)(tok_Cr5kTokenTagUndefined/.8))));

#define tok(a) m_map.SetAt(CString(#a    ).MakeLower(),tok##a); \
               m_map.SetAt(CString("(" #a).MakeLower(),ptok##a);
#define tok2(a,b) m_map.SetAt(CString(a).MakeLower(),tok##b);

#include "ZukenCr5000ReaderTokens.h"

#undef tok
#undef tok2
}

bool CCr5kTokenMap::lookup(CString tokenString,Cr5kTokenTag& tokenTag)
{
   bool retval = false;

   WORD tokenWord;
   tokenString.MakeLower();

   if (m_map.Lookup(tokenString,tokenWord))
   {
      tokenTag = (Cr5kTokenTag)tokenWord;

      retval = true;
   }
   else
   {
      tokenTag = tok_Cr5kTokenTagUndefined;
   }

   return retval;
}

//_____________________________________________________________________________
CCr5kTokenReaderException::CCr5kTokenReaderException(const CString& reason)
: m_reason(reason)
{
}

//_____________________________________________________________________________
CCr5kTokenMap* CCr5kTokenReader::s_tokenMap = NULL;

CCr5kTokenReader::CCr5kTokenReader(CZukenCr5000Reader& zukenReader,const CString& filePath)
: m_zukenReader(zukenReader)
, m_filePath(filePath)
, m_sourceFileType(sourceFileTypeUndefined)
, m_unTokenFlag(false)
, m_eofFlag(false)
, m_nextFunctionNameSetCount(1)
, m_optionWriteDebugInfo(false)
, m_optionMultiByteCharactersSupported(zukenReader.getOptionSupportMultiByteCharacters())
, m_units(pageUnitsUndefined)
{
   if (!m_file.Open(m_filePath,CFile::modeRead | CFile::shareDenyNone))
   {
      m_eofFlag = true;
   }

   CString status;
   status.Format("Reading '%s'",filePath);

   m_readProgress = new CFileReadProgress(m_file);
   m_readProgress->updateStatus(status);

   m_linePosition = 0;
   m_lineNumber = 0;
}

CCr5kTokenReader::~CCr5kTokenReader()
{
   delete m_readProgress;
}

CWriteFormat& CCr5kTokenReader::getLog() const
{
   return m_zukenReader.getLog();
}

void CCr5kTokenReader::setUnits(Cr5kTokenTag token)
{
   PageUnitsTag portUnits = pageUnitsUndefined;

   switch (token)
   {
   case tokMm:      portUnits = pageUnitsMilliMeters;       break;
   case tokInch:    portUnits = pageUnitsInches;            break;
   case tokMil:     portUnits = pageUnitsMils;              break;
   case tokDbUnit:  portUnits = pageUnitsCentiMicroMeters;  break;
   }

   if (portUnits == pageUnitsUndefined)
   {
      getLog().writef(PrefixError,"Unrecognized unit token '%s' encountered.",cr5kTokenToString(token));
   }
   else
   {
      m_units = m_zukenReader.getCamCadDoc().getUnits();
      m_units.setPortUnits(portUnits);
   }
}

CCr5kTokenMap& CCr5kTokenReader::getTokenMap()
{
   if (s_tokenMap == NULL)
   {
      s_tokenMap = new CCr5kTokenMap();
   }

   return *s_tokenMap;
}

bool CCr5kTokenReader::isNonTerminatingToken(Cr5kTokenTag token)
{
   bool terminatingToken = (token == tokRightParen || token == tok_Eof);

   return !terminatingToken;
}

bool CCr5kTokenReader::isNonListToken(const CString& tokenString)
{
   bool retval = true;

   if (! tokenString.IsEmpty())
   {
      char firstChar = tokenString.GetAt(0);

      retval = (firstChar != '(' && firstChar != ')');
   }

   return retval;
}

bool CCr5kTokenReader::isNonTerminatingToken()
{
   return isNonTerminatingToken(getCurrentTokenTag());
}

bool CCr5kTokenReader::isNonListToken()
{
   // DR 725207 - This has to operate on the original token, including the double
   // quote delimiters if there were any. I fact, especially if. Because otherwise
   // an unfortunate original like "(abc def)" turns into (abc def) and just like
   // here it turns into a regular part of format syntax. And the "(" happens to
   // open a list. But it might not be a list, it might be a literal string.
   // So check the original raw token here.
   return isNonListToken(getCurrentRawTokenString());
}

bool CCr5kTokenReader::isBoolean(const CString& tokenString,bool& validValue)
{
   bool retval = true;

   Cr5kTokenTag token = stringToCr5kToken(tokenString);

   switch (token)
   {
   case tokTrue:
   case tokOn:
   case tokYes:
   case tokEnable:
      validValue = true;

      break;
   case tokFalse:
   case tokOff:
   case tokNo:
   case tokDisable:
      validValue = false;

      break;
   default:
      retval = false;

      break;
   }

   return retval;
}

bool CCr5kTokenReader::verifyBoolean(const CString& tokenString)
{
   bool retval = false;

   if (! isBoolean(tokenString,retval))
   {
      CString message;
      message.Format("Expected boolean token, but '%s' found.",tokenString);

      getLog().writef(PrefixError,"%s\n",message);

      m_tokenStack.write(getLog());

      //throwException(message);
   }

   return retval;
}

bool CCr5kTokenReader::isInteger(const CString& tokenString,int& validValue)
{
   char* endChar;
   int value =  strtol(tokenString,&endChar,10);

   bool retval = (*endChar == '\0');

   if (retval)
   {
      validValue = value;
   }

   return retval;
}

int CCr5kTokenReader::verifyInteger(const CString& tokenString)
{
   int retval = 0;

   if (! isInteger(tokenString,retval))
   {
      CString message;
      message.Format("Expected integer token, but '%s' found.",tokenString);

      getLog().writef(PrefixError,"%s\n",message);

      m_tokenStack.write(getLog());

      //throwException(message);
   }

   return retval;
}

bool CCr5kTokenReader::isFloatingPoint(const CString& tokenString,double& validValue)
{
   char* endChar;
   double value = strtod(tokenString,&endChar);

   bool retval = (*endChar == '\0');

   if (retval)
   {
      validValue = value;
   }

   return retval;
}

double CCr5kTokenReader::verifyFloatingPoint(const CString& tokenString)
{
   double retval = 0.;

   if (! isFloatingPoint(tokenString,retval))
   {
      CString message;
      message.Format("Expected floating point token, but '%s' found.",tokenString);

      getLog().writef(PrefixError,"%s\n",message);

      m_tokenStack.write(getLog());

      //throwException(message);
   }

   return retval;
}

bool CCr5kTokenReader::verifyBoolean()
{
   return verifyBoolean(m_token);
}

int CCr5kTokenReader::verifyInteger()
{
   return verifyInteger(m_token);
}

double CCr5kTokenReader::verifyFloatingPoint()
{
   return verifyFloatingPoint(m_token);
}

bool CCr5kTokenReader::getOptionWriteDebugInfo() const
{
   return m_optionWriteDebugInfo;
}

Cr5kTokenTag CCr5kTokenReader::stringToCr5kToken(CString tokenString)
{
   Cr5kTokenTag token = tok_Cr5kTokenTagUndefined;
   tokenString.MakeLower();

   getTokenMap().lookup(tokenString,token);

   return token;
}

CString CCr5kTokenReader::cr5kTokenToString(Cr5kTokenTag token)
{
   const char* retval = "";

   switch (token)
   {
#define tok(a) case tok##a:   retval = #a;  break; \
               case ptok##a:  retval = "(" #a;  break;
#define tok2(a,b) case tok##b:   retval = a;  break;

#include "ZukenCr5000ReaderTokens.h"

#undef tok
#undef tok2
   }

   return CString(retval);
}

void CCr5kTokenReader::releaseStaticData()
{
   delete s_tokenMap;
   s_tokenMap = NULL;
}

void CCr5kTokenReader::getLine()
{
   if (!m_eofFlag)
   {
      if (! m_file.ReadString(m_lineBuffer))
      {
         m_eofFlag = true;
      }
      else
      {
         m_linePosition = 0;
         m_lineNumber++;
         m_readProgress->updateProgress();
      }
   }
}

Cr5kTokenTag CCr5kTokenReader::getCurrentTokenTag()
{
   Cr5kTokenTag tokenTag;

   if (m_eofFlag)
   {
      tokenTag = tok_Eof;
   }
   else
   {
      getTokenMap().lookup(m_token,tokenTag);
   }

   return tokenTag;
}

Cr5kTokenTag CCr5kTokenReader::getDefinedCurrentTokenTag()
{
   Cr5kTokenTag tokenTag;

   if (m_eofFlag)
   {
      tokenTag = tok_Eof;
   }
   else
   {
      getTokenMap().lookup(m_token,tokenTag);
   }

   if (tokenTag == tok_Eof || tokenTag == tok_Cr5kTokenTagUndefined)
   {
      expectedKnownTokenError(m_token);
   }

   return tokenTag;
}

Cr5kTokenTag CCr5kTokenReader::getNextTokenTag()
{
   getNextTokenString();
   return getCurrentTokenTag();
}

CString CCr5kTokenReader::getCurrentTokenString()
{
   return m_token;
}

CString CCr5kTokenReader::getCurrentRawTokenString()
{
   return m_rawToken;
}

bool CCr5kTokenReader::getNextRequiredToken(Cr5kTokenTag token)
{
   bool retval = (getNextTokenTag() == token);

   if (! retval)
   {
      expectedTokenError(token);
      ungetToken();
   }

   return retval;
}

bool CCr5kTokenReader::getNextRequiredBoolean(bool& validValue)
{
   bool retval = isBoolean(getNextTokenString(),validValue);

   if (! retval)
   {
      ungetToken();
   }

   return retval;
}

bool CCr5kTokenReader::getNextRequiredInteger(int& validValue)
{
   bool retval = isInteger(getNextTokenString(),validValue);

   if (! retval)
   {
      ungetToken();
   }

   return retval;
}

bool CCr5kTokenReader::getNextRequiredFloatingPoint(double& validValue)
{
   bool retval = isFloatingPoint(getNextTokenString(),validValue);

   if (! retval)
   {
      ungetToken();
   }

   return retval;
}

bool CCr5kTokenReader::getNextRequiredNonListToken(CString& tokenString)
{
   getNextTokenString();

   bool retval = isNonListToken();

   if (! retval)
   {
      expectedNonListTokenError(m_token);
      ungetToken();
   }
   else
   {
      tokenString = getCurrentTokenString();
   }

   return retval;
}

bool CCr5kTokenReader::getNextRequiredNonListDefinedToken(Cr5kTokenTag& token)
{
   getNextTokenString();

   bool retval = isNonListToken();

   if (! retval)
   {
      expectedNonListTokenError(m_token);
      ungetToken();
   }
   else
   {
      token = getDefinedCurrentTokenTag();

      retval = (token != tok_Eof && token != tok_Cr5kTokenTagUndefined);
   }

   return retval;
}

bool CCr5kTokenReader::getNextOptionalNonListToken()
{
   getNextTokenString();

   bool retval = isNonListToken();

   if (! retval)
   {
      ungetToken();
   }

   return retval;
}

bool CCr5kTokenReader::isSupportedMultiByteCharacter(const char* p)
{
   //static bool testedFlag = false;

   //if (!testedFlag)
   //{
   //   testedFlag = true;
   //   unsigned int c;

   //   for (int byte0 = 0;byte0 < 256;byte0++)
   //   {
   //      for (int byte1 = 0;byte1 < 256;byte1++)
   //      {
   //         c = byte0*256 + byte1;

   //         if (_ismbblead(c))
   //         {
   //            break;
   //         }

   //         int d = c << 16;

   //         if (_ismbblead(d))
   //         {
   //            break;
   //         }

   //         d |= c;

   //         if (_ismbblead(d))
   //         {
   //            break;
   //         }
   //      }
   //   }
   //}

   bool retval = false;

   if (m_optionMultiByteCharactersSupported)
   {
      int u = *((const unsigned char*)p);

      if ((u >= 0x81 && u <= 0x9f) || (u >= 0xe0 && u <= 0xfc))
      {
         retval = true;
      }
   }

   return retval;
}

CString CCr5kTokenReader::getNextTokenString()
{
   m_token.Empty();

   enum StateTag { sStart, sNormal, sEscape, sQuote, sEnd } state,escapeState;

   if (!m_eofFlag)
   {
      ULONGLONG filePosition = m_file.GetPosition();
      int linePosition;

      for (state = sStart;state != sEnd && !m_eofFlag;)
      {
         //if (m_linePosition < 0)
         //{
         //   int iii = 3;
         //}

         //if (m_linePosition > (m_lineBuffer.GetLength() + 1))
         //{
         //   int iii = 3;
         //}

         const char* p = m_lineBuffer.GetBuffer() + m_linePosition;

         switch (state)
         {
         case sStart:
            while (isspace(*p))
            {
               m_linePosition++;
               p++;
            }

            if (*p == ')')
            {
               linePosition = m_linePosition;
               m_linePosition++;
               m_token = ")";
               state = sEnd;
            }
            else if (*p == '\0' || *p == ';')
            {
               getLine();
               state = sStart;
            }
            else
            {
               linePosition = m_linePosition;
               state = sNormal;
            }

            break;
         case sNormal:
            if (isspace(*p) || *p == ')' || *p == '\0')
            {
               state = sEnd;
            }
            else if (*p == '\\')
            {
               m_token += *p;
               m_linePosition++;
               escapeState = state;
               state = sEscape;
            }
            else if (*p == '"')
            {
               m_token += *p;
               m_linePosition++;
               state = sQuote;
            }
            else
            {
               m_token += *p;
               m_linePosition++;

               if (isSupportedMultiByteCharacter(p))
               {
                  p++;

                  m_token += *p;
                  m_linePosition++;
               }
            }

            break;
         case sEscape:
            if (*p == '\0')
            {
               state = sEnd;
            }
            else
            {
               m_token += *p;
               m_linePosition++;

               if (isSupportedMultiByteCharacter(p))
               {
                  p++;

                  m_token += *p;
                  m_linePosition++;
               }

               state = escapeState;
            }

            break;
         case sQuote:
            if (*p == '\0')
            {
               m_token += '\n';
               getLine();
            }
            else if (*p == '\\')
            {
               m_token += *p;
               m_linePosition++;
               escapeState = state;
               state = sEscape;
            }
            else if (*p == '"')
            {
               m_token += *p;
               m_linePosition++;
               state = sNormal;
            }
            else
            {
               m_token += *p;
               m_linePosition++;

               if (isSupportedMultiByteCharacter(p))
               {
                  p++;

                  m_token += *p;
                  m_linePosition++;
               }
            }

            break;
         }
      }

      m_previousReaderToken = m_currentReaderToken;
      m_currentReaderToken.set(m_token,m_lineBuffer,linePosition,m_lineNumber,filePosition);
      m_unTokenFlag = true;
      m_rawToken = m_token;

      int tokenLen = m_token.GetLength();
      char firstTokenChar = ((tokenLen > 0) ? m_token.GetAt(0) : '\0');
      int indentCount = 0;

      if (firstTokenChar == '(')
      {
         m_tokenStack.push(m_currentReaderToken);
         indentCount = m_tokenStack.getSize();
      }
      else if (firstTokenChar == ')')
      {
         indentCount = m_tokenStack.getSize();
         m_tokenStack.pop();
      }
      else
      {
         if (firstTokenChar == '"' && tokenLen > 1 && m_token.Right(1) == "\"")
         {
            m_token = m_token.Mid(1,tokenLen - 2);
         }

         m_tokenStack.replaceTop(m_currentReaderToken);
         indentCount = m_tokenStack.getSize();
      }

      if (getOptionWriteDebugInfo())
      {
         writeDebugLine(PrefixGet,indentCount);
      }
   }

   return m_token;
}

void CCr5kTokenReader::ungetToken()
{
   if (m_unTokenFlag)
   {
      m_tokenStack.restoreLast(m_previousReaderToken);

      //m_token      = m_previousReaderToken.getToken();
      //m_lineBuffer = m_previousReaderToken.getLine();
      m_linePosition = m_currentReaderToken.getLinePosition();
      //m_lineNumber = m_previousReaderToken.getLineNumber();

      //m_file.Seek(m_previousReaderToken.getFilePosition(),CFile::begin);

      //m_currentReaderToken = m_previousReaderToken;

      m_unTokenFlag = false;

      if (getOptionWriteDebugInfo())
      {
         writeDebugLine(PrefixUnget,-1);
      }
   }
   else
   {
      formatMessageBoxApp("Logic error in '%s'",__FUNCTION__);
   }
}

CString CCr5kTokenReader::getNextRawTokenString()
{
   getNextTokenString();

   return m_rawToken;
}

void CCr5kTokenReader::writeDebugLine(int prefix,int indentCount)
{
   static int previousIndentCount = 0;

   if (indentCount < 0)
   {
      indentCount = previousIndentCount;
   }
   else
   {
      previousIndentCount = indentCount;
   }

   CString token;
   char firstChar = ((m_token.GetLength() > 0) ? m_token.GetAt(0) : '\0');

   token = ((firstChar != '(' && firstChar != ')') ? " " + m_token : m_token);

   CString debugLine;
   addSpaces(debugLine,indentCount);
   debugLine.Append(token);
   addSpaces(debugLine,max(3,70 - debugLine.GetLength()));
   debugLine.Append(m_functionName);
   debugLine.AppendFormat(" %d",getFunctionNameSetCount());

   m_zukenReader.getDebug().writef(prefix,"%7d:%3d %s\n",m_lineNumber,indentCount,debugLine);
}

bool CCr5kTokenReader::skipList()
{
   bool retval = true;
   int stackSize = m_tokenStack.getSize();

   if (stackSize > 1)
   {
      while (m_tokenStack.getSize() >= stackSize && !getEofFlag())
      {
         getNextTokenString();
      }
   }

   return retval;
}

bool CCr5kTokenReader::skipToEndOfList()
{
   bool retval = true;
   int stackSize = m_tokenStack.getSize();

   if (stackSize > 1)
   {
      while (m_tokenStack.getSize() >= stackSize)
      {
         getNextTokenString();
      }

      ungetToken();
   }

   return retval;
}

void CCr5kTokenReader::throwException(const CString& message)
{
   throw CCr5kTokenReaderException(message);
}

CString CCr5kTokenReader::getFunctionName() const
{
   return m_functionName;
}

// any remaining elements are logged to error file
bool CCr5kTokenReader::skipExpectedEmptyRemainderOfList()
{
   bool retval = true;
   int initialStackSize = m_tokenStack.getSize();

   if (initialStackSize > 1)
   {
      CString listToken = m_tokenStack.getTop()->getToken();

      for (int tokenCount = 0;;tokenCount++)
      {
         int stackSize = m_tokenStack.getSize();

         if (stackSize < initialStackSize)
         {
            break;
         }

         Cr5kTokenTag token = getNextTokenTag();

         if (tokenCount == 0 && token != tokRightParen)
         {
            retval = false;

            CString message;
            message.Format("Expected end of list '%s', but '%s' found.",listToken,getCurrentTokenString());

            getLog().writef(PrefixError,"%s\n",message);

            m_tokenStack.write(getLog());

            throwException(message);
         }

         //if (tokenCount == 1)
         //{
         //   retval = false;

         //   CString message;
         //   message.Format("Expected end of list, but '%s' found.",getCurrentTokenString());

         //   getLog().writef(PrefixError,"%s\n",message);

         //   m_tokenStack.write(getLog());

         //   throwException(message);
         //}
      }
   }

   return retval;
}

void CCr5kTokenReader::unexpectedTokenError(const char* functionName)
{
   CString message;
   message.Format("Unexpected token '%s' found in '%s'.",getCurrentTokenString(),functionName);

   getLog().writef(PrefixError,"%s\n",message);

   m_tokenStack.write(getLog());

   throwException(message);
}

void CCr5kTokenReader::expectedTokenError(const CString& token)
{
   CString message;
   message.Format("Expected token '%s', but '%s' found.",token,getCurrentTokenString());

   getLog().writef(PrefixError,"%s\n",message);

   m_tokenStack.write(getLog());

   throwException(message);
}

void CCr5kTokenReader::expectedTokenError(Cr5kTokenTag token)
{
   CString tokenString = cr5kTokenToString(token);

   expectedTokenError(tokenString);
}

void CCr5kTokenReader::expectedNonListTokenError(const CString& token)
{
   CString message;
   message.Format("Expected non list token, but '%s' found.",token);

   getLog().writef(PrefixError,"%s\n",message);

   m_tokenStack.write(getLog());

   throwException(message);
}

void CCr5kTokenReader::expectedKnownTokenError(const CString& token)
{
   CString message;
   message.Format("Expected known token, but '%s' found.",token);

   getLog().writef(PrefixError,"%s\n",message);

   m_tokenStack.write(getLog());

   throwException(message);
}

//bool CCr5kTokenReader::skipListIfDuplicate(void* element,const char* elementDescription)
//{
//}

//bool CCr5kTokenReader::isNextTokenEndOfList()
//{
//}

//bool CCr5kTokenReader::readExpectedElement(CCr5kElement& element,Cr5kTokenTag token)
//{
//}

// returns true if one element is read, false if zero elements are read
bool CCr5kTokenReader::readZeroOrOneElements(CCr5kElement& element,Cr5kTokenTag expectedToken)
{
   bool retval = false;

   Cr5kTokenTag token = getNextTokenTag();

   if (token == tokRightParen)
   {
      ungetToken();
   }
   else if (token != expectedToken && expectedToken != tok_Cr5kTokenTagUndefined)
   {
      ungetToken();
   }
   else
   {
      retval = element.read(*this);
   }

   return retval;
}

bool CCr5kTokenReader::readOneElement(CCr5kElement& element,Cr5kTokenTag expectedToken)
{
   bool retval = false;

   Cr5kTokenTag token = getNextTokenTag();

   if (token == tokRightParen)
   {
      expectedTokenError(token);

      ungetToken();
   }
   else if (token != expectedToken && expectedToken != tok_Cr5kTokenTagUndefined)
   {
      expectedTokenError(expectedToken);

      ungetToken();
   }
   else
   {
      retval = element.read(*this);
   }

   return retval;
}

// will read only one element from the list
bool CCr5kTokenReader::readOneFromList(CCr5kReadElementList& elementList)
{
   bool retval = false;

   Cr5kTokenTag token = getNextTokenTag();

   for (POSITION pos = elementList.getHeadPosition();pos != NULL;)
   {
      CCr5kReadElement* readElement = elementList.getNext(pos);

      if (readElement->getToken() == token)
      {
         retval = readElement->getElement().read(*this);
         break;
      }
   }

   return retval;
}

//bool CCr5kTokenReader::readZeroOrOneEachFromList(CCr5kReadElementList& elementList)
//{
//}

// 
bool CCr5kTokenReader::readList(CCr5kReadElementList& elementList)
{
   bool retval = false;
   bool missingFlag = false;
   CCr5kReadElementList foundElementList;

   while (! elementList.isEmpty())
   {
      Cr5kTokenTag token = getNextTokenTag();
      bool foundFlag = false;

      for (POSITION pos = elementList.getHeadPosition();pos != NULL;)
      {
         POSITION prevPos = pos;
         CCr5kReadElement* readElement = elementList.getNext(pos);

         if (readElement->getToken() == token)
         {
            retval = readElement->getElement().read(*this);
            foundFlag = true;
            foundElementList.takeElement(elementList,prevPos);

            break;
         }
         else if (readElement->getMatchCountType() == matchCountOne ||
                  readElement->getMatchCountType() == matchCountOneOrMore)
         {
            missingFlag = true;
         }
      }

      if (!foundFlag)
      {
         ungetToken();
         break;
      }
   }

   retval = retval && !missingFlag;

   return retval;
}

bool CCr5kTokenReader::readElementsAndParameters(CCr5kReadElementList& elementList,CCr5kParameters& parameters)
{
   while (true)
   {
      parameters.read(*this);

      if (elementList.isEmpty())
      {
         break;
      }

      int previousListCount = elementList.getCount();

      readList(elementList);

      int foundElementsCount = previousListCount - elementList.getCount();

      if (foundElementsCount <= 0)
      {
         break;
      }
   }

   return true;
}

bool CCr5kTokenReader::readElementsAndProperties(CCr5kReadElementList& elementList,CCr5kProperties& properties)
{
   while (true)
   {
      properties.read(*this);

      if (elementList.isEmpty())
      {
         break;
      }

      int previousListCount = elementList.getCount();

      readList(elementList);

      int foundElementsCount = previousListCount - elementList.getCount();

      if (foundElementsCount <= 0)
      {
         break;
      }
   }

   return true;
}

void CCr5kTokenReader::setFunctionName(const CString& functionName)
{
   m_functionName = functionName;
   m_functionNameSetCount =  m_nextFunctionNameSetCount++;

   m_tokenStack.setFunctionName(functionName);
}

void CCr5kTokenReader::resetFunctionName(const CString& functionName,int functionNameSetCount)
{
   m_functionName = functionName;
   m_functionNameSetCount = functionNameSetCount;
}

int CCr5kTokenReader::getFunctionNameSetCount() const
{
   return m_functionNameSetCount;
}

void CCr5kTokenReader::setSourceLineNumber(CCr5kElement& element) const
{
   element.setSourceLineNumber(m_sourceFileType,m_lineNumber);
}

//_____________________________________________________________________________
CCr5kTokenReaderFunction::CCr5kTokenReaderFunction(CCr5kTokenReader& tokenReader,const CString functionName)
: m_tokenReader(tokenReader)
{
   m_previousFunctionName = m_tokenReader.getFunctionName();
   m_previousFunctionNameSetCount = m_tokenReader.getFunctionNameSetCount();

   m_tokenReader.setFunctionName(functionName);
}

CCr5kTokenReaderFunction::~CCr5kTokenReaderFunction()
{
   m_tokenReader.resetFunctionName(m_previousFunctionName,m_previousFunctionNameSetCount);
}

//_____________________________________________________________________________
CCr5kProperty::CCr5kProperty(const CString& name,const CString& value)
: m_name(name)
, m_value(value)
{
}

void CCr5kProperty::setValue(const CString& value)
{
   m_value = value;
}

//_____________________________________________________________________________
CCr5kProperties::CCr5kProperties()
{
}

CCr5kProperties::~CCr5kProperties()
{
   empty();
}

void CCr5kProperties::empty()
{
   CString key;
   CCr5kProperty* property;

   for (POSITION pos = m_properties.GetStartPosition();pos != NULL;)
   {
      m_properties.GetNextAssoc(pos,key,property);

      delete property;
   }

   m_properties.RemoveAll();
}

void CCr5kProperties::setAt(const CString& name,const CString& value)
{
   CCr5kProperty* property = getAt(name);

   if (property == NULL)
   {
      property = new CCr5kProperty(name,value);
      m_properties.SetAt(name,property);
   }
   else
   {
      property->setValue(value);
   }
}

CCr5kProperty* CCr5kProperties::getAt(const CString& name)
{
   CCr5kProperty* property;

   if (! m_properties.Lookup(name,property))
   {
      property = NULL;
   }

   return property;
}

bool CCr5kProperties::hasProperty(const CString& name)
{
   bool retval = (getAt(name) != NULL);

   return retval;
}

int CCr5kProperties::getCount() const
{
   return m_properties.GetCount();
}

bool CCr5kProperties::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;
   CString propertyName,propertyValue;

   for (bool loopFlag = true;loopFlag;)
   {
      Cr5kTokenTag token = tokenReader.getNextTokenTag();

      switch (token)
      {
      case ptokProperty:
      case ptokPropertyS:
      case ptokPropertyI:
      case ptokPropertyF:
         if (! tokenReader.getNextRequiredNonListToken(propertyName))
         {
            tokenReader.skipList();
            return false;
         }

         if (! tokenReader.getNextRequiredNonListToken(propertyValue))
         {
            tokenReader.skipList();
            return false;
         }

         setAt(propertyName,propertyValue);

         tokenReader.skipExpectedEmptyRemainderOfList();

         if (token == ptokPropertyI)
         {
            tokenReader.verifyInteger(propertyValue);
         }
         else if (token == ptokPropertyF)
         {
            tokenReader.verifyFloatingPoint(propertyValue);
         }

         break;
      default:
         tokenReader.ungetToken();
         loopFlag = false;

         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
CCr5kParameter::CCr5kParameter(Cr5kTokenTag parameterKey,const CString& value)
: m_parameterKey(parameterKey)
, m_value(value)
{
}

void CCr5kParameter::setValue(const CString& value)
{
   m_value = value;
}

//_____________________________________________________________________________
CCr5kParameters::CCr5kParameters()
{
}

CCr5kParameters::~CCr5kParameters()
{
   empty();
}

void CCr5kParameters::empty()
{
   WORD key;
   CCr5kParameter* parameter;

   for (POSITION pos = m_parameters.GetStartPosition();pos != NULL;)
   {
      m_parameters.GetNextAssoc(pos,key,parameter);

      delete parameter;
   }

   m_parameters.RemoveAll();
}

void CCr5kParameters::registerKey(Cr5kTokenTag key)
{
   if (!hasKey(key))
   {
      m_parameters.SetAt(key,NULL);
   }
}

void CCr5kParameters::setAt(Cr5kTokenTag key,const CString& value)
{
   CCr5kParameter* parameter = getAt(key);

   if (parameter == NULL)
   {
      parameter = new CCr5kParameter(key,value);
      m_parameters.SetAt(key,parameter);
   }
   else
   {
      parameter->setValue(value);
   }
}

CCr5kParameter* CCr5kParameters::getAt(Cr5kTokenTag key)
{
   CCr5kParameter* parameter;

   if (! m_parameters.Lookup(key,parameter))
   {
      parameter = NULL;
   }

   return parameter;
}

bool CCr5kParameters::hasKey(Cr5kTokenTag key) const
{
   CCr5kParameter* parameter;

   bool retval = (m_parameters.Lookup(key,parameter) != 0);

   return retval;
}

bool CCr5kParameters::hasValueForKey(Cr5kTokenTag key) const
{
   CCr5kParameter* parameter;

   bool retval = (m_parameters.Lookup(key,parameter) != 0 && parameter != NULL);

   return retval;
}

int CCr5kParameters::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   int readCount = 0;

   while (true)
   {
      Cr5kTokenTag token = tokenReader.getNextTokenTag();

      if (hasKey(token))
      {
         CString parameterValue;

         if (! tokenReader.getNextRequiredNonListToken(parameterValue))
         {
            tokenReader.skipList();

            // if an error occurs, readCount should still be incremented
         }
         else
         {
            setAt(token,parameterValue);

            tokenReader.skipExpectedEmptyRemainderOfList();
         }

         readCount++; 
      }
      else
      {
         tokenReader.ungetToken();
         break;
      }
   }

   return readCount;
}

double CCr5kParameters::getDoubleValue(Cr5kTokenTag key,double defaultValue) const
{
   double retval;
   CCr5kParameter* parameter;

   if (! m_parameters.Lookup(key,parameter) || parameter == NULL)
   {
      retval = defaultValue;
   }
   else
   {
      retval = atof(parameter->getValue());
   }

   return retval;
}

int CCr5kParameters::getIntValue(Cr5kTokenTag key,int defaultValue) const
{
   int retval;
   CCr5kParameter* parameter;

   if (! m_parameters.Lookup(key,parameter) || parameter == NULL)
   {
      retval = defaultValue;
   }
   else
   {
      retval = atoi(parameter->getValue());
   }

   return retval;
}

bool CCr5kParameters::getBooleanValue(Cr5kTokenTag key,bool defaultValue) const
{
   bool retval;
   CCr5kParameter* parameter;

   if (! m_parameters.Lookup(key,parameter) || parameter == NULL)
   {
      retval = defaultValue;
   }
   else
   {
      CCr5kTokenReader::isBoolean(parameter->getValue(),retval);
   }

   return retval;
}

CString CCr5kParameters::getStringValue(Cr5kTokenTag key,const CString& defaultValue) const
{
   CString retval;
   CCr5kParameter* parameter;

   if (! m_parameters.Lookup(key,parameter) || parameter == NULL)
   {
      retval = defaultValue;
   }
   else
   {
      retval = parameter->getValue();
   }

   return retval;
}

Cr5kTokenTag CCr5kParameters::getTokenValue(Cr5kTokenTag key,Cr5kTokenTag defaultValue) const
{
   Cr5kTokenTag retval;
   CCr5kParameter* parameter;

   if (! m_parameters.Lookup(key,parameter) || parameter == NULL)
   {
      retval = defaultValue;
   }
   else
   {
      retval = CCr5kTokenReader::stringToCr5kToken(parameter->getValue());
   }

   return retval;
}

CString CCr5kParameters::getContentsString() const
{
   CString contentsString;
   CCr5kParameter* parameter;
   WORD key;
   Cr5kTokenTag token;

   for (POSITION pos = m_parameters.GetStartPosition();pos != NULL;)
   {
      m_parameters.GetNextAssoc(pos,key,parameter);

      if (parameter != NULL)
      {
         token = (Cr5kTokenTag)key;

         contentsString.AppendFormat("%s(%s)\n",CCr5kTokenReader::cr5kTokenToString(token),parameter->getValue());
      }
   }

   return contentsString;
}

//_____________________________________________________________________________
int CCr5kElement::m_nextId = 0;

CCr5kElement::CCr5kElement()
: m_loadedFlag(false)
, m_sourceLineNumber(0)
{
   m_id = m_nextId++;
}

CCr5kElement::~CCr5kElement()
{
   empty();
}

void CCr5kElement::setLoadedFlag()
{
   m_loadedFlag = true;
}

int CCr5kElement::getSourceLineNumber() const
{
   return m_sourceLineNumber;
}

CString CCr5kElement::getSourceLineDescriptor() const
{
   CString sourceLineNumber = cr5kSourceFileTypeTagToString(m_sourceFileType);
   sourceLineNumber.AppendFormat(".%d",m_sourceLineNumber);

   return sourceLineNumber;
}

void CCr5kElement::setSourceLineNumber(Cr5kSourceFileTypeTag sourceFileType,int sourceLineNumber)
{
   m_sourceFileType   = sourceFileType;
   m_sourceLineNumber = sourceLineNumber;
}

void CCr5kElement::empty()
{
   m_loadedFlag = false;
}

void CCr5kElement::resetNextId()
{
   m_nextId = 0;
}

//_____________________________________________________________________________
CCr5kSkippedElement::CCr5kSkippedElement()
{
}

bool CCr5kSkippedElement::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = tokenReader.skipList();

   return retval;
}

//_____________________________________________________________________________
CCr5kUnitDescriptor::CCr5kUnitDescriptor()
{
}

// bnf: unitDescriptor ::= '(' 'unit' { 'MM' | "INCH' | 'MIL' | 'DBUNIT' } ')'
bool CCr5kUnitDescriptor::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   switch (tokenReader.getNextTokenTag())
   {
   case tokMm:
   case tokInch:
   case tokMil:
   case tokDbUnit:
      m_unitToken = tokenReader.getDefinedCurrentTokenTag();

      break;
   default:
      tokenReader.unexpectedTokenError(__FUNCTION__);
      retval = false;

      break;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kVersion::CCr5kVersion()
{
}

// bnf: version ::= '(' 'version' versionString ')'
bool CCr5kVersion::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (tokenReader.isNonTerminatingToken(tokenReader.getNextTokenTag()))
   {
      m_versionString = tokenReader.getCurrentTokenString();
   }
   else
   {
      tokenReader.unexpectedTokenError(__FUNCTION__);

      retval = false;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kHeader::CCr5kHeader()
{
}

void CCr5kHeader::empty()
{
   m_version.empty();
   m_unitDescriptor.empty();

   CCr5kElement::empty();
}

// bnf: header ::= '(' 'header' version { <unitDescriptor> | <timeZone> } ')'
bool CCr5kHeader::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   // bnf: version ::= '(' 'version' versionString ')'
   retval = tokenReader.readOneElement(m_version,ptokVersion);

   CCr5kReadElementList elementList;

   // bnf: unitDescriptor ::= '(' 'unit' { 'MM' | "INCH' | 'MIL' | 'DBUNIT' } ')'
   elementList.addZeroOrOne(m_unitDescriptor,ptokUnit);

   // bnf: timeZone ::= '(' 'timeZone' string ')'
   CCr5kSkippedElement timeZone;
   elementList.addZeroOrOne(timeZone,ptokTimeZone);

   retval = tokenReader.readList(elementList) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   //tokenReader.getZukenReader().updateCr5kUnits(m_unitDescriptor.getUnits());
   tokenReader.setUnits(m_unitDescriptor.getUnits());

   return retval;
}

////_____________________________________________________________________________
//CCr5kGeometries::CCr5kGeometries()
//{
//}
//
//void CCr5kGeometries::empty()
//{
//   m_geometries.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf:       geometries ::= '(' 'geometry' {geometry} ')'
//bool CCr5kGeometries::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (int geometryCount = 0;;geometryCount++)
//   {
//      CCr5kGeometry* geometry = new CCr5kGeometry();
//
//      if (tokenReader.readZeroOrOneElements(*geometry))
//      {
//         m_geometries.AddTail(geometry);
//      }
//      else
//      {
//         delete geometry;
//         break;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kSingleGeometry::CCr5kSingleGeometry()
: m_allocatedGeometry(NULL)
{
}

void CCr5kSingleGeometry::empty()
{
   delete m_allocatedGeometry;
   m_allocatedGeometry = NULL;

   CCr5kGeometries::empty();

   CCr5kElement::empty();
}

const CCr5kGeometry& CCr5kSingleGeometry::getGeometry() const
{
   CCr5kGeometry* geometry = NULL;

   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
      geometry = getNext(pos);
      break;
   }

   if (geometry == NULL)
   {
      if (m_allocatedGeometry == NULL)
      {
         m_allocatedGeometry = new CCr5kGeometry();
      }

      geometry = m_allocatedGeometry;
   }

   return *geometry;
}

//_____________________________________________________________________________
CCr5kPad::CCr5kPad()
{
}

void CCr5kPad::empty()
{
   m_padName.Empty();
   m_geometries.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf: pads       ::= '(' 'pads' {pad} ')'
// bnf:    pad        ::= '(' 'pad' padName { <userVersion> | <createUser> | <lastEditUser> | <createTime> | <lastEditTime> | <figureArea> | 
//                                            <gridInfo> | <photoPolicy> | <panelUse> } geometries { property } ')'
// bnf:       geometries ::= '(' 'geometry' {geometry} ')'
bool CCr5kPad::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_padName))
   {
      tokenReader.skipList();
      retval = false;
   }

   CCr5kReadElementList elementList;
   CCr5kSkippedElement userVersion,createUser,lastEditUser,createTime,lastEditTime,
                       figureArea,gridInfo,photoPolicy,panelUse;

   elementList.addZeroOrOne(userVersion     ,ptokUver);
   elementList.addZeroOrOne(createUser      ,ptokCreateUser);
   elementList.addZeroOrOne(lastEditUser    ,ptokLastEditUser);
   elementList.addZeroOrOne(createTime      ,ptokCreateTime);
   elementList.addZeroOrOne(lastEditTime    ,ptokLastEditTime);
   elementList.addZeroOrOne(figureArea      ,ptokArea);
   elementList.addZeroOrOne(gridInfo        ,ptokGrid);
   elementList.addZeroOrOne(photoPolicy     ,ptokPhoto);
   elementList.addZeroOrOne(panelUse        ,ptokPanelUse);

   retval = tokenReader.readList(elementList) && retval;

   if (! tokenReader.getNextRequiredToken(ptokGeometry))
   {
      tokenReader.skipList();
      retval = false;
   }

   retval = m_geometries.read(tokenReader) && retval;

   retval = m_properties.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kPads::CCr5kPads()
{
}

void CCr5kPads::empty()
{

   CCr5kElement::empty();
}

// bnf: pads       ::= '(' 'pads' {pad} ')'
// bnf:    pad        ::= '(' 'pad' padName { <userVersion> | <createUser> | <lastEditUser> | <createTime> | <lastEditTime> | <figureArea> | 
//                                            <gridInfo> | <photoPolicy> | <panelUse> } geometries { property } ')'
// bnf:       geometries ::= '(' 'geometry' {geometry} ')'
bool CCr5kPads::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kPad* pad = new CCr5kPad();

      if (tokenReader.readZeroOrOneElements(*pad,ptokPad))
      {
         m_pads.AddTail(pad);
      }
      else
      {
         delete pad;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kPads::linkPads(CCr5kLinkedPads& linkedPads,COperationProgress* progress)
{
   for (POSITION pos = m_pads.GetHeadPosition();pos != NULL;)
   {
      CCr5kPad* pad = m_pads.GetNext(pos);

      linkedPads.add(*pad);

      if (progress != NULL) progress->incrementProgress();
   }
}

//_____________________________________________________________________________
CCr5kPadstackHole::CCr5kPadstackHole()
{
}

void CCr5kPadstackHole::empty()
{
   m_singleGeometry.empty();

   CCr5kElement::empty();
}

// bnf: pcbPadstackHoleGeometry ::= '(' 'hole' '(' 'geometry' geometry ')' ')'

// bnf:       padstackHole      ::= '(' 'hole' footprintLayerRef { <holeGeometry> | <padstackHoleType> | <materialNumber> } ')'
// bnf:          footprintLayerRef ::= '(' 'footLayer' layerName ')'
// bnf:          holeGeometry      ::= '(' 'geometry' geometry ')'
// bnf:          padstackHoleType  ::= '(' 'holeType' integerValue ')'
// bnf:          materialNumber    ::= '(' 'materialNumber' integerValue ')'
bool CCr5kPadstackHole::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredToken(ptokFootLayer))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
   {
      tokenReader.skipList();
      return false;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   CCr5kReadElementList elementList;

   elementList.addZeroOrOne(m_singleGeometry     ,ptokGeometry);

   m_parameters.registerKey(ptokHoleType);
   m_parameters.registerKey(ptokMaterialNumber);

   tokenReader.readElementsAndParameters(elementList,m_parameters);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kPadset::CCr5kPadset()
: m_materialNumber(-1)
{
}

void CCr5kPadset::empty()
{
   m_layerName.Empty();
   m_connectPadName.Empty();
   m_noconnectPadName.Empty();
   m_thermalPadName.Empty();
   m_clearancePadName.Empty();

   CCr5kElement::empty();
}

// bnf:       padset            ::= '(' 'padset' footprintLayerRef [material] [connectPad] [noConnectPad] [thermalPad] [clearancePad] ')'
// bnf:          footprintLayerRef ::= '(' 'footLayer' layerName ')'
// bnf:          material          ::= '(' 'materialNumber' integerValue ')'
// bnf:          connectPad        ::= '(' 'connect' padRef ')'
// bnf:          noConnectPad      ::= '(' 'noconnect' padRef ')'
// bnf:          thermalPad        ::= '(' 'thermal' padRef ')'
// bnf:          clearancePad      ::= '(' 'clearance' padRef ')'
// bnf:             padRef            ::= '(' 'pad' padName ')'
bool CCr5kPadset::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredToken(ptokFootLayer))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   //m_layerName = tokenReader.getNextRequiredNonListToken(.getCurrentTokenString();

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   for (bool loopFlag = true;loopFlag;)
   {
      Cr5kTokenTag token = tokenReader.getNextTokenTag();

      switch (token)
      {
      case ptokMaterialNumber:
         if (! tokenReader.getNextRequiredInteger(m_materialNumber))
         {
            tokenReader.skipList();
            return false;
         }

         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

         break;
      case ptokConnect:
         if (! tokenReader.getNextRequiredToken(ptokPad))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         if (! tokenReader.getNextRequiredNonListToken(m_connectPadName))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

         break;
      case ptokNoconnect:
         if (! tokenReader.getNextRequiredToken(ptokPad))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         if (! tokenReader.getNextRequiredNonListToken(m_noconnectPadName))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

         break;
      case ptokThermal:
         if (! tokenReader.getNextRequiredToken(ptokPad))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         if (! tokenReader.getNextRequiredNonListToken(m_thermalPadName))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

         break;
      case ptokClearance:
         if (! tokenReader.getNextRequiredToken(ptokPad))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         if (! tokenReader.getNextRequiredNonListToken(m_clearancePadName))
         {
            tokenReader.skipList();
            tokenReader.skipList();
            return false;
         }

         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
         retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

         break;
      default:
         tokenReader.ungetToken();

         loopFlag = false;

         break;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kPadstack::CCr5kPadstack()
{
}

void CCr5kPadstack::empty()
{
   m_padstackName.Empty();
   m_parameters.empty();
   m_padstackHoles.empty();
   m_padsets.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf: padstacks         ::= '(' 'padstacks' {padstack} ')'
// bnf:    padstack          ::= '(' 'padstack' padstackName { <createUser> | <lastEditUser> | <createTime> | <lastEditTime> | <figureArea> | 
//                                              <platingType> | <throughMode> | <panelUse> | <isBuildupVia> | <padstackHoleType> } { padstackHole | padset | property } ')'
// bnf:       createUser        ::= '(' 'createUser' string ')'
// bnf:       lastEditUser      ::= '(' 'lastEditUser' string ')'
// bnf:       createTime        ::= '(' 'createTime' timeExp ')'
// bnf:          timeExp           ::= '(' 'time' string ')'
// bnf:       lastEditTime      ::= '(' 'lastEditTime' timeExp ')'
// bnf:       figureArea        ::= '(' 'area' box ')'
// bnf:          box               ::= '(' 'box' point point ')'
// bnf:       platingType       ::= '(' 'type' ( 'PLATED' | 'NONPLATED' ) ')'
// bnf:       throughMode       ::= '(' 'throughMode' ( 'THROUGH' | 'NONTHROUGH' ) ')'
// bnf:       panelUse          ::= '(' 'panelUse' booleanValue ')'
// bnf:       isBuildupVia      ::= '(' 'isBuildupVia' booleanValue ')'
// bnf:       padstackHoleType  ::= '(' 'holeType' integerValue ')'  >>> added to spec

// bnf:       padstackHole      ::= '(' 'hole' footprintLayerRef { <holeGeometry> | <padstackHoleType> | <material> } ')'
// bnf:          footprintLayerRef ::= '(' 'footLayer' layerName ')'
// bnf:          holeGeometry      ::= '(' 'geometry' geometry ')'
// bnf:          padstackHoleType  ::= '(' 'holeType' integerValue ')'
// bnf:       padset            ::= '(' 'padset' footprintLayerRef [material] [connectPad] [noConnectPad] [thermalPad] [clearancePad] ')'
// bnf:          material          ::= '(' 'materialNumber' integerValue ')'
// bnf:          connectPad        ::= '(' 'connect' padRef ')'
// bnf:          noConnectPad      ::= '(' 'noconnect' padRef ')'
// bnf:          thermalPad        ::= '(' 'thermal' padRef ')'
// bnf:          clearancePad      ::= '(' 'clearance' padRef ')'
// bnf:             padRef            ::= '(' 'pad' padName ')'
bool CCr5kPadstack::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_padstackName))
   {
      tokenReader.skipList();
      return false;
   }

   CCr5kReadElementList elementList;
   CCr5kSkippedElement createUser,lastEditUser,createTime,lastEditTime,userVersion,
                       figureArea,platingType,panelUse,isBuildupVia;

   elementList.addZeroOrOne(createUser      ,ptokCreateUser);
   elementList.addZeroOrOne(lastEditUser    ,ptokLastEditUser);
   elementList.addZeroOrOne(userVersion     ,ptokUver);
   elementList.addZeroOrOne(createTime      ,ptokCreateTime);
   elementList.addZeroOrOne(lastEditTime    ,ptokLastEditTime);
   elementList.addZeroOrOne(figureArea      ,ptokArea);
   elementList.addZeroOrOne(platingType     ,ptokType);
   elementList.addZeroOrOne(panelUse        ,ptokPanelUse);
   elementList.addZeroOrOne(isBuildupVia    ,ptokIsBuildupVia);

   m_parameters.registerKey(ptokThroughMode);
   m_parameters.registerKey(ptokHoleType);

   tokenReader.readElementsAndParameters(elementList,m_parameters);

   for (int loopFoundCount = 1;loopFoundCount > 0;)
   {
      loopFoundCount = 0;
      Cr5kTokenTag token = tokenReader.getNextTokenTag();

      switch (token)
      {
      case ptokHole:
         {
            CCr5kPadstackHole* padstackHole = new CCr5kPadstackHole();
            m_padstackHoles.AddTail(padstackHole);

            retval = padstackHole->read(tokenReader) && retval;

            loopFoundCount = 1;
         }

         break;
      case ptokPadset:
         {
            CCr5kPadset* padset = new CCr5kPadset();
            m_padsets.AddTail(padset);

            retval = padset->read(tokenReader) && retval;

            loopFoundCount = 1;
         }

         break;
      default:
         {
            tokenReader.ungetToken();

            int initialCount = m_properties.getCount();

            m_properties.read(tokenReader);

            loopFoundCount = m_properties.getCount() - initialCount;
         }

         break;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kPadstacks::CCr5kPadstacks()
{
}

void CCr5kPadstacks::empty()
{

   CCr5kElement::empty();
}

// bnf: padstacks       ::= '(' 'padstacks' {padstack} ')'
bool CCr5kPadstacks::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kPadstack* padstack = new CCr5kPadstack();

      if (tokenReader.readZeroOrOneElements(*padstack,ptokPadstack))
      {
         m_padstacks.AddTail(padstack);
      }
      else
      {
         delete padstack;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kPadstacks::linkPadstacks(CCr5kLinkedPadstacks& linkedPadstacks,COperationProgress* progress)
{
   for (POSITION pos = m_padstacks.GetHeadPosition();pos != NULL;)
   {
      CCr5kPadstack* padstack = m_padstacks.GetNext(pos);

      linkedPadstacks.add(*padstack);

      if (progress != NULL) progress->incrementProgress();
   }
}

//_____________________________________________________________________________
CCr5kSystemLayerType::CCr5kSystemLayerType()
{
}

// bnf: systemLayerType ::= '(' 'type' systemLayerTypeEnum ')'
//      systemLayerTypeEnum ::= 'BOARD_FIGURE' | 'PADSTACK' | 'LAYOUT_AREA' | 'COMP_GROUP' | 'COMP_GROUP_B' | 'BASEPOINT' | 'BOARD_ASSY'
bool CCr5kSystemLayerType::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = false;

   switch (tokenReader.getNextTokenTag())
   {
   case tokBoard_Figure:
   case tokPadstack:
   case tokLayout_Area:
   case tokComp_Group:
   case tokComp_Group_B:
   case tokBasePoint:
   case tokBoard_Assy:
      m_systemLayerType = tokenReader.getDefinedCurrentTokenTag();
      retval = true;
      break;
   default:
      tokenReader.unexpectedTokenError(__FUNCTION__);

      break;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kLayerIdentifier::CCr5kLayerIdentifier()
: m_referedBy(NULL)
{
}

CCr5kLayerIdentifier::~CCr5kLayerIdentifier()
{
   empty();
}

void CCr5kLayerIdentifier::empty()
{
   m_systemLayerType.empty();

   delete m_referedBy;
   m_referedBy = NULL;

   CCr5kElement::empty();
}

// bnf: layerIdentifier ::= condLayerRef | nonCondLayerRef | subLayerRef | footprintLayerRef |
//                           footprintPadstackLayerRef
//      condLayerRef    ::= '(' 'conductive' layerNumber ')'
//      nonCondLayerRef ::= '(' 'nonConductive' layerName ')'
//      footprintLayerRef ::= '(' 'footLayer' layerName ')'
//      footprintPadstackLayerRef ::= '(' 'FPADSTACK' ')'
//      subLayerRef     ::= systemLayerRef | drawLayerRef | infoLayerRef

//      systemLayerRef  ::= '(' 'systemLayer' systemLayerType ')'
//      drawLayerRef    ::= '(' 'drawOf' referedBy ')'
//      infoLayerRef    ::= '(' 'infoOf' referedBy ')'

//      referedBy       ::= condLayerRef | nonCondLayerRef | systemLayerRef

//      systemLayerType ::= '(' 'type' systemLayerTypeEnum ')'
//      systemLayerTypeEnum ::= 'BOARD_FIGURE' | 'PADSTACK' | 'LAYOUT_AREA' | 'COMP_GROUP' | 'COMP_GROUP_B' | 'BASEPOINT'
bool CCr5kLayerIdentifier::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   m_identifierType = tokenReader.getDefinedCurrentTokenTag();

   switch (m_identifierType)
   {
   case ptokConductive:
   case ptokNonConductive:
   case ptokFootLayer:
      m_layerName = tokenReader.getNextTokenString();

      break;
   case ptokFpadstack:
      break;
   case ptokSystemLayer:
      retval = tokenReader.readOneElement(m_systemLayerType,ptokType);

      break;
   case ptokDrawOf:
   case ptokInfoOf:
      {
         m_referedBy = new CCr5kLayerIdentifier();
         Cr5kTokenTag referType = tokenReader.getNextTokenTag();

         if (referType == ptokConductive || referType == ptokNonConductive || referType == ptokSystemLayer)
         {
            retval = m_referedBy->read(tokenReader);
         }
         else
         {
            tokenReader.unexpectedTokenError(__FUNCTION__ "-1");
         }
      }

      break;
   default:
      tokenReader.unexpectedTokenError(__FUNCTION__ "-2");
      break;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

CString CCr5kLayerIdentifier::getLayerName() const
{
   CString layerName;

   switch (getType())
   {
   case ptokConductive:
   case ptokNonConductive:
   case ptokFootLayer:
      layerName = getName();
      break;
   case ptokFpadstack:
      //layerName = CCr5kTokenReader::cr5kTokenToString(layerIdentifier.getType());
      layerName = "None"; 
      break;
   case ptokSystemLayer:
      {
         Cr5kTokenTag systemLayerType = getSystemLayerType().getSystemLayerType();
         layerName = CCr5kTokenReader::cr5kTokenToString(systemLayerType);
      }

      break;
   case ptokDrawOf:
   case ptokInfoOf:
      {
         const CCr5kLayerIdentifier* referedByIdentifier = getReferedBy();

         if (referedByIdentifier != NULL)
         {
            layerName = referedByIdentifier->getName();
         }
         else
         {
            layerName = CCr5kTokenReader::cr5kTokenToString(getType());
         }
      }
      break;
   default:
      layerName = "Unknown";
      break;
   }

   return layerName;
}

//_____________________________________________________________________________
CCr5kVertex::CCr5kVertex()
{
}

bool CCr5kVertex::read(CCr5kTokenReader& tokenReader)
{
   return true;
}

bool CCr5kVertex::isArc() const
{
   return false;
}

bool CCr5kVertex::isLine() const
{
   return false;
}

bool CCr5kVertex::isTangentArc() const
{
   return false;
}

CPoint2d CCr5kVertex::getBeginPoint() const
{
   return CPoint2d(0.,0.);
}

CPoint2d CCr5kVertex::getEndPoint() const
{
   return CPoint2d(0.,0.);
}

//_____________________________________________________________________________
CCr5kFillet::CCr5kFillet()
{
}

void CCr5kFillet::empty()
{
   m_parameters.empty();

   CCr5kElement::empty();
}

// bnf:             fillet                 ::= '(' 'fillet' [filletType] { <filletDirection> | <filletWidth> | <filletLength> | <pointWidth> | } ')'
// bnf:                filletType             ::= '(' 'type' ( 'STRAIGHT' | 'CURVED' ) ')'
// bnf:                filletDirection        ::= '(' 'dir' ( 'FORWARD' | 'BACKWARD' ) ')'
// bnf:                filletWidth            ::= '(' 'filletWidth' distance ')'
// bnf:                filletLength           ::= '(' 'length' distance ')'
// bnf:                pointWidth             ::= '(' 'width' distance ')'
bool CCr5kFillet::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   m_parameters.registerKey(ptokType);
   m_parameters.registerKey(ptokDir);
   m_parameters.registerKey(ptokFilletWidth);
   m_parameters.registerKey(ptokLength);
   m_parameters.registerKey(ptokWidth);

   m_parameters.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kTanArc::CCr5kTanArc()
{
}

void CCr5kTanArc::empty()
{
   CCr5kElement::empty();
}

// bnf:             tanArcInfo             ::= '(' 'tarc' ( 'ON' | 'OFF' | 'SIMPLE' ) arcRadius ')'
// bnf:                arcRadius              ::= '(' 'r' distance ')'
bool CCr5kTanArc::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListDefinedToken(m_type))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredToken(ptokR))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_arcRadius))
   {
      tokenReader.skipList();
      retval = false;
   }
   else
   {
      retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kPointVertex::CCr5kPointVertex()
{
}

void CCr5kPointVertex::empty()
{
   m_parameters.empty();
   m_tanArc.empty();
   m_fillet.empty();

   CCr5kElement::empty();
}

bool CCr5kPointVertex::isArc() const
{
   return false;
}

bool CCr5kPointVertex::isLine() const
{
   return true;
}

bool CCr5kPointVertex::isTangentArc() const
{
   return m_tanArc.isOn();
}

CPoint2d CCr5kPointVertex::getBeginPoint() const
{
   return CPoint2d(m_x,m_y);
}

CPoint2d CCr5kPointVertex::getEndPoint() const
{
   return CPoint2d(m_x,m_y);
}

double CCr5kPointVertex::getWidth() const
{
   double width = m_parameters.getDoubleValue(ptokWidth,0.);

   return width;
}

bool CCr5kPointVertex::hasWidth() const
{
   bool hasWidthFlag = m_parameters.hasValueForKey(ptokWidth);

   return hasWidthFlag;
}

bool CCr5kPointVertex::hasFillet() const
{
   bool hasFilletFlag = m_fillet.isLoaded();

   return hasFilletFlag;
}

// bnf:       pointVertex           ::= '(' 'pt' coordX coordY { pointVertexInfo } ')'
// bnf:          pointVertexInfo        ::=  pointWidth | tanArcInfo | splineMode | penSmoothingMode | fillet
// bnf:             pointWidth             ::= '(' 'width' distance ')'
// bnf:             tanArcInfo             ::= '(' 'tarc' ( 'ON' | 'OFF' | 'SIMPLE' ) arcRadius ')'
// bnf:                arcRadius              ::= '(' 'r' distance ')'
// bnf:             splineMode             ::= '(' 'spline' ( 'BEGIN' | 'END' ) ')'
// bnf:             penSmoothingMode       ::= '(' 'smooth' ( 'ON' | 'OFF' ) ')'
// bnf:             fillet                 ::= '(' 'fillet' [filletType] { <filletDirection> | <filletWidth> | <filletLength> | <pointWidth> | } ')'
// bnf:                filletType             ::= '(' 'type' ( 'STRAIGHT' | 'CURVED' ) ')'
// bnf:                filletDirection        ::= '(' 'dir' ( 'FORWARD' | 'BACKWARD' ) ')'
// bnf:                filletWidth            ::= '(' 'filletWidth' distance ')'
// bnf:                filletLength           ::= '(' 'length' distance ')'
bool CCr5kPointVertex::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredFloatingPoint(m_x))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_y))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokWidth);
   m_parameters.registerKey(ptokSpline);
   m_parameters.registerKey(ptokSmooth);

   for (bool loopFlag = true;loopFlag;)
   {
      m_parameters.read(tokenReader);

      switch (tokenReader.getNextTokenTag())
      {
      case ptokTarc:
         retval = m_tanArc.read(tokenReader) && retval;

         break;
      case ptokFillet:
         retval = m_fillet.read(tokenReader) && retval;

         break;
      case tokRightParen:
         tokenReader.ungetToken();
         loopFlag = false;

         break;
      default:
         tokenReader.ungetToken();

         if (m_parameters.read(tokenReader) < 1)
         {
            loopFlag = false;
         }
       
         break;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kArcPoint::CCr5kArcPoint()
{
}

void CCr5kArcPoint::empty()
{
   m_parameters.empty();
   m_fillet.empty();
   CCr5kElement::empty();
}

bool CCr5kArcPoint::hasFillet() const
{
   bool hasFilletFlag = m_fillet.isLoaded();

   return hasFilletFlag;
}

// bnf:             arcPoint              ::= '(' 'pt' coordX coordY { arcVertexInfo } ')'
// bnf:                arcVertexInfo         ::=  pointWidth | penSmoothingMode
// bnf:                   pointWidth            ::= '(' 'width' distance ')'
// bnf:                   penSmoothingMode      ::= '(' 'smooth' ( 'ON' | 'OFF' ) ')'
bool CCr5kArcPoint::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredFloatingPoint(m_x))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_y))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokWidth);
   m_parameters.registerKey(ptokSmooth);   

   for (bool loopFlag = true;loopFlag;)
   {
      m_parameters.read(tokenReader);

      switch (tokenReader.getNextTokenTag())
      {
      case ptokFillet:
         retval = m_fillet.read(tokenReader) && retval;

         break;
      case tokRightParen:
         tokenReader.ungetToken();
         loopFlag = false;

         break;
      default:
         tokenReader.ungetToken();

         if (m_parameters.read(tokenReader) < 1)
         {
            loopFlag = false;
         }
       
         break;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kArcVertex::CCr5kArcVertex()
{
}

void CCr5kArcVertex::empty()
{
   m_beginPoint.empty();
   m_endPoint.empty();

   CCr5kElement::empty();
}

bool CCr5kArcVertex::isArc() const
{
   return true;
}

bool CCr5kArcVertex::isLine() const
{
   return false;
}

bool CCr5kArcVertex::isTangentArc() const
{
   return false;
}

CPoint2d CCr5kArcVertex::getBeginPoint() const
{
   return m_beginPoint.getPoint();
}

CPoint2d CCr5kArcVertex::getEndPoint() const
{
   return m_endPoint.getPoint();
}

double CCr5kArcVertex::getBulge() const
{
   CPoint2d arcCenter;

   return getBulge(arcCenter);
}

double CCr5kArcVertex::getBulge(CPoint2d& arcCenter) const
{
   double bulge = 1.;

   CPoint2d beginPoint = m_beginPoint.getPoint();
   CPoint2d   endPoint =   m_endPoint.getPoint();

   double startAngle,deltaAngle;

   int result = ArcFromStartEndRadiusDeltaXY(beginPoint.x,beginPoint.y,endPoint.x,endPoint.y,m_arcRadius,isCw(),m_deltaX,m_deltaY,&(arcCenter.x),&(arcCenter.y),&startAngle,&deltaAngle);

   if (result > 0)
   {
      bulge = tan(deltaAngle/4.);
   }
   else  // the arc is a semicircle
   {
      bulge = (isCw() ? -1. : 1.);
   }

   return bulge;
}

// bnf:       arcVertex             ::= '(' 'arc' arcDirection arcRadius arcBeginPoint arcCenterVector arcEndPoint ')'
// bnf:          arcDirection          ::= 'CW' | 'CCW'
// bnf:          arcRadius             ::= '(' 'r' distance ')'
// bnf:          arcBeginPoint         ::= '(' 'begin' arcPoint ')'
// bnf:          arcEndPoint           ::= '(' 'end'   arcPoint ')'
// bnf:             arcPoint              ::= '(' 'pt' coordX coordY { arcVertexInfo } ')'
// bnf:                arcVertexInfo         ::=  pointWidth | penSmoothingMode
// bnf:                   pointWidth            ::= '(' 'width' distance ')'
// bnf:                   penSmoothingMode      ::= '(' 'smooth' ( 'ON' | 'OFF' ) ')'
// bnf:          arcCenterVector       ::= '(' 'center' deltaX deltaY ')'
bool CCr5kArcVertex::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   // arcDirection
   if (! tokenReader.getNextRequiredNonListDefinedToken(m_arcDirection))
   {
      tokenReader.skipList();
      return false;
   }

   // arcRadius
   if (! tokenReader.getNextRequiredToken(ptokR))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_arcRadius))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   // arcBeginPoint
   if (! tokenReader.getNextRequiredToken(ptokBegin))
   {
      tokenReader.skipList();
      return false;
   }

   // pt
   if (! tokenReader.getNextRequiredToken(ptokPt))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   m_beginPoint.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   // arcCenterVector
   if (! tokenReader.getNextRequiredToken(ptokCenter))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_deltaX))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_deltaY))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   // arcEndPoint
   if (! tokenReader.getNextRequiredToken(ptokEnd))
   {
      tokenReader.skipList();
      return false;
   }

   // pt
   if (! tokenReader.getNextRequiredToken(ptokPt))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   m_endPoint.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//CPoint2d CCr5kArcVertex::correctCenterCoordinate(CPoint2d center,const CPoint2d& begin,double radius)
//{  //   maxCenterCoordinateValue = doc->convertToPageUnits(pageUnitsInches,50.);
//   bool xOutOfRange = (fabs(center.x) > maxCenterCoordinateValue);
//   bool yOutOfRange = (fabs(center.y) > maxCenterCoordinateValue);
//
//   if (xOutOfRange && center.y == 0. ||
//       yOutOfRange && center.x == 0.     )
//   {
//      if (center.y == 0.)
//      {
//         if (center.x > 0.)
//         {
//            center.x = radius;
//         }
//         else if (center.x < 0.)
//         {
//            center.x = -radius;
//         }
//      }
//      else if (center.x == 0.)
//      {
//         if (center.y > 0.)
//         {
//            center.y = radius;
//         }
//         else if (center.y < 0.)
//         {
//            center.y = -radius;
//         }
//      }
//      else
//      {
//         if (center.x < 0.)
//         {
//            if (xOutOfRange)
//            {
//               //center.x = begin.x - fabs(begin.x);
//               center.x = -begin.x;
//            }
//         }
//         else if (center.x > 0.)
//         {
//            if (xOutOfRange)
//            {
//               //center.x = begin.x + fabs(begin.x);
//               center.x = begin.x;
//            }
//         }
//
//         if (center.y < 0.)
//         {
//            if (yOutOfRange)
//            {
//               //center.y = begin.y - fabs(begin.y);
//               center.y = -begin.y;
//            }
//         }
//         else if (center.y > 0.)
//         {
//            if (yOutOfRange)
//            {
//               //center.y = begin.y + fabs(begin.y);
//               center.y = begin.y;
//            }
//         }
//      }
//
//      center.x += begin.x;
//      center.y += begin.y;
//   }
//   else  
//   {
//      // original code result
//      center.x = begin.x + radius;
//      center.y = begin.y;
//   }
//
//   return center;
//}

//_____________________________________________________________________________
CCr5kVertices::CCr5kVertices()
: m_units(pageUnitsUndefined)
{
}

//void CCr5kVertices::empty()
//{
//   m_vertices.empty();
//
//   CCr5kElement::empty();
//}

// bnf: vertices             ::= '(' 'vertex' { vertex } ')'
// bnf:    vertex               ::= pointVertex | arcVertex
// bnf:       pointVertex           ::= '(' 'pt' coordX coordY { pointVertexInfo } ')'
// bnf:          pointVertexInfo        ::=  pointWidth | tanArcInfo | splineMode | penSmoothingMode | fillet
// bnf:             pointWidth             ::= '(' 'width' distance ')'
// bnf:             tanArcInfo             ::= '(' 'tarc' ( 'ON' | 'OFF' | 'SIMPLE' ) arcRadius ')'
// bnf:                arcRadius              ::= '(' 'r' distance ')'
// bnf:             splineMode             ::= '(' 'spline' ( 'BEGIN' | 'END' ) ')'
// bnf:             penSmoothingMode       ::= '(' 'smooth' ( 'ON' | 'OFF' ) ')'
// bnf:             fillet                 ::= '(' 'fillet' [filletType] { <filletDirection> | <filletWidth> | <filletLength> | <pointWidth> | } ')'
// bnf:                filletType             ::= '(' 'type' ( 'STRAIGHT' | 'CURVED' ) ')'
// bnf:                filletDirection        ::= '(' 'dir' ( 'FORWARD' | 'BACKWARD' ) ')'
// bnf:                filletWidth            ::= '(' 'filletWidth' distance ')'
// bnf:                filletLength           ::= '(' 'length' distance ')'
// bnf:       arcVertex             ::= '(' 'arc' arcDirection arcRadius arcBeginPoint arcCenterVector arcEndPoint ')'
// bnf:          arcDirection          ::= 'CW' | 'CCW'
// bnf:          arcBeginPoint         ::= '(' 'begin' arcPoint ')'
// bnf:          arcEndPoint           ::= '(' 'end'   arcPoint ')'
// bnf:             arcPoint              ::= '(' 'pt' coordX coordY { arcVertexInfo } ')'
// bnf:                arcVertexInfo         ::=  pointWidth | penSmoothingMode
// bnf:          arcCenterVector       ::= '(' 'center' deltaX deltaY ')'
bool CCr5kVertices::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   setUnits(tokenReader.getUnits());
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   while (true)
   {
      Cr5kTokenTag token = tokenReader.getNextTokenTag();

      if (token == ptokPt)
      {
         CCr5kPointVertex* pointVertex = new CCr5kPointVertex();

         retval = pointVertex->read(tokenReader) && retval;

         m_elementList.AddTail(pointVertex);
      }
      else if (token == ptokArc)
      {
         CCr5kArcVertex* arcVertex = new CCr5kArcVertex();

         retval = arcVertex->read(tokenReader) && retval;

         m_elementList.AddTail(arcVertex);
      }
      else
      {
         tokenReader.ungetToken();

         break;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   return retval;
}

//_____________________________________________________________________________
CCr5kPointParameter::CCr5kPointParameter()
{
}

void CCr5kPointParameter::empty()
{
   m_point.empty();

   CCr5kElement::empty();
}

// bnf:    meshBasePoint       ::= '(' 'meshBasePoint' point ')'
bool CCr5kPointParameter::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   if (! tokenReader.getNextRequiredToken(ptokPt))
   {
      tokenReader.skipList();
      return false;
   }

   bool retval = m_point.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   return retval;
}

//_____________________________________________________________________________
CCr5kMeshFlags::CCr5kMeshFlags()
{
}

void CCr5kMeshFlags::empty()
{
   m_meshFlags.RemoveAll();

   CCr5kElement::empty();
}

// bnf:    meshFlags           ::= '(' 'meshFlags' {integerValue} ')'
bool CCr5kMeshFlags::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   while (tokenReader.getNextOptionalNonListToken())
   {
      int flag = tokenReader.verifyInteger();

      m_meshFlags.Add(flag);
   }

   bool retval = tokenReader.skipExpectedEmptyRemainderOfList();

   return retval;
}

//_____________________________________________________________________________
CCr5kRawString::CCr5kRawString()
{
}

void CCr5kRawString::empty()
{
   m_string.Empty();

   CCr5kElement::empty();
}

// bnf:    textString          ::= '(' 'string' string ')'
bool CCr5kRawString::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   m_string = tokenReader.getNextRawTokenString();

   bool retval = tokenReader.skipExpectedEmptyRemainderOfList();

   return retval;
}

//_____________________________________________________________________________
CCr5kOpenShape::CCr5kOpenShape()
: m_units(pageUnitsUndefined)
, m_useOutlineWidth(true)
{
}

void CCr5kOpenShape::empty()
{
   m_parameters.empty();
   m_vertices.empty();

   CCr5kElement::empty();
}

// bnf: openShape           ::= '(' 'openShape' [outlineWidth] vertices ')'
// bnf:    outlineWidth        ::= '(' 'outlineWidth' distance ')'
bool CCr5kOpenShape::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   m_useOutlineWidth = tokenReader.getZukenReader().getOptionUseOutlineWidth();

   setUnits(tokenReader.getUnits());

   m_parameters.registerKey(ptokOutlineWidth);
   m_parameters.read(tokenReader);

   if (! tokenReader.getNextRequiredToken(ptokVertex))
   {
      tokenReader.skipList();
      return false;
   }

   bool retval = m_vertices.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   return retval;
}

//_____________________________________________________________________________
//CCr5kOpenShapes::CCr5kOpenShapes()
//{
//}
//
//void CCr5kOpenShapes::empty()
//{
//   m_openShapes.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf: {openShape}
//// bnf: openShape           ::= '(' 'openShape' [outlineWidth] vertices ')'
//// bnf:    outlineWidth        ::= '(' 'outlineWidth' distance ')'
//bool CCr5kOpenShapes::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kOpenShape* openShape = new CCr5kOpenShape();
//
//      if (tokenReader.readZeroOrOneElements(*openShape,ptokOpenShape))
//      {
//         m_openShapes.AddTail(openShape);
//      }
//      else
//      {
//         delete openShape;
//         loopFlag = false;
//      }
//   }
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kGeometry::CCr5kGeometry()
: m_vertices(NULL)
, m_openShapes(NULL)
, m_meshBasePoint(NULL)
, m_meshFlags(NULL)
, m_units(pageUnitsUndefined)
, m_useOutlineWidth(true)
{
}

void CCr5kGeometry::empty()
{
   delete m_vertices;
   m_vertices = NULL;

   delete m_openShapes;
   m_openShapes = NULL;

   delete m_meshBasePoint;
   m_meshBasePoint = NULL;

   delete m_meshFlags;
   m_meshFlags = NULL;

   m_parameters.empty();

   CCr5kElement::empty();
}

CCr5kVertices& CCr5kGeometry::getVertices()
{
   if (m_vertices == NULL)
   {
      m_vertices = new CCr5kVertices();
   }

   return *m_vertices;
}

const CCr5kVertices& CCr5kGeometry::getVertices() const
{
   if (m_vertices == NULL)
   {
      m_vertices = new CCr5kVertices();
   }

   return *m_vertices;
}

bool CCr5kGeometry::hasOpenShapes() const
{
   bool retval = ((m_openShapes != NULL) && (m_openShapes->getCount() > 0));

   return retval;
}

const CCr5kOpenShapes& CCr5kGeometry::getOpenShapes() const
{
   if (m_openShapes == NULL)
   {
      m_openShapes = new CCr5kOpenShapes();
   }

   return *m_openShapes;
}

CCr5kOpenShapes& CCr5kGeometry::getOpenShapes()
{
   if (m_openShapes == NULL)
   {
      m_openShapes = new CCr5kOpenShapes();
   }

   return *m_openShapes;
}

CCr5kPointParameter& CCr5kGeometry::getMeshBasePoint()
{
   if (m_meshBasePoint == NULL)
   {
      m_meshBasePoint = new CCr5kPointParameter();
   }

   return *m_meshBasePoint;
}

CCr5kMeshFlags& CCr5kGeometry::getMeshFlags()
{
   if (m_meshFlags == NULL)
   {
      m_meshFlags = new CCr5kMeshFlags();
   }

   return *m_meshFlags;
}

// bnf: geometry         ::= circle | donut | oblong | line | surface | roundThermalFigure | squareThermalFigure | text | rectangle | oval | meshplane
// bnf: circle              ::= '(' 'circle' radius point ')'
// bnf:    point               ::= '(' 'pt' coordinateValue coordinateValue ')'
//
//       added 20061213 - knv
// bnf: squareHole          ::= '(' 'squareHole' point { <holeAngle> | <holwWidth> | <holeHeight> | <holeCornerRadius> }')'
// bnf:    holeAngle           ::= '(' 'holeAngle' degreesValue ')'
// bnf:    holeWidth           ::= '(' 'width' distance ')'
// bnf:    holeHeight          ::= '(' 'height' distance ')'
// bnf:    holeCornerRadius    ::= '(' 'r' distance ')'
//
// bnf: donut               ::= '(' 'donut' { <outerSize> | <innerSize> | <point> } ')'
// bnf:    outerSize           ::= '(' 'out' distance ')'
// bnf:    innerSize           ::= '(' 'in'  distance ')'
// bnf: oblong              ::= '(' 'oblong' width point point ')'
// bnf:    width               ::= '(' 'width' distance ')'
// bnf: line                ::= '(' 'line' { <lineType> | <penShape> | <dashLineParameters> } vertices ')'
// bnf:    lineType            ::= '(' 'type'     ( 'SOLID' | 'DASH' | 'PHANTOM' | 'DOUBLE_PHANTOM' ) ')'
// bnf:    penShape            ::= '(' 'penShape' ( 'CIRCLE' | 'SQUARE' | 'OCTAGON' ) ')'
// bnf:    dashLineParameters  ::= '(' 'dashLine' { <dashLength> | <dotLength> | <blankLength> } ')'
// bnf:       dashLength          ::= '(' 'dash' distance ')'
// bnf:       dotLength           ::= '(' 'dot' distance ')'
// bnf:       blankLength         ::= '(' 'blank' distance ')'
// bnf: surface             ::= '(' 'surface' { <outlineWidth> | <fillWidth> | <fillAngle> | <alreadySpread> } vertices {openShape} ')'
// bnf:    outlineWidth        ::= '(' 'outlineWidth' distance ')'
// bnf:    fillWidth           ::= '(' 'fillWidth' distance ')'
// bnf:    fillAngle           ::= '(' 'fillAngle' floatingValue ')'
// bnf:    alreadySpread       ::= '(' 'alreadySpread' booleanValue ')'
// bnf:    openShape           ::= '(' 'openShape' [outlineWidth] vertices ')'
// bnf: roundThermalFigure  ::= '(' 'roundThermal'  thermalFigureBody ')'
// bnf: squareThermalFigure ::= '(' 'squareThermal' thermalFigureBody ')'
// bnf:    thermalFigureBody   ::= { <outerSize> | <innerSize> | <point>  | <nBridge>  | <bridgeWidth>  | <bridgeAngle> }
// bnf:       nBridge             ::= '(' 'nBridge' integerValue ')'
// bnf:       bridgeWidth         ::= '(' 'bridgeWidth' distance ')'
// bnf:       bridgeAngle         ::= '(' 'bridgeAngle' integerValue ')'
// bnf: text                ::= '(' 'text' { <textString> | <singleByteFont> | <twoBytesFont> | <charWidth> | <charHeight> | <charSpace> | 
//                                           <lineSpace> | <strokeWidth> | <textAngle> | <point> | <textDirection> | <textJustify> | <textFlip> | 
//                                           <textReverseMode> | <textFrameMode> | <textFrameOffset> | <textFillWidth> } ')'
// bnf:    textString          ::= '(' 'string' string ')'
// bnf:    singleByteFont      ::= '(' 'font' integerValue ')'
// bnf:    twoBytesFont        ::= '(' 'kFont' integerValue ')'
// bnf:    charWidth           ::= '(' 'width' distance ')'
// bnf:    charHeight          ::= '(' 'height' distance ')'
// bnf:    charSpace           ::= '(' 'space' distance ')'
// bnf:    lineSpace           ::= '(' 'vSpace' distance ')'
// bnf:    strokeWidth         ::= '(' 'strokeWidth' distance ')'
// bnf:    textAngle           ::= '(' ( 'angle' | 'textAngle' ) fAngle ')'
// bnf:    textDirection       ::= '(' 'dir' ( 'LtoR' | 'RtoL' | 'BtoT' | 'TtoB' ) ')'
// bnf:    textJustify         ::= '(' 'justify' ( 'LO_L' | 'LO_C' | 'LO_R' | 'CE_L' | 'CE_C' | 'CE_R' | 'UP_L' | 'UP_C' | 'UP_R' ) ')'
// bnf:    textFlip            ::= '(' 'flip' ( 'NONE' | 'X' | 'Y' | 'XY' ) ')'
// bnf:    textReverseMode     ::= '(' 'reverse' booleanValue ')'
// bnf:    textFrameMode       ::= '(' 'textFrame' booleanValue ')'
// bnf:    textFrameOffset     ::= '(' 'textFrameOffset' distance ')'
// bnf:    textFillWidth       ::= '(' 'fillWidth' distance ')'
// bnf: rectangle           ::= '(' 'rectangle' { <point> | <width> | <height> | <rectangleAngle> | <outlineWidth> | <fillWidth> | <fillAngle> } ')'
// bnf:    rectangleAngle      ::= '(' 'rAngle' fAngle ')'
// bnf:    outlineWidth        ::= '(' 'outlineWidth' distance ')'
// bnf: oval                ::= '(' 'oval' { <point> | <width> | <height> | <ovalAngle> } ')'
// bnf:    ovalAngle           ::= '(' 'ovalAngle' fAngle ')'
// bnf: meshplane           ::= '(' 'meshplane' { <outlineWidth> | <fillWidth> | <fillAngle> | <alreadySpread> } vertices {openShape} 
//                                  { <meshBasePoint> | <meshPitch> | <meshShape> | <meshShapeDiameter> | <meshOutlineClearance> | <meshArrangeType> | 
//                                    <meshFlagCount> | <meshFlags> } ')'
// bnf:    meshBasePoint       ::= '(' 'meshBasePoint' point ')'
// bnf:    meshPitch           ::= '(' 'meshPitch' distance ')'
// bnf:    meshShape           ::= '(' 'meshShape' ( 'CIRCLE' | 'SQUARE' | 'DIAMOND' ) ')'
// bnf:    meshShapeDiameter   ::= '(' 'meshShapeDiameter' integerValue ')'
// bnf:    meshOutlineClearance::= '(' 'meshOutlineClearance' distance ')'
// bnf:    meshArrangeType     ::= '(' 'meshArrangeType' ( '4POINTS' | '5POINTS' ) ')'
// bnf:    meshFlagCount       ::= '(' 'meshFlagCount' integerValue ')'
// bnf:    meshFlags           ::= '(' 'meshFlags' {integerValue} ')'
// bnf:    meshFlagXCount      ::= '(' 'meshFlagXCount' integerValue ')'

// bnf: vertices             ::= '(' 'vertex' { vertex } ')'
// bnf:    vertex               ::= pointVertex | arcVertex
// bnf:       pointVertex           ::= '(' 'pt' coordX coordY { pointVertexInfo } ')'
// bnf:          pointVertexInfo        ::=  pointWidth | tanArcInfo | splineMode | penSmoothingMode | fillet
// bnf:             pointWidth             ::= '(' 'width' distance ')'
// bnf:             tanArcInfo             ::= '(' 'tarc' ( 'ON' | 'OFF' | 'SIMPLE' ) arcRadius ')'
// bnf:                arcRadius              ::= '(' 'r' distance ')'
// bnf:             splineMode             ::= '(' 'spline' ( 'BEGIN' | 'END' ) ')'
// bnf:             penSmoothingMode       ::= '(' 'smooth' ( 'ON' | 'OFF' ) ')'
// bnf:             fillet                 ::= '(' 'fillet' [filletType] { <filletDirection> | <filletWidth> | <filletLength> | <pointWidth> | } ')'
// bnf:                filletType             ::= '(' 'type' ( 'STRAIGHT' | 'CURVED' ) ')'
// bnf:                filletDirection        ::= '(' 'dir' ( 'FORWARD' | 'BACKWARD' ) ')'
// bnf:                filletWidth            ::= '(' 'filletWidth' distance ')'
// bnf:                filletLength           ::= '(' 'length' distance ')'
// bnf:       arcVertex             ::= '(' 'arc' arcDirection arcRadius arcBeginPoint arcCenterVector arcEndPoint ')'
// bnf:          arcDirection          ::= 'CW' | 'CCW'
// bnf:          arcBeginPoint         ::= '(' 'begin' arcPoint ')'
// bnf:          arcEndPoint           ::= '(' 'end'   arcPoint ')'
// bnf:             arcPoint              ::= '(' 'pt' coordX coordY { arcVertexInfo } ')'
// bnf:                arcVertexInfo         ::=  pointWidth | penSmoothingMode
// bnf:          arcCenterVector       ::= '(' 'center' deltaX deltaY ')'
bool CCr5kGeometry::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   m_useOutlineWidth = tokenReader.getZukenReader().getOptionUseOutlineWidth();
   setUnits(tokenReader.getUnits());
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   m_geometryType = tokenReader.getCurrentTokenTag();

   m_parameters.registerKey(ptokOut);
   m_parameters.registerKey(ptokIn);
   m_parameters.registerKey(ptokWidth);
   m_parameters.registerKey(ptokType);
   m_parameters.registerKey(ptokPenShape);
   m_parameters.registerKey(ptokDot);
   m_parameters.registerKey(ptokDash);
   m_parameters.registerKey(ptokBlank);
   m_parameters.registerKey(ptokOutlineWidth);
   m_parameters.registerKey(ptokFillWidth);
   m_parameters.registerKey(ptokFillAngle);
   m_parameters.registerKey(ptokAlreadySpread);
   m_parameters.registerKey(ptokNBridge);
   m_parameters.registerKey(ptokBridgeWidth);
   m_parameters.registerKey(ptokBridgeAngle);
   m_parameters.registerKey(ptokFont);
   m_parameters.registerKey(ptokKFont);
   m_parameters.registerKey(ptokHeight);
   m_parameters.registerKey(ptokHoleAngle);
   m_parameters.registerKey(ptokR);
   m_parameters.registerKey(ptokSpace);
   m_parameters.registerKey(ptokVSpace);
   m_parameters.registerKey(ptokStrokeWidth);
   m_parameters.registerKey(ptokAngle);
   m_parameters.registerKey(ptokTextAngle);
   m_parameters.registerKey(ptokDir);
   m_parameters.registerKey(ptokJustify);
   m_parameters.registerKey(ptokFlip);
   m_parameters.registerKey(ptokReverse);
   m_parameters.registerKey(ptokTextFrame);
   m_parameters.registerKey(ptokTextFrameOffset);
   m_parameters.registerKey(ptokFillWidth);
   m_parameters.registerKey(ptokRAngle);
   m_parameters.registerKey(ptokOvalAngle);
   m_parameters.registerKey(ptokMeshPitch);
   m_parameters.registerKey(ptokMeshShape);
   m_parameters.registerKey(ptokMeshShapeDiameter);
   m_parameters.registerKey(ptokMeshOutlineClearance);
   m_parameters.registerKey(ptokMeshArrangeType);
   m_parameters.registerKey(ptokMeshFlagCount);
   m_parameters.registerKey(ptokMeshFlagXCount);
// bnf:    point               ::= '(' 'pt' coordinateValue coordinateValue ')'
// bnf:    dashLineParameters  ::= '(' 'dashLine' { <dashLength> | <dotLength> | <blankLength> } ')'
// bnf:    openShape           ::= '(' 'openShape' [outlineWidth] vertices ')'
// bnf:    meshBasePoint       ::= '(' 'meshBasePoint' point ')'
// bnf:    meshFlags           ::= '(' 'meshFlags' {integerValue} ')'

   switch (m_geometryType)
   {
   case ptokCircle:
      if (! tokenReader.getNextRequiredFloatingPoint(m_floatValue))
      {
         tokenReader.skipList();
         return false;
      }

      if (! tokenReader.getNextRequiredToken(ptokPt))
      {
         tokenReader.skipList();
         return false;
      }

      retval = m_point.read(tokenReader);

      break;
   case ptokOblong:
      m_parameters.read(tokenReader);

      if (! tokenReader.getNextRequiredToken(ptokPt))
      {
         tokenReader.skipList();
         return false;
      }

      retval = m_point.read(tokenReader);

      if (! tokenReader.getNextRequiredToken(ptokPt))
      {
         tokenReader.skipList();
         return false;
      }

      retval = m_point2.read(tokenReader);

      break;
   case ptokText:
      {
         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_rawString,ptokString);
         elementList.addZeroOrOne(m_point    ,ptokPt);

         tokenReader.readElementsAndParameters(elementList,m_parameters);
      }

      break;
   case ptokDonut:
   case ptokRoundThermal:
   case ptokSquareThermal:
   case ptokRectangle:
   case ptokOval:
   case ptokSquareHole:
      {
         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_point    ,ptokPt);

         tokenReader.readElementsAndParameters(elementList,m_parameters);
      }

      break;
   case ptokLine:
      m_parameters.read(tokenReader);

      if (tokenReader.getNextTokenTag() == ptokDashLine)
      {
         m_parameters.read(tokenReader);
         tokenReader.skipExpectedEmptyRemainderOfList();
         m_parameters.read(tokenReader);
      }
      else
      {
         tokenReader.ungetToken();
      }

      if (! tokenReader.getNextRequiredToken(ptokVertex))
      {
         tokenReader.skipList();
         return false;
      }

      retval = getVertices().read(tokenReader);

      break;
   case ptokSurface:
      m_parameters.read(tokenReader);

      if (! tokenReader.getNextRequiredToken(ptokVertex))
      {
         tokenReader.skipList();
         return false;
      }

      retval = getVertices().read(tokenReader);

      if (tokenReader.getNextTokenTag() == ptokOpenShape)
      {
         retval = getOpenShapes().read(tokenReader) && retval;
      }
      else
      {
         tokenReader.ungetToken();
      }

      break;
   case ptokMeshplane:
      {
         m_parameters.read(tokenReader);

         if (! tokenReader.getNextRequiredToken(ptokVertex))
         {
            tokenReader.skipList();
            return false;
         }

         retval = getVertices().read(tokenReader);
         retval = getOpenShapes().read(tokenReader) && retval;

         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(getMeshBasePoint(),ptokMeshBasePoint);
         elementList.addZeroOrOne(getMeshFlags()    ,ptokMeshFlags);

         tokenReader.readElementsAndParameters(elementList,m_parameters);
      }

      break;

   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kPoint::CCr5kPoint()
: m_units(pageUnitsUndefined)
{
}

void CCr5kPoint::empty()
{

   CCr5kElement::empty();
}

// bnf: point                   ::= '(' 'pt'               coordinateValue coordinateValue ')'
bool CCr5kPoint::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   setUnits(tokenReader.getUnits());

   bool retval = true;

   if (! tokenReader.getNextRequiredFloatingPoint(m_x))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredFloatingPoint(m_y))
   {
      tokenReader.skipList();
      return false;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kFromTo::CCr5kFromTo()
{
}

void CCr5kFromTo::empty()
{

   CCr5kElement::empty();
}

// bnf: fromTo                  ::= '(' 'fromTo'           layerNumber [layerNumber] ')'
bool CCr5kFromTo::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredInteger(m_layerNumber1))
   {
      tokenReader.skipList();
      return false;
   }

   if (tokenReader.getNextOptionalNonListToken())
   {
      m_layerNumber2 = tokenReader.verifyInteger();
   }
   else
   {
      m_layerNumber2 = m_layerNumber1;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kTestPad::CCr5kTestPad()
: m_isTestPad(false)
{
}

void CCr5kTestPad::empty()
{
   m_isTestPad = false;
   m_parameters.empty();
   m_point.empty();

   CCr5kElement::empty();
}

// bnf: isTestPad               ::= '(' 'isTestPad'        booleanValue { <testPadSide> | <testPadId> | <testPoint>  | <tpProbeId> ')'
// bnf:    testPadSide             ::= '(' 'side' { 'A' | 'B' } ')'
// bnf:    testPadId               ::= '(' 'id' string ')'
// bnf:    tpProbeId               ::= '(' 'tpProbeId' string ')' --> added 20070816 knv
// bnf:    testPoint               ::= point
bool CCr5kTestPad::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredBoolean(m_isTestPad))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokSide);
   m_parameters.registerKey(ptokId);
   m_parameters.registerKey(ptokTpProbeId);
   m_parameters.read(tokenReader);

   CCr5kReadElementList elementList;

   elementList.addZeroOrOne(m_point                   ,ptokPt);

   tokenReader.readElementsAndParameters(elementList,m_parameters);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kConductivePadStackPadLayer::CCr5kConductivePadStackPadLayer()
{
}

void CCr5kConductivePadStackPadLayer::empty()
{
   m_parameters.empty();
   m_padstackGeometries.empty();

   CCr5kElement::empty();
}

// bnf: condPadstackPad              ::= '(' 'conductive'        {condPadstackPadLayer} ')'
// bnf:    condPadstackPadLayer         ::= '(' 'layerNumber' layerNumber condPadstackPadStatus [padstackPadGeometry] ')'
// bnf:       condPadstackPadStatus        ::= '(' 'status' { 'CONNECT' | 'NOCONNECT' | 'THERMAL' | 'CLEARANCE' | 'NOPAD' } ')'
// bnf:       padstackPadGeometry          ::= '(' 'geometry' {geometry} ')'
bool CCr5kConductivePadStackPadLayer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredInteger(m_layerNumber))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokStatus);
   m_parameters.read(tokenReader);

   CCr5kReadElementList elementList;

   elementList.addZeroOrOne(m_padstackGeometries,ptokGeometry);

   tokenReader.readList(elementList);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

Cr5kTokenTag CCr5kConductivePadStackPadLayer::getStatus() const
{
   Cr5kTokenTag status = m_parameters.getTokenValue(ptokStatus,tokUndefined);

   return status;
}

//_____________________________________________________________________________
bool CCr5kConductivePadStackPad::containsPadTypeOnLayer(int layerNumber,Cr5kTokenTag padType) const
{
   bool retval = false;

   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
      CCr5kConductivePadStackPadLayer* conductivePadStackPadLayer = getNext(pos);

      if (conductivePadStackPadLayer->getLayerNumber() == layerNumber)
      {
         if (conductivePadStackPadLayer->getStatus() == padType)
         {
            retval = true;
            break;
         }
      }
   }

   return retval;
}

CString CCr5kConductivePadStackPad::getDescriptor() const
{
   CString descriptor,delimiter;

   CTypedPtrArrayContainer<CCr5kConductivePadStackPadLayer*> padLayers(10,false);

   for (POSITION pos = getHeadPosition();pos != NULL;)
   {
      CCr5kConductivePadStackPadLayer* conductivePadStackPadLayer = getNext(pos);

      padLayers.SetAtGrow(conductivePadStackPadLayer->getLayerNumber(),conductivePadStackPadLayer);
   }

   for (int index = 0;index < padLayers.GetSize();index++)
   {
      CCr5kConductivePadStackPadLayer* conductivePadStackPadLayer = padLayers.GetAt(index);

      if (conductivePadStackPadLayer != NULL)
      {
         // bnf:       condPadstackPadStatus        ::= '(' 'status' { 'CONNECT' | 'NOCONNECT' | 'THERMAL' | 'CLEARANCE' | 'NOPAD' } ')'
         CString padType;

         switch (conductivePadStackPadLayer->getStatus())
         {
         case tokConnect:    padType = "C";    break;
         case tokNoconnect:  padType = "NC";   break;
         case tokThermal:    padType = "T";    break;
         case tokClearance:  padType = "CLR";  break;
         case tokNopad:      padType = "N";    break;
         default:            padType = CCr5kTokenReader::cr5kTokenToString(conductivePadStackPadLayer->getStatus());  break;
         }

         descriptor.AppendFormat("%s%d-%s",delimiter,conductivePadStackPadLayer->getLayerNumber(),padType);

         delimiter = ":";
      }
   }

   return descriptor;
}

//CCr5kConductivePadStackPad::CCr5kConductivePadStackPad()
//{
//}
//
//void CCr5kConductivePadStackPad::empty()
//{
//   m_parameters.empty();
//   m_padstackGeometries.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf: condPadstackPad              ::= '(' 'conductive'        {condPadstackPadLayer} ')'
//// bnf:    condPadstackPadLayer         ::= '(' 'layerNumber' layerNumber condPadstackPadStatus [padstackPadGeometry] ')'
//// bnf:       condPadstackPadStatus        ::= '(' 'status' { 'CONNECT' | 'NOCONNECT' | 'THERMAL' | 'CLEARANCE' | 'NOPAD' } ')'
//// bnf:       padstackPadGeometry          ::= '(' 'geometry' {geometry} ')'
//bool CCr5kConductivePadStackPad::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//   //// here it is iii iii
//   if (tokenReader.getNextTokenTag() == ptokLayerNumber)
//   {
//      if (! tokenReader.getNextRequiredInteger(m_layerNumber))
//      {
//         tokenReader.skipList();
//         return false;
//      }
//
//      m_parameters.registerKey(ptokStatus);
//      m_parameters.read(tokenReader);
//
//      if (tokenReader.getNextTokenTag() == ptokGeometry)
//      {
//         m_padstackGeometries.read(tokenReader);
//      }
//      else
//      {
//         tokenReader.ungetToken();
//      }
//
//      retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//   }
//   else if (tokenReader.isNonTerminatingToken())
//   {
//      tokenReader.expectedTokenError(ptokLayer);
//
//      tokenReader.skipList();
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kNonConductivePadStackPadLayer::CCr5kNonConductivePadStackPadLayer()
{
}

void CCr5kNonConductivePadStackPadLayer::empty()
{
   m_layerName.Empty();
   m_parameters.empty();
   m_padstackGeometries.empty();

   CCr5kElement::empty();
}

// bnf: nonCondPadstackPad           ::= '(' 'nonConductive'     {nonCondPadstackPadLayer} ')'
// bnf:    nonCondPadstackPadLayer      ::= '(' 'layer' layerName [visible] [padstackPadGeometry] ')'
// bnf:       visible                      ::= '(' 'visible' booleanValue ')'
// bnf:       padstackPadGeometry          ::= '(' 'geometry' {geometry} ')'
bool CCr5kNonConductivePadStackPadLayer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokVisible);
   m_parameters.read(tokenReader);

   CCr5kReadElementList elementList;

   elementList.addZeroOrOne(m_padstackGeometries,ptokGeometry);

   tokenReader.readList(elementList);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
//CCr5kNonConductivePadStackPad::CCr5kNonConductivePadStackPad()
//{
//}
//
//void CCr5kNonConductivePadStackPad::empty()
//{
//   m_layerName.Empty();
//   m_parameters.empty();
//   m_padstackGeometries.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf: nonCondPadstackPad           ::= '(' 'nonConductive'     {nonCondPadstackPadLayer} ')'
//// bnf:    nonCondPadstackPadLayer      ::= '(' 'layer' layerName [visible] [padstackPadGeometry] ')'
//// bnf:       visible                      ::= '(' 'visible' booleanValue ')'
//// bnf:       padstackPadGeometry          ::= '(' 'geometry' {geometry} ')'
//bool CCr5kNonConductivePadStackPad::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   if (! tokenReader.getNextRequiredToken(ptokLayer))
//   {
//      tokenReader.skipList();
//      return false;
//   }
//
//   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
//   {
//      tokenReader.skipList();
//      return false;
//   }
//
//   m_parameters.registerKey(ptokVisible);
//   m_parameters.read(tokenReader);
//
//   if (tokenReader.getNextTokenTag() == ptokGeometry)
//   {
//      m_padstackGeometries.read(tokenReader);
//
//      retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//   }
//   else
//   {
//      tokenReader.ungetToken();
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kPcbPadstackHoleGeometry::CCr5kPcbPadstackHoleGeometry()
{
}

void CCr5kPcbPadstackHoleGeometry::empty()
{
   m_singleGeometry.empty();

   CCr5kElement::empty();
}

// bnf: pcbPadstackHoleGeometry ::= '(' 'hole' '(' 'geometry' geometry ')' ')'
bool CCr5kPcbPadstackHoleGeometry::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = false;

   if (tokenReader.getNextTokenTag() == ptokGeometry)
   {
      retval = m_singleGeometry.read(tokenReader) && retval;

      retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
   }
   else if (tokenReader.isNonTerminatingToken())
   {
      tokenReader.skipList();
   }

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kReferPrimitive::CCr5kReferPrimitive()
{
}

void CCr5kReferPrimitive::empty()
{
   m_parameters.empty();
   m_layoutPrimitive.empty();

   CCr5kElement::empty();
}

//// bnf: referPrim          ::= '(' 'refer' {layoutPrimInfo} layoutPrim ')'
// bnf: referPrim          ::= '(' 'refer' {layoutPrimInfo} layoutPrim {property} ')' - updated 20061215 - knv
// bnf:    layoutPrimInfo     ::= { <primNet> | <primDrcMode> | <primDeleteLock> | <primMoveLock> | <primMaterialNumber> | <infoTag> | <msgTag> | 
// bnf:                             <barechipPad> | <wirebondPad> | <thermalData> | <ignoreRebuildNet> | <side> } 
// bnf:       primNet            ::= '(' 'net'              netName ')'
// bnf:       primDrcMode        ::= '(' 'drc'              { 'ON' | 'OFF' | 'ERROR' } ')'
// bnf:       primDeleteLock     ::= '(' 'deleteLock'       booleanValue ')'
// bnf:       primMoveLock       ::= '(' 'moveLock'         booleanValue ')'
// bnf:       primMaterialNumber ::= '(' 'materialNumber    integerValue ')'
// bnf:       infoTag            ::= '(' 'infoTag'          integerValue ')'
// bnf:       msgTag             ::= '(' 'msgTag'           integerValue ')'
// bnf:       barechipPad        ::= '(' 'barechipPad'      booleanValue ')'
// bnf:       wirebondPad        ::= '(' 'wirebondPad'      booleanValue ')'
// bnf:       thermalData        ::= '(' 'thermalData'      booleanValue ')'
// bnf:       ignoreRebuildNet   ::= '(' 'ignoreRebuildNet' booleanValue ')'
// bnf:       side               ::= '(' 'side' { 'A' | 'B' } ')'  >>> added

bool CCr5kReferPrimitive::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   m_parameters.registerKey(ptokNet);
   m_parameters.registerKey(ptokDrc);
   m_parameters.registerKey(ptokDeleteLock);
   m_parameters.registerKey(ptokMoveLock);
   m_parameters.registerKey(ptokMaterialNumber);
   m_parameters.registerKey(ptokInfoTag);
   m_parameters.registerKey(ptokMsgTag);
   m_parameters.registerKey(ptokBareChipPad);
   m_parameters.registerKey(ptokWireBondPad);
   m_parameters.registerKey(ptokThermalData);
   m_parameters.registerKey(ptokIgnoreRebuildNet);
   m_parameters.registerKey(ptokSide);

   m_parameters.read(tokenReader);

   tokenReader.readZeroOrOneElements(m_layoutPrimitive);

   retval = m_properties.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kFootPadstackGroupSet::CCr5kFootPadstackGroupSet()
{
}

void CCr5kFootPadstackGroupSet::empty()
{
   m_padstackGroupName.Empty();
   m_parameters.empty();

   CCr5kElement::empty();
}

// bnf:    footPadstackGroupSet ::= '(' 'padstackGroup' padstackGroupName padstackNameRef ')'
// bnf:       padstackNameRef      ::= '(' 'padstack' padstackName ')'
bool CCr5kFootPadstackGroupSet::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_padstackGroupName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokPadstack);

   m_parameters.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kLayoutPrimitive::CCr5kLayoutPrimitive()
: m_units(pageUnitsUndefined)
, m_referPrimitive(NULL)
{
}

void CCr5kLayoutPrimitive::empty()
{
   m_name.Empty();
   m_parameters.empty();
   m_singleGeometry.empty();
   m_properties.empty();
   m_point.empty();
   m_fromTo.empty();
   m_testPad.empty();
   m_conductivePadStackPad.empty();
   m_nonConductivePadStackPad.empty();
   m_padstackHoleGeometry.empty();

   delete m_referPrimitive;
   m_referPrimitive = NULL;

   CCr5kElement::empty();
}

// bnf: layoutPrim ::= linePrim | surfacePrim | areaPrim | holePrim | ovalHolePrim | squareHolePrim | 
//                     textPrim | symbolPrim | referPrim | deletePrim | pcbPadPrim | pcbPadstackPrim | 
//                     footPadstackPosPrim | footPadPrim | dimensionPrim | dimLeaderPrim | shieldLinePrim |
//                     basePointPrim | boardAssyPrim | infoPrim | messagePrim | meshplanePrim |
//                     ruleAreaPrim | hiePortPrim

// bnf: linePrim        ::= '(' 'line'       layoutPrimInfo lineGeometry {property} ')'
// bnf:    lineGeometry    ::= '(' 'geometry' line ')'
// bnf:       line            ::= '(' 'line' { <lineType> |  <penShape> |  <dashLineParameters> } vertices ')'
// bnf: surfacePrim     ::= '(' 'surface'    layoutPrimInfo surfaceGeometry {property} ')'
// bnf:    surfaceGeometry ::= '(' 'geometry' surface ')'
// bnf:       surface         ::= '(' 'surface' { <outlineWidth> | <fillWidth> | <fillAngle> | <alreadySpread> } vertices {openShape} ')'
// bnf: areaPrim        ::= '(' 'area'       layoutPrimInfo { <areaUpperHeight> | <areaLowerHeight> } surfaceGeometry {property} ')'
// bnf: holePrim        ::= '(' 'hole'       layoutPrimInfo { <fromTo> | <drillType> | <holeType> } circleGeometry {property} ')'
// bnf:    circleGeometry  ::= '(' 'circle' circle ')'
// bnf: textPrim        ::= '(' 'text'       layoutPrimInfo [font] textGeometry {property} ')'
// bnf: pcbPadstackPrim ::= '(' 'padstack' padstackName layoutPrimInfo { <point> | <padstackAngle> | <fromTo> | <drillType> | <holeType> | <isClearance> | 
//                                                                       <isFixLandKind> | <isTestPad> | <isBuildupVia> | <condPadstackPad> | <nonCondPadstackPad> | 
//                                                                       <pcbPadstackHoleGeometry> } {property} ')'
// bnf: boardAssyPrim   ::= '(' 'boardAssembly' assyName layoutPrimInfo subBoardRef surfaceGeometry { <placementAngle> | <point> | <boardAssyFlip> } {property} ')'
// bnf:    assyName        ::= identifier
// bnf:    subBoardRef     ::= '(' 'subBoard' subBoardNumber ')'
// bnf:       subBoardNumber  ::= integerValue
// bnf:    placementAngle  ::= '(' 'angle'            integerValue ')'
// bnf:    boardAssyFlip   ::= '(' 'flip' ( 'NONE' | 'X' | 'Y' | 'XY' ) ')'
// bnf: footPadPrim     ::= '(' 'fPad'       padName { <point> | <padAngle> | <barechipPad> | <wirebondPad> } {property} ')'

// bnf: ovalHolePrim    ::= '(' 'ovalHole'   layoutPrimInfo [holeType] ovalGeometry {property} ')'
// bnf: squareHolePrim  ::= '(' 'squareHole' layoutPrimInfo squareGeometry {property} ')'
// bnf: symbolPrim      ::= '(' 'symbolText' layoutPrimInfo { <font> | <symbolTextType> } textGeometry {property} ')'
// bnf:    symbolTextType  ::= '(' 'NONE' | 'REFERENCE' | 'PARTNAME' ')'
// bnf: referPrim       ::= '(' 'refer'      layoutPrim ')'
// bnf: deletePrim      ::= '(' 'delete'     layoutPrim ')'
// bnf: pcbPadPrim          ::= '(' 'pad' padName layoutPrimInfo { <point> | <padAngle> | <padGeometry> } {property} ')'
// bnf: footPadstackPosPrim  ::= '(' 'fpadstack'  { <point> | <padstackAngle> } { <barechipPad> | <wirebondPad> } {footPadstackGroupSet} {property} ')'
// bnf:    padstackAngle        ::= '(' 'angle'            fAngle ')'
// bnf:    barechipPad          ::= '(' 'barechipPad'      booleanValue ')'
// bnf:    wirebondPad          ::= '(' 'wirebondPad'      booleanValue ')'
// bnf:    footPadstackGroupSet ::= '(' 'padstackGroup' padstackGroupName padstackNameRef ')'
// bnf:       padstackNameRef      ::= '(' 'padstack' padstackName ')'
// bnf: dimensionPrim       ::= '(' 'dimension'  ... ')'
// bnf: dimLeaderPrim       ::= '(' 'dimLeader'  ... ')'
// bnf: shieldLinePrim      ::= '(' 'shieldLine' ... ')'
// bnf: basePointPrim       ::= '(' 'basePoint'  ... ')'
// bnf: infoPrim            ::= '(' 'info'       ... ')'
// bnf: messagePrim         ::= '(' 'message'    ... ')'
// bnf: meshplanePrim       ::= '(' 'meshplane'  ... ')'
// bnf: ruleAreaPrim        ::= '(' 'ruleArea'   ... ')'
// bnf: hiePortPrim         ::= '(' 'hiePort'    ... ')'
bool CCr5kLayoutPrimitive::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   setUnits(tokenReader.getUnits());
   tokenReader.setSourceLineNumber(*this);

   m_primitiveType = tokenReader.getCurrentTokenTag();

   // bnf: layoutPrimInfo     ::= { <primNet> | <primDrcMode> | <primDeleteLock> | <primMoveLock> | <primMaterialNumber> | <infoTag> | <msgTag> | 
   // bnf:                          <barechipPad> | <wirebondPad> | <thermalData> | <ignoreRebuildNet> | <side> } 
   // bnf: primNet            ::= '(' 'net'              netName ')'
   // bnf: primDrcMode        ::= '(' 'drc'              { 'ON' | 'OFF' | 'ERROR' } ')'
   // bnf: primDeleteLock     ::= '(' 'deleteLock'       booleanValue ')'
   // bnf: primMoveLock       ::= '(' 'moveLock'         booleanValue ')'
   // bnf: primMaterialNumber ::= '(' 'materialNumber    integerValue ')'
   // bnf: infoTag            ::= '(' 'infoTag'          integerValue ')'
   // bnf: msgTag             ::= '(' 'msgTag'           integerValue ')'
   // bnf: barechipPad        ::= '(' 'barechipPad'      booleanValue ')'
   // bnf: wirebondPad        ::= '(' 'wirebondPad'      booleanValue ')'
   // bnf: thermalData        ::= '(' 'thermalData'      booleanValue ')'
   // bnf: ignoreRebuildNet   ::= '(' 'ignoreRebuildNet' booleanValue ')'
   // bnf: side               ::= '(' 'side' { 'A' | 'B' } ')'  >>> added

   // bnf: areaUpperHeight    ::= '(' 'upperHeight'      distance ')'
   // bnf: areaLowerHeight    ::= '(' 'lowerHeight'      distance ')'
   // bnf: drillType          ::= '(' 'drillType'        { 'NORMAL' | 'TEST_COUPON' | 'CHECK' } ')'
   // bnf: holeType           ::= '(' 'holeType'         integerValue ')'
   // bnf: font               ::= '(' 'font'             integerValue ')'
   // bnf: padstackAngle      ::= '(' 'angle'            fAngle ')'
   // bnf: isClearance        ::= '(' 'isClearance'      booleanValue ')'
   // bnf: isFixLandKind      ::= '(' 'isFixLandKind'    booleanValue ')'
   // bnf: isBuildupVia       ::= '(' 'isBuildupVia'     booleanValue ')'
   // bnf: placementAngle     ::= '(' 'angle'            integerValue ')'
   // bnf: boardAssyFlip      ::= '(' 'flip'             { 'NONE' | 'X' | 'Y' | 'XY' } ')'
   // bnf: padAngle           ::= '(' 'angle'            integerValue ')'
   // bnf: subBoardRef        ::= '(' 'subBoard'         integerValue ')'

   // bnf: fromTo                  ::= '(' 'fromTo'           layerNumber [layerNumber] ')'
   // bnf: point                   ::= '(' 'pt'               coordinateValue coordinateValue ')'
   // bnf: isTestPad               ::= '(' 'isTestPad'        booleanValue { <testPadSide> | <testPadId> | <testPoint> ')'
   // bnf:    testPadSide             ::= '(' 'side' { 'A' | 'B' } ')'
   // bnf:    testPadId               ::= '(' 'id' string ')'
   // bnf:    testPoint               ::= point
   // bnf: condPadstackPad         ::= '(' 'conductive'        {condPadstackPadLayer} ')'
   // bnf: nonCondPadstackPad      ::= '(' 'nonConductive'     {nonCondPadstackPadLayer} ')'
   // bnf: pcbPadstackHoleGeometry ::= '(' 'hole' '(' 'geometry' geometry ')' ')'

   m_parameters.registerKey(ptokNet);
   m_parameters.registerKey(ptokDrc);
   m_parameters.registerKey(ptokDeleteLock);
   m_parameters.registerKey(ptokMoveLock);
   m_parameters.registerKey(ptokMaterialNumber);
   m_parameters.registerKey(ptokInfoTag);
   m_parameters.registerKey(ptokMsgTag);
   m_parameters.registerKey(ptokBareChipPad);
   m_parameters.registerKey(ptokWireBondPad);
   m_parameters.registerKey(ptokThermalData);
   m_parameters.registerKey(ptokIgnoreRebuildNet);
   m_parameters.registerKey(ptokSide);
   m_parameters.registerKey(ptokUpperHeight);
   m_parameters.registerKey(ptokLowerHeight);
   m_parameters.registerKey(ptokDrillType);
   m_parameters.registerKey(ptokHoleType);
   m_parameters.registerKey(ptokFont);
   m_parameters.registerKey(ptokAngle);
   m_parameters.registerKey(ptokIsClearance);
   m_parameters.registerKey(ptokIsFixLandKind);
   m_parameters.registerKey(ptokIsBuildupVia);
   m_parameters.registerKey(ptokFlip);
   m_parameters.registerKey(ptokSubBoard);
   m_parameters.registerKey(ptokUver);
   m_parameters.registerKey(ptokHoleType);
   m_parameters.registerKey(ptokSubBoard);
   m_parameters.registerKey(ptokType);
   m_parameters.registerKey(ptokShieldLine);
   switch (m_primitiveType)
   {
   case ptokShieldLine:
   case ptokLine:
   case ptokSurface:
   case ptokArea:
   case ptokHole:
   case ptokText:
   case ptokSymbolText:
      // bnf: linePrim        ::= '(' 'line'       layoutPrimInfo lineGeometry {property} ')'
      // bnf: surfacePrim     ::= '(' 'surface'    layoutPrimInfo surfaceGeometry {property} ')'
      // bnf: areaPrim        ::= '(' 'area'       layoutPrimInfo { <areaUpperHeight> | <areaLowerHeight> } surfaceGeometry {property} ')'
      // bnf: holePrim        ::= '(' 'hole'       layoutPrimInfo { <fromTo> | <drillType> | <holeType> } circleGeometry {property} ')'
      // bnf: textPrim        ::= '(' 'text'       layoutPrimInfo [font] textGeometry {property} ')'
      // bnf: symbolPrim      ::= '(' 'symbolText' layoutPrimInfo { <font> | <symbolTextType> } textGeometry {property} ')'

      //m_parameters.read(tokenReader);

      //tokenReader.readOneElement(m_singleGeometry,ptokGeometry);

      //m_properties.read(tokenReader);

      {
         m_parameters.read(tokenReader);

         CCr5kReadElementList elementList1;
         CCr5kSkippedElement shieldNet;

         elementList1.addZeroOrOne(shieldNet     ,ptokShieldNet);
         tokenReader.readList(elementList1);

         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_singleGeometry,ptokGeometry);
         elementList.addZeroOrOne(m_fromTo        ,ptokFromTo);

         tokenReader.readElementsAndParameters(elementList,m_parameters);

         m_properties.read(tokenReader);
      }

      tokenReader.skipExpectedEmptyRemainderOfList();

      break;
   case ptokPadstack:
      // bnf: pcbPadstackPrim ::= '(' 'padstack' padstackName layoutPrimInfo { <point> | <padstackAngle> | <fromTo> | <drillType> | <holeType> | <isClearance> | 
      //                                                                       <isFixLandKind> | <isTestPad> | <isBuildupVia> | <condPadstackPad> | <nonCondPadstackPad> | 
      //                                                                       <pcbPadstackHoleGeometry> } {property} ')'
      // bnf: point              ::= '(' 'pt'               coordinateValue coordinateValue ')'
      // bnf: padstackAngle      ::= '(' 'angle'            fAngle ')'
      // bnf: fromTo             ::= '(' 'fromTo'           layerNumber [layerNumber] ')'
      // bnf: drillType          ::= '(' 'drillType'        { 'NORMAL' | 'TEST_COUPON' | 'CHECK' } ')'
      // bnf: holeType           ::= '(' 'holeType'         integerValue ')'
      // bnf: isClearance        ::= '(' 'isClearance'      booleanValue ')'
      // bnf: isFixLandKind      ::= '(' 'isFixLandKind'    booleanValue ')'
      // bnf: isTestPad             ::= '(' 'isTestPad'        booleanValue { <testPadSide> | <testPadId> | <testPoint> ')'
      // bnf:    testPadSide           ::= '(' 'side' { 'A' | 'B' } ')'
      // bnf:    testPadId             ::= '(' 'id' string ')'
      // bnf:    testPoint             ::= point
      // bnf: isBuildupVia       ::= '(' 'isBuildupVia'     booleanValue ')'
      // bnf: condPadstackPad         ::= '(' 'conductive'        {condPadstackPadLayer} ')'
      // bnf:    condPadstackPadLayer    ::= '(' 'layerNumber' layerNumber condPadstackPadStatus [padstackPadGeometry] ')'
      // bnf:       condPadstackPadStatus   ::= '(' 'status' { 'CONNECT' | 'NOCONNECT' | 'THERMAL' | 'CLEARANCE' | 'NOPAD' } ')'
      // bnf:       padstackPadGeometry     ::= '(' 'geometry' {geometry} ')'
      // bnf: nonCondPadstackPad      ::= '(' 'nonConductive'     {nonCondPadstackPadLayer} ')'
      // bnf:    nonCondPadstackPadLayer  ::= '(' 'layer' layerName [visible] [padstackPadGeometry] ')'
      // bnf:       visible                  ::= '(' 'visible' booleanValue ')'
      // bnf:       padstackPadGeometry      ::= '(' 'geometry' {geometry} ')'
      // bnf: pcbPadstackHoleGeometry ::= '(' 'hole' '(' 'geometry' geometry ')' ')'
      {
         if (! tokenReader.getNextRequiredNonListToken(m_name))
         {
            tokenReader.skipList();
            return false;
         }

         m_parameters.read(tokenReader);

         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_point                   ,ptokPt);
         elementList.addZeroOrOne(m_fromTo                  ,ptokFromTo);
         elementList.addZeroOrOne(m_testPad                 ,ptokIsTestPad);
         elementList.addZeroOrOne(m_conductivePadStackPad   ,ptokConductive);
         elementList.addZeroOrOne(m_nonConductivePadStackPad,ptokNonConductive);
         elementList.addZeroOrOne(m_padstackHoleGeometry    ,ptokHole);

         tokenReader.readElementsAndParameters(elementList,m_parameters);

         m_properties.read(tokenReader);
      }

      tokenReader.skipExpectedEmptyRemainderOfList();

      break;
   case ptokBoardAssembly:
      // bnf: boardAssyPrim   ::= '(' 'boardAssembly' assyName layoutPrimInfo subBoardRef surfaceGeometry { <placementAngle> | <point> | <boardAssyFlip> } {property} ')'
      {
         if (! tokenReader.getNextRequiredNonListToken(m_name))
         {
            tokenReader.skipList();
            return false;
         }

         //m_parameters.read(tokenReader);

         //tokenReader.readOneElement(m_singleGeometry,ptokGeometry);

         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_point         ,ptokPt);
         elementList.addZeroOrOne(m_singleGeometry,ptokGeometry);

         tokenReader.readElementsAndParameters(elementList,m_parameters);

         m_properties.read(tokenReader);
      }

      tokenReader.skipExpectedEmptyRemainderOfList();

      break;
   case ptokFpad:
      // bnf: footPadPrim     ::= '(' 'fPad' padName { <point> | <padAngle> | <barechipPad> | <wirebondPad> } {property} ')'
      {
         if (! tokenReader.getNextRequiredNonListToken(m_name))
         {
            tokenReader.skipList();
            return false;
         }

         m_parameters.read(tokenReader);

         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_point                   ,ptokPt);

         tokenReader.readElementsAndParameters(elementList,m_parameters);

         m_properties.read(tokenReader);
      }

      tokenReader.skipExpectedEmptyRemainderOfList();

      break;
   case ptokPad:
      // bnf: pcbPadPrim      ::= '(' 'pad' padName layoutPrimInfo { <point> | <padAngle> | <padGeometry> } {property} ')'
      {
         if (! tokenReader.getNextRequiredNonListToken(m_name))
         {
            tokenReader.skipList();
            return false;
         }

         m_parameters.read(tokenReader);

         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_point         ,ptokPt);
         elementList.addZeroOrOne(m_singleGeometry,ptokGeometry);

         tokenReader.readElementsAndParameters(elementList,m_parameters);

         m_properties.read(tokenReader);
      }

      tokenReader.skipExpectedEmptyRemainderOfList();

      break;
   case ptokRefer:
      // bnf: referPrim       ::= '(' 'refer'      layoutPrim ')'
      if (m_referPrimitive == NULL)
      {
         m_referPrimitive = new CCr5kReferPrimitive();
      }

      m_referPrimitive->read(tokenReader);

      break;
   case ptokFpadstack:
      // bnf: footPadstackPosPrim  ::= '(' 'fpadstack'  { <point> | <padstackAngle> } { <barechipPad> | <wirebondPad> } {footPadstackGroupSet} {property} ')'
      // bnf:    padstackAngle        ::= '(' 'angle'            fAngle ')'
      // bnf:    barechipPad          ::= '(' 'barechipPad'      booleanValue ')'
      // bnf:    wirebondPad          ::= '(' 'wirebondPad'      booleanValue ')'
      // bnf:    footPadstackGroupSet ::= '(' 'padstackGroup' padstackGroupName padstackNameRef ')'
      // bnf:       padstackNameRef      ::= '(' 'padstack' padstackName ')'
      {
         CCr5kReadElementList elementList;

         elementList.addZeroOrOne(m_point               ,ptokPt);
         elementList.addZeroOrOne(m_footPadstackGroupSet,ptokPadstackGroup);

         tokenReader.readElementsAndParameters(elementList,m_parameters);

         m_properties.read(tokenReader);
      }

      tokenReader.skipExpectedEmptyRemainderOfList();

      break;
   case ptokOvalHole:
   case ptokSquareHole:
   case ptokDelete:
   case ptokDimension:
   case ptokDimLeader:
   case ptokBasePoint:
   case ptokInfo:
   case ptokMessage:
   case ptokMeshplane:
   case ptokRuleArea:
   case ptokHiePort:
      //if (m_primitiveType == ptokBasePoint)
      //{
      //   int iii = 3;
      //}

      tokenReader.skipList();
      return true;
      break;
   default:
      //tokenReader.ungetToken();
      tokenReader.skipList();
      return true;
      break;
   }

   return true;
}

CBasesVector CCr5kLayoutPrimitive::getPadstackBasesVector() const
{
   bool mirror    = false;   //   getPlacedBottom();
   double degrees = m_parameters.getDoubleValue(ptokAngle,0.);
   double x       = m_units.convertFromPortUnits(m_point.getX());
   double y       = m_units.convertFromPortUnits(m_point.getY());

   CBasesVector basesVector(x,y,degrees,mirror);

   return basesVector;
}

bool CCr5kLayoutPrimitive::isTestPad() const
{
   bool retval = m_testPad.isTestPad();

   return retval;
}

CString CCr5kLayoutPrimitive::getTestPadReference() const
{
   CString testPadReference = m_testPad.getTestPadReference();

   return testPadReference;
}

//CString CCr5kLayoutPrimitive::getViaReference() const
//{
//}

bool CCr5kLayoutPrimitive::getFromToLayers(int& fromLayerNumber,int& toLayerNumber) const
{
   bool retval = m_fromTo.isLoaded();

   if (retval)
   {
      fromLayerNumber = m_fromTo.getFromLayerNumber();
      toLayerNumber   = m_fromTo.getToLayerNumber();
   }

   return retval;
}

bool CCr5kLayoutPrimitive::getPlacedTop() const
{
   Cr5kTokenTag side = m_parameters.getTokenValue(ptokSide,tokA);
   bool placedTop = (side != tokB);

   return placedTop;
}

bool CCr5kLayoutPrimitive::hasSide() const
{
   bool retval = m_parameters.hasValueForKey(ptokSide);

   //CString debugContentsString = m_parameters.getContentsString();

   return retval;
}


bool CCr5kLayoutPrimitive::hasPadGeometry() const
{
   // Return true if either conductive or non-conductive pads have geometry in this primitive.

   return (hasConductivePadGeometry() || hasNonConductivePadGeometry());
}

bool CCr5kLayoutPrimitive::hasConductivePadGeometry() const
{
   // Return true if conductive pad has geometry in this primitive.

   const CCr5kConductivePadStackPad &condPdstkPad = this->getConductivePadStackPad();
   POSITION pos = condPdstkPad.getHeadPosition();
   while (pos != NULL)
   {
      CCr5kConductivePadStackPadLayer *cpsPadLayer = condPdstkPad.getNext(pos);
      if (cpsPadLayer != NULL)
      {
         const CCr5kGeometries &geoms = cpsPadLayer->getGeometries();
         if (geoms.getCount() > 0)
            return true;
      }
   }

   return false;
}

bool CCr5kLayoutPrimitive::hasNonConductivePadGeometry() const
{
   // Return true if non-conductive pad has geometry in this primitive.

   const CCr5kNonConductivePadStackPad &nonCondPdstkPad = this->getNonConductivePadStackPad();
   POSITION pos = nonCondPdstkPad.getHeadPosition();
   while (pos != NULL)
   {
      CCr5kNonConductivePadStackPadLayer *cpsPadLayer = nonCondPdstkPad.getNext(pos);
      if (cpsPadLayer != NULL)
      {
         const CCr5kGeometries &geoms = cpsPadLayer->getGeometries();
         if (geoms.getCount() > 0)
            return true;
      }
   }

   return false;
}

//_____________________________________________________________________________
//CCr5kLayoutLayer::CCr5kLayoutLayer()
//{
//}
//
//void CCr5kLayoutLayer::empty()
//{
//   m_layerIdentifier.empty();
//   m_primitiveList.empty();
//
//   CCr5kElement::empty();
//}

// bnf: layoutLayer ::= '(' 'layer' layerIdentifier {layoutPrim} ')'
bool CCr5kLayoutLayer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   // bnf: layerIdentifier ::= condLayerRef | nonCondLayerRef | subLayerRef | footprintLayerRef |
   //                           footprintPadstackLayerRef
   //      condLayerRef    ::= '(' 'conductive' layerNumber ')'
   //      nonCondLayerRef ::= '(' 'nonConductive' layerName ')'
   //      footprintLayerRef ::= '(' 'footLayer' layerName ')'
   //      footprintPadstackLayerRef ::= '(' 'FPADSTACK' ')'
   //      subLayerRef     ::= systemLayerRef | drawLayerRef | infoLayerRef

   //      systemLayerRef  ::= '(' 'systemLayer' systemLayerType ')'
   //      drawLayerRef    ::= '(' 'drawOf' referedBy ')'
   //      infoLayerRef    ::= '(' 'infoOf' referedBy ')'

   //      referedBy       ::= condLayerRef | nonCondLayerRef | systemLayerRef

   //      systemLayerType ::= '(' 'type' systemLayerTypeEnum ')'
   //      systemLayerTypeEnum ::= 'BOARD_FIGURE' | 'PADSTACK' | 'LAYOUT_AREA' | 'COMP_GROUP' | 'COMP_GROUP_B' | 'BASEPOINT' | 


   retval = tokenReader.readOneElement(m_layerIdentifier,tok_Cr5kTokenTagUndefined);

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kLayoutPrimitive* layoutPrimitive = new CCr5kLayoutPrimitive();

      // bnf: layoutPrim ::= linePrim | surfacePrim | areaPrim | holePrim | ovalHolePrim | squareHolePrim | 
      //                     textPrim | symbolPrim | referPrim | deletePrim | pcbPadPrim | pcbPadstackPrim | 
      //                     footPadstackPosPrim | footPadPrim | dimensionPrim | dimLeaderPrim | shieldLinePrim |
      //                     basePointPrim | boardAssyPrim | infoPrim | messagePrim | meshplanePrim |
      //                     ruleAreaPrim | hiePortPrim

      // bnf: linePrim ::= '(' 'line' layoutPrimInfo lineGeometry {property} ')'
      if (tokenReader.readZeroOrOneElements(*layoutPrimitive))
      {
         m_elementList.AddTail(layoutPrimitive);
      }
      else
      {
         delete layoutPrimitive;
         loopFlag = false;
      }
   }


   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

////_____________________________________________________________________________
//CCr5kLayout::CCr5kLayout()
//{
//}
//
//void CCr5kLayout::empty()
//{
//   m_layoutLayerList.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf: layout ::= '(' 'layout' {layoutLayer} ')'
//bool CCr5kLayout::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kLayoutLayer* layoutLayer = new CCr5kLayoutLayer();
//
//      // bnf: version ::= '(' 'layer' layerIdentifier {layoutPrim} ')'
//      if (tokenReader.readZeroOrOneElements(*layoutLayer,ptokLayer))
//      {
//         m_layoutLayerList.AddTail(layoutLayer);
//      }
//      else
//      {
//         delete layoutLayer;
//         loopFlag = false;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kHeelprint::CCr5kHeelprint()
{
}

void CCr5kHeelprint::empty()
{
   m_layout.empty();

   CCr5kElement::empty();
}

// bnf: heelprint ::= '(' 'heelprint' [minRect] layout ')'
bool CCr5kHeelprint::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   //if (tokenReader.getLineNumber() >= 110721)
   //{
   //   int iii = 3;
   //}

   bool retval = true;

   CCr5kSkippedElement minRect;

   tokenReader.readZeroOrOneElements(minRect ,ptokMinRect);

   // bnf: layout ::= '(' 'layout' {layoutLayer} ')'
   retval = tokenReader.readOneElement(m_layout ,ptokLayout) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kBondwire::CCr5kBondwire()
{
}

void CCr5kBondwire::empty()
{
   m_startPoint.empty();
   m_endPoint.empty();
   m_parameters.empty();

   CCr5kElement::empty();
}

// bnf:       bondwire       ::= '(' 'bondwire' bondwireNumber startPoint endPoint [bwIsGenerated] ')'
// bnf:          bondwireNumber ::= integerValue
// bnf:          startPoint     ::= point
// bnf:          endPoint       ::= point
// bnf:          bwIsGenerated  ::= '(' 'bwIsGenerated' booleanValue ')' >>> added
bool CCr5kBondwire::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredInteger(m_bondwireNumber))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredToken(ptokPt))
   {
      tokenReader.skipList();
      return false;
   }

   m_startPoint.read(tokenReader);

   if (! tokenReader.getNextRequiredToken(ptokPt))
   {
      tokenReader.skipList();
      return false;
   }

   m_endPoint.read(tokenReader);

   m_parameters.registerKey(ptokBwIsGenerated);

   m_parameters.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kBondwires::CCr5kBondwires()
: m_units(pageUnitsUndefined)
{
}

void CCr5kBondwires::empty()
{
   m_parameters.empty();

   CTypedCr5kElementContainer<CCr5kBondwire,ptokBondwire>::empty();
}

// bnf:    bondwires      ::= '(' 'bondwire' { <materialNumber> | <bwDiameter> | <bwCountPcb> } {bondwire} ')'
// bnf:       materialNumber ::= '(' 'materialNumber' integerValue ')'
// bnf:       bwDiameter     ::= '(' 'bwDiameter'     integerValue ')'
// bnf:       bwCountPcb     ::= '(' 'bwCountPCB'     integerValue ')' >>> added
// bnf:       bondwire       ::= '(' 'bondwire' bondwireNumber startPoint endPoint [bwIsGenerated]')'
// bnf:          bondwireNumber ::= integerValue
// bnf:          startPoint     ::= point
// bnf:          endPoint       ::= point
// bnf:          bwIsGenerated  ::= '(' 'bwIsGenerated' booleanValue ')' >>> added
bool CCr5kBondwires::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   setUnits(tokenReader.getUnits());

   bool retval = true;

   m_parameters.registerKey(ptokMaterialNumber);
   m_parameters.registerKey(ptokBwDiameter);
   m_parameters.registerKey(ptokBwCountPcb);

   m_parameters.read(tokenReader);

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kBondwire* bondwire = new CCr5kBondwire();

      if (tokenReader.readZeroOrOneElements(*bondwire,ptokBondwire))
      {
         m_elementList.AddTail(bondwire);
      }
      else
      {
         delete bondwire;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kToeprintPin::CCr5kToeprintPin()
{
}

void CCr5kToeprintPin::empty()
{
   m_pinNumber.Empty();
   m_point.empty();
   m_bondwires.empty();
   m_layout.empty();
   m_exitDirection.empty();

   CCr5kElement::empty();
}

// bnf: toeprint       ::= '(' 'pin' pinNumber { <point> | <minRect> | <bondwires> } layout | exitDirection')'
// bnf:    pinNumber      ::= identifier
// bnf:    bondwires      ::= '(' 'bondwire' { <materialNumber> | <bwDiameter> } {bondwire} ')'
// bnf:       materialNumber ::= '(' 'materialNumber' integerValue ')'
// bnf:       bwDiameter     ::= '(' 'bwDiameter'     integerValue ')'
// bnf:       bondwire       ::= '(' 'bondwire' bondwireNumber startPoint endPoint ')'
// bnf:          bondwireNumber ::= integerValue
// bnf:          startPoint     ::= point
// bnf:          endPoint       ::= point
bool CCr5kToeprintPin::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_pinNumber))
   {
      tokenReader.skipList();
      return false;
   }

   CCr5kReadElementList elementList;
   CCr5kSkippedElement minRect;

   elementList.addZeroOrOne(minRect          ,ptokMinRect);
   elementList.addZeroOrOne(m_point          ,ptokPt);
   elementList.addZeroOrOne(m_bondwires      ,ptokBondwire);
   elementList.addZeroOrOne(m_exitDirection  ,ptokexitDirection);

   retval = tokenReader.readList(elementList) && retval;

   // bnf: layout ::= '(' 'layout' {layoutLayer} ')'
   retval = tokenReader.readOneElement(m_layout ,ptokLayout) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kToeprint::CCr5kToeprint()
{
}

void CCr5kToeprint::empty()
{
   m_pinList.empty();

   CCr5kElement::empty();
}

// bnf: toeprints ::= '(' 'toeprint' {toeprint} ')'
bool CCr5kToeprint::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   // bnf: toeprint     ::= '(' 'pin' pinNumber { <point> | <minRect> } layout ')'
   // bnf:    pinNumber    ::= identifier
   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kToeprintPin* pin = new CCr5kToeprintPin();

      // bnf:  
      if (tokenReader.readZeroOrOneElements(*pin,ptokPin))
      {
         m_pinList.AddTail(pin);
      }
      else
      {
         delete pin;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kMountBasePoint::CCr5kMountBasePoint()
{
}

void CCr5kMountBasePoint::empty()
{
   m_mountBasePoint.empty();

   CCr5kElement::empty();
}

// bnf:          mountBasePoint     ::= '(' 'mountBasePoint' point ')'
bool CCr5kMountBasePoint::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   if (! tokenReader.getNextRequiredToken(ptokPt))
   {
      tokenReader.skipList();
      return false;
   }

   bool retval = m_mountBasePoint.read(tokenReader);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   return retval;
}

//_____________________________________________________________________________
CCr5kMachineName::CCr5kMachineName()
{
}

void CCr5kMachineName::empty()
{
   m_machineName.Empty();
   m_mountBasePoints.empty();

   CCr5kElement::empty();
}

// bnf:       machineName        ::= '(' 'machineName' machineNameIdentifier mountBasePoint [ mountBasePoint ] ')'
// bnf:          mountBasePoint     ::= '(' 'mountBasePoint' point ')'
bool CCr5kMachineName::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   if (! tokenReader.getNextRequiredNonListToken(m_machineName))
   {
      tokenReader.skipList();
      return false;
   }

   bool retval = m_mountBasePoints.read(tokenReader);

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kMounterInformation::CCr5kMounterInformation()
{
}

void CCr5kMounterInformation::empty()
{
   m_machineNames.empty();

   CCr5kElement::empty();
}

// bnf:    mounterInformation ::= '(' 'mounterInformation' machineName [ machineName ] ')'
// bnf:       machineName        ::= '(' 'machineName' machineNameIdentifier mountBasePoint [ mountBasePoint ] ')'
// bnf:          mountBasePoint     ::= '(' 'mountBasePoint' point ')'
bool CCr5kMounterInformation::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = m_machineNames.read(tokenReader);

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kFootprint::CCr5kFootprint()
{
}

void CCr5kFootprint::empty()
{
   m_heelprint.empty();
   m_toeprint.empty();

   CCr5kElement::empty();
}

// bnf: version ::= '(' 'footprint' footprintName { <createUser> | <lastEditUser> | 
//                   <createTime> | <lastEditTime> | <mounterInformation> | <figureArea> | 
//                   <gridInfo> | <minRect> | <polarity> | <panelUse> } 
//                   heelprint toeprints { property } ')'
// bnf:    mounterInformation ::= '(' 'mounterInformation' machineName [ machineName ] ')'
// bnf:       machineName        ::= '(' 'machineName' machineNameIdentifier mountBasePoint [ mountBasePoint ] ')'
// bnf:          mountBasePoint     ::= '(' 'mountBasePoint' point ')'
bool CCr5kFootprint::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_name))
   {
      tokenReader.skipList();
      retval = false;
   }

   CCr5kReadElementList elementList;
   CCr5kSkippedElement createUser,lastEditUser,userVersion,createTime,lastEditTime,mountInformation,
                       figureArea,gridInfo,minRect,polarity,panelUse,dimensionParameters;

   elementList.addZeroOrOne(createUser          ,ptokCreateUser);
   elementList.addZeroOrOne(lastEditUser        ,ptokLastEditUser);
   elementList.addZeroOrOne(userVersion         ,ptokUver);
   elementList.addZeroOrOne(createTime          ,ptokCreateTime);
   elementList.addZeroOrOne(lastEditTime        ,ptokLastEditTime);
   elementList.addZeroOrOne(mountInformation    ,ptokMountInformation);
   elementList.addZeroOrOne(m_mounterInformation,ptokMounterInformation);
   elementList.addZeroOrOne(figureArea          ,ptokArea);
   elementList.addZeroOrOne(gridInfo            ,ptokGrid);
   elementList.addZeroOrOne(minRect             ,ptokMinRect);
   elementList.addZeroOrOne(polarity            ,ptokPolarity);
   elementList.addZeroOrOne(panelUse            ,ptokPanelUse);
   elementList.addZeroOrOne(dimensionParameters ,ptokDimensionParameters);

   retval = tokenReader.readList(elementList) && retval;

   // bnf: heelprint ::= '(' 'heelprint' [minRect] layout ')'
   retval = tokenReader.readOneElement(m_heelprint,ptokHeelprint) && retval;

   // bnf: toeprints ::= '(' 'toeprint' {toeprint} ')'
   retval = tokenReader.readOneElement(m_toeprint,ptokToeprint) && retval;

   retval = m_properties.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kFootprints::CCr5kFootprints()
{
}

void CCr5kFootprints::empty()
{
   m_footPrintList.empty();

   CCr5kElement::empty();
}

// bnf: version ::= '(' 'footprints' {footprint} ')'
bool CCr5kFootprints::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kFootprint* footprint = new CCr5kFootprint();

      // bnf: version ::= '(' 'footprint' footprintName { <createUser> | <lastEditUser> | 
      //                   <createTime> | <lastEditTime> | <mountInformation> | <figureArea> | 
      //                   <gridInfo> | <minRect> | <polarity> | <panelUse> } 
      //                   heelprint toeprints { property} ')'
      if (tokenReader.readZeroOrOneElements(*footprint,ptokFootprint))
      {
         m_footPrintList.AddTail(footprint);
      }
      else
      {
         delete footprint;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kFootprints::linkFootprints(CCr5kLinkedFootprints& linkedFootprints,COperationProgress* progress)
{
   for (POSITION pos = m_footPrintList.GetHeadPosition();pos != NULL;)
   {
      CCr5kFootprint* footprint = m_footPrintList.GetNext(pos);

      //if (footprint->getName().CompareNoCase("M.10.R.05.AT_S070021_1") == 0)
      //{
      //   int iii = 3;
      //}

      linkedFootprints.add(*footprint);

      if (progress != NULL) progress->incrementProgress();
   }
}

//_____________________________________________________________________________
CCr5kFootprintContainer::CCr5kFootprintContainer()
{
}

void CCr5kFootprintContainer::empty()
{
   m_footContainerCommonTable.empty();
   m_padstackGroups.empty();
   m_pads.empty();
   m_padstacks.empty();
   m_footprints.empty();

   CCr5kElement::empty();
}

// bnf: footprintContainer ::= '(' 'footprintContainer' [footContainerCommonTable] [padstackGroups] [pads] [padstacks] [footprints] ')'
bool CCr5kFootprintContainer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   // bnf: footContainerCommonTable ::= '(' 'commonParameters' dimensionParameters ')'
   tokenReader.readZeroOrOneElements(m_footContainerCommonTable,ptokCommonParameters);

   // bnf: padstackGroups ::= '(' 'padstackGroups' padstackGroup ')'
   tokenReader.readZeroOrOneElements(m_padstackGroups,ptokPadstackGroups);

   // bnf: pads ::= '(' 'pads' {pad} ')'
   tokenReader.readZeroOrOneElements(m_pads,ptokPads);

   // bnf: padstacks ::= '(' 'padstacks' {padstack} ')'
   tokenReader.readZeroOrOneElements(m_padstacks,ptokPadstacks);

   // bnf: footprints ::= '(' 'footprints' {footprint} ')'
   tokenReader.readZeroOrOneElements(m_footprints,ptokFootprints);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kFootprintContainer::linkPads(CCr5kLinkedPads& linkedPads,COperationProgress* progress)
{
   m_pads.linkPads(linkedPads,progress);
}

void CCr5kFootprintContainer::linkPadstacks(CCr5kLinkedPadstacks& linkedPadstacks,COperationProgress* progress)
{
   m_padstacks.linkPadstacks(linkedPadstacks,progress);
}

void CCr5kFootprintContainer::linkFootprints(CCr5kLinkedFootprints& linkedFootprints,COperationProgress* progress)
{
   m_footprints.linkFootprints(linkedFootprints,progress);
}

//_____________________________________________________________________________
CCr5kFootprintFile::CCr5kFootprintFile()
{
}

void CCr5kFootprintFile::empty()
{
   m_header.empty();
   m_footprintContainer.empty();

   CCr5kElement::empty();
}

// bnf: $ftf                ::= '(' ftf [ftfHeader] technologyContainer footprintContainer ')'
// bnf:    header              ::= '(' 'header' version { <unitDescriptor> | <timeZone> } ')'
// bnf:    technologyContainer ::= '(' 'technologyContainer' {technology} ')'
// bnf:    footprintContainer  ::= '(' 'footprintContainer' [footContainerCommonTable] [padstackGroups] [pads] [padstacks] [footprints] ')'
bool CCr5kFootprintFile::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   tokenReader.readZeroOrOneElements(m_header,ptokHeader);
   CCr5kSkippedElement     technologyContainer;

   retval = tokenReader.readOneElement(technologyContainer,ptokTechnologyContainer) && retval;

   retval = tokenReader.readOneElement(m_footprintContainer,ptokFootprintContainer) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kFootprintFile::linkPads(CCr5kLinkedPads& linkedPads,COperationProgress* progress)
{
   m_footprintContainer.linkPads(linkedPads,progress);
}

void CCr5kFootprintFile::linkPadstacks(CCr5kLinkedPadstacks& linkedPadstacks,COperationProgress* progress)
{
   m_footprintContainer.linkPadstacks(linkedPadstacks,progress);
}

void CCr5kFootprintFile::linkFootprints(CCr5kLinkedFootprints& linkedFootprints,COperationProgress* progress)
{
   m_footprintContainer.linkFootprints(linkedFootprints,progress);
}

//_____________________________________________________________________________
CCr5kFootprintTechnology::CCr5kFootprintTechnology()
{
}

void CCr5kFootprintTechnology::empty()
{
   m_footprintId.Empty();
   m_technologyId.Empty();

   CCr5kElement::empty();
}

// bnf:             footprintNameRef            ::= '(' 'footprint'          identifier   [footprintTechnology] ')'
// bnf:                footprintTechnology         ::= '(' 'technology'        identifier   ')'
// bnf:             BsideFootprintNameRef       ::= '(' 'reverseFootprint'   identifier   [footprintTechnology] ')'
bool CCr5kFootprintTechnology::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_footprintId))
   {
      tokenReader.skipList();
      return false;
   }

   if (tokenReader.getNextTokenTag() == ptokTechnology)
   {
      if (! tokenReader.getNextRequiredNonListToken(m_technologyId))
      {
         tokenReader.skipList();
         tokenReader.skipList();
         return false;
      }

      retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
   }
   else
   {
      tokenReader.ungetToken();
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kCompPin::CCr5kCompPin()
: m_isThruHole(boolUnknown)
{
}

void CCr5kCompPin::empty()
{
   m_pinNumber.Empty();
   m_point.empty();
   m_parameters.empty();
   m_layout.empty();
   m_bondwires.empty();
   m_gatePinRefs.empty();
   m_exitDirection.empty();

   m_isThruHole = boolUnknown;

   CCr5kElement::empty();
}

// bnf:             compPin                     ::= '(' 'pin' pinNumber [point] { <temporaryConnectFlag> | gatePinRef | <layout> ')'
// bnf:                temporaryConnectFlag        ::= '(' 'tempConnect' booleanValue ')'
// bnf:                gatePinRef                  ::= '(' 'gate' gateNumber gatePin ')'
// bnf:                exitDirection               ::= '(' 'exitDirection' direction {N:E:W:S}')'
// bnf:                   gateNumber                  ::= integerValue
// bnf:                   gatePin                     ::= '(' 'name' funcPinName ')'
// bnf:             layout                      ::= '(' 'layout' {layoutLayer} ')'
// bnf:                layoutLayer                 ::= '(' 'layer' layerIdentifier {layoutPrim} ')'
bool CCr5kCompPin::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_pinNumber))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokTempConnect);

   CCr5kReadElementList elementList;

   elementList.addZeroOrOne(m_point          ,ptokPt);
   elementList.addZeroOrOne(m_gatePinRefs    ,ptokGate);
   elementList.addZeroOrOne(m_layout         ,ptokLayout);
   elementList.addZeroOrOne(m_bondwires      ,ptokBondwire);
   elementList.addZeroOrOne(m_exitDirection  ,ptokexitDirection);

   tokenReader.readElementsAndParameters(elementList,m_parameters);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

bool CCr5kCompPin::isThruHole() const
{
   if (m_isThruHole == boolUnknown)
   {
      for (POSITION layerPos = m_layout.getHeadPosition();layerPos != NULL;)
      {
         CCr5kLayoutLayer* layer = m_layout.getNext(layerPos);

         for (POSITION primitivePos = layer->getHeadPosition();primitivePos != NULL;)
         {
            CCr5kLayoutPrimitive* primitive = layer->getNext(primitivePos);

            if (primitive->isThruHole())
            {
               m_isThruHole = boolTrue;
               break;
            }
         }
      }
   }

   return (m_isThruHole == boolTrue);
}

CString CCr5kCompPin::getNetName() const
{
   if (m_netName.IsEmpty())
   {
      for (POSITION layerPos = m_layout.getHeadPosition();layerPos != NULL;)
      {
         CCr5kLayoutLayer* layer = m_layout.getNext(layerPos);

         for (POSITION primitivePos = layer->getHeadPosition();primitivePos != NULL;)
         {
            CCr5kLayoutPrimitive* primitive = layer->getNext(primitivePos);

            m_netName = primitive->getNetName();

            if (! m_netName.IsEmpty())
            {
               break;
            }
         }
      }

      if (m_netName.IsEmpty())
      {
         m_netName = NET_UNUSED_PINS;
      }
   }

   return m_netName;
}

CBasesVector CCr5kCompPin::getBasesVector()
{
   bool mirror     = false;
   double degrees  = m_parameters.getDoubleValue(ptokAngle,0.);
   CPoint2d origin = m_point.getPoint();

   CBasesVector basesVector(origin.x,origin.y,degrees,mirror);

   return basesVector;
}

//_____________________________________________________________________________
CCr5kCompPinReferenceMap::CCr5kCompPinReferenceMap(const CCr5kCompPins& compPins)
{
   m_pinMap.InitHashTable(nextPrime2n(min((int)(compPins.getCount() * 1.2),10)));

   for (POSITION pos = compPins.getHeadPosition();pos != NULL;)
   {
      CCr5kCompPin* compPin = (CCr5kCompPin*)compPins.getNext(pos);

      m_pinMap.SetAt(compPin->getPinNumber(),compPin);
   }
}

const CCr5kCompPin* CCr5kCompPinReferenceMap::getAt(const CString& pinName)
{
   CCr5kCompPin* compPin;

   if (! m_pinMap.Lookup(pinName,compPin))
   {
      compPin = NULL;
   }

   return compPin;
}

//_____________________________________________________________________________
CCr5kComponent::CCr5kComponent()
: m_units(pageUnitsUndefined)
, m_isThruHole(boolUnknown)
, m_componentHeight(-1.)
{
}

void CCr5kComponent::empty()
{
   m_parameters.empty();
   m_footprint.empty();
   m_reverseFootprint.empty();
   m_location.empty();
   m_layout.empty();
   m_compPins.empty();
   m_compGates.empty();

   m_isThruHole      = boolUnknown;
   m_componentHeight = -1.;

   CCr5kElement::empty();
}

// bnf:          component            ::= '(' 'component' referenceDesignator [compPinCount] [compGateCount] 
//                                            { <infoTag> | <msgTag> | <partNameRef> | <stockIdRef> | <packageNameRef> | <footprintSpec> | <footprintNameRef> | 
//                                              <BsideFootprintNameRef> | <originalReferenceDesignator> | <placed> | <placementSide> | <locationLock> | <angleLock> | 
//                                              <placementSideLock> | <jumperAttr> | <bypassCapacitorAttr> | <fixedAttr> | <isGenerated> | <packageSymbolAttr> | 
//                                              <outOfBoardAttr> | <outOfBoardLocation> | <location> | <minRect> | <placementAngle> | <drawRefDes> | 
//                                              compGate | compPin | 
//                                              <bypassCapacitorRef> | <layout> | property } ')'

// bnf:             referenceDesignator         ::= '(' 'reference'          reference    ')'
// bnf:             compPinCount                ::= '(' 'pinCount'           integerValue ')'
// bnf:             compGateCount               ::= '(' 'gateCount'          integerValue ')'
// bnf:             infoTag                     ::= '(' 'infoTag'            integerValue ')'
// bnf:             msgTag                      ::= '(' 'msgTag'             integerValue ')'
// bnf:             partNameRef                 ::= '(' 'part'               identifier   ')'
// bnf:             stockIdRef                  ::= '(' 'stockId'            identifier   ')'
// bnf:             packageNameRef              ::= '(' 'package'            identifier   ')'
// bnf:             footprintSpec               ::= '(' 'footprintSpec'      identifier   ')'
// bnf:             footprintNameRef            ::= '(' 'footprint'          identifier   [footprintTechnology] ')'
// bnf:                footprintTechnology         ::= '(' 'technology'        identifier   ')'
// bnf:             BsideFootprintNameRef       ::= '(' 'reverseFootprint'   identifier   [footprintTechnology] ')'
// bnf:             originalReferenceDesignator ::= '(' 'originalReference'  identifier   ')'
// bnf:             placed                      ::= '(' 'placed'             booleanValue ')'
// bnf:             placementSide               ::= '(' 'placementSide'     ( 'A' | 'B' ) ')'
// bnf:             locationLock                ::= '(' 'locationLock'       booleanValue ')'
// bnf:             angleLock                   ::= '(' 'angleLock'          booleanValue ')'
// bnf:             placementSideLock           ::= '(' 'placementSideLock'  booleanValue ')'
// bnf:             jumperAttr                  ::= '(' 'jumper'             booleanValue ')'
// bnf:             fixedAttr                   ::= '(' 'fixed'              booleanValue ')'
// bnf:             isGenerated                 ::= '(' 'isGenerated'        booleanValue ')'
// bnf:             packageSymbolAttr           ::= '(' 'packageSymbol'      booleanValue ')'
// bnf:             outOfBoardAttr              ::= '(' 'outOfBoard'         booleanValue ')'
// bnf:             outOfBoardLocation          ::= '(' 'outOfBoardLocation' point        ')'
// bnf:             location                    ::= '(' 'location'           point        ')'
// bnf:             minRect                     ::= '(' 'minRect'            box          ')'
// bnf:             placementAngle              ::= '(' 'angle'              angle        ')'
// bnf:             drawRefDes                  ::= '(' 'drawRefDes'         booleanValue ')'

// bnf:             bypassCapacitorRef          ::= '(' 'bypassCapacitor'   { compList }  ')'
// bnf:                compList                    ::= '(' 'list'   { reference }  ')'
// bnf:             compGate                    ::= '(' 'gate' gateNumber [symbolId] { <gateLockInfo> | <isGenerated> | compGatePin | compGate }  ')'
// bnf:                gateNumber                  ::= integerValue
// bnf:                symbolId                    ::= '(' 'id' identifier ')'
// bnf:                gateLockInfo                ::= '(' 'lockMode' ( 'FIXED' | 'COMPONENTFIXED' | 'UNFIXED' ) ')'
// bnf:                compGatePin                 ::= '(' 'pin' funcPinName { <gatePinLockInfo> | <matchUpperGatePin> } ')'
// bnf:                   funcPinName                 ::= identifier
// bnf:                   gatePinLockInfo             ::= '(' 'lockMode' ( 'FIXED' | 'UNFIXED' ) ')'
// bnf:                   matchUpperGatePin           ::= '(' 'match' funcPinName ')'
// bnf:             compPin                     ::= '(' 'pin' pinNumber [point] { <temporaryConnectFlag> | gatePinRef | <layout> ')'
// bnf:                temporaryConnectFlag        ::= '(' 'tempConnect' booleanValue ')'
// bnf:                gatePinRef                  ::= '(' 'gate' gateNumber gatePin ')'
// bnf:                   gateNumber                  ::= integerValue
// bnf:                   gatePin                     ::= '(' 'name' funcPinName ')'
// bnf:             layout                      ::= '(' 'layout' {layoutLayer} ')'
// bnf:                layoutLayer                 ::= '(' 'layer' layerIdentifier {layoutPrim} ')'
bool CCr5kComponent::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   setUnits(tokenReader.getUnits());
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   m_parameters.registerKey(ptokReference);
   m_parameters.registerKey(ptokPinCount);
   m_parameters.registerKey(ptokGateCount);
   m_parameters.registerKey(ptokInfoTag);
   m_parameters.registerKey(ptokMsgTag);
   m_parameters.registerKey(ptokPart);
   m_parameters.registerKey(ptokStockId);
   m_parameters.registerKey(ptokPackage);
   m_parameters.registerKey(ptokFootprintSpec);
   m_parameters.registerKey(ptokOriginalReference);
   m_parameters.registerKey(ptokPlaced);
   m_parameters.registerKey(ptokPlacementSide);
   m_parameters.registerKey(ptokLocationLock);
   m_parameters.registerKey(ptokAngleLock);
   m_parameters.registerKey(ptokPlacementSideLock);
   m_parameters.registerKey(ptokJumper);
   m_parameters.registerKey(ptokFixed);
   m_parameters.registerKey(ptokIsGenerated);
   m_parameters.registerKey(ptokPackageSymbol);
   m_parameters.registerKey(ptokOutOfBoard);
   m_parameters.registerKey(ptokAngle);
   m_parameters.registerKey(ptokDrawRefDes);

   CCr5kReadElementList elementList;
   CCr5kSkippedElement minRect,bypassCapacitor,outOfBoardLocation;

   elementList.addZeroOrOne(m_footprint       ,ptokFootprint);
   elementList.addZeroOrOne(m_reverseFootprint,ptokReverseFootprint);
   elementList.addZeroOrOne(outOfBoardLocation,ptokOutOfBoardLocation);
   elementList.addZeroOrOne(m_location        ,ptokLocation);
   elementList.addZeroOrOne(minRect           ,ptokMinRect);
   elementList.addZeroOrOne(bypassCapacitor   ,ptokBypassCapacitor);
   elementList.addZeroOrOne(m_compPins        ,ptokPin);
   elementList.addZeroOrOne(m_compGates       ,ptokGate);
   elementList.addZeroOrOne(m_layout          ,ptokLayout);

   tokenReader.readElementsAndParameters(elementList,m_parameters) && retval;

   m_properties.read(tokenReader);

   //int itemCount = 0;

   //while (true)
   //{
   //   retval = tokenReader.readElementsAndParameters(elementList,m_parameters) && retval;
   //   m_properties.read(tokenReader);

   //   int newItemCount = m_properties.getCount();
   //   newItemCount += elementList.getCount();

   //   while (true)
   //   {
   //      CCr5kSkippedElement gate;

   //      if (tokenReader.readZeroOrOneElements(gate,ptokGate))
   //      {
   //         newItemCount++;
   //      }
   //      else
   //      {
   //         break;
   //      }
   //   }

   //   while (true)
   //   {
   //      CCr5kCompPin* compPin = new CCr5kCompPin();

   //      if (tokenReader.readZeroOrOneElements(*compPin,ptokPin))
   //      {
   //         newItemCount++;
   //         m_compPins.AddTail(compPin);
   //      }
   //      else
   //      {
   //         delete compPin;
   //         break;
   //      }
   //   }

   //   if (newItemCount <= itemCount)
   //   {
   //      break;
   //   }

   //   itemCount = newItemCount;
   //}

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

bool CCr5kComponent::getPlacedTop() const
{
   Cr5kTokenTag side = m_parameters.getTokenValue(ptokPlacementSide,tokA);
   bool placedTop = (side == tokA);

   return placedTop;
}

bool CCr5kComponent::getPlacedBottom() const
{
   return !getPlacedTop();
}

CString CCr5kComponent::getRefDes() const
{
   CString refDes = m_parameters.getStringValue(ptokReference,"");

   if (refDes.IsEmpty())
   {
      refDes.Format("RefDesId%04d",getId());
   }

   return refDes;
}

void CCr5kComponent::getBasesComponents(double& x,double& y,double& degrees,bool& mirror) const
{
   mirror  = getPlacedBottom();
   degrees = m_parameters.getDoubleValue(ptokAngle,0.) * (mirror ? -1. : 1.);
   x       = m_units.convertFromPortUnits(m_location.getPoint().getX());
   y       = m_units.convertFromPortUnits(m_location.getPoint().getY());
}

CBasesVector CCr5kComponent::getBasesVector() const
{
   bool   mirror  = getPlacedBottom();
   double degrees = m_parameters.getDoubleValue(ptokAngle,0.) * (mirror ? -1. : 1.);
   double x       = m_units.convertFromPortUnits(m_location.getPoint().getX());
   double y       = m_units.convertFromPortUnits(m_location.getPoint().getY());

   CBasesVector basesVector(x,y,degrees,mirror);

   return basesVector;
}

CBasesVector CCr5kComponent::getBasesVector(bool mirrorRotationFlag) const
{
   bool mirror    = getPlacedBottom();
   double degrees = m_parameters.getDoubleValue(ptokAngle,0.) * (mirrorRotationFlag ? -1. : 1.);
   double x       = m_units.convertFromPortUnits(m_location.getPoint().getX());
   double y       = m_units.convertFromPortUnits(m_location.getPoint().getY());

   CBasesVector basesVector(x,y,degrees,mirror);

   return basesVector;
}

CBasesVector CCr5kComponent::getBasesVector(bool mirrorRotationFlag,bool mirrorFlag) const
{
   double degrees = m_parameters.getDoubleValue(ptokAngle,0.) * (mirrorRotationFlag ? -1. : 1.);
   double x       = m_units.convertFromPortUnits(m_location.getPoint().getX());
   double y       = m_units.convertFromPortUnits(m_location.getPoint().getY());

   CBasesVector basesVector(x,y,degrees,mirrorFlag);

   return basesVector;
}

bool CCr5kComponent::isThruHole() const
{
   if (m_isThruHole == boolUnknown)
   {
      for (POSITION pos = m_compPins.getHeadPosition();pos != NULL;)
      {
         CCr5kCompPin* compPin = m_compPins.getNext(pos);

         if (compPin->isThruHole())
         {
            m_isThruHole = boolTrue;
            break;
         }
      }
   }

   return (m_isThruHole == boolTrue);
}

CString CCr5kComponent::getDescriptor() const
{
   CString descriptor;
   descriptor.Format("%s(%s)",getRefDes(),getFootprintName());

   return descriptor;
}

double CCr5kComponent::getComponentHeight() const
{
   if (m_componentHeight < 0.)
   {
      for (POSITION pos = m_layout.getHeadPosition();pos != NULL;)
      {
         CCr5kLayoutLayer* layoutLayer = m_layout.getNext(pos);

         for (POSITION primPos = layoutLayer->getHeadPosition();primPos != NULL;)
         {
            const CCr5kLayoutPrimitive* primitive = layoutLayer->getNext(primPos);
            
            if (primitive->getPrimitiveType() == ptokRefer)
            {
               const CCr5kReferPrimitive* referPrimitive = primitive->getReferPrimitive();
               primitive = &(referPrimitive->getLayoutPrimitive());
            }
            
            if (primitive->getPrimitiveType() == ptokArea)
            {
               double upperHeight = primitive->getUpperHeight();

               if (upperHeight > m_componentHeight)
               {
                  m_componentHeight = upperHeight;
               }
            }
         }
      }
   }

   if (m_componentHeight < 0.)
   {
      m_componentHeight = 0.;
   }

   return m_componentHeight;
}

//_____________________________________________________________________________
CCr5kComponents::CCr5kComponents()
{
}

void CCr5kComponents::empty()
{
   m_components.empty();

   CCr5kElement::empty();
}

// bnf:       components           ::= '(' 'components' {component} ')'
bool CCr5kComponents::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kComponent* component = new CCr5kComponent();

      if (tokenReader.readZeroOrOneElements(*component,ptokComponent))
      {
         m_components.AddTail(component);
      }
      else
      {
         delete component;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kComponents::linkComponents(CCr5kLinkedComponents& linkedComponents,COperationProgress* progress)
{
   for (POSITION pos = m_components.GetHeadPosition();pos != NULL;)
   {
      CCr5kComponent* component = m_components.GetNext(pos);

      linkedComponents.add(*component);

      if (progress != NULL) progress->incrementProgress();
   }
}

//_____________________________________________________________________________
CCr5kCompPinRef::CCr5kCompPinRef()
{
}

void CCr5kCompPinRef::empty()
{
   m_compReference.Empty();
   m_pinNumber.Empty();

   CCr5kElement::empty();
}

// bnf:             compPinRef           ::= '(' 'comp' reference '(' 'pin' pinNumber ')' ')'
bool CCr5kCompPinRef::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_compReference))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredToken(ptokPin))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredNonListToken(m_pinNumber))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kCompPinRefs::CCr5kCompPinRefs()
{
}

void CCr5kCompPinRefs::empty()
{
   m_compPinRefs.empty();

   CCr5kElement::empty();
}

// bnf:             compPinRef           ::= '(' 'comp' reference '(' 'pin' pinNumber ')' ')'
bool CCr5kCompPinRefs::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kCompPinRef* compPinRef = new CCr5kCompPinRef();

      if (tokenReader.readZeroOrOneElements(*compPinRef,ptokComp))
      {
         m_compPinRefs.AddTail(compPinRef);
      }
      else
      {
         delete compPinRef;
         loopFlag = false;
      }
   }

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kNet::CCr5kNet()
{
}

void CCr5kNet::empty()
{
   m_netName.Empty();
   m_parameters.empty();
   m_compPinRefs.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf:          net                  ::= '(' 'net' netName { <netType> | <netLockMode> | <netPenNumber> | <drawRatsNest> | <rebuildRatsNest> | compPinRef | property } ')'
// bnf:             netType              ::= '(' 'type' ( 'SIGNAL' | 'POWER' | 'GROUND' | 'TEMPORARY' ) ')'
// bnf:             netLockMode          ::= '(' 'lockMode' ( 'FIXED' | 'UNFIXED' ) ')'
// bnf:             netPenNumber         ::= '(' 'pen' integerValue ')'
// bnf:             drawRatsNest         ::= '(' 'drawRatsNest' booleanValue ')'
// bnf:             rebuildRatsNest      ::= '(' 'rebuildRatsNest' booleanValue ')'
// bnf:             compPinRef           ::= '(' 'comp' reference '(' 'pin' pinNumber ')' ')'
bool CCr5kNet::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_netName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokType);
   m_parameters.registerKey(ptokLockMode);
   m_parameters.registerKey(ptokPen);
   m_parameters.registerKey(ptokDrawRatsNest);
   m_parameters.registerKey(ptokRebuildRatsNest);

   int itemCount = 0;

   while (true)
   {
      m_parameters.read(tokenReader);
      m_compPinRefs.read(tokenReader);
      m_properties.read(tokenReader);

      int newItemCount = m_properties.getCount();
      newItemCount += m_compPinRefs.getCount();

      if (newItemCount <= itemCount)
      {
         break;
      }

      itemCount = newItemCount;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kNets::CCr5kNets()
{
}

void CCr5kNets::empty()
{
   m_nets.empty();

   CCr5kElement::empty();
}

// bnf:       nets                 ::= '(' 'nets' {net} ')'
bool CCr5kNets::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kNet* net = new CCr5kNet();

      if (tokenReader.readZeroOrOneElements(*net,ptokNet))
      {
         m_nets.AddTail(net);
      }
      else
      {
         delete net;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kBoardLayout::CCr5kBoardLayout()
{
}

void CCr5kBoardLayout::empty()
{
   m_layout.empty();

   CCr5kElement::empty();
}

// bnf:    boardContainer       ::= '(' 'boardContainer' [components] [componentGroups] [nets] [busses] [subBoards] { <boardLayout> | <outComponentValid> | property } ')'
// bnf:       components           ::= '(' 'components' {component} ')'
// bnf:       componentGroups      ::= '(' 'compGroups' {compGroup} ')'
// bnf:       nets                 ::= '(' 'nets' {net} ')'
// bnf:       busses               ::= '(' 'busses' {bus} ')'
// bnf:       subBoards            ::= '(' 'subBoards' {subBoard} ')'
// bnf:       boardLayout          ::= '(' 'boardLayout' layout  ')'
// bnf:       outComponentValid    ::= '(' 'outComponentValid' booleanValue  ')'
bool CCr5kBoardLayout::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = tokenReader.readOneElement(m_layout,ptokLayout);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

////_____________________________________________________________________________
//CCr5kSubBoard::CCr5kSubBoard()
//{
//}
//
//void CCr5kSubBoard::empty()
//{
//   m_layout.empty();
//
//   CCr5kElement::empty();
//}
//
// bnf:       subBoards            ::= '(' 'subBoards' {subBoard} ')'
// bnf:          subBoard             ::= '(' 'subBoard' subBoardNumber { <absDatabasePath> | <relDatabasePath> | <subBoardLastEditTime> | <pcRelation> } 
// bnf:                                        { <subBoardRegularMap> | <subBoardReverseMap> } {property}  ')'
// bnf:             subBoardNumber       ::= integerValue
// bnf:             absDatabasePath      ::= '(' 'absolutePath' string ')'
// bnf:             relDatabasePath      ::= '(' 'relativePath' string ')'
// bnf:             subBoardLastEditTime ::= '(' 'utime' timeExp ')'
// bnf:                timeExp              ::= '(' 'time' string ')'
// bnf:             pcRelation           ::= '(' 'pcRelation' ( 'CHILD' | 'PARENT' | 'CUT' | 'CUT_BACK' ) ')'
// bnf:             subBoardRegularMap   ::= '(' 'regularMap' {subBoardConnector} ')'
// bnf:             subBoardReverseMap   ::= '(' 'reverseMap' {subBoardConnector} ')'
// bnf:                subBoardConnector    ::= '(' 'connect' ownerBoardLayerRef subBoardLayerRef ')'
// bnf:                   ownerBoardLayerRef   ::= '(' 'owner' layerIdentifier ')'
// bnf:                   subBoardLayerRef     ::= '(' 'sub' layerIdentifier ')'
//_____________________________________________________________________________
CCr5kSubBoard::CCr5kSubBoard ()
{
}

void CCr5kSubBoard::empty()
{
   CCr5kElement::empty();
}

bool CCr5kSubBoard::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   int boardNumber;
   if (! tokenReader.getNextRequiredInteger(boardNumber))
   {
      tokenReader.skipList();
      return false;
   }

   if (! tokenReader.getNextRequiredToken(ptokAbsolutePath))
   {
      tokenReader.skipList();
      return false;
   }

   CString path;
   if (! tokenReader.getNextRequiredNonListToken(path))
   {
      tokenReader.skipList();
      tokenReader.skipList();
      return false;
   }
   
   CFilePath boardPath = GetAbsoluteBoardPathFromPath(path);
   BlockStruct* boardBlock = NULL;   
   FileStruct* file = NULL;
   if(false == IsPCBPresent(boardPath.getBaseFileName(), doc, &file))
   {
      if(!boardPath.getBaseFileName().IsEmpty())
      {
         CCamCadDatabase camCadDatabase(*doc);
         CFilePath ftfFilePath(boardPath),pcfFilePath(boardPath),dstFilePath(boardPath);
         ftfFilePath.setExtension("ftf");
         pcfFilePath.setExtension("pcf");
         dstFilePath.setExtension("dst");
         if (fileExists(ftfFilePath.getPath()) &&
            fileExists(pcfFilePath.getPath())    )
         {
            CZukenCr5000Reader zukenReader(camCadDatabase,boardPath.getBaseFileName());

            CString settingsFile( getApp().getImportSettingsFilePath("Cr5k.in") );
            CString settingsFileMsg;
            settingsFileMsg.Format("\nZuken CR5000 Reader: Settings file [%s].\n", settingsFile);
            getApp().LogMessage(settingsFileMsg);

            CFilePath settingsFilePath(settingsFile);

            CFilePath logFileDirectoryPath(boardPath);
            CString logFileDirectory = logFileDirectoryPath.getDirectoryPath();
            zukenReader.setLogFileDirectoryPath(logFileDirectory);

            if (fileExists(settingsFilePath.getPath()))
            {
               zukenReader.loadSettings(settingsFilePath.getPath());
            }
            else
            {
               zukenReader.getLog().writef(PrefixWarning,"Could not find .in file for reading, '%s'\n",settingsFilePath.getPath());

               settingsFilePath.setBaseFileName("Cr5000");

               if (fileExists(settingsFilePath.getPath()))
               {
                  zukenReader.loadSettings(settingsFilePath.getPath());
               }
               else
               {
                  zukenReader.getLog().writef(PrefixWarning,"Could not find .in file for reading, '%s'\n",settingsFilePath.getPath());
               }
            }
            if (zukenReader.read(ftfFilePath.getPath(),ptokFtf))
            {
               if (zukenReader.read(pcfFilePath.getPath(),ptokPcf))
               {
                  bool hasPanelFlag = false;
                  zukenReader.initializeInstantiator();

                  zukenReader.instantiateBoardData();
                  if(NULL != zukenReader.getCadFile())
                     boardBlock = zukenReader.getCadFile()->getBlock();
                  zukenReader.getCamCadDoc().generatePinLocations();

                  if (fileExists(dstFilePath.getPath()))
                  {
                     zukenReader.readDstFile(dstFilePath.getPath());
                  }

                  if (zukenReader.getOptionEnableLayerMirroring())
                  {
                     zukenReader.performLayerMirroring();
                  }              
               }
            }
         }
      }
   }
   else if(NULL != file)
      boardBlock = file->getBlock();

   if(NULL != boardBlock)
      subBoardsMap.insert(std::make_pair<const int, BlockStruct*> (boardNumber,boardBlock));

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   CCr5kReadElementList elementList;
   CCr5kSkippedElement relativePath,pcRelation,utime,regularMap,reverseMap;

   elementList.addZeroOrOne(relativePath     ,ptokRelativePath);
   elementList.addZeroOrOne(pcRelation      ,ptokPcRelation);
   elementList.addZeroOrOne(utime           ,ptokUtime);
   elementList.addZeroOrOne(regularMap      ,ptokRegularMap);
   elementList.addZeroOrOne(reverseMap    ,ptokReverseMap);


   retval = tokenReader.readList(elementList) && retval;
   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

CString CCr5kSubBoard::GetAbsoluteBoardPathFromPath(CString filePath)
{
   CString boardName = "";
   CFilePath boardFilePath(filePath);
   CString srcBoardName = boardFilePath.getBaseFileName();
   const int pathSize = 300;

   CString currentDirectory;
   GetCurrentDirectory(pathSize,currentDirectory.GetBuffer(pathSize));
   currentDirectory.ReleaseBuffer();
   CFileFind fileFind;
   for (BOOL moreFiles = fileFind.FindFile(currentDirectory + "/*.pcf");moreFiles;)
   {
      moreFiles = fileFind.FindNextFile();
      CString tmpStr = fileFind.GetFileName();
      if(!srcBoardName.CompareNoCase(tmpStr.Left(srcBoardName.GetLength())))
      {
         boardName = fileFind.GetFilePath();
         break;
      }
   }
   fileFind.Close();
   return boardName;
}
//_____________________________________________________________________________
CCr5kSubBoards::CCr5kSubBoards()
{
}

void CCr5kSubBoards::empty()
{
   m_subBoards.empty();
   CCr5kElement::empty();
}

bool CCr5kSubBoards::read(CCr5kTokenReader& tokenReader)
{
   subBoardsMap.clear();
   m_subBoards.empty();
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kSubBoard * subBoard = new CCr5kSubBoard();

      if (tokenReader.readZeroOrOneElements(*subBoard,ptokSubBoard))
      {
         m_subBoards.AddTail(subBoard);
      }
      else
      {
         delete subBoard;
         loopFlag = false;
      }
   }
   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
   setLoadedFlag();
   return retval;
}


//_____________________________________________________________________________
CCr5kBoardContainer::CCr5kBoardContainer()
{
}

void CCr5kBoardContainer::empty()
{
   m_components.empty();
   m_nets.empty();
   m_boardLayout.empty();
   m_subBoards.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf:    boardContainer       ::= '(' 'boardContainer' [components] [componentGroups] [nets] [busses] [subBoards] { <boardLayout> | <outComponentValid> | property } ')'
// bnf:       components           ::= '(' 'components' {component} ')'
// bnf:       componentGroups      ::= '(' 'compGroups' {compGroup} ')'
// bnf:       nets                 ::= '(' 'nets' {net} ')'
// bnf:       busses               ::= '(' 'busses' {bus} ')'
// bnf:       subBoards            ::= '(' 'subBoards' {subBoard} ')'
// bnf:       boardLayout          ::= '(' 'boardLayout' layout  ')'
// bnf:       outComponentValid    ::= '(' 'outComponentValid' booleanValue  ')'
bool CCr5kBoardContainer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   CCr5kReadElementList elementList;
   CCr5kSkippedElement compGroups,busses,subBoards;

   elementList.addZeroOrOne(m_components      ,ptokComponents);
   elementList.addZeroOrOne(compGroups        ,ptokCompGroups);
   elementList.addZeroOrOne(m_nets            ,ptokNets);
   elementList.addZeroOrOne(busses            ,ptokBusses);
   

   retval = tokenReader.readList(elementList) && retval;

   CCr5kReadElementList elementList2;
   CCr5kSkippedElement outComponentValid;
   elementList2.addZeroOrOne(m_subBoards         ,ptokSubBoards);
   elementList2.addZeroOrOne(m_boardLayout     ,ptokBoardLayout);
   elementList2.addZeroOrOne(outComponentValid ,ptokOutComponentValid);
   
   retval = tokenReader.readElementsAndProperties(elementList2,m_properties);

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

void CCr5kBoardContainer::linkComponents(CCr5kLinkedComponents& linkedComponents,COperationProgress* progress)
{
   m_components.linkComponents(linkedComponents,progress);
}

////_____________________________________________________________________________
//CCr5kBoardLayerRef::CCr5kBoardLayerRef()
//{
//}
//
//void CCr5kBoardLayerRef::empty()
//{
//   m_layerIdentifier.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
//// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
//// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
//// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
//// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
//// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
//bool CCr5kBoardLayerRef::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   retval = tokenReader.readOneElement(m_layerIdentifier) && retval;
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
//CCr5kBoardLayerRefs::CCr5kBoardLayerRefs()
//{
//}
//
//void CCr5kBoardLayerRefs::empty()
//{
//   m_boardLayerRefs.empty();
//
//   CCr5kElement::empty();
//}

// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
//bool CCr5kBoardLayerRefs::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kLayerIdentifier* boardLayerRef = new CCr5kLayerIdentifier();
//
//      if (tokenReader.readZeroOrOneElements(*boardLayerRef))
//      {
//         m_boardLayerRefs.AddTail(boardLayerRef);
//      }
//      else
//      {
//         delete boardLayerRef;
//         loopFlag = false;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kLayerCorrespondence::CCr5kLayerCorrespondence()
{
}

void CCr5kLayerCorrespondence::empty()
{
   m_parameters.empty();
   m_boardLayerRefs.empty();

   CCr5kElement::empty();
}

// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
bool CCr5kLayerCorrespondence::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   m_parameters.registerKey(ptokFootLayer);

   retval = m_parameters.read(tokenReader) && retval;

   retval = tokenReader.readOneElement(m_boardLayerRefs,ptokBoardLayer) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
bool CCr5kLayerCorrespondences::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kLayerCorrespondence* correspondence = new CCr5kLayerCorrespondence();

      if (tokenReader.readZeroOrOneElements(*correspondence,ptokCorrespondence))
      {
         m_elementList.AddTail(correspondence);
      }
      else
      {
         delete correspondence;
         loopFlag = false;
      }
   }

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kLayerMap::CCr5kLayerMap()
{
}

void CCr5kLayerMap::empty()
{
   m_correspondences.empty();

   CCr5kElement::empty();
}

// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
bool CCr5kLayerMap::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListDefinedToken(m_typeToken))
   {
      tokenReader.skipList();
      return false;
   }

   retval = m_correspondences.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kLayerMaps::CCr5kLayerMaps()
{
}

void CCr5kLayerMaps::empty()
{
   m_layerMaps.empty();

   CCr5kElement::empty();
}

// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
bool CCr5kLayerMaps::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kLayerMap* layerMap = new CCr5kLayerMap();

      if (tokenReader.readZeroOrOneElements(*layerMap,ptokMap))
      {
         m_layerMaps.AddTail(layerMap);
      }
      else
      {
         delete layerMap;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

CCr5kLayerMap* CCr5kLayerMaps::getAt(Cr5kTokenTag mapType) const
{
   CCr5kLayerMap* layerMap = NULL;

   for (POSITION pos = m_layerMaps.GetHeadPosition();pos != NULL;)
   {
      layerMap = m_layerMaps.GetNext(pos);

      if (layerMap->getType() == mapType)
      {
         break;
      }

      layerMap = NULL;
   }

   return layerMap;
}

//_____________________________________________________________________________
CCr5kNonConductiveLayerRelation::CCr5kNonConductiveLayerRelation()
{
}

void CCr5kNonConductiveLayerRelation::empty()
{
   m_layerName.Empty();
   m_parameters.empty();

   CCr5kElement::empty();
}

// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
bool CCr5kNonConductiveLayerRelation::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokType);
   m_parameters.registerKey(ptokUserDefType);

   retval = m_parameters.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kNonConductiveLayerRelations::CCr5kNonConductiveLayerRelations()
{
}

//void CCr5kNonConductiveLayerRelations::empty()
//{
//   m_nonConductiveLayerRelations.empty();
//
//   CCr5kElement::empty();
//}

// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
bool CCr5kNonConductiveLayerRelations::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kNonConductiveLayerRelation* nonConductiveLayerRelation = new CCr5kNonConductiveLayerRelation();

      if (tokenReader.readZeroOrOneElements(*nonConductiveLayerRelation,ptokRefer))
      {
         m_elementList.AddTail(nonConductiveLayerRelation);
      }
      else
      {
         delete nonConductiveLayerRelation;
         loopFlag = false;
      }
   }

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
CCr5kConductiveLayer::CCr5kConductiveLayer()
{
}

void CCr5kConductiveLayer::empty()
{
   m_properties.empty();
   m_nonConductiveLayerRelations.empty();
   m_parameters.empty();

   CCr5kElement::empty();
}

// bnf:          condLayers             ::= '(' 'conductiveLayer' {condLayer} ')' 
// bnf:             condLayer              ::= '(' 'layerNumber' integerValue { <condLayerType> | <signalName> | <solderingMethod> | nonCondLayerRelation | property } ')' 
// bnf:                condLayerType          ::= '(' 'type' ( 'POSI' | 'POSINEGA' | 'FULLSURF' ) ')'
// bnf:                signalName             ::= '(' 'signalName' string ')'
// bnf:                solderingMethod        ::= '(' 'soldering' ( 'FLOW' | 'REFLOW' | 'REFLOW_2' | 'NONE' ) ')'
// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
bool CCr5kConductiveLayer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
   tokenReader.setSourceLineNumber(*this);

   bool retval = true;

   if (! tokenReader.getNextRequiredInteger(m_layerNumber))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokType);
   m_parameters.registerKey(ptokSignalName);
   m_parameters.registerKey(ptokSoldering);

   int itemCount = 0;

   while (true)
   {
      m_parameters.read(tokenReader);
      m_nonConductiveLayerRelations.read(tokenReader);
      m_properties.read(tokenReader);

      int newItemCount = m_properties.getCount();
      newItemCount += m_nonConductiveLayerRelations.getCount();

      if (newItemCount <= itemCount)
      {
         break;
      }

      itemCount = newItemCount;
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
//CCr5kConductiveLayers::CCr5kConductiveLayers()
//{
//}
//
//void CCr5kConductiveLayers::empty()
//{
//   m_conductiveLayers.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf:          condLayers             ::= '(' 'conductiveLayer' {condLayer} ')' 
//// bnf:             condLayer              ::= '(' 'layerNumber' integerValue { <condLayerType> | <signalName> | <solderingMethod> | nonCondLayerRelation | property } ')' 
//// bnf:                condLayerType          ::= '(' 'type' ( 'POSI' | 'POSINEGA' | 'FULLSURF' ) ')'
//// bnf:                signalName             ::= '(' 'signalName' string ')'
//// bnf:                solderingMethod        ::= '(' 'soldering' ( 'FLOW' | 'REFLOW' | 'REFLOW_2' | 'NONE' ) ')'
//// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
//// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
//// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
//// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
//bool CCr5kConductiveLayers::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kConductiveLayer* conductiveLayer = new CCr5kConductiveLayer();
//
//      if (tokenReader.readZeroOrOneElements(*conductiveLayer,ptokLayerNumber))
//      {
//         m_conductiveLayers.AddTail(conductiveLayer);
//      }
//      else
//      {
//         delete conductiveLayer;
//         loopFlag = false;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kNonConductiveLayer::CCr5kNonConductiveLayer()
{
}

void CCr5kNonConductiveLayer::empty()
{
   m_layerName.Empty();
   m_parameters.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf:          nonCondLayers          ::= '(' 'nonConductiveLayer' [layerType] {nonCondLayer} ')'
// bnf:             layerType              ::= '(' 'type' string ')' >>> added
// bnf:             nonCondLayer           ::= '(' 'layer' layerName {property} ')' 
bool CCr5kNonConductiveLayer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokType);

   m_parameters.read(tokenReader);

   retval = m_properties.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
//CCr5kNonConductiveLayers::CCr5kNonConductiveLayers()
//{
//}
//
//void CCr5kNonConductiveLayers::empty()
//{
//   m_nonConductiveLayers.empty();
//
//   CCr5kElement::empty();
//}
//
// bnf:          nonCondLayers          ::= '(' 'nonConductiveLayer' {nonCondLayer} ')' 
// bnf:             nonCondLayer           ::= '(' 'layer' layerName {property} ')' 
//bool CCr5kNonConductiveLayers::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kNonConductiveLayer* nonConductiveLayer = new CCr5kNonConductiveLayer();
//
//      if (tokenReader.readZeroOrOneElements(*nonConductiveLayer,ptokLayer))
//      {
//         m_nonConductiveLayers.AddTail(nonConductiveLayer);
//      }
//      else
//      {
//         delete nonConductiveLayer;
//         loopFlag = false;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kFootprintLayer::CCr5kFootprintLayer()
{
}

void CCr5kFootprintLayer::empty()
{
   m_layerName.Empty();
   m_parameters.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf:          footprintLayers        ::= '(' 'footprintLayer' {footprintLayer} ')' 
// bnf:             footprintLayer         ::= '(' 'layer' layerName { <footprintLayerType> | property } ')' 
// bnf:                footprintLayerType     ::= '(' 'type' ( 'CONDUCTIVE' | 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'HOLE' | 'PROHIBIT' | 'UNDEFINED' ) ')' 
bool CCr5kFootprintLayer::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_layerName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokType);

   retval = m_parameters.read(tokenReader) && retval;

   retval = m_properties.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
//CCr5kFootprintLayers::CCr5kFootprintLayers()
//{
//}
//
//void CCr5kFootprintLayers::empty()
//{
//   m_footprintLayers.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf:          footprintLayers        ::= '(' 'footprintLayer' {footprintLayer} ')' 
//// bnf:             footprintLayer         ::= '(' 'layer' layerName { <footprintLayerType> | property } ')' 
//// bnf:                footprintLayerType     ::= '(' 'type' ( 'CONDUCTIVE' | 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'HOLE' | 'PROHIBIT' | 'UNDEFINED' ) ')' 
//bool CCr5kFootprintLayers::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kFootprintLayer* footprintLayer = new CCr5kFootprintLayer();
//
//      if (tokenReader.readZeroOrOneElements(*footprintLayer,ptokLayer))
//      {
//         m_footprintLayers.AddTail(footprintLayer);
//      }
//      else
//      {
//         delete footprintLayer;
//         loopFlag = false;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kTechnology::CCr5kTechnology()
{
}

void CCr5kTechnology::empty()
{
   m_technologyName.Empty();
   m_parameters.empty();
   m_footprintLayers.empty();
   m_nonConductiveLayers.empty();
   m_conductiveLayers.empty();
   m_layerMaps.empty();
   m_properties.empty();

   CCr5kElement::empty();
}

// bnf:    technologyContainer    ::= '(' 'technologyContainer' {technology} ')'
// bnf:       technology             ::= '(' 'technology' techName [numberOfConductorLayer] [padstackGroupNameRef] [footprintLayers] 
// bnf:                                                            [nonCondLayers] [condLayers] [subLayers] [layerMaps] {property} ')'
// bnf:          numberOfConductorLayer ::= '(' 'numberOfConductorLayer' integerValue ')' 
// bnf:          padstackGroupNameRef   ::= '(' 'padstackGroup' padstackGroupName ')' 
// bnf:             padstackGroupName      ::= identifier
// bnf:          footprintLayers        ::= '(' 'footprintLayer' {footprintLayer} ')' 
// bnf:             footprintLayer         ::= '(' 'layer' layerName { <footprintLayerType> | property } ')' 
// bnf:                footprintLayerType     ::= '(' 'type' ( 'CONDUCTIVE' | 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'HOLE' | 'PROHIBIT' | 'UNDEFINED' ) ')' 
// bnf:          nonCondLayers          ::= '(' 'nonConductiveLayer' {nonCondLayer} ')' 
// bnf:             nonCondLayer           ::= '(' 'layer' layerName {property} ')' 
// bnf:          condLayers             ::= '(' 'conductiveLayer' {condLayer} ')' 
// bnf:             condLayer              ::= '(' 'layerNumber' integerValue { <condLayerType> | <signalName> | <solderingMethod> | nonCondLayerRelation | property } ')' 
// bnf:                condLayerType          ::= '(' 'type' ( 'POSI' | 'POSINEGA' | 'FULLSURF' ) ')'
// bnf:                signalName             ::= '(' 'signalName' string ')'
// bnf:                solderingMethod        ::= '(' 'soldering' ( 'FLOW' | 'REFLOW' | 'REFLOW_2' | 'NONE' ) ')'
// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
// bnf:          subLayers              ::= '(' 'subLayer' ... ')' 
// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
bool CCr5kTechnology::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   if (! tokenReader.getNextRequiredNonListToken(m_technologyName))
   {
      tokenReader.skipList();
      return false;
   }

   m_parameters.registerKey(ptokNumberOfConductorLayer);
   m_parameters.registerKey(ptokPadstackGroup);

   retval = m_parameters.read(tokenReader) && retval;

   CCr5kReadElementList elementList;
   CCr5kSkippedElement subLayers;

   elementList.addZeroOrOne(m_footprintLayers    ,ptokFootprintLayer);
   elementList.addZeroOrOne(m_nonConductiveLayers,ptokNonConductiveLayer);
   elementList.addZeroOrOne(m_conductiveLayers   ,ptokConductiveLayer);
   elementList.addZeroOrOne(subLayers            ,ptokSubLayer);
   elementList.addZeroOrOne(m_layerMaps          ,ptokLayerMapping);

   retval = tokenReader.readList(elementList) && retval;

   retval = m_properties.read(tokenReader) && retval;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

//_____________________________________________________________________________
//CCr5kTechnologyContainer::CCr5kTechnologyContainer()
//{
//}
//
//void CCr5kTechnologyContainer::empty()
//{
//   m_technologies.empty();
//
//   CCr5kElement::empty();
//}
//
//// bnf:    technologyContainer    ::= '(' 'technologyContainer' {technology} ')'
//// bnf:       technology             ::= '(' 'technology' techName [numberOfConductorLayer] [padstackGroupNameRef] [footprintLayers] 
//// bnf:                                                            [nonCondLayers] [condLayers] [subLayers] [layerMaps] {property} ')'
//bool CCr5kTechnologyContainer::read(CCr5kTokenReader& tokenReader)
//{
//   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);
//
//   bool retval = true;
//
//   for (bool loopFlag = true;loopFlag;)
//   {
//      CCr5kTechnology* technology = new CCr5kTechnology();
//
//      if (tokenReader.readZeroOrOneElements(*technology,ptokTechnology))
//      {
//         m_technologies.AddTail(technology);
//      }
//      else
//      {
//         delete technology;
//         loopFlag = false;
//      }
//   }
//
//   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;
//
//   setLoadedFlag();
//
//   return retval;
//}

//_____________________________________________________________________________
CCr5kLayerDirectoryEntry::CCr5kLayerDirectoryEntry(LayerStruct& layer,const CString& baseLayerName,const CString& materialName,int materialNumber)
: m_layer(layer)
, m_baseLayerName(baseLayerName)
, m_materialName(materialName)
, m_materialNumber(materialNumber)
{
}

//_____________________________________________________________________________
CCr5kLayerDirectoryEntryList::CCr5kLayerDirectoryEntryList()
: m_list(false)
{
}

//_____________________________________________________________________________
CCr5kRegularExpression::CCr5kRegularExpression()
{
   setMetaSubstitute('=');
   setMetaEscape('/');
   setEscapeRequiredForGrouping(false);
}

CCr5kRegularExpression::CCr5kRegularExpression(const char* regularExpression,bool anchorEntire)
{
   setMetaSubstitute('=');
   setMetaEscape('/');
   setEscapeRequiredForGrouping(false);

   setRegularExpression(regularExpression,anchorEntire);
}

CCr5kRegularExpression::CCr5kRegularExpression(const char* regularExpression,const char* substituteExpression,bool anchorEntire)
{
   setMetaSubstitute('=');
   setMetaEscape('/');
   setEscapeRequiredForGrouping(false);

   setRegularExpression(regularExpression,anchorEntire);
   setSubstituteExpression(substituteExpression);
}

//_____________________________________________________________________________
void CCr5kLayerDirectory::addLayer(LayerStruct& layer,const CString& baseLayerName,const CString& materialName,int materialNumber)
{
   CCr5kLayerDirectoryEntry* layerDirectoryEntry = NULL;

   int layerIndex = layer.getLayerIndex();

   if (layerIndex < m_layerArray.GetSize())
   {
      layerDirectoryEntry = m_layerArray.GetAt(layerIndex);
   }

   if (layerDirectoryEntry == NULL)
   {
      layerDirectoryEntry = new CCr5kLayerDirectoryEntry(layer,baseLayerName,materialName,materialNumber);
      m_layerArray.SetAtGrow(layerIndex,layerDirectoryEntry);
   }
}

void CCr5kLayerDirectory::getBaseLayerNameList(CCr5kLayerDirectoryEntryList& entryList,const CString& baseLayerName)
{
   for (int layerIndex = 0;layerIndex < m_layerArray.GetSize();layerIndex++)
   {
      CCr5kLayerDirectoryEntry* layerDirectoryEntry = m_layerArray.GetAt(layerIndex);

      if (layerDirectoryEntry != NULL)
      {
         if (layerDirectoryEntry->getBaseLayerName().CompareNoCase(baseLayerName) == 0)
         {
            entryList.addReference(layerDirectoryEntry);
         }
      }
   }
}

void CCr5kLayerDirectory::getMatchingBaseLayerNameList(CCr5kLayerDirectoryEntryList& entryList,const CString& baseLayerName)
{
   CCr5kRegularExpression baseLayerNameExpression(baseLayerName,true);

   for (int layerIndex = 0;layerIndex < m_layerArray.GetSize();layerIndex++)
   {
      CCr5kLayerDirectoryEntry* layerDirectoryEntry = m_layerArray.GetAt(layerIndex);

      if (layerDirectoryEntry != NULL)
      {
         CString entryBaseLayerName = layerDirectoryEntry->getBaseLayerName();

         if (baseLayerNameExpression.matches(entryBaseLayerName))
         {
            entryList.addReference(layerDirectoryEntry);
         }
      }
   }
}

void CCr5kLayerDirectory::getMatchingLayerNameList(CCr5kLayerDirectoryEntryList& entryList,const CString& layerName)
{
   CCr5kRegularExpression layerNameExpression(layerName,true);

   for (int layerIndex = 0;layerIndex < m_layerArray.GetSize();layerIndex++)
   {
      CCr5kLayerDirectoryEntry* layerDirectoryEntry = m_layerArray.GetAt(layerIndex);

      if (layerDirectoryEntry != NULL)
      {
         CString entryLayerName = layerDirectoryEntry->getLayerName();

         if (layerNameExpression.matches(entryLayerName))
         {
            entryList.addReference(layerDirectoryEntry);
         }
      }
   }
}

//_____________________________________________________________________________
CCr5kLayerLink::CCr5kLayerLink(const CCr5kFootprintLayer& footprintLayer)
: m_layerName(footprintLayer.getName())
, m_footprintLayer(&footprintLayer)
, m_conductiveLayer(NULL)
, m_nonConductiveLayer(NULL)
, m_ccLayer(NULL)
, m_layerType(tokUndefined)
, m_topFlag(false)
, m_bottomFlag(false)
{
}

CCr5kLayerLink::CCr5kLayerLink(const CCr5kConductiveLayer& conductiveLayer)
: m_layerName(conductiveLayer.getName())
, m_footprintLayer(NULL)
, m_conductiveLayer(&conductiveLayer)
, m_nonConductiveLayer(NULL)
, m_ccLayer(NULL)
, m_layerType(tokUndefined)
, m_topFlag(false)
, m_bottomFlag(false)
{
}

CCr5kLayerLink::CCr5kLayerLink(const CCr5kNonConductiveLayer& nonConductiveLayer)
: m_layerName(nonConductiveLayer.getName())
, m_footprintLayer(NULL)
, m_conductiveLayer(NULL)
, m_nonConductiveLayer(&nonConductiveLayer)
, m_ccLayer(NULL)
, m_layerType(tokUndefined)
, m_topFlag(false)
, m_bottomFlag(false)
{
}

bool CCr5kLayerLink::isReferencedBy(int layerIndex) const
{
   bool retval = m_referenceMask.contains(layerIndex);

   return retval;
}

void CCr5kLayerLink::addReference(int layerIndex)
{
   m_referenceMask.add(layerIndex);
}

void CCr5kLayerLink::dump(CWriteFormat& writeFormat) const
{
   CString sourceString;

   if (m_footprintLayer != NULL)
   {
      sourceString += " footprintLayer";
   }

   if (m_conductiveLayer != NULL)
   {
      sourceString += " conductiveLayer";
   }

   if (m_nonConductiveLayer != NULL)
   {
      sourceString += " nonConductiveLayer";
   }

   writeFormat.writef("name='%s' this=0x%08x type='%s' source='%s' contents='%s'\n",
      m_layerName,
      this,
      CCr5kTokenReader::cr5kTokenToString(m_layerType),
      sourceString,
      m_referenceMask.getContentsString());
}

//_____________________________________________________________________________
CCr5kLayerLinkMap::CCr5kLayerLinkMap(bool isContainer)
: m_map(50,isContainer)
{
}

//CCr5kLayerLink* CCr5kLayerLinkMap::addLayer(const CString& layerName)
//{
//   CCr5kLayerLink* layerLink = new CCr5kLayerLink(layerName);
//   m_map.add(layerName,layerLink);
//
//   return layerLink;
//}
//
//CCr5kLayerLink* CCr5kLayerLinkMap::addLayer(int layerNumber)
//{
//   CCr5kLayerLink* layerLink = NULL;
//
//   if (layerNumber >= 0)
//   {
//      CString layerName;
//      layerName.Format("%d",layerNumber);
//
//      layerLink = new CCr5kLayerLink(layerName);
//      m_map.setAt(layerNumber,layerName,layerLink);
//   }
//
//   return layerLink;
//}

CCr5kLayerLink* CCr5kLayerLinkMap::addLayer(CCr5kLayerLink* layerLink)
{
   CString layerName = layerLink->getLayerName();
   layerName.MakeLower();

   if (! m_map.isContainer())
   {
      m_map.add(layerName,layerLink);
   }

   return layerLink;
}

CCr5kLayerLink* CCr5kLayerLinkMap::addLayer(const CCr5kFootprintLayer& footprintLayer)
{
   CString layerName = footprintLayer.getName();
   layerName.MakeLower();

   CCr5kLayerLink* layerLink = NULL;

   if (m_map.isContainer())
   {
      layerLink = new CCr5kLayerLink(footprintLayer);
      m_map.add(layerName,layerLink);
   }

   return layerLink;
}

CCr5kLayerLink* CCr5kLayerLinkMap::addLayer(const CCr5kConductiveLayer& conductiveLayer)
{
   int layerNumber = conductiveLayer.getLayerNumber();
   CString layerName = conductiveLayer.getName();

   CCr5kLayerLink* layerLink = NULL;

   if (m_map.isContainer())
   {
      layerLink = new CCr5kLayerLink(conductiveLayer);
      m_map.setAt(layerNumber,layerName,layerLink);
   }

   return layerLink;
}

CCr5kLayerLink* CCr5kLayerLinkMap::addLayer(const CCr5kNonConductiveLayer& nonConductiveLayer)
{
   CString layerName = nonConductiveLayer.getName();
   layerName.MakeLower();

   CCr5kLayerLink* layerLink = NULL;

   if (m_map.isContainer())
   {
      layerLink = new CCr5kLayerLink(nonConductiveLayer);
      m_map.add(layerName,layerLink);
   }

   return layerLink;
}

CCr5kLayerLink* CCr5kLayerLinkMap::lookup(const CString& layerName) const
{
   CString key = layerName;
   key.MakeLower();

   CCr5kLayerLink* layerLink = m_map.getAt(key);

   return layerLink;
}

CCr5kLayerLink* CCr5kLayerLinkMap::getAt(int layerNumber) const
{
   CCr5kLayerLink* layerLink = m_map.getAt(layerNumber);

   return layerLink;
}

bool CCr5kLayerLinkMap::containsLayer(const CString& layerName) const
{
   bool retval = (lookup(layerName) != NULL);

   return retval;
}

void CCr5kLayerLinkMap::dump(CWriteFormat& writeFormat) const
{
   for (int index = 0;index < m_map.getSize();index++)
   {
      CCr5kLayerLink* layerLink = m_map.getAt(index);

      if (layerLink != NULL)
      {
         writeFormat.writef("%d: ",index);
         layerLink->dump(writeFormat);
      }
   }
}

//_____________________________________________________________________________
CCr5kLayerLinkMaps::CCr5kLayerLinkMaps()
: m_layerMap(false)
, m_conductiveLayerMap(true)
, m_nonConductiveLayerMap(true)
, m_footPrintLayerMap(true)
{
}

CCr5kLayerDirectory& CCr5kLayerLinkMaps::getLayerDirectory()
{
   return m_layerDirectory;
}

void CCr5kLayerLinkMaps::addLayer(const CCr5kFootprintLayer& footprintLayer)
{
   CCr5kLayerLink* layerLink = m_footPrintLayerMap.addLayer(footprintLayer);

   m_layerMap.addLayer(layerLink);
}

void CCr5kLayerLinkMaps::addLayer(const CCr5kConductiveLayer& conductiveLayer)
{
   CCr5kLayerLink* layerLink = m_conductiveLayerMap.addLayer(conductiveLayer);

   m_layerMap.addLayer(layerLink);
}

void CCr5kLayerLinkMaps::addLayer(const CCr5kNonConductiveLayer& nonConductiveLayer)
{
   CCr5kLayerLink* layerLink = m_nonConductiveLayerMap.addLayer(nonConductiveLayer);

   m_layerMap.addLayer(layerLink);
}

int CCr5kLayerLinkMaps::getMaximumConductiveLayerNumber() const
{
   int maximumConductiveLayerNumber = m_conductiveLayerMap.getSize() - 1;

   return maximumConductiveLayerNumber;
}

void CCr5kLayerLinkMaps::calculateNonConductiveLayerTypes()
{
   int maxLayerNumber = getMaximumConductiveLayerNumber();

   for (int layerNumber = 1;layerNumber <= maxLayerNumber;layerNumber++)
   {
      CCr5kLayerLink* layerLink = m_conductiveLayerMap.getAt(layerNumber);

      if (layerLink != NULL)
      {
         const CCr5kConductiveLayer* conductiveLayer = layerLink->getConductiveLayer();

         if (conductiveLayer != NULL)
         {
            layerLink->addReference(layerNumber);  // self reference

            const CCr5kNonConductiveLayerRelations& nonConductiveLayerRelations = conductiveLayer->getNonConductiveLayerRelations();

            for (POSITION pos = nonConductiveLayerRelations.getHeadPosition();pos != NULL;)
            {
               CCr5kNonConductiveLayerRelation* nonConductiveLayerRelation = nonConductiveLayerRelations.getNext(pos);

               CString layerName = nonConductiveLayerRelation->getLayerName();
               Cr5kTokenTag type = nonConductiveLayerRelation->getType();

               CCr5kLayerLink* nonConductiveLayerLink = m_nonConductiveLayerMap.lookup(layerName);

               if (nonConductiveLayerLink != NULL)
               {
                  nonConductiveLayerLink->setLayerType(type);
                  nonConductiveLayerLink->setTop(layerNumber == 1);
                  nonConductiveLayerLink->setBottom(layerNumber == maxLayerNumber);
                  nonConductiveLayerLink->addReference(layerNumber);
               }
            }
         }
      }
   }
}

LayerStruct& CCr5kLayerLinkMaps::getDefinedLayer(CZukenCr5000Reader& zukenReader,const CString& layerName,int materialNumber,bool mirroredLayerFlag)
{
   CCr5kLayerLink* layerLink = m_layerMap.lookup(layerName);
   CString materialLayerName(layerName);
   CString materialName;

   if (materialNumber >= 0)
   {
      materialName = zukenReader.getMaterials().getMaterialName(materialNumber);
      materialLayerName = zukenReader.getMaterialLayerName(layerName,materialName);
   }

   LayerStruct* ccLayer = zukenReader.getCamCadDatabase().getLayer(materialLayerName);

   if (ccLayer == NULL)
   {
      if (layerLink == NULL)
      {  
         LayerTypeTag ccLayerType = layerTypeUnknown;
         
         // system layer names will not show up in the layer links
         //getApp().getMessageFilterTypeMessage().formatMessageBoxApp("Could not find layer link for layerName '%s'",layerName);

         if (layerName.CompareNoCase("Board_Figure") == 0)
         {
            if (zukenReader.getPanelDataFlag())
            {
               materialLayerName = "Panel_Figure";
               ccLayerType = layerTypePanelOutline;
            }
            else
            {
               ccLayerType = layerTypeBoardOutline;
            }
         }
      
         if (zukenReader.getOptionAdjustLayerNames())
         {
            materialLayerName = zukenReader.adjustCase(materialLayerName);
         }

         ccLayer = zukenReader.getCamCadDatabase().getDefinedLayer(materialLayerName,false,ccLayerType);

         m_layerDirectory.addLayer(*ccLayer,layerName,materialName,materialNumber);
      }
      else
      {
         int ccElectricalStackNumber = -1;
         LayerTypeTag ccLayerType = layerTypeUndefined;

         const CCr5kConductiveLayer*    conductiveLayer    = layerLink->getConductiveLayer();
         const CCr5kNonConductiveLayer* nonConductiveLayer = layerLink->getNonConductiveLayer();
         const CCr5kFootprintLayer*     footprintLayer     = layerLink->getFootprintLayer();
         const CCr5kProperties*         properties         = NULL;

         if (conductiveLayer != NULL)
         {
            int layerNumber    = conductiveLayer->getLayerNumber();
            int maxLayerNumber = m_conductiveLayerMap.getSize() - 1;
            properties = &(conductiveLayer->getProperties());

            ccElectricalStackNumber = layerNumber;

            if (layerNumber == 1)
            {
               ccLayerType = layerTypeSignalTop;
            }
            else if (layerNumber == maxLayerNumber)
            {
               ccLayerType = layerTypeSignalBottom;
            }
            else
            {
               ccLayerType = layerTypeSignalInner;
            }
         }
         else if (nonConductiveLayer != NULL)
         {
            Cr5kTokenTag layerType = layerLink->getLayerType();
            properties = &(nonConductiveLayer->getProperties());

            switch (layerType)
            {
            case tokSolderResist:
               ccLayerType = layerTypeMaskAll;

               if      (layerLink->isTop()   ) ccLayerType = layerTypeMaskTop;   
               else if (layerLink->isBottom()) ccLayerType = layerTypeMaskBottom;

               break;
            case tokMetalMask:
               ccLayerType = layerTypeStencilTop;

               if (layerLink->isBottom()) ccLayerType = layerTypeStencilBottom;

               break;
            case tokCompArea:
               ccLayerType = layerTypeComponentOutline;

               break;
            case tokSymbolMark:
               ccLayerType = layerTypeSilkTop;

               if (layerLink->isBottom()) ccLayerType = layerTypeSilkBottom;

               break;
            case tokHeightLimit:       
            case tokMounter:           
            case tokWire_Prohibit:     
            case tokPlacement_Prohibit:
            case tokVia_Prohibit:      
            case tokThermal_Shape:     
            case tokOnlyWire_Prohibit: 
            case tokSubConductive:     
            case tokUndefined:         
            default:
               ccLayerType = layerTypeUndefined;
               break;
            }
         }

         if (zukenReader.getOptionAdjustLayerNames())
         {
            materialLayerName = zukenReader.adjustCase(materialLayerName);
         }

         ccLayer = zukenReader.getCamCadDatabase().getDefinedLayer(materialLayerName);

         m_layerDirectory.addLayer(*ccLayer,layerName,materialName,materialNumber);

         if (materialNumber < 0)
         {
            layerLink->setCcLayer(ccLayer);
         }

         if (ccElectricalStackNumber >= 0)
         {
            ccLayer->setElectricalStackNumber(ccElectricalStackNumber);
         }

         if (ccLayerType != layerTypeUndefined)
         {
            ccLayer->setLayerType(ccLayerType);
         }

         if (properties != NULL)
         {
            zukenReader.setProperties(ccLayer->attributes(),*properties);
         }
      }
   }

   if (zukenReader.getOptionGenerateMirroredLayers() && mirroredLayerFlag && !ccLayer->isFloating())
   {
      LayerStruct* mirroredLayer;

      if (!ccLayer->hasMirroredLayer())
      {
         CString mirroredLayerName = ccLayer->getName() + "[mirror]";

         mirroredLayer = &(getDefinedLayer(zukenReader,mirroredLayerName,-1,false));

         mirroredLayer->mirrorWithLayer(*ccLayer);
         mirroredLayer->setLayerType(getOppositeSideLayerType(ccLayer->getLayerType()));
      }
      else
      {
         mirroredLayer = zukenReader.getCamCadDatabase().getLayerAt(ccLayer->getMirroredLayerIndex());
      }

      ccLayer = mirroredLayer;
   }

    return *ccLayer;
}

bool CCr5kLayerLinkMaps::isLayerInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const
{
   bool retval = false;

   CCr5kLayerLink* layerLink = m_layerMap.lookup(layerName);

   if (layerLink != NULL)
   {
      for (int layerIndex = fromLayerIndex;layerIndex <= toLayerIndex && !retval;layerIndex++)
      {
         retval = layerLink->isReferencedBy(layerIndex);
      }
   }

   return retval;
}

bool CCr5kLayerLinkMaps::isConductiveLayer(const CString& layerName) const
{
   bool retval = false;

   CCr5kLayerLink* layerLink = m_layerMap.lookup(layerName);

   if (layerLink != NULL)
   {
      retval = (layerLink->getConductiveLayer() != NULL);
   }

   return retval;
}

bool CCr5kLayerLinkMaps::isConductiveLayerAssociate(const CString& layerName) const
{
   bool retval = false;

   CCr5kLayerLink* layerLink = m_layerMap.lookup(layerName);
   int maximumConductiveLayerNumber = getMaximumConductiveLayerNumber();

   if (layerLink != NULL)
   {
      for (int layerIndex = 1;layerIndex <= maximumConductiveLayerNumber && !retval;layerIndex++)
      {
         retval = layerLink->isReferencedBy(layerIndex);
      }
   }

   return retval;
}

void CCr5kLayerLinkMaps::dump(CWriteFormat& writeFormat) const
{
   writeFormat.writef("\n\n" "m_layerMap\n");

   m_layerMap.dump(writeFormat);

   writeFormat.writef("\n\n" "m_conductiveLayerMap\n");

   m_conductiveLayerMap.dump(writeFormat);

   writeFormat.writef("\n\n" "m_nonConductiveLayerMap\n");

   m_nonConductiveLayerMap.dump(writeFormat);

   writeFormat.writef("\n\n" "m_footPrintLayerMap\n");

   m_footPrintLayerMap.dump(writeFormat);
}

//_____________________________________________________________________________
CCr5kBoardFile::CCr5kBoardFile()
{
}

void CCr5kBoardFile::empty()
{
   m_header.empty();
   m_technologyContainer.empty();
   m_boardContainer.empty();

   CCr5kElement::empty();
}

// bnf: $pcf                 ::= '(' 'pcf' pcfHeader technologyContainer boardContainer [parameterContainer] [libraryEntities] ')' >>> added technologyContainer
// bnf:    pcfHeader            ::= '(' 'header' pcfVersion { <unitDescriptor> | <timeZone> } ')'
// bnf:       timeZone             ::= '(' 'timeZone' string ')'
// bnf:    boardContainer       ::= '(' 'boardContainer' [components] [componentGroups] [nets] [busses] [subBoards] { <boardLayout> | <outComponentValid> | property } ')'
// bnf:       components           ::= '(' 'components' {component} ')'
// bnf:          component            ::= '(' 'component' referenceDesignator [compPinCount] [compGateCount] 
//                                            { <infoTag> | <msgTag> | <partNameRef> | <stockIdRef> | <packageNameRef> | <footprintSpec> | <footprintNameRef> | 
//                                              <BsideFootprintNameRef> | <originalReferenceDesignator> | <placed> | <placementSide> | <locationLock> | <angleLock> | 
//                                              <placementSideLock> | <jumperAttr> | <bypassCapacitorAttr> | <fixedAttr> | <isGenerated> | <packageSymbolAttr> | 
//                                              <outOfBoardAttr> | <outOfBoardLocation> | <location> | <minRect> | <placementAngle> | <drawRefDes> | 
//                                              compGate | compPin | 
//                                              <bypassCapacitorRef> | <layout> | property } ')'

// bnf:             referenceDesignator         ::= '(' 'reference'          reference    ')'
// bnf:             compPinCount                ::= '(' 'pinCount'           integerValue ')'
// bnf:             compGateCount               ::= '(' 'gateCount'          integerValue ')'
// bnf:             infoTag                     ::= '(' 'infoTag'            integerValue ')'
// bnf:             msgTag                      ::= '(' 'msgTag'             integerValue ')'
// bnf:             partNameRef                 ::= '(' 'part'               identifier   ')'
// bnf:             stockIdRef                  ::= '(' 'stockId'            identifier   ')'
// bnf:             packageNameRef              ::= '(' 'package'            identifier   ')'
// bnf:             footprintSpec               ::= '(' 'footprintSpec'      identifier   ')'
// bnf:             footprintNameRef            ::= '(' 'footprint'          identifier   [footprintTechnology] ')'
// bnf:                footprintTechnology         ::= '(' 'technology'        identifier   ')'
// bnf:             BsideFootprintNameRef       ::= '(' 'reverseFootprint'   identifier   [footprintTechnology] ')'
// bnf:             originalReferenceDesignator ::= '(' 'originalReference'  identifier   ')'
// bnf:             placed                      ::= '(' 'placed'             booleanValue ')'
// bnf:             placementSide               ::= '(' 'placementSide'     ( 'A' | 'B' ) ')'
// bnf:             locationLock                ::= '(' 'locationLock'       booleanValue ')'
// bnf:             angleLock                   ::= '(' 'angleLock'          booleanValue ')'
// bnf:             placementSideLock           ::= '(' 'placementSideLock'  booleanValue ')'
// bnf:             jumperAttr                  ::= '(' 'jumper'             booleanValue ')'
// bnf:             fixedAttr                   ::= '(' 'fixed'              booleanValue ')'
// bnf:             isGenerated                 ::= '(' 'isGenerated'        booleanValue ')'
// bnf:             packageSymbolAttr           ::= '(' 'packageSymbol'      booleanValue ')'
// bnf:             outOfBoardAttr              ::= '(' 'outOfBoard'         booleanValue ')'
// bnf:             outOfBoardLocation          ::= '(' 'outOfBoardLocation' point        ')'
// bnf:             location                    ::= '(' 'location'           point        ')'
// bnf:             minRect                     ::= '(' 'minRect'            box          ')'
// bnf:             placementAngle              ::= '(' 'angle'              angle        ')'
// bnf:             drawRefDes                  ::= '(' 'drawRefDes'         booleanValue ')'

// bnf:             bypassCapacitorRef          ::= '(' 'bypassCapacitor'   { compList }  ')'
// bnf:                compList                    ::= '(' 'list'   { reference }  ')'
// bnf:             compGate                    ::= '(' 'gate' gateNumber [symbolId] { <gateLockInfo> | <isGenerated> | compGatePin | compGate }  ')'
// bnf:                gateNumber                  ::= integerValue
// bnf:                symbolId                    ::= '(' 'id' identifier ')'
// bnf:                gateLockInfo                ::= '(' 'lockMode' ( 'FIXED' | 'COMPONENTFIXED' | 'UNFIXED' ) ')'
// bnf:                compGatePin                 ::= '(' 'pin' funcPinName { <gatePinLockInfo> | <matchUpperGatePin> } ')'
// bnf:                   funcPinName                 ::= identifier
// bnf:                   gatePinLockInfo             ::= '(' 'lockMode' ( 'FIXED' | 'UNFIXED' ) ')'
// bnf:                   matchUpperGatePin           ::= '(' 'match' funcPinName ')'
// bnf:             compPin                     ::= '(' 'pin' pinNumber [point] { <temporaryConnectFlag> | gatePinRef | <layout> ')'
// bnf:                temporaryConnectFlag        ::= '(' 'tempConnect' booleanValue ')'
// bnf:                gatePinRef                  ::= '(' 'gate' gateNumber gatePin ')'
// bnf:                   gateNumber                  ::= integerValue
// bnf:                   gatePin                     ::= '(' 'name' funcPinName ')'
// bnf:       componentGroups      ::= '(' 'compGroups' {compGroup} ')'
// bnf:       nets                 ::= '(' 'nets' {net} ')'
// bnf:          net                  ::= '(' 'net' netName { <netType> | <netLockMode> | <netPenNumber> | <drawRatsNest> | <rebuildRatsNest> | compPinRef property } ')'
// bnf:             netType              ::= '(' 'type' ( 'SIGNAL' | 'POWER' | 'GROUND' | 'TEMPORARY' ) ')'
// bnf:             netLockMode          ::= '(' 'lockMode' ( 'FIXED' | 'UNFIXED' ) ')'
// bnf:             netPenNumber         ::= '(' 'pen' integerValue ')'
// bnf:             drawRatsNest         ::= '(' 'drawRatsNest' booleanValue ')'
// bnf:             rebuildRatsNest      ::= '(' 'rebuildRatsNest' booleanValue ')'
// bnf:             compPinRef           ::= '(' 'comp' reference '(' 'pin' pinNumber ')' ')'
// bnf:       busses               ::= '(' 'busses' {bus} ')'
// bnf:          bus                  ::= '(' 'bus' {... ')'
// bnf:       subBoards            ::= '(' 'subBoards' {subBoard} ')'
// bnf:          subBoard             ::= '(' 'subBoard' subBoardNumber { <absDatabasePath> | <relDatabasePath> | <subBoardLastEditTime> | <pcRelation> } 
// bnf:                                        { <subBoardRegularMap> | <subBoardReverseMap> } {property}  ')'
// bnf:             subBoardNumber       ::= integerValue
// bnf:             absDatabasePath      ::= '(' 'absolutePath' string ')'
// bnf:             relDatabasePath      ::= '(' 'relativePath' string ')'
// bnf:             subBoardLastEditTime ::= '(' 'utime' timeExp ')'
// bnf:                timeExp              ::= '(' 'time' string ')'
// bnf:             pcRelation           ::= '(' 'pcRelation' ( 'CHILD' | 'PARENT' | 'CUT' | 'CUT_BACK' ) ')'
// bnf:             subBoardRegularMap   ::= '(' 'regularMap' {subBoardConnector} ')'
// bnf:             subBoardReverseMap   ::= '(' 'reverseMap' {subBoardConnector} ')'
// bnf:                subBoardConnector    ::= '(' 'connect' ownerBoardLayerRef subBoardLayerRef ')'
// bnf:                   ownerBoardLayerRef   ::= '(' 'owner' layerIdentifier ')'
// bnf:                   subBoardLayerRef     ::= '(' 'sub' layerIdentifier ')'
// bnf:       boardLayout          ::= '(' 'boardLayout' layout  ')'
// bnf:       outComponentValid    ::= '(' 'outComponentValid' booleanValue  ')'

// bnf:    technologyContainer    ::= '(' 'technologyContainer' {technology} ')'
// bnf:       technology             ::= '(' 'technology' techName [numberOfConductorLayer] [padstackGroupNameRef] [footprintLayers] 
// bnf:                                                            [nonCondLayers] [condLayers] [subLayers] [layerMaps] {property} ')'
// bnf:          numberOfConductorLayer ::= '(' 'numberOfConductorLayer' integerValue ')' 
// bnf:          padstackGroupNameRef   ::= '(' 'padstackGroup' padstackGroupName ')' 
// bnf:             padstackGroupName      ::= identifier
// bnf:          footprintLayers        ::= '(' 'footprintLayer' {footprintLayer} ')' 
// bnf:             footprintLayer         ::= '(' 'layer' layerName { <footprintLayerType> | property } ')' 
// bnf:                footprintLayerType     ::= '(' 'type' ( 'CONDUCTIVE' | 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'HOLE' | 'PROHIBIT' | 'UNDEFINED' ) ')' 
// bnf:          nonCondLayers          ::= '(' 'nonConductiveLayer' {nonCondLayer} ')' 
// bnf:             nonCondLayer           ::= '(' 'layer' layerName {property} ')' 
// bnf:          condLayers             ::= '(' 'conductiveLayer' {condLayer} ')' 
// bnf:             condLayer              ::= '(' 'layerNumber' integerValue { <condLayerType> | <signalName> | <solderingMethod> | nonCondLayerRelation | property } ')' 
// bnf:                condLayerType          ::= '(' 'type' ( 'POSI' | 'POSINEGA' | 'FULLSURF' ) ')'
// bnf:                signalName             ::= '(' 'signalName' string ')'
// bnf:                solderingMethod        ::= '(' 'soldering' ( 'FLOW' | 'REFLOW' | 'REFLOW_2' | 'NONE' ) ')'
// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName nonCondLayerRelationType | nonCondLayerRelationUserDefType ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
// bnf:          subLayers              ::= '(' 'subLayer' ... ')' 
// bnf:          layerMaps              ::= '(' 'layerMapping' {layerMap} ')' 
// bnf:             layerMap               ::= '(' 'map' ( 'A' | 'B' | 'A_THRU' | 'B_THRU' ) {layerMap1} ')' 
// bnf:                layerMap1              ::= '(' 'correspondence' footprintLayerRef boardLayerRefs ')' 
// bnf:                   footprintLayerRef      ::= '(' 'footLayer' layerName ')' 
// bnf:                   boardLayerRefs         ::= '(' 'boardLayer' {boardLayerRef} ')' 
// bnf:                      boardLayerRef          ::= condLayerRef | nonCondLayerRef | systemLayerRef 
bool CCr5kBoardFile::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   retval = tokenReader.readOneElement(m_header        ,ptokHeader) && retval;

   retval = tokenReader.readOneElement(m_technologyContainer,ptokTechnologyContainer) && retval;

   retval = tokenReader.readOneElement(m_boardContainer,ptokBoardContainer) && retval;

   tokenReader.skipList();

   setLoadedFlag();

   return retval;
}

void CCr5kBoardFile::linkComponents(CCr5kLinkedComponents& linkedComponents,COperationProgress* progress)
{
   m_boardContainer.linkComponents(linkedComponents,progress);
}

void CCr5kBoardFile::generateLayerLinkMaps(CCr5kLayerLinkMaps& layerLinkMaps)
{
   const CCr5kTechnology* technology = m_technologyContainer.getTechnology();

   if (technology != NULL)
   {
      const CCr5kFootprintLayers&     footprintLayers     = technology->getFootPrintLayers();
      const CCr5kNonConductiveLayers& nonConductiveLayers = technology->getNonConductiveLayers();
      const CCr5kConductiveLayers&    conductiveLayers    = technology->getConductiveLayers();

      for (POSITION pos = footprintLayers.getHeadPosition();pos != NULL;)
      {
         const CCr5kFootprintLayer* footprintLayer = footprintLayers.getNext(pos);

         layerLinkMaps.addLayer(*footprintLayer);
      }

      for (POSITION pos = nonConductiveLayers.getHeadPosition();pos != NULL;)
      {
         const CCr5kNonConductiveLayer* nonConductiveLayer = nonConductiveLayers.getNext(pos);

         layerLinkMaps.addLayer(*nonConductiveLayer);
      }

      for (POSITION pos = conductiveLayers.getHeadPosition();pos != NULL;)
      {
         const CCr5kConductiveLayer* conductiveLayer = conductiveLayers.getNext(pos);

         layerLinkMaps.addLayer(*conductiveLayer);
      }

      layerLinkMaps.calculateNonConductiveLayerTypes();
   }
}

//_____________________________________________________________________________
CCr5kPanelFile::CCr5kPanelFile()
{
}

void CCr5kPanelFile::empty()
{
   CCr5kElement::empty();
}

// bnf: 
bool CCr5kPanelFile::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = tokenReader.skipList();  // todo

   return retval;
}

////_____________________________________________________________________________
//CCr5kLinkedPad::CCr5kLinkedPad(CCr5kPad& pad)
//: m_pad(pad)
//, m_geometry(NULL)
//{
//}
//
////_____________________________________________________________________________
//CCr5kLinkedPads::CCr5kLinkedPads()
//{
//}
//
//void CCr5kLinkedPads::empty()
//{
//   m_pads.empty();
//}
//
//void CCr5kLinkedPads::add(CCr5kPad& pad)
//{
//   CCr5kLinkedPad* linkedPad = new CCr5kLinkedPad(pad);
//
//   m_pads.setAt(pad.getName(),linkedPad);
//}
//
////_____________________________________________________________________________
//CCr5kLinkedPadstack::CCr5kLinkedPadstack(CCr5kPadstack& padstack)
//: m_padstack(padstack)
//, m_geometry(NULL)
//{
//}
//
////_____________________________________________________________________________
//CCr5kLinkedPadstacks::CCr5kLinkedPadstacks()
//{
//}
//
//void CCr5kLinkedPadstacks::empty()
//{
//   m_padstacks.empty();
//}
//
//void CCr5kLinkedPadstacks::add(CCr5kPadstack& padstack)
//{
//   CCr5kLinkedPadstack* linkedPadstack = new CCr5kLinkedPadstack(padstack);
//
//   m_padstacks.setAt(padstack.getName(),linkedPadstack);
//}
//
////_____________________________________________________________________________
//CCr5kLinkedFootprint::CCr5kLinkedFootprint(CCr5kFootprint& footprint)
//: m_footprint(footprint)
//, m_geometry(NULL)
//{
//}
//
////_____________________________________________________________________________
//CCr5kLinkedFootprints::CCr5kLinkedFootprints()
//{
//}
//
//void CCr5kLinkedFootprints::empty()
//{
//   m_footprints.empty();
//}
//
//void CCr5kLinkedFootprints::add(CCr5kFootprint& footprint)
//{
//   CCr5kLinkedFootprint* linkedFootprint = new CCr5kLinkedFootprint(footprint);
//
//   m_footprints.setAt(footprint.getName(),linkedFootprint);
//}
//
////_____________________________________________________________________________
//CCr5kLinkedComponent::CCr5kLinkedComponent(CCr5kComponent& component)
//: m_component(component)
//, m_componentData(NULL)
//{
//}
//
////_____________________________________________________________________________
//CCr5kLinkedComponents::CCr5kLinkedComponents()
//{
//}
//
//void CCr5kLinkedComponents::empty()
//{
//   m_components.empty();
//}
//
//void CCr5kLinkedComponents::add(CCr5kComponent& component)
//{
//   CCr5kLinkedComponent* linkedComponent = new CCr5kLinkedComponent(component);
//
//   m_components.AddTail(linkedComponent);
//}

//_____________________________________________________________________________
CCr5kMaterials::CCr5kMaterials()
{
   m_materialsMap.InitHashTable(nextPrime2n(50));
}

void CCr5kMaterials::setAt(int materialNumber,const CString& materialName)
{
   m_materialsMap.SetAt(getKey(materialNumber),materialName);
}

CString CCr5kMaterials::getMaterialName(int materialNumber) const
{
   CString materialName;

   if (! m_materialsMap.Lookup(getKey(materialNumber),materialName))
   {
      materialName.Format("Material(%d)",materialNumber);
   }

   return materialName;
}

CString CCr5kMaterials::getKey(int materialNumber) const
{
   CString key;

   key.Format("%d",materialNumber);

   return key;
}

//_____________________________________________________________________________
CZukenCr5000LayerMapCorrespondence::CZukenCr5000LayerMapCorrespondence(const CString& footLayerName)
: m_footLayerName(footLayerName)
{
}

void CZukenCr5000LayerMapCorrespondence::addBoardLayer(const CString& boardLayerName)
{
   m_boardLayers.AddTail(boardLayerName);
}

//_____________________________________________________________________________
CZukenCr5000LayerMap::CZukenCr5000LayerMap()
{
}

void CZukenCr5000LayerMap::empty()
{
   m_correspondences.empty();
}

void CZukenCr5000LayerMap::set(const CCr5kLayerMap& layerMap)
{
   empty();

   const CCr5kLayerCorrespondences& correspondences = layerMap.getLayerCorrespondences();

   for (POSITION pos = correspondences.getHeadPosition();pos != NULL;)
   {
      CCr5kLayerCorrespondence* correspondence = correspondences.getNext(pos);
      CString footLayerName = correspondence->getFootLayerName();
      const CCr5kBoardLayerRefs& boardLayerRefs = correspondence->getBoardLayerRefs();

      CZukenCr5000LayerMapCorrespondence* ccCorrespondence;

      if (! m_correspondences.Lookup(footLayerName,ccCorrespondence))
      {
         ccCorrespondence = new CZukenCr5000LayerMapCorrespondence(footLayerName);
         m_correspondences.SetAt(footLayerName,ccCorrespondence);
      }

      for (POSITION boardRefPos = boardLayerRefs.getHeadPosition();boardRefPos != NULL;)
      {
         CCr5kLayerIdentifier* layerIdentifier = boardLayerRefs.getNext(boardRefPos);

         CString boardLayerName = layerIdentifier->getName();

         ccCorrespondence->addBoardLayer(boardLayerName);
      }
   }
}

const CStringList& CZukenCr5000LayerMap::getMappedBoardLayers(const CString& footLayerName) const
{
   const CStringList* boardLayers = &m_emptyList;
   CZukenCr5000LayerMapCorrespondence* layerMapCorrespondence;

   if (m_correspondences.Lookup(footLayerName,layerMapCorrespondence))
   {
      boardLayers = &(layerMapCorrespondence->getBoardLayers());
   }

   return *boardLayers;
}

//_____________________________________________________________________________
/*
   implications:

   -  if footlayerX is mapped to boardlayer1 in map A and
         footlayerX is mapped to boardlayer2 in map B
      then boardlayer1 and boardlayer2 may be mirrored



*/
CZukenCr5000LayerMapping::CZukenCr5000LayerMapping()
{
}

CZukenCr5000LayerMapping::~CZukenCr5000LayerMapping()
{
   empty();
}

void CZukenCr5000LayerMapping::empty()
{
   m_mapA.empty();
   m_mapB.empty();
   m_mapAThru.empty();
   m_mapBThru.empty();
}

const CZukenCr5000LayerMap& CZukenCr5000LayerMapping::getLayerMap(bool thruFlag,bool topFlag) const
{
   const CZukenCr5000LayerMap& layerMap =
      (thruFlag ?
         (topFlag ? m_mapAThru : m_mapBThru) :
         (topFlag ? m_mapA     : m_mapB    )    );

   return layerMap;
}

const CStringList& CZukenCr5000LayerMapping::getMappedBoardLayers(bool thruFlag,bool topFlag,const CString& footLayerName) const
{
   const CZukenCr5000LayerMap& layerMap = getLayerMap(thruFlag,topFlag);
   const CStringList& boardLayers = layerMap.getMappedBoardLayers(footLayerName);

   return boardLayers;
}

void CZukenCr5000LayerMapping::set(const CCr5kTechnologyContainer& technologyContainer,CWriteFormat& writeFormat)
{
   empty();

   if (technologyContainer.getCount() > 0)
   {
      CCr5kTechnology* technology = technologyContainer.getHead();

      const CCr5kLayerMaps& layerMaps = technology->getLayerMaps();

      CCr5kLayerMap* layerMap;
      
      layerMap = layerMaps.getAt(tokA);

      if (layerMap != NULL)
      {
         m_mapA.set(*layerMap);
      }
      
      layerMap = layerMaps.getAt(tokB);

      if (layerMap != NULL)
      {
         m_mapB.set(*layerMap);
      }
      
      layerMap = layerMaps.getAt(tokA_Thru);

      if (layerMap != NULL)
      {
         m_mapAThru.set(*layerMap);
      }
      
      layerMap = layerMaps.getAt(tokB_Thru);

      if (layerMap != NULL)
      {
         m_mapBThru.set(*layerMap);
      }

#ifdef AnalyzeLayerNames
      // for analysis
      CMapSortStringToOb layerNameMap(300,false);

      const CCr5kFootprintLayers&     footprintLayers     = technology->getFootPrintLayers();
      const CCr5kNonConductiveLayers& nonConductiveLayers = technology->getNonConductiveLayers();
      const CCr5kConductiveLayers&    conductiveLayers    = technology->getConductiveLayers();

      for (POSITION pos = footprintLayers.getHeadPosition();pos != NULL;)
      {
         CCr5kFootprintLayer* layer = footprintLayers.getNext(pos);

         CString layerName = layer->getName();
         layerNameMap.SetAt(layerName,NULL);
      }

      for (POSITION pos = nonConductiveLayers.getHeadPosition();pos != NULL;)
      {
         CCr5kNonConductiveLayer* layer = nonConductiveLayers.getNext(pos);

         CString layerName = layer->getName();
         layerNameMap.SetAt(layerName,NULL);
      }

      for (POSITION pos = conductiveLayers.getHeadPosition();pos != NULL;)
      {
         CCr5kConductiveLayer* layer = conductiveLayers.getNext(pos);

         CString layerName = layer->getName();
         layerNameMap.SetAt(layerName,NULL);
      }

      int index1,index2;
      CObject *object1,*object2;
      CString layerName1,layerName2;

      COperationProgress operationProgress;
      operationProgress.updateStatus("Analyzing layer names.",layerNameMap.GetCount());

      for (layerNameMap.rewind(index1);layerNameMap.next(object1,layerName1,index1);)
      {
         operationProgress.incrementProgress();

         CString line;
         line.Format("%-15.15s - ",layerName1);

         for (layerNameMap.rewind(index2);layerNameMap.next(object2,layerName2,index2);)
         {
            int nameEditDistance = editDistance(layerName1,layerName2);

            if (nameEditDistance < 4)
            {
               line.AppendFormat("%s(%d) ",layerName2,nameEditDistance);
            }
         }

         line += "\n";

         writeFormat.write(line);
      }
#endif

   }
}

//_____________________________________________________________________________
CPadstackBuilder::CPadstackBuilder(CZukenCr5000Reader& zukenReader/*,int fromLayerIndex,int toLayerIndex*/)
: m_zukenReader(zukenReader)
//, m_fromLayerIndex(fromLayerIndex)
//, m_toLayerIndex(toLayerIndex)
, m_dataList(true)
{
}

CCamCadData& CPadstackBuilder::getCamCadData()
{
   return m_zukenReader.getCamCadData();
}

void CPadstackBuilder::takeData(CDataList& dataList,int fromLayerIndex,int toLayerIndex)
{
   CDataList* flattenedDataList = dataList.getFlattenedDataList(m_zukenReader.getCamCadData_Doc(),-1,false);
   filterDataList(*flattenedDataList,fromLayerIndex,toLayerIndex);

   m_dataList.takeData(*flattenedDataList);

   dataList.empty();
}

const CDataList& CPadstackBuilder::getDataList() const
{
   return m_dataList;
}

void CPadstackBuilder::copyData(const CDataList& dataList)
{
   CDataList dataListCopy(dataList,getCamCadData());

   m_dataList.takeData(dataListCopy);
}

void CPadstackBuilder::filterDataList(CDataList& dataList,int fromLayerIndex,int toLayerIndex) const
{
   if (fromLayerIndex > 0 && toLayerIndex > 0)
   {
      for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
      {
         POSITION oldPos = pos;
         DataStruct* data = dataList.GetNext(pos);
         bool keepDataFlag = false;

         LayerStruct* layer = m_zukenReader.getCamCadDatabase().getLayerAt(data->getLayerIndex());

         if (layer != NULL)
         {
            CString layerName = layer->getName();

            int fPos = layerName.Find("[Mirror]");

            if (fPos >= 0)
            {
               layerName = layerName.Left(fPos);
            }

            fPos = layerName.ReverseFind(':');

            if (fPos >= 0)
            {
               layerName = layerName.Left(fPos);
            }

            //keepDataFlag = m_zukenReader.isLayerConductiveOrInConductiveRange(layerName,fromLayerIndex,toLayerIndex);
            keepDataFlag = m_zukenReader.isLayerNonConductiveOrInConductiveRange(layerName,fromLayerIndex,toLayerIndex);
         }

         if (!keepDataFlag)
         {
            //dataList.RemoveDataFromList(&(m_zukenReader.getCamCadDoc()),data,oldPos);
            m_zukenReader.getCamCadDoc().removeDataFromDataList(dataList,data,oldPos);
         }
      }
   }
}

void CPadstackBuilder::buildPadstack(CDataList& dataList,const CString& padstackName,int fileNumber,const CTMatrix& compToPinMatrix)
{
   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("\n" "Entering %s  padstackName = '%s' ++++++++++++++++++++++++++++++++++++++\n",__FUNCTION__,padstackName);
      CDebugWriteFormat::getWriteFormat().writef("m_dataList Contents at entry (component coordinates)\n");
      dumpDataList(CDebugWriteFormat::getWriteFormat(),m_zukenReader.getCamCadDatabase(),m_dataList);
   }

   //CDataList* flattenedDataList = m_dataList.getFlattenedDataList(m_zukenReader.getCamCadDoc(),-1);

   //filterDataList(*flattenedDataList);

   //flattenedDataList->transform(compToPinMatrix);

   m_dataList.transform(compToPinMatrix);

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("m_dataList Contents at entry (pin coordinates)\n");
      dumpDataList(CDebugWriteFormat::getWriteFormat(),m_zukenReader.getCamCadDatabase(),m_dataList);
   }

   if (m_dataList.GetCount() == 1)
   {
      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("Copy one entity from m_dataList to dataList\n");
      }

      dataList.AddTail(new_DataStruct(*(m_dataList.GetHead())));
   }
   else
   {
      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("restructuring padstack geometry into dataList\n");
      }

      m_zukenReader.getCamCadDatabase().restructurePadstackGeometry(dataList,padstackName,m_dataList,fileNumber,false);
   }

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("\n" "dataList Contents at exit\n");
      dumpDataList(CDebugWriteFormat::getWriteFormat(),m_zukenReader.getCamCadDatabase(),dataList);
      CDebugWriteFormat::getWriteFormat().writef("\n" "Exiting %s  ++++++++++++++++++++++++++++++++++++++\n",__FUNCTION__);
   }
}

//_____________________________________________________________________________
CNamedDataList::CNamedDataList(const CString& name,const CBasesVector& basesVector,int fromLayerNumber,int toLayerNumber,bool isContainer)
: m_name(name)
, m_basesVector(basesVector)
, m_fromLayerNumber(fromLayerNumber)
, m_toLayerNumber(toLayerNumber)
, m_dataList(isContainer)
{
}

//_____________________________________________________________________________
void CNamedDataListMap::empty()
{
   m_dataListMap.empty();
}

CNamedDataList& CNamedDataListMap::add(const CString& name,const CBasesVector& basesVector)
{
   return add(name,basesVector,0,0);
}

CNamedDataList& CNamedDataListMap::add(CString name,const CBasesVector& basesVector,int fromLayerNumber,int toLayerNumber)
{
   if (fromLayerNumber > 0 && toLayerNumber > 0)
   {
      name.AppendFormat("[%d:%d]",fromLayerNumber,toLayerNumber);
   }

   CString uniqueName(name);
   CNamedDataList* namedDataList;

   for (int index=0;m_dataListMap.Lookup(uniqueName,namedDataList);index++)
   {
      uniqueName.Format("%s-%d",name,index);
   }

   namedDataList = new CNamedDataList(uniqueName,basesVector,fromLayerNumber,toLayerNumber);

   m_dataListMap.SetAt(uniqueName,namedDataList);

   return *namedDataList;
}

void CNamedDataListMap::rewind(int& index)
{
   m_dataListMap.rewind(index);
}

bool CNamedDataListMap::next(CNamedDataList*& namedDataList,CString& key,int& index)
{
   return m_dataListMap.next(namedDataList,key,index);
}

//_____________________________________________________________________________
CCr5kMappedLayerDataList::CCr5kMappedLayerDataList(CZukenCr5000Reader& zukenReader,CDataList& managedDataList)
: m_zukenReader(zukenReader)
, m_managedDataList(managedDataList)
, m_layerMapping(NULL)
, m_bufferDataList(true)
, m_dataList(&managedDataList)
, m_thruFlag(false)
, m_topFlag(true)
, m_padstackCachedModeFlag(false)
, m_cacheStackCount(0)
{
}

CCr5kMappedLayerDataList::CCr5kMappedLayerDataList(CZukenCr5000Reader& zukenReader,CDataList& managedDataList,const CZukenCr5000LayerMapping& layerMapping,bool enableLayerMapping)
: m_zukenReader(zukenReader)
, m_managedDataList(managedDataList)
, m_layerMapping(&layerMapping)
, m_bufferDataList(true)
, m_dataList(&managedDataList)
, m_thruFlag(false)
, m_topFlag(true)
, m_padstackCachedModeFlag(false)
, m_cacheStackCount(0)
{
   m_enableLayerMappingStack.push(enableLayerMapping);
}

CCr5kMappedLayerDataList::~CCr5kMappedLayerDataList()
{
   while (m_dataListStack.GetCount() > 0)
   {
      CDataList* dataList = m_dataListStack.GetAt(0);
      m_dataListStack.RemoveAt(0);

      m_managedDataList.takeData(*dataList);

      delete dataList;
   }
}

CCamCadData& CCr5kMappedLayerDataList::getCamCadData()
{
   return m_zukenReader.getCamCadData();
}

CCamCadDatabase& CCr5kMappedLayerDataList::getCamCadDatabase()
{
   return m_zukenReader.getCamCadDatabase();
}

bool CCr5kMappedLayerDataList::isLayerMappingEnabled() const
{
   bool retval = false;
   
   if (m_layerMapping != NULL)
   {
      if (! m_enableLayerMappingStack.isEmpty())
      {
         retval = m_enableLayerMappingStack.getTop();
      }
   }

   return retval;
}

void CCr5kMappedLayerDataList::pushEnableLayerMapping(bool enableFlag)
{
   m_enableLayerMappingStack.push(enableFlag);
}

bool CCr5kMappedLayerDataList::popEnableLayerMapping()
{
   bool retval = m_enableLayerMappingStack.pop();

   return retval;
}

void CCr5kMappedLayerDataList::addNetName(const CString& netName)
{
   int netNameKeywordIndex = m_zukenReader.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeNetName);

   for (POSITION pos = m_dataList->GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_dataList->GetNext(pos);

      m_zukenReader.getCamCadDatabase().addAttribute(data->attributes(),netNameKeywordIndex,netName);
      data->setGraphicClass(graphicClassEtch);
   }
}

void CCr5kMappedLayerDataList::addRefName(const CString& refName)
{
   int refNameKeywordIndex = m_zukenReader.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeRefName);

   for (POSITION pos = m_dataList->GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_dataList->GetNext(pos);

      m_zukenReader.getCamCadDatabase().addAttribute(data->attributes(),refNameKeywordIndex,refName);
   }
}

void CCr5kMappedLayerDataList::unfillPolys()
{
   for (CDataListIterator polyIterator(*m_dataList,dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* data = polyIterator.getNext();
      CPolyList& polyList = *(data->getPolyList());

      for (POSITION pos = polyList.GetHeadPosition();pos != NULL;)
      {
         CPoly* poly = polyList.GetNext(pos);

         poly->setFilled(false);
      }
   }
}

void CCr5kMappedLayerDataList::transform(const CTMatrix& matrix)
{
   m_dataList->transform(matrix);
}

void CCr5kMappedLayerDataList::pushBuffer()
{
   CDataList* dataList = new CDataList(true);

   m_dataListStack.Add(dataList);

   m_dataList = dataList;
}

void CCr5kMappedLayerDataList::popBuffer()
{
   int count = m_dataListStack.GetCount();

   if (count > 0)
   {
      if (m_cacheStackCount < 1)
      {
         // we are not in padstack cache mode, pop the top datalist off the stack and move the data from this list to the managed data list.
         // assign m_dataList to the new top of stack or to the managed data list if the stack is empty.
         CDataList* dataList = m_dataListStack.GetAt(count - 1);
         m_dataListStack.RemoveAt(count - 1);
         count = m_dataListStack.GetCount();

         if (count > 0)
         {
            m_dataList = m_dataListStack.GetAt(count - 1);
         }
         else
         {
            m_dataList = &m_managedDataList;
         }

         m_dataList->takeData(*dataList);

         delete dataList;
      }
      else
      {
         // we are in padstack cache mode, pop the padstack data list off the dataListStack and 
         // assign m_dataList to the new top of stack or to the managed data list if the stack is empty.
         m_dataListStack.RemoveAt(count - 1);
         count = m_dataListStack.GetCount();

         if (count > 0)
         {
            m_dataList = m_dataListStack.GetAt(count - 1);
         }
         else
         {
            m_dataList = &m_managedDataList;
         }

         m_cacheStackCount--;
      }
   }
}

void CCr5kMappedLayerDataList::pushPadBuffer(const CString& padName,const CBasesVector& basesVector)
{
   // bases vector is from padstack in component coordinate system

   if (getPadstackCacheModeFlag())
   {
      CString basesVectorDescriptor = basesVector.getDescriptor();

      bool atOriginFlag = basesVector.getOrigin().fpeq(m_pinBasesVector.getOrigin());

      CString dataListName = CString("pad1") + (atOriginFlag ? "_0_" : "_1_") + padName;
      dataListName.AppendFormat(":%d",m_dataListStack.GetCount());

      CNamedDataList& namedDataList = m_padstackCacheMap.add(dataListName,basesVector);

      m_dataList = &(namedDataList.getDataList());
      m_dataListStack.Add(m_dataList);
      m_cacheStackCount++;
   }
   else
   {
      pushBuffer();
   }
}

void CCr5kMappedLayerDataList::pushPadstackBuffer(const CString& padstackName,const CBasesVector& basesVector,int fromLayerNumber,int toLayerNumber)
{  
   // bases vector is from padstack in component coordinate system

   if (getPadstackCacheModeFlag())
   {
      //m_fromLayerNumber = fromLayerNumber;
      //m_toLayerNumber   = toLayerNumber;
      m_zukenReader.normalizeLayerNumberSpan(fromLayerNumber,toLayerNumber);

      CString basesVectorDescriptor = basesVector.getDescriptor();

      bool atOriginFlag = basesVector.getOrigin().fpeq(m_pinBasesVector.getOrigin());

      CString dataListName = CString("pad1") + (atOriginFlag ? "_0_" : "_1_") + padstackName;
      dataListName.AppendFormat(":%d",m_dataListStack.GetCount());

      CNamedDataList& namedDataList = m_padstackCacheMap.add(dataListName,basesVector,fromLayerNumber,toLayerNumber);

      m_dataList = &(namedDataList.getDataList());
      m_dataListStack.Add(m_dataList);
      m_cacheStackCount++;
   }
   else
   {
      pushBuffer();
   }
}

void CCr5kMappedLayerDataList::startPadstackCache(const CBasesVector& pinBasesVector,bool enableFlag)
{
   m_pinBasesVector = pinBasesVector;

   setPadstackCacheModeFlag(enableFlag);
}

BlockStruct* CCr5kMappedLayerDataList::getConglomeratedPadstack(CBasesVector& pinBasesVector,const CTMatrix& pinToCompMatrix,const CTMatrix& compToBoardMatrix,const CString& refDes,const CString& pinName)
{
   //if (refDes.CompareNoCase("I102") == 0)
   //{
   //   if (pinName.CompareNoCase("ASIA1") == 0)
   //   {
   //      int iii = 3;
   //   }
   //}

   CTMatrix compToPinMatrix(pinToCompMatrix);
   compToPinMatrix.invert();

   CString compToPinMatrixDescriptor1 = compToPinMatrix.getDescriptor();

   //CPadstackBuilder padstackBuilder(m_zukenReader);

   int dataListIndex;
   CNamedDataList* namedDataList;
   CString dataListName;
   CString descriptor;
   double padstackInsertRotationRadians = -100.;
   CString compToPinMatrixDescriptor2;
   bool singlePadstackFlag = false;
   BlockStruct* padstackGeometry = NULL;
   CDataList sortedDataList(false);
   CDataList unsortedDataList(true);
   CPoint2d zero;

   CTMatrix rMatrix;

   // dataList in padstackCache are in component coordinate system
   for (m_padstackCacheMap.rewind(dataListIndex);m_padstackCacheMap.next(namedDataList,dataListName,dataListIndex);)
   {
      CBasesVector dataListBasesVector = namedDataList->getBasesVector();
      CString dataListBasesVectorDescriptor = dataListBasesVector.getDescriptor();

      CBasesVector subPadstackBasesVectorBoard = namedDataList->getBasesVector();
      subPadstackBasesVectorBoard.transform(compToBoardMatrix);
      CString dataListBasesVectorBoardDescriptor = subPadstackBasesVectorBoard.getDescriptor();

      CBasesVector dataListBasesVectorPin = dataListBasesVector;
      dataListBasesVectorPin.transform(compToPinMatrix);
      CString dataListBasesVectorPinDescriptor = dataListBasesVectorPin.getDescriptor();

      if (dataListIndex == 1)
      {
         padstackInsertRotationRadians = dataListBasesVector.getRotationRadians();
         //compToPinMatrix.rotateRadiansCtm(-padstackInsertRotationRadians);
         rMatrix.rotateRadians(-padstackInsertRotationRadians);

         compToPinMatrixDescriptor2 = compToPinMatrix.getDescriptor();

         if (m_padstackCacheMap.getCount() == 1 && namedDataList->getDataList().GetCount() == 1)
         {
            DataStruct* padStack = namedDataList->getDataList().GetHead();

            if (padStack->getDataType() == dataTypeInsert)
            {
               padstackGeometry = getCamCadDatabase().getBlock(padStack->getInsert()->getBlockNumber());

               if (padstackGeometry->getBlockType() == blockTypePadstack)
               {
                  if (dataListBasesVectorPin.getOrigin().fpeq(zero))
                  {
                     singlePadstackFlag = true;

                     break;
                  }
               }
            }
         }
      }

      rMatrix.translate(-dataListBasesVectorPin.getOrigin());

      dataListBasesVector.transform(compToPinMatrix);
      dataListBasesVector.transform(rMatrix);

      bool singlePadstackInsertFlag = false;
      DataStruct* padStack = NULL;
      CString debugBasesVectorDescriptor1,debugBasesVectorDescriptor2;

      descriptor.Empty();

      if (namedDataList->getDataList().GetCount() == 1)
      {
         padStack = namedDataList->getDataList().GetHead();
         debugBasesVectorDescriptor1 = padStack->getInsert()->getBasesVector().getDescriptor();

         if (padStack->getDataType() == dataTypeInsert)
         {
            singlePadstackInsertFlag = true;

            BlockStruct* padstackGeometry = getCamCadDatabase().getBlock(padStack->getInsert()->getBlockNumber());
            descriptor = padstackGeometry->getName() + ":" + dataListBasesVector.getDescriptor();
            padStack->getInsert()->setRefname(descriptor);

            for (POSITION pos = sortedDataList.GetHeadPosition();;)
            {
               if (pos == NULL)
               {
                  sortedDataList.AddTail(padStack);
                  break;
               }

               POSITION oldPos = pos;
               DataStruct* sortedData = sortedDataList.GetNext(pos);

               if (sortedData->getInsert()->getRefname().Compare(descriptor) > 0)
               {
                  sortedDataList.InsertBefore(oldPos,padStack);
                  break;
               }
            }
         }
      }

      namedDataList->getDataList().transform(compToPinMatrix);
      namedDataList->getDataList().transform(rMatrix);

      if (singlePadstackInsertFlag)
      {
         debugBasesVectorDescriptor2 = padStack->getInsert()->getBasesVector().getDescriptor();
      }
      else
      {
         m_zukenReader.formatErrorMessage("data list does not contain a single insert in %s",__FUNCTION__);
      }

      unsortedDataList.takeData(namedDataList->getDataList());
   }

   if (!singlePadstackFlag)
   {
      descriptor.Empty();

      for (POSITION pos = sortedDataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = sortedDataList.GetNext(pos);

         if (!descriptor.IsEmpty())
         {
            descriptor += "|";
         }

         descriptor += data->getInsert()->getRefname();
      }

      CString padstackGeometryName = getPadstackGeometryName(descriptor);

      padstackGeometry = getCamCadDatabase().getBlock(padstackGeometryName,m_zukenReader.getCadFileNumber());

      if (padstackGeometry == NULL)
      {
         padstackGeometry = getCamCadDatabase().getNewBlock(padstackGeometryName,"-%d",blockTypePadstack,m_zukenReader.getCadFileNumber());

         unsortedDataList.transform(compToPinMatrix);
         unsortedDataList.transform(rMatrix);
         padstackGeometry->getDataList().takeData(unsortedDataList);
      }
   }

   m_padstackCacheMap.empty();

   pinBasesVector.set();
   pinBasesVector.transform(pinToCompMatrix);

   // debug
   CString pinBasesVectorDescriptor = pinBasesVector.getDescriptor();

   return padstackGeometry;
}

BlockStruct* CCr5kMappedLayerDataList::getRestructuredPadstack(CBasesVector& pinBasesVector,const CTMatrix& pinToCompMatrix,const CTMatrix& compToBoardMatrix,
   bool bottomPlacedComponentFlag,const CString& refDes,const CString& pinName)
{
   bool debugAxisFlag = false;
   bool enableDebugAxisFlag = false;
   CString pinRef = refDes + "." + pinName;

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("\n" "Entered %s  ---------------------------------------------------------------------\n",__FUNCTION__);
   }

   CTMatrix compToPinMatrix(pinToCompMatrix);
   compToPinMatrix.invert();

   CString compToPinMatrixDescriptor1 = compToPinMatrix.getDescriptor();

   CPadstackBuilder padstackBuilder(m_zukenReader);

   int dataListIndex;
   CNamedDataList* namedDataList;
   CString dataListName;
   CString concatenatedDescriptor;
   double padstackInsertRotationRadians = -100.;
   CString compToPinMatrixDescriptor2;
   bool singlePadstackFlag = false;
   BlockStruct* padstackGeometry = NULL;

   CTMatrix rMatrix;
   CPoint2d zero;

   // dataLists in padstackCache are in component coordinate system
   for (m_padstackCacheMap.rewind(dataListIndex);m_padstackCacheMap.next(namedDataList,dataListName,dataListIndex);)
   {
      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n------------\n" "pinRef = '%s', dataListIndex = %d\n",pinRef,dataListIndex);
      }

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n" "pinRef = '%s' old DataList contents for namedDataList='%s'\n",pinRef,dataListName);
         dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),namedDataList->getDataList());
      }

      if (bottomPlacedComponentFlag)
      {
         //if (getDebugRefDesFlag())
         //{
         //   CDebugWriteFormat::getWriteFormat().writef("\n" "pinRef = '%s' bottomPlacedComponent old DataList contents for namedDataList='%s'\n",pinRef,dataListName);
         //   dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),namedDataList->getDataList());
         //}

         for (CDataListIterator insertIterator(namedDataList->getDataList(),dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();
            CBasesVector basesVector = data->getInsert()->getBasesVector();
            basesVector.set(basesVector.getOrigin(),basesVector.getRotationDegrees());
            data->getInsert()->setBasesVector(basesVector);
         }

         //if (getDebugRefDesFlag())
         //{
         //   CDebugWriteFormat::getWriteFormat().writef("\n" "pinRef = '%s' bottomPlacedComponent new DataList contents for namedDataList='%s'\n",pinRef,dataListName);
         //   dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),namedDataList->getDataList());
         //}
      }

      CBasesVector subPadstackBasesVectorComp = namedDataList->getBasesVector();
      CString subPadstackBasesVectorCompDescriptor = subPadstackBasesVectorComp.getDescriptor();

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n" "subPadstackBasesVectorComp = '%s'\n",subPadstackBasesVectorCompDescriptor);
      }


      // try
      CBasesVector pinBasesVectorComp;
      pinBasesVectorComp.transform(pinToCompMatrix);
      CString pinBasesVectorCompDescriptor = pinBasesVectorComp.getDescriptor();

      CBasesVector pinBasesVectorBoard(pinBasesVectorComp);
      pinBasesVectorBoard.transform(compToBoardMatrix);
      CString pinBasesVectorBoardDescriptor = pinBasesVectorBoard.getDescriptor();

      if (debugAxisFlag) CDebug::addDebugAxis(getCamCadDatabase(),pinBasesVectorBoard,AxisSize,"db-pinBasesVectorBoard");

      //CBasesVector pinOffsetBasesVector = subPadstackBasesVectorComp;
      //pinOffsetBasesVector.transform(compToPinMatrix);
      //CString pinOffsetBasesVectorDescriptor = pinOffsetBasesVector.getDescriptor();

      //CBasesVector subPadstackBasesVectorBoard(subPadstackBasesVectorComp);
      //subPadstackBasesVectorBoard.transform(compToBoardMatrix);
      //CString dataListBasesVectorBoardDescriptor = subPadstackBasesVectorBoard.getDescriptor();

      //if (debugAxisFlag) CDebug::addDebugAxis(getCamCadDatabase(),subPadstackBasesVectorBoard,AxisSize,"db-subPadstackBasesVectorBoard");

      CPoint2d pinOffsetComp = subPadstackBasesVectorComp.getOrigin() - pinBasesVectorComp.getOrigin();
      double angleOffsetCompRadians = subPadstackBasesVectorComp.getRotationRadians() - pinBasesVectorComp.getRotationRadians();
      CString pinOffsetDescriptor = pinOffsetComp.getString();

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("pinBasesVectorComp='%s'\n",pinBasesVectorComp.getDescriptor());
         CDebugWriteFormat::getWriteFormat().writef("pinOffsetComp='%s'\n",pinOffsetDescriptor);
      }

      CTMatrix padstackToPinMatrix;
      padstackToPinMatrix.translateCtm(pinOffsetComp);
      padstackToPinMatrix.rotateRadiansCtm(angleOffsetCompRadians);
      CString padstackMatrixDescriptor = padstackToPinMatrix.getDescriptor();

      CTMatrix padstackToCompMatrix;
      padstackToCompMatrix = padstackToPinMatrix * pinToCompMatrix;
      CString padstackToCompMatrixDescriptor = padstackToCompMatrix.getDescriptor();

      CTMatrix compToPadstackMatrix(padstackToCompMatrix);
      compToPadstackMatrix.invert();

      //CBasesVector padstackBasesVectorComp;
      //padstackBasesVectorComp.transform(padstackToCompMatrix);
      //CString padstackBasesVectorCompDescriptor = padstackBasesVectorComp.getDescriptor();

      //CBasesVector padstackBasesVectorBoard(padstackBasesVectorComp);
      //padstackBasesVectorBoard.transform(compToBoardMatrix);
      //CString padstackBasesVectorBoardDescriptor = padstackBasesVectorBoard.getDescriptor();

      //if (debugAxisFlag) CDebug::addDebugAxis(getCamCadDatabase(),padstackBasesVectorBoard,AxisSize,"db-padstackBasesVectorBoard");

      // debug
      //CBasesVector dbZeroBasesVector;
      //dbZeroBasesVector.transform(compToPadstackMatrix);
      //CString dbZeroBasesVectorDescriptor = dbZeroBasesVector.getDescriptor();

      // debug
      //CBasesVector dbDataListBasesVector = subPadstackBasesVectorComp;
      //dbDataListBasesVector.transform(compToPadstackMatrix);
      //CString dbDataListBasesVectorDescriptor = dbDataListBasesVector.getDescriptor();

      // debug
      //CBasesVector dbPinOffsetBasesVector(pinOffsetBasesVector);
      //dbPinOffsetBasesVector.transform(compToPadstackMatrix);
      //CString dbPinOffsetBasesVectorDescriptor = dbPinOffsetBasesVector.getDescriptor();

      // the transformation
      //CBasesVector subPadstackBasesVectorPin(subPadstackBasesVectorComp);
      //subPadstackBasesVectorPin.transform(compToPinMatrix);   // compToPinMatrix looks like it
      //dataListBasesVector.transform(rMatrix);
      //CString descriptor;
      //descriptor.Format("%d:%s%s",dataListIndex,namedDataList->getName(),subPadstackBasesVectorPin.getDescriptor());

      //concatenatedDescriptor += descriptor;
      //m_zukenReader.filterDataList(namedDataList->getDataList(),namedDataList->getFromLayerNumber(),namedDataList->getToLayerNumber());

      // check the matrices
      //CBasesVector basesVector;
      //basesVector.transform(padstackToPinMatrix);
      //CString pinDescriptor = basesVector.getDescriptor();

      //basesVector.transform(pinToCompMatrix);
      //CString compDescriptor = basesVector.getDescriptor();
      //CBasesVector compBasesVector(basesVector);

      //basesVector.transform(compToBoardMatrix);
      //CString boardDescriptor = basesVector.getDescriptor();

      //compBasesVector.transform(compToPadstackMatrix);
      //CString padstackDescriptor = compBasesVector.getDescriptor();

      // check the named datalist padstacks and pads 
      for (CDataListIterator dbInsertIterator(namedDataList->getDataList());dbInsertIterator.hasNext();)
      {
         DataStruct* data = dbInsertIterator.getNext();
         BlockStruct* geometry = getCamCadDatabase().getBlock(data->getInsert()->getBlockNumber());
         CString geometryName = geometry->getName();

         CBasesVector dbInsertBasesVectorComp = data->getInsert()->getBasesVector();
         CString dbInsertBasesVectorCompDescriptor = dbInsertBasesVectorComp.getDescriptor();

         //CBasesVector dbInsertBasesVectorPadstack(dbInsertBasesVectorComp);
         //dbInsertBasesVectorPadstack.transform(compToPadstackMatrix);
         //CString dbInsertBasesVectorPadstackDescriptor = dbInsertBasesVectorPadstack.getDescriptor();

         CBasesVector dbInsertBasesVectorPin(dbInsertBasesVectorComp);
         dbInsertBasesVectorPin.transform(compToPinMatrix);
         CString dbInsertBasesVectorPinDescriptor = dbInsertBasesVectorPin.getDescriptor();

         CString descriptor;
         descriptor.Format("<%d:%s%s>",dataListIndex,geometryName,dbInsertBasesVectorPinDescriptor);
         concatenatedDescriptor += descriptor;
      }

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n" "Data List Contents in component space\n");
         dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),namedDataList->getDataList());
      }

      // transform to padstack coordinate system
      //namedDataList->getDataList().transform(compToPadstackMatrix);
      namedDataList->getDataList().transform(compToPinMatrix);

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n" "Data List Contents after transformation to pin space\n");
         dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),namedDataList->getDataList());
      }

      // this is where the data list is flattened, where, among other things, complex apertures are converted to polys
      padstackBuilder.takeData(namedDataList->getDataList(),namedDataList->getFromLayerNumber(),namedDataList->getToLayerNumber());

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n" "padstackBuilder data list contents\n");
         dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),padstackBuilder.getDataList());
      }
   }

   m_padstackCacheMap.empty();

   if (!singlePadstackFlag)
   {
      //m_zukenReader.formatErrorMessage("Non single pad case found in CCr5kMappedLayerDataList::instantiatePadstackCache() - inform Kurt N. Van Ness");

      CCrcCalculator crcCalculator;
      crcCalculator.input(concatenatedDescriptor);
      unsigned int crc = crcCalculator.getCrc();

      CString padstackGeometryName;
      if (crc != 0)
         padstackGeometryName.Format("PS_%08x",crc);
      else
         padstackGeometryName.Format("PS_%s",pinRef);


      if (m_zukenReader.getOptionAddPadstackDescriptor() && concatenatedDescriptor.GetLength() < MaxPadstackDescriptorLength)
      {
         padstackGeometryName.AppendFormat("<%s>",concatenatedDescriptor);
      }

      padstackGeometry = getCamCadDatabase().getBlock(padstackGeometryName,m_zukenReader.getCadFileNumber());

      if (padstackGeometry == NULL)
      {
         CTMatrix matrix;

         padstackGeometry = getCamCadDatabase().getNewBlock(padstackGeometryName,"-%d",blockTypePadstack,m_zukenReader.getCadFileNumber());
         padstackBuilder.buildPadstack(padstackGeometry->getDataList(),padstackGeometryName,m_zukenReader.getCadFileNumber(),matrix);

         if (getDebugRefDesFlag())
         {
            CDebugWriteFormat::getWriteFormat().writef("\n" "Created new padstack geometry '%s'\n",padstackGeometryName);
            dumpDataList(CDebugWriteFormat::getWriteFormat(),getCamCadDatabase(),padstackGeometry->getDataList());
         }
      }
      else
      {
         if (getDebugRefDesFlag())
         {
            CDebugWriteFormat::getWriteFormat().writef("\n" "Using existing padstack geometry '%s'\n",padstackGeometryName);
         }
      }
   }

   pinBasesVector.set();
   pinBasesVector.transform(pinToCompMatrix);

   //if (getDebugRefDesFlag())
   //{
   //   CDebugWriteFormat::getWriteFormat().writef("pinRef = '%s' preadjusted pinBasesVector='%s'\n",pinRef,pinBasesVector.getDescriptor());
   //}

   //if (pinBasesVector.getMirror())
   //{
   //   pinBasesVector.set(pinBasesVector.getOrigin().x,pinBasesVector.getOrigin().y,180. - pinBasesVector.getRotationDegrees(),pinBasesVector.getMirror());
   //}

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("pinRef = '%s' adjusted pinBasesVector='%s'\n",pinRef,pinBasesVector.getDescriptor());
   }

   // debug
   CString pinBasesVectorDescriptor = pinBasesVector.getDescriptor();

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("\n" "Exiting %s  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",__FUNCTION__);
   }

   return padstackGeometry;
}

void CCr5kMappedLayerDataList::instantiatePadstackCache(const CTMatrix& pinToCompMatrix,const CTMatrix& componentToBoardMatrix,bool bottomPlacedComponentFlag,const CString& refDes,const CString& pinName)
{
   CString pinRef = refDes + "." + pinName;

   // if mode is padstackCacheMode, then datas in datalists are in component coordinates
   bool conglomeratePadstacksFlag = m_zukenReader.getOptionConglomeratePadstacks();
   CPoint2d zero;

   if (getPadstackCacheModeFlag())
   {
      BlockStruct* padstackGeometry = NULL;

      if (m_cacheStackCount != 0)
      {
         getApp().getMessageFilterTypeFormat().formatMessageBoxApp("Logic Error in '%s'",__FUNCTION__);
      }

      CBasesVector pinBasesVector;

      if (conglomeratePadstacksFlag)
      {
         padstackGeometry = getConglomeratedPadstack(pinBasesVector,pinToCompMatrix,componentToBoardMatrix,refDes,pinName);
      }
      else
      {
         padstackGeometry = getRestructuredPadstack(pinBasesVector,pinToCompMatrix,componentToBoardMatrix,bottomPlacedComponentFlag,refDes,pinName);
      }

      CString pinBasesVectorDescriptor = pinBasesVector.getDescriptor();

      DataStruct* pinData = getCamCadDatabase().insertBlock(padstackGeometry,insertTypePin,pinName,-1,
                              pinBasesVector.getOrigin().x,pinBasesVector.getOrigin().y,
                              pinBasesVector.getRotationRadians(),pinBasesVector.getMirror());

      addTailWithoutLayerMapping(pinData,-1);

      setPadstackCacheModeFlag(false);
   }
   else
   {
      CTMatrix compToPinMatrix(pinToCompMatrix);
      compToPinMatrix.invert();

      m_dataList->transform(compToPinMatrix);

      CDataList sortedDataList(false);
      CString descriptor;

      for (CDataListIterator insertIterator(*m_dataList,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         InsertStruct* insert = data->getInsert();

         BlockStruct* geometry = getCamCadDatabase().getBlock(insert->getBlockNumber());
         descriptor = geometry->getName() + ":" + insert->getBasesVector().getDescriptor();
         insert->setRefname(descriptor);

         for (POSITION pos = sortedDataList.GetHeadPosition();;)
         {
            if (pos == NULL)
            {
               sortedDataList.AddTail(data);
            }

            POSITION oldPos = pos;
            DataStruct* sortedData = sortedDataList.GetNext(pos);

            if (sortedData->getInsert()->getRefname().Compare(descriptor) > 0)
            {
               sortedDataList.InsertBefore(oldPos,data);
               break;
            }
         }
      }

      descriptor.Empty();

      for (POSITION pos = sortedDataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = sortedDataList.GetNext(pos);

         if (!descriptor.IsEmpty())
         {
            descriptor += "|";
         }

         descriptor += data->getInsert()->getRefname();
      }

      CString padstackGeometryName = getPadstackGeometryName(descriptor);

      BlockStruct* padstackGeometry = getCamCadDatabase().getBlock(padstackGeometryName,m_zukenReader.getCadFileNumber());

      if (padstackGeometry == NULL)
      {
         padstackGeometry = getCamCadDatabase().getNewBlock(padstackGeometryName,"-%d",blockTypePadstack,m_zukenReader.getCadFileNumber());

         padstackGeometry->getDataList().takeData(*m_dataList);
      }

      CBasesVector pinBasesVector;
      pinBasesVector.transform(pinToCompMatrix);

      // debug
      CString pinBasesVectorDescriptor = pinBasesVector.getDescriptor();

      DataStruct* pinData = getCamCadDatabase().insertBlock(padstackGeometry,insertTypePin,pinName,-1,
                               pinBasesVector.getOrigin().x,pinBasesVector.getOrigin().y,
                               pinBasesVector.getRotationRadians(),pinBasesVector.getMirror());

      addTailWithoutLayerMapping(pinData,-1);
   }
}

int CCr5kMappedLayerDataList::addTail(DataStruct*& data,const CString& footLayerName,int materialNumber,int fromLayerNumber,int toLayerNumber,
   const CCr5kConductivePadStackPad* conductivePadStackPad,Cr5kTokenTag padStatus)
{
   int retval = 0;

   bool fromToFlag = (fromLayerNumber > 0 && toLayerNumber > 0);

   if (! isLayerMappingEnabled())
   {
      bool notInRangeFlag = (fromToFlag && ! m_zukenReader.isLayerConductiveOrInConductiveRange(footLayerName,fromLayerNumber,toLayerNumber));

      if (! notInRangeFlag)
      {
         LayerStruct& layer = m_zukenReader.getDefinedLayer(footLayerName,materialNumber,!m_topFlag);

         data->setLayerIndex(layer.getLayerIndex());
         m_dataList->AddTail(data);

         retval = 1;
      }
   }
   else
   {
      const CStringList& boardLayers = m_layerMapping->getMappedBoardLayers(m_thruFlag,m_topFlag,footLayerName);

      DataStruct* mappedData = data;

      for (POSITION pos = boardLayers.GetHeadPosition();pos != NULL;)
      {
         CString boardLayerName = boardLayers.GetNext(pos);
         bool conductiveLayerAssociateFlag = m_zukenReader.isConductiveLayerAssociate(boardLayerName);

         if (fromToFlag && conductiveLayerAssociateFlag)
         {
            if (! m_zukenReader.isLayerConductiveOrInConductiveRange(boardLayerName,fromLayerNumber,toLayerNumber))
            {
               continue;
            }
         }

         if (conductivePadStackPad != NULL)
         {
            bool skipLayerFlag = conductiveLayerAssociateFlag;

            for (POSITION pos = conductivePadStackPad->getHeadPosition();pos != NULL && skipLayerFlag;)
            {
               CCr5kConductivePadStackPadLayer* conductivePadStackPadLayer = conductivePadStackPad->getNext(pos);

               if (conductivePadStackPadLayer->getStatus() == padStatus)
               {
                  int layerNumber = conductivePadStackPadLayer->getLayerNumber();

                  if (m_zukenReader.isLayerInConductiveRange(boardLayerName,layerNumber,layerNumber))
                  {
                     skipLayerFlag = false;
                     break;
                  }
               }
            }

            if (skipLayerFlag)
            {
               continue;
            }
         }

         LayerStruct& layer = m_zukenReader.getDefinedLayer(boardLayerName,materialNumber,!m_topFlag);

         if (mappedData == NULL)
         {
            mappedData = new_DataStruct(*data);
         }

         mappedData->setLayerIndex(layer.getLayerIndex());

         m_dataList->AddTail(mappedData);

         mappedData = NULL;

         retval++;
      }
   }

   if (retval == 0)
   {
      delete data;
      data = NULL;
   }

   return retval;
}

int CCr5kMappedLayerDataList::addTailWithoutLayerMapping(DataStruct*& data,int layerIndex)
{
   int retval = 1;

   data->setLayerIndex(layerIndex);
   m_dataList->AddTail(data);

   return retval;
}

//_____________________________________________________________________________
CCr5kLinkedGeometries::CCr5kLinkedGeometries()
: m_linkedPads(500)
, m_linkedPadstacks(500)
, m_linkedFootprints(2000)
{
}

void CCr5kLinkedGeometries::initialize(CCr5kBoardFile& boardFile,CCr5kFootprintFile& footprintFile)
{
   m_linkedPads.empty();
   m_linkedPadstacks.empty();
   m_linkedFootprints.empty();
   // But not this one:  m_linkedComponents.empty();

   int linkCount = m_linkedPads.getCount() + m_linkedPadstacks.getCount() + m_linkedFootprints.getCount()/* + m_linkedComponents.getCount()*/;

   COperationProgress progress;
   progress.updateStatus("Linking data.",linkCount);

   footprintFile.linkPads(m_linkedPads,&progress);
   footprintFile.linkPadstacks(m_linkedPadstacks,&progress);
   footprintFile.linkFootprints(m_linkedFootprints,&progress);

   // Still not this one:  boardFile.linkComponents(m_linkedComponents,&progress);
}

CCr5kLinkedPad* CCr5kLinkedGeometries::getLinkedPad(const CString& padName)
{
   return m_linkedPads.getElement(padName);
}

CCr5kLinkedPadstack* CCr5kLinkedGeometries::getLinkedPadstack(const CString& padstackName)
{
   return m_linkedPadstacks.getElement(padstackName);
}

CCr5kLinkedFootprint* CCr5kLinkedGeometries::getLinkedFootprint(const CString& geometryName)
{
   return m_linkedFootprints.getElement(geometryName);
}

//_____________________________________________________________________________
CCr5kMirroredLayerEntry::CCr5kMirroredLayerEntry(const CString& commandString,const CString& layerExpression1,const CString& layerExpression2,bool baseLayerFlag)
: m_commandString(commandString)
, m_layerExpression1(layerExpression1)
, m_layerExpression2(layerExpression2)
, m_baseLayerFlag(baseLayerFlag)
{
}

//_____________________________________________________________________________
CCr5kMirroredLayers::CCr5kMirroredLayers()
{
}

void CCr5kMirroredLayers::addLayerEntry(const CString& commandString,const CString& layerExpression1,const CString& layerExpression2)
{
   CCr5kMirroredLayerEntry* mirroredLayerEntry = new CCr5kMirroredLayerEntry(commandString,layerExpression1,layerExpression2,false);

   m_mirroredLayersList.AddTail(mirroredLayerEntry);
}

void CCr5kMirroredLayers::addBaseLayerEntry(const CString& commandString,const CString& layerExpression1,const CString& layerExpression2)
{
   CCr5kMirroredLayerEntry* mirroredLayerEntry = new CCr5kMirroredLayerEntry(commandString,layerExpression1,layerExpression2,true);

   m_mirroredLayersList.AddTail(mirroredLayerEntry);
}

//_____________________________________________________________________________
CZukenCr5000Reader::CZukenCr5000Reader(CCamCadDatabase& camCadDatabase,const CString& projectName)
: m_camCadDatabase(camCadDatabase)
, m_camCadDoc(camCadDatabase.getCamCadDoc())
, m_projectName(projectName)
//, m_units(camCadDatabase.getPageUnits())
//, m_cr5kPageUnits(pageUnitsUndefined)
, m_logFile(NULL)
, m_debugFile(NULL)
, m_optionLibraryMode(false)
, m_optionAdjustLayerNames(false)
, m_optionAddSourceLineNumbers(false)
, m_optionAddPadstackDescriptor(false)
, m_optionUseOldReader(false)
, m_optionTestFtfRead(false)
, m_optionTestPcfRead(false)
, m_optionTestPnfRead(false)
, m_optionInstantiateConnectPads(true)
, m_optionInstantiateNoconnectPads(false)
, m_optionInstantiateThermalPads(false)
, m_optionInstantiateClearancePads(false)
, m_optionInstantiateFillets(true)
, m_optionPurgeUnusedGeometries(true)
, m_optionShowPurgeReport(true)
, m_optionUsePnfAsPcf(false)
, m_optionShowHiddenRefDes(true)
, m_optionRestructurePadstacks(true)
, m_optionConglomeratePadstacks(false)
, m_optionSegregateGeneratedBondwires(false)
, m_optionSegregateBondwiresByDiameter(false)
, m_optionIgnoreOutOfBoard(false)
, m_optionGenerateLoadedAttribute(true)
, m_optionDefaultVariantLoadedFlag(true)
, m_optionGenerateMirroredLayers(false)
, m_optionEnableLayerMirroring(true)
, m_optionMirrorElectricalLayers(true)
, m_optionEnablePanelFtfFile(true)
, m_optionGenerateDebugFile(false)
, m_optionGenerateLogFile(true)
, m_optionDisplayErrorMessages(false)
, m_optionSupportMultiByteCharacters(true)
, m_optionConvertComplexApertures(false)
, m_optionEnableReverseFootprints(true)
, m_optionDefaultViaPadDiameter(  getUnits().convertFrom(pageUnitsMilliMeters,.1  ))
, m_optionDefaultViaDrillDiameter(getUnits().convertFrom(pageUnitsMilliMeters,.075))
, m_optionProbeDiameter(getUnits().convertFrom(pageUnitsMilliMeters,.1))
, m_optionProbeDrillDiameter(getUnits().convertFrom(pageUnitsMilliMeters,.075))
, m_optionTestAccessDiameter(getUnits().convertFrom(pageUnitsMilliMeters,.12))
, m_optionTextHeightFactor(1.)
, m_optionTextWidthFactor(1.)
, m_optionAttributeHeightFactor(1.)
, m_optionAttributeWidthFactor(1.)
, m_cadFile(NULL)
, m_pcbFile(NULL)
, m_panFile(NULL)
, m_testAccessGeometry(NULL)
, m_testProbeGeometry(NULL)
, m_startingBlockIndex(camCadDatabase.getCamCadDoc().getMaxBlockIndex())
, m_subBoardGeometry(NULL)
, m_sourceLineNumberKeywordIndex(-1)
, m_parentReferenceKeywordIndex(-1)
, m_partKeywordIndex(-1)
, m_packageKeywordIndex(-1)
, m_stockIdKeywordIndex(-1)
, m_messageFilter(messageFilterTypeMessage)
, m_padstackGeometryPrefix("PS_")
, m_cr5kBottomSuffix("_m")
, m_unnamedPadCount(0)
//, m_linkedPanelGeometries(NULL)
//, m_linkedBoardPads(500)
//, m_linkedBoardPadstacks(500)
//, m_linkedBoardFootprints(2000)
{
   GetConsolidatorControl().SetIsEnabled(true); // Consolidator is On by default

   m_optionMirroredLayers.InitHashTable(nextPrime2n(100));
   m_optionLayerTypes.InitHashTable(nextPrime2n(50));
   //CDebug::setAddEnable(true);
   CCr5kElement::resetNextId();
}

CZukenCr5000Reader::~CZukenCr5000Reader()
{
   //delete m_linkedPanelGeometries;
   closeLog();
   delete m_debugFile;

   empty();

   CDebugWriteFormat::close();
}

void CZukenCr5000Reader::empty()
{
   m_boardFootprintFile.empty();
   m_panelFootprintFile.empty();
   m_boardFile.empty();
   m_panelFile.empty();
}

CString CZukenCr5000Reader::getLogFilePath(const CString& filePathString)
{
   CFilePath logFilePath(m_logFileDirectory);

   logFilePath.pushLeaf(filePathString);

   CString logFilePathString = logFilePath.getPath();

   return logFilePathString;
}

void CZukenCr5000Reader::setLogFileDirectoryPath(const CString& directoryPathString)
{
   m_logFileDirectory = directoryPathString;

#ifdef HtmlDebugOutput
   m_debugFilePath = GetLogfilePath("ZukenCr5000ReaderDebug.htm"));
#else
   m_debugFilePath = GetLogfilePath(("ZukenCr5000ReaderDebug.txt"));
#endif
}

CWriteFormat& CZukenCr5000Reader::getDebug()
{
   if (m_debugFile == NULL)
   {
      if (getOptionGenerateDebugFile())
      {
#ifdef HtmlDebugOutput
         CHtmlFileWriteFormat* debugFile = new CHtmlFileWriteFormat(1024);
#else
         CStdioFileWriteFormat* debugFile = new CStdioFileWriteFormat(1024);
#endif

         debugFile->open(m_debugFilePath);

         debugFile->setPrefix(PrefixGet  ,"  get> ");
         debugFile->setPrefix(PrefixUnget,"unget> ");

#ifdef HtmlDebugOutput
         debugFile->getRecordParser().setRecordPrefixAndColor(PrefixGet  ,"  get> ",colorBlack);
         debugFile->getRecordParser().setRecordPrefixAndColor(PrefixUnget,"unget> ",colorRed);
#endif

         m_debugFile = debugFile;
      }
      else
      {
         m_debugFile = new CNullWriteFormat();
      }
   }

   return *m_debugFile;
}

CWriteFormat& CZukenCr5000Reader::getLog()
{
   if (m_logFile == NULL)
   {
      if (getOptionGenerateLogFile())
      {
         CHtmlFileWriteFormat* logFile = new CHtmlFileWriteFormat(1024);

         CFilePath logFilePath(m_logFileDirectory);

         if (m_logFileName.IsEmpty())
         {
            m_logFileName = "ZukenCr5000Reader.htm";
         }

         logFilePath.pushLeaf(m_logFileName);
         CString logFilePathString = logFilePath.getPath();

         logFile->open(logFilePathString);
         m_logFile = logFile;
      }
      else
      {
         m_logFile = new CNullWriteFormat();
      }
   }

   return *m_logFile;
}

void CZukenCr5000Reader::closeLog()
{
   if (m_logFile != NULL)
   {
      delete m_logFile;
      m_logFile = NULL;
   }
}
void CZukenCr5000Reader::dts0100713526_CleanUpGeomNames()
{
   // DR 713526 asks for _m suffixes to be removed, to use original geom name as-is.
   // This importer makes top and bottom versions of geometries, always, when a given
   // geom is used on both top and bottom.
   // If layer mirroring is properly set up then the top and bottom geoms are usually
   // equivalent, and if Geometry Consolidator option is on, it will find these
   // equivalencies, and it will favor the original name.
   // The problem comes when there are geometries used only on the bottom.
   // In that case, we end up with only the _m version of the geometry.
   // In turn, there is nothing to Condolidate for these, so they remain as-is.
   // So this is a little post-process function that scans all the geoms, looking
   // for names that end in _m. If/when one is found, we look for the plain-named
   // version of the geom. If the plain-named geom exists, then we leave all as-is, we
   // don't want to end up with duplicate names. But if the plain-named version
   // does not exist, then we convert the geom in hand to the plain name, i.e. remove
   // the _m suffix. Arguably, this could be done in Geometry Consolidator, it seems
   // a good general overall feataure, but it would also have to treat _Bot, the other
   // suffix used by some other importers. For this DR we do not want to have to test
   // other importers, so by order of Mark Laing via email 9 Sep 2010 we are putting
   // this post process here, specific to CR5K only.

   int botSuffixLen = m_cr5kBottomSuffix.GetLength();

   if (botSuffixLen > 0)
   {
      CCamCadData &ccData = this->getCamCadData();

      for (int blockIndex = 0; blockIndex < ccData.getMaxBlockIndex(); blockIndex++)
      {
         BlockStruct* block = ccData.getBlockAt(blockIndex);

         if (block != NULL)
         {
            CString blockName( block->getName() );
            CString potentialSuffix( blockName.Right(botSuffixLen) );

            CString plainName;

            if (potentialSuffix.Compare(m_cr5kBottomSuffix) == 0 && blockName.GetLength() > botSuffixLen)
            {
               plainName = blockName.Left(blockName.GetLength() - botSuffixLen);
            }
            else if (block->getBlockType() == blockTypePadstack)
            {
               // Special case for padstacks, see getInstantiatedPadstackGeometry() for additional
               // suffixing that may be applied to padstacks. If the padstack geom had suffix at the
               // end then it was already handled above, so we only need to treat the "embedded suffix"
               // situation here. Take care to keep this in sync with getInstatiatedPadstackGeometry().
               CString adjustedSuffix( m_cr5kBottomSuffix + ":" );
               if (blockName.Find(adjustedSuffix) > -1)
               {
                  plainName = blockName;
                  plainName.Replace(adjustedSuffix, ":"); // just take out the suffix, leave the ":" delimiter.
               }
            }

            if (!plainName.IsEmpty())
            {
               if (ccData.getBlock(plainName, block->getFileNumber()) == NULL)
               {
                  // Block with plain name does not exist, rename current block to plain name.
                  block->setName(plainName);
               }
            }
         }
      }

      this->getCamCadDatabase().invalidateBlockIndex();  // Because index map is set up based on block name.
   }
}

bool CZukenCr5000Reader::read(const CString& filePath,Cr5kTokenTag startToken)
{
   CFilePath logFilePath(filePath);

   if (m_logFileDirectory.IsEmpty() || m_logFileDirectory == ".")
   {
      CString directoryPath = logFilePath.getDirectoryPath();
      setLogFileDirectoryPath(directoryPath);
   }

   m_logFileName = logFilePath.getBaseFileName() + "-" + logFilePath.getExtension() + "-ZukenCr5000ReaderLog.htm";

   closeLog();

   getLog().writef("Reading file '%s'\n",filePath);

   CCr5kTokenReader tokenReader(*this,filePath);

   return read(tokenReader,startToken);
}

bool CZukenCr5000Reader::read(CCr5kTokenReader& tokenReader,Cr5kTokenTag startToken)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   try
   {
      while (true)
      {
         Cr5kTokenTag token = tokenReader.getNextTokenTag();

         if (token == tok_Eof)
         {
            retval = false;
            break;
         }

         if (token == ptokPnf && getOptionUsePnfAsPcf() && getPanelDataFlag())
         {
            token = ptokPcf;
         }

         if (token == ptokFtf && startToken == ptokPanelFtf)
         {
            token = ptokPanelFtf;
         }

         if (token == startToken)
         {
            switch (startToken)
            {
            case ptokPanelFtf:
               tokenReader.setSourceFileType(sourceFileTypeFtf);
               retval = m_panelFootprintFile.read(tokenReader);
               break;
            case ptokFtf:
               tokenReader.setSourceFileType(sourceFileTypeFtf);
               retval = m_boardFootprintFile.read(tokenReader);
               break;
            case ptokPcf:
               tokenReader.setSourceFileType(sourceFileTypePcf);
               retval = m_boardFile.read(tokenReader);
               break;
            case ptokPnf:
               tokenReader.setSourceFileType(sourceFileTypePnf);
               retval = m_panelFile.read(tokenReader);
               break;
            }

            break;
         }
         else
         {
            tokenReader.skipList();
         }
      }
   }
   catch (CCr5kTokenReaderException e)
   {
      CString reason = e.getReason();
      getLog().writef("Exception encountered: %s\n", reason);
      retval = false;
   }

   return retval;
}

bool CZukenCr5000Reader::readDstFile(const CString& filePath)
{
   bool retval = true;

   CStdioFile dstFile;
   FileStruct* pcbFile = getPcbFile();

   if (pcbFile != NULL && dstFile.Open(filePath,CFile::modeRead))
   {
      CVariantList& variantList = pcbFile->getVariantList();
      CTypedPtrArrayContainer<CVariant*> variantArray(10,false);

      enum
      {
         lookForVariantDefinitions,
         variantDefinitions,
         lookForVariantItems,
         variantItems,
         complete
      } state = lookForVariantDefinitions;

      CSupString line;
      CStringArray params;

      while (state != complete && dstFile.ReadString(line))
      {
         line.Replace("\t"," ");
         int numPar = line.ParseQuote(params," ");

         switch (state)
         {
         case lookForVariantDefinitions:
            {
               if (numPar == 3 && params.GetAt(0).CompareNoCase("destination") == 0 &&
                                  params.GetAt(1).CompareNoCase("2"          ) == 0  &&
                                  params.GetAt(2).CompareNoCase("{"          ) == 0      )
               {
                  state = variantDefinitions;
               }
            }

            break;
         case variantDefinitions:
            {
               if (numPar == 2)
               {
                  CString variantName = params.GetAt(0) + "-" + params.GetAt(1);
                  CVariant* variant = variantList.AddVariant(getCamCadData_Doc(),*pcbFile,variantName, false);
                  if(variant != NULL)
                  {
                     variantArray.Add(variant);
                     variant->setLoadedAttributes(getCamCadData_Doc(),getOptionDefaultVariantLoadedFlag());
                  }
               }
               else if (numPar == 1)
               {
                  if (params.GetAt(0) == "}")
                  {
                     state = lookForVariantItems;
                  }
               }
            }

            break;
         case lookForVariantItems:
            {
               if (numPar == 3 && params.GetAt(0).CompareNoCase("partName") == 0 &&
                                  params.GetAt(2).CompareNoCase("{"       ) == 0      )
               {
                  state = variantItems;
               }
            }

            break;
         case variantItems:
            {
               if (numPar == 1 && params.GetAt(0) == "}")
               {
                  state = complete;
               }
               else if (numPar > 0)
               {
                  int variantCount = variantArray.GetCount();
                  CString refDes = params.GetAt(0);

                  for (int variantIndex = 0;variantIndex < variantCount;variantIndex++)
                  {
                     int index = variantIndex + 1;
                     CString partNumber;
                     
                     if (index < numPar)
                     {
                        partNumber = params.GetAt(index);
                     }

                     bool loadedFlag = !partNumber.IsEmpty();

                     CVariant* variant = variantArray.GetAt(variantIndex);
                     CVariantItem* variantItem = variant->FindItem(refDes);

                     if (variantItem != NULL)
                     {
                        variantItem->setLoaded(getCamCadData_Doc(),loadedFlag);
                        variantItem->setPartNumber(getCamCadData_Doc(),partNumber);
                     }
                  }
               }
            }

            break;
         }
      }
   }

   return retval;
}

bool CZukenCr5000Reader::loadSettings(const CString& filePath)
{
   const CUnits& docUnits = getCamCadDoc().getUnits();

   CInFile inFile;

   if (! inFile.open(filePath))
   {
      formatMessageBox("Could not open .in file '%s'\n",filePath);

      return false;
   }

   while (inFile.getNextCommandLine())
   {
      if      (inFile.isCommand(".AdjustLayerNames"            ,2))  inFile.parseYesNoParam(1,m_optionAdjustLayerNames            ,false);
      else if (inFile.isCommand(".LibraryMode"                 ,2))  inFile.parseYesNoParam(1,m_optionLibraryMode                 ,false);
      else if (inFile.isCommand(".AddSourceLineNumbers"        ,2))  inFile.parseYesNoParam(1,m_optionAddSourceLineNumbers        ,false);
      else if (inFile.isCommand(".AddPadstackDescriptor"       ,2))  inFile.parseYesNoParam(1,m_optionAddPadstackDescriptor       ,false);      
      else if (inFile.isCommand(".UseOldReader"                ,2))  inFile.parseYesNoParam(1,m_optionUseOldReader                ,false);
      else if (inFile.isCommand(".TestFtfRead"                 ,2))  inFile.parseYesNoParam(1,m_optionTestFtfRead                 ,false);
      else if (inFile.isCommand(".TestPcfRead"                 ,2))  inFile.parseYesNoParam(1,m_optionTestPcfRead                 ,false);
      else if (inFile.isCommand(".TestPnfRead"                 ,2))  inFile.parseYesNoParam(1,m_optionTestPnfRead                 ,false);
      else if (inFile.isCommand(".InstantiateConnectPads"      ,2))  inFile.parseYesNoParam(1,m_optionInstantiateConnectPads      ,true );
      else if (inFile.isCommand(".InstantiateNoconnectPads"    ,2))  inFile.parseYesNoParam(1,m_optionInstantiateNoconnectPads    ,false);
      else if (inFile.isCommand(".InstantiateThermalPads"      ,2))  inFile.parseYesNoParam(1,m_optionInstantiateThermalPads      ,false);
      else if (inFile.isCommand(".InstantiateClearancePads"    ,2))  inFile.parseYesNoParam(1,m_optionInstantiateClearancePads    ,false);
      else if (inFile.isCommand(".InstantiateFillets"          ,2))  inFile.parseYesNoParam(1,m_optionInstantiateFillets          ,true );
      else if (inFile.isCommand(".PurgeUnusedGeometries"       ,2))  inFile.parseYesNoParam(1,m_optionPurgeUnusedGeometries       ,true );
      else if (inFile.isCommand(".ShowPurgeReport"             ,2))  inFile.parseYesNoParam(1,m_optionShowPurgeReport             ,true );
      else if (inFile.isCommand(".UsePnfAsPcf"                 ,2))  inFile.parseYesNoParam(1,m_optionUsePnfAsPcf                 ,false);
      else if (inFile.isCommand(".ShowHiddenRefDes"            ,2))  inFile.parseYesNoParam(1,m_optionShowHiddenRefDes            ,true );
      else if (inFile.isCommand(".RestructurePadstacks"        ,2))  inFile.parseYesNoParam(1,m_optionRestructurePadstacks        ,true );
      else if (inFile.isCommand(".ConglomeratePadstacks"       ,2))  inFile.parseYesNoParam(1,m_optionConglomeratePadstacks       ,true );
      else if (inFile.isCommand(".SegregateGeneratedBondwires" ,2))  inFile.parseYesNoParam(1,m_optionSegregateGeneratedBondwires ,false);
      else if (inFile.isCommand(".SegregateBondwiresByDiameter",2))  inFile.parseYesNoParam(1,m_optionSegregateBondwiresByDiameter,false);
      else if (inFile.isCommand(".IgnoreOutOfBoard"            ,2))  inFile.parseYesNoParam(1,m_optionIgnoreOutOfBoard            ,false);
      else if (inFile.isCommand(".GeneratedLoadedAttribute"    ,2))  inFile.parseYesNoParam(1,m_optionGenerateLoadedAttribute     ,true );
      else if (inFile.isCommand(".DefaultVariantLoadedFlag"    ,2))  inFile.parseYesNoParam(1,m_optionDefaultVariantLoadedFlag    ,true );
      else if (inFile.isCommand(".GenerateMirroredLayers"      ,2))  inFile.parseYesNoParam(1,m_optionGenerateMirroredLayers      ,false);
      else if (inFile.isCommand(".EnableLayerMirroring"        ,2))  inFile.parseYesNoParam(1,m_optionEnableLayerMirroring        ,true );
      else if (inFile.isCommand(".MirrorElectricalLayers"      ,2))  inFile.parseYesNoParam(1,m_optionMirrorElectricalLayers      ,true );
      else if (inFile.isCommand(".EnablePanelFtfFile"          ,2))  inFile.parseYesNoParam(1,m_optionEnablePanelFtfFile          ,true );
      else if (inFile.isCommand(".GenerateDebugFile"           ,2))  inFile.parseYesNoParam(1,m_optionGenerateDebugFile           ,false);
      else if (inFile.isCommand(".GenerateLogFiles"            ,2))  inFile.parseYesNoParam(1,m_optionGenerateLogFile             ,true );
      else if (inFile.isCommand(".DisplayErrorMessages"        ,2))  inFile.parseYesNoParam(1,m_optionDisplayErrorMessages        ,false);      
      else if (inFile.isCommand(".SupportMultiByteCharacters"  ,2))  inFile.parseYesNoParam(1,m_optionSupportMultiByteCharacters  ,true );      
      else if (inFile.isCommand(".ConvertComplexApertures"     ,2))  inFile.parseYesNoParam(1,m_optionConvertComplexApertures     ,false );
      else if (inFile.isCommand(".EnableReverseFootprints"     ,2))  inFile.parseYesNoParam(1,m_optionEnableReverseFootprints     ,true );
      else if (inFile.isCommand(".UseOutlineWidth"             ,2))  inFile.parseYesNoParam(1,m_optionUseOutlineWidth             ,true );
      else if (inFile.isCommand(".LogFileDirectoryPath"        ,2))  setLogFileDirectoryPath(inFile.getParam(1));
      else if (inFile.isCommand(".MounterMachineName"          ,2))  m_optionMounterMachineName   = inFile.getParam(1);
      else if (inFile.isCommand(".MakeIntoFiducial"            ,2))  addFiducialExpression(   inFile.getParam(1));
      else if (inFile.isCommand(".MakeIntoTestPoint"           ,2))  addTestPointExpression(  inFile.getParam(1));
      else if (inFile.isCommand(".MakeIntoTooling"             ,2))  addToolingExpression(    inFile.getParam(1));
      else if (inFile.isCommand(".CompOutline"                 ,2))  addCompOutlineExpression(inFile.getParam(1));
      else if (inFile.isCommand(".TextHeightFactor"            ,2))  m_optionTextHeightFactor      = inFile.getDoubleParam(1);
      else if (inFile.isCommand(".TextWidthFactor"             ,2))  m_optionTextWidthFactor       = inFile.getDoubleParam(1);
      else if (inFile.isCommand(".AttributeHeightFactor"       ,2))  m_optionAttributeHeightFactor = inFile.getDoubleParam(1);
      else if (inFile.isCommand(".AttributeWidthFactor"        ,2))  m_optionAttributeWidthFactor  = inFile.getDoubleParam(1);
      else if (inFile.isCommand(".ConsolidateGeometry"         ,2))  GetConsolidatorControl().Parse( inFile.getCommandLineString() );
      else if (inFile.isCommand(".Material",3))
      {
         int materialNumber   = inFile.getIntParam(1);
         CString materialName = inFile.getParam(2);

         m_materials.setAt(materialNumber,materialName);
      }
      else if (inFile.isCommand(".MirrorLayer",3))
      {
         CString layerName1 = inFile.getParam(1);
         CString layerName2 = inFile.getParam(2);

         addMirroredLayer(inFile.getCommandLineString(),layerName1,layerName2);
      }
      else if (inFile.isCommand(".MirrorBaseLayer",3))
      {
         CString layerName1 = inFile.getParam(1);
         CString layerName2 = inFile.getParam(2);

         addMirroredBaseLayer(inFile.getCommandLineString(),layerName1,layerName2);
      }
      else if (inFile.isCommand(".SetLayerType",3) ||
               inFile.isCommand(".LayerAttr",3)     )
      {
         CString layerName = inFile.getParam(1).MakeLower();
         CString layerType = inFile.getParam(2);

         m_optionLayerTypes.SetAt(layerName,layerType);
      }
      else if (inFile.isCommand(".UndefinedPadstackSize",4))
      {
         CString unitsString   = inFile.getParam(1);
         double padDiameter    = inFile.getDoubleParam(2);
         double drillDiameter  = inFile.getDoubleParam(3);

         PageUnitsTag units = parseUnitsParameter(unitsString,inFile.getCommandLineString());

         if (units != pageUnitsUndefined)
         {
            m_optionDefaultViaPadDiameter   = getUnits().convertFrom(units,padDiameter);
            m_optionDefaultViaDrillDiameter = getUnits().convertFrom(units,drillDiameter);
         }
      }
      else if (inFile.isCommand(".ProbeSize",3))
      {
         double probeSize    = inFile.getDoubleParam(1);
         CString unitsString = inFile.getParam(2);

         PageUnitsTag units = parseUnitsParameter(unitsString,inFile.getCommandLineString());

         if (units != pageUnitsUndefined)
         {
            m_optionProbeDiameter   = getUnits().convertFrom(units,probeSize);
            m_optionProbeDescriptor = inFile.getParam(1) + " " + unitsString;
         }
      }
      else if (inFile.isCommand(".ProbeDrillSize",3))
      {
         double probeDrillSize = inFile.getDoubleParam(1);
         CString unitsString   = inFile.getParam(2);

         PageUnitsTag units = parseUnitsParameter(unitsString,inFile.getCommandLineString());

         if (units != pageUnitsUndefined)
         {
            m_optionProbeDrillDiameter = getUnits().convertFrom(units,probeDrillSize);
         }
      }
      else if (inFile.isCommand(".TestAccessSize",3))
      {
         double testAccessSize = inFile.getDoubleParam(1);
         CString unitsString   = inFile.getParam(2);

         PageUnitsTag units = parseUnitsParameter(unitsString,inFile.getCommandLineString());

         if (units != pageUnitsUndefined)
         {
            m_optionTestAccessDiameter = getUnits().convertFrom(units,testAccessSize);
         }
      }      
   }

   getLog().writef(PrefixStatus,"Reading .in file '%s'\n",filePath);

   return true;
}

PageUnitsTag CZukenCr5000Reader::parseUnitsParameter(CString unitsString,const CString& commandLineString)
{
   unitsString.MakeLower();
   PageUnitsTag unitsParameter = unitStringAbbreviationToTag(unitsString);

   if (unitsParameter == pageUnitsUndefined)
   {
      CString legalUnits;

      for (EnumIterator(PageUnitsTag,unitsIterator);unitsIterator.hasNext();)
      {
         PageUnitsTag unitsTag = unitsIterator.getNext();

         if (! legalUnits.IsEmpty())
         {
            legalUnits += ", ";
         }

         legalUnits += quoteString(unitStringAbbreviation(unitsTag));
      }

      getLog().writef(PrefixError,"Unrecognizable units string '%s' found in command '%s'; legal units are (%s)\n",unitsString,commandLineString,legalUnits);
   }

   return unitsParameter;
}

void CZukenCr5000Reader::addFiducialExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);
   regularExpression->setCaseSensitive(false);

   m_optionFiducialExpressionList.AddTail(regularExpression);
}

void CZukenCr5000Reader::addTestPointExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);
   regularExpression->setCaseSensitive(false);

   m_optionTestPointExpressionList.AddTail(regularExpression);
}

void CZukenCr5000Reader::addToolingExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);
   regularExpression->setCaseSensitive(false);

   m_optionToolingExpressionList.AddTail(regularExpression);
}

void CZukenCr5000Reader::addCompOutlineExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);
   regularExpression->setCaseSensitive(false);

   m_optionCompOutlineExpressionList.AddTail(regularExpression);
}

void CZukenCr5000Reader::addMirroredLayer(const CString& commandString,const CString& layerName1,const CString& layerName2)
{
   m_optionMirroredLayers.SetAt(layerName1,layerName2);
   m_mirroredLayers.addLayerEntry(commandString,layerName1,layerName2);

   //CString layerNameKey1 = layerName1;
   //layerNameKey1.MakeLower();

   //m_optionMirroredLayers.SetAt(layerNameKey1,layerName2);

   //CString layerNameKey2 = layerName2;
   //layerNameKey2.MakeLower();

   //m_optionMirroredLayers.SetAt(layerNameKey2,layerName1);
}

void CZukenCr5000Reader::addMirroredBaseLayer(const CString& commandString,const CString& layerName1,const CString& layerName2)
{
   m_mirroredLayers.addBaseLayerEntry(commandString,layerName1,layerName2);
}

CString CZukenCr5000Reader::getMirroredLayer(CString layerName) const
{
   CString mirroredLayerName;

   layerName.MakeLower();

   m_optionMirroredLayers.Lookup(layerName,mirroredLayerName);

   return mirroredLayerName;
}

CString CZukenCr5000Reader::adjustCase(const CString& string) const
{
   CString adjustedCaseString(string);
   bool alphaFlag = false;
   char* pp  = NULL;
   char* ppp = NULL;

   for (char* p = adjustedCaseString.GetBuffer();*p != '\0';p++)
   {
      if (isalpha(*p))
      {
         *p = toupper(*p);

         bool specialFlag = (ppp != NULL && *ppp == '_' && pp != NULL && (*pp == 'C' || *pp == 'S') &&
                             *p == 'S');

         if (alphaFlag && !specialFlag)
         {
            *p = tolower(*p);
         }
      }

      alphaFlag = (isalpha(*p) != 0);
      ppp = pp;
      pp  = p;
   }

   return adjustedCaseString;
}

//double CZukenCr5000Reader::convertCr5kUnitsToPageUnits(double cr5kUnits) const
//{
//   if (m_cr5kPageUnits == pageUnitsUndefined)
//   {
//      getLog().writef(PrefixError,"CR5000 units undefined");
//
//      m_cr5kPageUnits = pageUnitsCentiMicroMeters;
//   }
//
//   return m_units.convertFrom(m_cr5kPageUnits,cr5kUnits);
//}

//void CZukenCr5000Reader::updateCr5kUnits(Cr5kTokenTag token)
//{
//   PageUnitsTag portUnits = pageUnitsUndefined;
//
//   switch (token)
//   {
//   case tokMm:      portUnits = pageUnitsMilliMeters;       break;
//   case tokInch:    portUnits = pageUnitsInches;            break;
//   case tokMil:     portUnits = pageUnitsMils;              break;
//   case tokDbUnit:  portUnits = pageUnitsCentiMicroMeters;  break;
//   }
//
//   if (portUnits == pageUnitsUndefined)
//   {
//      getLog().writef(PrefixError,"Unrecognized unit token '%s' encountered.",CCr5kTokenReader::cr5kTokenToString(token));
//   }
//   else
//   {
//      m_cr5kPageUnits = portUnits;
//      m_units.setPortUnits(portUnits);
//   }
//}

LayerStruct& CZukenCr5000Reader::getDefinedLayer(const CString& layerName,int materialNumber,bool mirroredLayerFlag)
{
   LayerStruct& layer = m_layerLinkMaps.getDefinedLayer(*this,layerName,materialNumber,mirroredLayerFlag);

   return layer;
}

void CZukenCr5000Reader::mirrorLayers()
{
   const CLayerArray& layerArray = getCamCadDatabase().getCamCadDoc().getLayerArray();

   CString layerName1,layerName2;

   for (POSITION pos = m_optionMirroredLayers.GetStartPosition();pos != NULL;)
   {
      m_optionMirroredLayers.GetNextAssoc(pos,layerName1,layerName2);

      LayerStruct* layer1 = layerArray.getLayerNoCase(layerName1);
      LayerStruct* layer2 = layerArray.getLayerNoCase(layerName2);

      if (layer1 != NULL || layer2 != NULL)
      {
         if (layer1 == NULL)
         {
            if (getOptionAdjustLayerNames())
            {
               layerName1 = adjustCase(layerName1);
            }

            layer1 = getCamCadDatabase().getDefinedLayer(layerName1);
         }

         if (layer2 == NULL)
         {
            if (getOptionAdjustLayerNames())
            {
               layerName2 = adjustCase(layerName2);
            }

            layer2 = getCamCadDatabase().getDefinedLayer(layerName2);
         }

         layerArray.mirrorLayers(*layer1,*layer2);
      }
   }
}

void CZukenCr5000Reader::typeLayers()
{
   const CLayerArray& layerArray = getCamCadDatabase().getCamCadDoc().getLayerArray();

   CString layerName,layerTypeString;

   for (POSITION pos = m_optionLayerTypes.GetStartPosition();pos != NULL;)
   {
      m_optionLayerTypes.GetNextAssoc(pos,layerName,layerTypeString);

      LayerStruct* layer = layerArray.getLayerNoCase(layerName);
      LayerTypeTag layerType = stringToLayerTypeTag(layerTypeString);

      if (layer != NULL && layerType != layerTypeUndefined)
      {
         layer->setLayerType(layerType);
      }
   }
}

//LayerStruct& CZukenCr5000Reader::getDefinedLayer(CString layerName)
//{
//   LayerStruct* layer = getCamCadDatabase().getLayer(layerName);
//
//   if (layer == NULL)
//   {
//      if (getOptionAdjustLayerNames())
//      {
//         layerName = adjustCase(layerName);
//      }
//
//      layer = getCamCadDatabase().getDefinedLayer(layerName);
//      layerName.MakeLower();
//
//      CString layerTypeString;
//
//      if (m_optionLayerTypes.Lookup(layerName,layerTypeString))
//      {
//         LayerTypeTag layerType = stringToLayerTypeTag(layerTypeString);
//
//         if (layerType != layerTypeUndefined)
//         {
//            layer->setLayerType(layerType);
//         }
//      }
//
//      CString mirroredLayerName;
//
//      if (m_optionMirroredLayers.Lookup(layerName,mirroredLayerName))
//      {
//         if (getOptionAdjustLayerNames())
//         {
//            mirroredLayerName = adjustCase(mirroredLayerName);
//         }
//
//         LayerStruct* mirroredLayer = getCamCadDatabase().getDefinedLayer(mirroredLayerName);
//         mirroredLayerName.MakeLower();
//
//         getCamCadDatabase().mirrorLayers(layer->getLayerIndex(),mirroredLayer->getLayerIndex());
//
//         if (m_optionLayerTypes.Lookup(mirroredLayerName,layerTypeString))
//         {
//            LayerTypeTag layerType = stringToLayerTypeTag(layerTypeString);
//
//            if (layerType != layerTypeUndefined)
//            {
//               mirroredLayer->setLayerType(layerType);
//            }
//         }
//      }
//   }
//
//   return *layer;
//}

//_____________________________________________________________________________
const double CCr5kPolyListManager::m_cleanTolerance = .00001;

CCr5kPolyListManager::CCr5kPolyListManager(CCamCadDatabase& camCadDatabase,
   const CUnits& units, CPolyList& polyList, double width, bool squareWidthFlag, PolyTypeTag polyType, bool applyDoubleShrink)
: m_camCadDatabase(camCadDatabase)
, m_polyList(polyList)
, m_width(width)
, m_squareWidthFlag(squareWidthFlag)
, m_polyType(polyType)
, m_units(units)
, m_poly(NULL)
, m_polyWidth(0.)
, m_pointCount(0)
, m_polyCount(0)
, m_applyDoubleShrink(applyDoubleShrink)
{
   m_pointTolerance = getUnitsFactor(pageUnitsCentiMicroMeters,units.GetPortUnits());
}

void CCr5kPolyListManager::addArcVertex(const CCr5kArcVertex& arcVertex)
{
   double beginWidth   = arcVertex.getBeginWidth();
   double endWidth     = arcVertex.getEndWidth();
   double bulge        = arcVertex.getBulge();
   CPoint2d beginPoint = arcVertex.getBeginPoint();
   CPoint2d   endPoint = arcVertex.getEndPoint();

   if (beginPoint.fpnear(endPoint,m_pointTolerance))
   {
      CSize2d centerDelta = arcVertex.getDelta(); 
      CVector2d radius(centerDelta,beginPoint);

      radius.setLength(2.*arcVertex.getArcRadius());

      CPoint2d midPoint = radius.getTip();

      addPoint(beginPoint.x,beginPoint.y,1.,beginWidth);
      addPoint(  midPoint.x,  midPoint.y,1.,beginWidth);
      addPoint(beginPoint.x,beginPoint.y,0.,  endWidth);
   }
   else
   {
      addPoint(beginPoint.x,beginPoint.y,bulge,beginWidth);
      addPoint(  endPoint.x,  endPoint.y,   0.,  endWidth);
   }
}

void CCr5kPolyListManager::addTangentArc(const CPoint2d& beginPoint,const CCr5kPointVertex& pointVertex,const CPoint2d& endPoint)
{
   double width = m_polyWidth;
   
   if (pointVertex.hasWidth())
   {
      width = pointVertex.getWidth();
   }

   CPoint2d commonPoint = pointVertex.getPoint();
   CVector2d beginVector(commonPoint,beginPoint);
   CVector2d   endVector(commonPoint,  endPoint);

   CVector2d actualBeginVector(beginPoint,commonPoint);
   double crossProduct = actualBeginVector.crossProductMagnitude(endVector);

   if (fpeq(crossProduct,0.,1.))
   {
      // tangent lines are parallel
      addPoint(commonPoint.x,commonPoint.y,0.,width);
   }
   else
   {
      double bulgeSign = ((crossProduct < 0.) ? -1. : 1.);

      beginVector.setLength(1.);
      endVector.setLength(1.);

      CVector2d bisector = beginVector;
      bisector.setComponents(beginVector.getComponents() + endVector.getComponents());

      double cosHalfBeta = beginVector.dotProduct(bisector)/(beginVector.getLength() * bisector.getLength());
      double r = pointVertex.getTangentArcRadius();
      double tangentLengthSquared = (r*r/(1 - cosHalfBeta*cosHalfBeta)) - r*r;

      if (tangentLengthSquared < 0.)
      {
         formatMessageBoxApp("Logic error in '%s'",__FUNCTION__);

         addPoint( beginPoint.x, beginPoint.y,0.,width);
         addPoint(commonPoint.x,commonPoint.y,0.,width);
         addPoint(   endPoint.x,   endPoint.y,0.,width);
      }
      else
      {
         double halfBeta = acos(cosHalfBeta);
         double halfTheta = PiOver2 - halfBeta;
         double bulge = bulgeSign * abs(tan(halfTheta/2.));

         double tangentLength = sqrt(tangentLengthSquared);

         beginVector.setLength(tangentLength);
         endVector.setLength(tangentLength);

         CPoint2d beginTangentPoint = beginVector.getTip();
         CPoint2d   endTangentPoint =   endVector.getTip();

         splitPolyIfWidthChange(width);  // need to make sure the new width starts at the last poly point
         addPoint(beginTangentPoint.x,beginTangentPoint.y,bulge,width);
         addPoint(  endTangentPoint.x,  endTangentPoint.y,   0.,width);
      }
   }
}

void CCr5kPolyListManager::addPointVertex(const CCr5kPointVertex& pointVertex)
{
   double width   = pointVertex.getWidth();
   CPoint2d point = pointVertex.getPoint();

   addPoint(point.x,point.y,0.,width);
}

void CCr5kPolyListManager::splitPolyIfWidthChange(double width)
{
   if (m_width >= 0.)
   {
      width = m_width;
   }

   if (!fpeq(width,m_polyWidth))
   {
      if (m_poly != NULL && m_poly->getVertexCount() > 0)
      {
         m_poly->clean(m_cleanTolerance);

         CPnt* lastPnt = m_poly->getPntList().GetTail();

         m_poly = new CPoly();
         double widthInPageUnits = m_units.convertFromPortUnits(width);
         m_poly->setWidthIndex(getDefinedWidthIndex(widthInPageUnits));

         m_polyList.AddTail(m_poly);
         m_polyCount++;  // the poly list may have been empty when the CCr5kPolyListManager was constructed

         m_poly->addVertex(lastPnt->x,lastPnt->y);

         m_polyWidth = width;
      }
   }
}

void CCr5kPolyListManager::splitPoly()
{
   if (m_poly != NULL && m_poly->getVertexCount() > 1)
   {
      m_poly->clean(m_cleanTolerance);

      CPnt* lastPnt = m_poly->getPntList().GetTail();

      int widthIndex = m_poly->getWidthIndex();

      m_poly = new CPoly();
      m_poly->setWidthIndex(widthIndex);

      m_polyList.AddTail(m_poly);
      m_polyCount++;  // the poly list may have been empty when the CCr5kPolyListManager was constructed

      m_poly->addVertex(lastPnt->x,lastPnt->y);
   }
}

void CCr5kPolyListManager::addPoint(double x,double y,double bulge,double width)
{
   if (m_width >= 0.)
   {
      width = m_width;
   }

   if (m_pointCount++ == 0)
   {
      m_firstPoint.x = x;
      m_firstPoint.y = y;
   }

   double xInPageUnits = m_units.convertFromPortUnits(x);
   double yInPageUnits = m_units.convertFromPortUnits(y);

   if (!fpeq(width,m_polyWidth))
   {
      if (m_poly != NULL && m_poly->getVertexCount() > 0)
      {
         m_poly->addVertex(xInPageUnits,yInPageUnits);
         m_poly->clean(m_cleanTolerance);
      }

      m_poly = NULL;
   }

   if (m_poly == NULL)
   {
      m_poly = new CPoly();
      double widthInPageUnits = m_units.convertFromPortUnits(width);
      m_poly->setWidthIndex(getDefinedWidthIndex(widthInPageUnits));

      m_polyList.AddTail(m_poly);
      m_polyCount++;  // the poly list may have been empty when the CCr5kPolyListManager was constructed
   }

   m_poly->addVertex(xInPageUnits,yInPageUnits,bulge);
   m_polyWidth = width;
}

void CCr5kPolyListManager::addFillet(const CPoint2d& headPoint,const CPoint2d& tailPoint,bool straightTypeFlag,double pointWidth,double filletWidth,double filletLength)
{
   CPoint2d originPoint(   m_units.convertFromPortUnits(headPoint.x),m_units.convertFromPortUnits(headPoint.y));
   CPoint2d directionPoint(m_units.convertFromPortUnits(tailPoint.x),m_units.convertFromPortUnits(tailPoint.y));

   double L  = m_units.convertFromPortUnits(filletLength);
   double w  = m_units.convertFromPortUnits(filletWidth - pointWidth/2.);
   double pw = m_units.convertFromPortUnits(pointWidth);

   if (L > w && w > 0. && m_polyCount > 0)
   {
      if (straightTypeFlag)
      {
         double d = sqrt(L*L - w*w);
         double r = L*d/w;

         CVector2d vL(originPoint,directionPoint);
         vL.setLength(L);

         CVector2d vr(vL);
         vr.exchangeOriginAndTip();
         vr.makePerpendicular(false);
         vr.setLength(r);

         CVector2d vw0(vL.getOrigin(),vr.getTip());
         vw0.setLength(w);

         CPoint2d startPoint(vL.getTip());
         CPoint2d endPoint0(vw0.getTip());

         vr.reverse();
         
         CVector2d vw1(vL.getOrigin(),vr.getTip());
         vw1.setLength(w);
         CPoint2d endPoint1(vw1.getTip());

         double startAngle,deltaAngle,radius;

         ArcCenter2(endPoint0.x,endPoint0.y,endPoint1.x,endPoint1.y,originPoint.x,originPoint.y,&radius,&startAngle,&deltaAngle,false);

         double bulge1 = tan(deltaAngle/4.);

         CPoly* poly = new CPoly();

         poly->setWidthIndex(getDefinedWidthIndex(pw));

         poly->addVertex(startPoint.x,startPoint.y);
         poly->addVertex( endPoint0.x, endPoint0.y, bulge1);
         poly->addVertex( endPoint1.x, endPoint1.y);
         poly->addVertex(startPoint.x,startPoint.y);
         poly->close();
         poly->setFilled(true);

         splitPoly();

         POSITION pos = m_polyList.GetTailPosition();
         m_polyList.InsertBefore(pos,poly);
         m_polyCount++;
      }
      else
      {
         double r = (L*L - w*w)/(2. * w);

         CVector2d vL(originPoint,directionPoint);
         vL.setLength(L);

         CVector2d vr(vL);
         vr.exchangeOriginAndTip();
         vr.makePerpendicular(false);
         vr.setLength(r);

         CVector2d vw0(vL.getOrigin(),vr.getTip());
         vw0.setLength(w);

         CPoint2d startPoint(vL.getTip());
         CPoint2d endPoint0(vw0.getTip());
         CPoint2d centerPoint(vr.getTip());
         double startAngle,deltaAngle,radius;

         ArcCenter2(startPoint.x,startPoint.y,endPoint0.x,endPoint0.y,centerPoint.x,centerPoint.y,&radius,&startAngle,&deltaAngle,true);

         double bulge0 = tan(deltaAngle/4.);

         vr.reverse();
         
         CVector2d vw1(vL.getOrigin(),vr.getTip());
         vw1.setLength(w);
         CPoint2d endPoint1(vw1.getTip());

         ArcCenter2(endPoint0.x,endPoint0.y,endPoint1.x,endPoint1.y,originPoint.x,originPoint.y,&radius,&startAngle,&deltaAngle,false);

         double bulge1 = tan(deltaAngle/4.);

         CPoly* poly = new CPoly();

         poly->setWidthIndex(getDefinedWidthIndex(pw));

         poly->addVertex(startPoint.x,startPoint.y, bulge0);
         poly->addVertex( endPoint0.x, endPoint0.y, bulge1);
         poly->addVertex( endPoint1.x, endPoint1.y, bulge0);
         poly->addVertex(startPoint.x,startPoint.y);
         poly->close();
         poly->setFilled(true);

         splitPoly();

         POSITION pos = m_polyList.GetTailPosition();
         m_polyList.InsertBefore(pos,poly);
         m_polyCount++;
      }
   }
}

int CCr5kPolyListManager::getDefinedWidthIndex(double widthInPageUnits)
{
   int widthIndex = (m_squareWidthFlag ? m_camCadDatabase.getCamCadDoc().getDefinedSquareWidthIndex(widthInPageUnits) :
                                         m_camCadDatabase.getDefinedWidthIndex(widthInPageUnits));

   return widthIndex;
}

//void CCr5kPolyListManager::close()
//{
//   if (m_pointCount > 1)
//   {
//      if (m_polyCount == 1)
//      {
//         m_poly->close();
//      }
//      else
//      {
//         addPoint(m_firstPoint.x,m_firstPoint.y,0.,m_polyWidth);
//      }
//   }
//}
//
//void CCr5kPolyListManager::clean()
//{
//   if (m_poly != NULL)
//   {
//      m_poly->clean(m_cleanTolerance);
//   }
//}

void CCr5kPolyListManager::complete()
{
   if (m_poly != NULL)
   {
      bool closedFlag = false;
      bool filledFlag = false;
      bool voidFlag   = false;

      switch (m_polyType)
      {
      case polyTypeVoid:
         voidFlag = true;
         // fall thru intentional
      case polyTypeFilled:
         filledFlag = true;
         // fall thru intentional
      case polyTypeClosed:
         closedFlag = true;
         break;
      }

      m_poly->clean(m_cleanTolerance);

      if (closedFlag)
      {
         if (m_pointCount >= 1)
         {
            if (m_polyCount == 1)
            {
               m_poly->close();

               m_poly->setFilled(filledFlag);
               m_poly->setVoid(voidFlag);
            }
            else
            {
               addPoint(m_firstPoint.x,m_firstPoint.y,0.,m_polyWidth);
            }
         }
      }

      if (m_applyDoubleShrink && m_poly->getWidthIndex() >= 0 && m_poly->getVertexCount() >= 2)
      {
         double widthRadius = m_camCadDatabase.getCamCadDoc().getWidthTable()[m_poly->getWidthIndex()]->getSizeA() / 2;
         if(fabs(widthRadius) > SMALLNUMBER && m_poly->isClosed())
         {
            if(ShouldConsiderOutlineWidth())
            { 
#ifdef B4_DR698760
               m_poly->shrink(widthRadius,m_camCadDatabase.getCamCadDoc().getPageUnits());               
               m_poly->shrink(-widthRadius,m_camCadDatabase.getCamCadDoc().getPageUnits());
#else
               if (!m_poly->isVoid())
               {
                  // Shrink outer boundary to outer edge, really is an expand.
                  m_poly->shrink(widthRadius,m_camCadDatabase.getCamCadDoc().getPageUnits());               
                  m_poly->shrink(-widthRadius,m_camCadDatabase.getCamCadDoc().getPageUnits());
               }
               else
               {
                  // Shrink void boundaries to inner edge.
                  m_poly->shrink(-widthRadius,m_camCadDatabase.getCamCadDoc().getPageUnits());               
                  m_poly->shrink(widthRadius,m_camCadDatabase.getCamCadDoc().getPageUnits());
               }
#endif
            }
            m_poly->setWidthIndex(m_camCadDatabase.getCamCadDoc().getZeroWidthIndex());            
         }
      }
   }
}

bool CCr5kPolyListManager::ShouldConsiderOutlineWidth()
{
   // Note that init section assumes at least two points.
   // No need to process a "line" that has only one vertex.
   if (m_poly->getVertexCount() < 2)
      return false;

   double widthRadius = m_camCadDatabase.getCamCadDoc().getWidthTable()[m_poly->getWidthIndex()]->getSizeA() / 2;
   CPnt *pnt, *prev, *next;
   CPnt* firstPoint;
   POSITION pos = m_poly->getPntList().GetHeadPosition();
   POSITION pntPos = pos;
   pnt = m_poly->getPntList().GetNext(pos);
   firstPoint = pnt;
   POSITION nextPos = pos;
   next = m_poly->getPntList().GetNext(pos);   // THIS ASSUMES at least two points in poly
   bool atleastOneNonRoundedCorner = false;
   while (pos)
   {
      prev = pnt;
      pnt = next;
      pntPos = nextPos;
      nextPos = pos;
      next = m_poly->getPntList().GetNext(pos);

      if(fabs(prev->bulge) > SMALLNUMBER)
      {
         double sa, da, cx, cy, r;
         da = atan(prev->bulge) * 4;
         ArcPoint2Angle(prev->x, prev->y, pnt->x, pnt->y, da, &cx, &cy, &r, &sa);
         if(!((r - widthRadius > 0) || (fabs(r - widthRadius) <= SMALLNUMBER)))
            return false;
         continue;
      }

      if(fabs(pnt->bulge) > SMALLNUMBER)
      {
         double sa, da, cx, cy, r;
         da = atan(pnt->bulge) * 4;
         ArcPoint2Angle(pnt->x, pnt->y, next->x, next->y, da, &cx, &cy, &r, &sa);
         if(!((r - widthRadius > 0) || (fabs(r - widthRadius) <= SMALLNUMBER)))
            return false;
         continue;
      }

      if (!CPntInSeg(prev, next, pnt))      
         atleastOneNonRoundedCorner = true;
   }

   if(!CPntInSeg(pnt,firstPoint,next))
      atleastOneNonRoundedCorner = true;


   if(atleastOneNonRoundedCorner)
      return true;
   else
      return false;
}
//_____________________________________________________________________________
int CZukenCr5000Reader::getSourceLineNumberKeywordIndex()
{
   if (m_sourceLineNumberKeywordIndex < 0)
   {
      m_sourceLineNumberKeywordIndex = getCamCadDatabase().registerKeyword("SourceLineNumber",valueTypeString);
   }

   return m_sourceLineNumberKeywordIndex;
}

int CZukenCr5000Reader::getParentReferenceKeywordIndex()
{
   if (m_parentReferenceKeywordIndex < 0)
   {
      m_parentReferenceKeywordIndex = getCamCadDatabase().registerKeyword("ParentReference",valueTypeString);
   }

   return m_parentReferenceKeywordIndex;
}

void CZukenCr5000Reader::setSourceLineNumberAttribute(DataStruct& data,const CCr5kElement& element)
{
   if (getOptionAddSourceLineNumbers())
   {
      getCamCadDatabase().addAttribute(data.attributes(),getSourceLineNumberKeywordIndex(),element.getSourceLineDescriptor());   
   }
}

void CZukenCr5000Reader::setSourceLineNumberAttribute(BlockStruct& geometry,const CCr5kElement& element)
{
   if (getOptionAddSourceLineNumbers())
   {
      getCamCadDatabase().addAttribute(geometry.attributes(),getSourceLineNumberKeywordIndex(),element.getSourceLineDescriptor());   
   }
}

void CZukenCr5000Reader::setParentReferenceAttribute(DataStruct& data,const CString& parentReference)
{
   if (! parentReference.IsEmpty())
   {
      getCamCadDatabase().addAttribute(data.attributes(),getParentReferenceKeywordIndex(),parentReference); 
   }
}

void CZukenCr5000Reader::setDeviceTypeAttribute(BlockStruct& geometry,const CString& deviceType)
{
   int deviceTypeKeywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDeviceType);

   getCamCadDatabase().addAttribute(geometry.attributes(),deviceTypeKeywordIndex,deviceType);   
}

void CZukenCr5000Reader::generatePolyListFromVertices(CPolyList& polyList,const CCr5kVertices& vertices,double width,bool squareWidthFlag,PolyTypeTag polyType)
{
   //if (vertices.getSourceLineNumber() == 574525)
   //{
   //   int iii = 3;
   //}

   // I think the application of the incredibly time consuming double shrink algorithm for non-zero width boundaries ought
   // to be decoupled from the width value determination. But Mark wants only one control. We have had cases where not
   // all the geom objects are participating in the getOutlineWidth lookup that uses this control to really lookup the
   // value versus just return 0.0. So here I am using the same setting from .in file to explicitly set a flag that
   // says whether to apply the double shrink process or not. So even if something ends up coming through here with
   // non-zero width, if the .UseOutlineWidth is N, then we will still skip the double shrink.
   bool applyDoubleShrink = this->getOptionUseOutlineWidth();

   CCr5kPolyListManager polyListManager(getCamCadDatabase(), vertices.getUnits(), polyList, width, squareWidthFlag, polyType, applyDoubleShrink);
   int vertexCount = vertices.getCount();

   if (vertexCount > 0)
   {
      CCr5kVertex* headVertex = vertices.getHead();
      CCr5kVertex* tailVertex = vertices.getTail();

      if (vertexCount == 1)
      {
         if (headVertex->isLine())
         {
            formatErrorMessage("Line in one vertex list in '%s', source line '%s'",__FUNCTION__,vertices.getSourceLineDescriptor());
         }
         else
         {
            polyListManager.addArcVertex(*(headVertex->getArcVertex()));
            polyListManager.complete();
         }
      }
      else if (vertexCount == 2 &&
                 (headVertex->isTangentArc() || tailVertex->isTangentArc()) &&
                !(headVertex->isArc()        || tailVertex->isArc())           )
      {
         formatErrorMessage("Tangent arcs in two vertex list in '%s',\nsource line '%s'",__FUNCTION__,vertices.getSourceLineDescriptor());
      }
      else if (vertexCount > 1)
      {
         //if (vertexCount == 2 && (headVertex->isTangentArc() || tailVertex->isTangentArc()) )
         //{
         //   // for debug
         //   int iii = 3;
         //}

         POSITION pos = vertices.getHeadPosition();
         
         CCr5kVertex* vertex         = vertices.getNext(pos);
         CCr5kVertex* nextVertex     = vertices.getNext(pos);

         CCr5kVertex* firstVertex    = vertex;
         CCr5kVertex* previousVertex = tailVertex;

         while (true)
         {
            bool lastVertexFlag = (nextVertex == firstVertex);

            if (vertex->isArc())
            {
               CCr5kArcVertex* arcVertex = vertex->getArcVertex();
               polyListManager.addArcVertex(*arcVertex);
               if (getOptionInstantiateFillets() && 
                  (arcVertex->getBeginArcPoint().hasFillet() || arcVertex->getEndArcPoint().hasFillet()))
               {
                  const CCr5kFillet& fillet = arcVertex->getBeginArcPoint().getFillet();

                  double filletWidth  = fillet.getFilletWidth();
                  double filletLength = fillet.getFilletLength();
                  double pointWidth   = fillet.getPointWidth();
                  bool   straightTypeFlag = fillet.hasStraightType();

                  CPoint2d headPoint;
                  if(arcVertex->getBeginArcPoint().hasFillet())
                     headPoint = arcVertex->getBeginPoint();
                  else
                     headPoint = arcVertex->getEndPoint();
                  CPoint2d tailPoint;

                  if (fillet.hasForwardDirection() && !lastVertexFlag)
                  {
                     tailPoint = nextVertex->getBeginPoint();
                  }
                  else
                  {
                     tailPoint = previousVertex->getEndPoint();
                  }

                  polyListManager.addFillet(headPoint,tailPoint,straightTypeFlag,pointWidth,filletWidth,filletLength);
               }
            }
            else
            {
               CCr5kPointVertex* pointVertex = vertex->getPointVertex();

               if (pointVertex->isTangentArc())
               {
                  polyListManager.addTangentArc(previousVertex->getEndPoint(),*pointVertex,nextVertex->getBeginPoint());
               }
               else
               {
                  polyListManager.addPointVertex(*pointVertex);
               }

               if (getOptionInstantiateFillets() && pointVertex->hasFillet())
               {
                  const CCr5kFillet& fillet = pointVertex->getFillet();

                  double filletWidth  = fillet.getFilletWidth();
                  double filletLength = fillet.getFilletLength();
                  double pointWidth   = fillet.getPointWidth();
                  bool   straightTypeFlag = fillet.hasStraightType();

                  CPoint2d headPoint = pointVertex->getPoint();
                  CPoint2d tailPoint;

                  if (fillet.hasForwardDirection() && !lastVertexFlag)
                  {
                     tailPoint = nextVertex->getBeginPoint();
                  }
                  else
                  {
                     tailPoint = previousVertex->getEndPoint();
                  }

                  polyListManager.addFillet(headPoint,tailPoint,straightTypeFlag,pointWidth,filletWidth,filletLength);
               }
            }

            previousVertex = vertex;
            vertex = nextVertex;

            if (nextVertex == firstVertex)
            {
               break;
            }
            else if (pos == NULL)
            {
               nextVertex = firstVertex;
            }
            else
            {
               nextVertex = vertices.getNext(pos);
            }
         }

         polyListManager.complete();
      }
   }
}

void CZukenCr5000Reader::formatErrorMessage(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString message;
   message.FormatV(format,args);

   getLog().writef("%s\n",message);

   if (getOptionDisplayErrorMessages())
   {
      getMessageFilter().formatMessageBoxApp(message);
   }
}

void CZukenCr5000Reader::instantiateText(CCr5kMappedLayerDataList& mappedLayerDataList,const CString& layerName,int materialNumber,
   const CCr5kGeometry& geometry,const CString& parentReference,const CTMatrix* matrix,const CCr5kProperties* properties)
{
   CSupString textString = geometry.getTextString();
   CPoint2d ccPoint      = geometry.getPoint().getPoint();
   double charHeight     = geometry.getTextCharHeightInPageUnits() * getOptionTextHeightFactor();
   double charWidth      = geometry.getTextCharWidthInPageUnits()  * getOptionTextWidthFactor();
   double strokeWidth    = geometry.getTextStrokeWidthInPageUnits();
   double degrees        = geometry.getTextAngleDegrees();
   Cr5kTokenTag justification = geometry.getTextJustification();
   Cr5kTokenTag flip     = geometry.getTextFlip();


   if (! textString.IsEmpty())
   {
      textString.Replace("\r\n","\n");
      CStringArray textLines;
      int lineCount = textString.ParseQuote(textLines,"\n");
      int maxLineLength = 0;

      for (int lineIndex = 0;lineIndex < lineCount;lineIndex++)
      {
         CString line = textLines.GetAt(lineIndex);
         maxLineLength = max(line.GetLength(),maxLineLength);
      }

      int justificationFlags = 0;
      HorizontalPositionTag horizontalPosition;
      VerticalPositionTag verticalPosition;
      switch (justification)
      {
      case tokLo_C:
         {
            justificationFlags = GRTEXT_H_B | GRTEXT_W_C;
            horizontalPosition = horizontalPositionCenter;
            verticalPosition = verticalPositionBottom;
         }
         break;
      case tokLo_L:
         {
            justificationFlags = GRTEXT_H_B | GRTEXT_W_L;
            horizontalPosition = horizontalPositionLeft;
            verticalPosition = verticalPositionBottom;
         }
         break;
      case tokLo_R:
         {
            justificationFlags = GRTEXT_H_B | GRTEXT_W_R;
            horizontalPosition = horizontalPositionRight;
            verticalPosition = verticalPositionBottom;
         }
         break;
      case tokCe_C:
         {
            justificationFlags = GRTEXT_H_C | GRTEXT_W_C;
            horizontalPosition = horizontalPositionCenter;
            verticalPosition = verticalPositionCenter;
         }
         break;
      case tokCe_L:
         {
            justificationFlags = GRTEXT_H_C | GRTEXT_W_L;
            horizontalPosition = horizontalPositionLeft;
            verticalPosition = verticalPositionCenter;
         }
         break;
      case tokCe_R: 
         {
            justificationFlags = GRTEXT_H_C | GRTEXT_W_R;
            horizontalPosition = horizontalPositionRight;
            verticalPosition = verticalPositionCenter;         
         }
         break;
      case tokUp_C:
         {
            justificationFlags = GRTEXT_H_T | GRTEXT_W_C;
            horizontalPosition = horizontalPositionCenter;
            verticalPosition = verticalPositionTop;
         }
         break;
      case tokUp_L:
         {
            justificationFlags = GRTEXT_H_T | GRTEXT_W_L;
            horizontalPosition = horizontalPositionLeft;
            verticalPosition = verticalPositionTop;         
         }
         break;
      case tokUp_R:
         {
            justificationFlags = GRTEXT_H_T | GRTEXT_W_R;
            horizontalPosition = horizontalPositionRight;
            verticalPosition = verticalPositionTop;                  
         }
         break;
      default:
         {
            justificationFlags = GRTEXT_H_C | GRTEXT_W_C;
            horizontalPosition = horizontalPositionCenter;
            verticalPosition = verticalPositionCenter;
         }
         break;
      }

      bool mirrorFlag = false;
      if(flip == tokX)
      {
         mirrorFlag = true;
      }
      else if(flip == tokXY)
      {
         mirrorFlag = false;
         degrees = normalizeDegrees(degrees - 180);
      }
      else if(flip == tokY)
      {
         mirrorFlag = true;
         degrees = normalizeDegrees(degrees - 180);
      }

      double spaceratio = m_camCadDoc.getSettings().getTextSpaceRatio();
      double multiplicationFactor = 1/(1 + spaceratio);
      charHeight *= multiplicationFactor;
      charWidth *= multiplicationFactor;
      
      int penWidthIndex = getCamCadDatabase().getDefinedWidthIndex(strokeWidth);
      
      double textRectHeight = charHeight * 1.2 * lineCount;
      double textRectWidth  = charWidth * .8 * maxLineLength;
      double x = ccPoint.x;
      double y = ccPoint.y;

      if(lineCount > 1)
      {
         normalize_text(&x,&y,justificationFlags,degrees,mirrorFlag,textRectHeight,textRectWidth);
      }

      double xOff = 0.;
      double yOff = 0.;

      Rotate(0.,charHeight,degrees, &xOff, &yOff);

      x += lineCount * xOff;// + (xOff*0.2);   // start top to bottom 
      y += lineCount * yOff;// + (yOff*0.2);
      if(lineCount > 1)
      {
         x += (xOff*0.2);
         y += (yOff*0.2);
      }

      for (int lineIndex = 0;lineIndex < lineCount;lineIndex++)
      {
         CString line = textLines.GetAt(lineIndex);

         x -= xOff;
         y -= yOff;

         DataStruct* textData = new_DataStruct(dataTypeText);

         TextStruct* textStruct = textData->getText();

         textStruct->setText(line);
         textStruct->setPnt(x,y);
         textStruct->setHeight(charHeight);
         textStruct->setWidth(charWidth);
         textStruct->setRotationDegrees(degrees);
         textStruct->setProportionalSpacing(true);
         textStruct->setMirrored(mirrorFlag);
         textStruct->setPenWidthIndex(penWidthIndex);
         textStruct->setHorizontalPosition(horizontalPosition);
         textStruct->setVerticalPosition(verticalPosition);         
         

         if (matrix != NULL)
         {
            textData->transform(*matrix);
         }

         setParentReferenceAttribute(*textData,parentReference);

         if (properties != NULL)
         {
            setProperties(textData->attributes(),*properties);
         }

         mappedLayerDataList.addTail(textData,layerName,materialNumber);
      }
   }
}

BlockStruct* CZukenCr5000Reader::getInstantiatedStandardPadGeometry(const CCr5kGeometry& padGeometry,CBasesVector& apertureOffsetAndRotation)
{
   apertureOffsetAndRotation.set();
   BlockStruct* standardPadGeometry = getInstantiatedSimplePadGeometry(padGeometry,apertureOffsetAndRotation);

   if (standardPadGeometry == NULL)
   {
      PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();
      int descriptorDimensionFactorExponent = (isEnglish(getCamCadDatabase().getPageUnits()) ? 0 : 2);

      Cr5kTokenTag geometryType = padGeometry.getType();

      switch (geometryType)
      {
      case ptokRectangle:
         {
            double outlineWidth = padGeometry.getOutlineWidthInPageUnits();

            if (outlineWidth > 0.)
            {
               CPoint2d offset = padGeometry.getPoint().getPoint();
               double width    = padGeometry.getWidthInPageUnits();
               double height   = padGeometry.getHeightInPageUnits();
               double cornerRadius = outlineWidth/2.;

               CStandardAperture standardAperture(standardApertureRoundedRectangle,getCamCadDatabase().getPageUnits(),descriptorDimensionFactorExponent);
               standardAperture.setDimensions(width,height,cornerRadius);
               CString apertureName = "SA_" + standardAperture.getDescriptor();

               standardPadGeometry = getCamCadDatabase().getBlock(apertureName);

               if (standardPadGeometry == NULL)
               {
                  standardPadGeometry = standardAperture.createNewAperture(getCamCadData_Database(),apertureName);
               }

               apertureOffsetAndRotation.set(offset.x,offset.y);

               setSourceLineNumberAttribute(*standardPadGeometry,padGeometry);
            }
         }

         break;
      case ptokSurface:
         // possible analysis of surface to attempt reduction to standard shape could be placed here
         break;
      }
   }

   return standardPadGeometry;
}

BlockStruct* CZukenCr5000Reader::getInstantiatedSimplePadGeometry(const CCr5kGeometry& padGeometry,CBasesVector& apertureOffsetAndRotation)
{
   apertureOffsetAndRotation.set();
   BlockStruct* simplePadGeometry = NULL;

   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();
   int descriptorDimensionFactorExponent = (isEnglish(pageUnits) ? 0 : 2);

   Cr5kTokenTag geometryType = padGeometry.getType();

   switch (geometryType)
   {
   case ptokCircle:
      {
         CPoint2d offset = padGeometry.getPoint().getPoint();
         double   radius = padGeometry.getRadiusInPageUnits();

         CStandardApertureCircle standardAperture(pageUnits,descriptorDimensionFactorExponent);
         standardAperture.setDiameter(radius * 2.);
         apertureOffsetAndRotation.set(offset.x,offset.y);

         CString apertureName = "AP_" + standardAperture.getDescriptor();

         simplePadGeometry = getCamCadDatabase().getBlock(apertureName);

         if (simplePadGeometry == NULL)
         {
            simplePadGeometry = getCamCadDatabase().getDefinedAperture(apertureName,
                                   apertureRound,
                                   standardAperture.getDimension(0));

            setSourceLineNumberAttribute(*simplePadGeometry,padGeometry);
         }
      }

      break;
   case ptokDonut:
      {
         CPoint2d    offset = padGeometry.getPoint().getPoint();
         double innerRadius = padGeometry.getInnerRadiusInPageUnits();
         double outerRadius = padGeometry.getOuterRadiusInPageUnits();

         CStandardApertureRoundDonut standardAperture(pageUnits,descriptorDimensionFactorExponent);
         standardAperture.setInnerDiameter(innerRadius * 2.);
         standardAperture.setOuterDiameter(outerRadius * 2.);
         apertureOffsetAndRotation.set(offset.x,offset.y);

         CString apertureName = "AP_" + standardAperture.getDescriptor();

         simplePadGeometry = getCamCadDatabase().getBlock(apertureName);

         if (simplePadGeometry == NULL)
         {
            simplePadGeometry = getCamCadDatabase().getDefinedAperture(apertureName,
                                   apertureDonut,
                                   standardAperture.getDimension(0),standardAperture.getDimension(1));

            setSourceLineNumberAttribute(*simplePadGeometry,padGeometry);
         }
      }

      break;
   case ptokOval:
   case ptokOblong:
      {
         double height,angleDegrees;
         CPoint2d offset;

         double width  = padGeometry.getWidthInPageUnits();

         if (geometryType == ptokOval)
         {
            height = padGeometry.getHeightInPageUnits();

            angleDegrees = padGeometry.getOvalAngleDegrees() - 90.;
            offset       = padGeometry.getPoint().getPoint();
         }
         else
         {
            CPoint2d point1 = padGeometry.getPoint().getPoint();
            CPoint2d point2 = padGeometry.getPoint2().getPoint();
            CVector2d medialAxis(point1,point2);

            offset = medialAxis.getCenter();
            height = medialAxis.getLength() + width;
            angleDegrees = radiansToDegrees(medialAxis.getTheta()) - 90.;
         }

         CStandardApertureOval standardAperture(pageUnits,descriptorDimensionFactorExponent);
         standardAperture.setWidth(width);
         standardAperture.setHeight(height);
         apertureOffsetAndRotation.set(offset.x,offset.y,angleDegrees);

         CString apertureName = "AP_" + standardAperture.getDescriptor();

         simplePadGeometry = getCamCadDatabase().getBlock(apertureName);

         if (simplePadGeometry == NULL)
         {
            simplePadGeometry = getCamCadDatabase().getDefinedAperture(apertureName,
                                   apertureOblong,
                                   standardAperture.getDimension(0),standardAperture.getDimension(1));

            setSourceLineNumberAttribute(*simplePadGeometry,padGeometry);
         }
      }

      break;
   case ptokRectangle:
      {
         double outlineWidth = padGeometry.getOutlineWidthInPageUnits();

         if (outlineWidth == 0.)
         {
            CPoint2d offset = padGeometry.getPoint().getPoint();
            double width    = padGeometry.getWidthInPageUnits();
            double height   = padGeometry.getHeightInPageUnits();

            CStandardApertureRectangle standardAperture(pageUnits,descriptorDimensionFactorExponent);
            standardAperture.setWidth(width);
            standardAperture.setHeight(height);
            apertureOffsetAndRotation.set(offset.x,offset.y);

            CString apertureName = "AP_" + standardAperture.getDescriptor();

            //if (apertureName.CompareNoCase("AP_rectangle900x910m2") == 0)
            //{
            //   int iii = 3;
            //}

            simplePadGeometry = getCamCadDatabase().getBlock(apertureName);

            if (simplePadGeometry == NULL)
            {
               simplePadGeometry = getCamCadDatabase().getDefinedAperture(apertureName,
                                    apertureRectangle,
                                    standardAperture.getDimension(0),standardAperture.getDimension(1));

               setSourceLineNumberAttribute(*simplePadGeometry,padGeometry);
            }
         }
      }

      break;
   case ptokSquareThermal: // cheating, just making it round, at least it doesn't vanish altogether
   case ptokRoundThermal:  // what we really define here
      {
         CPoint2d    offset = padGeometry.getPoint().getPoint();
         double innerRadius = padGeometry.getInnerRadiusInPageUnits();
         double outerRadius = padGeometry.getOuterRadiusInPageUnits();
         double spokeAngle  = padGeometry.getBridgeAngle();
         double spokeWidth  = padGeometry.getBridgeWidthInPageUnits();
         int    spokeCount  = padGeometry.getBridgeCount();

         CStandardApertureRoundThermalSquare standardAperture(pageUnits,descriptorDimensionFactorExponent);
         standardAperture.setInnerDiameter(innerRadius * 2.);
         standardAperture.setOuterDiameter(outerRadius * 2.);
         standardAperture.setSpokeCount(spokeCount);
         standardAperture.setSpokeGap(spokeWidth);
         standardAperture.setSpokeAngleDegrees(spokeAngle);

         apertureOffsetAndRotation.set(offset.x,offset.y);

         CString apertureName = "AP_" + standardAperture.getDescriptor();

         simplePadGeometry = getCamCadDatabase().getBlock(apertureName);

         if (simplePadGeometry == NULL)
         {
            simplePadGeometry = getCamCadDatabase().getDefinedAperture(apertureName,
                                   apertureThermal,
                                   standardAperture.getDimension(1),standardAperture.getDimension(0));

            setSourceLineNumberAttribute(*simplePadGeometry,padGeometry);
         }
      }


      break;
   }

   return simplePadGeometry;
}

BlockStruct* CZukenCr5000Reader::getInstantiatedPadGeometry(const CCr5kGeometries& cr5kPadGeometries, const CString& padName, CBasesVector& originalApertureOffsetAndRotation)
{
   // This instantiates from a Cr5kGeometry definition (may have come from FTF or PCF).

   BlockStruct *padGeometry = NULL;

   CBasesVector apertureOffsetAndRotation = originalApertureOffsetAndRotation;

   bool simplePadFlag = (cr5kPadGeometries.getCount() == 1);

   if (simplePadFlag)
   {
      CCr5kGeometry* cr5kPadGeometry = cr5kPadGeometries.getHead();

      padGeometry = getInstantiatedStandardPadGeometry(*cr5kPadGeometry, apertureOffsetAndRotation);

      // DR 769579 
      // If instantiating standard pad geom worked then the apertureOffsetAndRotation has
      // been calculated to match. We need to pass this back to the caller.
      if (padGeometry != NULL)
         originalApertureOffsetAndRotation = apertureOffsetAndRotation;
   }

   if (padGeometry == NULL)
   {
      BlockStruct* complexApertureGeometry = getCamCadDatabase().getNewBlock(padName,"_%d-Cmplx",blockTypeUnknown,getCadFileNumber());
      padGeometry = getCamCadDatabase().getNewBlock(padName,"_%d",blockTypeUnknown,getCadFileNumber());
      padGeometry->setComplexApertureSubBlockNumber(complexApertureGeometry->getBlockNumber());
      padGeometry->setShape(apertureComplex);
      padGeometry->setFlags(BL_APERTURE | BL_GLOBAL);
      getCamCadDatabase().getDefinedWidthIndex(*complexApertureGeometry);

      for (POSITION geometryPos = cr5kPadGeometries.getHeadPosition(); geometryPos != NULL; )
      {
         CCr5kGeometry* cr5kPadGeometry = cr5kPadGeometries.getNext(geometryPos);

         apertureOffsetAndRotation = originalApertureOffsetAndRotation;
         BlockStruct* subPadGeometry = getInstantiatedStandardPadGeometry(*cr5kPadGeometry, apertureOffsetAndRotation);  // !!! this call will clear apertureOffsetAndRotation

         if (subPadGeometry == NULL)
         {
            CCr5kMappedLayerDataList mappedLayerDataList(*this,complexApertureGeometry->getDataList());

            // DR 748268 - The original transform goes the wrong direction, need to invert it
            // to get what we need to instantiate geom from here. This is because it will be inserted
            // with the original offset, so we want to shift it to be relative to that pt, not
            // shift it yet another insert point away.
            // DR 743879 - Same exact issue.
            CTMatrix anotherMatrix = originalApertureOffsetAndRotation.getTransformationMatrix();
            anotherMatrix.invert();

            instantiateGeometryData( mappedLayerDataList, getCamCadDatabase().getLayer(ccLayerFloat)->getName(), -1,
               *cr5kPadGeometry, "", graphicClassNormal, &anotherMatrix);
         }
         else
         {
            DataStruct* padData = getCamCadDatabase().insertBlock(subPadGeometry,insertTypeUnknown,"",getCamCadDatabase().getFloatingLayerIndex());
            CTMatrix matrix = apertureOffsetAndRotation.getTransformationMatrix();
            padData->transform(matrix);

            // The transform immediately above perhaps needs same treatment as given to the one in "if"
            // further above, for DR 748268. But that data is not coming through here. So this was left
            // as-is, not sure if it is okay but without test data I'm not going to alter it now.

            complexApertureGeometry->getDataList().AddTail(padData);

            setSourceLineNumberAttribute(*padData,*cr5kPadGeometry);
         }
      }

      // Reset for complex pad geometry - not clear if this ought to reset the one in
      // arg list or not. So far, this is working as-is. Need a test case to arrive that shows
      // it not working. Until then, it stays as-is.
      apertureOffsetAndRotation.set();
   }

   return padGeometry;
}

BlockStruct* CZukenCr5000Reader::getInstantiatedPadGeometry(CCr5kLinkedGeometries& linkedGeometries, const CString& padName, CBasesVector& apertureOffsetAndRotation)
{
   // This instantiates from footprint (FTF) file, the linkedGeometries are data from the FTF file.

   apertureOffsetAndRotation.set();
   BlockStruct* padGeometry = NULL;

   CCr5kLinkedPad* linkedPad = linkedGeometries.getLinkedPad(padName);

   if (linkedPad != NULL)
   {
      padGeometry               = linkedPad->getGeometry(true);
      apertureOffsetAndRotation = linkedPad->getOffsetAndRotation(true);

      if (padGeometry == NULL)
      {
         const CCr5kPad&                 cr5kPad  = linkedPad->getElement();
         const CCr5kGeometries& cr5kPadGeometries = cr5kPad.getGeometries();

         padGeometry = getInstantiatedPadGeometry(cr5kPadGeometries, padName, apertureOffsetAndRotation);
         
         setProperties(padGeometry->attributes(),cr5kPad.getProperties());
         linkedPad->setGeometry(true,padGeometry);
         linkedPad->setOffsetAndRotation(true,apertureOffsetAndRotation);
      }
   }

   return padGeometry;
}

BlockStruct* CZukenCr5000Reader::getInstantiatedPadstackGeometry(const CCr5kLayoutPrimitive& primitive,  CCr5kLinkedGeometries& linkedGeometries,
                                             const CString& pinRef, const CString& padstackName, bool placedTopFlag, int fromLayerNumber,int toLayerNumber,
                                             const CCr5kConductivePadStackPad* conductivePadStackPad)
{
   // Creates pin-specific padstack with pads from PCF file (which override pads from FTF).

   BlockStruct* padstackGeometry = NULL;

   // This gets the FTF padstack. We use that for settings and properties that are
   // there and not in the PCF. But later the geometry will be based on PCF.
   CCr5kLinkedPadstack* linkedPadstack = linkedGeometries.getLinkedPadstack(padstackName);

   if (linkedPadstack != NULL)
   {
      bool fromToFlag = (fromLayerNumber > 0 && toLayerNumber > 0);
      CString padstackGeometryName = getPadstackGeometryPrefix() + padstackName + ":" + pinRef + (placedTopFlag ? "" : m_cr5kBottomSuffix);

      if (fromToFlag || conductivePadStackPad != NULL)
      {
         if (fromToFlag)
         {
            padstackGeometryName.AppendFormat(":%d-%d",fromLayerNumber,toLayerNumber);
         }

         if (conductivePadStackPad != NULL)
         {
            padstackGeometryName.AppendFormat(":%s",conductivePadStackPad->getDescriptor());
         }

         padstackGeometry = getCamCadDatabase().getBlock(padstackGeometryName, getCadFileNumber());

      }
      else
      {
         //Never want this one, it is the shared one:  padstackGeometry = linkedPadstack->getGeometry(placedTopFlag);
      }

      if (padstackGeometry == NULL)
      {
         const CCr5kPadstack&      cr5kPadstack  = linkedPadstack->getElement();
         const CCr5kPadstackHoles& padstackHoles = cr5kPadstack.getPadstackHoles();
         const CCr5kPadsets&       padsets       = cr5kPadstack.getPadsets();

         padstackGeometry = getCamCadDatabase().getNewBlock(padstackGeometryName,"-%d",blockTypePadstack,getCadFileNumber());

         setProperties(padstackGeometry->attributes(), cr5kPadstack.getProperties());

         bool thruFlag = cr5kPadstack.isThruHole();
         CCr5kMappedLayerDataList mappedLayerDataList(*this, padstackGeometry->getDataList(), getLayerMapping());
         mappedLayerDataList.setTopFlag(placedTopFlag);
         mappedLayerDataList.setThruFlag(thruFlag);

         for (POSITION holePos = padstackHoles.GetHeadPosition();holePos != NULL;)
         {
            CCr5kPadstackHole* padstackHole = padstackHoles.GetNext(holePos);

            const CCr5kSingleGeometry& singleGeometry = padstackHole->getSingleGeometry();
            const CCr5kGeometry& holeGeometry = singleGeometry.getGeometry();

            if (holeGeometry.getType() == ptokCircle)
            {
               CPoint2d origin = holeGeometry.getPoint().getPoint();
               double radius   = holeGeometry.getRadiusInPageUnits();

               BlockStruct* toolGeometry = getCamCadDatabase().getDefinedTool(radius * 2.);

               // may want to add tool to a specific layer:material
               CString layerName  = padstackHole->getLayerName();
               int materialNumber = padstackHole->getMaterialNumber();
               LayerStruct& layer = getDefinedLayer(layerName,materialNumber);

               DataStruct* toolData = getCamCadDatabase().insertBlock(toolGeometry,insertTypeDrillTool,"",layer.getLayerIndex(),origin.x,origin.y);
               padstackGeometry->getDataList().AddTail(toolData);

               setSourceLineNumberAttribute(*toolData,*padstackHole);
            }
         }

         if (conductivePadStackPad == NULL)
         {
            // This area of pattern not used in this incarnation, but saved in case new test case proves this side is needed.
#ifdef POSTERITY
            for (POSITION padsetPos = padsets.GetHeadPosition();padsetPos != NULL;)
            {
               CCr5kPadset* padset = padsets.GetNext(padsetPos);

               // layer info
               int materialNumber = padset->getMaterialNumber();
               CString layerName  = padset->getLayerName();

               // pad info
               for (int padIndex = 0;padIndex < 4;padIndex++)
               {
                  CString padName;
                  Cr5kTokenTag padStatus = tokUndefined;

                  switch (padIndex)
                  {
                  case 0:  padName = (getOptionInstantiateConnectPads()   ? padset->getConnectPadName()   : "");  padStatus = tokConnect;    break;
                  case 1:  padName = (getOptionInstantiateNoconnectPads() ? padset->getNoconnectPadName() : "");  padStatus = tokNoconnect;  break;
                  case 2:  padName = (getOptionInstantiateThermalPads()   ? padset->getThermalPadName()   : "");  padStatus = tokThermal;    break;
                  case 3:  padName = (getOptionInstantiateClearancePads() ? padset->getClearancePadName() : "");  padStatus = tokClearance;  break;
                  }

                  if (! padName.IsEmpty())
                  {
                     CBasesVector apertureOffsetAndRotation;
                     BlockStruct* padGeometry = getInstantiatedPadGeometry(linkedGeometries,padName,apertureOffsetAndRotation);
                     DataStruct* padData = getCamCadDatabase().insertBlock(padGeometry,insertTypeUnknown,"",0,
                                             apertureOffsetAndRotation.getOrigin().x,apertureOffsetAndRotation.getOrigin().y,apertureOffsetAndRotation.getRotationRadians());

                     setSourceLineNumberAttribute(*padData,*padset);

                     mappedLayerDataList.addTail(padData,layerName,materialNumber,fromLayerNumber,toLayerNumber,conductivePadStackPad,padStatus);
                  }
               }
            }
#endif
         }
         else
         {
            for (POSITION conductiveLayerPos = conductivePadStackPad->getHeadPosition();conductiveLayerPos != NULL;)
            {
               CCr5kConductivePadStackPadLayer* conductivePadStackLayer = conductivePadStackPad->getNext(conductiveLayerPos);
               int padStackLayerNumber     = conductivePadStackLayer->getLayerNumber();
               Cr5kTokenTag padStackStatus = conductivePadStackLayer->getStatus();

               /*
               Pass 1 - Determine if a conductive pad was found
               __________________________________________________
               |            | pad layer  |          !            |
               |            |     is     |          !            |
               |            | associated | padstack !            |
               |            |    with    |  status  !            |
               | pad layer  | padstack   | matches  !            |
               |     is     | conductive |   pad    ! conductive |
               | conductive |   layer    |  status  ! pad found  |
               |------------+------------+----------+------------|
               |     n      |      n     |     n    !     n      |
               |     n      |      n     |     y    !     n      |
               |     n      |      y     |     n    !     n      |
               |     n      |      y     |     y    !     n      |
               |     y      |      n     |     n    !     n      |
               |     y      |      n     |     y    !     n      |
               |     y      |      y     |     n    !     n      |
               |     y      |      y     |     y    !     y      |
               |____________|____________|__________!____________|
               */

               bool conductivePadFoundFlag = false;

               for (POSITION padsetPos = padsets.GetHeadPosition();padsetPos != NULL && !conductivePadFoundFlag;)
               {
                  CCr5kPadset* padset = padsets.GetNext(padsetPos);

                  //if (padset->getSourceLineNumber() == 4045)
                  //{
                  //   int iii = 3;
                  //}

                  CString padName;

                  switch (padStackStatus)
                  {
                  case tokConnect:    padName = (getOptionInstantiateConnectPads()   ? padset->getConnectPadName()   : "");  break;
                  case tokNoconnect:  padName = (getOptionInstantiateNoconnectPads() ? padset->getNoconnectPadName() : "");  break;
                  case tokThermal:    padName = (getOptionInstantiateThermalPads()   ? padset->getThermalPadName()   : "");  break;
                  case tokClearance:  padName = (getOptionInstantiateClearancePads() ? padset->getClearancePadName() : "");  break;
                  }

                  // layer info
                  int materialNumber    = padset->getMaterialNumber();
                  CString footLayerName = padset->getLayerName();

                  if (! padName.IsEmpty())
                  {
                     const CStringList& boardLayers = m_layerMapping.getMappedBoardLayers(thruFlag,placedTopFlag,footLayerName);

                     for (POSITION pos = boardLayers.GetHeadPosition();pos != NULL;)
                     {
                        CString boardLayerName = boardLayers.GetNext(pos);

                        bool padLayerConductiveFlag = isConductiveLayer(boardLayerName);

                        if (padLayerConductiveFlag)
                        {
                           bool padLayerAssociatedFlag = isLayerInConductiveRange(boardLayerName,padStackLayerNumber,padStackLayerNumber);

                           if (padLayerAssociatedFlag)
                           {
                              conductivePadFoundFlag = true;

                              break;
                           }
                        }
                     }
                  }
               }

               /*
               Pass 2 - Determine which pads populate layers
               ________________________________________________________________
               |            |            | pad layer  |          !            |
               |            |            |     is     |          !            |
               |            |            | associated | padstack !            |
               |            |            |    with    |  status  !    pad     |
               | conductive | pad layer  | padstack   | matches  ! inserted   |
               |    pad     |     is     | conductive |   pad    !  on pad    |
               | was found  | conductive |   layer    |  status  !   layer    |
               |------------+------------+------------+----------+------------|
               |     n      |     n      |      n     |     n    !     y      |
               |     n      |     n      |      n     |     y    !     y      |
               |     n      |     n      |      y     |     n    !     n      |
               |     n      |     n      |      y     |     y    !     n      |
               |     n      |     y      |      n     |     n    !     n      |
               |     n      |     y      |      n     |     y    !     n      |
               |     n      |     y      |      y     |     n    !     n      |
               |     n      |     y      |      y     |     y    !    n/a     |
               |     y      |     n      |      n     |     n    !     y      |
               |     y      |     n      |      n     |     y    !     y      |
               |     y      |     n      |      y     |     n    !     y      |
               |     y      |     n      |      y     |     y    !     y      |
               |     y      |     y      |      n     |     n    !     n      |
               |     y      |     y      |      n     |     y    !     n      |
               |     y      |     y      |      y     |     n    !     n      |
               |     y      |     y      |      y     |     y    !     y      |
               |____________|____________|____________|__________!____________|

               */
               for (POSITION padsetPos = padsets.GetHeadPosition();padsetPos != NULL;)
               {
                  CCr5kPadset* padset = padsets.GetNext(padsetPos);

                  // layer info
                  int materialNumber    = padset->getMaterialNumber();
                  CString footLayerName = padset->getLayerName();

                  // pad info
                  for (int padIndex = 0;padIndex < 4;padIndex++)
                  {
                     CString padName;
                     Cr5kTokenTag padStatus = tokUndefined;

                     switch (padIndex)
                     {
                     case 0:  padName = (getOptionInstantiateConnectPads()   ? padset->getConnectPadName()   : "");  padStatus = tokConnect;    break;
                     case 1:  padName = (getOptionInstantiateNoconnectPads() ? padset->getNoconnectPadName() : "");  padStatus = tokNoconnect;  break;
                     case 2:  padName = (getOptionInstantiateThermalPads()   ? padset->getThermalPadName()   : "");  padStatus = tokThermal;    break;
                     case 3:  padName = (getOptionInstantiateClearancePads() ? padset->getClearancePadName() : "");  padStatus = tokClearance;  break;
                     }

                     if (! padName.IsEmpty())
                     {
                        padName += ":" + pinRef;

                        switch (padIndex)
                        {
                        case 0:  
                           {
                              const CCr5kConductivePadStackPad &condPdstkPad = primitive.getConductivePadStackPad();
                              POSITION pos = condPdstkPad.getHeadPosition();
                              while (pos != NULL)
                              {
                                 CCr5kConductivePadStackPadLayer *cpsPadLayer = condPdstkPad.getNext(pos);
                                 if (cpsPadLayer != NULL)
                                 {
                                    const CCr5kGeometries &cr5kPadGeometries = cpsPadLayer->getGeometries();
                                    if (cr5kPadGeometries.getCount() > 0)
                                    {
                                       CString boardLayerName;
                                       boardLayerName.Format("%d", cpsPadLayer->getLayerNumber());
                                       int layerIndex = getDefinedLayer(boardLayerName,materialNumber).getLayerIndex();
                                       CBasesVector apertureOffsetAndRotation;
                                       apertureOffsetAndRotation.set();

                                       // These are PCF based pads, and geom is in board coords, unlike FTF based
                                       // pads which are in their own comp-origin coords. So we need this transform to get
                                       // the pad geom relative to its own origin, not board origin.
                                       const CCr5kPoint &pnt = primitive.getPoint();
                                       CPoint2d pnt2d = pnt.getPoint();
                                       CBasesVector padOffset(pnt2d.x, pnt2d.y, -primitive.getAngleDegrees());

                                       BlockStruct* padGeometry = getInstantiatedPadGeometry(cr5kPadGeometries, padName, padOffset);

                                       DataStruct* padData = getCamCadDatabase().insertBlock(padGeometry, insertTypeUnknown, "", 0,
                                          apertureOffsetAndRotation.getOrigin().x,apertureOffsetAndRotation.getOrigin().y, apertureOffsetAndRotation.getRotationRadians());

                                       mappedLayerDataList.addTailWithoutLayerMapping(padData, layerIndex);

                                    }
                                 }
                              }
                           }
                           break;
                        case 1:
                           {
                              const CCr5kNonConductivePadStackPad &ncondPdstkPad = primitive.getNonConductivePadStackPad();
                              POSITION pos = ncondPdstkPad.getHeadPosition();
                              while (pos != NULL)
                              {
                                 CCr5kNonConductivePadStackPadLayer *ncpsPadLayer = ncondPdstkPad.getNext(pos);
                                 if (ncpsPadLayer != NULL)
                                 {
                                    const CCr5kGeometries &cr5kPadGeometries = ncpsPadLayer->getGeometries();
                                    if (cr5kPadGeometries.getCount() > 0)
                                    {
                                       CString boardLayerName(ncpsPadLayer->getLayerName());
                                       int layerIndex = getDefinedLayer(boardLayerName,materialNumber).getLayerIndex();
                                       CBasesVector apertureOffsetAndRotation;
                                       apertureOffsetAndRotation.set();

                                       // These are PCF based pads, and geom is in board coords, unlike FTF based
                                       // pads with are in their own coords. So we need this transform to get
                                       // the pad geom relative to its own origin, not board origin.
                                       const CCr5kPoint &pnt = primitive.getPoint();
                                       CPoint2d pnt2d = pnt.getPoint();
                                       CBasesVector padOffset(pnt2d.x, pnt2d.y, -primitive.getAngleDegrees());

                                       BlockStruct* padGeometry = getInstantiatedPadGeometry(cr5kPadGeometries, padName, padOffset);

                                       DataStruct* padData = getCamCadDatabase().insertBlock(padGeometry, insertTypeUnknown, "", 0,
                                          apertureOffsetAndRotation.getOrigin().x,apertureOffsetAndRotation.getOrigin().y, apertureOffsetAndRotation.getRotationRadians());

                                       mappedLayerDataList.addTailWithoutLayerMapping(padData, layerIndex);

                                    }
                                 }
                              }
                           }
                           break;
                        case 2:
                           // no-op
                           break;
                        case 3:
                           // no-op
                           break;
                        }    
                     }
                  }
               }
            }
         }
      }
   }

   return padstackGeometry;
}

BlockStruct* CZukenCr5000Reader::getInstantiatedPadstackGeometry(CCr5kLinkedGeometries& linkedGeometries,const CString& padstackName,
   bool placedTopFlag,int fromLayerNumber,int toLayerNumber,const CCr5kConductivePadStackPad* conductivePadStackPad)
{
   BlockStruct* padstackGeometry = NULL;

   //if (padstackName.Find("REF-KZF-LTCC40") >= 0)
   //{
   //   int iii = 3;
   //}


   CCr5kLinkedPadstack* linkedPadstack = linkedGeometries.getLinkedPadstack(padstackName);

   if (linkedPadstack != NULL)
   {
      bool fromToFlag = (fromLayerNumber > 0 && toLayerNumber > 0);
      CString padstackGeometryName = getPadstackGeometryPrefix() + padstackName + (placedTopFlag ? "" : m_cr5kBottomSuffix);

      if (fromToFlag || conductivePadStackPad != NULL)
      {
         if (fromToFlag)
         {
            padstackGeometryName.AppendFormat(":%d-%d",fromLayerNumber,toLayerNumber);
         }

         if (conductivePadStackPad != NULL)
         {
            padstackGeometryName.AppendFormat(":%s",conductivePadStackPad->getDescriptor());
         }

         padstackGeometry = getCamCadDatabase().getBlock(padstackGeometryName,getCadFileNumber());

      }
      else
      {
         padstackGeometry = linkedPadstack->getGeometry(placedTopFlag);
      }

      if (padstackGeometry == NULL)
      {
         const CCr5kPadstack&      cr5kPadstack  = linkedPadstack->getElement();
         const CCr5kPadstackHoles& padstackHoles = cr5kPadstack.getPadstackHoles();
         const CCr5kPadsets&       padsets       = cr5kPadstack.getPadsets();

         padstackGeometry = getCamCadDatabase().getNewBlock(padstackGeometryName,"-%d",blockTypePadstack,getCadFileNumber());

         if (!fromToFlag && conductivePadStackPad == NULL)
         {
            linkedPadstack->setGeometry(placedTopFlag,padstackGeometry);
         }

         setProperties(padstackGeometry->attributes(),cr5kPadstack.getProperties());

         bool thruFlag = cr5kPadstack.isThruHole();
         CCr5kMappedLayerDataList mappedLayerDataList(*this,padstackGeometry->getDataList(),getLayerMapping());
         mappedLayerDataList.setTopFlag(placedTopFlag);
         mappedLayerDataList.setThruFlag(thruFlag);

         for (POSITION holePos = padstackHoles.GetHeadPosition();holePos != NULL;)
         {
            CCr5kPadstackHole* padstackHole = padstackHoles.GetNext(holePos);

            const CCr5kSingleGeometry& singleGeometry = padstackHole->getSingleGeometry();
            const CCr5kGeometry& holeGeometry = singleGeometry.getGeometry();

            if (holeGeometry.getType() == ptokCircle)
            {
               CPoint2d origin = holeGeometry.getPoint().getPoint();
               double radius   = holeGeometry.getRadiusInPageUnits();

               BlockStruct* toolGeometry = getCamCadDatabase().getDefinedTool(radius * 2.);

               // may want to add tool to a specific layer:material
               CString layerName  = padstackHole->getLayerName();
               int materialNumber = padstackHole->getMaterialNumber();
               LayerStruct& layer = getDefinedLayer(layerName,materialNumber);

               DataStruct* toolData = getCamCadDatabase().insertBlock(toolGeometry,insertTypeDrillTool,"",layer.getLayerIndex(),origin.x,origin.y);
               padstackGeometry->getDataList().AddTail(toolData);

               setSourceLineNumberAttribute(*toolData,*padstackHole);
            }
         }

         if (conductivePadStackPad == NULL)
         {
            for (POSITION padsetPos = padsets.GetHeadPosition();padsetPos != NULL;)
            {
               CCr5kPadset* padset = padsets.GetNext(padsetPos);

               // layer info
               int materialNumber = padset->getMaterialNumber();
               CString layerName  = padset->getLayerName();

               // pad info
               for (int padIndex = 0;padIndex < 4;padIndex++)
               {
                  CString padName;
                  Cr5kTokenTag padStatus = tokUndefined;

                  switch (padIndex)
                  {
                  case 0:  padName = (getOptionInstantiateConnectPads()   ? padset->getConnectPadName()   : "");  padStatus = tokConnect;    break;
                  case 1:  padName = (getOptionInstantiateNoconnectPads() ? padset->getNoconnectPadName() : "");  padStatus = tokNoconnect;  break;
                  case 2:  padName = (getOptionInstantiateThermalPads()   ? padset->getThermalPadName()   : "");  padStatus = tokThermal;    break;
                  case 3:  padName = (getOptionInstantiateClearancePads() ? padset->getClearancePadName() : "");  padStatus = tokClearance;  break;
                  }

                  if (! padName.IsEmpty())
                  {
                     CBasesVector apertureOffsetAndRotation;
                     BlockStruct* padGeometry = getInstantiatedPadGeometry(linkedGeometries,padName,apertureOffsetAndRotation);
                     DataStruct* padData = getCamCadDatabase().insertBlock(padGeometry,insertTypeUnknown,"",0,
                                             apertureOffsetAndRotation.getOrigin().x,apertureOffsetAndRotation.getOrigin().y,apertureOffsetAndRotation.getRotationRadians());

                     setSourceLineNumberAttribute(*padData,*padset);

                     mappedLayerDataList.addTail(padData,layerName,materialNumber,fromLayerNumber,toLayerNumber,conductivePadStackPad,padStatus);
                  }
               }
            }
         }
         else
         {
            for (POSITION conductiveLayerPos = conductivePadStackPad->getHeadPosition();conductiveLayerPos != NULL;)
            {
               CCr5kConductivePadStackPadLayer* conductivePadStackLayer = conductivePadStackPad->getNext(conductiveLayerPos);
               int padStackLayerNumber     = conductivePadStackLayer->getLayerNumber();
               Cr5kTokenTag padStackStatus = conductivePadStackLayer->getStatus();

               /*
               Pass 1 - Determine if a conductive pad was found
               __________________________________________________
               |            | pad layer  |          !            |
               |            |     is     |          !            |
               |            | associated | padstack !            |
               |            |    with    |  status  !            |
               | pad layer  | padstack   | matches  !            |
               |     is     | conductive |   pad    ! conductive |
               | conductive |   layer    |  status  ! pad found  |
               |------------+------------+----------+------------|
               |     n      |      n     |     n    !     n      |
               |     n      |      n     |     y    !     n      |
               |     n      |      y     |     n    !     n      |
               |     n      |      y     |     y    !     n      |
               |     y      |      n     |     n    !     n      |
               |     y      |      n     |     y    !     n      |
               |     y      |      y     |     n    !     n      |
               |     y      |      y     |     y    !     y      |
               |____________|____________|__________!____________|
               */

               bool conductivePadFoundFlag = false;

               for (POSITION padsetPos = padsets.GetHeadPosition();padsetPos != NULL && !conductivePadFoundFlag;)
               {
                  CCr5kPadset* padset = padsets.GetNext(padsetPos);

                  //if (padset->getSourceLineNumber() == 4045)
                  //{
                  //   int iii = 3;
                  //}

                  CString padName;

                  switch (padStackStatus)
                  {
                  case tokConnect:    padName = (getOptionInstantiateConnectPads()   ? padset->getConnectPadName()   : "");  break;
                  case tokNoconnect:  padName = (getOptionInstantiateNoconnectPads() ? padset->getNoconnectPadName() : "");  break;
                  case tokThermal:    padName = (getOptionInstantiateThermalPads()   ? padset->getThermalPadName()   : "");  break;
                  case tokClearance:  padName = (getOptionInstantiateClearancePads() ? padset->getClearancePadName() : "");  break;
                  }

                  // layer info
                  int materialNumber    = padset->getMaterialNumber();
                  CString footLayerName = padset->getLayerName();

                  if (! padName.IsEmpty())
                  {
                     const CStringList& boardLayers = m_layerMapping.getMappedBoardLayers(thruFlag,placedTopFlag,footLayerName);

                     for (POSITION pos = boardLayers.GetHeadPosition();pos != NULL;)
                     {
                        CString boardLayerName = boardLayers.GetNext(pos);

                        bool padLayerConductiveFlag = isConductiveLayer(boardLayerName);

                        if (padLayerConductiveFlag)
                        {
                           bool padLayerAssociatedFlag = isLayerInConductiveRange(boardLayerName,padStackLayerNumber,padStackLayerNumber);

                           if (padLayerAssociatedFlag)
                           {
                              conductivePadFoundFlag = true;

                              break;
                           }
                        }
                     }
                  }
               }

               /*
               Pass 2 - Determine which pads populate layers
               ________________________________________________________________
               |            |            | pad layer  |          !            |
               |            |            |     is     |          !            |
               |            |            | associated | padstack !            |
               |            |            |    with    |  status  !    pad     |
               | conductive | pad layer  | padstack   | matches  ! inserted   |
               |    pad     |     is     | conductive |   pad    !  on pad    |
               | was found  | conductive |   layer    |  status  !   layer    |
               |------------+------------+------------+----------+------------|
               |     n      |     n      |      n     |     n    !     y      |
               |     n      |     n      |      n     |     y    !     y      |
               |     n      |     n      |      y     |     n    !     n      |
               |     n      |     n      |      y     |     y    !     n      |
               |     n      |     y      |      n     |     n    !     n      |
               |     n      |     y      |      n     |     y    !     n      |
               |     n      |     y      |      y     |     n    !     n      |
               |     n      |     y      |      y     |     y    !    n/a     |
               |     y      |     n      |      n     |     n    !     y      |
               |     y      |     n      |      n     |     y    !     y      |
               |     y      |     n      |      y     |     n    !     y      |
               |     y      |     n      |      y     |     y    !     y      |
               |     y      |     y      |      n     |     n    !     n      |
               |     y      |     y      |      n     |     y    !     n      |
               |     y      |     y      |      y     |     n    !     n      |
               |     y      |     y      |      y     |     y    !     y      |
               |____________|____________|____________|__________!____________|

               */
               for (POSITION padsetPos = padsets.GetHeadPosition();padsetPos != NULL;)
               {
                  CCr5kPadset* padset = padsets.GetNext(padsetPos);

                  // layer info
                  int materialNumber    = padset->getMaterialNumber();
                  CString footLayerName = padset->getLayerName();

                  // pad info
                  for (int padIndex = 0;padIndex < 4;padIndex++)
                  {
                     CString padName;
                     Cr5kTokenTag padStatus = tokUndefined;

                     switch (padIndex)
                     {
                     case 0:  padName = (getOptionInstantiateConnectPads()   ? padset->getConnectPadName()   : "");  padStatus = tokConnect;    break;
                     case 1:  padName = (getOptionInstantiateNoconnectPads() ? padset->getNoconnectPadName() : "");  padStatus = tokNoconnect;  break;
                     case 2:  padName = (getOptionInstantiateThermalPads()   ? padset->getThermalPadName()   : "");  padStatus = tokThermal;    break;
                     case 3:  padName = (getOptionInstantiateClearancePads() ? padset->getClearancePadName() : "");  padStatus = tokClearance;  break;
                     }

                     if (! padName.IsEmpty())
                     {
                        bool padStatusMatchFlag = (padStatus == padStackStatus);

                        const CStringList& boardLayers = m_layerMapping.getMappedBoardLayers(thruFlag,placedTopFlag,footLayerName);

                        for (POSITION pos = boardLayers.GetHeadPosition();pos != NULL;)
                        {
                           CString boardLayerName = boardLayers.GetNext(pos);

                           bool padLayerConductiveFlag = isConductiveLayer(boardLayerName);
                           bool padLayerAssociatedFlag = isLayerInConductiveRange(boardLayerName,padStackLayerNumber,padStackLayerNumber);

                           bool insertPadFlag = (!padLayerConductiveFlag && !padLayerAssociatedFlag) ||
                                                (conductivePadFoundFlag  && !padLayerConductiveFlag) ||
                                                (padLayerConductiveFlag  && padLayerAssociatedFlag && padStatusMatchFlag);

                           if (insertPadFlag)
                           {
                              int layerIndex = getDefinedLayer(boardLayerName,materialNumber).getLayerIndex();

                              CBasesVector apertureOffsetAndRotation;
                              BlockStruct* padGeometry = getInstantiatedPadGeometry(linkedGeometries,padName,apertureOffsetAndRotation);
                              DataStruct* padData = getCamCadDatabase().insertBlock(padGeometry,insertTypeUnknown,"",0,
                                                      apertureOffsetAndRotation.getOrigin().x,apertureOffsetAndRotation.getOrigin().y,apertureOffsetAndRotation.getRotationRadians());

                              setSourceLineNumberAttribute(*padData,*padset);

                              mappedLayerDataList.addTailWithoutLayerMapping(padData,layerIndex);
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return padstackGeometry;
}

bool CZukenCr5000Reader::isLayerConductiveOrInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const
{
   bool retval = isConductiveLayer(layerName) || isLayerInConductiveRange(layerName,fromLayerIndex,toLayerIndex);

   return retval;
}

bool CZukenCr5000Reader::isLayerNonConductiveOrInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const
{
   bool retval = !isConductiveLayer(layerName) || isLayerInConductiveRange(layerName,fromLayerIndex,toLayerIndex);

   return retval;
}

bool CZukenCr5000Reader::isLayerInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const
{
   bool retval = m_layerLinkMaps.isLayerInConductiveRange(layerName,fromLayerIndex,toLayerIndex);

   return retval;
}

bool CZukenCr5000Reader::isConductiveLayer(const CString& layerName) const
{
   bool retval = m_layerLinkMaps.isConductiveLayer(layerName);

   return retval;
}

bool CZukenCr5000Reader::isConductiveLayerAssociate(const CString& layerName) const
{
   bool retval = m_layerLinkMaps.isConductiveLayerAssociate(layerName);

   return retval;
}

void CZukenCr5000Reader::normalizeFromToLayerNumbers(int& fromLayerNumber,int& toLayerNumber) const
{
   if (fromLayerNumber > toLayerNumber)
   {
      int tmp = fromLayerNumber;
      fromLayerNumber = toLayerNumber;
      toLayerNumber   = tmp;
   }

   if (fromLayerNumber < 1)
   {
      fromLayerNumber = toLayerNumber = 0;
   }
   else
   {
      int maximumConductiveLayerNumber = m_layerLinkMaps.getMaximumConductiveLayerNumber();

      if (toLayerNumber > maximumConductiveLayerNumber)
      {
         toLayerNumber = maximumConductiveLayerNumber;

         if (fromLayerNumber > maximumConductiveLayerNumber)
         {
            fromLayerNumber = maximumConductiveLayerNumber;
         }
      }
   }
}

void CZukenCr5000Reader::normalizeLayerNumberSpan(int& fromLayerNumber,int& toLayerNumber) const
{
   normalizeFromToLayerNumbers(fromLayerNumber,toLayerNumber);

   int maximumConductiveLayerNumber = m_layerLinkMaps.getMaximumConductiveLayerNumber();

   if (fromLayerNumber == 1 && toLayerNumber == maximumConductiveLayerNumber)
   {
      fromLayerNumber = toLayerNumber = 0;
   }
}

BlockStruct* CZukenCr5000Reader::getInstantiatedDefaultViaGeometry(int fromLayerNumber,int toLayerNumber)
{
   CString geometryName;
   geometryName.Format("PS_DefaultVia%d-%d",fromLayerNumber, toLayerNumber);

   BlockStruct* viaGeometry = getCamCadDatabase().getBlock(geometryName,getCadFileNumber());

   if (viaGeometry == NULL)
   {
      viaGeometry = getCamCadDatabase().getNewBlock(geometryName,blockTypePadstack,getCadFileNumber());

      PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();
      int descriptorDimensionFactorExponent = (isEnglish(pageUnits) ? 0 : 2);

      CStandardApertureCircle standardAperture(pageUnits,descriptorDimensionFactorExponent);
      standardAperture.setDiameter(getOptionDefaultViaPadDiameter());

      CString apertureName = "AP_" + standardAperture.getDescriptor();

      BlockStruct* roundPadGeometry = getCamCadDatabase().getBlock(apertureName);

      if (roundPadGeometry == NULL)
      {
         roundPadGeometry = getCamCadDatabase().getDefinedAperture(apertureName,
                                 apertureRound,
                                 standardAperture.getDimension(0));
      }

      for (int layerNumber = fromLayerNumber;layerNumber <= toLayerNumber;layerNumber++)
      {
         CString layerName;
         layerName.Format("%d",layerNumber);

         LayerStruct& layer = getDefinedLayer(layerName);

         DataStruct* padData = getCamCadDatabase().insertBlock(roundPadGeometry,insertTypeUnknown,"",layer.getLayerIndex());

         viaGeometry->getDataList().AddTail(padData);
      }

      BlockStruct* drillGeometry = getCamCadDatabase().getDefinedTool(m_optionDefaultViaDrillDiameter);
      LayerStruct* drillLayer    = getCamCadDatabase().getLayer(ccLayerDrillHoles);

      DataStruct* toolData = getCamCadDatabase().insertBlock(drillGeometry,insertTypeDrillTool,"",drillLayer->getLayerIndex());

      viaGeometry->getDataList().AddTail(toolData);
   }

   return viaGeometry;
}

void CZukenCr5000Reader::instantiateGeometryAttributes(CAttributes& attributes,Cr5kTokenTag attributeType,bool visibleFlag,bool mirroredLayerFlag,
   const CString& layerName,int materialNumber,const CCr5kGeometry& geometry,const CTMatrix* matrix)
{
   if (geometry.getType() == ptokText)
   {
      CString textString = geometry.getTextString();

      if (! textString.IsEmpty())
      {
         CPoint2d ccPoint      = geometry.getPoint().getPoint();
         double charHeight     = geometry.getTextCharHeightInPageUnits() * getOptionAttributeHeightFactor();
         double charWidth      = geometry.getTextCharWidthInPageUnits()  * getOptionAttributeWidthFactor();
         double strokeWidth    = geometry.getTextStrokeWidthInPageUnits();
         double degrees        = geometry.getTextAngleDegrees();
         Cr5kTokenTag justification = geometry.getTextJustification();
         Cr5kTokenTag flip     = geometry.getTextFlip();

         StandardAttributeTag attributeTag;

         switch (attributeType)
         {
         case tokReference:  
            attributeTag = standardAttributeRefName;   

            if (getOptionShowHiddenRefDes())
            {
               visibleFlag = true;
            }

            break;
         case tokPartName:   
            attributeTag = standardAttributePartNumber; 
            visibleFlag = true;
            break;
         default:            
            attributeTag = standardAttributeMiscellaneous;  
            visibleFlag = false;
            break;
         }

         int keywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(attributeTag);

         HorizontalPositionTag horizontalPosition;
         VerticalPositionTag   verticalPosition;

         switch (justification)
         {
         case tokLo_C:  verticalPosition = verticalPositionBottom;  horizontalPosition = horizontalPositionCenter;  break;
         case tokLo_L:  verticalPosition = verticalPositionBottom;  horizontalPosition = horizontalPositionLeft;    break;
         case tokLo_R:  verticalPosition = verticalPositionBottom;  horizontalPosition = horizontalPositionRight;   break;
         case tokCe_C:  verticalPosition = verticalPositionCenter;  horizontalPosition = horizontalPositionCenter;  break;
         case tokCe_L:  verticalPosition = verticalPositionCenter;  horizontalPosition = horizontalPositionLeft;    break;
         case tokCe_R:  verticalPosition = verticalPositionCenter;  horizontalPosition = horizontalPositionRight;   break;
         default:
         case tokUp_C:  verticalPosition = verticalPositionTop;     horizontalPosition = horizontalPositionCenter;  break;
         case tokUp_L:  verticalPosition = verticalPositionTop;     horizontalPosition = horizontalPositionLeft;    break;
         case tokUp_R:  verticalPosition = verticalPositionTop;     horizontalPosition = horizontalPositionRight;   break;
         }

         bool mirrorFlag = (flip != tokNone);
         int penWidthIndex = getCamCadDatabase().getDefinedWidthIndex(strokeWidth);

         LayerStruct& layer = getDefinedLayer(layerName,materialNumber,mirroredLayerFlag);
         int layerIndex = layer.getLayerIndex();

         CBasesVector basesVector(ccPoint.x,ccPoint.y,1.,degrees,mirrorFlag);

         if (matrix != NULL)
         {
            basesVector.transform(*matrix);
         }

         CAttributes* pAttributes = &attributes;
         DbFlag dbFlag = 0;
         bool neverMirrorFlag  = false;
         bool proportionalFlag = true;

         getCamCadDoc().SetVisAttrib(&pAttributes,keywordIndex,valueTypeString,(void*)((const char*)textString),
            basesVector.getX(),basesVector.getY(),basesVector.getRotationRadians(),
            charHeight,charWidth,proportionalFlag,penWidthIndex,visibleFlag,SA_OVERWRITE,dbFlag,
            layerIndex,neverMirrorFlag,horizontalPosition,verticalPosition);
      }
   }
}

DataStruct *CZukenCr5000Reader::getSurfaceGeometryData(CCr5kMappedLayerDataList& mappedLayerDataList,
                                                const CString& layerName,int materialNumber,const CCr5kGeometry& cr5kGeometry,
                                                const CString& parentReference, GraphicClassTag graphicClass,
                                                const CTMatrix* matrix,const CCr5kProperties* properties, bool componentPlacedBottomFlag)
{
   if (cr5kGeometry.getType() == ptokSurface)
   {
      double outlineWidth = cr5kGeometry.getOutlineWidth();

      const CCr5kVertices& vertices = cr5kGeometry.getVertices();

      CString srcLineDesc( vertices.getSourceLineDescriptor() );
      //int srcLineNum = vertices.getSourceLineNumber();

      DataStruct* polyStruct = new_DataStruct(dataTypePoly);
      polyStruct->setGraphicClass(graphicClass);

      CPolyList& polyList = *(polyStruct->getPolyList());

      generatePolyListFromVertices(polyList,vertices,outlineWidth,false,polyTypeFilled);

      if (cr5kGeometry.hasOpenShapes())
      {
         const CCr5kOpenShapes& openShapes = cr5kGeometry.getOpenShapes();

         CPolyList voidPolyList;

         for (POSITION pos = openShapes.getHeadPosition();pos != NULL;)
         {
            CCr5kOpenShape* openShape = openShapes.getNext(pos);
            double outlineWidth = openShape->getOutlineWidth();
            const CCr5kVertices& openShapeVertices = openShape->getVertices();

            generatePolyListFromVertices(voidPolyList,openShapeVertices,outlineWidth,false,polyTypeVoid);
         }

         polyList.takeData(voidPolyList);
      }

      if (matrix != NULL)
      {
         CString materialLayerName;
         if (materialNumber >= 0)
         {
            CString materialName = this->getMaterials().getMaterialName(materialNumber);
            materialLayerName = this->getMaterialLayerName(layerName, materialName);
         }

         CTMatrix newMatrix(*matrix);
         LayerStruct* layer = NULL;
         if (!materialLayerName.IsEmpty())
            layer = doc->getLayer(materialLayerName);
         if (layer == NULL)
            layer = doc->getLayer(layerName);
         bool optionMirrorElectLayers = getOptionMirrorElectricalLayers();
         bool layerIsElectrical = (layer != NULL) ? (layer->isElectrical()) : false;
         bool hasElectricalMirrorLayer = (layerIsElectrical && optionMirrorElectLayers);

         if(!hasElectricalMirrorLayer && componentPlacedBottomFlag)
            newMatrix.rotateDegrees(180);
         polyStruct->transform(newMatrix);
      }

      setParentReferenceAttribute(*polyStruct,parentReference);
      setSourceLineNumberAttribute(*polyStruct,vertices);

      if (properties != NULL)
      {
         setProperties(polyStruct->attributes(),*properties);
      }

      return polyStruct;
   }

   return NULL;
}

void CZukenCr5000Reader::instantiateGeometryData(CCr5kMappedLayerDataList& mappedLayerDataList,
   const CString& layerName,int materialNumber,const CCr5kGeometry& cr5kGeometry,
   const CString& parentReference, GraphicClassTag graphicClass,
   const CTMatrix* matrix,const CCr5kProperties* properties, bool componentPlacedBottomFlag)
{
   switch (cr5kGeometry.getType())
   {
   case ptokSurface:
      {
         DataStruct *polyStruct = getSurfaceGeometryData(mappedLayerDataList,
            layerName, materialNumber, cr5kGeometry,
            parentReference,  graphicClass,
            matrix,  properties,  componentPlacedBottomFlag);

         // Save it.
         if (polyStruct != NULL)
            mappedLayerDataList.addTail(polyStruct, layerName, materialNumber);
      }

      break;
   case ptokLine:
   case ptokShieldLine:
      {
         const CCr5kVertices& vertices = cr5kGeometry.getVertices();
         DataStruct* polyStruct = new_DataStruct(dataTypePoly);
         polyStruct->setGraphicClass(graphicClass);

         generatePolyListFromVertices(*(polyStruct->getPolyList()),vertices,-1.,cr5kGeometry.hasSquarePenShape(),polyTypeOpen);

         if (matrix != NULL)
         {
            polyStruct->transform(*matrix);
         }

         setParentReferenceAttribute(*polyStruct,parentReference);
         setSourceLineNumberAttribute(*polyStruct,vertices);

         if (properties != NULL)
         {
            setProperties(polyStruct->attributes(),*properties);
         }

         mappedLayerDataList.addTail(polyStruct,layerName,materialNumber);
      }

      break;
   case ptokText:
   case ptokSymbolText:
      instantiateText(mappedLayerDataList,layerName,materialNumber,cr5kGeometry,parentReference,matrix,properties);
      break;
   }
}

void CZukenCr5000Reader::instantiatePrimitiveAttributes(CAttributes& attributes,bool visibleFlag,bool mirroredLayerFlag,const CString& layerName,const CCr5kLayoutPrimitive& primitive,const CTMatrix* matrix)
{
   switch (primitive.getPrimitiveType())
   {
   case ptokRefer:
      {
         const CCr5kReferPrimitive* referPrimitive = primitive.getReferPrimitive();

         if (referPrimitive != NULL)
         {  // coordinates in refer are not in board units, so don't use matrix
            instantiatePrimitiveAttributes(attributes,visibleFlag,mirroredLayerFlag,layerName,referPrimitive->getLayoutPrimitive());
         }
      }

      break;
   case ptokSymbolText:
      {
         const CCr5kGeometry& geometry = primitive.getSingleGeometry().getGeometry();
         Cr5kTokenTag attributeType = primitive.getType();

         instantiateGeometryAttributes(attributes,attributeType,visibleFlag,mirroredLayerFlag,layerName,primitive.getMaterialNumber(),geometry,matrix);
      }

      break;
   }
}

void CZukenCr5000Reader::instantiatePrimitiveData(
   CCr5kMappedLayerDataList& mappedLayerDataList,
   CCr5kLinkedGeometries& linkedGeometries,
   const CString& layerName,
   bool boardLayerFlag,
   bool boardLevelFlag,
   bool pinFlag,
   const CCr5kLayoutPrimitive& primitive,
   const CString& parentReference,
   const CString& pinRef,
   bool filterByConductivePadstackPadFlag,
   bool componentPlacedBottomFlag,
   const CTMatrix* matrix,
   const CBasesVector* componentBasesVector)
{
   Cr5kTokenTag primitiveType = primitive.getPrimitiveType();
   bool originalPinFlag = pinFlag;

   switch (primitiveType)
   {
   case ptokRefer:
      {
         const CCr5kReferPrimitive* referPrimitive = primitive.getReferPrimitive();

         if (referPrimitive != NULL)
         {  // coordinates in refer are not in board units, so don't use matrix, but do need to use it a little.
            CTMatrix mirrorMatrix;
            bool useMirrorMatrix = false;

            int reeferMaterialNumber = referPrimitive->getLayoutPrimitive().getMaterialNumber();
            CString materialLayerName;
            if (reeferMaterialNumber >= 0)
            {
               CString materialName = this->getMaterials().getMaterialName(reeferMaterialNumber);
               materialLayerName = this->getMaterialLayerName(layerName, materialName);
            }

            //LayerStruct* layer = doc->getLayer(layerName);
            LayerStruct* layer = NULL;
            if (!materialLayerName.IsEmpty())
               layer = doc->getLayer(materialLayerName);
            if (layer == NULL)
               layer = doc->getLayer(layerName);

            bool layerIsElectrical = (layer != NULL) ? (layer->isElectrical()) : false;
            bool optionMirrorElectLayers = getOptionMirrorElectricalLayers();
            bool hasElectricalMirrorLayer = (layerIsElectrical && optionMirrorElectLayers);
            bool matrixHasMirror = (matrix != NULL) && matrix->getMirror();


            // DR 831931 - Pin-related geometry came out in wrong place.
            // According to comment above it is apparently the case that matrix will do some units conversion
            // which is not desirable. But if this is for a pin, and pin stuff on electrical layers does mirroring,
            // then we need to consider mirror here too. Possibly other stuff like rotation too, but that does not
            // seem relevant to this test case so not pursuing it.
            // DR 856583 - Other geometry came out in wrong place.
            // In this case it is geometry on electrical layer but it is not a pin, but also we are
            // not getting mirror in the incoming matrix (matrixHasMirror is false). Not sure if
            // we should actually be adding that as a condition too.
            // Looks like the 831931 fix should not be limited to pins.
            bool mirrorForPin = (pinFlag && hasElectricalMirrorLayer && matrixHasMirror);
            bool mirrorForNonPinOnElectricalBottom = (!pinFlag && hasElectricalMirrorLayer && layer->isElectricalBottom());
            if (mirrorForPin || mirrorForNonPinOnElectricalBottom)
            {
               useMirrorMatrix = true;
               mirrorMatrix.mirrorAboutYAxis(true);
            }

            instantiatePrimitiveData(mappedLayerDataList,linkedGeometries,layerName,boardLayerFlag,boardLevelFlag,pinFlag,referPrimitive->getLayoutPrimitive(),parentReference,pinRef,filterByConductivePadstackPadFlag,componentPlacedBottomFlag, (useMirrorMatrix)?(&mirrorMatrix):NULL);
         }
      }

      break;

   case ptokSurface:
      // Pins processed differently than all else for surface. If not a pin then process
      // like "regular geometry", i.e. the same as before, the same as ptokLine and etc.
      // DR 810019 This importer is the most pathetic thing I have come across. It is far
      // for complex than is justified by the task at hand. What a pain in the neck.
#ifdef STILL_TRYING
      // This almost works, but not quite. I gave up on it for now and went with a 
      // post-process step instead.

      if (pinFlag && (debugIsR200T || debugIsR205T))
      {
         int jj = 0;
      }
      if (pinFlag)
      {
         CString padName = primitive.getName();
         if (padName.IsEmpty())
            padName = this->getNewPadName();
         CBasesVector apertureOffsetAndRotation;

         GraphicClassTag graphicClass = graphicClassNormal;
         const CCr5kGeometry& cr5kGeometry = primitive.getSingleGeometry().getGeometry();
         const CCr5kProperties& primitiveProperties = primitive.getProperties();

         bool padstackModeFlag = mappedLayerDataList.getPadstackCacheModeFlag();

         // The poly that is the shape.
         DataStruct *polyStruct = getSurfaceGeometryData(mappedLayerDataList,
            layerName, primitive.getMaterialNumber(), cr5kGeometry,
            parentReference,  graphicClass,
            matrix,  &primitiveProperties,  componentPlacedBottomFlag);

         // The geometry block that contains the poly.
         CString apBlkName = "AP_GEOM_" + padName;
         BlockStruct *apGeomBlk = this->getCamCadDatabase().getDefinedBlock(apBlkName, blockTypeUnknown);
         apGeomBlk->getDataList().AddHead(polyStruct);

         // The complex aperture that is the pad geometry.
         CString apName = "AP_" + padName + "_" + pinRef;
         BlockStruct* padGeometry = this->getCamCadDatabase().getDefinedAperture(apName, apertureComplex, apGeomBlk->getBlockNumber());

         DataStruct *padInsertData = instantiatePadInsertData(padGeometry, apertureOffsetAndRotation,
            mappedLayerDataList,
            linkedGeometries,
            layerName,
            boardLayerFlag,
            boardLevelFlag,
            pinFlag,
            primitive,
            parentReference,
            pinRef,
            filterByConductivePadstackPadFlag,
            componentPlacedBottomFlag,
            matrix,
            componentBasesVector);

         // It is correct to break inside the if.
         break;
      }
#endif
      // No break here, if not pinFlag then fall through to handle like other geometries.
   case ptokShieldLine:
   case ptokArea:
   case ptokText:
   case ptokLine:
      {
         GraphicClassTag graphicClass = graphicClassNormal;

         if (primitive.isNet())
         {
            graphicClass = graphicClassEtch;
         }

         mappedLayerDataList.pushBuffer();

         const CCr5kGeometry& geometry = primitive.getSingleGeometry().getGeometry();
         const CCr5kProperties& primitiveProperties = primitive.getProperties();

         instantiateGeometryData(mappedLayerDataList, layerName, primitive.getMaterialNumber(),
            geometry,
            parentReference, graphicClass, matrix, &primitiveProperties, componentPlacedBottomFlag);

         CString netName = primitive.getNetName();

         if (! netName.IsEmpty())
         {
            mappedLayerDataList.addNetName(netName);
         }

         mappedLayerDataList.popBuffer();
      }

      break;

   case ptokSymbolText:
      break;
   case ptokHole:
      break;
   case ptokOvalHole:
      break;
   case ptokSquareHole:
      break;
   case ptokDelete:
      break;

   case ptokPad:
   case ptokFpad:
      {
         CString padName = primitive.getName();
         CBasesVector apertureOffsetAndRotation;

         bool padstackModeFlag = mappedLayerDataList.getPadstackCacheModeFlag();

         BlockStruct* padGeometry = getInstantiatedPadGeometry(linkedGeometries, padName, apertureOffsetAndRotation);

         if (getDebugRefDesFlag())
         {
            CDebugWriteFormat::getWriteFormat().writef("\n" "%s; padName='%s', padGeometryName='%s', sourceLine=%s, apertureOffsetAndRotation=%s\n",
               __FUNCTION__, padName, ((padGeometry != NULL) ? padGeometry->getName() : "NULL"),
               primitive.getSourceLineDescriptor(),apertureOffsetAndRotation.getDescriptor());
         }

         //CDebugWriteFormat::getWriteFormat().writef("basesVector1=%s, basesVector2=%s, matrix=%s, inverseMatrix=%s\n",
         //   basesVectorDescriptor1,basesVectorDescriptor2,matrixDescriptor,inverseMatrix.getDescriptor());

         DataStruct *padInsertData = instantiatePadInsertData(padGeometry, apertureOffsetAndRotation,
            mappedLayerDataList,
            linkedGeometries,
            layerName,
            boardLayerFlag,
            boardLevelFlag,
            pinFlag,
            primitive,
            parentReference,
            pinRef,
            filterByConductivePadstackPadFlag,
            componentPlacedBottomFlag,
            matrix,
            componentBasesVector);
      }

      break;

   case ptokPadstack:   // the padstack may not be defined in the ftf file, instantiate a via if the fromTo parameter is present.
   case ptokFpadstack:  // footprint padstack should definitely be defined in the ftf file
      {
         CString padstackName = ((primitiveType == ptokPadstack) ? primitive.getName() : primitive.getFootPadstackGroupSet().getPadstackName());
         int fromLayerNumber = 0;
         int toLayerNumber   = 0;

         bool padstackCacheModeFlag = mappedLayerDataList.getPadstackCacheModeFlag();

         bool fromToFlag = primitive.getFromToLayers(fromLayerNumber,toLayerNumber);
         const CCr5kConductivePadStackPad* conductivePadStackPad = (filterByConductivePadstackPadFlag ? &(primitive.getConductivePadStackPad()) : NULL);
         
         bool topFlag = true;
         if (primitive.hasSide())
            topFlag = primitive.getPlacedTop();
         else
            topFlag = mappedLayerDataList.getTopFlag();

         bool instanceSpecificPadsPresent = primitive.hasPadGeometry();

         BlockStruct* cczPadstackGeometry = NULL;

         if (instanceSpecificPadsPresent)
         {
            cczPadstackGeometry = getInstantiatedPadstackGeometry(primitive, linkedGeometries, 
               pinRef, padstackName, topFlag, fromLayerNumber, toLayerNumber,
               conductivePadStackPad);
         }
         else
         {
            cczPadstackGeometry = getInstantiatedPadstackGeometry(linkedGeometries,padstackName,topFlag,fromLayerNumber,toLayerNumber,conductivePadStackPad);
         }

         InsertTypeTag insertType = (originalPinFlag ? insertTypePin : insertTypeMechanicalPin);

         if (boardLevelFlag)
         {
            insertType = insertTypeVia;
         }

         if (cczPadstackGeometry == NULL && fromToFlag) // not found in ftf file, create default
         {
            insertType = insertTypeVia;
            cczPadstackGeometry = getInstantiatedDefaultViaGeometry(fromLayerNumber,toLayerNumber);
         }

         if (cczPadstackGeometry != NULL)
         {
            bool pinFlag = true;
            CString reference = parentReference;

            if (primitive.isVia())
            {
               if (insertType != insertTypeMechanicalPin)
               {
                  insertType = insertTypeVia;
               }
            }

            if (primitive.isBareChipPad())
            {
               pinFlag = false;
               insertType = insertTypeFreePad;
            }

            CBasesVector basesVector = primitive.getPadstackBasesVector();

            // debug
            CString basesVectorDescriptor1 = basesVector.getDescriptor();

            DataStruct* ccPadstack = getCamCadDatabase().insertBlock(cczPadstackGeometry, insertType,reference,-1);
            ccPadstack->getInsert()->setBasesVector(basesVector);
            instantiateTestPad(primitive,*ccPadstack,pinRef); // use board coordinates for test pads

            if (matrix != NULL)
            {
               basesVector.transform(*matrix);
            }

            // debug
            CString basesVectorDescriptor2 = basesVector.getDescriptor();

            if (! padstackCacheModeFlag)
            {
               CString padstackBasesVectorDescriptor;
               CTMatrix inverseMatrix;
               CString matrixDescriptor("null");

               if (matrix != NULL)
               {
                  inverseMatrix = *matrix;
                  inverseMatrix.invert();

                  matrixDescriptor = matrix->getDescriptor();
               }

               if (getDebugRefDesFlag())
               {
                  CDebugWriteFormat::getWriteFormat().writef("\n" "%s; padstackName='%s', sourceLine=%s\n",
                     __FUNCTION__,padstackName,primitive.getSourceLineDescriptor());

                  CDebugWriteFormat::getWriteFormat().writef("basesVector1=%s, basesVector2=%s, matrix=%s, inverseMatrix=%s\n",
                     basesVectorDescriptor1,basesVectorDescriptor2,matrixDescriptor,inverseMatrix.getDescriptor());
               }

               if (componentBasesVector != NULL)
               {
                  CBasesVector adjustedComponentBasesVector(componentBasesVector->getOrigin().x,componentBasesVector->getOrigin().y,1.,componentBasesVector->getRotationDegrees(),false);

                  if (getDebugRefDesFlag())
                  {
                     CDebugWriteFormat::getWriteFormat().writef("componentBasesVector='%s', adjustedComponentBasesVector=%s\n",
                        componentBasesVector->getDescriptor(),adjustedComponentBasesVector.getDescriptor());
                  }

                  /////
                  CBasesVector adjustedComponentBasesVector2(*componentBasesVector);

                  if (componentBasesVector->getMirror())
                  {
                     CTMatrix mirrorMatrix;
                     mirrorMatrix.mirrorAboutYAxisCtm();
                     adjustedComponentBasesVector2.transform(mirrorMatrix);
                  }

                  if (getDebugRefDesFlag())
                  {
                     CDebugWriteFormat::getWriteFormat().writef("componentBasesVector='%s', adjustedComponentBasesVector2=%s\n",
                        componentBasesVector->getDescriptor(),adjustedComponentBasesVector2.getDescriptor());
                  }

                  /////
                  CBasesVector adjustedComponentBasesVector3(*componentBasesVector);

                  //if (componentBasesVector->getMirror())
                  //{
                  //   CTMatrix mirrorMatrix;
                  //   mirrorMatrix.mirrorAboutYAxis();
                  //   adjustedComponentBasesVector3.transform(mirrorMatrix);
                  //}

                  if (getDebugRefDesFlag())
                  {
                     CDebugWriteFormat::getWriteFormat().writef("componentBasesVector='%s', adjustedComponentBasesVector3=%s\n",
                        componentBasesVector->getDescriptor(),adjustedComponentBasesVector3.getDescriptor());
                  }

                  //
                  CTMatrix componentToBoardMatrix = adjustedComponentBasesVector.getTransformationMatrix();
                  CTMatrix boardToComponentMatrix(componentToBoardMatrix);
                  boardToComponentMatrix.invert();

                  CBasesVector padstackBasesVectorBoard     = primitive.getPadstackBasesVector();
                  CBasesVector padstackBasesVectorComponent(padstackBasesVectorBoard);
                  padstackBasesVectorComponent.transform(boardToComponentMatrix);
                  //padstackBasesVectorDescriptor = padstackBasesVector.getDescriptor();

                  basesVector = padstackBasesVectorComponent;

                  if (getDebugRefDesFlag())
                  {
                     CDebugWriteFormat::getWriteFormat().writef("boardToComponentMatrix=%s, componentToBoardMatrix=%s\n",
                        boardToComponentMatrix.getDescriptor(),componentToBoardMatrix.getDescriptor());

                     CDebugWriteFormat::getWriteFormat().writef("padstackBasesVectorBoard='%s', padstackBasesVectorComponent=%s\n",
                        padstackBasesVectorBoard.getDescriptor(),padstackBasesVectorComponent.getDescriptor());

                     CBasesVector checkBasesVectorBoard(padstackBasesVectorComponent);
                     checkBasesVectorBoard.transform(componentToBoardMatrix);

                     CDebugWriteFormat::getWriteFormat().writef("checkBasesVectorBoard='%s'\n",
                        checkBasesVectorBoard.getDescriptor());
                  }
               }
            }


            ccPadstack->getInsert()->setBasesVector(basesVector);

            setParentReferenceAttribute(*ccPadstack,parentReference);
            setSourceLineNumberAttribute(*ccPadstack,primitive);

            mappedLayerDataList.pushEnableLayerMapping(padstackCacheModeFlag && !boardLayerFlag);

            if (pinFlag)
            {
               normalizeLayerNumberSpan(fromLayerNumber,toLayerNumber);

               mappedLayerDataList.pushPadstackBuffer(padstackName,basesVector,fromLayerNumber,toLayerNumber);
            }
            else
            {
               mappedLayerDataList.pushBuffer();
            }

            mappedLayerDataList.addTailWithoutLayerMapping(ccPadstack,-1);

            CString netName = primitive.getNetName();

            if (! netName.IsEmpty())
            {
               mappedLayerDataList.addNetName(netName);
            }

            if (! reference.IsEmpty())
            {
               mappedLayerDataList.addRefName(reference);
            }

            mappedLayerDataList.popBuffer();
            mappedLayerDataList.popEnableLayerMapping();
         }
      }

      break;
   case ptokDimension:
      break;
   case ptokDimLeader:
      break;
   case ptokBasePoint:
      break;
   case ptokBoardAssembly:
      if (getPanelDataFlag())
      {
         BlockStruct* boardBlock = NULL;
         map<int, BlockStruct*>::iterator it = subBoardsMap.find(primitive.getSubBoard());         
         if(it != subBoardsMap.end())
            boardBlock = (*it).second;
         if (boardBlock != NULL)
         {
            CString refName = primitive.getName();
            CPoint2d origin = primitive.getPoint().getPoint();
            double angleDegrees = primitive.getAngleDegrees();
            bool mirrorFlag     = primitive.getFlipX();

            DataStruct* subBoardData = getCamCadDatabase().insertBlock(boardBlock,insertTypePcb,refName,-1,origin.x,origin.y,degreesToRadians(angleDegrees),mirrorFlag);

            mappedLayerDataList.addTailWithoutLayerMapping(subBoardData,-1);
         }
      }

      break;
   case ptokInfo:
      break;
   case ptokMessage:
      break;
   case ptokMeshplane:
      break;
   case ptokRuleArea:
      break;
   case ptokHiePort:
      //{
      //   int iii = 3;
      //}

      break;
   }
}


DataStruct *CZukenCr5000Reader::instantiatePadInsertData(BlockStruct *padGeometry, CBasesVector &apertureOffsetAndRotation,
   CCr5kMappedLayerDataList& mappedLayerDataList,
   CCr5kLinkedGeometries& linkedGeometries,
   const CString& layerName,
   bool boardLayerFlag,
   bool boardLevelFlag,
   bool pinFlag,
   const CCr5kLayoutPrimitive& primitive,
   const CString& parentReference,
   const CString& pinRef,
   bool filterByConductivePadstackPadFlag,
   bool componentPlacedBottomFlag,
   const CTMatrix* matrix,
   const CBasesVector* componentBasesVector)
{
   if (padGeometry != NULL)
   {
      CString padName( padGeometry->getName() );

      CBasesVector basesVector = primitive.getPadstackBasesVector();
      pinFlag = pinFlag && !primitive.isBareChipPad();

      DataStruct* ccPad = getCamCadDatabase().insertBlock(padGeometry,insertTypeUnknown,parentReference,0);
      ccPad->getInsert()->setBasesVector(basesVector);    

      bool padstackModeFlag = mappedLayerDataList.getPadstackCacheModeFlag();
      bool enableLayerMapping = (padstackModeFlag && !boardLayerFlag);

      if (! padstackModeFlag)
      {
         if (componentBasesVector != NULL)
         {
            CBasesVector adjustedComponentBasesVector(componentBasesVector->getOrigin().x,componentBasesVector->getOrigin().y,1.,componentBasesVector->getRotationDegrees(),false);

            CTMatrix componentToBoardMatrix = adjustedComponentBasesVector.getTransformationMatrix();
            CTMatrix boardToComponentMatrix(componentToBoardMatrix);
            boardToComponentMatrix.invert();

            CBasesVector padstackBasesVectorBoard     = primitive.getPadstackBasesVector();
            CTMatrix padstackToBoardMatrix = padstackBasesVectorBoard.getTransformationMatrix();

            CBasesVector padBasesVector = apertureOffsetAndRotation;
            padBasesVector.transform(padstackToBoardMatrix);

            if (getDebugRefDesFlag())
            {
               CDebugWriteFormat::getWriteFormat().writef("padBasesVector='%s', primitive.getPadstackBasesVector()=%s\n",
                  padBasesVector.getDescriptor(),primitive.getPadstackBasesVector().getDescriptor());
            }

            CBasesVector padstackBasesVectorComponent(padBasesVector);
            padstackBasesVectorComponent.transform(boardToComponentMatrix);

            basesVector = padstackBasesVectorComponent;

            if (getDebugRefDesFlag())
            {
               CDebugWriteFormat::getWriteFormat().writef("boardToComponentMatrix=%s, componentToBoardMatrix=%s\n",
                  boardToComponentMatrix.getDescriptor(),componentToBoardMatrix.getDescriptor());

               CDebugWriteFormat::getWriteFormat().writef("padstackBasesVectorBoard='%s', padstackBasesVectorComponent=%s\n",
                  padstackBasesVectorBoard.getDescriptor(),padstackBasesVectorComponent.getDescriptor());
            }

            CBasesVector checkBasesVectorBoard(padstackBasesVectorComponent);
            checkBasesVectorBoard.transform(componentToBoardMatrix);

            if (getDebugRefDesFlag())
            {
               CDebugWriteFormat::getWriteFormat().writef("checkBasesVectorBoard='%s'\n",
                  checkBasesVectorBoard.getDescriptor());
            }

            ccPad->getInsert()->setBasesVector(basesVector);
            apertureOffsetAndRotation.transform(boardToComponentMatrix);
         }
         else
         {
            CTMatrix padMatrix = apertureOffsetAndRotation.getTransformationMatrix() * basesVector.getTransformationMatrix();
            ccPad->getInsert()->setBasesVector(padMatrix.getBasesVector());

            enableLayerMapping = !boardLayerFlag;
         }
      }
      else if (matrix != NULL)
      {
         if (!pinFlag)
         {
            ccPad->transform(*matrix);
            apertureOffsetAndRotation.transform(*matrix);
         }
         else
         {
            // for T9532.C, pad R9.0-8.0Y-3.85
            CTMatrix pinToBoardMatrix = basesVector.getTransformationMatrix();
            CTMatrix boardToPinMatrix(pinToBoardMatrix);
            boardToPinMatrix.invert();

            if (getDebugRefDesFlag())
            {
               CDebugWriteFormat::getWriteFormat().writef("pinToBoardMatrix=%s, boardToPinMatrix=%s\n",
                  pinToBoardMatrix.getDescriptor(),boardToPinMatrix.getDescriptor());
            }

            //CDebugWriteFormat::getWriteFormat().writef("pinToBoardMatrix=%s\n",
            //   pinToBoardMatrix.getDescriptor());

            // To calculate the location (bases vector) of the pad in board space, A bases vector formed from the pad offset should
            // transformed from the pin's bases vector.
            // This pad bases vector in board space should then be converted to padstack space.
            CBasesVector padBasesVectorBoard(apertureOffsetAndRotation);
            padBasesVectorBoard.transform(pinToBoardMatrix);

            if (getDebugRefDesFlag())
            {
               CDebugWriteFormat::getWriteFormat().writef("padBasesVectorBoard=%s\n",
                  padBasesVectorBoard.getDescriptor());
            }

            CBasesVector adjustedComponentBasesVector(componentBasesVector->getOrigin().x,componentBasesVector->getOrigin().y,1.,componentBasesVector->getRotationDegrees(),componentBasesVector->getMirror());

            CTMatrix componentToBoardMatrix = adjustedComponentBasesVector.getTransformationMatrix();
            CTMatrix boardToComponentMatrix(componentToBoardMatrix);
            boardToComponentMatrix.invert();

            CBasesVector padBasesVectorComponent(padBasesVectorBoard);
            padBasesVectorComponent.transform(boardToComponentMatrix);

            ccPad->getInsert()->setBasesVector(padBasesVectorComponent);    
            apertureOffsetAndRotation.set();

            if (getDebugRefDesFlag())
            {
               CDebugWriteFormat::getWriteFormat().writef("padBasesVectorComponent=%s, apertureOffsetAndRotation=%s\n",
                  padBasesVectorComponent.getDescriptor(),apertureOffsetAndRotation.getDescriptor());
            }
         }
      }
      else  // library mode geometry, coordinates in component geometry space
      {
         CBasesVector pinBasesVector(mappedLayerDataList.getPinBasesVector());
         CTMatrix padToCompMatrix = apertureOffsetAndRotation.getTransformationMatrix() * pinBasesVector.getTransformationMatrix();

         CBasesVector padBasesVector;
         padBasesVector.transform(padToCompMatrix);

         ccPad->getInsert()->setBasesVector(padBasesVector);    
      }

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("apertureOffsetAndRotation='%s'\n",
            apertureOffsetAndRotation.getDescriptor());
      }

      setParentReferenceAttribute(*ccPad, parentReference);
      setSourceLineNumberAttribute(*ccPad, primitive);

      mappedLayerDataList.pushEnableLayerMapping(enableLayerMapping);

      if (pinFlag)
         mappedLayerDataList.pushPadBuffer(padName, apertureOffsetAndRotation);
      else
         mappedLayerDataList.pushBuffer();

      mappedLayerDataList.addTail(ccPad, layerName, primitive.getMaterialNumber());
      mappedLayerDataList.popBuffer();

      mappedLayerDataList.popEnableLayerMapping();

      return ccPad;
   }

   return NULL;
}

void CZukenCr5000Reader::instantiateMounterInformation(CCr5kMappedLayerDataList& mappedLayerDataList,const CCr5kFootprint& footprint)
{
#ifdef DEADCODE // makes side-spcific centroids which messes up consolidation
                // and we're not sure customers even want this centroid (we're asking them)
                // in the meantime we'll just stop doing this.

   const CCr5kMounterInformation& mounterInformation = footprint.getMounterInformation();

   if (mounterInformation.isLoaded())
   {
      const CCr5kMachineNames& machineNames = mounterInformation.getMachineNames();

      CString mounterMachineName = getOptionMounterMachineName();

      for (POSITION machineNamePos = machineNames.getHeadPosition();machineNamePos != NULL;)
      {
         CCr5kMachineName* machineName = machineNames.getAt(machineNamePos);
         CString machineNameString = machineName->getMachineName();

         if (mounterMachineName.IsEmpty() || mounterMachineName.CompareNoCase(machineNameString) == 0)
         {
            const CCr5kMountBasePoints& mountBasePoints = machineName->getMountBasePoints();

            for (POSITION mountBasePointPos = mountBasePoints.getHeadPosition();mountBasePointPos != NULL;)
            {
               CCr5kMountBasePoint* mountBasePoint = mountBasePoints.getAt(mountBasePointPos);
               CPoint2d centroidOrigin = mountBasePoint->getMountBasePoint().getPoint();

               BlockStruct* centroidGeometry = getCamCadDatabase().getDefinedCentroidGeometry(getCadFileNumber());
               LayerStruct* layer = getCamCadDatabase().getLayer(mappedLayerDataList.getTopFlag() ? ccLayerCentroidTop : ccLayerCentroidBottom);
               layer->setMirroredLayerIndex(layer->getLayerIndex()); // don't mirror centroid layers

               DataStruct* centroidData = getCamCadDatabase().insertBlock(centroidGeometry,insertTypeCentroid,"",layer->getLayerIndex(),centroidOrigin.x,centroidOrigin.y);

               int machineNameKeywordIndex = getCamCadDatabase().registerKeyword("machineName",valueTypeString);
               getCamCadDatabase().addAttribute(centroidData->attributes(),machineNameKeywordIndex,machineNameString);

               mappedLayerDataList.addTailWithoutLayerMapping(centroidData,layer->getLayerIndex());

               break;
            }

            break;
         }
      }
   }
#endif
}

void CZukenCr5000Reader::instantiateLayerAttributes(CAttributes& attributes,bool visibleFlag,bool mirroredLayerFlag,const CCr5kLayoutLayer& layer,const CTMatrix& matrix)
{
   const CCr5kLayerIdentifier& layerIdentifier = layer.getLayerIdentifier();
   CString layerName = layerIdentifier.getLayerName();

   for (POSITION primitivePos = layer.getHeadPosition();primitivePos != NULL;)
   {
      CCr5kLayoutPrimitive* primitive = layer.getNext(primitivePos);

      instantiatePrimitiveAttributes(attributes,visibleFlag,mirroredLayerFlag,layerName,*primitive,&matrix);
   }
}

void CZukenCr5000Reader::instantiateLayerData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,
   const CCr5kLayoutLayer& layer,bool boardLayerFlag,bool boardLevelFlag,bool pinFlag,const CString& parentReference,const CString& pinRef,bool filterByConductivePadstackPadFlag,bool componentPlacedBottomFlag,
   const CTMatrix* matrix,const CBasesVector* componentBasesVector)
{
   const CCr5kLayerIdentifier& layerIdentifier = layer.getLayerIdentifier();
   CString layerName = layerIdentifier.getLayerName();

   bool unfillPolyFlag = false;

   if (layerIdentifier.getType() == ptokSystemLayer)
   {
      Cr5kTokenTag systemLayerType = layerIdentifier.getSystemLayerType().getSystemLayerType();
      unfillPolyFlag = (systemLayerType == tokBoard_Figure || systemLayerType == tokLayout_Area);
   }

   if (unfillPolyFlag)  // may have to add a new methof to CCr5kMappedLayerDataList; setUnfillPolyFlag()
   {
      mappedLayerDataList.pushBuffer();
   }

   for (POSITION primitivePos = layer.getHeadPosition();primitivePos != NULL;)
   {
      CCr5kLayoutPrimitive* primitive = layer.getNext(primitivePos);

      instantiatePrimitiveData(mappedLayerDataList,linkedGeometries,layerName,boardLayerFlag,boardLevelFlag,pinFlag,*primitive,parentReference,pinRef,filterByConductivePadstackPadFlag,componentPlacedBottomFlag,matrix,componentBasesVector);
   }

   if (unfillPolyFlag)
   {
      mappedLayerDataList.unfillPolys();
      mappedLayerDataList.popBuffer();
   }
}

void CZukenCr5000Reader::instantiateLayoutAttributes(CAttributes& attributes,bool visibleFlag,bool mirroredLayerFlag,const CCr5kLayout& layout,const CTMatrix& matrix)
{
   for (POSITION layerPos = layout.getHeadPosition();layerPos != NULL;)
   {
      CCr5kLayoutLayer* layer = layout.getNext(layerPos);

      instantiateLayerAttributes(attributes,visibleFlag,mirroredLayerFlag,*layer,matrix);
   }
}

void CZukenCr5000Reader::instantiateLayoutData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,
   const CCr5kLayout& layout,bool boardLayerFlag,bool boardLevelFlag,bool pinFlag,const CString& parentReference,const CString& pinRef,bool filterByConductivePadstackPadFlag,bool componentPlacedBottomFlag,
   const CTMatrix* matrix,const CBasesVector* componentBasesVector,COperationProgress* progress)
{
   if (progress != NULL)
   {
      progress->updateStatus("Instantiating layout data.",layout.getCount());
   }

   for (POSITION layerPos = layout.getHeadPosition();layerPos != NULL;)
   {
      if (progress != NULL) progress->incrementProgress();
      CCr5kLayoutLayer* layer = layout.getNext(layerPos);
      instantiateLayerData(mappedLayerDataList,linkedGeometries,*layer,boardLayerFlag,boardLevelFlag,pinFlag,parentReference,pinRef,filterByConductivePadstackPadFlag,componentPlacedBottomFlag,matrix,componentBasesVector);
   }
}

CString CZukenCr5000Reader::getBondwireLayerName(bool topFlag,bool isGeneratedFlag,double diameter) const
{
   CString bondwireLayerName("Bondwire");

   if (isGeneratedFlag && m_optionSegregateGeneratedBondwires)
   {
      bondwireLayerName.Append("-generated");
   }

   if (m_optionSegregateBondwiresByDiameter)
   {
      bondwireLayerName.AppendFormat("-%s",fpfmt(diameter,6));
   }

   bondwireLayerName.Append(topFlag ?  "-top" : "-bottom");

   return bondwireLayerName;
}

void CZukenCr5000Reader::instantiateTestPad(const CCr5kLayoutPrimitive& layoutPrimitive,DataStruct& data,const CString& pinRef)
{
   if (data.getDataType() == dataTypeInsert)
   {
      const CCr5kTestPad& testPad = layoutPrimitive.getTestPad();
      CPoint2d testPadOrigin = data.getInsert()->getOrigin2d();

      //if (testPad.getSourceLineDescriptor() == "Pcf.11023")
      //{
      //   int iii = 3;
      //}

      if (testPad.isTestPad())
      {
         if (testPad.getPoint().isLoaded())
         {
            testPadOrigin = testPad.getPoint().getPoint();
         }

         CString netName = layoutPrimitive.getNetName();

         //if (netName.IsEmpty())
         //{
         //   netName = defaultNetName;
         //}

         CString refDes  = testPad.getTestPadReference();
         bool topFlag    = testPad.isTestPadSideTop();
         bool bottomFlag = !topFlag;
         
         int dataLinkKeywordIndex       = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataLink);
         int refNameKeywordIndex        = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeRefName);
         int netNameKeywordIndex        = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeNetName);
         int probePlacementKeywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeProbePlacement);
         int testResourceKeywordIndex   = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeTestResource);
         int testKeywordIndex           = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeTest);

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         BlockStruct* testAccessGeometry = getTestAccessGeometry();

         DataStruct* testAccessData = getCamCadDatabase().insertBlock(testAccessGeometry,insertTypeTestAccessPoint,refDes,-1,testPadOrigin.x,testPadOrigin.y,0.,bottomFlag);
         testAccessData->getInsert()->setPlacedBottom(bottomFlag);

         //if (testAccessData->getEntityNumber() == 202)
         //{
         //   int iii = 3;
         //}

         // Test Access Point attributes
         // - DATALINK
         // - NETNAME
         // - REFNAME
         // - EXPOSE_METAL_DIAMETER n/a
         // - TARGET_TYPE n/a

         if (netName.IsEmpty())
            netName = NET_UNUSED_PINS;

         long entityNumber = data.getEntityNumber();
         if (!pinRef.IsEmpty())
         {
            // This test access marker is pointed to a comppin so need to get entity number from comppin
            int index = pinRef.Find(".");
            if (index > 0)
            {
               CString refDes = pinRef.Left(index);
               CString pinNumber = pinRef.Right(pinRef.GetLength()-index-1);
               CCamCadPin* camCadPin = getCamCadDatabase().getDefinedCamCadPin(getCadFile(),refDes,pinNumber,netName);
               entityNumber = camCadPin->getCompPin()->getEntityNumber();
            }
         }

         getCamCadDatabase().addAttribute(testAccessData->attributes(),refNameKeywordIndex,refDes);
         getCamCadDatabase().addAttribute(testAccessData->attributes(),netNameKeywordIndex,netName);
         getCamCadDatabase().addAttribute(testAccessData->attributes(),dataLinkKeywordIndex,entityNumber);

         setSourceLineNumberAttribute(*testAccessData,layoutPrimitive);

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         BlockStruct* testProbeGeometry = getTestProbeGeometry();

         DataStruct* testProbeData = getCamCadDatabase().insertBlock(testProbeGeometry,insertTypeTestProbe,refDes,-1,testPadOrigin.x,testPadOrigin.y,0.,bottomFlag);
         testProbeData->getInsert()->setPlacedBottom(bottomFlag);

         // Test Probe attributes
         // - DATALINK
         // - NETNAME
         // - REFNAME
         // - PROBE_PLACEMENT "Placed"
         // - TEST_RESOURCE "Test"

         getCamCadDatabase().addAttribute(testProbeData->attributes(),refNameKeywordIndex,refDes);
         getCamCadDatabase().addAttribute(testProbeData->attributes(),netNameKeywordIndex,netName);
         getCamCadDatabase().addAttribute(testProbeData->attributes(),dataLinkKeywordIndex,testAccessData->getEntityNumber());
         getCamCadDatabase().addAttribute(testProbeData->attributes(),probePlacementKeywordIndex,"Placed");
         getCamCadDatabase().addAttribute(testProbeData->attributes(),testResourceKeywordIndex,"Test");

         setSourceLineNumberAttribute(*testProbeData,layoutPrimitive);

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         getPcbFile()->getBlock()->getDataList().AddTail(testAccessData);
         getPcbFile()->getBlock()->getDataList().AddTail(testProbeData);

         getCamCadDatabase().addAttribute(data.attributes(),testKeywordIndex,"");
      }
   }
}

void CZukenCr5000Reader::instantiateComponentPinData(CCr5kMappedLayerDataList& componentMappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,
   const CCr5kCompPins& compPins,const CCr5kComponent& component)
{
   componentMappedLayerDataList.setThruFlag(component.isThruHole());
   componentMappedLayerDataList.setTopFlag(component.getPlacedTop());

   CString refDes = component.getRefDes();
   bool componentPlacedBottomFlag = component.getPlacedBottom();

   // generate matrix to transform board units to geometry units
   CBasesVector componentBasesVector = component.getBasesVector(false);
   CTMatrix componentToBoardMatrix = componentBasesVector.getTransformationMatrix();
   CTMatrix boardToComponentMatrix(componentToBoardMatrix);
   boardToComponentMatrix.invert();

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("\n" "entered %s =====================================================================\n",__FUNCTION__);
      CDebugWriteFormat::getWriteFormat().writef("refDes='%s'  componentBasesVector='%s' [Board]\n\n",refDes,componentBasesVector.getDescriptor());
      CDebug::addDebugAxis(getCamCadDatabase(),componentBasesVector,AxisSize,"componentBasesVector");
   }

   for (POSITION pinPos = compPins.getHeadPosition();pinPos != NULL;)
   {
      CCr5kCompPin* compPin = compPins.getNext(pinPos);

      CString pinNumber   = compPin->getPinNumber();
      CString pinRef = refDes + "." + pinNumber;

      // ### This is where the pin coordinate system is defined
      CBasesVector pinBasesVector = compPin->getBasesVector();
      pinBasesVector.set(pinBasesVector.getOrigin(),componentBasesVector.getRotationDegrees()/*,componentBasesVector.getMirror()*/);

      // debug 
      CString pinBasesVectorDescriptor1 = pinBasesVector.getDescriptor();

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("\n");
         CDebugWriteFormat::getWriteFormat().writef("pinRef='%s'  pinBasesVector='%s' [Board]\n",pinRef,pinBasesVectorDescriptor1);
         CDebug::addDebugAxis(getCamCadDatabase(),pinBasesVector,AxisSize,"pinBasesVector");
      }

      //CPoint2d origin     = compPin->getPoint().getPoint();
      //CString  netName    = compPin->getNetName();

      //CCamCadPin* camCadPin = getCamCadDatabase().getDefinedCamCadPin(getCadFile(),refDes,pinNumber,netName);
      //camCadPin->getCompPin()->setOrigin(pinBasesVector.getOrigin());
      //camCadPin->getCompPin()->setPinCoordinatesComplete(true);
      //camCadPin->getCompPin()->setVisible(component.getPlacedTop() ? visibleTop : visibleBottom);

      CString reference;
      reference.Format("Pin(%s)",pinNumber);

      pinBasesVector.transform(boardToComponentMatrix);

      bool bottomPlacedComponentFlag = pinBasesVector.getMirror();

      if (bottomPlacedComponentFlag)
      {
         CBasesVector oldPinBasesVector(pinBasesVector);

         pinBasesVector.set(pinBasesVector.getOrigin(),pinBasesVector.getRotationDegrees());

         if (getDebugRefDesFlag())
         {
            CDebugWriteFormat::getWriteFormat().writef("\n");
            CDebugWriteFormat::getWriteFormat().writef("pinRef='%s' bottomPlacedComponent oldPinBasesVector='%s' [Component]\n",pinRef,oldPinBasesVector.getDescriptor());
            CDebugWriteFormat::getWriteFormat().writef("pinRef='%s' bottomPlacedComponent    pinBasesVector='%s' [Component]\n",pinRef,pinBasesVector.getDescriptor());
            CDebugWriteFormat::getWriteFormat().writef("\n");
         }
      }

      CTMatrix pinToCompMatrix = pinBasesVector.getTransformationMatrix();

      // debug 
      CString pinBasesVectorDescriptor2 = pinBasesVector.getDescriptor();

      if (getDebugRefDesFlag())
      {
         CDebugWriteFormat::getWriteFormat().writef("pinRef='%s'  pinBasesVector='%s' [Component]\n",pinRef,pinBasesVectorDescriptor2);

         CBasesVector examplePinBasesVector;
         CDebugWriteFormat::getWriteFormat().writef("pinRef='%s'  examplePinBasesVector='%s' [pin]\n",pinRef,examplePinBasesVector.getDescriptor());

         examplePinBasesVector.transform(pinToCompMatrix);
         CDebugWriteFormat::getWriteFormat().writef("pinRef='%s'  examplePinBasesVector='%s' [component]\n",pinRef,examplePinBasesVector.getDescriptor());

         examplePinBasesVector.transform(componentToBoardMatrix);
         CDebugWriteFormat::getWriteFormat().writef("pinRef='%s'  examplePinBasesVector='%s' [board]\n",pinRef,examplePinBasesVector.getDescriptor());
      }

      // create the padstack for the pin and instantiate it
      bool boardLayerFlag = true;
      bool boardLevelFlag = false;
      bool pinFlag = true;
      CBasesVector rawComponentBasesVector = component.getBasesVector(false);

      componentMappedLayerDataList.startPadstackCache(pinBasesVector,getOptionRestructurePadstacks());
      instantiateLayoutData(componentMappedLayerDataList,linkedGeometries,compPin->getLayout(),boardLayerFlag,boardLevelFlag,pinFlag,
         reference,pinRef,true,componentPlacedBottomFlag,&boardToComponentMatrix,&rawComponentBasesVector);
      componentMappedLayerDataList.instantiatePadstackCache(pinToCompMatrix,componentToBoardMatrix,bottomPlacedComponentFlag,refDes,pinNumber);

      const CCr5kBondwires& bondwires = compPin->getBondwires();
      int materialNumber  = bondwires.getMaterialNumber();
      double wireDiameter = bondwires.getWireDiameterInPageUnits();
      int widthIndex      = getCamCadDatabase().getDefinedWidthIndex(wireDiameter);

      for (POSITION wirePos = bondwires.getHeadPosition();wirePos != NULL;)
      {
         CCr5kBondwire* wire = bondwires.getNext(wirePos);

         CPoint2d startPoint = wire->getStartPoint().getPoint();
         CPoint2d   endPoint = wire->getEndPoint().getPoint();
         bool isGenerated    = wire->isGenerated();

         CString layerName = getBondwireLayerName(component.getPlacedTop(),isGenerated,wireDiameter);
         LayerStruct& ccLayer = getDefinedLayer(layerName,materialNumber);

         CPoly* poly = new CPoly();

         poly->addVertex(startPoint);
         poly->addVertex(  endPoint);
         poly->setWidthIndex(widthIndex);

         DataStruct* polyStruct = new_DataStruct(dataTypePoly);
         polyStruct->setLayerIndex(ccLayer.getLayerIndex());
         polyStruct->getPolyList()->AddTail(poly);

         polyStruct->transform(boardToComponentMatrix);

         setParentReferenceAttribute(*polyStruct,reference);
         setSourceLineNumberAttribute(*polyStruct,*wire);

         componentMappedLayerDataList.addTailWithoutLayerMapping(polyStruct,ccLayer.getLayerIndex());
      }
   }

   if (getDebugRefDesFlag())
   {
      CDebugWriteFormat::getWriteFormat().writef("\n" "exiting %s =====================================================================\n",__FUNCTION__);
   }

   //setDebugRefDesFlag(false);
}

void CZukenCr5000Reader::instantiateComponentCompPinData(const CCr5kComponent& component)
{
   CString refDes = component.getRefDes();
   const CCr5kCompPins& compPins = component.getCompPins();

   CBasesVector componentBasesVector = component.getBasesVector(false);

   for (POSITION pinPos = compPins.getHeadPosition();pinPos != NULL;)
   {
      CCr5kCompPin* compPin = compPins.getNext(pinPos);

      CString pinNumber = compPin->getPinNumber();
      CString pinRef    = refDes + "." + pinNumber;

      // ### This is where the pin coordinate system is defined
      CBasesVector pinBasesVector = compPin->getBasesVector();
      pinBasesVector.set(pinBasesVector.getOrigin(),componentBasesVector.getRotationDegrees()/*,componentBasesVector.getMirror()*/);

      CString netName = compPin->getNetName();
      if (netName.CompareNoCase(NET_UNUSED_PINS) == 0)
      {
         getCamCadDatabase().getDefinedNet(netName, getCadFile())->setFlags(NETFLAG_UNUSEDNET);
      }

      CCamCadPin* camCadPin = getCamCadDatabase().getDefinedCamCadPin(getCadFile(),refDes,pinNumber,netName);
      camCadPin->getCompPin()->setOrigin(pinBasesVector.getOrigin());
      camCadPin->getCompPin()->setPinCoordinatesComplete(true);
      camCadPin->getCompPin()->setVisible(component.getPlacedTop() ? visibleTop : visibleBottom);
   }
}

void CZukenCr5000Reader::instantiateToeprintCompPinData(const CCr5kToeprintPins& toeprintPins,const CCr5kComponent& component)
{
   CString refDes = component.getRefDes();
   CTMatrix matrix = component.getBasesVector(false).getTransformationMatrix();
   CCr5kCompPinReferenceMap compPinMap(component.getCompPins());

   for (POSITION pinPos = toeprintPins.GetHeadPosition();pinPos != NULL;)
   {
      CCr5kToeprintPin* toeprintPin = toeprintPins.GetNext(pinPos);

      CString pinNumber = toeprintPin->getPinNumber();
      CPoint2d origin   = toeprintPin->getPoint().getPoint();

      const CCr5kCompPin* compPin = compPinMap.getAt(pinNumber);

      CString netName = (compPin!=NULL)?compPin->getNetName():NET_UNUSED_PINS;
      if (netName.CompareNoCase(NET_UNUSED_PINS) == 0)
      {
         getCamCadDatabase().getDefinedNet(netName, getCadFile())->setFlags(NETFLAG_UNUSEDNET);
      }

      CCamCadPin* camCadPin = getCamCadDatabase().getDefinedCamCadPin(getCadFile(),refDes,pinNumber,netName);
      CPoint2d boardOrigin(origin);
      boardOrigin.transform(matrix);
      camCadPin->getCompPin()->setOrigin(boardOrigin);
      camCadPin->getCompPin()->setPinCoordinatesComplete(true);
      camCadPin->getCompPin()->setVisible(component.getPlacedTop() ? visibleTop : visibleBottom);
   }

   setDebugRefDesFlag(false);
}

void CZukenCr5000Reader::instantiateToeprintPinData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,
   const CCr5kToeprintPins& toeprintPins,const CCr5kComponent& component)
{
   mappedLayerDataList.setThruFlag(component.isThruHole());
   mappedLayerDataList.setTopFlag(component.getPlacedTop());

   CString refDes = component.getRefDes();
   bool componentPlacedBottomFlag = component.getPlacedBottom();

   CTMatrix componentToBoardMatrix = component.getBasesVector(false).getTransformationMatrix();
   CCr5kCompPinReferenceMap compPinMap(component.getCompPins());

   for (POSITION pinPos = toeprintPins.GetHeadPosition();pinPos != NULL;)
   {
      CCr5kToeprintPin* toeprintPin = toeprintPins.GetNext(pinPos);

      CString pinNumber = toeprintPin->getPinNumber();
      CPoint2d origin   = toeprintPin->getPoint().getPoint();

      const CCr5kCompPin* compPin = compPinMap.getAt(pinNumber);
      CString pinRef = component.getRefDes() + "." + pinNumber;

      CString reference;
      reference.Format("Pin(%s)",pinNumber);

      CTMatrix pinMatrix;
      pinMatrix.translateCtm(origin);
      CBasesVector pinBasesVector;
      pinBasesVector.transform(pinMatrix);
      CString pinBasesVectorDescriptor = pinBasesVector.getDescriptor();

      bool boardLayerFlag = false;
      bool boardLevelFlag = false;
      bool pinFlag = true;

      mappedLayerDataList.startPadstackCache(pinBasesVector,getOptionRestructurePadstacks());
      instantiateLayoutData(mappedLayerDataList, linkedGeometries, toeprintPin->getLayout(), boardLayerFlag, boardLevelFlag, pinFlag, reference, pinRef, false, componentPlacedBottomFlag);
      mappedLayerDataList.instantiatePadstackCache(pinMatrix,componentToBoardMatrix,false,refDes,pinNumber);

      const CCr5kBondwires& bondwires = toeprintPin->getBondwires();
      int materialNumber  = bondwires.getMaterialNumber();
      double wireDiameter = bondwires.getWireDiameterInPageUnits();
      int widthIndex      = getCamCadDatabase().getDefinedWidthIndex(wireDiameter);

      for (POSITION wirePos = bondwires.getHeadPosition();wirePos != NULL;)
      {
         CCr5kBondwire* wire = bondwires.getNext(wirePos);

         CPoint2d startPoint = wire->getStartPoint().getPoint();
         CPoint2d   endPoint = wire->getEndPoint().getPoint();
         bool isGenerated    = wire->isGenerated();

         CString layerName = getBondwireLayerName(component.getPlacedTop(),isGenerated,wireDiameter);
         LayerStruct& ccLayer = getDefinedLayer(layerName,materialNumber);

         CPoly* poly = new CPoly();

         poly->addVertex(startPoint);
         poly->addVertex(  endPoint);
         poly->setWidthIndex(widthIndex);

         DataStruct* polyStruct = new_DataStruct(dataTypePoly);
         polyStruct->setLayerIndex(ccLayer.getLayerIndex());
         polyStruct->getPolyList()->AddTail(poly);

         //polyStruct->transform(matrix);

         setParentReferenceAttribute(*polyStruct,reference);
         setSourceLineNumberAttribute(*polyStruct,*wire);

         mappedLayerDataList.addTailWithoutLayerMapping(polyStruct,ccLayer.getLayerIndex());
      }
   }
}

void CZukenCr5000Reader::instantiateComponentParameters(CAttributes& attributes,const CCr5kComponent& cr5kComponent)
{
   if (m_partKeywordIndex < 0)
   {
      m_partKeywordIndex = getCamCadDatabase().registerKeyword("Part",valueTypeString);
   }

   if (m_packageKeywordIndex < 0)
   {
      m_packageKeywordIndex = getCamCadDatabase().registerKeyword("Package",valueTypeString);
   }

   if (m_stockIdKeywordIndex < 0)
   {
      m_stockIdKeywordIndex = getCamCadDatabase().registerKeyword("StockId",valueTypeString);
   }

   getCamCadDatabase().addAttribute(attributes,m_partKeywordIndex   ,cr5kComponent.getPartNumber());
   getCamCadDatabase().addAttribute(attributes,m_packageKeywordIndex,cr5kComponent.getPackage());
   getCamCadDatabase().addAttribute(attributes,m_stockIdKeywordIndex,cr5kComponent.getStockId());

   int componentHeightKeywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeComponentHeight);
   double componentHeight = cr5kComponent.getComponentHeightInPageUnits();

   getCamCadDatabase().addAttribute(attributes,componentHeightKeywordIndex,componentHeight);
}

void CZukenCr5000Reader::instantiateComponentAttributes(CAttributes& attributes,const CCr5kComponent& cr5kComponent)
{
   // generate matrix to transform board units to geometry units
   CBasesVector componentBasesVector = cr5kComponent.getBasesVector();
   CTMatrix matrix = componentBasesVector.getTransformationMatrix();
   matrix.invert();

   const CCr5kLayout& layout = cr5kComponent.getLayout();
   bool visibleFlag = cr5kComponent.getDrawRefDes();

   instantiateLayoutAttributes(attributes,visibleFlag,cr5kComponent.getPlacedBottom(),layout,matrix);
}

BlockStruct* CZukenCr5000Reader::getTestAccessGeometry()
{
   if (m_testAccessGeometry == NULL)
   {
      m_testAccessGeometry = generate_TestAccessGeometry(&(getCamCadDoc()),"ZukenTestAccess",getOptionTestAccessDiameter());
   }

   return m_testAccessGeometry;
}

BlockStruct* CZukenCr5000Reader::getTestProbeGeometry()
{
   if (m_testProbeGeometry == NULL)
   {
      m_testProbeGeometry = generate_TestProbeGeometry(&(getCamCadDoc()),"ZukenTestProbe",getOptionProbeDiameter(),getOptionProbeDescriptor(),getOptionProbeDrillDiameter());
   }

   return m_testProbeGeometry;
}

BlockStruct* generate_TestAccessGeometry(CCEtoODBDoc *doc, const char *name, double size);
BlockStruct* generate_TestProbeGeometry(CCEtoODBDoc *doc, const char *name, double probediam, 
                                        const char *tool, double drillsize);

BlockStruct* CZukenCr5000Reader::getBoardComponentGeometry(CCr5kLinkedGeometries& linkedGeometries,const CCr5kComponent& component,const CString& geometryName, bool isReverseFootprintProcessed)
{
   BlockStruct* geometry = NULL;

   bool componentPlacedBottomFlag = component.getPlacedBottom();
   CCr5kLinkedFootprint* linkedFootprint = linkedGeometries.getLinkedFootprint(geometryName);

   geometry = getCamCadDatabase().getBlock(geometryName);

   if (geometry == NULL)
   {
      geometry = getCamCadDatabase().getDefinedBlock(geometryName,blockTypePcbComponent,getCadFileNumber());
      if (geometry->getOriginalName().IsEmpty())
         geometry->setOriginalName(component.getFootprintName());

      const CCr5kLayout& layout = component.getLayout();

      double x,y,rotationDegrees;
      bool mirrorFlag;
      component.getBasesComponents(x,y,rotationDegrees,mirrorFlag);
      CBasesVector componentBasesVector;
      componentBasesVector.set(x,y,rotationDegrees);

      CTMatrix matrix = componentBasesVector.getTransformationMatrix();
      matrix.invert();

      CBasesVector rawComponentBasesVector;
      //rawComponentBasesVector = component.getBasesVector();
      rawComponentBasesVector = component.getBasesVector(false);

      // must not use layer mapping for comp pins in board mode
      // must use layer mapping for bare padstack geometries present in the component
      CCr5kMappedLayerDataList mappedDataList(*this,geometry->getDataList(),getLayerMapping(),false);
      mappedDataList.setThruFlag(component.isThruHole());
      mappedDataList.setTopFlag(component.getPlacedTop());
      bool boardLayerFlag = true;
      bool boardLevelFlag = false;
      bool pinFlag = false;

      instantiateLayoutData(mappedDataList,linkedGeometries,layout,boardLayerFlag,boardLevelFlag,pinFlag,"","",true,componentPlacedBottomFlag,&matrix,&rawComponentBasesVector);

#ifdef DEADCODE // makes side-specific centroids which messes up consolidation
      if (linkedFootprint != NULL)
      {
         const CCr5kFootprint& footprint = linkedFootprint->getElement();

         instantiateMounterInformation(mappedDataList,footprint);
      }
#endif
      
      const CCr5kCompPins& compPins = component.getCompPins();

      instantiateComponentPinData(mappedDataList,linkedGeometries,compPins,component);
      if (mirrorFlag && isReverseFootprintProcessed)
      {
         CTMatrix mirrorMatrix;
         mirrorMatrix.mirrorAboutYAxisCtm();

         for (POSITION pos = geometry->getDataList().GetHeadPosition();pos != NULL;)
         {
            DataStruct* data = geometry->getDataList().GetNext(pos); 
            if (data != NULL)
            {
               switch (data->getDataType())
               {
               case dataTypeInsert:
                  {
                     if(insertTypePin != data->getInsert()->getInsertType())
                     {
                        data->getInsert()->transform(mirrorMatrix);
                        CBasesVector basesVector = data->getInsert()->getBasesVector();
                        basesVector.set(basesVector.getOrigin(),basesVector.getRotationDegrees());
                        data->getInsert()->setBasesVector(basesVector);
                     }
                  }
                  break;
               case dataTypePoly:
                  {
                     LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
                     bool hasElectricalMirrorLayer = (layer && layer->isElectrical() && getOptionMirrorElectricalLayers());
                     if(!hasElectricalMirrorLayer)
                        data->getPolyList()->transform(mirrorMatrix,NULL);
                  }
                  break;
               case dataTypeText:
                  {
                     data->getText()->transform(mirrorMatrix);
                     data->getText()->setMirrored(false);
                  }
                  break;
               case dataTypePoint:
                  {
                     data->getPoint()->transform(mirrorMatrix);
                  }
                  break;
               case dataTypeBlob:
                  {
                     data->getBlob()->transform(mirrorMatrix);
                  }
                  break;
               }               
            }
         }        
      }
      //instantiateComponentCompPinData(component);
   }

   return geometry;
}

BlockStruct* CZukenCr5000Reader::getLibraryComponentGeometry(CCr5kLinkedGeometries& linkedGeometries,const CCr5kComponent& component,const CString& geometryName)
{
   BlockStruct* geometry = NULL;

   bool componentPlacedBottomFlag = component.getPlacedBottom();
   CCr5kLinkedFootprint* linkedFootprint = linkedGeometries.getLinkedFootprint(geometryName);

   if (linkedFootprint == NULL)
   {
      getLog().writef("Footprint '%s' referenced in pcf file is missing in ftf file.\n",geometryName);
   }
   else
   {
      bool placedTopFlag = component.getPlacedTop();
      geometry = linkedFootprint->getGeometry(placedTopFlag);

      const CCr5kFootprint&    footprint    = linkedFootprint->getElement();
      const CCr5kHeelprint&    heelprint    = footprint.getHeelprint();
      const CCr5kToeprint&     toeprint     = footprint.getToeprint();
      const CCr5kToeprintPins& toeprintPins = toeprint.getToeprintPins();

      if (geometry == NULL)
      {
         geometry = getCamCadDatabase().getNewBlock(geometryName + (placedTopFlag ? "" : m_cr5kBottomSuffix),"-%d",blockTypePcbComponent,getCadFileNumber());
         geometry->setOriginalName(geometryName);  // Is important to Geometry Consolidator

         CCr5kMappedLayerDataList mappedDataList(*this,geometry->getDataList(),getLayerMapping());
         mappedDataList.setThruFlag(component.isThruHole());
         mappedDataList.setTopFlag(component.getPlacedTop());
         bool boardLayerFlag = false;
         bool boardLevelFlag = false;
         bool pinFlag = false;

         instantiateLayoutData(mappedDataList,linkedGeometries,heelprint.getLayout(),boardLayerFlag,boardLevelFlag,pinFlag,"","",false,componentPlacedBottomFlag);
         instantiateToeprintPinData(mappedDataList,linkedGeometries,toeprintPins,component);
#ifdef DEADCODE // makes side-spcific centroids which messes up consolidation
         instantiateMounterInformation(mappedDataList,footprint);
#endif

         linkedFootprint->setGeometry(placedTopFlag,geometry);

         setProperties(geometry->attributes(),footprint.getProperties());
      }

      //instantiateToeprintCompPinData(toeprintPins,component);
   }

   return geometry;
}

double CZukenCr5000Reader::compareGeometryPins(BlockStruct& geometry1,BlockStruct& geometry2)
{
   double comparisonMetric = 0.;
   
   CTypedMapStringToPtrContainer<DataStruct*> pinMap1(nextPrime2n(200),false);
   CTypedMapStringToPtrContainer<DataStruct*> pinMap2(nextPrime2n(200),false);

   for (CDataListIterator pinIterator(geometry1,insertTypePin);pinIterator.hasNext();)
   {
      DataStruct* pinData = pinIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();

      pinMap1.SetAt(pinName,pinData);
   }

   for (CDataListIterator pinIterator(geometry2,insertTypePin);pinIterator.hasNext();)
   {
      DataStruct* pinData = pinIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();

      pinMap2.SetAt(pinName,pinData);
   }

   CPoint2d pinCentroid = geometry1.getPinCentroid();

   for (CDataListIterator pinIterator(geometry1,insertTypePin);pinIterator.hasNext();)
   {
      DataStruct* pinData = pinIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();
      CPoint2d pinOrigin = pinData->getInsert()->getOrigin2d();

      DataStruct* otherPinData;
      CPoint2d otherPinOrigin;

      if (pinMap2.Lookup(pinName,otherPinData))
      {
         otherPinOrigin = otherPinData->getInsert()->getOrigin2d();
      }
      else
      {
         otherPinOrigin = pinCentroid;
      }

      double distance = pinOrigin.distance(otherPinOrigin);

      comparisonMetric += distance;
   }

   pinCentroid = geometry2.getPinCentroid();

   for (CDataListIterator pinIterator(geometry2,insertTypePin);pinIterator.hasNext();)
   {
      DataStruct* pinData = pinIterator.getNext();
      CString pinName = pinData->getInsert()->getRefname();
      CPoint2d pinOrigin = pinData->getInsert()->getOrigin2d();

      DataStruct* otherPinData;
      CPoint2d otherPinOrigin;

      if (pinMap1.Lookup(pinName,otherPinData))
      {
         otherPinOrigin = otherPinData->getInsert()->getOrigin2d();
      }
      else
      {
         otherPinOrigin = pinCentroid;
      }

      double distance = pinOrigin.distance(otherPinOrigin);

      comparisonMetric += distance;
   }

   return comparisonMetric;
}

BlockStruct* CZukenCr5000Reader::getInstantiatedComponentGeometry(CCr5kLinkedGeometries& linkedGeometries,const CCr5kComponent& component)
{
   BlockStruct* geometry = NULL;
   CString refDes = component.getRefDes();
   CString geometryName = component.getFootprintName();
   CString reverseGeometryName = component.getReverseFootprintName();
   bool reverseFootprintProcessedFlag = false;

   if (getOptionEnableReverseFootprints() && !reverseGeometryName.IsEmpty())
   {
      CCr5kLinkedFootprint* linkedFootprint        = linkedGeometries.getLinkedFootprint(geometryName);
      CCr5kLinkedFootprint* linkedReverseFootprint = linkedGeometries.getLinkedFootprint(reverseGeometryName);

      if (linkedFootprint != NULL && linkedReverseFootprint != NULL)
      {
         CString boardGeometryName = "BoardGeometry-" + geometryName + "-" + refDes;

         BlockStruct* boardGeometry     = getBoardComponentGeometry(linkedGeometries,component,boardGeometryName);
         BlockStruct* footprintGeometry = getLibraryComponentGeometry(linkedGeometries,component,geometryName);
         BlockStruct* reverseGeometry   = getLibraryComponentGeometry(linkedGeometries,component,reverseGeometryName);

         if (boardGeometry != NULL && footprintGeometry != NULL && reverseGeometry != NULL)
         {
            reverseFootprintProcessedFlag = true;

            double footprintComparisonMetric = compareGeometryPins(*boardGeometry,*footprintGeometry);
            double reverseComparisonMetric   = compareGeometryPins(*boardGeometry,*reverseGeometry);
            geometry = footprintGeometry;

            if (footprintComparisonMetric > reverseComparisonMetric)
            {
               geometryName    = reverseGeometryName;
               geometry        = reverseGeometry;
               linkedFootprint = linkedReverseFootprint;
            }

            if (getOptionInstanceSpecificMode())
            {
               geometry = boardGeometry;

               geometryName += "-" + refDes;
               geometry->setName(geometryName);

               instantiateComponentCompPinData(component);
            }
            else
            {
               const CCr5kFootprint&    footprint    = linkedFootprint->getElement();
               const CCr5kToeprint&     toeprint     = footprint.getToeprint();
               const CCr5kToeprintPins& toeprintPins = toeprint.getToeprintPins();

               instantiateToeprintCompPinData(toeprintPins,component);
            }
         }
      }
   }

   if (!reverseFootprintProcessedFlag)
   {
      CCr5kLinkedFootprint* linkedFootprint = linkedGeometries.getLinkedFootprint(geometryName);

      if (getOptionInstanceSpecificMode())
      {
         geometryName += "-" + refDes;
         geometry = getBoardComponentGeometry(linkedGeometries,component,geometryName);

         instantiateComponentCompPinData(component);
      }
      else
      {
         geometry = getLibraryComponentGeometry(linkedGeometries,component,geometryName);

         if (linkedFootprint != NULL)
         {
            const CCr5kFootprint&    footprint    = linkedFootprint->getElement();
            const CCr5kToeprint&     toeprint     = footprint.getToeprint();
            const CCr5kToeprintPins& toeprintPins = toeprint.getToeprintPins();

            instantiateToeprintCompPinData(toeprintPins,component);
         }
      }
   }

   if (geometry != NULL)
   {
      CString deviceTypeName = component.getPartNumber();

      if (! deviceTypeName.IsEmpty())
      {
         TypeStruct* typeStruct = getCamCadDatabase().getDefinedType(deviceTypeName,getCadFile());

         if (typeStruct->getBlockNumber() > 0 && typeStruct->getBlockNumber() != geometry->getBlockNumber())
         {
            deviceTypeName.AppendFormat("[%d]",geometry->getBlockNumber());

            typeStruct = getCamCadDatabase().getDefinedType(deviceTypeName,getCadFile());
         }

         typeStruct->setBlockNumber(geometry->getBlockNumber());
      }
   }

   return geometry;
}

//CCr5kLinkedGeometries& CZukenCr5000Reader::getLinkedPanelGeometries()
//{
//   CCr5kLinkedGeometries* linkedPanelGeometries = m_linkedPanelGeometries;
//
//   if (linkedPanelGeometries == NULL)
//   {
//      linkedPanelGeometries = &m_linkedBoardGeometries;
//   }
//
//   return *linkedPanelGeometries;
//}

void CZukenCr5000Reader::initializeInstantiator()
{
   bool panelFootprintFileFlag = m_panelFootprintFile.isLoaded();
   CCr5kFootprintFile& panelFootprintFile = (panelFootprintFileFlag ? m_panelFootprintFile : m_boardFootprintFile);

   m_linkedBoardGeometries.initialize(m_boardFile,m_boardFootprintFile);
   m_linkedPanelGeometries.initialize(m_panelFile,  panelFootprintFile);

   //COperationProgress progress;
   //progress.updateStatus("Linking components",linkCount);

   m_linkedBoardComponents.empty();
   m_boardFile.linkComponents(m_linkedBoardComponents,NULL);

   m_linkedPanelComponents.empty();
   m_panelFile.linkComponents(m_linkedPanelComponents,NULL);

   m_boardFile.generateLayerLinkMaps(m_layerLinkMaps);

   // debug 
   //m_layerLinkMaps.dump(CDebugWriteFormat::getWriteFormat());
}

bool CZukenCr5000Reader::instantiateBoardData()
{
   m_cadFile = Graph_File_Start(m_projectName,fileTypeCr5000Layout);
   m_cadFile->setBlockType(blockTypePcb);
   m_pcbFile = m_cadFile;

   bool retval = instantiateData(m_boardFile,m_linkedBoardComponents,m_linkedBoardGeometries,false);

   m_subBoardGeometry = m_cadFile->getBlock();

   return retval;
}

bool CZukenCr5000Reader::instantiatePanelData()
{
   m_cadFile = Graph_File_Start(m_projectName + "-panel",fileTypeCr5000Layout);
   m_cadFile->setShow(true);
   m_cadFile->setBlockType(blockTypePanel);
   m_panFile = m_cadFile;

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (NULL != pos)
   {      
      FileStruct *file = doc->getFileList().GetNext(pos);
      if(NULL == file)
         continue;
      
      if(file->getBlockType() != BLOCKTYPE_PANEL)
      {
         file->setShow(false);
      }      
   }
   return instantiateData(m_panelFile,m_linkedPanelComponents,m_linkedPanelGeometries,true);
}

bool CZukenCr5000Reader::instantiateData(CCr5kBoardFile& boardFile,CCr5kLinkedComponents& linkedComponents,CCr5kLinkedGeometries& linkedGeometries,bool panelDataFlag)
{
   bool retval = true;

   m_panelDataFlag = panelDataFlag;

   // layer mapping
   const CCr5kTechnologyContainer& technologyContainer = boardFile.getTechnologyContainer();

   m_layerMapping.set(technologyContainer,getLog());

   //if (technologyContainer.getCount() > 0)
   //{
   //   CCr5kTechnology* technology = technologyContainer.getHead();
   //   m_layerMapping.set(technology->getLayerMaps());
   //}

   COperationProgress progress;

   // board level graphics
   bool componentPlacedBottomFlag = false;
   bool boardLayerFlag = true;
   bool boardLevelFlag = true;
   bool pinFlag = false;
   CCr5kMappedLayerDataList mappedLayerDataList(*this,m_cadFile->getBlock()->getDataList());
   instantiateLayoutData(mappedLayerDataList,linkedGeometries,boardFile.getBoardContainer().getBoardLayout().getLayout(),boardLayerFlag,boardLevelFlag,pinFlag,
      "","",true,componentPlacedBottomFlag,NULL,NULL,&progress);

   progress.updateStatus("Instantiating components.",linkedComponents.getCount());

   int loadedKeywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeLoaded);

   // process components
   for (POSITION pos = linkedComponents.getHeadPosition();pos != NULL;)
   {
      progress.incrementProgress();

      CTypedCr5kLinkedElementToData<CCr5kComponent>* linkedComponent = linkedComponents.getNext(pos);

      CCr5kComponent& cr5kComponent = linkedComponent->getElement();

      if (getOptionIgnoreOutOfBoard() && cr5kComponent.getOutOfBoard())
      {
         getLog().writef("Out of board component, '%s', ignored.\n",cr5kComponent.getDescriptor());

         continue;
      }

      CString refDes = cr5kComponent.getRefDes();

      //
      // When debugging, this is the spot to check for comp refname you are after.
      //

      BlockStruct* geometry = getInstantiatedComponentGeometry(linkedGeometries,cr5kComponent);

      if (geometry != NULL)
      {
         CBasesVector basesVector = cr5kComponent.getBasesVector();
         bool mirrorFlag = basesVector.getMirror();

         DataStruct* ccComponent = getCamCadDatabase().insertBlock(geometry,insertTypePcbComponent,refDes,-1);
         ccComponent->getInsert()->setBasesVector(basesVector);
         ccComponent->getInsert()->setPlacedBottom(mirrorFlag);
         ccComponent->getInsert()->setLayerMirrorFlag(mirrorFlag);

         setProperties(ccComponent->attributes(),cr5kComponent.getProperties());
         instantiateComponentParameters(ccComponent->attributes(),cr5kComponent);
         instantiateComponentAttributes(ccComponent->attributes(),cr5kComponent);

         if (getOptionGenerateLoadedAttribute())
         {
            CString loadedValue = (cr5kComponent.getPlaced() ? "TRUE" : "FALSE" );

            getCamCadDatabase().addAttribute(ccComponent->attributes(),loadedKeywordIndex,loadedValue);
         }

         m_cadFile->getBlock()->getDataList().AddTail(ccComponent);

         setSourceLineNumberAttribute(*ccComponent,cr5kComponent);
      }

      //setDebugRefDesFlag(false);
   }

   makeIntoCorrectTypes();
   mirrorLayers();
   typeLayers();
   makeBoardOutline(*m_cadFile,panelDataFlag);

   PostProcessComponents();

   return retval;
}

void CZukenCr5000Reader::PostProcessComponents()
{
   // DR 810019 - Look elsewhere for this DR number, I tried to make it so it did the right
   // thing up front, but did get it to work. Too much time invested in that. So instead
   // we'll do this little post-process that will promote certain polys from component
   // geometry to pads in padstacks. It is based on the ParentReference attrib being
   // set to "Pin(nn)" where nn is the pin refname. Get these polys, make complex
   // apertures for them, then insert the aperture in the pin's padstack.

   CCamCadData &ccData = this->getCamCadDatabase().getCamCadData();
   for (int indx = 0; indx < ccData.getMaxBlockIndex(); indx++)
   {
      BlockStruct *blk = ccData.getBlockAt(indx);
      if (blk != NULL && blk->getBlockType() == blockTypePcbComponent)
      {
         PostProcessComponent(blk);
      }
   }
}

void CZukenCr5000Reader::PostProcessComponent(BlockStruct *compBlk)
{
   if (compBlk != NULL)
   {
      // Just to make sure we don't get an infinite loop.
      int loopCounter = 0;
      int loopLimit = 100000;

      CString parentReference;
      DataStruct *originalPolyStruct = GetPinPolyData(compBlk, parentReference);
      while (originalPolyStruct != NULL && (++loopCounter < loopLimit))
      {
         // process this one.
         CString pinRefname( parentReference );
         pinRefname.Replace("Pin(", "");
         pinRefname.Replace(")", "");
         pinRefname.Trim();

         // Make a copy to use  for complex aperture. Set it to use floating layer.
         DataStruct *newPolyStruct = this->m_camCadDatabase.getCamCadData().getNewDataStruct(*originalPolyStruct, true);
         newPolyStruct->setLayerIndex( this->m_camCadDatabase.getFloatingLayerIndex() );

         int polyEntityNum = newPolyStruct->getEntityNumber();

         DataStruct *pinInsertData = compBlk->FindInsertData(pinRefname, insertTypePin);



         if (pinInsertData != NULL)
         {
            // Poly is relative to component origin, shift it to be relative to pin.
            CTMatrix pinMatrix = pinInsertData->getInsert()->getTMatrix();
            pinMatrix.invert();
            newPolyStruct->transform(pinMatrix);

            BlockStruct *insertedPadstackBlock = this->getCamCadData().getBlockAt(pinInsertData->getInsert()->getBlockNumber());

            // DR 892646 - We need to make a copy of the padstack before adding polys to it. Other pins in this or in other
            // components may use same original padstack, and we do not want the polys to show up in all of those too.
            // We want to make the copy only once, so we'll alter the geometry name so we can tell if this is already done.
            if (insertedPadstackBlock != NULL)
            {
               CString padstackNameModifier = "_" + compBlk->getName() + "_" + parentReference; 
               if (insertedPadstackBlock->getName().Find(padstackNameModifier) < 0)
               {
                  // Is original padstack, make a copy, update the insert.
                  CString newPadstackName = insertedPadstackBlock->getName() + padstackNameModifier;
                  
                  BlockStruct *newPadstackBlock = this->getCamCadDatabase().copyBlock(newPadstackName, insertedPadstackBlock, true);
                  pinInsertData->getInsert()->setBlockNumber(newPadstackBlock->getBlockNumber());
                  insertedPadstackBlock = newPadstackBlock;
               }
               // else is already a copy so we're okay to modify it.
            }

            if (insertedPadstackBlock != NULL)
            {
               // The complex aperture geometry block that contains the poly.
               CString apBlkName;
               apBlkName.Format("AP_GEOM_%d", polyEntityNum);
               BlockStruct *apGeomBlk = this->getCamCadDatabase().getDefinedBlock(apBlkName, blockTypeUnknown);
               apGeomBlk->getDataList().AddHead(newPolyStruct);

               // The complex aperture that is the pad geometry.
               CString apName;
               apName.Format("AP_%d", polyEntityNum);
               BlockStruct* padAperture = this->getCamCadDatabase().getDefinedAperture(apName, apertureComplex, apGeomBlk->getBlockNumber());

               // Get the layer.
               int originalLayerIndx = originalPolyStruct->getLayerIndex();
               LayerStruct *originalLayer = this->m_camCadDatabase.getLayerAt(originalLayerIndx);
               int layerIndx = (originalLayer->isElectricalTop()) ? originalLayerIndx : (originalLayer->getMirroredLayerIndex());

               // Add it to the padstack.
               this->getCamCadDatabase().referenceBlock(insertedPadstackBlock, padAperture, insertTypeUnknown, "", layerIndx, 0.0, 0.0, 0.0, false);
            }
         }

         // remove this one from compBlk.
         bool removed = this->m_camCadDoc.removeDataFromDataList(*compBlk, originalPolyStruct);

         // get possible next one.
         originalPolyStruct = GetPinPolyData(compBlk, parentReference);
      }
   }
}

DataStruct *CZukenCr5000Reader::GetPinPolyData(BlockStruct *compBlk, CString &outParentReference)
{
   // Return first poly data that has "Pin(nn)" attrib value in ParentReference.
   // Only interested in polys on electrical layers.

   if (compBlk != NULL)
   {
      POSITION dataPos = compBlk->getHeadDataPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = compBlk->getNextData(dataPos);
         if (data != NULL && data->getDataType() == dataTypePoly)
         {
            LayerStruct *layer = this->m_camCadDatabase.getLayerAt(data->getLayerIndex());
            if (layer && layer->isElectrical())
            {
               Attrib *attrib;
               if (data->lookUpAttrib(getParentReferenceKeywordIndex(), attrib))
               {
                  outParentReference = attrib->getStringValue();
                  if (outParentReference.Find("Pin(") > -1)
                  {
                     return data;
                  }
               }
            }
         }
      }
   }
   return NULL;
}

void CZukenCr5000Reader::setProperties(CAttributes& attributes,const CCr5kProperties& properties)
{
   CCr5kProperty* property = NULL;
   CAttributes* pAttributes = &attributes;
   CString key;

   for (POSITION pos = properties.getStartPosition();pos != NULL;)
   {
      properties.getNextAssoc(pos,key,property);

      CString attributeName  = property->getName();
      CString attributeValue = property->getValue();

      getCamCadDoc().SetUnknownAttrib(&pAttributes,attributeName,attributeValue,SA_OVERWRITE,NULL);
   }
}

void CZukenCr5000Reader::makeBoardOutline(FileStruct& fileStruct,bool panelDataFlag)
{
   BlockStruct& boardBlock = *(fileStruct.getBlock());
   //LayerTypeTag boardOutlineLayerType       = (panelDataFlag ? layerTypePanelOutline    : layerTypeBoardOutline);
   LayerTypeTag boardOutlineLayerType       = layerTypeBoardOutline;
   GraphicClassTag boardOutlineGraphicClass = (panelDataFlag ? graphicClassPanelOutline : graphicClassBoardOutline);
   int polyCount = 0;

   for (CDataListIterator polyIterator(boardBlock,dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* polyData = polyIterator.getNext();
      LayerStruct* layer = getCamCadData().getLayer(polyData->getLayerIndex());

      if (layer->getLayerType() == boardOutlineLayerType)
      {
         GraphicClassTag graphicClass = ((polyCount == 0) ? boardOutlineGraphicClass : graphicClassBoardGeometry);

         polyData->setGraphicClass(graphicClass);

         polyCount++;
      }
   }
}

void CZukenCr5000Reader::makeIntoCorrectTypes()
{
   for (int blockIndex = m_startingBlockIndex;blockIndex < getCamCadDoc().getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* geometry = getCamCadDoc().getBlockAt(blockIndex);

      if (geometry != NULL)
      {
         CString geometryName = geometry->getName();

         CString name(geometryName);
         name.MakeLower();

         //if (name.Find("clp") == 0)
         //{
         //   int iii = 3;
         //}

         if (m_optionFiducialExpressionList.isStringMatched(geometryName))
         {
            geometry->setBlockType(blockTypeFiducial);
            getLog().writef(PrefixStatus,"Block type set to Fiducial for geometry '%s'\n",geometryName);
         }
         else if (m_optionTestPointExpressionList.isStringMatched(geometryName))
         {
            geometry->setBlockType(blockTypeTestPoint);
            getLog().writef(PrefixStatus,"Block type set to TestPoint for geometry '%s'\n",geometryName);
         }
         else if (m_optionToolingExpressionList.isStringMatched(geometryName))
         {
            geometry->setBlockType(blockTypeTooling);
            getLog().writef(PrefixStatus,"Block type set to Tooling for geometry '%s'\n",geometryName);
         }

         for (CDataListIterator polyIterator(*geometry,dataTypePoly);polyIterator.hasNext();)
         {
            DataStruct* polyStruct = polyIterator.getNext();
            int layerIndex = polyStruct->getLayerIndex();

            if (layerIndex >= 0)
            {
               LayerStruct* layer = getCamCadDoc().getLayer(layerIndex);
               CString layerName = layer->getName();

               if (m_optionCompOutlineExpressionList.isStringMatched(layerName))
               {
                  layer->setLayerType(layerTypeComponentOutline);

                  polyStruct->setGraphicClass(graphicClassComponentOutline);

                  for (POSITION polyPos = polyStruct->getPolyList()->GetHeadPosition();polyPos != NULL;)
                  {
                     CPoly* poly =  polyStruct->getPolyList()->GetNext(polyPos);
                     poly->setFilled(false);
                  }
               }
            }
         }
      }
   }

   for (CDataListIterator insertIterator(*(m_cadFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();
      BlockStruct* geometry = getCamCadDoc().getBlockAt(data->getInsert()->getBlockNumber());

      if (geometry->getBlockType() == blockTypeFiducial)
      {
         data->getInsert()->setInsertType(insertTypeFiducial);
      }
      else if (geometry->getBlockType() == blockTypeTestPoint)
      {
         data->getInsert()->setInsertType(insertTypeTestPoint);
      }
      else if (geometry->getBlockType() == blockTypeTooling)
      {
         data->getInsert()->setInsertType(insertTypeDrillTool);
      }
   }
}

void CZukenCr5000Reader::performLayerMirroring()
{
   CZukenLayerMirrorer layerMirrorer(*this);

   layerMirrorer.performLayerMirroring();
}

CString CZukenCr5000Reader::getMaterialLayerName(const CString& layerName,const CString& materialName)
{
   CString materialLayerName = layerName + ":" + materialName;

   return materialLayerName;
}

//_____________________________________________________________________________
CZukenMapBlockNumberToBlock::CZukenMapBlockNumberToBlock(int hashSize)
: m_blockMap(hashSize,false)
{
}

void CZukenMapBlockNumberToBlock::addBlock(BlockStruct* block)
{
   m_blockMap.SetAt(block->getBlockNumber(),block);
}

bool CZukenMapBlockNumberToBlock::containsBlock(BlockStruct* block) const
{
   BlockStruct* foundBlock;

   bool retval = (m_blockMap.Lookup(block->getBlockNumber(),foundBlock) != 0);

   return retval;
}

POSITION CZukenMapBlockNumberToBlock::getStartPos() const
{
   return m_blockMap.GetStartPosition();
}

BlockStruct* CZukenMapBlockNumberToBlock::getNext(POSITION& pos) const
{
   int blockNumber;
   BlockStruct* block;

   m_blockMap.GetNextAssoc(pos,blockNumber,block);

   return block;
}

//_____________________________________________________________________________
CZukenLayerMirrorer::CZukenLayerMirrorer(CZukenCr5000Reader& zukenReader)
: m_zukenReader(zukenReader)
{
   m_hashSize = nextPrime2n(getZukenReader().getCamCadDatabase().getCamCadDoc().getMaxBlockIndex() / 2);

   m_mirroredGeometries        = new CZukenMapBlockNumberToBlock(m_hashSize);
   m_unmirroredGeometries      = new CZukenMapBlockNumberToBlock(m_hashSize);
   m_tier1MirroredGeometries   = new CZukenMapBlockNumberToBlock(m_hashSize);
   m_tier1UnmirroredGeometries = new CZukenMapBlockNumberToBlock(m_hashSize);
   m_tier2MirroredGeometries   = new CZukenMapBlockNumberToBlock(m_hashSize);
   m_tier2UnmirroredGeometries = new CZukenMapBlockNumberToBlock(m_hashSize);
}

CZukenLayerMirrorer::~CZukenLayerMirrorer()
{
   delete m_mirroredGeometries;
   delete m_unmirroredGeometries;
   delete m_tier1MirroredGeometries;
   delete m_tier1UnmirroredGeometries;
   delete m_tier2MirroredGeometries;
   delete m_tier2UnmirroredGeometries;
}

void CZukenLayerMirrorer::performLayerMirroring()
{
   mirrorLayers();

   if (getZukenReader().getPcbFile() != NULL)
   {
      scanTier1Geometries(*(getZukenReader().getPcbFile()));
   }

   if (getZukenReader().getPanFile() != NULL)
   {
      scanTier1Geometries(*(getZukenReader().getPanFile()));
   }

   scanTier2Geometries();
   checkGeometryUsage();
   mirrorGeometryData();
}

void CZukenLayerMirrorer::mirrorLayers()
{
   const CLayerArray& layerArray       = getZukenReader().getCamCadDatabase().getCamCadDoc().getLayerArray();
   CCr5kLayerLinkMaps& layerLinkMaps   = getZukenReader().getLayerLinkMaps();
   CCr5kLayerDirectory& layerDirectory = layerLinkMaps.getLayerDirectory();
   int maximumConductiveLayerNumber    = layerLinkMaps.getMaximumConductiveLayerNumber();

   getZukenReader().getLog().writef("\n" "Mirroring electrical layers:\n");

   // mirror the electrical layers
   if (getZukenReader().getOptionMirrorElectricalLayers())
   {
      for (int conductiveLayerNumber = 1;conductiveLayerNumber <= maximumConductiveLayerNumber;conductiveLayerNumber++)
      {
         int mirroredConductiveLayerNumber = maximumConductiveLayerNumber + 1 - conductiveLayerNumber;
         CString condLayerBaseame, mirroredCondLayerBasename;

         condLayerBaseame.Format("%d", conductiveLayerNumber);
         mirroredCondLayerBasename.Format("%d",mirroredConductiveLayerNumber);

         // Get list of layer entries with this basename.
         CCr5kLayerDirectoryEntryList baseLayerNameList;
         layerDirectory.getBaseLayerNameList(baseLayerNameList, condLayerBaseame);

         for (POSITION pos = baseLayerNameList.getHeadPosition();pos != NULL;)
         {
            // The layer in hand we want to get a mirror for...
            CCr5kLayerDirectoryEntry* layerEntry = baseLayerNameList.getNext(pos);
            LayerStruct& layer         = layerEntry->getLayer();

            // The properly named mirror layer. Note we make this even if mirroring fails.
            LayerStruct& mirroredLayer = getZukenReader().getDefinedLayer(mirroredCondLayerBasename, layerEntry->getMaterialNumber());

            // Possibly they are already mirrored to each other.
            bool alreadyMirrored = 
               (layer.getMirroredLayerIndex() == mirroredLayer.getLayerIndex()) &&
               (mirroredLayer.getMirroredLayerIndex() == layer.getLayerIndex());

            if (! layer.hasMirroredLayer())
            {
               // If selected mirrorLayer is not mirrored yet, then set up the mirror.
               // Else if already mirrored but to something else, log an error.
               // If already set up as desired mirror, need to nothing.
               if (! mirroredLayer.hasMirroredLayer())
               {
                  // Neither were mirrored so go ahead and mirror them.
                  mirroredLayer.setElectricalStackNumber(mirroredConductiveLayerNumber);
                  layer.mirrorWithLayer(mirroredLayer);

                  getZukenReader().getLog().writef("Mirrored layers '%s' and '%s'\n",
                     layer.getName(), mirroredLayer.getName());
               }
               else if (!alreadyMirrored)
               {
                  // Error, the mirroredLayer is already mirrored but to something else.
                  LayerStruct* otherMirroredLayer = getZukenReader().getCamCadDatabase().getLayerAt(mirroredLayer.getMirroredLayerIndex());

                  getZukenReader().getLog().writef("Could not mirror layer pair '%s' and '%s' because desired mirror layer '%s' is already mirrored with '%s'\n",
                     layer.getName(), mirroredLayer.getName(), mirroredLayer.getName(), otherMirroredLayer->getName());
               }
            }
            else if (!alreadyMirrored)
            {
               LayerStruct* otherMirroredLayer = getZukenReader().getCamCadDatabase().getLayerAt(layer.getMirroredLayerIndex());

               getZukenReader().getLog().writef("Could not mirror layer pair '%s' and '%s' because '%s' is already mirrored with '%s'\n",
                  layer.getName(), mirroredLayer.getName(), layer.getName(), otherMirroredLayer->getName());
            }
         }
      }
   }

   getZukenReader().getLog().writef("\nMirroring layers from cr5k.in:\n");

   CCr5kMirroredLayers& mirroredLayers = getZukenReader().getMirroredLayers();

   if (mirroredLayers.getCount() < 1)
   {
      getZukenReader().getLog().writef("None found.\n");
   }

   for (POSITION mirroredLayerExpressionPos = mirroredLayers.getHeadPosition();mirroredLayerExpressionPos != NULL;)
   {
      CCr5kMirroredLayerEntry* mirroredLayerEntry = mirroredLayers.getNext(mirroredLayerExpressionPos);
      CString expression1 = mirroredLayerEntry->getLayerExpression1();
      CString expression2 = mirroredLayerEntry->getLayerExpression2();
      bool baseLayerFlag  = mirroredLayerEntry->getBaseLayerFlag();

      getZukenReader().getLog().writef("\nProcessing .in file command '%s'\n",mirroredLayerEntry->getCommandString());

      CCr5kLayerDirectoryEntryList nameList;

      if (baseLayerFlag)
      {
         layerDirectory.getMatchingBaseLayerNameList(nameList,expression1);

         for (POSITION pos = nameList.getHeadPosition();pos != NULL;)
         {
            CCr5kLayerDirectoryEntry* layerEntry = nameList.getNext(pos);

            LayerStruct& layer = layerEntry->getLayer();

            if (! layer.hasMirroredLayer())
            {
               CCr5kRegularExpression layerRegularExpression(expression1,expression2,true);

               if (layerRegularExpression.matches(layerEntry->getBaseLayerName()))
               {
                  CString matchingBaseLayerName = layerRegularExpression.getSubstitution();

                  LayerStruct& mirroredLayer = getZukenReader().getDefinedLayer(matchingBaseLayerName,layerEntry->getMaterialNumber());

                  if (! mirroredLayer.hasMirroredLayer())
                  {
                     layer.mirrorWithLayer(mirroredLayer);

                     mirroredLayer.setLayerType(getOppositeSideLayerType(layer.getLayerType()));

                     getZukenReader().getLog().writef("Mirrored layers '%s' and '%s'\n",
                         layer.getName(),mirroredLayer.getName());
                  }
                  else
                  {
                     LayerStruct* otherMirroredLayer = getZukenReader().getCamCadDatabase().getLayerAt(mirroredLayer.getMirroredLayerIndex());

                     getZukenReader().getLog().writef("Could not mirror layer pair '%s' and '%s' because '%s' is already mirrored with '%s'\n",
                        layer.getName(),mirroredLayer.getName(),mirroredLayer.getName(),otherMirroredLayer->getName());
                  }
               }
            }
            else
            {
               LayerStruct* otherMirroredLayer = getZukenReader().getCamCadDatabase().getLayerAt(layer.getMirroredLayerIndex());

               getZukenReader().getLog().writef("Layer '%s' previousely mirrored with '%s'\n",
                  layer.getName(),otherMirroredLayer->getName());
            }
         }
      }
      else
      {
         layerDirectory.getMatchingLayerNameList(nameList,expression1);

         for (POSITION pos = nameList.getHeadPosition();pos != NULL;)
         {
            CCr5kLayerDirectoryEntry* layerEntry = nameList.getNext(pos);

            LayerStruct& layer = layerEntry->getLayer();

            if (! layer.hasMirroredLayer())
            {
               CCr5kRegularExpression layerRegularExpression(expression1,expression2,true);

               if (layerRegularExpression.matches(layerEntry->getLayerName()))
               {
                  CString matchingLayerName = layerRegularExpression.getSubstitution();

                  LayerStruct& mirroredLayer = getZukenReader().getDefinedLayer(matchingLayerName);

                  if (! mirroredLayer.hasMirroredLayer())
                  {
                     layer.mirrorWithLayer(mirroredLayer);

                     mirroredLayer.setLayerType(getOppositeSideLayerType(layer.getLayerType()));

                     getZukenReader().getLog().writef("Mirrored layers '%s' and '%s'\n",
                         layer.getName(),mirroredLayer.getName());
                  }
                  else
                  {
                     LayerStruct* otherMirroredLayer = getZukenReader().getCamCadDatabase().getLayerAt(mirroredLayer.getMirroredLayerIndex());

                     getZukenReader().getLog().writef("Could not mirror layer pair '%s' and '%s' because '%s' is already mirrored with '%s'\n",
                        layer.getName(),mirroredLayer.getName(),mirroredLayer.getName(),otherMirroredLayer->getName());
                  }
               }
            }
            else
            {
               LayerStruct* otherMirroredLayer = getZukenReader().getCamCadDatabase().getLayerAt(layer.getMirroredLayerIndex());

               getZukenReader().getLog().writef("Layer '%s' previousely mirrored with '%s'\n",
                  layer.getName(),otherMirroredLayer->getName());
            }
         }
      }
   }
}

void CZukenLayerMirrorer::scanTier1Geometries(FileStruct& fileStruct)
{
   BlockStruct* fileBlock = fileStruct.getBlock();

   getZukenReader().getLog().writef("\n" "Scanning Tier1 geometries for fileStruct %d:\n",fileStruct.getFileNumber());

   scanGeometry(*fileBlock,*m_tier1MirroredGeometries,*m_tier1UnmirroredGeometries,false);
}

void CZukenLayerMirrorer::scanTier2Geometries()
{
   getZukenReader().getLog().writef("\n" "Scanning Tier2 geometries:\n");

   for (POSITION pos = m_tier1UnmirroredGeometries->getStartPos();pos != NULL;)
   {
      BlockStruct* tier1Geometry = m_tier1UnmirroredGeometries->getNext(pos);

      getZukenReader().getLog().writef("\n" "Scanning tier1 unmirrored Geometry %d '%s' for tier2 geometries:\n",
         tier1Geometry->getBlockNumber(),tier1Geometry->getName());

      scanGeometry(*tier1Geometry,*m_tier2MirroredGeometries,*m_tier2UnmirroredGeometries,false);
   }

   for (POSITION pos = m_tier1MirroredGeometries->getStartPos();pos != NULL;)
   {
      BlockStruct* tier1Geometry = m_tier1MirroredGeometries->getNext(pos);

      getZukenReader().getLog().writef("\n" "Scanning tier1 mirrored Geometry %d '%s' for tier2 geometries:\n",
         tier1Geometry->getBlockNumber(),tier1Geometry->getName());

      scanGeometry(*tier1Geometry,*m_tier2MirroredGeometries,*m_tier2UnmirroredGeometries,true);
   }
}

void CZukenLayerMirrorer::scanGeometry(BlockStruct& geometry,CZukenMapBlockNumberToBlock& mirroredGeometries,CZukenMapBlockNumberToBlock& unmirroredGeometries,bool mirrorFlag)
{
   BlockStruct* testAccessGeometry = getZukenReader().getTestAccessGeometry();
   BlockStruct* testProbeGeometry  = getZukenReader().getTestProbeGeometry();

   for (CDataListIterator insertIterator(geometry,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* insertData = insertIterator.getNext();
      InsertStruct* insert = insertData->getInsert();
      BlockStruct* insertedGeometry = getCamCadDatabase().getBlock(insert->getBlockNumber());

      if (insertedGeometry == testAccessGeometry || insertedGeometry == testProbeGeometry)
      {
         continue;
      }

      if (insert->getInsertType() == insertTypePcb)
      {
         continue;
      }

      //  placedBottom  mirrorFLag   mirroredFlag
      //        n           n             n
      //        n           y             y
      //        y           n             y
      //        y           y             n

      bool mirroredFlag = (insert->getPlacedBottom() != mirrorFlag);

      if (! mirroredFlag)
      {
         unmirroredGeometries.addBlock(insertedGeometry);
         m_unmirroredGeometries->addBlock(insertedGeometry);

         getZukenReader().getLog().writef("Found geometry instance '%s' [%d] for Geometry %d '%s'\n",
            insert->getRefname(), insertData->getEntityNumber(), insertedGeometry->getBlockNumber(),insertedGeometry->getName());
      }
      else
      {
         mirroredGeometries.addBlock(insertedGeometry);
         m_mirroredGeometries->addBlock(insertedGeometry);

         getZukenReader().getLog().writef("Found layer mirrored geometry instance '%s' [%d] for Geometry %d '%s'\n",
            insert->getRefname(), insertData->getEntityNumber(), insertedGeometry->getBlockNumber(),insertedGeometry->getName());
      }
   }
}

void CZukenLayerMirrorer::checkGeometryUsage()
{
   int errorCount = 0;

   getZukenReader().getLog().writef("\n" "Checking geometry usage.\n");

   // check if any top placed geometries instances are also bottom placed
   for (POSITION pos = m_unmirroredGeometries->getStartPos();pos != NULL;)
   {
      BlockStruct* unmirroredGeometry = m_unmirroredGeometries->getNext(pos);

      if (m_mirroredGeometries->containsBlock(unmirroredGeometry))
      {
         errorCount++;

         //formatMessageBox("found bad geometry usage in %s",__FUNCTION__);

         getZukenReader().getLog().writef("\n" "Geometry %d '%s' used as both mirrored and unmirrored.\n",
            unmirroredGeometry->getBlockNumber(),unmirroredGeometry->getName());
      }
   }
}

void CZukenLayerMirrorer::mirrorInsertAttributes(BlockStruct *insertedBlock, BlockStruct *datalistBlock)
{
   if (datalistBlock != NULL)
   {
      int lookingForInsertBlkNum = insertedBlock->getBlockNumber();

      for (CDataListIterator insertIterator(*datalistBlock, dataTypeInsert); insertIterator.hasNext();)
      {
         DataStruct* insertData = insertIterator.getNext();
         InsertStruct* insert = insertData->getInsert();
         //BlockStruct* insertedGeometry = getCamCadDatabase().getBlock(insert->getBlockNumber());

         if (insert->getBlockNumber() == lookingForInsertBlkNum)
         {
            // Iterate on visible attribs and flip their layer to mirror layer.
            if (insertData->getAttributesRef() != NULL)
            {
               POSITION attpos = insertData->getAttributesRef()->GetStartPosition();
               while (attpos != NULL)
               {
                  WORD key;
                  CAttribute *attrib = NULL;
                  insertData->getAttributesRef()->GetNextAssoc(attpos, key, attrib);
                  if (attrib != NULL && attrib->isVisible())
                  {
                     int layerIndx = attrib->getLayerIndex();
                     LayerStruct *layer = this->getCamCadDatabase().getLayerAt(layerIndx);
                     if (layer != NULL)
                     {
                        int mirrorLayerIndx = layer->getMirroredLayerIndex();
                        if (mirrorLayerIndx != layerIndx)  // not really a need to check this
                        {
                           attrib->setLayerIndex(mirrorLayerIndx);
                        }
                     }
                     
                  }
               }
            }
         }
      }
   }
}

void CZukenLayerMirrorer::mirrorInsertAttributes(BlockStruct *insertedBlock)
{
   // This is not a full depth recursive attrib updater. Could be made into one, but it is not for now.
   // The driving force for this feature is that visible REFDES attribs were coming out on wrong
   // layer. So we're keeping this at the upper levels of inserts in PCB or Panel, in order to
   // keep overhead down. If it turns out that visible attribs elsewhere have problems too,
   // then one could call the lower level with the insertedBlock also being the datalistBlock, this
   // would update the attribs on inserts within "self". Also, a block directly has an attrib list,
   // so some processing for that list would need to be added too.
   // Doing that would make this more complete, but also add yet more time, and this importer
   // is a slug already, so we're not adding stuff unless the need is shown, even when we
   // know not adding it is a potential bug.

   if (insertedBlock != NULL)
   {
      getZukenReader().getLog().writef("\n" "Mirroring Insert Attributes For Geometry [%s]\n", insertedBlock->getName());

      if (getZukenReader().getPcbFile() != NULL)
      {
         mirrorInsertAttributes(insertedBlock, getZukenReader().getPcbFile()->getBlock());
      }

      if (getZukenReader().getPanFile() != NULL)
      {
         mirrorInsertAttributes(insertedBlock, getZukenReader().getPanFile()->getBlock());
      }

   }
}

void CZukenLayerMirrorer::mirrorGeometryData()
{
   getZukenReader().getLog().writef("\n" "Mirroring geometry data\n");

   for (POSITION pos = m_mirroredGeometries->getStartPos();pos != NULL;)
   {
      BlockStruct* mirroredGeometry = m_mirroredGeometries->getNext(pos);

      getZukenReader().getLog().writef("Mirroring data in Geometry %d '%s'\n",
         mirroredGeometry->getBlockNumber(),mirroredGeometry->getName());

      for (CDataListIterator dataListIterator(*mirroredGeometry);dataListIterator.hasNext();)
      {
         DataStruct* data = dataListIterator.getNext();
         int layerIndex = data->getLayerIndex();

         if (layerIndex >= 0)
         {
            LayerStruct* layer = getCamCadDatabase().getLayerAt(layerIndex);

            int mirroredLayerIndex = layer->getMirroredLayerIndex();

            if (mirroredLayerIndex != layerIndex)
            {
               data->setLayerIndex(mirroredLayerIndex);
            }
         }
      }

      mirrorInsertAttributes(mirroredGeometry);
   }
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
CCr5kPrototype::CCr5kPrototype()
{
}

void CCr5kPrototype::empty()
{

   CCr5kElement::empty();
}

// bnf: 
bool CCr5kPrototype::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   return tokenReader.skipList();  // todo

   bool retval = true;

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}

#ifdef NeverEver
// bnf: 
bool CCr5kPrototype::read(CCr5kTokenReader& tokenReader)
{
   CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

   bool retval = true;

   for (bool loopFlag = true;loopFlag;)
   {
      CCr5kPrototypeChild* child = new CCr5kPrototypeChild();

      // bnf:  
      if (tokenReader.readZeroOrOneElements(*child,ptokChildTok))
      {
         m_protoTypeList.AddTail(child);
      }
      else
      {
         delete child;
         loopFlag = false;
      }
   }

   retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

   setLoadedFlag();

   return retval;
}
#endif

/*

















*/


#endif
