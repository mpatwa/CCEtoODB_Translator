// $Header: /CAMCAD/4.6/read_wrt/AlcGmfIn.cpp 26    2/23/07 9:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "StdAfx.h"
#include "CcDoc.h"
#include "CamCadDatabase.h"
#include "AlcGMFin.h"
#include "WriteFormat.h"
#include "CCEtoODB.h"

/******************************************************************************
* ReadAlcatelGMF
*/
void ReadAlcatelGMF(const char *filePath, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   ASSERT(Doc != NULL);

   CCamCadDatabase camCadDatabase(*Doc);
   CAlcatelGmfDatabase database(camCadDatabase);
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
      errorLog.writef("CAMCAD Alcatel GMF reader. %s\n",(const char*)configuration);

      database.read(filePath,pageUnitsUndefined,errorLog);
   }

   errorLog.close();

   //if (display_error && display_log)
      Logreader(errorLogFilePath.getPath());
}

//=============================================================================

#define NoLayer (-1)

double parseDegrees(const char* degreesString)
{
   double degrees = atof(degreesString);

   if (strlen(degreesString) > 4)
   {
      degrees /= 1000.;
   }

   return degrees;
}

CString genPadStackName(const CString& ltpName)
{
   return "PS_" + ltpName;
}

AlcatelGmfFileSectionTag stringToAlcatelGmfFileSectionTag(const CString& tagString)
{
   AlcatelGmfFileSectionTag retval = sectUndefined;

   if      (tagString == "FIL") retval = sectFil;
   else if (tagString == "MNG") retval = sectMng;
   else if (tagString == "REV") retval = sectRev;
   else if (tagString == "BOM") retval = sectBom;
   else if (tagString == "TEC") retval = sectTec;
   else if (tagString == "ASM") retval = sectAsm;
   else if (tagString == "PPN") retval = sectPpn;
   else if (tagString == "NET") retval = sectNet;
   else if (tagString == "ECD") retval = sectEcd;
   else if (tagString == "LTP") retval = sectLtp;
   else if (tagString == "PLB") retval = sectPlb;
   else if (tagString == "ICT") retval = sectIct;
   else if (tagString == "DRL") retval = sectDrl;
   else if (tagString == "RUT") retval = sectRut;
   else if (tagString == "ONS") retval = sectOns;

   return retval;
}

OutlineKindTag stringToOutlineKindTag(const CString& tagString)
{
   OutlineKindTag retval = olUndefined;

   if      (tagString == "BOARDARA") retval = olBoard;
   else if (tagString == "MOUNTARA") retval = olPartMountingArea;
   else if (tagString == "MNTIHARA") retval = olMountingInhibitArea;
   else if (tagString == "RESSTARA") retval = olResistArea;
   else if (tagString == "RSTIHARA") retval = olResistInhibitArea;
   else if (tagString == "ROUTARA" ) retval = olRoutingArea;
   else if (tagString == "NCOPARA" ) retval = olNegativeCopperPowerLayerArea;
   else if (tagString == "LINIHARA") retval = olLineInhibitArea;
   else if (tagString == "VIAIHARA") retval = olViaInhibitArea;
   else if (tagString == "COPIHARA") retval = olCopperInhibitArea;
   else if (tagString == "NCPIHARA") retval = olNegativeCopperPowerLayerInhibitArea;
   else if (tagString == "DDRWFIG" ) retval = olDrawingFigure;
   else if (tagString == "SILKFIG" ) retval = olSilkFigure;
   else if (tagString == "KPOUTARA") retval = olMountingInhibitArea;
   else if (tagString == "LVAIHARA") retval = olViaInhibitArea;
   else if (tagString == "RESIHARA") retval = olResistInhibitArea;

   return retval;
}

EntityTypeTag stringToEntityTypeTag(const CString& tagString)
{
   EntityTypeTag retval = entityUndefined;

   if      (tagString == "LIN" ) retval = entityLine;
   else if (tagString == "CIR" ) retval = entityCircle;
   else if (tagString == "ARC" ) retval = entityArc;
   else if (tagString == "POLY") retval = entityPoly;
   else if (tagString == "RECT") retval = entityRectangle;
   else if (tagString == "OVAL") retval = entityOval;
   else if (tagString == "RCT2") retval = entityTrimmedRectangle;
   else if (tagString == "RCT3") retval = entityRoundedRectangle;

   return retval;
}

struct sAlcatelGmfFileRecordTypeTableEntry
{
   AlcatelGmfFileRecordTypeTag recordType;
   const char* enumString;
};

sAlcatelGmfFileRecordTypeTableEntry alcatelGmfFileRecordTypeTable[] =
{
   { alcatelKwArea      , "AREA"     } ,
   { alcatelKwAsm       , "ASM"      } ,
   { alcatelKwBkpotara  , "BKPOTARA" } ,
   { alcatelKwBoard     , "BOARD"    } ,
   { alcatelKwBom       , "BOM"      } ,
   { alcatelKwComent    , "COMENT"   } ,
   { alcatelKwCompnt    , "COMPNT"   } ,
   { alcatelKwCopihara  , "COPIHARA" } ,
   { alcatelKwCopper    , "COPPER"   } ,
   { alcatelKwCutdat    , "CUTDAT"   } ,
   { alcatelKwDate      , "DATE"     } ,
   { alcatelKwDdrwfig   , "DDRWFIG"  } ,
   { alcatelKwDist      , "DIST"     } ,
   { alcatelKwDocum     , "DOCUM"    } ,
   { alcatelKwDrl       , "DRL"      } ,
   { alcatelKwEcd       , "ECD"      } ,
   { alcatelKwEnd       , "END"      } ,
   { alcatelKwFil       , "FIL"      } ,
   { alcatelKwFile      , "FILE"     } ,
   { alcatelKwFormat    , "FORMAT"   } ,
   { alcatelKwHead      , "HEAD"     } ,
   { alcatelKwIct       , "ICT"      } ,
   { alcatelKwJmpdat    , "JMPDAT"   } ,
   { alcatelKwKpoutara  , "KPOUTARA" } ,
   { alcatelKwLand      , "LAND"     } ,
   { alcatelKwLayer     , "LAYER"    } ,
   { alcatelKwLayno     , "LAYNO"    } ,
   { alcatelKwLayout    , "LAYOUT"   } ,
   { alcatelKwLine      , "LINE"     } ,
   { alcatelKwLinihara  , "LINIHARA" } ,
   { alcatelKwLndfig    , "LNDFIG"   } ,
   { alcatelKwLtp       , "LTP"      } ,
   { alcatelKwLvaihara  , "LVAIHARA" } ,
   { alcatelKwMng       , "MNG"      } ,
   { alcatelKwNcpihara  , "NCPIHARA" } ,
   { alcatelKwNet       , "NET"      } ,
   { alcatelKwNetdef    , "NETDEF"   } ,
   { alcatelKwNetend    , "NETEND"   } ,
   { alcatelKwNetlst    , "NETLST"   } ,
   { alcatelKwOns       , "ONS"      } ,
   { alcatelKwOrigin    , "ORIGIN"   } ,
   { alcatelKwPanel     , "PANEL"    } ,
   { alcatelKwPddef     , "PDDEF"    } ,
   { alcatelKwPddrwfig  , "PDDRWFIG" } ,
   { alcatelKwPdent     , "PDENT"    } ,
   { alcatelKwPin       , "PIN"      } ,
   { alcatelKwPlb       , "PLB"      } ,
   { alcatelKwPosmk     , "POSMK"    } ,
   { alcatelKwPpn       , "POPN"     } ,
   { alcatelKwPrtdef    , "PRTDEF"   } ,
   { alcatelKwPrtend    , "PRTEND"   } ,
   { alcatelKwPtndef    , "PTNDEF"   } ,
   { alcatelKwPtnend    , "PTNEND"   } ,
   { alcatelKwResihara  , "RESIHARA" } ,
   { alcatelKwRev       , "REV"      } ,
   { alcatelKwRottyp    , "ROTTYP"   } ,
   { alcatelKwRut       , "RUT"      } ,
   { alcatelKwSilkfig   , "SILKFIG"  } ,
   { alcatelKwSystem    , "SYSTEM"   } ,
   { alcatelKwTec       , "TEC"      } ,
   { alcatelKwTstpnt    , "TSTPNT"   } ,
   { alcatelKwTypdef    , "TYPDEF"   } ,
   { alcatelKwTypend    , "TYPEND"   } ,
   { alcatelKwUnit      , "UNIT"     } ,
   { alcatelKwVia       , "VIA"      } ,
   { alcatelKwViaihara  , "VIAIHARA" } ,
   { alcatelKwView      , "VIEW"     } ,

   { alcatelKwUndefined , ""         }
};

