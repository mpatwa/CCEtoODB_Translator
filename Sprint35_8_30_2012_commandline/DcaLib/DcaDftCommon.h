// $Header: /CAMCAD/5.0/Dca/DcaDftCommon.h 1     3/19/07 4:28a Kurt Van Ness $

#if !defined(__DcaDftCommon_h__)
#define __DcaDftCommon_h__

#pragma once

#include "DcaContainer.h"

class CPoint2d;
class CSize2d;

CString SwapSpecialCharacters(CString string);     // used for writing xml files, defined in Xml_wrt.h

/* Define Section *********************************************************/

#define DFT_ERROR_NONE                       0
#define DEF_ERROR_COMPPIN_LOCATION           -1


#define DFT_OUTLINE_NONE                     -1
#define DFT_OUTLINE_REALPART                 0
#define DFT_OUTLINE_DFT                      1
#define DFT_OUTLINE_BOTH                     2

#define DFT_OUTLINE_REALPART_PRIORITY        0
#define DFT_OUTLINE_DFT_PRIORITY             1

#define DFT_SURFACE_TOP                      0
#define DFT_SURFACE_BOT                      1
#define DFT_SURFACE_BOTH                     2
#define DFT_SURFACE_MAX                      3

#define DFT_TESTACCESS_NONE                  "NONE"
#define DFT_TESTACCESS_TOP                   "TOP"
#define DFT_TESTACCESS_BOTTOM                "BOTTOM"
#define DFT_TESTACCESS_BOTH                  "BOTH"

#define DFT_TESTER_FIXTURE                   0
#define DFT_TESTER_FIXTURELESS               1

#define DFT_OUTLINE_TOP                      "DFT_OUTLINE_TOP"
#define DFT_OUTLINE_BOTTOM                   "DFT_OUTLINE_BOTTOM"
#define DFT_LAYER_COMBIN_METAL_TOP           "Combined Metal Top"
#define DFT_LAYER_COMBIN_METAL_BOT           "Combined Metal Bottom"
#define DFT_LAYER_COMBIN_SOLDERMASK_TOP      "Combined Soldermask Top"
#define DFT_LAYER_COMBIN_SOLDERMASK_BOT      "Combined Soldermask Bottom"
#define DFT_LAYER_SM_ANALYSYS_TOP            "Soldermask Analysis Top"
#define DFT_LAYER_SM_ANALYSYS_BOT            "Soldermask Analysis Bot"
#define DFT_LAYER_EXPOSE_METAL_TOP           "Exposed Metal Top"
#define DFT_LAYER_EXPOSE_METAL_BOT           "Exposed Metal Bottom"
#define DFT_LAYER_T_OUTLINE_TOP              "T_OUTLINE_TOP"
#define DFT_LAYER_T_OUTLINE_BOT              "T_OUTLINE_BOT"
#define DFT_LAYER_FEATURE_SIZE               "Feature Size Layer"

#define DFT_LAY_EXPOSE_METAL_ATTRIB          "Expose Metal"
#define DFT_LAY_EXPOSE_METAL_VALUE_TOP       "Top"          
#define DFT_LAY_EXPOSE_METAL_VALUE_BOT       "Bottom"    

#define DFT_DEFAULT_UNIT                     pageUnitsInches
#define DFT_MIN_DRILL_SIZE                   0.011    // Minimum drill size is in DFT_DEFAULT_UNIT and is for determing is a padstack is through hole

#define DFT_ATT_TARGET_TYPE                  "TARGET_TYPE"
#define DFT_ATT_EXPOSE_METAL_DIAMETER        "EXPOSE_METAL_DIAMETER"

#define DFT_GENERIC_SOLUTION_NAME            "<Generic Solution>"

// These are different targettype flags
#define DFT_TARGETTYPE_UNKNOWN               0
#define DFT_TARGETTYPE_TESTATTRIBUTE         1
#define DFT_TARGETTYPE_VIA                   2
#define DFT_TARGETTYPE_CONNECTOR             4
#define DFT_TARGETTYPE_SMD                   8     // MULTI-pin SMD
#define DFT_TARGETTYPE_THRUHOLE              16
#define DFT_TARGETTYPE_CADPADSTACK           32
#define DFT_TARGETTYPE_SINGLEPINSMD          64

//_____________________________________________________________________________
/* AA_RC = Access Analysis _ Reason Code                                  */

