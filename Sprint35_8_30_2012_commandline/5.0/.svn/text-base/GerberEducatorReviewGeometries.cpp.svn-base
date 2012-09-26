
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GerberEducatorReviewGeometries.h


#include "stdafx.h"
#include "ccdoc.h"
#include "MainFrm.h"
#include "PcbUtil.h"
#include "Outline.h"
#include "PanRef.h"
#include "GenerateCentroidAndOutline.h"
#include "PcbComponentPinAnalyzer.h"
#include "GerberEducatorReviewGeometries.h"
#include "GerberEducatorToolbar.h"
#include ".\gerbereducatorreviewgeometries.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);
int PanReference(CCEtoODBView *view, const char *ref);

#define QBookReadingOrder  "BookReadingOrder"
#define QPinNumberingOrder "PinNumberingOrder"
#define QPinOrderingMethod "PinOrderingMethod"
#define QSkipAlphas        "SkipAlphas"
#define QIPCStatus			"IPCStatus"
#define QOriginMethod		"OriginMethod"
#define QXOffset				"XOffset"
#define QYOffset				"YOffset"
#define QReviewed				"Reviewed"
#define QIncludeMechPin    "IncludeMechPin"
#define QIncludeFidPin     "IncludeFidPin"
#define QBlockInsertType   "BlockInsertType"

CString originMethodTagToString(OriginMethodTag originMethodTag)
{
   CString retval;

	switch (originMethodTag)
	{
	case originMethodPinCenter:			retval = "Pin Center";				break;
	case originMethodPinExtent:			retval = "Pin Extent";				break;
	case originMethodSnapToPad:			retval = "Snap To Pad";				break;
	case originMethodOffsetFromZero:		retval = "Offset From Zero";		break;
	default:										retval = "Undefined";				break;
	}

	return retval;
}

OriginMethodTag stringToOriginMethodTag(CString originMethodString)
{
	OriginMethodTag retval = originMethodUndefined;

	if (originMethodTagToString(originMethodPinCenter).CompareNoCase(originMethodString) == 0)
	{
		retval = originMethodPinCenter;
	}
	else if (originMethodTagToString(originMethodPinExtent).CompareNoCase(originMethodString) == 0)
	{
		retval = originMethodPinExtent;
	}
	else if (originMethodTagToString(originMethodSnapToPad).CompareNoCase(originMethodString) == 0)
	{
		retval = originMethodSnapToPad;
	}
	else if (originMethodTagToString(originMethodOffsetFromZero).CompareNoCase(originMethodString) == 0)
	{
		retval = originMethodOffsetFromZero;
	}

	return retval;
}

OriginMethodTag intToOriginMethodTag(int tagValue)
{
	if (tagValue >= originMethodPinCenter && tagValue <= originMethodOffsetFromZero)
		return (OriginMethodTag)tagValue;
	else
		return originMethodUndefined;
}


//_____________________________________________________________________________
CGeometryReviewStatusAttribute::CGeometryReviewStatusAttribute(CCamCadDatabase& camCadDatabase)
: m_camCadDatabase(camCadDatabase)
{
	resetToDefault();
}

void CGeometryReviewStatusAttribute::resetToDefault()
{
   m_bookReadingOrder = bookReadingTopBottomLeftRight;
   m_pinLabelingMethod = pinLabelingMethodNumeric;
   m_pinOrderingMethod = pinOrderingMethodUndefined;
   m_alphaSkipList = "IOQSU";
	m_ipcStandard = IPCStatusUndefined;
	m_originMethod = originMethodUndefined;
	m_xOffset = 0.0;
	m_yOffset = 0.0;
	m_reviewed = boolUnknown;
   m_includeMechPin = boolTrue;
   m_includeFidPin = boolTrue;
   m_blockInsertType = insertTypeUndefined;
}

void CGeometryReviewStatusAttribute::loadFromAttribute(CAttributes& attributes)
{
   CSupString attributesString;
   int keywordIndex = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeGeomReviewStatus);
   
	resetToDefault();
   if (m_camCadDatabase.getAttributeStringValue(attributesString,attributes,keywordIndex))
   {
      CStringArray attributeStringArray;
      attributesString.Parse(attributeStringArray,"|");

      for (int index = 0;index < attributeStringArray.GetCount();index++)
      {
         CStringArray params;
         CSupString attributesString(attributeStringArray.GetAt(index));
         attributesString.ParseQuote(params,"=");

         if (params.GetCount() > 1)
         {
            if (params[0].CompareNoCase(QBookReadingOrder) == 0)
            {
               m_bookReadingOrder = stringToBookReadingOrderTag(params[1]);
            }
            else if (params[0].CompareNoCase(QPinNumberingOrder) == 0)
            {
               m_pinLabelingMethod = stringToPinLabelingMethodTag(params[1]);
            }
            else if (params[0].CompareNoCase(QPinOrderingMethod) == 0)
            {
               m_pinOrderingMethod = stringToPinOrderingMethodTag(params[1]);
            }
            else if (params[0].CompareNoCase(QSkipAlphas) == 0)
            {
               m_alphaSkipList = params[1];
            }
				else if (params[0].CompareNoCase(QIPCStatus) == 0)
				{
					m_ipcStandard = stringToIpcStatusTag(params[1]);
				}
				else if (params[0].CompareNoCase(QOriginMethod) == 0)
				{
					m_originMethod = stringToOriginMethodTag(params[1]);
				}
				else if (params[0].CompareNoCase(QXOffset) == 0)
				{
					m_xOffset = atof(params[1]);
				}
				else if (params[0].CompareNoCase(QYOffset) == 0)
				{
					m_yOffset = atof(params[1]);
				}
				else if (params[0].CompareNoCase(QReviewed) == 0)
				{
               if (params[1].CompareNoCase("TRUE") == 0)
               {
                  m_reviewed = boolTrue;
               }
               else if (params[1].CompareNoCase("FALSE") == 0)
               {
                  m_reviewed = boolFalse;
               }
               else
               {
                  m_reviewed = boolUnknown;
               }
				}
				else if (params[0].CompareNoCase(QIncludeMechPin) == 0)
				{
               if (params[1].CompareNoCase("TRUE") == 0)
               {
                  m_includeMechPin = boolTrue;
               }
               else if (params[1].CompareNoCase("FALSE") == 0)
               {
                  m_includeMechPin = boolFalse;
               }
               else
               {
                  m_includeMechPin = boolUnknown;
               }
				}
				else if (params[0].CompareNoCase(QIncludeFidPin) == 0)
				{
               if (params[1].CompareNoCase("TRUE") == 0)
               {
                  m_includeFidPin = boolTrue;
               }
               else if (params[1].CompareNoCase("FALSE") == 0)
               {
                  m_includeFidPin = boolFalse;
               }
               else
               {
                  m_includeFidPin = boolUnknown;
               }
				}
				else if (params[0].CompareNoCase(QBlockInsertType) == 0)
            {
               m_blockInsertType = insertDisplayStringToTypeTag(params[1]);
            }
			}
      }
   }
}

void CGeometryReviewStatusAttribute::storeToAttribute(CAttributes& attributes)
{
   CString attributeValue,delim;
   CString pipe("|");

   if (m_bookReadingOrder != bookReadingUndefined)
   {
      attributeValue += delim + QBookReadingOrder + "=" + bookReadingOrderTagToString(m_bookReadingOrder);
      delim = pipe;
   }

   if (m_pinLabelingMethod != pinLabelingMethodUndefined)
   {
      attributeValue += delim + QPinNumberingOrder + "=" + pinLabelingMethodTagToString(m_pinLabelingMethod);
      delim = pipe;
   }

   if (m_pinOrderingMethod != pinOrderingMethodUndefined)
   {
      attributeValue += delim + QPinOrderingMethod + "=" + pinOrderingMethodTagToString(m_pinOrderingMethod);
      delim = pipe;
   }

   if (!m_alphaSkipList.IsEmpty())
   {
      attributeValue += delim + QSkipAlphas + "=" + m_alphaSkipList;
      delim = pipe;
   } 

	if (m_ipcStandard != IPCStatusUndefined)
	{
		attributeValue += delim + QIPCStatus + "=" + ipcStatusTagToString(m_ipcStandard);
		delim = pipe;
	}

	if (m_originMethod != originMethodUndefined)
	{
		attributeValue += delim + QOriginMethod + "=" + originMethodTagToString(m_originMethod);
		delim = pipe;
	}

   if (m_xOffset != 0. || m_yOffset != 0.)
	{
		attributeValue += delim + QXOffset + "=" + fpfmt(m_xOffset,3);
		delim = pipe;

		attributeValue += delim + QYOffset + "=" + fpfmt(m_yOffset,3);
   }

   if (m_reviewed != boolUnknown)
   {
		attributeValue += delim + QReviewed + "=" + (m_reviewed == boolTrue ? "TRUE" : "FALSE");
		delim = pipe;
	}

   if (m_includeMechPin != boolUnknown)
   {
		attributeValue += delim + QIncludeMechPin + "=" + (m_includeMechPin == boolTrue ? "TRUE" : "FALSE");
		delim = pipe;
	}

   if (m_includeFidPin != boolUnknown)
   {
		attributeValue += delim + QIncludeFidPin + "=" + (m_includeFidPin == boolTrue ? "TRUE" : "FALSE");
		delim = pipe;
	}
   if ( m_blockInsertType != insertTypeUndefined)
   {
      attributeValue += delim + QBlockInsertType + "=" + insertTypeToDisplayString(m_blockInsertType);
      delim = pipe;
   }

   int keywordIndex = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeGeomReviewStatus);
   m_camCadDatabase.addAttribute(attributes,keywordIndex,attributeValue);
}

void CGeometryReviewStatusAttribute::set(CPcbComponentPinAnalyzer& pinAnalyzer)
{
   setBookReadingOrder(pinAnalyzer.getBookReadingOrder());
   setPinLabelingMethod(pinAnalyzer.getPinNumberingMethod());
   setPinOrderingMethod(pinAnalyzer.getPinOrderingMethod());
   setAlphaSkipList(pinAnalyzer.getAlphaSkipList());
   setIPCStandard(pinAnalyzer.getIpcStatus());

   InsertTypeTag blockInsertType = insertTypePcbComponent;
   switch (pinAnalyzer.getComponentGeometry()->getBlockType())
   {
   case blockTypeFiducial:
      blockInsertType = insertTypeFiducial;
      break;
   case blockTypeTooling:
      blockInsertType = insertTypeDrillTool;
      break;
   case blockTypeMechComponent:
      blockInsertType = insertTypeMechanicalComponent;
      break;
   case blockTypePadstack:
      blockInsertType = insertTypeVia;
      break;
   }
   setBlockInsertType(blockInsertType);
}


//-----------------------------------------------------------------------------
// CReviewQtySummary
//-----------------------------------------------------------------------------
CReviewQtySummary::CReviewQtySummary()
{
}

CReviewQtySummary::~CReviewQtySummary()
{
}

BlockTypeTag CReviewQtySummary::getBlockType(InsertTypeTag insertType)
{
	BlockTypeTag retval = blockTypeUnknown;
	
	switch(insertType)
	{
	case insertTypePcbComponent:				retval = blockTypePcbComponent;		break;
	case insertTypeMechanicalComponent:		retval = blockTypeMechComponent;		break;
	case insertTypeDrillTool:					retval = blockTypeTooling;				break;
	case insertTypeFiducial:					retval = blockTypeFiducial;			break;
	}

	return retval;
}

void CReviewQtySummary::removeReviewQuantityFrom(BlockTypeTag blockType, int review, int quantity)
{
	switch(blockType)
	{
	case blockTypePcbComponent:
		{
			m_pcbCompReviewQtyLabel.reviewMinus(review);
			m_pcbCompReviewQtyLabel.quantityMinus(quantity);
			m_pcbCompReviewQtyLabel.updateLabel();
		}
		break;
	case blockTypeMechComponent:
		{
			m_mechCompReviewQtyLabel.reviewMinus(review);
			m_mechCompReviewQtyLabel.quantityMinus(quantity);
			m_mechCompReviewQtyLabel.updateLabel();
		}
		break;
	case blockTypeTooling:
		{
			m_toolingReviewQtyLabel.reviewMinus(review);
			m_toolingReviewQtyLabel.quantityMinus(quantity);
			m_toolingReviewQtyLabel.updateLabel();
		}
		break;
	case blockTypeFiducial:
		{
			m_fiducialReviewQtyLabel.reviewMinus(review);
			m_fiducialReviewQtyLabel.quantityMinus(quantity);
			m_fiducialReviewQtyLabel.updateLabel();
		}
      break;
   case blockTypePadstack:
      {
         m_viaReviewQtyLabel.reviewMinus(review);
         m_viaReviewQtyLabel.quantityMinus(quantity);
         m_viaReviewQtyLabel.updateLabel();
      }
		break;
	}
}

void CReviewQtySummary::addReviewQuantityTo(BlockTypeTag blockType, int review, int quantity)
{
	switch(blockType)
	{
	case blockTypePcbComponent:
		{
			m_pcbCompReviewQtyLabel.reviewPlus(review);
			m_pcbCompReviewQtyLabel.quantityPlus(quantity);
			m_pcbCompReviewQtyLabel.updateLabel();
		}
		break;
	case blockTypeMechComponent:
		{
			m_mechCompReviewQtyLabel.reviewPlus(review);
			m_mechCompReviewQtyLabel.quantityPlus(quantity);
			m_mechCompReviewQtyLabel.updateLabel();
		}
		break;
	case blockTypeTooling:
		{
			m_toolingReviewQtyLabel.reviewPlus(review);
			m_toolingReviewQtyLabel.quantityPlus(quantity);
			m_toolingReviewQtyLabel.updateLabel();
		}
		break;
	case blockTypeFiducial:
		{
			m_fiducialReviewQtyLabel.reviewPlus(review);
			m_fiducialReviewQtyLabel.quantityPlus(quantity);
			m_fiducialReviewQtyLabel.updateLabel();
		}
		break;
   case blockTypePadstack:
      {
         m_viaReviewQtyLabel.reviewPlus(review);
         m_viaReviewQtyLabel.quantityPlus(quantity);
         m_viaReviewQtyLabel.updateLabel();
      }
      break;
	}
}

void CReviewQtySummary::removeReviewQuantityFrom(InsertTypeTag insertType, int review, int quantity)
{
	removeReviewQuantityFrom(getBlockType(insertType), review, quantity);
}

void CReviewQtySummary::addReviewQuantityTo(InsertTypeTag insertType, int review, int quantity)
{
	addReviewQuantityTo(getBlockType(insertType), review, quantity);
}

void CReviewQtySummary::clearSummary()
{
   m_pcbCompReviewQtyLabel.reset();
   m_mechCompReviewQtyLabel.reset();
   m_toolingReviewQtyLabel.reset();
   m_fiducialReviewQtyLabel.reset();
   m_viaReviewQtyLabel.reset();
}

void CReviewQtySummary::setBackGroundColor(COLORREF backGroundColor)
{
   m_pcbCompReviewQtyLabel.SetBkColor(backGroundColor);
   m_mechCompReviewQtyLabel.SetBkColor(backGroundColor);
   m_toolingReviewQtyLabel.SetBkColor(backGroundColor);
   m_fiducialReviewQtyLabel.SetBkColor(backGroundColor);
   m_viaReviewQtyLabel.SetBkColor(backGroundColor);
}