AlcatelGmfFileRecordTypeTag stringToAlcatelGmfFileRecordTypeTag(const CString& tagString)
{
   AlcatelGmfFileRecordTypeTag retval = alcatelKwUndefined;

   const int numAlcatelRecordTypeTags = sizeof(alcatelGmfFileRecordTypeTable) / sizeof(sAlcatelGmfFileRecordTypeTableEntry);

   for (int ind = 0;ind < numAlcatelRecordTypeTags;ind++)
   {
      if (tagString == alcatelGmfFileRecordTypeTable[ind].enumString)
      {
         retval = alcatelGmfFileRecordTypeTable[ind].recordType;
         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
CCompArchitypePin::CCompArchitypePin(int sequenceNumber,const CString& pinName,const CString& function)
{
   m_sequenceNumber = sequenceNumber;
   m_pinName        = pinName;
   m_function       = function;
}

bool CCompArchitypePin::operator==(const CCompArchitypePin& other)
{
   bool retval = (m_sequenceNumber == other.m_sequenceNumber &&
                  m_pinName        == other.m_pinName        &&
                  m_function       == other.m_function           ); 

   return retval;
}

//_____________________________________________________________________________
CCompArchitype::CCompArchitype(const CString& plbName,const CString& partNumber,const CString& refdes)
{
   m_plbName    = plbName;
   m_partNumber = partNumber;
   m_refdes     = refdes;
}

CCompArchitype::~CCompArchitype()
{
   for (int ind=0;ind < m_pins.GetCount();ind++)
   {
      CCompArchitypePin* pin = m_pins.GetAt(ind);
      delete pin;
   }
}

CString CCompArchitype::getName()
{
   CString name = m_plbName;

   if (m_partNumber.GetLength() != 0)
   {
      name += "_" + m_partNumber;

      if (m_refdes.GetLength() != 0)
      {
         name += "_" + m_refdes;
      }
   }

   return name;
}

CString CCompArchitype::getPopName()
{
   CString name = m_plbName;

   if (m_partNumber.GetLength() != 0 && m_refdes.GetLength() != 0)
   {
      name += "_" + m_partNumber;
   }

   return name;
}

bool CCompArchitype::popName()
{
   bool retval = false;

   if (m_refdes.GetLength() != 0)
   {
      m_refdes.GetBufferSetLength(0);
      retval = true;
   }
   else if (m_partNumber.GetLength() != 0)
   {
      m_partNumber.GetBufferSetLength(0);
      retval = true;
   }

   return retval;
}

CCompArchitypePin* CCompArchitype::addPin(int sequenceNumber,const CString& pinName,const CString& function)
{
   CCompArchitypePin* pin = NULL;

   if (sequenceNumber < m_pins.GetCount())
   {
      pin = m_pins.GetAt(sequenceNumber);
   }

   if (pin == NULL)
   {
      pin = new CCompArchitypePin(sequenceNumber,pinName,function);
      m_pins.SetAtGrow(sequenceNumber,pin);
   }

   return pin;
}

CCompArchitypePin* CCompArchitype::getPinAt(int sequenceNumber)
{
   CCompArchitypePin* pin = NULL;

   if (sequenceNumber >= 0 && sequenceNumber < m_pins.GetCount())
   {
      pin = m_pins.GetAt(sequenceNumber);
   }

   return pin;
}

bool CCompArchitype::pinsEqual(const CCompArchitype& other) const
{
   bool retval = (m_pins.GetCount() == other.m_pins.GetCount());

   for (int ind=0;retval && ind < m_pins.GetCount();ind++)
   {
      CCompArchitypePin* pin = m_pins.GetAt(ind);
      CCompArchitypePin* otherPin = other.m_pins.GetAt(ind);

      if (pin == NULL && otherPin == NULL)
      {
         continue;
      }

      retval = (pin != NULL && otherPin != NULL && (*pin == *otherPin));
   }

   return retval;
}

//_____________________________________________________________________________
CCompArchitypes::CCompArchitypes()
{
}

CCompArchitypes::~CCompArchitypes()
{
   CCompArchitype* architype;
   CString name;

   for (POSITION pos = m_architypes.GetStartPosition();pos != NULL;)
   {
      m_architypes.GetNextAssoc(pos,name,architype);
      delete architype;
   }
}

CCompArchitype* CCompArchitypes::addArchitype(const CString& plbName,const CString& partNumber,const CString& refdes)
{
   CCompArchitype* architype = NULL;

#ifdef _DEBUG
   CCompArchitype* establishedArchitype;
#endif
   
   if (!m_architypesByRefdes.Lookup(refdes,architype))
   {
      architype = new CCompArchitype(plbName,partNumber,refdes);

      ASSERT(!m_architypes.Lookup(architype->getName(),establishedArchitype));

      m_architypes.SetAt(architype->getName(),architype);
      m_architypesByRefdes.SetAt(refdes,architype);
   }

   return architype;
}

CCompArchitype* CCompArchitypes::addPin(const CString& refdes,int sequenceNumber,const CString& pinName,const CString& function)
{
   CCompArchitype* architype = NULL;
   
   if (m_architypesByRefdes.Lookup(refdes,architype))
   {
      architype->addPin(sequenceNumber,pinName,function);
   }
   else
   {
      ASSERT(0);
   }

   return architype;
}

// returns true if a redundancy occured
bool CCompArchitypes::removeRedundancies(const CString& refdes)
{
   bool retval = false;
   CCompArchitype* architype = NULL;
   CString establishedName;
   
   if (m_architypesByRefdes.Lookup(refdes,architype))
   {
      CCompArchitype* establishedArchitype;

      for (POSITION pos = m_architypes.GetStartPosition();pos != NULL && !retval;)
      {
         m_architypes.GetNextAssoc(pos,establishedName,establishedArchitype);

         if (establishedArchitype->getPlbName() == architype->getPlbName() &&
             establishedArchitype != architype)
         {
            retval = architype->pinsEqual(*establishedArchitype);
         }
      }

      if (retval)
      {
         m_architypesByRefdes.SetAt(refdes,establishedArchitype);
         m_architypes.RemoveKey(architype->getName());
         delete architype;
      }
      else
      {
         CString newName = architype->getPopName();

         if (! m_architypes.Lookup(newName,establishedArchitype))
         {
            m_architypes.RemoveKey(architype->getName());
            architype->popName();

            newName = architype->getPopName();

            if (! m_architypes.Lookup(newName,establishedArchitype))
            {
               architype->popName();
            }

            m_architypes.SetAt(architype->getName(),architype);
         }
      }
   }
   else
   {
      ASSERT(0);
   }

   return retval;
}

int CCompArchitypes::getPlbNames(CStringArray& plbNames,const CString& plbName)
{
   plbNames.RemoveAll();

   CCompArchitype* architype;
   CString name;

   for (POSITION pos = m_architypes.GetStartPosition();pos != NULL;)
   {
      m_architypes.GetNextAssoc(pos,name,architype);

      if (architype->getPlbName() == plbName)
      {
         plbNames.Add(architype->getName());
      }
   }

   return (int)plbNames.GetCount();
}

//_____________________________________________________________________________
CAlcatelGmfRecord::CAlcatelGmfRecord()
{
   m_params.SetSize(0,10000);
}

CString CAlcatelGmfRecord::getParam(int index)
{
   ASSERT(index < getParamCount());

   return m_params[index];
}

CPoint2d CAlcatelGmfRecord::getCoord(int index)
{
   ASSERT(index < getParamCount());

   CPoint2d coordinate;
   int x=0,y,junk;

   int* ordinate = &x;
   int sign = 1;

   for (const char* p = m_params[index];*p != '\0';p++)
   {
      switch (*p)
      {
      case 'X':
         ordinate = &x;
         *ordinate = 0;
         sign = 1;
         break;
      case 'Y':
         ordinate = &y;
         *ordinate = 0;
         sign = 1;
         break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         *ordinate = 10 * (*ordinate) + sign * (*p - '0');
         break;
      case '-':
         sign = -1;
         break;
      default:
         ordinate = &junk;
         *ordinate = 0;
         break;
      }
   }

   coordinate.x = x;
   coordinate.y = y;

   return coordinate;
}

CPoint2d CAlcatelGmfRecord::getCoordInInches(int index)
{
   ASSERT(index < getParamCount());

   CPoint2d param = getCoord(index);
   m_alcatelGmfDatabase->convertToInches(param);

   return param;
}

int CAlcatelGmfRecord::getInt(int index)
{
   ASSERT(index < getParamCount());

   int param = atoi(m_params[index]);

   return param;
}

int CAlcatelGmfRecord::getIntInches(int index)
{
   ASSERT(index < getParamCount());

   int param = atoi(m_params[index]);
   m_alcatelGmfDatabase->convertToInches(param);

   return param;
}

double CAlcatelGmfRecord::getDouble(int index)
{
   ASSERT(index < getParamCount());

   double param = atof(m_params[index]);

   return param;
}

double CAlcatelGmfRecord::getDegrees(int index)
{
   ASSERT(index < getParamCount());

   double degrees = atof(m_params[index]);

   if (m_params[index].GetLength() > 4)
   {
      degrees /= 1000.;
   }

   return degrees;
}

double CAlcatelGmfRecord::getInches(int index)
{
   ASSERT(index < getParamCount());

   double param = atof(m_params[index]);
   m_alcatelGmfDatabase->convertToInches(param);

   return param;
}

bool CAlcatelGmfRecord::isGood(AlcatelGmfFileSectionTag section,AlcatelGmfFileRecordTypeTag recordType,CWriteFormat& log)
{
   bool retval = false;

   if (m_recordType == recordType)
   {
      switch(recordType)
      {
      case alcatelKwEnd:
         retval = (getParamCount() >= 2);
         break;
      case alcatelKwUnit:
         retval = (getParamCount() >= 2);
         break;
      case alcatelKwLayno:
         retval = (getParamCount() >= 2);
         break;
      case alcatelKwLayer:
         retval = (getParamCount() >= 4);
         break;
      case alcatelKwLayout:
         retval = (getParamCount() >= 4);
         break;
      case alcatelKwArea:
         retval = (getParamCount() >= 8);
         break;
      case alcatelKwCompnt:
         retval = ( ((section == sectAsm) && (getParamCount() >= 16)) || ((section == sectBom) && (getParamCount() >= 10)) );
         break;
      case alcatelKwPtndef:
         retval = (getParamCount() >= 3);
         break;
      case alcatelKwPtnend:
         retval = (getParamCount() >= 1);
         break;
      case alcatelKwPin:
         retval = ( ((section == sectPlb) && getParamCount() >= 8) || ((section == sectPpn) && (getParamCount() >= 6)) );
         break;
      case alcatelKwTypdef:
         retval = (getParamCount() >= 9);
         break;
      case alcatelKwTypend:
         retval = (getParamCount() >= 1);
         break;
      case alcatelKwLndfig:
         retval = (getParamCount() >= 5);
         break;
      case alcatelKwDdrwfig:
      case alcatelKwSilkfig:
      case alcatelKwKpoutara:
      case alcatelKwBkpotara:
      case alcatelKwViaihara:
      case alcatelKwLvaihara:
      case alcatelKwLinihara:
      case alcatelKwCopihara:
      case alcatelKwNcpihara:
      case alcatelKwResihara:
         retval = (getParamCount() >= 7);
         break;   
      case alcatelKwNetdef:
         retval = (getParamCount() >= 2);
         break;         
      case alcatelKwLine:
         retval = (getParamCount() >= 4);
         break;         
      case alcatelKwVia:
         retval = (getParamCount() >= 6);
         break;         
      case alcatelKwLand:
         retval = (getParamCount() >= 6);
         break;         
      case alcatelKwCopper:
         retval = (getParamCount() >= 4);
         break;         
      case alcatelKwNetend:
         retval = (getParamCount() >= 1);
         break;         
      case alcatelKwNetlst:
         retval = (getParamCount() >= 5);
         break;         
      case alcatelKwPosmk:
         retval = (getParamCount() >= 8);
         break;         
      case alcatelKwPrtdef:
         retval = (getParamCount() >= 2);
         break;         
      case alcatelKwPrtend:
         retval = (getParamCount() >= 2);
         break;         
      }

      if (!retval)
      {
log.writef(PrefixError,"Invalid Alcatel Record encountered, '%s'.\n",
           (getParamCount() > 0 ? (const char*)getParam(0) : "?"));
      }
   }

   return retval;
}

CAlcatelGmfRecord& CAlcatelGmfRecord::operator=(const CString& recordString)
{
   m_recordString = recordString;
   m_recordString.Parse(m_params," :;,",true,m_maxParams);
   m_recordType = (getParamCount() > 0 ? stringToAlcatelGmfFileRecordTypeTag(m_params[0]) : alcatelKwUndefined);

   return *this;
}

//_____________________________________________________________________________
CAlcatelGmfDatabase::CAlcatelGmfDatabase(CCamCadDatabase& camCadDatabase)
: CAlienDatabase(camCadDatabase,fileTypeAlcatelGmf)
{
   m_currentSection = sectUndefined;
   m_plbBlock       = NULL;
   m_ltpBlock       = NULL;

   m_numberOfElectricalLayers = -1;
   m_signalLayerCount         =  0;

   m_attributeIndexDirectory.SetSize(20,20);

   m_thruHoleFlag   = false;
   m_layerRelatedType      = -1;
   m_topLayerIndex         = -1;
   m_bottomLayerIndex      = -1;
   m_minSignalLayerIndex   = -1;
   m_maxSignalLayerIndex   = -1;

   m_record.setDatabase(this);

   // Rut section
   m_rutLineWidth  = 0.;
   m_rutPolyStruct = NULL;
   m_rutPoly       = NULL;
}

int CAlcatelGmfDatabase::getAttributeIndex(AttributeIndexTag attributeIndexTag)
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
      case attributeIndexNetName:            attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_NETNAME     );  break;
      case attributeIndexValue:              attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_VALUE       );  break;
      case attributeIndexSmdShape:           attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_SMDSHAPE    );  break;
      case attributeIndexVoltage:            attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_VOLTAGE     );  break;
      case attributeIndexTolerance:          attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_TOLERANCE   );  break;
      case attributeIndexTypeListLink:       attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_TYPELISTLINK);  break;
      case attributeIndexCompHeight:         attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_COMPHEIGHT  );  break;
      case attributeIndexPowerNet:           attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_POWERNET    );  break;
      case attributeIndexCompPinnr:          attributeIndex = getCamCadDatabase().getKeywordIndex(ATT_COMPPINNR   );  break;

      case attributeIndexNumberOfComps:      attributeIndex = getCamCadDatabase().registerKeyword("NUMBEROFCOMPS" ,VT_INTEGER);  break;
      case attributeIndexEsd:                attributeIndex = getCamCadDatabase().registerKeyword("ESD"           ,VT_STRING );  break;
      case attributeIndexHeatMoisture:       attributeIndex = getCamCadDatabase().registerKeyword("HEATMOISTURE"  ,VT_STRING );  break;
      case attributeIndexGenericName:        attributeIndex = getCamCadDatabase().registerKeyword("GENERICNAME"   ,VT_STRING );  break;
      case attributeIndexPhysicalRefDes:     attributeIndex = getCamCadDatabase().registerKeyword("PHYSICALREFDES",VT_STRING );  break;
      case attributeIndexLayerPowerPos:      attributeIndex = getCamCadDatabase().registerKeyword("POWERPOS"      ,VT_NONE   );  break;
      case attributeIndexLayerSignalTop:     attributeIndex = getCamCadDatabase().registerKeyword("SIGNALTOP"     ,VT_NONE   );  break;
      case attributeIndexLayerSignalBottom:  attributeIndex = getCamCadDatabase().registerKeyword("SIGNALBOTTOM"  ,VT_NONE   );  break;
      case attributeIndexLayerSignalInner:   attributeIndex = getCamCadDatabase().registerKeyword("SIGNALINNER"   ,VT_NONE   );  break;
      case attributeIndexPlated:             attributeIndex = getCamCadDatabase().registerKeyword("PLATED"        ,VT_NONE   );  break;
      case attributeIndexLayerSignalStack:   attributeIndex = getCamCadDatabase().registerKeyword("SIGNALSTACK"   ,VT_INTEGER);  break;
      case attributeIndexPinFunction:        attributeIndex = getCamCadDatabase().registerKeyword("PINFUNC"       ,VT_STRING );  break;
      }

      ASSERT(attributeIndex > 0);

      m_attributeIndexDirectory.SetAtGrow(attributeIndexTag,attributeIndex);
   }

   return attributeIndex;
} 

void CAlcatelGmfDatabase::initializeElectricalLayers(CWriteFormat& errorLog)
{
   int signalLayerCount = getMaxSignalLayerIndex() - getMinSignalLayerIndex() + 1;

   if (m_signalLayerCount != m_numberOfElectricalLayers)
   {
errorLog.writef(PrefixError,
"The number of defined electrical layers, %d, does not match the number of stated electrical layers, %d\n",
m_signalLayerCount,m_numberOfElectricalLayers);
   }
   else
   {
      int layerIndex,layerStackIndex,layerStackInc;

      if (m_topLayerIndex < m_bottomLayerIndex)
      {
         layerStackIndex = 1;
         layerStackInc   = 1;
      }
      else
      {
         layerStackIndex = m_numberOfElectricalLayers;
         layerStackInc   = -1;
      }

      for (layerIndex = getMinSignalLayerIndex();layerIndex <= getMaxSignalLayerIndex();layerIndex++)
      {
         LayerStruct* layer = getCamCadDatabase().getLayerAt(layerIndex);

         if (layerIndex == m_topLayerIndex)
         {
            getCamCadDatabase().addAttribute(getAttributeMap(layer),getAttributeIndex(attributeIndexLayerSignalTop),
               errorLog);
         }
         else if (layerIndex == m_bottomLayerIndex)
         {
            getCamCadDatabase().addAttribute(getAttributeMap(layer),getAttributeIndex(attributeIndexLayerSignalBottom),
               errorLog);
         }
         else
         {
            getCamCadDatabase().addAttribute(getAttributeMap(layer),getAttributeIndex(attributeIndexLayerSignalInner),
               errorLog);
         }

         layer->setElectricalStackNumber(layerStackIndex);
         getCamCadDatabase().addAttribute(getAttributeMap(layer),getAttributeIndex(attributeIndexLayerSignalStack),layerStackIndex,
            errorLog);

         layerStackIndex += layerStackInc;
      }
   }
}

