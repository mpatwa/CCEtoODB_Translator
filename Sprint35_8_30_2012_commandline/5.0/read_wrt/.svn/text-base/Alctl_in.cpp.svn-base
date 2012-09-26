// $Header: /CAMCAD/4.6/read_wrt/Alctl_in.cpp 22    2/23/07 9:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "StdAfx.h"
#include "CcDoc.h"
#include "CamCadDatabase.h"
#include "Alctl_in.h"
#include "WriteFormat.h"
#include "CCEtoODB.h"

#define ValidSpecificationEdition "V4.1"

bool isTop(const CString& layerIndicator)
{
   return (layerIndicator == "KK");
}

bool isBottom(const CString& layerIndicator)
{
   return (layerIndicator == "KA");
}

/******************************************************************************
* ReadAlcatelDocica
*/
void ReadAlcatelDocica(const char *filePath, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   ASSERT(Doc != NULL);

   CCamCadDatabase camCadDatabase(*Doc);
   CAlcatelDocicaDatabase database(camCadDatabase);
   CLogWriteFormat errorLog(2048);

   CFilePath errorLogFilePath(filePath);
   errorLogFilePath.setExtension("log");

   if (!errorLog.open(errorLogFilePath.getPath()))
   {
      formatMessageBox(MB_ICONSTOP,"Could not open the error log, '%s'",(const char*)errorLogFilePath.getPath());
   }
   else
   {
#ifdef _DEBUG
      CString configuration = "(Debug Build)";
#else
      CString configuration = "";
#endif

      errorLog.writef("%s\n",(const char*)getApp().getCamCadSubtitle());
      errorLog.writef("CAMCAD Alcatel DOCICA reader. %s\n",(const char*)configuration);

      database.read(filePath,pageUnitsUndefined,errorLog);
   }

   errorLog.close();

   //if (display_error && display_log)
      Logreader(errorLogFilePath.getPath());
}

//=============================================================================

#define EntityCodeStringToInt(s) ( ((unsigned int)s[0]<<24) | ( (unsigned int)s[1]<<16) | ( (unsigned int)s[2]<<8) | ( (unsigned int)s[3]<<0))
#define EntityCode4(a,b,c,d)     ((((unsigned int)#@a)<<24) | (((unsigned int)#@b)<<16) | (((unsigned int)#@c)<<8) | (((unsigned int)#@d)<<0))
#define EntityCode3(a,b,c)       ((((unsigned int)#@a)<<24) | (((unsigned int)#@b)<<16) | (((unsigned int)#@c)<<8) | (((unsigned int)' ')<<0))
#define NoLayer (-1)

// diameters units are 1/100 of a millimeter
double diameterToInches(double diameter) { return diameter /(25.4 * 100.) ; }

//_____________________________________________________________________________
CAlcatelDocicaDatabase::CAlcatelDocicaDatabase(CCamCadDatabase& camCadDatabase)
: CAlienDatabase(camCadDatabase,fileTypeAlcatelDocica)
{
   setInchesPerUnit(1./10000.);
   setFigureUnitsMetric(true);
   m_figureBlock      = NULL;
   m_figureLayerIndex = -1;
   m_figurePolyStruct = NULL;
   m_figurePoly       = NULL;
   m_tespCount        = 0;

   m_attributeIndexDirectory.SetSize(20,20);
}

void CAlcatelDocicaDatabase::setFigureUnitsMetric(bool metricFlag)
{
   const double metricUnitConversion  = 1. / (1000. * 25.4); // figure units are 1/1000 of a millimeter
   const double englishUnitConversion = 1. / (10000.);       // figure units are 1/10000 of an inch

   m_figureUnitsConversionFactor = (metricFlag ? metricUnitConversion : englishUnitConversion);
}

double CAlcatelDocicaDatabase::convertFigureUnitsToInches(double figureUnit)
{
   double retval = figureUnit * m_figureUnitsConversionFactor;

   return retval;
}

int CAlcatelDocicaDatabase::getFigureLayerIndex()
{
   if (m_figureLayerIndex < 0)
   {
      m_figureLayerIndex = getCamCadDatabase().getDefinedLayerIndex("Figure",true);
   }

   return m_figureLayerIndex;
}

int CAlcatelDocicaDatabase::getAttributeIndex(AttributeIndexTag attributeIndexTag)
{
   int attributeIndex = 0;

   if (attributeIndexTag < m_attributeIndexDirectory.GetSize())
   {
      attributeIndex = m_attributeIndexDirectory[attributeIndexTag];
   }

   if (attributeIndex <= 0)
   {
      switch (attributeIndexTag)
      {
      case attributeIndexNetName:            attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_NETNAME       );  break;
      case attributeIndexValue:              attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_VALUE         );  break;
      case attributeIndexSmdShape:           attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_SMDSHAPE      );  break;
      case attributeIndexTolerance:          attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_TOLERANCE     );  break;
      case attributeIndexPlusTolerance:      attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_PLUSTOLERANCE );  break;
      case attributeIndexMinusTolerance:     attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_MINUSTOLERANCE);  break;
      case attributeIndexCompHeight:         attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_COMPHEIGHT    );  break;
      case attributeIndexCompPinnr:          attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_COMPPINNR     );  break;
      case attributeIndexTest:               attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_TEST          );  break;

      case attributeIndexPhysicalRefDes:     attributeIndex = getCamCadDatabase().registerKeyword("PhysicalRefDes"   ,VT_STRING );  break;
      case attributeIndexPlated:             attributeIndex = getCamCadDatabase().registerKeyword("Plated"           ,VT_NONE   );  break;
      case attributeIndexDescription:        attributeIndex = getCamCadDatabase().registerKeyword("Description"      ,VT_STRING );  break;
      case attributeIndexPackagePinName:     attributeIndex = getCamCadDatabase().registerKeyword("PackagePinName"   ,VT_STRING );  break;
      case attributeIndexPhysicalPinNumber:  attributeIndex = getCamCadDatabase().registerKeyword("PhysicalPinNumber",VT_STRING );  break;
      case attributeIndexAccess:             attributeIndex = getCamCadDatabase().registerKeyword("AlcatelAccess"    ,VT_STRING );  break;
      case attributeIndexTypeOfPin:          attributeIndex = getCamCadDatabase().registerKeyword("TypeOfPin"        ,VT_STRING );  break;
      case attributeIndexTerminal:           attributeIndex = getCamCadDatabase().registerKeyword("Terminal"         ,VT_NONE   );  break;
      }

      ASSERT(attributeIndex > 0);

      m_attributeIndexDirectory.SetAtGrow(attributeIndexTag,attributeIndex);
   }

   return attributeIndex;
} 

// Reads an Alcatel format text file and creates entities in a CCamCadDatabase by mapping entities
// from the Alcatel file.
bool CAlcatelDocicaDatabase::read(const CString& filePathString,PageUnitsTag pageUnits,CWriteFormat& errorLog)
{
   bool retval = true;

   CStdioFile inputFile;
   setDatabaseFilePath(filePathString);

   if (! inputFile.Open(getDatabaseFilePath().getPath(),CFile::modeRead | CFile::shareDenyWrite))
   {
errorLog.writef(PrefixError,
"Could not open file '%s' for reading.\n",
(const char*)getDatabaseFilePath().getPath());

      retval = false;
   }

   CString header;
   int lineNumber;
   int currentState = 0;
   int previousState = 0;
   errorLog.pushHeader("");
   m_blockNumber = 0;

   getBoardFile();

   for (lineNumber=1;retval;lineNumber++)
   {
      header.Format("(%s).%d ",(const char*)getDatabaseFilePath().getFileName(),lineNumber);
      errorLog.popHeader();
      errorLog.pushHeader(header);

      if (! inputFile.ReadString(m_recordString))
      {
errorLog.writef(PrefixStatus,
"EOF.\n");
         break;
      }

      if ((lineNumber % 1000) == 0)
      {
         TRACE("lineNumber=%d\n",lineNumber);
      }

      bool recordErrorFlag = parseRecord(errorLog);

      if (recordErrorFlag)
      {
errorLog.writef(PrefixError,
"Illegal %s record string, '%s', encountered.\n",
(const char*)m_entityCodeString,(const char*)m_recordString);
      }
      else
      {
         processRecord(errorLog);
      }
   }

   errorLog.popHeader();

   bool writeDebugFile = true;

   if (writeDebugFile)
   {
      header.Format("(%s)        %s ",(const char*)getDatabaseFilePath().getFileName(),
         (const char*)CTime::GetCurrentTime().Format("%Y%m%d.%H%M%S"));
      errorLog.pushHeader(header);

      CFilePath debugFilePath(getDatabaseFilePath());
      debugFilePath.setBaseFileName(getDatabaseFilePath().getFileName() + "DebugOut");
      debugFilePath.setExtension("xml");

      errorLog.writef(PrefixStatus,"Writing debug file, '%s'\n",(const char*)debugFilePath.getPath());

      //getCamCadDatabase().getCamCadDoc().SaveDataFileAs(debugFilePath.getPath());

      errorLog.writef(PrefixStatus,"Write of debug file completed.\n");

      if (getCamCadDatabase().hasRecursiveReference())
      {
         errorLog.writef(PrefixError,"Recursive reference detected.\n");

#ifdef _DEBUG
         AfxDebugBreak();
         getCamCadDatabase().hasRecursiveReference();
#endif
      }

      errorLog.popHeader();
   }

   return retval;
}