//-----------------------------------------------------------------------------
// CGEReviewGeometriesDlg dialog
//-----------------------------------------------------------------------------
CGEReviewGeometriesDlg::CGEReviewGeometriesDlg(CGerberEducatorUi& gerberEducatorUi)
	: CResizingDialog(CGEReviewGeometriesDlg::IDD) 
	, m_gerberEducatorUi(gerberEducatorUi)
	, m_camCadDoc(gerberEducatorUi.getCamCadDoc())
	, m_camCadDatabase(gerberEducatorUi.getCamCadDoc())
	, m_fileStruct(*gerberEducatorUi.getFileStruct())
	, m_pinNumberingOption(pinOrderingMethodUndefined)
	, m_rotationOption(-1)
	, m_originOption(originMethodUndefined)
   , m_includeMechPin(FALSE)
   , m_includeFidPin(FALSE)
	, m_cellEditError(cellEditErrorNone)
   , m_toolBar(*this)
{
	m_camCadDatabase.associateExistingLayers();
	m_geomReviewStatusKeyword = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeGeomReviewStatus);
	m_camCadDoc.getKeyWordArray().SetHidden(m_geomReviewStatusKeyword, true);
	m_initialized = true;
	m_pinLabelTopWasOn = false;
	m_pinLabelBottomWasOn = false;

	m_prevXOffset = "0.0";
	m_prevYOffset = "0.0";
	m_pinOneData = NULL;
	m_tolerance = Units_Factor(pageUnitsMils, m_camCadDatabase.getCamCadDoc().getSettings().getPageUnits()) * 0.1;

	addFieldControl(IDC_GeometryGridStatic			, anchorLeft, growBoth);
	addFieldControl(IDC_PinsGridStatic				, anchorBottomLeft, growHorizontal);
	addFieldControl(IDC_PinNumberingGroup			, anchorBottomRight);
	addFieldControl(IDC_ManualRadio					, anchorBottomRight);
	addFieldControl(IDC_ClockwiseRadio				, anchorBottomRight);
	addFieldControl(IDC_CounterClockwiseRadio		, anchorBottomRight);
	addFieldControl(IDC_ReadingOrderRadio			, anchorBottomRight);

	addFieldControl(IDC_TopLeftPin					, anchorBottomRight);
	addFieldControl(IDC_TopRightPin					, anchorBottomRight);
	addFieldControl(IDC_BottomLeftPin				, anchorBottomRight);
	addFieldControl(IDC_BottomRightPin				, anchorBottomRight);
	addFieldControl(IDC_LeftUpArrow					, anchorBottomRight);
	addFieldControl(IDC_LeftDownArrow				, anchorBottomRight);
	addFieldControl(IDC_RighUpArrow					, anchorBottomRight);
	addFieldControl(IDC_RightDownArrow				, anchorBottomRight);
	addFieldControl(IDC_TopLeftArrow					, anchorBottomRight);
	addFieldControl(IDC_TopRightArrow				, anchorBottomRight);
	addFieldControl(IDC_BottomLeftArrow				, anchorBottomRight);
	addFieldControl(IDC_BottomRightArrow			, anchorBottomRight);

	addFieldControl(IDC_OrientationMarkerGroup	, anchorBottomRight);
	addFieldControl(IDC_OrientationTopLeft			, anchorBottomRight);
	addFieldControl(IDC_OrientationTop				, anchorBottomRight);
	addFieldControl(IDC_OrientationTopRight		, anchorBottomRight);
	addFieldControl(IDC_OrientationRight			, anchorBottomRight);
	addFieldControl(IDC_OrientationBottomRight	, anchorBottomRight);
	addFieldControl(IDC_OrientationBottom			, anchorBottomRight);
	addFieldControl(IDC_OrientationBottomleft		, anchorBottomRight);
	addFieldControl(IDC_OrientationLeft				, anchorBottomRight);
	addFieldControl(IDC_ClearOrienationMarker		, anchorBottomRight);

	addFieldControl(IDC_PinNumberingCombo			, anchorBottomRight);
	addFieldControl(IDC_PinNamingStatic				, anchorBottomRight);
	addFieldControl(IDC_SkipAlphasStatic			, anchorBottomRight);
	addFieldControl(IDC_txtSkipAlphas				, anchorBottomRight);
	addFieldControl(IDC_SetPinOne						, anchorBottomRight);

	addFieldControl(IDC_IPCStatusGroup				, anchorBottomRight);
	addFieldControl(IDC_IPCStatus						, anchorBottomRight);
	addFieldControl(IDC_SetIPCStandard				, anchorBottomRight);
	addFieldControl(IDC_Rotate90						, anchorBottomRight);

	addFieldControl(IDC_OriginGroup					, anchorBottomRight);
	addFieldControl(IDC_PadCentersRadio				, anchorBottomRight);
	addFieldControl(IDC_PadExtentsRadio				, anchorBottomRight);
	addFieldControl(IDC_SnapToPadRadio				, anchorBottomRight);
	addFieldControl(IDC_XYOffsetRadio				, anchorBottomRight);
	addFieldControl(IDC_SnapToSelectedPad			, anchorBottomRight);
	addFieldControl(IDC_txtXOffset					, anchorBottomRight);
	addFieldControl(IDC_txtYOffset					, anchorBottomRight);
	addFieldControl(IDC_OriginIncludeGroup			, anchorBottomRight);
	addFieldControl(IDC_includeMechPin				, anchorBottomRight);
	addFieldControl(IDC_includeFidPin       		, anchorBottomRight);

	addFieldControl(IDC_SummaryGroup					, anchorBottomLeft, growHorizontal);
	addFieldControl(IDC_GeomTypeLabel				, anchorBottomLeft);
	addFieldControl(IDC_PcbCompLabel					, anchorBottomLeft);
	addFieldControl(IDC_MechCompLabel				, anchorBottomLeft);
	addFieldControl(IDC_ToolingLabel					, anchorBottomLeft);
	addFieldControl(IDC_FiducialLabel				, anchorBottomLeft);
	addFieldControl(IDC_ViaLabel				      , anchorBottomLeft);

	addFieldControl(IDC_ReviewQtyLabel				, anchorBottomLeft, growHorizontal); 
	addFieldControl(IDC_PcbCompReviewQtyLabel		, anchorBottomLeft, growHorizontal); 
	addFieldControl(IDC_MechCompReviewQtyLabel	, anchorBottomLeft, growHorizontal); 
	addFieldControl(IDC_ToolingReviewQtyLabel		, anchorBottomLeft, growHorizontal); 
	addFieldControl(IDC_FiducialReviewQtyLabel	, anchorBottomLeft, growHorizontal); 
	addFieldControl(IDC_ViaReviewQtyLabel	      , anchorBottomLeft, growHorizontal); 

	addFieldControl(IDC_CLOSE							, anchorBottomRight);
}

CGEReviewGeometriesDlg::~CGEReviewGeometriesDlg()
{
}

void CGEReviewGeometriesDlg::DoDataExchange(CDataExchange* pDX)
{
   if (! pDX->m_bSaveAndValidate)
   {
   }

   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_GeometryGridStatic, m_geometriesGrid);
   DDX_Control(pDX, IDC_PinsGridStatic, m_pinsGrid);
   DDX_Radio(pDX, IDC_ManualRadio, m_pinNumberingOption);
   DDX_Radio(pDX, IDC_PadCentersRadio, m_originOption);
   DDX_Check(pDX, IDC_includeMechPin, m_includeMechPin);
   DDX_Check(pDX, IDC_includeFidPin, m_includeFidPin);
   DDX_Control(pDX, IDC_PinNumberingCombo, m_pinNumberingCombo);
   DDX_Control(pDX, IDC_IPCStatus, m_ipcStatusStatic);

   if (pDX->m_bSaveAndValidate)
   {
      CString ipcStatus;
      m_ipcStatusStatic.GetWindowText(ipcStatus);

      setIpcStatus(stringToIpcStatusTag(ipcStatus));
   }

   DDX_Control(pDX, IDC_GeomTypeLabel, m_geomTypeLabel);
   DDX_Control(pDX, IDC_PcbCompLabel, m_pcbCompLabel);
   DDX_Control(pDX, IDC_MechCompLabel, m_mechCompLabel);
   DDX_Control(pDX, IDC_ToolingLabel, m_toolingLabel);
   DDX_Control(pDX, IDC_FiducialLabel, m_fiducialLabel);
   DDX_Control(pDX, IDC_ViaLabel, m_viaLabel);

   DDX_Control(pDX, IDC_ReviewQtyLabel, m_reviewQtyLabel);
   DDX_Control(pDX, IDC_PcbCompReviewQtyLabel, m_reviewQuantitySummary.getPcbCompReviewQtyLabel());
   DDX_Control(pDX, IDC_MechCompReviewQtyLabel, m_reviewQuantitySummary.getMechCompReviewQtyLabel());
   DDX_Control(pDX, IDC_ToolingReviewQtyLabel, m_reviewQuantitySummary.getToolingReviewQtyLabel());
   DDX_Control(pDX, IDC_FiducialReviewQtyLabel, m_reviewQuantitySummary.getFiducialReviewQtyLabel());
   DDX_Control(pDX, IDC_ViaReviewQtyLabel, m_reviewQuantitySummary.getViaReviewQtyLabel());
}

BEGIN_MESSAGE_MAP(CGEReviewGeometriesDlg, CResizingDialog)
	ON_BN_CLICKED(IDC_ManualRadio, OnPinNumberingClick)
	ON_BN_CLICKED(IDC_ClockwiseRadio, OnPinNumberingClick)
	ON_BN_CLICKED(IDC_CounterClockwiseRadio, OnPinNumberingClick)
	ON_BN_CLICKED(IDC_ReadingOrderRadio, OnPinNumberingClick)
	ON_CBN_SELCHANGE(IDC_PinNumberingCombo, OnCbnSelchangePinNumberingCombo)
	ON_EN_KILLFOCUS(IDC_txtSkipAlphas, OnKillFocusSkipAlphas)
	ON_BN_CLICKED(IDC_SetPinOne, OnBnClickSetPinOne)

	ON_BN_CLICKED(IDC_ClearOrienationMarker, OnBnClickedClearOrienationMarker)
	ON_BN_CLICKED(IDC_SetIPCStandard, OnBnClickedSetIpcStandard)
	ON_BN_CLICKED(IDC_Rotate90, OnBnClickedRotate90)

   ON_BN_CLICKED(IDC_includeFidPin, OnOriginOptionClick)
   ON_BN_CLICKED(IDC_includeMechPin, OnOriginOptionClick)
	ON_BN_CLICKED(IDC_PadCentersRadio, OnOriginOptionClick)
	ON_BN_CLICKED(IDC_PadExtentsRadio, OnOriginOptionClick)
	ON_BN_CLICKED(IDC_SnapToPadRadio, OnOriginOptionClick)
	ON_BN_CLICKED(IDC_XYOffsetRadio, OnOriginOptionClick)
	ON_BN_CLICKED(IDC_SnapToSelectedPad, OnBnClickedSnapToSelectedPad)
	ON_EN_KILLFOCUS(IDC_txtXOffset, OnKillFocusXOffset)
	ON_EN_KILLFOCUS(IDC_txtYOffset, OnKillFocusYOffset)

	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
	ON_WM_CLOSE()
	ON_WM_PAINT()

	ON_BN_CLICKED(IDC_LeftUpArrow, OnBnClickedLeftUpArrow)
	ON_BN_CLICKED(IDC_LeftDownArrow, OnBnClickedLeftDownArrow)
	ON_BN_CLICKED(IDC_RighUpArrow, OnBnClickedRightUpArrow)
	ON_BN_CLICKED(IDC_RightDownArrow, OnBnClickedRightDownArrow)
	ON_BN_CLICKED(IDC_TopLeftArrow, OnBnClickedTopLeftArrow)
	ON_BN_CLICKED(IDC_TopRightArrow, OnBnClickedTopRightArrow)
	ON_BN_CLICKED(IDC_BottomLeftArrow, OnBnClickedBottomLeftArrow)
	ON_BN_CLICKED(IDC_BottomRightArrow, OnBnClickedBottomRightArrow)

	ON_BN_CLICKED(IDC_TopLeftPin, OnBnClickedTopLeftPin)
	ON_BN_CLICKED(IDC_TopRightPin, OnBnClickedTopRightPin)
	ON_BN_CLICKED(IDC_BottomLeftPin, OnBnClickedBottomLeftPin)
	ON_BN_CLICKED(IDC_BottomRightPin, OnBnClickedBottomRightPin)

	ON_BN_CLICKED(IDC_OrientationTopLeft, OnBnClickedOrientationTopLeft)
	ON_BN_CLICKED(IDC_OrientationTop, OnBnClickedOrientationTop)
	ON_BN_CLICKED(IDC_OrientationTopRight, OnBnClickedOrientationTopRight)
	ON_BN_CLICKED(IDC_OrientationRight, OnBnClickedOrientationRight)
	ON_BN_CLICKED(IDC_OrientationBottomRight, OnBnClickedOrientationBottomRight)
	ON_BN_CLICKED(IDC_OrientationBottom, OnBnClickedOrientationBottom)
	ON_BN_CLICKED(IDC_OrientationBottomleft, OnBnClickedOrientationBottomLeft)
	ON_BN_CLICKED(IDC_OrientationLeft, OnBnClickedOrientationLeft)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)

   ON_COMMAND(IDC_ReviewComponentZoomIn, OnBnClickedZoomIn)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomIn, OnUpdateZoomIn)
   ON_COMMAND(IDC_ReviewComponentZoomOut, OnBnClickedZoomOut)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomOut, OnUpdateZoomOut)
   ON_COMMAND(IDC_ReviewComponentZoomWindow, OnBnClickedZoomWindow)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomWindow, OnUpdateZoomWindow)
   ON_COMMAND(IDC_ReviewComponentZoomExtents, OnBnClickedZoomExtents)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomExtents, OnUpdateZoomExtents)
   ON_COMMAND(IDC_ReviewComponentZoomOneToOne, OnBnClickedZoomFull)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomOneToOne, OnUpdateZoomFull)
   ON_COMMAND(IDC_ReviewComponentPanCenter, OnBnClickedPanCenter)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentPanCenter, OnUpdatePanCenter)

   ON_COMMAND(IDC_ReviewComponentQueryItem, OnBnClickedQueryItem)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentQueryItem, OnUpdateQueryItem)
   ON_COMMAND(IDC_ReviewComponentRepaint, OnBnClickedRepaint)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentRepaint, OnUpdateRepaint)

	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGEReviewGeometriesDlg, CResizingDialog)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x1a, KeyDownEditGeometryGrid, VTS_I4 VTS_I4 VTS_PI2 VTS_I2)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x18, AfterEditGeometryGrid, VTS_I4 VTS_I4)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x7, AfterRowColChangeOnGeometryGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x3, EnterCellGeometryGrid, VTS_NONE)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x1c, ChangeEditGeometryGrid, VTS_NONE)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x17, ValidateEditGeometryGrid, VTS_I4 VTS_I4 VTS_BOOL)
	ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x5, BeforeMouseDownGeometryGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4 VTS_BOOL)
   ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0xd, BeforeSortGeometryGrid, VTS_I4 VTS_PI2)
   ON_EVENT(CGEReviewGeometriesDlg, ID_GeometriesGrid, 0x15, BeforeEditGeometryGrid, VTS_I4 VTS_I4  VTS_BOOL)

	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x1a, KeyDownEditPinsGrid, VTS_I4 VTS_I4 VTS_PI2 VTS_I2)
	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x18, AfterEditPinsGrid, VTS_I4 VTS_I4)
	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x7, AfterRowColChangeOnPinsGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x3, EnterCellPinsGrid, VTS_NONE)
	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x1c, ChangeEditPinsGrid, VTS_NONE)
	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x17, ValidateEditPinsGrid, VTS_I4 VTS_I4 VTS_BOOL)
	ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0x5, BeforeMouseDownPinsGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4 VTS_BOOL)
   ON_EVENT(CGEReviewGeometriesDlg, ID_PinsGrid, 0xd, BeforeSortPinGrid, VTS_I4 VTS_PI2)
