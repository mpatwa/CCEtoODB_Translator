// $Header: /CAMCAD/DcaLib/DcaKeyword.cpp 8     6/17/07 9:01p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaKeyword.h"
#include "DcaLib.h"

//_____________________________________________________________________________
CString valueTypeTagToString(ValueTypeTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case valueTypeString:        retval = "String";         break;
   case valueTypeDouble:        retval = "Double";         break;
   case valueTypeInteger:       retval = "Integer";        break;
   case valueTypeNone:          retval = "None";           break;
   case valueTypeUnitDouble:    retval = "Unit Double";    break;
   case valueTypeEmailAddress:  retval = "Email Address";  break;
   case valueTypeWebAddress:    retval = "Web Address";    break;
   case valueTypeUndefined:     retval = "Undefined";      break;
   default:                     retval = "Unknown";        break;
   }

   return retval;
}

ValueTypeTag intToValueTypeTag(int tagValue)
{
   ValueTypeTag retval;

   switch (tagValue)
   {
   case valueTypeString:        retval = valueTypeString;        break;
   case valueTypeDouble:        retval = valueTypeDouble;        break;
   case valueTypeInteger:       retval = valueTypeInteger;       break;
   case valueTypeNone:          retval = valueTypeNone;          break;
   case valueTypeUnitDouble:    retval = valueTypeUnitDouble;    break;
   case valueTypeEmailAddress:  retval = valueTypeEmailAddress;  break;
   case valueTypeWebAddress:    retval = valueTypeWebAddress;    break;
   default:                     retval = valueTypeUndefined;     break;
   }

   return retval;
}

//_____________________________________________________________________________
CString attribGroupTagToString(AttribGroupTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case attribGroupUser:       retval = "User";       break;
   case attribGroupInternal:   retval = "Internal";   break;
   case attribGroupTest:       retval = "Test";       break;
   case attribGroupHpTest:     retval = "HP Test";    break;
   case attribGroupOgp:        retval = "OGP";        break;
   case attribGroupPcb:        retval = "PCB";        break;
   case attribGroupAssy:       retval = "Assembly";   break;
   case attribGroupDfm:        retval = "DFM";        break;
   case attribGroupRealPart:   retval = "Real Part";  break;
   case attribGroupHaf:        retval = "HAF";        break;
   case attribGroupUndefined:  retval = "Undefined";  break;
   }

   return retval;
}

AttribGroupTag intToAttribGroupTag(int tagValue)
{
   AttribGroupTag retval;

   switch (tagValue)
   {
   case attribGroupUser:      retval = attribGroupUser;       break;
   case attribGroupInternal:  retval = attribGroupInternal;   break;
   case attribGroupTest:      retval = attribGroupTest;       break;
   case attribGroupHpTest:    retval = attribGroupHpTest;     break;
   case attribGroupOgp:       retval = attribGroupOgp;        break;
   case attribGroupPcb:       retval = attribGroupPcb;        break;
   case attribGroupAssy:      retval = attribGroupAssy;       break;
   case attribGroupDfm:       retval = attribGroupDfm;        break;
   case attribGroupRealPart:  retval = attribGroupRealPart;   break;
   case attribGroupHaf:       retval = attribGroupHaf;        break;
   default:                   retval = attribGroupUndefined;  break;
   }

   return retval;
}

