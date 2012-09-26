// $Header: /CAMCAD/5.0/read_wrt/AgilentSjPlxIn.cpp 22    3/12/07 12:48p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "StdAfx.h"
#include "CamCadDatabase.h"
#include "AgilentSjPlxIn.h"
#include "CCEtoODB.h"
#include "StdioZipFile.h"
#include "CdErr.h"
#include "TMState.h"
#include "StandardAperture.h"
#include "GerberEducator.h"
#include "RwUiLib.h"

// uncomment to enable memory leak detection
//#define new DEBUG_NEW

void ReadAgilentSjPlx(CCamCadDatabase& camCadDatabase,FileStruct* pcbFileStruct,
   CFilePath& filePath,PageUnitsTag pageUnits,bool topFlag,CWriteFormat& log);

/******************************************************************************
* ReadODBpp
*/
void ReadAgilentSjPlx(const char* pFilePath,CCEtoODBDoc* camCadDoc,FormatStruct* format,int pageUnits)
{
   CSupString filePaths(pFilePath);
   CString topFilePath    = filePaths.Scan("|");
   CString bottomFilePath = filePaths.Scan("|");
   CFilePath projectFilePath(topFilePath.IsEmpty() ? bottomFilePath : topFilePath);

   if (camCadDoc != NULL && ! projectFilePath.getPath().IsEmpty())
   {
      CCamCadDatabase camCadDatabase(*camCadDoc);
      const int writeFormatBufSize = 2048;

      CFilePath htmlLogFilePath(projectFilePath);

      if (projectFilePath.getFileName().IsEmpty()) projectFilePath.popLeaf();

      htmlLogFilePath.setExtension("htm");
      CHtmlFileWriteFormat* htmlLogFile = new CHtmlFileWriteFormat(writeFormatBufSize);

      if (!htmlLogFile->open(htmlLogFilePath.getPath()))
      {
         formatMessageBoxApp(MB_ICONSTOP,"Could not open the error log, '%s'",
            (const char*)htmlLogFilePath.getPath());
      }

      CWriteFormat& log = *htmlLogFile;

   #ifdef _DEBUG
      CString configuration = "(Debug Build)";
   #else
      CString configuration = "";
   #endif

      log.writef("%s\n",(const char*)getApp().getCamCadSubtitle());
      log.writef("CAMCAD Agilent SJ PLX reader. %s\n",(const char*)configuration);
      displayProgress(log,"Opened log file '%s'.",(const char*)htmlLogFilePath.getPath());

      FileStruct* pcbFileStruct = camCadDatabase.getNewSingleVisiblePcb("Agilent SJ PLX",fileTypeAgilentSjPlx);

      if (! topFilePath.IsEmpty())
      {         
         ReadAgilentSjPlx(camCadDatabase,pcbFileStruct,CFilePath(topFilePath),intToPageUnitsTag(pageUnits),true,log);
      }

      if (! bottomFilePath.IsEmpty())
      {         
         ReadAgilentSjPlx(camCadDatabase,pcbFileStruct,CFilePath(bottomFilePath),intToPageUnitsTag(pageUnits),false,log);
      }

      //camCadDatabase.getCamCadDoc().CalcBlockExtents(pcbFileStruct->block_ptr);
   }
}