void CAlcatelGmfDatabase::initializeCamCadLayerN(CamCadLayerTag layerTag,int layerType)
{
   CString firstLayerName,lastLayerName;

   for (int layerIndex = getMinSignalLayerIndex();layerIndex <= getMaxSignalLayerIndex();layerIndex++)
   {
      LayerStruct* layer = getCamCadDatabase().getLayerAt(layerIndex);
      CString layerName  = layer->getName() + " " + getCamCadDatabase().getCamCadLayerName(layerTag);
      getCamCadDatabase().getDefinedLayerIndex(layerName,false,layerType);

      if (layerIndex == getMinSignalLayerIndex())
      {
         firstLayerName = layerName;
      }

      if (layerIndex == getMaxSignalLayerIndex())
      {
         lastLayerName = layerName;
      }
   }

   if (firstLayerName != lastLayerName && firstLayerName != "" && lastLayerName != "")
   {
      Graph_Level_Mirror(firstLayerName,lastLayerName,NULL);

      // other layer names
      CString layerNamePdF  = firstLayerName + 
                              getCamCadDatabase().getPlacementDependantLayerNameSuffix();
      CString layerNamePdL  = lastLayerName + 
                              getCamCadDatabase().getPlacementDependantLayerNameSuffix();
      CString layerNamePdmF = firstLayerName + 
                              getCamCadDatabase().getPlacementDependantMirroredLayerNameSuffix();
      CString layerNamePdmL = lastLayerName + 
                              getCamCadDatabase().getPlacementDependantMirroredLayerNameSuffix();

      // Setup mirroring
      Graph_Level_Mirror(layerNamePdF ,layerNamePdL ,NULL);
      Graph_Level_Mirror(layerNamePdmF,layerNamePdmL,NULL);

      // placement dependant layers
      LayerStruct* layerPdF  = getCamCadDatabase().getDefinedLayer(layerNamePdF ,false,layerType);
      LayerStruct* layerPdmF = getCamCadDatabase().getDefinedLayer(layerNamePdmF,false,layerType);
      LayerStruct* layerPdL  = getCamCadDatabase().getDefinedLayer(layerNamePdL ,false,layerType);
      LayerStruct* layerPdmL = getCamCadDatabase().getDefinedLayer(layerNamePdmL,false,layerType);

      // setup attributes
      layerPdF->setFlagBits(LY_NEVERMIRROR);
      layerPdmF->setFlagBits(LY_MIRRORONLY);
      layerPdL->setFlagBits(LY_NEVERMIRROR);
      layerPdmL->setFlagBits(LY_MIRRORONLY);
   }

   CString layerNameNr   = getCamCadDatabase().getCamCadLayerName(layerTag);
   LayerStruct* layerNr  = getCamCadDatabase().getDefinedLayer(layerNameNr ,false,layerType);
}

void CAlcatelGmfDatabase::initializeCamCadLayers()
{
   getCamCadDatabase().initializeCamCadLayer(ccLayerBoardOutline         );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerDrillHoles           );   

   getCamCadDatabase().initializeCamCadLayer(ccLayerPadTop               );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadBottom            );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadTopPd             );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadBottomPd          );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadTopPdM            );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadBottomPdM         );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadInner             );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadAll               );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadFlash             );  
   getCamCadDatabase().initializeCamCadLayer(ccLayerPadClearance         );  
   getCamCadDatabase().initializeCamCadLayer(ccLayerMaskTop              );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMaskBottom           );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMaskTopPd            );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMaskBottomPd         );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMaskTopPdM           );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMaskBottomPdM        );  

   getCamCadDatabase().initializeCamCadLayer(ccLayerSilkTop              );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerSilkBottom           );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerSilk                 );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssemblyTop          );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssemblyBottom       );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerAssembly             );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPlaceTop             );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPlaceBottom          );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerPlace                );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMountTop             );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMountBottom          );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerMount                );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerLargeViaKeepOutTop   );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerLargeViaKeepOutBottom);    
   getCamCadDatabase().initializeCamCadLayer(ccLayerLargeViaKeepOut      );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerResistTop            );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerResistBottom         );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerResist               );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerResistInhibitTop     );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerResistInhibitBottom  );    
   getCamCadDatabase().initializeCamCadLayer(ccLayerResistInhibit        );    

   initializeCamCadLayerN(ccLayerViaKeepOutN           ,LAYTYPE_UNKNOWN);
   initializeCamCadLayerN(ccLayerRouteKeepInN          ,LAYTYPE_UNKNOWN);
   initializeCamCadLayerN(ccLayerLineInhibitN          ,LAYTYPE_UNKNOWN);
   initializeCamCadLayerN(ccLayerCopperInhibitN        ,LAYTYPE_UNKNOWN);
   initializeCamCadLayerN(ccLayerNegativeCopperN       ,LAYTYPE_UNKNOWN);
   initializeCamCadLayerN(ccLayerNegativeCopperInhibitN,LAYTYPE_UNKNOWN);
}

bool CAlcatelGmfDatabase::readRecord(CStdioFile& inputFile,int& lineNumber)
{
   bool retval = true;
   CString line;

   CString recordString = "";

   while (true)
   {
      retval = (inputFile.ReadString(line) != 0);

      if (!retval)
      {
         break;
      }

      lineNumber++;

      line.Trim();
      recordString += line;

      if (line.Right(1) == ";")
      {
         m_record = recordString;

         break;
      }
   }

   return retval;
}

// Reads an Alcatel GMF+ format text file and creates entities in a CCamCadDatabase by mapping entities
// from the Alcatel file.
bool CAlcatelGmfDatabase::read(const CString& filePathString,PageUnitsTag pageUnits,CWriteFormat& errorLog)
{
   bool retval = true;

   CStdioFile inputFile;
   setDatabaseFilePath(filePathString);

   if (! inputFile.Open(getDatabaseFilePath().getPath(),CFile::modeRead | CFile::shareDenyWrite))
   {
errorLog.writef(PrefixError,
"Could not open file '%s'\n",
getDatabaseFilePath().getPath());

      retval = false;
   }

   CString header;
   int lineNumber = 1;
   int currentState = 0;
   int previousState = 0;
   errorLog.pushHeader("");

   getBoardFile();

   while (retval)
   {
      header.Format("(%s).%06d %s ",(const char*)getDatabaseFilePath().getFileName(),lineNumber,
         (const char*)CTime::GetCurrentTime().Format("%Y%m%d.%H%M%S"));
      errorLog.popHeader();
      errorLog.pushHeader(header);

      if (!readRecord(inputFile,lineNumber))
      {
errorLog.writef(PrefixStatus,
"EOF.\n");
         break;
      }

      if ((lineNumber % 1000) == 0)
      {
         TRACE("lineNumber=%d\n",lineNumber);
      }

      switch (m_currentSection)
      {
      case sectFil:  processFilRecord(errorLog);  break;
      case sectMng:  processMngRecord(errorLog);  break;
      case sectRev:  processRevRecord(errorLog);  break;
      case sectBom:  processBomRecord(errorLog);  break;
      case sectTec:  processTecRecord(errorLog);  break;
      case sectAsm:  processAsmRecord(errorLog);  break;
      case sectPpn:  processPpnRecord(errorLog);  break;
      case sectNet:  processNetRecord(errorLog);  break;
      case sectEcd:  processEcdRecord(errorLog);  break;
      case sectLtp:  processLtpRecord(errorLog);  break;
      case sectPlb:  processPlbRecord(errorLog);  break;
      case sectIct:  processIctRecord(errorLog);  break;
      case sectDrl:  processDrlRecord(errorLog);  break;
      case sectRut:  processRutRecord(errorLog);  break;
      case sectOns:  processOnsRecord(errorLog);  break;
      case sectUndefined:
         if (m_record.getRecordType() != alcatelKwHead)
         {
errorLog.writef(PrefixError,
"Expected 'HEAD' record, but '%s' encountered.\n",
(const char*)m_record.getRecordString());
         }
         else
         {
            if (m_record.getParamCount() < 2)
            {
errorLog.writef(PrefixError,
"Missing section id in '%s'.\n",
(const char*)m_record.getRecordString());
            }
            else
            {
               CString sectionName = m_record.getParam(1);
               m_currentSection = stringToAlcatelGmfFileSectionTag(sectionName);

               if (m_currentSection == sectUndefined)
               {
errorLog.writef(PrefixError,
"Unrecognizable section id '%s' encountered int '%s'.\n",
(const char*)sectionName,
(const char*)m_record.getRecordString());
               }
               else
               {
errorLog.writef(PrefixStatus,
"Reading section '%s'.\n",
(const char*)sectionName);

                  if (m_currentSection == sectIct)
                  {
errorLog.writef(PrefixWarning,
"Handling for section '%s' is not currently implemented.\n",
(const char*)sectionName);
                  }
               }
            }
         }

         break;
      }

      if (m_record.isGood(sectUndefined,alcatelKwEnd,errorLog))
      {
         m_currentSection = sectUndefined;
         continue;
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

void CAlcatelGmfDatabase::processFilRecord(CWriteFormat& errorLog)
{
   if (m_record.isGood(sectFil,alcatelKwUnit,errorLog))
   {
      // UNIT : 0.001MM;
      double mmPerUnit =  m_record.getDouble(1);
      const double mmPerInch = 25.4;

      setInchesPerUnit(mmPerUnit / mmPerInch);
   }
}

void CAlcatelGmfDatabase::processMngRecord(CWriteFormat& errorLog)
{
}

void CAlcatelGmfDatabase::processRevRecord(CWriteFormat& errorLog)
{
}

void CAlcatelGmfDatabase::processBomRecord(CWriteFormat& errorLog)
{
   if (m_record.isGood(sectBom,alcatelKwCompnt,errorLog))
   {
      CString partNumber  = m_record.getParam(1);
      CString smdFlag     = m_record.getParam(2);
      int partCount       = m_record.getInt(3);
      CString value       = m_record.getParam(4);
      CString voltageString   = m_record.getParam(5);
      CString toleranceString = m_record.getParam(6);
      double voltage      = m_record.getDouble(5);
      double tolerance    = m_record.getDouble(6);
      CString esdFlag     = m_record.getParam(7);
      CString heatFlag    = m_record.getParam(8);
      CString genericName = m_record.getParam(9);

      TypeStruct* deviceType = getCamCadDatabase().getDefinedType(partNumber,getBoardFile());

      getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexNumberOfComps),partCount,
         errorLog);

      if (value != "")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexValue),value,
         errorLog);
      }

      if (smdFlag == "SMD")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexSmdShape),
         errorLog);
      }

      if (voltageString != "")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexVoltage),voltage,
         errorLog);
      }

      if (toleranceString != "")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexTolerance),tolerance,
         errorLog);
      }

      if (esdFlag != "")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexEsd),esdFlag,
         errorLog);
      }

      if (heatFlag != "")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexHeatMoisture),heatFlag,
         errorLog);
      }

      if (genericName != "")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(deviceType),getAttributeIndex(attributeIndexGenericName),genericName,
         errorLog);
      }
   }
}

void CAlcatelGmfDatabase::addOutlineSegment(BlockStruct* parentBlock,
   int layerIndex,int widthIndex,GraphicClassTag graphicClass,
   double x1,double y1,double x2,double y2,double bulge,bool fillFlag,bool voidFlag)
{
   DataStruct* polyStruct = getCamCadDatabase().addPolyStruct(parentBlock,layerIndex,0,0,graphicClass);
   CPoly* poly = getCamCadDatabase().addPoly(polyStruct,widthIndex,fillFlag,voidFlag,true);
   getCamCadDatabase().addVertex(poly,x1,y1,bulge);
   getCamCadDatabase().addVertex(poly,x2,y2,bulge);
}

void CAlcatelGmfDatabase::addOutlineArcSegment(BlockStruct* parentBlock,
   int layerIndex,int widthIndex,GraphicClassTag graphicClass,
   CPoint2d& arcCenter,double radius,double startAngleDegrees,double stopAngleDegrees,bool fillFlag,bool voidFlag)
{
   const double f360 = 360.;
   startAngleDegrees = fmod(fmod(startAngleDegrees,f360) + f360,f360);
   stopAngleDegrees  = fmod(fmod(stopAngleDegrees,f360) + f360,f360);
   
   if (stopAngleDegrees < startAngleDegrees) stopAngleDegrees += f360;

   double x1 = arcCenter.x + radius * cos(degreesToRadians(startAngleDegrees));
   double y1 = arcCenter.y + radius * sin(degreesToRadians(startAngleDegrees));
   double x2 = arcCenter.x + radius * cos(degreesToRadians(stopAngleDegrees));
   double y2 = arcCenter.y + radius * sin(degreesToRadians(stopAngleDegrees));
   double bulge = tan(degreesToRadians(stopAngleDegrees - startAngleDegrees)/4.);

   addOutlineSegment(parentBlock,layerIndex,widthIndex,graphicClass,x1,y1,x2,y2,bulge,fillFlag,voidFlag);
}

void CAlcatelGmfDatabase::addOutlineEntity(BlockStruct* parentBlock,
   int layerIndex,int widthIndex,GraphicClassTag graphicClass,
   CString entityType,int entityParamIndex,bool fillFlag,bool voidFlag)
{
   if (entityParamIndex < m_record.getParamCount())
   {
      if (entityType == "LIN" && m_record.getParamCount() >= entityParamIndex + 4)
      {
         CPoint2d p1(m_record.getDouble(entityParamIndex + 0),m_record.getDouble(entityParamIndex + 1));
         CPoint2d p2(m_record.getDouble(entityParamIndex + 2),m_record.getDouble(entityParamIndex + 3));

         convertToInches(p1);
         convertToInches(p2);

         addOutlineSegment(parentBlock,layerIndex,widthIndex,graphicClass,p1.x,p1.y,p2.x,p2.y,0.,fillFlag,voidFlag);
      }
      else if (m_record.getParam(3) == "ARC" && m_record.getParamCount() >= 10)
      {
         CPoint2d arcCenter(m_record.getDouble(5),m_record.getDouble(6));
         convertToInches(arcCenter);

         double radius = m_record.getDouble(7);
         convertToInches(radius);
         double startAngleDegrees = m_record.getDegrees(8);
         double endAngleDegrees   = m_record.getDegrees(9);

         addOutlineArcSegment(parentBlock,layerIndex,widthIndex,graphicClass,arcCenter,radius,startAngleDegrees,endAngleDegrees,fillFlag,voidFlag);
      }
   }
}