//_____________________________________________________________________________
CString StandardAttributeTagToName(StandardAttributeTag standardAttributeTag)
{
   CString retval;

   switch (standardAttributeTag)
   {
   case standardAttribute3070Comment:                  retval = "3070_COMMENT";                 break;
   case standardAttribute3070CommonPin:                retval = "3070_COMMON_PIN";              break;
   case standardAttribute3070ContactPin:               retval = "3070_CONTACT_PIN";             break;
   case standardAttribute3070DeviceClass:              retval = "3070_DEVICECLASS";             break;
   case standardAttribute3070FailureMessage:           retval = "3070_FAILURE_MESSAGE";         break;
   case standardAttribute3070HiValue:                  retval = "3070_HI_VAL";                  break;
   case standardAttribute3070LowValue:                 retval = "3070_LOW_VAL";                 break;
   case standardAttribute3070MinusTolerance:           retval = "3070_NEG_TOL";                 break;
   case standardAttribute3070PlusTolerance:            retval = "3070_POS_TOL";                 break;
   case standardAttribute3070ProbeAccess:              retval = "3070_PROBEACCESS";             break;
   case standardAttribute3070ProbeSide:                retval = "3070_PROBESIDE";               break;
   case standardAttribute3070Type:                     retval = "3070_TYPE";                    break;
   case standardAttribute3070Value:                    retval = "3070_VALUE";                   break;
   case standardAttribute3070Netname:                  retval = "3070_NETNAME";                 break;
   case standardAttribute5dxAlignment1:                retval = "5DX_ALIGNMENT_1";              break;
   case standardAttribute5dxAlignment2:                retval = "5DX_ALIGNMENT_2";              break;
   case standardAttribute5dxAlignment3:                retval = "5DX_ALIGNMENT_3";              break;
   case standardAttribute5dxPackgeId:                  retval = "5DX_PACKAGE_ID";               break;
   case standardAttribute5dxPinFamily:                 retval = "5DX_PIN_FAMILY";               break;
   case standardAttribute5dxPitch:                     retval = "5DX_PITCH";                    break;
   case standardAttribute5dxTest:                      retval = "5DX_TEST";                     break;
   case standardAttribute5dxViaTest:                   retval = "5DX_VIATEST";                  break;
   case standardAttributeAccessDistance:               retval = "ACCESS_DISTANCE";              break;
   case standardAttributeAeroflexSubclass:             retval = "Aeroflex_Subclass";            break;
   case standardAttributeAlternateShape:               retval = "ALTERNATE_SHAPE";              break;
   case standardAttributeAoiPlacmentFeeder:            retval = "AOI_PLACEMENT_FEEDER";         break;
   case standardAttributeAoiPlacmentMachine:           retval = "AOI_PLACEMENT_MACHINE";        break;
   case standardAttributeAoiTest:                      retval = "AOI_TEST";                     break;
   case standardAttributeBlind:                        retval = "BLIND";                        break;
   case standardAttributeBottomPadstackDesignSurface:  retval = "BOT_PADSTACK_DESIGN_SURFACE";  break;
   case standardAttributeCBNetname:                    retval = "CB_NETNAME";                   break;
   case standardAttributeCentroidX:                    retval = "CENTROID_X";                   break;
   case standardAttributeCentroidY:                    retval = "CENTROID_Y";                   break;
   case standardAttributeCircuitNetName:               retval = "CKT_NETNAME";                  break;
   case standardAttributeComponentHeight:              retval = "COMPHEIGHT";                   break;
   case standardAttributeComponentOutlineOrientation:  retval = "ComponentOutlineOrientation";  break;
   case standardAttributeCurrent:                      retval = "CURRENT";                      break;
   case standardAttributeCurrentVariant:               retval = "CurrentVariant";               break;
   case standardAttributeDataType:                     retval = "DataType";                     break;
   case standardAttributeDataLink:                     retval = "DATALINK";                     break;
   case standardAttributeDataSource:                   retval = "DataSource";                   break;
   case standardAttributeDebugId:                      retval = "DebugId";                      break;
   case standardAttributeDerivedFrom:                  retval = "DERIVED_FROM";                 break;
   case standardAttributeDescription:                  retval = "DESCRIPTION";                  break;
   case standardAttributeDesignedSurface:              retval = "DESIGNED_SURFACE";             break;
   case standardAttributeDeviceToPackagePinMap:        retval = "DEVICE_TO_PACKAGE_PINMAP";     break;
   case standardAttributeDeviceType:                   retval = "DEVICETYPE";                   break;
   case standardAttributeDfmActualValue:               retval = "DFM_ACTUAL_VALUE";             break;
   case standardAttributeDfmCheckValue:                retval = "DFM_CHECK_VALUE";              break;
   case standardAttributeDfmFailureRange:              retval = "DFM_FAILURE_RANGE";            break;
   case standardAttributeDfmKeyword:                   retval = "DFM_KEYWORD";                  break;
   case standardAttributeDfmLayerStackNumber:          retval = "DFM_LAYER_STACKNUMBER";        break;
   case standardAttributeDfmLayerType:                 retval = "DFM_LAYER_TYPE";               break;
   case standardAttributeDfmPackageFamily:             retval = "PACKAGE_FAMILY";               break;
   case standardAttributeDfmRef1:                      retval = "DFM_REF1";                     break;
   case standardAttributeDfmRef2:                      retval = "DFM_REF2";                     break;
   case standardAttributeDfmTestName:                  retval = "DFM_TEST_NAME";                break;
   case standardAttributeDfmValue1:                    retval = "DFM_VALUE1";                   break;
   case standardAttributeDfmValue2:                    retval = "DFM_VALUE2";                   break;
   case standardAttributeDirectionOfTravel:            retval = "DIRECTION_OF_TRAVEL";          break;
   case standardAttributeDxfOutlineType:               retval = "DFX_OUTLINE_TYPE";             break;
   case standardAttributeEcadBoardOnPanel:             retval = "ECAD_BOARD_ON_PANEL";          break;
   case standardAttributeEcadGeometryName:             retval = "ECAD_GEOMETRY_NAME";           break;
   case standardAttributeEcadPin:                      retval = "ECAD_PIN";                     break;
   case standardAttributeEcadRefDes:                   retval = "ECAD_REFDES";                  break;
   case standardAttributeEducatorSurface:              retval = "EducatorSurface";              break;
   case standardAttributeFiducialPlacedBoth:           retval = "FIDUCIAL_BOTH";                break;
   case standardAttributeGencamType:                   retval = "GENCAM_TYPE";                  break;
   case standardAttributeGeomReviewStatus:             retval = "GeomReviewStatus";             break;
   case standardAttributeGridLocation:                 retval = "GRID_LOCATION";                break;
   case standardAttributeHafBoundaryScan:              retval = "HAF_BOUNDARYSCAN";             break;
   case standardAttributeHafComment:                   retval = "HAF_COMMENT";                  break;
   case standardAttributeHafComponentHeight:           retval = "HAF_COMPHEIGHT";               break;
   case standardAttributeHafDeviceClass:               retval = "HAF_DEVICECLASS";              break;
   case standardAttributeHafMinusTolerance:            retval = "HAF_NTOL";                     break;
   case standardAttributeHafNetPotential:              retval = "HAF_NETPOTENTIAL";             break;
   case standardAttributeHafPackage:                   retval = "HAF_PACKAGE";                  break;
   case standardAttributeHafPartNumber:                retval = "HAF_PARTNUMBER";               break;
   case standardAttributeHafPinDirection:              retval = "HAF_PINDIRECTION";             break;
   case standardAttributeHafPinFunction:               retval = "HAF_PINFUNCTION";              break;
   case standardAttributeHafPlusTolerance:             retval = "HAF_PTOL";                     break;
   case standardAttributeHafRefName:                   retval = "HAF_REFNAME";                  break;
   case standardAttributeHafTest:                      retval = "HAF_TEST";                     break;
   case standardAttributeHafValue:                     retval = "HAF_VALUE";                    break;
   case standardAttributeIngunDeviceClass:             retval = "TRI_DEVICECLASS";              break;
   case standardAttributeInsertionProcess:             retval = "INSERTION PROCESS";            break;
   case standardAttributeInstanceName:                 retval = "INSTANCENAME";                 break;
   case standardAttributeKeepDraw:                     retval = "KEEPDRAW";                     break;
   case standardAttributeLayerThickness:               retval = "LAYER THICKNESS";              break;
   case standardAttributeLoaded:                       retval = "LOADED";                       break;
   case standardAttributeMergeToolSurface:             retval = "MergeToolSurface";             break;
   case standardAttributeMinusTolerance:               retval = "-TOL";                         break;
   case standardAttributeMiscellaneous:                retval = "Miscellaneous";                break;
   case standardAttributeName:                         retval = "NAME";                         break;
   case standardAttributeNetName:                      retval = "NETNAME";                      break;
   case standardAttributeNetType:                      retval = "NET_TYPE";                     break;
   case standardAttributeNoConnect:                    retval = "No Connect";                   break;
   case standardAttributeNoProbe:                      retval = "No_Probe";                     break;
   case standardAttributePackageLink:                  retval = "$$PACKAGE$$";                  break;
   case standardAttributePadUsage:                     retval = "PAD_USAGE";                    break;
   case standardAttributePartNumber:                   retval = "PARTNUMBER";                   break;
   case standardAttributePath:                         retval = "PATH";                         break;
   case standardAttributePinAccess:                    retval = "PIN_ACCESS";                   break;
   case standardAttributePinCount:                     retval = "PIN_COUNT";                    break;
   case standardAttributePinFunction:                  retval = "PINFUNC";                      break;
   case standardAttributePinLogicName:                 retval = "PINDESC";                      break;
   case standardAttributePinNumber:                    retval = "PINNR";                        break;
   case standardAttributePinPitch:                     retval = "PIN_PITCH";                    break;
   case standardAttributePinsModified:                 retval = "PINS_MODIFIED";                break;
   case standardAttributePirRequired:                  retval = "PIR_Required";                 break;
   case standardAttributePlusTolerance:                retval = "+TOL";                         break;
   case standardAttributePowerNet:                     retval = "POWERNET";                     break;
   case standardAttributeProbePlacement:               retval = "PROBE_PLACEMENT";              break;
   case standardAttributeProbeSize:                    retval = "PROBE_SIZE";                   break;
   case standardAttributeProbeStyle:                   retval = "PROBE_STYLE";                  break;
   case standardAttributeRefName:                      retval = "REFNAME";                      break;
   case standardAttributeSiemensBoardThickness:        retval = "SIEMENS_THICKNESS_BOARD";      break;
   case standardAttributeSiemensFiducialSymbol:        retval = "SIEMENS_FIDSYM";               break;
   case standardAttributeSiemensPanelThickness:        retval = "SIEMENS_THICKNESS_PANEL";      break;
   case standartAttributeSiemensDialogSetting:         retval = "SiemensDialogSetting";         break;
   case standardAttributeSinglePinNet:                 retval = "SINGLEPINNET";                 break;
   case standardAttributeSmd:                          retval = "SMD";                          break;
   case standardAttributeSolderMask:                   retval = "SOLDERMASK";                   break;
   case standardAttributeSonyAoiSettings:              retval = "SonyAoiSettings";              break;
   case standardAttributeSpeaSubclass:                 retval = "Spea4040_Subclass";            break;
   case standardAttributeStencilGeneratorTempComponent:retval = "StencilGeneratorTempComponent";break;
   case standardAttributeStencilThickness:             retval = "StencilThickness";             break;
   case standardAttributeSubclass:                     retval = "SUBCLASS";                     break;
   case standardAttributeTakayaAccess:                 retval = "TAKAYA_ACCESS";                break;
   case standardAttributeTakayaDeviceClass:            retval = "TAKAYA_DEVICECLASS";           break;
   case standardAttributeTakayaDeviceClassSubclass:    retval = "TAKAYA_DEVICECLASS_SUBCLASS";  break;
   case standardAttributeTakayaRefDes:                 retval = "TAKAYA_REFDES";                break;
   case standardAttributeTakayaReferencePointSetting:  retval = "TakayaReferecePointSetting";   break;
   case standardAttributeTargetType:                   retval = "TARGET_TYPE";                  break;
   case standardAttributeTechnology:                   retval = "TECHNOLOGY";                   break;
   case standardAttributeTeradyne7200Test:             retval = "TERADYNE_7200_TEST";           break;
   case standardAttributeTeradyneAoiDeviceClass:       retval = "TERADYNEAOI_DEVICECLASS";      break;
   case standardAttributeTeradyneAoiPinTest:           retval = "TERADYNE_AOI_PINTEST";         break;
   case standardAttributeTeradyneSubclass:             retval = "Teradyne1800_Subclass";        break;
   case standardAttributeTest:                         retval = "TEST";                         break;
   case standardAttributeTestAccess:                   retval = "TEST_ACCESS";                  break;
   case standardAttributeTestAccessFail:               retval = "TEST_ACCESS_FAIL";             break;
   case standardAttributeTestAccessFailTop:            retval = "TEST_ACCESS_FAIL_TOP";         break;
   case standardAttributeTestAccessFailBottom:         retval = "TEST_ACCESS_FAIL_BOTTOM";      break;

   case standardAttributeTestConnector:                retval = "TEST_CONNECTOR";               break;
   case standardAttributeTesterInterface:              retval = "TIN";                          break;
   case standardAttributeTestIgnoreOutline:            retval = "TEST_IGNORE_OUTLINE";          break;
   case standardAttributeTestNetProbes:                retval = "TEST_NET_PROBES";              break;
   case standardAttributeTestNetStatus:                retval = "TEST_NET_STATUS";              break;
   case standardAttributeTestOffsetBottomX:            retval = "TEST_OFFSET_BOT_X";            break;
   case standardAttributeTestOffsetBottomY:            retval = "TEST_OFFSET_BOT_Y";            break;
   case standardAttributeTestOffsetTopX:               retval = "TEST_OFFSET_TOP_X";            break;
   case standardAttributeTestOffsetTopY:               retval = "TEST_OFFSET_TOP_Y";            break;
   case standardAttributeTestPreference:               retval = "TEST_PREFERENCE";              break;
   case standardAttributeTestProbeFail:                retval = "TEST_PROBE_FAIL";              break;
   case standardAttributeTestResource:                 retval = "TEST_RESOURCE";                break;
   case standardAttributeTestShort:                    retval = "TEST_SHORT";                   break;
   case standardAttributeTolerance:                    retval = "TOLERANCE";                    break;
   case standardAttributeTopPadstackDesignSurface:     retval = "TOP_PADSTACK_DESIGN_SURFACE";  break;
   case standardAttributeTrRequired:                   retval = "TR_Required";                  break;
   case standardAttributeTypeListLink:                 retval = "$$DEVICE$$";                   break;
   case standardAttributeUnplaced:                     retval = "UNPLACED";                     break;
   case standardAttributeValue:                        retval = "VALUE";                        break;
   case standardAttributeViaLayer:                     retval = "VIALAYER";                     break;
   case standardAttributeVoltage:                      retval = "VOLTAGE";                      break;
   case standardAttributeWire:                         retval = "WIRE";                         break;
   }

   return retval;
}

