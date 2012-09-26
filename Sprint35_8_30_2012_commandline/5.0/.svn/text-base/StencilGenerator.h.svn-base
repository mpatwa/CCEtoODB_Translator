// $Header: /CAMCAD/4.6/StencilGenerator.h 114   4/16/07 8:55p Kurt Van Ness $

/*---------------------------------------------------------------------------------------------------------------------------
The stencil generator is being enhanced to support rule based stencil hole generation for copper polys
inserted at the component level and separate rules for multiple copper pads in a padstack.

To support component level copper, the copper polys (or apertures) will be converted into padstacks
and inserted as mechanical pins.

The support for multiple copper pads will be more complex.
Currently SG attaches attributes to various datas to store the stencil rule for a particular stencil item.
Padstack rules cannot be inserted into padstack geometries because then all pins in all components using that
padstack would have the same rule.
In order to support a different stencil rule for a padstack inserted into different component geometries,
a new insert of a "padstackVessel" geometry is made for each separate padstack geometry used in a component -
the rule attribute for the padstack is attached to this insert. 
The reference name for the insert is set to the padstack geometry name.	
Component pin rules were attached to the padstack insert datas in the component geometry.
In order to support multiple rules for pins with sub pads, a new insert for each subpad instance must be placed
in the component geometry.
To increase the utility of the rule hierarchy machinery, a new insert for each subpad in each padstack must also be placed
in the component geometry.
In order to uniquely identify each subpad in a padstack, an attribute "SubpadNumber" will be attached to each pad insert in the 
padstack geometry.

Example:

In the pads sample there is a large transistor, geometry TO-213AA.
This geometry has been modified, pins 3 and 4 use a new padstack, "newPadstack_25" with 3 subpads.

- - - old structure
PCB
   Top Surface
      Thru-hole Geometries
         TO-213AA
            PADSTACK_24  --- padstackVessel insert
               1         --- pin insert
               2
            PADSTACK_25
               3
            PADSTACK_26
               4

- - - new structure
PCB
   Top Surface
      Thru-hole Geometries
         TO-213AA           --- componentVesselInsert - reference "top"
            PADSTACK_24     --- padstackVessel insert - reference "top.PADSTACK_24"
               1            --- pinVessel insert - reference "top.PADSTACK_24.1.1"
               2            --- pinVessel insert - reference "top.PADSTACK_24.1.2"
            newPadstack_25  --- padstackVessel insert - reference "top.newPadstack_25"
               subPad 1     --- subPadstackVessel insert - reference "top.newPadstack_25.1"
                  3         --- pinVessel insert - reference "top.newPadstack_25.1.3"
                  4         --- pinVessel insert - reference "top.newPadstack_25.1.4"
               subPad 2     --- subPadstackVessel insert - reference "top.newPadstack_25.2"
                  3         --- pinVessel insert - reference "top.newPadstack_25.2.3"
                  4         --- pinVessel insert - reference "top.newPadstack_25.2.4"
               subPad 3     --- subPadstackVessel insert - reference "top.newPadstack_25.3"
                  3         --- pinVessel insert - reference "top.newPadstack_25.3.3"
                  4         --- pinVessel insert - reference "top.newPadstack_25.3.4"
	
___________________________________________________________________________________

padstackVessel
subPadstackVessel
pinVessel	

___________________________________________________________________________________


CStencilPins::addStencilEntities() - update to process new vessels for subPadstacks and pins.

CStencilGenerator::initializeStencilEntities() - 
	restructure component geometries
	move geometry pin rules from padstack insert datas to pinVessel datas
	
CStencilPins::add() - add subPadstack parameter and change pin parameter to pinVessel.	


___________________________________________________________________________________

Excerpt from saved stencil settings - "SavedStencilSettings-SgSubPinTest-1.txt"
cc file - "PadsSample-SgSubPinTest-1.cc"


[StencilGenerationRules]
pcb Inset 0.000000 StencilWebSettings("0.03 In","0.005 In",0,Grid)
padstack TO-213AA.newPadstack_25 Aperture round170 StencilWebSettings("0.03 In","0.005 In",0,Grid)
geometryPin TO-213AA.4 Aperture square100 StencilWebSettings("0.03 In","0.005 In",0,Grid)
___________________________________________________________________________________

   <sideRef>  := ("top" | "bottom")
   <mountRef> := {"via" | "fiducial" | "smd" | "th")
   
   Old Source                        Structure                          Parent                 Description
   ______________________________    _____________________              __________             _______________________________________________________________________________________________
   attributeSourcePcb                BlockStruct(pcbBlock)              FileStruct             Pcb Geometry                                                     
   attributeSourceSurface            LayerStruct                                               ccLayerStencilTop,ccLayerStencilBottom                                                           
   attributeSourceMount              DataStruct.TextStruct              PcbBlock               ccLayerStencilTop text value of ("th","smd"),ccLayerStencilBottom text value of ("th","smd")
                                     BlockStruct($mountVessel$)         PcbBlock               refName of <sideRef> "-" <mountRef>                                                       
   attributeSourceGeometry           BlockStruct(componentBlock)        PcbBlock               insertTypePcbComponent                                                            
   attributeSourcePadstack           BlockStruct($PadStackVessel$)      componentBlock         refName of <padStackGeometryName>                                                                    
   attributeSourceSubPadstack        n/a                                                             
   attributeSourceGeometryPin        DataStruct(padStackBlock)          componentBlock         refName of <pinName>                                                             
   attributeSourceComponent          DataStruct(componentBlock)         PcbBlock               refName of <refDes>                                                  
   attributeSourceComponentPin       BlockStruct($ComponentPinVessel$)  $ComponentPinsVessel$                                                                
   attributeSourceComponentSubPin    n/a                                                                 

   
   New Source                        Structure                                Parent            Description   
   ______________________________    _____________________                    __________        _______________________________________________________________________________________________
   attributeSourcePcb                BlockStruct($PcbVessel$)                 closetBlock       refName of "pcb"                                                    
   attributeSourceSurface            BlockStruct($SurfaceVessel$)             closetBlock       refName of <sideRef>                                                         
   attributeSourceMount              BlockStruct($mountVessel$)               closetBlock       refName of <sideRef> "." <mountRef>
   attributeSourceGeometry           BlockStruct($ComponentGeometryVessel$)   componentBlock    refName of <sideRef> "." <componentGeometryName>                                                         
   attributeSourcePadstack           BlockStruct($PadstackVessel$)            componentBlock    refName of <sideRef> "." <componentGeometryName> "." <padStackGeometryName>
   attributeSourceSubPadstack        BlockStruct($SubPadstackVessel$)         componentBlock    refName of <sideRef> "." <componentGeometryName> "." <padStackGeometryName> "." <subPinName>
   attributeSourceGeometryPin        BlockStruct($SubPinVessel$)              componentBlock    refName of <sideRef> "." <componentGeometryName> "." <padStackGeometryName> "." <subPinName> "." <PinName>
   attributeSourceComponent          BlockStruct($ComponentVessel$)           closetBlock       refName of <sideRef> "." <refDes>
   attributeSourceComponentPin       BlockStruct($ComponentPinVessel$)        closetBlock       refName of <sideRef> "." <refDes> "." <pinName>
   attributeSourceComponentSubPin    BlockStruct($ComponentSubPinVessel$)     closetBlock       refName of <sideRef> "." <refDes> "." <pinName> "." <subPinName>
   
   
   Old Rule                          Designator Prefix         Designator Suffix       
   ________________                  ____________________      _____________________
   attributeSourcePcb                empty                     empty
   attributeSourceSurface            <sideRef>                 empty
   attributeSourceMount              <sideRef>                 <mountRef>
   attributeSourceGeometry           <componentGeometryName>
   attributeSourcePadstack           <componentGeometryName>   <padStackGeometryName>
   attributeSourceSubPadstack        n/a
   attributeSourceGeometryPin        <componentGeometryName>   <pinName>
   attributeSourceComponent          <refDes>
   attributeSourceComponentPin       <refDes>                  <pinName>
   attributeSourceComponentSubPin    n/a

   
   New Rule                          Designator 1              Designator 2               Designator 3            Designator 4   Designator 5   Designator Prefix          Designator Suffix
   ________________                  ____________________      _____________________      ____________________    ____________   ____________   ___________________        _______________________________________
   attributeSourcePcb                "pcb"                                                                                                                                 "pcb"
   attributeSourceSurface            <sideRef>                                                                                                  <sideRef>
   attributeSourceMount              <sideRef>                 <mountRef>                                                                       <sideRef>                  <mountRef>
   attributeSourceGeometry           <sideRef>                 <componentGeometryName>                                                          <componentGeometryName>    <sideRef>
   attributeSourcePadstack           <sideRef>                 <componentGeometryName>    <padStackGeometryName>                                <componentGeometryName>    <padStackGeometryName>.<sideRef>
   attributeSourceSubPadstack        <sideRef>                 <componentGeometryName>    <padStackGeometryName>  <subPinName>                  <componentGeometryName>    <padStackGeometryName>.<subPinName>.<sideRef>
   attributeSourceGeometryPin        <sideRef>                 <componentGeometryName>    <padStackGeometryName>  <subPinName>   <pinName>      <componentGeometryName>    <padStackGeometryName>.<subPinName>.<pinName>.<sideRef>
   attributeSourceComponent          <sideRef>                 <refDes>                                                                         <refDes>                   <sideRef>
   attributeSourceComponentPin       <sideRef>                 <refDes>                   <pinName>                                             <refDes>                   <pinName>.<sideRef>
   attributeSourceComponentSubPin    <sideRef>                 <refDes>                   <pinName>               <subPinName>                  <refDes>                   <pinName>.<subPinName>.<sideRef>

   
   Old Rule                          Designator Prefix        Designator Prefix          Designator Suffix          Designator Suffix
   ________________                  ____________________     ___________________        _____________________      _______________________________________
   attributeSourcePcb                empty                                               empty                      "pcb"
   attributeSourceSurface            <sideRef>                <sideRef>                  empty                      
   attributeSourceMount              <sideRef>                <sideRef>                  <mountRef>                 <mountRef>
   attributeSourceGeometry           <componentGeometryName>  <componentGeometryName>                               <sideRef>
   attributeSourcePadstack           <componentGeometryName>  <componentGeometryName>    <padStackGeometryName>     <padStackGeometryName>.<sideRef>
   attributeSourceSubPadstack        n/a                      <componentGeometryName>    n/a                        <padStackGeometryName>.<subPinName>.<sideRef>
   attributeSourceGeometryPin        <componentGeometryName>  <componentGeometryName>    <pinName>                  <padStackGeometryName>.<subPinName>.<pinName>.<sideRef>
   attributeSourceComponent          <refDes>                 <refDes>                                              <sideRef>
   attributeSourceComponentPin       <refDes>                 <refDes>                   <pinName>                  <pinName>.<sideRef>
   attributeSourceComponentSubPin    n/a                      <refDes>                   n/a                        <pinName>.<subPinName>.<sideRef>


20080110 - knv  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
Problem:
   Many readers create padstack geometries with algorithmically generated names.
   These algorithms do not necessarily generate the same padstack names for similar cad input files.
   This fact interferes with using rules contained in a stencil rule database where the rule is associated with a padstack geometry name
   since the database padstack name may differ from the imported design's padstack name.
   One solution to this dilemma is to identify padstacks based on a pin name instead of the padstack geometry name.
   The rational is that pin names are almost universally carried thru from the cad data to the cc data and the padstack used for a particular
   pin name can be readily ascertained by examining the component geometry.

Implementation:
   Although the cc file data structure allows padstack inserts in a component geometry to have the same refname (pin name),
   semantically this does not make sense - one reason being that there would be no way to differentiate the pins in a net list.
   Therefore, in the following discussion, it will be assumed that no component geometries have duplicate pin names.
   With each pin having a unique name, there is a many to one relationship between pin names and padstack names, 
   i.e. a pin can be associated with only one padstack, but a padstack can be associated with one or more pins.
   Therefore, a pin name uniquely identifies the padstack used.
   Instead of using the padstack geometry name as one of the stencil rule designators, one of the pin names can be used instead.
   Since it doesn't matter which of the "many" pin names is used as the identifier, the "first" pin name will be.
   This first pin will be determined using the well defined "sortable pin number" algorithm to sort the pin names associated with a padstack
   and using the pin name lowest in the sort order.

   New Source                        Structure                                Parent            Description   
   ______________________________    _____________________                    __________        _______________________________________________________________________________________________
   attributeSourcePcb                BlockStruct($PcbVessel$)                 closetBlock       refName of "pcb"                                                    
   attributeSourceSurface            BlockStruct($SurfaceVessel$)             closetBlock       refName of <sideRef>                                                         
   attributeSourceMount              BlockStruct($mountVessel$)               closetBlock       refName of <sideRef> "." <mountRef>
   attributeSourceGeometry           BlockStruct($ComponentGeometryVessel$)   componentBlock    refName of <sideRef> "." <componentGeometryName>                                                         
   attributeSourcePadstack           BlockStruct($PadstackVessel$)            componentBlock    refName of <sideRef> "." <componentGeometryName> "." <firstPinName>
   attributeSourceSubPadstack        BlockStruct($SubPadstackVessel$)         componentBlock    refName of <sideRef> "." <componentGeometryName> "." <firstPinName> "." <subPinName>
   attributeSourceGeometryPin        BlockStruct($SubPinVessel$)              componentBlock    refName of <sideRef> "." <componentGeometryName> "." <PinName> "." <subPinName>
   attributeSourceComponent          BlockStruct($ComponentVessel$)           closetBlock       refName of <sideRef> "." <refDes>
   attributeSourceComponentPin       BlockStruct($ComponentPinVessel$)        closetBlock       refName of <sideRef> "." <refDes> "." <pinName>
   attributeSourceComponentSubPin    BlockStruct($ComponentSubPinVessel$)     closetBlock       refName of <sideRef> "." <refDes> "." <pinName> "." <subPinName>

   
   New Rule                          Designator 1              Designator 2               Designator 3            Designator 4   Designator 5
   ________________                  ____________________      _____________________      ____________________    ____________   ____________
   attributeSourcePcb                "pcb"                                                                                                   
   attributeSourceSurface            <sideRef>                                                                                               
   attributeSourceMount              <sideRef>                 <mountRef>                                                                    
   attributeSourceGeometry           <sideRef>                 <componentGeometryName>                                                       
   attributeSourcePadstack           <sideRef>                 <componentGeometryName>    <firstPinName>                             
   attributeSourceSubPadstack        <sideRef>                 <componentGeometryName>    <firstPinName>          <subPinName>               
   attributeSourceGeometryPin        <sideRef>                 <componentGeometryName>    <pinName>               <subPinName>
   attributeSourceComponent          <sideRef>                 <refDes>                                                                      
   attributeSourceComponentPin       <sideRef>                 <refDes>                   <pinName>                                          
   attributeSourceComponentSubPin    <sideRef>                 <refDes>                   <pinName>               <subPinName>               

   
   Old Rule                          Designator Prefix        Designator Prefix          Designator Suffix          Designator Suffix
   ________________                  ____________________     ___________________        _____________________      _______________________________________
   attributeSourcePcb                empty                                               empty                      "pcb"
   attributeSourceSurface            <sideRef>                <sideRef>                  empty                      
   attributeSourceMount              <sideRef>                <sideRef>                  <mountRef>                 <mountRef>
   attributeSourceGeometry           <componentGeometryName>  <componentGeometryName>                               <sideRef>
   attributeSourcePadstack           <componentGeometryName>  <componentGeometryName>    <padStackGeometryName>     <firstPinName>.<sideRef>
   attributeSourceSubPadstack        n/a                      <componentGeometryName>    n/a                        <firstPinName>.<subPinName>.<sideRef>
   attributeSourceGeometryPin        <componentGeometryName>  <componentGeometryName>    <pinName>                  <pinName>.<subPinName>.<sideRef>
   attributeSourceComponent          <refDes>                 <refDes>                                              <sideRef>
   attributeSourceComponentPin       <refDes>                 <refDes>                   <pinName>                  <pinName>.<sideRef>
   attributeSourceComponentSubPin    n/a                      <refDes>                   n/a                        <pinName>.<subPinName>.<sideRef>




---------------------------------------------------------------------------------------------------------------------------*/

