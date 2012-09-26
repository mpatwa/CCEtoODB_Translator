// $Header: /CAMCAD/DcaLib/DcaKeyword.h 8     6/17/07 9:01p Kurt Van Ness $

#if !defined(__DcaKeyword_h__)
#define __DcaKeyword_h__

#pragma once

#include "DcaContainer.h"
#include "DcaCollections.h"

//_____________________________________________________________________________
enum ValueTypeTag
{
   valueTypeString       = 1,  // VT_STRING          1
   valueTypeDouble       = 2,  // VT_DOUBLE          2
   valueTypeInteger      = 3,  // VT_INTEGER         3
   valueTypeNone         = 4,  // VT_NONE            4
   valueTypeUnitDouble   = 5,  // VT_UNIT_DOUBLE     5
   valueTypeEmailAddress = 6,  // VT_EMAIL_ADDRESS   6
   valueTypeWebAddress   = 7,  // VT_WEB_ADDRESS     7
   valueTypeUndefined    = 8,
   ValueTypeTagMin       = 1, // \used to iterate over valid values by EnumIterator
   ValueTypeTagMax       = 7  // /
};

CString valueTypeTagToString(ValueTypeTag tagValue);
ValueTypeTag intToValueTypeTag(int tagValue);

//_____________________________________________________________________________
enum AttribGroupTag
{
   attribGroupUser         =  0,
   attribGroupInternal     =  1,
   attribGroupTest         =  2,
   attribGroupHpTest       =  3,
   attribGroupOgp          =  4,
   attribGroupPcb          =  5,
   attribGroupAssy         =  6,
   attribGroupDfm          =  7,
   attribGroupRealPart     =  8,
   attribGroupHaf          =  9,
   attribGroupUndefined    = -1,
   AttribGroupTagMin       =  0, // \used to iterate over valid values by EnumIterator
   AttribGroupTagMax       =  9  // /
};

CString attribGroupTagToString(AttribGroupTag tagValue);
AttribGroupTag intToAttribGroupTag(int tagValue);

//_____________________________________________________________________________
enum EKeyWordSection
{
   keyWordSectionCC  = 0,
   keyWordSectionIn  = 1,
   keyWordSectionOut = 2,
};

//_____________________________________________________________________________
enum StandardAttributeTag
{
   // knv - 20060917.1507
   // When adding a new enum value:
   // - add new enum values in alphabetical order
   // - update the StandardAttributeTagToName() function
   // - update the CCEtoODBDoc::RegisterInternalKeywords() function

