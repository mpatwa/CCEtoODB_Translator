// $Header: /CAMCAD/5.0/Dca/DcaDftCommon.cpp 1     3/19/07 4:28a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaDftCommon.h"
#include "DcaPoint2d.h"

//_____________________________________________________________________________
CString GetProbeShapeName(EProbeShape shape)
{
   switch (shape)
   {
   case probeShapeDefaultTarget:
      return "Target";
   case probeShapeRoundCross:
      return "Round Cross";
   case probeShapeRoundX:
      return "Round X";
   case probeShapeSquareCross:
      return "Square Cross";
   case probeShapeSquareX:
      return "Square X";
   case probeShapeDiamondCross:
      return "Diamond Cross";
   case probeShapeDiamondX:
      return "Diamond X";
   case probeShapeHexagonCross:
      return "Round Cross";
   case probeShapeHexagonX:
      return "Hexagon X";
   case probeShapeHourglass:
      return "Hour Glass";
   }
   return "";
}

//_____________________________________________________________________________
CString testSurfaceTagToString(ETestSurface tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case testSurfaceTop:     stringValue = "Top";   break;
   case testSurfaceBottom:  stringValue = "Bottom";  break;
   case testSurfaceBoth:    stringValue = "Both";   break;
   }

   return stringValue;
}

//_____________________________________________________________________________
CString accessOffsetDirectionsTagToString(AccessOffsetDirectionTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case accessOffsetDirectionWest:   stringValue = "West";   break;
   case accessOffsetDirectionNorth:  stringValue = "North";  break;
   case accessOffsetDirectionEast:   stringValue = "East";   break;
   case accessOffsetDirectionSouth:  stringValue = "South";  break;
   }

   return stringValue;
}

CString accessOffsetDirectionsTagToAbbreviatedString(AccessOffsetDirectionTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case accessOffsetDirectionWest:       stringValue = "W";  break;
   case accessOffsetDirectionNorth:      stringValue = "N";  break;
   case accessOffsetDirectionEast:       stringValue = "E";  break;
   case accessOffsetDirectionSouth:      stringValue = "S";  break;
   case accessOffsetDirectionUndefined:  stringValue = "A";  break;
   }

   return stringValue;
}

AccessOffsetDirectionTag stringToAccessOffsetDirectionsTag(const CString& tagValue)
{
   AccessOffsetDirectionTag retval = accessOffsetDirectionUndefined;

   if (tagValue.CompareNoCase(accessOffsetDirectionsTagToString(accessOffsetDirectionWest)) == 0 ||
       tagValue.CompareNoCase(accessOffsetDirectionsTagToAbbreviatedString(accessOffsetDirectionWest)) == 0)
   {
      retval = accessOffsetDirectionWest;
   }
   else if (tagValue.CompareNoCase(accessOffsetDirectionsTagToString(accessOffsetDirectionNorth)) == 0 ||
            tagValue.CompareNoCase(accessOffsetDirectionsTagToAbbreviatedString(accessOffsetDirectionNorth)) == 0)
   {
      retval = accessOffsetDirectionNorth;
   }
   else if (tagValue.CompareNoCase(accessOffsetDirectionsTagToString(accessOffsetDirectionEast)) == 0 ||
            tagValue.CompareNoCase(accessOffsetDirectionsTagToAbbreviatedString(accessOffsetDirectionEast)) == 0)
   {
      retval = accessOffsetDirectionEast;
   }
   else if (tagValue.CompareNoCase(accessOffsetDirectionsTagToString(accessOffsetDirectionSouth)) == 0 ||
            tagValue.CompareNoCase(accessOffsetDirectionsTagToAbbreviatedString(accessOffsetDirectionSouth)) == 0)
   {
      retval = accessOffsetDirectionSouth;
   }

   return retval;
}

AccessOffsetDirectionTag calcAccessOffsetDirection(const CPoint2d& originPoint,const CPoint2d& offsetPoint,double tolerance)
{
   AccessOffsetDirectionTag direction = accessOffsetDirectionUndefined;

   CSize2d delta = offsetPoint - originPoint;

   bool verticalFlag   = fpnear(delta.cx,0.,tolerance);
   bool horizontalFlag = fpnear(delta.cy,0.,tolerance);

   if (verticalFlag != horizontalFlag)
   {
      if (verticalFlag)
      {
         direction = ((delta.cy > 0.) ? accessOffsetDirectionNorth : accessOffsetDirectionSouth);
      }
      else
      {
         direction = ((delta.cx > 0.) ? accessOffsetDirectionEast : accessOffsetDirectionWest);
      }
   }

   return direction;
}