#if !defined(__StencilGenerator_h__)
#define __StencilGenerator_h__

#pragma once

#include "CamCadDatabase.h"
#include "StencilUi.h"

#define QtopPasteInHole           "topPasteInHole"
#define QbottomPasteInHole        "bottomPasteInHole"
#define QtopPasteInVia            "topPasteInVia"
#define QbottomPasteInVia         "bottomPasteInVia"
#define QtopStencilThickness      "topStencilThickness"
#define QbottomStencilThickness   "bottomStencilThickness"
#define QtopMaxFeatureSize        "topMaxFeatureSize"
#define QbottomMaxFeatureSize     "bottomMaxFeatureSize"
#define QtopWebSize               "topWebSize"
#define QbottomWebSize            "bottomWebSize"
#define QtopStencilSourceLayer    "topStencilSourceLayer"
#define QbottomStencilSourceLayer "bottomStencilSourceLayer"
#define QexteriorCornerRadius     "exteriorCornerRadius"
#define QpropagateRules           "propagateRules"
#define QuseMountTechAttrib       "useMountTechAttrib"
#define QmountTechAttribName      "mountTechAttribName"
#define QmountTechAttribSMDValue  "mountTechAttribSMDValue"

#define QminHoleAreaToWallAreaRatio "minHoleAreaToWallAreaRatio"
#define QminFeature                 "minFeature"
#define QmaxFeature                 "maxFeature"
#define QminInset                   "minInset"


class CHtmlFileWriteFormat;
class CStencilViolation;
class CStencilPin;
class CStdioFileWriteFormat;
class CStandardAperture;
class CStencilSubPin;

enum AttributeSourceTag
{
   attributeSourceGeometryPin,
   attributeSourceComponentPin,
   attributeSourceComponentSubPin,
   attributeSourceComponent,
   attributeSourcePadstack,
   attributeSourceSubPadstack,
   attributeSourceGeometry,
   attributeSourceMount,
 //attributeSourceVia,
   attributeSourceSurface,
   attributeSourcePcb,
   attributeSourceNone,
   attributeSourceUndefined
};

CString attributeSourceToString(AttributeSourceTag tagValue);
AttributeSourceTag stringToAttributeSourceTag(const CString& tagValue);
AttributeSourceTag parentAttributeSource(AttributeSourceTag tagValue);

enum StencilRuleTag
{
   stencilRuleNone,
   stencilRuleArea,
   stencilRuleInset,
   stencilRuleAperture,
   stencilRuleApertureGeometry,
   stencilRuleCopperSourceLayer,
   stencilRuleStencilSourceLayer,
   stencilRuleInheritFromParent,
   stencilRulePassthru,
   stencilRuleUndefined,
   stencilRuleUnknown
};

StencilRuleTag ruleStringToStencilRuleTag(const CString& ruleString);
CString stencilRuleToRuleString(StencilRuleTag tagValue);
CString stencilRuleTagToRuleString(StencilRuleTag tagValue);

//enum StencilObjectTag
//{
//   stencilObjectPcb,
//   stencilObjectTopSurface,
//   stencilObjectBottomSurface,
//   stencilObjectTopSmdMount,
//   stencilObjectBottomSmdMount,
//   stencilObjectTopThMount,
//   stencilObjectBottomThMount,
//   stencilObjectGeometry,
//   stencilObjectPadstack,
//   stencilObjectGeometryPin,
//   stencilObjectComponent,
//   stencilObjectComponentPin,
//   stencilObjectUndefined
//};

enum StencilAttributeTag
{
// stencilAttributeNone,
// stencilAttributeArea,
// stencilAttributeInset,
// stencilAttributeAperture,
// stencilAttributeApertureGeometry,
// stencilAttributeCopperSourceLayer,
// stencilAttributeStencilSourceLayer,
// stencilAttributeApertureModifier,
// stencilAttributeGroup,
   stencilAttributeTopThickness,
   stencilAttributeBottomThickness,
   stencilAttributeTopMaxFeature,
   stencilAttributeBottomMaxFeature,
   stencilAttributeTopWebWidth,
   stencilAttributeBottomWebWidth,
   stencilAttributeExteriorCornerRadius,
   stencilAttributeTopPasteInHole,
   stencilAttributeBottomPasteInHole,
   stencilAttributeTopPasteInVia,
   stencilAttributeBottomPasteInVia,
   stencilAttributeTopStencilSourceLayer,
   stencilAttributeBottomStencilSourceLayer,
   stencilAttributePropagateRules,
   stencilAttributeUseMountTechAttrib,
   stencilAttributeMountTechAttribName,
   stencilAttributeMountTechAttribSMDValue,
   stencilAttributeUndefined
};

//CString stencilAttributeToString(StencilAttributeTag tagValue);
//CString stencilAttributeToRuleString(StencilAttributeTag tagValue);
StencilAttributeTag intToStencilAttributeTag(int tagValue);
//StencilAttributeTag ruleStringToStencilAttributeTag(const CString& ruleString);
//StencilAttributeTag attributeNameToStencilAttributeTag(const CString& attributeName);
//StencilRuleTag stencilAttributeTagToStencilRule(StencilAttributeTag tagValue);
StencilRuleTag intToStencilRuleTag(int tagValue);
//StencilAttributeTag stencilRuleTagToStencilAttribute(StencilRuleTag tagValue);