   standardAttribute3070Comment,                  // "3070_COMMENT"
   standardAttribute3070CommonPin,                // "3070_COMMON_PIN"
   standardAttribute3070ContactPin,               // "3070_CONTACT_PIN"
   standardAttribute3070DeviceClass,              // "3070_DEVICECLASS"
   standardAttribute3070FailureMessage,           // "3070_FAILURE_MESSAGE"
   standardAttribute3070HiValue,                  // "3070_HI_VAL"
   standardAttribute3070LowValue,                 // "3070_LOW_VAL"
   standardAttribute3070MinusTolerance,           // "3070_NEG_TOL"
   standardAttribute3070PlusTolerance,            // "3070_POS_TOL"
   standardAttribute3070ProbeAccess,              // "3070_PROBEACCESS"
   standardAttribute3070ProbeSide,                // "3070_PROBESIDE"
   standardAttribute3070Type,                     // "3070_TYPE"
   standardAttribute3070Value,                    // "3070_VALUE"
   standardAttribute3070Netname,                  // "3070_NETNAME"
   standardAttribute5dxAlignment1,                // "5DX_ALIGNMENT_1"
   standardAttribute5dxAlignment2,                // "5DX_ALIGNMENT_2"
   standardAttribute5dxAlignment3,                // "5DX_ALIGNMENT_3"
   standardAttribute5dxPackgeId,                  // "5DX_PACKAGE_ID"
   standardAttribute5dxPinFamily,                 // "5DX_PIN_FAMILY"
   standardAttribute5dxPitch,                     // "5DX_PITCH"
   standardAttribute5dxTest,                      // "5DX_TEST"
   standardAttribute5dxViaTest,                   // "5DX_VIATEST"
   standardAttributeAccessDistance,               // "ACCESS_DISTANCE"
   standardAttributeAeroflexSubclass,             // "Aeroflex_Subclass"
   standardAttributeAlternateShape,               // "ALTERNATE_SHAPE"
   standardAttributeAoiPlacmentFeeder,            // "AOI_PLACEMENT_FEEDER"
   standardAttributeAoiPlacmentMachine,           // "AOI_PLACEMENT_MACHINE"
   standardAttributeAoiTest,                      // "AOI_TEST"
   standardAttributeBlind,                        // "BLIND"
   standardAttributeBottomPadstackDesignSurface,  // "BOT_PADSTACK_DESIGN_SURFACE"
   standardAttributeCentroidX,                    // "CENTROID_X"
   standardAttributeCentroidY,                    // "CENTROID_Y"
   standardAttributeCircuitNetName,               // "CKT_NETNAME"
   standardAttributeComponentHeight,              // "COMPHEIGHT"
   standardAttributeComponentOutlineOrientation,  // "ComponentOutlineOrientation"
   standardAttributeCurrent,                      // "CURRENT"
   standardAttributeCurrentVariant,               // "CurrentVariant"
   standardAttributeDataType,                     // "DataType"
   standardAttributeDataLink,                     // "DATALINK"
   standardAttributeDataSource,                   // "DataSource"
   standardAttributeDebugId,                      // "DebugId"
   standardAttributeDerivedFrom,                  // "DERIVED_FROM"
   standardAttributeDescription,                  // "DESCRIPTION"
   standardAttributeDesignedSurface,              // "DESIGNED_SURFACE"
   standardAttributeDeviceToPackagePinMap,        // "DEVICE_TO_PACKAGE_PINMAP"
   standardAttributeDeviceType,                   // "DEVICETYPE"
   standardAttributeDfmActualValue,               // "DFM_ACTUAL_VALUE"
   standardAttributeDfmCheckValue,                // "DFM_CHECK_VALUE"
   standardAttributeDfmFailureRange,              // "DFM_FAILURE_RANGE"
   standardAttributeDfmKeyword,                   // "DFM_KEYWORD"
   standardAttributeDfmLayerStackNumber,          // "DFM_LAYER_STACKNUMBER"
   standardAttributeDfmLayerType,                 // "DFM_LAYER_TYPE"
   standardAttributeDfmPackageFamily,             // "PACKAGE_FAMILY"
   standardAttributeDfmRef1,                      // "DFM_REF1"
   standardAttributeDfmRef2,                      // "DFM_REF2"
   standardAttributeDfmTestName,                  // "DFM_TEST_NAME"
   standardAttributeDfmValue1,                    // "DFM_VALUE1"
   standardAttributeDfmValue2,                    // "DFM_VALUE2"
   standardAttributeDirectionOfTravel,            // "DIRECTION_OF_TRAVEL"
   standardAttributeDxfOutlineType,               // "DFX_OUTLINE_TYPE"
   standardAttributeEcadBoardOnPanel,             // "ECAD_BOARD_ON_PANEL"
   standardAttributeEcadGeometryName,             // "ECAD_GEOMETRY_NAME"
   standardAttributeEcadPin,                      // "ECAD_PIN"
   standardAttributeEcadRefDes,                   // "ECAD_REFDES"
   standardAttributeEducatorSurface,              // "EducatorSurface"
   standardAttributeFiducialPlacedBoth,           // "FIDUCIAL_BOTH"
   standardAttributeGencamType,                   // "GENCAM_TYPE"
   standardAttributeGeomReviewStatus,             // "GeomReviewStatus"
   standardAttributeGridLocation,                 // "GRID_LOCATION"
   standardAttributeHafBoundaryScan,              // "HAF_BOUNDARYSCAN"
   standardAttributeHafComment,                   // "HAF_COMMENT"
   standardAttributeHafComponentHeight,           // "HAF_COMPHEIGHT"
   standardAttributeHafDeviceClass,               // "HAF_DEVICECLASS"
   standardAttributeHafMinusTolerance,            // "HAF_NTOL"
   standardAttributeHafNetPotential,              // "HAF_NETPOTENTIAL"
   standardAttributeHafPackage,                   // "HAF_PACKAGE"
   standardAttributeHafPartNumber,                // "HAF_PARTNUMBER"
   standardAttributeHafPinDirection,              // "HAF_PINDIRECTION"
   standardAttributeHafPinFunction,               // "HAF_PINFUNCTION"
   standardAttributeHafPlusTolerance,             // "HAF_PTOL"
   standardAttributeHafRefName,                   // "HAF_REFNAME"
   standardAttributeHafTest,                      // "HAF_TEST"
   standardAttributeHafValue,                     // "HAF_VALUE"
   standardAttributeIngunDeviceClass,             // "TRI_DEVICECLASS"
   standardAttributeInsertionProcess,             // "INSERTION PROCESS"
   standardAttributeInstanceName,                 // "INSTANCENAME"
   standardAttributeKeepDraw,                     // "KEEPDRAW"
   standardAttributeLayerThickness,               // "LAYER THICKNESS" 
   standardAttributeLoaded,                       // "LOADED"
   standardAttributeMergeToolSurface,             // "MergeToolSurface"
   standardAttributeMinusTolerance,               // "-TOL"
   standardAttributeMiscellaneous,                // "Miscellaneous"
   standardAttributeName,                         // "NAME"
   standardAttributeNetName,                      // "NETNAME"
   standardAttributeNetType,                      // "NET_TYPE"
   standardAttributeNoConnect,                    // "No Connect"
   standardAttributeNoProbe,                      // "No_Probe"
   standardAttributePackageLink,                  // "$$PACKAGE$$"
   standardAttributePadUsage,                     // "PAD_USAGE"
   standardAttributePartNumber,                   // "PARTNUMBER"
   standardAttributePath,                         // "PATH"
   standardAttributePinAccess,                    // "PIN_ACCESS"
   standardAttributePinCount,                     // "PIN_COUNT"
   standardAttributePinFunction,                  // "PINFUNC"
   standardAttributePinLogicName,                 // "PINDESC"
   standardAttributePinNumber,                    // "PINNR"
   standardAttributePinPitch,                     // "PIN_PITCH"
   standardAttributePinsModified,                 // "PINS_MODIFIED"
   standardAttributePirRequired,                  // "PIR_Required"
   standardAttributePlusTolerance,                // "+TOL"
   standardAttributePowerNet,                     // "POWERNET"
   standardAttributeProbePlacement,               // "PROBE_PLACEMENT"
   standardAttributeProbeSize,                    // "PROBE_SIZE" - in current units
   standardAttributeProbeStyle,                   // "PROBE_STYLE"
   standardAttributeRefName,                      // "REFNAME"
   standardAttributeSiemensBoardThickness,        // "SIEMENS_THICKNESS_BOARD"
   standardAttributeSiemensFiducialSymbol,        // "SIEMENS_FIDSYM"
   standardAttributeSiemensPanelThickness,        // "SIEMENS_THICKNESS_PANEL"
   standartAttributeSiemensDialogSetting,         // "SiemensDialogSetting"
   standardAttributeSinglePinNet,                 // "SINGLEPINNET"
   standardAttributeSmd,                          // "SMD"
   standardAttributeSolderMask,                   // "SOLDERMASK"
   standardAttributeSonyAoiSettings,              // "SonyAoiSettings"
   standardAttributeSpeaSubclass,                 // "Spea4040_Subclass"
   standardAttributeStencilGeneratorTempComponent,// "StencilGeneratorTempComponent"
   standardAttributeSubclass,                     // "SUBCLASS"
   standardAttributeTakayaAccess,                 // "TAKAYA_ACCESS"
   standardAttributeTakayaDeviceClass,            // "TAKAYA_DEVICECLASS"
   standardAttributeTakayaDeviceClassSubclass,    // "TAKAYA_DEVICECLASS_SUBCLASS"
   standardAttributeTakayaRefDes,                 // "TAKAYA_REFDES"
   standardAttributeTakayaReferencePointSetting,  // "TakayaReferecePointSetting"
   standardAttributeTargetType,                   // "TARGET_TYPE"
   standardAttributeTechnology,                   // "TECHNOLOGY"
   standardAttributeTeradyne7200Test,             // "TERADYNE_7200_TEST"
   standardAttributeTeradyneAoiDeviceClass,       // "TERADYNEAOI_DEVICECLASS"
   standardAttributeTeradyneAoiPinTest,           // "TERADYNE_AOI_PINTEST"
   standardAttributeTeradyneSubclass,             // "Teradyne1800_Subclass"
   standardAttributeTest,                         // "TEST"
   standardAttributeTestAccess,                   // "TEST_ACCESS"
   standardAttributeTestAccessFail,               // "TEST_ACCESS_FAIL"
   standardAttributeTestAccessFailTop,            // "TEST_ACCESS_FAIL_TOP"
   standardAttributeTestAccessFailBottom,         // "TEST_ACCESS_FAIL_BOTTOM"
   standardAttributeTestConnector,                // "TEST_CONNECTOR"
   standardAttributeTesterInterface,              // "TIN"
   standardAttributeTestIgnoreOutline,            // "TEST_IGNORE_OUTLINE"
   standardAttributeTestNetProbes,                // "TEST_NET_PROBES"
   standardAttributeTestNetStatus,                // "TEST_NET_STATUS"
   standardAttributeTestOffsetBottomX,            // "TEST_OFFSET_BOT_X"
   standardAttributeTestOffsetBottomY,            // "TEST_OFFSET_BOT_Y"
   standardAttributeTestOffsetTopX,               // "TEST_OFFSET_TOP_X"
   standardAttributeTestOffsetTopY,               // "TEST_OFFSET_TOP_Y"
   standardAttributeTestPreference,               // "TEST_PREFERENCE"
   standardAttributeTestProbeFail,                // "TEST_PROBE_FAIL"
   standardAttributeTestResource,                 // "TEST_RESOURCE"
   standardAttributeTestShort,                    // "TEST_SHORT"
   standardAttributeTestStrategy,                 // "TEST_STRATEGY"
   standardAttributeTolerance,                    // "TOLERANCE"
   standardAttributeTopPadstackDesignSurface,     // "TOP_PADSTACK_DESIGN_SURFACE"
   standardAttributeTrRequired,                   // "TR_Required"
   standardAttributeTypeListLink,                 // "$$DEVICE$$"
   standardAttributeUnplaced,                     // "UNPLACED"
   standardAttributeValue,                        // "VALUE"
   standardAttributeViaLayer,                     // "VIALAYER"
   standardAttributeVoltage,                      // "VOLTAGE"
   standardAttributeCBNetname,                    // "CB_NETNAME"
   standardAttributeWire,                         // "WIRE"
   standardAttributeStencilThickness,             // "StencilThickness"
   standardAttributeMax
};