void ReadAgilentSjPlx(CCamCadDatabase& camCadDatabase,FileStruct* pcbFileStruct,
   CFilePath& filePath,PageUnitsTag pageUnits,bool topFlag,CWriteFormat& log)
{
   CAgilentSjPlxFileReader fileReader;

   int refDesKeywordIndex        = camCadDatabase.getKeywordIndex(ATT_REFNAME);
   int partNumberKeywordIndex    = camCadDatabase.getKeywordIndex(ATT_PARTNUMBER);
   int shapeKeywordIndex         = camCadDatabase.registerKeyword("Shape"        ,valueTypeString);
   int machineFeederKeywordIndex = camCadDatabase.registerKeyword("MachineFeeder",valueTypeString);
   int machineKeywordIndex       = camCadDatabase.registerKeyword("Machine"      ,valueTypeString);
   int feederKeywordIndex        = camCadDatabase.registerKeyword("Feeder"       ,valueTypeString);
	int dataTypeKw						= camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);
	int dataSourceKw					= camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);

   while (true)
   {
      if (fileReader.openFile(filePath.getPath()) != statusSucceeded)
      {
         log.writef("Could not open file '%s' for reading.\n",(const char*)filePath.getPath());
         break;
      }

      LayerStruct* floatingLayer = camCadDatabase.getLayer(ccLayerFloat);
      LayerStruct* layer = camCadDatabase.getLayer(topFlag ? ccLayerCentroidTop : ccLayerCentroidBottom);

      CString dataSource = gerberEducatorDataSourceToString(gerberEducatorDataSourceCentroid);

		if (topFlag)
		{
         CString dataType   = gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeTop);
         camCadDatabase.addAttribute(layer->attributes(),dataSourceKw,dataSource);
         camCadDatabase.addAttribute(layer->attributes(),dataTypeKw  ,dataType);
			//layer->setAttrib(&camCadDatabase.getCamCadDoc(), dataSourceKw, valueTypeString, QDataSourceCentriod, SA_OVERWRITE, NULL);	
			//layer->setAttrib(&camCadDatabase.getCamCadDoc(), dataTypeKw, valueTypeString, QDataTypeTop, SA_OVERWRITE, NULL);
		}
		else
		{
         CString dataType   = gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeBottom);
         camCadDatabase.addAttribute(layer->attributes(),dataSourceKw,dataSource);
         camCadDatabase.addAttribute(layer->attributes(),dataTypeKw  ,dataType);
			//layer->setAttrib(&camCadDatabase.getCamCadDoc(), dataSourceKw, valueTypeString, QDataSourceCentriod, SA_OVERWRITE, NULL);	
			//layer->setAttrib(&camCadDatabase.getCamCadDoc(), dataTypeKw, valueTypeString, QDataTypeBottom, SA_OVERWRITE, NULL);
		}

      BlockStruct* pcbBlock = pcbFileStruct->getBlock();
      BlockStruct* centroidGeometry = camCadDatabase.getDefinedCentroidGeometry(pcbFileStruct->getFileNumber());

      CTMatrix matrix;
      matrix.rotateDegrees(-90.);
      matrix.scale(topFlag ? 1. : -1.,1.);      
      double xMicrons,yMicrons;
      BlockStruct* fiducialAperture = NULL;

      displayProgress(log,"Reading file, '%s'.",(const char*)filePath.getPath());

		while (fileReader.readFileRecord(&log) == statusSucceeded)
      {
         if (fileReader.getNumParams() > 8 && fileReader.getParam(0) == "d")
         {
            double orientationDegrees;

            fileReader.getParam(xMicrons,1);
            fileReader.getParam(yMicrons,2);
            fileReader.getParam(orientationDegrees,5);

            int iPos = 0;
            CString refDes        = fileReader.getParam(3);
            CString partNumber    = fileReader.getParam(6);
            CString machineFeeder = fileReader.getParam(7);
            CString machine       = machineFeeder.Tokenize("-",iPos);
            CString feeder        = machineFeeder.Tokenize("-",iPos);
            CString shape;
            
            if (fileReader.getNumParams() > 9)
            {
               shape = fileReader.getParam(9);
            }

            double x = (xMicrons / 1000.) *  getUnitsFactor(pageUnitsMilliMeters,pageUnits);
            double y = (yMicrons / 1000.) *  getUnitsFactor(pageUnitsMilliMeters,pageUnits);

            matrix.transform(x,y);

            double orientation = degreesToRadians(orientationDegrees);

            DataStruct* centroidInsert = 
               camCadDatabase.referenceBlock(pcbBlock,centroidGeometry,
                  INSERTTYPE_UNKNOWN,refDes,layer->getLayerIndex(),x,y,orientation,!topFlag);

            camCadDatabase.addAttribute(getAttributeMap(centroidInsert),refDesKeywordIndex,refDes);
            camCadDatabase.addAttribute(getAttributeMap(centroidInsert),partNumberKeywordIndex,partNumber);
            camCadDatabase.addAttribute(getAttributeMap(centroidInsert),shapeKeywordIndex,shape);
            camCadDatabase.addAttribute(getAttributeMap(centroidInsert),machineFeederKeywordIndex,machineFeeder);
            camCadDatabase.addAttribute(getAttributeMap(centroidInsert),machineKeywordIndex,machine);
            camCadDatabase.addAttribute(getAttributeMap(centroidInsert),feederKeywordIndex,feeder);
         }
         else if (fileReader.getNumParams() > 2 && fileReader.getParam(0) == "f")
         {
            fileReader.getParam(xMicrons,1);
            fileReader.getParam(yMicrons,2);

            double x = camCadDatabase.convertToPageUnits(pageUnitsMilliMeters,xMicrons / 1000.);
            double y = camCadDatabase.convertToPageUnits(pageUnitsMilliMeters,yMicrons / 1000.);

            matrix.transform(x,y);

            if (fiducialAperture == NULL)
            {
               CStandardAperture standardAperture("round10");
               fiducialAperture = standardAperture.getDefinedAperture(camCadDatabase.getCamCadData());
            }

            DataStruct* fiducialInsert = 
               camCadDatabase.referenceBlock(pcbBlock,fiducialAperture,
                  INSERTTYPE_FIDUCIAL,"",layer->getLayerIndex(),x,y);
         }
      }

      break;
   }
}