enum EAAReasonCode
{
   DFT_AA_RC_NONE                 =  0,
   DFT_AA_RC_NO_MULTI_PIN_NET     =  1,   //   "Multiple Pin Net not enabled"
   DFT_AA_RC_NO_UNCONNECTED_NET   =  2,   //   "Nonconnected Net not enabled"
   DFT_AA_RC_NO_SINGLE_PIN_NET    =  3,   //   "Single Pin Net not enabled"
   DFT_AA_RC_THRU_NOT_ALLOW       =  4,   //   "Thru-hole feature not allowed on mounted side using fixture tester type"
   DFT_AA_RC_NO_TARGET_TYPE       =  5,   //   "Target type not enabled"
   DFT_AA_RC_SMALL_METAL          =  6,   //   "Exposed metal (%0.3f) is smaller than feature size (%0.3f)"
   DFT_AA_RC_COMP_OUTLINE         =  7,   //   "Feature violated by Component Outline"
   DFT_AA_RC_BOARD_OUTLINE        =  8,   //   "Feature to Board Outline Distance violated minimum (%0.3f)"
   DFT_AA_RC_NO_SOLDERMASK        =  9,   //   "No exposed metal due to Soldermask Analysis"
   DFT_AA_RC_NO_METAL             = 10,   //   "No metal exists"
   DFT_AA_RC_NO_SURFACE           = 11,   //   "Surface is not enabled"
   DFT_AA_RC_MISSING_COMP         = 12,   //   "Component could not be found in Design"
   DFT_AA_RC_MISSING_COMPPIN      = 13,   //   "Component pin could not be found in Design"
   DFT_AA_RC_SINGLE_PIN_SMD       = 14,    //   "Single Pin SMD Component not analyzed"
   DFT_AA_RC_SMALL_METAL_BEAD_PROBE    =  15   //   "Exposed metal (%0.3f) is smaller than Bead Probe size (%0.3f)"
};

//_____________________________________________________________________________
enum EOutlineUse
{
   outlineUseNone = DFT_OUTLINE_NONE,
   outlineUseRealPart = DFT_OUTLINE_REALPART,
   outlineUseDftOutline = DFT_OUTLINE_DFT,
   outlineUseBoth = DFT_OUTLINE_BOTH,
};

//_____________________________________________________________________________
enum EOutlinePriority
{
   outlinePriorityRealPart = DFT_OUTLINE_REALPART_PRIORITY,
   outlinePriorityDftOutline = DFT_OUTLINE_DFT_PRIORITY,
};

//_____________________________________________________________________________
enum ETestSurface
{
   testSurfaceUnset  = -1,
   testSurfaceMIN    = DFT_SURFACE_TOP,
   testSurfaceTop    = DFT_SURFACE_TOP,
   testSurfaceBottom = DFT_SURFACE_BOT,
   testSurfaceBoth   = DFT_SURFACE_BOTH,
   testSurfaceMAX    = DFT_SURFACE_MAX,
};

CString testSurfaceTagToString(ETestSurface tagValue);

//_____________________________________________________________________________
enum ETesterType
{
   testerTypeFixture = DFT_TESTER_FIXTURE,
   testerTypeFixtureless = DFT_TESTER_FIXTURELESS,
};

//_____________________________________________________________________________
enum EIncludedNetType
{
   includeUnknown             = -1,
   includeMultiPinNet         = 0,
   includeSinglePinNet        = 1,
   includeUnconnecedNet       = 2,
};

//_____________________________________________________________________________
enum ECorner
{
   cornerLL = 0,  // lower left
   cornerCL = 1,  // center left
   cornerUL = 2,  // upper left
   cornerLC = 3,  // lower center
   cornerCC = 4,  // center center
   cornerUC = 5,  // upper center
   cornerLR = 6,  // lower right
   cornerCR = 7,  // center right
   cornerUR = 8,  // upper right
};

//_____________________________________________________________________________
enum AccessOffsetDirectionTag
{
   accessOffsetDirectionWest   = 0,
   accessOffsetDirectionNorth  = 1,
   accessOffsetDirectionEast   = 2,
   accessOffsetDirectionSouth  = 3,
   accessOffsetDirectionUndefined ,
   AccessOffsetDirectionsTagMin = accessOffsetDirectionWest,
   AccessOffsetDirectionsTagMax = accessOffsetDirectionSouth
};