void CAlcatelGmfDatabase::addBoardOutlineEntity(BlockStruct* parentBlock,GraphicClassTag graphicClass)
{
   int boardOutlineLayerIndex = getCamCadDatabase().getCamCadLayerIndex(ccLayerBoardOutline);

   // make sure aperture exists
   const double width = .024;
   int widthIndex = getCamCadDatabase().getDefinedWidthIndex(width);

   addOutlineEntity(parentBlock,boardOutlineLayerIndex,widthIndex,graphicClass,m_record.getParam(3),5);
}

int CAlcatelGmfDatabase::getElectricalLayerNames(CStringArray& layerNames)
{
   layerNames.RemoveAll();

   for (int layerIndex = getMinSignalLayerIndex();layerIndex <= getMaxSignalLayerIndex();layerIndex++)
   {
      LayerStruct* layer = getCamCadDatabase().getLayerAt(layerIndex);
      layerNames.Add(layer->getName());
   }

   return (int)layerNames.GetCount();
}

bool CAlcatelGmfDatabase::addAreaPoly(CPoly* poly,EntityTypeTag entityType,int entityParameterIndex,CWriteFormat& errorLog)
{
   bool retval = true;
   const double circularBulge = 1.;

   switch(entityType)
   {
   case entityLine:
      if (m_record.getParamCount() >= entityParameterIndex + 4)
      {
         getCamCadDatabase().addVertex(poly,m_record.getInches(entityParameterIndex + 0),
                                             m_record.getInches(entityParameterIndex + 1),0.);

         getCamCadDatabase().addVertex(poly,m_record.getInches(entityParameterIndex + 2),
                                             m_record.getInches(entityParameterIndex + 3),0.);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for line entity.\n");
      }

      break;
   case entityArc:
      if (m_record.getParamCount() >= entityParameterIndex + 5)
      {
         CPoint2d arcCenter(m_record.getInches(entityParameterIndex + 0),m_record.getInches(entityParameterIndex + 1));

         double radius = m_record.getDouble(entityParameterIndex + 2);
         convertToInches(radius);
         double startAngleDegrees = m_record.getDegrees(entityParameterIndex + 3);
         double stopAngleDegrees  = m_record.getDegrees(entityParameterIndex + 4);
         const double f360 = 360.;
         startAngleDegrees = fmod(fmod(startAngleDegrees,f360) + f360,f360);
         stopAngleDegrees  = fmod(fmod(stopAngleDegrees,f360) + f360,f360);
         
         if (stopAngleDegrees < startAngleDegrees) stopAngleDegrees += f360;

         double x1 = arcCenter.x + radius * cos(degreesToRadians(startAngleDegrees));
         double y1 = arcCenter.y + radius * sin(degreesToRadians(startAngleDegrees));
         double x2 = arcCenter.x + radius * cos(degreesToRadians(stopAngleDegrees));
         double y2 = arcCenter.y + radius * sin(degreesToRadians(stopAngleDegrees));
         double bulge = tan(degreesToRadians(stopAngleDegrees - startAngleDegrees)/4.);

         getCamCadDatabase().addVertex(poly,x1,y1,bulge);
         getCamCadDatabase().addVertex(poly,x2,y2,0.);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for arc entity.\n");
      }

      break;
   case entityCircle:
      if (m_record.getParamCount() >= entityParameterIndex + 3)
      {
         CPoint2d arcCenter(m_record.getInches(entityParameterIndex + 0),m_record.getInches(entityParameterIndex + 1));

         double radius = m_record.getDouble(entityParameterIndex + 2);
         convertToInches(radius);

         getCamCadDatabase().addVertex(poly,arcCenter.x + radius,arcCenter.y,1.0);
         getCamCadDatabase().addVertex(poly,arcCenter.x - radius,arcCenter.y,1.0);
         getCamCadDatabase().addVertex(poly,arcCenter.x + radius,arcCenter.y);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for circle entity.\n");
      }

      break;
   case entityRectangle:
      if (m_record.getParamCount() >= entityParameterIndex + 4)
      {
         double xMin = m_record.getInches(entityParameterIndex + 0);
         double yMin = m_record.getInches(entityParameterIndex + 1);
         double xMax = m_record.getInches(entityParameterIndex + 2);
         double yMax = m_record.getInches(entityParameterIndex + 3);

         getCamCadDatabase().addVertex(poly,xMin,yMin);
         getCamCadDatabase().addVertex(poly,xMin,yMax);
         getCamCadDatabase().addVertex(poly,xMax,yMax);
         getCamCadDatabase().addVertex(poly,xMax,yMin);
         getCamCadDatabase().addVertex(poly,xMin,yMin);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for rectangle entity.\n");
      }

      break;
   case entityTrimmedRectangle:
      if (m_record.getParamCount() >= entityParameterIndex + 5)
      {
         double xMin = m_record.getInches(entityParameterIndex + 0);
         double yMin = m_record.getInches(entityParameterIndex + 1);
         double xMax = m_record.getInches(entityParameterIndex + 2);
         double yMax = m_record.getInches(entityParameterIndex + 3);
         double cd   = m_record.getInches(entityParameterIndex + 4);

         getCamCadDatabase().addVertex(poly,xMin     ,yMin + cd);
         getCamCadDatabase().addVertex(poly,xMin     ,yMax - cd);
         getCamCadDatabase().addVertex(poly,xMin + cd,yMax     );
         getCamCadDatabase().addVertex(poly,xMax - cd,yMax     );
         getCamCadDatabase().addVertex(poly,xMax     ,yMax - cd);
         getCamCadDatabase().addVertex(poly,xMax     ,yMin + cd);
         getCamCadDatabase().addVertex(poly,xMax - cd,yMin     );
         getCamCadDatabase().addVertex(poly,xMin + cd,yMin     );
         getCamCadDatabase().addVertex(poly,xMin     ,yMin + cd);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for rct2 entity.\n");
      }

      break;
   case entityRoundedRectangle:
      if (m_record.getParamCount() >= entityParameterIndex + 5)
      {
         double xMin = m_record.getInches(entityParameterIndex + 0);
         double yMin = m_record.getInches(entityParameterIndex + 1);
         double xMax = m_record.getInches(entityParameterIndex + 2);
         double yMax = m_record.getInches(entityParameterIndex + 3);
         double cd   = m_record.getInches(entityParameterIndex + 4);

         getCamCadDatabase().addVertex(poly,xMin     ,yMin + cd);
         getCamCadDatabase().addVertex(poly,xMin     ,yMax - cd,circularBulge);
         getCamCadDatabase().addVertex(poly,xMin + cd,yMax     );
         getCamCadDatabase().addVertex(poly,xMax - cd,yMax     ,circularBulge);
         getCamCadDatabase().addVertex(poly,xMax     ,yMax - cd);
         getCamCadDatabase().addVertex(poly,xMax     ,yMin + cd,circularBulge);
         getCamCadDatabase().addVertex(poly,xMax - cd,yMin     );
         getCamCadDatabase().addVertex(poly,xMin + cd,yMin     ,circularBulge);
         getCamCadDatabase().addVertex(poly,xMin     ,yMin + cd);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for rct3 entity.\n");
      }

      break;
   case entityOval:
      if (m_record.getParamCount() >= entityParameterIndex + 5)
      {
         double x0 = m_record.getInches(entityParameterIndex + 0);
         double y0 = m_record.getInches(entityParameterIndex + 1);
         double x1 = m_record.getInches(entityParameterIndex + 2);
         double y1 = m_record.getInches(entityParameterIndex + 3);
         double r  = m_record.getInches(entityParameterIndex + 4);

         double ang = atan2(y1 - y0,x1 - x0);
         double sinAng = sin(ang);
         double cosAng = cos(ang);

         // sin(ang + Pi/2) =  cos(ang)
         // sin(ang - Pi/2) = -cos(ang)
         // cos(ang + Pi/2) = -sin(ang)
         // cos(ang - Pi/2) =  sin(ang)

         double x,y;

         x = x0 + r*( sinAng);
         y = y0 + r*(-cosAng);
         getCamCadDatabase().addVertex(poly,x,y);

         x = x0 + r*(-sinAng);
         y = y0 + r*( cosAng);
         getCamCadDatabase().addVertex(poly,x,y,circularBulge);

         x = x1 + r*(-sinAng);
         y = y1 + r*( cosAng);
         getCamCadDatabase().addVertex(poly,x,y);

         x = x1 + r*( sinAng);
         y = y1 + r*(-cosAng);
         getCamCadDatabase().addVertex(poly,x,y,circularBulge);

         x = x0 + r*( sinAng);
         y = y0 + r*(-cosAng);
         getCamCadDatabase().addVertex(poly,x,y);
      }
      else
      {
         retval = false;

errorLog.writef(PrefixError,
"Not enough parameters for oval entity.\n");
      }

      break;
   case entityPoly:
      for (int ind=entityParameterIndex;ind + 1 < m_record.getParamCount();ind += 2)
      {
         getCamCadDatabase().addVertex(poly,m_record.getInches(ind),m_record.getInches(ind + 1));
      }

      break;
   }

   return retval;
}

bool CAlcatelGmfDatabase::getLayerNamesL1orLn(CStringArray& layerNames,const CString& layerName,
   LayerCategoryTag layerCategory,CamCadLayerTag topLayerTag,CamCadLayerTag unrelatedLayerTag,CamCadLayerTag bottomLayerTag,
   CWriteFormat& errorLog)
{
   bool retval = true;

   if (layerCategory == lcNone) // unrelated to layers
   {
      layerNames.Add(getCamCadDatabase().getCamCadLayerName(unrelatedLayerTag));
   }
   else
   {
      if (layerCategory == lcOne)
      {
         if (layerName == getTopLayerName())
         {
            layerCategory = lcTop;
         }
         else if (layerName == getBottomLayerName())
         {
            layerCategory = lcBottom;
         }
      }

      switch(layerCategory)
      {
      case lcAll:
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(topLayerTag));
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(bottomLayerTag));
         break;
      case lcTop:
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(topLayerTag));
         break;
      //case lcMounted:
      case lcMountedTop:
      case lcMountedBottomOpposite:
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(topLayerTag) + getCamCadDatabase().getPlacementDependantLayerNameSuffix());
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(topLayerTag) + getCamCadDatabase().getPlacementDependantMirroredLayerNameSuffix());
         break;
      case lcBottom:
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(bottomLayerTag));
         break;
      //case lcMountedOpposite:
      case lcMountedTopOpposite:
      case lcMountedBottom:
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(bottomLayerTag) + getCamCadDatabase().getPlacementDependantLayerNameSuffix());
         layerNames.Add(getCamCadDatabase().getCamCadLayerName(bottomLayerTag) + getCamCadDatabase().getPlacementDependantMirroredLayerNameSuffix());
         break;
      default:
         retval = false;

errorLog.writef(PrefixError,
"Invalid layer specified in '%s'\n",
(const char*)m_record.getRecordString());
         break;
      }
   }

   return retval;
}

bool CAlcatelGmfDatabase::getLayerNamesL1thruLn(CStringArray& layerNames,const CString& layerName,LayerCategoryTag layerCategory,
   CamCadLayerTag layerTag,CWriteFormat& errorLog)
{
   bool retval = true;
   CString placementDependant         = getCamCadDatabase().getPlacementDependantLayerNameSuffix();
   CString placementDependantMirrored = getCamCadDatabase().getPlacementDependantMirroredLayerNameSuffix();
   CString camCadLayerName = getCamCadDatabase().getCamCadLayerName(layerTag);

   if (layerCategory == lcOne)
   {
      layerNames.Add(layerName + " " + camCadLayerName);
   }  
   else if (layerCategory == lcNone) // unrelated to layers
   {
      layerNames.Add(camCadLayerName);
   }
   else
   {
      // top layers
      switch (layerCategory)
      {
      case lcTop:
      case lcOuter:
      case lcAll:
         layerNames.Add(getTopLayerName() + " " + camCadLayerName);
         break;
      //case lcMounted:
      case lcMountedTop:
      case lcMountedBottomOpposite:
         layerNames.Add(getTopLayerName() + " " + camCadLayerName + placementDependant);
         layerNames.Add(getTopLayerName() + " " + camCadLayerName + placementDependantMirrored);
         break;
      }

      // inner layers
      if (layerCategory == lcAll)
      {
         for (int layerIndex = getMinSignalLayerIndex() + 1;layerIndex < getMaxSignalLayerIndex();layerIndex++)
         {
            LayerStruct* layer = getCamCadDatabase().getLayerAt(layerIndex);
            CString layerName = layer->getName();

            layerNames.Add(layerName + " " + camCadLayerName);
         }
      }

      // bottom layers
      switch(layerCategory)
      {
      case lcBottom:
      case lcOuter:
      case lcAll:
         layerNames.Add(getBottomLayerName() + " " + camCadLayerName);
         break;
      //case lcMountedOpposite:
      case lcMountedTopOpposite:
      case lcMountedBottom:
         layerNames.Add(getBottomLayerName() + " " + camCadLayerName + placementDependant);
         layerNames.Add(getBottomLayerName() + " " + camCadLayerName + placementDependantMirrored);
         break;
      }

      if (layerNames.GetCount() < 1)
      {
         retval = false;

errorLog.writef(PrefixError,
"Invalid layer specified in '%s'\n",
(const char*)m_record.getRecordString());
      }
   }

   return retval;
}