enum StencilViolationTag
{
   stencilViolationHoleAreaToWallAreaRatio,
   stencilViolationMinimumFeature,
   stencilViolationMaximumFeature,
   stencilViolationSlump,
   stencilViolationSplitPad,
   stencilViolationUndefined
};

enum StencilViolationStatusTag
{
   violationStatusOpen,
   violationStatusClosed,
   violationStatusInfo,
   violationStatusUndefined
};

enum StencilRuleModifierTag
{
   stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline=0,
   stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline,
   stencilRuleModifierAlignApertureMinorAxisTowardComponentCenterline,
   stencilRuleModifierAlignApertureMinorAxisAwayFromComponentCenterline,
   stencilRuleModifierAlignApertureMajorAxisParallelToComponentXAxis,
   stencilRuleModifierAlignApertureMajorAxisParallelToComponentYAxis,
   stencilRuleModifierAlignApertureMinorAxisParallelToComponentXAxis,
   stencilRuleModifierAlignApertureMinorAxisParallelToComponentYAxis,
   stencilRuleModifierOrientSymmetryAxis,
   stencilRuleModifierFlipSymmetryAxis,
   stencilRuleModifierOffsetRelativeToPadstack,
   stencilRuleModifierWebSettings,
   stencilRuleModifierExteriorCornerRadius,
   stencilRuleModifierUndefined,

   stencilRuleModifierFirst=0,
   stencilRuleModifierLast=(stencilRuleModifierUndefined - 1)
};

CString stencilRuleModifierToString(StencilRuleModifierTag tagValue);
CString stencilRuleModifierToDescription(StencilRuleModifierTag tagValue);
StencilRuleModifierTag stencilRuleModifierDescriptionToStencilRuleModifierTag(const CString& tagValue);
StencilRuleModifierTag stencilRuleModifierToStencilRuleModifierTag(const CString& tagValue);
StencilRuleModifierTag stringToStencilRuleModifierTag(const CString& tagValue);
StencilRuleModifierTag intToStencilRuleModifierTag(int tagValue);

bool ruleAllowsModifier(StencilRuleTag stencilRule,StencilRuleModifierTag ruleModifier);

enum StencilOrientationModifierTag
{
   orientationTowardCenter,
   orientationAwayFromCenter,
   orientationNormal,
   orientationUndefined,
};

CString stencilOrientationModifierToString(StencilOrientationModifierTag tagValue);
StencilOrientationModifierTag stringToStencilOrientationModifierTag(const CString& tagValue);

enum StencilSplitTypeTag
{
   splitTypeGrid,
   splitTypeStripe,
   splitTypeUndefined,
};

CString stencilSplitTypeToString(StencilSplitTypeTag tagValue);
StencilSplitTypeTag stringToSplitTypeTag(const CString& tagValue);

enum StencilRuleStatusTag
{
   ruleStatusUndefined,
   ruleStatusOk,
   ruleStatusNoSelectedPin,
   ruleStatusInvalid,
   ruleStatusInsetError,
};

CString getStencilRuleString(CCamCadDatabase& camCadDatabase,CComponentPin& componentPin);

//_____________________________________________________________________________                     
//class CMappedSettingsWriteFormat : public CStdioFileWriteFormat
//{
//   //DECLARE_DYNAMIC(CStdioFileWriteFormat);
//   
//private:
//   CStencilSettingEntryMap& m_entryMap;
//   
//public:
//   CMappedSettingsWriteFormat(CStencilSettingEntryMap& entryMap,int bufSize = 160);
//   //virtual ~CMappedSettingsWriteFormat();
//
//   //virtual bool open(const CString& filePath,CExtFileException* exception=NULL);
//   //virtual void close();
//   //virtual void flush();
//
//   //virtual CString getFilePath();
//   virtual void write(const char* string);
//};

//_____________________________________________________________________________
//class CKeywordAttributePair
//{
//private:
//   int m_keywordIndex;
//   Attrib* m_attribute;
//
//public:
//   CKeywordAttributePair(int keywordIndex,Attrib* attribute);
//
//   int getKeywordIndex()  const { return m_keywordIndex; }
//   Attrib* getAttribute() const { return m_attribute; }
//};

//_____________________________________________________________________________
//class CKeywordAttributePairs
//{
//private:
//   CTypedPtrArrayContainer<CKeywordAttributePair*> m_keywordAttributePairs;
//
//public:
//   CKeywordAttributePairs();
//   void empty();
//
//   void add(int keywordIndex,Attrib* attribute);
//   int getCount() const;
//   CKeywordAttributePair& getAt(int index) const;
//   CString getPairsString(CCamCadDatabase& camCadDatabase) const;
//};

//_____________________________________________________________________________
class CCompositePad : public CObject
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CDataList m_pads;
   DataStruct* m_pad;
   //double m_rotationModifierRadians;

public:
   CCompositePad(CCamCadDatabase& camCadDatabase);
   virtual ~CCompositePad();

   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   CCamCadData& getCamCadData() { return m_camCadDatabase.getCamCadData(); }

   void addPad(DataStruct* pad);
   void empty();

   int getCount();
   bool isComplex();
   bool isSimple();

   BlockStruct* getNormalizedAperture(BlockStruct& aperture);
   DataStruct* getNormalizedPad(DataStruct& pad);
   DataStruct* getPad();
   DataStruct* getPad(BlockStruct& padStackGeometry,CDataList& padList);
   BlockStruct* getPadGeometry();
   //void setPadGeometry(BlockStruct* aperture);
   //void setPad(CStandardAperture& standardAperture,bool overridePadGeometryRotation,double padGeometryRotationRadians);
   bool flattenPadGeometry(CDataList& parentDataList,CPolyList& polyList,BlockStruct& padGeometry,CTMatrix matrix,
      double exteriorCornerRadius,double inset);
   bool insetPadPerimeter(double inset,double exteriorCornerRadius,CTMatrix* padMatrix=NULL);
   void scalePadArea(double scaleFactor,double exteriorCornerRadius);

private:
   void setPad(DataStruct* pad);
};

//_____________________________________________________________________________
//class CCompositePads : public CObject
//{
//private:
//   CDataList m_pads;
//
//public:
//   DataStruct* getPad(CCompositePad& compositePad);
//};

//_____________________________________________________________________________
class CStencilGenerationParameters : public CObject
{
private:
   CCamCadDatabase& m_camCadDatabase;

   bool m_topPasteInHole;
   bool m_bottomPasteInHole;
   bool m_topPasteInVia;
   bool m_bottomPasteInVia;
   double m_topStencilThickness;
   double m_bottomStencilThickness;
   double m_topMaxFeatureSize;
   double m_bottomMaxFeatureSize;
   double m_topWebSize;
   double m_bottomWebSize;
   double m_exteriorCornerRadius;
   CString m_topStencilSourceLayerName;
   CString m_bottomStencilSourceLayerName;
   bool m_propagateRulesSideToSide; // false gives original behavior of independant side rules
   bool m_useMountTechAttrib; // false gives original behavior of internal determination based on padstacks
   CString m_mountTechAttribName; // attrib name to use, "TECHNOLOGY" is camcad internal standard name
   CString m_mountTechAttribSMDValue; // value that means attrib is SMD, all other values mean THRU

   //static int m_stencilTopPasteInHoleKeywordIndex;
   //static int m_stencilBottomPasteInHoleKeywordIndex;
   //static int m_stencilTopThicknessKeywordIndex;
   //static int m_stencilBottomThicknessKeywordIndex;
   //static int m_stencilTopMaxFeatureKeywordIndex;
   //static int m_stencilBottomMaxFeatureKeywordIndex;
   //static int m_stencilTopWebWidthKeywordIndex;
   //static int m_stencilBottomWebWidthKeywordIndex;

public:
   CStencilGenerationParameters(CCamCadDatabase& m_camCadDatabase);
   CStencilGenerationParameters(const CStencilGenerationParameters& other);
   CStencilGenerationParameters& operator=(const CStencilGenerationParameters& other);

   bool allParametersEqual(const CStencilGenerationParameters& other);
   bool smdParametersEqual(const CStencilGenerationParameters& other);

   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }

   bool   getTopPasteInHole()         { return m_topPasteInHole; }
   bool   getBottomPasteInHole()      { return m_bottomPasteInHole; }
   bool   getTopPasteInVia()          { return m_topPasteInVia; }
   bool   getBottomPasteInVia()       { return m_bottomPasteInVia; }
   double getTopStencilThickness()    { return m_topStencilThickness; }
   double getBottomStencilThickness() { return m_bottomStencilThickness; }

   double getTopMaxFeatureSize()          const { return m_topMaxFeatureSize; }
   double getBottomMaxFeatureSize()       const { return m_bottomMaxFeatureSize; }
   double getMaxFeatureSize(bool topFlag) const { return topFlag ? m_topMaxFeatureSize : m_bottomMaxFeatureSize; }

   double getTopWebSize()          const { return m_topWebSize; }
   double getBottomWebSize()       const { return m_bottomWebSize; }
   double getWebSize(bool topFlag) const { return topFlag ? m_topWebSize : m_bottomWebSize; }

   bool getPropagateRulesSideToSide()           { return true; } ///deadcode m_propagateRulesSideToSide; }

   bool getUseMountTechAttrib()           { return m_useMountTechAttrib; }
   CString getMountTechAttribName()       { return m_mountTechAttribName; }
   CString getMountTechAttribSMDValue()   { return m_mountTechAttribSMDValue; }

   double getExteriorCornerRadius()   { return m_exteriorCornerRadius; }
   CString getTopStencilSourceLayerName()     { return m_topStencilSourceLayerName; }
   CString getBottomStencilSourceLayerName()  { return m_bottomStencilSourceLayerName; }
   void initUndefinedStencilSourceLayerNames();

   void setTopPasteInHole(bool flag)                { m_topPasteInHole         = flag; }
   void setBottomPasteInHole(bool flag)             { m_bottomPasteInHole      = flag; }
   void setTopPasteInVia(bool flag)                 { m_topPasteInVia          = flag; }
   void setBottomPasteInVia(bool flag)              { m_bottomPasteInVia       = flag; }
   void setTopStencilThickness(double parameter)    { m_topStencilThickness    = parameter; }
   void setBottomStencilThickness(double parameter) { m_bottomStencilThickness = parameter; }
   void setTopMaxFeatureSize(double parameter)      { m_topMaxFeatureSize      = parameter; }
   void setBottomMaxFeatureSize(double parameter)   { m_bottomMaxFeatureSize   = parameter; }
   void setTopWebSize(double parameter)             { m_topWebSize             = parameter; }
   void setBottomWebSize(double parameter)          { m_bottomWebSize          = parameter; }
   void setExteriorCornerRadius(double parameter)   { m_exteriorCornerRadius   = parameter; }
   void setTopStencilSourceLayerName(const CString& layerName)     { m_topStencilSourceLayerName    = layerName; }
   void setBottomStencilSourceLayerName(const CString& layerName)  { m_bottomStencilSourceLayerName = layerName; }
   void setPropagateRulesSideToSide(bool val)       { m_propagateRulesSideToSide  = val; }
   void setUseMountTechAttrib(bool val)             { m_useMountTechAttrib      = val; }
   void setMountTechAttribName(CString val)         { m_mountTechAttribName     = val; }
   void setMountTechAttribSMDValue(CString val)     { m_mountTechAttribSMDValue = val; }

   void setToDefaults();
   bool readAttributes(int fileNumber);
   bool writeAttributes(int fileNumber);
   bool readStencilSettings(CStringArray& params);
   bool readStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase);
   bool writeStencilSettings(CWriteFormat& settingsFile);
   bool writeStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase);
   void clearStencilSettings(FileStruct* fileStruct);

   //static void registerStencilGenerationKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog);
   //static int getStencilTopThicknessKeywordIndex()        { return m_stencilTopThicknessKeywordIndex; }
   //static int getStencilBottomThicknessKeywordIndex()     { return m_stencilBottomThicknessKeywordIndex; }
   //static int getStencilTopMaxFeatureKeywordIndex()       { return m_stencilTopMaxFeatureKeywordIndex; }
   //static int getStencilBottomMaxFeatureKeywordIndex()    { return m_stencilBottomMaxFeatureKeywordIndex; }
   //static int getStencilTopWebWidthKeywordIndex()         { return m_stencilTopWebWidthKeywordIndex; }
   //static int getStencilBottomWebWidthKeywordIndex()      { return m_stencilBottomWebWidthKeywordIndex; }
   //static int getStencilTopPasteInHoleKeywordIndex()      { return m_stencilTopPasteInHoleKeywordIndex; }
   //static int getStencilBottomPasteInHoleKeywordIndex()   { return m_stencilBottomPasteInHoleKeywordIndex; }
};