//_____________________________________________________________________________
CString accessItemTypeTagToString(AccessItemTypeTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case accessItemTypePin:   stringValue = "Pin";   break;
   case accessItemTypeVia:   stringValue = "Via";   break;
   case accessItemTypeTp:    stringValue = "TP";    break;
   }

   return stringValue;
}

//_____________________________________________________________________________
CString accessOffsetPlacementTagToString(AccessOffsetPlacementTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case accessOffsetPlacementProximal:  stringValue = "Proximal";  break;
   case accessOffsetPlacementMedial:    stringValue = "Medial";    break;
   case accessOffsetPlacementDistal:    stringValue = "Distal";    break;
   }

   return stringValue;
}

CString accessOffsetPlacementTagToSimpleString(AccessOffsetPlacementTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case accessOffsetPlacementProximal:  stringValue = "Minimum";   break;
   case accessOffsetPlacementMedial:    stringValue = "Centered";  break;
   case accessOffsetPlacementDistal:    stringValue = "Maximum";   break;
   }

   return stringValue;
}

AccessOffsetPlacementTag stringToAccessOffsetPlacementTag(const CString& tagValue)
{
   AccessOffsetPlacementTag retval = accessOffsetPlacementUndefined;

   if (accessOffsetPlacementTagToSimpleString(accessOffsetPlacementProximal).CompareNoCase(tagValue) == 0 ||
       accessOffsetPlacementTagToString(accessOffsetPlacementProximal).CompareNoCase(tagValue)       == 0    )
   {
      retval = accessOffsetPlacementProximal;
   }
   else if (accessOffsetPlacementTagToSimpleString(accessOffsetPlacementMedial).CompareNoCase(tagValue) == 0 ||
            accessOffsetPlacementTagToString(accessOffsetPlacementMedial).CompareNoCase(tagValue)     == 0    )
   {
      retval = accessOffsetPlacementMedial;
   }
   else if (accessOffsetPlacementTagToSimpleString(accessOffsetPlacementDistal).CompareNoCase(tagValue) == 0 ||
            accessOffsetPlacementTagToString(accessOffsetPlacementDistal).CompareNoCase(tagValue)     == 0    )
   {
      retval = accessOffsetPlacementDistal;
   }

   return retval;
}

//---------------------------------------------------------------------------------------------

CString CAccessFailureReason::GetMsgStr()
{
   CString msg;

   switch (this->m_rc)
   {
   case DFT_AA_RC_NONE:
      /* msg is already blank */
      break;

   case DFT_AA_RC_NO_MULTI_PIN_NET:
      msg.Format("Multiple Pin Net not enabled");
      break;

   case DFT_AA_RC_NO_UNCONNECTED_NET:
      msg.Format("Nonconnected Net not enabled");
      break;

   case DFT_AA_RC_NO_SINGLE_PIN_NET:
      msg.Format("Single Pin Net not enabled");
      break;

   case DFT_AA_RC_THRU_NOT_ALLOW:
      msg.Format("Thru-hole feature not allowed on mounted side using fixture tester type");
      break;

   case DFT_AA_RC_NO_TARGET_TYPE:
      msg.Format("Target type not enabled");
      break;

   case DFT_AA_RC_SMALL_METAL:
      msg.Format("Exposed metal (%0.3f) is smaller than minimum feature size (%0.3f)", m_dparam1, m_dparam2);
      break;

   case DFT_AA_RC_SMALL_METAL_BEAD_PROBE:
      msg.Format("Exposed metal (%0.3f) is smaller than minimum Bead Probe size (%0.3f)", m_dparam1, m_dparam2);
      break;

   case DFT_AA_RC_COMP_OUTLINE:
      if (m_param1.IsEmpty())
         msg.Format("Feature violated by Component Outline");
      else
         msg.Format("Feature violated by Component Outline%s (%s)", m_param1.Find(",") > -1 ? "s" : "", m_param1);
      break;

   case DFT_AA_RC_BOARD_OUTLINE:
      msg.Format("Feature to Board Outline Distance violated minimum (%0.3f)", m_dparam1);
      break;

   case DFT_AA_RC_NO_SOLDERMASK:
      msg.Format("No exposed metal due to Soldermask Analysis");
      break;

   case DFT_AA_RC_NO_METAL:
      msg.Format("No metal exists");
      break;

   case DFT_AA_RC_NO_SURFACE:
      msg.Format("Surface is not enabled");
      break;

   case DFT_AA_RC_MISSING_COMP:
      msg.Format("Component could not be found in Design");
      break;

   case DFT_AA_RC_MISSING_COMPPIN:
      msg.Format("Component pin could not be found in Design");
      break;

   case DFT_AA_RC_SINGLE_PIN_SMD:
      msg.Format("Single Pin SMD Component not analyzed");
      break;

   }

   return msg;
}