CString accessOffsetDirectionsTagToString(AccessOffsetDirectionTag tagValue);
CString accessOffsetDirectionsTagToAbbreviatedString(AccessOffsetDirectionTag tagValue);
AccessOffsetDirectionTag stringToAccessOffsetDirectionsTag(const CString& tagValue);
AccessOffsetDirectionTag calcAccessOffsetDirection(const CPoint2d& originPoint,const CPoint2d& offsetPoint,double tolerance);

//_____________________________________________________________________________
enum AccessItemTypeTag
{
   accessItemTypePin = 0,
   accessItemTypeVia = 1,
   accessItemTypeTp  = 2
};

CString accessItemTypeTagToString(AccessItemTypeTag tagValue);

//_____________________________________________________________________________
enum AccessOffsetPlacementTag
{                                         // http://en.wikipedia.org/wiki/Anatomical_terms_of_location
   accessOffsetPlacementProximal   = 0,
   accessOffsetPlacementMedial     = 1,
   accessOffsetPlacementDistal     = 2,
   accessOffsetPlacementUndefined  = 3,
   AccessOffsetPlacementTagMin     = 0,
   AccessOffsetPlacementTagMax     = 2
};

CString accessOffsetPlacementTagToString(AccessOffsetPlacementTag tagValue);
CString accessOffsetPlacementTagToSimpleString(AccessOffsetPlacementTag tagValue);
AccessOffsetPlacementTag stringToAccessOffsetPlacementTag(const CString& tagValue);

//_____________________________________________________________________________
enum EProbeShape
{
   probeShapeDefaultTarget = 0,
   probeShapeMIN           = 0,
   probeShapeRoundCross    = 1,
   probeShapeRoundX        = 2,
   probeShapeSquareCross   = 3,
   probeShapeSquareX       = 4,
   probeShapeDiamondCross  = 5,
   probeShapeDiamondX      = 6,
   probeShapeHexagonCross  = 7,
   probeShapeHexagonX      = 8,
   probeShapeHourglass     = 9,
   probeShapeMAX           = 10,
};

CString GetProbeShapeName(EProbeShape shape);

//_____________________________________________________________________________
enum EProbeUsage
{
   probeUsageUseExisting   = 0,
   probeUsageAddAdditional = 1,
};

//_____________________________________________________________________________
enum TestProbeSequenceSurface
{
   tpssTop = 0,
   tpssBottom = 1,
   tpssUnplaced = 2,
};

//_____________________________________________________________________________
enum eProbeableNetType
{
   probeableNetTypePower = 0,
   probeableNetTypeGround = 1,
   probeableNetTypeSignal = 2,
};

//_____________________________________________________________________________
enum ETestResourceType
{
   testResourceTypeMIN = 0,
   testResourceTypeTest = 0,
   testResourceTypePowerInjection = 1,
   testResourceTypeMAX = 2,
};

//_____________________________________________________________________________
enum EFeatureCondition
{
   featureConditionMIN = 0,
   featureConditionProbeable = 0,
   featureConditionForced = 1,
   featureConditionNoProbe = 2,
   featureConditionMAX = 3,
};

//_____________________________________________________________________________

class CAccessFailureReason : public CObject
{
private:
   EAAReasonCode m_rc;    // Reason code
   CString m_param1 ;     // Param 1 varies as needed for rc
   CString m_param2;      // Likewise.
   double m_dparam1;      // Likewise.
   double m_dparam2;      // Likewise.

public:
   CAccessFailureReason()                                                   { m_rc = DFT_AA_RC_NONE;                            m_dparam1 = m_dparam2 = 0.; }
   CAccessFailureReason(EAAReasonCode rc)                                   { m_rc = rc;                                        m_dparam1 = m_dparam2 = 0.; }
   CAccessFailureReason(EAAReasonCode rc, CString param1)                   { m_rc = rc; m_param1 = param1;                     m_dparam1 = m_dparam2 = 0.; }
   CAccessFailureReason(EAAReasonCode rc, CString param1, CString param2)   { m_rc = rc; m_param1 = param1; m_param2 = param2;  m_dparam1 = m_dparam2 = 0.; }
   CAccessFailureReason(EAAReasonCode rc, double param1)                    { m_rc = rc; m_dparam1 = param1;                                m_dparam2 = 0.;}
   CAccessFailureReason(EAAReasonCode rc, double param1, double param2)     { m_rc = rc; m_dparam1 = param1; m_dparam2 = param2;}
   CAccessFailureReason(CAccessFailureReason& other)                        { *this = other; }
   