//_____________________________________________________________________________
class CStencilValidationParameters : public CObject
{
private:
   //CStencilUi& m_stencilUi;
   CCamCadDatabase& m_camCadDatabase;

   double m_minHoleAreaToWallAreaRatio;
   double m_minFeature;
   double m_maxFeature;
 //double m_splitFeatureWebWidth;
   double m_minInset;

   int m_violationCount;

   static int m_minHoleAreaToWallAreaRatioKeywordIndex;
   static int m_minFeatureKeywordIndex;
   static int m_maxFeatureKeywordIndex;
 //static int m_splitFeatureWebWidthKeywordIndex;
   static int m_minInsetKeywordIndex;

public:
   static void resetKeywordIndices();

public:
   CStencilValidationParameters(CCamCadDatabase& m_camCadDatabase);
   CStencilValidationParameters(const CStencilValidationParameters& other);
   CStencilValidationParameters& operator=(const CStencilValidationParameters& other);

   bool allParametersEqual(const CStencilValidationParameters& other);

   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }

   double getMinHoleAreaToWallAreaRatio() { return m_minHoleAreaToWallAreaRatio; }
   double getMinFeature()                 { return m_minFeature; }
   double getMaxFeature()                 { return m_maxFeature; }
 //double getSplitFeatureWebWidth()       { return m_splitFeatureWebWidth; }
   double getMinInset()                   { return m_minInset; }

   void setMinHoleAreaToWallAreaRatio(double parameter) { m_minHoleAreaToWallAreaRatio = parameter; }
   void setMinFeature(double parameter)                 { m_minFeature = parameter; }
   void setMaxFeature(double parameter)                 { m_maxFeature = parameter; }
 //void setSplitFeatureWebWidth(double parameter)       { m_splitFeatureWebWidth = parameter; }
   void setMinInset(double parameter)                   { m_minInset = parameter; }

   void setToDefaults();
   bool readAttributes(int fileNumber);
   bool writeAttributes(int fileNumber);
   bool readStencilSettings(CStringArray& params);
   bool readStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase);
   bool writeStencilSettings(CStdioFileWriteFormat& settingsFile);
   bool writeStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase);
   void clearStencilSettings(FileStruct* fileStruct);

   void setViolationCount(int count = -1) { m_violationCount = count; }
   int getViolationCount() { return m_violationCount; }
   int incrementViolationCount() { return m_violationCount++; }

   static void registerStencilValidationKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog);
};

//_____________________________________________________________________________
class CStencilRuleModifierValues
{
private:
   CStringArray m_modifierValues;

public:
   void empty();
   void copy(const CStencilRuleModifierValues& other);
   int getCount() const;
   CString getAt(int index) const;
   void setAt(int index,const CString& value);
   CStringArray& getStringArray();

   void add(const CString& value);
   void add(CString value,PageUnitsTag pageUnits);
};

//_____________________________________________________________________________
class CStencilRuleModifier
{
private:
   StencilRuleModifierTag m_modifier;
   CStencilRuleModifierValues m_modifierValues;

public:
   CStencilRuleModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues);
   CStencilRuleModifier(const CStencilRuleModifier& other);

   CStencilRuleModifier& operator=(const CStencilRuleModifier& other);

   StencilRuleModifierTag getModifier() const { return m_modifier; }
   void setModifier(StencilRuleModifierTag modifier) { m_modifier = modifier; }

   const CStencilRuleModifierValues& getModifierValues() const { return m_modifierValues; }
   void setModifierValues(const CStencilRuleModifierValues& modifierValues) { m_modifierValues.copy(modifierValues); }

   CString getModifierString() const;
   CString getValueAt(int index) const;
   double getUnitValueAt(int index,PageUnitsTag pageUnits,double defaultValue) const;

   void updateModifier(PageUnitsTag pageUnits);

private:
   void updateModifierValue(int index,PageUnitsTag pageUnits);
};

//_____________________________________________________________________________
class CStencilRuleModifiers : public CTypedPtrListContainer<CStencilRuleModifier*>
{
public:
   void setModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues);
   //void setModifier(StencilRuleModifierTag modifier);
   //void setModifierValues(const CStringArray& modifierValues);
   void addModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues);
   bool hasModifier(StencilRuleModifierTag modifier);
   CStencilRuleModifier* getModifier(StencilRuleModifierTag modifier) const;
   void updateModifiers(PageUnitsTag pageUnits);
};

//_____________________________________________________________________________
class CStencilRule
{
private:
   StencilRuleTag m_rule;
   CString m_ruleValue;
   CString m_comment;

   CStencilRuleModifiers m_modifiers;

public:
   CStencilRule();
   CStencilRule(const CStencilRule& other);
   //CStencilRule(const CString& ruleString);

   CStencilRule& operator=(const CStencilRule& other);

   StencilRuleTag getRule() const;
   void setRule(StencilRuleTag rule);

   CString getRuleValue() const;
   void setRuleValue(const CString& ruleValue);

   CString getComment() const;
   void setComment(const CString& comment);

   StencilRuleModifierTag getModifier() const;
   void setModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues);

   void getModifierValues(CStencilRuleModifierValues& modifierValues) const;
   CString getModifiersString() const;

   CString getEntityDesignator() const;

   bool hasModifiers() const;
   bool isValid() const;

   bool setRuleString(const CString& ruleString);
   bool setRuleFromString(const CString& ruleString);
   bool setRuleFromParameters(CStringArray& params,int startIndex);
   CString getRuleString(PageUnitsTag pageUnits=pageUnitsUndefined) const;

   void addModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues);
   void addModifier(const CString& modifierString);
   void addModifiers(const CStencilRule& other);
   bool hasModifier(StencilRuleModifierTag modifier);
   CStencilRuleModifier* getModifier(StencilRuleModifierTag modifier) const { return m_modifiers.getModifier(modifier); }
   void updateModifiers(PageUnitsTag pageUnits);

public:
   static StencilRuleModifierTag parseStencilRuleModifierAttribute(const CString& modifierString,CStencilRuleModifierValues& parameters);
   static CString fixModifiers(const CString& line);
};

//_____________________________________________________________________________
class CStencilEntityRule : public CStencilRule
{
private:
   AttributeSourceTag m_entity;
   CString m_entityDesignator1;
   CString m_entityDesignator2;
   CString m_entityDesignator3;
   CString m_entityDesignator4;
   CString m_entityDesignator5;

public:
   CStencilEntityRule();
   CStencilEntityRule(const CStencilEntityRule& other);
   CStencilEntityRule(const CStencilRule& other);
   CStencilEntityRule(const CString& ruleString);

   CStencilEntityRule& operator=(const CStencilEntityRule& other);

   void setEntity(AttributeSourceTag entity);
   void setEntity(AttributeSourceTag entity,const CString& entityDesignator1);
   void setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2);
   void setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2,const CString& entityDesignator3);
   void setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2,const CString& entityDesignator3,
                                            const CString& entityDesignator4);
   void setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2,const CString& entityDesignator3,
                                            const CString& entityDesignator4,const CString& entityDesignator5);

   AttributeSourceTag getEntity() const;
   CString getEntityString() const;

   CString getEntityDesignator() const;
   CString getEntityDesignator1() const;
   CString getEntityDesignator2() const;
   CString getEntityDesignator3() const;
   CString getEntityDesignator4() const;
   CString getEntityDesignator5() const;

   bool setRuleString(const CString& ruleString);
   CString getRuleString(PageUnitsTag pageUnits=pageUnitsUndefined) const;

public:
   void setEntityDesignator1(const CString& designator);
   void setEntityDesignator2(const CString& designator);
   void setEntityDesignator3(const CString& designator);
   void setEntityDesignator4(const CString& designator);
   void setEntityDesignator5(const CString& designator);

};

//_____________________________________________________________________________
class CStencilEntityRuleArray : public CTypedPtrArrayContainer<CStencilEntityRule*>
{
public:
   CStencilEntityRule* FindEntityRule(CStencilEntityRule* rule); // Finds rule with matching entity, not necessarily matching rule
};

//_____________________________________________________________________________
class CStencilRuleAttributes
{
private:
   int m_id;
   CCamCadDatabase* m_camCadDatabase;
   CAttributes* m_attributeMap;
   CAttributes* m_allocatedAttributeMap;
   CStencilRule m_stencilRule;

private:
   static int m_nextId;