void CAlcatelDocicaDatabase::addAccessAttribute(CAttributes** attributeMap,
   const CString& accessCodeString,CWriteFormat& errorLog)
{
   int accessCode = atoi(accessCodeString);
   CString accessValue;

   switch (accessCode)
   {
   case 0:  accessValue = "None";    break;
   case 1:  accessValue = "Top";     break;
   case 2:  accessValue = "Bottom";  break;
   case 3:  accessValue = "Both";    break;
   }

   if (accessValue != "")
   {
      getCamCadDatabase().addAttribute(attributeMap,getAttributeIndex(attributeIndexAccess),
         accessValue,errorLog);
   }
}

void CAlcatelDocicaDatabase::addTestAccessAttribute(CAttributes** attributeMap,
   const CString& testAccessCodeString,CWriteFormat& errorLog)
{
   int testAccessCode = atoi(testAccessCodeString);
   CString testAccessValue;

   switch (testAccessCode)
   {
   case 0:  testAccessValue = "None";    break;
   case 1:  testAccessValue = "Top";     break;
   case 2:  testAccessValue = "Bottom";  break;
   case 3:  testAccessValue = "Both";    break;
   }

   if (testAccessValue != "")
   {
      getCamCadDatabase().addAttribute(attributeMap,getAttributeIndex(attributeIndexTest),
         testAccessValue,errorLog);
   }
}