END_EVENTSINK_MAP()

BOOL CGEReviewGeometriesDlg::OnInitDialog()
{
	// Call OnInitDialog(), then createAndLoad(), then restoreWindowState()
	CResizingDialog::OnInitDialog();
   m_toolBar.createAndLoad(IDR_GerberEducatorReviewComponentToolbar);
	CResizingDialog::restoreWindowState();

	setReviewGeometriesView();

   m_ipcStatusStatic.SetBorder(true);
   m_ipcStatusStatic.SetBkColor(colorBlack);
   m_ipcStatusStatic.SetFontBold(true);

   m_geomTypeLabel.SetBkColor(colorHtmlLightSkyBlue);
   m_pcbCompLabel.SetBkColor(colorWhite);
   m_mechCompLabel.SetBkColor(colorWhite);
   m_toolingLabel.SetBkColor(colorWhite);
	m_fiducialLabel.SetBkColor(colorWhite);
	m_viaLabel.SetBkColor(colorWhite);

   m_reviewQtyLabel.SetBkColor(colorHtmlLightSkyBlue);
	m_reviewQuantitySummary.setBackGroundColor(colorWhite);

   // Create grid for geometries and pins
	m_geometriesGrid.substituteForDlgItem(ID_GeometriesGrid,IDC_GeometryGridStatic,"FlexGrid",*this);
	m_pinsGrid.substituteForDlgItem(ID_PinsGrid,IDC_PinsGridStatic,"FlexGrid",*this);

	// Load pin numbering combo
	m_pinNumberingCombo.Clear();
	m_pinNumberingCombo.AddString(pinLabelingMethodTagToString(pinLabelingMethodAlpha));
	m_pinNumberingCombo.AddString(pinLabelingMethodTagToString(pinLabelingMethodNumeric));
	m_pinNumberingCombo.AddString(pinLabelingMethodTagToString(pinLabelingMethodAlphaNumeric));
	m_pinNumberingCombo.AddString(pinLabelingMethodTagToString(pinLabelingMethodNumericAlpha));
	
	initGrids();
	loadGridSelectionImage();
	loadOrientationImage();
	loadGeomtriesGrid();

	// Turn on pin label
	m_pinLabelTopWasOn = m_camCadDoc.showPinnrsTop==TRUE;
	m_pinLabelBottomWasOn = m_camCadDoc.showPinnrsBottom==TRUE;
	m_camCadDoc.showPinnrsTop = TRUE;
	m_camCadDoc.showPinnrsBottom = TRUE;

	// Set layer selection filter
   CLayerFilter layerFilter(true);
	layerFilter.removeAll();
	LayerStruct* layer = m_camCadDatabase.getLayer(m_camCadDatabase.getCamCadLayerName(ccLayerPadTop));
	if (layer != NULL)
		layerFilter.add(layer->getLayerIndex());
	layer = m_camCadDatabase.getLayer(m_camCadDatabase.getCamCadLayerName(ccLayerPadBottom));
	if (layer != NULL)
		layerFilter.add(layer->getLayerIndex());
	m_camCadDoc.getSelectLayerFilterStack().push(layerFilter);

	m_initialized = true;
	return TRUE;
}

void CGEReviewGeometriesDlg::setReviewGeometriesView()
{
	// Set Top view and Original colors
	m_camCadDatabase.getCamCadDoc().OnTopview();
	m_camCadDatabase.getCamCadDoc().OnOriginalColors();

	// Turn off all layers, except the only necessary layers
	LayerStruct* layer;
	for (int i=0; i<m_camCadDatabase.getNumLayers(); i++)
	{
		layer = m_camCadDatabase.getLayerAt(i);
		if (layer == NULL)
			continue;

      COLORREF layerColor = layer->getColor();
      bool visible = false;

		if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerAssemblyTop)) == 0)
      {
         layerColor = m_layerColor.getColor(gerberEducatorReviewGeometriesComponentOutlineTopColor);
			visible = true;
      }
      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerAssemblyBottom)) == 0)
      {
         layerColor = m_layerColor.getColor(gerberEducatorReviewGeometriesComponentOutlineBottomColor);
			visible = true;
      }
      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidTop)) == 0)
      {
         layerColor = m_layerColor.getColor(gerberEducatorReviewGeometriesCentroidTopColor);
			visible = true;
      }
      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidBottom))	== 0)
      {
         layerColor = m_layerColor.getColor(gerberEducatorReviewGeometriesCentroidBottomColor);
			visible = true;
      }
	   else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerPadTop)) == 0)
      {
         layerColor = m_layerColor.getColor(gerberEducatorReviewGeometriesSmdPadTopColor);
			visible = true;
      }
      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerPadBottom)) == 0)
      {
         layerColor = m_layerColor.getColor(gerberEducatorReviewGeometriesSmdPadBottomColor);
			visible = true;
      }
	   else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerDrillHoles)) == 0)
      {
			visible = true;
      }

      layer->setColor(layerColor);
      layer->setVisible(visible);
   }
}

void CGEReviewGeometriesDlg::initGrids()
{
	// Initialize geometries grid
   getFlexGrid().clear();
   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	getFlexGrid().put_AllowSelection(FALSE);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);
   getFlexGrid().put_Cols(m_colCount);
   getFlexGrid().put_ExplorerBar(CFlexGrid::flexExSort);
   getFlexGrid().put_ExtendLastCol(true);
   getFlexGrid().put_FixedCols(0);
   getFlexGrid().put_FixedRows(1);
	getFlexGrid().put_FocusRect(CFlexGrid::flexFocusInset);
	getFlexGrid().put_HighLight(CFlexGrid::flexHighlightAlways);
   getFlexGrid().put_Rows(1);
	getFlexGrid().put_ScrollBars(CFlexGrid::flexScrollBarBoth);
	getFlexGrid().put_SelectionMode(CFlexGrid::flexSelectionByRow);

	getFlexGrid().put_ColDataType((long)m_colReview, CFlexGrid::flexDTBoolean);
	getFlexGrid().put_ColDataType((long)m_colIncludeMechPin, CFlexGrid::flexDTBoolean);
	getFlexGrid().put_ColDataType((long)m_colIncludeFidPin, CFlexGrid::flexDTBoolean);
   getFlexGrid().setCell(0, m_colReview, CONST_REVIEW);
   getFlexGrid().setCell(0, m_colGeometryName, CONST_GEOMETRYNAME);
   getFlexGrid().setCell(0, m_colGeometryType, CONST_GEOMETRYTYPE);
   getFlexGrid().autoSizeColumns();
	getFlexGrid().put_ColAlignment(m_colReview, CFlexGrid::flexAlignCenterCenter);
	getFlexGrid().put_ColAlignment(m_colGeometryName, CFlexGrid::flexAlignLeftCenter);

	// Hide the following columns because they are only use for storing setting temperary for each geometry
	// The setting will be save to the geometry attrib "GeomReviewStatus" when the dialog is closed
	getFlexGrid().put_ColHidden((long)m_colPinNumOption, TRUE);
	getFlexGrid().put_ColHidden((long)m_colBookReadingOrder, TRUE);
	getFlexGrid().put_ColHidden((long)m_colPinNumberingOrder, TRUE);
	getFlexGrid().put_ColHidden((long)m_colSkipAlphas, TRUE);
	getFlexGrid().put_ColHidden((long)m_colIPCStatus, TRUE);
	getFlexGrid().put_ColHidden((long)m_colOriginOption, TRUE);
	getFlexGrid().put_ColHidden((long)m_colIncludeMechPin, TRUE);
	getFlexGrid().put_ColHidden((long)m_colIncludeFidPin, TRUE);
	getFlexGrid().put_ColHidden((long)m_colXoffset, TRUE);
	getFlexGrid().put_ColHidden((long)m_colYoffset, TRUE);


	// Initialize pins grid
	getPinsFlexGrid().clear();
   getPinsFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	getPinsFlexGrid().put_AllowSelection(TRUE);
   getPinsFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getPinsFlexGrid().put_AutoResize(true);
   getPinsFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);
   getPinsFlexGrid().put_Cols(m_colCountPinGrid);
   getPinsFlexGrid().put_ExplorerBar(CFlexGrid::flexExSort);
   getPinsFlexGrid().put_ExtendLastCol(true);
   getPinsFlexGrid().put_FixedCols(0);
   getPinsFlexGrid().put_FixedRows(1);
	getPinsFlexGrid().put_FocusRect(CFlexGrid::flexFocusInset);
	getPinsFlexGrid().put_HighLight(CFlexGrid::flexHighlightAlways);
   getPinsFlexGrid().put_Rows(1);
	getPinsFlexGrid().put_ScrollBars(CFlexGrid::flexScrollBarBoth);
	getPinsFlexGrid().put_SelectionMode(CFlexGrid::flexSelectionListBox);

   getPinsFlexGrid().setCell(0, m_colPinName, CONST_PINNAME);
   getPinsFlexGrid().setCell(0, m_colPinType, CONST_TYPE);
   getPinsFlexGrid().setCell(0, m_colPinDrill, CONST_PINDRILL);
   getPinsFlexGrid().setCell(0, m_colPinSize, CONST_PINSIZE);
	getPinsFlexGrid().put_ColAlignment(m_colPinName, CFlexGrid::flexAlignLeftCenter);
	getPinsFlexGrid().put_ColAlignment(m_colPinName, CFlexGrid::flexAlignLeftCenter);
   getPinsFlexGrid().autoSizeColumns();
	getPinsFlexGrid().put_ColHidden((long)m_colSortablePinRefDes, TRUE);
}

void CGEReviewGeometriesDlg::loadGeomtriesGrid()
{
	m_reviewQuantitySummary.clearSummary();

   CString technology;
   int technologyKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeTechnology);
	CGeometryReviewStatusAttribute reviewStatusAttribute(m_camCadDatabase);

   for (int i=0; i<m_camCadDatabase.getNumBlocks(); i++)
	{
		BlockStruct* block = m_camCadDatabase.getBlock(i);
		if (block == NULL)			
			continue;

		if (block->getBlockType() != blockTypePcbComponent && block->getBlockType() != blockTypeFiducial &&
			 block->getBlockType() != blockTypeTooling && block->getBlockType() != blockTypeMechComponent && 
          block->getBlockType() != blockTypePadstack)
			continue;

      reviewStatusAttribute.resetToDefault();
		if (block->getAttributes() != NULL)
			reviewStatusAttribute.loadFromAttribute(*block->getAttributes());

      if (block->getBlockType() == blockTypePadstack)
      {
         if (reviewStatusAttribute.getBlockInsertType() != insertTypeVia)
            continue;
         else
            int a =0;
      }

      m_camCadDatabase.getAttributeStringValue(technology, &block->getAttributesRef(), technologyKw);

		CString tmp = "";
		tmp.AppendFormat("%s", pinOrderingMethodTagToString(reviewStatusAttribute.getPinOrderingMethod()));
		tmp.AppendFormat("\t%s", bookReadingOrderTagToString(reviewStatusAttribute.getBookReadingOrder()));
		tmp.AppendFormat("\t%s", pinLabelingMethodTagToString(reviewStatusAttribute.getPinLabelingMethod()));
		tmp.AppendFormat("\t%s", reviewStatusAttribute.getAlphaSkipList());
		tmp.AppendFormat("\t%s", ipcStatusTagToString(reviewStatusAttribute.getIPCStandard()));
		tmp.AppendFormat("\t%s", originMethodTagToString(reviewStatusAttribute.getOriginMethod()));
		tmp.AppendFormat("\t%0.3f", reviewStatusAttribute.getXOffset());
		tmp.AppendFormat("\t%0.3f", reviewStatusAttribute.getYOffset());
		tmp.AppendFormat("\t%d", reviewStatusAttribute.getIncludeMechPin()==boolTrue?1:0);
		tmp.AppendFormat("\t%d", reviewStatusAttribute.getIncludeFinPin()==boolTrue?1:0);
		tmp.AppendFormat("\t%d", reviewStatusAttribute.getReviewed()==boolTrue?1:0);
		tmp.AppendFormat("\t%s", block->getName());
      tmp.AppendFormat("\t%s", block->getBlockType()==blockTypePadstack?"Via":blockTypeToDisplayString(block->getBlockType()));
		getFlexGrid().addItem(tmp, (int)getFlexGrid().get_Rows());

		COleVariant data((long)block->getBlockNumber());
		getFlexGrid().put_RowData(getFlexGrid().get_Rows() - 1, data);

		m_reviewQuantitySummary.addReviewQuantityTo(block->getBlockType(), reviewStatusAttribute.getReviewed()==boolTrue?1:0, 1);
	}

	if (getFlexGrid().get_Rows() > 1)
	{	
		getFlexGrid().autoSizeColumns();

		getFlexGrid().put_ColSort(m_colGeometryName, CFlexGrid::flexSortGenericAscending);
		getFlexGrid().select(0, m_colGeometryName, 0, m_colGeometryName);
		getFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);

		// Find selected row
		long selectedRow = 1;
		if (!m_gerberEducatorUi.getCurrentEditGeometryName().IsEmpty())
		{
			for (int row=1; row<getFlexGrid().get_Rows(); row++)
			{
				CString geometryName = getFlexGrid().getCellText(row, m_colGeometryName);
				if (m_gerberEducatorUi.getCurrentEditGeometryName().CompareNoCase(geometryName) == 0)
				{
					selectedRow = row;
					break;
				}
			}
		}

		getFlexGrid().select(selectedRow, m_colGeometryName, selectedRow, m_colGeometryName);
		getFlexGrid().ShowCell(selectedRow, 0);
	}
	getFlexGrid().SetRedraw(TRUE);
}

void CGEReviewGeometriesDlg::loadPinsGrid()
{
	getPinsFlexGrid().clear();
	getPinsFlexGrid().put_Cols(m_colCountPinGrid);
	getPinsFlexGrid().put_Rows(1);
   getPinsFlexGrid().setCell(0, m_colPinName, CONST_PINNAME);
   getPinsFlexGrid().setCell(0, m_colPinType, CONST_TYPE);
   getPinsFlexGrid().setCell(0, m_colPinDrill, CONST_PINDRILL);
   getPinsFlexGrid().setCell(0, m_colPinSize, CONST_PINSIZE);

	m_pinNameMap.RemoveAll();

	BlockStruct* block = getSelectedGeometry();
	if (block == NULL)
		return;

   int technologyKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeTechnology);
   CString technology;
   m_camCadDatabase.getAttributeStringValue(technology, &block->getAttributesRef(), technologyKw);

   for (POSITION pos=block->getHeadDataInsertPosition(); pos != NULL;)
   {
		POSITION curPos = pos;
		DataStruct* pin = block->getNextDataInsert(pos);
		if (pin == NULL || pin->getDataType() != dataTypeInsert || pin->getInsert() == NULL)
			continue;
		
		InsertStruct* pinInsert = pin->getInsert();
		if (pinInsert->getInsertType() != insertTypePin && pinInsert->getInsertType() != insertTypeMechanicalPin &&
			 pinInsert->getInsertType() != insertTypeFiducial)
			continue;

		BlockStruct* pinPadstack = m_camCadDatabase.getCamCadDoc().getBlockAt(pinInsert->getBlockNumber());
		if (pinPadstack == NULL)
			continue;
		m_camCadDatabase.getCamCadDoc().validateBlockExtents(pinPadstack);
		

      CString drillSizeString = "n/a";
      if (technology.CompareNoCase("THRU") == 0)
      {
		   BlockStruct* drill = GetDrillInGeometry(&m_camCadDatabase.getCamCadDoc(), pinPadstack);
		   if (drill != NULL)
			   drillSizeString.Format("%0.3f", drill->getToolSize());
      }

		COleVariant data((long)curPos);
      CString item = pin->getInsert()->getSortableRefDes();
		item.AppendFormat("\t%s", pin->getInsert()->getRefname());
		item.AppendFormat("\t%s", insertTypeToDisplayString(pin->getInsert()->getInsertType()));
		item.AppendFormat("\t%s", drillSizeString);
		item.AppendFormat("\t%0.3f x %0.3f", pinPadstack->getExtent().getXsize(), pinPadstack->getExtent().getYsize());

		getPinsFlexGrid().addItem(item, (int)getPinsFlexGrid().get_Rows());		
		getPinsFlexGrid().put_RowData(getPinsFlexGrid().get_Rows() - 1, data);

		if (!pin->getInsert()->getRefname().IsEmpty())
			m_pinNameMap.SetAt(pin->getInsert()->getRefname(), pin->getInsert()->getRefname());
	}

	if (getPinsFlexGrid().get_Rows() > 1)
	{
		sortPinsGridByRefdes(true);

		getPinsFlexGrid().put_ColComboList((long)m_colPinType, "Pin|Mechanical Pin|Fiducial");
		getPinsFlexGrid().autoSizeColumns();
		getPinsFlexGrid().select(1, m_colCountPinGrid-1, 1, 0);

		OnClickPinsGrid();
	}
}