   static int m_stencilPadNoneKeywordIndex;
   static int m_stencilPadAreaKeywordIndex;
   static int m_stencilPadInsetKeywordIndex;
   static int m_stencilPadApertureKeywordIndex;
   static int m_stencilPadApertureGeometryKeywordIndex;
   static int m_stencilPadStencilSourceLayerKeywordIndex;
   static int m_stencilPadPassthruKeywordIndex;
   static int m_stencilPadCopperSourceLayerKeywordIndex;
   static int m_stencilPadInheritFromParentKeywordIndex;
 //static int m_stencilPadGroupKeywordIndex;
   static int m_stencilPadUndefinedKeywordIndex;
   static int m_stencilPadApertureModifierKeywordIndex;
   static int m_stencilPadCommentKeywordIndex;

public:
   CStencilRuleAttributes(CCamCadDatabase& camCadDatabase,CAttributes& attributeMap);
   CStencilRuleAttributes(CCamCadDatabase& camCadDatabase,CAttributes* attributeMap);
   CStencilRuleAttributes(const CStencilRuleAttributes& other);
   CStencilRuleAttributes& operator=(const CStencilRuleAttributes& other);
   ~CStencilRuleAttributes();

   void empty();
   bool isValid();

   bool setRule(const CStencilRule& stencilRule);
   const CStencilRule& getRule() const;
   bool hasRule() const;
   bool hasNonInheritedRule() const;

   CString getRuleValue() const;
   void setRuleValue(const CString& ruleValue);

   //int getModifierCount() const;
   //bool hasModifier() const;

   //CStencilRuleModifier getModifier(int index) const;
   //void addModifier(StencilRuleModifierTag modifier);
   //void addModifier(StencilRuleModifierTag modifier,const CStringArray& modifierValues);
   //void deleteModifiers();

private:
   void parseAttributes();
   void setAttributes();
   void removeStencilAttributes();

public:
   static void resetKeywordIndices();
   static void registerStencilKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog);
   static void removeStencilAttributes(CAttributes& attributes);

private:
   static int getStencilPadNoneKeywordIndex()               { return m_stencilPadNoneKeywordIndex; }
   static int getStencilPadAreaKeywordIndex()               { return m_stencilPadAreaKeywordIndex; }
   static int getStencilPadInsetKeywordIndex()              { return m_stencilPadInsetKeywordIndex; }
   static int getStencilPadApertureKeywordIndex()           { return m_stencilPadApertureKeywordIndex; }
   static int getStencilPadApertureGeometryKeywordIndex()   { return m_stencilPadApertureGeometryKeywordIndex; }
   static int getStencilPadStencilSourceLayerKeywordIndex() { return m_stencilPadStencilSourceLayerKeywordIndex; }
   static int getStencilPadPassthruKeywordIndex()           { return m_stencilPadPassthruKeywordIndex; }
   static int getStencilPadCopperSourceLayerKeywordIndex()  { return m_stencilPadCopperSourceLayerKeywordIndex; }
   static int getStencilPadInheritFromParentKeywordIndex()  { return m_stencilPadInheritFromParentKeywordIndex; }
   static int getStencilPadApertureModifierKeywordIndex()   { return m_stencilPadApertureModifierKeywordIndex; }
   static int getStencilPadCommentKeywordIndex()            { return m_stencilPadCommentKeywordIndex; }
 //static int getStencilPadGroupKeywordIndex()              { return m_stencilPadGroupKeywordIndex; }
   static int getStencilPadUndefinedKeywordIndex()          { return m_stencilPadUndefinedKeywordIndex; }

   static int getKeywordIndex(StencilRuleTag tagValue);
};

//_____________________________________________________________________________
class CStencilGroup : public CObject
{
private:
   CString m_groupName;
   Attrib* m_stencilAttribute;
   int m_stencilKeyword;

public:
   CStencilGroup(const CString& groupName,Attrib* stencilAttribute,int stencilKeyword);

   CString getGroupName() { return m_groupName; }
   Attrib* getStencilAttribute() { return m_stencilAttribute; }
   int getStencilKeyword() { return m_stencilKeyword; }
};

//_____________________________________________________________________________
class CStencilGroups : public CObject
{
private:
   CTypedMapStringToObContainer<CStencilGroup*> m_groups;

public:
   CStencilGroup* addGroup(const CString& groupName,Attrib* stencilAttribute,int stencilKeyword);
   bool lookup(const CString& groupName,CStencilGroup*& stencilGroup);
};

//_____________________________________________________________________________
class CSplitStencilHole
{
private:
   CCamCadDatabase& m_camCadDatabase;

   //int m_xSplits;
   //int m_ySplits;
   double m_xSplitSize;
   double m_ySplitSize;
   BlockStruct* m_geometry;
   DataStruct* m_stencilHole;

private:
   //CSplitStencilHole(CCamCadDatabase& camCadDatabase,
   //   FileStruct& fileStruct,DataStruct& pad,CExtent& padExtent,
   //   BlockStruct& padGeometry,double maxFeatureSize,double webSize,double webAngleDegrees,StencilSplitTypeTag splitType);
   CSplitStencilHole(CCamCadDatabase& camCadDatabase,
      FileStruct& fileStruct,DataStruct& pad,BlockStruct& padGeometry,
      double maxFeatureSize,double webSize,double webAngleDegrees,StencilSplitTypeTag splitType,double exteriorCornerRadius);

public:
   ~CSplitStencilHole();

public:
   static CSplitStencilHole* getSplitStencilHole(CCamCadDatabase& camCadDatabase,
      FileStruct& fileStruct,CCompositePad& compositePad,
      double maxFeatureSize,double webSize,double webAngleDegrees,StencilSplitTypeTag splitType,double exteriorCornerRadius);

   CCamCadDatabase& getCamCadDatabase() const;
   CCamCadData& getCamCadData() const;
   CCEtoODBDoc& getCamCadDoc() const;

   BlockStruct* getGeometry() { return m_geometry; }
   DataStruct* getStencilHole() { return m_stencilHole; }
   CSize2d getSplitSize() { return CSize2d(m_xSplitSize,m_ySplitSize); }
   double getArea() const;
   double getPerimeterLength() const;
};

//_____________________________________________________________________________
class CStencilHole : public CObject
{
private:
   int m_id;
   CStencilSubPin& m_stencilSubPin;
   bool m_topFlag;

   mutable DataStruct* m_copperPad;
   mutable bool m_copperPadInitializedFlag;

   DataStruct* m_stencilHole;
   DataStruct* m_stencilHoleInstance;
   double m_stencilThickness;
   CString m_ruleViolations;
   StencilRuleStatusTag m_ruleStatus;

   AttributeSourceTag m_attributeSource;
   CStencilRule m_stencilRule;

   CSplitStencilHole* m_splitStencilHole;

   mutable DataStruct* m_componentGeometryVessel;
   mutable DataStruct* m_padstackVessel;
   mutable DataStruct* m_subPadstackVessel;
   mutable DataStruct* m_subPinVessel;
   mutable DataStruct* m_componentVessel;
   mutable DataStruct* m_componentPinVessel;
   mutable DataStruct* m_componentSubPinVessel;

private:
   static int m_nextId;

public:
   CStencilHole(CStencilSubPin& stencilSubPin,bool topFlag);
   ~CStencilHole();

public:
   CCamCadDatabase& getCamCadDatabase() const;
   CCamCadData& getCamCadData() const;
   CStencilGenerator& getStencilGenerator() const;

   CStencilPin& getStencilPin() const;
   CStencilSubPin& getStencilSubPin() const;
    
   DataStruct* getStencilPad() const { return m_stencilHole; }

   DataStruct* getStencilHoleInstance() const;
   void setStencilHoleInstance(DataStruct* stencilHoleInstance);

   double getStencilThickness() const;
   void setStencilThickness(double stencilThickness) ;

   CString getRuleViolations() const;
   void setRuleViolations(const CString& ruleViolations);

   AttributeSourceTag getAttributeSource() const;
   void setAttributeSource(AttributeSourceTag attributeSource);

   StencilRuleStatusTag getRuleStatus() const { return m_ruleStatus ; }
   void setRuleStatus(StencilRuleStatusTag ruleStatus) { m_ruleStatus = ruleStatus; }

   bool getTopFlag() const { return m_topFlag; }

   DataStruct& getPcbVessel() const;
   DataStruct& getSurfaceVessel() const;
   DataStruct& getMountVessel() const;
   DataStruct& getComponentGeometryVessel() const;
   DataStruct& getPadstackVessel() const;
   DataStruct& getSubPadstackVessel() const;
   DataStruct& getSubPinVessel() const;
   DataStruct& getComponentVessel() const;
   DataStruct& getComponentPinVessel() const;
   DataStruct& getComponentSubPinVessel() const;

   CString getSurfaceName() const;
   CString getComponentName() const;
   CString getPadstackName() const;
   CString getSubPadstackName() const;
   CString getRefDes() const;
   CString getPinName() const;
   CString getSubPinName() const;
   CString getFirstPinName() const;
   
   bool getSplitFlag() const;
   CSplitStencilHole* getSplitStencilHole() const;
   DataStruct* getPad(const CLayerFilter& layerFilter) const;
   DataStruct* getPad(const CLayerFilter& layerFilter,DataStruct* topSubPad,DataStruct* bottomSubPad) const;
   DataStruct* getCopperPad() const;
   DataStruct* getStencilSourcePad() const;
   CPoly* getStencilPoly();
   CPoly* getCopperPoly();
   CPoly* getSplitStencilParentPoly();
   CString getCopperApertureDescriptor() const;
   CString getStencilApertureDescriptor() const;
   CString getDerivationMethod() const;
   CString getDerivationMethodForSourceLevel(AttributeSourceTag attributeSourceLevel) const;
   double getPasteVolume() const;

   CStencilRule getStencilRuleForSource(AttributeSourceTag attributeSource);
   CStencilRule getStencilRule();
   CStencilRuleAttributes getStencilRuleAttributes();
   CStencilRuleAttributes getStencilRuleAttributes(AttributeSourceTag sourceTag);
   AttributeSourceTag getStencilRuleForSourceLevel(AttributeSourceTag attributeSourceLevel,CStencilRule& stencilRule) const;

   void setStencilRuleForLevel(AttributeSourceTag sourceTag,const CStencilRule& stencilRule);
   DataStruct* calcStencilHole(CStencilGroups& stencilGroups,
      FileStruct* fileStruct,double maxFeatureSize,double webSize);
   void addAttributes(DataStruct* stencilAperture);
   bool updateStencilHoleAttribute();
   void performStencilValidationChecks(CStencilViolations& stencilViolations,
      CStencilValidationParameters& validationParameters,
      CStencilGenerationParameters& generationParameters);
   //bool calcRotationModifierRadians(double& padGeometryRotationRadians,CStandardAperture& standardAperture,StencilRuleModifierTag modifierTag);
   bool calcRotationModifierRadians(double& padGeometryRotationRadians,CStandardAperture& standardAperture,
      StencilOrientationModifierTag stencilOrientation,bool flipSymmetryAxis);
   bool calcOffsetModifierTMatrix(CTMatrix& offsetMatrix,
      CPoint2d offset,double angleOffsetRadians,
      StencilOrientationModifierTag offsetOrientation,bool flipSymmetryAxis);