//_____________________________________________________________________________
int KeyWordStruct::m_allocationCount = 0;

KeyWordStruct::KeyWordStruct(int index)
: m_index(index)
{
   m_allocationCount++;

   group       = attribGroupUser;
   valueType   = valueTypeNone;
   m_inherited = false;
   m_hidden    = false;
}

KeyWordStruct::KeyWordStruct(int index,const CString& keyword,AttribGroupTag _group,ValueTypeTag _valueType,bool inherited,bool hidden)
: m_index(index)
, in(keyword)
, cc(keyword)
, out(keyword)
, group(_group)
, valueType(_valueType)
, m_inherited(inherited)
, m_hidden(hidden)
{
   m_allocationCount++;
}

KeyWordStruct::~KeyWordStruct()
{
   m_allocationCount--;
}

//_____________________________________________________________________________
CKeyWordArray::CKeyWordArray()
{
   int hashSize = nextPrime2n(500);

   m_inMap.InitHashTable(hashSize);
   m_ccMap.InitHashTable(hashSize);
   m_outMap.InitHashTable(hashSize);

   m_keywordArray.SetSize(0,hashSize);
}

void CKeyWordArray::empty()
{   
   m_keywordArray.empty(); 

   m_inMap.RemoveAll();
   m_ccMap.RemoveAll();
   m_outMap.RemoveAll();
}