void CGEReviewGeometriesDlg::sortPinsGridByRefdes(bool ascending)
{
	if (getPinsFlexGrid().get_Rows() > 1)
	{
		getPinsFlexGrid().put_ColSort(m_colSortablePinRefDes, ascending ? CFlexGrid::flexSortStringAscending : CFlexGrid::flexSortStringDescending);
		getPinsFlexGrid().select(1,m_colSortablePinRefDes,1,m_colSortablePinRefDes);
		getPinsFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);
	}
}

void CGEReviewGeometriesDlg::loadGridSelectionImage()
{
	m_topLeftPin.AutoLoad(IDC_TopLeftPin, this);
	m_topRightPin.AutoLoad(IDC_TopRightPin, this);
	m_bottomLeftPin.AutoLoad(IDC_BottomLeftPin, this);
	m_bottomRightPin.AutoLoad(IDC_BottomRightPin, this);

	m_leftUpArrow.AutoLoad(IDC_LeftUpArrow, this);
	m_leftDownArrow.AutoLoad(IDC_LeftDownArrow, this);
	m_rightUpArrow.AutoLoad(IDC_RighUpArrow, this);
	m_rightDownArrow.AutoLoad(IDC_RightDownArrow, this);
	m_topLeftArrow.AutoLoad(IDC_TopLeftArrow, this);
	m_topRightArrow.AutoLoad(IDC_TopRightArrow, this);
	m_bottomLeftArrow.AutoLoad(IDC_BottomLeftArrow, this);
	m_bottomRightArrow.AutoLoad(IDC_BottomRightArrow, this);
}

void CGEReviewGeometriesDlg::loadOrientationImage()
{
	m_orientationTopLeft.AutoLoad(IDC_OrientationTopLeft, this);
	m_orientationTop.AutoLoad(IDC_OrientationTop, this);
	m_orientationTopRight.AutoLoad(IDC_OrientationTopRight, this);
	m_orientationRight.AutoLoad(IDC_OrientationRight, this);
	m_orientationBottomRight.AutoLoad(IDC_OrientationBottomRight, this);
	m_orientationBottom.AutoLoad(IDC_OrientationBottom, this);
	m_orientationBottomLeft.AutoLoad(IDC_OrientationBottomleft, this);
	m_orientationLeft.AutoLoad(IDC_OrientationLeft, this);
}

void CGEReviewGeometriesDlg::enableControlsByReviewedStatus()
{
	bool enable = !getFlexGrid().getCellChecked(getFlexGrid().get_Row(), m_colReview);

	// Pin number controls
	GetDlgItem(IDC_ManualRadio)->EnableWindow(enable);
	GetDlgItem(IDC_ClockwiseRadio)->EnableWindow(enable);
	GetDlgItem(IDC_CounterClockwiseRadio)->EnableWindow(enable);
	GetDlgItem(IDC_ReadingOrderRadio)->EnableWindow(enable);
	GetDlgItem(IDC_SetPinOne)->EnableWindow((enable && (m_pinNumberingOption == pinOrderingMethodClockwise || m_pinNumberingOption == pinOrderingMethodCounterClockwise)));
	enableReadingOrder(enable && m_pinNumberingOption == pinOrderingMethodReadingOrder);
	
	// Orientation control
	enableOrientationMarker(enable);

	// Rotation controls
	GetDlgItem(IDC_SetIPCStandard)->EnableWindow(enable);
	GetDlgItem(IDC_Rotate90)->EnableWindow(enable);

	// Origin control
	enableOriginGroup(enable);

	// Pins list
	enablePinNameEdit(enable);
}

void CGEReviewGeometriesDlg::enablePinNameEdit(bool enable)
{
	if (getPinsFlexGrid().get_Rows() > 1)
	{
		COleVariant row1((long)1);
		COleVariant row2(getPinsFlexGrid().get_Rows()-1);
		COleVariant col1((long)0);
		COleVariant col2((long)m_colCountPinGrid-1);
		
		if (enable)
		{
			if (m_pinNumberingOption == pinOrderingMethodManual)
			{
				getPinsFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col1, row2, col2, (COleVariant)GRID_COLOR_WHITE);
			}
			else
			{
				getPinsFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col1, row2, col1, (COleVariant)GRID_COLOR_LIGHT_GRAY);
				getPinsFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col2, row2, col2, (COleVariant)GRID_COLOR_WHITE);
			}
		}
		else
		{
			getPinsFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col1, row2, col2, (COleVariant)GRID_COLOR_LIGHT_GRAY);
		}
	}
}

void CGEReviewGeometriesDlg::enableReadingOrder(bool enable)
{
	m_pinNumberingCombo.EnableWindow(enable?TRUE:FALSE);
	GetDlgItem(IDC_PinNamingStatic)->EnableWindow(enable?TRUE:FALSE);
	GetDlgItem(IDC_SkipAlphasStatic)->EnableWindow(enable?TRUE:FALSE);
	GetDlgItem(IDC_txtSkipAlphas)->EnableWindow(enable?TRUE:FALSE);

	if (enable)
	{
		CString bookReadingOrder = getFlexGrid().getCellText(getFlexGrid().get_Row(), m_colBookReadingOrder);
		CString pinNumberOrder = getFlexGrid().getCellText(getFlexGrid().get_Row(), m_colPinNumberingOrder);
		BookReadingOrderTag bookReadingOrderTag = stringToBookReadingOrderTag(bookReadingOrder);
		PinLabelingMethodTag pinNumberingTag = stringToPinLabelingMethodTag(pinNumberOrder);

		setGridSelectionButtons(bookReadingOrderTag, pinNumberingTag);
	}
	else
	{
		resetGridSelectionButtons();
	}
}

void CGEReviewGeometriesDlg::enableOrientationMarker(bool enable)
{
	m_orientationTopLeft.EnableWindow(enable);
	m_orientationTop.EnableWindow(enable);
	m_orientationTopRight.EnableWindow(enable);
	m_orientationRight.EnableWindow(enable);
	m_orientationBottomRight.EnableWindow(enable);
	m_orientationBottom.EnableWindow(enable);
	m_orientationBottomLeft.EnableWindow(enable);
	m_orientationLeft.EnableWindow(enable);

	GetDlgItem(IDC_ClearOrienationMarker)->EnableWindow(enable);

	if (enable)
	{
		setOrientationMarkerButtons(getOrientationFromGeometry());
	}
}

void CGEReviewGeometriesDlg::enableIPCStatusGroup(bool enable)
{
	GetDlgItem(IDC_IPCStatusGroup)->EnableWindow(true);
	m_ipcStatusStatic.EnableWindow(true);
	GetDlgItem(IDC_SetIPCStandard)->EnableWindow(true);

	if (enable)
	{
		CString ipcStatus;
		m_ipcStatusStatic.GetWindowText(ipcStatus);

		if (ipcStatus.CompareNoCase(ipcStatusTagToString(IPCStatusNonStandardPart)) == 0)
			GetDlgItem(IDC_SetIPCStandard)->EnableWindow(false);
		else
			GetDlgItem(IDC_SetIPCStandard)->EnableWindow(true);
	}
}

void CGEReviewGeometriesDlg::enableOriginGroup(bool enable)
{
	GetDlgItem(IDC_PadCentersRadio)->EnableWindow(enable);
	GetDlgItem(IDC_PadExtentsRadio)->EnableWindow(enable);
	GetDlgItem(IDC_SnapToPadRadio)->EnableWindow(enable);
	GetDlgItem(IDC_XYOffsetRadio)->EnableWindow(enable);

	GetDlgItem(IDC_includeMechPin)->EnableWindow((enable && (m_originOption == originMethodPinCenter || m_originOption == originMethodPinExtent)));
	GetDlgItem(IDC_includeFidPin)->EnableWindow((enable && (m_originOption == originMethodPinCenter || m_originOption == originMethodPinExtent)));
	GetDlgItem(IDC_SnapToSelectedPad)->EnableWindow((enable && m_originOption == originMethodSnapToPad));
	GetDlgItem(IDC_txtXOffset)->EnableWindow((enable && m_originOption == originMethodOffsetFromZero));
	GetDlgItem(IDC_txtYOffset)->EnableWindow((enable && m_originOption == originMethodOffsetFromZero));
}

void CGEReviewGeometriesDlg::setGridSelectionButtons(BookReadingOrderTag bookReadingOrderTag, PinLabelingMethodTag pinNumberingTag)
{
	resetGridSelectionButtons();
	setPinAndDirectionButtons(bookReadingOrderTag, pinNumberingTag);
	setVisibleGridSelectionButtons();
}

void CGEReviewGeometriesDlg::resetGridSelectionButtons()
{
	CString bitmapName = "BlankPin";
	m_topLeftPin.LoadBitmaps(bitmapName + _T("U"));
	m_topRightPin.LoadBitmaps(bitmapName + _T("U"));
	m_bottomLeftPin.LoadBitmaps(bitmapName + _T("U"));
	m_bottomRightPin.LoadBitmaps(bitmapName + _T("U"));

	m_topLeftPin.EnableWindow(FALSE);
	m_topRightPin.EnableWindow(FALSE);
	m_bottomLeftPin.EnableWindow(FALSE);
	m_bottomRightPin.EnableWindow(FALSE);

	m_leftUpArrow.EnableWindow(FALSE);
	m_leftDownArrow.EnableWindow(FALSE);
	m_rightUpArrow.EnableWindow(FALSE);
	m_rightDownArrow.EnableWindow(FALSE);
	m_topLeftArrow.EnableWindow(FALSE);
	m_topRightArrow.EnableWindow(FALSE);
	m_bottomLeftArrow.EnableWindow(FALSE);
	m_bottomRightArrow.EnableWindow(FALSE);

	m_topLeftPin.SetState(FALSE);
	m_topRightPin.SetState(FALSE);
	m_bottomLeftPin.SetState(FALSE);
	m_bottomRightPin.SetState(FALSE);

	m_leftUpArrow.SetState(FALSE);
	m_leftDownArrow.SetState(FALSE);
	m_rightUpArrow.SetState(FALSE);
	m_rightDownArrow.SetState(FALSE);
	m_topLeftArrow.SetState(FALSE);
	m_topRightArrow.SetState(FALSE);
	m_bottomLeftArrow.SetState(FALSE);
	m_bottomRightArrow.SetState(FALSE);
}

void CGEReviewGeometriesDlg::setPinAndDirectionButtons(BookReadingOrderTag bookReadingOrder, PinLabelingMethodTag pinNumbering)
{
	// Always call resetGridSelectionButtons() to reset all buttons before call this function

	// Only this function should enable the buttons
	m_topLeftPin.EnableWindow(TRUE);
	m_topRightPin.EnableWindow(TRUE);
	m_bottomLeftPin.EnableWindow(TRUE);
	m_bottomRightPin.EnableWindow(TRUE);

	CString bitmapName;
	CString pinNumber;
	switch (pinNumbering)
	{
	case pinLabelingMethodAlpha:
		bitmapName = "Alpha";
		pinNumber = "A";
		break;
	case pinLabelingMethodNumeric:
		bitmapName = "Numeric";
		pinNumber = "1";
		break;
	case pinLabelingMethodAlphaNumeric:
		bitmapName = "AlphaNumeric";
		pinNumber = "A1";
		break;
	case pinLabelingMethodNumericAlpha:
		bitmapName = "NumericAlpha";
		pinNumber = "1A";
		break;
	default:
		break;
	}

	switch (bookReadingOrder)
	{
	case bookReadingLeftRightTopBottom:
	case bookReadingTopBottomLeftRight:
		{
			m_topLeftPin.LoadBitmaps(bitmapName + _T("U"), bitmapName + _T("D"), bitmapName + _T("F"), bitmapName + _T("X"));
			m_topLeftPin.SetState(TRUE);

			m_topRightArrow.EnableWindow(TRUE);
			m_leftDownArrow.EnableWindow(TRUE);

			if (bookReadingOrder == bookReadingLeftRightTopBottom)
				m_topRightArrow.SetState(TRUE);
			else
				m_leftDownArrow.SetState(TRUE);
		}
		break;
	case bookReadingRightLeftTopBottom:
	case bookReadingTopBottomRightLeft:
		{
			m_topRightPin.LoadBitmaps(bitmapName + _T("U"), bitmapName + _T("D"), bitmapName + _T("F"), bitmapName + _T("X"));
			m_topRightPin.SetState(TRUE);

			m_topLeftArrow.EnableWindow(TRUE);
			m_rightDownArrow.EnableWindow(TRUE);

			if (bookReadingOrder == bookReadingRightLeftTopBottom)
				m_topLeftArrow.SetState(TRUE);
			else
				m_rightDownArrow.SetState(TRUE);
		}
		break;
	case bookReadingLeftRightBottomTop:
	case bookReadingBottomTopLeftRight:
		{
			m_bottomLeftPin.LoadBitmaps(bitmapName + _T("U"), bitmapName + _T("D"), bitmapName + _T("F"), bitmapName + _T("X"));
			m_bottomLeftPin.SetState(TRUE);

			m_bottomRightArrow.EnableWindow(TRUE);
			m_leftUpArrow.EnableWindow(TRUE);

			if (bookReadingOrder == bookReadingLeftRightBottomTop)
				m_bottomRightArrow.SetState(TRUE);
			else
				m_leftUpArrow.SetState(TRUE);
		}
		break;
	case bookReadingRightLeftBottomTop:
	case bookReadingBottomTopRightLeft:
		{
			m_bottomRightPin.LoadBitmaps(bitmapName + _T("U"), bitmapName + _T("D"), bitmapName + _T("F"), bitmapName + _T("X"));
			m_bottomRightPin.SetState(TRUE);

			m_bottomLeftArrow.EnableWindow(TRUE);
			m_rightUpArrow.EnableWindow(TRUE);

			if (bookReadingOrder == bookReadingRightLeftBottomTop)
				m_bottomLeftArrow.SetState(TRUE);
			else
				m_rightUpArrow.SetState(TRUE);
		}
		break;
	default:
		break;
	}
}

