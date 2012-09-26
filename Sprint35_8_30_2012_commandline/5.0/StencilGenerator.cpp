
#include "StdAfx.h"
#include "StencilGenerator.h"
#include "TMState.h"
#include "StandardAperture.h"
#include "WriteFormat.h"
#include "Apertur2.h"
#include "Attribute.h"
#include "Vector3d.h"
#include "Region.h"
#include "EntityNumber.h"
#include "LyrManip.h"
#include "RegularExpression.h"
#include "ODBC_Lib.h"
#include "Response.h"


#define AllocateEntityNumber camCadDatabase.getCamCadData().allocateEntityNumber
#define newDataStruct getCamCadData().getNewDataStruct
#define newDataStructO camCadData.getNewDataStruct
#define camCadDatabase_camCadData m_camCadDatabase.getCamCadData()
#define camCadDatabase_camCadDataF getCamCadDatabase().getCamCadData
#define ShrinkPoly(a,b) shrink(a,b)
#define getCamCadDataDoc getCamCadData
#define PolyContains(a,b,c) contains(a,b,c)


#define QSAlignApertureMajorAxisTowardComponentCenterline   "Align aperture centerline toward component centerline"
#define QSAlignApertureMajorAxisAwayFromComponentCenterline "Align aperture centerline away from component centerline"
#define QSAlignApertureMinorAxisTowardComponentCenterline   "Align aperture minor axis toward component centerline"
#define QSAlignApertureMinorAxisAwayFromComponentCenterline "Align aperture minor axis away from component centerline"
#define QSAlignApertureMajorAxisParallelToComponentXAxis    "Align aperture major axis parallel to component X axis"
#define QSAlignApertureMajorAxisParallelToComponentYAxis    "Align aperture major axis parallel to component Y axis"
#define QSAlignApertureMinorAxisParallelToComponentXAxis    "Align aperture minor axis parallel to component X axis"
#define QSAlignApertureMinorAxisParallelToComponentYAxis    "Align aperture minor axis parallel to component Y axis"
#define QSOrientSymmetryAxis                                "Orient Symmetry Axis"
#define QSFlipSymmetryAxis                                  "Flip Symmetry Axis"
#define QSOffsetRelativeToPadstack                          "Offset relative to padstack"
#define QSStencilWebSettings                                "Stencil web settings"

#define QAlignApertureMajorAxisTowardComponentCenterline    "AlignApertureMajorAxisTowardComponentCenterline"
#define QAlignApertureMajorAxisAwayFromComponentCenterline  "AlignApertureMajorAxisAwayFromComponentCenterline"
#define QAlignApertureMinorAxisTowardComponentCenterline    "AlignApertureMinorAxisTowardComponentCenterline"
#define QAlignApertureMinorAxisAwayFromComponentCenterline  "AlignApertureMinorAxisAwayFromComponentCenterline"
#define QAlignApertureMajorAxisParallelToComponentXAxis     "AlignApertureMajorAxisParallelToComponentXAxis"
#define QAlignApertureMajorAxisParallelToComponentYAxis     "AlignApertureMajorAxisParallelToComponentYAxis"
#define QAlignApertureMinorAxisParallelToComponentXAxis     "AlignApertureMinorAxisParallelToComponentXAxis"
#define QAlignApertureMinorAxisParallelToComponentYAxis     "AlignApertureMinorAxisParallelToComponentYAxis"
#define QOrientSymmetryAxis                                 "OrientSymmetryAxis"
#define QFlipSymmetryAxis                                   "FlipSymmetryAxis"
#define QOffsetRelativeToPadstack                           "OffsetRelativeToPadstack"
#define QStencilWebSettings                                 "StencilWebSettings"
#define QStencilExteriorCornerRadius                        "StencilExteriorCornerRadius"

#define QTowardCenter    "TowardCenter"
#define QAwayFromCenter  "AwayFromCenter"
#define QNormal          "Normal"

#define QGrid            "Grid"
#define QStripe          "Stripe"



CompPinStruct *check_add_comppin(const char *c, const char *p, FileStruct *f);

static CString getSurfaceName(bool topFlag)
{
   /// Side-specific support has been disabled, we want rules to
   /// apply to both sides, so here we choose to disregard the topFlag

   ///return (topFlag ? QTop : QBottom);

   return QBoth;
}

static CString getRefDelimiter()
{
   return ".";
}

static CString getDesignatorString(const CString& rawDesignatorString)
{
   return quoteString(rawDesignatorString," .");
}

CString stencilRuleToRuleString(StencilRuleTag tagValue)
{
   const char* retval = "?";

   switch (tagValue)
   {
   case stencilRuleNone:                retval = "StencilPadNone";                break;
   case stencilRuleArea:                retval = "StencilPadArea";                break;
   case stencilRuleInset:               retval = "StencilPadInset";               break;
   case stencilRuleAperture:            retval = "StencilPadAperture";            break;
   case stencilRuleApertureGeometry:    retval = "StencilPadApertureGeometry";    break;
   case stencilRuleStencilSourceLayer:  retval = "stencilPadStencilSourceLayer";  break;
   case stencilRulePassthru:            retval = "stencilPadPassthru";            break;
   case stencilRuleCopperSourceLayer:   retval = "stencilPadCopperSourceLayer";   break;
   case stencilRuleInheritFromParent:   retval = "stencilPadInheritFromParent";   break;
 //case stencilRuleApertureModifier:    retval = "stencilPadApertureModifier";    break;
   }

   return CString(retval);
}

CString attributeSourceToString(AttributeSourceTag tagValue)
{
   const char* retval = "?";

   switch (tagValue)
   {
   case attributeSourceGeometryPin:      retval = QGeometryPin;      break;
   case attributeSourceComponentPin:     retval = QComponentPin;     break;
   case attributeSourceComponentSubPin:  retval = QComponentSubPin;  break;
   case attributeSourceComponent:        retval = QComponent;        break;
   case attributeSourcePadstack:         retval = QPadstack;         break;
   case attributeSourceSubPadstack:      retval = QSubPadstack;      break;
   case attributeSourceGeometry:         retval = QGeometry;         break;
   case attributeSourceMount:            retval = QMount;            break;
   case attributeSourceSurface:          retval = QSurface;          break;
   case attributeSourcePcb:              retval = QPcb;              break;
   case attributeSourceNone:             retval = QNone;             break;
   }

   return CString(retval);
}

AttributeSourceTag stringToAttributeSourceTag(const CString& tagValue)
{
   AttributeSourceTag attributeSourceTag = attributeSourceUndefined;

   if (tagValue.CompareNoCase(QGeometryPin) == 0)
   {
      attributeSourceTag = attributeSourceGeometryPin;
   }
   else if (tagValue.CompareNoCase(QComponentPin) == 0)
   {
      attributeSourceTag = attributeSourceComponentPin;
   }
   else if (tagValue.CompareNoCase(QComponentSubPin) == 0)
   {
      attributeSourceTag = attributeSourceComponentSubPin;
   }
   else if (tagValue.CompareNoCase(QComponent) == 0)
   {
      attributeSourceTag = attributeSourceComponent;
   }
   else if (tagValue.CompareNoCase(QPadstack) == 0)
   {
      attributeSourceTag = attributeSourcePadstack;
   }
   else if (tagValue.CompareNoCase(QSubPadstack) == 0)
   {
      attributeSourceTag = attributeSourceSubPadstack;
   }
   else if (tagValue.CompareNoCase(QGeometry) == 0)
   {
      attributeSourceTag = attributeSourceGeometry;
   }
   else if (tagValue.CompareNoCase(QMount) == 0)
   {
      attributeSourceTag = attributeSourceMount;
   }
   else if (tagValue.CompareNoCase(QSurface) == 0)
   {
      attributeSourceTag = attributeSourceSurface;
   }
   else if (tagValue.CompareNoCase(QPcb) == 0)
   {
      attributeSourceTag = attributeSourcePcb;
   }
   else if (tagValue.CompareNoCase(QNone) == 0)
   {
      attributeSourceTag = attributeSourceNone;
   }

   return attributeSourceTag;
}

AttributeSourceTag parentAttributeSource(AttributeSourceTag tagValue)
{
   AttributeSourceTag retval = attributeSourceUndefined;

   switch (tagValue)
   {
   case attributeSourceGeometryPin:      retval = attributeSourceSubPadstack;   break;
   case attributeSourceSubPadstack:      retval = attributeSourcePadstack;      break;
   case attributeSourcePadstack:         retval = attributeSourceGeometry;      break;
   case attributeSourceGeometry:         retval = attributeSourceMount;         break;
   case attributeSourceMount:            retval = attributeSourceSurface;       break;
   case attributeSourceSurface:          retval = attributeSourcePcb;           break;
   case attributeSourcePcb:              retval = attributeSourceNone;          break;

   case attributeSourceComponentSubPin:  retval = attributeSourceComponentPin;  break;
   case attributeSourceComponentPin:     retval = attributeSourceComponent;     break;
   case attributeSourceComponent:        retval = attributeSourceSurface;       break;

   case attributeSourceNone:             retval = attributeSourceNone;          break;
   }

   return retval;
}

CString stencilRuleTagToRuleString(StencilRuleTag tagValue)
{
   const char* retval = "";

   switch (tagValue)
   {
   case stencilRuleNone:                retval = "None";                  break;
   case stencilRuleArea:                retval = "Area";                  break;
   case stencilRuleInset:               retval = "Inset";                 break;
   case stencilRuleAperture:            retval = "Aperture";              break;
   case stencilRuleApertureGeometry:    retval = "Aperture Geometry";     break;
   case stencilRuleStencilSourceLayer:  retval = "Stencil Source Layer";  break;
   case stencilRulePassthru:            retval = "Passthru Stencil Source Layer";  break;
   case stencilRuleCopperSourceLayer:   retval = "Copper Source Layer";   break;
   case stencilRuleInheritFromParent:   retval = "Inherit From Parent";   break;
   //case stencilRuleApertureModifier:    retval = "ApertureModifier";      break;
   }

   return CString(retval);
}

StencilAttributeTag intToStencilAttributeTag(int tagValue)
{
   StencilAttributeTag stencilAttributeTag = stencilAttributeUndefined;

   if (tagValue >= 0 && tagValue < stencilAttributeUndefined)
   {
      stencilAttributeTag = (StencilAttributeTag)tagValue;
   }

   return stencilAttributeTag;
}

StencilRuleTag intToStencilRuleTag(int tagValue)
{
   StencilRuleTag ruleTag = stencilRuleUndefined;

   if (tagValue >= 0 && tagValue < stencilRuleUndefined)
   {
      ruleTag = (StencilRuleTag)tagValue;
   }

   return ruleTag;
}

StencilRuleTag ruleStringToStencilRuleTag(const CString& incomingTagValue)
{
   // Case dts0100447220 rebug, Rule strings now may or may not have
   // double quotes around them. For purposes here strip the quotes.
   CString tagValue( incomingTagValue );
   tagValue.Remove('"');

   StencilRuleTag stencilRuleTag = stencilRuleUndefined;

   if (tagValue.CompareNoCase("None") == 0)
   {
      stencilRuleTag = stencilRuleNone;
   }
   else if (tagValue.CompareNoCase("Area") == 0)
   {
      stencilRuleTag = stencilRuleArea;
   }
   else if (tagValue.CompareNoCase("Inset") == 0)
   {
      stencilRuleTag = stencilRuleInset;
   }
   else if (tagValue.CompareNoCase("Aperture") == 0)
   {
      stencilRuleTag = stencilRuleAperture;
   }
   else if (tagValue.CompareNoCase("Aperture Geometry") == 0)
   {
      stencilRuleTag = stencilRuleApertureGeometry;
   }
   else if (tagValue.CompareNoCase("Stencil Source Layer") == 0)
   {
      stencilRuleTag = stencilRuleStencilSourceLayer;
   }
   else if (tagValue.CompareNoCase("Passthru Stencil Source Layer") == 0)
   {
      stencilRuleTag = stencilRulePassthru;
   }
   else if (tagValue.CompareNoCase("Copper Source Layer") == 0)
   {
      stencilRuleTag = stencilRuleCopperSourceLayer;
   }
   else if (tagValue.CompareNoCase("Inherit From Parent") == 0)
   {
      stencilRuleTag = stencilRuleInheritFromParent;
   }
   //else if (tagValue.CompareNoCase("ApertureModifier") == 0)
   //{
   //   stencilRuleTag = stencilRuleApertureModifier;
   //}

   return stencilRuleTag;
}

//_____________________________________________________________________________
CString stencilRuleModifierToString(StencilRuleModifierTag tagValue)
{
   const char* retval = "";

   switch (tagValue)
   {
   case stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline:
      retval = QAlignApertureMajorAxisTowardComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline:
      retval = QAlignApertureMajorAxisAwayFromComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMinorAxisTowardComponentCenterline:
      retval = QAlignApertureMinorAxisTowardComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMinorAxisAwayFromComponentCenterline:
      retval = QAlignApertureMinorAxisAwayFromComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMajorAxisParallelToComponentXAxis:
      retval = QAlignApertureMajorAxisParallelToComponentXAxis;
      break;
   case stencilRuleModifierAlignApertureMajorAxisParallelToComponentYAxis:
      retval = QAlignApertureMajorAxisParallelToComponentYAxis;
      break;
   case stencilRuleModifierAlignApertureMinorAxisParallelToComponentXAxis:
      retval = QAlignApertureMinorAxisParallelToComponentXAxis;
      break;
   case stencilRuleModifierAlignApertureMinorAxisParallelToComponentYAxis:
      retval = QAlignApertureMinorAxisParallelToComponentYAxis;
      break;
   case stencilRuleModifierOrientSymmetryAxis:
      retval = QOrientSymmetryAxis;
      break;
   case stencilRuleModifierFlipSymmetryAxis:
      retval = QFlipSymmetryAxis;
      break;
   case stencilRuleModifierOffsetRelativeToPadstack:
      retval = QOffsetRelativeToPadstack;
      break;
   case stencilRuleModifierWebSettings:
      retval = QStencilWebSettings;
      break;
   case stencilRuleModifierExteriorCornerRadius:
      retval = QStencilExteriorCornerRadius;
      break;
   }

   return CString(retval);
}

CString stencilRuleModifierToDescription(StencilRuleModifierTag tagValue)
{
   const char* retval = "";

   switch (tagValue)
   {
   case stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline:
      retval = QSAlignApertureMajorAxisTowardComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline:
      retval = QSAlignApertureMajorAxisAwayFromComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMinorAxisTowardComponentCenterline:
      retval = QSAlignApertureMinorAxisTowardComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMinorAxisAwayFromComponentCenterline:
      retval = QSAlignApertureMinorAxisAwayFromComponentCenterline;
      break;
   case stencilRuleModifierAlignApertureMajorAxisParallelToComponentXAxis:
      retval = QSAlignApertureMajorAxisParallelToComponentXAxis;
      break;
   case stencilRuleModifierAlignApertureMajorAxisParallelToComponentYAxis:
      retval = QSAlignApertureMajorAxisParallelToComponentYAxis;
      break;
   case stencilRuleModifierAlignApertureMinorAxisParallelToComponentXAxis:
      retval = QSAlignApertureMinorAxisParallelToComponentXAxis;
      break;
   case stencilRuleModifierAlignApertureMinorAxisParallelToComponentYAxis:
      retval = QSAlignApertureMinorAxisParallelToComponentYAxis;
      break;
   case stencilRuleModifierOrientSymmetryAxis:
      retval = QSOrientSymmetryAxis;
      break;
   case stencilRuleModifierFlipSymmetryAxis:
      retval = QSFlipSymmetryAxis;
      break;
   case stencilRuleModifierOffsetRelativeToPadstack:
      retval = QSOffsetRelativeToPadstack;
      break;
   case stencilRuleModifierWebSettings:
      retval = QSStencilWebSettings;
      break;
   case stencilRuleModifierExteriorCornerRadius:
      retval = QStencilExteriorCornerRadius;
      break;
   }

   return CString(retval);
}

StencilRuleModifierTag stencilRuleModifierDescriptionToStencilRuleModifierTag(const CString& tagValue)
{
   StencilRuleModifierTag stencilRuleModifierTag = stencilRuleModifierUndefined;

   if (tagValue.CompareNoCase(QSAlignApertureMajorAxisTowardComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMajorAxisAwayFromComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMinorAxisTowardComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisTowardComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMinorAxisAwayFromComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisAwayFromComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMajorAxisParallelToComponentXAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisParallelToComponentXAxis;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMajorAxisParallelToComponentYAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisParallelToComponentYAxis;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMinorAxisParallelToComponentXAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisParallelToComponentXAxis;
   }
   else if (tagValue.CompareNoCase(QSAlignApertureMinorAxisParallelToComponentYAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisParallelToComponentYAxis;
   }
   else if (tagValue.CompareNoCase(QSOrientSymmetryAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierOrientSymmetryAxis;
   }
   else if (tagValue.CompareNoCase(QSFlipSymmetryAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierFlipSymmetryAxis;
   }
   else if (tagValue.CompareNoCase(QSOffsetRelativeToPadstack) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierOffsetRelativeToPadstack;
   }
   else if (tagValue.CompareNoCase(QSStencilWebSettings) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierWebSettings;
   }
   else if (tagValue.CompareNoCase(QStencilExteriorCornerRadius) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierExteriorCornerRadius;
   }

   return stencilRuleModifierTag;
}

StencilRuleModifierTag stencilRuleModifierToStencilRuleModifierTag(const CString& tagValue)
{
   StencilRuleModifierTag stencilRuleModifierTag = stencilRuleModifierUndefined;

   if (tagValue.CompareNoCase(QAlignApertureMajorAxisTowardComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisTowardComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMajorAxisAwayFromComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisAwayFromComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMinorAxisTowardComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisTowardComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMinorAxisAwayFromComponentCenterline) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisAwayFromComponentCenterline;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMajorAxisParallelToComponentXAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisParallelToComponentXAxis;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMajorAxisParallelToComponentYAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMajorAxisParallelToComponentYAxis;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMinorAxisParallelToComponentXAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisParallelToComponentXAxis;
   }
   else if (tagValue.CompareNoCase(QAlignApertureMinorAxisParallelToComponentYAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierAlignApertureMinorAxisParallelToComponentYAxis;
   }
   else if (tagValue.CompareNoCase(QOrientSymmetryAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierOrientSymmetryAxis;
   }
   else if (tagValue.CompareNoCase(QFlipSymmetryAxis) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierFlipSymmetryAxis;
   }
   else if (tagValue.CompareNoCase(QOffsetRelativeToPadstack) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierOffsetRelativeToPadstack;
   }
   else if (tagValue.CompareNoCase(QStencilWebSettings) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierWebSettings;
   }
   else if (tagValue.CompareNoCase(QStencilExteriorCornerRadius) == 0)
   {
      stencilRuleModifierTag = stencilRuleModifierExteriorCornerRadius;
   }

   return stencilRuleModifierTag;
}

StencilRuleModifierTag stringToStencilRuleModifierTag(const CString& tagValue)
{
   StencilRuleModifierTag stencilRuleModifierTag = stencilRuleModifierToStencilRuleModifierTag(tagValue);

   if (stencilRuleModifierTag == stencilRuleModifierUndefined)
   {
      stencilRuleModifierTag = stencilRuleModifierDescriptionToStencilRuleModifierTag(tagValue);
   }

   return stencilRuleModifierTag;
}

StencilRuleModifierTag intToStencilRuleModifierTag(int tagValue)
{
   StencilRuleModifierTag stencilRuleModifierTag = stencilRuleModifierUndefined;

   if (tagValue >= stencilRuleModifierFirst && tagValue <= stencilRuleModifierLast)
   {
      stencilRuleModifierTag = (StencilRuleModifierTag)tagValue;
   }

   return stencilRuleModifierTag;
}

bool ruleAllowsModifier(StencilRuleTag stencilRule,StencilRuleModifierTag ruleModifier)
{
   bool retval = false;

   switch (ruleModifier)
   {
   case stencilRuleModifierOrientSymmetryAxis:
      retval = (stencilRule == stencilRuleAperture);
      break;

   case stencilRuleModifierOffsetRelativeToPadstack:
      retval = ((stencilRule == stencilRuleAperture) ||
                (stencilRule == stencilRuleApertureGeometry)   );
      break;
   case stencilRuleModifierFlipSymmetryAxis:
      retval = ((stencilRule == stencilRuleAperture) ||
                (stencilRule == stencilRuleApertureGeometry)   );
      break;
   case stencilRuleModifierWebSettings:
   case stencilRuleModifierExteriorCornerRadius:
      retval = ((stencilRule == stencilRuleArea) ||
                (stencilRule == stencilRuleInset) ||
                (stencilRule == stencilRuleAperture) ||
                (stencilRule == stencilRuleApertureGeometry) ||
                (stencilRule == stencilRuleCopperSourceLayer) ||
                (stencilRule == stencilRuleStencilSourceLayer)   );
      break;
   }

   return retval;
}

//_____________________________________________________________________________
CString stencilOrientationModifierToString(StencilOrientationModifierTag tagValue)
{
   const char* retval = "";

   switch (tagValue)
   {
   case orientationTowardCenter:
      retval = QTowardCenter;
      break;
   case orientationAwayFromCenter:
      retval = QAwayFromCenter;
      break;
   case orientationNormal:
      retval = QNormal;
      break;
   }

   return CString(retval);
}

StencilOrientationModifierTag stringToStencilOrientationModifierTag(const CString& tagValue)
{
   StencilOrientationModifierTag stencilOrientationModifierTag = orientationUndefined;

   if (tagValue.CompareNoCase(QTowardCenter) == 0)
   {
      stencilOrientationModifierTag = orientationTowardCenter;
   }
   else if (tagValue.CompareNoCase(QAwayFromCenter) == 0)
   {
      stencilOrientationModifierTag = orientationAwayFromCenter;
   }
   else if (tagValue.CompareNoCase(QNormal) == 0)
   {
      stencilOrientationModifierTag = orientationNormal;
   }

   return stencilOrientationModifierTag;
}

//_____________________________________________________________________________
CString stencilSplitTypeToString(StencilSplitTypeTag tagValue)
{
   const char* retval = "";

   switch (tagValue)
   {
   case splitTypeGrid:
      retval = QGrid;
      break;
   case splitTypeStripe:
      retval = QStripe;
      break;
   }

   return CString(retval);
}

StencilSplitTypeTag stringToSplitTypeTag(const CString& tagValue)
{
   StencilSplitTypeTag splitTypeTag = splitTypeUndefined;

   if (tagValue.CompareNoCase(QGrid) == 0)
   {
      splitTypeTag = splitTypeGrid;
   }
   else if (tagValue.CompareNoCase(QStripe) == 0)
   {
      splitTypeTag = splitTypeStripe;
   }

   return splitTypeTag;
}

//_____________________________________________________________________________
CString getFirstPinName(BlockStruct& componentGeometry,DataStruct& padstackInsert)
{
   CString firstPinName,sortableFirstPinName;
   int padstackBlockNumber = padstackInsert.getInsert()->getBlockNumber();

   for (CDataListIterator insertPinIterator(componentGeometry,CInsertTypeMask(insertTypePin,insertTypeMechanicalPin));insertPinIterator.hasNext();)
   {
      DataStruct* pin = insertPinIterator.getNext();

      if (pin->getInsert()->getBlockNumber() == padstackBlockNumber)
      {
         CString pinName = pin->getInsert()->getRefname();
         CString sortablePinName = CompPinStruct::getSortableReference(pinName);

         if (firstPinName.IsEmpty() || sortablePinName.Compare(sortableFirstPinName) < 0)
         {
            sortableFirstPinName = sortablePinName;
            firstPinName = pinName;
         }
      }
   }

   return firstPinName;
}

//_____________________________________________________________________________
CString getStencilRuleString(CCamCadDatabase& camCadDatabase,CComponentPin& componentPin)
{
   CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,componentPin.getDefinedAttributes());
   CString ruleString = stencilRuleAttributes.getRule().getRuleString();

   return ruleString;
}

//_____________________________________________________________________________
CComponentPin::CComponentPin(CCamCadDatabase& camCadDatabase,BlockStruct& pinsVesselGeometry,
   const CString& refDes,const CString& pinName,DataStruct* pinVessel)
: m_camCadDatabase(camCadDatabase)
, m_entityNumber(AllocateEntityNumber())
, m_refDes(refDes)
, m_pinName(pinName)
{
   BlockStruct* pinVesselGeometry = camCadDatabase.getDefinedBlock("$ComponentPinVessel$",blockTypeUnknown);

   if (pinVessel != NULL)
   {
      m_pinVessel = pinVessel;
   }
   else
   {
      m_pinVessel = newDataStruct(dataTypeInsert);
      m_pinVessel->setLayerIndex(camCadDatabase.getFloatingLayerIndex());
      m_pinVessel->getInsert()->setBlockNumber(pinVesselGeometry->getBlockNumber());
      m_pinVessel->getInsert()->setRefname(getPinRef());
   }

   pinsVesselGeometry.getDataList().AddTail(m_pinVessel);
}

CComponentPin::~CComponentPin()
{
}

CCamCadData& CComponentPin::getCamCadData()
{
   return m_camCadDatabase.getCamCadData();
}

CString CComponentPin::getPinRef() const
{
   return CompPinStruct::getPinRef(m_refDes,m_pinName);
}

//_____________________________________________________________________________
CComponentPinMap::CComponentPinMap(CCamCadDatabase& camCadDatabase,FileStruct& file)
: m_camCadDatabase(camCadDatabase)
, m_file(file)
{
   m_componentPins       = NULL;

   m_componentPinsVessel = NULL;
   m_componentPinsVesselGeometry = camCadDatabase.getDefinedBlock("$ComponentPinsVessel$",blockTypeUnknown,file.getFileNumber());

   // search for Components pin vessel in the file
   for (CDataListIterator insertIterator(*(file.getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* insertData = insertIterator.getNext();

      if (insertData->getInsert()->getBlockNumber() == m_componentPinsVesselGeometry->getBlockNumber())
      {
         m_componentPinsVessel = insertData;
         break;
      }
   }

   // create Components pin vessel if it doesn't exist
   if (m_componentPinsVessel == NULL)
   {
      m_componentPinsVessel = newDataStruct(dataTypeInsert);
      m_componentPinsVessel->setLayerIndex(camCadDatabase.getFloatingLayerIndex());
      m_componentPinsVessel->getInsert()->setBlockNumber(m_componentPinsVesselGeometry->getBlockNumber());

      file.getBlock()->getDataList().AddTail(m_componentPinsVessel);
   }
}

CComponentPinMap::~CComponentPinMap()
{
   delete m_componentPins;
}

CCamCadData& CComponentPinMap::getCamCadData()
{
   return m_camCadDatabase.getCamCadData();
}

void CComponentPinMap::resync()
{
   delete m_componentPins;
   int numPins = 0;
   int geometryPinCount;
   void* geometryPinCountVoid;
   CMapPtrToPtr geometryPinCountMap;
   geometryPinCountMap.InitHashTable(nextPrime2n(1000));

   // count the number of pin instances in the file
   for (CDataListIterator componentIterator(*(m_file.getBlock()),insertTypePcbComponent);componentIterator.hasNext();)
   {
      DataStruct* component = componentIterator.getNext();
      int geometryBlockNumber = component->getInsert()->getBlockNumber();

      if (geometryPinCountMap.Lookup((void*)geometryBlockNumber,geometryPinCountVoid))
      {
         geometryPinCount = (int)geometryPinCountVoid;
      }
      else
      {
         BlockStruct* geometry = m_camCadDatabase.getBlock(geometryBlockNumber);
         geometryPinCount = 0;

         for (CDataListIterator insertPinIterator(*geometry,CInsertTypeMask(insertTypePin,insertTypeMechanicalPin));
              insertPinIterator.hasNext();)
         {
            DataStruct* pin = insertPinIterator.getNext();

            geometryPinCount++;
         }

         geometryPinCountMap.SetAt((void*)geometryBlockNumber,(void*)geometryPinCount);
      }

      numPins += geometryPinCount;
   }

   // create the pins arrayWithMap with the optimum size
   int size = max(100,(int)(1.3 * numPins));

   m_componentPins = new CTypedPtrArrayWithMap<CComponentPin>(size,true);
   CComponentPin* componentPin;

   // move the existing data on the component pins vessel geometry to a temp data list
   // and create a map to access current rule attributes
   CDataList oldPinVesselDataList(true);
   oldPinVesselDataList.takeData(m_componentPinsVesselGeometry->getDataList());
   CTypedMapStringToPtrContainer<DataStruct*> oldComponentPinMap(nextPrime2n(oldPinVesselDataList.GetCount()),true);

   for (POSITION pos = oldPinVesselDataList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      DataStruct* oldPin = oldPinVesselDataList.GetNext(pos);

      if (oldPin->getDataType() == dataTypeInsert)
      {
         DataStruct* existingPin;
         CString pinRef = oldPin->getInsert()->getRefname();

         if (!oldComponentPinMap.Lookup(pinRef,existingPin))
         {
            oldPinVesselDataList.RemoveAt(oldPos);
            oldComponentPinMap.SetAt(pinRef,oldPin);
         }
      }
   }

   // scan the file for pins and put the pins in the pins arrayWithMap
   for (CDataListIterator componentIterator(*(m_file.getBlock()),insertTypePcbComponent);componentIterator.hasNext();)
   {
      DataStruct* component = componentIterator.getNext();
      BlockStruct* geometry = m_camCadDatabase.getBlock(component->getInsert()->getBlockNumber());
      CString refDes = component->getInsert()->getRefname();

      for (CDataListIterator insertPinIterator(*geometry,CInsertTypeMask(insertTypePin,insertTypeMechanicalPin));
           insertPinIterator.hasNext();)
      {
         DataStruct* pin = insertPinIterator.getNext();

         CString pinName = pin->getInsert()->getRefname();
         CString pinRef = CompPinStruct::getPinRef(refDes,pinName);

         if (!m_componentPins->lookup(pinRef,componentPin))
         {
            // look up old attributes
            DataStruct* oldPin = NULL;

            if (oldComponentPinMap.Lookup(pinRef,oldPin))
            {
               oldComponentPinMap.RemoveKey(pinRef);
            }

            // for backwards compatibility, salvage and then remove rules from compPins
            CCamCadPin* camCadPin = m_camCadDatabase.getCamCadPin(&m_file,refDes,pinName);

            if (camCadPin != NULL)
            {
               CStencilRuleAttributes compPinStencilRuleAttributes(m_camCadDatabase,camCadPin->getCompPin()->getAttributes());

               if (oldPin != NULL && oldPin->getAttributes() != NULL)
               {
                  CStencilRuleAttributes stencilRuleAttributes(m_camCadDatabase,oldPin->getAttributes());

                  if (!stencilRuleAttributes.hasRule() && compPinStencilRuleAttributes.hasRule())
                  {
                     stencilRuleAttributes.setRule(compPinStencilRuleAttributes.getRule());
                  }
               }

               compPinStencilRuleAttributes.empty();
            }

            componentPin = new CComponentPin(m_camCadDatabase,*m_componentPinsVesselGeometry,refDes,pinName,oldPin);
            m_componentPins->add(pinRef,componentPin);
         }
      }
   }
}

// returns true if the arrays are in sync
bool CComponentPinMap::sync()
{
   bool retval = (m_componentPins != NULL && m_componentPins->getSize() > 0);

   if (! retval)
   {
      resync();
   }

   return retval;
}

CComponentPin* CComponentPinMap::getDefinedComponentPin(const CString& refDes,const CString& pinName)
{
   CComponentPin* componentPin = NULL;

   sync();

   if (!m_componentPins->lookup(CompPinStruct::getPinRef(refDes,pinName),componentPin))
   {
      componentPin = NULL;
   }

   if (componentPin == NULL)
   {
      componentPin = new CComponentPin(m_camCadDatabase,*m_componentPinsVesselGeometry,refDes,pinName);
      m_componentPins->add(componentPin->getPinRef(),componentPin);
   }

   return componentPin;
}

int CComponentPinMap::getSize()
{
   sync();

   int size = m_componentPins->getSize();

   return size;
}

CComponentPin* CComponentPinMap::getAt(int index)
{
   sync();

   CComponentPin* pin = m_componentPins->getAt(index);

   return pin;
}

//_____________________________________________________________________________
CComponentPinDirectory::CComponentPinDirectory()
{
}

CComponentPinDirectory::~CComponentPinDirectory()
{
}

void CComponentPinDirectory::empty()
{
   m_componentPinMapArray.empty();
}

CComponentPin* CComponentPinDirectory::getDefinedComponentPin(
   CCamCadDatabase& camCadDatabase,FileStruct& file,
   const CString& refDes,const CString& pinName)
{
   CComponentPinMap* pinMap = getAt(camCadDatabase,file);
   CComponentPin* componentPin = pinMap->getDefinedComponentPin(refDes,pinName);

   return componentPin;
}

CComponentPinMap* CComponentPinDirectory::getAt(CCamCadDatabase& camCadDatabase,FileStruct& file)
{
   CComponentPinMap* pinMap = NULL;

   if (file.getFileNumber() < m_componentPinMapArray.GetSize())
   {
      pinMap = m_componentPinMapArray.GetAt(file.getFileNumber());
   }

   if (pinMap == NULL)
   {
      pinMap = new CComponentPinMap(camCadDatabase,file);
      m_componentPinMapArray.SetAtGrow(file.getFileNumber(),pinMap);
   }

   return pinMap;
}

void CComponentPinDirectory::discardComponentPins(FileStruct& file)
{
   CComponentPinMap* pinMap = NULL;

   if (file.getFileNumber() < m_componentPinMapArray.GetSize())
   {
      pinMap = m_componentPinMapArray.GetAt(file.getFileNumber());
   }

   if (pinMap != NULL)
   {
      delete pinMap;
      m_componentPinMapArray.SetAt(file.getFileNumber(),NULL);
   }
}

//_____________________________________________________________________________
CCompositePad::CCompositePad(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase),
   m_pads(true)
{
   m_pad = NULL;
   //m_rotationModifierRadians = 0.;
}

CCompositePad::~CCompositePad()
{
   delete m_pad;
}

void CCompositePad::empty()
{
   setPad(NULL);
   m_pads.empty();
}

void CCompositePad::addPad(DataStruct* pad)
{
   setPad(NULL);

   if (pad != NULL)
   {
      m_pads.AddTail(pad);
   }
}

void CCompositePad::setPad(DataStruct* pad)
{
   delete m_pad;
   m_pad = pad;
}

int CCompositePad::getCount()
{
   return m_pads.GetCount();
}

//#define ORIGINAL

bool CCompositePad::isComplex()
{
   bool retval = (m_pads.GetCount() > 1);

   if (m_pads.GetCount() == 1)
   {
#ifdef ORIGINAL
      retval = !isSimple();
#else
      DataStruct* pad = m_pads.GetHead();
      BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());

      retval = padGeometry->isComplexAperture();
#endif
   }

   return retval;
}

bool CCompositePad::isSimple()
{
   bool retval = false;

   if (m_pads.GetCount() == 1)
   {
      DataStruct* pad = m_pads.GetHead();
      BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());
#ifdef ORIGINAL
      retval = ((padGeometry->getShape() != T_UNDEFINED) && (padGeometry->getShape() != T_COMPLEX));
#else
      retval = padGeometry->isSimpleAperture();
#endif
   }

   return retval;
}

BlockStruct* CCompositePad::getNormalizedAperture(BlockStruct& aperture)
{
   BlockStruct* normalizedAperture = NULL;

   if (aperture.isAperture())
   {
      if (aperture.getXoffset() == 0. && aperture.getYoffset() == 0. && aperture.getRotationDegrees() == 0.)
      {
         normalizedAperture = &aperture;
      }
      else if (aperture.isSimpleAperture())
      {
         CStandardAperture standardAperture(&aperture,m_camCadDatabase.getPageUnits());
         standardAperture.setRotationRadians(0.);
         standardAperture.detachApertureBlock();

         normalizedAperture = standardAperture.getDefinedAperture(camCadDatabase_camCadData);
      }
      else  // complex aperture
      {
         CString apertureName = aperture.getName();

         if (apertureName.IsEmpty())
         {
            apertureName.Format("AP_geometry_%d",aperture.getBlockNumber());
         }

         apertureName += "_normalized";

         normalizedAperture = m_camCadDatabase.getBlock(apertureName);

         if (normalizedAperture == NULL)
         {
            BlockStruct* complexApertureGeometry = m_camCadDatabase.getBlock(aperture.getComplexApertureSubBlockNumber());
            CString normalizedApertureGeometryName = apertureName + "-Cmplx";
            BlockStruct* normalizedApertureGeometry = m_camCadDatabase.getBlock(normalizedApertureGeometryName);

            if (normalizedApertureGeometry == NULL)
            {
               normalizedApertureGeometry = m_camCadDatabase.copyBlock(normalizedApertureGeometryName,complexApertureGeometry);

               CTMatrix matrix = complexApertureGeometry->getApertureTMatrix();
               normalizedApertureGeometry->transform(matrix);
            }

            normalizedAperture = m_camCadDatabase.copyBlock(apertureName,&aperture);
            normalizedAperture->setXoffset(0.);
            normalizedAperture->setYoffset(0.);
            normalizedAperture->setRotationRadians(0.);
            normalizedAperture->setComplexApertureSubBlockNumber(normalizedApertureGeometry->getBlockNumber());
         }
      }
   }

   return normalizedAperture;
}

DataStruct* CCompositePad::getNormalizedPad(DataStruct& pad)
{
   DataStruct* normalizedPad;

   BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad.getInsert()->getBlockNumber());
   BlockStruct* normalizedPadGeometry = getNormalizedAperture(*padGeometry);

   normalizedPad = newDataStruct(pad);

   if (normalizedPadGeometry != NULL)
   {
      normalizedPad->getInsert()->setBlockNumber(normalizedPadGeometry->getBlockNumber());
   }

   CBasesVector basesVector;
   CTMatrix matrix = padGeometry->getApertureTMatrix() * pad.getInsert()->getTMatrix();
   basesVector.transform(matrix);

   normalizedPad->getInsert()->setBasesVector(basesVector);

   return normalizedPad;
}

DataStruct* CCompositePad::getPad()
{
   if (m_pad == NULL && m_pads.GetCount() > 0)
   {
      if (m_pads.GetCount() == 1)
      {
         DataStruct* pad = getNormalizedPad(*(m_pads.GetHead()));

         setPad(pad);
      }
      else
      {
         // knv 20040829
         // 1) need to create new geometries for apertures with nonzero offsets or rotations.
         // 2) create new block name composed of signature of standardAperture descriptor
         //    with offsets and rotation.

         // may want to cache new geometries in the future
         BlockStruct* padGeometry = m_camCadDatabase.getNewBlock("Stencil_","%d",blockTypeUnknown);
         int floatingLayerIndex = m_camCadDatabase.getFloatingLayerIndex();

         DataStruct* firstPad = m_pads.GetHead();
         InsertStruct* firstPadInsert = firstPad->getInsert();

         CTMatrix matrix = firstPadInsert->getTMatrix();

         //matrix.translateCtm(firstPadInsert->getOrigin2d());
         //matrix.rotateRadiansCtm(firstPadInsert->getAngle());
         //matrix.scaleCtm(((firstPadInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1 : 1,1);
         matrix.invert();
         //matrix.rotateRadians(m_rotationModifierRadians);

         for (POSITION padPos = m_pads.GetHeadPosition();padPos != NULL;)
         {
            DataStruct* pad = m_pads.GetNext(padPos);
            DataStruct* xfPad = getNormalizedPad(*pad);
            xfPad->transform(matrix);
            padGeometry->getDataList().AddTail(xfPad);
         }

         DataStruct* pad = m_camCadDatabase.insertBlock(padGeometry,INSERTTYPE_FREEPAD,"",floatingLayerIndex,
                              firstPadInsert->getOriginX(),firstPadInsert->getOriginY(),
                              firstPadInsert->getAngle(),((firstPadInsert->getMirrorFlags() & MIRROR_FLIP) != 0));
         setPad(pad);
      }
   }

   return m_pad;
}

DataStruct* CCompositePad::getPad(BlockStruct& padStackGeometry,CDataList& padList)
{
   if (m_pad == NULL && padList.GetCount() > 0)
   {
      m_pads.empty();

      if (padList.GetCount() == 1)
      {
         m_pads.AddTail(newDataStruct(*(padList.GetHead())));
         getPad();
      }
      else
      {
         padList.sortByAscendingEntityNumber();

         CString geometryName(padStackGeometry.getName());

         for (POSITION pos = padList.GetHeadPosition();pos != NULL;)
         {
            DataStruct* pad = padList.GetNext(pos);

            geometryName.AppendFormat("_%d",pad->getEntityNumber());

            //m_pads.AddTail(newDataStruct(*pad));
         }

         BlockStruct* padGeometry = m_camCadDatabase.getBlock(geometryName);
         DataStruct* firstPad = padList.GetHead();
         InsertStruct* firstPadInsert = firstPad->getInsert();

         if (padGeometry == NULL)
         {
            padGeometry = m_camCadDatabase.getDefinedBlock(geometryName,blockTypeUnknown);

            CTMatrix matrix;

            matrix.translateCtm(firstPadInsert->getOrigin2d());
            matrix.rotateRadiansCtm(firstPadInsert->getAngle());
            matrix.scaleCtm(((firstPadInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1 : 1,1);
            matrix.invert();
            //matrix.rotateRadians(m_rotationModifierRadians);

            for (POSITION padPos = padList.GetHeadPosition();padPos != NULL;)
            {
               DataStruct* pad = padList.GetNext(padPos);
               DataStruct* xfPad = newDataStruct(*pad);
               xfPad->transform(matrix);
               padGeometry->getDataList().AddTail(xfPad);
            }
         }

         //CString apertureName("AP_" + geometryName);

         //int err;
         //int apertureIndex = m_camCadDatabase.getCamCadDoc().Graph_Aperture(apertureName,apertureComplex,padGeometry->getBlockNumber(),0.,0.,0.,0.,0,0,0,&err);
         //BlockStruct* aperture = m_camCadDatabase.getCamCadDoc().getWidthBlock(apertureIndex);
         //m_camCadDatabase.getCamCadDoc().CalcBlockExtents(aperture);

         //int floatingLayerIndex = m_camCadDatabase.getFloatingLayerIndex();

         //DataStruct* pad = m_camCadDatabase.insertBlock(aperture,insertTypeFreePad,"",floatingLayerIndex,
         //                     firstPadInsert->getOriginX(),firstPadInsert->getOriginY(),
         //                     firstPadInsert->getAngle(),((firstPadInsert->getMirrorFlags() & MIRROR_FLIP) != 0));

         int floatingLayerIndex = m_camCadDatabase.getFloatingLayerIndex();

         DataStruct* pad = m_camCadDatabase.insertBlock(padGeometry,insertTypeFreePad,"",floatingLayerIndex,
                              firstPadInsert->getOriginX(),firstPadInsert->getOriginY(),
                              firstPadInsert->getAngle(),((firstPadInsert->getMirrorFlags() & MIRROR_FLIP) != 0));
         setPad(pad);
      }
   }

   return m_pad;
}

BlockStruct* CCompositePad::getPadGeometry()
{
   BlockStruct* padGeometry = NULL;

   if (getPad() != NULL)
   {
      padGeometry = m_camCadDatabase.getBlock(m_pad->getInsert()->getBlockNumber());
   }

   return padGeometry;
}

//void CCompositePad::setPadGeometry(BlockStruct* aperture)
//{
//   getPad();
//
//   if (m_pad != NULL)
//   {
//      DataStruct* allocatedPad;
//      allocatedPad = newDataStruct(*m_pad);
//      allocatedPad->getInsert()->setBlockNumber(aperture->getBlockNumber());
//
//      m_pads.empty();
//      m_pads.AddTail(allocatedPad);
//
//      setPad(NULL);
//   }
//}

//void CCompositePad::setPad(CStandardAperture& standardAperture,bool overridePadGeometryRotation,double padGeometryRotationRadians)
//{
//   if (standardAperture.isStandardAperture())
//   {
//      CStandardAperture standardApertureCopy(standardAperture);
//      standardApertureCopy.setRotationRadians(0.);
//      BlockStruct* apertureGeometry = standardApertureCopy.getDefinedAperture(m_camCadDatabase);
//
//      if (apertureGeometry != NULL)
//      {
//         BlockStruct* padGeometry = getPadGeometry();
//         DataStruct* pad = getPad();
//         CTMatrix matrix = pad->getInsert()->getTMatrix();
//
//         //m_rotationModifierRadians = rotationModifierRadians;
//         setPadGeometry(apertureGeometry);
//         pad = getPad();
//
//         // compensate for offset pads
//         if (!overridePadGeometryRotation)
//         {
//            padGeometryRotationRadians = padGeometry->getRotationRadians();
//         }
//
//         matrix.rotateRadiansCtm(padGeometryRotationRadians);
//         matrix.translateCtm(padGeometry->getXoffset(),padGeometry->getYoffset());
//
//         CBasesVector newBasesVector;
//         newBasesVector.transform(matrix);
//         pad->getInsert()->setBasesVector(newBasesVector);
//
//         //pad->getInsert()->incAngle(m_rotationModifierRadians);
//      }
//   }
//}

bool CCompositePad::insetPadPerimeter(double inset,double exteriorCornerRadius,CTMatrix* padMatrix)
{
   static bool useFlattenPadGeometryFlag = true;
   bool retval = true;

   BlockStruct* padGeometry = getPadGeometry();

   if (padGeometry != NULL)
   {
      if (isSimple())
      {
         CStandardAperture standardAperture(padGeometry,m_camCadDatabase.getPageUnits());
         standardAperture.setRotationRadians(0.);
         standardAperture.setExteriorCornerRadius(exteriorCornerRadius);

         retval = standardAperture.inset(inset);

         BlockStruct* newPadGeometry = standardAperture.getDefinedAperture(camCadDatabase_camCadData);
         m_pad->getInsert()->setBlockNumber(newPadGeometry->getBlockNumber());
      }
      else if (useFlattenPadGeometryFlag)
      {
         CString complexApertureName;
         complexApertureName.Format("%s(inset %s)",padGeometry->getName(),fpfmt(inset,5));

         BlockStruct* scaledComplexApertureBlock = m_camCadDatabase.getBlock(complexApertureName);

         if (scaledComplexApertureBlock == NULL)
         {
            scaledComplexApertureBlock = m_camCadDatabase.getNewBlock(complexApertureName,"%d",padGeometry->getBlockType());
            DataStruct* polyStruct = newDataStruct(dataTypePoly);
            polyStruct->setLayerIndex(m_camCadDatabase.getFloatingLayerIndex());
            scaledComplexApertureBlock->getDataList().AddTail(polyStruct);

            CTMatrix matrix;
            flattenPadGeometry(scaledComplexApertureBlock->getDataList(), *(polyStruct->getPolyList()), *padGeometry, matrix, exteriorCornerRadius, inset);
         }

         m_pad->getInsert()->setBlockNumber(scaledComplexApertureBlock->getBlockNumber());
         m_pad->setLayerIndex(m_camCadDatabase.getFloatingLayerIndex());
      }
      else
      {
         int widthIndex;
         double totalInset = inset + exteriorCornerRadius;
         double tolerance = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters,.00001);

         if (exteriorCornerRadius > 0.)
         {
            widthIndex = m_camCadDatabase.getDefinedWidthIndex(exteriorCornerRadius*2.);
         }
         else
         {
            widthIndex = m_camCadDatabase.getZeroWidthIndex();
         }

         if (padGeometry->getShape() == apertureComplex)
         {
            CString complexApertureName;
            BlockStruct* complexApertureBlock = m_camCadDatabase.getBlock(padGeometry->getComplexApertureSubBlockNumber());
            complexApertureName.Format("%s(inset %s)",complexApertureBlock->getName(),fpfmt(inset,5));

            if (exteriorCornerRadius > 0.)
            {
               complexApertureName.AppendFormat("-w%d",widthIndex);
            }

            BlockStruct* insetComplexApertureBlock = m_camCadDatabase.getBlock(complexApertureName);

            if (insetComplexApertureBlock == NULL)
            {
               insetComplexApertureBlock = m_camCadDatabase.getNewBlock(complexApertureName,"%d",complexApertureBlock->getBlockType());

               for (CDataListIterator dataIterator(*complexApertureBlock,dataTypePoly);dataIterator.hasNext();)
               {
                  DataStruct* polyStruct = dataIterator.getNext();
                  LayerStruct* layer = m_camCadDatabase.getLayerAt(polyStruct->getLayerIndex());

                  if (layer->isFloating())
                  {
                     DataStruct* insetPolyStruct = newDataStruct(*polyStruct);

                     insetComplexApertureBlock->getDataList().AddTail(insetPolyStruct);

                     CPolyList* polyList = insetPolyStruct->getPolyList();

                     for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
                     {
                        POSITION oldPolyPos = polyPos;
                        CPoly* poly = polyList->GetNext(polyPos);

                        if (poly->isClosed())
                        {
                           retval = (poly->ShrinkPoly(totalInset,m_camCadDatabase.getPageUnits()) && retval);
                           poly->clean(tolerance);
                           poly->setWidthIndex(widthIndex);
                        }
                        else
                        {
                           polyList->deleteAt(oldPolyPos);
                           poly = NULL;
                        }
                     }
                  }
               }
            }

            m_pad->getInsert()->setBlockNumber(insetComplexApertureBlock->getBlockNumber());
            m_pad->setLayerIndex(m_camCadDatabase.getFloatingLayerIndex());
         }
         else
         {
            for (CDataListIterator dataIterator(*padGeometry,dataTypePoly);dataIterator.hasNext();)
            {
               DataStruct* polyStruct = dataIterator.getNext();
               CPolyList* polyList = polyStruct->getPolyList();

               for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
               {
                  CPoly* poly = polyList->GetNext(polyPos);

                  if (poly->isClosed())
                  {
                     retval = (poly->ShrinkPoly(inset,m_camCadDatabase.getPageUnits()) && retval);
                     poly->clean(tolerance);
                     poly->setWidthIndex(widthIndex);
                  }
               }
            }
         }
      }

      if (padMatrix == NULL)
      {
         // compensate for offset pads
         CTMatrix matrix = m_pad->getInsert()->getTMatrix();
         matrix.rotateRadiansCtm(padGeometry->getRotationRadians());
         matrix.translateCtm(padGeometry->getXoffset(),padGeometry->getYoffset());
         CBasesVector newBasesVector;
         newBasesVector.transform(matrix);
         m_pad->getInsert()->setBasesVector(newBasesVector);
      }
      else
      {
         *padMatrix = padGeometry->getApertureTMatrix();
      }
   }

   return retval;
}

bool CCompositePad::flattenPadGeometry(CDataList& parentDataList,CPolyList& polyList,BlockStruct& padGeometry,
   CTMatrix matrix,double exteriorCornerRadius,double inset)
{
   bool retval = true;
   //double scaleFactor = matrix.getScale();

   if (padGeometry.isSimpleAperture())
   {
      CStandardAperture standardAperture(&padGeometry,m_camCadDatabase.getPageUnits());
      standardAperture.setRotationRadians(0.);
      standardAperture.setExteriorCornerRadius(exteriorCornerRadius/* / scaleFactor*/);
      standardAperture.integrateExteriorCornerRadius();

      retval = standardAperture.inset(inset);

      DataStruct* polyStruct = m_camCadDatabase.addPolyStruct(parentDataList,m_camCadDatabase.getFloatingLayerIndex(),0,false,graphicClassNormal);

      standardAperture.getAperturePoly(camCadDatabase_camCadData,parentDataList,polyStruct);

      CBasesVector apertureBasesVector = padGeometry.getApertureBasesVector();
      CTMatrix mat2 = apertureBasesVector.getTransformationMatrix() * matrix;
      polyStruct->transform(mat2);
   }
   else if (padGeometry.isComplexAperture())
   {
      BlockStruct* complexApertureBlock = m_camCadDatabase.getBlock(padGeometry.getComplexApertureSubBlockNumber());

      CBasesVector apertureBasesVector = padGeometry.getApertureBasesVector();
      CTMatrix mat2 = apertureBasesVector.getTransformationMatrix() * matrix;

      flattenPadGeometry(parentDataList, polyList, *complexApertureBlock, mat2, exteriorCornerRadius, inset);
   }
   else
   {
      for (CDataListIterator dataIterator(padGeometry);dataIterator.hasNext();)
      {
         DataStruct* data = dataIterator.getNext();

         if (data->getDataType() == dataTypeInsert)
         {
            CTMatrix mat2 = data->getInsert()->getTMatrix() * matrix;
            BlockStruct* geometry = m_camCadDatabase.getBlock(data->getInsert()->getBlockNumber());

            flattenPadGeometry(parentDataList, polyList, *geometry, mat2, exteriorCornerRadius, inset);
         }
         else if (data->getDataType() == dataTypePoly)
         {
            CPolyList& dataPolyList = *(data->getPolyList());
            const double tolerance = .0001;
            double totalShrink = inset + exteriorCornerRadius;

            for (POSITION polyPos = dataPolyList.GetHeadPosition();polyPos != NULL;)
            {
               CPoly* poly = dataPolyList.GetNext(polyPos);
               CPoly* newPoly = new CPoly(*poly);
               newPoly->transform(matrix,&m_camCadDatabase.getCamCadDataDoc());

               if (! newPoly->isClosed())
               {
                  // The exteriorCornerRadius is implemented simply by drawing the poly outline with a round aperture
                  // of appropriate size. So the poly has to be shrunk by the width of the inset plus the width of
                  // the exteriorCOrnerRadius. The poly is then set to be drawn with the exteriorCornerRadius width
                  // to bring it back out to its full size.
                  // The commented out lines //-1- and //-2- would make polys of appropriate overall size for normal
                  // polys, but being set to zeroWidth they would lose the exteriorCornerRadius.
                  // For apertures that go on to become split-apertures, the split processes resets the drawing aperture
                  // to the exteriorCornerRadius, which then results in polys that are drawn too large.

                  //-1- double width = m_camCadDatabase.getCamCadDoc().getWidth(newPoly->getWidthIndex()) - 2. * inset;
                  double width = m_camCadDatabase.getCamCadDoc().getWidth(newPoly->getWidthIndex()) - (2. * (inset + exteriorCornerRadius));

                  if (width < 0.)
                  {
                     retval = false;
                     delete newPoly;
                     newPoly = NULL;
                  }
                  else
                  {
                     //-2- newPoly->convertToOutline(width,tolerance,m_camCadDatabase.getZeroWidthIndex());
                     int cornerWidthIndex    = m_camCadDatabase.getDefinedWidthIndex(exteriorCornerRadius * 2.);
                     newPoly->closePolyWithCoincidentEndPoints();
                     newPoly->convertToOutline(width,tolerance,cornerWidthIndex);

                     newPoly->setFilled(true);
                  }
               }
               else if (totalShrink != 0.)
               {
                  if (! newPoly->ShrinkPoly(totalShrink,m_camCadDatabase.getPageUnits()))
                  {
                     retval = false;
                     delete newPoly;
                     newPoly = NULL;
                  }
               }

               if (newPoly != NULL)
               {
                  int cornerWidthIndex    = m_camCadDatabase.getDefinedWidthIndex(exteriorCornerRadius * 2.);
                  newPoly->setWidthIndex(cornerWidthIndex);

                  polyList.AddTail(newPoly);
               }
            }
         }
      }
   }

   return retval;
}

void CCompositePad::scalePadArea(double areaScaleFactor,double exteriorCornerRadius)
{
   static bool useFlattenPadGeometryFlag = true;
   double linearScaleFactor = sqrt(areaScaleFactor);
   BlockStruct* padGeometry = getPadGeometry();

   if (padGeometry != NULL)
   {
      if (isSimple())
      {
         CStandardAperture standardAperture(padGeometry,m_camCadDatabase.getPageUnits());
         standardAperture.setRotationRadians(0.);
         standardAperture.setExteriorCornerRadius(exteriorCornerRadius);
         standardAperture.integrateExteriorCornerRadius();

         standardAperture.scale(linearScaleFactor);

         BlockStruct* newPadGeometry = standardAperture.getDefinedAperture(camCadDatabase_camCadData);
         m_pad->getInsert()->setBlockNumber(newPadGeometry->getBlockNumber());
      }
      else if (useFlattenPadGeometryFlag)
      {
         int widthIndex = getCamCadDatabase().getDefinedWidthIndex(exteriorCornerRadius);
         CString complexApertureName;
         complexApertureName.Format("%s(scaled %s)-w%d",padGeometry->getName(),fpfmt(areaScaleFactor,5),widthIndex);

         BlockStruct* scaledComplexApertureBlock = m_camCadDatabase.getBlock(complexApertureName);

         if (scaledComplexApertureBlock == NULL)
         {
            scaledComplexApertureBlock = m_camCadDatabase.getNewBlock(complexApertureName,"%d",padGeometry->getBlockType());
            DataStruct* polyStruct = newDataStruct(dataTypePoly);
            polyStruct->setLayerIndex(m_camCadDatabase.getFloatingLayerIndex());
            scaledComplexApertureBlock->getDataList().AddTail(polyStruct);

            CTMatrix matrix;
            //matrix.scale(linearScaleFactor);
            flattenPadGeometry(scaledComplexApertureBlock->getDataList(),*(polyStruct->getPolyList()),*padGeometry,matrix,exteriorCornerRadius/linearScaleFactor,0.);

            //matrix.scale(linearScaleFactor);
            //polyStruct->transform(matrix);

            // The complex aperture may ultimately have inserted a simple aperture.
            // The flattening of simple aperture does not put the boundary in the polyList arg in flatten, it
            // adds it to a block,the very block we have here. Not sure if that is right or wrong. Doing a takeData()
            // from the new polystructit makes (that gets added to the parentBlock) makes the original scaling here
            // work, though I'm not sure if it is okay, what else it might break. But it seems the polylist is in
            // the block's data list, so just scaling the whole block fixes it and I have not found anything
            // it breaks.
            scaledComplexApertureBlock->Scale(linearScaleFactor);
         }

         m_pad->getInsert()->setBlockNumber(scaledComplexApertureBlock->getBlockNumber());
         m_pad->setLayerIndex(m_camCadDatabase.getFloatingLayerIndex());
      }
      else if (padGeometry->getShape() == apertureComplex)
      {
         CString complexApertureName;
         BlockStruct* complexApertureBlock = m_camCadDatabase.getBlock(padGeometry->getComplexApertureSubBlockNumber());
         complexApertureName.Format("%s(scaled %s)",complexApertureBlock->getName(),fpfmt(areaScaleFactor,5));

         BlockStruct* scaledComplexApertureBlock = m_camCadDatabase.getBlock(complexApertureName);

         if (scaledComplexApertureBlock == NULL)
         {
            scaledComplexApertureBlock = m_camCadDatabase.getNewBlock(complexApertureName,"%d",complexApertureBlock->getBlockType());

            CTMatrix matrix;
            matrix.scale(linearScaleFactor,linearScaleFactor);

            for (CDataListIterator dataIterator(*complexApertureBlock,dataTypePoly);dataIterator.hasNext();)
            {
               DataStruct* polyStruct = dataIterator.getNext();
               LayerStruct* layer = m_camCadDatabase.getLayerAt(polyStruct->getLayerIndex());

               if (layer->isFloating())
               {
                  DataStruct* insetPolyStruct = newDataStruct(*polyStruct);
                  scaledComplexApertureBlock->getDataList().AddTail(insetPolyStruct);

                  CPolyList* polyList = insetPolyStruct->getPolyList();

                  for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
                  {
                     POSITION oldPolyPos = polyPos;
                     CPoly* poly = polyList->GetNext(polyPos);

                     if (poly->isClosed())
                     {
                        poly->transform(matrix);
                     }
                     else
                     {
                        polyList->deleteAt(oldPolyPos);
                        poly = NULL;
                     }
                  }
               }
            }
         }

         m_pad->getInsert()->setBlockNumber(scaledComplexApertureBlock->getBlockNumber());
         m_pad->setLayerIndex(m_camCadDatabase.getFloatingLayerIndex());
      }
      else
      {
         CTMatrix matrix;
         matrix.scale(linearScaleFactor,linearScaleFactor);

         for (CDataListIterator dataIterator(*padGeometry,dataTypePoly);dataIterator.hasNext();)
         {
            DataStruct* polyStruct = dataIterator.getNext();
            CPolyList* polyList = polyStruct->getPolyList();

            for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
            {
               CPoly* poly = polyList->GetNext(polyPos);

               if (poly->isClosed())
               {
                  poly->transform(matrix);
               }
            }
         }
      }

      CTMatrix rotateMatrix;
      CTMatrix apertureMatrix = padGeometry->getApertureTMatrix();
      CTMatrix padMatrix      = m_pad->getInsert()->getTMatrix();
      bool mirrorFlag = getPad()->getInsert()->getGraphicMirrored();
      CBasesVector apertureBasesVector = padGeometry->getApertureBasesVector();

      if (mirrorFlag)
      {
         apertureMatrix.translateCtm(apertureBasesVector.getOrigin());
      }

      // compensate for offset pads
      CTMatrix matrix = apertureMatrix * padMatrix;

      CBasesVector newBasesVector;
      newBasesVector.transform(matrix);
      m_pad->getInsert()->setBasesVector(newBasesVector);
   }
}

//_____________________________________________________________________________
CStencilGenerationParameters::CStencilGenerationParameters(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase)
{
   setToDefaults();
}

CStencilGenerationParameters::CStencilGenerationParameters(const CStencilGenerationParameters& other) :
   m_camCadDatabase(other.m_camCadDatabase)
{
   *this = other;
}

CStencilGenerationParameters& CStencilGenerationParameters::operator=(const CStencilGenerationParameters& other)
{
   if (&other != this)
   {
      m_topPasteInHole               = other.m_topPasteInHole;
      m_bottomPasteInHole            = other.m_bottomPasteInHole;
      m_topPasteInVia                = other.m_topPasteInVia;
      m_bottomPasteInVia             = other.m_bottomPasteInVia;
      m_topStencilThickness          = other.m_topStencilThickness;
      m_bottomStencilThickness       = other.m_bottomStencilThickness;
      m_topMaxFeatureSize            = other.m_topMaxFeatureSize;
      m_bottomMaxFeatureSize         = other.m_bottomMaxFeatureSize;
      m_topWebSize                   = other.m_topWebSize;
      m_bottomWebSize                = other.m_bottomWebSize;
      m_exteriorCornerRadius         = other.m_exteriorCornerRadius;
      m_topStencilSourceLayerName    = other.m_topStencilSourceLayerName;
      m_bottomStencilSourceLayerName = other.m_bottomStencilSourceLayerName;
      m_propagateRulesSideToSide     = other.m_propagateRulesSideToSide;
      m_useMountTechAttrib           = other.m_useMountTechAttrib;
      m_mountTechAttribName          = other.m_mountTechAttribName;
      m_mountTechAttribSMDValue      = other.m_mountTechAttribSMDValue;
   }

   return *this;
}

bool CStencilGenerationParameters::allParametersEqual(const CStencilGenerationParameters& other)
{
   bool sameStencilParams =
      (m_topPasteInHole    == other.m_topPasteInHole              ) &&
      (m_bottomPasteInHole == other.m_bottomPasteInHole           ) &&
      (m_topPasteInVia     == other.m_topPasteInVia               ) &&
      (m_bottomPasteInVia  == other.m_bottomPasteInVia            ) &&
      fpeq(m_topStencilThickness   ,other.m_topStencilThickness   ) &&
      fpeq(m_bottomStencilThickness,other.m_bottomStencilThickness) &&
      fpeq(m_topMaxFeatureSize     ,other.m_topMaxFeatureSize     ) &&
      fpeq(m_bottomMaxFeatureSize  ,other.m_bottomMaxFeatureSize  ) &&
      fpeq(m_topWebSize            ,other.m_topWebSize            ) &&
      fpeq(m_bottomWebSize         ,other.m_bottomWebSize         ) &&
      fpeq(m_exteriorCornerRadius  ,other.m_exteriorCornerRadius  ) &&
      (m_propagateRulesSideToSide     == other.m_propagateRulesSideToSide    ) &&
      (m_topStencilSourceLayerName    == other.m_topStencilSourceLayerName   ) &&
      (m_bottomStencilSourceLayerName == other.m_bottomStencilSourceLayerName);

   bool sameSmdMethod = smdParametersEqual(other);

   return sameStencilParams && sameSmdMethod;
}

bool CStencilGenerationParameters::smdParametersEqual(const CStencilGenerationParameters& other)
{
   // The mount tech attrib name and smd value only need to match if using them.
   // So equal is true if both not using attrib or both are using and name and smd value are the same.
   bool sameSmdMethod = (this->m_useMountTechAttrib == other.m_useMountTechAttrib); // first just check that both use same method, no chance of being same if not
   if (sameSmdMethod)
   {
      sameSmdMethod = (this->m_useMountTechAttrib == false) || // both not using attrib or
         (this->m_useMountTechAttrib == true && // both using attrib and
         this->m_mountTechAttribName.CompareNoCase(other.m_mountTechAttribName) == 0 && // attrib names match and
         this->m_mountTechAttribSMDValue.CompareNoCase(other.m_mountTechAttribSMDValue) == 0); // attrib smd values match
   }

   return sameSmdMethod;
}

void CStencilGenerationParameters::setToDefaults()
{
   double pageUnitsPerInch       = getUnitsFactor(pageUnitsInches     ,getCamCadDatabase().getPageUnits());
   double pageUnitsPerMilliMeter = getUnitsFactor(pageUnitsMilliMeters,getCamCadDatabase().getPageUnits());

   m_topPasteInHole         = false;
   m_topPasteInVia          = false;
   m_topStencilThickness    =  .008 * pageUnitsPerInch;
   m_topMaxFeatureSize      = 5.0   * pageUnitsPerMilliMeter;
   m_topWebSize             = 1.0   * pageUnitsPerMilliMeter;
   m_exteriorCornerRadius   =  .001 * pageUnitsPerInch;

   m_bottomPasteInHole      = m_topPasteInHole;
   m_bottomPasteInVia       = m_topPasteInVia;
   m_bottomStencilThickness = m_topStencilThickness;
   m_bottomMaxFeatureSize   = m_topMaxFeatureSize;
   m_bottomWebSize          = m_topWebSize;

   m_topStencilSourceLayerName.Empty();
   m_bottomStencilSourceLayerName.Empty();

   m_propagateRulesSideToSide = true;

   m_useMountTechAttrib = false; // false means use original style before attrib support existed
   m_mountTechAttribName.Empty();
   m_mountTechAttribSMDValue.Empty();
}

void CStencilGenerationParameters::initUndefinedStencilSourceLayerNames()
{
   if (m_topStencilSourceLayerName.IsEmpty() && m_bottomStencilSourceLayerName.IsEmpty())
   {
      CCEtoODBDoc& camCadDoc = getCamCadDatabase().getCamCadDoc();
      int maxLayerIndex = camCadDoc.getMaxLayerIndex();
      LayerStruct* topPasteLayer = NULL;
      LayerStruct* bottomPasteLayer = NULL;
      bool multipleFlag = false;

      for (int layerIndex = 0;layerIndex < maxLayerIndex && !multipleFlag;layerIndex++)
      {
         LayerStruct* layer = camCadDoc.getLayerAt(layerIndex);

         if (layer != NULL)
         {
            if (layer->getLayerType() == layerTypePasteTop)
            {
               if (topPasteLayer != NULL)
               {
                  multipleFlag = true;
               }
               else
               {
                  topPasteLayer = layer;
               }
            }
            else if (layer->getLayerType() == layerTypePasteBottom)
            {
               if (bottomPasteLayer != NULL)
               {
                  multipleFlag = true;
               }
               else
               {
                  bottomPasteLayer = layer;
               }
            }
         }
      }

      if (!multipleFlag && topPasteLayer != NULL && bottomPasteLayer != NULL)
      {
         m_topStencilSourceLayerName    = topPasteLayer->getName();
         m_bottomStencilSourceLayerName = bottomPasteLayer->getName();
      }
   }
}

bool CStencilGenerationParameters::readAttributes(int fileNumber)
{
   bool retval = false;

   FileStruct* fileStruct = getCamCadDatabase().getFile(fileNumber);

   if (fileStruct != NULL)
   {
      BlockStruct* pcb = fileStruct->getBlock();

      if (pcb != NULL)
      {
         CAttributes* pcbAttributes = pcb->getAttributesRef();

         if (pcbAttributes != NULL)
         {
            Attrib* attribute;

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeTopThickness),attribute))
            {
               m_topStencilThickness = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeBottomThickness),attribute))
            {
               m_bottomStencilThickness = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeTopMaxFeature),attribute))
            {
               m_topMaxFeatureSize = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeBottomMaxFeature),attribute))
            {
               m_bottomMaxFeatureSize = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeTopWebWidth),attribute))
            {
               m_topWebSize = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeBottomWebWidth),attribute))
            {
               m_bottomWebSize = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeTopStencilSourceLayer),attribute))
            {
               m_topStencilSourceLayerName = getCamCadDatabase().getAttributeStringValue(attribute);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeBottomStencilSourceLayer),attribute))
            {
               m_bottomStencilSourceLayerName = getCamCadDatabase().getAttributeStringValue(attribute);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeExteriorCornerRadius),attribute))
            {
               m_exteriorCornerRadius = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeTopPasteInHole),attribute))
            {
               m_topPasteInHole = (atoi(getCamCadDatabase().getAttributeStringValue(attribute)) != 0);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInHole),attribute))
            {
               m_bottomPasteInHole = (atoi(getCamCadDatabase().getAttributeStringValue(attribute)) != 0);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeTopPasteInVia),attribute))
            {
               m_topPasteInVia = (atoi(getCamCadDatabase().getAttributeStringValue(attribute)) != 0);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInVia),attribute))
            {
               m_bottomPasteInVia = (atoi(getCamCadDatabase().getAttributeStringValue(attribute)) != 0);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributePropagateRules),attribute))
            {
               m_propagateRulesSideToSide = (getCamCadDatabase().getAttributeStringValue(attribute)).CompareNoCase("Yes") == 0;
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeUseMountTechAttrib),attribute))
            {
               m_useMountTechAttrib = (getCamCadDatabase().getAttributeStringValue(attribute)).CompareNoCase("Yes") == 0;
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeMountTechAttribName),attribute))
            {
               m_mountTechAttribName = getCamCadDatabase().getAttributeStringValue(attribute);
            }

            if (pcbAttributes->Lookup(CStencilPin::getKeywordIndex(stencilAttributeMountTechAttribSMDValue),attribute))
            {
               m_mountTechAttribSMDValue = getCamCadDatabase().getAttributeStringValue(attribute);
            }
         }

         retval = true;
      }
   }

   return retval;
}

bool CStencilGenerationParameters::writeAttributes(int fileNumber)
{
   bool retval = false;

   FileStruct* fileStruct = getCamCadDatabase().getFile(fileNumber);

   if (fileStruct != NULL)
   {
      BlockStruct* pcb = fileStruct->getBlock();

      if (pcb != NULL)
      {
         CAttributes*& pcbAttributes = pcb->getDefinedAttributes();

         CString stringValue;

         stringValue.Format("%f",m_topStencilThickness);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopThickness),stringValue);

         stringValue.Format("%f",m_bottomStencilThickness);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomThickness),stringValue);

         stringValue.Format("%f",m_topMaxFeatureSize);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopMaxFeature),stringValue);

         stringValue.Format("%f",m_bottomMaxFeatureSize);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomMaxFeature),stringValue);

         stringValue.Format("%f",m_topWebSize);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopWebWidth),stringValue);

         stringValue.Format("%f",m_bottomWebSize);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomWebWidth),stringValue);

         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopStencilSourceLayer)   ,m_topStencilSourceLayerName);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomStencilSourceLayer),m_bottomStencilSourceLayerName);

         stringValue.Format("%f",m_exteriorCornerRadius);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeExteriorCornerRadius),stringValue);

         stringValue.Format("%d",(int)m_topPasteInHole);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopPasteInHole),stringValue);

         stringValue.Format("%d",(int)m_bottomPasteInHole);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInHole),stringValue);

         stringValue.Format("%d",(int)m_topPasteInVia);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopPasteInVia),stringValue);

         stringValue.Format("%d",(int)m_bottomPasteInVia);
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInVia),stringValue);

         //deadcode stringValue.Format("%s",(m_propagateRulesSideToSide ? "Yes" : "No"));
         //deadcode getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributePropagateRules),stringValue);

         stringValue.Format("%s",(m_useMountTechAttrib ? "Yes" : "No"));
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeUseMountTechAttrib),stringValue);

         stringValue = m_mountTechAttribName;
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeMountTechAttribName),stringValue);

         stringValue = m_mountTechAttribSMDValue;
         getCamCadDatabase().addAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeMountTechAttribSMDValue),stringValue);

         retval = true;
      }
   }

   return retval;
}

bool CStencilGenerationParameters::readStencilSettings(CStringArray& params)
{
   bool retval = (params.GetCount() > 1);

   if (retval)
   {
      double unitValue = 0.;
      CString attributeName  = params[0];

      CSupString attributeUnitValue(params[1]);
      CStringArray attributeUnitValueParams;

      int numAttributeUnitValueParams = attributeUnitValue.ParseWhite(attributeUnitValueParams);
      CString attributeValue = attributeUnitValueParams[0];

      if (numAttributeUnitValueParams > 1)
      {
         CString unitsString = attributeUnitValueParams[1];
         PageUnitsTag attributePageUnits = unitStringAbbreviationToTag(unitsString);

         if (attributePageUnits != pageUnitsUndefined)
         {
            unitValue = atof(attributeValue);
            CUnits units(attributePageUnits);

            unitValue = units.convertTo(getCamCadDatabase().getPageUnits(),unitValue);
         }
      }
      else
      {
         unitValue = atof(attributeValue);
      }

      if (attributeName.CompareNoCase(QtopPasteInHole) == 0)
      {
         m_topPasteInHole = (atoi(attributeValue) != 0);
      }
      else if (attributeName.CompareNoCase(QbottomPasteInHole) == 0)
      {
         m_bottomPasteInHole = (atoi(attributeValue) != 0);
      }
      else if (attributeName.CompareNoCase(QtopPasteInVia) == 0)
      {
         m_topPasteInVia = (atoi(attributeValue) != 0);
      }
      else if (attributeName.CompareNoCase(QbottomPasteInVia) == 0)
      {
         m_bottomPasteInVia = (atoi(attributeValue) != 0);
      }
      else if (attributeName.CompareNoCase(QtopStencilThickness) == 0)
      {
         m_topStencilThickness = unitValue;
      }
      else if (attributeName.CompareNoCase(QbottomStencilThickness) == 0)
      {
         m_bottomStencilThickness = unitValue;
      }
      else if (attributeName.CompareNoCase(QtopMaxFeatureSize) == 0)
      {
         m_topMaxFeatureSize = unitValue;
      }
      else if (attributeName.CompareNoCase(QbottomMaxFeatureSize) == 0)
      {
         m_bottomMaxFeatureSize = unitValue;
      }
      else if (attributeName.CompareNoCase(QtopWebSize) == 0)
      {
         m_topWebSize = unitValue;
      }
      else if (attributeName.CompareNoCase(QbottomWebSize) == 0)
      {
         m_bottomWebSize = unitValue;
      }
      else if (attributeName.CompareNoCase(QexteriorCornerRadius) == 0)
      {
         m_exteriorCornerRadius = unitValue;
      }
      else if (attributeName.CompareNoCase(QtopStencilSourceLayer) == 0)
      {
         m_topStencilSourceLayerName = attributeValue;
      }
      else if (attributeName.CompareNoCase(QbottomStencilSourceLayer) == 0)
      {
         m_bottomStencilSourceLayerName = attributeValue;
      }
      else if (attributeName.CompareNoCase(QuseMountTechAttrib) == 0)
      {
         m_useMountTechAttrib = attributeValue.CompareNoCase("Yes") == 0;
      }
      else if (attributeName.CompareNoCase(QmountTechAttribName) == 0)
      {
         m_mountTechAttribName = attributeValue;
      }
      else if (attributeName.CompareNoCase(QmountTechAttribSMDValue) == 0)
      {
         m_mountTechAttribSMDValue = attributeValue;
      }
   }

   return retval;
}

void CStencilGenerationParameters::clearStencilSettings(FileStruct* fileStruct)
{
   if (fileStruct != NULL)
   {
      BlockStruct* pcb = fileStruct->getBlock();

      if (pcb != NULL)
      {
         CAttributes*& pcbAttributes = pcb->getAttributesRef();

         if (pcbAttributes != NULL)
         {
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopThickness));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomThickness));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopMaxFeature));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomMaxFeature));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopWebWidth));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomWebWidth));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopStencilSourceLayer));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomStencilSourceLayer));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeExteriorCornerRadius));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeTopPasteInHole));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInHole));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributePropagateRules));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeUseMountTechAttrib));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeMountTechAttribName));
            getCamCadDatabase().removeAttribute(&pcbAttributes,CStencilPin::getKeywordIndex(stencilAttributeMountTechAttribSMDValue));
         }
      }
   }
}

bool CStencilGenerationParameters::writeStencilSettings(CWriteFormat& settingsFile)
{
   bool retval = true;
   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();

   settingsFile.write("[StencilGenerationParameters]\n");

   settingsFile.writef("%s %d\n",QtopPasteInHole          ,(int)m_topPasteInHole);
   settingsFile.writef("%s %d\n",QbottomPasteInHole       ,(int)m_bottomPasteInHole);
   settingsFile.writef("%s %d\n",QtopPasteInVia           ,(int)m_topPasteInVia);
   settingsFile.writef("%s %d\n",QbottomPasteInVia        ,(int)m_bottomPasteInVia);
   settingsFile.writef("%s %f\n",QtopStencilThickness     ,m_topStencilThickness   ,pageUnits);
   settingsFile.writef("%s %f\n",QbottomStencilThickness  ,m_bottomStencilThickness,pageUnits);
   settingsFile.writef("%s %f\n",QtopMaxFeatureSize       ,m_topMaxFeatureSize     ,pageUnits);
   settingsFile.writef("%s %f\n",QbottomMaxFeatureSize    ,m_bottomMaxFeatureSize  ,pageUnits);
   settingsFile.writef("%s %f\n",QtopWebSize              ,m_topWebSize            ,pageUnits);
   settingsFile.writef("%s %f\n",QbottomWebSize           ,m_bottomWebSize         ,pageUnits);
   settingsFile.writef("%s %s\n",QtopStencilSourceLayer   ,quoteString(m_topStencilSourceLayerName));
   settingsFile.writef("%s %s\n",QbottomStencilSourceLayer,quoteString(m_bottomStencilSourceLayerName));
   settingsFile.writef("%s %f\n",QexteriorCornerRadius    ,m_exteriorCornerRadius  ,pageUnits);
   settingsFile.writef("%s %f\n",QuseMountTechAttrib      ,m_useMountTechAttrib ? "Yes" : "No");
   settingsFile.writef("%s %f\n",QmountTechAttribName     ,m_mountTechAttribName);
   settingsFile.writef("%s %f\n",QmountTechAttribSMDValue ,m_mountTechAttribSMDValue);
   settingsFile.write("\n");

   return retval;
}

bool CStencilGenerationParameters::writeStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase)
{
   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();

   stencilGeneratorSettingsDatabase.setStencilSetting(QtopPasteInHole          ,(int)m_topPasteInHole);
   stencilGeneratorSettingsDatabase.setStencilSetting(QbottomPasteInHole       ,(int)m_bottomPasteInHole);
   stencilGeneratorSettingsDatabase.setStencilSetting(QtopPasteInVia           ,(int)m_topPasteInVia);
   stencilGeneratorSettingsDatabase.setStencilSetting(QbottomPasteInVia        ,(int)m_bottomPasteInVia);
   stencilGeneratorSettingsDatabase.setStencilSetting(QtopStencilThickness     ,m_topStencilThickness   ,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QbottomStencilThickness  ,m_bottomStencilThickness,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QtopMaxFeatureSize       ,m_topMaxFeatureSize     ,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QbottomMaxFeatureSize    ,m_bottomMaxFeatureSize  ,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QtopWebSize              ,m_topWebSize            ,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QbottomWebSize           ,m_bottomWebSize         ,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QtopStencilSourceLayer   ,m_topStencilSourceLayerName);
   stencilGeneratorSettingsDatabase.setStencilSetting(QbottomStencilSourceLayer,m_bottomStencilSourceLayerName);
   stencilGeneratorSettingsDatabase.setStencilSetting(QexteriorCornerRadius    ,m_exteriorCornerRadius  ,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QuseMountTechAttrib      ,m_useMountTechAttrib ? "Yes" : "No");
   stencilGeneratorSettingsDatabase.setStencilSetting(QmountTechAttribName     ,m_mountTechAttribName);
   stencilGeneratorSettingsDatabase.setStencilSetting(QmountTechAttribSMDValue ,m_mountTechAttribSMDValue);

   return true;
}

//_____________________________________________________________________________
int CStencilValidationParameters::m_minHoleAreaToWallAreaRatioKeywordIndex = -1;
int CStencilValidationParameters::m_minFeatureKeywordIndex                 = -1;
int CStencilValidationParameters::m_maxFeatureKeywordIndex                 = -1;
//int CStencilValidationParameters::m_splitFeatureWebWidthKeywordIndex       = -1;
int CStencilValidationParameters::m_minInsetKeywordIndex                   = -1;

void CStencilValidationParameters::resetKeywordIndices()
{
   m_minHoleAreaToWallAreaRatioKeywordIndex = -1;
   m_minFeatureKeywordIndex                 = -1;
   m_maxFeatureKeywordIndex                 = -1;
 //m_splitFeatureWebWidthKeywordIndex       = -1;
   m_minInsetKeywordIndex                   = -1;
}

CStencilValidationParameters::CStencilValidationParameters(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase)
{
   setToDefaults();

   m_violationCount = 0;
}

CStencilValidationParameters::CStencilValidationParameters(const CStencilValidationParameters& other) :
   m_camCadDatabase(other.m_camCadDatabase)
{
   *this = other;
}

CStencilValidationParameters& CStencilValidationParameters::operator=(const CStencilValidationParameters& other)
{
   if (&other != this)
   {
      m_minHoleAreaToWallAreaRatio = other.m_minHoleAreaToWallAreaRatio;
      m_minFeature                 = other.m_minFeature;
      m_maxFeature                 = other.m_maxFeature;
    //m_splitFeatureWebWidth       = other.m_splitFeatureWebWidth;
      m_minInset                   = other.m_minInset;
      m_violationCount             = other.m_violationCount;
   }

   return *this;
}

bool CStencilValidationParameters::allParametersEqual(const CStencilValidationParameters& other)
{
   bool retval = fpeq(m_minHoleAreaToWallAreaRatio,other.m_minHoleAreaToWallAreaRatio) &&
                 fpeq(m_minFeature                ,other.m_minFeature                ) &&
                 fpeq(m_maxFeature                ,other.m_maxFeature                ) &&
               //fpeq(m_splitFeatureWebWidth      ,other.m_splitFeatureWebWidth      ) &&
                 fpeq(m_minInset                  ,other.m_minInset                  );

   return retval;
}

void CStencilValidationParameters::setToDefaults()
{
   m_minHoleAreaToWallAreaRatio =  .66;
   m_minFeature                 =  .012 * getUnitsFactor(pageUnitsInches     ,getCamCadDatabase().getPageUnits());
   m_maxFeature                 = 5.0   * getUnitsFactor(pageUnitsMilliMeters,getCamCadDatabase().getPageUnits());
 //m_splitFeatureWebWidth       = 1.0   * getUnitsFactor(pageUnitsMilliMeters,getCamCadDatabase().getPageUnits());
   m_minInset                   =  .001 * getUnitsFactor(pageUnitsInches     ,getCamCadDatabase().getPageUnits());
}

bool CStencilValidationParameters::readAttributes(int fileNumber)
{
   bool retval = false;

   FileStruct* fileStruct = getCamCadDatabase().getFile(fileNumber);

   if (fileStruct != NULL)
   {
      BlockStruct* pcb = fileStruct->getBlock();

      if (pcb != NULL)
      {
         CAttributes* pcbAttributes = pcb->getAttributesRef();

         if (pcbAttributes != NULL)
         {
            Attrib* attribute;

            if (pcbAttributes->Lookup(m_minHoleAreaToWallAreaRatioKeywordIndex,attribute))
            {
               m_minHoleAreaToWallAreaRatio = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(m_minFeatureKeywordIndex,attribute))
            {
               m_minFeature = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            if (pcbAttributes->Lookup(m_maxFeatureKeywordIndex,attribute))
            {
               m_maxFeature = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }

            //if (pcbAttributes->Lookup(m_splitFeatureWebWidthKeywordIndex,attribute))
            //{
            //   m_splitFeatureWebWidth = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            //}

            if (pcbAttributes->Lookup(m_minInsetKeywordIndex,attribute))
            {
               m_minInset = atof(getCamCadDatabase().getAttributeStringValue(attribute));
            }
         }

         retval = true;
      }
   }

   return retval;
}

bool CStencilValidationParameters::writeAttributes(int fileNumber)
{
   bool retval = false;

   FileStruct* fileStruct = getCamCadDatabase().getFile(fileNumber);

   if (fileStruct != NULL)
   {
      BlockStruct* pcb = fileStruct->getBlock();

      if (pcb != NULL)
      {
         CAttributes*& pcbAttributes = pcb->getAttributesRef();

         if (pcbAttributes == NULL)
         {
            pcbAttributes = new CAttributes();
         }

         CString stringValue;

         stringValue.Format("%f",m_minHoleAreaToWallAreaRatio);
         getCamCadDatabase().addAttribute(&pcbAttributes,m_minHoleAreaToWallAreaRatioKeywordIndex,stringValue);

         stringValue.Format("%f",m_minFeature);
         getCamCadDatabase().addAttribute(&pcbAttributes,m_minFeatureKeywordIndex,stringValue);

         stringValue.Format("%f",m_maxFeature);
         getCamCadDatabase().addAttribute(&pcbAttributes,m_maxFeatureKeywordIndex,stringValue);

         //stringValue.Format("%f",m_splitFeatureWebWidth);
         //getCamCadDatabase().addAttribute(&pcbAttributes,m_splitFeatureWebWidthKeywordIndex,stringValue);

         stringValue.Format("%f",m_minInset);
         getCamCadDatabase().addAttribute(&pcbAttributes,m_minInsetKeywordIndex,stringValue);

         retval = true;
      }
   }

   return retval;
}

bool CStencilValidationParameters::readStencilSettings(CStringArray& params)
{
   bool retval = (params.GetCount() > 1);

   if (retval)
   {
      double unitValue = 0.;
      CString attributeName  = params[0];

      CSupString attributeUnitValue(params[1]);
      CStringArray attributeUnitValueParams;

      int numAttributeUnitValueParams = attributeUnitValue.ParseWhite(attributeUnitValueParams);
      CString attributeValue = attributeUnitValueParams[0];

      if (numAttributeUnitValueParams > 1)
      {
         CString unitsString = attributeUnitValueParams[1];
         PageUnitsTag attributePageUnits = unitStringAbbreviationToTag(unitsString);

         if (attributePageUnits != pageUnitsUndefined)
         {
            unitValue = atof(attributeValue);
            CUnits units(attributePageUnits);

            unitValue = units.convertTo(getCamCadDatabase().getPageUnits(),unitValue);
         }
      }
      else
      {
         unitValue = atof(attributeValue);
      }

      if (attributeName.CompareNoCase(QminHoleAreaToWallAreaRatio) == 0)
      {
         m_minHoleAreaToWallAreaRatio = atof(attributeValue);
      }
      else if (attributeName.CompareNoCase(QminFeature) == 0)
      {
         m_minFeature = unitValue;
      }
      else if (attributeName.CompareNoCase(QmaxFeature) == 0)
      {
         m_maxFeature = unitValue;
      }
      else if (attributeName.CompareNoCase(QminInset) == 0)
      {
         m_minInset = unitValue;
      }
   }

   return retval;
}

void CStencilValidationParameters::clearStencilSettings(FileStruct* fileStruct)
{
   if (fileStruct != NULL)
   {
      BlockStruct* pcb = fileStruct->getBlock();

      if (pcb != NULL)
      {
         CAttributes*& pcbAttributes = pcb->getAttributesRef();

         if (pcbAttributes != NULL)
         {
            getCamCadDatabase().removeAttribute(&pcbAttributes,m_minHoleAreaToWallAreaRatioKeywordIndex);
            getCamCadDatabase().removeAttribute(&pcbAttributes,m_minFeatureKeywordIndex);
            getCamCadDatabase().removeAttribute(&pcbAttributes,m_maxFeatureKeywordIndex);
            getCamCadDatabase().removeAttribute(&pcbAttributes,m_minInsetKeywordIndex);
         }
      }
   }
}

bool CStencilValidationParameters::writeStencilSettings(CStdioFileWriteFormat& settingsFile)
{
   bool retval = true;
   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();

   settingsFile.write("[StencilValidationParameters]\n");

   settingsFile.writef("%s %f\n",QminHoleAreaToWallAreaRatio,m_minHoleAreaToWallAreaRatio);
   settingsFile.writef("%s %f\n",QminFeature                ,m_minFeature,pageUnits);
   settingsFile.writef("%s %f\n",QmaxFeature                ,m_maxFeature,pageUnits);
   settingsFile.writef("%s %f\n",QminInset                  ,m_minInset  ,pageUnits);
   settingsFile.write("\n");

   return retval;
}

bool CStencilValidationParameters::writeStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase)
{
   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();

   stencilGeneratorSettingsDatabase.setStencilSetting(QminHoleAreaToWallAreaRatio,m_minHoleAreaToWallAreaRatio);
   stencilGeneratorSettingsDatabase.setStencilSetting(QminFeature                ,m_minFeature,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QmaxFeature                ,m_maxFeature,pageUnits);
   stencilGeneratorSettingsDatabase.setStencilSetting(QminInset                  ,m_minInset  ,pageUnits);

   return true;
}

void CStencilValidationParameters::registerStencilValidationKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog)
{
   if (m_minHoleAreaToWallAreaRatioKeywordIndex == -1)
   {
      m_minHoleAreaToWallAreaRatioKeywordIndex = camCadDatabase.registerHiddenKeyWord("StencilValidationMinHoleAreaToWallAreaRatio",0,valueTypeDouble    ,errorLog);
      m_minFeatureKeywordIndex                 = camCadDatabase.registerHiddenKeyWord("StencilValidationMinFeature"                ,0,valueTypeUnitDouble,errorLog);
      m_maxFeatureKeywordIndex                 = camCadDatabase.registerHiddenKeyWord("StencilValidationMaxFeature"                ,0,valueTypeUnitDouble,errorLog);
    //m_splitFeatureWebWidthKeywordIndex       = camCadDatabase.registerHiddenKeyWord("StencilValidationMSplitFeatureWebWidth"     ,0,valueTypeUnitDouble,errorLog);
      m_minInsetKeywordIndex                   = camCadDatabase.registerHiddenKeyWord("StencilValidationMinInset"                  ,0,valueTypeUnitDouble,errorLog);
   }
}

//_____________________________________________________________________________
void CStencilRuleModifierValues::empty()
{
   m_modifierValues.RemoveAll();
}

void CStencilRuleModifierValues::copy(const CStencilRuleModifierValues& other)
{
   m_modifierValues.Copy(other.m_modifierValues);
}

int CStencilRuleModifierValues::getCount() const
{
   return m_modifierValues.GetCount();
}

CString CStencilRuleModifierValues::getAt(int index) const
{
   return m_modifierValues.GetAt(index);
}

void CStencilRuleModifierValues::setAt(int index,const CString& value)
{
   m_modifierValues.SetAtGrow(index,value);
}

CStringArray& CStencilRuleModifierValues::getStringArray()
{
   return m_modifierValues;
}

void CStencilRuleModifierValues::add(const CString& value)
{
   m_modifierValues.Add(value);
}

void CStencilRuleModifierValues::add(CString value,PageUnitsTag pageUnits)
{
   if (pageUnits != pageUnitsUndefined)
   {
      value += " " + unitStringAbbreviation(pageUnits);
   }

   m_modifierValues.Add(value);
}

//_____________________________________________________________________________
CStencilRuleModifier::CStencilRuleModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues)
: m_modifier(modifier)
{
   m_modifierValues.copy(modifierValues);
}

CStencilRuleModifier::CStencilRuleModifier(const CStencilRuleModifier& other)
{
   *this = other;
}

CStencilRuleModifier& CStencilRuleModifier::operator=(const CStencilRuleModifier& other)
{
   if (&other != this)
   {
      m_modifier          = other.m_modifier;

      m_modifierValues.copy(other.m_modifierValues);
   }

   return *this;
}

CString CStencilRuleModifier::getModifierString() const
{
   CString modifierString;

   if (m_modifier != stencilRuleModifierUndefined)
   {
      modifierString += quoteString(stencilRuleModifierToString(m_modifier));

      if (m_modifierValues.getCount() > 0)
      {
         modifierString += "(";

         for (int index = 0;index < m_modifierValues.getCount();index++)
         {
            CString value = m_modifierValues.getAt(index);
            value.Trim();

            if (index != 0)
            {
               modifierString += ",";
            }

            modifierString += quoteString(value);
            //modifierString += value;
         }

         modifierString += ")";
      }
   }

   return modifierString;
}

CString CStencilRuleModifier::getValueAt(int index) const
{
   CString value;

   if (index >= 0 && index < m_modifierValues.getCount())
   {
      value = m_modifierValues.getAt(index);
      CString doubleQuote("\"");

      if (value.GetLength() >= 2 && value.Left(1) == doubleQuote && value.Right(1) == doubleQuote)
      {
         value = value.Mid(1,value.GetLength() - 2);
      }
   }

   return value;
}

double CStencilRuleModifier::getUnitValueAt(int index,PageUnitsTag pageUnits,double defaultValue) const
{
   double unitValue = defaultValue;
   CSupString value(getValueAt(index));

   if (!value.IsEmpty())
   {
      CString valueString = value.Scan(" ");

      if (!valueString.IsEmpty())
      {
         char* endChar;
         unitValue = strtod(valueString,&endChar);

         if (*endChar != '\0')
         {
            unitValue = defaultValue;
         }
         else
         {
            if (!value.IsEmpty())
            {
               PageUnitsTag unit = unitStringAbbreviationToTag(value);
               CUnits units(unit);

               unitValue = units.convertTo(pageUnits,unitValue);
            }
         }
      }
   }

   return unitValue;
}

void CStencilRuleModifier::updateModifierValue(int index,PageUnitsTag pageUnits)
{
   CSupString value(getValueAt(index));

   if (!value.IsEmpty())
   {
      CString valueString = value.Scan(" ");

      if (!valueString.IsEmpty() && value.IsEmpty())
      {
         char* endChar;
         strtod(valueString,&endChar);

         if (*endChar == '\0')
         {
            m_modifierValues.setAt(index,valueString + " " + unitStringAbbreviation(pageUnits));
         }
      }
   }
}

void CStencilRuleModifier::updateModifier(PageUnitsTag pageUnits)
{
   switch (m_modifier)
   {
   case stencilRuleModifierOffsetRelativeToPadstack:
      updateModifierValue(0,pageUnits);
      updateModifierValue(1,pageUnits);
      break;
   case stencilRuleModifierWebSettings:
      updateModifierValue(0,pageUnits);
      updateModifierValue(1,pageUnits);
      break;
   case stencilRuleModifierExteriorCornerRadius:
      updateModifierValue(0,pageUnits);
      break;
   }
}

//_____________________________________________________________________________
void CStencilRuleModifiers::setModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues)
{
   empty();

   addModifier(modifier,modifierValues);
}

void CStencilRuleModifiers::addModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues)
{
   if (modifier != stencilRuleModifierUndefined)
   {
      CStencilRuleModifier* stencilRuleModifier = new CStencilRuleModifier(modifier,modifierValues);
      CString debugModifierString = stencilRuleModifier->getModifierString();

      AddTail(stencilRuleModifier);
   }
}

bool CStencilRuleModifiers::hasModifier(StencilRuleModifierTag modifier)
{
   bool retval = false;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CStencilRuleModifier* stencilRuleModifier = GetNext(pos);

      if (stencilRuleModifier->getModifier() == modifier)
      {
         retval = true;
         break;
      }
   }

   return retval;
}

CStencilRuleModifier* CStencilRuleModifiers::getModifier(StencilRuleModifierTag modifier) const
{
   CStencilRuleModifier* stencilRuleModifier = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      stencilRuleModifier = GetNext(pos);

      if (stencilRuleModifier->getModifier() == modifier)
      {
         break;
      }

      stencilRuleModifier = NULL;
   }

   return stencilRuleModifier;
}

void CStencilRuleModifiers::updateModifiers(PageUnitsTag pageUnits)
{
   CStencilRuleModifier* stencilRuleModifier = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      stencilRuleModifier = GetNext(pos);

      stencilRuleModifier->updateModifier(pageUnits);
   }
}

//_____________________________________________________________________________
CStencilRule::CStencilRule()
{
   m_rule     = stencilRuleUndefined;
   //m_modifier = stencilRuleModifierUndefined;
}

CStencilRule::CStencilRule(const CStencilRule& other)
{
   *this = other;
}

CStencilRule& CStencilRule::operator=(const CStencilRule& other)
{
   if (&other != this)
   {
      m_rule              = other.m_rule;
      m_ruleValue         = other.m_ruleValue;
      m_comment           = other.m_comment;

      m_modifiers.empty();

      for (POSITION pos = other.m_modifiers.GetHeadPosition();pos != NULL;)
      {
         CStencilRuleModifier* modifier = other.m_modifiers.GetNext(pos);
         m_modifiers.AddTail(new CStencilRuleModifier(*modifier));
      }
   }

   return *this;
}

StencilRuleTag CStencilRule::getRule() const
{
   return m_rule;
}

void CStencilRule::setRule(StencilRuleTag rule)
{
   m_rule = rule;
}

CString CStencilRule::getRuleValue() const
{
   return m_ruleValue;
}

void CStencilRule::setRuleValue(const CString& ruleValue)
{
   m_ruleValue = ruleValue;
}

CString CStencilRule::getComment() const
{
   return m_comment;
}

void CStencilRule::setComment(const CString& comment)
{
   m_comment = comment;
}

bool CStencilRule::hasModifiers() const
{
   bool retval = (m_modifiers.GetCount() > 0);

   return retval;
}

void CStencilRule::addModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues)
{
   m_modifiers.addModifier(modifier,modifierValues);
}

void CStencilRule::addModifier(const CString& modifierString)
{
   CStencilRuleModifierValues modifierValues;
   StencilRuleModifierTag modifier = parseStencilRuleModifierAttribute(modifierString,modifierValues);

   addModifier(modifier,modifierValues);
}

bool CStencilRule::hasModifier(StencilRuleModifierTag modifier)
{
   return m_modifiers.hasModifier(modifier);
}

void CStencilRule::addModifiers(const CStencilRule& other)
{
   if (&other != this)
   {
      for (POSITION pos = other.m_modifiers.GetHeadPosition();pos != NULL;)
      {
         CStencilRuleModifier* otherStencilRuleModifier = other.m_modifiers.GetNext(pos);
         CStencilRuleModifier* stencilRuleModifier = new CStencilRuleModifier(*otherStencilRuleModifier);

         m_modifiers.AddTail(stencilRuleModifier);
      }
   }
}

void CStencilRule::updateModifiers(PageUnitsTag pageUnits)
{
   m_modifiers.updateModifiers(pageUnits);
}

bool CStencilRule::isValid() const
{
   bool retval = false;

   switch (m_rule)
   {
   case stencilRuleNone:
   case stencilRuleArea:
   case stencilRuleInset:
   case stencilRuleAperture:
   case stencilRuleApertureGeometry:
   case stencilRuleCopperSourceLayer:
   case stencilRulePassthru:
   case stencilRuleStencilSourceLayer:
   case stencilRuleInheritFromParent:
      retval = true;
      break;
   }

   return retval;
}

StencilRuleModifierTag CStencilRule::parseStencilRuleModifierAttribute(const CString& modifierString,CStencilRuleModifierValues& parameters)
{
   CSupString modifierStringValue = modifierString;

   CString stencilRuleModifierString = modifierStringValue.Scan("(");

   StencilRuleModifierTag modifierTag = stringToStencilRuleModifierTag(stencilRuleModifierString);

   if (modifierStringValue.Right(1) == ")")
   {
      modifierStringValue.GetBufferSetLength(modifierStringValue.GetLength() - 1);

      modifierStringValue.Parse(parameters.getStringArray(),",");
   }

   return modifierTag;
}

CString CStencilRule::fixModifiers(const CString& line)
{
   CString retval(line);

   int leftPos = line.Find("(");

   if (leftPos >= 0)
   {
      int rightPos = line.Find(")",leftPos);

      if (rightPos >= leftPos + 1)
      {
         CSupString modifierValues = line.Mid(leftPos + 1,rightPos - leftPos - 1);
         CStringArray params;

         modifierValues.Parse(params,",");
         CString newModifierValues;

         for (int index = 0;index < params.GetCount();index++)
         {
            newModifierValues += (index > 0 ? "," : "") + quoteString(params.GetAt(index));
         }

         retval = line.Left(leftPos + 1) + newModifierValues + fixModifiers(line.Mid(rightPos));
      }
   }

   return retval;
}

bool CStencilRule::setRuleFromString(const CString& ruleString)
{
   CSupString ruleSupString(ruleString);
   CStringArray params;

   ruleSupString.ParseQuote(params," ");

   return setRuleFromParameters(params,0);
}

bool CStencilRule::setRuleFromParameters(CStringArray& params,int ruleStartIndex)
{
   // Inset 0.050000
   // Area 0.900000
   // Aperture round100m2

   m_rule     = stencilRuleUndefined;
   m_modifiers.empty();

   int numpar = params.GetCount();

   if (ruleStartIndex >= 0)
   {
      m_rule = ruleStringToStencilRuleTag(params[ruleStartIndex++]);

      switch (m_rule)
      {
      case stencilRuleNone:
      case stencilRuleCopperSourceLayer:
      case stencilRuleStencilSourceLayer:
      case stencilRulePassthru:
      case stencilRuleInheritFromParent:
         break;
      case stencilRuleInset:
         if (numpar > ruleStartIndex)
         {
            CSupString ruleValue(params[ruleStartIndex++]);
            CStringArray ruleValueParams;

            ruleValue.Remove('"');
            int numValueParams = ruleValue.ParseWhite(ruleValueParams);

            m_ruleValue = ruleValueParams[0];

            if (numValueParams > 1)
            {
               CString unitsString = ruleValueParams[1];
               PageUnitsTag rulePageUnits = unitStringAbbreviationToTag(unitsString);

               if (rulePageUnits != pageUnitsUndefined)
               {
                  m_ruleValue += " " + unitsString;
               }
            }
         }
         else
         {
            m_rule = stencilRuleUndefined;
         }

         break;
      case stencilRuleArea:
      case stencilRuleAperture:
      case stencilRuleApertureGeometry:
         if (numpar > ruleStartIndex)
         {
            m_ruleValue = params[ruleStartIndex++];
         }
         else
         {
            m_rule = stencilRuleUndefined;
         }

         break;
      }

      while (numpar > ruleStartIndex)
      {
         CStencilRuleModifierValues modifierValues;

         StencilRuleModifierTag modifier = parseStencilRuleModifierAttribute(params[ruleStartIndex++],modifierValues);
         m_modifiers.addModifier(modifier,modifierValues);
      }
   }

   return isValid();
}

StencilRuleModifierTag CStencilRule::getModifier() const
{
   StencilRuleModifierTag stencilRuleModifierTag = stencilRuleModifierUndefined;

   if (m_modifiers.GetCount() > 0)
   {
      stencilRuleModifierTag = m_modifiers.GetHead()->getModifier();
   }

   return stencilRuleModifierTag;
}

void CStencilRule::setModifier(StencilRuleModifierTag modifier,const CStencilRuleModifierValues& modifierValues)
{
   m_modifiers.setModifier(modifier,modifierValues);
}

void CStencilRule::getModifierValues(CStencilRuleModifierValues& modifierValues) const
{
   if (m_modifiers.GetCount() > 0)
   {
      modifierValues.copy(m_modifiers.GetHead()->getModifierValues());
   }
   else
   {
      modifierValues.empty();
   }
}

CString CStencilRule::getModifiersString() const
{
   CString modifiersString;

   for (POSITION pos = m_modifiers.GetHeadPosition();pos != NULL;)
   {
      CStencilRuleModifier* modifier = m_modifiers.GetNext(pos);

      modifiersString += (modifiersString.IsEmpty() ? "" : ";") + modifier->getModifierString();
   }

   return modifiersString;
}

CString CStencilRule::getRuleString(PageUnitsTag pageUnits) const
{
   CString ruleString;
   CString space(" ");

   if (isValid())
   {
      ruleString = quoteString(stencilRuleTagToRuleString(m_rule));

      CString ruleValue(m_ruleValue);

      if (m_rule == stencilRuleInset && pageUnits != pageUnitsUndefined && !ruleValue.IsEmpty())
      {
         ruleValue += space + unitStringAbbreviation(pageUnits);
      }

      if (! ruleValue.IsEmpty())
      {
         ruleString += space + quoteString(ruleValue);
      }

      for (POSITION pos = m_modifiers.GetHeadPosition();pos != NULL;)
      {
         CStencilRuleModifier* stencilRuleModifier = m_modifiers.GetNext(pos);
         ruleString += space + stencilRuleModifier->getModifierString();
      }
   }

   return ruleString;
}

//_____________________________________________________________________________
CStencilEntityRule::CStencilEntityRule()
{
}

CStencilEntityRule::CStencilEntityRule(const CStencilEntityRule& other)
{
   *this = other;
}

CStencilEntityRule::CStencilEntityRule(const CStencilRule& other)
{
   CStencilRule::operator=(other);
}

CStencilEntityRule::CStencilEntityRule(const CString& ruleString)
{
   setRuleString(ruleString);
}

CStencilEntityRule& CStencilEntityRule::operator=(const CStencilEntityRule& other)
{
   if (&other != this)
   {
      CStencilRule::operator=(other);

      m_entity            = other.m_entity;
      setEntityDesignator1(other.m_entityDesignator1);
      setEntityDesignator2(other.m_entityDesignator2);
      setEntityDesignator3(other.m_entityDesignator3);
      setEntityDesignator4(other.m_entityDesignator4);
      setEntityDesignator5(other.m_entityDesignator5);
   }

   return *this;
}

AttributeSourceTag CStencilEntityRule::getEntity() const
{
   return m_entity;
}

void CStencilEntityRule::setEntity(AttributeSourceTag entity)
{
   m_entity = entity;
}

void CStencilEntityRule::setEntity(AttributeSourceTag entity,const CString& entityDesignator1)
{
   m_entity            = entity;
   setEntityDesignator1(entityDesignator1);
   m_entityDesignator2.Empty();
   m_entityDesignator3.Empty();
   m_entityDesignator4.Empty();
   m_entityDesignator5.Empty();
}

void CStencilEntityRule::setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2)
{
   m_entity            = entity;
   setEntityDesignator1(entityDesignator1);
   setEntityDesignator2(entityDesignator2);
   m_entityDesignator3.Empty();
   m_entityDesignator4.Empty();
   m_entityDesignator5.Empty();
}

void CStencilEntityRule::setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2,const CString& entityDesignator3)
{
   m_entity            = entity;
   setEntityDesignator1(entityDesignator1);
   setEntityDesignator2(entityDesignator2);
   setEntityDesignator3(entityDesignator3);
   m_entityDesignator4.Empty();
   m_entityDesignator5.Empty();
}

void CStencilEntityRule::setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2,const CString& entityDesignator3,
                                                             const CString& entityDesignator4)
{
   m_entity            = entity;
   setEntityDesignator1(entityDesignator1);
   setEntityDesignator2(entityDesignator2);
   setEntityDesignator3(entityDesignator3);
   setEntityDesignator4(entityDesignator4);
   m_entityDesignator5.Empty();
}

void CStencilEntityRule::setEntity(AttributeSourceTag entity,const CString& entityDesignator1,const CString& entityDesignator2,const CString& entityDesignator3,
                                                             const CString& entityDesignator4,const CString& entityDesignator5)
{
   m_entity            = entity;
   setEntityDesignator1(entityDesignator1);
   setEntityDesignator2(entityDesignator2);
   setEntityDesignator3(entityDesignator3);
   setEntityDesignator4(entityDesignator4);
   setEntityDesignator5(entityDesignator5);
}

CString CStencilEntityRule::getEntityString() const
{
   return attributeSourceToString(m_entity);
}

CString CStencilEntityRule::getEntityDesignator1() const
{
   return m_entityDesignator1;
}

CString CStencilEntityRule::getEntityDesignator2() const
{
   return m_entityDesignator2;
}

CString CStencilEntityRule::getEntityDesignator3() const
{
   return m_entityDesignator3;
}

CString CStencilEntityRule::getEntityDesignator4() const
{
   return m_entityDesignator4;
}

CString CStencilEntityRule::getEntityDesignator5() const
{
   return m_entityDesignator5;
}

void CStencilEntityRule::setEntityDesignator1(const CString& designator)
{
   m_entityDesignator1 = designator;
}

void CStencilEntityRule::setEntityDesignator2(const CString& designator)
{
   m_entityDesignator2 = designator;
}

void CStencilEntityRule::setEntityDesignator3(const CString& designator)
{
   m_entityDesignator3 = designator;
}

void CStencilEntityRule::setEntityDesignator4(const CString& designator)
{
   m_entityDesignator4 = designator;
}

void CStencilEntityRule::setEntityDesignator5(const CString& designator)
{
   m_entityDesignator5 = designator;
}

bool CStencilEntityRule::setRuleString(const CString& ruleString)
{
   // pcb Inset 0.050000
   // surface top Area 0.900000
   // mount top.smd Inset 0.040000
   // componentGeometry 0603 Aperture round100m2
   // //padstackGeometry PADSTACK_64 Area 0.700000
   // padstack 0603.PADSTACK_64 Area 0.700000
   // subPadstack 0603.PADSTACK_64.1 Area 0.700000
   // component C5 Inset 0.030000
   // geometryPin 0603.1 Area 0.600000 -- old style
   // geometryPin top.0603.1.1 Area 0.600000 -- new style for subpin support
   // componentPin C5.1 Area 0.600000
   // componentPin C263.1 Aperture tabbedRectangle200x100x75x50x2corm2

   CSupString ruleSupString(ruleString);
   ruleSupString.Trim();
   CStringArray params,quotedParams;
   const CString dot(".");
   int ruleStartIndex = -1;

   // rcf - 20071121 - Case dts0100447220
   // Parse into fields as usual but leave the double quotes intact.
   // Problem in case was an item with name in form xxxx."yyyy.zzzz".
   // The old ParseQuote here would return it as xxxx.yyyy.zzzz, which then
   // becomes ambiguous and failed to parse further later on. I.e. in the
   // QPadstack case, instead of getting exactly 2 fields we woudl get three
   // and processing would stop, presuming some sort of malformation error.
   //
   // So leave quotes in place at this stage so later stages can tell the
   // difference, and remove the quotes later as needed.
   //
   // knv - 20080125
   // As originally designed, the entity rule strings used spaces to delimit parameters at the rule level and periods
   // to delimit components of the entity designator.  
   // In order to accomodate these two delimiter characters to be embedded within parameters, double quoting was used.
   // However, the CSupString::ParseQuote() routine used to divide the rule string into parameters using the space delimiter removed
   // double quoting used at the entity designator level.
   // The fix for case dts0100447220 left the quotes intact for all parameters (except the the first "entity" parameter).
   // This potentially left quoted parameters in the rule string still quoted.
   // This latest fix reverts back to removing all quotes from the parsed parameters, but performs a second parse to preserve quotes for the designator parameter.
   int numpar  = ruleSupString.ParseQuote(params," ");
   int numparq = ruleSupString.ParseQuotePreserveQuotes(quotedParams," ");

   if (numpar > 0)
   {
      CString entity = params[0];

      if (numpar > 1)
      {
         params.SetAt(1,quotedParams.GetAt(1)); // designator parameter
      }

      if (entity.CompareNoCase(QMount) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            // mount top.smd Inset 0.040000
            if (numDesignatorParams == 2)
            {
               CString& surface   = designatorParams[0];
               CString& mountType = designatorParams[1];

               if (((surface.CompareNoCase(QTop)  == 0) || (surface.CompareNoCase(QBottom) == 0)) &&
                  ((mountType.CompareNoCase(QSmd) == 0) || (mountType.CompareNoCase(QTh)   == 0) || (mountType.CompareNoCase(QVia)   == 0))     )
               {
                  m_entity = attributeSourceMount;
                  setEntityDesignator1(surface);
                  setEntityDesignator2(mountType);

                  ruleStartIndex = 2;
               }
            }
         }
      }
      else if (entity.CompareNoCase(QPadstack ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            // padstack top.0603.1 Area 0.700000
            if (numDesignatorParams == 3)
            {
               m_entity = attributeSourcePadstack;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);
               setEntityDesignator3(designatorParams[2]);

               ruleStartIndex = 2;
            }
         }
      }
      else if (entity.CompareNoCase(QSubPadstack ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            // subPadstack top.0603.1.1 Area 0.700000
            if (numDesignatorParams == 4)
            {
               m_entity = attributeSourceSubPadstack;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);
               setEntityDesignator3(designatorParams[2]);
               setEntityDesignator4(designatorParams[3]);

               ruleStartIndex = 2;
            }
         }
      }
      else if (entity.CompareNoCase(QSurface ) == 0)
      {
         if (numpar > 2)
         {
            CString& surface = params[1];

            // surface top Area 0.900000
            if ((surface.CompareNoCase(QTop) == 0) || (surface.CompareNoCase(QBottom) == 0))
            {
               m_entity = attributeSourceSurface;
               setEntityDesignator1(surface);

               ruleStartIndex = 2;
            }
         }
      }
      else if (entity.CompareNoCase(QGeometry ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            if (numDesignatorParams == 2)
            {
               // componentGeometry top.0603 Aperture round100m2
               m_entity = attributeSourceGeometry;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);

               ruleStartIndex = 2;
            }
         }
      }
      else if (entity.CompareNoCase(QGeometryPin ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            if (numDesignatorParams == 4)
            {
               // geometryPin top.0603.1.1 Area 0.600000 -- new style for subpin support
               m_entity = attributeSourceGeometryPin;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);
               setEntityDesignator3(designatorParams[2]);
               setEntityDesignator4(designatorParams[3]);

               ruleStartIndex = 2;
            }
            else if (numDesignatorParams == 2)
            {
               // geometryPin 0603.1 Area 0.600000 -- old style
               //formatMessageBox("CStencilEntityRule::setRuleString() - Geometry Pin rule with 2 designator parameters encountered");
            }
            else if (numDesignatorParams == 3)
            {
               // geometryPin 0603.PADSTACK_64.1 Area 0.600000 -- old style
               //formatMessageBox("CStencilEntityRule::setRuleString() - Geometry Pin rule with 3 designator parameters encountered");
            }
         }
      }
      else if (entity.CompareNoCase(QComponent ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            if (numDesignatorParams == 2)
            {
               // component top.C5 Inset 0.030000
               m_entity = attributeSourceComponent;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);
            }

            ruleStartIndex = 2;
         }
      }
      else if (entity.CompareNoCase(QComponentPin ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            if (numDesignatorParams == 3)
            {
               // componentPin top.C5.1 Area 0.600000
               // componentPin top.C263.1 Aperture tabbedRectangle200x100x75x50x2corm2
               m_entity = attributeSourceComponentPin;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);
               setEntityDesignator3(designatorParams[2]);

               ruleStartIndex = 2;
            }
         }
      }
      else if (entity.CompareNoCase(QComponentSubPin ) == 0)
      {
         if (numpar > 2)
         {
            CSupString designator(params[1]);
            CStringArray designatorParams;

            int numDesignatorParams = designator.ParseQuote(designatorParams,dot);

            if (numDesignatorParams == 3)
            {
               // componentSubPin top.C5.1.1 Area 0.600000
               // componentPin Ctop.263.1.2 Aperture tabbedRectangle200x100x75x50x2corm2
               m_entity = attributeSourceComponentSubPin;
               setEntityDesignator1(designatorParams[0]);
               setEntityDesignator2(designatorParams[1]);
               setEntityDesignator3(designatorParams[2]);
               setEntityDesignator4(designatorParams[3]);

               ruleStartIndex = 2;
            }
         }
      }
      else if (entity.CompareNoCase(QPcb ) == 0)
      {
         if (numpar > 1)
         {
            // pcb Inset 0.050000
            m_entity = attributeSourcePcb;

            if (numpar > 2 && params[1].CompareNoCase("pcb") == 0)
            {
               // pcb pcb Inset 0.050000
               setEntityDesignator1("pcb");

               ruleStartIndex = 2;
            }
            else
            {
               ruleStartIndex = 1;
            }
         }
      }
   }

   setRuleFromParameters(params,ruleStartIndex);

   return isValid();
}

// <StencilEntityRuleString>            := <AttributeSourceTagString> <space> <EntityDesignator> <space> <RuleString>
// <RuleString>                         := <StencilRuleTagString> <space> <StencilRuleValue> <space> <StencilRuleModifiersString>
// <StencilRuleModifiersString>         := <StencilRuleModifierString> | <StencilRuleModifierString> <space> <StencilRuleModifiersString>
// <StencilRuleModifierString>          := <StencilRuleModifierTagString> <stencilRuleModifierValueListString>
// <stencilRuleModifierValueListString> := <empty> | "(" <StencilRuleModifierValues> ")"
// <StencilRuleModifierValues>          := <StencilRuleModifierValue> | <StencilRuleModifierValue> "," <StencilRuleModifierValues>
// <StencilRuleModifierValue>           :=

CString CStencilEntityRule::getRuleString(PageUnitsTag pageUnits) const
{
   CString ruleString;
   CString space(" ");

   if (isValid())
   {
      ruleString = attributeSourceToString(m_entity) + space + getEntityDesignator() + space + CStencilRule::getRuleString(pageUnits);
      //ruleString = attributeSourceToString(m_entity) + ((m_entity == attributeSourcePcb) ? space : space + getEntityDesignator() + space) + CStencilRule::getRuleString(pageUnits);
   }

   return ruleString;
}

CString CStencilEntityRule::getEntityDesignator() const
{
   CString entityDesignator = getDesignatorString(m_entityDesignator1);

   if (! m_entityDesignator2.IsEmpty())
   {
      entityDesignator += getRefDelimiter() + getDesignatorString(m_entityDesignator2);

      if (! m_entityDesignator3.IsEmpty())
      {
         entityDesignator += getRefDelimiter() + getDesignatorString(m_entityDesignator3);

         if (! m_entityDesignator4.IsEmpty())
         {
            entityDesignator += getRefDelimiter() + getDesignatorString(m_entityDesignator4);

            if (! m_entityDesignator5.IsEmpty())
            {
               entityDesignator += getRefDelimiter() + getDesignatorString(m_entityDesignator5);
            }
         }
      }
   }

   return entityDesignator;
}

//_____________________________________________________________________________

CStencilEntityRule* CStencilEntityRuleArray::FindEntityRule(CStencilEntityRule* rule)
{
   // Return rule with matching designators or NULL if not found

   if (rule != NULL)
   {
      for (int i = 0; i < this->GetCount(); i++)
      {
         CStencilEntityRule* candidate = this->GetAt(i);

         if (candidate->getEntityDesignator().Compare(rule->getEntityDesignator()) == 0)
         {
            return candidate;
         }
      }
   }

   return NULL;
}

//_____________________________________________________________________________
int CStencilRuleAttributes::m_nextId = 0;

int CStencilRuleAttributes::m_stencilPadNoneKeywordIndex                  = -1;
int CStencilRuleAttributes::m_stencilPadAreaKeywordIndex                  = -1;
int CStencilRuleAttributes::m_stencilPadInsetKeywordIndex                 = -1;
int CStencilRuleAttributes::m_stencilPadApertureKeywordIndex              = -1;
int CStencilRuleAttributes::m_stencilPadApertureGeometryKeywordIndex      = -1;
int CStencilRuleAttributes::m_stencilPadStencilSourceLayerKeywordIndex    = -1;
int CStencilRuleAttributes::m_stencilPadPassthruKeywordIndex              = -1;
int CStencilRuleAttributes::m_stencilPadCopperSourceLayerKeywordIndex     = -1;
int CStencilRuleAttributes::m_stencilPadInheritFromParentKeywordIndex     = -1;
int CStencilRuleAttributes::m_stencilPadApertureModifierKeywordIndex      = -1;
int CStencilRuleAttributes::m_stencilPadCommentKeywordIndex               = -1;
//int CStencilRuleAttributes::m_stencilPadGroupKeywordIndex               = -1;
int CStencilRuleAttributes::m_stencilPadUndefinedKeywordIndex             = -1;

CStencilRuleAttributes::CStencilRuleAttributes(CCamCadDatabase& camCadDatabase,CAttributes& attributeMap)
: m_camCadDatabase(&camCadDatabase)
, m_attributeMap(&attributeMap)
{
   m_id = m_nextId++;

   if (m_attributeMap == NULL)
   {
      m_allocatedAttributeMap = new CAttributes();
      m_attributeMap = m_allocatedAttributeMap;
   }
   else
   {
      m_allocatedAttributeMap = NULL;
   }

   parseAttributes();

   isValid();
}

CStencilRuleAttributes::CStencilRuleAttributes(CCamCadDatabase& camCadDatabase,CAttributes* attributeMap)
: m_camCadDatabase(&camCadDatabase)
, m_attributeMap(attributeMap)
{
   m_id = m_nextId++;

   if (m_attributeMap == NULL)
   {
      m_allocatedAttributeMap = new CAttributes();
      m_attributeMap = m_allocatedAttributeMap;
   }
   else
   {
      m_allocatedAttributeMap = NULL;
   }

   parseAttributes();

   isValid();
}

CStencilRuleAttributes::CStencilRuleAttributes(const CStencilRuleAttributes& other)
{
   m_id = m_nextId++;

   *this = other;

   isValid();
}

CStencilRuleAttributes::~CStencilRuleAttributes()
{
   delete m_allocatedAttributeMap;
}

CStencilRuleAttributes& CStencilRuleAttributes::operator=(const CStencilRuleAttributes& other)
{
   if (&other != this)
   {
      m_camCadDatabase = other.m_camCadDatabase;
      m_attributeMap   = other.m_attributeMap;
      m_stencilRule    = other.m_stencilRule;

      if (other.m_allocatedAttributeMap != NULL || m_attributeMap == NULL)
      {
         m_allocatedAttributeMap = new CAttributes();
         m_attributeMap = m_allocatedAttributeMap;
      }
      else
      {
         m_allocatedAttributeMap = NULL;
      }
   }

   isValid();

   return *this;
}

void CStencilRuleAttributes::resetKeywordIndices()
{
   m_stencilPadNoneKeywordIndex                  = -1;
   m_stencilPadAreaKeywordIndex                  = -1;
   m_stencilPadInsetKeywordIndex                 = -1;
   m_stencilPadApertureKeywordIndex              = -1;
   m_stencilPadApertureGeometryKeywordIndex      = -1;
   m_stencilPadCopperSourceLayerKeywordIndex     = -1;
   m_stencilPadStencilSourceLayerKeywordIndex    = -1;
   m_stencilPadPassthruKeywordIndex              = -1;
   m_stencilPadApertureModifierKeywordIndex      = -1;
   m_stencilPadInheritFromParentKeywordIndex     = -1;
   m_stencilPadCommentKeywordIndex               = -1;
   m_stencilPadUndefinedKeywordIndex             = -1;
}

void CStencilRuleAttributes::registerStencilKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog)
{
   m_stencilPadNoneKeywordIndex                  = camCadDatabase.registerHiddenKeyWord("StencilPadNone"              ,0,valueTypeNone      ,errorLog);
   m_stencilPadAreaKeywordIndex                  = camCadDatabase.registerHiddenKeyWord("StencilPadArea"              ,0,valueTypeDouble    ,errorLog);
   m_stencilPadInsetKeywordIndex                 = camCadDatabase.registerHiddenKeyWord("StencilPadInset"             ,0,valueTypeUnitDouble,errorLog);
   m_stencilPadApertureKeywordIndex              = camCadDatabase.registerHiddenKeyWord("StencilPadAperture"          ,0,valueTypeString    ,errorLog);
   m_stencilPadApertureGeometryKeywordIndex      = camCadDatabase.registerHiddenKeyWord("StencilPadApertureGeometry"  ,0,valueTypeString    ,errorLog);
   m_stencilPadStencilSourceLayerKeywordIndex    = camCadDatabase.registerHiddenKeyWord("StencilPadStencilSourceLayer",0,valueTypeNone      ,errorLog);
   m_stencilPadPassthruKeywordIndex              = camCadDatabase.registerHiddenKeyWord("StencilPadPassthru"          ,0,valueTypeNone      ,errorLog);
   m_stencilPadCopperSourceLayerKeywordIndex     = camCadDatabase.registerHiddenKeyWord("StencilPadCopperSourceLayer" ,0,valueTypeNone      ,errorLog);
   m_stencilPadInheritFromParentKeywordIndex     = camCadDatabase.registerHiddenKeyWord("StencilPadInheritFromParent" ,0,valueTypeNone      ,errorLog);
   m_stencilPadApertureModifierKeywordIndex      = camCadDatabase.registerHiddenKeyWord("StencilPadApertureModifier"  ,0,valueTypeString    ,errorLog);
   m_stencilPadCommentKeywordIndex               = camCadDatabase.registerHiddenKeyWord("StencilPadComment"           ,0,valueTypeString    ,errorLog);
 //m_stencilPadGroupKeywordIndex                 = camCadDatabase.registerHiddenKeyWord("StencilPadGroup"             ,0,valueTypeString    ,errorLog);
   m_stencilPadUndefinedKeywordIndex             = camCadDatabase.registerHiddenKeyWord("StencilPadUndefined"         ,0,valueTypeNone      ,errorLog);
}

int CStencilRuleAttributes::getKeywordIndex(StencilRuleTag tagValue)
{
   int keywordIndex = -1;

   switch (tagValue)
   {
   case stencilRuleNone:                      keywordIndex = m_stencilPadNoneKeywordIndex;                   break;
   case stencilRuleArea:                      keywordIndex = m_stencilPadAreaKeywordIndex;                   break;
   case stencilRuleInset:                     keywordIndex = m_stencilPadInsetKeywordIndex;                  break;
   case stencilRuleAperture:                  keywordIndex = m_stencilPadApertureKeywordIndex;               break;
   case stencilRuleApertureGeometry:          keywordIndex = m_stencilPadApertureGeometryKeywordIndex;       break;
   case stencilRuleStencilSourceLayer:        keywordIndex = m_stencilPadStencilSourceLayerKeywordIndex;     break;
   case stencilRulePassthru:                  keywordIndex = m_stencilPadPassthruKeywordIndex;               break;
   case stencilRuleCopperSourceLayer:         keywordIndex = m_stencilPadCopperSourceLayerKeywordIndex;      break;
   case stencilRuleInheritFromParent:         keywordIndex = m_stencilPadInheritFromParentKeywordIndex;      break;
 //case stencilAttributeApertureModifier:          keywordIndex = m_stencilPadApertureModifierKeywordIndex;       break;
   }

   return keywordIndex;
}

void CStencilRuleAttributes::empty()
{
   removeStencilAttributes();
   m_stencilRule.setRule(stencilRuleUndefined);
}

bool CStencilRuleAttributes::isValid()
{
   bool retval = (m_camCadDatabase != NULL) && (m_attributeMap != NULL);

   return retval;
}

void CStencilRuleAttributes::parseAttributes()
{
   m_stencilRule.setRule(stencilRuleUndefined);

   StencilRuleTag rule      = stencilRuleUnknown;
   Attrib* stencilAttribute = NULL;

   isValid();

   if      (m_attributeMap->Lookup(m_stencilPadUndefinedKeywordIndex       ,stencilAttribute))
   {
      rule = stencilRuleUndefined;
   }
   else if (m_attributeMap->Lookup(m_stencilPadApertureKeywordIndex        ,stencilAttribute))
   {
      rule = stencilRuleAperture;
   }
   else if (m_attributeMap->Lookup(m_stencilPadApertureGeometryKeywordIndex,stencilAttribute))
   {
      rule = stencilRuleApertureGeometry;
   }
   else if (m_attributeMap->Lookup(m_stencilPadStencilSourceLayerKeywordIndex,stencilAttribute))
   {
      rule = stencilRuleStencilSourceLayer;
   }
   else if (m_attributeMap->Lookup(m_stencilPadPassthruKeywordIndex,stencilAttribute))
   {
      rule = stencilRulePassthru;
   }
   else if (m_attributeMap->Lookup(m_stencilPadCopperSourceLayerKeywordIndex,stencilAttribute))
   {
      rule = stencilRuleCopperSourceLayer;
   }
   else if (m_attributeMap->Lookup(m_stencilPadInsetKeywordIndex           ,stencilAttribute))
   {
      rule = stencilRuleInset;
   }
   else if (m_attributeMap->Lookup(m_stencilPadAreaKeywordIndex            ,stencilAttribute))
   {
      rule = stencilRuleArea;
   }
   else if (m_attributeMap->Lookup(m_stencilPadNoneKeywordIndex            ,stencilAttribute))
   {
      rule = stencilRuleNone;
   }
   else if (m_attributeMap->Lookup(m_stencilPadInheritFromParentKeywordIndex,stencilAttribute))
   {
      rule = stencilRuleInheritFromParent;
   }

   if (stencilAttribute != NULL)
   {
      m_stencilRule.setRule(rule);
      CString ruleValue = m_camCadDatabase->getAttributeStringValue(stencilAttribute);
      m_stencilRule.setRuleValue(ruleValue);

      Attrib* stencilModifierAttribute = NULL;

      if (m_attributeMap->Lookup(m_stencilPadApertureModifierKeywordIndex,stencilModifierAttribute))
      {
         CSupString modifierValue = m_camCadDatabase->getAttributeStringValue(stencilModifierAttribute);
         CStringArray modifierStrings;
         int modifierCount = modifierValue.Parse(modifierStrings,";");

         for (int modifierIndex = 0;modifierIndex < modifierCount;modifierIndex++)
         {
            CString modifierString = modifierStrings.GetAt(modifierIndex);

            m_stencilRule.addModifier(modifierString);
         }
      }

      Attrib* stencilCommentAttribute = NULL;

      if (m_attributeMap->Lookup(m_stencilPadCommentKeywordIndex,stencilCommentAttribute))
      {
         CString comment = m_camCadDatabase->getAttributeStringValue(stencilCommentAttribute);

         m_stencilRule.setComment(comment);
      }
   }
}

bool CStencilRuleAttributes::setRule(const CStencilRule& stencilRule)
{
   m_stencilRule = stencilRule;

   setAttributes();

   return m_stencilRule.isValid();
}

const CStencilRule& CStencilRuleAttributes::getRule() const
{
   return m_stencilRule;
}

CString CStencilRuleAttributes::getRuleValue() const
{
   return m_stencilRule.getRuleValue();
}

void CStencilRuleAttributes::removeStencilAttributes()
{
   removeStencilAttributes(*m_attributeMap);
}

void CStencilRuleAttributes::removeStencilAttributes(CAttributes& attributes)
{
   WORD key;
   Attrib* attribute;
   POSITION previousPos;

   for (POSITION pos = attributes.GetStartPosition();pos != NULL;)
   {
      previousPos = pos;
      attributes.GetNextAssoc(pos,key,attribute);

      if (key == m_stencilPadNoneKeywordIndex               ||
          key == m_stencilPadAreaKeywordIndex               ||
          key == m_stencilPadInsetKeywordIndex              ||
          key == m_stencilPadApertureKeywordIndex           ||
          key == m_stencilPadApertureGeometryKeywordIndex   ||
          key == m_stencilPadStencilSourceLayerKeywordIndex ||
          key == m_stencilPadPassthruKeywordIndex           ||
          key == m_stencilPadCopperSourceLayerKeywordIndex  ||
          key == m_stencilPadApertureModifierKeywordIndex   ||
          key == m_stencilPadInheritFromParentKeywordIndex  ||
          key == m_stencilPadCommentKeywordIndex            ||
          key == m_stencilPadUndefinedKeywordIndex              )
      {
         //delete attribute;
         attributes.deleteAttribute(key);
      }
   }
}

void CStencilRuleAttributes::setAttributes()
{
   removeStencilAttributes();

   if (m_stencilRule.getRule() != stencilRuleUndefined)
   {
      int keywordIndex = getKeywordIndex(m_stencilRule.getRule());
      CString value = m_stencilRule.getRuleValue();

      m_camCadDatabase->addAttribute(*m_attributeMap,keywordIndex,value);

      if (m_stencilRule.hasModifiers())
      {
         keywordIndex = getStencilPadApertureModifierKeywordIndex();
         value = m_stencilRule.getModifiersString();

         m_camCadDatabase->addAttribute(*m_attributeMap,keywordIndex,value);
      }

      if (! m_stencilRule.getComment().IsEmpty())
      {
         keywordIndex = getStencilPadCommentKeywordIndex();

         m_camCadDatabase->addAttribute(*m_attributeMap,keywordIndex,m_stencilRule.getComment());
      }
   }
}

bool CStencilRuleAttributes::hasRule() const
{
   return (m_stencilRule.getRule() != stencilRuleUndefined &&
           m_stencilRule.getRule() != stencilRuleUnknown      );
}

bool CStencilRuleAttributes::hasNonInheritedRule() const
{
   return (m_stencilRule.getRule() != stencilRuleUndefined &&
           m_stencilRule.getRule() != stencilRuleUnknown   &&
           m_stencilRule.getRule() != stencilRuleInheritFromParent);
}

//_____________________________________________________________________________
CStencilGroup::CStencilGroup(const CString& groupName,Attrib* stencilAttribute,int stencilKeyword)
{
   m_groupName        = groupName;
   m_stencilAttribute = stencilAttribute;
   m_stencilKeyword   = stencilKeyword;
}

//_____________________________________________________________________________
CStencilGroup* CStencilGroups::addGroup(const CString& groupName,Attrib* stencilAttribute,int stencilKeyword)
{
   CStencilGroup* stencilGroup;

   if (! m_groups.Lookup(groupName,stencilGroup))
   {
      stencilGroup = new CStencilGroup(groupName,stencilAttribute,stencilKeyword);
      m_groups.SetAt(groupName,stencilGroup);
   }

   return stencilGroup;
}

bool CStencilGroups::lookup(const CString& groupName,CStencilGroup*& stencilGroup)
{
   bool retval = (m_groups.Lookup(groupName,stencilGroup) != 0);

   return retval;
}

//_____________________________________________________________________________
//CSplitStencilHole::CSplitStencilHole(CCamCadDatabase& camCadDatabase,
//   FileStruct& fileStruct,DataStruct& pad,CExtent& padExtent,
//   BlockStruct& padGeometry,double maxFeatureSize,double webSize,double webAngleDegrees,StencilSplitTypeTag splitType)
//{
//   m_stencilHole = NULL;
//
//   // n*SplitfeatureSize + (n - 1)*webSize = featureSize
//   m_xSplits    = 1 + (int)((padExtent.getXsize() + webSize - (maxFeatureSize/10000.)) / (maxFeatureSize + webSize));
//   m_xSplitSize = ((m_xSplits > 1) ? (padExtent.getXsize() - ((m_xSplits - 1) * webSize))/m_xSplits : padExtent.getXsize());
//   m_ySplits    = 1 + (int)((padExtent.getYsize() + webSize - (maxFeatureSize/10000.)) / (maxFeatureSize + webSize));
//   m_ySplitSize = ((m_ySplits > 1) ? (padExtent.getYsize() - ((m_ySplits - 1) * webSize))/m_ySplits : padExtent.getYsize());
//
//   CString splitGeometryName;
//   splitGeometryName.Format("%s_split_%d_%d",(const char*)padGeometry.getName(),
//      m_xSplits,m_ySplits);
//
//   m_geometry = camCadDatabase.getBlock(splitGeometryName);
//
//   if (m_geometry == NULL)
//   {
//      CString subGeometryName;
//      subGeometryName.Format("%s_sub_%d_%d",(const char*)padGeometry.getName(),
//         m_xSplits,m_ySplits);
//
//      BlockStruct* subPadGeometry = camCadDatabase.getDefinedAperture(subGeometryName,
//         padGeometry.getShape(),m_xSplitSize,m_ySplitSize);
//
//      m_geometry = camCadDatabase.getDefinedBlock(splitGeometryName,
//                      BLOCKTYPE_UNKNOWN,fileStruct.getFileNumber());
//
//      double dx = m_xSplitSize + webSize;
//      double dy = m_ySplitSize + webSize;
//      double x0 = (1 - m_xSplits) * (dx / 2.);
//      double y0 = (1 - m_ySplits) * (dy / 2.);
//
//      for (int xInd = 0;xInd < m_xSplits;xInd++)
//      {
//         for (int yInd = 0;yInd < m_ySplits;yInd++)
//         {
//            double x = x0 + (xInd * dx);
//            double y = y0 + (yInd * dy);
//
//            camCadDatabase.referenceBlock(m_geometry,subPadGeometry,
//               INSERTTYPE_STENCILHOLE,"",-1,x,y);
//         }
//      }
//   }
//
//   m_stencilHole =
//      camCadDatabase.insertBlock(m_geometry,
//         INSERTTYPE_STENCILHOLE,"",pad.getLayerIndex(),
//         pad.getInsert()->getOrigin().x,
//         pad.getInsert()->getOrigin().y,pad.getInsert()->getAngle(),
//         pad.getInsert()->getGraphicMirrored(),pad.getInsert()->getScale());
//}

CSplitStencilHole::CSplitStencilHole(CCamCadDatabase& camCadDatabase,
   FileStruct& fileStruct,DataStruct& pad,BlockStruct& padGeometry,
   double maxFeatureSize,double webSize,double webAngleDegrees,StencilSplitTypeTag splitType,double exteriorCornerRadius)
: m_camCadDatabase(camCadDatabase)
{
   m_stencilHole = NULL;

   //CExtent padExtent = padGeometry.getExtent();
   //CPolyList* allocatedAperturePolyList = NULL;
   CPolyList* aperturePolyList = NULL;

   if (padGeometry.isAperture())
   {
      if (padGeometry.isComplexAperture())
      {
         BlockStruct* padApertureGeometry = camCadDatabase.getBlock(padGeometry.getComplexApertureSubBlockNumber());

         for (CDataListIterator polyListIterator(*padApertureGeometry,dataTypePoly);polyListIterator.hasNext();)
         {
            DataStruct* polyStruct = polyListIterator.getNext();
            aperturePolyList = new CPolyList(*(polyStruct->getPolyList()));
            //aperturePolyList = polyStruct->getPolyList();

            break;
         }
      }
      else
      {
         aperturePolyList = padGeometry.convertApertureToPoly();
         //allocatedAperturePolyList = ApertureToPoly(&padGeometry);
         //aperturePolyList = allocatedAperturePolyList;
      }
   }
   else
   {
      for (CDataListIterator polyListIterator(padGeometry,dataTypePoly);polyListIterator.hasNext();)
      {
         DataStruct* polyStruct = polyListIterator.getNext();
         aperturePolyList = new CPolyList(*(polyStruct->getPolyList()));
         //aperturePolyList = polyStruct->getPolyList();

         break;
      }
   }

   if (aperturePolyList != NULL)
   {
      int iWebAngleDegrees = round(webAngleDegrees);
      CTMatrix rotationMatrix;
      rotationMatrix.rotateDegrees(-iWebAngleDegrees);

      aperturePolyList->transform(rotationMatrix);

      CExtent padExtent = aperturePolyList->getExtent( camCadDatabase.getCamCadData() );

      rotationMatrix.invert();

      bool stripeFlag = (splitType == splitTypeStripe);
      double maxExtent = (stripeFlag ? padExtent.getYsize() : max(padExtent.getXsize(),padExtent.getYsize()));

      if (maxExtent <= 0.) maxExtent = 1.;

      // Case 400301, note 3, to reduce jaggies in curves a little, increased relative resolution a little, to 1000 up from 400.
      double pixelsPerPageUnit = 1000./maxExtent;

      double insetWebSize = webSize + exteriorCornerRadius*2.;
      int webSizePixels = round(ceil(insetWebSize * pixelsPerPageUnit));

      if (webSizePixels < 1) webSizePixels = 1;

      pixelsPerPageUnit = webSizePixels / insetWebSize;

      int maxFeatureSizePixels = (int)(maxFeatureSize * pixelsPerPageUnit);

      // Case dts0100472908, see line width usage in splitRegion().
      int cornerRadiusPixels = round(ceil(exteriorCornerRadius * pixelsPerPageUnit));
      int lineWidthPixels = cornerRadiusPixels * 2;

      CRegionPolygon regionPolygon(camCadDatabase.getCamCadDoc(), *aperturePolyList, pixelsPerPageUnit);

      CSplitRegionPolygons regionPolygons;

      if (! regionPolygon.splitRegion(regionPolygons, lineWidthPixels, maxFeatureSizePixels, webSizePixels, stripeFlag))
      {
         CDebugWriteFormat::getWriteFormat().writef("splitRegion failure for padGeometry '%s'\n",padGeometry.getName());
      }

      //splitStencilHole = new CSplitStencilHole(camCadDatabase,fileStruct,regionPolygons);

      int webWidthIndex = camCadDatabase.getDefinedWidthIndex(insetWebSize);
      int widthIndex    = camCadDatabase.getDefinedWidthIndex(exteriorCornerRadius * 2.);

      CString splitGeometryName;
      splitGeometryName.Format("%s_split_%d_%d_%d-ww%d-w%d",(const char*)padGeometry.getName(),
         regionPolygons.getXSize(),regionPolygons.getYSize(),iWebAngleDegrees,webWidthIndex,widthIndex);

      m_geometry = camCadDatabase.getBlock(splitGeometryName,fileStruct.getFileNumber());

      if (m_geometry == NULL)
      {
         m_geometry = camCadDatabase.getDefinedBlock(splitGeometryName,
                        blockTypeUnknown,fileStruct.getFileNumber());

         for (int xIndex = 0;xIndex < regionPolygons.getXSize();xIndex++)
         {
            for (int yIndex = 0;yIndex < regionPolygons.getYSize();yIndex++)
            {
               CRegionPolygon* regionPolygon = regionPolygons.getAt(xIndex,yIndex);

               if (regionPolygon != NULL)
               {
                  CPolyList* polyList = regionPolygon->getSmoothedPolys();

                  if (polyList != NULL)
                  {
                     polyList->transform(rotationMatrix);

                     for (POSITION pos = polyList->GetHeadPosition();pos != NULL;)
                     {
                        POSITION oldPos = pos;

                        CPoly* poly = polyList->GetNext(pos);
                        CExtent polyExtent = poly->getExtent();

                        if (min(polyExtent.getXsize(),polyExtent.getYsize()) < 2.*exteriorCornerRadius)
                        {
                           polyList->deleteAt(oldPos);
                           poly = NULL;
                        }
                     }

                     DataStruct* polyStruct = newDataStruct(dataTypePoly);
                     polyStruct->getPolyList()->takeData(*polyList);
                     polyStruct->getPolyList()->setWidthIndex(widthIndex);
                     polyStruct->setLayerIndex(camCadDatabase.getFloatingLayerIndex());

                     m_geometry->getDataList().AddTail(polyStruct);
                  }
               }
            }
         }
      }

      m_stencilHole =
         camCadDatabase.insertBlock(m_geometry,
            INSERTTYPE_STENCILHOLE,"",pad.getLayerIndex(),
            pad.getInsert()->getOrigin().x, pad.getInsert()->getOrigin().y,
            pad.getInsert()->getAngle(), pad.getInsert()->getGraphicMirrored(),
            pad.getInsert()->getScale());
   }

   delete aperturePolyList;
   //delete allocatedAperturePolyList;
}

CSplitStencilHole::~CSplitStencilHole()
{
   delete m_stencilHole;
}

CCamCadDatabase& CSplitStencilHole::getCamCadDatabase() const
{
   return m_camCadDatabase;
}

CCEtoODBDoc& CSplitStencilHole::getCamCadDoc() const
{
   return m_camCadDatabase.getCamCadDoc();
}

CCamCadData& CSplitStencilHole::getCamCadData() const
{
   return m_camCadDatabase.getCamCadData();
}

CSplitStencilHole* CSplitStencilHole::getSplitStencilHole(CCamCadDatabase& camCadDatabase,
      FileStruct& fileStruct,CCompositePad& compositePad,
      double maxFeatureSize,double webSize,double webAngleDegrees,StencilSplitTypeTag splitType,double exteriorCornerRadius)
{
   CSplitStencilHole* splitStencilHole = NULL;

   DataStruct* pad = compositePad.getPad();
   bool rectPadFlag = false;
   CExtent padExtent;
   BlockStruct* padGeometry;

   if (pad != NULL)
   {
      padGeometry = compositePad.getPadGeometry();

      if (padGeometry->isComplexAperture())
      {
         padGeometry = camCadDatabase.getBlock(padGeometry->getComplexApertureSubBlockNumber());
      }

      padExtent = padGeometry->getExtent();

      if (! padExtent.isValid())
      {
         camCadDatabase.getCamCadDoc().CalcBlockExtents(padGeometry);
         padExtent = padGeometry->getExtent();
      }

      if (padExtent.getXsize() >= maxFeatureSize ||
          padExtent.getYsize() >= maxFeatureSize     )
      {
         //for (CDataListIterator polyListIterator(*padGeometry,dataTypePoly);polyListIterator.hasNext();)
         //{
         //   DataStruct* polyStruct = polyListIterator.getNext();
         //   double maxExtent = max(padExtent.getXsize(),padExtent.getYsize());

         //   if (maxExtent <= 0.) maxExtent = 1.;

         //   double pixelsPerPageUnit = 400./maxExtent;
         //   double insetWebSize = webSize + exteriorCornerRadius*2.;
         //   int webSizePixels = round((ceil(insetWebSize * pixelsPerPageUnit)));

         //   if (webSizePixels < 1) webSizePixels = 1;

         //   pixelsPerPageUnit = webSizePixels / insetWebSize;

         //   int maxFeatureSizePixels = (int)(maxFeatureSize * pixelsPerPageUnit);

         //   CRegionPolygon regionPolygon(camCadDatabase.getCamCadDoc(),*(polyStruct->getPolyList()),pixelsPerPageUnit);

         //   CSplitRegionPolygons regionPolygons;
         //   regionPolygon.splitRegion(regionPolygons,maxFeatureSizePixels,webSizePixels);

            splitStencilHole = new CSplitStencilHole(camCadDatabase,fileStruct,*pad,
                                    *padGeometry,maxFeatureSize,webSize,webAngleDegrees,splitType,exteriorCornerRadius);

         //   break;
         //}
      }
   }

   return splitStencilHole;
}

double CSplitStencilHole::getArea() const
{
   double area = 0.;

   if (m_geometry != NULL)
   {
      int zeroWidthIndex = camCadDatabase_camCadDataF().getZeroWidthIndex();
      double tolerance = camCadDatabase_camCadDataF().convertToPageUnits(pageUnitsMils,.1);

      for (CDataListIterator polyIterator(*m_geometry,dataTypePoly);polyIterator.hasNext();)
      {
         DataStruct* polyStruct = polyIterator.getNext();
         CPolyList& polyList = *(polyStruct->getPolyList());

         for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList.GetNext(polyPos);

            int widthIndex = poly->getWidthIndex();
            double width = getCamCadDataDoc().getWidth(widthIndex);

            if (width != 0.)
            {
               CPoly outlinePoly(*poly);
               outlinePoly.closePolyWithCoincidentEndPoints();
               outlinePoly.convertToOutline(width,tolerance,zeroWidthIndex);

               area += outlinePoly.getArea();
            }
            else
            {
               area += poly->getArea();
            }
         }
      }
   }

   //return m_xSplits * m_ySplits * m_xSplitSize * m_ySplitSize;
   return area;
}

double CSplitStencilHole::getPerimeterLength() const
{
   //return m_xSplits * m_ySplits * 2. * (m_xSplitSize + m_ySplitSize);
   return 0.;
}

//_____________________________________________________________________________
int CStencilHole::m_nextId = 0;

CStencilHole::CStencilHole(CStencilSubPin& stencilSubPin,bool topFlag)
: m_stencilSubPin(stencilSubPin)
, m_topFlag(topFlag)
, m_componentGeometryVessel(NULL)
, m_padstackVessel(NULL)
, m_subPadstackVessel(NULL)
, m_subPinVessel(NULL)
, m_componentVessel(NULL)
, m_componentPinVessel(NULL)
, m_componentSubPinVessel(NULL), m_attributeSource(attributeSourceUndefined)
{
   m_id                       = m_nextId++;
   m_copperPad                = NULL;
   m_stencilHole              = NULL;
   m_stencilHoleInstance      = NULL;
   //m_stencilAttribute         = NULL;
   //m_stencilModifierAttribute = NULL;
   m_splitStencilHole         = NULL;
   //m_stencilKeyword           = -1;
   //m_stencilModifierKeyword   = -1;

   m_stencilThickness         = 0.;

   m_copperPadInitializedFlag = false;
   m_ruleStatus               = ruleStatusUndefined;
}

CStencilHole::~CStencilHole()
{
   delete m_copperPad;
   delete m_stencilHole;
   delete m_splitStencilHole;
}

CCamCadDatabase& CStencilHole::getCamCadDatabase() const
{
   return getStencilPin().getCamCadDatabase();
}

CCamCadData& CStencilHole::getCamCadData() const
{
   return getCamCadDatabase().getCamCadData();
}

CStencilGenerator& CStencilHole::getStencilGenerator() const
{
   return m_stencilSubPin.getStencilPin().getStencilGenerator();
}

CStencilPin& CStencilHole::getStencilPin() const
{
   return m_stencilSubPin.getStencilPin();
}

CStencilSubPin& CStencilHole::getStencilSubPin() const
{
   return m_stencilSubPin;
}

DataStruct* CStencilHole::getStencilHoleInstance() const
{
   return m_stencilHoleInstance;
}

void CStencilHole::setStencilHoleInstance(DataStruct* stencilHoleInstance)
{
   m_stencilHoleInstance    = stencilHoleInstance;
}

double CStencilHole::getStencilThickness() const
{
   return m_stencilThickness;
}

void CStencilHole::setStencilThickness(double stencilThickness)
{
   m_stencilThickness    = stencilThickness;
}

CString CStencilHole::getRuleViolations() const
{
   return m_ruleViolations;
}

void CStencilHole::setRuleViolations(const CString& ruleViolations)
{
   m_ruleViolations    = ruleViolations;
}

AttributeSourceTag CStencilHole::getAttributeSource() const
{
   return m_attributeSource;
}

void CStencilHole::setAttributeSource(AttributeSourceTag attributeSource)
{
   m_attributeSource    = attributeSource;
}

bool CStencilHole::getSplitFlag() const
{
   return (m_splitStencilHole != NULL);
}

CSplitStencilHole* CStencilHole::getSplitStencilHole() const
{
   return m_splitStencilHole;
}

//CString CStencilHole::getRefDelimiter()
//{
//   return CString(".");
//}

DataStruct& CStencilHole::getVesselInstance(BlockStruct& vesselGeometry,const CString& vesselReference) const
{
   DataStruct* vesselInstance = NULL;
   BlockStruct* componentGeometry = camCadDatabase_camCadDataF().getBlock(getStencilPin().getComponent().getInsert()->getBlockNumber());

   for (CDataListIterator vesselIterator(*componentGeometry,dataTypeInsert);vesselIterator.hasNext();)
   {
      DataStruct* vesselData = vesselIterator.getNext();

      if (vesselData->getInsert()->getBlockNumber() == vesselGeometry.getBlockNumber())
      {
         if (vesselData->getInsert()->getRefname().CompareNoCase(vesselReference) == 0)
         {
            vesselInstance = vesselData;
            break;
         }
      }
   }

   if (vesselInstance == NULL)
   {
      vesselInstance = getCamCadDatabase().insertBlock(&vesselGeometry,insertTypeUnknown,vesselReference,0);
      componentGeometry->getDataList().AddTail(vesselInstance);
   }

   return *vesselInstance;
}

DataStruct& CStencilHole::getPcbVessel() const
{
   DataStruct& pcbVessel = getStencilGenerator().getPcbVessel();

   return pcbVessel;
}

DataStruct& CStencilHole::getSurfaceVessel() const
{
   DataStruct& surfaceVessel = getStencilGenerator().getSurfaceVessel(m_topFlag);

   return surfaceVessel;
}

DataStruct& CStencilHole::getMountVessel() const
{
   DataStruct& mountVessel = getStencilGenerator().getMountVessel(m_topFlag,getStencilPin().isSmd(),getStencilPin().isVia(),getStencilPin().isFiducial());

   return mountVessel;
}

DataStruct& CStencilHole::getComponentGeometryVessel() const
{
   if (m_componentGeometryVessel == NULL)
   {
      CString reference = getSurfaceName() + getRefDelimiter() + getDesignatorString(getComponentName());

      m_componentGeometryVessel = &(getVesselInstance(getStencilGenerator().getComponentGeometryVesselGeometry(),reference));
   }

   return *m_componentGeometryVessel;
}

DataStruct& CStencilHole::getPadstackVessel() const
{
   if (m_padstackVessel == NULL)
   {
      CString reference = getSurfaceName() + getRefDelimiter() + getDesignatorString(getComponentName()) + 
                                             getRefDelimiter() + getDesignatorString(getFirstPinName());

      m_padstackVessel = &(getVesselInstance(getStencilGenerator().getPadstackVesselGeometry(),reference));
   }

   return *m_padstackVessel;
}

DataStruct& CStencilHole::getSubPadstackVessel() const
{
   if (m_subPadstackVessel == NULL)
   {
      CString reference = getSurfaceName()  + getRefDelimiter() + getDesignatorString(getComponentName()) + 
                                              getRefDelimiter() + getDesignatorString(getFirstPinName()) + 
                                              getRefDelimiter() + getDesignatorString(getSubPadstackName());

      m_subPadstackVessel = &(getVesselInstance(getStencilGenerator().getSubPadstackVesselGeometry(),reference));
   }

   return *m_subPadstackVessel;
}

DataStruct& CStencilHole::getSubPinVessel() const
{
   if (m_subPinVessel == NULL)
   {
      CString reference = getSurfaceName() + getRefDelimiter() + getDesignatorString(getComponentName()) + 
                                             getRefDelimiter() + getDesignatorString(getPinName()) + 
                                             getRefDelimiter() + getDesignatorString(getSubPadstackName());

      m_subPinVessel = &(getVesselInstance(getStencilGenerator().getSubPinVesselGeometry(),reference));
   }

   return *m_subPinVessel;
}

DataStruct& CStencilHole::getComponentVessel() const
{
   if (m_componentVessel == NULL)
   {
      CString reference = getSurfaceName() + getRefDelimiter() + getDesignatorString(getRefDes());

      m_componentVessel = &(getStencilGenerator().getClosetVessel(reference,getStencilGenerator().getComponentVesselGeometry()));
   }

   return *m_componentVessel;
}

DataStruct& CStencilHole::getComponentPinVessel() const
{
   if (m_componentPinVessel == NULL)
   {
      CString reference = getSurfaceName() + getRefDelimiter() + getDesignatorString(getRefDes()) + 
                                             getRefDelimiter() + getDesignatorString(getPinName());

      m_componentPinVessel = &(getStencilGenerator().getClosetVessel(reference,getStencilGenerator().getComponentPinVesselGeometry()));
   }

   return *m_componentPinVessel;
}

DataStruct& CStencilHole::getComponentSubPinVessel() const
{
   if (m_componentSubPinVessel == NULL)
   {
      CString reference = getSurfaceName() + getRefDelimiter() + getDesignatorString(getRefDes()) + 
                                             getRefDelimiter() + getDesignatorString(getPinName()) + 
                                             getRefDelimiter() + getDesignatorString(getSubPinName());

      m_componentSubPinVessel = &(getStencilGenerator().getClosetVessel(reference,getStencilGenerator().getComponentSubPinVesselGeometry()));
   }

   return *m_componentSubPinVessel;
}

CString CStencilHole::getSurfaceName() const
{
   return ::getSurfaceName(m_topFlag);
}

CString CStencilHole::getComponentName() const
{
   return getStencilSubPin().getGeometryName();
}

CString CStencilHole::getPadstackName() const
{
   return getStencilSubPin().getPadStackName();
}

CString CStencilHole::getSubPadstackName() const
{
   CString subPadstackName;

   subPadstackName.Format("%d",getStencilSubPin().getSubPinIndex());

   return subPadstackName;
}

CString CStencilHole::getPinName() const
{
   return getStencilPin().getPinName();
}

CString CStencilHole::getRefDes() const
{
   return getStencilPin().getRefDes();
}

CString CStencilHole::getSubPinName() const
{
   return getStencilSubPin().getSubPinName();
}

CString CStencilHole::getFirstPinName() const
{
   return getStencilPin().getFirstPinName();
}

DataStruct* CStencilHole::getPad(const CLayerFilter& layerFilter) const
{
   int subPinIndex = getStencilSubPin().getSubPinIndex();
   CPadStackInsert padStackInsert(getCamCadDatabase(),getStencilPin().getPadStack());
   DataStruct* topSubPad    = padStackInsert.getSubPad(subPinIndex,true );
   DataStruct* bottomSubPad = padStackInsert.getSubPad(subPinIndex,false);

   return getPad(layerFilter,topSubPad,bottomSubPad);
}

DataStruct* CStencilHole::getPad(const CLayerFilter& layerFilter,DataStruct* topSubPad,DataStruct* bottomSubPad) const
{
   DataStruct* foundPad = NULL;

   bool mirrorLayerFlag = getStencilPin().getComponent().getInsert()->getLayerMirrored();

   //  topStencil   componentTop   surfaceType
   //   f                f          top
   //   f                t          bottom
   //   t                f          bottom
   //   t                t          top

   mirrorLayerFlag = (mirrorLayerFlag != getStencilPin().getPadStack().getInsert()->getLayerMirrored());

   CCompositePad surfacePad(getCamCadDatabase());
   BlockStruct* padStackGeometry = getCamCadDatabase().getBlock(getStencilPin().getPadStack().getInsert()->getBlockNumber());

   //int subPinIndex = getStencilSubPin().getSubPinIndex();
   //CPadStackInsert padStackInsert(getCamCadDatabase(),getStencilPin().getPadStack());
   //DataStruct* topSubPad    = padStackInsert.getSubPad(subPinIndex,true );
   //DataStruct* bottomSubPad = padStackInsert.getSubPad(subPinIndex,false);

   CDataList subPadList(false);

   if (topSubPad != NULL)
   {
      subPadList.AddTail(topSubPad);
   }

   if (bottomSubPad != NULL)
   {
      subPadList.AddTail(bottomSubPad);
   }

   CDataList padList(false);

   for (POSITION padPos = subPadList.GetHeadPosition();padPos != NULL;)
   {
      DataStruct* pad = subPadList.GetNext(padPos);
      InsertStruct* padInsert = pad->getInsert();
      BlockStruct* padGeometry = getCamCadDatabase().getBlock(padInsert->getBlockNumber());

      if (! padGeometry->isAperture())
      {
         continue;
      }

      if (padInsert->getInsertType() != insertTypeUnknown)
      {
         continue;
      }

      LayerStruct* layer = getCamCadDatabase().getLayerAt(pad->getLayerIndex());

      if (mirrorLayerFlag)
      {
         layer = getCamCadDatabase().getLayerAt(layer->getMirroredLayerIndex());
      }

      bool notInstantiatedFlag = (
         (layer->getNeverMirror() &&  mirrorLayerFlag) ||
         (layer->getMirrorOnly()  && !mirrorLayerFlag)     );

      if (notInstantiatedFlag || !layerFilter.contains(layer->getLayerIndex()))
      {
         continue;
      }

      bool instantiatedOnSurfaceLayerFlag = false;
      LayerTypeTag layerType   = layer->getLayerType();
      LayerGroupTag layerGroup = getLayerGroupForLayerType(layerType);

      switch (layerGroup)
      {
      case layerGroupTop:
         instantiatedOnSurfaceLayerFlag =  m_topFlag;
         break;

      case layerGroupBottom:
         instantiatedOnSurfaceLayerFlag = !m_topFlag;
         break;

      case layerGroupAll:
      case layerGroupOuter:
         instantiatedOnSurfaceLayerFlag = true;
         break;
      }

      if (!instantiatedOnSurfaceLayerFlag)
      {
         continue;
      }

      padList.AddTail(pad);
      //surfacePad.addPad(newDataStruct(*pad));
   }

   if (padList.GetCount() > 0)
   {
      foundPad = newDataStruct(*(surfacePad.getPad(*padStackGeometry,padList)));

      CTMatrix matrix;

      InsertStruct* componentInsert = getStencilPin().getComponent().getInsert();
      matrix.translateCtm(componentInsert->getOriginX(),componentInsert->getOriginY());
      matrix.rotateRadiansCtm(componentInsert->getAngle());
      matrix.scaleCtm(((componentInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1 : 1,1);

      InsertStruct* padstackInsert = getStencilPin().getPadStack().getInsert();
      matrix.translateCtm(padstackInsert->getOriginX(),padstackInsert->getOriginY());
      matrix.rotateRadiansCtm(padstackInsert->getAngle());
      matrix.scaleCtm(((padstackInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1 : 1,1);

      foundPad->transform(matrix);

      CPoint2d& origin = getStencilPin().getOrigin();

      origin.x = 0.;
      origin.y = 0.;

      matrix.transform(origin);
   }

   return foundPad;
}

DataStruct* CStencilHole::getCopperPad() const
{
   if (m_copperPad == NULL && !m_copperPadInitializedFlag)
   {
      CLayerFilter layerFilter(false);
      CCEtoODBDoc& camCadDoc = getCamCadDatabase().getCamCadDoc();
      int maxLayerIndex = camCadDoc.getMaxLayerIndex();

      for (int layerIndex = 0;layerIndex < maxLayerIndex;layerIndex++)
      {
         LayerStruct* layer = camCadDoc.getLayerAt(layerIndex);

         if (layer != NULL)
         {
            switch (layer->getLayerType())
            {
            case layerTypeSignalTop:
            case layerTypePadTop:
            case layerTypeTop:
            case layerTypeSignalBottom:
            case layerTypePadBottom:
            case layerTypeBottom:
            case layerTypeSignalAll:
            case layerTypePadAll:
            case layerTypeAll:
            case layerTypePadOuter:
            case layerTypeSignalOuter:
               layerFilter.add(layer->getLayerIndex());
               break;
            }
         }
      }

      CString debug = layerFilter.getContentsString();
      m_copperPad = getPad(layerFilter);

      m_copperPadInitializedFlag = true;
   }

   return m_copperPad;
}

DataStruct* CStencilHole::getStencilSourcePad() const
{
   DataStruct* stencilSourcePad = NULL;

   CCamCadDatabase& camCadDatabase = getStencilPin().getCamCadDatabase();
   CStencilGenerationParameters& stencilGenerationParameters = getStencilPin().getStencilGenerator().getStencilGenerationParameters();
   LayerStruct* sourceLayer = NULL;

   CString topStencilSourceLayerName    = stencilGenerationParameters.getTopStencilSourceLayerName();
   CString bottomStencilSourceLayerName = stencilGenerationParameters.getBottomStencilSourceLayerName();

   CString stencilSourceLayerName = (m_topFlag ? topStencilSourceLayerName : bottomStencilSourceLayerName);

   if (!stencilSourceLayerName.IsEmpty())
   {
      sourceLayer = camCadDatabase.getLayer(stencilSourceLayerName);
   }

   if (sourceLayer != NULL)
   {
      CLayerFilter layerFilter(false);
      layerFilter.add(sourceLayer->getLayerIndex());

      int subPinIndex = getStencilSubPin().getSubPinIndex();
      CPadStackInsert padStackInsert(getCamCadDatabase(),getStencilPin().getPadStack(),topStencilSourceLayerName,bottomStencilSourceLayerName);

      DataStruct* topSubPad    = padStackInsert.getStencilSourceSubPad(subPinIndex,true );
      DataStruct* bottomSubPad = padStackInsert.getStencilSourceSubPad(subPinIndex,false);

      stencilSourcePad = getPad(layerFilter,topSubPad,bottomSubPad);


      // RE dts0100546544  Missing pads on D200 in Stencil Generator with this design  
      // Complex apertures should use floating layer index for everything, so that
      // it all appears on what ever layer the aperture is inserted on.
      // Convert items on stencilSourceLayer to floating layer.
      if (stencilSourcePad != NULL && stencilSourcePad->getDataType() == dataTypeInsert)
      {
         BlockStruct *blk = camCadDatabase.getBlock( stencilSourcePad->getInsert()->getBlockNumber() );
         if (blk != NULL && blk->isComplexAperture())
         {
            BlockStruct *cmplxGeomBlk = camCadDatabase.getBlock( blk->getComplexApertureSubBlockNumber() );
            if (cmplxGeomBlk != NULL)
            {
               POSITION cmplxDataPos = cmplxGeomBlk->getHeadDataPosition();
               while (cmplxDataPos != NULL)
               {
                  DataStruct *cmplxData = cmplxGeomBlk->getNextData(cmplxDataPos);
                  if (cmplxData->getLayerIndex() == sourceLayer->getLayerIndex())
                     cmplxData->setLayerIndex( camCadDatabase.getFloatingLayerIndex());
               }
            }
         }
      }

   }

   return stencilSourcePad;
}

CPoly* CStencilHole::getStencilPoly()
{
   DataStruct* stencilPad = getStencilHoleInstance();
   CPoly* stencilPoly = getStencilPin().getPadPoly(stencilPad);

   return stencilPoly;
}

CPoly* CStencilHole::getCopperPoly()
{
   DataStruct* copperPad = getCopperPad();
   CPoly* copperPoly = getStencilPin().getPadPoly(copperPad);

   return copperPoly;
}

CPoly* CStencilHole::getSplitStencilParentPoly()
{
   CPoly* splitStencilParentPoly = NULL;

   if (m_splitStencilHole != NULL && m_stencilHole != NULL)
   {
      splitStencilParentPoly = getStencilPin().getPadPoly(m_stencilHole);
   }

   return splitStencilParentPoly;
}

CString CStencilHole::getCopperApertureDescriptor() const
{
   return getStencilPin().getPadDescriptor(getCopperPad());
}

CString CStencilHole::getStencilApertureDescriptor() const
{
   return getStencilPin().getPadDescriptor(getStencilPad());
}

CString CStencilHole::getDerivationMethod() const
{
   CString derivationMethod;

   if (m_stencilRule.isValid())
   {
      derivationMethod = (attributeSourceToString(m_attributeSource)) + ":" + m_stencilRule.getRuleString();
   }

   return derivationMethod;
}

CString CStencilHole::getDerivationMethodForSourceLevel(AttributeSourceTag attributeSourceLevel) const
{
   CString derivationMethod;

   CStencilRule stencilRule;
   AttributeSourceTag attributeSource = getStencilRuleForSourceLevel(attributeSourceLevel,stencilRule);

   if (stencilRule.isValid())
   {
      derivationMethod = (attributeSourceToString(attributeSource)) + ":" + stencilRule.getRuleString();
   }

   return derivationMethod;
}

CStencilRule CStencilHole::getStencilRuleForSource(AttributeSourceTag attributeSource)
{
   CStencilRuleAttributes stencilRuleAttributes = getStencilRuleAttributes(attributeSource);

   return stencilRuleAttributes.getRule();
}

CStencilRule CStencilHole::getStencilRule()
{
   CStencilRuleAttributes stencilRuleAttributes = getStencilRuleAttributes(m_attributeSource);

   return stencilRuleAttributes.getRule();
}

double CStencilHole::getPasteVolume() const
{
   double pasteVolume = 0.;

   DataStruct* stencilPad = getStencilPad();

   if (stencilPad != NULL)
   {
      PageUnitsTag pageUnits = intToPageUnitsTag(getCamCadDatabase().getCamCadDoc().getSettings().getPageUnits());
      double volumeFactor    = getStencilPin().getPageUnitsToDisplayUnitsFactor();
    //double defaultThickness = 8. * getUnitsFactor(pageUnitsMils,pageUnits);
      double padArea = 0.;

      volumeFactor = volumeFactor * volumeFactor * volumeFactor;

      BlockStruct* padBlock  = getCamCadDatabase().getBlock(stencilPad->getInsert()->getBlockNumber());

      if (getSplitStencilHole() != NULL)
      {
         padArea = getSplitStencilHole()->getArea();
      }
      else
      {
         CStandardAperture standardAperture(padBlock,pageUnits);

         if (standardAperture.isStandardAperture())
         {
            padArea = standardAperture.getArea(camCadDatabase_camCadDataF());
         }
      }

      pasteVolume = getStencilThickness() * padArea * volumeFactor;
   }

   return pasteVolume;
}

DataStruct* CStencilHole::calcStencilHole(CStencilGroups& stencilGroups,FileStruct* fileStruct,double maxFeatureSizeSetting,double webSizeSetting)
{
   DataStruct* stencilHole = NULL;
   CString pinref = getStencilPin().getPinRef();

   bool compensateRotation = true;

   try
   {
      bool enableSplitPad = true;

      if (m_stencilHole == NULL /*&& !hasDrillTool()*/)
      {
         CStencilGenerationParameters& stencilGenerationParameters = getStencilPin().getStencilGenerator().getStencilGenerationParameters();

         DataStruct* copperPad = getCopperPad();

         if (copperPad != NULL)
         {
            updateStencilHoleAttribute();
            setRuleStatus(ruleStatusOk);

            CCompositePad compositePad(getCamCadDatabase());
            compositePad.addPad(newDataStruct(*copperPad));

            // Debug
            CTMatrix copperPadMatrix = compositePad.getPad()->getInsert()->getTMatrix();
            //getStencilPin().getCamCadDatabase().drawAxes(copperPadMatrix,"calcStencilHole() - copperPad");

            CTMatrix pinMatrix = getStencilPin().getPadStack().getInsert()->getTMatrix() * getStencilPin().getComponent().getInsert()->getTMatrix();
            //getStencilPin().getCamCadDatabase().drawAxes(pinMatrix,"calcStencilHole() - pin");

            bool enableDerivedPadFlag = true;
            bool substitutedPadFlag   = false;
            double symmetryAngleOffsetDegrees = 0.;
            StencilOrientationModifierTag symmetryAngleOffsetOrientation = orientationNormal;

            PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();
            StencilRuleModifierTag modifierTag     = m_stencilRule.getModifier();
            CStencilRuleModifierValues modifierParameters;
            m_stencilRule.getModifierValues(modifierParameters);

            // exterior corner radius modifier  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            //double exteriorCornerRadius = exteriorCornerRadiusSetting;
            double exteriorCornerRadius = stencilGenerationParameters.getExteriorCornerRadius();

            CStencilRuleModifier* exteriorCornerRadiusStencilRuleModifier = m_stencilRule.getModifier(stencilRuleModifierExteriorCornerRadius);

            if (exteriorCornerRadiusStencilRuleModifier != NULL)
            {
               exteriorCornerRadius = exteriorCornerRadiusStencilRuleModifier->getUnitValueAt(0,pageUnits,0.);
            }

            // flip symmetry axis modifier  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            CStencilRuleModifier* flipSymmetryAxisStencilRuleModifier = m_stencilRule.getModifier(stencilRuleModifierFlipSymmetryAxis);
            bool flipSymmetryAxisFlag = (flipSymmetryAxisStencilRuleModifier != NULL);

            if (! m_stencilRule.isValid())
            {
               setAttributeSource(attributeSourceNone);
               enableDerivedPadFlag = false;
            }
            else if (m_stencilRule.getRule() == stencilRuleUndefined ||
                     m_stencilRule.getRule() == stencilRuleNone          )
            {
               enableDerivedPadFlag = false;
            }
            else if (m_stencilRule.getRule() == stencilRuleCopperSourceLayer)
            {
               enableSplitPad = false;
            }
            else if (m_stencilRule.getRule() == stencilRuleAperture)
            {
               enableDerivedPadFlag = false;

               CString apertureDescriptor  = m_stencilRule.getRuleValue();
               CStandardAperture standardAperture(apertureDescriptor,getCamCadDatabase().getPageUnits());
               standardAperture.setExteriorCornerRadius(exteriorCornerRadius);

               BlockStruct* apertureGeometry = standardAperture.getDefinedAperture(camCadDatabase_camCadDataF());

               if (apertureGeometry != NULL)
               {
                  substitutedPadFlag = true;

                  m_stencilHole = newDataStruct(dataTypeInsert);
                  m_stencilHole->getInsert()->setBlockNumber(apertureGeometry->getBlockNumber());
                  m_stencilHole->getInsert()->setInsertType(insertTypeStencilHole);

                  DataStruct* subPad = getStencilSubPin().getSubPad(m_topFlag);

                  if (subPad != NULL)
                  {
                     m_stencilHole->getInsert()->setOrigin(subPad->getInsert()->getOrigin());
                  }

                  // symmetry orientation modifier  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
                  CStencilRuleModifier* stencilRuleModifier = m_stencilRule.getModifier(stencilRuleModifierOrientSymmetryAxis);

                  if (stencilRuleModifier != NULL)
                  {
                     symmetryAngleOffsetOrientation = stringToStencilOrientationModifierTag(stencilRuleModifier->getValueAt(0));

                     if (symmetryAngleOffsetOrientation == orientationTowardCenter   ||
                         symmetryAngleOffsetOrientation == orientationAwayFromCenter    )
                     {
                        double symmetryAngleOffsetRadians = 0.;
                        if (calcRotationModifierRadians( symmetryAngleOffsetRadians, standardAperture, symmetryAngleOffsetOrientation, flipSymmetryAxisFlag ))
                        {
                           symmetryAngleOffsetDegrees = radiansToDegrees(symmetryAngleOffsetRadians);
                        }
                     }
                  }
                  else
                  {
                     compensateRotation = false;  // bottom side rotation is already what it needs to be
                  }
               }
            }
            else if (m_stencilRule.getRule() == stencilRuleApertureGeometry)
            {
               enableDerivedPadFlag = false;

               CString apertureName  = m_stencilRule.getRuleValue();
               BlockStruct* apertureGeometry = getCamCadDatabase().getBlock(apertureName);

               if (apertureGeometry != NULL && apertureGeometry->isAperture())
               {
                  substitutedPadFlag = true;

                  m_stencilHole = newDataStruct(dataTypeInsert);
                  m_stencilHole->getInsert()->setBlockNumber(apertureGeometry->getBlockNumber());
                  m_stencilHole->getInsert()->setInsertType(insertTypeStencilHole);

                  DataStruct* subPad = getStencilSubPin().getSubPad(m_topFlag);

                  if (subPad != NULL)
                  {
                     m_stencilHole->getInsert()->setBasesVector(subPad->getInsert()->getBasesVector());
                  }
               }
            }
            else if (m_stencilRule.getRule() == stencilRuleInset)
            {
               CString insetString  = m_stencilRule.getRuleValue();
               double inset = atof(insetString);

               if (!compositePad.insetPadPerimeter(inset,exteriorCornerRadius))
               {
                  setRuleStatus(ruleStatusInsetError);
               }
            }
            else if (m_stencilRule.getRule() == stencilRuleArea)
            {
               CString areaString  = m_stencilRule.getRuleValue();
               double areaScale    = atof(areaString);

               compositePad.scalePadArea(areaScale,exteriorCornerRadius);
            }
            else if (m_stencilRule.getRule() == stencilRuleStencilSourceLayer)
            {
               compositePad.empty();
               compositePad.addPad( getStencilSourcePad() );
               enableSplitPad = false;
            }
            else if (m_stencilRule.getRule() == stencilRulePassthru)
            {
               compositePad.empty();
               enableSplitPad = false;
            }

            // web settings modifier  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            double maxFeatureSize  = maxFeatureSizeSetting;
            double webSize         = webSizeSetting;
            double webAngleDegrees = 0.;
            StencilSplitTypeTag splitType = splitTypeGrid;

            CStencilRuleModifier* webSettingsStencilRuleModifier = m_stencilRule.getModifier(stencilRuleModifierWebSettings);

            if (webSettingsStencilRuleModifier != NULL)
            {
               maxFeatureSize  = webSettingsStencilRuleModifier->getUnitValueAt(0,pageUnits,0.);
               webSize         = webSettingsStencilRuleModifier->getUnitValueAt(1,pageUnits,0.);
               webAngleDegrees = atof(webSettingsStencilRuleModifier->getValueAt(2));
               splitType       = stringToSplitTypeTag(webSettingsStencilRuleModifier->getValueAt(3));
            }

            // offset modifier  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            CStencilRuleModifier* offsetStencilRuleModifier = m_stencilRule.getModifier(stencilRuleModifierOffsetRelativeToPadstack);
            StencilOrientationModifierTag offsetOrientation = orientationNormal;

            double offsetX            = 0.;
            double offsetY            = 0.;
            double offsetAngleDegrees = symmetryAngleOffsetDegrees;

            if (offsetStencilRuleModifier != NULL &&
                ruleAllowsModifier(m_stencilRule.getRule(),stencilRuleModifierOffsetRelativeToPadstack))
            {
               offsetX            = offsetStencilRuleModifier->getUnitValueAt(0,pageUnits,0.);
               offsetY            = offsetStencilRuleModifier->getUnitValueAt(1,pageUnits,0.);
               offsetAngleDegrees = atof(offsetStencilRuleModifier->getValueAt(2)) + symmetryAngleOffsetDegrees;
               offsetOrientation  = stringToStencilOrientationModifierTag(offsetStencilRuleModifier->getValueAt(3));
            }

            if (substitutedPadFlag)
            {
               CString pinref = getStencilPin().getPinRef();

               InsertStruct* componentInsert = getStencilPin().getComponent().getInsert();

               CTMatrix componentMatrix = getStencilPin().getComponent().getInsert()->getTMatrix();
               CTMatrix padstackMatrix  = getStencilPin().getPadStack().getInsert()->getTMatrix();
               CTMatrix offsetMatrix;
               calcOffsetModifierTMatrix(offsetMatrix,CPoint2d(offsetX,offsetY),degreesToRadians(offsetAngleDegrees),
                  offsetOrientation,flipSymmetryAxisFlag);

               // use premultiplication
               CTMatrix matrix = offsetMatrix * padstackMatrix * componentMatrix;

               m_stencilHole->transform(matrix);


               // This has clearly been a problem area, as evidenced by the number of tries at an "if" clause
               // and the number of tries as some action. I am guessing this is because different stencil rules
               // require different treatment, there is no one-size-fits-all.
               // That the code is in place at all suggests there are times when this check and action are needed.

               if (m_stencilHole->getInsert()->getGraphicMirrored())
               //if (m_stencilRule.getRule() == stencilRuleAperture && m_stencilHole->getInsert()->getGraphicMirrored())
               //if (m_stencilRule.getRule() == stencilRuleAperture && pinMatrix.getMirror())
               {
                  // dts0100582886 - If using symmetry angle orientations then stencil holes are already set 
                  // up with mirroring rotation about the symmetry axis, so don't mess any further with
                  // rotation for those. For the rest, reverse the positive rotation direction as usual.
                  if (symmetryAngleOffsetOrientation != orientationTowardCenter &&
                     symmetryAngleOffsetOrientation != orientationAwayFromCenter)
                  {
                     //m_stencilHole->getInsert()->setGraphicsMirrorFlag(false);
                    m_stencilHole->getInsert()->setAngleRadians(-m_stencilHole->getInsert()->getAngleRadians());
                  }
               }

               compositePad.empty();
               DataStruct* newPad = newDataStruct(*m_stencilHole);
               newPad->setLayerIndex(getCamCadDatabase().getFloatingLayerIndex());
               compositePad.addPad(newPad);

               if (enableSplitPad)
               {
                  m_splitStencilHole =
                     CSplitStencilHole::getSplitStencilHole(getCamCadDatabase(),*fileStruct,
                        compositePad,maxFeatureSize,webSize,webAngleDegrees,splitType,exteriorCornerRadius);
               }
            }
            else if (enableDerivedPadFlag)
            {
               DataStruct* pad = compositePad.getPad();

               BlockStruct* padGeometry = compositePad.getPadGeometry();

               if (pad != NULL)
               {
                  if (pad->getInsert()->getGraphicMirrored())
                  {
                     // dts0100559721
                     // The problem in DR was finally determined to be inconsistency between split and non-split
                     // stencil pad processing. The split code apparantly has a different notion of the base data
                     // being prepared here, than does other code. Applying this rotation seems to make the 
                     // behavior consistent for split and non-split. After processing the rotation is re-applied, see
                     // other location with comment on dts0100559721.
                     CBasesVector padbv = pad->getInsert()->getBasesVector();
                     padbv.mirrorRotation();

                     m_stencilHole = getCamCadDatabase().insertBlock(padGeometry,insertTypeStencilHole,"",pad->getLayerIndex());
                     //*rcf Original m_stencilHole->getInsert()->setBasesVector(pad->getInsert()->getBasesVector());
                     m_stencilHole->getInsert()->setBasesVector(padbv);
                  }
                  else
                  {
                     m_stencilHole = newDataStruct(*pad);
                     m_stencilHole->getInsert()->setInsertType(insertTypeStencilHole);
                  }
               }

               // need to implement offset modifier transformation here
               //m_stencilHole->transform(offsetModifierMatrix);

               if (enableSplitPad)
               {
                  m_splitStencilHole =
                     CSplitStencilHole::getSplitStencilHole(getCamCadDatabase(),*fileStruct,
                        compositePad,maxFeatureSize,webSize,webAngleDegrees,splitType,exteriorCornerRadius);

                  //if (pad->getInsert()->getGraphicMirrored())
                  //{
                     //CBasesVector basesVector = m_splitStencilHole->getStencilHole()->getInsert()->getBasesVector();
                     //basesVector.set(basesVector.getOrigin().x,basesVector.getOrigin().y,basesVector.getRotationDegrees(),basesVector.getMirror());
                     //m_splitStencilHole->getStencilHole()->getInsert()->setBasesVector(basesVector);
                     //int jj = 0;//*rcf
                  //}
               }
            }
         }
         //else 
         //{
         //   int jj = 0;  //*rcf copperPad == NULL
         //}
      }
      //else
      //{
      //   int jj = 0; //*rcf m_stencilHole is not NULL (already has stencil?)
      //}

      stencilHole = m_stencilHole;


      if (m_splitStencilHole != NULL && enableSplitPad)
      {
         stencilHole = m_splitStencilHole->getStencilHole();
      }


      // Regardless of pcb side a stencil is "on", a stencil hole insert should be flagged as "Top" and
      // should NOT have Layer Mirror set. The actual graphics of the stencil are on the specific Stencil Top
      // or Stencil Bottom side as appropriate, i.e. bottom stencils are "bottom defined".
      // If it is a bottom stencil it will have Mirror Graphic set.\
      // Because the insert settings are initially based on the pad getting the stencil, it may have
      // inapropriate side and mirror settings inherited from the pad. Because there is not supposed
      // to be a difference between top and bottom stencils in these fields, we can just
      // force them to be what we want without bothering to check any details on the stencil.
      // Just amke sure the Mirror Graphic bit is not lost.

      if (stencilHole != NULL)
      {
         // dts0100559721
         // Reset the rotation for mirrored stencil pads, this is the equal and opposite match to
         // setting made to base data to get consistent results from split/non-split pads. See
         // other location with comment on dts0100559721.
         if (compensateRotation && (stencilHole->getInsert()->getMirrorFlags() & MIRROR_FLIP))
            stencilHole->getInsert()->setAngle( -stencilHole->getInsert()->getAngle() );

         // Keep mirror flip, discard mirror layer
         stencilHole->getInsert()->setMirrorFlags( stencilHole->getInsert()->getMirrorFlags() & MIRROR_FLIP );
         // Always inserted on top, even if bottom stencil
         stencilHole->getInsert()->setPlacedBottom(false);
      }

   }
   catch (...) ///CException* e)
   {
      ///CString errorMessage;
      ///e->GetErrorMessage(errorMessage.GetBufferSetLength(128),128);
      ///int jj = 0;
   }

   return stencilHole;
}

bool CStencilHole::calcRotationModifierRadians(double& padGeometryRotationRadians,CStandardAperture& standardAperture,
   StencilOrientationModifierTag stencilOrientation,bool flipSymmetryAxis)
{
   bool retval = false;

   padGeometryRotationRadians = 0.;

   if (stencilOrientation == orientationTowardCenter   ||
       stencilOrientation == orientationAwayFromCenter    )
   {
      DataStruct& componentData      = getStencilPin().getComponent();
      BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentData.getInsert()->getBlockNumber());

      // get component's major/minor axes, make sure that the bases vector is unmirrored
      double rotationOffsetDegrees = (flipSymmetryAxis ? 90. : 0.);
      CPoint2d pinCentroid = componentGeometry->getPinCentroid();
      CBasesVector componentMajorMinorAxes(pinCentroid.x,pinCentroid.y,1.,
                      componentGeometry->getPinMajorMinorAxes().getRotationDegrees() + rotationOffsetDegrees,false);

      // get standard aperture's symmetry axes
      CBasesVector standardApertureBasesVector = standardAperture.getSymmetryAxes();

      // transform standard aperture's symmetry axes by the pin's position
      DataStruct& padstackData      = getStencilPin().getPadStack();
      CTMatrix matrix(padstackData.getInsert()->getTMatrix());
      standardApertureBasesVector.transform(matrix);

      CPoint2d pinOrigin(padstackData.getInsert()->getOrigin2d());
      CPoint2d componentOrigin(componentMajorMinorAxes.getOrigin());
      CVector3d pinVector(pinOrigin - componentOrigin);
      CVector3d componentMajorAxis(componentMajorMinorAxes.getXaxis());
      CVector3d normalVector = pinVector.crossProduct(componentMajorAxis);
      CVector3d towardCenterLineVector3d = normalVector.crossProduct(componentMajorAxis);
      CVector2d towardCenterLineVector2d(towardCenterLineVector3d.getComponentX(),towardCenterLineVector3d.getComponentY());
      CVector2d apertureMajorAxes(standardApertureBasesVector.getXaxis());

      double towardCenterLineVectorTheta = towardCenterLineVector2d.getTheta();
      double apertureMajorAxisTheta      = standardApertureBasesVector.getXaxis().getTheta();
      double rotationRadians = 0.;

      switch (stencilOrientation)
      {
      case orientationTowardCenter:
         rotationRadians = towardCenterLineVectorTheta - apertureMajorAxisTheta;

         break;
      case orientationAwayFromCenter:
         rotationRadians = towardCenterLineVectorTheta - apertureMajorAxisTheta + Pi;

         break;
      }

      padGeometryRotationRadians = normalizeRadians(rotationRadians);

      retval = true;
   }

   return retval;
}

bool CStencilHole::calcOffsetModifierTMatrix(CTMatrix& offsetMatrix,
   CPoint2d offset,double angleOffsetRadians,
   StencilOrientationModifierTag offsetOrientation,bool flipSymmetryAxis)
{
   bool retval = true;

   offsetMatrix.initMatrix();

   if (offsetOrientation == orientationTowardCenter   ||
       offsetOrientation == orientationAwayFromCenter    )
   {
      DataStruct& componentData      = getStencilPin().getComponent();
      BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentData.getInsert()->getBlockNumber());

      // get component's major/minor axes, make sure that the bases vector is unmirrored
      double majorAxisRotationDegrees = componentGeometry->getPinMajorMinorAxes().getRotationDegrees();
      majorAxisRotationDegrees = normalizeDegrees(majorAxisRotationDegrees + (flipSymmetryAxis ? 90. : 0.));

      CPoint2d componentPinCentroid = componentGeometry->getPinCentroid();
      CBasesVector componentMajorMinorAxes(componentPinCentroid.x,componentPinCentroid.y,1.,majorAxisRotationDegrees,false);
      CPoint2d componentOrigin(componentMajorMinorAxes.getOrigin());

      DataStruct& padstackData      = getStencilPin().getPadStack();
      CPoint2d pinOrigin(padstackData.getInsert()->getOrigin2d());

      // horizontal axis
      if (fpeq(majorAxisRotationDegrees,0.) || fpeq(majorAxisRotationDegrees,180.))
      {
         if (fpeq(pinOrigin.y,componentOrigin.y))
         {  // pin on axis
            offset.y = 0.;
         }
         else if (pinOrigin.y > componentOrigin.y)
         {  // pin above axis
            offset.y *= -1.;
         }

         if (offsetOrientation == orientationAwayFromCenter)
         {
            offset.y *= -1.;
         }
      }

      // vertical axis
      if (fpeq(majorAxisRotationDegrees,90.) || fpeq(majorAxisRotationDegrees,270.))
      {
         if (fpeq(pinOrigin.x,componentOrigin.x))
         {  // pin on axis
            offset.x = 0.;
         }
         else if (pinOrigin.x > componentOrigin.x)
         {  // pin to right of axis
            offset.x *= -1;
         }

         if (offsetOrientation == orientationAwayFromCenter)
         {
            offset.x *= -1.;
         }
      }

      //CBasesVector componentMajorMinorAxes;
      //double rotationOffsetDegrees = (flipSymmetryAxis ? 90. : 0.);
      //componentMajorMinorAxes.initRotationMirror(componentGeometry->getPinMajorMinorAxes().getRotationDegrees() + rotationOffsetDegrees,false);

      //DataStruct& padstackData      = getStencilPin().getPadStack();

      //CPoint2d pinOrigin(padstackData.getInsert()->getOrigin2d());
      //CPoint2d componentOrigin(componentMajorMinorAxes.getOrigin());
      //CVector3d pinVector(pinOrigin - componentOrigin);
      //CVector3d componentMajorAxis(componentMajorMinorAxes.getXaxis());
      //CVector3d normalVector = pinVector.crossProduct(componentMajorAxis);
      //CVector3d towardCenterLineVector3d = normalVector.crossProduct(componentMajorAxis);
      //CVector2d towardCenterLineVector2d(towardCenterLineVector3d.getComponentX(),towardCenterLineVector3d.getComponentY());
      ////CVector2d apertureMajorAxes(pinBasesVector.getXaxis());

      //double towardCenterLineVectorTheta = towardCenterLineVector2d.getTheta();

      //offsetMatrix.rotateRadiansCtm(towardCenterLineVectorTheta);
      //offsetMatrix.scaleCtm((offsetOrientation == orientationTowardCenter) ? 1. : -1.);
      //offsetMatrix.translateCtm(offset);
   }
   //else
   //{
   //   offsetMatrix.translateCtm(offset);
   //   offsetMatrix.rotateRadiansCtm(angleOffsetRadians);
   //}

   offsetMatrix.translateCtm(offset);
   offsetMatrix.rotateRadiansCtm(angleOffsetRadians);

   return retval;
}

void CStencilHole::addAttributes(DataStruct* stencilAperture)
{
   CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*(stencilAperture->getDefinedAttributes()));
   stencilRuleAttributes.setRule(m_stencilRule);
}

AttributeSourceTag CStencilHole::getStencilRuleForSourceLevel(AttributeSourceTag attributeSourceLevel,CStencilRule& stencilRule) const
{
   AttributeSourceTag attributeSource = attributeSourceNone;

   switch (attributeSourceLevel)
   {
   default:
   case attributeSourceComponentSubPin:
      // subPin instance ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*(getComponentSubPinVessel().getDefinedAttributes()));

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceComponentSubPin;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceComponentPin:
      // pin instance ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*(getComponentPinVessel().getDefinedAttributes()));

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceComponentPin;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceComponent:
      // component ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*(getComponentVessel().getDefinedAttributes()));

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceComponent;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceGeometryPin:
      // geometry pin ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getSubPinVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceGeometryPin;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceSubPadstack:
      // subPadstack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getSubPadstackVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceSubPadstack;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourcePadstack:
      // padstack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getPadstackVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourcePadstack;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceGeometry:
      {
         // geometry ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getComponentGeometryVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceGeometry;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceMount:
      {
         // mount type ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getMountVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceMount;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourceSurface:
      {
         // surface ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getSurfaceVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourceSurface;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      // fall thru intentional
   case attributeSourcePcb:
      // pcb ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      {
         CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),getPcbVessel().attributes());

         if (stencilRuleAttributes.hasNonInheritedRule())
         {
            attributeSource = attributeSourcePcb;
            stencilRule = stencilRuleAttributes.getRule();
            break;
         }
      }

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      attributeSource = attributeSourceNone;
      stencilRule.setRule(stencilRuleUndefined);

      break;
   }

   return attributeSource;
}

bool CStencilHole::updateStencilHoleAttribute()
{
   m_attributeSource = getStencilRuleForSourceLevel(attributeSourceComponentSubPin,m_stencilRule);

   return (m_attributeSource != attributeSourceNone);
}

CStencilRuleAttributes CStencilHole::getStencilRuleAttributes()
{
   updateStencilHoleAttribute();

   return getStencilRuleAttributes(m_attributeSource);
}

CStencilRuleAttributes CStencilHole::getStencilRuleAttributes(AttributeSourceTag sourceTag)
{
   CAttributes* attributes = NULL;

   switch (sourceTag)
   {
   case attributeSourceComponentSubPin:
      attributes = getComponentSubPinVessel().getDefinedAttributes();
      break;
   case attributeSourceComponentPin:
      attributes = getComponentPinVessel().getDefinedAttributes();
      break;
   case attributeSourceGeometryPin:
      attributes = getSubPinVessel().getDefinedAttributes();
      break;
   case attributeSourceComponent:
      attributes = getComponentVessel().getDefinedAttributes();
      break;
   case attributeSourceSubPadstack:
      attributes = getSubPadstackVessel().getDefinedAttributes();
      break;
   case attributeSourcePadstack:
      attributes = getPadstackVessel().getDefinedAttributes();
      break;
   case attributeSourceGeometry:
      attributes = getComponentGeometryVessel().getDefinedAttributes();
      break;
   case attributeSourceMount:
      attributes = getMountVessel().getDefinedAttributes();
      break;
   case attributeSourceSurface:
      attributes = getSurfaceVessel().getDefinedAttributes();
      break;
   case attributeSourcePcb:
      attributes = getPcbVessel().getDefinedAttributes();
      break;
   }

   CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),attributes);

   return stencilRuleAttributes;
}

void CStencilHole::setStencilRuleForLevel(AttributeSourceTag sourceTag,const CStencilRule& stencilRule)
{
   m_attributeSource = sourceTag;
   m_stencilRule     = stencilRule;

   CStencilRuleAttributes stencilRuleAttributes = getStencilRuleAttributes(sourceTag);
   stencilRuleAttributes.setRule(m_stencilRule);
}

void CStencilHole::performStencilValidationChecks(CStencilViolations& stencilViolations,
   CStencilValidationParameters& validationParameters,
   CStencilGenerationParameters& generationParameters)
{
   // Don't forget to account for roundoff, e.g. "shrink" with .001 min inset resulted in poly
   // with "distance" = .000999927520..., which is probably close enough to pass rather than fail.

   CPoly* poly       = NULL;
   CPoly* copperPoly = NULL;
   CStencilViolation* stencilViolation = NULL;
   m_ruleViolations.Empty();

   if (getSplitStencilHole() != NULL)
   {
      CSplitStencilHole* splitStencilHole = getSplitStencilHole();
      poly       = getSplitStencilParentPoly();

      if (poly != NULL)
      {
         CExtent polyExtent = poly->getExtent();
         double maxPolyFeature = max(polyExtent.getXsize(),polyExtent.getYsize());
         double minPolyFeature = min(polyExtent.getXsize(),polyExtent.getYsize());

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         CSize2d splitSize(splitStencilHole->getSplitSize());
         double maxFeature = max(splitSize.cx,splitSize.cy);
         double minFeature = min(splitSize.cx,splitSize.cy);

         stencilViolation = new CStencilSplitPadViolation(getStencilSubPin(),m_topFlag,maxPolyFeature,
                                 (m_topFlag ? generationParameters.getTopMaxFeatureSize() :
                                             generationParameters.getBottomMaxFeatureSize())  );

         stencilViolations.add(stencilViolation);
         validationParameters.incrementViolationCount();
         m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         double perimeterLength = splitStencilHole->getPerimeterLength();
         double wallArea = perimeterLength * getStencilThickness();
         double holeArea = splitStencilHole->getArea();
         double holeToWallRatio = holeArea / wallArea;

         // The ratio is essentially a percentage, the reporting of the error is rounded to
         // two significant digits, i.e. only to whole percentage points,no fractions of a
         // percent. Therefore round the values to same for comparison purposes.
         int actualHoleToWallRatioPercent = round(holeToWallRatio * 100.0);
         int settingHoleToWallRatioPercent = round(validationParameters.getMinHoleAreaToWallAreaRatio() * 100.0);

         if (actualHoleToWallRatioPercent < settingHoleToWallRatioPercent)
         {
            stencilViolation = new CStencilHoleAreaToWallAreaViolation(getStencilSubPin(),m_topFlag,holeArea,wallArea,
                                    validationParameters.getMinHoleAreaToWallAreaRatio());

            stencilViolations.add(stencilViolation);
            validationParameters.incrementViolationCount();
            m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
         }

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         if (maxFeature > (validationParameters.getMaxFeature() + SMALLNUMBER))
         {
            stencilViolation = new CStencilMaximumFeatureViolation(getStencilSubPin(),m_topFlag,maxFeature,
                                    validationParameters.getMaxFeature());

            stencilViolations.add(stencilViolation);
            validationParameters.incrementViolationCount();
            m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
         }

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         if (minFeature < (validationParameters.getMinFeature() - SMALLNUMBER))
         {
            stencilViolation = new CStencilMinimumFeatureViolation(getStencilSubPin(),m_topFlag,minFeature,
                                    validationParameters.getMinFeature());

            stencilViolations.add(stencilViolation);
            validationParameters.incrementViolationCount();
            m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
         }

         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         copperPoly = getCopperPoly();

         if (copperPoly != NULL && copperPoly->ShrinkPoly(validationParameters.getMinInset(),getCamCadDatabase().getPageUnits()))
         {
            // The distance returned by poly->contains is the distance between the polys, regardless of which
            // contains which. If the stencil is not contained, we may still get a positive distance, and it
            // makes for a goofy error message when it says "setting .001 exceeds actual .001" (which is what
            // we were getting if stencil did not have any inset applied at all. So if stencil is not
            // contained then reset the distance to 0 for the error message.

            double distance;
            bool stencilContained = copperPoly->PolyContains(*poly,getCamCadDatabase().getPageUnits(),&distance);
            if (!stencilContained)
               distance = 0.0;

            if ( (!stencilContained) || (distance < (validationParameters.getMinInset() - SMALLNUMBER)) )
            {
               stencilViolation = new CStencilMinimumInsetViolation(getStencilSubPin(),m_topFlag,distance,
                                       validationParameters.getMinInset());

               stencilViolations.add(stencilViolation);
               validationParameters.incrementViolationCount();
               m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
            }
         }
      }
   }
   else if ((poly = getStencilPoly()) != NULL)
   {
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      double perimeterLength = poly->getLength();
      double wallArea = perimeterLength * getStencilThickness();
      double holeArea = poly->getArea();
      double holeToWallRatio = holeArea / wallArea;

      // The ratio is essentially a percentage, the reporting of the error is rounded to
      // two significant digits, i.e. only to whole percentage points,no fractions of a
      // percent. Therefore round the values to same for comparison purposes.
      int actualHoleToWallRatioPercent = round(holeToWallRatio * 100.0);
      int settingHoleToWallRatioPercent = round(validationParameters.getMinHoleAreaToWallAreaRatio() * 100.0);

      if (actualHoleToWallRatioPercent < settingHoleToWallRatioPercent)
      {
         stencilViolation = new CStencilHoleAreaToWallAreaViolation(getStencilSubPin(),m_topFlag,holeArea,wallArea,
                                   validationParameters.getMinHoleAreaToWallAreaRatio());

         stencilViolations.add(stencilViolation);
         validationParameters.incrementViolationCount();
         m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      CExtent polyExtent = poly->getExtent();
      double maxFeature = max(polyExtent.getXsize(),polyExtent.getYsize());
      double minFeature = min(polyExtent.getXsize(),polyExtent.getYsize());

      if (maxFeature > (validationParameters.getMaxFeature() + SMALLNUMBER))
      {
         stencilViolation = new CStencilMaximumFeatureViolation(getStencilSubPin(),m_topFlag,maxFeature,
                                   validationParameters.getMaxFeature());

         stencilViolations.add(stencilViolation);
         validationParameters.incrementViolationCount();
         m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (minFeature < (validationParameters.getMinFeature() - SMALLNUMBER))
      {
         stencilViolation = new CStencilMinimumFeatureViolation(getStencilSubPin(),m_topFlag,minFeature,
                                   validationParameters.getMinFeature());

         stencilViolations.add(stencilViolation);
         validationParameters.incrementViolationCount();
         m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      copperPoly = getCopperPoly();

      if (copperPoly != NULL && copperPoly->ShrinkPoly(validationParameters.getMinInset(),getCamCadDatabase().getPageUnits()))
      {
         // The distance returned by poly->contains is the distance between the polys, regardless of which
         // contains which. If the stencil is not contained, we may still get a positive distance, and it
         // makes for a goofy error message when it says "setting .001 exceeds actual .001" (which is what
         // we were getting if stencil did not have any inset applied at all. So if stencil is not
         // contained then reset the distance to 0 for the error message.

         double distance;
         bool stencilContained = copperPoly->PolyContains(*poly,getCamCadDatabase().getPageUnits(),&distance);
         if (!stencilContained)
            distance = 0.0;

         if ( (!stencilContained) || (distance < (validationParameters.getMinInset() - SMALLNUMBER)) )
         {
            stencilViolation = new CStencilMinimumInsetViolation(getStencilSubPin(),m_topFlag,distance,
                                    validationParameters.getMinInset());

            stencilViolations.add(stencilViolation);
            validationParameters.incrementViolationCount();
            m_ruleViolations += stencilViolation->getAbbreviatedDescription() + " ";
         }
      }
   }

   delete poly;
   delete copperPoly;
}

void CStencilHole::clearStencilHole()
{
   delete m_stencilHole;
   m_stencilHole = NULL;

   //delete m_copperPad;
   //m_copperPad = NULL;

   //delete m_splitStencilHole;
   //m_splitStencilHole = NULL;
}

void CStencilHole::clearStencilVessels()
{
   m_componentGeometryVessel  = NULL;
   m_padstackVessel           = NULL;
   m_subPadstackVessel        = NULL;
   m_subPinVessel             = NULL;
   m_componentVessel          = NULL;
   m_componentPinVessel       = NULL;
   m_componentSubPinVessel    = NULL;
}

void CStencilHole::instantiateStencilVessels()
{
   getStencilRuleAttributes(attributeSourcePcb              );
   getStencilRuleAttributes(attributeSourceSurface          );
   getStencilRuleAttributes(attributeSourceMount            );

   getStencilRuleAttributes(attributeSourceGeometry         );
   getStencilRuleAttributes(attributeSourcePadstack         );
   getStencilRuleAttributes(attributeSourceSubPadstack      );
   getStencilRuleAttributes(attributeSourceGeometryPin      );

   getStencilRuleAttributes(attributeSourceComponent        );
   getStencilRuleAttributes(attributeSourceComponentPin     );
   getStencilRuleAttributes(attributeSourceComponentSubPin  );
}

//_____________________________________________________________________________
CStencilSubPin::CStencilSubPin(CStencilPin& stencilPin,int subPinIndex/*,DataStruct& subPadstackVessel,DataStruct& subPinVessel*/)
: m_stencilPin(stencilPin)
, m_subPinIndex(subPinIndex)
, m_topStencilHole(*this,true)
, m_bottomStencilHole(*this,false)
//, m_subPadstackVessel(subPadstackVessel)
//, m_subPinVessel(subPinVessel)
{
   m_subPinName.Format("%d",subPinIndex + 1);
   m_subPinRef = getPinRef() + "." + m_subPinName;
   m_sortableSubPinName = CompPinStruct::getSortableReference(m_subPinName);
   m_sortableSubPinRef = getSortablePinRef() + "." + m_sortableSubPinName;
}

CCamCadDatabase& CStencilSubPin::getCamCadDatabase() const
{
   return m_stencilPin.getCamCadDatabase();
}

int CStencilSubPin::getSubPinIndex() const
{
   return m_subPinIndex;
}

CString CStencilSubPin::getGeometryName() const
{
   return m_stencilPin.getGeometryName();
}

CString CStencilSubPin::getPadStackName() const
{
   return m_stencilPin.getPadStackName();
}

//CString CStencilSubPin::getSubPadstackName() const
//{
//   return m_subPadstackVessel.getInsert()->getRefname();
//}

CString CStencilSubPin::getSortablePinName() const
{
   return m_stencilPin.getSortablePinName();
}

CString CStencilSubPin::getPinRef() const
{
   return m_stencilPin.getPinRef();
}

CString CStencilSubPin::getSubPinRef() const
{
   return m_subPinRef;
}

CString CStencilSubPin::getSubPinName() const
{
   return m_subPinName;
}

CString CStencilSubPin::getPinName() const
{
   return m_stencilPin.getPinName();
}

CString CStencilSubPin::getSortablePinRef() const
{
   return m_stencilPin.getSortablePinRef();
}

CString CStencilSubPin::getSortableSubPinRef() const
{
   return m_sortableSubPinRef;
}

CStencilPin& CStencilSubPin::getStencilPin() const
{
   return m_stencilPin;
}

DataStruct& CStencilSubPin::getComponent()
{
   return m_stencilPin.getComponent();
}

BlockStruct* CStencilSubPin::getPadStackGeometry() const
{
   return m_stencilPin.getPadStackGeometry();
}

DataStruct* CStencilSubPin::getSubPad(bool topFlag) const
{
   DataStruct* subPad = NULL;

   BlockStruct* padStackGeometry = getPadStackGeometry();

   if (padStackGeometry != NULL)
   {
      CPadStackInsert padStackInsert(getCamCadDatabase(),getStencilPin().getPadStack());

      subPad = padStackInsert.getSubPad(m_subPinIndex,topFlag);
   }

   return subPad;
}

//DataStruct& CStencilSubPin::getComponentSubPinVessel() const
//{
//   return getStencilPin().getStencilGenerator().getComponentSubPinVessel(m_subPinRef);
//}
//
//DataStruct& CStencilSubPin::getSubPadstackVessel()
//{
//   return m_subPadstackVessel;
//}
//
//DataStruct& CStencilSubPin::getSubPinVessel()
//{
//   return m_subPinVessel;
//}

CString CStencilSubPin::getSortableGeomPadstackSubpinPin(bool topFlag) const
{
   CString sortableString;

   sortableString.Format("%s:%s:%d:%s",getGeometryName(),getPadStackName(),getSubPinIndex(),getPinName());

   return sortableString;
}

void CStencilSubPin::clearStencilVessels()
{
   m_topStencilHole.clearStencilVessels();
   m_bottomStencilHole.clearStencilVessels();
}

void CStencilSubPin::instantiateStencilVessels()
{
   m_topStencilHole.instantiateStencilVessels();
   m_bottomStencilHole.instantiateStencilVessels();
}

//_____________________________________________________________________________
CStencilSubPinArray::CStencilSubPinArray(CStencilPin& stencilPin)
: m_stencilPin(stencilPin)
{
}

CStencilSubPin& CStencilSubPinArray::addSubPin(/*DataStruct& subPadstackVessel,DataStruct& subPinVessel*/)
{
   int subPinIndex = m_subPins.GetCount();

   CStencilSubPin* subPin = new CStencilSubPin(m_stencilPin,subPinIndex/*,subPadstackVessel,subPinVessel*/);

   m_subPins.Add(subPin);

   return *subPin;
}

bool CStencilSubPinArray::hasCopperPad(bool topFlag) const
{
   bool hasCopperPadFlag = false;

   for (int subPinIndex = 0;subPinIndex < getCount() && !hasCopperPadFlag;subPinIndex++)
   {
      hasCopperPadFlag = (getStencilSubPin(subPinIndex)->getStencilHole(topFlag).getCopperPad() != NULL);
   }

   return hasCopperPadFlag;
}

void CStencilSubPinArray::clearStencilHoles()
{
   for (int subPinIndex = 0;subPinIndex < getCount();subPinIndex++)
   {
      getStencilSubPin(subPinIndex)->getStencilHole(true ).clearStencilHole();
      getStencilSubPin(subPinIndex)->getStencilHole(false).clearStencilHole();
   }
}

void CStencilSubPinArray::clearStencilVessels()
{
   for (int subPinIndex = 0;subPinIndex < getCount();subPinIndex++)
   {
      getStencilSubPin(subPinIndex)->getStencilHole(true ).clearStencilVessels();
      getStencilSubPin(subPinIndex)->getStencilHole(false).clearStencilVessels();
   }
}

void CStencilSubPinArray::instantiateStencilVessels()
{
   for (int subPinIndex = 0;subPinIndex < getCount();subPinIndex++)
   {
      getStencilSubPin(subPinIndex)->instantiateStencilVessels();
   }
}

bool CStencilSubPinArray::updateStencilHoleAttributes()
{
   bool retval = true;

   for (int subPinIndex = 0;subPinIndex < getCount();subPinIndex++)
   {
      if (!getStencilSubPin(subPinIndex)->getStencilHole(true ).updateStencilHoleAttribute())
      {
         retval = false;
      }

      if (!getStencilSubPin(subPinIndex)->getStencilHole(false).updateStencilHoleAttribute())
      {
         retval = false;
      }
   }

   return retval;
}

int CStencilSubPinArray::getCount() const
{
   return m_subPins.GetCount();
}

CStencilSubPin* CStencilSubPinArray::getStencilSubPin(int index) const
{
   CStencilSubPin* stencilSubPin = NULL;

   if (index >= 0 && index < getCount())
   {
      stencilSubPin = m_subPins.GetAt(index);
   }

   return stencilSubPin;
}

CStencilHole* CStencilSubPinArray::getStencilHole(int subPinIndex,bool topFlag)
{
   CStencilHole* stencilHole = NULL;
   CStencilSubPin* subPin = getStencilSubPin(subPinIndex);

   if (subPin != NULL)
   {
      stencilHole = &(subPin->getStencilHole(topFlag));
   }

   return stencilHole;
}

const CStencilHole* CStencilSubPinArray::getStencilHole(int subPinIndex,bool topFlag) const
{
   CStencilHole* stencilHole = NULL;
   CStencilSubPin* subPin = getStencilSubPin(subPinIndex);

   if (subPin != NULL)
   {
      stencilHole = &(subPin->getStencilHole(topFlag));
   }

   return stencilHole;
}

////_____________________________________________________________________________
//CStencilHoleArray::CStencilHoleArray(CStencilPin& stencilPin)
//: m_stencilPin(stencilPin)
//{
//   m_topStencilHoles.Add(   new CStencilHole(stencilPin,true ));
//   m_bottomStencilHoles.Add(new CStencilHole(stencilPin,false));
//}
//
//bool CStencilHoleArray::hasCopperPad(bool topFlag) const
//{
//   bool hasCopperPadFlag = false;
//
//   for (int holeIndex = 0;holeIndex < getCount() && !hasCopperPadFlag;holeIndex++)
//   {
//     hasCopperPadFlag = (getStencilHole(holeIndex,topFlag).getCopperPad() != NULL);
//   }
//
//   return hasCopperPadFlag;
//}
//
//void CStencilHoleArray::clearStencilHoles()
//{
//   for (int holeIndex = 0;holeIndex < getCount();holeIndex++)
//   {
//      getStencilHole(holeIndex,true ).clearStencilHole();
//      getStencilHole(holeIndex,false).clearStencilHole();
//   }
//}
//
//bool CStencilHoleArray::updateStencilHoleAttributes(bool topFlag)
//{
//   bool retval = true;
//
//   for (int holeIndex = 0;holeIndex < getCount();holeIndex++)
//   {
//      retval = getStencilHole(holeIndex,topFlag).updateStencilHoleAttribute() && retval;
//   }
//
//   return retval;
//}
//
//int CStencilHoleArray::getCount() const
//{
//   return m_topStencilHoles.GetCount();
//}
//
//CStencilHole& CStencilHoleArray::getStencilHole(int index,bool topFlag) const
//{
//   if (index < 0 || index >= getCount())
//   {
//      index = 0;
//   }
//
//   CStencilHole* stencilHole = (topFlag ? m_topStencilHoles.GetAt(index) : m_bottomStencilHoles.GetAt(index));
//
//   return *stencilHole;
//}

//_____________________________________________________________________________
int CStencilPin::m_nextId = 0;

int CStencilPin::m_stencilPinrefKeywordIndex                   = -1;
int CStencilPin::m_stencilDerivationMethodKeywordIndex         = -1;
int CStencilPin::m_stencilThicknessKeywordIndex                = -1;
int CStencilPin::m_stencilPasteVolumeKeywordIndex              = -1;

int CStencilPin::m_stencilTopThicknessKeywordIndex             = -1;
int CStencilPin::m_stencilBottomThicknessKeywordIndex          = -1;
int CStencilPin::m_stencilTopPasteInHoleKeywordIndex           = -1;
int CStencilPin::m_stencilBottomPasteInHoleKeywordIndex        = -1;
int CStencilPin::m_stencilTopPasteInViaKeywordIndex            = -1;
int CStencilPin::m_stencilBottomPasteInViaKeywordIndex         = -1;
int CStencilPin::m_stencilTopMaxFeatureKeywordIndex            = -1;
int CStencilPin::m_stencilBottomMaxFeatureKeywordIndex         = -1;
int CStencilPin::m_stencilTopWebWidthKeywordIndex              = -1;
int CStencilPin::m_stencilBottomWebWidthKeywordIndex           = -1;
int CStencilPin::m_stencilTopStencilSourceLayerKeywordIndex    = -1;
int CStencilPin::m_stencilBottomStencilSourceLayerKeywordIndex = -1;
int CStencilPin::m_stencilExteriorCornerRadiusKeywordIndex     = -1;
int CStencilPin::m_stencilPropagateRulesKeywordIndex           = -1;
int CStencilPin::m_stencilUseMountTechAttribKeywordIndex       = -1;
int CStencilPin::m_stencilMountTechAttribNameKeywordIndex      = -1;
int CStencilPin::m_stencilMountTechAttribSMDValueKeywordIndex  = -1;

void CStencilPin::resetNextId()
{
   m_nextId = 0;
}

void CStencilPin::resetKeywordIndices()
{
   m_stencilPinrefKeywordIndex                   = -1;
   m_stencilDerivationMethodKeywordIndex         = -1;
   m_stencilThicknessKeywordIndex                = -1;
   m_stencilPasteVolumeKeywordIndex              = -1;

   m_stencilTopThicknessKeywordIndex             = -1;
   m_stencilBottomThicknessKeywordIndex          = -1;
   m_stencilTopPasteInHoleKeywordIndex           = -1;
   m_stencilBottomPasteInHoleKeywordIndex        = -1;
   m_stencilTopPasteInViaKeywordIndex            = -1;
   m_stencilBottomPasteInViaKeywordIndex         = -1;
   m_stencilTopMaxFeatureKeywordIndex            = -1;
   m_stencilBottomMaxFeatureKeywordIndex         = -1;
   m_stencilTopWebWidthKeywordIndex              = -1;
   m_stencilBottomWebWidthKeywordIndex           = -1;
   m_stencilTopStencilSourceLayerKeywordIndex    = -1;
   m_stencilBottomStencilSourceLayerKeywordIndex = -1;
   m_stencilExteriorCornerRadiusKeywordIndex     = -1;
   m_stencilPropagateRulesKeywordIndex           = -1;
   m_stencilUseMountTechAttribKeywordIndex       = -1;
   m_stencilMountTechAttribNameKeywordIndex      = -1;
   m_stencilMountTechAttribSMDValueKeywordIndex  = -1;
}

CStencilPin::CStencilPin(CStencilGenerator& stencilGenerator,/*CComponentPin& componentPin,*/DataStruct& component,
      /*DataStruct& padstackVessel,*/DataStruct& padStack,BlockStruct& pcb)
: m_stencilGenerator(stencilGenerator)
//, m_componentPin(componentPin)
, m_component(component)
//, m_padstackVessel(padstackVessel)
, m_padStack(padStack)
, m_pcb(pcb)
, m_subPins(*this)
{
   m_id = m_nextId++;

   m_attributesModifiedFlag = false;

   m_drillToolValid    = false;
   m_drillTool         = NULL;

   m_sortableRefDes  = CompPinStruct::getSortableReference(getRefDes());
   m_sortablePinName = CompPinStruct::getSortableReference(getPinName());
   m_sortablePinRef  = m_sortableRefDes + "." + m_sortablePinName;
}

CStencilPin::~CStencilPin()
{
}

CCamCadDatabase& CStencilPin::getCamCadDatabase() const
{
   return m_stencilGenerator.getCamCadDatabase();
}

CStencilGenerator& CStencilPin::getStencilGenerator() const
{
   return m_stencilGenerator;
}

//CComponentPin& CStencilPin::getComponentPin()
//{
//   return m_componentPin;
//}

DataStruct& CStencilPin::getComponent()
{
   return m_component;
}

DataStruct& CStencilPin::getComponentPinVessel(bool topFlag) const
{
   CString vesselRef(::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(getRefDes()) +
                                                 getRefDelimiter() + getDesignatorString(getPinName()));

   return getStencilGenerator().getClosetVessel(vesselRef,getStencilGenerator().getComponentPinVesselGeometry());
}

void CStencilPin::registerStencilKeywords(CCamCadDatabase& camCadDatabase,CWriteFormat& errorLog)
{
   m_stencilPinrefKeywordIndex                   = camCadDatabase.registerHiddenKeyWord("StencilPinref"               ,0,valueTypeString    ,errorLog);
   m_stencilDerivationMethodKeywordIndex         = camCadDatabase.registerHiddenKeyWord("StencilDerivationMethod"     ,0,valueTypeString    ,errorLog);
   m_stencilThicknessKeywordIndex                = camCadDatabase.registerHiddenKeyWord("StencilThickness"            ,0,valueTypeUnitDouble,errorLog);
   m_stencilPasteVolumeKeywordIndex              = camCadDatabase.registerHiddenKeyWord("StencilPasteVolume"          ,0,valueTypeDouble    ,errorLog);

   m_stencilTopThicknessKeywordIndex             = camCadDatabase.registerHiddenKeyWord("StencilThicknessTop"         ,0,valueTypeUnitDouble,errorLog);
   m_stencilBottomThicknessKeywordIndex          = camCadDatabase.registerHiddenKeyWord("StencilThicknessBottom"      ,0,valueTypeUnitDouble,errorLog);
   m_stencilTopMaxFeatureKeywordIndex            = camCadDatabase.registerHiddenKeyWord("StencilMaxFeatureTop"        ,0,valueTypeUnitDouble,errorLog);
   m_stencilBottomMaxFeatureKeywordIndex         = camCadDatabase.registerHiddenKeyWord("StencilMaxFeatureBottom"     ,0,valueTypeUnitDouble,errorLog);
   m_stencilTopWebWidthKeywordIndex              = camCadDatabase.registerHiddenKeyWord("StencilWebWidthTop"          ,0,valueTypeUnitDouble,errorLog);
   m_stencilBottomWebWidthKeywordIndex           = camCadDatabase.registerHiddenKeyWord("StencilWebWidthBottom"       ,0,valueTypeUnitDouble,errorLog);
   m_stencilTopStencilSourceLayerKeywordIndex    = camCadDatabase.registerHiddenKeyWord("StencilSourceLayerTop"       ,0,valueTypeString    ,errorLog);
   m_stencilBottomStencilSourceLayerKeywordIndex = camCadDatabase.registerHiddenKeyWord("StencilSourceLayerBottom"    ,0,valueTypeString    ,errorLog);
   m_stencilExteriorCornerRadiusKeywordIndex     = camCadDatabase.registerHiddenKeyWord("StencilExteriorCornerRadius" ,0,valueTypeUnitDouble,errorLog);
   m_stencilTopPasteInHoleKeywordIndex           = camCadDatabase.registerHiddenKeyWord("StencilPasteInHoleTop"       ,0,valueTypeInteger   ,errorLog);
   m_stencilBottomPasteInHoleKeywordIndex        = camCadDatabase.registerHiddenKeyWord("StencilPasteInHoleBottom"    ,0,valueTypeInteger   ,errorLog);
   m_stencilTopPasteInViaKeywordIndex            = camCadDatabase.registerHiddenKeyWord("StencilPasteInViaTop"        ,0,valueTypeInteger   ,errorLog);
   m_stencilBottomPasteInViaKeywordIndex         = camCadDatabase.registerHiddenKeyWord("StencilPasteInViaBottom"     ,0,valueTypeInteger   ,errorLog);
   m_stencilPropagateRulesKeywordIndex           = camCadDatabase.registerHiddenKeyWord("PropagateRulesSideToSide"    ,0,valueTypeString    ,errorLog);
   m_stencilUseMountTechAttribKeywordIndex       = camCadDatabase.registerHiddenKeyWord("UseMountTechAttrib"          ,0,valueTypeString    ,errorLog);
   m_stencilMountTechAttribNameKeywordIndex      = camCadDatabase.registerHiddenKeyWord("MountTechAttribName"         ,0,valueTypeString    ,errorLog);
   m_stencilMountTechAttribSMDValueKeywordIndex  = camCadDatabase.registerHiddenKeyWord("MountTechAttribSMDValue"     ,0,valueTypeString    ,errorLog);


}

DataStruct& CStencilPin::getPadStack()
{
   return m_padStack;
}

BlockStruct& CStencilPin::getPcb()
{
   return m_pcb;
}

CPoint2d& CStencilPin::getOrigin()
{
   return m_origin;
}

int CStencilPin::getId() const
{
   return m_id;
}

CString CStencilPin::getRefDes() const
{
   return m_component.getInsert()->getRefname();
}

CString CStencilPin::getPinName() const
{
   return m_padStack.isInsert() ? m_padStack.getInsert()->getRefname() : "None";
}

CString CStencilPin::getPinRef() const
{
   return getRefDes() + "." + getPinName();
}

CString CStencilPin::getSortablePinName() const
{
   return m_sortablePinName;
}

CString CStencilPin::getFirstPinName()
{
   if (m_firstPinName.IsEmpty())
   {
      m_firstPinName = ::getFirstPinName(*(getGeometry()),m_padStack);
   }

   return m_firstPinName;
}

CString CStencilPin::getSortablePinRef() const
{
   return m_sortablePinRef;
}

CPoint2d CStencilPin::getOrigin() const
{
   return m_origin;
}

BlockStruct* CStencilPin::getGeometry() const
{
   BlockStruct* geometry = getCamCadDatabase().getBlock(m_component.getInsert()->getBlockNumber());

   return geometry;
}

CString CStencilPin::getSortableGeomPad(bool topFlag) const
{
   return getGeometryName() + ":" + getPadStackName();
}

CString CStencilPin::getSortableGeomPadPin(bool topFlag) const
{
   return getGeometryName() + ":" + getPadStackName() + ":" + getPinName();
}

CString CStencilPin::getGeometryName() const
{
   return getGeometry()->getName();
}

BlockStruct* CStencilPin::getPadStackGeometry() const
{
   if (m_padStack.isInsert())
   {
      BlockStruct* padStack = getCamCadDatabase().getBlock(m_padStack.getInsert()->getBlockNumber());
      return padStack;
   }
   return NULL;
}

CString CStencilPin::getPadStackName() const
{
   return getPadStackGeometry() != NULL ? getPadStackGeometry()->getName() : "None";
}

DataStruct* CStencilPin::getDrillTool()
{
   if (! m_drillToolValid && m_padStack.isInsert())
   {
      BlockStruct* padStackGeometry = getCamCadDatabase().getBlock(m_padStack.getInsert()->getBlockNumber());

      for (CDataListIterator dataIterator(*padStackGeometry,dataTypeInsert);dataIterator.hasNext();)
      {
         DataStruct* tool = dataIterator.getNext();
         InsertStruct* toolInsert = tool->getInsert();
         BlockStruct* toolGeometry = getCamCadDatabase().getBlock(toolInsert->getBlockNumber());

         if (toolGeometry->isDrillHole())
         {
            m_drillTool = tool;
            break;
         }
      }

      m_drillToolValid = true;
   }

   return m_drillTool;
}

bool CStencilPin::hasDrillTool()
{
   return (getDrillTool() != NULL);
}

CStencilHole& CStencilPin::getStencilHole(int holeIndex,bool topFlag)
{
   return m_subPins.getStencilSubPin(holeIndex)->getStencilHole(topFlag);
}

const CStencilHole& CStencilPin::getStencilHole(int holeIndex,bool topFlag) const
{
   return m_subPins.getStencilSubPin(holeIndex)->getStencilHole(topFlag);
}

//CStencilHoleArray& CStencilPin::getStencilHoles()
//{
//   return m_stencilHoles;
//}
//
//const CStencilHoleArray& CStencilPin::getStencilHoles() const
//{
//   return m_stencilHoles;
//}
//
//int CStencilPin::getStencilHoleCount() const
//{
//   return m_stencilHoles.getCount();
//}

CStencilSubPin& CStencilPin::addSubPin(/*DataStruct& subPadstackVessel,DataStruct& subPinVessel*/)
{
   return m_subPins.addSubPin(/*subPadstackVessel,subPinVessel*/);
}

CStencilSubPin* CStencilPin::getSubPin(int subPinIndex) const
{
   return m_subPins.getStencilSubPin(subPinIndex);
}

CStencilSubPinArray& CStencilPin::getSubPins()
{
   return m_subPins;
}

const CStencilSubPinArray& CStencilPin::getSubPins() const
{
   return m_subPins;
}

int CStencilPin::getSubPinCount() const
{
   return m_subPins.getCount();
}

void CStencilPin::clearStencilHoles()
{
   m_subPins.clearStencilHoles();
}

void CStencilPin::clearStencilVessels()
{
   m_subPins.clearStencilVessels();
}

void CStencilPin::instantiateStencilVessels()
{
   m_subPins.instantiateStencilVessels();
}

bool CStencilPin::isSmdPin() const
{
   //bool smdFlag = ((m_topStencilHole.getCopperPad() == NULL) || (m_bottomStencilHole.getCopperPad() == NULL));
   bool smdFlag = (!m_subPins.hasCopperPad(true) || !m_subPins.hasCopperPad(false));

   return smdFlag;
}

bool CStencilPin::isSmd() const
{
   bool smdFlag = getStencilGenerator().isSmdGeometry(getGeometryName());

   return smdFlag;
}

bool CStencilPin::isVia() const
{
   bool viaFlag = getStencilGenerator().isViaGeometry(getGeometryName());

   return viaFlag;
}

bool CStencilPin::isFiducial() const
{
   bool fiducialFlag = getStencilGenerator().isFiducialGeometry(getGeometryName());

   return fiducialFlag;
}

bool CStencilPin::updateStencilHoleAttribute()
{
   //bool retval = m_topStencilHole.updateStencilHoleAttribute();
   //retval      = m_bottomStencilHole.updateStencilHoleAttribute() && retval;
   bool retval = m_subPins.updateStencilHoleAttributes();

   return retval;
}

void CStencilPin::setAttributesModified(bool flag)
{
   m_attributesModifiedFlag = flag;

   if (m_attributesModifiedFlag)
   {
      CStencilUi::getStencilUi(getCamCadDatabase().getCamCadDoc()).generateEvent(ID_StencilCommand_ModifiedGenerationRule);
   }
}

//_____________________________________________________________________________
CStencilPins::CStencilPins()
{
}

CStencilPins::~CStencilPins()
{
}

CStencilPin& CStencilPins::addStencilPin(CStencilGenerator& stencilGenerator,
   /*CComponentPin& componentPin,*/DataStruct& component,/*DataStruct& padstackVessel,*/DataStruct& padStack,BlockStruct& pcb)
{
   CStencilPin* stencilPin = new CStencilPin(stencilGenerator,/*componentPin,*/component,/*padstackVessel,*/padStack,pcb);
   AddTail(stencilPin);

   return *stencilPin;
}

void CStencilPins::clearStencilVessels()
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = GetNext(pos);

      stencilPin->clearStencilVessels();
   }
}

void CStencilPins::instantiateStencilVessels()
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = GetNext(pos);

      stencilPin->instantiateStencilVessels();
   }
}

void CStencilPins::clearAttributesModifiedFlags()
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = GetNext(pos);

      stencilPin->setAttributesModified(false);
   }
}

bool CStencilPins::getAttributesModified()
{
   bool retval = false;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = GetNext(pos);

      if (stencilPin->getAttributesModified())
      {
         retval = true;
         break;
      }
   }

   return retval;
}

bool CStencilPins::isValid()
{
   bool retval = true;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CStencilPin* stencilPin = GetNext(pos);

      if (! AfxIsValidAddress(stencilPin,sizeof(CStencilPin),true))
      {
         retval = false;
      }
   }

   return retval;
}

void CStencilPins::addStencilEntities(CStencilGenerator& stencilGenerator,FileStruct& fileStruct)
{
   CCamCadDatabase& camCadDatabase = stencilGenerator.getCamCadDatabase();
   BlockStruct* pcb = fileStruct.getBlock();
   //BlockStruct& componentVesselGeometry   = stencilGenerator.getComponentGeometryVesselGeometry();
   //BlockStruct& padstackVesselGeometry    = stencilGenerator.getPadstackVesselGeometry();
   //BlockStruct& subPadstackVesselGeometry = stencilGenerator.getSubPadstackVesselGeometry();
   //BlockStruct& pinVesselGeometry         = stencilGenerator.getSubPinVesselGeometry();

   for (CDataListIterator dataListIterator(*pcb,insertTypePcbComponent);dataListIterator.hasNext();)
   {
      DataStruct* component = dataListIterator.getNext();
      InsertStruct* componentInsert = component->getInsert();
      bool topFlag = componentInsert->getPlacedTop();

      BlockStruct* componentGeometry = camCadDatabase.getBlock(componentInsert->getBlockNumber());
      CString refDes = componentInsert->getRefname();

      for (CDataListIterator dataIterator(*componentGeometry,CInsertTypeMask(insertTypePin,insertTypeMechanicalPin));dataIterator.hasNext();)
      {
         DataStruct* pin = dataIterator.getNext();
         InsertStruct* pinInsert = pin->getInsert();
         CString pinName = pinInsert->getRefname();

         CPadStackInsert padStackInsert(camCadDatabase,*pin);

         if (padStackInsert.getSubPadCount() > 0)
         {
            CStencilPin& stencilPin = this->addStencilPin(stencilGenerator,/**componentPin,*/*component,/**padstackVessel,*/*pin,*pcb);

            for (int subPadIndex = 0;subPadIndex < padStackInsert.getSubPadCount();subPadIndex++)
            {
               CStencilSubPin& subPin = stencilPin.addSubPin();
            }
         }
      }
   }
}

void CStencilPins::initializeRoot()
{
   for (POSITION stencilPinPos = this->GetHeadPosition();stencilPinPos != NULL;)
   {
      CStencilPin* stencilPin = this->GetNext(stencilPinPos);
      CStencilSubPinArray& subPins = stencilPin->getSubPins();

      for (int subPinIndex = 0;subPinIndex < subPins.getCount();subPinIndex++)
      {
         CStencilHole* stencilHole = subPins.getStencilHole(subPinIndex,true);
         CStencilRule stencilRule;

         AttributeSourceTag attributeSource = stencilHole->getStencilRuleForSourceLevel(attributeSourcePcb,stencilRule);

         if (attributeSource != attributeSourcePcb)
         {
            stencilRule.setRule(stencilRuleNone);
            CStencilRuleAttributes stencilRuleAttributes = stencilHole->getStencilRuleAttributes(attributeSourcePcb);
            stencilRuleAttributes.setRule(stencilRule);
         }
      }

      break;
   }
}

void CStencilPins::calcStencilHoles(CDataList& topStencilHoleDataList,CDataList& bottomStencilHoleDataList,
   CCamCadDatabase& camCadDatabase,
   CStencilGenerationParameters& stencilGenerationParameters,FileStruct& fileStruct)
{
   CCamCadData& camCadData = camCadDatabase.getCamCadData();
   CStencilGroups stencilGroups;
   CNullWriteFormat logFile;

   LayerStruct* topStencilLayer    = camCadDatabase.getLayer(ccLayerStencilTop   );
   LayerStruct* bottomStencilLayer = camCadDatabase.getLayer(ccLayerStencilBottom);

   BlockStruct* pcb = fileStruct.getBlock();
   CString stringValue;
   bool topPasteInHoleFlag    = false;
   bool bottomPasteInHoleFlag = false;
   bool topPasteInViaFlag     = false;
   bool bottomPasteInViaFlag  = false;

   if (camCadDatabase.getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeTopPasteInHole)))
   {
      topPasteInHoleFlag = (atoi(stringValue) != 0);
   }

   if (camCadDatabase.getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInHole)))
   {
      bottomPasteInHoleFlag = (atoi(stringValue) != 0);
   }

   if (camCadDatabase.getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeTopPasteInVia)))
   {
      topPasteInViaFlag = (atoi(stringValue) != 0);
   }

   if (camCadDatabase.getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInVia)))
   {
      bottomPasteInViaFlag = (atoi(stringValue) != 0);
   }

   for (POSITION stencilPinPos = this->GetHeadPosition();stencilPinPos != NULL;)
   {
      CStencilPin* stencilPin = this->GetNext(stencilPinPos);
      stencilPin->clearStencilHoles();

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         //CStencilSubPin* subPin = stencilPin->getSubPins(subPinIndex);

         CStencilHole& topStencilHole    = stencilPin->getStencilHole(subPinIndex,true );
         CStencilHole& bottomStencilHole = stencilPin->getStencilHole(subPinIndex,false);
         CString pinRef = stencilPin->getPinRef();

         DataStruct* topStencilHoleTemplate    =
            topStencilHole.calcStencilHole(stencilGroups,&fileStruct,
               stencilGenerationParameters.getTopMaxFeatureSize(),
               stencilGenerationParameters.getTopWebSize());

         DataStruct* bottomStencilHoleTemplate =
            bottomStencilHole.calcStencilHole(stencilGroups,&fileStruct,
               stencilGenerationParameters.getBottomMaxFeatureSize(),
               stencilGenerationParameters.getBottomWebSize());

         bool thFlag  = (topStencilHoleTemplate != NULL && bottomStencilHoleTemplate != NULL);
         bool viaFlag = stencilPin->isVia();

         topStencilHole.setStencilHoleInstance(NULL);
         bottomStencilHole.setStencilHoleInstance(NULL);

         bool topHoleDisabledFlag    = ((viaFlag && !topPasteInViaFlag   ) || (thFlag && !topPasteInHoleFlag    && !viaFlag));
         bool bottomHoleDisabledFlag = ((viaFlag && !bottomPasteInViaFlag) || (thFlag && !bottomPasteInHoleFlag && !viaFlag));

         if ((topStencilHoleTemplate != NULL) && !topHoleDisabledFlag)
         {
            topStencilHoleTemplate->setLayerIndex(topStencilLayer->getLayerIndex());
            DataStruct* stencilHoleInstance = newDataStructO(*topStencilHoleTemplate);
            topStencilHole.setStencilHoleInstance(stencilHoleInstance);
            topStencilHole.addAttributes(stencilHoleInstance/*,logFile*/);
            topStencilHoleDataList.AddTail(stencilHoleInstance);
         }

         if ((bottomStencilHoleTemplate != NULL) && !bottomHoleDisabledFlag)
         {
            bottomStencilHoleTemplate->setLayerIndex(bottomStencilLayer->getLayerIndex());
            DataStruct* stencilHoleInstance = newDataStructO(*bottomStencilHoleTemplate);
            bottomStencilHole.setStencilHoleInstance(stencilHoleInstance);
            bottomStencilHole.addAttributes(stencilHoleInstance/*,logFile*/);
            bottomStencilHoleDataList.AddTail(stencilHoleInstance);
         }
      }
   }
}

//_____________________________________________________________________________
CComponentDirectory::CComponentDirectory(CCamCadDatabase& camCadDatabase,FileStruct* fileStruct)
{
   if (fileStruct != NULL)
   {
      BlockStruct* pcbBlock = fileStruct->getBlock();

      if (pcbBlock != NULL)
      {
         for (CDataListIterator dataIterator(*pcbBlock,dataTypeInsert);dataIterator.hasNext();)
         {
            DataStruct* component = dataIterator.getNext();
            InsertStruct* componentInsert = component->getInsert();

            if (componentInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               CString refdes = componentInsert->getRefname();

               SetAt(refdes,component);
            }
         }
      }
   }
}

DataStruct* CComponentDirectory::getComponent(const CString& refdes)
{
   DataStruct* component = NULL;

   if (! Lookup(refdes,component))
   {
      component = NULL;
   }

   return component;
}

//_____________________________________________________________________________
int CPadStackInsert::m_subPadIndexKeywordIndex = -1;
int CPadStackInsert::m_stencilSourceSubPadIndexKeywordIndex = -1;

CPadStackInsert::CPadStackInsert(CCamCadDatabase& camCadDatabase,DataStruct& padStackInsertData)
: m_camCadDatabase(camCadDatabase)
, m_padStackInsertData(padStackInsertData)
{
   initializeSubPads(true );
   initializeSubPads(false);
}

CPadStackInsert::CPadStackInsert(CCamCadDatabase& camCadDatabase,DataStruct& padStackInsertData,const CString& topStencilSourceLayerName,const CString& bottomStencilSourceLayerName)
: m_camCadDatabase(camCadDatabase)
, m_padStackInsertData(padStackInsertData)
, m_topStencilSourceLayerName(topStencilSourceLayerName)
, m_bottomStencilSourceLayerName(bottomStencilSourceLayerName)
{
   initializeSubPads(true );
   initializeSubPads(false);
}

CCamCadData& CPadStackInsert::getCamCadData() const
{
   return m_camCadDatabase.getCamCadData();
}

CCEtoODBDoc& CPadStackInsert::getCamCadDoc() const
{
   return m_camCadDatabase.getCamCadDoc();
}

bool CPadStackInsert::isAllCopperLayer(const LayerStruct& layer) const
{
   bool retval = (layer.getLayerType() == layerTypePadAll || layer.getLayerType() == layerTypeSignalAll);

   return retval;
}

bool CPadStackInsert::isOuterCopperLayer(const LayerStruct& layer) const
{
   bool retval = (layer.getLayerType() == layerTypePadOuter || layer.getLayerType() == layerTypeSignalOuter);

   return retval;
}

bool CPadStackInsert::isTopCopperLayer(const LayerStruct& layer) const
{
   bool retval = (isAllCopperLayer(layer) || /* isOuterCopperLayer(layer) || *** Mark does not want to include this layer type at this time 8 May 08 ***/
      layer.getLayerType() == layerTypePadTop || layer.getLayerType() == layerTypeSignalTop);

   return retval;
}

bool CPadStackInsert::isBottomCopperLayer(const LayerStruct& layer) const
{
   bool retval = (isAllCopperLayer(layer) || /* isOuterCopperLayer(layer) || *** Mark does not want to include this layer type at this time 8 May 08 ***/
      layer.getLayerType() == layerTypePadBottom || layer.getLayerType() == layerTypeSignalBottom);

   return retval;
}

bool CPadStackInsert::isSurfaceCopperLayer(const LayerStruct& layer,bool topSurfaceFlag) const
{
   bool retval = (topSurfaceFlag ? isTopCopperLayer(layer) : isBottomCopperLayer(layer));

   return retval;
}

bool CPadStackInsert::isStencilSourceLayer(const LayerStruct& layer,bool topSurfaceFlag) const
{
   bool retval = false;

   if (topSurfaceFlag && !m_topStencilSourceLayerName.IsEmpty())
   {
      retval = (m_topStencilSourceLayerName.Compare(layer.getName()) == 0);
   }
   else if (!topSurfaceFlag && !m_bottomStencilSourceLayerName.IsEmpty())
   {
      retval = (m_bottomStencilSourceLayerName.Compare(layer.getName()) == 0);
   }

   return retval;
}

bool CPadStackInsert::getSubPadIndexAttributeValue(int& subPadIndex,DataStruct& data)
{
   bool retval = false;

   Attrib* subPadIndexAttribute = NULL;
   data.attributes().Lookup(getSubPadIndexKeywordIndex(),subPadIndexAttribute);

   if (subPadIndexAttribute != NULL)
   {
      subPadIndex = subPadIndexAttribute->getIntValue();

      retval = true;
   }

   return retval;
}

void CPadStackInsert::setSubPadIndexAttributeValue(int  subPadIndex,DataStruct& data)
{
   m_camCadDatabase.getCamCadDoc().SetAttrib(&(data.getAttributesRef()),getSubPadIndexKeywordIndex(),subPadIndex);
}

bool CPadStackInsert::getStencilSourceSubPadIndexAttributeValue(int& subPadIndex,DataStruct& data)
{
   bool retval = false;

   Attrib* subPadIndexAttribute = NULL;
   data.attributes().Lookup(getStencilSourceSubPadIndexKeywordIndex(),subPadIndexAttribute);

   if (subPadIndexAttribute != NULL)
   {
      subPadIndex = subPadIndexAttribute->getIntValue();

      retval = true;
   }

   return retval;
}

void CPadStackInsert::setStencilSourceSubPadIndexAttributeValue(int subPadIndex,DataStruct& data)
{
   m_camCadDatabase.getCamCadDoc().SetAttrib(&(data.getAttributesRef()),getStencilSourceSubPadIndexKeywordIndex(),subPadIndex);
}

void CPadStackInsert::initializeSubPads(bool topFlag)
{
   BlockStruct* padStackGeometry = m_camCadDatabase.getBlock(m_padStackInsertData.getInsert()->getBlockNumber());

   CDataList unassignedSubPadList(false);
   CDataList stencilSourcePadList(false);

   for (CDataListIterator polyIterator(*padStackGeometry,dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* polyStruct = polyIterator.getNext();

      LayerStruct* layer = m_camCadDatabase.getLayerAt(polyStruct->getLayerIndex());
////#define dr594978
#ifdef dr594978
      // This fixed dts0100594978 but caused 502077, 608368, and 608994.
      // So this change is disabled but left for reference until alternate fix is done.
      if (!topFlag)
         layer = &layer->getMirroredLayer(); // If bottom then flip to bottom layer, assumes top-built geometries
#endif

      if (isSurfaceCopperLayer(*layer,topFlag))
      {
         unassignedSubPadList.AddTail(polyStruct);
      }
      else if (isStencilSourceLayer(*layer,topFlag))
      {
         stencilSourcePadList.AddTail(polyStruct);
      }
   }

   for (CDataListIterator insertIterator(*padStackGeometry,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* apertureData = insertIterator.getNext();
      BlockStruct* apertureBlock = m_camCadDatabase.getBlock(apertureData->getInsert()->getBlockNumber());

      if (apertureBlock->isAperture())
      {
         LayerStruct* layer = m_camCadDatabase.getLayerAt(apertureData->getLayerIndex());
#ifdef dr594978
      // This fixed dts0100594978 but caused 502077, 608368, and 608994.
      // So this change is disabled but left for reference until alternate fix is done.
         if (!topFlag)
            layer = &layer->getMirroredLayer(); // If bottom then flip to bottom layer, assumes top-built geometries
#endif
         if (isSurfaceCopperLayer(*layer,topFlag))
         {
            if (apertureBlock->isComplexAperture())
            {
               BlockStruct* complexApertureGeometry = m_camCadDatabase.getBlock(apertureBlock->getComplexApertureSubBlockNumber());

               for (CDataListIterator polyIterator(*complexApertureGeometry,dataTypePoly);polyIterator.hasNext();)
               {
                  DataStruct* polyStruct = polyIterator.getNext();

                  LayerStruct* layer = m_camCadDatabase.getLayerAt(polyStruct->getLayerIndex());
#ifdef dr594978
                  // This fixed dts0100594978 but caused 502077, 608368, and 608994.
                  // So this change is disabled but left for reference until alternate fix is done.
                  if (!topFlag)
                     layer = &layer->getMirroredLayer(); // If bottom then flip to bottom layer, assumes top-built geometries
#endif

                  if (layer != NULL && layer->isFloating())
                  {
                     unassignedSubPadList.AddTail(apertureData);
                     break;
                  }
               }
            }
            else
            {
               unassignedSubPadList.AddTail(apertureData);
            }
         }
         else if (isStencilSourceLayer(*layer,topFlag))
         {
            stencilSourcePadList.AddTail(apertureData);
         }
      }
   }

   int subPinCount = unassignedSubPadList.GetCount();
   CTypedPtrArrayContainer<DataStruct*> subPadAssignmentArray(subPinCount,false);
   subPadAssignmentArray.SetSize(subPinCount);

   for (POSITION pos = unassignedSubPadList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      DataStruct* data = unassignedSubPadList.GetNext(pos);
      int subPinIndex = -1;

      getSubPadIndexAttributeValue(subPinIndex,*data);

      if (subPinIndex >= 0 && subPinIndex < subPinCount)
      {
         if (subPinIndex < subPadAssignmentArray.GetSize() && subPadAssignmentArray.GetAt(subPinIndex) == NULL)
         {
            unassignedSubPadList.RemoveAt(oldPos);

            subPadAssignmentArray.SetAt(subPinIndex,data);
         }
      }
   }

   int subPinIndex = 0;

   for (POSITION pos = unassignedSubPadList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = unassignedSubPadList.GetNext(pos);

      while (subPadAssignmentArray.GetAt(subPinIndex) != NULL)
      {
         subPinIndex++;
      }

      setSubPadIndexAttributeValue(subPinIndex,*data);

      subPinIndex++;
   }

   //*rcf Original  if (subPinCount == 1 && stencilSourcePadList.GetCount() == 1)
   // We're getting stencilSourcePadList count > 1, so skip this.
   // As hack, if > 0 then add first one, see if we get anything at all. 
   // Surely it would be incomplete when count > 1, but let's see about getting one at all first.
   int sspl_count = stencilSourcePadList.GetCount(); //*rcf debug
   if (subPinCount > 0 && stencilSourcePadList.GetCount() > 0)
   {
      DataStruct* stencilSourcePad = stencilSourcePadList.GetHead();
      setStencilSourceSubPadIndexAttributeValue(0,*stencilSourcePad);
   }
}

int CPadStackInsert::getSubPadCount()
{
   int subPadCount = 0;
   int subPinIndex;

   DataStruct* subPadData = NULL;
   BlockStruct* padStackGeometry = m_camCadDatabase.getBlock(m_padStackInsertData.getInsert()->getBlockNumber());

   for (CDataListIterator insertIterator(*padStackGeometry,dataTypeInsert);insertIterator.hasNext();)
   {
      subPadData = insertIterator.getNext();

      if (getSubPadIndexAttributeValue(subPinIndex,*subPadData))
      {
         if (subPinIndex + 1 > subPadCount)
         {
            subPadCount = subPinIndex + 1;
         }
      }
   }

   return subPadCount;
}

DataStruct* CPadStackInsert::getSubPad(int subPadIndex,bool topFlag)
{
   BlockStruct* padStackGeometry = m_camCadDatabase.getBlock(m_padStackInsertData.getInsert()->getBlockNumber());
   CString subPadIndexString;

   for (CDataListIterator insertIterator(*padStackGeometry,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct *subPadData = insertIterator.getNext();
      LayerStruct* layer = m_camCadDatabase.getLayerAt(subPadData->getLayerIndex());
      if (layer != NULL)
      {
#ifdef dr594978
         // This fixed dts0100594978 but caused 502077, 608368, and 608994.
         // So this change is disabled but left for reference until alternate fix is done.
         if (!topFlag)
            layer = &layer->getMirroredLayer();
#endif

         if (layer != NULL && isSurfaceCopperLayer(*layer,topFlag))
         {
            int subPadDataSubPadIndex;

            if (getSubPadIndexAttributeValue(subPadDataSubPadIndex,*subPadData))
            {
               if (subPadDataSubPadIndex == subPadIndex)
               {
                  return subPadData;
               }
            }
         }
      }
   }

   return NULL;
}

DataStruct* CPadStackInsert::getStencilSourceSubPad(int subPadIndex,bool topFlag)
{
   DataStruct* subPadData = NULL;

   BlockStruct* padStackGeometry = m_camCadDatabase.getBlock(m_padStackInsertData.getInsert()->getBlockNumber());
   CString subPadIndexString;

   for (CDataListIterator insertIterator(*padStackGeometry,dataTypeInsert);insertIterator.hasNext();)
   {
      subPadData = insertIterator.getNext();
      LayerStruct* layer = m_camCadDatabase.getLayerAt(subPadData->getLayerIndex());
#ifdef dr594978
      // This wasn't part of fix but want to turn on/off with same ifdef for testing.
      // This fixed dts0100594978 but caused 502077, 608368, and 608994.
      // So this change is disabled but left for reference until alternate fix is done.
      if (!topFlag)
         layer = &layer->getMirroredLayer();
#endif
      if (isStencilSourceLayer(*layer,topFlag))
      {
         int subPadDataSubPadIndex;

         if (getStencilSourceSubPadIndexAttributeValue(subPadDataSubPadIndex,*subPadData))
         {
            if (subPadDataSubPadIndex == subPadIndex)
            {
               break;
            }
         }
      }

      subPadData = NULL;
   }

   return subPadData;
}

int CPadStackInsert::getSubPadIndexKeywordIndex()
{
   if (m_subPadIndexKeywordIndex == -1)
   {
      CNullWriteFormat errorLog;
      m_subPadIndexKeywordIndex = m_camCadDatabase.registerHiddenKeyWord("StencilSubPadIndex",0,valueTypeInteger,errorLog);
   }

   return m_subPadIndexKeywordIndex;
}

int CPadStackInsert::getStencilSourceSubPadIndexKeywordIndex()
{
   if (m_stencilSourceSubPadIndexKeywordIndex == -1)
   {
      CNullWriteFormat errorLog;
      m_stencilSourceSubPadIndexKeywordIndex = m_camCadDatabase.registerHiddenKeyWord("StencilSourceSubPadIndex",0,valueTypeInteger,errorLog);
   }

   return m_stencilSourceSubPadIndexKeywordIndex;
}

void CPadStackInsert::resetKeywordIndices()
{
   m_subPadIndexKeywordIndex              = -1;
   m_stencilSourceSubPadIndexKeywordIndex = -1;
}

bool CPadStackInsert::hasStructurallyBuriedCopper() const
{
   BlockStruct* padStackGeometry = getCamCadDatabase().getBlock(m_padStackInsertData.getInsert()->getBlockNumber());

   bool hasBuriedCopperFlag = hasStructurallyBuriedCopper(*padStackGeometry,m_padStackInsertData.getLayerIndex(),0);

   return hasBuriedCopperFlag;
}

bool CPadStackInsert::hasStructurallyBuriedCopper(const BlockStruct& geometry,int parentLayerIndex,int level) const
{
   // level 0 - insertData is the padstack
   // level 1 - insertData is an aperture or other insert in the padstack
   // level 2 - insertData is data inside a complex aperture or another insert inside the padstack
   bool buriedCopperFlag = false;

   if (geometry.isAperture())
   {
      if (geometry.isComplexAperture())
      {
         BlockStruct* complexApertureBlock = getCamCadDatabase().getBlock(geometry.getComplexApertureSubBlockNumber());
         int copperCount = 0;

         for (POSITION pos = complexApertureBlock->getHeadDataPosition();pos != NULL && !buriedCopperFlag;)
         {
            DataStruct* data = complexApertureBlock->getNextData(pos);
            int dataLayerIndex = getCamCadDoc().propagateLayer(parentLayerIndex,data->getLayerIndex());

            if (data->getDataType() == dataTypeInsert)
            {
               BlockStruct* block = getCamCadDatabase().getBlock(data->getInsert()->getBlockNumber());

               buriedCopperFlag = hasStructurallyBuriedCopper(*block,dataLayerIndex,level + 1);
            }
            else
            {
               LayerStruct* layer = getCamCadDatabase().getLayerAt(dataLayerIndex);

               if (layer != NULL)
               {
                  if (isTopCopperLayer(*layer) || isBottomCopperLayer(*layer))
                  {
                     copperCount++;
                     buriedCopperFlag = (copperCount > 1) || (level > 1);
                  }
               }
            }
         }
      }
      else if (level > 1)
      {
         LayerStruct* layer = getCamCadDatabase().getLayerAt(parentLayerIndex);

         if (layer != NULL)
         {
            if (isTopCopperLayer(*layer) || isBottomCopperLayer(*layer))
            {
               buriedCopperFlag = true;
            }
         }
      }
   }

   const CDataList* dataList = &(geometry.getDataList());

   for (POSITION pos = dataList->GetHeadPosition();pos != NULL && !buriedCopperFlag;)
   {
      DataStruct* data = dataList->GetNext(pos);
      int dataLayerIndex = getCamCadDoc().propagateLayer(parentLayerIndex,data->getLayerIndex());

      if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* block = getCamCadDatabase().getBlock(data->getInsert()->getBlockNumber());
         buriedCopperFlag = hasStructurallyBuriedCopper(*block,dataLayerIndex,level + 1);
      }
      else if (level > 1)
      {
         LayerStruct* layer = getCamCadDatabase().getLayerAt(dataLayerIndex);

         if (layer != NULL)
         {
            if (isTopCopperLayer(*layer) || isBottomCopperLayer(*layer))
            {
               buriedCopperFlag = true;
            }
         }
      }
   }

   return buriedCopperFlag;
}

BlockStruct& CPadStackInsert::generateRestructuredPadStack() const
{
   BlockStruct* padStackGeometry = getCamCadDatabase().getBlock(m_padStackInsertData.getInsert()->getBlockNumber());

   CString restructuredPadStackGeometryName;
   restructuredPadStackGeometryName.Format("sg-%s-%d",padStackGeometry->getName(),padStackGeometry->getBlockNumber());

   BlockStruct* restructuredPadStackGeometry = getCamCadDatabase().getBlock(restructuredPadStackGeometryName,padStackGeometry->getFileNumber());

   if (restructuredPadStackGeometry == NULL)
   {
      restructuredPadStackGeometry = getCamCadDatabase().getNewBlock(restructuredPadStackGeometryName,padStackGeometry->getBlockType(),padStackGeometry->getFileNumber());

      flattenDataList(restructuredPadStackGeometry->getDataList(),padStackGeometry->getDataList(),m_padStackInsertData.getLayerIndex(),0);
   }

   return *restructuredPadStackGeometry;
}

void CPadStackInsert::flattenDataList(CDataList& flattenedDataList,const CDataList& sourceDataList,int parentLayerIndex,int level) const
{
   CTMatrix matrix;

   for (POSITION pos = sourceDataList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = sourceDataList.GetNext(pos);
      int dataLayerIndex = getCamCadDoc().propagateLayer(parentLayerIndex,data->getLayerIndex());

      if (data->getDataType() == dataTypeInsert)
      {
         flattenInsert(flattenedDataList,*data,dataLayerIndex,matrix);
      }
      else
      {
         DataStruct* newData = newDataStruct(*data);
         flattenedDataList.AddTail(newData);
      }
   }
}

void CPadStackInsert::flattenInsert(CDataList& flatDataList,DataStruct& parentDataInsert,int parentLayer,const CTMatrix& parentMatrix) const
{
   InsertStruct* parentInsert = parentDataInsert.getInsert();
   BlockStruct* subBlock = getCamCadDatabase().getBlock(parentInsert->getBlockNumber());

   CTMatrix matrix(parentMatrix);
   matrix.translateCtm(parentInsert->getOriginX(),parentInsert->getOriginY());
   matrix.rotateRadiansCtm(parentInsert->getAngle());
   matrix.scaleCtm(parentInsert->getGraphicMirrored() ? -1 : 1,1);

   if (subBlock->isAperture())
   {
      bool keepUnflattenedApertureFlag = true;

      if (subBlock->isComplexAperture())
      {
         BlockStruct* complexApertureBlock = getCamCadDatabase().getBlock(subBlock->getComplexApertureSubBlockNumber());

         if (!complexApertureBlock->getDataList().isCompatibleWithRegularComplexAperture(getCamCadDataDoc()))
         {
            keepUnflattenedApertureFlag = false;

            matrix = subBlock->getApertureTMatrix() * matrix;

            DataStruct* insertData = newDataStruct(dataTypeInsert);
            insertData->getInsert()->setBlockNumber(complexApertureBlock->getBlockNumber());

            flattenInsert(flatDataList,*insertData,parentLayer,matrix);

            delete insertData;
         }
      }

      if (keepUnflattenedApertureFlag)
      {
         DataStruct* newData = newDataStruct(parentDataInsert);
         int apertureLayerIndex = getCamCadDoc().propagateLayer(parentLayer,newData->getLayerIndex());
         newData->setLayerIndex(apertureLayerIndex);

         CBasesVector basesVector;
         newData->getInsert()->setBasesVector(basesVector);
         newData->transform(matrix);

         flatDataList.AddTail(newData);
      }
   }
   else if (subBlock->isDrillHole())
   {
      DataStruct* newData = newDataStruct(parentDataInsert);
      int drillLayerIndex = getCamCadDoc().propagateLayer(parentLayer,newData->getLayerIndex());
      newData->setLayerIndex(drillLayerIndex);

      CBasesVector basesVector;
      newData->getInsert()->setBasesVector(basesVector);
      newData->transform(matrix);

      flatDataList.AddTail(newData);
   }
   else
   {
      for (POSITION pos = subBlock->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = subBlock->getDataList().GetNext(pos);
         int dataLayer = getCamCadDoc().propagateLayer(parentLayer,data->getLayerIndex());

         if (data->getDataType() == dataTypeInsert)
         {
            flattenInsert(flatDataList,*data,dataLayer,matrix);
         }
         else
         {
            DataStruct* newData = newDataStruct(*data);
            newData->transform(matrix);
            newData->setLayerIndex(dataLayer);

            flatDataList.AddTail(newData);
         }
      }
   }
}

//_____________________________________________________________________________
CStencilGenerator::CStencilGenerator(CStencilUi& stencilUi)
: m_stencilUi(stencilUi)
, m_viaComponentMap(nextPrime2n(2000),false)
, m_fiducialComponentMap(nextPrime2n(20),false)
, m_closetVesselMap(nextPrime2n(10000),false)
, m_stencilBlockBaseName("$SG_SSL")
{
   //m_camCadDatabase = new CCamCadDatabase(camCadDoc);
   //m_stencilValidationParameters = new CStencilValidationParameters(*m_camCadDatabase);

   m_fileStruct = NULL;
   m_initializedFlag = false;
   m_smdListRevLevel = 0;

   m_pcbVessel        = NULL;

   m_surfaceVessel[0] = NULL;
   m_surfaceVessel[1] = NULL;

   m_mountVessel[0]   = NULL;
   m_mountVessel[1]   = NULL;
   m_mountVessel[2]   = NULL;
   m_mountVessel[3]   = NULL;
   m_mountVessel[4]   = NULL;
   m_mountVessel[5]   = NULL;
   m_mountVessel[6]   = NULL;
   m_mountVessel[7]   = NULL;

   //m_componentDirectory             = NULL;

   m_pcbVesselGeometry               = NULL;
   m_surfaceVesselGeometry           = NULL;
   m_mountVesselGeometry             = NULL;
   m_componentGeometryVesselGeometry = NULL;
   m_padstackVesselGeometry          = NULL;
   m_subPadstackVesselGeometry       = NULL;
   m_subPinVesselGeometry            = NULL;
   m_componentVesselGeometry         = NULL;
   m_componentPinVesselGeometry      = NULL;
   m_componentSubPinVesselGeometry   = NULL;
   m_stencilGeneratorClosetGeometry  = NULL;

   m_stencilGeneratorCloset         = NULL;

   CDebugWriteFormat::setFilePath("c:\\StencilGeneratorDebug.txt");
}

CStencilGenerator::~CStencilGenerator()
{
   //delete m_camCadDatabase;
   //delete m_stencilValidationParameters;
   //delete m_componentDirectory;

   discardViaComponents();
   discardFiducialComponents();
}

FileStruct* CStencilGenerator::getStencilMicrocosmFile()
{
   FileStruct* stencilMicrocosmFile = getCamCadDatabase().getCamCadDoc().getFileList().FindByName(CStencilGeometryMicrocosm::getFileName());

   return stencilMicrocosmFile;
}

void CStencilGenerator::showStencilMicrocosmFile()
{
   FileStruct* stencilMicrocosmFile = getStencilMicrocosmFile();

   if (stencilMicrocosmFile != NULL)
   {
      stencilMicrocosmFile->setShow(true);
   }
}

void CStencilGenerator::hideStencilMicrocosmFile()
{
   FileStruct* stencilMicrocosmFile = getStencilMicrocosmFile();

   if (stencilMicrocosmFile != NULL)
   {
      stencilMicrocosmFile->setShow(false);
   }
}

void CStencilGenerator::removeTemporaryStencilEntities()
{
   if (m_fileStruct != NULL)
   {
      CCEtoODBDoc& camCadDoc = getCamCadDatabase().getCamCadDoc();
      int tempComponentKeywordIndex = camCadDoc.getStandardAttributeKeywordIndex(standardAttributeStencilGeneratorTempComponent);
      Attrib* attribute;

      BlockStruct* pcb = m_fileStruct->getBlock();

      for (POSITION pos = pcb->getHeadDataPosition();pos != NULL;)
      {
         POSITION oldPos  = pos;
         DataStruct* data = pcb->getNextData(pos);

         if (data->getDataType() == dataTypeInsert)
         {
            if (data->attributes().Lookup(tempComponentKeywordIndex,attribute))
            {
               //pcb->getDataList().RemoveDataFromList(&camCadDoc,data,oldPos);
               camCadDoc.removeDataFromDataList(pcb->getDataList(),data,oldPos);
            }
         }
      }
   }
}

void CStencilGenerator::convertToPinnedData(DataStruct& data)
{
   BlockStruct* geometry = getCamCadDatabase().getBlock(data.getInsert()->getBlockNumber());

   BlockStruct& pinnedGeometry = getCamCadDatabase().generatePinnedComponent(*geometry);

   data.getInsert()->setBlockNumber(pinnedGeometry.getBlockNumber());
}

void CStencilGenerator::restructureFiducialGeometryForInsert(DataStruct& data)
{
   BlockStruct* geometry = getCamCadDatabase().getBlock(data.getInsert()->getBlockNumber());

   CString geometryName = geometry->getName();

   if (geometryName.Find(getFiducialComponentGeometryNamePrefix()) != 0)
   {
      CString fiducialComponentGeometryName = getFiducialComponentGeometryNamePrefix() + geometryName;
      fiducialComponentGeometryName.AppendFormat("_%d$",geometry->getBlockNumber());

      //CString fiducialPadstackGeometryName = getFiducialPadstackGeometryNamePrefix() + fiducialGeometry->getName() + "$";
      //fiducialPadstackGeometryName.AppendFormat("_%d$",geometry->getBlockNumber());

      BlockStruct& fiducialGeometry =
         getCamCadDatabase().restructureFiducialGeometry(fiducialComponentGeometryName,*geometry,data.getLayerIndex());

      data.getInsert()->setBlockNumber(fiducialGeometry.getBlockNumber());
   }
}

DataStruct* CStencilGenerator::generatePadStack(const CPoly& sourcePoly, int layerIndex, CString basename)
{
   CPoly poly(sourcePoly);
   CExtent extent = poly.getExtent();
   CPoint2d origin = extent.getCenter();

   CTMatrix matrix;
   matrix.translate(-origin);

   poly.transform(matrix);

   int polySignature = poly.getSignature();

   CString padStackGeometryName;
   padStackGeometryName.Format("PS_%s%d-%08x", basename, layerIndex, polySignature);

   int fileNumber = getFileStruct()->getFileNumber();

   BlockStruct* padStackGeometry = getCamCadData().getBlockDirectory().getAt(padStackGeometryName,fileNumber,blockTypePadstack);

   if (padStackGeometry == NULL)
   {
      padStackGeometry = &(getCamCadData().getNewBlock(padStackGeometryName,fileNumber,blockTypePadstack));

      CString apertureGeometryName;
      apertureGeometryName.Format("AP_%s%08x", basename, polySignature);

      BlockStruct* apertureGeometry = getCamCadData().getBlockDirectory().getAt(apertureGeometryName,fileNumber,blockTypeUnknown);

      if (apertureGeometry == NULL)
      {
         CString complexApertureGeometryName = apertureGeometryName + "-Cmplx";

         BlockStruct* complexApertureGeometry = getCamCadData().getBlockDirectory().getAt(complexApertureGeometryName,fileNumber,blockTypeUnknown);

         if (complexApertureGeometry == NULL)
         {
            complexApertureGeometry = getCamCadDatabase().getNewBlock(complexApertureGeometryName,blockTypeUndefined,fileNumber);
            DataStruct* polyInsertData = getCamCadDatabase().addPolyStruct(complexApertureGeometry->getDataList(),getCamCadDatabase().getFloatingLayerIndex(),0,false,graphicClassNormal);

            CPoly* newPoly = new CPoly(poly);
            polyInsertData->getPolyList()->AddTail(newPoly);
         }

         int widthIndex = getCamCadDoc().Graph_Complex(apertureGeometryName,0,complexApertureGeometry,0.,0.,0.);
         apertureGeometry = getCamCadDoc().getWidthBlock(widthIndex);
      }


      DataStruct* padData = getCamCadData().insertBlock(*apertureGeometry,insertTypeUnknown,"",layerIndex);

      padStackGeometry->getDataList().AddTail(padData);
   }

   DataStruct* padStackData = getCamCadDatabase().insertBlock(padStackGeometry,insertTypeMechanicalPin,"",-1,origin.x,origin.y);

   return padStackData;
}

DataStruct* CStencilGenerator::generatePadStack(DataStruct& apertureData, int layerIndex, CString basename)
{
   CBasesVector basesVector = apertureData.getInsert()->getBasesVector();

   CString padStackGeometryName;
   padStackGeometryName.Format("PS_%s%d-%d", basename, layerIndex, apertureData.getInsert()->getBlockNumber());

   int fileNumber = getFileStruct()->getFileNumber();

   BlockStruct* padStackGeometry = getCamCadData().getBlockDirectory().getAt(padStackGeometryName,fileNumber,blockTypePadstack);

   if (padStackGeometry == NULL)
   {
      padStackGeometry = &(getCamCadData().getNewBlock(padStackGeometryName,fileNumber,blockTypePadstack));

      DataStruct* padData = newDataStruct(apertureData);

      CBasesVector zeroBasesVector;
      padData->getInsert()->setBasesVector(zeroBasesVector);

      padStackGeometry->getDataList().AddTail(padData);
   }

   DataStruct* padStackData = getCamCadData().insertBlock(*padStackGeometry,insertTypeMechanicalPin,"",-1);

   padStackData->getInsert()->setBasesVector(basesVector);

   return padStackData;
}

void CStencilGenerator::convertComponentStencilSourceLayerPolysToPadstacks(BlockStruct& componentGeometry)
{
   CDataList padStackDataList(true);
   const double tolerance = .0001;
   int zeroWidthIndex = getCamCadDatabase().getZeroWidthIndex();
   int pinIndex = 0;

   CString topLyrName( this->getStencilGenerationParameters().getTopStencilSourceLayerName() );
   CString botLyrName( this->getStencilGenerationParameters().getBottomStencilSourceLayerName() );
   int topLyrIndx = this->getCamCadDatabase().getLayerIndex(topLyrName);
   int botLyrIndx = this->getCamCadDatabase().getLayerIndex(botLyrName);

   for (CModifiableDataListIterator polyIterator(componentGeometry,dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* polyStruct = polyIterator.getNext();

      //GraphicClassTag grClass = polyStruct->getGraphicClass();
      //bool isCopperGrClass = ( (grClass == graphicClassNormal) || (grClass == graphicClassEtch) );

      LayerStruct* layer = getCamCadData().getLayer(polyStruct->getLayerIndex());

      bool topLyrMatch = (topLyrIndx != -1) ? (layer->getLayerIndex() == topLyrIndx) : false;
      bool botLyrMatch = (botLyrIndx != -1) ? (layer->getLayerIndex() == botLyrIndx) : false;
      bool isStencil = topLyrMatch || botLyrMatch;

      /*
         LayerTypeTag layType = (layer != NULL) ? layer->getLayerType() : layerTypeUnknown;
      bool isCopperLayerType = (
         layType == layerTypePadTop       ||
         layType == layerTypePadBottom    ||
         layType == layerTypePadOuter     ||
         layType == layerTypePadAll       ||
         layType == layerTypeSignalTop    ||
         layType == layerTypeSignalBottom ||
         layType == layerTypeSignalOuter  ||
         layType == layerTypeSignalAll    );
         */

      if (isStencil)//isCopperLayerType && isCopperGrClass)
      {
         pinIndex++;

         CPolyList* polyList = polyStruct->getPolyList();

         for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList->GetNext(polyPos);
            double width = getCamCadDataDoc().getWidth(poly->getWidthIndex());

            if (! poly->isClosed())
            {
               poly->closePolyWithCoincidentEndPoints();
               poly->convertToOutline(width,tolerance,zeroWidthIndex);
            }

            DataStruct* padStack = generatePadStack(*poly, layer->getLayerIndex(), "StencilPin");

            CString pinName;
            pinName.Format("Stencil%d", pinIndex);
            padStack->getInsert()->setRefname(pinName);

            padStackDataList.AddTail(padStack);
         }

         polyIterator.deleteCurrent();
      }
   }

   componentGeometry.getDataList().takeData(padStackDataList);
}

void CStencilGenerator::convertComponentCopperToPadstacks(BlockStruct& componentGeometry)
{
   CDataList padStackDataList(true);
   const double tolerance = .0001;
   int zeroWidthIndex = getCamCadDatabase().getZeroWidthIndex();
   int pinIndex = 0;

   for (CModifiableDataListIterator polyIterator(componentGeometry,dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* polyStruct = polyIterator.getNext();

      GraphicClassTag grClass = polyStruct->getGraphicClass();
      bool isCopperGrClass = ( (grClass == graphicClassNormal) || (grClass == graphicClassEtch) );

      LayerStruct* layer = getCamCadData().getLayer(polyStruct->getLayerIndex());
      LayerTypeTag layType = (layer != NULL) ? layer->getLayerType() : layerTypeUnknown;
      bool isCopperLayerType = (
         layType == layerTypePadTop       ||
         layType == layerTypePadBottom    ||
         layType == layerTypePadOuter     ||
         layType == layerTypePadAll       ||
         layType == layerTypeSignalTop    ||
         layType == layerTypeSignalBottom ||
         layType == layerTypeSignalOuter  ||
         layType == layerTypeSignalAll    );

      if (isCopperLayerType && isCopperGrClass)
      {
         pinIndex++;

         CPolyList* polyList = polyStruct->getPolyList();

         for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList->GetNext(polyPos);
            double width = getCamCadDataDoc().getWidth(poly->getWidthIndex());

            if (! poly->isClosed())
            {
               poly->closePolyWithCoincidentEndPoints();
               poly->convertToOutline(width,tolerance,zeroWidthIndex);
            }

            DataStruct* padStack = generatePadStack(*poly, layer->getLayerIndex(), "CopperPin");

            CString pinName;
            pinName.Format("Copper%d",pinIndex);
            padStack->getInsert()->setRefname(pinName);

            padStackDataList.AddTail(padStack);
         }

         polyIterator.deleteCurrent();
      }
   }

   for (CModifiableDataListIterator insertIterator(componentGeometry,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* apertureData = insertIterator.getNext();
      BlockStruct* apertureBlock = getCamCadDatabase().getBlock(apertureData->getInsert()->getBlockNumber());

      if (apertureBlock->isAperture())
      {
         LayerStruct* layer = getCamCadData().getLayer(apertureData->getLayerIndex());
         LayerTypeTag layType = (layer != NULL) ? layer->getLayerType() : layerTypeUnknown;
         bool isCopperLayerType = (
            layType == layerTypePadTop       ||
            layType == layerTypePadBottom    ||
            layType == layerTypePadOuter     ||
            layType == layerTypePadAll       ||
            layType == layerTypeSignalTop    ||
            layType == layerTypeSignalBottom ||
            layType == layerTypeSignalOuter  ||
            layType == layerTypeSignalAll    );

         if (isCopperLayerType)
         {
            pinIndex++;

            DataStruct* padStack = generatePadStack(*apertureData, layer->getLayerIndex(), "CopperPin");

            CString pinName;
            pinName.Format("Copper%d",pinIndex);
            padStack->getInsert()->setRefname(pinName);

            padStackDataList.AddTail(padStack);

            insertIterator.deleteCurrent();
         }
      }
   }

   componentGeometry.getDataList().takeData(padStackDataList);
}

void CStencilGenerator::initializeStencilEntities(int fileNumber,COperationProgress* progress)
{
   m_viaComponentMap.empty();
   m_stencilViolations.empty();
   m_stencilPins.empty();
   CStencilPin::resetNextId();

   m_fileStruct = getCamCadDatabase().getFile(fileNumber);
   //int smdKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_SMDSHAPE);

   if (m_fileStruct != NULL)
   {
      removeTemporaryStencilEntities();
      migrateOldRuleData();
      //normalizeApertureOffsetsAndRotations();

      BlockStruct* pcb = m_fileStruct->getBlock();

      double topStencilThickness    = 0.;
      double bottomStencilThickness = 0.;
      double topMaxFeature          = 0.;
      double bottomMaxFeature       = 0.;
      double topWebWidth            = 0.;
      double bottomWebWidth         = 0.;
      CString stringValue;

      // default the pcb rule to stencilAttributeCopperSourceLayer
      CStencilRuleAttributes pcbStenclRuleAttributes(getCamCadDatabase(),*(getPcbVessel().getDefinedAttributes()));

      if (! pcbStenclRuleAttributes.hasNonInheritedRule())
      {
         CStencilRule stencilRule;
         stencilRule.setRule(stencilRuleCopperSourceLayer);

         pcbStenclRuleAttributes.setRule(stencilRule);
      }

      if (getCamCadDatabase().getAttributeStringValue(stringValue,getAttributeMap(pcb),
                              CStencilPin::getKeywordIndex(stencilAttributeTopThickness)))
      {
         topStencilThickness = atof(stringValue);
      }

      if (getCamCadDatabase().getAttributeStringValue(stringValue,getAttributeMap(pcb),
                              CStencilPin::getKeywordIndex(stencilAttributeBottomThickness)))
      {
         bottomStencilThickness = atof(stringValue);
      }

      CString topStencilSrcLyrName( this->getStencilGenerationParameters().getTopStencilSourceLayerName() );
      CString botStencilSrcLyrName( this->getStencilGenerationParameters().getBottomStencilSourceLayerName() );

      if (progress != NULL)
      {
         progress->updateStatus("Initializing Stencil Hole Data",pcb->getDataList().GetCount());
      }

      int debugLoopSize = pcb->getDataList().GetCount();

      BlockStruct& padstackVesselGeometry    = getPadstackVesselGeometry();
      BlockStruct& subPadstackVesselGeometry = getSubPadstackVesselGeometry();
      BlockStruct& pinVesselGeometry         = getSubPinVesselGeometry();
      CDataList syntheticComponentDataList(true);

      // iterate thru components and vias
      for (CDataListIterator dataListIterator(*pcb,dataTypeInsert);dataListIterator.hasNext();)
      {
         if (progress != NULL)
         {
            progress->updateProgress(dataListIterator.getIndex());
         }

         DataStruct* data = dataListIterator.getNext();
         DataStruct* component = NULL;
         bool topFlag = data->getInsert()->getPlacedTop();

         if (data->getInsert()->getInsertType() == insertTypeVia)
         {
            DataStruct* via = data;

            component = getViaComponent(via,syntheticComponentDataList);
         }
         else if (data->getInsert()->getInsertType() == insertTypeFiducial)
         {
            component = data;
            restructureFiducialGeometryForInsert(*component);
         }
         else if (data->getInsert()->getInsertType() == insertTypePcbComponent)
         {
            component = data;

            // knv 20071212 - restructure component geometries here (construct mechanical pins from component copper).
         }

         if (component != NULL)
         {
            InsertStruct* componentInsert = component->getInsert();

            BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentInsert->getBlockNumber());
            CString refDes = componentInsert->getRefname();
            CDataList& componentDataList = componentGeometry->getDataList();

            CString componentGeometryName = componentGeometry->getName();

            // convert copper polys in component to padstacks
            convertComponentCopperToPadstacks(*componentGeometry);

#ifdef SUPPORT_NONCOPPER_PADS
            // THIS WORKS... However much more needs to be done to make it useful.
            // The issue is COPPER. Most of SG will ignore an item that has no copper.
            // These items have no copper. So it was coerced as far as getting them
            // to appear in the geometry tree, but still could not get stencil holes generated.
            // We might want to pursue this again. Or maybe there is some other reason to convert
            // those things to Mech Pins. This will do that, and it works.

            // convert stencil source layer polys in component to padstacks
            convertComponentStencilSourceLayerPolysToPadstacks(*componentGeometry);
#endif

            // iterate thru pins
            for (CDataListIterator dataIterator(*componentGeometry,CInsertTypeMask(insertTypePin,insertTypeMechanicalPin));dataIterator.hasNext();)
            {
               DataStruct* pin = dataIterator.getNext();
               InsertStruct* pinInsert = pin->getInsert();

               CString pinName = pinInsert->getRefname();

#ifdef ORIGINAL
               CPadStackInsert* padStackInsert = new CPadStackInsert(getCamCadDatabase(),*pin);
#else
               CPadStackInsert* padStackInsert = new CPadStackInsert(getCamCadDatabase(),*pin, topStencilSrcLyrName, botStencilSrcLyrName);
#endif

               if (padStackInsert->hasStructurallyBuriedCopper())
               {
                  BlockStruct& restructurePadStackGeometry = padStackInsert->generateRestructuredPadStack();

                  pin->getInsert()->setBlockNumber(restructurePadStackGeometry.getBlockNumber());

                  delete padStackInsert;

                  padStackInsert = new CPadStackInsert(getCamCadDatabase(),*pin);
               }

               if (padStackInsert->getSubPadCount() > 0)
               {
                  CStencilPin& stencilPin = m_stencilPins.addStencilPin(*this, *component, *pin, *pcb);

                  for (int subPadIndex = 0;subPadIndex < padStackInsert->getSubPadCount();subPadIndex++)
                  {
                     CStencilSubPin& subPin = stencilPin.addSubPin();

                     subPin.getStencilHole(true ).setStencilThickness(topStencilThickness);
                     subPin.getStencilHole(false).setStencilThickness(bottomStencilThickness);
                  }
               }
               else if (padStackInsert->getStencilSourceSubPad(0, true))
               {
                  CStencilPin& stencilPin = m_stencilPins.addStencilPin(*this, *component, *pin, *pcb);
                  CStencilSubPin& subPin = stencilPin.addSubPin();

                  subPin.getStencilHole(true ).setStencilThickness(topStencilThickness);
                  subPin.getStencilHole(false).setStencilThickness(bottomStencilThickness);
               }

               delete padStackInsert;
            }

#ifdef KILLS_SG
            // This is an attempt to add stencil source layer polys as stencil pins.
            // This part works, but downstream doesn't. There is dependency and assumption in other
            // code that there is a padstack insert and padstack geometry block to work with.
            // We can pass a DataStruct that is a poly here, that is fine, types match, etc.
            // But later uses of that DataStruct assume it is an Insert, and that does not work.
            // We might want to make this work someday, so the foundation of it is left here for awhile.
            //*rcf ------------------------------------
            // If stencil rule is StencilSourceLayer then add all comp geom level StencilSourceLayer polys
            //if (stencilRule.getRule() == stencilRuleStencilSourceLayer ||
            //   stencilRule.getRule() == stencilRulePassthru)
            {
               CString topLyrName( this->getStencilGenerationParameters().getTopStencilSourceLayerName() );
               CString botLyrName( this->getStencilGenerationParameters().getBottomStencilSourceLayerName() );
               int topLyrIndx = this->getCamCadDatabase().getLayerIndex(topLyrName);
               int botLyrIndx = this->getCamCadDatabase().getLayerIndex(botLyrName);

               for (CDataListIterator polyIterator(*componentGeometry,dataTypePoly);polyIterator.hasNext();)
               {
                  DataStruct* polydata = polyIterator.getNext();
                  bool topLyrMatch = (topLyrIndx != -1) ? (polydata->getLayerIndex() == topLyrIndx) : false;
                  bool botLyrMatch = (botLyrIndx != -1) ? (polydata->getLayerIndex() == botLyrIndx) : false;

                  if (topLyrMatch || botLyrMatch)
                  {
                     //DataStruct *newData = geometryMicrocosm.getCamCadData().getNewDataStruct(*polydata);
                     //sourceSilkScreenDataList.AddTail(newData);
                     CStencilPin& stencilPin = m_stencilPins.addStencilPin(*this, *component, *polydata, *pcb);
                     CStencilSubPin& subPin = stencilPin.addSubPin();
                     subPin.getStencilHole(true ).setStencilThickness(topStencilThickness);
                     subPin.getStencilHole(false).setStencilThickness(bottomStencilThickness);
                  }
               }
            }
            //*rcf ------------------------------------
#endif

            //componentGeometry->getDataList().takeData(vesselDataList);
         }
      }

      pcb->getDataList().takeData(syntheticComponentDataList);
   }

   // update units for modifiers from older .cc files
   PageUnitsTag pageUnits = getCamCadDatabase().getPageUnits();

   for (CStencilEntityIterator stencilEntityIterator(*this);stencilEntityIterator.hasNext();)
   {
      CStencilEntityRule stencilEntityRule;
      CAttributes* attributes;

      stencilEntityIterator.getNext(stencilEntityRule,attributes);

      stencilEntityRule.updateModifiers(pageUnits);

      CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*attributes);
      stencilRuleAttributes.setRule(stencilEntityRule);
   }

   //CDebugWriteFormat::setFilePath("C:\\GerberEducatorDebug.txt");

   m_initializedFlag = true;

   identifySMDEntities(fileNumber, progress);
}

void CStencilGenerator::identifySMDEntities(int fileNumber,COperationProgress* progress)
{
   m_smdGeometries.RemoveAll();

   if (!this->m_initializedFlag)
      return;

   CMapStringToInt processedBlocks;

   m_smdListRevLevel++;  // increment list revision level, users of list use this to tell if they are in sync

   m_fileStruct = getCamCadDatabase().getFile(fileNumber);

   if (m_fileStruct != NULL)
   {
      BlockStruct* pcb = m_fileStruct->getBlock();

      // Prep regular expression matcher outside of loop, if it is used it
      // will not change during loop.
      CString mtAttName   = getStencilGenerationParameters().getMountTechAttribName();
      CString mtAttSMDVal = getStencilGenerationParameters().getMountTechAttribSMDValue();
      CRegularExpression smdValRegExp;
      if (getStencilGenerationParameters().getUseMountTechAttrib() && !mtAttSMDVal.IsEmpty())
      {
         smdValRegExp.setRegularExpression(CRegularExpression::fixRegularExpression(mtAttSMDVal),true);
         smdValRegExp.setCaseSensitive(false);
      }

      //BlockStruct& padstackVesselGeometry = getPadstackVesselGeometry();
      CDataList syntheticComponentDataList(true);

       if (progress != NULL)
      {
         CDataListIterator counter(*pcb,dataTypeInsert);
         int count;
         for (count = 0; counter.hasNext(); count++) counter.getNext();

         progress->updateStatus("Identifying SMD Components", count);
      }

       // iterate thru components and vias
      for (CDataListIterator dataListIterator(*pcb,dataTypeInsert);dataListIterator.hasNext();)
      {
         if (progress != NULL)
         {
            progress->updateProgress(dataListIterator.getIndex());
         }

         DataStruct* data = dataListIterator.getNext();
         DataStruct* component = NULL;

         if (data->getInsert()->getInsertType() == insertTypeVia)
         {
            DataStruct* via = data;
            component = getViaComponent(via,syntheticComponentDataList);
         }
         else if (data->getInsert()->getInsertType() == insertTypeFiducial)
         {
            component = data;
            restructureFiducialGeometryForInsert(*component);
         }
         else if (data->getInsert()->getInsertType() == insertTypePcbComponent)
         {
            component = data;
         }

         if (component != NULL)
         {
            InsertStruct* componentInsert = component->getInsert();

            // This identifySMDEntities() func is built from an extaction from initializeStencilEntities(), which is
            // where the smd geometry list used to be built. That func has this block of code, which means it will
            // not even consider comps that do not have attributes. It isn't really necessary to protect anything
            // here, but to keep this func in sync with that func, we do the same thing.
            if (component->getAttributesRef() == NULL)
            {
               continue;
            }

            BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentInsert->getBlockNumber());

            // A given block need be processed only once. This process, especially finding existing
            // stencil pads, can be lengthy for large ccz files, so do as little as possible.
            // Note that the DMS-identification of blocks in Stencil Gen is block-name based.
            // Block names are not requried to be unique in camcad, not even required to be set.
            // This is obviously a weak point in Stencil Gen.

            int blockNum;
            if (!processedBlocks.Lookup(componentGeometry->getName(), blockNum))
            {
               // It is not processed yet, but is about to be, so go ahead and add to processed list now
               processedBlocks.SetAt(componentGeometry->getName(), componentGeometry->getBlockNumber());

               if (getStencilGenerationParameters().getUseMountTechAttrib())
               {
                  // Use a mount technology attribute to determine component SMD/THRU.
                  // If either are empty then there is nothing to do.
                  if (!mtAttName.IsEmpty() && !mtAttSMDVal.IsEmpty() && component->getAttributesRef() != NULL)
                  {
                     Attrib *a = NULL;
                     if ((a = is_attvalue(&getCamCadDatabase().getCamCadDoc(), component->getAttributesRef(), mtAttName, 0)) != NULL)
                     {
                        if (a->getValueType() == VT_STRING)
                        {
                           CString attVal = getCamCadDatabase().getCamCadDoc().getAttributeStringValue(a);
                           if (smdValRegExp.matches(attVal))
                           {
                              // is SMD
                              m_smdGeometries.SetAt(componentGeometry->getName(),"");
                           }
                        }
                     }
                  }
               }
               else
               {
                  // Original Recipe - Use presence of pads on surfaces to determine component SMD/THRU.
                  // Scan all the pins, if any one pin is SMD then consider part SMD.
                  // Once an SMD pin is found, no need to look at rest of pins.
                  for (POSITION pos = m_stencilPins.GetHeadPosition();pos != NULL;)
                  {
                     CStencilPin *stencilPin = m_stencilPins.GetNext(pos);

                     if (stencilPin->getGeometry() == componentGeometry)
                     {
                        if (stencilPin->getSubPins().hasCopperPad(true) != stencilPin->getSubPins().hasCopperPad(false))
                        {
                           m_smdGeometries.SetAt(componentGeometry->getName(),"");
                           break;
                        }

                        //DataStruct* topCopperPad    = stencilPin->getStencilHole(true ).getCopperPad();
                        //DataStruct* bottomCopperPad = stencilPin->getStencilHole(false).getCopperPad();

                        //if ((topCopperPad == NULL) != (bottomCopperPad == NULL))
                        //{
                        //   m_smdGeometries.SetAt(componentGeometry->getName(),"");
                        //   pos = NULL; // We only needed to find one, we can quit looking now
                        //}
                     }
                  }
               }
            }
         }
      }
   }

   processedBlocks.RemoveAll();

   if (progress != NULL)
   {
      progress->updateStatus("", 100.0);
   }
}

void CStencilGenerator::propagateStencilThickness(double thickness, bool topSideFlag)
{
   // each stencil hole has the stencil thickness, propagate a new value throughout
   // existing stencil holes

   if (m_initializedFlag)
   {
      for (POSITION pos = m_stencilPins.GetHeadPosition();pos != NULL;)
      {
         CStencilPin* stencilPin = m_stencilPins.GetNext(pos);

         for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
         {
            stencilPin->getStencilHole(subPinIndex,topSideFlag).setStencilThickness(thickness);
         }
      }
   }
}


CString CStencilGenerator::getViaComponentGeometryNamePrefix()
{
   return "$stencilGeneratorVia-";
}

CString CStencilGenerator::getFiducialComponentGeometryNamePrefix()
{
   return "$stencilGeneratorFiducial-";
}

DataStruct* CStencilGenerator::getViaComponent(DataStruct* via,CDataList& viaComponentDataList)
{
   InsertStruct* viaInsert = via->getInsert();
   DataStruct* viaComponent;

   if (!m_viaComponentMap.Lookup(via->getEntityNumber(),viaComponent))
   {
      viaComponent = NULL;
   }

   if (viaComponent == NULL)
   {
      BlockStruct* viaGeometry = getCamCadDatabase().getBlock(via->getInsert()->getBlockNumber());

      CString viaComponentGeometryName = getViaComponentGeometryNamePrefix() + viaGeometry->getName() + "$";

      BlockStruct* viaComponentGeometry = getCamCadDatabase().getDefinedBlock(viaComponentGeometryName,
                                             blockTypeUnknown,m_fileStruct->getFileNumber());

      if (viaComponentGeometry->getDataList().GetCount() == 0)
      {
         DataStruct* viaPin = getCamCadDatabase().insertBlock(viaGeometry,insertTypePin,"1",0);
         viaComponentGeometry->getDataList().AddTail(viaPin);
      }

      CString viaRefDes;
      viaRefDes.Format("SgVia%d",m_viaComponentMap.GetCount()+1);

      viaComponent = getCamCadDatabase().insertBlock(viaComponentGeometry,insertTypePcbComponent,viaRefDes,0,
         viaInsert->getOriginX(),viaInsert->getOriginY(),viaInsert->getAngleRadians());

      int keywordIndex = getCamCadDatabase().getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeStencilGeneratorTempComponent);
      getCamCadDatabase().addAttribute(viaComponent->attributes(),keywordIndex,0);

      //BlockStruct* pcb = m_fileStruct->getBlock();
      //pcb->getDataList().AddTail(viaComponent);
      viaComponentDataList.AddTail(viaComponent);

      m_viaComponentMap.SetAt(via->getEntityNumber(),viaComponent);
   }

   return viaComponent;
}

void CStencilGenerator::discardViaComponents()
{
   CMapEntityToDataStruct componentMap(nextPrime2n(m_viaComponentMap.GetSize()),false);
   int viaEntityNumber;
   DataStruct* viaComponent;

   for (POSITION pos = m_viaComponentMap.GetStartPosition();pos != NULL;)
   {
      m_viaComponentMap.GetNextAssoc(pos,viaEntityNumber,viaComponent);

      componentMap.SetAt(viaComponent->getEntityNumber(),viaComponent);
   }

   if (m_fileStruct != NULL)
   {
      BlockStruct* pcb = m_fileStruct->getBlock();
      CDataList& pcbDataList = pcb->getDataList();

      for (POSITION pos = pcbDataList.GetHeadPosition();pos != NULL;)
      {
         POSITION prevPos = pos;

         DataStruct* data = pcbDataList.GetNext(pos);

         if (componentMap.Lookup(data->getEntityNumber(),viaComponent))
         {
            pcbDataList.RemoveAt(prevPos);
            delete data;
         }
      }
   }
}

DataStruct* CStencilGenerator::getFiducialComponent(DataStruct* fiducial,CDataList& fiducialComponentDataList)
{
   InsertStruct* fiducialInsert = fiducial->getInsert();
   DataStruct* fiducialComponent;

   if (!m_fiducialComponentMap.Lookup(fiducial->getEntityNumber(),fiducialComponent))
   {
      fiducialComponent = NULL;
   }

   if (fiducialComponent == NULL)
   {
      restructureFiducialGeometryForInsert(*fiducial);
      BlockStruct* fiducialComponentGeometry = getCamCadDatabase().getBlock(fiducial->getInsert()->getBlockNumber());

      ///
      //BlockStruct* fiducialGeometry = getCamCadDatabase().getBlock(fiducial->getInsert()->getBlockNumber());

      //CString fiducialComponentGeometryName = getFiducialComponentGeometryNamePrefix() + fiducialGeometry->getName() + "$";

      //BlockStruct* fiducialComponentGeometry = getCamCadDatabase().getDefinedBlock(fiducialComponentGeometryName,
      //                                       blockTypeUnknown,m_fileStruct->getFileNumber());

      //if (fiducialComponentGeometry->getDataList().GetCount() == 0)
      //{
      //   // look for pin1
      //   DataStruct* fiducialPin = NULL;
      //   int pinCount = 0;

      //   for (CDataListIterator pinIterator(*fiducialGeometry,insertTypePin);pinIterator.hasNext();)
      //   {
      //      DataStruct* pinData = pinIterator.getNext();

      //      fiducialPin = newDataStruct(*pinData);
      //      fiducialComponentGeometry->getDataList().AddTail(fiducialPin);

      //      pinCount++;
      //   }

      //   if (pinCount == 0)
      //   {
      //      fiducialPin = getCamCadDatabase().insertBlock(fiducialGeometry,insertTypePin,"1",0);
      //      fiducialComponentGeometry->getDataList().AddTail(fiducialPin);
      //   }
      //}

      CString fiducialRefDes;
      fiducialRefDes.Format("SgFiducial%d",m_fiducialComponentMap.GetCount()+1);

      fiducialComponent = getCamCadDatabase().insertBlock(fiducialComponentGeometry,insertTypePcbComponent,fiducialRefDes,-1,
                              fiducialInsert->getOriginX(),fiducialInsert->getOriginY(),fiducialInsert->getAngleRadians(),
                              fiducialInsert->getGraphicMirrored(),fiducialInsert->getScale());

      int keywordIndex = getCamCadDatabase().getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeStencilGeneratorTempComponent);
      getCamCadDatabase().addAttribute(fiducialComponent->attributes(),keywordIndex,0);

      //BlockStruct* pcb = m_fileStruct->getBlock();
      //pcb->getDataList().AddTail(fiducialComponent);
      fiducialComponentDataList.AddTail(fiducialComponent);

      m_fiducialComponentMap.SetAt(fiducial->getEntityNumber(),fiducialComponent);
   }

   return fiducialComponent;
}

void CStencilGenerator::discardFiducialComponents()
{
   CMapEntityToDataStruct componentMap(nextPrime2n(m_fiducialComponentMap.GetSize()),false);
   int fiducialEntityNumber;
   DataStruct* fiducialComponent;

   for (POSITION pos = m_fiducialComponentMap.GetStartPosition();pos != NULL;)
   {
      m_fiducialComponentMap.GetNextAssoc(pos,fiducialEntityNumber,fiducialComponent);

      componentMap.SetAt(fiducialComponent->getEntityNumber(),fiducialComponent);
   }

   if (m_fileStruct != NULL)
   {
      BlockStruct* pcb = m_fileStruct->getBlock();
      CDataList& pcbDataList = pcb->getDataList();

      for (POSITION pos = pcbDataList.GetHeadPosition();pos != NULL;)
      {
         POSITION prevPos = pos;

         DataStruct* data = pcbDataList.GetNext(pos);

         if (componentMap.Lookup(data->getEntityNumber(),fiducialComponent))
         {
            pcbDataList.RemoveAt(prevPos);
            delete data;
         }
      }
   }
}

//CComponentDirectory& CStencilGenerator::getComponentDirectory()
//{
//   if (m_componentDirectory == NULL)
//   {
//      m_componentDirectory = new CComponentDirectory(getCamCadDatabase(),m_fileStruct);
//   }
//
//   return  *m_componentDirectory;
//}

int CStencilGenerator::getMountVesselIndex(bool topFlag,bool smdFlag,bool viaFlag,bool fiducialFlag) const
{
   //   topFlag  viaFlag  fiducialFlag  smdFlag   index
   //      0        0           0          0        0
   //      0        0           0          1        1
   //      0        0           1          0        2
   //      0        0           1          1        2
   //      0        1           0          0        3
   //      0        1           0          1        3
   //      0        1           1          0        3
   //      0        1           1          1        3
   //      1        0           0          0        4
   //      1        0           0          1        5
   //      1        0           1          0        6
   //      1        0           1          1        6
   //      1        1           0          0        7
   //      1        1           0          1        7
   //      1        1           1          0        7
   //      1        1           1          1        7

   int mountVesselIndex = (topFlag ? 4 : 0) + (viaFlag ? 3 : (fiducialFlag ? 2 : (smdFlag ? 1 : 0)));

   return mountVesselIndex;
}

CString CStencilGenerator::getMountVesselReference(bool topFlag,bool smdFlag,bool viaFlag,bool fiducialFlag,bool oldDelimiterFlag) const
{
   CString delimiter(oldDelimiterFlag ? "-" : ".");
   CString mountVesselReference = (topFlag ? QTop : QBottom) + delimiter + (viaFlag ? QVia : (fiducialFlag ? QFiducial : (smdFlag ? QSmd : QTh)));

   return mountVesselReference;
}

DataStruct& CStencilGenerator::getPcbVessel()
{
   if (m_pcbVessel == NULL)
   {
      CString vesselRef("pcb");

      m_pcbVessel = &(getClosetVessel(vesselRef,getPcbVesselGeometry()));
   }

   return *m_pcbVessel;
}

DataStruct& CStencilGenerator::getSurfaceVessel(bool topFlag)
{
   DataStruct*& surfaceVessel = (topFlag ? m_surfaceVessel[0] : m_surfaceVessel[1]);

   if (surfaceVessel == NULL)
   {
      CString vesselRef(topFlag ? "top" : "bottom");

      BlockStruct& stencilGeneratorClosetGeometry = getStencilGeneratorClosetGeometry();

      surfaceVessel = &(getClosetVessel(vesselRef,getSurfaceVesselGeometry()));
   }

   return *surfaceVessel;
}

DataStruct& CStencilGenerator::getMountVessel(bool topFlag,bool smdFlag,bool viaFlag,bool fiducialFlag)
{
   int mountVesselIndex = getMountVesselIndex(topFlag,smdFlag,viaFlag,fiducialFlag);
   DataStruct* mountVessel = m_mountVessel[mountVesselIndex];

   if (mountVessel == NULL)
   {
      CString vesselRef = getMountVesselReference(topFlag,smdFlag,viaFlag,fiducialFlag);

      mountVessel = &(getClosetVessel(vesselRef,getMountVesselGeometry()));

      m_mountVessel[mountVesselIndex] = mountVessel;
   }

   return *mountVessel;
}

DataStruct& CStencilGenerator::getComponentGeometryVessel(bool topFlag,const CString& componentGeometryName)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(componentGeometryName);

   return getComponentClosetVessel(componentGeometryName,vesselRef,getComponentGeometryVesselGeometry());
}

DataStruct& CStencilGenerator::getPadstackVessel(bool topFlag,const CString& componentGeometryName,const CString& firstPinName)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(componentGeometryName) + 
                                                   getRefDelimiter() + getDesignatorString(firstPinName);

   return getComponentClosetVessel(componentGeometryName,vesselRef,getPadstackVesselGeometry());
}

DataStruct& CStencilGenerator::getSubPadstackVessel(bool topFlag,const CString& componentGeometryName,const CString& firstPinName,const CString& subPinName)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(componentGeometryName) + 
                                                   getRefDelimiter() + getDesignatorString(firstPinName) + 
                                                   getRefDelimiter() + getDesignatorString(subPinName);

   return getComponentClosetVessel(componentGeometryName,vesselRef,getSubPadstackVesselGeometry());
}

DataStruct& CStencilGenerator::getSubPinVessel(bool topFlag,const CString& componentGeometryName,const CString& pinName,const CString& subPinName)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(componentGeometryName) + 
                                                   getRefDelimiter() + getDesignatorString(pinName) + 
                                                   getRefDelimiter() + getDesignatorString(subPinName);

   return getComponentClosetVessel(componentGeometryName,vesselRef,getSubPinVesselGeometry());
}

DataStruct& CStencilGenerator::getComponentVessel(bool topFlag,const CString& refDes)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(refDes);

   return getClosetVessel(vesselRef,getComponentVesselGeometry());
}

DataStruct& CStencilGenerator::getComponentPinVessel(bool topFlag,const CString& refDes,const CString& pinName)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(refDes) + 
                                                   getRefDelimiter() + getDesignatorString(pinName);

   return getClosetVessel(vesselRef,getComponentPinVesselGeometry());
}

DataStruct& CStencilGenerator::getComponentSubPinVessel(bool topFlag,const CString& refDes,const CString& pinName,const CString& subPinName)
{
   CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(refDes) + 
                                                   getRefDelimiter() + getDesignatorString(pinName) + 
                                                   getRefDelimiter() + getDesignatorString(subPinName);

   return getClosetVessel(vesselRef,getComponentSubPinVesselGeometry());
}

BlockStruct& CStencilGenerator::getPcbVesselGeometry()
{
   if (m_pcbVesselGeometry == NULL)
   {
      m_pcbVesselGeometry = getCamCadDatabase().getDefinedBlock("$PcbVessel$",blockTypeUnknown);
   }

   return *m_pcbVesselGeometry;
}

BlockStruct& CStencilGenerator::getSurfaceVesselGeometry()
{
   if (m_surfaceVesselGeometry == NULL)
   {
      m_surfaceVesselGeometry = getCamCadDatabase().getDefinedBlock("$SurfaceVessel$",blockTypeUnknown);
   }

   return *m_surfaceVesselGeometry;
}

BlockStruct& CStencilGenerator::getMountVesselGeometry()
{
   if (m_mountVesselGeometry == NULL)
   {
      m_mountVesselGeometry = getCamCadDatabase().getDefinedBlock("$MountVessel$",blockTypeUnknown);
   }

   return *m_mountVesselGeometry;
}

BlockStruct& CStencilGenerator::getComponentGeometryVesselGeometry()
{
   if (m_componentGeometryVesselGeometry == NULL)
   {
      m_componentGeometryVesselGeometry = getCamCadDatabase().getDefinedBlock("$ComponentGeometryVessel$",blockTypeUnknown);
   }

   return *m_componentGeometryVesselGeometry;
}

BlockStruct& CStencilGenerator::getPadstackVesselGeometry()
{
   if (m_padstackVesselGeometry == NULL)
   {
      m_padstackVesselGeometry = getCamCadDatabase().getDefinedBlock("$PadstackVessel$",blockTypeUnknown);
   }

   return *m_padstackVesselGeometry;
}

BlockStruct& CStencilGenerator::getSubPadstackVesselGeometry()
{
   if (m_subPadstackVesselGeometry == NULL)
   {
      m_subPadstackVesselGeometry = getCamCadDatabase().getDefinedBlock("$SubPadstackVessel$",blockTypeUnknown);
   }

   return *m_subPadstackVesselGeometry;
}

BlockStruct& CStencilGenerator::getSubPinVesselGeometry()
{
   if (m_subPinVesselGeometry == NULL)
   {
      m_subPinVesselGeometry = getCamCadDatabase().getDefinedBlock("$SubPinVessel$",blockTypeUnknown);
   }

   return *m_subPinVesselGeometry;
}

BlockStruct& CStencilGenerator::getComponentVesselGeometry()
{
   if (m_componentVesselGeometry == NULL)
   {
      m_componentVesselGeometry = getCamCadDatabase().getDefinedBlock("$ComponentVessel$",blockTypeUnknown);
   }

   return *m_componentVesselGeometry;
}

BlockStruct& CStencilGenerator::getComponentPinVesselGeometry()
{
   if (m_componentPinVesselGeometry == NULL)
   {
      m_componentPinVesselGeometry = getCamCadDatabase().getDefinedBlock("$ComponentPinVessel$",blockTypeUnknown);
   }

   return *m_componentPinVesselGeometry;
}

BlockStruct& CStencilGenerator::getComponentSubPinVesselGeometry()
{
   if (m_componentSubPinVesselGeometry == NULL)
   {
      m_componentSubPinVesselGeometry = getCamCadDatabase().getDefinedBlock("$ComponentSubPinVessel$",blockTypeUnknown);
   }

   return *m_componentSubPinVesselGeometry;
}

BlockStruct& CStencilGenerator::getStencilGeneratorClosetGeometry()
{
   if (m_stencilGeneratorClosetGeometry == NULL)
   {
      m_stencilGeneratorClosetGeometry = getCamCadDatabase().getDefinedBlock("$stencilGeneratorCloset$",blockTypeUnknown,getFileStruct()->getFileNumber());

      for (CDataListIterator insertIterator(*m_stencilGeneratorClosetGeometry,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* vessel = insertIterator.getNext();
         CString vesselRef = vessel->getInsert()->getRefname();

         m_closetVesselMap.SetAt(vesselRef,vessel);
      }
   }

   return *m_stencilGeneratorClosetGeometry;
}

void CStencilGenerator::migrateOldRuleData()
{
   // if the closet doesn't yet exist, move old data to the closet
   if (findStencilGeneratorCloset() == NULL)
   {
      DataStruct& stencilGeneratorCloset = getStencilGeneratorCloset();

      BlockStruct& stencilGeneratorClosetGeometry = getStencilGeneratorClosetGeometry();
      BlockStruct& pcbBlock = *(getFileStruct()->getBlock());
      LayerStruct* topStencilLayer    = getCamCadDatabase().getLayer(ccLayerStencilTop   );
      LayerStruct* bottomStencilLayer = getCamCadDatabase().getLayer(ccLayerStencilBottom);

      // -- -- -- old pcb rule -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
      {
         CStencilRuleAttributes oldStencilRuleAttributes(getCamCadDatabase(),pcbBlock.getDefinedAttributes());
         CStencilRule stencilRule = oldStencilRuleAttributes.getRule();
         oldStencilRuleAttributes.empty();

         if (stencilRule.getRule() != stencilRuleUndefined)
         {
            DataStruct& pcbVessel = getPcbVessel();
            CStencilRuleAttributes newStencilRuleAttributes(getCamCadDatabase(),pcbVessel.attributes());
            newStencilRuleAttributes.setRule(stencilRule);
         }
      }

      // -- -- -- old surface rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
      for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
      {
         bool topFlag = (surfaceIndex == 0);

         LayerStruct* stencilLayer = (topFlag ? topStencilLayer : bottomStencilLayer);

         CStencilRuleAttributes oldStencilRuleAttributes(getCamCadDatabase(),stencilLayer->getDefinedAttributes());
         CStencilRule stencilRule = oldStencilRuleAttributes.getRule();
         oldStencilRuleAttributes.empty();

         if (stencilRule.getRule() != stencilRuleUndefined)
         {
            DataStruct& surfaceVessel = getSurfaceVessel(topFlag);
            CStencilRuleAttributes newStencilRuleAttributes(getCamCadDatabase(),surfaceVessel.attributes());
            newStencilRuleAttributes.setRule(stencilRule);
         }
      }

      // -- -- -- old mount rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
      CDataList oldMountDataDataList(true);
      CTypedMapStringToPtrContainer<DataStruct*> rev2MountVesselMap(nextPrime2n(16),false);
      CTypedPtrArrayContainer<DataStruct*> rev1MountVesselArray(8,false);

      rev1MountVesselArray.SetSize(8);

      for (CModifiableDataListIterator dataIterator(pcbBlock);dataIterator.hasNext();)
      {
         DataStruct* data = dataIterator.getNext();

         if (data->getDataType() == dataTypeText)
         {
            bool topFlag = (data->getLayerIndex() == topStencilLayer->getLayerIndex());

            if (topFlag || data->getLayerIndex() == bottomStencilLayer->getLayerIndex())
            {
               CString text = data->getText()->getText();
               bool smdFlag = (text == QSmd);

               if (smdFlag || text == QTh)
               {
                  CString vesselReference = getMountVesselReference(topFlag,smdFlag,false,false,true);
                  int mountVesselIndex = getMountVesselIndex(topFlag,smdFlag,false,false);
                  dataIterator.removeCurrent();

                  oldMountDataDataList.AddTail(data);
                  rev1MountVesselArray.SetAtGrow(mountVesselIndex,data);
               }
            }
         }
         else if (data->getDataType() == dataTypeInsert)
         {
            if (data->getInsert()->getBlockNumber() == getMountVesselGeometry().getBlockNumber())
            {
               dataIterator.removeCurrent();

               oldMountDataDataList.AddTail(data);
               CString mountRef = data->getInsert()->getRefname();

               if (mountRef.Left(4) == "top-")
               {
                  mountRef = "top." + mountRef.Mid(4);
               }
               else if (mountRef.Left(7) == "bottom-")
               {
                  mountRef = "bottom." + mountRef.Mid(7);
               }

               rev2MountVesselMap.SetAt(mountRef,data);
            }
         }
      }

      // copy old mount rules to new location
      for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
      {
         bool topFlag = (surfaceIndex == 0);

         for (int typeIndex = 0;typeIndex < 4;typeIndex++)
         {
            bool smdFlag      = (typeIndex == 0);
            bool viaFlag      = (typeIndex == 1);
            bool fiducialFlag = (typeIndex == 2);

            CStencilRule mountRuleRev2;

            CString mountVesselReference = getMountVesselReference(topFlag,smdFlag,viaFlag,fiducialFlag);
            int mountVesselIndex = getMountVesselIndex(topFlag,smdFlag,false,false);

            DataStruct& mountVessel = getMountVessel(topFlag,smdFlag,viaFlag,fiducialFlag);
            DataStruct* oldMountVessel = NULL;

            if (! rev2MountVesselMap.Lookup(mountVesselReference,oldMountVessel))
            {
               oldMountVessel = rev1MountVesselArray.GetAt(mountVesselIndex);
            }

            if (oldMountVessel != NULL)
            {
               CStencilRuleAttributes oldStencilRuleAttributes(getCamCadDatabase(),oldMountVessel->getDefinedAttributes());
               CStencilRule stencilRule = oldStencilRuleAttributes.getRule();

               if (stencilRule.getRule() != stencilRuleUndefined)
               {
                  CStencilRuleAttributes newStencilRuleAttributes(getCamCadDatabase(),mountVessel.attributes());
                  newStencilRuleAttributes.setRule(stencilRule);
               }
            }
         }
      }

      oldMountDataDataList.empty();

      // -- -- -- old rules on component inserts and component geometries -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
      CMapStringToString processedComponentGeometryForOldPadstackRules;
      processedComponentGeometryForOldPadstackRules.InitHashTable(nextPrime2n(300));

      for (CDataListIterator dataListIterator(pcbBlock,insertTypePcbComponent);dataListIterator.hasNext();)
      {
         DataStruct* component = dataListIterator.getNext();
         InsertStruct* componentInsert = component->getInsert();
         bool topFlag = componentInsert->getPlacedTop();
         CString refDes = componentInsert->getRefname();
         BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentInsert->getBlockNumber());
         CString componentGeometryName = componentGeometry->getName();

         // -- -- -- old componentGeometry rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         CStencilRule oldComponentGeometryRule;

         if (componentGeometry->getAttributesRef() != NULL)
         {
            // remove old structure component geometry rule
            CStencilRuleAttributes oldComponentGeometryRuleAttributes(getCamCadDatabase(),&(componentGeometry->attributes()));
            oldComponentGeometryRule = oldComponentGeometryRuleAttributes.getRule();
            oldComponentGeometryRuleAttributes.empty();
         }

         if (oldComponentGeometryRule.getRule() != stencilRuleUndefined)
         {
            CStencilRuleAttributes newComponentGeometryRuleAttributes(getCamCadDatabase(),getComponentGeometryVessel(topFlag,componentGeometryName).attributes());
            newComponentGeometryRuleAttributes.setRule(oldComponentGeometryRule);
         }

         // -- -- -- old component rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         CStencilRule oldComponentRule;

         if (component->getAttributesRef() != NULL)
         {
            // remove old structure component rule
            CStencilRuleAttributes oldComponentRuleAttributes(getCamCadDatabase(),&(component->attributes()));
            oldComponentRule = oldComponentRuleAttributes.getRule();
            oldComponentRuleAttributes.empty();
         }

         if (oldComponentRule.getRule() != stencilRuleUndefined)
         {
            CStencilRuleAttributes newComponentRuleAttributes(getCamCadDatabase(),getComponentVessel(topFlag,refDes).attributes());
            newComponentRuleAttributes.setRule(oldComponentRule);
         }

         // -- -- -- old padstack rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         CTypedMapStringToPtrContainer<DataStruct*> oldPadstackVesselMap(nextPrime2n(200),false);
         CDataList oldPadstackVesselDataList(true);
         CString temp;

         if (! processedComponentGeometryForOldPadstackRules.Lookup(componentGeometryName,temp))
         {
            processedComponentGeometryForOldPadstackRules.SetAt(componentGeometryName,componentGeometryName);

            // create map entries for padstackVessels, subPadstackVessels, and pinVessels
            for (CModifiableDataListIterator padstackVesselIterator(*componentGeometry,dataTypeInsert);padstackVesselIterator.hasNext();)
            {
               DataStruct* padstackVesselData = padstackVesselIterator.getNext();
               int blockNumber = padstackVesselData->getInsert()->getBlockNumber();
               CString vesselRef = padstackVesselData->getInsert()->getRefname();

               if (blockNumber == this->getPadstackVesselGeometry().getBlockNumber())
               {
                  padstackVesselIterator.removeCurrent();

                  oldPadstackVesselMap.SetAt(vesselRef,padstackVesselData);

                  oldPadstackVesselDataList.AddTail(padstackVesselData);
               }
            }
         }

         for (CDataListIterator dataIterator(*componentGeometry,CInsertTypeMask(insertTypePin,insertTypeMechanicalPin));dataIterator.hasNext();)
         {
            DataStruct* pin = dataIterator.getNext();
            InsertStruct* pinInsert = pin->getInsert();
            CString pinName = pinInsert->getRefname();
            BlockStruct* padstackGeometry = getCamCadDatabase().getBlock(pinInsert->getBlockNumber());
            CString padstackGeometryName = padstackGeometry->getName();

            CString firstPinName = getFirstPinName(*componentGeometry,*pin);

            // -- -- -- old padstack rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            CStencilRule oldPadstackRule;
            DataStruct* padstackVesselData;

            if (oldPadstackVesselMap.Lookup(padstackGeometryName,padstackVesselData))
            {
               CStencilRuleAttributes oldPadstackRuleAttributes(getCamCadDatabase(),&(padstackVesselData->attributes()));
               oldPadstackRule = oldPadstackRuleAttributes.getRule();

               oldPadstackVesselMap.RemoveKey(padstackGeometryName);
            }

            if (oldPadstackRule.getRule() != stencilRuleUndefined)
            {
               CStencilRuleAttributes newPadstackRuleAttributes(getCamCadDatabase(),getPadstackVessel(topFlag,componentGeometryName,firstPinName).attributes());
               newPadstackRuleAttributes.setRule(oldPadstackRule);
            }

            // -- -- -- old geometry pin rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            CStencilRule oldGeometryPinRule;

            if (pin->getAttributesRef() != NULL)
            {
               // remove old structure component geometry rule
               CStencilRuleAttributes oldGeometryPinRuleAttributes(getCamCadDatabase(),&(pin->attributes()));
               oldGeometryPinRule = oldGeometryPinRuleAttributes.getRule();
               oldGeometryPinRuleAttributes.empty();
            }

            if (oldGeometryPinRule.getRule() != stencilRuleUndefined)
            {
               CStencilRuleAttributes newGeometryPinRuleAttributes(getCamCadDatabase(),getSubPinVessel(topFlag,componentGeometryName,pinName,"0").attributes());
               newGeometryPinRuleAttributes.setRule(oldGeometryPinRule);
            }
         }
      }

      // -- -- -- old rules on comp pins -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

      // remove old structure componentPinRules and move to closet
      BlockStruct* componentPinsVesselGeometry = getCamCadDatabase().getBlock("$ComponentPinsVessel$",getFileStruct()->getFileNumber());

      if (componentPinsVesselGeometry != NULL)
      {
         int componentPinVesselGeometryNumber = getComponentPinVesselGeometry().getBlockNumber();

         for (CModifiableDataListIterator vesselIterator(*componentPinsVesselGeometry,dataTypeInsert);vesselIterator.hasNext();)
         {
            DataStruct* componentPinVessel = vesselIterator.getNext();

            if (componentPinVessel->getInsert()->getBlockNumber() == componentPinVesselGeometryNumber)
            {
               // -- -- -- old componentPin rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
               CStencilRule oldComponentPinRule;

               if (componentPinVessel->getAttributesRef() != NULL)
               {
                  // remove old structure component pin rule
                  CStencilRuleAttributes oldComponentPinRuleAttributes(getCamCadDatabase(),&(componentPinVessel->attributes()));
                  oldComponentPinRule = oldComponentPinRuleAttributes.getRule();
                  oldComponentPinRuleAttributes.empty();
               }

               // copy rule to new structure
               if (oldComponentPinRule.getRule() != stencilRuleUndefined)
               {
                  CSupString pinRef = componentPinVessel->getInsert()->getRefname();
                  CStringArray refDesParams;
                  int refDesParamCount = pinRef.Parse(refDesParams,".");

                  if (refDesParamCount == 2)
                  {
                     CString refDes  = refDesParams.GetAt(0);
                     CString pinName = refDesParams.GetAt(1);


                     CStencilRuleAttributes newTopComponentPinRuleAttributes(getCamCadDatabase(),getComponentPinVessel(true,refDes,pinName).attributes());
                     newTopComponentPinRuleAttributes.setRule(oldComponentPinRule);

                     CStencilRuleAttributes newBottomComponentPinRuleAttributes(getCamCadDatabase(),getComponentPinVessel(false,refDes,pinName).attributes());
                     newBottomComponentPinRuleAttributes.setRule(oldComponentPinRule);
                  }

                  vesselIterator.deleteCurrent();
               }
            }
         }
      }

      //CNetList& netList =  getFileStruct()->getNetList();

      //for (POSITION netPos = netList.GetHeadPosition();netPos != NULL;)
      //{
      //   NetStruct* netStruct = netList.GetNext(netPos);

      //   for (POSITION pinPos = netStruct->getHeadCompPinPosition();pinPos != NULL;)
      //   {
      //      CompPinStruct* compPinStruct = netStruct->getNextCompPin(pinPos);
      //      CString refDes  = compPinStruct->getRefDes();
      //      CString pinName = compPinStruct->getPinName();

      //      if (refDes.CompareNoCase("C8") == 0 && pinName == "1")
      //      {
     //         int iii = 3;
      //      }

      //      // -- -- -- old componentPin rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
      //      CStencilRule oldComponentPinRule;

      //      if (compPinStruct->getAttributesRef() != NULL)
      //      {
      //         // remove old structure component pin rule
      //         CStencilRuleAttributes oldComponentPinRuleAttributes(getCamCadDatabase(),&(compPinStruct->attributes()));
      //         oldComponentPinRule = oldComponentPinRuleAttributes.getRule();
      //         oldComponentPinRuleAttributes.empty();
      //      }

      //      // copy rule to new structure
      //      if (oldComponentPinRule.getRule() != stencilRuleUndefined)
      //      {
      //         CStencilRuleAttributes newTopComponentPinRuleAttributes(getCamCadDatabase(),getComponentPinVessel(true,refDes,pinName).attributes());
      //         newTopComponentPinRuleAttributes.setRule(oldComponentPinRule);

      //         CStencilRuleAttributes newBottomComponentPinRuleAttributes(getCamCadDatabase(),getComponentPinVessel(false,refDes,pinName).attributes());
      //         newBottomComponentPinRuleAttributes.setRule(oldComponentPinRule);
      //      }
      //   }
      //}

      // -- -- -- old componentSubPin rules -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

         // there were no old componentSubPin rules
   }
}

void CStencilGenerator::normalizeApertureOffsetsAndRotations()
{
   CTypedMapIntToPtrContainer<CTMatrix*> apertureTransformMap;

   for (int blockIndex = 0;blockIndex < getCamCadDoc().getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = getCamCadDoc().getBlockAt(blockIndex);

      if (block != NULL && block->isAperture())
      {
         if (block->getXoffset() != 0. || block->getYoffset() != 0. || block->getRotationRadians() != 0.)
         {
            CTMatrix* matrix = new CTMatrix();

            *matrix = block->getApertureTMatrix();
            matrix->invert();

            apertureTransformMap.SetAt(blockIndex,matrix);

            block->setXoffset(0.);
            block->setYoffset(0.);
            block->setRotationRadians(0.);
         }
      }
   }

   CTMatrix* matrix;

   for (int blockIndex = 0;blockIndex < getCamCadDoc().getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = getCamCadDoc().getBlockAt(blockIndex);

      if (block != NULL)
      {
         for (CDataListIterator insertIterator(*block,dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();
            int subBlockNumber = data->getInsert()->getBlockNumber();

            if (apertureTransformMap.Lookup(subBlockNumber,matrix))
            {
               data->transform(*matrix);
            }
         }
      }
   }
}

DataStruct* CStencilGenerator::findStencilGeneratorCloset()
{
   if (m_stencilGeneratorCloset == NULL)
   {
      int stencilGeneratorClosetGeometryNumber = getStencilGeneratorClosetGeometry().getBlockNumber();

      for (CDataListIterator insertIterator(*(getFileStruct()->getBlock()),dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();

         if (data->getInsert()->getBlockNumber() == stencilGeneratorClosetGeometryNumber)
         {
            m_stencilGeneratorCloset = data;

            break;
         }
      }
   }

   return m_stencilGeneratorCloset;
}

DataStruct& CStencilGenerator::getStencilGeneratorCloset()
{
   if (m_stencilGeneratorCloset == NULL)
   {
      m_stencilGeneratorCloset = findStencilGeneratorCloset();

      // if the closet doesn't exist, create it
      if (m_stencilGeneratorCloset == NULL)
      {
         m_stencilGeneratorCloset = getCamCadDatabase().insertBlock(&(getStencilGeneratorClosetGeometry()),insertTypeUnknown,"",-1);

         getFileStruct()->getBlock()->getDataList().AddTail(m_stencilGeneratorCloset);
      }
   }

   return *m_stencilGeneratorCloset;
}

DataStruct& CStencilGenerator::getClosetVessel(const CString& vesselRef,BlockStruct& vesselGeometry)
{
   DataStruct* vessel;

   if (! m_closetVesselMap.Lookup(vesselRef,vessel))
   {
      vessel = getCamCadDatabase().insertBlock(&vesselGeometry,insertTypeUnknown,vesselRef,-1);

      getStencilGeneratorClosetGeometry().getDataList().AddTail(vessel);

      m_closetVesselMap.SetAt(vesselRef,vessel);
   }

   return *vessel;
}

bool CStencilGenerator::isSmdGeometry(const CString& geometryName)
{
   CString value;

   bool retval = (m_smdGeometries.Lookup(geometryName,value) != 0);

   return retval;
}

bool CStencilGenerator::isViaGeometry(const CString& geometryName)
{
   bool retval = (geometryName.Find(getViaComponentGeometryNamePrefix()) == 0);

   return retval;
}

bool CStencilGenerator::isFiducialGeometry(const CString& geometryName)
{
   bool retval = (geometryName.Find(getFiducialComponentGeometryNamePrefix()) == 0);

   return retval;
}

void CStencilGenerator::renderStencils(CStencilGenerationParameters& stencilGenerationParameters,
   CStencilValidationParameters& stencilValidationParameters,
   CWriteFormat& logFile,CWriteFormat* reportFile)
{
   CWaitCursor waitCursor;

   CStencilPin::registerStencilKeywords(getCamCadDatabase(),logFile);
   CStencilRuleAttributes::registerStencilKeywords(getCamCadDatabase(),logFile);
   //getGroups();

   LayerStruct* topStencilLayer    = getCamCadDatabase().getLayer(ccLayerStencilTop   );
   LayerStruct* bottomStencilLayer = getCamCadDatabase().getLayer(ccLayerStencilBottom);

   // case 376263 - Mark says always set up the stencil layers as mirrors
   getCamCadDatabase().getCamCadDoc().Graph_Level_Mirror(topStencilLayer->getName(), bottomStencilLayer->getName(), NULL);

   unsigned int dataTypeMask = dataTypeMaskAll & ~dataTypeMaskText;

   getCamCadDatabase().deleteEntitiesOnLayer(*topStencilLayer   ,dataTypeMask);
   getCamCadDatabase().deleteEntitiesOnLayer(*bottomStencilLayer,dataTypeMask);

   // knv - 20071220
   // Sometimes geometries left over from previous stencil generator runs and used on the stencil layers
   // may have been constructed incorrectly by previous versions of CamCad.
   // These bad geometries will be reused for this rendering.
   // It would be nice to automatically delete these unused geometries after deleting the contents of the stencil layers.
   // Unfortunately, this messes up the CCamCadDatabase object's block directory.
   // For now, the workaround is for the user (outside of stencil generator) to manually delete the contents of the stencil layers
   // and then "Purge Unused Widths and Geometries"
   //getCamCadDoc().purgeUnusedWidthsAndBlocks(false);

   BlockStruct* pcb = m_fileStruct->getBlock();
   CString stringValue;
   bool topPasteInHoleFlag    = false;
   bool bottomPasteInHoleFlag = false;
   bool topPasteInViaFlag    = false;
   bool bottomPasteInViaFlag = false;
   int topStencilHoleCount = 0;
   int bottomStencilHoleCount = 0;

   if (getCamCadDatabase().getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeTopPasteInHole)))
   {
      topPasteInHoleFlag = (atoi(stringValue) != 0);
   }

   if (getCamCadDatabase().getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInHole)))
   {
      bottomPasteInHoleFlag = (atoi(stringValue) != 0);
   }

   if (getCamCadDatabase().getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeTopPasteInVia)))
   {
      topPasteInViaFlag = (atoi(stringValue) != 0);
   }

   if (getCamCadDatabase().getAttributeStringValue(stringValue,getAttributeMap(pcb),
          CStencilPin::getKeywordIndex(stencilAttributeBottomPasteInVia)))
   {
      bottomPasteInViaFlag = (atoi(stringValue) != 0);
   }

   // debug
   //CCamCadLayerMask layerMask(getCamCadDatabase());
   //layerMask.add(getCamCadDatabase().getDefinedLayer("calcStencilHole() - copperPad")->getLayerIndex());
   //layerMask.add(getCamCadDatabase().getDefinedLayer("calcStencilHole() - pin")->getLayerIndex());
   //getCamCadDatabase().deleteEntitiesOnLayers(layerMask);

   COperationProgress progress(m_stencilPins.GetCount());
   progress.updateStatus("Generating stencils");

   for (POSITION stencilPinPos = m_stencilPins.GetHeadPosition();stencilPinPos != NULL;)
   {
      progress.incrementProgress();

      CStencilPin* stencilPin = m_stencilPins.GetNext(stencilPinPos);
      stencilPin->clearStencilHoles();

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         CStencilHole& topStencilHole    = stencilPin->getStencilHole(subPinIndex,true );
         CStencilHole& bottomStencilHole = stencilPin->getStencilHole(subPinIndex,false);
         CString pinRef = stencilPin->getPinRef();

         DataStruct* topStencilHoleTemplate    =
            topStencilHole.calcStencilHole(m_stencilGroups,m_fileStruct,
               stencilGenerationParameters.getTopMaxFeatureSize(),
               stencilGenerationParameters.getTopWebSize());

         DataStruct* bottomStencilHoleTemplate =
            bottomStencilHole.calcStencilHole(m_stencilGroups,m_fileStruct,
               stencilGenerationParameters.getBottomMaxFeatureSize(),
               stencilGenerationParameters.getBottomWebSize());

         bool thFlag = (topStencilHoleTemplate != NULL && bottomStencilHoleTemplate != NULL);
         bool viaFlag = stencilPin->isVia();

         topStencilHole.setStencilHoleInstance(NULL);
         bottomStencilHole.setStencilHoleInstance(NULL);

         bool topHoleDisabledFlag    = ((viaFlag && !topPasteInViaFlag   ) || (thFlag && !topPasteInHoleFlag    && !viaFlag));
         bool bottomHoleDisabledFlag = ((viaFlag && !bottomPasteInViaFlag) || (thFlag && !bottomPasteInHoleFlag && !viaFlag));

         if ((topStencilHoleTemplate != NULL) && !topHoleDisabledFlag)
         {
            topStencilHoleTemplate->setLayerIndex(topStencilLayer->getLayerIndex());
            DataStruct* stencilHoleInstance = newDataStruct(*topStencilHoleTemplate);
            topStencilHole.setStencilHoleInstance(stencilHoleInstance);
            topStencilHole.addAttributes(stencilHoleInstance/*,logFile*/);
            pcb->getDataList().AddTail(stencilHoleInstance);

            topStencilHoleCount++;
         }

         if ((bottomStencilHoleTemplate != NULL) && !bottomHoleDisabledFlag)
         {
            bottomStencilHoleTemplate->setLayerIndex(bottomStencilLayer->getLayerIndex());
            DataStruct* stencilHoleInstance = newDataStruct(*bottomStencilHoleTemplate);
            bottomStencilHole.setStencilHoleInstance(stencilHoleInstance);
            bottomStencilHole.addAttributes(stencilHoleInstance/*,logFile*/);
            pcb->getDataList().AddTail(stencilHoleInstance);

            bottomStencilHoleCount++;
         }
      }
   }

   for (POSITION stencilPinPos = m_stencilPins.GetHeadPosition();stencilPinPos != NULL;)
   {
      progress.incrementProgress();

      CStencilPin* stencilPin = m_stencilPins.GetNext(stencilPinPos);

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         CStencilHole& topStencilHole    = stencilPin->getStencilHole(subPinIndex,true );
         CStencilHole& bottomStencilHole = stencilPin->getStencilHole(subPinIndex,false);
      }
   }

   PromotePanelFidsToStencilLayers();

   int topSslHoleCnt = 0;
   int botSslHoleCnt = 0;
   PromoteStencilSourceLayerToStencilLayers(topSslHoleCnt, botSslHoleCnt);
   topStencilHoleCount    += topSslHoleCnt;
   bottomStencilHoleCount += botSslHoleCnt;

   BringLayerToFront(&(getCamCadDatabase().getCamCadDoc()),bottomStencilLayer->getLayerIndex());
   BringLayerToFront(&(getCamCadDatabase().getCamCadDoc()),   topStencilLayer->getLayerIndex());

   logFile.writef("Generated %d stencil holes for top stencil.\n"   ,topStencilHoleCount);
   logFile.writef("Generated %d stencil holes for bottom stencil.\n",bottomStencilHoleCount);

   bool enableValidationChecks = false;  // disable until checks are fixed - 20051115.1636 knv

   if (enableValidationChecks)
   {
      performStencilValidationChecks(stencilValidationParameters,stencilGenerationParameters);
   }

   if (reportFile != NULL)
   {
      generatePinReports(*reportFile,&progress);
   }
}

//------------------------------------------------------------------------
// These two HasStencilGraphic functions play back and forth.
// You could start with either, but typically one would start with
// a call to the DataStruct version, if that DataStruct is directly
// on a stenil layer, yer done, if not and it is an insert then the
// BlockStruct HasStencilGraphic is called to process the inserted
// block. A BlockStruct is processed by walking down the datalist
// to see if any are on a stencil layer, and that is determined by
// calling the DataStruct HasStencilGraphic.
// Right now these are not picky about a complete match of all
// top graphics to top stencil and bottom graphics to bottom stencil,
// this might have to be expanded to be more picky.
// For now we justs presume if it has any at all then it is complete
// and ready to use as-is.

bool CStencilGenerator::HasStencilGraphic(BlockStruct *block)
{
   if (block != NULL)
   {
      POSITION datapos = block->getHeadDataPosition();
      while (datapos != NULL)
      {
         DataStruct *data = block->getNextData(datapos);
         if (HasStencilGraphic(data))
            return true;
      }
   }

   return false;
}

bool CStencilGenerator::HasStencilGraphic(DataStruct *data)
{
   if (data != NULL)
   {
      LayerStruct *lp = this->getCamCadData().getLayer( data->getLayerIndex() );

      if (lp != NULL && (lp->getType() == layerTypeStencilTop || lp->getType() == layerTypeStencilBottom))
      {
         return true;
      }

      if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct *insertedBlk = this->getCamCadData().getBlockAt( data->getInsert()->getBlockNumber() );
         return HasStencilGraphic(insertedBlk);
      }
   }

   return false;
}

//------------------------------------------------------------------------
void CStencilGenerator::CloneSurfaceGraphicsToStencilLayers(BlockStruct *block)
{
   if (block != NULL)
   {
      LayerStruct* topStencilLayer    = getCamCadDatabase().getLayer(ccLayerStencilTop   );
      LayerStruct* bottomStencilLayer = getCamCadDatabase().getLayer(ccLayerStencilBottom);

      POSITION datapos = block->getHeadDataPosition();
      while (datapos != NULL)
      {
         DataStruct *data = block->getNextData(datapos);

         LayerStruct *lp = this->getCamCadData().getLayer( data->getLayerIndex() );
         if (lp != NULL &&
            (lp->isLayerTypeTopSurface() || lp->isLayerTypeBottomSurface()) &&
            lp->getLayerType() != layerTypeStencilTop &&
            lp->getLayerType() != layerTypeStencilBottom)
         {
            DataStruct *cloneData = getCamCadData().getNewDataStruct(*data);
            if (lp->isLayerTypeTopSurface())
               cloneData->setLayerIndex( topStencilLayer->getLayerIndex() );
            else
               cloneData->setLayerIndex( bottomStencilLayer->getLayerIndex() );

            block->getDataList().AddTail(cloneData);
         }

         if (data->getDataType() == dataTypeInsert)
         {
            BlockStruct *insertedBlk = getCamCadData().getBlockAt( data->getInsert()->getBlockNumber() );
            CloneSurfaceGraphicsToStencilLayers(insertedBlk);
         }

      }
   }
}

void CStencilGenerator::PromotePanelFidToStencilLayers(DataStruct *fiddata)
{
   // If the fid does not have graphics on stencil layers then 
   // for each graphic on a top or bottom surface put an equivalent graphic
   // of stencil top or bottom layers.

   if (fiddata != NULL && !HasStencilGraphic(fiddata))
   {
      BlockStruct *insertedBlk = this->getCamCadData().getBlockAt( fiddata->getInsert()->getBlockNumber() );
      if (insertedBlk != NULL)
      {
         CloneSurfaceGraphicsToStencilLayers(insertedBlk);
      }
   }
}

void CStencilGenerator::PromotePanelFidsToStencilLayers()
{
   // For each panel fid, create a graphic on the surface stencil layers matching
   // the existing fid's surface layers. I.e. promote top to top and bottom to bottom.
   // Just clone the existing shape and add it to the fid itself, i.e. is permanent
   // change to the actual fid.

   // Note that SG works on PCB files, not panel files, so the active file in SG
   // is not "the" panel we are concerned with. In fact, unless we put in a chooser for
   // the user, we have no way to tell what "the" panel is. So we just do with to all
   // the panels in the document.

   POSITION filepos = this->getCamCadData().getFileList().GetHeadPosition();
   while (filepos != NULL)
   {
      FileStruct *file = this->getCamCadData().getFileList().GetNext(filepos);

      if (file->getBlockType() == blockTypePanel && file->getBlock() != NULL)
      {
         POSITION datapos = file->getBlock()->getHeadDataInsertPosition();
         while (datapos != NULL)
         {
            DataStruct *data = file->getBlock()->getNextDataInsert(datapos);
            if (data->isInsertType(insertTypeFiducial))
            {
               PromotePanelFidToStencilLayers(data);
            }
         }
      }
   }
}

//================================================================================================

bool CStencilGenerator::HasPassthruRuleSet(BlockStruct *block)
{
   // Find $ComponentGeometryVessel$ block, look at rule setting attrib on block.

   if (block != NULL)
   {
      POSITION pos = block->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = block->getNextDataInsert(pos);
         if (data->isInsertType(insertTypeUnknown))
         {
            BlockStruct *insertedBlk = this->getCamCadData().getBlockAt( data->getInsert()->getBlockNumber() );
            if (insertedBlk->getName().CompareNoCase("$ComponentGeometryVessel$") == 0)
            {
               // The rule is kept on the datastruct (insert) of the $ComponentGeometryVessel$.
               // If the keyword for the rule is present, the rule is assigned.
               // These attribs carry no value, i.e. no name/value pair. (Should have been, but aren't.)
               // So attrib is just name (just the keyword),if present it is assigned, not present is not assigned.

               // Would be nice to not use the string name here, to instead use the keyword as calc'd elsewhere in
               // stencilgen code. But we have a "can't get there from here", so heck with it for now. Just use the name.
               WORD stencilRuleKey = this->getCamCadDatabase().getKeywordIndex("StencilPadPassthru");

               CAttribute *stencilRuleAttrib;
               if (data->getAttributes() != NULL &&
                  data->getAttributes()->Lookup(stencilRuleKey, stencilRuleAttrib))
               {
                  return true;
               }
            }
         }
      }
   }

   return false;
}

//--------------------------------------------------------------------------------------

CString CStencilGenerator::GenerateStencilBlockName(BlockStruct &sourceBlock)
{
   // Name is based on a constant and the source block's number.
   // Could use source block's name, that would end up more readable, but
   // block names are not guaranteed unique, while block numbers are.

   CString stencilBlockName;
   stencilBlockName.Format("%s_%d$", m_stencilBlockBaseName, sourceBlock.getBlockNumber());
   return stencilBlockName;
}

//--------------------------------------------------------------------------------------

BlockStruct* CStencilGenerator::GetStencilBlock(FileStruct *file, BlockStruct *block, int insertLayerIndx, int topStencilSrcLyrIndx, int botStencilSrcLyrIndx)
{
   // Leave stencilBlock NULL until we determine that there is actually
   // something to put in it. Create it on the fly, when needed.
   BlockStruct *stencilBlock = NULL;

   if (block != NULL)
   {
      CString stencilBlockName( GenerateStencilBlockName(*block) );

      // Check if already exists, just use it if so.
      stencilBlock = getCamCadDatabase().getBlock(stencilBlockName, file->getFileNumber());
      if (stencilBlock != NULL)
         return stencilBlock;

      CDataList* flatDataList = block->getDataList().getFlattenedDataList(this->getCamCadData(), 
         insertLayerIndx, true);

      int floatingLayerIndx = getCamCadDatabase().getFloatingLayerIndex();

      POSITION flatPos = flatDataList->GetHeadPosition();
      while (flatPos != NULL)
      {
         DataStruct *flatData = flatDataList->GetNext(flatPos);

         /*debug
         DataTypeTag datatype = flatData->getDataType();
         int layerIndx = flatData->getLayerIndex();
         LayerStruct *lp = this->getCamCadData().getLayer(layerIndx);
         CString layername(lp?lp->getName():"null");
         CString insertedBlkName;
         if (flatData->getDataType() == dataTypeInsert)
         {
            BlockStruct *b = this->getCamCadData().getBlockAt(flatData->getInsert()->getBlockNumber());
            if (b)
               insertedBlkName = b->getName();
         }
         */


         switch (flatData->getDataType())
         {
         case dataTypeInsert:
            {
               if (flatData->getLayerIndex() == topStencilSrcLyrIndx)
               {
                  if (stencilBlock == NULL)
                  {
                     stencilBlock = this->getCamCadDatabase().getNewBlock(stencilBlockName, blockTypeUnknown, file->getFileNumber());
                  }
                  BlockStruct *insertedBlock = this->getCamCadData().getBlockAt(flatData->getInsert()->getBlockNumber());
                  if (insertedBlock != NULL)
                  {
                     CString name(insertedBlock->getName());
                     InsertStruct *insert = flatData->getInsert();
                     this->getCamCadDatabase().referenceBlock(stencilBlock, insertedBlock, insertTypeUnknown, "",
                        floatingLayerIndx, insert->getOriginX(), insert->getOriginY(), insert->getAngleRadians(), false);
                  }
               }
            }
            break;
         case dataTypePoly:
            {
               if (flatData->getLayerIndex() == topStencilSrcLyrIndx)
               {
                  if (stencilBlock == NULL)
                  {
                     stencilBlock = this->getCamCadDatabase().getNewBlock(stencilBlockName, blockTypeUnknown, file->getFileNumber());
                  }
                  // Copy poly
                  DataStruct *polystruct = this->getCamCadDatabase().addPolyStruct(stencilBlock->getDataList(), 
                     floatingLayerIndx, 0, false, graphicClassNormal);

                  /*POSITION polypos = flatData->getPolyList()->GetHeadPosition();
                  while (polypos != NULL)
                  {
                     CPoly *polyOriginal = flatData->getPolyList()->GetNext(polypos);
                     CPoly *polyCopy = new CPoly(*polyOriginal);
                     polystruct->getPolyList()->AddTail(polyCopy);
                  }*/
                  polystruct->getPolyList() = flatData->getPolyList();
               }
            }
            break;
         default:
            // Nothing else is supported
            break;
         }
      }
   }

   return stencilBlock;
}

//--------------------------------------------------------------------------------------

BlockStruct* CStencilGenerator::GetStencilBlock(FileStruct *file, DataStruct *insertdata, int topStencilSrcLyrIndx, int botStencilSrcLyrIndx)
{
   BlockStruct *stencilBlock = NULL;

   if (insertdata != NULL)
   {
      BlockStruct *insertedBlk = this->getCamCadData().getBlockAt( insertdata->getInsert()->getBlockNumber() );
      stencilBlock = GetStencilBlock(file, insertedBlk, insertdata->getLayerIndex(), topStencilSrcLyrIndx, botStencilSrcLyrIndx);
   }

   return stencilBlock;
}

//--------------------------------------------------------------------------------------

void CStencilGenerator::PromoteStencilSourceLayer(FileStruct *file, DataStruct *insertdata, int &topHoleCount, int &botHoleCount)
{
   if (file != NULL && insertdata != NULL && insertdata->getDataType() == dataTypeInsert)
   {
      BlockStruct *insertedBlk = this->getCamCadData().getBlockAt( insertdata->getInsert()->getBlockNumber() );

      if (HasPassthruRuleSet(insertedBlk))
      {
         CString topSSLyrName( this->getStencilGenerationParameters().getTopStencilSourceLayerName() );
         CString botSSLyrName( this->getStencilGenerationParameters().getBottomStencilSourceLayerName() );
         int topSSLyrIndx = this->getCamCadDatabase().getLayerIndex(topSSLyrName);
         int botSSLyrIndx = this->getCamCadDatabase().getLayerIndex(botSSLyrName);

         BlockStruct *stencilBlock = GetStencilBlock(file, insertdata, topSSLyrIndx, botSSLyrIndx);

         if (stencilBlock != NULL)
         {
            LayerStruct* topStencilLayer    = getCamCadDatabase().getLayer(ccLayerStencilTop   );
            LayerStruct* bottomStencilLayer = getCamCadDatabase().getLayer(ccLayerStencilBottom);

            InsertStruct *insert = insertdata->getInsert();
            int stencilLayerIndx = insert->getPlacedTop() ? topStencilLayer->getLayerIndex() : bottomStencilLayer->getLayerIndex();

            // Place stencilBlock at same location at original insertdata. 
            // EXCEPT do not use MIRROR. We always place stencils top, and put stencil content
            // on specific layers. Whether stencil layers are mirrored or not is variable, unreliable.
            // So we assume not mirrored, and place stencil from top, but always on specific layer.

            // For now atleast, the stencilblock is generated with only polys or aperture inserts in
            // the datalist. We shall assume that each item in the data list represents one hole.
            // So the count of stencil holes produced by this is the size of the datalist.

            DataStruct *stencilInsertData = getCamCadDatabase().referenceBlock(file->getBlock(), stencilBlock, insertTypeStencilHole, "", stencilLayerIndx, 
               insert->getOriginX(), insert->getOriginY(), insert->getAngleRadians(), 0 /*insert->getGraphicMirrored()*/);
            
            if (insert->getGraphicMirrored())
            {
               stencilInsertData->getInsert()->setMirrorFlags(MIRROR_FLIP); // Mirror graphics only, NOT layers
               botHoleCount += stencilBlock->getDataCount();
            }
            else
            {
               topHoleCount += stencilBlock->getDataCount();
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------

void CStencilGenerator::DeleteExistingStencilBlocks()
{
   // Delete all blocks that have name in form of stencil source layer stencil blocks.

   for (int i = 0; i < this->getCamCadData().getMaxBlockIndex(); i++)
   {
      BlockStruct *block = this->getCamCadData().getBlockAt(i);
      if (block != NULL)
      {
         if (block->getName().Find(m_stencilBlockBaseName) > -1)
         {
            delete block;
         }
      }
   }

   this->getCamCadDatabase().invalidateBlockIndex();  // Because we deleted blocks out from under it
}

//--------------------------------------------------------------------------------------

void CStencilGenerator::PromoteStencilSourceLayerToStencilLayers(int &topHoleCount, int &botHoleCount)
{
   topHoleCount = 0;
   botHoleCount = 0;

   DeleteExistingStencilBlocks();

   // For every file ...
   POSITION filepos = this->getCamCadData().getFileList().GetHeadPosition();
   while (filepos != NULL)
   {
      FileStruct *file = this->getCamCadData().getFileList().GetNext(filepos);

      // If file is panel or pcb ...
      if (file->getBlock() != NULL &&
         (file->getBlockType() == blockTypePanel || file->getBlockType() == blockTypePcb))
      {
         // Process datalist of file
         POSITION datapos = file->getBlock()->getHeadDataInsertPosition();
         while (datapos != NULL)
         {
            DataStruct *data = file->getBlock()->getNextDataInsert(datapos);
            if (data->isInsertType(insertTypePcbComponent))
            {
               this->PromoteStencilSourceLayer(file, data, topHoleCount, botHoleCount);
            }
         }
      }
   }
}

//================================================================================================

void CStencilGenerator::generatePinReports(CWriteFormat& logFile,COperationProgress* progress)
{
   CSortedStencilPins stencilHoles(nextPrime2n((int)(m_stencilPins.GetSize() * 1.2)));

   if (progress != NULL)
   {
      progress->updateStatus("Generating pin reports",3. * m_stencilPins.GetCount());
   }

   for (POSITION pos = m_stencilPins.GetHeadPosition();pos != NULL;)
   {
      if (progress != NULL)
      {
         progress->incrementProgress();
      }

      CStencilPin* stencilPin = m_stencilPins.GetNext(pos);

      stencilHoles.SetAt(stencilPin->getSortablePinRef(),stencilPin);
   }

   generatePinReport(logFile,stencilHoles,true ,progress);
   generatePinReport(logFile,stencilHoles,false,progress);
}

CString getHtmlGridContent(CString gridContent)
{
   gridContent.Trim();

   if (gridContent.IsEmpty())
   {
      gridContent = "&nbsp;";
   }

   return gridContent;
}

void CStencilGenerator::generatePinReport(CWriteFormat& logFile,
   CSortedStencilPins& stencilHoles,bool topFlag,COperationProgress* progress)
{
   logFile.writef(
"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\""
"<caption><p align=\"center\"><b><font size=\"6\" face=\"Tahoma\">Pin Report (%s)</font></b></p></caption>\n",
      (topFlag ? "Top" : "Bottom"));

   CString displayUnitsString = unitStringAbbreviation(CStencilPin::getDisplayUnits(camCadDatabase_camCadDataF().getPageUnits())).MakeLower();

   logFile.writef(
"<tr>\n"
"  <td align=\"center\"><b>Pin<br>Reference</b></td>\n"
"  <td align=\"center\"><b>Copper<br>Pad</b></td>\n"
"  <td align=\"center\"><b>Stencil<br>Pad</b></td>\n"
"  <td align=\"center\"><b>Component<br>Geometry<br>Name</b></td>\n"
"  <td align=\"center\"><b>Derivation<br>Method</b></td>\n"
"  <td align=\"center\"><b>Stencil<br>Thickness<br>(%s)</b></td>\n"
"  <td align=\"center\"><b>Solder<br>Paste<br>Volume<br>(%s<sup>3</sup>)</b></td>\n"
"  <td align=\"center\"><b>Rule<br>Violations</b></td>\n"
"  <td align=\"center\"><b>Rule<br>Comment</b></td>\n"
"</tr>\n",
displayUnitsString,
displayUnitsString);

   CStencilPin* stencilPin;
   double pageUnitsToDisplayUnitsFactor = 0.;

   for (stencilHoles.rewind();stencilHoles.next(stencilPin);)
   {
      if (progress != NULL)
      {
         progress->incrementProgress();
      }

      if (pageUnitsToDisplayUnitsFactor == 0.)
      {
         pageUnitsToDisplayUnitsFactor = stencilPin->getPageUnitsToDisplayUnitsFactor();
      }

      bool subPadFlag = (stencilPin->getSubPinCount() > 1);

      for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
      {
         CStencilHole& stencilHole = stencilPin->getStencilHole(subPinIndex,topFlag);

         CString stencilAperture,derivationMethod,ruleViolations,pasteVolumeString, ruleComment;

         CString pinRef           = stencilPin->getPinRef();
         CString geometryName     = stencilPin->getGeometryName();
         CString copperAperture   = stencilHole.getCopperApertureDescriptor();

         if (subPadFlag)
         {
            pinRef.AppendFormat(".%d",subPinIndex + 1);
         }

         bool hasStencilHoleFlag = (stencilHole.getStencilHoleInstance() != NULL);

         if (hasStencilHoleFlag)
         {
            ruleComment      = stencilHole.getStencilRule().getComment();
            stencilAperture  = stencilHole.getStencilApertureDescriptor();
            derivationMethod = stencilHole.getDerivationMethod();
            ruleViolations   = stencilHole.getRuleViolations();

            pasteVolumeString.Format("%.3f",stencilHole.getPasteVolume());
         }

         logFile.writef("<tr>\n");
         logFile.pushHeader("  ");

         logFile.writef("<td>%s</td>\n",pinRef);
         logFile.writef("<td>%s</td>\n",getHtmlGridContent(copperAperture));
         logFile.writef("<td>%s</td>\n",getHtmlGridContent(stencilAperture));
         logFile.writef("<td>%s</td>\n",getHtmlGridContent(geometryName));
         logFile.writef("<td>%s</td>\n",getHtmlGridContent(derivationMethod));
         logFile.writef("<td><p align=\"right\">%.1f</td>\n",stencilHole.getStencilThickness() * pageUnitsToDisplayUnitsFactor);
         logFile.writef("<td><p align=\"right\">%s</td>\n",getHtmlGridContent(pasteVolumeString));
         logFile.writef("<td>%s</td>\n",getHtmlGridContent(ruleViolations));
         logFile.writef("<td>%s</td>\n",getHtmlGridContent(ruleComment));

         logFile.popHeader();
         logFile.writef("</tr>\n");
      }
   }

   logFile.writef(
"</table>\n"
"<p>&nbsp;</p>\n"
   );
}

void CStencilGenerator::performStencilValidationChecks(
   CStencilValidationParameters& validationParameters,
   CStencilGenerationParameters& generationParameters)
{
   COperationProgress progress(m_stencilPins.GetCount());
   progress.updateStatus("Performing Stencil Validation Checks");
   validationParameters.setViolationCount(0);
   m_stencilViolations.empty();

   for (POSITION pos = m_stencilPins.GetHeadPosition();pos != NULL;)
   {
      progress.incrementProgress();

      CStencilPin* stencilPin = m_stencilPins.GetNext(pos);

      if (stencilPin != NULL)
      {
         for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
         {
            stencilPin->getStencilHole(subPinIndex,true ).performStencilValidationChecks(m_stencilViolations,validationParameters,generationParameters);
            stencilPin->getStencilHole(subPinIndex,false).performStencilValidationChecks(m_stencilViolations,validationParameters,generationParameters);
         }
      }
   }
}

/*
[StencilGenerationParameters]
topPasteInHole 0
bottomPasteInHole 0
topStencilThickness 0.203200
bottomStencilThickness 0.203200
topMaxFeatureSize 5.000000
bottomMaxFeatureSize 5.000000
topWebSize 1.000000
bottomWebSize 1.000000

[StencilValidationParameters]
minHoleAreaToWallAreaRatio 0.660000
minFeature 0.304800
maxFeature 5.000000
minInset 0.025400

[StencilGenerationRules]
pcb Inset 0.050000
surface top Area 0.900000
mount top.smd Inset 0.040000
componentGeometry 0603 Aperture round100m2
//padstackGeometry PADSTACK_64 Area 0.700000
padstack 0603.PADSTACK_64 Area 0.700000
component C5 Inset 0.030000
pad 0603.1 Area 0.600000
pin C5.1 Area 0.600000
pin C263.1 Aperture tabbedRectangle200x100x75x50x2corm2
*/
bool CStencilGenerator::readStencilSettings(const CString& ruleString)
{
   CStencilEntityRule stencilEntityRule(ruleString);

   bool retval = stencilEntityRule.isValid();

   if (retval)
   {
      //BlockStruct& padstackVesselGeometry = getPadstackVesselGeometry();
      //int numpar = params.GetCount();

      switch (stencilEntityRule.getEntity())
      {
      case attributeSourceGeometryPin:
         break;
      case attributeSourceComponentSubPin:
         break;
      case attributeSourceComponentPin:
         break;
      case attributeSourceComponent:
         break;
      case attributeSourceSubPadstack:
         break;
      case attributeSourcePadstack:
         break;
      case attributeSourceGeometry:
         break;
      case attributeSourceMount:
         {
            CString mountType = stencilEntityRule.getEntityDesignator1();
            CString surface   = stencilEntityRule.getEntityDesignator2();

            bool topFlag,smdFlag,viaFlag,fiducialFlag;

            if ((topFlag = (surface.CompareNoCase(QTop) == 0)) || surface.CompareNoCase(QBottom) == 0)
            {
               if ((smdFlag      = (mountType.CompareNoCase(QSmd     ) == 0)) ||
                   (viaFlag      = (mountType.CompareNoCase(QVia     ) == 0)) ||
                   (fiducialFlag = (mountType.CompareNoCase(QFiducial) == 0)) ||
                    mountType.CompareNoCase(QTh) == 0)
               {
                  DataStruct& mountVessel = getMountVessel(topFlag,smdFlag,viaFlag,fiducialFlag);

                  //attributes     = &(mountVessel->getAttributesRef());
                  //attributeStartIndex = 2;
               }
            }

            break;
         }
         break;
      case attributeSourceSurface:
         break;
      case attributeSourcePcb:
         break;
      }
   }

   return retval;
}

bool CStencilGenerator::readStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase,COperationProgress* operationProgress)
{
   bool retval = false;
   CStencilEntityIterator stencilEntityIterator(*this);
   CStencilEntityRule stencilEntityRule;
   CAttributes* attributes;

   CString value;
   CMapStringToString stencilEntityWithPrefixMap;
   stencilEntityWithPrefixMap.InitHashTable(nextPrime2n(stencilEntityIterator.getCount()));

   if (operationProgress != NULL)
   {
      operationProgress->setLength(stencilEntityIterator.getCount());
   }

   stencilGeneratorSettingsDatabase.emptyStencilEntities();


   // We need to read the settings before reading the rules.
   // The settings may effect how the rules are read, in particular, the propagateRules setting.
   int pcbFileNum = getStencilUi().getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      retval = stencilGeneratorSettingsDatabase.getStencilSettings(
                  getStencilGenerationParameters(),
                  getStencilValidationParameters());
   }


   for (;stencilEntityIterator.hasNext();)
   {
      stencilEntityIterator.getNext(stencilEntityRule,attributes);

      if (operationProgress != NULL)
      {
         operationProgress->incrementProgress();
      }

      if (stencilEntityRule.getEntity() == attributeSourcePcb         ||
          stencilEntityRule.getEntity() == attributeSourceSurface     ||
          stencilEntityRule.getEntity() == attributeSourceMount       ||
          stencilEntityRule.getEntity() == attributeSourceSubPadstack ||
          stencilEntityRule.getEntity() == attributeSourcePadstack    ||
          stencilEntityRule.getEntity() == attributeSourceGeometry    ||
          stencilEntityRule.getEntity() == attributeSourceGeometryPin    )
      {
         CString entityString     = stencilEntityRule.getEntityString();
         CString designator1      = stencilEntityRule.getEntityDesignator1();
         CString designator2      = stencilEntityRule.getEntityDesignator2();
         CString entityWithPrefix = entityString + " " + getDesignatorString(designator1) + " " + getDesignatorString(designator2);

         if (! stencilEntityWithPrefixMap.Lookup(entityWithPrefix,value))
         {
            stencilEntityWithPrefixMap.SetAt(entityWithPrefix,entityWithPrefix);

            stencilGeneratorSettingsDatabase.addStencilEntity(entityString,designator1,designator2);

            // This is for backward compatibility.
            // We no longer do side-specific rules in SG, but the database may have them side-specific.
            // So we want to add top and bottom versions to the query, so we get the whole mess, and we'll sort if out after.
            if (designator1.CompareNoCase(QBoth) == 0)
            {
               stencilGeneratorSettingsDatabase.addStencilEntity(entityString,QTop,designator2);
               stencilGeneratorSettingsDatabase.addStencilEntity(entityString,QBottom,designator2);
            }
         }
      }

      retval = true;
   }

   CStencilEntityRuleArray dbStencilEntityRules;

   if (stencilGeneratorSettingsDatabase.getStencilEntityRules(getStencilGenerationParameters().getPropagateRulesSideToSide(), dbStencilEntityRules))
   {
      CMapStringToInt geomPadstackRuleAppliedMap; // Keeps track of whether mapping rule worked on a given comp geometry
      CStencilEntityRuleArray unmatchedStencilEntityRules;  // Rules gathered for second pass attempt at entity matchup
      CStencilEntityRuleArray obsoleteRules;  // These will get deleted at end of processing

      for (int index = 0;index < dbStencilEntityRules.GetSize();index++)
      {
         CStencilEntityRule* originalStencilEntityRule = dbStencilEntityRules.GetAt(index);

         if (stencilEntityIterator.lookup(*originalStencilEntityRule,stencilEntityRule,attributes))
         {
            CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*attributes);
            stencilRuleAttributes.setRule(*originalStencilEntityRule);
         }
         else if (originalStencilEntityRule->getEntity() == attributeSourcePadstack) // Apply this twiddling only to padstack rules
         {
            // Cases dts0100500082 and dts0100500083 - Kluge to compensate for poor old-to-new database migration.
            // Old DB and old DB mirgated to new DB have padstack geometry names as keys, while the
            // new (current) system uses a pin name scheme. If the rule is a padstack (and the lookup 
            // above has already failed) then try to find a pin in given geometry with same
            // padstack geometry, if found then alter the rule and re-apply.

            CString compgeomname = originalStencilEntityRule->getEntityDesignator2();
            BlockStruct *compBlk = this->getCamCadDoc().Find_Block_by_Name(compgeomname, -1);

            if (compBlk != NULL)
            {
               // This portion treats dts0100500082 (Visula) style problem, where padstack geom name can be matched
               CString padstackgeomname(originalStencilEntityRule->getEntityDesignator3());
               DataStruct *pinData = compBlk->getPinUsingPadstack(this->getCamCadData(), padstackgeomname);

               if (pinData != NULL)
               {
                  CString pinRefname(pinData->getInsert()->getRefname());
                  CStencilEntityRule modifiedStencilEntityRule( *originalStencilEntityRule );
                  modifiedStencilEntityRule.setEntityDesignator3(pinRefname);

                  // Try modified rule, same as first lookup
                  if (stencilEntityIterator.lookup(modifiedStencilEntityRule,stencilEntityRule,attributes))
                  {
                     CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*attributes);
                     stencilRuleAttributes.setRule(modifiedStencilEntityRule);

                     // Add the old rule to the obsolete rule collection, Only the designator matters, for later lookup and destruction
                     CStencilEntityRule *badrule = new CStencilEntityRule(*originalStencilEntityRule);
                     obsoleteRules.Add(badrule);

                     // Save the adjusted rule to the DB
                     SetStencilRules2Data(modifiedStencilEntityRule.getEntityString(),
                        modifiedStencilEntityRule.getEntityDesignator1(), modifiedStencilEntityRule.getEntityDesignator2(),
                        modifiedStencilEntityRule.getEntityDesignator3(), modifiedStencilEntityRule.getEntityDesignator4(),
                        modifiedStencilEntityRule.getEntityDesignator5(), modifiedStencilEntityRule.getRuleString());

                     // Visula rule worked on this geometry, update tracking map
                     CString comppadstackKey;
                     comppadstackKey.Format("[%s]%s[%s]", originalStencilEntityRule->getEntityDesignator1(), compgeomname, padstackgeomname);
                     geomPadstackRuleAppliedMap.SetAt(comppadstackKey, 0);

                     // Log result
                     this->getStencilUi().getLogFile().writef("DB Load: Padstack rule %s|%s|%s mapped to Pin %s (Direct padstack name match).\n",
                        originalStencilEntityRule->getEntityDesignator1(), originalStencilEntityRule->getEntityDesignator2(),
                        originalStencilEntityRule->getEntityDesignator3(), modifiedStencilEntityRule.getEntityDesignator3());
                  }
                  else
                  {
                     unmatchedStencilEntityRules.Add(new CStencilEntityRule(*originalStencilEntityRule));
                  }
               }
               else
               {
                  // We already know the rule did not work as-is. And we tried the padstack to pin name mapping
                  // and that did not work either, there was no pin using this padstack. In the context of
                  // the current ccz this is a bogus rule. Add it to the obsolete rule collection, it may
                  // get deleted at the end of processing.

                  CStencilEntityRule *badrule = new CStencilEntityRule(*originalStencilEntityRule);
                  badrule->setEntity(originalStencilEntityRule->getEntity(), originalStencilEntityRule->getEntityDesignator1(),
                     originalStencilEntityRule->getEntityDesignator2(), padstackgeomname);
                  obsoleteRules.Add(badrule);

                  unmatchedStencilEntityRules.Add(new CStencilEntityRule(*originalStencilEntityRule));
               }
            }
         }
      }  // end for


      for (int index = 0;index < unmatchedStencilEntityRules.GetSize();index++)
      {
         CStencilEntityRule* originalStencilEntityRule = unmatchedStencilEntityRules.GetAt(index);

         if (originalStencilEntityRule->getEntity() == attributeSourcePadstack) // Apply this twiddling only to padstack rules
         {
            CString compgeomname = originalStencilEntityRule->getEntityDesignator2();
            BlockStruct *compBlk = this->getCamCadDoc().Find_Block_by_Name(compgeomname, -1);

            if (compBlk != NULL)
            {
               // This portion treats dts0100500083 (Zuken CR5K), where padstack geom name can not be matched.
               // We do not want this rule to override a potential good Visula style match that was done
               // above. We are switching on a per-component basis. So if the Visula style rule above
               // worked then do not apply this rule. We use the map of comp geometry names to tell if
               // any Visula rule worked for this geometry.

               bool success = false;
               bool excessRules = false;

               int padstackWithRulesCount = 0;
               int missingStencilEntityCount = 0;

               int padstackCount = compBlk->getPadstackCount();
               for (int padstackI = 1; padstackI <= padstackCount && !success; padstackI++)
               {
                  DataStruct *pinData = compBlk->getPinUsingPadstack(padstackI);
                  if (pinData != NULL)
                  {
                     BlockStruct *padstackBlk = this->getCamCadDoc().getBlockAt( pinData->getInsert()->getBlockNumber() );
                     if (padstackBlk != NULL)
                     {
                        CString thisPadstackGeomName( padstackBlk->getName() );
                        CString comppadstackKey;
                        comppadstackKey.Format("[%s]%s[%s]", originalStencilEntityRule->getEntityDesignator1(), compgeomname, thisPadstackGeomName);

                        // If no rule yet for this comp/padstack combo, apply this rule
                        int dontcare;
                        if (!geomPadstackRuleAppliedMap.Lookup(comppadstackKey, dontcare))
                        {
                           CString pinRefname = pinData->getInsert()->getRefname();
                           CStencilEntityRule modifiedStencilEntityRule( *originalStencilEntityRule );
                           modifiedStencilEntityRule.setEntityDesignator3(pinRefname);

                           // Try modified rule, same as first lookup
                           if (stencilEntityIterator.lookup(modifiedStencilEntityRule,stencilEntityRule,attributes))
                           {
                              CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*attributes);
                              stencilRuleAttributes.setRule(modifiedStencilEntityRule);

                              success = true;
                              geomPadstackRuleAppliedMap.SetAt(comppadstackKey, 0);

                              this->getStencilUi().getLogFile().writef("DB Load: Padstack rule %s|%s|%s mapped to Pin %s (Suggested padstack assignment).\n",
                                 originalStencilEntityRule->getEntityDesignator1(), originalStencilEntityRule->getEntityDesignator2(),
                                 originalStencilEntityRule->getEntityDesignator3(), modifiedStencilEntityRule.getEntityDesignator3());
                           }
                           else
                           {
                              // We found a candidate padstack according to component, but there is no stencil entity for it.
                              // This probably means this is a "null" padstack, i.e. it has no copper so does not qualify for 
                              // a stencil. (In case dts0100500083 example data this was the situation.)
                              // We'll count these up and use them to reduce the candidate padstack count where we decide
                              // is all padstacks have rules, or not (for the error message).
                              missingStencilEntityCount++;
                           }
                        }
                        else
                        {
                           padstackWithRulesCount++;
                        }
                     }
                     //else
                     //{
                     //   int jj = 0; // null padstack block
                     //}
                  }
                  //else
                  //{
                  //   int jj = 0; // null pin data ptr
                  //}
               } // end  for (int padstackI ...

               // If all padstacks that have stencil entities already have rules, then no room for this new rule
               excessRules = (padstackWithRulesCount >= (padstackCount - missingStencilEntityCount));

               if (!success)
               {
                  CString moreInfo;
                  if (excessRules)
                     moreInfo.Format(" (More rules in DB than padstacks available in CCZ)");

                  // Rule was not applied
                  this->getStencilUi().getLogFile().writef("DB Load: Padstack rule %s|%s|%s skipped%s.\n",
                     originalStencilEntityRule->getEntityDesignator1(),
                     originalStencilEntityRule->getEntityDesignator2(),
                     originalStencilEntityRule->getEntityDesignator3(),
                     moreInfo);
               }

            }
            //else
            //{
            //   int jj = 0; // null comp ptr
            //}
         } // end if (originalStencilEntityRule->getEntity() == attributeSourcePadstack)
      }  // end  for (int index


#ifdef SUPPORT_RULE_KILLER
      // If there were successes in rule mapping then get rid of the obsolete rules
      // Always --- if (atleastOneSuccess)
      {
         CDBInterface db;
         CString dbname( db.GetLibraryDatabaseName() );
         if (db.Connect(dbname, "", ""))
         {

            for (int ruleI = 0; ruleI < obsoleteRules.GetCount(); ruleI++)
            {
               CStencilEntityRule *r = obsoleteRules.GetAt(ruleI);
               db.KillStencilRule(r->getEntityString(), r->getEntityDesignator1(), r->getEntityDesignator2(), r->getEntityDesignator3());  // Disables but does not delete
            }

            db.Disconnect();
         }
      }
#endif

      obsoleteRules.empty();

   }


   if (retval)
   {
      if (pcbFileNum >= 0)
      {
         getStencilGenerationParameters().writeAttributes(pcbFileNum);
         getStencilValidationParameters().writeAttributes(pcbFileNum);
      }
   }

   this->getStencilUi().getLogFile().flush();

   return retval;
}

DataStruct& CStencilGenerator::getComponentClosetVessel(const CString& componentGeometryName,const CString& vesselRef,BlockStruct& vesselGeometry)
{
   BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentGeometryName,m_fileStruct->getFileNumber());

   if (componentGeometry == NULL)
   {
      componentGeometry = getCamCadDatabase().getBlock(componentGeometryName,-1);
   }

   if (componentGeometry == NULL)
   {
      componentGeometry = getCamCadDatabase().getDefinedBlock(componentGeometryName,blockTypePcbComponent,m_fileStruct->getFileNumber());
   }

   DataStruct* vessel = NULL;

   for (CDataListIterator vesselIterator(*componentGeometry,dataTypeInsert);vesselIterator.hasNext();)
   {
      DataStruct* vesselData = vesselIterator.getNext();

      if (vesselData->getInsert()->getBlockNumber() == vesselGeometry.getBlockNumber())
      {
         if (vesselData->getInsert()->getRefname().CompareNoCase(vesselRef) == 0)
         {
            vessel = vesselData;
            break;
         }
      }
   }

   if (vessel == NULL)
   {
      vessel = getCamCadDatabase().insertBlock(&vesselGeometry,insertTypeUnknown,vesselRef,-1);
      componentGeometry->getDataList().AddTail(vessel);
   }

   return *vessel;
}

bool CStencilGenerator::readStencilSettings(CStringArray& params)
{
   bool retval = true;

   CString dot("."),emptyString;
   int numpar = params.GetCount();

   BlockStruct* vesselGeometry = NULL;

   if (numpar > 1)
   {
      AttributeSourceTag entity = stringToAttributeSourceTag(params[0]);
      int attributeStartIndex   = -1;
      CAttributes** attributes  = NULL;

      switch (entity)
      {
      case attributeSourcePcb:              vesselGeometry = &(getPcbVesselGeometry());                break;
      case attributeSourceSurface:          vesselGeometry = &(getSurfaceVesselGeometry());            break;
      case attributeSourceMount:            vesselGeometry = &(getMountVesselGeometry());              break;
      case attributeSourceGeometry:         vesselGeometry = &(getComponentGeometryVesselGeometry());  break;
      case attributeSourcePadstack:         vesselGeometry = &(getPadstackVesselGeometry());           break;
      case attributeSourceSubPadstack:      vesselGeometry = &(getSubPadstackVesselGeometry());        break;
      case attributeSourceGeometryPin:      vesselGeometry = &(getSubPinVesselGeometry());             break;
      case attributeSourceComponent:        vesselGeometry = &(getComponentVesselGeometry());          break;
      case attributeSourceComponentPin:     vesselGeometry = &(getComponentPinVesselGeometry());       break;
      case attributeSourceComponentSubPin:  vesselGeometry = &(getComponentSubPinVesselGeometry());    break;
      }

      switch (entity)
      {
      case attributeSourceGeometry:
      case attributeSourcePadstack:
      case attributeSourceSubPadstack:
      case attributeSourceGeometryPin:
         {
            CString vesselRef             = params[1];
            CSupString vesselDescriptor   = vesselRef;
            CStringArray descriptorParams;
            int numVesselDescriptorParams = vesselDescriptor.ParseQuote(descriptorParams,dot);

            CString surfaceName           = ((numVesselDescriptorParams > 0) ? descriptorParams.GetAt(0) : emptyString);
            CString componentGeometryName = ((numVesselDescriptorParams > 1) ? descriptorParams.GetAt(1) : emptyString);
            //CString padstackGeometryName  = vesselDescriptor.Scan(dot);
            //CString subPinName            = vesselDescriptor.Scan(dot);
            //CString pinName               = vesselDescriptor;

            DataStruct& vessel = getComponentClosetVessel(componentGeometryName,vesselRef,*vesselGeometry);

            //CString vesselRef = surfaceName + dot + componentGeometryName + dot + padstackGeometryName + dot + subPadIndexString;

            //BlockStruct* componentGeometry = getCamCadDatabase().getBlock(componentGeometryName,m_fileStruct->getFileNumber());

            //if (componentGeometry == NULL)
            //{
            //   componentGeometry = getCamCadDatabase().getBlock(componentGeometryName,-1);
            //}

            //if (componentGeometry != NULL)
            //{
            //   DataStruct* vessel = NULL;

            //   for (CDataListIterator vesselIterator(*componentGeometry,dataTypeInsert);vesselIterator.hasNext();)
            //   {
            //      DataStruct* vesselData = vesselIterator.getNext();

            //      if (vesselData->getInsert()->getBlockNumber() == vesselGeometry->getBlockNumber())
            //      {
            //         if (vesselData->getInsert()->getRefname().CompareNoCase(vesselRef) == 0)
            //         {
            //            vessel = vesselData;
            //            break;
            //         }
            //      }
            //   }

            //   if (vessel == NULL)
            //   {
            //      vessel = getCamCadDatabase().insertBlock(vesselGeometry,insertTypeUnknown,vesselRef,-1);
            //      componentGeometry->getDataList().AddTail(vessel);
            //   }

            attributes     = &(vessel.getDefinedAttributes());
            attributeStartIndex = 2;
         }

         break;
      case attributeSourcePcb:
      case attributeSourceSurface:
      case attributeSourceMount:
      case attributeSourceComponent:
      case attributeSourceComponentPin:
      case attributeSourceComponentSubPin:
         {
            CString closetVesselRef = params[1];

            DataStruct& vessel = getClosetVessel(closetVesselRef,*vesselGeometry);

            attributes = &(vessel.getDefinedAttributes());
            attributeStartIndex = 2;
         }

         break;
      }                                  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

      if (attributeStartIndex > 0)
      {
         addStencilSettingAttributes(attributes,params,attributeStartIndex);
      }
   }

   return retval;
}

void CStencilGenerator::addStencilSettingAttributes(CAttributes** attributes,CStringArray& params,int startIndex)
{
   StencilRuleTag ruleTag = stencilRuleUndefined;
   CString ruleValue;
   CStencilRule stencilRule;

   CStencilRuleAttributes::removeStencilAttributes(**attributes);
   int index = startIndex;

   if (index < params.GetSize())
   {
      ruleTag   = ruleStringToStencilRuleTag(params[index]);
      index++;
   }

   switch (ruleTag)
   {
   case stencilRuleNone:
   case stencilRuleInheritFromParent:
      stencilRule.setRule(ruleTag);
      break;
   case stencilRuleInset:
      if (index < params.GetSize())
      {
         ruleValue = params[index];
         index++;

         if (index < params.GetSize())
         {
            CString unitsString = params[index];
            PageUnitsTag rulePageUnits = unitStringAbbreviationToTag(unitsString);

            if (rulePageUnits != pageUnitsUndefined)
            {
               double insetValue = atof(ruleValue);
               CUnits units(rulePageUnits);

               insetValue = units.convertTo(getCamCadDoc().getPageUnits(),insetValue);

               ruleValue = fpfmt(insetValue,6);

               index++;
            }
         }

         stencilRule.setRule(ruleTag);
         stencilRule.setRuleValue(ruleValue);
      }

      break;
   case stencilRuleArea:
   case stencilRuleAperture:
   case stencilRuleApertureGeometry:
      if (index < params.GetSize())
      {
         ruleValue = params[index];
         index++;

         stencilRule.setRule(ruleTag);
         stencilRule.setRuleValue(ruleValue);
      }

      break;
   case stencilRuleStencilSourceLayer:
   case stencilRulePassthru:
   case stencilRuleCopperSourceLayer:
      stencilRule.setRule(ruleTag);

      break;
   }

   if (stencilRule.isValid())
   {
      for (;index < params.GetSize();index++)
      {
         CString modifierString = params.GetAt(index);
         stencilRule.addModifier(modifierString);
      }

      CStencilRuleAttributes stencilRuleAttributes(getCamCadDatabase(),*attributes);
      stencilRuleAttributes.setRule(stencilRule);
   }
}

void CStencilGenerator::clearStencilVessels()
{
   // CStencilHole::m_componentGeometryVessel;
   // CStencilHole::m_padstackVessel;
   // CStencilHole::m_subPadstackVessel;
   // CStencilHole::m_subPinVessel;
   // CStencilHole::m_componentVessel;
   // CStencilHole::m_componentPinVessel;
   // CStencilHole::m_componentSubPinVessel;
   // CStencilGenerator::m_pcbVessel;
   // CStencilGenerator::m_surfaceVessel[2];
   // CStencilGenerator::m_mountVessel[8];
   // CStencilGenerator::m_stencilGeneratorCloset;
   // CStencilGenerator::m_closetVesselMap;

   m_stencilPins.clearStencilVessels();

   m_pcbVessel        = NULL;

   m_surfaceVessel[0] = NULL;
   m_surfaceVessel[1] = NULL;

   m_mountVessel[0]   = NULL;
   m_mountVessel[1]   = NULL;
   m_mountVessel[2]   = NULL;
   m_mountVessel[3]   = NULL;
   m_mountVessel[4]   = NULL;
   m_mountVessel[5]   = NULL;
   m_mountVessel[6]   = NULL;
   m_mountVessel[7]   = NULL;

   m_closetVesselMap.empty();

   {
      int stencilGeneratorClosetGeometryNumber = getStencilGeneratorClosetGeometry().getBlockNumber();

      for (CModifiableDataListIterator insertIterator(*(getFileStruct()->getBlock()),dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();

         if (data->getInsert()->getBlockNumber() == stencilGeneratorClosetGeometryNumber)
         {
            insertIterator.deleteCurrent();
         }
      }

      getStencilGeneratorClosetGeometry().getDataList().empty();

      m_stencilGeneratorCloset = NULL;
   }

   int componentGeometryVesselBlockNumber  = getComponentGeometryVesselGeometry().getBlockNumber();
   int padstackVesselBlockNumber           = getPadstackVesselGeometry().getBlockNumber();
   int subPadstackVesselBlockNumber        = getSubPadstackVesselGeometry().getBlockNumber();
   int subPinVesselBlockNumber             = getSubPinVesselGeometry().getBlockNumber();
   int pcbVesselBlockNumber                = getPcbVesselGeometry().getBlockNumber();
   int surfaceVesselBlockNumber            = getSurfaceVesselGeometry().getBlockNumber();
   int mountVesselBlockNumber              = getMountVesselGeometry().getBlockNumber();
   int componentVesselBlockNumber          = getComponentVesselGeometry().getBlockNumber();
   int componentPinVesselBlockNumber       = getComponentPinVesselGeometry().getBlockNumber();
   int componentSubPinVesselBlockNumber    = getComponentSubPinVesselGeometry().getBlockNumber();
   int stencilGeneratorClosetBlockNumber   = getStencilGeneratorClosetGeometry().getBlockNumber();

   for (int blockIndex = 0;blockIndex < getCamCadDoc().getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = getCamCadDoc().getBlockAt(blockIndex);

      if (block != NULL)
      {
         for (CModifiableDataListIterator insertIterator(*block,dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            int subBlockNumber = data->getInsert()->getBlockNumber();

            if (subBlockNumber == componentGeometryVesselBlockNumber  ||
                subBlockNumber == padstackVesselBlockNumber           ||
                subBlockNumber == subPadstackVesselBlockNumber        ||
                subBlockNumber == subPinVesselBlockNumber             ||
                subBlockNumber == pcbVesselBlockNumber                ||
                subBlockNumber == surfaceVesselBlockNumber            ||
                subBlockNumber == mountVesselBlockNumber              ||
                subBlockNumber == componentVesselBlockNumber          ||
                subBlockNumber == componentPinVesselBlockNumber       ||
                subBlockNumber == componentSubPinVesselBlockNumber    ||
                subBlockNumber == stencilGeneratorClosetBlockNumber        )
            {
               insertIterator.deleteCurrent();
            }
            else
            {
               BlockStruct* subBlock = getCamCadDoc().getBlockAt(subBlockNumber);
               CString blockName = subBlock->getName();

               if (blockName.CompareNoCase("$PinVessel$") == 0 ||
                   blockName.CompareNoCase("$componentVessel$") == 0 ||
                   blockName.CompareNoCase("$ComponentPinsVessel$") == 0  )
               {
                  insertIterator.deleteCurrent();
               }
            }
         }
      }
   }
}

void CStencilGenerator::instantiateStencilVessels()
{
   m_stencilPins.instantiateStencilVessels();
}

void CStencilGenerator::clearStencilSettings()
{
   for (CStencilEntityIterator stencilEntityIterator(*this);stencilEntityIterator.hasNext();)
   {
      CStencilEntityRule stencilEntityRule;
      CAttributes* attributes;

      stencilEntityIterator.getNext(stencilEntityRule,attributes);
      CStencilRuleAttributes::removeStencilAttributes(*attributes);
   }

   m_stencilPins.initializeRoot();
}

bool CStencilGenerator::writeStencilSettings(CStencilSettingEntryMap& stencilSettingEntryMap,bool ignoreComponentRulesFlag)
{
   bool retval = true;

   for (CStencilEntityIterator stencilEntityIterator(*this);stencilEntityIterator.hasNext();)
   {
      CStencilEntityRule stencilEntityRule;
      CAttributes* attributes;

      stencilEntityIterator.getNext(stencilEntityRule,attributes);

      bool componentRuleFlag = (stencilEntityRule.getEntity() == attributeSourceComponent       ||
                                stencilEntityRule.getEntity() == attributeSourceComponentPin    ||
                                stencilEntityRule.getEntity() == attributeSourceComponentSubPin    );

      if (ignoreComponentRulesFlag && componentRuleFlag)
      {
         continue;
      }

      stencilSettingEntryMap.addEntry(stencilEntityRule.getRuleString(getPageUnits()),stencilEntityRule.getComment());
   }

   return retval;
}

bool CStencilGenerator::writeStencilSettings(CWriteFormat& settingsFile,CStencilSettingEntryMap& stencilSettingEntryMap)
{
   bool retval = false;

   settingsFile.write("[StencilGenerationRules]\n");
   CString quoteTriggerList(" .");

   if (writeStencilSettings(stencilSettingEntryMap,false))
   {
      stencilSettingEntryMap.write(settingsFile);

      retval = true;
   }

   return retval;
}

bool CStencilGenerator::writeStencilSettings(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase)
{
   bool retval = false;
   CStencilSettingEntryMap stencilSettingEntryMap(nextPrime2n(1000));

   if (writeStencilSettings(stencilSettingEntryMap,true))
   {
      stencilSettingEntryMap.write(stencilGeneratorSettingsDatabase);

      retval = true;
   }

   return retval;
}

bool CStencilGenerator::isValid()
{
   bool retval = true;

   if (! m_stencilPins.isValid())
   {
      retval = false;
   }

   if (! m_stencilViolations.isValid())
   {
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CStencilEntityIteratorItem::CStencilEntityIteratorItem(const CStencilEntityRule& stencilEntityRule,DataStruct& vesselData)
: m_stencilEntityRule(stencilEntityRule)
, m_vesselData(vesselData)
{
}

const CStencilEntityRule& CStencilEntityIteratorItem::getStencilEntityRule()
{
   return m_stencilEntityRule;
}

const DataStruct& CStencilEntityIteratorItem::getVesselData() const
{
   return m_vesselData;
}

CAttributes* CStencilEntityIteratorItem::getAttributes()
{
   return m_vesselData.getDefinedAttributes();
}

//_____________________________________________________________________________
CStencilEntityIteratorItems::CStencilEntityIteratorItems()
: m_itemMap(NULL)
{
}

CStencilEntityIteratorItems::~CStencilEntityIteratorItems()
{
   delete m_itemMap;
}

void CStencilEntityIteratorItems::empty()
{
   m_items.empty();

   delete m_itemMap;
   m_itemMap = NULL;
}

CTypedMapStringToPtrContainer<CStencilEntityIteratorItem*>& CStencilEntityIteratorItems::getItemMap() const
{
   if (m_itemMap == NULL)
   {
      int size = (int)min(100,1.2*m_items.GetCount());

      m_itemMap = new CTypedMapStringToPtrContainer<CStencilEntityIteratorItem*>(nextPrime2n(size),false);

      for (POSITION pos = m_items.GetHeadPosition();pos != NULL;)
      {
         CStencilEntityIteratorItem* item = m_items.GetNext(pos);

         const CStencilEntityRule& stencilEntityRule = item->getStencilEntityRule();

         CString key = stencilEntityRule.getEntityString() + " " + stencilEntityRule.getEntityDesignator();
         m_itemMap->SetAt(key,item);
      }
   }

   return *m_itemMap;
}

POSITION CStencilEntityIteratorItems::GetHeadPosition() const
{
   return m_items.GetHeadPosition();
}

CStencilEntityIteratorItem* CStencilEntityIteratorItems::GetNext(POSITION& pos) const
{
   return m_items.GetNext(pos);
}

void CStencilEntityIteratorItems::AddTail(CStencilEntityIteratorItem* item)
{
   m_items.AddTail(item);

   if (m_itemMap != NULL)
   {
      const CStencilEntityRule& stencilEntityRule = item->getStencilEntityRule();

      CString key = stencilEntityRule.getEntityString() + " " + stencilEntityRule.getEntityDesignator();
      m_itemMap->SetAt(key,item);
   }
}

int CStencilEntityIteratorItems::getCount() const
{
   return m_items.GetCount();
}

bool CStencilEntityIteratorItems::lookup(const CStencilEntityRule& stencilEntityRule,
   CStencilEntityRule& foundStencilEntityRule,CAttributes*& foundAttributes) const
{
   bool retval = false;
   CString key = stencilEntityRule.getEntityString() + " " + stencilEntityRule.getEntityDesignator();
   CStencilEntityIteratorItem* item;
   //const DataStruct* vesselData;

   //CString debugRuleString = stencilEntityRule.getRuleString();

   if (getItemMap().Lookup(key,item))
   {
      //vesselData             = &(item->getVesselData());
      foundStencilEntityRule = item->getStencilEntityRule();
      foundAttributes        = item->getAttributes();

      retval = true;
   }
   else
   {
      retval = false;
   }

   return retval;
}

//_____________________________________________________________________________
CStencilEntityIterator::CStencilEntityIterator(CStencilGenerator& stencilGenerator)
: m_stencilGenerator(stencilGenerator)
, m_pcbFile(stencilGenerator.getFileStruct())
{
   init();
}

CStencilEntityIterator::CStencilEntityIterator(CStencilGenerator& stencilGenerator,FileStruct* pcbFile)
: m_stencilGenerator(stencilGenerator)
, m_pcbFile(pcbFile)
{
   init();
}

void CStencilEntityIterator::init()
{
   m_items.empty();

   if (m_pcbFile != NULL )
   {
      CStencilEntityRule stencilEntityRule;
      CString dot("."),emptyString;
      CAttributes* attributes;
      DataStruct* vesselData;
      CCamCadDatabase& camCadDatabase = m_stencilGenerator.getCamCadDatabase();
      CCEtoODBDoc& camCadDoc = camCadDatabase.getCamCadDoc();
      BlockStruct& padstackVesselGeometry        = m_stencilGenerator.getPadstackVesselGeometry();
      BlockStruct& subPadstackVesselGeometry     = m_stencilGenerator.getSubPadstackVesselGeometry();
      BlockStruct& subPinVesselGeometry          = m_stencilGenerator.getSubPinVesselGeometry();
      BlockStruct& componentVesselGeometry       = m_stencilGenerator.getComponentVesselGeometry();
      BlockStruct& componentPinVesselGeometry    = m_stencilGenerator.getComponentPinVesselGeometry();
      BlockStruct& componentSubPinVesselGeometry = m_stencilGenerator.getComponentSubPinVesselGeometry();
      BlockStruct* pcbBlock = m_pcbFile->getBlock();

      //  --  --  pcb  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
      {
         vesselData = &(m_stencilGenerator.getPcbVessel());
         attributes = vesselData->getDefinedAttributes();

         CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
         stencilEntityRule = stencilRuleAttributes.getRule();
         stencilEntityRule.setEntity(attributeSourcePcb,"pcb");

         addItem(stencilEntityRule,*vesselData);
      }

      //  --  --  surface  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
      for (int index = 0;index < 2;index++)
      {
         bool topFlag = (index == 0);
         vesselData = &(m_stencilGenerator.getSurfaceVessel(topFlag));
         attributes = vesselData->getDefinedAttributes();

         CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
         stencilEntityRule = stencilRuleAttributes.getRule();
         stencilEntityRule.setEntity(attributeSourceSurface,::getSurfaceName(topFlag));

         addItem(stencilEntityRule,*vesselData);
      }

      //  --  --  mount  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --

      //  oldIndex  topFlag  smdFlag  viaFlag  fidFlag
      //      0       1        0        0        0
      //      1       1        1        0        0
      //      2       0        0        0        0
      //      3       0        1        0        0
      //      4       0        0        1        0
      //      5       1        0        1        0
      //      6       0        0        0        1
      //      7       1        0        0        1

      for (int index = 0;index < 8;index++)
      {
         bool topFlag,smdFlag,viaFlag,fidFlag;
         CString designatorSuffix;

         switch (index)
         {
         case 0:  topFlag =  true;  smdFlag = false;  viaFlag = false;  fidFlag = false;  designatorSuffix = QTh;        break;
         case 1:  topFlag =  true;  smdFlag =  true;  viaFlag = false;  fidFlag = false;  designatorSuffix = QSmd;       break;
         case 2:  topFlag = false;  smdFlag = false;  viaFlag = false;  fidFlag = false;  designatorSuffix = QTh;        break;
         case 3:  topFlag = false;  smdFlag =  true;  viaFlag = false;  fidFlag = false;  designatorSuffix = QSmd;       break;
         case 4:  topFlag = false;  smdFlag = false;  viaFlag =  true;  fidFlag = false;  designatorSuffix = QVia;       break;
         case 5:  topFlag =  true;  smdFlag = false;  viaFlag =  true;  fidFlag = false;  designatorSuffix = QVia;       break;
         case 6:  topFlag = false;  smdFlag = false;  viaFlag = false;  fidFlag =  true;  designatorSuffix = QFiducial;  break;
         case 7:  topFlag =  true;  smdFlag = false;  viaFlag = false;  fidFlag =  true;  designatorSuffix = QFiducial;  break;
         }

         DataStruct& mountVessel = m_stencilGenerator.getMountVessel(topFlag,smdFlag,viaFlag,fidFlag);
         vesselData = &(m_stencilGenerator.getMountVessel(topFlag,smdFlag,viaFlag,fidFlag));
         attributes = vesselData->getDefinedAttributes();

         CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
         stencilEntityRule = stencilRuleAttributes.getRule();
         stencilEntityRule.setEntity(attributeSourceMount,::getSurfaceName(topFlag),designatorSuffix);

         addItem(stencilEntityRule,*vesselData);
      }

      //  --  --  component geometry  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
      CMapStringToString topComponentGeometriesMap,bottomComponentGeometriesMap;
      topComponentGeometriesMap.InitHashTable(nextPrime2n(250));
      bottomComponentGeometriesMap.InitHashTable(nextPrime2n(250));

      for (CDataListIterator componentIterator(*(m_pcbFile->getBlock()),insertTypePcbComponent);componentIterator.hasNext();)
      {
         DataStruct* component = componentIterator.getNext();
         bool topFlag = component->getInsert()->getPlacedTop();
         BlockStruct* componentGeometry = camCadDoc.getBlockAt(component->getInsert()->getBlockNumber());
         CString geometryName = componentGeometry->getName();

         if (true)//(m_stencilGenerator.getStencilGenerationParameters().getPropagateRulesSideToSide())
         {
            // Keep both sides the same - Propagate rules side to side
            topComponentGeometriesMap.SetAt(geometryName,geometryName);
            bottomComponentGeometriesMap.SetAt(geometryName,geometryName);
         }
         else
         {
            // original, non-shared non-propagate mode
            CMapStringToString& componentGeometriesMap = (topFlag ? topComponentGeometriesMap : bottomComponentGeometriesMap);

            componentGeometriesMap.SetAt(geometryName,geometryName);
         }
      }

      CString temp;

      for (int blockIndex = 0;blockIndex < camCadDoc.getMaxBlockIndex();blockIndex++)
      {
         BlockStruct* componentGeometry = camCadDoc.getBlockAt(blockIndex);

         if (componentGeometry == NULL) continue;

         CString componentGeometryName = componentGeometry->getName();

         if (componentGeometry->getFileNumber() == m_pcbFile->getFileNumber() ||
             componentGeometry->getFileNumber() == -1)
         {
            if (componentGeometry->getBlockType() == blockTypePcbComponent &&
                componentGeometry->getDataList().GetCount() > 0                )
            {
               for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
               {
                  bool topFlag = (surfaceIndex == 0);
                  CString surfaceName = ::getSurfaceName(topFlag);
                  CMapStringToString& componentGeometriesMap = (topFlag ? topComponentGeometriesMap : bottomComponentGeometriesMap);

                  if (componentGeometriesMap.Lookup(componentGeometryName,temp))
                  {
                     vesselData = &(m_stencilGenerator.getComponentGeometryVessel(topFlag,componentGeometryName));
                     attributes = vesselData->getDefinedAttributes();

                     CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
                     stencilEntityRule = stencilRuleAttributes.getRule();
                     stencilEntityRule.setEntity(attributeSourceGeometry,surfaceName,componentGeometryName);

                     addItem(stencilEntityRule,*vesselData);

                     for (CDataListIterator vesselIterator(*componentGeometry,dataTypeInsert);vesselIterator.hasNext();)
                     {
                        vesselData = vesselIterator.getNext();
                        int vesselGeometryNumber = vesselData->getInsert()->getBlockNumber();

                        CSupString reference(vesselData->getInsert()->getRefname());
                        CStringArray descriptorParams;
                        int numVesselDescriptorParams = reference.ParseQuote(descriptorParams,dot);

                        CString surface       = ((numVesselDescriptorParams > 0) ? descriptorParams.GetAt(0) : emptyString);
                        CString componentName = ((numVesselDescriptorParams > 1) ? descriptorParams.GetAt(1) : emptyString);
                        CString firstPinName  = ((numVesselDescriptorParams > 2) ? descriptorParams.GetAt(2) : emptyString);
                        CString subPin        = ((numVesselDescriptorParams > 3) ? descriptorParams.GetAt(3) : emptyString);
                        CString pinNumber     = ((numVesselDescriptorParams > 4) ? descriptorParams.GetAt(4) : emptyString);

                        if (firstPinName.IsEmpty())
                        {
                           firstPinName = "1";
                        }

                        if (subPin.IsEmpty())
                        {
                           subPin = "1";
                        }

                        if (surface.CompareNoCase(surfaceName) == 0)
                        {
                           attributes = vesselData->getDefinedAttributes();

                           CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
                           stencilEntityRule = stencilRuleAttributes.getRule();

                           if (vesselGeometryNumber == padstackVesselGeometry.getBlockNumber())
                           {
                              stencilEntityRule.setEntity(attributeSourcePadstack,surfaceName,componentGeometryName,firstPinName);

                              addItem(stencilEntityRule,*vesselData);
                           }
                           else if (vesselGeometryNumber == subPadstackVesselGeometry.getBlockNumber())
                           {
                              stencilEntityRule.setEntity(attributeSourceSubPadstack,surfaceName,componentGeometryName,firstPinName,subPin);

                              addItem(stencilEntityRule,*vesselData);
                           }
                           else if (vesselGeometryNumber == subPinVesselGeometry.getBlockNumber()        )
                           {
                              stencilEntityRule.setEntity(attributeSourceGeometryPin,surfaceName,componentGeometryName,firstPinName,subPin);

                              addItem(stencilEntityRule,*vesselData);
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      //  --  --  Components  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
      for (CDataListIterator dataIterator(*pcbBlock,insertTypePcbComponent);dataIterator.hasNext();)
      {
         DataStruct* component = dataIterator.getNext();
         InsertStruct* componentInsert = component->getInsert();
         bool topFlag = componentInsert->getPlacedTop();
         CString refDes = componentInsert->getRefname();

         CString vesselRef = ::getSurfaceName(topFlag) + getRefDelimiter() + getDesignatorString(refDes);

         vesselData = &(m_stencilGenerator.getClosetVessel(vesselRef,componentVesselGeometry));
         attributes = vesselData->getDefinedAttributes();

         CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
         stencilEntityRule = stencilRuleAttributes.getRule();
         stencilEntityRule.setEntity(attributeSourceComponent,::getSurfaceName(topFlag),refDes);

         addItem(stencilEntityRule,*vesselData);
      }

      //  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
      for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
      {
         bool topFlag = (surfaceIndex == 0);

         for (POSITION pinPos = m_stencilGenerator.getStencilPins().GetHeadPosition();pinPos != NULL;)
         {
            CStencilPin* stencilPin = m_stencilGenerator.getStencilPins().GetNext(pinPos);

            vesselData = &(stencilPin->getComponentPinVessel(topFlag));
            attributes = vesselData->getDefinedAttributes();

            CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
            stencilEntityRule = stencilRuleAttributes.getRule();
            stencilEntityRule.setEntity(attributeSourceComponentPin,::getSurfaceName(topFlag),stencilPin->getRefDes(),stencilPin->getPinName());

            addItem(stencilEntityRule,*vesselData);

            if (stencilPin->getSubPinCount() > 1)
            {
               for (int subPinIndex = 0;subPinIndex < stencilPin->getSubPinCount();subPinIndex++)
               {
                  CStencilSubPin* subPin = stencilPin->getSubPin(subPinIndex);

                  vesselData = &(subPin->getStencilHole(topFlag).getComponentSubPinVessel());
                  attributes = vesselData->getDefinedAttributes();

                  CStencilRuleAttributes stencilRuleAttributes(camCadDatabase,*attributes);
                  stencilEntityRule = stencilRuleAttributes.getRule();
                  stencilEntityRule.setEntity(attributeSourceComponentSubPin,::getSurfaceName(topFlag),stencilPin->getRefDes(),stencilPin->getPinName(),subPin->getSubPinName());

                  addItem(stencilEntityRule,*vesselData);
               }
            }
         }
      }
   }

   m_pos = m_items.GetHeadPosition();
}

int CStencilEntityIterator::getCount() const
{
   return m_items.getCount();
}

bool CStencilEntityIterator::hasNext() const
{
   return m_pos != NULL;
}

bool CStencilEntityIterator::getNext(CStencilEntityRule& stencilEntityRule,CAttributes*& attributes)
{
   CStencilEntityIteratorItem* item = m_items.GetNext(m_pos);

   stencilEntityRule = item->getStencilEntityRule();
   attributes        = item->getAttributes();

   return m_pos != NULL;
}

CStencilEntityIteratorItem* CStencilEntityIterator::addItem(const CStencilEntityRule& stencilEntityRule,DataStruct& vesselData)
{
   CStencilEntityIteratorItem* item = new CStencilEntityIteratorItem(stencilEntityRule,vesselData);
   m_items.AddTail(item);

   return item;
}

bool CStencilEntityIterator::lookup(const CStencilEntityRule& stencilEntityRule,
   CStencilEntityRule& foundStencilEntityRule,CAttributes*& foundAttributes) const
{
   return m_items.lookup(stencilEntityRule,foundStencilEntityRule,foundAttributes);
}

//_____________________________________________________________________________
CStencilViolation::CStencilViolation(CStencilSubPin& stencilSubPin,bool topFlag)
: m_stencilSubPin(stencilSubPin)
, m_topFlag(topFlag)
{
   m_status = violationStatusOpen;
}

CString CStencilViolation::getSortKey()
{
   CString sortKey;

   sortKey.Format("%s%02d%s",(m_topFlag ? "T" : "b"),(int)getViolationType(),getStencilPin().getSortablePinRef());

   return sortKey;
}

CString CStencilViolation::getPrintableValue(double val)
{
   // The trouble with some "xxx exceeds yyy" reports is that often yyy is printed as .000 when it is
   // non-zero. While it may be well enough to know in a report that ".001 exceeds .000" it might
   // lead a user to wonder why the yyy is zero when they have params set such that it should not be
   // zero. So here we try to format non-zero values so at least one non-zero significant digit shows.
   // Al this really only matters for values between 0 and 1.

   CString retval = ".000";  // possibly it really is zero
   double absval = fabs(val);

   if (absval >= 1.0)
   {
      // Good enough in standard form
      retval.Format("%.3f", val);
   }
   else if (absval > 0.0) // and we already know it is not greater-than-or-equal-to 1.0
   {
      int digits = 3; // minimum sig digits, matches original format %.3f

      // Go until we have a non-zero sig digit
      double shiftedval = absval * 1000.0; // aka pow(10.0, digits)
      while (shiftedval < 1.0 && digits < 11)  // 11 cuts off at 10 decimal places
      {
         digits++;
         shiftedval *= 10.0;  // a little faster than  shiftedval = absval * pow(10.0, digits);
      }

      retval.Format("%.*f", digits, val);
   }

   return retval;
}

//_____________________________________________________________________________
CStencilHoleAreaToWallAreaViolation::CStencilHoleAreaToWallAreaViolation(CStencilSubPin& stencilSubPin,bool topFlag,
   double holeArea,double wallArea,double minHoleAreaToWallAreaRatio)
: CStencilViolation(stencilSubPin,topFlag)
, m_holeArea(holeArea)
, m_wallArea(wallArea)
, m_minHoleAreaToWallAreaRatio(minHoleAreaToWallAreaRatio)
{
}

CString CStencilHoleAreaToWallAreaViolation::getDescription()
{
   CString description;
   double ratio = ((m_wallArea != 0.) ? m_holeArea / m_wallArea : 0.);

   description.Format("Minimum hole to wall area ratio of %.2f exceeds actual ratio of %.2f",
            m_minHoleAreaToWallAreaRatio,ratio);

   return description;
}

//_____________________________________________________________________________
CStencilMaximumFeatureViolation::CStencilMaximumFeatureViolation(CStencilSubPin& stencilSubPin,bool topFlag,
   double featureSize,double maxFeatureSize)
: CStencilViolation(stencilSubPin,topFlag)
, m_featureSize(featureSize)
, m_maxFeatureSize(maxFeatureSize)
{
}

CString CStencilMaximumFeatureViolation::getDescription()
{
   CString description;

   description.Format("Maximum feature size of %.3f exceeded by actual feature size of %.3f",
      m_maxFeatureSize,m_featureSize);

   return description;
}

//_____________________________________________________________________________
CStencilMinimumFeatureViolation::CStencilMinimumFeatureViolation(CStencilSubPin& stencilSubPin,bool topFlag,
   double featureSize,double minFeatureSize)
: CStencilViolation(stencilSubPin,topFlag)
, m_featureSize(featureSize)
, m_minFeatureSize(minFeatureSize)
{
}

CString CStencilMinimumFeatureViolation::getDescription()
{
   CString description;

   description.Format("Minimum feature size of %.3f exceeds actual feature size of %s",
      m_minFeatureSize, getPrintableValue(m_featureSize));

   return description;
}

//_____________________________________________________________________________
CStencilMinimumInsetViolation::CStencilMinimumInsetViolation(CStencilSubPin& stencilSubPin,bool topFlag,
   double minInsetDistance,double minInsetSize)
: CStencilViolation(stencilSubPin,topFlag)
, m_minInsetDistance(minInsetDistance)
, m_minInsetSize(minInsetSize)
{
}

CString CStencilMinimumInsetViolation::getDescription()
{
   CString description;

   // Take care that we don't make a message like ".001 exceeds .000" when .000 is not actually
   // correct, e.g. it is .0005.
   description.Format("Minimum inset size of %.3f exceeds actual minimum inset of %s",
      m_minInsetSize, getPrintableValue(m_minInsetDistance));

   return description;
}

//_____________________________________________________________________________
CStencilSplitPadViolation::CStencilSplitPadViolation(CStencilSubPin& stencilSubPin,bool topFlag,
   double featureSize,double maxFeatureSize)
: CStencilViolation(stencilSubPin,topFlag)
, m_featureSize(featureSize)
, m_maxFeatureSize(maxFeatureSize)
{
   m_status = violationStatusInfo;
}

CString CStencilSplitPadViolation::getDescription()
{
   CString description;

   description.Format("Pad split - maximum feature size of %.3f exceeded by actual feature size of %.3f",
      m_maxFeatureSize,m_featureSize);

   return description;
}

//_____________________________________________________________________________
void CStencilViolations::add(CStencilViolation* stencilViolation)
{
   //CStencilViolation* oldStencilViolation;
   CString key = stencilViolation->getSortKey();

   //if (Lookup(key,oldStencilViolation))
   //{
   //   delete oldStencilViolation;
   //}

   SetAt(key,stencilViolation);
}

bool CStencilViolations::isValid()
{
   bool retval = true;
   CStencilViolation* stencilViolation;
   CString key;

   for (POSITION pos = GetStartPosition();pos != NULL;)
   {
      GetNextAssoc(pos,key,stencilViolation);

      if (! AfxIsValidAddress(stencilViolation,sizeof(CStencilViolation),true))
      {
         retval = false;
      }
   }

   return retval;
}

//_____________________________________________________________________________
CString CStencilPin::getPadDescriptor(DataStruct* pad) const
{
   CString padDescriptor(" ");

   if (pad != NULL)
   {
      BlockStruct* padBlock = getCamCadDatabase().getBlock(pad->getInsert()->getBlockNumber());
      CStandardAperture standardAperture(padBlock,intToPageUnitsTag(getCamCadDatabase().getCamCadDoc().getSettings().getPageUnits()));

      if (standardAperture.isStandardNormalAperture())
      {
         padDescriptor = standardAperture.getDescriptor();
      }
      else
      {
         padDescriptor = padBlock->getName();
      }
   }

   return padDescriptor;
}

int CStencilPin::getKeywordIndex(StencilAttributeTag tagValue)
{
   int keywordIndex = -1;

   switch (tagValue)
   {
   case stencilAttributeTopThickness:              keywordIndex = m_stencilTopThicknessKeywordIndex;              break;
   case stencilAttributeBottomThickness:           keywordIndex = m_stencilBottomThicknessKeywordIndex;           break;
   case stencilAttributeTopMaxFeature:             keywordIndex = m_stencilTopMaxFeatureKeywordIndex;             break;
   case stencilAttributeBottomMaxFeature:          keywordIndex = m_stencilBottomMaxFeatureKeywordIndex;          break;
   case stencilAttributeTopWebWidth:               keywordIndex = m_stencilTopWebWidthKeywordIndex;               break;
   case stencilAttributeTopStencilSourceLayer:     keywordIndex = m_stencilTopStencilSourceLayerKeywordIndex;     break;
   case stencilAttributeBottomStencilSourceLayer:  keywordIndex = m_stencilBottomStencilSourceLayerKeywordIndex;  break;
   case stencilAttributeBottomWebWidth:            keywordIndex = m_stencilBottomWebWidthKeywordIndex;            break;
   case stencilAttributeExteriorCornerRadius:      keywordIndex = m_stencilExteriorCornerRadiusKeywordIndex;      break;
   case stencilAttributeTopPasteInHole:            keywordIndex = m_stencilTopPasteInHoleKeywordIndex;            break;
   case stencilAttributeBottomPasteInHole:         keywordIndex = m_stencilBottomPasteInHoleKeywordIndex;         break;
   case stencilAttributeTopPasteInVia:             keywordIndex = m_stencilTopPasteInViaKeywordIndex;             break;
   case stencilAttributeBottomPasteInVia:          keywordIndex = m_stencilBottomPasteInViaKeywordIndex;          break;
   case stencilAttributePropagateRules:            keywordIndex = m_stencilPropagateRulesKeywordIndex;            break;
   case stencilAttributeUseMountTechAttrib:        keywordIndex = m_stencilUseMountTechAttribKeywordIndex;        break;
   case stencilAttributeMountTechAttribName:       keywordIndex = m_stencilMountTechAttribNameKeywordIndex;       break;
   case stencilAttributeMountTechAttribSMDValue:   keywordIndex = m_stencilMountTechAttribSMDValueKeywordIndex;   break;
   }

   return keywordIndex;
}

PageUnitsTag CStencilPin::getDisplayUnits(PageUnitsTag pageUnits)
{
   switch (pageUnits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
      pageUnits = pageUnitsMils;
      break;
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      pageUnits = pageUnitsMilliMeters;
      break;
   }

   return pageUnits;
}

double CStencilPin::getPageUnitsToDisplayUnitsFactor() const
{
   PageUnitsTag pageUnits = intToPageUnitsTag(getCamCadDatabase().getCamCadDoc().getSettings().getPageUnits());
   PageUnitsTag displayPageUnits = getDisplayUnits(pageUnits);

   double factor = getUnitsFactor(pageUnits,displayPageUnits);

   return factor;
}

CPoly* CStencilPin::getPadPoly(DataStruct* pad)
{
   CPolyList* allocatedPolyList = NULL;
   CPolyList* polyList = NULL;
   CPoly* padPoly = NULL;

   if (pad != NULL)
   {
      if (pad->getDataType() == T_POLY)
      {
         polyList = pad->getPolyList();
      }
      else if (pad->getDataType() == T_INSERT)
      {
         // use first poly from geometry
         BlockStruct* padGeometry = getCamCadDatabase().getBlock(pad->getInsert()->getBlockNumber());

         if (padGeometry->isAperture())
         {
            if (padGeometry->isComplexAperture())
            {
               BlockStruct* apertureGeometry = getCamCadDatabase().getBlock(padGeometry->getComplexApertureSubBlockNumber());
               // ignoring offset for now

               for (CDataListIterator polyIterator(*apertureGeometry,dataTypePoly);polyIterator.hasNext();)
               {
                  DataStruct* polyStruct = polyIterator.getNext();

                  allocatedPolyList = new CPolyList(*(polyStruct->getPolyList()));
                  polyList = allocatedPolyList;

                  break;
               }
            }
            else
            {
               allocatedPolyList = ApertureToPoly(padGeometry,pad->getInsert(),pad->getInsert()->getMirrorFlags());
               polyList = allocatedPolyList;
            }
         }
         else
         {
            for (POSITION dataPos = padGeometry->getHeadDataPosition();dataPos != NULL;)
            {
               DataStruct* polyData = padGeometry->getNextData(dataPos);

               if (polyData != NULL && polyData->getDataType() == T_POLY)
               {
                  polyList = polyData->getPolyList();
                  break;
               }
            }
         }
      }

      if (polyList != NULL)
      {
         for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList->GetNext(polyPos);

            if (poly != NULL)
            {
               padPoly = new CPoly(*poly);
               break;
            }
         }
      }
   }

   delete allocatedPolyList;

   return padPoly;
}


//_____________________________________________________________________________
CStencilGeometryMicrocosm::CStencilGeometryMicrocosm(CStencilGenerator& stencilGenerator)
: m_stencilGenerator(stencilGenerator)
, m_topStencilPads(true)
, m_bottomStencilPads(true)
, m_refDes("U1")
{
   m_microcosmFile       = NULL;
   m_microcosmComponent  = NULL;
   m_microcosmGeometry   = NULL;
   m_sourceStencilSubPin = NULL;
   m_sourceTopFlag       = true;
   m_ruleStatus          = ruleStatusOk;
}

CStencilGeometryMicrocosm::~CStencilGeometryMicrocosm()
{
   destroyMicrocosmComponent();
   destroyMicrocosmGeometry();
}

void CStencilGeometryMicrocosm::destroyMicrocosmComponent()
{
   if (m_microcosmComponent != NULL)
   {
      RemoveOneEntityFromDataList(&(getCamCadDatabase().getCamCadDoc()),&(getDefinedMicrocosmFile().getBlock()->getDataList()),m_microcosmComponent);
      m_microcosmComponent = NULL;
   }
}

void CStencilGeometryMicrocosm::destroyMicrocosmGeometry()
{
   if (m_microcosmGeometry != NULL)
   {
      m_microcosmGeometry->getDataList().empty();
   }
}

void CStencilGeometryMicrocosm::emptyStencilEntities()
{
   m_topStencilPads.empty();
   m_bottomStencilPads.empty();
   m_stencilPins.empty();
   //getCamCadDatabase().discardCamCadPins(getDefinedMicrocosmFile());
}

void CStencilGeometryMicrocosm::createMicrocosmGeometry(BlockStruct& sourceGeometry,FileStruct& sourceFile)
{
   //CString geometryName(sourceGeometry.getName() + "-" + getFileName());
   CString geometryName(sourceGeometry.getName());

   destroyMicrocosmGeometry();
   m_microcosmGeometry = getCamCadDatabase().copyBlock(geometryName,*getMicrocosmFile(),&sourceGeometry);

   CAttributes*& microcosmFileAttributes = getDefinedMicrocosmFile().getBlock()->getDefinedAttributes();
   microcosmFileAttributes->empty();

   CAttributeMapWrap attributeMapWrap(microcosmFileAttributes);
   attributeMapWrap.copyFrom(sourceFile.getBlock()->getDefinedAttributes());
}

void CStencilGeometryMicrocosm::createMicrocosmComponent(DataStruct* sourceComponent,FileStruct* sourceFile)
{
   int layerIndex = -1;

   CString refDes;

   if (sourceComponent != NULL)
   {
      refDes = sourceComponent->getInsert()->getRefname();
      setRefDes(refDes);
   }

   destroyMicrocosmComponent();
   //m_microcosmComponent = getCamCadDatabase().referenceBlock(getDefinedMicrocosmFile().getBlock(),m_microcosmGeometry,insertTypePcbComponent,refDes,layerIndex);

   // knv - experimental fix
   bool mirrorFlag = sourceComponent->getInsert()->getGraphicMirrored();
   m_microcosmComponent = getCamCadDatabase().referenceBlock(getDefinedMicrocosmFile().getBlock(),m_microcosmGeometry,insertTypePcbComponent,refDes,layerIndex,0.,0.,0.,mirrorFlag);

   copyStencilGeneratorClosetInfo(sourceComponent->getInsert()->getPlacedTop(),refDes);

}

void CStencilGeometryMicrocosm::copyStencilGeneratorClosetInfo(bool topFlag,const CString& refDes)
{
}

void CStencilGeometryMicrocosm::setGeometry(BlockStruct& sourceGeometry,
   FileStruct& sourceFile,CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,
   AttributeSourceTag attributeSource)
{
   m_sourceStencilSubPin = &sourceStencilSubPin;
   m_sourceTopFlag       = sourceTopFlag;
   m_attributeSource     = attributeSource;

   emptyStencilEntities();
   getDefinedMicrocosmFile().getBlock()->getDataList().empty();

   createMicrocosmGeometry(sourceGeometry,sourceFile);
   createMicrocosmComponent(&(sourceStencilSubPin.getComponent()),&sourceFile);
}

void CStencilGeometryMicrocosm::setComponent(DataStruct& sourceComponent,
   FileStruct& sourceFile,CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,
   AttributeSourceTag attributeSource)
{
   m_sourceStencilSubPin = &sourceStencilSubPin;
   m_sourceTopFlag       = sourceTopFlag;
   m_attributeSource     = attributeSource;

   emptyStencilEntities();
   getDefinedMicrocosmFile().getBlock()->getDataList().empty();

   BlockStruct* sourceGeometry = getCamCadDatabase().getBlock(sourceComponent.getInsert()->getBlockNumber());

   createMicrocosmGeometry(*sourceGeometry,sourceFile);
   createMicrocosmComponent(&sourceComponent,&sourceFile);
}

FileStruct* CStencilGeometryMicrocosm::getMicrocosmFile() const
{
   return m_microcosmFile;
}

FileStruct& CStencilGeometryMicrocosm::getDefinedMicrocosmFile()
{
   if (m_microcosmFile == NULL)
   {
      CCEtoODBDoc& camCadDoc = getCamCadDatabase().getCamCadDoc();
      CFileList& fileList = camCadDoc.getFileList();

      for (POSITION pos = fileList.GetHeadPosition();pos != NULL;)
      {
         FileStruct* file = fileList.GetNext(pos);

         if (file->getName() == getFileName())
         {
            m_microcosmFile = file;
            break;
         }
      }

      if (m_microcosmFile == NULL)
      {
         m_microcosmFile = camCadDoc.Add_File(getFileName(),fileTypeUnknown);
      }
   }

   return *m_microcosmFile;
}

CCamCadDatabase& CStencilGeometryMicrocosm::getCamCadDatabase()
{
   return m_stencilGenerator.getCamCadDatabase();
}

CCamCadData& CStencilGeometryMicrocosm::getCamCadData()
{
   return getCamCadDatabase().getCamCadData();
}

CStencilGenerationParameters& CStencilGeometryMicrocosm::getStencilGenerationParameters()
{
   return m_stencilGenerator.getStencilGenerationParameters();
}

CStencilPins& CStencilGeometryMicrocosm::getStencilPins()
{
   return m_stencilPins;
}

DataStruct* CStencilGeometryMicrocosm::getComponent()
{
   return m_microcosmComponent;
}

BlockStruct* CStencilGeometryMicrocosm::getGeometry()
{
   return m_microcosmGeometry;
}

//CStencilPin* CStencilGeometryMicrocosm::getSourceStencilPin()
//{
//   return m_sourceStencilPin;
//}

CStencilSubPin* CStencilGeometryMicrocosm::getSourceStencilSubPin()
{
   return m_sourceStencilSubPin;
}

bool CStencilGeometryMicrocosm::getSourceTopFlag()
{
   return m_sourceTopFlag;
}

//int CStencilGeometryMicrocosm::getSourceHoleIndex() const
//{
//   return m_holeIndex;
//}

AttributeSourceTag CStencilGeometryMicrocosm::getAttributeSource()
{
   return m_attributeSource;
}

StencilRuleStatusTag CStencilGeometryMicrocosm::getRuleStatus() const
{
   return m_ruleStatus;
}

void CStencilGeometryMicrocosm::setRuleStatus(StencilRuleStatusTag status)
{
   m_ruleStatus = status;
}

CString CStencilGeometryMicrocosm::getFileName()
{
   return "StencilGeometryMicrocosm";
}

CString CStencilGeometryMicrocosm::getRefDes() const
{
   return m_refDes;
}

void CStencilGeometryMicrocosm::setRefDes(const CString& refDes)
{
   m_refDes = refDes;
}

void CStencilGeometryMicrocosm::getStencilPads(CDataList& destinationStencilPads,bool topFlag)
{
   CDataList& stencilPads = (topFlag ? m_topStencilPads : m_bottomStencilPads);

   if (m_stencilPins.GetCount() == 0)
   {
      m_stencilPins.addStencilEntities(m_stencilGenerator,getDefinedMicrocosmFile());
      m_stencilPins.calcStencilHoles(m_topStencilPads,m_bottomStencilPads,getCamCadDatabase(),getStencilGenerationParameters(),getDefinedMicrocosmFile());
   }

   destinationStencilPads.setEqual(stencilPads,getCamCadDatabase().getCamCadData());
}

void CStencilGeometryMicrocosm::resetRuleStatus()
{
   m_ruleStatus = ruleStatusOk;
}

void CStencilGeometryMicrocosm::updateRuleStatus(StencilRuleStatusTag status)
{
   if (status > m_ruleStatus)
   {
      m_ruleStatus = status;
   }
}