bool CAlcatelGmfDatabase::addArea(BlockStruct* parentBlock,const CString& layerName,
   LayerCategoryTag layerCategory,OutlineKindTag outlineKind,
   EntityTypeTag entityType,int entityParameterIndex,CWriteFormat& errorLog,
   double width,bool fillFlag)
{
   bool retval = true;

   GraphicClassTag graphicClass = graphicClassUndefined;
   bool voidFlag = false;
   fillFlag = false;   // for now, always initialize fill flag to false
   CStringArray layerNames;

   // determine which layers to add area
   switch (outlineKind)
   {
   case olBoard:
   case olPanel:
      layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerBoardOutline));

      break;
   case olPartMountingArea:
   case olMountingInhibitArea:
      retval = getLayerNamesL1orLn(layerNames,layerName,layerCategory,ccLayerMountTop,ccLayerMountBottom,ccLayerMount,errorLog);

      break;
   case olResistArea:
      retval = getLayerNamesL1orLn(layerNames,layerName,layerCategory,ccLayerResistTop,ccLayerResistBottom,ccLayerResist,errorLog);

      break;
   case olResistInhibitArea:
      retval = getLayerNamesL1orLn(layerNames,layerName,layerCategory,ccLayerResistInhibitTop,ccLayerResistInhibitBottom,ccLayerResistInhibit,errorLog);

      break;
   case olRoutingArea:
      retval = getLayerNamesL1thruLn(layerNames,layerName,layerCategory,ccLayerRouteKeepInN,errorLog);

      break;
   case olViaInhibitArea:
      retval = getLayerNamesL1thruLn(layerNames,layerName,layerCategory,ccLayerViaKeepOutN,errorLog);

      break;
   case olCopperInhibitArea:
      retval = getLayerNamesL1thruLn(layerNames,layerName,layerCategory,ccLayerCopperInhibitN,errorLog);

      break;
   case olLineInhibitArea:
      retval = getLayerNamesL1thruLn(layerNames,layerName,layerCategory,ccLayerLineInhibitN,errorLog);

      break;
   case olNegativeCopperPowerLayerArea:
      retval = getLayerNamesL1thruLn(layerNames,layerName,layerCategory,ccLayerNegativeCopperN,errorLog);

      break;
   case olNegativeCopperPowerLayerInhibitArea:
      retval = getLayerNamesL1thruLn(layerNames,layerName,layerCategory,ccLayerNegativeCopperInhibitN,errorLog);

      break;
   case olDrawingFigure:
      retval = getLayerNamesL1orLn(layerNames,layerName,layerCategory,ccLayerAssemblyTop,ccLayerAssemblyBottom,ccLayerAssembly,errorLog);
      //layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerAssemblyTop));

      break;
   case olSilkFigure:
      retval = getLayerNamesL1orLn(layerNames,layerName,layerCategory,ccLayerSilkTop,ccLayerSilkBottom,ccLayerSilk,errorLog);
      //layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerSilkTop));

      break;
   }

   if ((entityType == entityLine || entityType == entityArc))
   {
      fillFlag = false;

      if (outlineKind != olBoard         && outlineKind != olPanel && 
          outlineKind != olDrawingFigure && outlineKind != olSilkFigure)
      {
         retval = false;

errorLog.writef(PrefixWarning,
"Bad entity type in '%s'\n",
(const char*)m_record.getRecordString());
      }
   }

   // determine graphic class and width of areas
   if (retval)
   {
      switch (outlineKind)
      {
      case olBoard:                graphicClass = graphicClassBoardOutline;
         width = .024;
         break;
      case olPanel:                graphicClass = graphicClassPanelOutline;
         width = .024;
         break;
      case olPartMountingArea:     graphicClass = graphicClassPlaceKeepIn;                        break;
      case olMountingInhibitArea:  graphicClass = graphicClassPlaceKeepOut;                       break;
      case olResistArea:           graphicClass = graphicClassNormal;                             break;
      case olResistInhibitArea:    graphicClass = graphicClassNormal;           fillFlag = true;  break;
      case olRoutingArea:          graphicClass = graphicClassRouteKeepIn;                        break;
      case olViaInhibitArea:       graphicClass = graphicClassViaKeepOut;                         break;
      case olCopperInhibitArea:    graphicClass = graphicClassRouteKeepOut;                       break;
      case olDrawingFigure:        graphicClass = graphicClassComponentBoundary;                  break;
      case olSilkFigure:           graphicClass = graphicClassComponentOutline;                   break;
      }

      if (graphicClass != graphicClassUndefined)
      {
         int widthIndex = getCamCadDatabase().getDefinedWidthIndex(width);

         for (int ind = 0;ind < layerNames.GetCount();ind++)
         {
            int layerIndex = getCamCadDatabase().getDefinedLayerIndex(layerNames.GetAt(ind));

            DataStruct* polyStruct = getCamCadDatabase().addPolyStruct(parentBlock,layerIndex,0,0,graphicClass);
            CPoly* poly = getCamCadDatabase().addPoly(polyStruct,widthIndex,fillFlag,voidFlag,fillFlag);

            addAreaPoly(poly,entityType,entityParameterIndex,errorLog);
         }
      }
   }

   return retval;
}

void CAlcatelGmfDatabase::rectifyOuterLayers()
{
   if (m_topLayerIndex >= 0 && m_bottomLayerIndex >= 0)
   {
      m_minSignalLayerIndex = min(m_topLayerIndex,m_bottomLayerIndex);
      m_maxSignalLayerIndex = max(m_topLayerIndex,m_bottomLayerIndex);

      Graph_Level_Mirror(m_topLayerName,m_bottomLayerName,NULL);
   }
}

void CAlcatelGmfDatabase::setTopLayerIndex(int topLayerIndex)
{
   m_topLayerIndex = topLayerIndex;

   rectifyOuterLayers();
}

void CAlcatelGmfDatabase::setBottomLayerIndex(int bottomLayerIndex)
{
   m_bottomLayerIndex = bottomLayerIndex;

   rectifyOuterLayers();
}

int CAlcatelGmfDatabase::getMinSignalLayerIndex()
{
   ASSERT(m_minSignalLayerIndex >= 0);

   return m_minSignalLayerIndex;
}

int CAlcatelGmfDatabase::getMaxSignalLayerIndex()
{
   ASSERT(m_maxSignalLayerIndex >= 0);

   return m_maxSignalLayerIndex;
}

int CAlcatelGmfDatabase::getCamCadLayerNames(CStringArray& layerNames,int layerRelatedType,
   const CString& objectCodeString,bool thruHoleFlag)
{
   // object codes may be: "AW1", "AW2", "SR1", "SR2", "CLR", "FLSH"
   // layer related types may be: 1, 2, 3, 5, 6
   layerNames.RemoveAll();
   CString layerName;
   bool mirrorApertureFlag = false;

   enum { aw1, aw2, sr1, sr2, clr, flsh, undefined } objectCode;

   if      (objectCodeString == "AW1")  objectCode = aw1;
   else if (objectCodeString == "AW2")  objectCode = aw2;
   else if (objectCodeString == "SR1")  objectCode = sr1;
   else if (objectCodeString == "SR2")  objectCode = sr2;
   else if (objectCodeString == "CLR")  objectCode = clr;
   else if (objectCodeString == "FLSH") objectCode = flsh;
   else                                 objectCode = undefined;

   // flow soldering layer is assumed to mean "bottom"
   // reflow soldering layer is assumed to mean "top"

   if (objectCode == flsh)
   {
      layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadFlash));
   }
   else if (objectCode == clr)
   {
      layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadClearance));
   }
   else
   {
      switch (layerRelatedType)
      {
      case 1:  // th:  copper pads on all layers are the same [aw1].  Mask pad for top is [sr2], bottom [sr1] .
               // smd: copper pad is [aw1] on the mounted layer.  Mask pad is [sr2] if mounted on top, [sr1] if mounted on bottom.
         switch (objectCode)
         {
         case aw1:
            if (thruHoleFlag)
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadAll));
            }
            else
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadTop));
            }

            break;
         case sr2:  
            if (thruHoleFlag)
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPd));
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPdM));
            }
            else
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPd));
            }

            break;
         case sr1:  
            if (thruHoleFlag)
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPd));
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPdM));
            }
            else
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPdM));
            }

            break;
         }

         break;
      case 2:  // th:  copper pads on outer layers are [aw1], inner [aw2].
               // smd: copper pads on outer layers are [aw1] (both pads are present)
               // either: Mask pad for top is [sr2], bottom [sr1] .
         switch (objectCode)
         {
         case aw1:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadTop));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadBottom));

            break;
         case aw2:  
            if (thruHoleFlag)
            {
               layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadInner)); 
            }
            
            break;
         case sr2:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPd));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPdM));

            break;
         case sr1:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPd));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPdM));

            break;
         }

         break;
      case 3:  // smd: copper pad is [aw2] if mounted on top, [aw1] if mounted on bottom.
               //      mask pad is [sr2] if mounted on top, [sr1] if mounted on bottom.
         switch (objectCode)
         {
         case aw2:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadTopPd));

            break;
         case aw1:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadBottomPdM));

            break;
         case sr2:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPd));

            break;
         case sr1:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPdM));

            break;
         }

         break;
      case 5:  // th:  copper pad for top is [aw2], bottom [aw1] .  Mask pad for top is [sr2], bottom [sr1] .
         switch (objectCode)
         {
         case aw2:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadTopPd));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadTopPdM));

            break;
         case aw1:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadBottomPd));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerPadBottomPdM));

            break;
         case sr2:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPd));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskTopPdM));

            break;
         case sr1:  
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPd));
            layerNames.Add(getCamCadDatabase().getCamCadLayerName(ccLayerMaskBottomPdM));

            break;
         }

         break;
      case 6:
         break;
      }
   }

   return (int)layerNames.GetCount();
}

bool CAlcatelGmfDatabase::instantiateFlattenedVia(const CString& viaName,const CString& padStackName,
                                                  const CString& layer1name,const CString& layer2name,bool topFlag)
{
   bool retval = true;

   BlockStruct* viaBlock = Graph_Block_Exists(&(getCamCadDatabase().getCamCadDoc()),viaName,getBoardFileNum());

   if (viaBlock == NULL)
   {
      viaBlock = getCamCadDatabase().getDefinedBlock(viaName,blockTypePadstack,getBoardFileNum());
      BlockStruct* ltpBlock = getCamCadDatabase().getDefinedBlock(padStackName,blockTypePadstack,getBoardFileNum());

      int layer1Index = getCamCadDatabase().getDefinedLayerIndex(layer1name);
      int layer2Index = getCamCadDatabase().getDefinedLayerIndex(layer2name);

      int minLayerIndex      = min(layer1Index,layer2Index);
      int maxLayerIndex      = max(layer1Index,layer2Index);
      int minInnerLayerIndex = ((minLayerIndex == getMinSignalLayerIndex()) ? minLayerIndex + 1 : minLayerIndex);
      int maxInnerLayerIndex = ((maxLayerIndex == getMaxSignalLayerIndex()) ? maxLayerIndex - 1 : maxLayerIndex);

      bool normalFlag             = (minLayerIndex == getMinSignalLayerIndex() && maxLayerIndex == getMaxSignalLayerIndex());
      bool topSightedBlindFlag    = (!normalFlag && (minLayerIndex == m_topLayerIndex    || maxLayerIndex == m_topLayerIndex));
      bool bottomSightedBlindFlag = (!normalFlag && (minLayerIndex == m_bottomLayerIndex || maxLayerIndex == m_bottomLayerIndex));
      bool buriedFlag             = (!normalFlag && !topSightedBlindFlag && !bottomSightedBlindFlag);

      DataStruct *data,*newData;

      for (POSITION pos = ltpBlock->getDataList().GetHeadPosition();pos != NULL;)
      {
         data = ltpBlock->getDataList().GetNext(pos);
         bool copyFlag = true;
         int destinationLayerIndex = data->getLayerIndex();

         if (data->getDataType() == T_INSERT && !normalFlag)
         {
            copyFlag = false;

            if (data->getLayerIndex() >= getMinSignalLayerIndex() && data->getLayerIndex() <= getMaxSignalLayerIndex())
            {  // source is on an electrical layer
               if (data->getLayerIndex() >= minLayerIndex && data->getLayerIndex() <= maxLayerIndex)
               {
                  copyFlag = true;

                  if (!topFlag)
                  {
                     // flip the layer
                     destinationLayerIndex = (minLayerIndex + maxLayerIndex) - data->getLayerIndex();
                  }
               }
            }
            else  // source is not on an electrical layer
            {
               CamCadLayerTag destinationLayerTag = getCamCadDatabase().getCamCadLayerTag(data->getLayerIndex());

               if (!topFlag)
               {
                  destinationLayerTag = getCamCadDatabase().getOppositeCamCadLayerTag(destinationLayerTag);
               }

               bool padEntityFlag = true;

               switch (destinationLayerTag)
               {
               case ccLayerPadTop:       
               case ccLayerPadTopPd:     
               case ccLayerPadTopPdM:  
                  destinationLayerTag = (topSightedBlindFlag ? ccLayerPadTop : ccLayerUndefined);
                  break;
               case ccLayerMaskTop:      
               case ccLayerMaskTopPd:    
               case ccLayerMaskTopPdM:   
                  destinationLayerTag = (topSightedBlindFlag ? ccLayerMaskTop : ccLayerUndefined);;
                  break;
               case ccLayerPadBottom:    
               case ccLayerPadBottomPd:  
               case ccLayerPadBottomPdM: 
                  destinationLayerTag = (bottomSightedBlindFlag ? ccLayerPadBottom : ccLayerUndefined);
                  break;
               case ccLayerMaskBottom:   
               case ccLayerMaskBottomPd: 
               case ccLayerMaskBottomPdM:
                  destinationLayerTag = (bottomSightedBlindFlag ? ccLayerMaskBottom : ccLayerUndefined);
                  break;
               case ccLayerPadInner:
               case ccLayerPadAll:
                  break;
               default:
                  padEntityFlag = false;
                  break;
               }

               copyFlag = !padEntityFlag;

               if (padEntityFlag)
               {
                  int startLayerIndex = 0;
                  int endLayerIndex   = startLayerIndex - 1;

                  switch (destinationLayerTag)
                  {
                  case ccLayerPadTop: 
                     startLayerIndex = m_topLayerIndex;
                     endLayerIndex   = startLayerIndex;
                     break;
                  case ccLayerPadBottom: 
                     startLayerIndex = m_bottomLayerIndex;
                     endLayerIndex   = startLayerIndex;
                     break;
                  case ccLayerMaskTop: 
                  case ccLayerMaskBottom: 
                     startLayerIndex = getCamCadDatabase().getCamCadLayerIndex(destinationLayerTag);
                     endLayerIndex   = startLayerIndex;
                     break;
                  case ccLayerPadInner: 
                     startLayerIndex = minInnerLayerIndex;
                     endLayerIndex   = maxInnerLayerIndex;
                     break;
                  case ccLayerPadAll: 
                     startLayerIndex = minLayerIndex;
                     endLayerIndex   = maxLayerIndex;
                     break;
                  }

                  for (destinationLayerIndex = startLayerIndex;destinationLayerIndex <= endLayerIndex;destinationLayerIndex++)
                  {
                     newData = getCamCadData().getNewDataStruct(*data);
                     newData->setLayerIndex(destinationLayerIndex);
                     viaBlock->getDataList().AddTail(newData);
                  }
               }
            }
         }

         if (copyFlag)
         {  
            newData = getCamCadData().getNewDataStruct(*data);
            newData->setLayerIndex(destinationLayerIndex);
            viaBlock->getDataList().AddTail(newData);
         }
      }
   }

   return retval;
}