void CGEReviewGeometriesDlg::setVisibleGridSelectionButtons()
{
	// Show all buttons at first
	m_leftUpArrow.ShowWindow(TRUE);
	m_leftDownArrow.ShowWindow(TRUE);
	m_rightUpArrow.ShowWindow(TRUE);
	m_rightDownArrow.ShowWindow(TRUE);
	m_topLeftArrow.ShowWindow(TRUE);
	m_topRightArrow.ShowWindow(TRUE);
	m_bottomLeftArrow.ShowWindow(TRUE);
	m_bottomRightArrow.ShowWindow(TRUE);

	// Then hide buttons that is at the same location as the buttons being show
	if (m_topLeftPin.GetState() & m_buttonStatePush)
	{
		m_topLeftArrow.ShowWindow(SW_HIDE);
		m_leftUpArrow.ShowWindow(SW_HIDE);
	}
	else if (m_topRightPin.GetState() & m_buttonStatePush)
	{
		m_topRightArrow.ShowWindow(SW_HIDE);
		m_rightUpArrow.ShowWindow(SW_HIDE);
	}
	else if (m_bottomLeftPin.GetState() & m_buttonStatePush)
	{
		m_bottomLeftArrow.ShowWindow(SW_HIDE);
		m_leftDownArrow.ShowWindow(SW_HIDE);
	}
	else if (m_bottomRightPin.GetState() & m_buttonStatePush)
	{
		m_bottomRightArrow.ShowWindow(SW_HIDE);
		m_rightDownArrow.ShowWindow(SW_HIDE);
	}
}

BookReadingOrderTag CGEReviewGeometriesDlg::getBookReadingOrderFromButtons()
{
	BookReadingOrderTag retval = bookReadingLeftRightTopBottom;

	if (m_topLeftPin.GetState() & m_buttonStatePush)
	{
		if (m_topRightArrow.GetState() & m_buttonStatePush)
			retval = bookReadingLeftRightTopBottom;
		else
			retval = bookReadingTopBottomLeftRight;
	}
	else if (m_topRightPin.GetState() & m_buttonStatePush)
	{
		if (m_topLeftArrow.GetState() & m_buttonStatePush)
			retval = bookReadingRightLeftTopBottom;
		else
			retval = bookReadingTopBottomRightLeft;
	}
	else if (m_bottomLeftPin.GetState() & m_buttonStatePush)
	{
		if (m_bottomRightArrow.GetState() & m_buttonStatePush)
			retval = bookReadingLeftRightBottomTop;
		else
			retval = bookReadingBottomTopLeftRight;
	}
	else if (m_bottomRightPin.GetState() & m_buttonStatePush)
	{
		if (m_bottomLeftArrow.GetState() & m_buttonStatePush)
			retval = bookReadingRightLeftBottomTop;
		else
			retval = bookReadingBottomTopRightLeft;
	}

	return retval;
}

PinLabelingMethodTag CGEReviewGeometriesDlg::getPinNumberingFromComboBox()
{
	CString pinNumberingString;
	m_pinNumberingCombo.GetWindowText(pinNumberingString);

	return stringToPinLabelingMethodTag(pinNumberingString);
}

void CGEReviewGeometriesDlg::setOrientationMarkerButtons(OutlineIndicatorOrientationTag orientationTag)
{
	resetOrientationMarkerButtons();

	switch (orientationTag)
	{
	case outlineIndicatorOrientationTopLeft:
		m_orientationTopLeft.SetState(TRUE);
		break;
	case outlineIndicatorOrientationTop:
		m_orientationTop.SetState(TRUE);
		break;
	case outlineIndicatorOrientationTopRight:
		m_orientationTopRight.SetState(TRUE);
		break;
	case outlineIndicatorOrientationRight:
		m_orientationRight.SetState(TRUE);
		break;
	case outlineIndicatorOrientationBottomRight:
		m_orientationBottomRight.SetState(TRUE);
		break;
	case outlineIndicatorOrientationBottom:
		m_orientationBottom.SetState(TRUE);
		break;
	case outlineIndicatorOrientationBottomLeft:
		m_orientationBottomLeft.SetState(TRUE);
		break;
	case outlineIndicatorOrientationLeft:
		m_orientationLeft.SetState(TRUE);
		break;
	default:
		break;
	}
}

void CGEReviewGeometriesDlg::resetOrientationMarkerButtons()
{
	m_orientationTopLeft.SetState(FALSE);
	m_orientationTop.SetState(FALSE);
	m_orientationTopRight.SetState(FALSE);
	m_orientationRight.SetState(FALSE);
	m_orientationBottomRight.SetState(FALSE);
	m_orientationBottom.SetState(FALSE);
	m_orientationBottomLeft.SetState(FALSE);
	m_orientationLeft.SetState(FALSE);
}

OutlineIndicatorOrientationTag CGEReviewGeometriesDlg::getOrientationFromButtons()
{
	OutlineIndicatorOrientationTag retval = outlineIndicatorOrientationUndefined;
	
	if (m_orientationTopLeft.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationTopLeft;
	}
	else if (m_orientationTop.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationTop;
	}
	else if (m_orientationTopRight.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationTopRight;
	}
	else if (m_orientationRight.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationRight;
	}
	else if (m_orientationBottomRight.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationBottomRight;
	}
	else if (m_orientationBottom.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationBottom;
	}
	else if (m_orientationBottomLeft.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationBottomLeft;
	}
	else if (m_orientationLeft.GetState() & m_buttonStatePush)
	{
		retval = outlineIndicatorOrientationLeft;
	}

	return retval;
}

OutlineIndicatorOrientationTag CGEReviewGeometriesDlg::getOrientationFromGeometry()
{
	BlockStruct* block = getSelectedGeometry();
	OutlineIndicatorOrientationTag orientationTag = outlineIndicatorOrientationUndefined;
	if (block != NULL)
	{
		orientationTag = block->getComponentOutlineIndicatorOrientation(getCamCadData());
	}

	return orientationTag;
}

bool CGEReviewGeometriesDlg::checkDuplicateGeometryName(int editRow, CString newGeometryName)
{
	bool retval = false;

	for (int i=0; i<getFlexGrid().get_Rows(); i++)
	{
		if (i == editRow)
			continue;

		CString existingGeometryName = getFlexGrid().getCellText(i, m_colGeometryName);
		if (existingGeometryName.CompareNoCase(newGeometryName) == 0)
		{
			retval = true;
			break;
		}
	}

	return retval;
}

void CGEReviewGeometriesDlg::undoStandardIPCStatus()
{
	CString ipcStatus;
	m_ipcStatusStatic.GetWindowText(ipcStatus);
	if (ipcStatus.CompareNoCase(ipcStatusTagToString(IPCStatusStandardIPC)) == 0)
	{
		setIpcStatus(IPCStatusNonStandardIPC);
	}	
}

void CGEReviewGeometriesDlg::setIpcStatus(IPCStatusTag ipcStatus)
{
   COLORREF statusColor;

   switch (ipcStatus)
   {
   case IPCStatusNonStandardPart:  statusColor = colorRed;     break;
	case IPCStatusNonStandardIPC:   statusColor = colorYellow;  break;
	case IPCStatusStandardIPC:      statusColor = colorGreen;   break;
	default:                        statusColor = colorWhite;   break;
   }

   m_ipcStatusStatic.SetTextColor(statusColor);
   m_ipcStatusStatic.SetWindowText(ipcStatusTagToString(ipcStatus));
}

BlockStruct* CGEReviewGeometriesDlg::getSelectedGeometry()
{
	if (getFlexGrid().get_Row() > 0)
	{
		int num = COleVariant(getFlexGrid().get_RowData(getFlexGrid().get_Row())).intVal;
		return m_camCadDatabase.getBlock(num);
	}

	return NULL;
}

void CGEReviewGeometriesDlg::setGeometrySelctionInGrid(BlockStruct* block)
{
	if (block == NULL)
		return;

	for (long row=1; row<getFlexGrid().get_Rows(); row++)
	{
		int num = COleVariant(getFlexGrid().get_RowData(row)).lVal;
		BlockStruct* curBlock = m_camCadDatabase.getBlock(num);

		if (curBlock == block)
		{
			getFlexGrid().put_Row(row);
			getFlexGrid().ShowCell(row, 0);
			break;
		}
	}
}

DataStruct* CGEReviewGeometriesDlg::getSelectedPinData()
{
	return getPinDataByRow(getPinsFlexGrid().get_Row());
}

DataStruct* CGEReviewGeometriesDlg::getPinDataByRow(long row)
{
	BlockStruct* block = getSelectedGeometry();
	if (block == NULL)
		return NULL;

	if (row > 0 && row < getPinsFlexGrid().get_Rows())
	{
		long num = COleVariant(getPinsFlexGrid().get_RowData(row)).lVal;
		POSITION pos = (POSITION)num;
		
		return block->getDataList().GetAt(pos);
	}

	return NULL;
}

DataStruct* CGEReviewGeometriesDlg::getFirstSortedPinInGrid(InsertTypeTag insertType)
{
	sortPinsGridByRefdes(TRUE);

	for (long row=1; row<getPinsFlexGrid().get_Rows(); row++)
	{
		DataStruct* data = getPinDataByRow(row);
		if (data != NULL && data->getInsert()->getInsertType() == insertType)
			return data;
	}

	return NULL;
}

InsertStruct* CGEReviewGeometriesDlg::getSelectedPin()
{
	DataStruct* pinData = getSelectedPinData();
	if (pinData != NULL && pinData->getInsert())
		return pinData->getInsert();

	return NULL;
}

InsertStruct* CGEReviewGeometriesDlg::getSelectedPinFromCAMCAD()
{
   if (m_camCadDoc.somethingIsSelected())
   {
		SelectStruct* selected = m_camCadDoc.getSelectStack().getAtLevel();

		if (selected != NULL)
		{
			DataStruct* data = selected->getData();
			if (data != NULL && data->getDataType() == dataTypeInsert)
			{
				InsertStruct* insert = data->getInsert();
				if (insert != NULL && (insert->getInsertType() == insertTypePin ||
											  insert->getInsertType() == insertTypeMechanicalPin ||
											  insert->getInsertType() == insertTypeFiducial))
					return insert;
			}
		}
	}

	return NULL;
}

int CGEReviewGeometriesDlg::getPinCountByType(InsertTypeTag insertType)
{
   int pinCount = 0;
   for (int row = 1; row<getPinsFlexGrid().get_Rows(); row++)
   {
      if (insertDisplayStringToTypeTag(getPinsFlexGrid().getCellText(row, m_colPinType)) == insertType)
         pinCount++;
   }

   return pinCount;
}

CString CGEReviewGeometriesDlg::getUnusedMechanicalPinName()
{
   CString pinName;
   CMapStringToString pinNameMap;
   for (int row = 1; row<getPinsFlexGrid().get_Rows(); row++)
   {
      pinName = getPinsFlexGrid().getCellText(row, m_colPinName);
      pinNameMap.SetAt(pinName, pinName);
   }

   pinName = "MP1";
   int count = 1;
   while (pinNameMap.Lookup(pinName, pinName))
   {
      pinName.Format("MP%d", ++count);
   }

   return pinName;
}

CString CGEReviewGeometriesDlg::getUnusedFiducialName()
{
   CString pinName;
   CMapStringToString pinNameMap;
   for (int row = 1; row<getPinsFlexGrid().get_Rows(); row++)
   {
      pinName = getPinsFlexGrid().getCellText(row, m_colPinName);
      pinNameMap.SetAt(pinName, pinName);
   }

   pinName = "FID1";
   int count = 1;
   while (pinNameMap.Lookup(pinName, pinName))
   {
      pinName.Format("FID%d", ++count);
   }

   return pinName;
}

void CGEReviewGeometriesDlg::updateOptionSettingsToGrid()
{
	if (getFlexGrid().get_Row() > 0)
	{
		UpdateData(TRUE);

		int row = (int)getFlexGrid().get_Row();
		CString pinNumOption, bookReadingOrder, pinNumberOrder, skipAlphas, ipcStatus, originOption;
      CString includeMechPin, includeFidPin, xOffset, yOffset;

		pinNumOption = pinOrderingMethodTagToString(intToPinOrderMethodTag(m_pinNumberingOption));
		bookReadingOrder = bookReadingOrderTagToString(getBookReadingOrderFromButtons());
		originOption = originMethodTagToString(intToOriginMethodTag(m_originOption));

      includeMechPin.Format("%d", m_includeMechPin==TRUE?1:0);
      includeFidPin.Format("%d", m_includeFidPin==TRUE?1:0);

		m_pinNumberingCombo.GetWindowText(pinNumberOrder);
		GetDlgItem(IDC_txtSkipAlphas)->GetWindowText(skipAlphas);
		GetDlgItem(IDC_txtXOffset)->GetWindowText(xOffset);
		GetDlgItem(IDC_txtYOffset)->GetWindowText(yOffset);		
		m_ipcStatusStatic.GetWindowText(ipcStatus);		

		getFlexGrid().setCell(row, m_colPinNumOption, pinNumOption);
		getFlexGrid().setCell(row, m_colBookReadingOrder, bookReadingOrder);
		getFlexGrid().setCell(row, m_colPinNumberingOrder, pinNumberOrder);
		getFlexGrid().setCell(row, m_colSkipAlphas, skipAlphas);
		getFlexGrid().setCell(row, m_colIPCStatus, ipcStatus);
		getFlexGrid().setCell(row, m_colOriginOption, originOption);
      getFlexGrid().setCell(row, m_colIncludeMechPin, includeMechPin);
      getFlexGrid().setCell(row, m_colIncludeFidPin, includeFidPin);
		getFlexGrid().setCell(row, m_colXoffset, xOffset);
		getFlexGrid().setCell(row, m_colYoffset, yOffset);
	}
}

void CGEReviewGeometriesDlg::updateOptionSettingsFromGrid()
{
	if (getFlexGrid().get_Row() > 0)
	{
		int row = (int)getFlexGrid().get_Row();

		m_pinNumberingOption = stringToPinOrderingMethodTag(getFlexGrid().getCellText(row, m_colPinNumOption));
		m_rotationOption = stringToIpcStatusTag(getFlexGrid().getCellText(row, m_colIPCStatus));
		m_originOption =  stringToOriginMethodTag(getFlexGrid().getCellText(row, m_colOriginOption));
      m_includeMechPin = getFlexGrid().getCellChecked(row, m_colIncludeMechPin);
      m_includeFidPin = getFlexGrid().getCellChecked(row, m_colIncludeFidPin);

		CString bookReadingOrder = getFlexGrid().getCellText(row, m_colBookReadingOrder);
		CString pinNumberOrder = getFlexGrid().getCellText(row, m_colPinNumberingOrder);
		BookReadingOrderTag bookReadingOrderTag = stringToBookReadingOrderTag(bookReadingOrder);
		PinLabelingMethodTag pinNumberingTag = stringToPinLabelingMethodTag(pinNumberOrder);
		setGridSelectionButtons(bookReadingOrderTag, pinNumberingTag);
		m_pinNumberingCombo.SelectString(-1, pinNumberOrder);

		CString skipAlphas = getFlexGrid().getCellText(row, m_colSkipAlphas);
		//CString xOffset = getFlexGrid().getCellText(row, m_colXoffset);
		//CString yOffset = getFlexGrid().getCellText(row, m_colYoffset);

      // Dino said to alwasy reset to zero
      CString xOffset = "0.000";
      CString yOffset = "0.000";

		GetDlgItem(IDC_txtSkipAlphas)->SetWindowText(skipAlphas);
		GetDlgItem(IDC_txtXOffset)->SetWindowText(xOffset);
		GetDlgItem(IDC_txtYOffset)->SetWindowText(yOffset);		

		CString ipcStatus = getFlexGrid().getCellText(row, m_colIPCStatus);
      setIpcStatus(stringToIpcStatusTag(ipcStatus));
		//m_ipcStatusStatic.SetWindowText(ipcStatus);
		//m_ipcStatusStatic.SetTextColor(colorWhite);		

		m_prevXOffset = xOffset;
		m_prevYOffset = yOffset;
		m_pinOneData = NULL;

		UpdateData(FALSE);
	}
}