   LayerStruct* getStencilLayer() const { return getCamCadDatabase().getLayer(m_topFlag ? ccLayerStencilTop : ccLayerStencilBottom); }
   void clearStencilHole();
   void clearStencilVessels();
   void instantiateStencilVessels();

   //static CString getRefDelimiter();

private:
   DataStruct& getVesselInstance(BlockStruct& vesselGeometry,const CString& vesselReference) const;
};

//_____________________________________________________________________________
class CStencilSubPin : public CObject
{
private:
   CStencilPin& m_stencilPin;
   int m_subPinIndex;
   //DataStruct& m_subPadstackVessel;
   //DataStruct& m_subPinVessel;

   CStencilHole m_topStencilHole;
   CStencilHole m_bottomStencilHole;

   CString m_subPinName;
   CString m_subPinRef;
   CString m_sortableSubPinName;
   CString m_sortableSubPinRef;

public:
   CStencilSubPin(CStencilPin& stencilPin,int subPinIndex/*,DataStruct& subPadstackVessel,DataStruct& subPinVessel*/);

   CCamCadDatabase& getCamCadDatabase() const;
   CStencilPin& getStencilPin() const;
   DataStruct& getComponent();
   BlockStruct* getPadStackGeometry() const;
   DataStruct* getSubPad(bool topFlag) const;
   //DataStruct& getComponentSubPinVessel() const;

   int getSubPinIndex() const;
   //DataStruct& getSubPadstackVessel();
   //DataStruct& getSubPinVessel();
   void clearStencilHoles();

   CString getGeometryName() const;
   CString getPadStackName() const;
   //CString getSubPadstackName() const;
   CString getSortablePinName() const;
   CString getPinRef() const;
   CString getSubPinRef() const;
   CString getPinName() const;
   CString getSortablePinRef() const;
   CString getSortableSubPinRef() const;
   CString getSubPinName() const;

   CString getSortableGeomPadstackSubpinPin(bool topFlag) const;

   CStencilHole& getStencilHole(bool topFlag) { return (topFlag ? m_topStencilHole : m_bottomStencilHole); }
   const CStencilHole& getStencilHole(bool topFlag) const { return (topFlag ? m_topStencilHole : m_bottomStencilHole); }

   void clearStencilVessels();
   void instantiateStencilVessels();
};

//_____________________________________________________________________________
class CStencilSubPinArray
{
private:
   int m_id;
   CStencilPin& m_stencilPin;
   CTypedPtrArrayContainer<CStencilSubPin*> m_subPins;

public:
   CStencilSubPinArray(CStencilPin& stencilPin);

   bool hasCopperPad(bool topFlag) const;
   void clearStencilHoles();
   bool updateStencilHoleAttributes();

   CStencilHole* getStencilHole(int subPinIndex,bool topFlag);
   const CStencilHole* getStencilHole(int subPinIndex,bool topFlag) const;

   int getCount() const;
   CStencilSubPin* getStencilSubPin(int index) const;
   CStencilSubPin& addSubPin(/*DataStruct& subPadstackVessel,DataStruct& subPinVessel*/);

   void clearStencilVessels();
   void instantiateStencilVessels();
};

////_____________________________________________________________________________
//class CStencilHoleArray
//{
//private:
//   int m_id;
//   CStencilPin& m_stencilPin;
//   bool m_topFlag;
//
//   CTypedPtrArrayContainer<CStencilHole*> m_topStencilHoles;
//   CTypedPtrArrayContainer<CStencilHole*> m_bottomStencilHoles;
//
//public:
//   CStencilHoleArray(CStencilPin& stencilPin);
//
//   bool hasCopperPad(bool topFlag) const;
//   void clearStencilHoles();
//   bool updateStencilHoleAttributes(bool topFlag);
//   int getCount() const;
//   CStencilHole& getStencilHole(int index,bool topFlag) const;
//};

//_____________________________________________________________________________
class CStencilPin : public CObject
{
private:
   static int m_nextId;

private:
   int m_id;

   CStencilGenerator& m_stencilGenerator;
   //CompPinStruct& m_compPin;
   //CComponentPin& m_componentPin;
   DataStruct& m_component;
   //DataStruct& m_padstackVessel;
   DataStruct& m_padStack;
   BlockStruct& m_pcb;

   mutable CPoint2d m_origin;

   bool m_attributesModifiedFlag;

   CString m_sortableRefDes;
   CString m_sortablePinName;
   CString m_sortablePinRef;
   CString m_firstPinName;

   bool m_drillToolValid;
   DataStruct* m_drillTool;

   //CStencilHole m_topStencilHole;
   //CStencilHole m_bottomStencilHole;
   //CStencilHoleArray m_stencilHoles;
   CStencilSubPinArray m_subPins;

private:
   static int m_stencilPinrefKeywordIndex;
   static int m_stencilDerivationMethodKeywordIndex;
   static int m_stencilThicknessKeywordIndex;
   static int m_stencilPasteVolumeKeywordIndex;

   static int m_stencilTopThicknessKeywordIndex;
   static int m_stencilBottomThicknessKeywordIndex;
   static int m_stencilTopMaxFeatureKeywordIndex;
   static int m_stencilBottomMaxFeatureKeywordIndex;
   static int m_stencilTopWebWidthKeywordIndex;
   static int m_stencilBottomWebWidthKeywordIndex;
   static int m_stencilTopStencilSourceLayerKeywordIndex;
   static int m_stencilBottomStencilSourceLayerKeywordIndex;
   static int m_stencilExteriorCornerRadiusKeywordIndex;
   static int m_stencilTopPasteInHoleKeywordIndex;
   static int m_stencilBottomPasteInHoleKeywordIndex;
   static int m_stencilTopPasteInViaKeywordIndex;
   static int m_stencilBottomPasteInViaKeywordIndex;
   static int m_stencilUseMountTechAttribKeywordIndex;
   static int m_stencilMountTechAttribNameKeywordIndex;
   static int m_stencilMountTechAttribSMDValueKeywordIndex;
   static int m_stencilPropagateRulesKeywordIndex;

public:
   static void resetNextId();
   static void resetKeywordIndices();

   static int getStencilPinrefKeywordIndex()                { return m_stencilPinrefKeywordIndex; }
   static int getStencilDerivationMethodKeywordIndex()      { return m_stencilDerivationMethodKeywordIndex; }
   static int getStencilThicknessKeywordIndex()             { return m_stencilThicknessKeywordIndex; }
   static int getStencilPasteVolumeKeywordIndex()           { return m_stencilPasteVolumeKeywordIndex; }

public:
   CStencilPin(CStencilGenerator& stencilGenerator,/*CComponentPin& componentPin,*/DataStruct& component,
      /*DataStruct& padstackVessel,*/DataStruct& padStack,BlockStruct& pcb);
   virtual ~CStencilPin();

   CCamCadDatabase& getCamCadDatabase()     const;
   CStencilGenerator& getStencilGenerator() const;
   //CompPinStruct& getCompPin()              { return m_compPin; }
   CComponentPin& getComponentPin();
   DataStruct& getComponent();
   DataStruct& getComponentPinVessel(bool topFlag) const;
   DataStruct* getDrillTool();
   bool hasDrillTool();

   // insert of padstack vessel in component geometry
   //DataStruct& getPadstackVessel()     { return m_padstackVessel; }

   // insert of padstack in component geometry
   DataStruct& getPadStack();
   BlockStruct& getPcb();
   CPoint2d& getOrigin();

   int getId() const;
   CString getRefDes() const;
   CString getPinName() const;
   CString getPinRef() const;
   CString getSortablePinName() const;
   CString getFirstPinName();
   CString getSortablePinRef() const;
   CString getSortableGeomPad(bool topFlag) const;
   CString getSortableGeomPadPin(bool topFlag) const;
   CString getGeometryName() const;
   BlockStruct* getGeometry() const;
   BlockStruct* getPadStackGeometry() const;
   CString getPadStackName() const;
   CPoint2d getOrigin() const;

   //CStencilHole& getStencilHole(bool topFlag) { return (topFlag ? m_topStencilHole : m_bottomStencilHole); }
   //const CStencilHole& getStencilHole(bool topFlag) const { return (topFlag ? m_topStencilHole : m_bottomStencilHole); }
   CStencilHole& getStencilHole(int holeIndex,bool topFlag);
   const CStencilHole& getStencilHole(int holeIndex,bool topFlag) const;
   //CStencilHoleArray& getStencilHoles();
   //const CStencilHoleArray& getStencilHoles() const;
   //int getStencilHoleCount() const;

   CStencilSubPin& addSubPin(/*DataStruct& subPadstackVessel,DataStruct& subPinVessel*/);
   CStencilSubPinArray& getSubPins();
   CStencilSubPin* getSubPin(int subPinIndex) const;
   const CStencilSubPinArray& getSubPins() const;
   int getSubPinCount() const;

   CPoly* getPadPoly(DataStruct* pad);
   CString getPadDescriptor(DataStruct* pad) const;

   void clearStencilHoles();
   void clearStencilVessels();
   void instantiateStencilVessels();
   double getPageUnitsToDisplayUnitsFactor() const;
   bool isSmdPin() const;
   bool isSmd() const;
   bool isVia() const;
   bool isFiducial() const;

   static void registerStencilKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog);
   static int getKeywordIndex(StencilAttributeTag attributeTag);
   static PageUnitsTag getDisplayUnits(PageUnitsTag pageUnits);

   bool updateStencilHoleAttribute();
   bool getAttributesModified() { return m_attributesModifiedFlag; }
   void setAttributesModified(bool flag);
};

//_____________________________________________________________________________
class CStencilPins : public CTypedObListContainer<CStencilPin*>
{
private:

public:
   CStencilPins();
   virtual ~CStencilPins();

   CStencilPin& addStencilPin(CStencilGenerator& stencilGenerator,
      /*CComponentPin& componentPin,*/DataStruct& component,
      /*DataStruct& padstackVessel,*/DataStruct& padStack,BlockStruct& pcb);
   void clearAttributesModifiedFlags();
   bool getAttributesModified();
   bool isValid();
   void clearStencilVessels();
   void instantiateStencilVessels();