CString StandardAttributeTagToName(StandardAttributeTag standardAttributeTag);

//_____________________________________________________________________________
struct KeyWordStruct
{
private:
   int m_index;
   bool m_inherited;   // if this is true, do the inherit, otherwise do not.
   bool m_hidden;

public:
   CString in;
   CString cc;
   CString out;
   AttribGroupTag group;      // formaly internal (see ATTRIB.H ATTGROUP_xxx)
   ValueTypeTag valueType;  

public:
   KeyWordStruct(int index);
   KeyWordStruct(int index,const CString& keyword,AttribGroupTag group=attribGroupUser,
      ValueTypeTag valueType=valueTypeNone,bool inherited=true,bool hidden=false);
   ~KeyWordStruct();

   int getIndex() const { return m_index; }

   CString getCCKeyword() const { return cc; };
   CString getInKeyword() const { return in; };
   CString getOutKeyword() const { return out; };

   void setCCKeyword(CString ccKw) { cc = ccKw; };
   void setInKeyword(CString inKw) { in = inKw; };
   void setOutKeyword(CString outKw) { out = outKw; };

   AttribGroupTag getGroup() const { return group; };
   void setGroup(AttribGroupTag newGroup) { group = newGroup; }

   ValueTypeTag getValueType() const    { return valueType; }
   void setValueType(ValueTypeTag type) { valueType = type; }