void CGEReviewGeometriesDlg::doPinNumbering()
{
	if (m_pinNumberingOption == pinOrderingMethodManual)
		return;

	BlockStruct* block = getSelectedGeometry();
	if (m_pinNumberingOption == pinOrderingMethodReadingOrder)
	{
		CString skipAlphas;
		GetDlgItem(IDC_txtSkipAlphas)->GetWindowText(skipAlphas);

		BookReadingOrderTag readingOrderTag = getBookReadingOrderFromButtons();
		PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();

		CPcbComponentPinAnalyzer pinBucket(m_camCadDatabase, block->getDataList());
		pinBucket.analysizePinNumbering(readingOrderTag, pinNumberingTag, skipAlphas);

		loadPinsGrid();
		m_camCadDoc.UpdateAllViews(NULL);
	}
	else if (m_pinNumberingOption == pinOrderingMethodClockwise || m_pinNumberingOption == pinOrderingMethodCounterClockwise)
	{
		DataStruct* pinOneData;
		if (m_pinOneData != NULL)
			pinOneData = m_pinOneData;
		else
			pinOneData = getFirstSortedPinInGrid(insertTypePin);

		CPolarCoordinatePinArray polarPinArray(m_camCadDatabase, block->getDataList());
		polarPinArray.analysizePinNumbering(*pinOneData, m_pinNumberingOption == pinOrderingMethodClockwise);

		loadPinsGrid();
		m_camCadDoc.UpdateAllViews(NULL);
		m_pinOneData = NULL;
	}

}

void CGEReviewGeometriesDlg::doGeometryOrigin(CString xOffset, CString yOffset)
{
	BlockStruct* block = getSelectedGeometry();
	if (block == NULL)
		return;

	double originX = 0.0;
	double originY = 0.0;

	if (m_originOption == originMethodPinCenter)
	{
      double xMin, yMin, xMax, yMax;
      CInsertTypeMask includeInsertTypeMask(insertTypePin);

      if (m_includeMechPin == TRUE)
         includeInsertTypeMask.add(insertTypeMechanicalPin);
      if (m_includeFidPin == TRUE)
         includeInsertTypeMask.add(insertTypeFiducial);

      int pins = GetPinCenterExtents(block->getDataList(), includeInsertTypeMask, &xMin, &yMin, &xMax, &yMax);

		if (pins > 0)
		{
			originX = (xMin + xMax)/2.0;
			originY = (yMin + yMax)/2.0;
		}
		else
		{
			ErrorMessage("Cannot set origin by \"Pad Centers\" for selected geometry.", "Origin Location");
		}
	}
	else if (m_originOption == originMethodPinExtent)
	{
      CInsertTypeMask includeInsertTypeMask(insertTypePin);

      if (m_includeMechPin == TRUE)
         includeInsertTypeMask.add(insertTypeMechanicalPin);
      if (m_includeFidPin == TRUE)
         includeInsertTypeMask.add(insertTypeFiducial);

      Outline_Start(&m_camCadDoc);
      int outlineFound = DFT_OutlineAddPins(m_camCadDoc, block->getDataList(), includeInsertTypeMask, 0, 0, 0, 0, 1, 0, -1);
      if (outlineFound > 0)
      { 
         int returnCode;
			double accuracy = get_accuracy(&m_camCadDoc);

         CPntList* pntList = Outline_GetOutline(&returnCode, accuracy);
			if (pntList != NULL)
			{
				CPoint2d point = pntList->getExtent().getCenter();
				originX = point.x;
				originY = point.y;
			}
         else
         {
			   ErrorMessage("Cannot set origin by \"Pad Extents\" for selected geometry.", "Origin Location");
         }
		}
		else
		{
			ErrorMessage("Cannot set origin by \"Pad Extents\" for selected geometry.", "Origin Location");
		}

      Outline_FreeResults();
	}
	else if (m_originOption == originMethodOffsetFromZero)
	{
		if (is_number(xOffset) == 0)
		{
			GetDlgItem(IDC_txtXOffset)->SetFocus();
			return;
		}
		else if (is_number(yOffset) == 0)
		{
			GetDlgItem(IDC_txtYOffset)->SetFocus();
			return;
		}
		originX = atof(xOffset);
		originY = atof(yOffset);
	}
	else
	{
		return;
	}

	CTMatrix matrix;
	matrix.translateCtm(-originX, -originY);
	doTransform(matrix);
}

void CGEReviewGeometriesDlg::doTransform(const CTMatrix& transformationMatrix)
{
	BlockStruct* block = getSelectedGeometry();
	
	if (block != NULL)
	{
		block->transform(transformationMatrix,&(getCamCadData()), true);

		DataStruct* centroid = block->GetCentroidData();
		if (centroid != NULL)
		{
			InsertStruct* insert = centroid->getInsert();
			insert->setAngle(0.0);
			insert->setOrigin(0.0, 0.0);
		}

		updateComponentRotationAndLocation(transformationMatrix);

		m_camCadDoc.OnFitPageToImage();
		m_camCadDoc.UpdateAllViews(NULL);
	}
}

void CGEReviewGeometriesDlg::doOrientation(OutlineIndicatorOrientationTag orientationTag)
{
	BlockStruct* block = getSelectedGeometry();	
	if (block == NULL)
		return;

   block->setComponentOutlineIndicatorOrientation(getCamCadData(), orientationTag);
   block->generateDefaultComponentOutline(getCamCadData());

	updateOptionSettingsToGrid();
	m_camCadDoc.UpdateAllViews(NULL);
}

void CGEReviewGeometriesDlg::updateComponentRotationAndLocation(const CTMatrix& transformationMatrix)
{
	BlockStruct* block = getSelectedGeometry();	
	if (block == NULL)
		return;

	BlockStruct* fileBlock = m_fileStruct.getBlock();
	if (fileBlock == NULL)
		return;

	CTMatrix invertedMatrix(transformationMatrix);
	invertedMatrix.invert();

	for (POSITION pos=fileBlock->getHeadDataInsertPosition(); pos != NULL;)
	{
		DataStruct* data = fileBlock->getNextDataInsert(pos);
		if (data == NULL || data->getInsert() == NULL)
			return;

		InsertStruct* insert = data->getInsert();
		if (insert->getBlockNumber() != block->getBlockNumber())
			continue;
		
		int mirrorFlag = insert->getMirrorFlags();

		CTMatrix newInsertMatrix = invertedMatrix * insert->getBasesVector().getTransformationMatrix();
		CBasesVector basesVector;
		basesVector.transform(newInsertMatrix);
		insert->setBasesVector(basesVector);
		insert->setMirrorFlags(mirrorFlag);

		if (data->getAttributes() == NULL)
		{
			WORD key;
			Attrib* attrib;
			for (POSITION attribPos = data->getAttributes()->GetStartPosition(); attribPos != NULL;)
			{
				data->getAttributes()->GetNextAssoc(attribPos, key, attrib);
				attrib->transform(transformationMatrix);
			}
		}
	}
}

void CGEReviewGeometriesDlg::updateComppins(BlockStruct* componentBlock, CString oldPinName, CString newPinName)
{
	if (componentBlock == NULL)
		return;

	BlockStruct* fileBlock = m_fileStruct.getBlock();
	if (fileBlock == NULL)
		return;

	for (CDataListIterator dataList(*fileBlock, insertTypePcbComponent); dataList.hasNext();)
	{
		DataStruct* data = dataList.getNext();
		if (data == NULL)
			continue;

		InsertStruct* insert = data->getInsert();
		if (insert->getBlockNumber() != componentBlock->getBlockNumber())
			continue;

		// udpate comppin
		CompPinStruct* comppinOfOldName = m_camCadDatabase.getPin(&m_fileStruct, insert->getRefname(), oldPinName);
		CompPinStruct* comppinOfNewName = m_camCadDatabase.getPin(&m_fileStruct, insert->getRefname(), newPinName);

		if (comppinOfOldName != NULL)
			comppinOfOldName->setPinName(newPinName);

		if (comppinOfNewName != NULL)
			comppinOfNewName->setPinName(oldPinName);
	}
}

void CGEReviewGeometriesDlg::updateNetlist()
{
	//FreeNetList(&m_fileStruct);
   m_fileStruct.getNetList().empty();

	BlockStruct* fileBlock = m_fileStruct.getBlock();
	if (fileBlock == NULL)
		return;

   NetStruct* net = m_camCadDatabase.getDefinedNet(NET_UNUSED_PINS, &m_fileStruct);
	for (CDataListIterator dataList(*fileBlock, insertTypePcbComponent); dataList.hasNext();)
	{
		DataStruct* componentData = dataList.getNext();
		BlockStruct* componentBlock = m_camCadDatabase.getBlock(componentData->getInsert()->getBlockNumber());
		if (componentBlock == NULL)
			continue;

		CString refDes = componentData->getInsert()->getRefname();
		for (CDataListIterator pinIterator(*componentBlock,insertTypePin); pinIterator.hasNext();)
		{
			DataStruct* pinData = pinIterator.getNext();
			CString pinName = pinData->getInsert()->getRefname();
			CompPinStruct* comppin = m_camCadDatabase.addCompPin(net,refDes,pinName);
		}
	}

	m_camCadDatabase.getCamCadDoc().generatePinLocations(false);
}

void CGEReviewGeometriesDlg::removeComppins(BlockStruct* componentBlock, CString pinName)
{
	if (componentBlock == NULL)
		return;

	BlockStruct* fileBlock = m_fileStruct.getBlock();
	if (fileBlock == NULL)
		return;

	for (CDataListIterator dataList(*fileBlock, insertTypePcbComponent); dataList.hasNext();)
	{
		DataStruct* data = dataList.getNext();
		if (data == NULL)
			continue;

		InsertStruct* insert = data->getInsert();
		if (insert->getBlockNumber() != componentBlock->getBlockNumber())
			continue;

      m_camCadDatabase.deletePin(&m_fileStruct, insert->getRefname(), pinName);
   }
}

void CGEReviewGeometriesDlg::addComppin(BlockStruct* componentBlock, CString pinName)
{
	if (componentBlock == NULL)
		return;

	BlockStruct* fileBlock = m_fileStruct.getBlock();
	if (fileBlock == NULL)
		return;

   NetStruct* net = m_camCadDatabase.getNet(NET_UNUSED_PINS, &m_fileStruct);

	for (CDataListIterator dataList(*fileBlock, insertTypePcbComponent); dataList.hasNext();)
	{
		DataStruct* data = dataList.getNext();
		if (data == NULL)
			continue;

		InsertStruct* insert = data->getInsert();
		if (insert->getBlockNumber() != componentBlock->getBlockNumber())
			continue;

      m_camCadDatabase.addCompPin(net, insert->getRefname(), pinName);
   }
}

void CGEReviewGeometriesDlg::updateSelectedEntity()
{
	InsertStruct* pinInsert = getSelectedPinFromCAMCAD();
	setPinSelectionInGrid(pinInsert);
}

void CGEReviewGeometriesDlg::setPinSelectionInGrid(InsertStruct* pinInsert)
{
	getPinsFlexGrid().clearSelection();

	if (pinInsert == NULL)
		return;

	BlockStruct* block = getSelectedGeometry();
	if (block == NULL)
		return;

	for (long row=1; row<getPinsFlexGrid().get_Rows(); row++)
	{
		long num = COleVariant(getPinsFlexGrid().get_RowData(row)).lVal;
		POSITION pos = (POSITION)num;
		
		DataStruct* pinData = block->getDataList().GetAt(pos);
		if (pinData != NULL && pinData->getInsert() == pinInsert)
		{
			getPinsFlexGrid().put_Row(row);
			getPinsFlexGrid().ShowCell(row, 0);
			break;
		}
	}
}

void CGEReviewGeometriesDlg::OnClickGeometryGrid()
{
	BlockStruct* block = getSelectedGeometry();
	if (block != NULL)
	{
		EditGeometry(&m_camCadDoc, block);
		setOrientationMarkerButtons(block->getComponentOutlineIndicatorOrientation(getCamCadData()));
	}
	updateOptionSettingsFromGrid();
	loadPinsGrid();

	// Must be last call so the controls are enable or disable properly
	enableControlsByReviewedStatus();

	UpdateData(FALSE);
}

void CGEReviewGeometriesDlg::KeyDownEditGeometryGrid(long Row, long Col, short * KeyCode, short Shift)
{
	if (!m_initialized)
		return;

	int tmp = *KeyCode;
	if (tmp == 13 && getFlexGrid().get_Col() == m_colGeometryName)
	{
		if (Row > 0 && Row < getFlexGrid().get_Rows())
		{
			if (getFlexGrid().get_Row() == getFlexGrid().get_Rows() -1)
				getFlexGrid().put_Row(1);
			else
				getFlexGrid().put_Row(getFlexGrid().get_Row() + 1);
			
			if (!getFlexGrid().get_RowIsVisible(Row))
				getFlexGrid().ShowCell(Row, Col);
		}
	}
}

void CGEReviewGeometriesDlg::AfterEditGeometryGrid(long Row, long Col)
{
	if (Col == m_colReview)
	{
		enableControlsByReviewedStatus();

		BlockStruct* block = getSelectedGeometry();
		if (block != NULL)
		{
			bool review = getFlexGrid().getCellChecked((int)Row, (int)Col);
			if (review)
				m_reviewQuantitySummary.addReviewQuantityTo(block->getBlockType(), 1, 0);
			else 
				m_reviewQuantitySummary.removeReviewQuantityFrom(block->getBlockType(), 1, 0);
		}
	}
}

void CGEReviewGeometriesDlg::AfterRowColChangeOnGeometryGrid(long OldRow, long OldCol, long NewRow, long NewCol)
{
	if (OldCol == m_colGeometryName)
	{
		if (m_cellEditError != cellEditErrorNone)
		{
			m_cellEditError = cellEditErrorNone;
			if (OldRow > 0 && OldRow < getFlexGrid().get_Rows())
			{
				getFlexGrid().put_Row(OldRow);
				getFlexGrid().put_Col(OldCol);
				
				if (!getFlexGrid().get_RowIsVisible(OldRow))
					getFlexGrid().ShowCell(OldRow, OldCol);
				
				return;
			}
		}
	}

	EnterCellGeometryGrid();
	if (OldRow != NewRow)
		OnClickGeometryGrid();
}

void CGEReviewGeometriesDlg::EnterCellGeometryGrid()
{
	bool reviewed = getFlexGrid().getCellChecked(getFlexGrid().get_Row(), m_colReview);

	if (reviewed && getFlexGrid().get_MouseCol() != m_colReview && getFlexGrid().get_Col() != m_colReview)
	{
		getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	}
	else
	{
		getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
		
		if (getFlexGrid().get_Col() == m_colGeometryName)
		{
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText(getFlexGrid().get_Row(), m_colGeometryName).GetLength());
		}
	}
}