int CKeyWordArray::SetAtGrow(const CString& keyword, AttribGroupTag group, ValueTypeTag valueType, bool inherited, bool hidden, int nIndex)
{
   if (nIndex < 0)
      nIndex = GetCount();

   WORD index = 0;
   CString upperKeyword = keyword;
   upperKeyword.MakeUpper();

   if (m_ccMap.Lookup(upperKeyword, index))
      return -1;

   m_keywordArray.setAtGrow(nIndex, new KeyWordStruct(nIndex, keyword, group, valueType, inherited, hidden));
   m_ccMap.SetAt(upperKeyword, nIndex);

   return nIndex;
}

int CKeyWordArray::getDefinedKeywordIndex(const CString& keywordName,ValueTypeTag valueType)
{
   int keywordIndex = getKeywordIndex(keywordName);

   if (keywordIndex < 0)
   {
      keywordIndex = SetAtGrow(keywordName,attribGroupUser,valueType);
   }

   return keywordIndex;
}

int CKeyWordArray::GetSize() const
{
   return (int) m_keywordArray.GetSize();
}

int CKeyWordArray::GetCount() const
{
   return (int) m_keywordArray.GetCount();
}


int CKeyWordArray::Lookup(CString keyword, EKeyWordSection section) const
{
   WORD index = 0;
   BOOL found = FALSE;
   keyword.MakeUpper();

   switch (section)
   {
   case keyWordSectionCC:
      if (m_ccMap.Lookup(keyword, index))
         return index;
      return -1;
   case keyWordSectionIn:
      {
         for (int i=0; i<GetCount(); i++)
         {
            if (!GetAt(i)->getInKeyword().CompareNoCase(keyword))
               return i;
         }
         break;
      }
   case keyWordSectionOut:
      {
         for (int i=0; i<GetCount(); i++)
         {
            if (!GetAt(i)->getOutKeyword().CompareNoCase(keyword))
               return i;
         }
         break;
      }
   }

   return -1;
}