// returns true if an error occured
bool CAlcatelDocicaDatabase::parseRecord(CWriteFormat& errorLog)
{
   bool recordErrorFlag = false;

   m_entityCodeString = m_recordString.Left(4);
   m_entityCode       = EntityCodeStringToInt(m_entityCodeString);

   switch (m_entityCode)
   {
   case EntityCode4(I,D,E,N):
      recordErrorFlag = !extractField(m_idenProductPartNumber         , 5,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenProductPartNumberExtension,17, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenPartsListEdition          ,19, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenReserved1                 ,22, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenEditionIndexOfTechDocica  ,24, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenReserved2                 ,26, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenItemChangeStatus          ,28, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenEditionIndexOfManufDocica ,30, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenReserved3                 ,33,10,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenProductMnemonic           ,43, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenIndexSheet                ,51,17,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenIndexSheetEdition         ,68, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenPlatformIndicator         ,71, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenLayoutToolIndicator       ,72, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_idenTranslatorRelease         ,73, 8,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(P,D,E,S):
      recordErrorFlag = !extractField(m_pdesDescription1, 5,24,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pdesDescription2,29,24,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pdesDescription3,53,24,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(G,E,N,E):
      recordErrorFlag = !extractField(m_geneFiller1                         , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneCreationDate                    ,12, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneFiller2                         ,18,11,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneClusterImplementationLevel      ,29, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_genePanellizationImplementationLevel,30, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneCutsAndStrapsImplementationLevel,31, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_genePowerSupplyImplementationLevel  ,32, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneFiller3                         ,33, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneChangeDocumentNumber            ,35,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneDataOriginatorTechnicalCenter   ,49,10,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneFiller4                         ,59, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneNameOfPersonInCharge            ,61,16,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_geneSpecificationEdition            ,77, 4,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(P,R,O,J):
      recordErrorFlag = !extractField(m_projProjectSubproject, 5,24,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(P,B,T,1):
      recordErrorFlag = !extractField(m_pbt1DensityClass                                  , 5, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1ProtrusionLength                              , 9, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1ThicknessOfBoard                              ,12, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1PlatingSolderSideFinish                       ,15, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1PlatingComponentSideFinish                    ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1PcbOutlineGraphicSymbol                       ,23,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1Filler1                                       ,37, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1TechnologyFileName                            ,45,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1NumberOfWiresToBeAdded                        ,57, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1NumberOfTracksCutOff                          ,60, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1NumberOfSolderPointsToBeAddedDueToCutsAndWires,63, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt1DocicaValidityLevel                           ,66, 1,&errorLog) || recordErrorFlag;

      extractField(m_pbt1NumberOfSolderingPointsOnSolderSide           ,67, 5,NULL);
      extractField(m_pbt1NumberOfSolderingPointsOnComponentSide        ,72, 5,NULL);

      break;
   case EntityCode4(P,B,T,2):
      recordErrorFlag = !extractField(m_pbt2XminOfPbShape, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt2YminOfPbShape,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt2XmaxOfPbShape,19, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pbt2YmaxOfPbShape,26, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(P,A,N,O):
      recordErrorFlag = !extractField(m_panoPanelOutlineSymbolName,13,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_panoXminOfThePanelShape   ,27, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_panoYminOfThePanelShape   ,34, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_panoXmaxOfThePanelShape   ,41, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_panoYmaxOfThePanelShape   ,48, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(M,U,L,T):
      recordErrorFlag = !extractField(m_multOrientationOfCopy1OnPanel, 5, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multXcoordinateOfCopy1       , 9, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multYcoordinateOfCopy1       ,16, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multNumberOfXcopies1         ,23, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multXstepBetweenXcopies1     ,25, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multNumberOfYcopies1         ,32, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multYstepBetweenYcopies1     ,34, 7,&errorLog) || recordErrorFlag;

      recordErrorFlag = !extractField(m_multOrientationOfCopy2OnPanel,41, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multXcoordinateOfCopy2       ,45, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multYcoordinateOfCopy2       ,52, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multNumberOfXcopies2         ,59, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multXstepBetweenXcopies2     ,61, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multNumberOfYcopies2         ,68, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_multYstepBetweenYcopies2     ,70, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(P,S,U,P):
      recordErrorFlag = !extractField(m_psupType              , 5, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_psupVoltage           , 8, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_psupCurrent           ,16, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_psupSignalName        ,24,19,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_psupReferenceSignal   ,43,19,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_psupReferencePotential,62, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_psupExternalOrInternal,70, 1,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(N,L,M,C):
      recordErrorFlag = !extractField(m_nlmcPartNumber         , 5,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcPartNumberExtension,17, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcComponentName      ,19, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcItemReferenceNumber,27, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcMeasuringUnit      ,33, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcQuantity           ,36,10,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcDescription        ,46,24,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_nlmcClusterName        ,70, 8,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,M,P,1):
      recordErrorFlag = !extractField(m_cmp1PartNumber           , 5,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1PartNumberExtension  ,17, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1ComponentName        ,19, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1CenterX              ,27, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1CenterY              ,34, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1Orientation          ,41, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1ItemReferenceNumber  ,45, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1ComponentMountingSide,51, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1MountingCode         ,53, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp1ComponentType        ,59, 8,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,M,P,2):
      recordErrorFlag = !extractField(m_cmp2Description               , 5,24,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2SilkScreenMarkingIndicator,29, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2CompMountingHeight        ,30, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2StandardModel             ,33,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2Value                     ,45,10,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2Unit                      ,55, 5,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2MaxTolerance              ,60, 5,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2MinTolerance              ,65, 5,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2NumCpins                  ,70, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2NumChol                   ,74, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp2NumFidc                   ,78, 2,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,M,P,3):
      recordErrorFlag = !extractField(m_cmp3GraphicSymbolName         , 5,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp3GraphicSymbolNameAlternate,19,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp3Description               ,33,24,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cmp3ClusterName               ,57, 8,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(E,T,C,1):
      recordErrorFlag = !extractField(m_etc1XcoordOfComponentCenter   , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1YcoordOfComponentCenter   ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1Orientation               ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1GraphicSymbolName         ,23,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1GraphicSymbolNameAlternate,37,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1ComponentMountingSide     ,51, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1ComponentName             ,53, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1PartNumber                ,61,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1PartNumberExtension       ,73, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc1MountingCode              ,75, 6,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(E,T,C,2):
      recordErrorFlag = !extractField(m_etc2StandardModel  , 5,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2Value          ,17,10,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2Unit           ,27, 5,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2MaxTolerance   ,32, 5,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2MinTolerance   ,37, 5,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2CpinLinesNumber,42, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2ClusterName    ,46, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_etc2ComponentType  ,54, 8,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,P,I,N):
      recordErrorFlag = !extractField(m_cpinXcoordOfTheHoleArea         , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinYcoordOfTheHoleArea         ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinHoleDiameterOrPadOrientation,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinLayoutLayer                 ,23, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinGraphicDrawingPadSymbol     ,25,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinPhysicalPinNumber           ,39, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinTransferTypeIndicator       ,45, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinPlatingIndicator            ,46, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinPackagePinName              ,47, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinSignalName                  ,55,19,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinTypeOfPinPart1              ,74, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinTypeOfPinPart2              ,75, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinTypeOfPinPart3              ,76, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinTypeOfTerminal              ,77, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinAccessibilityCode           ,78, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cpinTestAccessCode              ,79, 1,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,H,O,L):
      recordErrorFlag = !extractField(m_cholXcoordOfTheHoleCenter, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cholYcoordOfTheHoleCenter,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cholHoleDiameter         ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cholPlatingIndicator     ,23, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cholPhysicalPinNumber    ,24, 6,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(H,O,L,B):
   case EntityCode4(H,O,L,P):
      recordErrorFlag = !extractField(m_holXcoordOfTheHoleCenter , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_holYcoordOfTheHoleCenter ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_holHoleDiameter           ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_holPlatingIndicator      ,23, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_holNameOfTheReferenceHole,31, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_holHoleIndicator         ,34, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_holIdentificationKey     ,35,12,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(F,I,D,B):
   case EntityCode4(F,I,D,C):
   case EntityCode4(F,I,D,P):
      recordErrorFlag = !extractField(m_fidXcoordinate                   , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidYcoordinate                   ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidDiameterTarget                ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidComponentMountingSide         ,23, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidGraphicFiducialSymbol         ,25,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidGraphicFiducialSymbolAlternate,39,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidFiducialName                  ,53, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidOrientation                   ,61, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_fidIdentificationKey             ,65,12,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(V,I,A,H):
      recordErrorFlag = !extractField(m_viahXcoordinateOfTheHoleCenter, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahYcoordinateOfTheHoleCenter,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahHoleDiameter              ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahFirstLayoutLayer          ,23, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahLastLayoutLayer           ,25, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahAccessibilityCode         ,27, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahTestAccessCode            ,28, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahPadDiameter               ,29, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahFiller1                   ,33,22,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_viahSignalName                ,55,19,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(T,E,S,P):
      recordErrorFlag = !extractField(m_tespCenterXcoordinate         , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespCenterYcoordinate         ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespOrientation               ,19, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespGraphicSymbolName         ,23,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespGraphicSymbolNameAlternate,37,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespMountingSideOfTheElement  ,51, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespTestPointName             ,53, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespSilkscreenMarkingIndicator,61, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespCpinLinesNumber           ,62, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_tespIdentificationKey         ,66,12,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(W,P,A,T):
      recordErrorFlag = !extractField(m_wpatNumberOfPointsInTheEntity, 5, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatXcoordinate1             , 6, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatYcoordinate1             ,13, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatXcoordinate2             ,20, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatYcoordinate2             ,27, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatXcoordinate3             ,34, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatYcoordinate3             ,41, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatXcoordinate4             ,48, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatYcoordinate4             ,55, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatXcoordinate5             ,62, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatYcoordinate5             ,69, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatContinuationCode         ,76, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_wpatSideOfTheAddedWire       ,77, 2,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(S,I,G,N):
      recordErrorFlag = !extractField(m_signDocicaName , 5,19,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_signDrawingName,24,50,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,L,U,1):
      recordErrorFlag = !extractField(m_clu1PartNumberIdentificationKey          , 5,12,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1PartNumberExtension                  ,17, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1ClusterName                          ,19, 8,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1XcoordinateOfClusterCenter           ,27, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1YcoordinateOfClusterCenter           ,34, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1Orientation                          ,41, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1ClusterMountingSide                  ,45, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1MountingCode                         ,47, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1SilkScreenMarkingIndicator           ,53, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1ComponentMountingHeight              ,54, 3,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1NumberOfElectricalComponentsInCluster,57, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1NumberOfComponenetsInCluster         ,61, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1CpinLinesNumber                      ,65, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1CholLinesNumber                      ,69, 4,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1FidcLinesNumber                      ,73, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu1ClusterType                          ,75, 1,&errorLog) || recordErrorFlag;

      break;
   case EntityCode4(C,L,U,2):
      recordErrorFlag = !extractField(m_clu2GraphicSymbolName         , 5,14,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_clu2GraphicSymbolNameAlternate,19,14,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(D,E,B):
      recordErrorFlag = !extractField(m_debName, 5,76,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(U,N,I):
      recordErrorFlag = !extractField(m_uniMmOrIn, 5, 2,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(E,C,H):
      recordErrorFlag = !extractField(m_echScaleUsed, 5, 4,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(F,I,N):
      recordErrorFlag = !extractField(m_finName, 5,76,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(P,L,G):
      recordErrorFlag = !extractField(m_plgFillingDescriptor, 5, 1,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(F,P,G):
   case EntityCode3(P,I,N):
   case EntityCode3(F,P,N):
      // no parameters
      break;
   case EntityCode3(L,I,G):
      recordErrorFlag = !extractField(m_ligNumberOfPoints  , 5, 1,&errorLog) || recordErrorFlag;

      if (m_ligNumberOfPoints >= 1)
      {
         recordErrorFlag = !extractField(m_ligXcoordinate1    , 6, 7,&errorLog) || recordErrorFlag;
         recordErrorFlag = !extractField(m_ligYcoordinate1    ,13, 7,&errorLog) || recordErrorFlag;
      }

      if (m_ligNumberOfPoints >= 2)
      {
         recordErrorFlag = !extractField(m_ligXcoordinate2    ,20, 7,&errorLog) || recordErrorFlag;
         recordErrorFlag = !extractField(m_ligYcoordinate2    ,27, 7,&errorLog) || recordErrorFlag;
      }

      if (m_ligNumberOfPoints >= 3)
      {
         recordErrorFlag = !extractField(m_ligXcoordinate3    ,34, 7,&errorLog) || recordErrorFlag;
         recordErrorFlag = !extractField(m_ligYcoordinate3    ,41, 7,&errorLog) || recordErrorFlag;
      }

      if (m_ligNumberOfPoints >= 4)
      {
         recordErrorFlag = !extractField(m_ligXcoordinate4    ,48, 7,&errorLog) || recordErrorFlag;
         recordErrorFlag = !extractField(m_ligYcoordinate4    ,55, 7,&errorLog) || recordErrorFlag;
      }

      if (m_ligNumberOfPoints >= 5)
      {
         recordErrorFlag = !extractField(m_ligXcoordinate5    ,62, 7,&errorLog) || recordErrorFlag;
         recordErrorFlag = !extractField(m_ligYcoordinate5    ,69, 7,&errorLog) || recordErrorFlag;
      }

      recordErrorFlag = !extractField(m_ligContinuationCode,76, 1,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(R,E,C):
      recordErrorFlag = !extractField(m_recXcoordinateOfExtremity1, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_recYcoordinateOfExtremity1,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_recXcoordinateOfExtremity2,19, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_recYcoordinateOfExtremity2,26, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(C,E,R):
      recordErrorFlag = !extractField(m_cerXcoordinateOfCircleCenter, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cerYcoordinateOfCircleCenter,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_cerRadius                   ,19, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(A,R,C):
      recordErrorFlag = !extractField(m_arcXcoordinateOfCircleCenter, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcYcoordinateOfCircleCenter,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcRadiusOfTheCircle        ,19, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcAngleAtStartOfTheArc     ,26, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcAngleAtEndOfTheArc       ,32, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcDirectionOfRotation      ,38, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcXcoordinateForStartOfArc ,39, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcYcoordinateForStartOfArc ,46, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcXcoordinateForEndOfArc   ,53, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_arcYcoordinateForEndOfArc   ,60, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(P,N,T):
      recordErrorFlag = !extractField(m_pntXcoordinateOfPoint, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_pntYcoordinateOfPoint,12, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(T,X,T):
      recordErrorFlag = !extractField(m_txtXcoordOfTheAlignmentPoint  , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_txtYcoordOfTheAlignmentPoint  ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_txtNumberOfCharactersInTheText,19, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_txtTextCharacters             ,21, 60,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(E,M,P):
      recordErrorFlag = !extractField(m_empXcoordOfTheAlignmentPoint     , 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_empYcoordOfTheAlignmentPoint     ,12, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_empVariableAssociatedWithLocation,19,20,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(A,T,L):
      recordErrorFlag = !extractField(m_atlThicknessOfLine, 5, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_atlLineType       ,12, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_atlColor          ,13, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_atlLayer          ,15, 4,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(A,T,H):
      recordErrorFlag = !extractField(m_athInclinationOfHatchingLines  , 5, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_athDistanceBetweenHatchingLines,11, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_athThicknessOfHatchingLines    ,18, 7,&errorLog) || recordErrorFlag;

      break;
   case EntityCode3(A,T,T):
      recordErrorFlag = !extractField(m_attTextAlignmentCode        , 5, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attTextDirectionHorizLine   , 6, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attInclinationOfCharacters  ,12, 6,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attMirrorEffect             ,18, 1,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attCharacterFont            ,19, 2,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attHeightOfCharacters       ,21, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attWidthOfCharacters        ,28, 7,&errorLog) || recordErrorFlag;
      recordErrorFlag = !extractField(m_attDistanceBetweenCharacters,35, 7,&errorLog) || recordErrorFlag;

      break;
   default:
errorLog.writef(PrefixError,
"Unrecognized code of '%s' encountered in record string '%s'.\n",
(const char*)m_entityCodeString,(const char*)m_recordString);

      break;
   }  

   return recordErrorFlag;
}

// returns true if successfull
bool CAlcatelDocicaDatabase::extractField(int& fieldValue    ,int fieldPosition,int fieldLength,CWriteFormat* errorLog)
{
   CString field;
   char* endptr;

   fieldValue = 0;

   bool retval = extractField(field,fieldPosition,fieldLength,errorLog);

   if (retval && field.TrimLeft() != "+")
   {
      fieldValue = (int)strtol(field,&endptr,10);

      retval = (*endptr == '\0');

      if (!retval && errorLog != NULL)
      {
errorLog->writef(PrefixError,
"Unrecognizable integer, '%s', at position %d to %d\n",
(const char*)field,
fieldPosition,
fieldPosition + fieldLength - 1);
      }
   }

   return retval;
}

// returns true if successfull
bool CAlcatelDocicaDatabase::extractField(double& fieldValue    ,int fieldPosition,int fieldLength,CWriteFormat* errorLog)
{
   CString field;
   char* endptr;

   fieldValue = 0.;

   bool retval = extractField(field,fieldPosition,fieldLength,errorLog);

   if (retval && field.TrimLeft() != "+")
   {
      fieldValue = strtod(field,&endptr);

      retval = (*endptr == '\0');

      if (!retval && errorLog != NULL)
      {
errorLog->writef(PrefixError,
"Unrecognizable real, '%s', at position %d to %d\n",
(const char*)field,
fieldPosition,
fieldPosition + fieldLength - 1);
      }
   }

   return retval;
}

// returns true if successfull
bool CAlcatelDocicaDatabase::extractField(CString& fieldValue,int fieldPosition,int fieldLength,CWriteFormat* errorLog)
{
   bool retval = m_recordString.GetLength() >= fieldPosition - 1 + fieldLength;

   if (retval)
   {
      fieldValue = m_recordString.Mid(fieldPosition - 1,fieldLength).Trim();
   }
   else if (errorLog != NULL)
   {
errorLog->writef(PrefixError,
"The record string, length=%d, does not contain characters at position %d to %d\n",
m_recordString.GetLength(),
fieldPosition,
fieldPosition + fieldLength - 1);
   }

   return retval;
}

void CAlcatelDocicaDatabase::initializeCamCadLayers()
{
   getCamCadDatabase().initializeCamCadLayer(ccLayerBoardOutline         );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerDrillHoles           );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadTop               );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadBottom            );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssemblyTop          );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssemblyBottom       );    
}

bool CAlcatelDocicaDatabase::processRecord(CWriteFormat& errorLog)
{
   bool retval = true;
   int blockNumber = -1;
   bool processedFlag = false;

   for (blockNumber = m_blockNumber;blockNumber <= 14;blockNumber++)
   {
      switch (blockNumber)
      {
      case 1:  //------------- General Information
         switch (m_entityCode)
         {
         // IDEN : Product / document identification
         case EntityCode4(I,D,E,N):  processedFlag = true;  break;
         // PDES : Product description
         case EntityCode4(P,D,E,S):  processedFlag = true;  break; 
         // GENE Generic data
         case EntityCode4(G,E,N,E):
            {
               processedFlag = true;

               if (m_geneSpecificationEdition != ValidSpecificationEdition)
               {
errorLog.writef(PrefixWarning,
"Expected Specification Edition of '%s' but '%s' found.\n",
(const char*)ValidSpecificationEdition,(const char*)m_geneSpecificationEdition);
               }
            }
            
            break;
         // Project / subproject
         case EntityCode4(P,R,O,J):  processedFlag = true;  break;
            break;
         }

         break;
      case 2:  //------------- Technology Description
         switch (m_entityCode)
         {
         // PBT1 : Line 1 of the technological data PB
         case EntityCode4(P,B,T,1):  
            {
               processedFlag = true; 

               initializeCamCadLayers();

               BlockStruct* figureBlock = getCamCadDatabase().getDefinedBlock(m_pbt1PcbOutlineGraphicSymbol,
                                             blockTypeUnknown,getBoardFileNum());

               int layerIndex = getCamCadDatabase().getCamCadLayerIndex(ccLayerBoardOutline);

               DataStruct* toolReference = getCamCadDatabase().referenceBlock(getBoardBlock(),figureBlock,
                                             INSERTTYPE_UNKNOWN,NULL,layerIndex);
            }

            break;

         // PBT2 : Line 2 of the technological data PB
         case EntityCode4(P,B,T,2):  processedFlag = true;  break;
         // HOLB : Mechanical holes for the board
         case EntityCode4(H,O,L,B):
            {
               processedFlag = true;

               double unfinishedHoleSize = diameterToInches(m_holHoleDiameter);
               double x = toInches(m_holXcoordOfTheHoleCenter);
               double y = toInches(m_holYcoordOfTheHoleCenter);
               int drillLayerIndex    = getCamCadDatabase().getCamCadLayerIndex(ccLayerDrillHoles);
               BlockStruct* toolBlock = getCamCadDatabase().getDefinedTool(unfinishedHoleSize);

               DataStruct* toolReference = getCamCadDatabase().referenceBlock(getBoardBlock(),toolBlock,
                                             INSERTTYPE_UNKNOWN,NULL,drillLayerIndex,x,y);
            }

            break;
         // FIDB : Targets of the board
         case EntityCode4(F,I,D,B):
            {
               processedFlag = true;

               CamCadLayerTag layerTag = (isTop(m_fidComponentMountingSide) ? ccLayerPadTop : ccLayerPadBottom);  
               int layerIndex = getCamCadDatabase().getCamCadLayerIndex(layerTag);
               double x = toInches(m_fidXcoordinate);
               double y = toInches(m_fidYcoordinate);

               BlockStruct* fiducialPadStack = getCamCadDatabase().getDefinedBlock(m_fidGraphicFiducialSymbol,blockTypePadstack,getBoardFileNum());

               DataStruct* fiducialData = getCamCadDatabase().referenceBlock(getBoardBlock(),fiducialPadStack,
                                             INSERTTYPE_FIDUCIAL,m_fidFiducialName,layerIndex,x,y);
            }
         }

         break;
      case 3:  //------------- Panellization
         switch (m_entityCode)
         {
         // PANO : Panelization data
         case EntityCode4(P,A,N,O):  processedFlag = true;  break;
         // MULT : Description of multiples on the panel
         case EntityCode4(M,U,L,T):  processedFlag = true;  break;
         // MULT : Mechanical holes on the panel
         case EntityCode4(H,O,L,P):  processedFlag = true;  break;
         // FIDP : Targets on the panel
         case EntityCode4(F,I,D,P):  processedFlag = true;  break;
         }

         break;
      case 4:  //------------- Power Supplies
         switch (m_entityCode)
         {
         // PSUP : Power supplies used on the board
         case EntityCode4(P,S,U,P):  processedFlag = true;  break;
         }

         break;
      case 5:  //------------- Non Located Items
         switch (m_entityCode)
         {
         // NLMC : Non located items
         case EntityCode4(N,L,M,C):  processedFlag = true;  break;
         }

         break;
      case 6:  //------------- Component Data
         switch (m_entityCode)
         {
         // CMP1 : Line 1 of the component data
         case EntityCode4(C,M,P,1):  
            {
               processedFlag = true; 

               m_compPinCount = 0;

               bool topSide = isTop(m_cmp1ComponentMountingSide);  

               CString footPrintName = m_cmp1PartNumber + m_cmp1PartNumberExtension + "_" + m_cmp1ComponentName;
               m_footPrintBlock = getCamCadDatabase().getDefinedBlock(footPrintName,blockTypePcbComponent,getBoardFileNum());

               // instantiate the component
               m_componentData = getCamCadDatabase().referenceBlock(getBoardBlock(),m_footPrintBlock,
                  INSERTTYPE_PCBCOMPONENT,m_cmp1ComponentName,NoLayer,
                  toInches(m_cmp1CenterX),toInches(m_cmp1CenterY),degreesToRadians(m_cmp1Orientation) * (topSide ? 1 : -1),!topSide);

               m_compTm.initMatrix();

               if (!topSide)
               {
                  m_compTm.scale(-1.,1.);
               }

               m_compTm.rotateDegrees(m_cmp1Orientation * (topSide ? 1 : -1));
               m_compTm.translate(toInches(m_cmp1CenterX),toInches(m_cmp1CenterY));
               m_compTm.invert();  // the matrix is now set up to transform board coords to coords relative to the comp origin
               
               getCamCadDatabase().referenceDevice(m_cmp1PartNumber,m_footPrintBlock,getBoardFile());

               getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                  getAttributeIndex(attributeIndexPhysicalRefDes),m_cmp1ComponentName,errorLog);
            }

            break;
         // CMP2 : Line 2 of the component data
         case EntityCode4(C,M,P,2):  
            {
               processedFlag = true; 

               CString numValue;

               getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                  getAttributeIndex(attributeIndexDescription),m_cmp2Description,errorLog);

               if (m_cmp2Unit.Left(2) != "--")
               {
                  numValue.Format("%.2f%s",m_cmp2Value,(const char*)m_cmp2Unit);
                  getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                     getAttributeIndex(attributeIndexValue),numValue,errorLog);
               }

               if (m_cmp2MaxTolerance != 0.)
               {
                  getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                     getAttributeIndex(attributeIndexTolerance    ),m_cmp2MaxTolerance,errorLog);
                  getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                     getAttributeIndex(attributeIndexPlusTolerance),m_cmp2MaxTolerance,errorLog);
               }
               else if (m_cmp2MinTolerance != 0.)
               {
                  getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                     getAttributeIndex(attributeIndexMinusTolerance),m_cmp2MinTolerance,errorLog);
               }

               float mountingHeight = (float)toInches(m_cmp2CompMountingHeight);
               getCamCadDatabase().addAttribute(getAttributeMap(m_componentData),
                  getAttributeIndex(attributeIndexCompHeight),mountingHeight,errorLog);
            }

            break;
         // CMP3 : Line 3 of the component data
         case EntityCode4(C,M,P,3):  
            {
               processedFlag = true;  

               bool compTopSideFlag = isTop(m_cmp1ComponentMountingSide);  

               if (m_cmp3GraphicSymbolName != "")
               {
                  // instantiate the geometry
                  BlockStruct* graphicSymbolBlock = getCamCadDatabase().getDefinedBlock(m_cmp3GraphicSymbolName,
                                                      blockTypeGenericComponent,getBoardFileNum());

                  int layerIndex = getCamCadDatabase().getCamCadLayerIndex(ccLayerAssemblyTop);

                  // reference the symbol from the geometry
                  m_componentData = getCamCadDatabase().referenceBlock(m_footPrintBlock,graphicSymbolBlock,
                                       INSERTTYPE_GENERICCOMPONENT,NULL,layerIndex);
               }

               if (m_cmp3GraphicSymbolNameAlternate.Left(1) != "-")
               {
errorLog.writef(PrefixWarning,
"Alternate Graphic Symbol handling is not implemented, alternate symbol '%s' for component '%s' is ignored.\n",
(const char*)m_cmp3GraphicSymbolNameAlternate,(const char*)m_cmp1ComponentName);
               }
            }

            break;

         // CPIN : Pins of the components
         case EntityCode4(C,P,I,N):  
            {
               processedFlag = true;  

               m_compPinCount++;

               bool smdFlag     = m_cpinTransferTypeIndicator == "A";
               bool thFlag      = m_cpinTransferTypeIndicator == "H";
               m_padTopSideFlag = true;

               if (!smdFlag && !thFlag)
               {
errorLog.writef(PrefixError,
"Expected 'A' or 'H' for transfer type indicator, but '%s' was found.\n",
(const char*)m_cpinTransferTypeIndicator);
               }

               // instantiate the padStack
               BlockStruct* padStackBlock = getPadStack(m_cpinGraphicDrawingPadSymbol,!smdFlag,m_padTopSideFlag,
                                               m_cpinHoleDiameterOrPadOrientation,errorLog);

               double padRotationInDegrees = 0.;
               double x = toInches(m_cpinXcoordOfTheHoleArea);
               double y = toInches(m_cpinYcoordOfTheHoleArea);

               // transform the board coordinates to coordinates relative to the components origin
               m_compTm.transform(x,y);

               if (smdFlag)
               {
                  padRotationInDegrees = m_cpinHoleDiameterOrPadOrientation;
                  m_compTm.transform(padRotationInDegrees);
               }

               // reference the padStack as a pin
               DataStruct* pinData = getCamCadDatabase().referenceBlock(m_footPrintBlock,padStackBlock,INSERTTYPE_PIN,m_cpinPhysicalPinNumber,NoLayer,
                  x,y,degreesToRadians(padRotationInDegrees));

               NetStruct*     net = getCamCadDatabase().getDefinedNet(m_cpinSignalName,getBoardFile());
               CompPinStruct* pin = getCamCadDatabase().addCompPin(net,m_cmp1ComponentName,m_cpinPhysicalPinNumber);

               // Attributes
               addAccessAttribute(    getAttributeMap(pinData),m_cpinAccessibilityCode,errorLog);
               addTestAccessAttribute(getAttributeMap(pinData),m_cpinTestAccessCode   ,errorLog);

               CString typeOfPinValue;

               if (m_cpinTypeOfPinPart1 == "A") typeOfPinValue += "Analog ";
               if (m_cpinTypeOfPinPart1 == "D") typeOfPinValue += "Digital ";
               if (m_cpinTypeOfPinPart2 == "I") typeOfPinValue += "Input ";
               if (m_cpinTypeOfPinPart2 == "O") typeOfPinValue += "Output ";
               if (m_cpinTypeOfPinPart2 == "B") typeOfPinValue += "Bidirectional ";
               if (m_cpinTypeOfPinPart3 == "C") typeOfPinValue += "OpenCollector ";
               if (m_cpinTypeOfPinPart3 == "E") typeOfPinValue += "OpenEmitter ";
               if (m_cpinTypeOfPinPart3 == "T") typeOfPinValue += "Tristate ";

               typeOfPinValue = typeOfPinValue.Trim();

               getCamCadDatabase().addAttribute(getAttributeMap(pinData),getAttributeIndex(attributeIndexNetName          ),
                  m_cpinSignalName       ,errorLog);
               getCamCadDatabase().addAttribute(getAttributeMap(pinData),getAttributeIndex(attributeIndexPackagePinName   ),
                  m_cpinPackagePinName   ,errorLog);
               getCamCadDatabase().addAttribute(getAttributeMap(pinData),getAttributeIndex(attributeIndexPhysicalPinNumber),
                  m_cpinPhysicalPinNumber,errorLog);
               getCamCadDatabase().addAttribute(getAttributeMap(pinData),getAttributeIndex(attributeIndexCompPinnr        ),
                  m_compPinCount         ,errorLog);

               if (typeOfPinValue != "")
               {
                  getCamCadDatabase().addAttribute(getAttributeMap(pinData),
                     getAttributeIndex(attributeIndexTypeOfPin),typeOfPinValue,errorLog);
               }

               if (smdFlag)
               {
                  getCamCadDatabase().addAttribute(getAttributeMap(pinData),
                     getAttributeIndex(attributeIndexSmdShape),errorLog);
               }

               if (m_cpinTypeOfTerminal == "T")
               {
                  getCamCadDatabase().addAttribute(getAttributeMap(pinData),
                     getAttributeIndex(attributeIndexTerminal),errorLog);
               }

               if (m_cpinPlatingIndicator == "P")
               {
                  getCamCadDatabase().addAttribute(getAttributeMap(pinData),
                     getAttributeIndex(attributeIndexPlated),errorLog);
               }
            }

            break;

         // CHOL : Component mechanical holes
         case EntityCode4(C,H,O,L):  
            {
               processedFlag = true;  

               m_compPinCount++;

               double finishedHoleSize = toInches(m_cholHoleDiameter);
               CString padStackName;
               padStackName.Format("MH_%d",m_cholHoleDiameter);

               // instantiate the padStack
               BlockStruct* padStackBlock = getCamCadDatabase().getDefinedBlock(padStackName,blockTypePadstack,getBoardFileNum());

               double padRotationInDegrees = 0.;
               double x = toInches(m_cpinXcoordOfTheHoleArea);
               double y = toInches(m_cpinYcoordOfTheHoleArea);

               int drillLayerIndex    = getCamCadDatabase().getCamCadLayerIndex(ccLayerDrillHoles);
               BlockStruct* toolBlock = getCamCadDatabase().getDefinedTool(finishedHoleSize);

               DataStruct* toolReference = getCamCadDatabase().referenceBlock(padStackBlock,toolBlock,
                                              INSERTTYPE_UNKNOWN,NULL,drillLayerIndex);

               // reference the padStack as a pin
               DataStruct* pinData = getCamCadDatabase().referenceBlock(m_footPrintBlock,padStackBlock,
                                        INSERTTYPE_MECHANICALPIN,m_cholPhysicalPinNumber,NoLayer,getBoardFileNum(),x,y);
            }

            break;

         // FIDC : Component fiducials
         case EntityCode4(F,I,D,C):
            {
               processedFlag = true;  

               m_compPinCount++;

               m_padTopSideFlag = isTop(m_fidComponentMountingSide);  

               CString padStackName = "FMS_" + m_fidGraphicFiducialSymbol;
               CString padName      = "FM_"  + m_fidGraphicFiducialSymbol;

               // instantiate the padStack
               BlockStruct* padStackBlock = getCamCadDatabase().getDefinedBlock(padStackName,blockTypePadstack,getBoardFileNum());

               double padRotationInDegrees = m_fidOrientation;
               double x = toInches(m_cpinXcoordOfTheHoleArea);
               double y = toInches(m_cpinYcoordOfTheHoleArea);

               // instantiate the pad
               BlockStruct* padBlock = getCamCadDatabase().getDefinedBlock(padName,blockTypePadshape,getBoardFileNum());

               int layerIndex = getCamCadDatabase().getCamCadLayerIndex((m_padTopSideFlag == m_compTopSideFlag) ? ccLayerPadTop : ccLayerPadBottom);

               // transform the board coordinates to coordinates relative to the components origin
               m_compTm.transform(x,y);
               m_compTm.transform(padRotationInDegrees);

               // reference the pad from the padStack
               getCamCadDatabase().referenceBlock(padStackBlock,padName,
                  INSERTTYPE_UNKNOWN,NULL,layerIndex,getBoardFileNum(),0.,0.,degreesToRadians(padRotationInDegrees));

               // reference the padStack as a pin
               DataStruct* pinData = getCamCadDatabase().referenceBlock(m_footPrintBlock,padStackBlock,INSERTTYPE_FIDUCIAL,m_cpinPhysicalPinNumber,layerIndex,
                  getBoardFileNum(),x,y);

               if (m_fidGraphicFiducialSymbolAlternate.Left(1) != "-")
               {
errorLog.writef(PrefixWarning,
"Alternate Graphic Symbol handling is not implemented, alternate symbol '%s' for fiducial '%s' is ignored.\n",
(const char*)m_fidGraphicFiducialSymbolAlternate,(const char*)m_fidFiducialName);
               }
            }

            break;
         }

         break;
      case 7:  //------------- Etched Components
         switch (m_entityCode)
         {
         // ETC1 : Etched components
         case EntityCode4(E,T,C,1):  processedFlag = true;  break;
         // ETC2 : Line 2 of etched components
         case EntityCode4(E,T,C,2):  processedFlag = true;  break;
         // CPIN : Etched component pins
         case EntityCode4(C,P,I,N):  processedFlag = true;  break;
         }

         break;
      case 8:  //------------- Test Pads
         switch (m_entityCode)
         {
         // TESP : Test points data
         case EntityCode4(T,E,S,P):
            {
               processedFlag = true; 

               m_tespPinCount = 0;
               m_tespCount++;
               m_tespNewGeometryFlag = false;

               if (m_tespGraphicSymbolNameAlternate.Left(1) != "-")
               {
errorLog.writef(PrefixWarning,
"Alternate Graphic Symbol handling is not implemented, alternate symbol '%s' for test point '%s' is ignored.\n",
(const char*)m_tespGraphicSymbolNameAlternate,(const char*)m_tespTestPointName);
               }
            }
            
            break;
         // CPIN : Pad area of the test points
         case EntityCode4(C,P,I,N):  
            {
               processedFlag = true;  

               m_tespPinCount++;
               double xPin,yPin;

               // instantiate the test point
               if (m_tespPinCount == 1)
               {
                  bool topSide = isTop(m_tespMountingSideOfTheElement);  
                  double xTp = toInches(m_tespCenterXcoordinate);
                  double yTp = toInches(m_tespCenterYcoordinate);

                  m_tespTm.initMatrix();

                  if (!topSide)
                  {
                     m_tespTm.scale(-1.,1.);
                  }

                  m_tespTm.rotateDegrees(m_cmp1Orientation * (topSide ? 1 : -1));
                  m_tespTm.translate(xTp,yTp);
                  m_tespTm.invert();  // the matrix is now set up to transform board coords to coords relative to the tesp origin               
               
                  xPin = toInches(m_cpinXcoordOfTheHoleArea);
                  yPin = toInches(m_cpinYcoordOfTheHoleArea);

                  if (fabs(xPin) < .0001) xPin = 0.;
                  if (fabs(yPin) < .0001) yPin = 0.;

                  // transform the board coordinates to coordinates relative to the tesp origin
                  m_tespTm.transform(xPin,yPin);

                  CString tespGeometryName;

                  if (xPin == 0. && yPin == 0.)
                  {
                     tespGeometryName = "TP_" + m_tespGraphicSymbolName;
                  }
                  else
                  {
                     tespGeometryName.Format("TP_%s_%d",m_tespGraphicSymbolName,m_tespCount);
                  }

                  // instantiate the geometry
                  m_tespGeometry  = getCamCadDatabase().getBlock(tespGeometryName,getBoardFileNum());

                  if (m_tespGeometry == NULL)
                  {
                     m_tespNewGeometryFlag = true;
                     m_tespGeometry = getCamCadDatabase().getDefinedBlock(tespGeometryName,blockTypeTestPoint,getBoardFileNum());
                  }

                  // instantiate the test point
                  m_tespData = getCamCadDatabase().referenceBlock(getBoardBlock(),m_tespGeometry,INSERTTYPE_TESTPOINT,
                     m_tespTestPointName,NoLayer,xTp,yTp,degreesToRadians(m_tespOrientation) * (topSide ? 1 : -1),!topSide);

                  getCamCadDatabase().addAttribute(getAttributeMap(m_tespData),
                     getAttributeIndex(attributeIndexPhysicalRefDes),m_tespTestPointName,errorLog);
                  getCamCadDatabase().addAttribute(getAttributeMap(m_tespData),
                     getAttributeIndex(attributeIndexNetName       ),m_cpinSignalName   ,errorLog);
               }
               else
               {
                  xPin = toInches(m_cpinXcoordOfTheHoleArea);
                  yPin = toInches(m_cpinYcoordOfTheHoleArea);

                  // transform the board coordinates to coordinates relative to the tesp origin
                  m_tespTm.transform(xPin,yPin);
               }

               bool smdFlag     = m_cpinTransferTypeIndicator == "A";
               bool thFlag      = m_cpinTransferTypeIndicator == "H";
               m_padTopSideFlag = true;

               if (!smdFlag && !thFlag)
               {
errorLog.writef(PrefixError,
"Expected 'A' or 'H' for transfer type indicator, but '%s' was found.\n",
(const char*)m_cpinTransferTypeIndicator);
               }

               // instantiate the padStack
               BlockStruct* padStackBlock = getPadStack(m_cpinGraphicDrawingPadSymbol,!smdFlag,m_padTopSideFlag,
                                               m_cpinHoleDiameterOrPadOrientation,errorLog);

               double padRotationInDegrees = 0.;

               if (smdFlag)
               {
                  padRotationInDegrees = m_cpinHoleDiameterOrPadOrientation;
                  m_tespTm.transform(padRotationInDegrees);
               }

               if (m_tespNewGeometryFlag)
               {
                  // reference the padStack as a pin
                  DataStruct* testpointData = getCamCadDatabase().referenceBlock(m_tespGeometry,padStackBlock,
                     INSERTTYPE_PIN,m_cpinPhysicalPinNumber,NoLayer,
                     xPin,yPin,degreesToRadians(padRotationInDegrees));
               }

               NetStruct*     net = getCamCadDatabase().getDefinedNet(m_cpinSignalName,getBoardFile());
               //CompPinStruct* pin = getCamCadDatabase().addCompPin(net,m_cmp1ComponentName,m_cpinPhysicalPinNumber);
            }
            
            break;
         }

         break;
      case 9:  //------------- Vias
         switch (m_entityCode)
         {
         // VIAH : Via holes
         case EntityCode4(V,I,A,H):  
            {
               processedFlag = true;  
               bool topPadFlag    = (   isTop(m_viahFirstLayoutLayer) ||    isTop(m_viahLastLayoutLayer));
               bool bottomPadFlag = (isBottom(m_viahFirstLayoutLayer) || isBottom(m_viahLastLayoutLayer));

               BlockStruct* viaBlock = getViaPadStack(m_viahHoleDiameter,topPadFlag,bottomPadFlag,m_viahPadDiameter);

               double x = toInches(m_viahXcoordinateOfTheHoleCenter);
               double y = toInches(m_viahYcoordinateOfTheHoleCenter);

               DataStruct* viaData = getCamCadDatabase().referenceBlock(getBoardBlock(),viaBlock,INSERTTYPE_VIA,
                  "",NoLayer,x,y);

               getCamCadDatabase().addAttribute(getAttributeMap(viaData),
                  getAttributeIndex(attributeIndexNetName),m_viahSignalName,errorLog);

               addAccessAttribute(    getAttributeMap(viaData),m_viahAccessibilityCode,errorLog);
               addTestAccessAttribute(getAttributeMap(viaData),m_viahTestAccessCode   ,errorLog);
            }
            
            break;
         }

         break;
      case 10:  //------------- Cuts
         switch (m_entityCode)
         {
         // CUTS : Cut traces
         case EntityCode4(C,U,T,S):  processedFlag = true;  break;
         }

         break;
      case 11:  //------------- Added wires
         switch (m_entityCode)
         {
         // AWIR : Added wires
         case EntityCode4(A,W,I,R):  processedFlag = true;  break;
         }

         break;
      case 12:  //------------- Signal name translations
         switch (m_entityCode)
         {
            // SIGN : Signals
         case EntityCode4(S,I,G,N):  processedFlag = true;  break;
         }

         break;
      case 13:  //------------- Clusters
         switch (m_entityCode)
         {
         // Cluster definition part 1
         case EntityCode4(C,L,U,1):  processedFlag = true;  break;
         // Cluster definition part 2
         case EntityCode4(C,L,U,2):  processedFlag = true;  break;
         // CPIN : 
         case EntityCode4(C,P,I,N):  processedFlag = true;  break;
         // CHOL : 
         case EntityCode4(C,H,O,L):  processedFlag = true;  break;
         // FIDC : 
         case EntityCode4(F,I,D,C):  processedFlag = true;  break;
         }

         break;
      case 14:  //------------- Graphical Descriptions (Figures)
         if (m_figureBlock != NULL)
         {
            switch (m_entityCode)
            {
            // UNI : Unit definition
            case EntityCode3(U,N,I):
               {
                  processedFlag = true;
                  
                  if (m_uniMmOrIn == "MM")
                  {
                     setFigureUnitsMetric(true);
                  }
                  else if (m_uniMmOrIn == "IN")
                  {
                     setFigureUnitsMetric(false);
                  }
                  else
                  {
                     setFigureUnitsMetric(true);

errorLog.writef(PrefixError,
"Expected unit code of 'MM' or 'IN', but '%s' found.  Assuming metric units.\n",
(const char*)m_uniMmOrIn);
                  }
               }

               break;
            // ECH : scale definition
            case EntityCode3(E,C,H):
               processedFlag = true;

               if (m_echScaleUsed != 100)
               {
errorLog.writef(PrefixError,
"Scale factors other than 100 are not implemented. (Graphic Definition '%s')\n",
(const char*)m_debName);
               }

               break;
            // FIN : Figure definition end
            case EntityCode3(F,I,N):
               processedFlag = true;
               m_figureBlock = NULL;

               break;
            // PLG : Polygon definition start
            case EntityCode3(P,L,G):
               {
                  processedFlag = true;
                  m_figurePolyStruct = getCamCadDatabase().addPolyStruct(m_figureBlock,getFigureLayerIndex(),0,0, graphicClassNormal);
                  m_figurePoly       = getCamCadDatabase().addPoly(m_figurePolyStruct,0,m_plgFillingDescriptor != 0,false,true);

                  if (m_plgFillingDescriptor != 0 && m_plgFillingDescriptor != 1)
                  {
errorLog.writef(PrefixWarning,
"Filling decriptors of only 0 and 1 are recognized.  Descriptor %d will be implemented as 1.  (Graphic Definition '%s')\n",
m_plgFillingDescriptor,
(const char*)m_debName);
                  }
               }

               break;
            // FPG : Polygon definition end
            case EntityCode3(F,P,G):
               processedFlag = true;
               m_figurePolyStruct = NULL;

               break;
            // PIN : Component pin definition start
            case EntityCode3(P,I,N):
               processedFlag = true;
               break;
            // FPN : Component pin definition end
            case EntityCode3(F,P,N):
               processedFlag = true;
               break;
            // LIG : Line definition
            case EntityCode3(L,I,G):
               processedFlag = true;

               if (m_figurePolyStruct == NULL)
               {
                  m_figurePolyStruct = getCamCadDatabase().addPolyStruct(m_figureBlock,getFigureLayerIndex(),0,0, graphicClassNormal);
                  m_figurePoly = NULL;
               }

               if (m_figurePoly == NULL)
               {
                  m_figurePoly       = getCamCadDatabase().addOpenPoly(m_figurePolyStruct,0);
               }

               if (m_ligNumberOfPoints >= 1)
               {
                  getCamCadDatabase().addVertex(m_figurePoly,convertFigureUnitsToInches(m_ligXcoordinate1),
                                                              convertFigureUnitsToInches(m_ligYcoordinate1));
               }

               if (m_ligNumberOfPoints >= 2)
               {
                  getCamCadDatabase().addVertex(m_figurePoly,convertFigureUnitsToInches(m_ligXcoordinate2),
                                                              convertFigureUnitsToInches(m_ligYcoordinate2));
               }

               if (m_ligNumberOfPoints >= 3)
               {
                  getCamCadDatabase().addVertex(m_figurePoly,convertFigureUnitsToInches(m_ligXcoordinate3),
                                                              convertFigureUnitsToInches(m_ligYcoordinate3));
               }

               if (m_ligNumberOfPoints >= 4)
               {
                  getCamCadDatabase().addVertex(m_figurePoly,convertFigureUnitsToInches(m_ligXcoordinate4),
                                                              convertFigureUnitsToInches(m_ligYcoordinate4));
               }

               if (m_ligNumberOfPoints >= 5)
               {
                  getCamCadDatabase().addVertex(m_figurePoly,convertFigureUnitsToInches(m_ligXcoordinate5),
                                                              convertFigureUnitsToInches(m_ligYcoordinate5));
               }

               if (m_ligContinuationCode == "E")
               {
                  m_figurePoly = NULL;
               }

               break;
            // REC : Rectangle definition
            case EntityCode3(R,E,C):
               {
                  processedFlag = true;

                  if (m_figurePolyStruct == NULL)
                  {
                     m_figurePolyStruct = getCamCadDatabase().addPolyStruct(m_figureBlock,getFigureLayerIndex(),0,0, graphicClassNormal);
                     m_figurePoly = NULL;
                  }

                  bool newPolyFlag = false;

                  if (m_figurePoly == NULL)
                  {
                     newPolyFlag = true;
                     m_figurePoly       = getCamCadDatabase().addFilledPoly(m_figurePolyStruct,0);
                  }

                  double x1 = convertFigureUnitsToInches(m_recXcoordinateOfExtremity1);
                  double y1 = convertFigureUnitsToInches(m_recYcoordinateOfExtremity1);
                  double x2 = convertFigureUnitsToInches(m_recXcoordinateOfExtremity2);
                  double y2 = convertFigureUnitsToInches(m_recYcoordinateOfExtremity2);

                  getCamCadDatabase().addVertex(m_figurePoly,x1,y1);
                  getCamCadDatabase().addVertex(m_figurePoly,x1,y2);
                  getCamCadDatabase().addVertex(m_figurePoly,x2,y2);
                  getCamCadDatabase().addVertex(m_figurePoly,x2,y1);
                  getCamCadDatabase().addVertex(m_figurePoly,x1,y1);

                  if (newPolyFlag)
                  {
                     m_figurePoly = NULL;
                  }
               }

               break;
            // CER : Circle definition
            case EntityCode3(C,E,R):
               {
                  processedFlag = true;

                  if (m_figurePolyStruct == NULL)
                  {
                     m_figurePolyStruct = getCamCadDatabase().addPolyStruct(m_figureBlock,getFigureLayerIndex(),0,0, graphicClassNormal);
                     m_figurePoly = NULL;
                  }

                  bool newPolyFlag = false;

                  if (m_figurePoly == NULL)
                  {
                     newPolyFlag = true;
                     m_figurePoly       = getCamCadDatabase().addFilledPoly(m_figurePolyStruct,0);
                  }

                  double x = convertFigureUnitsToInches(m_cerXcoordinateOfCircleCenter);
                  double y = convertFigureUnitsToInches(m_cerYcoordinateOfCircleCenter);
                  double r = convertFigureUnitsToInches(m_cerRadius);

                  getCamCadDatabase().addVertex(m_figurePoly,x - r,y - r,1.);
                  getCamCadDatabase().addVertex(m_figurePoly,x + r,y + r,1.);
                  getCamCadDatabase().addVertex(m_figurePoly,x - r,y - r,0.);

                  if (newPolyFlag)
                  {
                     m_figurePoly = NULL;
                  }
               }

               break;
            // ARC : Arc definition
            case EntityCode3(A,R,C):
               {
                  processedFlag = true;

                  if (m_figurePolyStruct == NULL)
                  {
                     m_figurePolyStruct = getCamCadDatabase().addPolyStruct(m_figureBlock,getFigureLayerIndex(),0,0, graphicClassNormal);
                     m_figurePoly = NULL;
                  }

                  bool newPolyFlag = false;

                  if (m_figurePoly == NULL)
                  {
                     newPolyFlag = true;
                     m_figurePoly       = getCamCadDatabase().addFilledPoly(m_figurePolyStruct,0);
                  }

                  int startAngleCentiDegrees = ((m_arcDirectionOfRotation == 1) ? m_arcAngleAtStartOfTheArc : m_arcAngleAtEndOfTheArc);
                  int endAngleCentiDegrees   = ((m_arcDirectionOfRotation == 1) ? m_arcAngleAtEndOfTheArc   : m_arcAngleAtStartOfTheArc);

                  double xc = convertFigureUnitsToInches(m_arcXcoordinateOfCircleCenter);
                  double yc = convertFigureUnitsToInches(m_arcYcoordinateOfCircleCenter);
                  double r  = convertFigureUnitsToInches(m_arcRadiusOfTheCircle);
                  double a0 = degreesToRadians(startAngleCentiDegrees/100.);
                  double a1 = degreesToRadians(endAngleCentiDegrees/100.);

                  if (a1 < a0) a1 += 2*Pi;

                  double x0 = xc + r * cos(a0);
                  double y0 = yc + r * sin(a0);
                  double x1 = xc + r * cos(a1);
                  double y1 = yc + r * sin(a1);
                  double bulge = tan((a1 - a0)/4.);

                  getCamCadDatabase().addVertex(m_figurePoly,x0,y0,bulge);
                  getCamCadDatabase().addVertex(m_figurePoly,x1,y1,0.);

                  if (newPolyFlag)
                  {
                     m_figurePoly = NULL;
                  }
               }

               break;
            // PNT : Point definition
            case EntityCode3(P,N,T):
               {
                  processedFlag = true;

                  if (m_figurePolyStruct == NULL)
                  {
                     m_figurePolyStruct = getCamCadDatabase().addPolyStruct(m_figureBlock,getFigureLayerIndex(),0,0, graphicClassNormal);
                     m_figurePoly = NULL;
                  }

                  bool newPolyFlag = false;

                  if (m_figurePoly == NULL)
                  {
                     newPolyFlag = true;
                     m_figurePoly       = getCamCadDatabase().addFilledPoly(m_figurePolyStruct,0);
                  }

                  double x = convertFigureUnitsToInches(m_pntXcoordinateOfPoint);
                  double y = convertFigureUnitsToInches(m_pntYcoordinateOfPoint);

                  getCamCadDatabase().addVertex(m_figurePoly,x,y);
                  getCamCadDatabase().addVertex(m_figurePoly,x,y);

                  if (newPolyFlag)
                  {
                     m_figurePoly = NULL;
                  }
               }

               break;
            // TXT : Text definition
            case EntityCode3(T,X,T):
               {
                  processedFlag = true;

                  double x = convertFigureUnitsToInches(m_txtXcoordOfTheAlignmentPoint);
                  double y = convertFigureUnitsToInches(m_txtYcoordOfTheAlignmentPoint);
                  CString text = m_txtTextCharacters;

                  if (text.GetLength() > m_txtNumberOfCharactersInTheText && m_txtNumberOfCharactersInTheText >= 0)
                  {
                     text.GetBufferSetLength(m_txtNumberOfCharactersInTheText);
                  }

                  text.TrimRight();
                  double height = .05;  // default height
                  double charWidth  = height / 2.;

                  getCamCadDatabase().addText(m_figureBlock,getFigureLayerIndex(),text,x,y,height,charWidth);
               }

               break;
            // EMP : Parameterized text location
            case EntityCode3(E,M,P):
               processedFlag = true;
               break;
            // ATL : Line attribute
            case EntityCode3(A,T,L):
               processedFlag = true;
               break;
            // ATH : Hatched area attribute
            case EntityCode3(A,T,H):
               processedFlag = true;
               break;
            // ATT : Text attribute
            case EntityCode3(A,T,T):
               processedFlag = true;

errorLog.writef(PrefixError,
"ATT attribute processing is not implemented.\n",
(const char*)m_uniMmOrIn);

               break;
            }
         }
         else
         {
            switch (m_entityCode)
            {
            // DEB : Figure description start
            case EntityCode3(D,E,B):
               processedFlag = true;

               m_figureBlock = getCamCadDatabase().getDefinedBlock(m_debName,blockTypeUnknown,getBoardFileNum());
               m_figurePolyStruct = NULL;

               break;
            }
         }

         break;
      }

      if (processedFlag)
      {
         break;
      }
   }

   if (!processedFlag)
   {
errorLog.writef(PrefixError,
"Unexpected record encountered, blockNumber=%d, '%s'\n",
m_blockNumber,(const char*)m_recordString);
   }
   else
   {
      m_blockNumber = blockNumber;
   }

   return retval;
}

BlockStruct* CAlcatelDocicaDatabase::getPadStack(const CString& padSymbolName,bool thruHoleFlag,
   bool topSideFlag,int holeDiameterOrPadOrientation,CWriteFormat& errorLog)
{
   CString padStackName = "PS_" + padSymbolName + (thruHoleFlag ? "_TH" : "_SM");

   BlockStruct* padStackBlock = getCamCadDatabase().getBlock(padStackName,getBoardFileNum());

   if (padStackBlock == NULL)
   {
      // instantiate the padStack
      padStackBlock = getCamCadDatabase().getDefinedBlock(padStackName,blockTypePadstack,getBoardFileNum());

      double x = toInches(m_cpinXcoordOfTheHoleArea);
      double y = toInches(m_cpinYcoordOfTheHoleArea);

      if (thruHoleFlag)
      {
         double unfinishedHoleSize = toInches(holeDiameterOrPadOrientation);
         int drillLayerIndex    = getCamCadDatabase().getCamCadLayerIndex(ccLayerDrillHoles);
         BlockStruct* toolBlock = getCamCadDatabase().getDefinedTool(unfinishedHoleSize);

         DataStruct* toolReference = getCamCadDatabase().referenceBlock(padStackBlock,toolBlock,INSERTTYPE_UNKNOWN,NULL,drillLayerIndex);
      }
      else
      {
         getCamCadDatabase().addAttribute(getAttributeMap(padStackBlock),
            getAttributeIndex(attributeIndexSmdShape),errorLog);
      }

      BlockStruct* padBlock = getCamCadDatabase().getDefinedBlock(padSymbolName,blockTypePadshape,getBoardFileNum());

      int layerIndex = getCamCadDatabase().getCamCadLayerIndex(ccLayerPadTop);

      // reference the pad from the padStack
      getCamCadDatabase().referenceBlock(padStackBlock,padBlock,
         INSERTTYPE_UNKNOWN,NULL,layerIndex);
   }

   return padStackBlock;
}

BlockStruct* CAlcatelDocicaDatabase::getViaPadStack(int holeDiameter,
   bool topPadFlag,bool bottomPadFlag,int padDiameter)
{
   CString padStackName;

   padStackName.Format("VIA_%d",holeDiameter);

   if (!topPadFlag && !bottomPadFlag)
   {
      padStackName += "_SM";
   }
   else if (topPadFlag != bottomPadFlag)
   {
      padStackName += (topPadFlag ? "_T" : "_B");
   }

   BlockStruct* padStackBlock = getCamCadDatabase().getBlock(padStackName,getBoardFileNum());

   if (padStackBlock == NULL)
   {
      // instantiate the padStack
      padStackBlock = getCamCadDatabase().getDefinedBlock(padStackName,blockTypePadstack,getBoardFileNum());

      if (holeDiameter != 0)
      {
         double unfinishedHoleSize = toInches(holeDiameter);
         int drillLayerIndex    = getCamCadDatabase().getCamCadLayerIndex(ccLayerDrillHoles);
         BlockStruct* toolBlock = getCamCadDatabase().getDefinedTool(unfinishedHoleSize);

         DataStruct* toolReference = 
            getCamCadDatabase().referenceBlock(padStackBlock,toolBlock,INSERTTYPE_UNKNOWN,NULL,drillLayerIndex);
      }

      if (topPadFlag || bottomPadFlag)
      {
         CString apertureName;
         apertureName.Format("RND%d",padDiameter);
         double diameterInInches = toInches(padDiameter);
         double sizeA    = diameterInInches;
         double sizeB    = diameterInInches;
         double xOffset  = 0.;
         double yOffset  = 0.;
         double rotation = 0.;

         BlockStruct* padBlock = getCamCadDatabase().getDefinedAperture(apertureName,apertureRound,sizeA,sizeB,xOffset,yOffset,rotation);

         if (topPadFlag)
         {
            int layerIndex = getCamCadDatabase().getCamCadLayerIndex(ccLayerPadTop);

            // reference the pad from the padStack
            getCamCadDatabase().referenceBlock(padStackBlock,padBlock,INSERTTYPE_UNKNOWN,NULL,layerIndex);
         }

         if (bottomPadFlag)
         {
            int layerIndex = getCamCadDatabase().getCamCadLayerIndex(ccLayerPadBottom);

            // reference the pad from the padStack
            getCamCadDatabase().referenceBlock(padStackBlock,padBlock,INSERTTYPE_UNKNOWN,NULL,layerIndex);
         }
      }
   }

   return padStackBlock;
}