void CGEReviewGeometriesDlg::ChangeEditGeometryGrid()
{
	if (getFlexGrid().get_Col() == m_colGeometryType)
	{
		BlockStruct* block = getSelectedGeometry();
		if (block != NULL)
		{
			BlockTypeTag oldBlockType = block->getBlockType();
			BlockTypeTag newBlockType = blockTypeUnknown;
			InsertTypeTag insertType = insertTypeUnknown;

			CString blockType = getFlexGrid().get_EditText();
			if (blockType.CompareNoCase("Fiducial") == 0)
			{
				newBlockType = blockTypeFiducial;
				insertType = insertTypeFiducial;
			}
			else if (blockType.CompareNoCase("Tooling Hole") == 0)
			{
				newBlockType = blockTypeTooling;
				insertType = insertTypeDrillTool;
			}
			else if (blockType.CompareNoCase("Mechanical Component") == 0)
			{
				newBlockType = blockTypeMechComponent;
				insertType = insertTypeMechanicalComponent;
			}
         else if (blockType.CompareNoCase("Via") == 0)
         {
            newBlockType = blockTypePadstack;
            insertType = insertTypeVia;

            //block->getDataList().RemoveDataByInsertType(&m_camCadDatabase.getCamCadDoc(), insertTypeCentroid);
            m_camCadDatabase.getCamCadDoc().removeDataFromDataListByInsertType(*block,insertTypeCentroid);

            //block->getDataList().RemoveDataByGraphicClass(&m_camCadDatabase.getCamCadDoc(), graphicClassComponentOutline);
            m_camCadDatabase.getCamCadDoc().removeDataFromDataListByGraphicClass(*block,graphicClassComponentOutline);

            m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
         }
			else
			{
				newBlockType = blockTypePcbComponent;
				insertType = insertTypePcbComponent;
			}

			block->setBlockType(newBlockType);
			bool review = getFlexGrid().getCellChecked((int)getFlexGrid().get_Row(), m_colReview);

			m_reviewQuantitySummary.removeReviewQuantityFrom(oldBlockType, review?1:0, 1);
			m_reviewQuantitySummary.addReviewQuantityTo(newBlockType, review?1:0, 1);

			for (CDataListIterator dataList(*m_fileStruct.getBlock(), dataTypeInsert); dataList.hasNext();)
			{
				DataStruct* data = dataList.getNext();
				if (data == NULL || data->getInsert() == NULL || data->getInsert()->getBlockNumber() != block->getBlockNumber())
					continue;

				data->getInsert()->setInsertType(insertType);
			}
		}
	}
}

void CGEReviewGeometriesDlg::ValidateEditGeometryGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
	m_cellEditError = cellEditErrorNone;
	if (!m_initialized)
		return;

	if (Row > 0 && Col == m_colGeometryName)
	{
		CString curGeometryName = getFlexGrid().getCellText((int)Row, (int)Col);
		CString newGeometryName = getFlexGrid().get_EditText();
		newGeometryName.Trim();

		if (newGeometryName.IsEmpty())
		{
			ErrorMessage("Geometry name cannot be blank.  Please enter another name.", "Geometry Name");
			getFlexGrid().put_EditText(curGeometryName);
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText(Row, m_colGeometryName).GetLength());

			m_cellEditError = cellEditErrorEmpty;
			*Cancel = VARIANT_TRUE;
		}
		else if (checkDuplicateGeometryName(Row, newGeometryName))
		{
			ErrorMessage("The entered geometry name is already used.  Please enter another name.", "Geometry Name");
			getFlexGrid().put_EditText(curGeometryName);
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText(Row, m_colGeometryName).GetLength());

			m_cellEditError = cellEditErrorDuplicate;
			*Cancel = VARIANT_TRUE;
		}
		else
		{
			BlockStruct* block = getSelectedGeometry();
			if(block != NULL)
				block->setName(newGeometryName);
		}
	}
}

void CGEReviewGeometriesDlg::BeforeEditGeometryGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
   if (Col == m_colGeometryType)
   {
      BlockStruct* block = getSelectedGeometry();
      if (block == NULL)
      {
		   getFlexGrid().put_ComboList(""); 
      }
      else if (block->getPinCount() > 1)
      {
	      getFlexGrid().put_ComboList("PCB Component|Fiducial|Tooling Hole|Mechanical Component");      
      }
      else
      {
         CString drillSize = getPinsFlexGrid().getCellText(1, m_colPinDrill);
         if (drillSize.CompareNoCase("n/a") != 0)
         {
	         getFlexGrid().put_ComboList("PCB Component|Fiducial|Tooling Hole|Mechanical Component|Via");      
         }
         else
         {
	         getFlexGrid().put_ComboList("PCB Component|Fiducial|Tooling Hole|Mechanical Component");      
         }
      }
   }
	else
	{
		getFlexGrid().put_ComboList("");
	}
}

void CGEReviewGeometriesDlg::BeforeMouseDownGeometryGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel)
{
	m_geometriesGrid.restoreWindowPlacement();
}

void CGEReviewGeometriesDlg::BeforeSortGeometryGrid(long col, short* order)
{
	BlockStruct* selectedBlock = getSelectedGeometry();

	getFlexGrid().put_ColSort(col, (long)(*order));
	getFlexGrid().select(0, col, 0, col);
	getFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);

	setGeometrySelctionInGrid(selectedBlock);
}

void CGEReviewGeometriesDlg::OnClickPinsGrid()
{
	InsertStruct* pin = getSelectedPin();
	if (pin == NULL)
		return;

   double xmin, xmax, ymin, ymax;
   if (!ComponentsExtents(&m_camCadDoc, pin->getRefname(), &xmin, &xmax, &ymin, &ymax, TRUE))
   {
      PanReference(getActiveView(), pin->getRefname());
      ErrorMessage("Pin has no extents", "Panning");
      return;
   }

	m_camCadDoc.UpdateAllViews(NULL);
}

void CGEReviewGeometriesDlg::KeyDownEditPinsGrid(long Row, long Col, short * KeyCode, short Shift)
{
	int tmp = *KeyCode;
	if (tmp == 13 && (getPinsFlexGrid().get_Col() == m_colPinName || getPinsFlexGrid().get_Col() == m_colPinDrill))
	{
		if (Row > 0 && Row < getPinsFlexGrid().get_Rows())
		{
			if (getPinsFlexGrid().get_Row() == getPinsFlexGrid().get_Rows() -1)
				getPinsFlexGrid().put_Row(1);
			else
				getPinsFlexGrid().put_Row(getPinsFlexGrid().get_Row() + 1);

			if (!getPinsFlexGrid().get_RowIsVisible(Row))
				getPinsFlexGrid().ShowCell(Row, Col);
		}
	}
}

void CGEReviewGeometriesDlg::AfterRowColChangeOnPinsGrid(long OldRow, long OldCol, long NewRow, long NewCol)
{
	EnterCellPinsGrid();
	if (OldRow != NewRow)
		OnClickPinsGrid();
}

void CGEReviewGeometriesDlg::EnterCellPinsGrid()
{
	if (getFlexGrid().getCellChecked(getFlexGrid().get_Row(), m_colReview))
	{
		getPinsFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	}
	else if (getPinsFlexGrid().get_MouseCol() == m_colPinName && m_pinNumberingOption != pinOrderingMethodManual)
	{
		getPinsFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	}
	else if (getPinsFlexGrid().get_MouseCol() == m_colPinSize || getPinsFlexGrid().get_Col() == m_colPinSize)
	{
		getPinsFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	}
   else if (getPinsFlexGrid().get_MouseCol() == m_colPinDrill)
   {
      CString drillSize = getPinsFlexGrid().getCellText(getPinsFlexGrid().get_Row(), m_colPinDrill);
      if (drillSize.CompareNoCase("n/a") != 0)
      {
		   getPinsFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
			getPinsFlexGrid().EditCell();
			getPinsFlexGrid().put_EditSelStart(0);
			getPinsFlexGrid().put_EditSelLength(getPinsFlexGrid().getCellText(getPinsFlexGrid().get_Row(), 1).GetLength());
      }
      else
      {
		   getPinsFlexGrid().put_Editable(CFlexGrid::flexEDNone);
      }
   }
	else
	{
		if (getPinsFlexGrid().get_MouseCol() == m_colPinName)
		{
			getPinsFlexGrid().EditCell();
			getPinsFlexGrid().put_EditSelStart(0);
			getPinsFlexGrid().put_EditSelLength(getPinsFlexGrid().getCellText(getPinsFlexGrid().get_Row(), 1).GetLength());
		}

		getPinsFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	}
}

void CGEReviewGeometriesDlg::ChangeEditPinsGrid()
{
	if (getPinsFlexGrid().get_Col() != m_colPinType)
		return;

	BlockStruct* componentBlock = getSelectedGeometry();	
	InsertStruct* pin = getSelectedPin();
	if (pin != NULL)
	{
		InsertTypeTag insertType = insertDisplayStringToTypeTag(getPinsFlexGrid().get_EditText());
		pin->setInsertType(insertType);

      // Update the name for mechanical pin and fiducial
		if (insertType == insertTypeMechanicalPin && pin->getRefname().Left(2) != "MP")
		{
         CString newPinName = getUnusedMechanicalPinName();
			pin->setRefname(newPinName);
			getPinsFlexGrid().setCell(getPinsFlexGrid().get_Row(), m_colPinName, newPinName);
		}
		else if (insertType == insertTypeFiducial && pin->getRefname().Left(3) != "FID")
		{
         CString newPinName = getUnusedFiducialName();
			pin->setRefname(newPinName);
			getPinsFlexGrid().setCell(getPinsFlexGrid().get_Row(), m_colPinName, newPinName);
		} 

      // Update pin numbering
		if (m_pinNumberingOption != pinOrderingMethodManual)
		{
			doPinNumbering();
		}
		else
		{
			m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
		}
	}
}

void CGEReviewGeometriesDlg::ValidateEditPinsGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
	if (Row > 0 && Col == m_colPinName)
	{
		CString curPinName = getPinsFlexGrid().getCellText((int)Row, (int)Col);
		CString newPinName = getPinsFlexGrid().get_EditText();
		if (newPinName.CompareNoCase(curPinName) == 0)
			return;

		DataStruct* pinData = getSelectedPinData();
		if (pinData == NULL)
			return;

		if(pinData->getInsert() != NULL)
		{
			// Swap the row with the newPinName with the curPinName if found
			CString tmp;
			if (m_pinNameMap.Lookup(newPinName, tmp))
			{
				for (long row=1; row<getPinsFlexGrid().get_Rows(); row++)
				{
					CString pinName = getPinsFlexGrid().getCellText(row, m_colPinName);
					if (pinName.CompareNoCase(newPinName) != 0)
						continue;

					getPinsFlexGrid().setCell(row, m_colPinName, curPinName);
					
					DataStruct* existingPinData = getPinDataByRow(row);
					if (existingPinData != NULL)
					{
						existingPinData->getInsert()->setRefname(curPinName);
					}

					break;
				}
			}
			else
			{
				// Remove curPinName from map and add newPinName to map
				m_pinNameMap.RemoveKey(curPinName);
				m_pinNameMap.SetAt(newPinName, newPinName);
			}

			// Set row with curPinName to newPinName
			pinData->getInsert()->setRefname(newPinName);

			m_camCadDoc.UpdateAllViews(NULL);
		}
		else
		{
			ErrorMessage("Cannot find selected pin", "Error");
			*Cancel = TRUE;
		}
	}
	else if (Row > 0 && Col == m_colPinDrill)
	{
		CString curPinDrill = getPinsFlexGrid().getCellText((int)Row, (int)Col);
		CString newPinDrill = getPinsFlexGrid().get_EditText();

		if (is_number(newPinDrill))
		{
			double curDrillSize = atof(curPinDrill);
			double newDrillSize = atof(newPinDrill);
		
			if (fpnear(curDrillSize, newDrillSize, m_tolerance))
				return;

			newPinDrill.Format("%0.3f", newDrillSize);
			if (getPinsFlexGrid().get_SelectedRows() > 0)
			{
				for (int i=0; i < getPinsFlexGrid().get_SelectedRows(); i++)
				{
					long selectedRow = getPinsFlexGrid().get_SelectedRow(i);
					DataStruct* pinData = getPinDataByRow(selectedRow);
					updateDrill(pinData, newDrillSize);

					getPinsFlexGrid().setCell((int)selectedRow, (int)m_colPinDrill, newPinDrill);
				}
			}
			else
			{
				DataStruct* pinData = getSelectedPinData();
				updateDrill(pinData, newDrillSize);

				getPinsFlexGrid().setCell((int)Row, (int)m_colPinDrill, newPinDrill);
			}
		}
		else
		{
			ErrorMessage("Drill size must be a number.  Please enter another drill size.", "Drill");
			getPinsFlexGrid().put_EditText(curPinDrill);
			getPinsFlexGrid().EditCell();
			getPinsFlexGrid().put_EditSelStart(0);
			getPinsFlexGrid().put_EditSelLength(getPinsFlexGrid().getCellText(Row, m_colPinDrill).GetLength());

			*Cancel = VARIANT_TRUE;
		}
	}
}

void CGEReviewGeometriesDlg::updateDrill(DataStruct* pinData, double drillSize) 
{
	if (pinData == NULL || pinData->getInsert() == NULL || pinData->getInsert()->getInsertType() != insertTypePin)
		return;

	BlockStruct* pinPadstack = m_camCadDatabase.getCamCadDoc().getBlockAt(pinData->getInsert()->getBlockNumber());
	if (pinPadstack == NULL)
		return;

	CString newPinPadstackName;
	newPinPadstackName.Format("%s_drill%0.3f", pinPadstack->getName(), drillSize);
	BlockStruct* newPinPadstack = m_camCadDatabase.getBlock(newPinPadstackName, pinPadstack->getFileNumber());
	if (newPinPadstack == NULL)
	{
		newPinPadstack = m_camCadDatabase.copyBlock(newPinPadstackName, pinPadstack);
		InsertStruct* drillInsert = GetDrillInsertInGeometry(&m_camCadDatabase.getCamCadDoc(), newPinPadstack);
		BlockStruct* newDrill = m_camCadDatabase.getDefinedTool(drillSize);

		if (drillInsert != NULL)
		{
			drillInsert->setBlockNumber(newDrill->getBlockNumber());
		}
		else
		{
			int layerIndex = m_camCadDatabase.getCamCadLayerIndex(ccLayerDrillHoles);
			DataStruct* drillData = m_camCadDatabase.insertBlock(newDrill, insertTypeDrillHole, "", layerIndex, 0.0, 0.0, 0.0);
			newPinPadstack->getDataList().AddTail(drillData);
		}
	}

	pinData->getInsert()->setBlockNumber(newPinPadstack->getBlockNumber());
}

void CGEReviewGeometriesDlg::AfterEditPinsGrid(long Row, long Col)
{
	if (Col == m_colPinDrill)
	{
		double drillSize = atof(getPinsFlexGrid().getCellText((int)Row, (int)Col));
		CString drillSizeString;
		drillSizeString.Format("%0.3f", drillSize);
		getPinsFlexGrid().setCell((int)Row, (int)Col, drillSizeString);
	}
}

void CGEReviewGeometriesDlg::BeforeMouseDownPinsGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel)
{
	m_pinsGrid.restoreWindowPlacement();
}

void CGEReviewGeometriesDlg::BeforeSortPinGrid(long col, short* order)
{
	InsertStruct* selectPinInsert = getSelectedPin();		

   if (col == m_colPinName)
   {	
		getPinsFlexGrid().put_ColSort(m_colSortablePinRefDes, (long)(*order));
		getPinsFlexGrid().select(0,m_colSortablePinRefDes,0,m_colSortablePinRefDes);
		getPinsFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);
   }
	else
	{
		getPinsFlexGrid().put_ColSort(col, (long)(*order));
		getPinsFlexGrid().select(0,col,0,col);
		getPinsFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);
	}

	setPinSelectionInGrid(selectPinInsert);
}

void CGEReviewGeometriesDlg::OnBnClickedZoomIn()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMIN);
   }
}

void CGEReviewGeometriesDlg::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedZoomOut()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMOUT);
   }
}

void CGEReviewGeometriesDlg::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedZoomWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMWINDOW);
   }
}

void CGEReviewGeometriesDlg::OnUpdateZoomWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedZoomExtents()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMEXTENTS);
   }
}