   bool getInherited() const { return m_inherited; }
   void setInherited(bool flag) { m_inherited = flag; }

   bool getHidden() const    { return m_hidden; }
   void setHidden(bool flag) { m_hidden = flag; }

   //void WriteXML(CWriteFormat &writeFormat) const;

private:
   static int m_allocationCount;
};

//_____________________________________________________________________________
class CKeyWordArray
{
public:
   CKeyWordArray();

private:
   CTypedPtrArrayContainer<KeyWordStruct*> m_keywordArray;

   CMapStringToWord m_inMap;
   CMapStringToWord m_ccMap;
   CMapStringToWord m_outMap;

public:
   void empty();

//// Attributes
   int GetSize() const;
   int GetCount() const;
   int Lookup(CString keyword, EKeyWordSection section) const;
   int getKeywordIndex(CString keyword) const;
   KeyWordStruct* getKeywordAt(const CString& keywordName) const;

   bool SetCCKeyword(int index, CString cc);
   bool SetInKeyword(int index, CString in);
   bool SetOutKeyword(int index, CString out);
   bool SetGroup(int index, AttribGroupTag group);
   bool SetInherited(int index, bool flag);
   bool SetHidden(int index, bool flag);
   bool SetValueType(int index, ValueTypeTag type);

// BOOL IsEmpty() const;
// INT_PTR GetUpperBound() const;
   void SetSize(int nNewSize, int nGrowBy = -1)
   { m_keywordArray.SetSize(nNewSize,nGrowBy); }
//
//// Operations
// // Clean up
// void FreeExtra();
// void RemoveAll();
//
// // Accessing elements
// void SetAt(INT_PTR nIndex, void* newElement);
//
// void*& ElementAt(INT_PTR nIndex);
//
// // Direct Access to the element data (may return NULL)
// const void** GetData() const;
// void** GetData();
//
// // Potentially growing the array
// void SetAtGrow(INT_PTR nIndex, KeyWordStruct* newElement);
   int SetAtGrow(const CString& keyword, AttribGroupTag group=attribGroupUser, ValueTypeTag valueType=valueTypeNone, bool inherited=true, bool hidden=false, int nIndex=-1);
   int getDefinedKeywordIndex(const CString& keywordName,ValueTypeTag valueType);
//
// INT_PTR Add(void* newElement);
//
// INT_PTR Append(const CPtrArray& src);
// void Copy(const CPtrArray& src);
//
// // overloaded operator helpers
   const KeyWordStruct* operator[](int nIndex) const
      { return m_keywordArray[nIndex]; }

   KeyWordStruct* GetAt(int nIndex) const
      { return m_keywordArray.GetAt(nIndex); }

   KeyWordStruct* getAt(int nIndex) const;

   //KeyWordStruct*& operator[](INT_PTR nIndex)
   //{ return (KeyWordStruct*&)CTypedPtrArrayContainer<KeyWordStruct*>::operator[](nIndex); }
//
// // Operations that move elements around
// void InsertAt(INT_PTR nIndex, void* newElement, INT_PTR nCount = 1);
//
   void RemoveAt(int nIndex, int nCount = 1);
// void InsertAt(INT_PTR nStartIndex, CPtrArray* pNewArray);

   //void WriteXML(CWriteFormat &writeFormat, CCamCadFileWriteProgress& progress) const;
};

#endif