   CAccessFailureReason& operator=(CAccessFailureReason& other)
   {
      this->m_rc      = other.m_rc;
      this->m_param1  = other.m_param1;
      this->m_param2  = other.m_param2;
      this->m_dparam1 = other.m_dparam1;
      this->m_dparam2 = other.m_dparam2;

      return *this;
   }

   EAAReasonCode GetReasonCode() { return m_rc; }
   CString GetParam1()           { return m_param1; }
   CString GetParam2()           { return m_param2; }
   double GetDParam1()           { return m_dparam1; }
   double GetDParam2()           { return m_dparam2; }

   void SetReasonCode(EAAReasonCode rc)                                  { m_rc = rc; } 
   void SetReasonCode(EAAReasonCode rc, CString& p1, CString& p2)        { m_rc = rc; m_param1  = p1; m_param2  = p2; }
   void SetReasonCode(EAAReasonCode rc, double  p1, double  p2 = -1.)    { m_rc = rc; m_dparam1 = p1; m_dparam2 = p2; }

   void Reset()      { m_rc = DFT_AA_RC_NONE; m_param1.Empty(); m_param2.Empty();  m_dparam1 = m_dparam2 = 0.; }
   bool IsEmpty()    { return m_rc == DFT_AA_RC_NONE; }


   CString GetMsgStr();
};

//_____________________________________________________________________________

class CAccessFailureReasonList : public CTypedMapWordToObContainer<CAccessFailureReason*>
{
   // As of this writing, a given access point may fail for multiple reasons, but
   // those a multiple unique reasons. I.e. there are no duplicate reasons.
   // If it should develop that multiple instances of same reason (say, with some 
   // different detail parameter) need to be kept track of, this can be saved
   // to a collection based class.

private:


public:
#ifdef USE_INLINE
   void AddRC(EAAReasonCode rc)                                   { this->setAt(rc, new CAccessFailureReason(rc));                 }
   void AddRC(EAAReasonCode rc, CString param1)                   { this->setAt(rc, new CAccessFailureReason(rc, param1));         }
   void AddRC(EAAReasonCode rc, CString param1, CString param2)   { this->setAt(rc, new CAccessFailureReason(rc, param1,param2));  }
   void AddRC(EAAReasonCode rc, double param1)                    { this->setAt(rc, new CAccessFailureReason(rc, param1));         }
   void AddRC(EAAReasonCode rc, double param1, double param2)     { this->setAt(rc, new CAccessFailureReason(rc, param1, param2)); }
   void AddRC(CAccessFailureReason& other)  { if (other.GetReasonCode() != DFT_AA_RC_NONE) { this->setAt(other.GetReasonCode(), new CAccessFailureReason(other)); } }
   void AddRC(EAAReasonCode rc, CString param1, CString param2, double dparam1, double dparam2);     
#else
   // for debugging
   void AddRC(EAAReasonCode rc);//                                   { this->setAt(rc, new CAccessFailureReason(rc));                 }
   void AddRC(EAAReasonCode rc, CString param1);//                   { this->setAt(rc, new CAccessFailureReason(rc, param1));         }
   void AddRC(EAAReasonCode rc, CString param1, CString param2);//   { this->setAt(rc, new CAccessFailureReason(rc, param1,param2));  }
   void AddRC(EAAReasonCode rc, double param1);//                    { this->setAt(rc, new CAccessFailureReason(rc, param1));         }
   void AddRC(EAAReasonCode rc, double param1, double param2);//     { this->setAt(rc, new CAccessFailureReason(rc, param1, param2)); }
   void AddRC(CAccessFailureReason& other);//  { if (other.GetReasonCode() != DFT_AA_RC_NONE) { this->setAt(other.GetReasonCode(), new CAccessFailureReason(other)); } }
   void AddRC(EAAReasonCode rc, CString param1, CString param2, double dparam1, double dparam2);     

#endif

   void Reset();
   bool HasRC(EAAReasonCode rc)      { CAccessFailureReason *ignored; return this->Lookup(rc, ignored) != NULL; }
   bool ContainsFatalAccessError();

   void CopyRCList(const CAccessFailureReasonList& other);   // copies other to this, leaves what is already in this
   void CloneRCList(const CAccessFailureReasonList& other);  // clears this first, then copies other


};

//_____________________________________________________________________________


#endif