int CKeyWordArray::getKeywordIndex(CString keyword) const
{
   int retval;
   keyword.MakeUpper();

   WORD keywordIndex;

   if (m_ccMap.Lookup(keyword,keywordIndex))
   {
      retval = keywordIndex;
   }
   else
   {
      retval = -1;
   }

   return retval;
}

KeyWordStruct* CKeyWordArray::getKeywordAt(const CString& keywordName) const
{
   KeyWordStruct* keywordStruct = NULL;

   int keywordIndex = getKeywordIndex(keywordName);

   if (keywordIndex >= 0)
   {
      keywordStruct = this->m_keywordArray.GetAt(keywordIndex);
   }

   return keywordStruct;
}

void CKeyWordArray::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
   m_inMap.RemoveKey(GetAt(nIndex)->getInKeyword());
   m_ccMap.RemoveKey(GetAt(nIndex)->getCCKeyword());
   m_outMap.RemoveKey(GetAt(nIndex)->getOutKeyword());

   m_keywordArray.RemoveAt(nIndex, nCount);
}

KeyWordStruct* CKeyWordArray::getAt(int index) const
{
   KeyWordStruct* keyword = NULL;

   if (index >= 0 && index < GetCount())
   {
      keyword = m_keywordArray.GetAt(index);
   }

   return keyword;
}