//_____________________________________________________________________________
CAgilentSjPlxFileReader::CAgilentSjPlxFileReader(int maxParams)
{
   m_maxParams = maxParams;

   if (m_maxParams > MaxParserParams) m_maxParams = MaxParserParams;
   else if (m_maxParams < 1) m_maxParams = 1;

   m_numBuf[MaxNumLength] = '\0';
   m_inchesPerUnit  = 1.;
   m_numRecordsRead = 0;
   m_stdioFile      = NULL;
}

CAgilentSjPlxFileReader::~CAgilentSjPlxFileReader()
{
   delete m_stdioFile;
}

bool CAgilentSjPlxFileReader::open(const CString& filePathString)
{
   bool retval;

   m_filePath = filePathString;

   close();

   CFilePath filePath(filePathString);

   m_stdioFile = new CStdioFile();

   retval = (m_stdioFile->Open(filePathString,CFile::modeRead | CFile::shareDenyNone) != 0);

   return retval;
}

FileStatusTag CAgilentSjPlxFileReader::openFile(const CString& filePathString)
{
   FileStatusTag retval;

   m_filePath = filePathString;

   close();

   CFilePath filePath(filePathString);

   m_stdioFile = new CStdioFile();

   retval = ((m_stdioFile->Open(filePathString,CFile::modeRead | CFile::shareDenyNone) != 0) ?
               statusSucceeded : statusFileOpenFailure);

   return retval;
}

void CAgilentSjPlxFileReader::close()
{
   delete m_stdioFile;
   m_stdioFile = NULL;
}

void CAgilentSjPlxFileReader::setRecord(const CString& record)
{
   m_record = record;

   parse();
}

bool CAgilentSjPlxFileReader::readRecord()
{
   bool retval = false;

   if (m_stdioFile != NULL)
   {
      retval = (m_stdioFile->ReadString(m_record) != 0);
   }

   if (retval)
   {
      m_numRecordsRead++;
      parse();
   }

   return retval;
}

FileStatusTag CAgilentSjPlxFileReader::readFileRecord(CWriteFormat* writeFormat)
{
   FileStatusTag retval = statusFileReadFailure;

   if (m_stdioFile != NULL)
   {
      retval = ((m_stdioFile->ReadString(m_record) != 0) ? statusSucceeded : statusEof);
   }

   if (retval == statusSucceeded)
   {
      m_numRecordsRead++;
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

void CAgilentSjPlxFileReader::parse()
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

int CAgilentSjPlxFileReader::getNumParams()
{
   return m_numParams;
}

CString CAgilentSjPlxFileReader::getParam(int index)
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

void CAgilentSjPlxFileReader::getParam(CString& param,int index)
{
   if (index >= 0 && index < m_numParams)
   {
      int len = m_end[index] - m_start[index];

      char* buf = param.GetBufferSetLength(len);

      strncpy(buf,m_start[index],len);
   }
}

void CAgilentSjPlxFileReader::getParam(int& iParam,int index)
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

void CAgilentSjPlxFileReader::getParam(double& fParam,int index)
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

int CAgilentSjPlxFileReader::getFileLength() 
{ 
   int retval = 0;

   if (m_stdioFile != NULL)
   {
      retval = (int)m_stdioFile->GetLength(); 
   }

   return retval;
}

int CAgilentSjPlxFileReader::getFilePosition() 
{ 
   int retval = 0;

   if (m_stdioFile != NULL)
   {
      retval = (int)m_stdioFile->GetPosition(); 
   }

   return retval;
}

//_____________________________________________________________________________
// CAgilentSjPlxInDialog dialog
//IMPLEMENT_DYNAMIC(CAgilentSjPlxInDialog, CResizingDialog)
CAgilentSjPlxInDialog::CAgilentSjPlxInDialog(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CAgilentSjPlxInDialog::IDD, pParent)
   , m_filePathTop(_T(""))
   , m_filePathBottom(_T(""))
{

   addFieldControl(IDC_StaticPathsGroup,anchorLeft,growHorizontal);

   addFieldControl(IDC_StaticTop       ,anchorLeft);
   addFieldControl(IDC_StaticBottom    ,anchorLeft);
   addFieldControl(IDC_FileBrowse      ,anchorLeft);
   addFieldControl(IDC_SwapTopBottom   ,anchorLeft);
   addFieldControl(IDC_FilePathTop     ,anchorLeft,growHorizontal);
   addFieldControl(IDC_FilePathBottom  ,anchorLeft,growHorizontal);
   
   addFieldControl(IDOK                ,anchorBottom);
   addFieldControl(IDCANCEL            ,anchorBottom);
}

CAgilentSjPlxInDialog::~CAgilentSjPlxInDialog()
{
}

void CAgilentSjPlxInDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_FilePathTop, m_filePathTop);
   DDX_Text(pDX, IDC_FilePathBottom, m_filePathBottom);
}