//-------------------------------------------------------------------------------

void CAccessFailureReasonList::AddRC(EAAReasonCode rc)
{ 
   this->setAt(rc, new CAccessFailureReason(rc));
}

void CAccessFailureReasonList::AddRC(EAAReasonCode rc, CString param1)
{ 
   this->setAt(rc, new CAccessFailureReason(rc, param1)); 
}

void CAccessFailureReasonList::AddRC(EAAReasonCode rc, CString param1, CString param2)  
{ 
   this->setAt(rc, new CAccessFailureReason(rc, param1,param2));
}

void CAccessFailureReasonList::AddRC(EAAReasonCode rc, double param1) 
{
   this->setAt(rc, new CAccessFailureReason(rc, param1));   
}

void CAccessFailureReasonList::AddRC(EAAReasonCode rc, double param1, double param2) 
{ 
   this->setAt(rc, new CAccessFailureReason(rc, param1, param2));
}

void CAccessFailureReasonList::AddRC(CAccessFailureReason& other) 
{
   if (other.GetReasonCode() != DFT_AA_RC_NONE) 
   { 
      this->setAt(other.GetReasonCode(), new CAccessFailureReason(other));
   }
}

void CAccessFailureReasonList::AddRC(EAAReasonCode rc, CString param1, CString param2, double dparam1, double dparam2) 
{ 
   switch(rc)
   {
   case DFT_AA_RC_NONE:
      /* msg is already blank */
      break;

   case DFT_AA_RC_NO_MULTI_PIN_NET:
   case DFT_AA_RC_NO_UNCONNECTED_NET:
   case DFT_AA_RC_NO_SINGLE_PIN_NET:
   case DFT_AA_RC_THRU_NOT_ALLOW:
   case DFT_AA_RC_NO_TARGET_TYPE:
   case DFT_AA_RC_NO_SOLDERMASK:       
   case DFT_AA_RC_NO_METAL:         
   case DFT_AA_RC_NO_SURFACE:       
   case DFT_AA_RC_MISSING_COMP:     
   case DFT_AA_RC_MISSING_COMPPIN:  
   case DFT_AA_RC_SINGLE_PIN_SMD: 
      AddRC(rc);
      break;

   case DFT_AA_RC_SMALL_METAL:
   case DFT_AA_RC_BOARD_OUTLINE:
   case DFT_AA_RC_SMALL_METAL_BEAD_PROBE:
      AddRC(rc, dparam1, dparam2);
      break;

   case DFT_AA_RC_COMP_OUTLINE:
      AddRC(rc,param1);
      break;

   default:
      break;
   }  
}

void CAccessFailureReasonList::Reset()
{ 
   this->empty();
};

void CAccessFailureReasonList::CloneRCList(const CAccessFailureReasonList& other)
{
   if (this != &other)
   {
      this->empty();
      this->CopyRCList(other);
   }
}

void CAccessFailureReasonList::CopyRCList(const CAccessFailureReasonList& other)
{
   if (this != &other)
   {
      POSITION pos = other.GetStartPosition();
      while (pos != NULL)
      {
         CAccessFailureReason *r;
         WORD key;
         other.GetNextAssoc(pos, key, r);
         if (r != NULL)
         {
            this->AddRC(*r);
         }
      }
   }
}

bool CAccessFailureReasonList::ContainsFatalAccessError()
{
   if (this->GetCount() > 0)
   {
      POSITION pos = this->GetStartPosition();
      while (pos != NULL)
      {
         CAccessFailureReason *r;
         WORD key;
         this->GetNextAssoc(pos, key, r);
         if (r != NULL && r->GetReasonCode() != DFT_AA_RC_NONE)
         {
            return true;  // Only reason codes allowed are "None"
         }
      }
   }

   // Did not contain an actual error reason code, "None" is not an error
   return false;
}

 