   void addStencilEntities(CStencilGenerator& stencilGenerator,FileStruct& fileStruct);
   void calcStencilHoles(CDataList& topStencilHoleDataList,CDataList& bottomStencilHoleDataList,
      CCamCadDatabase& camCadDatabase,
      CStencilGenerationParameters& stencilGenerationParameters,FileStruct& fileStruct);
   void initializeRoot();
};

//_____________________________________________________________________________
class CSortedStencilPins : public CMapSortedStringToOb<CStencilPin>
{
public:
   CSortedStencilPins(int blockSize) : CMapSortedStringToOb<CStencilPin>(blockSize,false)
   {
   }
};

//_____________________________________________________________________________
class CStencilViolation : public CObject
{
protected:
   bool m_topFlag;
   CStencilSubPin& m_stencilSubPin;
   StencilViolationStatusTag m_status;

public:
   CStencilViolation(CStencilSubPin& stencilSubPin,bool topFlag);
   virtual ~CStencilViolation(){}

   CStencilSubPin& getStencilSubPin() { return m_stencilSubPin; }
   CStencilPin&    getStencilPin()    { return m_stencilSubPin.getStencilPin(); }
   bool getTopFlag() { return m_topFlag; }

   CString getPrintableValue(double val);

   virtual StencilViolationTag getViolationType() { return stencilViolationUndefined; }
   virtual CString getSortKey();
   virtual CString getDescription() { return CString(""); }
   virtual CString getAbbreviatedDescription() { return CString(""); }
   virtual StencilViolationStatusTag getStatus() { return m_status; }
};

//_____________________________________________________________________________
class CStencilHoleAreaToWallAreaViolation : public CStencilViolation
{
private:
   double m_holeArea;
   double m_wallArea;
   double m_minHoleAreaToWallAreaRatio;

public:
   CStencilHoleAreaToWallAreaViolation(CStencilSubPin& stencilSubPin,bool topFlag,
      double holeArea,double wallArea,double minHoleAreaToWallAreaRatio);

   virtual StencilViolationTag getViolationType() { return stencilViolationHoleAreaToWallAreaRatio; }
   virtual CString getDescription();
   virtual CString getAbbreviatedDescription() { return CString("areaRatio"); }
};

//_____________________________________________________________________________
class CStencilMaximumFeatureViolation : public CStencilViolation
{
private:
   double m_featureSize;
   double m_maxFeatureSize;

public:
   CStencilMaximumFeatureViolation(CStencilSubPin& stencilSubPin,bool topFlag,
      double featureSize,double maxFeatureSize);

   virtual StencilViolationTag getViolationType() { return stencilViolationMaximumFeature; }
   virtual CString getDescription();
   virtual CString getAbbreviatedDescription() { return CString("maxFeature"); }
};

//_____________________________________________________________________________
class CStencilMinimumFeatureViolation : public CStencilViolation
{
private:
   double m_featureSize;
   double m_minFeatureSize;

public:
   CStencilMinimumFeatureViolation(CStencilSubPin& stencilSubPin,bool topFlag,
      double featureSize,double maxFeatureSize);

   virtual StencilViolationTag getViolationType() { return stencilViolationMinimumFeature; }
   virtual CString getDescription();
   virtual CString getAbbreviatedDescription() { return CString("minFeature"); }
};

//_____________________________________________________________________________
class CStencilMinimumInsetViolation : public CStencilViolation
{
private:
   double m_minInsetDistance;
   double m_minInsetSize;

public:
   CStencilMinimumInsetViolation(CStencilSubPin& stencilSubPin,bool topFlag,double minInsetDistance,
      double minInsetSize);

   virtual StencilViolationTag getViolationType() { return stencilViolationMinimumFeature; }
   virtual CString getDescription();
   virtual CString getAbbreviatedDescription() { return CString("minInset"); }
};

//_____________________________________________________________________________
class CStencilSplitPadViolation : public CStencilViolation
{
private:
   double m_featureSize;
   double m_maxFeatureSize;

public:
   CStencilSplitPadViolation(CStencilSubPin& stencilSubPin,bool topFlag,
      double featureSize,double maxFeatureSize);

   virtual StencilViolationTag getViolationType() { return stencilViolationSplitPad; }
   virtual CString getDescription();
   virtual CString getAbbreviatedDescription() { return CString("splitPad"); }
};

//_____________________________________________________________________________
class CStencilViolations : public CTypedMapSortStringToObContainer<CStencilViolation>
{
public:
   CStencilViolations() : CTypedMapSortStringToObContainer<CStencilViolation>(nextPrime2n(1000)){}
   virtual ~CStencilViolations(){}

   void add(CStencilViolation* stencilViolation);
   bool isValid();
};

//_____________________________________________________________________________
class CComponentDirectory : public CTypedPtrMap<CMapStringToPtr,CString,DataStruct*>
{
public:
   CComponentDirectory(CCamCadDatabase& camCadDatabase,FileStruct* fileStruct);

   DataStruct* getComponent(const CString& refdes);
};

//_____________________________________________________________________________
class CPadStackInsert
{
private:
   CCamCadDatabase& m_camCadDatabase;
   DataStruct& m_padStackInsertData;
   CString m_topStencilSourceLayerName;
   CString m_bottomStencilSourceLayerName;

   static int m_subPadIndexKeywordIndex;
   static int m_stencilSourceSubPadIndexKeywordIndex;

public:
   CPadStackInsert(CCamCadDatabase& camCadDatabase,DataStruct& padStackInsertData);
   CPadStackInsert(CCamCadDatabase& camCadDatabase,DataStruct& padStackInsertData,const CString& topStencilSourceLayerName,const CString& bottomStencilSourceLayerName);

   CCamCadDatabase& getCamCadDatabase() const { return m_camCadDatabase; }
   CCamCadData& getCamCadData() const;
   CCEtoODBDoc& getCamCadDoc() const;

   int getSubPadCount();
   DataStruct* getSubPad(int subPadIndex,bool topFlag);
   bool isAllCopperLayer(const LayerStruct& layer) const;
   bool isOuterCopperLayer(const LayerStruct& layer) const;
   bool isTopCopperLayer(const LayerStruct& layer) const;
   bool isBottomCopperLayer(const LayerStruct& layer) const;
   bool isSurfaceCopperLayer(const LayerStruct& layer,bool topSurfaceFlag) const;
   bool getSubPadIndexAttributeValue(int& subPadIndex,DataStruct& data);
   void setSubPadIndexAttributeValue(int  subPadIndex,DataStruct& data);

   DataStruct* getStencilSourceSubPad(int subPadIndex,bool topFlag);
   bool isStencilSourceLayer(const LayerStruct& layer,bool topSurfaceFlag) const;
   bool getStencilSourceSubPadIndexAttributeValue(int& subPadIndex,DataStruct& data);
   void setStencilSourceSubPadIndexAttributeValue(int  subPadIndex,DataStruct& data);

   bool hasStructurallyBuriedCopper() const;
   bool hasStructurallyBuriedCopper(const BlockStruct& geometry,int parentLayerIndex,int level) const;
   BlockStruct& generateRestructuredPadStack() const;
   void flattenDataList(CDataList& flattenedDataList,const CDataList& sourceDataList,int parentLayerIndex,int level) const;
   void flattenInsert(CDataList& flatDataList,DataStruct& parentDataInsert,int parentLayer,const CTMatrix& parentMatrix) const;

   int getSubPadIndexKeywordIndex();
   int getStencilSourceSubPadIndexKeywordIndex();

   static void resetKeywordIndices();

private:
   void initializeSubPads(bool topFlag);
};

//_____________________________________________________________________________
class CStencilGenerator : public CObject
{
private:
   CStencilUi& m_stencilUi;

   FileStruct* m_fileStruct;
   CStencilPins m_stencilPins;
   CStencilGroups m_stencilGroups;
   CStencilViolations m_stencilViolations;
   //CComponentDirectory* m_componentDirectory;

   DataStruct* m_pcbVessel;
   DataStruct* m_surfaceVessel[2];
   DataStruct* m_mountVessel[8];

   BlockStruct* m_pcbVesselGeometry;
   BlockStruct* m_surfaceVesselGeometry;
   BlockStruct* m_mountVesselGeometry;
   BlockStruct* m_componentGeometryVesselGeometry;
   BlockStruct* m_padstackVesselGeometry;
   BlockStruct* m_subPadstackVesselGeometry;
   BlockStruct* m_subPinVesselGeometry;
   BlockStruct* m_componentVesselGeometry;
   BlockStruct* m_componentPinVesselGeometry;
   BlockStruct* m_componentSubPinVesselGeometry;
   BlockStruct* m_stencilGeneratorClosetGeometry;

   CMapStringToString m_smdGeometries;
   CMapEntityToDataStruct m_viaComponentMap;
   CMapEntityToDataStruct m_fiducialComponentMap;

   DataStruct* m_stencilGeneratorCloset;
   CTypedMapStringToPtrContainer<DataStruct*> m_closetVesselMap; // maps closetRefs to vessels

   bool m_initializedFlag;
   int  m_smdListRevLevel;  // users of smd geometry list use this to tell if they are in sync

   CString m_stencilBlockBaseName;

public:
   CStencilGenerator(CStencilUi& stencilUi);
   ~CStencilGenerator();

public:
   CStencilUi& getStencilUi() { return m_stencilUi; }
   CStencilPins& getStencilPins() { return m_stencilPins; }
   CStencilViolations& getStencilViolations() { return m_stencilViolations; }
   CCamCadDatabase& getCamCadDatabase() const { return m_stencilUi.getCamCadDatabase(); }
   CCamCadData& getCamCadData() { return getCamCadDatabase().getCamCadData(); }
   CCEtoODBDoc& getCamCadDoc() const { return getCamCadDatabase().getCamCadDoc(); }
   CStencilValidationParameters& getStencilValidationParameters() 
      { return  m_stencilUi.getStencilValidationParameters(); }
   CStencilGenerationParameters& getStencilGenerationParameters() 
      { return  m_stencilUi.getStencilGenerationParameters(); }

   PageUnitsTag getPageUnits() const { return getCamCadDoc().getPageUnits(); }

   //CComponentDirectory& getComponentDirectory();
   bool getInitializedFlag() { return m_initializedFlag; }
   FileStruct* getFileStruct() const { return m_fileStruct; }

   FileStruct* getStencilMicrocosmFile();
   void showStencilMicrocosmFile();
   void hideStencilMicrocosmFile();

   void convertToPinnedData(DataStruct& data);
   void restructureFiducialGeometryForInsert(DataStruct& data);
   void initializeStencilEntities(int fileNumber,COperationProgress* progress=NULL);
   void identifySMDEntities(int fileNumber,COperationProgress* progress = NULL);
   void propagateStencilThickness(double thickness, bool topSideFlag);
   void removeTemporaryStencilEntities();
   DataStruct* getViaComponent(DataStruct* via,CDataList& viaComponentDataList);
   void discardViaComponents();
   CString getViaComponentGeometryNamePrefix();
   DataStruct* getFiducialComponent(DataStruct* fiducial,CDataList& fiducialComponentDataList);
   void discardFiducialComponents();
   CString getFiducialComponentGeometryNamePrefix();