void CAgilentSjPlxInDialog::addFilePath(const CString& filePath)
{
   if (m_filePathTop.IsEmpty())
   {
      m_filePathTop = m_filePathBottom;
      m_filePathBottom.Empty();
   }

   if (m_filePathTop.IsEmpty())
   {
      m_filePathTop = filePath;
   }
   else
   {
      m_filePathBottom = filePath;
   }
}

BEGIN_MESSAGE_MAP(CAgilentSjPlxInDialog, CResizingDialog)
   ON_BN_CLICKED(IDC_FileBrowse, OnBnClickedFileBrowse)
   ON_BN_CLICKED(IDC_SwapTopBottom, OnBnClickedSwapTopBottom)
END_MESSAGE_MAP()


// CAgilentSjPlxInDialog message handlers

void CAgilentSjPlxInDialog::OnBnClickedFileBrowse()
{
   UpdateData(true);

   const int fileNamesBufferSize = 4096;
   CString fileNamesBuffer;
   CString mruFilePathString = AfxGetApp()->GetProfileString(GetDialogProfileEntry(),"mruFilePath","");
   CFilePath mruFilePath(mruFilePathString);
   mruFilePath.setDelimiterBackSlash();
   CString mruDirectoryString = mruFilePath.getDirectoryPath();

   CFileDialog FileDialog(TRUE,"plx","*.plx",
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, 
         "Agilent SJ PLX (*.plx)|*.plx|Text (*.txt)|*.txt|All Files (*.*)|*.*||",NULL);

   FileDialog.m_ofn.lpstrFile = fileNamesBuffer.GetBufferSetLength(fileNamesBufferSize);
   FileDialog.m_ofn.nMaxFile  = fileNamesBufferSize;
   FileDialog.m_ofn.lpstrInitialDir = mruDirectoryString;

   int status = 0;

   while (true) 
   {
      status = FileDialog.DoModal();

      if (status == IDOK || status == IDCANCEL)
      {
         break;
      }

      if (CommDlgExtendedError() & FNERR_BUFFERTOOSMALL) // too many files
      {
         ErrorMessage("Break file group into multiple smaller groups.", "File Name Too Long",
            MB_ICONSTOP | MB_OK);  
      }
      else
      {
         break;
      }
   }

   if (status == IDOK)
   {
      mruFilePath.empty();

      POSITION pos = FileDialog.GetStartPosition();
      CString filePathString;

      if (pos != NULL)
      {
         filePathString = FileDialog.GetNextPathName(pos);
         addFilePath(filePathString);

         if (! filePathString.IsEmpty())
         {
            mruFilePathString = filePathString;
         }
      }

      if (pos != NULL)
      {
         filePathString = FileDialog.GetNextPathName(pos);
         addFilePath(filePathString);

         if (! filePathString.IsEmpty())
         {
            mruFilePathString = filePathString;
         }
      }

      if (! mruFilePathString.IsEmpty())
      {
         AfxGetApp()->WriteProfileString(GetDialogProfileEntry(),"mruFilePath",mruFilePathString);
      }
   }

   UpdateData(false);
}

void CAgilentSjPlxInDialog::OnBnClickedSwapTopBottom()
{
   UpdateData(true);

   CString filePath(m_filePathTop);
   m_filePathTop    = m_filePathBottom;
   m_filePathBottom = filePath;

   UpdateData(false);
}