bool CKeyWordArray::SetCCKeyword(int index, CString cc)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      WORD value = 0;
      CString ccUpper = cc;
      ccUpper.MakeUpper();

      if (m_ccMap.Lookup(ccUpper, value))
      {
         // this one exists already, so we need to make sure we don't make duplicates
         if (value != index)
            return false;

         return true;
      }

      m_ccMap.RemoveKey(keyword->getCCKeyword().MakeUpper());

      keyword->setCCKeyword(cc);
      m_ccMap.SetAt(keyword->getCCKeyword().MakeUpper(), index);
   }

   return retval;
}

bool CKeyWordArray::SetInKeyword(int index, CString in)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      keyword->setInKeyword(in);
   }

   return retval;
}

bool CKeyWordArray::SetOutKeyword(int index, CString out)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      keyword->setOutKeyword(out);
   }

   return retval;
}

bool CKeyWordArray::SetGroup(int index, AttribGroupTag group)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      keyword->setGroup(group);
   }

   return retval;
}

bool CKeyWordArray::SetInherited(int index, bool flag)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      keyword->setInherited(flag);
   }

   return retval;
}

bool CKeyWordArray::SetHidden(int index, bool flag)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      keyword->setHidden(flag);
   }

   return retval;
}

bool CKeyWordArray::SetValueType(int index, ValueTypeTag type)
{
   KeyWordStruct* keyword = getAt(index);

   bool retval = (keyword != NULL);

   if (retval)
   {
      keyword->setValueType(type);
   }

   return retval;
}
