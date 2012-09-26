// $Header: /CAMCAD/4.6/read_wrt/OdbPpIn.cpp 98    6/04/07 5:29p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2011. All Rights Reserved.
*/

#include "StdAfx.h"
#include "CamCadDatabase.h"
#include "OdbPpIn.h"
#include "Gauge.h"
#include "Format.h"
#include "PcbUtil.h"
#include "Lic.h"
#include "CCEtoODB.h"
#include "WriteFormat.h"
#include "StdioZipFile.h"
#include "TMState.h"
#include <sys/stat.h>
#include "CCEtoODB.h"
#include "Wincrypt.h"
#include "RwUiLib.h"
#include "DeviceType.h"
#include "RealPart.h"
#include "Outline.h"
#include "GenerateCentroidAndOutline.h"

#define EXPERIMENTAL_VPL
#ifdef  EXPERIMENTAL_VPL
static int getVplPkgsFileContent(CString vplPkgsFilePath, CString &vplContent);
#endif

// uncomment to enable memory leak detection
//#define new DEBUG_NEW

//typedef COdbMemFileReaderTester COdbAppFileReader ;
typedef COdbFileReader COdbAppFileReader ;

#define RecordCode1(a)       ((((unsigned int)#@a)<<24) | (((unsigned int)' ')<<16) | (((unsigned int)' ')<<8) | (((unsigned int)' ')<<0))
#define RecordCode2(a,b)     ((((unsigned int)#@a)<<24) | (((unsigned int)#@b)<<16) | (((unsigned int)' ')<<8) | (((unsigned int)' ')<<0))
#define RecordCode3(a,b,c)   ((((unsigned int)#@a)<<24) | (((unsigned int)#@b)<<16) | (((unsigned int)#@c)<<8) | (((unsigned int)' ')<<0))
#define RecordCode4(a,b,c,d) ((((unsigned int)#@a)<<24) | (((unsigned int)#@b)<<16) | (((unsigned int)#@c)<<8) | (((unsigned int)#@d)<<0))

#define NoLayer (-1)
#define DrillLayerNotCalculated (-2)
#define DrillLayerUndefined     (-1)
#define EXT_SMALL_NUMBER        (1.0E-9)

extern CString LogReaderProgram;
extern CProgressDlg* progress;
static CUniqueErrorFilter uniqueErrorFilter;

COdbOptions& getOdbOptions();
COdbCompressableFileCache& getOdbCompressableFileCache();
double getSearchTolerance();
void setSearchTolerance(double searchTolerance);
static inline bool fpeq2(double a,double b);
CString properCase(CString string);
bool isFolder(const CString& pathname);

CUnits g_odbUnits(pageUnitsInches);
CUnits& getOdbUnits()
{
   return g_odbUnits;
}

inline bool fpeq3(double a,double b)
{
   bool retval = (fabs(a - b) < SMALLNUMBER);

   // This was failing for a=0.000000 and b = -4.44nnnE-016, 
   // which I would consider both 0 for our purposes.
   //if (b != 0.)
   //{
   //   retval = (fabs((a/b) - 1.) < SMALLNUMBER);
   //}
   //else
   //{
   //   retval = (fabs(a) < SMALLNUMBER);
   //}

   return retval; 
}

void performMemoryAllocationTest(CWriteFormat& log)
{
   CMemoryStatus memoryStatusBefore("Before Test"),memoryStatusAfter("After Allocations");
   int chunkSize = 1024*1024*1024;
   CPtrList allocationList;

   memoryStatusBefore.sampleMemoryStatus();

   for (int index = 0;chunkSize >= 1024*1024;index++)
   {
      void* allocation = malloc(chunkSize);

      log.writef("Allocation %d:  size=0x%08x, address=0x%08x\n",index,chunkSize,allocation);

      if (allocation != NULL)
      {
         allocationList.AddTail(allocation);
      }
      else
      {
         chunkSize /= 2;
      }
   }

   memoryStatusAfter.sampleMemoryStatus();

   if (memoryStatusAfter.sampleMemoryStatus())
   {
      memoryStatusBefore.writeComparitiveStatusReport(log,memoryStatusAfter);
   }

   formatMessageBox("performMemoryAllocationTest(): At maximum memory usage");

   for (POSITION pos = allocationList.GetTailPosition();pos != NULL;)
   {
      void* allocation = allocationList.GetPrev(pos);
      free(allocation);
   }
}

/******************************************************************************
* ReadODBpp
*/
void ReadODBpp(const char *pFilePath,CCEtoODBDoc *Doc,FormatStruct *Format,int pageUnits)
{
   bool filePathDefaultDelimeterIsSlash = CFilePath::setDefaultDelimiterSlash(false);

   ASSERT(Doc != NULL);

   Format->setEnableGenerateSmdComponents(false);

   // test
   //bool testFlag = false;

   //if (testFlag)
   //{
   //   CStdioUnixCompressedFile inputFile;
   //   CStdioFile outputFile;

   //   if (inputFile.openRead("C:\\Projects\\OdbSamples\\ffconta\\steps\\pcb\\eda\\data.z") == statusSucceeded)
   //   {
   //      if (outputFile.Open("C:\\Projects\\OdbSamples\\ffconta\\steps\\pcb\\eda\\data",CFile::modeWrite | CFile::modeCreate))
   //      {
   //         CString string;
   //         int lineCount = 0;

   //         while (inputFile.readString(string) == statusSucceeded)
   //         {
   //            lineCount++;
   //            outputFile.WriteString(string);
   //         }

   //         inputFile.close();
   //         outputFile.Close();
   //      }
   //   }

   //   return;
   //}

   uniqueErrorFilter.empty();
   getOdbOptions().loadOdbSettings();
   getOdbOptions().loadOdbSettings(Format->formatString);

   CCamCadDatabase camCadDatabase(*Doc);
   COdbPpDatabase database(camCadDatabase);
   const int writeFormatBufSize = 2048;

   CString filePath(pFilePath);
   //filePath = "e:/Projects/Tar/_TestOdbNew.tar";
   CFilePath projectFilePath(filePath);

   if (projectFilePath.getFileName() == "") projectFilePath.popLeaf();

   CFilePath logFilePath(GetLogfilePath(projectFilePath.getFileName()));

   COdbArchiveFile odbArchiveFile(filePath);

   if (odbArchiveFile.isPathArchive())
   {
      if (progress != NULL)
         progress->SetStatus("Extracting temporary archived files.");
      OdbArchiveFileStatusTag status = odbArchiveFile.extract();

      if (status != statusOdbArchiveSucceeded)
      {
         formatMessageBoxApp(MB_ICONSTOP,"Failed to extract the ODB++ archive, '%s' - '%s'",
            filePath,(const char*)odbArchiveFileStatusTagToString(status));

         return;
      }

      projectFilePath.setPath(odbArchiveFile.getProjectPath());
      logFilePath.setPath(odbArchiveFile.getLogFilePath());
   }

   if (!isFolder(projectFilePath.getPath()))
   {
      formatMessageBoxApp(MB_ICONSTOP,"Path '%s' is not a folder.",
         filePath);

      return;
   }

   getApp().WriteProfileString("OdbImport","ProjectDirectory",projectFilePath.getPath());

   if (getOdbOptions().getOptionString(optionLogFileDirectoryPath).CompareNoCase(Qproject) == 0)
   {
      logFilePath.setPath(filePath);

      if (logFilePath.getFileName() == "") logFilePath.popLeaf();

      logFilePath.setPath(GetLogfilePath(logFilePath.getFileName()));
   }
   else if (getOdbOptions().getOptionString(optionLogFileDirectoryPath).CompareNoCase(QuserPathName) == 0)
   {
      logFilePath.setPath(GetLogfilePath(projectFilePath.getFileName()));
   }

   CFilePath htmlLogFilePath(logFilePath);
   CFilePath textLogFilePath(logFilePath);

   int logCount = 0;
   CHtmlFileWriteFormat* htmlLogFile = NULL;
   CLogWriteFormat* textLogFile = NULL;
   CTraceFormat* errorLogTrace = NULL;
   CWriteFormat* pLog = NULL;
   CMultipleWriteFormat* multipleWriteFormat = NULL;

   if (getOdbOptions().getOptionFlag(optionWriteTextLogFile))
   {
      textLogFilePath.setExtension("txt");
      textLogFile = new CLogWriteFormat(writeFormatBufSize);

      if (!textLogFile->open(textLogFilePath.getPath()))
      {
         formatMessageBoxApp(MB_ICONSTOP,"Could not open the error log, '%s'",
            (const char*)textLogFilePath.getPath());
      }
      else
      {
         logCount++;
         pLog = textLogFile;
      }
   }

   if (getOdbOptions().getOptionFlag(optionWriteHtmlLogFile) || !getOdbOptions().getOptionFlag(optionWriteTextLogFile))
   {
      htmlLogFilePath.setExtension("htm");
      htmlLogFile = new CHtmlFileWriteFormat(writeFormatBufSize);

      if (!htmlLogFile->open(htmlLogFilePath.getPath()))
      {
         formatMessageBoxApp(MB_ICONSTOP,"Could not open the error log, '%s'",
            (const char*)htmlLogFilePath.getPath());
      }
      else
      {
         logCount++;
         pLog = htmlLogFile;
      }
   }

   if (getOdbOptions().getOptionFlag(optionWriteUncompressedFiles))
   {
      COdbFileReader::setWriteUncompressedFileFlag(true);
   }

#ifdef _DEBUG
   errorLogTrace = new CTraceFormat(writeFormatBufSize);
   logCount++;
   pLog = errorLogTrace;

   CString configuration = "(Debug Build)";
#else
   CString configuration = "";
#endif

   if (logCount > 0)
   {
      if (logCount > 1)
      {
         multipleWriteFormat = new CMultipleWriteFormat(writeFormatBufSize);
         pLog = multipleWriteFormat;

         multipleWriteFormat->attach(textLogFile);
         multipleWriteFormat->attach(htmlLogFile);
         multipleWriteFormat->attach(errorLogTrace);
      }

      CWriteFormat& log = *pLog;

      log.writef("%s\n",(const char*)getApp().getCamCadSubtitle());
      log.writef("CAMCAD ODB++ reader. %s\n",(const char*)configuration);
      displayProgress(log,"Opened log file '%s'.",(const char*)logFilePath.getPath());

      getOdbOptions().loadOdbSettings(&log);
      getOdbOptions().loadOdbSettings(Format->formatString,&log);
      //getOdbOptions().getFeatureAttributes().dumpAttributeMap(log);

      if (getOdbOptions().getOptionFlag(optionEnableOptionLogging))
      {
         getOdbOptions().writeOptions(log);
      }

      bool readSuccessfull = database.read(projectFilePath.getPath(),intToPageUnitsTag(pageUnits),log);
      getOdbCompressableFileCache().cleanUp(&log);

      if (readSuccessfull)
      {
         double xMin,yMin,xMax,yMax;
         database.getCamCadDatabase().getCamCadDoc().get_extents(&xMin,&xMax,&yMin,&yMax);

         log.writef(PrefixStatus,
            "extents min(%.3f,%.3f), max(%.3f,%.3f)\n",
            xMin,yMin,xMax,yMax);
      }

      if (log.getPrefixCount(PrefixDisaster) > 0)
      {
         log.writef(PrefixSummary,
            "%d Disaster%s\n",
            log.getPrefixCount(PrefixDisaster),((log.getPrefixCount(PrefixDisaster) != 1) ? "s" : ""));
      }

      log.writef(PrefixSummary,
         "%d Error%s\n",
         log.getPrefixCount(PrefixError),((log.getPrefixCount(PrefixError) != 1) ? "s" : ""));

      log.writef(PrefixSummary,
         "%d Warning%s\n",
         log.getPrefixCount(PrefixWarning),((log.getPrefixCount(PrefixWarning) != 1) ? "s" : ""));

      bool htmlFormatFlag = (getOdbOptions().getOptionFlag(optionWriteHtmlLogFile) || 
                            !getOdbOptions().getOptionFlag(optionWriteTextLogFile)    );

      CString oldLogReaderProgram(LogReaderProgram);
      LogReaderProgram = (htmlFormatFlag ? "iexplore" : "NotePad");

      if (odbArchiveFile.hasTempFiles())
      {
         displayProgress(log,"Removing temporary archived files.");

         odbArchiveFile.removeTempFiles();
      }

      displayProgress(log,"Loading log file into '%s'",(const char*)LogReaderProgram);

      database.empty();

      if (textLogFile != NULL) textLogFile->close();
      if (htmlLogFile != NULL) htmlLogFile->close();

      if (getOdbOptions().getOptionFlag(optionDisplayLogFile) && getApp().getUseDialogsFlag())
      {
         if (htmlFormatFlag)
         {
            ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",htmlLogFilePath.getPath(),NULL,NULL,SW_SHOW);
         }
         else
         {
            Logreader(textLogFilePath.getPath());
         }
      }

      LogReaderProgram = oldLogReaderProgram;
   }

   if (getOdbOptions().GetConsolidatorControl().GetIsEnabled())
   {
      CGeometryConsolidator steamroller(Doc, "" /*no log file */);
      steamroller.Go( &(getOdbOptions().GetConsolidatorControl()) );
   }

   delete textLogFile;
   delete htmlLogFile;
   delete errorLogTrace;
   delete multipleWriteFormat;

   CFilePath::setDefaultDelimiterSlash(filePathDefaultDelimeterIsSlash);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
static COdbOptions* g_odbOptions;

COdbOptions& getOdbOptions()
{
   if (g_odbOptions == NULL)
   {
      g_odbOptions = new COdbOptions();
   }

   return *g_odbOptions;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
static COdbCompressableFileCache g_odbCompressableFileCache;

COdbCompressableFileCache& getOdbCompressableFileCache()
{
   return g_odbCompressableFileCache;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double g_searchTolerance  = .0000001;

double getSearchTolerance()
{
   return g_searchTolerance;
}

void setSearchTolerance(double searchTolerance)
{
   g_searchTolerance = searchTolerance;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
static inline bool fpeq2(double a,double b)
{
   return (fabs(a - b) < .0001);
}

void updateToExistingCompressableFilePath(CFilePath& filePath,CWriteFormat& log)
{
   static bool testUncompressionFlag = false;

   CFilePath compressedFilePath(filePath);
   compressedFilePath.setExtension("z");

   if (fileExists(compressedFilePath.getPath()))
   {
      if (testUncompressionFlag)
      {
         getOdbCompressableFileCache().uncompress(filePath.getPath(),log);

         log.writef(PrefixStatus,"Checking uncompression of '%s' with '%s'\n",
            (const char*)compressedFilePath.getPath(),
            (const char*)filePath.getPath());

         while (true)
         {
            COdbAppFileReader fileReader;

            if (fileReader.openFile(filePath.getPath()) != statusSucceeded)
            {
               log.writef(PrefixError,"Error opening '%s'\n",
                  (const char*)filePath.getPath());

               break;
            }

            COdbAppFileReader compressedFileReader;

            if (compressedFileReader.openFile(compressedFilePath.getPath()) != statusSucceeded)
            {
               log.writef(PrefixError,"Error opening '%s'\n",
                  (const char*)compressedFilePath.getPath());

               break;
            }

            CString lineFromFile,lineFromCompressedFile;
            FileStatusTag fileReadStatus,compressedFileReadStatus;
            int lineCount = 0;

            for (lineCount=0;;lineCount++)
            {
               fileReadStatus = fileReader.readFileRecord();
               compressedFileReadStatus = compressedFileReader.readFileRecord();

               if (fileReadStatus != compressedFileReadStatus)
               {
                  log.writef(PrefixError,
                     "Status mismatch: recordNumber=%d, fileReadStatus='%s', compressedFileReadStatus='%s'\n",
                     fileReader.getNumRecordsRead(),
                     (const char*)fileStatusTagToString(fileReadStatus),
                     (const char*)fileStatusTagToString(compressedFileReadStatus));

                  break;
               }
               else if (fileReadStatus != statusSucceeded)
               {
                  break;
               }
               else
               {
                  lineFromFile = fileReader.getRecord();
                  lineFromCompressedFile = compressedFileReader.getRecord();

                  if (lineFromFile != lineFromCompressedFile)
                  {
                     log.writef(PrefixError,
                        "Record mismatch: recordNumber=%d, lineFromFile='%s', lineFromCompressedFile='%s'\n",
                        fileReader.getNumRecordsRead(),
                        (const char*)lineFromFile,
                        (const char*)lineFromCompressedFile);

                     break;
                  }
               }
            }

            break;
         }
      }

      filePath = compressedFilePath;
   }
}

// filters multiple error messages into a single message base on the messageId,
// a string formed by the format and arguments appended with the file path.
bool CUniqueErrorFilter::writef(CWriteFormat& log,int prefix,
   const CString& filePath,int lineNumber,const CString& recordString,
   const char* format,...)
{
   bool retval = false;

   va_list args;
   va_start(args,format);

   CString errorId;
   errorId.FormatV(format,args);
   va_end(args);

   CString errorMessage(errorId);

   errorId += filePath;

   int count = 0;

   if (m_errorMessages.GetCount() == 0)
   {
      m_errorMessages.InitHashTable(nextPrime2n(100));
   }

   if (!m_errorMessages.Lookup(errorId,count))
   {
      retval = true;

      log.writef(prefix,"%s; Record='%s' in '%s'.%d\n",
(const char*)errorMessage,(const char*)recordString,(const char*)filePath,lineNumber);
   }

   count++;
   m_errorMessages.SetAt(errorId,count);

   return retval;
}

bool OdbPpIn_TranslateOdbNetName(CString& netName)
{
   if (netName.CompareNoCase("$NONE$") == 0)
   {
      netName = NET_UNUSED_PINS;
      return true; // Name was changed
   }
   return false; // Not changed
}

unsigned int getRecordCode(const char* param)
{
   unsigned int recordCode = 0;
   static char spaces[] = "    ";

   switch (*param)
   {
   case '#':  return RecordCode1(#);  break;
   case '@':  return RecordCode1(@);  break;
   case '$':  return RecordCode1($);  break;
   case '&':  return RecordCode1(&);  break;
   }

   if (*param == '\0') param = spaces;

   recordCode = *(param++);

   if (*param == '\0') param = spaces;

   recordCode = (recordCode << 8) | *(param++);

   if (*param == '\0') param = spaces;

   recordCode = (recordCode << 8) | *(param++);

   if (*param == '\0') param = spaces;

   recordCode = (recordCode << 8) | *(param++);

   return recordCode;
}

double orientationToRadians(int orientation)
{
   orientation = (((orientation % 4) + 4) % 4);

   return degreesToRadians(orientation * 90.);
}

double orientationToDegrees(int orientation)
{
   orientation = (((orientation % 4) + 4) % 4);

   return (orientation * 90.);
}

bool orientationToMirror(int orientation)
{
   orientation = (((orientation % 8) + 8) % 8);

   return (orientation >= 4);
}

//_____________________________________________________________________________
OdbTokenTag stringToOdbToken(CString tokenString)
{
   OdbTokenTag token = tok_OdbTokenTagUndefined;

   static CMapStringToWord s_map;

   tokenString.MakeLower();
   WORD value;

   if (s_map.Lookup(tokenString,value))
   {
      token = (OdbTokenTag)value;
   }
   else if (s_map.GetCount() == 0)
   {
      s_map.InitHashTable(nextPrime2n(min(100,(int)(tok_OdbTokenTagUndefined/.8))));

#define tok(a) s_map.SetAt(CString(#a).MakeLower(),tok##a);
#define tok2(a,b) s_map.SetAt(CString(a).MakeLower(),tok_##b);

      // tok2("@",AtSign) yields <<s_map.SetAt(CString("@").MakeLower(),tok_AtSign);>>
      // tok(Angle)       yields <<s_map.SetAt(CString("Angle").MakeLower(),tokAngle);>>

   #include "OdbPpReaderTokens.h"

#undef tok
#undef tok2

      if (s_map.GetCount() != tok_OdbTokenTagUndefined)
      {
         // missed some entries
         formatMessageBoxApp("Token map contains only %d of %d entries.",s_map.GetCount(),(int)tok_OdbTokenTagUndefined);
         //AfxDebugBreak();
      }

      token = stringToOdbToken(tokenString);
   }

   return token;
}

OdbTokenTag paramToOdbToken(const CStringArray& params,int index)
{
   OdbTokenTag tokenTag = tok_OdbTokenTagUndefined;

   if (index >= 0 || index < params.GetSize())
   {
      tokenTag = stringToOdbToken(params[index]);
   }

   return tokenTag;
}

FeatureIdTypeTag stringToFidTypeTag(CString tagString)
{
   FeatureIdTypeTag tagValue = featureTypeUndefined;
   tagString.MakeLower();

   if      (tagString == "c") tagValue = featureTypeCopper;
   else if (tagString == "l") tagValue = featureTypeLaminate;
   else if (tagString == "h") tagValue = featureTypeHole;

   return tagValue;
}

PackagePinTypeTag stringToPackagePinTypeTag(CString tagString)
{
   PackagePinTypeTag tagValue = packagePinTypeUndefined;
   tagString.MakeLower();

   if      (tagString == "t") tagValue = packagePinTypeThruHole;
   else if (tagString == "b") tagValue = packagePinTypeBlind;
   else if (tagString == "s") tagValue = packagePinTypeSurface;

   return tagValue;
}

PackagePinMountTypeTag stringToPackagePinMountTypeTag(CString tagString)
{
   PackagePinMountTypeTag tagValue = pinMountTypeUndefined;
   tagString.MakeLower();

   if      (tagString == "s") tagValue = pinMountTypeSmt;
   else if (tagString == "d") tagValue = pinMountTypeSmtPadSize;
   else if (tagString == "t") tagValue = pinMountTypeThruHole;
   else if (tagString == "r") tagValue = pinMountTypeThruHoleHoleSize;
   else if (tagString == "p") tagValue = pinMountTypePressfit;
   else if (tagString == "n") tagValue = pinMountTypeNonBoard;
   else if (tagString == "h") tagValue = pinMountTypeHole;

   return tagValue;
}

PackagePinElectricalTypeTag stringToPackagePinElectricalTypeTag(CString tagString)
{
   PackagePinElectricalTypeTag tagValue = packagePinElectricalTypeUndefined;
   tagString.MakeLower();

   if      (tagString == "e") tagValue = packagePinElectricalTypeElectrical;
   else if (tagString == "m") tagValue = packagePinElectricalTypeMechanical;

   return tagValue;
}

PlaneFillTypeTag stringToPlaneFillTypeTag(CString tagString)
{
   PlaneFillTypeTag tagValue = planeFillUndefined;
   tagString.MakeLower();

   if      (tagString == "s") tagValue = planeFillSolid;
   else if (tagString == "h") tagValue = planeFillHatched;
   else if (tagString == "o") tagValue = planeFillOutline;

   return tagValue;
}

CutoutTypeTag stringToCutoutTypeTag(CString tagString)
{
   CutoutTypeTag tagValue = cutoutUndefined;
   tagString.MakeLower();

   if      (tagString == "c") tagValue = cutoutCircle;
   else if (tagString == "r") tagValue = cutoutRectangle;
   else if (tagString == "o") tagValue = cutoutOctagon;
   else if (tagString == "e") tagValue = cutoutExact;

   return tagValue;
}

OdbLayerTypeTag stringToOdbLayerTypeTag(CString layerTypeString)
{
   OdbLayerTypeTag layerTypeTag = odbLayerTypeUndefined;
   layerTypeString.MakeLower();

   if      (layerTypeString == "signal"      ) layerTypeTag = odbLayerTypeSignal;
   else if (layerTypeString == "power_ground") layerTypeTag = odbLayerTypePower;
   else if (layerTypeString == "mixed")        layerTypeTag = odbLayerTypeMixed;
   else if (layerTypeString == "solder_mask")  layerTypeTag = odbLayerTypeSolderMask;
   else if (layerTypeString == "solder_paste") layerTypeTag = odbLayerTypeSolderPaste;
   else if (layerTypeString == "silk_screen")  layerTypeTag = odbLayerTypeSilkscreen;
   else if (layerTypeString == "drill")        layerTypeTag = odbLayerTypeDrill;
   else if (layerTypeString == "rout")         layerTypeTag = odbLayerTypeRout;
   else if (layerTypeString == "document")     layerTypeTag = odbLayerTypeDocument;
   else if (layerTypeString == "component")    layerTypeTag = odbLayerTypeComponent;

   return layerTypeTag;
}

//_____________________________________________________________________________
AttributeDefTypeTag stringToAttributeDefTypeTag(CString tagString)
{
   AttributeDefTypeTag retval = typeUndefined;
   tagString.MakeLower();

   if      (tagString == "layer"  ) retval = typeLayer;
   else if (tagString == "int"    ) retval = typeInt;
   else if (tagString == "double" ) retval = typeDouble;
   else if (tagString == "p_inch" ) retval = typePInch;
   else if (tagString == "p_mil"  ) retval = typePMil;
   else if (tagString == "string" ) retval = typeString;
   else if (tagString == "boolean") retval = typeBoolean;

   return retval;
}

//_____________________________________________________________________________
OdbPadUsageTag intToOdbPadUsageTag(int intVal)
{
   OdbPadUsageTag retval = padUsageUndefined;

   if (intVal >= 0 && intVal < padUsageUndefined)
   {
      retval = (OdbPadUsageTag)intVal;
   }

   return retval;
}

//_____________________________________________________________________________
CString odbPadUsageTagToString(OdbPadUsageTag tag)
{
   CString retval = "INVALID";

   switch (tag)
   {
   case padUsageToeprint:        retval = "ToePrint";          break;
   case padUsageVia:             retval = "Via";               break;
   case padUsageGlobalFiducial:  retval = "GlobalFiducial";    break;
   case padUsageLocalFiducial:   retval = "LocalFiducial";     break;
   case padUsageToolingHole:     retval = "ToolingHole";       break;
   case padUsageUndefined:       retval = "Undefined";         break;
   }

   return retval;
}

//_____________________________________________________________________________

CString standardSymbolTypeTagToString(StandardSymbolTypeTag tagValue)
{
   CString retval = "INVALID";

   switch (tagValue)
   {
   case standardSymbolCircle:                         retval = "standardSymbolCircle";                         break;
   case standardSymbolSquare:                         retval = "standardSymbolSquare";                         break;
   case standardSymbolRectangle:                      retval = "standardSymbolRectangle";                      break;
   case standardSymbolRoundedRectangle:               retval = "standardSymbolRoundedRectangle";               break;
   case standardSymbolChamferedRectangle:             retval = "standardSymbolChamferedRectangle";             break;
   case standardSymbolOval:                           retval = "standardSymbolOval";                           break;
   case standardSymbolDiamond:                        retval = "standardSymbolDiamond";                        break;
   case standardSymbolOctagon:                        retval = "standardSymbolOctagon";                        break;
   case standardSymbolRoundDonut:                     retval = "standardSymbolRoundDonut";                     break;
   case standardSymbolSquareDonut:                    retval = "standardSymbolSquareDonut";                    break;
   case standardSymbolHorizontalHexagon:              retval = "standardSymbolHorizontalHexagon";              break;
   case standardSymbolVerticalHexagon:                retval = "standardSymbolVerticalHexagon";                break;
   case standardSymbolButterfly:                      retval = "standardSymbolButterfly";                      break;
   case standardSymbolSquareButterfly:                retval = "standardSymbolSquareButterfly";                break;
   case standardSymbolTriangle:                       retval = "standardSymbolTriangle";                       break;
   case standardSymbolHalfOval:                       retval = "standardSymbolHalfOval";                       break;
   case standardSymbolRoundThermalRounded:            retval = "standardSymbolRoundThermalRounded";            break;
   case standardSymbolRoundThermalSquare:             retval = "standardSymbolRoundThermalSquare";             break;
   case standardSymbolSquareThermal:                  retval = "standardSymbolSquareThermal";                  break;
   case standardSymbolSquareThermalOpenCorners:       retval = "standardSymbolSquareThermalOpenCorners";       break;
   case standardSymbolSquareRoundThermal:             retval = "standardSymbolSquareRoundThermal";             break;
   case standardSymbolRectangularThermal:             retval = "standardSymbolRectangularThermal";             break;
   case standardSymbolRectangularThermalOpenCorners:  retval = "standardSymbolRectangularThermalOpenCorners";  break;
   case standardSymbolEllipse:                        retval = "standardSymbolEllipse";                        break;
   case standardSymbolMoire:                          retval = "standardSymbolMoire";                          break;
   case standardSymbolHole:                           retval = "standardSymbolHole";                           break;
   case standardSymbolNull:                           retval = "standardSymbolNull";                           break;
   case standardSymbolInvalid:                        retval = "standardSymbolInvalid";                        break;
   case standardSymbolUndefined:                      retval = "standardSymbolUndefined";                      break;
   }

   return retval;
}

CString fidTypeTagToString(FeatureIdTypeTag tag)
{
   CString stringValue;

   switch (tag)
   {
   case featureTypeCopper:     stringValue = "Copper";     break;
   case featureTypeLaminate:   stringValue = "Laminate";   break;
   case featureTypeHole:       stringValue = "Hole";       break;

   case featureTypeUndefined:
   default:                    stringValue = "Undefined";  break;
   }

   return stringValue;
}

CString padstackCompareStatusTagToString(PadstackCompareStatusTag tag)
{
   CString stringValue;

   switch (tag)
   {
   case padstackCompareNotEquivalent:  stringValue = "padstackCompareNotEquivalent";  break;
   case padstackCompareMissingLayer:   stringValue = "padstackCompareMissingLayer";   break;
   case padstackComparePadType:        stringValue = "padstackComparePadType";        break;
   case padstackComparePolarity:       stringValue = "padstackComparePolarity";       break;
   case padstackCompareOffset:         stringValue = "padstackCompareOffset";         break;
   case padstackCompareRotation:       stringValue = "padstackCompareRotation";       break;
   case padstackCompareDcode:          stringValue = "padstackCompareDcode";          break;
   case padstackCompareEquivalent:     stringValue = "padstackCompareEquivalent";     break;
   case padstackCompareUndefined:      stringValue = "padstackCompareUndefined";      break;
   default:                            stringValue = "Undefined";                     break;
   }

   return stringValue;
}

CString odbArchiveFileStatusTagToString(OdbArchiveFileStatusTag tagValue)
{
   CString stringValue("statusOdbArchiveUndefined");

   switch (tagValue)
   {
   case statusCopyOfArchiveFailed:        stringValue = "statusCopyOfArchiveFailed";        break;
   case statusGunzipFailure:              stringValue = "statusGunzipFailure";              break;
   case statusMakeTempDirFailure:         stringValue = "statusMakeTempDirFailure";         break;
   case statusNoTempDir:                  stringValue = "statusNoTempDir";                  break;
   case statusNoMatrixDir:                stringValue = "statusNoMatrixDir";                break;
   case statusOdbArchiveSucceeded:        stringValue = "statusOdbArchiveSucceeded";        break;
   case statusTarFailure:                 stringValue = "statusTarFailure";                 break;
   case statusTooManyDirs:                stringValue = "statusTooManyDirs";                break;
   case statusUnrecognizedArchiveFormat:  stringValue = "statusUnrecognizedArchiveFormat";  break;
   }

   return stringValue;
}

bool hasCorner(int corners,int cornerNumber)
{
   bool retval = (corners == 0);

   while (corners != 0 && !retval)
   {
      retval = (corners%10 == cornerNumber);
      corners /= 10;
   }

   return retval;
}

// 2--1
// |  |
// 3--4
int cornerSymmetry(int corners)
{
   int symmetry = 360;
   unsigned short mask = 0;

   for (int ind = 0;ind < 4;ind++)
   {
      switch (corners % 10)
      {
      case 1:  mask |= 1;  break;
      case 2:  mask |= 2;  break;
      case 3:  mask |= 4;  break;
      case 4:  mask |= 8;  break;
      }

      corners /= 10;
   }

   switch (mask)
   {
   case 0x0:  /* 0000 */  symmetry =  90;  break;
   case 0x5:  /* 0101 */  symmetry = 180;  break;
   case 0xa:  /* 1010 */  symmetry = 180;  break;
   case 0xf:  /* 1111 */  symmetry =  90;  break;
   }

   return symmetry;
}

bool angleSymmetryEqual(double a0,int symmetry0,double a1,int symmetry1)
{
   const double f360 = 360.;

   bool retval = false;
   
   if (symmetry0 == symmetry1)
   {
      if (symmetry0 == 0)
      {
         retval = true;
      }
      else
      {
         double angleDiff = fmod(fmod(a0 - a1,f360) + f360,symmetry0);
         //double angle0 = fmod(fmod(a0,f360) + f360,symmetry0);
         //double angle1 = fmod(fmod(a1,f360) + f360,symmetry1);

         retval = (angleDiff < .01 || symmetry0 - angleDiff < .01);
      }
   }

   return retval;
}

//_____________________________________________________________________________
CTimeInterval::CTimeInterval()
{
   m_startTime = clock();
   m_stopTime  = m_startTime;
}

void CTimeInterval::setStartTime()
{
   m_startTime = clock();
}

void CTimeInterval::setStopTime()
{
   m_stopTime = clock();
}

double CTimeInterval::getIntervalInSeconds()
{
   clock_t interval = m_stopTime - m_startTime;

   double seconds = ((double)interval) / CLOCKS_PER_SEC;

   return seconds;
}

double CTimeInterval::getNonZeroIntervalInSeconds()
{
   clock_t interval = m_stopTime - m_startTime;

   if (interval == 0) interval = 1;

   double seconds = ((double)interval) / CLOCKS_PER_SEC;

   return seconds;
}

//_____________________________________________________________________________
COdbArchiveFile::COdbArchiveFile(const CString& archiveFilePath)
{
   m_archiveFilePath.setPath(archiveFilePath);
   m_archiveFilePath.setDelimiterBackSlash();

   m_hasTempFiles = false;
}

bool COdbArchiveFile::isPathArchive()
{
   bool retval = false;

   if ((m_archiveFilePath.getExtension().CompareNoCase("tar") == 0) ||
       (m_archiveFilePath.getExtension().CompareNoCase("gz")  == 0) ||
       (m_archiveFilePath.getExtension().CompareNoCase("tgz") == 0)    )
   {
      retval = fileExists(m_archiveFilePath.getPath());
   }

   return retval;
}

CString COdbArchiveFile::getTempDirPath()
{
   if (m_tempDirPath.IsEmpty())
   {
      int len = 256;

      for (int bufSize = 0;len > bufSize;)
      {
         bufSize = len;
         len = GetTempPath(bufSize,m_tempDirPath.GetBufferSetLength(bufSize));
      }

      m_tempDirPath.ReleaseBuffer(len);
   }

   return m_tempDirPath;
}

OdbArchiveFileStatusTag COdbArchiveFile::extract()
{
   OdbArchiveFileStatusTag retval = statusOdbArchiveSucceeded;

   if (getTempDirPath().IsEmpty())
   {
      return statusNoTempDir;
   }

   CFilePath archiveCopyFilePath(getProjectTempPath());
   archiveCopyFilePath.pushLeaf(m_archiveFilePath.getFileName());
   archiveCopyFilePath.setDelimiterBackSlash();

   if (mkdirpath(archiveCopyFilePath.getPath()) != 0)
   {
      return statusMakeTempDirFailure;
   }

   bool copySucceeded = (CopyFile(m_archiveFilePath.getPath(),archiveCopyFilePath.getPath(),false) != 0);

   if (! copySucceeded)
   {
      DWORD lastError = GetLastError();
      CString messageBuffer;

      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,lastError,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer.GetBufferSetLength(256),256,0);

      messageBuffer.ReleaseBuffer();

      return statusCopyOfArchiveFailed;
   }

   m_hasTempFiles = true;

   CFilePath unixDirPath(archiveCopyFilePath);
   unixDirPath.setDelimiterSlash();
   unixDirPath.popLeaf();

   if (archiveCopyFilePath.getExtension().CompareNoCase("zip") == 0)
   {
      m_commandLine.Format("gunzip -f \"%s\"",(const char*)archiveCopyFilePath.getPath());

      if (ExecAndWait(m_commandLine, SW_HIDE, NULL))
      {
         retval = statusGunzipFailure;
      }
   }
   else if (archiveCopyFilePath.getExtension().CompareNoCase("tar") == 0)
   {
      m_commandLine.Format("tar -x -f \"%s\" -C \"%s\"",
         (const char*)archiveCopyFilePath.getPath(),(const char*)unixDirPath.getPath());

      if (ExecAndWait(m_commandLine, SW_HIDE, NULL))
      {
         retval = statusTarFailure;
      }
   }
   else if (archiveCopyFilePath.getExtension().CompareNoCase("tgz") == 0)
   {
      // this unzips from tgz to tar
      m_commandLine.Format("gunzip -f \"%s\"",(const char*)archiveCopyFilePath.getPath());

      if (ExecAndWait(m_commandLine, SW_HIDE, NULL))
      {
         retval = statusGunzipFailure;
      }
      else
      {
         CFilePath tarPath(archiveCopyFilePath);
         tarPath.setExtension("tar");

         m_commandLine.Format("tar -xf \"%s\" -C \"%s\"", 
            (const char*)tarPath.getPath(),(const char*)unixDirPath.getPath());

         if (ExecAndWait(m_commandLine, SW_HIDE, NULL))
         {
            retval = statusTarFailure;
         }
      }
   }
   else
   {
      retval = statusUnrecognizedArchiveFormat;
   }

   if (retval == statusOdbArchiveSucceeded)
   {
      retval = statusOdbArchiveUndefined;
      m_projectPath.setPath(getProjectTempPath());
      m_projectPath.setDelimiterBackSlash();
      CFileFind fileFinder;

      while (retval == statusOdbArchiveUndefined)
      {
         CString dirName;
         int dirCount = 0;
         CString searchPath = m_projectPath.getPath() + "\\*.*";

         for (bool moreFilesFlag = (fileFinder.FindFile(searchPath) != 0);moreFilesFlag;)
         {
            moreFilesFlag = (fileFinder.FindNextFile() != 0);

            if (fileFinder.IsDirectory() && ! fileFinder.IsDots())
            {
               dirCount++;

               dirName = fileFinder.GetFileName();

               if (dirName.CompareNoCase("matrix") == 0)
               {
                  retval = statusOdbArchiveSucceeded;
                  break;
               }
            }
         }

         fileFinder.Close();

         if (retval != statusOdbArchiveSucceeded)
         {
            if (dirCount == 1)
            {
               m_projectPath.pushLeaf(dirName);
            }
            else if (dirCount == 0)
            {
               retval = statusNoMatrixDir;
            }
            else if (dirCount > 1)
            {
               retval = statusTooManyDirs;
            }
         }
      }
   }

   return retval;
}

bool COdbArchiveFile::removeTempFiles()
{
   bool retval = false;

   if (! getTempDirPath().IsEmpty())
   {
      if (getProjectTempPath().CompareNoCase(getTempDirPath()) != 0)
      {
         retval = removeDirectoryTree(getProjectTempPath());
      }
   }

   return retval;
}

CString COdbArchiveFile::getProjectTempPath()
{
   CFilePath path(getTempDirPath());

   path.pushLeaf("CamCad");
   path.pushLeaf(m_archiveFilePath.getBaseFileName());

   return path.getPath();
}

CString COdbArchiveFile::getLogFilePath()
{
   CFilePath path(getTempDirPath());

   path.pushLeaf("CamCad");

   return path.getPath();
}

//_____________________________________________________________________________
COdbOption::COdbOption(OdbOptionTag optionTag,const CString& name,const CString& value)
{
   m_optionTag = optionTag;
   m_name      = name;

   setValue(value);
}

void COdbOption::setValue(const CString& value)
{
   m_stringValue = value;
   m_boolValue   = (value.CompareNoCase("yes") == 0 || value.CompareNoCase("true") == 0);
   m_intValue    = atoi(value);
   m_dblValue    = atof(value);
}

void COdbOption::setValue(bool value)
{
   m_stringValue = (value ? "true" : "false");
   m_boolValue   = value;
   m_intValue    = (int)value;
   m_dblValue    = (double)m_intValue;
}

void COdbOption::setValue(int value)
{
   m_stringValue.Format("%d",value);
   m_boolValue   = (value != 0);
   m_intValue    = value;
   m_dblValue    = (double)value;
}

void COdbOption::setValue(double value)
{
   m_stringValue.Format("%f",value);
   m_boolValue   = (value != 0.);
   m_intValue    = (int)value;
   m_dblValue    = value;
}

//_____________________________________________________________________________
COdbOptions::COdbOptions() : m_options(20)
{
// 0 = take existing ATT_SMD
// 1 = if all pins of comp are marked as SMD
// 2 = if most pins of comp are marked as SMD
// 3 = if one pin of comp are marked as SMD
   add(optionComponentsSmdRule      ,".ComponentSMDrule"       ,"2");
   add(optionDisplayLogFile         ,".DisplayLogFile"         ,"false");
   add(optionEnableDumpOutput       ,".EnableDumpOutput"       ,"false");
   add(optionEnableDebugProperties  ,".EnableDebugProperties"  ,"false");
   add(optionEnablePinNameRefdesEdit,".EnablePinNameRefdesEdit","false");
   add(optionEnableNcPinQuest       ,".EnableNcPinQuest"       ,"true");
   add(optionEnablePadstackQuest    ,".EnablePadstackQuest"    ,"true");
   add(optionEnableViastackQuest    ,".EnableViastackQuest"    ,"true");
   add(optionEnableOptionLogging    ,".EnableOptionLogging"    ,"false");
   add(optionEnablePadstackReport   ,".EnablePadstackReport"   ,"true");
   add(optionEnablePackagePinReport ,".EnablePackagePinReport" ,"false");
   add(optionEnableManufactureRead  ,".EnableManufactureRead"  ,"false");
   add(optionEnableArtworkRead      ,".EnableArtworkRead"      ,"false");
   add(optionKeepUncompressedFiles  ,".KeepUncompressedFiles"  ,"true");
   add(optionWriteDebugCcFile       ,".WriteDebugCcFile"       ,"false");
   add(optionWriteHtmlLogFile       ,".WriteHtmlLogFile"       ,"true");
   add(optionWriteTextLogFile       ,".WriteTextLogFile"       ,"false");
   add(optionWriteUncompressedFiles ,".WriteUncompressedFiles" ,"false");
   add(optionUncompressCommand      ,".Uncompress"             ,"");
   add(optionLogFileDirectoryPath   ,".LogFileDirectoryPath"   ,QuserPathName);
   add(optionUserPathName           ,"userPathName"            ,"");
   add(optionProject                ,"project"                 ,"");
   add(optionConvertFreePadsToVias  ,".ConvertFreePadsToVias"  ,"false");
   add(optionPadSearchRadius        ,".PadSearchRadius"        ,".00001");  // pageUnits, I think

   this->GetConsolidatorControl().SetIsEnabled(true);
}

bool COdbOptions::getOptionFlag(OdbOptionTag optionTag)
{
   bool retval = false;

   COdbOption* option = m_options.GetAt(optionTag);

   if (option != NULL)
   {
      retval = option->getBoolValue();
   }

   return retval;
}

CString COdbOptions::getOptionString(OdbOptionTag optionTag)
{
   CString retval;

   COdbOption* option = m_options.GetAt(optionTag);

   if (option != NULL)
   {
      retval = option->getStringValue();
   }

   return retval;
}

int COdbOptions::getOptionInt(OdbOptionTag optionTag)
{
   int retval = 0;

   COdbOption* option = m_options.GetAt(optionTag);

   if (option != NULL)
   {
      retval = option->getIntValue();
   }

   return retval;
}

double COdbOptions::getOptionDbl(OdbOptionTag optionTag)
{
   double retval = 0.;

   COdbOption* option = m_options.GetAt(optionTag);

   if (option != NULL)
   {
      retval = option->getDoubleValue();
   }

   return retval;
}

void COdbOptions::add(OdbOptionTag optionTag,const CString& name,const CString& value)
{
   COdbOption* option = new COdbOption(optionTag, name, value);
   m_options.setAtGrow(optionTag, option);
}

// load settings
bool COdbOptions::loadOdbSettings(const CString& formatString,CWriteFormat* log)
{
   CString line;
   int pos = 0;
   int len = formatString.GetLength();

   while (pos < len)
   {
      line = formatString.Tokenize("\n",pos);
      setOption(line);
   }

   return true;
}

// load settings
bool COdbOptions::loadOdbSettings(CWriteFormat* log)
{
   CString settingsFilename( getApp().getImportSettingsFilePath("Odb.In") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nODB++ Reader: Settings file [%s].\n", settingsFilename);
   getApp().LogMessage(settingsFileMsg);

   FILE  *fp;

   if ((fp = fopen(settingsFilename,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFilename);
      ErrorMessage(tmp,"ODB++ Settings", MB_OK | MB_ICONHAND);

      return 0;
   }

   if (log != NULL)
   {
      displayProgress(*log,"Reading ODB settings file, '%s'.", settingsFilename);
   }

   CSupString line;
   CStringArray params;
              
   while (fgets(line.GetBufferSetLength(255),255,fp))
   {
      line.ReleaseBuffer();
      line.Trim();
      int numPar = line.ParseQuote(params," ");

      if (numPar < 2)
      {
         continue;
      }

      if (log == NULL)
      {
        setOption(line);
      }
      else if (params[0].Left(1) == ".")
      {
         if (params[0].CompareNoCase(".ATTRIBMAP") == 0)
         {
            if (numPar > 2)
            {
               CString a1 = params[1];
               CString a2 = params[2];

               a1.MakeUpper();
               a2.MakeUpper();

               m_featureAttributes.addMappedAttribute(a1,a2);
            }
         }
         else if (params[0].CompareNoCase(".OptionValue") == 0)
         {
            if (numPar > 3)
            {
               CString optionTypeName  = params[1];
               int optionId        = atoi(params[2]);
               CString optionValue = params[3];

               m_featureAttributes.getOptionTypeValues().addValue(optionTypeName,optionId,optionValue);
            }
         }
         else if (params[0].CompareNoCase(".ODB") == 0)
         {
            if (numPar > 2)
            {
               CString a1 = params[1];
               CString a2 = params[2];

               a1.MakeUpper();
               a2.MakeUpper();

               CAttDef* attDef = m_featureAttributes.addAttDef(stringToAttributeDefTypeTag(a1),a2);

               if (attDef->getType() == typeUndefined)
               {
                  log->writef(PrefixError,
                     "Attribute definition [%s] not defined in '%s' \n",
                     a1, settingsFilename);
               }
            }
         }
         else
         {
           setOption(line);
         }
      }
   }

   fclose(fp);

   return true;
}

bool COdbOptions::setOption(const CString& optionLine)
{
   bool retval = false;

   if (this->GetConsolidatorControl().Parse(optionLine))
   {
      // Parse did the whole job if it was a consolidator command, just indicate success now
      retval = true;
   }
   else
   {
      // Parse local option

      CSupString line(optionLine);
      CStringArray params;

      line.Trim();
      int numParams = line.ParseQuote(params," ");

      if (numParams > 1)
      {
         for (int optionIndex=0; optionIndex < m_options.GetSize(); optionIndex++)
         {
            COdbOption* option = m_options.GetAt(optionIndex);

            if (option != NULL)
            {
               params[0].Replace("_","");
               if (option->getName().CompareNoCase(params[0]) == 0)
               {
                  option->setValue(params[1]);

                  retval = true;
                  break;
               }
            }
         }
      }
   }

   return retval;
}

void COdbOptions::writeOptions(CWriteFormat& log)
{
   bool retval = false;

   log.writef("Option Settings - - - - - - - - - - -\n");

   for (int optionIndex=0;optionIndex < m_options.GetSize() && !retval;optionIndex++)
   {
      COdbOption* option = m_options.GetAt(optionIndex);

      if (option != NULL)
      {
         log.writef("%s = %s\n",(const char*)option->getName(),(const char*)option->getStringValue());
      }
   }

   log.writef("end of Option Settings - - - - - - - - - - -\n");
}

//_____________________________________________________________________________
COdbCompressableFileCache::COdbCompressableFileCache()
{
}

COdbCompressableFileCache::~COdbCompressableFileCache()
{
   cleanUp();
}

void COdbCompressableFileCache::cleanUp(CWriteFormat* log)
{
   bool keepUncompressedFilesFlag = getOdbOptions().getOptionFlag(optionKeepUncompressedFiles);

   for (POSITION pos = m_uncompressedFilePaths.GetHeadPosition();pos != NULL;)
   {
      CString filePath = m_uncompressedFilePaths.GetNext(pos);

      if (keepUncompressedFilesFlag)
      {
         log->writef(PrefixStatus,"Keeping uncompressed file '%s'\n",(const char*)filePath);
         continue;
      }

      int status = _unlink(filePath);

      if (log != NULL)
      {
         if (status == 0)
         {
            log->writef(PrefixStatus,"Deleted uncompressed file '%s'\n",(const char*)filePath);
         }
         else
         {
            log->writef(PrefixError,"While deleting uncompressed file '%s'\n",(const char*)filePath);
         }
      }
   }

   m_uncompressedFilePaths.RemoveAll();
}

bool COdbCompressableFileCache::uncompress(const CString& filePathString,CWriteFormat& errorLog)
{
   bool retval = true;

   CFilePath filePath(filePathString);
   CFilePath compressedFilePath(filePathString + ".z");

   struct _stat statBuffer;

   int compressedFileStatus   = _stat(compressedFilePath.getPath(),&statBuffer);
   int fileStatus             = _stat(filePath.getPath(),&statBuffer);
   bool compressedFileExists  = (compressedFileStatus != -1);
   bool fileExists            = (fileStatus           != -1);

   if (compressedFileExists)
   {
      if (fileExists)
      {
         errorLog.writef(PrefixStatus,
            "Replacing file '%s' by uncompressing '%s'.\n",
            (const char*)filePath.getPath(),(const char*)compressedFilePath.getFileName());

         if (_unlink(filePath.getPath()) != 0)
         {
            errorLog.writef(PrefixError,
               "Could not unlink '%s'\n",
               (const char*)filePath.getPath());
         }
      }
      else
      {
         errorLog.writef(PrefixStatus,
            "Uncompressing file '%s' into '%s'.\n",
            (const char*)compressedFilePath.getPath(),(const char*)filePath.getFileName());
      }

      m_uncompressedFilePaths.AddTail(filePath.getPath());

      CFilePath originalCompressedFilePath(compressedFilePath);
      bool longBaseFileName = (compressedFilePath.getBaseFileName().GetLength() > 8);

      if (longBaseFileName)
      {
         CString shortBaseFileName = compressedFilePath.getBaseFileName().Left(4) + compressedFilePath.getBaseFileName().Right(4);
         compressedFilePath.setBaseFileName(shortBaseFileName);

         if (rename(originalCompressedFilePath.getPath(),compressedFilePath.getPath()) != 0)
         {
            int localErrno = errno;
            CString errorString = strerror(localErrno);

            errorLog.writef(PrefixError,
               "Could not rename '%s' to '%s' - '%s'\n",
               (const char*)originalCompressedFilePath.getPath(),
               (const char*)compressedFilePath.getPath(),
               (const char*)errorString);

            retval = false;
         }
      }

      if (retval)
      {
         CFilePath dosCompressedFilePath(compressedFilePath.getShortPath());

         CString commandLine;
         commandLine.Format("%s%s \"%s\"", getApp().getUserPath(),
            (const char*)getOdbOptions().getOptionString(optionUncompressCommand),dosCompressedFilePath.getPath());

         if (ExecAndWait(commandLine, SW_HIDE, NULL) != 0)
         {
            errorLog.writef(PrefixError,
               "Error uncompressing file, commandLine='%s'\n",
               (const char*)commandLine);

            retval = false;
         }

         CFilePath dosFilePath(dosCompressedFilePath);
         dosFilePath.setExtension("");

         if (dosFilePath.getFileName().CompareNoCase(filePath.getFileName()) != 0)
         {
            if (rename(dosFilePath.getPath(),filePath.getPath()) != 0)
            {
               errorLog.writef(PrefixError,
                  "Could not rename '%s' to '%s'\n",
                  (const char*)dosFilePath.getPath(),(const char*)filePath.getPath());

               retval =  false;
            }
         }

         if (longBaseFileName)
         {
            if (rename(compressedFilePath.getPath(),originalCompressedFilePath.getPath()) != 0)
            {
               errorLog.writef(PrefixError,
                  "Could not rename '%s' to '%s'\n",
                  (const char*)dosFilePath.getPath(),(const char*)filePath.getPath());

               retval =  false;
            }
         }
      }
   }
   else if (fileExists)
   {
      errorLog.writef(PrefixStatus,
         "Compressed file '%s' does not exist, using existing uncompressed file '%s'.\n",
         (const char*)compressedFilePath.getPath(),(const char*)filePath.getFileName());
   }
   else
   {
      errorLog.writef(PrefixStatus,
         "Compressed file '%s' does not exist, and uncompressed file '%s' does not exist.\n",
         (const char*)compressedFilePath.getPath(),(const char*)filePath.getFileName());
   }

   return retval;
}

//_____________________________________________________________________________
bool COdbFileReader::s_writeUncompressedFileFlag = false;

COdbFileReader::COdbFileReader(int maxParams)
{
   m_maxParams = maxParams;

   if (m_maxParams > MaxParserParams) m_maxParams = MaxParserParams;
   else if (m_maxParams < 1) m_maxParams = 1;

   m_numBuf[MaxNumLength] = '\0';
   m_inchesPerUnit    = 1.;
   m_numRecordsRead   = 0;
   m_fileSize         = 0;
   m_stdioFile        = NULL;
   m_compressedFile   = NULL;
   m_uncompressedFile = NULL;
}

COdbFileReader::~COdbFileReader()
{
   delete m_stdioFile;
   delete m_compressedFile;
   delete m_uncompressedFile;
}

bool COdbFileReader::open(const CString& filePathString)
{
   bool retval;

   m_filePath = filePathString;

   close();

   CFilePath filePath(filePathString);

   if (filePath.getExtension().CompareNoCase("z") == 0)
   {
      m_compressedFile = new CStdioUnixCompressedFile();
      m_openStatus = m_compressedFile->openRead(filePathString);

      retval = (m_openStatus == statusSucceeded);
   }
   else
   {
      m_stdioFile = new CStdioFile();

      retval = (m_stdioFile->Open(filePathString,CFile::modeRead | CFile::shareDenyNone) != 0);
      m_openStatus = (retval ? statusSucceeded : statusFileOpenFailure);
   }

   m_readTime.setStartTime();

   return retval;
}

FileStatusTag COdbFileReader::openFile(const CString& filePathString)
{
   FileStatusTag retval;

   m_filePath = filePathString;

   close();

   CFilePath filePath(filePathString);

   if (filePath.getExtension().CompareNoCase("z") == 0)
   {
      m_compressedFile = new CStdioUnixCompressedFile();

      retval = m_compressedFile->openRead(filePathString);

      if (retval == statusInvalidHeader)
      {
         delete m_compressedFile;
         m_compressedFile = new CStdioGzFile();

         retval = m_compressedFile->openRead(filePathString);
      }

      if (retval == statusSucceeded && s_writeUncompressedFileFlag)
      {
         CFilePath uncompressedFilePath(filePath);
         uncompressedFilePath.setExtension("");

         m_uncompressedFile = new CStdioFile();

         if (m_uncompressedFile->Open(uncompressedFilePath.getPath(),CFile::modeWrite | CFile::modeCreate) == 0)
         {
            delete m_uncompressedFile;
            m_uncompressedFile = NULL;
         }
      }
   }
   else
   {
      m_stdioFile = new CStdioFile();

      retval = ((m_stdioFile->Open(filePathString,CFile::modeRead | CFile::shareDenyNone) != 0) ?
                  statusSucceeded : statusFileOpenFailure);
   }

   m_openStatus = retval;

   if (m_openStatus != statusFileOpenFailure)
   {
      m_fileSize = getFileLength();
   }

   return retval;
}

void COdbFileReader::close()
{
   m_readTime.setStopTime();

   delete m_stdioFile;
   m_stdioFile = NULL;

   delete m_compressedFile;
   m_compressedFile = NULL;

   delete m_uncompressedFile;
   m_uncompressedFile = NULL;
}

void COdbFileReader::writeReadStatistics(CWriteFormat* log,int prefix)
{
   if (log != NULL)
   {
      double seconds = m_readTime.getNonZeroIntervalInSeconds();
      double kiloBytesPerSecond = (m_fileSize / seconds)/1000.;

      log->writef(prefix,
         "Read %d records in file '%s' with %d bytes in %.3f Seconds, %.3f KBytes/sec\n",
         m_numRecordsRead,m_filePath,m_fileSize,seconds,kiloBytesPerSecond);
   }
}

void COdbFileReader::setRecord(const CString& record)
{
   m_record = record;

   parse();
}

bool COdbFileReader::readRecord()
{
   bool retval = false;

   if (m_stdioFile != NULL)
   {
      retval = (m_stdioFile->ReadString(m_record) != 0);
   }
   else if (m_compressedFile != NULL)
   {
      retval = (m_compressedFile->readString(m_record) == statusSucceeded);
   }

   if (retval)
   {
      m_numRecordsRead++;

      if (m_uncompressedFile != NULL)
      {
         m_uncompressedFile->WriteString(m_record);
      }

      parse();
   }

   return retval;
}

FileStatusTag COdbFileReader::readFileRecord(CWriteFormat* writeFormat)
{
   FileStatusTag retval = statusFileReadFailure;

   if (m_stdioFile != NULL)
   {
      retval = ((m_stdioFile->ReadString(m_record) != 0) ? statusSucceeded : statusEof);
   }
   else if (m_compressedFile != NULL)
   {
      retval = m_compressedFile->readString(m_record);
   }

   if (retval == statusSucceeded)
   {
      m_numRecordsRead++;

      if (m_uncompressedFile != NULL)
      {
         m_uncompressedFile->WriteString(m_record);
         m_uncompressedFile->Write("\n",1);
      }

      parse();
   }
   else if (writeFormat != NULL)
   {
      if (retval == statusEof)
      {
         writeFormat->writef(PrefixStatus,"%d records read.\n",getNumRecordsRead());
      }
      else
      {
         writeFormat->writef(PrefixError,"status = '%s' while reading '%s'.\n",
            (const char*)fileStatusTagToString(retval),(const char*)getFilePath());
      }
   }

   return retval;
}

void COdbFileReader::parse()
{
   char* p = m_record.GetBuffer();

   for (m_numParams = 0;m_numParams < m_maxParams;m_numParams++)
   {
      while (*p == ' ')
      {
         p++;
      }

      if (*p == '\0')
      {
         break;
      }

      m_start[m_numParams] = p;

      if (*p == '\'')
      {
         p++;

         while (*p != '\'' && *p != '\0')
         {
            p++;
         }

         if (*p == '\'')
         {
            p++;
         }
      }

      while (*p != ' ' && *p != ';' && *p != ',' && *p != '\0')
      {
         p++;
      }

      m_end[m_numParams] = p;

      while (*p == ' ')
      {
         p++;
      }

      if (*p == ';' || *p == ',')
      {
         p++;
      }
   }
}

int COdbFileReader::getNumParams()
{
   return m_numParams;
}

CString COdbFileReader::getParam(int index)
{
   CString param;

   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      char* buf = param.GetBufferSetLength(len);

      strncpy(buf,m_start[index],len);

      param.ReleaseBuffer(len);
   }

   return param;
}

CString COdbFileReader::getParamToEndOfLine(int index)
{
   CString param;

   if (index >= 0 && index < m_numParams)
   {
      param = m_start[index];
   }

   return param;
}

void COdbFileReader::getParam(CString& param,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      char* buf = param.GetBufferSetLength(len);

      strncpy(buf,m_start[index],len);
   }
}

void COdbFileReader::getParam(int& iParam,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      if (len > MaxNumLength) len = MaxNumLength;

      strncpy(m_numBuf,m_start[index],len);

      m_numBuf[len] = '\0';

      iParam = atoi(m_numBuf);
   }
   else
   {
      iParam = 0;
   }
}

void COdbFileReader::getParam(double& fParam,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      if (len > MaxNumLength) len = MaxNumLength;

      strncpy(m_numBuf,m_start[index],len);

      m_numBuf[len] = '\0';

      fParam = atof(m_numBuf);
   }
   else
   {
      fParam = 0.;
   }
}

int COdbFileReader::getFileLength() 
{ 
   int retval = 0;

   if (m_stdioFile != NULL)
   {
      retval = (int)m_stdioFile->GetLength(); 
   }
   else if (m_compressedFile != NULL)
   {
      retval = m_compressedFile->getCompressedSize(); 
   }

   return retval;
}

int COdbFileReader::getFilePosition() 
{ 
   int retval = 0;

   if (m_stdioFile != NULL)
   {
      retval = (int)m_stdioFile->GetPosition(); 
   }
   else if (m_compressedFile != NULL)
   {
      retval = m_compressedFile->getNumBytesInput(); 
   }

   return retval;
}

//_____________________________________________________________________________
COdbMemFileReaderTester::COdbMemFileReaderTester(int maxParams) :
   COdbMemFileReader(maxParams), m_fileReader(maxParams)
{
}

bool COdbMemFileReaderTester::open(const CString& filePath)
{
   bool f1 = COdbMemFileReader::open(filePath);
   bool f0 = m_fileReader.open(filePath);

   testAssert(f0 == f1);

   return f1;
}

void COdbMemFileReaderTester::close()
{
   m_fileReader.close();
   COdbMemFileReader::close();
}

bool COdbMemFileReaderTester::readRecord()
{
   bool f0 = m_fileReader.readRecord();
   bool f1 = COdbMemFileReader::readRecord();

   testAssert(f0 == f1);

   if (f0 && f1)
   {
      testAssert(m_fileReader.getNumParams() == COdbMemFileReader::getNumParams());
      testAssert(m_fileReader.getRecord()    == COdbMemFileReader::getRecord());

      int maxInd = min(m_fileReader.getNumParams(),COdbMemFileReader::getNumParams());

      for (int ind = 0;ind < maxInd;ind++)
      {
         testAssert(m_fileReader.getParam(ind)   == COdbMemFileReader::getParam(ind));
      }
   }

   return f1;
}

bool COdbMemFileReaderTester::testAssert(bool assertFlag)
{
   if (!assertFlag)
   {
      bool notAssertedFlag = true;
   }

   return assertFlag;
}

//_____________________________________________________________________________
COdbMemFileReader::COdbMemFileReader(int maxParams)
{
   m_maxParams = maxParams;

   if (m_maxParams > MaxParserParams) m_maxParams = MaxParserParams;
   else if (m_maxParams < 1) m_maxParams = 1;

   m_numBuf[MaxNumLength] = '\0';
   m_inchesPerUnit  = 1.;
   m_numRecordsRead = 0;

   m_fileHandle    = INVALID_HANDLE_VALUE;
   m_mappingHandle = NULL;
   m_baseAddress   = NULL;
}

COdbMemFileReader::~COdbMemFileReader()
{
   close();
}

bool COdbMemFileReader::open(const CString& filePath)
{
   bool retval = false;

   close();
   m_filePath = filePath;

   m_fileHandle = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,
      NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);

   if (m_fileHandle != INVALID_HANDLE_VALUE)
   {
      m_mappingHandle = CreateFileMapping(m_fileHandle,NULL,PAGE_READONLY,0,0,NULL);

      if (m_mappingHandle != NULL)
      {
         m_baseAddress = (char*)MapViewOfFile(m_mappingHandle,FILE_MAP_READ,0,0,0);

         if (m_baseAddress != NULL)
         {
            retval = true;

            m_currentAddress = m_baseAddress;
            m_fileSize       = GetFileSize(m_fileHandle,NULL);
            m_limitAddress   = m_baseAddress + m_fileSize;
         }
      }
   }

   if (!retval)
   {
      if (m_mappingHandle != NULL)
      {
         CloseHandle(m_mappingHandle);
         m_mappingHandle = NULL;
      }

      if (m_fileHandle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(m_fileHandle);
         m_fileHandle    = INVALID_HANDLE_VALUE;
      }
   }

   return retval;
}

void COdbMemFileReader::close()
{
   if (m_baseAddress != NULL)
   {
      UnmapViewOfFile(m_baseAddress);
      CloseHandle(m_mappingHandle);
      CloseHandle(m_fileHandle);
   }

   m_fileHandle     = INVALID_HANDLE_VALUE;
   m_mappingHandle  = NULL;
   m_baseAddress    = NULL;
   m_numRecordsRead = 0;
   m_recordStart    = NULL;
   m_recordEnd      = NULL;
}

bool COdbMemFileReader::readRecord()
{
   bool retval = (m_currentAddress < m_limitAddress);

   if (retval)
   {
      m_numRecordsRead++;
      parse();
   }

   return retval;
}

void COdbMemFileReader::parse()
{
   const char*& p = m_currentAddress;
   m_recordStart = p;

   // assume that the last character in a file ia a '\n'

   for (m_numParams = 0;;m_numParams++)
   {
      if (m_numParams >= m_maxParams)
      {
         while (*p != '\n')
         {
            p++;
         }

         m_recordEnd = p++;

         break;
      }

      while (*p == ' ')
      {
         p++;
      }

      if (*p == '\n')
      {
         m_recordEnd = p++;
         break;
      }

      m_start[m_numParams] = p;

      if (*p == '\'')
      {
         while (*p != '\'' && *p != '\n')
         {
            p++;
         }

         if (*p == '\'')
         {
            p++;
         }
      }

      while (*p != ' ' && *p != ';' && *p != ',' && *p != '\n')
      {
         p++;
      }

      m_end[m_numParams] = p;

      while (*p == ' ')
      {
         p++;
      }

      if (*p == ';' || *p == ',')
      {
         p++;
      }
   }
}

int COdbMemFileReader::getNumParams()
{
   return m_numParams;
}

CString COdbMemFileReader::getRecord()
{
   if (m_recordStart != NULL)
   {
      int len = m_recordEnd - m_recordStart;

      char* buf = m_record.GetBufferSetLength(len);

      strncpy(buf,m_recordStart,len);
   }

   return m_record;
}

CString COdbMemFileReader::getParam(int index)
{
   CString param;

   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      char* buf = param.GetBufferSetLength(len);

      strncpy(buf,m_start[index],len);
   }

   return param;
}

void COdbMemFileReader::getParam(CString& param,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      char* buf = param.GetBufferSetLength(len);

      strncpy(buf,m_start[index],len);
   }
}

void COdbMemFileReader::getParam(int& iParam,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      if (len > MaxNumLength) len = MaxNumLength;

      strncpy(m_numBuf,m_start[index],len);

      m_numBuf[len] = '\0';

      iParam = atoi(m_numBuf);
   }
   else
   {
      iParam = 0;
   }
}

void COdbMemFileReader::getParam(double& fParam,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      if (len > MaxNumLength) len = MaxNumLength;

      strncpy(m_numBuf,m_start[index],len);

      m_numBuf[len] = '\0';

      fParam = atof(m_numBuf);
   }
   else
   {
      fParam = 0.;
   }
}

//_____________________________________________________________________________
COdbFeatureSymbol::COdbFeatureSymbol(CString name)
{
   m_name              = name.MakeLower();
   m_symbolFeatureFile = NULL;

   m_dimension[0]      = 0.;
   m_dimension[1]      = 0.;
   m_dimension[2]      = 0.;
   m_dimension[3]      = 0.;
   m_dimension[4]      = 0.;
   m_dimension[5]      = 0.;
   m_rotation          = 0.;

   m_apertureBlock     = NULL;

   parseName();

}

COdbFeatureSymbol::~COdbFeatureSymbol()
{
   delete m_symbolFeatureFile;
}

void COdbFeatureSymbol::parseName()
{
   CStringArray params;
   CString rotationString;
   CString typeString;

   int minParamCount = -1;
   int maxParamCount =  0;
   int numParams     =  0;
   int pos           = m_name.FindOneOf("0123456789.");

   if (pos > 0)
   {
      typeString      = m_name.Left(pos);
      CSupString name = m_name.Mid(pos);
      numParams       = name.Parse(params,"x");

      m_dimension[0]      = 0.;
      m_dimension[1]      = 0.;
      m_dimension[2]      = 0.;
      m_dimension[3]      = 0.;
      m_dimension[4]      = 0.;
      m_dimension[5]      = 0.;
      m_rotation          = 0.;

      if (numParams > 0)
      {
         int pos = params[numParams - 1].ReverseFind('_');

         if (pos > 0)
         {
            rotationString = params[numParams - 1].Mid(pos + 1);
            params[numParams - 1] = params[numParams - 1].Left(pos);
         }
      }

      if (typeString == "r")
      {
         m_type = standardSymbolCircle;
         minParamCount = 1;
      }
      else if (typeString == "s")
      {
         m_type = standardSymbolSquare;
         minParamCount = 1;
      }
      else if (typeString == "rect")
      {
         m_type = standardSymbolRectangle;
         minParamCount = 2;

         if (numParams > 2)
         {
            if (params[2].Left(1) == "r")
            {
               m_type = standardSymbolRoundedRectangle;
               minParamCount = 3;
               maxParamCount = 4;
               params[2] = params[2].Mid(1);
            }
            else if (params[2].Left(1) == "c")
            {
               m_type = standardSymbolChamferedRectangle;
               minParamCount = 3;
               maxParamCount = 4;
               params[2] = params[2].Mid(1);
            }
         }
      }
      else if (typeString == "oval")
      {
         m_type = standardSymbolOval;
         minParamCount = 2;
      }
      else if (typeString == "di")
      {
         m_type = standardSymbolDiamond;
         minParamCount = 2;
      }
      else if (typeString == "oct")
      {
         m_type = standardSymbolOctagon;
         minParamCount = 3;
      }
      else if (typeString == "donut_r")
      {
         m_type = standardSymbolRoundDonut;
         minParamCount = 2;
      }
      else if (typeString == "donut_s")
      {
         m_type = standardSymbolSquareDonut;
         minParamCount = 2;
      }
      else if (typeString == "hex_l")
      {
         m_type = standardSymbolHorizontalHexagon;
         minParamCount = 3;
      }
      else if (typeString == "hex_s")
      {
         m_type = standardSymbolVerticalHexagon;
         minParamCount = 3;
      }
      else if (typeString == "bfr")
      {
         m_type = standardSymbolButterfly;
         minParamCount = 1;
      }
      else if (typeString == "bfs")
      {
         m_type = standardSymbolSquareButterfly;
         minParamCount = 1;
      }
      else if (typeString == "tri")
      {
         m_type = standardSymbolTriangle;
         minParamCount = 2;
      }
      else if (typeString == "oval_h")
      {
         m_type = standardSymbolHalfOval;
         minParamCount = 2;
      }
      else if (typeString == "thr")
      {
         m_type = standardSymbolRoundThermalRounded;
         minParamCount = 5;
      }
      else if (typeString == "ths")
      {
         m_type = standardSymbolRoundThermalSquare;
         minParamCount = 5;
      }
      else if (typeString == "s_ths")
      {
         m_type = standardSymbolSquareThermal;
         minParamCount = 5;
      }
      else if (typeString == "s_tho")
      {
         m_type = standardSymbolSquareThermalOpenCorners;
         minParamCount = 5;
      }
      else if (typeString == "sr_ths")
      {
         m_type = standardSymbolSquareRoundThermal;
         minParamCount = 5;
      }
      else if (typeString == "rc_ths")
      {
         m_type = standardSymbolRectangularThermal;
         minParamCount = 6;
      }
      else if (typeString == "rc_tho")
      {
         m_type = standardSymbolRectangularThermalOpenCorners;
         minParamCount = 6;
      }
      else if (typeString == "el")
      {
         m_type = standardSymbolEllipse;
         minParamCount = 2;
      }
      else if (typeString == "moire")
      {
         m_type = standardSymbolMoire;
         minParamCount = 6;
      }
      else if (typeString == "hole")
      {
         m_type = standardSymbolHole;
         minParamCount = 4;
      }
      else if (typeString == "null")
      {
         m_type = standardSymbolNull;
         m_dimension[0] = 1.;
         minParamCount = 1;
      }
      else
      {
         m_type = standardSymbolUndefined;
      }
   }

   if (m_type != standardSymbolUndefined)
   {
      if (maxParamCount == 0) maxParamCount = minParamCount;

      if (numParams < minParamCount || numParams > maxParamCount)
      {
         m_type = standardSymbolInvalid;
      }
      else
      {
         char* endPtr;

         for (int ind = 0;ind < numParams;ind++)
         {
            // Case dts0100428976 -- The problem discovered in this case was that aperture names
            // that were intended to represent customer apertures were being "recognized" as
            // ODB++ Standard Apertures. Due to the name construction the values extracted were
            // nonsensical, but to the computer they looked like valid numbers. This in turn
            // cause the aperture to be treated as a Standard aperture down the line, and so the
            // related "feature" file was not being read and the camcad complex aperture that
            // should have been built was not built. Being more picky about extracting the
            // parameter values here fixes that.

            // All values are numeric except for HOLE, where 2nd param is one of three chars.
            // If we have that special case, then let those special chars through.
            // Otherwise enforce strictly numeric value.

            if (m_type == standardSymbolHole && ind == 1 /*2nd param*/)
            {
               if (
                  (params.GetAt(ind).CompareNoCase("p") == 0) || // plated
                  (params.GetAt(ind).CompareNoCase("n") == 0) || // non-plated
                  (params.GetAt(ind).CompareNoCase("v") == 0)    // via
                  )
               {
                  // The m_dimension array is double, so it can not store this char data.
                  // So this information is lost. There is no case against this as a problem, so
                  // I will take that to mean it does not matter.
                  // m_dimension[ind] = params.GetAt(ind);  
               }
               else
               {
                  m_type = standardSymbolInvalid;
               }
            }
            else
            {
               // Numeric values in scientific notation are not supported by the file format.
               // But some constructions of user data may look like scientific notation to
               // strtod(), in particular "d" and "e" values cause a problem because they both
               // form legitimate scientific notation values. 
               // If such chars exist then replace them with a space. This will in turn cause 
               // strtod() to stop parsing short of the whole string. This will be noticed and then
               // the type will be set to invalid, causing the aperture to be processed as a
               // complex aperture later.

               CString numberStr(params[ind]);
               numberStr.MakeLower();
               numberStr.Replace("d", " ");
               numberStr.Replace("e", " ");

               m_dimension[ind] = strtod(numberStr,&endPtr);

               if (*endPtr != '\0')
               {
                  m_type = standardSymbolInvalid;
               }
            }
         }

         if (rotationString.GetLength() != 0)
         {
            m_rotation = normalizeDegrees(-strtod(rotationString,&endPtr));

            if (*endPtr != '\0')
            {
               m_type = standardSymbolInvalid;
            }
         }
      }
   }

   int numSpokes = 0;

   switch (m_type)
   {
   case standardSymbolCircle:
   case standardSymbolRoundDonut:                     
   case standardSymbolHole:                           
      m_symmetry = 0;  
      break;
   case standardSymbolSquare:                         
   case standardSymbolOctagon:                        
   case standardSymbolSquareDonut:                    
   case standardSymbolMoire:                          
      m_symmetry = 90;  
      break;
   case standardSymbolOval:                           
   case standardSymbolHorizontalHexagon:              
   case standardSymbolVerticalHexagon:                
   case standardSymbolButterfly:                      
   case standardSymbolSquareButterfly:                
   case standardSymbolEllipse:                        
      m_symmetry = 180;  
      break;
   case standardSymbolRectangle:                      
   case standardSymbolDiamond:                        
      m_symmetry = (fpeq3(m_dimension[0],m_dimension[1]) ? 90 : 180);  
      break;
   case standardSymbolRoundedRectangle:               
   case standardSymbolChamferedRectangle:             
      m_symmetry = max((fpeq3(m_dimension[0],m_dimension[1]) ? 90 : 180),cornerSymmetry(DcaRound(m_dimension[3])));  
      break;
   case standardSymbolRoundThermalRounded:    
   case standardSymbolRoundThermalSquare:             
   case standardSymbolSquareThermalOpenCorners:       
      numSpokes = DcaRound(m_dimension[3]);

      if (numSpokes > 0 && ((360 / numSpokes) * numSpokes == 360))
      {
         m_symmetry = 360 / numSpokes;
      }
      else
      {
         m_symmetry = 360;  
      }

      break;
   case standardSymbolSquareThermal:                  
   case standardSymbolSquareRoundThermal:             
      numSpokes = DcaRound(m_dimension[3]);

      if ((numSpokes % 4) == 0)
      {
         m_symmetry = 90;
      }
      else if ((numSpokes % 2) == 0)
      {
         m_symmetry = 180;
      }
      else
      {
         m_symmetry = 360;
      }

      break;
   case standardSymbolRectangularThermal:             
   case standardSymbolRectangularThermalOpenCorners:  
      numSpokes = DcaRound(m_dimension[3]);

      if ((numSpokes % 4) == 0 && fpeq3(m_dimension[0],m_dimension[1]))
      {
         m_symmetry = 90;
      }
      else if ((numSpokes % 2) == 0)
      {
         m_symmetry = 180;
      }
      else
      {
         m_symmetry = 360;
      }

      break;
   case standardSymbolTriangle:                       
   case standardSymbolHalfOval:  
   case standardSymbolNull:                           
   case standardSymbolInvalid:                        
   case standardSymbolUndefined:                      
   default:
      m_symmetry = 360;  
      break;
   }
}

bool COdbFeatureSymbol::readSymbolFeatureFile(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   bool retval = false;

   if (m_symbolFeatureFile == NULL)
   {
      m_symbolFeatureFile = new COdbFeatureFile(m_name);

      retval = m_symbolFeatureFile->readSymbolFeatures(odbPpDatabase,log);

      if (!retval)
      {
         delete m_symbolFeatureFile;
         m_symbolFeatureFile = NULL;
      }
   }

   return retval;
}

bool COdbFeatureSymbol::isEquivalent(const COdbFeatureSymbol& other) const
{
   bool retval = (m_type == other.m_type && m_name == other.m_name);

   return retval;
}

CString COdbFeatureSymbol::getApertureNameDescriptor()
{
   CString typeName;

   switch (m_type)
   {
   case standardSymbolCircle:                         typeName = "Circle";                    break;
   case standardSymbolSquare:                         typeName = "Square";                    break;
   case standardSymbolRectangle:                      typeName = "Rectangle";                 break;
   case standardSymbolRoundedRectangle:               typeName = "RoundedRectangle";          break;
   case standardSymbolChamferedRectangle:             typeName = "ChamferedRectangle";        break;
   case standardSymbolOval:                           typeName = "Oval";                      break;
   case standardSymbolDiamond:                        typeName = "Diamond";                   break;
   case standardSymbolOctagon:                        typeName = "Octagon";                   break;
   case standardSymbolRoundDonut:                     typeName = "RoundDonut";                break;
   case standardSymbolSquareDonut:                    typeName = "SquareDonut";               break;
   case standardSymbolHorizontalHexagon:              typeName = "HorizontalHexagon";         break;
   case standardSymbolVerticalHexagon:                typeName = "VerticalHexagon";           break;
   case standardSymbolButterfly:                      typeName = "Butterfly";                 break;
   case standardSymbolSquareButterfly:                typeName = "SquareButterfly";           break;
   case standardSymbolTriangle:                       typeName = "Triangle";                  break;
   case standardSymbolHalfOval:                       typeName = "HalfOval";                  break;
   case standardSymbolRoundThermalRounded:            typeName = "RoundThermalRounded";       break;
   case standardSymbolRoundThermalSquare:             typeName = "RoundThermalSquare";        break;
   case standardSymbolSquareThermal:                  typeName = "SquareThermal";             break;
   case standardSymbolSquareThermalOpenCorners:       typeName = "SquareThermalOpenCorners";  break;
   case standardSymbolSquareRoundThermal:             typeName = "SquareRoundThermal";        break;
   case standardSymbolRectangularThermal:             typeName = "RectThermal";               break;
   case standardSymbolRectangularThermalOpenCorners:  typeName = "RectThermalOpenCorners";    break;  
   case standardSymbolEllipse:                        typeName = "Ellipse";                   break;
   case standardSymbolMoire:                          typeName = "Moire";                     break;
   case standardSymbolHole:                           typeName = "Hole";                      break;
   case standardSymbolNull:                           typeName = "Null";                      break;
   case standardSymbolInvalid:                        typeName = "Invalid";                   break;
   case standardSymbolUndefined:                      typeName = "Undefined";                 break;
   }

   CString retval = "AP_" + typeName;

   return retval;
}

ApertureShapeTag COdbFeatureSymbol::getApertureShape()
{
   ApertureShapeTag retval = apertureComplex;

   switch (m_type)
   {
   case standardSymbolCircle:                         retval = apertureRound;      break;
   case standardSymbolSquare:                         retval = apertureSquare;     break;
   case standardSymbolRectangle:                      retval = apertureRectangle;  break;
   case standardSymbolOval:                           retval = apertureOblong;     break;
   case standardSymbolRoundDonut:                     retval = apertureDonut;      break;
   case standardSymbolHole:                           retval = apertureRound;      break;                         
   case standardSymbolNull:                           retval = apertureRound;      break;                         
   }

   return retval;
}

void buildSpokes(CCamCadDatabase& camCadDatabase,DataStruct* polyStruct,
   double outerRadius,double startAngleDegrees,int numSpokes,double spokeWidth)
{
   int widthIndex = camCadDatabase.getDefinedWidthIndex(spokeWidth);

   for (int spokeInd = 0;spokeInd < numSpokes;spokeInd++)
   {
      double angle = degreesToRadians(startAngleDegrees + (spokeInd * 360./numSpokes));

      CPoly* poly = camCadDatabase.addOpenPoly(polyStruct,widthIndex);

      double x = outerRadius * cos(angle);
      double y = outerRadius * sin(angle);

      camCadDatabase.addVertex(poly,0.,0.);
      camCadDatabase.addVertex(poly, x, y);
   }
}

void buildThermalOpenCorners(CCamCadDatabase& camCadDatabase,DataStruct* polyStruct,
   double width,double height,double airGap,
   double startAngleDegrees,int numSpokes,double spokeWidth)
{
   double id = width - airGap;
   double theta = atan(((spokeWidth/2.)/id - sin(Pi/numSpokes)) / (-cos(Pi/numSpokes)));
   double w = id * tan(theta);
   double alpha = (Pi / numSpokes) - theta;
   double thetaDegrees = radiansToDegrees(theta);

   for (int ind = 0;ind < numSpokes;ind++)
   {
      CTMatrix matrix;
      matrix.rotateDegrees(startAngleDegrees - 2.*thetaDegrees + ind*360./numSpokes);
      matrix.scale(1.,height/width);

      CPoint2d p0(id         , w);
      CPoint2d p1(id         ,-w);
      CPoint2d p2(id + airGap,-w);
      CPoint2d p3(id + airGap, w);

      matrix.transform(p0);
      matrix.transform(p1);
      matrix.transform(p2);
      matrix.transform(p3);

      CPoly* poly = camCadDatabase.addFilledPoly(polyStruct,0);
      camCadDatabase.addVertex(poly,p0.x,p0.y);
      camCadDatabase.addVertex(poly,p1.x,p1.y);
      camCadDatabase.addVertex(poly,p2.x,p2.y);
      camCadDatabase.addVertex(poly,p3.x,p3.y);
      camCadDatabase.addVertex(poly,p0.x,p0.y);
   }
}

BlockStruct* COdbFeatureSymbol::getDefinedAperture(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   if (m_apertureBlock == NULL)
   {
      CString apertureName = odbPpDatabase.getCamCadDatabase().getNewBlockName("AP_" + m_name,"-%d");
      CCamCadDatabase& camCadDatabase = odbPpDatabase.getCamCadDatabase();

      if (getApertureShape() == apertureComplex)
      {
         m_apertureBlock = camCadDatabase.getDefinedAperture(apertureName,
            getApertureShape(),0.,0.,0.,0.,degreesToRadians(m_rotation));

         if (m_symbolFeatureFile != NULL)
         {
            BlockStruct* complexApertureBlock = m_symbolFeatureFile->instantiateFeatures(odbPpDatabase,NULL,log);
            m_apertureBlock->setSizeA((DbUnit)(complexApertureBlock->getBlockNumber()));
         }
         else
         {
            const DbFlag dbFlag = 0;
            const bool positive = false;
            const bool negative = true;

            CString complexApertureBlockNamePrefix = m_name + "-Cmplx";

            BlockStruct* complexApertureBlock = 
               camCadDatabase.getNewBlock(complexApertureBlockNamePrefix,"-%d",blockTypeUnknown);

            m_apertureBlock->setSizeA((DbUnit)(complexApertureBlock->getBlockNumber()));

            int floatLayerIndex = camCadDatabase.getLayerIndex(ccLayerFloat);

            DataStruct* polyStruct = camCadDatabase.addPolyStruct(
                                       complexApertureBlock,floatLayerIndex,dbFlag,positive, graphicClassNormal);

            CPoly* poly;

            switch (m_type)
            {
            case standardSymbolRoundedRectangle:  
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  double  radius     = getDimensionInPageUnits(2);
                  int corners        = DcaRound(m_dimension[3]);
                  double radius1     = (hasCorner(corners,1) ? radius : 0.);
                  double radius2     = (hasCorner(corners,2) ? radius : 0.);
                  double radius3     = (hasCorner(corners,3) ? radius : 0.);
                  double radius4     = (hasCorner(corners,4) ? radius : 0.);
                  double bulge90     = tan(degreesToRadians(90) / 4.);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  // 2--1
                  // |  |
                  // 3--4
                  // corner 1
                  if (radius1 != 0.)
                  {
                     camCadDatabase.addVertex(poly,halfWidth,halfHeight - radius,bulge90);
                  }

                  camCadDatabase.addVertex(poly,halfWidth - radius1,halfHeight);

                  // corner 2
                  if (radius2 != 0)
                  {
                     camCadDatabase.addVertex(poly,-halfWidth + radius,halfHeight,bulge90);
                  }

                  camCadDatabase.addVertex(poly,-halfWidth,halfHeight - radius2);

                  // corner 3
                  if (radius3 != 0)
                  {
                     camCadDatabase.addVertex(poly,-halfWidth ,-halfHeight + radius,bulge90);
                  }

                  camCadDatabase.addVertex(poly,-halfWidth + radius3,-halfHeight);

                  // corner 4
                  if (radius4 != 0)
                  {
                     camCadDatabase.addVertex(poly,halfWidth - radius,-halfHeight,bulge90);
                  }

                  camCadDatabase.addVertex(poly,halfWidth,-halfHeight + radius4);

                  camCadDatabase.addVertex(poly,halfWidth,halfHeight - radius1);
               }

               break;
            case standardSymbolChamferedRectangle:             
               {  
                  double halfWidth  = getDimensionInPageUnits(0)/2.;
                  double halfHeight = getDimensionInPageUnits(1)/2.;
                  double  radius    = getDimensionInPageUnits(2);
                  int corners       = DcaRound(m_dimension[3]);
                  double radius1    = (hasCorner(corners,1) ? radius : 0.);
                  double radius2    = (hasCorner(corners,2) ? radius : 0.);
                  double radius3    = (hasCorner(corners,3) ? radius : 0.);
                  double radius4    = (hasCorner(corners,4) ? radius : 0.);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  // 2--1
                  // |  |
                  // 3--4
                  // corner 1
                  if (radius1 != 0.)
                  {
                     camCadDatabase.addVertex(poly,halfWidth,halfHeight - radius);
                  }

                  camCadDatabase.addVertex(poly,halfWidth - radius1,halfHeight);

                  // corner 2
                  if (radius2 != 0)
                  {
                     camCadDatabase.addVertex(poly,-halfWidth + radius,halfHeight);
                  }

                  camCadDatabase.addVertex(poly,-halfWidth,halfHeight - radius2);

                  // corner 3
                  if (radius3 != 0)
                  {
                     camCadDatabase.addVertex(poly,-halfWidth ,-halfHeight + radius);
                  }

                  camCadDatabase.addVertex(poly,-halfWidth + radius3,-halfHeight);

                  // corner 4
                  if (radius4 != 0)
                  {
                     camCadDatabase.addVertex(poly,halfWidth - radius,-halfHeight);
                  }

                  camCadDatabase.addVertex(poly,halfWidth,-halfHeight + radius4);

                  camCadDatabase.addVertex(poly,halfWidth,halfHeight - radius1);
               }

               break;
            case standardSymbolDiamond:                        
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,         0, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth,          0);
                  camCadDatabase.addVertex(poly,         0,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth,          0);
                  camCadDatabase.addVertex(poly,         0, halfHeight);
               }

               break;
            case standardSymbolOctagon:              
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  double cornerSize  = getDimensionInPageUnits(2);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, halfWidth             , halfHeight - cornerSize);
                  camCadDatabase.addVertex(poly, halfWidth - cornerSize, halfHeight             );
                  camCadDatabase.addVertex(poly,-halfWidth + cornerSize, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth             , halfHeight - cornerSize);
                  camCadDatabase.addVertex(poly,-halfWidth             ,-halfHeight + cornerSize);
                  camCadDatabase.addVertex(poly,-halfWidth + cornerSize,-halfHeight             );
                  camCadDatabase.addVertex(poly, halfWidth - cornerSize,-halfHeight             );
                  camCadDatabase.addVertex(poly, halfWidth             ,-halfHeight + cornerSize);
                  camCadDatabase.addVertex(poly, halfWidth             , halfHeight - cornerSize);
               }

               break;
            case standardSymbolSquareDonut:                    
               {  
                  double outerRadius = getDimensionInPageUnits(0)/2.;
                  double innerRadius = getDimensionInPageUnits(1)/2.;

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, outerRadius, outerRadius);
                  camCadDatabase.addVertex(poly,-outerRadius, outerRadius);
                  camCadDatabase.addVertex(poly,-outerRadius,-outerRadius);
                  camCadDatabase.addVertex(poly, outerRadius,-outerRadius);
                  camCadDatabase.addVertex(poly, outerRadius, outerRadius);

                  polyStruct = camCadDatabase.addPolyStruct(complexApertureBlock,
                     floatLayerIndex,dbFlag,negative, graphicClassNormal);
                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, innerRadius, innerRadius);
                  camCadDatabase.addVertex(poly,-innerRadius, innerRadius);
                  camCadDatabase.addVertex(poly,-innerRadius,-innerRadius);
                  camCadDatabase.addVertex(poly, innerRadius,-innerRadius);
                  camCadDatabase.addVertex(poly, innerRadius, innerRadius);
               }

               break;
            case standardSymbolHorizontalHexagon:              
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  double cornerSize  = getDimensionInPageUnits(2);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, halfWidth             ,          0);
                  camCadDatabase.addVertex(poly, halfWidth - cornerSize, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth + cornerSize, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth             ,          0);
                  camCadDatabase.addVertex(poly,-halfWidth + cornerSize,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth - cornerSize,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth             ,          0);
               }

               break;
            case standardSymbolVerticalHexagon:                
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  double cornerSize  = getDimensionInPageUnits(2);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,         0, halfHeight             );
                  camCadDatabase.addVertex(poly,-halfWidth, halfHeight - cornerSize);
                  camCadDatabase.addVertex(poly,-halfWidth,-halfHeight + cornerSize);
                  camCadDatabase.addVertex(poly,         0,-halfHeight             );
                  camCadDatabase.addVertex(poly, halfWidth,-halfHeight + cornerSize);
                  camCadDatabase.addVertex(poly, halfWidth, halfHeight - cornerSize);
                  camCadDatabase.addVertex(poly,         0, halfHeight             );
               }

               break;
            case standardSymbolButterfly:                      
               {  
                  double radius  = getDimensionInPageUnits(0)/2.;
                  double bulge90 = tan(degreesToRadians(90) / 4.);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,-radius,      0);
                  camCadDatabase.addVertex(poly,      0,      0);
                  camCadDatabase.addVertex(poly,      0, radius,bulge90);
                  camCadDatabase.addVertex(poly,-radius,      0);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, radius,      0);
                  camCadDatabase.addVertex(poly,      0,      0);
                  camCadDatabase.addVertex(poly,      0,-radius,bulge90);
                  camCadDatabase.addVertex(poly, radius,      0);
               }

               break;
            case standardSymbolSquareButterfly:                
               {  
                  double halfSize = getDimensionInPageUnits(0)/2.;

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,        0,        0);
                  camCadDatabase.addVertex(poly,        0, halfSize);
                  camCadDatabase.addVertex(poly,-halfSize, halfSize);
                  camCadDatabase.addVertex(poly,-halfSize,        0);
                  camCadDatabase.addVertex(poly,        0,        0);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,        0,        0);
                  camCadDatabase.addVertex(poly,        0,-halfSize);
                  camCadDatabase.addVertex(poly, halfSize,-halfSize);
                  camCadDatabase.addVertex(poly, halfSize,        0);
                  camCadDatabase.addVertex(poly,        0,        0);
               }

               break;
            case standardSymbolTriangle:                       
               {  
                  double halfBase   = getDimensionInPageUnits(0)/2.;
                  double halfHeight = getDimensionInPageUnits(1)/2.;

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,        0, halfHeight);
                  camCadDatabase.addVertex(poly,-halfBase,-halfHeight);
                  camCadDatabase.addVertex(poly, halfBase,-halfHeight);
                  camCadDatabase.addVertex(poly,        0, halfHeight);
               }

               break;
            case standardSymbolHalfOval:                       
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, halfWidth - halfHeight,-halfHeight, 1.);
                  camCadDatabase.addVertex(poly, halfWidth - halfHeight, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth             , halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth             ,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth - halfHeight,-halfHeight);
               }

               break;
            case standardSymbolRoundThermalRounded:          
               {  
                  double outerRadius = getDimensionInPageUnits(0)/2.;
                  double innerRadius = getDimensionInPageUnits(1)/2.;
                  double startAngle  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);

                  if (numSpokes < 2) numSpokes = 2;

                  double arcWidth = outerRadius - innerRadius;
                  int widthIndex = camCadDatabase.getDefinedWidthIndex(arcWidth);
                  double radius   = (outerRadius + innerRadius)/2.;
                  double circumference = 2. * Pi * radius;
                  double sectorArcLen = circumference / numSpokes;
                  // sectorArcLen = arcWidth + arcLen + gapArcLen
                  double arcLen = sectorArcLen - arcWidth - spokeWidth;
                  double arcAngleRadians = arcLen / radius;
                  double angleOffsetRadians = ((spokeWidth / 2.) + (arcWidth / 2.)) / radius;
                  double bulge = tan(arcAngleRadians / 4.);
                  double startAngleRadians = degreesToRadians(startAngle);
                  double sectorAngleRadians = 2. * Pi / numSpokes;

                  for (int sectorInd = 0;sectorInd < numSpokes;sectorInd++)
                  {
                     double a0 = startAngleRadians + (sectorInd * sectorAngleRadians) + angleOffsetRadians;
                     double a1 = a0 + arcAngleRadians;
                     double x0 = radius * cos(a0);
                     double y0 = radius * sin(a0);
                     double x1 = radius * cos(a1);
                     double y1 = radius * sin(a1);

                     poly = camCadDatabase.addOpenPoly(polyStruct,widthIndex);

                     camCadDatabase.addVertex(poly,x0,y0,bulge);
                     camCadDatabase.addVertex(poly,x1,y1      );
                  }
               }

               break;
            case standardSymbolRoundThermalSquare:             
               {  
                  double outerRadius = getDimensionInPageUnits(0)/2.;
                  double innerRadius = getDimensionInPageUnits(1)/2.;
                  double startAngleDegrees  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, outerRadius, 0.,1.);
                  camCadDatabase.addVertex(poly,-outerRadius, 0.,1.);
                  camCadDatabase.addVertex(poly, outerRadius, 0.);

                  polyStruct = camCadDatabase.addPolyStruct(complexApertureBlock,
                     floatLayerIndex,dbFlag,negative, graphicClassNormal);
                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, innerRadius, 0.,1.);
                  camCadDatabase.addVertex(poly,-innerRadius, 0.,1.);
                  camCadDatabase.addVertex(poly, innerRadius, 0.);

                  buildSpokes(camCadDatabase,polyStruct,outerRadius,startAngleDegrees,numSpokes,spokeWidth);
               }

               break;
            case standardSymbolSquareThermal:                  
               {  
                  double outerRadius = getDimensionInPageUnits(0)/2.;
                  double innerRadius = getDimensionInPageUnits(1)/2.;
                  double startAngle  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, outerRadius, outerRadius);
                  camCadDatabase.addVertex(poly,-outerRadius, outerRadius);
                  camCadDatabase.addVertex(poly,-outerRadius,-outerRadius);
                  camCadDatabase.addVertex(poly, outerRadius,-outerRadius);
                  camCadDatabase.addVertex(poly, outerRadius, outerRadius);

                  polyStruct = camCadDatabase.addPolyStruct(complexApertureBlock,
                     floatLayerIndex,dbFlag,negative, graphicClassNormal);
                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, innerRadius, innerRadius);
                  camCadDatabase.addVertex(poly,-innerRadius, innerRadius);
                  camCadDatabase.addVertex(poly,-innerRadius,-innerRadius);
                  camCadDatabase.addVertex(poly, innerRadius,-innerRadius);
                  camCadDatabase.addVertex(poly, innerRadius, innerRadius);

                  buildSpokes(camCadDatabase,polyStruct,outerRadius*SqrtOf2,startAngle,numSpokes,spokeWidth);
               }

               break;
            case standardSymbolSquareThermalOpenCorners:  
               {
                  double outerRadius = getDimensionInPageUnits(0)/2.;
                  double innerRadius = getDimensionInPageUnits(1)/2.;
                  double startAngle  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);

                  double airGap = outerRadius - innerRadius;

                  buildThermalOpenCorners(camCadDatabase,polyStruct,outerRadius,outerRadius,airGap,
                     startAngle,numSpokes,spokeWidth);
               }

               break;
            case standardSymbolSquareRoundThermal:             
               {  
                  double outerRadius = getDimensionInPageUnits(0)/2.;
                  double innerRadius = getDimensionInPageUnits(1)/2.;
                  double startAngle  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, outerRadius, outerRadius);
                  camCadDatabase.addVertex(poly,-outerRadius, outerRadius);
                  camCadDatabase.addVertex(poly,-outerRadius,-outerRadius);
                  camCadDatabase.addVertex(poly, outerRadius,-outerRadius);
                  camCadDatabase.addVertex(poly, outerRadius, outerRadius);

                  polyStruct = camCadDatabase.addPolyStruct(complexApertureBlock,
                     floatLayerIndex,dbFlag,negative, graphicClassNormal);
                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, innerRadius, 0, 1.);
                  camCadDatabase.addVertex(poly,-innerRadius, 0, 1.);
                  camCadDatabase.addVertex(poly, innerRadius, 0);

                  buildSpokes(camCadDatabase,polyStruct,outerRadius*SqrtOf2,startAngle,numSpokes,spokeWidth);
               }

               break;
            case standardSymbolRectangularThermal:               // cheating        
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  double startAngle  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);
                  double airGap      = getDimensionInPageUnits(5);

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, halfWidth, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth, halfHeight);

                  polyStruct = camCadDatabase.addPolyStruct(complexApertureBlock,
                     floatLayerIndex,dbFlag,negative, graphicClassNormal);
                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  halfWidth  -= airGap;
                  halfHeight -= airGap;

                  camCadDatabase.addVertex(poly, halfWidth, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth, halfHeight);
                  camCadDatabase.addVertex(poly,-halfWidth,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth,-halfHeight);
                  camCadDatabase.addVertex(poly, halfWidth, halfHeight);

                  buildSpokes(camCadDatabase,polyStruct,max(halfWidth,halfHeight)*SqrtOf2,startAngle,numSpokes,spokeWidth);
               }

               break;
            case standardSymbolRectangularThermalOpenCorners:
               {
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  double startAngle  = getDimension(2);
                  int numSpokes      = DcaRound(m_dimension[3]);
                  double spokeWidth  = getDimensionInPageUnits(4);
                  double airGap      = getDimensionInPageUnits(5);

                  buildThermalOpenCorners(camCadDatabase,polyStruct,halfWidth,halfHeight,airGap,
                     startAngle,numSpokes,spokeWidth);
               }

               break;
            case standardSymbolEllipse:    
               {  
                  double halfWidth   = getDimensionInPageUnits(0)/2.;
                  double halfHeight  = getDimensionInPageUnits(1)/2.;
                  const int numPoints = 36;

                  poly = camCadDatabase.addFilledPoly(polyStruct,0);

                  for (int angleInd = 0;angleInd <= numPoints;angleInd++)
                  {
                     double angle = angleInd * 2. * Pi / numPoints;
                     double x = halfWidth  * cos(angle);
                     double y = halfHeight * sin(angle);

                     camCadDatabase.addVertex(poly,x,y);
                  }
               }

               break;
            case standardSymbolMoire:                          
               {  
                  double ringWidth      = getDimensionInPageUnits(0);
                  double ringGap        = getDimensionInPageUnits(1);
                  int numRings          = DcaRound(m_dimension[2]);
                  double lineWidth      = getDimensionInPageUnits(3);
                  double halfLineLength = getDimensionInPageUnits(4)/2.;
                  double lineAngle      = getDimension(5);

                  int ringWidthIndex = camCadDatabase.getDefinedWidthIndex(ringWidth);
                  double ringRadius  = ringWidth + ringGap;

                  for (int ringInd = 0;ringInd < numRings;ringInd++)
                  {
                     poly = camCadDatabase.addOpenPoly(polyStruct,ringWidthIndex);
                     double radius = (ringInd + 1) * ringRadius;

                     camCadDatabase.addVertex(poly, radius, 0, 1.);
                     camCadDatabase.addVertex(poly,-radius, 0, 1.);
                     camCadDatabase.addVertex(poly, radius, 0);
                  }

                  int lineWidthIndex = camCadDatabase.getDefinedWidthIndex(lineWidth);

                  for (int lineInd = 0;lineInd < 2;lineInd++)
                  {
                     double angle = degreesToRadians(lineAngle + lineInd * 90);
                     double x = halfLineLength * cos(angle);
                     double y = halfLineLength * sin(angle);

                     poly = camCadDatabase.addOpenPoly(polyStruct,lineWidthIndex);

                     camCadDatabase.addVertex(poly,-x,-y);
                     camCadDatabase.addVertex(poly, x, y);
                  }
               }

               break;
            case standardSymbolInvalid:                        
            case standardSymbolUndefined:  
               {  
                  double diameter = .02;
                  double radius   = diameter / 2.;
                  double unit     = radius   / 10.;

                  poly = camCadDatabase.addClosedPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,     0., radius,1.);
                  camCadDatabase.addVertex(poly,     0.,-radius,1.);
                  camCadDatabase.addVertex(poly,     0., radius);

                  poly = camCadDatabase.addClosedPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,-4.*unit, 6.*unit,1.);
                  camCadDatabase.addVertex(poly,-4.*unit, 2.*unit,1.);
                  camCadDatabase.addVertex(poly,-4.*unit, 6.*unit);

                  poly = camCadDatabase.addClosedPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly, 4.*unit, 6.*unit,1.);
                  camCadDatabase.addVertex(poly, 4.*unit, 2.*unit,1.);
                  camCadDatabase.addVertex(poly, 4.*unit, 6.*unit);

                  poly = camCadDatabase.addOpenPoly(polyStruct,0);

                  camCadDatabase.addVertex(poly,-6.*unit,-4.*unit,.5);
                  camCadDatabase.addVertex(poly, 6.*unit,-4.*unit);
               }

               break;
            }
         }
      }  // if (getApertureShape() == apertureComplex)
      else
      {
         m_apertureBlock = camCadDatabase.getDefinedAperture(apertureName,
            getApertureShape(),getDimensionInPageUnits(0),getDimensionInPageUnits(1),0.,0.,degreesToRadians(m_rotation));
      }
   }  // if (m_apertureBlock == NULL)

   return m_apertureBlock;
}

void COdbFeatureSymbol::buildCamCadData(COdbPpDatabase& odbPpDatabase,BlockStruct* apertureBlock)
{
}

int COdbFeatureSymbol::getWidthIndex(COdbPpDatabase& odbPpDatabase)
{
   double width = 0.;

   if (m_type == standardSymbolCircle || m_type == standardSymbolSquare)
   {
      width = getDimensionInPageUnits(0);  
   }

   int widthIndex = odbPpDatabase.getCamCadDatabase().getDefinedWidthIndex(width);

   return widthIndex;
}

void COdbFeatureSymbol::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbFeatureSymbol: ");
   writeFormat.writef(
"m_name='%s', m_type='%s', m_dimension[0:5]=(%.3f, %.3f, %.3f, %.3f, %.3f, %.3f)\n",
(const char*)m_name,(const char*)standardSymbolTypeTagToString(m_type),
m_dimension[0],m_dimension[1],m_dimension[2],m_dimension[3],m_dimension[4],m_dimension[5]);

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbFeatureSymbol* COdbFeatureSymbolArray::getAt(int index)
{
   COdbFeatureSymbol* symbol = NULL;

   if (index >= 0 && index < GetSize())
   {
      symbol = GetAt(index);
   }

   return symbol;
}

COdbFeatureSymbol* COdbFeatureSymbolArray::getAt(int index,CWriteFormat& log,
   const CString& line,const CString& filePath,int lineNumber)
{
   COdbFeatureSymbol* symbol = getAt(index);

   if (symbol == NULL)
   {
log.writef(PrefixError,
"Could not find feature symbol for serialNumber=%d .  Record='%s' in '%s'.%d\n",
index,(const char*)line,(const char*)filePath,lineNumber);
   }

   return symbol;
}

//_____________________________________________________________________________
COdbFeatureSymbol* COdbFeatureSymbolMap::getDefinedAt(const CString& name)
{
   COdbFeatureSymbol* symbol = getAt(name);

   if (symbol == NULL)
   {
      symbol = new COdbFeatureSymbol(name);
      setAt(name,symbol);
   }

   return symbol;
}

void COdbFeatureSymbolMap::buildCamCadData(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   for (int ind=0;ind < getCount();ind++)
   {
      COdbFeatureSymbol* featureSymbol = getAt(ind);

      if (featureSymbol != NULL)
      {
         log.writef(PrefixStatus,"Aperture: %s\n",(const char*)featureSymbol->getName());

         BlockStruct* block = featureSymbol->getDefinedAperture(odbPpDatabase,log);
      }
   }
}

//_____________________________________________________________________________
CAttributeMapEntry::CAttributeMapEntry(const CString& attribute,const CString& mapAttribute)
{
   m_attribute    = attribute;
   m_mapAttribute = mapAttribute;
}

void CAttributeMapEntry::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("CAttributeMapEntry: ");
   writeFormat.writef("m_attribute='%s', m_mapAttribute='%s'\n",
      (const char*)m_attribute,(const char*)m_mapAttribute);

   writeFormat.popHeader();
}

//_____________________________________________________________________________
CAttributeMapEntryArray::CAttributeMapEntryArray() :
   CTypedObArrayWithMapContainer<CAttributeMapEntry>(500,true)
{
}

CAttributeMapEntryArray::CAttributeMapEntryArray(CAttributeMapEntryArray& other)
{
   *this = other;
}

CAttributeMapEntryArray::~CAttributeMapEntryArray()
{
   empty();
}

CAttributeMapEntryArray& CAttributeMapEntryArray::operator=(CAttributeMapEntryArray& other)
{
   if (&other != this)
   {
      empty();

      for (int ind=0;ind < other.getSize();ind++)
      {
         CAttributeMapEntry* attributeMap = other.getAt(ind);
         add(attributeMap->getAttributeName(),attributeMap->getMappedAttributeName());
      }
   }

   return *this;
}

CAttributeMapEntry* CAttributeMapEntryArray::add(const CString& attribute,const CString& mapAttribute)
{
   CAttributeMapEntry* attributeMap = new CAttributeMapEntry(attribute,mapAttribute);

   CTypedObArrayWithMapContainer<CAttributeMapEntry>::add(attribute,attributeMap);

   return attributeMap;
}

void CAttributeMapEntryArray::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("CAttributeMapEntryArray: ");
   writeFormat.writef("getSize()=%d\n",getSize());

   if (depth != 0)
   {
      for (int ind=0;ind < getSize();ind++)
      {
         CAttributeMapEntry* attributeMapEntry = getAt(ind);

         if (attributeMapEntry != NULL)
         {
            attributeMapEntry->dump(writeFormat,depth);
         }
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
CAttDef::CAttDef(AttributeDefTypeTag type,const CString& name)
{
   m_type = type;
   m_name = name;
}

//_____________________________________________________________________________
CAttDefArray::CAttDefArray() :
   CTypedObArrayWithMapContainer<CAttDef>(100,true)
{
}

CAttDefArray::CAttDefArray(CAttDefArray& other)
{
   *this = other;
}

CAttDefArray::~CAttDefArray()
{
}

CAttDefArray& CAttDefArray::operator=(CAttDefArray& other)
{
   if (&other != this)
   {
      empty();

      for (int ind=0;ind < other.getSize();ind++)
      {
         CAttDef* attDef = other.getAt(ind);
         add(attDef->getType(),attDef->getName());
      }
   }

   return *this;
}

CAttDef* CAttDefArray::add(AttributeDefTypeTag type,const CString& name)
{
   CAttDef* attDef = new CAttDef(type,name);

   CTypedObArrayWithMapContainer<CAttDef>::add(name,attDef);

   return attDef;
}

//_____________________________________________________________________________
COdbOptionTypeValue::COdbOptionTypeValue(const CString& optionTypeName)
{
   m_optionTypeName = optionTypeName;
}

COdbOptionTypeValue::COdbOptionTypeValue(const COdbOptionTypeValue& other)
{
   *this = other;
}

COdbOptionTypeValue& COdbOptionTypeValue::operator=(const COdbOptionTypeValue& other)
{
   if (&other != this)
   {
      m_optionTypeName = other.m_optionTypeName;

      m_optionValues.RemoveAll();
      m_optionValues.SetSize(other.m_optionValues.GetSize(),5);

      for (int ind = 0;ind <  other.m_optionValues.GetSize();ind++)
      {
         m_optionValues.SetAtGrow(ind,other.m_optionValues.GetAt(ind));
      }
   }

   return *this;
}

void COdbOptionTypeValue::addValue(int optionId,const CString& optionValue)
{
   m_optionValues.SetAtGrow(optionId,optionValue);
}

CString COdbOptionTypeValue::getValue(int optionId)
{
   CString retval;

   if (optionId >= 0 && optionId < m_optionValues.GetSize())
   {
      retval = m_optionValues.GetAt(optionId);
   }

   if (retval.GetLength() == 0)
   {
      retval.Format("%d",optionId);
   }

   return retval;
}

//_____________________________________________________________________________
COdbOptionTypeValues::COdbOptionTypeValues()
{
}

COdbOptionTypeValues::COdbOptionTypeValues(const COdbOptionTypeValues& other)
{
   *this = other;
}

COdbOptionTypeValues& COdbOptionTypeValues::operator=(const COdbOptionTypeValues& other)
{
   if (&other != this)
   {
      empty();

      for (int ind = 0;ind <  other.m_optionTypes.getCount();ind++)
      {
         COdbOptionTypeValue* optionTypeValue = other.m_optionTypes.getAt(ind);
         COdbOptionTypeValue* newOptionTypeValue = new COdbOptionTypeValue(*optionTypeValue);

         m_optionTypes.setAt(newOptionTypeValue->getOptionTypeKey(),newOptionTypeValue);
      }
   }

   return *this;
}

bool COdbOptionTypeValues::contains(CString optionTypeName)
{
   COdbOptionTypeValue* optionTypeValue;  
   optionTypeName.MakeLower();

   bool retval = m_optionTypes.lookup(optionTypeName,optionTypeValue);

   return retval;
}

void COdbOptionTypeValues::addValue(const CString& optionTypeName,int optionId,const CString& optionValue)
{
   COdbOptionTypeValue* optionTypeValue = NULL;
   CString optionTypeKey(optionTypeName);
   optionTypeKey.MakeLower();

   m_optionTypes.lookup(optionTypeKey,optionTypeValue);

   if (optionTypeValue == NULL)
   {
      optionTypeValue = new COdbOptionTypeValue(optionTypeName);
      m_optionTypes.setAt(optionTypeKey,optionTypeValue);
   }

   optionTypeValue->addValue(optionId,optionValue);
}

CString COdbOptionTypeValues::getValue(const CString& optionTypeName,int optionId)
{
   CString retval;
   COdbOptionTypeValue* optionAttribute;
   CString optionNameKey(optionTypeName);
   optionNameKey.MakeLower();

   if (m_optionTypes.lookup(optionNameKey,optionAttribute))
   {
      retval = optionAttribute->getValue(optionId);
   }
   else
   {
      retval.Format("%d",optionId);
   }

   return retval;
}

//_____________________________________________________________________________
COdbFeatureAttribute::COdbFeatureAttribute(const CString& name,const CString& param,
      ValueTypeTag valueType,const CString& stringValue)
{
   m_name        = name;
   m_param       = param;
   m_stringValue = stringValue;
   m_valueType   = valueType;
}

COdbFeatureAttribute::COdbFeatureAttribute(const COdbFeatureAttribute& other)
{
   m_param       = other.m_param;
   m_name        = other.m_name;
   m_stringValue = other.m_stringValue;
   m_valueType   = other.m_valueType;
}

COdbFeatureAttribute& COdbFeatureAttribute::operator=(const COdbFeatureAttribute& other)
{
   if (&other != this)
   {
      m_param       = other.m_param;
      m_name        = other.m_name;
      m_stringValue = other.m_stringValue;
      m_valueType   = other.m_valueType;
   }

   return *this;
}

void COdbFeatureAttribute::addAttribute(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase,CWriteFormat& log)
{
   camCadDatabase.addAttribute(attributeMap,m_name,m_valueType,m_stringValue,log);
}

//_____________________________________________________________________________
COdbFeatureAttributeArray::COdbFeatureAttributeArray(const COdbFeatureAttributeArray& other) :
   CTypedObArrayContainer<COdbFeatureAttribute*>(other.getGrowBySize())
{
   for (int ind = 0;ind < GetSize();ind++)
   {
      COdbFeatureAttribute* otherAttribute = other.GetAt(ind);
      COdbFeatureAttribute* attribute = NULL;

      if (otherAttribute != NULL)
      {
         attribute = new COdbFeatureAttribute(*otherAttribute);
      }

      SetAtGrow(ind,attribute);     
   }
}

void COdbFeatureAttributeArray::addAttribute(const CString& name,
   ValueTypeTag valueType,const CString& stringValue)
{
   COdbFeatureAttribute* attribute = new COdbFeatureAttribute(name,"",valueType,stringValue);
   Add(attribute);
}

void COdbFeatureAttributeArray::takeAttributes(CStringArray* values,const CString& namePrefix)
{
   if (values != NULL)
   {
      for (int ind = 0;ind < values->GetSize();ind++)
      {
         CString value = values->GetAt(ind);

         if (value.GetLength() > 0)
         {
            CString indexString;
            indexString.Format("%d", ind);
            CString attributeName = namePrefix + indexString;

            addAttribute(attributeName,valueTypeString,value);
         }
      }

      values->RemoveAll();
   }
}

void COdbFeatureAttributeArray::placeAttributes(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase,CWriteFormat& log)
{
   condense();

   for (int ind=0;ind < GetSize();ind++)
   {
      COdbFeatureAttribute* featureAttribute = GetAt(ind);

      featureAttribute->addAttribute(attributeMap,camCadDatabase,log);
   }
}

void COdbFeatureAttributeArray::mergeFrom(const COdbFeatureAttributeArray& other)
{
   for (int ind=0;ind < other.GetSize();ind++)
   {
      COdbFeatureAttribute* featureAttribute = other.GetAt(ind);
      COdbFeatureAttribute* newFeatureAttribute = new COdbFeatureAttribute(*featureAttribute);

      Add(newFeatureAttribute);
   }
}

void COdbFeatureAttributeArray::condense()
{
   CTypedPtrMap<CMapStringToOb,CString,COdbFeatureAttribute*> map;
   map.InitHashTable(nextPrime2n((int)(GetSize() * 1.2)));
   int dstInd = 0;
   COdbFeatureAttribute* existingAttribute;

   for (int srcInd = 0;srcInd < GetSize();srcInd++)
   {
      COdbFeatureAttribute* attribute = GetAt(srcInd);

      if (map.Lookup(attribute->getName(),existingAttribute))
      {
         if (attribute->getName() == "TEST")  // special case for TEST, not sure why, it was just here already
         {
            if (attribute->getStringValue().CompareNoCase(existingAttribute->getStringValue()) != 0)
            {
               existingAttribute->setStringValue("");
            }
         }
         else
         {
            // Update to most recent value
            existingAttribute->setStringValue( attribute->getStringValue() );
         }

         delete attribute;
      }
      else
      {
         SetAt(dstInd++,attribute);
         map.SetAt(attribute->getName(),attribute);
      }
   }

   SetSize(dstInd);
}

//_____________________________________________________________________________
void COdbFeatureAttributeArrayWithMap::setAllUndefined()
{
   for (int ind=0;ind < getSize();ind++)
   {
      COdbFeatureAttribute* featureAttribute = getAt(ind);
      featureAttribute->setValueType(valueTypeUndefined);
   }
}

//_____________________________________________________________________________
COdbFeatureAttributes::COdbFeatureAttributes()
{
   initNamesAndStrings();

   m_layer = NULL;
}

COdbFeatureAttributes::COdbFeatureAttributes(COdbFeatureAttributes& other) :
   m_attribMapArray(other.m_attribMapArray),
   m_attDefArray(other.m_attDefArray),
   m_optionTypeValues(other.m_optionTypeValues)
{
   m_attributeNames.SetSize(other.m_attributeNames.GetSize(),1000);

   for (int ind=0;ind < other.m_attributeNames.GetSize();ind++)
   {
      m_attributeNames.SetAt(ind,other.m_attributeNames.GetAt(ind));
   }

   m_attributeTextStrings.SetSize(other.m_attributeTextStrings.GetSize(),1000);

   for (int ind=0;ind < other.m_attributeTextStrings.GetSize();ind++)
   {
      m_attributeTextStrings.SetAt(ind,other.m_attributeTextStrings.GetAt(ind));
   }
}

COdbFeatureAttributes::~COdbFeatureAttributes()
{
   empty();
}

void COdbFeatureAttributes::initializeSettings(COdbFeatureAttributes& other)
{
   m_attribMapArray     = other.m_attribMapArray;
   m_attDefArray        = other.m_attDefArray;
   m_optionTypeValues   = other.m_optionTypeValues;

   initNamesAndStrings();
}

void COdbFeatureAttributes::empty()
{
   m_attribMapArray.empty();
   m_attDefArray.empty();
   m_optionTypeValues.empty();
   m_attributeCache.empty();
}

void COdbFeatureAttributes::initNamesAndStrings()
{
   m_attributeNames.SetSize(0,1000);
   m_attributeTextStrings.SetSize(0,1000);
}

bool COdbFeatureAttributes::addAttributes(COdbFeatureAttributeArray& featureAttributeArray,
   CFileReader& odbParser,int attributeParamIndex,
   const CString& filePath,const CString& recordString,int lineNumber,
   CWriteFormat& log)
{
   bool retval = true;

   for (int index = attributeParamIndex;index < odbParser.getNumParams();index++)
   {
      COdbFeatureAttribute* featureAttribute = NULL;
         
      CString theParam( odbParser.getParam(index) );

      if (! m_attributeCache.lookup(theParam, featureAttribute))
      {
         featureAttribute = new COdbFeatureAttribute("", theParam);

         retval = getAttributeData(*featureAttribute,filePath,recordString,lineNumber,log) && retval;
         m_attributeCache.add(odbParser.getParam(index), featureAttribute);
      }

      if (featureAttribute->getValueType() != valueTypeUndefined)
      {
         if (featureAttributeArray.isContainer())
         {
            featureAttributeArray.Add(new COdbFeatureAttribute(*featureAttribute));
         }
         else
         {
            featureAttributeArray.Add(featureAttribute);
         }
      }
   }

   return retval;
}

bool COdbFeatureAttributes::addAttributes(COdbFeature& feature,
   CFileReader& odbParser,int attributeParamIndex,
   const CString& filePath,const CString& recordString,int lineNumber,
   CWriteFormat& log)
{
   bool retval = true;

   for (int index = attributeParamIndex;index < odbParser.getNumParams();index++)
   {
      COdbFeatureAttribute* featureAttribute = NULL;
         
      if (! m_attributeCache.lookup(odbParser.getParam(index),featureAttribute))
      {
         featureAttribute = new COdbFeatureAttribute("",odbParser.getParam(index));

         retval = getAttributeData(*featureAttribute,filePath,recordString,lineNumber,log) && retval;
         m_attributeCache.add(odbParser.getParam(index),featureAttribute);
      }

      if (featureAttribute->getValueType() != valueTypeUndefined)
      {
         feature.addAttribute(new COdbFeatureAttribute(*featureAttribute));
      }
   }

   return retval;
}

CString COdbFeatureAttributes::getMappedAttributeName(const CString& rawAttributeName)
{
   CString attributeNameKey(rawAttributeName);
   attributeNameKey.MakeUpper();

   CAttributeMapEntry* mapEntry = NULL;

   m_attribMapArray.lookup(attributeNameKey,mapEntry);
   CString mappedAttributeName = (mapEntry == NULL ? rawAttributeName : mapEntry->getMappedAttributeName());

   return mappedAttributeName;
}

bool COdbFeatureAttributes::getMappedAttributeName(const CString& rawAttributeName, CString &mappedAttributeName)
{
   CString attributeNameKey(rawAttributeName);
   attributeNameKey.MakeUpper();

   CAttributeMapEntry* mapEntry = NULL;

   m_attribMapArray.lookup(attributeNameKey,mapEntry);
   mappedAttributeName = (mapEntry == NULL ? rawAttributeName : mapEntry->getMappedAttributeName());

   return (mapEntry != NULL);  // true if mapped, false if not mapped.
}

bool COdbFeatureAttributes::fillAttributes(COdbFeatureAttributeArrayWithMap& attributeArray,
   CCamCadDatabase& camCadDatabase,
   const CStringArray& params,int attributeParamIndex,
   const CString& filePath,const CString& recordString,int lineNumber,
   CWriteFormat& log)
{
   bool retval = true;
   COdbFeatureAttribute* targetFeatureAttribute;

   for (int index = attributeParamIndex;index < params.GetSize();index++)
   {
      COdbFeatureAttribute featureAttribute("",params[index]);

      if (getAttributeData(featureAttribute,filePath,recordString,lineNumber,log))
      {
         if (attributeArray.lookup(featureAttribute.getName(),targetFeatureAttribute))
         {
            if (targetFeatureAttribute->getValueType() == valueTypeUndefined)
            {
               *targetFeatureAttribute = featureAttribute;
            }
         }
      }
   }

   return retval;
}

bool COdbFeatureAttributes::getAttributes(COdbFeatureAttributeArray& attributeArray,
   CCamCadDatabase& camCadDatabase,
   const CStringArray& params,int attributeParamIndex,
   const CString& filePath,const CString& recordString,int lineNumber,
   CWriteFormat& log)
{
   bool retval = true;
   COdbFeatureAttribute* featureAttribute = NULL;

   for (int index = attributeParamIndex;index < params.GetSize();index++)
   {
      if (featureAttribute == NULL)
      {
         featureAttribute = new COdbFeatureAttribute();
      }

      if (getAttributeData(*featureAttribute,filePath,recordString,lineNumber,log))
      {
         attributeArray.Add(featureAttribute);
         featureAttribute = NULL;
      }
   }

   delete featureAttribute;

   return retval;
}

bool COdbFeatureAttributes::addAttribute(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase,
   const CString& param,
   const CString& filePath,const CString& recordString,int lineNumber,
   CWriteFormat& log)
{
   COdbFeatureAttribute featureAttribute("",param);

   bool retval = getAttributeData(featureAttribute,filePath,recordString,lineNumber,log);

   if (retval && featureAttribute.getValueType() != valueTypeUndefined)
   {
      camCadDatabase.addAttribute(attributeMap,featureAttribute.getName(),
         featureAttribute.getValueType(),featureAttribute.getStringValue(),log);
   }

   return retval;
}

bool COdbFeatureAttributes::getAttributeData(COdbFeatureAttribute& featureAttribute,
   const CString& filePath,const CString& recordString,int lineNumber,
   CWriteFormat& log)
{
   bool retval = true;
   char* endPtr;
   int intValue;
   double doubleValue;
   CString stringValue;

   int attributeNameSerialNumber = strtol(featureAttribute.getParam(),&endPtr,10);
   bool valuePresentFlag  = (*endPtr == '=');
   featureAttribute.setValueType(valueTypeUndefined);

   if (valuePresentFlag)
   {
      stringValue = (endPtr + 1);
      featureAttribute.setStringValue(stringValue);
      doubleValue = atof(endPtr + 1);
      intValue    = (int)doubleValue;
   }

   if (attributeNameSerialNumber >= 0 && attributeNameSerialNumber < m_attributeNames.GetSize())
   {
      CString attributeName = m_attributeNames[attributeNameSerialNumber];
      CString attributeNameKey = attributeName;
      attributeNameKey.MakeUpper();
      CString optionTypeStringValue;

      if (valuePresentFlag)
      {
         if (m_optionTypeValues.contains(attributeName))
         {
            optionTypeStringValue = m_optionTypeValues.getValue(attributeName,intValue);
         }
      }

      if (attributeName.GetLength() > 1 && attributeName.GetAt(0) == '.')
      {
         if (attributeName.CompareNoCase(".pad_usage") == 0)
         {
            if (valuePresentFlag)
            {
               OdbPadUsageTag padUsage = intToOdbPadUsageTag(intValue);

               if (padUsage != padUsageUndefined)
               {
                  if (optionTypeStringValue.IsEmpty())
                  {
                     featureAttribute.setValueType(valueTypeInteger);
                     featureAttribute.getStringValue().Format("%d",(int)padUsage);
                  }
                  else
                  {
                     featureAttribute.setValueType(valueTypeString);
                     featureAttribute.setStringValue(optionTypeStringValue);
                  }
               }
               else
               {
                  log.writef(PrefixError,
                     "Bad attribute value for '.pad_usage' attribute.  Record='%s' in '%s'.%d\n",
                     (const char*)recordString,(const char*)filePath,lineNumber);
               }
            }
            else
            {
               log.writef(PrefixError,
                  "Missing attribute value for '.pad_usage' attribute.  Record='%s' in '%s'.%d\n",
                  (const char*)recordString,(const char*)filePath,lineNumber);
            }
         }
         else if (attributeName.CompareNoCase(".test_point") == 0)
         {
            attributeName = "TEST";
            featureAttribute.setValueType(valueTypeString);
            featureAttribute.setStringValue("");

            if (m_layer != NULL)
            {
               LayerStruct* layerStruct = m_layer->getCCZLayerStruct();

               if (layerStruct != NULL)
               {
                  if (layerStruct->getLayerType() == LAYTYPE_SIGNAL_TOP)
                  {
                     featureAttribute.setStringValue("TOP");
                  }
                  else if (layerStruct->getLayerType() == LAYTYPE_SIGNAL_BOT)
                  {
                     featureAttribute.setStringValue("BOTTOM");
                  }
               }
            }
         }
         else if (attributeName.CompareNoCase(".vpl1_pkg_name") == 0)
         {
            // This is just a string attrib and could have been handled in the standard
            // string attrib handling (in the else below) but that requires that it get
            // put into the odb.in string attribs list. Since our vpl connection depends on
            // this attrib, we handle it directly, as string attrib, whether in odb.in or not.

            if (!valuePresentFlag)
            {
               featureAttribute.setValueType(valueTypeString);
               featureAttribute.setStringValue("");
            }
            else if (intValue >= 0 && intValue < m_attributeTextStrings.GetSize())
            {
               featureAttribute.setValueType(valueTypeString);
               featureAttribute.setStringValue(m_attributeTextStrings.GetAt(intValue));
            }
            else
            {
               log.writef(PrefixError,
                  "Illegal attribute value string serial number in '%s'; Record='%s' in '%s'.%d\n",
                  (const char*)featureAttribute.getParam(),(const char*)recordString,(const char*)filePath,lineNumber);
            }
         }
         else
         {
            CAttDef* attDef;

            if (m_attDefArray.lookup(attributeNameKey,attDef))
            {
               if (attDef->getType() == typeString)
               {
                  if (!valuePresentFlag)
                  {
                     featureAttribute.setValueType(valueTypeString);
                     featureAttribute.setStringValue("");
                  }
                  else if (intValue >= 0 && intValue < m_attributeTextStrings.GetSize())
                  {
                     featureAttribute.setValueType(valueTypeString);
                     featureAttribute.setStringValue(m_attributeTextStrings.GetAt(intValue));
                  }
                  else
                  {
                     log.writef(PrefixError,
                        "Illegal attribute value string serial number in '%s'; Record='%s' in '%s'.%d\n",
                        (const char*)featureAttribute.getParam(),(const char*)recordString,(const char*)filePath,lineNumber);
                  }
               }
               else
               {
                  switch (attDef->getType())
                  {
                  case typeLayer:    featureAttribute.setValueType(valueTypeInteger);     break;
                  case typeInt:      featureAttribute.setValueType(valueTypeInteger);     break;
                  case typeDouble:   featureAttribute.setValueType(valueTypeDouble);      break;
                  case typePInch:    featureAttribute.setValueType(valueTypeUnitDouble);  break;
                  case typeString:   featureAttribute.setValueType(valueTypeString);      break;
                  case typeBoolean:  featureAttribute.setValueType(valueTypeNone);        break;
                  case typePMil:     
                     featureAttribute.setValueType(valueTypeUnitDouble);
                     featureAttribute.getStringValue().Format("%f",1000.*doubleValue);
                     break;
                  }
               }
            }
            else if (m_optionTypeValues.contains(attributeName))
            {
               if (valuePresentFlag)
               {
                  featureAttribute.setValueType(valueTypeString);
                  CString stringValue = m_optionTypeValues.getValue(attributeName,intValue);
                  featureAttribute.setStringValue(stringValue);
               }
               else
               {
                  log.writef(PrefixError,
                     "Missing attribute value for '%s' attribute.  Record='%s' in '%s'.%d\n",
                     (const char*)attributeName,(const char*)recordString,(const char*)filePath,lineNumber);
               }
            }
            else
            {
               uniqueErrorFilter.writef(log,PrefixWarning,filePath,lineNumber,recordString,
                  "Attribute name '%s' not defined in Odb.in",
                  (const char*)attributeName);
            }
         }
      }

      if (featureAttribute.getValueType() != valueTypeUndefined)
      {
         CAttributeMapEntry* attributeMapEntry;

         if (m_attribMapArray.lookup(attributeNameKey,attributeMapEntry))
         {
            attributeName = attributeMapEntry->getMappedAttributeName();
         }

         featureAttribute.setName(attributeName);
      }
      else
      {
         retval = false;
      }
   }
   else
   {
      retval = false;

      log.writef(PrefixError,
         "Illegal attribute name serial number in '%s'; Record='%s' in '%s'.%d\n",
         (const char*)featureAttribute.getParam(),(const char*)recordString,(const char*)filePath,lineNumber);
   }

   return retval;
}

//_____________________________________________________________________________
COdbPolygonSegment::COdbPolygonSegment(double xEnd,double yEnd)
{ 
   m_endPoint.x = xEnd;
   m_endPoint.y = yEnd;
}

void COdbPolygonSegment::mirrorPolygon()
{
   m_endPoint.x = -m_endPoint.x;
}

void COdbPolygonSegment::buildCamCadData(COdbPpDatabase& odbPpDatabase,CPoly* poly,CPnt*& lastVertex, CPoint2d &lastOdbVertex)
{
   lastVertex = odbPpDatabase.getCamCadDatabase().addVertex(poly,m_endPoint.x,m_endPoint.y);

   lastOdbVertex.x = m_endPoint.x; // Use this, it's a double, lastVertex has DbUnit which is float as of this writing,
   lastOdbVertex.y = m_endPoint.y; // and we need double accuracy for this.
}

//_____________________________________________________________________________
COdbPolygonCurve::COdbPolygonCurve(double xEnd,double yEnd,double xCenter,double yCenter,bool clockwise) :
   COdbPolygonSegment(xEnd,yEnd)
{ 
   m_center.x  = xCenter;
   m_center.y  = yCenter;
   m_clockwise = clockwise;
}

void COdbPolygonCurve::mirrorPolygon()
{
   m_endPoint.x = -m_endPoint.x;
   m_center.x = - m_center.x;
   m_clockwise = !m_clockwise;
}

void COdbPolygonCurve::buildCamCadData(COdbPpDatabase& odbPpDatabase,CPoly* poly,CPnt*& lastVertex, CPoint2d &lastOdbVertex)
{
   double x = m_endPoint.x;
   double y = m_endPoint.y;

   // CPnt is using float (aka DbUnit) and we are losing accuracy because of it.
   // We can't test to 1E-9 accuracy because floats don't even have enough accuracy.
   // DR 640949 showed that 1E-9 is not working.
   // Apparantly there was some need for more than 1E-6, though, or it would never have been set to 1E-9.
   // The CPnt structure used DbUnit which is currently float.
   // It is day before release, don't want to make a system-wide change to DbUnit today.
   // So extra arg was added to track last vertex as double, and this seems to have cleared up the problem.
   // NEED TO UPDATE DBUNIT TO DOUBLE !!!

   if (!fpeq(m_endPoint.x,lastOdbVertex.x,EXT_SMALL_NUMBER) || !fpeq(m_endPoint.y,lastOdbVertex.y,EXT_SMALL_NUMBER))
   {
      double radius,startAngle,deltaAngle;
      ArcCenter2(lastOdbVertex.x,lastOdbVertex.y,m_endPoint.x,m_endPoint.y,m_center.x,m_center.y,
         &radius,&startAngle,&deltaAngle,m_clockwise);

      double bulge = tan(deltaAngle / 4.);

      if (fabs(bulge) > 572.)
      {
         bulge = 0.;
      }

      lastVertex->bulge = (DbUnit)bulge;
   } // xs ys different from xe ye
   else
   {
      // arc as part of an outline contains same start and end koos!
      // exception full circle
      //CT
      //OB -0.216535433071 0 I
      //OC -0.216535433071 0 0 0 Y
      //OE
      //CE
      //
      double radius;
      CircleCenterPt(m_center.x,m_center.y,lastOdbVertex.x,lastOdbVertex.y,&radius);
      lastVertex->x = (DbUnit)(m_center.x - radius);
      lastVertex->y = (DbUnit)m_center.y;
      lastVertex->bulge = 1.;
      lastVertex = odbPpDatabase.getCamCadDatabase().addVertex(poly,m_center.x + radius,m_center.y,1.0);
      x = m_center.x - radius;
      y = m_center.y;
   }

   lastVertex = odbPpDatabase.getCamCadDatabase().addVertex(poly,x,y);  // only get floats (DbUnit) in lastVertex
   lastOdbVertex.x = x;  // This is because we need double accuracy, DR 640949, need to convert DbUnit to double !!!
   lastOdbVertex.y = y;
}

//_____________________________________________________________________________
COdbOutline::~COdbOutline()
{ 
}

//_____________________________________________________________________________
COdbCircleOutline::COdbCircleOutline(double xCenter,double yCenter,double radius)
{ 
   m_xCenter = xCenter;
   m_yCenter = yCenter;
   m_radius  = radius;
}

void COdbCircleOutline::mirrorOutline()
{
   m_xCenter = -m_xCenter;
}

void COdbCircleOutline::buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock)
{
   DataStruct* polyStruct = odbPpDatabase.getCamCadDatabase().addPolyStruct(parentBlock,layerIndex,0,
      false, this->getGraphicClass());

   CPoly* poly = odbPpDatabase.getCamCadDatabase().addClosedPoly(polyStruct,0);

   fillOutlinePoly(odbPpDatabase, *poly);
}

void COdbCircleOutline::fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly)
{
   poly.addVertex(m_xCenter - m_radius, m_yCenter, 1.);
   poly.addVertex(m_xCenter + m_radius, m_yCenter, 1.);
   poly.addVertex(m_xCenter - m_radius, m_yCenter);
}

//_____________________________________________________________________________
COdbSquareOutline::COdbSquareOutline(double xCenter,double yCenter,double halfSide)
{ 
   m_xCenter  = xCenter;
   m_yCenter  = yCenter;
   m_halfSide = halfSide;
}

void COdbSquareOutline::mirrorOutline()
{
   m_xCenter = -m_xCenter;
}

void COdbSquareOutline::buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock)
{
   DataStruct* polyStruct = odbPpDatabase.getCamCadDatabase().addPolyStruct(parentBlock,layerIndex,0,
      false, this->getGraphicClass());

   CPoly* poly = odbPpDatabase.getCamCadDatabase().addClosedPoly(polyStruct,0);

   fillOutlinePoly(odbPpDatabase, *poly);
}

void COdbSquareOutline::fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly)
{
   poly.addVertex(m_xCenter - m_halfSide,  m_yCenter - m_halfSide);
   poly.addVertex(m_xCenter + m_halfSide,  m_yCenter - m_halfSide);
   poly.addVertex(m_xCenter + m_halfSide,  m_yCenter + m_halfSide);
   poly.addVertex(m_xCenter - m_halfSide,  m_yCenter + m_halfSide);
   poly.addVertex(m_xCenter - m_halfSide,  m_yCenter - m_halfSide);
}

//_____________________________________________________________________________
COdbRectangleOutline::COdbRectangleOutline(double lowerLeftX,double lowerLeftY,double width,double height)
{ 
   m_lowerLeftX  = lowerLeftX;
   m_lowerLeftY  = lowerLeftY;
   m_width       = width;
   m_height      = height;
}

void COdbRectangleOutline::mirrorOutline()
{
   m_lowerLeftX  = -m_lowerLeftX - m_width;
}

void COdbRectangleOutline::buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock)
{
   DataStruct* polyStruct = odbPpDatabase.getCamCadDatabase().addPolyStruct(parentBlock,layerIndex,0,
      false, this->getGraphicClass());

   CPoly* poly = odbPpDatabase.getCamCadDatabase().addClosedPoly(polyStruct,0);

   fillOutlinePoly(odbPpDatabase, *poly);
}

void COdbRectangleOutline::fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly)
{
   poly.addVertex(m_lowerLeftX          ,   m_lowerLeftY           );
   poly.addVertex(m_lowerLeftX + m_width,   m_lowerLeftY           );
   poly.addVertex(m_lowerLeftX + m_width,   m_lowerLeftY + m_height);
   poly.addVertex(m_lowerLeftX          ,   m_lowerLeftY + m_height);
   poly.addVertex(m_lowerLeftX          ,   m_lowerLeftY           );
}


//_____________________________________________________________________________
COdbPolygon::COdbPolygon(double xStart,double yStart,bool holeFlag)
{ 
   m_startPoint.x = xStart;
   m_startPoint.y = yStart;
   m_holeFlag = holeFlag;
}

COdbPolygonSegment* COdbPolygon::addSegment(double x,double y)
{
   COdbPolygonSegment* segment = new COdbPolygonSegment(x,y);
   m_polygonSegmentList.AddTail(segment);

   return segment;
}

COdbPolygonCurve* COdbPolygon::addCurve(double xEnd,double yEnd,double xCenter,double yCenter,bool clockwise)
{
   COdbPolygonCurve* curve = new COdbPolygonCurve(xEnd,yEnd,xCenter,yCenter,clockwise);
   m_polygonSegmentList.AddTail(curve);

   return curve;
}

void COdbPolygon::mirrorOutline()
{
   m_startPoint.x = -m_startPoint.x;

   POSITION pos = m_polygonSegmentList.GetHeadPosition();
   while (pos != NULL)
   {
      COdbPolygonSegment* segment = m_polygonSegmentList.GetNext(pos);
      if (segment != NULL)
      {
         segment->mirrorPolygon();
      }
   }
}

void COdbPolygon::buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,
   BlockStruct* parentBlock)
{
   bool filledFlag = false;
   bool voidFlag   = false;
   int widthIndex  = odbPpDatabase.getCamCadDatabase().getZeroWidthIndex();

   DataStruct* polyStruct = odbPpDatabase.getCamCadDatabase().addPolyStruct(parentBlock,layerIndex,0,
      false, this->getGraphicClass());

   if (m_polygonSegmentList.GetCount() > 0)
   {
      CPoly* poly = odbPpDatabase.getCamCadDatabase().addPoly(polyStruct,widthIndex,filledFlag,m_holeFlag,true);
      fillOutlinePoly(odbPpDatabase, *poly);
   }
}

void COdbPolygon::fillOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &poly)
{
   POSITION pos = m_polygonSegmentList.GetHeadPosition();

   if (pos != NULL)
   {
      // CPnt *lastVertex is needed because bulge may be updated in this previous vertext while processing next vertex
      CPnt* lastVertex = odbPpDatabase.getCamCadDatabase().addVertex(&poly,m_startPoint.x,m_startPoint.y);
      // But as of this writing, CPnt only has floats for xy (DbUnit) and that is not good enough for some of the processing
      // to check coincident points, which needs 1E-9 accuracy. Need to update DbUnit to double !!!
      // So here we keep a double version of the last vertex.
      CPoint2d lastOdbVertex(m_startPoint.x, m_startPoint.y);

      while (pos != NULL)
      {
         COdbPolygonSegment* segment = m_polygonSegmentList.GetNext(pos);
         segment->buildCamCadData(odbPpDatabase,&poly,lastVertex, lastOdbVertex);
      }
   }
}

void COdbPolygon::buildCamCadData(COdbPpDatabase& odbPpDatabase,DataStruct* polyStruct,
                                  bool filledFlag,int widthIndex,bool positivePolarity,
                                  const CString& netName,CWriteFormat* log)
{
   POSITION pos = m_polygonSegmentList.GetHeadPosition();

   if (pos != NULL)
   {

      CPoly* poly = odbPpDatabase.getCamCadDatabase().addPoly(polyStruct,widthIndex,filledFlag,m_holeFlag,true);

#ifdef WANT_DUP_CODE
      // CPnt *lastVertex is needed because bulge may be updated in this previous vertext while processing next vertex
      CPnt* lastVertex = odbPpDatabase.getCamCadDatabase().addVertex(poly,m_startPoint.x,m_startPoint.y);
      // But as of this writing, CPnt only has floats for xy (DbUnit) and that is not good enough for some of the processing
      // to check coincident points, which needs 1E-9 accuracy. Need to update DbUnit to double !!!
      // So here we keep a double version of the last vertex.
      CPoint2d lastOdbVertex(m_startPoint.x, m_startPoint.y);

      while (pos != NULL)
      {
         COdbPolygonSegment* segment = m_polygonSegmentList.GetNext(pos);
         segment->buildCamCadData(odbPpDatabase,poly,lastVertex, lastOdbVertex);
      }
#else
      fillOutlinePoly(odbPpDatabase, *poly);
#endif

      if (log != NULL && !netName.IsEmpty())
      {
         odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(polyStruct),
            ATT_NETNAME,valueTypeString,netName,*log);
      }
   }
}

//_____________________________________________________________________________
COdbFeature::COdbFeature(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber) : 
   m_attributes(5)
{ 
   m_featureId             = featureId;
   m_featureFile           = featureFile;
   m_featureFileLineNumber = featureFileLineNumber;
   m_subnet                = NULL;

   m_instantiatedFlag = false;
}

COdbFeature::COdbFeature(const COdbFeature& other) : m_attributes(other.m_attributes)
{ 
   m_featureId             = other.m_featureId;
   m_featureFile           = other.m_featureFile;
   m_featureFileLineNumber = other.m_featureFileLineNumber;
   m_subnet                = other.m_subnet;

   m_instantiatedFlag      = other.m_instantiatedFlag;
}

COdbFeature::~COdbFeature()
{ 
}

CString COdbFeature::getNetName() 
{
   return ((m_subnet != NULL) ? m_subnet->getNetName() : "");
}

void COdbFeature::setInstantiatedFlag(bool flag)
{
   m_instantiatedFlag = flag;
}

void COdbFeature::addAttribute(COdbFeatureAttribute* attribute)
{ 
   m_attributes.Add(attribute); 
}

void COdbFeature::takeAttributes(COdbFeatureAttributeArray& attributeArray)
{ 
   for (int ind=0;ind < attributeArray.GetSize();ind++)
   {
      COdbFeatureAttribute* attribute = attributeArray.GetAt(ind);
      addAttribute(attribute);
   }

   attributeArray.RemoveAll();
}

void COdbFeature::addDebugProperties(CCamCadDatabase& camCadDatabase,CAttributes** attributeMap,
   CWriteFormat& errorLog)
{
   CString sourceFileLineNumber;
   sourceFileLineNumber.Format("%d",m_featureFileLineNumber);

   camCadDatabase.addAttribute(attributeMap,"sourceFileName" ,valueTypeString ,
      ((m_featureFile == NULL) ? "null" : m_featureFile->getName()),errorLog);
   camCadDatabase.addAttribute(attributeMap,"sourceFileLine#",valueTypeInteger,
      sourceFileLineNumber,errorLog);
}

void COdbFeature::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbFeature: ");
   writeFormat.writef("m_featureId=%d, getFeatureType()=%d\n",
      m_featureId,getFeatureType());

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbFeatureBuildState::COdbFeatureBuildState(COdbPpDatabase& odbPpDatabase,int layerIndex,
   BlockStruct* parentBlock,CWriteFormat& log) :
      m_odbPpDatabase(odbPpDatabase) , m_log(log)
{
   m_layerIndex     = layerIndex;
   m_parentBlock    = parentBlock;
   m_camCadDatabase = &(m_odbPpDatabase.getCamCadDatabase());

   m_polyStruct  = NULL;
   m_poly        = NULL;
   m_lastVertex  = NULL;
   m_widthIndex  = -1;
   m_surfaceGraphicClass = graphicClassEtch;
}

void COdbFeatureBuildState::addLine(const CPoint2d& startPoint,const CPoint2d& endPoint,
   int widthIndex,const CString& netName)
{
   const DbFlag dbFlag = 0;

   CPnt startPnt;
   startPnt.x = (DbUnit)startPoint.x;
   startPnt.y = (DbUnit)startPoint.y;

   bool commonPolyFlag = ((m_poly          != NULL      ) &&
                          (m_widthIndex    == widthIndex) && 
                          (m_netName       == netName   ) && 
                          (m_lastVertex->x == startPnt.x) && 
                          (m_lastVertex->y == startPnt.y)    );

   m_widthIndex = widthIndex;
   m_netName    = netName;

   if (!commonPolyFlag)
   {
      GraphicClassTag graphicClass = ((m_netName == "") ? graphicClassNormal : graphicClassEtch);

      m_polyStruct = m_camCadDatabase->addPolyStruct(
                                    m_parentBlock,m_layerIndex,dbFlag,false,graphicClass);

      m_poly = m_camCadDatabase->addOpenPoly(m_polyStruct,m_widthIndex);
      m_camCadDatabase->addVertex(m_poly,startPnt.x,startPnt.y);

      if (!m_netName.IsEmpty())
      {
         m_camCadDatabase->addAttribute(getAttributeMap(m_polyStruct),
            ATT_NETNAME,valueTypeString,m_netName,m_log);
      }
   }

   m_lastVertex = m_camCadDatabase->addVertex(m_poly,endPoint.x,endPoint.y);
}

void COdbFeatureBuildState::addArc(const CPoint2d& startPoint,const CPoint2d& endPoint,
   const CPoint2d& centerPoint,bool clockwise,int widthIndex,const CString& netName)
{
   const DbFlag dbFlag = 0;

   CPnt startPnt;
   startPnt.x = (DbUnit)startPoint.x;
   startPnt.y = (DbUnit)startPoint.y;

   bool commonPolyFlag = ((m_poly          != NULL      ) &&
                          (m_widthIndex    == widthIndex) && 
                          (m_netName       == netName   ) && 
                          (m_lastVertex->x == startPnt.x) && 
                          (m_lastVertex->y == startPnt.y)    );

   m_widthIndex = widthIndex;
   m_netName    = netName;

   if (!commonPolyFlag)
   {
      GraphicClassTag graphicClass = ((m_netName == "") ? graphicClassNormal : graphicClassEtch);

      DataStruct* polyStruct = m_camCadDatabase->addPolyStruct(
                                    m_parentBlock,m_layerIndex,dbFlag,false,graphicClass);

      m_poly = m_camCadDatabase->addOpenPoly(polyStruct,m_widthIndex);
      m_lastVertex = NULL;
      m_camCadDatabase->addVertex(m_poly,startPnt.x,startPnt.y);

      if (!m_netName.IsEmpty())
      {
         m_camCadDatabase->addAttribute(getAttributeMap(polyStruct),
            ATT_NETNAME,valueTypeString,m_netName,m_log);
      }
   }

   if (fpeq(startPoint.x,endPoint.x,EXT_SMALL_NUMBER) &&
       fpeq(startPoint.y,endPoint.y,EXT_SMALL_NUMBER)    )
   {
      // full circle
      double dx = centerPoint.x - startPoint.x;
      double dy = centerPoint.y - startPoint.y;
      double x = startPoint.x + 2. * dx;
      double y = startPoint.y + 2. * dy;

      if (m_lastVertex != NULL)
      {
         m_lastVertex->bulge = 1.0;
      }
      else
      {
         m_camCadDatabase->addVertex(m_poly,startPnt.x,startPnt.y,1.);
      }

      m_camCadDatabase->addVertex(m_poly,        x,        y,1.);
   }
   else
   {
      double radius,startAngle,deltaAngle;

      ArcCenter2(startPoint.x,startPoint.y,endPoint.x,endPoint.y,centerPoint.x,centerPoint.y,
         &radius,&startAngle,&deltaAngle,clockwise);

      double bulge = tan(deltaAngle / 4.);

      if (fabs(bulge) > 572.)
      {
         bulge = 0.;
      }

      if (m_lastVertex != NULL)
      {
         m_lastVertex->bulge = (DbUnit)bulge;
      }
      else
      {
         m_camCadDatabase->addVertex(m_poly,startPoint.x,startPoint.y,bulge);
      }
   }

   m_lastVertex = m_camCadDatabase->addVertex(m_poly,endPoint.x,endPoint.y);
}

void COdbFeatureBuildState::flush()
{
   m_poly        = NULL;
}

//_____________________________________________________________________________
COdbInvalidFeature::COdbInvalidFeature(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber) : 
   COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
}

//_____________________________________________________________________________
COdbLine::COdbLine(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
   double xStart,double yStart,double xEnd,double yEnd,
   COdbFeatureSymbol* symbol,bool positivePolarity,int dcode) : 
      COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
   m_start.x = xStart;
   m_start.y = yStart;
   m_end.x   = xEnd;
   m_end.y   = yEnd;
   m_featureSymbol    = symbol;
   m_positivePolarity = positivePolarity;
   m_dcode   = dcode;
}

void COdbLine::instantiateCamCadData(COdbFeatureBuildState& featureBuildState)
{
   int widthIndex = 0;
   
   if (m_featureSymbol != NULL)
   {
      widthIndex = m_featureSymbol->getWidthIndex(featureBuildState.getOdbPpDatabase());
   }

   CString netName(getNetName());
   featureBuildState.addLine(m_start, m_end, widthIndex, netName);
}

//_____________________________________________________________________________
COdbArc::COdbArc(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double xStart,double yStart,double xEnd,double yEnd,
      double xCenter,double yCenter,COdbFeatureSymbol* symbol,bool positivePolarity,
      int dcode,bool clockwise) : 
   COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
   m_start.x   = xStart;
   m_start.y   = yStart;
   m_end.x     = xEnd;
   m_end.y     = yEnd;
   m_center.x  = xCenter;
   m_center.y  = yCenter;
   m_featureSymbol    = symbol;
   m_positivePolarity = positivePolarity;
   m_dcode     = dcode;
   m_clockwise = clockwise;
}

void COdbArc::instantiateCamCadData(COdbFeatureBuildState& featureBuildState)
{
   int widthIndex = 0;
   
   if (m_featureSymbol != NULL)
   {
      widthIndex = m_featureSymbol->getWidthIndex(featureBuildState.getOdbPpDatabase());
   }

   CString netName = getNetName();

   featureBuildState.addArc(m_start,m_end,m_center,m_clockwise,widthIndex,netName);
}

COdbPad::COdbPad(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double x,double y,COdbFeatureSymbol* symbol,bool positivePolarity,
      int dcode,double rotation,bool mirrorFlag) : 
   COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
   m_origin.x    = x;
   m_origin.y    = y;
   m_featureSymbol    = symbol;
   m_positivePolarity = positivePolarity;
   m_dcode       = dcode;

   //if (rotation > 180 && rotation < 190)
   //{
   //   int jj = 0;
   //}


   if (rotation < 0)
   {
      double otherrot = -rotation;
      //if (otherrot > 60. && otherrot < 70.)
      //{
      //   int jj = 0;
      //}

   }

   m_rotation   = normalizeDegrees(rotation);
   //*rcf debug
   //if (m_rotation != 0 && m_rotation > -10 && m_rotation < 10)
   //{
   //   int jj = 0;
   //}


   m_mirrorFlag = mirrorFlag;

   m_padUsage         = padUsageUndefined;
}

COdbPad::COdbPad(const COdbPad& other) : 
   COdbFeature(other)
{ 
   m_origin           = other.m_origin;
   m_featureSymbol    = other.m_featureSymbol;
   m_positivePolarity = other.m_positivePolarity;
   m_dcode            = other.m_dcode;
   m_rotation         = other.m_rotation;
   m_mirrorFlag       = other.m_mirrorFlag;

   m_padUsage         = other.m_padUsage;
}

void COdbPad::addAttribute(COdbFeatureAttribute* attribute)
{ 
   COdbFeature::addAttribute(attribute); 

   if (attribute->getName().CompareNoCase(".pad_usage") == 0)
   {
      m_padUsage = intToOdbPadUsageTag(atoi(attribute->getStringValue()));
   }
}

void COdbPad::transform(CTMatrix& matrix,double deltaAngle)
{
   matrix.transform(m_origin);

   m_rotation -= deltaAngle;

   double rotation = ((DcaRound(m_rotation) / 90 ) * 90);

   if (fpeq3(rotation,m_rotation))
   {
      m_rotation = rotation;
   }
}

PadstackCompareStatusTag COdbPad::compare(const COdbPad& other) const
{
   PadstackCompareStatusTag retval = padstackCompareEquivalent;

   if (m_dcode != other.m_dcode)
   {
      retval = min(retval,padstackCompareDcode);
   }

   if (m_positivePolarity != other.m_positivePolarity)
   {
      retval = min(retval,padstackComparePolarity);
   }

   if (!fpeq3(m_origin.x,other.m_origin.x) ||
       !fpeq3(m_origin.y,other.m_origin.y)    )
   {
      retval = min(retval,padstackCompareOffset);
   }

   if (m_featureSymbol != NULL && other.m_featureSymbol != NULL)
   {
      if (! m_featureSymbol->isEquivalent(*(other.m_featureSymbol)))
      {
         retval = min(retval,padstackComparePadType);
      }

      if (! angleSymmetryEqual(      m_rotation,      m_featureSymbol->getSymmetry(),
                               other.m_rotation,other.m_featureSymbol->getSymmetry()) )
      {
         retval = min(retval,padstackCompareRotation);
      }
   }
   else if (m_featureSymbol != NULL || other.m_featureSymbol != NULL)
   {
      retval = min(retval,padstackComparePadType);
   }

   return retval;
}

bool COdbPad::isEquivalent(const COdbPad& other) const
{
   bool retval = (compare(other) == padstackCompareEquivalent);

   return retval;
}

void COdbPad::instantiateCamCadData(COdbFeatureBuildState& featureBuildState)
{
   if (m_featureSymbol != NULL)
   {
      BlockStruct* apertureBlock = m_featureSymbol->getDefinedAperture(featureBuildState.getOdbPpDatabase(),
         featureBuildState.getLog());

      int insertType = INSERTTYPE_FREEPAD;

      switch (m_padUsage)
      {
      case padUsageToeprint:        insertType = insertTypePin;        break;
      case padUsageVia:             insertType = insertTypeVia;        break;
      case padUsageGlobalFiducial:  insertType = insertTypeFiducial;   break;
      case padUsageLocalFiducial:   insertType = insertTypeFiducial;   break;
      case padUsageToolingHole:     insertType = insertTypeDrillTool;  break;
      }

      int layerIndx = featureBuildState.getLayerIndex();
      bool mirror = false;
      // Not sure if this is the best place, we'll maybe consider that more later.
      // For now, this works. Bottom side fids were coming in as Placed Top with
      // visible graphic on a Bottom layer. Change to Placed Bottom with Mirror in
      // the conventional fashion. Treating fids only, but possibly some of the
      // other types ought to get this too. Time will tell.
      if (insertType == insertTypeFiducial)
      {
         bool padMir = this->getMirror();

         LayerStruct *layer = featureBuildState.getCamCadDatabase().getLayerAt(layerIndx);
         if (layer != NULL)
         {
            if (layer->isLayerTypeBottomSurface())
            {
               int mirrorLayerIndx = layer->getMirroredLayerIndex();
               if (mirrorLayerIndx != layerIndx)
               {
                  mirror = true;
                  layerIndx = mirrorLayerIndx;
               }
            }
         }
      }

      DataStruct* padInsert = featureBuildState.getCamCadDatabase().referenceBlock(featureBuildState.getParentBlock(),
         apertureBlock,insertType,"",layerIndx,
         m_origin.x,m_origin.y,degreesToRadians(m_rotation), mirror);

      if (getOdbOptions().getOptionFlag(optionEnableDebugProperties))
      {
         addDebugProperties(featureBuildState.getOdbPpDatabase().getCamCadDatabase(),getAttributeMap(padInsert),
            featureBuildState.getLog());
      }
   }

   featureBuildState.flush();
}

void COdbPad::buildCamCadData(COdbPpDatabase& odbPpDatabase,int layerIndex,BlockStruct* parentBlock,CWriteFormat& log)
{
   if (m_featureSymbol != NULL)
   {
      BlockStruct* apertureBlock = m_featureSymbol->getDefinedAperture(odbPpDatabase,log);
   }
}

void COdbPad::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbPad: ");
   writeFormat.writef(
"m_origin=(%.3f,%.3f), m_positivePolarity=%d, m_dcode=%d, m_rotation=%.3f, m_instantiatedFlag=%d\n",
m_origin.x,m_origin.y,m_positivePolarity,m_dcode,m_rotation,getInstantiatedFlag());

   if (depth != 0)
   {
      if (m_featureSymbol != NULL)
      {
         m_featureSymbol->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbPadGeometry::COdbPadGeometry(const COdbPad& pad) : COdbPad(pad)
{ 
   m_block = NULL;
}

void COdbPadGeometry::buildCamCadData(COdbPpDatabase& odbPpDatabase,const CString& padApertureName,
   CWriteFormat& log)
{
   m_block = odbPpDatabase.getCamCadDatabase().getDefinedAperture(padApertureName,
      getApertureShape(),getSizeA(),getSizeB(),
      getX(),getY(),getRotation());

   COdbPad::buildCamCadData(odbPpDatabase,NoLayer,m_block,log);
}

//_____________________________________________________________________________
COdbText::COdbText(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
      double x,double y,const CString& font,bool positivePolarity,
      double rotation,bool mirrorFlag,double xSize,double ySize,double widthFactor,const CString& text,
      bool currentVersion) : 
   COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
   m_origin.x         = x;
   m_origin.y         = y;
   m_font             = font;
   m_positivePolarity = positivePolarity;
   m_rotation         = rotation;
   m_mirrorFlag       = mirrorFlag;
   m_xSize            = xSize;
   m_ySize            = ySize;
   m_widthFactor      = widthFactor;
   m_text             = text;
   m_currentVersion   = currentVersion;
}

void COdbText::instantiateCamCadData(COdbFeatureBuildState& featureBuildState)
{
   const DbFlag dbFlag = 0;

   bool proportional = false;
   bool oblique      = false;
   bool negative     = false;

   const double textWidthUnit = getOdbUnits().convertFrom(pageUnitsInches,.012);  //CUnits::convertInchesToPageUnits(.012);
   int penWidthIndex = featureBuildState.getCamCadDatabase().getDefinedWidthIndex(m_widthFactor * textWidthUnit);

   // Text width, aka xsize, in ODB includes inter-char spacing.
   // Text width in CCZ is just the char size, inter-char spacing is separate.
   // Adjust the ODB char width per current inter-char spacing factor, aka TextSpacingRatio.
   double textSpaceRatio = featureBuildState.getCamCadDatabase().getCamCadDoc().getSettings().getTextSpaceRatio();
   // Our writer output size factored by (1. + textSpaceRatio). This calc undoeos that.
   double adjustedXsize = m_xSize / (1. + textSpaceRatio);

   DataStruct* textData = featureBuildState.getCamCadDatabase().addText(featureBuildState.getParentBlock(),
      featureBuildState.getLayerIndex(),m_text,m_origin.x,m_origin.y, m_ySize, adjustedXsize,
      degreesToRadians(m_rotation),dbFlag,proportional,m_mirrorFlag,
      oblique,negative,penWidthIndex);

   if (getOdbOptions().getOptionFlag(optionEnableDebugProperties))
   {
      addDebugProperties(featureBuildState.getOdbPpDatabase().getCamCadDatabase(),getAttributeMap(textData),
         featureBuildState.getLog());
   }

   featureBuildState.flush();
}

//_____________________________________________________________________________
COdbBarcode::COdbBarcode(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber) : 
   COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
}

//_____________________________________________________________________________
COdbSurface::COdbSurface(int featureId,COdbFeatureFile* featureFile,int featureFileLineNumber,
   bool positivePolarity,int dcode) : 
   COdbFeature(featureId,featureFile,featureFileLineNumber)
{ 
   m_positivePolarity = positivePolarity;
   m_dcode            = dcode;
}

COdbPolygon* COdbSurface::addPolygon(double xStart,double yStart,bool holeFlag)
{
   COdbPolygon* polygon = new COdbPolygon(xStart,yStart,holeFlag);
   m_polygonList.AddTail(polygon);

   return polygon;
}

void COdbSurface::instantiateCamCadData(COdbFeatureBuildState& featureBuildState)
{
   CString netName = getNetName();

   DataStruct* polyStruct = featureBuildState.getOdbPpDatabase().getCamCadDatabase().addPolyStruct(
      featureBuildState.getParentBlock(),featureBuildState.getLayerIndex(),0,!m_positivePolarity,
      featureBuildState.getSurfaceGraphicClass());

   bool filledFlag = true;
   int widthIndex  = 0;
   
   if (featureBuildState.getSurfaceGraphicClass() == GR_CLASS_BOARDOUTLINE ||
       featureBuildState.getSurfaceGraphicClass() == GR_CLASS_PANELOUTLINE    )
   {
      filledFlag = false;
      widthIndex = featureBuildState.getCamCadDatabase().getDefinedWidthIndex(getOdbUnits().convertFrom(pageUnitsInches,.024));
   }
   else
   {
      LayerStruct* layerStruct = featureBuildState.getCamCadDatabase().getLayerAt(featureBuildState.getLayerIndex());

      if (layerStruct->getLayerType() == LAYTYPE_SPLITPLANE)
      {
         filledFlag = false;
      }
   }

   for (POSITION pos = m_polygonList.GetHeadPosition();pos != NULL;)
   {
      COdbPolygon* polygon = m_polygonList.GetNext(pos);

      polygon->buildCamCadData(featureBuildState.getOdbPpDatabase(),polyStruct,filledFlag,widthIndex,
         m_positivePolarity,netName,&(featureBuildState.getLog()));
   }

   if (getOdbOptions().getOptionFlag(optionEnableDebugProperties))
   {
      addDebugProperties(featureBuildState.getOdbPpDatabase().getCamCadDatabase(),getAttributeMap(polyStruct),
         featureBuildState.getLog());
   }
}

//_____________________________________________________________________________
COdbFeatureArray::COdbFeatureArray(int size,bool isContainer)
{
   m_isContainer = isContainer;

   m_featureArray.SetSize(0,size);
}

COdbFeatureArray::~COdbFeatureArray()
{
   empty();
}

void COdbFeatureArray::empty()
{
   if (m_isContainer)
   {
      for (int ind=0;ind < m_featureArray.GetSize();ind++)
      {
         COdbFeature* feature = m_featureArray.GetAt(ind);
         delete feature;
      }
   }

   m_featureArray.RemoveAll();
}

COdbFeature* COdbFeatureArray::getAt(int index)
{
   COdbFeature* feature = NULL;

   if (index >= 0 && index < m_featureArray.GetSize())
   {
      feature = m_featureArray.GetAt(index);
   }

   return feature;
}

COdbInvalidFeature* COdbFeatureArray::addInvalidFeature(COdbFeatureFile* featureFile,int lineNumber)
{
   int id = m_featureArray.GetSize();
   COdbInvalidFeature* feature = new COdbInvalidFeature(id,featureFile,lineNumber);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

COdbLine* COdbFeatureArray::addLine(COdbFeatureFile* featureFile,int lineNumber,
   double xStart,double yStart,double xEnd,double yEnd,
   COdbFeatureSymbol* symbol,bool positivePolarity,int dcode)
{
   int id = m_featureArray.GetSize();
   COdbLine* feature = new COdbLine(id,featureFile,lineNumber,xStart,yStart,xEnd,yEnd,symbol,
      positivePolarity,dcode);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

COdbArc* COdbFeatureArray::addArc(COdbFeatureFile* featureFile,int lineNumber,
   double xStart,double yStart,double xEnd,double yEnd,
   double xCenter,double yCenter,COdbFeatureSymbol* symbol,bool positivePolarity,
   int dcode,bool clockwise)
{
   int id = m_featureArray.GetSize();
   COdbArc* feature = new COdbArc(id,featureFile,lineNumber,
      xStart,yStart,xEnd,yEnd,xCenter,yCenter,symbol,
      positivePolarity,dcode,clockwise);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

COdbPad* COdbFeatureArray::addPad(COdbFeatureFile* featureFile,int lineNumber,
   double x,double y,COdbFeatureSymbol* symbol,bool positivePolarity,
   int dcode,double rotation,bool mirrorFlag)
{
   int id = m_featureArray.GetSize();
   COdbPad* feature = new COdbPad(id,featureFile,lineNumber,x,y,symbol,positivePolarity,
      dcode,rotation,mirrorFlag);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

COdbText* COdbFeatureArray::addText(COdbFeatureFile* featureFile,int lineNumber,double x,double y,const CString& font,bool positivePolarity,
   double rotation,bool mirrorFlag,double xSize,double ySize,double widthFactor,const CString& text,
   bool currentVersion)
{
   int id = m_featureArray.GetSize();
   COdbText* feature = new COdbText(id,featureFile,lineNumber,
      x,y,font,positivePolarity,rotation,mirrorFlag,xSize,ySize,
      widthFactor,text,currentVersion);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

COdbBarcode* COdbFeatureArray::addBarcode(COdbFeatureFile* featureFile,int lineNumber)
{
   int id = m_featureArray.GetSize();
   COdbBarcode* feature = new COdbBarcode(id,featureFile,lineNumber);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

COdbSurface* COdbFeatureArray::addSurface(COdbFeatureFile* featureFile,int lineNumber,
   bool positivePolarity,int dcode)
{
   int id = m_featureArray.GetSize();
   COdbSurface* feature = new COdbSurface(id,featureFile,lineNumber,positivePolarity,dcode);
   m_featureArray.SetAtGrow(id,feature);

   return feature;
}

void COdbFeatureArray::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbFeatureArray: ");
   writeFormat.writef("getCount()=%d\n",getCount());

   if (depth != 0)
   {
      for (int ind = 0;ind < m_featureArray.GetSize();ind++)
      {
         COdbFeature* feature = m_featureArray.GetAt(ind);
         feature->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbPad* COdbPadArray::getAt(int index)
{
   COdbPad* pad = NULL;

   if (index >= 0 && index < GetSize())
   {
      pad = GetAt(index);
   }

   return pad;
}

//_____________________________________________________________________________
COdbFeatureFile::COdbFeatureFile(const CString& name,int id)
{
   m_name  = name;
   m_id    = id;
   m_layer = NULL;
   m_block = NULL;
}

COdbFeatureFile::COdbFeatureFile(COdbLayer& layer,int id)
{
   m_name  = layer.getName();
   m_id    = id;
   m_layer = &layer;
   m_block = NULL;
}

bool COdbFeatureFile::readLayerFeatures(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
   CWriteFormat& log)
{
   CFilePath featureFilePath = odbPpDatabase.getDatabaseFilePath();
   featureFilePath.pushLeaf("steps");
   featureFilePath.pushLeaf(step.getName());

   if (layer.getType() == odbLayerTypeProfile)
   {
      featureFilePath.pushLeaf(layer.getName());
   }
   else
   {
      featureFilePath.pushLeaf("layers");
      featureFilePath.pushLeaf(layer.getName());
      featureFilePath.pushLeaf("features");
   }

   m_featureAttributes.setLayer(&layer);

   bool retval = readFeatures(odbPpDatabase,featureFilePath.getPath(),log);

   return retval;
}

bool COdbFeatureFile::readSymbolFeatures(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   CFilePath featureFilePath = odbPpDatabase.getDatabaseFilePath();
   featureFilePath.pushLeaf("symbols");
   featureFilePath.pushLeaf(m_name);
   featureFilePath.pushLeaf("features");

   bool retval = readFeatures(odbPpDatabase,featureFilePath.getPath(),log);

   return retval;
}

bool COdbFeatureFile::readFeatures(COdbPpDatabase& odbPpDatabase,
      const CString& featureFilePathString,CWriteFormat& log)
{
   m_filePath.setPath(featureFilePathString);

   bool retval = true;
   updateToExistingCompressableFilePath(m_filePath,log);
   CString featureFilePath = m_filePath.getPath();
   //bool retval = getOdbCompressableFileCache().uncompress(featureFilePath,log);

   while (retval)
   {
      COdbAppFileReader fileReader;

      if (fileReader.openFile(featureFilePath) != statusSucceeded)
      {
log.writef(PrefixError,
"Could not read the Feature file, '%s' - status = '%s'.\n",
(const char*)featureFilePath,fileStatusTagToString(fileReader.getOpenStatus()));

         retval = false;

         break;
      }
      double current_unitfactor = 1; //default pageunit is inch
      
      fileReader.setInchesPerUnit(odbPpDatabase.getInchesPerUnit());
      fileReader.setPageUnitsPerUnit(odbPpDatabase.getPageUnitsPerUnit());

      displayProgress(log,"Reading Feature file, '%s'",(const char*)featureFilePath);

      COdbSurface* surface = NULL;
      COdbPolygon* polygon = NULL;

      COdbFeature* feature = NULL;

      m_featureAttributes.initializeSettings(getOdbOptions().getFeatureAttributes());

      CString line;
      int numParams;
      int lineNumber;

      CFileReadProgress fileReadProgress(fileReader);

      while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         lineNumber = fileReader.getNumRecordsRead();
         line       = fileReader.getRecord();
         numParams  = fileReader.getNumParams();

         if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

         if (numParams < 1 || fileReader.getParam(0).GetLength() == 0)
         {
            continue;
         }

         CString param0 = fileReader.getParam(0);
         param0.MakeUpper();

         const char* recordType0 = param0.GetBuffer();
         const char* recordType  = recordType0;

         unsigned int recordCode =  getRecordCode(recordType0);

         switch (recordCode)
         {
         case RecordCode1(F): // in version 7.0
            break;
         case RecordCode1(U): // in version 7.0
            if(numParams >= 1) 
            {
               CString feature_units = fileReader.getParam(1);               
               if (!feature_units.CompareNoCase("MM"))
               {
                  current_unitfactor = getUnitsFactor(pageUnitsMilliMeters,pageUnitsInches);
               }

               fileReader.setInchesPerUnit(odbPpDatabase.getInchesPerUnit()* current_unitfactor);
               fileReader.setPageUnitsPerUnit(odbPpDatabase.getPageUnitsPerUnit()* current_unitfactor);
            }
            break;
         case RecordCode1(#):  // comment
            break;
         case RecordCode1($):  // feature symbol names
            if (numParams > 1)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString name = fileReader.getParam(1);
               
               double semi_unitfactor = 1; // in version 7.0, transform to INCH
               if(!fileReader.getParam(2).CompareNoCase("I")) // unit inch
               {
                  semi_unitfactor = 1;
               }
               else if(!fileReader.getParam(2).CompareNoCase("M")) //unit mm
               {
                  semi_unitfactor = getUnitsFactor(pageUnitsMilliMeters,pageUnitsInches);
               }
               else // default unit
               {
                  semi_unitfactor = current_unitfactor;
               }

               COdbFeatureSymbol* symbol = odbPpDatabase.getFeatureSymbol(name);
               symbol->setUnitFactor(semi_unitfactor);

               if (m_featureSymbolArray.GetSize() == serialNumber)
               {
                  m_featureSymbolArray.Add(symbol);
               }
               else
               {
                  log.writef(PrefixError,
                     "Feature symbol sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureSymbolArray.GetSize(),serialNumber,
                     (const char*)featureFilePath,lineNumber);

                  if (serialNumber >= m_featureSymbolArray.GetSize())
                  {
                     m_featureSymbolArray.SetAtGrow(serialNumber,symbol);
                  }
               }
            }

            break;
         case RecordCode1(@):  // feature attribute names
            if (numParams > 1)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString name = fileReader.getParam(1);

               if (m_featureAttributes.getAttributeNameCount() != serialNumber)
               {
                  log.writef(PrefixError,
                     "Feature attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureAttributes.getAttributeNameCount(),serialNumber,
                     (const char*)featureFilePath,lineNumber);
               }

               m_featureAttributes.addAttributeName(name);
            }

            break;
         case RecordCode1(&):  // feature attribute text strings
            if (numParams > 1)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString textString = ((numParams > 1) ? fileReader.getParam(1) : "");

               if (m_featureAttributes.getAttributeTextStringCount() != serialNumber)
               {
log.writef(PrefixError,
"Feature attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
m_featureAttributes.getAttributeTextStringCount(),serialNumber,
(const char*)featureFilePath,lineNumber);
               }

               m_featureAttributes.addAttributeTextString(textString);
            }

            break;
         case RecordCode1(L):  // Line
            if (numParams < 8)
            {
               feature = m_featureArray.addInvalidFeature(this,lineNumber);
            }
            else
            {
               double xStart,yStart,xEnd,yEnd;
               fileReader.getParamInPageUnits(xStart,1);
               fileReader.getParamInPageUnits(yStart,2);
               fileReader.getParamInPageUnits(xEnd,3);
               fileReader.getParamInPageUnits(yEnd,4);

               int featureSymbolSerialNumber = atoi(fileReader.getParam(5));
               bool positivePolarity = (fileReader.getParam(6).CompareNoCase("P") == 0);
               int dCode     = atoi(fileReader.getParam(7));

               COdbFeatureSymbol* symbol = m_featureSymbolArray.getAt(featureSymbolSerialNumber,log,
                                              line,featureFilePath,lineNumber);

               feature = m_featureArray.addLine(this,lineNumber,xStart,yStart,xEnd,yEnd,symbol,positivePolarity,dCode);
               m_featureAttributes.addAttributes(*feature,fileReader,8,
                  featureFilePath,line,lineNumber,log);
            }

            break;
         case RecordCode1(P):  // Pad
            if (numParams < 7)
            {
               feature = m_featureArray.addInvalidFeature(this,lineNumber);
            }
            else
            {
               double x,y,resize;
               fileReader.getParamInPageUnits(x,1);
               fileReader.getParamInPageUnits(y,2);

               int featureSymbolSerialNumber = atoi(fileReader.getParam(3));

               int aptdef_offset = 4; //in version 7.0, shift 2 for resize
               if(featureSymbolSerialNumber == -1) 
               {
                  featureSymbolSerialNumber = atoi(fileReader.getParam(aptdef_offset++));
                  fileReader.getParamInPageUnits(resize,aptdef_offset++);
               }

               bool positivePolarity = (fileReader.getParam(aptdef_offset++).CompareNoCase("P") == 0);
               int dCode     = atoi(fileReader.getParam(aptdef_offset++));
               int orientation   = atoi(fileReader.getParam(aptdef_offset++));
               
               double angle = 0;
               bool mirror = 0;              
               if(orientation < 8)
               {
                  angle = -orientationToDegrees(orientation);
                  mirror = orientationToMirror(orientation);                  
               }
               else  //in version 7.0, shift 1 for angle
               {
                  angle = -atof(fileReader.getParam(aptdef_offset++));
                  mirror = (orientation == 9);
               }

               COdbFeatureSymbol* symbol = m_featureSymbolArray.getAt(featureSymbolSerialNumber,log,
                                              line,featureFilePath,lineNumber);

               feature = m_featureArray.addPad(this,lineNumber,x,y,symbol,positivePolarity,dCode,angle,mirror);
               m_featureAttributes.addAttributes(*feature,fileReader,aptdef_offset,
                  featureFilePath,line,lineNumber,log);
            }

            break;
         case RecordCode1(A):  // Arc
            if (numParams < 11)
            {
               feature = m_featureArray.addInvalidFeature(this,lineNumber);
            }
            else
            {
               double xStart,yStart,xEnd,yEnd,xCenter,yCenter;
               fileReader.getParamInPageUnits(xStart,1);
               fileReader.getParamInPageUnits(yStart,2);
               fileReader.getParamInPageUnits(xEnd,3);
               fileReader.getParamInPageUnits(yEnd,4);
               fileReader.getParamInPageUnits(xCenter,5);
               fileReader.getParamInPageUnits(yCenter,6);

               int featureSymbolSerialNumber = atoi(fileReader.getParam(7));
               bool positivePolarity = (fileReader.getParam(8).CompareNoCase("P") == 0);
               int dCode             = atoi(fileReader.getParam(9));
               bool clockwise        = (fileReader.getParam(10).CompareNoCase("Y") == 0);

               COdbFeatureSymbol* symbol = m_featureSymbolArray.getAt(featureSymbolSerialNumber,log,
                                              line,featureFilePath,lineNumber);

               feature = m_featureArray.addArc(this,lineNumber,xStart,yStart,xEnd,yEnd,xCenter,yCenter,
                  symbol,positivePolarity,dCode,clockwise);
               m_featureAttributes.addAttributes(*feature,fileReader,12,
                  featureFilePath,line,lineNumber,log);
            }

            break;
         case RecordCode1(T):  // Text
            if (numParams < 11)
            {
               feature = m_featureArray.addInvalidFeature(this,lineNumber);
            }
            else
            {
               double x,y,xSize,ySize,penWidth;
               fileReader.getParamInPageUnits(x,1);
               fileReader.getParamInPageUnits(y,2);
               
               CString& fontName     = fileReader.getParam(3);
               bool positivePolarity = (fileReader.getParam(4).CompareNoCase("P") == 0);
               int orientation       = atoi(fileReader.getParam(5));
               
               double angle = 0;
               bool mirror = 0;
               int attr_shift = 6;
               if(orientation < 8)
               {
                  angle = -orientationToDegrees(orientation);
                  mirror = orientationToMirror(orientation); 
               }
               else //in version 7.0
               {
                  angle = -atoi(fileReader.getParam(attr_shift++));
                  mirror = (orientation == 9);
               }

               fileReader.getParamInPageUnits(xSize,attr_shift++);
               fileReader.getParamInPageUnits(ySize,attr_shift++);
               fileReader.getParamInPageUnits(penWidth,attr_shift++);
               penWidth = penWidth / 1000.;

               CString text          = fileReader.getParam(attr_shift++);
               bool currentVersion   = (atoi(fileReader.getParam(attr_shift++)) == 1);

               // Text field in these records is (or may be) delimited by single quotes.
               // They are not part of the data, get rid of them.
               text.TrimLeft("'");
               text.TrimRight("'");

               feature = m_featureArray.addText(this,lineNumber,x,y,fontName,positivePolarity,
                  angle,mirror,xSize,ySize,
                  penWidth,text,currentVersion);
               m_featureAttributes.addAttributes(*feature,fileReader,attr_shift,
                  featureFilePath,line,lineNumber,log);
            }

            break;
         case RecordCode1(B):  // Barcode
            feature = m_featureArray.addBarcode(this,lineNumber);

            break;
         case RecordCode1(S):  // Surface
            if (numParams < 3)
            {
               feature = m_featureArray.addInvalidFeature(this,lineNumber);
            }
            else
            {
               bool positivePolarity = (fileReader.getParam(1).CompareNoCase("P") == 0);
               int dCode             = atoi(fileReader.getParam(2));

               surface = m_featureArray.addSurface(this,lineNumber,positivePolarity,dCode);
               feature = surface;
               m_featureAttributes.addAttributes(*feature,fileReader,3,
                  featureFilePath,line,lineNumber,log);
            }

            break;
         case RecordCode2(S,E):  // end of surface
            if (surface != NULL)
            {
               surface = NULL;
            }
            else
            {
               log.writef(PrefixError,
                  "Surface record code of '%s' encountered without a preceding surface record.  Record='%s' in '%s'.%d\n",
                  recordType0,(const char*)line,(const char*)featureFilePath,lineNumber);
            }

            break;
         case RecordCode2(O,B):  // start of polygon
            if (surface != NULL)
            {
               if (numParams >= 4)
               {
                  double xStart,yStart;
                  fileReader.getParamInPageUnits(xStart,1);
                  fileReader.getParamInPageUnits(yStart,2);
                  bool polyTypeHoleFlag = (fileReader.getParam(3).CompareNoCase("H") == 0);

                  polygon = surface->addPolygon(xStart,yStart,polyTypeHoleFlag);
               }
               else
               {
                  log.writef(PrefixError,
                     "Not enough parameters for surface record.  Record='%s' in '%s'.%d\n",
                     (const char*)line,(const char*)featureFilePath,lineNumber);
               }
            }
            else
            {
               log.writef(PrefixError,
                  "Surface record code of '%s' encountered without a preceding surface record.  Record='%s' in '%s'.%d\n",
                  recordType0,(const char*)line,(const char*)featureFilePath,lineNumber);
            }

            break;
         case RecordCode2(O,S):  // polygon segment
            if (polygon != NULL)
            {
               if (numParams >= 3)
               {
                  double x,y;
                  fileReader.getParamInPageUnits(x,1);
                  fileReader.getParamInPageUnits(y,2);

                  polygon->addSegment(x,y);
               }
            }
            else
            {
               log.writef(PrefixError,
                  "Surface record code of '%s' encountered without a preceding polygon record.  Record='%s' in '%s'.%d\n",
                  recordType0,(const char*)line,(const char*)featureFilePath,lineNumber);
            }

            break;
         case RecordCode2(O,C):  // polygon curve
            if (polygon != NULL)
            {
               double xEnd,yEnd,xCenter,yCenter;
               fileReader.getParamInPageUnits(xEnd,1);
               fileReader.getParamInPageUnits(yEnd,2);
               fileReader.getParamInPageUnits(xCenter,3);
               fileReader.getParamInPageUnits(yCenter,4);
               bool clockwise        = (fileReader.getParam(5).CompareNoCase("Y") == 0);

               polygon->addCurve(xEnd,yEnd,xCenter,yCenter,clockwise);
            }
            else
            {
               log.writef(PrefixError,
                  "Surface record code of '%s' encountered without a preceding polygon record.  Record='%s' in '%s'.%d\n",
                  recordType0,(const char*)line,(const char*)featureFilePath,lineNumber);
            }

            break;
         case RecordCode2(O,E):  // end of polygon
            if (polygon != NULL)
            {
               polygon = NULL;
            }
            else
            {
               log.writef(PrefixError,
                  "Surface record code of '%s' encountered without a preceding polygon record.  Record='%s' in '%s'.%d\n",
                  recordType0,(const char*)line,(const char*)featureFilePath,lineNumber);
            }

            break;
         default:
            log.writef(PrefixError,
               "Unrecognized record code of '%s' encountered.  Record='%s' in '%s'.%d\n",
               recordType0,(const char*)line,(const char*)featureFilePath,lineNumber);

            break;
         }
      }

      fileReader.close();
      fileReader.writeReadStatistics(&log,PrefixStatus);

      break;
   }

   hideProgress();

   return retval;
}

BlockStruct* COdbFeatureFile::instantiateFeatures(COdbPpDatabase& odbPpDatabase,BlockStruct* parentBlock,CWriteFormat& log)
{
   if (m_block == NULL)
   {
      displayProgress(log,"Instantiating Features for '%s'.",(const char*)m_name);
      COperationProgress operationProgress(m_featureArray.getCount());

      int layerIndex = 0;
      GraphicClassTag surfaceGraphicClass = graphicClassNormal;

      if (m_layer != NULL)  // layer feature file
      {
         if (m_layer->getCCZLayerStruct() != NULL)
         {
            layerIndex = m_layer->getCCZLayerStruct()->getLayerIndex();

            if (m_layer->getType() == odbLayerTypeProfile)
            { 
               surfaceGraphicClass = graphicClassBoardOutline;

               if (parentBlock != NULL)
               {
                  FileStruct* file = odbPpDatabase.getCamCadDatabase().getCamCadDoc().Find_File(parentBlock->getFileNumber());

                  if (file != NULL && file->getBlockType() == BLOCKTYPE_PANEL)
                  {
                     surfaceGraphicClass = graphicClassPanelOutline;
                  }
               }
            }
            else if ((m_layer->getType() == odbLayerTypeSignal) ||
                     (m_layer->getType() == odbLayerTypePower )     )
            {
               surfaceGraphicClass = graphicClassEtch;
            }
         }

         m_block = parentBlock;
      }
      else  // symbol feature file
      {
         m_block = odbPpDatabase.getCamCadDatabase().getNewBlock("Symbol-" + m_name,"-%d",blockTypePadshape);
         parentBlock = m_block;

         LayerStruct* layer = odbPpDatabase.getCamCadDatabase().getLayer(ccLayerFloat);
         layerIndex = layer->getLayerIndex();
      }

      COdbFeatureBuildState featureBuildState(odbPpDatabase,layerIndex,parentBlock,log);
      featureBuildState.setSurfaceGraphicClass(surfaceGraphicClass);

      for (int ind = 0;ind < m_featureArray.getCount();ind++)
      {
         operationProgress.incrementProgress();

         COdbFeature* feature =  m_featureArray.getAt(ind);

         if (! feature->getInstantiatedFlag())
         {
            feature->instantiateCamCadData(featureBuildState);
            feature->setInstantiatedFlag();
         }
      }
   }

   return m_block;
}

void COdbFeatureFile::loadPads(CQfePadTree& padTree,CWriteFormat& log)
{
   displayProgress(log,"Sorting Pads for '%s'.",(const char*)m_name);
   COperationProgress operationProgress(m_featureArray.getCount());

   for (int ind = 0;ind < m_featureArray.getCount();ind++)
   {
      operationProgress.incrementProgress();

      COdbFeature* feature =  m_featureArray.getAt(ind);

      if (feature->getFeatureType() == padFeature)
      {
         COdbPad* odbPad = (COdbPad*)feature;
         CQfePad* qfePad = new CQfePad(*odbPad,*this);

         padTree.setAt(qfePad);
      }
   }
}

void COdbFeatureFile::loadUninstantiatedPads(CQfePadTree& padTree,CWriteFormat& log)
{
   displayProgress(log,"Sorting uninstantited Pads for '%s'.",(const char*)m_name);
   COperationProgress operationProgress(m_featureArray.getCount());

   for (int ind = 0;ind < m_featureArray.getCount();ind++)
   {
      operationProgress.incrementProgress();

      COdbFeature* feature =  m_featureArray.getAt(ind);

      if (! feature->getInstantiatedFlag() && feature->getFeatureType() == padFeature)
      {
         COdbPad* odbPad = (COdbPad*)feature;
         CQfePad* qfePad = new CQfePad(*odbPad,*this);

         padTree.setAt(qfePad);
      }
   }
}

void COdbFeatureFile::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbFeatureFile: ");
   writeFormat.writef("m_name='%s'\n",
      (const char*)m_name);

   if (depth != 0)
   {
      m_featureArray.dump(writeFormat,depth);
   }

   writeFormat.popHeader();
}

void COdbFeatureFile::convertFreePadsToVias(COdbPpDatabase& odbPpDatabase, COdbStep& step, CQfePadTree& padTree, CWriteFormat& log)
{
   CTMatrix matrix;
   for (int ind = 0;ind < m_featureArray.getCount();ind++)
   {
      COdbFeature* feature =  m_featureArray.getAt(ind);

      if (feature && feature->getFeatureType() == padFeature && m_layer)
      {
         COdbPad* featurePad = (COdbPad*)feature;
         if(featurePad && featurePad->getPadUsage() ==  padUsageUndefined && !featurePad->getInstantiatedFlag())
         {      
            COdbPadstackTemplate padStack;
            bool addedFlag = padStack.add(featurePad, m_layer->getCCZLayerIndex(), matrix, 0., log);
            featurePad->setInstantiatedFlag(addedFlag);

            if(padStack.addCoincidentFreePads(featurePad,padTree,log))
            {
               int layerIndex = 0;
               COdbPadstackGeometry& padstackGeometry = odbPpDatabase.getViastackGeometryFor(padStack,log);
               padstackGeometry.incrementUseCount();

               DataStruct* viaData = odbPpDatabase.getCamCadDatabase().referenceBlock(step.getBlock(),
                  padstackGeometry.getBlock(),insertTypeVia,"",layerIndex,
                  padStack.getOrigin().x,padStack.getOrigin().y);
            }
            

         }//if
      }
   }//for
}

//_____________________________________________________________________________
bool COdbFeatureFileArrayWithMap::readLayerFeatures(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
   CWriteFormat& log)
{
   bool retval = false;

   CString layerName = layer.getName();
   COdbFeatureFile* featureFile = getAt(layerName);

   if (featureFile == NULL)
   {
      featureFile = new COdbFeatureFile(layer,getSize());
      add(layerName,featureFile);
      
      retval = featureFile->readLayerFeatures(odbPpDatabase,step,layer,log);
   }
   else
   {
      log.writef(PrefixDisaster,"COdbFeatureFileArrayWithMap::readLayer() - layer '%s' already read.\n",
         (const char*)layerName);
   }

   return retval;
}

void COdbFeatureFileArrayWithMap::instantiateFeatures(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   for (int ind = 0;ind < getSize();ind++)
   {
      COdbFeatureFile* featureFile = getAt(ind);
      featureFile->instantiateFeatures(odbPpDatabase,step.getBlock(),log);
   }
}

void COdbFeatureFileArrayWithMap::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbFeatureFileArrayWithMap: ");
   writeFormat.writef("getSize()=%d\n",
      getSize());

   if (depth != 0)
   {
      for (int ind = 0;ind < getSize();ind++)
      {
         COdbFeatureFile* featureFile = getAt(ind);
         featureFile->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbStepper::COdbStepper(const CString& stepName,double x,double y,double dx,double dy,
   int nx,int ny,double angle,bool mirrorFlag, bool flipFlag)
{
   m_stepName    = stepName;
   m_x           = x;
   m_y           = y;
   m_dx          = dx;
   m_dy          = dy;
   m_nx          = nx;
   m_ny          = ny;
   m_angle       = angle; 
   m_mirrorFlag  = mirrorFlag;
   m_flipFlag    = flipFlag;
}

int COdbStepper::insertSubSteps(COdbPpDatabase& odbPpDatabase ,COdbStep& step, int pcbStartIndx, CWriteFormat& log)
{
   // Returns number of PCBs inserted for this step-repeat.

   COdbStep* subStep = odbPpDatabase.getStep(m_stepName);
   int pcaCount = 0;

   if (subStep != NULL)
   {
      if (m_nx > 0 && m_ny > 0)
      {
         step.getFile()->setBlockType(blockTypePanel);
      }

      // The purpose of this attribute is probe numbering (probe offset) in vPlan/DFT/TestExport world.
      int pcaSeqIdKW = odbPpDatabase.getCamCadDatabase().registerKeyword("PCA_Sequence_ID", valueTypeInteger);

      for (int xInd = 0;xInd < m_nx;xInd++)
      {
         double x = m_x + (xInd * m_dx);
			x = odbPpDatabase.toPageUnits(x);

         for (int yInd = 0;yInd < m_ny;yInd++)
         {
            double y = m_y + (yInd * m_dy);
				y = odbPpDatabase.toPageUnits(y);

            // Create the DataStruct and InsertStruct
            DataStruct* boardData = odbPpDatabase.getCamCadDatabase().insertBlock(subStep->getBlock(),insertTypePcb,m_stepName,NoLayer);
            if(boardData && boardData->getInsert())
            {
               // Give it a name.
               CString refName;
               refName.Format("%s_%d(%.3f,%.3f)",(const char*)m_stepName, boardData->getEntityNumber(),x,y);
               boardData->getInsert()->setRefname(refName);

               // Add the DataStruct to the paren't datalist.
               step.getBlock()->getDataList().AddTail(boardData);

               // Prepare the matrix that defines this insert location, rotation, etc.
               CTMatrix matrix;
               matrix.translateCtm(x,y);
               matrix.rotateDegreesCtm((m_flipFlag)?360-m_angle:m_angle);
               // matrix.mirrorAboutYAxis(m_mirrorFlag);  DR 821512 - We don't want to do this. We should mirror the PCB itself, but not it's location.
               matrix.translateCtm(-subStep->getDatumPoint());

               // Set up the rest of insert's parameters.
               CBasesVector boardBasesVector;
               boardBasesVector.transform(matrix);
               boardData->getInsert()->setBasesVector(boardBasesVector);
               // And here we have the usual problem in this importer and exporter pair, mangling of mirror.
               // We have a case of mirrored panel...
               // In ODB a PCB instance at some location and flip=true means it is flipped at that spot.
               // As seen from the top of panel, the PCB is flipped.
               // This does not mean it is inserted bottom, but that is the way CAMCAD thinks of it.
               boardData->getInsert()->setPlacedBottom(m_flipFlag);

               // If flipped then mirror both graphics and layers.
               boardData->getInsert()->setMirrorFlags(m_flipFlag?MIRROR_ALL:0);

               // Add PCA Sequence ID attribute. This counter starts at 1.
               ++pcaCount;
               odbPpDatabase.getCamCadDatabase().addAttribute(*boardData->getDefinedAttributes(), pcaSeqIdKW, (pcbStartIndx + pcaCount));

            }
         }
      }
   }
   else
   {
	   log.writef(PrefixError,
		   "Could not find sub step '%s' referenced in stephdr for step '%s'\n",
		   (const char*)m_stepName,(const char*)step.getName());
   }

   return pcaCount;
}

//_____________________________________________________________________________
void COdbStepperArray::insertSubSteps(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   int pcaCount = 0; // Sequence ID that gets assigned as attribute to each PCB that ends up inserted.

   for (int ind = 0;ind < GetSize();ind++)
   {
      COdbStepper* stepper = GetAt(ind);
      pcaCount += stepper->insertSubSteps(odbPpDatabase, step, pcaCount, log);
   }
}

//_____________________________________________________________________________
COdbProperty::COdbProperty(const CString& name,const CString& value,const CString& floatingNumbers)
{
   m_name            = name;
   m_value           = value;
   m_floatingNumbers = floatingNumbers;
}

CString COdbProperty::getStringValue()
{
   CString retval = m_value;

   if (m_floatingNumbers.GetLength() > 0)
   {
      retval += " " + m_floatingNumbers;
   }

   return retval;
}

//_____________________________________________________________________________
void COdbPropertyList::placeAttributes(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase,CWriteFormat& log)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      COdbProperty* property = GetNext(pos);

      CString attributeName = getOdbOptions().getMappedAttributeName(property->getName());
      camCadDatabase.addAttribute(attributeMap,attributeName,
         property->getValueType(),property->getStringValue(),log);
   }
}

//_____________________________________________________________________________
COdbPackagePin::COdbPackagePin(int id,const CString& pinName,PackagePinTypeTag pinType,
   double x,double y,double fhs,
   PackagePinElectricalTypeTag pinElectricalType,PackagePinMountTypeTag pinMountType)
{
   m_id                = id;
   m_pinName           = pinName;
   m_pinType           = pinType;
   m_origin.x          = x;
   m_origin.y          = y;
   m_fhs               = fhs;
   m_pinElectricalType = pinElectricalType;
   m_pinMountType      = pinMountType;

   m_outline         = NULL;
}

COdbPackagePin::~COdbPackagePin()
{
   delete m_outline;
}

COdbCircleOutline* COdbPackagePin::addCircleOutline(double xCenter,double yCenter,double radius)
{
   delete m_outline;

   COdbCircleOutline* outline = new COdbCircleOutline(xCenter,yCenter,radius);
   m_outline = outline;

   return outline;
}

COdbSquareOutline* COdbPackagePin::addSquareOutline(double xCenter,double yCenter,double halfSide)
{
   delete m_outline;

   COdbSquareOutline* outline = new COdbSquareOutline(xCenter,yCenter,halfSide);
   m_outline = outline;

   return outline;
}

COdbRectangleOutline* COdbPackagePin::addRectangleOutline(double lowerLeftX,double lowerLeftY,
   double width,double height)
{
   delete m_outline;

   COdbRectangleOutline* outline = new COdbRectangleOutline(lowerLeftX,lowerLeftY,width,height);
   m_outline = outline;

   return outline;
}

COdbPolygon* COdbPackagePin::addContourOutline(double xStart,double yStart,bool holeFlag)
{
   delete m_outline;

   COdbPolygon* outline = new COdbPolygon(xStart,yStart,holeFlag);
   m_outline = outline;

   return outline;
}

void COdbPackagePin::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbPackagePin: ");
   writeFormat.writef("m_id=%d, m_pinName='%s', m_x=%f, m_y=%f\n",
      m_id,(const char*)m_pinName,
      m_origin.x,m_origin.y
      );

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbPackageGeometry::COdbPackageGeometry(const CString& name,COdbPackage& package,COdbComponent& component) :
   m_name(name), m_package(package), m_component(component)
{
}

void COdbPackageGeometry::buildCamCadData(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   m_package.buildCamCadData( odbPpDatabase, m_block, m_component.getMirrorFlag());
   int drillLayerIndex = step.getDrillLayerIndex();

   for (int pinInd = 0;pinInd < m_component.getNumPins();pinInd++)
   {
      COdbComponentPin* odbComponentPin = m_component.getPin(pinInd);
      COdbPadstackGeometry* padstackGeometry = odbComponentPin->getPadstackGeometry();
      CPoint2d padstackOriginRelativeToPackage = m_component.getPadstackOriginRelativeToPackageForPin(pinInd);

      // Pin rotation in ODB comes relative to PCB, make it relative to component.
      // Not doing a top/bot adjustment, not sure if we need to. No sample in test case yet. //*rcf what about this test?
      double rawPinRot = odbComponentPin->getRotationDegrees();
      double pinRotRelToComp = rawPinRot - this->m_component.getRotation();

      // If bottom reverse direction.
      if (m_component.getMirrorFlag())
         pinRotRelToComp = -pinRotRelToComp;

      pinRotRelToComp = normalizeDegrees(pinRotRelToComp);

      bool padstackMirror = false;

      if (padstackGeometry == NULL)
      {
         padstackGeometry = odbPpDatabase.getDefaultPadstack(log);
      }

      DataStruct* pinData = odbPpDatabase.getCamCadDatabase().referenceBlock(
         m_block,padstackGeometry->getBlock(),insertTypePin, odbComponentPin->getPinReference(),
         NoLayer,padstackOriginRelativeToPackage.x,padstackOriginRelativeToPackage.y,
         DegToRad(pinRotRelToComp),padstackMirror);

      odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(pinData),
         ATT_COMPPINNR,valueTypeInteger, odbComponentPin->getPinNumberString(),log);

      double drillSize = padstackGeometry->getDrillSize(drillLayerIndex);

      if (drillSize == 0.)
      {
         odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(pinData),
            ATT_SMDSHAPE,valueTypeNone,"",log);
      }
   }
}

//_____________________________________________________________________________
int COdbPackage::m_attachPadStackSuccessCount = 0;
int COdbPackage::m_attachPadStackFailureCount = 0;

COdbPackage::COdbPackage(int id,const CString& originalName,const CString& validatedName,double pinPitch,
   double boundingBoxXmin,double boundingBoxYmin,
   double boundingBoxXmax,double boundingBoxYmax)
{
   m_id              = id;
   m_originalName    = originalName;
   m_validatedName   = validatedName;
   m_pinPitch        = pinPitch;
   m_boundingBoxXmin = boundingBoxXmin;
   m_boundingBoxYmin = boundingBoxYmin;
   m_boundingBoxXmax = boundingBoxXmax;
   m_boundingBoxYmax = boundingBoxYmax;

   m_outline         = NULL;
}

COdbPackage::~COdbPackage()
{
   delete m_outline;
}

COdbPackagePin* COdbPackage::getPinAt(int pinIndex)
{
   COdbPackagePin* pin = NULL;

   if (pinIndex >= 0 && pinIndex < m_pins.GetSize())
   {
      pin = m_pins.GetAt(pinIndex);
   }

   return pin;
}

COdbPackagePin* COdbPackage::addPin(const CString& pinName,PackagePinTypeTag pinType,double x,double y,double fhs,
   PackagePinElectricalTypeTag pinElectricalType,PackagePinMountTypeTag pinMountType)
{
   COdbPackagePin* pin = new COdbPackagePin(m_pins.GetSize(),pinName,pinType,x,y,fhs,
                                pinElectricalType,pinMountType);
   m_pins.Add(pin);

   return pin;
}

COdbCircleOutline* COdbPackage::addCircleOutline(double xCenter,double yCenter,double radius)
{
   delete m_outline;

   COdbCircleOutline* outline = new COdbCircleOutline(xCenter,yCenter,radius);
   m_outline = outline;

   return outline;
}

COdbSquareOutline* COdbPackage::addSquareOutline(double xCenter,double yCenter,double halfSide)
{
   delete m_outline;

   COdbSquareOutline* outline = new COdbSquareOutline(xCenter,yCenter,halfSide);
   m_outline = outline;

   return outline;
}

COdbRectangleOutline* COdbPackage::addRectangleOutline(double lowerLeftX,double lowerLeftY,
   double width,double height)
{
   delete m_outline;

   COdbRectangleOutline* outline = new COdbRectangleOutline(lowerLeftX,lowerLeftY,width,height);
   m_outline = outline;

   return outline;
}

COdbPolygon* COdbPackage::addContourOutline(double xStart,double yStart,bool holeFlag)
{
   delete m_outline;

   COdbPolygon* outline = new COdbPolygon(xStart,yStart,holeFlag);
   m_outline = outline;

   return outline;
}

void COdbPackage::addProperty(COdbProperty* property)
{
   m_propertyList.AddTail(property);
}

void COdbPackage::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbPackage: ");
   writeFormat.writef("m_validatedName='%s', m_originalName='%s', m_id=%d, getNumPins()=%d\n",
      (const char*)m_validatedName, (const char*)m_originalName, m_id, getNumPins());

   if (depth != 0)
   {
      for (int ind = 0;ind < m_pins.GetSize();ind++)
      {
         COdbPackagePin* pin = m_pins.GetAt(ind);
         pin->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

COdbPackageGeometry& COdbPackage::getPackageGeometryFor(COdbPpDatabase& odbPpDatabase,
   COdbStep& step,COdbComponent& component,CWriteFormat& log)
{
   COdbPackageGeometry* packageGeometry = NULL;

   for (POSITION pos = m_packageGeometries.GetHeadPosition();pos != NULL;)
   {
      packageGeometry = m_packageGeometries.GetNext(pos);

      if (packageGeometry->getComponent().padStacksMatch(component))
      {
         break;
      }

      packageGeometry = NULL;
   }

   if (packageGeometry == NULL)
   {
      CString packageName,suffix;
      BlockStruct* block = NULL;

      for (int ind=0;;ind++)
      {
         packageName = m_validatedName + suffix;
         block = odbPpDatabase.getCamCadDatabase().getBlock(packageName,step.getFileNum());

         if (block == NULL)
         {
            break;
         }

         suffix.Format("-%d",ind);
      }

      packageGeometry = new COdbPackageGeometry(packageName,*this,component);

      block = odbPpDatabase.getCamCadDatabase().getDefinedBlock(packageName,blockTypePcbComponent,step.getFileNum());
      block->setOriginalName(m_validatedName); // This is to help Geometry Consolidator produce a better result.
                                               // Note that it is correct that we do not use the ODBPackage's originalName value, we
                                               // want the name here to be a match to base block before suffix, which is based on
                                               // ODBPackage's validatedName.
         
      packageGeometry->setBlock(block);
      m_packageGeometries.AddTail(packageGeometry);

      packageGeometry->buildCamCadData(odbPpDatabase,step,log);
   }

   return *packageGeometry;
}

void COdbPackage::buildCamCadData(COdbPpDatabase& odbPpDatabase,BlockStruct* parentBlock, const bool mirrorPackage)
{
   if (m_outline != NULL)
   {
      int componentLayerIndex = odbPpDatabase.getCamCadDatabase().getCamCadLayerIndex(ccLayerAssemblyTop);

      if (mirrorPackage)
         m_outline->mirrorOutline();
      m_outline->buildCamCadData(odbPpDatabase,componentLayerIndex,parentBlock);
   }

   int pinLayerIndex = odbPpDatabase.getCamCadDatabase().getCamCadLayerIndex(ccLayerPinAssemblyTop);

   for (int pinIndex = 0;pinIndex < getNumPins();pinIndex++)
   {
      COdbPackagePin* pin = getPinAt(pinIndex);

      if (pin != NULL)
      {
         COdbOutline* pinOutline = pin->getOutline();

         if (pinOutline != NULL)
         {
            if (mirrorPackage)
               pinOutline->mirrorOutline();
            pinOutline->buildCamCadData(odbPpDatabase,pinLayerIndex,parentBlock);
         }
      }
   }

   // properties?
}

void COdbPackage::fillDftOutlinePoly(COdbPpDatabase& odbPpDatabase, CPoly &dftOutlinePoly)
{
   // Get minimal bounding outline for body and pins.

   Outline_Start(&odbPpDatabase.getCamCadDatabase().getCamCadDoc(), TRUE /*allow union*/);

   if (m_outline != NULL)
   {
      //int componentLayerIndex = odbPpDatabase.getCamCadDatabase().getCamCadLayerIndex(ccLayerAssemblyTop);

      //if (mirrorPackage)
      //   m_outline->mirrorOutline();
      //
      //m_outline->buildCamCadData(odbPpDatabase,componentLayerIndex,parentBlock);

      CPoly bodypoly;
      m_outline->fillOutlinePoly(odbPpDatabase, bodypoly);
      Outline_Add_Poly(&bodypoly, 1.0, 0.0, 0.0, 0.0, FALSE);
   }

   int pinLayerIndex = odbPpDatabase.getCamCadDatabase().getCamCadLayerIndex(ccLayerPinAssemblyTop);

   for (int pinIndex = 0;pinIndex < getNumPins();pinIndex++)
   {
      COdbPackagePin* pin = getPinAt(pinIndex);

      if (pin != NULL)
      {
         COdbOutline* pinOutline = pin->getOutline();

         if (pinOutline != NULL)
         {
            //if (mirrorPackage)
            //   pinOutline->mirrorOutline();
            //pinOutline->buildCamCadData(odbPpDatabase,pinLayerIndex,parentBlock);
            CPoly pinpoly;
            pinOutline->fillOutlinePoly(odbPpDatabase, pinpoly);
            Outline_Add_Poly(&pinpoly, 1.0, 0.0, 0.0, 0.0, FALSE);
         }
      }
   }

   // Now extract our outline
   dftOutlinePoly.empty();
   int returnCode;
   CPntList *pntlist = Outline_GetOutline(&returnCode, SMALLNUMBER);
   if (pntlist != NULL)
   {
      POSITION pos = pntlist->GetHeadPosition();
      while (pos != NULL)
      {
         CPnt *pnt = pntlist->GetNext(pos);
         dftOutlinePoly.addVertex(pnt->x, pnt->y, pnt->bulge);
      }
   }


   // Cleanup
   Outline_FreeResults();
}

void COdbPackage::report(CWriteFormat& writeFormat)
{
   writeFormat.writef("COdbPackage: m_attachPadStackSuccessCount=%d, m_attachPadStackFailureCount=%d\n",
      m_attachPadStackSuccessCount,m_attachPadStackFailureCount);
}

//_____________________________________________________________________________
COdbPackage* COdbPackages::addPackage(const CString& originalName, const CString& validatedName, 
                                      double pinPitch,
                                      double boundingBoxXmin,double boundingBoxYmin,
                                      double boundingBoxXmax,double boundingBoxYmax)
{
   int id = m_packages.GetSize();

   COdbPackage* package = new COdbPackage(id, originalName, validatedName, pinPitch,
      boundingBoxXmin,boundingBoxYmin,
      boundingBoxXmax,boundingBoxYmax);

   m_packages.SetAtGrow(id,package);

   return package;
}

COdbPackage* COdbPackages::getAtOdbIndex(int index)
{
   // Incoming index is as found in ODB++ data references.
   // We split the packages into two collections, one is for eda/data and one for eda/vpl_pkgs.
   // The indexing in ODB++ starts in eda/data and continues on in eda/vpl_pkgs.
   // When we store the vpl_pkgs stuff, we'll be staring at index 0 in the local list.
   // But that will actually be some index n in ODB++.
   // So we keep an offset in the Packages structure, to apply to incoming index for lookup.
   // The m_pkgIndexOffset is essentially the count of packages in eda/data, and should
   // be 0 for the eda/data collection and may be non-zero for eda/vpl_pkgs collection.

   COdbPackage* package = NULL;

   int adjustedIndex = index - this->m_pkgIndexOffset;

   return this->getAtRawIndex(adjustedIndex);
}

COdbPackage* COdbPackages::getAtRawIndex(int index)
{
   COdbPackage* package = NULL;

   if (index >= 0 && index < m_packages.GetSize())
   {
      package = m_packages.GetAt(index);
   }

   return package;
}

void COdbPackages::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   CString header;
   header.Format("COdbPackages [%s]: ", this->m_fileSourceDescription);
   writeFormat.pushHeader(header);
   writeFormat.writef("getNumPackages()=%d\n",getNumPackages());

   if (depth != 0)
   {
      for (int ind = 0;ind < m_packages.GetSize();ind++)
      {
         COdbPackage* package = m_packages.GetAt(ind);
         package->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbFeatureId::COdbFeatureId(FeatureIdTypeTag fidType,int layerNumber,int featureNumber,int lineNumber)
{
   m_fidType       = fidType;
   m_layerNumber   = layerNumber;
   m_featureNumber = featureNumber;
   m_lineNumber    = lineNumber;

   m_feature       = NULL;
}

CString COdbFeatureId::getDescriptor()
{
   CString descriptor;

   descriptor.Format("fidType=%s,layerNumber=%d,featureNumber=%d,lineNumber=%d",
      (const char*)fidTypeTagToString(m_fidType),m_layerNumber,m_featureNumber,m_lineNumber);

   return descriptor;
}

void COdbFeatureId::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbFid: ");
   writeFormat.writef("m_fidType=%d, m_layerNumber=%d, m_featureNumber=%d\n",
      m_fidType,m_layerNumber,m_featureNumber);

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbSubnet::COdbSubnet(int id)
{
   m_id = id;
   m_net = NULL;
}

COdbSubnet::~COdbSubnet()
{
}

CString COdbSubnet::getNetName()
{
   return ((m_net != NULL) ? m_net->getNetName() : "");
}

COdbFeatureId* COdbSubnet::addFeatureId(FeatureIdTypeTag fidType,int layerNumber,int featureNumber,int lineNumber)
{
   COdbFeatureId* fid = new COdbFeatureId(fidType,layerNumber,featureNumber,lineNumber);
   m_fidList.AddTail(fid);

   return fid;
}

CString COdbSubnet::getDescriptor() const
{
   CString descriptor;
   descriptor.Format("subnetId=%d",getId());

   if (m_net != NULL)
   {
      descriptor.AppendFormat(", netId=%d, netName='%s'",
         m_net->getNetId(),m_net->getNetName());
   }

   return descriptor;
}

bool COdbSubnet::linkFeatures(COdbFeatureFileArray& featureFileArray,CWriteFormat& log)
{
   bool retval = true;

   for (POSITION pos = m_fidList.GetHeadPosition();pos != NULL;)
   {
      COdbFeatureId* fid = m_fidList.GetNext(pos);

      int layerNumber = fid->getLayerNumber();

      COdbFeatureFile* featureFile = featureFileArray.GetAt(layerNumber);

      if (featureFile != NULL)
      {
         int featureNumber = fid->getFeatureNumber();
         COdbFeature* feature = featureFile->getFeatureAt(featureNumber);
         fid->setFeature(feature);

         if (feature != NULL)
         {
            COdbSubnet* linkedSubnet = feature->getSubnet();

            if (linkedSubnet == NULL)
            {
               feature->setSubnet(this);
            }
            else if (linkedSubnet != this && linkedSubnet->getNetName().CompareNoCase(getNetName()) != 0)
            {
               retval = false;

               log.writef(PrefixError,
                  "Cannot link Feature{%s} to subnet{%s}; it is already linked to subnet{%s}\n",
                  fid->getDescriptor(),getDescriptor(),linkedSubnet->getDescriptor());
            }
         }
         else
         {
            retval = false;

            if (featureNumber >= featureFile->getFeatureCount())
            {
               log.writef(PrefixError,
                  "Feature Id %d exceeds feature count of %d for layer '%s'; fid{%s}.\n",
                  featureNumber,featureFile->getFeatureCount(),(const char*)featureFile->getName(),
                  (const char*)fid->getDescriptor());
            }
            else
            {
               log.writef(PrefixError,
                  "Could not find feature %d for fid in subnet; fid{%s}.\n",
                  featureNumber,(const char*)fid->getDescriptor());
            }
         }
      }
      else
      {
         retval = false;

         log.writef(PrefixError,
            "Could not find layer %d for fid in subnet; fid{%s}.\n",
            layerNumber,(const char*)fid->getDescriptor());
      }
   }

   return retval;
}

void COdbSubnet::placeAttributes(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase,CWriteFormat& log)
{
   COdbFeatureAttributeArray attributes;

   for (POSITION pos = m_fidList.GetHeadPosition();pos != NULL;)
   {
      COdbFeatureId* fid = m_fidList.GetNext(pos);
      COdbFeature* feature = fid->getFeature();

      if (feature != NULL)
      {
         attributes.mergeFrom(feature->getAttributes());
      }
   }

   attributes.condense();
   attributes.placeAttributes(attributeMap,camCadDatabase,log);
}

void COdbSubnet::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbSubnet: ");
   writeFormat.writef("m_id=%d\n",m_id);

   if (depth != 0)
   {
      for (POSITION pos = m_fidList.GetHeadPosition();pos != NULL;)
      {
         COdbFeatureId* fid = m_fidList.GetNext(pos);
         fid->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbToeprintSubnet::COdbToeprintSubnet(int id,bool topSideFlag,int componentId,int pinId) :
   COdbSubnet(id)
{
   m_topSideFlag     = topSideFlag;
   m_componentId     = componentId;
   m_pinId           = pinId;

   m_odbComponentPin = NULL;
}

bool COdbToeprintSubnet::linkComponentPin(COdbComponentsFile& topComponentsFile,
   COdbComponentsFile& bottomComponentsFile,CWriteFormat& log)
{
   bool retval = true;

   COdbComponentsFile& componentsFile = (m_topSideFlag ? topComponentsFile : bottomComponentsFile);

   COdbComponent* component = componentsFile.getComponent(m_componentId);
   
   if (component != NULL)
   {
      COdbComponentPin* odbComponentPin = component->getPin(m_pinId);

      if (odbComponentPin != NULL)
      {
         m_odbComponentPin = odbComponentPin;
         odbComponentPin->addSubnet(this);
      }
      else
      {
         retval = false;

         log.writef(PrefixError,
            "Could not find pin id %d, refdes='%d', in component file '%s'\n",
            m_pinId,(const char*)component->getRefDes(),(const char*)componentsFile.getName());
      }
   }
   else
   {
      retval = false;

      log.writef(PrefixError,
         "Could not find component id %d in component file '%s'\n",
         m_componentId,(const char*)componentsFile.getName());
   }

   return retval;
}

//_____________________________________________________________________________
COdbViaSubnet::COdbViaSubnet(int id) :
   COdbSubnet(id)
{
}

//_____________________________________________________________________________
COdbTraceSubnet::COdbTraceSubnet(int id) :
   COdbSubnet(id)
{
}

//_____________________________________________________________________________
COdbPlaneSubnet::COdbPlaneSubnet(int id,PlaneFillTypeTag fillType,CutoutTypeTag cutoutType,double fillSize) :
   COdbSubnet(id)
{
   m_fillType   = fillType;
   m_cutoutType = cutoutType;
   m_fillSize   = fillSize;
}

//_____________________________________________________________________________
CString COdbLayerArray::getDescriptor() const
{
   CString descriptor;

   for (int index = 0;index < GetSize();index++)
   {
      COdbLayer* layer = GetAt(index);

      CString layerName = ((layer != NULL) ? layer->getName() : "null");

      descriptor.AppendFormat(" %d:'%s'",index,layerName);
   }

   return descriptor;
}

//_____________________________________________________________________________
COdbNet::COdbNet(int netId,const CString& netName)
{
   m_netId   = netId;
   m_netName = netName;

   m_subnetArray.setGrowBySize(100);
   m_subnetArrayGrowLimit = 100;
   m_padstackList = NULL;
}

COdbNet::~COdbNet()
{
   delete m_padstackList;
}

void COdbNet::addSubnet(COdbSubnet* subnet)
{
   int size = m_subnetArray.GetSize();

   if (size >= m_subnetArrayGrowLimit)
   {
      int newGrowBySize = m_subnetArray.getGrowBySize() * 10;

      m_subnetArray.SetSize(size,newGrowBySize);
      m_subnetArrayGrowLimit = size + newGrowBySize; 
   }

   m_subnetArray.Add(subnet);
}

COdbToeprintSubnet* COdbNet::addToeprintSubnet(bool topSideFlag,int componentId,int pinId)
{
   CString subnetName;
   subnetName.Format("SNT_TOP-%d-%d-%d",componentId,pinId,topSideFlag);

   //Find possible existing subnet.
   int subnetId = -1;
   COdbToeprintSubnet* subnet = NULL;
   if(m_subnetArray.getSubnetIDMap().Lookup(subnetName, subnetId) && subnetId > -1)
      subnet = (COdbToeprintSubnet*)m_subnetArray.GetAt(subnetId);
   
   // Not found, create it.
   if(!subnet)
   {
      int id = m_subnetArray.GetSize();
      subnet = new COdbToeprintSubnet(id,topSideFlag,componentId,pinId);
      addSubnet(subnet);
      m_subnetArray.getSubnetIDMap().SetAt(subnetName,id);
   }

   return subnet;
}

COdbViaSubnet* COdbNet::addViaSubnet()
{
   int id = m_subnetArray.GetSize();
   COdbViaSubnet* subnet = new COdbViaSubnet(id);
   addSubnet(subnet);

   return subnet;
}

COdbTraceSubnet* COdbNet::addTraceSubnet()
{
   int id = m_subnetArray.GetSize();
   COdbTraceSubnet* subnet = new COdbTraceSubnet(id);
   addSubnet(subnet);

   return subnet;
}

COdbPlaneSubnet* COdbNet::addPlaneSubnet(PlaneFillTypeTag planeFillType,CutoutTypeTag cutoutType,double fillSize)
{
   int id = m_subnetArray.GetSize();
   COdbPlaneSubnet* subnet = new COdbPlaneSubnet(id,planeFillType,cutoutType,fillSize);
   addSubnet(subnet);

   return subnet;
}

bool COdbNet::linkFeatures(COdbFeatureFileArray& featureFileArray,CWriteFormat& log,COperationProgress& operationProgress)
{
   bool retval = true;

   for (int ind = 0;ind < m_subnetArray.GetSize();ind++)
   {
      operationProgress.incrementProgress();

      COdbSubnet* subnet = m_subnetArray.GetAt(ind);
      subnet->setNet(this);

      retval = (subnet->linkFeatures(featureFileArray,log) && retval);
   }

   return retval;
}

bool COdbNet::linkComponentPins(COdbComponentsFile& topComponentsFile,
   COdbComponentsFile& bottomComponentsFile,CWriteFormat& log)
{
   bool retval = true;

   for (int ind = 0;ind < m_subnetArray.GetSize();ind++)
   {
      COdbSubnet* subnet = m_subnetArray.GetAt(ind);

      if (subnet->getSubnetType() == subnetToeprint)
      {
         COdbToeprintSubnet* toeprintSubnet = (COdbToeprintSubnet*)subnet;
         retval = (toeprintSubnet->linkComponentPin(topComponentsFile,bottomComponentsFile,log) && retval);
      }
   }

   return retval;
}

bool COdbNet::buildCamCadData(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   bool retval = true;

   if (m_subnetArray.GetSize() == 1 && m_netName.Left(6).CompareNoCase("$NONE$") == 0)
   {
      m_netName = NET_UNUSED_PINS;
   }

   NetStruct* net = odbPpDatabase.getCamCadDatabase().getDefinedNet(m_netName,step.getFile());
	if (m_netName == NET_UNUSED_PINS)
	{
		net->setFlagBits(NETFLAG_UNUSEDNET);
	}

   for (int ind = 0;ind < m_subnetArray.GetSize();ind++)
   {
      COdbSubnet* subnet = m_subnetArray.GetAt(ind);

      if (subnet->getSubnetType() == subnetToeprint)
      {
         COdbToeprintSubnet* toeprintSubnet = (COdbToeprintSubnet*)subnet;
         COdbComponentPin* odbComponentPin = toeprintSubnet->getComponentPin();

         if (odbComponentPin != NULL)
         {
            CompPinStruct* pinData = odbPpDatabase.getCamCadDatabase().getDefinedCompPin(net,
                                        odbComponentPin->getComponent().getRefDes(),odbComponentPin->getPinReference());

            toeprintSubnet->placeAttributes(getAttributeMap(pinData),odbPpDatabase.getCamCadDatabase(),log);
         }
         else
         {
            retval = false;

            log.writef(PrefixError,
               "COdbNet::buildCamCadData() - no pin for toeprint subnet on net '%s'.\n",
               (const char*)m_netName);
         }
      }
   }

   return retval;
}

void COdbNet::addViaPadstack(COdbPadstackTemplate* padstack)
{
   if (m_padstackList == NULL)
   {
      m_padstackList = new COdbPadstackTemplateListContainer();
   }

   m_padstackList->AddTail(padstack);
}

void COdbNet::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbNet: ");
   writeFormat.writef("m_netId=%d, m_netName='%s'\n",m_netId,(const char*)m_netName);

   if (depth != 0)
   {
      for (int ind = 0;ind < m_subnetArray.GetSize();ind++)
      {
         COdbSubnet* subnet = m_subnetArray.GetAt(ind);
         subnet->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbNetArray::COdbNetArray()
{
   m_subnetCount = -1;
}

int COdbNetArray::getSubnetCount()
{
   if (m_subnetCount <= 0)
   {
      m_subnetCount = 0;

      for (int ind = 0;ind < GetSize();ind++)
      {
         COdbNet* net = GetAt(ind);
         m_subnetCount += net->getSubnetArray().GetCount();
      }
   }

   return m_subnetCount;
}

COdbNet* COdbNetArray::addNet(const CString& netName)
{
   COdbNet* net = new COdbNet(GetSize(),netName);
   Add(net);

   return net;
}

bool COdbNetArray::linkFeatures(COdbFeatureFileArrayWithMap& featureFileArray,COdbLayerArray& layerArray,
   CWriteFormat& log)
{
   bool retval = true;

   //debug
   //CString layerArrayDescriptor = layerArray.getDescriptor();

   displayProgress(log,"Linking nets to features.");
   COperationProgress operationProgress(layerArray.GetSize());

   COdbFeatureFileArray featureFileArrayByEdaIndex;
   featureFileArrayByEdaIndex.SetSize(layerArray.GetSize());

   for (int ind = 0;ind < layerArray.GetSize();ind++)
   {
      operationProgress.incrementProgress();

      COdbLayer* layer = layerArray.GetAt(ind);

      if (layer == NULL)
      {
         log.writef(PrefixDisaster,
            "Could not find layer for index=%d\n",
            ind);
         continue;
      }

      CString layerName = layer->getName();

      if (layer->getType() == odbLayerTypeComponent)
      {
         log.writef(PrefixWarning,
            "Skipping unexpected component layer '%s' in layer list.\n",
            layerName);

         continue;
      }

      COdbFeatureFile* featureFile = featureFileArray.getAt(layerName);
      featureFileArrayByEdaIndex.SetAt(ind,featureFile);

      if (featureFile == NULL)
      {
         log.writef(PrefixDisaster,
            "Could not find feature file for layer '%s'\n",
            layerName);

         retval = false;
      }
   }

   if (retval)
   {
      int subnetCount = 0;

      for (int ind = 0;ind < GetSize();ind++)
      {
         COdbNet* net = GetAt(ind);
         subnetCount += net->getSubnetArray().GetSize();
      }

      COperationProgress operationProgress(subnetCount);

      for (int ind = 0;ind < GetSize();ind++)
      {
         COdbNet* net = GetAt(ind);
         retval = (net->linkFeatures(featureFileArrayByEdaIndex,log,operationProgress) && retval);
      }
   }

   return retval;
}

bool COdbNetArray::linkComponentPins(COdbComponentsFile& topComponentsFile,
   COdbComponentsFile& bottomComponentsFile,CWriteFormat& log)
{
   bool retval = true;

   displayProgress(log,"Linking nets to component pins.");
   COperationProgress operationProgress(GetSize());

   for (int ind = 0;ind < GetSize();ind++)
   {
      operationProgress.incrementProgress();

      COdbNet* net = GetAt(ind);
      retval = (net->linkComponentPins(topComponentsFile,bottomComponentsFile,log) && retval);
   }

   return retval;
}

bool COdbNetArray::instantiateVias(COdbPpDatabase& odbPpDatabase,COdbStep& step,
   CQfePadTree& padTree,CWriteFormat& log)
{
   bool retval = true;

   displayProgress(log,"Sorting vias.");
   int subnetCount = 0;

   COperationProgress operationProgress(getSubnetCount());
   CTMatrix matrix;
   CExtent searchExtent;
   const double searchRadius = .00001;
   bool viastackQuestFlag = getOdbOptions().getOptionFlag(optionEnableViastackQuest);

   for (int ind = 0;ind < GetSize();ind++)
   {
      COdbNet* net = GetAt(ind);

      for (int subnetIndex = 0;subnetIndex < net->getSubnetArray().GetCount();subnetIndex++)
      {
         operationProgress.incrementProgress();

         COdbSubnet* subnet = net->getSubnetArray().GetAt(subnetIndex);

         if (subnet->getSubnetType() == subnetVia)
         {
            COdbPadstackTemplate* padStack = NULL;

            for (POSITION pos = subnet->getFidList().GetHeadPosition();pos != NULL;)
            {
               COdbFeatureId* fid = subnet->getFidList().GetNext(pos);
               COdbFeature* feature = fid->getFeature();

               if (feature != NULL && feature->getFeatureType() == padFeature)
               {
                  int layerNumber = fid->getLayerNumber();
                  COdbPad* featurePad = (COdbPad*)feature;
                  COdbLayer* layer = step.getLayer(layerNumber);
                  int layerIndex = layer->getCCZLayerIndex();

                  if (padStack == NULL)
                  {
                     padStack = new COdbPadstackTemplate();
                  }

                  bool addedFlag = padStack->add(featurePad,layerIndex,matrix,0.,log);

                  if (addedFlag)
                  {
                     featurePad->setInstantiatedFlag();
                  }
                  else
                  {
                     // log.writef(PrefixError,
                     //   "Multiple Pad feature IDs found on layer %d for via id=%d, net id=%d, pad usage code=%d\n",
                     //   layerNumber,subnet->getId(),net->getNetId(), featurePad->getPadUsage() );
                  }
               }
            }

            if (padStack != NULL)
            {
               padStack->centerPads();
               net->addViaPadstack(padStack);

               if (viastackQuestFlag)
               {
                  CTMatrix boardToPadMatrix;
                  boardToPadMatrix.translate(-(padStack->getOrigin().x),-(padStack->getOrigin().y));

                  searchExtent.set(padStack->getOrigin().x - searchRadius,
                                   padStack->getOrigin().y - searchRadius,
                                   padStack->getOrigin().x + searchRadius,
                                   padStack->getOrigin().y + searchRadius);

                  CQfePadList foundList;
                  int numFound = padTree.search(searchExtent,foundList);

                  for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
                  {
                     CQfePad* qfePad = foundList.GetNext(foundPos);
                     COdbPad& odbPad = qfePad->getPad();

                     if (! odbPad.getInstantiatedFlag() && odbPad.getSubnet() == NULL)
                     {
                        COdbFeatureFile& featureFile = qfePad->getFeatureFile();
                        COdbLayer* layer = featureFile.getLayer();

                        int layerIndex = layer->getCCZLayerIndex();

                        bool addedFlag = padStack->add(&odbPad,layerIndex,boardToPadMatrix,0.,log);

                        if (addedFlag)
                        {
                           odbPad.setInstantiatedFlag();
                        }
                        // else
                        // {
                        //log.writef(PrefixWarning,
                        //"Multiple unconnected pads found on layer number %d for component '%s', pinNumber %d\n",
                        //layerIndex,(const char*)component->getRefDes(),componentPin->getPinNumber() );
                        // }
                     }
                  }
               }
            }
         }
      }
   }

   displayProgress(log,"Instantiating vias.");
   operationProgress.updateProgress(0.);

   for (int ind = 0;ind < GetSize();ind++)
   {
      operationProgress.incrementProgress();

      COdbNet* net = GetAt(ind);
      net->instantiateVias(odbPpDatabase,step,operationProgress,log);
   }

   return retval;
}

bool COdbNet::instantiateVias(COdbPpDatabase& odbPpDatabase,COdbStep& step,
   COperationProgress& operationProgress,CWriteFormat& log)
{
   bool retval = true;

   if (m_padstackList != NULL)
   {
      CQfePadstackTemplates qfePadstackTree;

      // load the tree
      for (POSITION pos = m_padstackList->GetHeadPosition();pos != NULL;)
      {
         COdbPadstackTemplate* padstack = m_padstackList->GetNext(pos);
         CQfePadstackTemplate* qfePadstack = new CQfePadstackTemplate(padstack);
         qfePadstackTree.setAt(qfePadstack);
      }

      CExtent searchExtent;
      const double searchRadius = .00001;

      // query the tree and coalesce padstacks
      for (POSITION pos = m_padstackList->GetHeadPosition();pos != NULL;)
      {
         operationProgress.incrementProgress();

         COdbPadstackTemplate* padstack = m_padstackList->GetNext(pos);

         if (padstack->getCount() == 0)
         {
            continue;
         }

         searchExtent.set(padstack->getOrigin().x - searchRadius,
                          padstack->getOrigin().y - searchRadius,
                          padstack->getOrigin().x + searchRadius,
                          padstack->getOrigin().y + searchRadius);

         CQfePadstackTemplateList foundList;
         int numFound = qfePadstackTree.search(searchExtent,foundList);

         for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
         {
            CQfePadstackTemplate* qfePadstack = foundList.GetNext(foundPos);
            COdbPadstackTemplate* otherPadstack = qfePadstack->getPadstack();

            if (otherPadstack == padstack)
            {
               continue;
            }

            padstack->conditionallyTakePads(*otherPadstack,log);
         }
      }

      // instantiate the vias
      for (POSITION pos = m_padstackList->GetHeadPosition();pos != NULL;)
      {
         COdbPadstackTemplate* padstack = m_padstackList->GetNext(pos);

         if (padstack->getCount() == 0)
         {
            continue;
         }

         int layerIndex = 0;

         COdbPadstackGeometry& padstackGeometry = odbPpDatabase.getViastackGeometryFor(*padstack,log);
         padstackGeometry.incrementUseCount();

         DataStruct* viaData = odbPpDatabase.getCamCadDatabase().referenceBlock(step.getBlock(),
            padstackGeometry.getBlock(),insertTypeVia,"",layerIndex,
            padstack->getOrigin().x,padstack->getOrigin().y);

         odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(viaData),
            ATT_NETNAME,valueTypeString,getNetName(),log);
         padstack->getAttributes().placeAttributes(getAttributeMap(viaData),odbPpDatabase.getCamCadDatabase(),log);
      }
   }

   return retval;
}

bool COdbNetArray::buildCamCadData(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   bool retval = true;

   displayProgress(log,"Building nets.");
   COperationProgress operationProgress(getSubnetCount());

   for (int ind = 0;ind < GetSize();ind++)
   {
      operationProgress.incrementProgress();

      COdbNet* net = GetAt(ind);
      retval = (net->buildCamCadData(odbPpDatabase,step,log) && retval);
   }

   return retval;
}

//_____________________________________________________________________________
COdbComponentPin::COdbComponentPin(COdbComponent& component,int pinId,int pinNumber,
   double x,double y,double rotation,
   bool mirrorFlag,int netNumber,int subnetNumber,const CString& pinName) :
   m_component(component)
{
   m_pinId        = pinId;
   m_pinNumber    = pinNumber;
   m_origin.x     = x;
   m_origin.y     = y;
   m_rotation     = normalizeDegrees(rotation);
   m_mirrorFlag   = mirrorFlag;
   m_netNumber    = netNumber;
   m_subnetNumber = subnetNumber;
   m_pinName      = pinName;

   m_subnet           = NULL;
   m_padstackGeometry = NULL;
   m_padstackOffsetFromPackagePin.x = 0.;
   m_padstackOffsetFromPackagePin.y = 0.;
}

COdbPad* COdbComponentPin::getFirstSubnetPadForLayerNumber(int layerNumber)
{
   COdbPad* pad = NULL;

   if (m_subnet != NULL)
   {
      COdbFidList& fidList = m_subnet->getFidList();

      for (POSITION pos = fidList.GetHeadPosition();pos != NULL;)
      {
         COdbFeatureId* fid = fidList.GetNext(pos);

         if (fid->getLayerNumber() == layerNumber)
         {
            COdbFeature* feature = fid->getFeature();

            if (feature->getFeatureType() == padFeature)
            {
               pad = (COdbPad*)feature;
               break;
            }
         }
      }
   }

   return pad;
}

void COdbComponentPin::setPadstackGeometry(COdbPadstackGeometry* padstackGeometry,CPoint2d stackOffset)
{
   m_padstackGeometry               = padstackGeometry;
   m_padstackOffsetFromPackagePin   = stackOffset;
   padstackGeometry->incrementUseCount();
}

bool COdbComponentPin::padStacksMatch(const COdbComponentPin& other)
{
   bool retval = false;

   if (m_padstackGeometry == other.m_padstackGeometry)
   {
      bool sameX = fpeq3(m_padstackOffsetFromPackagePin.x,other.m_padstackOffsetFromPackagePin.x);
      bool sameY = fpeq3(m_padstackOffsetFromPackagePin.y,other.m_padstackOffsetFromPackagePin.y);
      retval = sameX && sameY;
   }

   return retval;
}

void COdbComponentPin::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbComponentPin: ");
   writeFormat.writef(
      "m_pinId=%d, m_pinNumber=%d, m_origin=(%.3f,%.3f), m_rotation=%.3f, "
      "m_mirrorFlag=%d, m_netNumber=%d, m_subnetNumber=%d, m_pinName='%s', m_padstackOffsetFromPackagePin=(%.3f,%.3f)\n",
      m_pinId,m_pinNumber,m_origin.x,m_origin.y,m_rotation,
      m_mirrorFlag,m_netNumber,m_subnetNumber,(const char*)m_pinName,
      m_padstackOffsetFromPackagePin.x,m_padstackOffsetFromPackagePin.y);

   if (depth != 0)
   {
      if (m_padstackGeometry != NULL)
      {
         m_padstackGeometry->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbComponentPin* COdbComponentPinArray::getAt(int index)
{
   COdbComponentPin* odbComponentPin = NULL;

   if (index >= 0 && index < GetSize())
   {
      odbComponentPin = GetAt(index);
   }

   return odbComponentPin;
}

void COdbComponentPinArray::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbComponentPinArray: ");
   writeFormat.writef("GetSize()=%d\n",GetSize());

   if (depth != 0)
   {
      for (int ind = 0;ind < GetSize();ind++)
      {
         COdbComponentPin* odbComponentPin = GetAt(ind);
         odbComponentPin->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbComponent::COdbComponent(int componentId,int compsPackageId,double x,double y,double rotation,bool topFlag,bool mirrorFlag,
      const CString& refDes,const CString& partNumber)
{
   m_componentId = componentId;
   m_compsPackageId   = compsPackageId;
   m_comps3PackageId  = -1;
   m_origin.x    = x;
   m_origin.y    = y;
   m_rotation    = normalizeDegrees(rotation);
   m_topFlag     = topFlag;
   m_mirrorFlag  = mirrorFlag;
   m_refDes      = refDes;
   m_partNumber  = partNumber;

   m_package                    = NULL;
   m_descriptions               = NULL;
   m_vplVendorNames             = NULL;
   m_vplManufacturerPartNumbers = NULL;
   m_vendorNames                = NULL;
   m_manufacturerPartNumbers    = NULL;
   m_compInsertData             = NULL;
}

COdbComponent::~COdbComponent()
{
   delete m_descriptions;
   delete m_vplVendorNames;
   delete m_vplManufacturerPartNumbers;
   delete m_vendorNames;
   delete m_manufacturerPartNumbers;
}

void COdbComponent::addPin(int pinNumber,double x,double y,double rotation,bool mirrorFlag,
      int netNumber,int subnetNumber,const CString& pinName)
{
   COdbComponentPin* pin = new COdbComponentPin(*this,m_pinArray.GetSize(),pinNumber,
                              x,y,rotation,mirrorFlag,netNumber,subnetNumber,pinName);
   m_pinArray.Add(pin);
}

void COdbComponent::addProperty(COdbProperty* property)
{
   m_propertyList.AddTail(property);
}

void COdbComponent::addDescription(const CString& description)
{
   if (description.GetLength() > 0)
   {
      if (m_descriptions == NULL)
      {
         m_descriptions = new CStringArray();
      }

      m_descriptions->Add(description);
   }
}

void COdbComponent::addVplVendorName(const CString& vplVendorName)
{
   if (vplVendorName.GetLength() > 0)
   {
      if (m_vplVendorNames == NULL)
      {
         m_vplVendorNames = new CStringArray();
      }

      m_vplVendorNames->Add(vplVendorName);
   }
}

void COdbComponent::addVplManufacturerPartNumber(const CString& vplManufacturerPartNumber)
{
   if (vplManufacturerPartNumber.GetLength() > 0)
   {
      if (m_vplManufacturerPartNumbers == NULL)
      {
         m_vplManufacturerPartNumbers = new CStringArray();
      }

      m_vplManufacturerPartNumbers->Add(vplManufacturerPartNumber);
   }
}

void COdbComponent::addVendorName(const CString& vendorName)
{
   if (vendorName.GetLength() > 0)
   {
      if (m_vendorNames == NULL)
      {
         m_vendorNames = new CStringArray();
      }

      m_vendorNames->Add(vendorName);
   }
}

void COdbComponent::addManufacturerPartNumber(const CString& manufacturerPartNumber)
{
   if (manufacturerPartNumber.GetLength() > 0)
   {
      if (m_manufacturerPartNumbers == NULL)
      {
         m_manufacturerPartNumbers = new CStringArray();
      }

      m_manufacturerPartNumbers->Add(manufacturerPartNumber);
   }
}

void COdbComponent::placeAttributes(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase,CWriteFormat& log,
   CStringArray* values,const CString& namePrefix)
{
   if (values != NULL)
   {
      for (int ind = 0;ind < values->GetSize();ind++)
      {
         CString value = values->GetAt(ind);

         if (value.GetLength() > 0)
         {
            CString indexString;

            if (ind > 0)
            {
               indexString.Format("%d",ind);
            }

            CString attributeName = getOdbOptions().getMappedAttributeName(namePrefix + indexString);

            camCadDatabase.addAttribute(attributeMap,attributeName,valueTypeString,value,log);
         }
      }
   }
}

void COdbComponent::placeAttributes(CAttributes** attributeMap,
   CCamCadDatabase& camCadDatabase, CWriteFormat& log)
{
   m_attributes.placeAttributes(attributeMap,camCadDatabase,log);
   m_propertyList.placeAttributes(attributeMap,camCadDatabase,log);

   if (m_customerPartNumber.GetLength() > 0)
   {
      CString attributeName = getOdbOptions().getMappedAttributeName("CustomerPartNumber");

      camCadDatabase.addAttribute(attributeMap,attributeName,valueTypeString,m_customerPartNumber,log);
   }

   if (m_internalPartNumber.GetLength() > 0)
   {
      CString attributeName = getOdbOptions().getMappedAttributeName("InternalPartNumber");

      camCadDatabase.addAttribute(attributeMap,attributeName,valueTypeString,m_internalPartNumber,log);
   }

   if (m_packageName.GetLength() > 0)
   {
      CString attributeName = getOdbOptions().getMappedAttributeName("PackageName");

      camCadDatabase.addAttribute(attributeMap,attributeName,valueTypeString,m_packageName,log);
   }

   // These descriptions are from the components, components2, and especially the components3 file.
   // They are not "recognized", they are just transcribed. If there is a BOM then the same values
   // will likely appear in BOM, and get applied. The question occasionally comes up "why are we putting
   // the BOM values in these dsecription1, description2, etc attributes?". Answer is, we are NOT !!!
   // These are from components file and get saved as-is. Perhaps we should stop that?
   placeAttributes(attributeMap,camCadDatabase,log,m_descriptions              ,"Description");

   placeAttributes(attributeMap,camCadDatabase,log,m_vplVendorNames            ,"VplVendorName");
   placeAttributes(attributeMap,camCadDatabase,log,m_vplManufacturerPartNumbers,"VplManufacturerPartNumber");
   placeAttributes(attributeMap,camCadDatabase,log,m_vendorNames               ,"VendorName");
   placeAttributes(attributeMap,camCadDatabase,log,m_manufacturerPartNumbers   ,"ManufacturerPartNumber");

   bool loadedFlag = true;

   for (int index = 0;index < m_attributes.GetSize();index++)
   {
      COdbFeatureAttribute* featureAttribute = m_attributes.GetAt(index);

      if (featureAttribute->getName().CompareNoCase(".comp_ignore") == 0)
      {
         loadedFlag = false;
         break;
      }
   }

   camCadDatabase.addAttribute(attributeMap,ATT_LOADED,valueTypeString,(loadedFlag ? "TRUE" : "FALSE"),log);
}

bool COdbComponent::padStacksMatch(const COdbComponent& other)
{
   if (m_pinArray.GetSize() == other.m_pinArray.GetSize())
   {
      for (int pinInd = 0; pinInd < m_pinArray.GetSize(); pinInd++)
      {
         COdbComponentPin* thisPin = m_pinArray.GetAt(pinInd);
         COdbComponentPin* otherPin = other.m_pinArray.GetAt(pinInd);

         if (thisPin->getPinName() != otherPin->getPinName() || ! thisPin->padStacksMatch(*otherPin))
         {
            return false;  // Different pin name or different pin padstacks (geom or location)
         }
      }

      // If we get here then no pin differences were found
      return true;
   }

   return false; // Different pin counts
}

CPoint2d COdbComponent::getPadstackOriginRelativeToPackageForPin(int pinId)  
{
   COdbComponentPin* odbComponentPin = getPin(pinId);
   COdbPackagePin* packagePin = m_package->getPinAt(pinId);

   CPoint2d padstackOriginRelativeToPackage = (packagePin      != NULL ? packagePin->getOrigin()                            : CPoint2d(0,0)) + 
                                              (odbComponentPin != NULL ? odbComponentPin->getPadstackOffsetFromPackagePin() : CPoint2d(0,0));

   return padstackOriginRelativeToPackage;
}

bool COdbComponent::inferComponentPinPositionFromFeatures(int pinId,CPoint2d& pinPosition)
{
   bool retval = false;
   CNullWriteFormat log;

   while (true)
   {
      bool mirrorLayerFlag = false;

      COdbComponentPin* odbComponentPin = getPin(pinId);

      if (odbComponentPin == NULL)
      {
         break;
      }

      COdbToeprintSubnet* subnet = odbComponentPin->getSubnet();

      if (subnet == NULL)
      {
         break;
      }

      COdbPackage* package = getPackage();

      if (package == NULL)
      {
         break;
      }

      COdbPackagePin* packagePin = package->getPinAt(pinId);

      if (packagePin == NULL)
      {
         break;
      }

      COdbPadstackTemplate padStack;

      // used to convert board coordinates to coordinates relative to component
      CTMatrix matrix;
      matrix.rotateDegrees(getRotation());
      matrix.scale(m_topFlag ? 1. : -1.,1.);
      matrix.translate(getOrigin());
      matrix.invert();

      int padCount = 0;

      for (POSITION pos = subnet->getFidList().GetHeadPosition();pos != NULL;)
      {
         COdbFeatureId* fid = subnet->getFidList().GetNext(pos);
         COdbFeature* feature = fid->getFeature();

         if (feature != NULL && feature->getFeatureType() == padFeature)
         {
            int layerNumber = fid->getLayerNumber();
            COdbPad* featurePad = (COdbPad*)feature;

            bool addedFlag = padStack.add(featurePad,padCount++,matrix,getRotation(),log);
         }
      }

      if (padCount > 0)
      {
         pinPosition = padStack.centerPads();

         retval = true;
      }

      break;
   }

   return retval;
}

void COdbComponent::generatePackagePinReport(COdbStep& step,CWriteFormat& report)
{
	report.writef(
		"-----------------------------------------------------------------\n"
		"Package Pin Report for component refdes='%s', origin(%9.3f,%9.3f), rotation=%.3f, topFlag=%d, mirrorFlag=%d\n",
		(const char*)m_refDes,m_origin.x,m_origin.y,m_rotation,m_topFlag,m_mirrorFlag);

	if (m_package == NULL)
	{
		report.writef(PrefixError,
			"No package for component '%s'\n",
			(const char*)getRefDes());
	}
	else
	{
		if (m_package->getNumPins() != getNumPins())
		{
			report.writef(PrefixWarning,
				"Pincount mismatch, component=%d, package=%d\n",
				getNumPins(),m_package->getNumPins());
		}

      int pinCount = min(m_package->getNumPins(),getNumPins());

      double loSum=0.,hiSum=0.,sumOfLoSquared=0.,sumOfHiSquared=0.;
      int sampleCount = 0;
      int surfaceSignalLayerNumber = step.getLayerNumberWithType(m_topFlag ? layerTypeSignalTop : layerTypeSignalBottom);

      for (int pinId = 0;pinId < pinCount;pinId++)
      {
         COdbComponentPin* odbComponentPin = getPin(pinId);
         COdbPackagePin* packagePin = m_package->getPinAt(pinId);
         CPoint2d componentPinOrigin = odbComponentPin->getOrigin();
         CPoint2d inferredComponentPinOrigin;

         if (odbComponentPin != NULL && packagePin != NULL)
         {
            if (! inferComponentPinPositionFromFeatures(pinId,inferredComponentPinOrigin))
            {
               report.writef(PrefixError,
                  "Could not infer pin position for pinId %d\n",
                  pinId);
            }
            else
            {  // http://research3.tamu-commerce.edu/crrobinson/517/sdcalc.htm
               CPoint2d packagePinOrigin = packagePin->getOrigin();

               double dx = inferredComponentPinOrigin.x - packagePinOrigin.x;
               double dy = inferredComponentPinOrigin.y - packagePinOrigin.y;
               double loDelta = min(fabs(dx),fabs(dy));
               double hiDelta = max(fabs(dx),fabs(dy));

               loSum += loDelta;
               hiSum += hiDelta;
               sumOfLoSquared += loDelta * loDelta;
               sumOfHiSquared += hiDelta * hiDelta;

               sampleCount++;

               CPoint2d psOrigin = getPadstackOriginRelativeToPackageForPin(pinId);

               CString padInfo;
               COdbPad* surfacePad = odbComponentPin->getFirstSubnetPadForLayerNumber(surfaceSignalLayerNumber);

               if (surfacePad != NULL)
               {
                  padInfo.Format("surface pad (%9.3f,%9.3f) '%s'",
                     surfacePad->getOrigin().x,surfacePad->getOrigin().y,
                     (const char*)surfacePad->getFeatureSymbol()->getName());
               }
               else
               {
                  padInfo = "{No surface pad}";
               }

               report.writef(
                  "pinId=%3d, inferred(%9.3f,%9.3f), component(%9.3f,%9.3f), package(%9.3f,%9.3f), psOffset(%9.3f,%9.3f), psOriginRelToPkg(%9.3f,%9.3f), loDelta=%9.3f, hiDelta=%9.3f; %s\n",
                  pinId,
                  inferredComponentPinOrigin.x,inferredComponentPinOrigin.y,
                  componentPinOrigin.x,componentPinOrigin.y,
                  packagePinOrigin.x,packagePinOrigin.y,
                  odbComponentPin->getPadstackOffsetFromPackagePin().x,odbComponentPin->getPadstackOffsetFromPackagePin().y,
                  psOrigin.x,psOrigin.y,
                  loDelta,hiDelta,
                  (const char*)padInfo);
            }
         }
         else
         {
            report.writef("pinId=%3d",pinId);

            if (odbComponentPin == NULL)
            {
               report.writef(", missing component pin");
            }

            if (packagePin == NULL)
            {
               report.writef(", missing package pin");
            }

            report.writef("\n");
         }
      }

      if (sampleCount > 0)
      {
         double loMean = loSum / sampleCount;
         double standardDeviationLo = sqrt((sumOfLoSquared / sampleCount) - (loMean * loMean));
         double hiMean = hiSum / sampleCount;
         double standardDeviationHi = sqrt((sumOfHiSquared / sampleCount) - (hiMean * hiMean));

         report.writef(
            "\nStandardDeviationLo=%9.3f, standardDeviationHi=%9.3f\n",
            standardDeviationLo,standardDeviationHi);
      }
   }
}

void COdbComponent::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbComponent: ");
   writeFormat.writef(
      "m_componentId=%d, m_compsPackageId=%d, m_comps3PackageId=%d, m_origin=(%.3f,%.3f), m_rotation=%.3f,m_mirrorFlag=%d, m_refDes='%s', m_partNumber='%s'\n",
      m_componentId,m_compsPackageId,m_comps3PackageId,m_origin.x,m_origin.y,m_rotation,m_mirrorFlag,(const char*)m_refDes,(const char*)m_partNumber);

   if (depth != 0)
   {
      m_pinArray.dump(writeFormat,depth);
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbComponents::COdbComponents()
{
}

COdbComponents::~COdbComponents()
{
}

COdbComponent* COdbComponents::addComponent(int packageId,double x,double y,double rotation,bool topFlag,bool mirrorFlag,
      const CString& refDes,const CString& partNumber)
{
   int componentId = getSize();
   COdbComponent* component = new COdbComponent(componentId,packageId,x,y,rotation,topFlag,mirrorFlag,refDes,partNumber);

   add(refDes,component);

   return component;
}

void COdbComponents::generatePackagePinReport(COdbStep& step,CWriteFormat& report)
{
   for (int ind = 0;ind < getSize();ind++)
   {
      COdbComponent* component = getAt(ind);
      component->generatePackagePinReport(step,report);
   }
}

void COdbComponents::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbComponents: ");
   writeFormat.writef("getSize()=%d\n",getSize());

   if (depth != 0)
   {
      for (int ind = 0;ind < getSize();ind++)
      {
         COdbComponent* component = getAt(ind);
         component->dump(writeFormat,depth);
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbComponentsFile::COdbComponentsFile(bool topFlag) : 
   m_topFlag(topFlag)
{
}

bool COdbComponentsFile::readComponents(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
   bool topComponentLayerFlag,CWriteFormat& log)
{
   CFilePath componentsFilePath = odbPpDatabase.getDatabaseFilePath();
   componentsFilePath.pushLeaf("steps");
   componentsFilePath.pushLeaf(step.getName());
   componentsFilePath.pushLeaf("layers");
   componentsFilePath.pushLeaf(layer.getName());
   componentsFilePath.pushLeaf("components");

   bool retval = true;
   updateToExistingCompressableFilePath(componentsFilePath,log);
   //bool retval = getOdbCompressableFileCache().uncompress(componentsFilePath.getPath(),log);

   while (retval && fileExists(componentsFilePath.getPath()))
   {
      COdbAppFileReader fileReader;

      if (fileReader.openFile(componentsFilePath.getPath()) != statusSucceeded)
      {
         log.writef(PrefixError,
            "Could not read the Component Layer file, '%s' - status = '%s'.\n",
            (const char*)componentsFilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));

         break;
      }

      fileReader.setInchesPerUnit(odbPpDatabase.getInchesPerUnit());
      fileReader.setPageUnitsPerUnit(odbPpDatabase.getPageUnitsPerUnit());

      displayProgress(log,"Reading Component Layer file, '%s'",(const char*)componentsFilePath.getPath());

      COdbPackage* package = NULL;
      COdbComponent* component = NULL;
      DataStruct* componentData = NULL;
      CString refDes;

      m_featureAttributes.initializeSettings(getOdbOptions().getFeatureAttributes());

      CString line;
      int lineNumber;
      int numParams;

      CFileReadProgress fileReadProgress(fileReader);

      while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         lineNumber = fileReader.getNumRecordsRead();
         line       = fileReader.getRecord();
         numParams  = fileReader.getNumParams();

         if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

         if (numParams < 1 || fileReader.getParam(0).GetLength() == 0)
         {
            continue;
         }

         CString param0 = fileReader.getParam(0);
         param0.MakeUpper();

         const char* recordType0 = param0.GetBuffer();
         const char* recordType  = recordType0;

         unsigned int recordCode =  getRecordCode(recordType0);

         switch (recordCode)
         {
         case RecordCode1(#):  // comment
            break;
         case RecordCode1($):  // feature symbol names
            if (numParams > 1)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString name = fileReader.getParam(1);
               COdbFeatureSymbol* symbol = odbPpDatabase.getFeatureSymbol(name);

               if (m_featureSymbolArray.GetSize() == serialNumber)
               {
                  m_featureSymbolArray.Add(symbol);
               }
               else
               {
                  log.writef(PrefixError,
                     "Feature symbol sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureSymbolArray.GetSize(),serialNumber,
                     (const char*)componentsFilePath.getPath(),lineNumber);

                  if (serialNumber >= m_featureSymbolArray.GetSize())
                  {
                     m_featureSymbolArray.SetAtGrow(serialNumber,symbol);
                  }
               }
            }

            break;
         case RecordCode1(@):  // feature attribute names
            if (numParams > 1)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString name = fileReader.getParam(1);

               if (m_featureAttributes.getAttributeNameCount() != serialNumber)
               {
                  log.writef(PrefixError,
                     "Feature attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureAttributes.getAttributeNameCount(),serialNumber,
                     (const char*)componentsFilePath.getPath(),lineNumber);
               }

               m_featureAttributes.addAttributeName(name);
            }

            break;
         case RecordCode1(&):  // feature attribute text strings
            if (numParams > 0)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString textString = ((numParams > 1) ? fileReader.getParam(1) : "");

               if (m_featureAttributes.getAttributeTextStringCount() != serialNumber)
               {
                  log.writef(PrefixError,
                     "Feature attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureAttributes.getAttributeTextStringCount(),serialNumber,
                     (const char*)componentsFilePath.getPath(),lineNumber);
               }

               m_featureAttributes.addAttributeTextString(textString);
            }

            break;
         case RecordCode3(C,M,P):  // Start of component
            if (numParams >= 8)
            {
               int packageId      = atoi(fileReader.getParam(1));
               double x           = odbPpDatabase.toPageUnits(fileReader.getParam(2));
               double y           = odbPpDatabase.toPageUnits(fileReader.getParam(3));
               double rotation    = atof(fileReader.getParam(4));
               bool mirrorFlag    = (fileReader.getParam(5).CompareNoCase("M") == 0);
               refDes             = fileReader.getParam(6);
               CString partNumber = fileReader.getParam(7);

               refDes.MakeUpper();

               component = m_components.addComponent(packageId,x,y,-rotation,m_topFlag,mirrorFlag,
                              refDes,partNumber);
               m_featureAttributes.addAttributes(component->getAttributes(),fileReader,8,
                  componentsFilePath.getPath(), line, lineNumber, log);
            }

            break;
         case RecordCode3(T,O,P):  // Toeprint of a component
            if (numParams >= 9)
            {
               int pinNumber    = atoi(fileReader.getParam(1));
               double x         = odbPpDatabase.toPageUnits(fileReader.getParam(2));  // board coordinates
               double y         = odbPpDatabase.toPageUnits(fileReader.getParam(3));
               double rotation  = atof(fileReader.getParam(4));
               bool mirrorFlag  = (fileReader.getParam(5).CompareNoCase("M") == 0);  // N = not mirrored, M = mirrored
               int netNumber    = atoi(fileReader.getParam(6));
               int subnetNumber = atoi(fileReader.getParam(7));
               CString pinName  = fileReader.getParam(8);

               if (getOdbOptions().getOptionFlag(optionEnablePinNameRefdesEdit))
               {
                  pinName.MakeUpper();

                  if (pinName.Left(refDes.GetLength()) == refDes)
                  {
                     pinName = pinName.Mid(refDes.GetLength());

                     if (pinName.Left(1) == "-")
                     {
                        pinName = pinName.Mid(1);
                     }
                  }
               }

               if (component != NULL)
               {
                  component->addPin(pinNumber,x,y,-rotation,mirrorFlag,
                                                 netNumber,subnetNumber,pinName);
               }
            }

            break;
         case RecordCode3(P,R,P):  // Property of a component
            if (numParams >= 2 && component != NULL)
            {
               CString propertyName  = fileReader.getParam(1);
               CString propertyValue = fileReader.getParam(2);
               CString floatingNumbers;
               int floatNumIndex = 3;

               if(line.Find("'") > -1)
               {
                  for(; floatNumIndex < numParams && propertyValue.GetLength() >= 2; floatNumIndex++)
                  {
                     if (propertyValue.Left(1) != "'" && propertyValue.Left(1) != "\"")
                     {
                        propertyName.Append(" " + propertyValue);
                        propertyValue = fileReader.getParam(floatNumIndex);
                     }
                     else
                        break;
                  }
               }

               if (propertyValue.GetLength() >= 2)
               {
                  if ((propertyValue.Left(1) == "'"  && propertyValue.Right(1) == "'" ) ||
                      (propertyValue.Left(1) == "\"" && propertyValue.Right(1) == "\"")    )
                  {
                     propertyValue = propertyValue.Mid(1,propertyValue.GetLength() - 2);
                  }                  
               }

               for (int ind = floatNumIndex;ind < numParams;ind++)
               {
                  if (ind != floatNumIndex) floatingNumbers += " ";

                  floatingNumbers += fileReader.getParam(ind);
               }

               COdbProperty* property = new COdbProperty(propertyName,propertyValue,floatingNumbers);

               component->addProperty(property);
            }

            break;
         default:
            log.writef(PrefixError,
               "Unrecognized record code of '%s' encountered.  Record='%s' in '%s'.%d\n",
               recordType0,(const char*)line,(const char*)componentsFilePath.getPath(),lineNumber);

            break;
         }
      }

      fileReader.close();
      fileReader.writeReadStatistics(&log,PrefixStatus);

      break;
   }

   hideProgress();

   return retval;
}

//----------------------------------------------------------------------------------

bool COdbComponentsFile::readComponents2(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
   bool topComponentLayerFlag,CWriteFormat& log)
{
   CFilePath components2FilePath = odbPpDatabase.getDatabaseFilePath();
   components2FilePath.pushLeaf("steps");
   components2FilePath.pushLeaf(step.getName());
   components2FilePath.pushLeaf("layers");
   components2FilePath.pushLeaf(layer.getName());
   components2FilePath.pushLeaf("components2");

   bool retval = true;
   updateToExistingCompressableFilePath(components2FilePath,log);
   //bool retval = getOdbCompressableFileCache().uncompress(components2FilePath.getPath(),log);

   while (retval && fileExists(components2FilePath.getPath()))
   {
      COdbAppFileReader fileReader;

      if (fileReader.openFile(components2FilePath.getPath()) != statusSucceeded)
      {
         log.writef(PrefixError,
            "Could not read the Component Layer file, '%s' - status = '%s'.\n",
            (const char*)components2FilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));

         break;
      }

      fileReader.setInchesPerUnit(odbPpDatabase.getInchesPerUnit());
      fileReader.setPageUnitsPerUnit(odbPpDatabase.getPageUnitsPerUnit());

      displayProgress(log,"Reading Component Layer file, '%s'",(const char*)components2FilePath.getPath());

      COdbComponent* component = NULL;
      CString refDes;

      CString line;
      int lineNumber;
      int numParams;

      CFileReadProgress fileReadProgress(fileReader);

      while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         lineNumber = fileReader.getNumRecordsRead();
         line       = fileReader.getRecord();
         numParams  = fileReader.getNumParams();

         if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

         if (numParams < 1 || fileReader.getParam(0).GetLength() == 0)
         {
            continue;
         }

         CString param0 = fileReader.getParam(0);
         param0.MakeUpper();

         const char* recordType0 = param0.GetBuffer();
         const char* recordType  = recordType0;

         unsigned int recordCode =  getRecordCode(recordType0);

         switch (recordCode)
         {
         case RecordCode1(#):  // comment
            break;
         case RecordCode1($):  // feature symbol names
            break;
         case RecordCode1(@):  // feature attribute names
            break;
         case RecordCode1(&):  // feature attribute text strings
            break;
         case RecordCode3(C,M,P):  // Start of component
            if (numParams >= 8)
            {
               int packageId      = atoi(fileReader.getParam(1));
               double x           = odbPpDatabase.toPageUnits(fileReader.getParam(2));
               double y           = odbPpDatabase.toPageUnits(fileReader.getParam(3));
               double rotation    = atof(fileReader.getParam(4));
               bool mirrorFlag    = (fileReader.getParam(5).CompareNoCase("M") == 0);
               refDes             = fileReader.getParam(6);
               CString partNumber = fileReader.getParam(7);

               refDes.MakeUpper();

               m_components.lookup(refDes, component);

               if (component == NULL)
               {
                  // no op, rest of parsing will toss the data
               }
               else if (component->getRefDes() != refDes)
               {
                  component = NULL;
               }
            }

            break;
         case RecordCode3(T,O,P):  // Toeprint of a component
            break;
         case RecordCode3(P,R,P):  // Property of a component
            break;
         case RecordCode3(C,P,N):  // Customer Part Number
            if (numParams >= 2 && component != NULL)
            {
               CString customerPartNumber = fileReader.getParam(1);
               customerPartNumber.Trim();

               component->addCustomerPartNumber(customerPartNumber);
            }

            break;
         case RecordCode3(I,P,N):  // Internal Part Number
            if (numParams >= 2 && component != NULL)
            {
               CString internalPartNumber = fileReader.getParamToEndOfLine(1);  // PN may have embedded spaces
               internalPartNumber.Trim();

               component->addInternalPartNumber(internalPartNumber);
            }

            break;
         case RecordCode3(P,K,G):  // Package Name
            if (numParams >= 2 && component != NULL)
            {
               CString packageName = fileReader.getParam(1);
               packageName.Trim();

               component->addPackageName(packageName);
            }

            break;
         case RecordCode3(D,S,C):  // Description
            if (numParams >= 2 && component != NULL)
            {
               CString description = fileReader.getParamToEndOfLine(1);
               description.Trim();

               component->addDescription(description);
            }

            break;
         case RecordCode4(V,P,L,_):  
            if (numParams >= 2 && component != NULL)
            {
               if (param0 == "VPL_VND") // VPL Vendor
               {
                  CString vplVendor = fileReader.getParam(1);
                  vplVendor.Trim();

                  component->addVplVendorName(vplVendor);
               }
               else if (param0 == "VPL_MPN") // VPL Manufacturer Part Number
               {
                  CString vplManufacturerPartNumber = fileReader.getParam(1);
                  vplManufacturerPartNumber.Trim();

                  component->addVplManufacturerPartNumber(vplManufacturerPartNumber);
               }
            }

            break;
         case RecordCode3(V,N,D):  // Vendor
            if (numParams >= 2 && component != NULL)
            {
               CString vendor = fileReader.getParam(1);
               vendor.Trim();

               component->addVendorName(vendor);
            }

            break;
         case RecordCode3(M,P,N):  // Manufacturer Part Number
            if (numParams >= 2 && component != NULL)
            {
               CString manufacturerPartNumber = fileReader.getParam(1);
               manufacturerPartNumber.Trim();

               component->addManufacturerPartNumber(manufacturerPartNumber);
            }

            break;
         default:
            log.writef(PrefixError,
               "Unrecognized record code of '%s' encountered.  Record='%s' in '%s'.%d\n",
               recordType0,(const char*)line,(const char*)components2FilePath.getPath(),lineNumber);

            break;
         }
      }

      fileReader.close();
      fileReader.writeReadStatistics(&log,PrefixStatus);

      break;
   }

   hideProgress();

   return retval;
}

//----------------------------------------------------------------------------------

bool COdbComponentsFile::readComponents3(COdbPpDatabase& odbPpDatabase,COdbStep& step,COdbLayer& layer,
   bool topComponentLayerFlag,CWriteFormat& log)
{
   CFilePath components3FilePath = odbPpDatabase.getDatabaseFilePath();
   components3FilePath.pushLeaf("steps");
   components3FilePath.pushLeaf(step.getName());
   components3FilePath.pushLeaf("layers");
   components3FilePath.pushLeaf(layer.getName());
   components3FilePath.pushLeaf("components3");

   bool retval = true;
   updateToExistingCompressableFilePath(components3FilePath,log);
   //bool retval = getOdbCompressableFileCache().uncompress(components2FilePath.getPath(),log);

   while (retval && fileExists(components3FilePath.getPath()))
   {
      COdbAppFileReader fileReader;

      if (fileReader.openFile(components3FilePath.getPath()) != statusSucceeded)
      {
         log.writef(PrefixError,
            "Could not read the Component Layer file, '%s' - status = '%s'.\n",
            (const char*)components3FilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));

         break;
      }

      m_featureAttributes.ResetAttributeNameList();

      fileReader.setInchesPerUnit(odbPpDatabase.getInchesPerUnit());
      fileReader.setPageUnitsPerUnit(odbPpDatabase.getPageUnitsPerUnit());

      displayProgress(log,"Reading Component Layer file, '%s'",(const char*)components3FilePath.getPath());

      COdbComponent* component = NULL;
      CString refDes;

      CString line;
      int lineNumber;
      int numParams;

      CFileReadProgress fileReadProgress(fileReader);

      while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         lineNumber = fileReader.getNumRecordsRead();
         line       = fileReader.getRecord();
         numParams  = fileReader.getNumParams();

         if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

         if (numParams < 1 || fileReader.getParam(0).GetLength() == 0)
         {
            continue;
         }

         CString param0 = fileReader.getParam(0);
         param0.MakeUpper();

         const char* recordType0 = param0.GetBuffer();
         const char* recordType  = recordType0;

         unsigned int recordCode =  getRecordCode(recordType0);

         switch (recordCode)
         {
         case RecordCode1(#):  // comment
            break;
         case RecordCode1($):  // feature symbol names
            break;
         case RecordCode1(@):  // feature attribute names
            if (numParams > 1)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString name = fileReader.getParam(1);

               if (m_featureAttributes.getAttributeNameCount() != serialNumber)
               {
                  log.writef(PrefixError,
                     "Feature attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureAttributes.getAttributeNameCount(), serialNumber,
                     (const char*)components3FilePath.getPath(), lineNumber);
               }

               m_featureAttributes.addAttributeName(name);
            }
            break;
         case RecordCode1(&):  // feature attribute text strings
            if (numParams > 0)
            {
               int serialNumber = atoi(recordType0 + 1);
               CString textString = ((numParams > 1) ? fileReader.getParam(1) : "");

               if (m_featureAttributes.getAttributeTextStringCount() != serialNumber)
               {
                  log.writef(PrefixError,
                     "Feature attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                     m_featureAttributes.getAttributeTextStringCount(),serialNumber,
                     (const char*)components3FilePath.getPath(),lineNumber);
               }

               m_featureAttributes.addAttributeTextString(textString);
            }
            break;
         case RecordCode3(C,M,P):  // Start of component
            if (numParams >= 8)
            {
               int packageId      = atoi(fileReader.getParam(1));
               double x           = odbPpDatabase.toPageUnits(fileReader.getParam(2));
               double y           = odbPpDatabase.toPageUnits(fileReader.getParam(3));
               double rotation    = atof(fileReader.getParam(4));
               bool mirrorFlag    = (fileReader.getParam(5).CompareNoCase("M") == 0);
               refDes             = fileReader.getParam(6);
               CString partNumber = fileReader.getParam(7);

               refDes.MakeUpper();

               m_components.lookup(refDes, component);

               if (component == NULL)
               {
               }
               else if (component->getRefDes() != refDes)
               {
                  component = NULL;
               }
               else
               {
                  // Is okay
                  // Save pkg ID as specific from components3 file
                  component->setComps3PackageId(packageId);

                  // Save x,y,theta
                  component->setComps3Origin( CPoint2d(x,y) );
                  component->setComps3Rotation(-rotation);

                  // Update attribs from CMP line
                  m_featureAttributes.addAttributes(component->getAttributes(), fileReader, 8,
                     components3FilePath.getPath(), line, lineNumber, log);
               }
            }

            break;
         case RecordCode3(T,O,P):  // Toeprint of a component
            break;
         case RecordCode3(P,R,P):  // Property of a component
            break;
         case RecordCode3(C,P,N):  // Customer Part Number
            if (numParams >= 2 && component != NULL)
            {
               CString customerPartNumber = fileReader.getParamToEndOfLine(1);
               customerPartNumber.Trim();

               component->addCustomerPartNumber(customerPartNumber);
            }

            break;
         case RecordCode3(I,P,N):  // Internal Part Number
            if (numParams >= 2 && component != NULL)
            {
               CString internalPartNumber = fileReader.getParamToEndOfLine(1);  // PN may have embedded spaces
               internalPartNumber.Trim();

               component->addInternalPartNumber(internalPartNumber);
            }

            break;
         case RecordCode3(P,K,G):  // Package Name
            if (numParams >= 2 && component != NULL)
            {
               CString packageName = fileReader.getParamToEndOfLine(1);
               packageName.Trim();

               component->addPackageName(packageName);
            }

            break;
         case RecordCode3(D,S,C):  // Description
            if (numParams >= 2 && component != NULL)
            {
               CString description = fileReader.getParamToEndOfLine(1);
               description.Trim();

               component->addDescription(description);
            }

            break;
         case RecordCode4(V,P,L,_):  
            if (numParams >= 2 && component != NULL)
            {
               if (param0 == "VPL_VND") // VPL Vendor
               {
                  CString vplVendor = fileReader.getParamToEndOfLine(1);
                  vplVendor.Trim();

                  component->addVplVendorName(vplVendor);
               }
               else if (param0 == "VPL_MPN") // VPL Manufacturer Part Number
               {
                  CString vplManufacturerPartNumber = fileReader.getParamToEndOfLine(1);
                  vplManufacturerPartNumber.Trim();

                  component->addVplManufacturerPartNumber(vplManufacturerPartNumber);
               }
            }

            break;
         case RecordCode3(V,N,D):  // Vendor
            if (numParams >= 2 && component != NULL)
            {
               CString vendor = fileReader.getParamToEndOfLine(1);
               vendor.Trim();

               component->addVendorName(vendor);
            }

            break;
         case RecordCode3(M,P,N):  // Manufacturer Part Number
            if (numParams >= 2 && component != NULL)
            {
               // MPM 0 Y xxxx
               // 0 is of unknown use, Y (or N) = used or not, xxxx is the PN.
               // Only keep those with used=Y.

               bool isUsed = fileReader.getParam(2).CompareNoCase("Y") == 0;

               if (isUsed)
               {
                  CString manufacturerPartNumber = fileReader.getParamToEndOfLine(3);
                  manufacturerPartNumber.Trim();

                  component->addManufacturerPartNumber(manufacturerPartNumber);
               }
            }

            break;
         default:
            log.writef(PrefixError,
               "Unrecognized record code of '%s' encountered.  Record='%s' in '%s'.%d\n",
               recordType0,(const char*)line,(const char*)components3FilePath.getPath(),lineNumber);

            break;
         }
      }

      fileReader.close();
      fileReader.writeReadStatistics(&log,PrefixStatus);

      break;
   }

   hideProgress();

   return retval;
}

//----------------------------------------------------------------------------------

bool COdbComponentsFile::linkPackages(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   bool retval = true;

   displayProgress(log,"Linking packages to %s components.",m_topFlag ? "top" : "bottom");
   COperationProgress operationProgress(m_components.getSize());

   for (int componentId=0;componentId < m_components.getSize();componentId++)
   {
      operationProgress.incrementProgress();

      COdbComponent* component = m_components.getAt(componentId);

      int packageId = component->getCompsPackageId();

      COdbPackage* package = step.getEdaDataPackage(packageId);

      if (package != NULL)
      {
         if (package->getNumPins() != component->getNumPins())
         {
            retval = false;

            log.writef(PrefixError,
               "Pin count mismatch, package='%s', pinCount=%d, component='%s', pinCount = %d\n",
               (const char*)component->getRefDes(),component->getNumPins(),
               (const char*)package->getValidatedName(),package->getNumPins());
         }
      }
      else
      {
         retval = false;

         log.writef(PrefixError,
            "NULL package for packageId=%d reference by component '%s', componentId=%d\n",
            packageId,(const char*)component->getRefDes(),component->getComponentId());
      }

      component->setPackage(package);
   }

   return retval;
}

//---------------------------------------------------------------------------------------

bool COdbComponentsFile::instantiateAndAttachPadStacks(COdbPpDatabase& odbPpDatabase,
   COdbStep& step,CQfePadTree& padTree,CWriteFormat& log)
{
   bool retval = true;

   displayProgress(log,"Attaching pad stacks to %s components.",m_topFlag ? "top" : "bottom");
   COperationProgress operationProgress(m_components.getSize());

   bool mirrorLayerFlag = !m_topFlag;

   bool padstackQuestFlag = getOdbOptions().getOptionFlag(optionEnablePadstackQuest);

   for (int componentId=0;componentId < m_components.getSize();componentId++)
   {
      operationProgress.incrementProgress();

      COdbComponent* component = m_components.getAt(componentId);

      // used to convert coordinates relative to package/component to board coordinates
      CTMatrix componentToBoardMatrix;
      componentToBoardMatrix.rotateDegrees(component->getRotation());
      componentToBoardMatrix.scale(mirrorLayerFlag ? -1. : 1.,1.);
      componentToBoardMatrix.translate(component->getOrigin());

      for (int pinId = 0;pinId < component->getNumPins();pinId++)
      {
         COdbComponentPin* odbComponentPin = component->getPin(pinId);

         if (odbComponentPin == NULL)
         {
            log.writef(PrefixError,
               "No pin for pinId=%d on component '%s'\n",
               pinId,(const char*)component->getRefDes());

            continue;
         }

         COdbPackage* package = component->getPackage();

         if (package == NULL)
         {
            log.writef(PrefixError,
               "No package for component '%s'\n",
               (const char*)component->getRefDes());

            continue;
         }

         COdbPackagePin* packagePin = package->getPinAt(pinId);

         if (packagePin == NULL)
         {
            log.writef(PrefixError,
               "No package pin for pinId=%d on component '%s'\n",
               pinId,(const char*)component->getRefDes());

            continue;
         }

         CPoint2d packagePinBoardOrigin = packagePin->getOrigin();
         componentToBoardMatrix.transform(packagePinBoardOrigin);

         // used to convert coordinates relative to package pin to board coordinates
         CTMatrix packagePinToBoardMatrix;
         packagePinToBoardMatrix.translate(packagePin->getOrigin());
         packagePinToBoardMatrix.rotateDegrees(component->getRotation());
         packagePinToBoardMatrix.scale(mirrorLayerFlag ? -1. : 1.,1.);
         packagePinToBoardMatrix.translate(component->getOrigin());

         // used to convert board coordinates to coordinates relative to package pin
         CTMatrix boardToPackagePinMatrix(packagePinToBoardMatrix);
         boardToPackagePinMatrix.invert();


         COdbPadstackTemplate padStack;

         COdbToeprintSubnet* subnet = odbComponentPin->getSubnet();
         if (subnet == NULL)
         {
            log.writef(PrefixError,
               "No subnet for pinId=%d on component '%s'\n",
               pinId,(const char*)component->getRefDes());
         }
         else
         {
            for (POSITION pos = subnet->getFidList().GetHeadPosition();pos != NULL;)
            {
               COdbFeatureId* fid = subnet->getFidList().GetNext(pos);
               COdbFeature* feature = fid->getFeature();

               if (feature != NULL && feature->getFeatureType() == padFeature)
               {
                  int layerNumber = fid->getLayerNumber();
                  COdbPad* featurePad = (COdbPad*)feature;
                  COdbLayer* layer = step.getLayer(layerNumber);
                  int layerIndex = (mirrorLayerFlag ? layer->getODBMirrorLayer()->getCCZLayerIndex() : layer->getCCZLayerIndex());

                  // The odbComponentPin rotation is relative to PCB,so is sum of component rotation and
                  // pin rotation. This is the amount to offset the as-read feature pad rotation, to make it
                  // relative to the padstack instead of PCB.
                  double pinRotRelToPcb = odbComponentPin->getRotationDegrees();

                  // WI 16520 - Need to consider top/bottom for pad rotation.
                  //*rcf Yikes, disabling this fix for 16520 makes the current thing work... ?
                  // Does that mean we're writing rotation wrong to odb to start with?
                  if (!this->m_topFlag)
                     pinRotRelToPcb = -pinRotRelToPcb;

                  bool addedFlag = padStack.add(featurePad,layerIndex,boardToPackagePinMatrix,pinRotRelToPcb,log);

                  if (addedFlag)
                  {
                     featurePad->setInstantiatedFlag();
                  }
                  else
                  {
                     log.writef(PrefixWarning, "Multiple Pad feature IDs found on layer number %d for component '%s', pinNumber %d\n",
                        layerIndex, (const char*)component->getRefDes(), odbComponentPin->getPinNumber() );
                  }
               }
            }
         }

         if (padstackQuestFlag)
         {
            CPoint2d stackBoardOrigin(packagePinBoardOrigin);

            if (padStack.getCount() > 0)
            {
               stackBoardOrigin = padStack.getCenterPadOrigin();
               packagePinToBoardMatrix.transform(stackBoardOrigin);
            }

            CExtent searchExtent;
            double searchRadius = getOdbOptions().getOptionDbl(optionPadSearchRadius);
            searchExtent.set(stackBoardOrigin.x - searchRadius,
                             stackBoardOrigin.y - searchRadius,
                             stackBoardOrigin.x + searchRadius,
                             stackBoardOrigin.y + searchRadius);

            CQfePadList foundList;
            int numFound = padTree.search(searchExtent,foundList);

            bool addTopPads = false;
            bool addBotPads = false;
            DeterminePadSurfacesForPadstack(foundList, component, addTopPads, addBotPads);

            for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
            {
               CQfePad* qfePad = foundList.GetNext(foundPos);
               COdbPad& odbPad = qfePad->getPad();

               if (! odbPad.getInstantiatedFlag() && odbPad.getSubnet() == NULL)
               {
                  COdbFeatureFile& featureFile = qfePad->getFeatureFile();
                  COdbLayer* layer = featureFile.getLayer();

                  // The ODB layer has type characteristics, but not surface characteristics, so
                  // get the associated CCZ layer, from which we can get surface characteristics.
                  LayerStruct *cczLayer = cczLayer = layer->getCCZLayerStruct();

                  if (cczLayer != NULL)
                  {
                     // If (adding both top and bottom) OR (adding top and IsTop) OR (adding bottom and IsBottom)
                     if ((addTopPads && addBotPads) ||
                        (addTopPads && cczLayer->isLayerTypeTopSurface()) ||
                        (addBotPads && cczLayer->isLayerTypeBottomSurface()))
                     {

                        int layerIndex = layer->getCCZLayerIndex();

                        if (mirrorLayerFlag)
                        {
                           layerIndex = layer->getODBMirrorLayer()->getCCZLayerStruct()->getLayerIndex();
                        }

                        // WI 16520 - Not clear if rotation adjustment should be done here as well, like other spot above, or not.
                        // Did no need to alter this to make WI test case work, so leaving it as-is. Might be a bug, not sure.
                        // Need data that comes here, until issue is identified this is being left alone.

                        // Using component->rotation here is probably wrong, but don't have test case in hand to test it.
                        // Up above, code changed from component->getRotation to odbComponentPin->getRotation, to correct
                        // an offset issue (when pin and pad in padstack each have their own rotation).
                        // Probably the same offset should be used here, but don't have data to prove it at the moment.
                        // I expect we'll eventually get a bug report on this, then we'll have the data to prove it.

                        bool addedFlag = padStack.add(&odbPad,layerIndex,boardToPackagePinMatrix,component->getRotation(),log);

                        if (addedFlag)
                        {
                           odbPad.setInstantiatedFlag();
                        }
                        else
                        {
                           log.writef(PrefixWarning, "Multiple unconnected pads found on layer number %d for component '%s', pinNumber %d\n",
                              layerIndex,(const char*)component->getRefDes(),odbComponentPin->getPinNumber() );
                        }
                     }
                  }
               }
            }
         }

         if (padStack.getCount() > 0)
         {
            // the pad coordinates are relative to the package pin
            CPoint2d stackOffset = padStack.centerPads();

            COdbPadstackGeometry& padstackGeometry = odbPpDatabase.getPadstackGeometryFor(padStack,log);
            odbComponentPin->setPadstackGeometry(&padstackGeometry, stackOffset);
         }
      }
   }

   return retval;
}

bool COdbComponentsFile::createPcbComponentInserts(COdbPpDatabase& odbPpDatabase,
   COdbStep& step,CWriteFormat& log)
{
   bool retval = true;

   displayProgress(log,"Processing packages for %s components.",m_topFlag ? "top" : "bottom");
   COperationProgress operationProgress(m_components.getSize());

   for (int componentId=0;componentId < m_components.getSize();componentId++)
   {
      operationProgress.incrementProgress();

      COdbComponent* component = m_components.getAt(componentId);
      int compsPackageId = component->getCompsPackageId();

      COdbPackage* package = step.getEdaDataPackage(compsPackageId);

      if (package != NULL)
      {
         // Cnstantiate package geometry.
         COdbPackageGeometry& packageGeometry = package->getPackageGeometryFor(odbPpDatabase,step,*component,log);

         // Eeference the device.
         odbPpDatabase.getCamCadDatabase().referenceDevice(component->getPartNumber(),
            packageGeometry.getBlock(),step.getFile());

         // Create the component insert.
         double angleInRadians = degreesToRadians(component->getRotation());

         DataStruct* componentData = odbPpDatabase.getCamCadDatabase().referenceBlock(
            step.getBlock(),packageGeometry.getBlock(),
            insertTypePcbComponent, component->getRefDes(),NoLayer,
            component->getOrigin().x,component->getOrigin().y,
            (m_topFlag ? angleInRadians : -angleInRadians),!m_topFlag);

         // We'll need this later to set up RealPart inserts.
         component->setCompInsertData(componentData);

         // Put attribs on the component insert, from the imported data.
         component->placeAttributes(getAttributeMap(componentData),
            odbPpDatabase.getCamCadDatabase(), log);
         
         // Always put REFNAME attrib.
         odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(componentData),
            ATT_REFNAME,valueTypeString,component->getRefDes(),log);

         // Always put $$DEVICE$$ attrib.
         odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(componentData),
            ATT_TYPELISTLINK,valueTypeString,component->getPartNumber(),log);
      }
      else
      {
         log.writef(PrefixDisaster,
            "COdbComponentsFile::createPcbComponentInserts() - Could not find package for packageId %d\n",
            compsPackageId);
      }
   }

   return retval;
}

void COdbComponentsFile::DeterminePadSurfacesForPadstack(CQfePadList &foundList, COdbComponent *component, bool &addTopPads, bool &addBotPads)
{
   // Classify the collection, if found to be THRU then all pads will be added
   // to the padstack, if found to be SMD then only pads on same side as component
   // will be added to the padstack.
   bool hasTopPads = false;
   bool hasBotPads = false;
   bool hasDrill = false;
   for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
   {
      CQfePad* qfePad = foundList.GetNext(foundPos);
      COdbPad& odbPad = qfePad->getPad();

      // Only interested in pads that have not yet been used.
      if (! odbPad.getInstantiatedFlag())
      {
         COdbFeatureFile& featureFile = qfePad->getFeatureFile();
         COdbLayer* layer = featureFile.getLayer();

         if (layer->getType() == odbLayerTypeDrill)
         {
            hasDrill = true;
         }
         else
         {
            LayerStruct *cczLayer = NULL;
            //if (mirrorLayerFlag)
            //   cczLayer = layer->getODBMirrorLayer()->getCCZLayerStruct();
            //else
            cczLayer = layer->getCCZLayerStruct();

            bool isTop = cczLayer->isLayerTypeTopSurface();
            bool isBot = cczLayer->isLayerTypeBottomSurface();

            if (cczLayer != NULL)
            {
               if (cczLayer->isLayerTypeTopSurface())
                  hasTopPads = true;
               else if (cczLayer->isLayerTypeBottomSurface())
                  hasBotPads = true;
               // else is inner, we don't need to care about inner here.
            }
         }
      }
   }

   // If there is a drill then we will add all the pads, regardless of side.
   // If no drill then we want only pads on same side as component.
   if (hasDrill)
   {
      addTopPads = addBotPads = true;
   }
   else
   {
      // No drill, select pad set that matches component surface.
      addTopPads = component->IsPlacedTop(); // If comp is top then place top pads.
      addBotPads = !addTopPads;              // Bot gets opposite treatment of top.
   }
}

int COdbComponentsFile::attachPadStacksToUnconnectedPins(COdbPpDatabase& odbPpDatabase,
   COdbStep& step,CQfePadTree& padTree,CWriteFormat& log)
{
   int retval = 0;

   displayProgress(log,"Attaching pad stacks to unconnected pins for %s components.",m_topFlag ? "top" : "bottom");
   COperationProgress operationProgress(m_components.getSize());

   CExtent searchExtent;
   double searchRadius = getOdbOptions().getOptionDbl(optionPadSearchRadius);
   bool mirrorLayerFlag = !m_topFlag;

   int odbDrillLayerIndex = step.getDrillLayerIndex();

   for (int componentId=0; componentId < m_components.getSize(); componentId++)
   {
      operationProgress.incrementProgress();

      COdbComponent* component = m_components.getAt(componentId);

      // used to convert coordinates relative to package pin to board coordinates
      CTMatrix componentToBoardMatrix;
      componentToBoardMatrix.rotateDegrees(component->getRotation());
      componentToBoardMatrix.scale(mirrorLayerFlag ? -1. : 1.,1.);
      componentToBoardMatrix.translate(component->getOrigin());

      for (int pinId = 0;pinId < component->getNumPins();pinId++)
      {
         COdbComponentPin* odbComponentPin = component->getPin(pinId);

         if (odbComponentPin == NULL)
         {
            continue;
         }

         if (odbComponentPin->getPadstackGeometry() != NULL)
         {
            continue;
         }

         COdbPackage* package = component->getPackage();

         COdbPackagePin* packagePin = ((package == NULL) ? NULL : package->getPinAt(pinId));

         if (packagePin != NULL)
         {
            CPoint2d packagePinOrigin = packagePin->getOrigin();
            componentToBoardMatrix.transform(packagePinOrigin);

            // used to convert board coordinates to coordinates relative to component pin
            CTMatrix boardToPackagePinMatrix;
            boardToPackagePinMatrix.translate(packagePin->getOrigin());
            boardToPackagePinMatrix.rotateDegrees(component->getRotation());
            boardToPackagePinMatrix.translate(component->getOrigin());
            boardToPackagePinMatrix.invert();

            searchExtent.set(packagePinOrigin.x - searchRadius,
               packagePinOrigin.y - searchRadius,
               packagePinOrigin.x + searchRadius,
               packagePinOrigin.y + searchRadius);

            CQfePadList foundList;
            int numFound = padTree.search(searchExtent, foundList);

            bool addTopPads = false;
            bool addBotPads = false;
            DeterminePadSurfacesForPadstack(foundList, component, addTopPads, addBotPads);

            // Now actually create the padstack and add the pads.
            COdbPadstackTemplate padStack;
            for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
            {
               CQfePad* qfePad = foundList.GetNext(foundPos);
               COdbPad& odbPad = qfePad->getPad();

               if (! odbPad.getInstantiatedFlag())
               {
                  COdbFeatureFile& featureFile = qfePad->getFeatureFile();
                  COdbLayer* layer = featureFile.getLayer();

                  // The ODB layer has type characteristics, but not surface characteristics, so
                  // get the associated CCZ layer, from which we can get surface characteristics.
                  LayerStruct *cczLayer = cczLayer = layer->getCCZLayerStruct();

                  if (cczLayer != NULL)
                  {
                     // If (adding both top and bottom) OR (adding top and IsTop) OR (adding bottom and IsBottom)
                     if ((addTopPads && addBotPads) ||
                        (addTopPads && cczLayer->isLayerTypeTopSurface()) ||
                        (addBotPads && cczLayer->isLayerTypeBottomSurface()))
                     {
                        int layerIndex = layer->getCCZLayerIndex();

                        if (mirrorLayerFlag)
                        {
                           layerIndex = layer->getODBMirrorLayer()->getCCZLayerStruct()->getLayerIndex();
                        }

                        bool addedFlag = padStack.add(&odbPad, layerIndex, boardToPackagePinMatrix, component->getRotation(), log);

                        if (addedFlag)
                        {
                           odbPad.setInstantiatedFlag();
                        }
                        else
                        {
                           log.writef(PrefixWarning, 
                              "Multiple unconnected pads found on layer number %d for component '%s', pinNumber %d\n",
                              layerIndex,(const char*)component->getRefDes(),odbComponentPin->getPinNumber() );
                        }
                     }
                  }
               }
            }

            if (padStack.getCount() > 0)
            {
               CPoint2d stackOffset = padStack.centerPads();

               COdbPadstackGeometry& padstackGeometry = odbPpDatabase.getPadstackGeometryFor(padStack, log);
               odbComponentPin->setPadstackGeometry(&padstackGeometry,stackOffset);

               log.writef(PrefixStatus,
                  "Attached pad stack '%s' to unconnected pin %s.%s\n",
                  (const char*)padstackGeometry.getName(),
                  (const char*)component->getRefDes(),
                  (const char*)odbComponentPin->getPinReference() );
            }
            else
            {
               retval++;

               log.writef(PrefixWarning,
                  "Could not attach pad stack to unconnected pin %s.%s\n",
                  (const char*)component->getRefDes(),
                  (const char*)odbComponentPin->getPinReference() );
            }
         }
      }
   }

   return retval;
}

void COdbComponentsFile::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbComponentsFile: ");
   writeFormat.writef("m_filePath='%s'\n",(const char*)m_filePath.getPath());

   if (depth != 0)
   {
      m_components.dump(writeFormat,depth);
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbStep::COdbStep(const CString& name,int column)
: m_topComponentsFile(true)
, m_bottomComponentsFile(false)
, m_edaDataPackages("EDA/DATA")
, m_edaVplPackages("EDA/VPL_PKGS")
{
   m_name   = name;
   m_column = column;

   m_drillLayerIndex = DrillLayerNotCalculated;

   m_file = Graph_File_Start(m_name,Type_ODB_PP);
   m_file->setBlockType(blockTypePcb);
   m_file->setShow(false);
   m_block = m_file->getBlock();
	m_block->setBlockType(blockTypePcb);
}

COdbStep::~COdbStep()
{
   empty();
}

void COdbStep::empty()
{
}

COdbBomCp::COdbBomCp(CString cpn)
: m_cpn(cpn)
{
}

COdbBom::COdbBom()
: m_devicewTypeIndx(-1)
, m_valueIndx(-1)
, m_posTolIndx(-1)
, m_negTolIndx(-1)
, m_subclassIndx(-1)
, m_pinMappingsIndx(-1)
, m_ipnDescIndx(-1)
, m_cpnDescIndx(-1)
, m_plainDescIndx(-1)
{
}

bool COdbBom::IsMappedDescriptionIndex(int indx)
{
   if (indx < 0)
      return false;

   if (indx == m_devicewTypeIndx)
      return true;

   if (indx == m_valueIndx)
      return true;

   if (indx == m_posTolIndx)
      return true;

   if (indx == m_negTolIndx)
      return true;

   if (indx == m_subclassIndx)
      return true;

   if (indx == m_pinMappingsIndx)
      return true;

   if (indx == m_ipnDescIndx)
      return true;

   if (indx == m_cpnDescIndx)
      return true;

   if (indx == m_plainDescIndx)
      return true;

   return false;
}

void COdbBom::SetDescriptionIndex(CString descriptionAlias, int odbIndx)
{
   // ODB Index starts at 1, we convert this to zero-based CStringArray index.
   int ourIndx = odbIndx - 1;

   // Save DSC alias in table.
   int curCnt = m_descriptionAliasTable.GetCount();
   this->m_descriptionAliasTable.SetAtGrow(ourIndx, descriptionAlias);

   // Select out specifically recognized DSCs, these get mapped to specific
   // CCZ attribute.
   if (descriptionAlias.CompareNoCase("ElectricalType") == 0)
   {
      m_devicewTypeIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("ElectricalValue") == 0)
   {
      m_valueIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("PositiveTolerance") == 0)
   {
      m_posTolIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("NegativeTolerance") == 0)
   {
      m_negTolIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("Subclass") == 0)
   {
      m_subclassIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("PinMappings") == 0)
   {
      m_pinMappingsIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("Description") == 0)
   {
      m_plainDescIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("IPN_Description") == 0)
   {
      m_ipnDescIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("CPN_Description") == 0)
   {
      m_cpnDescIndx = ourIndx;
   }
   else if (descriptionAlias.CompareNoCase("CPN_Revision") == 0)
   {
      // Don't care, ignore.
   }
   else if (descriptionAlias.CompareNoCase("CPN_Cost") == 0)
   {
      // Don't care, ignore.
   }
   else
   {
      // Alias not recognized, don't care at the moment.
   }
}

CString COdbBom::GetValidatedDeviceType(COdbBomCp *cp)
{
   CString devTypeStr( this->GetDeviceType(cp) );

   DeviceTypeTag devType = stringToDeviceTypeTag(devTypeStr);

   if (devType != deviceTypeUnknown && devType != deviceTypeUndefined)
      return devTypeStr;

   return "";
}

void COdbStep::ApplyVplPkgs(COdbPpDatabase& odbPpDatabase, COdbComponentsFile& odbComponentsFile, CWriteFormat& log)
{
   // Create RP insert for components that are LOADED=TRUE only. Skip LOADED=FALSE components.

   CCamCadData& ccData = odbPpDatabase.getCamCadData();
   int loadedKW        = ccData.getAttributeKeywordIndex(standardAttributeLoaded);

   for (int componentId = 0; componentId < odbComponentsFile.getSize(); componentId++)
   {
      //operationProgress.incrementProgress();

      COdbComponent* component = odbComponentsFile.getComponent(componentId);
      
      if (component != NULL && component->getCompInsertData() != NULL)
      {
         DataStruct *data = component->getCompInsertData();

         if (data != NULL)
         {
            Attrib *attrib;
            if (data->lookUpAttrib(loadedKW, attrib))
            {
               CString val( attrib->getStringValue() );

               if (val.CompareNoCase("true") == 0)
                  CreateRealPartInsert(odbPpDatabase, component, odbComponentsFile.getTopFlag(), log);
            }
         }
      }
   }

}

void COdbStep::CreateRealPartInsert(COdbPpDatabase& odbPpDatabase, COdbComponent* component, bool topFlag, CWriteFormat& log)
{
   if (component != NULL && component->getCompInsertData() != NULL)
   {
      DataStruct *componentData = component->getCompInsertData();

      // Place the PACKAGE_SOURCE attrib.
      // Package_Source, value "Part Number". To match how we are constructing the RPs.
      odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(componentData),
         ATT_PACKAGE_SOURCE,valueTypeString,"Part Number",log);

      // Package ID from components and components3 files.
      int compsPkgId = component->getCompsPackageId();
      int comps3PkgId = component->getComps3PackageId();

      // Everybody gets an RP, the choice to be made is whether it is based on
      // the ODB package named in components or components3 file.
      // The default is to use the components file package.
      int chosenPkgId = compsPkgId;

      // Valid pkd ID is 0 or greater, -1 means not set.
      // There may not even be a components3 file in the ODB job, it is optional.
      // If components3 pkg ID is set then it takes precedence, use it. It may still
      // point to the eda/data file, but that is okay.
      if (comps3PkgId >= 0)
         chosenPkgId = comps3PkgId;

      // The chosenPkgId should always be valid by the time we get here, but hey, bad
      // data happens to good programs, so do a little check. We don't know the upper
      // end of range, so won't check, but we do know it has to be >= 0.
      if (chosenPkgId >= 0)
      {
         // Now we have a bit of awkwardness. The packages are split between two lists, one for 
         // eda/data and one for eda/vpl_pkgs. The PkgID numbering starts at 0 in the eda/data
         // file, then continues on in the eda/vpl_pkgs file. We could just ask each list to
         // give us the package with that ID, one should return NULL and the other should return
         // the package. Or we could check the value of the chosen PkgID and select the
         // correct list. We'll do that, it should be faster than doing the useless search
         // on the wrong list.
         // If chosen ID is a match to components file, then it is an eda/data PkgID, as that
         // is all that is referenced in components file. The components3 file may reference
         // eda/data or eda/vpl_pkgs.

         COdbPackage *pkg = NULL;

         // If pkg ID is less than EDA Pkg count then indx is an EDA Pkg.
         // Otherwise it is a VPL Pkg.
         int edaPkgCnt = getEdaDataPackageCount();
         if (chosenPkgId < edaPkgCnt)
            pkg = this->getEdaDataPackage(chosenPkgId);
         else
            pkg = this->getEdaVplPackage(chosenPkgId);

         // If we got a pkg, then place the RP.
         if (pkg != NULL)
         {
            // We have eda/vpl_pkg for this, we should  have already made
            // a geom for it, find and insert it.

            // This is to accomdate the Data Doctor. The Data Doctor has certain
            // expectations for how the Real Part is named.
            CString dataDocApprovedName;

            // Using partnumber based association naming convention.
            // Use the value of the partnumber attrib in componentData, do NOT use the
            // partnumber that is in ODB component. There are multiple partnumbers in ODB data,
            // import processing, e.g. of BOM file, will select and set the PARTNUMBER attrib
            // in the component. That is the one we need to use.
            // Let the default be the PN from the component, but it should nearly always
            // get replaced with PN from attrib.
            CString partnumber( component->getPartNumber() );
            int kwPartnumber = odbPpDatabase.getCamCadDatabase().getKeywordIndex(ATT_PARTNUMBER);
            Attrib *attrib = NULL;
            if (componentData->lookUpAttrib(kwPartnumber, attrib))
               partnumber = attrib->getStringValue();

            dataDocApprovedName.Format("PART_%s", partnumber );

            // This is the name the RP block will be found by the first time.
            CString rpBlockName;
            rpBlockName.Format("RP_%d", chosenPkgId);

            // Try the Data Doc approved name first.
            BlockStruct *realpartGeom = odbPpDatabase.getCamCadDatabase().getBlock(dataDocApprovedName, this->getFileNum());

            // Check if this PN based geom was created for same PKG ID. It should be. Data is bad if more comps
            // with same PN have different PKG ID. That is, by definition in DataDoc, such data is bad.
            // So if we find this bad PN data situation then update the PN in the component, append the
            // pkg ID.
            bool dataRequiresSuffix = (realpartGeom != NULL && realpartGeom->getOriginalName().Compare(rpBlockName) != 0);
            bool forceSuffix = false; // Debug only, forces Pkg num suffix onto PN, handy to looking at result in CAMCAD.
            CString suffixToAdd;
            if (dataRequiresSuffix || forceSuffix)
            {
               // This PN-based RP was made based on different package. Some options... (1) error out
               // and make nothing, (2) leave it as-is even though for some parts it is probably wrong
               // shape, (3) create a new one with same geom name, i.e. name will not be unique, (4)
               // append something to name and make it unique and make new geom, but it won't work right
               // with Data Doc.
               // Let's try 3. We can do this just be setting realpartGeom to NULL
               // and let it generate in normal fashion., except maybe the camcadDataBase util won't
               // let it happen. Let's see what happens.
               // 3 doesn't work. Only because of the camcaddatabase utility (not the database itself).
               // So we need to make the name unique, we'll supply a suffix to add at gen time.
               suffixToAdd.Format("[Pkg%d]", chosenPkgId);
               // Update the partnumber in the component.
               partnumber += suffixToAdd;
               component->setPartNumber(partnumber);

               // Update the partnumber attrib on the component that was already inserted above.
               odbPpDatabase.getCamCadDatabase().addAttribute(getAttributeMap(componentData),
                  ATT_PARTNUMBER, valueTypeString, partnumber, log);

               // Now update the dataDocApproved name to match, this keeps it still an approved name.
               dataDocApprovedName += suffixToAdd;

               // Maybe this has happened before, so try lookup again now with adjusted name.
               realpartGeom = odbPpDatabase.getCamCadDatabase().getBlock(dataDocApprovedName, this->getFileNum());
               // For now we'll trust that if that lookup worked then the PKG# is a match.
            }

            // 

            // If was not found by adjusted name, try original RP_nn name, where nn is pkg ID.
            if (realpartGeom == NULL)
            {
               BlockStruct *base_realpartGeom = odbPpDatabase.getCamCadDatabase().getBlock(rpBlockName, this->getFileNum());

               if (false) // Rename original to approved name
               {
                  // If found by initial name then reset to data doc approved name.
                  if (realpartGeom != NULL)
                  {
                     base_realpartGeom->setOriginalName( realpartGeom->getName() );
                     base_realpartGeom->setName(dataDocApprovedName);
                     realpartGeom = base_realpartGeom;
                  }
               }
               if (true)
               {
                  // Make copy of original for part specific
                  realpartGeom = odbPpDatabase.getCamCadDatabase().copyBlock(dataDocApprovedName, base_realpartGeom, true);
                  // Save the RP style name as original name. We use this to validate PKG and PN consistency.
                  realpartGeom->setOriginalName(rpBlockName);
               }

            }

            if (realpartGeom == NULL)
            {
               log.writef(PrefixWarning,
                  "COdbStep::CreateRealPartInsert() - Could not find RP package geometry for packageId %d\n",
                  chosenPkgId);
            }
            else
            {
               // Using checkedFlag support to determine if given RP geom is already
               // treated or not. We want to do this only one time to any given RP geom.
               if (realpartGeom->getCheckedFlag() == false)
               {
                  // Transform The Geometry To Align With PCB Component
                  // The notion of zero rotation has turned out to be different between
                  // the pkg def that is in eda/data and the one in eda/vpl_pkgs. One of
                  // the rules of using RealPart Package for DFT is that the notion of
                  // zero is the same for PCB Comp and RealPart. So we need to adjust one
                  // of these so it will match the other. It is because of Data Doctor, it
                  // needs to align and RealPart to a PCB Component.

                  // Keep track that this one has been adjusted. Well, it will be in a moment, if needed.
                  realpartGeom->setCheckedFlag(true);

                  double comps_FileAngleDeg = component->getRotation(); 
                  double comps3FileAngleDeg = component->getComps3Rotation();

                  // For our transformation purposes, the comps insert origin is The Origin,
                  // and the comp3 insert origin is what we want to treat. So if we are using
                  // a comps3 based insert, we need deltas from components3 to components
                  // coords.
                  double deltarot = 0.;
                  double deltax = 0.;
                  double deltay = 0.;

                  if (chosenPkgId != compsPkgId)
                  {
                     deltarot = normalizeDegrees(comps3FileAngleDeg - comps_FileAngleDeg);
                     deltax = component->getComps3Origin().x - component->getOrigin().x;
                     deltay = component->getComps3Origin().y - component->getOrigin().y;
                  }

                  if (deltarot != 0. || deltax != 0. || deltay != 0.)
                  {

                     // Raw dx and dy are effected by component insert rotation, so
                     // figure out what dx dy would be for 0 rotation.

                     bool mirrorGraphicFlag = false;  //*rcf Where should this come from?

                     // Using comps xy as origin, rotate delta to comps3 xy from comps xy by 
                     // the reverse of component insert angle to get 0 rotation insert pt.
                     CTMatrix mat1;
                     mat1.rotateDegrees(-comps_FileAngleDeg);
                     CPoint2d adjustedOrigin(deltax,deltay);
                     adjustedOrigin.transform(mat1);

                     // Now transform the RP geom. Start w/ matrix set up for the rotation
                     CTMatrix comps3ToCompsMat;
                     comps3ToCompsMat.rotateDegrees(deltarot);

                     // Now translate RP by normalized insert pt.
                     comps3ToCompsMat.translate(adjustedOrigin.x, adjustedOrigin.y);

                     // Mirror if appropriate
                     comps3ToCompsMat.scale(mirrorGraphicFlag ? -1. : 1.,1.);

                     // Now finally actually transform the RP geom.
                     realpartGeom->transform(comps3ToCompsMat);
                  }
               }

               // With adjusted RP geom we can insert the RP with same params
               // as component insert. This is a necessary condition, as per rules of
               // engagement between a PCB Component insert and its associated RP insert, for
               // purposes of DFT and proper viewing in Data Doctor.

               double comps_angleInRadians = degreesToRadians(component->getRotation());

               DataStruct* realpartData2 = odbPpDatabase.getCamCadDatabase().referenceBlock(
                  this->getBlock(), realpartGeom,
                  insertTypeRealPart, component->getRefDes(),NoLayer,
                  component->getOrigin().x,component->getOrigin().y,
                  (topFlag ? comps_angleInRadians : -comps_angleInRadians),!topFlag);
            }
         }
      }
   }
}

bool COdbStep::HasPartNumbers(COdbPpDatabase& odbPpDatabase)
{
   // Scan the components for partnumber attribute. First one that
   // is non-blank means this data has partnumbers, return true.
   // If none found then return false.

   CCamCadData& ccData = odbPpDatabase.getCamCadData();

   int partnumberKW  = ccData.getAttributeKeywordIndex(standardAttributePartNumber);

   BlockStruct *stepBlock = this->getBlock();

   POSITION insertPos = stepBlock->getHeadDataInsertPosition();
   while (insertPos != NULL)
   {

      DataStruct  *insertData = stepBlock->getNextDataInsert(insertPos);
      if (insertData != NULL && insertData->isInsertType(insertTypePcbComponent))
      {
         bool hasPN = false;

         CAttribute *pnAttrib;
         if (insertData->lookUpAttrib(partnumberKW, pnAttrib))
         {
            CString pn( pnAttrib->getStringValue() );
            pn.TrimLeft();
            pn.TrimRight();
            hasPN = !(pn.IsEmpty());
            if (hasPN)
               return true;
         }
      }
   }

   return false;
}

void COdbStep::SetLoaded(COdbPpDatabase& odbPpDatabase, bool loaded)
{
   // Set LOADED attrib to incoming loaded param value.

   CCamCadData& ccData = odbPpDatabase.getCamCadData();

   int loadedKW      = ccData.getAttributeKeywordIndex(standardAttributeLoaded);

   BlockStruct *stepBlock = this->getBlock();

   POSITION insertPos = stepBlock->getHeadDataInsertPosition();
   while (insertPos != NULL)
   {

      DataStruct  *insertData = stepBlock->getNextDataInsert(insertPos);
      if (insertData != NULL && insertData->isInsertType(insertTypePcbComponent))
      {
         // Save attrib, value as directed by incoming loaded param.
         ccData.setAttribute(*insertData->getDefinedAttributes(), loadedKW, loaded?"TRUE":"FALSE");

      }
   }
}

void COdbStep::SetLoadedBasedOnPartnumber(COdbPpDatabase& odbPpDatabase)
{
   // Post process - Scan all files for PCB Component inserts.
   // For each such insert, set LOADED attrib = true if PARTNUMBER attrib is 
   // present and not empty. If empty or not present, set LOADED = false.

   CCamCadData& ccData = odbPpDatabase.getCamCadData();

   int partnumberKW  = ccData.getAttributeKeywordIndex(standardAttributePartNumber);
   int loadedKW      = ccData.getAttributeKeywordIndex(standardAttributeLoaded);

   BlockStruct *stepBlock = this->getBlock();

   POSITION insertPos = stepBlock->getHeadDataInsertPosition();
   while (insertPos != NULL)
   {

      DataStruct  *insertData = stepBlock->getNextDataInsert(insertPos);
      if (insertData != NULL && insertData->isInsertType(insertTypePcbComponent))
      {
         bool loaded = false;

         CAttribute *pnAttrib;
         if (insertData->lookUpAttrib(partnumberKW, pnAttrib))
         {
            // PN Attrib is present, loaded = true if not blank, otherwise false.
            // Make sure not to be fooled by non-empty CString that is all blanks.
            CString pn( pnAttrib->getStringValue() );
            pn.TrimLeft();
            pn.TrimRight();
            loaded = !(pn.IsEmpty());
         }
         //else  PN attrib not present, set loaded = false. Locally already init'd to false above.
         
         // Save the attribute value.
         ccData.setAttribute(*insertData->getDefinedAttributes(), loadedKW, loaded?"TRUE":"FALSE");

      }
   }
}

void COdbStep::ApplyBom(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   CCamCadDatabase &ccdb = odbPpDatabase.getCamCadDatabase();

   BlockStruct *stepBlock = this->getBlock();

   if (stepBlock != NULL)
   {
      WORD pnkw = ccdb.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePartNumber);

      for (CDataListIterator insertIterator(*stepBlock, dataTypeInsert); insertIterator.hasNext();)
      {
         DataStruct *compInsertData = insertIterator.getNext();

         if (compInsertData->isInsertType(insertTypePcbComponent))
         {
            // Loaded flag will be set according to BOM entries, if refdes is in BOM.
            // If refdes is not in BOM then set Loaded=false.
            bool loaded = false;

            // Pure BOM style, lookup refdes in BOM to get CPN.
            // Use CPN to get values.
            CString refdes(compInsertData->getInsert()->getRefname());

            COdbBomRdCpn *rdCpn = this->getBom().GetRdCpn(refdes);
            if (rdCpn != NULL)
            {
               // Refdes was found in BOM
               loaded = !rdCpn->GetNoPop(); // CCZ Loaded flag is opposite logic of ODB++ NoPop flag.

               // Find CPN (part number) entry in BOM for refdes.
               COdbBomCp *cp = getBom().GetCp(rdCpn->GetCPN());
               if (cp != NULL)
               {
                  // CPN found, process it.

                  // Use IPN for CCZ PARTNUMBER.
                  if (!cp->GetIPN().IsEmpty())
                     ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributePartNumber,  cp->GetIPN());

                  // Simple attribs on comp data, from DSC fields.
                  CString stringval( getBom().GetValidatedDeviceType(cp) );
                  ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributeDeviceType,     stringval);

                  stringval = getBom().GetValue(cp);
                  ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributeValue,          stringval);

                  stringval = getBom().GetPTol(cp);
                  ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributePlusTolerance,  stringval);

                  stringval = getBom().GetNTol(cp);
                  ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributeMinusTolerance, stringval);

                  // Only set Description if it is non-blank in BOM.
                  // The components2 or components3 file may have already set a non-blank Description, we don't want to
                  // lose it if the one in BOM is blank.
                  stringval = getBom().GetCpnDesc(cp);
                  if (stringval.IsEmpty())
                     stringval = getBom().GetPlainDesc(cp);
                  if (!stringval.IsEmpty())
                     ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributeDescription,    stringval);

                  stringval = getBom().GetSubclass(cp);
                  ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributeSubclass,       stringval);

                  // Set any remaining attribs, i.e. those that are not specifically mapped to a CCZ internal attrib name.
                  for (int dscIndx = 0; dscIndx < cp->GetDscCount(); dscIndx++)
                  {
                     if (!getBom().IsMappedDescriptionIndex(dscIndx))
                     {
                        CString dsc(cp->GetDsc(dscIndx));
                        if (!dsc.IsEmpty())
                        {
                           CString attribName(getBom().GetDescriptionAlias(dscIndx));
                           if (!attribName.IsEmpty())
                           {
                              int kw = ccdb.registerKeyword(attribName, valueTypeString);
                              ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), kw,  dsc);
                           }
                        }
                     }
                  }

                  // Pin Mapping is more than setting attrib on comp, it is setting attribs on associated CompPins.
                  // Loop on pins in component geom, find their compPins, set pinmap attrib if pinmap value is present.
                  stringval = getBom().GetPinMapping(cp);
                  if (stringval.IsEmpty())
                  {
                     // Should an existing pinmap be cleared if this val is empty?
                     // NoOp for now.
                  }
                  else
                  {
                     // Parse pin mapping string, create a pin name map.
                     CString pinMapErrMsg;
                     CMapStringToString pinNameMap;
                     this->getBom().ExtractPinMap(stringval, pinNameMap, pinMapErrMsg);

                     // For removal of attrib if/when necessary.
                     int devToPkgPinMapKW = ccdb.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDeviceToPackagePinMap);

                     // Apply pin name mapping to component compPins.
                     BlockStruct *compGeom = ccdb.getCamCadData().getBlock( compInsertData->getInsert()->getBlockNumber() );
                     if (compGeom != NULL)
                     {
                        CString compRefname( compInsertData->getInsert()->getRefname() );

                        for (CDataListIterator pinIterator(*compGeom, dataTypeInsert); pinIterator.hasNext();)
                        {
                           DataStruct *pinInsertData = pinIterator.getNext();
                           if (pinInsertData->isInsertType(insertTypePin))
                           {
                              CString pinName( pinInsertData->getInsert()->getRefname() );

                              // We have an upper/lower case issue with the pin name. The getCompPin will do an exact
                              // match to what we send because it is looking up in a map. If we don't get it the first time
                              // then try forced lower and forced upper casing of pin name.
                              CompPinStruct *compPin = this->getFile()->getNetList().getCompPin(compRefname, pinName);
                              if (compPin == NULL)
                              {
                                 pinName.MakeUpper();
                                 compPin = this->getFile()->getNetList().getCompPin(compRefname, pinName);
                                 if (compPin == NULL)
                                 {
                                    pinName.MakeLower();
                                    compPin = this->getFile()->getNetList().getCompPin(compRefname, pinName);
                                 }
                              }

                              if (compPin != NULL)
                              {
                                 // If pin name mapping is non-blank then set attrib, otherwise clear attrib.
                                 CString mappedToName;
                                 pinName.MakeLower();
                                 pinNameMap.Lookup(pinName, mappedToName);
                                 if (!mappedToName.IsEmpty())
                                 {
                                    // set
                                    ccdb.getCamCadData().setAttribute(*compPin->getDefinedAttributes(), 
                                       standardAttributeDeviceToPackagePinMap, mappedToName);
                                 }
                                 else
                                 {
                                    // clear
                                    ccdb.removeAttribute(&compPin->getDefinedAttributes(), devToPkgPinMapKW);
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }

            // Set loaded flag attrib
            ccdb.getCamCadData().setAttribute(*compInsertData->getDefinedAttributeMap(), standardAttributeLoaded, loaded?"TRUE":"FALSE");
         }
      }
   }

}

void COdbBom::ExtractPinMap(const CString& pinMapStr, CMapStringToString &pinRefToPinNameMap, CString &errMsg)
{
   // Parse the single string pin name mapping (for entire comp) into a map.
   // Map index is pin refname, associated value is mapped pin name.

   // Data Doctor uses "=", ODB++ uses ":".
   // Data Doctor uses "|", ODB++ uses ",".

	CStringArray records;
   CSupString pinNamesMapString = pinMapStr;
	pinNamesMapString.ParseQuote(records, ",");

   // If value is not empty but we found no records, something must be wrong.
   errMsg.Empty();
   if (!pinMapStr.IsEmpty() && records.GetCount() < 1)
      errMsg.Format("Pin mapping value is malformed.\n[%s]", pinMapStr);

   // Clear the map.
   pinRefToPinNameMap.RemoveAll();

   // Delete all existing pinmap attribs.
   ///resetPinRefToPinNameMapAttrib(camCadDatabase);

   // Add entries to the map. No duplicates collision detection, assuming all pins are uniquely named.
	for (int index=0; index<records.GetCount(); index++)
	{
		CStringArray params;
		CSupString record = records.GetAt(index);
      record.ParseQuote(params, ":");

		if (params.GetCount() > 0)
		{
			CString pinRef = params.GetAt(0);
         CString pinName;
         if (params.GetCount() > 1)
			   pinName = params.GetAt(1);

			pinRef.Trim();
			pinName.Trim();

         // Add entry.
         // Some processing may change upper/lower case, so make map based on lower case pinRef.
         pinRef.MakeLower();
			pinRefToPinNameMap.SetAt(pinRef, pinName);
		}
	}
}

int COdbStep::getLayerNumberWithType(LayerTypeTag layerType)
{
   for (int indx = 0; indx < m_layerArray.GetSize(); indx++)
   {
      COdbLayer* layer = m_layerArray.GetAt(indx);

      if (layer != NULL && layer->getCCZLayerType() == layerType)
      {
         return indx;
      }
   }

   return -1;
}

void COdbStep::readStep(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log)
{
   empty();

   displayProgress(log,"Reading data for step '%s'",(const char*)m_name);

   // The order of these two is important, must read eda/data first so we have
   // the number of pkgs there, which gets set as an offset when reading vpl_pkgs.
   readEdaData(odbPpDatabase, artworkModeFlag, log);
   readEdaVplPkgs(odbPpDatabase, artworkModeFlag, log);

   GenerateRealParts(odbPpDatabase, m_edaDataPackages, log);
   GenerateRealParts(odbPpDatabase, m_edaVplPackages,  log);

   bool debugOutputFlag = true;

   if (debugOutputFlag && !artworkModeFlag && getOdbOptions().getOptionFlag(optionEnableDumpOutput))
   {
      displayProgress(log,"Dumping COdbStep::m_packages.");
      m_edaDataPackages.dump(log);
      m_edaVplPackages.dump(log);
   }

   readLayers(odbPpDatabase,artworkModeFlag,log);
   readStepHeader(odbPpDatabase,*this,log);
   odbPpDatabase.readSymbolFeatures(log);
   readBom(odbPpDatabase,artworkModeFlag,log);
}

bool COdbStep::readBom(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log)
{
   bool retval = true; // Optimist

   CString selectedBomName( getAffectingBomName() );  // Here we start with name from stephdr, possibly it is blank.

   if (selectedBomName.IsEmpty())
   {
      // No affecting BOM name in stephdr. Try a BOM named after the step.
      selectedBomName = this->getName();
   }
   
   if (!selectedBomName.IsEmpty())
   {
      // Read Affecting BOM, or whatever BOM we decided upon.
      // Note that there could be other BOMs, they are not relevant today. Perhaps someday
      // we'll read all BOMs and set up Variants, but that is not part of initial implementation.

      CFilePath bomFilePath = odbPpDatabase.getDatabaseFilePath();
      bomFilePath.pushLeaf("steps");
      bomFilePath.pushLeaf(getName());
      bomFilePath.pushLeaf("boms");
      bomFilePath.pushLeaf(selectedBomName);
      bomFilePath.pushLeaf("bom");

      updateToExistingCompressableFilePath(bomFilePath,log);

      CString bomFilePathStr( bomFilePath.getPath() );

      if (!fileExists(bomFilePathStr))
      {
         if (!getAffectingBomName().IsEmpty())
         {
            log.writef(PrefixStatus,
               "Skipping BOM. Designated Affecting BOM file does not exist.'%s'\n",
               (const char*)bomFilePath.getPath());
         }
         else
         {
            log.writef(PrefixStatus,
               "Skipping BOM. No designated Affecting BOM is set in stephdr, and default BOM not found.'%s'\n",
               (const char*)bomFilePath.getPath());
         }
         return false;
      }
      else if (artworkModeFlag)
      {
         log.writef(PrefixStatus,
            "Artwork Mode: Ignoring bom data file.'%s'\n",
            (const char*)bomFilePath.getPath());
         return false;
      }
      else
      {
         COdbAppFileReader fileReader;

         if (fileReader.openFile(bomFilePath.getPath()) != statusSucceeded)
         {
            log.writef(PrefixError,
               "Could not read the BOM file, '%s' - status = '%s'.\n",
               (const char*)bomFilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));
            return false;
         }

         // Read the file.
         displayProgress(log,"Reading BOM file, '%s'.",(const char*)bomFilePath.getPath());

         CString line;
         int lineNumber;
         int numParams;

         CFileReadProgress fileReadProgress(fileReader);

         bool inAliasHeader = false;
         bool inRdCpnSection = false;
         bool inCpnMpnSection = false;
         bool inCpSection = false;

         COdbBomCp *currentCp = NULL;
         COdbBomRdCpn *currentRdCpn = NULL;
         int currentOdbIndex = 0; // ODB++ Index starts at 1

         while (fileReader.readFileRecord(&log) == statusSucceeded)
         {
            lineNumber = fileReader.getNumRecordsRead();
            line       = fileReader.getRecord();
            numParams  = fileReader.getNumParams();

            if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

            if (numParams > 0)
            {
               CString param0 = fileReader.getParam(0);

               OdbTokenTag token = stringToOdbToken(fileReader.getParam(0));

               switch (token)
               {
               case tokDESC_ALIASES:
                  {
                     inAliasHeader = true;
                  }
                  break;
               case tokDESC_ALIASES_END:
                  {
                     inAliasHeader = false;
                  }
                  break;
               case tokRD_CPN:
                  {
                     inRdCpnSection = true;
                  }
                  break;
               case tokRD_CPN_END:
                  {
                     inRdCpnSection = false;
                  }
                  break;
               case tokCPN_MPN:
                  {
                     inCpnMpnSection = true;
                  }
                  break;
               case tokCPN_MPN_END:
                  {
                     inCpnMpnSection = false;
                  }
                  break;
               case tokCP:
                  {
                     inCpSection = true;
                  }
                  break;
               case tokCP_END:
                  {
                     inCpSection = false;
                  }
                  break;

               case tokCPN:
                  {
                     if (numParams > 1)
                     {
                        CString val = fileReader.getParamToEndOfLine(1);
                        if (inCpSection)
                        {
                           currentCp = new COdbBomCp(val);
                           getBom().AddCp(currentCp);
                        }
                        else if (inRdCpnSection && currentRdCpn != NULL)
                        {
                           currentRdCpn->SetCPN(val);
                        }
                        else if (inAliasHeader)
                        {
                           // There will be two CPN's per INDEX. First one is potentially non-English, second
                           // is always English. We don't try to filter them here, just send both. The
                           // non-English will end up ignored, the English will cause a setting.
                           this->getBom().SetDescriptionIndex(val, currentOdbIndex);
                        }
                     }
                  }
                  break;

               case tokINDEX:
                  {
                     if (numParams > 1)
                        currentOdbIndex = atoi(fileReader.getParam(1));
                  }
                  break;

               case tokIPN:
                  {
                     if (numParams > 1)
                     {
                        CString ipn = fileReader.getParamToEndOfLine(1);
                        if (inCpSection && currentCp != NULL)
                        {
                           currentCp->SetIPN(ipn);
                        }
                     }
                  }
                  break;

               case tokDSC:
                  {
                     if (inCpSection && currentCp != NULL)
                     {
                        // The fileReader will have parsed the line into separate fields, based on
                        // space as delimiter. Put them all back together as one string for this value.
                        CString dsc;
                        if (numParams > 1)
                        {
                           dsc = fileReader.getParamToEndOfLine(1);
                        }
                        currentCp->AddDsc(dsc);
                     }
                  }
                  break;
               case tokREF:
                  {
                     if (inRdCpnSection && numParams > 1)
                     {
                        CString refdes(fileReader.getParam(1));
                        currentRdCpn = new COdbBomRdCpn(refdes);
                        getBom().AddRdCpn(currentRdCpn);
                     }
                  }
                  break;
               case tokNO_POP:
                  {
                     if (inRdCpnSection && numParams > 1 && currentRdCpn != NULL)
                     {
                        CString nopopStr(fileReader.getParam(1));
                        int nopop = atoi(nopopStr); // ODB++ value is an integer (to represent a boolean)
                        currentRdCpn->SetNoPop(nopop?true:false);
                     }
                  }
                  break;
               default:
                  {
                     // Ignore it.
                  }
                  break;

               }

            }
         }

         // Finished
         fileReader.close();
         fileReader.writeReadStatistics(&log,PrefixStatus);
         hideProgress();
      }
   }

   return true;
}

void COdbStep::GenerateRealParts(COdbPpDatabase& odbPpDatabase, COdbPackages &odbPkgList, CWriteFormat& log)
{
   int numPkgs = odbPkgList.getNumPackages();

   // Convenient handle on the CCZ DB
   CCamCadDatabase &ccdb = odbPpDatabase.getCamCadDatabase();

   // Attrib kw index for CreatedBy attribute.
   int createdByKeywordIndex = ccdb.registerKeyword(ATT_CREATED_BY, valueTypeString);

   // Define and get index for layer for outline poly. Define both top and bottom, and
   // mirror them, but we create geometry using top only.
   int packageBodyOutlineTopLayerIndex = ccdb.getDefinedLayerIndex(RP_LAYER_PKG_BODY_TOP,    false, layerTypePackageBodyTop);
   int packageBodyOutlineBotLayerIndex = ccdb.getDefinedLayerIndex(RP_LAYER_PKG_BODY_BOTTOM, false, layerTypePackageBodyBottom);
   ccdb.mirrorLayers(packageBodyOutlineTopLayerIndex, packageBodyOutlineBotLayerIndex);

   // These get used only when we turn on the as-is vpl realpart generation.
   int packagePinOutlineTopLayerIndex = ccdb.getDefinedLayerIndex(RP_LAYER_PKG_PIN_TOP,    false, layerTypePackagePinFootTop);
   int packagePinOutlineBotLayerIndex = ccdb.getDefinedLayerIndex(RP_LAYER_PKG_PIN_BOTTOM, false, layerTypePackagePinFootBottom);
   ccdb.mirrorLayers(packagePinOutlineTopLayerIndex, packagePinOutlineBotLayerIndex);

   for (int i = 0; i < numPkgs; i++)
   {
      COdbPackage *vplPkg = odbPkgList.getAtRawIndex(i);

      int odbIndex = i + odbPkgList.getPkgIndxOffset();  // Accounts for indexing continuing across data and vpl_pkgs files.

      CString originalName( vplPkg->getOriginalName() );   // This name came from CAD, may or may not be same as validated name.
      //CString validatedName( vplPkg->getValidatedName() ); // This is the name seen in CCZ.

      CString realpartGeomName;
      realpartGeomName.Format("RP_%d", odbIndex);

      // Get existing RealPart geom
      BlockStruct *realpartGeom = ccdb.getBlock(realpartGeomName, -1);

      if (realpartGeom == NULL)
      {
         // RealPart Geom does not exist, create it.
         int fileNumber = this->getFileNum();
         realpartGeom = ccdb.getNewBlock(realpartGeomName, "-%d", blockTypeRealPart, fileNumber);
         CString actualRpName( realpartGeom->getName() );

         // Set the CreatedBy attribute.
         realpartGeom->getDefinedAttributes()->setAttribute(ccdb.getCamCadData(), createdByKeywordIndex, "CAMCAD ODB++ Import", attributeUpdateOverwrite);

         // Create Real Part Package geom
         CString realpartPackageGeomName;
         realpartPackageGeomName.Format("%s_PKG", realpartGeomName);
         BlockStruct *realpartPackageGeom = ccdb.getNewBlock(realpartPackageGeomName, "-%d", blockTypePackage, fileNumber);
         CString actualRpPkgName( realpartPackageGeom->getName() );

         // Put insert of RealPart Package into Real Part geom
         ccdb.referenceBlock(realpartGeom, realpartPackageGeom, insertTypePackage, "Package", -1);

         // Add vpl package outline as body outline
#ifdef THIS_ADDS_ORIGINAL_VPL_OUTLINE_AS_IS
         if (false)
         {
            COdbOutline *vplPkgOutline = vplPkg->getOutline();
            if (vplPkgOutline == NULL)
            {
               // Punt
               DataStruct *outlinePolyStruct = ccdb.addPolyStruct(realpartPackageGeom, packageBodyOutlineTopLayerIndex, 0, FALSE, graphicClassPackageBody);
               CPoly *outlinePoly = ccdb.addPoly(outlinePolyStruct, ccdb.getZeroWidthIndex(), false, false, true);

               double coord = 0.010;
               outlinePoly->addVertex(-coord,-coord);
               outlinePoly->addVertex( coord,-coord);
               outlinePoly->addVertex( coord, coord);
               outlinePoly->addVertex(-coord, coord);
               outlinePoly->addVertex(-coord,-coord);
            }
            else
            {
               // This is original outline as defined in ODB++
               OutlineTypeTag outlineType = vplPkgOutline->getOutlineType();
               vplPkgOutline->setGraphicClass(graphicClassPackageBody);
               vplPkgOutline->buildCamCadData(odbPpDatabase, packageBodyOutlineTopLayerIndex, realpartPackageGeom);
            }
         }
#endif

         // Add the pins, this code is kept so we can re-activate anytime, but in actual product we
         // do not want the separate pin outlines in the realpart, they are considered a portion of
         // Valor IP, to be protected.
#ifdef ADD_ORIGINAL_VPL_PIN_OUTLINES_AS_IS
         // This should be used for debug purposes only, this code should not be
         // active in a release build.

         if (false)
         {
            for (int pinI = 0; pinI < vplPkg->getNumPins(); pinI++)
            {
               COdbPackagePin *pkgPin = vplPkg->getPinAt(pinI);
               CString pinName( pkgPin->getPinName() );

               // Create Real Part Package PIN geom
               CString realpartPackagePinGeomName;
               realpartPackagePinGeomName.Format("%s_PKG_PIN_%s", realpartGeomName, pinName);
               BlockStruct *realpartPackagePinGeom = ccdb.getNewBlock(realpartPackagePinGeomName, "-%d", blockTypePackagePin, fileNumber);
               CString actualRpPkgPinName( realpartPackagePinGeom->getName() );

               // Put insert of RealPart Package Pin into Real Part Package geom.
               // This is a little odd because we are inserting the pin at 0,0 and then letting the
               // outline be shifted to the right place for the pin. Really this is probably because that is how
               // the original ODB data is. In any case, we are not using this for anything other than illustration and
               // validation (during development) and is not part of the product for users, so this is good enough.
               ccdb.referenceBlock(realpartPackageGeom, realpartPackagePinGeom, insertTypePackagePin, pinName, -1);

               // Define the outline
               COdbOutline *pinOutline = pkgPin->getOutline();
               if (pinOutline != NULL)
               {
                  pinOutline->buildCamCadData(odbPpDatabase, packagePinOutlineTopLayerIndex, realpartPackagePinGeom);
               }
            }
         }
#endif

         // Create an outline that is a rubberband around body and pins.
         if (true)
         {
            DataStruct *outlinePolyStruct = ccdb.addPolyStruct(realpartPackageGeom, packageBodyOutlineTopLayerIndex, 0, FALSE, graphicClassPackageBody);
            CPoly *outlinePoly = ccdb.addPoly(outlinePolyStruct, ccdb.getZeroWidthIndex(), false, false, true);
            if (outlinePoly != NULL)
            {
               vplPkg->fillDftOutlinePoly(odbPpDatabase, *outlinePoly);

               // Set outline method to Custom, since we have made a custom poly.
               CString value = outlineAlgTagToString(algCustom);
               int outlineMethodKW = ccdb.getCamCadDoc().RegisterKeyWord(ATT_OUTLINE_METHOD, 0, valueTypeString);
               ccdb.getCamCadDoc().SetAttrib(&outlinePolyStruct->getAttributesRef(), outlineMethodKW, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
            }
         }

      }
   }
}

bool COdbStep::readEdaVplPkgs(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log)
{
   CFilePath edaFilePath = odbPpDatabase.getDatabaseFilePath();
   edaFilePath.pushLeaf("steps");
   edaFilePath.pushLeaf(getName());
   edaFilePath.pushLeaf("eda");
   edaFilePath.pushLeaf("vpl_pkgs");

   CString vplPkgsFilePath(edaFilePath.getPath());

   bool retval = true;

   // Index offset is equal to number of pkgs in eda/data
   this->m_edaVplPackages.setPkgIndexOffset( this->m_edaDataPackages.getNumPackages() );

   // For contour aka generaly poly outline support
   bool countourOutlineFlag = false;
   OdbTokenTag holeToken = tok_OdbTokenTagUndefined;
   COdbPolygon* polygon = NULL;

   if (fileExists(vplPkgsFilePath))
   {
#ifdef EXPERIMENTAL_VPL
      CString vplContent;
      int val = getVplPkgsFileContent(vplPkgsFilePath, vplContent);

      // For debug purposes ONLY !!!
      //if (vplContent.GetLength() > 0)
      //{
      //   //*rcf Temp -- Echo vpl_pkgs to log file.
      //   log.write("\n\n---------------VPL CONTENT-----------------\n");
      //   log.writef("%s", vplContent);
      //   log.write("\n---------------VPL CONTENT-----------------\n\n");
      //}

      CSupString vplContentSupStr(vplContent);
      CStringArray vplContentLines;
      int pq = vplContentSupStr.ParseQuote(vplContentLines, "\n");
      if (vplContentLines.GetCount() > 0)
      {
         bool inHeader = false;
         int vplValorCnt = 0;
         int vplUserCnt = 0;
         int nonVplCnt = 0;

         COdbPackage    *package = NULL;
         COdbPackagePin *pin     = NULL;

         for (int lineNumber = 0; lineNumber < vplContentLines.GetCount(); lineNumber++)
         {
            CString vplLine( vplContentLines.GetAt(lineNumber) );

            CSupString vplLineSupStr( vplLine );
            CStringArray vplFields;
            int numParams = vplLineSupStr.ParseQuote(vplFields, " ");

            CString field1;
            if (vplFields.GetCount() > 0)
            {
               field1 = vplFields.GetAt(0);
            }

            if (field1.CompareNoCase("HEADER") == 0)
            {
               // Found header
               inHeader = true;
            }
            else if (field1.CompareNoCase("}") == 0)
            {
               if (inHeader)
                  inHeader = false;
            }
            else if (field1.CompareNoCase("VPL_VALOR:") == 0)
            {
               if (inHeader && vplFields.GetCount() > 1)
                  vplValorCnt = atoi(vplFields.GetAt(1));
            }
            else
            {

               OdbTokenTag token = stringToOdbToken(field1);

               switch (token)
               {
               case tokPkg:
                  {
                     // Reset
                     package = NULL;
                     pin = NULL;

                     if (numParams < 7)
                     {
                        log.writef(PrefixError,
                           "Not enough parameters for '%s' record, in '%s', line number %d\n",
                           field1.GetBuffer(0), (const char*)edaFilePath.getPath(), lineNumber);
                     }
                     else
                     {
                        CString packageName    =      vplFields.GetAt(1);
                        double pinPitch        = atof(vplFields.GetAt(2));
                        double boundingBoxXmin = atof(vplFields.GetAt(3));
                        double boundingBoxYmin = atof(vplFields.GetAt(4));
                        double boundingBoxXmax = atof(vplFields.GetAt(5));
                        double boundingBoxYmax = atof(vplFields.GetAt(6));

                        // Check for package name that has binary characters and rename it to a valid name.
                        CString validatedPackageName = checkInvalidPackageName(packageName, m_edaVplPackages.getNumPackages(), log);

                        package = m_edaVplPackages.addPackage(packageName, validatedPackageName, pinPitch, 
                           boundingBoxXmin, boundingBoxYmin,
                           boundingBoxXmax, boundingBoxYmax);
                     }

                  }
                  break;
               case tokPin:
                  {
                     if (package == NULL)
                     {
                        log.writef(PrefixError,
                           "PIN record encountered without a preceding PKG record '%s'.%d.\n",
                           (const char*)edaFilePath.getPath(), lineNumber);
                     }
                     else if (numParams < 6)
                     {
                        log.writef(PrefixError,
                           "Not enough parameters for '%s' record, in '%s'.%d\n",
                           field1, (const char*)edaFilePath.getPath(), lineNumber);
                     }
                     else
                     {
                        CString pinName = vplFields.GetAt(1);
                        PackagePinTypeTag pinType = stringToPackagePinTypeTag(vplFields.GetAt(2));
                        double x = atof(vplFields.GetAt(3));
                        double y = atof(vplFields.GetAt(4));
                        double fhs = atof(vplFields.GetAt(5));
                        PackagePinElectricalTypeTag electricalType = stringToPackagePinElectricalTypeTag((numParams > 6 ? vplFields.GetAt(6) : "U"));
                        PackagePinMountTypeTag mountType = stringToPackagePinMountTypeTag((numParams > 7 ? vplFields.GetAt(7) : "U"));

                        pin = package->addPin(pinName, pinType, x, y, fhs, electricalType, mountType);
                     }

                  }
                  break;
               case tok_AtPinExt:
                  {
                     // Not currently using @PIN_EXT, but basic parsing is in place anyway.
                     // We only saw "FORM" in field 3, I suspect there can be others.
                     // Perhaps there can be multiple @PIN_EXT records, don't know.
                     // So for now we parse, check for "FORM", if it is "FORM" then we
                     // save it, otherwise we skip it.

                     if (pin != NULL && numParams > 3) // Expecting 4 fields, per example data.
                     {
                        CString pinextKW( vplFields.GetAt(0) );
                        CString pinextApparantlyIsPinName( vplFields.GetAt(1) );
                        CString pinextFormKW( vplFields.GetAt(2) );
                        CString pinextFormValue( vplFields.GetAt(3) );

                        if (pinextFormKW.CompareNoCase("FORM") == 0)
                           pin->setVplForm(pinextFormValue);
                     }
                  }
                  break;
               case tokSq:  // square outline record
                  if (package == NULL)
                  {
                     log.writef(PrefixError,
                        "Outline record encountered without a preceding package record '%s'.%d .\n",
                        (const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else if (numParams < 4)
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record, in '%s'.%d\n",
                        field1, (const char*)edaFilePath.getPath(), lineNumber);
                  }
                  else
                  {
                     double x = atof(vplFields.GetAt(1));
                     double y = atof(vplFields.GetAt(2));
                     double halfSide = atof(vplFields.GetAt(3));

                     if (pin != NULL)
                     {
                        pin->addSquareOutline(x,y,halfSide);
                     }
                     else
                     {
                        package->addSquareOutline(x,y,halfSide);
                     }
                  }

                  break;
               case tokRc:
                  {
                     if (package == NULL)
                     {
                        log.writef(PrefixError,
                           "Outline record encountered without a preceding package record '%s'.%d .\n",
                           (const char*)edaFilePath.getPath(), lineNumber);
                     }
                     else if (numParams < 5)
                     {
                        log.writef(PrefixError,
                           "Not enough parameters for '%s' record, in '%s'.%d\n",
                           field1, (const char*)edaFilePath.getPath(), lineNumber);
                     }
                     else
                     {
                        double lowerLeftX = atof(vplFields.GetAt(1));
                        double lowerLeftY = atof(vplFields.GetAt(2));
                        double sizeX = atof(vplFields.GetAt(3));
                        double sizeY = atof(vplFields.GetAt(4));

                        if (pin != NULL)
                        {
                           pin->addRectangleOutline(lowerLeftX, lowerLeftY, sizeX, sizeY);
                        }
                        else
                        {
                           package->addRectangleOutline(lowerLeftX, lowerLeftY, sizeX, sizeY);
                        }
                     }
                  }
                  break;
               case tokCr:  // circle outline record
                  if (package == NULL)
                  {
                     log.writef(PrefixError,
                        "Outline record encountered without a preceding package record '%s'.%d .\n",
                        (const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else if (numParams < 4)
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record, in '%s'.%d\n",
                        field1,(const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else
                  {
                     double xCenter = atof(vplFields.GetAt(1));
                     double yCenter = atof(vplFields.GetAt(2));
                     double radius = atof(vplFields.GetAt(3));

                     if (pin != NULL)
                     {
                        pin->addCircleOutline(xCenter,yCenter,radius);
                     }
                     else
                     {
                        package->addCircleOutline(xCenter,yCenter,radius);
                     }
                  }

                  break;

               case tokCt:  // countour outline record
                  if (package == NULL)
                  {
                     log.writef(PrefixError,
                        "Outline record encountered without a preceding package or pin record '%s'.%d .\n",
                        (const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else
                  {
                     countourOutlineFlag = true;
                  }

                  break;
               case tokOb:  // countour outline record - start of polygon
                  if (!countourOutlineFlag)
                  {
                     log.writef(PrefixError,
                        "Contour record encountered without a preceding CT outline record '%s'.%d .\n",
                        (const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else if (numParams < 4)
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record, in '%s'.%d\n",
                        field1,(const char*)edaFilePath.getPath(),lineNumber);
                  }
                  //            else if (pin != NULL)  // ignore contours on pins
                  //            {
                  //                uniqueErrorFilter.writef(log,PrefixWarning,edaFilePath.getPath(),lineNumber,line,
                  //                "Contours on Pins in EDA files are ignored");
                  //            }
                  else
                  {
                     double x = atof(vplFields.GetAt(1));
                     double y = atof(vplFields.GetAt(2));
                     holeToken = stringToOdbToken(vplFields.GetAt(3));

                     //fileReader.getParamInPageUnits( x,1);
                     //fileReader.getParamInPageUnits( y,2);
                     //holeToken = stringToOdbToken(fileReader.getParam(3));

                     if (pin != NULL)
                     {
                        polygon = pin->addContourOutline(x,y,holeToken == tokH);
                     }
                     else
                     {
                        polygon = package->addContourOutline(x,y,holeToken == tokH);
                     }
                  }

                  break;
               case tokOs:  // countour outline record - segment point
                  if (polygon == NULL)
                  {
                     //if (pin == NULL)
                     //{
                     log.writef(PrefixError,
                        "Polygon record encountered without a preceding OB record '%s'.%d .\n",
                        (const char*)edaFilePath.getPath(),lineNumber);
                     //}
                  }
                  else if (numParams < 3)
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record, in '%s'.%d\n",
                        field1,(const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else
                  {
                     double x = atof(vplFields.GetAt(1));
                     double y = atof(vplFields.GetAt(2));

                     polygon->addSegment(x,y);
                  }

                  break;
               case tokOc:  // countour outline record - curve point
                  if (polygon == NULL)
                  {
                     //if (pin == NULL)
                     //{
                     log.writef(PrefixError,
                        "Polygon record encountered without a preceding OB record '%s'.%d .\n",
                        (const char*)edaFilePath.getPath(),lineNumber);
                     //}
                  }
                  else if (numParams < 6)
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record, in '%s'.%d\n",
                        field1,(const char*)edaFilePath.getPath(),lineNumber);
                  }
                  else
                  {
                     double x = atof(vplFields.GetAt(1));
                     double y = atof(vplFields.GetAt(2));
                     double cx = atof(vplFields.GetAt(3));
                     double cy = atof(vplFields.GetAt(4));

                     OdbTokenTag cwToken = stringToOdbToken(vplFields.GetAt(5));

                     polygon->addCurve(x,y,cx,cy,cwToken == tokY);
                  }

                  break;
               case tokOe:  // countour outline record - end of polygon
                  polygon = NULL;

                  break;
               case tokCe:  // countour outline record - end of contour
                  countourOutlineFlag = false;

                  break;


               default:
                  {
                     // no op
                  }
                  break;
               }

            }
         }
      }
#endif
   }

   return retval;
}

bool COdbStep::readEdaData(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log)
{
   CFilePath edaFilePath = odbPpDatabase.getDatabaseFilePath();
   edaFilePath.pushLeaf("steps");
   edaFilePath.pushLeaf(getName());
   edaFilePath.pushLeaf("eda");
   edaFilePath.pushLeaf("data");

   bool retval = true;
   updateToExistingCompressableFilePath(edaFilePath,log);
   //bool retval = getOdbCompressableFileCache().uncompress(edaFilePath.getPath(),log);

   while (retval)
   {
	   if (artworkModeFlag)
	   {
		   log.writef(PrefixStatus,
			   "Artwork Mode: Ignoring eda data file.'%s'\n",
			   (const char*)edaFilePath.getPath());

		   break;
	   }

	   COdbAppFileReader fileReader;

	   if (fileReader.openFile(edaFilePath.getPath()) != statusSucceeded)
	   {
		   log.writef(PrefixError,
			   "Could not read the EDA file, '%s' - status = '%s'.\n",
			   (const char*)edaFilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));

		   break;
	   }

      fileReader.setInchesPerUnit(odbPpDatabase.getInchesPerUnit());
      fileReader.setPageUnitsPerUnit(odbPpDatabase.getPageUnitsPerUnit());

      displayProgress(log,"Reading EDA file, '%s'.",(const char*)edaFilePath.getPath());

      CString line;
      int lineNumber;
      int numParams;

      CString netName;
      bool fgrFlag = false;
      BlockStruct* padBlock = NULL;
      DataStruct* contourPolyStruct = NULL;
      CPoly* countourPoly = NULL;
      int complexPadShapeIndex = 0;
      int padLayerIndex = 0;
      int contourLayerIndex = 0;
      CPnt* lastVertex = NULL;

      OdbTokenTag subnetPortionToken = tok_OdbTokenTagUndefined;
      OdbTokenTag pinTypeToken       = tok_OdbTokenTagUndefined;
      OdbTokenTag holeToken          = tok_OdbTokenTagUndefined;
      bool subnetTopSideFlag = true;
      int subnetCompNumber = 0;
      int subnetCompPinNumber = 0;
      int subnetViaCount = 0;
      CString subnetPlaneFillType;
      CString subnetPlaneCutoutType;
      CString subnetPlaneFillSize;

      m_layerArray.RemoveAll();
      COdbPackage* package = NULL;
      COdbNet* net = NULL;
      COdbPackagePin* pin = NULL;
      COdbSubnet* subnet = NULL;
      COdbPolygon* polygon = NULL;
      bool featureGroupFlag = false;
      bool countourOutlineFlag = false;

      CFileReadProgress fileReadProgress(fileReader);

      while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         lineNumber = fileReader.getNumRecordsRead();
         line       = fileReader.getRecord();
         numParams  = fileReader.getNumParams();

         if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

         if (numParams < 1)
         {
            continue;
         }

         CString param0 = fileReader.getParam(0);
         const char* recordType0 = param0.GetBuffer();

         if (*recordType0 == '#')
         {
            if (param0.GetLength() > 1)
            {
               recordType0++;

               unsigned int recordCode =  getRecordCode(recordType0);

               if (recordCode == RecordCode1(@))
               {
                  if (numParams > 1)
                  {
                     int serialNumber = atoi(recordType0 + 1);
                     CString name = fileReader.getParam(1);

                     if (m_netAttributes.getAttributeNameCount() != serialNumber)
                     {
                        log.writef(PrefixError,
                           "Net attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                           m_netAttributes.getAttributeNameCount(),serialNumber,
                           (const char*)edaFilePath.getPath(),lineNumber);
                     }

                     m_netAttributes.addAttributeName(name);

                     uniqueErrorFilter.writef(log,PrefixWarning,edaFilePath.getPath(),lineNumber,line,
                        "Net Attributes in EDA files are ignored");
                  }
               }
               else if (recordCode == RecordCode1(&))
               {
                  if (numParams > 1)
                  {
                     int serialNumber = atoi(recordType0 + 1);
                     CString textString = fileReader.getParam(1);

                     if (m_netAttributes.getAttributeTextStringCount() != serialNumber)
                     {
                        log.writef(PrefixError,
                           "Net attribute name sequence error - count=%d, serialNumber=%d - '%s'.%d .\n",
                           m_netAttributes.getAttributeTextStringCount(),serialNumber,
                           (const char*)edaFilePath.getPath(),lineNumber);
                     }

                     m_netAttributes.addAttributeTextString(textString);

                     uniqueErrorFilter.writef(log,PrefixWarning,edaFilePath.getPath(),lineNumber,line,
                        "Net Attributes in EDA files are ignored");
                  }
               }
            }

            continue;
         }

         OdbTokenTag token = stringToOdbToken(fileReader.getParam(0));

         switch (token)
         {
         case tokHdr:  // file header 
            break;
         case tokLyr:  // layer names
            for (int ind = 1;ind < numParams;ind++)
            {
               CString layerName = fileReader.getParam(ind);
               COdbLayer* layer = odbPpDatabase.getLayerArray().getAt(layerName);

               if (layer == NULL)
               {
                  log.writef(PrefixDisaster,
                     "Could not find layer for layerName='%s'.  Record='%s' in '%s'.%d\n",
                     (const char*)layerName,(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
               }

               m_layerArray.SetAtGrow(ind - 1,layer);
            }

            break;
         case tokNet:  // electrical net record
            featureGroupFlag = false;
            subnet  = NULL;
            package = NULL;

            if (numParams > 1)
            {
               fileReader.getParam(netName,1);
               OdbPpIn_TranslateOdbNetName(netName);

               net = m_netArray.addNet(netName);
            }

            break;
         case tokSnt:  // Subnet record
            if (net == NULL)
            {
               log.writef(PrefixError,
                  "SNT encountered befor NET.  Record='%s' in '%s'.%d\n",
                  (const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else if (numParams < 2)
            {
               log.writef(PrefixError,
                  "Not enough parameters for SNT record.  Record='%s' in '%s'.%d\n",
                  (const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               subnetPortionToken = stringToOdbToken(fileReader.getParam(1));

               switch (subnetPortionToken)
               {
               case tokTop:  // toeprint
                  if (numParams >= 5)
                  {
                     int subnetCompNumber,subnetCompPinNumber;

                     subnetTopSideFlag = (fileReader.getParam(2) == "T");
                     fileReader.getParam(subnetCompNumber    ,3);
                     fileReader.getParam(subnetCompPinNumber ,4);

                     subnet = net->addToeprintSubnet(subnetTopSideFlag,subnetCompNumber,subnetCompPinNumber);
                  }
                  else
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                        (const char*)fileReader.getParam(1),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
                  }

                  break;
               case tokVia:  // via
                  if (numParams >= 2)
                  {
                     subnet = net->addViaSubnet();
                  }
                  else
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                        (const char*)fileReader.getParam(1),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
                  }

                  break;
               case tokTrc:  // trace
                  if (numParams >= 2)
                  {
                     subnet = net->addTraceSubnet();
                  }
                  else
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                        (const char*)fileReader.getParam(1),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
                  }

                  break;
               case tokPln:  // plane
                  if (numParams >= 5)
                  {
                     double fillSize;

                     PlaneFillTypeTag planeFillType = stringToPlaneFillTypeTag(fileReader.getParam(2));
                     CutoutTypeTag    cutoutType    = stringToCutoutTypeTag(fileReader.getParam(3));
                     fileReader.getParamInPageUnits(fillSize,4);

                     subnet = net->addPlaneSubnet(planeFillType,cutoutType,fillSize);
                  }
                  else
                  {
                     log.writef(PrefixError,
                        "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                        (const char*)fileReader.getParam(1),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
                  }

                  break;
               default:
                  log.writef(PrefixError,
                     "Unrecognized subnet type of '%s' encountered.  Record='%s' in '%s'.%d\n",
                     (const char*)fileReader.getParam(1),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
                  break;
               }
            }

            break;
         case tokPkg:  // package record
            featureGroupFlag = false;
            subnet  = NULL;
            package = NULL;
            pin     = NULL;

            if (numParams < 7)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(1),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               CString packageName;
               double pinPitch,boundingBoxXmin,boundingBoxYmin,boundingBoxXmax,boundingBoxYmax;

               fileReader.getParam(        packageName    ,1);
               fileReader.getParamInPageUnits(pinPitch       ,2);
               fileReader.getParamInPageUnits(boundingBoxXmin,3);
               fileReader.getParamInPageUnits(boundingBoxYmin,4);
               fileReader.getParamInPageUnits(boundingBoxXmax,5);
               fileReader.getParamInPageUnits(boundingBoxYmax,6);

               // Check for package name that has binary characters and rename it to a valid name
               CString validatedPackageName = checkInvalidPackageName(packageName, m_edaDataPackages.getNumPackages(), log);

               package = m_edaDataPackages.addPackage(packageName, validatedPackageName, pinPitch, 
                  boundingBoxXmin, boundingBoxYmin, boundingBoxXmax, boundingBoxYmax);
            }

            break;
         case tokPin:  // pin record
            if (package == NULL)
            {
               log.writef(PrefixError,
                  "PIN record encountered without a preceding PKG record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
            }
            else if (numParams < 6)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               CString pinName;
               double x,y,fhs;
               
               fileReader.getParam(pinName,1);
               PackagePinTypeTag pinType = stringToPackagePinTypeTag(fileReader.getParam(2));
               fileReader.getParamInPageUnits(x  ,3);
               fileReader.getParamInPageUnits(y  ,4);
               fileReader.getParamInPageUnits(fhs,5);
               PackagePinElectricalTypeTag electricalType = stringToPackagePinElectricalTypeTag((numParams > 6 ? fileReader.getParam(6) : "U"));
               PackagePinMountTypeTag mountType = stringToPackagePinMountTypeTag((numParams > 7 ? fileReader.getParam(7) : "U"));

               pin = package->addPin(pinName,pinType,x,y,fhs,electricalType,mountType);
            }

            break;
         case tokFgr:  // feature group record
            featureGroupFlag = true;
            subnet  = NULL;
            package = NULL;

            break;
         case tokFid:  // feature id record
            if (subnet == NULL)
            {
               if (!featureGroupFlag)
               {
                  log.writef(PrefixError,
                     "FID record encountered without a preceding subnet record '%s'.%d .\n",
                     (const char*)edaFilePath.getPath(),lineNumber);
               }
            }
            else if (numParams < 4)   
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               FeatureIdTypeTag fidType = stringToFidTypeTag(fileReader.getParam(1));
               int layerNumber,featureNumber;

               fileReader.getParam(layerNumber  ,2);
               fileReader.getParam(featureNumber,3);

               if (layerNumber < 0 || layerNumber >= m_layerArray.GetSize())
               {
                  log.writef(PrefixError,
                     "FID record - expected layer to be between 0 and %d, but %d found - '%s'.%d .\n",
                     m_layerArray.GetSize(),layerNumber,
                     (const char*)edaFilePath.getPath(),lineNumber);
               }
               else
               {
                  COdbFeatureId* fid = subnet->addFeatureId(fidType,layerNumber,featureNumber,lineNumber);
               }
            }

            break;
         case tokPrp:  // property record
            if (package == NULL)
            {
               if (!featureGroupFlag)
               {
                  uniqueErrorFilter.writef(log,PrefixWarning,edaFilePath.getPath(),lineNumber,line,
                     "Encountered PRP record for board - All board properties are ignored.");
               }
            }
            else if (numParams < 3)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               CString propertyName,propertyValue,floatingNumbers;

               fileReader.getParam(propertyName ,1);
               fileReader.getParam(propertyValue,2);
               int floatNumIndex = 3;

               if(line.Find("'") > -1)
               {
                  for(; floatNumIndex < numParams && propertyValue.GetLength() >= 2; floatNumIndex++)
                  {
                     if (propertyValue.Left(1) != "'" && propertyValue.Left(1) != "\"")
                     {
                        propertyName.Append(" " + propertyValue);
                        propertyValue = fileReader.getParam(floatNumIndex);
                     }
                     else
                        break;
                  }
               }

               if (propertyValue.GetLength() >= 2)
               {
                  if ((propertyValue.Left(1) == "'"  && propertyValue.Right(1) == "'" ) ||
                      (propertyValue.Left(1) == "\"" && propertyValue.Right(1) == "\"")    )
                  {
                     propertyValue = propertyValue.Mid(1,propertyValue.GetLength() - 2);
                  }
               }

               for (int ind = 3;ind < numParams;ind++)
               {
                  if (ind != 3) floatingNumbers += " ";

                  floatingNumbers += fileReader.getParam(ind);
               }

               COdbProperty* property = new COdbProperty(propertyName,propertyValue,floatingNumbers);

               package->addProperty(property);
            }

            break;
         case tokCr:  // circle outline record
            if (package == NULL)
            {
               log.writef(PrefixError,
                  "Outline record encountered without a preceding package record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
            }
            else if (numParams < 4)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               double xCenter,yCenter,radius;

               fileReader.getParamInPageUnits(xCenter,1);
               fileReader.getParamInPageUnits(yCenter,2);
               fileReader.getParamInPageUnits(radius ,3);

               if (pin != NULL)
               {
                  pin->addCircleOutline(xCenter,yCenter,radius);
               }
               else
               {
                  package->addCircleOutline(xCenter,yCenter,radius);
               }
            }

            break;
         case tokSq:  // square outline record
            if (package == NULL)
            {
               log.writef(PrefixError,
                  "Outline record encountered without a preceding package record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
            }
            else if (numParams < 4)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               double x,y,halfSide;

               fileReader.getParamInPageUnits(x       ,1);
               fileReader.getParamInPageUnits(y       ,2);
               fileReader.getParamInPageUnits(halfSide,3);

               if (pin != NULL)
               {
                  pin->addSquareOutline(x,y,halfSide);
               }
               else
               {
                  package->addSquareOutline(x,y,halfSide);
               }
            }

            break;
         case tokRc:  // rectangle outline record
            if (package == NULL)
            {
               log.writef(PrefixError,
                  "Outline record encountered without a preceding package record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
            }
            else if (numParams < 5)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               double lowerLeftX,lowerLeftY,width,height;

               fileReader.getParamInPageUnits(lowerLeftX,1);
               fileReader.getParamInPageUnits(lowerLeftY,2);
               fileReader.getParamInPageUnits(width     ,3);
               fileReader.getParamInPageUnits(height    ,4);

               if (pin != NULL)
               {
                  pin->addRectangleOutline(lowerLeftX,lowerLeftY,width,height);
               }
               else
               {
                  package->addRectangleOutline(lowerLeftX,lowerLeftY,width,height);
               }
            }

            break;
         case tokCt:  // countour outline record
            if (package == NULL)
            {
               log.writef(PrefixError,
                  "Outline record encountered without a preceding package or pin record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               countourOutlineFlag = true;
            }

            break;
         case tokOb:  // countour outline record - start of polygon
            if (!countourOutlineFlag)
            {
               log.writef(PrefixError,
                  "Contour record encountered without a preceding CT outline record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
            }
            else if (numParams < 4)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
//            else if (pin != NULL)  // ignore contours on pins
//            {
//                uniqueErrorFilter.writef(log,PrefixWarning,edaFilePath.getPath(),lineNumber,line,
//                "Contours on Pins in EDA files are ignored");
//            }
            else
            {
               double x,y;

               fileReader.getParamInPageUnits( x,1);
               fileReader.getParamInPageUnits( y,2);
               holeToken = stringToOdbToken(fileReader.getParam(3));

               if (pin != NULL)
               {
                  polygon = pin->addContourOutline(x,y,holeToken == tokH);
               }
               else
               {
                  polygon = package->addContourOutline(x,y,holeToken == tokH);
               }
            }

            break;
         case tokOs:  // countour outline record - segment point
            if (polygon == NULL)
            {
               //if (pin == NULL)
				//{
				log.writef(PrefixError,
					"Polygon record encountered without a preceding OB record '%s'.%d .\n",
					(const char*)edaFilePath.getPath(),lineNumber);
				//}
			}
			else if (numParams < 3)
			{
				log.writef(PrefixError,
					"Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
					(const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
			}
            else
            {
               double x,y;
               
               fileReader.getParamInPageUnits( x,1);
               fileReader.getParamInPageUnits( y,2);

               polygon->addSegment(x,y);
            }

            break;
         case tokOc:  // countour outline record - curve point
            if (polygon == NULL)
            {
               //if (pin == NULL)
               //{
               log.writef(PrefixError,
                  "Polygon record encountered without a preceding OB record '%s'.%d .\n",
                  (const char*)edaFilePath.getPath(),lineNumber);
               //}
            }
            else if (numParams < 6)
            {
               log.writef(PrefixError,
                  "Not enough parameters for '%s' record.  Record='%s' in '%s'.%d\n",
                  (const char*)fileReader.getParam(0),(const char*)line,(const char*)edaFilePath.getPath(),lineNumber);
            }
            else
            {
               double x,y,cx,cy;

               fileReader.getParamInPageUnits( x,1);
               fileReader.getParamInPageUnits( y,2);
               fileReader.getParamInPageUnits(cx,3);
               fileReader.getParamInPageUnits(cy,4);
               OdbTokenTag cwToken = stringToOdbToken(fileReader.getParam(5));

               polygon->addCurve(x,y,cx,cy,cwToken == tokY);
            }

            break;
         case tokOe:  // countour outline record - end of polygon
            polygon = NULL;

            break;
         case tokCe:  // countour outline record - end of contour
            countourOutlineFlag = false;

            break;
		 default:  
			 log.writef(PrefixError,
				 "Unrecognized eda/data section, '%s' - '%s'.%d .\n",
				 (const char*)fileReader.getParam(0),
				 (const char*)edaFilePath.getPath(),lineNumber);

			 break;
		 }
      }

      fileReader.close();
      fileReader.writeReadStatistics(&log,PrefixStatus);

      break;
   }

   hideProgress();

   return retval;
}

CString COdbStep::checkInvalidPackageName(const CString packageName, int nextPkgIndex, CWriteFormat& log)
{
   CString retval;
   bool isValidChar = true;
   for (int i=0; i<packageName.GetLength(); i++)
   {
      int upperChar = toupper(packageName.GetAt(i));
      bool isDigit = (upperChar >=48 && upperChar <= 57);  // 0 .. 9
      bool isAlpha = (upperChar >= 65 && upperChar <= 90); // A .. Z
      //             dash -              dot .            equals =      underscore  _ 
      if (!(isDigit || isAlpha ||
         upperChar == 45 ||  // dash -
         upperChar == 46 ||  // dot .
         upperChar == 61 ||  // equals =
         upperChar == 95 ||  // underscore _
         upperChar == 47)    // forward slash /   DR 887868 added this one to valid chars.
         )
      {
         isValidChar = false;
         break;
      }
   }

   if (!isValidChar)
   {
      // Generate a valid name by appending the step name and the package index (aka id) to "Geom_"
      retval.Format("Geom_%s_%03d", m_name, nextPkgIndex);
      log.writef(PrefixWarning, "Rename invalid geometry name of Step [%s] from [%s] to [%s]\n", m_name, packageName, retval);
   }
   else
   {
      retval = packageName;
   }

   return retval;
}

bool COdbStep::readLayers(COdbPpDatabase& odbPpDatabase,bool artworkModeFlag,CWriteFormat& log)
{
   bool retval = true;

   CFilePath layersFilePath = odbPpDatabase.getDatabaseFilePath();
   layersFilePath.pushLeaf("steps");
   layersFilePath.pushLeaf(getName());
   layersFilePath.pushLeaf("layers");

   if (odbPpDatabase.getLayerArray().getSize() == 0)
   {
      retval = false;

      log.writef(PrefixError,
         "No layers found in matrix file - reading of undefined layers is no longer supported.\n");
   }

   if (retval)
   {
      for (int layerIndex = 0;layerIndex < odbPpDatabase.getLayerArray().getSize();layerIndex++)
      {
         COdbLayer* layer = odbPpDatabase.getLayerArray().getAt(layerIndex);

         if (layer != NULL)
         {
            if (layer->getType() == odbLayerTypeComponent)
            {
               if (artworkModeFlag)
               {
                  log.writef(PrefixStatus,
                     "Artwork Mode: Ignoring component file '%s'\n",
                     (const char*)layersFilePath.getPath());
               }
               else
               {
                  if      (layer->getName().CompareNoCase("COMP_+_TOP") == 0)
                  {
                     m_topComponentsFile.readComponents( odbPpDatabase,*this,*layer,true,log);
                     m_topComponentsFile.readComponents2(odbPpDatabase,*this,*layer,true,log);
                     m_topComponentsFile.readComponents3(odbPpDatabase,*this,*layer,true,log);
                  }
                  else if (layer->getName().CompareNoCase("COMP_+_BOT") == 0)
                  {
                     m_bottomComponentsFile.readComponents( odbPpDatabase,*this,*layer,false,log);
                     m_bottomComponentsFile.readComponents2(odbPpDatabase,*this,*layer,false,log);
                     m_bottomComponentsFile.readComponents3(odbPpDatabase,*this,*layer,false,log);
                  }
               }
            }
            else
            {
               m_featureFileArray.readLayerFeatures(odbPpDatabase,*this,*layer,log);
            }
         }
      }
   }

   return retval;
}

bool COdbStep::readStepHeader(COdbPpDatabase& odbPpDatabase,COdbStep& step,CWriteFormat& log)
{
   CFilePath stepHeaderFilePath = odbPpDatabase.getDatabaseFilePath();
   stepHeaderFilePath.pushLeaf("steps");
   stepHeaderFilePath.pushLeaf(getName());
   stepHeaderFilePath.pushLeaf("stephdr");

   CStdioFile stepHeaderFile;

   if (! stepHeaderFile.Open(stepHeaderFilePath.getPath(),CFile::modeRead | CFile::shareDenyNone))
   {
      log.writef(PrefixError,
         "Could not open the step header file, '%s', for reading.\n",
         (const char*)stepHeaderFilePath.getPath());

      return false;
   }

   displayProgress(log,"Reading Step Header file, '%s'.",(const char*)stepHeaderFilePath.getPath());

   CSupString line;
   CStringArray params;
   int numParams;

   enum { sBegin, sStep_Repeat } state = sBegin;

   double x = 0.,y = 0.,dX = 0.,dY = 0.,angle = 0.;
   CPoint2d datumPoint,originPoint;
   CString affectingBOM;

   int nX = 0,nY = 0;
   bool mirrorFlag = false;
   bool flipFlag = false;
   CString name = getName();

   while (stepHeaderFile.ReadString(line))
   {
      numParams = line.Parse(params," =");

      if (numParams < 1)
      {
         continue;
      }

      OdbTokenTag token = paramToOdbToken(params,0);

      int iParam = 0;
      double fParam = 0.;
      CString sParam;

      if (numParams > 1)
      {
         sParam = params[1];
         iParam = atoi(sParam);
         fParam = atof(sParam);
      }

      switch (state)
      {
      case sBegin:
         switch (token)
         {
         case tok_Step_Repeat:   
            state = sStep_Repeat;
            break;
         case tokX_Datum:   
            datumPoint.x = fParam;
            break;
         case tokY_Datum:   
            datumPoint.y = fParam;
            break;
         case tokX_Origin:   
            originPoint.x = fParam;
            break;
         case tokY_Origin:   
            originPoint.y = fParam;
            break;
		 case tokAffecting_BOM:
			 affectingBOM = sParam;
			 break;
         }

         break;
      case sStep_Repeat:
         switch (token)
         {
         case tok_RightBrace:
            {
               addStepper(name,x,y,dX,dY,nX,nY,-angle,mirrorFlag,flipFlag);

               state = sBegin;
            }

            break;
         case tokName:
            name = params[1];
            break;
         case tokX:
            x = fParam;
            break;
         case tokY:
            y = fParam;
            break;
         case tokDx:
            dX = fParam;
            break;
         case tokDy:
            dY = fParam;
            break;
         case tokNx:
            nX = iParam;
            break;
         case tokNy:
            nY = iParam;
            break;
         case tokAngle:
            angle = fParam;
            break;
         case tokMirror:
            mirrorFlag = (sParam.CompareNoCase("YES") == 0);
            break;
         case tokFlip:
            flipFlag = (sParam.CompareNoCase("YES") == 0);
            break;
         }

         break;
      }
   }

   step.setDatumPoint(datumPoint);
   step.setOriginPoint(originPoint);
   step.setAffectingBomName(affectingBOM);

   return true;
}

int COdbStep::getDrillLayerIndex()
{
   if (m_drillLayerIndex == DrillLayerNotCalculated)
   {
      m_drillLayerIndex = DrillLayerUndefined;

      for (int layerIndex = 0;layerIndex < m_layerArray.GetSize();layerIndex++)
      {
         COdbLayer* layer = m_layerArray.GetAt(layerIndex);

         if (layer != NULL)
         {
            if (layer->getType() == odbLayerTypeDrill)
            {
               m_drillLayerIndex = layerIndex;
               break;
            }
         }
      }
   }

   return m_drillLayerIndex;
}

void COdbStep::addStepper(const CString& stepName,double x,double y,double dx,double dy,int nx,int ny,
   double angle,bool mirrorFlag, bool flipFlag)
{
   COdbStepper* stepper = new COdbStepper(stepName,x,y,dx,dy,nx,ny,angle,mirrorFlag, flipFlag);
   m_stepperArray.Add(stepper);
}

bool COdbStep::buildCamCadBoard(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   bool retval = false;

   displayProgress(log,"Building CamCad data for PCB '%s'.",(const char*)m_name);

   bool fidsLinked = m_netArray.linkFeatures(m_featureFileArray,m_layerArray,log);
   bool pinsLinked = m_netArray.linkComponentPins(m_topComponentsFile,m_bottomComponentsFile,log);

   if (getOdbOptions().getOptionFlag(optionEnableNcPinQuest)    || 
       getOdbOptions().getOptionFlag(optionEnablePadstackQuest) || 
       getOdbOptions().getOptionFlag(optionEnableViastackQuest) || 
       getOdbOptions().getOptionFlag(optionConvertFreePadsToVias))
   {
      loadPadTree(log);
   }

   m_netArray.instantiateVias(odbPpDatabase,*this,m_padTree,log);

   bool topPackagesLinked    = m_topComponentsFile.linkPackages(odbPpDatabase,*this,log);
   bool bottomPackagesLinked = m_bottomComponentsFile.linkPackages(odbPpDatabase,*this,log);

   bool topPadStacksAttached    = m_topComponentsFile.instantiateAndAttachPadStacks(odbPpDatabase,
                                    *this,m_padTree,log);
   bool bottomPadStacksAttached = m_bottomComponentsFile.instantiateAndAttachPadStacks(odbPpDatabase,
                                    *this,m_padTree,log);

   if (getOdbOptions().getOptionFlag(optionEnableNcPinQuest))
   {
      int numUnconnectedPins = attachPadStacksToUnconnectedPins(odbPpDatabase,log);
   }

   if(getOdbOptions().getOptionFlag(optionConvertFreePadsToVias))
   {
      convertFreePadsToVias(odbPpDatabase,log);
   }

   if (getOdbOptions().getOptionFlag(optionEnableDumpOutput))
   {
      displayProgress(log,"Dumping data for components files.");
      m_topComponentsFile.dump(log);
      m_bottomComponentsFile.dump(log);
   }

   bool topPackagesAttached     = m_topComponentsFile.createPcbComponentInserts(odbPpDatabase,*this,log);
   bool bottomPackagesAttached  = m_bottomComponentsFile.createPcbComponentInserts(odbPpDatabase,*this,log);

   bool netsBuilt = m_netArray.buildCamCadData(odbPpDatabase,*this,log);

   m_stepperArray.insertSubSteps(odbPpDatabase,*this,log);

   m_featureFileArray.instantiateFeatures(odbPpDatabase,*this,log);

   // The generatePinbLocations operation has side effect of creating the comppins for unused pins.
   // This is needed before applying the BOM, in order to get any pin mappings onto the unused
   // pins, in particular the No Connect setting gets lost if comppin does not already exist.
   int errorCount = odbPpDatabase.getCamCadData().generatePinLocations(NULL, NULL);

   // The BOM sets lots of stuff, partnumbers, etc, including Loaded flag.
   // If no BOM is present then if partnumber attributes are present then
   // set Loaded flag based on partnumber attrib. Last ditch, none of that is
   // present, default loaded to true, which works best in DataDoctor.
   if (!getBom().IsEmpty())
      ApplyBom(odbPpDatabase, log);
   else if (this->HasPartNumbers(odbPpDatabase))
      SetLoadedBasedOnPartnumber(odbPpDatabase);
   else
      this->SetLoaded(odbPpDatabase, true); // Default for no BOM, no PN in ODB components files.

   // ApplyVplPkgs must be done after all things that alter PARTNUMBER attrib, this step
   // requires the final PARTNUMBER attrib be in place. Also requires final LOADED flag
   // be in place.
   ApplyVplPkgs(odbPpDatabase, m_topComponentsFile, log);
   ApplyVplPkgs(odbPpDatabase, m_bottomComponentsFile, log);

   displayProgress(log,"Build of CamCad data for PCB '%s' completed.",(const char*)m_name);

   if (getOdbOptions().getOptionFlag(optionEnablePackagePinReport))
   {
      m_topComponentsFile.generatePackagePinReport(*this,log);
      m_bottomComponentsFile.generatePackagePinReport(*this,log);
   }

   return retval;
}

void COdbStep::loadPadTree(CWriteFormat& log)
{
   for (int layerIndex = 0;layerIndex < m_featureFileArray.getSize();layerIndex++)
   {
      COdbFeatureFile* featureFile = m_featureFileArray.getAt(layerIndex);

      if (featureFile != NULL)
      {
         //featureFile->loadUninstantiatedPads(m_padTree,log);
         featureFile->loadPads(m_padTree,log);
      }
   }
}

int COdbStep::attachPadStacksToUnconnectedPins(COdbPpDatabase& odbPpDatabase,CWriteFormat& log)
{
   int retval = 0;

   retval += m_topComponentsFile.attachPadStacksToUnconnectedPins(odbPpDatabase,*this,m_padTree,log);
   retval += m_bottomComponentsFile.attachPadStacksToUnconnectedPins(odbPpDatabase,*this,m_padTree,log);

   return retval;
}

void COdbStep::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbStep: ");
   writeFormat.writef("m_name='%s', m_column=%d\n",
      (const char*)m_name,m_column);

   if (depth != 0)
   {
      for (int ind = 0;ind < m_netArray.GetSize();ind++)
      {
         COdbNet* net = m_netArray.GetAt(ind);
         net->dump(writeFormat,depth);
      }

      m_featureFileArray.dump(writeFormat,depth);
   }

   writeFormat.popHeader();
}

bool COdbStep::convertFreePadsToVias(COdbPpDatabase& odbPpDatabase,CWriteFormat& errorLog)
{
   displayProgress(errorLog,"Converting freepads to vias.");
   COperationProgress operationProgress(m_featureFileArray.getSize());
   
   for (int layerIndex = 0;layerIndex < m_featureFileArray.getSize();layerIndex++)
   {
      COdbFeatureFile* featureFile = m_featureFileArray.getAt(layerIndex);
      operationProgress.incrementProgress();

      if (featureFile != NULL)
      {
         featureFile->convertFreePadsToVias(odbPpDatabase,*this,m_padTree,errorLog);
      }
   }

   return TRUE;
}

//_____________________________________________________________________________
void COdbStepArray::empty()
{
   m_stepArray.empty();
}

COdbStep* COdbStepArray::getAt(const CString& stepName) const
{
   CString normalizedStepName(stepName);
   normalizedStepName.MakeLower();

   COdbStep* step = m_stepArray.getAt(normalizedStepName);

   return step;
}

COdbStep* COdbStepArray::getAt(int index) const
{
   COdbStep* step = m_stepArray.getAt(index);

   return step;
}

int COdbStepArray::getSize() const
{
   return m_stepArray.getSize();
}

int COdbStepArray::AddStep(const CString& stepName,COdbStep* step)
{
   CString normalizedStepName(stepName);
   normalizedStepName.MakeLower();

   int retval = m_stepArray.add(normalizedStepName,step);

   return retval;
}

//_____________________________________________________________________________
COdbLayer::COdbLayer(const CString& name,int row, bool boardContext, OdbLayerTypeTag layerType,
      const CString& oldName, bool positivePolarity,
      const CString& startLayerName, const CString& endLayerName)
{
   m_name             = properCase(name);
   m_row              = row;
   m_boardContext     = boardContext;
   m_type             = layerType;
   m_oldName          = oldName;
   m_positivePolarity = positivePolarity;
   m_startLayerName   = startLayerName;
   m_endLayerName     = endLayerName;

   m_layerStruct      = NULL;
   m_mirrorLayer      = this;
}

void COdbLayer::setODBMirrorLayer(COdbLayer* mirrorLayer)
{
   unsetMirrorLayer();

   if (mirrorLayer != NULL)
   {
      m_mirrorLayer = NULL;

      mirrorLayer->unsetMirrorLayer();

      mirrorLayer->m_mirrorLayer = this;
      m_mirrorLayer = mirrorLayer;

      m_layerStruct->setMirroredLayerIndex( m_mirrorLayer->m_layerStruct->getLayerIndex());
      m_mirrorLayer->m_layerStruct->setMirroredLayerIndex( m_layerStruct->getLayerIndex());
   }
}

void COdbLayer::unsetMirrorLayer()
{
   if (m_mirrorLayer != NULL && m_mirrorLayer->m_mirrorLayer == this)
   {
      m_mirrorLayer->m_mirrorLayer = m_mirrorLayer;
      m_mirrorLayer->m_layerStruct->setMirroredLayerIndex( m_mirrorLayer->m_layerStruct->getLayerIndex());
   }

   m_mirrorLayer = this;
   m_layerStruct->setMirroredLayerIndex( m_layerStruct->getLayerIndex());
}

//_____________________________________________________________________________
void COdbLayerArrayWithMap::setAt(COdbLayer* layer)
{
   if (layer != NULL)
   {
      m_layers.setAt(layer->getRow(),layer->getName(),layer);
   }
}

//_____________________________________________________________________________
COdbWheelDcode::COdbWheelDcode(int dcode,const CString& symbolName)
{
   m_dcode      = dcode;
   m_symbolName = symbolName;
}

//_____________________________________________________________________________
COdbWheel::COdbWheel(const CString& wheelName)
{
   m_name = wheelName;
}

bool COdbWheel::readDcodes(const CString& dcodesFilePathString,CWriteFormat& log)
{
   bool retval = true;
   CFilePath dcodesFilePath(dcodesFilePathString);
   updateToExistingCompressableFilePath(dcodesFilePath,log);
   //bool retval = getOdbCompressableFileCache().uncompress(dcodesFilePath,log);

   while (retval)
   {
      COdbAppFileReader fileReader;

      if (fileReader.openFile(dcodesFilePath.getPath()) != statusSucceeded)
      {
         log.writef(PrefixError,
            "Could not read the Dcodes file, '%s' - status = '%s'.\n",
            (const char*)dcodesFilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));

         break;
      }

      //CStdioFile dcodesFile;

      //if (! dcodesFile.Open(dcodesFilePath,CFile::modeRead | CFile::shareDenyNone))
      //{
      //   break;
      //}

      displayProgress(log,"Reading Dcodes file, '%s'",(const char*)dcodesFilePath.getPath());

      int numParams;

      CFileReadProgress fileReadProgress(fileReader);

      for (int lineNumber=1;fileReader.readFileRecord(&log) == statusSucceeded;lineNumber++)
      {
         if (lineNumber % LineStatusUpdateInterval == 0) fileReadProgress.updateProgress();

         numParams = fileReader.getNumParams();

         if (numParams < 2)
         {
            continue;
         }

         if (fileReader.getParam(0).Left(5).CompareNoCase("dcode") == 0)
         {
            int dcode = atoi(fileReader.getParam(0).GetBuffer() + 5);
            CString symbolName = fileReader.getParam(1);

            COdbWheelDcode* wheelDcode = new COdbWheelDcode(dcode,symbolName);

            m_wheelDcodes.Add(wheelDcode);
         }
      }

      break;
   }

   hideProgress();

   return retval;
}

//_____________________________________________________________________________
COdbWheel* COdbWheels::addWheel(const CString& wheelName)
{
   COdbWheel* wheel = new COdbWheel(wheelName);
   add(wheelName,wheel);

   return wheel;
}

//_____________________________________________________________________________
CPadstackReportColumns::CPadstackReportColumns(int numLayers)
{
   m_numLayers = numLayers;
}

void CPadstackReportColumns::updateColumnWidth(PadstackReportColumnTag columnTag,int width)
{
   if (columnTag >= m_columnWidths.GetSize() || width > m_columnWidths.GetAt(columnTag))
   {
      m_columnWidths.SetAtGrow(columnTag,width);
   }
}

void CPadstackReportColumns::updateColumnWidth(int layerIndex,int width)
{
   int columnIndex = layerIndex + reportColumnLayers;

   if (columnIndex >= m_columnWidths.GetSize() || width > m_columnWidths.GetAt(columnIndex))
   {
      m_columnWidths.SetAtGrow(columnIndex,width);
   }
}

//_____________________________________________________________________________
COdbPadstack::COdbPadstack(int growBySize) : m_padArray(growBySize,true)
{
   m_count = 0;
}

void COdbPadstack::empty()
{
   m_padArray.empty();
   m_count = 0;
}

int COdbPadstack::getCount()
{
   return m_count;
}

PadstackCompareStatusTag COdbPadstack::compare(const COdbPadstack& other) const
{
   PadstackCompareStatusTag retval = padstackCompareEquivalent;

   COdbPad* pad = NULL;
   COdbPad* otherPad = NULL;

   int maxInd = max(m_padArray.GetCount(),other.m_padArray.GetCount());

   for (int ind = 0;ind < maxInd ;ind++)
   {
      pad      = (ind < m_padArray.GetCount()       ? m_padArray.GetAt(ind)       : NULL);
      otherPad = (ind < other.m_padArray.GetCount() ? other.m_padArray.GetAt(ind) : NULL);

      if (pad != NULL && otherPad != NULL)
      {
         retval = min(retval,pad->compare(*otherPad));
      }
      else if (pad != NULL || otherPad != NULL)
      {
         retval = min(retval,padstackCompareMissingLayer);
      }
   }

   return retval;
}

bool COdbPadstack::isEquivalent(const COdbPadstack& other) const
{
   bool retval = (compare(other) == padstackCompareEquivalent);

   return retval;
}

double COdbPadstack::getDrillSize(int drillLayerIndex)
{
   double drillSize = 0.;

   COdbPad* pad = getAt(drillLayerIndex);

   if (pad != NULL)
   {
      COdbFeatureSymbol* featureSymbol = pad->getFeatureSymbol();

      if (featureSymbol != NULL)
      {
         drillSize = featureSymbol->getDimensionInPageUnits(0);
      }
   }

   return drillSize;
}

COdbPad* COdbPadstack::removeAt(int index)
{
   COdbPad* pad = NULL;

   if (index >= 0 && index < m_padArray.GetSize())
   {
      pad = m_padArray.GetAt(index);

      if (pad != NULL)
      {
         m_count--;
         m_padArray.SetAt(index,NULL);
      }
   }

   return pad;
}

void COdbPadstack::setAt(int index,COdbPad* pad)
{
   if (index >= 0 && index < m_padArray.GetSize())
   {
      COdbPad* oldPad = m_padArray.GetAt(index);

      if (oldPad != NULL)
      {
         m_count--;
      }
   }

   m_padArray.setAtGrow(index,pad);

   if (pad != NULL)
   {
      m_count++;
   }
}

void COdbPadstackGeometry::updateReportWidths(CPadstackReportColumns& reportColumns)
{
   if (m_block != NULL)
   {
      int len = m_block->getName().GetLength();

      reportColumns.updateColumnWidth(reportColumnName,len);
   }

   for (int ind = 0;ind < m_padArray.GetSize();ind++)
   {
      COdbPad* pad = m_padArray.GetAt(ind);

      if (pad != NULL && pad->getFeatureSymbol() != NULL)
      {
         int layerIndex = pad->getFeatureFile()->getLayer()->getRow();
         int len = pad->getFeatureSymbol()->getName().GetLength();

         reportColumns.updateColumnWidth(layerIndex,len);
      }
   }

   reportColumns.updateColumnWidth(reportColumnMatchDescription,getMatchDescription().GetLength());
}

void COdbPadstackGeometry::writeReport(CWriteFormat& report,
   CPadstackReportColumns& reportColumns,const CString& delimiter)
{
   CString stackName;

   if (m_block != NULL)
   {
      stackName = m_block->getName();
   }

   report.writef("%-*s%s",reportColumns.getColumnWidth(reportColumnName),
      (const char*)stackName,(const char*)delimiter);

   report.writef("%*d%s",reportColumns.getColumnWidth(reportColumnUseCount),
      m_useCount,(const char*)delimiter);

   COdbPadArray padArray(10,false);
   padArray.SetSize(m_padArray.GetSize() + 1);

   for (int padIndex = 0;padIndex < m_padArray.GetSize();padIndex++)
   {
      COdbPad* pad = m_padArray.GetAt(padIndex);
      
      if (pad != NULL)
      {
         int layerIndex = pad->getFeatureFile()->getLayer()->getRow();
         padArray.SetAtGrow(layerIndex,pad);
      }
   }

   for (int layerIndex = 1;layerIndex < reportColumns.getNumLayers();layerIndex++)
   {
      COdbPad* pad = NULL;
      
      if (layerIndex < padArray.GetSize())
      {
         pad = padArray.GetAt(layerIndex);
      }

      CString padName;

      if (pad != NULL && pad->getFeatureSymbol() != NULL)
      {
         padName = pad->getFeatureSymbol()->getName();
      }

      report.writef("%-*s%s",reportColumns.getColumnWidth(layerIndex),
         (const char*)padName,(const char*)delimiter);
   }

   report.writef("%-*s%s",reportColumns.getColumnWidth(reportColumnMatchDescription),
      (const char*)getMatchDescription(),(const char*)delimiter);
}

void COdbPadstack::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("COdbPadstack: ");
   writeFormat.writef("m_padArray.getSize()=%d\n",getSize());

   if (depth != 0)
   {
      for (int ind = 0;ind < getSize();ind++)
      {
         COdbPad* pad = getAt(ind);

         if (pad != NULL)
         {
            CString header;
            header.Format("[%d]:",ind);
            writeFormat.pushHeader(header);

            pad->dump(writeFormat,depth);

            writeFormat.popHeader();
         }
      }
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
COdbPadstackTemplate::COdbPadstackTemplate() : 
   COdbPadstack(20), 
   m_origin(0.,0.)
{
}

bool COdbPadstackTemplate::add(COdbPad* featurePad,int layerIndex,CTMatrix& matrix,double deltaAngle,CWriteFormat& log)
{

   bool retval = false;

   COdbPad* existingPad = getAt(layerIndex);

   if (existingPad == NULL)
   {
      COdbPad* pad = new COdbPad(*featurePad);
      pad->transform(matrix,deltaAngle);
      setAt(layerIndex,pad);
      m_attributes.mergeFrom(featurePad->getAttributes());
      retval = true;
   }
   else
   {
      // Case 2053, Keep larger of the two pads, log the conflict.
      // Assume circle, only need one number for size
      double existingSize = existingPad->getFeatureSymbol()->getDimension(0);
      double featureSize = featurePad->getFeatureSymbol()->getDimension(0);
      if (existingPad->getFeatureSymbol()->getDimension(1) > 0.0 &&
         featurePad->getFeatureSymbol()->getDimension(1) > 0.0)
      {
         // Two dimensions, assume rectangular, make size be area
         existingSize *= existingPad->getFeatureSymbol()->getDimension(1);
         featureSize *= featurePad->getFeatureSymbol()->getDimension(1);
      }

      COdbPad *keeper = existingPad; // assume status quo
      COdbPad *reject = featurePad;
      
      if (featureSize > existingSize)
      {
         reject = existingPad;
         keeper = featurePad;
      }

      log.writef(PrefixStatus, "Multiple Pad feature IDs found on layer %d for padstack --------------------\n", layerIndex);
      log.writef(PrefixStatus, "Keeping pad, pad usage code = %d (%s)\n", keeper->getPadUsage(), odbPadUsageTagToString(keeper->getPadUsage()));
      // will always be 2 (is a Pad)   log.writef(PrefixStatus, "            Feature type = %d\n", keeper->getFeatureType());
      log.writef(PrefixStatus, "            Aperture = %d (%s)\n", keeper->getFeatureSymbol()->getApertureShape(), keeper->getFeatureSymbol()->getApertureNameDescriptor());
      log.writef(PrefixStatus, "            Size A = %f\n", keeper->getFeatureSymbol()->getDimension(0));
      if (keeper->getFeatureSymbol()->getDimension(1) > 0.0)
         log.writef(PrefixStatus, "            Size B = %f\n", keeper->getFeatureSymbol()->getDimension(1));


      log.writef(PrefixStatus, "Dropping pad, pad usage code = %d (%s)\n", reject->getPadUsage(), odbPadUsageTagToString(reject->getPadUsage()));
      // will always be 2  (is a pad)  log.writef(PrefixStatus, "            Feature type = %d\n", reject->getFeatureType());
      log.writef(PrefixStatus, "            Aperture = %d (%s)\n", reject->getFeatureSymbol()->getApertureShape(), reject->getFeatureSymbol()->getApertureNameDescriptor());
      log.writef(PrefixStatus, "            Size A = %f\n", reject->getFeatureSymbol()->getDimension(0));
      if (reject->getFeatureSymbol()->getDimension(1) > 0.0)
         log.writef(PrefixStatus, "            Size B = %f\n", reject->getFeatureSymbol()->getDimension(1));


      if (featureSize > existingSize)
      {
         COdbPad* pad = new COdbPad(*featurePad);
         pad->transform(matrix,deltaAngle);
         setAt(layerIndex,pad);
         m_attributes.mergeFrom(featurePad->getAttributes());

         retval = true;
      }



   }

   return retval;
}

// returns origin of pad closest to the centroid of all pads
CPoint2d COdbPadstackTemplate::getCenterPadOrigin()
{
   CPoint2d origin(0.,0.);

   double xCentroid = 0.;
   double yCentroid = 0.;
   int padCount     = 0;

   for (int ind = 0;ind < getSize();ind++)
   {
      COdbPad* pad = getAt(ind);

      if (pad != NULL)
      {
         xCentroid += pad->getX();
         yCentroid += pad->getY();
         padCount++;
      }
   }

   if (padCount > 0)
   {
      xCentroid /= padCount;
      yCentroid /= padCount;

      double minDistanceSquared = 1e32;
      COdbPad* centerPad = NULL;

      for (int ind = 0;ind < getSize();ind++)
      {
         COdbPad* pad = getAt(ind);

         if (pad != NULL)
         {
            double dx = pad->getX() - xCentroid;
            double dy = pad->getY() - yCentroid;
            double distanceSquared = dx*dx + dy*dy;

            if (distanceSquared < minDistanceSquared)
            {
               minDistanceSquared = distanceSquared;
               centerPad = pad;
            }
         }
      }

      if (centerPad != NULL)
      {
         origin = centerPad->getOrigin();
      }
   }

   return origin;
}

CPoint2d COdbPadstackTemplate::centerPads()
{
   CPoint2d offset = getCenterPadOrigin();

   for (int ind = 0;ind < getSize();ind++)
   {
      COdbPad* pad = getAt(ind);

      if (pad != NULL)
      {
         pad->setOrigin(pad->getOrigin() - offset);
      }
   }

   m_origin = m_origin + offset;

   return offset;
}

bool COdbPadstackTemplate::conditionallyTakePads(COdbPadstackTemplate& otherPadstack,CWriteFormat& log)
{
   bool retval = true;

   for (int ind = 0;retval;ind++)
   {
      if (ind >= getSize() || ind >= otherPadstack.getSize())
      {
         break;
      }

      if (getAt(ind) != NULL && otherPadstack.getAt(ind) != NULL)
      {
         COdbPad* pad      = getAt(ind);
         COdbPad* otherPad = otherPadstack.getAt(ind);

         retval = false;
      }
   }

   if (retval)
   {
      for (int ind = 0;ind < otherPadstack.getSize();ind++)
      {
         COdbPad* pad = otherPadstack.removeAt(ind);

         if (pad != NULL)
         {
            setAt(ind,pad);
         }
      }

      otherPadstack.empty();
      m_attributes.mergeFrom(otherPadstack.m_attributes);
   }

   return retval;
}

bool COdbPadstackTemplate::addCoincidentFreePads(COdbPad* featurePad, CQfePadTree& padTree, CWriteFormat& log)
{
   double searchRadius = getOdbOptions().getOptionDbl(optionPadSearchRadius);

   if (featurePad->getInstantiatedFlag())
   {
      centerPads();

      CTMatrix boardToPadMatrix;
      boardToPadMatrix.translate(-(getOrigin().x),-(getOrigin().y));

      CExtent searchExtent;
      searchExtent.set(getOrigin().x - searchRadius,
                     getOrigin().y - searchRadius,
                     getOrigin().x + searchRadius,
                     getOrigin().y + searchRadius);

      CQfePadList foundList;
      int numFound = padTree.search(searchExtent,foundList);
      for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
      {
         CQfePad* qfePad = foundList.GetNext(foundPos);
         COdbPad& odbPad = qfePad->getPad();

         if (!odbPad.getInstantiatedFlag() && odbPad.getPadUsage() == padUsageUndefined && odbPad.getSubnet() == NULL)
         {
            COdbFeatureFile& featureFile = qfePad->getFeatureFile();
            COdbLayer* layer = featureFile.getLayer();
            int layerIndex = layer->getCCZLayerIndex();

            bool addedFlag = add(&odbPad,layerIndex,boardToPadMatrix,0.,log);
            odbPad.setInstantiatedFlag(addedFlag);
         }
      }//for

      return true;
   }//if 

   return false;
}

//_____________________________________________________________________________
COdbPadstackGeometry::COdbPadstackGeometry(PadstackCompareStatusTag closestMatchStatus,
   const CString& closestMatchPadstackName) : 
      COdbPadstack(20)
{
   m_block = NULL;
   m_closestMatchStatus = closestMatchStatus;
   m_closestMatchPadstackName = closestMatchPadstackName;
   m_useCount = 0;
}

//_____________________________________________________________________________
COdbPadstackGeometry* COdbPadstackGeometries::findEquivalent(const COdbPadstack& targetPadstack,
   PadstackCompareStatusTag& closestMatchStatus,CString& closestMatchPadstackName)
{
   COdbPadstackGeometry* equivalentPadstack = NULL;
   closestMatchStatus = padstackCompareNotEquivalent;

   for (int ind = 0;ind < m_padstackGeometries.getSize();ind++)
   {
      equivalentPadstack = m_padstackGeometries.getAt(ind);

      PadstackCompareStatusTag compareStatus = targetPadstack.compare(*equivalentPadstack);

      if (compareStatus > closestMatchStatus)
      {
         closestMatchStatus = compareStatus;
         closestMatchPadstackName = equivalentPadstack->getName();
      }

      if (compareStatus == padstackCompareEquivalent)
      {
         break;
      }

      equivalentPadstack = NULL;
   }

   return equivalentPadstack;
}

void COdbPadstackGeometries::writeReport(CWriteFormat& report,COdbLayerArrayWithMap& layerArray)
{
   COdbPadstackGeometry* padstack = NULL;
   CPadstackReportColumns reportColumns(layerArray.getSize());
   int closestMatchHeadingIndex = layerArray.getSize() + 1;
   CString stackNameHeading("Stack Name");
   CString useCountHeading("Use Count");
   CString closestMatchHeading("Closest Match:Reason");
   CString delimiter(" | ");

   reportColumns.updateColumnWidth(reportColumnName            ,stackNameHeading.GetLength());
   reportColumns.updateColumnWidth(reportColumnUseCount        ,useCountHeading.GetLength());
   reportColumns.updateColumnWidth(reportColumnMatchDescription,closestMatchHeading.GetLength());

   // the layerArray is index by the "row" number as defined in the matrix file

   for (int layerIndex = 1;layerIndex < layerArray.getSize();layerIndex++)
   {
      COdbLayer* layer = layerArray.getAt(layerIndex);
      CString layerName;

      if (layer != NULL)
      {
         layerName = layer->getName();
      }

      reportColumns.updateColumnWidth(layerIndex,layerName.GetLength());
   }

   for (int ind = 0;ind < m_padstackGeometries.getSize();ind++)
   {
      padstack = m_padstackGeometries.getAt(ind);

      if (padstack != NULL)
      {
         padstack->updateReportWidths(reportColumns);
      }
   }

   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   report.writef("     %-*s%s",reportColumns.getColumnWidth(reportColumnName),
      (const char*)stackNameHeading,(const char*)delimiter);

   report.writef("%-*s%s",reportColumns.getColumnWidth(reportColumnUseCount),
      (const char*)useCountHeading,(const char*)delimiter);

   for (int layerIndex = 1;layerIndex < layerArray.getSize();layerIndex++)
   {
      COdbLayer* layer = layerArray.getAt(layerIndex);
      CString layerName;

      if (layer != NULL)
      {
         layerName = layer->getName();
      }

      report.writef("%-*s%s",reportColumns.getColumnWidth(layerIndex),
         (const char*)layerName,(const char*)delimiter);
   }

   report.writef("%-*s%s\n",reportColumns.getColumnWidth(reportColumnMatchDescription),
      (const char*)closestMatchHeading,(const char*)delimiter);

   for (int ind = 0;ind < m_padstackGeometries.getSize();ind++)
   {
      report.writef("%3d: ",ind);

      padstack = m_padstackGeometries.getAt(ind);

      if (padstack != NULL)
      {
         padstack->writeReport(report,reportColumns,delimiter);
         report.writef("\n");
      }
   }
}

//_____________________________________________________________________________
COdbPpDatabase::COdbPpDatabase(CCamCadDatabase& camCadDatabase)
: CAlienDatabase(camCadDatabase,fileTypeOdbPlusPlus)
{
   m_defaultPadStackGeometry = NULL;
}

void COdbPpDatabase::empty()
{
   m_stepArray.empty();
   m_layerArray.empty();
   m_symbolFeatures.empty();
   m_wheels.empty();
   m_padstackGeometries.empty();
   m_viastackGeometries.empty();

   m_defaultPadStackGeometry = NULL;
}

bool COdbPpDatabase::read(const CString& filePathString,
   PageUnitsTag pageUnits,CWriteFormat& log)
{
   extern int Product;
   bool retval = true;

   setDatabaseFilePath(filePathString);

   CFilePath databaseDirectory(filePathString);

   m_pageUnits   = pageUnits;
   m_scaleFactor = getUnitsFactor(pageUnitsInches,m_pageUnits);
   setSearchTolerance(2. * getUnitsFactor(pageUnitsMils,m_pageUnits)); 
   setInchesPerUnit(1./m_scaleFactor);
   getOdbUnits().setPageUnits(m_pageUnits);
   //CUnits::setPageUnitsPerInch(getUnitsFactor(pageUnitsInches,m_pageUnits));

   bool artworkModeFlag = true;
   bool processFlag     = true;

   if (Product == PRODUCT_GRAPHIC)
   {
      artworkModeFlag = true;
   }
   else if (getOdbOptions().getOptionFlag(optionEnableArtworkRead))
   {
      artworkModeFlag = true;
   }
   else if (getOdbOptions().getOptionFlag(optionEnableManufactureRead))
   {
      artworkModeFlag = false;
   }
   else if (! getApp().getUseDialogsFlag())
   {
      artworkModeFlag = false;
   }
   else
   {
      ODB_dlg dlg;
      dlg.m_read = 0; // 0=Manufacturing, 1=Artwork

      if (dlg.DoModal() == IDOK)
      {
         artworkModeFlag = (dlg.m_read == 1);
         processFlag     = true;
      }
      else
      {
         processFlag     = false;
      }
   }

   if (processFlag)
   {
      CMemoryStatus memoryStatusBefore("Before Import"),memoryStatusAfter("After Memory Exception");

      try
      {
         CWaitCursor waitCursor;
         CTime startTime = CTime::GetCurrentTime();

         memoryStatusBefore.sampleMemoryStatus();

         //#define PerformMemoryAllocationTest
#ifdef PerformMemoryAllocationTest
         performMemoryAllocationTest(log);
#else

         log.writef(PrefixStatus,
            "%s Started import.\n",
            (const char*)startTime.Format("%Y%m%d.%H%M%S"));

         readMatrixData(log);
         COdbStep* firstStep = NULL;
         buildCamCadLayers();

         bool jobHasBom = false;

         for (int stepIndex = 0;stepIndex < m_stepArray.getSize();stepIndex++)
         {
            COdbStep* step = m_stepArray.getAt(stepIndex);

            if (step != NULL)
            {
               step->readStep(*this,artworkModeFlag,log);

               if (getOdbOptions().getOptionFlag(optionEnableDumpOutput))
               {
                  displayProgress(log,"Dumping data for Step");
                  step->dump(log);
               }

               buildCamCadApertures(log);

               step->buildCamCadBoard(*this,log);

               if (firstStep == NULL)
               {
                  firstStep = step;
                  firstStep->getFile()->setShow(true);
               }

               // If any one step has BOM then we'll assume the entire job has a BOM.
               if (!step->getBom().IsEmpty())
                  jobHasBom = true;
            }
         }

         readWheels(log);

         displayProgress(log,"Generating SMD component properties");
         getCamCadDatabase().getCamCadDoc().getSettings().ComponentSMDrule = 
            getOdbOptions().getOptionInt(optionComponentsSmdRule);
         getCamCadDatabase().getCamCadDoc().GenerateSmdComponentTechnologyAttribs(NULL,false);

         //*rcf Get rid of these once we're sure the new location for it works.
         //redundant, this is now done earlier: displayProgress(log,"Generating Pin Location Data");
         //redundant, this is now done earlier: getCamCadDatabase().getCamCadDoc().OnGeneratePinloc();

         displayProgress(log,"Generating Manufacturing Refdes information");
         DoMakeManufacturingRefDes(&(getCamCadDatabase().getCamCadDoc()),false);

         CTime finishTime = CTime::GetCurrentTime();
         CTimeSpan processTime = finishTime - startTime;

         log.writef(PrefixStatus,
            "%s Completed import, duration = %s\n",
            (const char*)finishTime.Format("%Y%m%d.%H%M%S"),
            (const char*)processTime.Format("%D.%H:%M:%S"));

         if (getOdbOptions().getOptionFlag(optionWriteDebugCcFile))
         {
            CFilePath debugFilePath(getDatabaseFilePath());
            debugFilePath.pushLeaf(getDatabaseFilePath().getFileName() + "DebugOut");
            debugFilePath.setExtension("cc");

            displayProgress(log,"Writing debug file, '%s'",(const char*)debugFilePath.getPath());

            //getCamCadDatabase().getCamCadDoc().SaveDataFileAs(debugFilePath.getPath());

            displayProgress(log,"Write of debug file completed.");

            if (getCamCadDatabase().hasRecursiveReference())
            {
               log.writef(PrefixError,"Recursive reference detected.\n");

#ifdef _DEBUG
               AfxDebugBreak();
               getCamCadDatabase().hasRecursiveReference();
#endif
            }
         }

         displayProgress(log,"Writing PadStack Reports");
         writePadstackReports(log);
#endif
      }
      catch (CMemoryException* memoryException)
      {
         TCHAR messageBuf[256];
         memoryException->GetErrorMessage(messageBuf,255);
         CString message(messageBuf);

         formatMessageBox("Out of Memory.  This ODB++ project is too big to read!");

         if (memoryStatusAfter.sampleMemoryStatus())
         {
            memoryStatusBefore.writeComparitiveStatusReport(log,memoryStatusAfter);
         }

         log.writef(PrefixDisaster,
            "Could not read the ODB++ project - Out of Memory.\n");

         retval = false;
      }
   }

   displayProgress(log,"Deallocating memory for ODB++ data.");
   empty();

   //_CrtMemCheckpoint(&memStateEnd);

   //if (_CrtMemDifference(&memStateDifference,&memStateBegin,&memStateEnd))
   //{
   // _CrtMemDumpStatistics(&memStateDifference);
   //}

   return retval;
}

#ifdef DEADCODE // no longer used, but save it for awhile
void COdbPpDatabase::setLoadedBasedOnPartnumber()
{
   // Post process - Scan all files for PCB Component inserts.
   // For each such insert, set LOADED attrib = true if PARTNUMBER attrib is 
   // present and not empty. If empty or not present, set LOADED = false.

   CCamCadData& ccData = this->getCamCadData();

   int partnumberKW  = ccData.getAttributeKeywordIndex(standardAttributePartNumber);
   int loadedKW      = ccData.getAttributeKeywordIndex(standardAttributeLoaded);

   POSITION filePos = ccData.getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = ccData.getFileList().GetNext(filePos);
      if (file != NULL && file->getBlock() != NULL)
      {
         BlockStruct *fileBlock = file->getBlock();
         POSITION insertPos = fileBlock->getHeadDataInsertPosition();
         while (insertPos != NULL)
         {
            DataStruct  *insertData = fileBlock->getNextDataInsert(insertPos);
            if (insertData != NULL && insertData->isInsertType(insertTypePcbComponent))
            {
               bool loaded = false;

               CAttribute *pnAttrib;
               if (insertData->lookUpAttrib(partnumberKW, pnAttrib))
               {
                  // PN Attrib is present, loaded = true if not blank, otherwise false.
                  // Make sure not to be fooled by non-empty CString that is all blanks.
                  CString pn( pnAttrib->getStringValue() );
                  pn.TrimLeft();
                  pn.TrimRight();
                  loaded = !(pn.IsEmpty());
               }
               //else
               //{
               //   // PN attrib not present, set loaded = false. Locally already init'd to false above.
               //}

               ccData.setAttribute(*insertData->getDefinedAttributes(), loadedKW, loaded?"TRUE":"FALSE");

            }
         }
      }
   }

}
#endif

bool COdbPpDatabase::readMatrixData(CWriteFormat& log)
{
   CFilePath matrixFilePath = getDatabaseFilePath();
   matrixFilePath.pushLeaf("matrix");
   matrixFilePath.pushLeaf("matrix");

   bool retval = true;
   updateToExistingCompressableFilePath(matrixFilePath,log);
   //bool retval = getOdbCompressableFileCache().uncompress(matrixFilePath.getPath(),log);

   while (retval)
   {
      COdbAppFileReader fileReader;

      if (fileReader.openFile(matrixFilePath.getPath()) != statusSucceeded)
      {
         log.writef(PrefixError,
            "Could not read the Matrix file, '%s' - status = '%s'.\n",
            (const char*)matrixFilePath.getPath(),fileStatusTagToString(fileReader.getOpenStatus()));

         break;
      }

      //CStdioFile matrixFile;

      //if (! matrixFile.Open(matrixFilePath.getPath(),CFile::modeRead | CFile::shareDenyNone))
      //{
      //   break;
      //}

      displayProgress(log,"Reading Matrix file, '%s'.",(const char*)matrixFilePath.getPath());

      CSupString line;
      CStringArray params;
      int numParams;
      enum { sBegin, sStep, sLayer } state = sBegin;

      CString stepName;
      int stepColumn;

      int layerRow,maxLayerRow = -1;
      bool layerBoardContext;
      OdbLayerTypeTag layerType;
      CString layerName;
      CString layerOldName;
      bool layerPositivePolarity;
      CString layerStartLayerName;
      CString layerEndLayerName;

      while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         line = fileReader.getRecord();
         numParams = line.Parse(params," =");

         if (numParams < 1)
         {
            continue;
         }

         OdbTokenTag token = paramToOdbToken(params,0);

         int iParam = 0;
         CString sParam;

         if (numParams > 1)
         {
            sParam = params[1];
            iParam = atoi(sParam);
         }

         switch (state)
         {
         case sBegin:
            switch (token)
            {
            case tokStep:   
               state = sStep;
               break;
            case tokLayer:  
               state = sLayer;
               break;
            case tok_Empty:  
               break;
            default:
               log.writef(PrefixError,
                  "Expected 'step' or 'layer' keyword, but '%s' was encountered.\n",
                  (const char*)params[0]);
            }

            break;
         case sStep:
            switch (token)
            {
            case tok_RightBrace:
               {
                  COdbStep* step = new COdbStep(stepName,stepColumn);
                  m_stepArray.AddStep(stepName,step);

                  state = sBegin;
               }

               break;
            case tokCol:
               stepColumn = iParam;
               break;
            case tokName:
               stepName = sParam;
               break;
            default:
               log.writef(PrefixError,
                  "Expected keyword for step, but '%s' was encountered.\n",
                  (const char*)params[0]);
            }

            break;
         case sLayer:
            switch (token)
            {
            case tok_RightBrace:
               {
                  COdbLayer* layer = new COdbLayer(layerName,layerRow,layerBoardContext,
                     layerType,layerOldName,layerPositivePolarity,
                     layerStartLayerName,layerEndLayerName);

                  m_layerArray.setAt(layer);

                  maxLayerRow = max(maxLayerRow,layerRow);
               
                  state = sBegin;
               }

               break;
            case tokRow:
               layerRow = iParam;
               break;
            case tokContext:
               layerBoardContext = (sParam.CompareNoCase("board") == 0);
               break;
            case tokType:
               layerType = stringToOdbLayerTypeTag(sParam);
               break;
            case tokName:
               layerName = sParam;
               break;
            case tokOld_Name:
               layerOldName = sParam;
               break;
            case tokPolarity:
               layerPositivePolarity = (sParam.CompareNoCase("positive") == 0);
               break;
            case tokStart_Name:
               layerStartLayerName = sParam;
               break;
            case tokEnd_Name:
               layerEndLayerName = sParam;
               break;
            case tokAdd_Type:
               break;
            case tokColor:
               break;
            default:
               log.writef(PrefixError,
                  "Expected keyword for layer, but '%s' was encountered.\n",
                  (const char*)params[0]);
            }

            break;
         default:
            log.writef(PrefixDisaster,
               "Logic error in %s().\n",
               (const char*)__FUNCTION__);
            break;
         }
      }

      m_boardOutlineLayerName = "Profile";

      if (m_layerArray.getAt(m_boardOutlineLayerName) == NULL)
      {
         maxLayerRow++;

         COdbLayer* layer = new COdbLayer(m_boardOutlineLayerName,maxLayerRow,true,
            odbLayerTypeProfile,"",true,"","");

         m_layerArray.setAt(layer);
      }

      break;
   }

   return retval;
}

bool COdbPpDatabase::readSymbolFeatures(CWriteFormat& log)
{
   bool retval = true;

   for (int ind = 0;ind < m_symbolFeatures.getCount();ind++)
   {
      COdbFeatureSymbol* symbol = m_symbolFeatures.getAt(ind);

      if (!symbol->isStandardSymbol() && !symbol->hasData())
      {
         retval = (symbol->readSymbolFeatureFile(*this,log) && retval);
      }
   }

   return retval;
}

bool COdbPpDatabase::readWheels(CWriteFormat& log)
{
   bool retval = false;

   CFilePath wheelsDirectoryPath = getDatabaseFilePath();
   wheelsDirectoryPath.pushLeaf("wheels");
   wheelsDirectoryPath.pushLeaf("*.*");

   CFileFind fileFinder;

   for (bool moreFilesFlag = (fileFinder.FindFile(wheelsDirectoryPath.getPath()) != 0);moreFilesFlag;)
   {
      moreFilesFlag = (fileFinder.FindNextFile() != 0);

      if (fileFinder.IsDirectory() && ! fileFinder.IsDots())
      {
         CFilePath dcodesFilePath = fileFinder.GetFilePath();
         CString wheelName = fileFinder.GetFileName();
         dcodesFilePath.pushLeaf("dcodes");

         if (fileExists(dcodesFilePath.getPath()))
         {
            COdbWheel* wheel = m_wheels.addWheel(wheelName);

            wheel->readDcodes(dcodesFilePath.getPath(),log);
         }
      }
   }

   return retval;
}

bool COdbPpDatabase::buildCamCadLayers()
{
   bool retval = true;

   getCamCadDatabase().initializeCamCadLayer(ccLayerBoardOutline     );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssemblyTop      );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssemblyBottom   );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPinAssemblyTop   );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPinAssemblyBottom);    

   COdbLayer* firstElectricalLayer  = NULL;
   COdbLayer* lastElectricalLayer   = NULL;
   COdbLayer* firstSolderMaskLayer  = NULL;
   COdbLayer* lastSolderMaskLayer   = NULL;
   COdbLayer* firstSolderPasteLayer = NULL;
   COdbLayer* lastSolderPasteLayer  = NULL;
   COdbLayer* firstSilkscreenLayer  = NULL;
   COdbLayer* lastSilkscreenLayer   = NULL;
   int electricalStackNumber = 0;
   int artworkStackNumber = 0;

   CTypedPtrArray<CPtrArray,COdbLayer*> electricalLayers;
   electricalLayers.SetSize(0,20);

   for (int layerIndex = 0;layerIndex < m_layerArray.getSize();layerIndex++)
   {
      COdbLayer* layer = m_layerArray.getAt(layerIndex);

      if (layer != NULL)
      {
         if (layer->getType() == odbLayerTypeComponent)
         {
            if      (layer->getName().CompareNoCase("COMP_+_TOP") == 0)
            {
            }
            else if (layer->getName().CompareNoCase("COMP_+_BOT") == 0)
            {
            }
         }
         else if (layer->getType() == odbLayerTypeProfile)
         {
            LayerStruct* boardLayer = getCamCadDatabase().getDefinedLayer(
               getCamCadDatabase().getCamCadLayerName(ccLayerBoardOutline),false,
               getCamCadDatabase().getCamCadLayerType(ccLayerBoardOutline));

            layer->setLayerStruct(boardLayer);
         }
         else
         {
            LayerStruct* layerStruct = getCamCadDatabase().getDefinedLayer(layer->getName());
            layer->setLayerStruct(layerStruct);

            // WI 15765 - Applying type for only board context layers is causing MISC context
            // layers to be skipped, in particular (for this WI test case) causing DOCUMENTATION
            // layers to not get their type set. It is not clear there is any usefulness in filtering
            // here by BOARD context versus MISC context, so just don't bother checking. Set
            // layer type for all contexts.
            //deadcode if (layer->getBoardContext())
            {
               layerStruct->setArtworkStackNumber(++artworkStackNumber);

               switch (layer->getType())
               {
               case odbLayerTypeSignal:
               case odbLayerTypePower:
                  electricalLayers.SetAtGrow(electricalStackNumber,layer);

                  layerStruct->setElectricalStackNumber(++electricalStackNumber);
                  layerStruct->setLayerType(LAYTYPE_SIGNAL_INNER);

                  if (firstElectricalLayer == NULL) firstElectricalLayer = layer;

                  lastElectricalLayer = layer;

                  if (layer->getType() == odbLayerTypePower && !(layer->getPositivePolarity()) )
                  {
                     layerStruct->setLayerType(LAYTYPE_POWERNEG);
                  }
                  break;

               case odbLayerTypeMixed:
                  electricalLayers.SetAtGrow(electricalStackNumber,layer);

                  layerStruct->setElectricalStackNumber(++electricalStackNumber);
                  layerStruct->setLayerType(LAYTYPE_SPLITPLANE);
                  break;

               case odbLayerTypeSolderMask:
                  if (firstSolderMaskLayer == NULL)
                  {
                     firstSolderMaskLayer   = layer;
                     layerStruct->setLayerType(LAYTYPE_MASK_TOP);
                  }
                  else
                  {
                     lastSolderMaskLayer    = layer;
                     layerStruct->setLayerType(LAYTYPE_MASK_BOTTOM);
                  }
                  break;

               case odbLayerTypeSolderPaste:
                  if (firstSolderPasteLayer == NULL)
                  {
                     firstSolderPasteLayer  = layer;
                     layerStruct->setLayerType(LAYTYPE_PASTE_TOP);
                  }
                  else
                  {
                     lastSolderPasteLayer   = layer;
                     layerStruct->setLayerType(LAYTYPE_PASTE_BOTTOM);
                  }
                  break;

               case odbLayerTypeSilkscreen:
                  if (firstSilkscreenLayer == NULL)
                  {
                     firstSilkscreenLayer   = layer;
                     layerStruct->setLayerType(LAYTYPE_SILK_TOP);
                  }
                  else
                  {
                     lastSilkscreenLayer    = layer;
                     layerStruct->setLayerType(LAYTYPE_SILK_BOTTOM);
                  }
                  break;

               case odbLayerTypeDrill:
                  layerStruct->setLayerType(LAYTYPE_DRILL);
                  break;

               case odbLayerTypeRout:
                  layerStruct->setLayerType(LAYTYPE_ALL);
                  break;

               case odbLayerTypeDocument:
                  layerStruct->setLayerType(LAYTYPE_DOCUMENTATION);
                  break;

               case odbLayerTypeComponent:
                  // No equivalent type in CCZ, just leave it as undefined.
                  break;
               }
            }
         }
      }
   }

   if (firstElectricalLayer != NULL && 
         firstElectricalLayer->getCCZLayerStruct()->getLayerType() == LAYTYPE_SIGNAL_INNER)
   {
      firstElectricalLayer->getCCZLayerStruct()->setLayerType(LAYTYPE_SIGNAL_TOP);
   }

   if (lastElectricalLayer != NULL && 
         lastElectricalLayer != firstElectricalLayer && 
         lastElectricalLayer->getCCZLayerStruct()->getLayerType() == LAYTYPE_SIGNAL_INNER)
   {
      lastElectricalLayer->getCCZLayerStruct()->setLayerType(LAYTYPE_SIGNAL_BOT);
   }

   // mirror the electrical layers
   for (int elecInd=0;;elecInd++)
   {
      int mirrorElecInd = electricalLayers.GetSize() - elecInd - 1;

      if (elecInd >= mirrorElecInd)
      {
         break;
      }

      COdbLayer* upperLayer = electricalLayers.GetAt(elecInd);
      COdbLayer* lowerLayer = electricalLayers.GetAt(mirrorElecInd);

      upperLayer->setODBMirrorLayer(lowerLayer);
   }

   if (firstSolderMaskLayer != NULL && lastSolderMaskLayer != NULL)
   {
      firstSolderMaskLayer->setODBMirrorLayer(lastSolderMaskLayer);
   }

   if (firstSolderPasteLayer != NULL && lastSolderPasteLayer != NULL)
   {
      firstSolderPasteLayer->setODBMirrorLayer(lastSolderPasteLayer);
   }

   if (firstSilkscreenLayer != NULL && lastSilkscreenLayer != NULL)
   {
      firstSilkscreenLayer->setODBMirrorLayer(lastSilkscreenLayer);
   }

   return retval;
}

COdbPadstackGeometry& COdbPpDatabase::getPadstackGeometryFor(COdbPadstackTemplate& padStack,CWriteFormat& log)
{
   return getPadstackGeometryFor(m_padstackGeometries,"PADSTACK",padStack,log);
}

COdbPadstackGeometry& COdbPpDatabase::getViastackGeometryFor(COdbPadstackTemplate& viaStack,CWriteFormat& log)
{
   return getPadstackGeometryFor(m_viastackGeometries,"VIASTACK",viaStack,log);
}

COdbPadstackGeometry& COdbPpDatabase::getPadstackGeometryFor(COdbPadstackGeometries& padstackGeometries,
   const CString& stackNamePrefix,COdbPadstackTemplate& padStack,CWriteFormat& log)
{
   int padstackCount = padstackGeometries.getCount();
   PadstackCompareStatusTag closestMatchStatus;
   CString closestMatchPadstackName;
   COdbPadstackGeometry* padstackGeometry = 
      padstackGeometries.findEquivalent(padStack, closestMatchStatus, closestMatchPadstackName);

   if (padstackGeometry == NULL)
   {
      padstackGeometry = new COdbPadstackGeometry(closestMatchStatus,closestMatchPadstackName);
      CString prefix;
      prefix.Format("%s%03d",(const char*)stackNamePrefix,padstackGeometries.getCount());

      BlockStruct* padstackBlock = getCamCadDatabase().getNewBlock(prefix, "-%d", blockTypePadstack);
      CString padstackName = padstackBlock->getName();

      padstackGeometry->setBlock(padstackBlock);
      padstackGeometries.add(padstackName,padstackGeometry);

      for (int layerIndex = 0; layerIndex < padStack.getSize(); layerIndex++)
      {
         COdbPad* pad = padStack.getAt(layerIndex);

         if (pad != NULL)
         {
            COdbFeatureSymbol* featureSymbol = pad->getFeatureSymbol();
            COdbFeatureFile* featureFile = pad->getFeatureFile();
            COdbLayer* layer = (featureFile == NULL) ? NULL : featureFile->getLayer();

            if (featureSymbol == NULL)
               log.writef(PrefixDisaster, "NULL feature symbol pointer in COdbPad.\n");

            // These two messages are linked because if there is no featureFile there will be no layer,
            // and we don't need both messages.
            if (featureFile == NULL)
               log.writef(PrefixDisaster, "NULL feature file pointer in COdbPad.\n");
            else if (layer == NULL)
               log.writef(PrefixDisaster, "NULL layer pointer in COdbFeatureFile.\n");

            if (featureSymbol != NULL && featureFile != NULL && layer != NULL)
            {
               // WI 15765 - The trouble with this was that this reader was making up an aperture that
               // does not exist when layer is drill layer. We ended up with the drill hole, which is
               // correct, plus an insert of an aperture that is the size of the drill, which is not
               // correct. When the layer is drill layer, the pad/aperture (from ODB++) is the hole.

               COdbPadGeometry* padGeometry = new COdbPadGeometry(*pad);
               padstackGeometry->setAt(layerIndex, padGeometry);

               if (layer->getType() == odbLayerTypeDrill)
               {
                  // Drill layer type gets the drill hole only, no other apertures.
                  double unfinishedHoleSize = pad->getFeatureSymbol()->getDimensionInPageUnits(0);
                  BlockStruct* toolBlock = getCamCadDatabase().getDefinedTool(unfinishedHoleSize);

                  DataStruct* toolReference = getCamCadDatabase().referenceBlock(padstackBlock,
                     toolBlock, INSERTTYPE_UNKNOWN, NULL, layerIndex);
               }
               else
               {
                  // All layers other than drill get an aperture insert.
                  getCamCadDatabase().referenceBlock(padstackBlock, featureSymbol->getDefinedAperture(*this,log),
                     INSERTTYPE_UNKNOWN, "", layerIndex, pad->getX(), pad->getY(), degreesToRadians(pad->getRotation()));
               }
            }

         }
      }
   }

   return *padstackGeometry;
}

COdbPadstackGeometry* COdbPpDatabase::getDefaultPadstack(CWriteFormat& log)
{
   if (m_defaultPadStackGeometry == NULL)
   {
      m_defaultPadStackGeometry = new COdbPadstackGeometry();
      CString padstackName,prefix,suffix;
      prefix.Format("DefaultPadstack",m_padstackGeometries.getCount());

      BlockStruct* padstackBlock;

      for (int ind=0;;ind++)
      {
         padstackName = prefix + suffix;
         padstackBlock = getCamCadDatabase().getBlock(padstackName);

         if (padstackBlock == NULL)
         {
            break;
         }

         suffix.Format("-%d",ind);
      }

      padstackBlock = getCamCadDatabase().getDefinedBlock(padstackName,blockTypePadstack);
      padstackName = padstackBlock->getName();

      m_defaultPadStackGeometry->setBlock(padstackBlock);
      m_padstackGeometries.add(padstackName,m_defaultPadStackGeometry);

      COdbFeatureSymbol* featureSymbol = m_symbolFeatures.getDefinedAt("HappyFace20");
      COdbPad pad(0,NULL,0,0.,0.,featureSymbol,true,0,0.,false);
      COdbPadGeometry* padGeometry = new COdbPadGeometry(pad);
      int layerIndex = getCamCadDatabase().getLayer(ccLayerDummyPads)->getLayerIndex();

      getCamCadDatabase().referenceBlock(padstackBlock,featureSymbol->getDefinedAperture(*this,log),
         INSERTTYPE_UNKNOWN,"",layerIndex,padGeometry->getX(),padGeometry->getY(),
         degreesToRadians(padGeometry->getRotation()));
   }

   return m_defaultPadStackGeometry;
}

void COdbPpDatabase::writePadstackReports(CWriteFormat& report)
{
   report.writef("\n- - - - - - - - - - - - - - - - - - - -\n   Pad Stack Report\n\n");

   m_padstackGeometries.writeReport(report,m_layerArray);

   report.writef("\n- - - - - - - - - - - - - - - - - - - -\n   Via Stack Report\n\n");

   m_viastackGeometries.writeReport(report,m_layerArray);

   report.writef("\n- - - - - - - - - - - - - - - - - - - -\n\n");
}

//_____________________________________________________________________________
CQfePad::CQfePad(COdbPad& pad,COdbFeatureFile& featureFile) :
   m_pad(pad), m_featureFile(featureFile)
{
}

CQfePad::~CQfePad()
{
}

int CQfePad::getObjectType() const
{
   return 0;
}

CExtent CQfePad::getExtent() const
{
   return CExtent(getOrigin(),getOrigin());
}

bool CQfePad::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double edgeDistance = _hypot(getOrigin().x - point.x,
                                getOrigin().y - point.y);

   bool retval = (edgeDistance < distance);

   return retval;
}

bool CQfePad::isExtentIntersecting(const CExtent& extent)
{
   bool retval = extent.distanceTo(getOrigin()) == 0.;

   return retval;
}

bool CQfePad::isInViolation(CObject2d& otherObject)
{
   ASSERT(0);
   return false;
}

CString CQfePad::getInfoString() const
{
   CString retval;

   retval.Format("CQfePad: origin=(%s,%s), extent=(%s,%s),(%s,%s)",
      fpfmt(getOrigin().x),
      fpfmt(getOrigin().y),
      fpfmt(getExtent().getXmin()),
      fpfmt(getExtent().getYmin()),
      fpfmt(getExtent().getXmax()),
      fpfmt(getExtent().getYmax())   );

   return retval;
}

//_____________________________________________________________________________
CQfePadList::CQfePadList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CQfeList,CQfePad*>(nBlockSize)
{
   m_isContainer = isContainer;
}

CQfePadList::~CQfePadList()
{
   empty();
}

void CQfePadList::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CQfePad* qfePad = GetNext(pos);
         delete qfePad;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
CQfePadTree::CQfePadTree() : CQfeExtentLimitedContainer(CSize2d(0.,0.),false)
{
}

CQfePadTree::~CQfePadTree()
{
}

//_____________________________________________________________________________
CQfePadstackTemplate::CQfePadstackTemplate(COdbPadstackTemplate* padstack)
{
   m_padstack = padstack;
}

CQfePadstackTemplate::~CQfePadstackTemplate()
{
}

int CQfePadstackTemplate::getObjectType() const
{
   return 0;
}

CExtent CQfePadstackTemplate::getExtent() const
{
   return CExtent(getOrigin(),getOrigin());
}

bool CQfePadstackTemplate::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double edgeDistance = _hypot(getOrigin().x - point.x,
                                getOrigin().y - point.y);

   bool retval = (edgeDistance < distance);

   return retval;
}

bool CQfePadstackTemplate::isExtentIntersecting(const CExtent& extent)
{
   bool retval = extent.distanceTo(getOrigin()) == 0.;

   return retval;
}

bool CQfePadstackTemplate::isInViolation(CObject2d& otherObject)
{
   ASSERT(0);
   return false;
}

CString CQfePadstackTemplate::getInfoString() const
{
   CString retval;

   retval.Format("CQfePadstackTemplate: origin=(%s,%s), extent=(%s,%s),(%s,%s)",
      fpfmt(getOrigin().x),
      fpfmt(getOrigin().y),
      fpfmt(getExtent().getXmin()),
      fpfmt(getExtent().getYmin()),
      fpfmt(getExtent().getXmax()),
      fpfmt(getExtent().getYmax())   );

   return retval;
}

//_____________________________________________________________________________
CQfePadstackTemplateList::CQfePadstackTemplateList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CQfeList,CQfePadstackTemplate*>(nBlockSize)
{
   m_isContainer = isContainer;
}

CQfePadstackTemplateList::~CQfePadstackTemplateList()
{
   empty();
}

void CQfePadstackTemplateList::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CQfePadstackTemplate* qfePadstack = GetNext(pos);
         delete qfePadstack;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
CQfePadstackTemplates::CQfePadstackTemplates() : CQfeExtentLimitedContainer(CSize2d(0.,0.),false)
{
}

CQfePadstackTemplates::~CQfePadstackTemplates()
{
}


//-------------------------------------------------------------
#ifdef EXPERIMENTAL_VPL

// THIS HAS TO STAY LAST
// These Valor header files defines some macros that are already defined in CAMCAD, and
// the definitions of some are not compatible. So keep this Valor interface stuff last
// in this source file (or move it to its own), that seems to work well enough.
//
// These are from Valor - Trilogy, are included in vPlan as legacy access stuff under Native.
// That is where we get them, for CAMCAD builds we have copies in ClearCase.
#define WINDOWS_NV
#undef STRLEN
#include "gen.h"
#include "gen_log.h"
#include "gen_log.p"
#include "gen_fs.h"
#include "gen_fs.p"
#include "gen_mem.h"
#include "gen_mem.p"

static void CallbackWrapper__LogMessage(log_severity_enum severity, const char *context, char *fmt, va_list args)
{
   // no op
}

// From Alex Zigelboim
// This DLL [coretrilogy.dll] contains exported function gen__fs_read_protected_file intended to read protected file 
// and to return its content as decrypted and uncompressed string that you can parse. Synopsis of the function:
// INPUT PARAMETERS:
// file_type – type of protected file (0 for protected VPL packages)
// decomp_buffer – compression option (ENCRYPT_AND_COMPRESS in your case, or if you cannot access the macro - 0x1000)
// deformat_buffer – formatting option (0 in your case)
// file_name – full path of protected file
// OUTPUT PARAMETERS
// inp_str – pointer to decrypted string (allocated in the function; you must release it after the use since memory allocated in legacy code is not released by garbage collector)
// inp_str_len – length of the decrypted string
// is_protected – TRUE if protected file exists and legal. Use it as success indication  
// Good luck

// int gen__fs_read_protected_file (int     file_type,
//                                  int     decomp_buffer,
//                                  int     deformat_buffer,
//                                  char    *file_name,
//                                  char    **inp_str,
//                                  int     *inp_str_len,
//                                  boolean *is_protected);

// Symbol value from Alex
#define ENCRYPT_AND_COMPRESS 0x1000

int getVplPkgsFileContent(CString vplPkgsFilePath, CString &vplContent)
{
   vplContent.Empty();

   // init coretrilogy dll, we're told this needs to be set.
   gen__log_set_log_cb(CallbackWrapper__LogMessage); 

   // go
   char *decryptedResult = NULL;
   int   decryptedResultLen;
   boolean is_protected;

   int stat = gen__fs_read_protected_file(
      0,           // file_type, 0 = protected VPL
      ENCRYPT_AND_COMPRESS, // decomp_buffer,
      0,                    // deformat_buffer,
      vplPkgsFilePath.GetBuffer(0),          // char    *file_name,
      &decryptedResult,     // char    **inp_str,
      &decryptedResultLen,     // int     *inp_str_len,
      &is_protected // boolean *is_protected);
      );

   if (is_protected)  // Means vpl file exists and was read okay.
   {
      vplContent = decryptedResult;
   }

   if (decryptedResult != NULL)  // Free the mem allocated in decrypt lib call
   {
      gen__mem_free(decryptedResult, GEN_MEM_TABLE_MODE, __FILE__, __LINE__);
   }

   return stat;
}

#endif