   //void getGroups();
   void migrateOldRuleData();
   void normalizeApertureOffsetsAndRotations();
   DataStruct* findStencilGeneratorCloset();
   DataStruct& getStencilGeneratorCloset();

   DataStruct& getPcbVessel();
   DataStruct& getSurfaceVessel(          bool topFlag);
   DataStruct& getMountVessel(            bool topFlag,bool smdFlag,bool viaFlag,bool fiducialFlag);
   DataStruct& getComponentGeometryVessel(bool topFlag,const CString& componentGeometryName);
   DataStruct& getPadstackVessel(         bool topFlag,const CString& componentGeometryName,const CString& firstPinName);
   DataStruct& getSubPadstackVessel(      bool topFlag,const CString& componentGeometryName,const CString& firstPinName,const CString& subPinName);
   DataStruct& getSubPinVessel(           bool topFlag,const CString& componentGeometryName,const CString& pinName     ,const CString& subPinName);
   DataStruct& getComponentVessel(        bool topFlag,const CString& refDes);
   DataStruct& getComponentPinVessel(     bool topFlag,const CString& refDes,const CString& pinName);
   DataStruct& getComponentSubPinVessel(  bool topFlag,const CString& refDes,const CString& pinName,const CString& subPinName);

   DataStruct& getClosetVessel(const CString& vesselRef,BlockStruct& vesselGeometry);
   DataStruct& getComponentClosetVessel(const CString& componentGeometryName,const CString& vesselRef,BlockStruct& vesselGeometry);
   void clearStencilVessels();
   void instantiateStencilVessels();

   BlockStruct& getPcbVesselGeometry();
   BlockStruct& getSurfaceVesselGeometry();
   BlockStruct& getMountVesselGeometry();
   BlockStruct& getComponentGeometryVesselGeometry();
   BlockStruct& getPadstackVesselGeometry();
   BlockStruct& getSubPadstackVesselGeometry();
   BlockStruct& getSubPinVesselGeometry();
   BlockStruct& getComponentVesselGeometry();
   BlockStruct& getComponentPinVesselGeometry();
   BlockStruct& getComponentSubPinVesselGeometry();
   BlockStruct& getStencilGeneratorClosetGeometry();

   void renderStencils(CStencilGenerationParameters& stencilGenerationParameters,
      CStencilValidationParameters& stencilValidationParameters,
      CWriteFormat& logFile,CWriteFormat* reportFile=NULL);
   void performStencilValidationChecks(CStencilValidationParameters& validationParameters,
      CStencilGenerationParameters& generationParameters);
   void generatePinReport(CWriteFormat& logFile,CSortedStencilPins& stencilHoles,bool topFlag,COperationProgress* progress=NULL);
   void generatePinReports(CWriteFormat& logFile,COperationProgress* progress=NULL);
   //CString getAttributeNameValuePair(Attrib* stencilAttribute,int stencilKeyword);
   bool isSmdGeometry(const CString& geometryName);
   bool isViaGeometry(const CString& geometryName);
   bool isFiducialGeometry(const CString& geometryName);

   bool readStencilSettings(CStringArray& params);
   bool readStencilSettings(const CString& ruleString);
   bool readStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase,
      COperationProgress* operationProgress=NULL);
   bool writeStencilSettings(CStencilSettingEntryMap& stencilSettingEntryMap,bool ignoreComponentRulesFlag);
   bool writeStencilSettings(CWriteFormat& settingsFile,CStencilSettingEntryMap& stencilSettingEntryMap);
   bool writeStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase);
   void clearStencilSettings();
   void addStencilSettingAttributes(CAttributes** attributes,CStringArray& params,int startIndex);

   bool isValid();
   int  getSMDListRevLevel()   { return m_smdListRevLevel; }

private:
   int getMountVesselIndex(bool topFlag,bool smdFlag,bool viaFlag,bool fiducialFlag) const;
   CString getMountVesselReference(bool topFlag,bool smdFlag,bool viaFlag,bool fiducialFlag,bool oldDelimiterFlag=false) const;
   DataStruct* generatePadStack(const CPoly& sourcePoly, int layerIndex, CString basename);
   DataStruct* generatePadStack(DataStruct& apertureData, int layerIndex, CString basename);
   void convertComponentCopperToPadstacks(BlockStruct& componentGeometry);
   void convertComponentStencilSourceLayerPolysToPadstacks(BlockStruct& componentGeometry);

   void PromoteStencilSourceLayerToStencilLayers(int &topHoleCount, int &botHoleCount);  // Top level, this is the one to call directly from renderStencils
   void PromoteStencilSourceLayer(FileStruct *file, DataStruct *insertdata, int &topHoleCount, int &botHoleCount);  // Support func
   bool HasPassthruRuleSet(BlockStruct *block); // Pass in a pcb component's inserted block
   BlockStruct* GetStencilBlock(FileStruct *file, DataStruct *insertdata, int topStencilSrcLyrIndx, int botStencilSrcLyrIndx);
   BlockStruct* GetStencilBlock(FileStruct *file, BlockStruct *block, int insertLayerIndx, int topStencilSrcLyrIndx, int botStencilSrcLyrIndx);
   CString GenerateStencilBlockName(BlockStruct &sourceBlock);
   void DeleteExistingStencilBlocks();

   void PromotePanelFidsToStencilLayers();                    // Processes all panel fids in all panel files
   void PromotePanelFidToStencilLayers(DataStruct *fiddata);  // Process this one fid
   void CloneSurfaceGraphicsToStencilLayers(BlockStruct *blk);
   bool HasStencilGraphic(DataStruct *data);                  // True if visible on stencil layers otherwise false
   bool HasStencilGraphic(BlockStruct *block);                // True if visible on stencil layers otherwise false
};

//_____________________________________________________________________________
class CStencilEntityIteratorItem
{
private:
   const CStencilEntityRule m_stencilEntityRule;
   DataStruct& m_vesselData;
   //CAttributes* m_attributes;

public:
   CStencilEntityIteratorItem(const CStencilEntityRule& stencilEntityRule,DataStruct& vesselData);

   const CStencilEntityRule& getStencilEntityRule();
   const DataStruct& getVesselData() const;
   CAttributes* getAttributes();
};

//_____________________________________________________________________________
class CStencilEntityIteratorItems
{
private:
   CTypedPtrListContainer<CStencilEntityIteratorItem*> m_items;
   mutable CTypedMapStringToPtrContainer<CStencilEntityIteratorItem*>* m_itemMap;

public:
   CStencilEntityIteratorItems();
   ~CStencilEntityIteratorItems();
   void empty();

   POSITION GetHeadPosition() const;
   CStencilEntityIteratorItem* GetNext(POSITION& pos) const;
   void AddTail(CStencilEntityIteratorItem* item);
   bool lookup(const CStencilEntityRule& stencilEntityRule,CStencilEntityRule& foundStencilEntityRule,CAttributes*& foundAttributes) const;
   int getCount() const;

private:
   CTypedMapStringToPtrContainer<CStencilEntityIteratorItem*>& getItemMap() const;
};

//_____________________________________________________________________________
class CStencilEntityIterator
{
private:
   CStencilGenerator& m_stencilGenerator;
   FileStruct* m_pcbFile;
   POSITION m_pos;
   CStencilEntityIteratorItems m_items;

public:
   CStencilEntityIterator(CStencilGenerator& stencilGenerator);
   CStencilEntityIterator(CStencilGenerator& stencilGenerator,FileStruct* fileStruct);
   void init();

   bool hasNext() const;
   bool getNext(CStencilEntityRule& stencilEntityRule,CAttributes*& attributes);
   bool lookup(const CStencilEntityRule& stencilEntityRule,CStencilEntityRule& foundStencilEntityRule,CAttributes*& foundAttributes) const;
   int getCount() const;

private:
   CStencilEntityIteratorItem* addItem(const CStencilEntityRule& stencilEntityRule,DataStruct& vesselData);
};

//_____________________________________________________________________________
class CStencilGeometryMicrocosm : public CObject
{
private:
   CStencilGenerator& m_stencilGenerator;
   FileStruct* m_microcosmFile;
   DataStruct* m_microcosmComponent;
   BlockStruct* m_microcosmGeometry;
   CString m_refDes;
   CStencilPins m_stencilPins;
   CDataList m_topStencilPads;
   CDataList m_bottomStencilPads;
   AttributeSourceTag m_attributeSource;
   CStencilSubPin* m_sourceStencilSubPin;
   bool m_sourceTopFlag;
   //int m_holeIndex;
   StencilRuleStatusTag m_ruleStatus;

public:
   CStencilGeometryMicrocosm(CStencilGenerator& stencilGenerator);
   ~CStencilGeometryMicrocosm();

   void setGeometry(BlockStruct& sourceGeometry ,FileStruct& sourceFile,CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,AttributeSourceTag attributeSource);
   void setComponent(DataStruct& sourceComponent,FileStruct& sourceFile,CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,AttributeSourceTag attributeSource);

   FileStruct* getMicrocosmFile() const;
   FileStruct& getDefinedMicrocosmFile();
   CCamCadDatabase& getCamCadDatabase();
   CCamCadData& getCamCadData();
   CStencilGenerationParameters& getStencilGenerationParameters();
   void getStencilPads(CDataList& destinationStencilPads,bool topFlag = true);
   CStencilPins& getStencilPins();
   DataStruct* getComponent();
   BlockStruct* getGeometry();
   //CStencilPin* getSourceStencilPin();
   CStencilSubPin* getSourceStencilSubPin();
   bool getSourceTopFlag();
   //int getSourceHoleIndex() const;
   AttributeSourceTag getAttributeSource();
   void copyStencilGeneratorClosetInfo(bool topFlag,const CString& refDes);

   StencilRuleStatusTag getRuleStatus() const;
   void setRuleStatus(StencilRuleStatusTag status);
   void updateRuleStatus(StencilRuleStatusTag status);
   void resetRuleStatus();

   CString getRefDes() const;
   void setRefDes(const CString& refDes);

private:
   void createMicrocosmComponent(DataStruct* sourceComponent=NULL,FileStruct* sourceFile=NULL);
   void createMicrocosmGeometry(BlockStruct& sourceGeometry,FileStruct& sourceFile);
   void emptyStencilEntities();

   void destroyMicrocosmComponent();
   void destroyMicrocosmGeometry();

public:
   static CString getFileName();
   //static CString getRefDes();
};

#endif