void CGEReviewGeometriesDlg::OnUpdateZoomExtents(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedZoomFull()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOM11);
   }
}

void CGEReviewGeometriesDlg::OnUpdateZoomFull(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedPanCenter()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_PAN);
   }
}

void CGEReviewGeometriesDlg::OnUpdatePanCenter(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedQueryItem()
{
   m_camCadDatabase.getCamCadDoc().OnEditEntity();
}

void CGEReviewGeometriesDlg::OnUpdateQueryItem(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnBnClickedRepaint()
{
   m_camCadDatabase.getCamCadDoc().OnRedraw();
}

void CGEReviewGeometriesDlg::OnUpdateRepaint(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewGeometriesDlg::OnPinNumberingClick()
{
	int previousOption = m_pinNumberingOption;
	UpdateData(TRUE);

	if (previousOption == m_pinNumberingOption)
		return;

	// Undo IPS Status since pin numbering has changed
	undoStandardIPCStatus();

	GetDlgItem(IDC_SetPinOne)->EnableWindow(m_pinNumberingOption == pinOrderingMethodClockwise || m_pinNumberingOption == pinOrderingMethodCounterClockwise);
	enablePinNameEdit(m_pinNumberingOption == pinOrderingMethodManual);
	enableReadingOrder(m_pinNumberingOption == pinOrderingMethodReadingOrder);

	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnCbnSelchangeReadingOrderCombo()
{
	BookReadingOrderTag readingOrderTag = getBookReadingOrderFromButtons();
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();

	setGridSelectionButtons(readingOrderTag, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnCbnSelchangePinNumberingCombo()
{
	BookReadingOrderTag readingOrderTag = getBookReadingOrderFromButtons();
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();

	setGridSelectionButtons(readingOrderTag, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnKillFocusSkipAlphas()
{
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickSetPinOne()
{
	m_pinOneData = getSelectedPinData();
	doPinNumbering();
}

void CGEReviewGeometriesDlg::OnBnClickedClearOrienationMarker()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationUndefined)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationUndefined);
		doOrientation(outlineIndicatorOrientationUndefined);
	}
}

void CGEReviewGeometriesDlg::OnBnClickedSetIpcStandard()
{
	BlockStruct* block = getSelectedGeometry();
	if (block != NULL)
	{
		InsertStruct* centroid = (block->GetCentroidData()!=NULL)?block->GetCentroidData()->getInsert():NULL;
		InsertStruct* pin = (getFirstSortedPinInGrid(insertTypePin)!=NULL)?getFirstSortedPinInGrid(insertTypePin)->getInsert():NULL;
		if (pin == NULL)
			return;

		double x = pin->getOriginX();
		double y = pin->getOriginY();
		if (centroid != NULL)
		{
			x -= centroid->getOriginX();
			y -= centroid->getOriginY();
		}

		int orientationIndex = getOrientationFromGeometry();
		int rotation = 0;
		double oneMils = Units_Factor(pageUnitsMils, m_camCadDoc.getSettings().getPageUnits()) * 0.1;
		if (block->getPinCount() == 2)
		{
			if (x < oneMils && y <= -oneMils)
			{
				// pin in Quadrant III
				rotation = 270;
				if (orientationIndex != outlineIndicatorOrientationUndefined)
					orientationIndex = orientationIndex + 6;
			}
			else if (x >= oneMils && y < oneMils)
			{
				// pin in Quadrant IV
				rotation = 180;
				if (orientationIndex != outlineIndicatorOrientationUndefined)
					orientationIndex = orientationIndex + 4;
			}
			else if (x > -oneMils && y >= oneMils)
			{
				// pin in Quadrant I
				rotation = 90;
				if (orientationIndex != outlineIndicatorOrientationUndefined)
					orientationIndex = orientationIndex + 2;
			}
		}
		else
		{
			if (x < -oneMils && y <= oneMils)
			{
				// pin in Quadrant III
				rotation = 270;
				if (orientationIndex != outlineIndicatorOrientationUndefined)
					orientationIndex = orientationIndex + 6;
			}
			else if (x >= -oneMils && y < -oneMils)
			{
				// pin in Quadrant IV
				rotation = 180;
				if (orientationIndex != outlineIndicatorOrientationUndefined)
					orientationIndex = orientationIndex + 4;
			}
			else if (x > oneMils && y >= -oneMils)
			{
				// pin in Quadrant I
				rotation = 90;
				if (orientationIndex != outlineIndicatorOrientationUndefined)
					orientationIndex = orientationIndex + 2;
			}
		}

		while (orientationIndex > outlineIndicatorOrientationUpperBound)
      {
			orientationIndex -= outlineIndicatorOrientationUpperBound + 1;
      }

		CTMatrix matrix;
		matrix.rotateDegrees(rotation);
		doTransform(matrix);

		block->setComponentOutlineIndicatorOrientation(getCamCadData(), (OutlineIndicatorOrientationTag)orientationIndex);
		setOrientationMarkerButtons((OutlineIndicatorOrientationTag)orientationIndex);

      setIpcStatus(IPCStatusStandardIPC);
		updateOptionSettingsToGrid();
	}
}

void CGEReviewGeometriesDlg::OnBnClickedRotate90()
{
	BlockStruct* block = getSelectedGeometry();
	if (block != NULL)
	{
		// Undo IPS Status since rotation has changed
		undoStandardIPCStatus();

		CTMatrix matrix;
		matrix.rotateDegrees(90);
		doTransform(matrix);

		OutlineIndicatorOrientationTag orientationTag = getOrientationFromGeometry();
		if (orientationTag != outlineIndicatorOrientationUndefined)
		{
			if (orientationTag == outlineIndicatorOrientationBottom)
				orientationTag = outlineIndicatorOrientationRight;
			else if (orientationTag == outlineIndicatorOrientationBottomRight)
				orientationTag = outlineIndicatorOrientationTopRight;
			else
				orientationTag = (OutlineIndicatorOrientationTag)(orientationTag + 2);
		}

		block->setComponentOutlineIndicatorOrientation(getCamCadData(), orientationTag);
		setOrientationMarkerButtons(orientationTag);
		updateOptionSettingsToGrid();
	}
}

void CGEReviewGeometriesDlg::OnOriginOptionClick()
{
	int previousOption = m_originOption;
	UpdateData(TRUE);

	if (previousOption == m_originOption && m_originOption == originMethodOffsetFromZero)
		return;

	CString xOffset = "0.000";
	CString yOffset = "0.000";
	if (m_originOption == originMethodOffsetFromZero)
	{
		GetDlgItem(IDC_txtXOffset)->GetWindowText(xOffset);
		GetDlgItem(IDC_txtYOffset)->GetWindowText(yOffset);
	}
   else
   {
		GetDlgItem(IDC_txtXOffset)->SetWindowText(xOffset);
		GetDlgItem(IDC_txtYOffset)->SetWindowText(yOffset);
   }

	enableOriginGroup(true);
	doGeometryOrigin(xOffset, yOffset);
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedSnapToSelectedPad()
{
	InsertStruct* selectPin = getSelectedPinFromCAMCAD();
	if (selectPin == NULL)
		return;

	CTMatrix matrix;
	matrix.translateCtm(-selectPin->getOriginX(), -selectPin->getOriginY());
	doTransform(matrix);
}

void CGEReviewGeometriesDlg::OnKillFocusXOffset()
{
	CString xOffset;
	GetDlgItem(IDC_txtXOffset)->GetWindowText(xOffset);

	if (m_prevXOffset.CompareNoCase(xOffset) == 0)
		return;

	m_prevXOffset = xOffset;
	doGeometryOrigin(xOffset, "0.0");
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnKillFocusYOffset()
{
	CString yOffset;
	GetDlgItem(IDC_txtYOffset)->GetWindowText(yOffset);

	if (m_prevYOffset.CompareNoCase(yOffset) == 0)
		return;

	m_prevYOffset = yOffset;
	doGeometryOrigin("0.0", yOffset);
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedApply()
{
	GetDlgItem(IDC_APPLY)->SetFocus();
}

void CGEReviewGeometriesDlg::OnBnClickedClose()
{
	OnClose();
}

void CGEReviewGeometriesDlg::OnClose()
{
	CResizingDialog::OnClose();

	updateReviewedStatuToGeometries();
	updateNetlist();
	m_camCadDoc.OnDoneEditing();

	m_layerColor.setDefaultViewColor(m_camCadDatabase, m_fileStruct);

	// Turn off pin label
	m_camCadDoc.showPinnrsTop = m_pinLabelTopWasOn?TRUE:FALSE;
	m_camCadDoc.showPinnrsBottom = m_pinLabelBottomWasOn?TRUE:FALSE;
	m_camCadDoc.getSelectLayerFilterStack().pop();
	m_gerberEducatorUi.generateEvent(ID_GerberEducatorCommand_TerminateSession);

   EndDialog(IDCANCEL);
}

void CGEReviewGeometriesDlg::updateReviewedStatuToGeometries()
{
	CGeometryReviewStatusAttribute reviewStatusAttribute(m_camCadDatabase);

	for (int row=1; row<(int)getFlexGrid().get_Rows(); row++)
	{
		int num = COleVariant(getFlexGrid().get_RowData((long)row)).intVal;
		BlockStruct* block = m_camCadDatabase.getBlock(num);
		if (block == NULL)
			continue;

		if (block->getAttributes() == NULL)
			block->getAttributesRef() = new CAttributes();

		reviewStatusAttribute.setPinOrderingMethod(stringToPinOrderingMethodTag(getFlexGrid().getCellText(row, m_colPinNumOption)));
		reviewStatusAttribute.setBookReadingOrder(stringToBookReadingOrderTag(getFlexGrid().getCellText(row, m_colBookReadingOrder)));
		reviewStatusAttribute.setPinLabelingMethod(stringToPinLabelingMethodTag(getFlexGrid().getCellText(row, m_colPinNumberingOrder)));
		reviewStatusAttribute.setAlphaSkipList(getFlexGrid().getCellText(row, m_colSkipAlphas));
		reviewStatusAttribute.setIPCStandard(stringToIpcStatusTag(getFlexGrid().getCellText(row, m_colIPCStatus)));
		reviewStatusAttribute.setOriginMethod(stringToOriginMethodTag(getFlexGrid().getCellText(row, m_colOriginOption)));
      reviewStatusAttribute.setIncludeMechPin(getFlexGrid().getCellChecked(row, m_colIncludeMechPin)?boolTrue:boolFalse);
      reviewStatusAttribute.setIncludeFidPin(getFlexGrid().getCellChecked(row, m_colIncludeFidPin)?boolTrue:boolFalse);
		reviewStatusAttribute.setXOffset(atof(getFlexGrid().getCellText(row, m_colXoffset)));
		reviewStatusAttribute.setYOffset(atof(getFlexGrid().getCellText(row, m_colYoffset)));
		reviewStatusAttribute.setReviewed(getFlexGrid().getCellChecked(row, m_colReview) ? boolTrue : boolFalse);

		InsertTypeTag blockInsertType = insertTypeUndefined;
		CString blockType = getFlexGrid().getCellText(row, m_colGeometryType);
		if (blockType.CompareNoCase("Fiducial") == 0)
		{
			blockInsertType = insertTypeFiducial;
		}
		else if (blockType.CompareNoCase("Tooling Hole") == 0)
		{
			blockInsertType = insertTypeDrillTool;
		}
		else if (blockType.CompareNoCase("Mechanical Component") == 0)
		{
			blockInsertType = insertTypeMechanicalComponent;
		}
      else if (blockType.CompareNoCase("Via") == 0)
      {
         blockInsertType = insertTypeVia;
      }
		else
		{
			blockInsertType = insertTypePcbComponent;
		}
      reviewStatusAttribute.setBlockInsertType(blockInsertType);

		reviewStatusAttribute.storeToAttribute(*block->getAttributes());
	}
}

void CGEReviewGeometriesDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	UpdateData();
}

void CGEReviewGeometriesDlg::OnBnClickedLeftUpArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingBottomTopLeftRight, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedLeftDownArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingTopBottomLeftRight, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedRightUpArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingBottomTopRightLeft, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedRightDownArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingTopBottomRightLeft, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedTopLeftArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingRightLeftTopBottom, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedTopRightArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingLeftRightTopBottom, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedBottomLeftArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingRightLeftBottomTop, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedBottomRightArrow()
{
	PinLabelingMethodTag pinNumberingTag = getPinNumberingFromComboBox();
	setGridSelectionButtons(bookReadingLeftRightBottomTop, pinNumberingTag);
	doPinNumbering();
	updateOptionSettingsToGrid();
}

void CGEReviewGeometriesDlg::OnBnClickedTopLeftPin()
{
	// Default to bookReadingLeftRightTopBottom, but only need to update buttons if it is not already pressed
	if (!(m_topRightArrow.GetState() & m_buttonStatePush) && !(m_leftDownArrow.GetState() & m_buttonStatePush))
	{
		OnBnClickedTopRightArrow();
	}
	m_topLeftPin.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedTopRightPin()
{
	// Default to bookReadingRightLeftTopBottom, but only need to update buttons if it is not already pressed
	if (!(m_topLeftArrow.GetState()  & m_buttonStatePush) && !(m_rightDownArrow.GetState()  & m_buttonStatePush))
	{
		OnBnClickedTopLeftArrow();
	}
	m_topRightPin.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedBottomLeftPin()
{
	// Default to bookReadingRightLeftBottomTop, but only need to update buttons if it is not already pressed
	if (!(m_bottomRightArrow.GetState()  & m_buttonStatePush) && !(m_leftUpArrow.GetState()  & m_buttonStatePush))
	{
		OnBnClickedBottomRightArrow();
	}
	m_bottomLeftPin.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedBottomRightPin()
{
	// Default to bookReadingLeftRightBottomTop, but only need to update buttons if it is not already pressed
	if (!(m_bottomLeftArrow.GetState()  & m_buttonStatePush) && !(m_rightUpArrow.GetState()  & m_buttonStatePush))
	{
		OnBnClickedBottomLeftArrow();
	}
	m_bottomRightPin.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationTopLeft()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationTopLeft)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationTopLeft);
		doOrientation(outlineIndicatorOrientationTopLeft);
	}
	m_orientationTopLeft.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationTop()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationTop)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationTop);
		doOrientation(outlineIndicatorOrientationTop);
	}
	m_orientationTop.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationTopRight()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationTopRight)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationTopRight);
		doOrientation(outlineIndicatorOrientationTopRight);
	}
	m_orientationTopRight.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationRight()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationRight)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationRight);
		doOrientation(outlineIndicatorOrientationRight);
	}
	m_orientationRight.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationBottomRight()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationBottomRight)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationBottomRight);
		doOrientation(outlineIndicatorOrientationBottomRight);
	}
	m_orientationBottomRight.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationBottom()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationBottom)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationBottom);
		doOrientation(outlineIndicatorOrientationBottom);
	}
	m_orientationBottom.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationBottomLeft()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationBottomLeft)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationBottomLeft);
		doOrientation(outlineIndicatorOrientationBottomLeft);
	}
	m_orientationBottomLeft.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnBnClickedOrientationLeft()
{
	if (getOrientationFromGeometry() != outlineIndicatorOrientationLeft)
	{
		setOrientationMarkerButtons(outlineIndicatorOrientationLeft);
		doOrientation(outlineIndicatorOrientationLeft);
	}
	m_orientationLeft.SetState(TRUE);
}

void CGEReviewGeometriesDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags &= ~SWP_HIDEWINDOW;
	//CResizingDialog::OnWindowPosChanging(lpwndpos);
}