void CAlcatelGmfDatabase::processTecRecord(CWriteFormat& errorLog)
{
   if (m_record.isGood(sectTec,alcatelKwLayno,errorLog))
   {
      m_numberOfElectricalLayers = m_record.getInt(1);
   }
   else if (m_record.isGood(sectTec,alcatelKwLayer,errorLog))
   {
      CString layerName  = m_record.getParam(1);
      CString layerSide  = m_record.getParam(2);
      CString layerPower = m_record.getParam(3);

      bool topFlag    = (layerSide  == "TOP"   );
      bool bottomFlag = (layerSide  == "BOTTOM");
      bool powerFlag  = (layerPower == "POWER" || layerPower == "GROUND");

      int layerType = LAYTYPE_SIGNAL_INNER;

      if      (topFlag   ) layerType = LAYTYPE_SIGNAL_TOP;
      else if (bottomFlag) layerType = LAYTYPE_SIGNAL_BOT;
      else if (powerFlag ) layerType = LAYTYPE_POWERPOS;

      int layerIndex = getCamCadDatabase().getDefinedLayerIndex(layerName,false,layerType);

      if (topFlag)
      {
         m_topLayerName = layerName;
         setTopLayerIndex(layerIndex);
      }
      else if (bottomFlag)
      {
         m_bottomLayerName = layerName;
         setBottomLayerIndex(layerIndex);
      }

      if (powerFlag)
      {
         LayerStruct* layer = getCamCadDatabase().getDefinedLayer(layerName);
         getCamCadDatabase().addAttribute(getAttributeMap(layer),getAttributeIndex(attributeIndexLayerPowerPos),
            errorLog);
      }

      m_signalLayerCount++;

      if (m_signalLayerCount >= m_numberOfElectricalLayers)
      {
         initializeCamCadLayers();
      }

      if (m_signalLayerCount > m_numberOfElectricalLayers)
      {
errorLog.writef(PrefixError,
"Electrical layer number %d with name '%s', exceeds the number of stated electrical layers, %d\n",
m_signalLayerCount,(const char*)layerName,m_numberOfElectricalLayers);
      }
   }  
   else if (m_record.isGood(sectTec,alcatelKwLayout,errorLog))
   {
      int boardSequenceNumber = m_record.getInt(1);
      CPoint2d position       = m_record.getCoordInInches(2);
      double rotation         = m_record.getDegrees(3);

      CString stepname;
      stepname.Format("BOARD_%d",boardSequenceNumber);
      int layerIndex = NoLayer;

      DataStruct* data = getCamCadDatabase().referenceBlock(getPanelBlock(),getBoardBlock(),INSERTTYPE_PCB,
         stepname,layerIndex,position.x,position.y,rotation);
   }
   else if (m_record.isGood(sectTec,alcatelKwArea,errorLog))
   {
      CString areaType    = m_record.getParam(1);
      CString outlineKind = m_record.getParam(2);
      CString entityType  = m_record.getParam(3);
      CString layerName   = m_record.getParam(4);

      OutlineKindTag outlineKindTag = stringToOutlineKindTag(outlineKind);

      bool panelFlag = (areaType == "PANEL");

      if (panelFlag && outlineKindTag == olBoard)
      {
         outlineKindTag = olPanel;
      }

      LayerCategoryTag layerCategory = lcUndefined;

      if (layerName == getTopLayerName())
      {
         layerCategory = lcTop;
      }
      else if (layerName == getBottomLayerName())
      {
         layerCategory = lcBottom;
      }

      switch(outlineKindTag)
      {
      case olBoard:
      case olPanel:
         layerCategory = lcOne;
         break;
      case olPartMountingArea:
      case olMountingInhibitArea:
      case olResistArea:
      case olResistInhibitArea:
         layerCategory = ((layerName == "") ? lcOuter : lcOne);

         break;
      case olRoutingArea:
      case olNegativeCopperPowerLayerArea:
      case olLineInhibitArea:
      case olViaInhibitArea:
      case olCopperInhibitArea:
      case olNegativeCopperPowerLayerInhibitArea:
         layerCategory = ((layerName == "") ? lcAll : lcOne);

         break;
      }

      if (layerCategory != lcUndefined)
      {
         BlockStruct* pcbBlock = (panelFlag ? getPanelBlock() : getBoardBlock());
         addArea(pcbBlock,layerName,layerCategory,outlineKindTag,stringToEntityTypeTag(entityType),5,errorLog);
      }
      else
      {
errorLog.writef(PrefixError,
"Invalid layer name '%s' used\n",
(const char*)layerName);
      }
   }
   else if (m_record.isGood(sectTec,alcatelKwPosmk,errorLog))
   {  // fiducial mark
      CString positionKind  = m_record.getParam(1);
      CString markType      = m_record.getParam(2);
      CPoint2d position     = m_record.getCoordInInches(3);
      CString landTypeName  = m_record.getParam(4);
      CString side          = m_record.getParam(5);
      double angleInRadians = m_record.getRadians(6);
      CString useCode       = m_record.getParam(7);

      bool panelFlag = (positionKind == "PANEL");
      bool topFlag   = (side         == "TOP");

      FileStruct*  pcbFile  = (panelFlag ? getPanelFile()    : getBoardFile() );
      BlockStruct* pcbBlock = (panelFlag ? getPanelBlock()   : getBoardBlock());
      CString layerName     = (topFlag   ? getTopLayerName() : getBottomLayerName());

      //int layerIndex = getCamCadDatabase().getDefinedLayerIndex(layerName);
      int layerIndex = NoLayer;

      BlockStruct* ltpBlock = getCamCadDatabase().getDefinedBlock(genPadStackName(landTypeName),blockTypePadstack,getBoardFileNum());

      DataStruct* fiducialData = getCamCadDatabase().referenceBlock(pcbBlock,ltpBlock,INSERTTYPE_FIDUCIAL,NULL,layerIndex,
         position.x,position.y,angleInRadians,!topFlag);

      if (markType == "SMD")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(fiducialData),getAttributeIndex(attributeIndexSmdShape),
            errorLog);
      }

      if (useCode != "")
      {
errorLog.writef(PrefixWarning,
"use code of '%s' ignored for POSMK record, '%s'\n",
(const char*)useCode,(const char*)m_record.getRecordString());
      }
   }
   else if (m_record.isGood(sectTec,alcatelKwLand,errorLog))
   {  // free pad
      CString positionKind  = m_record.getParam(1);
      CPoint2d position     = m_record.getCoordInInches(2);
      CString landTypeName  = m_record.getParam(3);
      CString side          = m_record.getParam(4);
      double angleInRadians = m_record.getRadians(5);
      CString useCode       = m_record.getParam(6);

      bool panelFlag = (positionKind == "PANEL");
      bool topFlag   = (side         == "TOP");
      int layerIndex = NoLayer;

      BlockStruct* pcbBlock = (panelFlag ? getPanelBlock() : getBoardBlock());
      BlockStruct* ltpBlock = getCamCadDatabase().getDefinedBlock(genPadStackName(landTypeName),blockTypePadstack,getBoardFileNum());

      DataStruct* fiducialData = getCamCadDatabase().referenceBlock(pcbBlock,ltpBlock,INSERTTYPE_FREEPAD,NULL,layerIndex,
         position.x,position.y,angleInRadians,!topFlag);

      if (useCode != "")
      {
errorLog.writef(PrefixWarning,
"use code of '%s' ignored for LAND record, '%s'\n",
(const char*)useCode,(const char*)m_record.getRecordString());
      }
   }
   else if (m_record.isGood(sectTec,alcatelKwEnd,errorLog))
   {
      initializeElectricalLayers(errorLog);
   }
}

// Component Assembly data (Components, Device Instance)
void CAlcatelGmfDatabase::processAsmRecord(CWriteFormat& errorLog)
{
   static int deviceCnt = 0;
   int maxDeviceCnt = -1;

   if (m_record.isGood(sectAsm,alcatelKwCompnt,errorLog))
   {
      if (++deviceCnt > maxDeviceCnt && maxDeviceCnt > 0)
      {
         return;
      }

      getCamCadDatabase().getCamCadDoc().PrepareAddEntity(getBoardFile());

      // make sure plb is instantiated
      CString plbName = m_record.getParam(13);

      BlockStruct* plbBlock = getCamCadDatabase().getDefinedBlock(plbName,blockTypePcbComponent,getBoardFileNum());

      // add a component to the board
      CString refdes         = m_record.getParam(1);
      CString physicalRefdes = m_record.getParam(2);
      CString side           = m_record.getParam(3);
      CPoint2d position      = m_record.getCoordInInches(4);
      double angleInRadians  = m_record.getRadians(5);
      CString partNumber     = m_record.getParam(7);
      double mountingHeight  = m_record.getInches(12);
      CString useCode        = m_record.getParam(14);

      bool topSide   = (side == "TOP");
      int layerIndex = NoLayer;

      DataStruct* componentData = getCamCadDatabase().referenceBlock(getBoardBlock(),plbBlock,INSERTTYPE_PCBCOMPONENT,refdes,layerIndex,
         position.x,position.y,(topSide ? angleInRadians : -angleInRadians),!topSide);

      getCamCadDatabase().addAttribute(getAttributeMap(componentData),
         getAttributeIndex(attributeIndexPhysicalRefDes),physicalRefdes,errorLog);
      getCamCadDatabase().addAttribute(getAttributeMap(componentData),
         getAttributeIndex(attributeIndexTypeListLink),partNumber,errorLog);
      getCamCadDatabase().addAttribute(getAttributeMap(componentData),
         getAttributeIndex(attributeIndexCompHeight),(float)mountingHeight,errorLog);
      
      getCamCadDatabase().referenceDevice(partNumber,plbBlock,getBoardFile());
      m_compArchitypes.addArchitype(plbName,partNumber,refdes);

      if (useCode != "")
      {
errorLog.writef(PrefixWarning,
"use code of '%s' ignored for refdes='%s', partNumber='%s'\n",
(const char*)useCode,(const char*)refdes,(const char*)partNumber);
      }
   }  
}

void CAlcatelGmfDatabase::processPpnRecord(CWriteFormat& errorLog)
{
   if (m_record.isGood(sectPpn,alcatelKwPrtdef,errorLog))
   {
      m_ppnRefdes = m_record.getParam(1);
   }
   else if (m_record.isGood(sectPpn,alcatelKwPin,errorLog))
   {
      int sequenceNumber = m_record.getInt(1);
      CString pinName    = m_record.getParam(2);
      CString polarity   = m_record.getParam(3);

      m_compArchitypes.addPin(m_ppnRefdes,sequenceNumber,pinName,polarity);
   }
   else if (m_record.isGood(sectPpn,alcatelKwPrtend,errorLog))
   {
      m_compArchitypes.removeRedundancies(m_ppnRefdes);
   }
}

void CAlcatelGmfDatabase::processNetRecord(CWriteFormat& errorLog)
{
   if (m_record.isGood(sectNet,alcatelKwNetlst,errorLog))
   {
      CString netName   = m_record.getParam(1);
      CString powerKind = m_record.getParam(3);

      NetStruct* net = getCamCadDatabase().getDefinedNet(netName,getBoardFile());

      if (powerKind == "POWER" || powerKind == "GROUND")
      {
         getCamCadDatabase().addAttribute(getAttributeMap(net),getAttributeIndex(attributeIndexPowerNet),
            errorLog);
      }

      CSupString refDesPinNumber;
      CStringArray params;
      CString refDes,pinNumber;

      for (int ind = 4;ind < m_record.getParamCount();ind++)
      {
         refDesPinNumber = m_record.getParam(ind);
         refDesPinNumber.Parse(params,".");

         ASSERT(params.GetCount() == 2);

         refDes    = params[0];
         pinNumber = params[1];

         CompPinStruct* pin = getCamCadDatabase().addCompPin(net,refDes,pinNumber);
      }
   }
}

void CAlcatelGmfDatabase::processEcdRecord(CWriteFormat& errorLog)
{
}

// Land Type data (Padstacks)
void CAlcatelGmfDatabase::processLtpRecord(CWriteFormat& errorLog)
{
   if (m_record.isGood(sectLtp,alcatelKwTypdef,errorLog))
   {
      ASSERT(m_ltpBlock == NULL);

      getCamCadDatabase().getCamCadDoc().PrepareAddEntity(getBoardFile());

      // add an ltb to the board
      CString ltpName    = m_record.getParam(1);
      CString useCode    = m_record.getParam(2);
      int useFlag        = m_record.getInt(3);
      m_layerRelatedType = m_record.getInt(4);
      m_thruHoleFlag     = (m_record.getInt(5) == 1);
      CString plated     = m_record.getParam(6);

      CString padStackName = genPadStackName(ltpName);

      m_ltpBlock = getCamCadDatabase().getDefinedBlock(padStackName,blockTypePadstack,getBoardFileNum());

      if (m_ltpBlock->getBlockType() != BLOCKTYPE_PADSTACK)
      {
errorLog.writef(PrefixWarning,
"Changing block type to BLOCKTYPE_PADSTACK for padStackName='%s', old block type = %d\n",
(const char*)padStackName,m_ltpBlock->getBlockType());

         m_ltpBlock->setBlockType(BLOCKTYPE_PADSTACK);
      }

      if (m_thruHoleFlag)
      {
         double unfinishedHoleSize = m_record.getInches(8);
         int drillLayerIndex    = getCamCadDatabase().getCamCadLayerIndex(ccLayerDrillHoles);
         BlockStruct* toolBlock = getCamCadDatabase().getDefinedTool(unfinishedHoleSize);

         DataStruct* toolReference = getCamCadDatabase().referenceBlock(m_ltpBlock,toolBlock,INSERTTYPE_UNKNOWN,NULL,drillLayerIndex);

         if (plated == "PTH")
         {
            getCamCadDatabase().addAttribute(getAttributeMap(m_ltpBlock),getAttributeIndex(attributeIndexPlated),
               errorLog);
         }
      }
      else
      {
         getCamCadDatabase().addAttribute(getAttributeMap(m_ltpBlock),getAttributeIndex(attributeIndexSmdShape),
            errorLog);
      }

      if (useFlag == 0)
      {
errorLog.writef(PrefixWarning,
"useflag of 0 is not implemented.  ltpName='%s'\n",
(const char*)ltpName);
      }

      if (useCode != "")
      {
errorLog.writef(PrefixWarning,
"use code of '%s' ignored for TYPEDEF record, '%s'\n",
(const char*)useCode,(const char*)m_record.getRecordString());
      }
   }
   else if (m_record.isGood(sectLtp,alcatelKwLndfig,errorLog))
   {
      ASSERT (m_ltpBlock != NULL);

      // add a pad to the ltp
      CString objectCode    = m_record.getParam(1);
      CString entityType    = m_record.getParam(2);

      CStringArray layerNames;
      getCamCadLayerNames(layerNames,m_layerRelatedType,objectCode,m_thruHoleFlag);

      for (int layerNameIndex = 0;layerNameIndex < layerNames.GetCount();layerNameIndex++)
      {
         CString layerName     = layerNames.GetAt(layerNameIndex);

         if (layerName != "")
         {
            int layerIndex        = getCamCadDatabase().getDefinedLayerIndex(layerName);

            ApertureShapeTag apertureShape = apertureUnknown;
            double sizeA,sizeB=0.,xOffset=0.,yOffset=0.,rotation=0.;
            CString apertureName;

            if (entityType == "RECT")
            {
               int xMin = m_record.getInt(3);
               int yMin = m_record.getInt(4);
               int xMax = m_record.getInt(5);
               int yMax = m_record.getInt(6);

               int dx = xMax - xMin;
               int dy = yMax - yMin;
               
               if (dx == dy)
               {
                  apertureShape = apertureSquare;
                  apertureName.Format("SQ%d",dx);
               }
               else
               {
                  apertureShape = apertureRectangle;
                  apertureName.Format("RECT%dX%d",dx,dy);
               }

               sizeA   = toInches(dx);
               sizeB   = toInches(dy);
               xOffset = toInches((xMin + xMax)/2);
               yOffset = toInches((yMin + yMax)/2);
            }
            else if (entityType == "CIR")
            {
               int xCenter = m_record.getInt(3);
               int yCenter = m_record.getInt(4);
               int radius  = m_record.getInt(5);

               apertureShape = apertureRound;
               apertureName.Format("RND%d",radius*2);

               sizeA   = toInches(2 * radius);
               xOffset = toInches(xCenter);
               yOffset = toInches(yCenter);

            }
            else if (entityType == "POLY")
            {
               BlockStruct* padBlock = getCamCadDatabase().getDefinedBlock(CString("APTPOLY_") + m_ltpBlock->getName(),blockTypePadshape,getBoardFileNum());

               int widthIndex = getCamCadDatabase().getDefinedWidthIndex(0.);

               DataStruct* polyStruct = getCamCadDatabase().addPolyStruct(padBlock,layerIndex,0,0, graphicClassNormal);
               CPoly* poly = getCamCadDatabase().addFilledPoly(polyStruct,widthIndex);

               double x,y,bulge=0.;

               for (int ind = 3;ind < m_record.getParamCount();ind += 2)
               {
                  x = m_record.getInches(ind + 0);
                  y = m_record.getInches(ind + 1);

                  getCamCadDatabase().addVertex(poly,x,y,bulge);
               }
            }
            else if (entityType == "OVAL")
            {
               double ovalWidth = 2. * m_record.getInches(7);

               CString apertureName = "APTOVAL_" + m_record.getParam(3) + "_" + m_record.getParam(4) + "_" + 
                                                   m_record.getParam(5) + "_" + m_record.getParam(6) + "_" + m_record.getParam(7);

               BlockStruct* padBlock = getCamCadDatabase().getDefinedBlock(apertureName,blockTypePadshape,getBoardFileNum());

               int widthIndex = getCamCadDatabase().getDefinedWidthIndex(ovalWidth);

               DataStruct* polyStruct = getCamCadDatabase().addPolyStruct(padBlock,layerIndex,0,0, graphicClassNormal);
               CPoly* poly = getCamCadDatabase().addOpenPoly(polyStruct,widthIndex);

               double x,y,bulge=0.;

               x = m_record.getInches(3);
               y = m_record.getInches(4);
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = m_record.getInches(5);
               y = m_record.getInches(6);
               getCamCadDatabase().addVertex(poly,x,y,bulge);
            }
            else if (entityType == "RCT2")
            {
               double xMin            = m_record.getInches(3);
               double yMin            = m_record.getInches(4);
               double xMax            = m_record.getInches(5);
               double yMax            = m_record.getInches(6);
               double cornerDimension = m_record.getInches(7);

               CString apertureName = "APTRCT2_" + m_record.getParam(3) + "_" + m_record.getParam(4) + "_" + 
                                                   m_record.getParam(5) + "_" + m_record.getParam(6) + "_" + m_record.getParam(7);

               BlockStruct* padBlock = getCamCadDatabase().getDefinedBlock(apertureName,blockTypePadshape,getBoardFileNum());

               int widthIndex = getCamCadDatabase().getDefinedWidthIndex(0.);

               DataStruct* polyStruct = getCamCadDatabase().addPolyStruct(padBlock,layerIndex,0,0, graphicClassNormal);
               CPoly* poly = getCamCadDatabase().addOpenPoly(polyStruct,widthIndex);

               double x,y,bulge=0.;

               x = xMin;
               y = yMin + cornerDimension;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMin;
               y = yMax - cornerDimension;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMin + cornerDimension;
               y = yMax;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMax - cornerDimension;
               y = yMax;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMax;
               y = yMax - cornerDimension;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMax;
               y = yMin + cornerDimension;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMax - cornerDimension;
               y = yMin;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMin + cornerDimension;
               y = yMin;
               getCamCadDatabase().addVertex(poly,x,y,bulge);

               x = xMin;
               y = yMin + cornerDimension;
               getCamCadDatabase().addVertex(poly,x,y,bulge);
            }
            //else if (entityType == "RCT5")
            //{
            // double outsideHeight = m_record.getInches(3);
            // double insideHeight  = m_record.getInches(4);
            // double spokeWidth    = m_record.getInches(5);
            // double rotation      = m_record.getDegrees(6);
            // double outsideCorner = m_record.getInches(7);
            // double insideCorner  = m_record.getInches(8);

            // CString apertureName = "APTRCT5_" + m_record.getParam(3) + "_" + m_record.getParam(4) + "_" + 
            //                                     m_record.getParam(5) + "_" + m_record.getParam(6) + "_" +
            //                                     m_record.getParam(7) + "_" + m_record.getParam(8);

            // //BlockStruct* boardBlock = Graph_Block_On(GBO_APPEND,CString("APTRCT2_") + m_ltpBlock->name,getBoardFileNum(),0);
            // BlockStruct* padBlock = getCamCadDatabase().getDefinedBlock(apertureName,BLOCKTYPE_PADSHAPE,getBoardFileNum());

            // int widthIndex = Graph_Aperture("",T_ROUND,0,0.,0.,0.,0.,0,BL_WIDTH,0,&err);

            // DataStruct* polyStruct = getCamCadDatabase().addPolyStruct(padBlock,layerIndex,0,0, graphicClassNormal);
            // CPoly* poly = getCamCadDatabase().addPoly(polyStruct,widthIndex,false,false,false);

            // double x,y,bulge=0.;

            // x = xMin;
            // y = yMin + cornerDimension;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMin;
            // y = yMax - cornerDimension;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMin + cornerDimension;
            // y = yMax;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMax - cornerDimension;
            // y = yMax;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMax;
            // y = yMax - cornerDimension;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMax;
            // y = yMin + cornerDimension;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMax - cornerDimension;
            // y = yMin;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMin + cornerDimension;
            // y = yMin;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // x = xMin;
            // y = yMin + cornerDimension;
            // getCamCadDatabase().addVertex(poly,x,y,bulge);

            // //Graph_Block_Off();
            //}
            else
            {
errorLog.writef(PrefixError,"Unimplemented entityType of '%s' in Ltp section.\n",(const char*)entityType);
            }

            if (apertureShape != apertureUnknown)
            {
               int err;
               int dcode=0;
               DbFlag flag=0;
               bool overwriteValues = false;

               int apertureIndex = Graph_Aperture(apertureName,apertureShape,sizeA,sizeB,xOffset,yOffset,rotation,dcode,flag,overwriteValues,&err);

               DataStruct* data = getCamCadDatabase().referenceBlock(m_ltpBlock,apertureName,
                                     INSERTTYPE_UNKNOWN,NULL,layerIndex,getBoardFileNum());
            }

            //if (apertureShape != apertureUnknown)
            //{
            // BlockStruct* aperture = getCamCadDatabase().getDefinedAperture(apertureName,apertureShape,
            //                            sizeA,sizeB,xOffset,yOffset,rotation);

            // //getCamCadDatabase().referenceBlock(m_ltpBlock,aperture,INSERTTYPE_UNKNOWN,NULL,layerIndex,getBoardFileNum());
            // int err;
            // int dcode=0;
            // DbFlag flag=0;
            // bool overwriteValues = false;

            // int apertureIndex = Graph_Aperture(apertureName,apertureShape,sizeA,sizeB,xOffset,yOffset,rotation,dcode,flag,overwriteValues,&err);

            // DataStruct* data = getCamCadDatabase().referenceBlock(m_ltpBlock,apertureName,
            //                       INSERTTYPE_UNKNOWN,NULL,layerIndex,getBoardFileNum());

            // // -------- check
            // //BlockStruct* aperture2 = getCamCadDatabase().getDefinedAperture(apertureName,apertureShape,
            // //                         sizeA,sizeB,xOffset,yOffset,rotation);
            // int apertureIndex2 = getCamCadDatabase().getDefinedApertureIndex(apertureName,apertureShape,
            //                            sizeA,sizeB,xOffset,yOffset,rotation);

            // if (apertureIndex2 != apertureIndex)
            // {
            //    AfxDebugBreak();
            // }

            // BlockStruct* aperture3 = getCamCadDatabase().getCamCadDoc().widthTable[apertureIndex];

            // if (aperture3 != aperture)
            // {
            //    AfxDebugBreak();
            // }

            // if (data->getInsert()->getBlockNumber() != aperture->num)
            // {
            //    AfxDebugBreak();
            // }
            //}
         }
      }
   }
   else if (m_record.isGood(sectLtp,alcatelKwTypend,errorLog))
   {
      if (m_ltpBlock != NULL)
      {
         m_ltpBlock = NULL;
      }
   }
}

// PLB data (Footprint, Decal)
void CAlcatelGmfDatabase::processPlbRecord(CWriteFormat& errorLog)
{
   bool silkFlag = false;

   if (m_record.isGood(sectPlb,alcatelKwPtndef,errorLog))
   {
      ASSERT(m_plbBlock == NULL);

      getCamCadDatabase().getCamCadDoc().PrepareAddEntity(getBoardFile());

      // add a plb to the board
      CString plbName = m_record.getParam(1);
      m_compArchitypes.getPlbNames(m_plbNames,plbName);

      m_plbBlock = getCamCadDatabase().getDefinedBlock(plbName,blockTypePcbComponent,getBoardFileNum());
   }
   else if (m_record.isGood(sectPlb,alcatelKwPin,errorLog))
   {
      ASSERT (m_plbBlock != NULL);

      // add a pin to the plb
      CString pinSequenceString = m_record.getParam(1);
      int pinSequence           = m_record.getInt(1);
      CPoint2d position         = m_record.getCoordInInches(2);
      CString pinKind           = m_record.getParam(3);
      CString landTypeName      = m_record.getParam(4);
      double angleInRadians     = m_record.getRadians(5);
      bool mirror  = false; 
      double scale = 1.;
      int layerIndex = -1;
      bool global  = true;

      CString plbName = m_plbBlock->getName();
      int insertType = INSERTTYPE_PIN;

      if (pinKind == "ATCHOL")
      {
         insertType = INSERTTYPE_MECHANICALPIN;
      }
      else if (pinKind == "POSMK")
      {
         insertType = INSERTTYPE_FIDUCIAL;
      }

      CCompArchitype* architype = NULL;

      for (int ind=0;ind < m_plbNames.GetCount();ind++)
      {
         CString plbName = m_plbNames.GetAt(ind);

         BlockStruct* plbBlock = getCamCadDatabase().getDefinedBlock(plbName,blockTypePcbComponent,getBoardFileNum());

         if (! m_compArchitypes.getArchitype(plbName,architype))
         {
errorLog.writef(PrefixError,
"Could not find component architype for '%s'\n",
(const char*)plbName);

            continue;
         }

         if (pinSequenceString.GetLength() == 0)
         {
            DataStruct* pinData = getCamCadDatabase().referenceBlock(plbBlock,genPadStackName(landTypeName),insertType,"",
               layerIndex,getBoardFileNum(),position.x,position.y,angleInRadians,mirror,scale,global);
         }
         else
         {
            CCompArchitypePin* pin = architype->getPinAt(pinSequence);

            if (pin == NULL)
            {
errorLog.writef(PrefixError,
"Could not find pin for sequence %d in component architype '%s'\n",
pinSequence,(const char*)plbName);

               continue;
            }

            DataStruct* pinData = getCamCadDatabase().referenceBlock(plbBlock,genPadStackName(landTypeName),insertType,pin->getPinName(),
               layerIndex,getBoardFileNum(),position.x,position.y,angleInRadians,mirror,scale,global);

            getCamCadDatabase().addAttribute(getAttributeMap(pinData),
               getAttributeIndex(attributeIndexCompPinnr)  ,pin->getSequenceNumber(),errorLog);
            getCamCadDatabase().addAttribute(getAttributeMap(pinData),
               getAttributeIndex(attributeIndexPinFunction),pin->getFunction(),errorLog);
         }
      }
   }
   else if (m_record.isGood(sectPlb,alcatelKwDdrwfig ,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwSilkfig ,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwKpoutara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwBkpotara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwViaihara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwLvaihara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwLinihara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwCopihara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwNcpihara,errorLog) || 
            m_record.isGood(sectPlb,alcatelKwResihara,errorLog)    )
   {
      ASSERT (m_plbBlock != NULL);

      // add a component outline to the plb
      CString entityType   = m_record.getParam(1);
      int side             = m_record.getInt(2);
      bool floodFlag       = (m_record.getInt(3) == 1);
      int width            = m_record.getIntInches(4);
      OutlineKindTag outlineKind = stringToOutlineKindTag(m_record.getParam(0));
      bool sideWarningFlag = false;
      CString layerName;
      LayerCategoryTag layerCategory = lcUndefined;

      switch(outlineKind)
      {
      case olDrawingFigure:
      case olSilkFigure:
         switch (side)
         {
         case 0:   layerCategory = lcTop;              break;
         case 1:   layerCategory = lcBottom;           break;
         default:  layerCategory = lcUndefined;        break;
         }

         break;
      default:
         switch (side)
         {
         case 0:   layerCategory = lcNone;             break;
         case 1:   layerCategory = lcAll;              break;
         case 2:   layerCategory = lcTop;              break;
         case 3:   layerCategory = lcBottom;           break;
         case 4:   layerCategory = lcMountedTop;       break;
         case 5:   layerCategory = lcMountedBottom;    break;
         default:  layerCategory = lcUndefined;        break;
         }

         break;
      }

      if (layerCategory == lcUndefined)
      {
errorLog.writef(PrefixWarning,
"Side value of %d is not implemented for outline kind of '%s'\n",
side,(const char*)m_record.getParam(0));
      }
      else
      {
         for (int ind=0;ind < m_plbNames.GetCount();ind++)
         {
            CString plbName = m_plbNames.GetAt(ind);

            BlockStruct* plbBlock = getCamCadDatabase().getDefinedBlock(plbName,blockTypePcbComponent,getBoardFileNum());

            addArea(plbBlock,layerName,layerCategory,outlineKind,
               stringToEntityTypeTag(entityType),5,errorLog,width,floodFlag);
         }
      }
   }
   else if (m_record.isGood(sectPlb,alcatelKwPtnend,errorLog))
   {
      if (m_plbBlock != NULL)
      {
         m_plbBlock = NULL;
      }

      m_plbNames.RemoveAll();
   }
}

void CAlcatelGmfDatabase::processIctRecord(CWriteFormat& errorLog)
{
}

void CAlcatelGmfDatabase::processDrlRecord(CWriteFormat& errorLog)
{
}

void CAlcatelGmfDatabase::processRutRecord(CWriteFormat& errorLog)
{
   bool landFlag = false;
   bool skipZeroWidthLinesFlag = true;

   if (m_record.isGood(sectRut,alcatelKwNetdef,errorLog))
   {
      m_netName = m_record.getParam(1);

      if (m_netName != m_rutNetName)
      {
         m_rutPolyStruct = NULL;
      }
   }
   else if (m_record.isGood(sectRut,alcatelKwLine,errorLog))
   {
      CString layerName = m_record.getParam(1);
      double lineWidth  = m_record.getInches(2);

      if (layerName != m_rutLayerName || m_rutPolyStruct == NULL)
      {
         int layerIndex = getCamCadDatabase().getDefinedLayerIndex(layerName);
         m_rutPolyStruct = getCamCadDatabase().addPolyStruct(getBoardBlock(),layerIndex,0,0, graphicClassEtch);
         getCamCadDatabase().addAttribute(getAttributeMap(m_rutPolyStruct),
            getAttributeIndex(attributeIndexNetName),m_netName,errorLog);
         m_rutPoly = NULL;
         m_rutVertexCount = 0;
         m_rutLayerName = layerName;
      }

      if (lineWidth != m_rutLineWidth)
      {
         m_rutPoly = NULL;
         m_rutVertexCount = 0;
         m_rutLineWidth = lineWidth;
      }

      if (!(lineWidth == 0. && skipZeroWidthLinesFlag))
      {
         CPoint2d position;
         const double bulge=0.;

         int firstInd = 3;
         int lastInd  = m_record.getParamCount() - 1;
         int indInc   = 1;

         ASSERT(lastInd > firstInd);

         if (m_rutVertexCount >= 2)
         {
            // determine if the incoming record connects with the previous poly
            CPoint2d firstPoint = m_record.getCoord(firstInd);
            CPoint2d lastPoint  = m_record.getCoord(lastInd);

            if (m_rutVertexCount == 2)
            {
               // if necessary,flip the first poly
               if (firstPoint == m_rutFirstVertexPoint || lastPoint == m_rutFirstVertexPoint)
               {
                  DbUnit x     =  m_rutFirstVertex->x;
                  DbUnit y     =  m_rutFirstVertex->y;
               //DbUnit bulge =  m_rutFirstVertex->bulge;
                  m_rutFirstVertex->x     = m_rutLastVertex->x;
                  m_rutFirstVertex->y     = m_rutLastVertex->y;
               //m_rutFirstVertex->bulge = m_rutLastVertex->bulge;
                  m_rutLastVertex->x      = x;
                  m_rutLastVertex->y      = y;
               //m_rutLastVertex->bulge  = bulge;

                  CPoint2d temp         = m_rutFirstVertexPoint;
                  m_rutFirstVertexPoint = m_rutLastVertexPoint;
                  m_rutLastVertexPoint  = temp;
               }
            }

            if (firstPoint == m_rutLastVertexPoint)
            {
               firstInd++;
            }
            else if (lastPoint == m_rutLastVertexPoint)
            {
               int tmp  = firstInd;
               firstInd = lastInd - 1;
               lastInd  = firstInd;
               indInc   = -1;
            }
            else
            {
               m_rutPoly = NULL;
            }
         }

         if (m_rutPoly == NULL)
         {
            int widthIndex = getCamCadDatabase().getDefinedWidthIndex(lineWidth);
            m_rutPoly = getCamCadDatabase().addOpenPoly(m_rutPolyStruct,widthIndex);
            m_rutLineWidth = lineWidth;
            m_rutVertexCount = 0;
         }

         for (int ind = firstInd;;ind += indInc)
         {
            m_rutLastVertexPoint = m_record.getCoord(ind);
            position = m_rutLastVertexPoint;
            convertToInches(position);

            m_rutLastVertex = getCamCadDatabase().addVertex(m_rutPoly,position.x,position.y,bulge);

            if (m_rutVertexCount == 0)
            {
               m_rutFirstVertex      = m_rutLastVertex;
               m_rutFirstVertexPoint = m_rutLastVertexPoint;
            }

            m_rutVertexCount++;

            if (ind == lastInd)
            {
               break;
            }
         }
      }
   }
   else if (m_record.isGood(sectRut,alcatelKwVia,errorLog) || (landFlag = m_record.isGood(sectRut,alcatelKwLand,errorLog)))
   {
      CSupString layerNames = m_record.getParam(1);
      CStringArray params;
      layerNames.Parse(params,"-");

      if (params.GetCount() < 2)
      {
errorLog.writef(PrefixError,"Expected layer names in VIA record, but '%s' was found.\n",
(const char*)layerNames);
      }
      else
      {
         CString ltpName    = m_record.getParam(2);
         CString layer1name = params[0];
         CString layer2name = params[1];
         bool topFlag = (m_record.getParam(3) == "TOP");
         double rotation = degreesToRadians(m_record.getDegrees(4));

         CString viaName = (landFlag ? "LAND_" : "VIA_") + ltpName;
         CString padStackName = genPadStackName(ltpName);

         bool regularViaFlag = ( (layer1name == getTopLayerName() && layer2name == getBottomLayerName()) ||
                                 (layer2name == getTopLayerName() && layer1name == getBottomLayerName())    );

         if (!regularViaFlag)
         {
            viaName += "_" + layer1name + "_" + layer2name;
         }

         viaName += (topFlag ? "_T" : "_B");

         instantiateFlattenedVia(viaName,padStackName,layer1name,layer2name,topFlag);

         CPoint2d position;

         for (int ind = 5;ind < m_record.getParamCount();ind++)
         {
            position = m_record.getCoordInInches(ind);

            DataStruct* viaData = getCamCadDatabase().referenceBlock(getBoardBlock(),viaName,INSERTTYPE_VIA,
               "",TRUE,getBoardFileNum(),position.x,position.y,rotation/*,!topFlag*/);

            getCamCadDatabase().addAttribute(getAttributeMap(viaData),
               getAttributeIndex(attributeIndexNetName),m_netName,errorLog);
         }
      }
   }
   else if (m_record.isGood(sectRut,alcatelKwLand,errorLog))
   {
   }
   else if (m_record.isGood(sectRut,alcatelKwCopper,errorLog))
   {
      CString layerName = m_record.getParam(1);

      int layerIndex = getCamCadDatabase().getDefinedLayerIndex(layerName);
      const int widthIndex = 0;  // small width
      DataStruct* polyStruct = NULL;
      CPoly* poly = NULL;
      double x,y,bulge=0.;

      for (int ind = 2;ind < m_record.getParamCount();ind++)
      {
         CString kind = m_record.getParam(ind);

         if (kind == "OUTER")
         {
            polyStruct = getCamCadDatabase().addPolyStruct(getBoardBlock(),layerIndex,0,0, graphicClassEtch);
            getCamCadDatabase().addAttribute(getAttributeMap(polyStruct),
               getAttributeIndex(attributeIndexNetName),m_netName,errorLog);

            poly = getCamCadDatabase().addFilledPoly(polyStruct,widthIndex);  // filled, not void, closed
         }
         else if (kind == "INNER")
         {
            if (polyStruct != NULL)
            {
               poly = getCamCadDatabase().addVoidPoly(polyStruct,widthIndex);  // filled, void, closed
            }
         }
         else
         {
            if (poly != NULL)
            {
               x = m_record.getInches(ind + 0);
               y = m_record.getInches(ind + 1);

               ind++;

               getCamCadDatabase().addVertex(poly,x,y,bulge);
            }
         }
      }
   }
   else if (m_record.isGood(sectRut,alcatelKwNetend,errorLog))
   {
   }
}

void CAlcatelGmfDatabase::processOnsRecord(CWriteFormat& errorLog)
{
}







